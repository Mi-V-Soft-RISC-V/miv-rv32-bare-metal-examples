
/*******************************************************************************
 *  Copyright 2018 Microchip Corporation.
 *  All rights reserved.
 *
 *  Configure ZL30722 device
 *
 * SVN $Revision: $
 * SVN $Date: $
 */

#include "fpga_design_config/fpga_design_config.h"
#include "drivers/fpga_ip/CoreGPIO/core_gpio.h"
#include "ZL30722.h"
#include "ZL30722_image.h"

#define ZL_RST  GPIO_2 // led3
#define ZL_AC_0 GPIO_1 // led2
#define ZL_AC_1 GPIO_0 // led1 + Test

static gpio_instance_t g_gpio;

static void
delay(uint32_t timeout)
{
    uint32_t i = 0;
    while (i < timeout)
    {
        i++;
    }
}

void
configure_zl30722(void)
{
    /* This section of code configures the external clock on the Everest board, so the Phy and the
     * CoreTSE are in sync with each other.
     */
    uint8_t buff[100];
    int len = 100;

    GPIO_init(&g_gpio, COREGPIO_OUT_BASE_ADDR, GPIO_APB_32_BITS_BUS);

    /* Set GPIO LED state */
    GPIO_set_output(&g_gpio, ZL_RST, 0);
    GPIO_set_output(&g_gpio, ZL_AC_0, 0);
    GPIO_set_output(&g_gpio, ZL_AC_1, 0);

    delay(60000);
    GPIO_set_output(&g_gpio, ZL_RST, 1);
    delay(60000);
    ZL30722_SPI_init();

    ZL30722_read(buff, len);

    /* ZL30722 already configured? */
    ZL30722_compare(ZL30722_image, ZL30722_IMAGE_SIZE);
}
