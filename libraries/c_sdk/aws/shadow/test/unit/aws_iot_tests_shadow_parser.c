/*
 * FreeRTOS Shadow V2.2.0
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

/**
 * @file aws_iot_tests_shadow_parser.c
 * @brief Tests for the Shadow topic name and JSON parser functions.
 */

/* The config header is always included first. */
#include "iot_config.h"

/* Standard includes. */
#include <stdarg.h>
#include <stdio.h>
#include <string.h>

/* Shadow internal include. */
#include "private/aws_iot_shadow_internal.h"

/* JSON utilities include. */
#include "iot_json_utils.h"

/* Test framework includes. */
#include "unity_fixture.h"

/*-----------------------------------------------------------*/

/**
 * @brief The size of the buffers allocated for holding Shadow error documents.
 */
#define ERROR_DOCUMENT_BUFFER_SIZE    ( 128 )

/*-----------------------------------------------------------*/

/**
 * @brief Wrapper for parsing JSON documents and checking the result.
 */
static void _parseJson( bool expectedResult,
                        const char * pJsonDocument,
                        size_t jsonDocumentLength,
                        const char * pJsonKey,
                        const char * pExpectedJsonValue,
                        size_t expectedJsonValueLength )
{
    const char * pJsonValue = NULL;
    size_t jsonValueLength = 0;

    TEST_ASSERT_EQUAL_INT( expectedResult,
                           IotJsonUtils_FindJsonValue( pJsonDocument,
                                                       jsonDocumentLength,
                                                       pJsonKey,
                                                       strlen( pJsonKey ),
                                                       &pJsonValue,
                                                       &jsonValueLength ) );

    if( expectedResult == true )
    {
        TEST_ASSERT_EQUAL( expectedJsonValueLength, jsonValueLength );
        TEST_ASSERT_EQUAL_STRING_LEN( pExpectedJsonValue, pJsonValue, jsonValueLength );
    }
}

/*-----------------------------------------------------------*/

/**
 * @brief Wrapper for generating and parsing error documents.
 */
static void _generateParseErrorDocument( char * pErrorDocument,
                                         AwsIotShadowError_t expectedCode,
                                         const char * pFormat,
                                         ... )
{
    int errorDocumentLength = 0;
    va_list arguments;

    /* Generate an error document. */
    va_start( arguments, pFormat );
    errorDocumentLength = vsnprintf( pErrorDocument,
                                     ERROR_DOCUMENT_BUFFER_SIZE,
                                     pFormat,
                                     arguments );
    va_end( arguments );

    /* Check for errors from vsnprintf. */
    TEST_ASSERT_GREATER_THAN_INT( 0, errorDocumentLength );
    TEST_ASSERT_LESS_THAN_INT( ERROR_DOCUMENT_BUFFER_SIZE, errorDocumentLength );

    /* Parse the error document and check the result. */
    TEST_ASSERT_EQUAL( expectedCode,
                       _AwsIotShadow_ParseErrorDocument( pErrorDocument,
                                                         ( size_t ) errorDocumentLength ) );
}

/*-----------------------------------------------------------*/

/**
 * @brief Wrapper for parsing Shadow Thing Names and checking the result.
 */
static void _parseThingName( const char * pTopicName,
                             AwsIotShadowError_t expectedResult,
                             const char * pExpectedThingName )
{
    AwsIotShadowError_t status = AWS_IOT_SHADOW_STATUS_PENDING;
    uint16_t topicNameLength = ( uint16_t ) strlen( pTopicName );
    const char * pThingName = NULL;
    size_t thingNameLength = 0;

    status = _AwsIotShadow_ParseThingName( pTopicName,
                                           topicNameLength,
                                           &pThingName,
                                           &thingNameLength );
    TEST_ASSERT_EQUAL( expectedResult, status );

    if( expectedResult == AWS_IOT_SHADOW_SUCCESS )
    {
        TEST_ASSERT_EQUAL( strlen( pExpectedThingName ), thingNameLength );
        TEST_ASSERT_EQUAL_STRING_LEN( pExpectedThingName, pThingName, thingNameLength );
    }
}

/*-----------------------------------------------------------*/

/**
 * @brief Test group for Shadow parser tests.
 */
TEST_GROUP( Shadow_Unit_Parser );

/*-----------------------------------------------------------*/

