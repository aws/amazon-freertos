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
 *  Implementation of the WicedFS Read-Only file system.
 */

#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include "wicedfs.h"
#define INSIDE_WICEDFS_C_
#include "wicedfs_internal.h"

/*@access wicedfs_filesystem_t, wicedfs_file_t, wicedfs_dir_t@*/ /* Lint: permit access to abstract handle implementations */

/******************************************************
 *                      Macros
 ******************************************************/
#ifndef WICEDFS_ASSERT_ACTION
#define WICEDFS_ASSERT_ACTION( )
#endif /* ifndef WICEDFS_ASSERT_ACTION */

#ifndef WICEDFS_NO_CHECK_PARAMS
#define WICEDFS_CHECK_PARAMS_NO_RETURN_VAL( expr )  { if (expr) { WICEDFS_ASSERT_ACTION(); return; }}
#define WICEDFS_CHECK_PARAMS( expr, retval )        { if (expr) { WICEDFS_ASSERT_ACTION(); return retval; }}
#else /* ifndef WICEDFS_NO_CHECK_PARAMS */
#ifdef DEBUG
#define WICEDFS_CHECK_PARAMS_NO_RETURN_VAL( expr )  { if (expr) { WICEDFS_ASSERT_ACTION(); }}
#define WICEDFS_CHECK_PARAMS( expr, retval )        { if (expr) { WICEDFS_ASSERT_ACTION(); }}
#else /* ifdef DEBUG */
#define WICEDFS_CHECK_PARAMS_NO_RETURN_VAL( expr )
#define WICEDFS_CHECK_PARAMS( expr, retval )
#endif /* ifdef DEBUG */
#endif /* ifndef WICEDFS_NO_CHECK_PARAMS */

/******************************************************
 *                    Constants
 ******************************************************/

#define WICEDFS_CMP_FILENAME_BUFF_SIZE (32)
#define WICEDFS_DIRECTORY_SEPARATOR    '/'

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

static int cmp_filename ( const wicedfs_filesystem_t* fs_handle, wicedfs_usize_t loc, const char* filename, uint32_t num_cmp_bytes, uint32_t filename_storage_len );
static int find_item    ( /*@in@*/ const wicedfs_filesystem_t* fs_handle, /*@in@*/ const char * filename, /*@out@*/ wicedfs_dir_header_t* curr_dir, /*@out@*/ wicedfs_file_header_t* file_hnd, /*@out@*/ wicedfs_usize_t* curr_dir_addr );

/******************************************************
 *               Variable Definitions
 ******************************************************/

/******************************************************
 *               Function Definitions
 ******************************************************/

int wicedfs_init ( wicedfs_usize_t base, wicedfs_read_func_t read_func, /*@out@*/ wicedfs_filesystem_t* fs_handle, /*@dependent@*/ void * user_param )
{
    wicedfs_filesystem_header_t fs_header;
    wicedfs_usize_t             bytes_read;

    WICEDFS_CHECK_PARAMS ( ( read_func == NULL ) || ( fs_handle == NULL ), -1 );

    /* Read the filesystem header */
    bytes_read = read_func( user_param, &fs_header, (wicedfs_usize_t) sizeof(fs_header), base );
    if ( bytes_read != sizeof(fs_header) )
    {
        return -1;
    }

    /*@-usedef@*/ /* Lint: splint does not pick up 'out' annotation of function pointer read_func */

    /* Check the magic number and version matches */
    if ( ( fs_header.magic_number != WICEDFS_MAGIC ) ||
         ( fs_header.version != (uint32_t) 1 ) )
    {
        return -2;
    }

    /* Save the hardware address of the root directory
     * in the filesystem handle for use by other functions
     */
    fs_handle->root_dir_addr = base + fs_header.root_dir_offset;       /*@+usedef@*/
    fs_handle->read_func     = read_func;
    fs_handle->user_param    = user_param;

    return 0;
}



