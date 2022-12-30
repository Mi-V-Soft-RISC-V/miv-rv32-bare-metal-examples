/*******************************************************************************
 * Copyright 2019-2022 Microchip FPGA Embedded Systems Solutions.
 *
 * SPDX-License-Identifier: MIT 
 * 
 * file name : miv-rv32-execute-in-place.ld
 * Mi-V soft processor linker script for creating a SoftConsole downloadable
 * image executing from a one address space whereas the data, sdata and stack 
 * sections are placed in another address space. This could be used in cases such
 * as:
 * 1) When using MIV_RV32, the reset vector points to the LSRAM at address 
 *    0x80000000 and the data, sdata, bss and stack sections are placed in the 
 *    TCM region.
 *  
 * 2) Executing from a Non Volatile memory. The actual memory will depend on 
 *    the FPGA platform. For exameple, it could be the eNVM on SmartFusion2, 
 *    Igloo2 or on-board non-volatile memory which supports code execution.
 * 
 * NOTE: Modify the memory section addresses and the sizes according to your
 * Libero design.
 * 
 * To know more about the memory map of the MIV_RV32 based Libero design, open 
 * the MIV_RV32 IP configurator and look for "Reset Vector Address" and the
 * "Memory Map" tab.
 * 
 */
 
OUTPUT_ARCH( "riscv" )
ENTRY(_start)


MEMORY
{
    rom (rx)  : ORIGIN = 0x80000000, LENGTH = 16k
    ram (rwx) : ORIGIN = 0x80004000, LENGTH = 16k
}

STACK_SIZE          = 1k;               /* needs to be calculated for your application */
HEAP_SIZE           = 1k;                /* needs to be calculated for your application */

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

