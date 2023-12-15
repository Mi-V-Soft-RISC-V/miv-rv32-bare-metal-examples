/*******************************************************************************
 * (c) Copyright 2022-2023 Microchip FPGA Embedded Systems Solutions.
 *
 * SPDX-License-Identifier: MIT
 *
 * Mi-V uDMA Soft IP bare-metal driver. This module is delivered as part of
 * Mi-V Extended Sub System(MIV_ESS) 
 */

#include "miv_udma_regs.h"
#include "miv_udma.h"

/***************************************************************************//**
 * MIV_uDMA_init()
 * See "miv_udma.h" for details of how to use this function.
 */
void
MIV_uDMA_init
(
    miv_udma_instance_t* this_udma,
    addr_t base_addr
)
{
    /* Assign the Mi-V uDMA base address to the uDMA instance structure */
    this_udma->base_address = base_addr;
}

/***************************************************************************//**
 * MIV_uDMA_config()
 * See "miv_udma.h" for details of how to use this function.
 */
void
MIV_uDMA_config
(
    miv_udma_instance_t* this_udma,
    addr_t src_addr,
    addr_t dest_addr,
    uint32_t transfer_size,
    uint32_t irq_config
)
{
    /* Source memory start address */
    HAL_set_32bit_reg(this_udma->base_address, SRC_START_ADDR, src_addr);

    /* Destination memory start address */
    HAL_set_32bit_reg(this_udma->base_address, DEST_START_ADDR, dest_addr);

    /* Data transfer size */
    HAL_set_32bit_reg(this_udma->base_address, BLK_SIZE, transfer_size);

    /* Configure the uDMA IRQ */
    HAL_set_32bit_reg(this_udma->base_address, IRQ_CFG, irq_config);
}

/***************************************************************************//**
 * MIV_uDMA_start()
 * See "miv_udma.h" for details of how to use this function.
 */
void
MIV_uDMA_start
(
    miv_udma_instance_t* this_udma
)
{
    /* Start the uDMA transfer */
    HAL_set_32bit_reg(this_udma->base_address, CONTROL_SR, CTRL_START_TX_MASK);
}

/***************************************************************************//**
 * MIV_uDMA_reset()
 * See "miv_udma.h" for details of how to use this function.
 */
void
MIV_uDMA_reset
(
    miv_udma_instance_t* this_udma
)
{
    /* Toggle the uDMA_reset bit to reset the uDMA.
     * Resetting the uDMA will clear all the configuration made by
     * MIV_uDMA_config().
     *
     * This function should be called from the interrupt handler to clear the
     * IRQ.
     */
    HAL_set_32bit_reg_field(this_udma->base_address, CTRL_RESET_TX, 0x1u);
    HAL_set_32bit_reg_field(this_udma->base_address, CTRL_RESET_TX, 0x0u);
}

/***************************************************************************//**
 * MIV_uDMA_read_status()
 * See "miv_udma.h" for details of how to use this function.
 */
uint32_t
MIV_uDMA_read_status
(
    miv_udma_instance_t* this_udma
)
{
    uint32_t status = 0u;

    /* Read the status of the uDMA transfer.
     * The transfer status register can be Error or Busy depending on the
     * current uDMA transfer.
     */
    status = HAL_get_32bit_reg(this_udma->base_address, TX_STATUS);

    return status;
}
