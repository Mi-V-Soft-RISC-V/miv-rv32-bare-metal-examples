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
 * Mi-V Timer Soft IP bare-metal driver. This module is delivered as part of
 * the Mi-V Extended Sub System(ESS) MIV_ESS.
 */

/*=========================================================================*//**
  @mainpage Mi-V Timer Bare Metal Driver.
  The Mi-V Timer bare metal software driver supports the timer module which
  serves as a system timer for the Mi-V Extended Sub System(ESS).

  @section intro_sec Introduction
  The MI-V Timer driver supports set of functions for controlling the Mi-V
  Timer module.
  The Mi-V Timer can generate a timer interrupt signal for the system based on
  special system clock intervals specified by the parameters that can be passed
  in by the user.

  The major features provided by Mi-V Timer driver are:
      - Support for Mi-V Timer instance for each Mi-V Timer peripheral.
      - Read current time
      - Write to the machine time compare register

  @section hw_dependencies Hardware Flow dependency
  The application should configure the Mi-V Timer driver through calls to
  MIV_TIMER_init() functions for each MIV_TIMER instance in the hardware
  design. The configuration parameter include the MIV_TIMER hardware instance,
  base address and number of ticks to generate timer interrupt.

  MIV_RV32 core offers flexibility in terms of generating the MTIME and MTIMECMP
  registers internal to the core or using external time reference.
  When MIV_ESS is interfaced with MIV_RV32 core, the timer module in the MIV_ESS
  can be configured as follows:
      - Internal MTIME External MTIME IRQ
        Generate the MTIME internally(MIV_RV32) and have a timer interrupt input
        to the core as external pin(from MIV_ESS).

      - External MTIME Internal MTIME IRQ
        Generate the time value externally(from MIV_ESS), in this case a 64-bit
        port will open in the MIV_RV32 core as input and MIV_ESS will output the
        64-bit TIME_COUNT value. The generation of mtimecmp and interrupt is
        done internally(MIV_RV32).

      - External MTIME External MTIME IRQ
        Generate both the time and timer interrupt externally.
        In this case 64-bit port will be available on the Mi-V RV32 core as input
        and a 1 pin port will be available for timer interrupt.

  The design must be configured accordingly to use these combinations in the
  firmware.

  No MIV_TIMER hardware configuration parameters are used by the driver, apart
  from MIV_TIMER base address. Hence, no additional configuration files are
  required to use the driver.

  @section theory_op Theory of Operation

  The MIV_TIMER module is a simple systick timer which can generate a timer
  interrupt signal for the  system at specific intervals specified by the
  parameters that can be passed by the user.
  These interrupt signal are then fed to the MIV_RV32 core via timer interrupt.

  The operation of MIV_TIMER is divided into following steps:
      - Initialization
      - Configuration
      - Read/Write TIME

  ## Initialization
  The MIV_TIMER is first initialized by a call to MIV_TIMER_init(). This
  function initializes the instance of Mi-V TIMER with the base address.
  The MIV_TIMER_init() function must be called before any other Mi-V Timer driver
  function.

  ## Configuration
  The Mi-V TIMER configuration includes writing the mtimecmp register with the
  initial time value at which timer interrupt should be generated.
  When the mtime register value becomes greater than or equal to mtimecmp value,
  a timer interrupt signal(TIMER_IRQ) is generated.

  ## Read/Write TIME
  The time value can be read by reading the mtime register via call to the
  MIV_TIMER_read_mtime(). This function reads the MTIME register which contains
  the 64-bit value of the timer count. The count increments by 1 every time the
  prescale ticks. This function returns 64-bit MTIME_COUNT value which is the
  current value of timer count.

  The time value read in the MIV_TIMER_read_mtime() function can be written to
  the mtimecmp register by calling MIV_TIMER_write_mtimecmp() to generate
  periodic interrupts.
  The writing of the mtimecmp register should be done in the systick_handler()
  function.
 */

#ifndef MIV_TIMER_H_
#define MIV_TIMER_H_

