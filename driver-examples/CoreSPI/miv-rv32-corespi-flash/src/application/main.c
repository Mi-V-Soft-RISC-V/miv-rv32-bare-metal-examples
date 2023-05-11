/*******************************************************************************
 * Copyright 2017-2023 Microchip FPGA Embedded Systems Solutions.
 *
 * SPDX-License-Identifier: MIT 
 *
 * This example project demonstrates communications with an external flash
 * device.
 *
 * Please refer to the file README.md for further details about this example.
 *
 */
#include "miv_rv32_hal/miv_rv32_hal.h"
#include "fpga_design_config/fpga_design_config.h"
#include "drivers/fpga_ip/CoreSPI/core_spi.h"
#include "drivers/fpga_ip/mt25ql01gbbb/mt25ql01gbbb.h"
#include "drivers/fpga_ip/CoreUARTapb/core_uart_apb.h"
  
#define BUFFER_A_SIZE   3000

/* Manufacture and device IDs for Micron MT25QL01GBBB SPI Flash. */
#define FLASH_MANUFACTURER_ID   (uint8_t)0x20
#define FLASH_DEVICE_ID         (uint8_t)0xBB

UART_instance_t g_uart;

uint8_t g_message[] =
"\r\n\r\n\
*******************  CoreSPI Example Project ********************************\r\n\
\r\n\r\n\
This example project demonstrates the use of the CoreSPI driver\r\n\
with Mi-V soft processor.\r\n";

uint8_t g_message1[] =
"\r\n\r\n\
Write-Read test on the SPI flash successful\r\n\
\r\n\r\n\
Data stored in g_flash_rd_buf is identical from the data stored in g_flash_wr_buf";
uint8_t g_message2[] =
"\r\n\r\n\
Write-Read test on the SPI flash failed\r\n\
\r\n\r\n\
Data stored in g_flash_rd_buf is different from the data stored in g_flash_wr_buf";


/*
 * Static global variables
 */
static uint8_t g_flash_wr_buf[BUFFER_A_SIZE];
static uint8_t g_flash_rd_buf[BUFFER_A_SIZE];

/* Local Function. */
static uint8_t verify_write(uint8_t* write_buff, uint8_t* read_buff, uint16_t size);

/***************************************************************************//**
 * main function.
 */
int main(void)
{
    volatile uint32_t errors = 0;
    uint32_t address = 0;
    uint16_t loop_count;
    uint8_t manufacturer_id = 0;
    uint8_t device_id = 0;

    /* Initialize UART */
    UART_init( &g_uart, COREUARTAPB0_BASE_ADDR,\
               BAUD_VALUE_115200, (DATA_8_BITS | NO_PARITY) );
			   
    /* Greeting message */
    UART_polled_tx_string(&g_uart,g_message);


    /*--------------------------------------------------------------------------
     * Initialize the write and read Buffers
    */
    for(loop_count = 0; loop_count < (BUFFER_A_SIZE); loop_count++)
    {
       if (loop_count < (BUFFER_A_SIZE/2))
       {
           g_flash_wr_buf[loop_count] = 0x45 + loop_count;
       }
       else
       {
           g_flash_wr_buf[loop_count] = 0x34;
       }
	   g_flash_rd_buf[loop_count] = 0x00;
    }

    /*--------------------------------------------------------------------------
     * Flash Driver Initialization
    */
    FLASH_init();

    FLASH_global_unprotect();

    FLASH_erase_4k_block(0);

    /*--------------------------------------------------------------------------
     * Check SPI Flash part manufacturer and device ID.
    */
    FLASH_read_device_id(&manufacturer_id, &device_id);
    if ((manufacturer_id != FLASH_MANUFACTURER_ID) || (device_id != FLASH_DEVICE_ID))
    {
        ++errors;
    }
    /*--------------------------------------------------------------------------
     * Write Data to Flash.
    */
    address = 200;
    FLASH_program(address, g_flash_wr_buf, sizeof(g_flash_wr_buf));

    /*--------------------------------------------------------------------------
     * Read Data From Flash.
    */
    address = 200;
    FLASH_read(address, g_flash_rd_buf, sizeof(g_flash_wr_buf));

    errors = verify_write(g_flash_wr_buf, g_flash_rd_buf, sizeof(g_flash_wr_buf));

    if(0 == errors)
    {
        UART_polled_tx_string(&g_uart,g_message1);
    }

    else
    {
        UART_polled_tx_string(&g_uart,g_message2);
    }

    while(1)
    {
        ;
    }
}

/***************************************************************************//**
 * Compare the data of write buffer and read buffer
 */
static uint8_t verify_write(uint8_t* write_buff, uint8_t* read_buff, uint16_t size)
{
    uint8_t error = 0;
    uint16_t index = 0;

    while(size != 0)
    {
        if(write_buff[index] != read_buff[index])
        {
            error = 1;
            break;
        }
        index++;
        size--;
    }

    return error;
}

