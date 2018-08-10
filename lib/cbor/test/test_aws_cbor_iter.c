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

typedef struct test_next_s
{
    cbor_byte_t buffer[ 32 ];
    cbor_int_t initial_index;
    cbor_int_t expected_index;
    char * message;
} test_next_t;

cbor_handle_t cbor_data;
const test_next_t * test_case;

TEST_GROUP( aws_cbor_iter );
TEST_SETUP( aws_cbor_iter )
{
    cbor_data = CBOR_New( 0 );
    test_case = NULL;
}

TEST_TEAR_DOWN( aws_cbor_iter )
{
    CBOR_Delete( &cbor_data );
    test_case = NULL;
}

TEST_GROUP_RUNNER( aws_cbor_iter )
{
    RUN_TEST_CASE( aws_cbor_iter, Next );
    RUN_TEST_CASE( aws_cbor_iter, Next_asserts_default_case );
    RUN_TEST_CASE( aws_cbor_iter, NextKey );
    RUN_TEST_CASE( aws_cbor_iter, SetCursor );

    RUN_TEST_CASE( aws_cbor_iter, null_checks );
}

#define NEXT_TEST_CASE_COUNT    ( 12 )
const test_next_t next_test_cases[ NEXT_TEST_CASE_COUNT ] =
{
    {
        .message = "next from open map goes to first key",
        .buffer =
        {
            0xBF,     /* 0  Open Map         */
            0x61,     /* 1  Key of length 1  */
            'a',      /* 2                   */
            0x00,     /* 3  0                */
            0xFF,     /* 4  End of map       */
        },
        .initial_index = 0,
        .expected_index = 1,
    },
    {
        .message = "next from key goes to its value",
        .buffer =
        {
            0xBF,     /* 0  Open Map         */
            0x61,     /* 1  Key of length 1  */
            'a',      /* 2                   */
            0x00,     /* 3  0                */
            0xFF,     /* 4  End of map       */
        },
        .initial_index = 1,
        .expected_index = 3,
    },
    {
        .message = "next from value goes to next key",
        .buffer =
        {
            0xBF,     /* 0  Open Map         */
            0x61,     /* 1  Key of length 1  */
            'a',      /* 2                   */
            0x00,     /* 3  0                */
            0x61,     /* 4  Key of length 1  */
            'b',      /* 5                   */
            0x00,     /* 6  0                */
            0xFF,     /* 7  End of map       */
        },
        .initial_index = 3,
        .expected_index = 4,
    },
    {
        .message = "next from last value goes to break",
        .buffer =
        {
            0xBF,     /* 0  Open Map         */
            0x61,     /* 1  Key of length 1  */
            'a',      /* 2                   */
            0x02,     /* 3  0                */
            0x61,     /* 4  Key of length 1  */
            'b',      /* 5                   */
            0x17,     /* 6  0x17, 23         */
            0xFF,     /* 7  End of map       */
        },
        .initial_index = 6,
        .expected_index = 7,
    },
    {
        .message = "1 byte cbor_int_t jump",
        .buffer =
        {
            0xBF,     /* 0  Open Map         */
            0x61,     /* 1  Key of length 1  */
            'a',      /* 2                   */
            0x18,     /* 3  1-byte uint      */
            0x41,     /* 4                   */
            0x61,     /* 5  Key of length 1  */
            'b',      /* 6                   */
            0x00,     /* 7  0                */
            0xFF,     /* 8  End of map       */
        },
        .initial_index = 3,
        .expected_index = 5,
    },
    {
        .message = "2 byte cbor_int_t jump",
        .buffer =
        {
            0xBF,     /* 0  Open Map         */
            0x61,     /* 1  Key of length 1  */
            'a',      /* 2                   */
            0x19,     /* 3  2-byte uint      */
            0xDE,     /* 4                   */
            0xA1,     /* 5                   */
            0x61,     /* 6  Key of length 1  */
            'b',      /* 7                   */
            0x00,     /* 8  0                */
            0xFF,     /* 9  End of map       */
        },
        .initial_index = 3,
        .expected_index = 6,
    },
    {
        .message = "4 byte cbor_int_t jump",
        .buffer =
        {
            0xBF,     /* 0  Open Map         */
            0x61,     /* 1  Key of length 1  */
            'a',      /* 2                   */
            0x1A,     /* 3  4-byte uint      */
            0xDE,     /* 4                   */
            0xAD,     /* 5                   */
            0xBE,     /* 6                   */
            0xEF,     /* 7                   */
            0x61,     /* 8  Key of length 1  */
            'b',      /* 9                   */
            0x00,     /* 10 0                */
            0xFF,     /* 11 End of map       */
        },
        .initial_index = 3,
        .expected_index = 8,
    },
    {
        .message = "small negative integer",
        .buffer =
        {
            0xBF,     /* 0  Open Map         */
            0x61,     /* 1  Key of length 1  */
            'a',      /* 2                   */
            0x24,     /* 3  -5               */
            0xFF,     /* 4  End of map       */
        },
        .initial_index = 1,
        .expected_index = 3,
    },
    {
        .message = "key of size 3",
        .buffer =
        {
            0xBF,     /* 0  Open Map         */
            0x63,     /* 1  Key of length 3  */
            'f',      /* 2                   */
            'o',      /* 3                   */
            'o',      /* 4                   */
            0x24,     /* 5  -5               */
            0xFF,     /* 6  End of map       */
        },
        .initial_index = 1,
        .expected_index = 5,
    },
    {
        .message = "Iterate into map",
        .buffer =
        {
            0xBF,     /* 0  Open Map         */
            0x61,     /* 1  Key of length 1  */
            'a',      /* 2                   */
            0xBF,     /* 3  Open Map         */
            0x61,     /* 4  Key of length 1  */
            'b',      /* 5                   */
            0x24,     /* 6  -5               */
            0xFF,     /* 7  End of map       */
            0x61,     /* 8  Key of length 1  */
            'b',      /* 9                   */
            0x25,     /* 10 -6               */
            0xFF,     /* 11 End of map       */
        },
        .initial_index = 1,
        .expected_index = 3,
    },
    {
        .message = "Iterate into map part 2",
        .buffer =
        {
            0xBF,     /* 0  Open Map         */
            0x61,     /* 1  Key of length 1  */
            'a',      /* 2                   */
            0xBF,     /* 3  Open Map         */
            0x61,     /* 4  Key of length 1  */
            'b',      /* 5                   */
            0x24,     /* 6  -5               */
            0xFF,     /* 7  End of map       */
            0x61,     /* 8  Key of length 1  */
            'b',      /* 9                   */
            0x25,     /* 10 -6               */
            0xFF,     /* 11 End of map       */
        },
        .initial_index = 3,
        .expected_index = 4,
    },
    {
        .message = "Iterate into map part 3",
        .buffer =
        {
            0xBF,     /* 0  Open Map         */
            0x61,     /* 1  Key of length 1  */
            'a',      /* 2                   */
            0xBF,     /* 3  Open Map         */
            0x61,     /* 4  Key of length 1  */
            'b',      /* 5                   */
            0x24,     /* 6  -5               */
            0xFF,     /* 7  End of map       */
            0x61,     /* 8  Key of length 1  */
            'b',      /* 9                   */
            0x25,     /* 10 -6               */
            0xFF,     /* 11 End of map       */
        },
        .initial_index = 7,
        .expected_index = 8,
    },
};

