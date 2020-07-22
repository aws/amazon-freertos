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
 * @file aws_iot_tests_shadow_system.c
 * @brief Full system tests for the AWS IoT Shadow library.
 */

/* The config header is always included first. */
#include "iot_config.h"

/* Standard includes. */
#include <stdio.h>
#include <string.h>

/* SDK initialization include. */
#include "iot_init.h"

/* Shadow internal include. */
#include "private/aws_iot_shadow_internal.h"

/* JSON utilities include. */
#include "iot_json_utils.h"

/* Platform layer includes. */
#include "platform/iot_threads.h"

/* Test network header include. */
#include IOT_TEST_NETWORK_HEADER

/* MQTT include. */
#include "iot_mqtt.h"

/* Test framework includes. */
#include "unity_fixture.h"
#include "aws_test_utils.h"

/* Require Shadow library asserts to be enabled for these tests. The Shadow
 * assert function is used to abort the tests on failure from the Shadow operation
 * complete callback. */
#if AWS_IOT_SHADOW_ENABLE_ASSERTS == 0
    #error "Shadow API unit tests require AWS_IOT_SHADOW_ENABLE_ASSERTS to be 1."
#endif

/*-----------------------------------------------------------*/

/**
 * @cond DOXYGEN_IGNORE
 * Doxygen should ignore this section.
 *
 * Provide default values of test configuration constants.
 */
#ifndef IOT_TEST_MQTT_SHORT_KEEPALIVE_INTERVAL_S
    #define IOT_TEST_MQTT_SHORT_KEEPALIVE_INTERVAL_S     ( 30 )
#endif
#ifndef IOT_TEST_MQTT_CONNECT_INIT_RETRY_DELAY_MS
    #define IOT_TEST_MQTT_CONNECT_INIT_RETRY_DELAY_MS    ( 1100 )
#endif
#ifndef IOT_TEST_MQTT_CONNECT_RETRY_COUNT
    #define IOT_TEST_MQTT_CONNECT_RETRY_COUNT            ( 6 )
#endif
#if IOT_TEST_MQTT_CONNECT_RETRY_COUNT < 1
    #error "IOT_TEST_MQTT_CONNECT_RETRY_COUNT must be at least 1."
#endif
#ifndef AWS_IOT_TEST_SHADOW_TIMEOUT
    #define AWS_IOT_TEST_SHADOW_TIMEOUT    ( 5000 )
#endif
/** @endcond */

/* Thing Name must be defined for these tests. */
#ifndef AWS_IOT_TEST_SHADOW_THING_NAME
    #error "Please define AWS_IOT_TEST_SHADOW_THING_NAME."
#endif

/**
 * @brief The length of @ref AWS_IOT_TEST_SHADOW_THING_NAME.
 */
#define THING_NAME_LENGTH              ( sizeof( AWS_IOT_TEST_SHADOW_THING_NAME ) - 1 )

/**
 * @brief The Shadow document used for these tests.
 */
#define TEST_SHADOW_DOCUMENT           "{\"state\":{\"reported\":{\"key\":\"value\"}},\"clientToken\":\"shadowtest\"}"

/**
 * @brief The length of #TEST_SHADOW_DOCUMENT.
 */
#define TEST_SHADOW_DOCUMENT_LENGTH    ( sizeof( TEST_SHADOW_DOCUMENT ) - 1 )

/*-----------------------------------------------------------*/

/**
 * @brief Parameter 1 of #_operationComplete.
 */
typedef struct _operationCompleteParams
{
    AwsIotShadowCallbackType_t expectedType; /**< @brief Expected callback type. */
    IotSemaphore_t waitSem;                  /**< @brief Used to unblock waiting test thread. */
    AwsIotShadowOperation_t operation;       /**< @brief Reference to expected completed operation. */
} _operationCompleteParams_t;

/*-----------------------------------------------------------*/

/**
 * @brief Network server info to share among the tests.
 */
static const IotTestNetworkServerInfo_t _serverInfo = IOT_TEST_NETWORK_SERVER_INFO_INITIALIZER;

