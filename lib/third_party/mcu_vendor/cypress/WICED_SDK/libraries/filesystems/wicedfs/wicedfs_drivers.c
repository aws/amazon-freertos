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
 *  User API driver for WicedFS
 *  Adapts the top level WicedFS API to match the Wiced API
 */

#include "wwd_assert.h"
#include "wiced_result.h"
#include "wicedfs.h"

#include "internal/wiced_filesystem_internal.h"

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
static wicedfs_usize_t wiced_filesystem_wicedfs_read_func( void* user_param, /*@out@*/ void* buf, wicedfs_usize_t size, wicedfs_usize_t pos );

static wiced_result_t wicedfs_shim_init             ( void );
static wiced_result_t wicedfs_shim_mount            ( wiced_block_device_t* device, wiced_filesystem_t* fs_handle_out );
static wiced_result_t wicedfs_shim_unmount          ( wiced_filesystem_t* fs_handle );
static wiced_result_t wicedfs_shim_file_get_details ( wiced_filesystem_t* fs_handle, const char* filename, wiced_dir_entry_details_t* details_out );
static wiced_result_t wicedfs_shim_file_open        ( wiced_filesystem_t* fs_handle, wiced_file_t* file_handle_out, const char* filename, wiced_filesystem_open_mode_t mode );
static wiced_result_t wicedfs_shim_file_seek        ( wiced_file_t* file_handle, int64_t offset, wiced_filesystem_seek_type_t whence );
static wiced_result_t wicedfs_shim_file_tell        ( wiced_file_t* file_handle, uint64_t* location );
static wiced_result_t wicedfs_shim_file_read        ( wiced_file_t* file_handle, void* data, uint64_t bytes_to_read, uint64_t* returned_bytes_count );
static wiced_result_t wicedfs_shim_file_write       ( wiced_file_t* file_handle, const void* data, uint64_t bytes_to_write, uint64_t* written_bytes_count );
static wiced_result_t wicedfs_shim_file_flush       ( wiced_file_t* file_handle );
static int            wicedfs_shim_file_end_reached ( wiced_file_t* file_handle );
static wiced_result_t wicedfs_shim_file_close       ( wiced_file_t* file_handle );
static wiced_result_t wicedfs_shim_file_delete      ( wiced_filesystem_t* fs_handle, const char* filename );
static wiced_result_t wicedfs_shim_dir_open         ( wiced_filesystem_t* fs_handle, wiced_dir_t* dir_handle, const char* dir_name );
static wiced_result_t wicedfs_shim_dir_read         ( wiced_dir_t* dir_handle, char* name_buffer, unsigned int name_buffer_length, wiced_dir_entry_type_t* type, wiced_dir_entry_details_t* details );
static int            wicedfs_shim_dir_end_reached  ( wiced_dir_t* dir_handle );
static wiced_result_t wicedfs_shim_dir_rewind       ( wiced_dir_t* dir_handle );
static wiced_result_t wicedfs_shim_dir_close        ( wiced_dir_t* dir_handle );
static wiced_result_t wicedfs_shim_dir_create       ( wiced_filesystem_t* fs_handle, const char* directory_name );
static wiced_result_t wicedfs_shim_dir_delete       ( wiced_filesystem_t* fs_handle, const char* directory_name );
static wiced_result_t wicedfs_shim_format           ( wiced_block_device_t* device );

/******************************************************
 *               Variable Definitions
 ******************************************************/

/* This is the User API driver structure for WicedFS */
wiced_filesystem_driver_t wiced_filesystem_driver_wicedfs =
{
    .init             = wicedfs_shim_init            ,
    .mount            = wicedfs_shim_mount           ,
    .unmount          = wicedfs_shim_unmount         ,
    .file_get_details = wicedfs_shim_file_get_details,
    .file_open        = wicedfs_shim_file_open       ,
    .file_seek        = wicedfs_shim_file_seek       ,
    .file_tell        = wicedfs_shim_file_tell       ,
    .file_read        = wicedfs_shim_file_read       ,
    .file_write       = wicedfs_shim_file_write      ,
    .file_flush       = wicedfs_shim_file_flush      ,
    .file_end_reached = wicedfs_shim_file_end_reached,
    .file_close       = wicedfs_shim_file_close      ,
    .file_delete      = wicedfs_shim_file_delete     ,
    .dir_open         = wicedfs_shim_dir_open        ,
    .dir_read         = wicedfs_shim_dir_read        ,
    .dir_end_reached  = wicedfs_shim_dir_end_reached ,
    .dir_rewind       = wicedfs_shim_dir_rewind      ,
    .dir_close        = wicedfs_shim_dir_close       ,
    .dir_create       = wicedfs_shim_dir_create      ,
    .dir_delete       = wicedfs_shim_dir_delete      ,
    .format           = wicedfs_shim_format          ,
};


/******************************************************
 *               Function Definitions
 ******************************************************/


