/*******************************************************************************
 * Copyright 2024 Microchip FPGA Embedded Systems Solutions.
 *
 * SPDX-License-Identifier: MIT
 *
 * @file fpga_design_config.h
 * @author Microchip FPGA Embedded Systems Solutions
 * @brief design configuration settings
 *
 */
 /*========================================================================*//**
  @mainpage
    Example file detailing how the fpga_design_config.h should be constructed
    for the SoftConsole project targeted for Mi-V processors.

    @section intro_sec Introduction
    The SoftConsole project targeted for Mi-V processors now have an improved
    folder structure. Detailed description of the folder structure is available
    at https://github.com/Mi-V-Soft-RISC-V/miv-rv32-documentation.

    The fpga_design_config.h must be stored as shown below
    <project-root>/boards/<board-name>/fpga_design_config.h

    Currently this file must be hand crafted when using the Mi-V Soft Processor.
    In future, all the design and soft IP configurations will be automatically
    generated from the Libero design description data.
    
    You can use this sample file as an example.
    Rename this file from sample_fpga_design_config.h to fpga_design_config.h
    and then customize it per your hardware design.

    @section Project configuration Instructions
    1. Change SYS_CLK_FREQ define to frequency of Mi-V Soft processor clock
    2  Add all the soft IP core BASE addresses
    3. Add the peripheral Core Interrupts to Mi-V Soft processor IRQ number
       mappings
    4. Define MSCC_STDIO_UART_BASE_ADDR if you want a CoreUARTapb mapped to
       STDIO

    **NOTE**
    In the legacy folder structures, the file hw_config.h as was used at the
    root of the project folder. This file is now depricated.

*//*=========================================================================*/

#ifndef FPGA_DESIGN_CONFIG_H_
#define FPGA_DESIGN_CONFIG_H_

/***************************************************************************//**
 * Soft-processor clock definition
 * This is the only clock brought over from the Mi-V Libero design.
 */
#ifndef SYS_CLK_FREQ
//#define SYS_CLK_FREQ                    80000000UL
#define SYS_CLK_FREQ                    70000000UL
#endif

/***************************************************************************//**
 * Peripheral base addresses.
 * Format of define is:
 * <corename>_<instance>_BASE_ADDR
 * The <instance> field is optional if there is only one instance of the core
 * in the design
 * MIV_ESS is an extended peripheral subsystem IP core with peripherals 
 * connections as defined below.
 * The system can be further extended by attaching APB peripherals to the 
 * empty APB slots.
 */
#define COREUARTAPB0_BASE_ADDR                    0x71000000UL
#define COREAXI4PROTOCONV_BASE_ADDR               0x81000000UL
#define COREGPIO_OUT_BASE_ADDR                    0x75000000UL
#define CORESPI_BASE_ADDR                         0x76000000UL

/***************************************************************************//**
 * Peripheral Interrupts are mapped to the corresponding Mi-V Soft processor
 * interrupt in the Libero design.
 *
 * On the legacy RV32 cores, there can be up to 31 external interrupts (IRQ[30:0]
 * pins). The legacy RV32 Soft processor external interrupts are defined in the
   miv_rv32_plic.h
 * 
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
 
 On the legacy RV32 processors, the PLIC identifies the interrupt and passes it
 on to the processor core. The interrupt 0 is not used. The pin IRQ[0] should
 map to External_1_IRQn likewise IRQ[30] should map to External_31_IRQn

e.g

#define TIMER0_IRQn                     External_30_IRQn
#define TIMER1_IRQn                     External_31_IRQn

 The MIV_RV32 soft processor has up to six optional system interrupts, MSYS_EI[n] 
 in addition to one EXT_IRQ.
 The MIV_RV32 does not have an inbuilt PLIC and all the interrupts are directly
 delivered to the processor core, hence unlike legacy RV32 cores, no interrupt
 number mapping is necessary on MIV_RV32 core.
 */

/****************************************************************************
 * Baud value to achieve a 115200 baud rate with system clock defined by
 * SYS_CLK_FREQ.
 * This value is calculated using the following equation:
 *      BAUD_VALUE = (CLOCK / (16 * BAUD_RATE)) - 1
 *****************************************************************************/
#define BAUD_VALUE_115200               ((SYS_CLK_FREQ / (16 * 115200)) - 1)

/******************************************************************************
 * Baud value to achieve a 57600 baud rate with system clock defined by
 * SYS_CLK_FREQ.
 * This value is calculated using the following equation:
 *      BAUD_VALUE = (CLOCK / (16 * BAUD_RATE)) - 1
 *****************************************************************************/
 #define BAUD_VALUE_57600                ((SYS_CLK_FREQ / (16 * 57600)) - 1)

/***************************************************************************//**
 * Define MSCC_STDIO_THRU_CORE_UART_APB in the project settings if you want the
 * standard IOs to be redirected to a terminal via UART.
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
