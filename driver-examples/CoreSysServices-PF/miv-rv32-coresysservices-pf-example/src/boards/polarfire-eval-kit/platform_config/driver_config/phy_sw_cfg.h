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
 * @file core10gbasekr_phy_sw_cfg.h
 * @author Microchip FPGA Embedded Systems Solutions
 * @brief PHY software configuration
 *
 */

#ifndef BOARDS_POLARFIRE_EVAL_KIT_PLATFORM_CONFIG_DRIVER_CONFIG_PHY_SW_CFG_H_
#define BOARDS_POLARFIRE_EVAL_KIT_PLATFORM_CONFIG_DRIVER_CONFIG_PHY_SW_CFG_H_

#ifdef __cplusplus
extern "C" {
#endif

/***************************************************************************//**
 * Driver versioning macros.
 */
#define CORE_VENDOR                     "Microchip"
#define CORE_LIBRARY                    "Firmware"
#define CORE_NAME                       "Core10GBaseKR_PHY_Driver"
#define CORE_VERSION                    "x.x.x"

/***************************************************************************//**
 * Supported PHY models, used to control compile time inclusion of the
 * associated PHY sub-drivers.
 */
#define CORE10GBASEKR_PHY
#define PF_XCVR_C10GB

/***************************************************************************//**
 * Define this macro to enable performance messages in the application.
 */
#undef C10GBKR_PERFORMANCE_MESSAGES

/***************************************************************************//**
 * User config options for overriding driver defaults of Core10GBaseKR_PHY
 *
 * These definitions can be overridden by defining the macro and assigning the
 * desired value.
 */
#ifdef CORE10GBASEKR_PHY

/* Main tap limits */
#undef C10GBKR_LT_MAIN_TAP_MAX_LIMIT
#undef C10GBKR_LT_MAIN_TAP_MIN_LIMIT

/* Post tap limits */
#undef C10GBKR_LT_POST_TAP_MAX_LIMIT
#undef C10GBKR_LT_POST_TAP_MIN_LIMIT

/* Pre tap limits */
#undef C10GBKR_LT_PRE_TAP_MAX_LIMIT
#undef C10GBKR_LT_PRE_TAP_MIN_LIMIT

/* Request to be sent to Link Partner
   0U => Preset
   1U => Initialize
 */
#undef C10GBKR_LT_INITIAL_REQUEST

#undef C10GBKR_LT_INITIALIZE_MAIN_TAP
#undef C10GBKR_LT_INITIALIZE_POST_TAP
#undef C10GBKR_LT_INITIALIZE_PRE_TAP

/***************************************************************************//**
  Override XCVR configurations
 */
#undef PF_XCVR_C10GB_REG_VAL_SER_DRV_CTRL
#undef PF_XCVR_C10GB_REG_VAL_SER_DRV_DATA_CTRL
#undef PF_XCVR_C10GB_REG_VAL_SER_DRV_CTRL_SEL
#undef PF_XCVR_C10GB_REG_VAL_DES_DFE_CAL_CTRL_0
#undef PF_XCVR_C10GB_REG_VAL_DES_DFE_CAL_CTRL_1
#undef PF_XCVR_C10GB_REG_VAL_DES_DFE_CAL_CTRL_2
#undef PF_XCVR_C10GB_REG_VAL_DES_DFE_CAL_CMD
#undef PF_XCVR_C10GB_REG_VAL_SER_RTL_CTRL
#undef PF_XCVR_C10GB_REG_VAL_DES_CDR_CTRL_2
#undef PF_XCVR_C10GB_REG_VAL_DES_CDR_CTRL_3
#undef PF_XCVR_C10GB_REG_VAL_DES_DFEEM_CTRL_1
#undef PF_XCVR_C10GB_REG_VAL_DES_DFEEM_CTRL_2
#undef PF_XCVR_C10GB_REG_VAL_DES_DFEEM_CTRL_3
#undef PF_XCVR_C10GB_REG_VAL_DES_DFE_CTRL_2
#undef PF_XCVR_C10GB_REG_VAL_DES_EM_CTRL_2

/***************************************************************************//**
  Override Auto-Negotiation data rate configurations
 */
#undef PF_XCVR_C10GB_AN_CFG_DES_RXPLL_DIV
#undef PF_XCVR_C10GB_AN_CFG_PMA_DES_RSTPD_POWER_DOWN
#undef PF_XCVR_C10GB_AN_CFG_PMA_SERDES_RTL_CTRL
#undef PF_XCVR_C10GB_AN_CFG_PMA_DES_RTL_LOCK_CTRL
#undef PF_XCVR_C10GB_AN_CFG_PMA_DES_RSTPD_POWER_UP
#undef PF_XCVR_C10GB_AN_CFG_PCS_LRST_R0_RESET_ASSERT
#undef PF_XCVR_C10GB_AN_CFG_PCS_LRST_R0_RESET_DEASSERT


/***************************************************************************//**
  Override Link Training data rate configurations
 */
#undef PF_XCVR_C10GB_LT_CFG_PMA_DES_RSTPD_POWER_DOWN
#undef PF_XCVR_C10GB_LT_CFG_PMA_DES_RSTPD_POWER_UP
#undef PF_XCVR_C10GB_LT_CFG_DES_RXPLL_DIV
#undef PF_XCVR_C10GB_LT_CFG_PMA_SERDES_RTL_CTRL
#undef PF_XCVR_C10GB_LT_CFG_PMA_DES_DFE_CAL_BYPASS
#undef PF_XCVR_C10GB_LT_CFG_PCS_LRST_R0_RESET_ASSERT
#undef PF_XCVR_C10GB_LT_CFG_PCS_LRST_R0_RESET_DEASSERT

#endif /* CORE10GBASEKR_PHY */

#ifdef __cplusplus
}
#endif


#endif /* BOARDS_POLARFIRE_EVAL_KIT_PLATFORM_CONFIG_DRIVER_CONFIG_PHY_SW_CFG_H_ */
