/*
 * Amazon FreeRTOS CBOR Library V1.0.0
 * Copyright (C) 2018 Amazon.com, Inc. or its affiliates.  All Rights Reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of
 * this software and associated documentation files (the "Software"), to deal in
 * the Software without restriction, including without limitation the rights to
 * use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
 * the Software, and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 * http://aws.amazon.com/freertos
 * http://www.FreeRTOS.org
 */
#include "assert_override.h"
#include "aws_cbor_internals.h"
#include "unity_fixture.h"
#include <assert.h>
#include <string.h>

cbor_handle_t cbor_data;

TEST_GROUP( aws_cbor_map );

TEST_SETUP( aws_cbor_map )
{
    cbor_data = CBOR_New( 0 );
}

TEST_TEAR_DOWN( aws_cbor_map )
{
    CBOR_Delete( &cbor_data );
}

TEST_GROUP_RUNNER( aws_cbor_map )
{
    RUN_TEST_CASE( aws_cbor_map, AssignKey_writes_byte_to_key );
    RUN_TEST_CASE( aws_cbor_map, AssignKey_update_key_with_larger_int );
    RUN_TEST_CASE( aws_cbor_map, AssignKey_update_key_with_smaller_int );
    RUN_TEST_CASE( aws_cbor_map, AssignKey_will_return_err_if_not_map_or_empty );
    RUN_TEST_CASE( aws_cbor_map, AssignKey_with_string );
    RUN_TEST_CASE( aws_cbor_map, AssignKey_with_map );

    RUN_TEST_CASE( aws_cbor_map, FindKey_read_string_at_1_char_key );
    RUN_TEST_CASE( aws_cbor_map, FindKey_read_int_at_longer_key );
    RUN_TEST_CASE( aws_cbor_map, FindKey_returns_true_when_key_is_found );
    RUN_TEST_CASE( aws_cbor_map, FindKey_returns_false_when_key_not_found );

    RUN_TEST_CASE( aws_cbor_map, WriteMap );

    RUN_TEST_CASE( aws_cbor_map, ReadMap_returns_cbor_object_with_map_at_cursor );
    RUN_TEST_CASE( aws_cbor_map, ReadMap_returned_map_has_correct_map_end );
    RUN_TEST_CASE( aws_cbor_map, ReadMap_sets_err_when_wrong_type );
    RUN_TEST_CASE( aws_cbor_map, ReadMap_sets_err_when_new_fails );

    RUN_TEST_CASE( aws_cbor_map, AppendKey );

    RUN_TEST_CASE( aws_cbor_map, null_checks );
}

TEST( aws_cbor_map, AssignKey_writes_byte_to_key )
{
    cbor_byte_t expected[] =
    {
        0xBF, /* 0  Open Map         */
        0x66, /* 1  Key of length 6  */
        'a',  /* 2                   */
        'n',  /* 3                   */
        's',  /* 4                   */
        'w',  /* 5                   */
        'e',  /* 6                   */
        'r',  /* 7                   */
        0x18, /* 8  Start 8-bit int  */
        42,   /* 9                   */
        0xFF, /* 10 End of map       */
    };

    cbor_int_t my_answer = 42;

    CBOR_AssignKey( cbor_data, "answer", CBOR_WriteInt, &my_answer );
    TEST_ASSERT_EQUAL( eCBOR_ERR_NO_ERROR, cbor_data->err );

    TEST_ASSERT_EQUAL_HEX8_ARRAY(
        expected, cbor_data->buffer_start, sizeof( expected ) );
}

