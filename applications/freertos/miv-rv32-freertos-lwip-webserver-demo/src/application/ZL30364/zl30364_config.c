
/*******************************************************************************
 * Copyright 2025 Microchip FPGA Embedded Systems Solutions.
 *
 * SPDX-License-Identifier: MIT
 *
 * @file zl30364_config.c
 * @author Microchip FPGA Embedded Systems Solutions
 * @brief Configure ZL30364 device.
 *
 */

#include "drivers/fpga_ip/CoreSPI/core_spi.h"
#include "zl_reg_config.h"
#include "fpga_design_config/fpga_design_config.h"

#define DELAY_COUNT_1    64UL
#define DELAY_COUNT_2    4096UL
#define DELAY_COUNT_BASE 40000000
void
delay(uint32_t div)
{
    volatile uint32_t delay_count = DELAY_COUNT_BASE / div; /// 128u;

    while (delay_count > 0u)
    {
        --delay_count;
    }
}

spi_instance_t g_flash_core_spi;

#define SPI_SLAVE             0
#define SPI_INSTANCE          &g_flash_core_spi
/*==============================================================================
 * ZL30362 registers are 8-bit registers.
 */
#define ZL_ADDRESS_BITS_MASK  0x0000007Fu
#define ZL_REG_BITS_MASK      0x000000FFu
#define ZL_READY_CODE         0x0000009Fu

#define READ_ZL_READY_REG_CMD 0x00008000u
#define SELECT_ZL_PAGE0_CMD   0x00007F00u
extern void delay(uint32_t div);
/*==============================================================================
 * Configure the ZL30364 device through its SPI interface.
 */
void
configure_zl30364(void)
{
    uint32_t inc, size = 0;
    uint32_t zl_register = 0;
    uint32_t zl_ready;
    uint32_t tx_bits;
    /*--------------------------------------------------------------------------
     * Configure SPI interface.
     */
    SPI_init(SPI_INSTANCE, CORESPI_BASE_ADDR, 32);
    SPI_configure_master_mode(SPI_INSTANCE);

    SPI_set_slave_select(SPI_INSTANCE, SPI_SLAVE);

    /*--------------------------------------------------------------------------
     * Wait for ZL30362 to become ready.
     * Compare against full ready register value instead of simply polling the
     * ready bit. This avoids false ready bit detection when the ZL device has
     * not fully come out of reset and does not respond to SPI transactions yet.
     * The SPI transfer would typically return all 1's when there is no SPI
     * device responding to the transaction.
     */

    SPI_transfer_frame(SPI_INSTANCE, SELECT_ZL_PAGE0_CMD);
    do
    {
        zl_register =
            SPI_transfer_frame(SPI_INSTANCE, READ_ZL_READY_REG_CMD); // READ_ZL_READY_REG_CMD);
        zl_ready = zl_register & ZL_REG_BITS_MASK;
    } while (zl_ready != ZL_READY_CODE);

    /*--------------------------------------------------------------------------
     * Transfer ZL30362 configuration via SPI.
     *
     */
    size = sizeof(g_zl30362_config2) - 1;

    tx_bits = ((609 & ZL_ADDRESS_BITS_MASK) << 8u) | 0xbb;

    SPI_transfer_frame(SPI_INSTANCE, tx_bits);

    for (inc = 0; inc < size; ++inc)
    {
        tx_bits = ((inc & ZL_ADDRESS_BITS_MASK) << 8u) | g_zl30362_config2[inc];

        SPI_transfer_frame(SPI_INSTANCE, tx_bits);
        if (inc == 0x10)
        {
            delay(DELAY_COUNT_1);
        }
        else
        {
            delay(DELAY_COUNT_2);
        }
    }
    tx_bits = ((609 & ZL_ADDRESS_BITS_MASK) << 8u) | 0xbb;

    SPI_transfer_frame(SPI_INSTANCE, tx_bits);

    SPI_clear_slave_select(SPI_INSTANCE, SPI_SLAVE);
}
