/*
 * Amazon FreeRTOS OTA AFQP V1.0.0
 * Copyright (C) 2017 Amazon.com, Inc. or its affiliates.  All Rights Reserved.
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
#include <stdint.h>
#include <stdbool.h>
#include <string.h>

/* FreeRTOS includes. */
#include "FreeRTOS.h"

#include "unity_fixture.h"
#include "unity.h"
#include "jsmn.h"
#include "aws_ota_agent_test_access_declare.h"
#include "aws_ota_agent.h"
#include "aws_clientcredential.h"

/* MQTT includes. */
#include "aws_mqtt_agent.h"

/* Configuration for this test. */
#include "aws_test_ota_config.h"

/**
 * @brief Configuration for this test group.
 */
#define otatestMAX_LOOP_MEM_LEAK_CHECK    ( 1 )
#define otatestSHUTDOWN_WAIT              10000
#define otatestAGENT_INIT_WAIT            10000
#define otatestGARBAGE_JSON               "{sioudgoijergijoijosdigjoeiwgoiew893752379\"}"

#define otatestSTREAM_NAME                "1"
#define otatestFILE_SIZE                  90860
#define otatestFILE_PATH                  "payload.bin"
#define otatestCERT_FILE                  "rsasigner.crt"
#define otatestATTRIBUTES                 3
#define otatestFILE_ID                    0
static const uint8_t otatestSIGNATURE[] =
{
    0x38, 0x78, 0xf9, 0xb0, 0xd8, 0xf1, 0xa8, 0xc3, 0x4a, 0xdd, 0x63, 0x44, 0xc1, 0xbc, 0x9f, 0xb3,
    0xf3, 0xde, 0x49, 0x24, 0xb5, 0x93, 0x32, 0xe4, 0x01, 0x0e, 0x0c, 0x4a, 0xed, 0x32, 0x28, 0xd1,
    0x68, 0x1b, 0x12, 0x6b, 0x50, 0xab, 0x88, 0x5a, 0x18, 0xd0, 0x6d, 0x08, 0x8f, 0x95, 0x77, 0x03,
    0xb5, 0x79, 0x52, 0x5c, 0x4f, 0x32, 0x74, 0x0c, 0x28, 0xc6, 0x79, 0xf0, 0xe6, 0x57, 0x30, 0xa4,
    0x9b, 0x1b, 0x5f, 0x10, 0xf5, 0x2b, 0x53, 0xf0, 0x64, 0x9a, 0x1c, 0xff, 0x79, 0xaf, 0xe5, 0x71,
    0xd7, 0x6e, 0xfb, 0xc4, 0xa0, 0x5a, 0xf8, 0xc8, 0x0d, 0x2b, 0x85, 0x8e, 0x2d, 0xca, 0xb3, 0x03,
    0x05, 0xeb, 0x04, 0xb4, 0xe5, 0x6b, 0x5b, 0x4c, 0x88, 0xe2, 0x63, 0x38, 0x2d, 0xd7, 0xb9, 0x3a,
    0xd7, 0x48, 0xc6, 0x8f, 0x2c, 0x8d, 0x34, 0x8f, 0x19, 0x7a, 0x36, 0x12, 0x3b, 0xa1, 0x9f, 0xe2,
    0xc3, 0x44, 0x3d, 0xe0, 0x29, 0xd6, 0xf5, 0x82, 0xd3, 0xe9, 0xa0, 0x9f, 0xd8, 0x05, 0x09, 0x98,
    0x29, 0x71, 0xc5, 0x43, 0x94, 0x16, 0xe7, 0xc1, 0x8e, 0x4a, 0x50, 0x7d, 0xa6, 0xba, 0xb9, 0xbf,
    0xe3, 0x25, 0xa1, 0x50, 0x80, 0x4e, 0x39, 0xb3, 0x6f, 0xdb, 0x6e, 0xe2, 0x6b, 0x12, 0x71, 0x76,
    0x18, 0xcb, 0x8d, 0x62, 0x90, 0x48, 0x4a, 0xd9, 0xec, 0x9f, 0x97, 0xbf, 0xef, 0xa5, 0xcd, 0xaf,
    0x30, 0xd5, 0xfa, 0xba, 0x1c, 0xb2, 0x79, 0x98, 0x64, 0xbb, 0xd9, 0xda, 0x98, 0x8e, 0x0e, 0x66,
    0x6b, 0x29, 0xef, 0x6b, 0x4b, 0x2f, 0x80, 0xf8, 0xa4, 0x5b, 0x78, 0xfe, 0x70, 0xd6, 0x61, 0x20,
    0x28, 0xf2, 0xc4, 0x00, 0xc2, 0x7b, 0x35, 0x44, 0xd6, 0x3e, 0x8f, 0x9d, 0x8a, 0x7e, 0xf8, 0x2f,
    0x28, 0xa3, 0x77, 0xbb, 0xa1, 0xb7, 0xb2, 0xe1, 0x72, 0x55, 0x0a, 0x31, 0x58, 0x9b, 0xb7, 0x68
};

