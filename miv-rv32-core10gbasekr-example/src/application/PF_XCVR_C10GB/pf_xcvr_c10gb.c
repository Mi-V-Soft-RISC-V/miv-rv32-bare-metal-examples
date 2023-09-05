/**
 * Copyright 2022 Microchip FPGA Embedded Systems Solutions.
 *
 * SPDX-License-Identifier: MIT
 *
 * @file pf_xcvr_c10gb.c
 * @author Microchip FPGA Embedded Systems Solutions
 * @brief PolarFire Core10GBaseKR PHY SerDes Configuration
 *
 */

#include "PF_XCVR_C10GB/pf_xcvr_c10gb.h"

/*------------------------Preprocessors---------------------------------------*/

#define PF_XCVR_C10GB_LANE_ID_BIT_MASK       (0x00001000UL)
#define PF_XCVR_C10GB_SERDES_ID_BIT_MASK     (0x00040000UL)
#define PF_XCVR_C10GB_SERDES_ID_CMN_BIT_MASK (0x00010000UL)
#define PF_XCVR_C10GB_SERDES_PMA_BIT_MASK    (0x01000000UL)

#define MAX_SEGMENT_VALUE                    (7U)
#define TRIM_SHIFT                           (8U)
#define DELAY_CURSOR_C0                      (2U)
#define DELAY_CURSOR_CP1                     (3U)
#define DELAY_SHIFT                          (4U)
#define INVERSE_CONST                        (1U)
#define INVERSE_SHIFT                        (2U)

/*------------------------Private Variables-----------------------------------*/

/**
 * The tx_tap_t enumeration identifies a TX tap.
 */
typedef enum __tx_tap
{
    TX_MAIN_TAP,
    TX_POST_TAP,
    TX_PRE_TAP
} tx_tap_t;

/**
 * The tx_equalize_t structure identifies data used during TX equalization calculations.
 */
typedef struct __tx_equalize
{
    int32_t segment;
    uint32_t delay;
    uint32_t inverse;
    uint32_t trim;
} tx_equalize_t;

/*------------------------Private Functions-----------------------------------*/

/**
 * This function calculates and returns the coefficient segment
 */
uint32_t
tx_equalize_segment(uint32_t base, int32_t exp)
{
    int32_t i;
    uint32_t cal_value;

    if (exp < 0)
    {
        HAL_ASSERT(0);
    }
    if (exp == 0)
    {
        return 1;
    }
    else
    {
        cal_value = base;
        for (i = 1; i < exp; i++)
        {
            cal_value *= base;
        }
    }
    return cal_value;
}

/**
 * This function calculates and sets the coefficient trim
 */
void
tx_equalize_trim(int32_t coeff_adj, tx_equalize_t *tx_equalize)
{
    if (tx_equalize->segment < 0)
    {
        HAL_ASSERT(0);
    }

    if (coeff_adj > MAX_SEGMENT_VALUE)
    {
        tx_equalize->trim =
            tx_equalize->trim +
            MAX_SEGMENT_VALUE * tx_equalize_segment(TRIM_SHIFT, tx_equalize->segment);
    }
    else
    {
        tx_equalize->trim = tx_equalize->trim +
                            (coeff_adj - 1) * tx_equalize_segment(TRIM_SHIFT, tx_equalize->segment);
    }
}

/**
 * This function calculates the trim, delay, inverse and segment for the
 * coefficient for each tap
 */
