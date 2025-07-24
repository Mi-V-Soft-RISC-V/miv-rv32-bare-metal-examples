
# CoreCAN Example

This example project demonstrates the use of CoreCAN IP to perform CAN message
transmission and reception with Mi-V soft processor. The CoreCAN driver has APIs
for Basic CAN and Full CAN Configurations. This project is configured for Full
CAN communication. The operation of the CoreCAN is controlled via a serial
console.

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

Run the example project using a debugger. A greeting message will appear over
the UART terminal followed by the more information. On startup, the example
project requests the user to enter the data to be send via the CAN Bus. You can
enter up to 32 pairs of hex digits (no separating spaces) and the data will be
sent out in chunks of 8 bytes at a time. You can send less than 32 bytes of 
data by pressing return to terminate the data early.

The test program then enters a loop looking for user input to select the next
action to perform. Whilst in this loop, the data portion of any CAN Bus packets
received into the rx buffers is displayed on the console. The following menu
options are available:

    6 - Get data from user and send via CAN Bus.
    7 - Reinitailze the CoreCAN peripheral then get data from user and send via
        CAN Bus.

The following macro modifies the behaviour of the program:

    CAN_TX_EXTENDED_ID        - Defining this macro causes CAN messages with
                                with extended 29 bit IDs to be sent instead of
                                the standard 11 bit IDs.

## Test CAN Message Transmission
    1. Enter the data on UART terminal, which will be received through UART.
    2. Based on received data bytes, segregate as CAN messages of maximum 8 
       bytes length.
    3. Send the received data in terms of CAN messages.
    4. Observe the CAN messages on CAN Analyzer with data length.
    5. Compare the data received on CAN Analyzer with the data sent from the 
       UART terminal data should be same.

## Test CAN Message Reception
    1. Use the provided Python script located in the <project-root>/script
       folder to send the data onto the CAN bus.
    2. Read the data using CAN APIs and store it in to buffer.
    3. Transmit the data on to UART terminal.
    4. Observe the data received on UART terminal.
    5. Compare the data sent from CAN Analyzer with the data received on 
       UART terminal data should be same.

Note: This example project was tested using the APGDT006 CAN Bus Analyzer FD to
      capture and transmit CAN packets.

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

## Target Hardware

This example can be targeted at the latest Mi-V Soft Processor MIV_RV32. Choose
the build configurations per the supported ISA extensions that you want to use.

All the design specific definitions such as peripheral base addresses, system
clock frequency etc. are included in fpga_design_config.h. 

This example project can be used with another design using a different hardware
configurations. This can be achieved by overwriting the content of this example
project's "fpga_design_config.h (hw_config.h)" file with the correct data from
your Libero design.

Note: The Polarfire Eval Kit base design for this project is currently not
      available on GitHub [Mi-V-Soft-RISC-V.](https://mi-v-ecosystem.github.io/redirects/repo-polarfire-evaluation-kit-mi-v-sample-fpga-designs)

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

This example is tested on PolarFire Evaluation Kit (MPF300T-EVAL-KIT).
