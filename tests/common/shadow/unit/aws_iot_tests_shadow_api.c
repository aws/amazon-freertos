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
 * @file aws_iot_tests_shadow_api.c
 * @brief Tests for the user-facing API functions (declared in aws_iot_shadow.h).
 */

/* Build using a config header, if provided. */
#ifdef IOT_CONFIG_FILE
    #include IOT_CONFIG_FILE
#endif

/* Standard includes. */
#include <stdint.h>
#include <string.h>

/* POSIX includes. */
#ifdef POSIX_UNISTD_HEADER
    #include POSIX_UNISTD_HEADER
#else
    #include <unistd.h>
#endif

/* Shadow internal include. */
#include "private/aws_iot_shadow_internal.h"

/* Undefine logging configuration set in Shadow internal header. */
#undef _LIBRARY_LOG_NAME
#undef _LIBRARY_LOG_LEVEL

/* MQTT internal include. */
#include "private/iot_mqtt_internal.h"

/* Undefine logging configuration set in MQTT internal header. */
#undef _LIBRARY_LOG_NAME
#undef _LIBRARY_LOG_LEVEL

/* Platform layer includes. */
#include "platform/iot_clock.h"
#include "platform/iot_threads.h"

/* Test framework includes. */
#include "unity_fixture.h"

/* MQTT test access include. */
#include "iot_test_access_mqtt.h"

/* Require Shadow library asserts to be enabled for these tests. The Shadow
 * assert function is used to abort the tests on failure from the MQTT send
 * or receive threads. */
#if AWS_IOT_SHADOW_ENABLE_ASSERTS == 0
    #error "Shadow API unit tests require AWS_IOT_SHADOW_ENABLE_ASSERTS to be 1."
#endif

/*-----------------------------------------------------------*/

/**
 * @brief The Thing Name shared among all the tests.
 */
#define _TEST_THING_NAME                "TestThingName"

/**
 * @brief The length of #_TEST_THING_NAME.
 */
#define _TEST_THING_NAME_LENGTH         ( sizeof( _TEST_THING_NAME ) - 1 )

/**
 * @brief A delay that simulates the time required for an MQTT packet to be sent
 * to the server and for the server to send a response.
 */
#define _NETWORK_ROUND_TRIP_TIME_MS     ( 250 )

/**
 * @brief The maximum size of any MQTT acknowledgement packet (e.g. SUBACK,
 * PUBACK, UNSUBACK) used in these tests.
 */
#define _ACKNOWLEDGEMENT_PACKET_SIZE    ( 5 )

/*-----------------------------------------------------------*/

/**
 * @brief The MQTT connection object shared among all the tests.
 */
static _mqttConnection_t * _pMqttConnection = NULL;

/**
 * @brief Timer used to simulate a response from the network.
 */
static IotTimer_t _receiveTimer;

/**
 * @brief Synchronizes the MQTT send and receive threads in these tests.
 */
static IotMutex_t _lastPacketMutex;

/**
 * @brief The type of the last packet sent by the send thread.
 *
 * Must be one of: PUBLISH, SUBSCRIBE, UNSUBSCRIBE.
 */
static uint8_t _lastPacketType = 0;

/**
 * @brief The packet identifier of the last packet send by the send thread.
 */
static uint16_t _lastPacketIdentifier = 0;

/*-----------------------------------------------------------*/

/**
 * @brief Invokes the MQTT receive callback to simulate a response received from
 * the network.
 */