void
tx_equalize_update(int32_t tx_tap_value, tx_equalize_t *tx_equalize, tx_tap_t tap)
{
    int32_t coeff_adj;
    switch (tap)
    {
        case TX_MAIN_TAP:
            coeff_adj = tx_tap_value;
            while (coeff_adj > 0)
            {
                tx_equalize_trim(coeff_adj, tx_equalize);
                tx_equalize->delay =
                    tx_equalize->delay +
                    DELAY_CURSOR_C0 * tx_equalize_segment(DELAY_SHIFT, tx_equalize->segment);
                tx_equalize->inverse = 0;
                tx_equalize->segment++;
                coeff_adj -= TRIM_SHIFT;
            }
            break;

        case TX_POST_TAP:
            coeff_adj = -1 * tx_tap_value;
            while (coeff_adj > 0)
            {
                tx_equalize_trim(coeff_adj, tx_equalize);
                tx_equalize->delay =
                    tx_equalize->delay +
                    DELAY_CURSOR_CP1 * tx_equalize_segment(DELAY_SHIFT, tx_equalize->segment);
                tx_equalize->inverse =
                    tx_equalize->inverse +
                    INVERSE_CONST * tx_equalize_segment(INVERSE_SHIFT, tx_equalize->segment);
                tx_equalize->segment++;
                coeff_adj -= TRIM_SHIFT;
            }
            break;

        case TX_PRE_TAP:
            coeff_adj = -1 * tx_tap_value;
            if (coeff_adj > 0)
            {
                tx_equalize->trim =
                    tx_equalize->trim +
                    (coeff_adj - 1) * tx_equalize_segment(TRIM_SHIFT, tx_equalize->segment);
                tx_equalize->inverse =
                    tx_equalize->inverse +
                    INVERSE_CONST * tx_equalize_segment(INVERSE_SHIFT, tx_equalize->segment);
                tx_equalize->segment++;
            }

            uint32_t i;
            for (i = tx_equalize->segment; i <= MAX_SEGMENT_VALUE; i++)
            {
                tx_equalize->delay =
                    tx_equalize->delay + DELAY_CURSOR_C0 * tx_equalize_segment(DELAY_SHIFT, i);
            }
            tx_equalize->segment--;
            break;
    }
}

/*------------------------Public Functions------------------------------------*/

/**
 * Please see pf_xcvr_c10gb.h for description
 */
