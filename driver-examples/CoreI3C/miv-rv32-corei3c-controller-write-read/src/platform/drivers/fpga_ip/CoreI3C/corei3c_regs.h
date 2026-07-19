/*******************************************************************************
 * Copyright 2026 Microchip FPGA Embedded Systems Solutions.
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
 * @file corei3c_regs.h
 * @author Microchip FPGA Embedded Systems Solutions
 * @brief CoreI3C IP bare metal driver register map.
 *        See core_i3c.h for API description.
 *
 * Naming convention (matches Microchip HAL macro requirements):
 *
 *   COREI3C_<REG>_REG_OFFSET        - byte offset; used as 2nd arg to
 *                                      HAL_set_32bit_reg / HAL_get_32bit_reg
 *                                      via the short token COREI3C_<REG>
 *   COREI3C_<REG>_REG_RESET_VALUE   - hardware reset value
 *   COREI3C_<REG>_<FIELD>_SHIFT     - LSB position of field in register
 *   COREI3C_<REG>_<FIELD>_MASK      - field mask (shifted into position)
 *   COREI3C_<REG>_<FIELD>_NS_MASK   - field mask (not shifted, width only)
 */

#ifndef COREI3C_REGS_H_
#define COREI3C_REGS_H_

#ifdef __cplusplus
extern "C" {
#endif

/*=============================================================================
 * Register: I3C_VERSION  (0x000)   RO
 * Short token: COREI3C_I3C_VERSION
 *===========================================================================*/
#define COREI3C_I3C_VERSION_REG_OFFSET              (0x000U)
#define COREI3C_I3C_VERSION_REG_RESET_VALUE         (0x02000000UL)
#define COREI3C_I3C_VERSION_REG_LENGTH              (0x4U)
#define COREI3C_I3C_VERSION_REG_RO_MASK             (0xFFFF0000U)

/* MAJOR [31:24] */
#define COREI3C_I3C_VERSION_MAJOR_SHIFT             (24U)
#define COREI3C_I3C_VERSION_MAJOR_NS_MASK           (0xFFU)
#define COREI3C_I3C_VERSION_MAJOR_MASK              ((uint32_t)0xFFU << 24U)

/* MINOR [23:16] */
#define COREI3C_I3C_VERSION_MINOR_SHIFT             (16U)
#define COREI3C_I3C_VERSION_MINOR_NS_MASK           (0xFFU)
#define COREI3C_I3C_VERSION_MINOR_MASK              ((uint32_t)0xFFU << 16U)

/*=============================================================================
 * Register: I3C_CNTRL  (0x020)   RO/RW/RW1C
 * Short token: COREI3C_CNTRL  (alias: COREI3C_I3C_CNTRL)
 *===========================================================================*/
#define COREI3C_I3C_CNTRL_REG_OFFSET               (0x020U)
/* Short-token alias used with HAL_set/get_32bit_reg */
#define COREI3C_CNTRL_REG_OFFSET                   (0x020U)
#define COREI3C_I3C_CNTRL_REG_RESET_VALUE          (0x00000000UL)
#define COREI3C_I3C_CNTRL_REG_LENGTH               (0x4U)
#define COREI3C_I3C_CNTRL_REG_RW_MASK              (0xA0001981U)

/* Short-token field aliases (COREI3C_CNTRL_*) matching the short token above */
#define COREI3C_CNTRL_BUS_ENABLE_MASK              COREI3C_I3C_CNTRL_BUS_ENABLE_MASK
#define COREI3C_CNTRL_RESUME_MASK                  COREI3C_I3C_CNTRL_RESUME_MASK
#define COREI3C_CNTRL_ABORT_MASK                   COREI3C_I3C_CNTRL_ABORT_MASK
#define COREI3C_CNTRL_HALT_ON_TIMEOUT_MASK         COREI3C_I3C_CNTRL_HALT_ON_TIMEOUT_MASK
#define COREI3C_CNTRL_HOT_JOIN_CTRL_MASK           COREI3C_I3C_CNTRL_HOT_JOIN_CTRL_MASK
#define COREI3C_CNTRL_I2C_DEV_PRESENT_MASK         COREI3C_I3C_CNTRL_I2C_DEV_PRESENT_MASK
#define COREI3C_CNTRL_IBA_INCLUDE_MASK             COREI3C_I3C_CNTRL_IBA_INCLUDE_MASK

/* BUS_ENABLE [31]  RW */
#define COREI3C_I3C_CNTRL_BUS_ENABLE_SHIFT         (31U)
#define COREI3C_I3C_CNTRL_BUS_ENABLE_NS_MASK       (0x1U)
#define COREI3C_I3C_CNTRL_BUS_ENABLE_MASK          ((uint32_t)0x1U << 31U)

/* RESUME [30]  RW1C */
#define COREI3C_I3C_CNTRL_RESUME_SHIFT             (30U)
#define COREI3C_I3C_CNTRL_RESUME_NS_MASK           (0x1U)
#define COREI3C_I3C_CNTRL_RESUME_MASK              ((uint32_t)0x1U << 30U)

/* ABORT [29]  RW */
#define COREI3C_I3C_CNTRL_ABORT_SHIFT              (29U)
#define COREI3C_I3C_CNTRL_ABORT_NS_MASK            (0x1U)
#define COREI3C_I3C_CNTRL_ABORT_MASK               ((uint32_t)0x1U << 29U)

/* HALT_ON_CMD_SEQ_TIMEOUT [12]  RW */
#define COREI3C_I3C_CNTRL_HALT_ON_TIMEOUT_SHIFT    (12U)
#define COREI3C_I3C_CNTRL_HALT_ON_TIMEOUT_NS_MASK  (0x1U)
#define COREI3C_I3C_CNTRL_HALT_ON_TIMEOUT_MASK     ((uint32_t)0x1U << 12U)

/* HOT_JOIN_CTRL [8]  RW  (1=NACK, 0=ACK) */
#define COREI3C_I3C_CNTRL_HOT_JOIN_CTRL_SHIFT      (8U)
#define COREI3C_I3C_CNTRL_HOT_JOIN_CTRL_NS_MASK    (0x1U)
#define COREI3C_I3C_CNTRL_HOT_JOIN_CTRL_MASK       ((uint32_t)0x1U << 8U)

/* I2C_DEV_PRESENT [7]  RW */
#define COREI3C_I3C_CNTRL_I2C_DEV_PRESENT_SHIFT    (7U)
#define COREI3C_I3C_CNTRL_I2C_DEV_PRESENT_NS_MASK  (0x1U)
#define COREI3C_I3C_CNTRL_I2C_DEV_PRESENT_MASK     ((uint32_t)0x1U << 7U)

/* IBA_INCLUDE [0]  RW */
#define COREI3C_I3C_CNTRL_IBA_INCLUDE_SHIFT        (0U)
#define COREI3C_I3C_CNTRL_IBA_INCLUDE_NS_MASK      (0x1U)
#define COREI3C_I3C_CNTRL_IBA_INCLUDE_MASK         ((uint32_t)0x1U << 0U)

/*=============================================================================
 * Register: CNTRL_DEVICE_ADDR  (0x024)   RW
 * Short token: COREI3C_CNTRL_DEV_ADDR  (alias: COREI3C_CNTRL_DEVICE_ADDR)
 *===========================================================================*/
#define COREI3C_CNTRL_DEVICE_ADDR_REG_OFFSET       (0x024U)
/* Short-token alias */
#define COREI3C_CNTRL_DEV_ADDR_REG_OFFSET          (0x024U)
#define COREI3C_CNTRL_DEVICE_ADDR_REG_RESET_VALUE  (0x00000000UL)
#define COREI3C_CNTRL_DEVICE_ADDR_REG_LENGTH       (0x4U)
#define COREI3C_CNTRL_DEVICE_ADDR_REG_RW_MASK      (0x807F0000U)

/* Short-token field aliases (COREI3C_DEV_ADDR_*) */
#define COREI3C_DEV_ADDR_VALID_MASK                COREI3C_CNTRL_DEVICE_ADDR_VALID_MASK
#define COREI3C_DEV_ADDR_DYNAMIC_SHIFT             COREI3C_CNTRL_DEVICE_ADDR_DYNADDR_SHIFT
#define COREI3C_DEV_ADDR_DYNAMIC_NS_MASK           COREI3C_CNTRL_DEVICE_ADDR_DYNADDR_NS_MASK

/* DYNAMIC_ADDR_VALID [31]  RW */
#define COREI3C_CNTRL_DEVICE_ADDR_VALID_SHIFT      (31U)
#define COREI3C_CNTRL_DEVICE_ADDR_VALID_NS_MASK    (0x1U)
#define COREI3C_CNTRL_DEVICE_ADDR_VALID_MASK       ((uint32_t)0x1U << 31U)

/* DYNAMIC_ADDR [22:16]  RW */
#define COREI3C_CNTRL_DEVICE_ADDR_DYNADDR_SHIFT    (16U)
#define COREI3C_CNTRL_DEVICE_ADDR_DYNADDR_NS_MASK  (0x7FU)
#define COREI3C_CNTRL_DEVICE_ADDR_DYNADDR_MASK     ((uint32_t)0x7FU << 16U)

/*=============================================================================
 * Register: I3C_CAPB  (0x028)   RO
 * Short token: COREI3C_I3C_CAPB
 *===========================================================================*/
#define COREI3C_I3C_CAPB_REG_OFFSET                (0x028U)
#define COREI3C_I3C_CAPB_REG_RESET_VALUE           (0x00000000UL)
#define COREI3C_I3C_CAPB_REG_LENGTH                (0x4U)
#define COREI3C_I3C_CAPB_REG_RO_MASK               (0x00001C23U)

/* TRGT_IBI_CAP [12]  RO */
#define COREI3C_I3C_CAPB_TRGT_IBI_CAP_SHIFT        (12U)
#define COREI3C_I3C_CAPB_TRGT_IBI_CAP_NS_MASK      (0x1U)
#define COREI3C_I3C_CAPB_TRGT_IBI_CAP_MASK         ((uint32_t)0x1U << 12U)

/* TRGT_HJ_CAP [11]  RO */
#define COREI3C_I3C_CAPB_TRGT_HJ_CAP_SHIFT         (11U)
#define COREI3C_I3C_CAPB_TRGT_HJ_CAP_NS_MASK       (0x1U)
#define COREI3C_I3C_CAPB_TRGT_HJ_CAP_MASK          ((uint32_t)0x1U << 11U)

/* CMD_CCC_DEFBYTE [10]  RO */
#define COREI3C_I3C_CAPB_CCC_DEFBYTE_SHIFT         (10U)
#define COREI3C_I3C_CAPB_CCC_DEFBYTE_NS_MASK       (0x1U)
#define COREI3C_I3C_CAPB_CCC_DEFBYTE_MASK          ((uint32_t)0x1U << 10U)

/* STANDBY_CR_CAP [5]  RO */
#define COREI3C_I3C_CAPB_STANDBY_CR_SHIFT          (5U)
#define COREI3C_I3C_CAPB_STANDBY_CR_NS_MASK        (0x1U)
#define COREI3C_I3C_CAPB_STANDBY_CR_MASK           ((uint32_t)0x1U << 5U)

/* DEV_TYPE_CONFIG [1:0]  RO */
#define COREI3C_I3C_CAPB_DEV_TYPE_SHIFT            (0U)
#define COREI3C_I3C_CAPB_DEV_TYPE_NS_MASK          (0x3U)
#define COREI3C_I3C_CAPB_DEV_TYPE_MASK             ((uint32_t)0x3U << 0U)

/*=============================================================================
 * Register: RESET_CNTRL  (0x02C)   RW  (self-clearing)
 * Short token: COREI3C_RESET_CNTRL
 *===========================================================================*/
#define COREI3C_RESET_CNTRL_REG_OFFSET             (0x02CU)
#define COREI3C_RESET_CNTRL_REG_RESET_VALUE        (0x00000000UL)
#define COREI3C_RESET_CNTRL_REG_LENGTH             (0x4U)
#define COREI3C_RESET_CNTRL_REG_RW_MASK            (0x0000001FU)

/* RD_FIFO_RST [4]  RW */
#define COREI3C_RESET_CNTRL_RD_FIFO_RST_SHIFT      (4U)
#define COREI3C_RESET_CNTRL_RD_FIFO_RST_NS_MASK    (0x1U)
#define COREI3C_RESET_CNTRL_RD_FIFO_RST_MASK       ((uint32_t)0x1U << 4U)

/* WRT_FIFO_RST [3]  RW */
#define COREI3C_RESET_CNTRL_WRT_FIFO_RST_SHIFT     (3U)
#define COREI3C_RESET_CNTRL_WRT_FIFO_RST_NS_MASK   (0x1U)
#define COREI3C_RESET_CNTRL_WRT_FIFO_RST_MASK      ((uint32_t)0x1U << 3U)

/* STS_FIFO_RST [2]  RW */
#define COREI3C_RESET_CNTRL_STS_FIFO_RST_SHIFT     (2U)
#define COREI3C_RESET_CNTRL_STS_FIFO_RST_NS_MASK   (0x1U)
#define COREI3C_RESET_CNTRL_STS_FIFO_RST_MASK      ((uint32_t)0x1U << 2U)

/* CMND_FIFO_RST [1]  RW */
#define COREI3C_RESET_CNTRL_CMND_FIFO_RST_SHIFT    (1U)
#define COREI3C_RESET_CNTRL_CMND_FIFO_RST_NS_MASK  (0x1U)
#define COREI3C_RESET_CNTRL_CMND_FIFO_RST_MASK     ((uint32_t)0x1U << 1U)

/* SOFT_RST [0]  RW */
#define COREI3C_RESET_CNTRL_SOFT_RST_SHIFT         (0U)
#define COREI3C_RESET_CNTRL_SOFT_RST_NS_MASK       (0x1U)
#define COREI3C_RESET_CNTRL_SOFT_RST_MASK          ((uint32_t)0x1U << 0U)

/* Convenience: all FIFOs reset at once */
#define COREI3C_RESET_CNTRL_ALL_FIFOS_MASK \
    (COREI3C_RESET_CNTRL_RD_FIFO_RST_MASK   | \
     COREI3C_RESET_CNTRL_WRT_FIFO_RST_MASK  | \
     COREI3C_RESET_CNTRL_STS_FIFO_RST_MASK  | \
     COREI3C_RESET_CNTRL_CMND_FIFO_RST_MASK)

/*=============================================================================
 * Register: PRESENT_STATE  (0x030)   RO
 * Short token: COREI3C_PRESENT_STATE
 *===========================================================================*/
#define COREI3C_PRESENT_STATE_REG_OFFSET           (0x030U)
#define COREI3C_PRESENT_STATE_REG_RESET_VALUE      (0x00000004UL)
#define COREI3C_PRESENT_STATE_REG_LENGTH           (0x4U)
#define COREI3C_PRESENT_STATE_REG_RO_MASK          (0x00000004U)

/* CURRENT_CONTROLLER [2]  RO */
#define COREI3C_PRESENT_STATE_CURRENT_CTRL_SHIFT   (2U)
#define COREI3C_PRESENT_STATE_CURRENT_CTRL_NS_MASK (0x1U)
#define COREI3C_PRESENT_STATE_CURRENT_CTRL_MASK    ((uint32_t)0x1U << 2U)

/*=============================================================================
 * Register: INTR_STAT  (0x034)   RW1C
 * Short token: COREI3C_INTR_STAT
 *===========================================================================*/
#define COREI3C_INTR_STAT_REG_OFFSET               (0x034U)
#define COREI3C_INTR_STAT_REG_RESET_VALUE          (0x00000000UL)
#define COREI3C_INTR_STAT_REG_LENGTH               (0x4U)
#define COREI3C_INTR_STAT_REG_RW1C_MASK            (0x00003F80U)

/* I3C_ERR_CMD_SEQ_TIMEOUT [13]  RW1C */
#define COREI3C_INTR_STAT_CMD_TIMEOUT_SHIFT        (13U)
#define COREI3C_INTR_STAT_CMD_TIMEOUT_NS_MASK      (0x1U)
#define COREI3C_INTR_STAT_CMD_TIMEOUT_MASK         ((uint32_t)0x1U << 13U)

/* I3C_WARN_CMD_SEQ_STALL [12]  RW1C */
#define COREI3C_INTR_STAT_CMD_STALL_SHIFT          (12U)
#define COREI3C_INTR_STAT_CMD_STALL_NS_MASK        (0x1U)
#define COREI3C_INTR_STAT_CMD_STALL_MASK           ((uint32_t)0x1U << 12U)

/* I3C_SEQ_CANCEL [11]  RW1C */
#define COREI3C_INTR_STAT_SEQ_CANCEL_SHIFT         (11U)
#define COREI3C_INTR_STAT_SEQ_CANCEL_NS_MASK       (0x1U)
#define COREI3C_INTR_STAT_SEQ_CANCEL_MASK          ((uint32_t)0x1U << 11U)

/* I3C_INTERNAL_ERR [10]  RW1C */
#define COREI3C_INTR_STAT_INTERNAL_ERR_SHIFT       (10U)
#define COREI3C_INTR_STAT_INTERNAL_ERR_NS_MASK     (0x1U)
#define COREI3C_INTR_STAT_INTERNAL_ERR_MASK        ((uint32_t)0x1U << 10U)

/* BUS_CNTRL_CHANGED [9]  RW1C */
#define COREI3C_INTR_STAT_BUS_CTRL_CHG_SHIFT       (9U)
#define COREI3C_INTR_STAT_BUS_CTRL_CHG_NS_MASK     (0x1U)
#define COREI3C_INTR_STAT_BUS_CTRL_CHG_MASK        ((uint32_t)0x1U << 9U)

/* TRGT_IBI_REQ_STAT [8]  RW1C */
#define COREI3C_INTR_STAT_TRGT_IBI_REQ_SHIFT       (8U)
#define COREI3C_INTR_STAT_TRGT_IBI_REQ_NS_MASK     (0x1U)
#define COREI3C_INTR_STAT_TRGT_IBI_REQ_MASK        ((uint32_t)0x1U << 8U)

/* TRGT_DYN_ADDR_ASSIGN [7]  RW1C */
#define COREI3C_INTR_STAT_DYN_ADDR_SHIFT           (7U)
#define COREI3C_INTR_STAT_DYN_ADDR_NS_MASK         (0x1U)
#define COREI3C_INTR_STAT_DYN_ADDR_MASK            ((uint32_t)0x1U << 7U)

/*=============================================================================
 * Register: INTR_STAT_EN  (0x038)   RW
 * Short token: COREI3C_INTR_STAT_EN
 *===========================================================================*/
#define COREI3C_INTR_STAT_EN_REG_OFFSET            (0x038U)
#define COREI3C_INTR_STAT_EN_REG_RESET_VALUE       (0x00000000UL)
#define COREI3C_INTR_STAT_EN_REG_LENGTH            (0x4U)
#define COREI3C_INTR_STAT_EN_REG_RW_MASK           (0x00003FC0U)

#define COREI3C_INTR_STAT_EN_CMD_TIMEOUT_SHIFT     (13U)
#define COREI3C_INTR_STAT_EN_CMD_TIMEOUT_NS_MASK   (0x1U)
#define COREI3C_INTR_STAT_EN_CMD_TIMEOUT_MASK      ((uint32_t)0x1U << 13U)

#define COREI3C_INTR_STAT_EN_CMD_STALL_SHIFT       (12U)
#define COREI3C_INTR_STAT_EN_CMD_STALL_NS_MASK     (0x1U)
#define COREI3C_INTR_STAT_EN_CMD_STALL_MASK        ((uint32_t)0x1U << 12U)

#define COREI3C_INTR_STAT_EN_SEQ_CANCEL_SHIFT      (11U)
#define COREI3C_INTR_STAT_EN_SEQ_CANCEL_NS_MASK    (0x1U)
#define COREI3C_INTR_STAT_EN_SEQ_CANCEL_MASK       ((uint32_t)0x1U << 11U)

#define COREI3C_INTR_STAT_EN_INTERNAL_ERR_SHIFT    (10U)
#define COREI3C_INTR_STAT_EN_INTERNAL_ERR_NS_MASK  (0x1U)
#define COREI3C_INTR_STAT_EN_INTERNAL_ERR_MASK     ((uint32_t)0x1U << 10U)

#define COREI3C_INTR_STAT_EN_BUS_CTRL_CHG_SHIFT    (9U)
#define COREI3C_INTR_STAT_EN_BUS_CTRL_CHG_NS_MASK  (0x1U)
#define COREI3C_INTR_STAT_EN_BUS_CTRL_CHG_MASK     ((uint32_t)0x1U << 9U)

#define COREI3C_INTR_STAT_EN_TRGT_IBI_SHIFT        (8U)
#define COREI3C_INTR_STAT_EN_TRGT_IBI_NS_MASK      (0x1U)
#define COREI3C_INTR_STAT_EN_TRGT_IBI_MASK         ((uint32_t)0x1U << 8U)

#define COREI3C_INTR_STAT_EN_TRGT_RD_SHIFT         (7U)
#define COREI3C_INTR_STAT_EN_TRGT_RD_NS_MASK       (0x1U)
#define COREI3C_INTR_STAT_EN_TRGT_RD_MASK          ((uint32_t)0x1U << 7U)

#define COREI3C_INTR_STAT_EN_DYN_ADDR_SHIFT        (6U)
#define COREI3C_INTR_STAT_EN_DYN_ADDR_NS_MASK      (0x1U)
#define COREI3C_INTR_STAT_EN_DYN_ADDR_MASK         ((uint32_t)0x1U << 6U)

/*=============================================================================
 * Register: INTR_SIG_EN  (0x03C)   RW
 * Short token: COREI3C_INTR_SIG_EN
 *===========================================================================*/
#define COREI3C_INTR_SIG_EN_REG_OFFSET             (0x03CU)
#define COREI3C_INTR_SIG_EN_REG_RESET_VALUE        (0x00000000UL)
#define COREI3C_INTR_SIG_EN_REG_LENGTH             (0x4U)
#define COREI3C_INTR_SIG_EN_REG_RW_MASK            (0x00003FC0U)

#define COREI3C_INTR_SIG_EN_CMD_TIMEOUT_SHIFT      (13U)
#define COREI3C_INTR_SIG_EN_CMD_TIMEOUT_NS_MASK    (0x1U)
#define COREI3C_INTR_SIG_EN_CMD_TIMEOUT_MASK       ((uint32_t)0x1U << 13U)

#define COREI3C_INTR_SIG_EN_CMD_STALL_SHIFT        (12U)
#define COREI3C_INTR_SIG_EN_CMD_STALL_NS_MASK      (0x1U)
#define COREI3C_INTR_SIG_EN_CMD_STALL_MASK         ((uint32_t)0x1U << 12U)

#define COREI3C_INTR_SIG_EN_SEQ_CANCEL_SHIFT       (11U)
#define COREI3C_INTR_SIG_EN_SEQ_CANCEL_NS_MASK     (0x1U)
#define COREI3C_INTR_SIG_EN_SEQ_CANCEL_MASK        ((uint32_t)0x1U << 11U)

#define COREI3C_INTR_SIG_EN_INTERNAL_ERR_SHIFT     (10U)
#define COREI3C_INTR_SIG_EN_INTERNAL_ERR_NS_MASK   (0x1U)
#define COREI3C_INTR_SIG_EN_INTERNAL_ERR_MASK      ((uint32_t)0x1U << 10U)

#define COREI3C_INTR_SIG_EN_BUS_CTRL_CHG_SHIFT     (9U)
#define COREI3C_INTR_SIG_EN_BUS_CTRL_CHG_NS_MASK   (0x1U)
#define COREI3C_INTR_SIG_EN_BUS_CTRL_CHG_MASK      ((uint32_t)0x1U << 9U)

#define COREI3C_INTR_SIG_EN_TRGT_IBI_SHIFT         (8U)
#define COREI3C_INTR_SIG_EN_TRGT_IBI_NS_MASK       (0x1U)
#define COREI3C_INTR_SIG_EN_TRGT_IBI_MASK          ((uint32_t)0x1U << 8U)

#define COREI3C_INTR_SIG_EN_TRGT_RD_SHIFT          (7U)
#define COREI3C_INTR_SIG_EN_TRGT_RD_NS_MASK        (0x1U)
#define COREI3C_INTR_SIG_EN_TRGT_RD_MASK           ((uint32_t)0x1U << 7U)

#define COREI3C_INTR_SIG_EN_DYN_ADDR_SHIFT         (6U)
#define COREI3C_INTR_SIG_EN_DYN_ADDR_NS_MASK       (0x1U)
#define COREI3C_INTR_SIG_EN_DYN_ADDR_MASK          ((uint32_t)0x1U << 6U)

/*=============================================================================
 * Register: I3C_SCL_OD_HIGH_LOW_TIME  (0x040)   RW
 * Short token: COREI3C_SCL_OD_TIME  (alias: COREI3C_I3C_SCL_OD_HIGH_LOW_TIME)
 *===========================================================================*/
#define COREI3C_I3C_SCL_OD_HIGH_LOW_TIME_REG_OFFSET       (0x040U)
/* Short-token alias */
#define COREI3C_SCL_OD_TIME_REG_OFFSET                    (0x040U)
#define COREI3C_I3C_SCL_OD_HIGH_LOW_TIME_REG_RESET_VALUE  (0x00010003UL)
#define COREI3C_I3C_SCL_OD_HIGH_LOW_TIME_REG_LENGTH       (0x4U)
#define COREI3C_I3C_SCL_OD_HIGH_LOW_TIME_REG_RW_MASK      (0xFFFFFFFFU)

/* SCL_OD_HIGH_TIME [31:16] */
#define COREI3C_I3C_SCL_OD_HIGH_LOW_TIME_HIGH_SHIFT       (16U)
#define COREI3C_I3C_SCL_OD_HIGH_LOW_TIME_HIGH_NS_MASK     (0xFFFFU)
#define COREI3C_I3C_SCL_OD_HIGH_LOW_TIME_HIGH_MASK        ((uint32_t)0xFFFFU << 16U)
/* Short-token field aliases */
#define COREI3C_SCL_OD_HIGH_SHIFT                         (16U)
#define COREI3C_SCL_OD_HIGH_NS_MASK                       (0xFFFFU)

/* SCL_OD_LOW_TIME [15:0] */
#define COREI3C_I3C_SCL_OD_HIGH_LOW_TIME_LOW_SHIFT        (0U)
#define COREI3C_I3C_SCL_OD_HIGH_LOW_TIME_LOW_NS_MASK      (0xFFFFU)
#define COREI3C_I3C_SCL_OD_HIGH_LOW_TIME_LOW_MASK         ((uint32_t)0xFFFFU << 0U)
/* Short-token field aliases */
#define COREI3C_SCL_OD_LOW_SHIFT                          (0U)
#define COREI3C_SCL_OD_LOW_NS_MASK                        (0xFFFFU)

/*=============================================================================
 * Register: I3C_SCL_PP_HIGH_LOW_TIME  (0x044)   RW
 * Short token: COREI3C_SCL_PP_TIME  (alias: COREI3C_I3C_SCL_PP_HIGH_LOW_TIME)
 *===========================================================================*/
#define COREI3C_I3C_SCL_PP_HIGH_LOW_TIME_REG_OFFSET       (0x044U)
/* Short-token alias */
#define COREI3C_SCL_PP_TIME_REG_OFFSET                    (0x044U)
#define COREI3C_I3C_SCL_PP_HIGH_LOW_TIME_REG_RESET_VALUE  (0x00010003UL)
#define COREI3C_I3C_SCL_PP_HIGH_LOW_TIME_REG_LENGTH       (0x4U)
#define COREI3C_I3C_SCL_PP_HIGH_LOW_TIME_REG_RW_MASK      (0xFFFFFFFFU)
#define I3C_CMD_ATTR_ADDR_ASSIGN        0x00000002u
/* SCL_PP_HIGH_TIME [31:16] */
#define COREI3C_I3C_SCL_PP_HIGH_LOW_TIME_HIGH_SHIFT       (16U)
#define COREI3C_I3C_SCL_PP_HIGH_LOW_TIME_HIGH_NS_MASK     (0xFFFFU)
#define COREI3C_I3C_SCL_PP_HIGH_LOW_TIME_HIGH_MASK        ((uint32_t)0xFFFFU << 16U)
/* Short-token field aliases */
#define COREI3C_SCL_PP_HIGH_SHIFT                         (16U)
#define COREI3C_SCL_PP_HIGH_NS_MASK                       (0xFFFFU)

/* SCL_PP_LOW_TIME [15:0] */
#define COREI3C_I3C_SCL_PP_HIGH_LOW_TIME_LOW_SHIFT        (0U)
#define COREI3C_I3C_SCL_PP_HIGH_LOW_TIME_LOW_NS_MASK      (0xFFFFU)
#define COREI3C_I3C_SCL_PP_HIGH_LOW_TIME_LOW_MASK         ((uint32_t)0xFFFFU << 0U)
/* Short-token field aliases */
#define COREI3C_SCL_PP_LOW_SHIFT                          (0U)
#define COREI3C_SCL_PP_LOW_NS_MASK                        (0xFFFFU)
#define I3C_CMD_DEV_CNT(cnt)            (((uint32_t)(cnt)) << 24u)
/*=============================================================================
 * Register: I3C_PID_L  (0x048)   RO
 * Short token: COREI3C_I3C_PID_L
 *===========================================================================*/
#define COREI3C_I3C_PID_L_REG_OFFSET               (0x048U)
#define COREI3C_I3C_PID_L_REG_RESET_VALUE          (0x00000000UL)
#define COREI3C_I3C_PID_L_REG_LENGTH               (0x4U)

/*=============================================================================
 * Register: I3C_DCR_BCR_PID_M  (0x04C)   RO
 * Short token: COREI3C_I3C_DCR_BCR_PID_M
 *===========================================================================*/
#define COREI3C_I3C_DCR_BCR_PID_M_REG_OFFSET       (0x04CU)
#define COREI3C_I3C_DCR_BCR_PID_M_REG_RESET_VALUE  (0x00000000UL)
#define COREI3C_I3C_DCR_BCR_PID_M_REG_LENGTH       (0x4U)

/* DCR [31:24] */
#define COREI3C_I3C_DCR_BCR_PID_M_DCR_SHIFT        (24U)
#define COREI3C_I3C_DCR_BCR_PID_M_DCR_NS_MASK      (0xFFU)
#define COREI3C_I3C_DCR_BCR_PID_M_DCR_MASK         ((uint32_t)0xFFU << 24U)

/* BCR [23:16] */
#define COREI3C_I3C_DCR_BCR_PID_M_BCR_SHIFT        (16U)
#define COREI3C_I3C_DCR_BCR_PID_M_BCR_NS_MASK      (0xFFU)
#define COREI3C_I3C_DCR_BCR_PID_M_BCR_MASK         ((uint32_t)0xFFU << 16U)

/* PID_MSB [15:0] */
#define COREI3C_I3C_DCR_BCR_PID_M_PID_MSB_SHIFT    (0U)
#define COREI3C_I3C_DCR_BCR_PID_M_PID_MSB_NS_MASK  (0xFFFFU)
#define COREI3C_I3C_DCR_BCR_PID_M_PID_MSB_MASK     ((uint32_t)0xFFFFU << 0U)

/*=============================================================================
 * Register: I3C_CMND_FIFO  (0x050)   WO
 * Short token: COREI3C_CMND_FIFO  (alias: COREI3C_I3C_CMND_FIFO)
 *===========================================================================*/
#define COREI3C_I3C_CMND_FIFO_REG_OFFSET           (0x050U)
#define COREI3C_I3C_CMND_FIFO_REG_RESET_VALUE      (0x00000000UL)
#define COREI3C_I3C_CMND_FIFO_REG_LENGTH           (0x4U)
#define COREI3C_I3C_CMND_FIFO_REG_WO_MASK          (0xFFFFFFFFU)
/* Short-token alias */
#define COREI3C_CMND_FIFO_REG_OFFSET               (0x050U)
#define I3C_CMD_CCC(ccc)                (((uint32_t)(ccc)) << 7u)
#define I3C_CMD_TID(tid)                (((uint32_t)(tid) & 0xFu) << 3u)
//#define I3C_CCC_ENEC                    0x00u   /* Enable Events Command */
//#define I3C_CCC_DISEC                   0x01u   /* Disable Events Command */
//#define I3C_CCC_ENTDAA                  0x07u   /* Enter Dynamic Address Assignment */
//#define I3C_CCC_RSTDAA                  0x06u   /* Reset Dynamic Address Assignment */

/*
 * Well-known CCC command codes (direct)
 */
//#define I3C_CCC_SETDASA                 0x87u   /* Set Dynamic Addr from Static Addr */
//#define I3C_CCC_GETPID                  0x8Du   /* Get Provisional ID */
//#define I3C_CCC_GETBCR                  0x8Eu   /* Get Bus Characteristics Register */
//#define I3C_CCC_GETDCR                  0x8Fu   /* Get Device Characteristics Register */
/*
 * Command FIFO descriptor bit field encoding (32-bit word written to CMND_FIFO)
 *
 * Private Transfer Command:
 *   [31:23] DEV_ADDR   — 7-bit target dynamic address (in [22:16], upper bits 0)
 *   [22]    RNW        — 1=read from target, 0=write to target
 *   [21:16] XFER_LEN   — transfer length in bytes (max 63)
 *   [15:8]  TID        — transaction ID (4-bit, placed in [11:8])
 *   [2]     TOC        — terminate on completion (emit STOP)
 *   [1]     ROC        — response on completion (write to STATUS FIFO)
 *   [0]     CP         — command present (always 1)
 *
 * CCC Command:
 *   [31:24] CCC_CODE   — CCC byte value
 *   [23:17] TGT_ADDR   — target address (direct CCC only)
 *   [16]    BROADCAST  — 1=broadcast CCC, 0=direct CCC
 *   [15:8]  DATA_LEN   — payload byte count
 *   [11:8]  TID        — transaction ID
 *   [2]     TOC
 *   [1]     ROC
 *   [0]     CP
 */

/* Private command fields */
#define COREI3C_CMD_DEV_ADDR_SHIFT                 (16U)
#define COREI3C_CMD_DEV_ADDR_NS_MASK               (0x7FU)
#define COREI3C_CMD_DEV_ADDR_MASK                  ((uint32_t)0x7FU << 16U)

#define COREI3C_CMD_RNW_SHIFT                      (22U)
#define COREI3C_CMD_RNW_NS_MASK                    (0x1U)
#define COREI3C_CMD_RNW_MASK                       ((uint32_t)0x1U << 22U)

#define COREI3C_CMD_XFER_LEN_SHIFT                 (16U)
#define COREI3C_CMD_XFER_LEN_NS_MASK               (0x3FU)

#define COREI3C_CMD_TID_SHIFT                      (8U)
#define COREI3C_CMD_TID_NS_MASK                    (0xFU)
#define COREI3C_CMD_TID_MASK                       ((uint32_t)0xFU << 8U)

#define COREI3C_CMD_TOC_SHIFT                      (2U)
#define COREI3C_CMD_TOC_NS_MASK                    (0x1U)
#define COREI3C_CMD_TOC_MASK                       ((uint32_t)0x1U << 2U)

#define COREI3C_CMD_ROC_SHIFT                      (1U)
#define COREI3C_CMD_ROC_NS_MASK                    (0x1U)
#define COREI3C_CMD_ROC_MASK                       ((uint32_t)0x1U << 1U)

#define COREI3C_CMD_CP_SHIFT                       (0U)
#define COREI3C_CMD_CP_NS_MASK                     (0x1U)
#define COREI3C_CMD_CP_MASK                        ((uint32_t)0x1U << 0U)

/* CCC command specific fields */
#define COREI3C_CCC_CODE_SHIFT                     (24U)
#define COREI3C_CCC_CODE_NS_MASK                   (0xFFU)
#define COREI3C_CCC_CODE_MASK                      ((uint32_t)0xFFU << 24U)

#define COREI3C_CCC_TGT_ADDR_SHIFT                 (17U)
#define COREI3C_CCC_TGT_ADDR_NS_MASK               (0x7FU)
#define COREI3C_CCC_TGT_ADDR_MASK                  ((uint32_t)0x7FU << 17U)

#define COREI3C_CCC_BROADCAST_SHIFT                (16U)
#define COREI3C_CCC_BROADCAST_NS_MASK              (0x1U)
#define COREI3C_CCC_BROADCAST_MASK                 ((uint32_t)0x1U << 16U)

#define COREI3C_CCC_DATA_LEN_SHIFT                 (8U)
#define COREI3C_CCC_DATA_LEN_NS_MASK               (0xFFU)
#define COREI3C_CCC_DATA_LEN_MASK                  ((uint32_t)0xFFU << 8U)

/*=============================================================================
 * Register: I3C_WRT_FIFO  (0x054)   WO
 * Short token: COREI3C_WRT_FIFO  (alias: COREI3C_I3C_WRT_FIFO)
 *===========================================================================*/
#define COREI3C_I3C_WRT_FIFO_REG_OFFSET            (0x054U)
#define COREI3C_I3C_WRT_FIFO_REG_RESET_VALUE       (0x00000000UL)
#define COREI3C_I3C_WRT_FIFO_REG_LENGTH            (0x4U)
#define COREI3C_I3C_WRT_FIFO_REG_WO_MASK           (0xFFFFFFFFU)
/* Short-token alias */
#define COREI3C_WRT_FIFO_REG_OFFSET                (0x054U)

/*=============================================================================
 * Register: I3C_RD_FIFO  (0x058)   RO
 * Short token: COREI3C_RD_FIFO  (alias: COREI3C_I3C_RD_FIFO)
 *===========================================================================*/
#define COREI3C_I3C_RD_FIFO_REG_OFFSET             (0x058U)
#define COREI3C_I3C_RD_FIFO_REG_RESET_VALUE        (0x00000000UL)
#define COREI3C_I3C_RD_FIFO_REG_LENGTH             (0x4U)
#define COREI3C_I3C_RD_FIFO_REG_RO_MASK            (0xFFFFFFFFU)
/* Short-token alias */
#define COREI3C_RD_FIFO_REG_OFFSET                 (0x058U)

/*=============================================================================
 * Register: I3C_STATUS_FIFO  (0x05C)   RO
 * Short token: COREI3C_STATUS_FIFO  (alias: COREI3C_I3C_STATUS_FIFO)
 *
 * Response descriptor encoding:
 *   [31:28] ERR_TYPE   — error type code (0=no error)
 *   [27:24] TID        — transaction ID echo
 *   [23:16] DATA_LEN   — bytes transferred
 *===========================================================================*/
#define COREI3C_I3C_STATUS_FIFO_REG_OFFSET         (0x05CU)
#define COREI3C_I3C_STATUS_FIFO_REG_RESET_VALUE    (0x00000000UL)
#define COREI3C_I3C_STATUS_FIFO_REG_LENGTH         (0x4U)
#define COREI3C_I3C_STATUS_FIFO_REG_RO_MASK        (0xFFFFFFFFU)

#define COREI3C_I3C_STATUS_FIFO_ERR_TYPE_SHIFT     (28U)
#define COREI3C_I3C_STATUS_FIFO_ERR_TYPE_NS_MASK   (0xFU)
#define COREI3C_I3C_STATUS_FIFO_ERR_TYPE_MASK      ((uint32_t)0xFU << 28U)

#define COREI3C_I3C_STATUS_FIFO_TID_SHIFT          (24U)
#define COREI3C_I3C_STATUS_FIFO_TID_NS_MASK        (0xFU)
#define COREI3C_I3C_STATUS_FIFO_TID_MASK           ((uint32_t)0xFU << 24U)

#define COREI3C_I3C_STATUS_FIFO_DATA_LEN_SHIFT     (16U)
#define COREI3C_I3C_STATUS_FIFO_DATA_LEN_NS_MASK   (0xFFU)
#define COREI3C_I3C_STATUS_FIFO_DATA_LEN_MASK      ((uint32_t)0xFFU << 16U)
/* Short-token alias */
#define COREI3C_STATUS_FIFO_REG_OFFSET             (0x05CU)

/*=============================================================================
 * Register: CMND_RSPNS_INTR_STAT  (0x060)   RO/RW1C
 * Short token: COREI3C_CR_INTR_STAT  (alias: COREI3C_CMND_RSPNS_INTR_STAT)
 *===========================================================================*/
#define COREI3C_CMND_RSPNS_INTR_STAT_REG_OFFSET        (0x060U)
/* Short-token alias */
#define COREI3C_CR_INTR_STAT_REG_OFFSET                (0x060U)
#define COREI3C_CMND_RSPNS_INTR_STAT_REG_RESET_VALUE   (0x00000000UL)
#define COREI3C_CMND_RSPNS_INTR_STAT_REG_LENGTH        (0x4U)
#define COREI3C_CMND_RSPNS_INTR_STAT_REG_RW1C_MASK     (0x0000020BU)
#define COREI3C_CMND_RSPNS_INTR_STAT_REG_RO_MASK       (0x00000034U)

/* TRANSFR_ERR [9]  RW1C */
#define COREI3C_CMND_RSPNS_INTR_STAT_XFER_ERR_SHIFT    (9U)
#define COREI3C_CMND_RSPNS_INTR_STAT_XFER_ERR_NS_MASK  (0x1U)
#define COREI3C_CMND_RSPNS_INTR_STAT_XFER_ERR_MASK     ((uint32_t)0x1U << 9U)

/* TRANSFR_ABRT [5]  RO */
#define COREI3C_CMND_RSPNS_INTR_STAT_XFER_ABRT_SHIFT   (5U)
#define COREI3C_CMND_RSPNS_INTR_STAT_XFER_ABRT_NS_MASK (0x1U)
#define COREI3C_CMND_RSPNS_INTR_STAT_XFER_ABRT_MASK    ((uint32_t)0x1U << 5U)

/* RSPNS_RDY [4]  RO */
#define COREI3C_CMND_RSPNS_INTR_STAT_RESP_RDY_SHIFT    (4U)
#define COREI3C_CMND_RSPNS_INTR_STAT_RESP_RDY_NS_MASK  (0x1U)
#define COREI3C_CMND_RSPNS_INTR_STAT_RESP_RDY_MASK     ((uint32_t)0x1U << 4U)

/* CMND_RDY [3]  RW1C */
#define COREI3C_CMND_RSPNS_INTR_STAT_CMD_RDY_SHIFT     (3U)
#define COREI3C_CMND_RSPNS_INTR_STAT_CMD_RDY_NS_MASK   (0x1U)
#define COREI3C_CMND_RSPNS_INTR_STAT_CMD_RDY_MASK      ((uint32_t)0x1U << 3U)

/* IBI_RECV [2]  RO */
#define COREI3C_CMND_RSPNS_INTR_STAT_IBI_RECV_SHIFT    (2U)
#define COREI3C_CMND_RSPNS_INTR_STAT_IBI_RECV_NS_MASK  (0x1U)
#define COREI3C_CMND_RSPNS_INTR_STAT_IBI_RECV_MASK     ((uint32_t)0x1U << 2U)

/* WRT_STAT [1]  RO */
#define COREI3C_CMND_RSPNS_INTR_STAT_WRT_SHIFT         (1U)
#define COREI3C_CMND_RSPNS_INTR_STAT_WRT_NS_MASK       (0x1U)
#define COREI3C_CMND_RSPNS_INTR_STAT_WRT_MASK          ((uint32_t)0x1U << 1U)

/* RD_STAT [0]  RO */
#define COREI3C_CMND_RSPNS_INTR_STAT_RD_SHIFT          (0U)
#define COREI3C_CMND_RSPNS_INTR_STAT_RD_NS_MASK        (0x1U)
#define COREI3C_CMND_RSPNS_INTR_STAT_RD_MASK           ((uint32_t)0x1U << 0U)

/* Short-token field aliases for COREI3C_CR_INTR_STAT */
#define COREI3C_CR_STAT_CMD_RDY_MASK    COREI3C_CMND_RSPNS_INTR_STAT_CMD_RDY_MASK
#define COREI3C_CR_STAT_RESP_RDY_MASK   COREI3C_CMND_RSPNS_INTR_STAT_RESP_RDY_MASK
#define COREI3C_CR_STAT_IBI_RECV_MASK   COREI3C_CMND_RSPNS_INTR_STAT_IBI_RECV_MASK
#define COREI3C_CR_STAT_XFER_ERR_MASK   COREI3C_CMND_RSPNS_INTR_STAT_XFER_ERR_MASK
#define COREI3C_CR_STAT_XFER_ABRT_MASK  COREI3C_CMND_RSPNS_INTR_STAT_XFER_ABRT_MASK
#define COREI3C_CR_STAT_WRT_MASK        COREI3C_CMND_RSPNS_INTR_STAT_WRT_MASK
#define COREI3C_CR_STAT_RD_MASK         COREI3C_CMND_RSPNS_INTR_STAT_RD_MASK

/*=============================================================================
 * Register: CMND_RSPNS_INTR_STAT_EN  (0x064)   RW
 * Short token: COREI3C_CR_INTR_STAT_EN  (alias: COREI3C_CMND_RSPNS_INTR_STAT_EN)
 *===========================================================================*/
#define COREI3C_CMND_RSPNS_INTR_STAT_EN_REG_OFFSET     (0x064U)
/* Short-token alias */
#define COREI3C_CR_INTR_STAT_EN_REG_OFFSET              (0x064U)
#define COREI3C_CMND_RSPNS_INTR_STAT_EN_REG_RESET_VALUE (0x00000000UL)
#define COREI3C_CMND_RSPNS_INTR_STAT_EN_REG_LENGTH     (0x4U)
#define COREI3C_CMND_RSPNS_INTR_STAT_EN_REG_RW_MASK    (0x0000023FU)

#define COREI3C_CMND_RSPNS_INTR_STAT_EN_XFER_ERR_MASK  ((uint32_t)0x1U << 9U)
#define COREI3C_CMND_RSPNS_INTR_STAT_EN_XFER_ABRT_MASK ((uint32_t)0x1U << 5U)
#define COREI3C_CMND_RSPNS_INTR_STAT_EN_RESP_RDY_MASK  ((uint32_t)0x1U << 4U)
#define COREI3C_CMND_RSPNS_INTR_STAT_EN_CMD_RDY_MASK   ((uint32_t)0x1U << 3U)
#define COREI3C_CMND_RSPNS_INTR_STAT_EN_IBI_RECV_MASK  ((uint32_t)0x1U << 2U)
#define COREI3C_CMND_RSPNS_INTR_STAT_EN_WRT_MASK       ((uint32_t)0x1U << 1U)
#define COREI3C_CMND_RSPNS_INTR_STAT_EN_RD_MASK        ((uint32_t)0x1U << 0U)

#define COREI3C_CMND_RSPNS_ALL_STAT_MASK       (0x00000001u  | \
                                                0x00000010u  | \
                                                0x00000004u  | \
                                                0x00000008u  | \
                                                0x00000002u  | \
                                                0x00000020u  | \
                                                0x00000200u)