/**
 * @brief Network credential info to share among the tests.
 */
#if IOT_TEST_SECURED_CONNECTION == 1
    static const IotTestNetworkCredentials_t _credentials = IOT_TEST_NETWORK_CREDENTIALS_INITIALIZER;
#endif

/**
 * @brief An MQTT network setup parameter to share among the tests.
 */
static IotMqttNetworkInfo_t _networkInfo = IOT_MQTT_NETWORK_INFO_INITIALIZER;

/**
 * @brief An MQTT connection to share among the tests.
 */
static IotMqttConnection_t _mqttConnection = IOT_MQTT_CONNECTION_INITIALIZER;

/*-----------------------------------------------------------*/

/**
 * @brief Shadow operation completion callback function. Checks parameters
 * and unblocks the main test thread.
 */
static void _operationComplete( void * pArgument,
                                AwsIotShadowCallbackParam_t * pOperation )
{
    _operationCompleteParams_t * pParams = ( _operationCompleteParams_t * ) pArgument;
    const char * pJsonValue = NULL;
    size_t jsonValueLength = 0;

    /* Check parameters against received operation information. */
    AwsIotShadow_Assert( pOperation->callbackType == pParams->expectedType );
    AwsIotShadow_Assert( pOperation->mqttConnection == _mqttConnection );
    AwsIotShadow_Assert( pOperation->u.operation.result == AWS_IOT_SHADOW_SUCCESS );
    AwsIotShadow_Assert( pOperation->u.operation.reference == pParams->operation );
    AwsIotShadow_Assert( pOperation->thingNameLength == THING_NAME_LENGTH );
    AwsIotShadow_Assert( strncmp( pOperation->pThingName,
                                  AWS_IOT_TEST_SHADOW_THING_NAME,
                                  THING_NAME_LENGTH ) == 0 );

    /* Check the retrieved Shadow document. */
    if( pOperation->callbackType == AWS_IOT_SHADOW_GET_COMPLETE )
    {
        AwsIotShadow_Assert( pOperation->u.operation.get.documentLength > 0 );

        AwsIotShadow_Assert( IotJsonUtils_FindJsonValue( pOperation->u.operation.get.pDocument,
                                                         pOperation->u.operation.get.documentLength,
                                                         "key",
                                                         3,
                                                         &pJsonValue,
                                                         &jsonValueLength ) == true );
        AwsIotShadow_Assert( jsonValueLength == 7 );
        AwsIotShadow_Assert( strncmp( pJsonValue, "\"value\"", 7 ) == 0 );
    }

    /* Unblock the main test thread. */
    IotSemaphore_Post( &( pParams->waitSem ) );
}

/*-----------------------------------------------------------*/

/**
 * @brief Shadow delta callback. Checks parameters and unblocks the main test
 * thread.
 */
static void _deltaCallback( void * pArgument,
                            AwsIotShadowCallbackParam_t * pCallback )
{
    IotSemaphore_t * pWaitSem = ( IotSemaphore_t * ) pArgument;
    const char * pValue = NULL, * pClientToken = NULL;
    size_t valueLength = 0, clientTokenLength = 0;

    /* Check callback type and MQTT connection. */
    AwsIotShadow_Assert( pCallback->callbackType == AWS_IOT_SHADOW_DELTA_CALLBACK );
    AwsIotShadow_Assert( pCallback->mqttConnection == _mqttConnection );

    /* Check delta document state. */
    AwsIotShadow_Assert( IotJsonUtils_FindJsonValue( pCallback->u.callback.pDocument,
                                                     pCallback->u.callback.documentLength,
                                                     "key",
                                                     3,
                                                     &pValue,
                                                     &valueLength ) == true );
    AwsIotShadow_Assert( valueLength == 4 );
    AwsIotShadow_Assert( strncmp( pValue, "true", valueLength ) == 0 );

    /* Check delta document client token. */
    AwsIotShadow_Assert( IotJsonUtils_FindJsonValue( pCallback->u.callback.pDocument,
                                                     pCallback->u.callback.documentLength,
                                                     "clientToken",
                                                     11,
                                                     &pClientToken,
                                                     &clientTokenLength ) );
    AwsIotShadow_Assert( clientTokenLength == 12 );
    AwsIotShadow_Assert( strncmp( "\"shadowtest\"", pClientToken, 12 ) == 0 );

    /* Unblock the main test thread. */
    IotSemaphore_Post( pWaitSem );
}

