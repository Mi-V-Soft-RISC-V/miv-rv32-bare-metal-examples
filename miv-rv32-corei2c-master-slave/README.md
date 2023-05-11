CoreI2C Master/Slave example  
================================================================================
This example project demonstrates the use of the CoreI2C Soft IP in PolarFire
FPGA. It demonstrates reading and writing data between a pair of CoreI2Cs 
configured as a master and a slave.

There are two different build configurations provided with this project 
which configure this SoftConsole project for RISC-V IMC instruction extension. 
The following configurations are provided with the example:

  - miv-rv32-imc-debug
  - miv-rv32-imc-release

The operation of the CoreI2Cs is controlled via a serial console.

Mi-V Soft Processor
--------------------------------------------------------------------------------
This example uses a Mi-V Soft processor MiV_RV32. The design is 
built for debugging MiV_RV32 through the PolarFire FPGA programming 
JTAG port using a FlashPro5. To achieve this the CoreJTAGDebug IP is used to 
connect to the JTAG port of the MiV_RV32.

All the platform/design specific definitions such as peripheral base addresses,
system clock frequency etc. are included in fpga_design_config.h. 
The fpga_design_config.h is located at the root folder of this project.

The Mi-V Soft Processor MiV_RV32 firmware projects needs the miv_rv32_hal and 
the hal firmware(RISC-V HAL).

The RISC-V HAL is available at GitHub [Mi-V-Soft-RISC-V](https://mi-v-ecosystem.github.io/redirects/platform).

How to use this example
--------------------------------------------------------------------------------
This example project is targeted at a MIV_RV32 design running on a PolarFire-Eval-Kit 
connected via a USB-UART serial cable to a host PC running a terminal emulator such as 
TeraTerm or Putty configured as follows:

 - 115200 baud
 - 8 data bits
 - 1 stop bit
 - no parity
 - no flow control.

Run the example project using a debugger. A greeting message will appear over the 
UART terminal followed by a menu system and instructions.

Use menu option 1 to write between 0 and 32 bytes of data from the Master I2C
device to the slave I2C device. Any data received by the slave is written to
the slave_tx_buffer[ ] array and overwrites some or all of the default contents 
"<<-------Slave Tx data ------->>". 0 byte transfers are allowed with this
option but not with option 3 below.

Use menu option 2 to read the 32 bytes of data from the Slave I2C 
slave_tx_buffer[] via I2C and display it on the console.

Use menu option 3 to write between 1 and 32 bytes of data to the slave and read
it back in the same operation (uses repeat start between read and write). 

Use menu option 4 to terminate the demo.

To demonstrate the error detection and time out features of the driver, follow
these steps:

1. Error: Disconnect the SDA line by removing the flying lead or Jumper 
   from J7 pin 3. Attempt to write some data to the slave via menu option 1
   and observe the "Data Write Failed!" message.
   
2. Time out: Connect the flying lead or Jumper removed from J7 in the previous
   step to gnd (e.g. J7 pin 3). Attempt to write some data to the slave via menu
   option 1 and observe the "Data Write Timed Out!" message after 3 seconds.

fpga_design_config (formerly known as hw_config.h)
--------------------------------------------------------------------------------
The SoftConsole project targeted for Mi-V processors now use an improved
directory structure. The fpga_design_config.h must be stored as shown below

`
    <project-root>/boards/<board-name>/fpga_design_config/fpga_design_config.h
`

Currently, this file must be hand crafted when using the Mi-V Soft Processor.
In the future, all the design and Soft IP configurations will be automatically
generated from the Libero design description data.

You can use the sample file provided with MIV_RV32 HAL as an example.
Rename it from sample_fpga_design_config.h to fpga_design_config.h and then
customize it according to your hardware design such as SYS_CLK_FREQ, peripheral
BASE addresses, interrupt numbers, definition of MSCC_STDIO_UART_BASE_ADDR if
you want a CoreUARTapb mapped to STDIO, etc.

Target hardware
--------------------------------------------------------------------------------
This example project is targeted at a PolarFire Eval kit using Mi-V 
Soft Processor with 2 CoreI2C instantiated in the fabric (CoreI2C_0 and 
CoreI2C_1) and 4 BIBUFS. Choose the build configurations per the supported 
ISA extensions that you want to use.

The clock frequency in the example is 83MHz to ensure the design timing is SMBUS 
compatible (see CoreI2C Hand Book page 14 note for details).Trying to execute 
this example project on a different design will result in incorrect baud rate 
being used by CoreUART and timer load value.

This example project can be used with another design using a different clock
configuration. This can be achieved by overwriting the content of this example
project's "fpga_design_config.h(hw_config.h)" file with the correct data from your Libero design.

The CoreI2C instances were configured for "Full Master RX/TX, Slave RX/TX Modes"
with single channels. CoreI2C_0 int[0] connects to External System Interrupt 0 and 
CoreI2C_1 int[0] connects to External System Interrupt 1 of RISC-V core.

CoreI2C_0 registers are located at 0x70004000 and CoreI2C_1 registers are located
at 0x70005000. If your design differs, the COREI2C0_BASE_ADDR and 
COREI2C1_BASE_ADDR macros in fpga_design_config.h will need to be modified to match 
your design.

The BIBUFs are connected as follows to provide the required Open Drain type
operation for I2C communications.

                           +---------------------+
                     I/O---| PAD               Y |--SCLI/SDAI
                           |                     |
                     gnd---| D                   |
                           |                     |
            SCLO/SDAO---|>O| E                   |
                           +---------------------+

The SCLO and SDAO signals are inverted and fed to the BIBUF enable whilst the
BIBUF D pin is connected to ground. This way when the signal is high, the
output is disabled (tri stated) and is pulled high by an external resistor and
when the signal is low, the output is actively pulled to ground.

It is a good idea to set the drive current for the I2C I/O to the maximum
allowed so that the fall time for the I2C pins is minimised. This will help to
improve the reliability of the I2C communications.

Connect the I/O for the SDA and SCL of the two CoreI2Cs as follows:

 1. COREI2C_0 SCL - Pin C3   
 2. COREI2C_0 SDA - Pin A3
 3. COREI2C_1 SCL - Pin D4
 4. COREI2C_1 SDA - Pin D3
 
The following jumper settings are required on the PolarFire Eval Kit
board. Please check your design if you have a different revision of the board.

 1. Connect J7 pin 3(A3) to J7 pin 4(D3) via a Jumper (SDA 0 to SDA 1). 
 2. Connect J7 pin 5(C3) to J7 pin 6(D4) via a Jumper (SCL 0 to SCL 1).

Note: The Polarfire Eval Kit base design for this project is currently not available
      on GitHub, but it will soon be available at GitHub [Mi-V-Soft-RISC-V](https://mi-v-ecosystem.github.io/redirects/repo-polarfire-evaluation-kit-mi-v-sample-fpga-designs)

Silicon revision dependencies
--------------------------------------------------------------------------------
This example is tested on MPF300TS device.
