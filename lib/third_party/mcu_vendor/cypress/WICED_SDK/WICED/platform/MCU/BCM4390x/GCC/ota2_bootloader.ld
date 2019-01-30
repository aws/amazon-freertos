/*
 * Copyright 2019, Cypress Semiconductor Corporation or a subsidiary of
 * Cypress Semiconductor Corporation. All Rights Reserved.
 * 
 * This software, associated documentation and materials ("Software")
 * is owned by Cypress Semiconductor Corporation,
 * or one of its subsidiaries ("Cypress") and is protected by and subject to
 * worldwide patent protection (United States and foreign),
 * United States copyright laws and international treaty provisions.
 * Therefore, you may use this Software only as provided in the license
 * agreement accompanying the software package from which you
 * obtained this Software ("EULA").
 * If no EULA applies, Cypress hereby grants you a personal, non-exclusive,
 * non-transferable license to copy, modify, and compile the Software
 * source code solely for use in connection with Cypress's
 * integrated circuit products. Any reproduction, modification, translation,
 * compilation, or representation of this Software except as specified
 * above is prohibited without the express written permission of Cypress.
 *
 * Disclaimer: THIS SOFTWARE IS PROVIDED AS-IS, WITH NO WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING, BUT NOT LIMITED TO, NONINFRINGEMENT, IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE. Cypress
 * reserves the right to make changes to the Software without notice. Cypress
 * does not assume any liability arising out of the application or use of the
 * Software or any product or circuit described in the Software. Cypress does
 * not authorize its products for use in any products where a malfunction or
 * failure of the Cypress product may reasonably be expected to result in
 * significant property damage, injury or death ("High Risk Product"). By
 * including Cypress's product in a High Risk Product, the manufacturer
 * of such system or application assumes all risk of such use and in doing
 * so agrees to indemnify Cypress against all liability.
 */

/* Mapping of platform interrupts over the top of generic versions */

reset_handler = _low_start;

/* Force system to reset via ROM bootloader & serial flash.  This works around A0 issue that FLOPS is not reset by SRSTn */
irq_instruction_vector_low_reset = 0xB03EF000;


ENTRY( _low_start );

INCLUDE GCC_ota2_bootloader_memory.ld

