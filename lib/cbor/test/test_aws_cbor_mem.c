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
#include <string.h>

typedef struct test_size_s {
    cbor_byte_t  buffer[32];
    cbor_ssize_t cursor_index;
    cbor_ssize_t expected_size;
    char *       message;
} test_size_t;

static cbor_handle_t cbor_data;

TEST_GROUP(aws_cbor_mem);
TEST_SETUP(aws_cbor_mem)
{
    cbor_data = CBOR_New(0);
}

TEST_TEAR_DOWN(aws_cbor_mem)
{
    TEST_ASSERT_EQUAL(eCBOR_ERR_NO_ERROR, cbor_data->err);
    CBOR_Delete(&cbor_data);
}

TEST_GROUP_RUNNER(aws_cbor_mem)
{
    RUN_TEST_CASE(aws_cbor_mem, test_setup);

    RUN_TEST_CASE(aws_cbor_mem, AssignAndIncrement);
    RUN_TEST_CASE(
        aws_cbor_mem, AssignAndIncrement_reallocates_when_buffer_is_full);
    RUN_TEST_CASE(
        aws_cbor_mem, AssignAndIncrement_returns_err_when_out_of_memory);
    RUN_TEST_CASE(aws_cbor_mem,
        AssignAndIncrement_asserts_when_cursor_before_start_of_buffer);

    RUN_TEST_CASE(aws_cbor_mem, AssignAndDecrement);
    RUN_TEST_CASE(aws_cbor_mem,
        AssignAndDecrement_asserts_when_cursor_before_start_of_buffer);
    RUN_TEST_CASE(
        aws_cbor_mem, AssignAndDecrement_returns_err_when_past_end_of_buffer);

    RUN_TEST_CASE(aws_cbor_mem, MemCopy_will_forward_copy_overlapping_strings);
    RUN_TEST_CASE(aws_cbor_mem, MemCopy_will_reverse_copy_overlapping_strings);
    RUN_TEST_CASE(aws_cbor_mem, MemCopy_will_do_nothing_if_src_matches_dest);
    RUN_TEST_CASE(aws_cbor_mem, MemCopy_returns_err_when_out_of_memory);

    RUN_TEST_CASE(aws_cbor_mem, GetValueSize);

    RUN_TEST_CASE(aws_cbor_mem, ValueResize_will_shrink_value);
    RUN_TEST_CASE(aws_cbor_mem, ValueResize_will_grow_value);
    RUN_TEST_CASE(
        aws_cbor_mem, ValueResize_makes_no_change_if_data_size_unknown);
    RUN_TEST_CASE(aws_cbor_mem, ValueResize_asserts_on_map_end_errors);

    RUN_TEST_CASE(aws_cbor_mem, null_checks);
}

TEST(aws_cbor_mem, test_setup)
{
    /* Test to ensure tests are setup correctly */
    TEST_ASSERT_NOT_NULL(cbor_data);
}

TEST(aws_cbor_mem, AssignAndIncrement)
{
    CBOR_AssignAndIncrementCursor(cbor_data, 0x42);
    CBOR_AssignAndIncrementCursor(cbor_data, 0x24);
    TEST_ASSERT_EQUAL_HEX8(0x42, (cbor_data->buffer_start)[0]);
    TEST_ASSERT_EQUAL_HEX8(0x24, (cbor_data->buffer_start)[1]);
}

TEST(aws_cbor_mem, AssignAndIncrement_reallocates_when_buffer_is_full)
{
    cbor_int_t initial_buffer_size =
        cbor_data->buffer_end - cbor_data->buffer_start + 1;
    cbor_data->cursor = cbor_data->buffer_end;
    CBOR_AssignAndIncrementCursor(cbor_data, 0xF0);
    CBOR_AssignAndIncrementCursor(cbor_data, 0x01);
    cbor_int_t final_buffer_size =
        cbor_data->buffer_end - cbor_data->buffer_start + 1;
    TEST_ASSERT_GREATER_THAN(initial_buffer_size, final_buffer_size);
}

