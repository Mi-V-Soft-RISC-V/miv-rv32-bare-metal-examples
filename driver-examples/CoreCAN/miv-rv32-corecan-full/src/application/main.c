/*******************************************************************************
 * Copyright 2025 Microchip FPGA Embedded Systems Solutions.
 *
 * SPDX-License-Identifier: MIT
 *
 * @file main.c
 * @author Microchip FPGA Embedded Systems Solutions
 * @brief This example project demonstrates the use of CoreCAN IP
 *
 */

#include <stdio.h>
#include "miv_rv32_hal/miv_rv32_hal.h"
#include "fpga_design_config/fpga_design_config.h"
#include "drivers/fpga_ip/CoreUARTapb/core_uart_apb.h"
#include "drivers/fpga_ip/CoreCAN/core_can.h"

UART_instance_t   g_uart;
can_instance_t    g_can;

can_txmsgobject_t tx_msg;
can_rxmsgobject_t rx_msg;

#define CAN_TX_MB_NUM                       6u
#define CAN_RX_MB_NUM                       6u

#define ENTER                               0xDu
#define BUFF_SIZE                           64u

static uint8_t get_data_frm_uart(void);
static void ascii_to_hex(uint8_t * in_buffer, uint32_t byte_length);
static void display_option(void);
static void check_rx_buffer(void);
static void display_hex_values(const uint8_t * in_buffer, uint32_t byte_length);

/*------------------------------------------------------------------------------
 * Static Variables.
 */
static uint8_t g_temp[BUFF_SIZE];
static uint8_t g_uart_to_can[BUFF_SIZE];
static uint8_t g_can_to_uart[BUFF_SIZE];

volatile uint8_t tx_done_flag = 0;
volatile uint8_t rx_ready_flag = 0;

/******************************************************************************
 * Instruction message. This message will be transmitted to the UART terminal
 * when the program starts.
 *****************************************************************************/
const uint8_t g_message[] =
"\r\n *************************************************************************\
\r\n\r\n                CoreCAN Example Project         \r\n\r\n\
 **************************************************************************\r\n\
\r\n\nThis example project demonstrates the use of CoreCAN driver transmission\n\r\
and reception with Mi-V soft processor. Read data from the UART and transmit\n\r\
as CAN message using CoreCAN. Receive the CAN msg from can analyzer and send\n\r\
it to UART.\r\n";

const uint8_t g_select_operation_msg[] =
"\n\r*************** Select the Option to proceed further *****************\n\r\
Press Key '6' to send data.\n\r\
Press Key '7' to reinitalize CoreCAN device.\n\r";

/***************************************************************************//**
 * main function.
 */
