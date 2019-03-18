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
 * @file iot_tests_mqtt_system.c
 * @brief Full system tests for the MQTT library.
 */

/* Build using a config header, if provided. */
#ifdef IOT_CONFIG_FILE
    #include IOT_CONFIG_FILE
#endif

/* Standard includes. */
#include <string.h>

/* Common include. */
#include "iot_common.h"

/* MQTT internal include. */
#include "private/iot_mqtt_internal.h"

/* Platform layer includes. */
#include "platform/iot_clock.h"
#include "platform/iot_threads.h"
#include "platform/iot_network_afr.h"

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
#ifndef IOT_TEST_MQTT_TIMEOUT_MS
    #define IOT_TEST_MQTT_TIMEOUT_MS      ( 5000 )
#endif
#ifndef IOT_TEST_MQTT_TOPIC_PREFIX
    #define IOT_TEST_MQTT_TOPIC_PREFIX    "iotmqtttest"
#endif
/** @endcond */

/**
 * @brief Determine which MQTT server mode to test (AWS IoT or Mosquitto).
 */
#if !defined( IOT_TEST_MQTT_MOSQUITTO ) || IOT_TEST_MQTT_MOSQUITTO == 0
    #define _AWS_IOT_MQTT_SERVER    true
#else
    #define _AWS_IOT_MQTT_SERVER    false

/* Redefine the connect info initializer if not using an AWS IoT MQTT server. */
    #undef IOT_MQTT_CONNECT_INFO_INITIALIZER
    #define IOT_MQTT_CONNECT_INFO_INITIALIZER    { 0 }
#endif

/**
 * @brief The maximum length of an MQTT client identifier.
 *
 * When @ref IOT_TEST_MQTT_CLIENT_IDENTIFIER is defined, this value must
 * accommodate the length of @ref IOT_TEST_MQTT_CLIENT_IDENTIFIER plus 4
 * to accommodate the Last Will and Testament test. Otherwise, this value is
 * set to 24, which is the longest client identifier length an MQTT server is
 * obligated to accept plus a NULL terminator.
 */
#ifdef IOT_TEST_MQTT_CLIENT_IDENTIFIER
    #define _CLIENT_IDENTIFIER_MAX_LENGTH    ( sizeof( IOT_TEST_MQTT_CLIENT_IDENTIFIER ) + 4 )
#else
    #define _CLIENT_IDENTIFIER_MAX_LENGTH    ( 24 )
#endif

/*-----------------------------------------------------------*/

/**
 * @brief Parameter 1 of #_operationComplete.
 */
typedef struct _operationCompleteParams
{
    IotMqttOperationType_t expectedOperation; /**< @brief Expected completed operation. */
    IotSemaphore_t waitSem;                   /**< @brief Used to unblock waiting test thread. */
    IotMqttReference_t reference;             /**< @brief Reference to expected completed operation. */
} _operationCompleteParams_t;

/*-----------------------------------------------------------*/

extern IotMqttConnection_t IotMqttConnection;
extern IotMqttNetworkInfo_t IotNetworkInfo;
/* Network functions used by the tests, declared and implemented in one of
 * the test network function files. */
extern BaseType_t IotTestNetwork_Connect(void);
extern void IotTestNetwork_Cleanup(void);
extern void IotTestNetwork_Close(void);
extern void IotTestNetwork_Destroy(void);
extern void IotTestNetwork_OverrideSerializer( IotMqttSerializer_t * pSerializer );
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
    " culpa qui officia deserunt mollit anim id est laborum."
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
char _pClientIdentifier[ _CLIENT_IDENTIFIER_MAX_LENGTH ] = { 0 };

/*
 * Track if the serializer overrides were called for a test.
 */
static bool _freePacketOverride = true;            /**< @brief Tracks if #_freePacket was called. */
static bool _connectSerializerOverride = true;     /**< @brief Tracks if #_connectSerializerOverride was called. */
static bool _publishSerializerOverride = true;     /**< @brief Tracks if #_publishSerializerOverride was called. */
static bool _pubackSerializerOverride = true;      /**< @brief Tracks if #_pubackSerializerOverride was called. */
static bool _subscribeSerializerOverride = true;   /**< @brief Tracks if #_subscribeSerializerOverride was called. */
static bool _unsubscribeSerializerOverride = true; /**< @brief Tracks if #_unsubscribeSerializerOverride was called. */
static bool _disconnectSerializerOverride = true;  /**< @brief Tracks if #_disconnectSerializerOverride was called. */

/*-----------------------------------------------------------*/

/**
 * @brief Packet free function override
 */
static void _freePacket( uint8_t * pPacket )
{
    _freePacketOverride = true;

    _IotMqtt_FreePacket( pPacket );
}

/*-----------------------------------------------------------*/

/**
 * @brief Serializer override for CONNECT.
 */
static IotMqttError_t _serializeConnect( const IotMqttConnectInfo_t * pConnectInfo,
                                         uint8_t ** pConnectPacket,
                                         size_t * pPacketSize )
{
    _connectSerializerOverride = true;

    return _IotMqtt_SerializeConnect( pConnectInfo,
                                      pConnectPacket,
                                      pPacketSize );
}

/*-----------------------------------------------------------*/

/**
 * @brief Serializer override for PUBLISH.
 */
static IotMqttError_t _serializePublish( const IotMqttPublishInfo_t * pPublishInfo,
                                         uint8_t ** pPublishPacket,
                                         size_t * pPacketSize,
                                         uint16_t * pPacketIdentifier,
                                         uint8_t ** pPacketIdentifierHigh )
{
    _publishSerializerOverride = true;

    return _IotMqtt_SerializePublish( pPublishInfo,
                                      pPublishPacket,
                                      pPacketSize,
                                      pPacketIdentifier,
                                      pPacketIdentifierHigh );
}

/*-----------------------------------------------------------*/

/**
 * @brief Serializer override for PUBACK.
 */
