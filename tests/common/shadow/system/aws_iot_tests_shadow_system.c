/*
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
 */

/**
 * @file aws_iot_tests_shadow_system.c
 * @brief Full system tests for the AWS IoT Shadow library.
 */

/* Build using a config header, if provided. */
#ifdef AWS_IOT_CONFIG_FILE
    #include AWS_IOT_CONFIG_FILE
#endif

/* Standard includes. */
#include <string.h>

/* Shadow internal include. */
#include "private/aws_iot_shadow_internal.h"

/* JSON utilities include. */
#include "aws_iot_json_utils.h"

/* Platform layer includes. */
#include "platform/aws_iot_clock.h"

/* Test framework includes. */
#include "unity_fixture.h"

/* Require Shadow library asserts to be enabled for these tests. The Shadow
 * assert function is used to abort the tests on failure from the Shadow operation
 * complete callback. */
#if AWS_IOT_SHADOW_ENABLE_ASSERTS == 0
    #error "Shadow API unit tests require AWS_IOT_SHADOW_ENABLE_ASSERTS to be 1."
#endif

/**
 * @cond DOXYGEN_IGNORE
 * Doxygen should ignore this section.
 *
 * Including stdio.h also brings in unwanted (and conflicting) symbols on some
 * platforms. Therefore, any functions in stdio.h needed in this file have an
 * extern declaration here. */
extern int snprintf( char *,
                     size_t,
                     const char *,
                     ... );
/** @endcond */

/*-----------------------------------------------------------*/

/**
 * @cond DOXYGEN_IGNORE
 * Doxygen should ignore this section.
 *
 * Provide default values of test configuration constants.
 */
#ifndef AWS_IOT_TEST_MQTT_TOPIC_PREFIX
    #define AWS_IOT_TEST_MQTT_TOPIC_PREFIX                  "awsiotmqtttest"
#endif
#ifndef AWS_IOT_TEST_MQTT_SHORT_KEEPALIVE_INTERVAL_S
    #define AWS_IOT_TEST_MQTT_SHORT_KEEPALIVE_INTERVAL_S    ( 30 )
#endif
#ifndef AWS_IOT_TEST_SHADOW_TIMEOUT
    #define AWS_IOT_TEST_SHADOW_TIMEOUT                     ( 5000 )
#endif
/** @endcond */

/* Thing Name must be defined for these tests. */
#ifndef AWS_IOT_TEST_SHADOW_THING_NAME
    #error "Please define AWS_IOT_TEST_SHADOW_THING_NAME."
#endif

/**
 * @brief The maximum length of an MQTT client identifier.
 */
#define _CLIENT_IDENTIFIER_MAX_LENGTH    ( 23 )

/**
 * @brief The length of @ref AWS_IOT_TEST_SHADOW_THING_NAME.
 */
#define _THING_NAME_LENGTH               ( sizeof( AWS_IOT_TEST_SHADOW_THING_NAME ) - 1 )

/**
 * @brief The Shadow document used for these tests.
 */
#define _TEST_SHADOW_DOCUMENT            "{\"state\":{\"reported\":{\"key\":\"value\"}},\"clientToken\":\"shadowtest\"}"

/**
 * @brief The length of #_TEST_SHADOW_DOCUMENT.
 */
#define _TEST_SHADOW_DOCUMENT_LENGTH     ( sizeof( _TEST_SHADOW_DOCUMENT ) - 1 )

/*-----------------------------------------------------------*/

/**
 * @brief Parameter 1 of #_operationComplete.
 */
typedef struct _operationCompleteParams
{
    AwsIotShadowCallbackType_t expectedType; /**< @brief Expected callback type. */
    AwsIotSemaphore_t waitSem;               /**< @brief Used to unblock waiting test thread. */
    AwsIotShadowReference_t reference;       /**< @brief Reference to expected completed operation. */
} _operationCompleteParams_t;

/*-----------------------------------------------------------*/

/* Network functions used by the tests, declared and implemented in one of
 * the test network function files. */