TEST(aws_cbor_mem, AssignAndIncrement_returns_err_when_out_of_memory)
{
    cbor_data->cursor = cbor_data->buffer_end;
    UnityMalloc_MakeMallocFailAfterCount(0);
    CBOR_AssignAndIncrementCursor(cbor_data, 0xDE);
    CBOR_AssignAndIncrementCursor(cbor_data, 0xAD);
    TEST_ASSERT_EQUAL(eCBOR_ERR_INSUFFICENT_SPACE, cbor_data->err);

    cbor_data->err = eCBOR_ERR_NO_ERROR;
}

TEST(
    aws_cbor_mem, AssignAndIncrement_asserts_when_cursor_before_start_of_buffer)
{
    cbor_data->cursor = cbor_data->buffer_start - 1;
    TEST_EXPECT_ASSERT(CBOR_AssignAndIncrementCursor(cbor_data, 0xBC));
}

TEST(aws_cbor_mem, AssignAndDecrement)
{
    cbor_data->cursor = &(cbor_data->buffer_start)[1];
    CBOR_AssignAndDecrementCursor(cbor_data, 0x73);
    CBOR_AssignAndDecrementCursor(cbor_data, 0x24);
    TEST_ASSERT_EQUAL_HEX8(0x73, (cbor_data->buffer_start)[1]);
    TEST_ASSERT_EQUAL_HEX8(0x24, (cbor_data->buffer_start)[0]);
}

TEST(
    aws_cbor_mem, AssignAndDecrement_asserts_when_cursor_before_start_of_buffer)
{
    CBOR_AssignAndDecrementCursor(cbor_data, 0xEF);
    TEST_EXPECT_ASSERT(CBOR_AssignAndDecrementCursor(cbor_data, 0xBE));
}

TEST(aws_cbor_mem, AssignAndDecrement_returns_err_when_past_end_of_buffer)
{
    cbor_data->cursor = cbor_data->buffer_end + 1;

    UnityMalloc_MakeMallocFailAfterCount(0);
    CBOR_AssignAndDecrementCursor(cbor_data, 0xCE);
    TEST_ASSERT_EQUAL(eCBOR_ERR_INSUFFICENT_SPACE, cbor_data->err);

    cbor_data->err = eCBOR_ERR_NO_ERROR;
}

TEST(aws_cbor_mem, MemCopy_will_forward_copy_overlapping_strings)
{
    char pcInit[]     = "Helllo, World!";
    char pcExpected[] = "Hello, World!";

    // Copy the string into the CBOR buffer.
    memcpy((cbor_data->buffer_start), pcInit, strlen(pcInit) + 1);

    // Point the cursor to the third l.
    // This is where the data will be copied to.
    cbor_data->cursor = &(cbor_data->buffer_start)[4];

    // Starting from the 'o', copy back to the cursor.
    CBOR_MemCopy(cbor_data, &(cbor_data->buffer_start)[5], 10);

    TEST_ASSERT_EQUAL_STRING(pcExpected, (cbor_data->buffer_start));

    // Cursor will now be pointing to the position after the null terminator for
    // the string "Hello, World!"
    cbor_byte_t *expected_cursor_position =
        &(cbor_data->buffer_start)[strlen(pcExpected) + 1];
    TEST_ASSERT_EQUAL_PTR(expected_cursor_position, cbor_data->cursor);
}

TEST(aws_cbor_mem, MemCopy_will_reverse_copy_overlapping_strings)
{
    char pcInit[]     = "Helo, World!";
    char pcExpected[] = "Hello, World!";

    // Copy the string into the CBOR buffer.
    memcpy((cbor_data->buffer_start), pcInit, strlen(pcInit) + 1);

    // Point the cursor at the 'o'
    cbor_data->cursor = &(cbor_data->buffer_start)[3];

    // Starting from the 'l' in "Helo", copy to the cursor
    CBOR_MemCopy(cbor_data, &(cbor_data->buffer_start)[2], 11);

    TEST_ASSERT_EQUAL_STRING(pcExpected, (cbor_data->buffer_start));

    // Cursor will now be pointing to the position after the null terminator for
    // the string "Hello, World!"
    cbor_byte_t *expected_cursor_position =
        &(cbor_data->buffer_start)[strlen(pcExpected) + 1];
    TEST_ASSERT_EQUAL_PTR(expected_cursor_position, cbor_data->cursor);
}

