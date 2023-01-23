/***************************************************************************//**
 * Copyright 2022 Microchip FPGA Embedded Systems Solutions.
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
 * @file pf_xcvr_c10gb.c
 * @author Microchip FPGA Embedded Systems Solutions
 * @brief Core10GBaseKR PHY SerDes Configuration
 *
 */

#include "PF_XCVR_C10GB/pf_xcvr_c10gb.h"

/*------------------------Preprocessors---------------------------------------*/

#define PF_XCVR_C10GB_LANE_ID_BIT_MASK                  (0x00001000UL)
#define PF_XCVR_C10GB_SERDES_ID_BIT_MASK                (0x00040000UL)
#define PF_XCVR_C10GB_SERDES_ID_CMN_BIT_MASK            (0x00010000UL)
#define PF_XCVR_C10GB_SERDES_PMA_BIT_MASK               (0x01000000UL)

#define PF_XCVR_C10GB_CTLE_READ_TIMEOUT                 (100000U)


/*------------------------Private Variables-----------------------------------*/

/***************************************************************************//**

 */
typedef enum __tx_tap
{
    TX_MAIN_TAP,
    TX_POST_TAP,
    TX_PRE_TAP
} tx_tap_t;

/***************************************************************************//**

 */
typedef struct __tx_equalize
{
    int32_t segment;
    uint32_t delay;
    uint32_t inverse;
    uint32_t trim;
} tx_equalize_t;

/*------------------------Private Functions-----------------------------------*/

/***************************************************************************//**
  This function calculates and returns the coefficient segment
 */
uint32_t
tx_equalize_segment
(
    uint32_t base,
    int32_t exp
)
{
    int32_t i;
    uint32_t cal_value;

    if(exp < 0)
    {
        HAL_ASSERT(0);
    }
    if(exp == 0)
    {
        return 1;
    }
    else
    {
        cal_value = base;
        for(i=1; i<exp; i++)
        {
            cal_value *= base;
        }
    }
    return cal_value;
}

/***************************************************************************//**
  This function calculates and sets the coefficient trim
 */
void
tx_equalize_trim
(
    int32_t coeff_adj,
    tx_equalize_t * tx_equalize
)
{
    if(tx_equalize->segment < 0)
    {
        HAL_ASSERT(0);
    }

    if(coeff_adj > 7)
    {
        tx_equalize->trim = tx_equalize->trim + 7 *
                                tx_equalize_segment(8 , tx_equalize->segment);
    }
    else
    {
        tx_equalize->trim = tx_equalize->trim + (coeff_adj - 1) *
                                tx_equalize_segment(8 , tx_equalize->segment);
    }
}

/***************************************************************************//**
  This function calculates the trim, delay, inverse and segment for the
  coefficient for each tap
 */
void
tx_equalize_update
(
    int32_t tx_tap_value,
    tx_equalize_t * tx_equalize,
    tx_tap_t tap
)
{
    int32_t coeff_adj;
    switch(tap)
    {
    case TX_MAIN_TAP:
        coeff_adj = tx_tap_value;
        while(coeff_adj > 0)
        {
            tx_equalize_trim(coeff_adj, tx_equalize);
            tx_equalize->delay = tx_equalize->delay + 2 *
                                tx_equalize_segment(4, tx_equalize->segment);
            tx_equalize->inverse = 0;
            tx_equalize->segment++;
            coeff_adj -= 8;
        }
        break;

    case TX_POST_TAP:
        coeff_adj = -1 * tx_tap_value;
        while(coeff_adj > 0)
        {
            tx_equalize_trim(coeff_adj, tx_equalize);
            tx_equalize->delay = tx_equalize->delay + 3 *
                                tx_equalize_segment(4, tx_equalize->segment);
            tx_equalize->inverse = tx_equalize->inverse + 1 *
                                tx_equalize_segment(2, tx_equalize->segment);
            tx_equalize->segment++;
            coeff_adj -= 8;
        }
        break;

    case TX_PRE_TAP:
        coeff_adj = -1 * tx_tap_value;
        if(coeff_adj > 0)
        {
            tx_equalize->trim = tx_equalize->trim + (coeff_adj - 1) *
                                tx_equalize_segment(8, tx_equalize->segment);
            tx_equalize->inverse = tx_equalize->inverse + 1 *
                                tx_equalize_segment(2, tx_equalize->segment);
            tx_equalize->segment++;
        }

        uint32_t i;
        for(i = tx_equalize->segment; i <= 7; i++)
        {
            tx_equalize->delay = tx_equalize->delay + 2 *
                                    tx_equalize_segment(4 , i);
        }
        tx_equalize->segment--;
        break;
    }
}