extern bool AwsIotTest_NetworkSetup( void );
extern void AwsIotTest_NetworkCleanup( void );
extern bool AwsIotTest_NetworkConnect( void ** const pNewConnection,
                                       AwsIotMqttConnection_t * pMqttConnection );
extern void AwsIotTest_NetworkClose( void * pDisconnectContext );
extern void AwsIotTest_NetworkDestroy( void * pConnection );

/* Network variables used by the tests, declared in one of the test network
 * function files. */
extern AwsIotMqttNetIf_t _AwsIotTestNetworkInterface;
extern AwsIotMqttConnection_t _AwsIotTestMqttConnection;

/*-----------------------------------------------------------*/

/**
 * @brief Buffer holding the MQTT client identifier used for the tests.
 */
static char _pClientIdentifier[ _CLIENT_IDENTIFIER_MAX_LENGTH ] = { 0 };

/*-----------------------------------------------------------*/

/**
 * @brief Shadow operation completion callback function. Checks parameters
 * and unblocks the main test thread.
 */
static void _operationComplete( void * pArgument,
                                AwsIotShadowCallbackParam_t * const pOperation )
{
    _operationCompleteParams_t * pParams = ( _operationCompleteParams_t * ) pArgument;
    const char * pJsonValue = NULL;
    size_t jsonValueLength = 0;

    /* Check parameters against received operation information. */
    AwsIotShadow_Assert( pOperation->callbackType == pParams->expectedType );
    AwsIotShadow_Assert( pOperation->operation.result == AWS_IOT_SHADOW_SUCCESS );
    AwsIotShadow_Assert( pOperation->operation.reference == pParams->reference );
    AwsIotShadow_Assert( pOperation->thingNameLength == _THING_NAME_LENGTH );
    AwsIotShadow_Assert( strncmp( pOperation->pThingName,
                                  AWS_IOT_TEST_SHADOW_THING_NAME,
                                  _THING_NAME_LENGTH ) == 0 );

    /* Check the retrieved Shadow document. */
    if( pOperation->callbackType == AWS_IOT_SHADOW_GET_COMPLETE )
    {
        AwsIotShadow_Assert( pOperation->operation.get.documentLength > 0 );

        AwsIotShadow_Assert( AwsIotJsonUtils_FindJsonValue( pOperation->operation.get.pDocument,
                                                            pOperation->operation.get.documentLength,
                                                            "key",
                                                            3,
                                                            &pJsonValue,
                                                            &jsonValueLength ) == true );
        AwsIotShadow_Assert( jsonValueLength == 7 );
        AwsIotShadow_Assert( strncmp( pJsonValue, "\"value\"", 7 ) == 0 );
    }

    /* Unblock the main test thread. */
    AwsIotSemaphore_Post( &( pParams->waitSem ) );
}

/*-----------------------------------------------------------*/

/**
 * @brief Shadow delta callback. Checks parameters and unblocks the main test
 * thread.
 */
static void _deltaCallback( void * pArgument,
                            AwsIotShadowCallbackParam_t * const pCallback )
{
    AwsIotSemaphore_t * pWaitSem = ( AwsIotSemaphore_t * ) pArgument;
    const char * pValue = NULL, * pClientToken = NULL;
    size_t valueLength = 0, clientTokenLength = 0;

    /* Check callback type. */
    AwsIotShadow_Assert( pCallback->callbackType == AWS_IOT_SHADOW_DELTA_CALLBACK );

    /* Check delta document state. */
    AwsIotShadow_Assert( AwsIotJsonUtils_FindJsonValue( pCallback->callback.pDocument,
                                                        pCallback->callback.documentLength,
                                                        "key",
                                                        3,
                                                        &pValue,
                                                        &valueLength ) == true );
    AwsIotShadow_Assert( valueLength == 4 );
    AwsIotShadow_Assert( strncmp( pValue, "true", valueLength ) == 0 );

    /* Check delta document client token. */
    AwsIotShadow_Assert( AwsIotJsonUtils_FindJsonValue( pCallback->callback.pDocument,
                                                        pCallback->callback.documentLength,
                                                        "clientToken",
                                                        11,
                                                        &pClientToken,
                                                        &clientTokenLength ) );
    AwsIotShadow_Assert( clientTokenLength == 12 );
    AwsIotShadow_Assert( strncmp( "\"shadowtest\"", pClientToken, 12 ) == 0 );

    /* Unblock the main test thread. */
    AwsIotSemaphore_Post( pWaitSem );
}

