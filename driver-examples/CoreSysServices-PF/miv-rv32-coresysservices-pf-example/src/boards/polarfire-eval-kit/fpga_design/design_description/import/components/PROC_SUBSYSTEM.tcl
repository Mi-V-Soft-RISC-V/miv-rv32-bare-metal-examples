# Creating SmartDesign PROC_SUBSYSTEM
set sd_name {PROC_SUBSYSTEM}
create_smartdesign -sd_name ${sd_name}

# Disable auto promotion of pins of type 'pad'
auto_promote_pad_pins -promote_all 0

# Create top level Ports
sd_create_scalar_port -sd_name ${sd_name} -port_name {RX} -port_direction {IN}
sd_create_scalar_port -sd_name ${sd_name} -port_name {TX} -port_direction {OUT}
sd_create_scalar_port -sd_name ${sd_name} -port_name {TDI} -port_direction {IN}
sd_create_scalar_port -sd_name ${sd_name} -port_name {TCK} -port_direction {IN}
sd_create_scalar_port -sd_name ${sd_name} -port_name {TMS} -port_direction {IN}
sd_create_scalar_port -sd_name ${sd_name} -port_name {TRSTB} -port_direction {IN}
sd_create_scalar_port -sd_name ${sd_name} -port_name {TDO} -port_direction {OUT}
sd_create_scalar_port -sd_name ${sd_name} -port_name {PCLK} -port_direction {IN}
sd_create_scalar_port -sd_name ${sd_name} -port_name {reset} -port_direction {IN}
sd_create_scalar_port -sd_name ${sd_name} -port_name {APB_CLK} -port_direction {OUT}
sd_create_scalar_port -sd_name ${sd_name} -port_name {APB_Reset} -port_direction {OUT}
sd_create_scalar_port -sd_name ${sd_name} -port_name {USR_BUSY} -port_direction {OUT}
sd_create_scalar_port -sd_name ${sd_name} -port_name {USR_CMD_ERROR} -port_direction {OUT}
sd_create_scalar_port -sd_name ${sd_name} -port_name {USR_RDVLD} -port_direction {OUT}


# Add CoreAHBLite_C0_0 instance
sd_instantiate_component -sd_name ${sd_name} -component_name {CoreAHBLite_C0} -instance_name {CoreAHBLite_C0_0}
sd_connect_pins_to_constant -sd_name ${sd_name} -pin_names {CoreAHBLite_C0_0:REMAP_M0} -value {GND}



# Add CoreAHBLite_C1_0 instance
sd_instantiate_component -sd_name ${sd_name} -component_name {CoreAHBLite_C1} -instance_name {CoreAHBLite_C1_0}
sd_connect_pins_to_constant -sd_name ${sd_name} -pin_names {CoreAHBLite_C1_0:REMAP_M0} -value {GND}



# Add COREAHBTOAPB3_C0_0 instance
sd_instantiate_component -sd_name ${sd_name} -component_name {COREAHBTOAPB3_C0} -instance_name {COREAHBTOAPB3_C0_0}




# Add CoreAPB3_C0_0 instance
sd_instantiate_component -sd_name ${sd_name} -component_name {CoreAPB3_C0} -instance_name {CoreAPB3_C0_0}




# Add COREJTAGDEBUG_C0_0 instance
sd_instantiate_component -sd_name ${sd_name} -component_name {COREJTAGDEBUG_C0} -instance_name {COREJTAGDEBUG_C0_0}




# Add CoreUARTapb_C0_0 instance
sd_instantiate_component -sd_name ${sd_name} -component_name {CoreUARTapb_C0} -instance_name {CoreUARTapb_C0_0}
sd_mark_pins_unused -sd_name ${sd_name} -pin_names {CoreUARTapb_C0_0:TXRDY}
sd_mark_pins_unused -sd_name ${sd_name} -pin_names {CoreUARTapb_C0_0:RXRDY}
sd_mark_pins_unused -sd_name ${sd_name} -pin_names {CoreUARTapb_C0_0:PARITY_ERR}
sd_mark_pins_unused -sd_name ${sd_name} -pin_names {CoreUARTapb_C0_0:OVERFLOW}
sd_mark_pins_unused -sd_name ${sd_name} -pin_names {CoreUARTapb_C0_0:FRAMING_ERR}



# Add MIV_RV32IMA_L1_AHB_C0_0 instance
sd_instantiate_component -sd_name ${sd_name} -component_name {MIV_RV32IMA_L1_AHB_C0} -instance_name {MIV_RV32IMA_L1_AHB_C0_0}
sd_mark_pins_unused -sd_name ${sd_name} -pin_names {MIV_RV32IMA_L1_AHB_C0_0:AHB_MST_MEM_HSEL}
sd_mark_pins_unused -sd_name ${sd_name} -pin_names {MIV_RV32IMA_L1_AHB_C0_0:AHB_MST_MMIO_HSEL}
sd_connect_pins_to_constant -sd_name ${sd_name} -pin_names {MIV_RV32IMA_L1_AHB_C0_0:IRQ} -value {GND}
sd_mark_pins_unused -sd_name ${sd_name} -pin_names {MIV_RV32IMA_L1_AHB_C0_0:DRV_TDO}



# Add PF_SRAM_0 instance
sd_instantiate_component -sd_name ${sd_name} -component_name {PF_SRAM} -instance_name {PF_SRAM_0}