/*=============================================================================
 * Register: CMND_RSPNS_INTR_SIG_EN  (0x068)   RW
 * Short token: COREI3C_CR_INTR_SIG_EN  (alias: COREI3C_CMND_RSPNS_INTR_SIG_EN)
 *===========================================================================*/
#define COREI3C_CMND_RSPNS_INTR_SIG_EN_REG_OFFSET      (0x068U)
/* Short-token alias */
#define COREI3C_CR_INTR_SIG_EN_REG_OFFSET               (0x068U)
#define COREI3C_CMND_RSPNS_INTR_SIG_EN_REG_RESET_VALUE (0x00000000UL)
#define COREI3C_CMND_RSPNS_INTR_SIG_EN_REG_LENGTH      (0x4U)
#define COREI3C_CMND_RSPNS_INTR_SIG_EN_REG_RW_MASK     (0x0000023FU)

#define COREI3C_CMND_RSPNS_INTR_SIG_EN_XFER_ERR_MASK   ((uint32_t)0x1U << 9U)
#define COREI3C_CMND_RSPNS_INTR_SIG_EN_XFER_ABRT_MASK  ((uint32_t)0x1U << 5U)
#define COREI3C_CMND_RSPNS_INTR_SIG_EN_RESP_RDY_MASK   ((uint32_t)0x1U << 4U)
#define COREI3C_CMND_RSPNS_INTR_SIG_EN_CMD_RDY_MASK    ((uint32_t)0x1U << 3U)
#define COREI3C_CMND_RSPNS_INTR_SIG_EN_IBI_RECV_MASK   ((uint32_t)0x1U << 2U)
#define COREI3C_CMND_RSPNS_INTR_SIG_EN_WRT_MASK        ((uint32_t)0x1U << 1U)
#define COREI3C_CMND_RSPNS_INTR_SIG_EN_RD_MASK         ((uint32_t)0x1U << 0U)

