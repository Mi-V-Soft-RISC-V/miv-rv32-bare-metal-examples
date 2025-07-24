/*******************************************************************************
 * Copyright 2022 Microchip FPGA Embedded Systems Solutions.
 *
 * SPDX-License-Identifier: MIT
 *
 * Mi-V Watchdog Soft IP bare-metal driver. This module is delivered as part of
 * Extended Sub System(ESS) MIV_ESS.
 * Please refer to miv_watchdog.h file for more information.
 */

#include "miv_watchdog.h"

addr_t g_this_wdog;

/***************************************************************************//*
 * Please refer to miv_watchdog.h for more info about this function
 */
void
MIV_WDOG_init
(
    addr_t base_addr
)
{
    /* Register the Mi-V Watchdog base address to the driver */
    g_this_wdog = base_addr;
}

/***************************************************************************//*
 * Please refer to miv_watchdog.h for more info about this function
 */
uint8_t MIV_WDOG_configure
(
    const miv_wdog_config_t  *config
)
{
    uint8_t error = 0u;

    /* check load value and trigger max value */
    if (config->timeout_val <= MIV_WDOG_TRIGGER_MAX)
    {
        HAL_set_32bit_reg(g_this_wdog, WDOGTRIG,(config->timeout_val));
    }
    else
    {
        error = 1u;
    }

    if (config->time_val <= MIV_WDOG_TIMER_MAX)
    {
        HAL_set_32bit_reg(g_this_wdog, WDOGMSVP,(config->mvrp_val));
    }
    else
    {
        error = 1u;
    }

    HAL_set_32bit_reg_field(g_this_wdog, WDOGCNTL_NEXT_ENFORBIDDEN,
                                                  config->forbidden_en);

    /* Reload watchdog with new load if it is not in forbidden window */
    if (!(WDOGSTAT_FORBIDDEN_MASK & (HAL_get_32bit_reg(g_this_wdog, WDOGSTAT))))
    {
        HAL_set_32bit_reg(g_this_wdog, WDOGRFSH, MIV_WDOG_REFRESH_KEY);
    }
    else
    {
        error = 1u;
    }

    return (error);
}

/***************************************************************************//*
 * Please refer to miv_watchdog.h for more info about this function
 */
void MIV_WDOG_get_config
(
    miv_wdog_config_t *config
)
{
    if (0 != g_this_wdog)
    {

        config->time_val = HAL_get_32bit_reg(g_this_wdog, WDOGTIME);

        config->timeout_val = HAL_get_32bit_reg(g_this_wdog, WDOGTRIG);

        config->mvrp_val = HAL_get_32bit_reg(g_this_wdog, WDOGMSVP);

        config->forbidden_en = HAL_get_32bit_reg_field(g_this_wdog,
                                                       WDOGCNTL_NEXT_ENFORBIDDEN);
    }
}

