/*******************************************************************************
 * Copyright 2019-2021 Microchip FPGA Embedded Systems Solutions.
 *
 * SPDX-License-Identifier: MIT
 *
 * @file cpu_types.h
 * @author Microchip FPGA Embedded Systems Solutions
 * @brief Type definitions which can be  commonly used by the fabric-ip drivers.
 * 
 */
#ifndef __CPU_TYPES_H
#define __CPU_TYPES_H   1

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef unsigned int size_t;

/*------------------------------------------------------------------------------
 * addr_t: address type.
 * Used to specify the address of peripherals present in the processor's memory
 * map.
 */
typedef unsigned long addr_t;

/*------------------------------------------------------------------------------
 * psr_t: processor state register.
 * Used by HAL_disable_interrupts() and HAL_restore_interrupts() to store the
 * processor's state between disabling and restoring interrupts.
 */
typedef unsigned long psr_t;

#ifdef __cplusplus
}
#endif

#endif  /* CPU_TYPES_H */

