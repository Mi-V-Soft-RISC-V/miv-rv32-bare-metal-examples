
# System timer interrupt example on Mi-V soft processors

This project demonstrates the Mi-V soft processor system timer interrupt functionality.
This is a minimal project which can be used with MIV_RV32 as well as the legacy RV32
soft processors.

There are 6 different build configurations provided with this project which configure
this SoftConsole project for different RISC-V instruction extension combinations.

The Following configurations can be used with any of the Mi-V soft processors
since they use only the base instruction set RV32I.
 - miv32i-Debug
 - miv32i-Release

**Note:** These configurations use the peripherals mapped with memory map as
supported by the MIV_ESS. The legacy RV32 processor designs use a different
peripheral map. You must update it if you are using these configurations on
legacy cores.

Refer:
'
    <project-root>/boards/<board-name>/miv-rv32-design/fpga_design_config/fpga_design_config.h
'

The following configurations can be used with the corresponding Mi-V soft processors
on which the respective instruction extensions are enabled. You must make sure
that the Mi-V IP core configurations and the SoftConsole project build configuration
that you are using are compatible with each other.

 - miv32ima-Debug
 - miv32ima-Release
 - miv32imc-Debug
 - miv32imc-Release

## fpga_design_config (formerly known as hw_config.h)
The SoftConsole project targeted for Mi-V processors now use an improved
directory structure. Detailed description of the folder structure is available
at https://github.com/Mi-V-Soft-RISC-V/miv-rv32-documentation.
The fpga_design_config.h must be stored as shown below


    <project-root>/boards/<board-name>/<design-name>/fpga_design_config/fpga_design_config.h


Currently, this file must be hand crafted when using the Mi-V Soft Processor.
In future, all the design and soft IP configurations will be automatically
generated from the Libero design description data.

You can use the sample file provided with MIV_RV32 HAL as an example.
Rename it from sample_fpga_design_config.h to fpga_design_config.h and then
customize it per your hardware design such as SYS_CLK_FREQ, peripheral
BASE addresses, interrupt numbers, definition of MSCC_STDIO_UART_BASE_ADDR if
you want a CoreUARTapb mapped to STDIO, etc.

```
 **NOTE:**
 In the legacy folder structures, the hw_config.h  at the root of the project
 folder was used to store these configurations. This file is now **DEPRECATED**.

 The MIV_RV32 HAL still supports the legacy folder structure with hw_platform.h.
 if you define LEGACY_DIR_STRUCTURE in your project settings.

```
## Linker script changes

You must make sure that in addition to the reset vector, all other memory address
configurations in the Mi-V soft processor match with the memory layout defined
in the linker script.

The MIV_RV32 cores provide Tightly Coupled Memory (TCM), it's start address is
configurable (maximum size = 256k). You can also download and execute code
from the TCM. In this case you must make sure that the linker script is updated
according to the TCM configurations in your IP core.

## MACROs for conditional compilation

### MIV_LEGACY_RV32

Define this macro in the project settings (assembler and compiler) when you are
using one of the legacy RV32 cores (any combination of the supported ISA extension).

### MIV_LEGACY_RV32_VECTORED_INTERRUPTS

Enable this macro only in the **assembler properties** when you want to use
vectored interrupts on legacy RV32 cores (any combination of the supported ISA extension).

`
C/C++ Build > Settings > Tool Settings > GNU RISC-V Cross Assembler > Preprocessor > Defined Symbols
`

By default, all interrupts on legacy RV32 cores are treated as non-vectored interrupts.


### MTIME and MTIMECMP

MIV_RV32 core offers flexibility in terms of generating MTIME and MTIMECMP
registers internal to the core or using external time reference.
There are following 4 possible combinations:

1. _Internal MTIME_ and _Internal MTIME IRQ_ enabled
    Generate the MTIME and MTIMECMP registers internally.
    (This is the only combination available on legacy RV32 cores).

2. _Internal MTIME_ enabled and _Internal MTIME IRQ_ disabled
    Generate the MTIME internally and have a timer interrupt input to the core
    as external pin.  
    In this case 1 pin port will be available on MIV_RV32 for timer interrupt.

3.  _Internal MTIME_ disabled and _Internal MTIME IRQ_ enabled
    Generate the time value externally and generate the mtimecmp and interrupt
    internally.
    (for example a multiprocessor system with a shared time between all cores)
    In this case a 64 bit port will be available on the MIV_RV32 core as input.

4.  _Internal MTIME_ and _Internal MTIME IRQ_ disabled
    Generate both the time and timer interrupts externally.
    In this case a 64 bit port will be available on the MIV_RV32 core as input.
    and a 1 pin port will be available for timer interrupt.

To handle all these combinations in the firmware, the following constants must be
defined in accordance with the configuration that you have made on your
MIV_RV32 core design.

### MIV_RV32_EXT_TIMER
 When defined, it means that the MTIME register is not available internal to the
 core. In this case a 64 bit port will be available on the MIV_RV32 core
 as input. When this macro is not defined, it means that the MTIME register is
 available internal to the core.

### MIV_RV32_EXT_TIMECMP
 When defined, it means the MTIMECMP register is not available internally to the
 core and the Timer interrupt input to the core comes as an external
 pin. When this macro is not defined it means the that MTIMECMP register exists
 internal to the core and that the timer interrupt is generated internally.

**NOTE:**
All these macros must **not** be defined if you are using a MIV_RV32 core.

## Target hardware

This example project can be targeted to Mi-V designs available at
https://github.com/Mi-V-Soft-RISC-V


## Mi-V soft processor revision dependencies

The MIV_RV32 HAL v3.0 (and later) supports _MIV_RV32_ as well as the _legacy RV32 IP cores_.

The term _MIV_RV32_ represents following two cores
 - MIV_RV32 v3.0 and later             (Latest and greatest Mi-V soft processor)

 - MIV_RV32IMC v2.1                    (MIV_RV32 v3.0 is a drop in replacement
 for this core. It is recommended to migrate your design to MIV_RV32 v3.0)

The term _legacy RV32 IP cores_ represents following IP cores
 - MiV_RV32IMA_L1_AHB
 - MIV_RV32IMA_L1_AXI
 - MiV_RV32IMAF_L1_AHB

(These legacy RV32 IP cores are **deprecated**. It is **recommended** that
you migrate your designs to MIV_RV32 v3.0 or later for latest enhancements, bug
fixes and support.)
