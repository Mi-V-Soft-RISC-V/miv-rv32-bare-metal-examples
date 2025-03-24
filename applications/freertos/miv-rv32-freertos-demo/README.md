# Mi-V Soft processor FreeRTOS Examples

This repository provides a FreeRTOS demonstration project targeted for the PolarFire
Evaluation Kit built with Microchip's SoftConsole IDE.

```markdown
<repo>
  |-- src
  |       |- applications
  |       |-- boards
  |       |     |- polarfire-eval-kit
  |       |- middleware
  |       |     |- FreeRTOS
  |       |- platform

```

- *applications* contains applications that are ready for use with
the provided design for the PolarFire Evaluation Kit.

- *boards* contains the files, such as the linker script, that needed to configure
  the PolarFire Evaluation Kit so the FreeRTOS demo run correctly.

- *middleware/FreeRTOS* contains the FreeRTOS kernel, as well as the
files defining the tasks used in the full demo.

- *platform* contains the Mi-V HAL and the bare metal drivers.

## Build configuration

Four default build configurations are provided for this project:

| Configuration         | Description                                                                                                                                    |
| --------------------- | ---------------------------------------------------------------------------------------------------------------------------------------------- |
| miv-rv32-Debug        | Targeted for the Mi-V soft processor configured with RV32I base ISA + M and C extensions. Not-optimized (-O0). Can be used with any Mi-V RV32  |
| miv-rv32-Release      | Targeted for the Mi-V soft processor configured with RV32I base ISA + M and C extensions. Optimized size (-Os). Can be used with any Mi-V RV32 |
| legacy-rv32imaf-Debug | Targeted for the legacy cores configured with RV32I base ISA + M, A, and F extensions. Not-optimized (-O0)                                     |
| legacy-rv32imaf-Debug | Targeted for the legacy cores configured with RV32I base ISA + M, A, and F extensions. Optimized size (-Os)                                    |

You can create your own build configuration to match the specific needs for your
project by going to the Project Explorer tab in SoftConcole, right clicking on
the project's name, choosing Properties and then going to C/C++ build->Settings.

**Linker scripts:** Each build configuration needs a linker script to describe the
memory layout of the executable.
An appropriate linker script is set for the build configuration in the project settings.
The linker script, located in `/<project-root>/src/boards/<target-board>/`, must
match the Libero&reg; design for the target board.

### Settings via header files

Beside the SoftConsole project settings, a project needs a few additional
configurations, these are categorised into hardware and software configurations.

#### Hardware configurations

The hardware configuration file is the
`/<project-root>/src/boards/<board>/fpga_design_config/fpga_design_config.h`
file.
This is where you define the hardware configuration such as clock, MTIME prescaler
(under the name TICK_COUNT_PRESCALER) and the peripherals memory map.
It is also in this file that you must specify if the project is running on Renode
or on hardware.
You must make sure that the configuration in this example project matches the
actual configurations of the Libero&reg; design that you are using to test this
example project.

To choose a particular hardware configuration, include an appropriate
`/<project-root>/src/boards/<board>/`
folder path via the SoftConsole project settings.

#### Software configurations

A different software configuration might be required for your project to run correctly.

Depending on your application requirements you might want to change the FreeRTOS
configuration.
This is done by modifying the
`/<project-root>/src/application/FreeRTOSConfig.h` file.
For further information, see [FreeRTOS customisation][1]:.

[1]:https://www.freertos.org/a00110.html

## Libero Design

The FreeRTOS demo targets a modified version of CFG1 of the
[PolarFire Evaluation Kit Mi-V Sample FPGA Designs][2].

The size of the SRAM block connected to the MiV-RV32 processor core needs to be
increased to *256kB* (i.e. *65536 words*) for MiV-RV32 when the C extension is on,
and to *512kB* (i.e. *131072 words*) for legacy MiV of for MiV-RV32 when the C
extension is off.

The SRAM (PF_SRAM_AHB) block is connected to the MiV-RV32 over an AHB interface.
To increase the size of the SRAM, right click on the PF_SRAM_AHB, select "Configure".
In the window that pops up select the "Port settings" tab and change the value
of the "Memory Depth" field to 256000 or 512000 as appropriate.

[2]:https://mi-v-ecosystem.github.io/redirects/repo-polarfire-evaluation-kit-mi-v-sample-fpga-designs

## Running FreeRTOS on Renode emulation platform

The FreeRTOS demo can be used on a physical board or with the Renode emulation
platform.
To use the provided Renode debug configurations, you will need to use
**SoftConsole version 2022.2-782** or later.

### Software configuration

To use the demo on Renode, the `RENODE_SIMULATION` macro must be set to 1 in the
`fpga_design_config.h` file.
This step is needed because the MiV core has the `MTIME` prescaler, which divides
the CPU clock to generate the `MTIME`.
The FreeRTOS kernel generates its own clock by generating tick interrupts and
incrementing a tick count at a given frequency.
This tick interrupt being based on the `MTIME`, the prescaler has to be considered
when setting the tick frequency
(see `src/middleware/FreeRTOS/source/portable/GCC/RISC-V/port.c`).

Nevertheless, there is no such prescaler in the Renode model.
Setting the `RENODE_SIMULATION` macro to 1 will adapt the tick frequency for the
platform.
You should also check that the definition of the macro TICK_COUNT_PRESCALER matches
the MTIME prescaler in the Libero design.

### Start a Renode session

1. Compile the project with the ***Release*** configuration of the design you want
2. Right click on *miv-rv32-freertos renode-rv32i start-platform-and-debug.launch*
 (respectively *miv-rv32-freertos renode-legacy-rv32 start-platform-and-debug.
 Launch* if you want to emulate the legacy design), select "Debug as" and
 "miv-rv32-freertos renode-rv32i start-platform-and-debug" (respectively
 *miv-rv32-freertos renode-legacy-rv32 start-platform-and-debug.launch*)
3. You can then use the usual debug tools, such as breakpoints, steps, etc.
4. You should see either a "blink" message or a dot appearing in the Renode window,
 depending on if you use the simple blinky demo or the full demo

### Further reading

[Mi-V soft CPUs](https://mi-v-ecosystem.github.io/docs/mi-v-soft-cpu/#mi-v-soft-cpus)

[Future Avalanche board](https://mi-v-soft-risc-v.github.io/Future-Avalanche-Board/)
___
Libero&reg; and PolarFire&reg; are the property of Microchip Technology Inc. (MCHP).