/*------------------------Public Functions------------------------------------*/

/***************************************************************************//**
  Please see pf_xcvr_c10gb.h for description
 */
void
PF_XCVR_c10gb_config
(
    xcvr_c10gb_instance_t * xcvr_cfg,
    addr_t xcvr_base,
    uint32_t serdes_id,
    uint32_t lane_id
)
{
    uint32_t tmp_reg;

    /* Set xcvr config */
    xcvr_cfg->base_addr = xcvr_base;
    xcvr_cfg->serdes_id = serdes_id;
    xcvr_cfg->lane_id = lane_id;

    /* Set PCS and PMA addresses */
    xcvr_cfg->pcs_cmn_addr = xcvr_base | PF_XCVR_C10GB_SERDES_ID_CMN_BIT_MASK |
                                (PF_XCVR_C10GB_SERDES_ID_BIT_MASK << serdes_id);

    xcvr_cfg->pcs_lane_addr = xcvr_base |
                            (PF_XCVR_C10GB_SERDES_ID_BIT_MASK << serdes_id) |
                            (PF_XCVR_C10GB_LANE_ID_BIT_MASK << lane_id);

    xcvr_cfg->pma_cmn_addr = xcvr_base | PF_XCVR_C10GB_SERDES_PMA_BIT_MASK |
                                PF_XCVR_C10GB_SERDES_ID_CMN_BIT_MASK |
                                (PF_XCVR_C10GB_SERDES_ID_BIT_MASK << serdes_id);

    xcvr_cfg->pma_lane_addr = xcvr_base | PF_XCVR_C10GB_SERDES_PMA_BIT_MASK |
                            (PF_XCVR_C10GB_SERDES_ID_BIT_MASK << serdes_id) |
                            (PF_XCVR_C10GB_LANE_ID_BIT_MASK << lane_id);

    /* clear status members */
    xcvr_cfg->status.cdr_flock = DATA_CLCK_UNLOCKED;
    xcvr_cfg->status.cdr_lock = DATA_CLCK_UNLOCKED;
    xcvr_cfg->status.ctle_cal = CTLE_CAL_INCOMPLETE;

    /* Initialise SerDes for 10GBaseKR PHY */
    HAL_set_32bit_reg(xcvr_cfg->pma_lane_addr,
                        PF_XCVR_C10GB_SER_DRV_CTRL,
                        PF_XCVR_C10GB_REG_VAL_SER_DRV_CTRL);

    HAL_set_32bit_reg(xcvr_cfg->pma_lane_addr,
                        PF_XCVR_C10GB_SER_DRV_DATA_CTRL,
                        PF_XCVR_C10GB_REG_VAL_SER_DRV_DATA_CTRL);

    HAL_set_32bit_reg(xcvr_cfg->pma_lane_addr,
                        PF_XCVR_C10GB_SER_DRV_CTRL_SEL,
                        PF_XCVR_C10GB_REG_VAL_SER_DRV_CTRL_SEL);

    HAL_set_32bit_reg(xcvr_cfg->pma_lane_addr,
                        PF_XCVR_C10GB_DES_DFE_CAL_CTRL_0,
                        PF_XCVR_C10GB_REG_VAL_DES_DFE_CAL_CTRL_0);

    HAL_set_32bit_reg(xcvr_cfg->pma_lane_addr,
                        PF_XCVR_C10GB_DES_DFE_CAL_CTRL_1,
                        PF_XCVR_C10GB_REG_VAL_DES_DFE_CAL_CTRL_1);

    HAL_set_32bit_reg(xcvr_cfg->pma_lane_addr,
                        PF_XCVR_C10GB_DES_DFE_CAL_CTRL_2,
                        PF_XCVR_C10GB_REG_VAL_DES_DFE_CAL_CTRL_2);

    HAL_set_32bit_reg(xcvr_cfg->pma_lane_addr,
                        PF_XCVR_C10GB_DES_DFE_CAL_CMD,
                        PF_XCVR_C10GB_REG_VAL_DES_DFE_CAL_CMD);

    HAL_set_32bit_reg(xcvr_cfg->pma_lane_addr,
                        PF_XCVR_C10GB_SER_RTL_CTRL,
                        PF_XCVR_C10GB_REG_VAL_SER_RTL_CTRL);

    tmp_reg = HAL_get_32bit_reg(xcvr_cfg->pma_lane_addr,
                                    PF_XCVR_C10GB_DES_CDR_CTRL_2);
    HAL_set_32bit_reg(xcvr_cfg->pma_lane_addr,
                        PF_XCVR_C10GB_DES_CDR_CTRL_2,
                        (tmp_reg | PF_XCVR_C10GB_REG_VAL_DES_CDR_CTRL_2));

    tmp_reg = HAL_get_32bit_reg(xcvr_cfg->pma_lane_addr,
                                PF_XCVR_C10GB_DES_CDR_CTRL_3);
    HAL_set_32bit_reg(xcvr_cfg->pma_lane_addr,
                        PF_XCVR_C10GB_DES_CDR_CTRL_3,
                        (tmp_reg | PF_XCVR_C10GB_REG_VAL_DES_CDR_CTRL_3));

    tmp_reg = HAL_get_32bit_reg(xcvr_cfg->pma_lane_addr,
                                PF_XCVR_C10GB_DES_DFEEM_CTRL_1);
    HAL_set_32bit_reg(xcvr_cfg->pma_lane_addr,
                        PF_XCVR_C10GB_DES_DFEEM_CTRL_1,
                        (tmp_reg | PF_XCVR_C10GB_REG_VAL_DES_DFEEM_CTRL_1));

    tmp_reg = HAL_get_32bit_reg(xcvr_cfg->pma_lane_addr,
                                PF_XCVR_C10GB_DES_DFEEM_CTRL_2);
    HAL_set_32bit_reg(xcvr_cfg->pma_lane_addr,
                        PF_XCVR_C10GB_DES_DFEEM_CTRL_2,
                        (tmp_reg | PF_XCVR_C10GB_REG_VAL_DES_DFEEM_CTRL_2));

    tmp_reg = HAL_get_32bit_reg(xcvr_cfg->pma_lane_addr,
                                PF_XCVR_C10GB_DES_DFEEM_CTRL_3);
    HAL_set_32bit_reg(xcvr_cfg->pma_lane_addr,
                        PF_XCVR_C10GB_DES_DFEEM_CTRL_3,
                        (tmp_reg | PF_XCVR_C10GB_REG_VAL_DES_DFEEM_CTRL_3));

    tmp_reg = HAL_get_32bit_reg(xcvr_cfg->pma_lane_addr,
                                PF_XCVR_C10GB_DES_DFE_CTRL_2);
    HAL_set_32bit_reg(xcvr_cfg->pma_lane_addr,
            PF_XCVR_C10GB_DES_DFE_CTRL_2,
            (tmp_reg | PF_XCVR_C10GB_REG_VAL_DES_DFE_CTRL_2));

    tmp_reg = HAL_get_32bit_reg(xcvr_cfg->pma_lane_addr,
                                PF_XCVR_C10GB_DES_EM_CTRL_2);
    HAL_set_32bit_reg(xcvr_cfg->pma_lane_addr,
                        PF_XCVR_C10GB_DES_EM_CTRL_2,
                        (tmp_reg | PF_XCVR_C10GB_REG_VAL_DES_EM_CTRL_2));
}

