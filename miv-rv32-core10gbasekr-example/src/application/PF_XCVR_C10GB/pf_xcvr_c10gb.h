/**
 * Copyright 2022 Microchip FPGA Embedded Systems Solutions.
 *
 * SPDX-License-Identifier: MIT
 *
 * @file pf_xcvr_c10gb.h
 * @author Microchip FPGA Embedded Systems Solutions
 * @brief PolarFire Core10GBaseKR PHY SerDes Configuration
 *
 */

/**
 * @mainpage PolarFire Core10GBaseKR PHY SerDes Bare Metal Driver
 *
 * ==============================================================================
 * Introduction
 * ==============================================================================
 * This PolarFire XCVR driver provides a set of functions for interacting with
 * the Core10GBaseKR_PHY. All of the described functions should only be used for
 * a hardware design that uses the Core10GBaseKR_PHY IP block.
 */
#ifndef PF_XCVR_C10GBKR_H_
#define PF_XCVR_C10GBKR_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "hal/hal.h"

/*------------------------Public Register Definitions-------------------------*/

/**
 * SerDes register offsets
 */
#define PF_XCVR_C10GB_SER_DRV_CTRL_REG_OFFSET           (0x9CU)
#define PF_XCVR_C10GB_SER_DRV_DATA_CTRL_REG_OFFSET      (0x98U)
#define PF_XCVR_C10GB_SER_DRV_CTRL_SEL_REG_OFFSET       (0xA0U)
#define PF_XCVR_C10GB_DES_DFE_CAL_CTRL_2_REG_OFFSET     (0xD8U)
#define PF_XCVR_C10GB_SER_RTL_CTRL_REG_OFFSET           (0xC0U)
#define PF_XCVR_C10GB_DES_CDR_CTRL_2_REG_OFFSET         (0x08U)
#define PF_XCVR_C10GB_DES_CDR_CTRL_3_REG_OFFSET         (0x0CU)
#define PF_XCVR_C10GB_DES_DFEEM_CTRL_1_REG_OFFSET       (0x10U)
#define PF_XCVR_C10GB_DES_DFEEM_CTRL_2_REG_OFFSET       (0x14U)
#define PF_XCVR_C10GB_DES_DFEEM_CTRL_3_REG_OFFSET       (0x18U)
#define PF_XCVR_C10GB_DES_DFE_CTRL_2_REG_OFFSET         (0x24U)
#define PF_XCVR_C10GB_DES_EM_CTRL_2_REG_OFFSET          (0x2CU)
#define PF_XCVR_C10GB_SER_TERM_CTRL_REG_OFFSET          (0x14U)

/**
 * Deserializer DFE Calibration 0
 */
#define PF_XCVR_C10GB_DES_DFE_CAL_CTRL_0_REG_OFFSET     (0xD0U)

#define PF_XCVR_C10GB_EN_OFFSET_CAL_OFFSET              PF_XCVR_C10GB_DES_DFE_CAL_CTRL_0_REG_OFFSET
#define PF_XCVR_C10GB_EN_OFFSET_CAL_SHIFT               (4)
#define PF_XCVR_C10GB_EN_OFFSET_CAL_MASK                (0x00000010UL)

#define PF_XCVR_C10GB_EN_DFE_CAL_OFFSET                 PF_XCVR_C10GB_DES_DFE_CAL_CTRL_0_REG_OFFSET
#define PF_XCVR_C10GB_EN_DFE_CAL_SHIFT                  (3)
#define PF_XCVR_C10GB_EN_DFE_CAL_MASK                   (0x00000008UL)

#define PF_XCVR_C10GB_EN_FE_CAL_OFFSET                  PF_XCVR_C10GB_DES_DFE_CAL_CTRL_0_REG_OFFSET
#define PF_XCVR_C10GB_EN_FE_CAL_SHIFT                   (2)
#define PF_XCVR_C10GB_EN_FE_CAL_MASK                    (0x00000004UL)

/**
 * Deserializer DFE Calibration 1
 */
#define PF_XCVR_C10GB_DES_DFE_CAL_CTRL_1_REG_OFFSET     (0xD4U)

#define PF_XCVR_C10GB_BYPASS_DFECAL_USER_OFFSET         PF_XCVR_C10GB_DES_DFE_CAL_CTRL_1_REG_OFFSET
#define PF_XCVR_C10GB_BYPASS_DFECAL_USER_SHIFT          (0)
#define PF_XCVR_C10GB_BYPASS_DFECAL_USER_MASK           (0x00000001UL)

