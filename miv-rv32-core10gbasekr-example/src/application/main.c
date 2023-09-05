/**
 * Copyright 2022 Microchip FPGA Embedded Systems Solutions.
 *
 * SPDX-License-Identifier: MIT
 *
 * @file main.c
 * @author Microchip FPGA Embedded Systems Solutions
 * @brief PolarFire Core10GBaseKR PHY Sample Application
 *
 */

#include <stdio.h>
#include "hal/hal.h"
#include "miv_rv32_hal/miv_rv32_hal.h"
#include "drivers/fpga_ip/CoreUARTapb/core_uart_apb.h"
#include "drivers/fpga_ip/Core10GBaseKR_PHY/core10gbasekr_phy.h"
#include "PF_XCVR_C10GB/pf_xcvr_c10gb.h"
#include "polarfire_core10gbasekr_phy_cfg.h"

/**
 * Define C10GBKR_10GMAC_PACKET_GEN_EXAMPLE to enable the Core10GMAC and the
 * packet generator client.
 */
#define C10GBKR_10GMAC_PACKET_GEN_EXAMPLE
#ifdef C10GBKR_10GMAC_PACKET_GEN_EXAMPLE
#define MAC_BASE_ADDR                         (0x14000000U)
#define PATTERN_GEN_BASE                      (0x16000000U)
#define PATTERN_MON_BASE                      (0x40000000U)
#define DRI_BASE_ADDR                         (0x20000000U)

/** Core10GMAC Register addresses */
#define CORE10GMAC_TX_CONFIG_CONFIG_REG       (MAC_BASE_ADDR + 0x28CU)
#define CORE10GMAC_TX_CONFIG_SYS_1_REG        (MAC_BASE_ADDR + 0x290U)
#define CORE10GMAC_RX_CONFIG_STATIC_REG       (MAC_BASE_ADDR + 0x2C0U)
#define CORE10GMAC_RX_CONFIG_CONFIG_REG       (MAC_BASE_ADDR + 0x2CCU)
/** Core10GMAC Register configurations */
#define CORE10GMAC_TX_CONFIG_CONFIG_REG_VALUE (0x1C000U)
#define CORE10GMAC_TX_CONFIG_SYS_1_REG_VALUE  (0x10CU)
#define CORE10GMAC_RX_CONFIG_STATIC_REG_VALUE (0x8U)
#define CORE10GMAC_RX_CONFIG_CONFIG_REG_VALUE 0x1C000

/** Packet Generator Register Addresses */
#define PATTERN_GEN_PACKET_GAP_REG            (PATTERN_GEN_BASE + 0X24U)
#define PATTERN_GEN_PACKET_LEN_REG            (PATTERN_GEN_BASE + 0x10U)
#define PATTERN_GEN_NO_PKTS_REG               (PATTERN_GEN_BASE + 0x0CU)
#define PATTERN_GEN_START_PKT_TXN_REG         (PATTERN_GEN_BASE + 0x0U)
#define PATTERN_GEN_STOP_PKT_TXN_REG          (PATTERN_GEN_BASE + 0x04U)
#define PATTERN_GEN_CLR_PKT_CNT_REG           (PATTERN_GEN_BASE + 0x50U)
#define PATTERN_GEN_BASE_TOTAL_PKTS_0_REG     (PATTERN_GEN_BASE + 0x28U)
#define PATTERN_GEN_BASE_TOTAL_PKTS_1_REG     (PATTERN_GEN_BASE + 0x2CU)
#define PATTERN_GEN_BASE_GOOD_PKTS_0_REG      (PATTERN_GEN_BASE + 0x54U)
#define PATTERN_GEN_BASE_GOOD_PKTS_1_REG      (PATTERN_GEN_BASE + 0x58U)
#define PATTERN_GEN_BASE_BAD_PKTS_0_REG       (PATTERN_GEN_BASE + 0x5CU)
#define PATTERN_GEN_BASE_BAD_PKTS_1_REG       (PATTERN_GEN_BASE + 0x60U)
/** Packet Generator Register configurations */
#define PATTERN_GEN_PACKET_GAP_REG_VALUE      (0xFU)
#define PATTERN_GEN_PACKET_LEN_REG_VALUE      (0x100U)
#define PATTERN_GEN_NO_PKTS_REG_VALUE         (0x0U)
#define PATTERN_GEN_START_PKT_TXN_REG_VALUE   (0x01)
#define PATTERN_GEN_STOP_PKT_TXN_REG_VALUE    (0x0U)

