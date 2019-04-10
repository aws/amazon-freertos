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
 * Defines macros describe cache
 */
#pragma once

#ifdef __cplusplus
extern "C" {
#endif

/******************************************************
 *                      Macros
 ******************************************************/

#ifdef PLATFORM_L1_CACHE_SHIFT

#define PLATFORM_L1_CACHE_BYTES             (1U << PLATFORM_L1_CACHE_SHIFT)
#define PLATFORM_L1_CACHE_LINE_MASK         (PLATFORM_L1_CACHE_BYTES - 1)
#define PLATFORM_L1_CACHE_ROUND_UP(a)       (((a) + PLATFORM_L1_CACHE_LINE_MASK) & ~(PLATFORM_L1_CACHE_LINE_MASK))
#define PLATFORM_L1_CACHE_ROUND_DOWN(a)     ((a) & ~(PLATFORM_L1_CACHE_LINE_MASK))
#define PLATFORM_L1_CACHE_PTR_ROUND_UP(p)   ((void*)PLATFORM_L1_CACHE_ROUND_UP((uint32_t)(p)))
#define PLATFORM_L1_CACHE_PTR_ROUND_DOWN(p) ((void*)PLATFORM_L1_CACHE_ROUND_DOWN((uint32_t)(p)))
#define PLATFORM_L1_CACHE_LINE_OFFSET(a)    ((uint32_t)(a) & (PLATFORM_L1_CACHE_LINE_MASK) )
#define PLATFORM_L1_CACLE_LINE_ALIGNED(a)

#if defined ( __GNUC__ )
#define DEFINE_CACHE_LINE_ALIGNED_ARRAY(type, name, size) \
type name[PLATFORM_L1_CACHE_ROUND_UP(size)] ALIGNED(PLATFORM_L1_CACHE_BYTES);
#elif defined ( __IAR_SYSTEMS_ICC__ )
#define DEFINE_CACHE_LINE_ALIGNED_ARRAY(type, name, size) \
_Pragma( "data_alignment=PLATFORM_L1_CACHE_BYTES" ) \
type name[PLATFORM_L1_CACHE_ROUND_UP(size)];
#endif /* defined ( __GNUC__ ) */

#else

#define PLATFORM_L1_CACHE_BYTES             (0)
#define PLATFORM_L1_CACHE_LINE_MASK         (0)
#define PLATFORM_L1_CACHE_ROUND_UP(a)       (a)
#define PLATFORM_L1_CACHE_ROUND_DOWN(a)     (a)
#define PLATFORM_L1_CACHE_PTR_ROUND_UP(a)   (a)
#define PLATFORM_L1_CACHE_PTR_ROUND_DOWN(a) (a)
#define PLATFORM_L1_CACHE_LINE_OFFSET(a)    (0)

#define DEFINE_CACHE_LINE_ALIGNED_ARRAY(type, name, size) \
type name[size];

#endif /* PLATFORM_L1_CACHE_SHIFT */

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

#ifdef __cplusplus
} /*extern "C" */
#endif
