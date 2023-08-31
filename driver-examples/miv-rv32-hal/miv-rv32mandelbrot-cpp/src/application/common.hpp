/*******************************************************************************
 * Copyright 2023 Microchip FPGA Embedded Systems Solutions.
 *
 * SPDX-License-Identifier: MIT
 *
 * @file common.hpp
 * @author Microchip FPGA Embedded Systems Solutions
 * @brief Contains the common functions used by both the demos
 *
 */

#ifndef SRC_APPLICATION_COMMON_HPP_
#define SRC_APPLICATION_COMMON_HPP_

#include <cfloat>
#include <cmath>
#include <stddef.h>


#define WIDTH  80             // Terminal's columns
#define HEIGHT 21             // Terminal's rows
#define SMOOTHNESS 20.5f      // Affects shininess of the sphere


#ifndef ITERATIONS
#define ITERATIONS 1          // How many times the same the frame should be repeated
#endif


// Raytracer
#ifndef ROTATION_STEPS
#define ROTATION_STEPS 36.0f  // More steps will make the movement smoother
#endif


// Mandelbrot
#ifndef ANIMATION_SPEED
#define ANIMATION_SPEED 0.01f // How large steps are done between the frames
#endif


#ifndef VT100_COLORS
#define VT100_COLORS 1        // Will use basic vt100 colors
#endif


// Can switch between newlib's sqrt and custom inline assembly sqrt
// https://gcc.gnu.org/onlinedocs/gcc/Using-Assembly-Language-with-C.html
#ifdef FAST_FLOAT
#define SQUARE_ROOT(x) fast_square_root(x)
#define MAXF(x, y) fast_fmaxf(x, y)
#define MINF(x, y) fast_fminf(x, y)
#else
#define SQUARE_ROOT(x) sqrtf(x)
#define MAXF(x, y) fmaxf(x, y)
#define MINF(x, y) fminf(x, y)
#endif


#define M_PI_F 3.14159265f    // Single floating point version of M_PI


// https://coderwall.com/p/nb9ngq/better-getting-array-size-in-c
template<size_t SIZE, class T> inline size_t array_size(T (&arr)[SIZE])
{
  return SIZE;
}


// https://stackoverflow.com/questions/37538
#define NELEMS(x) (sizeof(x) / sizeof((x)[0]))


#ifdef FAST_FLOAT
extern float fast_square_root(const float input);

extern float fast_fmaxf(float x, float y);

extern float fast_fminf(float x, float y);
#endif


#endif /* SRC_APPLICATION_COMMON_HPP_ */