/**
 * Configure the Core10GMAC to send data when training is complete
 */
void core10gmac_config(void);

/**
 * Configure the packet generator client to send data when training is complete
 */
void packet_generator_config(void);

/**
 * Display packet data performance on the UART terminal
 */
void packet_data_performance(UART_instance_t *uart, char *msg);
#endif /** C10GBKR_10GMAC_PACKET_GEN_EXAMPLE */

/**
 * Define C10GBKR_PERFORMANCE_MESSAGES to print performance messages after
 * link training failure and completion.
 */
#undef C10GBKR_PERFORMANCE_MESSAGES
#ifdef C10GBKR_PERFORMANCE_MESSAGES

#undef C10GBKR_AN_STATE_DISPLAY

/**
 * function prototypes
 */
void lt_performances_display(UART_instance_t *uart, phy10gkr_instance_t *phy, char *perf_msg);

/**
 * print auto-negotiation state to uart
 */
void lt_fail_display(UART_instance_t *uart, phy10gkr_instance_t *phy, char *perf_msg);

/**
 * print receiver calibration errors to uart
 */
void calibration_error_display(pf_xcvr_c10gb_instance_t *xcvr,
                               UART_instance_t *uart,
                               phy10gkr_instance_t *phy,
                               char *perf_msg);

#ifdef C10GBKR_AN_STATE_DISPLAY
/**
 * print auto-negotiation state to uart
 */
void an_state_display(UART_instance_t *uart, phy10gkr_instance_t *phy, char *perf_msg);
#endif /** C10GBKR_AN_STATE_DISPLAY */

#endif /** C10GBKR_PERFORMANCE_MESSAGES */

#if defined(C10GBKR_PERFORMANCE_MESSAGES) || defined(C10GBKR_10GMAC_PACKET_GEN_EXAMPLE)
char msg[256];
#endif

/**
 * Mtime definition
 */
#define CURRENT_MTIME_MS            ((MRV_read_mtime() / (SYS_CLK_FREQ / (MTIME_PRESCALER * 1.0))) * 1000)

/**
 * This definition will determine when an Auto-negotiation in progress message will
 * be displayed
 */
#define AN_PROGRESS_DISPLAY_TIMEOUT (3000u)

/**
 * Display good link message when count reaches this value
 */
#define GOOD_LINK_TOTAL_COUNT       (10u)

/**
 * UART instance data.
 */
UART_instance_t g_uart;

/**
 * Core10GBaseKR_PHY instance data.
 */
phy10gkr_instance_t g_phy;

/**
 * PolarFire XCVR instance data.
 */
pf_xcvr_c10gb_instance_t g_xcvr;

/**
 * Override design specific driver APIs
 *
 * Note: called from Core10GBaseKR_PHY Driver in this application
 */
uint32_t
PHY10GKR_get_current_time_ms(void)
{
    return (uint32_t)CURRENT_MTIME_MS;
}

/**
 * Application delay in milli-seconds using MTIME
 */
static void
delay_ms(uint32_t timeout_ms)
{
    uint64_t end_time = 0;
    uint64_t start_time = CURRENT_MTIME_MS;

    while (end_time < timeout_ms)
    {
        end_time = CURRENT_MTIME_MS - start_time;
    }
}

/**
 * Display IP and driver versions
 */
static void
display_versions(phy10gkr_instance_t phy, UART_instance_t uart)
{
    char version_msg[20];
    uint32_t ip_version_major;
    uint32_t ip_version_minor;
    uint32_t ip_version_sub;

    uint32_t driver_version_major;
    uint32_t driver_version_minor;
    uint32_t driver_version_patch;

    PHY10GKR_get_ip_version(&phy, &ip_version_major, &ip_version_minor, &ip_version_sub);

    PHY10GKR_get_driver_version(&driver_version_major,
                                &driver_version_minor,
                                &driver_version_patch);

    UART_polled_tx_string(&g_uart, "IP Version: ");
    sprintf(version_msg, "%d.%d.%d\n\r\0", ip_version_major, ip_version_minor, ip_version_sub);
    UART_polled_tx_string(&g_uart, version_msg);

    UART_polled_tx_string(&g_uart, "Driver Version: ");
    sprintf(version_msg,
            "%d.%d.%03d\n\r\0",
            driver_version_major,
            driver_version_minor,
            driver_version_patch);
    UART_polled_tx_string(&g_uart, version_msg);
}

