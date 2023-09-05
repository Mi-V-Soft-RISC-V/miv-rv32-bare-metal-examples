﻿
# CoreUARTapb Example

This example project demonstrates the use of CoreUARTapb driver APIs. 

## How to use this example

This sample project is targeted at a MIV_RV32 design running on a PolarFire 
Avalanche board(Future Electronics) connected via a USB-UART serial cable to a host PC running 
a terminal emulator such as TeraTerm or Putty configured as follows:
 - 115200 baud
 - 8 data bits
 - 1 stop bit
 - no parity
 - no flow control.

## Target hardware
This example can be targeted at the latest Mi-V Soft Processor MIV_RV32. Choose the build 
configurations per the supported ISA extensions that you want to use.

All the design specific definitions such as peripheral base addresses, system clock frequency 
etc. are included in hw_platform.h. 

To test this project, use Mi-V soft processor design targeted at Avalanche board available 
at [github Mi-V-Soft-RISC-V](https://github.com/Mi-V-Soft-RISC-V/Future-Avalanche-Board/tree/main/Libero_Projects).
Ensure that the hw_platform.h matches the design.

This example project can be used with another design using a different hardware
configuration. This can be achieved by overwriting the content of this example
project's "hw_platform.h" file with the correct data from your Libero design.

### Booting the system:

Currently the example project is configured to use FlashPro debugger to execute 
from LSRAM in both Debug and Release mode.

In the release mode build configuration, following setting is used
`--change-section-lma *-0x80000000`
under
Tool Settings > GNU RISC-V Cross Create Flash Image > General > Other flags . 

This will allow you to attach the release mode executable as the memory 
initialization client in Libero when you want to execute it from non-volatile memory. 

### Renode Emulation

The CoreUARTapb example can be run using Renode. A new window will open displaying the UART
console messages. Any characters typed into this window will be echoed back on the console.

The following configuration can be launched using miv-rv32-coreuartapb-tx-rx renode start-platform-and-debug.launch:

 - miv-rv32i-release

## Silicon revision dependencies
This example is tested on PolarFire MPF300TS device.