/*-----------------------------------------------------------*/

/**
 * @brief Shadow updated callback. Checks parameters and unblocks the main test
 * thread.
 */
static void _updatedCallback( void * pArgument,
                              AwsIotShadowCallbackParam_t * pCallback )
{
    IotSemaphore_t * pWaitSem = ( IotSemaphore_t * ) pArgument;
    const char * pPrevious = NULL, * pCurrent = NULL, * pClientToken = NULL;
    size_t previousStateLength = 0, currentStateLength = 0, clientTokenLength = 0;

    /* Check MQTT connection. */
    AwsIotShadow_Assert( pCallback->mqttConnection == _mqttConnection );

    /* Check updated document previous state. */
    AwsIotShadow_Assert( IotJsonUtils_FindJsonValue( pCallback->u.callback.pDocument,
                                                     pCallback->u.callback.documentLength,
                                                     "previous",
                                                     8,
                                                     &pPrevious,
                                                     &previousStateLength ) == true );
    AwsIotShadow_Assert( previousStateLength > 0 );
    AwsIotShadow_Assert( strncmp( "{\"state\":{},\"metadata\":{},",
                                  pPrevious,
                                  26 ) == 0 );

    /* Check updated document current state. */
    AwsIotShadow_Assert( IotJsonUtils_FindJsonValue( pCallback->u.callback.pDocument,
                                                     pCallback->u.callback.documentLength,
                                                     "current",
                                                     7,
                                                     &pCurrent,
                                                     &currentStateLength ) == true );
    AwsIotShadow_Assert( currentStateLength > 0 );
    AwsIotShadow_Assert( strncmp( "{\"state\":{\"desired\":{\"key\":true}}",
                                  pCurrent,
                                  33 ) == 0 );

    /* Check updated document client token. */
    AwsIotShadow_Assert( IotJsonUtils_FindJsonValue( pCallback->u.callback.pDocument,
                                                     pCallback->u.callback.documentLength,
                                                     "clientToken",
                                                     11,
                                                     &pClientToken,
                                                     &clientTokenLength ) );
    AwsIotShadow_Assert( clientTokenLength == 12 );
    AwsIotShadow_Assert( strncmp( "\"shadowtest\"", pClientToken, 12 ) == 0 );

    /* Unblock the main test thread. */
    IotSemaphore_Post( pWaitSem );
}

/*-----------------------------------------------------------*/

/**
 * @brief Run the Update-Get-Delete asynchronous tests at various QoS.
 */