static IotMqttError_t _serializePuback( uint16_t packetIdentifier,
                                        uint8_t ** pPubackPacket,
                                        size_t * pPacketSize )
{
    _pubackSerializerOverride = true;

    return _IotMqtt_SerializePuback( packetIdentifier,
                                     pPubackPacket,
                                     pPacketSize );
}

/*-----------------------------------------------------------*/

/**
 * @brief Serializer override for SUBSCRIBE.
 */
static IotMqttError_t _serializeSubscribe( const IotMqttSubscription_t * pSubscriptionList,
                                           size_t subscriptionCount,
                                           uint8_t ** pSubscribePacket,
                                           size_t * pPacketSize,
                                           uint16_t * pPacketIdentifier )
{
    _subscribeSerializerOverride = true;

    return _IotMqtt_SerializeSubscribe( pSubscriptionList,
                                        subscriptionCount,
                                        pSubscribePacket,
                                        pPacketSize,
                                        pPacketIdentifier );
}

/*-----------------------------------------------------------*/

/**
 * @brief Serializer override for UNSUBSCRIBE.
 */
static IotMqttError_t _serializeUnsubscribe( const IotMqttSubscription_t * pSubscriptionList,
                                             size_t subscriptionCount,
                                             uint8_t ** pSubscribePacket,
                                             size_t * pPacketSize,
                                             uint16_t * pPacketIdentifier )
{
    _unsubscribeSerializerOverride = true;

    return _IotMqtt_SerializeUnsubscribe( pSubscriptionList,
                                          subscriptionCount,
                                          pSubscribePacket,
                                          pPacketSize,
                                          pPacketIdentifier );
}

/*-----------------------------------------------------------*/

/**
 * @brief Serializer override for DISCONNECT.
 */
static IotMqttError_t _serializeDisconnect( uint8_t ** pDisconnectPacket,
                                            size_t * pPacketSize )
{
    _disconnectSerializerOverride = true;

    return _IotMqtt_SerializeDisconnect( pDisconnectPacket,
                                         pPacketSize );
}

static const IotMqttSerializer_t _IotTestMqttSerializer = {
    .serialize.connect       = _serializeConnect,
    .serialize.publish       = _serializePublish,
    .serialize.publishSetDup = NULL,
    .serialize.puback        = _serializePuback,
    .serialize.subscribe     = _serializeSubscribe,
    .serialize.unsubscribe   = _serializeUnsubscribe,
    .serialize.disconnect    = _serializeDisconnect,
    .freePacket              = _freePacket,
    .getPacketType           = NULL,
    .getRemainingLength      = NULL,
    .deserialize.connack     = NULL,
    .deserialize.publish     = NULL,
    .deserialize.puback      = NULL,
    .deserialize.suback      = NULL,
    .deserialize.unsuback    = NULL
};

/*-----------------------------------------------------------*/

/**
 * @brief Subscription callback function. Checks for valid parameters and unblocks
 * the main test thread.
 */
static void _publishReceived( void * pArgument,
                              IotMqttCallbackParam_t * pPublish )
{
    IotSemaphore_t * pWaitSem = ( IotSemaphore_t * ) pArgument;

    /* If the received messages matches what was sent, unblock the waiting thread. */
    if( ( pPublish->message.info.payloadLength == _samplePayloadLength ) &&
        ( strncmp( pPublish->message.info.pPayload,
                   _pSamplePayload,
                   _samplePayloadLength ) == 0 ) )
    {
        IotSemaphore_Post( pWaitSem );
    }
}

/*-----------------------------------------------------------*/

/**
 * @brief Operation completion callback function. Checks for valid parameters
 * and unblocks the main test thread.
 */
static void _operationComplete( void * pArgument,
                                IotMqttCallbackParam_t * pOperation )
{
    _operationCompleteParams_t * pParams = ( _operationCompleteParams_t * ) pArgument;

    /* If the operation information matches the parameters and the operation was
     * successful, unblock the waiting thread. */
    if( ( pParams->expectedOperation == pOperation->operation.type ) &&
        ( pParams->reference == pOperation->operation.reference ) &&
        ( pOperation->operation.result == IOT_MQTT_SUCCESS ) )
    {
        IotSemaphore_Post( &( pParams->waitSem ) );
    }
}

/*-----------------------------------------------------------*/

/**
 * @brief Callback that makes additional MQTT API calls.
 */
static void _reentrantCallback( void * pArgument,
                                IotMqttCallbackParam_t * pOperation )
{
    bool status = true;
    IotMqttError_t mqttStatus = IOT_MQTT_STATUS_PENDING;
    IotSemaphore_t * pWaitSemaphores = ( IotSemaphore_t * ) pArgument;
    IotMqttPublishInfo_t publishInfo = IOT_MQTT_PUBLISH_INFO_INITIALIZER;
    IotMqttSubscription_t subscription = IOT_MQTT_SUBSCRIPTION_INITIALIZER;
    IotMqttReference_t unsubscribeRef = IOT_MQTT_REFERENCE_INITIALIZER;

    /* Topic used in this test. */
    const char * const pTopic = IOT_TEST_MQTT_TOPIC_PREFIX "/Reentrancy";
    const uint16_t topicLength = ( uint16_t ) strlen( pTopic );

    publishInfo.qos = IOT_MQTT_QOS_1;
    publishInfo.pTopicName = pTopic;
    publishInfo.topicNameLength = topicLength;
    publishInfo.pPayload = _pSamplePayload;
    publishInfo.payloadLength = _samplePayloadLength;
    publishInfo.retryLimit = 3;
    publishInfo.retryMs = 5000;

    mqttStatus = IotMqtt_TimedPublish( pOperation->mqttConnection,
                                       &publishInfo,
                                       0,
                                       IOT_TEST_MQTT_TIMEOUT_MS );

    if( mqttStatus == IOT_MQTT_SUCCESS )
    {
        status = IotSemaphore_TimedWait( &( pWaitSemaphores[ 0 ] ),
                                         IOT_TEST_MQTT_TIMEOUT_MS );
    }
    else
    {
        status = false;
    }

    /* Remove subscription and disconnect. */
    if( status == true )
    {
        subscription.pTopicFilter = pTopic;
        subscription.topicFilterLength = topicLength;

        mqttStatus = IotMqtt_Unsubscribe( pOperation->mqttConnection,
                                          &subscription,
                                          1,
                                          IOT_MQTT_FLAG_WAITABLE,
                                          NULL,
                                          &unsubscribeRef );

        if( mqttStatus == IOT_MQTT_STATUS_PENDING )
        {
            /* Disconnect the MQTT connection. */
            IotMqtt_Disconnect( pOperation->mqttConnection, false );

            /* Waiting on an operation whose connection is closed should return
             * "Network Error". */
            mqttStatus = IotMqtt_Wait( unsubscribeRef,
                                       500 );

            status = ( mqttStatus == IOT_MQTT_NETWORK_ERROR );
        }
        else
        {
            status = false;
        }
    }

    /* Disconnect and unblock the main test thread. */
    if( status == true )
    {
        IotSemaphore_Post( &( pWaitSemaphores[ 1 ] ) );
    }
}

