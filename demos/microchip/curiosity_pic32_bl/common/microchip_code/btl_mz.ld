/*--------------------------------------------------------------------------
 * MPLAB XC Compiler -  PIC32MZ BootLoader linker script
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
 *-------------------------------------------------------------------------*/


/* Custom linker script, for bootloaders residing completely in boot flash */

/*  NOTE: This single-file linker script replaces the two-file system used
 *  for older PIC32 devices.
 */

OUTPUT_FORMAT("elf32-tradlittlemips")
OUTPUT_ARCH(pic32mx)
ENTRY(_reset)
/*
 * Provide for a minimum stack and heap size
 * - _min_stack_size - represents the minimum space that must be made
 *                     available for the stack.  Can be overridden from
 *                     the command line using the linker's --defsym option.
 * - _min_heap_size  - represents the minimum space that must be made
 *                     available for the heap.  Must be specified on
 *                     the command line using the linker's --defsym option.
 */
EXTERN (_min_stack_size _min_heap_size)

/*************************************************************************
 * Processor-specific object file.  Contains SFR definitions.
 *************************************************************************/
INPUT("processor.o")

/*************************************************************************
 * Processor-specific peripheral libraries are optional
 *************************************************************************/
OPTIONAL("libmchp_peripheral.a")


/*************************************************************************
 * Vector-offset initialization
 *************************************************************************/
OPTIONAL("vector_offset_init.o")

/*************************************************************************
 * For interrupt vector handling
 *************************************************************************/
PROVIDE(_vector_spacing = 0x00000001);
PROVIDE(_ebase_address = 0x9FC01000);

/*************************************************************************
 * Memory Address Equates
 * _RESET_ADDR                    -- Reset Vector
 * _BEV_EXCPT_ADDR                -- Boot exception Vector
 * _DBG_EXCPT_ADDR                -- In-circuit Debugging Exception Vector
 * _SIMPLE_TLB_REFILL_EXCPT_ADDR  -- Simple TLB-Refill Exception Vector
 * _CACHE_ERR_EXCPT_ADDR          -- Cache-error Exception Vector
 * _GEN_EXCPT_ADDR                -- General Exception Vector
 *************************************************************************/

_BEV_EXCPT_ADDR                = 0xBFC00380;
_DBG_EXCPT_ADDR                = 0xBFC00480;
_RESET_ADDR                    = 0xBFC00000;
_SIMPLE_TLB_REFILL_EXCPT_ADDR  = _ebase_address + 0;
_CACHE_ERR_EXCPT_ADDR          = _ebase_address + 0x100;
_GEN_EXCPT_ADDR                = _ebase_address + 0x180;

/*************************************************************************
 * Memory Regions
 *
 * Memory regions without attributes cannot be used for orphaned sections.
 * Only sections specifically assigned to these regions can be allocated
 * into these regions.
 *
 * The Debug exception vector is located at 0x9FC00480.
 * The config_<address> sections are used to locate the config words at
 * their absolute addresses.
 *************************************************************************/
MEMORY
{
    kseg1_boot_mem          : ORIGIN = 0xBFC00000, LENGTH = 0x480
    kseg0_program_mem  (rx) : ORIGIN = 0x9fc01000, LENGTH = 0xFF00 - 0x1000
    kseg0_boot_mem          : ORIGIN = 0x9fc004b0, LENGTH = 0x00000000
  kseg1_boot_mem_4B0          : ORIGIN = 0xBFC004B0, LENGTH = 0x1000 - 0x4B0
  config_BFC0FF40             : ORIGIN = 0xBFC0FF40, LENGTH = 0x4
  config_BFC0FF44             : ORIGIN = 0xBFC0FF44, LENGTH = 0x4
  config_BFC0FF48             : ORIGIN = 0xBFC0FF48, LENGTH = 0x4
  config_BFC0FF4C             : ORIGIN = 0xBFC0FF4C, LENGTH = 0x4
  config_BFC0FF50             : ORIGIN = 0xBFC0FF50, LENGTH = 0x4
  config_BFC0FF54             : ORIGIN = 0xBFC0FF54, LENGTH = 0x4
  config_BFC0FF58             : ORIGIN = 0xBFC0FF58, LENGTH = 0x4
  config_BFC0FF5C             : ORIGIN = 0xBFC0FF5C, LENGTH = 0x4
  config_BFC0FF60             : ORIGIN = 0xBFC0FF60, LENGTH = 0x4
  config_BFC0FF64             : ORIGIN = 0xBFC0FF64, LENGTH = 0x4
  config_BFC0FF68             : ORIGIN = 0xBFC0FF68, LENGTH = 0x4
  config_BFC0FF6C             : ORIGIN = 0xBFC0FF6C, LENGTH = 0x4
  config_BFC0FF70             : ORIGIN = 0xBFC0FF70, LENGTH = 0x4
  config_BFC0FF74             : ORIGIN = 0xBFC0FF74, LENGTH = 0x4
  config_BFC0FF78             : ORIGIN = 0xBFC0FF78, LENGTH = 0x4
  config_BFC0FF7C             : ORIGIN = 0xBFC0FF7C, LENGTH = 0x4
  config_BFC0FFC0             : ORIGIN = 0xBFC0FFC0, LENGTH = 0x4
  config_BFC0FFC4             : ORIGIN = 0xBFC0FFC4, LENGTH = 0x4
  config_BFC0FFC8             : ORIGIN = 0xBFC0FFC8, LENGTH = 0x4
  config_BFC0FFCC             : ORIGIN = 0xBFC0FFCC, LENGTH = 0x4
  config_BFC0FFD0             : ORIGIN = 0xBFC0FFD0, LENGTH = 0x4
  config_BFC0FFD4             : ORIGIN = 0xBFC0FFD4, LENGTH = 0x4
  config_BFC0FFD8             : ORIGIN = 0xBFC0FFD8, LENGTH = 0x4
  config_BFC0FFDC             : ORIGIN = 0xBFC0FFDC, LENGTH = 0x4
  config_BFC0FFE0             : ORIGIN = 0xBFC0FFE0, LENGTH = 0x4
  config_BFC0FFE4             : ORIGIN = 0xBFC0FFE4, LENGTH = 0x4
  config_BFC0FFE8             : ORIGIN = 0xBFC0FFE8, LENGTH = 0x4
  config_BFC0FFEC             : ORIGIN = 0xBFC0FFEC, LENGTH = 0x4
  config_BFC0FFF0             : ORIGIN = 0xBFC0FFF0, LENGTH = 0x4
  config_BFC0FFF4             : ORIGIN = 0xBFC0FFF4, LENGTH = 0x4
  config_BFC0FFF8             : ORIGIN = 0xBFC0FFF8, LENGTH = 0x4
  config_BFC0FFFC             : ORIGIN = 0xBFC0FFFC, LENGTH = 0x4
  lowerbootaliaslastpage      : ORIGIN = 0xBFC10000, LENGTH = 0x4000
  upperbootalias              : ORIGIN = 0xBFC20000, LENGTH = 0x14000
  boot1                       : ORIGIN = 0xBFC40000, LENGTH = 0xFF00
  config_BFC4FF40             : ORIGIN = 0xBFC4FF40, LENGTH = 0x4
  config_BFC4FF44             : ORIGIN = 0xBFC4FF44, LENGTH = 0x4
  config_BFC4FF48             : ORIGIN = 0xBFC4FF48, LENGTH = 0x4
  config_BFC4FF4C             : ORIGIN = 0xBFC4FF4C, LENGTH = 0x4
  config_BFC4FF50             : ORIGIN = 0xBFC4FF50, LENGTH = 0x4
  config_BFC4FF54             : ORIGIN = 0xBFC4FF54, LENGTH = 0x4
  config_BFC4FF58             : ORIGIN = 0xBFC4FF58, LENGTH = 0x4
  config_BFC4FF5C             : ORIGIN = 0xBFC4FF5C, LENGTH = 0x4
  config_BFC4FF60             : ORIGIN = 0xBFC4FF60, LENGTH = 0x4
  config_BFC4FF64             : ORIGIN = 0xBFC4FF64, LENGTH = 0x4
  config_BFC4FF68             : ORIGIN = 0xBFC4FF68, LENGTH = 0x4
  config_BFC4FF6C             : ORIGIN = 0xBFC4FF6C, LENGTH = 0x4
  config_BFC4FF70             : ORIGIN = 0xBFC4FF70, LENGTH = 0x4
  config_BFC4FF74             : ORIGIN = 0xBFC4FF74, LENGTH = 0x4
  config_BFC4FF78             : ORIGIN = 0xBFC4FF78, LENGTH = 0x4
  config_BFC4FF7C             : ORIGIN = 0xBFC4FF7C, LENGTH = 0x4
  config_BFC4FFC0             : ORIGIN = 0xBFC4FFC0, LENGTH = 0x4
  config_BFC4FFC4             : ORIGIN = 0xBFC4FFC4, LENGTH = 0x4
  config_BFC4FFC8             : ORIGIN = 0xBFC4FFC8, LENGTH = 0x4
  config_BFC4FFCC             : ORIGIN = 0xBFC4FFCC, LENGTH = 0x4
  config_BFC4FFD0             : ORIGIN = 0xBFC4FFD0, LENGTH = 0x4
  config_BFC4FFD4             : ORIGIN = 0xBFC4FFD4, LENGTH = 0x4
  config_BFC4FFD8             : ORIGIN = 0xBFC4FFD8, LENGTH = 0x4
  config_BFC4FFDC             : ORIGIN = 0xBFC4FFDC, LENGTH = 0x4
  config_BFC4FFE0             : ORIGIN = 0xBFC4FFE0, LENGTH = 0x4
  config_BFC4FFE4             : ORIGIN = 0xBFC4FFE4, LENGTH = 0x4
  config_BFC4FFE8             : ORIGIN = 0xBFC4FFE8, LENGTH = 0x4
  config_BFC4FFEC             : ORIGIN = 0xBFC4FFEC, LENGTH = 0x4
  config_BFC4FFF0             : ORIGIN = 0xBFC4FFF0, LENGTH = 0x4
  config_BFC4FFF4             : ORIGIN = 0xBFC4FFF4, LENGTH = 0x4
  config_BFC4FFF8             : ORIGIN = 0xBFC4FFF8, LENGTH = 0x4
  config_BFC4FFFC             : ORIGIN = 0xBFC4FFFC, LENGTH = 0x4
  boot1lastpage               : ORIGIN = 0xBFC50000, LENGTH = 0x4000
  config_BFC54000             : ORIGIN = 0xBFC54000, LENGTH = 0x4
  config_BFC54004             : ORIGIN = 0xBFC54004, LENGTH = 0x4
  config_BFC54008             : ORIGIN = 0xBFC54008, LENGTH = 0x4
  config_BFC5400C             : ORIGIN = 0xBFC5400C, LENGTH = 0x4
  config_BFC54010             : ORIGIN = 0xBFC54010, LENGTH = 0x4
  config_BFC54020             : ORIGIN = 0xBFC54020, LENGTH = 0x4
  config_BFC54024             : ORIGIN = 0xBFC54024, LENGTH = 0x4
  boot2                       : ORIGIN = 0xBFC60000, LENGTH = 0xFF00
  config_BFC6FF40             : ORIGIN = 0xBFC6FF40, LENGTH = 0x4
  config_BFC6FF44             : ORIGIN = 0xBFC6FF44, LENGTH = 0x4
  config_BFC6FF48             : ORIGIN = 0xBFC6FF48, LENGTH = 0x4
  config_BFC6FF4C             : ORIGIN = 0xBFC6FF4C, LENGTH = 0x4
  config_BFC6FF50             : ORIGIN = 0xBFC6FF50, LENGTH = 0x4
  config_BFC6FF54             : ORIGIN = 0xBFC6FF54, LENGTH = 0x4
  config_BFC6FF58             : ORIGIN = 0xBFC6FF58, LENGTH = 0x4
  config_BFC6FF5C             : ORIGIN = 0xBFC6FF5C, LENGTH = 0x4
  config_BFC6FF60             : ORIGIN = 0xBFC6FF60, LENGTH = 0x4
  config_BFC6FF64             : ORIGIN = 0xBFC6FF64, LENGTH = 0x4
  config_BFC6FF68             : ORIGIN = 0xBFC6FF68, LENGTH = 0x4
  config_BFC6FF6C             : ORIGIN = 0xBFC6FF6C, LENGTH = 0x4
  config_BFC6FF70             : ORIGIN = 0xBFC6FF70, LENGTH = 0x4
  config_BFC6FF74             : ORIGIN = 0xBFC6FF74, LENGTH = 0x4
  config_BFC6FF78             : ORIGIN = 0xBFC6FF78, LENGTH = 0x4
  config_BFC6FF7C             : ORIGIN = 0xBFC6FF7C, LENGTH = 0x4
  config_BFC6FFC0             : ORIGIN = 0xBFC6FFC0, LENGTH = 0x4
  config_BFC6FFC4             : ORIGIN = 0xBFC6FFC4, LENGTH = 0x4
  config_BFC6FFC8             : ORIGIN = 0xBFC6FFC8, LENGTH = 0x4
  config_BFC6FFCC             : ORIGIN = 0xBFC6FFCC, LENGTH = 0x4
  config_BFC6FFD0             : ORIGIN = 0xBFC6FFD0, LENGTH = 0x4
  config_BFC6FFD4             : ORIGIN = 0xBFC6FFD4, LENGTH = 0x4
  config_BFC6FFD8             : ORIGIN = 0xBFC6FFD8, LENGTH = 0x4
  config_BFC6FFDC             : ORIGIN = 0xBFC6FFDC, LENGTH = 0x4
  config_BFC6FFE0             : ORIGIN = 0xBFC6FFE0, LENGTH = 0x4
  config_BFC6FFE4             : ORIGIN = 0xBFC6FFE4, LENGTH = 0x4
  config_BFC6FFE8             : ORIGIN = 0xBFC6FFE8, LENGTH = 0x4
  config_BFC6FFEC             : ORIGIN = 0xBFC6FFEC, LENGTH = 0x4
  config_BFC6FFF0             : ORIGIN = 0xBFC6FFF0, LENGTH = 0x4
  config_BFC6FFF4             : ORIGIN = 0xBFC6FFF4, LENGTH = 0x4
  config_BFC6FFF8             : ORIGIN = 0xBFC6FFF8, LENGTH = 0x4
  config_BFC6FFFC             : ORIGIN = 0xBFC6FFFC, LENGTH = 0x4
  boot2lastpage               : ORIGIN = 0xBFC70000, LENGTH = 0x4000
  configsfrs_BFC0FF40         : ORIGIN = 0xBFC0FF40, LENGTH = 0x40
  configsfrs_BFC0FFC0         : ORIGIN = 0xBFC0FFC0, LENGTH = 0x40
  configsfrs_BFC4FF40         : ORIGIN = 0xBFC4FF40, LENGTH = 0x40
  configsfrs_BFC4FFC0         : ORIGIN = 0xBFC4FFC0, LENGTH = 0x40
  configsfrs_BFC54000         : ORIGIN = 0xBFC54000, LENGTH = 0x14
  configsfrs_BFC54020         : ORIGIN = 0xBFC54020, LENGTH = 0x8
  configsfrs_BFC6FF40         : ORIGIN = 0xBFC6FF40, LENGTH = 0x40
  configsfrs_BFC6FFC0         : ORIGIN = 0xBFC6FFC0, LENGTH = 0x40
  kseg0_data_mem       (w!x)  : ORIGIN = 0x80000000, LENGTH = 0x80000
  sfrs                        : ORIGIN = 0xBF800000, LENGTH = 0x100000
  kseg2_ebi_data_mem          : ORIGIN = 0xC0000000, LENGTH = 0x4000000
  kseg2_sqi_data_mem          : ORIGIN = 0xD0000000, LENGTH = 0x4000000
  kseg3_ebi_data_mem          : ORIGIN = 0xE0000000, LENGTH = 0x4000000
  kseg3_sqi_data_mem          : ORIGIN = 0xF0000000, LENGTH = 0x4000000
}

