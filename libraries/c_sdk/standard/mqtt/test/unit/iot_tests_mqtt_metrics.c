/*
 * FreeRTOS MQTT V2.3.1
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
 * @file iot_tests_mqtt_metrics.c
 * @brief Tests for metrics generated for MQTT.
 */
#include <stdio.h>
#include <stdint.h>
#include <string.h>

/* Test framework includes. */
#include "unity_fixture.h"

#include "iot_config.h"

extern void Utils_generateMD5Hash( const char * pData,
                                   size_t dataLength,
                                   uint8_t * pHash,
                                   size_t hashLength );

extern const char * getDeviceIdentifier( void );

extern const char * getDeviceMetrics( void );

extern uint16_t getDeviceMetricsLength( void );


#define MD5_HASH_LENGTH             ( 16 )

#define DEVICE_IDENTIFIER_LENGTH    ( 2 * MD5_HASH_LENGTH )

/*
 * @brief Following test data is copied from RFC 1321 for MD5 hashing.
 */
#define NUM_TEST_DATA               ( 7 )

#define METRICS_PREFIX              "?SDK=" IOT_SDK_NAME "&Version=" IOT_SDK_VERSION "&Platform=" IOT_PLATFORM_NAME

static const char * inputData[] =
{
    "",
    "a",
    "abc",
    "message digest",
    "abcdefghijklmnopqrstuvwxyz",
    "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789",
    "12345678901234567890123456789012345678901234567890123456789012345678901234567890"
};

static const uint8_t expectedOutput[][ 16 ] =
{
    { 0xd4, 0x1d, 0x8c, 0xd9, 0x8f, 0x00, 0xb2, 0x04, 0xe9, 0x80, 0x09, 0x98, 0xec, 0xf8, 0x42, 0x7e },
    { 0x0c, 0xc1, 0x75, 0xb9, 0xc0, 0xf1, 0xb6, 0xa8, 0x31, 0xc3, 0x99, 0xe2, 0x69, 0x77, 0x26, 0x61 },
    { 0x90, 0x01, 0x50, 0x98, 0x3c, 0xd2, 0x4f, 0xb0, 0xd6, 0x96, 0x3f, 0x7d, 0x28, 0xe1, 0x7f, 0x72 },
    { 0xf9, 0x6b, 0x69, 0x7d, 0x7c, 0xb7, 0x93, 0x8d, 0x52, 0x5a, 0x2f, 0x31, 0xaa, 0xf1, 0x61, 0xd0 },
    { 0xc3, 0xfc, 0xd3, 0xd7, 0x61, 0x92, 0xe4, 0x00, 0x7d, 0xfb, 0x49, 0x6c, 0xca, 0x67, 0xe1, 0x3b },
    { 0xd1, 0x74, 0xab, 0x98, 0xd2, 0x77, 0xd9, 0xf5, 0xa5, 0x61, 0x1c, 0x2c, 0x9f, 0x41, 0x9d, 0x9f },
    { 0x57, 0xed, 0xf4, 0xa2, 0x2b, 0xe3, 0xc9, 0x55, 0xac, 0x49, 0xda, 0x2e, 0x21, 0x07, 0xb6, 0x7a }
};

TEST_GROUP( MQTT_Unit_Metrics );

TEST_SETUP( MQTT_Unit_Metrics )
{
}

TEST_TEAR_DOWN( MQTT_Unit_Metrics )
{
}

/**
 * @brief Test group runner for MQTT validate tests.
 */
TEST_GROUP_RUNNER( MQTT_Unit_Metrics )
{
    RUN_TEST_CASE( MQTT_Unit_Metrics, Generate_MD5Hash );
    RUN_TEST_CASE( MQTT_Unit_Metrics, Get_DeviceIdentifier );
    RUN_TEST_CASE( MQTT_Unit_Metrics, Get_DeviceMetrics )
}

/**
 * @brief Tests the generate MD5 hash algorithm against the test data defined in
 * RFC 1321 ( https://tools.ietf.org/html/rfc1321 )
 */
TEST( MQTT_Unit_Metrics, Generate_MD5Hash )
{
    int i;
    uint8_t hash[ MD5_HASH_LENGTH ];

    for( i = 0; i < NUM_TEST_DATA; i++ )
    {
        memset( hash, 0x00, sizeof( hash ) );
        Utils_generateMD5Hash( inputData[ i ], strlen( inputData[ i ] ), hash, sizeof( hash ) );
        TEST_ASSERT_EQUAL( 0, memcmp( expectedOutput[ i ], hash, sizeof( hash ) ) );
    }
}

TEST( MQTT_Unit_Metrics, Get_DeviceIdentifier )
{
    const char * pDeviceCert = keyCLIENT_CERTIFICATE_PEM;
    const char * pDeviceIdentifier = getDeviceIdentifier();

    if( ( pDeviceCert == NULL ) || ( strcmp( pDeviceCert, "" ) == 0 ) )
    {
        TEST_ASSERT_EQUAL( 0, strncmp( pDeviceCert, "Unknown", strlen( pDeviceCert ) ) );
    }
    else
    {
        TEST_ASSERT_EQUAL( DEVICE_IDENTIFIER_LENGTH, strlen( pDeviceIdentifier ) );
    }
}

TEST( MQTT_Unit_Metrics, Get_DeviceMetrics )
{
    const char * pDeviceIdentifier = getDeviceIdentifier();
    const char * pDeviceMetrics = getDeviceMetrics();
    size_t deviceMetricsLength = getDeviceMetricsLength();
    int ret;
    char expected[ DEVICE_IDENTIFIER_LENGTH + 10 ] = { 0 };

    ret = snprintf( expected, sizeof( expected ), "AFRDevID=%s", pDeviceIdentifier );
    TEST_ASSERT( ret > 0 );
    TEST_ASSERT( deviceMetricsLength > 0 );
    TEST_ASSERT_NOT_NULL( strstr( pDeviceMetrics, METRICS_PREFIX ) );
    TEST_ASSERT_NOT_NULL( strstr( pDeviceMetrics, expected ) );
}
