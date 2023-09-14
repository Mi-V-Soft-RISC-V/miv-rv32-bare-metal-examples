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
 * Watchdog module driver. This module is delivered as a part of Mi-V Extended 
 * Sub-System(MIV_ESS).
 */
/*=========================================================================*//**
  @mainpage MiV Watchdog Bare Metal Driver

  ==============================================================================
  Introduction
  ==============================================================================
  The Mi-V Watchdog module in the MIV_ESS is used to generate a reset for the
  system automatically if the software doesn't periodically update or refresh
  the timer countdown register. This software driver provides set of functions
  for controlling Mi-V Watchdog module as a part of bare metal system where no
  operating system is available. The driver can be adapted for use as a part of
  an operating system, but the implementation of the adaptation layer between
  the driver and the operating system's driver model is outside the scope of the
  driver.

  Mi-V Watchdog provides following features:
      - Initializing the Mi-V Watchdog
      - Reading current value and status of watchdog timer
      - Refreshing the watchdog timer value
      - Enabling, disabling and clearing timeout and Maximum Value up to which
        Refresh is Permitted (MVRP) interrupts.

  ==============================================================================
  Hardware Flow Dependencies
  ==============================================================================
  The application software should initialize and configure the Mi-V Watchdog
  module the the call to the MIV_WDOG_init() and MIV_WDOG_configure() functions.

  No MIV_WDOG hardware configuration parameter are used by the driver, apart
  from the MIV_WDOG base address. Hence, no additional configuration files are
  required to use the driver.

  ==============================================================================
  Theory of Operation
  ==============================================================================
  The Mi-V Watchdog driver functions are grouped into the following categories:
    - Initialization and configuration
    - Reading the current value and status of the watchdog timer
    - Refreshing the watchdog timer value
    - Support for enabling, disabling and clearing time-out and MVRP interrupts.

  --------------------------------
  Initialization and Configuration
  --------------------------------
  The MIV_WDOG_init() function stores the base of MIV_WDT module in the MIV_ESS.
  This base address is used by rest of the functions to access the Mi-V Watchdog
  registers. Please make call this function before calling any other function
  from this driver.
  Note: The Mi-V Watchdog driver supports only one instance of MIV_WDT in the
  hardware.
  The Mi-V Watchdog driver provides the MIV_WDOG_configure() function to
  configure the MIV_WDOG module with desired configuration values. It also
  provides the MIV_WDOG_get_config() to read back the current configuration of
  the MIV_WDOG. You can use this function to retrieve the current configurations
  and then overwrite them with the application specific values, such as initial
  watchdog timer value, Maximum Value (up to which) Refresh (is) Permitted,
  watchdog time-out value, enable/disable forbidden region, enable/disable
  MVRP interrupt and interrupt type.

  --------------------------------------------
  Reading the Watchdog Timer Value and Status
  --------------------------------------------
  Mi-V Watchdog is a down counter. A refresh forbidden window can be created by
  configuring the watchdog Maximum Value up to which Refresh is Permitted (MVRP).
  When the current value of the watchdog timer is greater than the MVRP value,
  refreshing the watchdog is forbidden. Attempting to refresh the watchdog timer
  in the forbidden window will assert a timeout interrupt. The
  MIV_WDOG_forbidden_status() function can be used to know whether the watchdog
  timer is in forbidden window or has crossed it. By default, the forbidden
  window is disabled. It can be enabled by providing an appropriate value as
  parameter to the MIV_WDOG_configure() function. When the forbidden window is
  disabled, any attempt to refresh the watchdog timer is ignored and the counter
  keeps on down counting.

  The current value of the watchdog timer can be read using the
  MIV_WDOG_current_value() function. This function can be called at any time.

  --------------------------------------------
  Refreshing the Watchdog Timer Value
  --------------------------------------------
  The watchdog timer value is refreshed using the MIV_WDOG_reload() function.
  The value reloaded into the watchdog timer down-counter is specified at the
  configuration time with an appropriate value as parameter to the
  MIV_WDOG_get_config() function.

  --------------------------------------------
  Interrupt Control
  --------------------------------------------
  The Mi-V Watchdog generates two interrupts, The MVRP interrupt and
  the timeout interrupt.
  The MVRP interrupt is generated when the watchdog down-counter crosses the
  Maximum Value up to which Refresh is Permitted (MVRP). Following functions to
  control MVRP interrupt:
    - MIV_WDOG_enable_mvrp_irq
    - MIV_WDOG_disable_mvrp_irq
    - MIV_WDOG_clear_mvrp_irq

  The timeout interrupt is generated when the watchdog down-counter crosses the
  watchdog timeout value. The timeout value is a non-zero value and it can be
  set to a maximum of MIV_WDOG_TRIGGER_MAX. The non-maskable interrupt is
  generated when the watchdog crosses this timeout value, the down counter
  keeps on down counting and a reset signal is generated when reaches zero.
  Following functions to control timeout interrupt:
    - MIV_WDOG_enable_timeout_irq
    - MIV_WDOG_disable_timeout_irq
    - MIV_WDOG_clear_timeout_irq

*//*=========================================================================*/

