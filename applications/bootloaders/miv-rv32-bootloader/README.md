
#  Mi-V Bootloader firmware

 This project provides the capability to download an executable to the LSRAM and run a new application from there. 
 Additionally, you can write the application executable into the SPI flash or I2C EEPROM.
 Note that the default executable is usually preloaded in the LSRAM memory using the Libero memory client configurator

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

This program displays a self-explanatory menu which can be used to perform
different actions.

### Build configurations
The following build configurations are provided with this project

| Configuration      | Description |
| -----------        | ---------------------- |
| Bootloader-Debug   | Run Bootloader in step-debug mode. RV32 IMC extensions. Links to default TCM base address 0x40000000. Not Optimized (-O0). |
| Bootstrap          | Used to generate the elf file used by the DGC designs. Links to default TCM base address 0x40000000. Not Optimized (-O0). </br> For general usage, the *Bootloader-Debug* configuration is recommended|

The Bootloader-Debug configuration provides additional YMODEM functionality to 
download a raw binary file over UART terminal to the LSRAM at address 0x80000000. Once the new executable is downloaded to the LSRAM, 
you can choose to do the following two actions using UART menu options.
 - Jump to LSRAM start address and execute the newly downloaded executable 
 - Copy the executable into either the SPI flash or the I2C EEPROM (For bootstrap functionality)

The Bootstrap configuration assumes that the LSRAM is preloaded with the raw binary 
file as a memory client in the Libero design flow as explained in the design guide. If you are using this flow, you may not need the YMODEM functionality.

## Target hardware
The Libero designs that are compatible with the use cases provided by this example application are listed in the following table.

| UART Menu option | Design Configuration   | Feature                    | GitHub Repository          |
| ----------- | ---------------------- | -------------------------- | -------------------------- |
| Copy to SPI flash | DGC1             | SPI Flash Bootstrap        | [PolarFire® Evaluation Kit](https://github.com/Mi-V-Soft-RISC-V/PolarFire-Eval-Kit) |
| Copy to EEPROM    | DGC2             | I2C Flash Bootstrap       | [PolarFire® Avalanche board](https://github.com/Mi-V-Soft-RISC-V/Future-Avalanche-Board) |
| Download raw binary file to LSRAM  | DGC1 and DGC2 can be used | Download raw binary file to LSRAM. </br> Both DGC1 and DGC2 designs boot with a default raw binary file attached to the LSRAM as a client. </br> Use this menu to overwrite the LSRAM with a new raw binary file.| See above |

Notes:
- MikroBus Dual EE Click board is available from https://www.mikroe.com/dual-ee-click

All the design specific definitions such as peripheral base addresses, system
clock frequency etc. are included in fpga_design_config.h.

To ensure that a 10ms timer is provided, the Systick handler must be set based on the system clock frequency. Additionally, sufficient packet time 
must be set in order to initiate YMODEM data transfer

while using the YMODEM download option, it is important to set the maximum file size to avoid encountering 
an "YMODEM file transfer Failed- Aborted/File size exceeds limit" error.

The LSRAM used in the Libero design used to run this example project has a size of 128KB. However, it is important to note 
that the maximum size of the SPI flash and I2C eeprom when copying from the LSRAM is 32KB.

This example project can be used with another design using a different hardware
configuration. This can be achieved by overwriting the content of this example
project's _fpga_design_config.h_ file with the correct data from your Libero design.


### Bootloader-Debug : Memory details and Boot flow:

#### Memory details:
| Memory/Functionality | Default values   | How to change default values  |
| ----------- | ---------------------- | -------------------------- |
| LSRAM  | 128KB | LSRAM Configurator in Libero design|
| Copy from LSRAM to NVM(SPI Flash or I2C EEPROM) | 32KB | FLASH_EXECUTABLE_SIZE macro in source code|
| Copy from NVM to TCM | 32KB | MIV_ESS Configurator in Libero design|
| Download from YMODEM to LSRAM | 128KB | LSRAM_SIZE macro in source code |

Note: LSRAM_SIZE value should not exceed LSRAM size configured in the Libero design.

#### The boot flow is as below:
 - Launch the project in step-debug mode from the TCM address space
 - (Optional) If the LSRAM is not initialized with a memory client in the Libero design, download a new raw binary file to the LSRAM address space using UART menu option 3
 - To Run the loaded application select UART menu option 4
 - To Copy the downloaded raw binary file to the SPI flash or EEPROM based on your Libero design using UART menu options 1 and 2
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
