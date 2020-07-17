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

/* Standard includes. */
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Unity framework includes. */
#include "unity_fixture.h"
#include "unity.h"

/* Serializer and CBOR includes. */
#include "iot_serializer.h"
#include "cbor.h"

#define _encoder        _IotSerializerCborEncoder
#define _decoder        _IotSerializerCborDecoder

#define _BUFFER_SIZE    100

static IotSerializerEncoderObject_t _encoderObject;

uint8_t _buffer[ _BUFFER_SIZE ];

TEST_GROUP( Serializer_Unit_CBOR );

TEST_SETUP( Serializer_Unit_CBOR )
{
    /* Reset buffer to zero. */
    memset( _buffer, 0, _BUFFER_SIZE );

    /* Init encoder object with buffer. */
    TEST_ASSERT_EQUAL( IOT_SERIALIZER_SUCCESS,
                       _encoder.init( &_encoderObject, _buffer, _BUFFER_SIZE ) );
}

TEST_TEAR_DOWN( Serializer_Unit_CBOR )
{
    /* Destroy encoder object. */
    _encoder.destroy( &_encoderObject );

    TEST_ASSERT_NULL( _encoderObject.pHandle );
}

/* TODO:
 * - append NULL
 * - append bool
 */
TEST_GROUP_RUNNER( Serializer_Unit_CBOR )
{
    RUN_TEST_CASE( Serializer_Unit_CBOR, Encoder_init_with_null_buffer );

    RUN_TEST_CASE( Serializer_Unit_CBOR, Encoder_append_integer );
    RUN_TEST_CASE( Serializer_Unit_CBOR, Encoder_append_text_string );
    RUN_TEST_CASE( Serializer_Unit_CBOR, Encoder_append_byte_string );

    RUN_TEST_CASE( Serializer_Unit_CBOR, Encoder_open_a_scalar );
    RUN_TEST_CASE( Serializer_Unit_CBOR, Encoder_open_map );
    RUN_TEST_CASE( Serializer_Unit_CBOR, Encoder_open_array );

    RUN_TEST_CASE( Serializer_Unit_CBOR, Encoder_map_nest_map );
    RUN_TEST_CASE( Serializer_Unit_CBOR, Encoder_map_nest_array );
}

TEST( Serializer_Unit_CBOR, Encoder_init_with_null_buffer )
{
    IotSerializerEncoderObject_t encoderObject = { .type = ( IotSerializerDataType_t ) 0 };

    TEST_ASSERT_EQUAL( IOT_SERIALIZER_SUCCESS,
                       _encoder.init( &encoderObject, NULL, 0 ) );

    /* Set the type to stream. */
    TEST_ASSERT_EQUAL( IOT_SERIALIZER_CONTAINER_STREAM, encoderObject.type );

    /* Assigned value to handle pointer. */
    TEST_ASSERT_NOT_NULL( encoderObject.pHandle );

    /* Append an integer. */
    TEST_ASSERT_EQUAL( IOT_SERIALIZER_BUFFER_TOO_SMALL, _encoder.append( &encoderObject, IotSerializer_ScalarSignedInt( 1 ) ) );

    /* Needed buffer size is 1 to encode integer "1". */
    TEST_ASSERT_EQUAL( 1, _encoder.getExtraBufferSizeNeeded( &encoderObject ) );

    _encoder.destroy( &encoderObject );

    TEST_ASSERT_NULL( encoderObject.pHandle );
}

TEST( Serializer_Unit_CBOR, Encoder_append_integer )
{
    int64_t value = 6;

    TEST_ASSERT_EQUAL( IOT_SERIALIZER_SUCCESS,
                       _encoder.append( &_encoderObject, IotSerializer_ScalarSignedInt( value ) ) );

    /* --- Verification --- */

    CborParser parser;
    CborValue outermostValue;

    TEST_ASSERT_EQUAL( CborNoError,
                       cbor_parser_init( _buffer, _BUFFER_SIZE, 0, &parser, &outermostValue ) );

    TEST_ASSERT_EQUAL( CborIntegerType, cbor_value_get_type( &outermostValue ) );

    int64_t result = 0;
    TEST_ASSERT_EQUAL( CborNoError, cbor_value_get_int64( &outermostValue, &result ) );

    TEST_ASSERT_EQUAL( value, result );

    /* Encoded size is 1. */
    TEST_ASSERT_EQUAL( 1, _encoder.getEncodedSize( &_encoderObject, _buffer ) );
}

