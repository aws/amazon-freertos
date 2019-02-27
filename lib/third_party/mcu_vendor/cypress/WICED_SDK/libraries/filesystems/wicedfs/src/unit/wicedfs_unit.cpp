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
 *  Unit Tester for WicedFS
 *
 *  Runs a suite of tests on the WicedFS API to attempt
 *  to discover bugs
 */

#include <stdio.h>
#include <string.h>
#include "gtest/gtest.h"
#include "wicedfs.h"
#include "wicedfs_unit_images.h"

/******************************************************
 *                      Macros
 ******************************************************/
#ifndef _WIN32
/*define_style_exception_start*/
#define off64_t __off64_t
#define _stati64 stat64
/*define_style_exception_end*/
#endif /* ifndef _WIN32 */

#ifndef MIN
extern int MIN (/*@sef@*/ int x, /*@sef@*/ int y); /* LINT : This tells lint that  the parameter must be side-effect free. i.e. evaluation does not change any values (since it is being evaulated more than once */
#define MIN(x,y) ((x) < (y) ? (x) : (y))
#endif /* ifndef MIN */

#define RETCHK_START( retcheck_var_in )  \
        { \
            int count = 0; \
            int* retcheck_var = &(retcheck_var_in); \
            do \
            { \
                *retcheck_var = count; \
                count++;

#define RETCHK_ACTIVATED()  (*retcheck_var == -1)

#define RETCHK_EXPECT_EQ( expected, val )  if ( RETCHK_ACTIVATED( ) ) { EXPECT_EQ( (expected), (val) ); }
#define RETCHK_EXPECT_NE( expected, val )  if ( RETCHK_ACTIVATED( ) ) { EXPECT_NE( (expected), (val) ); }
#define RETCHK_EXPECT_GT( expected, val )  if ( RETCHK_ACTIVATED( ) ) { EXPECT_GT( (expected), (val) ); }

#define RETCHK_COUNT( ) count

#define RETCHK_END( ) \
            } while ( *retcheck_var == -1 ); \
            *retcheck_var = -1; \
        }

/******************************************************
 *                    Constants
 ******************************************************/
#define DIRECTORY_SEPARATOR_STR "/"
#define TEST_FILENAME           "test.txt"
#define TEST2_DIR               "test2"
#define TEST2_FILENAME          "test2/test2.txt"
#define GARBAGE_FILENAME        "badfood"
#define BASE_SIZE               ((int)sizeof(wicedfs_usize_t))

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
static wicedfs_usize_t testimage_wicedfs_read( void* user_param, void* buf, wicedfs_usize_t size, wicedfs_usize_t pos );
//static int unit_fopen( void );
//static int unit_fread_ftell_feof_fseek( void );
//static int unit_filesize( void );
//static int unit_opendir( void );
//static int unit_readdir_eodir( void );
//static int unit_wicedfs_init( void );

/******************************************************
 *               Variable Definitions
 ******************************************************/

static const char lorem[] = "Lorem ipsum dolor sit amet, consectetur adipisicing elit, sed do eiusmod tempor\n" \
                            "incididunt ut labore et dolore magna aliqua. Ut enim ad minim veniam, quis nostrud\n" \
                            "exercitation ullamco laboris nisi ut aliquip ex ea commodo consequat. Duis aute irure\n" \
                            "dolor in reprehenderit in voluptate velit esse cillum dolore eu fugiat nulla pariatur.\n" \
                            "Excepteur sint occaecat cupidatat non proident, sunt in culpa qui officia deserunt mollit\n" \
                            "anim id est laborum.";

static const unsigned int lorem_len = sizeof(lorem)-1;

static int bad_read_countdown = -1;
static int short_read_countdown = -1;
static int corruption_countdown = -1;
static void* user_param_received = NULL;

/******************************************************
 *               Function Definitions
 ******************************************************/


/******************************************************
 *            wicedfs_init tests
 ******************************************************/


/* Initialise WicedFS normally */
TEST(unit_test_wicedfs_init, normal)
{
    wicedfs_filesystem_t fs_handle;
    int result;
    result = wicedfs_init( (wicedfs_usize_t) 0, testimage_wicedfs_read, &fs_handle, NULL );
    EXPECT_EQ( 0, result );
}


/* null function pointer */
TEST(unit_test_wicedfs_init, null_function_pointer)
{
    wicedfs_filesystem_t fs_handle;
    int result;
    result = wicedfs_init( (wicedfs_usize_t) 0, NULL, &fs_handle, NULL );
    EXPECT_NE( 0, result );
}


