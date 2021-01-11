/*
 * FreeRTOS MQTT V2.3.1
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
 * @file iot_tests_mqtt_receive.c
 * @brief Tests for the function @ref mqtt_function_receivecallback.
 */

/* The config header is always included first. */
#include "iot_config.h"

/* Standard includes. */
#include <string.h>

/* SDK initialization include. */
#include "iot_init.h"

/* Platform layer includes. */
#include "platform/iot_threads.h"

/* MQTT internal include. */
#include "private/iot_mqtt_internal.h"

/* Test framework includes. */
#include "unity_fixture.h"

/* MQTT test access include. */
#include "iot_test_access_mqtt.h"

/* Error handling include. */
#include "private/iot_error.h"

/**
 * @brief Determine which MQTT server mode to test (AWS IoT or Mosquitto).
 */
#if !defined( IOT_TEST_MQTT_MOSQUITTO ) || IOT_TEST_MQTT_MOSQUITTO == 0
    #define AWS_IOT_MQTT_SERVER    true
#else
    #define AWS_IOT_MQTT_SERVER    false
#endif

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
#define TEST_TOPIC_NAME             "/test/topic"

/**
 * @brief Length of #TEST_TOPIC_NAME.
 */
#define TEST_TOPIC_LENGTH           ( ( uint16_t ) ( sizeof( TEST_TOPIC_NAME ) - 1 ) )

/**
 * @brief Timeout for waiting on a PUBLISH callback.
 */
#define PUBLISH_CALLBACK_TIMEOUT    ( 1000 )

/**
 * @brief Declare a buffer holding a packet and its size.
 */
#define DECLARE_PACKET( pTemplate, bufferName, sizeName ) \
    uint8_t bufferName[ sizeof( pTemplate ) ] = { 0 };    \
    const size_t sizeName = sizeof( pTemplate );          \
    ( void ) memcpy( bufferName, pTemplate, sizeName );

/**
 * @brief Initializer for operations in the tests.
 */
#define INITIALIZE_OPERATION( name )                                                                             \
    {                                                                                                            \
        .link = { 0 }, .incomingPublish = false, .pMqttConnection = _pMqttConnection,                            \
        .jobStorage = IOT_TASKPOOL_JOB_STORAGE_INITIALIZER, .job = IOT_TASKPOOL_JOB_INITIALIZER,                 \
        .u.operation =                                                                                           \
        {                                                                                                        \
            .jobReference     = 1, .type        = name,                    .flags      = IOT_MQTT_FLAG_WAITABLE, \
            .packetIdentifier = 1, .pMqttPacket = NULL,                    .packetSize = 0,                      \
            .notify           = { .callback = { 0 } },.status      = IOT_MQTT_STATUS_PENDING, .retry      = { 0 }                   \
        }                                                                                                        \
    }

/*-----------------------------------------------------------*/

/**
 * @brief Context for calls to the network receive function.
 */
typedef struct _receiveContext
{
    const uint8_t * pData; /**< @brief The data to receive. */
    size_t dataLength;     /**< @brief Length of data. */
    size_t dataIndex;      /**< @brief Next byte of data to read. */
} _receiveContext_t;

/**
 * @brief Each compilation unit that consumes the NetworkContext must define it.
 * It should contain a single pointer to the type of your desired transport.
 * When using multiple transports in the same compilation unit, define this pointer as void *.
 *
 * @note Transport stacks are defined in amazon-freertos/libraries/abstractions/transport/secure_sockets/transport_secure_sockets.h.
 */
struct NetworkContext
{
    MqttTransportParams_t * pParams;
};

/*-----------------------------------------------------------*/

/**
 * @brief The MQTT connection shared by all the tests.
 */
static _mqttConnection_t * _pMqttConnection = IOT_MQTT_CONNECTION_INITIALIZER;

/**
 * @brief The network interface shared by all the tests.
 */
static IotNetworkInterface_t _networkInterface = { 0 };

/**
 * @brief The subscription shared by all the tests.
 */
static IotMqttSubscription_t _subscription = IOT_MQTT_SUBSCRIPTION_INITIALIZER;

/**
 * @brief Tracks whether a deserializer override was called for a test.
 */
static bool _deserializeOverrideCalled = false;

/**
 * @brief Tracks whether #_getPacketType has been called.
 */
static bool _getPacketTypeCalled = false;

/**
 * @brief Tracks whether #_getRemainingLength has been called.
 */
static bool _getRemainingLengthCalled = false;

/**
 * @brief Tracks whether #_close has been called.
 */
static bool _networkCloseCalled = false;

/**
 * @brief Tracks whether #_disconnectCallback has been called.
 */
static bool _disconnectCallbackCalled = false;

/**
 * @brief Represents the network context used for the TLS session with the
 * server.
 */
static NetworkContext_t networkContext = { 0 };

/*-----------------------------------------------------------*/

/* Using initialized connToContext variable. */
extern _connContext_t connToContext[ MAX_NO_OF_MQTT_CONNECTIONS ];

/*-----------------------------------------------------------*/

/**
 * @brief Get packet type function override.
 */
static uint8_t _getPacketType( void * pNetworkConnection,
                               const IotNetworkInterface_t * pNetworkInterface )
{
    _getPacketTypeCalled = true;

    return _IotMqtt_GetPacketType( pNetworkConnection, pNetworkInterface );
}

/*-----------------------------------------------------------*/

/**
 * @brief Get remaining length function override.
 */
static size_t _getRemainingLength( void * pNetworkConnection,
                                   const IotNetworkInterface_t * pNetworkInterface )
{
    _getRemainingLengthCalled = true;

    return _IotMqtt_GetRemainingLength( pNetworkConnection, pNetworkInterface );
}

/*-----------------------------------------------------------*/

/**
 * @brief Deserializer override for CONNACK.
 */
static IotMqttError_t _deserializeConnack( _mqttPacket_t * pConnack )
{
    _deserializeOverrideCalled = true;

    return _IotMqtt_deserializeConnackWrapper( pConnack );
}

/*-----------------------------------------------------------*/

/**
 * @brief Deserializer override for PUBLISH.
 */
static IotMqttError_t _deserializePublish( _mqttPacket_t * pPublish )
{
    _deserializeOverrideCalled = true;

    return _IotMqtt_deserializePublishWrapper( pPublish );
}

/*-----------------------------------------------------------*/

/**
 * @brief Deserializer override for PUBACK.
 */
static IotMqttError_t _deserializePuback( _mqttPacket_t * pPuback )
{
    _deserializeOverrideCalled = true;

    return _IotMqtt_deserializePubackWrapper( pPuback );
}

/*-----------------------------------------------------------*/

/**
 * @brief Deserializer override for SUBACK.
 */
static IotMqttError_t _deserializeSuback( _mqttPacket_t * pSuback )
{
    _deserializeOverrideCalled = true;

    return _IotMqtt_deserializeSubackWrapper( pSuback );
}

/*-----------------------------------------------------------*/

/**
 * @brief Deserializer override for UNSUBACK.
 */
static IotMqttError_t _deserializeUnsuback( _mqttPacket_t * pUnsuback )
{
    _deserializeOverrideCalled = true;

    return _IotMqtt_deserializeUnsubackWrapper( pUnsuback );
}

/*-----------------------------------------------------------*/

/**
 * @brief Deserializer override for PINGRESP.
 */
static IotMqttError_t _deserializePingresp( _mqttPacket_t * pPingresp )
{
    _deserializeOverrideCalled = true;

    return _IotMqtt_deserializePingrespWrapper( pPingresp );
}

/*-----------------------------------------------------------*/

/**
 * @brief Reset the status of an #_mqttOperation_t and push it to the list of
 * MQTT operations awaiting network response.
 */
static void _operationResetAndPush( _mqttOperation_t * pOperation )
{
    pOperation->u.operation.status = IOT_MQTT_STATUS_PENDING;
    pOperation->u.operation.jobReference = 1;

    IotListDouble_InsertHead( &( _pMqttConnection->pendingResponse ), &( pOperation->link ) );
}

/*-----------------------------------------------------------*/

/**
 * @brief Process a non-PUBLISH buffer and check the result.
 */
static bool _processBuffer( const _mqttOperation_t * pOperation,
                            const uint8_t * pBuffer,
                            size_t bufferSize,
                            IotMqttError_t expectedResult )
{
    bool status = true;
    _receiveContext_t receiveContext = { 0 };

    /* Set the members of the receive context. */
    receiveContext.pData = pBuffer;
    receiveContext.dataLength = bufferSize;

    /* Call the receive callback on pBuffer. */
    IotMqtt_ReceiveCallback( &receiveContext,
                             _pMqttConnection );

    /* Check expected result if operation is given. */
    if( pOperation != NULL )
    {
        status = ( expectedResult == pOperation->u.operation.status );
    }

    return status;
}

/*-----------------------------------------------------------*/

/**
 * @brief Process a PUBLISH message and check the result.
 */
