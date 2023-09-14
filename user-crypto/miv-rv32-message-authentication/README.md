PolarFire User Crypto - Message Authentication And Hash Service Example
================================================================================
This example project demonstrates the use of the following PolarFire User Crypto
Services functions:

   - CALSymEncAuth()
   - CALSymEncAuthDMA()
   - CALMAC()
   - CALMACDMA()
   - CALHash()
   - CALHashDMA()
    
The User Crypto message authentication service provides a way to ensure the 
message integrity and to confirm that the message came from the stated sender
(its authenticity). This example project demonstrate the use of MAC services 
to generate message authenticate code, authenticated encryption (the input 
message), and hash service to covert data of arbitrary length to a fixed length.

There are two different build configurations provided with this project which
configure this SoftConsole project for RISC-V IMC instruction extension. 
The following configurations are provided with the example:

  - miv-rv32-imc-debug
  - miv-rv32-imc-release

Mi-V Soft Processor
--------------------------------------------------------------------------------
This example uses Mi-V SoftProcessor MiV_RV32.The design is built for debugging
MiV_RV32 through the PolarFire FPGA programming JTAG port using a FlashPro5.
To achieve this the CoreJTAGDebug IP is used to connect to the JTAG port of the
MiV_RV32.

All the platform/design specific definitions such as peripheral base addresses,
system clock frequency etc. are included in fpga_design_config.h. The
fpga_design_config.h is located at the root folder of this project.

The Mi-V Soft Processor MiV_RV32 firmware projects needs the miv_rv32_hal and 
the hal firmware(RISC-V HAL).

