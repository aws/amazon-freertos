/*
 * $ Copyright Cypress Semiconductor $
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