TEST(aws_cbor_mem, MemCopy_will_do_nothing_if_src_matches_dest)
{
    char pcInit[]     = "Hello, World!";
    char pcExpected[] = "Hello, World!";
    // Copy the string into the CBOR buffer.
    memcpy((cbor_data->buffer_start), pcInit, sizeof(pcInit));

    // Point the cursor to the start of the buffer
    cbor_data->cursor = (cbor_data->buffer_start);

    // From the start of the buffer, copy to the cursor
    // (Which is at the same position (the start of the buffer)).
    CBOR_MemCopy(cbor_data, (cbor_data->buffer_start), sizeof(pcInit));

    TEST_ASSERT_EQUAL_STRING(pcExpected, (cbor_data->buffer_start));

    // Cursor will now be pointing to the position after the null terminator for
    // the string "Hello, World!"
    cbor_byte_t *expected_cursor_position =
        &(cbor_data->buffer_start)[strlen(pcExpected) + 1];
    TEST_ASSERT_EQUAL(0, expected_cursor_position - cbor_data->cursor);
}

TEST(aws_cbor_mem, MemCopy_returns_err_when_out_of_memory)
{
    char lorem[] =
        "Lorem ipsum dolor sit amet, consectetur adipisicing elit, sed do "
        "eiusmod tempor incididunt ut labore et dolore magna aliqua. Ut enim "
        "ad minim veniam, quis nostrud exercitation ullamco laboris nisi ut "
        "aliquip ex ea commodo consequat. Duis aute irure dolor in "
        "reprehenderit in voluptate velit esse cillum dolore eu fugiat nulla "
        "pariatur. Excepteur sint occaecat cupidatat non proident, sunt in "
        "culpa qui officia deserunt mollit anim id est laborum. Lorem ipsum "
        "dolor sit amet, consectetur adipisicing elit, sed do eiusmod tempor "
        "incididunt ut labore et dolore magna aliqua. Ut enim ad minim veniam, "
        "quis nostrud exercitation ullamco laboris nisi ut aliquip ex ea "
        "commodo consequat. Duis aute irure dolor in reprehenderit in "
        "voluptate velit esse cillum dolore eu fugiat nulla pariatur. "
        "Excepteur sint occaecat cupidatat non proident, sunt in culpa qui "
        "officia deserunt mollit anim id est laborum. Lorem ipsum dolor sit "
        "amet, consectetur adipisicing elit, sed do eiusmod tempor incididunt "
        "ut labore et dolore magna aliqua. Ut enim ad minim veniam, quis "
        "nostrud exercitation ullamco laboris nisi ut aliquip ex ea commodo "
        "consequat. Duis aute irure dolor in reprehenderit in voluptate velit "
        "esse cillum dolore eu fugiat nulla pariatur. Excepteur sint occaecat "
        "cupidatat non proident, sunt in culpa qui officia deserunt mollit "
        "anim id est laborum.";
    UnityMalloc_MakeMallocFailAfterCount(0);
    CBOR_MemCopy(cbor_data, lorem, sizeof(lorem));
    TEST_ASSERT_EQUAL(eCBOR_ERR_INSUFFICENT_SPACE, cbor_data->err);
    cbor_data->err = eCBOR_ERR_NO_ERROR;
}

