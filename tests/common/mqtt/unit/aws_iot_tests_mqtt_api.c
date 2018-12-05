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
 * @file aws_iot_tests_mqtt_api.c
 * @brief Tests for the user-facing API functions (declared in aws_iot_mqtt.h).
 */

/* Build using a config header, if provided. */
#ifdef AWS_IOT_CONFIG_FILE
    #include AWS_IOT_CONFIG_FILE
#endif

/* Standard includes. */
#include <string.h>

/* POSIX includes. */
#ifdef POSIX_UNISTD_HEADER
    #include POSIX_UNISTD_HEADER
#else
    #include <unistd.h>
#endif

/* MQTT internal include. */
#include "private/aws_iot_mqtt_internal.h"

/* Platform layer includes. */
#include "platform/aws_iot_clock.h"

/* Test framework includes. */
#include "unity_fixture.h"

/* MQTT test access include. */
#include "aws_iot_test_access_mqtt.h"

/*-----------------------------------------------------------*/

/**
 * @brief Determine which MQTT server mode to test (AWS IoT or Mosquitto).
 */
#if !defined( AWS_IOT_TEST_MQTT_MOSQUITTO ) || AWS_IOT_TEST_MQTT_MOSQUITTO == 0
    #define _AWS_IOT_MQTT_SERVER    true
#else
    #define _AWS_IOT_MQTT_SERVER    false

    /* Redefine the connect info initializer if not using an AWS IoT MQTT server. */
    #undef AWS_IOT_MQTT_CONNECT_INFO_INITIALIZER
    #define AWS_IOT_MQTT_CONNECT_INFO_INITIALIZER    { 0 }
#endif

/**
 * @brief Timeout to use for the tests. This can be short, but should allow time
 * for other threads to run.
 */
#define _TIMEOUT_MS                  ( 400 )

/*
 * Client identifier and length to use for the MQTT API tests.
 */
#define _CLIENT_IDENTIFIER           ( "test" )                                            /**< @brief Client identifier. */
#define _CLIENT_IDENTIFIER_LENGTH    ( ( uint16_t ) ( sizeof( _CLIENT_IDENTIFIER ) - 1 ) ) /**< @brief Length of client identifier. */

/*
 * Will topic name and length to use for the MQTT API tests.
 */
#define _TEST_TOPIC_NAME             ( "/test/topic" )                                   /**< @brief An arbitrary topic name. */
#define _TEST_TOPIC_NAME_LENGTH      ( ( uint16_t ) ( sizeof( _TEST_TOPIC_NAME ) - 1 ) ) /**< @brief Length of topic name. */

/**
 * @brief A non-NULL function pointer to use for subscription callback. This
 * "function" should cause a crash if actually called.
 */
#define _SUBSCRIPTION_CALLBACK \
    ( ( void ( * )( void *,    \
                    AwsIotMqttCallbackParam_t * const ) ) 0x01 )

/**
 * @brief How many times #TEST_MQTT_Unit_API_DisconnectMallocFail_ will test
 * malloc failures.
 *
 * The DISCONNECT function provides no mechanism to wait on its successful
 * completion. Therefore, this function simply uses the value below as an estimate
 * for the maximum number of times DISCONNECT will use malloc.
 */
#define _DISCONNECT_MALLOC_LIMIT    ( 20 )

/*
 * Constants that affect the behavior of #TEST_MQTT_Unit_API_PublishDuplicates.
 */
#define _DUP_CHECK_RETRY_MS         ( 100 )  /**< @brief When to start sending duplicate packets. */
#define _DUP_CHECK_RETRY_LIMIT      ( 3 )    /**< @brief How many duplicate packets to send. */
#define _DUP_CHECK_TIMEOUT          ( 3000 ) /**< @brief Total time allowed to send all duplicate packets.
                                              * Duplicates are sent using an exponential backoff strategy. */
/** @brief The minimum amount of time the test can take. */
#define _DUP_CHECK_MINIMUM_WAIT \
    ( _DUP_CHECK_RETRY_MS +     \
      2 * _DUP_CHECK_RETRY_MS + \
      4 * _DUP_CHECK_RETRY_MS + \
      AWS_IOT_MQTT_RESPONSE_WAIT_MS )

/**
 * @brief Tracks whether #_publishSetDup has been called.
 */
static bool _publishSetDupCalled = false;

/*-----------------------------------------------------------*/

/**
 * @brief PUBLISH set DUP function override.
 */
static void _publishSetDup( bool awsIotMqttMode,
                            uint8_t * const pPublishPacket,
                            uint16_t * const pNewPacketIdentifier )
{
    _publishSetDupCalled = true;

    AwsIotMqttInternal_PublishSetDup( awsIotMqttMode,
                                      pPublishPacket,
                                      pNewPacketIdentifier );
}

/*-----------------------------------------------------------*/

/**
 * @brief A send function that always "succeeds".
 */
