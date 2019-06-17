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
 * Declare functions to manipulate caches
 */
#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include "platform_cache_def.h"

/******************************************************
 *                      Macros
 ******************************************************/

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

#ifndef __ASSEMBLER__

#ifndef UNUSED_PARAMETER
#define UNUSED_PARAMETER(x) (void) x
#endif /* UNUSED_PARAMETER */

void platform_icache_inv_all( void );

#ifdef PLATFORM_L1_CACHE_SHIFT

void platform_dcache_enable( void );
void platform_dcache_disable( void );
void platform_dcache_inv_all( void );
void platform_dcache_clean_all( void );
void platform_dcache_clean_and_inv_all( void );
void platform_dcache_inv_range( volatile void *p, uint32_t length );
void platform_dcache_clean_range( const volatile void *p, uint32_t length );
void platform_dcache_clean_and_inv_range( const volatile void *p, uint32_t length );
void platform_icache_inv_range( volatile void *p, uint32_t length );

#else

#define platform_dcache_enable()                  do {                                         } while(0)
#define platform_dcache_disable()                 do {                                         } while(0)
#define platform_dcache_inv_all()                 do {                                         } while(0)
#define platform_dcache_clean_all()               do {                                         } while(0)
#define platform_dcache_clean_and_inv_all()       do {                                         } while(0)
#define platform_dcache_inv_range(p, l)           do {UNUSED_PARAMETER(p); UNUSED_PARAMETER(l);} while(0)
#define platform_dcache_clean_range(p, l)         do {UNUSED_PARAMETER(p); UNUSED_PARAMETER(l);} while(0)
#define platform_dcache_clean_and_inv_range(p, l) do {UNUSED_PARAMETER(p); UNUSED_PARAMETER(l);} while(0)
#define platform_icache_inv_range(p, l)           do {UNUSED_PARAMETER(p); UNUSED_PARAMETER(l);} while(0)
#endif /* PLATFORM_L1_CACHE_SHIFT */

#endif /* __ASSEMBLER__ */

#ifdef __cplusplus
} /*extern "C" */
#endif
