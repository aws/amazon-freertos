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
 *  Implementation of the WicedFS External-Use file system.
 */

#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include "wiced_filesystem.h"
#include "wiced_filesystem_internal.h"
#include "wwd_debug.h"
#include "wwd_assert.h"

/******************************************************
 *                      Macros
 ******************************************************/

/******************************************************
 *                    Constants
 ******************************************************/

#define DEFAULT_SECTOR_SIZE  (512)

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
 *                 Static Variables
 ******************************************************/

static wiced_bool_t wiced_filesystem_inited = WICED_FALSE;

static wiced_filesystem_mounted_device_t mounted_table[WICED_FILESYSTEM_MOUNT_DEVICE_NUM_MAX];
static uint32_t total_mounted_index = 0;

/******************************************************
 *               Static Function Declarations
 ******************************************************/

static wiced_result_t wiced_filesystem_add_mounted_device ( wiced_filesystem_t* fs_handle, const char* mounted_name);
static wiced_result_t wiced_filesystem_del_mounted_device ( wiced_filesystem_t* fs_handle );

/******************************************************
 *               Variable Definitions
 ******************************************************/




/******************************************************
 *               Function Definitions
 ******************************************************/

wiced_result_t wiced_filesystem_init ( void )
{
    if ( wiced_filesystem_inited == WICED_TRUE )
    {
        return WICED_SUCCESS;
    }

    memset( (void *) mounted_table, 0, sizeof(mounted_table) );
    total_mounted_index = 0;

#ifdef USING_WICEDFS
    WICED_VERIFY( wiced_filesystem_driver_wicedfs.init() );
#endif /* USING_WICEDFS */
#ifdef USING_FATFS
    WICED_VERIFY( wiced_filesystem_driver_fatfs.init() );
#endif /* USING_FATFS */
#ifdef USING_FILEX
    WICED_VERIFY( wiced_filesystem_driver_filex.init() );
#endif /* USING_FILEX */
#ifdef USING_FILEX_USBX
    WICED_VERIFY( wiced_filesystem_driver_filex_usbx.init() );
#endif /* USING_FILEX_USBX */
    wiced_filesystem_inited = WICED_TRUE;
    return WICED_SUCCESS;
}

static wiced_result_t wiced_filesystem_add_mounted_device ( wiced_filesystem_t* fs_handle, const char* mounted_name)
{
    uint32_t i;

    /* Error checking */
    if ( (fs_handle == NULL) || (mounted_name == NULL) )
    {
        WPRINT_WICED_ERROR( ("Null input!\n") );
        return WICED_ERROR;
    }

    if ( total_mounted_index >= WICED_FILESYSTEM_MOUNT_DEVICE_NUM_MAX )
    {
        WPRINT_WICED_ERROR( ("Mounted device number exceeds upper limit!\n") );
        return WICED_ERROR;
    }

    if ( strlen(mounted_name) > (WICED_FILESYSTEM_MOUNT_NAME_LENGTH_MAX - 1) )
    {
        WPRINT_WICED_ERROR( ("Device name length too long!\n") );
        return WICED_ERROR;
    }

    /* Device duplicated checking */
    for ( i = 0; i < WICED_FILESYSTEM_MOUNT_DEVICE_NUM_MAX; i++ )
    {
        if ( mounted_table[i].fs_handle == fs_handle )
        {
            WPRINT_WICED_ERROR( ("Duplicated handle with index %ld\n", i) );
            return WICED_ERROR;
        }
        if ( strcmp(mounted_table[i].name, mounted_name) == 0 )
        {
            WPRINT_WICED_ERROR( ("Duplicated name with index %ld\n", i) );
            return WICED_ERROR;
        }
    }

    /* Add into table */
    mounted_table[total_mounted_index].fs_handle = fs_handle;
    memcpy( mounted_table[total_mounted_index].name, mounted_name, WICED_FILESYSTEM_MOUNT_NAME_LENGTH_MAX );
    mounted_table[total_mounted_index].name[strlen(mounted_name)] = '\0';
    WPRINT_WICED_INFO( ("Added %s into mounted table with index %lu\n", mounted_table[total_mounted_index].name, (unsigned long)total_mounted_index) );

    /* Increase index if all right */
    total_mounted_index ++;

    /* Only for debug dump */
    #if 0
    printf( "\n--- Filesystem Mounted Table ---\n");
    for ( i = 0; i < WICED_FILESYSTEM_MOUNT_DEVICE_NUM_MAX; i++ )
    {
        printf( "[%02ld]\t %s\t\t 0x%08lx\n", i, mounted_table[i].name, (uint32_t)mounted_table[i].fs_handle);
    }
    #endif

    return WICED_SUCCESS;
}