static void _receiveThread( void * pArgument )
{
    uint8_t pReceivedData[ _ACKNOWLEDGEMENT_PACKET_SIZE ] = { 0 };
    size_t receivedDataLength = 0;
    int32_t bytesProcessed = 0;

    /* Silence warnings about unused parameters. */
    ( void ) pArgument;

    /* Lock mutex to read and process the last packet sent. */
    IotMutex_Lock( &_lastPacketMutex );

    /* Ensure that the last packet type and identifier were set. */
    AwsIotShadow_Assert( _lastPacketType != 0 );
    AwsIotShadow_Assert( _lastPacketIdentifier != 0 );

    /* Set the packet identifier in the ACK packet. */
    pReceivedData[ 2 ] = _UINT16_HIGH_BYTE( _lastPacketIdentifier );
    pReceivedData[ 3 ] = _UINT16_LOW_BYTE( _lastPacketIdentifier );

    /* Create the corresponding ACK packet based on the last packet type. */
    switch( _lastPacketType )
    {
        case _MQTT_PACKET_TYPE_PUBLISH:

            pReceivedData[ 0 ] = _MQTT_PACKET_TYPE_PUBACK;
            pReceivedData[ 1 ] = 2;
            receivedDataLength = 4;
            break;

        case _MQTT_PACKET_TYPE_SUBSCRIBE:

            pReceivedData[ 0 ] = _MQTT_PACKET_TYPE_SUBACK;
            pReceivedData[ 1 ] = 3;
            pReceivedData[ 4 ] = 1;
            receivedDataLength = 5;
            break;

        case _MQTT_PACKET_TYPE_UNSUBSCRIBE:

            pReceivedData[ 0 ] = _MQTT_PACKET_TYPE_UNSUBACK;
            pReceivedData[ 1 ] = 2;
            receivedDataLength = 4;
            break;

        default:

            /* The only valid outgoing packets are PUBLISH, SUBSCRIBE, and
             * UNSUBSCRIBE. Abort if any other packet is found. */
            AwsIotShadow_Assert( 0 );
    }

    /* Call the MQTT receive callback to process the ACK packet. */
    bytesProcessed = IotMqtt_ReceiveCallback( ( IotMqttConnection_t * ) &_pMqttConnection,
                                              NULL,
                                              pReceivedData,
                                              receivedDataLength,
                                              0,
                                              NULL );
    AwsIotShadow_Assert( bytesProcessed == ( int32_t ) receivedDataLength );

    IotMutex_Unlock( &_lastPacketMutex );
}

/*-----------------------------------------------------------*/

/**
 * @brief A send function that always "succeeds". It also sets the receive
 * timer to respond with an ACK when necessary.
 */
static size_t _sendSuccess( void * pSendContext,
                            const uint8_t * const pMessage,
                            size_t messageLength )
{
    IotMqttError_t status = IOT_MQTT_STATUS_PENDING;
    const uint8_t * pPacketIdentifier = NULL;
    IotMqttPublishInfo_t decodedPublish = IOT_MQTT_PUBLISH_INFO_INITIALIZER;
    size_t publishBytesProcessed = 0;

    /* Ignore the send context. */
    ( void ) pSendContext;

    /* Lock the mutex to modify the information on the last packet sent. */
    IotMutex_Lock( &_lastPacketMutex );

    /* Set the last packet type based on the outgoing message. */
    switch( _IotMqtt_GetPacketType( pMessage, messageLength ) )
    {
        case ( _MQTT_PACKET_TYPE_PUBLISH & 0xf0 ):

            /* Only set the last packet type to PUBLISH for QoS 1. */
            if( ( ( *pMessage & 0x06 ) >> 1 ) == 1 )
            {
                _lastPacketType = _MQTT_PACKET_TYPE_PUBLISH;
            }
            else
            {
                _lastPacketType = 0;
                _lastPacketIdentifier = 0;
            }

            break;

        case ( _MQTT_PACKET_TYPE_SUBSCRIBE & 0xf0 ):
            _lastPacketType = _MQTT_PACKET_TYPE_SUBSCRIBE;
            break;

        case ( _MQTT_PACKET_TYPE_UNSUBSCRIBE & 0xf0 ):
            _lastPacketType = _MQTT_PACKET_TYPE_UNSUBSCRIBE;
            break;

        default:

            /* The only valid outgoing packets are PUBLISH, SUBSCRIBE, and
             * UNSUBSCRIBE. Abort if any other packet is found. */
            AwsIotShadow_Assert( 0 );
    }

    /* Check if a network response is needed. */
    if( _lastPacketType != 0 )
    {
        /* Decode the remaining length. */
        if( _lastPacketType != _MQTT_PACKET_TYPE_PUBLISH )
        {
            status = IotTestMqtt_decodeRemainingLength( pMessage + 1,
                                                        &pPacketIdentifier,
                                                        NULL );

            /* Save the packet identifier as the last packet identifier. */
            _lastPacketIdentifier = _UINT16_DECODE( pPacketIdentifier );
        }
        else
        {
            status = _IotMqtt_DeserializePublish( pMessage,
                                                  messageLength,
                                                  &decodedPublish,
                                                  &_lastPacketIdentifier,
                                                  &publishBytesProcessed );

            AwsIotShadow_Assert( publishBytesProcessed == messageLength );
        }

        AwsIotShadow_Assert( status == IOT_MQTT_SUCCESS );

        /* Set the receive thread to run after a "network round-trip". */
        IotClock_TimerArm( &_receiveTimer,
                           _NETWORK_ROUND_TRIP_TIME_MS,
                           0 );
    }

    IotMutex_Unlock( &_lastPacketMutex );

    /* Return the message length to simulate a successful send. */
    return messageLength;
}

