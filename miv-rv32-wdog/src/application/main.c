/*******************************************************************************
 * Copyright 2022 Microchip FPGA Embedded Systems Solutions.
 *
 * SPDX-License-Identifier: MIT
 *
 * Sample project to demonstrate the MIV Watchdog functionality.
 *
 * Please refer README.md in the root folder of this project for more details.
 */

#include <stdio.h>
#include "miv_rv32_hal/miv_rv32_hal.h"
#include "fpga_design_config/fpga_design_config.h"
#include "drivers/fpga_ip/CoreUARTapb/core_uart_apb.h"
#include "drivers/fpga_ip/miv_watchdog/miv_watchdog.h"

/******************************************************************************
 * MIV Peripheral instance data.
 *****************************************************************************/
 UART_instance_t g_uart;
 miv_wdog_config_t wd0_config;

/*-----------------------------------------------------------------------------
 * Watchdog instance data.
 */
volatile uint8_t h0_triggered = 0u;
volatile uint8_t h0_mvrp = 0u;

/*==============================================================================
 * Messages displayed over the UART.
 */
const uint8_t g_greeting_msg[] =
"\r\n***********************************************************************\r\n\n\
********************    Mi-V Watchdog Example    ***********************\r\n\n\
***************************************************************************\r\n\r\n\
The MVRP interrupt will occur after 34 seconds, after system reset \r\n\
The Time out interrupt will occur after 85 seconds, after system reset \r\n\n ";

const uint8_t g_separator[] =
"\r\n----------------------------------------------------------------------\r\n";

static void display_greeting(void);

uint8_t data_buffer [1024];
uint8_t input_text[506] = {0x00};

static void clear_data_buffer(void)
{
    uint32_t idx = 0u;

    while (idx < 1024u)
    {
        data_buffer[idx++] = 0x00u;
    }
}
/*==============================================================================
 * Display greeting message when application is started.
 */
static void display_greeting(void)
{
    UART_polled_tx_string(&g_uart, g_greeting_msg);
}

void MSYS_EI3_IRQHandler()
{
    h0_mvrp = 1;

    /* Un-commenting the below statement will refresh the Watchdog and then the
     * timer will be reloaded to original TIME value and start decrementing.
     * This will avoid the timeout interrupt and RESET.
     *
     * Not reloading the Watchdog will make the down counter to pass below
     * trigger value and cause a timeout interrupt. When the down counter rolls
     * down to zero, it would generate a RESET.
     * */

//    MIV_WDOG_reload();

    MIV_WDOG_clear_mvrp_irq();

}

void MSYS_EI4_IRQHandler()
{
    h0_triggered = 1u;

    MIV_WDOG_clear_timeout_irq();
}

/******************************************************************************
 * main function.
 *****************************************************************************/
void main(void)
{
    size_t rx_size;
    uint8_t rx_buff[1] = {0x00};
    uint32_t counter = 0;
    uint32_t read_data[100] = {0};
    uint32_t icount = 0u;
    uint32_t current_value = 0u;

    clear_data_buffer();

    /* Initialize CoreUARTapb with its base address, baud value, and line
     * configuration.
     */
    UART_init(&g_uart, COREUARTAPB0_BASE_ADDR, BAUD_VALUE_115200, (DATA_8_BITS | NO_PARITY));

    display_greeting();

    MRV_enable_local_irq(MRV32_MSYS_EIE3_IRQn | MRV32_MSYS_EIE4_IRQn );

    HAL_enable_interrupts();

    MIV_WDOG_init(MIV_ESS_WDOG_BASE_ADDR);
    /* Read the default config */
    MIV_WDOG_get_config(&wd0_config);

    /* Set such that the MVRP interrupt will happen after ~13 seconds after
     * reset and the Trigger interrupt will happen after ~25 seconds.
     */

    wd0_config.forbidden_en = MIV_WDOG_ENABLE;
    wd0_config.timeout_val = 0x3e0u;
    wd0_config.mvrp_val = (10000000UL);
    wd0_config.time_val = (2UL*10000000UL);

    MIV_WDOG_configure(&wd0_config);

    MIV_WDOG_enable_mvrp_irq();

    while(1u)
    {
        icount++;

        if (0x90000u == icount)
        {
            /*
             * Read the current value after 90000 ticks.
             */
            icount = 0u;
            current_value = MIV_WDOG_current_value();
            snprintf((char *)data_buffer, sizeof(data_buffer),
                     "WD0 value = %x\r\n", *(uint32_t*)0x79000000);


            UART_polled_tx_string(&g_uart,
                                  data_buffer);
        }

        if(h0_mvrp)
        {
            UART_polled_tx_string(&g_uart,
                                (const uint8_t *)"\r\nMVRP Interrupt...\n");
            h0_mvrp = 0;

            UART_polled_tx_string(&g_uart,
                                (const uint8_t *)"\r\nMVRP IRQ Cleared \r\n");

            snprintf((char *)data_buffer, sizeof(data_buffer),
                     "\r\nH0 MVRP - Returned to Local\r\n\n");

            UART_polled_tx_string(&g_uart,
                                  data_buffer);
        }

        if(h0_triggered)
        {
            UART_polled_tx_string(&g_uart,
                                (const uint8_t *)"\r\nWDOG Interrupt...\n");

            h0_triggered = 0;

            UART_polled_tx_string(&g_uart,
                                (const uint8_t *)"\r\nWDOG IRQ Cleared \r\n");

            snprintf((char *)data_buffer, sizeof(data_buffer),
                     "\r\nH0 timeout - Returned to Local\r\n");


            UART_polled_tx_string(&g_uart,
                                  data_buffer);
        }

    }
}
