/*******************************************************************************
 * Copyright 2009-2023 Microchip FPGA Embedded Systems Solutions.
 *
 * SPDX-License-Identifier: MIT
 *
 * CoreI2C driver interrupt control.
 *
 */
#include "core_i2c.h"
#include "miv_rv32_hal/miv_rv32_hal.h"

extern i2c_instance_t g_core_i2c0;
extern i2c_instance_t g_core_i2c1;

/*------------------------------------------------------------------------------
 * This function must be modified to enable interrupts generated from the
 * CoreI2C instance identified as parameter.
 */
void I2C_enable_irq( i2c_instance_t * this_i2c )
{
    if(this_i2c == &g_core_i2c0)
    {
        MRV_enable_local_irq(MRV32_MSYS_EIE0_IRQn);
    }

    if(this_i2c == &g_core_i2c1)
    {
        MRV_enable_local_irq(MRV32_MSYS_EIE1_IRQn);
    }
}

/*------------------------------------------------------------------------------
 * This function must be modified to disable interrupts generated from the
 * CoreI2C instance identified as parameter.
 */
void I2C_disable_irq( i2c_instance_t * this_i2c )
{
    if(this_i2c == &g_core_i2c0)
    {
        MRV_disable_local_irq(MRV32_MSYS_EIE0_IRQn);
    }

    if(this_i2c == &g_core_i2c1)
    {
        MRV_disable_local_irq(MRV32_MSYS_EIE1_IRQn);
    }


}