void
PF_XCVR_c10gb_init(pf_xcvr_c10gb_instance_t *xcvr)
{
    const pf_xcvr_c10gb_cfg_t *xcvr_cfg = xcvr->hw_cfg;
    uint32_t tmp_reg;

    /* Set PCS and PMA addresses */
    xcvr->pcs_cmn_addr = xcvr_cfg->base_addr | PF_XCVR_C10GB_SERDES_ID_CMN_BIT_MASK |
                         (PF_XCVR_C10GB_SERDES_ID_BIT_MASK << xcvr_cfg->serdes_id);

    xcvr->pcs_lane_addr = xcvr_cfg->base_addr |
                          (PF_XCVR_C10GB_SERDES_ID_BIT_MASK << xcvr_cfg->serdes_id) |
                          (PF_XCVR_C10GB_LANE_ID_BIT_MASK << xcvr_cfg->lane_id);

    xcvr->pma_cmn_addr = xcvr_cfg->base_addr | PF_XCVR_C10GB_SERDES_PMA_BIT_MASK |
                         PF_XCVR_C10GB_SERDES_ID_CMN_BIT_MASK |
                         (PF_XCVR_C10GB_SERDES_ID_BIT_MASK << xcvr_cfg->serdes_id);

    xcvr->pma_lane_addr = xcvr_cfg->base_addr | PF_XCVR_C10GB_SERDES_PMA_BIT_MASK |
                          (PF_XCVR_C10GB_SERDES_ID_BIT_MASK << xcvr_cfg->serdes_id) |
                          (PF_XCVR_C10GB_LANE_ID_BIT_MASK << xcvr_cfg->lane_id);

    /* clear status members */
    xcvr->status.cdr_flock = DATA_CLCK_UNLOCKED;
    xcvr->status.cdr_lock = DATA_CLCK_UNLOCKED;
    xcvr->status.ctle_cal = CTLE_CAL_INCOMPLETE;

    /* Initialise SerDes for 10GBaseKR PHY */
    HAL_set_32bit_reg(xcvr->pma_lane_addr,
                      PF_XCVR_C10GB_SER_DRV_CTRL,
                      PF_XCVR_C10GB_REG_VAL_SER_DRV_CTRL);

    HAL_set_32bit_reg(xcvr->pma_lane_addr,
                      PF_XCVR_C10GB_SER_DRV_DATA_CTRL,
                      PF_XCVR_C10GB_REG_VAL_SER_DRV_DATA_CTRL);

    HAL_set_32bit_reg(xcvr->pma_lane_addr,
                      PF_XCVR_C10GB_SER_DRV_CTRL_SEL,
                      PF_XCVR_C10GB_REG_VAL_SER_DRV_CTRL_SEL);

    HAL_set_32bit_reg(xcvr->pma_lane_addr,
                      PF_XCVR_C10GB_DES_DFE_CAL_CTRL_0,
                      PF_XCVR_C10GB_REG_VAL_DES_DFE_CAL_CTRL_0);

    HAL_set_32bit_reg(xcvr->pma_lane_addr,
                      PF_XCVR_C10GB_DES_DFE_CAL_CTRL_1,
                      PF_XCVR_C10GB_REG_VAL_DES_DFE_CAL_CTRL_1);

    HAL_set_32bit_reg(xcvr->pma_lane_addr,
                      PF_XCVR_C10GB_DES_DFE_CAL_CTRL_2,
                      PF_XCVR_C10GB_REG_VAL_DES_DFE_CAL_CTRL_2);

    HAL_set_32bit_reg(xcvr->pma_lane_addr,
                      PF_XCVR_C10GB_DES_DFE_CAL_CMD,
                      PF_XCVR_C10GB_REG_VAL_DES_DFE_CAL_CMD);

    HAL_set_32bit_reg(xcvr->pma_lane_addr,
                      PF_XCVR_C10GB_SER_RTL_CTRL,
                      PF_XCVR_C10GB_REG_VAL_SER_RTL_CTRL);

    tmp_reg = HAL_get_32bit_reg(xcvr->pma_lane_addr, PF_XCVR_C10GB_DES_CDR_CTRL_2);
    HAL_set_32bit_reg(xcvr->pma_lane_addr,
                      PF_XCVR_C10GB_DES_CDR_CTRL_2,
                      (tmp_reg | PF_XCVR_C10GB_REG_VAL_DES_CDR_CTRL_2));

    tmp_reg = HAL_get_32bit_reg(xcvr->pma_lane_addr, PF_XCVR_C10GB_DES_CDR_CTRL_3);
    HAL_set_32bit_reg(xcvr->pma_lane_addr,
                      PF_XCVR_C10GB_DES_CDR_CTRL_3,
                      (tmp_reg | PF_XCVR_C10GB_REG_VAL_DES_CDR_CTRL_3));

    tmp_reg = HAL_get_32bit_reg(xcvr->pma_lane_addr, PF_XCVR_C10GB_DES_DFEEM_CTRL_1);
    HAL_set_32bit_reg(xcvr->pma_lane_addr,
                      PF_XCVR_C10GB_DES_DFEEM_CTRL_1,
                      (tmp_reg | PF_XCVR_C10GB_REG_VAL_DES_DFEEM_CTRL_1));

    tmp_reg = HAL_get_32bit_reg(xcvr->pma_lane_addr, PF_XCVR_C10GB_DES_DFEEM_CTRL_2);
    HAL_set_32bit_reg(xcvr->pma_lane_addr,
                      PF_XCVR_C10GB_DES_DFEEM_CTRL_2,
                      (tmp_reg | PF_XCVR_C10GB_REG_VAL_DES_DFEEM_CTRL_2));

    tmp_reg = HAL_get_32bit_reg(xcvr->pma_lane_addr, PF_XCVR_C10GB_DES_DFEEM_CTRL_3);
    HAL_set_32bit_reg(xcvr->pma_lane_addr,
                      PF_XCVR_C10GB_DES_DFEEM_CTRL_3,
                      (tmp_reg | PF_XCVR_C10GB_REG_VAL_DES_DFEEM_CTRL_3));

    tmp_reg = HAL_get_32bit_reg(xcvr->pma_lane_addr, PF_XCVR_C10GB_DES_DFE_CTRL_2);
    HAL_set_32bit_reg(xcvr->pma_lane_addr,
                      PF_XCVR_C10GB_DES_DFE_CTRL_2,
                      (tmp_reg | PF_XCVR_C10GB_REG_VAL_DES_DFE_CTRL_2));

    tmp_reg = HAL_get_32bit_reg(xcvr->pma_lane_addr, PF_XCVR_C10GB_DES_EM_CTRL_2);
    HAL_set_32bit_reg(xcvr->pma_lane_addr,
                      PF_XCVR_C10GB_DES_EM_CTRL_2,
                      (tmp_reg | PF_XCVR_C10GB_REG_VAL_DES_EM_CTRL_2));
}