#define PF_XCVR_C10GB_DFE_CAL_FORCE_CDR_COEFFS_OFFSET   PF_XCVR_C10GB_DES_DFE_CAL_CTRL_1_REG_OFFSET
#define PF_XCVR_C10GB_DFE_CAL_FORCE_CDR_COEFFS_SHIFT    (11)
#define PF_XCVR_C10GB_DFE_CAL_FORCE_CDR_COEFFS_MASK     (0x00000800UL)

#define PF_XCVR_C10GB_MAX_DFE_CYCLES_OFFSET             PF_XCVR_C10GB_DES_DFE_CAL_CTRL_1_REG_OFFSET
#define PF_XCVR_C10GB_MAX_DFE_CYCLES_SHIFT              (16)
#define PF_XCVR_C10GB_MAX_DFE_CYCLES_MASK               (0x0000001FUL) << PF_XCVR_C10GB_MAX_DFE_CYCLES_SHIFT

/**
 * Deserializer DFE Calibration Command Register
 */
#define PF_XCVR_C10GB_DES_DFE_CAL_CMD_REG_OFFSET        (0xDCU)

#define PF_XCVR_C10GB_RUN_DFECAL_USER_OFFSET            PF_XCVR_C10GB_DES_DFE_CAL_CMD_REG_OFFSET
#define PF_XCVR_C10GB_RUN_DFECAL_USER_SHIFT             (16)
#define PF_XCVR_C10GB_RUN_DFECAL_USER_MASK              (0x00010000UL)

#define PF_XCVR_C10GB_RUN_FULL_CAL_USER_OFFSET          PF_XCVR_C10GB_DES_DFE_CAL_CMD_REG_OFFSET
#define PF_XCVR_C10GB_RUN_FULL_CAL_USER_SHIFT           (18)
#define PF_XCVR_C10GB_RUN_FULL_CAL_USER_MASK            (0x00040000UL)

/**
 * Deserializer DFE Calibration Flag Register
 */
#define PF_XCVR_C10GB_DES_DFE_CAL_FLAG_REG_OFFSET       (0x100U)

#define PF_XCVR_C10GB_DONE_DFECAL_OFFSET                PF_XCVR_C10GB_DES_DFE_CAL_FLAG_REG_OFFSET
#define PF_XCVR_C10GB_DONE_DFECAL_SHIFT                 (18)
#define PF_XCVR_C10GB_DONE_DFECAL_MASK                  (0x00040000UL)

#define PF_XCVR_C10GB_DONE_FULL_CAL_OFFSET              PF_XCVR_C10GB_DES_DFE_CAL_FLAG_REG_OFFSET
#define PF_XCVR_C10GB_DONE_FULL_CAL_SHIFT               (20)
#define PF_XCVR_C10GB_DONE_FULL_CAL_MASK                (0x00100000UL)

/**
 *
 */
#define PF_XCVR_C10GB_PMA_DES_RSTPD_REG_OFFSET          (0x4CU)

#define PF_XCVR_C10GB_PMA_DES_RSTPD_POWER_OFFSET        PF_XCVR_C10GB_PMA_DES_RSTPD_REG_OFFSET
#define PF_XCVR_C10GB_PMA_DES_RSTPD_POWER_SHIFT         (0)
#define PF_XCVR_C10GB_PMA_DES_RSTPD_POWER_MASK          (0x00000001UL)

#define PF_XCVR_C10GB_PMA_DES_RSTPD_RST_FIFO_OFFSET     PF_XCVR_C10GB_PMA_DES_RSTPD_REG_OFFSET
#define PF_XCVR_C10GB_PMA_DES_RSTPD_RST_FIFO_SHIFT      (5)
#define PF_XCVR_C10GB_PMA_DES_RSTPD_RST_FIFO_MASK       (0x00000020UL)

#define PF_XCVR_C10GB_PMA_DES_RSTPD_RCVEN_OFFSET        PF_XCVR_C10GB_PMA_DES_RSTPD_REG_OFFSET
#define PF_XCVR_C10GB_PMA_DES_RSTPD_RCVEN_SHIFT         (4)
#define PF_XCVR_C10GB_PMA_DES_RSTPD_RCVEN_MASK          (0x00000010UL)

/**
 * Deserializer RX PLL divider register
 */
