PolarFire User Crypto DRBG Services example
================================================================================ 
This example project demonstrates the use of PolarFire User Crypto 
deterministic random bit generator using functions:

   - CALDRBGInstantiate()
   - CALDRBGGenerate()
   - CALDRBGUninstantiate()
   
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

Run the example project using a debugger. The example project will display
instructions over the serial port. To execute the particular service, user has
to select the options as shown over the serial port.

Press the "i" to instantiate random numbers. Press the "g" to generate a random
number. Press "d" to un-instantiate the currently instantiated DRBG.
Please note that you will not be able to generate random numbers once the DRBG 
instance has been un-instantiates DRBG service.

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