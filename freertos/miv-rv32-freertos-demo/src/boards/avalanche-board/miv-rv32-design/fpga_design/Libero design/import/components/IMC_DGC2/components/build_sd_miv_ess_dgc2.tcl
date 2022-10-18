# Creating SmartDesign BaseDesign
set sd_name {BaseDesign}
create_smartdesign -sd_name ${sd_name}

# Disable auto promotion of pins of type 'pad'
auto_promote_pad_pins -promote_all 0

# Create top level Scalar Ports
sd_create_scalar_port -sd_name ${sd_name} -port_name {BOOTSTRAP_BYPASS} -port_direction {IN}
sd_create_scalar_port -sd_name ${sd_name} -port_name {RX} -port_direction {IN}
sd_create_scalar_port -sd_name ${sd_name} -port_name {SYS_RESET_REQ} -port_direction {IN}
sd_create_scalar_port -sd_name ${sd_name} -port_name {TCK} -port_direction {IN}
sd_create_scalar_port -sd_name ${sd_name} -port_name {TDI} -port_direction {IN}
sd_create_scalar_port -sd_name ${sd_name} -port_name {TMS} -port_direction {IN}
sd_create_scalar_port -sd_name ${sd_name} -port_name {TRSTB} -port_direction {IN}
sd_create_scalar_port -sd_name ${sd_name} -port_name {USER_RST} -port_direction {IN}

sd_create_scalar_port -sd_name ${sd_name} -port_name {TDO} -port_direction {OUT}
sd_create_scalar_port -sd_name ${sd_name} -port_name {TX} -port_direction {OUT}

sd_create_scalar_port -sd_name ${sd_name} -port_name {SCL} -port_direction {INOUT} -port_is_pad {1}
sd_create_scalar_port -sd_name ${sd_name} -port_name {SDA} -port_direction {INOUT} -port_is_pad {1}

# Create top level Bus Ports
sd_create_bus_port -sd_name ${sd_name} -port_name {GPIO_OUT} -port_direction {OUT} -port_range {[3:0]}


# Add BIBUF_0 instance
sd_instantiate_macro -sd_name ${sd_name} -macro_name {BIBUF} -instance_name {BIBUF_0}
sd_invert_pins -sd_name ${sd_name} -pin_names {BIBUF_0:E}



# Add BIBUF_1 instance
sd_instantiate_macro -sd_name ${sd_name} -macro_name {BIBUF} -instance_name {BIBUF_1}
sd_invert_pins -sd_name ${sd_name} -pin_names {BIBUF_1:E}



# Add COREJTAGDEBUG_C0_0 instance
sd_instantiate_component -sd_name ${sd_name} -component_name {COREJTAGDEBUG_C0} -instance_name {COREJTAGDEBUG_C0_0}



# Add CORERESET_PF_C0_0 instance
sd_instantiate_component -sd_name ${sd_name} -component_name {CORERESET_PF_C0} -instance_name {CORERESET_PF_C0_0}
sd_connect_pins_to_constant -sd_name ${sd_name} -pin_names {CORERESET_PF_C0_0:BANK_x_VDDI_STATUS} -value {VCC}
sd_connect_pins_to_constant -sd_name ${sd_name} -pin_names {CORERESET_PF_C0_0:BANK_y_VDDI_STATUS} -value {VCC}
sd_connect_pins_to_constant -sd_name ${sd_name} -pin_names {CORERESET_PF_C0_0:SS_BUSY} -value {GND}
sd_connect_pins_to_constant -sd_name ${sd_name} -pin_names {CORERESET_PF_C0_0:FF_US_RESTORE} -value {GND}



# Add CoreTimer_C0_0 instance
sd_instantiate_component -sd_name ${sd_name} -component_name {CoreTimer_C0} -instance_name {CoreTimer_C0_0}



# Add CoreTimer_C1_0 instance
sd_instantiate_component -sd_name ${sd_name} -component_name {CoreTimer_C1} -instance_name {CoreTimer_C1_0}



# Add MIV_ESS_C0_0 instance
sd_instantiate_component -sd_name ${sd_name} -component_name {MIV_ESS_C0} -instance_name {MIV_ESS_C0_0}
sd_invert_pins -sd_name ${sd_name} -pin_names {MIV_ESS_C0_0:SYS_RESET_REQ}
sd_mark_pins_unused -sd_name ${sd_name} -pin_names {MIV_ESS_C0_0:I2C_IRQ}
sd_connect_pins_to_constant -sd_name ${sd_name} -pin_names {MIV_ESS_C0_0:GPIO_IN} -value {GND}
sd_mark_pins_unused -sd_name ${sd_name} -pin_names {MIV_ESS_C0_0:GPIO_INT}



