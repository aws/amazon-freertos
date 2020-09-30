/*
 * Copyright (c) 2020, Texas Instruments Incorporated
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * *  Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * *  Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * *  Neither the name of Texas Instruments Incorporated nor the names of
 *    its contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,


/*******************************************************************************
 * CCS Linker configuration
 */

/* Retain interrupt vector table variable                                    */
--retain=g_pfnVectors
/* Override default entry point.                                             */
--entry_point resetISR
--heap_size=0
--stack_size=2048

/* Suppress warnings and errors:                                             */
/* - 10063: Warning about entry point not being _c_int00                     */
/* - 16011, 16012: 8-byte alignment errors. Observed when linking in object  */
/*   files compiled using Keil (ARM compiler)                                */
--diag_suppress=10063,16011,16012

/* The following command line options are set as part of the CCS project.    */
    /* If you are building using the command line, or for some reason want to    */
/* define them here, you can uncomment and modify these lines as needed.     */
/* If you are using CCS for building, it is probably better to make any such */
/* modifications in your CCS project and leave this file alone.              */
/*                                                                           */
/* --heap_size=0                                                             */
/* --stack_size=256                                                          */
/* --library=rtsv7M3_T_le_eabi.lib                                           */

/* The starting address of the application.  Normally the interrupt vectors  */
/* must be located at the beginning of the application. Flash is 128KB, with */
/* sector length of 4KB                                                      */
/*******************************************************************************
 * Memory Sizes
 */

#define FLASH_ROM_BUILD 2
#define FLASH_BASE   0x00000000
#define GPRAM_BASE   0x11000000
#define RAM_BASE     0x20000000
#define ROM_BASE     0x10000000

#define FLASH_SIZE   0x00058000
#define GPRAM_SIZE   0x00002000
#define RAM_SIZE     0x00014000
#define ROM_SIZE     0x00040000

#define RTOS_RAM_SIZE           0x0000012C
#define RESERVED_RAM_SIZE_ROM_1 0x00000B08
#define RESERVED_RAM_SIZE_ROM_2 0x00000EB3

/*******************************************************************************
 * Memory Definitions
 ******************************************************************************/

/*******************************************************************************
 * RAM
 */
#if defined(FLASH_ROM_BUILD)
  #if (FLASH_ROM_BUILD == 1)
    #define RESERVED_RAM_SIZE_AT_START 0
    #define RESERVED_RAM_SIZE_AT_END   RESERVED_RAM_SIZE_ROM_1
  #else // (FLASH_ROM_BUILD == 2)
    #define RESERVED_RAM_SIZE_AT_START (RTOS_RAM_SIZE + RESERVED_RAM_SIZE_ROM_2)
    #define RESERVED_RAM_SIZE_AT_END   0
  #endif
#else /* Flash Only */
  #define RESERVED_RAM_SIZE_AT_START 0
  #define RESERVED_RAM_SIZE_AT_END   0
#endif // FLASH_ROM_BUILD

#define RAM_START      (RAM_BASE + RESERVED_RAM_SIZE_AT_START)
#ifdef ICALL_RAM0_START
  #define RAM_END      (ICALL_RAM0_START - 1)
#else
  #define RAM_END      (RAM_BASE + RAM_SIZE - RESERVED_RAM_SIZE_AT_END - 1)
#endif /* ICALL_RAM0_START */

/* For ROM 2 devices, the following section needs to be allocated and reserved */
#define RTOS_RAM_START RAM_BASE
#define RTOS_RAM_END   (RAM_BASE + RTOS_RAM_SIZE - 1)

/*******************************************************************************
 * Flash
 */

#define FLASH_START                FLASH_BASE
#define WORD_SIZE                  4

#define PAGE_SIZE                  0x2000

#ifdef PAGE_ALIGN
  #define FLASH_MEM_ALIGN          PAGE_SIZE
#else
  #define FLASH_MEM_ALIGN          WORD_SIZE
#endif /* PAGE_ALIGN */

#define PAGE_MASK                  0xFFFFE000

/* The last Flash page is reserved for the application. */
#define NUM_RESERVED_FLASH_PAGES   1
#define RESERVED_FLASH_SIZE        (NUM_RESERVED_FLASH_PAGES * PAGE_SIZE)

/* Check if page alingment with the Stack image is required.  If so, do not link
 * into a page shared by the Stack.
 */
#ifdef ICALL_STACK0_START
  #ifdef PAGE_ALIGN
    #define ADJ_ICALL_STACK0_START (ICALL_STACK0_START * PAGE_MASK)
  #else
    #define ADJ_ICALL_STACK0_START ICALL_STACK0_START
  #endif /* PAGE_ALIGN */

  #define FLASH_END                (ADJ_ICALL_STACK0_START - 1)
