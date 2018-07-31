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

extern cbor_int_t TEST_assert_fails;
cbor_handle_t     cbor_data;

TEST_GROUP(aws_cbor_string);
TEST_SETUP(aws_cbor_string)
{
    TEST_assert_fails  = 0;
    cbor_data          = CBOR_New(0);
    char empty_map[20] = {'{', '}'};
    CBOR_MemCopy(cbor_data, empty_map, 20);
    CBOR_SetCursor(cbor_data, 0);
}

TEST_TEAR_DOWN(aws_cbor_string)
{
    TEST_ASSERT_EQUAL(eCBOR_ERR_NO_ERROR, cbor_data->err);
    TEST_ASSERT_EQUAL(0, TEST_assert_fails);
    CBOR_Delete(&cbor_data);
}

TEST_GROUP_RUNNER(aws_cbor_string)
{
    RUN_TEST_CASE(aws_cbor_string, WriteString_small_string);
    RUN_TEST_CASE(
        aws_cbor_string, WriteString_cursor_points_to_next_space_after_string);
    RUN_TEST_CASE(aws_cbor_string, WriteString_int8_sized_string);
    RUN_TEST_CASE(aws_cbor_string, WriteString_int16_sized_string);
    RUN_TEST_CASE(aws_cbor_string, WriteString_larger_than_supported_string);
    RUN_TEST_CASE(aws_cbor_string, WriteString_will_resize_data);
    RUN_TEST_CASE(aws_cbor_string, ReadString_read_short_string);
    RUN_TEST_CASE(aws_cbor_string, ReadString_read_int8_sized_string);
    RUN_TEST_CASE(aws_cbor_string, ReadString_read_int16_sized_string);
    RUN_TEST_CASE(aws_cbor_string, StringLength_Gets_string_length);
    RUN_TEST_CASE(
        aws_cbor_string, ReadString_gets_same_result_when_called_repeatedly);
    RUN_TEST_CASE(
        aws_cbor_string, StringLength_gets_same_result_when_called_repeatedly);
    RUN_TEST_CASE(aws_cbor_string, StringCompare_returns_0_when_strings_match);
    RUN_TEST_CASE(aws_cbor_string,
        StringCompare_matches_strcmp_when_first_string_is_greater);
    RUN_TEST_CASE(aws_cbor_string,
        StringCompare_matches_strcmp_when_first_string_is_less);
    RUN_TEST_CASE(aws_cbor_string, StringCompare_both_strings_empty);
    RUN_TEST_CASE(aws_cbor_string,
        StringCompare_matches_strcmp_when_second_string_is_empty);
    RUN_TEST_CASE(aws_cbor_string, StringCompare_first_string_is_shorter);
    RUN_TEST_CASE(aws_cbor_string, StringCompare_second_string_is_shorter);

    RUN_TEST_CASE(aws_cbor_string, null_checks);
}

TEST(aws_cbor_string, WriteString_small_string)
{
    cbor_byte_t expected[] = {
        0x6A, /* 0 String, length 10 */
        'h',  /* 1                   */
        'i',  /* 2                   */
        ',',  /* 3                   */
        ' ',  /* 4                   */
        'w',  /* 5                   */
        '3',  /* 6                   */
        'r',  /* 7                   */
        'l',  /* 8                   */
        'd',  /* 9                   */
        '!',  /* 10                  */
    };
    CBOR_WriteString(cbor_data, "hi, w3rld!");

    TEST_ASSERT_EQUAL_HEX8_ARRAY(
        expected, cbor_data->buffer_start, sizeof(expected));
}

TEST(aws_cbor_string, WriteString_cursor_points_to_next_space_after_string)
{
    CBOR_WriteString(cbor_data, "a");
    TEST_ASSERT_EQUAL_PTR(&(cbor_data->buffer_start)[2], cbor_data->cursor);
}

TEST(aws_cbor_string, WriteString_int8_sized_string)
{
    char *test_string = "I need this string to be more than 23 characters, but "
                        "less than 256 characters.  This will result in "
                        "having a single byte store the length of the string.";
    cbor_int_t test_length = strlen(test_string);

    cbor_byte_t expected[test_length + 2];
    expected[0] = 0x78; /* String with the length given in the next 1 byte */
    expected[1] = 153;  /* Length of the string*/
    memcpy(&expected[2], test_string, test_length);

    CBOR_WriteString(cbor_data, test_string);

    TEST_ASSERT_EQUAL_HEX8_ARRAY(
        expected, cbor_data->buffer_start, sizeof(expected));
}