const test_size_t *test_case;
const test_size_t  size_test_cases[] = {
    {
        .message = "smallest integer in additional data",
        .buffer =
            {
                0xBF, /* 0  Open Map         */
                0x61, /* 1  Key of length 1  */
                'a',  /* 2                   */
                0x00, /* 3  0                */
                0xFF, /* 4  End of map       */
            },
        .cursor_index  = 3,
        .expected_size = 1,
    },
    {
        .message = "largest integer in additional data",
        .buffer =
            {
                0xBF, /* 0  Open Map         */
                0x61, /* 1  Key of length 1  */
                'a',  /* 2                   */
                0x17, /* 3  23                */
                0xFF, /* 4  End of map       */
            },
        .cursor_index  = 3,
        .expected_size = 1,
    },
    {
        .message = "integer in 1 following byte",
        .buffer =
            {
                0xBF, /* 0  Open Map              */
                0x61, /* 1  Key of length 1       */
                'a',  /* 2                        */
                0x18, /* 3  cbor_int_t follows in 1 byte */
                24,   /* 4                        */
                0xFF, /* 5  End of map            */
            },
        .cursor_index  = 3,
        .expected_size = 2,
    },
    {
        .message = "integer in 2 following bytes",
        .buffer =
            {
                0xBF, /* 0  Open Map              */
                0x61, /* 1  Key of length 1       */
                'a',  /* 2                        */
                0x19, /* 3  cbor_int_t follows in 2 byte */
                0xDE, /* 4                        */
                0xAD, /* 5                        */
                0xFF, /* 6  End of map            */
            },
        .cursor_index  = 3,
        .expected_size = 3,
    },
    {
        .message = "integer in 4 following bytes",
        .buffer =
            {
                0xBF, /* 0  Open Map              */
                0x61, /* 1  Key of length 1       */
                'a',  /* 2                        */
                0x1A, /* 3  cbor_int_t follows in 4 byte */
                0xDE, /* 4                        */
                0xAD, /* 5                        */
                0xBE, /* 6                        */
                0xEF, /* 7                        */
                0xFF, /* 8  End of map            */
            },
        .cursor_index  = 3,
        .expected_size = 5,
    },
    {
        .message = "zero length string",
        .buffer =
            {
                0xBF, /* 0  Open Map              */
                0x61, /* 1  Key of length 1       */
                'a',  /* 2                        */
                0x60, /* 3  zero length string    */
            },
        .cursor_index  = 3,
        .expected_size = 1,
    },
    {
        .message = "23 char string",
        .buffer =
            {
                0xBF, /* 0  Open Map              */
                0x61, /* 1  Key of length 1       */
                'a',  /* 2                        */
                0x77, /* 3  23 char string        */
            },
        .cursor_index  = 3,
        .expected_size = 24,
    },
    {
        .message = "24 char string",
        .buffer =
            {
                0xBF, /* 0  Open Map                      */
                0x61, /* 1  Key of length 1               */
                'a',  /* 2                                */
                0x78, /* 3  string size follows in 1 byte */
                24,   /* 4  24 char string                */
            },
        .cursor_index  = 3,
        .expected_size = 24 + 2,
    },
    {
        .message = "256 char string",
        .buffer =
            {
                0xBF, /* 0  Open Map                      */
                0x61, /* 1  Key of length 1               */
                'a',  /* 2                                */
                0x79, /* 3  string size follows in 2 byte */
                0x01, /* 4  256 char string               */
                0x00, /* 5                                */
            },
        .cursor_index  = 3,
        .expected_size = 256 + 3,
    },
    {
        .message = "empty map",
        .buffer =
            {
                0xBF, /* 0 Open Map   */
                0xFF, /* 1 End of map */
            },
        .cursor_index  = 0,
        .expected_size = 2,
    },
    {
        .message = "simple map",
        .buffer =
            {
                0xBF, /* 0 Open Map        */
                0x61, /* 1 Key of length 1 */
                'a',  /* 2                 */
                0x00, /* 3 0               */
                0xFF, /* 4 End of map      */
            },
        .cursor_index  = 0,
        .expected_size = 5,
    },
    {
        .message = "map inside of map",
        .buffer =
            {
                0xBF, /* 0  Open Map               */
                0x61, /* 1  Key of length 1        */
                'm',  /* 2                         */
                0xBF, /* 3  Open Map               */
                0x61, /* 4  Key, length 1          */
                '1',  /* 5                         */
                0x19, /* 6  cbor_int_t follows in 2 bytes */
                0xDE, /* 7                         */
                0xAD, /* 8                         */
                0xFF, /* 9  End of map             */
                0x61, /* 10 Key, length 1          */
                'a',  /* 11                        */
                0x10, /* 12 16                     */
                0xFF, /* 13 End of map             */
            },
        .cursor_index  = 3,
        .expected_size = 7,
    },
    {
        .message = "map containing a map",
        .buffer =
            {
                0xBF, /* 0  Open Map               */
                0x61, /* 1  Key of length 1        */
                'm',  /* 2                         */
                0xBF, /* 3  Open Map               */
                0x61, /* 4  Key, length 1          */
                '1',  /* 5                         */
                0x19, /* 6  cbor_int_t follows in 2 bytes */
                0xDE, /* 7                         */
                0xAD, /* 8                         */
                0xFF, /* 9  End of map             */
                0x61, /* 10 Key, length 1          */
                'a',  /* 11                        */
                0x10, /* 12 16                     */
                0xFF, /* 13 End of map             */
            },
        .cursor_index  = 0,
        .expected_size = 14,
    },
};

