
# PolarFire User Crypto RSA Encyption/Deryption Services Example

This example project demonstrates the use of the PolarFire RSA service to 
encrypt and decrypt the message. The following User Athena service are used:

  - RSA Encryption  
  - RSA Decryption

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

The example project will display instructions over the serial port. To execute
the particular service, user has to enter the required information as shown over
the serial port. This program also displays the return data from User Crypto
processor.

RSA is an algorithm used by modern computers to encrypt and decrypt messages. 
It is an asymmetric cryptographic algorithm. Asymmetric means that there are two
different keys. This is also called public key cryptography, because one of them
can be given to everyone. The other key must be kept private. It is based on the
fact that finding the factors of an integer is hard (the factoring problem). 
RSA involves a public key and private key. The public key can be known to 
everyone, it is used to encrypt messages. Messages encrypted using the public 
key can only be decrypted with the private key. 

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

Option '1' to encrypt the message using RSA.
This service encrypt the message based on public key. This example project will
read the public key(n & e) and message from UART terminal and computes the
cipher text corresponding to c = m^e mod n. The cipher text is displayed on 
UART terminal.

### Data Decryption

Option '2' to decrypt the message using RSA.
This service decrypt the message based on private key. This service performs
an RSA private key decryption operation with CRT and SCA countermeasures on the 
data buffer. This example project will read the cipher text, private key (n & d), 
public key(n & e), private prime modulus i.e. P modulus and q modulus from UART
terminal and computes the plain text corresponding to m = c^d mod n = m^(de) mod n.
The computed plain text is displayed on UART terminal.

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

The CAL source code is bound by license agreement. If you need access to the CAL
source code, please contact FPGA_marketing@microchip.com for further details on
NDA requirements.