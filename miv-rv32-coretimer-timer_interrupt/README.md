
# CoreTimer interrupt mode example

This example program demonstrates the use of the CoreTimer driver used in
continuous interrupt mode. It toggles a LED on and off every second.

There are two different build configurations provided with this project.
which configure this SoftConsole project for RISC-V IMC instruction extension.
The Following configurations are provided with the example.

- miv-rv32-imc-debug
- miv-rv32-imc-release

## How to use this example

This example project is targeted at a MIV_RV32 design running on a PolarFire-Eval-Kit.
Run the example project using a debugger.

## fpga_design_config (formerly known as hw_config.h)

The SoftConsole project targeted for Mi-V processors now use an improved
directory structure. The fpga_design_config.h must be stored as shown below

`<project-root>/boards/<board-name>/fpga_design_config/fpga_design_config.h`

Currently, this file must be hand crafted when using the Mi-V Soft Processor.
In future, all the design and soft IP configurations will be automatically
generated from the Libero design description data.

You can use the sample file provided with MIV_RV32 HAL as an example. Rename it from
sample_fpga_design_config.h to fpga_design_config.h and then customize it per your
hardware design such as SYS_CLK_FREQ, peripheral BASE addresses, interrupt numbers,
definition of MSCC_STDIO_UART_BASE_ADDR if you want a CoreUARTapb mapped to STDIO, etc.

## Target hardware

This example can be targeted at the latest Mi-V Soft Processor MIV_RV32. Choose the build
configurations per the target processor in your design and the supported ISA extensions
that you want to use.

All the design specific definitions such as peripheral base addresses, system clock frequency
etc. are included in fpga_design_config.h(hw_config.h).

This project is tested on PolarFire Eval Kit with CFG1 configuration of the
MiV_RV32 base design available at github [Mi-V-Soft-RISC-V](https://mi-v-ecosystem.github.io/redirects/repo-polarfire-evaluation-kit-mi-v-sample-fpga-designs)

CFG1 design uses the MIV_ESS core.
It is configured with a clock frequency of 83MHz.
Ensure that the fpga_design_config.h(hw_config.h) matches the design.

This example project can be used with another design using a different hardware
configuration. This can be achieved by overwriting the content of this example
project's "fpga_design_config.h(hw_config.h)" file with the correct data from your Libero design.

## Silicon revision dependencies

This example is tested on PolarFire MPF300TS device.