static size_t _sendSuccess( void * pSendContext,
                            const void * const pMessage,
                            size_t messageLength )
{
    ( void ) pSendContext;
    ( void ) pMessage;

    /* This function just returns the message length to simulate a successful
     * send. */
    return messageLength;
}

/*-----------------------------------------------------------*/

/**
 * @brief This send function checks that a duplicate outgoing message differs from
 * the original.
 */
static size_t _dupChecker( void * pSendContext,
                           const void * const pMessage,
                           size_t messageLength )
{
    static int runCount = 0;
    static bool status = true;
    bool * pDupCheckResult = ( bool * ) pSendContext;
    uint8_t publishFlags = *( ( uint8_t * ) pMessage );

    /* Declare the remaining variables required to check packet identifier
     * for the AWS IoT MQTT server. */
    #if _AWS_IOT_MQTT_SERVER == true
        static uint16_t lastPacketIdentifier = 0;
        uint16_t currentPacketIdentifier = 0;
        size_t bytesProcessed = 0;
        AwsIotMqttPublishInfo_t publishInfo = { 0 };
    #endif

    /* Ignore any MQTT packet that's not a PUBLISH. */
    if( ( publishFlags & 0xf0 ) != _MQTT_PACKET_TYPE_PUBLISH )
    {
        return messageLength;
    }

    runCount++;

    /* Check how many times this function has been called. */
    if( runCount == 1 )
    {
        #if _AWS_IOT_MQTT_SERVER == true
            /* Deserialize the PUBLISH to read the packet identifier. */
            if( AwsIotMqttInternal_DeserializePublish( pMessage,
                                                       messageLength,
                                                       &publishInfo,
                                                       &lastPacketIdentifier,
                                                       &bytesProcessed ) != AWS_IOT_MQTT_SUCCESS )
            {
                status = false;
            }
            else
            {
                /* Ensure that all bytes of the PUBLISH were processed. */
                status = ( bytesProcessed == messageLength );
            }
        #else /* if _AWS_IOT_MQTT_SERVER == true */
            /* DUP flag should not be set on this function's first run. */
            if( ( publishFlags & 0x08 ) == 0x08 )
            {
                status = false;
            }
        #endif /* if _AWS_IOT_MQTT_SERVER == true */
    }
    else
    {
        /* Only check the packet again if the previous run checks passed. */
        if( status == true )
        {
            #if _AWS_IOT_MQTT_SERVER == true
                /* Deserialize the PUBLISH to read the packet identifier. */
                if( AwsIotMqttInternal_DeserializePublish( pMessage,
                                                           messageLength,
                                                           &publishInfo,
                                                           &currentPacketIdentifier,
                                                           &bytesProcessed ) != AWS_IOT_MQTT_SUCCESS )
                {
                    status = false;
                }
                else
                {
                    /* Ensure that all bytes of the PUBLISH were processed. */
                    status = ( bytesProcessed == messageLength );

                    /* Check that the packet identifier is different. */
                    if( status == true )
                    {
                        status = ( currentPacketIdentifier != lastPacketIdentifier );
                        lastPacketIdentifier = currentPacketIdentifier;
                    }
                }
            #else /* if _AWS_IOT_MQTT_SERVER == true */
                /* DUP flag should be set when this function runs again. */
                if( ( publishFlags & 0x08 ) != 0x08 )
                {
                    status = false;
                }
            #endif /* if _AWS_IOT_MQTT_SERVER == true */
        }

        /* Write the check result on the last expected run of this function. */
        if( runCount == _DUP_CHECK_RETRY_LIMIT )
        {
            *pDupCheckResult = status;
        }
    }

    /* Return the message length to simulate a successful send. */
    return messageLength;
}

/*-----------------------------------------------------------*/

/**
 * @brief A disconnect function that only reports whether it was invoked.
 */
static void _disconnect( void * pDisconnectContext )
{
    bool * pDisconnectInvoked = ( bool * ) pDisconnectContext;

    /* This function just sets a flag testifying that it was invoked. */
    if( pDisconnectInvoked != NULL )
    {
        *pDisconnectInvoked = true;
    }
}

/*-----------------------------------------------------------*/

/**
 * @brief Test group for MQTT API tests.
 */
TEST_GROUP( MQTT_Unit_API );

/*-----------------------------------------------------------*/

/**
 * @brief Test setup for MQTT API tests.
 */
TEST_SETUP( MQTT_Unit_API )
{
    _publishSetDupCalled = false;
    TEST_ASSERT_EQUAL( AWS_IOT_MQTT_SUCCESS, AwsIotMqtt_Init() );
}

/*-----------------------------------------------------------*/

/**
 * @brief Test tear down for MQTT API tests.
 */
TEST_TEAR_DOWN( MQTT_Unit_API )
{
    AwsIotMqtt_Cleanup();
}

/*-----------------------------------------------------------*/

/**
 * @brief Test group runner for MQTT API tests.
 */