#ifndef MIV_WATCHDOG_H_
#define MIV_WATCHDOG_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include "miv_watchdog_regs.h"

#ifndef LEGACY_DIR_STRUCTURE
#include "hal/hal.h"
#else
#include "hal.h"
#endif

/****************************************************************************//*
 * The following constants can be used to configure the Mi-V Watchdog where a
 * zero or non-zero value such as enable or disable is to be provided as a input
 * parameter as shown below:
 */
#define MIV_WDOG_ENABLE                            1u
#define MIV_WDOG_DISABLE                           0u

/***************************************************************************//**
  The miv_wdog_config_t type for the watchdog Configuration structure. This
  type is used as a parameter for the MIV_WDOG_configure() and the
  MIV_WDOG_get_config() functions.

  Following are the values as part of this structure
|  Parameter       |                         Description                       |
|------------------|-----------------------------------------------------------|
|   time_val       | The value from which the watchdog timer counts down       |
|   mvrp_val       | The Watchdog MVRP value                                   |
|   timeout_val    | The watchdog timeout value                                |
|   forbidden_en   | Enable/disable the forbidden window                       |
|                  | When set, if a refresh occurs in the forbidden window,    |
|                  | the watchdog timeout interrupt will be generated.         |

Time calculation example:

   time_val = 0xFFFFF0u
   mvrp_val = 0x989680u
   timeout_val = 0x3e8u

   A prescaler = 256 is used.
   Considering clock = 50Mhz

   The MVRP interrupt will happen after
   (0xFFFFF0 - 0x989680) * ( 1/(50MHz/256))
   mvrp interrupt will happen after 34 sec. after system reset

   (0xFFFFF0 - 0x3e8) * ( 1/(50MHz/256))
   timeout interrupt will happen after 85 sec. after system reset
 */
typedef struct miv_wdog_config
{
    uint32_t      time_val;
    uint32_t      mvrp_val;
    uint32_t      timeout_val;
    uint32_t      forbidden_en;
    uint32_t      intr_type;
}miv_wdog_config_t;

extern addr_t g_this_wdog;

/***************************************************************************//*
 Internal constants and types
*******************************************************************************/

/// @cond private
#define MIV_WDOG_TRIGGER_MAX                4095u
#define MIV_WDOG_TIMER_MAX                  16777200u
/// @endcond

/*-------------------------------------------------------------------------*//*
MIV_WDOG_REFRESH_KEY 
=====================

The MIV_WDOG_REFRESH_KEY macro holds the magic value which will cause a
reload of the watchdog's down counter when written to the watchdog's
WDOGREFRESH register.
*/
#define MIV_WDOG_REFRESH_KEY         (uint32_t)0xDEADC0DEU

/*-------------------------------------------------------------------------*//*
MIV_WDOG_FORCE_RESET_KEY 
=====================
The MIV_WDOG_FORCE_RESET_KEY macro holds the magic value which will force a
reset if the watchdog is already timeout. Writing any other value or writing
TRIGGER register at other times will trigger the watchdog NMI sequence
(i.e raise a timeout interrupt)
 */
#define MIV_WDOG_FORCE_RESET_KEY     (uint32_t)0xDEADU

/***************************************************************************//**
 * The MIV_WDOG_init() is used to register the Mi-V Watchdog module base
 * address to the driver.
 *
 * Note: User should call this function before calling any other Mi-V watchdog
 * driver function.
 *
 * @param base_addr
 *     The base address of the Mi-V watchdog module. This address is used by
 *     rest of the watchdog driver functions to access the registers.
 *
 * @return
 *    This function does not return any value.
 */
void
MIV_WDOG_init
(
    addr_t base_addr
);

