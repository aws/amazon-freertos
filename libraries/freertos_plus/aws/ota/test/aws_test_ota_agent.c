/*
 * FreeRTOS OTA V1.2.0
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

/* MQTT includes. */
#include "iot_mqtt.h"
#include "iot_init.h"

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
#include "aws_iot_ota_agent.h"
#include "aws_clientcredential.h"
#include "aws_iot_ota_agent_internal.h"

/* Test network header include. */
#include IOT_TEST_NETWORK_HEADER

/* Test framework includes. */
#include "aws_test_utils.h"

/* Configuration for this test. */
#include "aws_test_ota_config.h"

/**
 * @brief Configuration for this test group.
 */
#define otatestMQTT_RETRIES_START_MS      ( 250 )
#define otatestMQTT_RETRIES_MAX           ( 7 )
#define otatestMAX_LOOP_MEM_LEAK_CHECK    ( 1 )
#define otatestSHUTDOWN_WAIT              pdMS_TO_TICKS( 10000 )
#define otatestAGENT_INIT_WAIT            10000
#define otatestGARBAGE_JSON               "{sioudgoijergijoijosdigjoeiwgoiew893752379\"}"

#define otatestSTREAM_NAME                "1"
#define otatestFILE_SIZE                  90860
#define otatestFILE_PATH                  "payload.bin"
#define otatestCERT_FILE                  "rsasigner.crt"
#define otatestATTRIBUTES                 3
#define otatestFILE_ID                    0
static const uint8_t ucOtatestSIGNATURE[] =
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

/**
 * @brief Job document, with all required fields, that is expected to parse.
 */
#define otatestLASER_JSON                        "{\"clientToken\":\"mytoken\",\"timestamp\":1508445004,\"execution\":{\"jobId\":\"15\",\"status\":\"QUEUED\",\"queuedAt\":1507697924,\"lastUpdatedAt\":1507697924,\"versionNumber\":1,\"executionNumber\":1,\"jobDocument\":{\"afr_ota\": {\"protocols\":[\"MQTT\"],\"streamname\": \"1\",\"files\": [{\"filepath\": \"payload.bin\",\"version\":\"1.0.0.0\",\"filesize\": 90860,\"fileid\": 0,\"attr\": 3,\"certfile\":\"rsasigner.crt\", \"" otatestVALID_SIG_METHOD "\":\"OHj5sNjxqMNK3WNEwbyfs/PeSSS1kzLkAQ4MSu0yKNFoGxJrUKuIWhjQbQiPlXcDtXlSXE8ydAwoxnnw5lcwpJsbXxD1K1PwZJoc/3mv5XHXbvvEoFr4yA0rhY4tyrMDBesEtOVrW0yI4mM4Lde5OtdIxo8sjTSPGXo2Ejuhn+LDRD3gKdb1gtPpoJ/YBQmYKXHFQ5QW58GOSlB9prq5v+MloVCATjmzb9tu4msScXYYy41ikEhK2eyfl7/vpc2vMNX6uhyyeZhku9namI4OZmsp72tLL4D4pFt4/nDWYSAo8sQAwns1RNY+j52KfvgvKKN3u6G3suFyVQoxWJu3aA==\"}]}}}}"

/**
 * @brief Job document that is missing a required field.
 */
#define otatestBAD_LASER_JSON                    "{\"clientToken\":\"mytoken\",\"timestamp\":1508445004,\"execution\":{\"jobId\":\"15\",\"status\":\"QUEUED\",\"queuedAt\":1507697924,\"lastUpdatedAt\":1507697924,\"versionNumber\":1,\"executionNumber\":1,\"jobDocument\":{\"afr_ota\": {\"protocols\":[\"MQTT\"],\"streamname\": \"1\",\"files\": [{\"filepath\": \"payload.bin\",\"version\":\"1.0.0.0\",\"filesize\": 90860,\"fileid\": 0,\"attr\": 3,\"certfile\":\"rsasigner.crt\", \"" otatestINVALID_SIG_METHOD "\":\"OHj5sNjxqMNK3WNEwbyfs/PeSSS1kzLkAQ4MSu0yKNFoGxJrUKuIWhjQbQiPlXcDtXlSXE8ydAwoxnnw5lcwpJsbXxD1K1PwZJoc/3mv5XHXbvvEoFr4yA0rhY4tyrMDBesEtOVrW0yI4mM4Lde5OtdIxo8sjTSPGXo2Ejuhn+LDRD3gKdb1gtPpoJ/YBQmYKXHFQ5QW58GOSlB9prq5v+MloVCATjmzb9tu4msScXYYy41ikEhK2eyfl7/vpc2vMNX6uhyyeZhku9namI4OZmsp72tLL4D4pFt4/nDWYSAo8sQAwns1RNY+j52KfvgvKKN3u6G3suFyVQoxWJu3aA==\"}]}}}}"

