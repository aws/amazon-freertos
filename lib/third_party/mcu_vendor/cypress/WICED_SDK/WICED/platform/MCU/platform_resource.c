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
 * Common resource implementation
 */

#include <stdio.h>
#include <string.h>
#include "wicedfs.h"
#include "wiced_resource.h"
#include "platform_config.h"
#include "platform_resource.h"
#include "platform_toolchain.h"

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

resource_result_t resource_read ( const resource_hnd_t* resource, uint32_t offset, uint32_t maxsize, uint32_t* size, void* buffer )
{
    if ( offset > resource->size )
    {
        return RESOURCE_OFFSET_TOO_BIG;
    }

    *size = MIN( maxsize, resource->size - offset );

    if (resource->location == RESOURCE_IN_MEMORY)
    {
        memcpy(buffer, &resource->val.mem.data[offset], *size);
    }
#ifdef USES_RESOURCES_IN_EXTERNAL_STORAGE
    else if ( resource->location == RESOURCE_IN_EXTERNAL_STORAGE )
    {
        return platform_read_external_resource( resource, offset, maxsize, size, buffer );
    }
#endif
#ifdef USES_RESOURCE_GENERIC_FILESYSTEM
    else
    {
        wiced_file_t file_handle;
        uint64_t size64;
        uint64_t maxsize64 =  maxsize;
        if (WICED_SUCCESS != wiced_filesystem_file_open ( &resource_fs_handle, &file_handle, resource->val.fs.filename, WICED_FILESYSTEM_OPEN_FOR_READ ) )
        {
            return RESOURCE_FILE_OPEN_FAIL;
        }
        if ( WICED_SUCCESS != wiced_filesystem_file_seek ( &file_handle, (offset + resource->val.fs.offset), SEEK_SET) )
        {
            return RESOURCE_FILE_SEEK_FAIL;
        }
        if ( WICED_SUCCESS != wiced_filesystem_file_read ( &file_handle, buffer, maxsize64, &size64) )
        {
            wiced_filesystem_file_close ( &file_handle );
            return RESOURCE_FILE_READ_FAIL;
        }
        *size = (uint32_t)size64;
        wiced_filesystem_file_close ( &file_handle );
    }
#else
#ifdef USES_RESOURCE_FILESYSTEM
    else
    {
        wicedfs_file_t file_hnd;

        if ( 0 != wicedfs_fopen( &resource_fs_handle, &file_hnd, resource->val.fs.filename ) )
        {
            return RESOURCE_FILE_OPEN_FAIL;
        }

        if ( 0 != wicedfs_fseek( &file_hnd, (long)(offset + resource->val.fs.offset), SEEK_SET ) )
        {
            wicedfs_fclose( &file_hnd );
            return RESOURCE_FILE_SEEK_FAIL;
        }

        if ( *size != wicedfs_fread( buffer, 1, *size, &file_hnd ) )
        {
            wicedfs_fclose( &file_hnd );
            return RESOURCE_FILE_READ_FAIL;
        }

        wicedfs_fclose( &file_hnd );
    }
#endif /* ifdef USES_RESOURCE_FILESYSTEM */
#endif /* USES_RESOURCE_GENERIC_FILESYSTEM */
    return RESOURCE_SUCCESS;
}

resource_result_t resource_get_readonly_buffer ( const resource_hnd_t* resource, uint32_t offset, uint32_t maxsize, uint32_t* size_out, const void** buffer )
{
    UNUSED_PARAMETER( maxsize );

    if ( offset > resource->size )
    {
        return RESOURCE_OFFSET_TOO_BIG;
    }

    if ( resource->location == RESOURCE_IN_MEMORY )
    {
        *buffer   = &resource->val.mem.data[offset];
        *size_out = MIN(maxsize, resource->size - offset);
        return RESOURCE_SUCCESS;
    }
#if defined (USES_RESOURCE_FILESYSTEM ) || defined (USES_RESOURCE_GENERIC_FILESYSTEM)
    else
    {
        resource_result_t result;
        uint32_t size_returned = 0;
        *size_out = MIN(maxsize, resource->size - offset);
        *buffer = malloc( *size_out );
        result = resource_read( resource, offset, *size_out, &size_returned, (void*) *buffer );
        *size_out = MIN( *size_out, size_returned );

        if ( result != RESOURCE_SUCCESS )
        {
            free( (void*) *buffer );
            return result;
        }
        return RESOURCE_SUCCESS;
    }
#endif /* ifdef USES_RESOURCE_FILESYSTEM */
    return RESOURCE_UNSUPPORTED;
}

resource_result_t resource_free_readonly_buffer( const resource_hnd_t* resource, const void* buffer )
{
    if ( resource->location != RESOURCE_IN_MEMORY )
    {
        free( (void*) buffer );
    }
    return RESOURCE_SUCCESS;
}

