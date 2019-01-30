/*
*****************************************************************************
**

**  File        : LinkerScript.ld
**
**  Abstract    : Linker script for STM32H753IIT6 Device with
**                2Mb FLASH, 1Mb RAM
				
				SRAM MAP:
					
				0x20000000 - 0x2001FFFF 	128k	DTCM
				0x24000000 - 0x2407FFFF 	512k	AXI SRAM
				0x30000000 - 0x3001FFFF 	128k	SRAM1
				0x30020000 - 0x3003FFFF 	128k	SRAM2
				0x30040000 - 0x30047FFF 	 32k	SRAM3
				0x38000000 - 0x3800FFFF 	 64k	SRAM4
				0x38800000 - 0x38800FFF 	  4k	Backup SRAM

**  Notes       : If you plan to use any of those areas, don't forget 
**  Notes       : to add them as MMU table entries as well!
**  Notes       : -- Do not declare Backup SRAM as MMU entry!!! --

*****************************************************************************
*/

/* Entry Point */
ENTRY(Reset_Handler)

/* Highest address of the user mode stack */
_estack = 0x24080000;    		/* end of RAM */
/* Generate a link error if heap and stack don't fit into RAM */
_Min_Heap_Size = 0x7000;      	/* required amount of heap  */
_Min_Stack_Size = 0x3000; 		/* required amount of stack */

/* Specify the memory areas */
/* FLASH   (rx)       : ORIGIN = 0x08000000, LENGTH = 2048K */
MEMORY
{
DTCM_RAM(xrw)      : ORIGIN = 0x20000000, LENGTH = 128K
AXI_RAM (xrw)      : ORIGIN = 0x24000000, LENGTH = 512K
RAM_D1  (xrw)      : ORIGIN = 0x30000000, LENGTH = 128K
RAM_D2  (xrw)      : ORIGIN = 0x30020000, LENGTH = 128K
RAM_D3  (xrw)      : ORIGIN = 0x30040000, LENGTH = 32K
RAM_D4  (xrw)      : ORIGIN = 0x38000000, LENGTH = 64K
RAM_BKP (xrw)      : ORIGIN = 0x38800000, LENGTH = 4K
FLASH   (rx)       : ORIGIN = 0x08020000, LENGTH = 1920K
}

/* Define output sections */
SECTIONS
{
  /* The startup code goes first into FLASH */
  .isr_vector :
  {
    . = ALIGN(4);
    KEEP(*(.isr_vector)) /* Startup code */
    . = ALIGN(4);
  } >FLASH

  /* The program code and other data goes into FLASH */
  .text :
  {
    . = ALIGN(4);
    *(.text)           /* .text sections (code) */
    *(.text*)          /* .text* sections (code) */
    *(.glue_7)         /* glue arm to thumb code */
    *(.glue_7t)        /* glue thumb to arm code */
    *(.eh_frame)

    KEEP (*(.init))
    KEEP (*(.fini))

    . = ALIGN(4);
    _etext = .;        /* define a global symbols at end of code */
  } >FLASH

  /* Constant data goes into FLASH */
  .rodata :
  {
    . = ALIGN(4);
    *(.rodata)         /* .rodata sections (constants, strings, etc.) */
    *(.rodata*)        /* .rodata* sections (constants, strings, etc.) */
    . = ALIGN(4);
  } >FLASH

  .ARM.extab   : { *(.ARM.extab* .gnu.linkonce.armextab.*) } >FLASH
  .ARM : {
    __exidx_start = .;
    *(.ARM.exidx*)
    __exidx_end = .;
  } >FLASH

  .preinit_array     :
  {
    PROVIDE_HIDDEN (__preinit_array_start = .);
    KEEP (*(.preinit_array*))
    PROVIDE_HIDDEN (__preinit_array_end = .);
  } >FLASH
  .init_array :
  {
    PROVIDE_HIDDEN (__init_array_start = .);
    KEEP (*(SORT(.init_array.*)))
    KEEP (*(.init_array*))
    PROVIDE_HIDDEN (__init_array_end = .);
  } >FLASH
  .fini_array :
  {
    PROVIDE_HIDDEN (__fini_array_start = .);
    KEEP (*(SORT(.fini_array.*)))
    KEEP (*(.fini_array*))
    PROVIDE_HIDDEN (__fini_array_end = .);
  } >FLASH

  /* used by the startup to initialize data */
  _sidata = LOADADDR(.data);

  /* Initialized data sections goes into RAM, load LMA copy after code */
  .data : 
  {
    . = ALIGN(4);
    _sdata = .;        /* create a global symbol at data start */
    *(.data)           /* .data sections */
    *(.data*)          /* .data* sections */

    . = ALIGN(4);
    _edata = .;        /* define a global symbol at data end */
  } >AXI_RAM AT> FLASH

  
  /* Uninitialized data section */
  . = ALIGN(4);
  .bss :
  {
    /* This is used by the startup in order to initialize the .bss secion */
    _sbss = .;         /* define a global symbol at bss start */
    __bss_start__ = _sbss;
    *(.bss)
    *(.bss*)
    *(COMMON)

    . = ALIGN(4);
    _ebss = .;         /* define a global symbol at bss end */
    __bss_end__ = _ebss;
  } >AXI_RAM

  /* User_heap_stack section, used to check that there is enough RAM left */
  ._user_heap_stack :
  {
    . = ALIGN(8);
    PROVIDE ( end = . );
    PROVIDE ( _end = . );
    . = . + _Min_Heap_Size;
    . = . + _Min_Stack_Size;
    . = ALIGN(8);
  } >AXI_RAM

  /* Remove information from the standard libraries */
  /DISCARD/ :
  {
    libc.a ( * )
    libm.a ( * )
    libgcc.a ( * )
  }

  .ARM.attributes 0 : { *(.ARM.attributes) }
  /*.RamData2 (NOLOAD): { *(.RamData2) } >RAM_D4*/
}


