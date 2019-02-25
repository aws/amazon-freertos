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
#include <stdint.h>
#include <string.h>
#include "platform_peripheral.h"
#include "platform_config.h"
#include "wwd_constants.h"
#include "wwd_platform_common.h"
#include "wwd_assert.h"
#include "platform/wwd_platform_interface.h"
#include "wiced_framework.h"

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
 *               Static Function Declarations
 ******************************************************/

/******************************************************
 *               Variable Definitions
 ******************************************************/

/******************************************************
 *               Function Definitions
 ******************************************************/

wwd_result_t host_platform_init( void )
{
    host_platform_deinit_wlan_powersave_clock( );
#ifdef USES_RESOURCE_FILESYSTEM
    platform_filesystem_init();
#endif
#if defined ( WICED_USE_WIFI_RESET_PIN )
    platform_gpio_init( &wifi_control_pins[WWD_PIN_RESET], OUTPUT_PUSH_PULL );
    host_platform_reset_wifi( WICED_TRUE );  /* Start wifi chip in reset */
#endif

#if defined ( WICED_USE_WIFI_POWER_PIN )
    platform_gpio_init( &wifi_control_pins[WWD_PIN_POWER], OUTPUT_PUSH_PULL );
    host_platform_power_wifi( WICED_FALSE ); /* Start wifi chip with regulators off */
#endif
    return WWD_SUCCESS;
}

wwd_result_t host_platform_deinit( void )
{
#if defined ( WICED_USE_WIFI_RESET_PIN )
    platform_gpio_init( &wifi_control_pins[WWD_PIN_RESET], OUTPUT_PUSH_PULL );
    host_platform_reset_wifi( WICED_TRUE );  /* Start wifi chip in reset */
#endif

#if defined ( WICED_USE_WIFI_POWER_PIN )
    platform_gpio_init( &wifi_control_pins[WWD_PIN_POWER], OUTPUT_PUSH_PULL );
    host_platform_power_wifi( WICED_FALSE ); /* Start wifi chip with regulators off */
#endif

    host_platform_deinit_wlan_powersave_clock( );

    return WWD_SUCCESS;
}

void host_platform_reset_wifi( wiced_bool_t reset_asserted )
{
#if defined ( WICED_USE_WIFI_RESET_PIN )
    ( reset_asserted == WICED_TRUE ) ? platform_gpio_output_low( &wifi_control_pins[ WWD_PIN_RESET ] ) : platform_gpio_output_high( &wifi_control_pins[ WWD_PIN_RESET ] );
#else
    UNUSED_PARAMETER( reset_asserted );
#endif
}

void host_platform_power_wifi( wiced_bool_t power_enabled )
{
#if   defined ( WICED_USE_WIFI_POWER_PIN ) && defined ( WICED_USE_WIFI_POWER_PIN_ACTIVE_HIGH )
    ( power_enabled == WICED_TRUE ) ? platform_gpio_output_high( &wifi_control_pins[WWD_PIN_POWER] ) : platform_gpio_output_low ( &wifi_control_pins[WWD_PIN_POWER] );
#elif defined ( WICED_USE_WIFI_POWER_PIN )
    ( power_enabled == WICED_TRUE ) ? platform_gpio_output_low ( &wifi_control_pins[WWD_PIN_POWER] ) : platform_gpio_output_high( &wifi_control_pins[WWD_PIN_POWER] );
#else
    UNUSED_PARAMETER( power_enabled );
#endif
}

wwd_result_t host_platform_get_mac_address( wiced_mac_t* mac )
{
#if !defined ( WICED_DISABLE_BOOTLOADER )
    wiced_mac_t* temp_mac;
    wiced_result_t result;
    result = wiced_dct_read_lock( (void**) &temp_mac, WICED_FALSE, DCT_WIFI_CONFIG_SECTION, OFFSETOF( platform_dct_wifi_config_t, mac_address ), sizeof(wiced_mac_t) );
    if ( result != WICED_SUCCESS )
    {
        return (wwd_result_t) result;
    }
    memcpy( mac->octet, temp_mac, sizeof(wiced_mac_t) );
    wiced_dct_read_unlock( temp_mac, WICED_FALSE );
#else
    UNUSED_PARAMETER( mac );
#endif
    return WWD_SUCCESS;
}

wwd_result_t host_platform_deinit_wlan_powersave_clock( void )
{
#if defined WICED_USE_WIFI_32K_CLOCK_MCO
    platform_gpio_init( &wifi_control_pins[WWD_PIN_32K_CLK], OUTPUT_PUSH_PULL );
    platform_gpio_output_low( &wifi_control_pins[WWD_PIN_32K_CLK] );
#endif
    return WWD_SUCCESS;
}