/**
 * @brief Test setup for Shadow parser tests.
 */
TEST_SETUP( Shadow_Unit_Parser )
{
    /* Initialize the Shadow library. */
    TEST_ASSERT_EQUAL( AWS_IOT_SHADOW_SUCCESS, AwsIotShadow_Init( 0 ) );
}

/*-----------------------------------------------------------*/

/**
 * @brief Test tear down for Shadow parser tests.
 */
TEST_TEAR_DOWN( Shadow_Unit_Parser )
{
    /* Clean up the Shadow library. */
    AwsIotShadow_Cleanup();
}

/*-----------------------------------------------------------*/

/**
 * @brief Test group runner for Shadow parser tests.
 */
TEST_GROUP_RUNNER( Shadow_Unit_Parser )
{
    RUN_TEST_CASE( Shadow_Unit_Parser, StatusValid );
    RUN_TEST_CASE( Shadow_Unit_Parser, StatusInvalid );
    RUN_TEST_CASE( Shadow_Unit_Parser, JsonValid );
    RUN_TEST_CASE( Shadow_Unit_Parser, JsonInvalid );
    RUN_TEST_CASE( Shadow_Unit_Parser, ErrorDocument );
    RUN_TEST_CASE( Shadow_Unit_Parser, ErrorDocumentInvalid );
    RUN_TEST_CASE( Shadow_Unit_Parser, ThingName );
}

/*-----------------------------------------------------------*/

/**
 * @brief Tests parsing Shadow status from a valid topic name.
 */
TEST( Shadow_Unit_Parser, StatusValid )
{
    _shadowOperationStatus_t status = _UNKNOWN_STATUS;

    /* Parse "accepted" status. */
    status = _AwsIotShadow_ParseShadowStatus( "$aws/things/Test_device/shadow/accepted",
                                              39 );
    TEST_ASSERT_EQUAL( _SHADOW_ACCEPTED, status );

    /* Parse "rejected" status. */
    status = _AwsIotShadow_ParseShadowStatus( "$aws/things/Test_device/shadow/rejected",
                                              39 );
    TEST_ASSERT_EQUAL( _SHADOW_REJECTED, status );
}

/*-----------------------------------------------------------*/

/**
 * @brief Tests parsing Shadow status from invalid topic names.
 */
TEST( Shadow_Unit_Parser, StatusInvalid )
{
    /* Topic too short. */
    TEST_ASSERT_EQUAL( _UNKNOWN_STATUS,
                       _AwsIotShadow_ParseShadowStatus( "accepted",
                                                        8 ) );

    /* Topic missing last character. */
    TEST_ASSERT_EQUAL( _UNKNOWN_STATUS,
                       _AwsIotShadow_ParseShadowStatus( "$aws/things/Test_device/shadow/accepte",
                                                        38 ) );

    /* Topic missing level separator. */
    TEST_ASSERT_EQUAL( _UNKNOWN_STATUS,
                       _AwsIotShadow_ParseShadowStatus( "$aws/things/Test_device/shadowaccepted",
                                                        38 ) );

    /* Topic suffix isn't "accepted" or "rejected". */
    TEST_ASSERT_EQUAL( _UNKNOWN_STATUS,
                       _AwsIotShadow_ParseShadowStatus( "$aws/things/Test_device/shadow/unknown",
                                                        38 ) );
}

/*-----------------------------------------------------------*/

/**
 * @brief Tests parsing valid JSON documents.
 */