/*-----------------------------------------------------------*/

/**
 * @brief Shadow updated callback. Checks parameters and unblocks the main test
 * thread.
 */
static void _updatedCallback( void * pArgument,
                              AwsIotShadowCallbackParam_t * const pCallback )
{
    AwsIotSemaphore_t * pWaitSem = ( AwsIotSemaphore_t * ) pArgument;
    const char * pPrevious = NULL, * pCurrent = NULL, * pClientToken = NULL;
    size_t previousStateLength = 0, currentStateLength = 0, clientTokenLength = 0;

    /* Check updated document previous state. */
    AwsIotShadow_Assert( AwsIotJsonUtils_FindJsonValue( pCallback->callback.pDocument,
                                                        pCallback->callback.documentLength,
                                                        "previous",
                                                        8,
                                                        &pPrevious,
                                                        &previousStateLength ) == true );
    AwsIotShadow_Assert( previousStateLength > 0 );
    AwsIotShadow_Assert( strncmp( "{\"state\":{},\"metadata\":{},",
                                  pPrevious,
                                  26 ) == 0 );

    /* Check updated document current state. */
    AwsIotShadow_Assert( AwsIotJsonUtils_FindJsonValue( pCallback->callback.pDocument,
                                                        pCallback->callback.documentLength,
                                                        "current",
                                                        7,
                                                        &pCurrent,
                                                        &currentStateLength ) == true );
    AwsIotShadow_Assert( currentStateLength > 0 );
    AwsIotShadow_Assert( strncmp( "{\"state\":{\"desired\":{\"key\":true}}",
                                  pCurrent,
                                  33 ) == 0 );

    /* Check updated document client token. */
    AwsIotShadow_Assert( AwsIotJsonUtils_FindJsonValue( pCallback->callback.pDocument,
                                                        pCallback->callback.documentLength,
                                                        "clientToken",
                                                        11,
                                                        &pClientToken,
                                                        &clientTokenLength ) );
    AwsIotShadow_Assert( clientTokenLength == 12 );
    AwsIotShadow_Assert( strncmp( "\"shadowtest\"", pClientToken, 12 ) == 0 );

    /* Unblock the main test thread. */
    AwsIotSemaphore_Post( pWaitSem );
}

/*-----------------------------------------------------------*/

/**
 * @brief Run the Update-Get-Delete asynchronous tests at various QoS.
 */
