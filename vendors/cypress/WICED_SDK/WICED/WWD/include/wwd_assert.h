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
 *  Defines macro for assertions
 *
 */
#pragma once

#include "wwd_debug.h"
#include "platform_assert.h"

#ifdef __cplusplus
extern "C"
{
#endif

/******************************************************
 * @cond       Macros
 ******************************************************/

#ifdef DEBUG
    #ifdef WPRINT_ENABLE_ERROR
        #define WPRINT_ERROR(args)                      do { WPRINT_MACRO(args); WICED_ASSERTION_FAIL_ACTION(); } while(0)
        #define wiced_assert( error_string, assertion ) do { if (!(assertion)) { WICED_ASSERTION_FAIL_ACTION(); } } while(0)
        #define wiced_minor_assert( error_string, assertion )   do { if ( !(assertion) ) WPRINT_MACRO( ( error_string ) ); } while(0)
    #else
        #define WPRINT_ERROR(args)                      do { WICED_ASSERTION_FAIL_ACTION();} while(0)
        #define wiced_assert( error_string, assertion ) do { if (!(assertion)) { WICED_ASSERTION_FAIL_ACTION();} } while(0)
        #define wiced_minor_assert( error_string, assertion )   do { (void)(assertion); } while(0)
    #endif
#else
    #define wiced_assert( error_string, assertion )         do { (void)(assertion); } while(0)
    #define wiced_minor_assert( error_string, assertion )   do { (void)(assertion); } while(0)
#endif

#ifdef __GNUC__
#define WICED_UNUSED_VAR __attribute__ ((unused))
#else
#define WICED_UNUSED_VAR
#endif

#define wiced_static_assert( descr, expr ) \
{ \
    /* Make sure the expression is constant. */ \
    typedef enum { _STATIC_ASSERT_NOT_CONSTANT = (expr) } _static_assert_e WICED_UNUSED_VAR; \
    /* Make sure the expression is true. */ \
    typedef char STATIC_ASSERT_FAIL_##descr[(expr) ? 1 : -1] WICED_UNUSED_VAR; \
}

/** @endcond */

#ifdef __cplusplus
} /* extern "C" */
#endif