TEST( Shadow_Unit_Parser, JsonValid )
{
    /* Parse JSON document with string, int, bool, and null. */
    {
        const char pJsonDocument[ 81 ] = "{\"name\" \n\r:\n \"John Smith\", \"age\"    :\n\r 30, \n \"isAlive\"  : true, \r \"spouse\":null}";
        size_t jsonDocumentLength = 81;

        _parseJson( true, pJsonDocument, jsonDocumentLength, "name", "\"John Smith\"", 12 );
        _parseJson( true, pJsonDocument, jsonDocumentLength, "age", "30", 2 );
        _parseJson( true, pJsonDocument, jsonDocumentLength, "isAlive", "true", 4 );
        _parseJson( true, pJsonDocument, jsonDocumentLength, "spouse", "null", 4 );

        /* Attempt to find a key not in the JSON document. */
        _parseJson( false, pJsonDocument, jsonDocumentLength, "address", NULL, 0 );
    }

    /* Parse JSON document with objects and arrays. */
    {
        const char pJsonDocument[ 90 ] = "{\"object\" : { \"nestedObject\": { \"string\":\"\\\"test\\\"\", \"array\":[[1,2,3],[1,2,3],[1,2,3]]}}}";
        size_t jsonDocumentLength = 90;

        _parseJson( true, pJsonDocument, jsonDocumentLength, "object", "{ \"nestedObject\": { \"string\":\"\\\"test\\\"\", \"array\":[[1,2,3],[1,2,3],[1,2,3]]}}", 76 );
        _parseJson( true, pJsonDocument, jsonDocumentLength, "nestedObject", "{ \"string\":\"\\\"test\\\"\", \"array\":[[1,2,3],[1,2,3],[1,2,3]]}", 57 );
        _parseJson( true, pJsonDocument, jsonDocumentLength, "array", "[[1,2,3],[1,2,3],[1,2,3]]", 25 );
    }

    /* JSON document with escape sequences. */
    {
        const char pJsonDocument[ 40 ] = "{\"key\": \"value\", \"ke\\\"y2\": \"\\\"value\\\"\"}";
        size_t jsonDocumentLength = 40;

        /* Attempt to find a JSON key that is actually a value. */
        _parseJson( false, pJsonDocument, jsonDocumentLength, "value", NULL, 0 );

        /* Attempt to find a JSON key that is a substring of a key. */
        _parseJson( false, pJsonDocument, jsonDocumentLength, "ke", NULL, 0 );

        /* Find a key and string that contain escaped quotes. */
        _parseJson( true, pJsonDocument, jsonDocumentLength, "ke\\\"y2", "\"\\\"value\\\"\"", 11 );
    }

    /* Short JSON document. */
    {
        const char pJsonDocument[ 16 ] = "{\"key\":\"value\"}";
        size_t jsonDocumentLength = 16;

        /* Attempt to find a key longer than the JSON document. */
        _parseJson( false, pJsonDocument, jsonDocumentLength, "longlonglonglongkey", NULL, 0 );
    }
}

/*-----------------------------------------------------------*/

/**
 * @brief Tests that parsing invalid JSON documents does not read out-of-bounds
 * memory.
 */
TEST( Shadow_Unit_Parser, JsonInvalid )
{
    /* JSON key not followed by a : */
    {
        const char pJsonDocument[ 15 ] = "{\"string\"      ";
        size_t jsonDocumentLength = 15;

        _parseJson( false, pJsonDocument, jsonDocumentLength, "string", NULL, 0 );
    }

    /* JSON value not followed by a , */
    {
        const char pJsonDocument[ 29 ] = "{\"int\": 10 \"string\": \"hello\"}";
        size_t jsonDocumentLength = 29;

        _parseJson( false, pJsonDocument, jsonDocumentLength, "int", NULL, 0 );
    }

    /* JSON key with no value. */
    {
        const char pJsonDocument[ 17 ] = "{\"string\":       ";
        size_t jsonDocumentLength = 17;

        _parseJson( false, pJsonDocument, jsonDocumentLength, "string", NULL, 0 );
    }

    /* Unterminated JSON primitive. */
    {
        const char pJsonDocument[ 11 ] = "{\"int\":1000";
        size_t jsonDocumentLength = 11;

        _parseJson( false, pJsonDocument, jsonDocumentLength, "int", NULL, 0 );
    }

    /* Unterminated JSON string (ending is an escaped quote). */
    {
        const char pJsonDocument[ 14 ] = "{\"string\": \"\\\"";
        size_t jsonDocumentLength = 14;

        _parseJson( false, pJsonDocument, jsonDocumentLength, "string", NULL, 0 );
    }

    /* Unterminated JSON string (ending is not a quote). */
    {
        const char pJsonDocument[ 14 ] = "{\"string\": \" \\";
        size_t jsonDocumentLength = 14;

        _parseJson( false, pJsonDocument, jsonDocumentLength, "string", NULL, 0 );
    }

    /* Unterminated JSON object. */
    {
        const char pJsonDocument[ 41 ] = "{\"object\": {\"key\": { \"nestedKey\":\"value\"}";
        size_t jsonDocumentLength = 41;

        _parseJson( false, pJsonDocument, jsonDocumentLength, "object", NULL, 0 );
    }

    /* Unterminated JSON array. */
    {
        const char pJsonDocument[ 26 ] = "{\"array\": [[1,2,3],[1,2,3]";
        size_t jsonDocumentLength = 26;

        _parseJson( false, pJsonDocument, jsonDocumentLength, "array", NULL, 0 );
    }
}