static bool _processPublish( const uint8_t * pPublish,
                             size_t publishSize,
                             uint32_t expectedInvokeCount )
{
    IotSemaphore_t invokeCount;
    uint32_t finalInvokeCount = 0, i = 0;
    bool waitResult = true;
    _receiveContext_t receiveContext = { 0 };
    int8_t contextIndex = -1;

    /* Create a semaphore that counts how many times the publish callback is invoked. */
    if( expectedInvokeCount > 0 )
    {
        if( IotSemaphore_Create( &invokeCount, 0, expectedInvokeCount ) == false )
        {
            return false;
        }
    }

    /* Getting context index for the given MQTT Connection.  */
    contextIndex = _IotMqtt_getContextIndexFromConnection( _pMqttConnection );
    ( connToContext[ contextIndex ].subscriptionArray[ 0 ] ).unsubscribed = false;
    ( connToContext[ contextIndex ].subscriptionArray[ 0 ] ).callback.pCallbackContext = &invokeCount;
    /* Set the members of the receive context. */
    receiveContext.pData = pPublish;
    receiveContext.dataLength = publishSize;

    /* Call the receive callback on pPublish. */
    IotMqtt_ReceiveCallback( &receiveContext,
                             _pMqttConnection );

    /* Check how many times the publish callback is invoked. */
    for( i = 0; i < expectedInvokeCount; i++ )
    {
        waitResult = IotSemaphore_TimedWait( &invokeCount,
                                             PUBLISH_CALLBACK_TIMEOUT );

        if( waitResult == false )
        {
            break;
        }
    }

    /* Ensure that the invoke count semaphore has a value of 0, then destroy the
     * semaphore. */
    if( expectedInvokeCount > 0 )
    {
        finalInvokeCount = IotSemaphore_GetCount( &invokeCount );
        IotSemaphore_Destroy( &invokeCount );
    }

    /* Check results against expected values. */
    return ( finalInvokeCount == 0 ) &&
           ( waitResult == true );
}

/*-----------------------------------------------------------*/

/**
 * @brief Called when a PUBLISH message is "received".
 */
