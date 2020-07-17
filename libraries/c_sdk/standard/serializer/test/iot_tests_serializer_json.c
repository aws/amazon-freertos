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

#define _encoder        _IotSerializerJsonEncoder
#define _decoder        _IotSerializerJsonDecoder

#define _BUFFER_SIZE    100

static IotSerializerEncoderObject_t _encoderObject;

static uint8_t _buffer[ _BUFFER_SIZE ];

static void _verifyExpectedString( const char * pExpectedResult );

TEST_GROUP( Serializer_Unit_JSON );

TEST_SETUP( Serializer_Unit_JSON )
{
    /* Reset buffer to zero. */
    memset( _buffer, 0, _BUFFER_SIZE );

    /* Init encoder object with buffer. */
    TEST_ASSERT_EQUAL( IOT_SERIALIZER_SUCCESS,
                       _encoder.init( &_encoderObject, _buffer, _BUFFER_SIZE ) );
}

TEST_TEAR_DOWN( Serializer_Unit_JSON )
{
    /* Destroy encoder object. */
    _encoder.destroy( &_encoderObject );

    TEST_ASSERT_NULL( _encoderObject.pHandle );
}

/* TODO:
 * - append NULL
 * - append bool
 */
TEST_GROUP_RUNNER( Serializer_Unit_JSON )
{
    RUN_TEST_CASE( Serializer_Unit_JSON, Encoder_init_with_null_buffer );

    RUN_TEST_CASE( Serializer_Unit_JSON, Encoder_append_integer );
    RUN_TEST_CASE( Serializer_Unit_JSON, Encoder_append_text_string );
    RUN_TEST_CASE( Serializer_Unit_JSON, Encoder_append_byte_string );

    RUN_TEST_CASE( Serializer_Unit_JSON, Encoder_open_map );
    RUN_TEST_CASE( Serializer_Unit_JSON, Encoder_open_array );

    RUN_TEST_CASE( Serializer_Unit_JSON, Encoder_map_nest_map );
    RUN_TEST_CASE( Serializer_Unit_JSON, Encoder_map_nest_array );
}

TEST( Serializer_Unit_JSON, Encoder_init_with_null_buffer )
{
    IotSerializerEncoderObject_t encoderObject = { .type = ( IotSerializerDataType_t ) 0 };
    IotSerializerEncoderObject_t arrayObject = IOT_SERIALIZER_ENCODER_CONTAINER_INITIALIZER_ARRAY;

    TEST_ASSERT_EQUAL( IOT_SERIALIZER_SUCCESS,
                       _encoder.init( &encoderObject, NULL, 0 ) );

    /* Set the type to stream. */
    TEST_ASSERT_EQUAL( IOT_SERIALIZER_CONTAINER_STREAM, encoderObject.type );

    /* Assigned value to handle pointer. */
    TEST_ASSERT_NOT_NULL( encoderObject.pHandle );

    TEST_ASSERT_EQUAL( IOT_SERIALIZER_BUFFER_TOO_SMALL,
                       _encoder.openContainer( &encoderObject, &arrayObject, 1 ) );

    /* Append an integer. */
    TEST_ASSERT_EQUAL( IOT_SERIALIZER_BUFFER_TOO_SMALL,
                       _encoder.append( &arrayObject, IotSerializer_ScalarSignedInt( 1 ) ) );

    TEST_ASSERT_EQUAL( IOT_SERIALIZER_BUFFER_TOO_SMALL,
                       _encoder.closeContainer( &encoderObject, &arrayObject ) );

    /* Needed 4 bytes for integer 1 (over-calculated). */
    TEST_ASSERT_EQUAL( 6, _encoder.getExtraBufferSizeNeeded( &encoderObject ) );

    _encoder.destroy( &encoderObject );

    TEST_ASSERT_NULL( encoderObject.pHandle );
}

