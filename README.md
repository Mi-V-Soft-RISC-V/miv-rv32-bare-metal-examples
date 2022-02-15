# Mi-V Soft processor Bare Metal Examples
This repository provides bare metal embedded software example projects built with Microchip's SoftConsole IDE.

```
<repo>
  |-- applications
  |       |- .
  |       |- .
  |
  |
  |-- driver-examples
  |       |- CoreGPIO
  |       |- CoreUARTapb
  |       |     .
  |       |     .
  |       |     .
  |       |- CoreSysServices-PF
  |       |- miv-i2c
  |       |- miv-watchdog
  |       |     .
  |       |     .
  |       |     .

```

The *applications* directory contains applications that are ready for use with selected hardware platforms.

The *driver-examples* directory contains example projects demonstrating the Soft-IP device drivers. These examples serve as an easy starting point for the users to start exploring FPGA devices.

Each project provides a README.md explaining its functionality and usage instructions.
All projects provide a set of ready to use build configurations and debug launch configurations. They are described in the following sections.

## Build configurations
The build configurations configure the projects to build in a specific way. For example, Optimization level -Os, linking to a specific memory address etc..
Below is the list of the common build configurations provided by default with each bare metal driver example project. You may use these build configurations or create your own to suit your project.

|Configuration     | Description    |
|----------------- | -------------- |
|miv32imc-Debug    | Targeted for the Mi-V soft processor configured with RV32I base ISA + M and C extension. </br> Links to MIV_RV32 default reset vector address (0x80000000). </br> Uses peripheral memory map as provided by MIV_ESS  </br> Not-optimized (-O0).|
|miv32imc-Release  | Targeted for the Mi-V soft processor configured with RV32I base ISA + M and C extension. </br> Links to MIV_RV32 default reset vector address (0x80000000). </br> Uses peripheral memory map as provided by MIV_ESS  </br> Optimized (-Os).|

Note that some of the example projects provided in the *miv-rv32-bare-metal-examples* repository may contain additional or different build configs to suit the functionality. Refer the example projects for details.

A build configuration is a particular combination of SoftConsole project settings.
The following sections describe the configurations provided by the default build configurations.

### SoftConsole project settings
Some of the important project settings are explained in following sections.

#### Debug build configurations
The Debug configuration is intended for the early development and debug phase of the project. By convention, the *-Debug configurations use optimization level -O0 and generate maximum debug symbol information.

#### Release build configurations
The Release configuration is intended for the final production release, where an executable stored in non-volatile memory runs after power-on-reset. By convention, the *-Release configurations use higher optimization level (-Os) and do not generate debug symbol information. It also defines a NDEBUG macro which is used to exclude any debug code from the build.

**Linker scripts:** Each build configuration needs a linker script. The linker script describes the memory layout of the executable. Each build configuration selects an appropriate linker scripts via project settings. For example, miv32imc-Release uses miv-rv32-ram-imc.ld

There are several other settings that are required for a project. For complete project settings go to \<project-name>->Properties->settings->Tool settings.

### Settings via header files
Apart from the SoftConsole project settings, each project needs a few more configurations. These configurations are categorized into hardware and software configurations.

#### Hardware configurations
The hardware configurations are located in the \<project-root>/src/boards/\<target-board> folder. The include files in the \<project-root>/src/boards/\<target-board>/fpga_design_config folder define the hardware configurations such as clocks and base addressess. You must make sure that the configurations in this example project match the actual configurations of the Libero&reg; design that you are using to test this example project.

To choose a particular hardware configuration, include an appropriate \<project-root>/src/boards/\<target-board> folder path via the SoftConsole project settings.

#### Software configurations
Software configurations may be required for the correct functioning of the MIV_RV32 HAL. The README.md in each example project describes the software configurations that are required for that project.

## Debug launchers
The following two pre-configured debug launchers are provided with each project.

|Configuration              | Description                                                                                                |
|---------------------------|------------------------------------------------------------------------------------------------------------|
|_\<project name> hw debug.launch_ | Intended to be used with *-Debug configurations. Resets soft processor.<br> Downloads the executable and the symbols to the memory. Sets up PC to start location. |
|_\<project name> hw attach.launch_ | Intended to be used with *-Release configurations. Does not trigger reset. <br> Downloads only the symbol information and attaches to processor. Typically used to check the current state of harts. If the project is linked to RAM memory such as LSRAM, you will be able to put breakpoints and step debug further.                   |

Both launchers are configured to use the currently _active_ build configuration, hence the same launcher can be used with any of the build configurations. Make sure that an appropriate build configuration is set as _active_ to avoid issues. Trying to use <*> attach.launch_ while a *-Debug build is _active_ may not work.

You may change the existing debug launchers or create your own launcher to suit your project.

## Target hardware
Please refer README.md in each project for details.

### Further reading
[Mi-V soft CPUs](https://mi-v-ecosystem.github.io/docs/mi-v-soft-cpu/#mi-v-soft-cpus)

[Hardware platforms and boards](https://mi-v-ecosystem.github.io/docs/mi-v-soft-cpu/#hardware-platforms-and-boards)
___
Libero&reg; and PolarFire&reg; are the property of Microchip Technology Inc. (MCHP).