TEST( Serializer_Unit_CBOR, Encoder_append_text_string )
{
    char * str = "hello world";

    TEST_ASSERT_EQUAL( IOT_SERIALIZER_SUCCESS,
                       _encoder.append( &_encoderObject, IotSerializer_ScalarTextString( str ) ) );

    /* --- Verification --- */

    CborParser parser;
    CborValue outermostValue;

    TEST_ASSERT_EQUAL( CborNoError,
                       cbor_parser_init( _buffer, _BUFFER_SIZE, 0, &parser, &outermostValue ) );

    TEST_ASSERT_EQUAL( CborTextStringType, cbor_value_get_type( &outermostValue ) );

    bool equal = false;
    TEST_ASSERT_EQUAL( CborNoError,
                       cbor_value_text_string_equals( &outermostValue, str, &equal ) );

    TEST_ASSERT_TRUE( equal );
}

TEST( Serializer_Unit_CBOR, Encoder_append_byte_string )
{
    uint8_t inputBytes[] = "hello world";
    size_t inputLength = strlen( ( const char * ) inputBytes );

    TEST_ASSERT_EQUAL( IOT_SERIALIZER_SUCCESS,
                       _encoder.append( &_encoderObject, IotSerializer_ScalarByteString( inputBytes, inputLength ) ) );

    /* --- Verification --- */

    CborParser parser;
    CborValue outermostValue;

    TEST_ASSERT_EQUAL( CborNoError,
                       cbor_parser_init( _buffer, _BUFFER_SIZE, 0, &parser, &outermostValue ) );

    TEST_ASSERT_EQUAL( CborByteStringType, cbor_value_get_type( &outermostValue ) );

    uint8_t outputBytes[ 20 ];
    size_t outputLength = 20;

    TEST_ASSERT_EQUAL( CborNoError,
                       cbor_value_copy_byte_string( &outermostValue, outputBytes, &outputLength, NULL ) );

    TEST_ASSERT_EQUAL( inputLength, outputLength );

    TEST_ASSERT_EQUAL( 0, strcmp( ( const char * ) inputBytes, ( const char * ) outputBytes ) );
}

TEST( Serializer_Unit_CBOR, Encoder_open_a_scalar )
{
    IotSerializerEncoderObject_t integerObject = { .pHandle = NULL, .type = IOT_SERIALIZER_SCALAR_SIGNED_INT };

    TEST_ASSERT_EQUAL( IOT_SERIALIZER_INVALID_INPUT,
                       _encoder.openContainer( &_encoderObject, &integerObject, 1 ) );
}

TEST( Serializer_Unit_CBOR, Encoder_open_map )
{
    IotSerializerEncoderObject_t mapObject = IOT_SERIALIZER_ENCODER_CONTAINER_INITIALIZER_MAP;

    TEST_ASSERT_EQUAL( IOT_SERIALIZER_SUCCESS,
                       _encoder.openContainer( &_encoderObject, &mapObject, 1 ) );

    TEST_ASSERT_EQUAL( IOT_SERIALIZER_SUCCESS,
                       _encoder.appendKeyValue( &mapObject, "key", IotSerializer_ScalarTextString( "value" ) ) );

    TEST_ASSERT_EQUAL( IOT_SERIALIZER_SUCCESS,
                       _encoder.closeContainer( &_encoderObject, &mapObject ) );

    /* --- Verification --- */

    CborParser parser;
    CborValue outermostValue, value;

    TEST_ASSERT_EQUAL( CborNoError,
                       cbor_parser_init( _buffer, _BUFFER_SIZE, 0, &parser, &outermostValue ) );

    TEST_ASSERT_EQUAL( CborMapType, cbor_value_get_type( &outermostValue ) );

    TEST_ASSERT_EQUAL( CborNoError,
                       cbor_value_map_find_value( &outermostValue, "key", &value ) );

    TEST_ASSERT_EQUAL( CborTextStringType, cbor_value_get_type( &value ) );

    bool equal = false;
    TEST_ASSERT_EQUAL( CborNoError,
                       cbor_value_text_string_equals( &value, "value", &equal ) );

    TEST_ASSERT_TRUE( equal );
}

