## Mi-V Extended Subsystem Design Guide Configuration 2: I2C Write & Boot
This folder contains Tcl scripts that build Libero SoC v2022.1 MIV_ESS DGC2 design project for the Future Avalanche Board. The script is executed in Libero SoC to generate the sample design. 
> Design is catered for Libero SoC v2022.1. Using older versions of Libero SoC will result in errors.

#### PF_Avalanche_MIV_RV32_BaseDesign

| Config  | Description |
| :------:|:----------------------------------------|
| DGC2    | This design uses the **MIV_RV32** core configured as follows: <ul><li>RISC-V Extensions: IMC</li><li>Multiplier: MACC (Pipelined)</li><li>Interfaces: AHB Master (mirrored), APB3 Master</li><li>Reset Vector Address: 0x4000_0000</li><li>Internal IRQs: 6</li><li>TCM: Enabled</li><li>TCM APB Slave (TAS): Enabled</li><li>System Timer: Internal MTIME enabled, Internal MTIME IRQ enabled</li><li>Debug: enabled</li></ul>This design uses the **MIV_ESS** core configured as follows: <ul><li>Bootstrap: Enabled</li><li>Bootstrap Source: I2C</li><li>uDMA: Disabled</li><li>GPIO: Enabled, 2 GPIO_IN and 4 GPIO_OUT (fixed config)</li><li>I2C: Enabled, Two-Byte I2C Address</li><li>PLIC: Disabled</li><li>SPI: Disabled</li><li>Timer: Disabled</li><li>UART: Enabled</li><li>Watchdog: Disabled</li></ul>|

> This design configuration is only available for the PolarFire Avalanche Kit (Revision 3 with production silicon devices).