/*************************************************************************
 * Configuration-word sections. Map the config-pragma input sections to
 * absolute-address output sections.
 *************************************************************************/
SECTIONS
{
  .config_BFC0FF40 : {
    KEEP(*(.config_BFC0FF40))
  } > config_BFC0FF40
  .config_BFC0FF44 : {
    KEEP(*(.config_BFC0FF44))
  } > config_BFC0FF44
  .config_BFC0FF48 : {
    KEEP(*(.config_BFC0FF48))
  } > config_BFC0FF48
  .config_BFC0FF4C : {
    KEEP(*(.config_BFC0FF4C))
  } > config_BFC0FF4C
  .config_BFC0FF50 : {
    KEEP(*(.config_BFC0FF50))
  } > config_BFC0FF50
  .config_BFC0FF54 : {
    KEEP(*(.config_BFC0FF54))
  } > config_BFC0FF54
  .config_BFC0FF58 : {
    KEEP(*(.config_BFC0FF58))
  } > config_BFC0FF58
  .config_BFC0FF5C : {
    KEEP(*(.config_BFC0FF5C))
  } > config_BFC0FF5C
  .config_BFC0FF60 : {
    KEEP(*(.config_BFC0FF60))
  } > config_BFC0FF60
  .config_BFC0FF64 : {
    KEEP(*(.config_BFC0FF64))
  } > config_BFC0FF64
  .config_BFC0FF68 : {
    KEEP(*(.config_BFC0FF68))
  } > config_BFC0FF68
  .config_BFC0FF6C : {
    KEEP(*(.config_BFC0FF6C))
  } > config_BFC0FF6C
  .config_BFC0FF70 : {
    KEEP(*(.config_BFC0FF70))
  } > config_BFC0FF70
  .config_BFC0FF74 : {
    KEEP(*(.config_BFC0FF74))
  } > config_BFC0FF74
  .config_BFC0FF78 : {
    KEEP(*(.config_BFC0FF78))
  } > config_BFC0FF78
  .config_BFC0FF7C : {
    KEEP(*(.config_BFC0FF7C))
  } > config_BFC0FF7C
  .config_BFC0FFC0 : {
    KEEP(*(.config_BFC0FFC0))
  } > config_BFC0FFC0
  .config_BFC0FFC4 : {
    KEEP(*(.config_BFC0FFC4))
  } > config_BFC0FFC4
  .config_BFC0FFC8 : {
    KEEP(*(.config_BFC0FFC8))
  } > config_BFC0FFC8
  .config_BFC0FFCC : {
    KEEP(*(.config_BFC0FFCC))
  } > config_BFC0FFCC
  .config_BFC0FFD0 : {
    KEEP(*(.config_BFC0FFD0))
  } > config_BFC0FFD0
  .config_BFC0FFD4 : {
    KEEP(*(.config_BFC0FFD4))
  } > config_BFC0FFD4
  .config_BFC0FFD8 : {
    KEEP(*(.config_BFC0FFD8))
  } > config_BFC0FFD8
  .config_BFC0FFDC : {
    KEEP(*(.config_BFC0FFDC))
  } > config_BFC0FFDC
  .config_BFC0FFE0 : {
    KEEP(*(.config_BFC0FFE0))
  } > config_BFC0FFE0
  .config_BFC0FFE4 : {
    KEEP(*(.config_BFC0FFE4))
  } > config_BFC0FFE4
  .config_BFC0FFE8 : {
    KEEP(*(.config_BFC0FFE8))
  } > config_BFC0FFE8
  .config_BFC0FFEC : {
    KEEP(*(.config_BFC0FFEC))
  } > config_BFC0FFEC
  .config_BFC0FFF0 : {
    KEEP(*(.config_BFC0FFF0))
  } > config_BFC0FFF0
  .config_BFC0FFF4 : {
    KEEP(*(.config_BFC0FFF4))
  } > config_BFC0FFF4
  .config_BFC0FFF8 : {
    KEEP(*(.config_BFC0FFF8))
  } > config_BFC0FFF8
  .config_BFC0FFFC : {
    KEEP(*(.config_BFC0FFFC))
  } > config_BFC0FFFC
  .config_BFC4FF40 : {
    KEEP(*(.config_BFC4FF40))
  } > config_BFC4FF40
  .config_BFC4FF44 : {
    KEEP(*(.config_BFC4FF44))
  } > config_BFC4FF44
  .config_BFC4FF48 : {
    KEEP(*(.config_BFC4FF48))
  } > config_BFC4FF48
  .config_BFC4FF4C : {
    KEEP(*(.config_BFC4FF4C))
  } > config_BFC4FF4C
  .config_BFC4FF50 : {
    KEEP(*(.config_BFC4FF50))
  } > config_BFC4FF50
  .config_BFC4FF54 : {
    KEEP(*(.config_BFC4FF54))
  } > config_BFC4FF54
  .config_BFC4FF58 : {
    KEEP(*(.config_BFC4FF58))
  } > config_BFC4FF58
  .config_BFC4FF5C : {
    KEEP(*(.config_BFC4FF5C))
  } > config_BFC4FF5C
  .config_BFC4FF60 : {
    KEEP(*(.config_BFC4FF60))
  } > config_BFC4FF60
  .config_BFC4FF64 : {
    KEEP(*(.config_BFC4FF64))
  } > config_BFC4FF64
  .config_BFC4FF68 : {
    KEEP(*(.config_BFC4FF68))
  } > config_BFC4FF68
  .config_BFC4FF6C : {
    KEEP(*(.config_BFC4FF6C))
  } > config_BFC4FF6C
  .config_BFC4FF70 : {
    KEEP(*(.config_BFC4FF70))
  } > config_BFC4FF70
  .config_BFC4FF74 : {
    KEEP(*(.config_BFC4FF74))
  } > config_BFC4FF74
  .config_BFC4FF78 : {
    KEEP(*(.config_BFC4FF78))
  } > config_BFC4FF78
  .config_BFC4FF7C : {
    KEEP(*(.config_BFC4FF7C))
  } > config_BFC4FF7C
  .config_BFC4FFC0 : {
    KEEP(*(.config_BFC4FFC0))
  } > config_BFC4FFC0
  .config_BFC4FFC4 : {
    KEEP(*(.config_BFC4FFC4))
  } > config_BFC4FFC4
  .config_BFC4FFC8 : {
    KEEP(*(.config_BFC4FFC8))
  } > config_BFC4FFC8
  .config_BFC4FFCC : {
    KEEP(*(.config_BFC4FFCC))
  } > config_BFC4FFCC
  .config_BFC4FFD0 : {
    KEEP(*(.config_BFC4FFD0))
  } > config_BFC4FFD0
  .config_BFC4FFD4 : {
    KEEP(*(.config_BFC4FFD4))
  } > config_BFC4FFD4
  .config_BFC4FFD8 : {
    KEEP(*(.config_BFC4FFD8))
  } > config_BFC4FFD8
  .config_BFC4FFDC : {
    KEEP(*(.config_BFC4FFDC))
  } > config_BFC4FFDC
  .config_BFC4FFE0 : {
    KEEP(*(.config_BFC4FFE0))
  } > config_BFC4FFE0
  .config_BFC4FFE4 : {
    KEEP(*(.config_BFC4FFE4))
  } > config_BFC4FFE4
  .config_BFC4FFE8 : {
    KEEP(*(.config_BFC4FFE8))
  } > config_BFC4FFE8
  .config_BFC4FFEC : {
    KEEP(*(.config_BFC4FFEC))
  } > config_BFC4FFEC
  .config_BFC4FFF0 : {
    KEEP(*(.config_BFC4FFF0))
  } > config_BFC4FFF0
  .config_BFC4FFF4 : {
    KEEP(*(.config_BFC4FFF4))
  } > config_BFC4FFF4
  .config_BFC4FFF8 : {
    KEEP(*(.config_BFC4FFF8))
  } > config_BFC4FFF8
  .config_BFC4FFFC : {
    KEEP(*(.config_BFC4FFFC))
  } > config_BFC4FFFC
  .config_BFC54000 : {
    KEEP(*(.config_BFC54000))
  } > config_BFC54000
  .config_BFC54004 : {
    KEEP(*(.config_BFC54004))
  } > config_BFC54004
  .config_BFC54008 : {
    KEEP(*(.config_BFC54008))
  } > config_BFC54008
  .config_BFC5400C : {
    KEEP(*(.config_BFC5400C))
  } > config_BFC5400C
  .config_BFC54010 : {
    KEEP(*(.config_BFC54010))
  } > config_BFC54010
  .config_BFC54020 : {
    KEEP(*(.config_BFC54020))
  } > config_BFC54020
  .config_BFC54024 : {
    KEEP(*(.config_BFC54024))
  } > config_BFC54024
  .config_BFC6FF40 : {
    KEEP(*(.config_BFC6FF40))
  } > config_BFC6FF40
  .config_BFC6FF44 : {
    KEEP(*(.config_BFC6FF44))
  } > config_BFC6FF44
  .config_BFC6FF48 : {
    KEEP(*(.config_BFC6FF48))
  } > config_BFC6FF48
  .config_BFC6FF4C : {
    KEEP(*(.config_BFC6FF4C))
  } > config_BFC6FF4C
  .config_BFC6FF50 : {
    KEEP(*(.config_BFC6FF50))
  } > config_BFC6FF50
  .config_BFC6FF54 : {
    KEEP(*(.config_BFC6FF54))
  } > config_BFC6FF54
  .config_BFC6FF58 : {
    KEEP(*(.config_BFC6FF58))
  } > config_BFC6FF58
  .config_BFC6FF5C : {
    KEEP(*(.config_BFC6FF5C))
  } > config_BFC6FF5C
  .config_BFC6FF60 : {
    KEEP(*(.config_BFC6FF60))
  } > config_BFC6FF60
  .config_BFC6FF64 : {
    KEEP(*(.config_BFC6FF64))
  } > config_BFC6FF64
  .config_BFC6FF68 : {
    KEEP(*(.config_BFC6FF68))
  } > config_BFC6FF68
  .config_BFC6FF6C : {
    KEEP(*(.config_BFC6FF6C))
  } > config_BFC6FF6C
  .config_BFC6FF70 : {
    KEEP(*(.config_BFC6FF70))
  } > config_BFC6FF70
  .config_BFC6FF74 : {
    KEEP(*(.config_BFC6FF74))
  } > config_BFC6FF74
  .config_BFC6FF78 : {
    KEEP(*(.config_BFC6FF78))
  } > config_BFC6FF78
  .config_BFC6FF7C : {
    KEEP(*(.config_BFC6FF7C))
  } > config_BFC6FF7C
  .config_BFC6FFC0 : {
    KEEP(*(.config_BFC6FFC0))
  } > config_BFC6FFC0
  .config_BFC6FFC4 : {
    KEEP(*(.config_BFC6FFC4))
  } > config_BFC6FFC4
  .config_BFC6FFC8 : {
    KEEP(*(.config_BFC6FFC8))
  } > config_BFC6FFC8
  .config_BFC6FFCC : {
    KEEP(*(.config_BFC6FFCC))
  } > config_BFC6FFCC
  .config_BFC6FFD0 : {
    KEEP(*(.config_BFC6FFD0))
  } > config_BFC6FFD0
  .config_BFC6FFD4 : {
    KEEP(*(.config_BFC6FFD4))
  } > config_BFC6FFD4
  .config_BFC6FFD8 : {
    KEEP(*(.config_BFC6FFD8))
  } > config_BFC6FFD8
  .config_BFC6FFDC : {
    KEEP(*(.config_BFC6FFDC))
  } > config_BFC6FFDC
  .config_BFC6FFE0 : {
    KEEP(*(.config_BFC6FFE0))
  } > config_BFC6FFE0
  .config_BFC6FFE4 : {
    KEEP(*(.config_BFC6FFE4))
  } > config_BFC6FFE4
  .config_BFC6FFE8 : {
    KEEP(*(.config_BFC6FFE8))
  } > config_BFC6FFE8
  .config_BFC6FFEC : {
    KEEP(*(.config_BFC6FFEC))
  } > config_BFC6FFEC
  .config_BFC6FFF0 : {
    KEEP(*(.config_BFC6FFF0))
  } > config_BFC6FFF0
  .config_BFC6FFF4 : {
    KEEP(*(.config_BFC6FFF4))
  } > config_BFC6FFF4
  .config_BFC6FFF8 : {
    KEEP(*(.config_BFC6FFF8))
  } > config_BFC6FFF8
  .config_BFC6FFFC : {
    KEEP(*(.config_BFC6FFFC))
  } > config_BFC6FFFC
}
SECTIONS
{
  /* Boot Sections */
  .reset _RESET_ADDR :
  {
    KEEP(*(.reset))
    KEEP(*(.reset.startup))
  } > kseg1_boot_mem
  .bev_excpt _BEV_EXCPT_ADDR :
  {
    KEEP(*(.bev_handler))
  } > kseg1_boot_mem
  .cache_init :
  {
    *(.cache_init)
    *(.cache_init.*)
  } > kseg1_boot_mem_4B0
  .simple_tlb_refill_excpt _SIMPLE_TLB_REFILL_EXCPT_ADDR :
  {
    KEEP(*(.simple_tlb_refill_vector))
  } > kseg0_program_mem
  .cache_err_excpt _CACHE_ERR_EXCPT_ADDR :
  {
    KEEP(*(.cache_err_vector))
  } > kseg0_program_mem
  .app_excpt _GEN_EXCPT_ADDR :
  {
    KEEP(*(.gen_handler))
  } > kseg0_program_mem

  /* Interrupt vector table with vector offsets */
  .vectors _ebase_address + 0x200 :
  {
    /*  Symbol __vector_offset_n points to .vector_n if it exists,
     *  otherwise points to the default handler. The
     *  vector_offset_init.o module then provides a .data section
     *  containing values used to initialize the vector-offset SFRs
     *  in the crt0 startup code.
     */
    __vector_offset_0 = (DEFINED(__vector_dispatch_0) ? (. - _ebase_address) : __vector_offset_default);
    KEEP(*(.vector_0))
    __vector_offset_1 = (DEFINED(__vector_dispatch_1) ? (. - _ebase_address) : __vector_offset_default);
    KEEP(*(.vector_1))
    __vector_offset_2 = (DEFINED(__vector_dispatch_2) ? (. - _ebase_address) : __vector_offset_default);
    KEEP(*(.vector_2))
    __vector_offset_3 = (DEFINED(__vector_dispatch_3) ? (. - _ebase_address) : __vector_offset_default);
    KEEP(*(.vector_3))
    __vector_offset_4 = (DEFINED(__vector_dispatch_4) ? (. - _ebase_address) : __vector_offset_default);
    KEEP(*(.vector_4))
    __vector_offset_5 = (DEFINED(__vector_dispatch_5) ? (. - _ebase_address) : __vector_offset_default);
    KEEP(*(.vector_5))
    __vector_offset_6 = (DEFINED(__vector_dispatch_6) ? (. - _ebase_address) : __vector_offset_default);
    KEEP(*(.vector_6))
    __vector_offset_7 = (DEFINED(__vector_dispatch_7) ? (. - _ebase_address) : __vector_offset_default);
    KEEP(*(.vector_7))
    __vector_offset_8 = (DEFINED(__vector_dispatch_8) ? (. - _ebase_address) : __vector_offset_default);
    KEEP(*(.vector_8))
    __vector_offset_9 = (DEFINED(__vector_dispatch_9) ? (. - _ebase_address) : __vector_offset_default);
    KEEP(*(.vector_9))
    __vector_offset_10 = (DEFINED(__vector_dispatch_10) ? (. - _ebase_address) : __vector_offset_default);
    KEEP(*(.vector_10))
    __vector_offset_11 = (DEFINED(__vector_dispatch_11) ? (. - _ebase_address) : __vector_offset_default);
    KEEP(*(.vector_11))
    __vector_offset_12 = (DEFINED(__vector_dispatch_12) ? (. - _ebase_address) : __vector_offset_default);
    KEEP(*(.vector_12))
    __vector_offset_13 = (DEFINED(__vector_dispatch_13) ? (. - _ebase_address) : __vector_offset_default);
    KEEP(*(.vector_13))
    __vector_offset_14 = (DEFINED(__vector_dispatch_14) ? (. - _ebase_address) : __vector_offset_default);
    KEEP(*(.vector_14))
    __vector_offset_15 = (DEFINED(__vector_dispatch_15) ? (. - _ebase_address) : __vector_offset_default);
    KEEP(*(.vector_15))
    __vector_offset_16 = (DEFINED(__vector_dispatch_16) ? (. - _ebase_address) : __vector_offset_default);
    KEEP(*(.vector_16))
    __vector_offset_17 = (DEFINED(__vector_dispatch_17) ? (. - _ebase_address) : __vector_offset_default);
    KEEP(*(.vector_17))
    __vector_offset_18 = (DEFINED(__vector_dispatch_18) ? (. - _ebase_address) : __vector_offset_default);
    KEEP(*(.vector_18))
    __vector_offset_19 = (DEFINED(__vector_dispatch_19) ? (. - _ebase_address) : __vector_offset_default);
    KEEP(*(.vector_19))
    __vector_offset_20 = (DEFINED(__vector_dispatch_20) ? (. - _ebase_address) : __vector_offset_default);
    KEEP(*(.vector_20))
    __vector_offset_21 = (DEFINED(__vector_dispatch_21) ? (. - _ebase_address) : __vector_offset_default);
    KEEP(*(.vector_21))
    __vector_offset_22 = (DEFINED(__vector_dispatch_22) ? (. - _ebase_address) : __vector_offset_default);
    KEEP(*(.vector_22))
    __vector_offset_23 = (DEFINED(__vector_dispatch_23) ? (. - _ebase_address) : __vector_offset_default);
    KEEP(*(.vector_23))
    __vector_offset_24 = (DEFINED(__vector_dispatch_24) ? (. - _ebase_address) : __vector_offset_default);
    KEEP(*(.vector_24))
    __vector_offset_25 = (DEFINED(__vector_dispatch_25) ? (. - _ebase_address) : __vector_offset_default);
    KEEP(*(.vector_25))
    __vector_offset_26 = (DEFINED(__vector_dispatch_26) ? (. - _ebase_address) : __vector_offset_default);
    KEEP(*(.vector_26))
    __vector_offset_27 = (DEFINED(__vector_dispatch_27) ? (. - _ebase_address) : __vector_offset_default);
    KEEP(*(.vector_27))
    __vector_offset_28 = (DEFINED(__vector_dispatch_28) ? (. - _ebase_address) : __vector_offset_default);
    KEEP(*(.vector_28))
    __vector_offset_29 = (DEFINED(__vector_dispatch_29) ? (. - _ebase_address) : __vector_offset_default);
    KEEP(*(.vector_29))
    __vector_offset_30 = (DEFINED(__vector_dispatch_30) ? (. - _ebase_address) : __vector_offset_default);
    KEEP(*(.vector_30))
    __vector_offset_31 = (DEFINED(__vector_dispatch_31) ? (. - _ebase_address) : __vector_offset_default);
    KEEP(*(.vector_31))
    __vector_offset_32 = (DEFINED(__vector_dispatch_32) ? (. - _ebase_address) : __vector_offset_default);
    KEEP(*(.vector_32))
    __vector_offset_33 = (DEFINED(__vector_dispatch_33) ? (. - _ebase_address) : __vector_offset_default);
    KEEP(*(.vector_33))
    __vector_offset_34 = (DEFINED(__vector_dispatch_34) ? (. - _ebase_address) : __vector_offset_default);
    KEEP(*(.vector_34))
    __vector_offset_35 = (DEFINED(__vector_dispatch_35) ? (. - _ebase_address) : __vector_offset_default);
    KEEP(*(.vector_35))
    __vector_offset_36 = (DEFINED(__vector_dispatch_36) ? (. - _ebase_address) : __vector_offset_default);
    KEEP(*(.vector_36))
    __vector_offset_37 = (DEFINED(__vector_dispatch_37) ? (. - _ebase_address) : __vector_offset_default);
    KEEP(*(.vector_37))
    __vector_offset_38 = (DEFINED(__vector_dispatch_38) ? (. - _ebase_address) : __vector_offset_default);
    KEEP(*(.vector_38))
    __vector_offset_39 = (DEFINED(__vector_dispatch_39) ? (. - _ebase_address) : __vector_offset_default);
    KEEP(*(.vector_39))
    __vector_offset_40 = (DEFINED(__vector_dispatch_40) ? (. - _ebase_address) : __vector_offset_default);
    KEEP(*(.vector_40))
    __vector_offset_41 = (DEFINED(__vector_dispatch_41) ? (. - _ebase_address) : __vector_offset_default);
    KEEP(*(.vector_41))
    __vector_offset_42 = (DEFINED(__vector_dispatch_42) ? (. - _ebase_address) : __vector_offset_default);
    KEEP(*(.vector_42))
    __vector_offset_43 = (DEFINED(__vector_dispatch_43) ? (. - _ebase_address) : __vector_offset_default);
    KEEP(*(.vector_43))
    __vector_offset_44 = (DEFINED(__vector_dispatch_44) ? (. - _ebase_address) : __vector_offset_default);
    KEEP(*(.vector_44))
    __vector_offset_45 = (DEFINED(__vector_dispatch_45) ? (. - _ebase_address) : __vector_offset_default);
    KEEP(*(.vector_45))
    __vector_offset_46 = (DEFINED(__vector_dispatch_46) ? (. - _ebase_address) : __vector_offset_default);
    KEEP(*(.vector_46))
    __vector_offset_47 = (DEFINED(__vector_dispatch_47) ? (. - _ebase_address) : __vector_offset_default);
    KEEP(*(.vector_47))
    __vector_offset_48 = (DEFINED(__vector_dispatch_48) ? (. - _ebase_address) : __vector_offset_default);
    KEEP(*(.vector_48))
    __vector_offset_49 = (DEFINED(__vector_dispatch_49) ? (. - _ebase_address) : __vector_offset_default);
    KEEP(*(.vector_49))
    __vector_offset_50 = (DEFINED(__vector_dispatch_50) ? (. - _ebase_address) : __vector_offset_default);
    KEEP(*(.vector_50))
    __vector_offset_51 = (DEFINED(__vector_dispatch_51) ? (. - _ebase_address) : __vector_offset_default);
    KEEP(*(.vector_51))
    __vector_offset_52 = (DEFINED(__vector_dispatch_52) ? (. - _ebase_address) : __vector_offset_default);
    KEEP(*(.vector_52))
    __vector_offset_53 = (DEFINED(__vector_dispatch_53) ? (. - _ebase_address) : __vector_offset_default);
    KEEP(*(.vector_53))
    __vector_offset_54 = (DEFINED(__vector_dispatch_54) ? (. - _ebase_address) : __vector_offset_default);
    KEEP(*(.vector_54))
    __vector_offset_55 = (DEFINED(__vector_dispatch_55) ? (. - _ebase_address) : __vector_offset_default);
    KEEP(*(.vector_55))
    __vector_offset_56 = (DEFINED(__vector_dispatch_56) ? (. - _ebase_address) : __vector_offset_default);
    KEEP(*(.vector_56))
    __vector_offset_57 = (DEFINED(__vector_dispatch_57) ? (. - _ebase_address) : __vector_offset_default);
    KEEP(*(.vector_57))
    __vector_offset_58 = (DEFINED(__vector_dispatch_58) ? (. - _ebase_address) : __vector_offset_default);
    KEEP(*(.vector_58))
    __vector_offset_59 = (DEFINED(__vector_dispatch_59) ? (. - _ebase_address) : __vector_offset_default);
    KEEP(*(.vector_59))
    __vector_offset_60 = (DEFINED(__vector_dispatch_60) ? (. - _ebase_address) : __vector_offset_default);
    KEEP(*(.vector_60))
    __vector_offset_61 = (DEFINED(__vector_dispatch_61) ? (. - _ebase_address) : __vector_offset_default);
    KEEP(*(.vector_61))
    __vector_offset_62 = (DEFINED(__vector_dispatch_62) ? (. - _ebase_address) : __vector_offset_default);
    KEEP(*(.vector_62))
    __vector_offset_63 = (DEFINED(__vector_dispatch_63) ? (. - _ebase_address) : __vector_offset_default);
    KEEP(*(.vector_63))
    __vector_offset_64 = (DEFINED(__vector_dispatch_64) ? (. - _ebase_address) : __vector_offset_default);
    KEEP(*(.vector_64))
    __vector_offset_65 = (DEFINED(__vector_dispatch_65) ? (. - _ebase_address) : __vector_offset_default);
    KEEP(*(.vector_65))
    __vector_offset_66 = (DEFINED(__vector_dispatch_66) ? (. - _ebase_address) : __vector_offset_default);
    KEEP(*(.vector_66))
    __vector_offset_67 = (DEFINED(__vector_dispatch_67) ? (. - _ebase_address) : __vector_offset_default);
    KEEP(*(.vector_67))
    __vector_offset_68 = (DEFINED(__vector_dispatch_68) ? (. - _ebase_address) : __vector_offset_default);
    KEEP(*(.vector_68))
    __vector_offset_69 = (DEFINED(__vector_dispatch_69) ? (. - _ebase_address) : __vector_offset_default);
    KEEP(*(.vector_69))
    __vector_offset_70 = (DEFINED(__vector_dispatch_70) ? (. - _ebase_address) : __vector_offset_default);
    KEEP(*(.vector_70))
    __vector_offset_71 = (DEFINED(__vector_dispatch_71) ? (. - _ebase_address) : __vector_offset_default);
    KEEP(*(.vector_71))
    __vector_offset_72 = (DEFINED(__vector_dispatch_72) ? (. - _ebase_address) : __vector_offset_default);
    KEEP(*(.vector_72))
    __vector_offset_73 = (DEFINED(__vector_dispatch_73) ? (. - _ebase_address) : __vector_offset_default);
    KEEP(*(.vector_73))
    __vector_offset_74 = (DEFINED(__vector_dispatch_74) ? (. - _ebase_address) : __vector_offset_default);
    KEEP(*(.vector_74))
    __vector_offset_75 = (DEFINED(__vector_dispatch_75) ? (. - _ebase_address) : __vector_offset_default);
    KEEP(*(.vector_75))
    __vector_offset_76 = (DEFINED(__vector_dispatch_76) ? (. - _ebase_address) : __vector_offset_default);
    KEEP(*(.vector_76))
    __vector_offset_77 = (DEFINED(__vector_dispatch_77) ? (. - _ebase_address) : __vector_offset_default);
    KEEP(*(.vector_77))
    __vector_offset_78 = (DEFINED(__vector_dispatch_78) ? (. - _ebase_address) : __vector_offset_default);
    KEEP(*(.vector_78))
    __vector_offset_79 = (DEFINED(__vector_dispatch_79) ? (. - _ebase_address) : __vector_offset_default);
    KEEP(*(.vector_79))
    __vector_offset_80 = (DEFINED(__vector_dispatch_80) ? (. - _ebase_address) : __vector_offset_default);
    KEEP(*(.vector_80))
    __vector_offset_81 = (DEFINED(__vector_dispatch_81) ? (. - _ebase_address) : __vector_offset_default);
    KEEP(*(.vector_81))
    __vector_offset_82 = (DEFINED(__vector_dispatch_82) ? (. - _ebase_address) : __vector_offset_default);
    KEEP(*(.vector_82))
    __vector_offset_83 = (DEFINED(__vector_dispatch_83) ? (. - _ebase_address) : __vector_offset_default);
    KEEP(*(.vector_83))
    __vector_offset_84 = (DEFINED(__vector_dispatch_84) ? (. - _ebase_address) : __vector_offset_default);
    KEEP(*(.vector_84))
    __vector_offset_85 = (DEFINED(__vector_dispatch_85) ? (. - _ebase_address) : __vector_offset_default);
    KEEP(*(.vector_85))
    __vector_offset_86 = (DEFINED(__vector_dispatch_86) ? (. - _ebase_address) : __vector_offset_default);
    KEEP(*(.vector_86))
    __vector_offset_87 = (DEFINED(__vector_dispatch_87) ? (. - _ebase_address) : __vector_offset_default);
    KEEP(*(.vector_87))
    __vector_offset_88 = (DEFINED(__vector_dispatch_88) ? (. - _ebase_address) : __vector_offset_default);
    KEEP(*(.vector_88))
    __vector_offset_89 = (DEFINED(__vector_dispatch_89) ? (. - _ebase_address) : __vector_offset_default);
    KEEP(*(.vector_89))
    __vector_offset_90 = (DEFINED(__vector_dispatch_90) ? (. - _ebase_address) : __vector_offset_default);
    KEEP(*(.vector_90))
    __vector_offset_91 = (DEFINED(__vector_dispatch_91) ? (. - _ebase_address) : __vector_offset_default);
    KEEP(*(.vector_91))
    __vector_offset_92 = (DEFINED(__vector_dispatch_92) ? (. - _ebase_address) : __vector_offset_default);
    KEEP(*(.vector_92))
    __vector_offset_93 = (DEFINED(__vector_dispatch_93) ? (. - _ebase_address) : __vector_offset_default);
    KEEP(*(.vector_93))
    __vector_offset_94 = (DEFINED(__vector_dispatch_94) ? (. - _ebase_address) : __vector_offset_default);
    KEEP(*(.vector_94))
    __vector_offset_95 = (DEFINED(__vector_dispatch_95) ? (. - _ebase_address) : __vector_offset_default);
    KEEP(*(.vector_95))
    __vector_offset_96 = (DEFINED(__vector_dispatch_96) ? (. - _ebase_address) : __vector_offset_default);
    KEEP(*(.vector_96))
    __vector_offset_97 = (DEFINED(__vector_dispatch_97) ? (. - _ebase_address) : __vector_offset_default);
    KEEP(*(.vector_97))
    __vector_offset_98 = (DEFINED(__vector_dispatch_98) ? (. - _ebase_address) : __vector_offset_default);
    KEEP(*(.vector_98))
    __vector_offset_99 = (DEFINED(__vector_dispatch_99) ? (. - _ebase_address) : __vector_offset_default);
    KEEP(*(.vector_99))
    __vector_offset_100 = (DEFINED(__vector_dispatch_100) ? (. - _ebase_address) : __vector_offset_default);
    KEEP(*(.vector_100))
    __vector_offset_101 = (DEFINED(__vector_dispatch_101) ? (. - _ebase_address) : __vector_offset_default);
    KEEP(*(.vector_101))
    __vector_offset_102 = (DEFINED(__vector_dispatch_102) ? (. - _ebase_address) : __vector_offset_default);
    KEEP(*(.vector_102))
    __vector_offset_103 = (DEFINED(__vector_dispatch_103) ? (. - _ebase_address) : __vector_offset_default);
    KEEP(*(.vector_103))
    __vector_offset_104 = (DEFINED(__vector_dispatch_104) ? (. - _ebase_address) : __vector_offset_default);
    KEEP(*(.vector_104))
    __vector_offset_105 = (DEFINED(__vector_dispatch_105) ? (. - _ebase_address) : __vector_offset_default);
    KEEP(*(.vector_105))
    __vector_offset_106 = (DEFINED(__vector_dispatch_106) ? (. - _ebase_address) : __vector_offset_default);
    KEEP(*(.vector_106))
    __vector_offset_107 = (DEFINED(__vector_dispatch_107) ? (. - _ebase_address) : __vector_offset_default);
    KEEP(*(.vector_107))
    __vector_offset_108 = (DEFINED(__vector_dispatch_108) ? (. - _ebase_address) : __vector_offset_default);
    KEEP(*(.vector_108))
    __vector_offset_109 = (DEFINED(__vector_dispatch_109) ? (. - _ebase_address) : __vector_offset_default);
    KEEP(*(.vector_109))
    __vector_offset_110 = (DEFINED(__vector_dispatch_110) ? (. - _ebase_address) : __vector_offset_default);
    KEEP(*(.vector_110))
    __vector_offset_111 = (DEFINED(__vector_dispatch_111) ? (. - _ebase_address) : __vector_offset_default);
    KEEP(*(.vector_111))
    __vector_offset_112 = (DEFINED(__vector_dispatch_112) ? (. - _ebase_address) : __vector_offset_default);
    KEEP(*(.vector_112))
    __vector_offset_113 = (DEFINED(__vector_dispatch_113) ? (. - _ebase_address) : __vector_offset_default);
    KEEP(*(.vector_113))
    __vector_offset_114 = (DEFINED(__vector_dispatch_114) ? (. - _ebase_address) : __vector_offset_default);
    KEEP(*(.vector_114))
    __vector_offset_115 = (DEFINED(__vector_dispatch_115) ? (. - _ebase_address) : __vector_offset_default);
    KEEP(*(.vector_115))
    __vector_offset_116 = (DEFINED(__vector_dispatch_116) ? (. - _ebase_address) : __vector_offset_default);
    KEEP(*(.vector_116))
    __vector_offset_117 = (DEFINED(__vector_dispatch_117) ? (. - _ebase_address) : __vector_offset_default);
    KEEP(*(.vector_117))
    __vector_offset_118 = (DEFINED(__vector_dispatch_118) ? (. - _ebase_address) : __vector_offset_default);
    KEEP(*(.vector_118))
    __vector_offset_119 = (DEFINED(__vector_dispatch_119) ? (. - _ebase_address) : __vector_offset_default);
    KEEP(*(.vector_119))
    __vector_offset_120 = (DEFINED(__vector_dispatch_120) ? (. - _ebase_address) : __vector_offset_default);
    KEEP(*(.vector_120))
    __vector_offset_121 = (DEFINED(__vector_dispatch_121) ? (. - _ebase_address) : __vector_offset_default);
    KEEP(*(.vector_121))
    __vector_offset_122 = (DEFINED(__vector_dispatch_122) ? (. - _ebase_address) : __vector_offset_default);
    KEEP(*(.vector_122))
    __vector_offset_123 = (DEFINED(__vector_dispatch_123) ? (. - _ebase_address) : __vector_offset_default);
    KEEP(*(.vector_123))
    __vector_offset_124 = (DEFINED(__vector_dispatch_124) ? (. - _ebase_address) : __vector_offset_default);
    KEEP(*(.vector_124))
    __vector_offset_125 = (DEFINED(__vector_dispatch_125) ? (. - _ebase_address) : __vector_offset_default);
    KEEP(*(.vector_125))
    __vector_offset_126 = (DEFINED(__vector_dispatch_126) ? (. - _ebase_address) : __vector_offset_default);
    KEEP(*(.vector_126))
    __vector_offset_127 = (DEFINED(__vector_dispatch_127) ? (. - _ebase_address) : __vector_offset_default);
    KEEP(*(.vector_127))
    __vector_offset_128 = (DEFINED(__vector_dispatch_128) ? (. - _ebase_address) : __vector_offset_default);
    KEEP(*(.vector_128))
    __vector_offset_129 = (DEFINED(__vector_dispatch_129) ? (. - _ebase_address) : __vector_offset_default);
    KEEP(*(.vector_129))
    __vector_offset_130 = (DEFINED(__vector_dispatch_130) ? (. - _ebase_address) : __vector_offset_default);
    KEEP(*(.vector_130))
    __vector_offset_131 = (DEFINED(__vector_dispatch_131) ? (. - _ebase_address) : __vector_offset_default);
    KEEP(*(.vector_131))
    __vector_offset_132 = (DEFINED(__vector_dispatch_132) ? (. - _ebase_address) : __vector_offset_default);
    KEEP(*(.vector_132))
    __vector_offset_133 = (DEFINED(__vector_dispatch_133) ? (. - _ebase_address) : __vector_offset_default);
    KEEP(*(.vector_133))
    __vector_offset_134 = (DEFINED(__vector_dispatch_134) ? (. - _ebase_address) : __vector_offset_default);
    KEEP(*(.vector_134))
    __vector_offset_135 = (DEFINED(__vector_dispatch_135) ? (. - _ebase_address) : __vector_offset_default);
    KEEP(*(.vector_135))
    __vector_offset_136 = (DEFINED(__vector_dispatch_136) ? (. - _ebase_address) : __vector_offset_default);
    KEEP(*(.vector_136))
    __vector_offset_137 = (DEFINED(__vector_dispatch_137) ? (. - _ebase_address) : __vector_offset_default);
    KEEP(*(.vector_137))
    __vector_offset_138 = (DEFINED(__vector_dispatch_138) ? (. - _ebase_address) : __vector_offset_default);
    KEEP(*(.vector_138))
    __vector_offset_139 = (DEFINED(__vector_dispatch_139) ? (. - _ebase_address) : __vector_offset_default);
    KEEP(*(.vector_139))
    __vector_offset_140 = (DEFINED(__vector_dispatch_140) ? (. - _ebase_address) : __vector_offset_default);
    KEEP(*(.vector_140))
    __vector_offset_141 = (DEFINED(__vector_dispatch_141) ? (. - _ebase_address) : __vector_offset_default);
    KEEP(*(.vector_141))
    __vector_offset_142 = (DEFINED(__vector_dispatch_142) ? (. - _ebase_address) : __vector_offset_default);
    KEEP(*(.vector_142))
    __vector_offset_143 = (DEFINED(__vector_dispatch_143) ? (. - _ebase_address) : __vector_offset_default);
    KEEP(*(.vector_143))
    __vector_offset_144 = (DEFINED(__vector_dispatch_144) ? (. - _ebase_address) : __vector_offset_default);
    KEEP(*(.vector_144))
    __vector_offset_145 = (DEFINED(__vector_dispatch_145) ? (. - _ebase_address) : __vector_offset_default);
    KEEP(*(.vector_145))
    __vector_offset_146 = (DEFINED(__vector_dispatch_146) ? (. - _ebase_address) : __vector_offset_default);
    KEEP(*(.vector_146))
    __vector_offset_147 = (DEFINED(__vector_dispatch_147) ? (. - _ebase_address) : __vector_offset_default);
    KEEP(*(.vector_147))
    __vector_offset_148 = (DEFINED(__vector_dispatch_148) ? (. - _ebase_address) : __vector_offset_default);
    KEEP(*(.vector_148))
    __vector_offset_149 = (DEFINED(__vector_dispatch_149) ? (. - _ebase_address) : __vector_offset_default);
    KEEP(*(.vector_149))
    __vector_offset_150 = (DEFINED(__vector_dispatch_150) ? (. - _ebase_address) : __vector_offset_default);
    KEEP(*(.vector_150))
    __vector_offset_151 = (DEFINED(__vector_dispatch_151) ? (. - _ebase_address) : __vector_offset_default);
    KEEP(*(.vector_151))
    __vector_offset_152 = (DEFINED(__vector_dispatch_152) ? (. - _ebase_address) : __vector_offset_default);
    KEEP(*(.vector_152))
    __vector_offset_153 = (DEFINED(__vector_dispatch_153) ? (. - _ebase_address) : __vector_offset_default);
    KEEP(*(.vector_153))
    __vector_offset_154 = (DEFINED(__vector_dispatch_154) ? (. - _ebase_address) : __vector_offset_default);
    KEEP(*(.vector_154))
    __vector_offset_155 = (DEFINED(__vector_dispatch_155) ? (. - _ebase_address) : __vector_offset_default);
    KEEP(*(.vector_155))
    __vector_offset_156 = (DEFINED(__vector_dispatch_156) ? (. - _ebase_address) : __vector_offset_default);
    KEEP(*(.vector_156))
    __vector_offset_157 = (DEFINED(__vector_dispatch_157) ? (. - _ebase_address) : __vector_offset_default);
    KEEP(*(.vector_157))
    __vector_offset_158 = (DEFINED(__vector_dispatch_158) ? (. - _ebase_address) : __vector_offset_default);
    KEEP(*(.vector_158))
    __vector_offset_159 = (DEFINED(__vector_dispatch_159) ? (. - _ebase_address) : __vector_offset_default);
    KEEP(*(.vector_159))
    __vector_offset_160 = (DEFINED(__vector_dispatch_160) ? (. - _ebase_address) : __vector_offset_default);
    KEEP(*(.vector_160))
    __vector_offset_161 = (DEFINED(__vector_dispatch_161) ? (. - _ebase_address) : __vector_offset_default);
    KEEP(*(.vector_161))
    __vector_offset_162 = (DEFINED(__vector_dispatch_162) ? (. - _ebase_address) : __vector_offset_default);
    KEEP(*(.vector_162))
    __vector_offset_163 = (DEFINED(__vector_dispatch_163) ? (. - _ebase_address) : __vector_offset_default);
    KEEP(*(.vector_163))
    __vector_offset_164 = (DEFINED(__vector_dispatch_164) ? (. - _ebase_address) : __vector_offset_default);
    KEEP(*(.vector_164))
    __vector_offset_165 = (DEFINED(__vector_dispatch_165) ? (. - _ebase_address) : __vector_offset_default);
    KEEP(*(.vector_165))
    __vector_offset_166 = (DEFINED(__vector_dispatch_166) ? (. - _ebase_address) : __vector_offset_default);
    KEEP(*(.vector_166))
    __vector_offset_167 = (DEFINED(__vector_dispatch_167) ? (. - _ebase_address) : __vector_offset_default);
    KEEP(*(.vector_167))
    __vector_offset_168 = (DEFINED(__vector_dispatch_168) ? (. - _ebase_address) : __vector_offset_default);
    KEEP(*(.vector_168))
    __vector_offset_169 = (DEFINED(__vector_dispatch_169) ? (. - _ebase_address) : __vector_offset_default);
    KEEP(*(.vector_169))
    __vector_offset_170 = (DEFINED(__vector_dispatch_170) ? (. - _ebase_address) : __vector_offset_default);
    KEEP(*(.vector_170))
    __vector_offset_171 = (DEFINED(__vector_dispatch_171) ? (. - _ebase_address) : __vector_offset_default);
    KEEP(*(.vector_171))
    __vector_offset_172 = (DEFINED(__vector_dispatch_172) ? (. - _ebase_address) : __vector_offset_default);
    KEEP(*(.vector_172))
    __vector_offset_173 = (DEFINED(__vector_dispatch_173) ? (. - _ebase_address) : __vector_offset_default);
    KEEP(*(.vector_173))
    __vector_offset_174 = (DEFINED(__vector_dispatch_174) ? (. - _ebase_address) : __vector_offset_default);
    KEEP(*(.vector_174))
    __vector_offset_175 = (DEFINED(__vector_dispatch_175) ? (. - _ebase_address) : __vector_offset_default);
    KEEP(*(.vector_175))
    __vector_offset_176 = (DEFINED(__vector_dispatch_176) ? (. - _ebase_address) : __vector_offset_default);
    KEEP(*(.vector_176))
    __vector_offset_177 = (DEFINED(__vector_dispatch_177) ? (. - _ebase_address) : __vector_offset_default);
    KEEP(*(.vector_177))
    __vector_offset_178 = (DEFINED(__vector_dispatch_178) ? (. - _ebase_address) : __vector_offset_default);
    KEEP(*(.vector_178))
    __vector_offset_179 = (DEFINED(__vector_dispatch_179) ? (. - _ebase_address) : __vector_offset_default);
    KEEP(*(.vector_179))
    __vector_offset_180 = (DEFINED(__vector_dispatch_180) ? (. - _ebase_address) : __vector_offset_default);
    KEEP(*(.vector_180))
    __vector_offset_181 = (DEFINED(__vector_dispatch_181) ? (. - _ebase_address) : __vector_offset_default);
    KEEP(*(.vector_181))
    __vector_offset_182 = (DEFINED(__vector_dispatch_182) ? (. - _ebase_address) : __vector_offset_default);
    KEEP(*(.vector_182))
    __vector_offset_183 = (DEFINED(__vector_dispatch_183) ? (. - _ebase_address) : __vector_offset_default);
    KEEP(*(.vector_183))
    __vector_offset_184 = (DEFINED(__vector_dispatch_184) ? (. - _ebase_address) : __vector_offset_default);
    KEEP(*(.vector_184))
    __vector_offset_185 = (DEFINED(__vector_dispatch_185) ? (. - _ebase_address) : __vector_offset_default);
    KEEP(*(.vector_185))
    __vector_offset_186 = (DEFINED(__vector_dispatch_186) ? (. - _ebase_address) : __vector_offset_default);
    KEEP(*(.vector_186))
    __vector_offset_187 = (DEFINED(__vector_dispatch_187) ? (. - _ebase_address) : __vector_offset_default);
    KEEP(*(.vector_187))
    __vector_offset_188 = (DEFINED(__vector_dispatch_188) ? (. - _ebase_address) : __vector_offset_default);
    KEEP(*(.vector_188))
    __vector_offset_189 = (DEFINED(__vector_dispatch_189) ? (. - _ebase_address) : __vector_offset_default);
    KEEP(*(.vector_189))
    __vector_offset_190 = (DEFINED(__vector_dispatch_190) ? (. - _ebase_address) : __vector_offset_default);
    KEEP(*(.vector_190))
    __vector_offset_191 = (DEFINED(__vector_dispatch_191) ? (. - _ebase_address) : __vector_offset_default);
    KEEP(*(.vector_191))
    __vector_offset_192 = (DEFINED(__vector_dispatch_192) ? (. - _ebase_address) : __vector_offset_default);
    KEEP(*(.vector_192))
    __vector_offset_193 = (DEFINED(__vector_dispatch_193) ? (. - _ebase_address) : __vector_offset_default);
    KEEP(*(.vector_193))
    __vector_offset_194 = (DEFINED(__vector_dispatch_194) ? (. - _ebase_address) : __vector_offset_default);
    KEEP(*(.vector_194))
    __vector_offset_195 = (DEFINED(__vector_dispatch_195) ? (. - _ebase_address) : __vector_offset_default);
    KEEP(*(.vector_195))
    __vector_offset_196 = (DEFINED(__vector_dispatch_196) ? (. - _ebase_address) : __vector_offset_default);
    KEEP(*(.vector_196))
    __vector_offset_197 = (DEFINED(__vector_dispatch_197) ? (. - _ebase_address) : __vector_offset_default);
    KEEP(*(.vector_197))
    __vector_offset_198 = (DEFINED(__vector_dispatch_198) ? (. - _ebase_address) : __vector_offset_default);
    KEEP(*(.vector_198))
    __vector_offset_199 = (DEFINED(__vector_dispatch_199) ? (. - _ebase_address) : __vector_offset_default);
    KEEP(*(.vector_199))
    __vector_offset_200 = (DEFINED(__vector_dispatch_200) ? (. - _ebase_address) : __vector_offset_default);
    KEEP(*(.vector_200))
    __vector_offset_201 = (DEFINED(__vector_dispatch_201) ? (. - _ebase_address) : __vector_offset_default);
    KEEP(*(.vector_201))
    __vector_offset_202 = (DEFINED(__vector_dispatch_202) ? (. - _ebase_address) : __vector_offset_default);
    KEEP(*(.vector_202))
    __vector_offset_203 = (DEFINED(__vector_dispatch_203) ? (. - _ebase_address) : __vector_offset_default);
    KEEP(*(.vector_203))
    __vector_offset_204 = (DEFINED(__vector_dispatch_204) ? (. - _ebase_address) : __vector_offset_default);
    KEEP(*(.vector_204))
    __vector_offset_205 = (DEFINED(__vector_dispatch_205) ? (. - _ebase_address) : __vector_offset_default);
    KEEP(*(.vector_205))
    __vector_offset_206 = (DEFINED(__vector_dispatch_206) ? (. - _ebase_address) : __vector_offset_default);
    KEEP(*(.vector_206))
    __vector_offset_207 = (DEFINED(__vector_dispatch_207) ? (. - _ebase_address) : __vector_offset_default);
    KEEP(*(.vector_207))
    __vector_offset_208 = (DEFINED(__vector_dispatch_208) ? (. - _ebase_address) : __vector_offset_default);
    KEEP(*(.vector_208))
    __vector_offset_209 = (DEFINED(__vector_dispatch_209) ? (. - _ebase_address) : __vector_offset_default);
    KEEP(*(.vector_209))
    __vector_offset_210 = (DEFINED(__vector_dispatch_210) ? (. - _ebase_address) : __vector_offset_default);
    KEEP(*(.vector_210))
    __vector_offset_211 = (DEFINED(__vector_dispatch_211) ? (. - _ebase_address) : __vector_offset_default);
    KEEP(*(.vector_211))
    __vector_offset_212 = (DEFINED(__vector_dispatch_212) ? (. - _ebase_address) : __vector_offset_default);
    KEEP(*(.vector_212))
    __vector_offset_213 = (DEFINED(__vector_dispatch_213) ? (. - _ebase_address) : __vector_offset_default);
    KEEP(*(.vector_213))
    __vector_offset_214 = (DEFINED(__vector_dispatch_214) ? (. - _ebase_address) : __vector_offset_default);
    KEEP(*(.vector_214))
    __vector_offset_215 = (DEFINED(__vector_dispatch_215) ? (. - _ebase_address) : __vector_offset_default);
    KEEP(*(.vector_215))
    __vector_offset_216 = (DEFINED(__vector_dispatch_216) ? (. - _ebase_address) : __vector_offset_default);
    KEEP(*(.vector_216))
    __vector_offset_217 = (DEFINED(__vector_dispatch_217) ? (. - _ebase_address) : __vector_offset_default);
    KEEP(*(.vector_217))
    __vector_offset_218 = (DEFINED(__vector_dispatch_218) ? (. - _ebase_address) : __vector_offset_default);
    KEEP(*(.vector_218))
    __vector_offset_219 = (DEFINED(__vector_dispatch_219) ? (. - _ebase_address) : __vector_offset_default);
    KEEP(*(.vector_219))
    __vector_offset_220 = (DEFINED(__vector_dispatch_220) ? (. - _ebase_address) : __vector_offset_default);
    KEEP(*(.vector_220))
    __vector_offset_221 = (DEFINED(__vector_dispatch_221) ? (. - _ebase_address) : __vector_offset_default);
    KEEP(*(.vector_221))
    __vector_offset_222 = (DEFINED(__vector_dispatch_222) ? (. - _ebase_address) : __vector_offset_default);
    KEEP(*(.vector_222))
    __vector_offset_223 = (DEFINED(__vector_dispatch_223) ? (. - _ebase_address) : __vector_offset_default);
    KEEP(*(.vector_223))
    __vector_offset_224 = (DEFINED(__vector_dispatch_224) ? (. - _ebase_address) : __vector_offset_default);
    KEEP(*(.vector_224))
    __vector_offset_225 = (DEFINED(__vector_dispatch_225) ? (. - _ebase_address) : __vector_offset_default);
    KEEP(*(.vector_225))
    __vector_offset_226 = (DEFINED(__vector_dispatch_226) ? (. - _ebase_address) : __vector_offset_default);
    KEEP(*(.vector_226))
    __vector_offset_227 = (DEFINED(__vector_dispatch_227) ? (. - _ebase_address) : __vector_offset_default);
    KEEP(*(.vector_227))
    __vector_offset_228 = (DEFINED(__vector_dispatch_228) ? (. - _ebase_address) : __vector_offset_default);
    KEEP(*(.vector_228))
    __vector_offset_229 = (DEFINED(__vector_dispatch_229) ? (. - _ebase_address) : __vector_offset_default);
    KEEP(*(.vector_229))
    __vector_offset_230 = (DEFINED(__vector_dispatch_230) ? (. - _ebase_address) : __vector_offset_default);
    KEEP(*(.vector_230))
    __vector_offset_231 = (DEFINED(__vector_dispatch_231) ? (. - _ebase_address) : __vector_offset_default);
    KEEP(*(.vector_231))
    __vector_offset_232 = (DEFINED(__vector_dispatch_232) ? (. - _ebase_address) : __vector_offset_default);
    KEEP(*(.vector_232))
    __vector_offset_233 = (DEFINED(__vector_dispatch_233) ? (. - _ebase_address) : __vector_offset_default);
    KEEP(*(.vector_233))
    __vector_offset_234 = (DEFINED(__vector_dispatch_234) ? (. - _ebase_address) : __vector_offset_default);
    KEEP(*(.vector_234))
    __vector_offset_235 = (DEFINED(__vector_dispatch_235) ? (. - _ebase_address) : __vector_offset_default);
    KEEP(*(.vector_235))
    __vector_offset_236 = (DEFINED(__vector_dispatch_236) ? (. - _ebase_address) : __vector_offset_default);
    KEEP(*(.vector_236))
    __vector_offset_237 = (DEFINED(__vector_dispatch_237) ? (. - _ebase_address) : __vector_offset_default);
    KEEP(*(.vector_237))
    __vector_offset_238 = (DEFINED(__vector_dispatch_238) ? (. - _ebase_address) : __vector_offset_default);
    KEEP(*(.vector_238))
    __vector_offset_239 = (DEFINED(__vector_dispatch_239) ? (. - _ebase_address) : __vector_offset_default);
    KEEP(*(.vector_239))
    __vector_offset_240 = (DEFINED(__vector_dispatch_240) ? (. - _ebase_address) : __vector_offset_default);
    KEEP(*(.vector_240))
    __vector_offset_241 = (DEFINED(__vector_dispatch_241) ? (. - _ebase_address) : __vector_offset_default);
    KEEP(*(.vector_241))
    __vector_offset_242 = (DEFINED(__vector_dispatch_242) ? (. - _ebase_address) : __vector_offset_default);
    KEEP(*(.vector_242))
    __vector_offset_243 = (DEFINED(__vector_dispatch_243) ? (. - _ebase_address) : __vector_offset_default);
    KEEP(*(.vector_243))
    __vector_offset_244 = (DEFINED(__vector_dispatch_244) ? (. - _ebase_address) : __vector_offset_default);
    KEEP(*(.vector_244))
    __vector_offset_245 = (DEFINED(__vector_dispatch_245) ? (. - _ebase_address) : __vector_offset_default);
    KEEP(*(.vector_245))
    __vector_offset_246 = (DEFINED(__vector_dispatch_246) ? (. - _ebase_address) : __vector_offset_default);
    KEEP(*(.vector_246))
    __vector_offset_247 = (DEFINED(__vector_dispatch_247) ? (. - _ebase_address) : __vector_offset_default);
    KEEP(*(.vector_247))
    __vector_offset_248 = (DEFINED(__vector_dispatch_248) ? (. - _ebase_address) : __vector_offset_default);
    KEEP(*(.vector_248))
    __vector_offset_249 = (DEFINED(__vector_dispatch_249) ? (. - _ebase_address) : __vector_offset_default);
    KEEP(*(.vector_249))
    __vector_offset_250 = (DEFINED(__vector_dispatch_250) ? (. - _ebase_address) : __vector_offset_default);
    KEEP(*(.vector_250))
    __vector_offset_251 = (DEFINED(__vector_dispatch_251) ? (. - _ebase_address) : __vector_offset_default);
    KEEP(*(.vector_251))
    __vector_offset_252 = (DEFINED(__vector_dispatch_252) ? (. - _ebase_address) : __vector_offset_default);
    KEEP(*(.vector_252))
    __vector_offset_253 = (DEFINED(__vector_dispatch_253) ? (. - _ebase_address) : __vector_offset_default);
    KEEP(*(.vector_253))
    __vector_offset_254 = (DEFINED(__vector_dispatch_254) ? (. - _ebase_address) : __vector_offset_default);
    KEEP(*(.vector_254))
    __vector_offset_255 = (DEFINED(__vector_dispatch_255) ? (. - _ebase_address) : __vector_offset_default);
    KEEP(*(.vector_255))
    /* Default interrupt handler */
    __vector_offset_default = . - _ebase_address;
    KEEP(*(.vector_default))
  } > kseg0_program_mem

  /*  The startup code is in the .reset.startup section.
   *  Keep this here for backwards compatibility with older
   *  C32 v1.xx releases.
   */
  .startup ORIGIN(kseg0_boot_mem) :
  {
    KEEP(*(.startup))
  } > kseg0_boot_mem
  /* Code Sections - Note that input sections *(.text) and *(.text.*)
  ** are not mapped here. The best-fit allocator locates them,
  ** so that .text may flow around absolute sections as needed.
  */
  .text :
  {
    *(.stub .gnu.linkonce.t.*)
    KEEP (*(.text.*personality*))
    *(.mips16.fn.*)
    *(.mips16.call.*)
    *(.gnu.warning)
    . = ALIGN(4) ;
  } >kseg0_program_mem
  /* Global-namespace object initialization */
  .init   :
  {
    KEEP (*crti.o(.init))
    KEEP (*crtbegin.o(.init))
    KEEP (*(EXCLUDE_FILE (*crtend.o *crtend?.o *crtn.o ).init))
    KEEP (*crtend.o(.init))
    KEEP (*crtn.o(.init))
    . = ALIGN(4) ;
  } >kseg0_program_mem
  .fini   :
  {
    KEEP (*(.fini))
    . = ALIGN(4) ;
  } >kseg0_program_mem
  .preinit_array   :
  {
    PROVIDE_HIDDEN (__preinit_array_start = .);
    KEEP (*(.preinit_array))
    PROVIDE_HIDDEN (__preinit_array_end = .);
    . = ALIGN(4) ;
  } >kseg0_program_mem
  .init_array   :
  {
    PROVIDE_HIDDEN (__init_array_start = .);
    KEEP (*(SORT(.init_array.*)))
    KEEP (*(.init_array))
    PROVIDE_HIDDEN (__init_array_end = .);
    . = ALIGN(4) ;
  } >kseg0_program_mem
  .fini_array   :
  {
    PROVIDE_HIDDEN (__fini_array_start = .);
    KEEP (*(SORT(.fini_array.*)))
    KEEP (*(.fini_array))
    PROVIDE_HIDDEN (__fini_array_end = .);
    . = ALIGN(4) ;
  } >kseg0_program_mem
  .ctors   :
  {
    /* XC32 uses crtbegin.o to find the start of
       the constructors, so we make sure it is
       first.  Because this is a wildcard, it
       doesn't matter if the user does not
       actually link against crtbegin.o; the
       linker won't look for a file to match a
       wildcard.  The wildcard also means that it
       doesn't matter which directory crtbegin.o
       is in.  */
    KEEP (*crtbegin.o(.ctors))
    KEEP (*crtbegin?.o(.ctors))
    /* We don't want to include the .ctor section from
       the crtend.o file until after the sorted ctors.
       The .ctor section from the crtend file contains the
       end of ctors marker and it must be last */
    KEEP (*(EXCLUDE_FILE (*crtend.o *crtend?.o ) .ctors))
    KEEP (*(SORT(.ctors.*)))
    KEEP (*(.ctors))
    . = ALIGN(4) ;
  } >kseg0_program_mem
  .dtors   :
  {
    KEEP (*crtbegin.o(.dtors))
    KEEP (*crtbegin?.o(.dtors))
    KEEP (*(EXCLUDE_FILE (*crtend.o *crtend?.o ) .dtors))
    KEEP (*(SORT(.dtors.*)))
    KEEP (*(.dtors))
    . = ALIGN(4) ;
  } >kseg0_program_mem
  /* Read-only sections */
  .rodata   :
  {
    *( .gnu.linkonce.r.*)
    *(.rodata1)
    . = ALIGN(4) ;
  } >kseg0_program_mem
  /*
   * Small initialized constant global and static data can be placed in the
   * .sdata2 section.  This is different from .sdata, which contains small
   * initialized non-constant global and static data.
   */
  .sdata2 ALIGN(4) :
  {
    *(.sdata2 .sdata2.* .gnu.linkonce.s2.*)
    . = ALIGN(4) ;
  } >kseg0_program_mem
  /*
   * Uninitialized constant global and static data (i.e., variables which will
   * always be zero).  Again, this is different from .sbss, which contains
   * small non-initialized, non-constant global and static data.
   */
  .sbss2 ALIGN(4) :
  {
    *(.sbss2 .sbss2.* .gnu.linkonce.sb2.*)
    . = ALIGN(4) ;
  } >kseg0_program_mem
  .eh_frame_hdr   :
  {
    *(.eh_frame_hdr)
  } >kseg0_program_mem
    . = ALIGN(4) ;
  .eh_frame   : ONLY_IF_RO
  {
    KEEP (*(.eh_frame))
  } >kseg0_program_mem
    . = ALIGN(4) ;
  .gcc_except_table   : ONLY_IF_RO
  {
    *(.gcc_except_table .gcc_except_table.*)
  } >kseg0_program_mem
    . = ALIGN(4) ;
  .dbg_data (NOLOAD) :
  {
    . += (DEFINED (_DEBUGGER) ? 0x200 : 0x0);
    /* Additional data memory required for DSPr2 registers */
    . += (DEFINED (_DEBUGGER) ? 0x80 : 0x0);
  } >kseg0_data_mem
  .jcr   :
  {
    KEEP (*(.jcr))
    . = ALIGN(4) ;
  } >kseg0_data_mem
  .eh_frame    : ONLY_IF_RW
  {
    KEEP (*(.eh_frame))
  } >kseg0_data_mem
    . = ALIGN(4) ;
  .gcc_except_table    : ONLY_IF_RW
  {
    *(.gcc_except_table .gcc_except_table.*)
  } >kseg0_data_mem
    . = ALIGN(4) ;
  /* Persistent data - Use the new C 'persistent' attribute instead. */
  .persist   :
  {
    _persist_begin = .;
    *(.persist .persist.*)
    *(.pbss .pbss.*)
    . = ALIGN(4) ;
    _persist_end = .;
  } >kseg0_data_mem
  /*
   *  Note that input sections named .data* are not mapped here.
   *  The best-fit allocator locates them, so that they may flow
   *  around absolute sections as needed.
   */
  .data   :
  {
    *( .gnu.linkonce.d.*)
    SORT(CONSTRUCTORS)
    *(.data1)
    . = ALIGN(4) ;
  } >kseg0_data_mem
  . = .;
  _gp = ALIGN(16) + 0x7ff0;
  .got ALIGN(4) :
  {
    *(.got.plt) *(.got)
    . = ALIGN(4) ;
  } >kseg0_data_mem /* AT>kseg0_program_mem */
  /*
   * Note that 'small' data sections are still mapped in the linker
   * script. This ensures that they are grouped together for
   * gp-relative addressing. Absolute sections are allocated after
   * the 'small' data sections so small data cannot flow around them.
   */
  /*
   * We want the small data sections together, so single-instruction offsets
   * can access them all, and initialized data all before uninitialized, so
   * we can shorten the on-disk segment size.
   */
  .sdata ALIGN(4) :
  {
    _sdata_begin = . ;
    *(.sdata .sdata.* .gnu.linkonce.s.*)
    . = ALIGN(4) ;
    _sdata_end = . ;
  } >kseg0_data_mem
  .lit8           :
  {
    *(.lit8)
  } >kseg0_data_mem
  .lit4           :
  {
    *(.lit4)
  } >kseg0_data_mem
  . = ALIGN (4) ;
  _data_end = . ;
  _bss_begin = . ;
  .sbss ALIGN(4) :
  {
    _sbss_begin = . ;
    *(.dynsbss)
    *(.sbss .sbss.* .gnu.linkonce.sb.*)
    *(.scommon)
    _sbss_end = . ;
    . = ALIGN(4) ;
  } >kseg0_data_mem
  /*
   *  Align here to ensure that the .bss section occupies space up to
   *  _end.  Align after .bss to ensure correct alignment even if the
   *  .bss section disappears because there are no input sections.
   *
   *  Note that input sections named .bss* are no longer mapped here.
   *  The best-fit allocator locates them, so that they may flow
   *  around absolute sections as needed.
   *
   */
  .bss     :
  {
    *(.dynbss)
    *(COMMON)
   /* Align here to ensure that the .bss section occupies space up to
      _end.  Align after .bss to ensure correct alignment even if the
      .bss section disappears because there are no input sections. */
   . = ALIGN(. != 0 ? 4 : 1);
  } >kseg0_data_mem
  . = ALIGN(4) ;
  _end = . ;
  _bss_end = . ;
  /*
   *  The heap and stack are best-fit allocated by the linker after other
   *  data and bss sections have been allocated.
   */
  /*
   * RAM functions go at the end of our stack and heap allocation.
   * Alignment of 2K required by the boundary register (BMXDKPBA).
   *
   * RAM functions are now allocated by the linker. The linker generates
   * _ramfunc_begin and _bmxdkpba_address symbols depending on the
   * location of RAM functions.
   */
  _bmxdudba_address = LENGTH(kseg0_data_mem) ;
  _bmxdupba_address = LENGTH(kseg0_data_mem) ;
    /* The .pdr section belongs in the absolute section */
    /DISCARD/ : { *(.pdr) }
  .gptab.sdata : { *(.gptab.data) *(.gptab.sdata) }
  .gptab.sbss : { *(.gptab.bss) *(.gptab.sbss) }
  .mdebug.abi32 : { KEEP(*(.mdebug.abi32)) }
  .mdebug.abiN32 : { KEEP(*(.mdebug.abiN32)) }
  .mdebug.abi64 : { KEEP(*(.mdebug.abi64)) }
  .mdebug.abiO64 : { KEEP(*(.mdebug.abiO64)) }
  .mdebug.eabi32 : { KEEP(*(.mdebug.eabi32)) }
  .mdebug.eabi64 : { KEEP(*(.mdebug.eabi64)) }
  .gcc_compiled_long32 : { KEEP(*(.gcc_compiled_long32)) }
  .gcc_compiled_long64 : { KEEP(*(.gcc_compiled_long64)) }
  /* Stabs debugging sections.  */
  .stab          0 : { *(.stab) }
  .stabstr       0 : { *(.stabstr) }
  .stab.excl     0 : { *(.stab.excl) }
  .stab.exclstr  0 : { *(.stab.exclstr) }
  .stab.index    0 : { *(.stab.index) }
  .stab.indexstr 0 : { *(.stab.indexstr) }
  .comment       0 : { *(.comment) }
  /* DWARF debug sections used by MPLAB X for source-level debugging.
     Symbols in the DWARF debugging sections are relative to the beginning
     of the section so we begin them at 0.  */
  /* DWARF 1 */
  .debug          0 : { *(.debug) }
  .line           0 : { *(.line) }
  /* GNU DWARF 1 extensions */
  .debug_srcinfo  0 : { *(.debug_srcinfo) }
  .debug_sfnames  0 : { *(.debug_sfnames) }
  /* DWARF 1.1 and DWARF 2 */
  .debug_aranges  0 : { *(.debug_aranges) }
  .debug_pubnames 0 : { *(.debug_pubnames) }
  /* DWARF 2 */
  .debug_info     0 : { *(.debug_info .gnu.linkonce.wi.*) }
  .debug_abbrev   0 : { *(.debug_abbrev) }
  .debug_line     0 : { *(.debug_line) }
  .debug_frame    0 : { *(.debug_frame) }
  .debug_str      0 : { *(.debug_str) }
  .debug_loc      0 : { *(.debug_loc) }
  .debug_macinfo  0 : { *(.debug_macinfo) }
  /* SGI/MIPS DWARF 2 extensions */
  .debug_weaknames 0 : { *(.debug_weaknames) }
  .debug_funcnames 0 : { *(.debug_funcnames) }
  .debug_typenames 0 : { *(.debug_typenames) }
  .debug_varnames  0 : { *(.debug_varnames) }
  .debug_pubtypes 0 : { *(.debug_pubtypes) }
  .debug_ranges   0 : { *(.debug_ranges) }
  /DISCARD/ : { *(.rel.dyn) }
  .gnu.attributes 0 : { KEEP (*(.gnu.attributes)) }
  /DISCARD/ : { *(.note.GNU-stack) }
  /DISCARD/ : { *(.note.GNU-stack) *(.gnu_debuglink) *(.gnu.lto_*) *(.discard) }
  /DISCARD/ : { *(._debug_exception) }
}