/**
 * main() function.
 */
int
main(void)
{
    /** PHY local variables */
    phy10gkr_cfg_t phy_cfg;
    phy10gkr_status_t phy_status;
    phy10gkr_error_t phy_init_error;

    /** Application variables */
    uint32_t good_link_cnt = 0;
    uint32_t an_start_time = 0;

    UART_init(&g_uart, COREUARTAPB0_BASE_ADDR, BAUD_VALUE_115200, (DATA_8_BITS | NO_PARITY));

    UART_polled_tx_string(&g_uart, "\n\r*** Core10GBaseKR PHY ***\n\r\n\r\0");

    /** PHY configurations */
    PHY10GKR_cfg_struct_def_init(&phy_cfg);
    phy_cfg.xcvr_api = polarfire_xcvr_api_0; /** Add XCVR API pointers */
    phy_cfg.fec_request = C10GBKR_ENABLE_FEC_REQUEST;

    /** XCVR hardware configuration */
    g_xcvr.hw_cfg = &polarfire_xcvr_cfg_0;

    phy_init_error = PHY10GKR_init(&g_phy, CORE10GBKR_0_PHY_BASE_ADDR, &phy_cfg, &g_xcvr);

    display_versions(g_phy, g_uart);

    if (phy_init_error == 0)
    {
        while (1)
        {
            if (g_phy.fec_configured)
            {
                UART_polled_tx_string(&g_uart, "\n\rFEC is configured in IP\n\r\0");
                if (g_phy.fec_request)
                {
                    UART_polled_tx_string(&g_uart, "\n\rFEC is being requested\n\r\0");
                }
            }
            else
            {
                UART_polled_tx_string(&g_uart, "\n\rFEC is not configured in IP\n\r\0");
            }
            UART_polled_tx_string(&g_uart, "\n\rStarting Auto-Negotiation!!\n\r\0");
            an_start_time = CURRENT_MTIME_MS;
            while (1)
            {
                phy_status = PHY10GKR_10gbasekr_sm(&g_phy);
                if (phy_status != LINK_ESTABLISHED)
                {
                    /** print error if reported */
                    switch (phy_status)
                    {
                        case AN_IN_PROGRESS:
                            if ((CURRENT_MTIME_MS - an_start_time) >= AN_PROGRESS_DISPLAY_TIMEOUT)
                            {
                                UART_polled_tx_string(&g_uart,
                                                      "\n\rAuto-negotiation in progress\n\r\0");
                                an_start_time = CURRENT_MTIME_MS;
                            }
                            break;

                        case LT_SERDES_CAL_FAILURE:
                            UART_polled_tx_string(&g_uart, "\n\rLT SerDes Config Failure!!\n\r\0");
                            break;

                        case LT_FAILURE:
                            UART_polled_tx_string(&g_uart, "\n\rLT Training Failure!!\n\r\0");
#ifdef C10GBKR_PERFORMANCE_MESSAGES
                            lt_fail_display(&g_uart, &g_phy, msg);
#endif /** C10GBKR_PERFORMANCE_MESSAGES */
                            break;

                        default:
                            /** do nothing no error status */
                            break;
                    }
                }
                else
                {
                    UART_polled_tx_string(&g_uart, "\n\rLT Link Established!!\n\r\0");
                    if (g_phy.fec_configured)
                    {
                        if (g_phy.fec_negotiated)
                        {
                            UART_polled_tx_string(&g_uart,
                                                  "\n\rFEC negotiated with the link partner\n\r\0");
                        }
                        else
                        {
                            UART_polled_tx_string(
                                &g_uart,
                                "\n\rFEC not negotiated with the link partner\n\r\0");
                        }
                    }
#ifdef C10GBKR_PERFORMANCE_MESSAGES
                    lt_performances_display(&g_uart, &g_phy, msg);
#endif /** C10GBKR_PERFORMANCE_MESSAGES */
                    break;
                }
            }

#ifdef C10GBKR_10GMAC_PACKET_GEN_EXAMPLE
            core10gmac_config();
            /** Allow the MAC to initialize before sending data */
            delay_ms(4000);
            packet_generator_config();
#else
            /** Note: traffic may be sent through the link now. Embedded software
             * should check the link status to determine if the link partner has
             * disconnected.
             */
#endif /** C10GBKR_10GMAC_PACKET_GEN_EXAMPLE */

            while (1)
            {
                delay_ms(1000);

                phy_status = PHY10GKR_10gbasekr_sm(&g_phy);
                if (phy_status != LINK_ESTABLISHED)
                {
                    UART_polled_tx_string(&g_uart, "\n\rLink: Broken!!\n\r\0");
                    break;
                }
                else
                {
                    if (good_link_cnt >= GOOD_LINK_TOTAL_COUNT)
                    {
                        UART_polled_tx_string(&g_uart, "\n\rLink: Good!!\n\r\0");
                        good_link_cnt = 0;
#ifdef C10GBKR_10GMAC_PACKET_GEN_EXAMPLE
                        packet_data_performance(&g_uart, msg);
#endif /** C10GBKR_10GMAC_PACKET_GEN_EXAMPLE */
                    }
                    else
                    {
                        good_link_cnt++;
                    }
                }
            }
        }
    }
    else
    {
        UART_polled_tx_string(&g_uart, "\n\rERROR: ");
        switch (phy_init_error)
        {
            case PHY10GKR_ERR_USER_CONFIG:
                UART_polled_tx_string(&g_uart, "PHY Configuration\n\r\0");
                break;

            case PHY10GKR_ERR_NO_XCVR:
                UART_polled_tx_string(&g_uart, "No XCVR Configured\n\r\0");
                break;

            case PHY10GKR_ERR_XCVR_API_FUNCTION_POINTER:
                UART_polled_tx_string(&g_uart, "XCVR API Pointers\n\r\0");
                break;

            default:
                UART_polled_tx_string(
                    &g_uart,
                    "Unknown, please check hardware and software configurations\n\r\0");
                break;
        }

        while (1)
        {
            /** Release build configuration error, waiting here */
        }
    }
    return 0U;
}

