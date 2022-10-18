/*******************************************************************************
 * Copyright 2022 Microchip FPGA Embedded Systems Solutions.
 *
 * SPDX-License-Identifier: MIT
 *
 * Sample project to demonstrate the MIV Timer functionality.
 *
 * Following example will trigger the MIV Timer interrupt after every ~1 second
 * for a 50MHz clock.
 *
 * For different clock frequency, calculate the timer compare value accordingly.
 *
 * Please refer README.md in the root folder of this project for more details.
 */
#include <stdio.h>
#include "fpga_design_config/fpga_design_config.h"
#include "miv_rv32_hal/miv_rv32_hal.h"
#include "drivers/fpga_ip/CoreUARTapb/core_uart_apb.h"
#include "drivers/fpga_ip/miv_timer/miv_timer.h"

/******************************************************************************
 * Peripheral instance data.
 *****************************************************************************/
UART_instance_t g_uart;

miv_timer_instance_t g_miv_timer;

/******************************************************************************
 * Messages displayed over the UART.
 */
const uint8_t g_greeting_msg[] =
"\r\n***************************************************************************\r\n\n\
********************    Mi-V ESS Timer Example    ***********************\r\n\n\
*****************************************************************************\r\n\r\n\
Timer interrupt is configured to trigger after ~every second.\r\n";

static void display_greeting(void);

/*==============================================================================
 * Display greeting message when application is started.
 */
static void display_greeting(void)
{
    UART_polled_tx_string(&g_uart, g_greeting_msg);
}

/* Systick handler to handle the MIV Timer interrupt.
 *
 * Write to the compare time register(mtimecmp) register to deassert the Timer
 * interrupt.
 * Following example will trigger the MIV Timer interrupt after every ~1 second
 * for a 50MHz clock.
 *
 * The MIV_ESS Timer is the external timer to the MIV_RV32, The
 * MIV_RV32_EXT_TIMECMP macro must be enabled in the project settings.
 *
 *Change this as per your requirement.
 */
void SysTick_Handler(void)
{
    UART_polled_tx_string(&g_uart,
                        (const uint8_t *)"\r\nMIV TIMER Interrupt");

    uint64_t read_mtime = MIV_TIMER_read_current_time(&g_miv_timer);

    MIV_TIMER_write_compare_time(&g_miv_timer, (read_mtime + 49550));
}

/******************************************************************************
 * main function.
 *****************************************************************************/
void main(void)
{
    size_t rx_size;
    uint8_t rx_buff[1] = {0x00};

    /* Initialize CoreUARTapb with its base address, baud value, and line
     * configuration.
     */
    UART_init(&g_uart, COREUARTAPB0_BASE_ADDR, BAUD_VALUE_115200, (DATA_8_BITS | NO_PARITY));

    display_greeting();

    HAL_enable_interrupts();

    MIV_TIMER_init(&g_miv_timer, MIV_MTIMER_BASE_ADDR);
    MIV_TIMER_config(&g_miv_timer, SYS_CLK_FREQ);
    MRV_enable_local_irq(MIP_MTIP);

    /**************************************************************************
    * Loop
    *************************************************************************/
    do
    {
        rx_size = UART_get_rx(&g_uart, rx_buff, sizeof(rx_buff));
        if (rx_size > 0u)
        {
            /* Display greeting message on terminal with each input */
            UART_polled_tx_string(&g_uart, (const uint8_t *)g_greeting_msg);
            rx_size = 0u;
        }
        uint64_t read_mtime = MIV_TIMER_read_current_time(&g_miv_timer);
    } while (1);
}
