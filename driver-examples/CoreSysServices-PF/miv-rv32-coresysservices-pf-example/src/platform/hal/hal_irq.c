/***************************************************************************//**
 * Copyright 2019-2022 Microchip FPGA Embedded Systems Solutions.
 *
 * SPDX-License-Identifier: MIT
 * 
 * @file hal_irq.c
 * @author Microchip FPGA Embedded Systems Solutions
 * @brief Legacy interrupt control functions for the Microchip driver library 
 * hardware abstraction layer.
 *
 */
#include "hal.h"
#include "miv_rv32_hal/miv_rv32_hal.h"

#ifdef __cplusplus
extern "C" {
#endif

/*------------------------------------------------------------------------------
 * 
 */
void HAL_enable_interrupts(void) {
    MRV_enable_interrupts();
}

/*------------------------------------------------------------------------------
 * 
 */
psr_t HAL_disable_interrupts(void) {
    psr_t psr;
    psr = read_csr(mstatus);
    MRV_disable_interrupts();
    return(psr);
}

/*------------------------------------------------------------------------------
 * 
 */
void HAL_restore_interrupts(psr_t saved_psr) {
    write_csr(mstatus, saved_psr);
}

#ifdef __cplusplus
}
#endif