TEST_GROUP_RUNNER( MQTT_Unit_API )
{
    RUN_TEST_CASE( MQTT_Unit_API, ConnectParameters );
    RUN_TEST_CASE( MQTT_Unit_API, ConnectMallocFail );
    RUN_TEST_CASE( MQTT_Unit_API, ConnectRestoreSessionMallocFail );
    RUN_TEST_CASE( MQTT_Unit_API, DisconnectMallocFail );
    RUN_TEST_CASE( MQTT_Unit_API, PublishQoS0Parameters );
    RUN_TEST_CASE( MQTT_Unit_API, PublishQoS0MallocFail );
    RUN_TEST_CASE( MQTT_Unit_API, PublishQoS1 );
    RUN_TEST_CASE( MQTT_Unit_API, PublishDuplicates );
    RUN_TEST_CASE( MQTT_Unit_API, SubscribeUnsubscribeParameters );
    RUN_TEST_CASE( MQTT_Unit_API, SubscribeMallocFail );
    RUN_TEST_CASE( MQTT_Unit_API, UnsubscribeMallocFail );
}

/*-----------------------------------------------------------*/

/**
 * @brief Tests the behavior of @ref mqtt_function_connect with various
 * invalid parameters.
 */
TEST( MQTT_Unit_API, ConnectParameters )
{
    AwsIotMqttError_t status = AWS_IOT_MQTT_STATUS_PENDING;
    AwsIotMqttConnection_t mqttConnection = AWS_IOT_MQTT_CONNECTION_INITIALIZER;
    AwsIotMqttNetIf_t networkInterface = AWS_IOT_MQTT_NETIF_INITIALIZER;
    AwsIotMqttConnectInfo_t connectInfo = AWS_IOT_MQTT_CONNECT_INFO_INITIALIZER;
    AwsIotMqttPublishInfo_t willInfo = AWS_IOT_MQTT_PUBLISH_INFO_INITIALIZER;
    AwsIotMqttSubscription_t subscription = AWS_IOT_MQTT_SUBSCRIPTION_INITIALIZER;

    /* Check that the network interface is validated. */
    status = AwsIotMqtt_Connect( &mqttConnection,
                                 &networkInterface,
                                 &connectInfo,
                                 NULL,
                                 _TIMEOUT_MS );
    TEST_ASSERT_EQUAL( AWS_IOT_MQTT_BAD_PARAMETER, status );
    networkInterface.send = _sendSuccess;

    /* Check that the connection info is validated. */
    status = AwsIotMqtt_Connect( &mqttConnection,
                                 &networkInterface,
                                 &connectInfo,
                                 NULL,
                                 _TIMEOUT_MS );
    TEST_ASSERT_EQUAL( AWS_IOT_MQTT_BAD_PARAMETER, status );
    connectInfo.cleanSession = true;
    connectInfo.pClientIdentifier = _CLIENT_IDENTIFIER;
    connectInfo.clientIdentifierLength = _CLIENT_IDENTIFIER_LENGTH;

    /* AwsIotMqtt_ConnectRestoreSession with bad subscription. */
    connectInfo.cleanSession = false;
    status = AwsIotMqtt_ConnectRestoreSession( &mqttConnection,
                                               &networkInterface,
                                               &connectInfo,
                                               NULL,
                                               &subscription,
                                               1,
                                               _TIMEOUT_MS );
    TEST_ASSERT_EQUAL( AWS_IOT_MQTT_BAD_PARAMETER, status );

    /* AwsIotMqtt_ConnectRestoreSession with clean session. */
    connectInfo.cleanSession = true;
    subscription.pTopicFilter = _TEST_TOPIC_NAME;
    subscription.topicFilterLength = _TEST_TOPIC_NAME_LENGTH;
    subscription.callback.function = _SUBSCRIPTION_CALLBACK;
    status = AwsIotMqtt_ConnectRestoreSession( &mqttConnection,
                                               &networkInterface,
                                               &connectInfo,
                                               &willInfo,
                                               &subscription,
                                               1,
                                               _TIMEOUT_MS );
    TEST_ASSERT_EQUAL( AWS_IOT_MQTT_BAD_PARAMETER, status );

    /* Check that the will info is validated when it's provided. */
    status = AwsIotMqtt_Connect( &mqttConnection,
                                 &networkInterface,
                                 &connectInfo,
                                 &willInfo,
                                 _TIMEOUT_MS );
    TEST_ASSERT_EQUAL( AWS_IOT_MQTT_BAD_PARAMETER, status );
    willInfo.pTopicName = _TEST_TOPIC_NAME;
    willInfo.topicNameLength = _TEST_TOPIC_NAME_LENGTH;

    /* Check that a will message longer than 65535 is not allowed. */
    willInfo.pPayload = "";
    willInfo.payloadLength = 65536;
    status = AwsIotMqtt_Connect( &mqttConnection,
                                 &networkInterface,
                                 &connectInfo,
                                 &willInfo,
                                 _TIMEOUT_MS );
    TEST_ASSERT_EQUAL( AWS_IOT_MQTT_BAD_PARAMETER, status );
    willInfo.payloadLength = 0;

    /* Check that passing a wait time of 0 returns immediately. */
    status = AwsIotMqtt_Connect( &mqttConnection,
                                 &networkInterface,
                                 &connectInfo,
                                 &willInfo,
                                 0 );
    TEST_ASSERT_EQUAL( AWS_IOT_MQTT_TIMEOUT, status );
}