TEST( aws_cbor_map, AssignKey_update_key_with_larger_int )
{
    CBOR_AssignKey( cbor_data, "1", CBOR_WriteInt, &( cbor_int_t ) { 0x23 } );
    TEST_ASSERT_EQUAL( eCBOR_ERR_NO_ERROR, cbor_data->err );

    CBOR_AssignKey( cbor_data, "2", CBOR_WriteInt, &( cbor_int_t ) { 0x7392 } );
    TEST_ASSERT_EQUAL( eCBOR_ERR_NO_ERROR, cbor_data->err );

    /* Note, this test assert is here for documentation.  It demonstrates the
     * state of the CBOR buffer before re-writing the value for key '1'. */
    cbor_byte_t expected[] =
    {
        0xBF, /* 0  Open Map         */
        0x61, /* 1  Key of length 6  */
        0x31, /* 2 '1'               */
        0x18, /* 3  Start 8-bit int  */
        0x23, /* 4                   */
        0x61, /* 5  Key of length 6  */
        0x32, /* 6 '2'               */
        0x19, /* 7  Start 16-bit int */
        0x73, /* 8                   */
        0x92, /* 9                   */
        0xFF, /* 10 End of map       */
    };
    TEST_ASSERT_EQUAL_HEX8_ARRAY(
        expected, cbor_data->buffer_start, sizeof( expected ) );

    CBOR_AssignKey( cbor_data, "1", CBOR_WriteInt, &( cbor_int_t ) { 0x1234 } );
    TEST_ASSERT_EQUAL( eCBOR_ERR_NO_ERROR, cbor_data->err );

    cbor_byte_t expected_rewrite[] =
    {
        0xBF, /* 0  Open Map         */
        0x61, /* 1  Key of length 6  */
        0x31, /* 2 '1'               */
        0x19, /* 3  Start 16-bit int */
        0x12, /* 4                   */
        0x34, /* 5                   */
        0x61, /* 6  Key of length 6  */
        0x32, /* 7 '2'               */
        0x19, /* 8  Start 16-bit int */
        0x73, /* 9                   */
        0x92, /* 10                  */
        0xFF, /* 11 End of map       */
    };
    TEST_ASSERT_EQUAL_HEX8_ARRAY(
        expected_rewrite, cbor_data->buffer_start, sizeof( expected_rewrite ) );
}

TEST( aws_cbor_map, AssignKey_update_key_with_smaller_int )
{
    CBOR_AssignKey( cbor_data, "1", CBOR_WriteInt, &( cbor_int_t ) { 0x5CA1AB1E } );
    TEST_ASSERT_EQUAL( eCBOR_ERR_NO_ERROR, cbor_data->err );

    CBOR_AssignKey( cbor_data, "2", CBOR_WriteInt, &( cbor_int_t ) { 0xCAFE } );
    TEST_ASSERT_EQUAL( eCBOR_ERR_NO_ERROR, cbor_data->err );

    CBOR_AssignKey( cbor_data, "1", CBOR_WriteInt, &( cbor_int_t ) { 0xED } );
    TEST_ASSERT_EQUAL( eCBOR_ERR_NO_ERROR, cbor_data->err );

    cbor_byte_t expected_rewrite[] =
    {
        0xBF, /* 0  Open Map         */
        0x61, /* 1  Key of length 6  */
        0x31, /* 2 '1'               */
        0x18, /* 3  Start 8-bit int  */
        0xED, /* 4                   */
        0x61, /* 5  Key of length 6  */
        0x32, /* 6 '2'               */
        0x19, /* 7  Start 16-bit int */
        0xCA, /* 8                   */
        0xFE, /* 9                   */
        0xFF, /* 10 End of map       */
    };
    TEST_ASSERT_EQUAL_HEX8_ARRAY(
        expected_rewrite, cbor_data->buffer_start, sizeof( expected_rewrite ) );
}

TEST( aws_cbor_map, AssignKey_will_return_err_if_not_map_or_empty )
{
    *( cbor_data->buffer_start ) = 2;
    CBOR_AssignKey( cbor_data, "1", CBOR_WriteInt, &( cbor_int_t ) { 0x5CA1AB1E } );
    TEST_ASSERT_EQUAL( eCBOR_ERR_UNSUPPORTED_READ_OPERATION, cbor_data->err );
}

TEST( aws_cbor_map, AssignKey_with_string )
{
    CBOR_AssignKey( cbor_data, "1", CBOR_WriteString, "Hi" );

    cbor_byte_t expected_rewrite[] =
    {
        0xBF, /* 0  Open Map           */
        0x61, /* 1  Key of length 1    */
        0x31, /* 2 '1'                 */
        0x62, /* 3  String of length 2 */
        'H',  /* 4                     */
        'i',  /* 5                     */
        0xFF, /* 6  End of map         */
    };
    TEST_ASSERT_EQUAL_HEX8_ARRAY(
        expected_rewrite, cbor_data->buffer_start, sizeof( expected_rewrite ) );
}

