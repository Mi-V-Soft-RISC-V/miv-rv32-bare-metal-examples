#MIV Cores    : MIV_RV32IMAF_L1_AHB
#
#Libero's TCL top level script
#
#This Tcl file sources other Tcl files to build the design(on which recursive export is run) in a bottom-up fashion

#Sourcing the Tcl files for creating individual components under the top level
source ./import/components/SHARED_COMPONENTS/COREAHBTOAPB3_0.tcl 
source ./import/components/SHARED_COMPONENTS/CoreAHBL_0.tcl 
source ./import/components/SHARED_COMPONENTS/CoreAPB3_0.tcl 
source ./import/components/SHARED_COMPONENTS/CoreGPIO_IN.tcl 
source ./import/components/SHARED_COMPONENTS/CoreGPIO_OUT.tcl 
source ./import/components/SHARED_COMPONENTS/CoreJTAGDebug_1.tcl 
source ./import/components/SHARED_COMPONENTS/CoreRESET_PF_0.tcl 
source ./import/components/SHARED_COMPONENTS/CoreTimer_0.tcl 
source ./import/components/SHARED_COMPONENTS/CoreTimer_1.tcl 
source ./import/components/SHARED_COMPONENTS/CoreUARTapb_0.tcl 
source ./import/components/SHARED_COMPONENTS/MIV_RV32IMAF_L1_AHB_0.tcl 
source ./import/components/SHARED_COMPONENTS/PF_CCC_0.tcl 
source ./import/components/SHARED_COMPONENTS/PF_INIT_MONITOR_0.tcl 
source ./import/components/SHARED_COMPONENTS/PF_OSC_0.tcl 
source ./import/components/SHARED_COMPONENTS/PF_SRAM_0.tcl 

# Creating SmartDesign BaseDesign
set sd_name {BaseDesign}
create_smartdesign -sd_name ${sd_name}

# Disable auto promotion of pins of type 'pad'
auto_promote_pad_pins -promote_all 0

# Create top level Ports
sd_create_scalar_port -sd_name ${sd_name} -port_name {TDO} -port_direction {OUT}
sd_create_scalar_port -sd_name ${sd_name} -port_name {TRSTB} -port_direction {IN}
sd_create_scalar_port -sd_name ${sd_name} -port_name {TCK} -port_direction {IN}
sd_create_scalar_port -sd_name ${sd_name} -port_name {TDI} -port_direction {IN}
sd_create_scalar_port -sd_name ${sd_name} -port_name {TMS} -port_direction {IN}
sd_create_scalar_port -sd_name ${sd_name} -port_name {RX} -port_direction {IN}
sd_create_scalar_port -sd_name ${sd_name} -port_name {TX} -port_direction {OUT}
sd_create_scalar_port -sd_name ${sd_name} -port_name {USER_RST} -port_direction {IN}

sd_create_scalar_port -sd_name ${sd_name} -port_name {PUSH_BTN_1} -port_direction {IN}
sd_create_scalar_port -sd_name ${sd_name} -port_name {PUSH_BTN_2} -port_direction {IN}
sd_create_scalar_port -sd_name ${sd_name} -port_name {LED_1} -port_direction {OUT}
sd_create_scalar_port -sd_name ${sd_name} -port_name {LED_2} -port_direction {OUT}
sd_create_scalar_port -sd_name ${sd_name} -port_name {LED_3} -port_direction {OUT}
sd_create_scalar_port -sd_name ${sd_name} -port_name {LED_4} -port_direction {OUT}

# Add CoreAHBL_0 instance
sd_instantiate_component -sd_name ${sd_name} -component_name {CoreAHBL_0} -instance_name {CoreAHBL_0}
sd_connect_pins_to_constant -sd_name ${sd_name} -pin_names {CoreAHBL_0:REMAP_M0} -value {GND}



# Add COREAHBTOAPB3_0 instance
sd_instantiate_component -sd_name ${sd_name} -component_name {COREAHBTOAPB3_0} -instance_name {COREAHBTOAPB3_0}