/*=============================================================================
 * Register: IBI_PORT  (0x06C)   RO
 * Short token: COREI3C_IBI_PORT
 *===========================================================================*/
#define COREI3C_IBI_PORT_REG_OFFSET                (0x06CU)
#define COREI3C_IBI_PORT_REG_RESET_VALUE           (0x00000000UL)
#define COREI3C_IBI_PORT_REG_LENGTH                (0x4U)
#define COREI3C_IBI_PORT_REG_RO_MASK               (0xFFFFFFFFU)

/*=============================================================================
 * Register: DAT_SECTION_OFFSET  (0x070)   RO
 * Short token: COREI3C_DAT_SECTION  (alias: COREI3C_DAT_SECTION_OFFSET)
 *===========================================================================*/
#define COREI3C_DAT_SECTION_OFFSET_REG_OFFSET      (0x070U)
/* Short-token alias */
#define COREI3C_DAT_SECTION_REG_OFFSET             (0x070U)
#define COREI3C_DAT_SECTION_OFFSET_REG_RESET_VALUE (0x00040200UL)
#define COREI3C_DAT_SECTION_OFFSET_REG_LENGTH      (0x4U)
#define COREI3C_DAT_SECTION_OFFSET_REG_RO_MASK     (0x000FFFFFU)