/**
 * @brief Has a duplicate JSON field, but otherwise valid.
 */
#define otatestLASER_JSON_WITH_DUPLICATE         "{\"clientToken\":\"mytoken0\",\"clientToken\":\"mytoken\",\"timestamp\":1508445004,\"execution\":{\"jobId\":\"15\",\"status\":\"QUEUED\",\"queuedAt\":1507697924,\"lastUpdatedAt\":1507697924,\"versionNumber\":1,\"executionNumber\":1,\"jobDocument\":{\"afr_ota\": {\"streamname\": \"1\",\"files\": [{\"filepath\": \"payload.bin\",\"version\":\"1.0.0.0\",\"filesize\": 90860,\"fileid\": 0,\"attr\": 3,\"certfile\":\"rsasigner.crt\", \"" otatestVALID_SIG_METHOD "\":\"OHj5sNjxqMNK3WNEwbyfs/PeSSS1kzLkAQ4MSu0yKNFoGxJrUKuIWhjQbQiPlXcDtXlSXE8ydAwoxnnw5lcwpJsbXxD1K1PwZJoc/3mv5XHXbvvEoFr4yA0rhY4tyrMDBesEtOVrW0yI4mM4Lde5OtdIxo8sjTSPGXo2Ejuhn+LDRD3gKdb1gtPpoJ/YBQmYKXHFQ5QW58GOSlB9prq5v+MloVCATjmzb9tu4msScXYYy41ikEhK2eyfl7/vpc2vMNX6uhyyeZhku9namI4OZmsp72tLL4D4pFt4/nDWYSAo8sQAwns1RNY+j52KfvgvKKN3u6G3suFyVQoxWJu3aA==\"}]}}}}"

/**
 * @brief Job document with an invalid client token (is a primitive instead of a string).
 */
#define otatestLASER_JSON_WITH_FIELD_MISMATCH    "{\"clientToken\":5,\"timestamp\":1508445004,\"execution\":{\"jobId\":\"15\",\"status\":\"QUEUED\",\"queuedAt\":1507697924,\"lastUpdatedAt\":1507697924,\"versionNumber\":1,\"executionNumber\":1,\"jobDocument\":{\"afr_ota\": {\"streamname\": \"1\",\"files\": [{\"filepath\": \"payload.bin\",\"version\":\"1.0.0.0\",\"filesize\": 90860,\"fileid\": 0,\"attr\": 3,\"certfile\":\"rsasigner.crt\", \"" otatestVALID_SIG_METHOD "\":\"OHj5sNjxqMNK3WNEwbyfs/PeSSS1kzLkAQ4MSu0yKNFoGxJrUKuIWhjQbQiPlXcDtXlSXE8ydAwoxnnw5lcwpJsbXxD1K1PwZJoc/3mv5XHXbvvEoFr4yA0rhY4tyrMDBesEtOVrW0yI4mM4Lde5OtdIxo8sjTSPGXo2Ejuhn+LDRD3gKdb1gtPpoJ/YBQmYKXHFQ5QW58GOSlB9prq5v+MloVCATjmzb9tu4msScXYYy41ikEhK2eyfl7/vpc2vMNX6uhyyeZhku9namI4OZmsp72tLL4D4pFt4/nDWYSAo8sQAwns1RNY+j52KfvgvKKN3u6G3suFyVQoxWJu3aA==\"}]}}}}"

/**
 * @brief Job document with an invalid base64 signature.
 */
