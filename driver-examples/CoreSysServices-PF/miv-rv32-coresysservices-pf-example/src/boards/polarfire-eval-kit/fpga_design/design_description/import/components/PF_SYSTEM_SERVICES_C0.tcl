# Exporting Component Description of PF_SYSTEM_SERVICES_C0 to TCL
# Family: PolarFire
# Part Number: MPF300TS_ES-1FCG1152E
# Create and Configure the core component PF_SYSTEM_SERVICES_C0
create_and_configure_core -core_vlnv {Actel:SgCore:PF_SYSTEM_SERVICES:3.0.100} -component_name {PF_SYSTEM_SERVICES_C0} -params {\
"AUTHBITSTREAM:true"  \
"AUTHIAPIMG:true"  \
"DCSERVICE:true"  \
"DIGESTCHECK:true"  \
"DIGSIGSERVICE:true"  \
"DVSERVICE:true"  \
"FF_MAILBOX_ADDR:0x100"  \
"FF_MAILBOX_ADDR_HEX_RANGE:32"  \
"FF_TIMEOUT_VAL:0x20000000"  \
"FF_TIMEOUT_VAL_HEX_RANGE:32"  \
"FFSERVICE:false"  \
"IAPAUTOUPD:true"  \
"IAPSERVICE:true"  \
"NONCESERVICE:true"  \
"OSC_2MHZ_ON:false"  \
"PUFEMSERVICE:true"  \
"QUERYSECSERVICE:true"  \
"RDDEBUGINFO:true"  \
"RDDIGEST:true"  \
"SECNVMRD:true"  \
"SECNVMWR:true"  \
"SNSERVICE:true"  \
"UCSERVICE:true"   }
# Exporting Component Description of PF_SYSTEM_SERVICES_C0 to TCL done
