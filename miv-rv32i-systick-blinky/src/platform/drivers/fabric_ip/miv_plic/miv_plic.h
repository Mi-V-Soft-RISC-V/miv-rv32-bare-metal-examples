/*******************************************************************************
 * Copyright 2022 Microchip FPGA Embedded Systems Solutions.
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
 * This file contains the application programming interface for the MI-V Soft IP
 * PLIC module driver. This module is delivered as a part of Mi-V Extended
 * Sub-System(ESS).
 */
 /*=========================================================================*//**
  @mainpage Mi-V PLIC Bare Metal Driver

  ==============================================================================
  Introduction
  ==============================================================================
  The Mi-V driver provides a set of functions for controlling the Mi-V PLIC
  (platform level interrupt controller) soft-IP module. This module is delivered
  as a part of the MIV_ESS. The PLIC multiplexes external interrupt signals into
  a single interrupt signal that is connected to an external interrupt of the
  processor.

  The major features provided by the driver are:
      - Support for configuring the PLIC instances.
      - Enabling and Disabling interrupts
      - Interrupt Handling

  This driver can be used as part of a bare metal system where no operating
  system  is available. The driver can be adapted for use as part of an
  operating system, but the implementation of the adaptation layer between the
  driver and  the operating system's driver model is outside the scope of this
  driver.

  ==============================================================================
  Hardware Flow Dependencies
  ==============================================================================
  The application software should initialize the Mi-V PLIC through the call to
  the MIV_PLIC_init() function for Mi-V PLIC instance in the design.

  No Mi-V PLIC hardware configuration parameters are used by the driver, apart
  from the Mi-V PLIC base address. Hence, no additional configuration files
  are required to use the driver.

  ==============================================================================
  Theory of Operation
  ==============================================================================
  The operation of Mi-V PLIC driver is divided into following steps:
      - Initialization
      - Enabling and Disabling interrupts
      - Interrupt control

  --------------------------------------------
  Initialization
  --------------------------------------------
  The Mi-V PLIC module is first initialized by the call to MIV_PLIC_init(). This
  function takes a pointer to the Mi-V PLIC instance data structure and the base
  address of the Mi-V PLIC instance is defined by the hardware design. The
  instance data structure is used to store the base address of the Mi-V PLIC
  module and a pointer to the Mi-V PLIC register data structure. The Mi-V PLIC
  register data structure maps the address of the Mi-V PLIC registers.

  ---------------------------------------------
  Enabling and Disabling interrupts
  ---------------------------------------------
  The MIV_PLIC_enable_irq() function enables the specific interrupt provided by
  user. A call to this function will allow the enabling of each of the global
  interrupts corresponding to the bit in the interrupt enable register of Mi-V
  PLIC.
  The MIV_PLIC_disable_irq() function disables the specific interrupt provided
  by the user. This function can be used to disable the interrupts from outside
  of the external interrupt handler.

  ----------------------------------------
  Interrupt Control
  ----------------------------------------
  When an interrupt occurs on an enabled interrupt, the PLIC gateway captures
  the interrupt and asserts the corresponding interrupt pending bit. Once
  the enable bit and pending bit are asserted, then the PLIC_IRQ signal asserts
  until the interrupt is claimed by the driver interrupt handler MIV_PLIC_irq()
  function.
  When multiple interrupts assert then the lowest interrupt number will be
  serviced first, for example, if interrupt 1 and 6 assert at the same time,
  interrupt 1 will be serviced first, followed by interrupt 6.

*/
#include <stdint.h>
#include "miv_plic_regs.h"

#ifndef LEGACY_DIR_STRUCTURE
#include "hal/hal.h"
#include "miv_rv32_hal/miv_rv32_hal.h"
#else
#include "hal.h"
#include "miv_rv32_hal.h"
#endif