int main(void)
{
    uint8_t rx_size = 0u;
    uint8_t rx_char;
    uint8_t rx_bytes = 0u;
    uint8_t loop_count;
    uint32_t msg_len;
    uint32_t chunk_size;
    uint32_t error_flag = 0u;
    uint32_t offset = 0;
    uint8_t ret_status;
    uint32_t no_of_msgs;
    volatile uint8_t count = 0u;
    uint32_t tx_status = 0u;

    /**************************************************************************
     * Initialize CoreUARTapb with its base address, baud value, and line
     * configuration.
     *************************************************************************/
    UART_init( &g_uart, COREUARTAPB0_BASE_ADDR,
            BAUD_VALUE_115200, (DATA_8_BITS | NO_PARITY) );

    /**************************************************************************
     * Send the Greeting message.
     *************************************************************************/
    UART_polled_tx_string( &g_uart, g_message );

    uint8_t init_ret_value = CAN_init( &g_can, CORECAN0_BASE_ADDR, CAN_SPEED_40M_1M,
            0u, CAN_RX_MB_NUM, CAN_TX_MB_NUM);

    if (CAN_OK != init_ret_value)
    {
        UART_polled_tx_string( &g_uart, "\r\nCAN_init failed!\n" );
    }

    CAN_set_mode( &g_can, CANOP_MODE_NORMAL);
    CAN_start(&g_can);

    /**************************************************************************
     * Enable MSYS external interrupt 0
     *************************************************************************/
    MRV_enable_local_irq(MRV32_MSYS_EIE0_IRQn);

    CAN_enable_irq(&g_can, CAN_INT_TX_MSG | CAN_INT_RX_MSG);

    /**************************************************************************
     * Enables all interrupts
     *************************************************************************/
    HAL_enable_interrupts();

    /* Configure transmit message */
    tx_msg.RTR = 0x0;
    tx_msg.DLC = 0x8;
    tx_msg.word1 = 0xa1a1b1b1;
    tx_msg.word0 = 0xc1d1;

#ifdef CAN_TX_EXTENDED_ID
    tx_msg.IDE = 0x1;
    tx_msg.ID = 0x10A1238E; /* Extended ID 29 bit ID, 8 bytes of data */
#else
    tx_msg.IDE = 0x0;
    tx_msg.ID = 0x123;        /* Standard ID 11 bit ID, 8 bytes of data */
#endif

#ifdef CAN_RX_EXTENDED_ID
    rx_msg.AMR = 0x00000000u; //IDE-1, 29-bits are masked i.e 0's
    rx_msg.ACR = 0x85091C74u; //IDE-1, so ID>>3 so actual ID is (0x10A1238E)
#else
    rx_msg.AMR = 0x001FFFF8u; //IDE-0, Upper 11-bits are masked i.e 0's
    rx_msg.ACR = 0x11c00000u; //IDE-0, so ID>>21 so actual ID is (0x8E)
#endif
    rx_msg.AMR_D = (uint16_t) 0xFFFFu;
    rx_msg.ACR_D = 0x00000000u;

    ret_status = CAN_config_buffer_f( &g_can, CAN_MB_0, &rx_msg);
    if (CAN_OK != ret_status)
    {
        UART_polled_tx_string(&g_uart, (const uint8_t *)"\n\rMessage buffer configuration error");
    }

    CAN_send_msg_f( &g_can, CAN_MB_0, &tx_msg);

    while(1)
    {
        /*----------------------------------------------------------------------
         * Read the Data from UART and Transmit using CAN
         */
        rx_bytes = get_data_frm_uart();

        /* Convert ASCII values to Hex */
        ascii_to_hex(g_temp, rx_bytes);

        for (loop_count = 0u; loop_count < rx_bytes / 2u; loop_count++)
        {
            g_uart_to_can[loop_count] = g_temp[loop_count * 2u];
            g_uart_to_can[loop_count] = g_uart_to_can[loop_count] << 4u;
            g_uart_to_can[loop_count] |= g_temp[(loop_count * 2u) + 1u];
        }

        display_hex_values(g_uart_to_can, loop_count);

        no_of_msgs = rx_bytes / 8;
        if (rx_bytes % 8 != 0)
        {
            no_of_msgs++;
        }

        if (0u == loop_count)
        {
            no_of_msgs = 1;  // allow sending an empty packet
        }
        count = 0u;
        msg_len = loop_count;
        error_flag = 0u;
        while ((no_of_msgs != 0u) && (0u == error_flag))
        {
           if (msg_len >= 8u)
           {
               chunk_size = 8u;
           }
           else
           {
               chunk_size = msg_len;
           }
           // Clear the word0 and word1 before packing new data
           tx_msg.word0 = 0u;
           tx_msg.word1 = 0u;

           for (uint8_t i = 0; i < chunk_size; i++)
           {
               if (i < 4)
               {
                   tx_msg.word1 |= ((uint32_t)g_uart_to_can[count + i] << (8 * (3 - i)));
               }
               else
               {
                   tx_msg.word0 |= ((uint32_t)g_uart_to_can[count + i] << (8 * (7 - i)));
               }
           }

            tx_msg.DLC = chunk_size;
            ret_status = CAN_send_msg_f( &g_can, CAN_MB_0, &tx_msg);
            if (CAN_VALID_MSG != ret_status)
            {
                error_flag = 1; // Error occurred
            }
            else
            {
                /*Wait around for this packet to send before going any further*/
                tx_status = CAN_get_tx_buff_sts(&g_can);
                while (1u == (tx_status & 0x1))
                {
                    tx_status = CAN_get_tx_buff_sts(&g_can);
                }
                no_of_msgs--;
                msg_len -= chunk_size;
                count++;
            }
        }
        if(tx_done_flag)
        {
            UART_polled_tx_string( &g_uart, "\n\n\rMsg transmitted and interrupt received. \n" );
            tx_done_flag = 0u;
        }
        if (0u == count) /* Nothing sent */
        {
            UART_polled_tx_string( &g_uart, "\r\n\rUnable to send data via CAN bus\n\r");
        }
        else
        {
            if (0u == error_flag) /* Everything sent */
            {
                UART_polled_tx_string( &g_uart, "\r\n\n\rObserve the data received on CAN analyzer\n\r");
                UART_polled_tx_string( &g_uart, "\r\n\rIt should be same as the data transmitted from UART terminal\n\r");
            }
            else /* Some error occurred */
            {
                UART_polled_tx_string( &g_uart, "\r\n\n\rObserve the data received on CAN analyzer\n\r");
                UART_polled_tx_string( &g_uart, "\r\n\rSome transmission error(s) were detected.\n\r");
            }
        }

        UART_polled_tx_string( &g_uart, "\r\n\r------------------------------------------------------------------------------\n\r");
        UART_polled_tx_string( &g_uart, "\r\n\rPress any key to continue...\n\r");

        do {
            rx_size = UART_get_rx(&g_uart, &rx_char, sizeof(rx_char));
        } while (rx_size == 0u);

        UART_polled_tx_string( &g_uart, (const uint8_t*)"\n\r");

        /*----------------------------------------------------------------------
         *  Display options
         */
         display_option();
    }
}

