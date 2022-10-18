# Mi-V Soft processor FreeRTOS Examples
This repository provides a FreeRTOS demonstration project for the Future Avalanche board built with Microchip's SoftConsole IDE.

```
<repo>
  |-- src
  |       |- applications
  |       |-- boards
  |       |     |- avalanche-board
  |       |- freertos-source
  |       |- platform

```

The *applications* directory contains applications that are ready for use with the provided design for the Future Avalanche board.

The *boards* directory contains the files needed to configure the Future Avalanche board to make the FreeRTOS demo run correctly.

The *freertos-source* directory contains the FreeRTOS kernel, as well as the files defining the tasks used in the full demo.

The *platform* directory contains the HAL and the drivers.

## Build configuration
Four default build configurations are provided for this project:

|Configuration             | Description                                                                                                  |
|------------------------- | ----------------------------------------------------------------------------------------------------------   |
|miv-rv32-Debug              | Targeted for the Mi-V soft processor configured with RV32I base ISA + M and C extensions. Not-optimized (-O0). Can be used with any Mi-V RV32 |
|miv-rv32-Release              | Targeted for the Mi-V soft processor configured with RV32I base ISA + M and C extensions. Optimized size (-Os). Can be used with any Mi-V RV32 |
|legacy-rv32imaf-Debug              | Targeted for the legacy cores configured with RV32I base ISA + M, A, and F extensions. Not-optimized (-O0)|
|legacy-rv32imaf-Debug              | Targeted for the legacy cores configured with RV32I base ISA + M, A, and F extensions. Optimized size (-Os)|

You can create your own build configuration to match the specific needs for your project by going to the Project Explorer tab in SoftConcole, right clicking on the project's name, choosing Properties and then going to C/C++ build->Settings.

**Linker scripts:** A build configuration needs a linker script to describe the memory layout of the executable. Each build configuration selects an appropriate linker scripts via the project settings. The linker script for each configuration in \<project-root>/src/boards/avalanche-board/\<configuration-name>/platform_config\linker matches the Libero&reg; design for the Avalanche kit, located in \<project-root>/src/boards/avalanche-board\libero_design.

### Settings via header files
Beside the SoftConsole project settings, a project needs few additionnal configurations. These configurations are categorized into hardware and software configurations.

#### Hardware configurations
The hardware configuration file is the \<project-root>/src/boards/avalanche-board/\<configuration-name>/fpga_design_config/fpga_design_config.h file. This is where you define the hardware configuration such as clock, MTIME prescaler (under the name TICK_COUNT_PRESCALER) and the peripherals memory map. It is also in this file that you must specify if the project is running on Renode or on hardware. You must make sure that the configuration in this example project matches the actual configurations of the Libero&reg; design that you are using to test this example project.

To choose a particular hardware configuration, include an appropriate \<project-root>/src/boards/avalanche-board folder path via the SoftConsole project settings.

#### Software configurations
A different software configuration might be required for your project to run correctly.

