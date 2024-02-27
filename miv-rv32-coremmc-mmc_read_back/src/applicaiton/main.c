/*******************************************************************************
 * Copyright 2019 Microchip FPGA Embedded Systems Solutions.
 *
 * SPDX-License-Identifier: MIT
 *
 * This sample program demonstrates the use of CoreMMC driver to interface
 * with the eMMC Card on-board the SmartFusion 2 Development Board.
 *
 * Please refer to the file README.md for further details about this example.
 *
 */

/*-------------------------------------------------------------------------*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "hal/hal.h"
#include "miv_rv32_hal/miv_rv32_hal.h"
#include "drivers/fpga_ip/CoreUARTapb/core_uart_apb.h"
#include "drivers/fpga_ip/CoreMMC/core_mmc.h"

#define INTERRUPT_EVERY_10MS (100u)
#define SECTOR_SIZE_BYTES    (512u)
#define SECTOR_SIZE_WORDS    (SECTOR_SIZE_BYTES / sizeof(uint32_t))
#define NUMBER_OF_SECTORS    (1u)
#define SECTOR_1             (0x1u)
#define TIMEOUT_10_SECS      (1000u)
#define INCREASE_BY_2        (8u)
#define SUCCESS              (0u)
#define FAILURE              (1u)

/* Sector address within eMMC device */
#define BLOCK_1              0x00000001u
#define NO_MULTI_BLKS        64
#define MAX_NO_BLOCKS        15515584 /* 8meg device- can be read from the extended CSR register */

static uint32_t g_write_data_buffer[SECTOR_SIZE_WORDS] = {0u};
static uint32_t g_read_data_buffer[SECTOR_SIZE_WORDS] = {0u};
static uint32_t ten_ms_sys_tick = 0;

static const uint8_t init_fail[] = "\r\neMMC device initialisation failure\r\n";
static const uint8_t read_back_success[] = "\r\nSUCCESS: Write and read data match\r\n";
static const uint8_t data_mismatch_fail[] = "\r\nFAILURE: Write and read data mismatch\r\n";
static const uint8_t read_fail[] = "\r\nFAILURE: Read operation failed!\r\n";
static const uint8_t write_fail[] = "\r\nFAILURE: Write operation failed!\r\n";
static const uint8_t greeting_message[] =
    "\033[2J******************************************************************************\r\n\n"
    "*****************************    Mi-V CoreMMC    *****************************\r\n\n"
    "*************    Single and Multi-block Read and Write Example    ************\r\n\n"
    "******************************************************************************\r\n\r\n"
    "This example project demonstrates writing and reading blocks of data to the MMC device.\r\n";

static mmc_instance_t g_emmc;
static mmc_params_t g_emmc_params;

static UART_instance_t g_uart;

void
MSYS_EI0_IRQHandler(void)
{
    MMC_isr(&g_emmc);
}

void
SysTick_Handler(void)
{
    ten_ms_sys_tick++;
}

static uint32_t
get_clock_ticks(void)
{
    return (ten_ms_sys_tick);
}

static void
print(const uint8_t *const message)
{
    UART_polled_tx_string(&g_uart, message);
}

static void
fill_buffer(uint32_t *const buffer, const uint32_t increment)
{
    uint32_t value = 0u;

    for (uint32_t index = 0u; index < SECTOR_SIZE_WORDS; index++)
    {
        *(buffer + index) = value;
        value += increment;
    }
}

static uint8_t
verify_data(const uint32_t *const read_buffer,
            const uint32_t *const write_buffer,
            const uint32_t number_of_sectors)
{
    for (uint32_t index = 0; index < (SECTOR_SIZE_WORDS * number_of_sectors); index++)
    {
        if (*(read_buffer + index) != *(write_buffer + index))
        {
            return FAILURE;
        }
    }
    return SUCCESS;
}

static mmc_transfer_status_t
single_block_write_readback(mmc_instance_t *const this_mmc,
                            const uint32_t *const write_buffer,
                            uint32_t *const read_buffer,
                            const uint32_t sector_address)
{
    volatile uint32_t *write_fifo =
        (uint32_t *)MMC_get_fifo_write_address((mmc_instance_t *)this_mmc);
    volatile uint32_t *read_fifo =
        (uint32_t *)MMC_get_fifo_read_address((mmc_instance_t *)this_mmc);
    volatile mmc_transfer_status_t mmc_command_status = MMC_TRANSFER_FAIL;

    print("\r\n\nCoreMMC single block write and read-back test:");

    MMC_init_fifo(this_mmc);

    mmc_command_status = MMC_single_block_write_nb((mmc_instance_t *)this_mmc,
                                                   sector_address,
                                                   get_clock_ticks(),
                                                   TIMEOUT_10_SECS);

    for (uint32_t index = 0; index < SECTOR_SIZE_WORDS; index++)
    {
        *write_fifo = write_buffer[index];
    }

    while (MMC_CMD_PROCESSING == mmc_command_status)
    {
        mmc_command_status = MMC_status((mmc_instance_t *)this_mmc, get_clock_ticks());
    }
    if (MMC_TRANSFER_SUCCESS == mmc_command_status)
    {
        MMC_init_fifo(this_mmc);
        mmc_command_status = MMC_single_block_read_nb((mmc_instance_t *)this_mmc,
                                                      sector_address,
                                                      get_clock_ticks(),
                                                      TIMEOUT_10_SECS);
        while (MMC_CMD_PROCESSING == mmc_command_status)
        {
            mmc_command_status = MMC_status((mmc_instance_t *)this_mmc, get_clock_ticks());
        }
        if (MMC_TRANSFER_SUCCESS == mmc_command_status)
        {
            for (uint32_t index = 0; index < SECTOR_SIZE_WORDS; index++)
            {
                *(read_buffer + index) = *read_fifo;
            }
            if (verify_data(read_buffer, write_buffer, SECTOR_1) == SUCCESS)
            {
                print(read_back_success);
            }
            else
            {
                print(data_mismatch_fail);
            }
        }
        else
        {
            print(read_fail);
        }
    }
    else
    {
        print(write_fail);
    }
    return mmc_command_status;
}

