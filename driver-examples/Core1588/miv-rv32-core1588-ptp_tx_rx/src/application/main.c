/*******************************************************************************
 * Copyright 2025 Microchip FPGA Embedded Systems Solutions.
 *
 * SPDX-License-Identifier: MIT
 *
 * @file main.c
 * @author Microchip FPGA Embedded Systems Solutions
 * @brief This example project demonstrates PTP packet receive and transmit
 * timestamping with the Core1588 driver. It also demonstrates the RTC
 * functionality of the Core1588.
 *
 */

/*-------------------------------------------------------------------------*/

#include "miv_rv32_hal/miv_rv32_hal.h"
#include "drivers/fpga_ip/CoreTSE/core_tse.h"
#include "drivers/fpga_ip/CoreTSE/coretse_regs.h"
#include "drivers/fpga_ip/CoreUARTapb/core_uart_apb.h"
#include "drivers/fpga_ip/Core1588/core1588.h"
#include "drivers/fpga_ip/Core1588/core1588_regs.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define BUFFER_EMPTY   0u
#define RELEASE_BUFFER BUFFER_EMPTY
static UART_instance_t g_uart;

static tse_cfg_t g_tse_config;
static tse_instance_t *g_tse = (tse_instance_t *)TSE_DESC;
static uint8_t *g_mac_rx_buffer = (uint8_t *)TSE_RX_BUF;

static c1588_cfg_t g_c1588_config;
static c1588_instance_t *g_c1588 = (c1588_instance_t *)CORE1588_DESC;

static uint8_t rtc_sec_mode_count = 0;
static uint8_t rtc_sec_mode_flag = 0;
static uint8_t rtc_sec_mode_tick_flag = 0;
static uint8_t tse_tx_success_flag = 0;

#define TSE_TX_SUCCESS 1u

#if defined(PF_EVAL_KIT)
extern void configure_zl30364(void);
#define EVAL_KIT_TSE_PHY_ADDR 28u
#endif

static const unsigned char tx_demo_sync_eth[60] = {
0x01, 0x1b, 0x19, 0x00, 0x00, 0x00, 0x00, 0x80,  0x63, 0x00, 0x09, 0xba, 0x88, 0xf7, 0x00, 0x02,
0x00, 0x2c, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x80, 0x63, 0xff, 0xff, 0x00,  0x09, 0xba, 0x00, 0x02, 0x00, 0x7b, 0x00, 0x00,
0x00, 0x00, 0x45, 0xb1, 0x11, 0x49, 0x2e, 0x32,  0x42, 0x63, 0x00, 0x00
};
static const unsigned char tx_demo_delay_request_eth[68] = {
0x01, 0x1b, 0x19, 0x00, 0x00, 0x00, 0x00, 0x80,  0x63, 0x00, 0x09, 0xba, 0x88, 0xf7, 0x01, 0x02,
0x00, 0x36, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x80, 0x63, 0xff, 0xff, 0x00,  0x09, 0xba, 0x00, 0x02, 0x00, 0x7b, 0x01, 0x0f,
0x00, 0x00, 0x45, 0xb1, 0x11, 0x4a, 0x28, 0x2c,  0x8b, 0x35, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00
};
static const unsigned char tx_demo_pdelay_request_eth[68] = {
0x01, 0x80, 0xc2, 0x00, 0x00, 0x0e, 0x00, 0x80,  0x63, 0x00, 0x09, 0xba, 0x88, 0xf7, 0x02, 0x02,
0x00, 0x36, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x80, 0x63, 0xff, 0xff, 0x00,  0x09, 0xba, 0x00, 0x02, 0x00, 0x7b, 0x05, 0x0f,
0x00, 0x00, 0x45, 0xb1, 0x11, 0x4a, 0x0a, 0x5b,  0x91, 0x23, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00
};
static const unsigned char tx_demo_pdelay_response_eth[68] = {
0x01, 0x80, 0xc2, 0x00, 0x00, 0x0e, 0x00, 0x80,  0x63, 0x00, 0x09, 0xba, 0x88, 0xf7, 0x03, 0x02,
0x00, 0x36, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x80, 0x63, 0xff, 0xff, 0x00,  0x09, 0xba, 0x00, 0x02, 0x00, 0x7b, 0x05, 0x0f,
0x00, 0x00, 0x45, 0xb1, 0x11, 0x4a, 0x28, 0x2c,  0x8b, 0x35, 0x00, 0x80, 0x63, 0xff, 0xff, 0x00,
0x09, 0xba, 0x00, 0x02
};

