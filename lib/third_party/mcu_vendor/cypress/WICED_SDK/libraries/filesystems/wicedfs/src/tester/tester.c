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
 *  Tester tool for WicedFS
 *
 *  Creates an WicedFS image file from a specified host PC
 *  directory, then uses the WicedFS API to read back
 *  the data and compare it to the files/directories
 *  on the ost PC
 */

#include <stdio.h>
#include <inttypes.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <sys/stat.h>
#include <errno.h>
#include "wicedfs.h"

#define INSIDE_WICEDFS_C_
#include "wicedfs_create.h"
#include "wicedfs_internal.h"

/******************************************************
 *                      Macros
 ******************************************************/

/******************************************************
 *                    Constants
 ******************************************************/
#define COMPARE_BUFFER_SIZE     (1024*1024)
#define IMAGE_FILENAME          "image.bin"
#define PATH_BUFFER_SIZE        (256)
#define DIRECTORY_SEPARATOR_STR "/"
#define ROOT_DIRECTORY          "/"
#define DIRECTORY_SEPARATOR_CHR '/'

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
static int             cmp_fn                ( const void * a, const void * b );
static int             get_sorted            ( const char* dir_name, char *** ptr_list, unsigned long* count );
static void            free_list             ( char*** ptr_list, unsigned long count );
static int             test_compare_dir      ( wicedfs_filesystem_t* fs_handle, const char* wdir_name, const char* dir_name );
static wicedfs_usize_t hostfile_wicedfs_read ( void* user_param, void* buf, wicedfs_usize_t size, wicedfs_usize_t pos );

/******************************************************
 *               Variable Definitions
 ******************************************************/
static char buffer1[COMPARE_BUFFER_SIZE];
static char buffer2[COMPARE_BUFFER_SIZE];

/******************************************************
 *               Function Definitions
 ******************************************************/

int main (int argc, const char * argv[])
{
    const char *         image_filename = IMAGE_FILENAME;
    FILE*                image_file;
    wicedfs_filesystem_t fs_handle;

    /* Check command-line argument */
    if ( argc != 2 )
    {
        printf( "Usage: mk_wicedfs <source_dir>\n\n" );
        return -1;
    }

    /* Create the WicedFS image file from the specified directory */
    if ( 0 != create_wicedfs( image_filename, argv[1] ) )
    {
        printf( "Error creating file system image file %s\n", image_filename );
        return -2;
    }

    /* Open the WicedFS image file for reading back */
    image_file = fopen( image_filename, "rb" );
    if ( image_file == NULL )
    {
        printf( "Error opening file system image file %s\n", image_filename );
        return -1;
    }

    /* Initialise WicedFS */
    wicedfs_init( (wicedfs_usize_t) 0, hostfile_wicedfs_read, &fs_handle, image_file );

    /* Compare the WicedFS filesystem to the Host PC directory */
    if ( 0 == test_compare_dir( &fs_handle, ROOT_DIRECTORY, argv[1] ) )
    {
        printf( "Success!\n");
    }

    /* Clean up */
    fclose( image_file );

    return 0;

}


/******************************************************
 *               Static Function Definitions
 ******************************************************/

/** String comparison function to be used with qsort
 *
 * @param[in] a : first string
 * @param[in] b : second string
 *
 * @return
 */
static int cmp_fn( const void * a, const void * b )
{
    return strcmp( *((char **)a), *((char**) b) );
}


/** Get a sorted list of the content of a directory
 *
 * @note : Both the items in the list, and the list
 *         itself are allocated using malloc( ) and
 *         must be freed by the caller to avoid memory leakage
 *
 * @param[in]  dir_name : Path of the directory
 * @param[out] ptr_list : Receives the list of item names
 * @param[out] count    : Receives the number of items in the list
 */