/***************************************************************************//**
  Please see pf_xcvr_c10gb.h for description
 */
void
PF_XCVR_set_data_rate_auto_negotiation
(
    xcvr_c10gb_instance_t * xcvr_cfg
)
{
    uint32_t tmp_reg = 0;

    /* Rx Power down */
    HAL_set_32bit_reg(xcvr_cfg->pma_lane_addr,
                        PF_XCVR_C10GB_PMA_DES_RSTPD,
                        PF_XCVR_C10GB_AN_CFG_PMA_DES_RSTPD_POWER_DOWN);

    /*
     * Set Desirializer RXPLL Dividers
     *
     * set cdr gain
     * set to low speed mode (0x3: 390 Mb/s - 1.56 Gb/s)
     * set rxpll reference divide to 16
     * set rxpll reference feedback divide to 16
     */
    HAL_set_32bit_reg(xcvr_cfg->pma_lane_addr,
                        PF_XCVR_C10GB_PMA_DES_RXPLL_DIV,
                        PF_XCVR_C10GB_AN_CFG_DES_RXPLL_DIV);

    /* Reset Serdes RTL ctrl to default (0x00) */
    HAL_set_32bit_reg(xcvr_cfg->pma_lane_addr,
                        PF_XCVR_C10GB_PMA_SERDES_RTL_CTRL,
                        PF_XCVR_C10GB_AN_CFG_PMA_SERDES_RTL_CTRL);

    /* Reset RTL Lock Control to default (0x00) */
    HAL_set_32bit_reg(xcvr_cfg->pma_lane_addr,
                        PF_XCVR_C10GB_PMA_DES_RTL_LOCK_CTRL,
                        PF_XCVR_C10GB_AN_CFG_PMA_DES_RTL_LOCK_CTRL);

    HAL_set_32bit_reg(xcvr_cfg->pma_lane_addr,
                        PF_XCVR_C10GB_PMA_DES_RSTPD,
                        PF_XCVR_C10GB_AN_CFG_PMA_DES_RSTPD_POWER_UP);

    HAL_set_32bit_reg(xcvr_cfg->pcs_lane_addr,
                        PF_XCVR_C10GB_PCS_LRST_R0,
                        PF_XCVR_C10GB_AN_CFG_PCS_LRST_R0_RESET_ASSERT);

    HAL_set_32bit_reg(xcvr_cfg->pcs_lane_addr,
                        PF_XCVR_C10GB_PCS_LRST_R0,
                        PF_XCVR_C10GB_AN_CFG_PCS_LRST_R0_RESET_DEASSERT);
}