/* null handle pointer */
TEST(unit_test_wicedfs_init, null_handle)
{
    int result;
    result = wicedfs_init( (wicedfs_usize_t) 0, testimage_wicedfs_read, NULL, NULL );
    EXPECT_NE( 0, result );
}



/* Non null user data pointer - shoud be passed through to read function */
TEST(unit_test_wicedfs_init, non_null_user_data)
{
    wicedfs_filesystem_t fs_handle;
    int result;
    user_param_received = NULL;
    result = wicedfs_init( (wicedfs_usize_t) 0, testimage_wicedfs_read, &fs_handle, (void*)0xa5a5a5a5 );
    EXPECT_EQ( 0, result );
    EXPECT_EQ( (void*)0xa5a5a5a5, user_param_received ); // should be 0 - TODO mock up read function
}




/* Bad reads from hardware */
TEST(unit_test_wicedfs_init, bad_hardware_reads)
{
    wicedfs_filesystem_t fs_handle;
    int result;

    RETCHK_START( bad_read_countdown )
    {
        result = wicedfs_init( (wicedfs_usize_t) 0, testimage_wicedfs_read, &fs_handle, NULL );
        RETCHK_EXPECT_GT( 0, result );
    }
    RETCHK_END( )
}


/* Short reads from hardware */
TEST(unit_test_wicedfs_init, short_hardware_reads)
{
    wicedfs_filesystem_t fs_handle;
    int result;
    RETCHK_START( short_read_countdown )
    {
        result = wicedfs_init( (wicedfs_usize_t) 0, testimage_wicedfs_read, &fs_handle, NULL );
        RETCHK_EXPECT_GT( 0, result );
    }
    RETCHK_END( )
}



/* File Corruption */
TEST(unit_test_wicedfs_init, file_corruption)
{
    wicedfs_filesystem_t fs_handle;
    int result;

    RETCHK_START( corruption_countdown )
    {
        result = wicedfs_init( (wicedfs_usize_t) 0, testimage_wicedfs_read, &fs_handle, NULL );
        if ( RETCHK_COUNT() <= 8 )
        {
            RETCHK_EXPECT_GT( 0, result );   /* Error below offset 8 */
        }
        else
        {
            RETCHK_EXPECT_EQ( 0, result ); /* No error above offset 8 */
        }
    }
    RETCHK_END( )
}








/******************************************************
 *            wicedfs_fopen tests
 ******************************************************/

class unit_test_wicedfs_fopen :  public ::testing::Test
{

    protected:

    wicedfs_filesystem_t fs_handle;


    virtual void SetUp()
    {
        /* Setup - Initialise WicedFS */
        wicedfs_init( (wicedfs_usize_t) 0, testimage_wicedfs_read, &fs_handle, NULL );
    }
    virtual void TearDown()
    {

    }
};

/* wicedfs_fopen normal - with valid filename */
TEST_F( unit_test_wicedfs_fopen, normal_valid )
{
    int result;
    wicedfs_file_t file_handle;
    result = wicedfs_fopen( &fs_handle, &file_handle, TEST_FILENAME );
    EXPECT_EQ( 0, result );

    result = wicedfs_fclose( &file_handle );
    EXPECT_EQ( 0, result );
}


/* Subdir with leading separator - Should open */
TEST_F( unit_test_wicedfs_fopen, subdir_leading_separator )
{
    int result;
    wicedfs_file_t file_handle;
    result = wicedfs_fopen( &fs_handle, &file_handle, DIRECTORY_SEPARATOR_STR  TEST2_FILENAME );
    EXPECT_EQ( 0, result );

    result = wicedfs_fclose( &file_handle );
    EXPECT_EQ( 0, result );
}



/* Subdir without leading separator - Should open */
TEST_F( unit_test_wicedfs_fopen, subdir_no_leading_separator )
{
    int result;
    wicedfs_file_t file_handle;
    result = wicedfs_fopen( &fs_handle, &file_handle,  TEST2_FILENAME );
    EXPECT_EQ( 0, result );

    result = wicedfs_fclose( &file_handle );
    EXPECT_EQ( 0, result );
}



/* Blank filename - error */
TEST_F( unit_test_wicedfs_fopen, blank_filename )
{
    int result;
    wicedfs_file_t file_handle;
    result = wicedfs_fopen( &fs_handle, &file_handle,  "" );
    EXPECT_NE( 0, result );
}

/* NULL filename - error */
TEST_F( unit_test_wicedfs_fopen, null_filename )
{
    int result;
    wicedfs_file_t file_handle;
    result = wicedfs_fopen( &fs_handle, &file_handle,  NULL );
    EXPECT_NE( 0, result );
}

