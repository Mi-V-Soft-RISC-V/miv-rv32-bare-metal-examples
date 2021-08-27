#PolarFire System Services design 
#
#Libero's TCL top level script
# Core: MIV_RV32IMA_L1_AHB
#
#This Tcl file sources other Tcl files to build the design(on which recursive export is run) in a bottom-up fashion

#Sourcing the Tcl files for creating individual components under the top level
source ./import/components/PF_SRAM.tcl 
source ./import/components/PF_SYSTEM_SERVICES_C0.tcl 
source ./import/components/PF_CCC_C0.tcl
source ./import/components/CoreAHBLite_C0.tcl
source ./import/components/CoreAHBLite_C1.tcl
source ./import/components/COREAHBTOAPB3_C0.tcl
source ./import/components/CoreAPB3_C0.tcl
source ./import/components/COREJTAGDEBUG_C0.tcl
source ./import/components/CoreUARTapb_C0.tcl
source ./import/components/MIV_RV32IMA_L1_AHB_C0.tcl
source ./import/components/PROC_SUBSYSTEM.tcl 


# Creating SmartDesign RISCV_TOP
set sd_name {RISCV_TOP}
create_smartdesign -sd_name ${sd_name}

# Disable auto promotion of pins of type 'pad'
auto_promote_pad_pins -promote_all 0

# Create top level Ports
sd_create_scalar_port -sd_name ${sd_name} -port_name {TRSTB} -port_direction {IN}
sd_create_scalar_port -sd_name ${sd_name} -port_name {TDI} -port_direction {IN}
sd_create_scalar_port -sd_name ${sd_name} -port_name {TCK} -port_direction {IN}
sd_create_scalar_port -sd_name ${sd_name} -port_name {TMS} -port_direction {IN}
sd_create_scalar_port -sd_name ${sd_name} -port_name {UART_RX} -port_direction {IN}
sd_create_scalar_port -sd_name ${sd_name} -port_name {TDO} -port_direction {OUT}
sd_create_scalar_port -sd_name ${sd_name} -port_name {UART_TX} -port_direction {OUT}
sd_create_scalar_port -sd_name ${sd_name} -port_name {CLK_LED4} -port_direction {OUT}
sd_create_scalar_port -sd_name ${sd_name} -port_name {USER_RESET} -port_direction {IN}
sd_create_scalar_port -sd_name ${sd_name} -port_name {USR_RDVLD} -port_direction {OUT}
sd_create_scalar_port -sd_name ${sd_name} -port_name {USR_CMD_ERROR} -port_direction {OUT}
sd_create_scalar_port -sd_name ${sd_name} -port_name {USR_BUSY} -port_direction {OUT}
sd_create_scalar_port -sd_name ${sd_name} -port_name {CLK_50MHZ} -port_direction {IN} -port_is_pad {1}


# Add AND2_0 instance
sd_instantiate_macro -sd_name ${sd_name} -macro_name {AND2} -instance_name {AND2_0}
sd_connect_pins_to_constant -sd_name ${sd_name} -pin_names {AND2_0:B} -value {VCC}



# Add INBUF_0 instance
sd_instantiate_macro -sd_name ${sd_name} -macro_name {INBUF} -instance_name {INBUF_0}




# Add PF_CCC_C0_0 instance
sd_instantiate_component -sd_name ${sd_name} -component_name {PF_CCC_C0} -instance_name {PF_CCC_C0_0}
sd_mark_pins_unused -sd_name ${sd_name} -pin_names {PF_CCC_C0_0:OUT1_FABCLK_0}
sd_mark_pins_unused -sd_name ${sd_name} -pin_names {PF_CCC_C0_0:PLL_LOCK_0}



# Add PROC_SUBSYSTEM_0 instance
sd_instantiate_component -sd_name ${sd_name} -component_name {PROC_SUBSYSTEM} -instance_name {PROC_SUBSYSTEM_0}
sd_mark_pins_unused -sd_name ${sd_name} -pin_names {PROC_SUBSYSTEM_0:APB_CLK}
sd_mark_pins_unused -sd_name ${sd_name} -pin_names {PROC_SUBSYSTEM_0:APB_Reset}




# Add HDL components
import_files -hdl_source ./import/hdl/led_blink.v
import_files -hdl_source ./import/hdl/reset_synchronizer.v
build_design_hierarchy
sd_instantiate_hdl_module -sd_name ${sd_name} -hdl_module_name {led_blink} -hdl_file {hdl\led_blink.v} -instance_name {led_blink_0}
sd_instantiate_hdl_module -sd_name ${sd_name} -hdl_module_name {reset_synchronizer} -hdl_file {hdl\reset_synchronizer.v} -instance_name {reset_synchronizer_0}



# Add scalar net connections
sd_connect_pins -sd_name ${sd_name} -pin_names {"AND2_0:Y" "reset_synchronizer_0:reset" }
sd_connect_pins -sd_name ${sd_name} -pin_names {"INBUF_0:PAD" "CLK_50MHZ" }
sd_connect_pins -sd_name ${sd_name} -pin_names {"led_blink_0:led" "CLK_LED4" }
sd_connect_pins -sd_name ${sd_name} -pin_names {"INBUF_0:Y" "led_blink_0:clock" "PF_CCC_C0_0:REF_CLK_0" }
sd_connect_pins -sd_name ${sd_name} -pin_names {"PF_CCC_C0_0:OUT0_FABCLK_0" "reset_synchronizer_0:clock" "PROC_SUBSYSTEM_0:PCLK" }
sd_connect_pins -sd_name ${sd_name} -pin_names {"led_blink_0:reset" "reset_synchronizer_0:reset_sync" "PROC_SUBSYSTEM_0:reset" }
sd_connect_pins -sd_name ${sd_name} -pin_names {"PROC_SUBSYSTEM_0:TCK" "TCK" }
sd_connect_pins -sd_name ${sd_name} -pin_names {"PROC_SUBSYSTEM_0:TDI" "TDI" }
sd_connect_pins -sd_name ${sd_name} -pin_names {"PROC_SUBSYSTEM_0:TDO" "TDO" }
sd_connect_pins -sd_name ${sd_name} -pin_names {"PROC_SUBSYSTEM_0:TMS" "TMS" }
sd_connect_pins -sd_name ${sd_name} -pin_names {"PROC_SUBSYSTEM_0:TRSTB" "TRSTB" }
sd_connect_pins -sd_name ${sd_name} -pin_names {"PROC_SUBSYSTEM_0:RX" "UART_RX" }
sd_connect_pins -sd_name ${sd_name} -pin_names {"PROC_SUBSYSTEM_0:TX" "UART_TX" }
sd_connect_pins -sd_name ${sd_name} -pin_names {"AND2_0:A" "USER_RESET" }
sd_connect_pins -sd_name ${sd_name} -pin_names {"USR_BUSY" "PROC_SUBSYSTEM_0:USR_BUSY" }
sd_connect_pins -sd_name ${sd_name} -pin_names {"USR_CMD_ERROR" "PROC_SUBSYSTEM_0:USR_CMD_ERROR" }
sd_connect_pins -sd_name ${sd_name} -pin_names {"USR_RDVLD" "PROC_SUBSYSTEM_0:USR_RDVLD" }



# Re-enable auto promotion of pins of type 'pad'
auto_promote_pad_pins -promote_all 1
# Save the smartDesign
save_smartdesign -sd_name ${sd_name}
# Generate SmartDesign RISCV_TOP
generate_component -component_name ${sd_name}