# Add MIV_RV32_C0_0 instance
sd_instantiate_component -sd_name ${sd_name} -component_name {MIV_RV32_C0} -instance_name {MIV_RV32_C0_0}
sd_create_pin_slices -sd_name ${sd_name} -pin_name {MIV_RV32_C0_0:MSYS_EI} -pin_slices {[0:0]}
sd_create_pin_slices -sd_name ${sd_name} -pin_name {MIV_RV32_C0_0:MSYS_EI} -pin_slices {[1:1]}
sd_connect_pins_to_constant -sd_name ${sd_name} -pin_names {MIV_RV32_C0_0:MSYS_EI[1:1]} -value {GND}
sd_create_pin_slices -sd_name ${sd_name} -pin_name {MIV_RV32_C0_0:MSYS_EI} -pin_slices {[2:2]}
sd_connect_pins_to_constant -sd_name ${sd_name} -pin_names {MIV_RV32_C0_0:MSYS_EI[2:2]} -value {GND}
sd_create_pin_slices -sd_name ${sd_name} -pin_name {MIV_RV32_C0_0:MSYS_EI} -pin_slices {[3:3]}
sd_connect_pins_to_constant -sd_name ${sd_name} -pin_names {MIV_RV32_C0_0:MSYS_EI[3:3]} -value {GND}
sd_create_pin_slices -sd_name ${sd_name} -pin_name {MIV_RV32_C0_0:MSYS_EI} -pin_slices {[4:4]}
sd_connect_pins_to_constant -sd_name ${sd_name} -pin_names {MIV_RV32_C0_0:MSYS_EI[4:4]} -value {GND}
sd_create_pin_slices -sd_name ${sd_name} -pin_name {MIV_RV32_C0_0:MSYS_EI} -pin_slices {[5:5]}
sd_connect_pins_to_constant -sd_name ${sd_name} -pin_names {MIV_RV32_C0_0:MSYS_EI[5:5]} -value {GND}
sd_mark_pins_unused -sd_name ${sd_name} -pin_names {MIV_RV32_C0_0:JTAG_TDO_DR}
sd_mark_pins_unused -sd_name ${sd_name} -pin_names {MIV_RV32_C0_0:EXT_RESETN}
sd_mark_pins_unused -sd_name ${sd_name} -pin_names {MIV_RV32_C0_0:TIME_COUNT_OUT}



# Add PF_CCC_C0_0 instance
sd_instantiate_component -sd_name ${sd_name} -component_name {PF_CCC_C0} -instance_name {PF_CCC_C0_0}



# Add PF_INIT_MONITOR_C0_0 instance
sd_instantiate_component -sd_name ${sd_name} -component_name {PF_INIT_MONITOR_C0} -instance_name {PF_INIT_MONITOR_C0_0}
sd_mark_pins_unused -sd_name ${sd_name} -pin_names {PF_INIT_MONITOR_C0_0:PCIE_INIT_DONE}
sd_mark_pins_unused -sd_name ${sd_name} -pin_names {PF_INIT_MONITOR_C0_0:USRAM_INIT_DONE}
sd_mark_pins_unused -sd_name ${sd_name} -pin_names {PF_INIT_MONITOR_C0_0:SRAM_INIT_DONE}
sd_mark_pins_unused -sd_name ${sd_name} -pin_names {PF_INIT_MONITOR_C0_0:XCVR_INIT_DONE}
sd_mark_pins_unused -sd_name ${sd_name} -pin_names {PF_INIT_MONITOR_C0_0:USRAM_INIT_FROM_SNVM_DONE}
sd_mark_pins_unused -sd_name ${sd_name} -pin_names {PF_INIT_MONITOR_C0_0:USRAM_INIT_FROM_UPROM_DONE}
sd_mark_pins_unused -sd_name ${sd_name} -pin_names {PF_INIT_MONITOR_C0_0:USRAM_INIT_FROM_SPI_DONE}
sd_mark_pins_unused -sd_name ${sd_name} -pin_names {PF_INIT_MONITOR_C0_0:SRAM_INIT_FROM_SNVM_DONE}
sd_mark_pins_unused -sd_name ${sd_name} -pin_names {PF_INIT_MONITOR_C0_0:SRAM_INIT_FROM_UPROM_DONE}
sd_mark_pins_unused -sd_name ${sd_name} -pin_names {PF_INIT_MONITOR_C0_0:SRAM_INIT_FROM_SPI_DONE}
sd_mark_pins_unused -sd_name ${sd_name} -pin_names {PF_INIT_MONITOR_C0_0:AUTOCALIB_DONE}