/*************************************************************************
 *                    L1 Cache initialization symbols
 *************************************************************************/
/*
 *  Provide virtual addresses for cache initialization
 *  These symbols are used by the pic32_init_cache.o module to set up
 *  the cache at startup.
 */
EXTERN  (__pic32_init_cache_program_base_addr)
PROVIDE (__pic32_init_cache_program_base_addr = 0x9D000000) ;
EXTERN  (__pic32_init_cache_data_base_addr)
PROVIDE (__pic32_init_cache_data_base_addr = 0x80000000) ;

/*************************************************************************
 *   TLB-Based MMU Initialization section for EBI/SQI memory regions
 *************************************************************************/
/*
 *  Provide virtual and physical addresses of the SQI for TLB initialization
 *  These symbols are used below to create a table, which is in turn used to
 *  initialize the TLB at startup.
 */
EXTERN  (__pic32_init_sqi_kseg2_virtual_base_addr)
PROVIDE (__pic32_init_sqi_kseg2_virtual_base_addr = 0xD0000000) ;
EXTERN  (__pic32_init_sqi_kseg2_physical_base_addr)
PROVIDE (__pic32_init_sqi_kseg2_physical_base_addr = 0x30000000) ;
EXTERN  (__pic32_init_sqi_kseg2_entrylo0_bitset)
PROVIDE (__pic32_init_sqi_kseg2_entrylo0_bitset = 0x1F) ;

