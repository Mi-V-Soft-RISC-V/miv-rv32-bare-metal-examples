/*******************************************************************************
 * Copyright 2022 Microchip FPGA Embedded Systems Solutions.
 *
 * SPDX-License-Identifier: MIT
 *
 * This file contains functions used for MIV_I2C driver interrupt control.
 * User should enable and disable the interrupts according to their design.
 * Please refer to miv_i2c.h file for more information.
 */

#include "miv_rv32_hal/miv_rv32_hal.h"

void MIV_I2C_disable_irq()
{
/* Disable I2C interrupt */
    MRV_disable_local_irq(MRV32_MSYS_EIE2_IRQn);
}

void MIV_I2C_enable_irq()
{
/* Enable I2C interrupt */
    MRV_enable_local_irq(MRV32_MSYS_EIE2_IRQn);
}