/*-----------------------------------------------------------*/

/**
 * @brief Tests the behavior of @ref mqtt_function_connect when memory
 * allocation fails at various points.
 */
TEST( MQTT_Unit_API, ConnectMallocFail )
{
    int i = 0;
    AwsIotMqttError_t status = AWS_IOT_MQTT_STATUS_PENDING;
    AwsIotMqttConnection_t mqttConnection = AWS_IOT_MQTT_CONNECTION_INITIALIZER;
    AwsIotMqttNetIf_t networkInterface = AWS_IOT_MQTT_NETIF_INITIALIZER;
    AwsIotMqttConnectInfo_t connectInfo = AWS_IOT_MQTT_CONNECT_INFO_INITIALIZER;

    /* Initialize parameters. */
    networkInterface.send = _sendSuccess;
    connectInfo.keepAliveSeconds = 100;
    connectInfo.cleanSession = true;
    connectInfo.pClientIdentifier = _CLIENT_IDENTIFIER;
    connectInfo.clientIdentifierLength = _CLIENT_IDENTIFIER_LENGTH;

    for( i = 0; ; i++ )
    {
        UnityMalloc_MakeMallocFailAfterCount( i );

        /* Call CONNECT. Memory allocation will fail at various times during
         * this call. */
        status = AwsIotMqtt_Connect( &mqttConnection,
                                     &networkInterface,
                                     &connectInfo,
                                     NULL,
                                     _TIMEOUT_MS );

        /* If the return value is timeout, then all memory allocation succeeded
         * and the loop can exit. The expected return value is timeout (and not
         * success) because the receive callback is never invoked. */
        if( status == AWS_IOT_MQTT_TIMEOUT )
        {
            break;
        }

        /* If the return value isn't timeout, check that it is memory allocation
         * failure. */
        TEST_ASSERT_EQUAL( AWS_IOT_MQTT_NO_MEMORY, status );
    }
}

/*-----------------------------------------------------------*/

/**
 * @brief Tests the behavior of @ref mqtt_function_connectrestoresession
 * when memory allocation fails at various points.
 */
TEST( MQTT_Unit_API, ConnectRestoreSessionMallocFail )
{
    int i = 0;
    AwsIotMqttError_t status = AWS_IOT_MQTT_STATUS_PENDING;
    AwsIotMqttConnection_t mqttConnection = AWS_IOT_MQTT_CONNECTION_INITIALIZER;
    AwsIotMqttNetIf_t networkInterface = AWS_IOT_MQTT_NETIF_INITIALIZER;
    AwsIotMqttConnectInfo_t connectInfo = AWS_IOT_MQTT_CONNECT_INFO_INITIALIZER;
    AwsIotMqttSubscription_t subscription = AWS_IOT_MQTT_SUBSCRIPTION_INITIALIZER;

    /* Initialize parameters. */
    networkInterface.send = _sendSuccess;
    connectInfo.cleanSession = false;
    connectInfo.keepAliveSeconds = 100;
    connectInfo.pClientIdentifier = _CLIENT_IDENTIFIER;
    connectInfo.clientIdentifierLength = _CLIENT_IDENTIFIER_LENGTH;
    subscription.pTopicFilter = _TEST_TOPIC_NAME;
    subscription.topicFilterLength = _TEST_TOPIC_NAME_LENGTH;
    subscription.callback.function = _SUBSCRIPTION_CALLBACK;

    for( i = 0; ; i++ )
    {
        UnityMalloc_MakeMallocFailAfterCount( i );

        /* Call CONNECT with a previous session. Memory allocation will fail at
         * various times during this call. */
        status = AwsIotMqtt_ConnectRestoreSession( &mqttConnection,
                                                   &networkInterface,
                                                   &connectInfo,
                                                   NULL,
                                                   &subscription,
                                                   1,
                                                   _TIMEOUT_MS );

        /* If the return value is timeout, then all memory allocation succeeded
         * and the loop can exit. The expected return value is timeout (and not
         * success) because the receive callback is never invoked. */
        if( status == AWS_IOT_MQTT_TIMEOUT )
        {
            break;
        }

        /* If the return value isn't timeout, check that it is memory allocation
         * failure. */
        TEST_ASSERT_EQUAL( AWS_IOT_MQTT_NO_MEMORY, status );
    }
}

/*-----------------------------------------------------------*/

/**
 * @brief Tests the behavior of @ref mqtt_function_disconnect when memory
 * allocation fails at various points.
 */