# Add PF_OSC_C0_0 instance
sd_instantiate_component -sd_name ${sd_name} -component_name {PF_OSC_C0} -instance_name {PF_OSC_C0_0}



# Add SRC_MEM_0 instance
sd_instantiate_component -sd_name ${sd_name} -component_name {SRC_MEM} -instance_name {SRC_MEM_0}



# Add scalar net connections
sd_connect_pins -sd_name ${sd_name} -pin_names {"BIBUF_0:D" "MIV_ESS_C0_0:I2C_SDA_O" }
sd_connect_pins -sd_name ${sd_name} -pin_names {"BIBUF_0:E" "MIV_ESS_C0_0:I2C_SDA_O_EN" }
sd_connect_pins -sd_name ${sd_name} -pin_names {"BIBUF_0:PAD" "SDA" }
sd_connect_pins -sd_name ${sd_name} -pin_names {"BIBUF_0:Y" "MIV_ESS_C0_0:I2C_SDA_I" }
sd_connect_pins -sd_name ${sd_name} -pin_names {"BIBUF_1:D" "MIV_ESS_C0_0:I2C_SCL_O" }
sd_connect_pins -sd_name ${sd_name} -pin_names {"BIBUF_1:E" "MIV_ESS_C0_0:I2C_SCL_O_EN" }
sd_connect_pins -sd_name ${sd_name} -pin_names {"BIBUF_1:PAD" "SCL" }
sd_connect_pins -sd_name ${sd_name} -pin_names {"BIBUF_1:Y" "MIV_ESS_C0_0:I2C_SCL_I" }
sd_connect_pins -sd_name ${sd_name} -pin_names {"BOOTSTRAP_BYPASS" "MIV_ESS_C0_0:BOOTSTRAP_BYPASS" }
sd_connect_pins -sd_name ${sd_name} -pin_names {"COREJTAGDEBUG_C0_0:TCK" "TCK" }
sd_connect_pins -sd_name ${sd_name} -pin_names {"COREJTAGDEBUG_C0_0:TDI" "TDI" }
sd_connect_pins -sd_name ${sd_name} -pin_names {"COREJTAGDEBUG_C0_0:TDO" "TDO" }
sd_connect_pins -sd_name ${sd_name} -pin_names {"COREJTAGDEBUG_C0_0:TGT_TCK_0" "MIV_RV32_C0_0:JTAG_TCK" }
sd_connect_pins -sd_name ${sd_name} -pin_names {"COREJTAGDEBUG_C0_0:TGT_TDI_0" "MIV_RV32_C0_0:JTAG_TDI" }
sd_connect_pins -sd_name ${sd_name} -pin_names {"COREJTAGDEBUG_C0_0:TGT_TDO_0" "MIV_RV32_C0_0:JTAG_TDO" }
sd_connect_pins -sd_name ${sd_name} -pin_names {"COREJTAGDEBUG_C0_0:TGT_TMS_0" "MIV_RV32_C0_0:JTAG_TMS" }
sd_connect_pins -sd_name ${sd_name} -pin_names {"COREJTAGDEBUG_C0_0:TGT_TRSTN_0" "MIV_RV32_C0_0:JTAG_TRSTN" }
sd_connect_pins -sd_name ${sd_name} -pin_names {"COREJTAGDEBUG_C0_0:TMS" "TMS" }
sd_connect_pins -sd_name ${sd_name} -pin_names {"COREJTAGDEBUG_C0_0:TRSTB" "TRSTB" }
sd_connect_pins -sd_name ${sd_name} -pin_names {"CORERESET_PF_C0_0:CLK" "CoreTimer_C0_0:PCLK" "CoreTimer_C1_0:PCLK" "MIV_ESS_C0_0:PCLK" "MIV_RV32_C0_0:CLK" "PF_CCC_C0_0:OUT0_FABCLK_0" "SRC_MEM_0:HCLK" }
sd_connect_pins -sd_name ${sd_name} -pin_names {"CORERESET_PF_C0_0:EXT_RST_N" "USER_RST" }
sd_connect_pins -sd_name ${sd_name} -pin_names {"CORERESET_PF_C0_0:FABRIC_RESET_N" "CoreTimer_C0_0:PRESETn" "CoreTimer_C1_0:PRESETn" "MIV_ESS_C0_0:PRESETN" "SRC_MEM_0:HRESETN" }
sd_connect_pins -sd_name ${sd_name} -pin_names {"CORERESET_PF_C0_0:FPGA_POR_N" "PF_INIT_MONITOR_C0_0:FABRIC_POR_N" }
sd_connect_pins -sd_name ${sd_name} -pin_names {"CORERESET_PF_C0_0:INIT_DONE" "PF_INIT_MONITOR_C0_0:DEVICE_INIT_DONE" }
sd_connect_pins -sd_name ${sd_name} -pin_names {"CORERESET_PF_C0_0:PLL_LOCK" "PF_CCC_C0_0:PLL_LOCK_0" }
sd_connect_pins -sd_name ${sd_name} -pin_names {"CORERESET_PF_C0_0:PLL_POWERDOWN_B" "PF_CCC_C0_0:PLL_POWERDOWN_N_0" }
sd_connect_pins -sd_name ${sd_name} -pin_names {"CoreTimer_C0_0:TIMINT" "MIV_RV32_C0_0:MSYS_EI[0:0]" }
sd_connect_pins -sd_name ${sd_name} -pin_names {"CoreTimer_C1_0:TIMINT" "MIV_RV32_C0_0:EXT_IRQ" }
sd_connect_pins -sd_name ${sd_name} -pin_names {"MIV_ESS_C0_0:CPU_ACCESS_DISABLE" "MIV_RV32_C0_0:TCM_CPU_ACCESS_DISABLE" }
sd_connect_pins -sd_name ${sd_name} -pin_names {"MIV_ESS_C0_0:CPU_RESETN" "MIV_RV32_C0_0:RESETN" }
sd_connect_pins -sd_name ${sd_name} -pin_names {"MIV_ESS_C0_0:SYS_RESET_REQ" "SYS_RESET_REQ" }
sd_connect_pins -sd_name ${sd_name} -pin_names {"MIV_ESS_C0_0:TAS_ACCESS_DISABLE" "MIV_RV32_C0_0:TCM_TAS_ACCESS_DISABLE" }
sd_connect_pins -sd_name ${sd_name} -pin_names {"MIV_ESS_C0_0:UART_RX" "RX" }
sd_connect_pins -sd_name ${sd_name} -pin_names {"MIV_ESS_C0_0:UART_TX" "TX" }
sd_connect_pins -sd_name ${sd_name} -pin_names {"PF_CCC_C0_0:REF_CLK_0" "PF_OSC_C0_0:RCOSC_160MHZ_GL" }

