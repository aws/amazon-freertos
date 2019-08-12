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
 * Handle WAF (WICED Application Framework) common stuff.
 * Mainly:
 *      * Erasing Apps
 *      * Loading Apps
 *      * Setting to boot from certain App
 */
/* include platform first so defines therein can take precedence */
#include "platform.h"
#include "wwd_assert.h"
#include "wiced_result.h"
#include "wiced_utilities.h"
#include "platform_dct.h"
#include "wiced_framework.h"
#include "wiced_apps_common.h"
#include "wiced_waf_common.h"
#include "wiced_dct_common.h"
#include "waf_platform.h"
#include "wicedfs.h"
#include "elf.h"
#include "platform_peripheral.h"
#include "platform_resource.h"

/******************************************************
 *                      Macros
 ******************************************************/

/******************************************************
 *                    Constants
 ******************************************************/

#define PLATFORM_SFLASH_PERIPHERAL_ID  (0)

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
 *               Function Declarations
 ******************************************************/

/******************************************************
 *               Variables Definitions
 ******************************************************/

/******************************************************
 *               Function Definitions
 ******************************************************/

wiced_result_t wiced_waf_reboot( void )
{
    /* Reset request */
    platform_mcu_reset( );

    return WICED_SUCCESS;
}

#if (DCT_BOOTLOADER_SDK_VERSION < DCT_BOOTLOADER_SDK_3_1_2)
    /* this SDK does not have apps_locations in bootloader_dct_header_t (platform_dct_header_t for the SDK) */
#else

wiced_result_t wiced_waf_app_set_boot(uint8_t app_id, char load_once)
{
    boot_detail_t boot;

    boot.entry_point                 = 0;
    boot.load_details.load_once      = load_once;
    boot.load_details.valid          = 1;
    boot.load_details.destination.id = INTERNAL;

    if ( wiced_dct_get_app_header_location( app_id, &boot.load_details.source ) != WICED_SUCCESS )
    {
        return WICED_ERROR;
    }
    if ( wiced_dct_write_boot_details( &boot ) != WICED_SUCCESS)
        return WICED_ERROR;

    return WICED_SUCCESS;
}


wiced_result_t wiced_waf_app_open( uint8_t app_id, wiced_app_t* app )
{
    if ( wiced_dct_get_app_header_location( app_id, &app->app_header_location ) != WICED_SUCCESS )
    {
        return WICED_ERROR;
    }
    app->offset = 0x00000000;
    app->app_id = app_id;
    app->last_erased_sector = 0xFFFFFFFF;

    return WICED_SUCCESS;
}

wiced_result_t wiced_waf_app_close( wiced_app_t* app )
{
    (void) app;
    return WICED_SUCCESS;
}

wiced_result_t wiced_waf_app_erase( wiced_app_t* app )
{
    return wiced_apps_erase( &app->app_header_location );
}

wiced_result_t wiced_waf_app_get_size( wiced_app_t* app, uint32_t* size )
{
    return wiced_apps_get_size( &app->app_header_location, size );
}

wiced_result_t wiced_waf_app_set_size(wiced_app_t* app, uint32_t size)
{
    if ( wiced_apps_set_size( &app->app_header_location, size ) != WICED_SUCCESS )
    {
        return WICED_ERROR;
    }
    return wiced_dct_set_app_header_location( app->app_id, &app->app_header_location );
}

static wiced_bool_t wiced_waf_is_elf_segment_load( elf_program_header_t* prog_header )
{
    if ( ( prog_header->data_size_in_file == 0 ) || /* size is zero */
         ( ( prog_header->type & 0x1 ) == 0 ) ) /* non- loadable segment */
    {
        return WICED_FALSE;
    }

    return WICED_TRUE;
}

static wiced_result_t wiced_waf_app_area_erase(const image_location_t* app_header_location)
{
    elf_header_t header;
    uint32_t     i;
    uint32_t     start_address = 0xFFFFFFFF;
    uint32_t     end_address   = 0x00000000;

    wiced_apps_read( app_header_location, (uint8_t*) &header, 0, sizeof( header ) );

    for ( i = 0; i < header.program_header_entry_count; i++ )
    {
        elf_program_header_t prog_header;
        unsigned long offset;

        offset = header.program_header_offset + header.program_header_entry_size * (unsigned long) i;
        wiced_apps_read( app_header_location, (uint8_t*) &prog_header, offset, sizeof( prog_header ) );

        if ( wiced_waf_is_elf_segment_load( &prog_header ) == WICED_FALSE )
        {
            continue;
        }

        if ( prog_header.physical_address < start_address )
        {
            start_address = prog_header.physical_address;
        }

        if ( prog_header.physical_address + prog_header.data_size_in_file > end_address )
        {
            end_address = prog_header.physical_address + prog_header.data_size_in_file;
        }
    }
    platform_erase_app_area( start_address, end_address - start_address );
    return WICED_SUCCESS;
}

