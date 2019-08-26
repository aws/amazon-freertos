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
 * Defines functions to manipulate caches
 */

#ifdef __cplusplus
extern "C" {
#endif

#include "stdint.h"
#include "platform_cache.h"

/******************************************************
 *                      Macros
 ******************************************************/

/******************************************************
 *                    Constants
 ******************************************************/

/******************************************************
 *                   Enumerations
 ******************************************************/

typedef enum
{
    PLATFORM_DCACHE_CLEAN_OPERATION,
    PLATFORM_DCACHE_INV_OPERATION,
    PLATFORM_DCACHE_CLEAN_AND_INV_OPERATION,
} platform_dcache_range_operation_t;

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

/******************************************************
 *               Function Definitions
 ******************************************************/

#ifdef PLATFORM_L1_CACHE_SHIFT


void platform_icache_inv_range( volatile void *p, uint32_t length )
{
    uint32_t addr_start = PLATFORM_L1_CACHE_ROUND_DOWN((uint32_t)p);
    const uint32_t addr_end = PLATFORM_L1_CACHE_ROUND_UP((uint32_t)p + length);

    __asm__ __volatile__ ("DSB");

    while (addr_start < addr_end)
    {
        __asm__ __volatile__ ("MCR p15, 0, %0, c7, c5, 1" : : "r"(addr_start));

        addr_start += PLATFORM_L1_CACHE_BYTES;
    }

    __asm__ __volatile__ ("DSB");
    __asm__ __volatile__ ("ISB");
}


static void platform_dcache_range_operation( platform_dcache_range_operation_t operation, const volatile void *p, uint32_t length )
{
    uint32_t addr_start = PLATFORM_L1_CACHE_ROUND_DOWN((uint32_t)p);
    const uint32_t addr_end = PLATFORM_L1_CACHE_ROUND_UP((uint32_t)p + length);

    __asm__ __volatile__ ("DSB");

    while (addr_start < addr_end)
    {
        switch ( operation )
        {
            case PLATFORM_DCACHE_CLEAN_OPERATION:
                __asm__ __volatile__ ("MCR p15, 0, %0, c7, c10, 1" : : "r"(addr_start));
                break;

            case PLATFORM_DCACHE_INV_OPERATION:
                __asm__ __volatile__ ("MCR p15, 0, %0, c7, c6, 1" : : "r"(addr_start));
                break;

            case PLATFORM_DCACHE_CLEAN_AND_INV_OPERATION:
                __asm__ __volatile__ ("MCR p15, 0, %0, c7, c14, 1" : : "r"(addr_start));
                break;

            default:
                break;
        }

        addr_start += PLATFORM_L1_CACHE_BYTES;
    }

    __asm__ __volatile__ ("DSB");
}

void platform_dcache_inv_range( volatile void *p, uint32_t length )
{
    /* Invalidate d-cache lines from range */
    platform_dcache_range_operation( PLATFORM_DCACHE_INV_OPERATION, p, length);
}

void platform_dcache_clean_range( const volatile void *p, uint32_t length )
{
    /* Clean (save dirty lines) d-cache lines from range */

#ifndef WICED_DCACHE_WTHROUGH

    platform_dcache_range_operation( PLATFORM_DCACHE_CLEAN_OPERATION, p, length);

#else

    UNUSED_PARAMETER(p);
    UNUSED_PARAMETER(length);

    __asm__ __volatile__ ("DSB");

#endif /* WICED_DCACHE_WTHROUGH */
}

void platform_dcache_clean_and_inv_range( const volatile void *p, uint32_t length)
{
    /* Clean and invalidate (save dirty lines and then invalidate) d-cache lines from range */
    platform_dcache_range_operation( PLATFORM_DCACHE_CLEAN_AND_INV_OPERATION, p, length);
}

#endif /* PLATFORM_L1_CACHE_SHIFT */
