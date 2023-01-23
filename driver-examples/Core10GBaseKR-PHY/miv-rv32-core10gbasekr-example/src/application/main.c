/*******************************************************************************
 * Copyright 2022 Microchip FPGA Embedded Systems Solutions.
 *
 * SPDX-License-Identifier: MIT
 *
 * Common example project which can run on all Mi-V soft processor variants.
 * Please refer README.md in the root folder of this project for more details.
 *
 */

#include <stdio.h>
#include "hal/hal.h"
#include "miv_rv32_hal/miv_rv32_hal.h"
#include "drivers/fpga_ip/CoreUARTapb/core_uart_apb.h"
#include "drivers/fpga_ip/Core10GBaseKR_PHY/phy.h"
#include "PF_XCVR_C10GB/pf_xcvr_c10gb.h"

#ifdef C10GBKR_PERFORMANCE_MESSAGES

#undef C10GBKR_AN_STATE_DISPLAY

/*-----------------------------------------------------------------------------
 * function prototypes
 */
void
lt_performances_display
(
    UART_instance_t * uart,
    phy10gkr_instance_t * phy,
    char * perf_msg
);

/*-----------------------------------------------------------------------------
 * print auto-negotiation state to uart
 */
void
lt_fail_display
(
    UART_instance_t * uart,
    phy10gkr_instance_t * phy,
    char * perf_msg
);


/*-----------------------------------------------------------------------------
 * print receiver calibration errors to uart
 */
void
calibration_error_display
(
    xcvr_c10gb_instance_t * xcvr,
    UART_instance_t * uart,
    phy10gkr_instance_t * phy,
    char * perf_msg
);

#ifdef C10GBKR_AN_STATE_DISPLAY
/*-----------------------------------------------------------------------------
 * print auto-negotiation state to uart
 */
void
an_state_display
(
    UART_instance_t * uart,
    phy10gkr_instance_t * phy,
    char * perf_msg
);
#endif /* C10GBKR_AN_STATE_DISPLAY */

char msg[256];

#endif /* C10GBKR_PERFORMANCE_MESSAGES */

/*-----------------------------------------------------------------------------
 * Mtime definition
 */
#define CURENT_MTIME_MS                 ((MRV_read_mtime()/(SYS_CLK_FREQ/\
                                                (MTIME_PRESCALER*1.0)))*1000)

/*-----------------------------------------------------------------------------
 * Application delay in milli-seconds prototype
 */
void
delay_ms
(
    uint32_t timeout_ms
);

/*-----------------------------------------------------------------------------
 * Timeout definitions
 */
#define LT_SOFTWARE_FAIL_TIMEOUT_MS     (500U)

/*-----------------------------------------------------------------------------
 * UART instance data.
 */
UART_instance_t g_uart;

/*-----------------------------------------------------------------------------
 * PHY instance data.
 */
phy10gkr_instance_t g_phy;

/*-----------------------------------------------------------------------------
 * XCVR instance data.
 */
xcvr_c10gb_instance_t g_xcvr;

/*-----------------------------------------------------------------------------
 * Override design specific driver APIs
 *
 * Note: called from Core10GBaseKR_PHY Driver in this application
 */
void
PHY10GKR_serdes_tx_equalization
(
    uint32_t tx_main_tap,
    uint32_t tx_post_tap,
    uint32_t tx_pre_tap
)
{
    PF_XCVR_tx_equalization(&g_xcvr, tx_main_tap, tx_post_tap, tx_pre_tap);
}

/*-----------------------------------------------------------------------------
 * Override design specific driver APIs
 *
 * Note: called from Core10GBaseKR_PHY Driver in this application
 */
uint32_t
PHY10GKR_serdes_dfe_cal
(
    void
)
{
    return PF_XCVR_serdes_dfe_cal(&g_xcvr, 10);
}

/*-----------------------------------------------------------------------------
 * Override design specific driver APIs
 *
 * Note: called from PF_XCVR Driver in this application
 */
uint32_t
PF_XCVR_get_current_time_ms
(
    void
)
{
    return (uint32_t)CURENT_MTIME_MS;
}

/*-----------------------------------------------------------------------------
 * Override design specific driver APIs
 *
 * Note: called from Core10GBaseKR_PHY Driver in this application
 */