TEST( Serializer_Unit_JSON, Encoder_append_integer )
{
    IotSerializerEncoderObject_t arrayObject = IOT_SERIALIZER_ENCODER_CONTAINER_INITIALIZER_ARRAY;
    int64_t value = 6;

    TEST_ASSERT_EQUAL( IOT_SERIALIZER_SUCCESS,
                       _encoder.openContainer( &_encoderObject, &arrayObject, 1 ) );

    TEST_ASSERT_EQUAL( IOT_SERIALIZER_SUCCESS,
                       _encoder.append( &arrayObject, IotSerializer_ScalarSignedInt( value ) ) );

    TEST_ASSERT_EQUAL( IOT_SERIALIZER_SUCCESS,
                       _encoder.closeContainer( &_encoderObject, &arrayObject ) );

    /* --- Verification --- */
    _verifyExpectedString( "[6]" );
}

TEST( Serializer_Unit_JSON, Encoder_append_text_string )
{
    IotSerializerEncoderObject_t arrayObject = IOT_SERIALIZER_ENCODER_CONTAINER_INITIALIZER_ARRAY;
    char * str = "hello world";

    TEST_ASSERT_EQUAL( IOT_SERIALIZER_SUCCESS,
                       _encoder.openContainer( &_encoderObject, &arrayObject, 1 ) );

    TEST_ASSERT_EQUAL( IOT_SERIALIZER_SUCCESS,
                       _encoder.append( &arrayObject, IotSerializer_ScalarTextString( str ) ) );

    TEST_ASSERT_EQUAL( IOT_SERIALIZER_SUCCESS,
                       _encoder.closeContainer( &_encoderObject, &arrayObject ) );

    /* --- Verification --- */
    _verifyExpectedString( "[\"hello world\"]" );
}

TEST( Serializer_Unit_JSON, Encoder_append_byte_string )
{
    IotSerializerEncoderObject_t arrayObject = IOT_SERIALIZER_ENCODER_CONTAINER_INITIALIZER_ARRAY;
    uint8_t inputBytes[] = "hello world";
    size_t inputLength = strlen( ( const char * ) inputBytes );


    TEST_ASSERT_EQUAL( IOT_SERIALIZER_SUCCESS,
                       _encoder.openContainer( &_encoderObject, &arrayObject, 1 ) );

    TEST_ASSERT_EQUAL( IOT_SERIALIZER_SUCCESS,
                       _encoder.append( &arrayObject, IotSerializer_ScalarByteString( inputBytes, inputLength ) ) );

    TEST_ASSERT_EQUAL( IOT_SERIALIZER_SUCCESS,
                       _encoder.closeContainer( &_encoderObject, &arrayObject ) );

    /* --- Verification --- */
    /* Encoded with base64. */
    /* Original string has 11 bytes, thus 16 bytes after base64 encoding. */
    _verifyExpectedString( "[\"aGVsbG8gd29ybGQ=\"]" );
}

TEST( Serializer_Unit_JSON, Encoder_open_map )
{
    IotSerializerEncoderObject_t mapObject = IOT_SERIALIZER_ENCODER_CONTAINER_INITIALIZER_MAP;

    TEST_ASSERT_EQUAL( IOT_SERIALIZER_SUCCESS,
                       _encoder.openContainer( &_encoderObject, &mapObject, 1 ) );

    TEST_ASSERT_EQUAL( IOT_SERIALIZER_SUCCESS,
                       _encoder.appendKeyValue( &mapObject, "key", IotSerializer_ScalarTextString( "value" ) ) );

    TEST_ASSERT_EQUAL( IOT_SERIALIZER_SUCCESS,
                       _encoder.closeContainer( &_encoderObject, &mapObject ) );

    /* --- Verification --- */
    _verifyExpectedString( "{\"key\":\"value\"}" );
}

TEST( Serializer_Unit_JSON, Encoder_open_array )
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
    _verifyExpectedString( "[3,2,1]" );
}

TEST( Serializer_Unit_JSON, Encoder_map_nest_map )
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
    _verifyExpectedString( "{\"map1\":{\"key\":\"value\"}}" );
}

TEST( Serializer_Unit_JSON, Encoder_map_nest_array )
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
    _verifyExpectedString( "{\"array\":[3,2,1]}" );
}

/*-----------------------------------------------------------*/

static void _verifyExpectedString( const char * pExpectedResult )
{
    TEST_ASSERT_EQUAL( strlen( pExpectedResult ), _encoder.getEncodedSize( &_encoderObject, _buffer ) );
    TEST_ASSERT_EQUAL( 0, strncmp( pExpectedResult, ( const char * ) _buffer, strlen( pExpectedResult ) ) );
}
