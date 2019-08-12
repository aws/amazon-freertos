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
 *  A tool to extract files/directories from a WicedFS filesystem image file
 */

#include <stdio.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <errno.h>
#include <string.h>
#include <dirent.h>
#include "wicedfs.h"

/******************************************************
 *                      Macros
 ******************************************************/
#ifndef _WIN32
/*define_style_exception_start*/
#define off64_t __off64_t
#define _stati64 stat64
#define compat_mkdir( name )  mkdir( (name), S_IRWXU )
#else
#define compat_mkdir( name )  mkdir( (name) )
/*define_style_exception_end*/
#endif /* ifndef _WIN32 */


/******************************************************
 *                    Constants
 ******************************************************/
#define FILE_READ_BUFFER_SIZE   (10*1024)
#define FILENAME_BUFFER_SIZE    (10240)
#define DIRECTORY_SEPARATOR_STR "/"
#define ROOT_DIRECTORY          "/"

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

static wicedfs_usize_t hostfile_wicedfs_read( void* user_param, void* buf, wicedfs_usize_t size, wicedfs_usize_t pos );
static int extract_directory( const char* dir_name, const char* root_output_directory_name, const wicedfs_filesystem_t* fs_handle );

/******************************************************
 *               Variable Definitions
 ******************************************************/

/******************************************************
 *               Function Definitions
 ******************************************************/

int main (int argc, const char * argv[])
{
    const char*          input_image_filename;
    const char*          root_output_directory_name;
    FILE*                input_handle;
    wicedfs_filesystem_t fs_handle;
    int                  result;


    /* Check number of arguments */
    if ( argc != 3 )
    {
        printf( "Usage: extract_wicedfs <wicedfs_image.bin> <output_dir>\n\n" );
        return -1;
    }

    /* Open WicedFS image file for input */
    input_image_filename = argv[1];

    input_handle = fopen( input_image_filename, "rb" );

    if( input_handle == NULL )
    {
        printf( "Couldn't open input image file %s\n", input_image_filename );
        return -1;
    }

    /* Initialise the WicedFS filesytem */
    wicedfs_init( 0, hostfile_wicedfs_read, &fs_handle, input_handle );

    /* Recursively extract the files from the root directory of the image into the requested output directory */
    root_output_directory_name = argv[2];

    result = extract_directory( ROOT_DIRECTORY, root_output_directory_name, &fs_handle );

    fclose( input_handle );

    return result;
}


/******************************************************
 *               Static Function Definitions
 ******************************************************/

/** Recursively extracts a wicedfs directory to a local PC filesystem
 *
 * @param[in] dir_name                   : directory path within wicedfs image which will be extracted
 * @param[in] root_output_directory_name : directory on the local PC where the root directory
 *                                         of the wicedfs image will be located
 * @param[in] fs_handle                  : Handle to use for accessing the wicedfs filesystem.
 *
 * @return 0 on success
 */
