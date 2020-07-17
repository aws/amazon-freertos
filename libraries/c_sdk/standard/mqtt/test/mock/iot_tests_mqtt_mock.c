/*
 * FreeRTOS MQTT V2.2.0
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

/* MQTT internal include. */
#include "private/iot_mqtt_internal.h"

/* MQTT mock include. */
#include "iot_tests_mqtt_mock.h"

/* MQTT test access include. */
#include "iot_test_access_mqtt.h"

/* Platform layer includes. */
#include "platform/iot_clock.h"
#include "platform/iot_threads.h"

/*-----------------------------------------------------------*/

/**
 * @brief A delay that simulates the time required for an MQTT packet to be sent
 * to the server and for the server to send a response.
 */
#define NETWORK_ROUND_TRIP_TIME_MS     ( 25 )

/**
 * @brief The maximum size of any MQTT acknowledgement packet (e.g. SUBACK,
 * PUBACK, UNSUBACK) used in these tests.
 */
#define ACKNOWLEDGEMENT_PACKET_SIZE    ( 5 )

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

    /* Read the packet type, which is the first byte in the message. */
    mqttPacket.type = *pMessage;

    /* Set the members of the receive context. */
    receiveContext.pData = pMessage + 1;
    receiveContext.dataLength = messageLength;

    /* Lock the mutex to modify the information on the last packet sent. */
    IotMutex_Lock( &_lastPacketMutex );

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
                _lastPacketType = MQTT_PACKET_TYPE_PUBLISH;
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

    /* Check if a network response is needed. */
    if( _lastPacketType != 0 )
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
            mqttPacket.pRemainingData = ( uint8_t * ) pMessage + ( messageLength - mqttPacket.remainingLength );

            status = _IotMqtt_DeserializePublish( &mqttPacket );
            _lastPacketIdentifier = mqttPacket.packetIdentifier;
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