int wicedfs_fopen ( /*@dependent@*/ const wicedfs_filesystem_t* fs_handle, /*@out@*/ wicedfs_file_t* handle, const char* filename )
{
    wicedfs_dir_header_t  curr_dir;
    wicedfs_file_header_t file_hnd;
    wicedfs_usize_t       curr_dir_addr;

    WICEDFS_CHECK_PARAMS( ( fs_handle == NULL ) || ( handle    == NULL ) || ( filename  == NULL ), -1 );

    /* Find the file */
    if ( WICEDFS_TYPE_FILE != find_item( fs_handle, filename, &curr_dir, &file_hnd, &curr_dir_addr ) )
    {
        return -1;
    }

    /* Fill out the details of the file handle */
    handle->fs_handle        = fs_handle;
    handle->location         = curr_dir_addr;
    handle->size             = file_hnd.size;
    handle->eof              = 0;
    handle->current_location = 0;

    return 0;
}



wicedfs_ssize_t wicedfs_filesize ( const wicedfs_filesystem_t* fs_handle, const char* filename )
{
    wicedfs_dir_header_t  curr_dir;
    wicedfs_file_header_t file_hnd;
    wicedfs_usize_t       curr_dir_addr;

    WICEDFS_CHECK_PARAMS( ( fs_handle == NULL ) || ( filename  == NULL ), -1 );

    /* Find the file */
    if ( WICEDFS_TYPE_FILE != find_item( fs_handle, filename, &curr_dir, &file_hnd, &curr_dir_addr ) )
    {
        return (wicedfs_ssize_t) -1;
    }

    /* Return the file size */
    return  (wicedfs_ssize_t) file_hnd.size;
}


int wicedfs_fclose( wicedfs_file_t * stream )
{
    WICEDFS_CHECK_PARAMS( ( stream == NULL ), -1 );

    /* Nothing currently needed here */
    UNUSED_PARAMETER( stream );
    return 0;
}

int wicedfs_fseek ( wicedfs_file_t* stream, wicedfs_ssize_t offset, int whence )
{
    wicedfs_ssize_t newpos = offset;

    WICEDFS_CHECK_PARAMS( ( stream == NULL ), -1 );

    /* Calculate the new file position
     * based on the Whence parameter
     */
    if ( whence == SEEK_CUR)
    {
        /* Seek from current file position */
        newpos += (wicedfs_ssize_t) stream->current_location;
    }
    else if ( whence == SEEK_END )
    {
        /* Seek from end of file */
        newpos += (wicedfs_ssize_t)stream->size;
    }
    else if ( whence == SEEK_SET )
    {
        /* Seek from start of file */
        /* do nothing */
    }
#ifdef WICEDFS_CHECK_PARAMS
    else
    {
        /* Bad whence parameter */
        WICEDFS_CHECK_PARAMS( 0==1, -1 );
    }
#endif /* ifndef WICEDFS_NO_CHECK_PARAMS */

    WICEDFS_CHECK_PARAMS( ( newpos < 0 ) || ( newpos > (wicedfs_ssize_t)stream->size ), -2 ); /* bad requested position */

    /* Reset EOF flag */
    stream->eof = 0;

    /* Set new file position */
    stream->current_location = (wicedfs_usize_t) newpos;
    return 0;
}

wicedfs_usize_t wicedfs_ftell(wicedfs_file_t* stream)
{
    WICEDFS_CHECK_PARAMS( ( stream == NULL ), 0 );

    return stream->current_location;
}