void test_Next( void )
{
    CBOR_SetCursor( cbor_data, 0 );
    CBOR_MemCopy( cbor_data, test_case->buffer, sizeof( test_case->buffer ) );
    CBOR_SetCursor( cbor_data, test_case->initial_index );
    CBOR_Next( cbor_data );
    cbor_ssize_t actual_index = cbor_data->cursor - cbor_data->buffer_start;
    TEST_ASSERT_EQUAL_MESSAGE(
        test_case->expected_index, actual_index, test_case->message );
}

TEST( aws_cbor_iter, Next )
{
    for( cbor_int_t i = 0; i < NEXT_TEST_CASE_COUNT; i++ )
    {
        test_case = &next_test_cases[ i ];
        RUN_TEST( test_Next );
    }
}

TEST( aws_cbor_iter, Next_asserts_default_case )
{
    CBOR_SetCursor( cbor_data, 0 );
    *cbor_data->cursor = CBOR_TAG;
    TEST_EXPECT_ASSERT( CBOR_Next( cbor_data ) );
}

#define NEXT_KEY_TEST_CASE_COUNT    ( 7 )
const test_next_t next_key_test_cases[ NEXT_KEY_TEST_CASE_COUNT ] =
{
    {
        .message = "next from open map goes to first key",
        .buffer =
        {
            0xBF,     /* 0  Open Map         */
            0x61,     /* 1  Key of length 1  */
            'a',      /* 2                   */
            0x00,     /* 3  0                */
            0x61,     /* 4  Key of length 1  */
            'b',      /* 5                   */
            0x00,     /* 6  0                */
            0xFF,     /* 7  End of map       */
        },
        .initial_index = 0,
        .expected_index = 1,
    },
    {
        .message = "next key first key goes to second key",
        .buffer =
        {
            0xBF,     /* 0  Open Map         */
            0x61,     /* 1  Key of length 1  */
            'a',      /* 2                   */
            0x00,     /* 3  0                */
            0x61,     /* 4  Key of length 1  */
            'b',      /* 5                   */
            0x00,     /* 6  0                */
            0xFF,     /* 7  End of map       */
        },
        .initial_index = 1,
        .expected_index = 4,
    },
    {
        .message = "Next key after map",
        .buffer =
        {
            0xBF,     /* 0  Open Map         */
            0x61,     /* 1  Key of length 1  */
            'a',      /* 2                   */
            0xBF,     /* 3  Open Map         */
            0x61,     /* 4  Key of length 1  */
            'b',      /* 5                   */
            0x24,     /* 6  -5               */
            0xFF,     /* 7  End of map       */
            0x61,     /* 8  Key of length 1  */
            'b',      /* 9                   */
            0x25,     /* 10 -6               */
            0xFF,     /* 11 End of map       */
        },
        .initial_index = 1,
        .expected_index = 8,
    },
    {
        .message = "Next key after string",
        .buffer =
        {
            0xBF,     /* 0 Open Map           */
            0x61,     /* 1 Key of length 1    */
            'a',      /* 2                    */
            0x61,     /* 3 String of length 1 */
            'f',      /* 4                    */
            0x61,     /* 5 Key of length 1    */
            'b',      /* 6                    */
            0x25,     /* 7 -6                 */
            0xFF,     /* 8 End of map         */
        },
        .initial_index = 1,
        .expected_index = 5,
    },
    {
        .message = "Next key after byte string",
        .buffer =
        {
            0xBF,     /* 0 Open Map                */
            0x61,     /* 1 Key of length 1         */
            'a',      /* 2                         */
            0x41,     /* 3 Byte String of length 1 */
            0x10,     /* 4                         */
            0x61,     /* 5 Key of length 1         */
            'b',      /* 6                         */
            0x25,     /* 7 -6                      */
            0xFF,     /* 8 End of map              */
        },
        .initial_index = 1,
        .expected_index = 5,
    },
    {
        .message = "Next key after last key",
        .buffer =
        {
            0xBF,     /* 0 Open Map                */
            0x61,     /* 1 Key of length 1         */
            'a',      /* 2                         */
            0x41,     /* 3 Byte String of length 1 */
            0x10,     /* 4                         */
            0x61,     /* 5 Key of length 1         */
            'b',      /* 6                         */
            0x25,     /* 7 -6                      */
            0xFF,     /* 8 End of map              */
        },
        .initial_index = 5,
        .expected_index = 8,
    },
    {
        .message = "Next key after nested map",
        .buffer =
        {
            0xBF,     /* 0  Open Map 1      */
            0x61,     /* 1  Key of length 1 */
            'm',      /* 2                  */
            0xBF,     /* 3  Open Map 2      */
            0x61,     /* 4  Key of length 1 */
            'n',      /* 5                  */
            0xBF,     /* 6  Open Map 3      */
            0xFF,     /* 7  End of map 3    */
            0xFF,     /* 8  End of map 2    */
            0x61,     /* 9  Key of length 1 */
            'b',      /* 10                 */
            0x25,     /* 11 -6              */
            0xFF,     /* 12 End of map 1    */
        },
        .initial_index = 1,
        .expected_index = 9,
    },
};

