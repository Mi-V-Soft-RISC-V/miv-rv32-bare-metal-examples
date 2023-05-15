/*******************************************************************************
 * Copyright 2019-2023 Microchip FPGA Embedded Systems Solutions.
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

/*=========================================================================*//**
  @mainpage MIV_RV32 Hardware Abstraction Layer

  ==============================================================================
  Introduction
  ==============================================================================
  This document describes the Hardware Abstraction Layer (HAL) for the MIV_RV32 
  Soft IP Core. This release of the HAL corresponds to the Soft IP core MIV_RV32
  v3.1 release. It also supports earlier versions of the MIV_RV32 as well as the 
  legacy RV32 IP cores.
  The preprocessor macros provided with the MIV_RV32 HAL are used to customize 
  it to target the Soft Processor IP version being used in your project.

  The term "MIV_RV32" represents following two cores:    
    - MIV_RV32 v3.0 and later (the latest and greatest Mi-V soft processor)      
    - MIV_RV32IMC v2.1 (MIV_RV32 v3.0 is a drop in replacement for this core)
  It is highly recommended to migrate your design to MIV_RV32 v3.1
 
  The term, Legacy RV32 IP cores, represents following IP cores:    
    - MIV_RV32IMA_L1_AHB     
    - MIV_RV32IMA_L1_AXI     
    - MIV_RV32IMAF_L1_AHB

  These legacy RV32 IP cores are deprecated. It is highly recommended to migrate
  your designs to MIV_RV32 v3.1 (and subsequent IP releases) for the latest 
  enhancements, bug fixes, and support.

  --------------------------------
  MIV_RV32 V3.1
  --------------------------------
  This is the latest release of the MIV_RV32 Soft IP core. For more details, 
  refer to the MIV_RV32 User [Guide](https://www.microchip.com/en-us/products/fpgas-and-plds/ip-core-tools/miv-rv32)

  The MIV_RV32 Core as well as this document use the terms defined below:

  --------------------------------
    - SUBSYS - Processor Subsystem for RISC-V
    - OPSRV - Offload Processor Subsystem for RISC-V
    - GPR - General Purpose Registers
    - MGECIE - Machine GPR ECC Correctable Interrupt Enable
    - MGEUIE - Machine GPR ECC Uncorrectable Interrupt Enable
    - MTIE - Machine Timer Interrupt Enable
    - MEIE - Machine External Interrupt Enable
    - MSIE - Machine Software Interrupt Enable
    - ISR - Interrupt Service Routine

  ==============================================================================
  Customizing MIV_RV32 HAL
  ==============================================================================
  To use the HAL with older releases of MIV_RV32, preprocessor macros have been 
  provided. Using these macros, any of the IP version is targeted.
  The HAL is used to target any of the mentioned platforms by adding the 
  following macros in the way :
  Project Properties > C/C++ Build > Settings > Preprocessor in Assembler and 
  Compiler settings.
  The table below shows the macros corresponding to the MIV Core being used in 
  your libero project. By default, the HAL targets v3.1 of the IP core and no 
  macros need to be set for this configutation.
  
  | Libero MI-V Soft IP Version | SoftConsole Macro |
  |-----------------------------|-------------------|
  |       MIV_RV32 v3.1       |  no macro required  |
  |       MIV_RV32 v3.0       |    MIV_CORE_V3_0    |
  |     Legacy RV32 Cores     |    MIV_LEGACY_RV32  |

  --------------------------------
  Interrupt Handling
  --------------------------------
  The MIE Register is defined as a enum in the HAL, and the table below is used 
  as a reference when the vectored interrupts are enabled in the GUI core
  configurator.
 
  The MIE register is a RISC-V Control and Status Register (CSR), which stands
  for the Machine Interrupt Enable. This is used to enable the machine mode
  interrupts in the MIV_RV32 hart. Refer to the RISC-V Priv spec for more details.
  
  The following table shows the trap entry addresses when an interrupt occurs and
  the vectored interrupts are enabled in the GUI configurator.
  
  | MIE Register Bit  | Interrupt Enable | Vector Address |
  |-------------------|------------------|----------------|
  |        31         |     MSYS_IE7     |  mtvec.BASE + 0x7C   |
  |        30         |     MSYS_IE6     |  mtvec.BASE + 0x78   |
  |        29         |     MSYS_IE5     |  mtvec.BASE + 0x74   |
  |        28         |     MSYS_IE4     |  mtvec.BASE + 0x70   |
  |        27         |     MSYS_IE3     |  mtvec.BASE + 0x6C   |
  |        26         |     MSYS_IE2     |  mtvec.BASE + 0x68   |
  |        25         |     MSYS_IE1     |  mtvec.BASE + 0x64   |
  |        24         |     MSYS_IE0     |  mtvec.BASE + 0x60   |
  |        23         |    SUBSYS_EI     |  mtvec.BASE + 0x5C   |
  |        22         |     SUBSYSR      |  mtvec.BASE + 0x58   |
  |        17         |      MGECIE      |  mtvec.BASE + 0x44   |
  |        16         |      MGEUIE      |  mtvec.BASE + 0x40   |
  |        11         |       MEIE       |  mtvec.BASE + 0x2C   |
  |         7         |       MTIE       |  mtvec.BASE + 0x1C   |
  |         3         |       MSIE       |  mtvec.BASE + 0x0C   |

  
  For changes in MIE register map, see the [MIE Register Map for MIV_RV32 v3.0]
  (#mie-register-map-for-miv_rv32-v3.0) section. 
  
  SUBSYSR is currently not being used by the core and is Reserved for future use.

  The mtvec.BASE field corresponds to the bits [31:2], where mtvec stands for 
  Machine Trap Vector, and all traps set the PC to the the value stored in the 
  mtvec.BASE field when in Non-Vectored mode. In this case, a generic trap 
  handler is as an interrupt service routine.

  When Vectored interrupts are enabled, use this formula to calculate the trap
  address: (mtvec.BASE + 4*cause), where cause comes from the mcause CSR. The 
  mcause register is written with a code indicating the event that caused the trap.
  For more details, see the RISC-V priv specification. 

  The MIV_RV32 Soft IP core does not contain a Platfrom Level Interrup Controller 
  (PLIC). It is advised to use the PLIC contained within the MIV_ESS sub-system.
  Connect the PLIC interrupt output of the MIV_ESS to the EXT_IRQ pin on the 
  MIV_RV32.

  The following table is the MIE register map for the MIV_RV32 Core V3.0. It only
  highlights the differences between the V3.0 and V3.1 of the core.

  --------------------------------
  MIE Register Map for MIV_RV32 V3.0 
  --------------------------------
   
  | MIE Register Bit  | Target Interrupt | Vector Address |
  |-------------------|------------------|----------------|
  |        31         |    Not in use    |   top table   |
  |        30         |     SUBSYS_EI    |  addr + 0x78   |
  |        23         |    Not in use    |   Not in use   |
  |        22         |    Not in use    |   Not in use   |

  Other interrupt bit postions like the MGEUIE and MSYS_IE5 to MSYS_IE0 remain 
  unchanged.

  --------------------------------
  Floating Point Interrupt Support
  --------------------------------
  When an interrupt is taken and Floating Point instructions are used in the 
  ISR, the floating point register context must be saved to resume the application
  correctly. To use this feature, enable the provided macro in the 
  Softconsole build settings.
  This feature is turned off by default as it adds overhead which is not required 
  when the ISR does not used FP insturctions and saving the general purpose 
  register context is sufficient.

  |       Macro Name       |                    Definition                     |
  |--------------------------|-------------------------------------------------|
  |    MIV_FP_CONTEXT_SAVE   |     Define to save the FP register file         |

  
  --------------------------------
  SUBSYS - SubSystem for RISC-V
  --------------------------------
  SUBSYS stands for SubSystem for RISC-V. This was previously (MIV_RV32 v3.0) 
  known as OPSRV, which stands for "Offload Processor Subsystem
  for RISC-V". See the earlier versions of the handbook for more details.
  In the latest release of the MIV_RV32 IP core v3.1, OPSRV has been renamed to 
  SUBSYS. The MIV_RV32 HAL now uses SUBSYS instead of OPSRV.

 *//*=========================================================================*/