/*  ENTRYHI = VPN2<31:13> VPN2X<12:11> 0<10:8> ASID<7:0> */
__pic32_tlb_0_entryhi  = __pic32_init_sqi_kseg2_virtual_base_addr ;


/*  Create one 64MB page table entry consisting of two 32MB physical pages.
 *  ENTRYLO0 = RI<31> XI<30> 0<29:26> PFN<25:6> C<5:3> D<2> V<1> G<0>
 *  Shift __pic32_init_sqi_kseg3_physical_base_addr to PFN.
 *  Bitwise-OR the other bits of ENTRYLO0.
 */
__pic32_tlb_0_entrylo0 = (__pic32_init_sqi_kseg2_physical_base_addr >> 6) | __pic32_init_sqi_kseg2_entrylo0_bitset ;


/*  ENTRYLO1 = RI<31> XI<30> 0<29:26> PFN<25:6> C<5:3> D<2> V<1> G<0> */
__pic32_tlb_0_entrylo1 = __pic32_tlb_0_entrylo0 | 0x00080000 ;

EXTERN  (__pic32_init_sqi_kseg3_virtual_base_addr)
PROVIDE (__pic32_init_sqi_kseg3_virtual_base_addr = 0xF0000000) ;
EXTERN  (__pic32_init_sqi_kseg3_physical_base_addr)
PROVIDE (__pic32_init_sqi_kseg3_physical_base_addr = 0x30000000) ;
EXTERN  (__pic32_init_sqi_kseg3_entrylo0_bitset)
PROVIDE (__pic32_init_sqi_kseg3_entrylo0_bitset = 0x17) ;