/* NULL fs_handle - error */
TEST_F( unit_test_wicedfs_fopen, null_fs_handle )
{
    int result;
    wicedfs_file_t file_handle;
    result = wicedfs_fopen( NULL, &file_handle,  TEST_FILENAME );
    EXPECT_NE( 0, result );
}



/* NULL file handle - error */
TEST_F( unit_test_wicedfs_fopen, null_file_handle )
{
    int result;
    result = wicedfs_fopen( &fs_handle, NULL, TEST_FILENAME );
    EXPECT_NE( 0, result );
}


/* fclose with NULL file handle - error */
TEST_F( unit_test_wicedfs_fopen, fclose_null_file_handle )
{
    int result;
    result = wicedfs_fclose( NULL );
    EXPECT_NE( 0, result );
}


/* Not Found filename - Should fail */
TEST_F( unit_test_wicedfs_fopen, not_found_filename )
{
    int result;
    wicedfs_file_t file_handle;
    result = wicedfs_fopen( &fs_handle, &file_handle, GARBAGE_FILENAME );
    EXPECT_NE( 0, result );
}


/* Bad reads from hardware */
TEST_F(unit_test_wicedfs_fopen, bad_hardware_reads)
{
    int result;
    wicedfs_file_t file_handle;

    RETCHK_START( bad_read_countdown )
    {
        result = wicedfs_fopen( &fs_handle, &file_handle, TEST_FILENAME );
        RETCHK_EXPECT_GT( 0, result );
    }
    RETCHK_END( )

    wicedfs_fclose( &file_handle );
}


/* Short reads from hardware */
TEST_F(unit_test_wicedfs_fopen, short_hardware_reads)
{
    int result;
    wicedfs_file_t file_handle;
    RETCHK_START( short_read_countdown )
    {
        result = wicedfs_fopen( &fs_handle, &file_handle, TEST_FILENAME );
        RETCHK_EXPECT_GT( 0, result );
    }
    RETCHK_END( )

    wicedfs_fclose( &file_handle );
}



/* File Corruption */
TEST_F(unit_test_wicedfs_fopen, file_corruption)
{
    int result;
    wicedfs_file_t file_handle;

    RETCHK_START( corruption_countdown )
    {
        result = wicedfs_fopen( &fs_handle, &file_handle, TEST_FILENAME );
        if ( RETCHK_COUNT() <= BASE_SIZE )
        {
            RETCHK_EXPECT_GT( 0, result );   /* Error below offset 8 */
        }
        else
        {
            RETCHK_EXPECT_EQ( 0, result ); /* No error above offset 8 */
        }
    }
    RETCHK_END( )

    wicedfs_fclose( &file_handle );
}




/******************************************************
 *            wicedfs_fread, ftell, feof, fseek tests
 ******************************************************/

class unit_test_wicedfs_fread_ftell_feof_fseek :  public ::testing::Test
{

    protected:

    wicedfs_filesystem_t fs_handle;
    wicedfs_file_t file_handle;


    virtual void SetUp()
    {
        /* Setup - Initialise WicedFS */
        wicedfs_init( (wicedfs_usize_t) 0, testimage_wicedfs_read, &fs_handle, NULL );
        wicedfs_fopen( &fs_handle, &file_handle, TEST_FILENAME );
    }
    virtual void TearDown()
    {
        wicedfs_fclose( &file_handle );
    }
};



/* read 0 bytes - should succeed */
TEST_F( unit_test_wicedfs_fread_ftell_feof_fseek, read_zero_bytes )
{
    int num_read;
    char buffer[lorem_len];
    num_read = wicedfs_fread( buffer, 0, 0, &file_handle );
    EXPECT_EQ( 0, num_read );
}

/* read part file - should succeed */
TEST_F( unit_test_wicedfs_fread_ftell_feof_fseek, read_part_of_file )
{
    int result;
    wicedfs_usize_t num_read;
    wicedfs_usize_t location;
    char buffer[lorem_len];
    wicedfs_usize_t readsize = (wicedfs_usize_t)lorem_len - 10;

    /* read part file - should succeed */
    num_read = wicedfs_fread( buffer, 1, readsize, &file_handle );
    EXPECT_EQ( readsize, num_read );

    /* compare content */
    result = memcmp( buffer, lorem, (size_t)readsize );
    EXPECT_EQ( 0, result );

    /* check current location */
    location = wicedfs_ftell( &file_handle );
    EXPECT_EQ( readsize, location );
}