/* This function is the Block Device Driver for WicedFS */
static wicedfs_usize_t wiced_filesystem_wicedfs_read_func( void* user_param, /*@out@*/ void* buf, wicedfs_usize_t size, wicedfs_usize_t pos )
{
    wiced_result_t result;
    wiced_block_device_t* device = (wiced_block_device_t*) user_param;

    result = device->driver->read( device, (wicedfs_usize_t) pos, buf, size );

    return ( result == WICED_SUCCESS )? size : 0;
}

/* Initialises WicedFS shim - nothing to be done */
static wiced_result_t wicedfs_shim_init( void )
{
    return WICED_SUCCESS;
}

/* Mounts a WicedFS filesystem from a block device */
static wiced_result_t wicedfs_shim_mount( wiced_block_device_t* device, wiced_filesystem_t* fs_handle_out )
{
    int retval;
    wiced_result_t result;

    wiced_assert( "Format not possible for Read-Only WicedFS",  device->init_data->volatile_and_requires_format_when_mounting == WICED_FALSE );

    /* Init the block device */
    result = device->driver->init( device, BLOCK_DEVICE_READ_ONLY );
    if ( result != WICED_SUCCESS )
    {
        return result;
    }

    retval = wicedfs_init( (wicedfs_usize_t) device->init_data->base_address_offset, wiced_filesystem_wicedfs_read_func, &fs_handle_out->data.wicedfs, device );
    if ( retval != 0 )
    {
        return WICED_FILESYSTEM_ERROR;
    }
    return WICED_SUCCESS;
}

/* Unmounts a WicedFS filesystem from a block device */
static wiced_result_t wicedfs_shim_unmount( wiced_filesystem_t* fs_handle )
{
    return fs_handle->device->driver->deinit( fs_handle->device );
}

/* Opens a file within a WicedFS filesystem */
static wiced_result_t wicedfs_shim_file_open( wiced_filesystem_t* fs_handle, wiced_file_t* file_handle_out, const char* filename, wiced_filesystem_open_mode_t mode )
{
    int retval;

    if ( mode != WICED_FILESYSTEM_OPEN_FOR_READ )
    {
        return WICED_FILESYSTEM_WRITE_PROTECTED;
    }

    retval = wicedfs_fopen( &fs_handle->data.wicedfs, &file_handle_out->data.wicedfs, filename );
    if ( retval != 0 )
    {
        return WICED_FILESYSTEM_ERROR;
    }
    return WICED_SUCCESS;
}

/* Get details of a file within a WicedFS filesystem */
static wiced_result_t wicedfs_shim_file_get_details ( wiced_filesystem_t* fs_handle, const char* filename, wiced_dir_entry_details_t* details_out )
{
    wicedfs_ssize_t size;
    size = wicedfs_filesize (  &fs_handle->data.wicedfs, filename );
    if ( size < 0 )
    {
        return WICED_FILESYSTEM_ERROR;
    }
    details_out->size                  = (uint64_t) size;
    details_out->attributes_available  = WICED_FALSE;
    details_out->date_time_available   = WICED_FALSE;
    details_out->permissions_available = WICED_FALSE;

    return WICED_SUCCESS;
}

/* Close a file within a WicedFS filesystem */
static wiced_result_t wicedfs_shim_file_close       ( wiced_file_t* file_handle )
{
    int result;
    result = wicedfs_fclose( &file_handle->data.wicedfs );
    if ( result != 0 )
    {
        return WICED_FILESYSTEM_ERROR;
    }

    return WICED_SUCCESS;
}

/* Seek to a location in an open file within a WicedFS filesystem */
static wiced_result_t wicedfs_shim_file_seek        ( wiced_file_t* file_handle, int64_t offset, wiced_filesystem_seek_type_t whence )
{
    if ( wicedfs_fseek( &file_handle->data.wicedfs, (wicedfs_ssize_t) offset, whence ) != 0 )
    {
        return WICED_FILESYSTEM_ERROR;
    }
    return WICED_SUCCESS;
}

/* Get the current location in an open file within a WicedFS filesystem */
static wiced_result_t wicedfs_shim_file_tell        ( wiced_file_t* file_handle, uint64_t* location )
{
    *location = (uint64_t) wicedfs_ftell( &file_handle->data.wicedfs );

    return WICED_SUCCESS;
}

/* Read data from an open file within a WicedFS filesystem */
static wiced_result_t wicedfs_shim_file_read        ( wiced_file_t* file_handle, void* data, uint64_t bytes_to_read, uint64_t* returned_bytes_count )
{
    *returned_bytes_count = (uint64_t) wicedfs_fread( data, (wicedfs_usize_t)1, (wicedfs_usize_t)bytes_to_read, &file_handle->data.wicedfs );

    return WICED_SUCCESS;
}

/* Get end-of-file (EOF) flag for an open file within a WicedFS filesystem */
static int            wicedfs_shim_file_end_reached ( wiced_file_t* file_handle )
{
    return wicedfs_feof( &file_handle->data.wicedfs );
}

