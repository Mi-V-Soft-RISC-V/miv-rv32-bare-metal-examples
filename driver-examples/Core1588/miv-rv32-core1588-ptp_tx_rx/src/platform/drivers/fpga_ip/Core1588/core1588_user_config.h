/*******************************************************************************
 * (c) Copyright 2025 Microchip FPGA Embedded Systems Solutions.
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
 * @file core1588_user_config.h
 * @author Microchip FPGA Embedded Systems Solutions
 * @brief This file contains the user config options for the Core1588 IP
 * bare metal driver.
 *
 */
/*=========================================================================*/

#ifndef __CORE1588_USER_CONFIG_H
#define __CORE1588_USER_CONFIG_H 1

/***************************************************************************/ /**
   Select buffer size for the individual buffers. It is recommended to select
   buffer sizes that are a power of two.
 */
#define C1588_RX_RING_SIZE 4
#define C1588_TX_RING_SIZE 4
#define C1588_LATCH_RING_SIZE 4
#define C1588_TRIGGER_RING_SIZE 4

/***************************************************************************/ /**
   Enable or disable custom user handlers for interrupt types. If enabled,
   handler must be implemented or the application will halt.
 */
#define C1588_RX_IRQ_USER_HANDLER 0
#define C1588_TX_IRQ_USER_HANDLER 0
#define C1588_LATCH_IRQ_USER_HANDLER 0
#define C1588_TRIGGER_IRQ_USER_HANDLER 0
#define C1588_RTCSEC_IRQ_USER_HANDLER 0

#endif // __CORE1588_USER_CONFIG_H
