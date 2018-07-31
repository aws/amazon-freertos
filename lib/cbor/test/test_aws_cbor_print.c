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

cbor_handle_t cbor_data;

TEST_GROUP(aws_cbor_print);

TEST_SETUP(aws_cbor_print)
{
    cbor_data = CBOR_New(0);
}

TEST_TEAR_DOWN(aws_cbor_print)
{
    CBOR_Delete(&cbor_data);
}

TEST_GROUP_RUNNER(aws_cbor_print)
{
    RUN_TEST_CASE(aws_cbor_print, AsString_prints_empty_map);
    RUN_TEST_CASE(aws_cbor_print, AsString_prints_key_followed_by_int_value);
    RUN_TEST_CASE(aws_cbor_print, AsString_prints_key_followed_by_string_value);
    RUN_TEST_CASE(aws_cbor_print, AsString_prints_key_followed_by_map_value);
    RUN_TEST_CASE(
        aws_cbor_print, AsString_prints_commas_between_key_value_pairs);
}

TEST(aws_cbor_print, AsString_prints_empty_map)
{
    char *expected = "{}";
    char *result   = CBOR_AsString(cbor_data);
    TEST_ASSERT_EQUAL_STRING(expected, result);
    free(result);
}

TEST(aws_cbor_print, AsString_prints_key_followed_by_int_value)
{
    char *expected = "{\"answer\":42}";
    CBOR_AppendKeyWithInt(cbor_data, "answer", 42);
    char *result = CBOR_AsString(cbor_data);
    TEST_ASSERT_EQUAL_STRING(expected, result);
    free(result);
}

TEST(aws_cbor_print, AsString_prints_key_followed_by_string_value)
{
    char *expected = "{\"hello\":\"world\"}";
    CBOR_AppendKeyWithString(cbor_data, "hello", "world");
    char *result = CBOR_AsString(cbor_data);
    TEST_ASSERT_EQUAL_STRING(expected, result);
    free(result);
}

TEST(aws_cbor_print, AsString_prints_key_followed_by_map_value)
{
    char *        expected  = "{\"a map\":{\"key\":\"value\"}}";
    cbor_handle_t inner_map = CBOR_New(0);
    CBOR_AppendKeyWithString(inner_map, "key", "value");
    CBOR_AppendKeyWithMap(cbor_data, "a map", inner_map);
    CBOR_Delete(&inner_map);
    char *result = CBOR_AsString(cbor_data);
    TEST_ASSERT_EQUAL_STRING(expected, result);
    free(result);
}

TEST(aws_cbor_print, AsString_prints_commas_between_key_value_pairs)
{
    char *expected = "{\"a\":1,\"b\":2}";
    CBOR_AppendKeyWithInt(cbor_data, "a", 1);
    CBOR_AppendKeyWithInt(cbor_data, "b", 2);
    char *result = CBOR_AsString(cbor_data);
    TEST_ASSERT_EQUAL_STRING(expected, result);
    free(result);
}
