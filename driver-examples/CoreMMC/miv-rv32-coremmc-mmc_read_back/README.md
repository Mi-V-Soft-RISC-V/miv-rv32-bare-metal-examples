# CoreMMC MMC Write and Read back example

This project writes a block of data to the eMMC (embedded Multi-Media Card)
device, reads the block back and performs a read-back check to ensure that the
data is valid.

The eMMC block size in this example is fixed to 512 Bytes, which is the standard
for eMMC devices which have a storage capacity of greater than 2GB.

There are two different build configurations provided with this project which configure
this SoftConsole project for RISC-V IMC instruction extension.
The following configurations are provided with the example.

- miv-rv32-imc-debug
- miv-rv32-imc-release

## How to use this example

- Connect via the board's USB-UART serial cable to a host PC running a terminal
  emulator such as TeraTerm or PuTTY configured as follows:
   - 115200 baud
   - 8 data bits
   - 1 stop bit
   - no parity
   - no flow control
- Run the example project using a debugger.

The terminal emulator command line interface will display (through UART) the status
of the write and read operations to the eMMC device.

## fpga_design_config (formerly known as hw_config.h)

The SoftConsole project targeted for Mi-V processors now uses an improved directory
structure.
The `fpga_design_config.h` must be stored as shown below:

`<project-root>/boards/<board-name>/fpga_design_config/fpga_design_config.h`

Currently, this file must be hand crafted when using the Mi-V Soft Processor.
In future, all the design and soft IP configurations will be automatically generated
from the Libero design description data.

You can use the sample file provided with MIV_RV32 HAL as an example.
Rename it from `sample_fpga_design_config.h` to `fpga_design_config.h` and then customize
it per your hardware design such as `SYS_CLK_FREQ`, peripheral BASE addresses, interrupt
umbers, definition of `MSCC_STDIO_UART_BASE_ADDR` if you want a CoreUARTapb mapped
to STDIO, etc.

## Target hardware

This example can be targeted at the latest Mi-V Soft Processor MIV_RV32.
Choose the build configurations per the target processor in your design and the supported
ISA extensions that you want to use.

All the design specific definitions such as peripheral base addresses, system clock
frequency etc. are included in fpga_design_config.h(hw_config.h).

This project has been tested on the following board with the MIV_RV32 IMC Core:

- Smart Fusion 2 Development Kit (Revision C) (M2S050T-FG896)

The Smart Fusion 2 Development Kit has a SanDisk SDIN5C2-8G eMMC device on-board.
  
The design for this sample program needs to contain a single instance of the CoreMMC
configured to an MMC Data Width and FIFO depth of your choice.
The driver supports all possible configurations of these parameters automatically.

This example project can be used with another design using a different hardware
configuration. This can be achieved by overwriting the content of this example project's
"fpga_design_config.h(hw_config.h)" file with the correct data from your Libero design.

## Silicon revision dependencies

This example is tested on Smart Fusion 2 Development Kit (Revision C) (M2S050T-FG896)