# Add PF_SYSTEM_SERVICES_C0_0 instance
sd_instantiate_component -sd_name ${sd_name} -component_name {PF_SYSTEM_SERVICES_C0} -instance_name {PF_SYSTEM_SERVICES_C0_0}
sd_mark_pins_unused -sd_name ${sd_name} -pin_names {PF_SYSTEM_SERVICES_C0_0:SS_BUSY}
sd_mark_pins_unused -sd_name ${sd_name} -pin_names {PF_SYSTEM_SERVICES_C0_0:SYSSERV_INIT_REQ}



# Add scalar net connections
sd_connect_pins -sd_name ${sd_name} -pin_names {"COREJTAGDEBUG_C0_0:TGT_TCK_0" "MIV_RV32IMA_L1_AHB_C0_0:TCK" }
sd_connect_pins -sd_name ${sd_name} -pin_names {"COREJTAGDEBUG_C0_0:TGT_TDI_0" "MIV_RV32IMA_L1_AHB_C0_0:TDI" }
sd_connect_pins -sd_name ${sd_name} -pin_names {"COREJTAGDEBUG_C0_0:TGT_TMS_0" "MIV_RV32IMA_L1_AHB_C0_0:TMS" }
sd_connect_pins -sd_name ${sd_name} -pin_names {"COREJTAGDEBUG_C0_0:TGT_TDO_0" "MIV_RV32IMA_L1_AHB_C0_0:TDO" }
sd_connect_pins -sd_name ${sd_name} -pin_names {"MIV_RV32IMA_L1_AHB_C0_0:CLK" "PCLK" "APB_CLK" "CoreUARTapb_C0_0:PCLK" "COREAHBTOAPB3_C0_0:HCLK" "CoreAHBLite_C0_0:HCLK" "PF_SRAM_0:HCLK" "CoreAHBLite_C1_0:HCLK" "PF_SYSTEM_SERVICES_C0_0:CLK" }
sd_connect_pins -sd_name ${sd_name} -pin_names {"MIV_RV32IMA_L1_AHB_C0_0:RESETN" "reset" "APB_Reset" "CoreUARTapb_C0_0:PRESETN" "COREAHBTOAPB3_C0_0:HRESETN" "CoreAHBLite_C0_0:HRESETN" "PF_SRAM_0:HRESETN" "CoreAHBLite_C1_0:HRESETN" "PF_SYSTEM_SERVICES_C0_0:RESETN" }
sd_connect_pins -sd_name ${sd_name} -pin_names {"RX" "CoreUARTapb_C0_0:RX" }
sd_connect_pins -sd_name ${sd_name} -pin_names {"COREJTAGDEBUG_C0_0:TCK" "TCK" }
sd_connect_pins -sd_name ${sd_name} -pin_names {"COREJTAGDEBUG_C0_0:TDI" "TDI" }
sd_connect_pins -sd_name ${sd_name} -pin_names {"COREJTAGDEBUG_C0_0:TDO" "TDO" }
sd_connect_pins -sd_name ${sd_name} -pin_names {"COREJTAGDEBUG_C0_0:TMS" "TMS" }
sd_connect_pins -sd_name ${sd_name} -pin_names {"COREJTAGDEBUG_C0_0:TRSTB" "TRSTB" }
sd_connect_pins -sd_name ${sd_name} -pin_names {"TX" "CoreUARTapb_C0_0:TX" }
sd_connect_pins -sd_name ${sd_name} -pin_names {"USR_BUSY" "PF_SYSTEM_SERVICES_C0_0:USR_BUSY" }
sd_connect_pins -sd_name ${sd_name} -pin_names {"USR_CMD_ERROR" "PF_SYSTEM_SERVICES_C0_0:USR_CMD_ERROR" }
sd_connect_pins -sd_name ${sd_name} -pin_names {"USR_RDVLD" "PF_SYSTEM_SERVICES_C0_0:USR_RDVLD" }


# Add bus interface net connections
sd_connect_pins -sd_name ${sd_name} -pin_names {"COREAHBTOAPB3_C0_0:AHBslave" "CoreAHBLite_C0_0:AHBmslave1" }
sd_connect_pins -sd_name ${sd_name} -pin_names {"PF_SRAM_0:AHBSlaveInterface" "CoreAHBLite_C1_0:AHBmslave8" }
sd_connect_pins -sd_name ${sd_name} -pin_names {"COREAHBTOAPB3_C0_0:APBmaster" "CoreAPB3_C0_0:APB3mmaster" }
sd_connect_pins -sd_name ${sd_name} -pin_names {"CoreUARTapb_C0_0:APB_bif" "CoreAPB3_C0_0:APBmslave1" }
sd_connect_pins -sd_name ${sd_name} -pin_names {"CoreAPB3_C0_0:APBmslave5" "PF_SYSTEM_SERVICES_C0_0:APBSlave" }
sd_connect_pins -sd_name ${sd_name} -pin_names {"MIV_RV32IMA_L1_AHB_C0_0:AHB_MST_MEM" "CoreAHBLite_C1_0:AHBmmaster0" }
sd_connect_pins -sd_name ${sd_name} -pin_names {"MIV_RV32IMA_L1_AHB_C0_0:AHB_MST_MMIO" "CoreAHBLite_C0_0:AHBmmaster0" }

# Re-enable auto promotion of pins of type 'pad'
auto_promote_pad_pins -promote_all 1
# Save the smartDesign
save_smartdesign -sd_name ${sd_name}
# Generate SmartDesign PROC_SUBSYSTEM
generate_component -component_name ${sd_name}