#define PF_XCVR_C10GB_PMA_DES_RXPLL_DIV_REG_OFFSET      (0x40U)

/**
 * Serdes RTL control
 */
#define PF_XCVR_C10GB_PMA_SERDES_RTL_CTRL_REG_OFFSET    (0xC0U)

/**
 * Deserilizer calibration bypass
 */
#define PF_XCVR_C10GB_PMA_DES_DFE_CAL_BYPASS_REG_OFFSET (0xE0U)

#define PF_XCVR_C10GB_PMA_SEL_LOCK_OVERRIDE_OFFSET      PF_XCVR_C10GB_PMA_DES_DFE_CAL_BYPASS_REG_OFFSET
#define PF_XCVR_C10GB_PMA_SEL_LOCK_OVERRIDE_SHIFT       (21)
#define PF_XCVR_C10GB_PMA_SEL_LOCK_OVERRIDE_MASK        (0x00200000UL)

/**
 * RTL lock
 */
#define PF_XCVR_C10GB_PMA_DES_RTL_LOCK_CTRL_REG_OFFSET  (0x3CU)

#define PF_XCVR_C10GB_PMA_LOCK_OVERRIDE_OFFSET          PF_XCVR_C10GB_PMA_DES_RTL_LOCK_CTRL_REG_OFFSET
#define PF_XCVR_C10GB_PMA_LOCK_OVERRIDE_SHIFT           (1)
#define PF_XCVR_C10GB_PMA_LOCK_OVERRIDE_MASK            (0x0000001FUL) << PF_XCVR_C10GB_PMA_LOCK_OVERRIDE_SHIFT

/**
 * PCS Soft reset
 */
#define PF_XCVR_C10GB_PCS_LRST_R0_REG_OFFSET            (0x68U)

#define PF_XCVR_C10GB_PCS_RX_RESET_OFFSET               PF_XCVR_C10GB_PCS_LRST_R0_REG_OFFSET
#define PF_XCVR_C10GB_PCS_RX_RESET_SHIFT                (0)
#define PF_XCVR_C10GB_PCS_RX_RESET_MASK                 (0x00000001UL)

#define PF_XCVR_C10GB_PCS_RESET_CDR_RX_OFFSET           PF_XCVR_C10GB_PCS_LRST_R0_REG_OFFSET
#define PF_XCVR_C10GB_PCS_RESET_CDR_RX_SHIFT            (2)
#define PF_XCVR_C10GB_PCS_RESET_CDR_RX_MASK             (0x00000004UL)

#define PF_XCVR_C10GB_RESETS_PCS_TX_OFFSET              PF_XCVR_C10GB_PCS_LRST_R0_REG_OFFSET
#define PF_XCVR_C10GB_RESETS_PCS_TX_SHIFT               (10)
#define PF_XCVR_C10GB_RESETS_PCS_TX_MASK                (0x00000400UL)

/**
 * RTL locks
 * - CDR RXPLL Frequency Lock
 * - CDR RXPLL indicates valid rx data after lock
 */
#define PF_XCVR_C10GB_DES_RTL_LOCK_CTRL_REG_OFFSET      (0x03CU)

#define PF_XCVR_C10GB_RXPLL_FLOCK_OFFSET                PF_XCVR_C10GB_DES_RTL_LOCK_CTRL_REG_OFFSET
#define PF_XCVR_C10GB_RXPLL_FLOCK_SHIFT                 (23U)
#define PF_XCVR_C10GB_RXPLL_FLOCK_MASK                  (0x00800000UL)

#define PF_XCVR_C10GB_RXPLL_LOCK_OFFSET                 PF_XCVR_C10GB_DES_RTL_LOCK_CTRL_REG_OFFSET
#define PF_XCVR_C10GB_RXPLL_LOCK_SHIFT                  (22U)
#define PF_XCVR_C10GB_RXPLL_LOCK_MASK                   (0x00400000UL)

/**
 * PCS Lane - PMA control
 */
#define PF_XCVR_C10GB_PMA_CTRLR0_REG_OFFSET             (0x088U)

#define PF_XCVR_C10GB_PMA_RXPLL_LOCK_STATUS_OFFSET      PF_XCVR_C10GB_PMA_CTRLR0_REG_OFFSET
#define PF_XCVR_C10GB_PMA_RXPLL_LOCK_STATUS_SHIFT       (21U)
#define PF_XCVR_C10GB_PMA_RXPLL_LOCK_STATUS_MASK        (0x00200000UL)

