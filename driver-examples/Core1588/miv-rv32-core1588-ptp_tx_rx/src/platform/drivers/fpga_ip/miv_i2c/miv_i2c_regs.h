 /*******************************************************************************
 * Copyright 2022 Microchip FPGA Embedded Systems Solutions.
 *
 * SPDX-License-Identifier: MIT
 *
 * This file contains Register bit offsets and masks definitions for MI-V Soft
 * IP I2C module driver. This module is delivered as a part of Mi-V extended
 * Sub-System(MIV_ESS).
 */

#ifndef MIV_I2C_APB_REGISTERS
#define MIV_I2C_APB_REGISTERS   1

#ifdef __cplusplus
extern "C" {
#endif

/*------------------------------------------------------------------------------
 * Prescale register details
 */
#define PRESCALE_REG_OFFSET                         0x00u

/* Prescale register bits */
#define PRESCALE_OFFSET                             0x00u
#define PRESCALE_MASK                               0xFFFFu
#define PRESCALE_SHIFT                              0u

/*------------------------------------------------------------------------------
 * Control register details
 */
#define CONTROL_REG_OFFSET                          0x04u

/* Control register bits */
#define CONTROL_OFFSET                              0x04u
#define CONTROL_MASK                                0xC0u
#define CONTROL_SHIFT                               0u

/* Control register Core Enable Bit */
#define CTRL_CORE_EN_OFFSET                         0x04u
#define CTRL_CORE_EN_MASK                           0x80u
#define CTRL_CORE_EN_SHIFT                          7u

/* Control register IRQ Enable bit */
#define CTRL_IRQ_EN_OFFSET                          0x04u
#define CTRL_IRQ_EN_MASK                            0x40u
#define CTRL_IRQ_EN_SHIFT                           6u

/*------------------------------------------------------------------------------
 * Transmit register details
 */
#define TRANSMIT_REG_OFFSET                         0x08u

/* Transmit register bits */
#define TRANSMIT_OFFSET                             0x08u
#define TRANSMIT_MASK                               0xFFu
#define TRANSMIT_SHIFT                              0u

/* Transmit register DIR bit */
#define TX_DIR_OFFSET                               0x08u
#define TX_DIR_MASK                                 0x01u
#define TX_DIR_SHIFT                                0u

/* Transmit register TARGET_ADDR bit */
#define TX_TARGET_ADDR_OFFSET                       0x08u
#define TX_TARGET_ADDR_MASK                         0xFEu
#define TX_TARGET_ADDR_SHIFT                        1u

/*------------------------------------------------------------------------------
 * Receive register details
 */
#define RECEIVE_REG_OFFSET                          0x0Cu

/* Receive register bits */
#define RECEIVE_OFFSET                              0x0Cu
#define RECEIVE_MASK                                0xFFu
#define RECEIVE_SHIFT                               0u

/*------------------------------------------------------------------------------
 * Command register details
 */
#define COMMAND_REG_OFFSET                          0x10u

/* Command register bits */
#define COMMAND_OFFSET                              0x10u
#define COMMAND_MASK                                0xF9u
#define COMMAND_SHIFT                               0u

/* Command register IACK bit */
#define CMD_IACK_OFFSET                             0x10u
#define CMD_IACK_MASK                               0x01u
#define CMD_IACK_SHIFT                              0u

/* Command register ACK bit */
#define CMD_ACK_OFFSET                              0x10u
#define CMD_ACK_MASK                                0x08u
#define CMD_ACK_SHIFT                               3u

/* Command register WR bit */
#define CMD_WR_OFFSET                               0x10u
#define CMD_WR_MASK                                 0x10u
#define CMD_WR_SHIFT                                4u

/* Command register RD bit */
#define CMD_RD_OFFSET                               0x10u
#define CMD_RD_MASK                                 0x20u
#define CMD_RD_SHIFT                                5u

/* Command register STO bit */
#define CMD_STO_OFFSET                              0x10u
#define CMD_STO_MASK                                0x40u
#define CMD_STO_SHIFT                               6u

/* Command register STA bit */
#define CMD_STA_OFFSET                              0x10u
#define CMD_STA_MASK                                0x80u
#define CMD_STA_SHIFT                               7u

/*------------------------------------------------------------------------------
 * Status register details
 */
#define STATUS_REG_OFFSET                           0x14u

/* Command register bits */
#define STATUS_OFFSET                               0x14u
#define STATUS_MASK                                 0xFFu
#define STATUS_SHIFT                                0u

/* Status register Interrupt Flag(IF) bit */
#define STAT_IF_OFFSET                              0x14u
#define STAT_IF_MASK                                0x01u
#define STAT_IF_SHIFT                               0u

/* Status register Transfer in Progress(TIP) bit */
#define STAT_TIP_OFFSET                             0x14u
#define STAT_TIP_MASK                               0x02u
#define STAT_TIP_SHIFT                              1u

/* Status register Arbitration Lost(AL) bit */
#define STAT_AL_OFFSET                              0x14u
#define STAT_AL_MASK                                0x20u
#define STAT_AL_SHIFT                               5u

/* Status register Busy(BUSY) bit */
#define STAT_BUSY_OFFSET                            0x14u
#define STAT_BUSY_MASK                              0x40u
#define STAT_BUSY_SHIFT                             6u

/* Status register Ack received(RXACK) bit */
#define STAT_RXACK_OFFSET                           0x14u
#define STAT_RXACK_MASK                             0x80u
#define STAT_RXACK_SHIFT                            7u

  
#ifdef __cplusplus
}
#endif

#endif   /* MIV_I2C_APB_REGISTERS */
