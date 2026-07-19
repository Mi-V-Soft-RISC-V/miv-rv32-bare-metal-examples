
# CoreI3C Controller Write/Read Example

This example project demonstrates the use of the CoreI3C Soft IP in controller
mode with a Mi-V soft processor. It exercises Dynamic Address Assignment (DAA),
private write and read transfers.

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

This example project is targeted at a MIV_RV32 design running on a
PolarFire-Eval-Kit connected via a USB-UART serial cable to a host PC running
a terminal emulator such as TeraTerm or Putty configured as follows:

 - 57600 baud
 - 8 data bits
 - 1 stop bit
 - no parity
 - no flow control

Run the example project using a debugger. A greeting message will appear over
the UART terminal followed by the menu below:

```
 ---- I3C Bus Menu ----------------------------------------
  1 - Run DAA (discover targets)
  2 - Write bytes to target
  3 - Read bytes from target
 ----------------------------------------------------------

```

### Menu option 1 — Run DAA

Issues the ENTDAA broadcast command. Each responding I3C target is assigned a
7-bit dynamic address. The dynamic address, BCR, and DCR of each discovered
target are printed on the console. Run this option once at startup before using
options 2, or 3.

### Menu option 2 — Write bytes to target

Prompts for a target dynamic address (decimal) and a payload as hex pairs
(e.g. `DEADBEEF`). The driver writes the payload to the target via a private
write transfer and prints the response descriptor.

### Menu option 3 — Read bytes from target

Prompts for a target dynamic address and byte count. The driver reads the
requested number of bytes and prints them as hex on the console.

## Interrupt Handling

Initial version supports only polling mode.

## fpga_design_config (formerly known as hw_config.h)

The SoftConsole project targeted for Mi-V processors now use an improved
directory structure. The fpga_design_config.h must be stored as shown below:

```
<project-root>/boards/<board-name>/fpga_design_config/fpga_design_config.h
```

Currently, this file must be hand crafted when using the Mi-V Soft Processor.
In future, all the design and soft IP configurations will be automatically
generated from the Libero design description data.

Customize the following defines per your Libero design:

| Define                  | Description                                |
|-------------------------|--------------------------------------------|
| `SYS_CLK_FREQ`          | Mi-V processor clock frequency (Hz)        |
| `COREUARTAPB0_BASE_ADDR`| CoreUARTapb base address                   |
| `COREI3C_BASE_ADDR`     | CoreI3C base address (default 0x44000000)  |

Also update the SCL timing constants in `main.c` to match your clock:

```c
#define SCL_OD_HIGH_CYCLES   (4u)   /* ACLK cycles for open-drain HIGH */
#define SCL_OD_LOW_CYCLES    (10u)  /* ACLK cycles for open-drain LOW  */
#define SCL_PP_HIGH_CYCLES   (2u)   /* ACLK cycles for push-pull HIGH  */
#define SCL_PP_LOW_CYCLES    (2u)   /* ACLK cycles for push-pull LOW   */
```

Example for 50 MHz ACLK targeting ~3.5 Mbps OD / 12.5 Mbps PP (default).

## Target Hardware

This example is targeted at the latest Mi-V Soft Processor MIV_RV32 instantiated
in a PolarFire Evaluation Kit design with CoreI3C instantiated in the fabric.

The CoreI3C INT_O pin must be connected to MIV_RV32 MSYS_EI[0].

Choose the build configurations per the supported ISA extensions that you want
to use.

Note: The PolarFire Eval Kit base design for this project is currently not
available on GitHub [Mi-V-Soft-RISC-V](https://mi-v-ecosystem.github.io/redirects/repo-polarfire-evaluation-kit-mi-v-sample-fpga-designs).

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
