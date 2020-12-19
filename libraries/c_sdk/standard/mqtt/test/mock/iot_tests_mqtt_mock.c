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
 * @file iot_tests_mqtt_mock.c
 * @brief Implements functions to mock MQTT network responses.
 */

/* The config header is always included first. */
#include "iot_config.h"

/* Standard includes. */
#include <string.h>

/* MQTT internal includes. */
#include "private/iot_mqtt_internal.h"

/* MQTT types include. */
#include "types/iot_mqtt_types.h"

/* MQTT mock include. */
#include "iot_tests_mqtt_mock.h"

/* MQTT test access include. */
#include "iot_test_access_mqtt.h"

/* Error handling include. */
#include "private/iot_error.h"

/* Platform layer includes. */
#include "platform/iot_clock.h"
#include "platform/iot_threads.h"

/*-----------------------------------------------------------*/

/**
 * @brief A delay that simulates the time required for an MQTT packet to be sent
 * to the server and for the server to send a response.
 */
#define NETWORK_ROUND_TRIP_TIME_MS              ( 25 )

/**
 * @brief The maximum size of any MQTT acknowledgement packet (e.g. SUBACK,
 * PUBACK, UNSUBACK) used in these tests.
 */
#define ACKNOWLEDGEMENT_PACKET_SIZE             ( 5 )

/**
 * @brief Test packet type for MQTT PUBLISH header.
 */
#define TEST_MQTT_PACKET_TYPE_PUBLISH_HEADER    ( MQTT_PACKET_TYPE_PUBLISH + 1 )

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
 * @brief The MQTT connection object shared among all the tests.
 */
static _mqttConnection_t * _pMqttConnection = IOT_MQTT_CONNECTION_INITIALIZER;

/**
 * @brief The #IotNetworkInterface_t to share among the tests.
 */
static IotNetworkInterface_t _networkInterface = { 0 };

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

/**
 * @brief Receive context for the MQTT PUBLISH packet.
 *
 * MQTT library sends MQTT PUBLISH packet using 2 different transport calls.
 * First one is for sending the header and then the payload. This static
 * context variable stores the header and adds payload to it to form the
 * complete PUBLISH packet.
 */
static _receiveContext_t _publishContext = { 0 };

/**
 * @brief Fixed Size Array to hold Mapping of MQTT Connection used in MQTT 201906.00 library to MQTT Context
 * used in calling MQTT LTS API from shim to send packets on the network.
 */
extern _connContext_t connToContext[ MAX_NO_OF_MQTT_CONNECTIONS ];

/**
 * @brief Represents the network context used for the TLS session with the
 * server.
 */
static NetworkContext_t networkContext = { 0 };

/*-----------------------------------------------------------*/

/**
 * @brief Invokes the MQTT receive callback to simulate a response received from
 * the network.
 */
static void _receiveThread( void * pArgument )
{
    uint8_t pReceivedData[ ACKNOWLEDGEMENT_PACKET_SIZE ] = { 0 };
    _receiveContext_t receiveContext = { 0 };

    receiveContext.pData = pReceivedData;
    receiveContext.dataLength = ACKNOWLEDGEMENT_PACKET_SIZE;

    /* Silence warnings about unused parameters. */
    ( void ) pArgument;

    /* Lock mutex to read and process the last packet sent. */
    IotMutex_Lock( &_lastPacketMutex );

    /* Ensure that the last packet type and identifier were set. */
    IotTest_Assert( _lastPacketType != 0 );
    IotTest_Assert( _lastPacketIdentifier != 0 );

    /* Set the packet identifier in the ACK packet. */
    pReceivedData[ 2 ] = UINT16_HIGH_BYTE( _lastPacketIdentifier );
    pReceivedData[ 3 ] = UINT16_LOW_BYTE( _lastPacketIdentifier );

    /* Create the corresponding ACK packet based on the last packet type. */
    switch( _lastPacketType )
    {
        case MQTT_PACKET_TYPE_PUBLISH:

            pReceivedData[ 0 ] = MQTT_PACKET_TYPE_PUBACK;
            pReceivedData[ 1 ] = 2;
            receiveContext.dataLength = 4;
            break;

        case MQTT_PACKET_TYPE_SUBSCRIBE:

            pReceivedData[ 0 ] = MQTT_PACKET_TYPE_SUBACK;
            pReceivedData[ 1 ] = 3;
            pReceivedData[ 4 ] = 1;
            receiveContext.dataLength = 5;
            break;

        case MQTT_PACKET_TYPE_UNSUBSCRIBE:

            pReceivedData[ 0 ] = MQTT_PACKET_TYPE_UNSUBACK;
            pReceivedData[ 1 ] = 2;
            receiveContext.dataLength = 4;
            break;

        default:

            /* The only valid outgoing packets are PUBLISH, SUBSCRIBE, and
             * UNSUBSCRIBE. Abort if any other packet is found. */
            IotTest_Assert( 0 );
    }

    /* Call the MQTT receive callback to process the ACK packet. */
    IotMqtt_ReceiveCallback( &receiveContext,
                             _pMqttConnection );

    IotMutex_Unlock( &_lastPacketMutex );
}

