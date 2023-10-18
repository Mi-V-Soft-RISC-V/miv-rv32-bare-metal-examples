/*******************************************************************************
 * Copyright 2014 Microchip FPGA Embedded Systems Solutions.
 *
 * SPDX-License-Identifier: MIT
 *
 * This example project demonstrates packet receive and transmit with the
 * CoreTSE driver.
 *
 * Please refer to the file README.md for further details about this example.
 *
 */

/*-------------------------------------------------------------------------*/

#include "drivers/fpga_ip/CoreTSE/core_tse.h"
#include "drivers/fpga_ip/CoreTSE/coretse_regs.h"
#include "drivers/fpga_ip/CoreUARTapb/core_uart_apb.h"
#include "miv_rv32_hal/miv_rv32_hal.h"
#include <stdio.h>
#include <string.h>

#define BUFFER_EMPTY   0u
#define RELEASE_BUFFER BUFFER_EMPTY

static UART_instance_t g_uart;

static tse_cfg_t g_tse_config;
static tse_instance_t *g_tse = (tse_instance_t *)TSE_DESC;
static uint8_t *g_mac_tx_buffer = (uint8_t *)TSE_TX_BUF;
static uint8_t *g_mac_rx_buffer = (uint8_t *)TSE_RX_BUF;

#if defined(PF_EVAL_KIT)
extern void configure_zl30364(void);
#endif
#if defined(EVEREST_BOARD)
extern void configure_zl30722(void);
#endif

const static uint8_t tx_buffer_content[] =
    {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xc0, 0xb1, 0x3c, 0x60, 0x60, 0x60};

static uint32_t g_ethernet_speed_choice = TSE_ANEG_ALL_SPEEDS;
static uint32_t g_ethernet_filter_choice = TSE_FC_PROMISCOUS_MODE_MASK;

static volatile uint32_t g_mac_tx_buffer_used = RELEASE_BUFFER;
static volatile uint32_t g_mac_rx_buffer_data_valid = RELEASE_BUFFER;

static volatile uint32_t g_packet_received_length = 0;

const static uint8_t mac_address[6] = {0xC0, 0xB1, 0x3C, 0x61, 0x60, 0x60};

/*Unicast, multicast and broadcast address*/
const static uint8_t address_filter_hash[4][6] = {{0x10, 0x10, 0x10, 0x10, 0x10, 0x10},
                                                  {0x43, 0x40, 0x40, 0x40, 0x40, 0x43},
                                                  {0xC0, 0xB1, 0x3C, 0x60, 0x60, 0x60},
                                                  {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF}};

static const uint8_t g_hello_msg[] =
    "\r\n\n\n\n\n\n ***** Mi-V CoreTSE: Transmit and Receive Packet Example Project *****\r\n\n"
    " This example project implements a simple transmit and receive packet application.\r\n\n\n";

static const uint8_t g_link_up_msg[] =
    "\r\n\n----------------------------------------------------------------------"
    "\r\n Ethernet link up:";

static const uint8_t g_link_down_msg[] =
    "----------------------------------------------------------------------"
    "\r\n\n Ethernet link down.\r\n";

static const uint8_t g_reset_msg[] = "\r\nApplying changes and re-configuring CoreTSE_AHB.\r\n";

static const uint8_t g_show_menu_msg[] =
    "\r\n Press any key to show the link configuration menu.\r\n\n";

static const uint8_t g_instructions_msg[] =
    "\r\n----------------------------------------------------------------------\r\n\
Press a key to select:\r\n\n\
 Options to choose PHY interface parameters:\r\n\
  [1]: Enable auto-negotiation to all speeds\r\n\
  [2]: Select 1000Mpbs, Full-duplex \r\n\
  [3]: Select 100Mpbs, Full-duplex \r\n\
  [4]: Select 100Mpbs, Half-duplex \r\n\
  [5]: Select 10Mpbs, Full-duplex \r\n\
  [6]: Select 10Mpbs, Half-duplex \r\n\
 Options to choose Frame Filtering Combination:\r\n\
  [a]: Receive all Multicast, Broadcast and Perfect-Unicast Match frames\r\n\
  [b]: Enable Promiscuous mode\r\n\
  [c]: Enable Broadcast Frames only, reject all other frames\r\n\
  [d]: Receive Broadcast,Unicast, Hash-Unicast and Hash-Multicast match frames\r\n\n\
  [S]: Display Link status \r\n\n";