uint32_t
PHY10GKR_get_current_time_ms
(
    void
)
{
    return (uint32_t)CURENT_MTIME_MS;
}

/*-----------------------------------------------------------------------------
 * Override design specific driver APIs
 *
 * Note: called from Core10GBaseKR_PHY Driver in this application
 */
void
PHY10GKR_serdes_an_config
(
    void
)
{
    PF_XCVR_c10gb_config(&g_xcvr,
                        SERDES_BASE_ADDRESS,
                        SERDES_QUAD_2,
                        SERDES_LANE_0);

    PHY10GKR_set_lane_los_signal(&g_phy, LANE_LOS_LOCK_TO_REF);
    delay_ms(5);

    PF_XCVR_set_data_rate_auto_negotiation(&g_xcvr);
    delay_ms(10);
}

/*-----------------------------------------------------------------------------
 * Override design specific driver APIs
 *
 * Note: called from Core10GBaseKR_PHY Driver in this application
 */
uint32_t
PHY10GKR_serdes_lt_config
(
    void
)
{
    PHY10GKR_set_lane_los_signal(&g_phy, LANE_LOS_LOCK_TO_DATA);
    PF_XCVR_set_data_rate_link_training(&g_xcvr);

    while(DATA_CLCK_UNLOCKED == g_xcvr.status.cdr_flock ||
            DATA_CLCK_UNLOCKED == g_xcvr.status.cdr_lock)
    {
        PF_XCVR_get_cdr_lock_status(&g_xcvr, 20);

        g_phy.lt.timer.end = PHY10GKR_get_current_time_ms() -
                                g_phy.lt.timer.start;

        /* determine if hardware or software timed out */
        if(g_phy.lt.timer.end >= LT_SOFTWARE_FAIL_TIMEOUT_MS ||
                HAL_get_32bit_reg_field(g_phy.lt_base_addr,
                C10GB_LT_TRAINING_FAIL) == 1)
        {
            return 1;
        }
    }

    PF_XCVR_ctle(&g_xcvr);
    while(CTLE_CAL_COMPLETE != g_xcvr.status.ctle_cal)
    {
        PF_XCVR_ctle_status(&g_xcvr);

        g_phy.lt.timer.end = PHY10GKR_get_current_time_ms() -
                                g_phy.lt.timer.start;
        if(g_phy.lt.timer.end >= LT_SOFTWARE_FAIL_TIMEOUT_MS ||
                HAL_get_32bit_reg_field(g_phy.lt_base_addr,
                C10GB_LT_TRAINING_FAIL) == 1)
        {
            return 2;
        }
    }

    /* Success: CTLE Calibration complete and CDR locked */
    return 0;
}

/*-----------------------------------------------------------------------------
 * Override design specific driver APIs
 *
 * Note: called from Core10GBaseKR_PHY Driver in this application
 */
uint32_t
PHY10GKR_serdes_cdr_lock
(
    void
)
{
    PF_XCVR_get_cdr_lock_status(&g_xcvr, 20);
    if(DATA_CLCK_UNLOCKED == g_xcvr.status.cdr_flock ||
            DATA_CLCK_UNLOCKED == g_xcvr.status.cdr_lock)
    {
        return 1;
    }
    else
    {
        return 0;
    }
}