#define otatestLASER_JSON_WITH_BAD_BASE64        "{\"clientToken\":\"mytoken\",\"timestamp\":1508445004,\"execution\":{\"jobId\":\"15\",\"status\":\"QUEUED\",\"queuedAt\":1507697924,\"lastUpdatedAt\":1507697924,\"versionNumber\":1,\"executionNumber\":1,\"jobDocument\":{\"afr_ota\": {\"streamname\": \"1\",\"files\": [{\"filepath\": \"payload.bin\",\"version\":\"1.0.0.0\",\"filesize\": 90860,\"fileid\": 0,\"attr\": 3,\"certfile\":\"rsasigner.crt\", \"" otatestVALID_SIG_METHOD "\":\"(Hj5sNjxqMNK3WNEwbyfs/PeSSS1kzLkAQ4MSu0yKNFoGxJrUKuIWhjQbQiPlXcDtXlSXE8ydAwoxnnw5lcwpJsbXxD1K1PwZJoc/3mv5XHXbvvEoFr4yA0rhY4tyrMDBesEtOVrW0yI4mM4Lde5OtdIxo8sjTSPGXo2Ejuhn+LDRD3gKdb1gtPpoJ/YBQmYKXHFQ5QW58GOSlB9prq5v+MloVCATjmzb9tu4msScXYYy41ikEhK2eyfl7/vpc2vMNX6uhyyeZhku9namI4OZmsp72tLL4D4pFt4/nDWYSAo8sQAwns1RNY+j52KfvgvKKN3u6G3suFyVQoxWJu3aA==\"}]}}}}"

/**
 * @brief Valid job document; includes the "self_test" key.
 */
#define otatestLASER_JSON_WITH_SELF_TEST         "{\"clientToken\":\"mytoken\",\"timestamp\":1508445004,\"execution\":{\"self_test\":\"true\",\"jobId\":\"15\",\"status\":\"QUEUED\",\"queuedAt\":1507697924,\"lastUpdatedAt\":1507697924,\"versionNumber\":1,\"executionNumber\":1,\"jobDocument\":{\"afr_ota\": {\"streamname\": \"1\",\"files\": [{\"filepath\": \"payload.bin\",\"version\":\"1.0.0.0\",\"filesize\": 90860,\"fileid\": 0,\"attr\": 3,\"certfile\":\"rsasigner.crt\", \"" otatestVALID_SIG_METHOD "\":\"OHj5sNjxqMNK3WNEwbyfs/PeSSS1kzLkAQ4MSu0yKNFoGxJrUKuIWhjQbQiPlXcDtXlSXE8ydAwoxnnw5lcwpJsbXxD1K1PwZJoc/3mv5XHXbvvEoFr4yA0rhY4tyrMDBesEtOVrW0yI4mM4Lde5OtdIxo8sjTSPGXo2Ejuhn+LDRD3gKdb1gtPpoJ/YBQmYKXHFQ5QW58GOSlB9prq5v+MloVCATjmzb9tu4msScXYYy41ikEhK2eyfl7/vpc2vMNX6uhyyeZhku9namI4OZmsp72tLL4D4pFt4/nDWYSAo8sQAwns1RNY+j52KfvgvKKN3u6G3suFyVQoxWJu3aA==\"}]}}}}"

/**
 * @brief Shared MQTT client handle, used across setup, tests, and teardown.
 * But only used by one test at a time. */
static IotMqttConnection_t xMQTTClientHandle = NULL;

/**
 * @brief Connection context for OTA agent.
 */
static OTA_ConnectionContext_t xOTAConnContext = { NULL, NULL, NULL };

/**
 * @brief Initialize OTA agent. Some tests don't use an initialized OTA Agent, so this isn't done in SETUP.
 */
static OTA_State_t prvOTAAgentInit()
{
    OTA_State_t eOtaStatus = eOTA_AgentState_Init;
    TickType_t xTicksToWait = pdMS_TO_TICKS( otatestAGENT_INIT_WAIT );

    eOtaStatus = OTA_AgentInit(
        &xOTAConnContext,
        ( const uint8_t * ) clientcredentialIOT_THING_NAME,
        NULL,
        xTicksToWait );

    if( eOtaStatus != eOTA_AgentState_Ready )
    {
        configPRINTF( ( "Failed to initialize OTA agent" ) );
    }
    else
    {
        /* Wait for OTA agent to transit to eOTA_AgentState_WaitingForJob state.*/
        while( ( xTicksToWait > 0U ) && ( eOtaStatus != eOTA_AgentState_WaitingForJob ) )
        {
            eOtaStatus = OTA_GetAgentState();
            vTaskDelay( 1 );
            xTicksToWait--;
        }
    }

    return eOtaStatus;
}

/**
 * @brief Test group definition.
 */
