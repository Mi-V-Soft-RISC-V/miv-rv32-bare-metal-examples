/*******************************************************************************
 * Copyright 2019-2022 Microchip FPGA Embedded Systems Solutions.
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
 */

#ifndef RISCV_HAL_H
#define RISCV_HAL_H

#include "miv_rv32_regs.h"
#include "miv_rv32_plic.h"

#ifndef LEGACY_DIR_STRUCTURE
#include "fpga_design_config/fpga_design_config.h"
#else
#include "hw_platform.h"
#endif

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
uint32_t MRV_systick_config(uint64_t ticks);


#ifdef MIV_LEGACY_RV32
#define MSIP                                    (*(uint32_t*)0x44000000UL)
#define MTIMECMP                                (*(uint32_t*)0x44004000UL)
#define MTIMECMPH                               (*(uint32_t*)0x44004004UL)
#define MTIME                                   (*(uint32_t*)0x4400BFF8UL)
#define MTIMEH                                  (*(uint32_t*)0x4400BFFCUL)

/* To maintain backward compatibility with FreeRTOS config code */
#define PRCI_BASE                               0x44000000UL

#else

/* To maintain backward compatibility with FreeRTOS config code */
#define PRCI_BASE                       0x02000000UL

/* OPSRV stands for "Offload Processor Subsystem for RISC-V" (OPSRV) on the
 * MIV_RV32 IP core. Please see the handbook for more details. */

/* TCM ECC correctable error irq enable mask value */
#define OPSRV_TCM_ECC_CE_IRQ            0x01u

/* TCMECC uncorrectable error irq enable */
#define OPSRV_TCM_ECC_UCE_IRQ           0x02u

/* AXI write response error irq enable */
#define OPSRV_AXI_WR_RESP_IRQ           0x10u

#define OPSRV_BASE_ADDR                 0x00006000UL

typedef struct
{
    volatile uint32_t cfg;          	/*Parity is not being supported by MIV_RV32 v3.0*/
    volatile uint32_t reserved0[3];
    volatile uint32_t irq_en;           /*offset 0x10*/
    volatile uint32_t irq_pend;
    volatile uint32_t reserved1[2];
    volatile uint32_t soft_reg;         /*offset 0x20*/
} OPSRV_Type;

#define OPSRV                           ((OPSRV_Type *)OPSRV_BASE_ADDR)

#define EXT_INTR_SOURCES                1

#define MTIMECMP                        (*(volatile uint32_t*)0x02004000UL)
#define MTIMECMPH                       (*(volatile uint32_t*)0x02004004UL)

/* On MIV_RV32IMC v2.0 and v2.1 MTIME_PRESCALER is not defined and using this
 * definition will result in crash. For those core use the definition as below
 * #define MTIME_PRESCALER              100u
 */
#define MTIME_PRESCALER                 (*(volatile uint32_t*)0x02005000UL)

#ifndef MIV_RV32_EXT_TIMER
#define MTIME                           (*(volatile uint32_t*)0x0200BFF8UL)
#define MTIMEH                          (*(volatile uint32_t*)0x0200BFFCUL)
#else
#define MTIME                           HAL_ASSERT(0);
#endif

/* These definitions are provided for convenient identification of the interrupts
 * in the MIE/MIP registers.
 * Apart from the standard software, timer and external interrupts, the names
 * of the additional interrupts correspond to the names as used in the MIV_RV32
 * handbook. Please refer the MIV_RV32 handbook for more details.
 * */
#define MRV32_SOFT_IRQn                 MIE_3_IRQn
#define MRV32_TIMER_IRQn                MIE_7_IRQn
#define MRV32_EXT_IRQn                  MIE_11_IRQn

/*==============================================================================
 * Interrupt numbers:
 * This enum represents the interrupt enable bits in the MIE register.
 */
enum
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

#ifndef MIV_LEGACY_RV32
#define MRV32_MGEUIE_IRQn               MIE_16_IRQn
#define MRV32_MGECIE_IRQn               MIE_17_IRQn
#define MRV32_MSYS_EIE0_IRQn            MIE_24_IRQn
#define MRV32_MSYS_EIE1_IRQn            MIE_25_IRQn
#define MRV32_MSYS_EIE2_IRQn            MIE_26_IRQn
#define MRV32_MSYS_EIE3_IRQn            MIE_27_IRQn
#define MRV32_MSYS_EIE4_IRQn            MIE_28_IRQn
#define MRV32_MSYS_EIE5_IRQn            MIE_29_IRQn
#define MRV32_MSYS_OPSRV_IRQn           MIE_30_IRQn

