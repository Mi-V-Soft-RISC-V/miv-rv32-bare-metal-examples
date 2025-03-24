
/*******************************************************************************
 * Copyright 2025 Microchip FPGA Embedded Systems Solutions.
 *
 * SPDX-License-Identifier: MIT
 *
 * @file ethernet_status.h
 * @author Microchip FPGA Embedded Systems Solutions
 * @brief Structure to hold the link status information.
 *
 */

#ifndef ETHERNET_STATUS_H
#define ETHERNET_STATUS_H

#include "drivers/fpga_ip/CoreTSE/core_tse.h"

typedef struct ethernet_status
{
    tse_speed_t speed;
    uint8_t duplex_mode;
} ethernet_status_t;

#endif
