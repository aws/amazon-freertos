/*--------------------------------------------------------------------------
 * MPLAB XC32 Compiler -  ATSAME70Q21B linker script
 * 
 * Copyright (c) 2018, Microchip Technology Inc. and its subsidiaries ("Microchip")
 * All rights reserved.
 * 
 * This software is developed by Microchip Technology Inc. and its
 * subsidiaries ("Microchip").
 * 
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are 
 * met:
 * 
 * 1.      Redistributions of source code must retain the above copyright
 *         notice, this list of conditions and the following disclaimer.
 * 2.      Redistributions in binary form must reproduce the above 
 *         copyright notice, this list of conditions and the following 
 *         disclaimer in the documentation and/or other materials provided 
 *         with the distribution.
 * 3.      Microchip's name may not be used to endorse or promote products
 *         derived from this software without specific prior written 
 *         permission.
 * 
 * THIS SOFTWARE IS PROVIDED BY MICROCHIP "AS IS" AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR PURPOSE ARE DISCLAIMED. IN NO EVENT 
 * SHALL MICROCHIP BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING BUT NOT LIMITED TO
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWSOEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR 
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
 * ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 * 
 */

OUTPUT_FORMAT("elf32-littlearm", "elf32-littlearm", "elf32-littlearm")
OUTPUT_ARCH(arm)
SEARCH_DIR(.)

/*  
 *  Define the __XC32_RESET_HANDLER_NAME macro on the command line when you 
 *  want to use a different name for the Reset Handler function.
 */
#ifndef __XC32_RESET_HANDLER_NAME 
#define __XC32_RESET_HANDLER_NAME Reset_Handler
#endif /* __XC32_RESET_HANDLER_NAME */

/*  Set the entry point in the ELF file. Once the entry point is in the ELF 
 *  file, you can then use the --write-sla option to xc32-bin2hex to place 
 *  the address into the hex file using the SLA field (RECTYPE 5). This hex 
 *  record may be useful for a bootloader that needs to determine the entry 
 *  point to the application.
 */
ENTRY(__XC32_RESET_HANDLER_NAME)

/*************************************************************************
 * Memory-Region Macro Definitions
 * The XC32 linker preprocesses linker scripts. You may define these
 * macros in the MPLAB X project properties or on the command line when
 * calling the linker via the xc32-gcc shell.
 *************************************************************************/

#define ROM_ORIGIN 0x400000
#define ROM_LENGTH 0x10000

#ifndef RAM_ORIGIN
#  define RAM_ORIGIN 0x20400000
#endif
#ifndef RAM_LENGTH
#  define RAM_LENGTH 0x60000
#elif (RAM_LENGTH > 0x60000)
#  error RAM_LENGTH is greater than the max size of 0x60000
#endif
 
#ifndef ITCM_ORIGIN
#  define ITCM_ORIGIN 0x0
#endif
#ifndef __XC32_ITCM_LENGTH
#  define __XC32_ITCM_LENGTH 0x0
#elif (defined(__XC32_ITCM_LENGTH)  && __XC32_ITCM_LENGTH != 0x8000 && __XC32_ITCM_LENGTH != 0x10000 && __XC32_ITCM_LENGTH != 0x20000) 
#  warning Non-standard ITCM length, using default 0x8000
#  undef __XC32_ITCM_LENGTH
#  define __XC32_ITCM_LENGTH 0x8000
#endif

#ifndef DTCM_ORIGIN
#  define DTCM_ORIGIN 0x20000000
#endif
#ifndef __XC32_DTCM_LENGTH
#  define __XC32_DTCM_LENGTH 0x0
#elif (defined(__XC32_DTCM_LENGTH)  && __XC32_DTCM_LENGTH != 0x8000 && __XC32_DTCM_LENGTH != 0x10000 && __XC32_DTCM_LENGTH != 0x20000) 
#  warning Non-standard DTCM length, using default 0x8000
#  undef __XC32_DTCM_LENGTH
#  define __XC32_DTCM_LENGTH 0x8000
#endif