static void _updateGetDeleteAsync( IotMqttQos_t qos )
{
    AwsIotShadowError_t status = AWS_IOT_SHADOW_STATUS_PENDING;
    AwsIotShadowCallbackInfo_t callbackInfo = AWS_IOT_SHADOW_CALLBACK_INFO_INITIALIZER;
    AwsIotShadowDocumentInfo_t documentInfo = AWS_IOT_SHADOW_DOCUMENT_INFO_INITIALIZER;
    _operationCompleteParams_t callbackParam = { .expectedType = ( AwsIotShadowCallbackType_t ) 0 };

    /* Initialize the members of the operation callback info. */
    callbackInfo.pCallbackContext = &callbackParam;
    callbackInfo.function = _operationComplete;

    /* Initialize the common members of the Shadow document info. */
    documentInfo.pThingName = AWS_IOT_TEST_SHADOW_THING_NAME;
    documentInfo.thingNameLength = THING_NAME_LENGTH;
    documentInfo.qos = qos;

    /* Create the wait semaphore for operations. */
    TEST_ASSERT_EQUAL_INT( true, IotSemaphore_Create( &( callbackParam.waitSem ), 0, 1 ) );

    if( TEST_PROTECT() )
    {
        /* Set the expected callback type to UPDATE. */
        callbackParam.expectedType = AWS_IOT_SHADOW_UPDATE_COMPLETE;

        /* Set the members of the Shadow document info for UPDATE. */
        documentInfo.u.update.pUpdateDocument = TEST_SHADOW_DOCUMENT;
        documentInfo.u.update.updateDocumentLength = TEST_SHADOW_DOCUMENT_LENGTH;

        /* Create a new Shadow document. */
        status = AwsIotShadow_Update( _mqttConnection,
                                      &documentInfo,
                                      0,
                                      &callbackInfo,
                                      &( callbackParam.operation ) );

        if( IotSemaphore_TimedWait( &( callbackParam.waitSem ),
                                    AWS_IOT_TEST_SHADOW_TIMEOUT ) == false )
        {
            TEST_FAIL_MESSAGE( "Timed out waiting to update Shadow document." );
        }

        /* Set the expected callback type to GET. */
        callbackParam.expectedType = AWS_IOT_SHADOW_GET_COMPLETE;

        /* Retrieve the Shadow document. */
        status = AwsIotShadow_Get( _mqttConnection,
                                   &documentInfo,
                                   0,
                                   &callbackInfo,
                                   &( callbackParam.operation ) );
        TEST_ASSERT_EQUAL_INT( AWS_IOT_SHADOW_STATUS_PENDING, status );

        if( IotSemaphore_TimedWait( &( callbackParam.waitSem ),
                                    AWS_IOT_TEST_SHADOW_TIMEOUT ) == false )
        {
            TEST_FAIL_MESSAGE( "Timed out waiting to retrieve Shadow document." );
        }

        /* Set the expected callback type to DELETE. */
        callbackParam.expectedType = AWS_IOT_SHADOW_DELETE_COMPLETE;

        /* Delete the Shadow document. */
        status = AwsIotShadow_Delete( _mqttConnection,
                                      AWS_IOT_TEST_SHADOW_THING_NAME,
                                      THING_NAME_LENGTH,
                                      0,
                                      &callbackInfo,
                                      &( callbackParam.operation ) );

        if( IotSemaphore_TimedWait( &( callbackParam.waitSem ),
                                    AWS_IOT_TEST_SHADOW_TIMEOUT ) == false )
        {
            TEST_FAIL_MESSAGE( "Timed out waiting to delete Shadow document." );
        }
    }

    IotSemaphore_Destroy( &( callbackParam.waitSem ) );
}

/*-----------------------------------------------------------*/

/**
 * @brief Run the Update-Get-Delete blocking tests at various QoS.
 */
