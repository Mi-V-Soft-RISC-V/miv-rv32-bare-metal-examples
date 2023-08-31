# Description
This example project is based on `miv-rv32i-systick-blinky` but was modified to demonstrate three use cases:
How to use C++ on RV32 platform without an overhead.
How to use RV32IM**F** floating-point extension.
How to use standard library `printf` with a passthrough to a UART (See `MSCC_STDIO_THRU_CORE_UART_APB` define).

Note: This demo is tested for the Renode emulation platform only, and has not been tested on hardware.

## Troubleshooting
If you are not able to launch Renode Emulation due to the error "The file does not exist for the external tool named MiV-RV32IMCF-Renode-emulation-platform." This can be resolved by running softconsole through the softconsole.cmd script present in the Softconsole installation directorty (For Windows).

Renode Emulation is currently supported for Release builds. Running in debug mode may cause issues.

The error "Variable references empty selection: ${project_loc}" may be caused due to the miv-rv32mandelbrot-cpp project not being selected in the Project Explorer window. 

# Output
ASCII art output is shown in the UART at 115200 baud rate.

By default, the project runs the mandelbrot C++ demo. To see the raytracer demo, undefine the `//#define DEMO_MANDELBROT` define in the `/src/application/main.cpp` file.