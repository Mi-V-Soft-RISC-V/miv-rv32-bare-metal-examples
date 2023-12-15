/*******************************************************************************
 * Copyright 2019 Microchip FPGA Embedded Systems Solutions.
 *
 * SPDX-License-Identifier: MIT
 *
 * @file main.c
 * @author Microchip FPGA Embedded Systems Solutions
 * @brief Application demonstrating the use of CoreUARTapb driver. Please see
 * the README.md for more details.
 *
 */

#include <drivers/fpga_ip/CoreUARTapb/core_uart_apb.h>
#include "fpga_design_config/fpga_design_config.h"
#include "miv_rv32_hal/miv_rv32_hal.h"

/******************************************************************************
 * Maximum receiver buffer size.
 *****************************************************************************/
#define MAX_RX_DATA_SIZE    256

/******************************************************************************
 * CoreUARTapb instance data.
 *****************************************************************************/
UART_instance_t g_uart;

/******************************************************************************
 * Instruction message. This message will be transmitted over the UART to
 * HyperTerminal when the program starts.
 *****************************************************************************/
uint8_t g_message[] =
"\r\n\r\n\
***************************************************************************\r\n\
*******************  CoreUARTapb Example Project **************************\r\n\
***************************************************************************\r\n\
\r\n\r\n\
This example demonstrates CoreUARTapb polled transmit and receiver APIs. \
\n\rAll characters typed will be echoed back.\n\r";

/******************************************************************************
 * main function.
 *****************************************************************************/
void main( void )
{
    uint8_t rx_data[MAX_RX_DATA_SIZE];
    size_t rx_size;

    /**************************************************************************
     * Initialize CoreUARTapb with its base address, baud value, and line
     * configuration.
     *************************************************************************/
    UART_init( &g_uart, COREUARTAPB0_BASE_ADDR,
            BAUD_VALUE_115200, (DATA_8_BITS | NO_PARITY) );

    /**************************************************************************
     * Send the instructions message.
     *************************************************************************/
    UART_polled_tx_string( &g_uart, g_message );

    /**************************************************************************
     * Infinite Loop.
     *************************************************************************/
    while(1)
    {
        /***********************************************************************
         * Check for any errors in communication while receiving data
         **********************************************************************/
        if(UART_APB_NO_ERROR == UART_get_rx_status(&g_uart))
        {
            /*******************************************************************
             * Read data received by the UART.
             ******************************************************************/
            rx_size = UART_get_rx( &g_uart, rx_data, sizeof(rx_data) );

            /*******************************************************************
             * Echo back data received, if any.
             ******************************************************************/
            if ( rx_size > 0 )
            {
                UART_send( &g_uart, rx_data, rx_size );
            }
        }
    }
}