/*
 * @brief Application-defined callback for the OTA agent.
 */
static void vOTACompleteCallback( OTA_ImageState_t eState )
{
}

/*
 * @brief Test group definition.
 */
TEST_GROUP( Full_OTA_AGENT );

TEST_SETUP( Full_OTA_AGENT )
{
}

TEST_TEAR_DOWN( Full_OTA_AGENT )
{
}

TEST_GROUP_RUNNER( Full_OTA_AGENT )
{
    MQTTAgentHandle_t xMQTTClientHandle;
    MQTTAgentConnectParams_t xConnectParams;
    MQTTAgentReturnCode_t xMqttStatus;
    OTA_State_t xOtaStatus;

    /* Create the MQTT Client. */
    xMqttStatus = MQTT_AGENT_Create( &xMQTTClientHandle );
    TEST_ASSERT_EQUAL_INT( eMQTTAgentSuccess, xMqttStatus );

    /* Connect to the broker. */
    memset( &xConnectParams, 0, sizeof( xConnectParams ) );
    xConnectParams.pucClientId = ( const uint8_t * ) ( clientcredentialIOT_THING_NAME );
    xConnectParams.usClientIdLength = sizeof( clientcredentialIOT_THING_NAME ) - 1; /* Length doesn't include trailing 0. */
    xConnectParams.pcURL = clientcredentialMQTT_BROKER_ENDPOINT;
    xConnectParams.usPort = clientcredentialMQTT_BROKER_PORT;
    xConnectParams.xFlags = mqttagentREQUIRE_TLS;
    xMqttStatus = MQTT_AGENT_Connect(
        xMQTTClientHandle,
        &xConnectParams,
        pdMS_TO_TICKS( otatestAGENT_INIT_WAIT ) );
    TEST_ASSERT_EQUAL_INT( eMQTTAgentSuccess, xMqttStatus );

    /* Initialize the OTA agent. */
    xOtaStatus = OTA_AgentInit(
        xMQTTClientHandle,
        vOTACompleteCallback,
        pdMS_TO_TICKS( otatestAGENT_INIT_WAIT ) );
    TEST_ASSERT_EQUAL_INT( eOTA_AgentState_Ready, xOtaStatus );

    /* Check OTA agent status via separate API. */
    xOtaStatus = OTA_GetAgentState();
    TEST_ASSERT_EQUAL_INT( eOTA_AgentState_Ready, xOtaStatus );

    /* Run OTA-dependent test. */
    RUN_TEST_CASE( Full_OTA_AGENT, prvParseJobDocFromJSONandPrvOTA_Close );

    /* Shutdown the OTA agent. */
    xOtaStatus = OTA_AgentShutdown( pdMS_TO_TICKS( otatestSHUTDOWN_WAIT ) );
    TEST_ASSERT_EQUAL_INT( eOTA_AgentState_NotReady, xOtaStatus );

    /* Disconnect from the MQTT broker. */
    xMqttStatus = MQTT_AGENT_Disconnect(
        xMQTTClientHandle,
        pdMS_TO_TICKS( otatestSHUTDOWN_WAIT ) );
    TEST_ASSERT_EQUAL_INT( eMQTTAgentSuccess, xMqttStatus );

    /* Delete the MQTT handle. */
    xMqttStatus = MQTT_AGENT_Delete( xMQTTClientHandle );
    TEST_ASSERT_EQUAL_INT( eMQTTAgentSuccess, xMqttStatus );
}