wicedfs_usize_t wicedfs_fread( /*@out@*/ void* data, wicedfs_usize_t size, wicedfs_usize_t count, wicedfs_file_t* stream)
{
    wicedfs_usize_t total_bytes = size * count;
    wicedfs_usize_t read_bytes;

    WICEDFS_CHECK_PARAMS( ( data == NULL ) || ( stream  == NULL ), 0 );

    /* User requested zero bytes - not an error */
    if ( total_bytes == 0 )
    {
        /*@-mustdefine@*/ /* LINT: Do not need to define data parameter */
        return 0;
        /*@+mustdefine@*/
    }

    /* Check if the stream is past the end of the file */
    if ( stream->current_location >= stream->size )
    {
        /* Stream past end of file
         * Set EOF flag
         */
        stream->eof = (uint8_t) 1;
        /*@-mustdefine@*/ /* LINT: Error occurred, do not need to define data parameter */
        return 0;
        /*@+mustdefine@*/
    }

    /* Reduce number of bytes to read if there are
     * not enough left before the end of the file
     */
    if ( stream->size - stream->current_location < total_bytes )
    {
        total_bytes = stream->size - stream->current_location;
        stream->eof = 1;
    }

    /* Read the bytes from the hardware */
    read_bytes = stream->fs_handle->read_func( stream->fs_handle->user_param, data, total_bytes, stream->location + stream->current_location );

    if ( read_bytes != total_bytes )
    {
        /* hardware read less than requested - something is wrong */
        return 0;
    }

    /* Update the current location */
    stream->current_location += read_bytes;

    /* Return the number of items read */
    return read_bytes / size;
}


int wicedfs_feof(wicedfs_file_t *stream)
{
    WICEDFS_CHECK_PARAMS( stream == NULL, -1 );

    return (int) stream->eof;
}


int wicedfs_opendir( /*@dependent@*/ const wicedfs_filesystem_t* fs_handle, /*@out@*/ wicedfs_dir_t* handle, const char* dirname )
{
    wicedfs_dir_header_t  curr_dir;
    wicedfs_file_header_t file_hnd;
    wicedfs_usize_t       curr_dir_addr;

    WICEDFS_CHECK_PARAMS( ( fs_handle == NULL ) || ( handle == NULL ) || ( dirname == NULL ) || ( ( dirname != NULL ) && ( dirname[0] == '\x00' ) ), -1 );

    /* Find the directory */
    if ( WICEDFS_TYPE_DIR != find_item( fs_handle, dirname, &curr_dir, &file_hnd, &curr_dir_addr ) )
    {
        return -1;
    }

    /* Fill out the details of the directory handle */
    handle->fs_handle           = fs_handle;
    handle->file_table_location = (wicedfs_usize_t)( (wicedfs_ssize_t) curr_dir_addr + curr_dir.file_table_offset );
    handle->num_files           = curr_dir.num_files;
    handle->file_header_size    = curr_dir.file_header_size;
    handle->filename_size       = curr_dir.file_header_size;
    handle->current_location    = 0;

    return 0;
}

int wicedfs_closedir( wicedfs_dir_t* dirp )
{
    WICEDFS_CHECK_PARAMS( ( dirp == NULL ), -1 );

    /* Nothing currently needed here */
    UNUSED_PARAMETER( dirp );
    return 0;
}


int wicedfs_readdir_with_size( wicedfs_dir_t* dirp, /*@out@*/ char* name_buf, unsigned int name_buf_len, wicedfs_entry_type_t* type, wicedfs_usize_t* size )
{
    wicedfs_usize_t bytes_read;
    wicedfs_usize_t file_header_loc;
    wicedfs_file_header_t file_header;

    WICEDFS_CHECK_PARAMS( ( dirp == NULL ) || ( name_buf  == NULL ) || ( type == NULL ), -1 );

    if ( (wicedfs_usize_t) name_buf_len < dirp->filename_size )
    {
        /* Not enough space in buffer */ /*@-mustdefine@*/
        return -1;
        /*@+mustdefine@*/
    }

    if ( dirp->current_location >= dirp->num_files )
    {
        /* Trying to read past end of directory */ /*@-mustdefine@*/
        return -2;
        /*@+mustdefine@*/
    }

    file_header_loc = dirp->file_table_location + dirp->current_location * dirp->file_header_size;

    /* Read the file header from the file table entry */
    bytes_read = dirp->fs_handle->read_func( dirp->fs_handle->user_param, &file_header, sizeof(file_header), file_header_loc );

    if ( bytes_read != sizeof(file_header) )
    {
        return -1;
    }

    /* Read the filename from the file table entry */
    bytes_read = dirp->fs_handle->read_func( dirp->fs_handle->user_param, name_buf, dirp->filename_size, (wicedfs_usize_t) ( file_header_loc + sizeof(wicedfs_file_header_t) ) );

    if ( bytes_read != dirp->filename_size )
    {
        return -2;
    }

    /* Get the entry type (file or dir) */
    if ( ( file_header.type_flags_permissions & WICEDFS_TYPE_MASK ) == WICEDFS_TYPE_FILE )
    {
        *type = WICEDFS_FILE;
        *size = file_header.size;
    }
    else
    {
        *type = WICEDFS_DIR;
        *size = 0;
    }

    /* Increment the current location within the file table */
    dirp->current_location++;

    return 0;
}

