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
 * @file iot_tests_mqtt_receive.c
 * @brief Tests for the function @ref mqtt_function_receivecallback.
 */

/* Build using a config header, if provided. */
#ifdef IOT_CONFIG_FILE
    #include IOT_CONFIG_FILE
#endif

/* Standard includes. */
#include <string.h>

/* Platform layer includes. */
#include "platform/iot_threads.h"

/* MQTT internal include. */
#include "private/iot_mqtt_internal.h"

/* Test framework includes. */
#include "unity_fixture.h"

/* MQTT test access include. */
#include "iot_test_access_mqtt.h"

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
 * @cond DOXYGEN_IGNORE
 * Doxygen should ignore this section.
 *
 * Provide default malloc and free functions in dynamic memory mode.
 */
#if !defined( IOT_STATIC_MEMORY_ONLY ) || ( IOT_STATIC_MEMORY_ONLY == 0 )
    #ifndef IotTest_Malloc
        #include <stdlib.h>
        #define IotTest_Malloc    malloc
    #endif
    #ifndef IotTest_Free
        #include <stdlib.h>
        #define IotTest_Free    free
    #endif
#endif /* if !defined( IOT_STATIC_MEMORY_ONLY ) || ( IOT_STATIC_MEMORY_ONLY == 0 ) */
/** @endcond */

/** @brief Default CONNACK packet for the receive tests. */
static const uint8_t _pConnackTemplate[] = { 0x20, 0x02, 0x00, 0x00 };
/** @brief Default PUBLISH packet for the receive tests. */
static const uint8_t _pPublishTemplate[] =
{
    0x30, 0x8d, 0x02, 0x00, 0x0b, 0x2f, 0x74, 0x65, 0x73, 0x74, 0x2f, 0x74, 0x6f, 0x70, 0x69, 0x63,
    0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f,
    0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0x1a, 0x1b, 0x1c, 0x1d, 0x1e, 0x1f,
    0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27, 0x28, 0x29, 0x2a, 0x2b, 0x2c, 0x2d, 0x2e, 0x2f,
    0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x3a, 0x3b, 0x3c, 0x3d, 0x3e, 0x3f,
    0x40, 0x41, 0x42, 0x43, 0x44, 0x45, 0x46, 0x47, 0x48, 0x49, 0x4a, 0x4b, 0x4c, 0x4d, 0x4e, 0x4f,
    0x50, 0x51, 0x52, 0x53, 0x54, 0x55, 0x56, 0x57, 0x58, 0x59, 0x5a, 0x5b, 0x5c, 0x5d, 0x5e, 0x5f,
    0x60, 0x61, 0x62, 0x63, 0x64, 0x65, 0x66, 0x67, 0x68, 0x69, 0x6a, 0x6b, 0x6c, 0x6d, 0x6e, 0x6f,
    0x70, 0x71, 0x72, 0x73, 0x74, 0x75, 0x76, 0x77, 0x78, 0x79, 0x7a, 0x7b, 0x7c, 0x7d, 0x7e, 0x7f,
    0x80, 0x81, 0x82, 0x83, 0x84, 0x85, 0x86, 0x87, 0x88, 0x89, 0x8a, 0x8b, 0x8c, 0x8d, 0x8e, 0x8f,
    0x90, 0x91, 0x92, 0x93, 0x94, 0x95, 0x96, 0x97, 0x98, 0x99, 0x9a, 0x9b, 0x9c, 0x9d, 0x9e, 0x9f,
    0xa0, 0xa1, 0xa2, 0xa3, 0xa4, 0xa5, 0xa6, 0xa7, 0xa8, 0xa9, 0xaa, 0xab, 0xac, 0xad, 0xae, 0xaf,
    0xb0, 0xb1, 0xb2, 0xb3, 0xb4, 0xb5, 0xb6, 0xb7, 0xb8, 0xb9, 0xba, 0xbb, 0xbc, 0xbd, 0xbe, 0xbf,
    0xc0, 0xc1, 0xc2, 0xc3, 0xc4, 0xc5, 0xc6, 0xc7, 0xc8, 0xc9, 0xca, 0xcb, 0xcc, 0xcd, 0xce, 0xcf,
    0xd0, 0xd1, 0xd2, 0xd3, 0xd4, 0xd5, 0xd6, 0xd7, 0xd8, 0xd9, 0xda, 0xdb, 0xdc, 0xdd, 0xde, 0xdf,
    0xe0, 0xe1, 0xe2, 0xe3, 0xe4, 0xe5, 0xe6, 0xe7, 0xe8, 0xe9, 0xea, 0xeb, 0xec, 0xed, 0xee, 0xef,
    0xf0, 0xf1, 0xf2, 0xf3, 0xf4, 0xf5, 0xf6, 0xf7, 0xf8, 0xf9, 0xfa, 0xfb, 0xfc, 0xfd, 0xfe, 0xff
};
/** @brief Default PUBACK packet for the receive tests. */
static const uint8_t _pPubackTemplate[] = { 0x40, 0x02, 0x00, 0x01 };
/** @brief Default SUBACK packet for the receive tests. */
static const uint8_t _pSubackTemplate[] = { 0x90, 0x05, 0x00, 0x01, 0x00, 0x01, 0x02 };
/** @brief Default UNSUBACK packet for the receive tests. */
static const uint8_t _pUnsubackTemplate[] = { 0xb0, 0x02, 0x00, 0x01 };
/** @brief Default PINGRESP packet for the receive tests. */
static const uint8_t _pPingrespTemplate[] = { 0xd0, 0x00 };

/*-----------------------------------------------------------*/

/**
 * @brief Topic name and filter used in the tests.
 */
#define _TEST_TOPIC_NAME             "/test/topic"

/**
 * @brief Length of #_TEST_TOPIC_NAME.
 */
#define _TEST_TOPIC_LENGTH           ( ( uint16_t ) ( sizeof( _TEST_TOPIC_NAME ) - 1 ) )

/**
 * @brief Timeout for waiting on a PUBLISH callback.
 */
#define _PUBLISH_CALLBACK_TIMEOUT    ( 1000 )

/**
 * @brief Size of data stream in #TEST_MQTT_Unit_Receive_DataStream_.
 */
#define _DATA_STREAM_SIZE            \
    ( sizeof( _pConnackTemplate ) +  \
      sizeof( _pSubackTemplate ) +   \
      sizeof( _pPublishTemplate ) +  \
      sizeof( _pUnsubackTemplate ) + \
      sizeof( _pPingrespTemplate ) )

/**
 * @brief Number of PUBLISH messages in the stream for #TEST_MQTT_Unit_Receive_PublishStream_
 * and #TEST_MQTT_Unit_Receive_PublishInvalidStream_.
 */
#define _PUBLISH_STREAM_COUNT    ( 3 )

/**
 * @brief Declare a buffer holding a packet and its size.
 */
#if IOT_STATIC_MEMORY_ONLY == 1
    #define _DECLARE_PACKET( pTemplate, bufferName, sizeName ) \
    uint8_t bufferName[ sizeof( pTemplate ) ] = { 0 };         \
    const size_t sizeName = sizeof( pTemplate );               \
    ( void ) memcpy( bufferName, pTemplate, sizeName );        \
    TEST_ASSERT_EQUAL_PTR( NULL, _mallocWrapper( 0 ) );
#else
    #define _DECLARE_PACKET( pTemplate, bufferName, sizeName )    \
    uint8_t * bufferName = _mallocWrapper( sizeof( pTemplate ) ); \
    TEST_ASSERT_NOT_EQUAL( NULL, bufferName );                    \
    const size_t sizeName = sizeof( pTemplate );                  \
    ( void ) memcpy( bufferName, pTemplate, sizeName );
#endif /* if IOT_STATIC_MEMORY_ONLY == 1 */

/**
 * @brief Initializer for operations in the tests.
 */
#define _INITIALIZE_OPERATION( name )                                                                 \
    {                                                                                                 \
        .link = { 0 }, .incomingPublish = false, .pMqttConnection = _pMqttConnection,                 \
        .job = { 0 }, .jobReference = 1, .operation = name, .flags = IOT_MQTT_FLAG_WAITABLE,          \
        .packetIdentifier = 1, .pMqttPacket = NULL, .packetSize = 0, .notify = { .callback = { 0 } }, \
        .status = IOT_MQTT_STATUS_PENDING, .retry = { 0 }                                             \
    }

/*-----------------------------------------------------------*/

/**
 * @brief The MQTT connection shared by all the tests.
 */
static _mqttConnection_t * _pMqttConnection = NULL;

/**
 * @brief Synchronizes malloc and free, which may be called from different threads.
 */
static IotSemaphore_t _mallocSemaphore;

/**
 * @brief Tracks whether a deserializer override was called for a test.
 */
static bool _deserializeOverrideCalled = false;

/**
 * @brief Tracks whether #_getPacketType has been called.
 */
static bool _getPacketTypeCalled = false;

/*-----------------------------------------------------------*/

/**
 * @brief Wrapper for IotTest_Malloc.
 */