TEST_GROUP( Full_OTA_AGENT );

/**
 * @brief Set up the MQTT Agent before running any OTA tests.
 */
TEST_SETUP( Full_OTA_AGENT )
{
    IotMqttError_t connectStatus = IOT_MQTT_STATUS_PENDING;
    struct IotNetworkServerInfo serverInfo = IOT_TEST_NETWORK_SERVER_INFO_INITIALIZER;
    struct IotNetworkCredentials credentials = IOT_TEST_NETWORK_CREDENTIALS_INITIALIZER;
    IotMqttNetworkInfo_t networkInfo = IOT_MQTT_NETWORK_INFO_INITIALIZER;
    IotMqttConnectInfo_t connectInfo = IOT_MQTT_CONNECT_INFO_INITIALIZER;

    TEST_ASSERT_EQUAL_INT_MESSAGE( true, IotSdk_Init(), "Failed to initialize IoT SDK in TEST_SETUP." );
    TEST_ASSERT_EQUAL_INT_MESSAGE( IOT_MQTT_SUCCESS, IotMqtt_Init(), "Failed to initialize MQTT in TEST_SETUP." );

    /* Set the parameters for MQTT connect. */
    networkInfo.createNetworkConnection = true;
    networkInfo.u.setup.pNetworkServerInfo = &serverInfo;
    networkInfo.u.setup.pNetworkCredentialInfo = &credentials;
    networkInfo.pNetworkInterface = IOT_TEST_NETWORK_INTERFACE;

    connectInfo.awsIotMqttMode = true;
    connectInfo.pClientIdentifier = clientcredentialIOT_THING_NAME;
    connectInfo.clientIdentifierLength = sizeof( clientcredentialIOT_THING_NAME ) - 1;

    /* Connect to the broker. */
    RETRY_EXPONENTIAL( connectStatus = IotMqtt_Connect( &networkInfo,
                                                        &connectInfo,
                                                        otatestAGENT_INIT_WAIT,
                                                        &xMQTTClientHandle ),
                       IOT_MQTT_SUCCESS,
                       otatestMQTT_RETRIES_START_MS,
                       otatestMQTT_RETRIES_MAX );

    TEST_ASSERT_EQUAL_INT_MESSAGE(
        IOT_MQTT_SUCCESS, connectStatus,
        "Failed to connect to the MQTT broker in TEST_SETUP." );

    xOTAConnContext.pvControlClient = xMQTTClientHandle;
}

/**
 * @brief Disconnect and delete the MQTT Agent after the OTA tests.
 */
TEST_TEAR_DOWN( Full_OTA_AGENT )
{
    /* Disconnect from the MQTT broker. */
    /* This must be protected against a Null xMQTTClientHandle if setup failed, for example due to an intermittent network connection */
    if( xMQTTClientHandle != NULL )
    {
        IotMqtt_Disconnect( xMQTTClientHandle, 0 );
    }

    xMQTTClientHandle = NULL;
    IotMqtt_Cleanup();

    IotSdk_Cleanup();
}

TEST_GROUP_RUNNER( Full_OTA_AGENT )
{
    RUN_TEST_CASE( Full_OTA_AGENT, OTA_SetImageState_AbortBeforeInit );
    RUN_TEST_CASE( Full_OTA_AGENT, OTA_SetImageState_AbortAfterShutDown );
    RUN_TEST_CASE( Full_OTA_AGENT, OTA_SetImageState_BadState );
    RUN_TEST_CASE( Full_OTA_AGENT, OTA_GetAgentState_InitAndStop );
    RUN_TEST_CASE( Full_OTA_AGENT, OTA_Shutdown_BeforeInit );
    RUN_TEST_CASE( Full_OTA_AGENT, OTA_AgentInit_BackToBack );
    RUN_TEST_CASE( Full_OTA_AGENT, OTA_CheckForUpdate_BeforeInit );
    RUN_TEST_CASE( Full_OTA_AGENT, OTA_GetStatistics_BeforeInit );
    RUN_TEST_CASE( Full_OTA_AGENT, prvParseJobDocFromJSONandPrvOTA_Close );
    RUN_TEST_CASE( Full_OTA_AGENT, prvParseJSONbyModel_Errors );
}