static void _updateGetDeleteBlocking( IotMqttQos_t qos )
{
    AwsIotShadowError_t status = AWS_IOT_SHADOW_STATUS_PENDING;
    AwsIotShadowDocumentInfo_t documentInfo = AWS_IOT_SHADOW_DOCUMENT_INFO_INITIALIZER;
    const char * pShadowDocument = NULL, * pJsonValue = NULL;
    size_t shadowDocumentLength = 0, jsonValueLength = 0;

    /* Initialize the common members of the Shadow document info. */
    documentInfo.pThingName = AWS_IOT_TEST_SHADOW_THING_NAME;
    documentInfo.thingNameLength = THING_NAME_LENGTH;
    documentInfo.qos = qos;

    /* Set the members of the Shadow document info for UPDATE. */
    documentInfo.u.update.pUpdateDocument = TEST_SHADOW_DOCUMENT;
    documentInfo.u.update.updateDocumentLength = TEST_SHADOW_DOCUMENT_LENGTH;

    /* Create a new Shadow document. */
    status = AwsIotShadow_TimedUpdate( _mqttConnection,
                                       &documentInfo,
                                       0,
                                       AWS_IOT_TEST_SHADOW_TIMEOUT );
    TEST_ASSERT_EQUAL( AWS_IOT_SHADOW_SUCCESS, status );

    /* Set the members of the Shadow document info for GET. */
    documentInfo.u.get.mallocDocument = IotTest_Malloc;

    /* Retrieve the Shadow document. */
    status = AwsIotShadow_TimedGet( _mqttConnection,
                                    &documentInfo,
                                    0,
                                    AWS_IOT_TEST_SHADOW_TIMEOUT,
                                    &pShadowDocument,
                                    &shadowDocumentLength );
    TEST_ASSERT_EQUAL_INT( AWS_IOT_SHADOW_SUCCESS, status );

    /* Check the retrieved Shadow document. */
    TEST_ASSERT_GREATER_THAN( 0, shadowDocumentLength );
    TEST_ASSERT_NOT_NULL( pShadowDocument );
    TEST_ASSERT_EQUAL_INT( true, IotJsonUtils_FindJsonValue( pShadowDocument,
                                                             shadowDocumentLength,
                                                             "key",
                                                             3,
                                                             &pJsonValue,
                                                             &jsonValueLength ) );
    TEST_ASSERT_EQUAL( 7, jsonValueLength );
    TEST_ASSERT_EQUAL_STRING_LEN( "\"value\"", pJsonValue, jsonValueLength );

    /* Free the retrieved Shadow document. */
    IotTest_Free( ( void * ) pShadowDocument );

    /* Delete the Shadow document. */
    status = AwsIotShadow_TimedDelete( _mqttConnection,
                                       AWS_IOT_TEST_SHADOW_THING_NAME,
                                       THING_NAME_LENGTH,
                                       0,
                                       AWS_IOT_TEST_SHADOW_TIMEOUT );
    TEST_ASSERT_EQUAL( AWS_IOT_SHADOW_SUCCESS, status );
}

/*-----------------------------------------------------------*/

/**
 * @brief Establish an MQTT connection. Retry if enabled.
 */
static IotMqttError_t _mqttConnect( const IotMqttNetworkInfo_t * pNetworkInfo,
                                    const IotMqttConnectInfo_t * pConnectInfo,
                                    uint32_t timeoutMs,
                                    IotMqttConnection_t * const pMqttConnection )
{
    IotMqttError_t status = IOT_MQTT_STATUS_PENDING;

    RETRY_EXPONENTIAL( status = IotMqtt_Connect( pNetworkInfo, pConnectInfo, timeoutMs, pMqttConnection ),
                       IOT_MQTT_SUCCESS,
                       IOT_TEST_MQTT_CONNECT_INIT_RETRY_DELAY_MS,
                       IOT_TEST_MQTT_CONNECT_RETRY_COUNT );

    return status;
}

/**
 * @brief Test group for Shadow system tests.
 */
TEST_GROUP( Shadow_System );

/*-----------------------------------------------------------*/

/**
 * @brief Test setup for Shadow system tests.
 */
