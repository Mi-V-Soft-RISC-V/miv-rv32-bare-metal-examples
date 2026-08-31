# CoreQSE Example

This example project demonstrates the use of CoreQSE driver APIs. CoreQSE is an
IEEE 802.3-2022 Ethernet MAC with an integrated Physical Coding Sublayer (PCS).

For complete details of the CoreQSE IP, its register map, configuration
parameters and reference designs, refer to the CoreQSE User Guide:

[CoreQSE User Guide](https://www.microchip.com/en-us/products/fpgas-and-plds/ip-core-tools/coreqse)

The application is a self-checking validation test. Every check prints PASS,
FAIL or SKIP and the run ends with a numeric summary.

The following build configuration is provided:

  - miv32i-Debug

## Mi-V Soft Processor

This example uses the Mi-V Soft Processor MIV_RV32. The design is built for
debugging MIV_RV32 through the PolarFire FPGA programming JTAG port using a
FlashPro5. CoreJTAGDebug is used to connect to the JTAG port of MIV_RV32.

All platform and design specific definitions such as peripheral base addresses
and system clock frequency are in fpga_design_config.h.

## Selecting the Target Design

CoreQSE has two reference designs, 1 Gbps and 2.5 Gbps. They differ in topology,
not only in link rate, so the application must be built for one or the other.
See the user guide for the design details.

Select the target in fpga_design_config.h:

```c
#define BOARD_DESIGN   BOARD_COREQSE_2_5G     /* or BOARD_COREQSE_1G */
```

Or pass it on the command line, which avoids editing the file when switching:

```
-DBOARD_DESIGN=BOARD_COREQSE_2_5G
-DBOARD_DESIGN=BOARD_COREQSE_1G
```

The binary **must** match the programmed bitstream, because the two designs
place CoreQSE at different base addresses. The application prints its build
configuration at startup and checks CORE_VER before running any test, so a
mismatch is reported rather than producing misleading results.

## Drivers Used

| Driver | Purpose | 2.5 Gbps | 1 Gbps |
| --- | --- | --- | --- |
| CoreQSE | MAC and PCS configuration, statistics | yes | yes |
| CoreAXI4ProtoConv | moves frame data between memory and stream | yes | absent |
| CoreMDIO | external PHY management | absent | yes |
| CoreGPIO | DMA completion interrupts | yes | absent |
| CoreUARTapb | console | yes | yes |

CoreQSE has **no** MII management hardware of its own. PHY access is performed
by a separate COREMDIO_APB IP, presented to the application through the
`QSE_phy_*` functions in core_qse.h.

## Test Setup

Console settings for both designs: **115200 baud, 8 data bits, 1 stop bit, no
parity, no flow control**.

### 2.5 Gbps

| Step | Action |
| --- | --- |
| 1 | Program the CoreQSE_2_5Gbps bitstream |
| 2 | Fit the SMA loopback cable between the transceiver TX and RX SMA connectors |
| 3 | Set `BOARD_DESIGN` to `BOARD_COREQSE_2_5G` |
| 4 | Connect the USB-UART cable, console at 0x71000000 |
| 5 | Run using a debugger |

The SMA cable is required. Without it the transceiver has no CDR lock, the
receive path stays in reset, and no frame is received.

Runs unattended.

### 1 Gbps

| Step | Action |
| --- | --- |
| 1 | Program the CoreQSE_1Gbps bitstream |
| 2 | Fit the SGMII daughter board to the FMC connector |
| 3 | Connect the RJ45 cable to **port 0, connector J15, on the daughter board** |
| 4 | Set the far end port to 1000BASE-T full duplex, auto-negotiation enabled |
| 5 | Set `BOARD_DESIGN` to `BOARD_COREQSE_1G` |
| 6 | Connect the USB-UART cable, console at 0x61000000 |
| 7 | Run using a debugger |

Use the RJ45 on the daughter board, **not** either socket on the Evaluation Kit.
The transceiver lanes route to the FMC connector.

Tests 1 to 4 run unattended. Test 5 prompts: start traffic on the external test
set, then press any key.

The RJ45 LEDs are driven by vendor specific PHY LED registers that this driver
does not program, so they may stay dark even with the link up. The PHY status
register reported by the application is authoritative.


## fpga_design_config (formerly known as hw_config.h)

The SoftConsole project targeted for Mi-V processors uses an improved directory
structure. The fpga_design_config.h must be stored as shown below:

`
    <project-root>/boards/<board-name>/fpga_design_config/fpga_design_config.h
`

This file must be hand crafted when using the Mi-V Soft Processor. Customize it
per your hardware design: SYS_CLK_FREQ, peripheral base addresses, the design
selection macro and the PHY selection macro.

## Target Hardware

This example targets the Mi-V Soft Processor MIV_RV32 on a PolarFire Evaluation
Kit. The 1 Gbps design additionally requires the SGMII daughter board on the FMC
connector.

Ensure that fpga_design_config.h matches the design. This example can be used
with a different hardware configuration by overwriting the contents of that file
with the correct data from your Libero design.

### Booting the System

The example project is configured to use the FlashPro debugger to execute from
LSRAM.

## Silicon Revision Dependencies

This example is tested on the PolarFire Evaluation Kit (MPF300T-EVAL-KIT) with
CoreQSE v2.0.101.
