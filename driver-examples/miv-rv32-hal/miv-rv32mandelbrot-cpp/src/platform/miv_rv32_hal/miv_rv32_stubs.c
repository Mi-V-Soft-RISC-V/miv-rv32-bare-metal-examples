/*******************************************************************************
 * Copyright 2019 Microchip FPGA Embedded Systems Solutions.
 *
 * SPDX-License-Identifier: MIT
 *
 * @file miv_rv32_stubs.c
 * @author Microchip FPGA Embedded Systems Solutions
 * @brief Stubs for the Mi-V soft processor Interrupt handler.
 *
 * The functions below will only be linked with the application code if the user
 * does not provide an implementation for these functions. These functions are
 * defined with weak linking so that they can be overridden by a function with
 * same prototype in the user's application code.
 *
 */
#include <unistd.h>

#ifdef __cplusplus
extern "C" {
#endif

__attribute__((weak)) void Software_IRQHandler(void)
{
    _exit(10);
}

__attribute__((weak)) void SysTick_Handler(void)
{
    /* Default handler */
}

#ifdef MIV_LEGACY_RV32
__attribute__((weak)) uint8_t Invalid_IRQHandler(void)
{
    return(0U); /* Default handler */
}

__attribute__((weak)) uint8_t External_1_IRQHandler(void)
{
    return(0U); /* Default handler */
}

__attribute__((weak)) uint8_t External_2_IRQHandler(void)
{
    return(0U); /* Default handler */
}

__attribute__((weak)) uint8_t External_3_IRQHandler(void)
{
    return(0U); /* Default handler */
}

__attribute__((weak)) uint8_t External_4_IRQHandler(void)
{
    return(0U); /* Default handler */
}

__attribute__((weak)) uint8_t External_5_IRQHandler(void)
{
    return(0U); /* Default handler */
}

__attribute__((weak)) uint8_t External_6_IRQHandler(void)
{
    return(0U); /* Default handler */
}

__attribute__((weak)) uint8_t External_7_IRQHandler(void)
{
    return(0U); /* Default handler */
}

__attribute__((weak)) uint8_t External_8_IRQHandler(void)
{
    return(0U); /* Default handler */
}

__attribute__((weak)) uint8_t External_9_IRQHandler(void)
{
    return(0U); /* Default handler */
}

__attribute__((weak)) uint8_t External_10_IRQHandler(void)
{
    return(0U); /* Default handler */
}

__attribute__((weak)) uint8_t External_11_IRQHandler(void)
{
    return(0U); /* Default handler */
}

__attribute__((weak)) uint8_t External_12_IRQHandler(void)
{
    return(0U); /* Default handler */
}

__attribute__((weak)) uint8_t External_13_IRQHandler(void)
{
    return(0U); /* Default handler */
}

__attribute__((weak)) uint8_t External_14_IRQHandler(void)
{
    return(0U); /* Default handler */
}

__attribute__((weak)) uint8_t External_15_IRQHandler(void)
{
    return(0U); /* Default handler */
}

__attribute__((weak)) uint8_t External_16_IRQHandler(void)
{
    return(0U); /* Default handler */
}

__attribute__((weak)) uint8_t External_17_IRQHandler(void)
{
    return(0U); /* Default handler */
}

__attribute__((weak)) uint8_t External_18_IRQHandler(void)
{
    return(0U); /* Default handler */
}

__attribute__((weak)) uint8_t External_19_IRQHandler(void)
{
    return(0U); /* Default handler */
}

__attribute__((weak)) uint8_t External_20_IRQHandler(void)
{
    return(0U); /* Default handler */
}

__attribute__((weak)) uint8_t External_21_IRQHandler(void)
{
    return(0U); /* Default handler */
}

__attribute__((weak)) uint8_t External_22_IRQHandler(void)
{
    return(0U); /* Default handler */
}

__attribute__((weak)) uint8_t External_23_IRQHandler(void)
{
    return(0U); /* Default handler */
}

__attribute__((weak)) uint8_t External_24_IRQHandler(void)
{
    return(0U); /* Default handler */
}

__attribute__((weak)) uint8_t External_25_IRQHandler(void)
{
    return(0U); /* Default handler */
}

__attribute__((weak)) uint8_t External_26_IRQHandler(void)
{
    return(0U); /* Default handler */
}

__attribute__((weak)) uint8_t External_27_IRQHandler(void)
{
    return(0U); /* Default handler */
}

__attribute__((weak)) uint8_t External_28_IRQHandler(void)
{
    return(0U); /* Default handler */
}

__attribute__((weak)) uint8_t External_29_IRQHandler(void)
{
    return(0U); /* Default handler */
}

__attribute__((weak)) uint8_t External_30_IRQHandler(void)
{
    return(0U); /* Default handler */
}

__attribute__((weak)) uint8_t External_31_IRQHandler(void)
{
    return(0U); /* Default handler */
}

#else
__attribute__((weak)) void External_IRQHandler(void)
{
}
__attribute__((weak)) void MGECI_IRQHandler(void)
{
}
__attribute__((weak)) void MGEUI_IRQHandler(void)
{
}
__attribute__((weak)) void SUBSYS_IRQHandler(void)
{
}
__attribute__((weak)) void MSYS_EI0_IRQHandler(void)
{
}
__attribute__((weak)) void MSYS_EI1_IRQHandler(void)
{
}
__attribute__((weak)) void MSYS_EI2_IRQHandler(void)
{
}
__attribute__((weak)) void MSYS_EI3_IRQHandler(void)
{
}
__attribute__((weak)) void MSYS_EI4_IRQHandler(void)
{
}
__attribute__((weak)) void MSYS_EI5_IRQHandler(void)
{
}
__attribute__((weak)) void Reserved_IRQHandler(void)
{
    _exit(10);
}
#ifndef MIV_RV32_V3_0 /* For MIV_RV32 v3.0 */
__attribute__((weak)) void MSYS_EI6_IRQHandler(void)
{
}
__attribute__((weak)) void MSYS_EI7_IRQHandler(void)
{
}
__attribute__((weak)) void SUBSYSR_IRQHandler(void)
{
}
#endif  /* MIV_RV32_V3_0 */
#endif  /* MIV_LEGACY_RV32 */

#ifdef __cplusplus
}
#endif
