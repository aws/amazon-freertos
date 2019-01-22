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
 * @file aws_iot_tests_mqtt_system.c
 * @brief Full system tests for the AWS IoT MQTT library.
 */

/* Build using a config header, if provided. */
#ifdef AWS_IOT_CONFIG_FILE
    #include AWS_IOT_CONFIG_FILE
#endif

/* Standard includes. */
#include <string.h>

/* MQTT internal include. */
#include "private/aws_iot_mqtt_internal.h"

/* Platform layer includes. */
#include "platform/aws_iot_clock.h"

/* Test framework includes. */
#include "unity_fixture.h"

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
#ifndef AWS_IOT_TEST_MQTT_TIMEOUT_MS
    #define AWS_IOT_TEST_MQTT_TIMEOUT_MS      ( 5000 )
#endif
#ifndef AWS_IOT_TEST_MQTT_TOPIC_PREFIX
    #define AWS_IOT_TEST_MQTT_TOPIC_PREFIX    "awsiotmqtttest"
#endif
/** @endcond */

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
 * @brief The maximum length of an MQTT client identifier.
 *
 * When @ref AWS_IOT_TEST_MQTT_CLIENT_IDENTIFIER is defined, this value must
 * accommodate the length of @ref AWS_IOT_TEST_MQTT_CLIENT_IDENTIFIER plus 4
 * to accommodate the Last Will and Testament test. Otherwise, this value is
 * set to 24, which is the longest client identifier length an MQTT server is
 * obligated to accept plus a NULL terminator.
 */
#ifdef AWS_IOT_TEST_MQTT_CLIENT_IDENTIFIER
    #define _CLIENT_IDENTIFIER_MAX_LENGTH    ( sizeof( AWS_IOT_TEST_MQTT_CLIENT_IDENTIFIER ) + 4 )
#else
    #define _CLIENT_IDENTIFIER_MAX_LENGTH    ( 24 )
#endif

/*-----------------------------------------------------------*/

/**
 * @brief Parameter 1 of #_operationComplete.
 */
