/*******************************************************************************
 * Copyright 2019-2023 Microchip FPGA Embedded Systems Solutions.
 *
 * SPDX-License-Identifier: MIT
 *
 * @file hal_assert.h
 * @author Microchip FPGA Embedded Systems Solutions
 * @brief HAL assert functions
 */
#ifndef __HAL_ASSERT_HEADER
#define __HAL_ASSERT_HEADER 1

#ifdef __cplusplus
extern "C" {
#endif

/* Disable assertions if we do not recognize the compiler. */
#if defined ( __GNUC__ )
#if defined(NDEBUG)
/***************************************************************************//**
 * HAL_ASSERT() is defined out when the NDEBUG symbol is used.
 ******************************************************************************/
#define HAL_ASSERT(CHECK)

#else

/***************************************************************************//**
 * Default behavior for HAL_ASSERT() macro:
 *------------------------------------------------------------------------------
  The behavior is toolchain specific and project setting specific.
 ******************************************************************************/
#define HAL_ASSERT(CHECK)\
    do { \
        if (!(CHECK)) \
        { \
            __asm__ volatile ("ebreak"); \
        }\
    } while(0);

#endif  /* NDEBUG */
#endif  /*__GNUC__*/

#ifdef __cplusplus
}
#endif
#endif  /* __HAL_ASSERT_HEADER */