/*------------------------------------------------------------------------------
 * Receive data from UART terminal.
 */
static uint8_t get_data_frm_uart(void)
{
    uint8_t complete = 0;
    uint8_t rx_buff[1];
    uint8_t count = 0;
    uint8_t rx_size = 0;

    UART_polled_tx_string( &g_uart, "\r\n\nEnter the data to transmit through the CAN channel:\n\r");

    count = 0u;
    while (!complete)
    {
        rx_size = UART_get_rx(&g_uart, rx_buff, sizeof(rx_buff));
        if(rx_size > 0u)
        {
            UART_send(&g_uart, rx_buff, sizeof(rx_buff));

            if (ENTER == rx_buff[0])
            {
                complete = 1u;
            }
            else
            {
                g_temp[count] = rx_buff[0];
                count++;
            }

            if (BUFF_SIZE == count)
            {
                complete = 1u;
            }
        }
    }
    return(count);
}

/*------------------------------------------------------------------------------
 * Converts ASCII values to HEX values
 */
static void ascii_to_hex
(
    uint8_t * in_buffer,
    uint32_t byte_length
)
{
    uint32_t inc;

    for (inc = 0u; inc < byte_length; inc++)
    {
        if ((in_buffer[inc] <= 0x39u) && (in_buffer[inc] >= 0x30u))
        {
            in_buffer[inc] = in_buffer[inc] - 0x30u;
        }
        else if ((in_buffer[inc] <= 0x5Au) && (in_buffer[inc] >= 0x41u))
        {
            in_buffer[inc] = 0xAu + in_buffer[inc] - 0x41u;
        }
        else if ((in_buffer[inc] <= 0x7Au) && (in_buffer[inc] >= 0x61u))
        {
            in_buffer[inc] = 0xAu + (in_buffer[inc] - 0x61u);
        }
        else
        {
            ;/* Do Nothing. */
        }
    }
}

/*------------------------------------------------------------------------------
 * Display content of buffer passed as parameter as hex values.
 */
