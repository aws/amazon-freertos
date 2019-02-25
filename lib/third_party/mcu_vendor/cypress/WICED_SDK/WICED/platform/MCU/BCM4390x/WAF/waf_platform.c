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
 * Defines BCM439x WICED application framework functions
 */
#include "platform.h"
#include "waf_platform.h"
#include "wiced_framework.h"
#include "wiced_apps_common.h"
#include "wiced_low_power.h"
#include "wicedfs.h"
#include "platform_map.h"

/******************************************************
 *                      Macros
 ******************************************************/

/* Application linker script ensures that tiny bootloader binary is starting from the beginning of Always-On memory.
 * Tiny bootloader linker script ensures that its configuration structure is starting from the beginning of Always-On memory.
 */
#define WICED_DEEP_SLEEP_TINY_BOOTLOADER_CONFIG    ( (volatile wiced_deep_sleep_tiny_bootloader_config_t *)PLATFORM_SOCSRAM_CH0_AON_RAM_BASE(0x0) )

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
 *              Function Definitions
 ******************************************************/

/******************************************************
 *              Global Variables
 ******************************************************/

/******************************************************
 *              Function Declarations
  ******************************************************/

/******************************************************
 *                 DCT Functions
 ******************************************************/

static wiced_bool_t platform_is_load_permitted( void* physical_address, uint32_t size, wiced_bool_t* is_aon_segment )
{
    const uint32_t     destination = (uint32_t)physical_address;
    const wiced_bool_t aon_segment = WICED_DEEP_SLEEP_IS_AON_SEGMENT( destination, size ) ? WICED_TRUE : WICED_FALSE;
    const wiced_bool_t xip_segment = WICED_IS_XIP_SEGMENT( destination, size );
    wiced_bool_t       result      = WICED_FALSE;

    if ( ( !aon_segment || !platform_mcu_powersave_is_warmboot() ) && !xip_segment )
    {
        result          = WICED_TRUE;
        *is_aon_segment = aon_segment;
    }

    return result;
}

void platform_load_app_chunk_from_fs( const image_location_t* app_header_location, void* file_handler, void* physical_address, uint32_t size )
{
    wiced_bool_t aon_segment;

    UNUSED_PARAMETER( app_header_location );

    if ( platform_is_load_permitted( physical_address, size, &aon_segment ) )
    {
        wicedfs_file_t* stream = file_handler;

        wicedfs_fread( physical_address, size, 1, stream );

        if ( aon_segment )
        {
            WICED_DEEP_SLEEP_TINY_BOOTLOADER_CONFIG->app_address = stream->location;
        }
    }
}

void platform_erase_app_area( uint32_t physical_address, uint32_t size )
{
    /* App is in RAM, no need for erase */
    UNUSED_PARAMETER( physical_address );
    UNUSED_PARAMETER( size );
}
#if (DCT_BOOTLOADER_SDK_VERSION >= DCT_BOOTLOADER_SDK_3_1_1)
void platform_load_app_chunk(const image_location_t* app_header_location, uint32_t offset, void* physical_address, uint32_t size )
{
    wiced_bool_t aon_segment;

    if ( platform_is_load_permitted( physical_address, size, &aon_segment ) )
    {
        wiced_apps_read( app_header_location, physical_address, offset, size );

        if ( aon_segment )
        {
            WICED_DEEP_SLEEP_TINY_BOOTLOADER_CONFIG->app_address = app_header_location->detail.external_fixed.location;
        }
    }
}

/*
 * Instruct the tiny bootloader to load an application when returning from Deepsleep
 */

platform_result_t platform_deepsleep_set_boot(uint8_t app_idx)
{
#if defined(BOOTLOADER_LOAD_MAIN_APP_FROM_EXTERNAL_LOCATION)
    image_location_t    source = { 0 };

    if ( wiced_dct_get_app_header_location( app_idx, &source ) == WICED_SUCCESS )
    {
        if ((source.id == EXTERNAL_FIXED_LOCATION) && (source.detail.external_fixed.location != 0x00))
        {
            WICED_DEEP_SLEEP_TINY_BOOTLOADER_CONFIG->app_address = source.detail.external_fixed.location;
            return PLATFORM_SUCCESS;
        }
    }
#else
    UNUSED_PARAMETER(app_idx);
#error Not supported

#endif  /* defined(BOOTLOADER_LOAD_MAIN_APP_FROM_EXTERNAL_LOCATION) */
    return PLATFORM_ERROR;
}

#endif  /* SDK version check */
