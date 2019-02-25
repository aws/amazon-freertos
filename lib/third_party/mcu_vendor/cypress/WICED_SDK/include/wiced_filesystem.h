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
 *  Public API of filesystem functions for WICED
 */

#pragma once

#include <stdio.h>
#include "wiced_result.h"
#include "wiced_time.h"
#include "wiced_block_device.h"

#ifdef USING_WICEDFS
#include "wicedfs.h"
#endif /* USING_WICEDFS */
#ifdef USING_FATFS
#include "ff.h"
#endif /* USING_FATFS */
#if (defined(USING_FILEX) || defined(USING_FILEX_USBX))
#include "wiced_filex.h"
#endif /* USING_FILEX */


#ifdef __cplusplus
extern "C" {
#endif

/******************************************************
 *                     Macros
 ******************************************************/

/******************************************************
 *                    Constants
 ******************************************************/

#define WICED_FILESYSTEM_DIRECTORY_SEPARATOR    '/'
#define WICED_FILESYSTEM_MOUNT_NAME_LENGTH_MAX  32
#define WICED_FILESYSTEM_MOUNT_DEVICE_NUM_MAX   8

/******************************************************
 *                   Enumerations
 ******************************************************/

typedef enum
{
    WICED_FILESYSTEM_OPEN_FOR_READ,      /**< Specifies read access to the object. Data can be read from the file - equivalent to "r" or "rb" */
    WICED_FILESYSTEM_OPEN_FOR_WRITE,     /**< Specifies read/write access to the object. Data can be written to the file - equivalent to "r+" or "rb+" or "r+b" */
    WICED_FILESYSTEM_OPEN_WRITE_CREATE,  /**< Opens for read/write access, creates it if it doesn't exist */
    WICED_FILESYSTEM_OPEN_ZERO_LENGTH,   /**< Opens for read/write access, Truncates file to zero length if it exists, or creates it if it doesn't - equivalent to "w+", "wb+" or "w+b" */
    WICED_FILESYSTEM_OPEN_APPEND,        /**< Opens for read/write access, places the current location at the end of the file ready for appending - equivalent to "a", "ab" */
    WICED_FILESYSTEM_OPEN_APPEND_CREATE, /**< Opens for read/write access, creates it if it doesn't exist, and places the current location at the end of the file ready for appending  - equivalent to "a+", "ab+" or "a+b" */
} wiced_filesystem_open_mode_t;

typedef enum
{
    WICED_FILESYSTEM_HANDLE_WICEDFS,
    WICED_FILESYSTEM_HANDLE_FATFS,
    WICED_FILESYSTEM_HANDLE_FILEX,
    WICED_FILESYSTEM_HANDLE_FILEX_USBX,
} wiced_filesystem_handle_type_t;

typedef enum
{
    WICED_FILESYSTEM_MEDIA_USB_MSD,
    FATFS_HANDLE,
    FILEX_HANDLE,
} wiced_filesystem_physical_media_driver_t;

typedef enum
{
    WICED_FILESYSTEM_SEEK_SET = SEEK_SET,      /**< Offset from start of file */
    WICED_FILESYSTEM_SEEK_CUR = SEEK_CUR,      /**< Offset from current position in file */
    WICED_FILESYSTEM_SEEK_END = SEEK_END,      /**< Offset from end of file */
} wiced_filesystem_seek_type_t;


typedef enum
{
    WICED_FILESYSTEM_ATTRIBUTE_READ_ONLY  = 0x01,
    WICED_FILESYSTEM_ATTRIBUTE_HIDDEN     = 0x02,
    WICED_FILESYSTEM_ATTRIBUTE_SYSTEM     = 0x04,
    WICED_FILESYSTEM_ATTRIBUTE_VOLUME     = 0x08,
    WICED_FILESYSTEM_ATTRIBUTE_DIRECTORY  = 0x10,
    WICED_FILESYSTEM_ATTRIBUTE_ARCHIVE    = 0x20,
} wiced_filesystem_attribute_type_t;


typedef enum
{
    WICED_FILESYSTEM_FILE,
    WICED_FILESYSTEM_DIR,
    WICED_FILESYSTEM_LINK,
} wiced_dir_entry_type_t;

/******************************************************
 *                 Type Definitions
 ******************************************************/

typedef struct wiced_filesystem_driver_struct wiced_filesystem_driver_t;

/**
 * File-system Handle Structure
 */
typedef struct wiced_filesystem_struct wiced_filesystem_t;



/**
 * File Handle Structure
 *
 * Equivalent of ISO-C type FILE
 */
typedef struct wiced_file_struct wiced_file_t;



/**
 * Directory handle structure
 *
 * Equivalent of ISO-C type DIR
 */

typedef struct wiced_dir_struct wiced_dir_t;

/******************************************************
 *                    Structures
 ******************************************************/




/**
 * File Information Structure
 *
 * Equivalent of ISO-C struct dirent
 */
typedef struct
{
    uint64_t                           size;

    wiced_bool_t                       attributes_available;
    wiced_bool_t                       date_time_available;
    wiced_bool_t                       permissions_available;
    wiced_filesystem_attribute_type_t  attributes;    /* Attribute */
    wiced_utc_time_t                   date_time;     /* Last modified date & time */
//    uint32_t                           permissions;  /* Not supported yet */
} wiced_dir_entry_details_t;


/**
 * A list element for user interactive selection of filesystem devices
 */
typedef struct
{
        wiced_block_device_t*          device;
        wiced_filesystem_handle_type_t type;
        char*                          name;
} filesystem_list_t;

/**
 * A mounted filesystem handle entry
 */
typedef struct wiced_filesystem_mounted_device_struct
{
        wiced_filesystem_t*            fs_handle;
        char                           name[WICED_FILESYSTEM_MOUNT_NAME_LENGTH_MAX];
} wiced_filesystem_mounted_device_t;

/******************************************************
 *                 Global Variables
 ******************************************************/

/**
 * A List of all filesystem devices available on the platform - for interactive user selection (e.g. console app)
 * Terminated by an element where the device pointer is NULL
 */
extern const filesystem_list_t   all_filesystem_devices[];

/******************************************************
 *               Function Declarations
 ******************************************************/
/**
 * NOTE: The idea of a present/current working directory (pwd/cwd) has been intentionally omitted.
 *       These inherently require per-thread storage which unnecessarily complicates things
 */

/**
 * Initialise the filesystem module
 *
 * Initialises the filesystem module before mounting a physical device.
 *
 * @return WICED_SUCCESS on success
 */
wiced_result_t wiced_filesystem_init ( void );


/**
 * Mount the physical device
 *
 * This assumes that the device is ready to read/write immediately.
 *
 * @param[in]  device        - Physical media to init.
 * @param[in]  fs_type       - Type of filesystem to be mounted.
 * @param[out] fs_handle_out - Receives the filesystem handle.
 * @param[in]  mounted_name  - Name of the mounted filesystem.
 *
 * @return WICED_SUCCESS on success
 */
wiced_result_t wiced_filesystem_mount ( wiced_block_device_t* device, wiced_filesystem_handle_type_t fs_type, wiced_filesystem_t* fs_handle_out, const char* mounted_name);


/**
 * Unmount the filesystem
 *
 * @param[in]  fs_handle   - The filesystem to unmount
 *
 * @return WICED_SUCCESS on success
 */
wiced_result_t wiced_filesystem_unmount ( wiced_filesystem_t* fs_handle );


/**
 * Get the filesystem handle by name
 *
 * @param[in]  mounted_name   - The mounted name for search corresponding fs_handle
 *
 * @return fs_handle on success, NULL on failure
 */
wiced_filesystem_t* wiced_filesystem_retrieve_mounted_fs_handle ( const char* mounted_name );


/**
 * Gets the size/timestamp/attribute details of a file
 *
 * @param[in]  fs_handle      - The filesystem handle to use - obtained from wiced_filesystem_mount
 * @param[in]  filename       - The filename of the file to examine
 * @param[out] details_out    - Receives the details of the file
 *
 * @return WICED_SUCCESS on success
 */
wiced_result_t wiced_filesystem_file_get_details ( wiced_filesystem_t* fs_handle, const char* filename, wiced_dir_entry_details_t* details_out );


/**
 * Open a file for reading or writing
 *
 * @param[in]  fs_handle       - The filesystem handle to use - obtained from wiced_filesystem_mount
 * @param[out] file_handle_out - a pointer to a wiced_file_t structure which will receive the
 *                               file handle after it is opened
 * @param[in]  filename        - The filename of the file to open
 * @param[in]  mode            - Specifies read or write access
 *
 * @return WICED_SUCCESS on success
 */
wiced_result_t wiced_filesystem_file_open ( wiced_filesystem_t* fs_handle, wiced_file_t* file_handle_out, const char* filename, wiced_filesystem_open_mode_t mode );

/**
 * Seek to a location within a file
 *
 * @note This is similar to the fseek() in ISO C.
 *
 * @param[in] file_handle - The file handle on which to perform the seek.
 *                          Must have been previously opened with wiced_filesystem_fopen.
 * @param[in] offset      - The offset in bytes
 * @param[in] whence      - WICED_FILESYSTEM_SEEK_SET = Offset from start of file
 *                          WICED_FILESYSTEM_SEEK_CUR = Offset from current position in file
 *                          WICED_FILESYSTEM_SEEK_END = Offset from end of file
 *
 * @return WICED_SUCCESS  on success
 */
wiced_result_t wiced_filesystem_file_seek ( wiced_file_t* file_handle, int64_t offset, wiced_filesystem_seek_type_t whence );


/**
 * Returns the current location within a file
 *
 * @note This is similar to the ftell() in ISO C.
 *
 * @param[in]  file_handle - The file handle to be examined
 * @param[out] location    - Receives the current location within the file
 *
 * @return WICED_SUCCESS  on success
 */
wiced_result_t wiced_filesystem_file_tell ( wiced_file_t* file_handle, uint64_t* location );


/**
 * Reads data from a file into a memory buffer
 *
 * @param[in] file_handle           - The file handle to read from
 * @param[out] data                 - A pointer to the memory buffer that will
 *                                    receive the data that is read
 * @param[in] bytes_to_read         - The number of bytes to read
 * @param[out] returned_bytes_count - The number of bytes successfully read.
 *
 * @return WICED_SUCCESS  on success
 */
wiced_result_t wiced_filesystem_file_read ( wiced_file_t* file_handle, void* data, uint64_t bytes_to_read, uint64_t* returned_bytes_count );


/**
 * Writes data to a file from a memory buffer
 *
 * @param[in] file_handle          - The file handle to write to
 * @param[in] data                 - A pointer to the memory buffer that contains
 *                                   the data that is to be written
 * @param[in] bytes_to_write       - The number of bytes to write
 * @param[out] written_bytes_count - Receives the number of bytes successfully written.
 *
 * @return WICED_SUCCESS  on success
 */
wiced_result_t wiced_filesystem_file_write( wiced_file_t* file_handle, const void* data, uint64_t bytes_to_write, uint64_t* written_bytes_count );


/**
 * Flush write data to media
 *
 * @note This is similar to the fflush() in ISO C.
 *
 * @param[in] file_handle - The file handle to flush
 *
 * @return WICED_SUCCESS  on success
 */
wiced_result_t wiced_filesystem_file_flush ( wiced_file_t* file_handle );


/**
 * Check the end-of-file flag for a file
 *
 * @note This is similar to the feof() in ISO C.
 *
 * @param[in] file_handle - The file handle to check for EOF
 *
 * @return 1 = EOF or invalid file handle
 */
int wiced_filesystem_file_end_reached ( wiced_file_t* file_handle );


/**
 * Close a file
 *
 * @note This is similar to the fclose() in ISO C.
 *
 * @param[in] file_handle - The file handle to close
 *
 * @return WICED_SUCCESS = success
 */
wiced_result_t wiced_filesystem_file_close ( wiced_file_t* file_handle );


/**
 * Delete a file
 *
 * @note This is similar to the remove() in ISO C.
 *
 * @param[in]  fs_handle      - The filesystem handle to use - obtained from wiced_filesystem_mount
 * @param[in]  filename       - The filename of the file to delete
 *
 * @return WICED_SUCCESS on success
 */
wiced_result_t wiced_filesystem_file_delete ( wiced_filesystem_t* fs_handle, const char* filename );


/**
 * Opens a directory
 *
 * @note This is similar to the opendir() in ISO C.
 *
 * @param[in]  fsp      - The filesystem handle to use - obtained from wiced_filesystem_mount
 * @param[out] dirp     - A pointer to a directory structure which
 *                        will be filled with the opened handle
 * @param[in]  dir_name - The path of the directory to open
 *
 * @return WICED_SUCCESS on success
 */
wiced_result_t wiced_filesystem_dir_open ( wiced_filesystem_t* fs_handle, wiced_dir_t* dir_handle, const char* dir_name );


/**
 * Reads a directory entry
 *
 * @note This is similar to the readdir() in ISO C.
 *
 * @param[in]  dir_handle         - The directory handle to read from
 * @param[out] name_buffer        - Pointer to a buffer that will receive the filename
 * @param[in]  name_buffer_length - The maximum number of bytes that can be put in the buffer
 * @param[out] type               - Pointer to variable that will receive entry type (file or dir)
 * @param[out] details            - Pointer to variable that will receive entry information (attribute, size, modified date/time)
 *
 * @return WICED_SUCCESS on success
 */
wiced_result_t wiced_filesystem_dir_read( wiced_dir_t* dir_handle, char* name_buffer, unsigned int name_buffer_length, wiced_dir_entry_type_t* type, wiced_dir_entry_details_t* details );


/**
 * Check the end-of-directory flag for a directory
 *
 * Checks whether the selected directory handle is
 * at the end of the available directory entries.
 *
 * @param[in] dir_handle - The directory handle to check
 *
 * @return 1 = End-of-Directory
 */
int wiced_filesystem_dir_end_reached ( wiced_dir_t* dir_handle );


/**
 * Returns a directory handle to the first entry
 *
 * @note This is similar to the rewinddir() in ISO C.
 *
 * @param[in] dir_handle - The directory handle to rewind
 *
 * @return WICED_SUCCESS = Success
 */
wiced_result_t wiced_filesystem_dir_rewind ( wiced_dir_t* dir_handle );


/**
 * Closes a directory handle
 *
 * @param[in] dir_handle - The directory handle to close
 *
 * @return WICED_SUCCESS = Success
 */
wiced_result_t wiced_filesystem_dir_close ( wiced_dir_t* dir_handle );


/**
 * Create a directory
 *
 * @param[in]  fs_handle       - The filesystem handle to use
 * @param[in]  directory_name  - The path of the directory to create
 *
 * @return WICED_SUCCESS on success
 */
wiced_result_t wiced_filesystem_dir_create( wiced_filesystem_t* fs_handle, const char* directory_name );

/**
 * Delete a directory
 *
 * @note This is similar to the remove() in ISO C.
 *
 * @param[in]  fs_handle      - The filesystem handle to use - obtained from wiced_filesystem_mount
 * @param[in]  directory_name       - The path of the directory to delete
 *
 * @return WICED_SUCCESS on success
 */
wiced_result_t wiced_filesystem_dir_delete ( wiced_filesystem_t* fs_handle, const char* directory_name );

/**
 * Formats the media
 *
 * Creates a new, blank filesystem
 *
 * @param[in]  device   - The block device to format
 * @param[in]  fs_type  - Which type of filesystem to create
 *
 * @return WICED_SUCCESS on success
 */
wiced_result_t wiced_filesystem_format( wiced_block_device_t* device, wiced_filesystem_handle_type_t fs_type );


/******************************************************
 *  Opaque types - do not use directly
 ******************************************************/

struct wiced_dir_struct
{
    wiced_filesystem_driver_t* driver;
    wiced_filesystem_t*        filesystem;

    union
    {
#ifdef USING_WICEDFS
        wicedfs_dir_t wicedfs;
#endif /* USING_WICEDFS */
#ifdef USING_FATFS
        struct
        {
#ifdef FATFS_VER011
            FATFS_DIR    handle;
#else
            DIR    handle;
#endif
            wiced_bool_t eodir;
        } fatfs;
#endif /* USING_FATFS */
#if (defined(USING_FILEX) || defined(USING_FILEX_USBX))
        struct
        {
            wiced_bool_t eodir;
            FX_PATH      path;
        } filex;
#endif /* USING_FILEX */
    } data;
};

struct wiced_file_struct
{
    wiced_filesystem_driver_t* driver;
    wiced_filesystem_t*        filesystem;
    union
    {
#ifdef USING_WICEDFS
        wicedfs_file_t wicedfs;
#endif /* USING_WICEDFS */
#ifdef USING_FATFS
        FIL fatfs;
#endif /* USING_FATFS */
#if (defined(USING_FILEX) || defined(USING_FILEX_USBX))
        FX_FILE filex;
#endif /* USING_FILEX */
    } data;
};

struct wiced_filesystem_struct
{
    wiced_filesystem_driver_t* driver;
    wiced_block_device_t*      device;
    union
    {
#ifdef USING_WICEDFS
        wicedfs_filesystem_t wicedfs;
#endif /* USING_WICEDFS */
#ifdef USING_FATFS
        struct
        {
            FATFS       handle;
            const TCHAR drive_id[5];
        } fatfs;
#endif /* USING_FATFS */
#ifdef USING_FILEX
        struct
        {
            FX_MEDIA handle;
            char     buffer[FILEX_MEDIA_BUFFER_SIZE_PADDED];
        } filex;
#endif /* USING_FILEX */
#ifdef USING_FILEX_USBX
        struct
        {
            FX_MEDIA* handle;
        } filex_usbx;
#endif /* USING_FILEX_USBX */
    } data;
};



#ifdef __cplusplus
} /*extern "C" */
#endif