/*-------------------------------------------------------------------------*//**
  This enumeration is used to select a specific Mi-V PLIC interrupt. It is
  used as a parameter to enable or disable the interrupt.
*/
typedef enum miv_plic_irq_num
{
    NoInterrupt_IRQn = 0,
    MIV_PLIC_EXT0_IRQn  = 1,
    MIV_PLIC_EXT1_IRQn  = 2,
    MIV_PLIC_EXT2_IRQn  = 3,
    MIV_PLIC_EXT3_IRQn  = 4,
    MIV_PLIC_EXT4_IRQn  = 5,
    MIV_PLIC_EXT5_IRQn  = 6,
    MIV_PLIC_EXT6_IRQn  = 7,
    MIV_PLIC_EXT7_IRQn  = 8,
    MIV_PLIC_EXT8_IRQn  = 9,
    MIV_PLIC_EXT9_IRQn = 10,
    MIV_PLIC_EXT10_IRQn = 11,
    MIV_PLIC_EXT11_IRQn = 12,
    MIV_PLIC_EXT12_IRQn = 13,
    MIV_PLIC_EXT13_IRQn = 14,
    MIV_PLIC_EXT14_IRQn = 15,
    MIV_PLIC_EXT15_IRQn = 16,
    MIV_PLIC_EXT16_IRQn = 17,
    MIV_PLIC_EXT17_IRQn = 18,
    MIV_PLIC_EXT18_IRQn = 19,
    MIV_PLIC_EXT19_IRQn = 20,
    MIV_PLIC_EXT20_IRQn = 21,
    MIV_PLIC_EXT21_IRQn = 22,
    MIV_PLIC_EXT22_IRQn = 23,
    MIV_PLIC_EXT23_IRQn = 24,
    MIV_PLIC_EXT24_IRQn = 25,
    MIV_PLIC_EXT25_IRQn = 26,
    MIV_PLIC_EXT26_IRQn = 27,
    MIV_PLIC_EXT27_IRQn = 28,
    MIV_PLIC_EXT28_IRQn = 29,
    MIV_PLIC_EXT29_IRQn = 30,
    MIV_PLIC_EXT30_IRQn = 31
} miv_plic_irq_num_t;

/*--------------------------------------------------------------------------*//*
 * This structure maps the priority threshold and claim complete register in
 * the memory.
 */
typedef struct
{
    volatile uint32_t PRIORITY_THRESHOLD;
    volatile uint32_t CLAIM_COMPLETE;
    volatile uint32_t reserved[1022];
} IRQ_Target_Type;

/*--------------------------------------------------------------------------*//*
 * This structure maps the Interrupt enable sources from 0 - 1023 for one
 * context.
 */
typedef struct
{
    volatile uint32_t ENABLES[32];
} Target_Enables_Type;

/*-------------------------------------------------------------------------*//**
  This structure holds the base address of the Mi-V PLIC module. This structure
  is used by all the functions to access the Mi-V PLIC registers.
*/
typedef struct miv_plic_instance
{
    addr_t base_addr;
} miv_plic_instance_t;

/*-------------------------------------------------------------------------*//**
 * The MIV_PLIC_isr is the top level interrupt handler function for the Mi-V PLIC
 * driver. You must call the MIV_PLIC_isr() from the system level interrupt
 * handler(External_IRQHandler).
 * This function must be called from the external interrupt handler function
 * provided by the processor hardware abstraction layer. In case of MIV_RV32
 * soft processor, it must be called from External_IRQHandler() function
 * provided by MIV_RV32 HAL.
 *
 * The MIV_PLIC_isr() function claims the interrupt number
 * that triggered the interrupt and then invokes the appropriate PLIC interrupt
 * handler.
 * After handling the PLIC interrupt, this function will complete the interrupt
 * by clearing the claim complete bit for the particular interrupt source.
 *
 * @param this_plic
 *                  A pointer to the miv_plic_instance_t data structure which
 *                  will hold all the data related to the Mi-V PLIC instance
 *                  being used. A pointer to this data structure is passed to
 *                  rest of Mi-V PLIC driver functions for operation.
 *
 * @return
 *                  This function does not return any value.
 *
 * Example:
 * @code
 *         #define MIV_PLIC_BASE_ADDR          0x70000000
 *         #define PLIC_EXT_INTR_SOURCES       31
 *
 *         miv_plic_instance_t g_plic;
 *         uint8_t MIV_PLIC_EXT0_IRQHandler(void)
 *         {
 *             ***  ISR operation  ***
 *
 *             return(EXT_IRQ_KEEP_ENABLED);
 *         }
 *
 *         void External_IRQHandler(void)
 *         {
 *             uint32_t reg_val = read_csr(mip);
 *             MIV_PLIC_isr(&g_plic);
 *         }
 *
 *         void main(void)
 *         {
 *             MIV_PLIC_init(&g_plic, MIV_PLIC_BASE_ADDR, PLIC_EXT_INTR_SOURCES);
 *
 *             MIV_PLIC_enable_irq(&g_plic, MIV_PLIC_EXT0_IRQn);
 *             MIV_PLIC_enable_irq(&g_plic, MIV_PLIC_EXT1_IRQn);
 *             MIV_PLIC_enable_irq(&g_plic, MIV_PLIC_EXT2_IRQn);
 *             MIV_PLIC_enable_irq(&g_plic, MIV_PLIC_EXT3_IRQn);
 *         }
 * @endcode
 */