TEST( Serializer_Unit_CBOR, Encoder_open_array )
{
    uint8_t i = 0;
    IotSerializerEncoderObject_t arrayObject = IOT_SERIALIZER_ENCODER_CONTAINER_INITIALIZER_ARRAY;

    int64_t numberArray[] = { 3, 2, 1 };

    TEST_ASSERT_EQUAL( IOT_SERIALIZER_SUCCESS,
                       _encoder.openContainer( &_encoderObject, &arrayObject, 3 ) );

    for( i = 0; i < 3; i++ )
    {
        TEST_ASSERT_EQUAL( IOT_SERIALIZER_SUCCESS,
                           _encoder.append( &arrayObject, IotSerializer_ScalarSignedInt( numberArray[ i ] ) ) );
    }

    TEST_ASSERT_EQUAL( IOT_SERIALIZER_SUCCESS,
                       _encoder.closeContainer( &_encoderObject, &arrayObject ) );

    /* --- Verification --- */

    CborParser parser;
    CborValue outermostValue, arrayValue;
    int64_t number = 0;

    TEST_ASSERT_EQUAL( CborNoError,
                       cbor_parser_init( _buffer, _BUFFER_SIZE, 0, &parser, &outermostValue ) );

    TEST_ASSERT_EQUAL( CborArrayType, cbor_value_get_type( &outermostValue ) );

    TEST_ASSERT_EQUAL( CborNoError,
                       cbor_value_enter_container( &outermostValue, &arrayValue ) );

    for( i = 0; i < 3; i++ )
    {
        TEST_ASSERT_EQUAL( CborIntegerType, cbor_value_get_type( &arrayValue ) );
        TEST_ASSERT_EQUAL( CborNoError, cbor_value_get_int64( &arrayValue, &number ) );
        TEST_ASSERT_EQUAL( numberArray[ i ], number );

        TEST_ASSERT_EQUAL( CborNoError, cbor_value_advance( &arrayValue ) );
    }

    TEST_ASSERT_TRUE( cbor_value_at_end( &arrayValue ) );
}

TEST( Serializer_Unit_CBOR, Encoder_map_nest_map )
{
    IotSerializerEncoderObject_t mapObject_1 = IOT_SERIALIZER_ENCODER_CONTAINER_INITIALIZER_MAP;
    IotSerializerEncoderObject_t mapObject_2 = IOT_SERIALIZER_ENCODER_CONTAINER_INITIALIZER_MAP;

    TEST_ASSERT_EQUAL( IOT_SERIALIZER_SUCCESS,
                       _encoder.openContainer( &_encoderObject, &mapObject_1, 1 ) );

    TEST_ASSERT_EQUAL( IOT_SERIALIZER_SUCCESS,
                       _encoder.openContainerWithKey( &mapObject_1, "map1", &mapObject_2, 1 ) );

    TEST_ASSERT_EQUAL( IOT_SERIALIZER_SUCCESS,
                       _encoder.appendKeyValue( &mapObject_2, "key", IotSerializer_ScalarTextString( "value" ) ) );

    TEST_ASSERT_EQUAL( IOT_SERIALIZER_SUCCESS,
                       _encoder.closeContainer( &mapObject_1, &mapObject_2 ) );

    TEST_ASSERT_EQUAL( IOT_SERIALIZER_SUCCESS,
                       _encoder.closeContainer( &_encoderObject, &mapObject_1 ) );

    /* --- Verification --- */

    CborParser parser;
    CborValue outermostValue, map1, str;

    TEST_ASSERT_EQUAL( CborNoError,
                       cbor_parser_init( _buffer, _BUFFER_SIZE, 0, &parser, &outermostValue ) );

    TEST_ASSERT_EQUAL( CborMapType, cbor_value_get_type( &outermostValue ) );

    TEST_ASSERT_EQUAL( CborNoError,
                       cbor_value_map_find_value( &outermostValue, "map1", &map1 ) );

    TEST_ASSERT_EQUAL( CborMapType, cbor_value_get_type( &map1 ) );

    TEST_ASSERT_EQUAL( CborNoError,
                       cbor_value_map_find_value( &map1, "key", &str ) );

    TEST_ASSERT_EQUAL( CborTextStringType, cbor_value_get_type( &str ) );

    bool equal = false;
    TEST_ASSERT_EQUAL( CborNoError,
                       cbor_value_text_string_equals( &str, "value", &equal ) );

    TEST_ASSERT_TRUE( equal );
}

