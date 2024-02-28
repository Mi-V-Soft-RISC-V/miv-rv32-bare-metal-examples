/***************************************************************************************************
 * Copyright 2023 Microchip FPGA Embedded Systems Solutions.
 *
 * SPDX-License-Identifier: MIT
 *
 * This example project demonstrates communications with an external flash device.
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

UART_instance_t g_core_uart_0;

uint8_t g_message[] =
    "\r\n\r\n\**************  Mi-V CoreTimer: Timer Interrupt Example Project  "
    "**************\r\n\r\n\r\nThis example project demonstrates the use of the CoreTimer "
    "driver operating in\r\ntimer interrupt mode\r\n";

/*--------------------------------------------------------------------------------------------------
 * Timer load value.
 * This value is calculated to result in the timer timing out after after 1 second with a system
 * clock of 83MHz and the timer prescaler set to divide by 1024.
 */
#define TIMER_LOAD_VALUE 500000

/*--------------------------------------------------------------------------------------------------
 * GPIO instance
 */
gpio_instance_t g_gpio;

/*--------------------------------------------------------------------------------------------------
 * Timer 0 instance
 */
timer_instance_t g_core_timer_0;

/*--------------------------------------------------------------------------------------------------
 * In the MIV_RV32 processor design, The CoreTimer interrupt is connected to the
 * MSYS_EI0 pin. There is no PLIC inside the MIV_RV32 processor. The CoreTimer 
 * interrupt is treated as local interrupt and the interrupt handler function, 
 * MSYS_EI0_IRQHandler(void), is invoked directly by the HAL. 
 * 
 * In the designs with legacy processor such as MiV_RV32IMA_L1_AHB, The CoreTimer 
 * interrupt is connected to External Interrupt pin 30 of the RV32IMA processor. 
 * It then gets rounted to the PLIC in-build in the RV32IMA processor. 
 * The interrupt handler function External_30_IRQHandler(void) is called by the 
 * PLIC/External interrupt handler in the HAL.
 */
#ifndef MIV_LEGACY_RV32
uint8_t MSYS_EI0_IRQHandler(void)
#else
uint8_t External_30_IRQHandler(void)
#endif
{
    /* WARNING!
     * In case of modifying this example project, be aware of the time it takes for the interrupt
     * handler to return and how long the timer is set for.
     * If the timer's countdown duration is shorter than the time it takes for the interrupt handler
     * to return, the program may never return and it may get stuck in the interrupt handler.
     * To safeguard against this event consider using the "TMR_stop()" and "TMR_start()" APIs to
     * stop the counter decrementing during the interrupt hander.
     */

    /* LEDs 1 - 4 are connected to GPIO pins 0 - 3.
     * Invert the state of GPIO pins 0 - 3 every time the interrupt is triggered
     */
    gpio_pins_state = gpio_pins_state ^ (GPIO_0_MASK | GPIO_1_MASK | GPIO_2_MASK | GPIO_3_MASK);
    UART_polled_tx_string(&g_core_uart_0, "\rTMR\n");

    GPIO_set_outputs(&g_gpio, gpio_pins_state);

    /* Clear the interrupt within the timer */
    TMR_clear_int(&g_core_timer_0);
    return (EXT_IRQ_KEEP_ENABLED);
}

int main(void)
{
    /* Initialise the UART and print the greeting message*/
    UART_init(&g_core_uart_0, COREUARTAPB0_BASE_ADDR, BAUD_VALUE_115200, DATA_8_BITS | NO_PARITY);

    UART_polled_tx_string(&g_core_uart_0, g_message);

#ifndef MIV_LEGACY_RV32
    /* Enable local interrupt for the MSYS_EI interrupt pin */
    MRV_enable_local_irq(MRV32_MSYS_EIE0_IRQn);

    /*
     * The legacy RV32IMA design uses External Interrupts and an in-built PLIC for interrupt handling.
     * To use this project with the Legacy RV32IMA designs, the following initializations
     * need to be used.
     *
     * First we need to enable the PLIC and then we can enable specific interrups which can be enabled
     * as per the requirement of the applicaiton. In this case, the CoreTimer0 is connected to external
     * interrupt 30, and has been enabled below.
    */
#else
    MRV_PLIC_init();
    MRV_PLIC_enable_irq(External_30_IRQn);
#endif


    /* Initialise the GPIO */
    GPIO_init(&g_gpio, COREGPIO_OUT_BASE_ADDR, GPIO_APB_32_BITS_BUS);

    /* Configure the GPIOs, turn them all off initially */
    GPIO_set_outputs(&g_gpio, 0x00u);

    /* Initialise and configure the timer */
    TMR_init(&g_core_timer_0,
             CORETIMER0_BASE_ADDR,
             TMR_CONTINUOUS_MODE,
             PRESCALER_DIV_256,
             TIMER_LOAD_VALUE);

    /* Enable interrupts in general.*/
    HAL_enable_interrupts();

    /* Enable the timer to generate interrupts */
    TMR_enable_int(&g_core_timer_0);

    /* Start the timer */
    TMR_start(&g_core_timer_0);

    while (1u)
    {
        ;
    }
    return 0u;
}
