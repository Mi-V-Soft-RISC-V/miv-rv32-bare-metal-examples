/*******************************************************************************
 * Copyright 2019-2023 Microchip FPGA Embedded Systems Solutions.
 *
 * SPDX-License-Identifier: MIT
 *
 * Common example project which can run on all Mi-V soft processor variants.
 * Please refer README.md in the root folder of this project for more details.
 *
 */
#include <stdio.h>
#include "hal/hal.h"
#include "miv_rv32_hal/miv_rv32_hal.h"
#include "drivers/fpga_ip/CoreGPIO/core_gpio.h"
#include "drivers/fpga_ip/CoreUARTapb/core_uart_apb.h"

const char * g_hello_msg =
"\r\n******************************************************************************\r\n\n\
********************    Mi-V System Timer Blinky Example    ***********************\r\n\n\
******************************************************************************\r\n\r\n\
Observe the LEDs blinking on the board. The LED pattern changes \
every time a system timer interrupt occurs.\r\n";

#define RX_BUFF_SIZE                64u
/*-----------------------------------------------------------------------------
 * UART instance data.
 */
UART_instance_t g_uart;
uint8_t g_rx_buff[RX_BUFF_SIZE] = {0u};
volatile uint8_t g_rx_size =   0u;
uint8_t g_ui_buf[100u] = {0u};

/*-----------------------------------------------------------------------------
 * GPIO instance data.
 */
gpio_instance_t g_gpio_out;

/*-----------------------------------------------------------------------------
 * Interrupt handlers.
 * Weakly linked default versions of all the interrupt handlers are defined
 * in miv_rv32_stubs.c.
 */
void Software_IRQHandler()
{
    MRV_clear_soft_irq();
}

void SysTick_Handler(void)
{
    static volatile uint32_t val = 0u;
    val ^= 0xFu;
    GPIO_set_outputs(&g_gpio_out, val);
    printf("\r\nInternal System Timer Interrupt");
}

/*-------------------------------------------------------------------------//**
  main() function.
*/
int main(void)
{
    uint8_t rx_char;
    uint8_t rx_count;
    uint32_t switches;

    UART_init(&g_uart,
              COREUARTAPB0_BASE_ADDR,
              BAUD_VALUE_115200,
              (DATA_8_BITS | NO_PARITY));

    printf(g_hello_msg);

    /* Initializing GPIOs */
    GPIO_init(&g_gpio_out, COREGPIO_OUT_BASE_ADDR, GPIO_APB_32_BITS_BUS);

    /* The CoreGPIO IP instantiated in the reference designs provided on github
     * is configured to have GPIO_0 to GPIO_3 ports as outputs.
     * This configuration can not be changed by the firmware since it is fixed in
     * the CoreGPIO IP instance. In your Libero design if you do not make
     * the GPIO configurations 'fixed', then you will need to configure them
     * using GPIO_config() function
	 * 
	 * However, the Renode currently doesn't support 'fixed' configurations on 
	 * CoreGPIO and therefore the GPIO's have to be configured at runtime. 
	 * Hence, uncomment following lines when targeting Renode:
     * 
	 * GPIO_config(&g_gpio_out, 0, GPIO_OUTPUT_MODE);
     * GPIO_config(&g_gpio_out, 1, GPIO_OUTPUT_MODE);
	 */

    /* set the output value */
    GPIO_set_outputs(&g_gpio_out, 0x0u);

    /* This must be done for all Mi-V cores to enable interrupts globally. */
    HAL_enable_interrupts();

#ifndef MIV_LEGACY_RV32
    MRV_enable_local_irq(MRV32_MSYS_EIE0_IRQn | MRV32_MSYS_EIE1_IRQn | MRV32_MSYS_EIE2_IRQn);
#endif

    MRV_systick_config(SYS_CLK_FREQ);

    /**************************************************************************
    * Loop
    *************************************************************************/
    do
    {
        g_rx_size = UART_get_rx(&g_uart, g_rx_buff, sizeof(g_rx_buff));

        if (g_rx_size > 0u)
        {
            /* Echo the characters received from the terminal */
            UART_polled_tx_string(&g_uart, (const uint8_t *)g_rx_buff);
            g_rx_size = 0u;
        }

    } while (1);

    return 0u;
}