static void * _mallocWrapper( size_t size )
{
    void * pBuffer = NULL;

    ( void ) size;

    #if !defined( IOT_STATIC_MEMORY_ONLY ) || ( IOT_STATIC_MEMORY_ONLY == 0 )
        pBuffer = IotTest_Malloc( size );

        /* Decrement the malloc semaphore. */
        if( pBuffer != NULL )
    #endif
    {
        if( ( IotSemaphore_TryWait( &_mallocSemaphore ) == true ) &&
            ( IotSemaphore_GetCount( &_mallocSemaphore ) > 0 ) )
        {
            /* If the malloc semaphore value isn't what's expected, return NULL. */
            IotTest_Free( pBuffer );
            pBuffer = NULL;
        }
    }

    return pBuffer;
}


/*-----------------------------------------------------------*/

/**
 * @brief Get packet type function override.
 */
static uint8_t _getPacketType( const uint8_t * pPacket,
                               size_t packetSize )
{
    _getPacketTypeCalled = true;

    return _IotMqtt_GetPacketType( pPacket, packetSize );
}

/*-----------------------------------------------------------*/

/**
 * @brief Wrapper for IotTest_Free.
 */
static void _freeWrapper( void * ptr )
{
    /* This function should do nothing in static memory mode. */
    #if IOT_STATIC_MEMORY_ONLY == 1
        ( void ) ptr;
    #else
        IotTest_Free( ptr );
    #endif

    IotSemaphore_Post( &_mallocSemaphore );
}

/*-----------------------------------------------------------*/

/**
 * @brief Deserializer override for CONNACK.
 */
static IotMqttError_t _deserializeConnack( const uint8_t * pConnackStart,
                                           size_t dataLength,
                                           size_t * pBytesProcessed )
{
    _deserializeOverrideCalled = true;

    return _IotMqtt_DeserializeConnack( pConnackStart,
                                        dataLength,
                                        pBytesProcessed );
}

/*-----------------------------------------------------------*/

/**
 * @brief Deserializer override for PUBLISH.
 */
static IotMqttError_t _deserializePublish( const uint8_t * pPublishStart,
                                           size_t dataLength,
                                           IotMqttPublishInfo_t * pOutput,
                                           uint16_t * pPacketIdentifier,
                                           size_t * pBytesProcessed )
{
    _deserializeOverrideCalled = true;

    return _IotMqtt_DeserializePublish( pPublishStart,
                                        dataLength,
                                        pOutput,
                                        pPacketIdentifier,
                                        pBytesProcessed );
}

/*-----------------------------------------------------------*/

/**
 * @brief Deserializer override for PUBACK.
 */
static IotMqttError_t _deserializePuback( const uint8_t * pPubackStart,
                                          size_t dataLength,
                                          uint16_t * pPacketIdentifier,
                                          size_t * pBytesProcessed )
{
    _deserializeOverrideCalled = true;

    return _IotMqtt_DeserializePuback( pPubackStart,
                                       dataLength,
                                       pPacketIdentifier,
                                       pBytesProcessed );
}

/*-----------------------------------------------------------*/

/**
 * @brief Deserializer override for SUBACK.
 */
static IotMqttError_t _deserializeSuback( IotMqttConnection_t mqttConnection,
                                          const uint8_t * pSubackStart,
                                          size_t dataLength,
                                          uint16_t * pPacketIdentifier,
                                          size_t * pBytesProcessed )
{
    _deserializeOverrideCalled = true;

    return _IotMqtt_DeserializeSuback( mqttConnection,
                                       pSubackStart,
                                       dataLength,
                                       pPacketIdentifier,
                                       pBytesProcessed );
}

/*-----------------------------------------------------------*/

/**
 * @brief Deserializer override for UNSUBACK.
 */
static IotMqttError_t _deserializeUnsuback( const uint8_t * pUnsubackStart,
                                            size_t dataLength,
                                            uint16_t * pPacketIdentifier,
                                            size_t * pBytesProcessed )
{
    _deserializeOverrideCalled = true;

    return _IotMqtt_DeserializeUnsuback( pUnsubackStart,
                                         dataLength,
                                         pPacketIdentifier,
                                         pBytesProcessed );
}

/*-----------------------------------------------------------*/

/**
 * @brief Deserializer override for PINGRESP.
 */
static IotMqttError_t _deserializePingresp( const uint8_t * pPingrespStart,
                                            size_t dataLength,
                                            size_t * pBytesProcessed )
{
    _deserializeOverrideCalled = true;

    return _IotMqtt_DeserializePingresp( pPingrespStart,
                                         dataLength,
                                         pBytesProcessed );
}

/*-----------------------------------------------------------*/

/**
 * @brief Reset the status of an #_mqttOperation_t and push it to the queue of
 * MQTT operations awaiting network response.
 */
static void _operationResetAndPush( _mqttOperation_t * pOperation )
{
    pOperation->status = IOT_MQTT_STATUS_PENDING;
    pOperation->jobReference = 1;
    IotQueue_Enqueue( &( _pMqttConnection->pendingResponse ), &( pOperation->link ) );
}

/*-----------------------------------------------------------*/

/**
 * @brief Process a non-PUBLISH buffer and check the result.
 */
static bool _processBuffer( const _mqttOperation_t * pOperation,
                            const uint8_t * pBuffer,
                            size_t bufferSize,
                            int32_t expectedBytesProcessed,
                            IotMqttError_t expectedResult )
{
    bool status = false;

    /* Call the receive callback on pBuffer. */
    int32_t bytesProcessed = IotMqtt_ReceiveCallback( ( IotMqttConnection_t * ) &_pMqttConnection,
                                                      NULL,
                                                      pBuffer,
                                                      bufferSize,
                                                      0,
                                                      _freeWrapper );

    /* Free pBuffer if the receive callback wasn't expected to free it. */
    if( expectedBytesProcessed <= 0 )
    {
        _freeWrapper( ( void * ) pBuffer );
    }

    /* Check expected bytes processed. */
    status = ( expectedBytesProcessed == bytesProcessed );

    /* Check expected result if operation is given. */
    if( pOperation != NULL )
    {
        status = status && ( expectedResult == pOperation->status );
    }

    return status;
}

/*-----------------------------------------------------------*/

/**
 * @brief Process a PUBLISH message and check the result.
 */
static bool _processPublish( const uint8_t * pPublish,
                             size_t publishSize,
                             int32_t expectedBytesProcessed,
                             uint32_t expectedInvokeCount )
{
    IotSemaphore_t invokeCount;
    uint32_t finalInvokeCount = 0, i = 0;
    int32_t bytesProcessed = 0;
    bool waitResult = true;

    /* Create a semaphore that counts how many times the publish callback is invoked. */
    if( IotSemaphore_Create( &invokeCount, 0, expectedInvokeCount ) == false )
    {
        return false;
    }

    /* Set the subscription parameter. */
    if( IotListDouble_IsEmpty( &( _pMqttConnection->subscriptionList ) ) == false )
    {
        _mqttSubscription_t * pSubscription = IotLink_Container( _mqttSubscription_t,
                                                                 IotListDouble_PeekHead( &( _pMqttConnection->subscriptionList ) ),
                                                                 link );
        pSubscription->callback.param1 = &invokeCount;
    }

    /* Call the receive callback on pPublish. */
    bytesProcessed = IotMqtt_ReceiveCallback( ( IotMqttConnection_t * ) &_pMqttConnection,
                                              NULL,
                                              pPublish,
                                              publishSize,
                                              0,
                                              _freeWrapper );

    /* Check how many times the publish callback is invoked. */
    for( i = 0; i < expectedInvokeCount; i++ )
    {
        waitResult = IotSemaphore_TimedWait( &invokeCount,
                                             _PUBLISH_CALLBACK_TIMEOUT );

        if( waitResult == false )
        {
            break;
        }
    }

    /* Ensure that the invoke count semaphore has a value of 0, then destroy the
     * semaphore. */
    finalInvokeCount = IotSemaphore_GetCount( &invokeCount );
    IotSemaphore_Destroy( &invokeCount );

    /* Check results against expected values. */
    return ( finalInvokeCount == 0 ) &&
           ( expectedBytesProcessed == bytesProcessed ) &&
           ( waitResult == true );
}

/*-----------------------------------------------------------*/

/**
 * @brief Called when a PUBLISH message is "received".
 */
static void _publishCallback( void * param1,
                              IotMqttCallbackParam_t * pPublish )
{
    IotSemaphore_t * pInvokeCount = ( IotSemaphore_t * ) param1;

    /* QoS 2 is valid for these tests, but currently unsupported by the MQTT library.
     * Change the QoS to 0 so that QoS validation passes. */
    pPublish->message.info.qos = IOT_MQTT_QOS_0;

    /* Check that the parameters to this function are valid. */
    if( ( _IotMqtt_ValidatePublish( _AWS_IOT_MQTT_SERVER,
                                    &( pPublish->message.info ) ) == true ) &&
        ( pPublish->message.info.topicNameLength == _TEST_TOPIC_LENGTH ) &&
        ( strncmp( _TEST_TOPIC_NAME, pPublish->message.info.pTopicName, _TEST_TOPIC_LENGTH ) == 0 ) )
    {
        IotSemaphore_Post( pInvokeCount );
    }
}