TEST( aws_cbor_map, AssignKey_with_map )
{
    cbor_byte_t map_buffer[] =
    {
        0xBF, /* 0  Open Map           */
        0x63, /* 1  Key of length 3    */
        'k',  /* 2                     */
        'e',  /* 3                     */
        'y',  /* 4                     */
        0x18, /* 5  1 byte int         */
        78,   /* 6                     */
        0xFF, /* 7  End of map         */
    };

    cbor_handle_t map = CBOR_New( 0 );

    CBOR_MemCopy( map, map_buffer, sizeof( map_buffer ) );
    CBOR_AssignKey( cbor_data, "1", CBOR_WriteMap, map );
    CBOR_Delete( &map );

    cbor_byte_t expected_rewrite[] =
    {
        0xBF, /* 0  Open Map        */
        0x61, /* 1  Key of length 1 */
        0x31, /* 2' 1'              */
        0xBF, /* 3  Open Map        */
        0x63, /* 4  Key of length 3 */
        'k',  /* 5                  */
        'e',  /* 6                  */
        'y',  /* 7                  */
        0x18, /* 8  1 byte int      */
        78,   /* 9                  */
        0xFF, /* 10 End of map      */
        0xFF, /* 11 End of map      */
    };
    TEST_ASSERT_EQUAL_HEX8_ARRAY(
        expected_rewrite, cbor_data->buffer_start, sizeof( expected_rewrite ) );
}

TEST( aws_cbor_map, FindKey_read_string_at_1_char_key )
{
    cbor_byte_t buffer[] =
    {
        0xBF, /* 0  Open Map           */
        0x61, /* 1  Key of length 1    */
        '1',  /* 2                     */
        0x62, /* 3  String of length 2 */
        'H',  /* 4                     */
        'i',  /* 5                     */
        0xFF, /* 6  End of map         */
    };

    CBOR_SetCursor( cbor_data, 0 );
    CBOR_MemCopy( cbor_data, buffer, sizeof( buffer ) );
    CBOR_FindKey( cbor_data, "1" );
    char * str = CBOR_ReadString( cbor_data );
    TEST_ASSERT_EQUAL_STRING( "Hi", str );
    free( str );
}

TEST( aws_cbor_map, FindKey_read_int_at_longer_key )
{
    cbor_byte_t buffer[] =
    {
        0xBF, /* 0  Open Map           */
        0x63, /* 1  Key of length 3    */
        'k',  /* 2                     */
        'e',  /* 3                     */
        'y',  /* 4                     */
        0x18, /* 5  1 byte int         */
        78,   /* 6                     */
        0xFF, /* 7  End of map         */
    };

    CBOR_SetCursor( cbor_data, 0 );
    CBOR_MemCopy( cbor_data, buffer, sizeof( buffer ) );
    CBOR_FindKey( cbor_data, "key" );
    cbor_int_t result = CBOR_ReadInt( cbor_data );
    TEST_ASSERT_EQUAL_INT( 78, result );
}

TEST( aws_cbor_map, FindKey_returns_true_when_key_is_found )
{
    cbor_byte_t buffer[] =
    {
        0xBF, /* 0  Open Map           */
        0x63, /* 1  Key of length 3    */
        'k',  /* 2                     */
        'e',  /* 3                     */
        'y',  /* 4                     */
        0x18, /* 5  1 byte int         */
        78,   /* 6                     */
        0xFF, /* 7  End of map         */
    };

    CBOR_SetCursor( cbor_data, 0 );
    CBOR_MemCopy( cbor_data, buffer, sizeof( buffer ) );
    bool result = CBOR_FindKey( cbor_data, "key" );
    TEST_ASSERT_TRUE( result );
}

TEST( aws_cbor_map, FindKey_returns_false_when_key_not_found )
{
    cbor_byte_t buffer[] =
    {
        0xBF, /* 0  Open Map           */
        0x63, /* 1  Key of length 3    */
        'k',  /* 2                     */
        'e',  /* 3                     */
        'y',  /* 4                     */
        0x18, /* 5  1 byte int         */
        78,   /* 6                     */
        0xFF, /* 7  End of map         */
    };

    CBOR_SetCursor( cbor_data, 0 );
    CBOR_MemCopy( cbor_data, buffer, sizeof( buffer ) );
    cbor_data->map_end = cbor_data->cursor - 1;
    bool result = CBOR_FindKey( cbor_data, "lock" );
    TEST_ASSERT_FALSE( result );
}