/**
 * Please see pf_xcvr_c10gb.h for description
 */
void
PF_XCVR_set_data_rate_auto_negotiation(pf_xcvr_c10gb_instance_t *xcvr)
{
    uint32_t tmp_reg = 0;

    /* Rx Power down */
    HAL_set_32bit_reg(xcvr->pma_lane_addr,
                      PF_XCVR_C10GB_PMA_DES_RSTPD,
                      PF_XCVR_C10GB_AN_CFG_PMA_DES_RSTPD_POWER_DOWN);

    /*
     * Set Deserializer RXPLL Dividers
     *
     * set cdr gain
     * set to low speed mode (0x3: 390 Mb/s - 1.56 Gb/s)
     * set rxpll reference divide to 16
     * set rxpll reference feedback divide to 16
     */
    HAL_set_32bit_reg(xcvr->pma_lane_addr,
                      PF_XCVR_C10GB_PMA_DES_RXPLL_DIV,
                      PF_XCVR_C10GB_AN_CFG_DES_RXPLL_DIV);

    /* Reset Serdes RTL ctrl to default (0x00) */
    HAL_set_32bit_reg(xcvr->pma_lane_addr,
                      PF_XCVR_C10GB_PMA_SERDES_RTL_CTRL,
                      PF_XCVR_C10GB_AN_CFG_PMA_SERDES_RTL_CTRL);

    /* Reset RTL Lock Control to default (0x00) */
    HAL_set_32bit_reg(xcvr->pma_lane_addr,
                      PF_XCVR_C10GB_PMA_DES_RTL_LOCK_CTRL,
                      PF_XCVR_C10GB_AN_CFG_PMA_DES_RTL_LOCK_CTRL);

    HAL_set_32bit_reg(xcvr->pma_lane_addr,
                      PF_XCVR_C10GB_PMA_DES_RSTPD,
                      PF_XCVR_C10GB_AN_CFG_PMA_DES_RSTPD_POWER_UP);

    HAL_set_32bit_reg(xcvr->pcs_lane_addr,
                      PF_XCVR_C10GB_PCS_LRST_R0,
                      PF_XCVR_C10GB_AN_CFG_PCS_LRST_R0_RESET_ASSERT);

    HAL_set_32bit_reg(xcvr->pcs_lane_addr,
                      PF_XCVR_C10GB_PCS_LRST_R0,
                      PF_XCVR_C10GB_AN_CFG_PCS_LRST_R0_RESET_DEASSERT);
}

/**
 * Please see pf_xcvr_c10gb.h for description
 */