#ifdef C10GBKR_10GMAC_PACKET_GEN_EXAMPLE
void
core10gmac_config(void)
{
    /** Enable MAC TX core to send frames onto the line and disable max length check */
    HW_set_32bit_reg(CORE10GMAC_TX_CONFIG_CONFIG_REG, CORE10GMAC_TX_CONFIG_CONFIG_REG_VALUE);

    /** Enable FCS insert */
    HW_set_32bit_reg(CORE10GMAC_TX_CONFIG_SYS_1_REG, CORE10GMAC_TX_CONFIG_SYS_1_REG_VALUE);

    /** Strip FCS */
    HW_set_32bit_reg(CORE10GMAC_RX_CONFIG_STATIC_REG, CORE10GMAC_RX_CONFIG_STATIC_REG_VALUE);

    /** Enable reception of data from PCS layer and disable max length check */
    HW_set_32bit_reg(CORE10GMAC_RX_CONFIG_CONFIG_REG, CORE10GMAC_RX_CONFIG_CONFIG_REG_VALUE);
}

void
packet_generator_config(void)
{
    /** Packet to packet gap */
    HW_set_32bit_reg(PATTERN_GEN_PACKET_GAP_REG, PATTERN_GEN_PACKET_GAP_REG_VALUE);

    /** Set the Packet Length */
    HW_set_32bit_reg(PATTERN_GEN_PACKET_LEN_REG, PATTERN_GEN_PACKET_LEN_REG_VALUE);

    /** Set Number of packets to be sent ..set 0 to send continuously */
    HW_set_32bit_reg(PATTERN_GEN_NO_PKTS_REG, PATTERN_GEN_NO_PKTS_REG_VALUE);

    /** Start to send packets */
    HW_set_32bit_reg(PATTERN_GEN_START_PKT_TXN_REG, PATTERN_GEN_START_PKT_TXN_REG_VALUE);

    /** Stop to send packets */
    HW_set_32bit_reg(PATTERN_GEN_STOP_PKT_TXN_REG, PATTERN_GEN_STOP_PKT_TXN_REG_VALUE);
}