TEST( aws_cbor_map, WriteMap )
{
    cbor_byte_t map_buffer[] =
    {
        0xBF, /* 0  Open Map           */
        0x63, /* 1  Key of length 3    */
        'k',  /* 2                     */
        'e',  /* 3                     */
        'y',  /* 4                     */
        0x18, /* 5  1 byte int         */
        78,   /* 6                     */
        0xFF, /* 7  End of map         */
    };
    cbor_handle_t map = CBOR_New( 0 );

    CBOR_MemCopy( map, map_buffer, sizeof( map_buffer ) );
    CBOR_WriteMap( cbor_data, map );
    CBOR_Delete( &map );
    TEST_ASSERT_EQUAL_HEX8_ARRAY(
        map_buffer, cbor_data->buffer_start, sizeof( map_buffer ) );
}

TEST( aws_cbor_map, ReadMap_returns_cbor_object_with_map_at_cursor )
{
    cbor_byte_t full_buffer[] =
    {
        0xBF, /* 0  Open Map          */
        0x61, /* 1  Key of length 1   */
        'z',  /* 2                    */
        0xBF, /* 3  Open Map          */
        0x61, /* 4  Key of length 1   */
        'z',  /* 5                    */
        0x18, /* 6  1 byte int        */
        87,   /* 7                    */
        0xFF, /* 8  End of map        */
        0x18, /* 9  1 byte int        */
        78,   /* 10                   */
        0xFF, /* 11 End of map        */
    };

    CBOR_MemCopy( cbor_data, full_buffer, sizeof( full_buffer ) );
    cbor_data->map_end = cbor_data->buffer_start + sizeof( full_buffer );
    CBOR_FindKey( cbor_data, "z" );
    cbor_handle_t map = CBOR_ReadMap( cbor_data );

    CBOR_FindKey( map, "z" );
    cbor_int_t result = CBOR_ReadInt( map );
    TEST_ASSERT_EQUAL( 87, result );

    CBOR_Delete( &map );
}

TEST( aws_cbor_map, ReadMap_returned_map_has_correct_map_end )
{
    cbor_byte_t full_buffer[] =
    {
        0xBF, /* 0  Open Map          */
        0x61, /* 1  Key of length 1   */
        'z',  /* 2                    */
        0xBF, /* 3  Open Map          */
        0x61, /* 4  Key of length 1   */
        'y',  /* 5                    */
        22,   /* 6  small int (22)    */
        0xFF, /* 7  End of map        */
        0x18, /* 8  1 byte int        */
        78,   /* 9                    */
        0xFF, /* 10 End of map        */
    };

    CBOR_MemCopy( cbor_data, full_buffer, sizeof( full_buffer ) );
    cbor_data->map_end = cbor_data->buffer_start + sizeof( full_buffer );
    CBOR_FindKey( cbor_data, "z" );
    cbor_handle_t map = CBOR_ReadMap( cbor_data );

    cbor_int_t expected = 4;
    cbor_int_t actual = map->map_end - map->buffer_start;
    TEST_ASSERT_EQUAL( expected, actual );

    CBOR_Delete( &map );
}

TEST( aws_cbor_map, ReadMap_sets_err_when_wrong_type )
{
    CBOR_AssignKeyWithInt( cbor_data, "1", 834 );
    CBOR_FindKey( cbor_data, "1" );
    ( void ) CBOR_ReadMap( cbor_data );
    TEST_ASSERT_EQUAL( eCBOR_ERR_READ_TYPE_MISMATCH, cbor_data->err );
}

