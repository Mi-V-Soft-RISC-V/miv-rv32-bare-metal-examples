# CoreTSE receive and transmit packet application

This example project demonstrates the use of CoreTSE driver APIs implementing a
transmit and receive packet application.

There are two different build configurations provided with this project which
configure this SoftConsole project for RISC-V IMC instruction extension.
The following configurations are provided with the example.

- miv-rv32-imc-debug
- miv-rv32-imc-release

## How to use this example

- Connect the board to a DHCP capable switch or network.
- Connect via the board's USB-UART serial cable to a host PC running a terminal
  emulator such as TeraTerm or PuTTY configured as follows:
   - 115200 baud
   - 8 data bits
   - 1 stop bit
   - no parity
   - no flow control
- Run the example project using a debugger. A greeting message will appear over
  the UART terminal followed by a menu system and instructions.

The software will auto-negotiate an Ethernet link with the network's switch.
The terminal emulator command line interface will display (through UART) the
status of the link, transmit packet success and receive packet size.

## fpga_design_config (formerly known as hw_config.h)

The SoftConsole project targeted for Mi-V processors now use an improved directory
structure.
The `fpga_design_config.h` must be stored as shown below:

`<project-root>/boards/<board-name>/fpga_design_config/fpga_design_config.h`

Currently, this file must be hand crafted when using the Mi-V Soft Processor.
In future, all the design and soft IP configurations will be automatically generated
from the Libero design description data.

You can use the sample file provided with MIV_RV32 HAL as an example.
Rename it from `sample_fpga_design_config.h` to `fpga_design_config.h` and then customize
it per your hardware design such as `SYS_CLK_FREQ`, peripheral BASE addresses, interrupt
umbers, definition of `MSCC_STDIO_UART_BASE_ADDR` if you want a CoreUARTapb mapped
to STDIO, etc.

## Target hardware

This project has been tested on the following board with the MIV_RV32 IMC Core:

- PolarFire Evaluation Kit (MPF300-EVAL-KIT)
  
The PolarFire Evaluation Kit uses Microchip's VSC8575 PHY.
The driver for this PHY can be found in the CoreTSE driver directory, `src/platform/drivers/fpga_ip/CoreTSE`.

This project uses a custom hardware design that can be found in the
[Application Note: AN4569][1].

[1]: https://www.microchip.com/en-us/application-notes/an4569

## Modifying this example to target other boards or interfaces

Refer to the `src/application/config.h` to configure a the project to target a different
board.
The macro definitions in this file will set the correct PHY driver and clock configuration
settings based on the selected board.

The `main.c` file can be used to further modify the CoreTSE configuration for new
boards.
The following code in that file configures the MAC:

``` {c}
    TSE_cfg_struct_def_init(&g_tse_config);

    g_tse_config.phy_addr = 0;
    g_tse_config.speed_duplex_select = TSE_ANEG_ALL_SPEEDS;
    g_tse_config.mac_addr[0] = mac_address[0];
    g_tse_config.mac_addr[1] = mac_address[1];
    g_tse_config.mac_addr[2] = mac_address[2];
    g_tse_config.mac_addr[3] = mac_address[3];
    g_tse_config.mac_addr[4] = mac_address[4];
    g_tse_config.mac_addr[5] = mac_address[5];
    g_tse_config.framefilter = TSE_FC_PROMISCOUS_MODE_MASK;
```

You only need to modify these files to use a different PHY or force the link to
use a specific speed.

## Silicon revision dependencies

This example is tested on PolarFire Evaluation Kit (MPF300-EVAL-KIT).