/*-----------------------------------------------------------*/

/**
 * @brief Test group for Shadow API tests.
 */
TEST_GROUP( Shadow_Unit_API );

/*-----------------------------------------------------------*/

/**
 * @brief Test setup for Shadow API tests.
 */
TEST_SETUP( Shadow_Unit_API )
{
    IotMqttNetIf_t networkInterface = IOT_MQTT_NETIF_INITIALIZER;

    /* Clear the last packet type and identifier. */
    _lastPacketType = 0;
    _lastPacketIdentifier = 0;

    /* Create the mutex that synchronizes the receive callback and send thread. */
    TEST_ASSERT_EQUAL_INT( true, IotMutex_Create( &_lastPacketMutex, false ) );

    /* Create the receive thread timer. */
    IotClock_TimerCreate( &_receiveTimer,
                          _receiveThread,
                          NULL );

    /* Initialize the MQTT library. */
    TEST_ASSERT_EQUAL( IOT_MQTT_SUCCESS, IotMqtt_Init() );

    /* Set the network interface send function. */
    networkInterface.send = _sendSuccess;

    /* Initialize the MQTT connection object to use for the Shadow tests. */
    _pMqttConnection = IotTestMqtt_createMqttConnection( false,
                                                         &networkInterface,
                                                         0 );

    /* Initialize the Shadow library. */
    TEST_ASSERT_EQUAL( AWS_IOT_SHADOW_SUCCESS, AwsIotShadow_Init( 0 ) );
}

/*-----------------------------------------------------------*/

/**
 * @brief Test tear down for Shadow API tests.
 */
TEST_TEAR_DOWN( Shadow_Unit_API )
{
    /* Clean up the Shadow library. */
    AwsIotShadow_Cleanup();

    /* Clean up the MQTT connection object. */
    IotMqtt_Disconnect( _pMqttConnection, true );

    /* Clean up the MQTT library. */
    IotMqtt_Cleanup();

    /* Destroy the receive thread timer. */
    IotClock_TimerDestroy( &_receiveTimer );

    /* Wait for the receive thread to finish and release the last packet mutex. */
    IotMutex_Lock( &_lastPacketMutex );

    /* Destroy the last packet mutex. */
    IotMutex_Unlock( &_lastPacketMutex );
    IotMutex_Destroy( &_lastPacketMutex );
}

/*-----------------------------------------------------------*/

/**
 * @brief Test group runner for Shadow API tests.
 */