/**
 * Initialize settings for Core10GBaseKR PHY
 */

#ifndef PF_XCVR_C10GB_REG_VAL_SER_DRV_CTRL
#define PF_XCVR_C10GB_REG_VAL_SER_DRV_CTRL (0x35007FFFUL)
#endif

#ifndef PF_XCVR_C10GB_REG_VAL_SER_DRV_DATA_CTRL
#define PF_XCVR_C10GB_REG_VAL_SER_DRV_DATA_CTRL (0xAAAAUL)
#endif

#ifndef PF_XCVR_C10GB_REG_VAL_SER_DRV_CTRL_SEL
#define PF_XCVR_C10GB_REG_VAL_SER_DRV_CTRL_SEL (0x00000000UL)
#endif

#ifndef PF_XCVR_C10GB_REG_VAL_DES_DFE_CAL_CTRL_0
#define PF_XCVR_C10GB_REG_VAL_DES_DFE_CAL_CTRL_0 (0xFF3F071DUL)
#endif

#ifndef PF_XCVR_C10GB_REG_VAL_DES_DFE_CAL_CTRL_1
#define PF_XCVR_C10GB_REG_VAL_DES_DFE_CAL_CTRL_1 (0x0105421CUL)
#endif

#ifndef PF_XCVR_C10GB_REG_VAL_DES_DFE_CAL_CTRL_2
#define PF_XCVR_C10GB_REG_VAL_DES_DFE_CAL_CTRL_2 (0x00400000UL)
#endif

#ifndef PF_XCVR_C10GB_REG_VAL_DES_DFE_CAL_CMD
#define PF_XCVR_C10GB_REG_VAL_DES_DFE_CAL_CMD (0x00000500UL)
#endif

#ifndef PF_XCVR_C10GB_REG_VAL_SER_RTL_CTRL
#define PF_XCVR_C10GB_REG_VAL_SER_RTL_CTRL (0x00010000UL)
#endif

#ifndef PF_XCVR_C10GB_REG_VAL_DES_CDR_CTRL_2
#define PF_XCVR_C10GB_REG_VAL_DES_CDR_CTRL_2 (0x0000002AUL)
#endif

#ifndef PF_XCVR_C10GB_REG_VAL_DES_CDR_CTRL_3
#define PF_XCVR_C10GB_REG_VAL_DES_CDR_CTRL_3 (0x0000036FUL)
#endif

#ifndef PF_XCVR_C10GB_REG_VAL_DES_DFEEM_CTRL_1
#define PF_XCVR_C10GB_REG_VAL_DES_DFEEM_CTRL_1 (0x0000002AUL)
#endif

#ifndef PF_XCVR_C10GB_REG_VAL_DES_DFEEM_CTRL_2
#define PF_XCVR_C10GB_REG_VAL_DES_DFEEM_CTRL_2 (0x11111508UL)
#endif

#ifndef PF_XCVR_C10GB_REG_VAL_DES_DFEEM_CTRL_3
#define PF_XCVR_C10GB_REG_VAL_DES_DFEEM_CTRL_3 (0x00006F00UL)
#endif

#ifndef PF_XCVR_C10GB_REG_VAL_DES_DFE_CTRL_2
#define PF_XCVR_C10GB_REG_VAL_DES_DFE_CTRL_2 (0x00000007UL)
#endif

#ifndef PF_XCVR_C10GB_REG_VAL_DES_EM_CTRL_2
#define PF_XCVR_C10GB_REG_VAL_DES_EM_CTRL_2 (0x00000007UL)
#endif

#ifndef PF_XCVR_C10GB_REG_VAL_DES_RXPLL_DIV
#define PF_XCVR_C10GB_REG_VAL_DES_RXPLL_DIV (0x0000F010UL)
#endif

/**
 * Auto-Negotiation data rate configurations
 */
#ifndef PF_XCVR_C10GB_AN_CFG_DES_RXPLL_DIV
#define PF_XCVR_C10GB_AN_CFG_DES_RXPLL_DIV (0x0000F010UL)
#endif

#ifndef PF_XCVR_C10GB_AN_CFG_PMA_DES_RSTPD_POWER_DOWN
#define PF_XCVR_C10GB_AN_CFG_PMA_DES_RSTPD_POWER_DOWN (0x00000032UL)
#endif