/*  ENTRYHI = VPN2<31:13> VPN2X<12:11> 0<10:8> ASID<7:0> */
__pic32_tlb_1_entryhi  = __pic32_init_sqi_kseg3_virtual_base_addr ;


/*  Create one 64MB page table entry consisting of two 32MB physical pages.
 *  ENTRYLO0 = RI<31> XI<30> 0<29:26> PFN<25:6> C<5:3> D<2> V<1> G<0>
 *  Shift __pic32_init_sqi_kseg3_physical_base_addr to PFN.
 *  Bitwise-OR the other bits of ENTRYLO0.
 */
__pic32_tlb_1_entrylo0 = (__pic32_init_sqi_kseg3_physical_base_addr >> 6) | __pic32_init_sqi_kseg3_entrylo0_bitset ;


/*  ENTRYLO1 = RI<31> XI<30> 0<29:26> PFN<25:6> C<5:3> D<2> V<1> G<0> */
__pic32_tlb_1_entrylo1 = __pic32_tlb_1_entrylo0 | 0x00080000 ;
/*
 *  Provide virtual and physical addresses of the EBI for TLB initialization
 *  These symbols are used below to create a table, which is in turn used to
 *  initialize the TLB at startup.
 */
EXTERN  (__pic32_init_ebi_kseg2_virtual_base_addr)
PROVIDE (__pic32_init_ebi_kseg2_virtual_base_addr = 0xC0000000) ;
EXTERN  (__pic32_init_ebi_kseg2_physical_base_addr)
PROVIDE (__pic32_init_ebi_kseg2_physical_base_addr = 0x20000000) ;
EXTERN  (__pic32_init_ebi_kseg2_entrylo0_bitset)
PROVIDE (__pic32_init_ebi_kseg2_entrylo0_bitset = 0x1F) ;

