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
 * Helper function public API.
 *
 */
#ifndef __HELPER_H_
#define __HELPER_H_ 1

/******************************************************************************
 * Maximum buffer size.
 *****************************************************************************/
#define MAX_RX_DATA_SIZE    256
#define MASTER_TX_BUFFER    10
#define DATA_LENGTH_32_BYTES  32

/*==============================================================================
  Macro
 */
#define   VALID                   0U
#define   INVALID                 1U
#define   ENTER                   13u

/******************************************************************************
 * CoreUARTapb instance data.
 *****************************************************************************/
extern UART_instance_t g_uart;

uint16_t get_input_data
(
    uint8_t* location,
    uint16_t size,
    const uint8_t* msg,
    uint16_t msg_size
);
void get_key
(
    uint8_t key_type,
    uint8_t* location,
    uint8_t size,
    const uint8_t* msg,
    uint8_t msg_size
);
uint16_t get_data_from_uart
(
    uint8_t* src_ptr,
    uint16_t size,
    const uint8_t* msg,
    uint16_t msg_size
);
void display_output
(
    uint8_t* in_buffer,
    uint32_t byte_length
);

uint32_t get_value_from_uart
(
    uint16_t base
);
#endif /* __HELPER_H_ */