void
packet_data_performance(UART_instance_t *uart, char *msg)
{
    uint32_t good_packets[2];
    uint32_t bad_packets[2];
    uint32_t total_packets[2];

    total_packets[0] = HW_get_32bit_reg(PATTERN_GEN_BASE_TOTAL_PKTS_0_REG);
    total_packets[1] = HW_get_32bit_reg(PATTERN_GEN_BASE_TOTAL_PKTS_1_REG);
    good_packets[0] = HW_get_32bit_reg(PATTERN_GEN_BASE_GOOD_PKTS_0_REG);
    good_packets[1] = HW_get_32bit_reg(PATTERN_GEN_BASE_GOOD_PKTS_1_REG);
    bad_packets[0] = HW_get_32bit_reg(PATTERN_GEN_BASE_BAD_PKTS_0_REG);
    bad_packets[1] = HW_get_32bit_reg(PATTERN_GEN_BASE_BAD_PKTS_1_REG);

    UART_polled_tx_string(uart, "\n\rPacket testing:\n\r\0");

    sprintf(msg, "\tTotal packets (msb): %u\n\r\0", (uint32_t)total_packets[0]);
    UART_polled_tx_string(uart, (const uint8_t *)msg);

    sprintf(msg, "\tTotal packets (lsb): %u\n\r\0", (uint32_t)total_packets[1]);
    UART_polled_tx_string(uart, (const uint8_t *)msg);

    sprintf(msg, "\tGood packets (msb): %u\n\r\0", (uint32_t)good_packets[0]);
    UART_polled_tx_string(uart, (const uint8_t *)msg);

    sprintf(msg, "\tGood packets (lsb): %u\n\r\0", (uint32_t)good_packets[1]);
    UART_polled_tx_string(uart, (const uint8_t *)msg);

    sprintf(msg, "\tBad packets (msb): %u\n\r\0", (uint32_t)bad_packets[0]);
    UART_polled_tx_string(uart, (const uint8_t *)msg);

    sprintf(msg, "\tBad packets (lsb): %u\n\r\0", (uint32_t)bad_packets[1]);
    UART_polled_tx_string(uart, (const uint8_t *)msg);
}
#endif /** C10GBKR_10GMAC_PACKET_GEN_EXAMPLE */

#ifdef C10GBKR_PERFORMANCE_MESSAGES
void
calibration_error_display(pf_xcvr_c10gb_instance_t *xcvr,
                          UART_instance_t *uart,
                          phy10gkr_instance_t *phy,
                          char *perf_msg)
{
    sprintf(perf_msg, "\n\n\rRx Cal Error:\n\r\0");
    UART_polled_tx_string(&g_uart, (const uint8_t *)perf_msg);

    if ((DATA_CLCK_UNLOCKED == xcvr->status.cdr_flock) ||
        (DATA_CLCK_UNLOCKED == xcvr->status.cdr_lock))
    {
        sprintf(perf_msg, "\tCDR Fail\n\r\0");
        UART_polled_tx_string(&g_uart, (const uint8_t *)perf_msg);
    }

    if (CTLE_CAL_COMPLETE != xcvr->status.ctle_cal)
    {
        sprintf(perf_msg, "\tCTLE Fail\n\r\0");
        UART_polled_tx_string(&g_uart, (const uint8_t *)perf_msg);
    }
}

void
lt_fail_display(UART_instance_t *uart, phy10gkr_instance_t *phy, char *perf_msg)
{
    sprintf(perf_msg, "tx equ count: %d\n\r\0", phy->lt.tx_equ_cnt);
    UART_polled_tx_string(uart, (const uint8_t *)perf_msg);

    sprintf(perf_msg, "tx equ count: %d\n\r\0", phy->lt.rx_cal_cnt);
    UART_polled_tx_string(uart, (const uint8_t *)perf_msg);
}

void
lt_iteration_display(UART_instance_t *uart, phy10gkr_instance_t *phy, char *perf_msg)
{
    /** Iteration counters */
    UART_polled_tx_string(&g_uart, "Training Counters:\n\r\0");
    sprintf(perf_msg, "\tTotal: %d\n\r\0", g_phy.lt.complete_cnt + g_phy.lt.fail_cnt);
    UART_polled_tx_string(uart, (const uint8_t *)perf_msg);
    sprintf(perf_msg, "\tFails: %d\n\r\0", g_phy.lt.fail_cnt);
    UART_polled_tx_string(uart, (const uint8_t *)perf_msg);
    sprintf(perf_msg, "\tCompletions: %d\n\r\0", g_phy.lt.complete_cnt);
    UART_polled_tx_string(uart, (const uint8_t *)perf_msg);
}

