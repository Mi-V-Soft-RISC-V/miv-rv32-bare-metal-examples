
# PolarFire User Crypto AES Cryptography Services example

This example project demonstrate the use of symmetric key encryption and 
decryption services. The following PolarFire User Crypto service functions are
used:

   - CALSymEncrypt()
   - CALSymEncryptDMA()
   - CALSymDecrypt()
   - CALSymDecryptDMA()
   
There are two different build configurations provided with this project which
configure this SoftConsole project for RISC-V IMC instruction extension. 
The following configurations are provided with the example:

  - miv-rv32-imc-debug
  - miv-rv32-imc-release

## Mi-V Soft Processor

This example uses Mi-V SoftProcessor MIV_RV32.The design is built for debugging
MIV_RV32 through the PolarFire FPGA programming JTAG port using a FlashPro5.
To achieve this the CoreJTAGDebug IP is used to connect to the JTAG port of the
MIV_RV32.

All the platform/design specific definitions such as peripheral base addresses,
system clock frequency etc. are included in fpga_design_config.h.

The Mi-V Soft Processor MIV_RV32 firmware projects needs the MIV_RV32 HAL and 
the hal firmware(RISC-V HAL).

The hal is available at GitHub [Mi-V-Soft-RISC-V](https://mi-v-ecosystem.github.io/redirects/platform).
    
## How to use this example

This example project is targeted at a MIV_RV32 design running on a PolarFire-Eval-Kit 
connected via a USB-UART serial cable to a host PC running a terminal emulator
such as TeraTerm or Putty configured as follows:

 - 115200 baud
 - 8 data bits
 - 1 stop bit
 - no parity
 - no flow control.

Run the example project using a debugger. A greeting message will appear over the 
UART terminal followed by a menu system and instructions.

This program displays the return data from User Crypto processor for encryption 
and decryption services.

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

### Data Encryption

Select option '1' for data encryption using AES-256 algorithm based on electronic
code book encryption (ECB) mode and key. This example project reads the plain 
text and an encryption key from URAT terminal and calls **CALSymEncrypt()** or 
**CALSymEncryptDMA()** function. The **CALSymEncrypt()** and **CALSymEncryptDMA()** 
functions encrypt the message based on electronic code book encryption (ECB) 
mode and symmetric encryption key and generate the cipher text. This cipher
text is also displayed on UART terminal.

### Data Decryption

Select option '2' for data decryption using AES-256 algorithm based on electronic
code book (ECB) mode and key. This example project reads a cipher text and a 
decryption key from URAT terminal and calls **CALSymDecrypt()** or **CALSymDecrypt()**
function. The **CALSymDecrypt()** and **CALSymDecryptDMA()** functions decrypt the 
cipher text based on ECB mode and symmetric encryption key, and generate the 
plain text. The generated plain text is also displayed on UART terminal. You 
have to provide same key to decrypt the cipher text, which is used for encryption.

**NOTE:**
	If you try to enter data values other than 0 - 9, a - f, A - F, an error 
    message will be displayed on the serial port terminal.

### Configurations

The CAL library needs a config_user.h containing the configuration data.
This application provides following settings as per CAL requirement   
  1. A variable which provides the base address of the User Crypto hardware block
     is defined in main.c
     
     `uint32_t g_user_crypto_base_addr = 0x62000000UL;`
     
  2. A symbol INC_STDINT_H is defined in project preprocessor setting. 
     For more detail, please refer to caltypes.h file present in CAL folder.

### Test Script

A test script is provided with this example which automatically enters the NIST
vectors and associated data to verify the functionality. You can use 
AES_Cryptography.ttl Tera Term Macro script present in project directory for 
testing AES Cryptography example project.

**NOTE:**
1. Tera Term Macros don’t work with Windows 10 build 14393.0. You should update
   to Windows 10 build 14393.0.105 or [later.](https://osdn.net/ticket/browse.php?group_id=1412&tid=36526) 
2. Before running Tera Term Macro script, set language as English 
   (Setup->General->Language). Also setup transmit delay in (Setup->Serial port)
   to 5msec/char and 5msec/line.
3. By default, Tera Term log will be stored in Tera Term installation Directory.

## Target Hardware

This project is tested on PolarFire-Eval-Kit with CFG4 configuration of the
MIV_RV32 base design available at github [Mi-V-Soft-RISC-V](https://mi-v-ecosystem.github.io/redirects/repo-polarfire-evaluation-kit-mi-v-sample-fpga-designs)

All the design specific definitions such as peripheral base addresses, system
clock frequency etc. are included in fpga_design_config.h. 

The firmware projects needs the HAL and the MIV_RV32 HAL firmware components.

This example project can be used with another design using a different hardware
configurations. This can be achieved by overwriting the content of this example
project's "fpga_design_config.h" file with the correct data per your Libero design.

### Booting the System

Currently the example project is configured to use FlashPro debugger to execute 
from LSRAM in both Debug and Release mode.

In the release mode build configuration, following setting is used
`--change-section-lma *-0x80000000` under
Tool Settings > Cross RISCV GNU Create Flash Image > General > Other flags. 

This will allow you to attach the release mode executable as the memory 
initialization client in Libero when you want to execute it from non-volatile memory. 

## Silicon Revision Dependencies

This example is tested on PolarFire MPF300TS device.

### CAL Library Src

The CAL source code is bound by license agreement. If you need access to the CAL
source code, please contact FPGA_marketing@microchip.com for further details on
NDA requirements.