# Add CoreAPB3_0 instance
sd_instantiate_component -sd_name ${sd_name} -component_name {CoreAPB3_0} -instance_name {CoreAPB3_0}



# Add CoreGPIO_IN instance
sd_instantiate_component -sd_name ${sd_name} -component_name {CoreGPIO_IN} -instance_name {CoreGPIO_IN}
sd_mark_pins_unused -sd_name ${sd_name} -pin_names {CoreGPIO_IN:INT}
sd_mark_pins_unused -sd_name ${sd_name} -pin_names {CoreGPIO_IN:GPIO_OUT}
sd_create_pin_slices -sd_name ${sd_name} -pin_name {CoreGPIO_IN:GPIO_IN} -pin_slices {"[0:0]"} 
sd_create_pin_slices -sd_name ${sd_name} -pin_name {CoreGPIO_IN:GPIO_IN} -pin_slices {"[1:1]"} 



# Add CoreGPIO_OUT instance
sd_instantiate_component -sd_name ${sd_name} -component_name {CoreGPIO_OUT} -instance_name {CoreGPIO_OUT}
sd_mark_pins_unused -sd_name ${sd_name} -pin_names {CoreGPIO_OUT:INT}
sd_connect_pins_to_constant -sd_name ${sd_name} -pin_names {CoreGPIO_OUT:GPIO_IN} -value {GND}
sd_create_pin_slices -sd_name ${sd_name} -pin_name {CoreGPIO_OUT:GPIO_OUT} -pin_slices {"[0:0]"} 
sd_create_pin_slices -sd_name ${sd_name} -pin_name {CoreGPIO_OUT:GPIO_OUT} -pin_slices {"[1:1]"} 
sd_create_pin_slices -sd_name ${sd_name} -pin_name {CoreGPIO_OUT:GPIO_OUT} -pin_slices {"[2:2]"} 
sd_create_pin_slices -sd_name ${sd_name} -pin_name {CoreGPIO_OUT:GPIO_OUT} -pin_slices {"[3:3]"} 



# Add CoreJTAGDebug_1 instance
sd_instantiate_component -sd_name ${sd_name} -component_name {CoreJTAGDebug_1} -instance_name {CoreJTAGDebug_1}



# Add CoreRESET_PF_0 instance
sd_instantiate_component -sd_name ${sd_name} -component_name {CoreRESET_PF_0} -instance_name {CoreRESET_PF_0}
sd_connect_pins_to_constant -sd_name ${sd_name} -pin_names {CoreRESET_PF_0:BANK_x_VDDI_STATUS} -value {VCC}
sd_connect_pins_to_constant -sd_name ${sd_name} -pin_names {CoreRESET_PF_0:BANK_y_VDDI_STATUS} -value {VCC}
sd_connect_pins_to_constant -sd_name ${sd_name} -pin_names {CoreRESET_PF_0:SS_BUSY} -value {GND}
sd_connect_pins_to_constant -sd_name ${sd_name} -pin_names {CoreRESET_PF_0:FF_US_RESTORE} -value {GND}
sd_mark_pins_unused -sd_name ${sd_name} -pin_names {CoreRESET_PF_0:PLL_POWERDOWN_B}



# Add CoreTimer_0 instance
sd_instantiate_component -sd_name ${sd_name} -component_name {CoreTimer_0} -instance_name {CoreTimer_0}



# Add CoreTimer_1 instance
sd_instantiate_component -sd_name ${sd_name} -component_name {CoreTimer_1} -instance_name {CoreTimer_1}