/* TABLE_SIZE [19:12] */
#define COREI3C_DAT_SECTION_OFFSET_SIZE_SHIFT      (12U)
#define COREI3C_DAT_SECTION_OFFSET_SIZE_NS_MASK    (0xFFU)
#define COREI3C_DAT_SECTION_OFFSET_SIZE_MASK       ((uint32_t)0xFFU << 12U)
/* Short-token field aliases */
#define COREI3C_DAT_TABLE_OFFSET_SHIFT             (0U)
#define COREI3C_DAT_TABLE_OFFSET_MASK              COREI3C_DAT_SECTION_OFFSET_OFFSET_MASK
#define COREI3C_DAT_TABLE_SIZE_SHIFT               COREI3C_DAT_SECTION_OFFSET_SIZE_SHIFT
#define COREI3C_DAT_TABLE_SIZE_MASK                COREI3C_DAT_SECTION_OFFSET_SIZE_MASK

/* TABLE_OFFSET [11:0] */
#define COREI3C_DAT_SECTION_OFFSET_OFFSET_SHIFT    (0U)
#define COREI3C_DAT_SECTION_OFFSET_OFFSET_NS_MASK  (0xFFFU)
#define COREI3C_DAT_SECTION_OFFSET_OFFSET_MASK     ((uint32_t)0xFFFU << 0U)

