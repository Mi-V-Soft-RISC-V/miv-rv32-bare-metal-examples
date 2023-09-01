/*******************************************************************************
 * Copyright 2019-2021 Microchip FPGA Embedded Systems Solutions.
 *
 * SPDX-License-Identifier: MIT
 *
 * Application demonstrating the use of CoreGPIO driver. Please see the
 * Readme.md for more details.
 */

#include <drivers/fpga_ip/CoreGPIO/core_gpio.h>
#include <drivers/fpga_ip/CoreUARTapb/core_uart_apb.h>
#include "fpga_design_config/hw_platform.h"
#include "miv_rv32_hal/miv_rv32_hal.h"

/*******************************************************************************
 * Types, Constants and Greetings messages
 *******************************************************************************/

#define APB_BUS_WIDTH   GPIO_APB_32_BITS_BUS
#define RX_BUFF_SIZE    64U
#define HIGH            1u
#define LOW             0U

gpio_instance_t g_gpio;
UART_instance_t g_uart;

uint8_t g_message[] =
"\r\n\r\n\
******************************************************************************\r\n\
*******************  CoreGPIO Example Project ********************************\r\n\
******************************************************************************\r\n\
This example project demonstrates the use of the CoreGPIO driver\r\n\
with Mi-V soft processor.\r\n\
observe LED1 and LED2 on the board. LED1 and LED2 will blink simultaneously\r\n\
after a predefined delay. \r\n\n";

/* delay in ms */
static void delay(uint8_t t_in_ms);

/*******************************************************************************
 * main
 *******************************************************************************/
void main()
{
    /* LED state variable */
    uint8_t LED1_state = HIGH;
    uint8_t LED2_state = LOW;

     /* Initialize UART and GPIO instances */
    UART_init( &g_uart, COREUARTAPB0_BASE_ADDR,\
            BAUD_VALUE_115200, (DATA_8_BITS | NO_PARITY) );

    GPIO_init(&g_gpio, COREGPIO_OUT_BASE_ADDR, APB_BUS_WIDTH);

    /* Greeting message */
    UART_polled_tx_string(&g_uart,g_message);

    /* The CoreGPIO IP instantiated in the reference designs provided on github
     * is configured to have GPIO_0 to GPIO_3 ports as outputs.
     * This configuration can not be changed by the firmware since it is fixed in
     * the CoreGPIO IP instance. In your Libero design, if you do not make
     * the GPIO configurations 'fixed', then you will need to configure them
     * using GPIO_config() function.
     *
     * The Renode GPIO instances do not use 'fixed' configurations and
     * therefore these GPIO's have to be configured at runtime.
     * Please uncomment following lines when targeting Renode:
     */
     // GPIO_config(&g_gpio, 0, GPIO_OUTPUT_MODE);
     // GPIO_config(&g_gpio, 1, GPIO_OUTPUT_MODE);

    /* Set GPIO LED state */
    GPIO_set_output(&g_gpio, GPIO_1, LED1_state );
    GPIO_set_output(&g_gpio, GPIO_0, LED2_state );

    while(1)
    {

        /* Toggle LEDS indefinitely */
        delay(5);

        LED1_state = (~LED1_state & 0b00000001) ;
        LED2_state = (~LED2_state & 0b00000001) ;

        GPIO_set_output(&g_gpio, GPIO_1, LED1_state );
        GPIO_set_output(&g_gpio, GPIO_0, LED2_state );

        delay(5);
    }
}

/* delay in ms */
static void delay(uint8_t t_in_ms)
{
    uint32_t loopcount = (t_in_ms * SYS_CLK_FREQ)/1000U;
    for (volatile int count  = 0 ; count < loopcount ; count++)
    {

    }
}
