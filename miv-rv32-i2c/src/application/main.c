/*******************************************************************************
 * Copyright 2022 Microchip FPGA Embedded Systems Solutions.
 *
 * SPDX-License-Identifier: MIT
 *
 * Sample project to demonstrate the MIV I2C functionality.
 *
 * This is a basic example to demonstrate read and read_write operation to
 * EEPROM memory in the Mikroe Dual EE Click board.
 *
 * Please refer README.md in the root folder of this project for more details.
 */

#include <stdio.h>
#include "fpga_design_config/fpga_design_config.h"
#include "miv_rv32_hal/miv_rv32_hal.h"
#include "drivers/fpga_ip/CoreUARTapb/core_uart_apb.h"
#include "drivers/fpga_ip/miv_i2c/miv_i2c.h"

/******************************************************************************
 * MIV_ESS Peripheral instance data
 *****************************************************************************/
UART_instance_t g_uart;

miv_i2c_instance_t miv_i2c;

miv_i2c_status_t status;

/* Mikroe Dual EE slave addresses. There are two EEPROM IC's in the Dual EE Click
 * board.
 * Each slave address indicate the addresses of each EEPROM IC.
 */
#define DUALEE_SLAVEADDRESS_1                    0x50
#define DUALEE_SLAVEADDRESS_2                    0x54

/* Mikroe Dual EE device constants */
#define EEPROM_PAGE_SIZE                         256u

#define EEPROM_1_PAGE_START                      0x00000000u
#define EEPROM_1_PAGE_END                        0x0003FFFFu

#define EEPROM_2_PAGE_START                      0x00040000u
#define EEPROM_2_PAGE_END                        0x0004FFFFu

/*--------------------------------------------------------------------------//**
 * Read and write buffers.
 */
uint8_t  i2c_tx_buffer[EEPROM_PAGE_SIZE];
uint16_t  write_length = EEPROM_PAGE_SIZE;

uint8_t  i2c_rx_buffer[EEPROM_PAGE_SIZE];
uint16_t read_length = EEPROM_PAGE_SIZE;

/*==============================================================================
 * Messages displayed over the UART.
 */
const uint8_t g_greeting_msg[] =
"\r\n***************************************************************************\r\n\n\
********************    Mi-V I2C Example    ***********************\r\n\n\
*****************************************************************************\r\n\r\n\
Basic Mi-V I2C example to demonstrate read and read_write operation.\
Please check the buffer contents to match the data read from the Dual EE Click board. \r\n";

static void display_greeting(void);

/*==============================================================================
 * Display greeting message when application is started.
 */
static void display_greeting(void)
{
    UART_polled_tx_string(&g_uart, g_greeting_msg);
}

void MSYS_EI2_IRQHandler(void)
{
    MIV_I2C_isr (&miv_i2c);
}

/******************************************************************************
 * main function.
 *****************************************************************************/
void main(void)
{
    size_t rx_size;
    uint8_t rx_buff[1] = {0x00};

    volatile uint8_t miv_i2c_status = 0u;

    uint32_t *src_mem  = (uint32_t*)0x80000000;
    uint32_t *dest_mem = (uint32_t*)0x60000000;

    /* Initialize CoreUARTapb with its base address, baud value, and line
     * configuration.
     */
    UART_init(&g_uart, COREUARTAPB0_BASE_ADDR, BAUD_VALUE_115200, (DATA_8_BITS | NO_PARITY));

    display_greeting();

    MRV_enable_local_irq( MRV32_MSYS_EIE2_IRQn );

    HAL_enable_interrupts();

    static uint32_t page_address = EEPROM_2_PAGE_START;
    uint16_t transfer_size = 64;

    i2c_tx_buffer[0] = page_address >> 8; /* Page address */
    i2c_tx_buffer[1] = page_address;      /* Second byte */

    /* Populate the tx buffer */
    for (uint8_t count = 2; count < transfer_size; count++)
    {
        i2c_tx_buffer[count] = count - 2u;
    }

    MIV_I2C_init(&miv_i2c, MIV_I2C_BASE_ADDR);

    MIV_I2C_config(&miv_i2c, 0x63);

    MIV_I2C_write (&miv_i2c,
            DUALEE_SLAVEADDRESS_2,
                   i2c_tx_buffer,
                   transfer_size,
                   MIV_I2C_RELEASE_BUS,
                   MIV_I2C_ACK_POLLING_ENABLE
                  );

    do {
        miv_i2c_status = miv_i2c.master_status;
    }while (MIV_I2C_IN_PROGRESS == miv_i2c_status);

    miv_i2c_status = 0;

    uint8_t addr_offset[2] = {0x00, 0x00};
    MIV_I2C_write_read(&miv_i2c,
            DUALEE_SLAVEADDRESS_2,
                        addr_offset,
                        2,
                        i2c_rx_buffer,
                        transfer_size,
                        MIV_I2C_RELEASE_BUS,
                        MIV_I2C_ACK_POLLING_ENABLE
    );

    do {
        miv_i2c_status = miv_i2c.master_status;
    }while (MIV_I2C_IN_PROGRESS == miv_i2c_status);

    for (;;)
    {
    }
}
