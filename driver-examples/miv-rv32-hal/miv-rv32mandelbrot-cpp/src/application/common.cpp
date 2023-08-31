/*******************************************************************************
 * Copyright 2023 Microchip FPGA Embedded Systems Solutions.
 *
 * SPDX-License-Identifier: MIT
 *
 * @file common.cpp
 * @author Microchip FPGA Embedded Systems Solutions
 * @brief contains the common functions used by both the demos
 *
 */

#include "common.hpp"

#ifdef FAST_FLOAT
float fast_square_root(const float input)
{
    float output;
    asm("fsqrt.s %0, %1": "=f" (output) : "f" (input));
    return output;
}

float fast_fmaxf(float x, float y)
{
      float ret;
      asm( "fmax.s %0, %1, %2": "=f" (ret) : "f" (x) , "f" (y));
      return ret;
}

float fast_fminf(float x, float y)
{
      float ret;
      asm( "fmin.s %0, %1, %2": "=f" (ret) : "f" (x) , "f" (y));
      return ret;
}
#endif


