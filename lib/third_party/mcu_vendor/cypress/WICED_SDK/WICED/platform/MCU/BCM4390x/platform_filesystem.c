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
 * Defines BCM439x filesystem
 */
#include "stdint.h"
#include "string.h"
#include "platform_init.h"
#include "platform_peripheral.h"
#include "platform_mcu_peripheral.h"
#include "platform_stdio.h"
#include "platform_sleep.h"
#include "platform_config.h"
#include "platform_sflash_dct.h"
#include "platform_dct.h"
#include "wwd_constants.h"
#include "wwd_rtos.h"
#include "wwd_assert.h"
#include "RTOS/wwd_rtos_interface.h"
#include "spi_flash.h"
#include "wicedfs.h"
#include "wiced_framework.h"
#include "wiced_dct_common.h"
#include "wiced_apps_common.h"
#include "wiced_block_device.h"

#include "platform_sdio.h"
/* to get glob_sd structure */
#ifndef BCMSDIO
#define BCMSDIO
#endif /* BCMSDIO */
#include "sdio.h"
#include "osl.h"
#include "bcmsdstd.h"

/******************************************************
 *                      Macros
 ******************************************************/

/******************************************************
 *                    Constants
 ******************************************************/

#define DDR_BASE      PLATFORM_DDR_BASE(0x0)
#define SD_BLOCK_SZ   (512)

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

static wicedfs_usize_t read_callback ( void* user_param, void* buf, wicedfs_usize_t size, wicedfs_usize_t pos );

/******************************************************
 *               Variables Definitions
 ******************************************************/

host_semaphore_type_t sflash_mutex;
sflash_handle_t       wicedfs_sflash_handle;
wicedfs_filesystem_t  resource_fs_handle;
static  wiced_app_t   fs_app;
static  uint8_t       fs_init_done = 0;

#if (DCT_BOOTLOADER_SDK_VERSION >= DCT_BOOTLOADER_SDK_3_1_2)
static  wiced_app_t   fs_app;
#endif
/******************************************************
 *               Function Definitions
 ******************************************************/

void platform_sflash_init( void )
{
    host_rtos_init_semaphore( &sflash_mutex );

    host_rtos_set_semaphore( &sflash_mutex, WICED_FALSE );
}

/* To handle WWD applications that don't go through wiced_init() (yet need to use the file system):
 * File system initialization is called twice
 * wiced_init()->wiced_core_init()->wiced_platform_init()->platform_filesystem_init()
 * wwd_management_wifi_on()->host_platform_init()->platform_filesystem_init()
 */
platform_result_t platform_filesystem_init( void )
{
    int             result;

    if ( fs_init_done == 0)
    {
        if ( init_sflash( &wicedfs_sflash_handle, 0, SFLASH_WRITE_NOT_ALLOWED ) != WICED_SUCCESS )
        {
            return PLATFORM_ERROR;
        }

#if (DCT_BOOTLOADER_SDK_VERSION < DCT_BOOTLOADER_SDK_3_1_2)
        /* this SDK does not have apps_locations in bootloader_dct_header_t (platform_dct_header_t for the SDK) */
#else
        if ( wiced_framework_app_open( DCT_FILESYSTEM_IMAGE_INDEX, &fs_app ) != WICED_SUCCESS )
        {
            return PLATFORM_ERROR;
        }
#endif /* (DCT_BOOTLOADER_SDK_VERSION < DCT_BOOTLOADER_SDK_3_1_2) */

        result = wicedfs_init( 0, read_callback, &resource_fs_handle, &wicedfs_sflash_handle );
        wiced_assert( "wicedfs init fail", result == 0 );
        fs_init_done = ( result == 0 ) ? 1 : 0;
        return ( result == 0 ) ? PLATFORM_SUCCESS : PLATFORM_ERROR;
    }
    return PLATFORM_SUCCESS;
}

static wicedfs_usize_t read_callback( void* user_param, void* buf, wicedfs_usize_t size, wicedfs_usize_t pos )
{
#if (DCT_BOOTLOADER_SDK_VERSION < DCT_BOOTLOADER_SDK_3_1_2)
    UNUSED_PARAMETER(user_param);
    UNUSED_PARAMETER(buf);
    UNUSED_PARAMETER(size);
    UNUSED_PARAMETER(pos);
            /* this SDK does not have apps_locations in bootloader_dct_header_t (platform_dct_header_t for the SDK) */
    return 0;
#else
    wiced_result_t retval;
    (void) user_param;
    retval = wiced_framework_app_read_chunk( &fs_app, pos, (uint8_t*) buf, size );
    return ( ( WICED_SUCCESS == retval ) ? size : 0 );
#endif /* (DCT_BOOTLOADER_SDK_VERSION < DCT_BOOTLOADER_SDK_3_1_2) */
}