/*==============================================================================
 * DAT (Device Address Table) - starts at base + 0x200
 * Each entry is 8 bytes (2 x 32-bit words).
 *
 * DAT Word 0 (offset + 0x00):
 *   bits [31:17] - Reserved / flags
 *   bit  [16]    - Dynamic address valid (for some encodings)
 *   bits [15:0]  - Static address / I2C flag / config
 *
 * DAT Word 1 (offset + 0x04):
 *   Reserved / device-specific
 *============================================================================*/
#define COREI3C_DAT_BASE_OFFSET_REG_OFFSET  (0x200u)
#define COREI3C_DCT_BASE_OFFSET_REG_OFFSET  (0x400u)
#define COREI3C_DCT_BASE_OFFSET1_REG_OFFSET  (0x404u)
#define COREI3C_DCT_BASE_OFFSET2_REG_OFFSET  (0x412u)
#define COREI3C_DAT0_INIT_WORD0             0x00891000u     /* BMP581      dyn=0x09, even_parity=1, IBI_PAYLOAD=1 (bit 12) */
#define COREI3C_DAT1_INIT_WORD0             0x008A1000u     /* LPS22DFTR   dyn=0x0A, even_parity=1, IBI_PAYLOAD=1 (bit 12) */
#define COREI3C_DAT2_INIT_WORD0             0x80000051u     /* 24FC1025T I2C EEPROM */
#define COREI3C_DAT0_INIT_WORD1             (0u)
#define COREI3C_DAT_BASE_OFFSET1_REG_OFFSET  (0x204u)
/*=============================================================================
 * Register: DCT_SECTION_OFFSET  (0x074)   RO/RW
 * Short token: COREI3C_DCT_SECTION  (alias: COREI3C_DCT_SECTION_OFFSET)
 *===========================================================================*/