void test_GetSize(void)
{
    CBOR_SetCursor(cbor_data, 0);
    CBOR_MemCopy(cbor_data, test_case->buffer, sizeof(test_case->buffer));
    CBOR_SetCursor(cbor_data, test_case->cursor_index);
    cbor_ssize_t actual_size = CBOR_DataItemSize(cbor_data);
    TEST_ASSERT_EQUAL_MESSAGE(
        test_case->expected_size, actual_size, test_case->message);
}

TEST(aws_cbor_mem, GetValueSize)
{
    cbor_int_t test_case_count =
        sizeof(size_test_cases) / sizeof(size_test_cases[0]);
    for (cbor_int_t i = 0; i < test_case_count; i++) {
        test_case = &size_test_cases[i];
        RUN_TEST(test_GetSize);
    }
}

TEST(aws_cbor_mem, ValueResize_will_shrink_value)
{
    cbor_byte_t initial[] = {
        0xBF, /* 0  Open Map          */
        0x61, /* 1  Key of length 1   */
        0x31, /* 2 '1'                */
        0x1A, /* 3  Start 32-bit cbor_int_t  */
        0x5C, /* 4                    */
        0xA1, /* 5                    */
        0xAB, /* 6                    */
        0x1E, /* 7                    */
        0x61, /* 8  Key of length 1   */
        0x32, /* 9 '2'                */
        0x19, /* 10  Start 16-bit cbor_int_t */
        0x73, /* 11                   */
        0x92, /* 12                   */
        0xFF, /* 13 End of map        */
    };

    cbor_byte_t expected[] = {
        0xBF, /* 0      */
        0x61, /* 1      */
        0x31, /* 2      */
        0x1A, /* 3      */
        0x5C, /* 4      */
        0x61, /* 5      */
        0x32, /* 6      */
        0x19, /* 7      */
        0x73, /* 8      */
        0x92, /* 9      */
        0xFF, /* 10     */
    };

    memcpy((cbor_data->buffer_start), initial, sizeof(initial));
    cbor_data->cursor     = &(cbor_data->buffer_start)[3];
    cbor_data->map_end    = &(cbor_data->buffer_start)[13];
    cbor_ssize_t new_size = 2;
    CBOR_ValueResize(cbor_data, new_size);

    TEST_ASSERT_EQUAL_HEX8_ARRAY(
        expected, (cbor_data->buffer_start), sizeof(expected));
}