/*-----------------------------------------------------------*/

/**
 * @brief A PUBACK serializer function that does nothing, but always returns failure.
 *
 * Prevents any tests on QoS 1 PUBLISH packets from sending any PUBACKS.
 */
static IotMqttError_t _serializePuback( uint16_t packetIdentifier,
                                        uint8_t ** pPubackPacket,
                                        size_t * pPacketSize )
{
    ( void ) packetIdentifier;
    ( void ) pPubackPacket;
    ( void ) pPacketSize;

    return IOT_MQTT_NO_MEMORY;
}

/*-----------------------------------------------------------*/

/**
 * @brief Test group for MQTT Receive tests.
 */
TEST_GROUP( MQTT_Unit_Receive );

/*-----------------------------------------------------------*/

/**
 * @brief Test setup for MQTT Receive tests.
 */
TEST_SETUP( MQTT_Unit_Receive )
{
    IotMqttNetIf_t networkInterface = IOT_MQTT_NETIF_INITIALIZER;
    IotMqttSubscription_t subscription = IOT_MQTT_SUBSCRIPTION_INITIALIZER;

    /* Set the deserializer overrides. */
    networkInterface.deserialize.connack = _deserializeConnack;
    networkInterface.deserialize.publish = _deserializePublish;
    networkInterface.deserialize.puback = _deserializePuback;
    networkInterface.deserialize.suback = _deserializeSuback;
    networkInterface.deserialize.unsuback = _deserializeUnsuback;
    networkInterface.deserialize.pingresp = _deserializePingresp;
    networkInterface.getPacketType = _getPacketType;

    /* Create the memory allocation semaphore. */
    TEST_ASSERT_EQUAL_INT( true, IotSemaphore_Create( &_mallocSemaphore,
                                                      1,
                                                      1 ) );

    /* Initialize the MQTT library. */
    TEST_ASSERT_EQUAL( IOT_MQTT_SUCCESS, IotMqtt_Init() );

    /* Initialize the MQTT connection used by the tests. */
    _pMqttConnection = IotTestMqtt_createMqttConnection( _AWS_IOT_MQTT_SERVER,
                                                         &networkInterface,
                                                         0 );
    TEST_ASSERT_NOT_EQUAL( NULL, _pMqttConnection );

    /* Set the members of the subscription. */
    subscription.pTopicFilter = _TEST_TOPIC_NAME;
    subscription.topicFilterLength = _TEST_TOPIC_LENGTH;
    subscription.callback.function = _publishCallback;

    /* Add the subscription to the MQTT connection. */
    TEST_ASSERT_EQUAL( IOT_MQTT_SUCCESS, _IotMqtt_AddSubscriptions( _pMqttConnection,
                                                                    1,
                                                                    &subscription,
                                                                    1 ) );

    /* Clear the deserialize override called flag. */
    _deserializeOverrideCalled = false;
    _getPacketTypeCalled = false;
}

/*-----------------------------------------------------------*/

/**
 * @brief Test tear down for MQTT Receive tests.
 */
TEST_TEAR_DOWN( MQTT_Unit_Receive )
{
    /* Clean up resources taken in test setup. */
    IotMqtt_Disconnect( _pMqttConnection, true );
    IotMqtt_Cleanup();
    IotSemaphore_Destroy( &_mallocSemaphore );
    _pMqttConnection = NULL;

    /* Check that the tests used a deserializer override. */
    TEST_ASSERT_EQUAL_INT( true, _deserializeOverrideCalled );
    TEST_ASSERT_EQUAL_INT( true, _getPacketTypeCalled );
}

/*-----------------------------------------------------------*/

/**
 * @brief Test group runner for MQTT Receive tests.
 */
TEST_GROUP_RUNNER( MQTT_Unit_Receive )
{
    RUN_TEST_CASE( MQTT_Unit_Receive, DecodeRemainingLength );
    RUN_TEST_CASE( MQTT_Unit_Receive, InvalidPacket );
    RUN_TEST_CASE( MQTT_Unit_Receive, DataStream );
    RUN_TEST_CASE( MQTT_Unit_Receive, ConnackValid );
    RUN_TEST_CASE( MQTT_Unit_Receive, ConnackInvalid );
    RUN_TEST_CASE( MQTT_Unit_Receive, PublishValid );
    RUN_TEST_CASE( MQTT_Unit_Receive, PublishInvalid );
    RUN_TEST_CASE( MQTT_Unit_Receive, PublishStream );
    RUN_TEST_CASE( MQTT_Unit_Receive, PublishInvalidStream );
    RUN_TEST_CASE( MQTT_Unit_Receive, PubackValid );
    RUN_TEST_CASE( MQTT_Unit_Receive, PubackInvalid );
    RUN_TEST_CASE( MQTT_Unit_Receive, SubackValid );
    RUN_TEST_CASE( MQTT_Unit_Receive, SubackInvalid );
    RUN_TEST_CASE( MQTT_Unit_Receive, UnsubackValid );
    RUN_TEST_CASE( MQTT_Unit_Receive, UnsubackInvalid );
    RUN_TEST_CASE( MQTT_Unit_Receive, Pingresp );
}

/*-----------------------------------------------------------*/

/**
 * @brief Tests the function #_decodeRemainingLength.
 */
TEST( MQTT_Unit_Receive, DecodeRemainingLength )
{
    const uint8_t * pEnd = NULL;
    size_t decodedLength = 0;

    /* Decode 0, which has a 1-byte representation. */
    {
        uint8_t pRemainingLength[ 4 ] = { 0 };

        TEST_ASSERT_EQUAL( IOT_MQTT_SUCCESS,
                           IotTestMqtt_decodeRemainingLength( pRemainingLength,
                                                              &pEnd,
                                                              &decodedLength ) );
        TEST_ASSERT_EQUAL_PTR( pEnd, pRemainingLength + 1 );
        TEST_ASSERT_EQUAL( 0, decodedLength );
    }

    /* Decode 129, which has a 2-byte representation. */
    {
        uint8_t pRemainingLength[ 4 ] = { 0x81, 0x01, 0x00, 0x00 };

        TEST_ASSERT_EQUAL( IOT_MQTT_SUCCESS,
                           IotTestMqtt_decodeRemainingLength( pRemainingLength,
                                                              &pEnd,
                                                              &decodedLength ) );
        TEST_ASSERT_EQUAL_PTR( pEnd, pRemainingLength + 2 );
        TEST_ASSERT_EQUAL( 129, decodedLength );
    }

    /* Decode 16,386, which has a 3-byte representation. */
    {
        uint8_t pRemainingLength[ 4 ] = { 0x82, 0x80, 0x01, 0x00 };

        TEST_ASSERT_EQUAL( IOT_MQTT_SUCCESS,
                           IotTestMqtt_decodeRemainingLength( pRemainingLength,
                                                              &pEnd,
                                                              &decodedLength ) );
        TEST_ASSERT_EQUAL_PTR( pEnd, pRemainingLength + 3 );
        TEST_ASSERT_EQUAL( 16386, decodedLength );
    }

    /* Decode 268,435,455, which has a 4-byte representation. This value is the
     * largest representable value. */
    {
        uint8_t pRemainingLength[ 4 ] = { 0xff, 0xff, 0xff, 0x7f };

        TEST_ASSERT_EQUAL( IOT_MQTT_SUCCESS,
                           IotTestMqtt_decodeRemainingLength( pRemainingLength,
                                                              &pEnd,
                                                              &decodedLength ) );
        TEST_ASSERT_EQUAL_PTR( pEnd, pRemainingLength + 4 );
        TEST_ASSERT_EQUAL( 268435455, decodedLength );
    }

    /* Attempt to decode an invalid value where all continuation bits are set. */
    {
        uint8_t pRemainingLength[ 4 ] = { 0xff, 0xff, 0xff, 0x8f };

        TEST_ASSERT_EQUAL( IOT_MQTT_BAD_PARAMETER,
                           IotTestMqtt_decodeRemainingLength( pRemainingLength,
                                                              &pEnd,
                                                              &decodedLength ) );
    }

    /* Attempt to decode a 4-byte representation of 0. According to the spec,
     * this representation is not valid. */
    {
        uint8_t pRemainingLength[ 4 ] = { 0x80, 0x80, 0x80, 0x00 };

        TEST_ASSERT_EQUAL( IOT_MQTT_BAD_PARAMETER,
                           IotTestMqtt_decodeRemainingLength( pRemainingLength,
                                                              &pEnd,
                                                              &decodedLength ) );
    }

    /* Test tear down for this test group checks that deserializer overrides
     * were called. However, this test does not use any deserializer overrides;
     * set these values to true so that the checks pass. */
    _deserializeOverrideCalled = true;
    _getPacketTypeCalled = true;
}

/*-----------------------------------------------------------*/

/**
 * @brief Tests the behavior of @ref mqtt_function_receivecallback with an
 * invalid control packet type.
 */