#define COREI3C_DCT_SECTION_OFFSET_REG_OFFSET      (0x074U)
/* Short-token alias */
#define COREI3C_DCT_SECTION_REG_OFFSET             (0x074U)
#define COREI3C_DCT_SECTION_OFFSET_REG_RESET_VALUE (0x00080400UL)
#define COREI3C_DCT_SECTION_OFFSET_REG_LENGTH      (0x4U)
#define COREI3C_DCT_SECTION_OFFSET_REG_RW_MASK     (0x01F00000U)
#define COREI3C_DCT_SECTION_OFFSET_REG_RO_MASK     (0x000FFFFFU)

/* TABLE_INDEX [24:20]  RW */
#define COREI3C_DCT_SECTION_OFFSET_INDEX_SHIFT     (20U)
#define COREI3C_DCT_SECTION_OFFSET_INDEX_NS_MASK   (0x1FU)
#define COREI3C_DCT_SECTION_OFFSET_INDEX_MASK      ((uint32_t)0x1FU << 20U)
/* Short-token field aliases */
#define COREI3C_DCT_TABLE_INDEX_SHIFT              COREI3C_DCT_SECTION_OFFSET_INDEX_SHIFT
#define COREI3C_DCT_TABLE_INDEX_MASK               COREI3C_DCT_SECTION_OFFSET_INDEX_MASK
#define COREI3C_DCT_TABLE_OFFSET_SHIFT             (0U)
#define COREI3C_DCT_TABLE_OFFSET_MASK              COREI3C_DCT_SECTION_OFFSET_OFFSET_MASK
#define COREI3C_DCT_TABLE_SIZE_SHIFT               COREI3C_DCT_SECTION_OFFSET_SIZE_SHIFT
#define COREI3C_DCT_TABLE_SIZE_MASK                COREI3C_DCT_SECTION_OFFSET_SIZE_MASK