# Add CoreUARTapb_0 instance
sd_instantiate_component -sd_name ${sd_name} -component_name {CoreUARTapb_0} -instance_name {CoreUARTapb_0}
sd_mark_pins_unused -sd_name ${sd_name} -pin_names {CoreUARTapb_0:TXRDY}
sd_mark_pins_unused -sd_name ${sd_name} -pin_names {CoreUARTapb_0:RXRDY}
sd_mark_pins_unused -sd_name ${sd_name} -pin_names {CoreUARTapb_0:PARITY_ERR}
sd_mark_pins_unused -sd_name ${sd_name} -pin_names {CoreUARTapb_0:OVERFLOW}
sd_mark_pins_unused -sd_name ${sd_name} -pin_names {CoreUARTapb_0:FRAMING_ERR}



# Add MIV_RV32IMAF_L1_AHB_0 instance
sd_instantiate_component -sd_name ${sd_name} -component_name {MIV_RV32IMAF_L1_AHB_0} -instance_name {MIV_RV32IMAF_L1_AHB_0}
sd_create_pin_slices -sd_name ${sd_name} -pin_name {MIV_RV32IMAF_L1_AHB_0:IRQ} -pin_slices {[28:0]}
sd_connect_pins_to_constant -sd_name ${sd_name} -pin_names {MIV_RV32IMAF_L1_AHB_0:IRQ[28:0]} -value {GND}
sd_create_pin_slices -sd_name ${sd_name} -pin_name {MIV_RV32IMAF_L1_AHB_0:IRQ} -pin_slices {[29]}
sd_create_pin_slices -sd_name ${sd_name} -pin_name {MIV_RV32IMAF_L1_AHB_0:IRQ} -pin_slices {[30]}
sd_mark_pins_unused -sd_name ${sd_name} -pin_names {MIV_RV32IMAF_L1_AHB_0:AHB_MST_MEM_HSEL}
sd_mark_pins_unused -sd_name ${sd_name} -pin_names {MIV_RV32IMAF_L1_AHB_0:AHB_MST_MMIO_HSEL}
sd_mark_pins_unused -sd_name ${sd_name} -pin_names {MIV_RV32IMAF_L1_AHB_0:DRV_TDO}
sd_mark_pins_unused -sd_name ${sd_name} -pin_names {MIV_RV32IMAF_L1_AHB_0:EXT_RESETN}



# Add PF_CCC_0 instance
sd_instantiate_component -sd_name ${sd_name} -component_name {PF_CCC_0} -instance_name {PF_CCC_0}



# Add PF_INIT_MONITOR_0 instance
sd_instantiate_component -sd_name ${sd_name} -component_name {PF_INIT_MONITOR_0} -instance_name {PF_INIT_MONITOR_0}
sd_mark_pins_unused -sd_name ${sd_name} -pin_names {PF_INIT_MONITOR_0:PCIE_INIT_DONE}
sd_mark_pins_unused -sd_name ${sd_name} -pin_names {PF_INIT_MONITOR_0:USRAM_INIT_DONE}
sd_mark_pins_unused -sd_name ${sd_name} -pin_names {PF_INIT_MONITOR_0:SRAM_INIT_DONE}
sd_mark_pins_unused -sd_name ${sd_name} -pin_names {PF_INIT_MONITOR_0:XCVR_INIT_DONE}
sd_mark_pins_unused -sd_name ${sd_name} -pin_names {PF_INIT_MONITOR_0:USRAM_INIT_FROM_SNVM_DONE}
sd_mark_pins_unused -sd_name ${sd_name} -pin_names {PF_INIT_MONITOR_0:USRAM_INIT_FROM_UPROM_DONE}
sd_mark_pins_unused -sd_name ${sd_name} -pin_names {PF_INIT_MONITOR_0:USRAM_INIT_FROM_SPI_DONE}
sd_mark_pins_unused -sd_name ${sd_name} -pin_names {PF_INIT_MONITOR_0:SRAM_INIT_FROM_SNVM_DONE}
sd_mark_pins_unused -sd_name ${sd_name} -pin_names {PF_INIT_MONITOR_0:SRAM_INIT_FROM_UPROM_DONE}
sd_mark_pins_unused -sd_name ${sd_name} -pin_names {PF_INIT_MONITOR_0:SRAM_INIT_FROM_SPI_DONE}
sd_mark_pins_unused -sd_name ${sd_name} -pin_names {PF_INIT_MONITOR_0:AUTOCALIB_DONE}