static mmc_transfer_status_t
multiblock_write_readback(mmc_instance_t *const this_mmc,
                          const uint32_t *const write_buffer,
                          uint32_t *const read_buffer,
                          const uint32_t sector_address,
                          const uint32_t number_of_sectors)
{
    volatile uint32_t *write_fifo =
        (uint32_t *)MMC_get_fifo_write_address((mmc_instance_t *)this_mmc);
    volatile uint32_t *read_fifo =
        (uint32_t *)MMC_get_fifo_read_address((mmc_instance_t *)this_mmc);
    mmc_transfer_status_t mmc_command_status = MMC_TRANSFER_FAIL;

    print("\r\n\nCoreMMC multi-block write and read-back test:");

    MMC_init_fifo(this_mmc);

    mmc_command_status = MMC_multi_block_write((mmc_instance_t *)this_mmc,
                                               number_of_sectors,
                                               sector_address,
                                               get_clock_ticks(),
                                               TIMEOUT_10_SECS);

    for (uint32_t index = 0; index < (number_of_sectors * SECTOR_SIZE_WORDS); index++)
    {
        *write_fifo = *(write_buffer + index);
    }

    while (MMC_CMD_PROCESSING == mmc_command_status)
    {
        mmc_command_status = MMC_status((mmc_instance_t *)this_mmc, get_clock_ticks());
    }
    if (MMC_TRANSFER_SUCCESS == mmc_command_status)
    {
        MMC_init_fifo(this_mmc);
        mmc_command_status = MMC_multi_block_read((mmc_instance_t *)this_mmc,
                                                  number_of_sectors,
                                                  sector_address,
                                                  get_clock_ticks(),
                                                  TIMEOUT_10_SECS);

        while (MMC_CMD_PROCESSING == mmc_command_status)
        {
            mmc_command_status = MMC_status((mmc_instance_t *)this_mmc, get_clock_ticks());
        }

        if (MMC_TRANSFER_SUCCESS == mmc_command_status)
        {
            for (uint32_t index = 0; index < (number_of_sectors * SECTOR_SIZE_WORDS); index++)
            {
                *(read_buffer + index) = *read_fifo;
            }
            if (verify_data(read_buffer, write_buffer, number_of_sectors) == SUCCESS)
            {
                print(read_back_success);
            }
            else
            {
                print(data_mismatch_fail);
                return MMC_TRANSFER_FAIL;
            }
        }
        else
        {
            print(read_fail);
        }
    }
    else
    {
        print(write_fail);
    }
    return mmc_command_status;
}

int
main(void)
{
    mmc_transfer_status_t mmc_state = MMC_TRANSFER_FAIL;
    uint32_t sector = 1u;

    MRV_enable_interrupts();
    MRV_systick_config(SYS_CLK_FREQ / INTERRUPT_EVERY_10MS);
    MRV_enable_local_irq(MRV32_TIMER_IRQn);
    MRV_enable_local_irq(MRV32_MSYS_EIE0_IRQn);

    UART_init(&g_uart, COREUARTAPB0_BASE_ADDR, BAUD_VALUE_115200, (DATA_8_BITS | NO_PARITY));
    print(greeting_message);

    MMC_param_config(&g_emmc_params);
    g_emmc_params.data_timeout = SYS_CLK_FREQ; /* Timeout set to 1 second (50000000 cycles) */
    mmc_state = MMC_init(&g_emmc, COREMMC_BASE_ADDR, &g_emmc_params);

    if (MMC_INIT_SUCCESS == mmc_state)
    {
        memset(g_read_data_buffer, 0x0u, sizeof(g_read_data_buffer));
        fill_buffer(g_write_data_buffer, INCREASE_BY_2);

        mmc_state =
            single_block_write_readback(&g_emmc, g_write_data_buffer, g_read_data_buffer, SECTOR_1);

        memset(g_read_data_buffer, 0x0u, sizeof(g_read_data_buffer));
        fill_buffer(g_write_data_buffer, INCREASE_BY_2);

        mmc_state = multiblock_write_readback(&g_emmc,
                                              g_write_data_buffer,
                                              g_read_data_buffer,
                                              sector * NO_MULTI_BLKS,
                                              NUMBER_OF_SECTORS);
    }
    else
    {
        print(init_fail);
    }

    while (1)
    {
        ;
    }
    return 0;
}
