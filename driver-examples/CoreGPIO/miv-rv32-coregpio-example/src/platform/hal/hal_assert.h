/*******************************************************************************
 * Copyright 2019-2021 Microchip FPGA Embedded Systems Solutions.
 *
 * SPDX-License-Identifier: MIT
 * 
 * SVN $Revision: 13158 $
 * SVN $Date: 2021-01-31 10:57:57 +0530 (Sun, 31 Jan 2021) $
 */
#ifndef __HAL_ASSERT_HEADER
#define __HAL_ASSERT_HEADER

#ifdef __cplusplus
extern "C" {
#endif
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
            __asm volatile ("ebreak"); \
        }\
    } while(0);

#endif  /* NDEBUG */

#ifdef __cplusplus
}
#endif
#endif  /* __HAL_ASSERT_HEADER */

