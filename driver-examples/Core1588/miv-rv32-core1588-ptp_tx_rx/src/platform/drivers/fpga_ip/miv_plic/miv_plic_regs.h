 /*******************************************************************************
 * Copyright 2022 Microchip FPGA Embedded Systems Solutions.
 *
 * SPDX-License-Identifier: MIT
 *
 * This file contains Register bit offsets and masks definitions for MI-V Soft
 * IP PLIC module driver. This module is delivered as a part of Mi-V extended
 * Sub-System(MIV_ESS).
 */

#ifndef MIV_PLIC_REGISTERS
#define MIV_PLIC_REGISTERS   1

#ifdef __cplusplus
extern "C" {
#endif

/* Interrupt pending register offset */
#define INT_PENDING_REG_OFFSET                      0x1000u

/* Interrupt enable register */
#define INT_ENABLE_REG_OFFSET                       0x2000u

/* Interrupt claim complete register */
#define INT_CLAIM_COMPLETE_REG_OFFSET               0x200004u

#ifdef __cplusplus
}
#endif

#endif  /* MIV_PLIC_REGISTERS */