#ifndef RISCV_HAL_H
#define RISCV_HAL_H

#include "miv_rv32_regs.h"
#include "miv_rv32_plic.h"
#include "miv_rv32_assert.h"
#include "miv_rv32_subsys.h"

#ifndef LEGACY_DIR_STRUCTURE
#include "fpga_design_config/fpga_design_config.h"
#else
#include "hw_platform.h"
#endif  /*LEGACY_DIR_STRUCTURE*/

#ifdef __cplusplus
extern "C" {
#endif
/*-------------------------------------------------------------------------*//**
  SUBSYS Backwards Compatibility 
  =======================================
  For application code using the older macro names and API functions, these macros
  act as a compatibility layer and applications which use OPSRV API features work 
  due to these macro definitions. However, it is adviced to update your
  application code to use the SUBSYS macros and API functions.
   
  |      Macro Name         |       Now Called         |
  |-------------------------|--------------------------|
  | OPSRV_TCM_ECC_CE_IRQ    | SUBSYS_TCM_ECC_CE_IRQ    | 
  | OPSRV_TCM_ECC_UCE_IRQ   | SUBSYS_TCM_ECC_UCE_IRQ   | 
  | OPSRV_AXI_WR_RESP_IRQ   | SUBSYS_AXI_WR_RESP_IRQ   | 
  | MRV32_MSYS_OPSRV_IRQn   | MRV32_SUBSYS_IRQn        | 
  | MRV32_opsrv_enable_irq  | MRV32_subsys_enable_irq  | 
  | MRV32_opsrv_disable_irq | MRV32_subsys_disable_irq | 
  | MRV32_opsrv_clear_irq   | MRV32_subsys_clear_irq   | 
  | OPSRV_IRQHandler        | SUBSYS_IRQHandler        |
 */

/*-------------------------------------------------------------------------*//**
  MTIME Timer Interrupt Constants
  =======================================
  These values contain the register addresses for the registers used by the 
  machine timer interrupt

  MTIME_PRESCALER is not defined on the MIV_RV32IMC v2.0 and v2.1. By using this
  definition the system crashes. For those core, use the following definition:

  #define MTIME_PRESCALER              100u

  MTIME and MTIMECMP
  --------------------------------
  MIV_RV32 core offers flexibility in terms of generating MTIME and MTIMECMP 
  registers internal to the core or using external time reference. There four
  possible combinations:

  - Internal MTIME and Internal MTIME IRQ enabled Generate the MTIME and MTIMECMP
  registers internally. (The only combination available on legacy RV32 cores)

  - Internal MTIME enabled and Internal MTIME IRQ disabled Generate the MTIME 
  internally and have a timer interrupt input to the core as external pin. In 
  this case, 1 pin port will be available on MIV_RV32 for timer interrupt.

  - When the internal MTIME is disabled, and the Internal MTIME IRQ is enabled, the
  system generates the time value externally and generates the mtimecmp and 
  interrupt internally (for example, a multiprocessor system with a shared time 
  between all cores). In this case, a 64-bit port is available on the MIV_RV32 
  core as input.

  - Internal MTIME and Internal MTIME IRQ disabled Generate both the time and 
  timer interrupts externally. In this case a 64 bit port will be available on 
  the MIV_RV32 core as input, and a 1 pin port will be available for timer 
  interrupt.

  To handle all these combinations in the firmware, the following constants must 
  be defined in accordance with the configuration that you have made on your 
  MIV_RV32 core design.

  MIV_RV32_EXT_TIMER
  --------------------------------
  When defined, it means that the MTIME register is not available internal to 
  the core. In this case, a 64 bit port will be available on the MIV_RV32 core as
  input. When this macro is not defined, it means that the MTIME register is 
  available internally to the core.

  MIV_RV32_EXT_TIMECMP
  --------------------------------
  When defined, it means the MTIMECMP register is not available internally to 
  the core and the Timer interrupt input to the core comes as an external pin. 
  When this macro is not defined it means the that MTIMECMP register exists 
  internal to the core and that the timer interrupt is generated internally.

NOTE: All these macros must not be defined if you are using a MIV_RV32 core.
 */

#define OPSRV_TCM_ECC_CE_IRQ                SUBSYS_TCM_ECC_CE_IRQ
#define OPSRV_TCM_ECC_UCE_IRQ               SUBSYS_TCM_ECC_UCE_IRQ
#define OPSRV_AXI_WR_RESP_IRQ               SUBSYS_AXI_WR_RESP_IRQ
#define MRV32_MSYS_OPSRV_IRQn               MRV32_SUBSYS_IRQn
#define MRV32_opsrv_enable_irq              MRV32_subsys_enable_irq
#define MRV32_opsrv_disable_irq             MRV32_subsys_disable_irq
#define MRV32_opsrv_clear_irq               MRV32_subsys_clear_irq
#define OPSRV_IRQHandler                    SUBSYS_IRQHandler

/*-------------------------------------------------------------------------*//**
  External IRQ
  =======================================
  Return value from External IRQ handler. This is used to disable the
  External Interrupt.
  
  | Macro Name  | Value |  Description|
  |-------------------|--------|----------------|
  | EXT_IRQ_KEEP_ENABLED  |    0    |  Keep external interrupts enabled |
  | EXT_IRQ_DISABLE       |    1    |  Disable external interrupts      |
 */
#define EXT_IRQ_KEEP_ENABLED                0U
#define EXT_IRQ_DISABLE                     1U

#define MTIME_DELTA                     5
#ifdef MIV_LEGACY_RV32
#define MSIP                            (*(uint32_t*)0x44000000UL)
#define MTIMECMP                        (*(uint32_t*)0x44004000UL)
#define MTIMECMPH                       (*(uint32_t*)0x44004004UL)
#define MTIME                           (*(uint32_t*)0x4400BFF8UL)
#define MTIMEH                          (*(uint32_t*)0x4400BFFCUL)

/* To maintain backward compatibility with FreeRTOS config code */
#define PRCI_BASE                       0x44000000UL
#else /* MIV_LEGACY_RV32 */

/* To maintain backward compatibility with FreeRTOS config code */
#define PRCI_BASE                       0x02000000UL

#ifndef MIV_RV32_EXT_TIMECMP
#define MTIMECMP                        (*(volatile uint32_t*)0x02004000UL)
#define MTIMECMPH                       (*(volatile uint32_t*)0x02004004UL)
#else
#define MTIMECMP                        (0u)
#define MTIMECMPH                       (0u)
#endif

#define MTIME_PRESCALER                 (*(volatile uint32_t*)0x02005000UL)

#ifndef MIV_RV32_EXT_TIMER
#define MTIME                           (*(volatile uint32_t*)0x0200BFF8UL)
#define MTIMEH                          (*(volatile uint32_t*)0x0200BFFCUL)

/***************************************************************************//**
  MIMPID Register
  The MIMPID register is a RISC-V Control and Status Register In the v3.0 of 
  MIV_RV32, the value of `MIMPID = 0x000540AD`. In the v3.1 of MIV_RV32, the 
  value if `MIMPID = 0xE5010301` corresponding to (E)mbedded (5)ystem(01) core 
  version (03).(01) this terminology will be followed in the subsequent releases 
  of the core read the csr value and store it in a varible which may be used to 
  check the MIV_RV32 core version during runtime.

  Future releases of the core will increment the 03 and 01 as major and minor 
  releases respectively and the register can be read at runtime to find the 
  Soft IP core version.

  |  Core Version  |  Register  |  Value  |  Notes  |
  |----------------|------------|---------|---------|
  |  MIV_RV32 V3.1  |  mimpid |   0xE5010301  | implimentation ID |
  |  MIV_RV32 V3.0  |  mimpid |   0x000540AD  | implimentation ID |
 */
#define MIMPID                          read_csr(mimpid)

/*Used as a mask to read and write to mte mtvec.BASE address*/
#define MTVEC_BASE_ADDR_MASK            0xFFFFFFFC

#else
#define MTIME                           (0u)
#define MTIMEH                          (0u)
#endif  /*MIV_RV32_EXT_TIMER*/

/*-------------------------------------------------------------------------*//**
  RISC-V Specification Interrupts
  =======================================
  These definitions are provided for easy identification of the interrupt
  in the MIE/MIP registers.
  Apart from the standard software, timer, and external interrupts, the names
  of the additional interrupts correspond to the names as used in the MIV_RV32
  handbook. Please refer the MIV_RV32 handbook for more details.
 
  All the interrups, provided by the MIV_RV32 core, follow the interrupt priority
  order and register description as mentioned in the RISC-V spec.

  | Macro Name  | Value |  Description|
  |-------------------|--------|----------------|
  | MRV32_SOFT_IRQn   | MIE_3_IRQn  |  Software interrupt enable  |
  | MRV32_TIMER_IRQn  | MIE_7_IRQn  |  Timer interrupt enable     |
  | MRV32_EXT_IRQn    | MIE_11_IRQn |  External interrupt enable  |

 */
#define MRV32_SOFT_IRQn                 MIE_3_IRQn
#define MRV32_TIMER_IRQn                MIE_7_IRQn
#define MRV32_EXT_IRQn                  MIE_11_IRQn

/***************************************************************************//**
  Interrupt numbers:
  This enum represents the interrupt enable bits in the MIE register.
 */
enum
{
    MIE_0_IRQn  =  (0x01u),
    MIE_1_IRQn  =  (0x01u<<1u),
    MIE_2_IRQn  =  (0x01u<<2u),
    MIE_3_IRQn  =  (0x01u<<3u),         /*MSIE 0xC*/
    MIE_4_IRQn  =  (0x01u<<4u),
    MIE_5_IRQn  =  (0x01u<<5u),
    MIE_6_IRQn  =  (0x01u<<6u),
    MIE_7_IRQn  =  (0x01u<<7u),         /*MTIE 0x1C*/
    MIE_8_IRQn  =  (0x01u<<8u),
    MIE_9_IRQn  =  (0x01u<<9u),
    MIE_10_IRQn =  (0x01u<<10u),
    MIE_11_IRQn =  (0x01u<<11u),        /*MEIE 0x2C*/
    MIE_12_IRQn =  (0x01u<<12u),
    MIE_13_IRQn =  (0x01u<<13u),
    MIE_14_IRQn =  (0x01u<<14u),
    MIE_15_IRQn =  (0x01u<<15u),
    MIE_16_IRQn =  (0x01u<<16u),        /*MGEUIE ECC Uncorrectable 0x40*/
    MIE_17_IRQn =  (0x01u<<17u),        /*MGECIE ECC Correctable 0x44*/
    MIE_18_IRQn =  (0x01u<<18u),
    MIE_19_IRQn =  (0x01u<<19u),
    MIE_20_IRQn =  (0x01u<<20u),
    MIE_21_IRQn =  (0x01u<<21u),
    MIE_22_IRQn =  (0x01u<<22u),        /*SUBSYSR 0x58 (R)eserved*/        
    MIE_23_IRQn =  (0x01u<<23u),        /*SUBSYS_IE 0x5C for MIV_RV32 v3.1*/      
    MIE_24_IRQn =  (0x01u<<24u),        /*MSYS_IE0 0x60*/
    MIE_25_IRQn =  (0x01u<<25u),        /*MSYS_IE1 0x64*/
    MIE_26_IRQn =  (0x01u<<26u),        /*MSYS_IE2 0x68*/
    MIE_27_IRQn =  (0x01u<<27u),        /*MSYS_IE3 0x6C*/
    MIE_28_IRQn =  (0x01u<<28u),        /*MSYS_IE4 0x70*/        
    MIE_29_IRQn =  (0x01u<<29u),        /*MSYS_IE5 0x74*/
    MIE_30_IRQn =  (0x01u<<30u),        /*MSYS_IE6 0x78, read comment below*/
    MIE_31_IRQn =  (0x01u<<31u)         /*MSYS_IE7 0x7C*/
} MRV_LOCAL_IRQn_Type;

#define MRV32_MGEUIE_IRQn               MIE_16_IRQn
#define MRV32_MGECIE_IRQn               MIE_17_IRQn
#define MRV32_MSYS_EIE0_IRQn            MIE_24_IRQn
#define MRV32_MSYS_EIE1_IRQn            MIE_25_IRQn
#define MRV32_MSYS_EIE2_IRQn            MIE_26_IRQn
#define MRV32_MSYS_EIE3_IRQn            MIE_27_IRQn
#define MRV32_MSYS_EIE4_IRQn            MIE_28_IRQn
#define MRV32_MSYS_EIE5_IRQn            MIE_29_IRQn
#ifndef MIV_RV32_V3_0 /*For MIV_RV32 v3.1*/
#define MRV32_SUBSYSR_IRQn              MIE_22_IRQn
#define MRV32_SUBSYS_IRQn               MIE_23_IRQn
#define MRV32_MSYS_EIE6_IRQn            MIE_30_IRQn
#define MRV32_MSYS_EIE7_IRQn            MIE_31_IRQn
#else
#define MRV32_SUBSYS_IRQn               MIE_30_IRQn
#endif /*MIV_RV32_V3_0*/

/*--------------------------------Public APIs---------------------------------*/

/***************************************************************************//**
  The MRV32_clear_gpr_ecc_errors() function clears single bit ECC errors on the 
  GPRs. The ECC block does not write back corrected data to memory. Hence, when 
  ECC is enabled for the GPRs and if that data has a single bit error then the 
  data coming out of the ECC block is corrected and will not have the error, but 
  the data source will still have the error. Therefore, if data has a single bit
  error, then the corrected data must be written back to prevent the single bit
  error from becoming a double bit error. Clear the pending interrupt bit after 
  this using MRV32_mgeci_clear_irq() function to complete the ECC error handling.

  @param
  This function does not take any parameters.

  @return
  This functions returns the CORE_GPR_DED_RESET_REG bit value.
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


/***************************************************************************//**
  The MRV32_mgeui_clear_irq() function clears the GPR ECC Uncorrectable 
  Interrupt. MGEUI interrupt is available only when ECC is enabled in the MIV_RV32 
  IP configurator.

  @return
  This function does not return any value.
 */
static inline void MRV32_mgeui_clear_irq(void)
{
    clear_csr(mip, MRV32_MGEUIE_IRQn);
}

/***************************************************************************//**
  The MRV32_mgeci_clear_irq() function clears the GPR ECC Correctable Interrupt
  MGECI interrupt is available only when ECC is enabled in the MIV_RV32 IP 
  configurator.

  @return 
  This function does not return any value.
 */
static inline void MRV32_mgeci_clear_irq(void)
{
    clear_csr(mip, MRV32_MGECIE_IRQn);
}

/***************************************************************************//**
  The MRV_enable_local_irq() function enables the local interrupts. It takes a 
  mask value as input. For each set bit in the mask value, the corresponding 
  interrupt bit in the MIE register is enabled.
  
  MRV_enable_local_irq( MRV32_SOFT_IRQn | MRV32_TIMER_IRQn | MRV32_EXT_IRQn |
                        MRV32_MSYS_EIE0_IRQn |
                        MRV32_MSYS_SUBSYS_IRQn);                
 */
static inline void MRV_enable_local_irq(uint32_t mask)
{
    set_csr(mie, mask);
}

/***************************************************************************//**
  The MRV_disable_local_irq() function disables the local interrupts. It takes a 
  mask value as input. For each set bit in the mask value, the corresponding 
  interrupt bit in the MIE register is disabled.
  
  MRV_disable_local_irq( MRV32_SOFT_IRQn | MRV32_TIMER_IRQn | MRV32_EXT_IRQn |
                         MRV32_MSYS_EIE0_IRQn |
                         MRV32_MSYS_SUBSYS_IRQn);
 */
static inline void MRV_disable_local_irq(uint32_t mask)
{
    clear_csr(mie, mask);
}
#endif /* MIV_LEGACY_RV32 */ 

/***************************************************************************//**
  The MRV_enable_interrupts() function enables all interrupts by setting the
  machine mode interrupt enable bit in MSTATUS register.

  @param
  This function does not take any parameters.

  @return
  This functions returns the CORE_GPR_DED_RESET_REG bit value.
 */
static inline void MRV_enable_interrupts(void)
{
    set_csr(mstatus, MSTATUS_MIE);
}

/***************************************************************************//**
  The MRV_disable_interrupts() function disables all interrupts by clearing the
  machine mode interrupt enable bit in MSTATUS register.
  @param
  This function does not take any parameters.

  @return
  This functions returns the CORE_GPR_DED_RESET_REG bit value.
 */
static inline void MRV_disable_interrupts(void)
{
    clear_csr(mstatus, MSTATUS_MPIE);
    clear_csr(mstatus, MSTATUS_MIE);
}

/***************************************************************************//**
  The MRV_read_mtvec_base() function reads the mtvec base value, which is the 
  addr used when an interrupt/trap occurs. In the mtvec register, [31:2] is the 
  BASE address. NOTE: The BASE address must be aligned on a 4B boundary.

  @param
  The function does not take any parameters.

  @return
  The function returns the value of the BASE field [31:2] as an unsigned 32-bit 
  value.
 */

#ifndef MIV_LEGACY_RV32
#ifndef MIV_RV32_v3_0
static inline uint32_t MRV_read_mtvec_base (void)
{
    uint32_t mtvec_addr_base = read_csr(mtvec);
    return mtvec_addr_base & MTVEC_BASE_ADDR_MASK;
}

/***************************************************************************//**
  The MRV_set_mtvec_base() function takes the mtvec_base address as a unsigned int 
  and writes the value into the BASE field [31:2] in the mtvec CSR, MODE[1:0] 
  is Read-only. BASE is 4B aligned, so the lowest 2 bits of mtvec_base are 
  ignored.

  @param mtvec_base
  Any legal value is passed into the function, and it is used as the trap_entry 
  for interrupts. The PC jumps to this address provided when an interrupt occurs. 
  In case of vectored interrupts, the address value mentioned in the vector 
  table under the MIE Register Map is updated to the value passed to this 
  function parameter.

  @return
  This function does not return any value.
 */
static inline void MRV_set_mtvec_base (uint32_t mtvec_base)
{
    mtvec_base = mtvec_base & MTVEC_BASE_ADDR_MASK;
    write_csr(mtvec, mtvec_base);
}
#endif /*MIV_RV32_v3_0*/
#endif /*MIV_LEGACY_RV32*/

/***************************************************************************//**
  The MRV_read_mtime() function returns the current MTIME register value.
 */
static inline uint64_t MRV_read_mtime(void)
{
    volatile uint32_t hi = 0u;
    volatile uint32_t lo = 0u;

    /* when mtime lower word is 0xFFFFFFFF, there will be rollover and
     * returned value could be wrong. */
    do {
        hi = MTIMEH;
        lo = MTIME;
    } while(hi != MTIMEH);

    return((((uint64_t)MTIMEH) << 32u) | lo);
}

/***************************************************************************//**
  The MRV_raise_soft_irq() function raises a synchronous software interrupt
  by writing into the MSIP register.
  @param
  This function does not take any parameters.

  @return
  This function does not return any value.
 */
static inline void MRV_raise_soft_irq(void)
{
    set_csr(mie, MIP_MSIP);       /* Enable software interrupt bit */

#ifdef MIV_LEGACY_RV32
    /* You need to make sure that the global interrupt is enabled */
    MSIP = 0x01;   /* raise soft interrupt */
#else
    /* Raise soft IRQ on MIV_RV32 processor */
    SUBSYS->soft_reg |= SUBSYS_SOFT_IRQ;
#endif
}

/***************************************************************************//**
  The MRV_clear_soft_irq() function clears a synchronous software interrupt
  by clearing the MSIP register.
  @param
  This function does not take any parameters.

  @return
  This function does not return any value.
 */
static inline void MRV_clear_soft_irq(void)
{
#ifdef MIV_LEGACY_RV32
    MSIP = 0x00u;   /* clear soft interrupt */
#else
    /* Clear soft IRQ on MIV_RV32 processor */
    SUBSYS->soft_reg &= ~SUBSYS_SOFT_IRQ;
#endif
}

/***************************************************************************//**
  System tick handler. This handler function gets called when the Machine
  timer interrupt asserts. An implementation of this function must be provided 
  by the application to implement the application specific machine timer 
  interrupt handling. If application does not provide such implementation, the 
  weakly linked handler stub function implemented in riscv_hal_stubs.c gets
  linked.
 */
void SysTick_Handler(void);

/***************************************************************************//**
  System timer tick configuration.
  Configures the machine timer to generate a system tick interrupt at regular
  intervals. Takes the number of system clock ticks between interrupts.
 
  Though this function can take any valid ticks value as parameter, we expect
  that, for all practical purposes, a small tick value (to generate periodic 
  interrupts every few miliseconds) is passed. If you need to generate periodic 
  events in the range of seconds or more, you may use the SysTick_Handler() to 
  further count the number of interrupts and hence the larger time intervals.

  @param ticks
  This is the number of ticks or clock cycles which are counted down from the 
  interrupt to be triggered.
 
  @return
  Returns 0 if successful.
  Returns 1 if the interrupt interval is not achieved.
 */
uint32_t MRV_systick_config(uint64_t ticks);

#ifdef __cplusplus
}
#endif
#endif  /* RISCV_HAL_H */