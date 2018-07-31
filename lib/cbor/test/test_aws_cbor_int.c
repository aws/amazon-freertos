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

static cbor_handle_t cbor_data;

extern cbor_int_t TEST_assert_fails;

TEST_GROUP(aws_cbor_int);
TEST_SETUP(aws_cbor_int)
{
    TEST_assert_fails    = 0;
    cbor_data            = CBOR_New(0);
    cbor_byte_t init[32] = {0};

    CBOR_MemCopy(cbor_data, init, sizeof(init));
    CBOR_SetCursor(cbor_data, 0);
}

TEST_TEAR_DOWN(aws_cbor_int)
{
    TEST_ASSERT_EQUAL(eCBOR_ERR_NO_ERROR, cbor_data->err);
    TEST_ASSERT_EQUAL(0, TEST_assert_fails);
    CBOR_Delete(&cbor_data);
}

TEST_GROUP_RUNNER(aws_cbor_int)
{
    RUN_TEST_CASE(aws_cbor_int, IntSize_SmallInt);
    RUN_TEST_CASE(aws_cbor_int, IntSize_Int8);
    RUN_TEST_CASE(aws_cbor_int, IntSize_Int16);
    RUN_TEST_CASE(aws_cbor_int, IntSize_Int32);

    RUN_TEST_CASE(aws_cbor_int, WriteSmallInt);
    RUN_TEST_CASE(aws_cbor_int, WriteInt8);
    RUN_TEST_CASE(aws_cbor_int, WriteInt16);
    RUN_TEST_CASE(aws_cbor_int, WriteInt32);

    RUN_TEST_CASE(aws_cbor_int, ReadSmallPositiveInt);
    RUN_TEST_CASE(aws_cbor_int, ReadPositiveInt8);
    RUN_TEST_CASE(aws_cbor_int, ReadPositiveInt16);
    RUN_TEST_CASE(aws_cbor_int, ReadPositiveInt32);

    RUN_TEST_CASE(aws_cbor_int, WriteInt);

    RUN_TEST_CASE(aws_cbor_int, ReadInt);

    RUN_TEST_CASE(aws_cbor_int, ReadInt_sets_error_if_wrong_type);

    RUN_TEST_CASE(aws_cbor_int, null_checks);
}

TEST(aws_cbor_int, IntSize_SmallInt)
{
    int num = 12;
    CBOR_WriteInt(cbor_data, &num);
    CBOR_SetCursor(cbor_data, 0);
    int result = CBOR_IntSize(cbor_data->cursor);
    TEST_ASSERT_EQUAL(1, result);
}

TEST(aws_cbor_int, IntSize_Int8)
{
    int num = 42;
    CBOR_WriteInt(cbor_data, &num);
    CBOR_SetCursor(cbor_data, 0);
    int result = CBOR_IntSize(cbor_data->cursor);
    TEST_ASSERT_EQUAL(2, result);
}

TEST(aws_cbor_int, IntSize_Int16)
{
    int num = 1234;
    CBOR_WriteInt(cbor_data, &num);
    CBOR_SetCursor(cbor_data, 0);
    int result = CBOR_IntSize(cbor_data->cursor);
    TEST_ASSERT_EQUAL(3, result);
}

TEST(aws_cbor_int, IntSize_Int32)
{
    int num = INT32_MAX;
    CBOR_WriteInt(cbor_data, &num);
    CBOR_SetCursor(cbor_data, 0);
    int result = CBOR_IntSize(cbor_data->cursor);
    TEST_ASSERT_EQUAL(5, result);
}

TEST(aws_cbor_int, WriteSmallInt)
{
    cbor_int_t expected = 23;
    CBOR_WriteSmallInt(cbor_data, expected);
    TEST_ASSERT_EQUAL_INT8(expected, *cbor_data->buffer_start);
}

TEST(aws_cbor_int, WriteInt8)
{
    CBOR_WriteInt8(cbor_data, 0x21);
    cbor_byte_t expected[] = {CBOR_INT8_FOLLOWS, 0x21};
    TEST_ASSERT_EQUAL_HEX8_ARRAY(
        expected, cbor_data->buffer_start, sizeof(expected));
}

TEST(aws_cbor_int, WriteInt16)
{
    CBOR_WriteInt16(cbor_data, 0x1221);
    cbor_byte_t expected[] = {CBOR_INT16_FOLLOWS, 0x12, 0x21};
    TEST_ASSERT_EQUAL_HEX8_ARRAY(
        expected, cbor_data->buffer_start, sizeof(expected));
}

TEST(aws_cbor_int, WriteInt32)
{
    CBOR_WriteInt32(cbor_data, 0xBA5EBA11);
    cbor_byte_t expected[] = {CBOR_INT32_FOLLOWS, 0xBA, 0x5E, 0xBA, 0x11};
    TEST_ASSERT_EQUAL_HEX8_ARRAY(
        expected, cbor_data->buffer_start, sizeof(expected));
}