/*  ENTRYHI = VPN2<31:13> VPN2X<12:11> 0<10:8> ASID<7:0> */
__pic32_tlb_2_entryhi  = __pic32_init_ebi_kseg2_virtual_base_addr ;


/*  Create one 64MB page table entry consisting of two 32MB physical pages.
 *  ENTRYLO0 = RI<31> XI<30> 0<29:26> PFN<25:6> C<5:3> D<2> V<1> G<0>
 *  Shift __pic32_init_sqi_kseg3_physical_base_addr to PFN.
 *  Bitwise-OR the other bits of ENTRYLO0.
 */
__pic32_tlb_2_entrylo0 = (__pic32_init_ebi_kseg2_physical_base_addr >> 6) | __pic32_init_ebi_kseg2_entrylo0_bitset ;


/*  ENTRYLO1 = RI<31> XI<30> 0<29:26> PFN<25:6> C<5:3> D<2> V<1> G<0> */
__pic32_tlb_2_entrylo1 = __pic32_tlb_2_entrylo0 | 0x00080000 ;

EXTERN  (__pic32_init_ebi_kseg3_virtual_base_addr)
PROVIDE (__pic32_init_ebi_kseg3_virtual_base_addr = 0xE0000000) ;
EXTERN  (__pic32_init_ebi_kseg3_physical_base_addr)
PROVIDE (__pic32_init_ebi_kseg3_physical_base_addr = 0x20000000) ;
EXTERN  (__pic32_init_ebi_kseg3_entrylo0_bitset)
PROVIDE (__pic32_init_ebi_kseg3_entrylo0_bitset = 0x17) ;

