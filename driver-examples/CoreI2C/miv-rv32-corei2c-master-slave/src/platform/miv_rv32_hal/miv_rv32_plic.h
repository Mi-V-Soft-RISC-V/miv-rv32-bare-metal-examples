/*******************************************************************************
 * Copyright 2019-2022 Microchip FPGA Embedded Systems Solutions.
 *
 * SPDX-License-Identifier: MIT
 *
 * @file miv_rv32_plic.h
 * @author Microchip FPGA Embedded Systems Solutions
 * @brief Mi-V legacy RV32 soft processor PLIC access data structures and
 *        functions.
 *        Legacy RV32 soft processors are DEPRICATED.
 *        Migrate to MIV_RV32 v3.0 or later.
 *
 */
#ifndef RISCV_PLIC_H
#define RISCV_PLIC_H

#include <stdint.h>
#include "miv_rv32_regs.h"

#ifdef __cplusplus
extern "C" {
#endif

/*==============================================================================
 * Interrupt numbers:
 */
#ifdef MIV_LEGACY_RV32
typedef enum
{
    NoInterrupt_IRQn = 0,
    External_1_IRQn  = 1,
    External_2_IRQn  = 2,
    External_3_IRQn  = 3,
    External_4_IRQn  = 4,
    External_5_IRQn  = 5,
    External_6_IRQn  = 6,
    External_7_IRQn  = 7,
    External_8_IRQn  = 8,
    External_9_IRQn  = 9,
    External_10_IRQn = 10,
    External_11_IRQn = 11,
    External_12_IRQn = 12,
    External_13_IRQn = 13,
    External_14_IRQn = 14,
    External_15_IRQn = 15,
    External_16_IRQn = 16,
    External_17_IRQn = 17,
    External_18_IRQn = 18,
    External_19_IRQn = 19,
    External_20_IRQn = 20,
    External_21_IRQn = 21,
    External_22_IRQn = 22,
    External_23_IRQn = 23,
    External_24_IRQn = 24,
    External_25_IRQn = 25,
    External_26_IRQn = 26,
    External_27_IRQn = 27,
    External_28_IRQn = 28,
    External_29_IRQn = 29,
    External_30_IRQn = 30,
    External_31_IRQn = 31
} IRQn_Type;

#define EXT_INTR_SOURCES                        31

/*==============================================================================
 * PLIC: Platform Level Interrupt Controller
 */
#define PLIC_BASE_ADDR                          0x40000000UL

typedef struct
{
    volatile uint32_t PRIORITY_THRESHOLD;
    volatile uint32_t CLAIM_COMPLETE;
    volatile uint32_t reserved[1022];
} IRQ_Target_Type;

typedef struct
{
    volatile uint32_t ENABLES[32];
} Target_Enables_Type;

typedef struct
{
    /*-------------------- Source Priority --------------------*/
    volatile uint32_t SOURCE_PRIORITY[1024];
    
    /*-------------------- Pending array --------------------*/
    volatile const uint32_t PENDING_ARRAY[32];
    volatile uint32_t RESERVED1[992];
    
    /*-------------------- Target enables --------------------*/
    volatile Target_Enables_Type TARGET_ENABLES[15808];

    volatile uint32_t RESERVED2[16384];
    
    /*--- Target Priority threshold and claim/complete---------*/
    IRQ_Target_Type TARGET[15872];
    
} PLIC_Type;

#define PLIC    ((PLIC_Type *)PLIC_BASE_ADDR)

/*==============================================================================
 * The function MRV_PLIC_init() initializes the PLIC controller and enables
 * the global external interrupt bit.
 */
static inline void MRV_PLIC_init(void)
{
    uint32_t inc;
    unsigned long hart_id = read_csr(mhartid);

    /* Disable all interrupts for the current hart. */
    for(inc = 0; inc < ((EXT_INTR_SOURCES + 32u) / 32u); ++inc)
    {
        PLIC->TARGET_ENABLES[hart_id].ENABLES[inc] = 0;
    }

    /* Set priorities to zero. */
    for(inc = 0; inc < EXT_INTR_SOURCES; ++inc)
    {
        PLIC->SOURCE_PRIORITY[inc] = 0;
    }

    /* Set the threshold to zero. */
    PLIC->TARGET[hart_id].PRIORITY_THRESHOLD = 0;

    /* Enable machine external interrupts. */
    set_csr(mie, MIP_MEIP);
}

/*==============================================================================
 * The function MRV_PLIC_enable_irq() enables the external interrupt for the
 * interrupt number indicated by the parameter IRQn.
 */
static inline void MRV_PLIC_enable_irq(IRQn_Type IRQn)
{
    unsigned long hart_id = read_csr(mhartid);
    uint32_t current = PLIC->TARGET_ENABLES[hart_id].ENABLES[IRQn / 32];
    current |= (uint32_t)1 << (IRQn % 32);
    PLIC->TARGET_ENABLES[hart_id].ENABLES[IRQn / 32] = current;
}

/*==============================================================================
 * The function MRV_PLIC_disable_irq() disables the external interrupt for
 * the interrupt number indicated by the parameter IRQn.

 * NOTE:
 * This function can be used to disable the external interrupt from outside
 * external interrupt handler function.
 * This function MUST NOT be used from within the External Interrupt handler.
 * If you wish to disable the external interrupt while the interrupt handler
 * for that external interrupt is executing then you must use the return value
 * EXT_IRQ_DISABLE to return from the extern interrupt handler.
 */
static inline void MRV_PLIC_disable_irq(IRQn_Type IRQn)
{
    unsigned long hart_id = read_csr(mhartid);
    uint32_t current = PLIC->TARGET_ENABLES[hart_id].ENABLES[IRQn / 32];

    current &= ~((uint32_t)1 << (IRQn % 32));

    PLIC->TARGET_ENABLES[hart_id].ENABLES[IRQn / 32] = current;
}

/*==============================================================================
 * The function MRV_PLIC_set_priority() sets the priority for the external
 * interrupt for the interrupt number indicated by the parameter IRQn.
 */
static inline void MRV_PLIC_set_priority(IRQn_Type IRQn, uint32_t priority)
{
    PLIC->SOURCE_PRIORITY[IRQn] = priority;
}

/*==============================================================================
 * The function MRV_PLIC_get_priority() returns the priority for the external
 * interrupt for the interrupt number indicated by the parameter IRQn.
 */
static inline uint32_t MRV_PLIC_get_priority(IRQn_Type IRQn)
{
    return PLIC->SOURCE_PRIORITY[IRQn];
}

/***************************************************************************//**
 *  MRV_PLIC_clear_pending_irq(void)
 *  This is only called by the startup hart and only once
 *  Clears any pending interrupts as PLIC can be in unknown state on startup
 */
static inline void MRV_PLIC_clear_pending_irq(void)
{
    unsigned long hart_id = read_csr(mhartid);
    volatile uint32_t int_num  = PLIC->TARGET[hart_id].CLAIM_COMPLETE;
    volatile int32_t wait_possible_int;

    while (NoInterrupt_IRQn != int_num)
    {
        PLIC->TARGET[hart_id].CLAIM_COMPLETE = int_num;
        wait_possible_int = 0xFU;
        while (wait_possible_int)
        {
            wait_possible_int--;
        }

        int_num  = PLIC->TARGET[hart_id].CLAIM_COMPLETE;
    }
}

#endif /* MIV_LEGACY_RV32 */

#ifdef __cplusplus
}
#endif

#endif  /* RISCV_PLIC_H */