# Add PF_OSC_0 instance
sd_instantiate_component -sd_name ${sd_name} -component_name {PF_OSC_0} -instance_name {PF_OSC_0}



# Add PF_SRAM_0 instance
sd_instantiate_component -sd_name ${sd_name} -component_name {PF_SRAM_0} -instance_name {PF_SRAM_0}



# Add scalar net connections
sd_connect_pins -sd_name ${sd_name} -pin_names {"MIV_RV32IMAF_L1_AHB_0:TCK" "CoreJTAGDebug_1:TGT_TCK_0" }
sd_connect_pins -sd_name ${sd_name} -pin_names {"MIV_RV32IMAF_L1_AHB_0:TDI" "CoreJTAGDebug_1:TGT_TDI_0" }
sd_connect_pins -sd_name ${sd_name} -pin_names {"MIV_RV32IMAF_L1_AHB_0:TMS" "CoreJTAGDebug_1:TGT_TMS_0" }
sd_connect_pins -sd_name ${sd_name} -pin_names {"MIV_RV32IMAF_L1_AHB_0:TRST" "CoreJTAGDebug_1:TGT_TRST_0" }
sd_connect_pins -sd_name ${sd_name} -pin_names {"COREAHBTOAPB3_0:HRESETN" "CoreTimer_1:PRESETn" "MIV_RV32IMAF_L1_AHB_0:RESETN" "PF_SRAM_0:HRESETN" "CoreGPIO_IN:PRESETN" "CoreGPIO_OUT:PRESETN" "CoreAHBL_0:HRESETN" "CoreTimer_0:PRESETn" "CoreUARTapb_0:PRESETN" "CoreRESET_PF_0:FABRIC_RESET_N" }
sd_connect_pins -sd_name ${sd_name} -pin_names {"MIV_RV32IMAF_L1_AHB_0:IRQ[29]" "CoreTimer_0:TIMINT" }
sd_connect_pins -sd_name ${sd_name} -pin_names {"CoreTimer_1:TIMINT" "MIV_RV32IMAF_L1_AHB_0:IRQ[30]" }
sd_connect_pins -sd_name ${sd_name} -pin_names {"MIV_RV32IMAF_L1_AHB_0:TDO" "CoreJTAGDebug_1:TGT_TDO_0" }
sd_connect_pins -sd_name ${sd_name} -pin_names {"COREAHBTOAPB3_0:HCLK" "PF_CCC_0:OUT0_FABCLK_0" "CoreTimer_1:PCLK" "MIV_RV32IMAF_L1_AHB_0:CLK" "PF_SRAM_0:HCLK" "CoreUARTapb_0:PCLK" "CoreGPIO_IN:PCLK" "CoreGPIO_OUT:PCLK" "CoreAHBL_0:HCLK" "CoreTimer_0:PCLK" "CoreRESET_PF_0:CLK" }
sd_connect_pins -sd_name ${sd_name} -pin_names {"PF_CCC_0:PLL_LOCK_0" "CoreRESET_PF_0:PLL_LOCK" }
sd_connect_pins -sd_name ${sd_name} -pin_names {"PF_INIT_MONITOR_0:DEVICE_INIT_DONE" "CoreRESET_PF_0:INIT_DONE" }
sd_connect_pins -sd_name ${sd_name} -pin_names {"PF_OSC_0:RCOSC_160MHZ_GL" "PF_CCC_0:REF_CLK_0" }
sd_connect_pins -sd_name ${sd_name} -pin_names {"CoreUARTapb_0:RX" "RX" }
sd_connect_pins -sd_name ${sd_name} -pin_names {"TCK" "CoreJTAGDebug_1:TCK" }
sd_connect_pins -sd_name ${sd_name} -pin_names {"CoreJTAGDebug_1:TDI" "TDI" }
sd_connect_pins -sd_name ${sd_name} -pin_names {"CoreJTAGDebug_1:TDO" "TDO" }
sd_connect_pins -sd_name ${sd_name} -pin_names {"CoreJTAGDebug_1:TMS" "TMS" }
sd_connect_pins -sd_name ${sd_name} -pin_names {"TRSTB" "CoreJTAGDebug_1:TRSTB" }
sd_connect_pins -sd_name ${sd_name} -pin_names {"CoreUARTapb_0:TX" "TX" }
sd_connect_pins -sd_name ${sd_name} -pin_names {"CoreRESET_PF_0:EXT_RST_N" "USER_RST" }
sd_connect_pins -sd_name ${sd_name} -pin_names {"PF_INIT_MONITOR_0:FABRIC_POR_N" "CoreRESET_PF_0:FPGA_POR_N"}