TEST( MQTT_Unit_Receive, InvalidPacket )
{
    int32_t bytesProcessed = 0;
    uint8_t invalidPacket = 0xf0;

    /* Processing a control packet 0xf is a protocol violation. */
    bytesProcessed = IotMqtt_ReceiveCallback( ( IotMqttConnection_t * ) &_pMqttConnection,
                                              NULL,
                                              &invalidPacket,
                                              sizeof( uint8_t ),
                                              0,
                                              NULL );
    TEST_ASSERT_EQUAL( -1, bytesProcessed );

    /* This test should not have called any deserializer. Set the deserialize
     * override called flag to true so that the check for it passes. */
    TEST_ASSERT_EQUAL_INT( false, _deserializeOverrideCalled );
    _deserializeOverrideCalled = true;
}

/*-----------------------------------------------------------*/

/**
 * @brief Tests the behavior of @ref mqtt_function_receivecallback with an stream
 * of data (instead of discrete packets).
 */
TEST( MQTT_Unit_Receive, DataStream )
{
    size_t copyOffset = 0, processOffset = 0;
    int32_t bytesProcessed = 0;

    /* Allocate the data stream depending on the memory allocation mode. */
    #if IOT_STATIC_MEMORY_ONLY == 1
        uint8_t pDataStream[ _DATA_STREAM_SIZE ] = { 0 };
        TEST_ASSERT_EQUAL_PTR( NULL, _mallocWrapper( 0 ) );
    #else
        uint8_t * pDataStream = _mallocWrapper( _DATA_STREAM_SIZE );
        TEST_ASSERT_NOT_EQUAL( NULL, pDataStream );
    #endif

    /* Construct the data stream by placing a CONNACK, SUBACK, PUBLISH, UNSUBACK,
     * and PINGRESP in it. */
    ( void ) memcpy( pDataStream, _pConnackTemplate, sizeof( _pConnackTemplate ) );
    copyOffset += sizeof( _pConnackTemplate );
    ( void ) memcpy( pDataStream + copyOffset, _pSubackTemplate, sizeof( _pSubackTemplate ) );
    copyOffset += sizeof( _pSubackTemplate );
    ( void ) memcpy( pDataStream + copyOffset, _pPublishTemplate, sizeof( _pPublishTemplate ) );
    copyOffset += sizeof( _pPublishTemplate );
    ( void ) memcpy( pDataStream + copyOffset, _pUnsubackTemplate, sizeof( _pUnsubackTemplate ) );
    copyOffset += sizeof( _pUnsubackTemplate );
    ( void ) memcpy( pDataStream + copyOffset, _pPingrespTemplate, sizeof( _pPingrespTemplate ) );
    TEST_ASSERT_EQUAL( _DATA_STREAM_SIZE, copyOffset + sizeof( _pPingrespTemplate ) );

    /* Passing an offset greater than dataLength should not process anything. */
    bytesProcessed = IotMqtt_ReceiveCallback( ( IotMqttConnection_t * ) &_pMqttConnection,
                                              NULL,
                                              pDataStream,
                                              4,
                                              5,
                                              _freeWrapper );
    TEST_ASSERT_EQUAL( 0, bytesProcessed );

    /* The first call to process 64 bytes should only process the CONNACK and
     * SUBACK. */
    bytesProcessed = IotMqtt_ReceiveCallback( ( IotMqttConnection_t * ) &_pMqttConnection,
                                              NULL,
                                              pDataStream,
                                              processOffset + 64,
                                              0,
                                              _freeWrapper );
    TEST_ASSERT_EQUAL( 11, bytesProcessed );
    processOffset += ( size_t ) bytesProcessed;

    /* A second call to process 64 bytes should not process anything, as the
     * PUBLISH is incomplete. */
    bytesProcessed = IotMqtt_ReceiveCallback( ( IotMqttConnection_t * ) &_pMqttConnection,
                                              NULL,
                                              pDataStream,
                                              processOffset + 64,
                                              processOffset,
                                              _freeWrapper );
    TEST_ASSERT_EQUAL( 0, bytesProcessed );

    /* A call to process 273 bytes should process the PUBLISH packet (272 bytes). */
    TEST_ASSERT_EQUAL_INT( true, _processPublish( pDataStream + processOffset,
                                                  273,
                                                  272,
                                                  1 ) );
    processOffset += 272;

    /* A call to process 5 bytes should only process the UNSUBACK (4 bytes). */
    bytesProcessed = IotMqtt_ReceiveCallback( ( IotMqttConnection_t * ) &_pMqttConnection,
                                              NULL,
                                              pDataStream,
                                              processOffset + 5,
                                              processOffset,
                                              _freeWrapper );
    TEST_ASSERT_EQUAL( 4, bytesProcessed );
    processOffset += ( size_t ) bytesProcessed;

    /* Process the last 2 bytes (PINGRESP). */
    bytesProcessed = IotMqtt_ReceiveCallback( ( IotMqttConnection_t * ) &_pMqttConnection,
                                              NULL,
                                              pDataStream,
                                              processOffset + 2,
                                              processOffset,
                                              _freeWrapper );
    TEST_ASSERT_EQUAL( 2, bytesProcessed );

    /* Wait for the buffer to be freed. */
    TEST_ASSERT_EQUAL_INT( true, IotSemaphore_TimedWait( &_mallocSemaphore,
                                                         _PUBLISH_CALLBACK_TIMEOUT ) );

    /* Check that the entire buffer was processed. */
    TEST_ASSERT_EQUAL( _DATA_STREAM_SIZE, processOffset + ( size_t ) bytesProcessed );
}

/*-----------------------------------------------------------*/

/**
 * @brief Tests the behavior of @ref mqtt_function_receivecallback with a spec-compliant
 * CONNACK.
 */
TEST( MQTT_Unit_Receive, ConnackValid )
{
    uint8_t i = 0;
    _mqttOperation_t connect = _INITIALIZE_OPERATION( IOT_MQTT_CONNECT );

    /* Create the wait semaphore so notifications don't crash. The value of
     * this semaphore will not be checked, so the maxValue argument is arbitrary. */
    TEST_ASSERT_EQUAL_INT( true, IotSemaphore_Create( &( connect.notify.waitSemaphore ),
                                                      0,
                                                      10 ) );

    /* Even though no CONNECT is in the receive queue, 4 bytes should still be
     * processed (should not crash). */
    {
        _DECLARE_PACKET( _pConnackTemplate, pConnack, connackSize );
        TEST_ASSERT_EQUAL_INT( true, _processBuffer( &connect,
                                                     pConnack,
                                                     connackSize,
                                                     ( int32_t ) connackSize,
                                                     IOT_MQTT_STATUS_PENDING ) );
    }

    /* Process a valid, successful CONNACK with no SP flag. */
    {
        _DECLARE_PACKET( _pConnackTemplate, pConnack, connackSize );
        _operationResetAndPush( &connect );
        TEST_ASSERT_EQUAL_INT( true, _processBuffer( &connect,
                                                     pConnack,
                                                     connackSize,
                                                     ( int32_t ) connackSize,
                                                     IOT_MQTT_SUCCESS ) );
    }

    /* Process a valid, successful CONNACK with SP flag set. */
    {
        _DECLARE_PACKET( _pConnackTemplate, pConnack, connackSize );
        pConnack[ 2 ] = 0x01;
        _operationResetAndPush( &connect );
        TEST_ASSERT_EQUAL_INT( true, _processBuffer( &connect,
                                                     pConnack,
                                                     connackSize,
                                                     ( int32_t ) connackSize,
                                                     IOT_MQTT_SUCCESS ) );
    }

    /* Check each of the CONNACK failure codes, which range from 1 to 5. */
    for( i = 0x01; i < 0x06; i++ )
    {
        _DECLARE_PACKET( _pConnackTemplate, pConnack, connackSize );

        /* Set the CONNECT state and code. */
        _operationResetAndPush( &connect );
        pConnack[ 3 ] = i;

        /* Push a CONNECT operation to the queue and process a CONNACK. */
        TEST_ASSERT_EQUAL_INT( true, _processBuffer( &connect,
                                                     pConnack,
                                                     connackSize,
                                                     ( int32_t ) connackSize,
                                                     IOT_MQTT_SERVER_REFUSED ) );
    }

    IotSemaphore_Destroy( &( connect.notify.waitSemaphore ) );
}

/*-----------------------------------------------------------*/

/**
 * @brief Tests the behavior of @ref mqtt_function_receivecallback with a
 * CONNACK that doesn't comply to MQTT spec.
 */