void
print(uint8_t *string)
{
    UART_polled_tx_string(&g_uart, string);
}

void
External_IRQHandler(void)
{
    TSE_isr(g_tse);
}

static void
packet_tx_complete_handler(void *caller_info)
{
    /*
     * caller_info points to g_mac_tx_buffer_used. Signal that content of
     * g_mac_tx_buffer has been sent by the MAC by resetting
     * g_mac_tx_buffer_used.
     */
    *((uint32_t *)caller_info) = 0;
    print("\tPacket Tx success");
}

static void
mac_rx_callback(uint8_t *p_rx_packet, uint32_t packet_length, void *caller_info)
{
    g_packet_received_length = packet_length;
}

static void
display_link_status(tse_instance_t *this_tse)
{
    uint8_t link_up;
    uint8_t full_duplex;
    tse_speed_t speed;

    link_up = TSE_get_link_status(this_tse, (tse_speed_t *)&speed, (uint8_t *)&full_duplex);
    if (1u == link_up)
    {
        print((uint8_t *)g_link_up_msg);

        switch (speed)
        {
            case TSE_MAC10MBPS:
                print("  10Mbps ");
                break;

            case TSE_MAC100MBPS:
                print("  100Mbps ");
                break;

            case TSE_MAC1000MBPS:
                print("  1000Mbps ");
                break;

            default:
                break;
        }

        if (1u == full_duplex)
        {
            print("Full Duplex\r\n");
        }
        else
        {
            print("Half Duplex\r\n");
        }
    }
    else
    {
        print((uint8_t *)g_link_down_msg);
    }
}

static void
initialise_coretse(void)
{
    TSE_init(g_tse, TSE_BASEADDR, &g_tse_config);

    TSE_set_tx_callback(g_tse, packet_tx_complete_handler);

    TSE_set_rx_callback(g_tse, mac_rx_callback);

    TSE_receive_pkt(g_tse, g_mac_rx_buffer, 0);

    display_link_status(g_tse);
}

