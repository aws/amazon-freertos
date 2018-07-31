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
#include "aws_cbor.h"
#include "aws_cbor_alloc.h"
#include "unity_fixture.h"
#include <string.h>

#ifdef __free_rtos__
#include "FreeRTOS.h"
#include "portable.h"
#define ConfigAllocator()                                                      \
    do {                                                                       \
        pxCBOR_malloc  = pvPortMalloc;                                         \
        pxCBOR_free    = vPortFree;                                            \
        pxCBOR_realloc = CBOR_ReallocImpl;                                     \
    } while (0)
#else
#define ConfigAllocator() (void)(0)
#endif

TEST_GROUP(aws_cbor_acceptance);
TEST_SETUP(aws_cbor_acceptance)
{
    ConfigAllocator();
}

TEST_TEAR_DOWN(aws_cbor_acceptance)
{
}

TEST_GROUP_RUNNER(aws_cbor_acceptance)
{
    RUN_TEST_CASE(aws_cbor_acceptance, assign_and_read_supported_types);
    RUN_TEST_CASE(aws_cbor_acceptance, append_and_read_supported_types);
    RUN_TEST_CASE(
        aws_cbor_acceptance, public_functions_set_err_when_given_null_pointers);
}

TEST(aws_cbor_acceptance, assign_and_read_supported_types)
{
    // Write and then read a the following key value pairs:
    // {
    //     "hello":"world",
    //     "model":"of the modern major general",
    //     "answer":42,
    //     "prime":1033,
    //     "map":{
    //         "direction":"north",
    //         "miles":2000
    //     },
    //     "lorem": "Lorem ipsum..."
    // }

    // Write the key value pairs using Assign functions
    cbor_handle_t cbor_data = CBOR_New(0);
    CBOR_AssignKeyWithString(cbor_data, "hello", "world");
    char *model = "of the modern major general";
    CBOR_AssignKeyWithString(cbor_data, "model", model);
    CBOR_AssignKeyWithInt(cbor_data, "answer", 42);
    cbor_int_t prime = 1033;
    CBOR_AssignKeyWithInt(cbor_data, "prime", prime);

    cbor_handle_t map       = CBOR_New(0);
    char *        direction = "north";
    CBOR_AssignKeyWithString(map, "direction", direction);
    CBOR_AssignKeyWithInt(map, "miles", 2000);
    CBOR_AssignKeyWithMap(cbor_data, "map", map);
    CBOR_Delete(&map);

    char *lorem =
        "Lorem ipsum dolor sit amet, consectetur adipisicing elit, sed do "
        "eiusmod tempor incididunt ut labore et dolore magna aliqua. Ut enim "
        "ad minim veniam, quis nostrud exercitation ullamco laboris nisi ut "
        "aliquip ex ea commodo consequat. Duis aute irure dolor in "
        "reprehenderit in voluptate velit esse cillum dolore eu fugiat nulla "
        "pariatur. Excepteur sint occaecat cupidatat non proident, sunt in "
        "culpa qui officia deserunt mollit anim id est laborum.";
    CBOR_AssignKeyWithString(cbor_data, "lorem", lorem);

    // Read the values from the key value pairs

    char *read_lorem = CBOR_FromKeyReadString(cbor_data, "lorem");
    TEST_ASSERT_EQUAL_STRING(lorem, read_lorem);
    pxCBOR_free(read_lorem);

    char *read_hello = CBOR_FromKeyReadString(cbor_data, "hello");
    TEST_ASSERT_EQUAL_STRING("world", read_hello);
    pxCBOR_free(read_hello);

    char *read_model = CBOR_FromKeyReadString(cbor_data, "model");
    TEST_ASSERT_EQUAL_STRING(model, read_model);
    pxCBOR_free(read_model);

    cbor_int_t read_answer = CBOR_FromKeyReadInt(cbor_data, "answer");
    TEST_ASSERT_EQUAL(42, read_answer);

    cbor_int_t read_prime = CBOR_FromKeyReadInt(cbor_data, "prime");
    TEST_ASSERT_EQUAL(prime, read_prime);

    cbor_handle_t read_map = CBOR_FromKeyReadMap(cbor_data, "map");

    char *read_direction = CBOR_FromKeyReadString(read_map, "direction");
    TEST_ASSERT_EQUAL_STRING(direction, read_direction);
    pxCBOR_free(read_direction);

    cbor_int_t read_miles = CBOR_FromKeyReadInt(read_map, "miles");
    TEST_ASSERT_EQUAL(2000, read_miles);

    CBOR_Delete(&read_map);

    CBOR_Delete(&cbor_data);
}

