================================================================================
                CoreI2C Master/Slave example
================================================================================

This example project demonstrates the use of the CoreI2C IP in a PolarFire Eval
kit. It demonstrates reading and writing data between a pair of CoreI2Cs 
configured as a master and a slave.

The operation of the CoreI2Cs is controlled via a serial console.

--------------------------------------------------------------------------------
                            Mi-V Soft Processor
--------------------------------------------------------------------------------
This example uses a Mi-V Soft processor MiV_RV32IMA_L1_AHB. The design is 
built for debugging MiV_RV32IMA_L1_AHB through the PolarFire FPGA programming 
JTAG port using a FlashPro5. To achieve this the CoreJTAGDebug IP is used to 
connect to the JTAG port of the MiV_RV32IMA_L1_AHB.

All the platform/design specific definitions such as peripheral base addresses,
system clock frequency etc. are included in hw_platform.h. The hw_platform.h is 
located at the root folder of this project.

The MiV_RV32IMA_L1_AHB firmware projects needs the riscv_hal and the hal firmware
(RISC-V HAL).

The RISC-V HAL is available through Firmware catalog as well as the link below:
    https://github.com/RISCV-on-Microsemi-FPGA/riscv-hal
    
--------------------------------------------------------------------------------
                            How to use this example
--------------------------------------------------------------------------------
Connect the IO pins for the SDA and SCL of the two CoreI2Cs together using
flying leads.

Connect a serial cable between the PolarFire Eval Kit and the PC and use your 
serial terminal software of choice, configured for 57600bps, no parity 
and one stop bit, to exercise the CoreI2Cs via the provided menu system. 

Alternatively, connect the PolarFire Eval Kit and the PC using the USB MINI 
connection using the same serial communications settings. In this case 
the serial port to use will be 3rd serial port provided by the 4232H device and 
may be labelled "FlashPro5 Port(COM135)".

Use menu option 1 to write between 0 and 32 bytes of data from the Master I2C
device to the slave I2C device. Any data received by the slave is written to
the slave_tx_buffer[] array and overwrites some or all of the default contents 
- "<<-------Slave Tx data ------->>". 0 byte transfers are allowed with this
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
      
--------------------------------------------------------------------------------
                                Target hardware
--------------------------------------------------------------------------------
This example project is targeted at a PolarFire Eval kit using Mi-V 
Soft Processor with 2 CoreI2C instantiated in the fabric (CoreI2C_0 and 
CoreI2C_1) and 4 BIBUFS.

The clock frequency in the example is 83MHz to ensure the design timing is SMBUS 
compatible (see CoreI2C Hand Book page 14 note for details).Trying to execute 
this example project on a different design will result in incorrect baud rate 
being used by CoreUART and timer load value.

This example project can be used with another design using a different clock
configuration. This can be achieved by overwriting the content of this example
project's "hw_platform.h" file with the correct data from your Libero design.

The CoreI2C instances were configured for "Full Master RX/TX, Slave RX/TX Modes"
with single channels. CoreI2C_0 int[0] connects to External Interrupt 4 and 
CoreI2C_1 int[0] connects to External Interrupt 5 of RISC-V core.

CoreI2C_0 registers are located at 0x70007000 and CoreI2C_1 registers are located
at 0x70008000. If your design differs, the COREI2C0_BASE_ADDR and 
COREI2C1_BASE_ADDR macros in hw_platform.h will need to be modified to match 
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

Connect the I/O as follows:

 1. COREI2C_0 SCL - Pin K10   
 2. COREI2C_0 SDA - Pin K9
 3. COREI2C_1 SCL - Pin T8
 4. COREI2C_1 SDA - Pin T9
 
The following jumper settings are required on the PolarFire Eval Kit
board. Please check your design if you have a different revision of the board.

 1. Connect J7 pin 3 to J7 pin 4 via a Jumper (SDA 0 to SDA 1). 
 2. Connect J7 pin 5 to J7 pin 6 via a Jumper (SCL 0 to SCL 1).

!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
                          Silicon revision dependencies
!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
This example is tested on MPF300T device.