TEST_SETUP( Shadow_System )
{
    IotMqttConnectInfo_t connectInfo = IOT_MQTT_CONNECT_INFO_INITIALIZER;
    AwsIotShadowError_t status = AWS_IOT_SHADOW_STATUS_PENDING;

    /* Initialize SDK. */
    if( IotSdk_Init() == false )
    {
        TEST_FAIL_MESSAGE( "Failed to initialize SDK." );
    }

    /* Set up the network stack. */
    if( IotTestNetwork_Init() != IOT_NETWORK_SUCCESS )
    {
        TEST_FAIL_MESSAGE( "Failed to set up network stack." );
    }

    /* Initialize the MQTT library. */
    if( IotMqtt_Init() != IOT_MQTT_SUCCESS )
    {
        TEST_FAIL_MESSAGE( "Failed to initialize MQTT library." );
    }

    /* Initialize the Shadow library. */
    if( AwsIotShadow_Init( 0 ) != AWS_IOT_SHADOW_SUCCESS )
    {
        TEST_FAIL_MESSAGE( "Failed to initialize Shadow library." );
    }

    /* Set the MQTT network setup parameters. */
    ( void ) memset( &_networkInfo, 0x00, sizeof( IotMqttNetworkInfo_t ) );
    _networkInfo.createNetworkConnection = true;
    _networkInfo.u.setup.pNetworkServerInfo = ( void * ) &_serverInfo;
    _networkInfo.pNetworkInterface = IOT_TEST_NETWORK_INTERFACE;

    #if IOT_TEST_SECURED_CONNECTION == 1
        _networkInfo.u.setup.pNetworkCredentialInfo = ( void * ) &_credentials;
    #endif

    #ifdef IOT_TEST_MQTT_SERIALIZER
        _networkInfo.pMqttSerializer = IOT_TEST_MQTT_SERIALIZER;
    #endif

    /* Set the members of the connect info. Use the Shadow Thing Name as the MQTT
     * client identifier. */
    connectInfo.awsIotMqttMode = true;
    connectInfo.pClientIdentifier = AWS_IOT_TEST_SHADOW_THING_NAME;
    connectInfo.clientIdentifierLength = ( uint16_t ) ( sizeof( AWS_IOT_TEST_SHADOW_THING_NAME ) - 1 );
    connectInfo.keepAliveSeconds = IOT_TEST_MQTT_SHORT_KEEPALIVE_INTERVAL_S;

    /* Establish an MQTT connection. */
    if( _mqttConnect( &_networkInfo,
                      &connectInfo,
                      AWS_IOT_TEST_SHADOW_TIMEOUT,
                      &_mqttConnection ) != IOT_MQTT_SUCCESS )
    {
        TEST_FAIL_MESSAGE( "Failed to establish MQTT connection for Shadow tests." );
    }

    /* Delete any existing Shadow so all tests start with no Shadow. */
    status = AwsIotShadow_TimedDelete( _mqttConnection,
                                       AWS_IOT_TEST_SHADOW_THING_NAME,
                                       THING_NAME_LENGTH,
                                       0,
                                       AWS_IOT_TEST_SHADOW_TIMEOUT );

    /* Acceptable statuses are SUCCESS and NOT FOUND. Both of these statuses allow
     * the tests to start with no Shadow. */
    if( ( status != AWS_IOT_SHADOW_SUCCESS ) && ( status != AWS_IOT_SHADOW_NOT_FOUND ) )
    {
        TEST_FAIL_MESSAGE( "Failed to delete shadow in test set up." );
    }
}

/*-----------------------------------------------------------*/

/**
 * @brief Test tear down for Shadow system tests.
 */
TEST_TEAR_DOWN( Shadow_System )
{
    /* Disconnect the MQTT connection if it was created. */
    if( _mqttConnection != IOT_MQTT_CONNECTION_INITIALIZER )
    {
        IotMqtt_Disconnect( _mqttConnection, 0 );
        _mqttConnection = IOT_MQTT_CONNECTION_INITIALIZER;
    }

    /* Clean up the Shadow library. */
    AwsIotShadow_Cleanup();

    /* Clean up the MQTT library. */
    IotMqtt_Cleanup();

    /* Clean up the network stack. */
    IotTestNetwork_Cleanup();

    /* Clean up SDK. */
    IotSdk_Cleanup();
}

/*-----------------------------------------------------------*/

/**
 * @brief Test group runner for Shadow system tests.
 */
TEST_GROUP_RUNNER( Shadow_System )
{
    RUN_TEST_CASE( Shadow_System, UpdateGetDeleteAsyncQoS0 );
    RUN_TEST_CASE( Shadow_System, UpdateGetDeleteAsyncQoS1 );
    RUN_TEST_CASE( Shadow_System, UpdateGetDeleteBlockingQoS0 );
    RUN_TEST_CASE( Shadow_System, UpdateGetDeleteBlockingQoS1 );
    RUN_TEST_CASE( Shadow_System, DeltaCallback );
    RUN_TEST_CASE( Shadow_System, UpdatedCallback );
}

/*-----------------------------------------------------------*/

