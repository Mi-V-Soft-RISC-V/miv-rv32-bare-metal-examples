# MIV PLIC Example

This project demonstrates the Mi-V Soft IP PLIC module functionality, which is delivered as a
part of MIV Extended Sub System(MIV_ESS).  
In this project the PLIC IRQ is connected to the external interrupt of the MIV_RV32.
The general interrupt handling flow is something like this:

*PLIC_IRQ(from MIV_ESS) -> MIV_HAL IRQ handler -> PLIC driver isr() -> PLIC number ISR handler().*

The PLIC interrupts can be connected to any of the available external interrupts to the MIV_RV32. The application developer will have to perform interrupt handling accordingly.

There are 2 different build configurations provided with this project which configure
this SoftConsole project for RISC-V IMC instruction extension.
The Following configurations are provided with the example.
 - miv32imc-Debug
 - miv32imc-Release

To use this project, configure the COM port interface as below:
  - 115200 baud
  - 8 data bits
  - 1 stop bit
  - no parity
  - no flow control

## fpga_design_config (formerly known as hw_config.h)
The SoftConsole project targeted for Mi-V processors now use an improved
directory structure. Detailed description of the folder structure is available
at https://github.com/Mi-V-Soft-RISC-V/miv-rv32-documentation.
The fpga_design_config.h must be stored as shown below

`
    <project-root>/boards/<board-name>/fpga_design_config/fpga_design_config.h
`

Currently, this file must be hand crafted when using the Mi-V Soft Processor.
In future, all the design and soft IP configurations will be automatically
generated from the Libero design description data.

You can use the sample file provided with MIV_RV32 HAL as an example.
Rename it from sample_fpga_design_config.h to fpga_design_config.h and then
customize it per your hardware design such as SYS_CLK_FREQ, peripheral
BASE addresses, interrupt numbers, definition of MSCC_STDIO_UART_BASE_ADDR if
you want a CoreUARTapb mapped to STDIO, etc.

## Linker script changes

You must make sure that in addition to the reset vector, all other memory address
configurations in the Mi-V soft processor match with the memory layout defined
in the linker script.

## Target hardware
This project is tested on PolarFire Eval Kit with MIV_RV32 IMC Core.    
Libero example/job URL here:
https://mi-v-soft-risc-v.github.io/PolarFire-Eval-Kit/

## Mi-V soft processor revision dependencies
This project is tested with MIV_RV32 v3.0