void MIV_PLIC_isr(miv_plic_instance_t *this_plic);

/*-------------------------------------------------------------------------*//**
 * The MIV_PLIC_init() function initializes the Mi-V PLIC instance with base
 * address. This function resets the PLIC controller by disabling all the PLIC
 * interrupts.
 *
 * Note: This function must be called before calling any other Mi-V PLIC driver
 * function.
 *
 * @param this_plic
 *                  A pointer to the miv_plic_instance_t data structure which
 *                  will hold all the data related to the Mi-V PLIC instance
 *                  being used. A pointer to this data structure is passed to
 *                  rest of Mi-V PLIC driver functions for operation.
 *
 * @param base_addr
 *                  Base address of the Mi-V PLIC instance in the MIV_ESS soft-IP.
 *
 * @param ext_intr_sources
 *                  Number of interrupts initialized in the design.
 *
 * @return
 *                  This function does not return any value.
 *
 * Example
 * @code
 *          #define MIV_PLIC_BASE_ADDR          0x70000000
 *          #define PLIC_EXT_INTR_SOURCES       31
 *
 *          miv_plic_instance_t g_plic;
 *
 *          void main(void)
 *          {
 *              MIV_PLIC_init(&g_plic, MIV_PLIC_BASE_ADDR, PLIC_EXT_INTR_SOURCES);
 *          }
 * @endcode
 */
static inline void
MIV_PLIC_init
(
    miv_plic_instance_t *this_plic,
    addr_t base_addr,
    uint8_t ext_intr_sources
)
{
    uint32_t inc;
    unsigned long hart_id = read_csr(mhartid);

    this_plic->base_addr = base_addr;

    /* Disable all interrupts for the current hart.
     * The PLIC_EXT_INTR_SOURCES should be defined in the hw_platform.h. This 
     * macro holds the number of PLIC interrupts enabled in the design.
     */
    for(inc = 0; inc < ((ext_intr_sources + 32u) / 32u); ++inc)
    {
        HAL_set_32bit_reg(
                (this_plic->base_addr + inc + (hart_id * 128)), INT_ENABLE , 0x0u);
    }
}

/*-------------------------------------------------------------------------*//**
 * The MIV_PLIC_enable_irq() function enables the PLIC interrupt provided with
 * IRQn parameter.
 *
 * @param this_plic
 *                  A pointer to the miv_plic_instance_t data structure which
 *                  will hold all the data related to the Mi-V PLIC instance
 *                  being used. A pointer to this data structure is passed to
 *                  rest of Mi-V PLIC driver functions for operation.
 * @param IRQn
 *                  Number of PLIC interrupt to enable.
 *
 * @return
 *                 This function does not return any value.
 *
 * Example
 * @code
 *          #define MIV_PLIC_BASE_ADDR          0x70000000
 *          #define PLIC_EXT_INTR_SOURCES       31
 *
 *          miv_plic_instance_t g_plic;
 *
 *          void main(void)
 *          {
 *              MIV_PLIC_init(&g_plic, MIV_PLIC_BASE_ADDR, PLIC_EXT_INTR_SOURCES);
 *
 *              MIV_PLIC_enable_irq(&g_plic, MIV_PLIC_EXT0_IRQn);
 *              MIV_PLIC_enable_irq(&g_plic, MIV_PLIC_EXT1_IRQn);
 *              MIV_PLIC_enable_irq(&g_plic, MIV_PLIC_EXT2_IRQn);
 *              MIV_PLIC_enable_irq(&g_plic, MIV_PLIC_EXT3_IRQn);
 *         }
 * @endcode
 */
