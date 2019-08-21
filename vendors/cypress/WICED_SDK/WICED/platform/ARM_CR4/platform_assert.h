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
 * Defines macros for defining asserts for ARM-Cortex-R4 CPU
 */
#pragma once

#ifdef __cplusplus
extern "C" {
#endif

/******************************************************
 *                      Macros
 ******************************************************/

/******************************************************
 *                    Constants
 ******************************************************/

#if defined ( __GNUC__ )

#if defined ( __clang__ )

static inline /*@noreturn@*/void WICED_TRIGGER_BREAKPOINT( void ) __attribute__((analyzer_noreturn))
{
    __asm__("bkpt");
}

#else

#if defined ( LINT ) /* Lint requires a prototype */

extern /*@noreturn@*/ void WICED_TRIGGER_BREAKPOINT( void );

#else /* #if defined ( LINT ) */

#define WICED_TRIGGER_BREAKPOINT( ) do { __asm__("bkpt"); } while (0)

#endif /* #if defined ( LINT ) */

#endif /* #if defined ( __clang__ ) */

#define WICED_ASSERTION_FAIL_ACTION() WICED_TRIGGER_BREAKPOINT()

#ifdef WICED_NO_VECTORS
#define WICED_DISABLE_INTERRUPTS() do { __asm__("" : : : "memory"); } while (0)
#define WICED_ENABLE_INTERRUPTS() do { __asm__("" : : : "memory"); } while (0)
#else
#define WICED_DISABLE_INTERRUPTS() do { __asm__("CPSID i" : : : "memory"); } while (0)
#define WICED_ENABLE_INTERRUPTS() do { __asm__("CPSIE i" : : : "memory"); } while (0)
#endif

#define CSPR_INTERRUPTS_DISABLED (0x80)

#elif defined ( __IAR_SYSTEMS_ICC__ )

#include <cmsis_iar.h>

#define WICED_TRIGGER_BREAKPOINT() do { __asm("bkpt 0"); } while (0)

#define WICED_ASSERTION_FAIL_ACTION() WICED_TRIGGER_BREAKPOINT()

#define WICED_DISABLE_INTERRUPTS() do { __asm("CPSID i"); } while (0)
#define WICED_ENABLE_INTERRUPTS() do { __asm("CPSIE i"); } while (0)

#endif

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

#ifdef __cplusplus
} /*extern "C" */
#endif