/**
 * @brief Update-Get-Delete asynchronous (QoS 0).
 */
TEST( Shadow_System, UpdateGetDeleteAsyncQoS0 )
{
    _updateGetDeleteAsync( IOT_MQTT_QOS_0 );
}

/*-----------------------------------------------------------*/

/**
 * @brief Update-Get-Delete asynchronous (QoS 1).
 */
TEST( Shadow_System, UpdateGetDeleteAsyncQoS1 )
{
    _updateGetDeleteAsync( IOT_MQTT_QOS_1 );
}

/*-----------------------------------------------------------*/

/**
 * @brief Update-Get-Delete blocking (QoS 0).
 */
TEST( Shadow_System, UpdateGetDeleteBlockingQoS0 )
{
    _updateGetDeleteBlocking( IOT_MQTT_QOS_0 );
}

/*-----------------------------------------------------------*/

/**
 * @brief Update-Get-Delete blocking (QoS 1).
 */
TEST( Shadow_System, UpdateGetDeleteBlockingQoS1 )
{
    _updateGetDeleteBlocking( IOT_MQTT_QOS_1 );
}

/*-----------------------------------------------------------*/

/**
 * @brief Tests the Shadow delta callback.
 */
TEST( Shadow_System, DeltaCallback )
{
    AwsIotShadowError_t status = AWS_IOT_SHADOW_STATUS_PENDING;
    AwsIotShadowCallbackInfo_t deltaCallback = AWS_IOT_SHADOW_CALLBACK_INFO_INITIALIZER;
    AwsIotShadowDocumentInfo_t updateDocument = AWS_IOT_SHADOW_DOCUMENT_INFO_INITIALIZER;
    IotSemaphore_t waitSem;

    /* Create a semaphore to wait on. */
    TEST_ASSERT_EQUAL_INT( true, IotSemaphore_Create( &waitSem, 0, 2 ) );

    /* Set the delta callback information. */
    deltaCallback.pCallbackContext = &waitSem;
    deltaCallback.function = _deltaCallback;

    /* Set a desired state in the Update document. */
    updateDocument.pThingName = AWS_IOT_TEST_SHADOW_THING_NAME;
    updateDocument.thingNameLength = THING_NAME_LENGTH;
    updateDocument.u.update.pUpdateDocument = "{\"state\": {\"desired\": {\"key\": true}}, \"clientToken\":\"shadowtest\"}";
    updateDocument.u.update.updateDocumentLength = 65;

    if( TEST_PROTECT() )
    {
        /* Set the delta callback. */
        status = AwsIotShadow_SetDeltaCallback( _mqttConnection,
                                                AWS_IOT_TEST_SHADOW_THING_NAME,
                                                THING_NAME_LENGTH,
                                                0,
                                                &deltaCallback );
        TEST_ASSERT_EQUAL( AWS_IOT_SHADOW_SUCCESS, status );

        /* Create a Shadow document with a desired state. */
        status = AwsIotShadow_TimedUpdate( _mqttConnection,
                                           &updateDocument,
                                           AWS_IOT_SHADOW_FLAG_KEEP_SUBSCRIPTIONS,
                                           AWS_IOT_TEST_SHADOW_TIMEOUT );
        TEST_ASSERT_EQUAL( AWS_IOT_SHADOW_SUCCESS, status );

        /* Set a different reported state in the Update document. */
        updateDocument.u.update.pUpdateDocument = "{\"state\": {\"reported\": {\"key\": false}}, \"clientToken\":\"shadowtest\"}";
        updateDocument.u.update.updateDocumentLength = 67;

        /* Create a Shadow document with a reported state. */
        status = AwsIotShadow_TimedUpdate( _mqttConnection,
                                           &updateDocument,
                                           0,
                                           AWS_IOT_TEST_SHADOW_TIMEOUT );
        TEST_ASSERT_EQUAL( AWS_IOT_SHADOW_SUCCESS, status );

        /* Block on the wait semaphore until the delta callback is invoked. */
        if( IotSemaphore_TimedWait( &waitSem, AWS_IOT_TEST_SHADOW_TIMEOUT ) == false )
        {
            TEST_FAIL_MESSAGE( "Timed out waiting for delta callback." );
        }

        /* Remove the delta callback. */
        status = AwsIotShadow_SetDeltaCallback( _mqttConnection,
                                                AWS_IOT_TEST_SHADOW_THING_NAME,
                                                THING_NAME_LENGTH,
                                                0,
                                                NULL );
        TEST_ASSERT_EQUAL( AWS_IOT_SHADOW_SUCCESS, status );

        /* Remove persistent subscriptions for Shadow Update. */
        status = AwsIotShadow_RemovePersistentSubscriptions( _mqttConnection,
                                                             AWS_IOT_TEST_SHADOW_THING_NAME,
                                                             THING_NAME_LENGTH,
                                                             AWS_IOT_SHADOW_FLAG_REMOVE_UPDATE_SUBSCRIPTIONS );
        TEST_ASSERT_EQUAL( AWS_IOT_SHADOW_SUCCESS, status );
    }

    IotSemaphore_Destroy( &waitSem );
}

