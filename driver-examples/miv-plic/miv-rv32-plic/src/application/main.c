/*******************************************************************************
 * Copyright 2022 Microchip FPGA Embedded Systems Solutions.
 *
 * SPDX-License-Identifier: MIT
 *
 * MIV PLIC Example.
 *
 * This project demonstrates the use of MiV PLIC bare-metal driver.
 * This project uses GPIO 4, 5, 6 and 7 to trigger the PLIC interrupt 0 - 3
 * respectively.
 * Please refer README.md in the root folder of this project for more details.
 */

#include <stdio.h>
#include "miv_rv32_hal/miv_rv32_hal.h"
#include "fpga_design_config/fpga_design_config.h"
#include "drivers/fpga_ip/miv_plic/miv_plic.h"
#include "drivers/fpga_ip/CoreGPIO/core_gpio.h"
#include "drivers/fpga_ip/CoreUARTapb/core_uart_apb.h"

/******************************************************************************
 * CoreUARTapb instance data.
 *****************************************************************************/
UART_instance_t g_uart;

gpio_instance_t g_gpio_inout;

miv_plic_instance_t g_plic;

#define PLIC_EXT_INTR_SOURCES                          31u

/*==============================================================================
 * Messages displayed over the UART.
 */
const uint8_t g_greeting_msg[] =
"\r\n\r\n **** PolarFire MiV PLIC Example ****\n\n\n\r\
Select 1 -> PLIC0, 2 -> PLIC1, 3 ->PLIC2, 4 ->PLIC3 interrupt\r\n\
";

const uint8_t g_separator[] =
"\r\n----------------------------------------------------------------------\r\n";

static void display_greeting(void);

uint8_t data_buffer [1024];
uint8_t input_text[506] = {0x00};
uint32_t g_udma_status = 0u;

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

/************************************************************************//***
 * In the test design the plic interrupts 0-3 are connected to GPIO 4:7
 * respectively. This application demonstrates the invoking of these plic
 * interrupt handlers.
 */
uint8_t MIV_PLIC_EXT0_IRQHandler(void)
{
    GPIO_set_output(&g_gpio_inout, GPIO_4, 0u);
    UART_polled_tx_string(&g_uart,
                        (const uint8_t *)"\r\nESS PLIC 0 Interrupt");

    return(EXT_IRQ_KEEP_ENABLED);
}

uint8_t MIV_PLIC_EXT1_IRQHandler(void)
{
    GPIO_set_output(&g_gpio_inout, GPIO_5, 0u);
    UART_polled_tx_string(&g_uart,
                        (const uint8_t *)"\r\nESS PLIC 1 Interrupt");

    return(EXT_IRQ_KEEP_ENABLED);
}

uint8_t MIV_PLIC_EXT2_IRQHandler(void)
{
    GPIO_set_output(&g_gpio_inout, GPIO_6, 0u);
    UART_polled_tx_string(&g_uart,
                        (const uint8_t *)"\r\nESS PLIC 2 Interrupt");

    return(EXT_IRQ_KEEP_ENABLED);
}

uint8_t MIV_PLIC_EXT3_IRQHandler(void)
{
    GPIO_set_output(&g_gpio_inout, GPIO_7, 0u);
    UART_polled_tx_string(&g_uart,
                        (const uint8_t *)"\r\nESS PLIC 3 Interrupt");
    return(EXT_IRQ_KEEP_ENABLED);
}

uint8_t ESS_PLIC_EXT5_IRQHandler(void)
{
    return(EXT_IRQ_KEEP_ENABLED);
}

uint8_t ESS_PLIC_EXT6_IRQHandler(void)
{
    return(EXT_IRQ_KEEP_ENABLED);
}

uint8_t ESS_PLIC_EXT7_IRQHandler(void)
{
    return(EXT_IRQ_KEEP_ENABLED);
}

/******************************************************************************/

/**************************************************************************//**
 * PLIC interrupts are routed as external interrupt to the MIV_RV32 core from
 * MIV_ESS. The handle_plic_interrupt() is the ESS_PLIC driver function which
 * will identify the plic interrupt number and then invokes the appropriate
 * plic interrupt handler.
 */
void External_IRQHandler(void)
{
    uint32_t reg_val = read_csr(mip);
    MIV_PLIC_isr(&g_plic);
}

void Invalid_IRQHandler(void)
{
    UART_polled_tx_string(&g_uart,
                        (const uint8_t *)"\r\nInvalid\r\n");
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

    clear_data_buffer();

    /* Initialize CoreUARTapb with its base address, baud value, and line
     * configuration.
     */
    UART_init(&g_uart, COREUARTAPB0_BASE_ADDR, BAUD_VALUE_115200, (DATA_8_BITS | NO_PARITY));

    GPIO_init(&g_gpio_inout, COREGPIO_BASE_ADDR, GPIO_APB_32_BITS_BUS);

    GPIO_config(&g_gpio_inout, GPIO_4, GPIO_OUTPUT_MODE);
    GPIO_config(&g_gpio_inout, GPIO_5, GPIO_OUTPUT_MODE);
    GPIO_config(&g_gpio_inout, GPIO_6, GPIO_OUTPUT_MODE);
    GPIO_config(&g_gpio_inout, GPIO_7, GPIO_OUTPUT_MODE);

    /* set the output value */
    GPIO_set_outputs(&g_gpio_inout, 0x0u);

    display_greeting();

    MRV_enable_local_irq(MRV32_EXT_IRQn |
                         MRV32_MSYS_EIE0_IRQn |
                         MRV32_MSYS_EIE1_IRQn |
                         MRV32_MSYS_EIE2_IRQn |
                         MRV32_MSYS_EIE3_IRQn |
                         MRV32_MSYS_EIE4_IRQn |
                         MRV32_MSYS_EIE5_IRQn);

    MIV_PLIC_init(&g_plic, MIV_PLIC_BASE_ADDR, PLIC_EXT_INTR_SOURCES);

    MIV_PLIC_enable_irq(&g_plic, MIV_PLIC_EXT0_IRQn); /* GPIO-4 */
    MIV_PLIC_enable_irq(&g_plic, MIV_PLIC_EXT1_IRQn); /* GPIO-5 */
    MIV_PLIC_enable_irq(&g_plic, MIV_PLIC_EXT2_IRQn); /* GPIO-6 */
    MIV_PLIC_enable_irq(&g_plic, MIV_PLIC_EXT3_IRQn); /* GPIO-7 */

    MRV_enable_local_irq(MIP_MEIP);
    HAL_enable_interrupts();

    volatile uint32_t reg_val = 0;
    for (;;)
    {
        /* Start command line interface if any key is pressed. */
        rx_size = UART_get_rx(&g_uart, rx_buff, sizeof(rx_buff));
        static volatile uint32_t val = 0u;

        if (rx_size > 0u)
        {
            UART_polled_tx_string(&g_uart, g_separator);
            switch(rx_buff[0])
            {
            case '1':
                val = 0x10u;
                GPIO_set_outputs(&g_gpio_inout, val);
                break;

                case '2':
                    val = 0x20u;
                    GPIO_set_outputs(&g_gpio_inout, 0x20u);
                break;

                case '3':
                    val = 0x40u;
                    GPIO_set_outputs(&g_gpio_inout, val);
                break;

                case '4':
                    val = 0x80u;
                    GPIO_set_outputs(&g_gpio_inout, val);
                break;

                default:
                    display_greeting();
                break;
            }
        }
    }
}