TEST( MQTT_Unit_Receive, ConnackInvalid )
{
    _mqttOperation_t connect = _INITIALIZE_OPERATION( IOT_MQTT_CONNECT );

    /* Create the wait semaphore so notifications don't crash. The value of
     * this semaphore will not be checked, so the maxValue argument is arbitrary. */
    TEST_ASSERT_EQUAL_INT( true, IotSemaphore_Create( &( connect.notify.waitSemaphore ),
                                                      0,
                                                      10 ) );

    /* An incomplete CONNACK should not be processed, and no status should be set. */
    {
        _DECLARE_PACKET( _pConnackTemplate, pConnack, connackSize );
        _operationResetAndPush( &connect );
        TEST_ASSERT_EQUAL_INT( true, _processBuffer( &connect,
                                                     pConnack,
                                                     connackSize - 1,
                                                     0,
                                                     IOT_MQTT_STATUS_PENDING ) );
    }

    /* The CONNACK control packet type must be 0x20. */
    {
        _DECLARE_PACKET( _pConnackTemplate, pConnack, connackSize );
        pConnack[ 0 ] = 0x21;
        TEST_ASSERT_EQUAL_INT( true, _processBuffer( &connect,
                                                     pConnack,
                                                     connackSize,
                                                     -1,
                                                     IOT_MQTT_BAD_RESPONSE ) );
    }

    /* A CONNACK must have a remaining length of 2. */
    {
        _DECLARE_PACKET( _pConnackTemplate, pConnack, connackSize );
        pConnack[ 1 ] = 0x03;
        _operationResetAndPush( &connect );
        TEST_ASSERT_EQUAL_INT( true, _processBuffer( &connect,
                                                     pConnack,
                                                     connackSize,
                                                     -1,
                                                     IOT_MQTT_BAD_RESPONSE ) );
    }

    /* The reserved bits in CONNACK must be 0. */
    {
        _DECLARE_PACKET( _pConnackTemplate, pConnack, connackSize );
        pConnack[ 2 ] = 0x80;
        _operationResetAndPush( &connect );
        TEST_ASSERT_EQUAL_INT( true, _processBuffer( &connect,
                                                     pConnack,
                                                     connackSize,
                                                     -1,
                                                     IOT_MQTT_BAD_RESPONSE ) );
    }

    /* The fourth byte of CONNACK must be 0 if the SP flag is set. */
    {
        _DECLARE_PACKET( _pConnackTemplate, pConnack, connackSize );
        pConnack[ 2 ] = 0x01;
        pConnack[ 3 ] = 0x01;
        _operationResetAndPush( &connect );
        TEST_ASSERT_EQUAL_INT( true, _processBuffer( &connect,
                                                     pConnack,
                                                     connackSize,
                                                     -1,
                                                     IOT_MQTT_BAD_RESPONSE ) );
    }

    /* CONNACK return codes cannot be above 5. */
    {
        _DECLARE_PACKET( _pConnackTemplate, pConnack, connackSize );
        pConnack[ 3 ] = 0x06;
        _operationResetAndPush( &connect );
        TEST_ASSERT_EQUAL_INT( true, _processBuffer( &connect,
                                                     pConnack,
                                                     connackSize,
                                                     -1,
                                                     IOT_MQTT_BAD_RESPONSE ) );
    }

    IotSemaphore_Destroy( &( connect.notify.waitSemaphore ) );
}

/*-----------------------------------------------------------*/

/**
 * @brief Tests the behavior of @ref mqtt_function_receivecallback with a spec-compliant
 * PUBLISH.
 */
TEST( MQTT_Unit_Receive, PublishValid )
{
    /* Set the PUBACK serializer function. This serializer function always returns
     * failure to prevent any PUBACK packets from actually being sent. */
    _pMqttConnection->network.serialize.puback = _serializePuback;

    /* Process a valid QoS 0 PUBLISH. */
    {
        _DECLARE_PACKET( _pPublishTemplate, pPublish, publishSize );
        TEST_ASSERT_EQUAL_INT( true, _processPublish( pPublish,
                                                      publishSize,
                                                      ( int32_t ) publishSize,
                                                      1 ) );
        TEST_ASSERT_EQUAL_INT( true, IotSemaphore_TimedWait( &_mallocSemaphore,
                                                             _PUBLISH_CALLBACK_TIMEOUT ) );
    }

    /* Process a valid QoS 1 PUBLISH. Prevent an attempt to send PUBACK by
     * making no memory available for the PUBACK. */
    {
        _DECLARE_PACKET( _pPublishTemplate, pPublish, publishSize );
        pPublish[ 0 ] = 0x32;
        TEST_ASSERT_EQUAL_INT( true, _processPublish( pPublish,
                                                      publishSize,
                                                      ( int32_t ) publishSize,
                                                      1 ) );
        TEST_ASSERT_EQUAL_INT( true, IotSemaphore_TimedWait( &_mallocSemaphore,
                                                             _PUBLISH_CALLBACK_TIMEOUT ) );
    }

    /* Process a valid QoS 2 PUBLISH. */
    {
        _DECLARE_PACKET( _pPublishTemplate, pPublish, publishSize );
        pPublish[ 0 ] = 0x34;
        TEST_ASSERT_EQUAL_INT( true, _processPublish( pPublish,
                                                      publishSize,
                                                      ( int32_t ) publishSize,
                                                      1 ) );
        TEST_ASSERT_EQUAL_INT( true, IotSemaphore_TimedWait( &_mallocSemaphore,
                                                             _PUBLISH_CALLBACK_TIMEOUT ) );
    }

    /* Process a valid PUBLISH with DUP flag set. */
    {
        _DECLARE_PACKET( _pPublishTemplate, pPublish, publishSize );
        pPublish[ 0 ] = 0x38;
        TEST_ASSERT_EQUAL_INT( true, _processPublish( pPublish,
                                                      publishSize,
                                                      ( int32_t ) publishSize,
                                                      1 ) );
        TEST_ASSERT_EQUAL_INT( true, IotSemaphore_TimedWait( &_mallocSemaphore,
                                                             _PUBLISH_CALLBACK_TIMEOUT ) );
    }

    /* Remove the subscription. Even though there is no matching subscription,
     * all bytes of the PUBLISH should still be processed (should not crash). */
    {
        _DECLARE_PACKET( _pPublishTemplate, pPublish, publishSize );
        IotListDouble_RemoveAll( &( _pMqttConnection->subscriptionList ),
                                 IotMqtt_FreeSubscription,
                                 offsetof( _mqttSubscription_t, link ) );

        TEST_ASSERT_EQUAL_INT( true, _processPublish( pPublish,
                                                      publishSize,
                                                      ( int32_t ) publishSize,
                                                      0 ) );
        TEST_ASSERT_EQUAL_INT( true, IotSemaphore_TimedWait( &_mallocSemaphore,
                                                             _PUBLISH_CALLBACK_TIMEOUT ) );
    }
}

/*-----------------------------------------------------------*/

/**
 * @brief Tests the behavior of @ref mqtt_function_receivecallback with a PUBLIS
 * that doesn't comply to MQTT spec.
 */
TEST( MQTT_Unit_Receive, PublishInvalid )
{
    /* Attempting to process a packet smaller than 5 bytes should result in no
     * bytes processed. 5 bytes is the minimum size of a PUBLISH. */
    {
        _DECLARE_PACKET( _pPublishTemplate, pPublish, publishSize );
        TEST_ASSERT_EQUAL_INT( true, _processPublish( pPublish,
                                                      4,
                                                      0,
                                                      0 ) );
        _freeWrapper( pPublish );
        TEST_ASSERT_EQUAL_UINT32( 1, IotSemaphore_GetCount( &_mallocSemaphore ) );
    }

    /* The PUBLISH cannot have a QoS of 3. */
    {
        _DECLARE_PACKET( _pPublishTemplate, pPublish, publishSize );
        pPublish[ 0 ] = 0x36;
        TEST_ASSERT_EQUAL_INT( true, _processPublish( pPublish,
                                                      publishSize,
                                                      -1,
                                                      0 ) );
        _freeWrapper( pPublish );
        TEST_ASSERT_EQUAL_UINT32( 1, IotSemaphore_GetCount( &_mallocSemaphore ) );
    }

    /* Attempt to process a PUBLISH with an invalid "Remaining length". */
    {
        _DECLARE_PACKET( _pPublishTemplate, pPublish, publishSize );
        pPublish[ 1 ] = 0xff;
        pPublish[ 2 ] = 0xff;
        pPublish[ 3 ] = 0xff;
        pPublish[ 4 ] = 0xff;
        TEST_ASSERT_EQUAL_INT( true, _processPublish( pPublish,
                                                      publishSize,
                                                      -1,
                                                      0 ) );
        _freeWrapper( pPublish );
        TEST_ASSERT_EQUAL_UINT32( 1, IotSemaphore_GetCount( &_mallocSemaphore ) );
    }

    /* Attempt to process a PUBLISH larger than the size of the data stream. */
    {
        _DECLARE_PACKET( _pPublishTemplate, pPublish, publishSize );
        pPublish[ 2 ] = 0x52;
        TEST_ASSERT_EQUAL_INT( true, _processPublish( pPublish,
                                                      publishSize,
                                                      0,
                                                      0 ) );
        _freeWrapper( pPublish );
        TEST_ASSERT_EQUAL_UINT32( 1, IotSemaphore_GetCount( &_mallocSemaphore ) );
    }

    /* Attempt to process a PUBLISH with a "Remaining length" smaller than the
     * spec allows. */
    {
        /* QoS 0. */
        _DECLARE_PACKET( _pPublishTemplate, pPublish0, publish0Size );
        pPublish0[ 1 ] = 0x02;
        TEST_ASSERT_EQUAL_INT( true, _processPublish( pPublish0,
                                                      publish0Size,
                                                      -1,
                                                      0 ) );
        _freeWrapper( pPublish0 );
        TEST_ASSERT_EQUAL_UINT32( 1, IotSemaphore_GetCount( &_mallocSemaphore ) );

        /* QoS 1. */
        _DECLARE_PACKET( _pPublishTemplate, pPublish1, publish1Size );
        pPublish1[ 0 ] = 0x32;
        pPublish1[ 1 ] = 0x04;
        TEST_ASSERT_EQUAL_INT( true, _processPublish( pPublish1,
                                                      publish1Size,
                                                      -1,
                                                      0 ) );
        _freeWrapper( pPublish1 );
        TEST_ASSERT_EQUAL_UINT32( 1, IotSemaphore_GetCount( &_mallocSemaphore ) );
    }

    /* Attempt to process a PUBLISH with a "Remaining length" smaller than the
     * end of the variable header. */
    {
        _DECLARE_PACKET( _pPublishTemplate, pPublish, publishSize );
        pPublish[ 1 ] = 0x0a;
        TEST_ASSERT_EQUAL_INT( true, _processPublish( pPublish,
                                                      publishSize,
                                                      -1,
                                                      0 ) );
        _freeWrapper( pPublish );
        TEST_ASSERT_EQUAL_UINT32( 1, IotSemaphore_GetCount( &_mallocSemaphore ) );
    }

    /* Attempt to process a PUBLISH with packet identifier 0. */
    {
        _DECLARE_PACKET( _pPublishTemplate, pPublish, publishSize );
        pPublish[ 0 ] = 0x32;
        pPublish[ 17 ] = 0x00;
        TEST_ASSERT_EQUAL_INT( true, _processPublish( pPublish,
                                                      publishSize,
                                                      -1,
                                                      0 ) );
        _freeWrapper( pPublish );
        TEST_ASSERT_EQUAL_UINT32( 1, IotSemaphore_GetCount( &_mallocSemaphore ) );
    }
}