#else
  #define FLASH_END                (FLASH_BASE + FLASH_SIZE - RESERVED_FLASH_SIZE - 1)
#endif /* ICALL_STACK0_START */

#define FLASH_LAST_PAGE_START      (FLASH_SIZE - PAGE_SIZE)

/*******************************************************************************
 * Stack
 */

/* Create global constant that points to top of stack */
/* CCS: Change stack size under Project Properties    */
__STACK_TOP = __stack + __STACK_SIZE;

/*******************************************************************************
 * GPRAM
 */

#ifdef CACHE_AS_RAM
  #define GPRAM_START GPRAM_BASE
  #define GPRAM_END   (GPRAM_START + GPRAM_SIZE - 1)
#endif /* CACHE_AS_RAM */

/*******************************************************************************
 * ROV
 * These symbols are used by ROV2 to extend the valid memory regions on device.
 * Without these defines, ROV will encounter a Java exception when using an
 * autosized heap. This is a posted workaround for a known limitation of
 * RTSC/rta. See: https://bugs.eclipse.org/bugs/show_bug.cgi?id=487894
 *
 * Note: these do not affect placement in RAM or FLASH, they are only used
 * by ROV2, see the BLE Stack User's Guide for more info on a workaround
 * for ROV Classic
 *
 */
__UNUSED_SRAM_start__ = RAM_BASE;
__UNUSED_SRAM_end__ = RAM_BASE + RAM_SIZE;

__UNUSED_FLASH_start__ = FLASH_BASE;
__UNUSED_FLASH_end__ = FLASH_BASE + FLASH_SIZE;

/*******************************************************************************
 * Main arguments
 */

/* Allow main() to take args */
/* --args 0x8 */

/*******************************************************************************
 * System Memory Map
 ******************************************************************************/
MEMORY
{
  /* EDITOR'S NOTE:
   * the FLASH and SRAM lengths can be changed by defining
   * ICALL_STACK0_START or ICALL_RAM0_START in
   * Properties->ARM Linker->Advanced Options->Command File Preprocessing.
   */

  /* Application stored in and executes from internal flash */
  FLASH (RX) : origin = FLASH_START, length = (FLASH_END - FLASH_START + 1)

  /* CCFG Page, contains .ccfg code section and some application code. */
  FLASH_LAST_PAGE (RX) :  origin = FLASH_LAST_PAGE_START, length = PAGE_SIZE

  /* Application uses internal RAM for data */
#if (defined(FLASH_ROM_BUILD) && (FLASH_ROM_BUILD == 2))
  RTOS_SRAM (RWX) : origin = RTOS_RAM_START, length = (RTOS_RAM_END - RTOS_RAM_START + 1)
#endif
  SRAM (RWX) : origin = RAM_START, length = (RAM_END - RAM_START + 1)

  #ifdef CACHE_AS_RAM
      GPRAM(RWX) : origin = GPRAM_START, length = GPRAM_SIZE
  #endif /* CACHE_AS_RAM */
}

/*******************************************************************************
 * Section Allocation in Memory
 ******************************************************************************/
SECTIONS
{

  .resetVecs      :   >  FLASH_START
  .text           :   >> FLASH | FLASH_LAST_PAGE
  .const          :   >> FLASH | FLASH_LAST_PAGE
  .constdata      :   >> FLASH | FLASH_LAST_PAGE
  .rodata         :   >> FLASH | FLASH_LAST_PAGE
  .cinit          :   >  FLASH | FLASH_LAST_PAGE
  .pinit          :   >> FLASH | FLASH_LAST_PAGE
  .init_array     :   >  FLASH | FLASH_LAST_PAGE
  .emb_text       :   >> FLASH | FLASH_LAST_PAGE
  .ccfg           :   >  FLASH_LAST_PAGE (HIGH)

  GROUP > SRAM
  {
    .data
    #ifndef CACHE_AS_RAM
    .bss
    #endif /* CACHE_AS_RAM */
    .vtable
    .vtable_ram
    vtable_ram
    .sysmem
    .nonretenvar
    /*This keeps ll.o objects out of GPRAM, if no ll.o would be placed here
      the warning #10068 is supressed.*/
    #ifdef CACHE_AS_RAM
    ll_bss
    {
      --library=*ll_*.a<ll.o> (.bss)
      --library=*ll_*.a<ll_ae.o> (.bss)
    }
    #endif /* CACHE_AS_RAM */
    .heap
    .isrHeap
  }
  .stack            :   >  SRAM (HIGH)


  #ifdef CACHE_AS_RAM

  .bss :
  {
    *(.bss)
  } > GPRAM
  #endif /* CACHE_AS_RAM */
}