TEST(aws_cbor_int, ReadSmallPositiveInt)
{
    cbor_byte_t init[] = {
        19,
    };
    CBOR_MemCopy(cbor_data, init, sizeof(init));
    CBOR_SetCursor(cbor_data, 0);
    cbor_int_t result   = CBOR_ReadSmallPositiveInt(cbor_data);
    cbor_int_t expected = 19;
    TEST_ASSERT_EQUAL(expected, result);
}

TEST(aws_cbor_int, ReadPositiveInt8)
{
    cbor_byte_t init[] = {
        CBOR_INT8_FOLLOWS,
        42,
    };
    CBOR_MemCopy(cbor_data, init, sizeof(init));
    CBOR_SetCursor(cbor_data, 0);
    cbor_int_t result   = CBOR_ReadPositiveInt8(cbor_data);
    cbor_int_t expected = 42;
    TEST_ASSERT_EQUAL(expected, result);
}

TEST(aws_cbor_int, ReadPositiveInt16)
{
    cbor_byte_t init[] = {
        CBOR_INT16_FOLLOWS,
        0x01,
        0x02,
    };
    CBOR_MemCopy(cbor_data, init, sizeof(init));
    CBOR_SetCursor(cbor_data, 0);
    cbor_int_t result   = CBOR_ReadPositiveInt16(cbor_data);
    cbor_int_t expected = 258;
    TEST_ASSERT_EQUAL(expected, result);
}

TEST(aws_cbor_int, ReadPositiveInt32)
{
    cbor_byte_t init[] = {
        CBOR_INT16_FOLLOWS,
        0x01,
        0x02,
        0x03,
        0x04,
    };
    CBOR_MemCopy(cbor_data, init, sizeof(init));
    CBOR_SetCursor(cbor_data, 0);
    cbor_int_t result   = CBOR_ReadPositiveInt32(cbor_data);
    cbor_int_t expected = 16909060;
    TEST_ASSERT_EQUAL(expected, result);
}

typedef struct write_test_s {
    char *      message;
    cbor_int_t  input;
    cbor_byte_t expected[32];
} write_test_t;

write_test_t write_tests[] = {
    {
        .message  = "write small cbor_int_t",
        .input    = 8,
        .expected = {8},
    },
    {
        .message  = "write 8 bit cbor_int_t",
        .input    = 56,
        .expected = {CBOR_INT8_FOLLOWS, 56},
    },
    {
        .message  = "write 16 bit cbor_int_t",
        .input    = 0x11FE,
        .expected = {CBOR_INT16_FOLLOWS, 0x11, 0xFE},
    },
    {
        .message  = "write 32 bit cbor_int_t",
        .input    = 0x1337C0DE,
        .expected = {CBOR_INT32_FOLLOWS, 0x13, 0x37, 0xC0, 0xDE},
    },
    {
        .message  = "write largest small positive cbor_int_t",
        .input    = 23,
        .expected = {23},
    },
    {
        .message  = "write smallest 8-bit cbor_int_t",
        .input    = 24,
        .expected = {CBOR_INT8_FOLLOWS, 24},
    },
    {
        .message  = "write largest 8-bit cbor_int_t",
        .input    = 255,
        .expected = {CBOR_INT8_FOLLOWS, 255},
    },
    {
        .message  = "write smallest 16-bit cbor_int_t",
        .input    = 256,
        .expected = {CBOR_INT16_FOLLOWS, 0x01, 0x00},
    },
    {
        .message  = "write largest 16-bit cbor_int_t",
        .input    = 0xFFFF,
        .expected = {CBOR_INT16_FOLLOWS, 0xFF, 0xFF},
    },
    {
        .message = "write smallest 32-bit cbor_int_t",
        .input   = 0x010000,
        .expected =
            {
                CBOR_INT32_FOLLOWS,
                0x00,
                0x01,
                0x00,
                0x00,
            },
    },
    {
        .message = "write largest positive signed 32-bit cbor_int_t",
        .input   = INT32_MAX,
        .expected =
            {
                CBOR_INT32_FOLLOWS,
                0x7F,
                0xFF,
                0xFF,
                0xFF,
            },
    },
};

cbor_int_t write_test_count = (sizeof(write_tests) / sizeof(write_tests[0]));

write_test_t *write_test;

void test_WriteInt(void)
{
    CBOR_SetCursor(cbor_data, 0);
    cbor_byte_t init[32] = {0};
    CBOR_MemCopy(cbor_data, init, sizeof(init));
    CBOR_SetCursor(cbor_data, 0);

    CBOR_WriteInt(cbor_data, &write_test->input);
    TEST_ASSERT_EQUAL_HEX8_ARRAY_MESSAGE(
        write_test->expected, cbor_data->buffer_start, 32, write_test->message);
}