/*-------------------------------------------------------------------------//**
  main() function.
*/
int main(void)
{
    phy10gkr_status_t status;
    uint32_t good_link_cnt;

    UART_init(&g_uart,
                COREUARTAPB0_BASE_ADDR,
                BAUD_VALUE_115200,
                (DATA_8_BITS | NO_PARITY));

    PHY10GKR_init(&g_phy, CORE10GBKR_0_PHY_BASE_ADDR);

    UART_polled_tx_string(&g_uart, "\n\r*** Core10GBaseKR PHY ***\n\r\n\r\0");

    while(1)
    {
        UART_polled_tx_string(&g_uart,
                "\n\rStarting Auto-Negotiation!!\n\r\0");
        while(1)
        {
            status = PHY10GKR_10gbasekr_sm(&g_phy);
            if(status != LINK_ESTABLISHED)
            {
                /* print error if reported */
                switch(status)
                {
                    case LT_SERDES_CAL_FAILURE:
                        UART_polled_tx_string(&g_uart,
                                "\n\rLT Serdes Config Failure!!\n\r\0");
                    break;

                    case LT_FAILURE:
                        UART_polled_tx_string(&g_uart,
                                "\n\rLT Training Failure!!\n\r\0");
                        #ifdef C10GBKR_PERFORMANCE_MESSAGES
                        lt_fail_display(&g_uart, &g_phy, msg);
                        #endif
                    break;

                    default:
                        /* do nothing no error status */
                    break;
                }
            }
            else
            {
                UART_polled_tx_string(&g_uart,
                        "\n\rLT Link Established!!\n\r\0");
                #ifdef C10GBKR_PERFORMANCE_MESSAGES
                lt_performances_display(&g_uart, &g_phy, msg);
                #endif
                break;
            }
        }

        /* Note: traffic may be sent through the link now. Embedded software
         * should check the link status to determine if the link partner has
         * disconnected.
         */

        while(1)
        {
            delay_ms(1000);

            status = PHY10GKR_10gbasekr_sm(&g_phy);
            if(status != LINK_ESTABLISHED)
            {
                UART_polled_tx_string(&g_uart, "\n\rLink: Broken!!\n\r\0");
                break;
            }
            else
            {
                if(good_link_cnt >= 10)
                {
                    UART_polled_tx_string(&g_uart, "\n\rLink: Good!!\n\r\0");
                    good_link_cnt = 0;
                }
                else
                {
                    good_link_cnt++;
                }
            }
        }
    }

    return 0u;
}

/*-----------------------------------------------------------------------------
 * Application delay in milli-seconds
 */
void
delay_ms
(
    uint32_t timeout_ms
)
{
    uint64_t end_time = 0;
    uint64_t start_time = CURENT_MTIME_MS;

    while(end_time < timeout_ms)
    {
        end_time = CURENT_MTIME_MS - start_time;
    }
}

#ifdef C10GBKR_PERFORMANCE_MESSAGES
void
calibration_error_display
(
    xcvr_c10gb_instance_t * xcvr,
    UART_instance_t * uart,
    phy10gkr_instance_t * phy,
    char * perf_msg
)
{
    sprintf(perf_msg,"\n\n\rRx Cal Error:\n\r\0");
    UART_polled_tx_string(&g_uart, (const uint8_t *)perf_msg);

    if((DATA_CLCK_UNLOCKED == xcvr->status.cdr_flock) ||
            (DATA_CLCK_UNLOCKED == xcvr->status.cdr_lock))
    {
        sprintf(perf_msg,"\tCDR Fail\n\r\0");
        UART_polled_tx_string(&g_uart, (const uint8_t *)perf_msg);
    }

    if(CTLE_CAL_COMPLETE != xcvr->status.ctle_cal)
    {
        sprintf(perf_msg,"\tCTLE Fail\n\r\0");
        UART_polled_tx_string(&g_uart, (const uint8_t *)perf_msg);
    }
}

void
lt_fail_display
(
    UART_instance_t * uart,
    phy10gkr_instance_t * phy,
    char * perf_msg
)
{
    sprintf(perf_msg,"tx equ count: %d\n\r\0",
            phy->lt.tx_equ_cnt);
    UART_polled_tx_string(uart, (const uint8_t *)perf_msg);

    sprintf(perf_msg,"tx equ count: %d\n\r\0",
            phy->lt.rx_cal_cnt);
    UART_polled_tx_string(uart, (const uint8_t *)perf_msg);
}

void
lt_iteration_display
(
    UART_instance_t * uart,
    phy10gkr_instance_t * phy,
    char * perf_msg
)
{
    /* Iteration counters */
    UART_polled_tx_string(&g_uart, "Training Counters:\n\r\0");
    sprintf(perf_msg,"\tTotal: %d\n\r\0",
            g_phy.lt.complete_cnt + g_phy.lt.fail_cnt);
    UART_polled_tx_string(uart, (const uint8_t *)perf_msg);
    sprintf(perf_msg,"\tFails: %d\n\r\0", g_phy.lt.fail_cnt);
    UART_polled_tx_string(uart, (const uint8_t *)perf_msg);
    sprintf(perf_msg,"\tCompletions: %d\n\r\0", g_phy.lt.complete_cnt);
    UART_polled_tx_string(uart, (const uint8_t *)perf_msg);
}