/*-----------------------------------------------------------*/

/**
 * @brief A send function that always "succeeds". It also sets the receive
 * timer to respond with an ACK when necessary.
 */
static size_t _sendSuccess( void * pNetworkConnection,
                            const uint8_t * pMessage,
                            size_t messageLength )
{
    IotMqttError_t status = IOT_MQTT_STATUS_PENDING;
    _mqttOperation_t deserializedPublish = { .link = { 0 } };
    _mqttPacket_t mqttPacket = { .u.pMqttConnection = NULL };
    _receiveContext_t receiveContext = { 0 };

    /* Ignore the network connection. */
    ( void ) pNetworkConnection;

    /* Lock the mutex to modify the information on the last packet sent. */
    IotMutex_Lock( &_lastPacketMutex );

    if( _lastPacketType != TEST_MQTT_PACKET_TYPE_PUBLISH_HEADER )
    {
        /* Read the packet type, which is the first byte in the message. */
        mqttPacket.type = *pMessage;

        /* Set the members of the receive context. */
        receiveContext.pData = pMessage + 1;
        receiveContext.dataLength = messageLength;

        /* Read the remaining length. */
        mqttPacket.remainingLength = _IotMqtt_GetRemainingLength( &receiveContext,
                                                                  &_networkInterface );
        IotTest_Assert( mqttPacket.remainingLength != MQTT_REMAINING_LENGTH_INVALID );

        /* Set the last packet type based on the outgoing message. */
        switch( mqttPacket.type & 0xf0 )
        {
            case MQTT_PACKET_TYPE_PUBLISH:

                /* Only set the last packet type to PUBLISH for QoS 1. */
                if( ( ( *pMessage & 0x06 ) >> 1 ) == 1 )
                {
                    /* Check if only MQTT PUBLISH header is received. */
                    if( messageLength < mqttPacket.remainingLength )
                    {
                        /* PUBLISH header is received. */
                        _lastPacketType = TEST_MQTT_PACKET_TYPE_PUBLISH_HEADER;

                        /* Save the received data. */
                        memset( &_publishContext, 0x00, sizeof( _receiveContext_t ) );
                        /* Allocate a size of remaining length + first 2 bytes. */
                        _publishContext.pData = IotTest_Malloc( mqttPacket.remainingLength + sizeof( uint16_t ) );
                        AwsIotShadow_Assert( _publishContext.pData != NULL );

                        /* Copy the data received. */
                        memcpy( ( void * ) _publishContext.pData, pMessage, messageLength );
                        _publishContext.dataLength = messageLength;
                    }
                    else
                    {
                        _lastPacketType = MQTT_PACKET_TYPE_PUBLISH;
                    }
                }
                else
                {
                    _lastPacketType = 0;
                    _lastPacketIdentifier = 0;
                }

                break;

            case ( MQTT_PACKET_TYPE_SUBSCRIBE & 0xf0 ):
                _lastPacketType = MQTT_PACKET_TYPE_SUBSCRIBE;
                break;

            case ( MQTT_PACKET_TYPE_UNSUBSCRIBE & 0xf0 ):
                _lastPacketType = MQTT_PACKET_TYPE_UNSUBSCRIBE;
                break;

            case ( MQTT_PACKET_TYPE_DISCONNECT & 0xf0 ):
                _lastPacketType = 0;
                _lastPacketIdentifier = 0;
                break;

            default:

                /* The only valid outgoing packets are PUBLISH, SUBSCRIBE,
                 * UNSUBSCRIBE and DISCONNECT. Abort if any other packet is found. */
                IotTest_Assert( 0 );
        }
    }
    else
    {
        /* MQTT PUBLISH payload is received. Mark the completion of PUBLISH. */
        _lastPacketType = MQTT_PACKET_TYPE_PUBLISH;
    }

    /* Check if a network response is needed. */
    if( ( _lastPacketType != 0 ) && ( _lastPacketType != TEST_MQTT_PACKET_TYPE_PUBLISH_HEADER ) )
    {
        /* Save the packet identifier as the last packet identifier. */
        if( _lastPacketType != MQTT_PACKET_TYPE_PUBLISH )
        {
            _lastPacketIdentifier = UINT16_DECODE( receiveContext.pData + receiveContext.dataIndex );
            status = IOT_MQTT_SUCCESS;
        }
        else
        {
            mqttPacket.u.pIncomingPublish = &deserializedPublish;

            /* Check if MQTT PUBLISH packet is fully received. */
            if( _publishContext.dataLength == 0 )
            {
                mqttPacket.pRemainingData = ( uint8_t * ) pMessage + ( messageLength - mqttPacket.remainingLength );
            }
            else
            {
                AwsIotShadow_Assert( _publishContext.pData != NULL );
                /* Copy the remaining part of the MQTT PUBLISH. */
                memcpy( ( void * ) ( _publishContext.pData + _publishContext.dataLength ), pMessage, messageLength );
                _publishContext.dataLength += messageLength;

                /* Read the packet type, which is the first byte in the message. */
                mqttPacket.type = _publishContext.pData[ 0 ];

                /* Set the members of the receive context. */
                receiveContext.pData = _publishContext.pData + 1;
                receiveContext.dataLength = _publishContext.dataLength;

                /* Read the remaining length. */
                mqttPacket.remainingLength = _IotMqtt_GetRemainingLength( &receiveContext,
                                                                          &_networkInterface );

                mqttPacket.pRemainingData = ( uint8_t * ) _publishContext.pData +
                                            ( _publishContext.dataLength - mqttPacket.remainingLength );
            }

            status = _IotMqtt_deserializePublishWrapper( &mqttPacket );
            _lastPacketIdentifier = mqttPacket.packetIdentifier;

            /* Clear the publish data stored. */
            if( _publishContext.dataLength != 0 )
            {
                IotTest_Free( ( void * ) _publishContext.pData );
                memset( &_publishContext, 0x00, sizeof( _receiveContext_t ) );
            }
        }

        IotTest_Assert( status == IOT_MQTT_SUCCESS );
        IotTest_Assert( _lastPacketIdentifier != 0 );

        /* Set the receive thread to run after a "network round-trip". */
        ( void ) IotClock_TimerArm( &_receiveTimer,
                                    NETWORK_ROUND_TRIP_TIME_MS,
                                    0 );
    }

    IotMutex_Unlock( &_lastPacketMutex );

    /* Return the message length to simulate a successful send. */
    return messageLength;
}

