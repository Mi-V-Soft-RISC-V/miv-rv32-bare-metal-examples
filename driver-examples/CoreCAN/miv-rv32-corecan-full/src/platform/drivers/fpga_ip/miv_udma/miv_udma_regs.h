 /*******************************************************************************
 * Copyright 2022 Microchip FPGA Embedded Systems Solutions.
 *
 * SPDX-License-Identifier: MIT
 *
 * This file contains Register bit offsets and masks definitions for MI-V Soft
 * IP uDMA module driver. This module is delivered as a part of Mi-V
 * extended Sub-System(ESS) MIV_ESS.
 */

#ifndef MIV_UDMA_APB_REGISTERS
#define MIV_UDMA_APB_REGISTERS  1

#ifdef __cplusplus
extern "C" {
#endif

/**************************************************************************//**
 * Control start/Reset register details
 */

#define CONTROL_SR_REG_OFFSET               0x0u

/* Control start/Reset register bits */
#define CONTROL_SR_OFFSET                   0x00u
#define CONTROL_SR_MASK                     0x03u
#define CONTROL_SR_SHIFT                    0u

/* uDMA Control Start Transfer */

#define CTRL_START_TX_OFFSET                0x00u
#define CTRL_START_TX_MASK                  0x01u
#define CTRL_START_TX_SHIFT                 0u

/* uDMA Control Reset Transfer */
#define CTRL_RESET_TX_OFFSET                0x00u
#define CTRL_RESET_TX_MASK                  0x02u
#define CTRL_RESET_TX_SHIFT                 1u

/**************************************************************************//**
 * IRQ Configuration register details
 */
#define IRQ_CFG_REG_OFFSET                  0x4u

/* Control start/Reset register bits */
#define IRQ_CFG_OFFSET                      0x04u
#define IRQ_CFG_MASK                        0x01u
#define IRQ_CFG_SHIFT                       0u

/***************************************************************************//**
 * Transfer Status register details
 */
#define TX_STATUS_REG_OFFSET                0x08u

/* Transfer status register bits */
#define TX_STATUS_OFFSET                    0x08u
#define TX_STATUS_MASK                      0x03u
#define TX_STATUS_SHIFT                     0u

/***************************************************************************//**
 * Source Memory Start Address Register
 */
#define SRC_START_ADDR_REG_OFFSET            0x0cu

/* Source Memory Start Address Register bits */
#define SRC_START_ADDR_OFFSET               0x0cu
#define SRC_START_ADDR_MASK                 0xFFFFFFFFu
#define SRC_START_ADDR_SHIFT                0u

/***************************************************************************//**
 * Destination Memory Start Address register details
 */
#define DEST_START_ADDR_REG_OFFSET          0x10u

/* Destination Memory Start Address register bits */
#define DEST_START_ADDR_OFFSET              0x10u
#define DEST_START_ADDR_MASK                0xFFFFFFFFu
#define DEST_START_ADDR_SHIFT               0x0u

/***************************************************************************//**
 * Block Size register details
 */
#define BLK_SIZE_REG_OFFSET                 0x14u

/* Destination Memory Start Address register bits */
#define BLK_SIZE_OFFSET                     0x14u
#define BLK_SIZE_MASK                       0xFFFFFFFFu
#define BLK_SIZE_SHIFT                      0x0u

#ifdef __cplusplus
}
#endif

#endif  /* MIV_UDMA_APB_REGISTERS */ 