SECTIONS
{
    .text :
    {
        . = ALIGN(4);

        link_code_location = .;
        KEEP(*(.text.startup)) /* has to be first */
        *(.text .text.* .gnu.linkonce.t.*)
        KEEP(*(.text.vPortSVCHandler    .text.xPortPendSVHandler .text.xPortSysTickHandler ))
        KEEP(*(.text.__tx_SVCallHandler .text.__tx_PendSVHandler .text.__tx_SysTickHandler ))
        KEEP(*(.text.NoOS_systick_irq ))
        KEEP(*(.text.sdio_rtos_irq .text.sdio_irq ))
        KEEP(*(.text.uart_rtos_irq .text.uart_irq ))
        KEEP(*(.text.dma_rtos_irq  .text.dma_irq  ))
        KEEP(*(.text.gpio_rtos_irq .text.gpio_irq ))
        KEEP(*(.text.dbg_watchdog_rtos_irq  .text.dbg_watchdog_irq  ))
        KEEP(*(.text.usart1_rtos_irq .text.usart1_irq ))
        KEEP(*(.text.usart2_rtos_irq .text.usart2_irq ))
        KEEP(*(.text.usart1_tx_dma_rtos_irq .text.usart1_tx_dma_irq ))
        KEEP(*(.text.usart2_tx_dma_rtos_irq .text.usart2_tx_dma_irq ))
        KEEP(*(.text.usart1_rx_dma_rtos_irq .text.usart1_rx_dma_irq ))
        KEEP(*(.text.usart2_rx_dma_rtos_irq .text.usart2_rx_dma_irq ))
        link_code_end = .;
    } > BOOTLOADER_SRAM

    .rodata :
    {
        . = ALIGN(4);

        link_const_variable_data_location = .;
        *(.rodata .rodata.* .gnu.linkonce.r.*)
        link_const_variable_data_end = .;

        . = ALIGN(4);

        link_constructors_location = .;
        KEEP(*(.preinit_array))
        KEEP(*(.init_array))
        KEEP (*crtbegin.o(.ctors))
        KEEP (*(EXCLUDE_FILE (*crtend.o) .ctors))
        KEEP (*(SORT(.ctors.*)))
        KEEP (*crtend.o(.ctors))
        link_constructors_end = .;

        . = ALIGN(4);

        link_destructors_location = .;
        KEEP (*crtbegin.o(.dtors))
        KEEP (*(EXCLUDE_FILE (*crtend.o) .dtors))
        KEEP (*(SORT(.dtors.*)))
        KEEP (*crtend.o(.dtors))
        link_destructors_end = .;
    } > BOOTLOADER_SRAM

    .vectors_copyfrom :
    {
        link_interrupt_vectors_copyfrom = LOADADDR(.vectors_copyfrom);
        KEEP(*(.vectors_copyfrom))
    } > FLOPS AT > BOOTLOADER_SRAM

    /*
     * The .ARM.exidx and .ARM.extab sections are used for C++ exception handling.
     * It is located here for completeness. Bare-metal ARM projects
     * typically cannot afford the overhead associated with C++
     * exceptions handling.
     */
    .ARM.exidx :
    {
        __exidx_start = ALIGN(4);
        *(.ARM.exidx* .gnu.linkonce.armexidx.*)
        __exidx_end = .;
    } > BOOTLOADER_SRAM

    .ARM.extab :
    {
        __extab_start = ALIGN(4);
        *(.ARM.extab*)
        __extab_end = .;
    } > BOOTLOADER_SRAM

    .data : /* Contains the non-zero initialised global variables */
    {
        . = ALIGN(4);

        *(.data*)
    } > BOOTLOADER_SRAM

    .dma (NOLOAD) : /* Zero initialised memory used DMA descriptors */
    {
        . = ALIGN(32);

        link_dma_location = .;
        *(.dma.*)
        link_dma_end = ALIGN(4);
    } > BOOTLOADER_SRAM

    .bss (NOLOAD) : /* Zero initialised memory used for zero initialised variables */
    {
        . = ALIGN(4);

        link_bss_location = .;
        *(.bss*)
        *(COMMON)
        link_bss_end = ALIGN(4);
    } > BOOTLOADER_SRAM

    .stack_start (NOLOAD) :
    {
        . = ALIGN(8);

        link_stack_location = .;
        *(.stack)
        . = MAX(link_stack_location + START_STACK_SIZE , .);
        link_stack_end = ALIGN(8);
    } > BOOTLOADER_SRAM

    .stack_isr (NOLOAD) :
    {
        . = ALIGN(8);

        link_stack_isr_location = .;
        . = . + link_stack_isr_location + SYS_STACK_SIZE + IRQ_STACK_SIZE + FIQ_STACK_SIZE + 3 * 8;
        link_stack_isr_end = ALIGN(8);
        /*
         * Stack is full descending.
         * Here we give choice to RTOS to pre-allocate frame,
         * so on entry to ISR sp register points to empty frame.
         */
        KEEP(*(.isr_stack_initial_frame))
        link_heap_start = ALIGN(8);
    } > BOOTLOADER_SRAM

    INCLUDE GCC/ddr.ld

    /DISCARD/ :
    {
        *(.ARM.attributes*)
        *(.comment)
        *(.init)
        *(.preinit)
        *(.fini)
        *(.fini_array)
        *(.ARM.exidx*)
        *(.gnu.linkonce.armexidx.*)
        *(.eh_frame_hdr)
        *(.eh_frame)
        *(.gnu.linkonce.armextab.*)
        *(.v4_bx)
        *(.vfp11_veneer)
        *(.gcc_except_table)
        *(.eh_frame_hdr)
        *(.eh_frame)
        *(.glue*)
    }
}

/* Declare libc Heap to start at end of allocated RAM and end at top of RAM, aligned 8 byte */
PROVIDE( _heap = link_heap_start );
PROVIDE( _eheap = ALIGN( ORIGIN( BOOTLOADER_SRAM ) + LENGTH( BOOTLOADER_SRAM ) - 8, 8 ) );

/* Declare ThreadX free memory (passed to app as tx_application_define() func's argument) */
PROVIDE( __RAM_segment_used_end__ = link_stack_isr_end );

