# CoreSysService_PF driver System Services example.
  This simple example demonstrates how to use the CoreSysService_PF driver to
  execute Device and design info services and Data security service supported by
  PolarFire System Controller.
  This project can be used with MIV_RV32 IP core as well as the legacy RV32 IP cores.

## How to use this example

This sample project is targeted at a MIV_RV32 design running on the
PolarFire Eval Kit.

This example project requires USB-UART interface to be connected to a host PC.
The host PC must connect to the serial port using a terminal emulator such as
HyperTerminal or PuTTY configured as follows:
 - 115200 baud
 - 8 data bits
 - 1 stop bit
 - no parity
 - no flow control

This program displays the return data from System controller for various Device
and Design services and Data security service:

## Target hardware
This example can be targeted at the latest Mi-V Soft Processor MIV_RV32 or its
variants. Choose the build configurations per the target processor in your
design and the supported ISA extensions that you want to use.

All the design specific definitions such as peripheral base addresses, system
clock frequency etc. are included in hw_platform.h.

A TCL based Libero design used for testing this project is provided under
./boards/icicle-kit-es/fpga_design/design_description folder.

This example project can be used with another design using a different hardware
configuration. This can be achieved by overwriting the content of this example
project's "hw_platform.h" file with the correct data from your Libero design.

This project was tested on PolarFire Eval Kit with CFG4 configuration of the
MIV_RV32 design available [here](https://mi-v-ecosystem.github.io/redirects/repo-polarfire-evaluation-kit-mi-v-sample-fpga-designs)

**Booting the system:**

Currently the example project is configured to use FlashPro debugger to execute
from LSRAM in both Debug and Release mode.

In the release mode build configuration, following setting is used
`--change-section-lma *-0x80000000`
under
Tool Settings > GNU RISC-V Cross Create Flash Image > General > Other flags .

This will allow you to attach the release mode executable as the memory
initialization client in Libero when you want to execute it from non-volatile memory.

## Silicon revision dependencies
This example is tested on PolarFire MPF300TS device.
