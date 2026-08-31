/*******************************************************************************
 * Copyright 2026 Microchip FPGA Embedded Systems Solutions.
 *
 * SPDX-License-Identifier: MIT
 *
 * @file coreqse_user_config.h
 * @author Microchip FPGA Embedded Systems Solutions
 * @brief CoreQSE user configuration.
 *
 * IMPORTANT
 * ---------
 * The macros in this file MUST be kept in step with the CoreQSE IP configurator
 * settings used to build the Libero design. Optional hardware is not present
 * unless it was enabled at generation time; reading a register belonging to an
 * absent block returns 0 and writing it has no effect.
 *
 * APPLIES TO BOTH THE 1 Gbps AND 2.5 Gbps DESIGNS
 * ------------------------------------------------
 * The two reference designs were generated with IDENTICAL CoreQSE IP settings.
 * A parameter by parameter comparison of
 *   Design\CoreQSE_2_5Gbps\component\work\COREQSE_C0\COREQSE_C0.cxf
 *   Design\CoreQSE_1Gbps\component\work\COREQSE_C0\COREQSE_C0.cxf
 * shows no functional difference; the only discrepancy is an OUTFORMAT entry,
 * which is a generation artefact.
 *
 * This file therefore needs NO change when switching between designs. Link rate
 * and topology are declared in board_config.h instead, because those genuinely
 * do differ.
 *
 * Verify against your own .cxf before use. If you regenerate either design with
 * different options, for example to enable the statistics counters, update the
 * macros below to match and be aware that the two designs may then diverge.
 */

#ifndef COREQSE_USER_CONFIG_H_
#define COREQSE_USER_CONFIG_H_

/*******************************************************************************
 * Link rate.
 *
 * CoreQSE has NO software speed-select register. The rate is fixed by the
 * transceiver configuration and the clock rates in the Libero design, so no
 * setting in this file affects it.
 *
 * The rate is therefore declared by the application in board_config.h, as
 * BOARD_LINK_SPEED_MBPS, alongside the base addresses and the topology flags
 * that DO differ between the two designs. Nothing in this file is rate
 * dependent.
 *
 * For reference, from the two reference designs:
 *
 *   2.5 Gbps   XCVR serial 3125 Mbps, div 2, XCVR_TX_CLK 156.25 MHz
 *              3125 x 16/20 = 2500 Mbps after CoreQSE 16b/20b encoding
 *
 *   1   Gbps   XCVR serial 1250 Mbps, div 4, XCVR_TX_CLK  62.5  MHz
 *              1250 x 16/20 = 1000 Mbps
 *
 * In both designs SYS_CLK is tied to LANE0_TX_CLK_R.
 */

/*******************************************************************************
 * Optional MAC configuration blocks.
 *
 * Set to 1 only if the corresponding IP configurator generic was enabled.
 * From COREQSE_C0.cxf of BOTH designs, ALL of these are false:
 *
 *   RX_MAC_CFG_FRMFIL_PKT   = false  -> frame filter registers absent
 *   RX_MAC_CFG_FRMFIL_HASH  = false  -> multicast hash table absent
 *   RX_MAC_CFG_PAUSE_EN     = 0      -> RX pause/flow control absent
 *   TX_MAC_CFG_PAUSE_EN     = 0      -> TX pause/flow control absent
 *   RX_MAC_CFG_PREAMBLE     = false  -> programmable RX preamble absent
 *   TX_MAC_CFG_PREAMBLE     = false  -> programmable TX preamble absent
 *   RX_MAC_CFG_WOL          = false  -> wake-on-LAN absent
 */
#define COREQSE_CFG_RX_FRMFIL_PKT           0u
#define COREQSE_CFG_RX_FRMFIL_HASH          0u
#define COREQSE_CFG_RX_PAUSE                0u
#define COREQSE_CFG_TX_PAUSE                0u
#define COREQSE_CFG_RX_PREAMBLE             0u
#define COREQSE_CFG_TX_PREAMBLE             0u
#define COREQSE_CFG_RX_WOL                  0u

/*******************************************************************************
 * Statistics counters.
 *
 * Every TX_MAC_STATS_*_CNT_EN, RX_MAC_STATS_*_CNT_EN and RX_PCS_STATS_*_CNT_EN
 * generic is false in BOTH designs, so the whole counter block
 * (0x200-0x50C) is NOT implemented and every counter reads 0.
 *
 * Set this to 1 after regenerating CoreQSE with the counters enabled. Until
 * then QSE_get_stats() returns QSE_NOT_SUPPORTED rather than silently
 * reporting all-zero counters as a healthy result.
 */
#define COREQSE_CFG_STATS                   0u

/*******************************************************************************
 * ECC on the MAC FIFOs and the PCS elastic buffer.
 * COREQSE_C0.cxf: ECC_ENABLE = false.
 */
#define COREQSE_CFG_ECC                     0u

/*******************************************************************************
 * Frame length limits used for parameter validation and defaults.
 *
 * COREQSE_FRAME_LEN_MIN is the padded minimum frame excluding FCS. The MAC pads
 * frames whose payload is shorter than 46 bytes, i.e. 14 header + 46 = 60.
 *
 * COREQSE_FRAME_LEN_MAX becomes MAC_TX_MAX_PKT_LEN / MAC_RX_MAX_PKT_LEN. A
 * valid frame is less than or equal to this value.
 *
 * Note that the reset value of both CFG registers puts 0xC000 in this field.
 * Per the user guide, any value whose bits [15:14] are both set disables the
 * maximum length check entirely, so CoreQSE comes out of reset with length
 * checking OFF. Programming 1518 here turns it on. Use
 * QSE_MAX_PKT_LEN_NO_CHECK if you want the reset behaviour back.
 */
#define COREQSE_FRAME_LEN_MIN               60u    /* without FCS            */
#define COREQSE_FRAME_LEN_MAX               1518u  /* standard, without VLAN */

#endif /* COREQSE_USER_CONFIG_H_ */