static void _updateGetDeleteAsync( int QoS )
{
    AwsIotShadowError_t status = AWS_IOT_SHADOW_STATUS_PENDING;
    AwsIotShadowCallbackInfo_t callbackInfo = AWS_IOT_SHADOW_CALLBACK_INFO_INITIALIZER;
    AwsIotShadowDocumentInfo_t documentInfo = AWS_IOT_SHADOW_DOCUMENT_INFO_INITIALIZER;
    _operationCompleteParams_t callbackParam = { 0 };

    /* Initialize the members of the operation callback info. */
    callbackInfo.param1 = &callbackParam;
    callbackInfo.function = _operationComplete;

    /* Initialize the common members of the Shadow document info. */
    documentInfo.pThingName = AWS_IOT_TEST_SHADOW_THING_NAME;
    documentInfo.thingNameLength = _THING_NAME_LENGTH;
    documentInfo.QoS = QoS;

    /* Create the wait semaphore for operations. */
    TEST_ASSERT_EQUAL_INT( true, AwsIotSemaphore_Create( &( callbackParam.waitSem ), 0, 1 ) );

    if( TEST_PROTECT() )
    {
        /* Set the expected callback type to UPDATE. */
        callbackParam.expectedType = AWS_IOT_SHADOW_UPDATE_COMPLETE;

        /* Set the members of the Shadow document info for UPDATE. */
        documentInfo.update.pUpdateDocument = _TEST_SHADOW_DOCUMENT;
        documentInfo.update.updateDocumentLength = _TEST_SHADOW_DOCUMENT_LENGTH;

        /* Create a new Shadow document. */
        status = AwsIotShadow_Update( _AwsIotTestMqttConnection,
                                      &documentInfo,
                                      0,
                                      &callbackInfo,
                                      &( callbackParam.reference ) );

        if( AwsIotSemaphore_TimedWait( &( callbackParam.waitSem ),
                                       AWS_IOT_TEST_SHADOW_TIMEOUT ) == false )
        {
            TEST_FAIL_MESSAGE( "Timed out waiting to update Shadow document." );
        }

        /* Set the expected callback type to GET. */
        callbackParam.expectedType = AWS_IOT_SHADOW_GET_COMPLETE;

        /* Retrieve the Shadow document. */
        status = AwsIotShadow_Get( _AwsIotTestMqttConnection,
                                   &documentInfo,
                                   0,
                                   &callbackInfo,
                                   &( callbackParam.reference ) );
        TEST_ASSERT_EQUAL_INT( AWS_IOT_SHADOW_STATUS_PENDING, status );

        if( AwsIotSemaphore_TimedWait( &( callbackParam.waitSem ),
                                       AWS_IOT_TEST_SHADOW_TIMEOUT ) == false )
        {
            TEST_FAIL_MESSAGE( "Timed out waiting to retrieve Shadow document." );
        }

        /* Set the expected callback type to DELETE. */
        callbackParam.expectedType = AWS_IOT_SHADOW_DELETE_COMPLETE;

        /* Delete the Shadow document. */
        status = AwsIotShadow_Delete( _AwsIotTestMqttConnection,
                                      AWS_IOT_TEST_SHADOW_THING_NAME,
                                      _THING_NAME_LENGTH,
                                      0,
                                      &callbackInfo,
                                      &( callbackParam.reference ) );

        if( AwsIotSemaphore_TimedWait( &( callbackParam.waitSem ),
                                       AWS_IOT_TEST_SHADOW_TIMEOUT ) == false )
        {
            TEST_FAIL_MESSAGE( "Timed out waiting to delete Shadow document." );
        }
    }

    AwsIotSemaphore_Destroy( &( callbackParam.waitSem ) );
}

/*-----------------------------------------------------------*/

/**
 * @brief Run the Update-Get-Delete blocking tests at various QoS.
 */
