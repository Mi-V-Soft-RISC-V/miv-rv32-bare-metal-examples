/*******************************************************************************
 * Copyright 2023 Microchip FPGA Embedded Systems Solutions.
 *
 * SPDX-License-Identifier: MIT
 * 
 * @file test-utils.h
 * @author Microchip FPGA Embedded Systems Solutions
 * @brief Test utility functions for GDB
 *
 */

#ifndef TEST_UTILS_H_
#define TEST_UTILS_H_

#ifdef __cplusplus
extern "C" {
#endif

#define CONFIGURATION_RISCV 1
#define CONFIGURATION_ARM 2
#define CONFIGURATION_HARDFLOAT 4
#define CONFIGURATION_OPTIMALIZATION_0 8
#define CONFIGURATION_OPTIMALIZATION_1 16
#define CONFIGURATION_OPTIMALIZATION_2 32
#define CONFIGURATION_OPTIMALIZATION_3 64

extern void testValidate(unsigned int iteration, unsigned int blocking);
extern void testAddToChecksumInt(unsigned int checksum);
extern void testAddToChecksumFloat(float value);

#ifdef __cplusplus
}
#endif

#endif /* TEST_UTILS_H_ */
