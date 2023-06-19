set config [string toupper [lindex $argv 0]]
set design_flow_stage [string toupper [lindex $argv 1]]
set die_variant [string toupper [lindex $argv 2]]

set hw_platform PF_Avalanche
set soft_cpu MIV_RV32
set sd_reference BaseDesign

#
# Procedure blocks start
proc create_new_project_label { } {
	puts "\n------------------------------------------------------------------------------- \
		  \r\nCreating a new project for the 'PF_Avalanche' board. \
		  \r\n-------------------------------------------------------------------------------"
}

proc project_exists { } {
	puts "\n------------------------------------------------------------------------------- \
		  \r\nError: A project exists for the 'PF_Avalanche' with this configuration. \
		  \r\n-------------------------------------------------------------------------------"
}

proc no_first_argument_entered { } {
	puts "\n------------------------------------------------------------------------------- \
		  \r\nInfo: No 1st Argument has been entered. \
		  \r\nInfo: Enter the 1st Argument responsible for type of design configuration -'CFG1..CFGn' \
		  \r\nInfo: Default 'CFG1' design has been selected. \
		  \r\n-------------------------------------------------------------------------------"
}

proc invalid_first_argument { } {
	puts "\n------------------------------------------------------------------------------- \
		  \r\nError: Wrong 1st Argument has been entered. No valid configuration detected. \
		  \r\nInfo: Make sure you enter a valid first argument -'CFG1..CFGn'. \
		  \r\n-------------------------------------------------------------------------------"
}

proc no_second_argument_entered { } {
	puts "\n------------------------------------------------------------------------------- \
		  \r\nInfo: No 2nd Argument has been entered. \
		  \r\nInfo: Enter the 2nd Argument after the 1st to be taken further in the Design Flow. \
		  \r\n-------------------------------------------------------------------------------"
}

proc invalid_second_argument { } {
	puts "\n------------------------------------------------------------------------------- \
		  \r\nError: Wrong 2nd Argument has been entered. \
		  \r\nInfo: Make sure you enter a valid 2nd argument -'Synthesize...Export_Programming_File'.\
		  \r\n-------------------------------------------------------------------------------"
}

proc no_third_argument_entered { } {
	puts "\n------------------------------------------------------------------------------- \
		  \r\nInfo: No 3rd Argument has been entered. \
		  \r\nInfo: Assuming the default 'PS' die type as target \
		  \r\n-------------------------------------------------------------------------------"
}

proc invalid_third_argument { } {
	puts "\n------------------------------------------------------------------------------- \
          \r\nError: Wrong 3rd Argument has been entered. \
          \r\nInfo: Make sure you enter 'PS' or 'ES' to specify die target type. \
		  \r\n-------------------------------------------------------------------------------"
}

proc  base_design_built { } {
	puts "\n------------------------------------------------------------------------------- \
		  \r\nInfo: BaseDesign built. \
		  \r\n-------------------------------------------------------------------------------"
}