#ifdef __cplusplus
extern "C" {
#endif

#ifndef LEGACY_DIR_STRUCTURE
#include "hal/hal.h"

#else
#include "hal.h"
#endif
/*-------------------------------------------------------------------------*//**
MIV_TIMER_SUCCESS 
=====================

The MIV_TIMER_SUCCESS constant indicates successful configuration of 
Mi-V Timer module.
*/
#define MIV_TIMER_SUCCESS                        0u

/*-------------------------------------------------------------------------*//**
MIV_TIMER_ERROR 
=====================

The MIV_TIMER_ERROR constant indicates that there is an error with
configuring the Mi-V Timer module.
*/
#define MIV_TIMER_ERROR                          1u

/*-------------------------------------------------------------------------*//*
MIV_TIMER_MASK_32BIT
=====================

32-bit mask constant used in calculation of 64-bit register value.
*/
#define MIV_TIMER_MASK_32BIT                               0xFFFFFFFFu

/*-------------------------------------------------------------------------*//*
Mi-V Timer register offsets 
=====================
The MTIMECMP is the 64-bit timer compare register, it pre-sets the threshold
which needs to be reached by the timer count register.
This 64-bit register is accessed with 2 32-bit address offset, lower 32-bits
and higher 32-bits.
 - MIV_TIMER_MTIMECMP_L_REG_OFFSET
 - MIV_TIMER_MTIMECMP_H_REG_OFFSET

The MTIME is the 64-bit register that contains the 64-bit timer count. The
count increments by 1 every time the prescaler ticks.
This 64-bit register is accessed with 2 32-bit address offset, lower 32-bits
and higher 32-bits.
 - MIV_TIMER_MTIME_L_REG_OFFSET
 - MIV_TIMER_MTIME_H_REG_OFFSET

The PRESCALE register is used to determine the amount of clock cycles the
selected clock needs to go through, for MTIME register to increment count.
 - MIV_TIMER_PRESCALAR_REG_OFFSET
*/

/// @cond private
#define MIV_TIMER_MTIMECMP_L_REG_OFFSET                     0x4000u
#define MIV_TIMER_MTIMECMP_H_REG_OFFSET                     0x4004u

#define MIV_TIMER_MTIME_L_REG_OFFSET                        0xBFF8u
#define MIV_TIMER_MTIME_H_REG_OFFSET                        0xBFFCu

#define MIV_TIMER_PRESCALAR_REG_OFFSET                      0x5000u
/// @endcond 

/*-------------------------------------------------------------------------*//**
 This structure holds the base address of the Mi-V Timer module and instance
 of the Mi-V Timer register structure.
*/
typedef struct miv_timer_instance
{
    addr_t base_addr;
} miv_timer_instance_t;

/** The MIV_TIMER_init() is used to initialize the Mi-V Timer module. This
  function will assign the base addresses of the Mi-V Timer module.
  User should call this function before calling any of the Mi-V Timer driver
  APIs.

  @param this_timer
         Timer structure which holds the base address for the Mi-V Timer hardware
         instance.

  @param base_address
         Base address of the Mi-V Timer module.

  @return
         This function does not return any value.
 */
static inline void
MIV_TIMER_init
(
    miv_timer_instance_t* this_timer,
    addr_t base_addr
)
{
    this_timer->base_addr = base_addr;
}

/** MIV_TIMER_read_current_time() is used to read the mtimecmp register values.

  @param this_timer
         Timer structure which holds the base address for the Mi-V Timer hardware
         instance.

  @return
  This function returns 64-bit mtimecmp register value.
 */
static inline uint64_t
MIV_TIMER_read_current_time
(
    miv_timer_instance_t* this_timer
)
{
    volatile uint64_t read_data = 0u;
    volatile uint32_t mtime_hi = 0u;
    volatile uint32_t mtime_lo = 0u;

    /* when mtime lower word is 0xFFFFFFFF, there will be rollover and
     * returned value could be wrong. */
    do {
        mtime_hi = HAL_get_32bit_reg(this_timer->base_addr, MIV_TIMER_MTIME_H);
        mtime_lo = HAL_get_32bit_reg(this_timer->base_addr, MIV_TIMER_MTIME_L);

    } while(mtime_hi != HAL_get_32bit_reg(this_timer->base_addr, MIV_TIMER_MTIME_H));

    read_data = mtime_hi;

    return(((read_data) << 32u) | mtime_lo);
}

/** MIV_TIMER_write_compare_time() is used to write to the MTIMECMP register in
  the event of interrupt. User must use this function in the interrupt handler
  to de-assert the MIV_TIMER interrupt.

  @param this_timer
         Timer structure which holds the base address for the Mi-V Timer hardware
         instance.

  @param write_value
         Value to write into the mtimecmp register.

  @return
         This function does not return any value.
 */
static inline void
MIV_TIMER_write_compare_time
(
    miv_timer_instance_t* this_timer,
    uint64_t compare_reg_value
)
{
      HAL_set_32bit_reg(this_timer->base_addr, MIV_TIMER_MTIMECMP_H, MIV_TIMER_MASK_32BIT);

      HAL_set_32bit_reg(this_timer->base_addr, MIV_TIMER_MTIMECMP_L,
                                              (compare_reg_value & MIV_TIMER_MASK_32BIT));

      HAL_set_32bit_reg(this_timer->base_addr, MIV_TIMER_MTIMECMP_H,
                                      ((compare_reg_value >> 32u) & MIV_TIMER_MASK_32BIT));
}

/** The MIV_TIMER_config() is used to configure the MIV_ESS Timer module. The
   prescale value serves to divide the count of clock cycles for the timer and
   provides control over what point in time, the timer interrupt gets
   asserted.

  @param this_timer
         Timer structure which holds the base address for the Mi-V Timer hardware
         instance.

  @param ticks
         Number of ticks after which interrupt will be generated.

  @return
         This function returns Mi-V Timer configuration status.
 */
static inline uint32_t
MIV_TIMER_config
(
    miv_timer_instance_t* this_timer,
    uint64_t ticks
)
{
    uint32_t ret_val = MIV_TIMER_ERROR;
    uint64_t mtime_val = 0u;
    uint32_t prescalar = 0u;
    uint64_t miv_timer_increment = 0U;

    prescalar = HAL_get_32bit_reg(this_timer->base_addr, MIV_TIMER_PRESCALAR);

    miv_timer_increment = (uint64_t)(ticks) / prescalar;

    if (miv_timer_increment > 0U)
    {
        mtime_val = MIV_TIMER_read_current_time(this_timer);

        MIV_TIMER_write_compare_time(this_timer ,(mtime_val + miv_timer_increment));

        ret_val = MIV_TIMER_SUCCESS;
    }

    return ret_val;
}
  
#ifdef __cplusplus
}
#endif

#endif  /* MIV_TIMER_H */
