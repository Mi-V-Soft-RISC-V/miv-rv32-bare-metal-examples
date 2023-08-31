/*******************************************************************************
 * Copyright 2019-2022 Microchip FPGA Embedded Systems Solutions.
 *
 * SPDX-License-Identifier: MIT 
 * 
 * file name : miv-rv32-ram.ld
 * Mi-V soft processor linker script for creating a SoftConsole downloadable
 * debug image executing in SRAM.
 * 
 * This linker script assumes that a RAM is connected at on Mi-V soft processor 
 * memory space pointed by the reset vector address.
 *
 * NOTE : Modify the memory section address and the size according to your
 * Libero design. 
 * For example:
 * 1) If you want to download and step debug at a different RAM memory address in
 *    your design (For example TCM base address) than the one provided in this file.
 * 2) The MIV_RV32, when used with MIV_ESS IP, provides ways to copy the executable 
 *    HEX file from external Non-Volatile memory into the TCM at reset. In this 
 *    case your executable must be linked to the TCM address. 
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
    ram (rwx) : ORIGIN = 0x80000000, LENGTH = 32k
}

STACK_SIZE          = 4k;               /* needs to be calculated for your application */
HEAP_SIZE           = 1k;               /* needs to be calculated for your application */

SECTIONS
{
  .entry : ALIGN(0x10)
  {
    KEEP (*(SORT_NONE(.entry)))
    . = ALIGN(0x10);
  } > ram

  .text : ALIGN(0x10)
  {
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
    
  } > ram

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
  } > ram

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
  } > ram

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