static int get_sorted( const char* dir_name, char *** ptr_list, unsigned long* count )
{
    DIR* dir;
    struct dirent * entry;
    unsigned long entry_num;

    /* Open the specified directory for reading */
    if ( NULL == ( dir = opendir( dir_name ) ) )
    {
        printf( "Error opening disk dir %s\n", dir_name );
        return -2;
    }


    /* Count the number of items in the directory */
    *count = 0;
    while ( ( entry = readdir( dir ) ) != NULL )
    {
        (*count)++;
    }

    /* Rewind the directory ready for re-reading */
    rewinddir( dir );

    /* Allocate space for the list pointers */
    *ptr_list = (char**) malloc( (*count) * sizeof(char*) );

    /* Read the directory items into the list*/
    entry_num = 0;
    while ( ( entry = readdir( dir ) ) != NULL )
    {
        /* Allocate space in the list for this item name */
        (*ptr_list)[entry_num] = (char*) malloc( strlen( entry->d_name ) + 1 );

        /* Copy the item name into the list */
        strcpy( (*ptr_list)[entry_num], entry->d_name );
        entry_num++;
    }

    /* Sort the list */
    qsort( *ptr_list, (*count), sizeof(char*), cmp_fn );

    /* Finished */
    closedir( dir );

    return 0;
}


/** Free a list created with get_sorted( )
 *
 * @param[in] ptr_list : a pointer to the list
 * @param[in] count    : the number of items in the list
 */
static void free_list( char*** ptr_list, unsigned long count )
{
    unsigned long i;
    /* Free each item in the list */
    for( i = 0; i < count; i++ )
    {
        free( (*ptr_list)[i] );
    }
    /* Free the list pointers */
    free( *ptr_list );
}

/** Compare a WicedFS filesystem against a Host PC directory
 *
 * Recursively compares the contents of the WicedFS to a Host PC directory
 *
 * @param[in] fs_handle : Filesystem handle obtained from wicedfs_init
 * @param[in] wdir_name : Directory path in the WicedFS filesystem
 * @param[in] dir_name  : Directory path on the Host PC
 *
 * @return 0 = Directories match exactly
 */