proc download_required_direct_cores  { } {
	download_core -vlnv {Actel:DirectCore:CoreUARTapb:5.7.100} -location {www.microchip-ip.com/repositories/DirectCore}
	download_core -vlnv {Actel:DirectCore:CoreTimer:2.0.103} -location {www.microchip-ip.com/repositories/DirectCore}
	download_core -vlnv {Actel:DirectCore:CORERESET_PF:2.3.100} -location {www.microchip-ip.com/repositories/DirectCore}
	download_core -vlnv {Actel:DirectCore:COREJTAGDEBUG:4.0.100} -location {www.microchip-ip.com/repositories/DirectCore}
	download_core -vlnv {Actel:DirectCore:CoreGPIO:3.2.102} -location {www.microchip-ip.com/repositories/DirectCore}
	download_core -vlnv {Actel:DirectCore:COREAXITOAHBL:3.6.101} -location {www.microchip-ip.com/repositories/DirectCore}
	download_core -vlnv {Actel:DirectCore:CoreAPB3:4.2.100} -location {www.microchip-ip.com/repositories/DirectCore}
	download_core -vlnv {Actel:DirectCore:COREAHBTOAPB3:3.2.101} -location {www.microchip-ip.com/repositories/DirectCore}
	download_core -vlnv {Actel:DirectCore:CoreAHBLite:5.6.105} -location {www.microchip-ip.com/repositories/DirectCore}
	download_core -vlnv {Microsemi:MiV:MIV_RV32:3.0.100} -location {www.microchip-ip.com/repositories/DirectCore}
	download_core -vlnv {Microsemi:MiV:MIV_RV32IMA_L1_AHB:2.3.100} -location {www.microchip-ip.com/repositories/DirectCore} 
	download_core -vlnv {Microsemi:MiV:MIV_RV32IMA_L1_AXI:2.1.100} -location {www.microchip-ip.com/repositories/DirectCore} 
	download_core -vlnv {Microsemi:MiV:MIV_RV32IMAF_L1_AHB:2.1.100} -location {www.microchip-ip.com/repositories/DirectCore} 
}

proc pre_configure_place_and_route { } {
	# Configuring Place_and_Route tool for a timing pass.
	configure_tool -name {PLACEROUTE} -params {EFFORT_LEVEL:false} -params {REPAIR_MIN_DELAY:true} -params {TDPR:true}
}

proc run_verify_timing { } {
	run_tool -name {VERIFYTIMING}	
}
# Procedure blocks end
#

#Filter for argument argv0: config
if {$config == ""} then {
	set config "CFG1"
	no_first_argument_entered
} elseif {$config != "CFG1"
		  && $config != "CFG2"
		  && $config != "CFG3"
		  && $config != "DGC2"} then {
	puts "config is: $config"
	invalid_first_argument
	exit 1
} else {
	puts "Info: Configuration selected: $config"
}

#Filter for argument argv1: design flow
if {$design_flow_stage == ""} then {
	no_second_argument_entered
} elseif {$design_flow_stage == "SYNTHESIZE"
		  || $design_flow_stage == "PLACE_AND_ROUTE"
		  || $design_flow_stage == "GENERATE_BITSTREAM"
		  || $design_flow_stage == "EXPORT_PROGRAMMING_FILE"} then {
	puts "Info: Design flow run tool selected: $design_flow_stage"
} elseif {$design_flow_stage == "ES" 
		  || $design_flow_stage == "PS"} then {
	set die_variant "$design_flow_stage"
} else {
	invalid_second_argument
	exit 1
}

#Filter for argument argv2: die type
if {$die_variant == ""} {
	set die_variant "PS"
	no_third_argument_entered
} elseif {$die_variant == "PS"
		  || $die_variant == "ES"} then {
	puts "Info: Die type selected: $die_variant"
} else {
	invalid_third_argument
	exit 1
}

append target_board $hw_platform _ $die_variant
append project_folder_name MIV_ $config _BD
set project_dir "./$project_folder_name"
append project_name $target_board _ $soft_cpu _ $config _ $sd_reference

