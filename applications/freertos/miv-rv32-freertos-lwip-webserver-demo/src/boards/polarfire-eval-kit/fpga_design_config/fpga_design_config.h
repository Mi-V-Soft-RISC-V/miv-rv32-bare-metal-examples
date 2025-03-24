/*******************************************************************************
 * Copyright 2017-2023 Microchip FPGA Embedded Systems Solutions.
 *
 * SPDX-License-Identifier: MIT
 *
 * @file fpga_design_config.h
 * @author Microchip FPGA Embedded Systems Solutions
 * @brief This file is taken from the miv_rv32_hal/sample_fpga_design_config.h
 *
 */

/*=========================================================================*//**
  @mainpage Sample file detailing how fpga_design_config.h should be constructed for
    the Mi-V processors.

    @section intro_sec Introduction
    The fpga_design_config.h is to be located in the project root directory.
    Currently this file must be hand crafted when using the Mi-V Soft processor.

    You can use this file as sample.
    Rename this file from sample_fpga_design_config.h to fpga_design_config.h and 
	store it in the root folder of your project. Then customize it per your HW design.

    @section driver_configuration Project configuration Instructions
    1. Change SYS_CLK_FREQ define to frequency of Mi-V Soft processor clock
    2  Add all other core BASE addresses
    3. Add peripheral Core Interrupt to Mi-V Soft processor interrupt mappings
    4. Define MSCC_STDIO_UART_BASE_ADDR if you want a CoreUARTapb mapped to STDIO
*//*=========================================================================*/

#ifndef FPGA_DESIGN_CONFIG_H_
#define FPGA_DESIGN_CONFIG_H_

/***************************************************************************//**
 * Soft-processor clock definition
 * This is the only clock brought over from the Mi-V Soft processor Libero design.
 */
#define SYS_CLK_FREQ                    83333000UL

/***************************************************************************//**
 * MTIME prescaler definition
 * The MiV soft-cores include, by default, a prescaler (100 is the default value)
 * on the MTIME on the hardware. To take this prescaler into account and make
 * the Renode and the hardware timings match, we set the macro RENODE_SIMULATION
 * to 1 if the demo is run on Renode, to 0 otherwise.
 */

#define RENODE_SIMULATION 0

#ifndef TICK_COUNT_PRESCALER
    #define TICK_COUNT_PRESCALER 100UL
#endif

/***************************************************************************//**
 * Non-memory Peripheral base addresses
 * Format of define is:
 * <corename>_<instance>_BASE_ADDR
 */
#define COREUARTAPB0_BASE_ADDR          0x60000000UL
#define CORESPI_BASE_ADDR               0x60001000UL
#define COREGPIO_OUT_BASE_ADDR          0x60003000UL
#define PF_CORE_SYSTEM_SERVICES         0x60004000UL

#define TSE_BASEADDR                    0x70000000UL
#define TSE_DESC                        0x71000000UL
#define TSE_TX_BUF                      0x72000000UL
#define TSE_RX_BUF                      0x72001000UL

#define MIV_ESS_APBSLOT3_BASE_ADDR      0x73000000UL
#define MIV_ESS_APBSLOT4_BASE_ADDR      0x74000000UL


/***************************************************************************//**
 * Peripheral Interrupts are mapped to the corresponding Mi-V Soft processor
 * interrupt from the Libero design.
 * There can be up to 31 external interrupts (IRQ[30:0] pins) on the Mi-V Soft
 * processor. The Mi-V Soft processor external interrupts are defined in the
 * riscv_plic.h
 * These are of the form
 * typedef enum
{
    NoInterrupt_IRQn = 0,
    External_1_IRQn  = 1,
    External_2_IRQn  = 2,
    .
    .
    .
    External_31_IRQn = 31
} IRQn_Type;

 The interrupt 0 on RISC-V processor is not used. The pin IRQ[0] should map to
 External_1_IRQn likewise IRQ[30] should map to External_31_IRQn
 * Format of define is:
 * <corename>_<instance>_<core interrupt name>
 */

#define TSE_IRQn                        MRV32_MSYS_EIE1_IRQn

/****************************************************************************
 * Baud value to achieve a 115200 baud rate with a 83MHz system clock.
 * This value is calculated using the following equation:
 *      BAUD_VALUE = (CLOCK / (16 * BAUD_RATE)) - 1
 *****************************************************************************/
#define BAUD_VALUE_115200               ((SYS_CLK_FREQ / (16 * 115200)) - 1)

/***************************************************************************//**
 * User edit section- Edit sections below if required
 */
#ifdef MSCC_STDIO_THRU_CORE_UART_APB
/*
 * A base address mapping for the STDIO printf/scanf mapping to CortUARTapb
 * must be provided if it is being used
 *
 * e.g. #define MSCC_STDIO_UART_BASE_ADDR COREUARTAPB1_BASE_ADDR
 */
#define MSCC_STDIO_UART_BASE_ADDR COREUARTAPB0_BASE_ADDR

#ifndef MSCC_STDIO_UART_BASE_ADDR
#error MSCC_STDIO_UART_BASE_ADDR not defined- e.g. #define MSCC_STDIO_UART_BASE_ADDR COREUARTAPB1_BASE_ADDR
#endif

#ifndef MSCC_STDIO_BAUD_VALUE
/*
 * The MSCC_STDIO_BAUD_VALUE define should be set in your project's settings to
 * specify the baud value used by the standard output CoreUARTapb instance for
 * generating the UART's baud rate if you want a different baud rate from the
 * default of 115200 baud
 */
#define MSCC_STDIO_BAUD_VALUE           115200
#endif  /*MSCC_STDIO_BAUD_VALUE*/

#endif  /* end of MSCC_STDIO_THRU_CORE_UART_APB */
/*******************************************************************************
 * End of user edit section
 */
#endif /* FPGA_DESIGN_CONFIG_H_ */