/***************************************************************************//**
    Enable OPSRV interrupt. Parameter takes logical OR of following values
    #define OPSRV_TCM_ECC_CE_IRQ                    0x01u
    #define OPSRV_TCM_ECC_UCE_IRQ                   0x02u
    #define OPSRV_AXI_WR_RESP_IRQ                   0x10u
 */
static inline void MRV32_opsrv_enable_irq(uint32_t irq_mask)
{
    OPSRV->irq_en = irq_mask;
}

/***************************************************************************//**
    Disable OPSRV interrupt. Parameter takes logical OR of following values
    #define OPSRV_TCM_ECC_CE_IRQ                    0x01u
    #define OPSRV_TCM_ECC_UCE_IRQ                   0x02u
    #define OPSRV_AXI_WR_RESP_IRQ                   0x10u
 */
static inline void MRV32_opsrv_disable_irq(uint32_t irq_mask)
{
    OPSRV->irq_en &= ~irq_mask;
}

/***************************************************************************//**
    Clear OPSRV interrupt. Parameter takes logical OR of following values
    #define OPSRV_TCM_ECC_CE_IRQ                    0x01u
    #define OPSRV_TCM_ECC_UCE_IRQ                   0x02u
    #define OPSRV_AXI_WR_RESP_IRQ                   0x10u
 */
static inline void MRV32_opsrv_clear_irq(uint32_t irq_mask)
{
    OPSRV->irq_pend |= irq_mask;
}

/***************************************************************************//**
 * The function MRV32_is_gpr_ded() returns the core_gpr_ded_reset_reg bit value.
 * When ECC is enabled, the core_gpr_ded_reset_reg is set when the core was
 * reset due to GPR DED error.
 */
static inline uint32_t MRV32_is_gpr_ded(void)
{
    return((OPSRV->soft_reg & 0x04u) >> 0x02u);
}

/***************************************************************************//**
 * The function MRV32_clear_gpr_ded() can be used to clear the
 * core_gpr_ded_reset_reg bit. When ECC is enabled, the core_gpr_ded_reset_reg
 * is set when the core was previously reset due to GPR DED error.
 */
static inline void MRV32_clear_gpr_ded(void)
{
    OPSRV->soft_reg &= ~0x04u;
}

/***************************************************************************//**
 * The function MRV32_enable_parity_check() is used to enable parity check on
 * the TCM and it's interface transactions. This feature is not available on
 * MIV_RV32 v3.0.100 soft processor core.
 */
static inline void MRV32_enable_parity_check(void)
{
    OPSRV->cfg |= 0x01u;
}

/***************************************************************************//**
 * The function MRV32_disable_parity_check() is used to disable parity check on
 * the TCM and it's interface transactions.
 */
static inline void MRV32_disable_parity_check(void)
{
    OPSRV->cfg &= ~0x01u;
}

/***************************************************************************//**
 * The function MRV32_cpu_soft_reset() is used to cause a soft cpu reset on
 * the MIV_RV32 soft processor core.
 */
static inline void MRV32_cpu_soft_reset(void)
{
    OPSRV->soft_reg &= ~0x01u;
}

/***************************************************************************//**
    Clear GPR ECC Uncorrectable interrupt. MGEUI interrupt is available only when
    ECC is enabled in MIV_RV32 IP configurator.
 */
static inline void MRV32_mgeui_clear_irq(uint32_t irq_mask)
{
    clear_csr(mip, MRV32_MGEUIE_IRQn);
}

/***************************************************************************//**
    Clear GPR ECC correctable interrupt. MGECI interrupt is available only when
    ECC is enabled in MIV_RV32 IP configurator.
 */
static inline void MRV32_mgeci_clear_irq(uint32_t irq_mask)
{
    clear_csr(mip, MRV32_MGECIE_IRQn);
}

/***************************************************************************//**
  When ECC is enabled for the GPRs and if that data has a single bit error then
  the data coming out of the ECC block will be corrected and will not have the
  error but the data source will still have the error.
  The ECC block does not write back corrected data to memory.
  Therefore, if data has a single bit error, then the corrected data should be 
  written back to prevent the single bit error from becoming a double bit error.
  The MRV32_clear_gpr_ecc_errors() can be used for that.

  Clear the pending interrupt bit after this using MRV32_mgeci_clear_irq()
  function to complete the ECC error handling.
 */