/* read from mid-file to past end - should succeed */
TEST_F( unit_test_wicedfs_fread_ftell_feof_fseek, read_off_end_of_file )
{
    int result;
    wicedfs_usize_t readpos = (wicedfs_usize_t)lorem_len - 10;
    wicedfs_usize_t readlen = (wicedfs_usize_t)lorem_len - readpos;
    wicedfs_usize_t num_read;
    wicedfs_usize_t location;
    char buffer[lorem_len];


    /* seek to near end of file */
    result = wicedfs_fseek( &file_handle, readpos, SEEK_SET );
    EXPECT_EQ( 0, result );

    /* check current location */
    location = wicedfs_ftell( &file_handle );
    EXPECT_EQ( readpos, location );

    /* read past end of file - should succeed with less bytes */
    num_read = wicedfs_fread( buffer, 1, sizeof(buffer), &file_handle );
    EXPECT_EQ( readlen, num_read );

    /* should now be EOF */
    result = wicedfs_feof( &file_handle );
    EXPECT_NE( 0, result );

    /* check current location */
    location = wicedfs_ftell( &file_handle );
    EXPECT_EQ( lorem_len, location );

    /* compare content */
    result = memcmp( buffer, &lorem[readpos], (size_t)readlen );
    EXPECT_EQ( 0, result );

    /* read whilst EOF - should succeed with zero bytes */
    num_read = wicedfs_fread( buffer, 1, sizeof(buffer), &file_handle );
    EXPECT_EQ( (wicedfs_usize_t) 0, num_read);

    /* seek back to start and reset eof flag */
    result = wicedfs_fseek( &file_handle, 0, SEEK_SET );
    EXPECT_EQ( 0, result );

    /* should NOT be EOF */
    result = wicedfs_feof( &file_handle );
    EXPECT_EQ( 0, result );

    /* check current location */
    location = wicedfs_ftell( &file_handle );
    EXPECT_EQ( (wicedfs_usize_t) 0, location );
}

/* read whole file*/
TEST_F( unit_test_wicedfs_fread_ftell_feof_fseek, read_whole_file )
{
    int result;
    wicedfs_usize_t num_read;
    wicedfs_usize_t location;
    char buffer[lorem_len];

    /* read whole file - should succeed but NOT set eof */
    num_read = wicedfs_fread( buffer, 1, lorem_len, &file_handle );
    EXPECT_EQ( lorem_len, num_read );

    /* should NOT be EOF */
    result = wicedfs_feof( &file_handle );
    EXPECT_EQ( 0, result );

    /* check current location */
    location = wicedfs_ftell( &file_handle );
    EXPECT_EQ( lorem_len, location );
}

/* seeking with different whence parameters */
TEST_F( unit_test_wicedfs_fread_ftell_feof_fseek, seek_whence )
{
    int result;
    wicedfs_usize_t location;


    /* seek from end of file backward 10 bytes */
    result = wicedfs_fseek( &file_handle, -10, SEEK_END );
    EXPECT_EQ( 0, result );

    /* check current location */
    location = wicedfs_ftell( &file_handle );
    EXPECT_EQ( lorem_len - 10, location );

    /* seek backward 5 bytes */
    result = wicedfs_fseek( &file_handle, -5, SEEK_CUR );
    EXPECT_EQ( 0, result );

    /* check current location */
    location = wicedfs_ftell( &file_handle );
    EXPECT_EQ( lorem_len - 15, location );

}

/* read with count greater than one */
TEST_F( unit_test_wicedfs_fread_ftell_feof_fseek, read_count )
{
    wicedfs_usize_t num_read;
    char buffer[lorem_len];

    /* read 4 bytes in two items - should return 2 */
    num_read = wicedfs_fread( buffer, 2, 2, &file_handle );
    EXPECT_EQ( (wicedfs_usize_t) 2, num_read );
}


/* read with null handle */
TEST_F( unit_test_wicedfs_fread_ftell_feof_fseek, read_null_handle )
{
    wicedfs_usize_t num_read;
    char buffer[lorem_len];

    num_read = wicedfs_fread( buffer, 1, lorem_len, NULL );
    EXPECT_EQ( (wicedfs_usize_t) 0, num_read );
}

/* read with null buffer */
TEST_F( unit_test_wicedfs_fread_ftell_feof_fseek, read_null_buffer )
{
    wicedfs_usize_t num_read;
    num_read = wicedfs_fread( NULL, 1, lorem_len, &file_handle );
    EXPECT_EQ( (wicedfs_usize_t) 0, num_read );
}


/* ftell with null handle */
TEST_F( unit_test_wicedfs_fread_ftell_feof_fseek, ftell_null_handle )
{
    wicedfs_usize_t location;
    location = wicedfs_ftell( NULL );
    EXPECT_EQ( (wicedfs_usize_t) 0, location );
}


/* seek with null handle */
TEST_F( unit_test_wicedfs_fread_ftell_feof_fseek, fseek_null_handle )
{
    int result;
    result = wicedfs_fseek( NULL, -10, SEEK_END );
    EXPECT_NE( 0, result );
}

