/*******************************************************************************
 * Copyright 2019-2021 Microchip FPGA Embedded Systems Solutions.
 *
 * SPDX-License-Identifier: MIT 
 * 
 * file name : miv-rv32-nvm.ld
 * Mi-V soft processor linker script for creating a SoftConsole downloadable
 * image executing from a ROM. The actual memory will depend on the FPGA 
 * platform. For exameple, it could be the eNVM on SmartFusion2, Igloo2 or 
 * on-board non-volatile memory which supports code execution.
 * 
 * Supports MIV_RV32 as well as the legacy RV32 cores with appropriate memory 
 * section addresses as per your design.
 *
 * SVN $Revision: 13158 $
 * SVN $Date: 2021-01-31 10:57:57 +0530 (Sun, 31 Jan 2021) $
 */
 
OUTPUT_ARCH( "riscv" )
ENTRY(_start)


MEMORY
{
    rom (rx) : ORIGIN = 0x80000000, LENGTH =  64k
    ram (rwx) : ORIGIN = 0x8001FFFF, LENGTH = 64k
}

RAM_START_ADDRESS   = 0x8001FFFF;       /* Must be the same value MEMORY region ram ORIGIN above. */
RAM_SIZE            = 64k;              /* Must be the same value MEMORY region ram LENGTH above. */
STACK_SIZE          = 2k;               /* needs to be calculated for your application */             
HEAP_SIZE           = 2k;               /* needs to be calculated for your application */

SECTIONS
{
  .entry : ALIGN(0x10)
  {
    KEEP (*(SORT_NONE(.entry)))
    . = ALIGN(0x10);
  } > rom
  
  .text : ALIGN(0x10)
  {
    KEEP (*(SORT_NONE(.text.entry)))   
    . = ALIGN(0x10);
    *(.text .text.* .gnu.linkonce.t.*)
    *(.plt)
    . = ALIGN(0x10);
    
    KEEP (*crtbegin.o(.ctors))
    KEEP (*(EXCLUDE_FILE (*crtend.o) .ctors))
    KEEP (*(SORT(.ctors.*)))
    KEEP (*crtend.o(.ctors))
    KEEP (*crtbegin.o(.dtors))
    KEEP (*(EXCLUDE_FILE (*crtend.o) .dtors))
    KEEP (*(SORT(.dtors.*)))
    KEEP (*crtend.o(.dtors))
    
    *(.rodata .rodata.* .gnu.linkonce.r.*)
    *(.gcc_except_table) 
    *(.eh_frame_hdr)
    *(.eh_frame)
    
    KEEP (*(.init))
    KEEP (*(.fini))

    PROVIDE_HIDDEN (__preinit_array_start = .);
    KEEP (*(.preinit_array))
    PROVIDE_HIDDEN (__preinit_array_end = .);
    PROVIDE_HIDDEN (__init_array_start = .);
    KEEP (*(SORT(.init_array.*)))
    KEEP (*(.init_array))
    PROVIDE_HIDDEN (__init_array_end = .);
    PROVIDE_HIDDEN (__fini_array_start = .);
    KEEP (*(.fini_array))
    KEEP (*(SORT(.fini_array.*)))
    PROVIDE_HIDDEN (__fini_array_end = .);
    . = ALIGN(0x10);
    
  } >rom

  /* short/global data section */
  .sdata : ALIGN(0x10)
  {
    __sdata_load = LOADADDR(.sdata);
    __sdata_start = .; 
    PROVIDE( __global_pointer$ = . + 0x800);
    *(.srodata.cst16) *(.srodata.cst8) *(.srodata.cst4) *(.srodata.cst2)
    *(.srodata*)
    *(.sdata .sdata.* .gnu.linkonce.s.*)
    . = ALIGN(0x10);
    __sdata_end = .;
  } >ram AT>rom

  /* data section */
  .data : ALIGN(0x10)
  { 
    __data_load = LOADADDR(.data);
    __data_start = .; 
    *(.got.plt) *(.got)
    *(.shdata)
    *(.data .data.* .gnu.linkonce.d.*)
    . = ALIGN(0x10);
    __data_end = .;
  } >ram AT>rom

  /* sbss section */
  .sbss : ALIGN(0x10)
  {
    __sbss_start = .;
    *(.sbss .sbss.* .gnu.linkonce.sb.*)
    *(.scommon)
    . = ALIGN(0x10);
    __sbss_end = .;
  } > ram
  
  /* sbss section */
  .bss : ALIGN(0x10)
  { 
    __bss_start = .;
    *(.shbss)
    *(.bss .bss.* .gnu.linkonce.b.*)
    *(COMMON)
    . = ALIGN(0x10);
    __bss_end = .;
  } > ram

  /* End of uninitialized data segment */
  _end = .;
  
  .heap : ALIGN(0x10)
  {
    __heap_start = .;
    . += HEAP_SIZE;
    __heap_end = .;
    . = ALIGN(0x10);
    _heap_end = __heap_end;
  } > ram
  
  .stack : ALIGN(0x10)
  {
    __stack_bottom = .;
    . += STACK_SIZE;
    __stack_top = .;
  } > ram
}