/*-----------------------------------------------------------*/

/**
 * @brief Tests the behavior of @ref mqtt_function_receivecallback with a stream
 * of incoming PUBLISH messages.
 */
TEST( MQTT_Unit_Receive, PublishStream )
{
    size_t i = 0;

    /* Allocate the data stream depending on the memory allocation mode. */
    #if IOT_STATIC_MEMORY_ONLY == 1
        uint8_t pPublishStream[ _PUBLISH_STREAM_COUNT * sizeof( _pPublishTemplate ) ] = { 0 };
        TEST_ASSERT_EQUAL_PTR( NULL, _mallocWrapper( 0 ) );
    #else
        uint8_t * pPublishStream = _mallocWrapper( _PUBLISH_STREAM_COUNT * sizeof( _pPublishTemplate ) );
        TEST_ASSERT_NOT_EQUAL( NULL, pPublishStream );
    #endif

    /* Fill the data stream with PUBLISH messages. */
    for( i = 0; i < _PUBLISH_STREAM_COUNT; i++ )
    {
        ( void ) memcpy( pPublishStream + i * sizeof( _pPublishTemplate ),
                         _pPublishTemplate,
                         sizeof( _pPublishTemplate ) );
    }

    /* Process a stream that contains one complete and one partial PUBLISH message. */
    TEST_ASSERT_EQUAL_INT( true, _processPublish( pPublishStream,
                                                  sizeof( _pPublishTemplate ) + 1,
                                                  sizeof( _pPublishTemplate ),
                                                  1 ) );
    TEST_ASSERT_EQUAL_INT( false, IotSemaphore_TimedWait( &_mallocSemaphore,
                                                          _PUBLISH_CALLBACK_TIMEOUT ) );

    /* Process the complete stream of PUBLISH messages. */
    TEST_ASSERT_EQUAL_INT( true, _processPublish( pPublishStream,
                                                  _PUBLISH_STREAM_COUNT * sizeof( _pPublishTemplate ),
                                                  _PUBLISH_STREAM_COUNT * sizeof( _pPublishTemplate ),
                                                  _PUBLISH_STREAM_COUNT ) );
    TEST_ASSERT_EQUAL_INT( true, IotSemaphore_TimedWait( &_mallocSemaphore,
                                                         _PUBLISH_CALLBACK_TIMEOUT ) );
}

/*-----------------------------------------------------------*/

/**
 * @brief Tests the behavior of @ref mqtt_function_receivecallback with a stream
 * of incoming PUBLISH messages that is invalid.
 */
TEST( MQTT_Unit_Receive, PublishInvalidStream )
{
    size_t i = 0;

    /* Allocate the data stream depending on the memory allocation mode. */
    #if IOT_STATIC_MEMORY_ONLY == 1
        uint8_t pPublishStream[ _PUBLISH_STREAM_COUNT * sizeof( _pPublishTemplate ) + 1 ] = { 0 };
        TEST_ASSERT_EQUAL_PTR( NULL, _mallocWrapper( 0 ) );
    #else
        uint8_t * pPublishStream = _mallocWrapper( _PUBLISH_STREAM_COUNT * sizeof( _pPublishTemplate ) + 1 );
        TEST_ASSERT_NOT_EQUAL( NULL, pPublishStream );
    #endif

    /* Fill the data stream with PUBLISH messages. */
    for( i = 0; i < _PUBLISH_STREAM_COUNT; i++ )
    {
        ( void ) memcpy( pPublishStream + i * sizeof( _pPublishTemplate ),
                         _pPublishTemplate,
                         sizeof( _pPublishTemplate ) );
    }

    /* Place an invalid byte at the end of the stream. */
    pPublishStream[ _PUBLISH_STREAM_COUNT * sizeof( _pPublishTemplate ) ] = 0xff;

    /* Process the stream of PUBLISH messages. */
    TEST_ASSERT_EQUAL_INT( true, _processPublish( pPublishStream,
                                                  _PUBLISH_STREAM_COUNT * sizeof( _pPublishTemplate ) + 1,
                                                  -1,
                                                  0 ) );
    TEST_ASSERT_EQUAL_INT( false, IotSemaphore_TimedWait( &_mallocSemaphore,
                                                          _PUBLISH_CALLBACK_TIMEOUT ) );
    _freeWrapper( pPublishStream );
}

/*-----------------------------------------------------------*/

/**
 * @brief Tests the behavior of @ref mqtt_function_receivecallback with a
 * spec-compliant PUBACK.
 */
TEST( MQTT_Unit_Receive, PubackValid )
{
    _mqttOperation_t publish = _INITIALIZE_OPERATION( IOT_MQTT_PUBLISH_TO_SERVER );

    /* Create the wait semaphore so notifications don't crash. The value of
     * this semaphore will not be checked, so the maxValue argument is arbitrary. */
    TEST_ASSERT_EQUAL_INT( true, IotSemaphore_Create( &( publish.notify.waitSemaphore ),
                                                      0,
                                                      10 ) );

    /* Even though no PUBLISH is in the receive queue, 4 bytes should still be
     * processed (should not crash). */
    {
        _DECLARE_PACKET( _pPubackTemplate, pPuback, pubackSize );
        TEST_ASSERT_EQUAL_INT( true, _processBuffer( &publish,
                                                     pPuback,
                                                     pubackSize,
                                                     ( int32_t ) pubackSize,
                                                     IOT_MQTT_STATUS_PENDING ) );
    }

    /* Process a valid PUBACK. */
    {
        _DECLARE_PACKET( _pPubackTemplate, pPuback, pubackSize );
        _operationResetAndPush( &publish );
        TEST_ASSERT_EQUAL_INT( true, _processBuffer( &publish,
                                                     pPuback,
                                                     pubackSize,
                                                     ( int32_t ) pubackSize,
                                                     IOT_MQTT_SUCCESS ) );
    }

    IotSemaphore_Destroy( &( publish.notify.waitSemaphore ) );
}

/*-----------------------------------------------------------*/

/**
 * @brief Tests the behavior of @ref mqtt_function_receivecallback with a PUBACK
 * that doesn't comply to MQTT spec.
 */
TEST( MQTT_Unit_Receive, PubackInvalid )
{
    _mqttOperation_t publish = _INITIALIZE_OPERATION( IOT_MQTT_PUBLISH_TO_SERVER );

    /* Create the wait semaphore so notifications don't crash. The value of
     * this semaphore will not be checked, so the maxValue argument is arbitrary. */
    TEST_ASSERT_EQUAL_INT( true, IotSemaphore_Create( &( publish.notify.waitSemaphore ),
                                                      0,
                                                      10 ) );

    /* An incomplete PUBACK should not be processed, and no status should be set. */
    {
        _DECLARE_PACKET( _pPubackTemplate, pPuback, pubackSize );
        _operationResetAndPush( &publish );
        TEST_ASSERT_EQUAL_INT( true, _processBuffer( &publish,
                                                     pPuback,
                                                     pubackSize - 1,
                                                     0,
                                                     IOT_MQTT_STATUS_PENDING ) );
    }

    /* The PUBACK control packet type must be 0x40. */
    {
        _DECLARE_PACKET( _pPubackTemplate, pPuback, pubackSize );
        pPuback[ 0 ] = 0x41;
        TEST_ASSERT_EQUAL_INT( true, _processBuffer( &publish,
                                                     pPuback,
                                                     pubackSize,
                                                     -1,
                                                     IOT_MQTT_BAD_RESPONSE ) );
    }

    /* A PUBACK must have a remaining length of 2. */
    {
        _DECLARE_PACKET( _pPubackTemplate, pPuback, pubackSize );
        pPuback[ 1 ] = 0x03;
        _operationResetAndPush( &publish );
        TEST_ASSERT_EQUAL_INT( true, _processBuffer( &publish,
                                                     pPuback,
                                                     pubackSize,
                                                     -1,
                                                     IOT_MQTT_BAD_RESPONSE ) );
    }

    /* The packet identifier in PUBACK cannot be 0. No status should be set if
     * packet identifier 0 is received. */
    {
        _DECLARE_PACKET( _pPubackTemplate, pPuback, pubackSize );
        pPuback[ 3 ] = 0x00;
        _operationResetAndPush( &publish );
        TEST_ASSERT_EQUAL_INT( true, _processBuffer( &publish,
                                                     pPuback,
                                                     pubackSize,
                                                     -1,
                                                     IOT_MQTT_STATUS_PENDING ) );
    }

    /* Remove unprocessed PUBLISH if present. */
    if( IotLink_IsLinked( &( publish.link ) ) == true )
    {
        IotQueue_Remove( &( publish.link ) );
    }

    IotSemaphore_Destroy( &( publish.notify.waitSemaphore ) );
}