/* seek before start of file */
TEST_F( unit_test_wicedfs_fread_ftell_feof_fseek, seek_before_file_start )
{
    int result;
    result = wicedfs_fseek( &file_handle, -10, SEEK_SET );
    EXPECT_NE( 0, result );
}


/* seek before after end of file */
TEST_F( unit_test_wicedfs_fread_ftell_feof_fseek, seek_after_file_end )
{
    int result;
    result = wicedfs_fseek( &file_handle, 10, SEEK_END );
    EXPECT_NE( 0, result );
}


/* seek with bad whence value - should fail */
TEST_F( unit_test_wicedfs_fread_ftell_feof_fseek, seek_with_bad_whence )
{
    int result;
    result = wicedfs_fseek( &file_handle, -10, SEEK_END + 100 );
    EXPECT_NE( 0, result );
}



/* feof with null handle*/
TEST_F( unit_test_wicedfs_fread_ftell_feof_fseek, feof_null_handle )
{
    int result;
    result = wicedfs_feof( NULL );
    EXPECT_GT( 0, result );
}


/* Bad reads from hardware */
TEST_F(unit_test_wicedfs_fread_ftell_feof_fseek, bad_reads )
{
    char buffer[lorem_len];
    wicedfs_usize_t num_read;

    RETCHK_START( bad_read_countdown )
    {
        num_read = wicedfs_fread( buffer, 1, lorem_len, &file_handle );
        RETCHK_EXPECT_EQ( (wicedfs_usize_t) 0, num_read );
    }
    RETCHK_END( )
}

/* Short reads from hardware */
TEST_F(unit_test_wicedfs_fread_ftell_feof_fseek, short_reads )
{
    char buffer[lorem_len];
    wicedfs_usize_t num_read;

    RETCHK_START( short_read_countdown )
    {
        num_read = wicedfs_fread( buffer, 1, lorem_len, &file_handle );
        RETCHK_EXPECT_EQ( (wicedfs_usize_t) 0, num_read );
    }
    RETCHK_END( )
}



/* File Corruption */
TEST_F(unit_test_wicedfs_fread_ftell_feof_fseek, file_corruption )
{
    int result;
    char buffer[lorem_len];
    wicedfs_usize_t num_read;

    RETCHK_START( corruption_countdown )
    {
        num_read = wicedfs_fread( buffer, 1, lorem_len, &file_handle );
        RETCHK_EXPECT_EQ( (wicedfs_usize_t) lorem_len, num_read );   /* corruption should only affect read data, not function operation */
        result = wicedfs_fseek( &file_handle, 0, SEEK_SET );
        RETCHK_EXPECT_EQ( 0, result );
    }
    RETCHK_END( )
}






/******************************************************
 *            wicedfs_filesize
 ******************************************************/

class unit_test_wicedfs_filesize :  public ::testing::Test
{

    protected:

    wicedfs_filesystem_t fs_handle;

    virtual void SetUp()
    {
        /* Setup - Initialise WicedFS */
        wicedfs_init( (wicedfs_usize_t) 0, testimage_wicedfs_read, &fs_handle, NULL );
    }
    virtual void TearDown()
    {
    }
};




/* get filesize of valid file */
TEST_F( unit_test_wicedfs_filesize, valid_file )
{
    wicedfs_ssize_t filesize;
    filesize = wicedfs_filesize( &fs_handle, TEST_FILENAME );
    EXPECT_EQ( (wicedfs_ssize_t) lorem_len, filesize );
}

/* get filesize of missing file */
TEST_F( unit_test_wicedfs_filesize, missing_file )
{
    wicedfs_ssize_t filesize;
    filesize = wicedfs_filesize( &fs_handle, GARBAGE_FILENAME );
    EXPECT_GT( (wicedfs_ssize_t) 0, filesize );
}


/* get filesize with blank filename */
TEST_F( unit_test_wicedfs_filesize, blank_filename )
{
    wicedfs_ssize_t filesize;
    filesize = wicedfs_filesize( &fs_handle, "" );
    EXPECT_GT( (wicedfs_ssize_t) 0, filesize );
}



/* get filesize with null filename */
TEST_F( unit_test_wicedfs_filesize, null_filename )
{
    wicedfs_ssize_t filesize;
    filesize = wicedfs_filesize( &fs_handle, NULL );
    EXPECT_GT( (wicedfs_ssize_t) 0, filesize );
}

/* get filesize with null handle */
TEST_F( unit_test_wicedfs_filesize, null_handle )
{
    wicedfs_ssize_t filesize;
    filesize = wicedfs_filesize( NULL, TEST_FILENAME );
    EXPECT_GT( (wicedfs_ssize_t) 0, filesize );
}