TEST( MQTT_Unit_API, DisconnectMallocFail )
{
    int i = 0;
    bool disconnectInvoked = false;
    AwsIotMqttNetIf_t networkInterface = AWS_IOT_MQTT_NETIF_INITIALIZER;
    _mqttConnection_t * pMqttConnection = NULL;

    /* Set the members of the network interface. */
    networkInterface.pDisconnectContext = &disconnectInvoked;
    networkInterface.send = _sendSuccess;
    networkInterface.disconnect = _disconnect;

    for( i = 0; i < _DISCONNECT_MALLOC_LIMIT; i++ )
    {
        /* Allow unlimited use of malloc during connection initialization. */
        UnityMalloc_MakeMallocFailAfterCount( -1 );

        /* Create a new MQTT connection. */
        pMqttConnection = AwsIotTestMqtt_createMqttConnection( _AWS_IOT_MQTT_SERVER,
                                                                &networkInterface,
                                                                0 );
        TEST_ASSERT_NOT_EQUAL( NULL, pMqttConnection );

        /* Set malloc to eventually fail. */
        UnityMalloc_MakeMallocFailAfterCount( i );

        /* Call DISCONNECT; this function should always perform cleanup regardless
         * of memory allocation errors. */
        AwsIotMqtt_Disconnect( pMqttConnection, false );
        TEST_ASSERT_EQUAL_INT( true, disconnectInvoked );
    }
}

/*-----------------------------------------------------------*/

/**
 * @brief Tests the behavior of @ref mqtt_function_publish (QoS 0) with various
 * valid and invalid parameters.
 */
TEST( MQTT_Unit_API, PublishQoS0Parameters )
{
    AwsIotMqttError_t status = AWS_IOT_MQTT_STATUS_PENDING;
    _mqttConnection_t mqttConnection = { 0 };
    AwsIotMqttPublishInfo_t publishInfo = AWS_IOT_MQTT_PUBLISH_INFO_INITIALIZER;
    AwsIotMqttReference_t publishReference = AWS_IOT_MQTT_REFERENCE_INITIALIZER;
    AwsIotMqttCallbackInfo_t callbackInfo = AWS_IOT_MQTT_CALLBACK_INFO_INITIALIZER;

    /* Set a send function for the MQTT connection. */
    mqttConnection.network.send = _sendSuccess;

    /* Check that the publish info is validated. */
    status = AwsIotMqtt_Publish( &mqttConnection, &publishInfo, 0, NULL, NULL );
    TEST_ASSERT_EQUAL( AWS_IOT_MQTT_BAD_PARAMETER, status );
    publishInfo.pTopicName = _TEST_TOPIC_NAME;
    publishInfo.topicNameLength = _TEST_TOPIC_NAME_LENGTH;

    /* Check that a QoS 0 publish is refused if a notification is requested. */
    status = AwsIotMqtt_Publish( &mqttConnection, &publishInfo, AWS_IOT_MQTT_FLAG_WAITABLE, NULL, &publishReference );
    TEST_ASSERT_EQUAL( AWS_IOT_MQTT_BAD_PARAMETER, status );
    status = AwsIotMqtt_Publish( &mqttConnection, &publishInfo, 0, &callbackInfo, NULL );
    TEST_ASSERT_EQUAL( AWS_IOT_MQTT_BAD_PARAMETER, status );

    /* If valid parameters are passed, QoS 0 publish should always return success. */
    status = AwsIotMqtt_Publish( &mqttConnection, &publishInfo, 0, 0, &publishReference );
    TEST_ASSERT_EQUAL( AWS_IOT_MQTT_SUCCESS, status );

    /* Allow time for the send thread to run and clean up the PUBLISH. QoS 0
     * PUBLISH provides no mechanism to wait on completion, so sleep is used. */
    sleep( 1 );
}

/*-----------------------------------------------------------*/

/**
 * @brief Tests the behavior of @ref mqtt_function_publish (QoS 0) when memory
 * allocation fails at various points.
 */
TEST( MQTT_Unit_API, PublishQoS0MallocFail )
{
    int i = 0;
    AwsIotMqttError_t status = AWS_IOT_MQTT_STATUS_PENDING;
    _mqttConnection_t mqttConnection = { 0 };
    AwsIotMqttPublishInfo_t publishInfo = AWS_IOT_MQTT_PUBLISH_INFO_INITIALIZER;

    /* Set a send function for the MQTT connection; also set the necessary
     * members of publish info. */
    mqttConnection.network.send = _sendSuccess;
    publishInfo.pTopicName = _TEST_TOPIC_NAME;
    publishInfo.topicNameLength = _TEST_TOPIC_NAME_LENGTH;

    for( i = 0; ; i++ )
    {
        UnityMalloc_MakeMallocFailAfterCount( i );

        /* Call PUBLISH. Memory allocation will fail at various times during
         * this call. */
        status = AwsIotMqtt_Publish( &mqttConnection, &publishInfo, 0, NULL, NULL );

        /* Once PUBLISH succeeds, the loop can exit. */
        if( status == AWS_IOT_MQTT_SUCCESS )
        {
            break;
        }

        /* If the return value isn't success, check that it is memory allocation
         * failure. */
        TEST_ASSERT_EQUAL( AWS_IOT_MQTT_NO_MEMORY, status );
    }

    /* Wait for any pending QoS 0 publishes to clean up. */
    sleep( 1 );
}