The RISC-V HAL is available at GitHub [Mi-V-Soft-RISC-V](https://mi-v-ecosystem.github.io/redirects/platform).
    
How to use this example
--------------------------------------------------------------------------------
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

This program displays the return data from User Crypto processor for message 
authentication and hash service.

fpga_design_config (formerly known as hw_config.h)
--------------------------------------------------------------------------------
The SoftConsole project targeted for Mi-V processors now use an improved
directory structure. The fpga_design_config.h must be stored as shown below

`
    <project-root>/boards/<board-name>/fpga_design_config/fpga_design_config.h
`

Currently, this file must be hand crafted when using the Mi-V Soft Processor.
In future, all the design and soft IP configurations will be automatically
generated from the Libero design description data.

You can use the sample file provided with MIV_RV32 HAL as an example. Rename it
from sample_fpga_design_config.h to fpga_design_config.h and then customize it
per your hardware design such as SYS_CLK_FREQ, peripheral BASE addresses,
interrupt numbers, definition of MSCC_STDIO_UART_BASE_ADDR if you want a
CoreUARTapb mapped to STDIO, etc.

### Data Authenticated Encryption Using AES 256-bit key

Select option '1' to perform authenticated encryption using Galois/counter mode (GCM)
algorithm. This example project reads the 256 bit key, IV, 16 bytes plain text,
and additional authentication data (AAD) from UART terminal and calls the 
**CALSymEncAuth()** or **CALSymEncAuthDMA()** function. This function performs 
authenticated encryption with associated data for confidentiality and integrity.
The plain text is both encrypted and used in computation of message 
authentication code according to the **SATSYMTYPE_AES256** encryption algorithm and
GCM mode. Both the encrypted data and tag are displayed on UART terminal.

**NOTE** -: 
   In Case of GCM, in place of IV paramter user has to pass JO value which is calculated 
   according to the algorithm 
   mentioned in the following document. 
   [document](http://nvlpubs.nist.gov/nistpubs/Legacy/SP/nistspecialpublication800-38d.pdf)

Generate MAC Using **HMACSHA-256** Algorithms
Press 2 to calculate message authentication code using HMAC SHA-256 algorithms.
This example project reads the key and message from UART terminal and calls the
**CALMAC()** or **CALMACDMA()** function. The **CALMAC()** and **CALMACDMA()** functions 
calculate the MAC for the message based on SHA256 algorithm and the secret key.
The calculated MAC is also displayed on UART terminal.

Generate MAC Using AES-CMAC-256 Algorithms
Press 3 to calculated message authentication code using AES-CMAC-256 algorithms.
This example project reads the key and message from UART terminal and calls 
the CALMAC function. The **CALMAC()** functions calculates the MAC for the message
based on AES CMAC 256 algorithm and the secret key. The calculated MAC is also
displayed on UART terminal.
NOTE: 
     MAC computation using DMA (i.e. **CALMACDMA** function) doesn’t support 
     **AES-CMAC-256** Algorithm.

Hashing
Press 4 to perform hashing. This service reads 64 bytes of message value and 
performs the hash operation using SHA-256 hash algorithm on the stored data.
The hash results are stored at user memory location and also displayed on UART
terminal.

### Configurations

The CAL library needs a config_user.h containing the configuration data.
This application provides following settings as per CAL requirement   
  1. A variable which provides the base address of the User Crypto hardware block
     is defined in main.c
     
     `uint32_t g_user_crypto_base_addr = 0x62000000UL;`
     
  2. A symbol INC_STDINT_H is defined in project preprocessor setting. 
     For more detail, please refer to caltypes.h file present in CAL folder.

**NOTE**: 
   1. If you try to enter data values other than 0 - 9, a - f, A - F, an error 
      message will be displayed on the serial terminal.
   2. You must enter all input data as whole bytes. If you enter the 128-bit key
      {1230...0} as 0x12 0x3 and press return, this will be treated as
      byte0 = 0x12, byte1 = 0x30, byte2-127 = 0x00.

### Test script

A test script is provided with this example which automatically enters the NIST
vectors and associated data to verify the functionality. 

The following Tera Term macro scripts are present in "script" folder present. 
You can use these script for testing Message Authentication and Hashing 
services.
1. MAC-gcm_msg_auth.ttl - Data Authenticated Encryption
2. MAC-hmac_aes_cmac_256.ttl - Generate MAC Using AES-CMAC-256 Algorithms
3. MAC-hmac_sha_256.ttl - Generate MAC Using HMAC SHA-256 Algorithms
4. SHA-256.ttl - Hashing

**NOTE:**
1. Tera Term Macros don’t work with Windows 10 build 14393.0. You should update
   to Windows 10 build 14393.0.105 or [later.](https://osdn.net/ticket/browse.php?group_id=1412&tid=36526) 
2. Before running Tera Term Macro script, set language as English 
   (Setup->General->Language). Also setup transmit delay in (Setup->Serial port)
   to 5msec/char and 5msec/line.
3. By default, Tera Term log will be stored in Tera Term installation Directory.

## Target hardware

This project was tested on PolarFire-Eval-Kit with CFG4 configuration of the
MIV_RV32 design available [here](https://github.com/Mi-V-Soft-RISC-V/PolarFire-Eval-Kit/tree/main/Libero_Projects)

All the design specific definitions such as peripheral base addresses, system
clock frequency etc. are included in fpga_design_config.h. 

The firmware projects needs the HAL and the MIV_RV32 HAL firmware components.

This example project can be used with another design using a different hardware
configurations. This can be achieved by overwriting the content of this example
project's "fpga_design_config.h" file with the correct data per your Libero design.

### Booting the system

Currently the example project is configured to use FlashPro debugger to execute 
from LSRAM in both Debug and Release mode.

In the release mode build configuration, following setting is used
`--change-section-lma *-0x80000000` under
Tool Settings > Cross RISCV GNU Create Flash Image > General > Other flags. 

This will allow you to attach the release mode executable as the memory 
initialization client in Libero when you want to execute it from non-volatile memory. 

## Silicon revision dependencies

This example is tested on PolarFire MPF300TS device.

### CAL library src
 
To obtain the CAL source code and a SoftConsole project to generate the CAL
library archive file (*.a) refer [SoftConsole Documentation](https://mi-v-ecosystem.github.io/SoftConsole-Documentation/SoftConsole-v2021.3/using_softconsole/other.html#crypto-application-library).
The CAL source code is bound by license agreement and it will be available as
part of the SoftConsole installation if the CAL specific license was agreed
while installing it.