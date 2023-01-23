
# Core10GBaseKR PHY example on Mi-V soft processors

This project demonstrates the use of Core10GBaseKR PHY driver APIs.

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

## phy_sw_cfg
The PHY software configuration file provides supported PHY models and an interface for overriding driver default values.
The phy_sw_cfg.h must be stored as shown below

`
    <project-root>/boards/<board-name>/platform_config/driver_config/phy_sw_cfg.h
`

- The PHY demonstrated in this example is enabled by defining the macros; "CORE10GBASEKR_PHY" and "PF_XCVR_C10GB".
- The Core10GBaseKR_PHY driver defaults configurations can be overridden by defining them in this file.
- Define the macro "C10GBKR_PERFORMANCE_MESSAGES" to enable 10GBASE-KR performance and debug messages.  

## fpga_design_config (formerly known as hw_config.h)
The SoftConsole project targeted for Mi-V processors now use an improved
directory structure. 
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

## Mi-V soft processor revision dependencies
This project is tested with MIV_RV32 v3.0