platform_result_t platform_get_sflash_dct_loc( sflash_handle_t* sflash_handle, uint32_t* loc )
{
    UNUSED_PARAMETER( sflash_handle );

    *loc = 0;
    return PLATFORM_SUCCESS;
}

#if !PLATFORM_NO_DDR

static wiced_result_t ddr_block_device_init( wiced_block_device_t* device, wiced_block_device_write_mode_t write_mode )
{
    ddr_block_device_specific_data_t* ddr_specific_data = (ddr_block_device_specific_data_t*) device->device_specific_data;

    if ( ! PLATFORM_FEATURE_ENAB(DDR) )
    {
        return WICED_ERROR;
    }

    if ( device->init_data->base_address_offset == (uint64_t)-1 )
    {
        ddr_specific_data->offset = PLATFORM_DDR_FREE_OFFSET;
    }
    else
    {
        ddr_specific_data->offset = (uint32_t)device->init_data->base_address_offset;
    }
    ddr_specific_data->offset = MIN( ddr_specific_data->offset, platform_ddr_get_size( ) );

    if ( device->init_data->maximum_size == 0 )
    {
        device->device_size = platform_ddr_get_size( ) - ddr_specific_data->offset;
    }
    else
    {
        device->device_size = MIN( platform_ddr_get_size( ) - ddr_specific_data->offset, device->init_data->maximum_size );
    }

    device->device_id        = 0;
    device->erase_block_size = 0;
    device->read_block_size  = 1;
    device->write_block_size = 1;

    ddr_specific_data->write_mode = write_mode;

    device->initialized = WICED_TRUE;

    return WICED_SUCCESS;
}

static wiced_result_t ddr_block_device_deinit( wiced_block_device_t* device )
{
    device->initialized = WICED_FALSE;

    return WICED_SUCCESS;
}

static wiced_result_t ddr_block_write( wiced_block_device_t* device, uint64_t start_address, const uint8_t* data, uint64_t size )
{
    ddr_block_device_specific_data_t* ddr_specific_data = (ddr_block_device_specific_data_t*) device->device_specific_data;
    if ( start_address + size > device->device_size )
    {
        return WICED_BADARG;
    }
    memcpy( (void*)(ptrdiff_t)( DDR_BASE + start_address + ddr_specific_data->offset ), data, (size_t) size );
    return WICED_SUCCESS;
}

static wiced_result_t ddr_block_flush( wiced_block_device_t* device )
{
    UNUSED_PARAMETER( device );
    return WICED_SUCCESS;
}

static wiced_result_t ddr_block_read( wiced_block_device_t* device, uint64_t start_address, uint8_t* data, uint64_t size )
{
    ddr_block_device_specific_data_t* ddr_specific_data = (ddr_block_device_specific_data_t*) device->device_specific_data;
    if ( start_address + size > device->device_size )
    {
        return WICED_BADARG;
    }
    memcpy( data, (void*)(ptrdiff_t)( DDR_BASE + start_address + ddr_specific_data->offset ), (size_t) size );
    return WICED_SUCCESS;
}

static wiced_result_t ddr_block_register_callback( wiced_block_device_t* device, wiced_block_device_status_change_callback_t callback )
{
    UNUSED_PARAMETER( device );
    UNUSED_PARAMETER( callback );
    return WICED_SUCCESS;
}

static wiced_result_t ddr_block_status( wiced_block_device_t* device, wiced_block_device_status_t* status )
{
    UNUSED_PARAMETER( device );
    *status = BLOCK_DEVICE_UP_READ_WRITE;
    return WICED_SUCCESS;
}

const wiced_block_device_driver_t ddr_block_device_driver =
{
    .init                = ddr_block_device_init,
    .deinit              = ddr_block_device_deinit,
    .erase               = NULL,
    .write               = ddr_block_write,
    .flush               = ddr_block_flush,
    .read                = ddr_block_read,
    .register_callback   = ddr_block_register_callback,
    .status              = ddr_block_status,
};