typedef struct _operationCompleteParams
{
    AwsIotMqttOperationType_t expectedOperation; /**< @brief Expected completed operation. */
    AwsIotSemaphore_t waitSem;                   /**< @brief Used to unblock waiting test thread. */
    AwsIotMqttReference_t reference;             /**< @brief Reference to expected completed operation. */
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
 * @brief Filler text to publish.
 */
static const char _pSamplePayload[] =
    "Lorem ipsum dolor sit amet, consectetur adipiscing elit, sed do eiusmod tempor"
    " incididunt ut labore et dolore magna aliqua. Ut enim ad minim veniam, quis "
    "nostrud exercitation ullamco laboris nisi ut aliquip ex ea commodo consequat. "
    "Duis aute irure dolor in reprehenderit in voluptate velit esse cillum dolore eu"
    " fugiat nulla pariatur. Excepteur sint occaecat cupidatat non proident, sunt in"
    " culpa qui officia deserunt mollit anim id est laborum.";

/**
 * @brief Length of #_pSamplePayload.
 */
static const size_t _samplePayloadLength = sizeof( _pSamplePayload ) - 1;

/**
 * @brief Buffer holding the client identifier used for the tests.
 */
static char _pClientIdentifier[ _CLIENT_IDENTIFIER_MAX_LENGTH ] = { 0 };

/*
 * Track if the serializer overrides were called for a test.
 */
static bool _freePacketOverride = false;            /**< @brief Tracks if #_freePacket was called. */
static bool _connectSerializerOverride = false;     /**< @brief Tracks if #_connectSerializerOverride was called. */
static bool _publishSerializerOverride = false;     /**< @brief Tracks if #_publishSerializerOverride was called. */
static bool _pubackSerializerOverride = false;      /**< @brief Tracks if #_pubackSerializerOverride was called. */
static bool _subscribeSerializerOverride = false;   /**< @brief Tracks if #_subscribeSerializerOverride was called. */
static bool _unsubscribeSerializerOverride = false; /**< @brief Tracks if #_unsubscribeSerializerOverride was called. */
static bool _disconnectSerializerOverride = false;  /**< @brief Tracks if #_disconnectSerializerOverride was called. */

/*-----------------------------------------------------------*/

/**
 * @brief Packet free function override
 */
static void _freePacket( uint8_t * pPacket )
{
    _freePacketOverride = true;

    AwsIotMqttInternal_FreePacket( pPacket );
}

/*-----------------------------------------------------------*/

/**
 * @brief Serializer override for CONNECT.
 */
static AwsIotMqttError_t _serializeConnect( const AwsIotMqttConnectInfo_t * const pConnectInfo,
                                            const AwsIotMqttPublishInfo_t * const pWillInfo,
                                            uint8_t ** const pConnectPacket,
                                            size_t * const pPacketSize )
{
    _connectSerializerOverride = true;

    return AwsIotMqttInternal_SerializeConnect( pConnectInfo,
                                                pWillInfo,
                                                pConnectPacket,
                                                pPacketSize );
}

/*-----------------------------------------------------------*/

/**
 * @brief Serializer override for PUBLISH.
 */
static AwsIotMqttError_t _serializePublish( const AwsIotMqttPublishInfo_t * const pPublishInfo,
                                            uint8_t ** const pPublishPacket,
                                            size_t * const pPacketSize,
                                            uint16_t * const pPacketIdentifier )
{
    _publishSerializerOverride = true;

    return AwsIotMqttInternal_SerializePublish( pPublishInfo,
                                                pPublishPacket,
                                                pPacketSize,
                                                pPacketIdentifier );
}

/*-----------------------------------------------------------*/

/**
 * @brief Serializer override for PUBACK.
 */
static AwsIotMqttError_t _serializePuback( uint16_t packetIdentifier,
                                           uint8_t ** const pPubackPacket,
                                           size_t * const pPacketSize )
{
    _pubackSerializerOverride = true;

    return AwsIotMqttInternal_SerializePuback( packetIdentifier,
                                               pPubackPacket,
                                               pPacketSize );
}

/*-----------------------------------------------------------*/

/**
 * @brief Serializer override for SUBSCRIBE.
 */
static AwsIotMqttError_t _serializeSubscribe( const AwsIotMqttSubscription_t * const pSubscriptionList,
                                              size_t subscriptionCount,
                                              uint8_t ** const pSubscribePacket,
                                              size_t * const pPacketSize,
                                              uint16_t * const pPacketIdentifier )
{
    _subscribeSerializerOverride = true;

    return AwsIotMqttInternal_SerializeSubscribe( pSubscriptionList,
                                                  subscriptionCount,
                                                  pSubscribePacket,
                                                  pPacketSize,
                                                  pPacketIdentifier );
}

/*-----------------------------------------------------------*/

/**
 * @brief Serializer override for UNSUBSCRIBE.
 */
static AwsIotMqttError_t _serializeUnsubscribe( const AwsIotMqttSubscription_t * const pSubscriptionList,
                                                size_t subscriptionCount,
                                                uint8_t ** const pSubscribePacket,
                                                size_t * const pPacketSize,
                                                uint16_t * const pPacketIdentifier )
{
    _unsubscribeSerializerOverride = true;

    return AwsIotMqttInternal_SerializeUnsubscribe( pSubscriptionList,
                                                    subscriptionCount,
                                                    pSubscribePacket,
                                                    pPacketSize,
                                                    pPacketIdentifier );
}

/*-----------------------------------------------------------*/

/**
 * @brief Serializer override for DISCONNECT.
 */
static AwsIotMqttError_t _serializeDisconnect( uint8_t ** const pDisconnectPacket,
                                               size_t * const pPacketSize )
{
    _disconnectSerializerOverride = true;

    return AwsIotMqttInternal_SerializeDisconnect( pDisconnectPacket,
                                                   pPacketSize );
}

/*-----------------------------------------------------------*/

/**
 * @brief Subscription callback function. Checks for valid parameters and unblocks
 * the main test thread.
 */
static void _publishReceived( void * pArgument,
                              AwsIotMqttCallbackParam_t * const pPublish )
{
    AwsIotSemaphore_t * pWaitSem = ( AwsIotSemaphore_t * ) pArgument;

    /* If the received messages matches what was sent, unblock the waiting thread. */
    if( ( pPublish->message.info.payloadLength == _samplePayloadLength ) &&
        ( strncmp( pPublish->message.info.pPayload,
                   _pSamplePayload,
                   _samplePayloadLength ) == 0 ) )
    {
        AwsIotSemaphore_Post( pWaitSem );
    }
}

/*-----------------------------------------------------------*/

/**
 * @brief Operation completion callback function. Checks for valid parameters
 * and unblocks the main test thread.
 */
static void _operationComplete( void * pArgument,
                                AwsIotMqttCallbackParam_t * const pOperation )
{
    _operationCompleteParams_t * pParams = ( _operationCompleteParams_t * ) pArgument;

    /* If the operation information matches the parameters and the operation was
     * successful, unblock the waiting thread. */
    if( ( pParams->expectedOperation == pOperation->operation.type ) &&
        ( pParams->reference == pOperation->operation.reference ) &&
        ( pOperation->operation.result == AWS_IOT_MQTT_SUCCESS ) )
    {
        AwsIotSemaphore_Post( &( pParams->waitSem ) );
    }
}

/*-----------------------------------------------------------*/

/**
 * @brief Run the subscribe-publish-wait tests at various QoS.
 */
static void _subscribePublishWait( int QoS )
{
    AwsIotMqttError_t status = AWS_IOT_MQTT_STATUS_PENDING;
    AwsIotMqttNetIf_t networkInterface = _AwsIotTestNetworkInterface;
    AwsIotMqttConnectInfo_t connectInfo = AWS_IOT_MQTT_CONNECT_INFO_INITIALIZER;
    AwsIotMqttSubscription_t subscription = AWS_IOT_MQTT_SUBSCRIPTION_INITIALIZER;
    AwsIotMqttPublishInfo_t publishInfo = AWS_IOT_MQTT_PUBLISH_INFO_INITIALIZER;
    AwsIotSemaphore_t waitSem;

    /* Set the serializer overrides. */
    networkInterface.freePacket = _freePacket;
    networkInterface.serialize.connect = _serializeConnect;
    networkInterface.serialize.publish = _serializePublish;
    networkInterface.serialize.puback = _serializePuback;
    networkInterface.serialize.subscribe = _serializeSubscribe;
    networkInterface.serialize.unsubscribe = _serializeUnsubscribe;
    networkInterface.serialize.disconnect = _serializeDisconnect;

    /* Create the wait semaphore. */
    TEST_ASSERT_EQUAL_INT( true, AwsIotSemaphore_Create( &waitSem, 0, 1 ) );

    if( TEST_PROTECT() )
    {
        /* Set the members of the MQTT connection info. */
        connectInfo.cleanSession = true;
        connectInfo.pClientIdentifier = _pClientIdentifier;
        connectInfo.clientIdentifierLength = ( uint16_t ) strlen( _pClientIdentifier );

        /* Establish the MQTT connection. */
        status = AwsIotMqtt_Connect( &_AwsIotTestMqttConnection,
                                     &networkInterface,
                                     &connectInfo,
                                     NULL,
                                     AWS_IOT_TEST_MQTT_TIMEOUT_MS );
        TEST_ASSERT_EQUAL( AWS_IOT_MQTT_SUCCESS, status );

        if( TEST_PROTECT() )
        {
            /* Set the members of the subscription. */
            subscription.QoS = QoS;
            subscription.pTopicFilter = AWS_IOT_TEST_MQTT_TOPIC_PREFIX "/SubscribePublishWait";
            subscription.topicFilterLength = ( uint16_t ) strlen( subscription.pTopicFilter );
            subscription.callback.function = _publishReceived;
            subscription.callback.param1 = &waitSem;

            /* Subscribe to the test topic filter using the blocking SUBSCRIBE
             * function. */
            status = AwsIotMqtt_TimedSubscribe( _AwsIotTestMqttConnection,
                                                &subscription,
                                                1,
                                                0,
                                                AWS_IOT_TEST_MQTT_TIMEOUT_MS );
            TEST_ASSERT_EQUAL( AWS_IOT_MQTT_SUCCESS, status );

            /* Set the members of the publish info. */
            publishInfo.QoS = QoS;
            publishInfo.pTopicName = AWS_IOT_TEST_MQTT_TOPIC_PREFIX "/SubscribePublishWait";
            publishInfo.topicNameLength = ( uint16_t ) strlen( publishInfo.pTopicName );
            publishInfo.pPayload = _pSamplePayload;
            publishInfo.payloadLength = _samplePayloadLength;

            /* Publish the message. */
            status = AwsIotMqtt_TimedPublish( _AwsIotTestMqttConnection,
                                              &publishInfo,
                                              0,
                                              AWS_IOT_TEST_MQTT_TIMEOUT_MS );

            /* Wait for the message to be received. */
            if( AwsIotSemaphore_TimedWait( &waitSem,
                                           AWS_IOT_TEST_MQTT_TIMEOUT_MS ) == false )
            {
                TEST_FAIL_MESSAGE( "Timed out waiting for subscription." );
            }

            /* Unsubscribe from the test topic filter. */
            status = AwsIotMqtt_TimedUnsubscribe( _AwsIotTestMqttConnection,
                                                  &subscription,
                                                  1,
                                                  0,
                                                  AWS_IOT_TEST_MQTT_TIMEOUT_MS );
            TEST_ASSERT_EQUAL( AWS_IOT_MQTT_SUCCESS, status );
        }

        /* Close the MQTT connection. */
        AwsIotMqtt_Disconnect( _AwsIotTestMqttConnection, false );
    }

    AwsIotSemaphore_Destroy( &waitSem );

    /* Check that the serializer overrides were called. */
    if( TEST_PROTECT() )
    {
        TEST_ASSERT_EQUAL_INT( true, _freePacketOverride );
        TEST_ASSERT_EQUAL_INT( true, _connectSerializerOverride );
        TEST_ASSERT_EQUAL_INT( true, _publishSerializerOverride );
        TEST_ASSERT_EQUAL_INT( true, _subscribeSerializerOverride );
        TEST_ASSERT_EQUAL_INT( true, _unsubscribeSerializerOverride );
        TEST_ASSERT_EQUAL_INT( true, _disconnectSerializerOverride );

        if( QoS == 1 )
        {
            TEST_ASSERT_EQUAL_INT( true, _pubackSerializerOverride );
        }
    }
}

/*-----------------------------------------------------------*/

/**
 * @brief Test group for MQTT system tests.
 */
TEST_GROUP( MQTT_System );

/*-----------------------------------------------------------*/

/**
 * @brief Test setup for MQTT system tests.
 */
TEST_SETUP( MQTT_System )
{
    /* Clear the serializer override flags. */
    _freePacketOverride = false;
    _connectSerializerOverride = false;
    _publishSerializerOverride = false;
    _pubackSerializerOverride = false;
    _subscribeSerializerOverride = false;
    _unsubscribeSerializerOverride = false;
    _disconnectSerializerOverride = false;

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

    /* Generate a new, unique client identifier based on the time if no client
     * identifier is defined. Otherwise, copy the provided client identifier. */
    #ifndef AWS_IOT_TEST_MQTT_CLIENT_IDENTIFIER
        ( void ) snprintf( _pClientIdentifier,
                           _CLIENT_IDENTIFIER_MAX_LENGTH,
                           "aws%llu",
                           ( long long unsigned int ) AwsIotClock_GetTimeMs() );
    #else
        ( void ) strncpy( _pClientIdentifier,
                          AWS_IOT_TEST_MQTT_CLIENT_IDENTIFIER,
                          _CLIENT_IDENTIFIER_MAX_LENGTH );
    #endif
}

/*-----------------------------------------------------------*/

/**
 * @brief Test tear down for MQTT system tests.
 */
TEST_TEAR_DOWN( MQTT_System )
{
    /* Clean up the network stack. */
    AwsIotTest_NetworkCleanup();

    /* Clean up the MQTT library. */
    AwsIotMqtt_Cleanup();
    _AwsIotTestMqttConnection = AWS_IOT_MQTT_CONNECTION_INITIALIZER;
}

/*-----------------------------------------------------------*/

/**
 * @brief Test group runner for MQTT system tests.
 */
TEST_GROUP_RUNNER( MQTT_System )
{
    RUN_TEST_CASE( MQTT_System, SubscribePublishWaitQoS0 );
    RUN_TEST_CASE( MQTT_System, SubscribePublishWaitQoS1 );
    RUN_TEST_CASE( MQTT_System, SubscribePublishAsync );
    RUN_TEST_CASE( MQTT_System, LastWillAndTestament );
    RUN_TEST_CASE( MQTT_System, RestorePreviousSession );
}

/*-----------------------------------------------------------*/

/**
 * @brief Subscribe-publish-wait (QoS 0).
 */
TEST( MQTT_System, SubscribePublishWaitQoS0 )
{
    _subscribePublishWait( 0 );
}

/*-----------------------------------------------------------*/

/**
 * @brief Subscribe-publish-wait (QoS 1).
 */
TEST( MQTT_System, SubscribePublishWaitQoS1 )
{
    _subscribePublishWait( 1 );
}

/*-----------------------------------------------------------*/

/**
 * @brief Subscribe-publish asynchronous (QoS 1).
 */
TEST( MQTT_System, SubscribePublishAsync )
{
    AwsIotMqttError_t status = AWS_IOT_MQTT_STATUS_PENDING;
    AwsIotMqttConnectInfo_t connectInfo = AWS_IOT_MQTT_CONNECT_INFO_INITIALIZER;
    AwsIotMqttSubscription_t subscription = AWS_IOT_MQTT_SUBSCRIPTION_INITIALIZER;
    AwsIotMqttPublishInfo_t publishInfo = AWS_IOT_MQTT_PUBLISH_INFO_INITIALIZER;
    AwsIotMqttCallbackInfo_t callbackInfo = AWS_IOT_MQTT_CALLBACK_INFO_INITIALIZER;
    _operationCompleteParams_t callbackParam = { 0 };
    AwsIotSemaphore_t publishWaitSem;

    /* Initialize members of the operation callback info. */
    callbackInfo.function = _operationComplete;
    callbackInfo.param1 = &callbackParam;

    /* Initialize members of the subscription. */
    subscription.pTopicFilter = AWS_IOT_TEST_MQTT_TOPIC_PREFIX "/SubscribePublishAsync";
    subscription.topicFilterLength = ( uint16_t ) strlen( subscription.pTopicFilter );
    subscription.callback.function = _publishReceived;
    subscription.callback.param1 = &publishWaitSem;

    /* Initialize members of the connect info. */
    connectInfo.cleanSession = true;
    connectInfo.pClientIdentifier = _pClientIdentifier;
    connectInfo.clientIdentifierLength = ( uint16_t ) strlen( _pClientIdentifier );

    /* Initialize members of the publish info. */
    publishInfo.QoS = 1;
    publishInfo.pTopicName = AWS_IOT_TEST_MQTT_TOPIC_PREFIX "/SubscribePublishAsync";
    publishInfo.topicNameLength = ( uint16_t ) strlen( publishInfo.pTopicName );
    publishInfo.pPayload = _pSamplePayload;
    publishInfo.payloadLength = _samplePayloadLength;

    /* Create the wait semaphore for operations. */
    TEST_ASSERT_EQUAL_INT( true, AwsIotSemaphore_Create( &( callbackParam.waitSem ), 0, 1 ) );

    if( TEST_PROTECT() )
    {
        /* Create the wait semaphore for subscriptions. */
        TEST_ASSERT_EQUAL_INT( true, AwsIotSemaphore_Create( &publishWaitSem, 0, 1 ) );

        if( TEST_PROTECT() )
        {
            /* Establish the MQTT connection. */
            status = AwsIotMqtt_Connect( &_AwsIotTestMqttConnection,
                                         &_AwsIotTestNetworkInterface,
                                         &connectInfo,
                                         NULL,
                                         AWS_IOT_TEST_MQTT_TIMEOUT_MS );
            TEST_ASSERT_EQUAL( AWS_IOT_MQTT_SUCCESS, status );

            if( TEST_PROTECT() )
            {
                /* Subscribe to the test topic filter. */
                callbackParam.expectedOperation = AWS_IOT_MQTT_SUBSCRIBE;
                status = AwsIotMqtt_Subscribe( _AwsIotTestMqttConnection,
                                               &subscription,
                                               1,
                                               0,
                                               &callbackInfo,
                                               &( callbackParam.reference ) );

                if( AwsIotSemaphore_TimedWait( &( callbackParam.waitSem ),
                                               AWS_IOT_TEST_MQTT_TIMEOUT_MS ) == false )
                {
                    TEST_FAIL_MESSAGE( "Timed out waiting for SUBSCRIBE to complete." );
                }

                /* Publish the message. */
                callbackParam.expectedOperation = AWS_IOT_MQTT_PUBLISH_TO_SERVER;
                status = AwsIotMqtt_Publish( _AwsIotTestMqttConnection,
                                             &publishInfo,
                                             0,
                                             &callbackInfo,
                                             &( callbackParam.reference ) );

                if( AwsIotSemaphore_TimedWait( &( callbackParam.waitSem ),
                                               AWS_IOT_TEST_MQTT_TIMEOUT_MS ) == false )
                {
                    TEST_FAIL_MESSAGE( "Timed out waiting for PUBLISH to complete." );
                }

                /* Wait for the message to be received. */
                if( AwsIotSemaphore_TimedWait( &publishWaitSem,
                                               AWS_IOT_TEST_MQTT_TIMEOUT_MS ) == false )
                {
                    TEST_FAIL_MESSAGE( "Timed out waiting for subscription." );
                }

                /* Unsubscribe from the test topic filter. */
                callbackParam.expectedOperation = AWS_IOT_MQTT_UNSUBSCRIBE;
                status = AwsIotMqtt_Unsubscribe( _AwsIotTestMqttConnection,
                                                 &subscription,
                                                 1,
                                                 0,
                                                 &callbackInfo,
                                                 &( callbackParam.reference ) );

                if( AwsIotSemaphore_TimedWait( &( callbackParam.waitSem ),
                                               AWS_IOT_TEST_MQTT_TIMEOUT_MS ) == false )
                {
                    TEST_FAIL_MESSAGE( "Timed out waiting for UNSUBSCRIBE to complete." );
                }
            }

            AwsIotMqtt_Disconnect( _AwsIotTestMqttConnection, false );
        }

        AwsIotSemaphore_Destroy( &publishWaitSem );
    }

    AwsIotSemaphore_Destroy( &( callbackParam.waitSem ) );
}

/*-----------------------------------------------------------*/

/**
 * @brief Test that a LWT is published if an MQTT connection is unexpectedly closed.
 */
TEST( MQTT_System, LastWillAndTestament )
{
    AwsIotMqttError_t status = AWS_IOT_MQTT_STATUS_PENDING;
    AwsIotMqttNetIf_t lwtNetIf = _AwsIotTestNetworkInterface;
    char pLwtListenerClientIdentifier[ _CLIENT_IDENTIFIER_MAX_LENGTH ] = { 0 };
    AwsIotMqttConnection_t lwtListener = AWS_IOT_MQTT_CONNECTION_INITIALIZER;
    AwsIotMqttConnectInfo_t lwtConnectInfo = AWS_IOT_MQTT_CONNECT_INFO_INITIALIZER,
                            connectInfo = AWS_IOT_MQTT_CONNECT_INFO_INITIALIZER;
    AwsIotMqttSubscription_t willSubscription = AWS_IOT_MQTT_SUBSCRIPTION_INITIALIZER;
    AwsIotMqttPublishInfo_t willInfo = AWS_IOT_MQTT_PUBLISH_INFO_INITIALIZER;
    void * pLwtListenerConnection = NULL;
    AwsIotSemaphore_t waitSem;

    /* Create the wait semaphore. */
    TEST_ASSERT_EQUAL_INT( true, AwsIotSemaphore_Create( &waitSem, 0, 1 ) );

    /* Generate a client identifier for LWT listener. */
    #ifndef AWS_IOT_TEST_MQTT_CLIENT_IDENTIFIER
        ( void ) snprintf( pLwtListenerClientIdentifier,
                           _CLIENT_IDENTIFIER_MAX_LENGTH,
                           "awslwt%llu",
                           ( long long unsigned int ) AwsIotClock_GetTimeMs() );
    #else
        ( void ) strncpy( pLwtListenerClientIdentifier,
                          AWS_IOT_TEST_MQTT_CLIENT_IDENTIFIER "LWT",
                          _CLIENT_IDENTIFIER_MAX_LENGTH );
    #endif

    if( TEST_PROTECT() )
    {
        /* Establish an independent MQTT over TCP connection to receive a Last
         * Will and Testament message. */
        TEST_ASSERT_EQUAL( true,
                           AwsIotTest_NetworkConnect( &pLwtListenerConnection,
                                                      &lwtListener ) );

        if( TEST_PROTECT() )
        {
            lwtNetIf.pDisconnectContext = pLwtListenerConnection;
            lwtNetIf.pSendContext = pLwtListenerConnection;
            lwtConnectInfo.cleanSession = true;
            lwtConnectInfo.pClientIdentifier = pLwtListenerClientIdentifier;
            lwtConnectInfo.clientIdentifierLength = ( uint16_t ) strlen( lwtConnectInfo.pClientIdentifier );

            status = AwsIotMqtt_Connect( &lwtListener,
                                         &lwtNetIf,
                                         &lwtConnectInfo,
                                         NULL,
                                         AWS_IOT_TEST_MQTT_TIMEOUT_MS );
            TEST_ASSERT_EQUAL( AWS_IOT_MQTT_SUCCESS, status );

            if( TEST_PROTECT() )
            {
                /* Register a subscription for the LWT. */
                willSubscription.pTopicFilter = AWS_IOT_TEST_MQTT_TOPIC_PREFIX "/LastWillAndTestament";
                willSubscription.topicFilterLength = ( uint16_t ) strlen( willSubscription.pTopicFilter );
                willSubscription.callback.function = _publishReceived;
                willSubscription.callback.param1 = &waitSem;

                status = AwsIotMqtt_TimedSubscribe( lwtListener,
                                                    &willSubscription,
                                                    1,
                                                    0,
                                                    AWS_IOT_TEST_MQTT_TIMEOUT_MS );
                TEST_ASSERT_EQUAL( AWS_IOT_MQTT_SUCCESS, status );

                /* Create a connection that requests the LWT. */
                connectInfo.cleanSession = true;
                connectInfo.pClientIdentifier = _pClientIdentifier;
                connectInfo.clientIdentifierLength = ( uint16_t ) strlen( _pClientIdentifier );

                willInfo.pTopicName = AWS_IOT_TEST_MQTT_TOPIC_PREFIX "/LastWillAndTestament";
                willInfo.topicNameLength = ( uint16_t ) strlen( willInfo.pTopicName );
                willInfo.pPayload = _pSamplePayload;
                willInfo.payloadLength = _samplePayloadLength;

                status = AwsIotMqtt_Connect( &_AwsIotTestMqttConnection,
                                             &_AwsIotTestNetworkInterface,
                                             &connectInfo,
                                             &willInfo,
                                             AWS_IOT_TEST_MQTT_TIMEOUT_MS );
                TEST_ASSERT_EQUAL( AWS_IOT_MQTT_SUCCESS, status );

                /* Abruptly close the MQTT connection. This should cause the LWT
                 * to be sent to the LWT listener. */
                AwsIotTest_NetworkClose( NULL );
                AwsIotMqtt_Disconnect( _AwsIotTestMqttConnection, true );
                AwsIotTest_NetworkDestroy( NULL );

                /* Check that the LWT was received. */
                if( AwsIotSemaphore_TimedWait( &waitSem,
                                               AWS_IOT_TEST_MQTT_TIMEOUT_MS ) == false )
                {
                    TEST_FAIL_MESSAGE( "Timed out waiting for Last Will and Testament." );
                }
            }

            AwsIotMqtt_Disconnect( lwtListener, false );
            AwsIotTest_NetworkDestroy( pLwtListenerConnection );
            pLwtListenerConnection = NULL;
        }

        if( pLwtListenerConnection != NULL )
        {
            AwsIotTest_NetworkClose( pLwtListenerConnection );
            AwsIotTest_NetworkDestroy( pLwtListenerConnection );
        }
    }

    AwsIotSemaphore_Destroy( &waitSem );
}

/*-----------------------------------------------------------*/

/**
 * @brief Test that subscriptions from a previous session are restored.
 */
TEST( MQTT_System, RestorePreviousSession )
{
    AwsIotMqttError_t status = AWS_IOT_MQTT_STATUS_PENDING;
    AwsIotMqttConnectInfo_t connectInfo = AWS_IOT_MQTT_CONNECT_INFO_INITIALIZER;
    AwsIotMqttSubscription_t subscription = AWS_IOT_MQTT_SUBSCRIPTION_INITIALIZER;
    AwsIotMqttPublishInfo_t publishInfo = AWS_IOT_MQTT_PUBLISH_INFO_INITIALIZER;
    AwsIotSemaphore_t waitSem;

    /* Create the wait semaphore for operations. */
    TEST_ASSERT_EQUAL_INT( true, AwsIotSemaphore_Create( &waitSem, 0, 1 ) );

    /* Set the members of the connection info for a persistent session. */
    connectInfo.cleanSession = false;
    connectInfo.pClientIdentifier = _pClientIdentifier;
    connectInfo.clientIdentifierLength = ( uint16_t ) strlen( _pClientIdentifier );

    if( TEST_PROTECT() )
    {
        /* Establish a persistent MQTT connection. */
        status = AwsIotMqtt_Connect( &_AwsIotTestMqttConnection,
                                     &_AwsIotTestNetworkInterface,
                                     &connectInfo,
                                     NULL,
                                     AWS_IOT_TEST_MQTT_TIMEOUT_MS );
        TEST_ASSERT_EQUAL( AWS_IOT_MQTT_SUCCESS, status );

        /* Add a subscription. */
        subscription.pTopicFilter = AWS_IOT_TEST_MQTT_TOPIC_PREFIX "/RestorePreviousSession";
        subscription.topicFilterLength = ( uint16_t ) strlen( subscription.pTopicFilter );
        subscription.callback.param1 = &waitSem;
        subscription.callback.function = _publishReceived;

        status = AwsIotMqtt_TimedSubscribe( _AwsIotTestMqttConnection,
                                            &subscription,
                                            1,
                                            0,
                                            AWS_IOT_TEST_MQTT_TIMEOUT_MS );
        TEST_ASSERT_EQUAL( AWS_IOT_MQTT_SUCCESS, status );

        /* Disconnect the MQTT connection and clean up network connection. */
        AwsIotMqtt_Disconnect( _AwsIotTestMqttConnection, false );
        AwsIotTest_NetworkCleanup();

        /* Re-establish the network connection. */
        TEST_ASSERT_EQUAL_INT( true, AwsIotTest_NetworkSetup() );

        /* Re-establish the MQTT connection with a previous session. */
        connectInfo.cleanSession = false;
        status = AwsIotMqtt_ConnectRestoreSession( &_AwsIotTestMqttConnection,
                                                   &_AwsIotTestNetworkInterface,
                                                   &connectInfo,
                                                   NULL,
                                                   &subscription,
                                                   1,
                                                   AWS_IOT_TEST_MQTT_TIMEOUT_MS );
        TEST_ASSERT_EQUAL( AWS_IOT_MQTT_SUCCESS, status );

        /* Publish a message to the subscription added in the previous session. */
        publishInfo.pTopicName = AWS_IOT_TEST_MQTT_TOPIC_PREFIX "/RestorePreviousSession";
        publishInfo.topicNameLength = ( uint16_t ) strlen( publishInfo.pTopicName );
        publishInfo.pPayload = _pSamplePayload;
        publishInfo.payloadLength = _samplePayloadLength;

        status = AwsIotMqtt_TimedPublish( _AwsIotTestMqttConnection,
                                          &publishInfo,
                                          0,
                                          AWS_IOT_TEST_MQTT_TIMEOUT_MS );
        TEST_ASSERT_EQUAL( AWS_IOT_MQTT_SUCCESS, status );

        /* Wait for the message to be received. */
        if( AwsIotSemaphore_TimedWait( &waitSem,
                                       AWS_IOT_TEST_MQTT_TIMEOUT_MS ) == false )
        {
            TEST_FAIL_MESSAGE( "Timed out waiting for message." );
        }

        /* Disconnect the MQTT connection. */
        AwsIotMqtt_Disconnect( _AwsIotTestMqttConnection, false );
        AwsIotTest_NetworkCleanup();
    }

    AwsIotSemaphore_Destroy( &waitSem );

    if( TEST_PROTECT() )
    {
        /* Re-establish the network connection. */
        TEST_ASSERT_EQUAL_INT( true, AwsIotTest_NetworkSetup() );

        /* After this test is finished, establish one more connection with a clean
         * session to clean up persistent sessions on the MQTT server created by this
         * test. */
        connectInfo.cleanSession = true;
        status = AwsIotMqtt_Connect( &_AwsIotTestMqttConnection,
                                     &_AwsIotTestNetworkInterface,
                                     &connectInfo,
                                     NULL,
                                     AWS_IOT_TEST_MQTT_TIMEOUT_MS );
        TEST_ASSERT_EQUAL( AWS_IOT_MQTT_SUCCESS, status );

        AwsIotMqtt_Disconnect( _AwsIotTestMqttConnection, false );
    }
}

/*-----------------------------------------------------------*/