TEST( Full_OTA_AGENT, prvParseJobDocFromJSONandPrvOTA_Close )
{
    OTA_FileContext_t * pstUpdateFile;
    uint32_t ulLoopIndex;

    /* Test:
     * 1. That in ideal scenario, the JSON gets correctly processed.
     * 2. look for potential memory leak by alloc-dealloc several times.
     * Start test.
     */
    if( TEST_PROTECT() )
    {
        for( ulLoopIndex = 0; ulLoopIndex < otatestMAX_LOOP_MEM_LEAK_CHECK; ulLoopIndex++ )
        {
            pstUpdateFile = TEST_OTA_prvParseJobDocFromJSON( otatestLASER_JSON, sizeof( otatestLASER_JSON ) );
            TEST_ASSERT_TRUE( pstUpdateFile != NULL );

            /* Check a string and integer conversion */
            TEST_ASSERT_EQUAL_STRING( otatestSTREAM_NAME, pstUpdateFile->pacStreamName );
            TEST_ASSERT_EQUAL( otatestFILE_SIZE, pstUpdateFile->iFileSize );
            TEST_ASSERT_EQUAL_STRING( otatestFILE_PATH, pstUpdateFile->pacFilepath );
            TEST_ASSERT_EQUAL_STRING( otatestCERT_FILE, pstUpdateFile->pacCertFilepath );
            TEST_ASSERT_EQUAL( otatestATTRIBUTES, pstUpdateFile->ulFileAttributes );
            TEST_ASSERT_EQUAL( otatestFILE_ID, pstUpdateFile->ulServerFileID );
            TEST_ASSERT_EQUAL_MEMORY( otatestSIGNATURE, pstUpdateFile->pacSignature, sizeof( otatestSIGNATURE ) );

            TEST_OTA_prvOTA_Close( pstUpdateFile );
        }
    }

    if( pstUpdateFile != NULL )
    {
        TEST_OTA_prvOTA_Close( pstUpdateFile );
    }

    /* End test. */

    /* Test:
     * 1. That null is returned if a field is missing from JSON file
     * 2. That memory is correctly deallocated every time
     * Start test.
     */
    for( ulLoopIndex = 0; ulLoopIndex < otatestMAX_LOOP_MEM_LEAK_CHECK; ulLoopIndex++ )
    {
        if( TEST_PROTECT() )
        {
            pstUpdateFile = TEST_OTA_prvParseJobDocFromJSON( otatestBAD_LASER_JSON, sizeof( otatestBAD_LASER_JSON ) );
            TEST_ASSERT_TRUE( pstUpdateFile == NULL );
        }

        if( pstUpdateFile != NULL )
        {
            TEST_OTA_prvOTA_Close( pstUpdateFile );
        }
    }

    /* End test. */

    /* Test that null is returned if JSON file with incorrect length is passed in parameter.
     * Start test.
     */
    pstUpdateFile = TEST_OTA_prvParseJobDocFromJSON( otatestLASER_JSON, sizeof( otatestLASER_JSON ) / 2 );
    TEST_ASSERT_TRUE( pstUpdateFile == NULL );
    /* End test. */

    /* Test that null is returned if corrupted JSON file is passed in parameter.
     * Start test.
     */
    pstUpdateFile = TEST_OTA_prvParseJobDocFromJSON( otatestGARBAGE_JSON, sizeof( otatestGARBAGE_JSON ) );
    TEST_ASSERT_TRUE( pstUpdateFile == NULL );
    /* End test. */

    /* Test that prvOTA_Close doesn't try to free already freed memory.
     * Start test.
     */
    pstUpdateFile = TEST_OTA_prvParseJobDocFromJSON( otatestLASER_JSON, sizeof( otatestLASER_JSON ) );

    if( pstUpdateFile != NULL )
    {
        vPortFree( pstUpdateFile->pacFilepath );
        pstUpdateFile->pacFilepath = NULL;
        vPortFree( pstUpdateFile->pacCertFilepath );
        pstUpdateFile->pacCertFilepath = NULL;
        vPortFree( pstUpdateFile->pacSignature );
        pstUpdateFile->pacSignature = NULL;
        vPortFree( pstUpdateFile->pacStreamName );
        pstUpdateFile->pacStreamName = NULL;
        TEST_OTA_prvOTA_Close( pstUpdateFile );
    }

    /* End test. */
}