According to your project, you might want to change the FreeRTOS configuration. This is done by modifying the \<project-root>/src/applications/FreeRTOSConfig.h file. For further information, see [FreeRTOS customisation](https://www.freertos.org/a00110.html).

## Libero Design

The FreeRTOS demo targets the 2022.1-v1.0 release of MiV for the Avalanche board. The base design of soft CPU for PolarFire FPGA can be found [here](https://mi-v-ecosystem.github.io/docs/mi-v-soft-cpu/#mi-v-soft-cpus). If you are going to build the 2022.1-v1.0 release of the Libero&reg; project from [that GitHub repository](https://mi-v-ecosystem.github.io/docs/mi-v-soft-cpu/#mi-v-soft-cpus), you are going to need **Libero&reg; 2022.1** or later installed. Nonetheless, the base design needs to be modified to be able to run the FreeRTOS demo.

The size of the RAM must be increased to *256kB* (i.e. *65536 words*) for MiV-RV32 when the C extension is on, and to *512kB* (i.e. *131072 words*) for legacy MiV of for MiV-RV32 when the C extension is off.

When using the MiV-RV32 base design, two CoreTimers must be added to the design. 
To add them, go to the *Catalogue* tab in Libero, type "CoreTimer" in the IP Catalog search bar, and drag two CoreTimers to the BaseDesign SmartDesign window.
You then need to connect them. To do so: 
1. Connect the CoreTimer's *PCLK* input to the *OUT0_FABCLK_0* output of the Clock Conditioning Circuitry (CCC).
2. Connect the CoreTimer's *PRESETn* input to the *FABRIC_RESET_N* output of the CoreReset block.
3. Double-click on the MiV-ESS component, go to the APB tab, and enable the APB slots 3 and 4. You can then connect each CoreTimer's APB input to one of these APB slots from MiV-ESS.
4. Connect the output of the CoreTimer connected to the MiV_ESS's APB slot 3 to the MSYS_EI[0] input of the MiV_RV32.
5. Connect the output of the CoreTimer connected to the MiV_ESS's APB slot 4 to the EXT_IRQ input of the MiV_RV32.
6. Generate the component and follow the usual Libero design flow.

To avoid adding the previous changes by hand, you can simply execute the tcl script provided under ..\miv-rv32-freertos-applications\miv-rv32-freertos-demo\src\boards\avalanche-board\miv-rv32-design\fpga_design\libero design. To do so, open Libero&reg;, go to Project->Execute Script.., select the PF_Avalanche_MIV_RV32_BaseDesign.tcl file and type "DGC2" in the argument field. If you are going to build the Libero&reg; project from the scripts provided in this repository, you are going to need **Libero&reg; 2022.2** or later installed.

To use the legacy-IMAF design, you can simply use the tcl script provided under ..\miv-rv32-freertos-applications\miv-rv32-freertos-demo\src\boards\avalanche-board\legacy-rv32imaf-design\fpga_design\libero design. To do so, you must use **Libero&reg; 2022.2** or later, go to Project->Execute Script.., select the PF_Avalanche_MIV_RV32_BaseDesign.tcl file and type "CFG1" in the argument field.

## How to run FreeRTOS demo on Renode emulation platform
The FreeRTOS demo can be used on board or on Renode emulation platform. To use the provided Renode debug configurations, you will need to use **SoftConsole v2022.2-782** or later.

### Software configuration
To properly use the demo on Renode, the RENODE_SIMULATION macro must be set to 1 in src/boards/avalanche-board/\<configuration-name>/fpga_design_config/fpga_design_config.h. This step is needed because the MiV core has MTIME prescaler, which divides the CPU clock to generate the MTIME. The FreeRTOS kernel generates its own clock by generating tick interrupts and incrementing a tick count at a given frequency. This tick interrupt being based on the MTIME, the prescaler has to be considered when setting the tick frequency (see src/freertos-source/source/portable/GCC/RISC-V/port.c). Nevertheless, there is no such prescaler in the Renode model. So, setting RENODE_SIMULATION to 0 for hardware or to 1 for Renode makes the tick frequency adapted to the platform. You should also check that the definition of the macro TICK_COUNT_PRESCALER matches the MTIME prescaler in the Libero design.

### Start a Renode session

1. Compile the project with the ***Release*** configuration of the design you want
2. Righ click on *miv-rv32-freertos renode-rv32i start-platform-and-debug.launch* (respectively *miv-rv32-freertos renode-legacy-rv32 start-platform-and-debug.launch* if you want to emulate the legacy design), select "Debug as" and "miv-rv32-freertos renode-rv32i start-platform-and-debug" (respectively *miv-rv32-freertos renode-legacy-rv32 start-platform-and-debug.launch*)
3. You can then use the usual debug tools, such as breakpoints, steps, etc.
4. You should see either a "blink" message or a dot appearing in the Renode window, depending on if you use the simple blinky demo or the full demo


### Further reading
[Mi-V soft CPUs](https://mi-v-ecosystem.github.io/docs/mi-v-soft-cpu/#mi-v-soft-cpus)

[Future Avalanche board](https://mi-v-soft-risc-v.github.io/Future-Avalanche-Board/)
___
Libero&reg; and PolarFire&reg; are the property of Microchip Technology Inc. (MCHP).