/*-----------------------------------------------------------*/

/**
 * @brief Run the subscribe-publish-wait tests at various QoS.
 */
void IotTestMqtt_SubscribePublishWait( IotMqttQos_t qos, IotMqttNetworkInfo_t * networkInfo )
{
    IotMqttError_t status = IOT_MQTT_STATUS_PENDING;
    IotMqttConnectInfo_t connectInfo = IOT_MQTT_CONNECT_INFO_INITIALIZER;
    IotMqttSubscription_t subscription = IOT_MQTT_SUBSCRIPTION_INITIALIZER;
    IotMqttPublishInfo_t publishInfo = IOT_MQTT_PUBLISH_INFO_INITIALIZER;
    IotSemaphore_t waitSem;

    /* Create the wait semaphore. */
    TEST_ASSERT_EQUAL_INT( true, IotSemaphore_Create( &waitSem, 0, 1 ) );

    if( TEST_PROTECT() )
    {
        /* Set the members of the MQTT connection info. */
        connectInfo.cleanSession = true;
        connectInfo.pClientIdentifier = _pClientIdentifier;
        connectInfo.clientIdentifierLength = ( uint16_t ) strlen( _pClientIdentifier );

        /* Establish the MQTT connection. */
        status = IotMqtt_Connect( networkInfo,
                                  &connectInfo,
                                  IOT_TEST_MQTT_TIMEOUT_MS,
                                  &IotMqttConnection );
        TEST_ASSERT_EQUAL( IOT_MQTT_SUCCESS, status );

        if( TEST_PROTECT() )
        {
            /* Set the members of the subscription. */
            subscription.qos = qos;
            subscription.pTopicFilter = IOT_TEST_MQTT_TOPIC_PREFIX "/SubscribePublishWait";
            subscription.topicFilterLength = ( uint16_t ) strlen( subscription.pTopicFilter );
            subscription.callback.function = _publishReceived;
            subscription.callback.param1 = &waitSem;

            /* Subscribe to the test topic filter using the blocking SUBSCRIBE
             * function. */
            status = IotMqtt_TimedSubscribe( IotMqttConnection,
                                             &subscription,
                                             1,
                                             0,
                                             IOT_TEST_MQTT_TIMEOUT_MS );
            TEST_ASSERT_EQUAL( IOT_MQTT_SUCCESS, status );

            /* Set the members of the publish info. */
            publishInfo.qos = qos;
            publishInfo.pTopicName = IOT_TEST_MQTT_TOPIC_PREFIX "/SubscribePublishWait";
            publishInfo.topicNameLength = ( uint16_t ) strlen( publishInfo.pTopicName );
            publishInfo.pPayload = _pSamplePayload;
            publishInfo.payloadLength = _samplePayloadLength;

            /* Publish the message. */
            status = IotMqtt_TimedPublish( IotMqttConnection,
                                           &publishInfo,
                                           0,
                                           IOT_TEST_MQTT_TIMEOUT_MS );

            /* Wait for the message to be received. */
            if( IotSemaphore_TimedWait( &waitSem,
                                        IOT_TEST_MQTT_TIMEOUT_MS ) == false )
            {
                TEST_FAIL_MESSAGE( "Timed out waiting for subscription." );
            }

            /* Unsubscribe from the test topic filter. */
            status = IotMqtt_TimedUnsubscribe( IotMqttConnection,
                                               &subscription,
                                               1,
                                               0,
                                               IOT_TEST_MQTT_TIMEOUT_MS );
            TEST_ASSERT_EQUAL( IOT_MQTT_SUCCESS, status );
        }

        /* Close the MQTT connection. */
        IotMqtt_Disconnect( IotMqttConnection, false );
    }

    IotSemaphore_Destroy( &waitSem );

    /* Check that the serializer overrides were called. */
    if( TEST_PROTECT() )
    {
        TEST_ASSERT_EQUAL_INT( true, _freePacketOverride );
        TEST_ASSERT_EQUAL_INT( true, _connectSerializerOverride );
        TEST_ASSERT_EQUAL_INT( true, _publishSerializerOverride );
        TEST_ASSERT_EQUAL_INT( true, _subscribeSerializerOverride );
        TEST_ASSERT_EQUAL_INT( true, _unsubscribeSerializerOverride );
        TEST_ASSERT_EQUAL_INT( true, _disconnectSerializerOverride );

        if( qos == IOT_MQTT_QOS_1 )
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

    /* Initialize common components. */
    if( IotCommon_Init() == false )
    {
        TEST_FAIL_MESSAGE( "Failed to initialize common components." );
    }

    /* Initialize the MQTT library. */
    if( IotMqtt_Init() != IOT_MQTT_SUCCESS )
    {
        TEST_FAIL_MESSAGE( "Failed to initialize MQTT library." );
    }

    /* Set up the network stack. */
    if( IotTestNetwork_Connect() == false )
    {
        TEST_FAIL_MESSAGE( "Failed to connect." );
    }

    IotTestNetwork_OverrideSerializer ((IotMqttSerializer_t *)&_IotTestMqttSerializer );

    /* Generate a new, unique client identifier based on the time if no client
     * identifier is defined. Otherwise, copy the provided client identifier. */
    #ifndef IOT_TEST_MQTT_CLIENT_IDENTIFIER
        ( void ) snprintf( _pClientIdentifier,
                           _CLIENT_IDENTIFIER_MAX_LENGTH,
                           "iot%llu",
                           ( long long unsigned int ) IotClock_GetTimeMs() );
    #else
        ( void ) strncpy( _pClientIdentifier,
                          IOT_TEST_MQTT_CLIENT_IDENTIFIER,
                          _CLIENT_IDENTIFIER_MAX_LENGTH );
    #endif
}

void IotTestMqtt_TearDown(void)
{
    /* Clean up the MQTT library. */
    IotMqtt_Cleanup();

    /* Clean up the network stack. */
    IotTestNetwork_Cleanup();
}

/*-----------------------------------------------------------*/

/**
 * @brief Test tear down for MQTT system tests.
 */
TEST_TEAR_DOWN( MQTT_System )
{
    /* Clean up common components. */
    IotCommon_Cleanup();

    /* Clean up the MQTT library. */
    IotMqtt_Cleanup();

    /* Clean up the network stack. */
    IotTestNetwork_Cleanup();
}

/*-----------------------------------------------------------*/

/**
 * @brief Test group runner for MQTT system tests.
 */
TEST_GROUP_RUNNER( MQTT_System )
{
#if (DEFAULT_NETWORK == AWSIOT_NETWORK_TYPE_WIFI )
    RUN_TEST_CASE( MQTT_System, LastWillAndTestament );
#endif
    RUN_TEST_CASE( MQTT_System, SubscribePublishWaitQoS0 );
    RUN_TEST_CASE( MQTT_System, SubscribePublishWaitQoS1 );
    RUN_TEST_CASE( MQTT_System, SubscribePublishAsync );
    RUN_TEST_CASE( MQTT_System, RestorePreviousSession );
    RUN_TEST_CASE( MQTT_System, WaitAfterDisconnect );
    RUN_TEST_CASE( MQTT_System, SubscribeCompleteReentrancy );
    RUN_TEST_CASE( MQTT_System, IncomingPublishReentrancy )
}

/*-----------------------------------------------------------*/

/**
 * @brief Subscribe-publish-wait (QoS 0).
 */
TEST( MQTT_System, SubscribePublishWaitQoS0 )
{
    IotTestMqtt_SubscribePublishWait( IOT_MQTT_QOS_0, &IotNetworkInfo );
}

/*-----------------------------------------------------------*/

/**
 * @brief Subscribe-publish-wait (QoS 1).
 */
TEST( MQTT_System, SubscribePublishWaitQoS1 )
{
    IotTestMqtt_SubscribePublishWait( IOT_MQTT_QOS_1, &IotNetworkInfo );
}

/*-----------------------------------------------------------*/

/**
 * @brief Subscribe-publish asynchronous (QoS 1).
 */
TEST( MQTT_System, SubscribePublishAsync )
{
    IotMqttError_t status = IOT_MQTT_STATUS_PENDING;
    IotMqttConnectInfo_t connectInfo = IOT_MQTT_CONNECT_INFO_INITIALIZER;
    IotMqttSubscription_t subscription = IOT_MQTT_SUBSCRIPTION_INITIALIZER;
    IotMqttPublishInfo_t publishInfo = IOT_MQTT_PUBLISH_INFO_INITIALIZER;
    IotMqttCallbackInfo_t callbackInfo = IOT_MQTT_CALLBACK_INFO_INITIALIZER;
    _operationCompleteParams_t callbackParam = { 0 };
    IotSemaphore_t publishWaitSem;

    /* Initialize members of the operation callback info. */
    callbackInfo.function = _operationComplete;
    callbackInfo.param1 = &callbackParam;

    /* Initialize members of the subscription. */
    subscription.pTopicFilter = IOT_TEST_MQTT_TOPIC_PREFIX "/SubscribePublishAsync";
    subscription.topicFilterLength = ( uint16_t ) strlen( subscription.pTopicFilter );
    subscription.callback.function = _publishReceived;
    subscription.callback.param1 = &publishWaitSem;

    /* Initialize members of the connect info. */
    connectInfo.cleanSession = true;
    connectInfo.pClientIdentifier = _pClientIdentifier;
    connectInfo.clientIdentifierLength = ( uint16_t ) strlen( _pClientIdentifier );

    /* Initialize members of the publish info. */
    publishInfo.qos = IOT_MQTT_QOS_1;
    publishInfo.pTopicName = IOT_TEST_MQTT_TOPIC_PREFIX "/SubscribePublishAsync";
    publishInfo.topicNameLength = ( uint16_t ) strlen( publishInfo.pTopicName );
    publishInfo.pPayload = _pSamplePayload;
    publishInfo.payloadLength = _samplePayloadLength;

    /* Create the wait semaphore for operations. */
    TEST_ASSERT_EQUAL_INT( true, IotSemaphore_Create( &( callbackParam.waitSem ), 0, 1 ) );

    if( TEST_PROTECT() )
    {
        /* Create the wait semaphore for subscriptions. */
        TEST_ASSERT_EQUAL_INT( true, IotSemaphore_Create( &publishWaitSem, 0, 1 ) );

        if( TEST_PROTECT() )
        {
            /* Establish the MQTT connection. */
            status = IotMqtt_Connect( &IotNetworkInfo,
                                      &connectInfo,
                                      IOT_TEST_MQTT_TIMEOUT_MS,
                                      &IotMqttConnection );
            TEST_ASSERT_EQUAL( IOT_MQTT_SUCCESS, status );

            if( TEST_PROTECT() )
            {
                /* Subscribe to the test topic filter. */
                callbackParam.expectedOperation = IOT_MQTT_SUBSCRIBE;
                status = IotMqtt_Subscribe( IotMqttConnection,
                                            &subscription,
                                            1,
                                            0,
                                            &callbackInfo,
                                            &( callbackParam.reference ) );

                if( IotSemaphore_TimedWait( &( callbackParam.waitSem ),
                                            IOT_TEST_MQTT_TIMEOUT_MS ) == false )
                {
                    TEST_FAIL_MESSAGE( "Timed out waiting for SUBSCRIBE to complete." );
                }

                /* Publish the message. */
                callbackParam.expectedOperation = IOT_MQTT_PUBLISH_TO_SERVER;
                status = IotMqtt_Publish( IotMqttConnection,
                                          &publishInfo,
                                          0,
                                          &callbackInfo,
                                          &( callbackParam.reference ) );

                if( IotSemaphore_TimedWait( &( callbackParam.waitSem ),
                                            IOT_TEST_MQTT_TIMEOUT_MS ) == false )
                {
                    TEST_FAIL_MESSAGE( "Timed out waiting for PUBLISH to complete." );
                }

                /* Wait for the message to be received. */
                if( IotSemaphore_TimedWait( &publishWaitSem,
                                            IOT_TEST_MQTT_TIMEOUT_MS ) == false )
                {
                    TEST_FAIL_MESSAGE( "Timed out waiting for subscription." );
                }

                /* Unsubscribe from the test topic filter. */
                callbackParam.expectedOperation = IOT_MQTT_UNSUBSCRIBE;
                status = IotMqtt_Unsubscribe( IotMqttConnection,
                                              &subscription,
                                              1,
                                              0,
                                              &callbackInfo,
                                              &( callbackParam.reference ) );

                if( IotSemaphore_TimedWait( &( callbackParam.waitSem ),
                                            IOT_TEST_MQTT_TIMEOUT_MS ) == false )
                {
                    TEST_FAIL_MESSAGE( "Timed out waiting for UNSUBSCRIBE to complete." );
                }
            }

            IotMqtt_Disconnect( IotMqttConnection, false );
        }

        IotSemaphore_Destroy( &publishWaitSem );
    }

    IotSemaphore_Destroy( &( callbackParam.waitSem ) );
}

/*-----------------------------------------------------------*/

/**
 * @brief Test that a LWT is published if an MQTT connection is unexpectedly closed.
 */
#if (DEFAULT_NETWORK == AWSIOT_NETWORK_TYPE_WIFI )
TEST( MQTT_System, LastWillAndTestament )
{
    bool lwtListenerCreated = false;
    IotMqttError_t status = IOT_MQTT_STATUS_PENDING;
    IotMqttNetworkInfo_t lwtNetworkInfo = IotNetworkInfo;
    IotNetworkConnectionAfr_t xConnection = IOT_NETWORK_CONNECTION_AFR_INITIALIZER;
    char pLwtListenerClientIdentifier[ _CLIENT_IDENTIFIER_MAX_LENGTH ] = { 0 };
    IotMqttConnection_t lwtListener = IOT_MQTT_CONNECTION_INITIALIZER;
    IotMqttConnectInfo_t lwtConnectInfo = IOT_MQTT_CONNECT_INFO_INITIALIZER,
                         connectInfo = IOT_MQTT_CONNECT_INFO_INITIALIZER;
    IotMqttSubscription_t willSubscription = IOT_MQTT_SUBSCRIPTION_INITIALIZER;
    IotMqttPublishInfo_t willInfo = IOT_MQTT_PUBLISH_INFO_INITIALIZER;
    IotNetworkServerInfoAfr_t xServerInfo = AWS_IOT_NETWORK_SERVER_INFO_AFR_INITIALIZER;
    IotNetworkCredentialsAfr_t xCredentials = AWS_IOT_NETWORK_CREDENTIALS_AFR_INITIALIZER;
    IotSemaphore_t waitSem;

    /* Create the wait semaphore. */
    TEST_ASSERT_EQUAL_INT( true, IotSemaphore_Create( &waitSem, 0, 1 ) );

    /* Generate a client identifier for LWT listener. */
    #ifndef IOT_TEST_MQTT_CLIENT_IDENTIFIER
        ( void ) snprintf( pLwtListenerClientIdentifier,
                           _CLIENT_IDENTIFIER_MAX_LENGTH,
                           "iotlwt%llu",
                           ( long long unsigned int ) IotClock_GetTimeMs() );
    #else
        ( void ) strncpy( pLwtListenerClientIdentifier,
                          IOT_TEST_MQTT_CLIENT_IDENTIFIER "LWT",
                          _CLIENT_IDENTIFIER_MAX_LENGTH );
    #endif

    if( TEST_PROTECT() )
    {
	   /* Disable ALPN if not using port 443. */
		if( clientcredentialMQTT_BROKER_PORT != 443 )
		{
			xCredentials.pAlpnProtos = NULL;
		}

        /* Establish an independent MQTT over TCP connection to receive a Last
         * Will and Testament message. */
        TEST_ASSERT_EQUAL( true,
                           (IotNetworkAfr.create(&xServerInfo,&xCredentials, &xConnection)) );
        lwtListenerCreated = true;

        if( TEST_PROTECT() )
        {
            lwtNetworkInfo.createNetworkConnection = false;
            lwtNetworkInfo.pNetworkConnection = &xConnection;
            lwtConnectInfo.cleanSession = true;
            lwtConnectInfo.pClientIdentifier = pLwtListenerClientIdentifier;
            lwtConnectInfo.clientIdentifierLength = ( uint16_t ) strlen( lwtConnectInfo.pClientIdentifier );

            status = IotMqtt_Connect( &lwtNetworkInfo,
                                      &lwtConnectInfo,
                                      IOT_TEST_MQTT_TIMEOUT_MS,
                                      &lwtListener );
            TEST_ASSERT_EQUAL( IOT_MQTT_SUCCESS, status );

            if( TEST_PROTECT() )
            {
                /* Register a subscription for the LWT. */
                willSubscription.pTopicFilter = IOT_TEST_MQTT_TOPIC_PREFIX "/LastWillAndTestament";
                willSubscription.topicFilterLength = ( uint16_t ) strlen( willSubscription.pTopicFilter );
                willSubscription.callback.function = _publishReceived;
                willSubscription.callback.param1 = &waitSem;

                status = IotMqtt_TimedSubscribe( lwtListener,
                                                 &willSubscription,
                                                 1,
                                                 0,
                                                 IOT_TEST_MQTT_TIMEOUT_MS );
                TEST_ASSERT_EQUAL( IOT_MQTT_SUCCESS, status );

                /* Create a connection that requests the LWT. */
                connectInfo.cleanSession = true;
                connectInfo.pClientIdentifier = _pClientIdentifier;
                connectInfo.clientIdentifierLength = ( uint16_t ) strlen( _pClientIdentifier );
                connectInfo.pWillInfo = &willInfo;

                willInfo.pTopicName = IOT_TEST_MQTT_TOPIC_PREFIX "/LastWillAndTestament";
                willInfo.topicNameLength = ( uint16_t ) strlen( willInfo.pTopicName );
                willInfo.pPayload = _pSamplePayload;
                willInfo.payloadLength = _samplePayloadLength;

                status = IotMqtt_Connect( &IotNetworkInfo,
                                          &connectInfo,
                                          IOT_TEST_MQTT_TIMEOUT_MS,
                                          &IotMqttConnection );
                TEST_ASSERT_EQUAL( IOT_MQTT_SUCCESS, status );

                /* Abruptly close the MQTT connection. This should cause the LWT
                 * to be sent to the LWT listener. */
                IotTestNetwork_Close( );
                IotMqtt_Disconnect( IotMqttConnection, true );
                IotTestNetwork_Destroy(  );

                /* Check that the LWT was received. */
                if( IotSemaphore_TimedWait( &waitSem,
                                            IOT_TEST_MQTT_TIMEOUT_MS ) == false )
                {
                    TEST_FAIL_MESSAGE( "Timed out waiting for Last Will and Testament." );
                }
            }

            IotMqtt_Disconnect( lwtListener, false );
            IotNetworkAfr.destroy( &xConnection );
            lwtListenerCreated = false;
        }

        if( lwtListenerCreated == true )
        {
        	IotNetworkAfr.close( &xConnection );
            IotNetworkAfr.destroy( &xConnection );
        }
    }

    IotSemaphore_Destroy( &waitSem );
}
#endif
/*-----------------------------------------------------------*/

/**
 * @brief Test that subscriptions from a previous session are restored.
 */
TEST( MQTT_System, RestorePreviousSession )
{
    IotMqttError_t status = IOT_MQTT_STATUS_PENDING;
    IotMqttConnectInfo_t connectInfo = IOT_MQTT_CONNECT_INFO_INITIALIZER;
    IotMqttSubscription_t subscription = IOT_MQTT_SUBSCRIPTION_INITIALIZER;
    IotMqttPublishInfo_t publishInfo = IOT_MQTT_PUBLISH_INFO_INITIALIZER;
    IotSemaphore_t waitSem;

    /* Create the wait semaphore for operations. */
    TEST_ASSERT_EQUAL_INT( true, IotSemaphore_Create( &waitSem, 0, 1 ) );

    /* Set the members of the connection info for a persistent session. */
    connectInfo.cleanSession = false;
    connectInfo.pClientIdentifier = _pClientIdentifier;
    connectInfo.clientIdentifierLength = ( uint16_t ) strlen( _pClientIdentifier );

    if( TEST_PROTECT() )
    {
        /* Establish a persistent MQTT connection. */
        status = IotMqtt_Connect( &IotNetworkInfo,
                                  &connectInfo,
                                  IOT_TEST_MQTT_TIMEOUT_MS,
                                  &IotMqttConnection );
        TEST_ASSERT_EQUAL( IOT_MQTT_SUCCESS, status );

        /* Add a subscription. */
        subscription.pTopicFilter = IOT_TEST_MQTT_TOPIC_PREFIX "/RestorePreviousSession";
        subscription.topicFilterLength = ( uint16_t ) strlen( subscription.pTopicFilter );
        subscription.callback.param1 = &waitSem;
        subscription.callback.function = _publishReceived;

        status = IotMqtt_TimedSubscribe( IotMqttConnection,
                                         &subscription,
                                         1,
                                         0,
                                         IOT_TEST_MQTT_TIMEOUT_MS );
        TEST_ASSERT_EQUAL( IOT_MQTT_SUCCESS, status );

        /* Disconnect the MQTT connection and clean up network connection. */
        IotMqtt_Disconnect( IotMqttConnection, false );
        IotTestNetwork_Cleanup();

        /* Re-establish the network connection. */
        TEST_ASSERT_EQUAL_INT( true, IotTestNetwork_Connect());

        /* Re-establish the MQTT connection with a previous session. */
        connectInfo.cleanSession = false;
        connectInfo.pPreviousSubscriptions = &subscription;
        connectInfo.previousSubscriptionCount = 1;
        status = IotMqtt_Connect( &IotNetworkInfo,
                                  &connectInfo,
                                  IOT_TEST_MQTT_TIMEOUT_MS,
                                  &IotMqttConnection );
        TEST_ASSERT_EQUAL( IOT_MQTT_SUCCESS, status );

        /* Publish a message to the subscription added in the previous session. */
        publishInfo.pTopicName = IOT_TEST_MQTT_TOPIC_PREFIX "/RestorePreviousSession";
        publishInfo.topicNameLength = ( uint16_t ) strlen( publishInfo.pTopicName );
        publishInfo.pPayload = _pSamplePayload;
        publishInfo.payloadLength = _samplePayloadLength;

        status = IotMqtt_TimedPublish( IotMqttConnection,
                                       &publishInfo,
                                       0,
                                       IOT_TEST_MQTT_TIMEOUT_MS );
        TEST_ASSERT_EQUAL( IOT_MQTT_SUCCESS, status );

        /* Wait for the message to be received. */
        if( IotSemaphore_TimedWait( &waitSem,
                                    IOT_TEST_MQTT_TIMEOUT_MS ) == false )
        {
            TEST_FAIL_MESSAGE( "Timed out waiting for message." );
        }

        /* Disconnect the MQTT connection. */
        IotMqtt_Disconnect( IotMqttConnection, false );
        IotTestNetwork_Cleanup();
    }
    else
    {
        /* Close network connection on test failure. */
        IotTestNetwork_Close( );
    }

    IotSemaphore_Destroy( &waitSem );

    if( TEST_PROTECT() )
    {
        /* Re-establish the network connection. */
        TEST_ASSERT_EQUAL_INT( true, IotTestNetwork_Connect() );

        /* After this test is finished, establish one more connection with a clean
         * session to clean up persistent sessions on the MQTT server created by this
         * test. */
        connectInfo.cleanSession = true;
        status = IotMqtt_Connect( &IotNetworkInfo,
                                  &connectInfo,
                                  IOT_TEST_MQTT_TIMEOUT_MS,
                                  &IotMqttConnection );
        TEST_ASSERT_EQUAL( IOT_MQTT_SUCCESS, status );

        IotMqtt_Disconnect( IotMqttConnection, false );
    }
}

/*-----------------------------------------------------------*/

/**
 * @brief Test that Wait can be safely invoked after Disconnect.
 */
TEST( MQTT_System, WaitAfterDisconnect )
{
    int32_t i = 0;
    IotMqttError_t status = IOT_MQTT_STATUS_PENDING;
    IotMqttConnectInfo_t connectInfo = IOT_MQTT_CONNECT_INFO_INITIALIZER;
    IotMqttPublishInfo_t publishInfo = IOT_MQTT_PUBLISH_INFO_INITIALIZER;
    IotMqttReference_t pPublishRef[ 3 ] = { IOT_MQTT_REFERENCE_INITIALIZER };

    /* Set the client identifier and length. */
    connectInfo.pClientIdentifier = _pClientIdentifier;
    connectInfo.clientIdentifierLength = ( uint16_t ) strlen( _pClientIdentifier );

    /* Set the members of the publish info. */
    publishInfo.qos = IOT_MQTT_QOS_1;
    publishInfo.pTopicName = IOT_TEST_MQTT_TOPIC_PREFIX "/WaitAfterDisconnect";
    publishInfo.topicNameLength = ( uint16_t ) strlen( publishInfo.pTopicName );
    publishInfo.pPayload = _pSamplePayload;
    publishInfo.payloadLength = _samplePayloadLength;
    publishInfo.retryLimit = 3;
    publishInfo.retryMs = 5000;

    /* Establish the MQTT connection. */
    status = IotMqtt_Connect( &IotNetworkInfo,
                              &connectInfo,
                              IOT_TEST_MQTT_TIMEOUT_MS,
                              &IotMqttConnection );
    TEST_ASSERT_EQUAL( IOT_MQTT_SUCCESS, status );

    if( TEST_PROTECT() )
    {
        /* Publish a sequence of messages. */
        for( i = 0; i < 3; i++ )
        {
            status = IotMqtt_Publish( IotMqttConnection,
                                      &publishInfo,
                                      IOT_MQTT_FLAG_WAITABLE,
                                      NULL,
                                      &( pPublishRef[ i ] ) );
            TEST_ASSERT_EQUAL( IOT_MQTT_STATUS_PENDING, status );
        }
    }

    /* Disconnect the MQTT connection. */
    IotMqtt_Disconnect( IotMqttConnection, false );

    if( TEST_PROTECT() )
    {
        /* Waiting on operations after a connection is disconnected should not crash.
         * The actual statuses of the PUBLISH operations may vary depending on the
         * timing of publish versus disconnect, so the statuses are not checked. */
        for( i = 0; i < 3; i++ )
        {
            status = IotMqtt_Wait( pPublishRef[ i ], 100 );
        }
    }
}

/*-----------------------------------------------------------*/

/**
 * @brief Test that API functions can be invoked from a callback for a completed
 * subscription operation.
 */
TEST( MQTT_System, SubscribeCompleteReentrancy )
{
    IotMqttError_t status = IOT_MQTT_STATUS_PENDING;
    IotMqttConnectInfo_t connectInfo = IOT_MQTT_CONNECT_INFO_INITIALIZER;
    IotMqttSubscription_t subscription = IOT_MQTT_SUBSCRIPTION_INITIALIZER;
    IotMqttCallbackInfo_t callbackInfo = IOT_MQTT_CALLBACK_INFO_INITIALIZER;

    /* Two semaphores are needed for this test: one for incoming PUBLISH and one
     * for test completion. */
    IotSemaphore_t pWaitSemaphores[ 2 ];

    /* Create the semaphores. */
    TEST_ASSERT_EQUAL_INT( true, IotSemaphore_Create( &( pWaitSemaphores[ 0 ] ), 0, 1 ) );

    if( TEST_PROTECT() )
    {
        TEST_ASSERT_EQUAL_INT( true, IotSemaphore_Create( &( pWaitSemaphores[ 1 ] ), 0, 1 ) );

        if( TEST_PROTECT() )
        {
            connectInfo.pClientIdentifier = _pClientIdentifier;
            connectInfo.clientIdentifierLength = ( uint16_t ) strlen( _pClientIdentifier );

            if( TEST_PROTECT() )
            {
                /* Establish the MQTT connection. */
                status = IotMqtt_Connect( &IotNetworkInfo,
                                          &connectInfo,
                                          IOT_TEST_MQTT_TIMEOUT_MS,
                                          &IotMqttConnection );
                TEST_ASSERT_EQUAL( IOT_MQTT_SUCCESS, status );

                /* Subscribe with a completion callback. */
                subscription.qos = 1;
                subscription.pTopicFilter = IOT_TEST_MQTT_TOPIC_PREFIX "/Reentrancy";
                subscription.topicFilterLength = ( uint16_t ) strlen( subscription.pTopicFilter );
                subscription.callback.function = _publishReceived;
                subscription.callback.param1 = &( pWaitSemaphores[ 0 ] );

                callbackInfo.function = _reentrantCallback;
                callbackInfo.param1 = pWaitSemaphores;

                status = IotMqtt_Subscribe( IotMqttConnection,
                                            &subscription,
                                            1,
                                            0,
                                            &callbackInfo,
                                            NULL );
                TEST_ASSERT_EQUAL( IOT_MQTT_STATUS_PENDING, status );

                /* Wait for the reentrant callback to complete. */
                if( IotSemaphore_TimedWait( &( pWaitSemaphores[ 1 ] ),
                                            IOT_TEST_MQTT_TIMEOUT_MS ) == false )
                {
                    TEST_FAIL_MESSAGE( "Timed out waiting for reentrant callback." );
                }
            }
        }

        IotSemaphore_Destroy( &( pWaitSemaphores[ 1 ] ) );
    }

    IotSemaphore_Destroy( &( pWaitSemaphores[ 0 ] ) );
}

/*-----------------------------------------------------------*/

/**
 * @brief Test that API functions can be invoked from a callback for an incoming
 * PUBLISH.
 */
TEST( MQTT_System, IncomingPublishReentrancy )
{
    IotMqttError_t status = IOT_MQTT_STATUS_PENDING;
    IotMqttConnectInfo_t connectInfo = IOT_MQTT_CONNECT_INFO_INITIALIZER;
    IotMqttSubscription_t pSubscription[ 2 ] = { IOT_MQTT_SUBSCRIPTION_INITIALIZER };
    IotMqttPublishInfo_t publishInfo = IOT_MQTT_PUBLISH_INFO_INITIALIZER;

    /* Two semaphores are needed for this test: one for incoming PUBLISH and one
     * for test completion. */
    IotSemaphore_t pWaitSemaphores[ 2 ];

    /* Create the semaphores. */
    TEST_ASSERT_EQUAL_INT( true, IotSemaphore_Create( &( pWaitSemaphores[ 0 ] ), 0, 1 ) );

    if( TEST_PROTECT() )
    {
        TEST_ASSERT_EQUAL_INT( true, IotSemaphore_Create( &( pWaitSemaphores[ 1 ] ), 0, 1 ) );

        if( TEST_PROTECT() )
        {
            connectInfo.pClientIdentifier = _pClientIdentifier;
            connectInfo.clientIdentifierLength = ( uint16_t ) strlen( _pClientIdentifier );

            if( TEST_PROTECT() )
            {
                /* Establish the MQTT connection. */
                status = IotMqtt_Connect( &IotNetworkInfo,
                                          &connectInfo,
                                          IOT_TEST_MQTT_TIMEOUT_MS,
                                          &IotMqttConnection );
                TEST_ASSERT_EQUAL( IOT_MQTT_SUCCESS, status );

                /* Subscribe with to the test topics. */
                pSubscription[ 0 ].qos = IOT_MQTT_QOS_1;
                pSubscription[ 0 ].pTopicFilter = IOT_TEST_MQTT_TOPIC_PREFIX "/IncomingPublishReentrancy";
                pSubscription[ 0 ].topicFilterLength = ( uint16_t ) strlen( pSubscription[ 0 ].pTopicFilter );
                pSubscription[ 0 ].callback.function = _reentrantCallback;
                pSubscription[ 0 ].callback.param1 = pWaitSemaphores;

                pSubscription[ 1 ].qos = IOT_MQTT_QOS_1;
                pSubscription[ 1 ].pTopicFilter = IOT_TEST_MQTT_TOPIC_PREFIX "/Reentrancy";
                pSubscription[ 1 ].topicFilterLength = ( uint16_t ) strlen( pSubscription[ 1 ].pTopicFilter );
                pSubscription[ 1 ].callback.function = _publishReceived;
                pSubscription[ 1 ].callback.param1 = &( pWaitSemaphores[ 0 ] );

                status = IotMqtt_TimedSubscribe( IotMqttConnection,
                                                 pSubscription,
                                                 2,
                                                 0,
                                                 IOT_TEST_MQTT_TIMEOUT_MS );
                TEST_ASSERT_EQUAL( IOT_MQTT_SUCCESS, status );

                /* Publish a message to the test topic. */
                publishInfo.qos = IOT_MQTT_QOS_1;
                publishInfo.pTopicName = IOT_TEST_MQTT_TOPIC_PREFIX "/IncomingPublishReentrancy";
                publishInfo.topicNameLength = ( uint16_t ) strlen( publishInfo.pTopicName );
                publishInfo.pPayload = _pSamplePayload;
                publishInfo.payloadLength = _samplePayloadLength;
                publishInfo.retryLimit = 3;
                publishInfo.retryMs = 5000;

                status = IotMqtt_TimedPublish( IotMqttConnection,
                                               &publishInfo,
                                               0,
                                               IOT_TEST_MQTT_TIMEOUT_MS );
                TEST_ASSERT_EQUAL( IOT_MQTT_SUCCESS, status );

                /* Wait for the reentrant callback to complete. */
                if( IotSemaphore_TimedWait( &( pWaitSemaphores[ 1 ] ),
                                            IOT_TEST_MQTT_TIMEOUT_MS ) == false )
                {
                    TEST_FAIL_MESSAGE( "Timed out waiting for reentrant callback." );
                }
            }
        }

        IotSemaphore_Destroy( &( pWaitSemaphores[ 1 ] ) );
    }

    IotSemaphore_Destroy( &( pWaitSemaphores[ 0 ] ) );
}

/*-----------------------------------------------------------*/