TEST_GROUP_RUNNER( Shadow_Unit_API )
{
    RUN_TEST_CASE( Shadow_Unit_API, Init );
    RUN_TEST_CASE( Shadow_Unit_API, OperationInvalidParameters );
    RUN_TEST_CASE( Shadow_Unit_API, DocumentInvalidParameters );
    RUN_TEST_CASE( Shadow_Unit_API, WaitInvalidParameters );
    RUN_TEST_CASE( Shadow_Unit_API, DeleteMallocFail );
    RUN_TEST_CASE( Shadow_Unit_API, GetMallocFail );
    RUN_TEST_CASE( Shadow_Unit_API, UpdateMallocFail );
}

/*-----------------------------------------------------------*/

/**
 * @brief Tests the function @ref shadow_function_init.
 */
TEST( Shadow_Unit_API, Init )
{
    /* Check that test set up set the default value. */
    TEST_ASSERT_EQUAL( AWS_IOT_SHADOW_DEFAULT_MQTT_TIMEOUT_MS, _AwsIotShadowMqttTimeoutMs );

    /* The Shadow library was already initialized by test set up. Clean it up
     * before running this test. */
    AwsIotShadow_Cleanup();

    /* Set a MQTT timeout. */
    AwsIotShadow_Init( AWS_IOT_SHADOW_DEFAULT_MQTT_TIMEOUT_MS + 1 );
    TEST_ASSERT_EQUAL( AWS_IOT_SHADOW_DEFAULT_MQTT_TIMEOUT_MS + 1, _AwsIotShadowMqttTimeoutMs );

    /* Cleanup should restore the default MQTT timeout. */
    AwsIotShadow_Cleanup();
    TEST_ASSERT_EQUAL( AWS_IOT_SHADOW_DEFAULT_MQTT_TIMEOUT_MS, _AwsIotShadowMqttTimeoutMs );

    /* Initialize the Shadow library for test clean up. */
    AwsIotShadow_Init( 0 );
}

/*-----------------------------------------------------------*/

/**
 * @brief Tests the behavior of Shadow operation functions with various
 * invalid parameters.
 */
TEST( Shadow_Unit_API, OperationInvalidParameters )
{
    AwsIotShadowError_t status = AWS_IOT_SHADOW_STATUS_PENDING;
    AwsIotShadowDocumentInfo_t documentInfo = AWS_IOT_SHADOW_DOCUMENT_INFO_INITIALIZER;
    AwsIotShadowReference_t reference = AWS_IOT_SHADOW_REFERENCE_INITIALIZER;
    AwsIotShadowCallbackInfo_t callbackInfo = AWS_IOT_SHADOW_CALLBACK_INFO_INITIALIZER;

    /* Missing Thing Name. */
    status = AwsIotShadow_Delete( _pMqttConnection,
                                  NULL,
                                  0,
                                  0,
                                  NULL,
                                  NULL );
    TEST_ASSERT_EQUAL( AWS_IOT_SHADOW_BAD_PARAMETER, status );

    status = AwsIotShadow_Update( _pMqttConnection,
                                  &documentInfo,
                                  0,
                                  0,
                                  NULL );
    TEST_ASSERT_EQUAL( AWS_IOT_SHADOW_BAD_PARAMETER, status );

    /* Thing Name too long. */
    status = AwsIotShadow_Delete( _pMqttConnection,
                                  _TEST_THING_NAME,
                                  _MAX_THING_NAME_LENGTH + 1,
                                  0,
                                  NULL,
                                  NULL );
    TEST_ASSERT_EQUAL( AWS_IOT_SHADOW_BAD_PARAMETER, status );

    /* No reference with waitable operation. */
    status = AwsIotShadow_Delete( _pMqttConnection,
                                  _TEST_THING_NAME,
                                  _TEST_THING_NAME_LENGTH,
                                  AWS_IOT_SHADOW_FLAG_WAITABLE,
                                  NULL,
                                  NULL );
    TEST_ASSERT_EQUAL( AWS_IOT_SHADOW_BAD_PARAMETER, status );

    /* Both callback and waitable flag set. */
    status = AwsIotShadow_Delete( _pMqttConnection,
                                  _TEST_THING_NAME,
                                  _TEST_THING_NAME_LENGTH,
                                  AWS_IOT_SHADOW_FLAG_WAITABLE,
                                  &callbackInfo,
                                  &reference );
    TEST_ASSERT_EQUAL( AWS_IOT_SHADOW_BAD_PARAMETER, status );

    /* No callback for non-waitable GET. */
    documentInfo.pThingName = _TEST_THING_NAME;
    documentInfo.thingNameLength = _TEST_THING_NAME_LENGTH;
    status = AwsIotShadow_Get( _pMqttConnection,
                               &documentInfo,
                               0,
                               NULL,
                               NULL );
    TEST_ASSERT_EQUAL( AWS_IOT_SHADOW_BAD_PARAMETER, status );

    /* Callback function not set. */
    status = AwsIotShadow_Delete( _pMqttConnection,
                                  _TEST_THING_NAME,
                                  _TEST_THING_NAME_LENGTH,
                                  0,
                                  &callbackInfo,
                                  &reference );
    TEST_ASSERT_EQUAL( AWS_IOT_SHADOW_BAD_PARAMETER, status );
}