/*-----------------------------------------------------------*/

/**
 * @brief Tests the behavior of @ref mqtt_function_publish (QoS 1) with various
 * invalid parameters. Also tests the behavior of @ref mqtt_function_publish
 * (QoS 1) when memory allocation fails at various points.
 */
TEST( MQTT_Unit_API, PublishQoS1 )
{
    int i = 0;
    AwsIotMqttError_t status = AWS_IOT_MQTT_STATUS_PENDING;
    _mqttConnection_t * pMqttConnection = NULL;
    AwsIotMqttNetIf_t networkInterface = AWS_IOT_MQTT_NETIF_INITIALIZER;
    AwsIotMqttPublishInfo_t publishInfo = AWS_IOT_MQTT_PUBLISH_INFO_INITIALIZER;
    AwsIotMqttReference_t publishRef = AWS_IOT_MQTT_REFERENCE_INITIALIZER;
    AwsIotMqttCallbackInfo_t callbackInfo = AWS_IOT_MQTT_CALLBACK_INFO_INITIALIZER;

    /* Set the members of the network interface and create a new MQTT connection. */
    networkInterface.send = _sendSuccess;
    pMqttConnection = AwsIotTestMqtt_createMqttConnection( _AWS_IOT_MQTT_SERVER,
                                                            &networkInterface,
                                                            0 );
    TEST_ASSERT_NOT_EQUAL( NULL, pMqttConnection );

    /* Set the publish info. */
    publishInfo.QoS = 1;
    publishInfo.pTopicName = _TEST_TOPIC_NAME;
    publishInfo.topicNameLength = _TEST_TOPIC_NAME_LENGTH;

    if( TEST_PROTECT() )
    {
        /* Setting the waitable flag with no reference is not allowed. */
        status = AwsIotMqtt_Publish( pMqttConnection,
                                     &publishInfo,
                                     AWS_IOT_MQTT_FLAG_WAITABLE,
                                     NULL,
                                     NULL );
        TEST_ASSERT_EQUAL( AWS_IOT_MQTT_BAD_PARAMETER, status );

        /* Setting both the waitable flag and callback info is not allowed. */
        status = AwsIotMqtt_Publish( pMqttConnection,
                                     &publishInfo,
                                     AWS_IOT_MQTT_FLAG_WAITABLE,
                                     &callbackInfo,
                                     &publishRef );
        TEST_ASSERT_EQUAL( AWS_IOT_MQTT_BAD_PARAMETER, status );

        /* Check QoS 1 PUBLISH behavior with malloc failures. */
        for( i = 0; ; i++ )
        {
            UnityMalloc_MakeMallocFailAfterCount( i );

            /* Call PUBLISH. Memory allocation will fail at various times during
             * this call. */
            status = AwsIotMqtt_Publish( pMqttConnection,
                                         &publishInfo,
                                         AWS_IOT_MQTT_FLAG_WAITABLE,
                                         NULL,
                                         &publishRef );

            /* If the PUBLISH succeeded, the loop can exit after waiting for the QoS
             * 1 PUBLISH to be cleaned up. */
            if( status == AWS_IOT_MQTT_STATUS_PENDING )
            {
                TEST_ASSERT_EQUAL( AWS_IOT_MQTT_TIMEOUT, AwsIotMqtt_Wait( publishRef, _TIMEOUT_MS ) );
                break;
            }

            /* If the return value isn't success, check that it is memory allocation
             * failure. */
            TEST_ASSERT_EQUAL( AWS_IOT_MQTT_NO_MEMORY, status );
        }
    }

    /* Clean up MQTT connection. */
    AwsIotTestMqtt_destroyMqttConnection( pMqttConnection );
}

/*-----------------------------------------------------------*/

/**
 * @brief Tests that duplicate QoS 1 PUBLISH packets are different from the
 * original.
 *
 * For non-AWS IoT MQTT servers, checks that the DUP flag is set. For
 * AWS IoT MQTT servers, checks that the packet identifier is different.
 */