TEST(aws_cbor_int, WriteInt)
{
    for (cbor_int_t i = 0; i < write_test_count; i++) {
        write_test = &write_tests[i];
        RUN_TEST(test_WriteInt);
    }
}

typedef struct read_test_s {
    char *      message;
    cbor_byte_t buffer[32];
    cbor_int_t  expected;
} read_test_t;

read_test_t read_tests[] = {
    {
        .message  = "read small cbor_int_t",
        .buffer   = {13},
        .expected = 13,
    },
    {
        .message  = "read 8-bit cbor_int_t",
        .buffer   = {CBOR_INT8_FOLLOWS, 78},
        .expected = 78,
    },
    {
        .message  = "read 16-bit cbor_int_t",
        .buffer   = {CBOR_INT16_FOLLOWS, 0x12, 0x34},
        .expected = 0x1234,
    },
    {
        .message  = "read 32-bit cbor_int_t",
        .buffer   = {CBOR_INT32_FOLLOWS, 0x09, 0x87, 0x65, 0x43},
        .expected = 0x09876543,
    },
    {
        .message  = "read largest small positive cbor_int_t",
        .expected = 23,
        .buffer   = {23},
    },
    {
        .message  = "read smallest 8-bit cbor_int_t",
        .expected = 24,
        .buffer   = {CBOR_INT8_FOLLOWS, 24},
    },
    {
        .message  = "read largest 8-bit cbor_int_t",
        .expected = 255,
        .buffer   = {CBOR_INT8_FOLLOWS, 255},
    },
    {
        .message  = "read smallest 16-bit cbor_int_t",
        .expected = 256,
        .buffer   = {CBOR_INT16_FOLLOWS, 0x01, 0x00},
    },
    {
        .message  = "read largest 16-bit cbor_int_t",
        .expected = 0xFFFF,
        .buffer   = {CBOR_INT16_FOLLOWS, 0xFF, 0xFF},
    },
    {
        .message  = "read smallest 32-bit cbor_int_t",
        .expected = 0x010000,
        .buffer =
            {
                CBOR_INT32_FOLLOWS,
                0x00,
                0x01,
                0x00,
                0x00,
            },
    },
    {
        .message  = "read largest positive signed 32-bit cbor_int_t",
        .expected = INT32_MAX,
        .buffer =
            {
                CBOR_INT32_FOLLOWS,
                0x7F,
                0xFF,
                0xFF,
                0xFF,
            },
    },
};

cbor_int_t read_test_count = (sizeof(read_tests) / sizeof(read_tests[0]));

read_test_t *read_test;

void test_ReadInt(void)
{
    CBOR_SetCursor(cbor_data, 0);
    CBOR_MemCopy(cbor_data, read_test->buffer, sizeof(read_test->buffer));
    CBOR_SetCursor(cbor_data, 0);

    cbor_int_t result = CBOR_ReadInt(cbor_data);
    TEST_ASSERT_EQUAL_MESSAGE(read_test->expected, result, read_test->message);
}

TEST(aws_cbor_int, ReadInt)
{
    for (cbor_int_t i = 0; i < read_test_count; i++) {
        read_test = &read_tests[i];
        RUN_TEST(test_ReadInt);
    }
}

TEST(aws_cbor_int, ReadInt_sets_error_if_wrong_type)
{
    CBOR_AssignAndIncrementCursor(cbor_data, 0xFF);
    CBOR_SetCursor(cbor_data, 0);
    (void)CBOR_ReadInt(cbor_data);
    TEST_ASSERT_EQUAL(eCBOR_ERR_READ_TYPE_MISMATCH, cbor_data->err);
    cbor_data->err = eCBOR_ERR_NO_ERROR;
}

TEST(aws_cbor_int, null_checks)
{
    uint8_t foo[] = {CBOR_INT8_FOLLOWS, 42};

    TEST_EXPECT_ASSERT(CBOR_IntSize(NULL));

    TEST_EXPECT_ASSERT(CBOR_WriteInt(NULL, foo));
    TEST_EXPECT_ASSERT(CBOR_WriteInt(cbor_data, NULL));

    TEST_EXPECT_ASSERT(CBOR_WriteInt32(NULL, 0));
    TEST_EXPECT_ASSERT(CBOR_WriteInt16(NULL, 0));
    TEST_EXPECT_ASSERT(CBOR_WriteInt8(NULL, 0));
    TEST_EXPECT_ASSERT(CBOR_WriteSmallInt(NULL, 0));

    TEST_EXPECT_ASSERT(CBOR_ReadInt(NULL));

    TEST_EXPECT_ASSERT(CBOR_ReadPositiveInt32(NULL));
    TEST_EXPECT_ASSERT(CBOR_ReadPositiveInt16(NULL));
    TEST_EXPECT_ASSERT(CBOR_ReadPositiveInt8(NULL));
    TEST_EXPECT_ASSERT(CBOR_ReadSmallPositiveInt(NULL));
}
