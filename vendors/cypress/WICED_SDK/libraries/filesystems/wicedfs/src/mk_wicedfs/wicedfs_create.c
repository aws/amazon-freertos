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
 *  Implementation for WicedFS filesystem creation code
 */

#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <dirent.h>
#include <inttypes.h>
#include <sys/stat.h>
#include "wicedfs.h"
#include "wicedfs_create.h"

#define INSIDE_WICEDFS_C_
#include "wicedfs_internal.h"

/******************************************************
 *                      Macros
 ******************************************************/
#ifndef _WIN32
/*define_style_exception_start*/
#define off64_t __off64_t
#define _stati64 stat64
/*define_style_exception_end*/
#endif /* ifndef _WIN32 */

/******************************************************
 *                    Constants
 ******************************************************/
#define WICEDFS_CREATE_BUFFER_SIZE (1024*1024)

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
static int     get_sorted_filelist ( const char* dir_name, char** list, unsigned long* item_len, unsigned long* item_count );
static int64_t fsize               ( const char* filename );
static char    isdir               ( const char* filename );
static int     write_dir           ( FILE* output_handle, const char* dir_name, uint64_t* bytes_written );

/******************************************************
 *               Variable Definitions
 ******************************************************/
static char buffer[WICEDFS_CREATE_BUFFER_SIZE];

/******************************************************
 *               Function Definitions
 ******************************************************/

int create_wicedfs( const char* out_file, const char* dir_name )
{
    FILE*                       output_handle;
    wicedfs_filesystem_header_t fs_header;
    uint64_t                    bytes_written;
    int                         retval;

    /* Open the WicedFS image file for writing */
    output_handle = fopen( out_file, "w+b" );

    if( output_handle == NULL )
    {
        printf( "Couldn't open output file %s\n", out_file );
        return -2;
    }

    /* Write the WicedFS overall filesystem header */
    fs_header.magic_number    = WICEDFS_MAGIC;
    fs_header.version         = 1;
    fs_header.root_dir_offset = sizeof(fs_header);

    fwrite( &fs_header, sizeof(fs_header), 1, output_handle );

    /* Recursively write the supplied directory path into the WicedFS image */
    retval = write_dir( output_handle, dir_name, &bytes_written );
    if ( retval != 0 )
    {
        return retval;
    }

    /* Close the WicedFS image file */
    retval = fclose( output_handle );
    if ( retval != 0 )
    {
        printf( "Error closing output\n" );
        return retval;
    }

    return 0;
}




/******************************************************
 *               Static Function Definitions
 ******************************************************/


/**
 * Write the specified PC directory into the supplied WicedFS image file
 *
 * Recursively reads the specified host PC directory and writes the files
 * and subdirectories into the supplied WicedFS file handle.
 *
 * @param[in] output_handle : File handle of the WicedFS image
 * @param[in] dir_name      : Directory path on Host PC
 * @param[out] bytes_written : Receives the number of bytes written to the image file
 *
 * @return 0 = success
 */