void
PF_XCVR_set_data_rate_link_training(pf_xcvr_c10gb_instance_t *xcvr)
{
    uint32_t tmp_reg = 0;

    HAL_set_32bit_reg(xcvr->pma_lane_addr,
                      PF_XCVR_C10GB_PMA_DES_RSTPD,
                      PF_XCVR_C10GB_LT_CFG_PMA_DES_RSTPD_POWER_DOWN);

    HAL_set_32bit_reg(xcvr->pma_lane_addr,
                      PF_XCVR_C10GB_PMA_DES_RXPLL_DIV,
                      PF_XCVR_C10GB_LT_CFG_DES_RXPLL_DIV);

    HAL_set_32bit_reg(xcvr->pma_lane_addr,
                      PF_XCVR_C10GB_PMA_SERDES_RTL_CTRL,
                      PF_XCVR_C10GB_LT_CFG_PMA_SERDES_RTL_CTRL);

    HAL_set_32bit_reg(xcvr->pma_lane_addr,
                      PF_XCVR_C10GB_PMA_DES_RSTPD,
                      PF_XCVR_C10GB_LT_CFG_PMA_DES_RSTPD_POWER_UP);

    HAL_set_32bit_reg(xcvr->pcs_lane_addr,
                      PF_XCVR_C10GB_PCS_LRST_R0,
                      PF_XCVR_C10GB_LT_CFG_PCS_LRST_R0_RESET_ASSERT);

    HAL_set_32bit_reg(xcvr->pcs_lane_addr,
                      PF_XCVR_C10GB_PCS_LRST_R0,
                      PF_XCVR_C10GB_LT_CFG_PCS_LRST_R0_RESET_DEASSERT);
}

/**
 * Please see pf_xcvr_c10gb.h for description
 */
uint32_t
PF_XCVR_get_cdr_lock_status(pf_xcvr_c10gb_instance_t *xcvr, uint32_t lock_samples)
{
    uint32_t lock_stability_cnt = 0;

    while (lock_stability_cnt < lock_samples)
    {
        xcvr->status.cdr_flock =
            HAL_get_32bit_reg_field(xcvr->pma_lane_addr, PF_XCVR_C10GB_RXPLL_FLOCK);

        xcvr->status.cdr_lock =
            HAL_get_32bit_reg_field(xcvr->pma_lane_addr, PF_XCVR_C10GB_RXPLL_LOCK);

        if ((xcvr->status.cdr_lock == DATA_CLCK_UNLOCKED) ||
            (xcvr->status.cdr_flock == DATA_CLCK_UNLOCKED))
        {
            break;
        }
        else
        {
            lock_stability_cnt++;
        }
    }

    if (DATA_CLCK_UNLOCKED == xcvr->status.cdr_flock || DATA_CLCK_UNLOCKED == xcvr->status.cdr_lock)
    {
        return 1;
    }
    else
    {
        return 0;
    }
}

/**
 * Please see pf_xcvr_c10gb.h for description
 */
void
PF_XCVR_ctle(pf_xcvr_c10gb_instance_t *xcvr)
{
    xcvr->status.ctle_cal = CTLE_CAL_INCOMPLETE;

    uint32_t tmp_reg = 0;

    tmp_reg = HAL_get_32bit_reg(xcvr->pma_lane_addr, PF_XCVR_C10GB_DES_DFE_CAL_CTRL_0);

    tmp_reg &= ~(PF_XCVR_C10GB_EN_DFE_CAL_MASK);
    tmp_reg |= (PF_XCVR_C10GB_EN_OFFSET_CAL_MASK) | (PF_XCVR_C10GB_EN_FE_CAL_MASK);
    HAL_set_32bit_reg(xcvr->pma_lane_addr, PF_XCVR_C10GB_DES_DFE_CAL_CTRL_0, tmp_reg);

    tmp_reg = HAL_get_32bit_reg(xcvr->pma_lane_addr, PF_XCVR_C10GB_DES_DFE_CAL_CTRL_1);
    tmp_reg &= ~(PF_XCVR_C10GB_MAX_DFE_CYCLES_MASK << PF_XCVR_C10GB_MAX_DFE_CYCLES_SHIFT);
    tmp_reg |= PF_XCVR_C10GB_BYPASS_DFECAL_USER_MASK | PF_XCVR_C10GB_DFE_CAL_FORCE_CDR_COEFFS_MASK |
               ((0x1 & PF_XCVR_C10GB_MAX_DFE_CYCLES_MASK) << PF_XCVR_C10GB_MAX_DFE_CYCLES_SHIFT);
    HAL_set_32bit_reg(xcvr->pma_lane_addr, PF_XCVR_C10GB_DES_DFE_CAL_CTRL_1, tmp_reg);

    HAL_set_32bit_reg_field(xcvr->pma_lane_addr, PF_XCVR_C10GB_RUN_FULL_CAL_USER, 0x0);
    HAL_set_32bit_reg_field(xcvr->pma_lane_addr, PF_XCVR_C10GB_RUN_FULL_CAL_USER, 0x1);
    HAL_set_32bit_reg_field(xcvr->pma_lane_addr, PF_XCVR_C10GB_RUN_FULL_CAL_USER, 0x0);
}

