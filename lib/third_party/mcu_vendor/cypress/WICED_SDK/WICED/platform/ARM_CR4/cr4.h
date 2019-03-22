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

/** @file
 *
 */
#pragma once

#include <stdint.h>
#include "platform_toolchain.h"

#ifdef __cplusplus
extern "C" {
#endif

/******************************************************
 *                      Macros
 ******************************************************/

#define INSTRUCTION_BARRIER()  __asm__ __volatile__ ("isb" : : : "memory")

/******************************************************
 *                    Constants
 ******************************************************/

/******************************************************
 *                   Enumerations
 ******************************************************/

/******************************************************
 *                 Type Definitions
 ******************************************************/

/******************************************************
 *                    Structures
 ******************************************************/

/******************************************************
 *                 Global Variables
 ******************************************************/

/******************************************************
 *               Function Declarations
 ******************************************************/

/**
 * Read the Instruction Fault Address Register
 */
static inline ALWAYS_INLINE uint32_t get_IFAR( void )
{
    uint32_t value;
    __asm__( "mrc p15, 0, %0, c6, c0, 2" : "=r" (value) : );
    return value;
}

/**
 * Read the Instruction Fault Status Register
 */
static inline ALWAYS_INLINE uint32_t get_IFSR( void )
{
    uint32_t value;
    __asm__( "mrc p15, 0, %0, c5, c0, 1" : "=r" (value) : );
    return value;
}

/**
 * Read the Data Fault Address Register
 */
static inline ALWAYS_INLINE uint32_t get_DFAR( void )
{
    uint32_t value;
    __asm__( "mrc p15, 0, %0, c6, c0, 0" : "=r" (value) : );
    return value;
}

/**
 * Read the Data Fault Status Register
 */
static inline ALWAYS_INLINE uint32_t get_DFSR( void )
{
    uint32_t value;
    __asm__( "mrc p15, 0, %0, c5, c0, 0" : "=r" (value) : );
    return value;
}

/**
 * Read the Program Counter and Processor Status Register
 */
static inline ALWAYS_INLINE uint32_t get_CPSR( void )
{
    uint32_t value;
    __asm__( "mrs %0, cpsr" : "=r" (value) : );
    return value;
}

/**
 * Read the Link Register
 */
static inline ALWAYS_INLINE uint32_t get_LR( void )
{
    uint32_t value;
    __asm__( "mov %0, lr" : "=r" (value) : );
    return value;
}

/**
 * Wait for interrupts
 */
static inline ALWAYS_INLINE void cpu_wait_for_interrupt( void )
{
    __asm__( "wfi" : : : "memory");
}

/**
 * Data Synchronisation Barrier
 */
static inline ALWAYS_INLINE void cpu_data_synchronisation_barrier( void )
{
    __asm__( "dsb" : : : "memory");
}

/*
 * Initialize performance monitoring registers
 */
static inline void cr4_init_cycle_counter( void )
{
    /* Reset cycle counter. Performance Monitors Control Register - PMCR */
    __asm__ __volatile__ ("MCR p15, 0, %0, c9, c12, 0" : : "r"(0x00000005));
    /* Enable the count - Performance Monitors Count Enable Set Register-  PMCNTENSET */
    __asm__ __volatile__ ("MCR p15, 0, %0, c9, c12, 1" : : "r"(0x80000000));
    /* Clear overflows - Performance Monitors Overflow Flag Status Register - PMOVSR */
    __asm__ __volatile__ ("MCR p15, 0, %0, c9, c12, 3" : : "r"(0x80000000));
}

/*
 * Read cycle counter
 */
static inline ALWAYS_INLINE uint32_t cr4_get_cycle_counter( void )
{
    uint32_t count;
    __asm__ __volatile__ ("MRC p15, 0, %0, c9, c13, 0" : "=r"(count));
    return count;
}

/*
 * Read overflow status register
 */
static inline ALWAYS_INLINE uint32_t cr4_get_overflow_flag_status( void )
{
    uint32_t flag;
    __asm__ __volatile__ ("MRC p15, 0, %0, c9, c12, 3" : "=r"(flag));
    return flag;
}

/*
 * Return whether cycle counter overflowed
 */
static inline ALWAYS_INLINE int cr4_is_cycle_counter_overflowed( void )
{
    return ( cr4_get_overflow_flag_status() & 0x80000000 ) ? 1 : 0;
}


/*
 * Should be defined by MCU
 */
void platform_backplane_debug( void );
void platform_exception_debug( void );

#ifdef __cplusplus
} /*extern "C" */
#endif
