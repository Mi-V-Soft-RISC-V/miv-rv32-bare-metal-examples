
# Mi-V Soft processor FreeRTOS LwIP Webserver Example

This project implements a example webserver application using the lwIP
TCP/IP stack and FreeRTOS operating system for the PolarFire Evaluation Kit built
with Microchip's SoftConsole IDE.

## Mi-V Soft Processor

This example uses Mi-V SoftProcessor MIV_RV32.The design is built for debugging
MIV_RV32 through the PolarFire FPGA programming JTAG port using a FlashPro5.
To achieve this the CoreJTAGDebug IP is used to connect to the JTAG port of the
MIV_RV32.

All the platform/design specific definitions such as peripheral base addresses,
system clock frequency etc. are included in `fpga_design_config.h.`.

The Mi-V Soft Processor MIV_RV32 firmware projects needs the MIV_RV32 HAL and
the hal firmware (RISC-V HAL).

The HAL is available at GitHub [Mi-V-Soft-RISC-V][1].

[1]: https://mi-v-ecosystem.github.io/redirects/platform

## How to use this example

### Required Hardware

- [PolarFire FPGA Evaluation Kit][2]

### Steps

- Open a terminal connection to the board's serial port with a USB-UART cable connected
  to the host PC.
  Configure the connection as follows:
   - 115200 baud
   - 8 data bits
   - 1 stop bit
   - no parity
   - no flow control
- Connect the board to a DHCP capable switch or network.
   - Use an ethernet cable connected to the Ethernet Connector RJ45 - Port 0

The program will print status messages to the UART terminal.

When the program has set up the webserver it will print the IP address to the UART
terminal.
Open this address in an internet browser to interact with the webserver.

[2]: https://www.microchip.com/en-us/development-tool/mpf300-eval-kit

## Interacting with the Webserver

The webserver will display receive and transmit statistics for the CoreTSE ethernet
MAC module in the FPGA design.

The webserver can be interacted with in various ways:

- Navigate to the `/m` directory to view the Microchip logo.
- Navigate to the `/s` directory to download a copy of the Rx-Tx statistics.

## Build configuration

Two default build configurations are provided for this project:

| Configuration    | Description                                                                                                                                    |
| ---------------- | ---------------------------------------------------------------------------------------------------------------------------------------------- |
| miv-rv32-Debug   | Targeted for the Mi-V soft processor configured with RV32I base ISA + M and C extensions. Not-optimized (-O0). Can be used with any Mi-V RV32  |
| miv-rv32-Release | Targeted for the Mi-V soft processor configured with RV32I base ISA + M and C extensions. Optimized size (-Os). Can be used with any Mi-V RV32 |

You can create your own build configuration to match the specific needs for your
project by going to the Project Explorer tab in SoftConcole, right clicking on
the project's name, choosing Properties and then going to C/C++ build->Settings.

### Linker scripts

Each build configuration needs a linker script to describe the memory layout of
the executable.
An appropriate linker script is set for the build configuration in the project settings.
The linker script, located in `/<project-root>/src/boards/<target-board>/`, must
match the Libero&reg; design for the target board.

## Target Hardware

This example project is targeted at the application note AN4569 design.

The design is available from the Microchip website at the following location:
[Application Note: AN4569][3].

All the design specific definitions such as peripheral base addresses, system
clock frequency etc. are included in `fpga_design_config.h`.

The firmware projects needs the HAL and the MIV_RV32 HAL firmware components.

This example project can be implemented with another design using a different hardware
configuration. This can be achieved by overwriting the content of this example
project's `fpga_design_config.h` file with the correct data per your Libero design.

[3]: https://www.microchip.com/en-us/application-notes/an4569

## Software Configuration

### Settings via header files

The project can be configured in other ways aside from the SoftConsole project settings.

Depending on your application requirements you might want to change the FreeRTOS
configuration.
This is done by modifying the `/<project-root>/src/application/FreeRTOSConfig.h`
file.
For further information, see [FreeRTOS customisation][4].

To configure the LwIP TCP/IP stack some of the following files can be edited
depending on your project requirements:

- `lwipopts.h`: is contains settings used to configure the behaviour and features
  of the LwIP stack, it is located in `middleware/lwip/lwipopts.h`

- `cc.h`: is a basic header that describes your compiler and processor to lwIP,
  it is located in `middleware/lwip/port/freertos/include/arch/cc.h`

- `sys_arch.c`: The sys_arch provides semaphores and mailboxes to lwIP, it is
  located in `middleware/lwip/port/freertos/sys_arch.c`

For further information on how to fully configure LwIP refer to the [LwIP wiki][5].

[4]: https://www.freertos.org/a00110.html
[5]: https://lwip.fandom.com/wiki/LwIP_Wiki

## Silicon Revision Dependencies

This example is tested on PolarFire MPF300T device.