TEST( MQTT_Unit_API, PublishDuplicates )
{
    AwsIotMqttNetIf_t networkInterface = AWS_IOT_MQTT_NETIF_INITIALIZER;
    _mqttConnection_t * pMqttConnection = NULL;
    AwsIotMqttPublishInfo_t publishInfo = AWS_IOT_MQTT_PUBLISH_INFO_INITIALIZER;
    AwsIotMqttReference_t publishRef = AWS_IOT_MQTT_REFERENCE_INITIALIZER;
    volatile bool dupCheckResult = false;
    uint64_t startTime = 0;

    /* Set the members of the network interface and create a new MQTT connection. */
    networkInterface.pSendContext = ( void * ) &dupCheckResult;
    networkInterface.send = _dupChecker;
    networkInterface.serialize.publishSetDup = _publishSetDup;
    pMqttConnection = AwsIotTestMqtt_createMqttConnection( _AWS_IOT_MQTT_SERVER,
                                                            &networkInterface,
                                                            0 );
    TEST_ASSERT_NOT_EQUAL( NULL, pMqttConnection );

    /* Set the publish info. */
    publishInfo.QoS = 1;
    publishInfo.pTopicName = _TEST_TOPIC_NAME;
    publishInfo.topicNameLength = _TEST_TOPIC_NAME_LENGTH;
    publishInfo.pPayload = "test";
    publishInfo.payloadLength = 4;
    publishInfo.retryMs = _DUP_CHECK_RETRY_MS;
    publishInfo.retryLimit = _DUP_CHECK_RETRY_LIMIT;

    startTime = AwsIotClock_GetTimeMs();

    if( TEST_PROTECT() )
    {
        /* Send a PUBLISH with retransmissions enabled. */
        TEST_ASSERT_EQUAL( AWS_IOT_MQTT_STATUS_PENDING,
                           AwsIotMqtt_Publish( pMqttConnection,
                                               &publishInfo,
                                               AWS_IOT_MQTT_FLAG_WAITABLE,
                                               NULL,
                                               &publishRef ) );

        /* Since _dupChecker doesn't actually transmit a PUBLISH, no PUBACK is
         * expected. */
        TEST_ASSERT_EQUAL( AWS_IOT_MQTT_RETRY_NO_RESPONSE,
                           AwsIotMqtt_Wait( publishRef, _DUP_CHECK_TIMEOUT ) );

        /* Check the result of the DUP check. */
        TEST_ASSERT_EQUAL_INT( true, dupCheckResult );

        /* Check that at least the minimum wait time elapsed. */
        TEST_ASSERT_TRUE( startTime + _DUP_CHECK_MINIMUM_WAIT <= AwsIotClock_GetTimeMs() );
    }

    /* Clean up MQTT connection. */
    AwsIotTestMqtt_destroyMqttConnection( pMqttConnection );

    /* Check that the set DUP override was called. */
    if( TEST_PROTECT() )
    {
        TEST_ASSERT_EQUAL_INT( true, _publishSetDupCalled );
    }
}

/*-----------------------------------------------------------*/

/**
 * @brief Tests the behavior of @ref mqtt_function_subscribe and
 * @ref mqtt_function_unsubscribe with various invalid parameters.
 */
TEST( MQTT_Unit_API, SubscribeUnsubscribeParameters )
{
    AwsIotMqttError_t status = AWS_IOT_MQTT_STATUS_PENDING;
    _mqttConnection_t mqttConnection = { 0 };
    AwsIotMqttSubscription_t subscription = AWS_IOT_MQTT_SUBSCRIPTION_INITIALIZER;
    AwsIotMqttReference_t reference = AWS_IOT_MQTT_REFERENCE_INITIALIZER;

    /* Check that subscription info is validated. */
    status = AwsIotMqtt_Subscribe( &mqttConnection,
                                   &subscription,
                                   1,
                                   AWS_IOT_MQTT_FLAG_WAITABLE,
                                   NULL,
                                   &reference );
    TEST_ASSERT_EQUAL( AWS_IOT_MQTT_BAD_PARAMETER, status );

    status = AwsIotMqtt_Unsubscribe( &mqttConnection,
                                     &subscription,
                                     1,
                                     AWS_IOT_MQTT_FLAG_WAITABLE,
                                     NULL,
                                     &reference );
    TEST_ASSERT_EQUAL( AWS_IOT_MQTT_BAD_PARAMETER, status );

    subscription.pTopicFilter = _TEST_TOPIC_NAME;
    subscription.topicFilterLength = _TEST_TOPIC_NAME_LENGTH;
    subscription.callback.function = _SUBSCRIPTION_CALLBACK;

    /* A reference must be provided for a waitable SUBSCRIBE. */
    status = AwsIotMqtt_Subscribe( &mqttConnection,
                                   &subscription,
                                   1,
                                   AWS_IOT_MQTT_FLAG_WAITABLE,
                                   NULL,
                                   NULL );
    TEST_ASSERT_EQUAL( AWS_IOT_MQTT_BAD_PARAMETER, status );

    status = AwsIotMqtt_Unsubscribe( &mqttConnection,
                                     &subscription,
                                     1,
                                     AWS_IOT_MQTT_FLAG_WAITABLE,
                                     NULL,
                                     NULL );
    TEST_ASSERT_EQUAL( AWS_IOT_MQTT_BAD_PARAMETER, status );
}

/*-----------------------------------------------------------*/

/**
 * @brief Tests the behavior of @ref mqtt_function_subscribe when memory allocation
 * fails at various points.
 */