/*-----------------------------------------------------------*/

/**
 * @brief Tests parsing valid Shadow error documents.
 */
TEST( Shadow_Unit_Parser, ErrorDocument )
{
    size_t i = 0;
    char pErrorDocument[ ERROR_DOCUMENT_BUFFER_SIZE ] = { 0 };
    AwsIotShadowError_t pValidErrorCodes[] =
    {
        AWS_IOT_SHADOW_BAD_REQUEST,
        AWS_IOT_SHADOW_UNAUTHORIZED,
        AWS_IOT_SHADOW_FORBIDDEN,
        AWS_IOT_SHADOW_NOT_FOUND,
        AWS_IOT_SHADOW_CONFLICT,
        AWS_IOT_SHADOW_TOO_LARGE,
        AWS_IOT_SHADOW_UNSUPPORTED,
        AWS_IOT_SHADOW_TOO_MANY_REQUESTS,
        AWS_IOT_SHADOW_SERVER_ERROR
    };

    /* Generate an error document for every valid error code and parse it. */
    for( i = 0; i < ( sizeof( pValidErrorCodes ) / sizeof( pValidErrorCodes[ 0 ] ) ); i++ )
    {
        _generateParseErrorDocument( pErrorDocument,
                                     pValidErrorCodes[ i ],
                                     "{\"code\": %d, \"message\": \"%s\"}",
                                     ( int ) pValidErrorCodes[ i ],
                                     "Test" );
    }
}

/*-----------------------------------------------------------*/

/**
 * @brief Tests parsing invalid Shadow error documents.
 */
TEST( Shadow_Unit_Parser, ErrorDocumentInvalid )
{
    char pErrorDocument[ ERROR_DOCUMENT_BUFFER_SIZE ] = { 0 };

    /* Parse an error document with an unknown code. */
    _generateParseErrorDocument( pErrorDocument,
                                 AWS_IOT_SHADOW_BAD_RESPONSE,
                                 "{\"code\": %d, \"message\": \"%s\"}",
                                 -1,
                                 "Test" );

    /* Parse an error document missing the "code" key. */
    _generateParseErrorDocument( pErrorDocument,
                                 AWS_IOT_SHADOW_BAD_RESPONSE,
                                 "{\"message\": \"Test\"}" );

    /* Parse an error document missing the "message" key. */
    _generateParseErrorDocument( pErrorDocument,
                                 AWS_IOT_SHADOW_BAD_RESPONSE,
                                 "{\"code\": 400}" );

    /* Parse a malformed error document where the code is unterminated. */
    _generateParseErrorDocument( pErrorDocument,
                                 AWS_IOT_SHADOW_BAD_RESPONSE,
                                 "{\"code\": 400" );
}

/*-----------------------------------------------------------*/

/**
 * @brief Tests parsing both valid and invalid Shadow topics.
 */
TEST( Shadow_Unit_Parser, ThingName )
{
    /* Valid operation topic. */
    _parseThingName( "$aws/things/TEST/shadow/get/accepted",
                     AWS_IOT_SHADOW_SUCCESS,
                     "TEST" );

    /* Valid callback topic. */
    _parseThingName( "$aws/things/TEST/shadow/update/delta",
                     AWS_IOT_SHADOW_SUCCESS,
                     "TEST" );

    /* Topic too short. */
    _parseThingName( "$aws/things/TEST/",
                     AWS_IOT_SHADOW_BAD_RESPONSE,
                     "TEST" );

    /* Incorrect prefix. */
    _parseThingName( "$awsshadow/TEST/shadow/update/accepted",
                     AWS_IOT_SHADOW_BAD_RESPONSE,
                     "TEST" );

    /* Thing Name unterminated. */
    _parseThingName( "$aws/things/TESTTESTTESTTESTTESTTEST",
                     AWS_IOT_SHADOW_BAD_RESPONSE,
                     "TESTTESTTESTTESTTESTTEST" );
}

/*-----------------------------------------------------------*/