void wicedfs_rewinddir( wicedfs_dir_t *dirp )
{
    WICEDFS_CHECK_PARAMS_NO_RETURN_VAL( dirp == NULL );

    /* Reset the current location within the file table */
    dirp->current_location = 0;
}

int wicedfs_eodir ( wicedfs_dir_t* dirp )
{
    WICEDFS_CHECK_PARAMS( dirp == NULL, 0 );

    return dirp->current_location >= dirp->num_files;
}


/******************************************************
 *               Static Function Definitions
 ******************************************************/


/**
 * Compares part of a file path to a pathname on the hardware device
 *
 * @param[in] fs_handle            : handle of the filesystem obtained from wicedfs_init
 * @param[in] loc                  : The hardware device location of the path to compare against
 * @param[in] filename             : The path that will be partly compared against the path from the hardware device
 * @param[in] num_cmp_bytes        : Number of bytes of parameter "filename" to compare
 * @param[in] filename_storage_len : The maximum number of path bytes available on the hardware
 *
 * @return 0  if the required path parts match, 1 = no match, negative = error
 */
static int cmp_filename( const wicedfs_filesystem_t* fs_handle,
                         wicedfs_usize_t           loc,
                         const char*               filename,
                         uint32_t                  num_cmp_bytes,
                         uint32_t                  filename_storage_len )
{
    /* Stack allocate a small buffer for the comparison */
    char buf[WICEDFS_CMP_FILENAME_BUFF_SIZE];

    /* Determine how many bytes to compare */
    uint32_t cmp_len = ( num_cmp_bytes > filename_storage_len )? filename_storage_len : num_cmp_bytes;

    do
    {
        uint32_t        read_len;
        wicedfs_usize_t bytes_read;

        /* Read up to one buffer of the filename from the hardware */
        read_len = ( cmp_len > sizeof(buf) )? (uint32_t) sizeof(buf) : cmp_len;
        bytes_read = fs_handle->read_func( fs_handle->user_param, buf, (wicedfs_usize_t) read_len, loc );
        if ( bytes_read != (wicedfs_usize_t) read_len )
        {
            /* Error reading data */
            return -1;
        }

        /* Compare the amount read to the filename parameter */
        if ( 0 != strncmp( buf, filename, (size_t) read_len ) )
        {
            /* Filename on hardware did not match the provided filename parameter */
            return 1;
        }

        /* Increment amount read */
        cmp_len  -= read_len;
        filename += read_len;
        loc      += read_len;

    } while ( cmp_len > 0 );  /* Loop to read more if needed */

    /* The entire filename was compared and matched */
    return 0;
}

/**
 * Finds a named file/directory
 *
 * Starting at the root directory, this function
 * searches file tables to find required subdirectories and
 * eventually to find the required file table entry.
 *
 * @param[in]     fs_handle     : filesystem handle obtained from wicedfs_init()
 * @param[in]     filename      : the path of the file table entry (e.g. a file or directory)
 * @param[out]    curr_dir      : Receives the directory header of the item found.
 * @param[out]    file_hnd      : Receives the file table entry of the item found.
 * @param[out]    curr_dir_addr : Receives the hardware address of the content data for the item found.
 *
 * @return The WICEDFS_TYPE (e.g. DIR, FILE, etc) or negative on error
 */