wiced_result_t wiced_waf_app_write_chunk( wiced_app_t* app, const uint8_t* data, uint32_t size)
{
    wiced_result_t  result;
    result = wiced_apps_write( &app->app_header_location, data, app->offset, size, &app->last_erased_sector );
    if ( result == WICED_SUCCESS )
    {
        app->offset += size;
    }
    return result;
}

wiced_result_t wiced_waf_app_read_chunk( wiced_app_t* app, uint32_t offset, uint8_t* data, uint32_t size )
{
    return wiced_apps_read( &app->app_header_location, data, offset, size );
}

wiced_result_t wiced_waf_app_load( const image_location_t* app_header_location, uint32_t* destination )
{
    wiced_result_t       result = WICED_BADARG;
    elf_header_t         header;

    UNUSED_PARAMETER(app_header_location);
    (void)wiced_waf_app_area_erase;
#ifdef BOOTLOADER_LOAD_MAIN_APP_FROM_FILESYSTEM
    if ( app_header_location->id == EXTERNAL_FILESYSTEM_FILE )
    {
        uint32_t i;
        elf_program_header_t prog_header;
        wicedfs_file_t f_in;

        /* Read the image header */
        wicedfs_fopen( &resource_fs_handle, &f_in, app_header_location->detail.filesystem_filename );
        wicedfs_fread( &header, sizeof(header), 1, &f_in );

        for( i = 0; i < header.program_header_entry_count; i++ )
        {
            wicedfs_fseek( &f_in, (wicedfs_ssize_t)( header.program_header_offset + header.program_header_entry_size * i ), SEEK_SET);
            wicedfs_fread( &prog_header, sizeof(prog_header), 1, &f_in );

            if ( wiced_waf_is_elf_segment_load( &prog_header ) == WICED_FALSE )
            {
                continue;
            }

            wicedfs_fseek( &f_in, (wicedfs_ssize_t)prog_header.data_offset, SEEK_SET);
            platform_load_app_chunk_from_fs( app_header_location, &f_in, (void*) prog_header.physical_address, prog_header.data_size_in_file );
        }

        wicedfs_fclose( &f_in );

        result = WICED_SUCCESS;
    }

#else
    if ( app_header_location->id == EXTERNAL_FIXED_LOCATION )
    {
        uint32_t i;
        elf_program_header_t prog_header;

        /* Erase the application area */
        if ( wiced_waf_app_area_erase( app_header_location ) != WICED_SUCCESS )
        {
            return WICED_ERROR;
        }

        /* Read the image header */
        wiced_apps_read( app_header_location, (uint8_t*) &header, 0, sizeof( header ) );

        for ( i = 0; i < header.program_header_entry_count; i++ )
        {
            unsigned long offset = header.program_header_offset + header.program_header_entry_size * (unsigned long) i;

            wiced_apps_read( app_header_location, (uint8_t*) &prog_header, offset, sizeof( prog_header ) );

            if ( wiced_waf_is_elf_segment_load( &prog_header ) == WICED_FALSE )
            {
                continue;
            }

            offset = prog_header.data_offset;
            platform_load_app_chunk( app_header_location, offset, (void*) prog_header.physical_address, prog_header.data_size_in_file );
        }

        result = WICED_SUCCESS;
    }

#endif /* BOOTLOADER_LOAD_MAIN_APP_FROM_FILESYSTEM */
    if ( result == WICED_SUCCESS )
    {
        *(uint32_t *) destination = header.entry;
    }

    return result;
}

void wiced_waf_start_app( uint32_t entry_point )
{
    platform_start_app( entry_point );
}
#endif

wiced_result_t wiced_waf_check_factory_reset( void )
{
    uint32_t factory_reset_time;

    factory_reset_time = platform_get_factory_reset_button_time( PLATFORM_FACTORY_RESET_TIMEOUT );
    if (factory_reset_time >= PLATFORM_FACTORY_RESET_TIMEOUT)
    {
        /* The operation time ran long; return WICED_TIMEOUT */
        return WICED_TIMEOUT;
    }
    /* the time was short; return zero */
    return WICED_SUCCESS;
}

uint32_t wiced_waf_get_button_press_time( int button_index, int led_index, uint32_t max_time )
{
    return platform_get_button_press_time( button_index, led_index, max_time );
}


uint32_t wiced_waf_get_factory_reset_button_time( void )
{
    return platform_get_factory_reset_button_time( PLATFORM_FACTORY_RESET_TIMEOUT );
}