static int write_dir( FILE* output_handle, const char* dir_name, uint64_t* bytes_written )
{
    unsigned long        item_len;
    unsigned long        item_count;
    char*                list;
    wicedfs_dir_header_t dir_header;
    uint64_t             table_start;
    unsigned long        file_table_size;
    unsigned long        i;
    uint64_t             curr_file_data_location;

    /* Get a sorted list of the contents of the given directory pathname */
    get_sorted_filelist( dir_name, &list, &item_len, &item_count );

    /* Write the directory header for this directory to the WicedFS image file */
    dir_header.file_table_offset = sizeof(wicedfs_dir_header_t);
    dir_header.num_files         = (wicedfs_usize_t) item_count;
    dir_header.file_header_size  = (uint32_t) (sizeof(wicedfs_file_header_t) + item_len);
    dir_header.filename_size     = (uint32_t) item_len;

    fwrite( &dir_header, sizeof(dir_header), 1, output_handle );

    *bytes_written = sizeof(dir_header);


    /* Retrive current image file location which will be the start of the file table */
    table_start = (uint64_t) ftello64( output_handle );

    /* Calculate the size of the file table */
    file_table_size = item_count * dir_header.file_header_size;

    /* Calculate where the file contents will be written (after the end of the file table */
    curr_file_data_location = table_start + file_table_size;


    /* Loop for each item in the supplied directory path */
    for ( i = 0; i < item_count; i++ )
    {
        wicedfs_file_header_t* file_header;
        char                   path_is_dir;
        char *                 full_path;
        uint64_t               entry_pos;

        /* Allocate space for file table entry header (including the variable size filename field) */
        file_header = (wicedfs_file_header_t*) malloc(  dir_header.file_header_size );

        /* Calculate the location in the image of this file table entry header */
        entry_pos = table_start + i * dir_header.file_header_size;

        /* Copy the name of the item into the filename part of the file table entry */
        strcpy( (char*)file_header + sizeof(wicedfs_file_header_t), &list[item_len*i] );

        /* Allocate space for a string containing the full path on the host PC to this item */
        full_path = (char*) malloc( strlen(dir_name) + strlen( &list[item_len*i] ) + 2 );

        /* Make a string of the full path on the host PC to this item */
        strcpy( full_path, dir_name );
        strcat( full_path, "/" );
        strcat( full_path, &list[item_len*i] );

        /* Check whether the item is a directory */
        path_is_dir = isdir( full_path );

        if ( path_is_dir )
        {
            /* Path is a directory - Set properties appropriately */
            file_header->size                   = 0;
            file_header->type_flags_permissions = WICEDFS_TYPE_DIR;
        }
        else
        {
            /* Path is a file */
            int64_t filesize;

            /* Get size of the file on the Host PC */
            filesize = fsize( full_path );

            /* Check if the file will fit for 32bit WicedFS */
            if ( ( sizeof(wicedfs_usize_t) == 4 ) &&
                 ( filesize > 0x7FFFFFFF ) )
            {
                printf( "Error: File %s is too big for 32 bit version. Please enable WicedFS 64 bit support.\n", full_path);
                free( file_header );
                free( full_path );
                return -5;
            }

            /* Set file table entry properties */
            file_header->size                   = (wicedfs_usize_t) filesize;
            file_header->type_flags_permissions = WICEDFS_TYPE_FILE;
        }

        /* Calculate the offset of the file content data from this file table entry */
        file_header->offset = (wicedfs_ssize_t) ( curr_file_data_location - entry_pos );

        /* Seek to the file location of the file table entry */
        fseeko64( output_handle, (off64_t) entry_pos, SEEK_SET );

        /* Write the file table entry */
        fwrite( file_header, dir_header.file_header_size, 1, output_handle );

        *bytes_written +=  dir_header.file_header_size;

        /* Check whether the current file content data will overflow a 32bit WicedFS */
        if ( ( sizeof(wicedfs_usize_t) == 4 ) &&
             ( curr_file_data_location + file_header->size > 0x7FFFFFFF ) )
        {
            printf( "Error: Output is too big for 32 bit version. Please enable WicedFS 64 bit support.\n");
            free( full_path );
            free( file_header );
            return -5;
        }

        /* The file table entry header is no longer needed */
        free( file_header );

        /* Seek to where the file content data will be written */
        if ( 0 != fseeko64( output_handle, (off64_t) curr_file_data_location, SEEK_SET ) )
        {
             printf( "Error seeking output to %" PRIu64 "\n", curr_file_data_location );
             free( full_path );
            return -4;
        }

        if ( path_is_dir )
        {
            /* Item is a directory - the content data is another directory header
             * Recurse into the subdirectory and write it to the image file.
             */
            uint64_t tmp_bytes_written;

            write_dir( output_handle, full_path, &tmp_bytes_written );

            curr_file_data_location += tmp_bytes_written;
            *bytes_written +=  tmp_bytes_written;
        }
        else
        {
            /* Item is a file */
            uint32_t bytes_read;
            FILE*    input_handle;

            /* Open the file */
            input_handle = fopen( full_path, "rb" );
            if( input_handle == NULL )
            {
                printf( "Couldn't open input file %s\n", full_path );
                free( full_path );
                return -2;
            }

            /* Read the file content in chunks until end-of-file is reached */
            while ( ( bytes_read = (uint32_t) fread( buffer, 1, sizeof(buffer), input_handle ) ) != 0 )
            {
                /* Write the chunk to the image file */
                if ( bytes_read != fwrite(buffer, 1, bytes_read, output_handle) )
                {
                    printf( "Error writing to output\n");
                    free( full_path );
                    fclose( input_handle );
                    return -6;
                }

                /* Update position counters after write */
                *bytes_written          +=  bytes_read;
                curr_file_data_location += bytes_read;
            }

            fclose( input_handle );
        }

        free( full_path );

    }

    free( list );

    return 0;
}


