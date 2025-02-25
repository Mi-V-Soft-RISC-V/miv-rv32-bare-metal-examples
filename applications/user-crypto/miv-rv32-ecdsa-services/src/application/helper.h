/*******************************************************************************
 * Copyright 2019 Microchip FPGA Embedded Systems Solutions.
 *
 * SPDX-License-Identifier: MIT
 *
 * @file helper.h
 * @author Microchip FPGA Embedded Systems Solutions
 * @brief Helper function public API.
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
    uint32_t byte_length,
    uint8_t reverse_buffer
);
uint8_t enable_dma
(
    const uint8_t* msg,
    uint8_t msg_size
);
#endif /* __HELPER_H_ */