/*-----------------------------------------------------------*/

/**
 * @brief Tests the behavior of Shadow operation functions with an invalid
 * document info parameter.
 */
TEST( Shadow_Unit_API, DocumentInvalidParameters )
{
    AwsIotShadowError_t status = AWS_IOT_SHADOW_STATUS_PENDING;
    AwsIotShadowDocumentInfo_t documentInfo = AWS_IOT_SHADOW_DOCUMENT_INFO_INITIALIZER;
    AwsIotShadowReference_t reference = AWS_IOT_SHADOW_REFERENCE_INITIALIZER;

    /* Missing Thing Name. */
    status = AwsIotShadow_Get( _pMqttConnection,
                               &documentInfo,
                               AWS_IOT_SHADOW_FLAG_WAITABLE,
                               NULL,
                               &reference );
    TEST_ASSERT_EQUAL( AWS_IOT_SHADOW_BAD_PARAMETER, status );
    documentInfo.pThingName = _TEST_THING_NAME;
    documentInfo.thingNameLength = _TEST_THING_NAME_LENGTH;

    /* Invalid QoS. */
    documentInfo.qos = 3;
    status = AwsIotShadow_Get( _pMqttConnection,
                               &documentInfo,
                               AWS_IOT_SHADOW_FLAG_WAITABLE,
                               NULL,
                               &reference );
    TEST_ASSERT_EQUAL( AWS_IOT_SHADOW_BAD_PARAMETER, status );
    documentInfo.qos = IOT_MQTT_QOS_0;

    /* Invalid retry parameters. */
    documentInfo.retryLimit = -1;
    status = AwsIotShadow_Get( _pMqttConnection,
                               &documentInfo,
                               AWS_IOT_SHADOW_FLAG_WAITABLE,
                               NULL,
                               &reference );
    TEST_ASSERT_EQUAL( AWS_IOT_SHADOW_BAD_PARAMETER, status );

    documentInfo.retryLimit = 1;
    status = AwsIotShadow_Get( _pMqttConnection,
                               &documentInfo,
                               AWS_IOT_SHADOW_FLAG_WAITABLE,
                               NULL,
                               &reference );
    TEST_ASSERT_EQUAL( AWS_IOT_SHADOW_BAD_PARAMETER, status );
    documentInfo.retryLimit = 0;

    /* Waitable Shadow get with no memory allocation function. */
    status = AwsIotShadow_Get( _pMqttConnection,
                               &documentInfo,
                               AWS_IOT_SHADOW_FLAG_WAITABLE,
                               NULL,
                               &reference );
    TEST_ASSERT_EQUAL( AWS_IOT_SHADOW_BAD_PARAMETER, status );

    /* Update with no document. */
    status = AwsIotShadow_Update( _pMqttConnection,
                                  &documentInfo,
                                  0,
                                  0,
                                  NULL );
    TEST_ASSERT_EQUAL( AWS_IOT_SHADOW_BAD_PARAMETER, status );

    /* Update with no client token. */
    documentInfo.update.pUpdateDocument = "{\"state\":{\"reported\":{null}}}";
    documentInfo.update.updateDocumentLength = 29;
    status = AwsIotShadow_Update( _pMqttConnection,
                                  &documentInfo,
                                  0,
                                  0,
                                  NULL );
    TEST_ASSERT_EQUAL( AWS_IOT_SHADOW_BAD_PARAMETER, status );

    /* Client token too long. */
    documentInfo.update.pUpdateDocument = "{\"state\":{\"reported\":{null}}},\"clientToken\": "
                                          "\"aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa\"";
    documentInfo.update.updateDocumentLength = 146;
    status = AwsIotShadow_Update( _pMqttConnection,
                                  &documentInfo,
                                  0,
                                  0,
                                  NULL );
    TEST_ASSERT_EQUAL( AWS_IOT_SHADOW_BAD_PARAMETER, status );
}

