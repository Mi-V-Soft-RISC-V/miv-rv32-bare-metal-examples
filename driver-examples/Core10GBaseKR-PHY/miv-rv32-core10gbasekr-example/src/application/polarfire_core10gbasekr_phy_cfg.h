/**
 * Copyright 2022 Microchip FPGA Embedded Systems Solutions.
 *
 * SPDX-License-Identifier: MIT
 *
 * @file polarfire_core10gbasekr_phy_cfg.h
 * @author Microchip FPGA Embedded Systems Solutions
 * @brief Core10GBaseKR PHY and PolarFire XCVR Configuration
 *
 */

#ifndef POLARFIRE_CORE10GBASEKR_PHY_CFG_H_
#define POLARFIRE_CORE10GBASEKR_PHY_CFG_H_

#include "drivers/fpga_ip/Core10GBaseKR_PHY/core10gbasekr_phy.h"
#include "PF_XCVR_C10GB/pf_xcvr_c10gb.h"
#include "fpga_design_config/fpga_design_config.h"

#define PF_XCVR_CDR_SAMPLES (10u)

/**
 * PolarFire XCVR hardware configurations instance 0
 */
pf_xcvr_c10gb_cfg_t polarfire_xcvr_cfg_0 = {.base_addr = SERDES_BASE_ADDRESS,
                                          .serdes_id = SERDES_QUAD_2,
                                          .lane_id = SERDES_LANE_0};

/**
 * The functions below integrate the PF_XCVR_C10GB APIs that are required by the
 * Core10GBaseKR_PHY Drivers XCVR API structure function pointers.
 */
uint8_t
polarfire_xcvr_init(void *xcvr)
{
    PF_XCVR_c10gb_init((pf_xcvr_c10gb_instance_t *)xcvr);
    return 0;
}

uint8_t
polarfire_xcvr_an_data_rate(void *xcvr)
{
    PF_XCVR_set_data_rate_auto_negotiation((pf_xcvr_c10gb_instance_t *)xcvr);
    return 0;
}

uint8_t
polarfire_xcvr_lt_data_rate(void *xcvr)
{
    PF_XCVR_set_data_rate_link_training((pf_xcvr_c10gb_instance_t *)xcvr);
    return 0;
}

uint8_t
polarfire_xcvr_cdr_status(void *xcvr)
{
    return (uint8_t)PF_XCVR_get_cdr_lock_status((pf_xcvr_c10gb_instance_t *)xcvr,
                                                PF_XCVR_CDR_SAMPLES);
}

uint8_t
polarfire_xcvr_ctle(void *xcvr)
{
    PF_XCVR_ctle((pf_xcvr_c10gb_instance_t *)xcvr);
    return 0;
}

uint8_t
polarfire_xcvr_ctle_status(void *xcvr)
{
    return (uint8_t)PF_XCVR_ctle_status((pf_xcvr_c10gb_instance_t *)xcvr);
}

uint8_t
polarfire_xcvr_dfe_cal(void *xcvr)
{
    PF_XCVR_dfe_cal((pf_xcvr_c10gb_instance_t *)xcvr);
    return 0;
}

uint8_t
polarfire_xcvr_dfe_status(void *xcvr)
{
    return (uint8_t)PF_XCVR_dfe_cal_status((pf_xcvr_c10gb_instance_t *)xcvr);
}

uint8_t
polarfire_xcvr_reset_pcs_rx(void *xcvr)
{
    PF_XCVR_reset_pcs_rx((pf_xcvr_c10gb_instance_t *)xcvr);
    return 0;
}

uint8_t
polarfire_xcvr_tx_equalization(void *xcvr,
                               uint32_t tx_main_tap,
                               uint32_t tx_post_tap,
                               uint32_t tx_pre_tap)
{
    PF_XCVR_tx_equalization((pf_xcvr_c10gb_instance_t *)xcvr, tx_main_tap, tx_post_tap, tx_pre_tap);
    return 0;
}

/**
 * Point XCVR APIs to external PolarFire XCVR application driver instance 0
 */
phy10gkr_xcvr_api_t polarfire_xcvr_api_0 = {.init = &polarfire_xcvr_init,
                                          .auto_neg_data_rate = &polarfire_xcvr_an_data_rate,
                                          .link_training_data_rate = &polarfire_xcvr_lt_data_rate,
                                          .cdr_lock = &polarfire_xcvr_cdr_status,
                                          .ctle_cal = &polarfire_xcvr_ctle,
                                          .ctle_cal_status = &polarfire_xcvr_ctle_status,
                                          .dfe_cal = &polarfire_xcvr_dfe_cal,
                                          .dfe_cal_status = &polarfire_xcvr_dfe_status,
                                          .reset_pcs_rx = &polarfire_xcvr_reset_pcs_rx,
                                          .tx_equalization = &polarfire_xcvr_tx_equalization};

#endif /* POLARFIRE_CORE10GBASEKR_PHY_CFG_H_ */