TEST(aws_cbor_acceptance, append_and_read_supported_types)
{
    // Write and then read a the following key value pairs:
    // {
    //     "hello":"world",
    //     "model":"of the modern major general",
    //     "answer":42,
    //     "prime":1033,
    //     "map":{
    //         "direction":"north",
    //         "miles":2000
    //     },
    //     "lorem": "Lorem ipsum..."
    // }

    // Write the key value pairs using Assign functions
    cbor_handle_t cbor_data = CBOR_New(0);
    CBOR_AppendKeyWithString(cbor_data, "hello", "world");
    char *model = "of the modern major general";
    CBOR_AppendKeyWithString(cbor_data, "model", model);
    CBOR_AppendKeyWithInt(cbor_data, "answer", 42);
    cbor_int_t prime = 1033;
    CBOR_AppendKeyWithInt(cbor_data, "prime", prime);

    cbor_handle_t map       = CBOR_New(0);
    char *        direction = "north";
    CBOR_AppendKeyWithString(map, "direction", direction);
    CBOR_AppendKeyWithInt(map, "miles", 2000);
    CBOR_AppendKeyWithMap(cbor_data, "map", map);
    CBOR_Delete(&map);

    char *lorem =
        "Lorem ipsum dolor sit amet, consectetur adipisicing elit, sed do "
        "eiusmod tempor incididunt ut labore et dolore magna aliqua. Ut enim "
        "ad minim veniam, quis nostrud exercitation ullamco laboris nisi ut "
        "aliquip ex ea commodo consequat. Duis aute irure dolor in "
        "reprehenderit in voluptate velit esse cillum dolore eu fugiat nulla "
        "pariatur. Excepteur sint occaecat cupidatat non proident, sunt in "
        "culpa qui officia deserunt mollit anim id est laborum.";
    CBOR_AppendKeyWithString(cbor_data, "lorem", lorem);

    // Read the values from the key value pairs

    char *read_lorem = CBOR_FromKeyReadString(cbor_data, "lorem");
    TEST_ASSERT_EQUAL_STRING(lorem, read_lorem);
    pxCBOR_free(read_lorem);

    char *read_hello = CBOR_FromKeyReadString(cbor_data, "hello");
    TEST_ASSERT_EQUAL_STRING("world", read_hello);
    pxCBOR_free(read_hello);

    char *read_model = CBOR_FromKeyReadString(cbor_data, "model");
    TEST_ASSERT_EQUAL_STRING(model, read_model);
    pxCBOR_free(read_model);

    cbor_int_t read_answer = CBOR_FromKeyReadInt(cbor_data, "answer");
    TEST_ASSERT_EQUAL(42, read_answer);

    cbor_int_t read_prime = CBOR_FromKeyReadInt(cbor_data, "prime");
    TEST_ASSERT_EQUAL(prime, read_prime);

    cbor_handle_t read_map = CBOR_FromKeyReadMap(cbor_data, "map");

    char *read_direction = CBOR_FromKeyReadString(read_map, "direction");
    TEST_ASSERT_EQUAL_STRING(direction, read_direction);
    pxCBOR_free(read_direction);

    cbor_int_t read_miles = CBOR_FromKeyReadInt(read_map, "miles");
    TEST_ASSERT_EQUAL(2000, read_miles);

    CBOR_Delete(&read_map);

    CBOR_Delete(&cbor_data);
}