TEST(aws_cbor_mem, ValueResize_will_grow_value)
{
    cbor_byte_t initial[] = {
        0xBF, /* 0  Open Map         */
        0x61, /* 1  Key of length 1  */
        0x31, /* 2 '1'               */
        0x18, /* 3  Start 8-bit cbor_int_t  */
        0x23, /* 4                   */
        0x61, /* 5  Key of length 1  */
        0x32, /* 6 '2'               */
        0x19, /* 7  Start 16-bit cbor_int_t */
        0x73, /* 8                   */
        0x92, /* 9                   */
        0xFF, /* 10 End of map       */
    };

    cbor_byte_t expected[] = {
        0xBF, /* 0               */
        0x61, /* 1               */
        0x31, /* 2               */
        0x18, /* 3               */
        0x23, /* 4               */
        0x61, /* 5               */
        0x61, /* 6               */
        0x32, /* 7               */
        0x19, /* 8               */
        0x73, /* 9               */
        0x92, /* 10              */
        0xFF, /* 11              */
    };

    memcpy((cbor_data->buffer_start), initial, sizeof(initial));
    cbor_data->cursor     = &(cbor_data->buffer_start)[3];
    cbor_data->map_end    = &(cbor_data->buffer_start)[10];
    cbor_ssize_t new_size = 3;
    CBOR_ValueResize(cbor_data, new_size);

    TEST_ASSERT_EQUAL_HEX8_ARRAY(
        expected, (cbor_data->buffer_start), sizeof(expected));
}

TEST(aws_cbor_mem, ValueResize_makes_no_change_if_data_size_unknown)
{
    cbor_byte_t expected[] = {
        0xBF, /* 0  Open Map         */
        0x61, /* 1  Key of length 1  */
        0x31, /* 2 '1'               */
        0x20, /* 3  unkown data type */
        0x23, /* 4                   */
        0x61, /* 5  Key of length 1  */
        0x32, /* 6 '2'               */
        0x19, /* 7  Start 16-bit cbor_int_t */
        0x73, /* 8                   */
        0x92, /* 9                   */
        0xFF, /* 10 End of map       */
    };

    memcpy((cbor_data->buffer_start), expected, sizeof(expected));
    CBOR_SetCursor(cbor_data, 3);
    cbor_data->map_end    = cbor_data->buffer_start + 10;
    cbor_ssize_t new_size = 2;
    CBOR_ValueResize(cbor_data, new_size);

    TEST_ASSERT_EQUAL_HEX8_ARRAY(
        expected, (cbor_data->buffer_start), sizeof(expected));
}

TEST(aws_cbor_mem, ValueResize_asserts_on_map_end_errors)
{
    cbor_byte_t initial[] = {
        0xBF, /* 0 Open Map        */
        0x61, /* 1 Key of length 1 */
        0x31, /* 2 '1'             */
        0x1A, /* 3 32-bit cbor_int_t      */
        0x23, /* 4                 */
        0x23, /* 5                 */
        0x23, /* 6                 */
        0x23, /* 7                 */
        0xFF, /* 8 End of map      */
    };

    memcpy((cbor_data->buffer_start), initial, sizeof(initial));
    cbor_data->cursor = &(cbor_data->buffer_start)[3];

    // point map_end to a spot before the end of the map
    cbor_data->map_end    = &(cbor_data->buffer_start)[4];
    cbor_ssize_t new_size = 1;
    TEST_EXPECT_ASSERT(CBOR_ValueResize(cbor_data, new_size));
}
TEST(aws_cbor_mem, null_checks)
{
    TEST_EXPECT_ASSERT(CBOR_AssignAndIncrementCursor(NULL, 0));
    TEST_EXPECT_ASSERT(CBOR_AssignAndDecrementCursor(NULL, 0));
    TEST_EXPECT_ASSERT(CBOR_MemCopy(NULL, "hello", 0));
    TEST_EXPECT_ASSERT(CBOR_MemCopy(cbor_data, NULL, 0));
    TEST_EXPECT_ASSERT(CBOR_DataItemSize(NULL));
    TEST_EXPECT_ASSERT(CBOR_DataItemSizePtr(NULL));
    TEST_EXPECT_ASSERT(CBOR_ValueResize(NULL, 0));
}
