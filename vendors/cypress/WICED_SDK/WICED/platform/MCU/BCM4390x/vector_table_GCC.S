@
@ Copyright 2019, Cypress Semiconductor Corporation or a subsidiary of
 @ Cypress Semiconductor Corporation. All Rights Reserved.
 @ 
 @ This software, associated documentation and materials ("Software")
 @ is owned by Cypress Semiconductor Corporation,
 @ or one of its subsidiaries ("Cypress") and is protected by and subject to
 @ worldwide patent protection (United States and foreign),
 @ United States copyright laws and international treaty provisions.
 @ Therefore, you may use this Software only as provided in the license
 @ agreement accompanying the software package from which you
 @ obtained this Software ("EULA").
 @ If no EULA applies, Cypress hereby grants you a personal, non-exclusive,
 @ non-transferable license to copy, modify, and compile the Software
 @ source code solely for use in connection with Cypress's
 @ integrated circuit products. Any reproduction, modification, translation,
 @ compilation, or representation of this Software except as specified
 @ above is prohibited without the express written permission of Cypress.
 @
 @ Disclaimer: THIS SOFTWARE IS PROVIDED AS-IS, WITH NO WARRANTY OF ANY KIND,
 @ EXPRESS OR IMPLIED, INCLUDING, BUT NOT LIMITED TO, NONINFRINGEMENT, IMPLIED
 @ WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE. Cypress
 @ reserves the right to make changes to the Software without notice. Cypress
 @ does not assume any liability arising out of the application or use of the
 @ Software or any product or circuit described in the Software. Cypress does
 @ not authorize its products for use in any products where a malfunction or
 @ failure of the Cypress product may reasonably be expected to result in
 @ significant property damage, injury or death ("High Risk Product"). By
 @ including Cypress's product in a High Risk Product, the manufacturer
 @ of such system or application assumes all risk of such use and in doing
 @ so agrees to indemnify Cypress against all liability.
@

#define VENEER_NAME_THUMB2(name) veneer_thumb2_##name
#define VENEER_NAME_ARM(name)    veneer_arm_##name

#if defined(__thumb__) || defined(__NUTTX__)
#define VECTOR_VENEER_NEEDED
#endif

#ifdef VECTOR_VENEER_NEEDED
#define VECTOR_NAME(name)        VENEER_NAME_THUMB2(name)
#else
#define VECTOR_NAME(name)        name
#endif

#ifndef RESET_ENTRY_POINT
#define RESET_ENTRY_POINT        _low_start
#endif

    /* Vectors */

    .text
    .section .vectors_copyfrom, "ax"
    .align 2
    .global irq_instruction_vector_low_reset
    .global irq_vector_undefined_instruction
    .global irq_vector_software_interrupt
    .global irq_vector_prefetch_abort
    .global irq_vector_reserved
    .global irq_vector_external_interrupt
    .global irq_vector_fast_interrupt

    /* Total 8 entries */

#if defined(RESET_ENTRY_POINT_THUMB2)
    /* CPU starts in Thumb2 mode. This conditional compilation can be used when no ROM bootloader involved and we need to run application directly. */
    .thumb
    .syntax unified
    B.W        VENEER_NAME_ARM(RESET_ENTRY_POINT)
#elif defined(RESET_ENTRY_POINT_ARM)
    .arm
    B          RESET_ENTRY_POINT
#else
     /* Original reset vector which jumps into ROM bootloader. */
    .word      irq_instruction_vector_low_reset
#endif

    .arm

    B          VECTOR_NAME(irq_vector_undefined_instruction)    /* 0x04 - undefined instruction */
    B          VECTOR_NAME(irq_vector_software_interrupt)       /* 0x08 - software interrupt    */
    B          VECTOR_NAME(irq_vector_prefetch_abort)           /* 0x0c - prefetch abort        */
    B          VECTOR_NAME(irq_vector_data_abort)               /* 0x10 - data abort            */
    B          VECTOR_NAME(irq_vector_reserved)                 /* 0x14 - reserved              */
    B          VECTOR_NAME(irq_vector_external_interrupt)       /* 0x18 - external interrupt    */
    B          VECTOR_NAME(irq_vector_fast_interrupt)           /* 0x1c - fast interrupt        */

    /* Vector veneers when need to change instruction set */

#ifdef VECTOR_VENEER_NEEDED
    .text
    .section .text.vectors_veneer, "ax"
    .align 2
    .arm
VECTOR_NAME(irq_vector_undefined_instruction):
    LDR        pc, =irq_vector_undefined_instruction
VECTOR_NAME(irq_vector_software_interrupt):
    LDR        pc, =irq_vector_software_interrupt
VECTOR_NAME(irq_vector_prefetch_abort):
    LDR        pc, =irq_vector_prefetch_abort
VECTOR_NAME(irq_vector_data_abort):
    LDR        pc, =irq_vector_data_abort
VECTOR_NAME(irq_vector_reserved):
    LDR        pc, =irq_vector_reserved
VECTOR_NAME(irq_vector_external_interrupt):
    LDR        pc, =irq_vector_external_interrupt
VECTOR_NAME(irq_vector_fast_interrupt):
    LDR        pc, =irq_vector_fast_interrupt
#endif /* VECTOR_VENEER_NEEDED */

#ifdef RESET_ENTRY_POINT_THUMB2
    .thumb
    .syntax unified
VENEER_NAME_ARM(RESET_ENTRY_POINT):
    LDR pc, =RESET_ENTRY_POINT
#endif