static wiced_result_t wiced_filesystem_del_mounted_device ( wiced_filesystem_t* fs_handle )
{
    wiced_bool_t is_handle_found = WICED_FALSE;
    uint32_t shift_up_num = 0;
    uint32_t i;

    /* Error checking */
    if ( fs_handle == NULL )
    {
        WPRINT_WICED_ERROR( ("Null input!\n") );
        return WICED_ERROR;
    }

    /* Find device in mounted table */
    for ( i = 0; i < WICED_FILESYSTEM_MOUNT_DEVICE_NUM_MAX; i++ )
    {
        if ( mounted_table[i].fs_handle == fs_handle )
        {
            WPRINT_WICED_INFO( ("Found device in mounted table with index %lu\n", (unsigned long)i) );
            is_handle_found = WICED_TRUE;
            break;
        }
    }
    if ( (is_handle_found == WICED_FALSE) || (i > total_mounted_index) )
    {
        WPRINT_WICED_ERROR( ("Not existing mounted device!\n") );
        return WICED_ERROR;
    }

    /* Delete in table (shift up in table if deleted not the last one) */
    shift_up_num = (total_mounted_index - i);
    WPRINT_WICED_INFO( ("Deleting %s in mounted table with index %lu. (shift up %lu)\n", mounted_table[i].name, (unsigned long)i, (unsigned long)shift_up_num) );

    if ( ( shift_up_num != 0 ) && ( i < ( WICED_FILESYSTEM_MOUNT_DEVICE_NUM_MAX - 1 ) ) )
    {
        /* If shift up not zero, copy up before clearing the last entry */
        memcpy( (void *) &mounted_table[i], (void *) &mounted_table[i+1], (sizeof(wiced_filesystem_mounted_device_t) * shift_up_num) );
    }
    memset( (void *) &mounted_table[total_mounted_index], 0, sizeof(wiced_filesystem_mounted_device_t) );

    /* Decrease index if all right */
    total_mounted_index --;

    /* Only for debug dump */
    #if 0
    printf( "\n--- Filesystem Mounted Table ---\n");
    for ( i = 0; i < WICED_FILESYSTEM_MOUNT_DEVICE_NUM_MAX; i++ )
    {
        printf( "[%02ld]\t %s\t\t 0x%08lx\n", i, mounted_table[i].name, (uint32_t)mounted_table[i].fs_handle);
    }
    #endif

    return WICED_SUCCESS;
}