static int test_compare_dir( wicedfs_filesystem_t* fs_handle, const char* wdir_name, const char* dir_name )
{
    wicedfs_dir_t wdir;
    char          namebuf[PATH_BUFFER_SIZE];
    char **       sorted_ptr_list;
    unsigned long sorted_count;
    unsigned long entry_num;

    /* Open the WicedFS directory */
    if ( 0 != wicedfs_opendir( fs_handle, &wdir, wdir_name ) )
    {
        printf( "Error opening wicedfs dir %s\n", wdir_name );
        return -1;
    }

    printf("opened dir %s at %" PRIu64 "\n", wdir_name, (uint64_t)wdir.file_table_location );

    /* Open the local Host PC directory and get a sorted list of items */
    if ( 0 != get_sorted( dir_name, &sorted_ptr_list, &sorted_count ))
    {
        printf( "Error sorting disk dir %s\n", dir_name );
        wicedfs_closedir( &wdir );
        return -2;
    }

    /* Loop for each local Host PC item */
    entry_num = 0;
    while ( entry_num < sorted_count )
    {
        FILE*                file;
        char                 path[PATH_BUFFER_SIZE];
        char                 wpath[PATH_BUFFER_SIZE];
        struct               stat st;
        wicedfs_entry_type_t entry_type;

        /* Ignore "." and ".." */
        if ( ( 0 == strcmp( sorted_ptr_list[entry_num], ".." ) ) ||
             ( 0 == strcmp( sorted_ptr_list[entry_num], "." ) ) )
        {
            entry_num++;
            continue;
        }

        /* Start constructing the full host path of this item - Copy the base directory */
        strcpy( path, dir_name );

        /* If the base directory does not have a separator, add one */
        if ( dir_name[ strlen(dir_name) - 1 ] != DIRECTORY_SEPARATOR_CHR )
        {
            strcat( path, DIRECTORY_SEPARATOR_STR );
        }

        /* Add the item name to complete the path */
        strcat( path, sorted_ptr_list[entry_num] );

        /* Start constructing the full WicedFS path of this item - Copy the base directory */
        strcpy( wpath, wdir_name );

        /* If the base directory does not have a separator, add one */
        if ( wdir_name[ strlen(wdir_name) - 1 ] != DIRECTORY_SEPARATOR_CHR )
        {
            strcat( wpath, DIRECTORY_SEPARATOR_STR );
        }

        /* Add the item name to complete the path */
        strcat( wpath, sorted_ptr_list[entry_num] );

        /* Check if the host PC item is a directory */
        if ( ( stat( path, &st ) == 0) &&
             ( S_ISDIR(st.st_mode ) ) )
        {
            /* Item is a sub-directory - Recurse into it. */
            int retval;
            printf( "recursing to %s , %s\n", wpath, path );
            retval = test_compare_dir( fs_handle, wpath, path );
            if ( retval != 0 )
            {
                free_list( &sorted_ptr_list, sorted_count );
                wicedfs_closedir( &wdir );
                return retval;
            }
        }

        /* Read the next item name from the WicedFS directory*/
        if ( 0 != wicedfs_readdir( &wdir, namebuf, sizeof(namebuf), &entry_type ) )
        {
            printf( "Error reading wicedfs directory\n" );
            free_list( &sorted_ptr_list, sorted_count );
            wicedfs_closedir( &wdir );
            return -3;
        }

        /* Compare the item name against the one from the Host PC directory */
        if ( 0 != strcmp( namebuf, sorted_ptr_list[entry_num] ) )
        {
            printf( "Error names do not match: wicedfs \"%s\", disk \"%s\"\n", namebuf, sorted_ptr_list[entry_num] );
            free_list( &sorted_ptr_list, sorted_count );
            wicedfs_closedir( &wdir );
            return -4;
        }

        /* Check if the item is a file */
        if ( (stat(path, &st) == 0) &&
             (! S_ISDIR(st.st_mode) ) )
        {
            /* Item is a file - compare the content */
            size_t         num_read;
            wicedfs_file_t bfile;

            /* Open the Host PC file for reading */
            if ( NULL == ( file = fopen ( path, "rb" ) ) )
            {
                printf( "Error opening disk file %s , %s\n", path, strerror( errno ) );
                free_list( &sorted_ptr_list, sorted_count );
                wicedfs_closedir( &wdir );
                return -5;
            }

            /* Open the WicedFS for reading */
            if ( 0 != wicedfs_fopen( fs_handle, &bfile, wpath ) )
            {
                printf( "Error opening wicedfs file %s\n", wpath );
                free_list( &sorted_ptr_list, sorted_count );
                wicedfs_closedir( &wdir );
                fclose(file);
                return -5;
            }

            /* Read chunks from Host PC file until end-of-file reached */
            while ( ( num_read = fread( buffer1, 1, sizeof(buffer1), file ) ) != 0 )
            {
                size_t bnum_read;

                /* Read a chunk from the WicedFS file */
                bnum_read = (size_t) wicedfs_fread( buffer2, (wicedfs_usize_t) 1, (wicedfs_usize_t) sizeof(buffer2), &bfile );

                /* Check that the number of bytes read matches */
                if ( bnum_read != num_read )
                {
                    printf( "Error: num read mismatch %lu %lu\n", num_read, bnum_read );
                    free_list( &sorted_ptr_list, sorted_count );
                    wicedfs_closedir( &wdir );
                    return -6;
                }

                /* Check that the data matches */
                if ( 0 != memcmp( buffer1, buffer2, num_read ) )
                {
                    printf( "Error data mismatch in file %s\n", wpath );
                    free_list( &sorted_ptr_list, sorted_count );
                    wicedfs_closedir( &wdir );
                    return -7;
                }

            }

            /* Close both files */
            wicedfs_fclose( &bfile );
            fclose( file );
        }
        printf( "verified %s\n", namebuf );
        entry_num++;
    }

    /* Cleanup */
    free_list( &sorted_ptr_list, sorted_count );

    wicedfs_closedir( &wdir );

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
