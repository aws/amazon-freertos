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

#pragma once

#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include "wiced_filesystem.h"


#ifdef __cplusplus
extern "C" {
#endif
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

struct wiced_filesystem_driver_struct
{
    wiced_result_t (*init)             ( void );
    wiced_result_t (*mount)            ( wiced_block_device_t* device, wiced_filesystem_t* fs_handle_out );
    wiced_result_t (*unmount)          ( wiced_filesystem_t* fs_handle );
    wiced_result_t (*file_get_details) ( wiced_filesystem_t* fs_handle, const char* filename, wiced_dir_entry_details_t* details_out );
    wiced_result_t (*file_open)        ( wiced_filesystem_t* fs_handle, wiced_file_t* file_handle_out, const char* filename, wiced_filesystem_open_mode_t mode );
    wiced_result_t (*file_seek)        ( wiced_file_t* file_handle, int64_t offset, wiced_filesystem_seek_type_t whence );
    wiced_result_t (*file_tell)        ( wiced_file_t* file_handle, uint64_t* location );
    wiced_result_t (*file_read)        ( wiced_file_t* file_handle, void* data, uint64_t bytes_to_read, uint64_t* returned_bytes_count );
    wiced_result_t (*file_write)       ( wiced_file_t* file_handle, const void* data, uint64_t bytes_to_write, uint64_t* written_bytes_count );
    wiced_result_t (*file_flush)       ( wiced_file_t* file_handle );
    int            (*file_end_reached) ( wiced_file_t* file_handle );
    wiced_result_t (*file_close)       ( wiced_file_t* file_handle );
    wiced_result_t (*file_delete)      ( wiced_filesystem_t* fs_handle, const char* filename );
    wiced_result_t (*dir_open)         ( wiced_filesystem_t* fs_handle, wiced_dir_t* dir_handle, const char* dir_name );
    wiced_result_t (*dir_read)         ( wiced_dir_t* dir_handle, char* name_buffer, unsigned int name_buffer_length, wiced_dir_entry_type_t* type, wiced_dir_entry_details_t* details );
    int            (*dir_end_reached)  ( wiced_dir_t* dir_handle );
    wiced_result_t (*dir_rewind)       ( wiced_dir_t* dir_handle );
    wiced_result_t (*dir_close)        ( wiced_dir_t* dir_handle );
    wiced_result_t (*dir_create)       ( wiced_filesystem_t* fs_handle, const char* directory_name );
    wiced_result_t (*dir_delete)       ( wiced_filesystem_t* fs_handle, const char* directory_name );
    wiced_result_t (*format)           ( wiced_block_device_t* device );
};



/******************************************************
 *                 Static Variables
 ******************************************************/

/******************************************************
 *               Static Function Declarations
 ******************************************************/

/******************************************************
 *               Variable Definitions
 ******************************************************/
extern wiced_filesystem_driver_t wiced_filesystem_driver_wicedfs;
extern wiced_filesystem_driver_t wiced_filesystem_driver_fatfs;
extern wiced_filesystem_driver_t wiced_filesystem_driver_filex;
extern wiced_filesystem_driver_t wiced_filesystem_driver_filex_usbx;

/******************************************************
 *               Function Definitions
 ******************************************************/

#ifdef __cplusplus
} /*extern "C" */
#endif