TEST( Full_OTA_AGENT, OTA_SetImageState_AbortBeforeInit )
{
    /* Attempt to set image state aborted without initializing the OTA Agent. */
    TEST_ASSERT_EQUAL_UINT32( kOTA_Err_Panic, OTA_SetImageState( eOTA_ImageState_Aborted ) );
}

TEST( Full_OTA_AGENT, OTA_SetImageState_AbortAfterShutDown )
{
    OTA_State_t eOtaStatus = eOTA_AgentState_Init;

    /* Initialize the Agent. */
    eOtaStatus = prvOTAAgentInit();
    TEST_ASSERT_EQUAL( eOTA_AgentState_WaitingForJob, eOtaStatus );

    /* Shutdown the OTA agent. */
    eOtaStatus = OTA_AgentShutdown( otatestSHUTDOWN_WAIT );
    TEST_ASSERT_EQUAL( eOTA_AgentState_Stopped, eOtaStatus );

    /* Attempt to set image state aborted. */
    TEST_ASSERT_EQUAL_UINT32( kOTA_Err_Panic, OTA_SetImageState( eOTA_ImageState_Aborted ) );
}

TEST( Full_OTA_AGENT, OTA_SetImageState_BadState )
{
    /* Initial OTA image state. */
    OTA_ImageState_t xOTAImageState = OTA_GetImageState();

    /* Attempt to set bad image states. */
    TEST_ASSERT_EQUAL_UINT32( kOTA_Err_BadImageState, OTA_SetImageState( eOTA_ImageState_Unknown ) );
    TEST_ASSERT_EQUAL_UINT32( kOTA_Err_BadImageState, OTA_SetImageState( ( OTA_ImageState_t ) -1 ) );

    /* Ensure that the failed SetImageState calls didn't modify the image state. */
    TEST_ASSERT_EQUAL( xOTAImageState, OTA_GetImageState() );
}

TEST( Full_OTA_AGENT, OTA_GetAgentState_InitAndStop )
{
    /* OTA Agent should be in stopped state when not initialized. */
    TEST_ASSERT_EQUAL( eOTA_AgentState_Stopped, OTA_GetAgentState() );

    /* Initialize the Agent. */
    TEST_ASSERT_EQUAL( eOTA_AgentState_WaitingForJob, prvOTAAgentInit() );

    /* Shutdown the OTA agent. */
    TEST_ASSERT_EQUAL( eOTA_AgentState_Stopped, OTA_AgentShutdown( otatestSHUTDOWN_WAIT ) );

    /* Calling shutdown again after the agent stops should do nothing. */
    TEST_ASSERT_EQUAL( eOTA_AgentState_Stopped, OTA_AgentShutdown( otatestSHUTDOWN_WAIT ) );
}

TEST( Full_OTA_AGENT, OTA_Shutdown_BeforeInit )
{
    /* Attempt to stop the agent before initializing. */
    TEST_ASSERT_EQUAL( eOTA_AgentState_Stopped, OTA_AgentShutdown( otatestSHUTDOWN_WAIT ) );
}

TEST( Full_OTA_AGENT, OTA_AgentInit_BackToBack )
{
    OTA_State_t eOtaStatus = eOTA_AgentState_Init;
    TickType_t xTicksToWait = pdMS_TO_TICKS( otatestAGENT_INIT_WAIT );

    /* Initialize the Agent. */
    TEST_ASSERT_EQUAL( eOTA_AgentState_WaitingForJob, prvOTAAgentInit() );

    /* Call init again should do nothing and only reset statistics. */
    eOtaStatus = OTA_AgentInit(
        &xOTAConnContext,
        ( const uint8_t * ) clientcredentialIOT_THING_NAME,
        NULL,
        xTicksToWait );
    TEST_ASSERT_EQUAL( eOTA_AgentState_WaitingForJob, eOtaStatus );

    /* Shutdown the OTA agent. */
    eOtaStatus = OTA_AgentShutdown( otatestSHUTDOWN_WAIT );
    TEST_ASSERT_EQUAL( eOTA_AgentState_Stopped, eOtaStatus );
}

TEST( Full_OTA_AGENT, OTA_CheckForUpdate_BeforeInit )
{
    /* Attempt to call OTA_CheckForUpdate without initializing the agent. */
    TEST_ASSERT_EQUAL( kOTA_Err_EventQueueSendFailed, OTA_CheckForUpdate() );
}