static void _updateGetDeleteBlocking( int QoS )
{
    AwsIotShadowError_t status = AWS_IOT_SHADOW_STATUS_PENDING;
    AwsIotShadowDocumentInfo_t documentInfo = AWS_IOT_SHADOW_DOCUMENT_INFO_INITIALIZER;
    const char * pShadowDocument = NULL, * pJsonValue = NULL;
    size_t shadowDocumentLength = 0, jsonValueLength = 0;

    /* Initialize the common members of the Shadow document info. */
    documentInfo.pThingName = AWS_IOT_TEST_SHADOW_THING_NAME;
    documentInfo.thingNameLength = _THING_NAME_LENGTH;
    documentInfo.QoS = QoS;

    /* Set the members of the Shadow document info for UPDATE. */
    documentInfo.update.pUpdateDocument = _TEST_SHADOW_DOCUMENT;
    documentInfo.update.updateDocumentLength = _TEST_SHADOW_DOCUMENT_LENGTH;

    /* Create a new Shadow document. */
    status = AwsIotShadow_TimedUpdate( _AwsIotTestMqttConnection,
                                       &documentInfo,
                                       0,
                                       AWS_IOT_TEST_SHADOW_TIMEOUT );
    TEST_ASSERT_EQUAL( AWS_IOT_SHADOW_SUCCESS, status );

    /* Set the members of the Shadow document info for GET. */
    documentInfo.get.mallocDocument = AwsIotTest_Malloc;

    /* Retrieve the Shadow document. */
    status = AwsIotShadow_TimedGet( _AwsIotTestMqttConnection,
                                    &documentInfo,
                                    0,
                                    AWS_IOT_TEST_SHADOW_TIMEOUT,
                                    &pShadowDocument,
                                    &shadowDocumentLength );
    TEST_ASSERT_EQUAL_INT( AWS_IOT_SHADOW_SUCCESS, status );

    /* Check the retrieved Shadow document. */
    TEST_ASSERT_GREATER_THAN( 0, shadowDocumentLength );
    TEST_ASSERT_NOT_NULL( pShadowDocument );
    TEST_ASSERT_EQUAL_INT( true, AwsIotJsonUtils_FindJsonValue( pShadowDocument,
                                                                shadowDocumentLength,
                                                                "key",
                                                                3,
                                                                &pJsonValue,
                                                                &jsonValueLength ) );
    TEST_ASSERT_EQUAL( 7, jsonValueLength );
    TEST_ASSERT_EQUAL_STRING_LEN( "\"value\"", pJsonValue, jsonValueLength );

    /* Free the retrieved Shadow document. */
    AwsIotTest_Free( ( void * ) pShadowDocument );

    /* Delete the Shadow document. */
    status = AwsIotShadow_TimedDelete( _AwsIotTestMqttConnection,
                                       AWS_IOT_TEST_SHADOW_THING_NAME,
                                       _THING_NAME_LENGTH,
                                       0,
                                       AWS_IOT_TEST_SHADOW_TIMEOUT );
    TEST_ASSERT_EQUAL( AWS_IOT_SHADOW_SUCCESS, status );
}

/*-----------------------------------------------------------*/

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
    int clientIdentifierLength = 0;
    AwsIotMqttConnectInfo_t connectInfo = AWS_IOT_MQTT_CONNECT_INFO_INITIALIZER;
    AwsIotShadowError_t status = AWS_IOT_SHADOW_STATUS_PENDING;

    /* Set up the network stack. */
    if( AwsIotTest_NetworkSetup() == false )
    {
        TEST_FAIL_MESSAGE( "Failed to set up network connection." );
    }

    /* Initialize the MQTT library. */
    if( AwsIotMqtt_Init() != AWS_IOT_MQTT_SUCCESS )
    {
        TEST_FAIL_MESSAGE( "Failed to initialize MQTT library." );
    }

    /* Generate a new, unique client identifier based on the time. */
    clientIdentifierLength = snprintf( _pClientIdentifier,
                                       _CLIENT_IDENTIFIER_MAX_LENGTH,
                                       "aws%llu",
                                       ( long long unsigned int ) AwsIotClock_GetTimeMs() );

    if( clientIdentifierLength <= 0 )
    {
        TEST_FAIL_MESSAGE( "Failed to generate MQTT client identifier." );
    }

    /* Set the members of the connect info. */
    connectInfo.pClientIdentifier = _pClientIdentifier;
    connectInfo.clientIdentifierLength = ( uint16_t ) clientIdentifierLength;
    connectInfo.keepAliveSeconds = AWS_IOT_TEST_MQTT_SHORT_KEEPALIVE_INTERVAL_S;

    /* Establish an MQTT connection. */
    if( AwsIotMqtt_Connect( &_AwsIotTestMqttConnection,
                            &_AwsIotTestNetworkInterface,
                            &connectInfo,
                            NULL,
                            AWS_IOT_TEST_SHADOW_TIMEOUT ) != AWS_IOT_MQTT_SUCCESS )
    {
        TEST_FAIL_MESSAGE( "Failed to establish MQTT connection for Shadow tests." );
    }

    /* Initialize the Shadow library. */
    if( AwsIotShadow_Init( 0 ) != AWS_IOT_SHADOW_SUCCESS )
    {
        TEST_FAIL_MESSAGE( "Failed to initialize Shadow library." );
    }

    /* Delete any existing Shadow so all tests start with no Shadow. */
    status = AwsIotShadow_TimedDelete( _AwsIotTestMqttConnection,
                                       AWS_IOT_TEST_SHADOW_THING_NAME,
                                       _THING_NAME_LENGTH,
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
    /* Disconnect the MQTT connection. */
    AwsIotMqtt_Disconnect( _AwsIotTestMqttConnection, false );

    /* Clean up the Shadow library. */
    AwsIotShadow_Cleanup();

    /* Clean up the network stack. */
    AwsIotTest_NetworkCleanup();

    /* Clean up the MQTT library. */
    AwsIotMqtt_Cleanup();
    _AwsIotTestMqttConnection = AWS_IOT_MQTT_CONNECTION_INITIALIZER;
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
    _updateGetDeleteAsync( 0 );
}

