# Mi-V soft processor platform source code

## Repo organization

```
<platform>
  |
  |-- drivers
  |     |- fpga_ip
  |     |     | CoreGPIO
  |     |     | CoreSysServices_PF
  |     |     | CoreUARTapb
  |     |
  |     |- off_chip
  |     |     |  .
  |     |     |  .
  |     |
  |-- hal
  |     |
  |-- miv_rv32_hal

  
```

The drivers published here are compatible with the improved SoftConsole project folder structure being used in the latest [example projects](https://github.com/Mi-V-Soft-RISC-V/miv-rv32-bare-metal-examples).
These drivers can also be used with the legacy folder structure (projects released via Firmware Catalog) by defining the **LEGACY_DIR_STRUCTURE** macro in the SoftConsole project settings.