static int find_item( /*@in@*/  const wicedfs_filesystem_t*  fs_handle,
                      /*@in@*/  const char*                filename,
                      /*@out@*/ wicedfs_dir_header_t*      curr_dir,
                      /*@out@*/ wicedfs_file_header_t*     file_hnd,
                      /*@out@*/ wicedfs_usize_t*           curr_dir_addr )
{
    uint32_t        cmp_bytes;
    char*           slash_offset;
    uint32_t        file_num;
    wicedfs_usize_t curr_file_table_addr;

    /* Start at the root directory */
    *curr_dir_addr = fs_handle->root_dir_addr;

    file_hnd->type_flags_permissions = WICEDFS_TYPE_DIR;

    /* Each interation of this loop descends one subdirectory */
    while ( 1 == 1 )
    {
        wicedfs_usize_t bytes_read;
        int cmp_val = -1;

        /* Skip the first character if it is a directory separator */
        if ( filename[0] == WICEDFS_DIRECTORY_SEPARATOR )
        {
            filename++;
        }

        /* Read the header of the current directory from the hardware */
        bytes_read = fs_handle->read_func( fs_handle->user_param, curr_dir, (wicedfs_usize_t) sizeof(wicedfs_dir_header_t), *curr_dir_addr );
        if ( bytes_read != sizeof(wicedfs_dir_header_t) )
        {
            /* Error reading hardware device */
            return -1;
        }

        /* If the filename has been reduced to nothing, then the
         * caller requested a directory name, and it has been found.
         */
        if ( filename[ 0 ] == '\x00' )
        {
            /* Return the file table entry-type */
            return (int) ( file_hnd->type_flags_permissions & WICEDFS_TYPE_MASK );
        }

        /* Look for a directory separator character */
        if ( ( slash_offset = strchr( filename, WICEDFS_DIRECTORY_SEPARATOR ) ) != NULL )
        {
            /* A directory separator was found.
             * Calculate the length of the name comparison that should be performed
             */
            cmp_bytes = (uint32_t)( slash_offset - filename );
        }
        else
        {
            /* No directory separator. Compare entire name */
            cmp_bytes = (uint32_t) strlen( filename );
        }


        /* Set up loop variables */
        file_num = 0;
        curr_file_table_addr = (wicedfs_usize_t)( (wicedfs_ssize_t) *curr_dir_addr + curr_dir->file_table_offset );

        /* Loop over each file table entry
         * For each: read the entry from hardware, then compare the entry name to the given filename
         */
        while ( ( ( bytes_read = fs_handle->read_func( fs_handle->user_param, file_hnd, (wicedfs_usize_t) sizeof(wicedfs_file_header_t), curr_file_table_addr + file_num * curr_dir->file_header_size ) ) == (wicedfs_usize_t) sizeof(wicedfs_file_header_t) ) &&
                ( 1 == ( cmp_val = cmp_filename( fs_handle, (wicedfs_usize_t) ( curr_file_table_addr + file_num * curr_dir->file_header_size + sizeof(wicedfs_file_header_t) ), filename, cmp_bytes, curr_dir->filename_size ) ) ) )
        {
            /* Not found yet - increment file table position */
            file_num++;

            /* Check if position has passed the end of the file table */
            if ( file_num >= curr_dir->num_files )
            {
                /* Table position has passed the end of the
                 * file table without finding a match.
                 *
                 * i.e. file not found
                 */
                return -2;
            }
        }
        if ( ( bytes_read != (wicedfs_usize_t) sizeof(wicedfs_file_header_t) ) ||
             ( cmp_val < 0 ) )
        {
            /* error reading hardware device */
            return -3;
        }

        /* Prepare current directory pointer to descend into next directory */
        *curr_dir_addr = (wicedfs_usize_t) ((wicedfs_ssize_t) curr_file_table_addr + file_hnd->offset +  (wicedfs_ssize_t)( file_num * curr_dir->file_header_size ));

        /* Strip off the part of the filename that was found */
        filename += cmp_bytes;
    }

    /* No return needed here due to the while(1) */ /*@-noret@*/
}
    /*@+noret@*/

/*@noaccess wicedfs_filesystem_t, wicedfs_file_t, wicedfs_dir_t@*/