/*  ENTRYHI = VPN2<31:13> VPN2X<12:11> 0<10:8> ASID<7:0> */
__pic32_tlb_3_entryhi  = __pic32_init_ebi_kseg3_virtual_base_addr ;


/*  Create one 64MB page table entry consisting of two 32MB physical pages.
 *  ENTRYLO0 = RI<31> XI<30> 0<29:26> PFN<25:6> C<5:3> D<2> V<1> G<0>
 *  Shift __pic32_init_sqi_kseg3_physical_base_addr to PFN.
 *  Bitwise-OR the other bits of ENTRYLO0.
 */
__pic32_tlb_3_entrylo0 = (__pic32_init_ebi_kseg3_physical_base_addr >> 6) | __pic32_init_ebi_kseg3_entrylo0_bitset ;


/*  ENTRYLO1 = RI<31> XI<30> 0<29:26> PFN<25:6> C<5:3> D<2> V<1> G<0> */
__pic32_tlb_3_entrylo1 = __pic32_tlb_3_entrylo0 | 0x00080000 ;
/* 
 *  The default pic32_init_tlb_ebi_sqi.o module, which is
 *  called by the default C startup code (crt0_<isa>.o),
 *  uses the table created in the .tlb_init_values output
 *  section to initialize the Translation Lookaside buffer (TLB)
 *  of the Memory Mangagement Unit (MMU).
 */
SECTIONS
{
  .tlb_init_values :
  {
    . = ALIGN(4) ;
    __pic32_tlb_init_values_begin = ABSOLUTE(.);
    LONG(__pic32_tlb_0_entryhi) ;
    LONG(__pic32_tlb_0_entrylo0) ;
    LONG(__pic32_tlb_0_entrylo1) ;
    LONG(__pic32_tlb_1_entryhi) ;
    LONG(__pic32_tlb_1_entrylo0) ;
    LONG(__pic32_tlb_1_entrylo1) ;
    LONG(__pic32_tlb_2_entryhi) ;
    LONG(__pic32_tlb_2_entrylo0) ;
    LONG(__pic32_tlb_2_entrylo1) ;
    LONG(__pic32_tlb_3_entryhi) ;
    LONG(__pic32_tlb_3_entrylo0) ;
    LONG(__pic32_tlb_3_entrylo1) ;
    __pic32_tlb_init_values_end = ABSOLUTE(.);
    __pic32_tlb_init_values_count = 4 ;
  } > kseg1_boot_mem_4B0


} 