# Add bus net connections
sd_connect_pins -sd_name ${sd_name} -pin_names {"CoreGPIO_IN:GPIO_IN[0]" "PUSH_BTN_1" }
sd_connect_pins -sd_name ${sd_name} -pin_names {"CoreGPIO_IN:GPIO_IN[1]" "PUSH_BTN_2" }
sd_connect_pins -sd_name ${sd_name} -pin_names {"CoreGPIO_OUT:GPIO_OUT[0]" "LED_1" }
sd_connect_pins -sd_name ${sd_name} -pin_names {"CoreGPIO_OUT:GPIO_OUT[1]" "LED_2" }
sd_connect_pins -sd_name ${sd_name} -pin_names {"CoreGPIO_OUT:GPIO_OUT[2]" "LED_3" }
sd_connect_pins -sd_name ${sd_name} -pin_names {"CoreGPIO_OUT:GPIO_OUT[3]" "LED_4" }

# Add bus interface net connections
sd_connect_pins -sd_name ${sd_name} -pin_names {"COREAHBTOAPB3_0:AHBslave" "CoreAHBL_0:AHBmslave7" }
sd_connect_pins -sd_name ${sd_name} -pin_names {"PF_SRAM_0:AHBSlaveInterface" "CoreAHBL_0:AHBmslave8" }
sd_connect_pins -sd_name ${sd_name} -pin_names {"CoreAPB3_0:APB3mmaster" "COREAHBTOAPB3_0:APBmaster" }
sd_connect_pins -sd_name ${sd_name} -pin_names {"CoreAPB3_0:APBmslave1" "CoreUARTapb_0:APB_bif" }
sd_connect_pins -sd_name ${sd_name} -pin_names {"CoreAPB3_0:APBmslave2" "CoreGPIO_IN:APB_bif" }
sd_connect_pins -sd_name ${sd_name} -pin_names {"CoreAPB3_0:APBmslave3" "CoreTimer_0:APBslave" }
sd_connect_pins -sd_name ${sd_name} -pin_names {"CoreAPB3_0:APBmslave4" "CoreTimer_1:APBslave" }
sd_connect_pins -sd_name ${sd_name} -pin_names {"CoreAPB3_0:APBmslave5" "CoreGPIO_OUT:APB_bif" }
sd_connect_pins -sd_name ${sd_name} -pin_names {"MIV_RV32IMAF_L1_AHB_0:AHB_MST_MEM" "CoreAHBL_0:AHBmmaster1" }
sd_connect_pins -sd_name ${sd_name} -pin_names {"CoreAHBL_0:AHBmmaster0" "MIV_RV32IMAF_L1_AHB_0:AHB_MST_MMIO" }

# Re-enable auto promotion of pins of type 'pad'
auto_promote_pad_pins -promote_all 1
# Re-arrange SmartDesign layout
sd_reset_layout -sd_name ${sd_name}
# Save the smartDesign
save_smartdesign -sd_name ${sd_name}
# Generate SmartDesign BaseDesign
generate_component -component_name ${sd_name}