#ifndef PF_XCVR_C10GB_AN_CFG_PMA_SERDES_RTL_CTRL
#define PF_XCVR_C10GB_AN_CFG_PMA_SERDES_RTL_CTRL (0x00000000UL)
#endif

#ifndef PF_XCVR_C10GB_AN_CFG_PMA_DES_RTL_LOCK_CTRL
#define PF_XCVR_C10GB_AN_CFG_PMA_DES_RTL_LOCK_CTRL (0x00000000UL)
#endif

#ifndef PF_XCVR_C10GB_AN_CFG_PMA_DES_RSTPD_POWER_UP
#define PF_XCVR_C10GB_AN_CFG_PMA_DES_RSTPD_POWER_UP (0x00000010UL)
#endif

#ifndef PF_XCVR_C10GB_AN_CFG_PCS_LRST_R0_RESET_ASSERT
#define PF_XCVR_C10GB_AN_CFG_PCS_LRST_R0_RESET_ASSERT (0x0000040DUL)
#endif

#ifndef PF_XCVR_C10GB_AN_CFG_PCS_LRST_R0_RESET_DEASSERT
#define PF_XCVR_C10GB_AN_CFG_PCS_LRST_R0_RESET_DEASSERT (0x00000404UL)
#endif

/**
 * Link Training data rate configurations
 */
#ifndef PF_XCVR_C10GB_LT_CFG_PMA_DES_RSTPD_POWER_DOWN
#define PF_XCVR_C10GB_LT_CFG_PMA_DES_RSTPD_POWER_DOWN PF_XCVR_C10GB_AN_CFG_PMA_DES_RSTPD_POWER_DOWN
#endif

#ifndef PF_XCVR_C10GB_LT_CFG_PMA_DES_RSTPD_POWER_UP
#define PF_XCVR_C10GB_LT_CFG_PMA_DES_RSTPD_POWER_UP PF_XCVR_C10GB_AN_CFG_PMA_DES_RSTPD_POWER_UP
#endif

#ifndef PF_XCVR_C10GB_LT_CFG_DES_RXPLL_DIV
#define PF_XCVR_C10GB_LT_CFG_DES_RXPLL_DIV (0x00008242UL)
#endif

#ifndef PF_XCVR_C10GB_LT_CFG_PMA_SERDES_RTL_CTRL
#define PF_XCVR_C10GB_LT_CFG_PMA_SERDES_RTL_CTRL (0x00010000UL)
#endif

#ifndef PF_XCVR_C10GB_LT_CFG_PMA_DES_DFE_CAL_BYPASS
#define PF_XCVR_C10GB_LT_CFG_PMA_DES_DFE_CAL_BYPASS (0x00200000UL)
#endif

#ifndef PF_XCVR_C10GB_LT_CFG_PCS_LRST_R0_RESET_ASSERT
#define PF_XCVR_C10GB_LT_CFG_PCS_LRST_R0_RESET_ASSERT PF_XCVR_C10GB_AN_CFG_PCS_LRST_R0_RESET_ASSERT
#endif

#ifndef PF_XCVR_C10GB_LT_CFG_PCS_LRST_R0_RESET_DEASSERT
#define PF_XCVR_C10GB_LT_CFG_PCS_LRST_R0_RESET_DEASSERT \
    PF_XCVR_C10GB_AN_CFG_PCS_LRST_R0_RESET_DEASSERT
#endif

/* SER_DRV_CTRL */
#define PF_XCVR_C10GB_PMA_TXODRV    (1U)
#define PF_XCVR_C10GB_PMA_TXITRIM   (2U)
#define PF_XCVR_C10GB_PMA_TXDRV     (0U)
#define PF_XCVR_C10GB_PMA_TXDRVTRIM (0U)

/**
 * Clock Data Recovery lock enumeration
 */
typedef enum __data_clck_lock
{
    DATA_CLCK_UNLOCKED = 0,
    DATA_CLCK_LOCKED = 1
} data_clck_lock_t;

/**
 * Continuous time linear equalisation status enumeration
 */
typedef enum __ctle_status
{
    CTLE_CAL_INCOMPLETE = 0,
    CTLE_CAL_COMPLETE = 1
} ctle_status_t;

/*------------------------Public data structures------------------------------*/
/*----------------------------------- XCVR -----------------------------------*/
/*----------------------------------------------------------------------------*/
/**
 * The pf_xcvr_c10gb_cfg_t structure identifies the XCVR hardware configuration
 */