void
lt_performances_display(UART_instance_t *uart, phy10gkr_instance_t *phy, char *perf_msg)
{
    sprintf(perf_msg, "\n\n\rTraining Complete\n\n\r\0");
    UART_polled_tx_string(&g_uart, (const uint8_t *)perf_msg);

    lt_iteration_display(uart, phy, perf_msg);

    /** Complete time */
    sprintf(perf_msg, "\n\rCompletion Time: %d ms\n\r\0", phy->lt.timer.end);
    UART_polled_tx_string(uart, (const uint8_t *)perf_msg);

    /** Tx tap settings */
    sprintf(perf_msg, "\n\rTx Tap Settings\n\r\0");
    UART_polled_tx_string(uart, (const uint8_t *)perf_msg);

    sprintf(perf_msg,
            "\tC(0) Tap: %d \n\r\0",
            HAL_get_32bit_reg_field(g_phy.lt_base_addr, C10GB_LT_TX_NEW_MAIN_TAP));
    UART_polled_tx_string(uart, (const uint8_t *)perf_msg);

    sprintf(perf_msg,
            "\tC(1) Tap: %d \n\r\0",
            (int)HAL_get_32bit_reg_field(g_phy.lt_base_addr, C10GB_LT_TX_NEW_POST_TAP) -
                C10GBKR_LT_POST_TAP_MAX_LIMIT);
    UART_polled_tx_string(uart, (const uint8_t *)perf_msg);

    sprintf(perf_msg,
            "\tC(-1) Tap: %d \n\r\0",
            (int)HAL_get_32bit_reg_field(g_phy.lt_base_addr, C10GB_LT_TX_NEW_PRE_TAP) -
                C10GBKR_LT_PRE_TAP_MAX_LIMIT);
    UART_polled_tx_string(uart, (const uint8_t *)perf_msg);
}

#ifdef C10GBKR_AN_STATE_DISPLAY
void
an_state_display(UART_instance_t *uart, phy10gkr_instance_t *phy, char *perf_msg)
{
    switch (phy->an.status)
    {
        case ST_AUTO_NEG_ENABLE:
            sprintf(perf_msg, "ST_AUTO_NEG_ENABLE\n\r\0");
            break;

        case ST_TRANSMIT_DISABLE:
            sprintf(perf_msg, "ST_TRANSMIT_DISABLE\n\r\0");
            break;

        case ST_ABILITY_DETECT:
            sprintf(perf_msg, "ST_ABILITY_DETECT\n\r\0");
            break;

        case ST_ACKNOWLEDGE_DETECT:
            sprintf(perf_msg, "ST_ACKNOWLEDGE_DETECT\n\r\0");
            break;

        case ST_COMPLETE_ACKNOWLEDEGE:
            sprintf(perf_msg, "ST_COMPLETE_ACKNOWLEDEGE\n\r\0");
            break;

        case ST_AN_GOOD_CHECK:
            sprintf(perf_msg, "ST_AN_GOOD_CHECK\n\r\0");
            break;

        case ST_AN_GOOD:
            sprintf(perf_msg, "ST_AN_GOOD\n\r\0");
            break;

        case ST_NEXT_PAGE_WAIT:
            sprintf(perf_msg, "ST_NEXT_PAGE_WAIT\n\r\0");
            break;

        case ST_NEXT_PAGE_WAIT_TX_IDLE:
            sprintf(perf_msg, "ST_NEXT_PAGE_WAIT_TX_IDLE\n\r\0");
            break;

        case ST_LINK_STATUS_CHECK:
            sprintf(perf_msg, "ST_LINK_STATUS_CHECK\n\r\0");
            break;

        case ST_PARALLEL_DETECTION_FAULT:
            sprintf(perf_msg, "ST_PARALLEL_DETECTION_FAULT\n\r\0");
            break;
    }

    UART_polled_tx_string(uart, (const uint8_t *)perf_msg);
}
#endif /** C10GBKR_AN_STATE_DISPLAY */

#endif /** C10GBKR_PERFORMANCE_MESSAGES */