wiced_result_t wiced_filesystem_mount ( wiced_block_device_t* device, wiced_filesystem_handle_type_t fs_type, wiced_filesystem_t* fs_handle_out, const char* mounted_name )
{
    wiced_result_t result;

    /* These ifdefs ensure that the drivers are only pulled in if they are used */
    switch ( fs_type )
    {
#ifdef USING_WICEDFS
        case WICED_FILESYSTEM_HANDLE_WICEDFS:
            fs_handle_out->driver = &wiced_filesystem_driver_wicedfs;
            break;
#endif /* ifdef USING_WICEDFS */
#ifdef USING_FATFS
        case WICED_FILESYSTEM_HANDLE_FATFS:
            fs_handle_out->driver = &wiced_filesystem_driver_fatfs;
            break;
#endif /* ifdef USING_FATFS */
#ifdef USING_FILEX
        case WICED_FILESYSTEM_HANDLE_FILEX:
            fs_handle_out->driver = &wiced_filesystem_driver_filex;
            break;
#endif /* ifdef USING_FILEX */
#ifdef USING_FILEX_USBX
        case WICED_FILESYSTEM_HANDLE_FILEX_USBX:
            fs_handle_out->driver = &wiced_filesystem_driver_filex_usbx;
            break;
#endif /* ifdef USING_FILEX_USBX */

#ifndef USING_WICEDFS
        case WICED_FILESYSTEM_HANDLE_WICEDFS:
#endif /* ifdef USING_WICEDFS */
#ifndef USING_FATFS
        case WICED_FILESYSTEM_HANDLE_FATFS:
#endif /* ifdef USING_FATFS */
#ifndef USING_FILEX
        case WICED_FILESYSTEM_HANDLE_FILEX:
#endif /* ifndef USING_FILEX */
#ifndef USING_FILEX_USBX
        case WICED_FILESYSTEM_HANDLE_FILEX_USBX:
#endif /* ifdef USING_FILEX_USBX */
        default:
            return WICED_FILESYSTEM_ERROR;
    }

    fs_handle_out->device = device;

    result = fs_handle_out->driver->mount( device, fs_handle_out );
    if ( result == WICED_SUCCESS )
    {
       result = wiced_filesystem_add_mounted_device ( fs_handle_out, mounted_name);
    }

    return result;
}

wiced_result_t wiced_filesystem_unmount ( wiced_filesystem_t* fs_handle )
{
    wiced_result_t result;

    result = fs_handle->driver->unmount( fs_handle );
    if ( result == WICED_SUCCESS )
    {
        result = wiced_filesystem_del_mounted_device ( fs_handle );
    }

    return result;
}

wiced_filesystem_t* wiced_filesystem_retrieve_mounted_fs_handle ( const char* mounted_name )
{
    wiced_filesystem_t* fs_handle_get = NULL;
    uint32_t i;

    /* Find device in mounted table */
    for ( i = 0; i < WICED_FILESYSTEM_MOUNT_DEVICE_NUM_MAX; i++ )
    {
        if ( strcmp(mounted_table[i].name, mounted_name) == 0 )
        {
            WPRINT_WICED_INFO( ("Found name in mounted table with index %lu\n", (unsigned long)i) );
            fs_handle_get = mounted_table[i].fs_handle;
            break;
        }
    }

    return fs_handle_get;
}

wiced_result_t wiced_filesystem_file_open ( wiced_filesystem_t* fs_handle, wiced_file_t* file_handle_out, const char* filename, wiced_filesystem_open_mode_t mode )
{
    file_handle_out->filesystem = fs_handle;
    file_handle_out->driver     = fs_handle->driver;
    return fs_handle->driver->file_open( fs_handle, file_handle_out, filename, mode );
}

wiced_result_t wiced_filesystem_file_get_details ( wiced_filesystem_t* fs_handle, const char* filename, wiced_dir_entry_details_t* details_out )
{
    return fs_handle->driver->file_get_details( fs_handle, filename, details_out );
}

wiced_result_t wiced_filesystem_file_close ( wiced_file_t* file_handle )
{
    return file_handle->driver->file_close( file_handle );
}

wiced_result_t wiced_filesystem_file_delete ( wiced_filesystem_t* fs_handle, const char* filename )
{
    return fs_handle->driver->file_delete( fs_handle, filename );
}

wiced_result_t wiced_filesystem_file_seek ( wiced_file_t* file_handle, int64_t offset, wiced_filesystem_seek_type_t whence )
{
    return file_handle->driver->file_seek( file_handle, offset, whence );
}

wiced_result_t wiced_filesystem_file_tell ( wiced_file_t* file_handle, uint64_t* location )
{
    return file_handle->driver->file_tell( file_handle, location );
}