/*-----------------------------------------------------------*/

/**
 * @brief Tests the behavior of @ref shadow_function_wait with various
 * invalid parameters.
 */
TEST( Shadow_Unit_API, WaitInvalidParameters )
{
    AwsIotShadowError_t status = AWS_IOT_SHADOW_STATUS_PENDING;
    _shadowOperation_t operation;

    ( void ) memset( &operation, 0x00, sizeof( _shadowOperation_t ) );

    /* NULL reference. */
    status = AwsIotShadow_Wait( NULL, 0, NULL, NULL );
    TEST_ASSERT_EQUAL( AWS_IOT_SHADOW_BAD_PARAMETER, status );

    /* No waitable flag set. */
    status = AwsIotShadow_Wait( &operation, 0, NULL, NULL );
    TEST_ASSERT_EQUAL( AWS_IOT_SHADOW_BAD_PARAMETER, status );

    /* NULL output parameters for Shadow GET. */
    operation.flags = AWS_IOT_SHADOW_FLAG_WAITABLE;
    operation.type = _SHADOW_GET;
    status = AwsIotShadow_Wait( &operation, 0, NULL, NULL );
}

/*-----------------------------------------------------------*/

/**
 * @brief Tests the behavior of @ref shadow_function_delete when memory
 * allocation fails at various points.
 */
TEST( Shadow_Unit_API, DeleteMallocFail )
{
    int32_t i = 0;
    AwsIotShadowError_t status = AWS_IOT_SHADOW_STATUS_PENDING;
    AwsIotShadowReference_t reference = AWS_IOT_SHADOW_REFERENCE_INITIALIZER;

    for( i = 0; ; i++ )
    {
        UnityMalloc_MakeMallocFailAfterCount( i );

        /* Call Shadow DELETE. Memory allocation will fail at various times
         * during this call. */
        status = AwsIotShadow_Delete( _pMqttConnection,
                                      _TEST_THING_NAME,
                                      _TEST_THING_NAME_LENGTH,
                                      AWS_IOT_SHADOW_FLAG_WAITABLE,
                                      NULL,
                                      &reference );

        /* Once the Shadow DELETE call succeeds, wait for it to complete. */
        if( status == AWS_IOT_SHADOW_STATUS_PENDING )
        {
            /* No response will be received from the network, so the Shadow DELETE
             * is expected to time out. */
            TEST_ASSERT_EQUAL( AWS_IOT_SHADOW_TIMEOUT,
                               AwsIotShadow_Wait( reference, 0, NULL, NULL ) );
            break;
        }

        /* If the return value isn't success, check that it is memory allocation
         * failure. */
        TEST_ASSERT_EQUAL( AWS_IOT_SHADOW_NO_MEMORY, status );
    }

    /* Wait for any pending QoS 0 publishes to clean up. */
    sleep( 1 );
}

/*-----------------------------------------------------------*/

/**
 * @brief Tests the behavior of @ref shadow_function_get when memory
 * allocation fails at various points.
 */