/*-----------------------------------------------------------*/

/**
 * @brief Update-Get-Delete asynchronous (QoS 1).
 */
TEST( Shadow_System, UpdateGetDeleteAsyncQoS1 )
{
    _updateGetDeleteAsync( 1 );
}

/*-----------------------------------------------------------*/

/**
 * @brief Update-Get-Delete blocking (QoS 0).
 */
TEST( Shadow_System, UpdateGetDeleteBlockingQoS0 )
{
    _updateGetDeleteBlocking( 0 );
}

/*-----------------------------------------------------------*/

/**
 * @brief Update-Get-Delete blocking (QoS 1).
 */
TEST( Shadow_System, UpdateGetDeleteBlockingQoS1 )
{
    _updateGetDeleteBlocking( 1 );
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
    AwsIotSemaphore_t waitSem;

    /* Create a semaphore to wait on. */
    TEST_ASSERT_EQUAL_INT( true, AwsIotSemaphore_Create( &waitSem, 0, 1 ) );

    /* Set the delta callback information. */
    deltaCallback.param1 = &waitSem;
    deltaCallback.function = _deltaCallback;

    /* Set a desired state in the Update document. */
    updateDocument.pThingName = AWS_IOT_TEST_SHADOW_THING_NAME;
    updateDocument.thingNameLength = _THING_NAME_LENGTH;
    updateDocument.update.pUpdateDocument = "{\"state\": {\"desired\": {\"key\": true}}, \"clientToken\":\"shadowtest\"}";
    updateDocument.update.updateDocumentLength = 65;

    if( TEST_PROTECT() )
    {
        /* Set the delta callback. */
        status = AwsIotShadow_SetDeltaCallback( _AwsIotTestMqttConnection,
                                                AWS_IOT_TEST_SHADOW_THING_NAME,
                                                _THING_NAME_LENGTH,
                                                0,
                                                &deltaCallback );
        TEST_ASSERT_EQUAL( AWS_IOT_SHADOW_SUCCESS, status );

        /* Create a Shadow document with a desired state. */
        status = AwsIotShadow_TimedUpdate( _AwsIotTestMqttConnection,
                                           &updateDocument,
                                           AWS_IOT_SHADOW_FLAG_KEEP_SUBSCRIPTIONS,
                                           AWS_IOT_TEST_SHADOW_TIMEOUT );
        TEST_ASSERT_EQUAL( AWS_IOT_SHADOW_SUCCESS, status );

        /* Set a different reported state in the Update document. */
        updateDocument.update.pUpdateDocument = "{\"state\": {\"reported\": {\"key\": false}}, \"clientToken\":\"shadowtest\"}";
        updateDocument.update.updateDocumentLength = 67;

        /* Create a Shadow document with a reported state. */
        status = AwsIotShadow_TimedUpdate( _AwsIotTestMqttConnection,
                                           &updateDocument,
                                           0,
                                           AWS_IOT_TEST_SHADOW_TIMEOUT );
        TEST_ASSERT_EQUAL( AWS_IOT_SHADOW_SUCCESS, status );

        /* Block on the wait semaphore until the delta callback is invoked. */
        if( AwsIotSemaphore_TimedWait( &waitSem, AWS_IOT_TEST_SHADOW_TIMEOUT ) == false )
        {
            TEST_FAIL_MESSAGE( "Timed out waiting for delta callback." );
        }

        /* Remove the delta callback. */
        status = AwsIotShadow_SetDeltaCallback( _AwsIotTestMqttConnection,
                                                AWS_IOT_TEST_SHADOW_THING_NAME,
                                                _THING_NAME_LENGTH,
                                                0,
                                                NULL );
        TEST_ASSERT_EQUAL( AWS_IOT_SHADOW_SUCCESS, status );

        /* Remove persistent subscriptions for Shadow Update. */
        status = AwsIotShadow_RemovePersistentSubscriptions( _AwsIotTestMqttConnection,
                                                             AWS_IOT_TEST_SHADOW_THING_NAME,
                                                             _THING_NAME_LENGTH,
                                                             AWS_IOT_SHADOW_FLAG_REMOVE_UPDATE_SUBSCRIPTIONS );
        TEST_ASSERT_EQUAL( AWS_IOT_SHADOW_SUCCESS, status );
    }

    AwsIotSemaphore_Destroy( &waitSem );
}