/**
 * Gets a sorted list of files contained in the specified host PC directory
 *
 * @note: the list returned is allocated with malloc() and must be freed by the caller
 *
 * @param[in]  dir_name   : The path to the directory to read
 * @param[out] list       : Receives the sorted file list - Malloced - must be freed by caller
 * @param[out] item_len   : Receives the maximum string length in the list (including terminating null)
 * @param[out] item_count : Receives the number of items in the list
 *
 * @return 0 = success
 */
static int get_sorted_filelist( const char* dir_name, char** list, unsigned long* item_len, unsigned long* item_count )
{
    DIR*           dir_handle;
    struct dirent* file_desc;
    const char *   last_item = "\x00";
    char *         curr_find;
    unsigned long  file_count;
    unsigned long  max_filename_len;
    char*          unsorted;
    char*          sorted;
    unsigned long  file_num;
    unsigned long  sorted_file_num;

    /* Open the specified directory */
    dir_handle = opendir( dir_name );

    if( dir_handle == NULL )
    {
        printf( "Couldn't open directory %s\n", dir_name );
        return -1;
    }



    /* Read directory to find out how much space is required to store filenames */

    file_count = 0;
    max_filename_len = 0;
    while( ( file_desc = readdir( dir_handle ) ) != NULL )
    {
        /* Ignore "." and ".." */
        if ( ( 0 == strcmp( ".",  file_desc->d_name ) ) ||
             ( 0 == strcmp( "..", file_desc->d_name ) ) )
        {
            continue;
        }

        /* Count filenames */
        file_count++;

        /* Find longest filename */
        if ( max_filename_len < strlen( file_desc->d_name ) )
        {
            max_filename_len = strlen( file_desc->d_name );
        }
    }

    *item_len = max_filename_len+1;

    /* Allocate space for sorting filenames */

    unsorted = (char*) malloc( *item_len * file_count );
    sorted   = (char*) malloc( *item_len * file_count );


    /* Rewind to the start of the directory ready to re-read the entries */
    rewinddir( dir_handle );

    /* Read and store filenames for sorting */
    file_num = 0;
    while( ( file_desc = readdir( dir_handle ) ) != NULL )
    {
        /* Ignore "." and ".." */
        if ( ( 0 == strcmp( ".",  file_desc->d_name ) ) ||
             ( 0 == strcmp( "..", file_desc->d_name ) ) )
        {
            continue;
        }

        /* Copy the filename into the unsorted list */
        strcpy( &unsorted[*item_len * file_num], file_desc->d_name );
        file_num++;
    }


    /* Sort filenames */

    /* Cycle through the sorted list slots */
    for( sorted_file_num = 0; sorted_file_num < file_count; sorted_file_num++ )
    {
        unsigned long unsorted_file_num;

        /* Cycle through the unsorted list */
        curr_find = NULL;
        for( unsorted_file_num = 0; unsorted_file_num < file_count; unsorted_file_num++ )
        {
            /* Check whether the item is greater than the previous sorted item AND
             * either it is the first unsorted item, or is smaller than the smallest
             * unsorted item found so far...
             */
            if( ( 0 > strcmp( last_item, &unsorted[*item_len * unsorted_file_num] ) ) &&
                ( ( curr_find == NULL ) || ( 0 < strcmp( curr_find, &unsorted[*item_len * unsorted_file_num] ) ) ) )
            {
                /* Save item as it is the smallest found that is still larger than the sorted list top */
                curr_find = &unsorted[*item_len * unsorted_file_num];
            }
        }

        /* Copy the smallest unsorted item found into the next sorted slot */
        strcpy( &sorted[*item_len * sorted_file_num], curr_find );
        last_item = curr_find;
    }

    /* The unsorted list is no longer needed */
    free( unsorted );

    /* Nor is the directory handle */
    closedir( dir_handle );

    /* Return the sorted results */
    *list = sorted;
    *item_count = file_count;
    return 0;
}



/** Get the size of a file on the host PC
 *
 * @param[in] filename : the pathname of the file
 *
 * @return the file size, or negative on error
 */
static int64_t fsize( const char *filename )
{
    struct _stati64 st;

    if (_stati64(filename, &st) == 0)
    {
        return st.st_size;
    }

    return -1;
}


/** Determines if the given path is a directory
 *
 * @param[in] filename : the pathname to examine
 *
 * @return 1 = directory, 0 = file, negative = error
 */
static char isdir( const char *filename )
{
    struct stat st;

    if ( stat( filename, &st ) == 0 )
    {
        return ( S_ISDIR( st.st_mode ) ) ? 1 : 0;
    }

    return -1;
}