TEST( Serializer_Unit_CBOR, Encoder_map_nest_array )
{
    uint8_t i = 0;
    IotSerializerEncoderObject_t mapObject = IOT_SERIALIZER_ENCODER_CONTAINER_INITIALIZER_MAP;
    IotSerializerEncoderObject_t arrayObject = IOT_SERIALIZER_ENCODER_CONTAINER_INITIALIZER_ARRAY;

    int64_t numberArray[] = { 3, 2, 1 };

    TEST_ASSERT_EQUAL( IOT_SERIALIZER_SUCCESS,
                       _encoder.openContainer( &_encoderObject, &mapObject, 1 ) );

    TEST_ASSERT_EQUAL( IOT_SERIALIZER_SUCCESS,
                       _encoder.openContainerWithKey( &mapObject, "array", &arrayObject, 3 ) );

    for( i = 0; i < 3; i++ )
    {
        TEST_ASSERT_EQUAL( IOT_SERIALIZER_SUCCESS,
                           _encoder.append( &arrayObject, IotSerializer_ScalarSignedInt( numberArray[ i ] ) ) );
    }

    TEST_ASSERT_EQUAL( IOT_SERIALIZER_SUCCESS,
                       _encoder.closeContainer( &mapObject, &arrayObject ) );

    TEST_ASSERT_EQUAL( IOT_SERIALIZER_SUCCESS,
                       _encoder.closeContainer( &_encoderObject, &mapObject ) );

    /* --- Verification --- */

    CborParser parser;
    CborValue outermost, array, arrayElement;
    int64_t number = 0;

    TEST_ASSERT_EQUAL( CborNoError,
                       cbor_parser_init( _buffer, _BUFFER_SIZE, 0, &parser, &outermost ) );

    TEST_ASSERT_EQUAL( CborMapType, cbor_value_get_type( &outermost ) );

    TEST_ASSERT_EQUAL( CborNoError,
                       cbor_value_map_find_value( &outermost, "array", &array ) );

    TEST_ASSERT_EQUAL( CborArrayType, cbor_value_get_type( &array ) );

    TEST_ASSERT_EQUAL( CborNoError,
                       cbor_value_enter_container( &array, &arrayElement ) );

    for( i = 0; i < 3; i++ )
    {
        TEST_ASSERT_EQUAL( CborIntegerType, cbor_value_get_type( &arrayElement ) );
        TEST_ASSERT_EQUAL( CborNoError, cbor_value_get_int64( &arrayElement, &number ) );
        TEST_ASSERT_EQUAL( numberArray[ i ], number );

        TEST_ASSERT_EQUAL( CborNoError, cbor_value_advance( &arrayElement ) );
    }

    TEST_ASSERT_TRUE( cbor_value_at_end( &arrayElement ) );
}