TEST( Full_OTA_AGENT, OTA_GetStatistics_BeforeInit )
{
    /* Attempt to get statistics without initializing the agent. */
    TEST_ASSERT_EQUAL( 0, OTA_GetPacketsDropped() );
    TEST_ASSERT_EQUAL( 0, OTA_GetPacketsQueued() );
    TEST_ASSERT_EQUAL( 0, OTA_GetPacketsProcessed() );
    TEST_ASSERT_EQUAL( 0, OTA_GetPacketsReceived() );
}

TEST( Full_OTA_AGENT, prvParseJobDocFromJSONandPrvOTA_Close )
{
    OTA_State_t eOtaStatus = eOTA_AgentState_Init;
    OTA_FileContext_t * pxUpdateFile = NULL;
    uint32_t ulLoopIndex = 0;
    bool_t bUpdateJob = false;

    eOtaStatus = prvOTAAgentInit();
    TEST_ASSERT_EQUAL( eOTA_AgentState_WaitingForJob, eOtaStatus );

    /* Test:
     * 1. That in ideal scenario, the JSON gets correctly processed.
     * 2. look for potential memory leak by alloc-dealloc several times.
     * Start test.
     */
    if( TEST_PROTECT() )
    {
        for( ulLoopIndex = 0; ulLoopIndex < otatestMAX_LOOP_MEM_LEAK_CHECK; ulLoopIndex++ )
        {
            pxUpdateFile = TEST_OTA_prvParseJobDoc( otatestLASER_JSON, sizeof( otatestLASER_JSON ), &bUpdateJob );
            TEST_ASSERT_TRUE( pxUpdateFile != NULL );

            /* Check the various document field conversions. */
            TEST_ASSERT_EQUAL_STRING( otatestSTREAM_NAME, pxUpdateFile->pucStreamName );
            TEST_ASSERT_EQUAL( otatestFILE_SIZE, pxUpdateFile->ulFileSize );
            TEST_ASSERT_EQUAL_STRING( otatestFILE_PATH, pxUpdateFile->pucFilePath );
            TEST_ASSERT_EQUAL_STRING( otatestCERT_FILE, pxUpdateFile->pucCertFilepath );
            TEST_ASSERT_EQUAL( otatestATTRIBUTES, pxUpdateFile->ulFileAttributes );
            TEST_ASSERT_EQUAL( otatestFILE_ID, pxUpdateFile->ulServerFileID );
            TEST_ASSERT_EQUAL( sizeof( ucOtatestSIGNATURE ), pxUpdateFile->pxSignature->usSize );
            TEST_ASSERT_EQUAL_MEMORY( ucOtatestSIGNATURE,
                                      pxUpdateFile->pxSignature->ucData,
                                      sizeof( ucOtatestSIGNATURE ) );

            TEST_OTA_prvOTA_Close( pxUpdateFile );
            pxUpdateFile = NULL;
        }
    }

    if( pxUpdateFile != NULL )
    {
        TEST_OTA_prvOTA_Close( pxUpdateFile );
        pxUpdateFile = NULL;
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
            pxUpdateFile = TEST_OTA_prvParseJobDoc( otatestBAD_LASER_JSON, sizeof( otatestBAD_LASER_JSON ), &bUpdateJob );
            TEST_ASSERT_TRUE( pxUpdateFile == NULL );
        }

        if( pxUpdateFile != NULL )
        {
            TEST_OTA_prvOTA_Close( pxUpdateFile );
            pxUpdateFile = NULL;
        }
    }

    /* End test. */

    /* Test that null is returned if JSON file with incorrect length is passed in parameter.
     * Start test.
     */
    pxUpdateFile = TEST_OTA_prvParseJobDoc( otatestLASER_JSON, sizeof( otatestLASER_JSON ) / 2, &bUpdateJob );
    TEST_ASSERT_TRUE( pxUpdateFile == NULL );
    /* End test. */

    /* Test that null is returned if corrupted JSON file is passed in parameter.
     * Start test.
     */
    pxUpdateFile = TEST_OTA_prvParseJobDoc( otatestGARBAGE_JSON, sizeof( otatestGARBAGE_JSON ), &bUpdateJob );
    TEST_ASSERT_TRUE( pxUpdateFile == NULL );
    /* End test. */

    /* Test that prvOTA_Close doesn't try to free already freed memory.
     * Start test.
     */
    pxUpdateFile = TEST_OTA_prvParseJobDoc( otatestLASER_JSON, sizeof( otatestLASER_JSON ), &bUpdateJob );

    if( pxUpdateFile != NULL )
    {
        vPortFree( pxUpdateFile->pucFilePath );
        pxUpdateFile->pucFilePath = NULL;
        vPortFree( pxUpdateFile->pucCertFilepath );
        pxUpdateFile->pucCertFilepath = NULL;
        vPortFree( pxUpdateFile->pxSignature );
        pxUpdateFile->pxSignature = NULL;
        vPortFree( pxUpdateFile->pucStreamName );
        pxUpdateFile->pucStreamName = NULL;
        TEST_OTA_prvOTA_Close( pxUpdateFile );
    }

    /* Shutdown the OTA agent. */
    eOtaStatus = OTA_AgentShutdown( otatestSHUTDOWN_WAIT );
    TEST_ASSERT_EQUAL( eOTA_AgentState_Stopped, eOtaStatus );
}