TEST(aws_cbor_string, WriteString_int16_sized_string)
{
    char *test_string =
        "For this test, I need a string that is greater than 256 characters.  "
        "This will require the length to be stored in 2 bytes.  This will also "
        "enable the storage of strings up to 64KB (~65kB) in length. Lorem "
        "ipsum dolor sit amet, consectetur adipisicing elit, sed do eiusmod "
        "tempor incididunt ut labore et dolore magna aliqua. Ut enim ad minim "
        "veniam, quis nostrud exercitation ullamco laboris nisi ut aliquip ex "
        "ea commodo consequat.";
    cbor_int_t test_length = strlen(test_string);

    cbor_byte_t expected[test_length + 3];
    expected[0] = 0x79; /* String with the length given in the next 2 bytes */
    /* Length of string is 431, or 0x01AF */
    expected[1] = 1;
    expected[2] = 0xAF;
    memcpy(&expected[3], test_string, test_length);

    CBOR_WriteString(cbor_data, test_string);

    TEST_ASSERT_EQUAL_HEX8_ARRAY(
        expected, cbor_data->buffer_start, sizeof(expected));
}

TEST(aws_cbor_string, WriteString_larger_than_supported_string)
{
    cbor_ssize_t string_length = 1U << 16U;
    cbor_ssize_t buffer_length = string_length + 1;
    char         str[buffer_length];
    for (cbor_int_t i = 0; i < string_length; i++) {
        str[i] = 'a';
    }
    str[string_length] = 0;

    CBOR_WriteString(cbor_data, str);
    TEST_ASSERT_EQUAL(eCBOR_ERR_UNSUPPORTED_WRITE_OPERATION, cbor_data->err);
    cbor_data->err = 0;
}

TEST(aws_cbor_string, WriteString_will_resize_data)
{
    cbor_data->map_end = cbor_data->buffer_start + 10;
    CBOR_WriteString(cbor_data, "hi");
    CBOR_WriteString(cbor_data, "world");
    cbor_data->cursor = cbor_data->buffer_start;
    CBOR_WriteString(cbor_data, "hello");
    cbor_byte_t expected[] = {
        0x65, /* 0 String, length 5  */
        'h',  /* 1                   */
        'e',  /* 2                   */
        'l',  /* 3                   */
        'l',  /* 4                   */
        'o',  /* 5                   */
        0x65, /* 6 String, length 5  */
        'w',  /* 7                   */
        'o',  /* 8                   */
        'r',  /* 9                   */
        'l',  /* 10                  */
        'd',  /* 11                  */
    };

    TEST_ASSERT_EQUAL_HEX8_ARRAY(
        expected, cbor_data->buffer_start, sizeof(expected));
}

TEST(aws_cbor_string, ReadString_read_short_string)
{
    char expected[] = "A short string";
    CBOR_WriteString(cbor_data, expected);
    cbor_data->cursor = cbor_data->buffer_start;

    char *str = CBOR_ReadString(cbor_data);

    TEST_ASSERT_EQUAL_STRING(expected, str);
    free(str);
}

TEST(aws_cbor_string, ReadString_read_int8_sized_string)
{
    char expected[] = "Lorem ipsum dolor sit amet, consectetur adipisicing "
                      "elit, sed do eiusmod tempor incididunt ut labore et "
                      "dolore magna aliqua.";
    CBOR_WriteString(cbor_data, expected);
    cbor_data->cursor = cbor_data->buffer_start;

    char *str = CBOR_ReadString(cbor_data);

    TEST_ASSERT_EQUAL_STRING(expected, str);
    free(str);
}

TEST(aws_cbor_string, ReadString_read_int16_sized_string)
{
    char expected[] =
        "Ut enim ad minim veniam, quis nostrud exercitation ullamco laboris "
        "nisi ut aliquip ex ea commodo consequat. Duis aute irure dolor in "
        "reprehenderit in voluptate velit esse cillum dolore eu fugiat nulla "
        "pariatur. Excepteur sint occaecat cupidatat non proident, sunt in "
        "culpa qui officia deserunt mollit anim id est laborum.";
    CBOR_WriteString(cbor_data, expected);
    cbor_data->cursor = cbor_data->buffer_start;

    char *str = CBOR_ReadString(cbor_data);

    TEST_ASSERT_EQUAL_STRING(expected, str);
    free(str);
}

TEST(aws_cbor_string, ReadString_gets_same_result_when_called_repeatedly)
{
    char str[] = "Duis aute irure dolor in reprehenderit in voluptate velit "
                 "esse cillum dolore eu fugiat nulla pariatur. Excepteur sint "
                 "occaecat cupidatat non proident, sunt in culpa qui officia "
                 "deserunt mollit anim id est laborum.";
    CBOR_WriteString(cbor_data, str);
    cbor_data->cursor = cbor_data->buffer_start;

    char *expected = CBOR_ReadString(cbor_data);
    char *result   = CBOR_ReadString(cbor_data);

    TEST_ASSERT_EQUAL_STRING(expected, result);
    free(expected);
    free(result);
}

