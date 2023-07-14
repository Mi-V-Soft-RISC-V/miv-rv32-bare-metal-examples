/*******************************************************************************
 * Copyright 2023 Microchip FPGA Embedded Systems Solutions.
 *
 * SPDX-License-Identifier: MIT
 *
 * This example project demonstrates communications with an external flash
 * device.
 *
 * Please refer to the file README.md for further details about this example.
 *
 */
#include "miv_rv32_hal/miv_rv32_hal.h"
#include "fpga_design_config/fpga_design_config.h"
#include "drivers/fpga_ip/CoreUARTapb/core_uart_apb.h"
#include "drivers/fpga_ip/CoreTimer/core_timer.h"
#include "drivers/fpga_ip/CoreGPIO/core_gpio.h"

static volatile uint32_t gpio_pins_state = 0x0u;
static uint32_t timer_reload_check = 0x0u;

UART_instance_t g_core_uart_0;

uint8_t g_message[] =
    "\r\n\r\n\**************  Mi-V CoreTimer: Timer Single-Shot Example Project  "
    "**************\r\n\r\n\r\nThis example project demonstrates the use of the CoreTimer "
    "driver operating in\r\nsingle-shot mode\r\n";

/*------------------------------------------------------------------------------
 * Timer load value.
 * This value is calculated to result in the timer timing out after after 1
 * second with a system clock of 83MHz and the timer prescaler set to divide by
 * 1024.
 */
#define TIMER_LOAD_VALUE 81050

/*------------------------------------------------------------------------------
 * GPIO instance
 */
gpio_instance_t g_gpio;

/*------------------------------------------------------------------------------
 * Timer 0 instance
 */
timer_instance_t g_core_timer_0;

/*------------------------------------------------------------------------------
 * Interrupt handler for the MIV_RV32 MSYS_EI interrupt connected to CoreTimer 0
 */
uint8_t
MSYS_EI0_IRQHandler(void)
{
    /* LEDs 1 - 4 are connected to GPIO pins 0 - 3.
     * Invert the state of GPIO pins 0 - 3 every time the interrupt is triggered
     */
    gpio_pins_state = gpio_pins_state ^ (GPIO_0_MASK | GPIO_1_MASK | GPIO_2_MASK | GPIO_3_MASK);

    GPIO_set_outputs(&g_gpio, gpio_pins_state);

    /* Clear the interrupt within the timer */
    TMR_clear_int(&g_core_timer_0);
    return (EXT_IRQ_KEEP_ENABLED);
}

int
main(void)
{
    UART_init(&g_core_uart_0, COREUARTAPB0_BASE_ADDR, BAUD_VALUE_115200, DATA_8_BITS | NO_PARITY);

    UART_polled_tx_string(&g_core_uart_0, g_message);

    /* Enable local interrupt for the MSYS_EI interrupt pin */
    MRV_enable_local_irq(MRV32_MSYS_EIE0_IRQn);

    /* Initialise the GPIO */
    GPIO_init(&g_gpio, COREGPIO_OUT_BASE_ADDR, GPIO_APB_32_BITS_BUS);

    /* Configure the GPIOs, turn them all off initially */
    GPIO_set_outputs(&g_gpio, 0x00u);

    /* Initialise and configure the timer */
    TMR_init(&g_core_timer_0,
             CORETIMER0_BASE_ADDR,
             TMR_ONE_SHOT_MODE,
             PRESCALER_DIV_1024,
             TIMER_LOAD_VALUE);

    /* Enable interrupts in general.*/
    HAL_enable_interrupts();

    /* Enable the timer to generate interrupts */
    TMR_enable_int(&g_core_timer_0);

    /* Start the timer */
    TMR_start(&g_core_timer_0);

    while (1u)
    {
        /* If the state of the gpio pins have been changed by the interrupt
         * handler, reload the timer tp start again .
         */
        if (gpio_pins_state != timer_reload_check)
        {
            TMR_reload(&g_core_timer_0, TIMER_LOAD_VALUE);
            timer_reload_check = gpio_pins_state;
        }
    }
    return 0u;
}