/* Opens a directory within a WicedFS filesystem */
static wiced_result_t wicedfs_shim_dir_open         ( wiced_filesystem_t* fs_handle, wiced_dir_t* dir_handle, const char* dir_name )
{
    if ( wicedfs_opendir( &fs_handle->data.wicedfs,  &dir_handle->data.wicedfs, dir_name ) != 0 )
    {
        return WICED_FILESYSTEM_ERROR;
    }
    return WICED_SUCCESS;
}

/* Reads directory entry from an open within a WicedFS filesystem */
static wiced_result_t wicedfs_shim_dir_read         ( wiced_dir_t* dir_handle, char* name_buffer, unsigned int name_buffer_length, wiced_dir_entry_type_t* type, wiced_dir_entry_details_t* details_out )
{
    wicedfs_entry_type_t tmp_type;
    wicedfs_usize_t size;

    if ( wicedfs_readdir_with_size( &dir_handle->data.wicedfs, name_buffer, name_buffer_length, &tmp_type, &size ) != 0 )
    {
        return WICED_FILESYSTEM_ERROR;
    }

    /* Fill in the detail structure */
    details_out->size                  = (uint64_t) size;
    details_out->attributes_available  = WICED_FALSE;
    details_out->date_time_available   = WICED_FALSE;
    details_out->permissions_available = WICED_FALSE;

    *type = ( wiced_dir_entry_type_t ) ( ( tmp_type == WICEDFS_FILE ) ? WICED_FILESYSTEM_FILE :
            ( tmp_type == WICEDFS_DIR  ) ? WICED_FILESYSTEM_DIR  : 0 );

    return WICED_SUCCESS;
}

/* Get end-of-directory flag for an open directory within a WicedFS filesystem */
static int            wicedfs_shim_dir_end_reached  ( wiced_dir_t* dir_handle )
{
    return wicedfs_eodir( &dir_handle->data.wicedfs );
}

/* Moves the current location within a directory back to the first entry within a WicedFS filesystem */
static wiced_result_t wicedfs_shim_dir_rewind       ( wiced_dir_t* dir_handle )
{
    wicedfs_rewinddir( &dir_handle->data.wicedfs );
    return WICED_SUCCESS;
}

/* Closes an open directory within a WicedFS filesystem */
static wiced_result_t wicedfs_shim_dir_close        ( wiced_dir_t* dir_handle )
{
    if ( wicedfs_closedir( &dir_handle->data.wicedfs ) != 0 )
    {
        return WICED_FILESYSTEM_ERROR;
    }
    return WICED_SUCCESS;
}






/******************************************************
 * Unimplemented Functions - Due to being Read-Only
 ******************************************************/

static wiced_result_t wicedfs_shim_file_delete      ( wiced_filesystem_t* fs_handle, const char* filename )
{
    UNUSED_PARAMETER( fs_handle );
    UNUSED_PARAMETER( filename );
    wiced_assert( "WicedFS is Read-Only!", 1 == 0 );
    return ( wiced_result_t ) WICED_FILESYSTEM_ATTRIBUTE_READ_ONLY;
}

static wiced_result_t wicedfs_shim_file_write       ( wiced_file_t* file_handle, const void* data, uint64_t bytes_to_write, uint64_t* written_bytes_count )
{
    UNUSED_PARAMETER( file_handle );
    UNUSED_PARAMETER( data );
    UNUSED_PARAMETER( bytes_to_write );
    UNUSED_PARAMETER( written_bytes_count );
    wiced_assert( "WicedFS is Read-Only!", 1 == 0 );
    return WICED_FILESYSTEM_WRITE_PROTECTED;
}

static wiced_result_t wicedfs_shim_file_flush       ( wiced_file_t* file_handle )
{
    UNUSED_PARAMETER( file_handle );
    wiced_assert( "WicedFS is Read-Only!", 1 == 0 );
    return WICED_FILESYSTEM_WRITE_PROTECTED;
}

static wiced_result_t wicedfs_shim_dir_create       ( wiced_filesystem_t* fs_handle, const char* directory_name )
{
    UNUSED_PARAMETER( fs_handle );
    UNUSED_PARAMETER( directory_name );
    wiced_assert( "WicedFS is Read-Only!", 1 == 0 );
    return WICED_FILESYSTEM_WRITE_PROTECTED;
}

static wiced_result_t wicedfs_shim_dir_delete       ( wiced_filesystem_t* fs_handle, const char* directory_name )
{
    UNUSED_PARAMETER( fs_handle );
    UNUSED_PARAMETER( directory_name );
    wiced_assert( "WicedFS is Read-Only!", 1 == 0 );
    return WICED_FILESYSTEM_WRITE_PROTECTED;
}

static wiced_result_t wicedfs_shim_format( wiced_block_device_t* device )
{
    UNUSED_PARAMETER( device );
    wiced_assert( "WicedFS is Read-Only!", 1 == 0 );
    return WICED_FILESYSTEM_WRITE_PROTECTED;
}