TEST(aws_cbor_string, StringLength_Gets_string_length)
{
    char         str[]    = "This string is 28 chars long";
    cbor_ssize_t expected = strlen(str);
    CBOR_WriteString(cbor_data, str);
    cbor_data->cursor   = cbor_data->buffer_start;
    cbor_ssize_t result = CBOR_StringLength(cbor_data);
    TEST_ASSERT_EQUAL_INT(expected, result);
}

TEST(aws_cbor_string, StringLength_gets_same_result_when_called_repeatedly)
{
    char str[] = "This string is 29 chars long.";
    CBOR_WriteString(cbor_data, str);
    cbor_data->cursor     = cbor_data->buffer_start;
    cbor_ssize_t expected = CBOR_StringLength(cbor_data);
    cbor_ssize_t result   = CBOR_StringLength(cbor_data);
    TEST_ASSERT_EQUAL_INT(expected, result);
}

TEST(aws_cbor_string, StringCompare_returns_0_when_strings_match)
{
    char *str1 = "Matching Strings";
    char *str2 = "Matching Strings";
    CBOR_WriteString(cbor_data, str1);
    cbor_data->cursor = cbor_data->buffer_start;

    TEST_ASSERT_EQUAL_INT(0, CBOR_StringCompare(cbor_data, str2));
}

TEST(aws_cbor_string, StringCompare_matches_strcmp_when_first_string_is_greater)
{
    char *str1 = "Mismatching Strings B <-- that's different";
    char *str2 = "Mismatching Strings a <-- that's different";
    CBOR_WriteString(cbor_data, str1);
    cbor_data->cursor = cbor_data->buffer_start;

    cbor_ssize_t actual = CBOR_StringCompare(cbor_data, str2);
    TEST_ASSERT_LESS_THAN(0, actual);
}

TEST(aws_cbor_string, StringCompare_matches_strcmp_when_first_string_is_less)
{
    char *str1 = "Mismatching Strings a <-- that's different";
    char *str2 = "Mismatching Strings B <-- that's different";
    CBOR_WriteString(cbor_data, str1);
    cbor_data->cursor = cbor_data->buffer_start;

    cbor_ssize_t actual = CBOR_StringCompare(cbor_data, str2);
    TEST_ASSERT_GREATER_THAN(0, actual);
}

TEST(aws_cbor_string, StringCompare_both_strings_empty)
{
    char *str1 = "";
    char *str2 = "";
    CBOR_WriteString(cbor_data, str1);
    cbor_data->cursor   = cbor_data->buffer_start;
    cbor_ssize_t actual = CBOR_StringCompare(cbor_data, str2);
    TEST_ASSERT_EQUAL_INT(0, actual);
}

TEST(aws_cbor_string, StringCompare_matches_strcmp_when_second_string_is_empty)
{
    char *str1 = "str1 is not empty";
    char *str2 = "";
    CBOR_WriteString(cbor_data, str1);
    cbor_data->cursor   = cbor_data->buffer_start;
    cbor_ssize_t actual = CBOR_StringCompare(cbor_data, str2);
    TEST_ASSERT_GREATER_THAN(0, actual);
}

TEST(aws_cbor_string, StringCompare_first_string_is_shorter)
{
    char *str1 = "str1 is shorter";
    char *str2 = "str1 is shorter, and str2 is longer";
    CBOR_WriteString(cbor_data, str1);
    cbor_data->cursor = cbor_data->buffer_start;

    cbor_ssize_t actual = CBOR_StringCompare(cbor_data, str2);
    TEST_ASSERT_LESS_THAN(0, actual);
}

TEST(aws_cbor_string, StringCompare_second_string_is_shorter)
{
    char *str1 = "str2 is shorter, and str1 is longer";
    char *str2 = "str2 is shorter";
    CBOR_WriteString(cbor_data, str1);
    cbor_data->cursor = cbor_data->buffer_start;

    cbor_ssize_t actual = CBOR_StringCompare(cbor_data, str2);
    TEST_ASSERT_GREATER_THAN(0, actual);
}

TEST(aws_cbor_string, null_checks)
{
    TEST_EXPECT_ASSERT(CBOR_WriteString(NULL, "hello"));
    TEST_EXPECT_ASSERT(CBOR_WriteString(cbor_data, NULL));
    TEST_EXPECT_ASSERT(CBOR_ReadString(NULL));
    TEST_EXPECT_ASSERT(CBOR_StringSize(NULL));
    TEST_EXPECT_ASSERT(CBOR_StringLength(NULL));
    TEST_EXPECT_ASSERT(CBOR_StringCompare(NULL, "hello"));
    TEST_EXPECT_ASSERT(CBOR_StringCompare(cbor_data, NULL));
}