TEST( MQTT_Unit_API, SubscribeMallocFail )
{
    int i = 0;
    AwsIotMqttError_t status = AWS_IOT_MQTT_STATUS_PENDING;
    _mqttConnection_t mqttConnection = { 0 };
    AwsIotMqttSubscription_t subscription = AWS_IOT_MQTT_SUBSCRIPTION_INITIALIZER;
    AwsIotMqttReference_t subscribeRef = AWS_IOT_MQTT_REFERENCE_INITIALIZER;

    /* Set the necessary members of the MQTT connection and subscription. */
    mqttConnection.network.send = _sendSuccess;
    subscription.pTopicFilter = _TEST_TOPIC_NAME;
    subscription.topicFilterLength = _TEST_TOPIC_NAME_LENGTH;
    subscription.callback.function = _SUBSCRIPTION_CALLBACK;

    TEST_ASSERT_EQUAL_INT( true,
                           AwsIotList_Create( &( mqttConnection.subscriptionList ) ) );

    if( TEST_PROTECT() )
    {
        for( i = 0; ; i++ )
        {
            UnityMalloc_MakeMallocFailAfterCount( i );

            /* Call SUBSCRIBE. Memory allocation will fail at various times during
             * this call. */
            status = AwsIotMqtt_Subscribe( &mqttConnection,
                                           &subscription,
                                           1,
                                           AWS_IOT_MQTT_FLAG_WAITABLE,
                                           NULL,
                                           &subscribeRef );

            /* If the SUBSCRIBE succeeded, the loop can exit after waiting for
             * the SUBSCRIBE to be cleaned up. */
            if( status == AWS_IOT_MQTT_STATUS_PENDING )
            {
                TEST_ASSERT_EQUAL( AWS_IOT_MQTT_TIMEOUT, AwsIotMqtt_Wait( subscribeRef, _TIMEOUT_MS ) );
                break;
            }

            /* If the return value isn't success, check that it is memory allocation
             * failure. */
            TEST_ASSERT_EQUAL( AWS_IOT_MQTT_NO_MEMORY, status );
        }

        /* No lingering subscriptions should be in the MQTT connection. */
        TEST_ASSERT_EQUAL_PTR( NULL, mqttConnection.subscriptionList.pHead );
    }

    AwsIotList_RemoveAllMatches( &( mqttConnection.subscriptionList ),
                                 _SUBSCRIPTION_LINK_OFFSET,
                                 NULL,
                                 NULL,
                                 AwsIotMqtt_FreeSubscription );
    AwsIotList_Destroy( &( mqttConnection.subscriptionList ) );
}

/*-----------------------------------------------------------*/

/**
 * @brief Tests the behavior of @ref mqtt_function_unsubscribe when memory
 * allocation fails at various points.
 */
TEST( MQTT_Unit_API, UnsubscribeMallocFail )
{
    int i = 0;
    AwsIotMqttError_t status = AWS_IOT_MQTT_STATUS_PENDING;
    _mqttConnection_t mqttConnection = { 0 };
    AwsIotMqttSubscription_t subscription = AWS_IOT_MQTT_SUBSCRIPTION_INITIALIZER;
    AwsIotMqttReference_t unsubscribeRef = AWS_IOT_MQTT_REFERENCE_INITIALIZER;

    /* Set the necessary members of the MQTT connection and subscription. */
    mqttConnection.network.send = _sendSuccess;
    subscription.pTopicFilter = _TEST_TOPIC_NAME;
    subscription.topicFilterLength = _TEST_TOPIC_NAME_LENGTH;
    subscription.callback.function = _SUBSCRIPTION_CALLBACK;

    TEST_ASSERT_EQUAL_INT( true,
                           AwsIotList_Create( &( mqttConnection.subscriptionList ) ) );

    if( TEST_PROTECT() )
    {
        for( i = 0; ; i++ )
        {
            UnityMalloc_MakeMallocFailAfterCount( i );

            /* Call UNSUBSCRIBE. Memory allocation will fail at various times during
             * this call. */
            status = AwsIotMqtt_Unsubscribe( &mqttConnection,
                                             &subscription,
                                             1,
                                             AWS_IOT_MQTT_FLAG_WAITABLE,
                                             NULL,
                                             &unsubscribeRef );

            /* If the UNSUBSCRIBE succeeded, the loop can exit after waiting for
             * the UNSUBSCRIBE to be cleaned up. */
            if( status == AWS_IOT_MQTT_STATUS_PENDING )
            {
                TEST_ASSERT_EQUAL( AWS_IOT_MQTT_TIMEOUT, AwsIotMqtt_Wait( unsubscribeRef, _TIMEOUT_MS ) );
                break;
            }

            /* If the return value isn't success, check that it is memory allocation
             * failure. */
            TEST_ASSERT_EQUAL( AWS_IOT_MQTT_NO_MEMORY, status );
        }

        /* No lingering subscriptions should be in the MQTT connection. */
        TEST_ASSERT_EQUAL_PTR( NULL, mqttConnection.subscriptionList.pHead );
    }

    AwsIotList_RemoveAllMatches( &( mqttConnection.subscriptionList ),
                                 _SUBSCRIPTION_LINK_OFFSET,
                                 NULL,
                                 NULL,
                                 AwsIotMqtt_FreeSubscription );
    AwsIotList_Destroy( &( mqttConnection.subscriptionList ) );
}

/*-----------------------------------------------------------*/