/*-----------------------------------------------------------*/

/**
 * @brief Tests the behavior of @ref mqtt_function_receivecallback with a
 * spec-compliant SUBACK.
 */
TEST( MQTT_Unit_Receive, SubackValid )
{
    _mqttSubscription_t * pNewSubscription = NULL;
    _mqttOperation_t subscribe = _INITIALIZE_OPERATION( IOT_MQTT_SUBSCRIBE );
    IotMqttSubscription_t pSubscriptions[ 2 ] = { IOT_MQTT_SUBSCRIPTION_INITIALIZER };

    /* Create the wait semaphore so notifications don't crash. The value of
     * this semaphore will not be checked, so the maxValue argument is arbitrary. */
    TEST_ASSERT_EQUAL_INT( true, IotSemaphore_Create( &( subscribe.notify.waitSemaphore ),
                                                      0,
                                                      10 ) );

    /* Add 2 additional subscriptions to the MQTT connection. */
    pSubscriptions[ 0 ].qos = IOT_MQTT_QOS_1;
    pSubscriptions[ 0 ].callback.function = _publishCallback;
    pSubscriptions[ 0 ].pTopicFilter = _TEST_TOPIC_NAME "1";
    pSubscriptions[ 0 ].topicFilterLength = _TEST_TOPIC_LENGTH + 1;

    pSubscriptions[ 1 ].qos = IOT_MQTT_QOS_1;
    pSubscriptions[ 1 ].callback.function = _publishCallback;
    pSubscriptions[ 1 ].pTopicFilter = _TEST_TOPIC_NAME "2";
    pSubscriptions[ 1 ].topicFilterLength = _TEST_TOPIC_LENGTH + 1;

    TEST_ASSERT_EQUAL( IOT_MQTT_SUCCESS, _IotMqtt_AddSubscriptions( _pMqttConnection,
                                                                    1,
                                                                    pSubscriptions,
                                                                    2 ) );

    /* Set orders 2 and 1 for the new subscriptions. */
    pNewSubscription = IotLink_Container( _mqttSubscription_t,
                                          IotListDouble_PeekHead( &( _pMqttConnection->subscriptionList ) ),
                                          link );
    pNewSubscription->packetInfo.order = 2;

    pNewSubscription = IotLink_Container( _mqttSubscription_t,
                                          pNewSubscription->link.pNext,
                                          link );
    pNewSubscription->packetInfo.order = 1;

    /* Even though no SUBSCRIBE is in the receive queue, all bytes of the SUBACK
     * should still be processed (should not crash). */
    {
        _DECLARE_PACKET( _pSubackTemplate, pSuback, subackSize );
        TEST_ASSERT_EQUAL_INT( true, _processBuffer( &subscribe,
                                                     pSuback,
                                                     subackSize,
                                                     ( int32_t ) subackSize,
                                                     IOT_MQTT_STATUS_PENDING ) );
    }

    /* Process a valid SUBACK where all subscriptions are successful. */
    {
        IotMqttSubscription_t currentSubscription = IOT_MQTT_SUBSCRIPTION_INITIALIZER;
        _DECLARE_PACKET( _pSubackTemplate, pSuback, subackSize );
        _operationResetAndPush( &subscribe );
        TEST_ASSERT_EQUAL_INT( true, _processBuffer( &subscribe,
                                                     pSuback,
                                                     subackSize,
                                                     ( int32_t ) subackSize,
                                                     IOT_MQTT_SUCCESS ) );

        /* Test the subscription check function. QoS is not tested. */
        TEST_ASSERT_EQUAL_INT( true, IotMqtt_IsSubscribed( _pMqttConnection,
                                                           pSubscriptions[ 0 ].pTopicFilter,
                                                           pSubscriptions[ 0 ].topicFilterLength,
                                                           &currentSubscription ) );
        currentSubscription.qos = pSubscriptions[ 0 ].qos;
        TEST_ASSERT_EQUAL_MEMORY( &pSubscriptions[ 0 ],
                                  &currentSubscription,
                                  sizeof( IotMqttSubscription_t ) );
    }

    /* Process a valid SUBACK where some subscriptions were rejected. */
    {
        _DECLARE_PACKET( _pSubackTemplate, pSuback, subackSize );
        pSuback[ 4 ] = 0x80;
        pSuback[ 6 ] = 0x80;
        _operationResetAndPush( &subscribe );
        TEST_ASSERT_EQUAL_INT( true, _processBuffer( &subscribe,
                                                     pSuback,
                                                     subackSize,
                                                     ( int32_t ) subackSize,
                                                     IOT_MQTT_SERVER_REFUSED ) );

        /* Check that rejected subscriptions were removed from the subscription
         * list. */
        TEST_ASSERT_EQUAL_INT( false, IotMqtt_IsSubscribed( _pMqttConnection,
                                                            _TEST_TOPIC_NAME,
                                                            _TEST_TOPIC_LENGTH,
                                                            NULL ) );
        TEST_ASSERT_EQUAL_INT( false, IotMqtt_IsSubscribed( _pMqttConnection,
                                                            pSubscriptions[ 1 ].pTopicFilter,
                                                            pSubscriptions[ 1 ].topicFilterLength,
                                                            NULL ) );
    }

    IotSemaphore_Destroy( &( subscribe.notify.waitSemaphore ) );
}

/*-----------------------------------------------------------*/

/**
 * @brief Tests the behavior of @ref mqtt_function_receivecallback with a
 * SUBACK that doesn't comply to MQTT spec.
 */
TEST( MQTT_Unit_Receive, SubackInvalid )
{
    _mqttOperation_t subscribe = _INITIALIZE_OPERATION( IOT_MQTT_SUBSCRIBE );

    /* Create the wait semaphore so notifications don't crash. The value of
     * this semaphore will not be checked, so the maxValue argument is arbitrary. */
    TEST_ASSERT_EQUAL_INT( true, IotSemaphore_Create( &( subscribe.notify.waitSemaphore ),
                                                      0,
                                                      10 ) );

    /* Attempting to process a packet smaller than 5 bytes should result in no
     * bytes processed. 5 bytes is the minimum size of a SUBACK. */
    {
        _DECLARE_PACKET( _pSubackTemplate, pSuback, subackSize );
        _operationResetAndPush( &subscribe );
        TEST_ASSERT_EQUAL_INT( true, _processBuffer( &subscribe,
                                                     pSuback,
                                                     4,
                                                     0,
                                                     IOT_MQTT_STATUS_PENDING ) );
    }

    /* Attempt to process a SUBACK with an invalid "Remaining length". */
    {
        _DECLARE_PACKET( _pSubackTemplate, pSuback, subackSize );
        pSuback[ 1 ] = 0xff;
        pSuback[ 2 ] = 0xff;
        pSuback[ 3 ] = 0xff;
        pSuback[ 4 ] = 0xff;
        TEST_ASSERT_EQUAL_INT( true, _processBuffer( &subscribe,
                                                     pSuback,
                                                     subackSize,
                                                     -1,
                                                     IOT_MQTT_STATUS_PENDING ) );
    }

    /* Attempt to process a SUBACK larger than the size of the data stream. */
    {
        _DECLARE_PACKET( _pSubackTemplate, pSuback, subackSize );
        pSuback[ 1 ] = 0x52;
        TEST_ASSERT_EQUAL_INT( true, _processBuffer( &subscribe,
                                                     pSuback,
                                                     subackSize,
                                                     0,
                                                     IOT_MQTT_STATUS_PENDING ) );
    }

    /* Attempt to process a SUBACK with a "Remaining length" smaller than the
     * spec allows. */
    {
        _DECLARE_PACKET( _pSubackTemplate, pSuback, subackSize );
        pSuback[ 1 ] = 0x02;
        TEST_ASSERT_EQUAL_INT( true, _processBuffer( &subscribe,
                                                     pSuback,
                                                     subackSize,
                                                     -1,
                                                     IOT_MQTT_STATUS_PENDING ) );
    }

    /* Attempt to process a SUBACK with a bad return code. */
    {
        _DECLARE_PACKET( _pSubackTemplate, pSuback, subackSize );
        pSuback[ 6 ] = 0xff;
        TEST_ASSERT_EQUAL_INT( true, _processBuffer( &subscribe,
                                                     pSuback,
                                                     subackSize,
                                                     -1,
                                                     IOT_MQTT_BAD_RESPONSE ) );
    }

    /* The SUBACK control packet type must be 0x90. */
    {
        _DECLARE_PACKET( _pSubackTemplate, pSuback, subackSize );
        pSuback[ 0 ] = 0x91;
        _operationResetAndPush( &subscribe );
        TEST_ASSERT_EQUAL_INT( true, _processBuffer( &subscribe,
                                                     pSuback,
                                                     subackSize,
                                                     -1,
                                                     IOT_MQTT_BAD_RESPONSE ) );
    }

    IotSemaphore_Destroy( &( subscribe.notify.waitSemaphore ) );
}

