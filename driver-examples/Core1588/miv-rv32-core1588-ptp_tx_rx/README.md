# Core1588 receive and transmit PTP packet application

This example project demonstrates the use of Core1588 driver APIs implementing a
transmit and receive PTP packet application, as well as the internal RTC features.

There are two different build configurations provided with this project which
configure this SoftConsole project for RISC-V IMC instruction extension.
The following configurations are provided with the example.

- miv-rv32-imc-debug
- miv-rv32-imc-release

## How to use this example

- Connect the board to a DHCP capable switch or network.
- Connect via the board's USB-UART serial cable to a host PC running a terminal
  emulator such as TeraTerm or PuTTY configured as follows:
   - 115200 baud
   - 8 data bits
   - 1 stop bit
   - no parity
   - no flow control
- Run the example project using a debugger. A greeting message will appear over
  the UART terminal followed by a menu system and instructions.

The software will auto-negotiate an Ethernet link with the network's switch.
The application may take several attempts before an ethernet link can be established.
PTP messages should be sent with a sequence ID of 123 to be shown when the
application is placed in receive message demo mode. There is a known issue where a
message may have to be received multiple times before it is recognised by the Core1588.
There is another known issue regarding the trigger funcitonality of the RTC. The trigger
does not trip until much later than the trigger time that is programmed to the Core1588.

## fpga_design_config (formerly known as hw_config.h)

The SoftConsole project targeted for Mi-V processors now use an improved directory
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

This project has been tested on the following board with the MIV_RV32 IMC Core:

- PolarFire Evaluation Kit (MPF300-EVAL-KIT)
  
The PolarFire Evaluation Kit uses Microchip's VSC8575 PHY.
The driver for this PHY can be found in the CoreTSE driver directory, `src/platform/drivers/fpga_ip/CoreTSE`.

## Modifying this example to target other boards or interfaces

Refer to the `src/application/config.h` to configure a the project to target a different
board.
The macro definitions in this file will set the correct PHY driver and clock configuration
settings based on the selected board.

## Silicon revision dependencies

This example is tested on PolarFire Evaluation Kit (MPF300-EVAL-KIT).
