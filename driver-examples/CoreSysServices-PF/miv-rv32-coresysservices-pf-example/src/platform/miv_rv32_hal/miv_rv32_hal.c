/*******************************************************************************
 * Copyright 2019-2021 Microchip FPGA Embedded Systems Solutions.
 *
 * SPDX-License-Identifier: MIT
 *
 * @file miv_rv32_hal.c
 * @author Microchip FPGA Embedded Systems Solutions
 * @brief Implementation of Hardware Abstraction Layer for Mi-V soft processors
 *
 */
#include <unistd.h>
#include "miv_rv32_hal.h"

#ifdef __cplusplus
extern "C" {
#endif

#define SUCCESS                         0U
#define ERROR                           1U

extern void Software_IRQHandler(void);

#ifdef MIV_LEGACY_RV32
#define MTIME_PRESCALER                 100UL
/*------------------------------------------------------------------------------
 *
 */
uint8_t Invalid_IRQHandler(void);
uint8_t External_1_IRQHandler(void);
uint8_t External_2_IRQHandler(void);
uint8_t External_3_IRQHandler(void);
uint8_t External_4_IRQHandler(void);
uint8_t External_5_IRQHandler(void);
uint8_t External_6_IRQHandler(void);
uint8_t External_7_IRQHandler(void);
uint8_t External_8_IRQHandler(void);
uint8_t External_9_IRQHandler(void);
uint8_t External_10_IRQHandler(void);
uint8_t External_11_IRQHandler(void);
uint8_t External_12_IRQHandler(void);
uint8_t External_13_IRQHandler(void);
uint8_t External_14_IRQHandler(void);
uint8_t External_15_IRQHandler(void);
uint8_t External_16_IRQHandler(void);
uint8_t External_17_IRQHandler(void);
uint8_t External_18_IRQHandler(void);
uint8_t External_19_IRQHandler(void);
uint8_t External_20_IRQHandler(void);
uint8_t External_21_IRQHandler(void);
uint8_t External_22_IRQHandler(void);
uint8_t External_23_IRQHandler(void);
uint8_t External_24_IRQHandler(void);
uint8_t External_25_IRQHandler(void);
uint8_t External_26_IRQHandler(void);
uint8_t External_27_IRQHandler(void);
uint8_t External_28_IRQHandler(void);
uint8_t External_29_IRQHandler(void);
uint8_t External_30_IRQHandler(void);
uint8_t External_31_IRQHandler(void);


/*------------------------------------------------------------------------------
 * RISC-V interrupt handler for external interrupts.
 */
uint8_t (* const ext_irq_handler_table[32])(void) =
{

    Invalid_IRQHandler,
    External_1_IRQHandler,
    External_2_IRQHandler,
    External_3_IRQHandler,
    External_4_IRQHandler,
    External_5_IRQHandler,
    External_6_IRQHandler,
    External_7_IRQHandler,
    External_8_IRQHandler,
    External_9_IRQHandler,
    External_10_IRQHandler,
    External_11_IRQHandler,
    External_12_IRQHandler,
    External_13_IRQHandler,
    External_14_IRQHandler,
    External_15_IRQHandler,
    External_16_IRQHandler,
    External_17_IRQHandler,
    External_18_IRQHandler,
    External_19_IRQHandler,
    External_20_IRQHandler,
    External_21_IRQHandler,
    External_22_IRQHandler,
    External_23_IRQHandler,
    External_24_IRQHandler,
    External_25_IRQHandler,
    External_26_IRQHandler,
    External_27_IRQHandler,
    External_28_IRQHandler,
    External_29_IRQHandler,
    External_30_IRQHandler,
    External_31_IRQHandler
};

#else
/*------------------------------------------------------------------------------
 *
 */
extern void MGEUI_IRQHandler(void);
extern void MGECI_IRQHandler(void);
extern void MSYS_EI0_IRQHandler(void);
extern void MSYS_EI1_IRQHandler(void);
extern void MSYS_EI2_IRQHandler(void);
extern void MSYS_EI3_IRQHandler(void);
extern void MSYS_EI4_IRQHandler(void);
extern void MSYS_EI5_IRQHandler(void);
extern void OPSRV_IRQHandler(void);
extern void External_IRQHandler(void);
#endif

/*------------------------------------------------------------------------------
 * Increment value for the mtimecmp register in order to achieve a system tick
 * interrupt as specified through the MRV_systick_config() function.
 */
static uint64_t g_systick_increment = 0U;

/*------------------------------------------------------------------------------
 * Disable all interrupts.
 */
void MRV_disable_interrupts(void)
{
    clear_csr(mstatus, MSTATUS_MPIE);
    clear_csr(mstatus, MSTATUS_MIE);
}

/*------------------------------------------------------------------------------
 * Enable all interrupts.
 */
void MRV_enable_interrupts(void)
{
    set_csr(mstatus, MSTATUS_MIE);
}

/*------------------------------------------------------------------------------
 * Configure the machine timer to generate an interrupt.
 */
uint32_t MRV_systick_config(uint32_t ticks)
{
    uint32_t ret_val = ERROR;

    g_systick_increment = (uint64_t)(ticks) / MTIME_PRESCALER;

    if (g_systick_increment > 0U)
    {
#ifndef MIV_RV32_EXT_TIMECMP
        MTIMECMP = MTIME + g_systick_increment;
#endif
        set_csr(mie, MIP_MTIP);
        MRV_enable_interrupts();

        ret_val = SUCCESS;
    }

    return ret_val;
}

/*------------------------------------------------------------------------------
 * RISC-V interrupt handler for machine timer interrupts.
 */
void handle_m_timer_interrupt(void)
{
    clear_csr(mie, MIP_MTIP);
    SysTick_Handler();

#ifndef MIV_RV32_EXT_TIMECMP
    MTIMECMP = MTIME + g_systick_increment;
#endif

    set_csr(mie, MIP_MTIP);
}

/*------------------------------------------------------------------------------
 *
 */
#ifdef MIV_LEGACY_RV32
void handle_m_ext_interrupt(void)
{
    unsigned long hart_id = read_csr(mhartid);
    uint32_t int_num  = PLIC->TARGET[hart_id].CLAIM_COMPLETE;
    uint8_t disable = EXT_IRQ_KEEP_ENABLED;

    if (0 !=int_num)
    {
        disable = ext_irq_handler_table[int_num]();

        PLIC->TARGET[hart_id].CLAIM_COMPLETE = int_num;

        if(EXT_IRQ_DISABLE == disable)
        {
            MRV_PLIC_disable_irq((IRQn_Type)int_num);
        }
    }
}
#endif

void handle_m_soft_interrupt(void)
{
    Software_IRQHandler();

    /* Clear software interrupt */
    MRV_clear_soft_irq();
}

/*------------------------------------------------------------------------------
 * Trap handler
 */
void handle_trap(uintptr_t mcause, uintptr_t mepc)
{
    if ((mcause & MCAUSE_INT) && ((mcause & MCAUSE_CAUSE) == IRQ_M_EXT))
    {
#ifdef MIV_LEGACY_RV32
        handle_m_ext_interrupt();
#else
        External_IRQHandler();
#endif
    }
    else if ((mcause & MCAUSE_INT) && ((mcause & MCAUSE_CAUSE) == IRQ_M_SOFT))
    {
        handle_m_soft_interrupt();
    }
    else if ((mcause & MCAUSE_INT) && ((mcause & MCAUSE_CAUSE) == IRQ_M_TIMER))
    {
        handle_m_timer_interrupt();
    }

#ifndef MIV_LEGACY_RV32
    else if ((mcause & MCAUSE_INT) && ((mcause & MCAUSE_CAUSE) == MSYS_EI0))
    {
        MSYS_EI0_IRQHandler();
    }
    else if ((mcause & MCAUSE_INT) && ((mcause & MCAUSE_CAUSE) == MSYS_EI1))
    {
        MSYS_EI1_IRQHandler();
    }
    else if ((mcause & MCAUSE_INT) && ((mcause & MCAUSE_CAUSE) == MSYS_EI2))
    {
        MSYS_EI2_IRQHandler();
    }
    else if ((mcause & MCAUSE_INT) && ((mcause & MCAUSE_CAUSE) == MSYS_EI3))
    {
        MSYS_EI3_IRQHandler();
    }
    else if ((mcause & MCAUSE_INT) && ((mcause & MCAUSE_CAUSE) == MSYS_EI4))
    {
        MSYS_EI4_IRQHandler();
    }
    else if ((mcause & MCAUSE_INT) && ((mcause & MCAUSE_CAUSE) == MSYS_EI5))
    {
        MSYS_EI5_IRQHandler();
    }
    else if ((mcause & MCAUSE_INT) && ((mcause & MCAUSE_CAUSE) == OPSRV_REG))
    {
        OPSRV_IRQHandler();
    }
    else if ((mcause & MCAUSE_INT) && ((mcause & MCAUSE_CAUSE) == MGEUI))
    {
        MGEUI_IRQHandler();
    }
    else if ((mcause & MCAUSE_INT) && ((mcause & MCAUSE_CAUSE) == MGECI))
    {
        MGECI_IRQHandler();
    }
#endif

    else
    {
#ifndef NDEBUG
        /*
         Arguments supplied to this function are mcause, mepc (exception PC) and
         stack pointer.
         Based on privileged-isa specification mcause values and meanings are:

         0 Instruction address misaligned (mtval/mtval is the address)
         1 Instruction access fault       (mtval/mtval is the address)
         2 Illegal instruction            (mtval/mtval contains the
                                           offending instruction opcode)
         3 Breakpoint
         4 Load address misaligned        (mtval/mtval is the address)
         5 Load address fault             (mtval/mtval is the address)
         6 Store/AMO address fault        (mtval/mtval is the address)
         7 Store/AMO access fault         (mtval/mtval is the address)
         8 Environment call from U-mode
         9 Environment call from S-mode
         A Environment call from M-mode
         B Instruction page fault
         C Load page fault                (mtval/mtval is the address)
         E Store page fault               (mtval/mtval is the address)

         # Please note: mtval is the newer name for register mbadaddr
         # If you get a compile failure here, use the older name.
         # At this point, both are supported in latest compiler, older compiler
         # versions only support mbadaddr.
         # See: https://github.com/riscv/riscv-gcc/issues/133
        */

        /* interrupt pending */
         uintptr_t mip      = read_csr(mip);

         /* additional info and meaning depends on mcause */
         uintptr_t mtval = read_csr(mtval);

         /* trap vector */
         uintptr_t mtvec    = read_csr(mtvec);

         /* temporary, sometimes might hold temporary value of a0 */
         uintptr_t mscratch = read_csr(mscratch);

         /* status contains many smaller fields: */
         uintptr_t mstatus  = read_csr(mstatus);

         /* PC value when the exception was taken*/
         uintptr_t mmepc  = read_csr(mepc);

        /* breakpoint */
        __asm("ebreak");
#else
        _exit(1 + mcause);
#endif
    }
}

#ifdef __cplusplus
}
#endif