TEST( aws_cbor_map, ReadMap_sets_err_when_new_fails )
{
    cbor_byte_t full_buffer[] =
    {
        0xBF, /* 0  Open Map          */
        0x61, /* 1  Key of length 1   */
        'z',  /* 2                    */
        0xBF, /* 3  Open Map          */
        0x61, /* 4  Key of length 1   */
        'z',  /* 5                    */
        0x18, /* 6  1 byte int        */
        87,   /* 7                    */
        0xFF, /* 8  End of map        */
        0x18, /* 9  1 byte int        */
        78,   /* 10                   */
        0xFF, /* 11 End of map        */
    };

    CBOR_MemCopy( cbor_data, full_buffer, sizeof( full_buffer ) );
    cbor_data->map_end = cbor_data->buffer_start + sizeof( full_buffer );

    UnityMalloc_MakeMallocFailAfterCount( 0 );
    CBOR_FindKey( cbor_data, "z" );
    cbor_handle_t map = CBOR_ReadMap( cbor_data );
    TEST_ASSERT_NULL( map );
    TEST_ASSERT_EQUAL( eCBOR_ERR_INSUFFICENT_SPACE, cbor_data->err );
}

TEST( aws_cbor_map, AppendKey )
{
    cbor_byte_t init[] =
    {
        0xBF, /* 0  Open Map           */
        0x63, /* 1  Key of length 3    */
        'k',  /* 2                     */
        'e',  /* 3                     */
        'y',  /* 4                     */
        0x18, /* 5  1 byte int         */
        78,   /* 6                     */
        0xFF, /* 7  End of map         */
    };

    CBOR_MemCopy( cbor_data, init, sizeof( init ) );
    CBOR_SetCursor( cbor_data, 7 );
    cbor_data->map_end = cbor_data->cursor;

    CBOR_AppendKey( cbor_data, "lock", CBOR_WriteInt, &( cbor_int_t ) { 0x0347 } );

    cbor_byte_t expected[] =
    {
        0xBF, /* 0  Open Map           */
        0x63, /* 1  Key of length 3    */
        'k',  /* 2                     */
        'e',  /* 3                     */
        'y',  /* 4                     */
        0x18, /* 5  1 byte int         */
        78,   /* 6                     */
        0x64, /* 7  Key of length 4    */
        'l',  /* 8                     */
        'o',  /* 9                     */
        'c',  /* 10                    */
        'k',  /* 11                    */
        0x19, /* 12 2 byte int follows */
        0x03, /* 13                    */
        0x47, /* 14                    */
        0xFF, /* 15 End of map         */
    };

    TEST_ASSERT_EQUAL_HEX8_ARRAY(
        expected, cbor_data->buffer_start, sizeof( expected ) );
}

TEST( aws_cbor_map, null_checks )
{
    TEST_EXPECT_ASSERT( CBOR_OpenMap( NULL ) );

    TEST_EXPECT_ASSERT( CBOR_CloseMap( NULL ) );

    TEST_EXPECT_ASSERT( CBOR_KeyIsMatch( NULL, "key" ) );
    TEST_EXPECT_ASSERT( CBOR_KeyIsMatch( cbor_data, NULL ) );

    TEST_EXPECT_ASSERT( CBOR_SearchForKey( NULL, "key" ) );
    TEST_EXPECT_ASSERT( CBOR_SearchForKey( cbor_data, NULL ) );

    TEST_EXPECT_ASSERT( CBOR_AppendKey( NULL, "1", CBOR_WriteString, "a" ) );
    TEST_EXPECT_ASSERT( CBOR_AppendKey( cbor_data, NULL, CBOR_WriteString, "a" ) );
    TEST_EXPECT_ASSERT( CBOR_AppendKey( cbor_data, "1", NULL, "a" ) );
    TEST_EXPECT_ASSERT( CBOR_AppendKey( cbor_data, "1", CBOR_WriteString, NULL ) );

    TEST_EXPECT_ASSERT( CBOR_AssignKey( NULL, "1", CBOR_WriteString, "a" ) );
    TEST_EXPECT_ASSERT( CBOR_AssignKey( cbor_data, NULL, CBOR_WriteString, "a" ) );
    TEST_EXPECT_ASSERT( CBOR_AssignKey( cbor_data, "1", NULL, "a" ) );
    TEST_EXPECT_ASSERT( CBOR_AssignKey( cbor_data, "1", CBOR_WriteString, NULL ) );

    TEST_EXPECT_ASSERT( CBOR_MapSize( NULL ) );

    TEST_EXPECT_ASSERT( CBOR_WriteMap( NULL, cbor_data ) );
    TEST_EXPECT_ASSERT( CBOR_WriteMap( cbor_data, NULL ) );

    TEST_EXPECT_ASSERT( CBOR_ReadMap( NULL ) );
}
