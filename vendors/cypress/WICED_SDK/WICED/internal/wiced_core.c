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
#include "wiced_result.h"
#include "wwd_constants.h"
#include "platform_config.h"
#include "wwd_debug.h"

extern wiced_result_t wiced_core_init( void );
extern wiced_result_t wiced_core_deinit( void );
extern wiced_result_t wiced_platform_init( void );
extern wiced_result_t wiced_rtos_init  ( void );
extern wiced_result_t wiced_dct_init  ( void );
wiced_result_t wiced_rtos_deinit( void );
extern void wiced_hwcrypto_init( void );

#ifdef USES_RESOURCE_FILESYSTEM
extern platform_result_t platform_filesystem_init( void );
#endif

static wiced_bool_t wiced_core_initialised = WICED_FALSE;

wiced_result_t wiced_core_init( void )
{
    if ( wiced_core_initialised == WICED_TRUE )
    {
        return WICED_SUCCESS;
    }

    wiced_platform_init( );

    wiced_rtos_init( );

    wiced_dct_init( );

#ifdef USES_RESOURCE_FILESYSTEM
    platform_filesystem_init();
#endif

#if defined(PLATFORM_HAS_HW_CRYPTO_SUPPORT)
    wiced_hwcrypto_init( );
#endif

    wiced_core_initialised = WICED_TRUE;

    WPRINT_APP_INFO( ("WICED_core Initialized\r\n") );
    return WICED_SUCCESS;
}

wiced_result_t wiced_core_deinit( void )
{
    if ( wiced_core_initialised != WICED_TRUE )
    {
        return WICED_SUCCESS;
    }

    wiced_rtos_deinit();

    wiced_core_initialised = WICED_FALSE;

    return WICED_SUCCESS;
}