/*************************************************************************
 * Memory-Region Definitions
 * The MEMORY command describes the location and size of blocks of memory
 * on the target device. The command below uses the macros defined above.
 *************************************************************************/
MEMORY
{
  rom (rx) : ORIGIN = ROM_ORIGIN, LENGTH = ROM_LENGTH
  ram (rwx) : ORIGIN = RAM_ORIGIN, LENGTH = RAM_LENGTH - __XC32_ITCM_LENGTH - __XC32_DTCM_LENGTH
  itcm (rwx) : ORIGIN = ITCM_ORIGIN, LENGTH = __XC32_ITCM_LENGTH
  dtcm (rwx) : ORIGIN = DTCM_ORIGIN, LENGTH = __XC32_DTCM_LENGTH
}

/*************************************************************************
 * Section Definitions - Map input sections to output sections
 *************************************************************************/
SECTIONS
{
    /*
     * The linker moves the .vectors section into itcm when itcm is 
     * enabled via the -mitcm option, but only when this .vectors output 
     * section exists in the linker script. 
     */
    .vectors :
    {
        . = ALIGN(4);
        _sfixed = .;
        KEEP(*(.vectors .vectors.*))
    } > rom
    /* 
     * Code Sections - Note that standard input sections such as
     * *(.text), *(.text.*), *(.rodata), & *(.rodata.*)
     * are not mapped here. The best-fit allocator locates them,
     * so that input sections may flow around absolute sections
     * as needed.
     */
    .text :
    {
        . = ALIGN(4);
        *(.glue_7t) *(.glue_7)
        *(.gnu.linkonce.r.*)
        *(.ARM.extab* .gnu.linkonce.armextab.*)

        /* Support C constructors, and C destructors in both user code
           and the C library. This also provides support for C++ code. */
        . = ALIGN(4);
        KEEP(*(.init))
        . = ALIGN(4);
        __preinit_array_start = .;
        KEEP (*(.preinit_array))
        __preinit_array_end = .;

        . = ALIGN(4);
        __init_array_start = .;
        KEEP (*(SORT(.init_array.*)))
        KEEP (*(.init_array))
        __init_array_end = .;

        . = ALIGN(0x4);
        KEEP (*crtbegin.o(.ctors))
        KEEP (*(EXCLUDE_FILE (*crtend.o) .ctors))
        KEEP (*(SORT(.ctors.*)))
        KEEP (*crtend.o(.ctors))

        . = ALIGN(4);
        KEEP(*(.fini))

        . = ALIGN(4);
        __fini_array_start = .;
        KEEP (*(.fini_array))
        KEEP (*(SORT(.fini_array.*)))
        __fini_array_end = .;

        KEEP (*crtbegin.o(.dtors))
        KEEP (*(EXCLUDE_FILE (*crtend.o) .dtors))
        KEEP (*(SORT(.dtors.*)))
        KEEP (*crtend.o(.dtors))

        . = ALIGN(4);
        _efixed = .;            /* End of text section */
    } > rom

    /* .ARM.exidx is sorted, so has to go in its own output section.  */
    PROVIDE_HIDDEN (__exidx_start = .);
    .ARM.exidx :
    {
      *(.ARM.exidx* .gnu.linkonce.armexidx.*)
    } > rom
    PROVIDE_HIDDEN (__exidx_end = .);

    . = ALIGN(4);
    _etext = .;

    /*
     *  Align here to ensure that the .bss section occupies space up to
     *  _end.  Align after .bss to ensure correct alignment even if the
     *  .bss section disappears because there are no input sections.
     *
     *  Note that input sections named .bss* are no longer mapped here.
     *  The best-fit allocator locates them, so that they may flow
     *  around absolute sections as needed.
     */
    .bss (NOLOAD) :
    {
        . = ALIGN(4);
        __bss_start__ = .;
        _sbss = . ;
        _szero = .;
        *(COMMON)
        . = ALIGN(4);
        __bss_end__ = .;
        _ebss = . ;
        _ezero = .;
    } > ram

    . = ALIGN(4);
    _end = . ;
    _ram_end_ = ORIGIN(ram) + LENGTH(ram) -1 ;
}

