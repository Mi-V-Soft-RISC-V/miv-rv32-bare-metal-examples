/*******************************************************************************
 * Copyright 2023 Microchip FPGA Embedded Systems Solutions.
 *
 * SPDX-License-Identifier: MIT
 * 
 * @file test-utils.c
 * @author Microchip FPGA Embedded Systems Solutions
 * @brief Test utility functions for GDB
 *
 */
#include <stdio.h>
#include <float.h>
#include "test-utils.h"

#ifdef __cplusplus
extern "C" {
#endif


#ifdef GDB_TESTING
  unsigned int actualChecksum = 0;
  unsigned int current_configuration = 0;
#endif


// This variable can be set by different defines when using different compile
// configurations and then it can detected with GDB on runtime and distinguish
// what results the tests should use as the GDB test needs to have different
// expected results for soft/hard floats as they slightly differ in the implementation.
unsigned int getConfigurationState() {
#ifdef GDB_TESTING
  unsigned int ret = 0;

  ret |= CONFIGURATION_RISCV;
  ret |= CONFIGURATION_OPTIMALIZATION_0;

#ifdef __riscv_flen
  ret |= CONFIGURATION_HARDFLOAT;
#endif

  return ret;
#else
  return 0;
#endif
}


void testAddToChecksumInt(unsigned int checksum) {
#ifdef GDB_TESTING
  actualChecksum += checksum;
#endif
}


void testAddToChecksumFloat(float value) {
#ifdef GDB_TESTING
  actualChecksum += *(unsigned int*)&value;
#endif
}


void testValidateBreak(unsigned int iteration, unsigned int blocking) {
#ifdef GDB_TESTING
  // When testing with gdb, place breakpoint here
  volatile unsigned keepBlocking = blocking;
  printf("Test point reached \n");
  while (keepBlocking); // Feel free to unblock it on runtime if needed
#endif
}


void testValidate(unsigned int iteration, unsigned int blocking) {
#ifdef GDB_TESTING
  current_configuration = getConfigurationState();
  testValidateBreak(iteration, blocking);
#endif
}

#ifdef __cplusplus
}
#endif