/* TABLE_SIZE [19:12]  RO */
#define COREI3C_DCT_SECTION_OFFSET_SIZE_SHIFT      (12U)
#define COREI3C_DCT_SECTION_OFFSET_SIZE_NS_MASK    (0xFFU)
#define COREI3C_DCT_SECTION_OFFSET_SIZE_MASK       ((uint32_t)0xFFU << 12U)

/* TABLE_OFFSET [11:0]  RO */
#define COREI3C_DCT_SECTION_OFFSET_OFFSET_SHIFT    (0U)
#define COREI3C_DCT_SECTION_OFFSET_OFFSET_NS_MASK  (0xFFFU)
#define COREI3C_DCT_SECTION_OFFSET_OFFSET_MASK     ((uint32_t)0xFFFU << 0U)

/* PRESENT_STATE alias for controller check */
#define COREI3C_STATE_CURRENT_CTRL_MASK            COREI3C_PRESENT_STATE_CURRENT_CTRL_MASK

/* RESET_CNTRL short-token field aliases */
#define COREI3C_RESET_SOFT_MASK                    COREI3C_RESET_CNTRL_SOFT_RST_MASK
#define COREI3C_RESET_ALL_FIFOS_MASK               COREI3C_RESET_CNTRL_ALL_FIFOS_MASK