if {"$config" == "CFG1"} then {
	if {[file exists $project_dir] == 1} then {
		project_exists
	} else {
		create_new_project_label
		if {"$die_variant" == "PS"} then {
			new_project -location $project_dir -name $project_name -project_description {} -block_mode 0 -standalone_peripheral_initialization 0 -instantiate_in_smartdesign 1 -ondemand_build_dh 1 -hdl {VERILOG} -family {PolarFire} -die {MPF300TS} -package {FCG484} -speed {STD} -die_voltage {1.0} -part_range {IND} -adv_options {IO_DEFT_STD:LVCMOS 1.8V} -adv_options {RESTRICTPROBEPINS:1} -adv_options {RESTRICTSPIPINS:0} -adv_options {SYSTEM_CONTROLLER_SUSPEND_MODE:0} -adv_options {TEMPR:IND} -adv_options {VCCI_1.2_VOLTR:IND} -adv_options {VCCI_1.5_VOLTR:IND} -adv_options {VCCI_1.8_VOLTR:IND} -adv_options {VCCI_2.5_VOLTR:IND} -adv_options {VCCI_3.3_VOLTR:IND} -adv_options {VOLTR:IND}
		} elseif {"$die_variant" == "ES"} then {
			new_project -location $project_dir -name $project_name -project_description {} -block_mode 0 -standalone_peripheral_initialization 0 -instantiate_in_smartdesign 1 -ondemand_build_dh 1 -hdl {VERILOG} -family {PolarFire} -die {MPF300TS_ES} -package {FCG484} -speed {STD} -die_voltage {1.0} -part_range {EXT} -adv_options {IO_DEFT_STD:LVCMOS 1.8V} -adv_options {RESTRICTPROBEPINS:1} -adv_options {RESTRICTSPIPINS:0} -adv_options {SYSTEM_CONTROLLER_SUSPEND_MODE:0} -adv_options {TEMPR:EXT} -adv_options {VCCI_1.2_VOLTR:EXT} -adv_options {VCCI_1.5_VOLTR:EXT} -adv_options {VCCI_1.8_VOLTR:EXT} -adv_options {VCCI_2.5_VOLTR:EXT} -adv_options {VCCI_3.3_VOLTR:EXT} -adv_options {VOLTR:EXT}
		} else {
			invalid_third_argument
			exit 1
		}
		download_required_direct_cores
		source ./import/components/IMC_CFG1/import_sd_and_constraints_imc_cfg1.tcl
		save_project
        base_design_built
	}
} elseif {"$config" == "CFG2"} then {
	if {[file exists $project_dir] == 1} then {
		project_exists
	} else {
		create_new_project_label
		if {"$die_variant" == "PS"} then {
			new_project -location $project_dir -name $project_name -project_description {} -block_mode 0 -standalone_peripheral_initialization 0 -instantiate_in_smartdesign 1 -ondemand_build_dh 1 -hdl {VERILOG} -family {PolarFire} -die {MPF300TS} -package {FCG484} -speed {STD} -die_voltage {1.0} -part_range {IND} -adv_options {IO_DEFT_STD:LVCMOS 1.8V} -adv_options {RESTRICTPROBEPINS:1} -adv_options {RESTRICTSPIPINS:0} -adv_options {SYSTEM_CONTROLLER_SUSPEND_MODE:0} -adv_options {TEMPR:IND} -adv_options {VCCI_1.2_VOLTR:IND} -adv_options {VCCI_1.5_VOLTR:IND} -adv_options {VCCI_1.8_VOLTR:IND} -adv_options {VCCI_2.5_VOLTR:IND} -adv_options {VCCI_3.3_VOLTR:IND} -adv_options {VOLTR:IND}
		} elseif {"$die_variant" == "ES"} then {
			new_project -location $project_dir -name $project_name -project_description {} -block_mode 0 -standalone_peripheral_initialization 0 -instantiate_in_smartdesign 1 -ondemand_build_dh 1 -hdl {VERILOG} -family {PolarFire} -die {MPF300TS_ES} -package {FCG484} -speed {STD} -die_voltage {1.0} -part_range {EXT} -adv_options {IO_DEFT_STD:LVCMOS 1.8V} -adv_options {RESTRICTPROBEPINS:1} -adv_options {RESTRICTSPIPINS:0} -adv_options {SYSTEM_CONTROLLER_SUSPEND_MODE:0} -adv_options {TEMPR:EXT} -adv_options {VCCI_1.2_VOLTR:EXT} -adv_options {VCCI_1.5_VOLTR:EXT} -adv_options {VCCI_1.8_VOLTR:EXT} -adv_options {VCCI_2.5_VOLTR:EXT} -adv_options {VCCI_3.3_VOLTR:EXT} -adv_options {VOLTR:EXT}
		} else {
			invalid_third_argument
			exit 1
		}
		download_required_direct_cores
		source ./import/components/IMC_CFG2/import_sd_and_constraints_imc_cfg2.tcl
		save_project
        base_design_built
	}
} elseif {"$config" == "CFG3"} then {
	if {[file exists $project_dir] == 1} then {
		project_exists
	} else {
		create_new_project_label
		if {"$die_variant" == "PS"} then {
			new_project -location $project_dir -name $project_name -project_description {} -block_mode 0 -standalone_peripheral_initialization 0 -instantiate_in_smartdesign 1 -ondemand_build_dh 1 -hdl {VERILOG} -family {PolarFire} -die {MPF300TS} -package {FCG484} -speed {STD} -die_voltage {1.0} -part_range {IND} -adv_options {IO_DEFT_STD:LVCMOS 1.8V} -adv_options {RESTRICTPROBEPINS:1} -adv_options {RESTRICTSPIPINS:0} -adv_options {SYSTEM_CONTROLLER_SUSPEND_MODE:0} -adv_options {TEMPR:IND} -adv_options {VCCI_1.2_VOLTR:IND} -adv_options {VCCI_1.5_VOLTR:IND} -adv_options {VCCI_1.8_VOLTR:IND} -adv_options {VCCI_2.5_VOLTR:IND} -adv_options {VCCI_3.3_VOLTR:IND} -adv_options {VOLTR:IND}
		} elseif {"$die_variant" == "ES"} then {
			new_project -location $project_dir -name $project_name -project_description {} -block_mode 0 -standalone_peripheral_initialization 0 -instantiate_in_smartdesign 1 -ondemand_build_dh 1 -hdl {VERILOG} -family {PolarFire} -die {MPF300TS_ES} -package {FCG484} -speed {STD} -die_voltage {1.0} -part_range {EXT} -adv_options {IO_DEFT_STD:LVCMOS 1.8V} -adv_options {RESTRICTPROBEPINS:1} -adv_options {RESTRICTSPIPINS:0} -adv_options {SYSTEM_CONTROLLER_SUSPEND_MODE:0} -adv_options {TEMPR:EXT} -adv_options {VCCI_1.2_VOLTR:EXT} -adv_options {VCCI_1.5_VOLTR:EXT} -adv_options {VCCI_1.8_VOLTR:EXT} -adv_options {VCCI_2.5_VOLTR:EXT} -adv_options {VCCI_3.3_VOLTR:EXT} -adv_options {VOLTR:EXT}
		} else {
			invalid_third_argument
			exit 1
		}
		download_required_direct_cores
		source ./import/components/IMC_CFG3/import_sd_and_constraints_imc_cfg3.tcl
		save_project
        base_design_built
	}
} elseif {"$config" == "DGC2"} then {
	if {[file exists $project_dir] == 1} then {
		project_exists
	} else {
		create_new_project_label
		if {"$die_variant" == "PS"} then {
			new_project -location $project_dir -name $project_name -project_description {} -block_mode 0 -standalone_peripheral_initialization 0 -instantiate_in_smartdesign 1 -ondemand_build_dh 1 -hdl {VERILOG} -family {PolarFire} -die {MPF300TS} -package {FCG484} -speed {STD} -die_voltage {1.0} -part_range {IND} -adv_options {IO_DEFT_STD:LVCMOS 1.8V} -adv_options {RESTRICTPROBEPINS:1} -adv_options {RESTRICTSPIPINS:0} -adv_options {SYSTEM_CONTROLLER_SUSPEND_MODE:0} -adv_options {TEMPR:IND} -adv_options {VCCI_1.2_VOLTR:IND} -adv_options {VCCI_1.5_VOLTR:IND} -adv_options {VCCI_1.8_VOLTR:IND} -adv_options {VCCI_2.5_VOLTR:IND} -adv_options {VCCI_3.3_VOLTR:IND} -adv_options {VOLTR:IND}
		} elseif {"$die_variant" == "ES"} then {
			new_project -location $project_dir -name $project_name -project_description {} -block_mode 0 -standalone_peripheral_initialization 0 -instantiate_in_smartdesign 1 -ondemand_build_dh 1 -hdl {VERILOG} -family {PolarFire} -die {MPF300TS_ES} -package {FCG484} -speed {STD} -die_voltage {1.0} -part_range {EXT} -adv_options {IO_DEFT_STD:LVCMOS 1.8V} -adv_options {RESTRICTPROBEPINS:1} -adv_options {RESTRICTSPIPINS:0} -adv_options {SYSTEM_CONTROLLER_SUSPEND_MODE:0} -adv_options {TEMPR:EXT} -adv_options {VCCI_1.2_VOLTR:EXT} -adv_options {VCCI_1.5_VOLTR:EXT} -adv_options {VCCI_1.8_VOLTR:EXT} -adv_options {VCCI_2.5_VOLTR:EXT} -adv_options {VCCI_3.3_VOLTR:EXT} -adv_options {VOLTR:EXT}
		} else {
			invalid_third_argument
			exit 1
		}
		file copy ./import/components/IMC_DGC2/hex/miv-rv32i-systick-blinky.hex $project_dir    
		download_required_direct_cores
		file copy ./import/components/IMC_DGC2/bootloader_elf  ./MIV_DGC2_BD
		source ./import/components/IMC_DGC2/import_sd_and_constraints_miv_ess_dgc2.tcl
		save_project
        base_design_built
	}
} else {
		invalid_first_argument
		exit 1
}