int
main(void)
{
    uint16_t packet_length = 0u;
    uint8_t packet_received_count = 0;

    size_t rx_size = 0;
    uint8_t rx_buff[1] = {0};

    /* Disable Interrupts while the CoreTSE is being initialised*/
    HAL_disable_interrupts();

    UART_init(&g_uart, COREUARTAPB0_BASE_ADDR, BAUD_VALUE_115200, (DATA_8_BITS | NO_PARITY));
    print((uint8_t *)g_hello_msg);

#if defined(EVEREST_BOARD)
    configure_zl30722();
#endif
#if defined(PF_EVAL_KIT)
    configure_zl30364();
#endif

    /* Copy the content of the Tx buffer into the buffer memory */
    memcpy(g_mac_rx_buffer, &tx_buffer_content, sizeof(tx_buffer_content));

    TSE_cfg_struct_def_init(&g_tse_config);

#if defined(EVEREST_BOARD)
    g_tse_config.phy_addr = 0;
#endif
#if defined(PF_EVAL_KIT)
    g_tse_config.phy_addr = 28;
#endif
    g_tse_config.speed_duplex_select = TSE_ANEG_1000M_FD;
    g_tse_config.framefilter = TSE_FC_DEFAULT_MASK;
    g_tse_config.framedrop_mask = TSE_DEFVAL_FRAMEDROP_MASK;

    initialise_coretse();
    print((uint8_t *)g_show_menu_msg);

    /* Enable Interrupts */
    HAL_enable_interrupts();
    /* Turn on the external Interrupt */
    MRV_enable_local_irq(MRV32_EXT_IRQn);

    while (1)
    {
        /* Check for input from the user. */
        rx_size = UART_get_rx(&g_uart, rx_buff, sizeof(rx_buff));
        if (rx_size > 0)
        {
            switch (rx_buff[0])
            {
                default:
                    print((uint8_t *)g_instructions_msg);
                    *rx_buff = 0;
                    rx_size = 0;

                    do
                    {
                        rx_size = UART_get_rx(&g_uart, rx_buff, sizeof(rx_buff));
                    } while (0 == rx_size);

                    switch (rx_buff[0])
                    {
                        case 'a':
                        case 'A': /*Receive Perfect Unicast, perfect Multicast and broadcast
                                     frames*/
                            print((uint8_t *)g_reset_msg);
                            g_ethernet_filter_choice = TSE_FC_PASS_BROADCAST_MASK |
                                                       TSE_FC_PASS_MULTICAST_MASK |
                                                       TSE_FC_PASS_UNICAST_MASK;
                            break;

                        case 'b':
                        case 'B': /*Enable Promiscuous mode*/
                            print((uint8_t *)g_reset_msg);
                            g_ethernet_filter_choice = TSE_FC_PROMISCOUS_MODE_MASK;
                            break;

                        case 'c':
                        case 'C': /*Enable Broadcast Frames only,reject all other frames*/
                            print((uint8_t *)g_reset_msg);
                            g_ethernet_filter_choice = TSE_FC_PASS_BROADCAST_MASK;
                            break;

                        case 'd':
                        case 'D': /*Receive Broadcast,unicast Hash-Unicast and Hash-Multicast match
                         frames*/
                            print((uint8_t *)g_reset_msg);
                            TSE_set_address_filter(g_tse, address_filter_hash[0], 4);
                            break;

                        case '1':
                            print((uint8_t *)g_reset_msg);
                            g_ethernet_speed_choice = TSE_ANEG_ALL_SPEEDS;
                            break;

                        case '2':
                            print((uint8_t *)g_reset_msg);
                            g_ethernet_speed_choice = TSE_ANEG_1000M_FD;
                            break;

                        case '3':
                            print((uint8_t *)g_reset_msg);
                            g_ethernet_speed_choice = TSE_ANEG_100M_FD;
                            break;

                        case '4':
                            print((uint8_t *)g_reset_msg);
                            g_ethernet_speed_choice = TSE_ANEG_100M_HD;
                            break;

                        case '5':
                            print((uint8_t *)g_reset_msg);
                            g_ethernet_speed_choice = TSE_ANEG_10M_FD;
                            break;

                        case '6':
                            print((uint8_t *)g_reset_msg);
                            g_ethernet_speed_choice = TSE_ANEG_10M_HD;
                            break;

                        default:
                            display_link_status(g_tse);
                            break;
                    }
                    break;
            }
            rx_size = 0;
            g_tse_config.speed_duplex_select = g_ethernet_speed_choice;
            g_tse_config.framefilter = g_ethernet_filter_choice;
            initialise_coretse();
        }
        if (g_packet_received_length != 0)
        {
            uint8_t rx_size_msg[20] = {0};
            packet_received_count++;

            sprintf(rx_size_msg, "\n\r Rx packet size = %-3d", (int)g_packet_received_length);
            print((uint8_t *)rx_size_msg);

            TSE_send_pkt(g_tse, g_mac_rx_buffer, sizeof(tx_buffer_content), RELEASE_BUFFER);

            g_packet_received_length = 0;

            TSE_receive_pkt(g_tse, g_mac_rx_buffer, 0);
            if (packet_received_count >= 40)
            {
                packet_received_count = 0;

                display_link_status(g_tse);

                print((uint8_t *)g_show_menu_msg);
            }
        }
    }
    return 0u;
}
