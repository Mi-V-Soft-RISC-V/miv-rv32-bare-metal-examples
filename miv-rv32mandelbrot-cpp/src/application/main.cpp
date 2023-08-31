/*******************************************************************************
 * Copyright 2023 Microchip FPGA Embedded Systems Solutions.
 *
 * SPDX-License-Identifier: MIT
 *
 * @file main.cpp
 * @author Microchip FPGA Embedded Systems Solutions
 * @brief C++ demo project which can run on the Renode Emulation Platform. 
 * 
 * This demo highlights the IMCF extensions of Mi-V soft processor IP.
 * Please refer README.md in the root folder of this project for more details.
 *
 */

#include "hal/hal.h"
#include "miv_rv32_hal/miv_rv32_hal.h"
#include "drivers/fpga_ip/CoreUARTapb/core_uart_apb.h"

#include "common.hpp"
#include "test-utils.h"

/***************************************************************************//**
 * Undefine the DEMO_MANDELBROT in the project settings to run raytracer 
 * demo instead.
*/

#ifdef DEMO_MANDELBROT
#include "mandelbrot.hpp"
#else
#include "raytracer.hpp"
#endif

int main()
{

#ifdef DEMO_MANDELBROT
  demoMandelbrot();
#else
  demoRaytracer();
#endif

  /* if GDB testing is enabled, then it will validate the checksums */ 
  testValidate(ITERATIONS, 1);

#ifndef EXIT_FROM_THE_INFINITE_LOOP
  while(1);
#endif

  return 0;  /* In case it's build as native application */
}