pre_configure_place_and_route

if {"$design_flow_stage" == "SYNTHESIZE"} then {
	puts "\n------------------------------------------------------------------------------- \
		  \r\nBegin Synthesis... \
		  \r\n-------------------------------------------------------------------------------"

    run_tool -name {SYNTHESIZE}
    save_project

	puts "\n------------------------------------------------------------------------------- \
		  \r\nSynthesis Complete. \
		  \r\n-------------------------------------------------------------------------------"


} elseif {"$design_flow_stage" == "PLACE_AND_ROUTE"} then {

	puts "\n------------------------------------------------------------------------------- \
		  \r\nBegin Place and Route... \
		  \r\n-------------------------------------------------------------------------------"

	run_verify_timing
	save_project

	puts "\n------------------------------------------------------------------------------- \
		  \r\nPlace and Route Complete. \
		  \r\n-------------------------------------------------------------------------------"


} elseif {"$design_flow_stage" == "GENERATE_BITSTREAM"} then {

	puts "\n------------------------------------------------------------------------------- \
		  \r\nGenerating Bitstream... \
		  \r\n-------------------------------------------------------------------------------"

	run_verify_timing
    run_tool -name {GENERATEPROGRAMMINGDATA}
    run_tool -name {GENERATEPROGRAMMINGFILE}
    save_project

	puts "\n------------------------------------------------------------------------------- \
		  \r\nBitstream Generated. \
		  \r\n-------------------------------------------------------------------------------"


} elseif {"$design_flow_stage" == "EXPORT_PROGRAMMING_FILE"} then {

	puts "\n------------------------------------------------------------------------------- \
		  \r\nExporting Programming Files... \
		  \r\n-------------------------------------------------------------------------------"
	run_verify_timing
	run_tool -name {GENERATEPROGRAMMINGFILE}
	
	
	export_prog_job \
		-job_file_name $project_name \
		-export_dir $project_dir/designer/BaseDesign/export \
		-bitstream_file_type {TRUSTED_FACILITY} \
		-bitstream_file_components {}
	save_project


	puts "\n------------------------------------------------------------------------------- \
		  \r\nProgramming Files Exported. \
		  \r\n-------------------------------------------------------------------------------"

} else {
	puts "Info: No design flow tool run."
}