/* Bad reads from hardware */
TEST_F(unit_test_wicedfs_filesize, bad_reads )
{
    wicedfs_ssize_t filesize;

    RETCHK_START( bad_read_countdown )
    {
        filesize = wicedfs_filesize( &fs_handle, TEST_FILENAME );
        RETCHK_EXPECT_GT( (wicedfs_ssize_t) 0, filesize );
    }
    RETCHK_END( )
}

/* Short reads from hardware */
TEST_F(unit_test_wicedfs_filesize, short_reads )
{
    wicedfs_ssize_t filesize;

    RETCHK_START( short_read_countdown )
    {
        filesize = wicedfs_filesize( &fs_handle, TEST_FILENAME );
        RETCHK_EXPECT_GT( (wicedfs_ssize_t) 0, filesize );
    }
    RETCHK_END( )
}



/* File Corruption */
TEST_F(unit_test_wicedfs_filesize, file_corruption )
{
    wicedfs_ssize_t filesize;

    RETCHK_START( corruption_countdown )
    {
        filesize = wicedfs_filesize( &fs_handle, TEST_FILENAME );
        if ( RETCHK_COUNT() <= BASE_SIZE )
        {
            RETCHK_EXPECT_GT( 0, filesize );
        }
        else
        {
            RETCHK_EXPECT_EQ( (wicedfs_ssize_t) lorem_len, filesize );
        }
    }
    RETCHK_END( )
}





/******************************************************
 *            wicedfs_opendir
 ******************************************************/

class unit_test_wicedfs_opendir :  public ::testing::Test
{

    protected:

    wicedfs_filesystem_t fs_handle;

    virtual void SetUp()
    {
        /* Setup - Initialise WicedFS */
        wicedfs_init( (wicedfs_usize_t) 0, testimage_wicedfs_read, &fs_handle, NULL );
    }
    virtual void TearDown()
    {
    }
};



/* Open root directory */
TEST_F( unit_test_wicedfs_opendir, root_dir )
{
    int result;
    wicedfs_dir_t dir_handle;
    result = wicedfs_opendir( &fs_handle, &dir_handle, DIRECTORY_SEPARATOR_STR );
    EXPECT_EQ( 0, result );

    result = wicedfs_closedir( &dir_handle );
    EXPECT_EQ( 0, result );
}

/* Open sub directory */
TEST_F( unit_test_wicedfs_opendir, sub_dir )
{
    int result;
    wicedfs_dir_t dir_handle;
    result = wicedfs_opendir( &fs_handle, &dir_handle, TEST2_DIR );
    EXPECT_EQ( 0, result );

    result = wicedfs_closedir( &dir_handle );
    EXPECT_EQ( 0, result );
}


/* Open sub directory with leading slash */
TEST_F( unit_test_wicedfs_opendir, subdir_leading_slash )
{
    int result;
    wicedfs_dir_t dir_handle;
    result = wicedfs_opendir( &fs_handle, &dir_handle, DIRECTORY_SEPARATOR_STR  TEST2_DIR );
    EXPECT_EQ( 0, result );

    result = wicedfs_closedir( &dir_handle );
    EXPECT_EQ( 0, result );
}


/* Open sub directory with trailing slash */
TEST_F( unit_test_wicedfs_opendir, subdir_trailing_slash )
{
    int result;
    wicedfs_dir_t dir_handle;
    result = wicedfs_opendir( &fs_handle, &dir_handle, TEST2_DIR  DIRECTORY_SEPARATOR_STR );
    EXPECT_EQ( 0, result );

    result = wicedfs_closedir( &dir_handle );
    EXPECT_EQ( 0, result );
}


/* Open garbage sub directory */
TEST_F( unit_test_wicedfs_opendir, garbage_dir )
{
    int result;
    wicedfs_dir_t dir_handle;
    result = wicedfs_opendir( &fs_handle, &dir_handle, GARBAGE_FILENAME );
    EXPECT_GT( 0, result );
}


/* Opendir with blank directory name */
TEST_F( unit_test_wicedfs_opendir, blank_dir )
{
    int result;
    wicedfs_dir_t dir_handle;
    result = wicedfs_opendir( &fs_handle, &dir_handle, "" );
    EXPECT_GT( 0, result );
}

/* Opendir with NULL directory name */
TEST_F( unit_test_wicedfs_opendir, null_dir )
{
    int result;
    wicedfs_dir_t dir_handle;
    result = wicedfs_opendir( &fs_handle, &dir_handle, NULL );
    EXPECT_GT( 0, result );
}

