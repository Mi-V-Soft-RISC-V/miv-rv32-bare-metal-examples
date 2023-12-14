﻿
# CoreSPI Flash Example

This example project demonstrates the use of the CoreSPI Soft IP. It reads and
writes the content of an external SPI flash device.

There are two different build configurations provided with this project which
configure this SoftConsole project for RISC-V IMC instruction extension. 
The following configurations are provided with the example:

  - miv-rv32-imc-debug
  - miv-rv32-imc-release

## Mi-V Soft Processor

This example uses Mi-V SoftProcessor MIV_RV32. The design is built for debugging
MIV_RV32 through the PolarFire FPGA programming JTAG port using a FlashPro5.
To achieve this the CoreJTAGDebug IP is used to connect to the JTAG port of the
MIV_RV32.

All the platform/design specific definitions such as peripheral base addresses,
system clock frequency etc. are included in fpga_design_config.h.

The Mi-V Soft Processor MIV_RV32 firmware projects needs the MIV_RV32 HAL and 
the hal firmware(RISC-V HAL).

The hal is available at GitHub [Mi-V-Soft-RISC-V](https://mi-v-ecosystem.github.io/redirects/platform).

## How to Use This Example

This example project is targeted at a MIV_RV32 design running on a PolarFire-Eval-Kit
connected via a USB-UART serial cable to a host PC running a terminal emulator
such as TeraTerm or Putty configured as follows:

 - 115200 baud
 - 8 data bits
 - 1 stop bit
 - no parity
 - no flow control.

Run the example project using a debugger. A greeting message will appear over
the UART terminal followed by the more information.

## fpga_design_config (formerly known as hw_config.h)

The SoftConsole project targeted for Mi-V processors now use an improved
directory structure. The fpga_design_config.h must be stored as shown below

`
    <project-root>/boards/<board-name>/fpga_design_config/fpga_design_config.h
`

Currently, this file must be hand crafted when using the Mi-V Soft Processor.
In future, all the design and soft IP configurations will be automatically
generated from the Libero design description data.

You can use the sample file provided with miv_rv32_hal as an example. Rename it
from sample_fpga_design_config.h to fpga_design_config.h and then customize it
per your hardware design such as SYS_CLK_FREQ, peripheral BASE addresses,
interrupt numbers, definition of MSCC_STDIO_UART_BASE_ADDR if you want a
CoreUARTapb mapped to STDIO, etc.

## Target Hardware

This example can be targeted at the latest Mi-V Soft Processor MIV_RV32. Choose
the build configurations per the supported ISA extensions that you want to use.

All the design specific definitions such as peripheral base addresses, system
clock frequency etc. are included in fpga_design_config.h. 

This project is tested on PolarFire Eval Kit with DGC1 configuration of the
MiV_RV32 base design available at github [Mi-V-Soft-RISC-V](https://mi-v-ecosystem.github.io/redirects/repo-polarfire-evaluation-kit-mi-v-sample-fpga-designs)

DGC1 design uses the MIV_ESS core. It is configured to support the I2C, SPI,
UART, Timer,.. Ensure that the fpga_design_config.h(hw_config.h) matches the
design.

This example project can be used with another design using a different hardware
configurations. This can be achieved by overwriting the content of this example
project's "fpga_design_config.h (hw_config.h)" file with the correct data from
your Libero design.

### Booting the System

Currently the example project is configured to use FlashPro debugger to execute 
from LSRAM in both Debug and Release mode.

In the release mode build configuration, following setting is used
`--change-section-lma *-0x80000000` under
Tool Settings > Cross RISCV GNU Create Flash Image > General > Other flags. 

This will allow you to attach the release mode executable as the memory 
initialization client in Libero when you want to execute it from non-volatile
memory. 

## Silicon Revision Dependencies

This example is tested on PolarFire Evaluation Kit (MPF300T-EVAL-KIT).