static void display_hex_values
(
    const uint8_t * in_buffer,
    uint32_t byte_length
)
{
    uint8_t display_buffer[BUFF_SIZE];
    uint32_t inc;

    if (0u == byte_length)
    {
        UART_polled_tx_string(&g_uart, (const uint8_t*)"\n\rNo data present>\n\r");
    }
    else
    {
        if (byte_length > 16u)
        {
            UART_polled_tx_string(&g_uart, (const uint8_t*)"\n\r");
        }

        for (inc = 0u; inc < byte_length; ++inc)
        {
            if ((inc > 1u) && (0u == (inc % 16u)))
            {
                UART_polled_tx_string(&g_uart, (const uint8_t*)"\n\r");
            }
            snprintf((char *)display_buffer, sizeof(display_buffer), "%02x ", in_buffer[inc]);
            UART_polled_tx_string(&g_uart, display_buffer);
        }
    }
}

/*------------------------------------------------------------------------------
 * Display the Option to continue or exit.
 */
static void display_option(void)
{
    uint8_t rx_size=0;
    uint8_t rx_buff[1];

    UART_polled_tx_string( &g_uart, g_select_operation_msg );

    do
    {
        /* Start command line interface if any key is pressed. */
        rx_size = UART_get_rx(&g_uart, rx_buff, sizeof(rx_buff));
        if (rx_size > 0u)
        {
            switch(rx_buff[0])
            {
                case '6':
                break;

                case '7':

                    CAN_init( &g_can, CORECAN0_BASE_ADDR, CAN_SPEED_40M_1M,
                                             0u, CAN_RX_MB_NUM, CAN_TX_MB_NUM);
                    CAN_set_mode( &g_can, CANOP_MODE_NORMAL);
                    CAN_start(&g_can);
                break;

                default:
                break;
            }
        }

        /*----------------------------------------------------------------------
         *  Read the Data from CAN channel and Transmit Through UART1
         */
        check_rx_buffer();

    }while ((rx_buff[0]!= '6') && (rx_buff[0]!= '7'));
}

static void check_rx_buffer(void)
{
    uint8_t loop_count;

    if (rx_ready_flag)
    {
        UART_polled_tx_string(&g_uart, "\n\n\rCAN message received and interrupt triggered.\n");
        rx_ready_flag = 0u;
    }

    /*----------------------------------------------------------------------
     * Read the Data from CAN channel and transmit through UART
     */
    if (CAN_VALID_MSG == CAN_get_msg_f(&g_can, CAN_MB_0, &rx_msg))
    {
        for (loop_count = 0u; loop_count < rx_msg.DLC; loop_count++)
        {
            if (loop_count < 4u)
            {
                g_can_to_uart[loop_count] = (uint8_t)(rx_msg.word1 >> (8 * (3 - loop_count)));;
            }
            else
            {
                g_can_to_uart[loop_count] = (uint8_t)(rx_msg.word0 >> (8 * (7 - loop_count)));
            }
        }

        UART_polled_tx_string( &g_uart, "\n\rData Received as CAN Message\n\r");

        /* Send to UART */
        display_hex_values(g_can_to_uart, rx_msg.DLC);

        UART_polled_tx_string( &g_uart, "\n\rObserve the message sent from the CAN Analyzer ");
        UART_polled_tx_string( &g_uart, "\n\rIt should be same as message Received on UART terminal ");
        UART_polled_tx_string( &g_uart, "\n\r******************************************************************************\n\r");
    }
}

/******************************************************************************
 * Interrupt Service Routine (ISR) for the MSYS external interrupt 0,
 * which is connected to CAN_INT_N pin.
 *****************************************************************************/
void MSYS_EI0_IRQHandler(void)
{
    uint32_t int_status = CAN_get_int_src(&g_can);

    /* TX complete? */
    if (int_status & CAN_INT_TX_MSG)
    {
        tx_done_flag = 1;
        CAN_clear_int_src(&g_can, CAN_INT_TX_MSG);
    }

    /*RX message available?*/
    if (int_status & CAN_INT_RX_MSG)
    {
        rx_ready_flag = 1;
        CAN_clear_int_src(&g_can, CAN_INT_RX_MSG);
    }
}