/*-----------------------------------------------------------*/

TEST( Shadow_System, UpdatedCallback )
{
    AwsIotShadowError_t status = AWS_IOT_SHADOW_STATUS_PENDING;
    AwsIotShadowCallbackInfo_t updatedCallback = AWS_IOT_SHADOW_CALLBACK_INFO_INITIALIZER;
    AwsIotShadowDocumentInfo_t updateDocument = AWS_IOT_SHADOW_DOCUMENT_INFO_INITIALIZER;
    AwsIotSemaphore_t waitSem;

    /* Create a semaphore to wait on. */
    TEST_ASSERT_EQUAL_INT( true, AwsIotSemaphore_Create( &waitSem, 0, 1 ) );

    /* Set the delta callback information. */
    updatedCallback.param1 = &waitSem;
    updatedCallback.function = _updatedCallback;

    /* Set a desired state in the Update document. */
    updateDocument.pThingName = AWS_IOT_TEST_SHADOW_THING_NAME;
    updateDocument.thingNameLength = _THING_NAME_LENGTH;
    updateDocument.update.pUpdateDocument = "{\"state\": {\"desired\": {\"key\": true}}, \"clientToken\":\"shadowtest\"}";
    updateDocument.update.updateDocumentLength = 65;

    if( TEST_PROTECT() )
    {
        /* Set the updated callback. */
        status = AwsIotShadow_SetUpdatedCallback( _AwsIotTestMqttConnection,
                                                  AWS_IOT_TEST_SHADOW_THING_NAME,
                                                  _THING_NAME_LENGTH,
                                                  0,
                                                  &updatedCallback );
        TEST_ASSERT_EQUAL( AWS_IOT_SHADOW_SUCCESS, status );

        /* Create a Shadow document. */
        status = AwsIotShadow_TimedUpdate( _AwsIotTestMqttConnection,
                                           &updateDocument,
                                           0,
                                           AWS_IOT_TEST_SHADOW_TIMEOUT );
        TEST_ASSERT_EQUAL( AWS_IOT_SHADOW_SUCCESS, status );

        /* Block on the wait semaphore until the updated callback is invoked. */
        if( AwsIotSemaphore_TimedWait( &waitSem, AWS_IOT_TEST_SHADOW_TIMEOUT ) == false )
        {
            TEST_FAIL_MESSAGE( "Timed out waiting for updated callback." );
        }

        /* Remove the updated callback. */
        status = AwsIotShadow_SetUpdatedCallback( _AwsIotTestMqttConnection,
                                                  AWS_IOT_TEST_SHADOW_THING_NAME,
                                                  _THING_NAME_LENGTH,
                                                  0,
                                                  NULL );
        TEST_ASSERT_EQUAL( AWS_IOT_SHADOW_SUCCESS, status );
    }

    AwsIotSemaphore_Destroy( &waitSem );
}

/*-----------------------------------------------------------*/
