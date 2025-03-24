
/*******************************************************************************
 * Copyright 2025 Microchip FPGA Embedded Systems Solutions.
 *
 * SPDX-License-Identifier: MIT
 *
 * @file config.h
 * @author Microchip FPGA Embedded Systems Solutions
 * @brief Board configuration file to set which PHY driver to use.
 *
 */

#ifndef CONFIG_H_
#define CONFIG_H_

/* Define whichever one of the following boards you are running the example on. */

#undef EVEREST_BOARD
#define PF_EVAL_KIT (1)

#if defined(EVEREST_BOARD) || defined(PF_EVAL_KIT)

#define VSC8575_PHY (1)

#endif
#undef M88E1340_PHY
#undef M88E1111_PHY

#endif /* CONFIG_H_ */
