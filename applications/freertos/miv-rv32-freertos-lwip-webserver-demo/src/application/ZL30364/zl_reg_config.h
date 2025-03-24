
/*******************************************************************************
 * Copyright 2025 Microchip FPGA Embedded Systems Solutions.
 *
 * SPDX-License-Identifier: MIT
 *
 * @file zl_reg_config.h
 * @author Microchip FPGA Embedded Systems Solutions
 * @brief ZL30362 configuration.
 * The values for this configuration were created using the ZL30362 GUI.
 *
 */

const uint8_t g_zl30362_config2[] = {
    0x1F, /* Ready indicator */
    0x40, /* Chip Revision */
    0x05, /* HW Revision =- note:- ML changed to five */
    0x00, /* Reserved */
    0x00, /* Reserved */
    0x09, /* Reserved note:- ML changed to 10 */
    0xF2, /* Reserved */
    0xFF, /* Customer identification [31:24] */
    0xFF, /* Customer identification [23:16] */
    0xFF, /* Customer identification [15:8] */
    0xFF, /* Customer identification [7:0] */
    0x04, /* Central Freq Offset [31:24] */
    0x6A, /* Central Freq Offset [23:16] */
    0xAA, /* Central Freq Offset [15:8] */
    0xAB, /* Central Freq Offset [7:0] */
    0x00, /* Reserved */
    0x00, /* Spurs Suppression */
    0x00, /* Sticky Lock Register */
    0x00, /* Reserved */
    0x00, /* Reserved */
    0x00, /* Reserved */
    0x00, /* Reserved */
    0x00, /* Reserved */
    0x00, /* Reserved */
    0x01, /* Reserved */
    0x80, /* GPIO at Startup [15:8] */
    0x03, /* GPIO at Startup [7:0] */
    0x00, /* Reserved */
    0x00, /* Reserved */
    0x00, /* Reserved */
    0x00, /* Reserved */
    0x00, /* Reserved */
    0x00, /* Reference Fail 7-0 */
    0x00, /* Reference Fail 10-8 */
    0x00, /* DPLL Status */
    0x00, /* Ref Fail Mask 7-0 */
    0x00, /* Ref Fail Mask 10-8 */
    0x00, /* DPLL Fail Mask */
    0x00, /* Reference Monitor Fail 0 */
    0x00, /* Reference Monitor Fail 1 */
    0x00, /* Reference Monitor Fail 2 */
    0x00, /* Reference Monitor Fail 3 */
    0x00, /* Reference Monitor Fail 4 */
    0x00, /* Reference Monitor Fail 5 */
    0x00, /* Reference Monitor Fail 6 */
    0x00, /* Reference Monitor Fail 7 */
    0x00, /* Reference Monitor Fail 8 */
    0x00, /* Reference Monitor Fail 9 */
    0x00, /* Reference Monitor Fail 10 */
    0x00, /* Reserved */
    0x00, /* Reserved */
    0x00, /* Reserved */
    0x00, /* Reserved */
    0x00, /* Reserved */
    0x16, /* Reference Monitor Mask 0 */
    0x16, /* Reference Monitor Mask 1 */
    0x16, /* Reference Monitor Mask 2 */
    0x16, /* Reference Monitor Mask 3 */
    0x16, /* Reference Monitor Mask 4 */
    0x16, /* Reference Monitor Mask 5 */
    0x16, /* Reference Monitor Mask 6 */
    0x16, /* Reference Monitor Mask 7 */
    0x16, /* Reference Monitor Mask 8 */
    0x16, /* Reference Monitor Mask 9 */
    0x16, /* Reference Monitor Mask 10 */
    0x00, /* Reserved */
    0x00, /* Reserved */
    0x00, /* Reserved */
    0x00, /* Reserved */
    0x00, /* Reserved */
    0xAA, /* GST Disqualify Time 3-0 */
    0xAA, /* GST Disqualify Time 7-4 */
    0x2A, /* GST Disqualify Time 10-8 */
    0x00, /* Reserved */
    0x55, /* GST Qualify Time 3-0 */
    0x55, /* GST Qualify Time 7-4 */
    0x15, /* GST Qualify Time 10-8 */
    0x00, /* Reserved */
    0x00, /* Reserved */
    0x00, /* Reserved */
    0x55, /* SCM/CFM Limit 0 */
    0x55, /* SCM/CFM Limit 1 */
    0x55, /* SCM/CFM Limit 2 */
    0x55, /* SCM/CFM Limit 3 */
    0x55, /* SCM/CFM Limit 4 */
    0x55, /* SCM/CFM Limit 5 */
    0x55, /* SCM/CFM Limit 6 */
    0x55, /* SCM/CFM Limit 7 */
    0x55, /* SCM/CFM Limit 8 */
    0x55, /* SCM/CFM Limit 9 */
    0x55, /* SCM/CFM Limit 10 */
    0x00, /* Reserved */
    0x00, /* Reserved */
    0x00, /* Reserved */
    0x00, /* Reserved */
    0x00, /* Reserved */
    0x33, /* PFM Limit 1-0 */
    0x33, /* PFM Limit 3-2 */
    0x33, /* PFM Limit 5-4 */
    0x33, /* PFM Limit 7-6 */
    0x33, /* PFM Limit 9-8 */
    0x03, /* PFM Limit 10 */
    0x00, /* Reserved */
    0x00, /* Reserved */
    0xFF, /* Phase Acquisition Enable 7-0 */
    0x07, /* Phase Acquisition Enable 10-8 */
    0x1B, /* Phase Memory Limit 0 */
    0x1B, /* Phase Memory Limit 1 */
    0x1B, /* Phase Memory Limit 2 */
    0x1B, /* Phase Memory Limit 3 */
    0xA0, /* Phase Memory Limit 4 */
    0x1B, /* Phase Memory Limit 5 */
    0xA0, /* Phase Memory Limit 6 */
    0x1B, /* Phase Memory Limit 7 */
    0x1B, /* Phase Memory Limit 8 */
    0x1B, /* Phase Memory Limit 9 */
    0x1B, /* Phase Memory Limit 10 */
    0x00, /* Reserved */
    0x00, /* Reserved */
    0x00, /* Reserved */
    0x00, /* Reserved */
    0x00, /* Reserved */
    0x00, /* Reference Config 7-0 */
    0x00, /* Reference Config 8 */
    0x00, /* Reference Pre-divide 7-0 */
    0x00, /* Reference Pre-divide 10-8 */
    0x00, /* Microport Status */
    0x01, /* Page Select */
    0x9C, /* Ref0 Base Freq Br [15:8] */
    0x40, /* Ref0 Base Freq Br [7:0] */
    0x00, /* Ref0 Frequency Multiple Kr [15:8] */
    0xFA, /* Ref0 Frequency Multiple Kr [7:0] */
    0x00, /* Ref0 Numerator Mr [15:8] */
    0x01, /* Ref0 Numerator Mr [7:0] */
    0x00, /* Ref0 Denominator Nr [15:8] */
    0x01, /* Ref0 Denominator Nr [7:0] */
    0x9C, /* Ref1 Base Freq Br [15:8] */
    0x40, /* Ref1 Base Freq Br [7:0] */
    0x00, /* Ref1 Frequency Multiple Kr [15:8] */
    0xFA, /* Ref1 Frequency Multiple Kr [7:0] */
    0x00, /* Ref1 Numerator Mr [15:8] */
    0x01, /* Ref1 Numerator Mr [7:0] */
    0x00, /* Ref1 Denominator Nr [15:8] */
    0x01, /* Ref1 Denominator Nr [7:0] */
    0x9C, /* Ref2 Base Freq Br [15:8] */
    0x40, /* Ref2 Base Freq Br [7:0] */
    0x00, /* Ref2 Frequency Multiple Kr [15:8] */
    0xFA, /* Ref2 Frequency Multiple Kr [7:0] */
    0x00, /* Ref2 Numerator Mr [15:8] */
    0x01, /* Ref2 Numerator Mr [7:0] */
    0x00, /* Ref2 Denominator Nr [15:8] */
    0x01, /* Ref2 Denominator Nr [7:0] */
    0x9C, /* Ref3 Base Freq Br [15:8] */
    0x40, /* Ref3 Base Freq Br [7:0] */
    0x00, /* Ref3 Frequency Multiple Kr [15:8] */
    0xFA, /* Ref3 Frequency Multiple Kr [7:0] */
    0x00, /* Ref3 Numerator Mr [15:8] */
    0x01, /* Ref3 Numerator Mr [7:0] */
    0x00, /* Ref3 Denominator Nr [15:8] */
    0x01, /* Ref3 Denominator Nr [7:0] */
    0x00, /* Ref4 Base Freq Br [15:8] */
    0x01, /* Ref4 Base Freq Br [7:0] */
    0x00, /* Ref4 Frequency Multiple Kr [15:8] */
    0x01, /* Ref4 Frequency Multiple Kr [7:0] */
    0x00, /* Ref4 Numerator Mr [15:8] */
    0x01, /* Ref4 Numerator Mr [7:0] */
    0x00, /* Ref4 Denominator Nr [15:8] */
    0x01, /* Ref4 Denominator Nr [7:0] */
    0x9C, /* Ref5 Base Freq Br [15:8] */
    0x40, /* Ref5 Base Freq Br [7:0] */
    0x01, /* Ref5 Frequency Multiple Kr [15:8] */
    0xE6, /* Ref5 Frequency Multiple Kr [7:0] */
    0x00, /* Ref5 Numerator Mr [15:8] */
    0x01, /* Ref5 Numerator Mr [7:0] */
    0x00, /* Ref5 Denominator Nr [15:8] */
    0x01, /* Ref5 Denominator Nr [7:0] */
    0x00, /* Ref6 Base Freq Br [15:8] */
    0x01, /* Ref6 Base Freq Br [7:0] */
    0x00, /* Ref6 Frequency Multiple Kr [15:8] */
    0x01, /* Ref6 Frequency Multiple Kr [7:0] */
    0x00, /* Ref6 Numerator Mr [15:8] */
    0x01, /* Ref6 Numerator Mr [7:0] */
    0x00, /* Ref6 Denominator Nr [15:8] */
    0x01, /* Ref6 Denominator Nr [7:0] */
    0x9C, /* Ref7 Base Freq Br [15:8] */
    0x40, /* Ref7 Base Freq Br [7:0] */
    0x02, /* Ref7 Frequency Multiple Kr [15:8] */
    0x71, /* Ref7 Frequency Multiple Kr [7:0] */
    0x00, /* Ref7 Numerator Mr [15:8] */
    0x01, /* Ref7 Numerator Mr [7:0] */
    0x00, /* Ref7 Denominator Nr [15:8] */
    0x01, /* Ref7 Denominator Nr [7:0] */
    0x9C, /* Ref8 Base Freq Br [15:8] */
    0x40, /* Ref8 Base Freq Br [7:0] */
    0x02, /* Ref8 Frequency Multiple Kr [15:8] */
    0x71, /* Ref8 Frequency Multiple Kr [7:0] */
    0x00, /* Ref8 Numerator Mr [15:8] */
    0x01, /* Ref8 Numerator Mr [7:0] */
    0x00, /* Ref8 Denominator Nr [15:8] */
    0x01, /* Ref8 Denominator Nr [7:0] */
    0x9C, /* Ref9 Base Freq Br [15:8] */
    0x40, /* Ref9 Base Freq Br [7:0] */
    0x0C, /* Ref9 Frequency Multiple Kr [15:8] */
    0x35, /* Ref9 Frequency Multiple Kr [7:0] */
    0x00, /* Ref9 Numerator Mr [15:8] */
    0x01, /* Ref9 Numerator Mr [7:0] */
    0x00, /* Ref9 Denominator Nr [15:8] */
    0x01, /* Ref9 Denominator Nr [7:0] */
    0x9C, /* Ref10 Base Freq Br [15:8] */
    0x40, /* Ref10 Base Freq Br [7:0] */
    0x0C, /* Ref10 Frequency Multiple Kr [15:8] */
    0x35, /* Ref10 Frequency Multiple Kr [7:0] */
    0x00, /* Ref10 Numerator Mr [15:8] */
    0x01, /* Ref10 Numerator Mr [7:0] */
    0x00, /* Ref10 Denominator Nr [15:8] */
    0x01, /* Ref10 Denominator Nr [7:0] */
    0x00, /* Ref0 Sync Ctrl */
    0x00, /* Ref1 Sync Ctrl */
    0x00, /* Ref2 Sync Ctrl */
    0x00, /* Ref3 Sync Ctrl */
    0x00, /* Ref4 Sync Ctrl */
    0x00, /* Ref5 Sync Ctrl */
    0x00, /* Ref6 Sync Ctrl */
    0x00, /* Ref7 Sync Ctrl */
    0x00, /* Ref8 Sync Ctrl */
    0x00, /* Ref9 Sync Ctrl */
    0x00, /* Ref10 Sync Ctrl */
    0x00, /* DPLL0 PSL Decay Time */
    0x00, /* DPLL1 PSL Decay Time */
    0x00, /* DPLL2 PSL Decay Time */
    0x00, /* Reserved */
    0x00, /* DPLL0 PSL Scaling */
    0x00, /* DPLL1 PSL Scaling */
    0x00, /* DPLL2 PSL Scaling */
    0x00, /* Reserved */
    0x00, /* Reserved */
    0x10, /* Clock Sync Pulse 1-0 */
    0x32, /* Clock Sync Pulse 3-2 */
    0x54, /* Clock Sync Pulse 5-4 */
    0x76, /* Clock Sync Pulse 7-6 */
    0x98, /* Clock Sync Pulse 9-8 */
    0x0A, /* Clock Sync Pulse 10 */
    0x00, /* Reserved */
    0x00, /* Reserved */
    0x00, /* Reserved */
    0x00, /* Reserved */
    0x00, /* Reserved */
    0x00, /* Reserved */
    0x00, /* Reserved */
    0x00, /* Reserved */
    0x00, /* Reserved */
    0x00, /* Reserved */
    0x00, /* Reserved */
    0x00, /* Reserved */
    0x00, /* Reserved */
    0x02, /* Page Select */
    0xE4, /* DPLL0 Control */
    0x82, /* DPLL0 Bandwidth Select */
    0x00, /* DPLL0 Pull-in/Hold-in Select */
    0x33, /* DPLL0 Mode/Reference Select */
    0x00, /* DPLL0 Reference Select Status */
    0xFF, /* DPLL0 Ref Priority 1-0 */
    0xFF, /* DPLL0 Ref Priority 3-2 */
    0xFF, /* DPLL0 Ref Priority 5-4 */
    0xFF, /* DPLL0 Ref Priority 7-6 */
    0xFF, /* DPLL0 Ref Priority 9-8 */
    0x0F, /* DPLL0 Ref Priority 10 */
    0x00, /* DPLL0 PSL Max Phase [15:8] */
    0x00, /* DPLL0 PSL Max Phase [7:0] */
    0x87, /* DPLL0 Ref Fail Mask */
    0x01, /* DPLL0 PFM Fail Mask */
    0x0B, /* DPLL0 Holdover Delay/Edge Sel */
    0x00, /* DPLL0 Phase Buildout Control */
    0x22, /* DPLL0 PBO Phase Error Threshold */
    0x70, /* DPLL0 PBO Min Phase Slope */
    0x20, /* DPLL0 PBO End Interval */
    0x64, /* DPLL0 PBO Timeout */
    0x00, /* DPLL0 PBO Counter */
    0x00, /* DPLL0 PBO Error Count [23:16] */
    0x00, /* DPLL0 PBO Error Count [15:8] */
    0x00, /* DPLL0 PBO Error Count [7:0] */
    0x05, /* DPLL0 Damping Factor Control */
    0x05, /* Reserved */
    0x00, /* Reserved */
    0x00, /* Reserved */
    0x00, /* Reserved */
    0x00, /* Reserved */
    0x00, /* Reserved */
    0xE4, /* DPLL1 Control */
    0x87, /* DPLL1 Bandwidth Select */
    0x00, /* DPLL1 Pull-in/Hold-in Select */
    0x33, /* DPLL1 Mode/Reference Select */
    0x00, /* DPLL1 Reference Select Status */
    0xFF, /* DPLL1 Ref Priority 1-0 */
    0xFF, /* DPLL1 Ref Priority 3-2 */
    0xFF, /* DPLL1 Ref Priority 5-4 */
    0xFF, /* DPLL1 Ref Priority 7-6 */
    0xFF, /* DPLL1 Ref Priority 9-8 */
    0x0F, /* DPLL1 Ref Priority 10 */
    0x00, /* DPLL1 PSL Max Phase [15:8] */
    0x00, /* DPLL1 PSL Max Phase [7:0] */
    0x87, /* DPLL1 Ref Fail Mask */
    0x01, /* DPLL1 PFM Fail Mask */
    0x0B, /* DPLL1 Holdover Delay/Edge Sel */
    0x00, /* DPLL1 Phase Buildout Control */
    0x22, /* DPLL1 PBO Jitter Threshold */
    0x70, /* DPLL1 PBO Min Phase Slope */
    0x20, /* DPLL1 PBO End Interval */
    0x64, /* DPLL1 PBO Timeout */
    0x00, /* DPLL1 PBO Counter */
    0x00, /* DPLL1 PBO Error Count [23:16] */
    0x00, /* DPLL1 PBO Error Count [15:8] */
    0x00, /* DPLL1 PBO Error Count [7:0] */
    0x05, /* DPLL1 Damping Factor Control */
    0x05, /* Reserved */
    0x00, /* Reserved */
    0x00, /* Reserved */
    0x00, /* Reserved */
    0x00, /* Reserved */
    0x00, /* Reserved */
    0x0C, /* DPLL2 Control */
    0x00, /* DPLL2 Bandwidth Select */
    0x00, /* DPLL2 Pull-in/Hold-in Select */
    0x33, /* DPLL2 Mode/Reference Select */
    0x00, /* DPLL2 Reference Select Status */
    0xFF, /* DPLL2 Ref Priority 1-0 */
    0x0F, /* DPLL2 Ref Priority 3-2 */
    0xFF, /* DPLL2 Ref Priority 5-4 */
    0xFF, /* DPLL2 Ref Priority 7-6 */
    0xFF, /* DPLL2 Ref Priority 9-8 */
    0x0F, /* DPLL2 Ref Priority 10 */
    0x00, /* DPLL2 PSL Max Phase [15:8] */
    0x00, /* DPLL2 PSL Max Phase [7:0] */
    0x87, /* DPLL2 Ref Fail Mask */
    0x01, /* DPLL2 PFM Fail Mask */
    0x0B, /* DPLL2 Holdover Delay/Edge Sel */
    0x00, /* DPLL2 Phase Buildout Control */
    0x22, /* DPLL2 PBO Jitter Threshold */
    0x70, /* DPLL2 PBO Min Phase Slope */
    0x20, /* DPLL2 PBO End Interval */
    0x64, /* DPLL2 PBO Timeout */
    0x00, /* DPLL2 PBO Counter */
    0x00, /* DPLL2 PBO Error Count [23:16] */
    0x00, /* DPLL2 PBO Error Count [15:8] */
    0x00, /* DPLL2 PBO Error Count [7:0] */
    0x05, /* DPLL2 Damping Factor Control */
    0x05, /* Reserved */
    0x00, /* Reserved */
    0x00, /* Reserved */
    0x00, /* Reserved */
    0x00, /* Reserved */
    0x00, /* Reserved */
    0x00, /* Reserved */
    0x00, /* Reserved */
    0x00, /* Reserved */
    0x00, /* Reserved */
    0x00, /* Reserved */
    0x00, /* Reserved */
    0x00, /* Reserved */
    0x00, /* Reserved */
    0x00, /* Reserved */
    0x00, /* Reserved */
    0x00, /* Reserved */
    0x00, /* Reserved */
    0x00, /* Reserved */
    0x00, /* Reserved */
    0x00, /* Reserved */
    0x00, /* Reserved */
    0x00, /* Reserved */
    0x00, /* Reserved */
    0x00, /* Reserved */
    0x00, /* Reserved */
    0x00, /* Reserved */
    0x00, /* Reserved */
    0x00, /* Reserved */
    0x00, /* Reserved */
    0x00, /* Reserved */
    0x00, /* Reserved */
    0x00, /* Reserved */
    0x00, /* Reserved */
    0x00, /* Reserved */
    0x00, /* Reserved */
    0x00, /* Reserved */
    0x03, /* Page Select */
    0x00, /* DPLL Hold/Lock Status */
    0x00, /* External Feedback Control */
    0x03, /* DPLL Configuration */
    0x00, /* DPLL Lock Selection */
    0x00, /* Reserved */
    0x00, /* Reserved */
    0x00, /* Reserved */
    0x00, /* Reserved */
    0x00, /* Reserved */
    0x00, /* Reserved */
    0x00, /* Reserved */
    0x00, /* Reserved */
    0x00, /* Reserved */
    0x00, /* DPLL0 Delta Freq Offset [39:32] */
    0x00, /* DPLL0 Delta Freq Offset [31:24] */
    0x00, /* DPLL0 Delta Freq Offset [23:16] */
    0x00, /* DPLL0 Delta Freq Offset [15:8] */
    0x00, /* DPLL0 Delta Freq Offset [7:0] */
    0x00, /* DPLL1 Delta Freq Offset [39:32] */
    0x00, /* DPLL1 Delta Freq Offset [31:24] */
    0x00, /* DPLL1 Delta Freq Offset [23:16] */
    0x00, /* DPLL1 Delta Freq Offset [15:8] */
    0x00, /* DPLL1 Delta Freq Offset [7:0] */
    0x00, /* DPLL2 Delta Freq Offset [39:32] */
    0x00, /* DPLL2 Delta Freq Offset [31:24] */
    0x00, /* DPLL2 Delta Freq Offset [23:16] */
    0x00, /* DPLL2 Delta Freq Offset [15:8] */
    0x00, /* DPLL2 Delta Freq Offset [7:0] */
    0x00, /* Reserved */
    0x00, /* Reserved */
    0x00, /* Reserved */
    0x00, /* Reserved */
    0x00, /* Reserved */
    0x00, /* Reserved */
    0x00, /* Reserved */
    0x00, /* Reserved */
    0x00, /* Reserved */
    0x00, /* Reserved */
    0x00, /* Reserved */
    0x00, /* Reserved */
    0x00, /* Reserved */
    0x14, /* Reserved */
    0x00, /* Reserved */
    0x00, /* Reserved */
    0x00, /* Reserved */
    0x00, /* Reserved */
    0x00, /* Reserved */
    0x00, /* Reserved */
    0x90, /* DPLL->Synth Drive Select */
    0x0F, /* Synthesizer Enable */
    0x00, /* Synthesizer Filter Select */
    0x00, /* Reserved */
    0x00, /* Reserved */
    0x00, /* Reserved */
    0x00, /* Synth fail Status */
    0x00, /* Synth Clear Fail Flag */
    0x61, /* Synth0 Base Frequency Bs [15:8] */
    0xA8, /* Synth0 Base Frequency Bs [7:0] */
    0x09, /* Synth0 Freq Multiple Ks [15:8] */
    0xC4, /* Synth0 Freq Multiple Ks [7:0] */
    0x00, /* Synth0 Numerator Ms [15:8] */
    0x01, /* Synth0 Numerator Ms [7:0] */
    0x00, /* Synth0 Denominator Ns [15:8] */
    0x01, /* Synth0 Denominator Ns [7:0] */
    0x61, /* Synth1 Base Frequency [15:8] */
    0xA8, /* Synth1 Base Frequency [7:0] */
    0x09, /* Synth1 Freq Multiple Ks [15:8] */
    0xC4, /* Synth1 Freq Multiple Ks [7:0] */
    0x00, /* Synth1 Numerator Ms [15:8] */
    0x01, /* Synth1 Numerator Ms [7:0] */
    0x00, /* Synth1 Denominator Ns [15:8] */
    0x01, /* Synth1 Denominator Ns [7:0] */
    0x61, /* Synth2 Base Frequency [15:8] */
    0xA8, /* Synth2 Base Frequency [7:0] */
    0x09, /* Synth2 Freq Multiple Ks [15:8] */
    0xC4, /* Synth2 Freq Multiple Ks [7:0] */
    0x00, /* Synth2 Numerator Ms [15:8] */
    0x01, /* Synth2 Numerator Ms [7:0] */
    0x00, /* Synth2 Denominator Ns [15:8] */
    0x01, /* Synth2 Denominator Ns [7:0] */
    0x61, /* Synth3 Base Frequency [15:8] */
    0xA8, /* Synth3 Base Frequency [7:0] */
    0x09, /* Synth3 Freq Multiple Ks [15:8] */
    0xC4, /* Synth3 Freq Multiple Ks [7:0] */
    0x00, /* Synth3 Numerator Ms [15:8] */
    0x01, /* Synth3 Numerator Ms [7:0] */
    0x00, /* Synth3 Denominator Ns [15:8] */
    0x01, /* Synth3 Denominator Ns [7:0] */
    0x00, /* Reserved */
    0x00, /* Reserved */
    0x00, /* Reserved */
    0x00, /* Reserved */
    0x00, /* Reserved */
    0x00, /* Reserved */
    0x00, /* Reserved */
    0x00, /* Reserved */
    0x00, /* Reserved */
    0x00, /* Reserved */
    0x00, /* Reserved */
    0x00, /* Reserved */
    0x00, /* Reserved */
    0x00, /* Reserved */
    0x00, /* Reserved */
    0x00, /* Reserved */
    0x00, /* Reserved */
    0x00, /* Reserved */
    0x00, /* Reserved */
    0x00, /* Reserved */
    0x00, /* Reserved */
    0x00, /* Reserved */
    0x00, /* Reserved */
    0x00, /* Reserved */
    0x00, /* Reserved */
    0x00, /* Reserved */
    0x00, /* Reserved */
    0x00, /* Reserved */
    0x00, /* Reserved */
    0x00, /* Reserved */
    0x00, /* Reserved */
    0x00, /* Reserved */
    0x00, /* Reserved */
    0x00, /* Reserved */
    0x00, /* Reserved */
    0x00, /* Reserved */
    0x00, /* Reserved */
    0x00, /* Reserved */
    0x00, /* Reserved */
    0x04, /* Page Select */
    0x00, /* Synth 0A Post Divide [23:16] */
    0x00, /* Synth 0A Post Divide [15:8] */
    0x04, /* Synth 0A Post Divide [7:0] */
    0x00, /* Synth 0B Post Divide [23:16] */
    0x00, /* Synth 0B Post Divide [15:8] */
    0x08, /* Synth 0B Post Divide [7:0]8=>125Mhz fabric clock */
    0xF2, /* Synth 0C Post Divide [23:16] */
    0xC3, /* Synth 0C Post Divide [15:8] */
    0x50, /* Synth 0C Post Divide [7:0] */
    0xF3, /* Synth 0D Post Divide [23:16] */
    0xC3, /* Synth 0D Post Divide [15:8] */
    0x50, /* Synth 0D Post Divide [7:0] */
    0x00, /* Synth 1A Post Divide [23:16] */
    0x00, /* Synth 1A Post Divide [15:8] */
    0x02, /* Synth 1A Post Divide [7:0] */
    0x00, /* Synth 1B Post Divide [23:16] */
    0x00, /* Synth 1B Post Divide [15:8] */
    0x64, /* Synth 1B Post Divide [7:0] */
    0x00, /* Synth 1C Post Divide [23:16] */
    0x00, /* Synth 1C Post Divide [15:8] */
    0x64, /* Synth 1C Post Divide [7:0] */
    0xF3, /* Synth 1D Post Divide [23:16] */
    0xC3, /* Synth 1D Post Divide [15:8] */
    0x50, /* Synth 1D Post Divide [7:0] */
    0x00, /* Synth 2A Post Divide [23:16] */
    0x00, /* Synth 2A Post Divide [15:8] */
    0x08, /* Synth 2A Post Divide [7:0] */
    0x00, /* Synth 2B Post Divide [23:16] */
    0x00, /* Synth 2B Post Divide [15:8] */
    0x08, /* Synth 2B Post Divide [7:0] */
    0x00, /* Synth 2C Post Divide [23:16] */
    0x00, /* Synth 2C Post Divide [15:8] */
    0x64, /* Synth 2C Post Divide [7:0] */
    0x00, /* Synth 2D Post Divide [23:16] */
    0x00, /* Synth 2D Post Divide [15:8] */
    0x28, /* Synth 2D Post Divide [7:0] */
    0x00, /* Synth 3A Post Divide [23:16] */
    0x00, /* Synth 3A Post Divide [15:8] */
    0x08, /* Synth 3A Post Divide [7:0] */
    0x00, /* Synth 3B Post Divide [23:16] */
    0x00, /* Synth 3B Post Divide [15:8] */
    0x08, /* Synth 3B Post Divide [7:0] */
    0x00, /* Synth 3C Post Divide [23:16] */
    0x00, /* Synth 3C Post Divide [15:8] */
    0x64, /* Synth 3C Post Divide [7:0] */
    0x00, /* Synth 3D Post Divide [23:16] */
    0x00, /* Synth 3D Post Divide [15:8] */
    0x64, /* Synth 3D Post Divide [7:0] */
    0x00, /* Reserved */
    0x00, /* Reserved */
    0x00, /* Reserved */
    0x00, /* Reserved */
    0x00, /* Synth 0C Post Div Phase [15:8] */
    0x00, /* Synth 0C Post Div Phase [7:0] */
    0x00, /* Synth 0D Post Div Phase [15:8] */
    0x00, /* Synth 0D Post Div Phase [7:0] */
    0x00, /* Reserved */
    0x00, /* Reserved */
    0x00, /* Reserved */
    0x00, /* Reserved */
    0x00, /* Synth 1C Post Div Phase [15:8] */
    0x00, /* Synth 1C Post Div Phase [7:0] */
    0x00, /* Synth 1D Post Div Phase [15:8] */
    0x00, /* Synth 1D Post Div Phase [7:0] */
    0x00, /* Reserved */
    0x00, /* Reserved */
    0x00, /* Reserved */
    0x00, /* Reserved */
    0x00, /* Synth 2C Post Div Phase [15:8] */
    0x00, /* Synth 2C Post Div Phase [7:0] */
    0x00, /* Synth 2D Post Div Phase [15:8] */
    0x00, /* Synth 2D Post Div Phase [7:0] */
    0x00, /* Reserved */
    0x00, /* Reserved */
    0x00, /* Reserved */
    0x00, /* Reserved */
    0x00, /* Synth 3C Post Div Phase [15:8] */
    0x00, /* Synth 3C Post Div Phase [7:0] */
    0x00, /* Synth 3D Post Div Phase [15:8] */
    0x00, /* Synth 3D Post Div Phase [7:0] */
    0x00, /* Synth0 Skew */
    0x00, /* Synth1 Skew */
    0x00, /* Synth2 Skew */
    0x00, /* Synth3 Skew */
    0x45, /* Synth Stop Clock 1-0 */
    0x00, /* Synth Stop Clock 3-2 */
    0x00, /* Reserved */
    0x00, /* Reserved */
    0x00, /* Reserved */
    0x00, /* Reserved */
    0x00, /* Reserved */
    0x00, /* Reserved */
    0x00, /* Reserved */
    0x00, /* Reserved */
    0x00, /* Reserved */
    0x00, /* Reserved */
    0x00, /* Reserved */
    0xBB, /* HP Diff Output Enable */
    0xFF, /* HP CMOS Output Enable */
    0xFF, /* Reserved */
    0xFF, /* Reserved */
    0xFF, /* Reserved */
    0x00, /* GPIO-0 Select/Status */
    0x00, /* GPIO-1 Select/Status */
    0x60, /* GPIO-2 Select/Status */
    0x00, /* GPIO-3 Select/Status */
    0x00, /* GPIO-4 Select/Status */
    0x00, /* GPIO-5 Select/Status */
    0x00, /* GPIO-6 Select/Status */
    0x00, /* Reserved */
    0x00, /* Reserved */
    0x00, /* Reserved */
    0x00, /* Reserved */
    0x00, /* Reserved */
    0x00, /* Reserved */
    0x00, /* Reserved */
    0x00, /* Reserved */
    0x00, /* Reserved */
    0x00, /* GPIO Input 7-0 */
    0x00, /* Reserved */
    0x00, /* GPIO Output 7-0 */
    0x00, /* Reserved */
    0x00, /* GPIO Output Enable 7-0 */
    0x00, /* Reserved */
    0x00, /* GPIO Freeze 7-0 */
    0x00, /* Reserved */
    0x00, /* Reserved */
    0x05, /* Page Select */
    0x00, /* Reserved */
    0x00, /* Reserved */
    0x00, /* Reserved */
    0x00, /* Reserved */
    0x00, /* Reserved */
    0x00, /* Reserved */
    0x00, /* Reserved */
    0x00, /* Reserved */
    0x00, /* Reserved */
    0x00, /* Reserved */
    0x00, /* Reserved */
    0x00, /* Reserved */
    0x00, /* Reserved */
    0x00, /* Reserved */
    0x00, /* Reserved */
    0x00, /* Reserved */
    0x00, /* Reserved */
    0x00, /* Reserved */
    0x00, /* Reserved */
    0x00, /* Reserved */
    0x00, /* Reserved */
    0x00, /* Reserved */
    0x00, /* Reserved */
    0x00, /* Reserved */
    0x00, /* Reserved */
    0x00, /* Reserved */
    0x00, /* Reserved */
    0x00, /* Reserved */
    0x00, /* Reserved */
    0x00, /* Reserved */
    0x00, /* Reserved */
    0x00, /* Reserved */
    0x00, /* Reserved */
    0x00, /* Reserved */
    0x00, /* Reserved */
    0x00, /* Reserved */
    0x00, /* Reserved */
    0x00, /* Reserved */
    0x00, /* Reserved */
    0x00, /* Reserved */
    0x00, /* Reserved */
    0x00, /* Reserved */
    0x00, /* Reserved */
    0x00, /* Reserved */
    0x00, /* Reserved */
    0x00, /* Reserved */
    0x00, /* Reserved */
    0x00, /* Reserved */
    0x00, /* Reserved */
    0x00, /* Reserved */
    0x00, /* Reserved */
    0x00, /* Reserved */
    0x00, /* Reserved */
    0x00, /* Reserved */
    0x00, /* Reserved */
    0x00, /* Reserved */
    0x00, /* Reserved */
    0x00, /* Reserved */
    0x00, /* Reserved */
    0x00, /* Reserved */
    0x00, /* Reserved */
    0x00, /* Reserved */
    0x00, /* Reserved */
    0x00, /* Reserved */
    0x00, /* Reserved */
    0x00, /* Reserved */
    0x01, /* DPLL0 Fast Lock Ctrl */
    0xFF, /* DPLL0 Fast Lock Phase Error */
    0x04, /* DPLL0 Fast Lock Freq Error */
    0x01, /* DPLL1 Fast Lock Ctrl */
    0xFF, /* DPLL1 Fast Lock Phase Error */
    0x04, /* DPLL1 Fast Lock Freq Error */
    0x01, /* DPLL2 Fast Lock Ctrl */
    0xFF, /* DPLL2 Fast Lock Phase Error */
    0x04, /* DPLL2 Fast Lock Freq Error */
    0x00, /* Reserved */
    0x00, /* Reserved */
    0x00, /* Reserved */
    0x00, /* DPLL Fast Lock Error */
    0x00, /* DPLL FCL Control */
    0x00, /* Reserved */
    0x00, /* Reserved */
    0x00, /* Reserved */
    0x00, /* Reserved */
    0x00, /* DPLL0 Holdover Filt Ctrl */
    0x00, /* DPLL1 Holdover Filt Ctrl */
    0x00, /* DPLL2 Holdover Filt Ctrl */
    0x00, /* Reserved */
    0x00, /* DPLL0 NCO Ref Switch Ctrl */
    0x00, /* DPLL1 NCO Ref Switch Ctrl */
    0x00, /* DPLL2 NCO Ref Switch Ctrl */
    0x00, /* Reserved */
    0x00, /* DPLL0 Lock Delay */
    0x00, /* DPLL1 Lock Delay */
    0x00, /* DPLL2 Lock Delay */
    0x00, /* Reserved */
    0x00, /* DPLL0 FP Lock Criteria */
    0x00, /* DPLL1 FP Lock Criteria */
    0x00, /* DPLL2 FP Lock Criteria */
    0x00, /* Reserved */
    0x00, /* Ref0 Sync Offset Comp */
    0x00, /* Ref1 Sync Offset Comp */
    0x00, /* Ref2 Sync Offset Comp */
    0x00, /* Ref3 Sync Offset Comp */
    0x00, /* Ref4 Sync Offset Comp */
    0x00, /* Ref5 Sync Offset Comp */
    0x00, /* Ref6 Sync Offset Comp */
    0x00, /* Ref7 Sync Offset Comp */
    0x00, /* Ref8 Sync Offset Comp */
    0x00, /* Ref9 Sync Offset Comp */
    0x00, /* Ref10 Sync Offset Comp */
    0x00, /* Reserved */
    0x00, /* Reserved */
    0x00, /* Reserved */
    0x00, /* Reserved */
    0x00, /* Reserved */
    0x00, /* Reserved */
    0x00, /* Reserved */
    0x00, /* Reserved */
    0x00, /* Reserved */
    0x00, /* Reserved */
    0x00, /* Reserved */
    0x00, /* DPLL0 Ref Sync Ctrl */
    0x00, /* DPLL1 Ref Sync Ctrl */
    0x00, /* DPLL2 Ref Sync Ctrl */
    0x00, /* Reserved */
    0x00, /* Reserved */
    0x00, /* Page Select */
};