/***************************************************************************//**
  Please see pf_xcvr_c10gb.h for description
 */
void
PF_XCVR_set_data_rate_link_training
(
    xcvr_c10gb_instance_t * xcvr_cfg
)
{

    uint32_t tmp_reg = 0;

    HAL_set_32bit_reg(xcvr_cfg->pma_lane_addr,
                        PF_XCVR_C10GB_PMA_DES_RSTPD,
                        PF_XCVR_C10GB_LT_CFG_PMA_DES_RSTPD_POWER_DOWN);

    HAL_set_32bit_reg(xcvr_cfg->pma_lane_addr,
                        PF_XCVR_C10GB_PMA_DES_RXPLL_DIV,
                        PF_XCVR_C10GB_LT_CFG_DES_RXPLL_DIV);

    HAL_set_32bit_reg(xcvr_cfg->pma_lane_addr,
                        PF_XCVR_C10GB_PMA_SERDES_RTL_CTRL,
                        PF_XCVR_C10GB_LT_CFG_PMA_SERDES_RTL_CTRL);

    HAL_set_32bit_reg(xcvr_cfg->pma_lane_addr,
                        PF_XCVR_C10GB_PMA_DES_RSTPD,
                        PF_XCVR_C10GB_LT_CFG_PMA_DES_RSTPD_POWER_UP);

    HAL_set_32bit_reg(xcvr_cfg->pcs_lane_addr,
                        PF_XCVR_C10GB_PCS_LRST_R0,
                        PF_XCVR_C10GB_LT_CFG_PCS_LRST_R0_RESET_ASSERT);

    HAL_set_32bit_reg(xcvr_cfg->pcs_lane_addr,
                        PF_XCVR_C10GB_PCS_LRST_R0,
                        PF_XCVR_C10GB_LT_CFG_PCS_LRST_R0_RESET_DEASSERT);
}

/***************************************************************************//**
  Please see pf_xcvr_c10gb.h for description
 */
