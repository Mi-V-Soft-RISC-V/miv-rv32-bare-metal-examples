
# PolarFire User Crypto - Message Authentication, Verification and Hash Service
Example

This example project demonstrates the use of the following PolarFire User Crypto
Services functions:

   - CALSymEncAuth()
   - CALSymEncAuthDMA()
   - CALSymDecVerify()
   - CALSymDecVerifyDMA()
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

### Data Authenticated Encryption & Decryption Using AES 256-bit key

Select option '1' to perform authenticated encryption & decryption using Galois/
counter mode (GCM) algorithm. This example project reads the 256 bit key, IV,
16 bytes plain text, and additional authentication data (AAD) from UART terminal
and calls the **CALSymEncAuth()** or **CALSymEncAuthDMA()** function. This function
performs authenticated encryption with associated data for confidentiality and
integrity. The plain text is both encrypted and used in computation of message 
authentication code according to the **SATSYMTYPE_AES256** encryption algorithm
and GCM mode. Both the encrypted data and tag are displayed on UART terminal.
For authenticated decryption, it calls the **CALSymDecVerify()** or
**CALSymDecVerifyDMA()** function to perform authenticated decryption with
associated data for confidentiality and integrity. Both the decrypted data and
tag are also displayed on UART terminal.

**NOTE**:
   In Case of GCM, in place of IV paramter user has to pass JO value which is calculated 
   according to the algorithm mentioned in the following [document.](http://nvlpubs.nist.gov/nistpubs/Legacy/SP/nistspecialpublication800-38d.pdf)

### Generate MAC Using **HMACSHA-256** Algorithms

Select option '2' to calculate message authentication code using HMAC SHA-256
algorithms. This example project reads the key and message from UART terminal
and calls the **CALMAC()** or **CALMACDMA()** function. The **CALMAC()** and
**CALMACDMA()** functions calculate the MAC for the message based on SHA256
algorithm and the secret key. The calculated MAC is also displayed on UART
terminal.

### Generate MAC Using AES-CMAC-256 Algorithms

Select option '3' to calculated message authentication code using AES-CMAC-256
algorithms. This example project reads the key and message from UART terminal
and calls the CALMAC function. The **CALMAC()** functions calculates the MAC for
the message based on AES CMAC 256 algorithm and the secret key. The calculated
MAC is also displayed on UART terminal.

**NOTE**:
    MAC computation using DMA (i.e. **CALMACDMA** function) doesn’t support 
    **AES-CMAC-256** Algorithm.

### Hashing

Select option '4' to perform hashing. This service reads 64 bytes of message
value and performs the hash operation using SHA-256 hash algorithm on the stored
data. The hash results are stored at user memory location and also displayed on
UART terminal.

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

### Test Script

A test script is provided with this example which automatically enters the NIST
vectors and associated data to verify the functionality.

The following Tera Term macro scripts are present in "script" folder present. 
You can use these script for testing Message Authentication, Verfication and
Hashing services.
1. MAC-gcm_msg_auth_ver.ttl - Data Authenticated Encryption
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

## Target Hardware

This example can be targeted at the latest Mi-V Soft Processor MIV_RV32. Choose
the build configurations per the supported ISA extensions that you want to use.

All the design specific definitions such as peripheral base addresses, system
clock frequency etc. are included in fpga_design_config.h. 

This example project can be used with another design using a different hardware
configurations. This can be achieved by overwriting the content of this example
project's "fpga_design_config.h (hw_config.h)" file with the correct data from
your Libero design.

Please refer to the application note ([AN4591](https://www.microchip.com/en-us/application-notes/an4591))
for the design files in the prerequisites section. The application note provides
detailed information about the design description, programming the polarfire
device and SPI flash, and running user crypto example project.

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

This example is tested on PolarFire MPF300TS device.

### CAL Library Src

The CAL source code is bound by a license agreement. Please refer to the
application note ([AN4591](https://www.microchip.com/en-us/application-notes/an4591))
and navigate to the **Steps to Access PolarFire Security CAL and Documentation**
section for instructions on accessing the CAL source code.