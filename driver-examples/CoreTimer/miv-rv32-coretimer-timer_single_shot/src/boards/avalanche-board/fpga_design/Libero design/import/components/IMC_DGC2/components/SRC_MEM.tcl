# Exporting Component Description of SRC_MEM to TCL
# Family: PolarFire
# Part Number: MPF300T_ES-FCG484E
# Create and Configure the core component SRC_MEM
create_and_configure_core -core_vlnv {Actel:SystemBuilder:PF_SRAM_AHBL_AXI:1.2.108} -component_name {SRC_MEM} -params {\
"AXI4_AWIDTH:32" \
"AXI4_DWIDTH:32" \
"AXI4_IDWIDTH:8" \
"AXI4_IFTYPE_RD:T" \
"AXI4_IFTYPE_WR:T" \
"AXI4_WRAP_SUPPORT:F" \
"BYTEENABLES:1" \
"BYTE_ENABLE_WIDTH:4" \
"B_REN_POLARITY:2" \
"CASCADE:1" \
"ECC_OPTIONS:0" \
"FABRIC_INTERFACE_TYPE:0" \
"IMPORT_FILE:miv-rv32i-systick-blinky.hex" \
"INIT_RAM:T" \
"LPM_HINT:0" \
"PIPELINE_OPTIONS:1" \
"RDEPTH:65536" \
"RWIDTH:40" \
"USE_NATIVE_INTERFACE:F" \
"WDEPTH:65536" \
"WWIDTH:40" }
# Exporting Component Description of SRC_MEM to TCL done
