 /*******************************************************************************
 * Copyright 2022 Microchip FPGA Embedded Systems Solutions.
 *
 * SPDX-License-Identifier: MIT
 *
 * This file contains Register bit offsets and masks definitions for MI-V Soft
 * IP watchdog module driver. This module is delivered as a part of Mi-V 
 * extended Sub-System(ESS) MIV_ESS.
 */

#ifndef MIV_WDOG_REGISTERS
#define MIV_WDOG_REGISTERS          1u

/*------------------------------------------------------------------------------
 * Refresh register details
 */
#define WDOGRFSH_REG_OFFSET                         0x00u

/* Refresh register bits */
#define WDOGRFSH_OFFSET                             0x00u
#define WDOGRFSH_MASK                               0xFFFFFFFFu
#define WDOGRFSH_SHIFT                              0u

/*------------------------------------------------------------------------------
 * Control register details
 */
#define WDOGCNTL_REG_OFFSET                         0x04u

/* Control register next intent msvp  bit */
#define WDOGCNTL_NEXT_INTENT_MSVP_OFFSET            0x04u
#define WDOGCNTL_NEXT_INTENT_MSVP_MASK              0x01u
#define WDOGCNTL_NEXT_INTENT_MSVP_SHIFT             0u

/* Control register next intent wdog bit */
#define WDOGCNTL_NEXT_INTENT_WDOG_OFFSET            0x04u
#define WDOGCNTL_NEXT_INTENT_WDOG_MASK              0x02u
#define WDOGCNTL_NEXT_INTENT_WDOG_SHIFT             1u

/* Control register next enforbidden bit */
#define WDOGCNTL_NEXT_ENFORBIDDEN_OFFSET            0x04u
#define WDOGCNTL_NEXT_ENFORBIDDEN_MASK              0x04u
#define WDOGCNTL_NEXT_ENFORBIDDEN_SHIFT             2u

/*------------------------------------------------------------------------------
 * Watchdog status register
 */
#define WDOGSTAT_REG_OFFSET                         0x08u

/* msvp_tripped bit */
#define WDOGSTAT_MSVP_TRIPPED_OFFSET                0x08u
#define WDOGSTAT_MSVP_TRIPPED_MASK                  0x01u
#define WDOGSTAT_MSVP_TRIPPED_SHIFT                 0u

/* WDOG Tripped bit */
#define WDOGSTAT_WDOG_TRIPPED_OFFSET                0x08u
#define WDOGSTAT_WDOG_TRIPPED_MASK                  0x02u
#define WDOGSTAT_WDOG_TRIPPED_SHIFT                 1u

/* Forbidden bit */
#define WDOGSTAT_FORBIDDEN_OFFSET                   0x08u
#define WDOGSTAT_FORBIDDEN_MASK                     0x04u
#define WDOGSTAT_FORBIDDEN_SHIFT                    2u

/* Triggered bit */
#define WDOGSTAT_TRIGGERED_OFFSET                   0x08u
#define WDOGSTAT_TRIGGERED_MASK                     0x08u
#define WDOGSTAT_TRIGGERED_SHIFT                    3u

/* wdoglocked bit */
#define WDOGSTAT_WDOGLOCKED_OFFSET                  0x08u
#define WDOGSTAT_WDOGLOCKED_MASK                    0x10u
#define WDOGSTAT_WDOGLOCKED_SHIFT                   4u

/*------------------------------------------------------------------------------
 * Watchdog runtime register
 */
#define WDOGTIME_REG_OFFSET                         0x0Cu

/* wdogmsvp bit */
#define WDOGTIME_WDOGVALUE_OFFSET                   0x0Cu
#define WDOGTIME_WDOGVALUE_MASK                     0xFFFFFFu
#define WDOGTIME_WDOGVALUE_SHIFT                    0u

/*------------------------------------------------------------------------------
 * Watchdog MVRP register
 */
#define WDOGMSVP_REG_OFFSET                         0x10u

/* wdogmsvp bit */
#define WDOGMSVP_OFFSET                             0x10u
#define WDOGMSVP_MASK                               0xFFFFFFu
#define WDOGMSVP_SHIFT                              0u

/*------------------------------------------------------------------------------
 * Watchdog Trigger Timeout register
 */
#define WDOGTRIG_REG_OFFSET                          0x14u

/* wdogmsvp bit */
#define WDOGTRIG_WDOGRST_OFFSET                      0x14u
#define WDOGTRIG_WDOGRST_MASK                        0xFFFFFFu
#define WDOGTRIG_WDOGRST_SHIFT                       0u

/*------------------------------------------------------------------------------
 * Watchdog Force Reset register details
 */
#define WDOGFORCE_REG_OFFSET                        0x18u

/* Refresh register bits */
#define WDOGFORCE_OFFSET                             0x18u
#define WDOGFORCE_MASK                               0xFFFFFFFFu
#define WDOGFORCE_SHIFT                              0u

#endif
