/*
 * FreeRTOS Serializer V1.1.2
 * Copyright (C) 2020 Amazon.com, Inc. or its affiliates.  All Rights Reserved.
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

/* Unity framework includes. */
#include "unity_fixture.h"
#include "unity.h"

/* Serializer includes. */
#include "iot_serializer.h"

#define _encoder    _IotSerializerJsonEncoder
#define _decoder    _IotSerializerJsonDecoder

static IotSerializerDecoderObject_t rootObject = IOT_SERIALIZER_DECODER_OBJECT_INITIALIZER;
static IotSerializerDecoderObject_t childObject = IOT_SERIALIZER_DECODER_OBJECT_INITIALIZER;

static const uint8_t test_data[] =                                                                                                                                     \
    "{"                                                                                                                                                                \
    "    \"name\" : \"xQueueSend\","                                                                                                                                   \
    "    \"number\" : 3,"                                                                                                                                              \
    "    \"returns\" : { \"type\" :  \"BaseType_t\" },"                                                                                                                \
    "    \"parameters\": [{\"name\" : \"xQueue\", \"index\": 1 }, { \"name\" : \"pvItemToQueue\", \"index\" : 2 }, { \"name\" : \"xTicksToWait\" , \"index\" : 3 }], " \
    "    \"related\" : {"                                                                                                                                              \
    "       \"id\" : \"ABC123\","                                                                                                                                      \
    "       \"types\" : [ {\"type\" : \"QueueHandle_t\" }, {\"type\": \"TickType_t\"}  ]"                                                                              \
    "    }"                                                                                                                                                            \
    "}";

static const uint16_t test_data_length = sizeof( test_data ) / sizeof( test_data[ 0 ] );

TEST_GROUP( Serializer_Unit_JSON_deserialize );

TEST_SETUP( Serializer_Unit_JSON_deserialize )
{
    /* Init decoder object with buffer. */
    TEST_ASSERT_EQUAL( IOT_SERIALIZER_SUCCESS,
                       _decoder.init( &rootObject, test_data, test_data_length ) );
}

TEST_TEAR_DOWN( Serializer_Unit_JSON_deserialize )
{
    /* Destroy encoder object. */
    _decoder.destroy( &rootObject );
    _decoder.destroy( &childObject );
    TEST_ASSERT_NULL( rootObject.u.pHandle );
    TEST_ASSERT_TRUE( ( childObject.type != IOT_SERIALIZER_CONTAINER_ARRAY &&
                        childObject.type != IOT_SERIALIZER_CONTAINER_MAP ) ||
                      childObject.u.pHandle == NULL );
}

TEST_GROUP_RUNNER( Serializer_Unit_JSON_deserialize )
{
    RUN_TEST_CASE( Serializer_Unit_JSON_deserialize, find_key_string_value );
    RUN_TEST_CASE( Serializer_Unit_JSON_deserialize, find_key_object_value );
    RUN_TEST_CASE( Serializer_Unit_JSON_deserialize, find_key_array_of_objects_value );
    RUN_TEST_CASE( Serializer_Unit_JSON_deserialize, find_nested_key_array_of_objects_value );
}

TEST( Serializer_Unit_JSON_deserialize, find_key_string_value )
{
    const char name[] = "xQueueSend";
    IotSerializerError_t json_error = _decoder.find( &rootObject, "name", &childObject );

    TEST_ASSERT_EQUAL( json_error, IOT_SERIALIZER_SUCCESS );
    TEST_ASSERT_EQUAL( childObject.type, IOT_SERIALIZER_SCALAR_TEXT_STRING );
    TEST_ASSERT_EQUAL( childObject.u.value.u.string.length, strlen( name ) );
    TEST_ASSERT_EQUAL( 0, strncmp( ( const char * ) childObject.u.value.u.string.pString, name, strlen( name ) ) );
}

TEST( Serializer_Unit_JSON_deserialize, find_key_object_value )
{
    IotSerializerError_t json_error = _decoder.find( &rootObject, "returns", &childObject );

    TEST_ASSERT_EQUAL( json_error, IOT_SERIALIZER_SUCCESS );
    TEST_ASSERT_EQUAL( childObject.type, IOT_SERIALIZER_CONTAINER_MAP );
}

TEST( Serializer_Unit_JSON_deserialize, find_key_array_of_objects_value )
{
    IotSerializerError_t json_error = _decoder.find( &rootObject, "parameters", &childObject );

    TEST_ASSERT_EQUAL( json_error, IOT_SERIALIZER_SUCCESS );
    TEST_ASSERT_EQUAL( childObject.type, IOT_SERIALIZER_CONTAINER_ARRAY );
}

TEST( Serializer_Unit_JSON_deserialize, find_nested_key_array_of_objects_value )
{
    IotSerializerDecoderObject_t nestedObject;


    IotSerializerError_t json_error = _decoder.find( &rootObject, "related", &nestedObject );

    TEST_ASSERT_EQUAL( json_error, IOT_SERIALIZER_SUCCESS );
    TEST_ASSERT_EQUAL( nestedObject.type, IOT_SERIALIZER_CONTAINER_MAP );

    json_error = _decoder.find( &nestedObject, "types", &childObject );
    TEST_ASSERT_EQUAL( json_error, IOT_SERIALIZER_SUCCESS );
    TEST_ASSERT_EQUAL( childObject.type, IOT_SERIALIZER_CONTAINER_ARRAY );

    _decoder.destroy( &nestedObject );
}