static inline void
MIV_PLIC_enable_irq
(
        miv_plic_instance_t *this_plic,
        miv_plic_irq_num_t IRQn
)
{
    unsigned long hart_id = read_csr(mhartid);

    uint32_t current = HAL_get_32bit_reg(
            (this_plic->base_addr + (IRQn/32) + (hart_id * 128)) , INT_ENABLE);

    current |= (uint32_t)1 << (IRQn % 32);

    HAL_set_32bit_reg(
            (this_plic->base_addr + (IRQn/32) + (hart_id * 128)), INT_ENABLE, current);

}

/*-------------------------------------------------------------------------*//**
 * The MIV_PLIC_disable_irq() function disables the PLIC interrupt provided with
 * IRQn parameter.
 *
 * NOTE:
 * This function can be used to disable the PLIC interrupt from outside the
 * external interrupt handler functions.
 * If you wish to disable the PLIC interrupt from the external interrupt handler,
 * you should use the return value of EXT_IRQ_DISABLE. This will disable the
 * selected PLIC interrupt from the Mi-V PLIC driver interrupt handler.
 *
 * @param this_plic
 *                  A pointer to the miv_plic_instance_t data structure which
 *                  will hold all the data related to the Mi-V PLIC instance
 *                  being used. A pointer to this data structure is passed to
 *                  rest of Mi-V PLIC driver functions for operation.
 * @param IRQn
 *                  Number of PLIC interrupt to disable.
 *
 * @return
 *                 This function does not return any value.
 *
 * Example
 * @code
 *          #define MIV_PLIC_BASE_ADDR          0x70000000
 *          #define PLIC_EXT_INTR_SOURCES       31
 *
 *          miv_plic_instance_t g_plic;
 *
 *          void main(void)
 *          {
 *              MIV_PLIC_init(&g_plic, MIV_PLIC_BASE_ADDR, PLIC_EXT_INTR_SOURCES);
 *
 *              MIV_PLIC_enable_irq(&g_plic, MIV_PLIC_EXT0_IRQn);
 *              MIV_PLIC_enable_irq(&g_plic, MIV_PLIC_EXT1_IRQn);
 *              MIV_PLIC_enable_irq(&g_plic, MIV_PLIC_EXT2_IRQn);
 *              MIV_PLIC_enable_irq(&g_plic, MIV_PLIC_EXT3_IRQn);
 *
 *              MIV_PLIC_disable_irq(&g_plic, MIV_PLIC_EXT0_IRQn);
 *              MIV_PLIC_disable_irq(&g_plic, MIV_PLIC_EXT1_IRQn);
 *              MIV_PLIC_disable_irq(&g_plic, MIV_PLIC_EXT2_IRQn);
 *              MIV_PLIC_disable_irq(&g_plic, MIV_PLIC_EXT3_IRQn);
 *         }
 * @endcode
 */
static inline void
MIV_PLIC_disable_irq
(
        miv_plic_instance_t *this_plic,
        miv_plic_irq_num_t IRQn
)
{
    unsigned long hart_id = read_csr(mhartid);

    uint32_t current = HAL_get_32bit_reg((this_plic->base_addr + (IRQn/32) + (hart_id * 128)) , INT_ENABLE);

    current &= ~((uint32_t)1 << (IRQn % 32));

    HAL_set_32bit_reg((this_plic->base_addr + (IRQn/32) + (hart_id * 128)), INT_ENABLE, current);

}
