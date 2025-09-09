/*******************************************************************************
 * Copyright 2019 Microchip FPGA Embedded Systems Solutions.
 *
 * SPDX-License-Identifier: MIT
 *
 * @file miv_rv32_init.c
 * @author Microchip FPGA Embedded Systems Solutions
 * @brief Mi-V soft processor memory section initializations and start-up code.
 *
 */

#include <unistd.h>

#ifdef __cplusplus
extern "C" {
#endif

extern int main(void);

void _init(void)
{
    /* This function is a placeholder for the case where some more hardware
     * specific initializations are required before jumping into the application
     * code. You can implement it here. */

    /* Jump to the application code after all initializations are completed */
    int code = 0;

    code = main();
    _exit(code);
}

/* Function called after main() finishes */
void
_fini(void)
{
}

#ifdef __cplusplus
}
#endif