static volatile uint32_t g_packet_received_length = 0;

static const uint8_t g_hello_msg[] =
    "\r\n\n\n\n\n\n ***** Mi-V Core1588: RTC and PTP Tx + Rx Example Project *****\r\n\n"
    " This example project implements simple RTC and PTP transmit + receive demos.\r\n\n\n";

static const uint8_t g_link_up_msg[] =
    "\r\n\n----------------------------------------------------------------------"
    "\r\n Ethernet link up:";

static const uint8_t g_link_down_msg[] =
    "----------------------------------------------------------------------"
    "\r\n\n Ethernet link down.\r\n";

static const uint8_t g_reset_msg[] = "\r\n Re-configuring Core1588.\r\n";

static const uint8_t g_receive_waiting_msg[] = "\r\n Waiting for Rx messages.\r\n";

static const uint8_t g_latch_waiting_msg[] = "\r\n Waiting for latch input.\r\n";

static const uint8_t g_trigger_waiting_msg[] = "\r\n Waiting for trigger to trip.\r\n";

static const uint8_t g_show_menu_msg[] =
    "\r\n Press any key to show demo options menu.\r\n\n";

static const uint8_t g_instructions_msg[] =
    "\r\n----------------------------------------------------------------------\r\n\
 Press a key to select:\r\n\n\
 Options to demo RTC functionality:\r\n\
  [a]: Get current RTC timestamp \r\n\
  [b]: Enter RTC Latch listen mode \r\n\
  [c]: Set RTC trigger for 10 seconds \r\n\
  [d]: Enter RTC seconds tick mode \r\n\
 Options to demo PTP functionality:\r\n\
  [1]: Enter PTP receive mode \r\n\
  [2]: Transmit PTP Sync message \r\n\
  [3]: Transmit PTP Sync message with timestamp insertion \r\n\
  [4]: Transmit PTP Delay Request message \r\n\
  [5]: Transmit PTP Peer Delay Request message \r\n\
  [6]: Transmit PTP Peer Delay Response message \r\n\n";

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

uint8_t MSYS_EI0_IRQHandler(void)
{
    core1588_isr(g_c1588);
    if (rtc_sec_mode_flag == 1)
    {
        rtc_sec_mode_count += 1;
        rtc_sec_mode_tick_flag = 1;
    }
    return (EXT_IRQ_KEEP_ENABLED);
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
    tse_tx_success_flag = TSE_TX_SUCCESS;
}

static void
mac_rx_callback(uint8_t *p_rx_packet, uint32_t packet_length, void *caller_info)
{
    g_packet_received_length = packet_length;
}

static uint8_t
display_link_status(tse_instance_t *this_tse)
{
    uint8_t link_up;
    uint8_t full_duplex;
    tse_speed_t speed;

    link_up = TSE_get_link_status(this_tse, (tse_speed_t *)&speed, (uint8_t *)&full_duplex);
    if (link_up)
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

        if (full_duplex == TSE_FULL_DUPLEX)
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
    return link_up;
}

static void
initialise_coretse(void)
{
    uint8_t status = 0;
    while(status == 0) {
        TSE_init(g_tse, TSE_BASEADDR, &g_tse_config);

        TSE_set_tx_callback(g_tse, packet_tx_complete_handler);

        TSE_set_rx_callback(g_tse, mac_rx_callback);

        TSE_receive_pkt(g_tse, g_mac_rx_buffer, 0);

        status = display_link_status(g_tse);
    }
}