wiced_result_t wiced_filesystem_file_read ( wiced_file_t* file_handle, void* data, uint64_t bytes_to_read, uint64_t* returned_bytes_count )
{
    return file_handle->driver->file_read( file_handle, data, bytes_to_read, returned_bytes_count );
}

wiced_result_t wiced_filesystem_file_write( wiced_file_t* file_handle, const void* data, uint64_t bytes_to_write, uint64_t* written_bytes_count )
{
    return file_handle->driver->file_write( file_handle, data, bytes_to_write, written_bytes_count );
}

wiced_result_t wiced_filesystem_file_flush ( wiced_file_t* file_handle )
{
    return file_handle->driver->file_flush( file_handle );
}


int wiced_filesystem_file_end_reached ( wiced_file_t* file_handle )
{
    return file_handle->driver->file_end_reached( file_handle );
}

wiced_result_t wiced_filesystem_dir_open ( wiced_filesystem_t* fs_handle, wiced_dir_t* dir_handle, const char* dir_name )
{
    dir_handle->filesystem = fs_handle;
    dir_handle->driver     = fs_handle->driver;

    if ( ( dir_name == NULL ) || ( strlen(dir_name) <= 0 ) )
    {
        return WICED_BADARG;
    }

    return fs_handle->driver->dir_open( fs_handle, dir_handle, dir_name );
}

wiced_result_t wiced_filesystem_dir_close ( wiced_dir_t* dir_handle )
{
    return dir_handle->driver->dir_close( dir_handle );
}

wiced_result_t wiced_filesystem_dir_read( wiced_dir_t* dir_handle, char* name_buffer, unsigned int name_buffer_length, wiced_dir_entry_type_t* type, wiced_dir_entry_details_t* details )
{
    return dir_handle->driver->dir_read( dir_handle, name_buffer, name_buffer_length, type, details );
}

int wiced_filesystem_dir_end_reached ( wiced_dir_t* dir_handle )
{
    return dir_handle->driver->dir_end_reached( dir_handle );
}

wiced_result_t wiced_filesystem_dir_rewind ( wiced_dir_t* dir_handle )
{
    return dir_handle->driver->dir_rewind( dir_handle );
}

wiced_result_t wiced_filesystem_dir_create( wiced_filesystem_t* fs_handle, const char* directory_name )
{
    return fs_handle->driver->dir_create( fs_handle, directory_name );
}

wiced_result_t wiced_filesystem_dir_delete ( wiced_filesystem_t* fs_handle, const char* directory_name )
{
    return fs_handle->driver->dir_delete( fs_handle, directory_name );
}

wiced_result_t wiced_filesystem_format( wiced_block_device_t* device, wiced_filesystem_handle_type_t fs_type )
{
    wiced_filesystem_driver_t* driver;
    /* These ifdefs ensure that the drivers are only pulled in if they are used */
    switch ( fs_type )
    {
#ifdef USING_WICEDFS
        case WICED_FILESYSTEM_HANDLE_WICEDFS:
            driver = &wiced_filesystem_driver_wicedfs;
            break;
#endif /* ifdef USING_WICEDFS */
#ifdef USING_FATFS
        case WICED_FILESYSTEM_HANDLE_FATFS:
            driver = &wiced_filesystem_driver_fatfs;
            break;
#endif /* ifdef USING_FATFS */
#ifdef USING_FILEX
        case WICED_FILESYSTEM_HANDLE_FILEX:
            driver = &wiced_filesystem_driver_filex;
            break;
#endif /* ifdef USING_FILEX */
#ifdef USING_FILEX_USBX
        case WICED_FILESYSTEM_HANDLE_FILEX_USBX:
            driver = &wiced_filesystem_driver_filex_usbx;
            break;
#endif /* ifdef USING_FILEX_USBX */

#ifndef USING_WICEDFS
        case WICED_FILESYSTEM_HANDLE_WICEDFS:
#endif /* ifdef USING_WICEDFS */
#ifndef USING_FATFS
        case WICED_FILESYSTEM_HANDLE_FATFS:
#endif /* ifdef USING_FATFS */
#ifndef USING_FILEX
        case WICED_FILESYSTEM_HANDLE_FILEX:
#endif /* ifndef USING_FILEX */
#ifndef USING_FILEX_USBX
        case WICED_FILESYSTEM_HANDLE_FILEX_USBX:
#endif /* ifdef USING_FILEX_USBX */
        default:
            return WICED_FILESYSTEM_ERROR;
    }

    return driver->format( device );
}