TEST( Full_OTA_AGENT, prvParseJSONbyModel_Errors )
{
    JSON_DocModel_t xDocModel = { 0 };
    JSON_DocParam_t xDocParam = { 0 };
    bool_t bUpdateJob = false;

    /* Initialize the OTA Agent for the following tests. */
    TEST_ASSERT_EQUAL( eOTA_AgentState_WaitingForJob, prvOTAAgentInit() );

    /* Ensure that NULL parameters are rejected. */
    TEST_ASSERT_EQUAL( eDocParseErr_NullModelPointer,
                       TEST_OTA_prvParseJSONbyModel( otatestLASER_JSON,
                                                     sizeof( otatestLASER_JSON ),
                                                     NULL ) );
    TEST_ASSERT_EQUAL( eDocParseErr_NullBodyPointer,
                       TEST_OTA_prvParseJSONbyModel( otatestLASER_JSON,
                                                     sizeof( otatestLASER_JSON ),
                                                     &xDocModel ) );
    /* Set pxBodyDef to a non-NULL value. */
    xDocModel.pxBodyDef = &xDocParam;
    TEST_ASSERT_EQUAL( eDocParseErr_NullDocPointer,
                       TEST_OTA_prvParseJSONbyModel( NULL,
                                                     sizeof( otatestLASER_JSON ),
                                                     &xDocModel ) );

    /* Ensure an invalid JSON document is rejected. */
    TEST_ASSERT_EQUAL( eDocParseErr_NoTokens, TEST_OTA_prvParseJSONbyModel( otatestLASER_JSON,
                                                                            0,
                                                                            &xDocModel ) );

    /* Ensure usNumModelParams is rejected if too large. */
    xDocModel.usNumModelParams = ( uint16_t ) ( 0xffffU );
    TEST_ASSERT_EQUAL( eDocParseErr_TooManyParams,
                       TEST_OTA_prvParseJSONbyModel( otatestLASER_JSON,
                                                     sizeof( otatestLASER_JSON ),
                                                     &xDocModel ) );

    /* Ensure that a JSON document containing duplicate keys is rejected. */
    TEST_ASSERT_EQUAL( NULL, TEST_OTA_prvParseJobDoc( otatestLASER_JSON_WITH_DUPLICATE,
                                                      sizeof( otatestLASER_JSON_WITH_DUPLICATE ),
                                                      &bUpdateJob ) );

    /* Ensure that a JSON document containing a mismatched field is rejected. */
    TEST_ASSERT_EQUAL( NULL, TEST_OTA_prvParseJobDoc( otatestLASER_JSON_WITH_FIELD_MISMATCH,
                                                      sizeof( otatestLASER_JSON_WITH_FIELD_MISMATCH ),
                                                      &bUpdateJob ) );

    /* The OTA Agent must be shut down if these tests fail, so a TEST_PROTECT is necessary. */
    if( TEST_PROTECT() )
    {
        /* Ensure that a JSON document containing bad base64 character is rejected. */
        TEST_ASSERT_EQUAL( NULL, TEST_OTA_prvParseJobDoc( otatestLASER_JSON_WITH_BAD_BASE64,
                                                          sizeof( otatestLASER_JSON_WITH_BAD_BASE64 ),
                                                          &bUpdateJob ) );
    }

    /* Shut down the OTA Agent. */
    ( void ) OTA_AgentShutdown( otatestSHUTDOWN_WAIT );
}