/*-----------------------------------------------------------*/

/**
 * @brief Tests the behavior of @ref mqtt_function_receivecallback with a
 * spec-compliant UNSUBACK.
 */
TEST( MQTT_Unit_Receive, UnsubackValid )
{
    _mqttOperation_t unsubscribe = _INITIALIZE_OPERATION( IOT_MQTT_UNSUBSCRIBE );

    /* Create the wait semaphore so notifications don't crash. The value of
     * this semaphore will not be checked, so the maxValue argument is arbitrary. */
    TEST_ASSERT_EQUAL_INT( true, IotSemaphore_Create( &( unsubscribe.notify.waitSemaphore ),
                                                      0,
                                                      10 ) );

    /* Even though no UNSUBSCRIBE is in the receive queue, 4 bytes should still be
     * processed (should not crash). */
    {
        _DECLARE_PACKET( _pUnsubackTemplate, pUnsuback, unsubackSize );
        TEST_ASSERT_EQUAL_INT( true, _processBuffer( &unsubscribe,
                                                     pUnsuback,
                                                     unsubackSize,
                                                     ( int32_t ) unsubackSize,
                                                     IOT_MQTT_STATUS_PENDING ) );
    }

    /* Process a valid UNSUBACK. */
    {
        _DECLARE_PACKET( _pUnsubackTemplate, pUnsuback, unsubackSize );
        _operationResetAndPush( &unsubscribe );
        TEST_ASSERT_EQUAL_INT( true, _processBuffer( &unsubscribe,
                                                     pUnsuback,
                                                     unsubackSize,
                                                     ( int32_t ) unsubackSize,
                                                     IOT_MQTT_SUCCESS ) );
    }

    IotSemaphore_Destroy( &( unsubscribe.notify.waitSemaphore ) );
}

/*-----------------------------------------------------------*/

/**
 * @brief Tests the behavior of @ref mqtt_function_receivecallback with an
 * UNSUBACK that doesn't comply to MQTT spec.
 */
TEST( MQTT_Unit_Receive, UnsubackInvalid )
{
    _mqttOperation_t unsubscribe = _INITIALIZE_OPERATION( IOT_MQTT_UNSUBSCRIBE );

    /* Create the wait semaphore so notifications don't crash. The value of
     * this semaphore will not be checked, so the maxValue argument is arbitrary. */
    TEST_ASSERT_EQUAL_INT( true, IotSemaphore_Create( &( unsubscribe.notify.waitSemaphore ),
                                                      0,
                                                      10 ) );

    /* An incomplete UNSUBACK should not be processed, and no status should be set. */
    {
        _DECLARE_PACKET( _pUnsubackTemplate, pUnsuback, unsubackSize );
        _operationResetAndPush( &unsubscribe );
        TEST_ASSERT_EQUAL_INT( true, _processBuffer( &unsubscribe,
                                                     pUnsuback,
                                                     unsubackSize - 1,
                                                     0,
                                                     IOT_MQTT_STATUS_PENDING ) );
    }

    /* The UNSUBACK control packet type must be 0xb0. */
    {
        _DECLARE_PACKET( _pUnsubackTemplate, pUnsuback, unsubackSize );
        pUnsuback[ 0 ] = 0xb1;
        TEST_ASSERT_EQUAL_INT( true, _processBuffer( &unsubscribe,
                                                     pUnsuback,
                                                     unsubackSize,
                                                     -1,
                                                     IOT_MQTT_BAD_RESPONSE ) );
    }

    /* An UNSUBACK must have a remaining length of 2. */
    {
        _DECLARE_PACKET( _pUnsubackTemplate, pUnsuback, unsubackSize );
        pUnsuback[ 1 ] = 0x03;
        _operationResetAndPush( &unsubscribe );
        TEST_ASSERT_EQUAL_INT( true, _processBuffer( &unsubscribe,
                                                     pUnsuback,
                                                     unsubackSize,
                                                     -1,
                                                     IOT_MQTT_BAD_RESPONSE ) );
    }

    /* The packet identifier in UNSUBACK cannot be 0. No status should be set if
     * packet identifier 0 is received. */
    {
        _DECLARE_PACKET( _pUnsubackTemplate, pUnsuback, unsubackSize );
        pUnsuback[ 3 ] = 0x00;
        _operationResetAndPush( &unsubscribe );
        TEST_ASSERT_EQUAL_INT( true, _processBuffer( &unsubscribe,
                                                     pUnsuback,
                                                     unsubackSize,
                                                     -1,
                                                     IOT_MQTT_STATUS_PENDING ) );
    }

    /* Remove unprocessed UNSUBSCRIBE if present. */
    if( IotLink_IsLinked( &( unsubscribe.link ) ) == true )
    {
        IotQueue_Remove( &( unsubscribe.link ) );
    }

    IotSemaphore_Destroy( &( unsubscribe.notify.waitSemaphore ) );
}

/*-----------------------------------------------------------*/

/**
 * @brief Tests the behavior of @ref mqtt_function_receivecallback when receiving
 * a PINGRESP packet (both compliant and non-compliant packets).
 */
TEST( MQTT_Unit_Receive, Pingresp )
{
    /* Even though no PINGREQ is expected, the keep-alive failure flag should
     * be cleared (should not crash). */
    {
        _pMqttConnection->keepAliveFailure = false;

        _DECLARE_PACKET( _pPingrespTemplate, pPingresp, pingrespSize );
        TEST_ASSERT_EQUAL_INT( true, _processBuffer( NULL,
                                                     pPingresp,
                                                     pingrespSize,
                                                     ( int32_t ) pingrespSize,
                                                     IOT_MQTT_SUCCESS ) );

        TEST_ASSERT_EQUAL_INT( false, _pMqttConnection->keepAliveFailure );
    }

    /* Process a valid PINGRESP. */
    {
        _pMqttConnection->keepAliveFailure = true;

        _DECLARE_PACKET( _pPingrespTemplate, pPingresp, pingrespSize );
        TEST_ASSERT_EQUAL_INT( true, _processBuffer( NULL,
                                                     pPingresp,
                                                     pingrespSize,
                                                     ( int32_t ) pingrespSize,
                                                     IOT_MQTT_SUCCESS ) );

        TEST_ASSERT_EQUAL_INT( false, _pMqttConnection->keepAliveFailure );
    }

    /* An incomplete PINGRESP should not be processed, and the keep-alive failure
     * flag should not be cleared. */
    {
        _pMqttConnection->keepAliveFailure = true;

        _DECLARE_PACKET( _pPingrespTemplate, pPingresp, pingrespSize );
        TEST_ASSERT_EQUAL_INT( true, _processBuffer( NULL,
                                                     pPingresp,
                                                     pingrespSize - 1,
                                                     0,
                                                     IOT_MQTT_SUCCESS ) );

        TEST_ASSERT_EQUAL_INT( true, _pMqttConnection->keepAliveFailure );
    }

    /* A PINGRESP should have a remaining length of 0. */
    {
        _pMqttConnection->keepAliveFailure = true;

        _DECLARE_PACKET( _pPingrespTemplate, pPingresp, pingrespSize );
        pPingresp[ 1 ] = 0x01;
        TEST_ASSERT_EQUAL_INT( true, _processBuffer( NULL,
                                                     pPingresp,
                                                     pingrespSize,
                                                     -1,
                                                     IOT_MQTT_SUCCESS ) );

        TEST_ASSERT_EQUAL_INT( true, _pMqttConnection->keepAliveFailure );
    }

    /* The PINGRESP control packet type must be 0xd0. */
    {
        _pMqttConnection->keepAliveFailure = true;

        _DECLARE_PACKET( _pPingrespTemplate, pPingresp, pingrespSize );
        pPingresp[ 0 ] = 0xd1;
        TEST_ASSERT_EQUAL_INT( true, _processBuffer( NULL,
                                                     pPingresp,
                                                     pingrespSize,
                                                     -1,
                                                     IOT_MQTT_SUCCESS ) );

        TEST_ASSERT_EQUAL_INT( true, _pMqttConnection->keepAliveFailure );
    }
}

/*-----------------------------------------------------------*/