/***************************************************************************//**
 * The MIV_WDOG_get_config() function returns the current configurations of the
 * Mi-V Watchdog. The Mi-V Watchdog is pre-initialized by the flash
 * bits at the design time. When used for the first time before calling the
 * MIV_WDOG_configure() function, this function will return the default
 * configurations as configured at the design time.
 *
 * @param config
 *   The config parameter is used to store the current configuration of the Mi-V
 *   Watchdog.
 *
 *   Please see the description of miv_wdog_config_t for details.
 *
 * @return
 *   This function does not return any value.
 *
 * Example:
 */
void
MIV_WDOG_get_config
(
    miv_wdog_config_t* config
);

/***************************************************************************//**
 * The MIV_WDOG_configure() function configures the watchdog module. The
 * Watchdog module is pre-initialized by the flash bits at the design time to the
 * default values. You can reconfigure the Watchdog module using
 * MIV_WDOG_configure() function.
 *
 * Note that the MIV_WDOG_configure() function can be used only once, as it
 * writes into the TIME register. After a write into the TIME register, the TIME,
 * TRIGGER and MSVP register values are frozen and can't be altered again unless
 * a system reset happens.
 *
 * Note also that the Mi-V Watchdog is not enabled at reset, calling this function
 * will start the watchdog, it cannot then be disabled and must be refreshed
 * periodically.
 *
 * @param config
 *   The config parameter is the input parameter in which the configurations to
 *   be applied to the watchdog module are provided by the application.
 *   Please see the description of miv_wdog_config_t for details.
 *
 * @return
 *   This function returns a zero value when executed successfully. A non-zero
 *   value is returned when the configuration values are out of bound.
 *
 * Example:
 */
uint8_t 
MIV_WDOG_configure
(
    const miv_wdog_config_t * config
);

/***************************************************************************//**
 * The MIV_WDOG_reload() function causes the watchdog to reload its down-counter
 * timer with the load value configured through interrupt handler. This function
 * must be called regularly to avoid a system reset or a watchdog interrupt.
 * 
 * @param 
 *   Void
 *
 * @return
 *   This function does not return a value.
 */
static inline void 
MIV_WDOG_reload
(
    void
)
{
    if (0 != g_this_wdog)
    {
        HAL_set_32bit_reg(g_this_wdog, WDOGRFSH, MIV_WDOG_REFRESH_KEY);
    }
}

/***************************************************************************//**
 * The MIV_WDOG_current_value() function returns the current value of the
 * watchdog's down-counter.
 * 
 * @param 
 *   Void
 *
 * @return
 *   This function returns the current value of the watchdog’s down-counter as
 *   a 32-bit unsigned integer.
 */
static inline uint32_t 
MIV_WDOG_current_value
(
    void
)
{
    return (HAL_get_32bit_reg(g_this_wdog, WDOGRFSH));
}

/***************************************************************************//**
 * The MIV_WDOG_forbidden_status() function returns the refresh status of the
 * Mi-V Watchdog.
 * 
 * @param 
 *   Void
 *
 * @return
 *   This function returns the refresh status of the watchdog. A value of 1
 *   indicates that watchdog's down-counter is within the forbidden window and
 *   that a reload should not be done. A value of 0 indicates that the watchdog's
 *   down counter is within the permitted window and that a reload is allowed.
 */
static inline uint32_t
MIV_WDOG_forbidden_status
(
    void
)
{
    return (HAL_get_32bit_reg_field(g_this_wdog, WDOGSTAT_FORBIDDEN));
}

/***************************************************************************//**
 * The MIV_WDOG_enable_mvrp_irq() function enables the MVRP interrupt.
 * This interrupt is asserted when the timer countdown register leaves the
 * maximum value up to which refresh is permitted (MVRP) window.
 * 
 * @param 
 *   Void
 *
 * @return
 *   This function does not return a value.
 *
 * Example:
 */
static inline void
MIV_WDOG_enable_mvrp_irq
(
    void
)
{

    if (0 != g_this_wdog)
    {
        HAL_set_32bit_reg_field(g_this_wdog, WDOGCNTL_NEXT_INTENT_MSVP, 0x01u);
    }
}

/***************************************************************************//**
 * The MIV_WDOG_disable_mvrp_irq() function disables the generation of the
 * MVRP interrupt.
 * 
 * @param 
 *   Void
 *
 * @return
 *   This function does not return a value.
 */
static inline void
MIV_WDOG_disable_mvrp_irq
(
    void
)
{
    if (0 != g_this_wdog)
    {
        HAL_set_32bit_reg_field(g_this_wdog, WDOGCNTL_NEXT_INTENT_MSVP, 0x0u);
    }
}