static int extract_directory( const char* dir_name, const char* root_output_directory_name, const wicedfs_filesystem_t* fs_handle )
{
    wicedfs_dir_t cur_dir_handle;
    int           result;
    char*         curr_output_directory_name;
    char*         last_char;

    /* Make a string with the root output directory name in it */
    curr_output_directory_name = (char*) malloc( strlen(root_output_directory_name) + strlen(dir_name) + 2 );
    if ( curr_output_directory_name == NULL )
    {
        printf( "Error allocating curr output directory buffer\n");
        free( curr_output_directory_name );
        return -4;
    }
    strcpy( curr_output_directory_name, root_output_directory_name );

    /* If directory name ends with a slash, strip it*/
    last_char = &curr_output_directory_name[ strlen(curr_output_directory_name)-1 ];
    if ( ( *last_char == '/' ) || ( *last_char == '\\' ) )
    {
        *last_char = '\x00';
    }
    strcat( curr_output_directory_name, dir_name );

    /* If directory name ends with a slash, strip it*/
    last_char = &curr_output_directory_name[ strlen(curr_output_directory_name)-1 ];
    if ( ( *last_char == '/' ) || ( *last_char == '\\' ) )
    {
        *last_char = '\x00';
    }


    /* Open the directory in the WicedFS image */
    result = wicedfs_opendir( fs_handle, &cur_dir_handle, dir_name );
    if ( result != 0 )
    {
        printf( "Error opening directory %s\n", dir_name );
        free( curr_output_directory_name );
        return -5;
    }

    /* Create a local PC directory to place the extracted files into. */
    result = compat_mkdir( curr_output_directory_name );
    if ( result != 0 )
    {
        /* Error creating directory */
        if ( errno == EEXIST )
        {
            /* Error was due to pre-exising file/directory with same name
             * Check if it is a directory
             */
            struct stat statbuf;
            result = stat( curr_output_directory_name, &statbuf );
            if ( ( result != 0 ) || ( !( S_ISDIR( statbuf.st_mode ) ) ) )
            {
                printf( "Location \"%s\" exists and is not a directory\n", curr_output_directory_name );
                free( curr_output_directory_name );
                return -6;
            }
        }
        else
        {
            /* Some other error */
            printf( "Error creating directory %s\n", curr_output_directory_name );
            free( curr_output_directory_name );
            return -7;
        }
    }


    /* Loop through directory entries in the WicedFS image until none left */
    while ( ! wicedfs_eodir( &cur_dir_handle ) )
    {
        char                 curr_filename[FILENAME_BUFFER_SIZE];
        char*                curr_input_file_path;
        wicedfs_entry_type_t entry_type;

        /* Read the directory entry name and type */
        result = wicedfs_readdir( &cur_dir_handle, curr_filename, sizeof(curr_filename), &entry_type );
        if ( result != 0 )
        {
            printf( "Error reading directory %s\n", dir_name );
            free( curr_output_directory_name );
            return -8;
        }

        /* Make a full path for input file */
        curr_input_file_path = malloc( strlen(dir_name) + strlen(curr_filename) + 2 );
        if ( curr_input_file_path == NULL )
        {
            printf( "Error allocating current input file path buffer\n");
            free( curr_output_directory_name );
            return -9;
        }
        strcpy( curr_input_file_path, dir_name );
        /* If directory name ends without a slash, add one */
        last_char = &curr_input_file_path[ strlen(curr_input_file_path)-1 ];
        if ( ( *last_char != '/' ) && ( *last_char != '\\' ) )
        {
            strcat( curr_input_file_path, DIRECTORY_SEPARATOR_STR );
        }
        strcat( curr_input_file_path, curr_filename );

        /* Check if directory entry was a file or directory */
        if ( entry_type == WICEDFS_FILE )
        {
            /* Processing a file. */
            char            transfer_buffer[FILE_READ_BUFFER_SIZE];
            FILE*           output_handle;
            wicedfs_file_t  input_handle;
            wicedfs_usize_t file_pos;
            wicedfs_ssize_t file_size;
            char*           curr_output_file_path;

            /* Make a full path for output file */
            curr_output_file_path = malloc( strlen(curr_output_directory_name) + strlen(curr_filename) + 2 );
            if ( curr_output_file_path == NULL )
            {
                printf( "Error allocating current output file path buffer\n");
                free( curr_input_file_path );
                free( curr_output_directory_name );
                return -10;
            }
            strcpy( curr_output_file_path, curr_output_directory_name );
            /* If directory name ends without a slash, add one */
            last_char = &curr_output_directory_name[ strlen(curr_output_directory_name)-1 ];
            if ( ( *last_char != '/' ) && ( *last_char != '\\' ) )
            {
                strcat( curr_output_file_path, DIRECTORY_SEPARATOR_STR );
            }
            strcat( curr_output_file_path, curr_filename );

            /* Open the file within the WicedFS image */
            result = wicedfs_fopen( fs_handle, &input_handle, curr_input_file_path );
            if ( result != 0 )
            {
                printf( "Error opening WicedFS file %s\n", curr_input_file_path );
                free( curr_output_file_path );
                free( curr_input_file_path );
                free( curr_output_directory_name );
                return -11;
            }

            /* Get the size of the file in the WicedFS image */
            file_size = wicedfs_filesize( fs_handle, curr_input_file_path );
            if ( file_size < 0 )
            {
                printf( "Error getting file size for %s\n", curr_input_file_path );
                wicedfs_fclose( &input_handle );
                free( curr_output_file_path );
                free( curr_input_file_path );
                free( curr_output_directory_name );
                return -13;
            }

            /* Open the output file for writing */
            output_handle = fopen( curr_output_file_path, "w+b" );
            if( output_handle == NULL )
            {
                printf( "Couldn't open output file %s\n", curr_output_file_path );
                wicedfs_fclose( &input_handle );
                free( curr_output_file_path );
                free( curr_input_file_path );
                free( curr_output_directory_name );
                return -12;
            }


            /* Loop through chunks of the the file */
            for( file_pos = 0; file_pos < (wicedfs_usize_t)file_size; file_pos = (wicedfs_usize_t) ( file_pos + sizeof(transfer_buffer) ) )
            {
                wicedfs_usize_t read_size;
                wicedfs_usize_t num_read;
                size_t num_written;

                /* Check how much data is left in file */
                if ( sizeof(transfer_buffer) > (wicedfs_usize_t)file_size - file_pos )
                {
                    /* Less than one buffer-full left in file - fetch what is left */
                    read_size = (wicedfs_usize_t)file_size - file_pos;
                }
                else
                {
                    /* More than one buffer-full left in file - fetch a full buffer */
                    read_size = (wicedfs_usize_t) sizeof(transfer_buffer);
                }

                /* Read a data chunk from the WicedFS file */
                num_read = wicedfs_fread( transfer_buffer, 1, read_size, &input_handle );
                if ( num_read != read_size )
                {
                    printf( "Error reading WicedFS file %s\n", curr_input_file_path );
                    fclose( output_handle );
                    wicedfs_fclose( &input_handle );
                    free( curr_output_file_path );
                    free( curr_input_file_path );
                    free( curr_output_directory_name );
                    return -14;
                }

                /* Write the chunk to the local PC filesystem file */
                num_written = fwrite( transfer_buffer, 1, (size_t)read_size, output_handle );
                if ( num_written != read_size )
                {
                    printf( "Error writing output file %s\n", curr_output_file_path );
                    fclose( output_handle );
                    wicedfs_fclose( &input_handle );
                    free( curr_output_file_path );
                    free( curr_input_file_path );
                    free( curr_output_directory_name );
                    return -15;
                }
            }

            /* Close local PC output file */
            result = fclose( output_handle );
            if ( result != 0 )
            {
                printf( "Error closing fcurr_output_directory_nameile %s\n", curr_output_file_path );
                wicedfs_fclose( &input_handle );
                free( curr_output_file_path );
                free( curr_input_file_path );
                free( curr_output_directory_name );
                return -16;
            }

            /* Close WicedFS file */
            result = wicedfs_fclose( &input_handle );
            if ( result != 0 )
            {
                printf( "Error closing input file %s\n", curr_input_file_path );
                fclose( output_handle );
                wicedfs_fclose( &input_handle );
                free( curr_output_file_path );
                free( curr_input_file_path );
                free( curr_output_directory_name );
                return -16;
            }

            free( curr_output_file_path );

        }
        else if ( entry_type == WICEDFS_DIR )
        {
            /* Entry is a subdirectory - Recurse into it */
            result = extract_directory( curr_input_file_path, root_output_directory_name, fs_handle );
            if ( result != 0 )
            {
                free( curr_input_file_path );
                free( curr_output_directory_name );
                return result;
            }
        }

        free( curr_input_file_path );

    }
    free( curr_output_directory_name );

    return 0;
}




/** Read function provided to WicedFS
 *
 *  This reads data from the hardware device (a local host file)
 *
 * @param[in]  user_param - The user_param value passed when wicedfs_init was called (use for hardware device handles)
 * @param[out] buf        - The buffer to be filled with data
 * @param[in]  size       - The number of bytes to read
 * @param[in]  pos        - Absolute hardware device location to read from
 *
 * @return Number of bytes successfully read  (i.e. 0 on failure)
 */
static wicedfs_usize_t hostfile_wicedfs_read( void* user_param, void* buf, wicedfs_usize_t size, wicedfs_usize_t pos )
{
    /* Seek to the requested hardware location in the image file */
    if ( 0 != fseeko64( (FILE*)user_param, (off64_t)(pos), SEEK_SET) )
    {
        return 0;
    }

    /* Read the requested data from the image file */
    return (wicedfs_usize_t) fread( buf, 1, (size_t)size, (FILE*)user_param );
}