# Add bus net connections
sd_connect_pins -sd_name ${sd_name} -pin_names {"GPIO_OUT" "MIV_ESS_C0_0:GPIO_OUT" }

# Add bus interface net connections
sd_connect_pins -sd_name ${sd_name} -pin_names {"CoreTimer_C0_0:APBslave" "MIV_ESS_C0_0:APB_3_mTARGET" }
sd_connect_pins -sd_name ${sd_name} -pin_names {"CoreTimer_C1_0:APBslave" "MIV_ESS_C0_0:APB_4_mTARGET" }
sd_connect_pins -sd_name ${sd_name} -pin_names {"MIV_ESS_C0_0:APB_0_TARGET" "MIV_RV32_C0_0:APB_M_SLV" }
sd_connect_pins -sd_name ${sd_name} -pin_names {"MIV_ESS_C0_0:TAS_APB_mTARGET" "MIV_RV32_C0_0:TCM_APB_SLV" }
sd_connect_pins -sd_name ${sd_name} -pin_names {"MIV_RV32_C0_0:AHBL_M_SLV" "SRC_MEM_0:AHBSlaveInterface" }

# Re-enable auto promotion of pins of type 'pad'
auto_promote_pad_pins -promote_all 1
# Save the smartDesign
save_smartdesign -sd_name ${sd_name}
# Generate SmartDesign BaseDesign
generate_component -component_name ${sd_name}
