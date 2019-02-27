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

/* MQTT internal include. */
#include "private/iot_mqtt_internal.h"

/* Platform layer includes. */
#include "platform/iot_clock.h"
#include "platform/iot_threads.h"

/* Test network header include. */
#include IOT_TEST_NETWORK_HEADER

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

/* Network functions used by the tests, declared and implemented in one of
 * the test network function files. */
extern bool IotTest_NetworkSetup( void );
extern void IotTest_NetworkCleanup( void );
extern bool IotTest_NetworkConnect( IotTestNetworkConnection_t * pNewConnection,
                                    IotMqttConnection_t * pMqttConnection );
extern IotNetworkError_t IotTest_NetworkClose( void * pNetworkConnection );
extern void IotTest_NetworkDestroy( void * pConnection );

/* Network variables used by the tests, declared in one of the test network
 * function files. */
extern IotMqttNetIf_t _IotTestNetworkInterface;
extern IotMqttConnection_t _IotTestMqttConnection;

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
                                         uint16_t * pPacketIdentifier )
{
    _publishSerializerOverride = true;

    return _IotMqtt_SerializePublish( pPublishInfo,
                                      pPublishPacket,
                                      pPacketSize,
                                      pPacketIdentifier );
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
static void _subscribePublishWait( IotMqttQos_t qos )
{
    IotMqttError_t status = IOT_MQTT_STATUS_PENDING;
    IotMqttNetIf_t networkInterface = _IotTestNetworkInterface;
    IotMqttConnectInfo_t connectInfo = IOT_MQTT_CONNECT_INFO_INITIALIZER;
    IotMqttSubscription_t subscription = IOT_MQTT_SUBSCRIPTION_INITIALIZER;
    IotMqttPublishInfo_t publishInfo = IOT_MQTT_PUBLISH_INFO_INITIALIZER;
    IotSemaphore_t waitSem;

    /* Set the serializer overrides. */
    networkInterface.freePacket = _freePacket;
    networkInterface.serialize.connect = _serializeConnect;
    networkInterface.serialize.publish = _serializePublish;
    networkInterface.serialize.puback = _serializePuback;
    networkInterface.serialize.subscribe = _serializeSubscribe;
    networkInterface.serialize.unsubscribe = _serializeUnsubscribe;
    networkInterface.serialize.disconnect = _serializeDisconnect;

    /* Create the wait semaphore. */
    TEST_ASSERT_EQUAL_INT( true, IotSemaphore_Create( &waitSem, 0, 1 ) );

    if( TEST_PROTECT() )
    {
        /* Set the members of the MQTT connection info. */
        connectInfo.cleanSession = true;
        connectInfo.pClientIdentifier = _pClientIdentifier;
        connectInfo.clientIdentifierLength = ( uint16_t ) strlen( _pClientIdentifier );

        /* Establish the MQTT connection. */
        status = IotMqtt_Connect( &_IotTestMqttConnection,
                                  &networkInterface,
                                  &connectInfo,
                                  IOT_TEST_MQTT_TIMEOUT_MS );
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
            status = IotMqtt_TimedSubscribe( _IotTestMqttConnection,
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
            status = IotMqtt_TimedPublish( _IotTestMqttConnection,
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
            status = IotMqtt_TimedUnsubscribe( _IotTestMqttConnection,
                                               &subscription,
                                               1,
                                               0,
                                               IOT_TEST_MQTT_TIMEOUT_MS );
            TEST_ASSERT_EQUAL( IOT_MQTT_SUCCESS, status );
        }

        /* Close the MQTT connection. */
        IotMqtt_Disconnect( _IotTestMqttConnection, false );
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

    /* Initialize the MQTT library. */
    if( IotMqtt_Init() != IOT_MQTT_SUCCESS )
    {
        TEST_FAIL_MESSAGE( "Failed to initialize MQTT library." );
    }

    /* Set up the network stack. */
    if( IotTest_NetworkSetup() == false )
    {
        TEST_FAIL_MESSAGE( "Failed to set up network connection." );
    }

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

/*-----------------------------------------------------------*/

/**
 * @brief Test tear down for MQTT system tests.
 */
TEST_TEAR_DOWN( MQTT_System )
{
    /* Clean up the MQTT library. */
    IotMqtt_Cleanup();

    /* Clean up the network stack. */
    IotTest_NetworkCleanup();
    _IotTestMqttConnection = IOT_MQTT_CONNECTION_INITIALIZER;
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
    _subscribePublishWait( IOT_MQTT_QOS_0 );
}

/*-----------------------------------------------------------*/

/**
 * @brief Subscribe-publish-wait (QoS 1).
 */
TEST( MQTT_System, SubscribePublishWaitQoS1 )
{
    _subscribePublishWait( IOT_MQTT_QOS_1 );
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
            status = IotMqtt_Connect( &_IotTestMqttConnection,
                                      &_IotTestNetworkInterface,
                                      &connectInfo,
                                      IOT_TEST_MQTT_TIMEOUT_MS );
            TEST_ASSERT_EQUAL( IOT_MQTT_SUCCESS, status );

            if( TEST_PROTECT() )
            {
                /* Subscribe to the test topic filter. */
                callbackParam.expectedOperation = IOT_MQTT_SUBSCRIBE;
                status = IotMqtt_Subscribe( _IotTestMqttConnection,
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
                status = IotMqtt_Publish( _IotTestMqttConnection,
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
                status = IotMqtt_Unsubscribe( _IotTestMqttConnection,
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

            IotMqtt_Disconnect( _IotTestMqttConnection, false );
        }

        IotSemaphore_Destroy( &publishWaitSem );
    }

    IotSemaphore_Destroy( &( callbackParam.waitSem ) );
}

/*-----------------------------------------------------------*/

/**
 * @brief Test that a LWT is published if an MQTT connection is unexpectedly closed.
 */
TEST( MQTT_System, LastWillAndTestament )
{
    bool lwtListenerCreated = false;
    IotMqttError_t status = IOT_MQTT_STATUS_PENDING;
    IotMqttNetIf_t lwtNetIf = _IotTestNetworkInterface;
    char pLwtListenerClientIdentifier[ _CLIENT_IDENTIFIER_MAX_LENGTH ] = { 0 };
    IotMqttConnection_t lwtListener = IOT_MQTT_CONNECTION_INITIALIZER;
    IotMqttConnectInfo_t lwtConnectInfo = IOT_MQTT_CONNECT_INFO_INITIALIZER,
                         connectInfo = IOT_MQTT_CONNECT_INFO_INITIALIZER;
    IotMqttSubscription_t willSubscription = IOT_MQTT_SUBSCRIPTION_INITIALIZER;
    IotMqttPublishInfo_t willInfo = IOT_MQTT_PUBLISH_INFO_INITIALIZER;
    IotTestNetworkConnection_t lwtListenerConnection = IOT_TEST_NETWORK_CONNECTION_INITIALIZER;
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
        /* Establish an independent MQTT over TCP connection to receive a Last
         * Will and Testament message. */
        TEST_ASSERT_EQUAL( true,
                           IotTest_NetworkConnect( &lwtListenerConnection,
                                                   &lwtListener ) );
        lwtListenerCreated = true;

        if( TEST_PROTECT() )
        {
            lwtNetIf.pDisconnectContext = &lwtListenerConnection;
            lwtNetIf.pSendContext = &lwtListenerConnection;
            lwtConnectInfo.cleanSession = true;
            lwtConnectInfo.pClientIdentifier = pLwtListenerClientIdentifier;
            lwtConnectInfo.clientIdentifierLength = ( uint16_t ) strlen( lwtConnectInfo.pClientIdentifier );

            status = IotMqtt_Connect( &lwtListener,
                                      &lwtNetIf,
                                      &lwtConnectInfo,
                                      IOT_TEST_MQTT_TIMEOUT_MS );
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

                status = IotMqtt_Connect( &_IotTestMqttConnection,
                                          &_IotTestNetworkInterface,
                                          &connectInfo,
                                          IOT_TEST_MQTT_TIMEOUT_MS );
                TEST_ASSERT_EQUAL( IOT_MQTT_SUCCESS, status );

                /* Abruptly close the MQTT connection. This should cause the LWT
                 * to be sent to the LWT listener. */
                IotTest_NetworkClose( NULL );
                IotMqtt_Disconnect( _IotTestMqttConnection, true );
                IotTest_NetworkDestroy( NULL );

                /* Check that the LWT was received. */
                if( IotSemaphore_TimedWait( &waitSem,
                                            IOT_TEST_MQTT_TIMEOUT_MS ) == false )
                {
                    TEST_FAIL_MESSAGE( "Timed out waiting for Last Will and Testament." );
                }
            }

            IotMqtt_Disconnect( lwtListener, false );
            IotTest_NetworkDestroy( &lwtListenerConnection );
            lwtListenerCreated = false;
        }

        if( lwtListenerCreated == true )
        {
            IotTest_NetworkClose( &lwtListenerConnection );
            IotTest_NetworkDestroy( &lwtListenerConnection );
        }
    }

    IotSemaphore_Destroy( &waitSem );
}

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
        status = IotMqtt_Connect( &_IotTestMqttConnection,
                                  &_IotTestNetworkInterface,
                                  &connectInfo,
                                  IOT_TEST_MQTT_TIMEOUT_MS );
        TEST_ASSERT_EQUAL( IOT_MQTT_SUCCESS, status );

        /* Add a subscription. */
        subscription.pTopicFilter = IOT_TEST_MQTT_TOPIC_PREFIX "/RestorePreviousSession";
        subscription.topicFilterLength = ( uint16_t ) strlen( subscription.pTopicFilter );
        subscription.callback.param1 = &waitSem;
        subscription.callback.function = _publishReceived;

        status = IotMqtt_TimedSubscribe( _IotTestMqttConnection,
                                         &subscription,
                                         1,
                                         0,
                                         IOT_TEST_MQTT_TIMEOUT_MS );
        TEST_ASSERT_EQUAL( IOT_MQTT_SUCCESS, status );

        /* Disconnect the MQTT connection and clean up network connection. */
        IotMqtt_Disconnect( _IotTestMqttConnection, false );
        IotTest_NetworkCleanup();

        /* Re-establish the network connection. */
        TEST_ASSERT_EQUAL_INT( true, IotTest_NetworkSetup() );

        /* Re-establish the MQTT connection with a previous session. */
        connectInfo.cleanSession = false;
        connectInfo.pPreviousSubscriptions = &subscription;
        connectInfo.previousSubscriptionCount = 1;
        status = IotMqtt_Connect( &_IotTestMqttConnection,
                                  &_IotTestNetworkInterface,
                                  &connectInfo,
                                  IOT_TEST_MQTT_TIMEOUT_MS );
        TEST_ASSERT_EQUAL( IOT_MQTT_SUCCESS, status );

        /* Publish a message to the subscription added in the previous session. */
        publishInfo.pTopicName = IOT_TEST_MQTT_TOPIC_PREFIX "/RestorePreviousSession";
        publishInfo.topicNameLength = ( uint16_t ) strlen( publishInfo.pTopicName );
        publishInfo.pPayload = _pSamplePayload;
        publishInfo.payloadLength = _samplePayloadLength;

        status = IotMqtt_TimedPublish( _IotTestMqttConnection,
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
        IotMqtt_Disconnect( _IotTestMqttConnection, false );
        IotTest_NetworkCleanup();
    }
    else
    {
        /* Close network connection on test failure. */
        IotTest_NetworkClose( NULL );
    }

    IotSemaphore_Destroy( &waitSem );

    if( TEST_PROTECT() )
    {
        /* Re-establish the network connection. */
        TEST_ASSERT_EQUAL_INT( true, IotTest_NetworkSetup() );

        /* After this test is finished, establish one more connection with a clean
         * session to clean up persistent sessions on the MQTT server created by this
         * test. */
        connectInfo.cleanSession = true;
        status = IotMqtt_Connect( &_IotTestMqttConnection,
                                  &_IotTestNetworkInterface,
                                  &connectInfo,
                                  IOT_TEST_MQTT_TIMEOUT_MS );
        TEST_ASSERT_EQUAL( IOT_MQTT_SUCCESS, status );

        IotMqtt_Disconnect( _IotTestMqttConnection, false );
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
    status = IotMqtt_Connect( &_IotTestMqttConnection,
                              &_IotTestNetworkInterface,
                              &connectInfo,
                              IOT_TEST_MQTT_TIMEOUT_MS );
    TEST_ASSERT_EQUAL( IOT_MQTT_SUCCESS, status );

    if( TEST_PROTECT() )
    {
        /* Publish a sequence of messages. */
        for( i = 0; i < 3; i++ )
        {
            status = IotMqtt_Publish( _IotTestMqttConnection,
                                      &publishInfo,
                                      IOT_MQTT_FLAG_WAITABLE,
                                      NULL,
                                      &( pPublishRef[ i ] ) );
            TEST_ASSERT_EQUAL( IOT_MQTT_STATUS_PENDING, status );
        }
    }

    /* Disconnect the MQTT connection. */
    IotMqtt_Disconnect( _IotTestMqttConnection, false );

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
                status = IotMqtt_Connect( &_IotTestMqttConnection,
                                          &_IotTestNetworkInterface,
                                          &connectInfo,
                                          IOT_TEST_MQTT_TIMEOUT_MS );
                TEST_ASSERT_EQUAL( IOT_MQTT_SUCCESS, status );

                /* Subscribe with a completion callback. */
                subscription.qos = 1;
                subscription.pTopicFilter = IOT_TEST_MQTT_TOPIC_PREFIX "/Reentrancy";
                subscription.topicFilterLength = ( uint16_t ) strlen( subscription.pTopicFilter );
                subscription.callback.function = _publishReceived;
                subscription.callback.param1 = &( pWaitSemaphores[ 0 ] );

                callbackInfo.function = _reentrantCallback;
                callbackInfo.param1 = pWaitSemaphores;

                status = IotMqtt_Subscribe( _IotTestMqttConnection,
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
                status = IotMqtt_Connect( &_IotTestMqttConnection,
                                          &_IotTestNetworkInterface,
                                          &connectInfo,
                                          IOT_TEST_MQTT_TIMEOUT_MS );
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

                status = IotMqtt_TimedSubscribe( _IotTestMqttConnection,
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

                status = IotMqtt_TimedPublish( _IotTestMqttConnection,
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