typedef struct __pf_xcvr_c10gb_cfg_t
{
    addr_t base_addr;
    uint32_t serdes_id;
    uint32_t lane_id;
} pf_xcvr_c10gb_cfg_t;

/**
 * The pf_xcvr_c10gb_instance_t structure identifies the XCVR instance
 */
typedef struct __pf_xcvr_c10gb_instance
{
    addr_t pcs_cmn_addr;
    addr_t pcs_lane_addr;
    addr_t pma_cmn_addr;
    addr_t pma_lane_addr;

    const void *hw_cfg;

    struct status_t
    {
        data_clck_lock_t cdr_lock; /* RX PLL Lock, Rx data valid if locked */
        data_clck_lock_t cdr_flock; /* Rx PLL frequency lock indication for CDR */
        ctle_status_t ctle_cal;
        data_clck_lock_t pma_rxpll;
    } status;
} pf_xcvr_c10gb_instance_t;

/*--------------------------------Public APIs---------------------------------*/

/**
 * The PF_XCVR_c10gb_init function initiates and configures SerDes to
 * interface with the Core10GBaseKR_PHY IP block. This function sets the base
 * addresses of the PCS and PMA lanes. It then sets the SerDes with specified
 * default values to interface with the Core10GBaseKR_PHY.
 *
 * @param pf_xcvr
 * This parameter specifies the XCVR instance
 *
 * @return
 * This function does not return a value.
 *
 * Example:
 * @code
 *     #include "pf_xcvr_c10gbkr.h"
 *     int main(void)
 *     {
 *       xcvr_c10gb_cfg_t xcvr_cfg;
 *       xcvr_cfg.base_addr = 0xXXXXXXXX;
 *       xcvr_cfg.serdes_id = 0;
 *       xcvr_cfg.lane_id = 0;
 *       g_xcvr.hw_cfg = &xcvr_cfg;
 *       PF_XCVR_c10gb_config(&g_xcvr);
 *       return (0u);
 *     }
 * @endcode
 */
void PF_XCVR_c10gb_init(pf_xcvr_c10gb_instance_t *xcvr);

/**
 * The PF_XCVR_set_data_rate_auto_negotiation function changes the XCVR RX data
 * path data-rate to 322Mbps to allow the Core10GBaseKR_PHY to auto-negotiate with a
 * link partner.
 *
 * @param xcvr
 * The xcvr parameter specifies the SerDes ID and lane number.
 *
 * @return
 * This function does not return a value.
 *
 * Example:
 * @code
 *     #include "pf_xcvr_c10gbkr.h"
 *     int main(void)
 *     {
 *       PF_XCVR_c10gb_init(&g_xcvr);
 *       PF_XCVR_set_data_rate_auto_negotiation(&g_xcvr);
 *       return (0u);
 *     }
 * @endcode
 */
void PF_XCVR_set_data_rate_auto_negotiation(pf_xcvr_c10gb_instance_t *xcvr);

/**
 * The PF_XCVR_set_data_rate_link_training function changes the XCVR data-
 * rate to 10Gbps to allow the Core10GBaseKR_PHY to perform linking training with
 * a link partner.
 *
 * @param xcvr
 * The xcvr parameter specifies the SerDes ID and lane number.
 *
 * @return
 * This function does not return a value.
 *
 * Example:
 * @code
 *     #include "pf_xcvr_c10gbkr.h"
 *     int main(void)
 *     {
 *       PF_XCVR_c10gb_init(&g_xcvr);
 *       PF_XCVR_set_data_rate_link_training(&g_xcvr);
 *       return (0u);
 *     }
 * @endcode
 */
void PF_XCVR_set_data_rate_link_training(pf_xcvr_c10gb_instance_t *xcvr);

/**
 * The PF_XCVR_get_cdr_lock_status function reads the CDR lock and flock
 * register and sets the xcvr_cgf.status.rx_pll_lock and
 * xcvr_cgf.status.rx_pll_flock members which the user can poll.
 *
 * @param xcvr
 * The xcvr parameter specifies the SerDes ID and lane number.
 *
 * @param lock_samples
 * The lock samples parameter sets how many cycles that the CDR must be locked to qualify
 * as a success.
 *
 * @return
 * This function returns 0 on success.
 *
 * Example:
 * @code
 *     #include "pf_xcvr_c10gbkr.h"
 *     int main(void)
 *     {
 *       PF_XCVR_c10gb_init(&g_xcvr);
 *       PF_XCVR_set_data_rate_link_training(&g_xcvr);
 *       if(PF_XCVR_get_cdr_lock_status(&g_xcvr) == 0)
 *       {
 *          // locked to data
 *       }
 *       return (0u);
 *     }
 * @endcode
 */