/*=============================================================================
 * MIPI I3C CCC (Common Command Code) byte values
 *===========================================================================*/
/* Broadcast CCCs */
#define I3C_CMD_RNW                                 0x20000000U
#define I3C_CMD_CCC_FLAG                            0x80000000U
#define I3C_CMD_BCAST                               0x00008000U
#define I3C_CMD_DIRECT                              0x0000C000U

#define I3C_CCC_ENEC                               (0x00U)
#define I3C_CCC_DISEC                              (0x01U)
#define I3C_CCC_RSTDAA                             (0x06U)
#define I3C_CCC_ENTDAA                             (0x07U)
#define I3C_CCC_SETMWL_BC                          (0x09U)
#define I3C_CCC_SETMRL_BC                          (0x0AU)

/* Direct CCCs */
#define I3C_CCC_SETDASA                            (0x87U)
#define I3C_CCC_SETNEWDA                           (0x84U)
#define I3C_CCC_GETPID                             (0x8DU)
#define I3C_CCC_GETBCR                             (0x8EU)
#define I3C_CCC_GETDCR                             (0x8FU)
#define I3C_CCC_GETSTATUS                          (0x90U)
#define I3C_CCC_GETMWL                             (0x8BU)
#define I3C_CCC_GETMRL                             (0x8CU)

#define I3C_CMD_ATTR_TRANSFER                       0x00000000u
#define I3C_CMD_ATTR_ADDR_ASSIGN                    0x00000002u

/* I3C broadcast address (used for broadcast CCCs and DAA) */
#define I3C_BROADCAST_ADDR                         (0x7EU)


#ifdef __cplusplus
}
#endif

#endif /* COREI3C_REGS_H_ */