/*-----------------------------------------------------------*/

/**
 * @brief Simulates a network receive function.
 */
static size_t _receive( void * pNetworkConnection,
                        uint8_t * pBuffer,
                        size_t bytesRequested )
{
    size_t bytesReceived = 0;
    _receiveContext_t * pReceiveContext = ( _receiveContext_t * ) pNetworkConnection;

    IotTest_Assert( bytesRequested != 0 );
    IotTest_Assert( pReceiveContext->dataIndex < pReceiveContext->dataLength );

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

    return bytesReceived;
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
 * @brief Transport send interface provided to the MQTT context used in calling MQTT LTS APIs.
 */
static int32_t transportSend( NetworkContext_t * pNetworkContext,
                              const void * pMessage,
                              size_t bytesToSend )
{
    int32_t bytesSent = 0;

    IotMqtt_Assert( pNetworkContext != NULL );
    IotMqtt_Assert( pMessage != NULL );

    /* Sending the bytes on the network using Network Interface. */
    bytesSent = pNetworkContext->pParams->pNetworkInterface->send( pNetworkContext->pParams->pNetworkConnection, ( const uint8_t * ) pMessage, bytesToSend );

    if( bytesSent < 0 )
    {
        /* Network Send Interface return negative value in case of any socket error,
         * unifying the error codes here for socket error and timeout to comply with the MQTT LTS Library.
         */
        bytesSent = -1;
    }

    return bytesSent;
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
 * @brief Setting the MQTT Context for the given MQTT Connection.
 *
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

bool IotTest_MqttMockInit( IotMqttConnection_t * pMqttConnection )
{
    bool status = true;
    IotMqttNetworkInfo_t networkInfo = IOT_MQTT_NETWORK_INFO_INITIALIZER;

    /* Flags to track clean up */
    bool packetMutexCreated = false, timerCreated = false;

    /* Set the network interface send and receive functions. */
    ( void ) memset( &_networkInterface, 0x00, sizeof( IotNetworkInterface_t ) );
    _networkInterface.send = _sendSuccess;
    _networkInterface.receive = _receive;
    networkInfo.pNetworkInterface = &_networkInterface;

    /* Create the mutex that synchronizes the receive callback and send thread. */
    packetMutexCreated = IotMutex_Create( &_lastPacketMutex, false );

    if( packetMutexCreated == false )
    {
        status = false;
        goto cleanup;
    }

    /* Create the receive thread timer. */
    timerCreated = IotClock_TimerCreate( &_receiveTimer,
                                         _receiveThread,
                                         NULL );

    if( timerCreated == false )
    {
        status = false;
        goto cleanup;
    }

    /* Initialize the MQTT connection object. */
    _pMqttConnection = IotTestMqtt_createMqttConnection( false,
                                                         &networkInfo,
                                                         0 );

    if( _pMqttConnection == NULL )
    {
        status = false;
        goto cleanup;
    }

    /* Setting the MQTT Context for the MQTT Connection. */
    TEST_ASSERT_EQUAL( IOT_MQTT_SUCCESS, _setContext( _pMqttConnection ) );

cleanup:

    /* Clean up on error. */
    if( status == false )
    {
        if( packetMutexCreated == true )
        {
            IotMutex_Destroy( &_lastPacketMutex );
        }

        if( timerCreated == true )
        {
            IotClock_TimerDestroy( &_receiveTimer );
        }
    }
    else
    {
        /* Set the output parameter. */
        *pMqttConnection = _pMqttConnection;
    }

    return status;
}

/*-----------------------------------------------------------*/

void IotTest_MqttMockCleanup( void )
{
    /* Clean up the MQTT connection object. */
    IotMqtt_Disconnect( _pMqttConnection, IOT_MQTT_FLAG_CLEANUP_ONLY );

    /* Destroy the receive thread timer. */
    IotClock_TimerDestroy( &_receiveTimer );

    /* Wait a short time for the timer thread to finish. */
    IotClock_SleepMs( NETWORK_ROUND_TRIP_TIME_MS * 2 );

    /* Clear the last packet type and identifier. */
    IotMutex_Lock( &_lastPacketMutex );

    _lastPacketType = 0;
    _lastPacketIdentifier = 0;

    IotMutex_Unlock( &_lastPacketMutex );

    /* Destroy the last packet mutex. */
    IotMutex_Destroy( &_lastPacketMutex );
}

/*-----------------------------------------------------------*/