uint32_t PF_XCVR_get_cdr_lock_status(pf_xcvr_c10gb_instance_t *xcvr, uint32_t lock_samples);

/**
 * The PF_XCVR_ctle function configures and starts CTLE calibration.
 * The purpose of CTLE is to provide equalization that corrects any ISI issues due
 * to insertion loss between a link partner’s transmitter and the receiver.
 *
 * @param xcvr
 * The xcvr parameter specifies the SerDes ID and lane number.
 *
 * @return
 * This function does not return a value.
 */
void PF_XCVR_ctle(pf_xcvr_c10gb_instance_t *xcvr);

/**
 * The PF_XCVR_ctle_status function checks the status of the CTLE calibration
 * and sets the XCVR CTLE status member accordingly.
 *
 * @param xcvr
 * The xcvr parameter specifies the SerDes ID and lane number.
 *
 * @return
 * This function returns 0 on completion of CTLE.
 *
 * Example:
 * @code
 *     #include "pf_xcvr_c10gbkr.h"
 *     int main(void)
 *     {
 *       PF_XCVR_c10gb_init(&g_xcvr);
 *       PF_XCVR_ctle(&g_xcvr);
 *       while(PF_XCVR_ctle_status() != 0)
 *       {
 *          // ctle not complete
 *       }
 *       return (0u);
 *     }
 * @endcode
 */
uint32_t PF_XCVR_ctle_status(pf_xcvr_c10gb_instance_t *xcvr);

/**
 * The PF_XCVR_dfe function performs decision feedback equalizer (DFE). This
 * calibrates the taps after modification.
 *
 * @param xcvr
 * The xcvr parameter specifies the SerDes ID and lane number.
 *
 * @return
 * This function does not return a value.
 */
void PF_XCVR_dfe_cal(pf_xcvr_c10gb_instance_t *xcvr);

/**
 * The PF_XCVR_ctle_status function checks the status of the DFE calibration.
 *
 * @param xcvr
 * The xcvr parameter specifies the SerDes ID and lane number.
 *
 * @return
 * This function returns 0 on completion of DFE.
 *
 * Example:
 * @code
 *     #include "pf_xcvr_c10gbkr.h"
 *     int main(void)
 *     {
 *       PF_XCVR_c10gb_init(&g_xcvr);
 *       PF_XCVR_dfe_cal(&g_xcvr);
 *       while(PF_XCVR_dfe_cal_status() != 0)
 *       {
 *          // dfe not complete
 *       }
 *       return (0u);
 *     }
 * @endcode
 */
uint32_t PF_XCVR_dfe_cal_status(pf_xcvr_c10gb_instance_t *xcvr);

/**
 * The PF_XCVR_reset_pcs_rx function resets the PCS RX path.
 *
 * @param xcvr
 * The xcvr parameter specifies the SerDes ID and lane number.
 *
 * @return
 * This function does not return a value.
 */
void PF_XCVR_reset_pcs_rx(pf_xcvr_c10gb_instance_t *xcvr);

/**
 * The PF_XCVR_tx_equalization function calculates the transmit settings for
 * the transceiver based on the tap settings passed as arguments to the function.
 *
 * @param xcvr
 * The xcvr parameter specifies the SerDes ID and lane number.
 *
 * @param tx_main_tap
 * The tx_main_tap parameter specifies the transmit main tap value.
 *
 * @param tx_post_tap
 * The tx_post_tap parameter specifies the transmit post tap value.
 *
 * @param tx_pre_tap
 * The tx_pre_tap parameter specifies the transmit pre tap value.
 *
 * @return
 * This function does not return a value.
 */
void PF_XCVR_tx_equalization(pf_xcvr_c10gb_instance_t *xcvr,
                             uint32_t tx_main_tap,
                             uint32_t tx_post_tap,
                             uint32_t tx_pre_tap);

#ifdef __cplusplus
}
#endif

#endif /* PF_XCVR_C10GBKR_H_ */