static void _publishCallback( void * pCallbackContext,
                              IotMqttCallbackParam_t * pPublish )
{
    IotSemaphore_t * pInvokeCount = ( IotSemaphore_t * ) pCallbackContext;

    /* QoS 2 is valid for these tests, but currently unsupported by the MQTT library.
     * Change the QoS to 0 so that QoS validation passes. */
    pPublish->u.message.info.qos = IOT_MQTT_QOS_0;

    /* Check that the parameters to this function are valid. */
    if( ( _IotMqtt_ValidatePublish( AWS_IOT_MQTT_SERVER,
                                    &( pPublish->u.message.info ) ) == true ) &&
        ( pPublish->u.message.info.topicNameLength == TEST_TOPIC_LENGTH ) &&
        ( strncmp( TEST_TOPIC_NAME, pPublish->u.message.info.pTopicName, TEST_TOPIC_LENGTH ) == 0 ) )
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
 * @brief Simulates a network receive function.
 */
static size_t _receive( void * pConnection,
                        uint8_t * pBuffer,
                        size_t bytesRequested )
{
    size_t bytesReceived = 0;
    _receiveContext_t * pReceiveContext = pConnection;

    if( pReceiveContext->dataIndex != pReceiveContext->dataLength )
    {
        TEST_ASSERT_NOT_EQUAL( 0, bytesRequested );
        TEST_ASSERT_LESS_THAN( pReceiveContext->dataLength, pReceiveContext->dataIndex );

        /* Calculate how much data to copy. */
        const size_t dataAvailable = pReceiveContext->dataLength - pReceiveContext->dataIndex;

        if( bytesRequested > dataAvailable )
        {
            bytesReceived = dataAvailable;
        }
        else
        {
            bytesReceived = bytesRequested;
        }

        /* Copy data into given buffer. */
        if( bytesReceived > 0 )
        {
            ( void ) memcpy( pBuffer,
                             pReceiveContext->pData + pReceiveContext->dataIndex,
                             bytesReceived );

            pReceiveContext->dataIndex += bytesReceived;
        }
    }

    return bytesReceived;
}

/*-----------------------------------------------------------*/

/**
 * @brief A network close function that reports if it was invoked.
 */
static IotNetworkError_t _close( void * pConnection )
{
    /* Silence warnings about unused parameters. */
    ( void ) pConnection;

    /* Report that this function was called. */
    _networkCloseCalled = true;

    return IOT_NETWORK_SUCCESS;
}

/*-----------------------------------------------------------*/

/**
 * @brief A disconnect callback function that checks for a "bad packet" reason
 * and reports if it was invoked.
 */
static void _disconnectCallback( void * pCallbackContext,
                                 IotMqttCallbackParam_t * pCallbackParam )
{
    /* Silence warnings about unused parameters. */
    ( void ) pCallbackContext;

    if( pCallbackParam->u.disconnectReason == IOT_MQTT_BAD_PACKET_RECEIVED )
    {
        _disconnectCallbackCalled = true;
    }
}

/**
 * @brief A dummy function for transport interface send.
 *
 * Transport send is not used in this test.
 *
 * @param[in] pNetworkContext Implementation-defined network context.
 * @param[in] pMessage Message buffer to send.
 * @param[in] bytesToSend Number of bytes sent.
 *
 * @return The number of bytes received or a negative error code.
 */
static int32_t transportSend( NetworkContext_t * pNetworkContext,
                              const void * pMessage,
                              size_t bytesToSend )
{
    ( void ) pNetworkContext;
    ( void ) pMessage;

    return bytesToSend;
}

/*-----------------------------------------------------------*/

/**
 * @brief A dummy function for transport interface receive.
 *
 * MQTT shim handles the receive from the network and hence transport
 * implementation for receive is not used by the coreMQTT library. This
 * dummy implementation is used for passing a non-NULL parameter to
 * `MQTT_Init()`.
 *
 * @param[in] pNetworkContext Implementation-defined network context.
 * @param[in] pBuffer Buffer to receive the data into.
 * @param[in] bytesToRecv Number of bytes requested from the network.
 *
 * @return The number of bytes received or a negative error code.
 */
static int32_t transportRecv( NetworkContext_t * pNetworkContext,
                              void * pBuffer,
                              size_t bytesToRecv )
{
    /* MQTT shim handles the receive from the network and hence transport
     * implementation for receive is not used by the coreMQTT library. This
     * dummy implementation is used for passing a non-NULL parameter to
     * `MQTT_Init()`. */
    ( void ) pNetworkContext;
    ( void ) pBuffer;
    ( void ) bytesToRecv;

    /* Always return an error. */
    return -1;
}

/*-----------------------------------------------------------*/

/**
 * @brief The time interface provided to the MQTT context used in calling MQTT LTS APIs.
 */
static uint32_t getTimeMs( void )
{
    return 0U;
}

/*-----------------------------------------------------------*/

/**
 * @brief The dummy application callback function.
 *
 * This function doesn't need to have any implementation as
 * the receive from network is not handled by the coreMQTT library,
 * but the MQTT shim itself. This function is just a dummy function
 * to be passed as a parameter to `MQTT_Init()`.
 *
 * @param[in] pMqttContext MQTT context pointer.
 * @param[in] pPacketInfo Packet Info pointer for the incoming packet.
 * @param[in] pDeserializedInfo Deserialized information from incoming packet.
 */
static void eventCallback( MQTTContext_t * pContext,
                           MQTTPacketInfo_t * pPacketInfo,
                           MQTTDeserializedInfo_t * pDeserializedInfo )
{
    /* This function doesn't need to have any implementation as
     * the receive from network is not handled by the coreMQTT library,
     * but the MQTT shim itself. This function is just a dummy function
     * to be passed as a parameter to `MQTT_Init()`. */
    ( void ) pContext;
    ( void ) pPacketInfo;
    ( void ) pDeserializedInfo;
}

/*-----------------------------------------------------------*/

/**
 * @brief Setting the MQTT Context for the given MQTT Connection.
 */
static IotMqttError_t _setContext( IotMqttConnection_t pMqttConnection )
{
    IOT_FUNCTION_ENTRY( IotMqttError_t, IOT_MQTT_BAD_PARAMETER );
    int8_t contextIndex = -1;
    bool subscriptionMutexCreated = false;
    bool contextMutex = false;
    TransportInterface_t transport;
    MQTTFixedBuffer_t networkBuffer;
    MQTTStatus_t managedMqttStatus = MQTTBadParameter;

    /* Clear the MQTT connection to context array. */
    memset( connToContext, 0x00, sizeof( connToContext ) );

    /* Getting the free index from the MQTT connection to MQTT context mapping array. */
    contextIndex = _IotMqtt_getFreeIndexFromContextConnectionArray();
    TEST_ASSERT_NOT_EQUAL( -1, contextIndex );

    /* Clear the array at the index obtained. */
    memset( &( connToContext[ contextIndex ] ), 0x00, sizeof( _connContext_t ) );

    /* Creating Mutex for the synchronization of MQTT Context used for sending the packets
     * on the network using MQTT LTS API. */
    contextMutex = IotMutex_CreateRecursiveMutex( &( connToContext[ contextIndex ].contextMutex ),
                                                  &( connToContext[ contextIndex ].contextMutexStorage ) );

    /* Create the subscription mutex for a new connection. */
    if( contextMutex == true )
    {
        /* Assigning the MQTT Connection. */
        connToContext[ contextIndex ].mqttConnection = pMqttConnection;

        /* Assigning the Network Context to be used by this MQTT Context. */
        networkContext.pParams = &connToContext[ contextIndex ].mqttTransportParams;
        connToContext[ contextIndex ].mqttTransportParams.pNetworkConnection = pMqttConnection->pNetworkConnection;
        connToContext[ contextIndex ].mqttTransportParams.pNetworkInterface = pMqttConnection->pNetworkInterface;

        /* Fill in TransportInterface send function pointer. We will not be implementing the
         * TransportInterface receive function pointer as receiving of packets is handled in shim by network
         * receive task. Only using MQTT LTS APIs for transmit path.*/
        transport.pNetworkContext = &networkContext;
        transport.send = transportSend;
        transport.recv = transportRecv;

        /* Fill the values for network buffer. */
        networkBuffer.pBuffer = &( connToContext[ contextIndex ].buffer[ 0 ] );
        networkBuffer.size = NETWORK_BUFFER_SIZE;
        subscriptionMutexCreated = IotMutex_CreateNonRecursiveMutex( &( connToContext[ contextIndex ].subscriptionMutex ),
                                                                     &( connToContext[ contextIndex ].subscriptionMutexStorage ) );

        if( subscriptionMutexCreated == false )
        {
            IotLogError( "Failed to create subscription mutex for new connection." );
            IOT_SET_AND_GOTO_CLEANUP( IOT_MQTT_NO_MEMORY );
        }
        else
        {
            /* Initializing the MQTT context used in calling MQTT LTS API. */
            managedMqttStatus = MQTT_Init( &( connToContext[ contextIndex ].context ), &transport, getTimeMs, eventCallback, &networkBuffer );
            status = convertReturnCode( managedMqttStatus );
        }

        if( status != IOT_MQTT_SUCCESS )
        {
            IotLogError( "(MQTT connection %p) Failed to initialize context for "
                         "the MQTT connection.",
                         pMqttConnection );
            IOT_GOTO_CLEANUP();
        }
    }
    else
    {
        IotLogError( "(MQTT connection %p) Failed to create mutex for "
                     "the MQTT context.",
                     pMqttConnection );
        IOT_SET_AND_GOTO_CLEANUP( IOT_MQTT_NO_MEMORY );
    }

    IOT_FUNCTION_CLEANUP_BEGIN();

    /* Clean up the context on error. */
    if( status != IOT_MQTT_SUCCESS )
    {
        _IotMqtt_removeContext( pMqttConnection );
    }

    IOT_FUNCTION_CLEANUP_END();
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
    static IotMqttSerializer_t serializer = IOT_MQTT_SERIALIZER_INITIALIZER;
    IotMqttNetworkInfo_t networkInfo = IOT_MQTT_NETWORK_INFO_INITIALIZER;

    /* Initialize SDK. */
    TEST_ASSERT_EQUAL_INT( true, IotSdk_Init() );

    /* Initialize the MQTT library. */
    TEST_ASSERT_EQUAL( IOT_MQTT_SUCCESS, IotMqtt_Init() );

    /* Set the deserializer overrides. */
    serializer.serialize.puback = _serializePuback;
    serializer.deserialize.connack = _IotMqtt_deserializeConnackWrapper;
    serializer.deserialize.publish = _IotMqtt_deserializePublishWrapper;
    serializer.deserialize.puback = _IotMqtt_deserializePubackWrapper;
    serializer.deserialize.suback = _IotMqtt_deserializeSubackWrapper;
    serializer.deserialize.unsuback = _IotMqtt_deserializeUnsubackWrapper;
    serializer.deserialize.pingresp = _IotMqtt_deserializePingrespWrapper;
    serializer.getPacketType = _getPacketType;
    serializer.getRemainingLength = _getRemainingLength;

    _networkInterface.receive = _receive;
    _networkInterface.close = _close;
    networkInfo.pNetworkInterface = &_networkInterface;
    networkInfo.disconnectCallback.function = _disconnectCallback;

    /* Initialize the MQTT connection used by the tests. */
    _pMqttConnection = IotTestMqtt_createMqttConnection( AWS_IOT_MQTT_SERVER,
                                                         &networkInfo,
                                                         0 );
    TEST_ASSERT_NOT_NULL( _pMqttConnection );

    /* Setting the MQTT Context for the given MQTT Connection. */
    TEST_ASSERT_EQUAL( IOT_MQTT_SUCCESS, _setContext( _pMqttConnection ) );

    /* Set the MQTT serializer overrides. */
    _pMqttConnection->pSerializer = &serializer;

    /* Set the members of the subscription. */
    _subscription.pTopicFilter = TEST_TOPIC_NAME;
    _subscription.topicFilterLength = TEST_TOPIC_LENGTH;
    _subscription.callback.function = _publishCallback;

    /* Add the subscription to the MQTT connection. */
    TEST_ASSERT_EQUAL( IOT_MQTT_SUCCESS, _IotMqtt_AddSubscriptions( _pMqttConnection,
                                                                    1,
                                                                    &_subscription,
                                                                    1 ) );

    /* Clear functions called flags. */
    _getPacketTypeCalled = false;
    _getRemainingLengthCalled = false;
    _networkCloseCalled = false;
    _disconnectCallbackCalled = false;
}

/*-----------------------------------------------------------*/

/**
 * @brief Test tear down for MQTT Receive tests.
 */
TEST_TEAR_DOWN( MQTT_Unit_Receive )
{
    /* Clean up resources taken in test setup. */
    IotMqtt_Disconnect( _pMqttConnection, IOT_MQTT_FLAG_CLEANUP_ONLY );
    IotMqtt_Cleanup();
    IotSdk_Cleanup();

    TEST_ASSERT_EQUAL_INT( true, _getPacketTypeCalled );
    TEST_ASSERT_EQUAL_INT( true, _getRemainingLengthCalled );
}

/*-----------------------------------------------------------*/

/**
 * @brief Test group runner for MQTT Receive tests.
 */
TEST_GROUP_RUNNER( MQTT_Unit_Receive )
{
    RUN_TEST_CASE( MQTT_Unit_Receive, DecodeRemainingLength );
    RUN_TEST_CASE( MQTT_Unit_Receive, InvalidPacket );
    RUN_TEST_CASE( MQTT_Unit_Receive, ConnackValid );
    RUN_TEST_CASE( MQTT_Unit_Receive, ConnackInvalid );
    RUN_TEST_CASE( MQTT_Unit_Receive, PublishValid );
    RUN_TEST_CASE( MQTT_Unit_Receive, PublishInvalid );
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
 * @brief Tests the function for decoding MQTT remaining length.
 */
TEST( MQTT_Unit_Receive, DecodeRemainingLength )
{
    /* Decode 0, which has a 1-byte representation. */
    {
        uint8_t pRemainingLength[ 4 ] = { 0 };
        _receiveContext_t receiveContext = { 0 };

        /* Set the members of the receive context. */
        receiveContext.pData = pRemainingLength;
        receiveContext.dataLength = 4;

        TEST_ASSERT_EQUAL( 0,
                           _IotMqtt_GetRemainingLength( &receiveContext,
                                                        &_networkInterface ) );
    }

    /* Decode 129, which has a 2-byte representation. */
    {
        uint8_t pRemainingLength[ 4 ] = { 0x81, 0x01, 0x00, 0x00 };
        _receiveContext_t receiveContext = { 0 };

        /* Set the members of the receive context. */
        receiveContext.pData = pRemainingLength;
        receiveContext.dataLength = 4;

        TEST_ASSERT_EQUAL( 129,
                           _IotMqtt_GetRemainingLength( &receiveContext,
                                                        &_networkInterface ) );
    }

    /* Decode 16,386, which has a 3-byte representation. */
    {
        uint8_t pRemainingLength[ 4 ] = { 0x82, 0x80, 0x01, 0x00 };
        _receiveContext_t receiveContext = { 0 };

        /* Set the members of the receive context. */
        receiveContext.pData = pRemainingLength;
        receiveContext.dataLength = 4;

        TEST_ASSERT_EQUAL( 16386,
                           _IotMqtt_GetRemainingLength( &receiveContext,
                                                        &_networkInterface ) );
    }

    /* Decode 268,435,455, which has a 4-byte representation. This value is the
     * largest representable value. */
    {
        uint8_t pRemainingLength[ 4 ] = { 0xff, 0xff, 0xff, 0x7f };
        _receiveContext_t receiveContext = { 0 };

        /* Set the members of the receive context. */
        receiveContext.pData = pRemainingLength;
        receiveContext.dataLength = 4;

        TEST_ASSERT_EQUAL( 268435455,
                           _IotMqtt_GetRemainingLength( &receiveContext,
                                                        &_networkInterface ) );
    }

    /* Attempt to decode an invalid value where all continuation bits are set. */
    {
        uint8_t pRemainingLength[ 4 ] = { 0xff, 0xff, 0xff, 0x8f };
        _receiveContext_t receiveContext = { 0 };

        /* Set the members of the receive context. */
        receiveContext.pData = pRemainingLength;
        receiveContext.dataLength = 4;

        TEST_ASSERT_EQUAL( MQTT_REMAINING_LENGTH_INVALID,
                           _IotMqtt_GetRemainingLength( &receiveContext,
                                                        &_networkInterface ) );
    }

    /* Attempt to decode a 4-byte representation of 0. According to the spec,
     * this representation is not valid. */
    {
        uint8_t pRemainingLength[ 4 ] = { 0x80, 0x80, 0x80, 0x00 };
        _receiveContext_t receiveContext = { 0 };

        /* Set the members of the receive context. */
        receiveContext.pData = pRemainingLength;
        receiveContext.dataLength = 4;

        TEST_ASSERT_EQUAL( MQTT_REMAINING_LENGTH_INVALID,
                           _IotMqtt_GetRemainingLength( &receiveContext,
                                                        &_networkInterface ) );
    }

    /* Test tear down for this test group checks that deserializer overrides
     * were called. However, this test does not use any deserializer overrides;
     * set these values to true so that the checks pass. */
    _getPacketTypeCalled = true;
    _getRemainingLengthCalled = true;
}

/*-----------------------------------------------------------*/

/**
 * @brief Tests the behavior of @ref mqtt_function_receivecallback with an
 * invalid control packet type.
 */
TEST( MQTT_Unit_Receive, InvalidPacket )
{
    uint8_t invalidPacket = 0xf0;
    _receiveContext_t receiveContext = { 0 };

    /* Set the members of the receive context. */
    receiveContext.pData = &invalidPacket;
    receiveContext.dataLength = 1;

    /* Processing a control packet 0xf is a protocol violation. */
    IotMqtt_ReceiveCallback( &receiveContext,
                             _pMqttConnection );

    /* Processing an invalid packet should cause the network connection to be closed. */
    TEST_ASSERT_EQUAL_INT( true, _networkCloseCalled );
    TEST_ASSERT_EQUAL_INT( true, _disconnectCallbackCalled );

    /* This test should not have called any deserializer. Set the deserialize
     * override called flag to true so that the check for it passes. */
    TEST_ASSERT_EQUAL_INT( false, _getRemainingLengthCalled );
    _getRemainingLengthCalled = true;
}

/*-----------------------------------------------------------*/

/**
 * @brief Tests the behavior of @ref mqtt_function_receivecallback with a spec-compliant
 * CONNACK.
 */
TEST( MQTT_Unit_Receive, ConnackValid )
{
    uint8_t i = 0;
    _mqttOperation_t connect = INITIALIZE_OPERATION( IOT_MQTT_CONNECT );

    /* Create the wait semaphore so notifications don't crash. The value of
     * this semaphore will not be checked, so the maxValue argument is arbitrary. */
    TEST_ASSERT_EQUAL_INT( true, IotSemaphore_Create( &( connect.u.operation.notify.waitSemaphore ),
                                                      0,
                                                      10 ) );

    /* Even though no CONNECT is in the receive queue, 4 bytes should still be
     * processed (should not crash). */
    {
        DECLARE_PACKET( _pConnackTemplate, pConnack, connackSize );
        TEST_ASSERT_EQUAL_INT( true, _processBuffer( &connect,
                                                     pConnack,
                                                     connackSize,
                                                     IOT_MQTT_STATUS_PENDING ) );
    }

    /* Process a valid, successful CONNACK with no SP flag. */
    {
        DECLARE_PACKET( _pConnackTemplate, pConnack, connackSize );
        _operationResetAndPush( &connect );
        TEST_ASSERT_EQUAL_INT( true, _processBuffer( &connect,
                                                     pConnack,
                                                     connackSize,
                                                     IOT_MQTT_SUCCESS ) );
    }

    /* Process a valid, successful CONNACK with SP flag set. */
    {
        DECLARE_PACKET( _pConnackTemplate, pConnack, connackSize );
        pConnack[ 2 ] = 0x01;
        _operationResetAndPush( &connect );
        TEST_ASSERT_EQUAL_INT( true, _processBuffer( &connect,
                                                     pConnack,
                                                     connackSize,
                                                     IOT_MQTT_SUCCESS ) );
    }

    /* Check each of the CONNACK failure codes, which range from 1 to 5. */
    for( i = 0x01; i < 0x06; i++ )
    {
        DECLARE_PACKET( _pConnackTemplate, pConnack, connackSize );

        /* Set the CONNECT state and code. */
        _operationResetAndPush( &connect );
        pConnack[ 3 ] = i;

        /* Push a CONNECT operation to the queue and process a CONNACK. */
        TEST_ASSERT_EQUAL_INT( true, _processBuffer( &connect,
                                                     pConnack,
                                                     connackSize,
                                                     IOT_MQTT_SERVER_REFUSED ) );
    }

    IotSemaphore_Destroy( &( connect.u.operation.notify.waitSemaphore ) );

    /* Network close function should not have been invoked. */
    TEST_ASSERT_EQUAL_INT( false, _networkCloseCalled );
    TEST_ASSERT_EQUAL_INT( false, _disconnectCallbackCalled );
}

/*-----------------------------------------------------------*/

/**
 * @brief Tests the behavior of @ref mqtt_function_receivecallback with a
 * CONNACK that doesn't comply to MQTT spec.
 */
TEST( MQTT_Unit_Receive, ConnackInvalid )
{
    _mqttOperation_t connect = INITIALIZE_OPERATION( IOT_MQTT_CONNECT );

    /* Create the wait semaphore so notifications don't crash. The value of
     * this semaphore will not be checked, so the maxValue argument is arbitrary. */
    TEST_ASSERT_EQUAL_INT( true, IotSemaphore_Create( &( connect.u.operation.notify.waitSemaphore ),
                                                      0,
                                                      10 ) );

    /* An incomplete CONNACK should not be processed, and no status should be set. */
    {
        DECLARE_PACKET( _pConnackTemplate, pConnack, connackSize );
        _operationResetAndPush( &connect );
        TEST_ASSERT_EQUAL_INT( true, _processBuffer( &connect,
                                                     pConnack,
                                                     connackSize - 1,
                                                     IOT_MQTT_STATUS_PENDING ) );

        /* Network close should have been called for invalid packet. */
        TEST_ASSERT_EQUAL_INT( true, _networkCloseCalled );
        TEST_ASSERT_EQUAL_INT( true, _disconnectCallbackCalled );
        _networkCloseCalled = false;
        _disconnectCallbackCalled = false;
    }

    /* The CONNACK control packet type must be 0x20. */
    {
        DECLARE_PACKET( _pConnackTemplate, pConnack, connackSize );
        pConnack[ 0 ] = 0x21;
        TEST_ASSERT_EQUAL_INT( true, _processBuffer( &connect,
                                                     pConnack,
                                                     connackSize,
                                                     IOT_MQTT_BAD_RESPONSE ) );

        /* Network close should have been called for invalid packet. */
        TEST_ASSERT_EQUAL_INT( true, _networkCloseCalled );
        TEST_ASSERT_EQUAL_INT( true, _disconnectCallbackCalled );
        _networkCloseCalled = false;
        _disconnectCallbackCalled = false;
    }

    /* A CONNACK must have a remaining length of 2. */
    {
        DECLARE_PACKET( _pConnackTemplate, pConnack, connackSize );
        pConnack[ 1 ] = 0x03;
        _operationResetAndPush( &connect );
        TEST_ASSERT_EQUAL_INT( true, _processBuffer( &connect,
                                                     pConnack,
                                                     connackSize,
                                                     IOT_MQTT_STATUS_PENDING ) );

        /* Network close should have been called for invalid packet. */
        TEST_ASSERT_EQUAL_INT( true, _networkCloseCalled );
        TEST_ASSERT_EQUAL_INT( true, _disconnectCallbackCalled );
        _networkCloseCalled = false;
        _disconnectCallbackCalled = false;
    }

    /* The reserved bits in CONNACK must be 0. */
    {
        DECLARE_PACKET( _pConnackTemplate, pConnack, connackSize );
        pConnack[ 2 ] = 0x80;
        TEST_ASSERT_EQUAL_INT( true, _processBuffer( &connect,
                                                     pConnack,
                                                     connackSize,
                                                     IOT_MQTT_BAD_RESPONSE ) );

        /* Network close should have been called for invalid packet. */
        TEST_ASSERT_EQUAL_INT( true, _networkCloseCalled );
        TEST_ASSERT_EQUAL_INT( true, _disconnectCallbackCalled );
        _networkCloseCalled = false;
        _disconnectCallbackCalled = false;
    }

    /* The fourth byte of CONNACK must be 0 if the SP flag is set. */
    {
        DECLARE_PACKET( _pConnackTemplate, pConnack, connackSize );
        pConnack[ 2 ] = 0x01;
        pConnack[ 3 ] = 0x01;
        _operationResetAndPush( &connect );
        TEST_ASSERT_EQUAL_INT( true, _processBuffer( &connect,
                                                     pConnack,
                                                     connackSize,
                                                     IOT_MQTT_BAD_RESPONSE ) );

        /* Network close should have been called for invalid packet. */
        TEST_ASSERT_EQUAL_INT( true, _networkCloseCalled );
        TEST_ASSERT_EQUAL_INT( true, _disconnectCallbackCalled );
        _networkCloseCalled = false;
        _disconnectCallbackCalled = false;
    }

    /* CONNACK return codes cannot be above 5. */
    {
        DECLARE_PACKET( _pConnackTemplate, pConnack, connackSize );
        pConnack[ 3 ] = 0x06;
        _operationResetAndPush( &connect );
        TEST_ASSERT_EQUAL_INT( true, _processBuffer( &connect,
                                                     pConnack,
                                                     connackSize,
                                                     IOT_MQTT_BAD_RESPONSE ) );

        /* Network close should have been called for invalid packet. */
        TEST_ASSERT_EQUAL_INT( true, _networkCloseCalled );
        TEST_ASSERT_EQUAL_INT( true, _disconnectCallbackCalled );
        _networkCloseCalled = false;
        _disconnectCallbackCalled = false;
    }

    IotSemaphore_Destroy( &( connect.u.operation.notify.waitSemaphore ) );
}

/*-----------------------------------------------------------*/

/**
 * @brief Tests the behavior of @ref mqtt_function_receivecallback with a spec-compliant
 * PUBLISH.
 */
TEST( MQTT_Unit_Receive, PublishValid )
{
    /* Process a valid QoS 0 PUBLISH. */
    {
        DECLARE_PACKET( _pPublishTemplate, pPublish, publishSize );
        TEST_ASSERT_EQUAL_INT( true, _processPublish( pPublish,
                                                      publishSize,
                                                      1 ) );
    }

    /* Process a valid QoS 1 PUBLISH. Prevent an attempt to send PUBACK by
     * making no memory available for the PUBACK. */
    {
        DECLARE_PACKET( _pPublishTemplate, pPublish, publishSize );
        pPublish[ 0 ] = 0x32;
        TEST_ASSERT_EQUAL_INT( true, _processPublish( pPublish,
                                                      publishSize,
                                                      1 ) );
    }

    /* Process a valid QoS 2 PUBLISH. */
    {
        DECLARE_PACKET( _pPublishTemplate, pPublish, publishSize );
        pPublish[ 0 ] = 0x34;
        TEST_ASSERT_EQUAL_INT( true, _processPublish( pPublish,
                                                      publishSize,
                                                      1 ) );
    }

    /* Process a valid PUBLISH with DUP flag set. */
    {
        DECLARE_PACKET( _pPublishTemplate, pPublish, publishSize );
        pPublish[ 0 ] = 0x38;
        TEST_ASSERT_EQUAL_INT( true, _processPublish( pPublish,
                                                      publishSize,
                                                      1 ) );
    }

    /* Remove the subscription. Even though there is no matching subscription,
     * all bytes of the PUBLISH should still be processed (should not crash). */
    {
        DECLARE_PACKET( _pPublishTemplate, pPublish, publishSize );

        _IotMqtt_RemoveSubscriptionByTopicFilter( _pMqttConnection,
                                                  &_subscription,
                                                  1 );

        TEST_ASSERT_EQUAL_INT( true, _processPublish( pPublish,
                                                      publishSize,
                                                      0 ) );
    }

    /* Network close function should not have been invoked. */
    TEST_ASSERT_EQUAL_INT( false, _networkCloseCalled );
    TEST_ASSERT_EQUAL_INT( false, _disconnectCallbackCalled );
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
        DECLARE_PACKET( _pPublishTemplate, pPublish, publishSize );
        TEST_ASSERT_EQUAL_INT( true, _processPublish( pPublish,
                                                      4,
                                                      0 ) );

        /* Network close should have been called for invalid packet. */
        TEST_ASSERT_EQUAL_INT( true, _networkCloseCalled );
        TEST_ASSERT_EQUAL_INT( true, _disconnectCallbackCalled );
        _networkCloseCalled = false;
        _disconnectCallbackCalled = false;
    }

    /* The PUBLISH cannot have a QoS of 3. */
    {
        DECLARE_PACKET( _pPublishTemplate, pPublish, publishSize );
        pPublish[ 0 ] = 0x36;
        TEST_ASSERT_EQUAL_INT( true, _processPublish( pPublish,
                                                      publishSize,
                                                      0 ) );

        /* Network close should have been called for invalid packet. */
        TEST_ASSERT_EQUAL_INT( true, _networkCloseCalled );
        TEST_ASSERT_EQUAL_INT( true, _disconnectCallbackCalled );
        _networkCloseCalled = false;
        _disconnectCallbackCalled = false;
    }

    /* Attempt to process a PUBLISH with an invalid "Remaining length". */
    {
        DECLARE_PACKET( _pPublishTemplate, pPublish, publishSize );
        pPublish[ 1 ] = 0xff;
        pPublish[ 2 ] = 0xff;
        pPublish[ 3 ] = 0xff;
        pPublish[ 4 ] = 0xff;
        TEST_ASSERT_EQUAL_INT( true, _processPublish( pPublish,
                                                      publishSize,
                                                      0 ) );

        /* Network close should have been called for invalid packet. */
        TEST_ASSERT_EQUAL_INT( true, _networkCloseCalled );
        TEST_ASSERT_EQUAL_INT( true, _disconnectCallbackCalled );
        _networkCloseCalled = false;
        _disconnectCallbackCalled = false;
    }

    /* Attempt to process a PUBLISH where some bytes could not be received. */
    {
        DECLARE_PACKET( _pPublishTemplate, pPublish, publishSize );
        pPublish[ 2 ] = 0x03;
        TEST_ASSERT_EQUAL_INT( true, _processPublish( pPublish,
                                                      publishSize,
                                                      0 ) );

        /* Network close should have been called for invalid packet. */
        TEST_ASSERT_EQUAL_INT( true, _networkCloseCalled );
        TEST_ASSERT_EQUAL_INT( true, _disconnectCallbackCalled );
        _networkCloseCalled = false;
        _disconnectCallbackCalled = false;
    }

    /* Attempt to process a PUBLISH with a "Remaining length" smaller than the
     * spec allows. */
    {
        /* QoS 0. */
        DECLARE_PACKET( _pPublishTemplate, pPublish0, publish0Size );
        pPublish0[ 1 ] = 0x02;
        TEST_ASSERT_EQUAL_INT( true, _processPublish( pPublish0,
                                                      publish0Size,
                                                      0 ) );

        /* Network close should have been called for invalid packet. */
        TEST_ASSERT_EQUAL_INT( true, _networkCloseCalled );
        _networkCloseCalled = false;

        /* QoS 1. */
        DECLARE_PACKET( _pPublishTemplate, pPublish1, publish1Size );
        pPublish1[ 0 ] = 0x32;
        pPublish1[ 1 ] = 0x04;
        TEST_ASSERT_EQUAL_INT( true, _processPublish( pPublish1,
                                                      publish1Size,
                                                      0 ) );

        /* Network close should have been called for invalid packet. */
        TEST_ASSERT_EQUAL_INT( true, _networkCloseCalled );
        TEST_ASSERT_EQUAL_INT( true, _disconnectCallbackCalled );
        _networkCloseCalled = false;
        _disconnectCallbackCalled = false;
    }

    /* Attempt to process a PUBLISH with a "Remaining length" smaller than the
     * end of the variable header. */
    {
        DECLARE_PACKET( _pPublishTemplate, pPublish, publishSize );
        pPublish[ 1 ] = 0x0a;
        TEST_ASSERT_EQUAL_INT( true, _processPublish( pPublish,
                                                      publishSize,
                                                      0 ) );

        /* Network close should have been called for invalid packet. */
        TEST_ASSERT_EQUAL_INT( true, _networkCloseCalled );
        TEST_ASSERT_EQUAL_INT( true, _disconnectCallbackCalled );
        _networkCloseCalled = false;
        _disconnectCallbackCalled = false;
    }

    /* Attempt to process a PUBLISH with packet identifier 0. */
    {
        DECLARE_PACKET( _pPublishTemplate, pPublish, publishSize );
        pPublish[ 0 ] = 0x32;
        pPublish[ 17 ] = 0x00;
        TEST_ASSERT_EQUAL_INT( true, _processPublish( pPublish,
                                                      publishSize,
                                                      0 ) );

        /* Network close should have been called for invalid packet. */
        TEST_ASSERT_EQUAL_INT( true, _networkCloseCalled );
        TEST_ASSERT_EQUAL_INT( true, _disconnectCallbackCalled );
        _networkCloseCalled = false;
        _disconnectCallbackCalled = false;
    }
}

/*-----------------------------------------------------------*/

/**
 * @brief Tests the behavior of @ref mqtt_function_receivecallback with a
 * spec-compliant PUBACK.
 */
TEST( MQTT_Unit_Receive, PubackValid )
{
    int8_t contextIndex = -1;
    _mqttOperation_t publish = INITIALIZE_OPERATION( IOT_MQTT_PUBLISH_TO_SERVER );

    /* Create the wait semaphore so notifications don't crash. The value of
     * this semaphore will not be checked, so the maxValue argument is arbitrary. */
    TEST_ASSERT_EQUAL_INT( true, IotSemaphore_Create( &( publish.u.operation.notify.waitSemaphore ),
                                                      0,
                                                      10 ) );

    /* Set the content in the state records for receiving a PUBACK. */
    contextIndex = _IotMqtt_getContextIndexFromConnection( _pMqttConnection );
    TEST_ASSERT_NOT_EQUAL( -1, contextIndex );

    /* Add a publish record at the start index. */
    connToContext[ contextIndex ].context.outgoingPublishRecords[ 0 ].packetId = 1U;
    connToContext[ contextIndex ].context.outgoingPublishRecords[ 0 ].publishState = MQTTPubAckPending;
    connToContext[ contextIndex ].context.outgoingPublishRecords[ 0 ].qos = MQTTQoS1;

    /* Even though no PUBLISH is in the receive queue, 4 bytes should still be
     * processed (should not crash). */
    {
        DECLARE_PACKET( _pPubackTemplate, pPuback, pubackSize );
        TEST_ASSERT_EQUAL_INT( true, _processBuffer( &publish,
                                                     pPuback,
                                                     pubackSize,
                                                     IOT_MQTT_STATUS_PENDING ) );
    }

    /* The state record should be deleted after receiving a PUBACK. */
    TEST_ASSERT_EQUAL_INT( MQTT_PACKET_ID_INVALID, connToContext[ contextIndex ].context.outgoingPublishRecords[ 0 ].packetId );

    /* Add a publish record at the start index. */
    connToContext[ contextIndex ].context.outgoingPublishRecords[ 0 ].packetId = 1U;
    connToContext[ contextIndex ].context.outgoingPublishRecords[ 0 ].publishState = MQTTPubAckPending;
    connToContext[ contextIndex ].context.outgoingPublishRecords[ 0 ].qos = MQTTQoS1;

    /* Process a valid PUBACK. */
    {
        DECLARE_PACKET( _pPubackTemplate, pPuback, pubackSize );
        _operationResetAndPush( &publish );
        TEST_ASSERT_EQUAL_INT( true, _processBuffer( &publish,
                                                     pPuback,
                                                     pubackSize,
                                                     IOT_MQTT_SUCCESS ) );
    }

    /* The state record should be deleted after receiving a PUBACK. */
    TEST_ASSERT_EQUAL_INT( MQTT_PACKET_ID_INVALID, connToContext[ contextIndex ].context.outgoingPublishRecords[ 0 ].packetId );

    IotSemaphore_Destroy( &( publish.u.operation.notify.waitSemaphore ) );

    /* Network close function should not have been invoked. */
    TEST_ASSERT_EQUAL_INT( false, _networkCloseCalled );
    TEST_ASSERT_EQUAL_INT( false, _disconnectCallbackCalled );
}

/*-----------------------------------------------------------*/

/**
 * @brief Tests the behavior of @ref mqtt_function_receivecallback with a PUBACK
 * that doesn't comply to MQTT spec.
 */
TEST( MQTT_Unit_Receive, PubackInvalid )
{
    _mqttOperation_t publish = INITIALIZE_OPERATION( IOT_MQTT_PUBLISH_TO_SERVER );

    /* Create the wait semaphore so notifications don't crash. The value of
     * this semaphore will not be checked, so the maxValue argument is arbitrary. */
    TEST_ASSERT_EQUAL_INT( true, IotSemaphore_Create( &( publish.u.operation.notify.waitSemaphore ),
                                                      0,
                                                      10 ) );

    _operationResetAndPush( &publish );

    /* An incomplete PUBACK should not be processed, and no status should be set. */
    {
        DECLARE_PACKET( _pPubackTemplate, pPuback, pubackSize );
        TEST_ASSERT_EQUAL_INT( true, _processBuffer( &publish,
                                                     pPuback,
                                                     pubackSize - 1,
                                                     IOT_MQTT_STATUS_PENDING ) );

        /* Network close should have been called for invalid packet. */
        TEST_ASSERT_EQUAL_INT( true, _networkCloseCalled );
        TEST_ASSERT_EQUAL_INT( true, _disconnectCallbackCalled );
        _networkCloseCalled = false;
        _disconnectCallbackCalled = false;
    }

    /* The PUBACK control packet type must be 0x40. */
    {
        DECLARE_PACKET( _pPubackTemplate, pPuback, pubackSize );
        pPuback[ 0 ] = 0x41;
        TEST_ASSERT_EQUAL_INT( true, _processBuffer( &publish,
                                                     pPuback,
                                                     pubackSize,
                                                     IOT_MQTT_STATUS_PENDING ) );

        /* Network close should have been called for invalid packet. */
        TEST_ASSERT_EQUAL_INT( true, _networkCloseCalled );
        TEST_ASSERT_EQUAL_INT( true, _disconnectCallbackCalled );
        _networkCloseCalled = false;
        _disconnectCallbackCalled = false;
    }

    /* Deserializing the PUBACK packet using the lightweight API doesnot serialize the packet id if the packet type
     * is not valid and as a result operation cannot be deleted from the pendingResponse list. So removing it explicitily. */
    IotListDouble_RemoveHead( &( _pMqttConnection->pendingResponse ) );
    _operationResetAndPush( &publish );

    /* A PUBACK must have a remaining length of 2. */
    {
        DECLARE_PACKET( _pPubackTemplate, pPuback, pubackSize );
        pPuback[ 1 ] = 0x03;
        TEST_ASSERT_EQUAL_INT( true, _processBuffer( &publish,
                                                     pPuback,
                                                     pubackSize,
                                                     IOT_MQTT_STATUS_PENDING ) );

        /* Network close should have been called for invalid packet. */
        TEST_ASSERT_EQUAL_INT( true, _networkCloseCalled );
        TEST_ASSERT_EQUAL_INT( true, _disconnectCallbackCalled );
        _networkCloseCalled = false;
        _disconnectCallbackCalled = false;
    }

    /* The packet identifier in PUBACK cannot be 0. No status should be set if
     * packet identifier 0 is received. */
    {
        DECLARE_PACKET( _pPubackTemplate, pPuback, pubackSize );
        pPuback[ 3 ] = 0x00;
        TEST_ASSERT_EQUAL_INT( true, _processBuffer( &publish,
                                                     pPuback,
                                                     pubackSize,
                                                     IOT_MQTT_STATUS_PENDING ) );

        /* Network close should have been called for invalid packet. */
        TEST_ASSERT_EQUAL_INT( true, _networkCloseCalled );
        TEST_ASSERT_EQUAL_INT( true, _disconnectCallbackCalled );
        _networkCloseCalled = false;
        _disconnectCallbackCalled = false;
    }

    /* Remove unprocessed PUBLISH if present. */
    if( IotLink_IsLinked( &( publish.link ) ) == true )
    {
        IotDeQueue_Remove( &( publish.link ) );
    }

    IotSemaphore_Destroy( &( publish.u.operation.notify.waitSemaphore ) );
}

/*-----------------------------------------------------------*/

/**
 * @brief Tests the behavior of @ref mqtt_function_receivecallback with a
 * spec-compliant SUBACK.
 */
TEST( MQTT_Unit_Receive, SubackValid )
{
    _mqttSubscription_t * pNewSubscription = NULL;
    _mqttOperation_t subscribe = INITIALIZE_OPERATION( IOT_MQTT_SUBSCRIBE );
    IotMqttSubscription_t pSubscriptions[ 2 ] = { IOT_MQTT_SUBSCRIPTION_INITIALIZER };
    int8_t contextIndex = -1;

    contextIndex = _IotMqtt_getContextIndexFromConnection( _pMqttConnection );

    /* Create the wait semaphore so notifications don't crash. The value of
     * this semaphore will not be checked, so the maxValue argument is arbitrary. */
    TEST_ASSERT_EQUAL_INT( true, IotSemaphore_Create( &( subscribe.u.operation.notify.waitSemaphore ),
                                                      0,
                                                      10 ) );

    /* Add 2 additional subscriptions to the MQTT connection. */
    pSubscriptions[ 0 ].qos = IOT_MQTT_QOS_1;
    pSubscriptions[ 0 ].callback.function = _publishCallback;
    pSubscriptions[ 0 ].pTopicFilter = TEST_TOPIC_NAME "1";
    pSubscriptions[ 0 ].topicFilterLength = TEST_TOPIC_LENGTH + 1;

    pSubscriptions[ 1 ].qos = IOT_MQTT_QOS_1;
    pSubscriptions[ 1 ].callback.function = _publishCallback;
    pSubscriptions[ 1 ].pTopicFilter = TEST_TOPIC_NAME "2";
    pSubscriptions[ 1 ].topicFilterLength = TEST_TOPIC_LENGTH + 1;

    TEST_ASSERT_EQUAL( IOT_MQTT_SUCCESS, _IotMqtt_AddSubscriptions( _pMqttConnection,
                                                                    1,
                                                                    pSubscriptions,
                                                                    2 ) );


    ( connToContext[ contextIndex ].subscriptionArray[ 1 ] ).packetInfo.order = 1;
    ( connToContext[ contextIndex ].subscriptionArray[ 2 ] ).packetInfo.order = 2;

    /* Even though no SUBSCRIBE is in the receive queue, all bytes of the SUBACK
     * should still be processed (should not crash). */
    {
        DECLARE_PACKET( _pSubackTemplate, pSuback, subackSize );
        TEST_ASSERT_EQUAL_INT( true, _processBuffer( &subscribe,
                                                     pSuback,
                                                     subackSize,
                                                     IOT_MQTT_STATUS_PENDING ) );
    }

    /* Process a valid SUBACK where all subscriptions are successful. */
    {
        IotMqttSubscription_t currentSubscription = IOT_MQTT_SUBSCRIPTION_INITIALIZER;
        DECLARE_PACKET( _pSubackTemplate, pSuback, subackSize );
        _operationResetAndPush( &subscribe );
        TEST_ASSERT_EQUAL_INT( true, _processBuffer( &subscribe,
                                                     pSuback,
                                                     subackSize,
                                                     IOT_MQTT_SUCCESS ) );

        /* Test the subscription check function. QoS is not tested. */
        TEST_ASSERT_EQUAL_INT( true, IotMqtt_IsSubscribed( _pMqttConnection,
                                                           pSubscriptions[ 0 ].pTopicFilter,
                                                           pSubscriptions[ 0 ].topicFilterLength,
                                                           &currentSubscription ) );
        TEST_ASSERT_EQUAL_UINT16( currentSubscription.topicFilterLength,
                                  pSubscriptions[ 0 ].topicFilterLength );
        TEST_ASSERT_EQUAL_STRING_LEN( currentSubscription.pTopicFilter,
                                      pSubscriptions[ 0 ].pTopicFilter,
                                      currentSubscription.topicFilterLength );
        TEST_ASSERT_EQUAL_PTR( currentSubscription.callback.function,
                               pSubscriptions[ 0 ].callback.function );
        TEST_ASSERT_EQUAL_PTR( currentSubscription.callback.pCallbackContext,
                               pSubscriptions[ 0 ].callback.pCallbackContext );
    }

    /* Process a valid SUBACK where some subscriptions were rejected. */
    {
        DECLARE_PACKET( _pSubackTemplate, pSuback, subackSize );
        pSuback[ 4 ] = 0x80;
        pSuback[ 6 ] = 0x80;
        _operationResetAndPush( &subscribe );
        TEST_ASSERT_EQUAL_INT( true, _processBuffer( &subscribe,
                                                     pSuback,
                                                     subackSize,
                                                     IOT_MQTT_SERVER_REFUSED ) );

        /* Check that rejected subscriptions were removed from the subscription
         * list. */
        TEST_ASSERT_EQUAL_INT( false, IotMqtt_IsSubscribed( _pMqttConnection,
                                                            TEST_TOPIC_NAME,
                                                            TEST_TOPIC_LENGTH,
                                                            NULL ) );
        TEST_ASSERT_EQUAL_INT( false, IotMqtt_IsSubscribed( _pMqttConnection,
                                                            pSubscriptions[ 1 ].pTopicFilter,
                                                            pSubscriptions[ 1 ].topicFilterLength,
                                                            NULL ) );
    }

    IotSemaphore_Destroy( &( subscribe.u.operation.notify.waitSemaphore ) );

    /* Network close function should not have been invoked. */
    TEST_ASSERT_EQUAL_INT( false, _networkCloseCalled );
    TEST_ASSERT_EQUAL_INT( false, _disconnectCallbackCalled );
}

/*-----------------------------------------------------------*/

/**
 * @brief Tests the behavior of @ref mqtt_function_receivecallback with a
 * SUBACK that doesn't comply to MQTT spec.
 */
TEST( MQTT_Unit_Receive, SubackInvalid )
{
    _mqttOperation_t subscribe = INITIALIZE_OPERATION( IOT_MQTT_SUBSCRIBE );

    /* Create the wait semaphore so notifications don't crash. The value of
     * this semaphore will not be checked, so the maxValue argument is arbitrary. */
    TEST_ASSERT_EQUAL_INT( true, IotSemaphore_Create( &( subscribe.u.operation.notify.waitSemaphore ),
                                                      0,
                                                      10 ) );

    _operationResetAndPush( &subscribe );

    /* Attempting to process a packet smaller than 5 bytes should result in no
     * bytes processed. 5 bytes is the minimum size of a SUBACK. */
    {
        DECLARE_PACKET( _pSubackTemplate, pSuback, subackSize );
        TEST_ASSERT_EQUAL_INT( true, _processBuffer( &subscribe,
                                                     pSuback,
                                                     4,
                                                     IOT_MQTT_STATUS_PENDING ) );

        /* Network close should have been called for invalid packet. */
        TEST_ASSERT_EQUAL_INT( true, _networkCloseCalled );
        TEST_ASSERT_EQUAL_INT( true, _disconnectCallbackCalled );
        _networkCloseCalled = false;
        _disconnectCallbackCalled = false;
    }

    /* Attempt to process a SUBACK with an invalid "Remaining length". */
    {
        DECLARE_PACKET( _pSubackTemplate, pSuback, subackSize );
        pSuback[ 1 ] = 0xff;
        pSuback[ 2 ] = 0xff;
        pSuback[ 3 ] = 0xff;
        pSuback[ 4 ] = 0xff;
        TEST_ASSERT_EQUAL_INT( true, _processBuffer( &subscribe,
                                                     pSuback,
                                                     subackSize,
                                                     IOT_MQTT_STATUS_PENDING ) );

        /* Network close should have been called for invalid packet. */
        TEST_ASSERT_EQUAL_INT( true, _networkCloseCalled );
        TEST_ASSERT_EQUAL_INT( true, _disconnectCallbackCalled );
        _networkCloseCalled = false;
        _disconnectCallbackCalled = false;
    }

    /* Attempt to process a SUBACK larger than the size of the data stream. */
    {
        DECLARE_PACKET( _pSubackTemplate, pSuback, subackSize );
        pSuback[ 1 ] = 0x52;
        TEST_ASSERT_EQUAL_INT( true, _processBuffer( &subscribe,
                                                     pSuback,
                                                     subackSize,
                                                     IOT_MQTT_STATUS_PENDING ) );

        /* Network close should have been called for invalid packet. */
        TEST_ASSERT_EQUAL_INT( true, _networkCloseCalled );
        TEST_ASSERT_EQUAL_INT( true, _disconnectCallbackCalled );
        _networkCloseCalled = false;
        _disconnectCallbackCalled = false;
    }

    /* Attempt to process a SUBACK with a "Remaining length" smaller than the
     * spec allows. */
    {
        DECLARE_PACKET( _pSubackTemplate, pSuback, subackSize );
        pSuback[ 1 ] = 0x02;
        TEST_ASSERT_EQUAL_INT( true, _processBuffer( &subscribe,
                                                     pSuback,
                                                     subackSize,
                                                     IOT_MQTT_STATUS_PENDING ) );

        /* Network close should have been called for invalid packet. */
        TEST_ASSERT_EQUAL_INT( true, _networkCloseCalled );
        TEST_ASSERT_EQUAL_INT( true, _disconnectCallbackCalled );
        _networkCloseCalled = false;
        _disconnectCallbackCalled = false;
    }

    /* Attempt to process a SUBACK with a bad return code. */
    {
        DECLARE_PACKET( _pSubackTemplate, pSuback, subackSize );
        pSuback[ 6 ] = 0xff;
        TEST_ASSERT_EQUAL_INT( true, _processBuffer( &subscribe,
                                                     pSuback,
                                                     subackSize,
                                                     IOT_MQTT_BAD_RESPONSE ) );

        /* Network close should have been called for invalid packet. */
        TEST_ASSERT_EQUAL_INT( true, _networkCloseCalled );
        TEST_ASSERT_EQUAL_INT( true, _disconnectCallbackCalled );
        _networkCloseCalled = false;
        _disconnectCallbackCalled = false;
    }

    /* The SUBACK control packet type must be 0x90. */
    {
        DECLARE_PACKET( _pSubackTemplate, pSuback, subackSize );
        pSuback[ 0 ] = 0x91;
        TEST_ASSERT_EQUAL_INT( true, _processBuffer( &subscribe,
                                                     pSuback,
                                                     subackSize,
                                                     IOT_MQTT_BAD_RESPONSE ) );

        /* Network close should have been called for invalid packet. */
        TEST_ASSERT_EQUAL_INT( true, _networkCloseCalled );
        TEST_ASSERT_EQUAL_INT( true, _disconnectCallbackCalled );
        _networkCloseCalled = false;
        _disconnectCallbackCalled = false;
    }

    IotSemaphore_Destroy( &( subscribe.u.operation.notify.waitSemaphore ) );
}

/*-----------------------------------------------------------*/

/**
 * @brief Tests the behavior of @ref mqtt_function_receivecallback with a
 * spec-compliant UNSUBACK.
 */
TEST( MQTT_Unit_Receive, UnsubackValid )
{
    _mqttOperation_t unsubscribe = INITIALIZE_OPERATION( IOT_MQTT_UNSUBSCRIBE );

    /* Create the wait semaphore so notifications don't crash. The value of
     * this semaphore will not be checked, so the maxValue argument is arbitrary. */
    TEST_ASSERT_EQUAL_INT( true, IotSemaphore_Create( &( unsubscribe.u.operation.notify.waitSemaphore ),
                                                      0,
                                                      10 ) );

    /* Even though no UNSUBSCRIBE is in the receive queue, 4 bytes should still be
     * processed (should not crash). */
    {
        DECLARE_PACKET( _pUnsubackTemplate, pUnsuback, unsubackSize );
        TEST_ASSERT_EQUAL_INT( true, _processBuffer( &unsubscribe,
                                                     pUnsuback,
                                                     unsubackSize,
                                                     IOT_MQTT_STATUS_PENDING ) );
    }

    /* Process a valid UNSUBACK. */
    {
        DECLARE_PACKET( _pUnsubackTemplate, pUnsuback, unsubackSize );
        _operationResetAndPush( &unsubscribe );
        TEST_ASSERT_EQUAL_INT( true, _processBuffer( &unsubscribe,
                                                     pUnsuback,
                                                     unsubackSize,
                                                     IOT_MQTT_SUCCESS ) );
    }

    IotSemaphore_Destroy( &( unsubscribe.u.operation.notify.waitSemaphore ) );

    /* Network close function should not have been invoked. */
    TEST_ASSERT_EQUAL_INT( false, _networkCloseCalled );
    TEST_ASSERT_EQUAL_INT( false, _disconnectCallbackCalled );
}

/*-----------------------------------------------------------*/

/**
 * @brief Tests the behavior of @ref mqtt_function_receivecallback with an
 * UNSUBACK that doesn't comply to MQTT spec.
 */
TEST( MQTT_Unit_Receive, UnsubackInvalid )
{
    _mqttOperation_t unsubscribe = INITIALIZE_OPERATION( IOT_MQTT_UNSUBSCRIBE );

    /* Create the wait semaphore so notifications don't crash. The value of
     * this semaphore will not be checked, so the maxValue argument is arbitrary. */
    TEST_ASSERT_EQUAL_INT( true, IotSemaphore_Create( &( unsubscribe.u.operation.notify.waitSemaphore ),
                                                      0,
                                                      10 ) );

    _operationResetAndPush( &unsubscribe );

    /* An incomplete UNSUBACK should not be processed, and no status should be set. */
    {
        DECLARE_PACKET( _pUnsubackTemplate, pUnsuback, unsubackSize );
        TEST_ASSERT_EQUAL_INT( true, _processBuffer( &unsubscribe,
                                                     pUnsuback,
                                                     unsubackSize - 1,
                                                     IOT_MQTT_STATUS_PENDING ) );

        /* Network close should have been called for invalid packet. */
        TEST_ASSERT_EQUAL_INT( true, _networkCloseCalled );
        TEST_ASSERT_EQUAL_INT( true, _disconnectCallbackCalled );
        _networkCloseCalled = false;
        _disconnectCallbackCalled = false;
    }

    /* The UNSUBACK control packet type must be 0xb0. */
    {
        DECLARE_PACKET( _pUnsubackTemplate, pUnsuback, unsubackSize );
        pUnsuback[ 0 ] = 0xb1;
        TEST_ASSERT_EQUAL_INT( true, _processBuffer( &unsubscribe,
                                                     pUnsuback,
                                                     unsubackSize,
                                                     IOT_MQTT_STATUS_PENDING ) );

        /* Network close should have been called for invalid packet. */
        TEST_ASSERT_EQUAL_INT( true, _networkCloseCalled );
        TEST_ASSERT_EQUAL_INT( true, _disconnectCallbackCalled );
        _networkCloseCalled = false;
        _disconnectCallbackCalled = false;
    }

    /* Deserializing the UNSUBACK packet using the lightweight API doesnot serialize the packet id if the packet type
     * is not valid and as a result operation cannot be deleted from the pendingResponse list. So removing it explicitily. */
    IotListDouble_RemoveHead( &( _pMqttConnection->pendingResponse ) );
    _operationResetAndPush( &unsubscribe );

    /* An UNSUBACK must have a remaining length of 2. */
    {
        DECLARE_PACKET( _pUnsubackTemplate, pUnsuback, unsubackSize );
        pUnsuback[ 1 ] = 0x03;
        TEST_ASSERT_EQUAL_INT( true, _processBuffer( &unsubscribe,
                                                     pUnsuback,
                                                     unsubackSize,
                                                     IOT_MQTT_STATUS_PENDING ) );

        /* Network close should have been called for invalid packet. */
        TEST_ASSERT_EQUAL_INT( true, _networkCloseCalled );
        TEST_ASSERT_EQUAL_INT( true, _disconnectCallbackCalled );
        _networkCloseCalled = false;
        _disconnectCallbackCalled = false;
    }

    /* The packet identifier in UNSUBACK cannot be 0. No status should be set if
     * packet identifier 0 is received. */
    {
        DECLARE_PACKET( _pUnsubackTemplate, pUnsuback, unsubackSize );
        pUnsuback[ 3 ] = 0x00;
        TEST_ASSERT_EQUAL_INT( true, _processBuffer( &unsubscribe,
                                                     pUnsuback,
                                                     unsubackSize,
                                                     IOT_MQTT_STATUS_PENDING ) );

        /* Network close should have been called for invalid packet. */
        TEST_ASSERT_EQUAL_INT( true, _networkCloseCalled );
        TEST_ASSERT_EQUAL_INT( true, _disconnectCallbackCalled );
        _networkCloseCalled = false;
        _disconnectCallbackCalled = false;
    }

    /* Remove unprocessed UNSUBSCRIBE if present. */
    if( IotLink_IsLinked( &( unsubscribe.link ) ) == true )
    {
        IotDeQueue_Remove( &( unsubscribe.link ) );
    }

    IotSemaphore_Destroy( &( unsubscribe.u.operation.notify.waitSemaphore ) );
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

        DECLARE_PACKET( _pPingrespTemplate, pPingresp, pingrespSize );
        TEST_ASSERT_EQUAL_INT( true, _processBuffer( NULL,
                                                     pPingresp,
                                                     pingrespSize,
                                                     IOT_MQTT_SUCCESS ) );

        TEST_ASSERT_EQUAL_INT( false, _pMqttConnection->keepAliveFailure );
        TEST_ASSERT_EQUAL_INT( false, _networkCloseCalled );
        TEST_ASSERT_EQUAL_INT( false, _disconnectCallbackCalled );
    }

    /* Process a valid PINGRESP. */
    {
        _pMqttConnection->keepAliveFailure = true;

        DECLARE_PACKET( _pPingrespTemplate, pPingresp, pingrespSize );
        TEST_ASSERT_EQUAL_INT( true, _processBuffer( NULL,
                                                     pPingresp,
                                                     pingrespSize,
                                                     IOT_MQTT_SUCCESS ) );

        TEST_ASSERT_EQUAL_INT( false, _pMqttConnection->keepAliveFailure );
        TEST_ASSERT_EQUAL_INT( false, _networkCloseCalled );
        TEST_ASSERT_EQUAL_INT( false, _disconnectCallbackCalled );
    }

    /* An incomplete PINGRESP should not be processed, and the keep-alive failure
     * flag should not be cleared. */
    {
        _pMqttConnection->keepAliveFailure = true;

        DECLARE_PACKET( _pPingrespTemplate, pPingresp, pingrespSize );
        TEST_ASSERT_EQUAL_INT( true, _processBuffer( NULL,
                                                     pPingresp,
                                                     pingrespSize - 1,
                                                     IOT_MQTT_SUCCESS ) );

        TEST_ASSERT_EQUAL_INT( true, _pMqttConnection->keepAliveFailure );
        TEST_ASSERT_EQUAL_INT( true, _networkCloseCalled );
        TEST_ASSERT_EQUAL_INT( true, _disconnectCallbackCalled );
        _networkCloseCalled = false;
        _disconnectCallbackCalled = false;
    }

    /* A PINGRESP should have a remaining length of 0. */
    {
        _pMqttConnection->keepAliveFailure = true;

        DECLARE_PACKET( _pPingrespTemplate, pPingresp, pingrespSize );
        pPingresp[ 1 ] = 0x01;
        TEST_ASSERT_EQUAL_INT( true, _processBuffer( NULL,
                                                     pPingresp,
                                                     pingrespSize,
                                                     IOT_MQTT_SUCCESS ) );

        TEST_ASSERT_EQUAL_INT( true, _pMqttConnection->keepAliveFailure );
        TEST_ASSERT_EQUAL_INT( true, _networkCloseCalled );
        TEST_ASSERT_EQUAL_INT( true, _disconnectCallbackCalled );
        _networkCloseCalled = false;
        _disconnectCallbackCalled = false;
    }

    /* The PINGRESP control packet type must be 0xd0. */
    {
        _pMqttConnection->keepAliveFailure = true;

        DECLARE_PACKET( _pPingrespTemplate, pPingresp, pingrespSize );
        pPingresp[ 0 ] = 0xd1;
        TEST_ASSERT_EQUAL_INT( true, _processBuffer( NULL,
                                                     pPingresp,
                                                     pingrespSize,
                                                     IOT_MQTT_SUCCESS ) );

        TEST_ASSERT_EQUAL_INT( true, _pMqttConnection->keepAliveFailure );
        TEST_ASSERT_EQUAL_INT( true, _networkCloseCalled );
        TEST_ASSERT_EQUAL_INT( true, _disconnectCallbackCalled );
        _networkCloseCalled = false;
        _disconnectCallbackCalled = false;
    }
}

/*-----------------------------------------------------------*/