**Important**: This design requires a Dual EE Click board from mikroBUS inserted into the mikroBUS header on the Avalanche Development Kit board. The Dual EE Click is available at [Mikroe.com](https://www.mikroe.com/dual-ee-click).


## <a name="quick"></a> Instructions

#### Running Libero SoC in GUI mode, with Script Arguments
    1. Open Libero SoC
    2. Execute the selected script, Project -> Execute Script
    3. Select the directory that the script is located in, using the "..."
    4. Select the script and select "Open"
    5. In the arguments text box, enter "DGC2 SYNTHESIZE PS"
    6. Select the "Run" button to execute the script
    7. Once complete, a script report will be generated.

In this example, the arguments "DGC2 SYNTHESIZE PS" are entered to take the production silicon (PS) die project through to Synthesis.

Libero executes the script and opens the Mi-V sample project targeted for a production silicon (PS) die. The script adds Timing constraints to the project for Synthesis, Place and Route, and Timing Verification. Additionally, I/O Constraints are added to the project for Place and Route. The project can now be taken through the remainder of the Libero SoC design flow.

## <a name="Script arguments"></a> Script Arguments
The complete set of script arguments are documented here.

#### First argument:
| Argument                  |  Description   |
| ------------------------- |:---------------|
| DGC2                      | Generate a MIV_ESS example design from the *MIV_ESS v2.0 Design Guide* (accessible from the Libero catalog)  |

#### Second argument:
| Argument                  |  Description   |
| ------------------------- |:---------------|
| SYNTHESIZE                | Run synthesis on the design  |
| PLACE_AND_ROUTE           | Run place and route on the design  |
| GENERATE_BITSTREAM        | Generate the bitstream for the design|
| EXPORT_PROGRAMMING_FILE   | Export the programming file (.job) |


#### Third argument:
| Argument                  |  Description   |
| ------------------------- |:---------------|
| PS | Build a base design targeted for 'PS' die |
| ES | Build a base design targeted for 'ES' die |


## <a name="Software Provided"></a> Software Provided
There are two programs included with this configuration:
* **miv-rv32i-systick-blinky.hex**: A Hex program configured to run from TCM's address (0x4000_0000). The program is initialized in the LSRAM component at 0x8000_0000 and it is accessible over the AHB interface.
    > The example hex program was created using  miv-rv32i-systick-blinky in release mode (miv32i-Release). For more information about the project go to bare metal example: [miv-rv32i-systick-blinky](https://github.com/Mi-V-Soft-RISC-V/miv-rv32-bare-metal-examples/tree/main/applications/miv-rv32-hal/miv-rv32i-systick-blinky)

* **miv-rv32-ess-bootloader.elf**: The supplied Bootloader .elf file is used to copy data from the LSRAM (SRC_MEM) @0x8000_0000 to external I2C Flash memory (Dual EE Click board required)
    > The .elf program was compiled using 'miv-rv32-ess-bootloader' in Bootstrap mode. For more information about the project go to bare metal example: [miv-rv32-ess-bootloader](https://github.com/Mi-V-Soft-RISC-V/miv-rv32-bare-metal-examples/tree/main/applications/bootloaders/miv-rv32-bootloader)

#### Running a Bootloader .elf program for DGC2
The provided program, *miv-rv32-ess-bootloader.elf* , is available in the Libero project folder after the *create_project* .tcl script has been run for the Design Configuration 2 (DGC2). The program can be used to transfer a program stored in LSRAM to an external I2C EEPROM. The MIV_ESS can then copy the code to the MIV_RV32 Tightly Coupled Memory (TCM), then the MIV_RV32 can boot the copied code.

The sources are available from the [Mi-V Soft processor Bare Metal examples](https://github.com/Mi-V-Soft-RISC-V/miv-rv32-bare-metal-examples).
To run the Bootloader .elf program, follow the steps below or refer to the *MIV_ESS v2.0 Design Guide* (accessible from the Libero catalog) for more detailed instructions:
> A DGC2 Libero design directory is required to have been created to access the .elf file. The hardware needs to be programmed with DGC2 bitstream.

    1. Open SoftConsole (v2021.1 or above)
    2. From Run > Debug Configurations, double click GDB OpenOCD Debugging
    3. In the Main window, select C/C++ Applications using the Browse button, then navigate to your Libero's project directory ./MIV_DGC2_BD and select the Bootloader .elf file
    4. Select the Debugger tab to set up OpenOCD and GDB
        a. To set up OpenOCD, check the Start OpenOCD locally check box and browse to the OpenOCD path <SoftConsole-install-directory>\openocd\bin\openocd.exe
        b. To set up GDB, browse to the GDB path <SoftConsole-install-directory>\riscv-unknownelf-gcc\bin\riscv64-unknown-elf-gdb.exe.
    5. Click Apply
    6. Click Debug to launch the debug session. 

## <a name="Design Guide Configuration - DGC2: I2C Write & Boot"></a> Design Guide Configuration - DGC2: I2C Write & Boot

> This design requires a Dual EE Click board from mikroBUS inserted into the mikroBUS header on the Avalanche Development Kit board. The Dual EE Click is available at [Mikroe.com](https://www.mikroe.com/dual-ee-click).

### Features
The Libero designs include the following features:
* A soft RISC-V processor.
* A RISC-V debug block allowing on-target debug using SoftConsole
* An Extended subsystem with peripheral cores
* The operating frequency of the design is 50MHz
* Target memory is SRAM/TCM (32kB)
* User peripherals: MIV_ESS (Bootstrap, I2C, GPIO, UART)

### Boot Sequence Operation
A more detailed description of the boot sequence can be found in this section.

> Pre-requisites:
> * Ensure the Dual EE Click board is inserted correctly in the mikroBUS header on the Future Avalanche Board.
> * The board needs to be programmed with DGC2 bitstream. Refer to this section, run the [Libero Design](#Running Libero SoC in GUI mode, with Script Arguments)
> * Initiating the Boot Sequence for DGC2, requires data to be previously written to external memory I2C EEPROM. Use the provided Bootloader .elf program *miv-rv32-ess-bootloader.elf* to write the *miv-rv32i-systick-blinky.hex* program in the LSRAM to the external memory I2C Flash.

    1. Once the board has been powered-on, hold SW1 to enable the Bootstrap functionality in the MIV_ESS. Then press and release SW2 to perform a system reset request.
    2. MIV_ESS copies a program from the I2C Flash device to the MIV_RV32 Tightly-Coupled Memory (TCM) via the TCM APB Slave (TAS) interface.
    3. When the transfer from I2C Flash is complete, MIV_ESS releases MIV_RV32 core from reset and MIV_RV32 is allowed to boot the program from TCM.
    4. The LEDs on the Future Avalanche Board will start blinking, signifying Bootstrap has completed its transfer and SW1 can then be released.   

### Peripherals - MIV_ESS

| Peripheral                       | Address Start | Address End    |
| ------------------------------:  |:-------------:|:--------------:|
| PLIC                             | 0x7000_0000   | 0x70FF_FFFF    |
| CoreUARTapb                      | 0x7100_0000   | 0x71FF_FFFF    |
| Timer                            | 0x7200_0000   | 0x72FF_FFFF    |
| CoreTimer_0 / MIV_ESS_APBSLOT3   | 0x7300_0000   | 0x73FF_FFFF    |
| CoreTimer_1 / MIV_ESS_APBSLOT4   | 0x7400_0000   | 0x74FF_FFFF    |
| CoreGPIO_OUT                     | 0x7500_0000   | 0x75FF_FFFF    |
| SPI                              | 0x7600_0000   | 0x76FF_FFFF    |
| uDMA                             | 0x7800_0000   | 0x78FF_FFFF    |
| WDOG                             | 0x7900_0000   | 0x79FF_FFFF    |
| I2C                              | 0x7A00_0000   | 0x7AFF_FFFF    |
| MIV_ESS_APBSLOTB_BASE            | 0x7B00_0000   | 0x7BFF_FFFF    |
| MIV_ESS_APBSLOTC_BASE            | 0x7C00_0000   | 0x7CFF_FFFF    |
| MIV_ESS_APBSLOTD_BASE            | 0x7D00_0000   | 0x7DFF_FFFF    |
| MIV_ESS_APBSLOTE_BASE            | 0x7E00_0000   | 0x7EFF_FFFF    |
| MIV_ESS_APBSLOTF_BASE            | 0x7F00_0000   | 0x7FFF_FFFF    |
    
### Memory Sources
    
| Memory Source                    | Address Start | Address End | Size   |
| -------------------------------: |:-------------:|:-----------:|:------:|
| TCM                              | 0x4000_0000   | 0x4000_7FFF | 32kB   | 
| LSRAM                            | 0x8000_0000   | 0x8000_7FFF | 32kB   |