#endif /* !PLATFORM_NO_DDR */

#if !PLATFORM_NO_SDIO
#ifdef WICED_SDMMC_SUPPORT

extern sdioh_info_t *glob_sd;

/* Client Interrupt handler */
static void sdcard_client_check_isr(void * unused)
{
    UNUSED_PARAMETER(unused);
    /*  Handle client interrupt here. The sequence should be:
     *  1. disable client interrupt
     *  2. signal client thread by mutex or semaphore to indicate interrupt arrives
     *  3. process interrupt in client thread
     *  4. enable client interrupt
     */
}

static wiced_result_t sdmmc_block_device_init( wiced_block_device_t* device, wiced_block_device_write_mode_t write_mode )
{
    UNUSED_PARAMETER( write_mode );

    if ( ! PLATFORM_FEATURE_ENAB(SDIO) )
    {
        return WICED_ERROR;
    }

    if ( ( platform_sdio_host_init( sdcard_client_check_isr ) != WICED_SUCCESS ) || ( glob_sd == NULL ) )
    {
        wiced_assert( "SDIO Host init FAIL", 0 );
        return WICED_ERROR;
    }

    device->device_specific_data = glob_sd;

    device->device_id = 123;
    device->device_size = ( glob_sd->csd_blocknr_512 ) * (uint64_t) SD_BLOCK_SZ;
    device->read_block_size = SD_BLOCK_SZ;
    device->write_block_size = SD_BLOCK_SZ;
    device->erase_block_size = SD_BLOCK_SZ;
    device->initialized = WICED_TRUE;

    return WICED_SUCCESS;
}

static wiced_result_t sdmmc_block_device_deinit( wiced_block_device_t* device )
{
    platform_sdio_host_deinit();

    device->device_id = 0;
    device->device_size = 0;
    device->read_block_size = 0;
    device->write_block_size = 0;
    device->erase_block_size = 0;
    device->initialized = WICED_FALSE;

    return WICED_SUCCESS;
}

static wiced_result_t sdmmc_block_write( wiced_block_device_t* device, uint64_t start_address, const uint8_t* data, uint64_t size )
{
    wiced_assert( "sdmmc_block_write: start_address is not multiple of write blocks", ( start_address % device->write_block_size ) == 0 );
    wiced_assert( "sdmmc_block_write: size is not multiple of write blocks", ( size % device->write_block_size ) == 0 );

    return platform_sdio_host_write_block( (uint32_t) ( start_address / device->write_block_size ), (uint32_t) ( size / device->write_block_size ), data );
}

static wiced_result_t sdmmc_block_flush( wiced_block_device_t* device )
{
    UNUSED_PARAMETER( device );

    return WICED_SUCCESS;
}

static wiced_result_t sdmmc_block_read( wiced_block_device_t* device, uint64_t start_address, uint8_t* data, uint64_t size )
{
    wiced_assert( "sdmmc_block_write: start_address is not multiple of write blocks", ( start_address % device->write_block_size ) == 0 );
    wiced_assert( "sdmmc_block_read: size is not multiple of read blocks", ( size % device->read_block_size ) == 0 );

    return platform_sdio_host_read_block( (uint32_t) ( start_address / device->write_block_size ), (uint32_t) ( size / device->read_block_size ), data );
}

static wiced_result_t sdmmc_block_register_callback( wiced_block_device_t* device, wiced_block_device_status_change_callback_t callback )
{
    UNUSED_PARAMETER( device );
    UNUSED_PARAMETER( callback );

    return WICED_SUCCESS;
}

static wiced_result_t sdmmc_block_status( wiced_block_device_t* device, wiced_block_device_status_t* status )
{
    UNUSED_PARAMETER( device );
    UNUSED_PARAMETER( status );

    return WICED_SUCCESS;
}

const wiced_block_device_driver_t sdmmc_block_device_driver =
{
    .init                = sdmmc_block_device_init,
    .deinit              = sdmmc_block_device_deinit,
    .erase               = NULL,
    .write               = sdmmc_block_write,
    .flush               = sdmmc_block_flush,
    .read                = sdmmc_block_read,
    .register_callback   = sdmmc_block_register_callback,
    .status              = sdmmc_block_status,
};

#endif /* WICED_SDMMC_SUPPORT */
#endif /* !PLATFORM_NO_SDIO */