/**
 * Please see pf_xcvr_c10gb.h for description
 */
uint32_t
PF_XCVR_ctle_status(pf_xcvr_c10gb_instance_t *xcvr)
{
    xcvr->status.ctle_cal =
        HAL_get_32bit_reg_field(xcvr->pma_lane_addr, PF_XCVR_C10GB_DONE_FULL_CAL);
    if (CTLE_CAL_COMPLETE == xcvr->status.ctle_cal)
    {
        return 0;
    }
    else
    {
        return 1;
    }
}

/**
 * Please see pf_xcvr_c10gb.h for description
 */
void
PF_XCVR_dfe_cal(pf_xcvr_c10gb_instance_t *xcvr)
{
    HAL_set_32bit_reg_field(xcvr->pcs_lane_addr, PF_XCVR_C10GB_PCS_RX_RESET, 0x1);

    HAL_set_32bit_reg_field(xcvr->pma_lane_addr, PF_XCVR_C10GB_RUN_DFECAL_USER, 0x0);
    HAL_set_32bit_reg_field(xcvr->pma_lane_addr, PF_XCVR_C10GB_RUN_DFECAL_USER, 0x1);
    HAL_set_32bit_reg_field(xcvr->pma_lane_addr, PF_XCVR_C10GB_RUN_DFECAL_USER, 0x0);
}

/**
 * Please see pf_xcvr_c10gb.h for description
 */
uint32_t
PF_XCVR_dfe_cal_status(pf_xcvr_c10gb_instance_t *xcvr)
{
    if (HAL_get_32bit_reg_field(xcvr->pma_lane_addr, PF_XCVR_C10GB_DONE_DFECAL))
    {
        return 0;
    }
    else
    {
        return 1;
    }
}

/**
 * Please see pf_xcvr_c10gb.h for description
 */
void
PF_XCVR_reset_pcs_rx(pf_xcvr_c10gb_instance_t *xcvr)
{
    HAL_set_32bit_reg_field(xcvr->pcs_lane_addr, PF_XCVR_C10GB_PCS_RX_RESET, 0x0);
}

/**
 * Please see pf_xcvr_c10gb.h for description
 */
void
PF_XCVR_tx_equalization(pf_xcvr_c10gb_instance_t *xcvr,
                        uint32_t tx_main_tap,
                        uint32_t tx_post_tap,
                        uint32_t tx_pre_tap)
{
    tx_equalize_t tx_equalize = {0};

    tx_equalize_update(tx_main_tap, &tx_equalize, TX_MAIN_TAP);
    tx_equalize_update(tx_post_tap, &tx_equalize, TX_POST_TAP);
    tx_equalize_update(tx_pre_tap, &tx_equalize, TX_PRE_TAP);

    HAL_set_32bit_reg(xcvr->pma_lane_addr,
                      PF_XCVR_C10GB_SER_DRV_CTRL,
                      (PF_XCVR_C10GB_PMA_TXODRV << 29) | (PF_XCVR_C10GB_PMA_TXITRIM << 27) |
                          (tx_equalize.segment << 24) | (tx_equalize.trim));

    HAL_set_32bit_reg(xcvr->pma_lane_addr,
                      PF_XCVR_C10GB_SER_DRV_DATA_CTRL,
                      (tx_equalize.inverse << 16) | (tx_equalize.delay));
}
