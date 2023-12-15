/*******************************************************************************
 * Copyright 2023 Microchip FPGA Embedded Systems Solutions.
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
#define NULL 0

/******************************************************************************
 * CoreUARTapb instance data.
 *****************************************************************************/
UART_instance_t g_uart;

uint8_t rx_data[MAX_RX_DATA_SIZE];
size_t rx_size = NULL;

/******************************************************************************
 * Greeting message displayed over the UART terminal.
 *****************************************************************************/
uint8_t g_message[] =
"\r\n\r\n\
***************************************************************************\r\n\
*******************  CoreUARTapb Example Project **************************\r\n\
***************************************************************************\r\n\
\r\n\r\n\
This example project demonstrates the interrupt based transmission and reception\
\r\nusing CoreUARTapb. Press any key to get an interrupt.\r\n\n";

/******************************************************************************
 * Interrupt message displayed over the UART.
 *****************************************************************************/
uint8_t interrupt_message[] = "\r\r\n\r\n\n Both MSYS external interrupt 1 and \
MSYS external interrupt 2 occurred. \n\r";

/******************************************************************************
 * Interrupt Service Routine (ISR) for the MSYS external interrupt 2,
 * which is connected to UART_RXRDY pin of CoreUARTapb
 *****************************************************************************/
void MSYS_EI2_IRQHandler(void)
{
    rx_size = UART_get_rx( &g_uart, rx_data, sizeof(rx_data) );
}

/******************************************************************************
 * Interrupt Service Routine (ISR) for the MSYS external interrupt 1,
 * which is connected to UART_TXRDY pin of CoreUARTapb
 *****************************************************************************/
void MSYS_EI1_IRQHandler(void)
{
    if(rx_size > NULL)
    {
        UART_send( &g_uart, rx_data, rx_size );
        rx_size--;
        UART_polled_tx_string( &g_uart, interrupt_message );
    }
}

/******************************************************************************
 * main function.
 *****************************************************************************/
void main( void )
{

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

    /**************************************************************************
     * Enable MSYS external interrupt 2 - UART_RXRDY
     *************************************************************************/
    MRV_enable_local_irq(MRV32_MSYS_EIE2_IRQn);

    /**************************************************************************
     * Enable MSYS external interrupt 1 - UART_TXRDY
     *************************************************************************/
    MRV_enable_local_irq(MRV32_MSYS_EIE1_IRQn);

    /**************************************************************************
     * Enables all interrupts
     *************************************************************************/
    HAL_enable_interrupts();

    /**************************************************************************
     * Infinite Loop.
     *************************************************************************/
    while(1u)
    {
        ;
    }
}