int
main(void)
{
    uint16_t packet_length = 0u;
    uint8_t packet_received_count = 0;
    uint8_t trigger_en = 0;

    c1588_timestamp_t ts = {0};
    uint8_t rtc_ts_msg[40] = {0};

    c1588_rtc_event_timestamp_t rtc_ts = {0};
    c1588_ptp_packet_info_t rx_pkt_info = {0};
    c1588_ptp_packet_info_t tx_pkt_info = {0};

    c1588_status_t c1588_status = C1588_SUCCESS;

    size_t rx_size = 0;
    uint8_t rx_buff[1] = {0};

    /* Disable Interrupts while the CoreTSE is being initialised*/
    HAL_disable_interrupts();

    UART_init(&g_uart, COREUARTAPB0_BASE_ADDR, BAUD_VALUE_115200, (DATA_8_BITS | NO_PARITY));
    print((uint8_t *)g_hello_msg);

#if defined(PF_EVAL_KIT)
    configure_zl30364();
#endif

    TSE_cfg_struct_def_init(&g_tse_config);

#if defined(PF_EVAL_KIT)
    g_tse_config.phy_addr = EVAL_KIT_TSE_PHY_ADDR;
#endif
    g_tse_config.speed_duplex_select = TSE_ANEG_1000M_FD;
    g_tse_config.framefilter = TSE_FC_DEFAULT_MASK;
    g_tse_config.framedrop_mask = TSE_DEFVAL_FRAMEDROP_MASK;

    core1588_cfg_struct_def_init(&g_c1588_config);
    g_c1588_config.config_mask = C1588_CORE_ENABLE;
    g_c1588_config.initial_time.secs = 0u;
    g_c1588_config.initial_time.nsecs = 0u;
    g_c1588_config.rtc_freq = CORE1588_PTP_CLK_FREQ;

    initialise_coretse();
    core1588_init(g_c1588, CORE1588_BASE_ADDR);
    core1588_configure(g_c1588, &g_c1588_config);
    core1588_latch_control(g_c1588, C1588_LATCH_0, C1588_ENABLE);

    /* Enable Interrupts */
    HAL_enable_interrupts();
    /* Turn on the external Interrupt */
    MRV_enable_local_irq(MRV32_EXT_IRQn | MRV32_MSYS_EIE0_IRQn);

    print((uint8_t *)g_show_menu_msg);

    while (1)
    {
        /* Check for input from the user. */
        rx_size = UART_get_rx(&g_uart, rx_buff, sizeof(rx_buff));
        if (rx_size > 0)
        {
            switch (rx_buff[0])
            {
                default:
                    core1588_irq_control(g_c1588, C1588_MASK_ALL_IRQ, C1588_DISABLE);
                    print((uint8_t *)g_instructions_msg);
                    *rx_buff = 0;
                    rx_size = 0;
                    rtc_sec_mode_count = 0;
                    rtc_sec_mode_flag = 0;
                    if (trigger_en == 1)
                    {
                        trigger_en = 0;
                        core1588_trigger_control(g_c1588, C1588_TRIGGER_0, C1588_DISABLE);
                    }

                    do
                    {
                        rx_size = UART_get_rx(&g_uart, rx_buff, sizeof(rx_buff));
                    } while (0 == rx_size);

                    switch (rx_buff[0])
                    {
                        case 'a':
                        case 'A': /*Get current RTC timestamp*/
                            core1588_rtc_get_time(g_c1588, &ts);
                            sprintf(rtc_ts_msg, "\n\r Current RTC time = %lu seconds, %lu nanoseconds", (uint32_t)ts.secs, ts.nsecs);
                            print((uint8_t *)rtc_ts_msg);
                            break;

                        case 'b':
                        case 'B': /*Enter RTC Latch listen mode*/
                            print((uint8_t *)g_reset_msg);
                            core1588_irq_control(g_c1588, C1588_LT0_IRQ, C1588_ENABLE);
                            print((uint8_t *)g_latch_waiting_msg);
                            break;

                        case 'c':
                        case 'C': /*Set RTC trigger for 3 seconds*/
                            print((uint8_t *)g_reset_msg);
                            trigger_en = 1;
                            core1588_rtc_get_time(g_c1588, &ts);
                            ts.secs = ts.secs + (uint64_t)3u;
                            core1588_trigger_set_timestamp(g_c1588, &ts, C1588_TRIGGER_0);
                            core1588_trigger_control(g_c1588, C1588_TRIGGER_0, C1588_ENABLE);
                            core1588_irq_control(g_c1588, C1588_TT0_IRQ, C1588_ENABLE);
                            print((uint8_t *)g_trigger_waiting_msg);
                            break;

                        case 'd':
                        case 'D': /*Enter RTC seconds tick mode*/
                            print((uint8_t *)g_reset_msg);
                            core1588_irq_control(g_c1588, C1588_RTCSEC_IRQ, C1588_ENABLE);
                            rtc_sec_mode_flag = 1;
                            break;

                        case '1': /*Enter PTP receive mode*/
                            print((uint8_t *)g_reset_msg);
                            core1588_irq_control(g_c1588, C1588_RX_IRQ_MASK, C1588_ENABLE);
                            print((uint8_t *)g_receive_waiting_msg);
                            break;

                        case '2': /*Transmit PTP Sync message*/
                            print((uint8_t *)g_reset_msg);
                            core1588_irq_control(g_c1588, C1588_TXSYNC_IRQ, C1588_ENABLE);
                            memcpy(g_mac_rx_buffer, &tx_demo_sync_eth, sizeof(tx_demo_sync_eth));
                            TSE_send_pkt(g_tse, g_mac_rx_buffer, sizeof(tx_demo_sync_eth), RELEASE_BUFFER);
                            break;

                        case '3': /*Transmit PTP Sync message with timestamp insertion*/
                            print((uint8_t *)g_reset_msg);
                            g_c1588_config.config_mask = C1588_CORE_ENABLE | C1588_ONE_STEP_SYNC_MODE | C1588_REQUESTOR_MODE;
                            HAL_set_32bit_reg(g_c1588->base_address, CORE1588_REGS_GCFG, g_c1588_config.config_mask);
                            core1588_irq_control(g_c1588, C1588_TXSYNC_IRQ, C1588_ENABLE);
                            memcpy(g_mac_rx_buffer, &tx_demo_sync_eth, sizeof(tx_demo_sync_eth));
                            TSE_send_pkt(g_tse, g_mac_rx_buffer, sizeof(tx_demo_sync_eth), RELEASE_BUFFER);
                            HAL_set_32bit_reg(g_c1588->base_address, CORE1588_REGS_GCFG, g_c1588_config.config_mask);
                            core1588_latch_control(g_c1588, C1588_LATCH_0, C1588_ENABLE);
                            break;

                        case '4': /*Transmit PTP Delay Request message*/
                            print((uint8_t *)g_reset_msg);
                            core1588_irq_control(g_c1588, C1588_TXDELAYREQ_IRQ, C1588_ENABLE);
                            memcpy(g_mac_rx_buffer, &tx_demo_delay_request_eth, sizeof(tx_demo_delay_request_eth));
                            TSE_send_pkt(g_tse, g_mac_rx_buffer, sizeof(tx_demo_delay_request_eth), RELEASE_BUFFER);
                            break;

                        case '5': /*Transmit PTP Peer Delay Request message*/
                            print((uint8_t *)g_reset_msg);
                            core1588_irq_control(g_c1588, C1588_TXPDELAYREQ_IRQ, C1588_ENABLE);
                            memcpy(g_mac_rx_buffer, &tx_demo_pdelay_request_eth, sizeof(tx_demo_pdelay_request_eth));
                            TSE_send_pkt(g_tse, g_mac_rx_buffer, sizeof(tx_demo_pdelay_request_eth), RELEASE_BUFFER);
                            break;

                        case '6': /*Transmit PTP Peer Delay Response message*/
                            print((uint8_t *)g_reset_msg);
                            core1588_irq_control(g_c1588, C1588_TXPDELAYRESP_IRQ, C1588_ENABLE);
                            memcpy(g_mac_rx_buffer, &tx_demo_pdelay_response_eth, sizeof(tx_demo_pdelay_response_eth));
                            TSE_send_pkt(g_tse, g_mac_rx_buffer, sizeof(tx_demo_pdelay_response_eth), RELEASE_BUFFER);
                            break;

                        default:
                            display_link_status(g_tse);
                            break;
                    }
                    break;
            }
            rx_size = 0;
        }

        c1588_status = core1588_latch_get_from_buffer(g_c1588, &rtc_ts);
        if (c1588_status == 0)
        {
            uint8_t latch_msg[80] = {0};
            sprintf(latch_msg, "\n\r Latch triggered at %lu seconds, %lu nanoseconds", (uint32_t)rtc_ts.ts.secs, rtc_ts.ts.nsecs);
            print((uint8_t *)latch_msg);
        }

        c1588_status = core1588_trigger_get_from_buffer(g_c1588, &rtc_ts);
        if (c1588_status == 0)
        {
            uint8_t trigger_msg[80] = {0};
            sprintf(trigger_msg, "\n\r Trigger tripped at %lu seconds, %lu nanoseconds", (uint32_t)rtc_ts.ts.secs, rtc_ts.ts.nsecs);
            print((uint8_t *)trigger_msg);
        }

        c1588_status = core1588_ptp_rx_get_from_buffer(g_c1588, &rx_pkt_info, 123);
        if (c1588_status == 0)
        {
            uint8_t pkt_info_msg[80] = {0};
            switch(rx_pkt_info.type)
            {
                case C1588_RXSYNC:
                    sprintf(pkt_info_msg, "\n\r Sync message received at %lu seconds, %lu nanoseconds", (uint32_t)rx_pkt_info.ts.secs, rx_pkt_info.ts.nsecs);
                    break;
                case C1588_RXDELAYREQ:
                    sprintf(pkt_info_msg, "\n\r Delay request message received at %lu seconds, %lu nanoseconds", (uint32_t)rx_pkt_info.ts.secs, rx_pkt_info.ts.nsecs);
                    break;
                case C1588_RXPDELAYREQ:
                    sprintf(pkt_info_msg, "\n\r Peer delay request message received at %lu seconds, %lu nanoseconds", (uint32_t)rx_pkt_info.ts.secs, rx_pkt_info.ts.nsecs);
                    break;
                case C1588_RXPDELAYRESP:
                    sprintf(pkt_info_msg, "\n\r Peer delay response message received at %lu seconds, %lu nanoseconds", (uint32_t)rx_pkt_info.ts.secs, rx_pkt_info.ts.nsecs);
                    break;
            }
            print((uint8_t *)pkt_info_msg);
        }

        c1588_status = core1588_ptp_tx_get_from_buffer(g_c1588, &tx_pkt_info, 123);
        if (c1588_status == 0)
        {
            uint8_t pkt_info_msg[80] = {0};
            switch(tx_pkt_info.type)
            {
                case C1588_TXSYNC:
                    sprintf(pkt_info_msg, "\n\r Sync message transmitted at %lu seconds, %lu nanoseconds", (uint32_t)tx_pkt_info.ts.secs, tx_pkt_info.ts.nsecs);
                    break;
                case C1588_TXDELAYREQ:
                    sprintf(pkt_info_msg, "\n\r Delay request message transmitted at %lu seconds, %lu nanoseconds", (uint32_t)tx_pkt_info.ts.secs, tx_pkt_info.ts.nsecs);
                    break;
                case C1588_TXPDELAYREQ:
                    sprintf(pkt_info_msg, "\n\r Peer delay request message transmitted at %lu seconds, %lu nanoseconds", (uint32_t)tx_pkt_info.ts.secs, tx_pkt_info.ts.nsecs);
                    break;
                case C1588_TXPDELAYRESP:
                    sprintf(pkt_info_msg, "\n\r Peer delay response message transmitted at %lu seconds, %lu nanoseconds", (uint32_t)tx_pkt_info.ts.secs, tx_pkt_info.ts.nsecs);
                    break;
            }
            print((uint8_t *)pkt_info_msg);
        }

        if (rtc_sec_mode_tick_flag == 1)
        {
            uint8_t rtc_sec_msg[20] = {0};
            sprintf(rtc_sec_msg, "\n\r %d second(s)", rtc_sec_mode_count);
            print((uint8_t *)rtc_sec_msg);
            rtc_sec_mode_tick_flag = 0;
        }

        if (tse_tx_success_flag == TSE_TX_SUCCESS)
        {
            print("\n\r  Packet Tx success");
            tse_tx_success_flag = 0;
        }
    }
    return 0u;
}