void
lt_performances_display
(
    UART_instance_t * uart,
    phy10gkr_instance_t * phy,
    char * perf_msg
)
{
    sprintf(perf_msg,"\n\n\rTraining Complete\n\n\r\0");
    UART_polled_tx_string(&g_uart, (const uint8_t *)perf_msg);

    lt_iteration_display(uart, phy, perf_msg);

    /* Complete time */
    sprintf(perf_msg,"\n\rCompletion Time: %d ms\n\r\0",
            phy->lt.timer.end);
    UART_polled_tx_string(uart, (const uint8_t *)perf_msg);

    /* Tx tap settings */
    sprintf(perf_msg,"\n\rTx Tap Settings\n\r\0");
    UART_polled_tx_string(uart, (const uint8_t *)perf_msg);

    sprintf(perf_msg,"\tC(0) Tap: %d \n\r\0",
            HAL_get_32bit_reg_field(g_phy.lt_base_addr,
            C10GB_LT_TX_NEW_MAIN_TAP));
    UART_polled_tx_string(uart, (const uint8_t *)perf_msg);

    sprintf(perf_msg,"\tC(1) Tap: %d \n\r\0",
            (int)HAL_get_32bit_reg_field(g_phy.lt_base_addr,
            C10GB_LT_TX_NEW_POST_TAP) -
            C10GBKR_LT_POST_TAP_MAX_LIMIT);
    UART_polled_tx_string(uart, (const uint8_t *)perf_msg);

    sprintf(perf_msg,"\tC(-1) Tap: %d \n\r\0",
            (int)HAL_get_32bit_reg_field(g_phy.lt_base_addr,
            C10GB_LT_TX_NEW_PRE_TAP) -
            C10GBKR_LT_PRE_TAP_MAX_LIMIT);
    UART_polled_tx_string(uart, (const uint8_t *)perf_msg);

}

#ifdef C10GBKR_AN_STATE_DISPLAY
void
an_state_display
(
    UART_instance_t * uart,
    phy10gkr_instance_t * phy,
    char * perf_msg
)
{
    switch(phy->an.status)
    {
        case ST_AUTO_NEG_ENABLE:
            sprintf(perf_msg,"ST_AUTO_NEG_ENABLE\n\r\0");
        break;

        case ST_TRANSMIT_DISABLE:
            sprintf(perf_msg,"ST_TRANSMIT_DISABLE\n\r\0");
        break;

        case ST_ABILITY_DETECT:
            sprintf(perf_msg,"ST_ABILITY_DETECT\n\r\0");
        break;

        case ST_ACKNOWLEDGE_DETECT:
            sprintf(perf_msg,"ST_ACKNOWLEDGE_DETECT\n\r\0");
        break;

        case ST_COMPLETE_ACKNOWLEDEGE:
            sprintf(perf_msg,"ST_COMPLETE_ACKNOWLEDEGE\n\r\0");
        break;

        case ST_AN_GOOD_CHECK:
            sprintf(perf_msg,"ST_AN_GOOD_CHECK\n\r\0");
        break;

        case ST_AN_GOOD:
            sprintf(perf_msg,"ST_AN_GOOD\n\r\0");
        break;

        case ST_NEXT_PAGE_WAIT:
            sprintf(perf_msg,"ST_NEXT_PAGE_WAIT\n\r\0");
        break;

        case ST_NEXT_PAGE_WAIT_TX_IDLE:
            sprintf(perf_msg,"ST_NEXT_PAGE_WAIT_TX_IDLE\n\r\0");
        break;

        case ST_LINK_STATUS_CHECK:
            sprintf(perf_msg,"ST_LINK_STATUS_CHECK\n\r\0");
        break;

        case ST_PARALLEL_DETECTION_FAULT:
            sprintf(perf_msg,"ST_PARALLEL_DETECTION_FAULT\n\r\0");
        break;
    }

    UART_polled_tx_string(uart, (const uint8_t *)perf_msg);
}
#endif /* C10GBKR_AN_STATE_DISPLAY */

#endif /* C10GBKR_PERFORMANCE_MESSAGES */