void
PF_XCVR_get_cdr_lock_status
(
    xcvr_c10gb_instance_t * xcvr_cfg,
    uint32_t lock_samples
)
{
    uint32_t lock_stability_cnt = 0;

    while(lock_stability_cnt < lock_samples)
    {
        xcvr_cfg->status.cdr_flock = HAL_get_32bit_reg_field(
                                                    xcvr_cfg->pma_lane_addr,
                                                    PF_XCVR_C10GB_RXPLL_FLOCK);

        xcvr_cfg->status.cdr_lock = HAL_get_32bit_reg_field(
                                                    xcvr_cfg->pma_lane_addr,
                                                    PF_XCVR_C10GB_RXPLL_LOCK);

        if((xcvr_cfg->status.cdr_lock == DATA_CLCK_UNLOCKED) ||
                (xcvr_cfg->status.cdr_flock == DATA_CLCK_UNLOCKED) )
        {
            break;
        }
        else
        {
            lock_stability_cnt++;
        }
    }
}

/***************************************************************************//**
  Please see pf_xcvr_c10gb.h for description
 */
void
PF_XCVR_get_cdr_lock
(
    xcvr_c10gb_instance_t * xcvr_cfg,
    uint32_t timeout_ms
)
{
    HAL_ASSERT(xcvr_cfg->pma_lane_addr);
    uint32_t start_time = PF_XCVR_get_current_time_ms();
    uint32_t end_time = 0;
    do
    {
        PF_XCVR_get_cdr_lock_status(xcvr_cfg, 20);

        if((xcvr_cfg->status.cdr_flock == DATA_CLCK_LOCKED) &&
                (xcvr_cfg->status.cdr_lock == DATA_CLCK_LOCKED))
        {
            break;
        }
        end_time = PF_XCVR_get_current_time_ms() - start_time;
    }while(end_time < timeout_ms);
}

/***************************************************************************//**
  Please see pf_xcvr_c10gb.h for description
 */
void
PF_XCVR_ctle
(
    xcvr_c10gb_instance_t * xcvr_cfg
)
{
    HAL_ASSERT(xcvr_cfg->pma_lane_addr);

    xcvr_cfg->status.ctle_cal = CTLE_CAL_INCOMPLETE;

    uint32_t tmp_reg = 0;

    HAL_ASSERT(xcvr_cfg->pma_lane_addr);

    tmp_reg = HAL_get_32bit_reg(xcvr_cfg->pma_lane_addr,
                                PF_XCVR_C10GB_DES_DFE_CAL_CTRL_0);

    tmp_reg &= ~(PF_XCVR_C10GB_EN_DFE_CAL_MASK);
    tmp_reg |= (PF_XCVR_C10GB_EN_OFFSET_CAL_MASK) |
                (PF_XCVR_C10GB_EN_FE_CAL_MASK);
    HAL_set_32bit_reg(xcvr_cfg->pma_lane_addr,
                        PF_XCVR_C10GB_DES_DFE_CAL_CTRL_0,
                        tmp_reg);

    tmp_reg = HAL_get_32bit_reg(xcvr_cfg->pma_lane_addr,
                                PF_XCVR_C10GB_DES_DFE_CAL_CTRL_1);
    tmp_reg &= ~(PF_XCVR_C10GB_MAX_DFE_CYCLES_MASK <<
                PF_XCVR_C10GB_MAX_DFE_CYCLES_SHIFT);
    tmp_reg |= PF_XCVR_C10GB_BYPASS_DFECAL_USER_MASK |
                PF_XCVR_C10GB_DFE_CAL_FORCE_CDR_COEFFS_MASK |
                ((0x1 & PF_XCVR_C10GB_MAX_DFE_CYCLES_MASK) <<
                PF_XCVR_C10GB_MAX_DFE_CYCLES_SHIFT);
    HAL_set_32bit_reg(xcvr_cfg->pma_lane_addr,
                        PF_XCVR_C10GB_DES_DFE_CAL_CTRL_1, tmp_reg);

    HAL_set_32bit_reg_field(xcvr_cfg->pma_lane_addr,
                            PF_XCVR_C10GB_RUN_FULL_CAL_USER,
                            0x0);
    HAL_set_32bit_reg_field(xcvr_cfg->pma_lane_addr,
                            PF_XCVR_C10GB_RUN_FULL_CAL_USER,
                            0x1);
    HAL_set_32bit_reg_field(xcvr_cfg->pma_lane_addr,
                            PF_XCVR_C10GB_RUN_FULL_CAL_USER,
                            0x0);
}

/***************************************************************************//**
  Please see pf_xcvr_c10gb.h for description
 */
