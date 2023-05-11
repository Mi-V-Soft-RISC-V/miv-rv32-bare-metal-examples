/*******************************************************************************
 * Copyright 2022 Microchip FPGA Embedded Systems Solutions.
 *
 * SPDX-License-Identifier: MIT
 *
 * MIV_RV32 HAL Embedded Software
 *
 */
#ifndef MIV_RV32_ASSERT_HEADER
#define MIV_RV32_ASSERT_HEADER

#ifdef __cplusplus
extern "C" {
#endif

/***************************************************************************//**
 * ASSERT() implementation.
 ******************************************************************************/
/* Disable assertions if we do not recognize the compiler. */
#if defined ( __GNUC__ )
#if defined(NDEBUG)
#define ASSERT(CHECK)
#else
#define ASSERT(CHECK)\
    do { \
        if (!(CHECK)) \
        { \
            __asm__ volatile ("ebreak"); \
        }\
    } while(0);

#endif /* NDEBUG check */
#endif /* compiler check */

#ifdef __cplusplus
}
#endif

#endif  /* MIV_RV32_ASSERT_HEADER */