void test_NextKey( void )
{
    CBOR_SetCursor( cbor_data, 0 );
    CBOR_MemCopy( cbor_data, test_case->buffer, sizeof( test_case->buffer ) );
    cbor_data->map_end = cbor_data->cursor - 1;
    CBOR_SetCursor( cbor_data, test_case->initial_index );
    CBOR_NextKey( cbor_data );
    cbor_ssize_t actual_index = cbor_data->cursor - cbor_data->buffer_start;
    TEST_ASSERT_EQUAL_MESSAGE(
        test_case->expected_index, actual_index, test_case->message );
}

TEST( aws_cbor_iter, NextKey )
{
    for( cbor_int_t i = 0; i < NEXT_KEY_TEST_CASE_COUNT; i++ )
    {
        test_case = &next_key_test_cases[ i ];
        RUN_TEST( test_NextKey );
    }
}

TEST( aws_cbor_iter, SetCursor )
{
    cbor_int_t expected_position = 0;

    CBOR_SetCursor( cbor_data, expected_position );
    TEST_ASSERT_EQUAL_PTR(
        &( cbor_data->buffer_start[ expected_position ] ), cbor_data->cursor );

    expected_position = 3;
    CBOR_SetCursor( cbor_data, expected_position );
    TEST_ASSERT_EQUAL_PTR(
        &( cbor_data->buffer_start[ expected_position ] ), cbor_data->cursor );
}

TEST( aws_cbor_iter, null_checks )
{
    TEST_EXPECT_ASSERT( CBOR_NextPtr( NULL ) );
    TEST_EXPECT_ASSERT( CBOR_Next( NULL ) );
    TEST_EXPECT_ASSERT( CBOR_NextKeyPtr( NULL ) );
    TEST_EXPECT_ASSERT( CBOR_NextKey( NULL ) );
    TEST_EXPECT_ASSERT( CBOR_SetCursor( NULL, 0 ) );
}