/***************************************************************************//**
 * The MIV_WDOG_clear_timeout_irq() function clears the watchdog’s timeout
 * interrupt which is connected to the MIV-RV32 interrupt. Calling
 * MIV_WDOG_clear_timeout_irq() results in clearing the MIV-RV32 interrupt.
 * Note: You must call the MIV_WDOG_clear_timeout_irq() function as part of your
 *       implementation of the interrupt handler in order to prevent the same
 *       interrupt event re-triggering a call to the timeout ISR.
 * 
 * @param 
 *   Void
 *
 * @return
 *   This function does not return any value.
 *
 */
static inline void
MIV_WDOG_clear_timeout_irq
(
    void
)
{
    if (0 != g_this_wdog)
    {
        HAL_set_32bit_reg_field(g_this_wdog, WDOGSTAT_WDOG_TRIPPED, 0x01u);
        /*
         * Perform a second write to ensure that the first write completed before
         * returning from this function. This is to account for posted writes across
         * the AHB matrix. The second write ensures that the first write has
         * completed and that the interrupt line has been de-asserted by the time
         * the function returns. Omitting the second write may result in a delay
         * in the de-assertion of the interrupt line going to the RISC-V and a
         * retriggering of the interrupt.
         */
        HAL_set_32bit_reg_field(g_this_wdog, WDOGSTAT_WDOG_TRIPPED, 0x01u);
    }
}

/***************************************************************************//**
 * The MIV_WDOG_clear_mvrp_irq() function clears the mvrp interrupt.
 *
 * Note: You must call the MIV_WDOG_clear_mvrp_irq() function as part of  your
 *       implementation of the interrupt service routine (ISR) in order to
 *       prevent the same interrupt event re-triggering a call to the mvrp ISR.
 * 
 * @param 
 *   Void
 *
 * @return
 *   This function does not return a value.
 */
static inline void
MIV_WDOG_clear_mvrp_irq
(
    void
)
{
    if (0 != g_this_wdog)
    {
        HAL_set_32bit_reg_field(g_this_wdog, WDOGSTAT_MSVP_TRIPPED, 0x01u);
        /*
         * Perform a second write to ensure that the first write completed before
         * returning from this function. This is to account for posted writes across
         * the AHB matrix. The second write ensures that the first write has
         * completed and that the interrupt line has been de-asserted by the time
         * the function returns. Omitting the second write may result in a delay
         * in the de-assertion of the interrupt line going to the RISC-V and a
         * re-triggering of the interrupt.
         */
        HAL_set_32bit_reg_field(g_this_wdog, WDOGSTAT_MSVP_TRIPPED, 0x01u);
    }
}

/***************************************************************************//**
 * The MIV_WDOG_timeout_occured() function reports the occurrence of a timeout
 * event.
 * 
 * @param 
 *   Void
 *
 * @return
 *   A zero value indicates no watchdog timeout event occurred. A value of 1
 *   indicates that a timeout event occurred.
 */
static inline uint32_t
MIV_WDOG_timeout_occured
(
    void
)
{
    return (HAL_get_32bit_reg_field(g_this_wdog, WDOGSTAT_TRIGGERED));
}

/***************************************************************************//**
 * The MIV_WDOG_force_reset() function is used to force an immediate reset
 * if the watchdog has already triggered. Writing any value in this condition
 * will result in watchdog timeout.
 * The time out interrupt WDOG_IRQ will be set to high and watchdog timer
 * countdown register updated with watchdog trigger timeout register value.
 * If the Watchdog has timed out, a special 16-bit value needs to be written
 * to the register to force a reset on CPU_RESETN, 0xDEAD
 * Then the Watchdog countdown is reset/updated with the top Watchdog Runtime
 * register value.
 * 
 * @param
 *   Void
 * 
 * @return
 *   This function does not return a value.
 */
static inline void
MIV_WDOG_force_reset
(
    void
)
{
    if (WDOGSTAT_TRIGGERED_MASK ==
            HAL_get_32bit_reg_field(g_this_wdog, WDOGSTAT_TRIGGERED))

    {
        HAL_set_32bit_reg(g_this_wdog, WDOGFORCE, MIV_WDOG_FORCE_RESET_KEY);
    }

    else
    {
        HAL_set_32bit_reg(g_this_wdog, WDOGFORCE, 0x0u);
    }
}

#ifdef __cplusplus
}
#endif

#endif /* MIV_WATCHDOG_H_ */
