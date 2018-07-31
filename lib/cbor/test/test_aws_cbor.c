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
#include "aws_cbor_internals.h"
#include "unity_fixture.h"
#include <assert.h>
#include <string.h>

cbor_handle_t cbor_data;

TEST_GROUP(aws_cbor);
TEST_SETUP(aws_cbor)
{
    cbor_data = CBOR_New(0);
}

TEST_TEAR_DOWN(aws_cbor)
{
    CBOR_Delete(&cbor_data);
}

TEST_GROUP_RUNNER(aws_cbor)
{
    RUN_TEST_CASE(aws_cbor, New_returns_not_null);
    RUN_TEST_CASE(aws_cbor, New_returns_null_when_malloc_fails_for_struct);
    RUN_TEST_CASE(aws_cbor, New_returns_null_when_malloc_fails_for_buffer);
    RUN_TEST_CASE(aws_cbor, New_allocates_minimum_space_for_cbor_buffer);
    RUN_TEST_CASE(aws_cbor, New_allocates_specified_space_for_cbor_buffer);
    RUN_TEST_CASE(aws_cbor, New_initializes_buffer_with_empty_map);
    RUN_TEST_CASE(aws_cbor, Delete_frees_all_allocated_space);
    RUN_TEST_CASE(aws_cbor, GetRawBuffer_returns_pointer_to_cbor_buffer);
    RUN_TEST_CASE(aws_cbor, GetBufferSize_returns_size_of_empty_map_in_bytes);
    RUN_TEST_CASE(aws_cbor, GetBuffer_returns_size_of_map_in_bytes);
    RUN_TEST_CASE(aws_cbor, ClearError_sets_err_to_CBOR_ERR_NO_ERROR);
    RUN_TEST_CASE(aws_cbor, AppendMap);
}

TEST(aws_cbor, New_returns_not_null)
{
    cbor_handle_t test_data = CBOR_New(0);
    TEST_ASSERT_NOT_NULL(test_data);
    CBOR_Delete(&test_data);
}

TEST(aws_cbor, New_returns_null_when_malloc_fails_for_struct)
{
    UnityMalloc_MakeMallocFailAfterCount(0);
    cbor_handle_t null_buffer = CBOR_New(0);
    TEST_ASSERT_NULL(null_buffer);
}

TEST(aws_cbor, New_returns_null_when_malloc_fails_for_buffer)
{
    UnityMalloc_MakeMallocFailAfterCount(1);
    cbor_handle_t null_buffer = CBOR_New(0);
    TEST_ASSERT_NULL(null_buffer);
}

TEST(aws_cbor, New_allocates_minimum_space_for_cbor_buffer)
{
    cbor_handle_t test_data = CBOR_New(0);
    TEST_ASSERT_NOT_NULL(test_data->buffer_start);
    cbor_ssize_t size = test_data->buffer_end - test_data->buffer_start + 1;
    TEST_ASSERT_EQUAL(32, size);
    CBOR_Delete(&test_data);
}

TEST(aws_cbor, New_allocates_specified_space_for_cbor_buffer)
{
    cbor_handle_t test_data = CBOR_New(3845);
    TEST_ASSERT_NOT_NULL(test_data->buffer_start);
    cbor_ssize_t size = test_data->buffer_end - test_data->buffer_start + 1;
    TEST_ASSERT_EQUAL(3845, size);
    CBOR_Delete(&test_data);
}

TEST(aws_cbor, New_initializes_buffer_with_empty_map)
{
    cbor_handle_t test_data   = CBOR_New(0);
    cbor_byte_t   empty_map[] = {CBOR_MAP_OPEN, CBOR_BREAK};
    TEST_ASSERT_EQUAL_HEX8_ARRAY(
        empty_map, test_data->buffer_start, sizeof(empty_map));
    CBOR_Delete(&test_data);
}

TEST(aws_cbor, Delete_frees_all_allocated_space)
{
    cbor_handle_t test_data = CBOR_New(0);
    CBOR_Delete(&test_data);
    TEST_ASSERT_NULL(test_data);
}

TEST(aws_cbor, GetRawBuffer_returns_pointer_to_cbor_buffer)
{
    cbor_byte_t const *const actual_ptr   = CBOR_GetRawBuffer(cbor_data);
    cbor_byte_t *            expected_ptr = cbor_data->buffer_start;
    TEST_ASSERT_EQUAL_PTR(expected_ptr, actual_ptr);
}

TEST(aws_cbor, GetBufferSize_returns_size_of_empty_map_in_bytes)
{
    cbor_ssize_t expected_size = 2;
    cbor_ssize_t actual_size   = CBOR_GetBufferSize(cbor_data);
    TEST_ASSERT_EQUAL(expected_size, actual_size);
}

TEST(aws_cbor, GetBuffer_returns_size_of_map_in_bytes)
{
    CBOR_AppendKeyWithInt(cbor_data, "answer", 42);
    cbor_ssize_t expected_size = 11;
    cbor_ssize_t actual_size   = CBOR_GetBufferSize(cbor_data);
    TEST_ASSERT_EQUAL(expected_size, actual_size);
}

TEST(aws_cbor, ClearError_sets_err_to_CBOR_ERR_NO_ERROR)
{
    cbor_data->err = eCBOR_ERR_DEFAULT_ERROR;
    CBOR_ClearError(cbor_data);
    TEST_ASSERT_EQUAL(eCBOR_ERR_NO_ERROR, cbor_data->err);
}

TEST(aws_cbor, AppendMap)
{
    CBOR_AppendKeyWithInt(cbor_data, "answer", 42);

    cbor_handle_t src_data = CBOR_New(0);
    CBOR_AppendKeyWithString(src_data, "question", "unknown");

    CBOR_AppendMap(cbor_data, src_data);
    CBOR_Delete(&src_data);

    bool answer_found   = CBOR_FindKey(cbor_data, "answer");
    bool question_found = CBOR_FindKey(cbor_data, "question");

    // Both keys should be in the first map
    TEST_ASSERT_TRUE(answer_found);
    TEST_ASSERT_TRUE(question_found);
}