static inline void MRV32_clear_gpr_ecc_errors(void)
{
    uint32_t temp;

    __asm__ __volatile__ (
            "sw x31, %0"
            :"=m" (temp));

    __asm__ volatile (
            "mv x31, x1;"
            "mv x1, x31;"

            "mv x31, x2;"
            "mv x2, x31;"

            "mv x31, x3;"
            "mv x3, x31;"

            "mv x31, x4;"
            "mv x4, x31;"

            "mv x31, x5;"
            "mv x5, x31;"

            "mv x31, x6;"
            "mv x6, x31;"

            "mv x31, x7;"
            "mv x7, x31;"

            "mv x31, x8;"
            "mv x8, x31;"

            "mv x31, x9;"
            "mv x9, x31;"

            "mv x31, x10;"
            "mv x10, x31;"

            "mv x31, x11;"
            "mv x11, x31;"

            "mv x31, x12;"
            "mv x12, x31;"

            "mv x31, x13;"
            "mv x13, x31;"

            "mv x31, x14;"
            "mv x14, x31;"

            "mv x31, x15;"
            "mv x15, x31;"

            "mv x31, x16;"
            "mv x16, x31;"

            "mv x31, x17;"
            "mv x17, x31;"

            "mv x31, x18;"
            "mv x18, x31;"

            "mv x31, x19;"
            "mv x19, x31;"

            "mv x31, x20;"
            "mv x20, x31;"

            "mv x31, x21;"
            "mv x21, x31;"

            "mv x31, x22;"
            "mv x22, x31;"

            "mv x31, x23;"
            "mv x23, x31;"

            "mv x31, x24;"
            "mv x24, x31;"

            "mv x31, x25;"
            "mv x25, x31;"

            "mv x31, x26;"
            "mv x26, x31;"

            "mv x31, x27;"
            "mv x27, x31;"

            "mv x31, x28;"
            "mv x28, x31;"

            "mv x31, x29;"
            "mv x29, x31;"

            "mv x31, x30;"
            "mv x30, x31;");

    __asm__ __volatile__ (
            "lw x31, %0;"
            :
            :"m" (temp));
}

#endif

/***************************************************************************//**
 * Enable interrupts.
 This function takes a mask value as input. For each set bit in the mask value,
 corresponding interrupt bit in the MIE register is enabled.

 MRV_enable_local_irq(MRV32_SOFT_IRQn  |
                      MRV32_TIMER_IRQn |
                      MRV32_EXT_IRQn   |
                      MRV32_MSYS_EIE0_IRQn |
                      MRV32_MSYS_OPSRV_IRQn);
 */
static inline void MRV_enable_local_irq(uint32_t mask)
{
    set_csr(mie, mask);
}

/***************************************************************************//**
 * Disable interrupts.
 This function takes a mask value as input. For each set bit in the mask value,
 corresponding interrupt bit in the MIE register is disabled.

  MRV_disable_local_irq(MRV32_SOFT_IRQn  |
                        MRV32_TIMER_IRQn |
                        MRV32_EXT_IRQn   |
                        MRV32_MSYS_EIE0_IRQn |
                        MRV32_MSYS_OPSRV_IRQn);
 */
static inline void MRV_disable_local_irq(uint32_t mask)
{
    clear_csr(mie, mask);
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
 * The function MRV_enable_interrupts() enables all interrupts setting the
 * machine mode interrupt enable bit in MSTATUS register.
 */
static inline void MRV_enable_interrupts(void)
{
    set_csr(mstatus, MSTATUS_MIE);
}

/***************************************************************************//**
 * The function MRV_disable_interrupts() disables all interrupts clearing the
 * machine mode interrupt enable bit in MSTATUS register.
 */
static inline void MRV_disable_interrupts(void)
{
    clear_csr(mstatus, MSTATUS_MPIE);
    clear_csr(mstatus, MSTATUS_MIE);
}

/***************************************************************************//**
 * The function MRV_read_mtime() returns the current MTIME register value.
 */
static inline uint64_t MRV_read_mtime(void)
{
    volatile uint32_t hi = 0u;
    volatile uint32_t lo = 0u;

#ifndef MIV_RV32_EXT_TIMER
    /* when mtime lower word is 0xFFFFFFFF, there will be rollover and
     * returned value could be wrong. */
    do {
        hi = MTIMEH;
        lo = MTIME;
    } while(hi != MTIMEH);

    return((((uint64_t)MTIMEH) << 32u) | lo);
#endif
}

#ifdef __cplusplus
}
#endif

#endif  /* RISCV_HAL_H */