/* Opendir with NULL fs handle */
TEST_F( unit_test_wicedfs_opendir, null_fs_handle )
{
    int result;
    wicedfs_dir_t dir_handle;
    result = wicedfs_opendir( NULL, &dir_handle, TEST2_DIR );
    EXPECT_GT( 0, result );
}


/* Opendir with NULL dir handle */
TEST_F( unit_test_wicedfs_opendir, null_dir_handle )
{
    int result;
    result = wicedfs_opendir( &fs_handle, NULL, TEST2_DIR );
    EXPECT_GT( 0, result );
}

/* closedir with NULL dir handle */
TEST_F( unit_test_wicedfs_opendir, closedir_null_dir_handle )
{
    int result;
    result = wicedfs_closedir( NULL );
    EXPECT_GT( 0, result );
}



/* Bad reads from hardware */
TEST_F(unit_test_wicedfs_opendir, bad_reads )
{
    wicedfs_dir_t dir_handle;
    int result;

    RETCHK_START( bad_read_countdown )
    {
        result = wicedfs_opendir( &fs_handle, &dir_handle, TEST2_DIR );
        RETCHK_EXPECT_GT( 0, result );
    }
    RETCHK_END( )

    result = wicedfs_closedir( &dir_handle );
    EXPECT_EQ( 0, result );

}

/* Short reads from hardware */
TEST_F(unit_test_wicedfs_opendir, short_reads )
{
    wicedfs_dir_t dir_handle;
    int result;

    RETCHK_START( short_read_countdown )
    {
        result = wicedfs_opendir( &fs_handle, &dir_handle, TEST2_DIR );
        RETCHK_EXPECT_GT( 0, result );
    }
    RETCHK_END( )

    result = wicedfs_closedir( &dir_handle );
    EXPECT_EQ( 0, result );
}



/* File Corruption */
TEST_F(unit_test_wicedfs_opendir, file_corruption )
{
    wicedfs_dir_t dir_handle;
    int result;

    RETCHK_START( corruption_countdown )
    {
        result = wicedfs_opendir( &fs_handle, &dir_handle, TEST2_DIR );
        if ( RETCHK_COUNT() <= BASE_SIZE )
        {
            RETCHK_EXPECT_GT( 0, result );
        }
        else if ( RETCHK_COUNT() <= BASE_SIZE * 2 )
        {
            RETCHK_EXPECT_EQ( 0, result );
        }
        else if ( RETCHK_COUNT() <= BASE_SIZE * 2 + 4 )
        {
            RETCHK_EXPECT_GT( 0, result );
        }
        else
        {
            RETCHK_EXPECT_EQ( 0, result );
        }
    }
    RETCHK_END( )

    result = wicedfs_closedir( &dir_handle );
    EXPECT_EQ( 0, result );
}




/******************************************************
 *            wicedfs_readdir_eodir_rewind
 ******************************************************/

class unit_test_wicedfs_readdir_eodir_rewind :  public ::testing::Test
{

    protected:

    wicedfs_filesystem_t fs_handle;
    wicedfs_dir_t dir_handle;

    virtual void SetUp()
    {
        /* Setup - Initialise WicedFS */
        wicedfs_init( (wicedfs_usize_t) 0, testimage_wicedfs_read, &fs_handle, NULL );
        wicedfs_opendir( &fs_handle, &dir_handle, DIRECTORY_SEPARATOR_STR );
    }
    virtual void TearDown()
    {
        wicedfs_closedir( &dir_handle );
    }
};


/* Read entries normally */
TEST_F( unit_test_wicedfs_readdir_eodir_rewind, normal )
{
    int result;
    wicedfs_entry_type_t type;
    char  filename[100];

    /* Read first entry */
    result = wicedfs_readdir( &dir_handle, filename, sizeof(filename), &type );
    EXPECT_EQ( 0, result );

    result = strcmp( filename, TEST_FILENAME );
    EXPECT_EQ( 0, result );
    EXPECT_EQ( WICEDFS_FILE, type );

    /* Read second entry */
    result = wicedfs_readdir( &dir_handle, filename, sizeof(filename), &type );
    EXPECT_EQ( 0, result );

    result = strcmp( filename, TEST2_DIR );
    EXPECT_EQ( 0, result );
    EXPECT_EQ( WICEDFS_DIR, type );

    /* should now indicate end of directory */
    result = wicedfs_eodir( &dir_handle );
    EXPECT_NE( 0, result );

    /* Read off end of directory - should fail */
    result = wicedfs_readdir( &dir_handle, filename, sizeof(filename), &type );
    EXPECT_NE( 0, result );

    /* Rewind directory */
    wicedfs_rewinddir( &dir_handle );

    /* Read first entry again */
    result = wicedfs_readdir( &dir_handle, filename, sizeof(filename), &type );
    EXPECT_EQ( 0, result );

    result = strcmp( filename, TEST_FILENAME );
    EXPECT_EQ( 0, result );
    EXPECT_EQ( WICEDFS_FILE, type );
}