void
PF_XCVR_ctle_status
(
    xcvr_c10gb_instance_t * xcvr_cfg
)
{
    xcvr_cfg->status.ctle_cal = HAL_get_32bit_reg_field(xcvr_cfg->pma_lane_addr,
                                    PF_XCVR_C10GB_DONE_FULL_CAL);
}

/***************************************************************************//**
  Please see pf_xcvr_c10gb.h for description
 */
uint32_t
PF_XCVR_serdes_dfe_cal
(
    xcvr_c10gb_instance_t * xcvr_cfg,
    uint32_t timeout_ms
)
{
    HAL_ASSERT(xcvr_cfg->pma_lane_addr | xcvr_cfg->pcs_lane_addr);

    HAL_set_32bit_reg_field(xcvr_cfg->pcs_lane_addr,
                            PF_XCVR_C10GB_PCS_RX_RESET,
                            0x1);

    HAL_set_32bit_reg_field(xcvr_cfg->pma_lane_addr,
                            PF_XCVR_C10GB_RUN_DFECAL_USER,
                            0x0);
    HAL_set_32bit_reg_field(xcvr_cfg->pma_lane_addr,
                            PF_XCVR_C10GB_RUN_DFECAL_USER,
                            0x1);
    HAL_set_32bit_reg_field(xcvr_cfg->pma_lane_addr,
                            PF_XCVR_C10GB_RUN_DFECAL_USER,
                            0x0);

    uint32_t start_time = PF_XCVR_get_current_time_ms();
    uint32_t end_time = 0;
    uint32_t dfe_done = 0;
    do
    {

        dfe_done = HAL_get_32bit_reg_field(xcvr_cfg->pma_lane_addr,
                                            PF_XCVR_C10GB_DONE_DFECAL);
        if(dfe_done)
        {
            HAL_set_32bit_reg_field(xcvr_cfg->pcs_lane_addr,
                                    PF_XCVR_C10GB_PCS_RX_RESET,
                                    0x0);
            return 0;
        }
        end_time = PF_XCVR_get_current_time_ms() - start_time;
    }while(end_time < timeout_ms);

    HAL_set_32bit_reg_field(xcvr_cfg->pcs_lane_addr,
                            PF_XCVR_C10GB_PCS_RX_RESET,
                            0x0);
    return 1;
}

/***************************************************************************//**
  Please see pf_xcvr_c10gb.h for description
 */
void
PF_XCVR_tx_equalization
(
    xcvr_c10gb_instance_t * xcvr_cfg,
    uint32_t tx_main_tap,
    uint32_t tx_post_tap,
    uint32_t tx_pre_tap
)
{
    tx_equalize_t tx_equalize = {0};

    tx_equalize_update(tx_main_tap, &tx_equalize, TX_MAIN_TAP);
    tx_equalize_update(tx_post_tap, &tx_equalize, TX_POST_TAP);
    tx_equalize_update(tx_pre_tap, &tx_equalize, TX_PRE_TAP);

    HAL_set_32bit_reg(xcvr_cfg->pma_lane_addr,
                        PF_XCVR_C10GB_SER_DRV_CTRL,
                        (PF_XCVR_C10GB_PMA_TXODRV << 29) |
                        (PF_XCVR_C10GB_PMA_TXITRIM << 27) |
                        (tx_equalize.segment << 24) | (tx_equalize.trim));

    HAL_set_32bit_reg(xcvr_cfg->pma_lane_addr,
                        PF_XCVR_C10GB_SER_DRV_DATA_CTRL,
                        (tx_equalize.inverse << 16) | (tx_equalize.delay));
}

/***************************************************************************//**
  Please see pf_xcvr_c10gb.h for description
 */
void
PF_XCVR_rxpll_lock_status
(
    xcvr_c10gb_instance_t * xcvr_cfg,
    uint32_t timeout_ms
)
{
    uint32_t start_time = PF_XCVR_get_current_time_ms();
    uint32_t end_time = 0;
    do
    {
        xcvr_cfg->status.pma_rxpll = HAL_get_32bit_reg_field(
                                        xcvr_cfg->pcs_lane_addr,
                                        PF_XCVR_C10GB_PMA_RXPLL_LOCK_STATUS);
        end_time = PF_XCVR_get_current_time_ms() - start_time;
    }while(DATA_CLCK_LOCKED == xcvr_cfg->status.pma_rxpll &&
            end_time < timeout_ms);

}