/******************************************************
 *               Static Function Definitions
 ******************************************************/

#if 0 // WIP


typedef struct
{
    char*                        iso_c_mode;
    wiced_filesystem_open_mode_t wiced_mode;
} iso_c_mode_translation_t;

wiced_filesystem_t* default_iso_c_filesystem;

void wiced_filesystem_set_iso_c_default( wiced_filesystem_t* fs_handle )
{
    default_iso_c_filesystem = fs_handle;
}


static const iso_c_mode_translation_t  iso_c_modes[] =
{
        { "r",   WICED_FILESYSTEM_OPEN_FOR_READ      },
        { "rb",  WICED_FILESYSTEM_OPEN_FOR_READ      },
        { "w",   WICED_FILESYSTEM_OPEN_FOR_WRITE     },
        { "wb",  WICED_FILESYSTEM_OPEN_FOR_WRITE     },
        { "r+",  WICED_FILESYSTEM_OPEN_FOR_WRITE     },
        { "rb+", WICED_FILESYSTEM_OPEN_FOR_WRITE     },
        { "r+b", WICED_FILESYSTEM_OPEN_FOR_WRITE     },
        { "a",   WICED_FILESYSTEM_OPEN_APPEND        },
        { "ab",  WICED_FILESYSTEM_OPEN_APPEND        },
        { "w+",  WICED_FILESYSTEM_OPEN_APPEND        },
        { "wb+", WICED_FILESYSTEM_OPEN_ZERO_LENGTH   },
        { "w+b", WICED_FILESYSTEM_OPEN_ZERO_LENGTH   },
        { "a+",  WICED_FILESYSTEM_OPEN_APPEND_CREATE },
        { "ab+", WICED_FILESYSTEM_OPEN_APPEND_CREATE },
        { "a+b", WICED_FILESYSTEM_OPEN_APPEND_CREATE },
};

static const int iso_c_mode_count = sizeof(iso_c_modes) / sizeof(iso_c_modes[0]);

FILE *fopen( const char *filename, const char *mode )
{
    int i;
    wiced_result_t result;
    wiced_file_t* file_handle

    for ( i = 0; i < iso_c_mode_count; i++ )
    {
        if ( strcmp( mode, iso_c_modes[i].iso_c_mode ) == 0 )
        {
            break;
        }
    }
    if ( i >= iso_c_mode_count )
    {
        wiced_assert( "Unknown mode", 0 == 1 );
        return NULL;
    }

    if ( default_iso_c_filesystem == NULL )
    {
        wiced_assert( "Filesystem not set", 0 == 1 );  /* Use wiced_filesystem_set_iso_c_default */
        return NULL;
    }

    file_handle = (wiced_file_t*) malloc( sizeof(wiced_file_t) );
    if ( handle == NULL )
    {
        wiced_assert( "No memory for file_handle", 0 == 1 );
        return NULL;
    }

    result = wiced_filesystem_file_open ( default_iso_c_filesystem, file_handle, filename, iso_c_modes[i].wiced_mode );

    if ( result != WICED_SUCCESS )
    {
        wiced_assert( "File open failed", 0 == 1 );
        return NULL;
    }

    return (FILE*) file_handle;
}



static inline int feof( FILE* stream )
{
    return wiced_filesystem_file_end_reached( (wiced_file_t*) stream );
}

static inline int fflush( FILE *stream )
{
    return wiced_filesystem_file_flush( (wiced_file_t*) stream );
}

#endif // WIP