TEST(aws_cbor_acceptance, public_functions_set_err_when_given_null_pointers)
{
    // All public functions that take pointers should set the appropriate error
    // when an invalid (e.g. NULL pointer) is passed in.
    cbor_handle_t cbor_data = CBOR_New(0);

    TEST_ASSERT_EQUAL(eCBOR_ERR_NULL_HANDLE, CBOR_CheckError(NULL));

    CBOR_ClearError(NULL);

    CBOR_Delete(NULL);
    cbor_handle_t null_cbor_object = NULL;
    CBOR_Delete(&null_cbor_object);

    TEST_ASSERT_FALSE(CBOR_FindKey(NULL, "key"));
    TEST_ASSERT_FALSE(CBOR_FindKey(cbor_data, NULL));
    TEST_ASSERT_EQUAL(eCBOR_ERR_NULL_KEY, CBOR_CheckError(cbor_data));

    CBOR_AssignKeyWithString(NULL, "key", "value");
    CBOR_AssignKeyWithString(cbor_data, NULL, "value");
    TEST_ASSERT_EQUAL(eCBOR_ERR_NULL_KEY, CBOR_CheckError(cbor_data));
    CBOR_AssignKeyWithString(cbor_data, "key", NULL);
    TEST_ASSERT_EQUAL(eCBOR_ERR_NULL_VALUE, CBOR_CheckError(cbor_data));

    CBOR_AppendKeyWithString(NULL, "key", "value");
    CBOR_AppendKeyWithString(cbor_data, NULL, "value");
    TEST_ASSERT_EQUAL(eCBOR_ERR_NULL_KEY, CBOR_CheckError(cbor_data));
    CBOR_AppendKeyWithString(cbor_data, "key", NULL);
    TEST_ASSERT_EQUAL(eCBOR_ERR_NULL_VALUE, CBOR_CheckError(cbor_data));

    (void)CBOR_FromKeyReadString(NULL, "key");
    (void)CBOR_FromKeyReadString(cbor_data, NULL);
    TEST_ASSERT_EQUAL(eCBOR_ERR_NULL_KEY, CBOR_CheckError(cbor_data));

    CBOR_AssignKeyWithInt(NULL, "key", 123);
    CBOR_AssignKeyWithInt(cbor_data, NULL, 123);
    TEST_ASSERT_EQUAL(eCBOR_ERR_NULL_KEY, CBOR_CheckError(cbor_data));

    CBOR_AppendKeyWithInt(NULL, "key", 123);
    CBOR_AppendKeyWithInt(cbor_data, NULL, 123);
    TEST_ASSERT_EQUAL(eCBOR_ERR_NULL_KEY, CBOR_CheckError(cbor_data));

    (void)CBOR_FromKeyReadInt(NULL, "key");
    (void)CBOR_FromKeyReadInt(cbor_data, NULL);
    TEST_ASSERT_EQUAL(eCBOR_ERR_NULL_KEY, CBOR_CheckError(cbor_data));

    cbor_handle_t map = CBOR_New(0);
    CBOR_AssignKeyWithMap(NULL, "key", map);
    CBOR_AssignKeyWithMap(cbor_data, NULL, map);
    TEST_ASSERT_EQUAL(eCBOR_ERR_NULL_KEY, CBOR_CheckError(cbor_data));
    CBOR_AssignKeyWithMap(cbor_data, "key", NULL);
    TEST_ASSERT_EQUAL(eCBOR_ERR_NULL_VALUE, CBOR_CheckError(cbor_data));

    CBOR_AppendKeyWithMap(NULL, "key", map);
    CBOR_AppendKeyWithMap(cbor_data, NULL, map);
    TEST_ASSERT_EQUAL(eCBOR_ERR_NULL_KEY, CBOR_CheckError(cbor_data));
    CBOR_AppendKeyWithMap(cbor_data, "key", NULL);
    TEST_ASSERT_EQUAL(eCBOR_ERR_NULL_VALUE, CBOR_CheckError(cbor_data));

    (void)CBOR_FromKeyReadMap(NULL, "key");
    (void)CBOR_FromKeyReadMap(cbor_data, NULL);
    TEST_ASSERT_EQUAL(eCBOR_ERR_NULL_KEY, CBOR_CheckError(cbor_data));

    CBOR_Delete(&map);
    CBOR_Delete(&cbor_data);
}
