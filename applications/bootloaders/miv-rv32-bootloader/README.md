
#  Mi-V Bootloader firmware

 This project allows you to write application executable into the SPI flash or
 the I2C EEPROM.
 The executable is expected to be preloaded in the LSRAM memory using the Libero
 memory client configurator.

 Writing to the SPI flash is tested on PolarFire Eval Kit on-board SPI flash.
 Writing to the EEPROM is tested on PolarFire Avalanche kit with MikroBus Dual
 EE kit.

## How to use this example

This example project requires USB-UART interface to be connected to a host PC.
The host PC must connect to the serial port using a terminal emulator such as
HyperTerminal or PuTTY configured as follows:
 - 115200 baud
 - 8 data bits
 - 1 stop bit
 - no parity
 - no flow control

This program displays a self explainatory menu which can be used to perform
different actions.

### Build configurations
The following build configurations are provided with this project

| Configuration      | Description |
| -----------        | ---------------------- |
| Bootloader-Debug   | Run Bootloader in step-debug mode. RV32 IMC extensions. Links to default TCM base address 0x40000000. Not Optimized (-O0). |
| Bootstrap          | Used to generate the elf file used by the DGC designs. Links to default TCM base address 0x40000000. Not Optimized (-O0). </br> For general usage, the *Bootloader-Debug* configuration is recommended|

The Bootloader-Debug configuration provides additional YMODEM functionality to
download a hex file over UART terminal to the LSRAM at address 0x80000000. It can
then be copied into either the SPI flash or the LSRAM.

The Bootstrap configuration assumes that the LSRAM is preloaded with the hex
file as a memory client in the Libero design flow as explained in the design guide. If you are using this flow, you may not need the YMODEM functionality.

## Target hardware
The Libero designs that are compatible with the use cases provided by this example application are listed in the following table.

| UART Menu option | Design Configuration   | Feature                    | GitHub Repository          |
| ----------- | ---------------------- | -------------------------- | -------------------------- |
| Copy to SPI flash | DGC1             | SPI Flash Bootstrap        | [PolarFire® Evaluation Kit](https://github.com/Mi-V-Soft-RISC-V/PolarFire-Eval-Kit) |
| Copy to EEPROM    | DGC2             | I2C Flash Bootstrap       | [PolarFire® Avalanche board](https://github.com/Mi-V-Soft-RISC-V/Future-Avalanche-Board) |
| Download hex file to LSRAM  | DGC1 and DGC2 can be used | Download hex file to LSRAM. </br> Both DGC1 and DGC2 designs boot with a default hex file attached to the LSRAM as a client. </br> Use this menu to overwrite the LSRAM with a new hex file.| See above |

Notes:
- MikroBus Dual EE Click board is available from https://www.mikroe.com/dual-ee-click

All the design specific definitions such as peripheral base addresses, system
clock frequency etc. are included in fpga_design_config.h.

This example project can be used with another design using a different hardware
configuration. This can be achieved by overwriting the content of this example
project's _fpga_design_config.h_ file with the correct data from your Libero design.

### Booting the system:

The boot flow is as below:
 - Run this project in step-debug mode from TCM address space
 - Download new hex file to LSRAM address space using UART menu option 3. (This step is optional if the LSRAM is preloaded)
 - Copy the downloaded hex file to EEPROM or SPI flash per your design using UART menu options 1 and 2
 - For SPI boot on PolarFire Eval Kit:
    - Push and hold SW8 and press and release SW6 or SW7.
 - For I2C boot on Avalanche board:
    - Push and hold SW1 to disable BOOTSTRAP_BYPASS.
    - Press and release SW2 to perform a system reset request.
    - Release SW1 when the LEDs on the board are actively blinking.

For more details, Refer **MIV_ESS_DG_50003259A.pdf** provided with MIV_ESS core
via Libero catalog and associate designs at [github Repository](https://github.com/Mi-V-Soft-RISC-V/Future-Avalanche-Board/tree/main/Libero_Projects/import/components/IMC_DGC2).

## Silicon revision dependencies
This example is tested on PolarFire MPF300T and TS device.