/* Read with buffer too small */
TEST_F( unit_test_wicedfs_readdir_eodir_rewind, buffer_too_small )
{
    int result;
    wicedfs_entry_type_t type;
    char  filename[100];

    result = wicedfs_readdir( &dir_handle, filename, 2, &type );
    EXPECT_NE( 0, result );
}


/* Read with null dir handle */
TEST_F( unit_test_wicedfs_readdir_eodir_rewind, null_dir_handle )
{
    int result;
    wicedfs_entry_type_t type;
    char  filename[100];

    result = wicedfs_readdir( NULL, filename, sizeof(filename), &type );
    EXPECT_NE( 0, result );
}



/* Read with null buffer pointer */
TEST_F( unit_test_wicedfs_readdir_eodir_rewind, null_buffer_pointer )
{
    int result;
    wicedfs_entry_type_t type;

    result = wicedfs_readdir( &dir_handle, NULL, 100, &type );
    EXPECT_NE( 0, result );
}


/* Read with null type pointer */
TEST_F( unit_test_wicedfs_readdir_eodir_rewind, null_type_pointer )
{
    int result;
    char  filename[100];

    result = wicedfs_readdir( &dir_handle, filename, sizeof(filename), NULL );
    EXPECT_NE( 0, result );
}


/* eodir with null pointer */
TEST_F( unit_test_wicedfs_readdir_eodir_rewind, eodir_null_pointer )
{
    int result;

    result = wicedfs_eodir( NULL );
    EXPECT_EQ( 0, result );
}

/* Rewind directory with null pointer */
TEST_F( unit_test_wicedfs_readdir_eodir_rewind, rewind_null_pointer )
{
    wicedfs_rewinddir( NULL );

    /* returns void */
}


/* Bad reads from hardware */
TEST_F(unit_test_wicedfs_readdir_eodir_rewind, bad_reads )
{
    int result;
    wicedfs_entry_type_t type;
    char  filename[100];

    RETCHK_START( bad_read_countdown )
    {
        result = wicedfs_readdir( &dir_handle, filename, sizeof(filename), &type );
        RETCHK_EXPECT_GT( 0, result );
    }
    RETCHK_END( )
}

/* Short reads from hardware */
TEST_F(unit_test_wicedfs_readdir_eodir_rewind, short_reads )
{
    int result;
    wicedfs_entry_type_t type;
    char  filename[100];

    RETCHK_START( short_read_countdown )
    {
        result = wicedfs_readdir( &dir_handle, filename, sizeof(filename), &type );
        RETCHK_EXPECT_GT( 0, result );
    }
    RETCHK_END( )
}



/* File Corruption */
TEST_F(unit_test_wicedfs_readdir_eodir_rewind, file_corruption )
{
    int result;
    wicedfs_entry_type_t type;
    char  filename[100];

    RETCHK_START( corruption_countdown )
    {
        result = wicedfs_readdir( &dir_handle, filename, sizeof(filename), &type );
        RETCHK_EXPECT_EQ( 0, result ); /* corruption doesn't affect readdir */
    }
    RETCHK_END( )
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
static wicedfs_usize_t testimage_wicedfs_read( void* user_param, void* buf, wicedfs_usize_t size, wicedfs_usize_t pos )
{
    wicedfs_usize_t copy_size = 0;

    user_param_received = user_param;

    if ( bad_read_countdown == 0 )
    {
        bad_read_countdown--;
        return 0;
    }
    if ( bad_read_countdown != -1 )
    {
        bad_read_countdown--;
    }

    if ( short_read_countdown == 0 )
    {
        short_read_countdown--;
        return (size>0)? size-1 : 0;
    }
    if ( short_read_countdown != -1 )
    {
        short_read_countdown--;
    }

    if ( (pos >= 0 ) &&
         ( pos < fs_test_img_size ) )
    {
        copy_size = MIN( size, fs_test_img_size - pos );
        memcpy( buf, &fs_test_img[pos], copy_size );

    }

    if ( ( corruption_countdown >= 0 ) &&
         ( corruption_countdown < (int)size ) )
    {
        ((unsigned char*)buf)[ corruption_countdown ] |= (unsigned char)0xa5;
        corruption_countdown = -1;
    }
    if ( corruption_countdown != -1 )
    {
        short_read_countdown -= (int)size;
    }

    return copy_size;
}