TEST( Shadow_Unit_API, GetMallocFail )
{
    int32_t i = 0;
    AwsIotShadowError_t status = AWS_IOT_SHADOW_STATUS_PENDING;
    AwsIotShadowDocumentInfo_t documentInfo = AWS_IOT_SHADOW_DOCUMENT_INFO_INITIALIZER;
    AwsIotShadowReference_t reference = AWS_IOT_SHADOW_REFERENCE_INITIALIZER;
    const char * pRetrievedDocument = NULL;
    size_t retrievedDocumentSize = 0;

    /* Set the members of the document info. */
    documentInfo.pThingName = _TEST_THING_NAME;
    documentInfo.thingNameLength = _TEST_THING_NAME_LENGTH;
    documentInfo.qos = IOT_MQTT_QOS_1;
    documentInfo.get.mallocDocument = IotTest_Malloc;

    for( i = 0; ; i++ )
    {
        UnityMalloc_MakeMallocFailAfterCount( i );

        /* Call Shadow GET. Memory allocation will fail at various times
         * during this call. */
        status = AwsIotShadow_Get( _pMqttConnection,
                                   &documentInfo,
                                   AWS_IOT_SHADOW_FLAG_WAITABLE,
                                   NULL,
                                   &reference );

        /* Once the Shadow GET call succeeds, wait for it to complete. */
        if( status == AWS_IOT_SHADOW_STATUS_PENDING )
        {
            /* No response will be received from the network, so the Shadow GET
             * is expected to time out. */
            TEST_ASSERT_EQUAL( AWS_IOT_SHADOW_TIMEOUT,
                               AwsIotShadow_Wait( reference,
                                                  0,
                                                  &pRetrievedDocument,
                                                  &retrievedDocumentSize ) );
            break;
        }

        /* If the return value isn't success, check that it is memory allocation
         * failure. */
        TEST_ASSERT_EQUAL( AWS_IOT_SHADOW_NO_MEMORY, status );
    }
}

/*-----------------------------------------------------------*/

TEST( Shadow_Unit_API, UpdateMallocFail )
{
    int32_t i = 0;
    AwsIotShadowError_t status = AWS_IOT_SHADOW_STATUS_PENDING;
    AwsIotShadowDocumentInfo_t documentInfo = AWS_IOT_SHADOW_DOCUMENT_INFO_INITIALIZER;
    AwsIotShadowReference_t reference = AWS_IOT_SHADOW_REFERENCE_INITIALIZER;

    /* Set the members of the document info. */
    documentInfo.pThingName = _TEST_THING_NAME;
    documentInfo.thingNameLength = _TEST_THING_NAME_LENGTH;
    documentInfo.qos = IOT_MQTT_QOS_1;
    documentInfo.update.pUpdateDocument = "{\"state\":{\"reported\":{null}},\"clientToken\":\"TEST\"}";
    documentInfo.update.updateDocumentLength = 50;

    for( i = 0; ; i++ )
    {
        UnityMalloc_MakeMallocFailAfterCount( i );

        /* Call Shadow UPDATE. Memory allocation will fail at various times
         * during this call. */
        status = AwsIotShadow_Update( _pMqttConnection,
                                      &documentInfo,
                                      AWS_IOT_SHADOW_FLAG_WAITABLE,
                                      NULL,
                                      &reference );

        /* Once the Shadow UPDATE call succeeds, wait for it to complete. */
        if( status == AWS_IOT_SHADOW_STATUS_PENDING )
        {
            /* No response will be received from the network, so the Shadow UPDATE
             * is expected to time out. */
            TEST_ASSERT_EQUAL( AWS_IOT_SHADOW_TIMEOUT,
                               AwsIotShadow_Wait( reference,
                                                  0,
                                                  NULL,
                                                  NULL ) );
            break;
        }

        /* If the return value isn't success, check that it is memory allocation
         * failure. */
        TEST_ASSERT_EQUAL( AWS_IOT_SHADOW_NO_MEMORY, status );
    }
}

/*-----------------------------------------------------------*/
