
# CoreSPI Flash example

This example project demonstrates the use of the CoreSPI Soft IP.
It reads and writes the content of an external SPI flash device

There are two different build configurations provided with this project 
which configure this SoftConsole project for RISC-V IMC instruction extension. 
The Following configurations are provided with the example.

  - miv-rv32-imc-debug
  - miv-rv32-imc-release
    
## How to use this example

This example project is targeted at a MIV_RV32 design running on a PolarFire-Eval-Kit connected via a USB-UART serial cable to a host PC running 
a terminal emulator such as TeraTerm or Putty configured as follows:

 - 115200 baud
 - 8 data bits
 - 1 stop bit
 - no parity
 - no flow control.

Run the example project using a debugger. A greeting message will appear over the UART terminal followed by the more information and instructions.

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

## Target hardware

This example can be targeted at the latest Mi-V Soft Processor MIV_RV32. Choose the build 
configurations per the supported ISA extensions that you want to use.

All the design specific definitions such as peripheral base addresses, system clock frequency 
etc. are included in fpga_design_config.h(hw_config.h). 

To test this project, use Mi-V soft processor design targeted at PolarFire-Eval-Kit available 
at github [Mi-V-Soft-RISC-V](https://github.com/Mi-V-Soft-RISC-V/PolarFire-Eval-Kit/blob/main/Libero_Projects/import/components/IMC_DGC1/README.md).
This design uses the MIV_ESS core. It is configured to support the I2C,SPI,UART,Timer,..
Ensure that the fpga_design_config.h(hw_config.h) matches the design.

This example project can be used with another design using a different clock
configuration. This can be achieved by overwriting the content of this example
project's "fpga_design_config.h(hw_config.h)" file with the correct data from your Libero design.

## Silicon revision dependencies

This example is tested on PolarFire MPF300TS device.