/*-----------------------------------------------------------*/

/**
 * @brief Tests the Shadow updated callback.
 */
TEST( Shadow_System, UpdatedCallback )
{
    AwsIotShadowError_t status = AWS_IOT_SHADOW_STATUS_PENDING;
    AwsIotShadowCallbackInfo_t updatedCallback = AWS_IOT_SHADOW_CALLBACK_INFO_INITIALIZER;
    AwsIotShadowDocumentInfo_t updateDocument = AWS_IOT_SHADOW_DOCUMENT_INFO_INITIALIZER;
    IotSemaphore_t waitSem;

    /* Create a semaphore to wait on. */
    TEST_ASSERT_EQUAL_INT( true, IotSemaphore_Create( &waitSem, 0, 1 ) );

    /* Set the delta callback information. */
    updatedCallback.pCallbackContext = &waitSem;
    updatedCallback.function = _updatedCallback;

    /* Set a desired state in the Update document. */
    updateDocument.pThingName = AWS_IOT_TEST_SHADOW_THING_NAME;
    updateDocument.thingNameLength = THING_NAME_LENGTH;
    updateDocument.u.update.pUpdateDocument = "{\"state\": {\"desired\": {\"key\": true}}, \"clientToken\":\"shadowtest\"}";
    updateDocument.u.update.updateDocumentLength = 65;

    if( TEST_PROTECT() )
    {
        /* Set the updated callback. */
        status = AwsIotShadow_SetUpdatedCallback( _mqttConnection,
                                                  AWS_IOT_TEST_SHADOW_THING_NAME,
                                                  THING_NAME_LENGTH,
                                                  0,
                                                  &updatedCallback );
        TEST_ASSERT_EQUAL( AWS_IOT_SHADOW_SUCCESS, status );

        /* Create a Shadow document. */
        status = AwsIotShadow_TimedUpdate( _mqttConnection,
                                           &updateDocument,
                                           0,
                                           AWS_IOT_TEST_SHADOW_TIMEOUT );
        TEST_ASSERT_EQUAL( AWS_IOT_SHADOW_SUCCESS, status );

        /* Block on the wait semaphore until the updated callback is invoked. */
        if( IotSemaphore_TimedWait( &waitSem, AWS_IOT_TEST_SHADOW_TIMEOUT ) == false )
        {
            TEST_FAIL_MESSAGE( "Timed out waiting for updated callback." );
        }

        /* Remove the updated callback. */
        status = AwsIotShadow_SetUpdatedCallback( _mqttConnection,
                                                  AWS_IOT_TEST_SHADOW_THING_NAME,
                                                  THING_NAME_LENGTH,
                                                  0,
                                                  NULL );
        TEST_ASSERT_EQUAL( AWS_IOT_SHADOW_SUCCESS, status );
    }

    IotSemaphore_Destroy( &waitSem );
}

/*-----------------------------------------------------------*/
