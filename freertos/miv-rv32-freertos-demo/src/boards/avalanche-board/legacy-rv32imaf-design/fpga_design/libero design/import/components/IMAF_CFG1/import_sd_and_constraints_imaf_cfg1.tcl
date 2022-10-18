puts "\n------------------------------------------------------------------------------- \
	  \r\nImporting Components... \
	  \r\n------------------------------------------------------------------------------- \n"

source ./import/components/IMAF_CFG1/build_sd_imaf_cfg1.tcl

puts "\n------------------------------------------------------------------------------- \
	  \r\nComponents Imported. \
	  \r\n------------------------------------------------------------------------------- \n"

build_design_hierarchy
set_root BaseDesign

puts "\n------------------------------------------------------------------------------- \
	  \r\nApplying Design Constraints... \
	  \r\n------------------------------------------------------------------------------- \n"

import_files -io_pdc ./import/constraints/io/io_constraints.pdc
import_files -sdc ./import/constraints/io_jtag_constraints.sdc
import_files -fp_pdc ./import/constraints/fp/ccc_fp.pdc

# #Associate SDC constraint file to Place and Route tool
organize_tool_files -tool {PLACEROUTE} \
    -file $project_dir/constraint/io/io_constraints.pdc \
    -file $project_dir/constraint/io_jtag_constraints.sdc \
	-file $project_dir/constraint/fp/ccc_fp.pdc \
    -module {BaseDesign::work} -input_type {constraint}

organize_tool_files -tool {SYNTHESIZE} \
	-file $project_dir/constraint/io_jtag_constraints.sdc \
    -module {BaseDesign::work} -input_type {constraint}

organize_tool_files -tool {VERIFYTIMING} \
	-file $project_dir/constraint/io_jtag_constraints.sdc \
    -module {BaseDesign::work} -input_type {constraint}

set_root BaseDesign
run_tool -name {CONSTRAINT_MANAGEMENT}
derive_constraints_sdc

puts "\n------------------------------------------------------------------------------- \
	  \r\nDesign Constraints Applied. \
	  \r\n------------------------------------------------------------------------------- \n"



