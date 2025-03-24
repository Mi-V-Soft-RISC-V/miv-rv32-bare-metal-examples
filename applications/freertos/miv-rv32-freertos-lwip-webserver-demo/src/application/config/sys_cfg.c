
/*******************************************************************************
 * Copyright 2025 Microchip FPGA Embedded Systems Solutions.
 *
 * SPDX-License-Identifier: MIT
 *
 * @file sys_cfg.c
 * @author Microchip FPGA Embedded Systems Solutions
 * @brief Setting the device's MAC address
 *
 */

#include "drivers/fpga_ip/CoreTSE/core_tse.h"

const uint8_t *
sys_cfg_get_mac_address(void)
{
    static uint8_t mac_address[6];

    mac_address[0] = 0xC0u;
    mac_address[1] = 0xB1u;
    mac_address[2] = 0x3Cu;
    mac_address[3] = 0x61u;
    mac_address[4] = 0x60u;
    mac_address[5] = 0x60u;

    return (const uint8_t *)mac_address;
}
