/*******************************************************************************
 * Copyright 2019-2021 Microchip FPGA Embedded Systems Solutions.
 *
 * SPDX-License-Identifier: MIT
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to
 * deal in the Software without restriction, including without limitation the
 * rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
 * sell copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
 * IN THE SOFTWARE.
 * 
 * @file miv_rv32_hal.h
 * @author Microchip FPGA Embedded Systems Solutions
 * @brief Hardware Abstraction Layer functions for Mi-V soft processors
 *
 * SVN $Revision: 13158 $
 * SVN $Date: 2021-01-31 10:57:57 +0530 (Sun, 31 Jan 2021) $
 */

#ifndef RISCV_HAL_H
#define RISCV_HAL_H

#include "miv_rv32_regs.h"
#include "miv_rv32_plic.h"

#ifdef __cplusplus
extern "C" {
#endif

/***************************************************************************//**
 * Return value from External IRQ handler. This will be used to disable the
 * External interrupt.
 */
#define EXT_IRQ_KEEP_ENABLED                0U
#define EXT_IRQ_DISABLE                     1U

/***************************************************************************//**
 * Interrupt enable/disable.
 */
void MRV_enable_interrupts(void);
void MRV_disable_interrupts(void);


/***************************************************************************//**
 * System tick handler. This handler function gets called when the Machine
 * timer interrupt asserts. An implementation of this function should be
 * provided by the application to implement the application specific machine
 * timer interrupt handling. If application does not provide such implementation
 * the weakly linked handler stub function implemented in riscv_hal_stubs.c gets
 * linked.
 */
void SysTick_Handler(void);

/***************************************************************************//**
 * System timer tick configuration.
 * Configures the machine timer to generate a system tick interrupt at regular
 * intervals.
 * Takes the number of system clock ticks between interrupts.
 *
 * Returns 0 if successful.
 * Returns 1 if the interrupt interval cannot be achieved.
 */
uint32_t MRV_systick_config(uint32_t ticks);


#ifdef MIV_LEGACY_RV32
#define MSIP                                    (*(uint32_t*)0x44000000UL)
#define MTIMECMP                                (*(uint64_t*)0x44004000UL)
#define MTIME                                   (*(uint64_t*)0x4400BFF8UL)

/* To maintain backward compatibility with FreeRTOS config code */
#define PRCI_BASE                               0x44000000UL

#else

/* To maintain backward compatibility with FreeRTOS config code */
#define PRCI_BASE                               0x02000000UL

/* OPSRV stands for "Offload Processor Subsystem for RISC-V" (OPSRV) on the
 * MIV_RV32 IP core. Please see the handbook for more details. */

/* TCM ECC correctable err irq en mask value */
#define OPSRV_TCM_ECC_CE_IRQ                    0x01u

/* TCMECC uncorrectable err irq en */
#define OPSRV_TCM_ECC_UCE_IRQ                   0x02u

/* AXI write response err irq en */
#define OPSRV_AXI_WR_RESP_IRQ                   0x10u

#define OPSRV_BASE_ADDR                         0x00006000UL

typedef struct
{
    volatile uint32_t cfg;
    volatile uint32_t reserved0[3];
    volatile uint32_t irq_en;
    volatile uint32_t irq_pend;
    volatile uint32_t reserved1[2];
    volatile uint32_t soft_reg;
} OPSRV_Type;

#define OPSRV                           ((OPSRV_Type *)OPSRV_BASE_ADDR)

#define EXT_INTR_SOURCES                1

#define MTIMECMP                        (*(uint64_t*)0x02004000UL)

/* On MIV_RV32IMC v2.0 and v2.1 MTIME_PRESCALER is not defined and using this
 * definition will result in crash. For those core use the definition as below
 * #define MTIME_PRESCALER              100u
 */
#define MTIME_PRESCALER                 (*(uint32_t*)0x02005000UL)

#ifndef MIV_RV32_EXT_TIMER
#define MTIME                           (*(uint64_t*)0x0200BFF8UL)
#else
#define MTIME                           HAL_ASSERT(0);
#endif

/* These definitions are provided for convenient identification of the interrupts
 * in the MIE/MIP registers.
 * Apart from the standard software, timer and external interrupts, the names
 * of the additional interrupts correspond to the names as used in the MIV_RV32
 * handbook. Please refer the MIV_RV32 handbook for more details.
 * */
#define MIVRV32_SOFT_IRQn               MIE_3_IRQn
#define MIVRV32_TIMER_IRQn              MIE_7_IRQn
#define MIVRV32_EXT_IRQn                MIE_11_IRQn

#ifndef MIV_LEGACY_RV32
#define MIVRV32_MGEUIE_IRQn             MIE_16_IRQn
#define MIVRV32_MGECIE_IRQn             MIE_17_IRQn
#define MIVRV32_MSYS_EIE0_IRQn          MIE_24_IRQn
#define MIVRV32_MSYS_EIE1_IRQn          MIE_25_IRQn
#define MIVRV32_MSYS_EIE2_IRQn          MIE_26_IRQn
#define MIVRV32_MSYS_EIE3_IRQn          MIE_27_IRQn
#define MIVRV32_MSYS_EIE4_IRQn          MIE_28_IRQn
#define MIVRV32_MSYS_EIE5_IRQn          MIE_29_IRQn
#define MIVRV32_MSYS_OPSRV_IRQn         MIE_30_IRQn

/***************************************************************************//**
    Enable OPSRV interrupt. Parameter takes logical OR of following values
    #define OPSRV_TCM_ECC_CE_IRQ                    0x01u
    #define OPSRV_TCM_ECC_UCE_IRQ                   0x02u
    #define OPSRV_AXI_WR_RESP_IRQ                   0x10u
 */
static inline void MRV_enable_opsrv_irq(uint32_t irq_mask)
{
    OPSRV->irq_en = irq_mask;
}

/***************************************************************************//**
    Disable OPSRV interrupt. Parameter takes logical OR of following values
    #define OPSRV_TCM_ECC_CE_IRQ                    0x01u
    #define OPSRV_TCM_ECC_UCE_IRQ                   0x02u
    #define OPSRV_AXI_WR_RESP_IRQ                   0x10u
 */
static inline void MRV_disable_opsrv_irq(uint32_t irq_mask)
{
    OPSRV->cfg &= ~irq_mask;
}

/***************************************************************************//**
    Clear OPSRV interrupt. Parameter takes logical OR of following values
    #define OPSRV_TCM_ECC_CE_IRQ                    0x01u
    #define OPSRV_TCM_ECC_UCE_IRQ                   0x02u
    #define OPSRV_AXI_WR_RESP_IRQ                   0x10u
 */
static inline void MRV_OPSRV_clear_irq(uint32_t irq_mask)
{
    OPSRV->irq_pend |= irq_mask;
}

#endif

/*==============================================================================
 * Interrupt numbers:
 * This enum represents the interrupt enable bits in the MIE register.
 */
typedef enum
{
    MIE_0_IRQn  =  (0x01u),
    MIE_1_IRQn  =  (0x01u<<1u),
    MIE_2_IRQn  =  (0x01u<<2u),
    MIE_3_IRQn  =  (0x01u<<3u),         /*MSIE*/
    MIE_4_IRQn  =  (0x01u<<4u),
    MIE_5_IRQn  =  (0x01u<<5u),
    MIE_6_IRQn  =  (0x01u<<6u),
    MIE_7_IRQn  =  (0x01u<<7u),         /*MTIE*/
    MIE_8_IRQn  =  (0x01u<<8u),
    MIE_9_IRQn  =  (0x01u<<9u),
    MIE_10_IRQn =  (0x01u<<10u),
    MIE_11_IRQn =  (0x01u<<11u),        /*MEIE*/
    MIE_12_IRQn =  (0x01u<<12u),
    MIE_13_IRQn =  (0x01u<<13u),
    MIE_14_IRQn =  (0x01u<<14u),
    MIE_15_IRQn =  (0x01u<<15u),
    MIE_16_IRQn =  (0x01u<<16u),        /*MGEUIE*/
    MIE_17_IRQn =  (0x01u<<17u),        /*MGECIE*/
    MIE_18_IRQn =  (0x01u<<18u),
    MIE_19_IRQn =  (0x01u<<19u),
    MIE_20_IRQn =  (0x01u<<20u),
    MIE_21_IRQn =  (0x01u<<21u),
    MIE_22_IRQn =  (0x01u<<22u),
    MIE_23_IRQn =  (0x01u<<23u),
    MIE_24_IRQn =  (0x01u<<24u),        /*MSYS_EIE0*/
    MIE_25_IRQn =  (0x01u<<25u),        /*MSYS_EIE1*/
    MIE_26_IRQn =  (0x01u<<26u),        /*MSYS_EIE2*/
    MIE_27_IRQn =  (0x01u<<27u),        /*MSYS_EIE3*/
    MIE_28_IRQn =  (0x01u<<28u),        /*MSYS_EIE4*/
    MIE_29_IRQn =  (0x01u<<29u),        /*MSYS_EIE5*/
    MIE_30_IRQn =  (0x01u<<30u)         /*OPSRV_IRQ_IE*/

} MRV_LOCAL_IRQn_Type;

/***************************************************************************//**
 * Enable interrupts.
 This function takes a mask value as input. All interrupts in the MIE register
 are enabled as per the set bits in the provided mask value.
 */
static inline void MRV_enable_local_irq(uint32_t mask)
{
    uint32_t reg = read_csr(mie);
    reg |= mask;
    set_csr(mie, reg);
}

/***************************************************************************//**
 * disable interrupts.
 This function takes a mask value as input. All interrupts in the MIE register
 are disabled as per the set bits in the provided mask value.
 */
static inline void MRV_disable_local_irq(uint32_t mask)
{
    uint32_t reg = read_csr(mie);
    reg |= mask;
    clear_csr(mie, reg);
}
#endif

/***************************************************************************//**
 * The function MRV_raise_soft_irq() raises a synchronous software interrupt
 * by writing into the MSIP register.
 */
static inline void MRV_raise_soft_irq(void)
{
    set_csr(mie, MIP_MSIP);       /* Enable software interrupt bit */

#ifdef MIV_LEGACY_RV32
    /* You need to make sure that the global interrupt is enabled */
    MSIP = 0x01;   /* raise soft interrupt */
#else
    /* Raise soft IRQ on MIV_RV32 processor */
    OPSRV->soft_reg |= (1u << 1u);

#endif
}

/***************************************************************************//**
 * The function MRV_clear_soft_irq() clears a synchronous software interrupt
 * by clearing the MSIP register.
 */
static inline void MRV_clear_soft_irq(void)
{
#ifdef MIV_LEGACY_RV32
    MSIP = 0x00u;   /* clear soft interrupt */
#else
    /* Clear soft IRQ on MIV_RV32 processor */
    OPSRV->soft_reg &= ~(1u << 1u);
#endif
}

/***************************************************************************//**
 * The function MRV_read_mtime() returns the current MTIME register value.
 */
static inline uint64_t MRV_read_mtime(void)
{
#ifndef MIV_RV32_EXT_TIMER
    return(MTIME);
#endif
}

#ifdef __cplusplus
}
#endif

#endif  /* RISCV_HAL_H */

