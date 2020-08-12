/*
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
 */
#include <string.h>
#include <assert.h>

#include "FreeRTOS.h"
#include "stream_buffer.h"
#include "iot_ble.h"
#include "iot_ble_mqtt_transport.h"
#include "iot_ble_mqtt_serialize.h"
#include "mqtt_lightweight.h"

/*-----------------------------------------------------------*/

/* Size of CONNACK, PUBACK. */
#define SIZE_OF_SIMPLE_ACK    4U

/* Size of DISCONNECT, PINGRESP and PINGREQ. */
#define SIZE_OF_PING          2U

/* Size of SUBACK, UNSUBACK.
 * As of now, the only one ACK is received for any request
 * and the size is always 5.
 */
#define SIZE_OF_SUB_ACK       5U


/* Define masks for each flag in Connect packets */
#define CLEAN_SESSION_MASK    0x02U

#define WILL_FLAG_MASK        0X04U

#define WILL_QOS_MASK         0x18U

#define WILL_RETAIN_MASK      0x20U

#define PASSWORD_MASK         0x40U

#define USERNAME_MASK         0x80U


/*-----------------------------------------------------------*/

/**
 * @brief Returns the integer represented by two length bytes.
 *
 * @param[in] buf A pointer to the MSB of the integer.
 */
static uint16_t getIntFromTwoBytes( const uint8_t * buf );


/**
 * @brief Converts the given integer to MQTTQoS_t
 *
 * @param[in] incomingQos, the integer to convert
 */
static MQTTQoS_t convertIntToQos( const uint8_t incomingQos );

/**
 * @brief Sets flags in connectConfig according to the packet in buf.
 *
 * @param[in] connectConfig, expected to be empty.
 * @param[in] buf, which points to the MQTT packet.
 * @return MQTTSuccess or MQTTBadParameter
 */
static MQTTStatus_t parseConnect( MQTTConnectInfo_t * connectConfig,
                                  const uint8_t * buf );

/**
 * @brief Calculates number of subscriptions requested in a subscribe packet.
 *
 * @param[in] buf, whic points to the MQTT packet.
 * @param[in] remainingLength which is the size of the rest of the packet.
 * @param[in] subscribe, true iff you are parsing a subscribe packet, false if unsubscribe.
 * @return the number of subscriptions.
 */
static uint16_t calculateNumSubs( const uint8_t * buf,
                                  uint8_t remainingLength,
                                  bool subscribe );

/**
 * @brief Sets flags in global var subscriptions according to the packet in buf.
 *
 * @param[in] subscription count, a pointer to a size_t with num subscriptions.
 * @param[in] buf, which points to the MQTT packet.
 * @param[out] pIdentifier, which points to packet identifier.
 * @param[in] subscribe, true iff you are parsing a subscribe packet, false if unsubscribe.
 * @return MQTTSuccess or MQTTBadParameter
 */
static MQTTStatus_t parseSubscribe( size_t * subscriptionCount,
                                    const uint8_t * buf,
                                    uint16_t * pIdentifier,
                                    bool subscribe );

/**
 * @brief Serializes an ACK packet in MQTT format.
 *
 * @param[in] pBuffer, which is the buffer to write the packet to.
 * @param[in] packetType which corresponds to which ack should be written.
 * @param[in] packetId, the packet Identifier (unused with CONNACK packets).
 * @return returns MQTTSuccess, MQTTBadParameter, or MQTTNoMemory.
 */
static MQTTStatus_t transportSerializeSuback( MQTTFixedBuffer_t * pBuffer,
                                              uint8_t packetType,
                                              uint16_t packetId );


/**
 * @brief Serializes a SUBACK packet in MQTT format.
 *
 * @param[in] pBuffer, which is the buffer to write the packet to.
 * @param[in] packetType which corresponds to which ack should be written.
 * @param[in] packetId, the packet Identifier.
 * @return returns MQTTSuccess, MQTTBadParameter, or MQTTNoMemory.
 */
static MQTTStatus_t transportSerializeAck( MQTTFixedBuffer_t * pBuffer,
                                           uint8_t packetType,
                                           uint16_t packetId );


/**
 * @brief Serializes a Pingresp packet in MQTT Format.
 *
 * @param[in] pBuffer, which is the buffer to write the packet to.
 * @return returns MQTTSuccess, MQTTBadParameter, or MQTTNoMemory.
 */
static MQTTStatus_t transportSerializePingresp( MQTTFixedBuffer_t * pBuffer );


/**
 * @brief Deserializes a Publish packet in MQTT Format
 * @param[in]
 *
 * statis MQTTStatus_t */
/*-----------------------------------------------------------*/

/**
 * @brief Holds the list of topic filters to put into the subscribe packet.
 */
static MQTTSubscribeInfo_t _subscriptions[ MQTT_MAX_SUBS_PER_PACKET ];

/**
 * @brief Data structure to hold received data before user makes a request for it.
 */
static StreamBufferHandle_t streamBuffer;
StaticStreamBuffer_t xStreamBufferStruct;

/*-----------------------------------------------------------*/


bool IotBleMqttTransportInit( const NetworkContext_t * pContext )
{
    bool status = true;

    streamBuffer = xStreamBufferCreateStatic( pContext->bufSize, 1, pContext->buf, &xStreamBufferStruct );

    if( streamBuffer == NULL )
    {
        status = false;
    }

    return status;
}


void IotBleMqttTransportCleanup( void )
{
    vStreamBufferDelete( streamBuffer );
}


static uint16_t getIntFromTwoBytes( const uint8_t * buf )
{
    uint16_t result = 0;

    result = buf[ 0 ];
    result <<= 8;
    result += buf[ 1 ];
    return result;
}


static MQTTQoS_t convertIntToQos( const uint8_t incomingQos )
{
    MQTTQoS_t qosValue = MQTTQoS0;

    switch( incomingQos )
    {
        case 0U:
            qosValue = MQTTQoS0;
            break;

        case 1U:
            qosValue = MQTTQoS1;
            break;

        default:
            LogError( ( "QoS 2 is not supported by MQTT over BLE. Defaulting to Qos 1." ) );
            qosValue = MQTTQoS1;
            break;
    }

    return qosValue;
}


static MQTTStatus_t parseConnect( MQTTConnectInfo_t * connectConfig,
                                  const uint8_t * buf )
{
    bool willFlag = false;
    bool willRetain = false;
    bool passwordFlag = false;
    bool usernameFlag = false;
    MQTTStatus_t status = MQTTSuccess;
    MQTTQoS_t willQos = 0;
    uint8_t connectionFlags = 0;
    size_t bufferIndex = 0;


    if( ( buf[ 0 ] & 0xf0U ) != MQTT_PACKET_TYPE_CONNECT )
    {
        status = MQTTBadParameter;
    }

    if( status == MQTTSuccess )
    {
        /* Skips the remaining length, which is not needed in connect packet.  Finds first 'M' (77) in 'MQTT'. */
        while( buf[ bufferIndex ] != 77U )
        {
            bufferIndex++;
        }

        /* Skips 'MQTT'. */
        bufferIndex += 4U;

        /* The service level of the packet. Must be 4. */
        if( ( buf[ bufferIndex ] != 4U ) )
        {
            LogError( ( "The service level of a connect packet must be 4, see [MQTT-3.1.2-2]." ) );
            status = MQTTBadParameter;
        }

        /* Increment bufferIndex to be the start of the payload. */
        bufferIndex++;
        /* Second byte after payload is connection flags. */
        connectionFlags = buf[ bufferIndex ];

        /* The LSB is reserved and must be 0. */
        if( ( connectionFlags & 0x01U ) != 0U )
        {
            LogError( ( "LSB of Connect Flags byte must be 0, see [MQTT-3.1.2-3]." ) );
            status = MQTTBadParameter;
        }
    }

    if( status == MQTTSuccess )
    {
        /* Get flag data from the flag byte. */
        connectConfig->cleanSession = ( ( ( connectionFlags & CLEAN_SESSION_MASK ) >> 1 ) == 1U );
        willFlag = ( ( ( connectionFlags & WILL_FLAG_MASK ) >> 2 ) == 1U );
        willQos = convertIntToQos( ( connectionFlags & WILL_QOS_MASK ) >> 3 );
        willRetain = ( ( ( connectionFlags & WILL_RETAIN_MASK ) >> 5 ) == 1U );
        passwordFlag = ( ( ( connectionFlags & PASSWORD_MASK ) >> 6 ) == 1U );
        usernameFlag = ( ( ( connectionFlags & USERNAME_MASK ) >> 7 ) == 1U );

        /* The will retain flag is currently unused */
        ( void ) willRetain;
    }

    if( ( status == MQTTSuccess ) && ( willQos > MQTTQoS1 ) )
    {
        LogError( ( "Qos 2 or higher is not currently supported by this library." ) );
        status = MQTTBadParameter;
    }

    if( status == MQTTSuccess )
    {
        /* Third and fourth bytes are keep alive time */
        connectConfig->keepAliveSeconds = getIntFromTwoBytes( &buf[ bufferIndex + 1U ] );

        /* Start of the payload */
        bufferIndex += 3U;

        /* Client identifier is required, 2 length bytes and then identifier */
        connectConfig->clientIdentifierLength = getIntFromTwoBytes( &buf[ bufferIndex ] );
        connectConfig->pClientIdentifier = ( const char * ) &buf[ bufferIndex + 2U ];
    }

    if( ( status == MQTTSuccess ) && ( connectConfig->clientIdentifierLength == 0U ) )
    {
        LogError( ( "A client identifier must be present in a connect packet [MQTT-3.1.3-3]." ) );
        status = MQTTBadParameter;
    }

    /* Set to start of rest of payload. */
    bufferIndex = bufferIndex + 2U + connectConfig->clientIdentifierLength;

    if( ( status == MQTTSuccess ) && ( willFlag == true ) )
    {
        /* Populate Last Will header information. */
        MQTTPublishInfo_t willInfo;
        willInfo.qos = willQos;
        willInfo.retain = willRetain;
        willInfo.topicNameLength = getIntFromTwoBytes( &buf[ bufferIndex ] );
        willInfo.pTopicName = ( const char * ) &buf[ bufferIndex + 2U ];

        if( willInfo.topicNameLength == 0U )
        {
            LogError( ( "The will flag was set but no will topic was given." ) );
            status = MQTTBadParameter;
        }

        if( status == MQTTSuccess )
        {
            bufferIndex = bufferIndex + 2U + willInfo.topicNameLength;

            /* Populate Last Will payload information. */
            willInfo.payloadLength = getIntFromTwoBytes( &buf[ bufferIndex ] );
            willInfo.pPayload = ( const char * ) &buf[ bufferIndex + 2U ];

            bufferIndex = bufferIndex + 2U + willInfo.payloadLength;
        }

        /* As of now, last will is not used */
        ( void ) willInfo;
    }

    if( ( status == MQTTSuccess ) && ( usernameFlag == true ) )
    {
        /* Populate Username header information. */
        connectConfig->userNameLength = getIntFromTwoBytes( &buf[ bufferIndex ] );
        connectConfig->pUserName = ( const char * ) &buf[ bufferIndex + 2U ];

        if( connectConfig->userNameLength == 0U )
        {
            LogError( ( "The username flag was set but no username was given." ) );
            status = MQTTBadParameter;
        }

        bufferIndex = bufferIndex + 2U + connectConfig->userNameLength;
    }

    if( ( status == MQTTSuccess ) && ( passwordFlag == true ) )
    {
        /* Populate Password header information. */
        connectConfig->passwordLength = getIntFromTwoBytes( &buf[ bufferIndex ] );
        connectConfig->pPassword = ( const char * ) &buf[ bufferIndex + 2U ];

        if( connectConfig->passwordLength == 0U )
        {
            LogError( ( "The password flag was set but no password was given." ) );
            status = MQTTBadParameter;
        }

        bufferIndex = bufferIndex + 2U + connectConfig->passwordLength;
    }

    return status;
}


static uint16_t calculateNumSubs( const uint8_t * buf,
                                  uint8_t remainingLength,
                                  bool subscribe )
{
    uint16_t numSubs = 0U, totalSize = 0U;

    /* Loop through the rest of the packet. */
    while( ( remainingLength - totalSize ) > 0U )
    {
        totalSize += ( getIntFromTwoBytes( &buf[ totalSize ] ) + 2U );

        if( subscribe )
        {
            /* Increment by 1 for Qos Byte. If it is unsubscribe packet, skip this. */
            totalSize++;
        }

        numSubs++;
    }

    return numSubs;
}

static MQTTStatus_t parseSubscribe( size_t * subscriptionCount,
                                    const uint8_t * buf,
                                    uint16_t * pIdentifier,
                                    bool subscribe )
{
    MQTTStatus_t status = MQTTSuccess;
    uint8_t remainingLength = 0;
    uint16_t bufferIndex = 0;
    size_t subscriptionIndex = 0;

    *pIdentifier = getIntFromTwoBytes( &buf[ 2 ] );

    /* Stores the remaining length after the identifier */
    remainingLength = buf[ 1 ] - 2U;

    *subscriptionCount = calculateNumSubs( &buf[ 4 ], remainingLength, subscribe );

    bufferIndex = 4U;

    for( subscriptionIndex = 0; subscriptionIndex < *subscriptionCount; ++subscriptionIndex )
    {
        /* Populate the name of the topic to (un)subscribe to. */
        _subscriptions[ subscriptionIndex ].topicFilterLength = getIntFromTwoBytes( &buf[ bufferIndex ] );
        _subscriptions[ subscriptionIndex ].pTopicFilter = ( const char * ) &buf[ bufferIndex + 2U ];
        bufferIndex += _subscriptions[ subscriptionIndex ].topicFilterLength + 2U;

        /* For subscribe packets only, increment past qos byte. */
        if( subscribe )
        {
            _subscriptions[ subscriptionIndex ].qos = convertIntToQos( buf[ bufferIndex ] & 0x03U );
            bufferIndex++;

            if( _subscriptions[ subscriptionIndex ].qos > MQTTQoS1 )
            {
                LogError( ( "Only Qos 0 and 1 are supported over BLE." ) );
                status = MQTTBadParameter;
                break;
            }
        }
    }

    return status;
}

static MQTTStatus_t transportSerializePublish( MQTTFixedBuffer_t * pBuffer,
                                               MQTTPublishInfo_t * pPublishConfig,
                                               uint16_t packetId )
{
    const char * restOfTopicName = &pPublishConfig->pTopicName[ 1 ];
    const uint16_t restOfTopicNameLength = pPublishConfig->topicNameLength - 1U;
    const char * pPayload = pPublishConfig->pPayload;
    const uint16_t payloadLength = ( uint16_t ) pPublishConfig->payloadLength;
    size_t remainingLength = 0U;
    size_t packetSize = 0U;
    size_t headerSize = 0U;
    uint8_t headerBuffer[ 10 ];
    uint8_t serializedArray[ 2 ];
    MQTTStatus_t status = MQTTSuccess;

    /* Header will only be type (1) + remaining length ( <= 4) + topic length (2) + 1 char of topic */
    pBuffer->pBuffer = headerBuffer;

    /* Change size to prevent memory errors, the real allocation is in stream buffer. */
    pBuffer->size = 0xFFU;

    pPublishConfig->topicNameLength = 1U;
    pPublishConfig->payloadLength = 0;

    status = MQTT_GetPublishPacketSize( pPublishConfig, &remainingLength, &packetSize );

    /* Topic name length + topic name + 1 for its first character + payload size */
    remainingLength = 2U + restOfTopicNameLength + 1U + payloadLength;

    /* Packet Identifier only exists in Qos 1 and Qos 2 */
    if( pPublishConfig->qos >= MQTTQoS1 )
    {
        remainingLength += 2;
    }

    /* Generate the header with one character of topic to save stack allocation */
    status = MQTT_SerializePublishHeader( pPublishConfig, packetId, remainingLength, pBuffer, &headerSize );

    if( status == MQTTSuccess )
    {
        /* Change the length of the topic name in the packet to its real length */
        pBuffer->pBuffer[ packetSize - 2U ] = ( ( restOfTopicNameLength + 1U ) );

        /* Send the Fixed header + length + first character of topic into the buffer */
        ( void ) xStreamBufferSend( streamBuffer, ( void * ) pBuffer->pBuffer, packetSize, pdMS_TO_TICKS( RECV_TIMEOUT_MS ) );

        /* Send the rest of the topic into the buffer */
        ( void ) xStreamBufferSend( streamBuffer, ( void * ) restOfTopicName, restOfTopicNameLength, pdMS_TO_TICKS( RECV_TIMEOUT_MS ) );

        if( pPublishConfig->qos >= MQTTQoS1 )
        {
            /* Send the packet identifier to the buffer; need to represent as two seperate bytes to resolve endianness. */
            serializedArray[ 0 ] = ( uint8_t ) ( ( packetId & 0xFF00U ) >> 8 );
            serializedArray[ 1 ] = ( uint8_t ) ( packetId & 0x00FFU );
            ( void ) xStreamBufferSend( streamBuffer, ( void * ) serializedArray, 2U, pdMS_TO_TICKS( RECV_TIMEOUT_MS ) );
        }

        /* Send the payload itself into the buffer */
        ( void ) xStreamBufferSend( streamBuffer, ( void * ) pPayload, payloadLength, pdMS_TO_TICKS( RECV_TIMEOUT_MS ) );
    }

    return status;
}

static MQTTStatus_t transportSerializePingresp( MQTTFixedBuffer_t * pBuffer )
{
    MQTTStatus_t status = MQTTSuccess;

    if( pBuffer == NULL )
    {
        LogError( ( "Provided buffer is NULL." ) );
        status = MQTTBadParameter;
    }
    /* The buffer must be able to fit 2 bytes for the packet. */
    else if( pBuffer->size < SIZE_OF_PING )
    {
        LogError( ( "Insufficient memory for packet." ) );
        status = MQTTNoMemory;
    }
    else
    {
        pBuffer->pBuffer[ 0 ] = MQTT_PACKET_TYPE_PINGRESP;
        pBuffer->pBuffer[ 1 ] = 0;
    }

    return status;
}

static MQTTStatus_t transportSerializeAck( MQTTFixedBuffer_t * pBuffer,
                                           uint8_t packetType,
                                           uint16_t packetId )
{
    MQTTStatus_t status = MQTTSuccess;

    if( pBuffer == NULL )
    {
        LogError( ( "Provided buffer is NULL." ) );
        status = MQTTBadParameter;
    }
    /* The buffer must be able to fit 4 bytes for the packet. */
    else if( pBuffer->size < SIZE_OF_SUB_ACK )
    {
        LogError( ( "Insufficient memory for packet." ) );
        status = MQTTNoMemory;
    }
    else if( ( packetId == 0U ) && ( packetType == MQTT_PACKET_TYPE_PUBACK ) )
    {
        LogError( ( "Packet ID cannot be 0." ) );
        status = MQTTBadParameter;
    }
    else
    {
        pBuffer->pBuffer[ 0 ] = packetType;
        pBuffer->pBuffer[ 1 ] = 2;
        pBuffer->pBuffer[ 2 ] = ( uint8_t ) ( packetId >> 8 );
        pBuffer->pBuffer[ 3 ] = ( uint8_t ) ( packetId & 0x00ffU );
    }

    return status;
}


static MQTTStatus_t transportSerializeSuback( MQTTFixedBuffer_t * pBuffer,
                                              uint8_t packetType,
                                              uint16_t packetId )
{
    MQTTStatus_t status = MQTTSuccess;

    if( pBuffer == NULL )
    {
        LogError( ( "Provided buffer is NULL." ) );
        status = MQTTBadParameter;
    }
    /* The buffer must be able to fit 4 bytes for the packet. */
    else if( pBuffer->size < SIZE_OF_SUB_ACK )
    {
        LogError( ( "Insufficient memory for packet." ) );
        status = MQTTNoMemory;
    }
    else if( packetId == 0U )
    {
        LogError( ( "Packet ID cannot be 0." ) );
        status = MQTTBadParameter;
    }
    else
    {
        pBuffer->pBuffer[ 0 ] = packetType;
        pBuffer->pBuffer[ 1 ] = 3;
        pBuffer->pBuffer[ 2 ] = ( uint8_t ) ( packetId >> 8 );
        pBuffer->pBuffer[ 3 ] = ( uint8_t ) ( packetId & 0x00ffU );
        pBuffer->pBuffer[ 4 ] = 1;
    }

    return status;
}

/*-----------------------------------------------------------*/

static MQTTStatus_t handleOutgoingConnect( const void * buf,
                                           MQTTFixedBuffer_t * bufToSend,
                                           size_t * pSize )
{
    MQTTStatus_t status = MQTTSuccess;
    MQTTConnectInfo_t connectConfig;

    memset( &connectConfig, 0x00, sizeof( MQTTConnectInfo_t ) );

    status = parseConnect( &connectConfig, buf );

    if( status == MQTTSuccess )
    {
        status = IotBleMqtt_SerializeConnect( &connectConfig, &bufToSend->pBuffer, pSize );
    }

    return status;
}

static MQTTStatus_t handleOutgoingPublish( void * buf,
                                           MQTTFixedBuffer_t * bufToSend,
                                           size_t * pSize,
                                           size_t * bytesToSend )
{
    MQTTStatus_t status = MQTTSuccess;
    uint16_t packetIdentifier = 0;
    MQTTPublishInfo_t publishConfig;
    MQTTPacketInfo_t publishPacket;

    memset( &publishConfig, 0x00, sizeof( MQTTPublishInfo_t ) );

    publishPacket.pRemainingData = &( ( uint8_t * ) buf )[ 2 ];
    publishPacket.type = MQTT_PACKET_TYPE_PUBLISH;
    publishPacket.remainingLength = *bytesToSend - 2U;

    status = MQTT_DeserializePublish( &publishPacket, &packetIdentifier, &publishConfig );

    if( status == MQTTSuccess )
    {
        status = IotBleMqtt_SerializePublish( &publishConfig, &bufToSend->pBuffer, pSize, &packetIdentifier );
    }

    return status;
}


static MQTTStatus_t handleOutgoingPuback( const void * buf,
                                          MQTTFixedBuffer_t * bufToSend,
                                          size_t * pSize )
{
    MQTTStatus_t status = MQTTSuccess;
    uint16_t packetIdentifier = 0;
    MQTTPacketInfo_t pubackPacket;

    pubackPacket.pRemainingData = ( uint8_t * ) buf;
    pubackPacket.type = MQTT_PACKET_TYPE_PUBACK;

    status = MQTT_DeserializeAck( &pubackPacket, &packetIdentifier, NULL );

    if( status == MQTTSuccess )
    {
        status = IotBleMqtt_SerializePuback( packetIdentifier, &bufToSend->pBuffer, pSize );
    }

    return status;
}


static MQTTStatus_t handleOutgoingSubscribe( const void * buf,
                                             MQTTFixedBuffer_t * bufToSend,
                                             size_t * pSize )
{
    MQTTStatus_t status = MQTTSuccess;
    uint16_t packetIdentifier = 0;
    size_t subscriptionCount = 0;

    status = parseSubscribe( &subscriptionCount, buf, &packetIdentifier, true );

    if( status == MQTTSuccess )
    {
        status = IotBleMqtt_SerializeSubscribe( _subscriptions, subscriptionCount, &bufToSend->pBuffer, pSize, &packetIdentifier );
    }

    return status;
}


static MQTTStatus_t handleOutgoingUnsubscribe( const void * buf,
                                               MQTTFixedBuffer_t * bufToSend,
                                               size_t * pSize )
{
    MQTTStatus_t status = MQTTSuccess;
    uint16_t packetIdentifier = 0;
    size_t subscriptionCount = 0;

    status = parseSubscribe( &subscriptionCount, buf, &packetIdentifier, false );

    if( status == MQTTSuccess )
    {
        status = IotBleMqtt_SerializeUnsubscribe( _subscriptions, subscriptionCount, &bufToSend->pBuffer, pSize, &packetIdentifier );
    }

    return status;
}


static MQTTStatus_t handleOutgoingPingReq( MQTTFixedBuffer_t * bufToSend,
                                           size_t * pSize )
{
    MQTTStatus_t status = MQTTSuccess;

    *pSize = SIZE_OF_PING;
    status = IotBleMqtt_SerializePingreq( &bufToSend->pBuffer, pSize );
    return status;
}

static MQTTStatus_t handleOutgoingDisconnect( MQTTFixedBuffer_t * bufToSend,
                                              size_t * pSize )
{
    MQTTStatus_t status = MQTTSuccess;

    *pSize = SIZE_OF_PING;
    /* Disconnect packets are always 2 bytes */
    status = IotBleMqtt_SerializeDisconnect( &bufToSend->pBuffer, pSize );
    return status;
}

/*-----------------------------------------------------------*/

static MQTTStatus_t handleIncomingConnack( MQTTPacketInfo_t * packet,
                                           MQTTFixedBuffer_t * pBuffer )
{
    MQTTStatus_t status = MQTTSuccess;

    LogDebug( ( "Processing incoming CONNACK from channel." ) );

    status = IotBleMqtt_DeserializeConnack( packet );

    if( status == MQTTSuccess )
    {
        /* Packet ID is not used in connack. */
        status = transportSerializeAck( pBuffer, packet->type, 0 );
    }

    if( status == MQTTSuccess )
    {
        ( void ) xStreamBufferSend( streamBuffer, pBuffer->pBuffer, SIZE_OF_SIMPLE_ACK, pdMS_TO_TICKS( RECV_TIMEOUT_MS ) );
    }

    return status;
}

static MQTTStatus_t handleIncomingPuback( MQTTPacketInfo_t * packet,
                                          MQTTFixedBuffer_t * pBuffer )
{
    MQTTStatus_t status = MQTTSuccess;
    uint16_t packetIdentifier = 0;

    LogDebug( ( "Processing incoming PUBACK from channel." ) );
    status = IotBleMqtt_DeserializePuback( packet, &packetIdentifier );

    if( status == MQTTSuccess )
    {
        status = transportSerializeAck( pBuffer, packet->type, packetIdentifier );
    }

    if( status == MQTTSuccess )
    {
        ( void ) xStreamBufferSend( streamBuffer, pBuffer->pBuffer, SIZE_OF_SIMPLE_ACK, pdMS_TO_TICKS( RECV_TIMEOUT_MS ) );
    }

    return status;
}

static MQTTStatus_t handleIncomingPublish( MQTTPacketInfo_t * packet,
                                           MQTTFixedBuffer_t * pBuffer )
{
    MQTTStatus_t status = MQTTSuccess;
    MQTTPublishInfo_t publishInfo;
    uint16_t packetIdentifier = 0;

    LogDebug( ( "Processing incoming PUBLISH from channel." ) );

    status = IotBleMqtt_DeserializePublish( packet, &publishInfo, &packetIdentifier );

    if( status == MQTTSuccess )
    {
        status = transportSerializePublish( pBuffer, &publishInfo, packetIdentifier );
    }

    return status;
}


static MQTTStatus_t handleIncomingSuback( MQTTPacketInfo_t * packet,
                                          MQTTFixedBuffer_t * pBuffer )
{
    MQTTStatus_t status = MQTTSuccess;
    uint16_t packetIdentifier = 0;

    LogDebug( ( "Processing incoming SUBACK from channel." ) );
    status = IotBleMqtt_DeserializeSuback( packet, &packetIdentifier );

    if( status == MQTTSuccess )
    {
        status = transportSerializeSuback( pBuffer, packet->type, packetIdentifier );
    }

    if( status == MQTTSuccess )
    {
        ( void ) xStreamBufferSend( streamBuffer, pBuffer->pBuffer, SIZE_OF_SUB_ACK, pdMS_TO_TICKS( RECV_TIMEOUT_MS ) );
    }

    return status;
}

static MQTTStatus_t handleIncomingUnsuback( MQTTPacketInfo_t * packet,
                                            MQTTFixedBuffer_t * pBuffer )
{
    MQTTStatus_t status = MQTTSuccess;
    uint16_t packetIdentifier = 0;

    printf( "here here\r\n" );
    LogDebug( ( "Processing incoming UNSUBACK from channel." ) );
    status = IotBleMqtt_DeserializeUnsuback( packet, &packetIdentifier );

    if( status == MQTTSuccess )
    {
        status = transportSerializeAck( pBuffer, packet->type, packetIdentifier );
    }

    if( status == MQTTSuccess )
    {
        ( void ) xStreamBufferSend( streamBuffer, pBuffer->pBuffer, SIZE_OF_SIMPLE_ACK, pdMS_TO_TICKS( RECV_TIMEOUT_MS ) );
    }

    return status;
}


static MQTTStatus_t handleIncomingPingresp( MQTTPacketInfo_t * packet,
                                            MQTTFixedBuffer_t * pBuffer )
{
    MQTTStatus_t status = MQTTSuccess;

    LogDebug( ( "Processing incoming PINGRESP from channel." ) );
    status = IotBleMqtt_DeserializePingresp( packet );

    if( status == MQTTSuccess )
    {
        status = transportSerializePingresp( pBuffer );
    }

    if( status == MQTTSuccess )
    {
        ( void ) xStreamBufferSend( streamBuffer, pBuffer->pBuffer, SIZE_OF_PING, pdMS_TO_TICKS( RECV_TIMEOUT_MS ) );
    }

    return status;
}

/*-----------------------------------------------------------*/


/**
 * @brief Transport interface write prototype.
 *
 * @param[in] context An opaque used by transport interface.
 * @param[in] buf A pointer to a buffer containing data to be sent out.
 * @param[in] bytesToWrite number of bytes to write from the buffer.
 */
int32_t IotBleMqttTransportSend( NetworkContext_t * pContext,
                                 void * buf,
                                 size_t bytesToWrite )
{
    size_t CBORbytesWritten = 0;
    uint8_t packetType = *( ( uint8_t * ) buf );
    MQTTStatus_t status = MQTTSuccess;
    MQTTFixedBuffer_t bufToSend;
    size_t packetSize = 0;

    /* The send function returns the CBOR bytes written, so need to return 0 or full amount of bytes sent. */
    int32_t MQTTBytesWritten = ( int32_t ) bytesToWrite;

    switch( packetType )
    {
        case MQTT_PACKET_TYPE_CONNECT:
            status = handleOutgoingConnect( buf, &bufToSend, &packetSize );
            break;

        case MQTT_PACKET_TYPE_PUBLISH:
            status = handleOutgoingPublish( buf, &bufToSend, &packetSize, &bytesToWrite );
            break;

        case MQTT_PACKET_TYPE_PUBACK:
            status = handleOutgoingPuback( buf, &bufToSend, &packetSize );
            break;

        case MQTT_PACKET_TYPE_SUBSCRIBE:
            status = handleOutgoingSubscribe( buf, &bufToSend, &packetSize );
            break;

        case MQTT_PACKET_TYPE_UNSUBSCRIBE:
            status = handleOutgoingUnsubscribe( buf, &bufToSend, &packetSize );
            break;

        case MQTT_PACKET_TYPE_PINGREQ:
            status = handleOutgoingPingReq( &bufToSend, &packetSize );
            break;

        case MQTT_PACKET_TYPE_DISCONNECT:
            status = handleOutgoingDisconnect( &bufToSend, &packetSize );
            break;

        /* QoS 2 cases, currently not supported by BLE */
        case MQTT_PACKET_TYPE_PUBREC:
        case MQTT_PACKET_TYPE_PUBREL:
        case MQTT_PACKET_TYPE_PUBCOMP:
            status = MQTTSendFailed;
            LogError( ( "Only Qos 0 and 1 are supported over BLE." ) );
            break;

        /* Client tries to send a server to client only packet */
        default:
            status = MQTTBadParameter;
            LogError( ( "A server to client only packet was sent. Check packet type or ensure Qos < 2." ) );
            LogError( ( "Your packet type was %i", packetType ) );
            break;
    }

    if( status != MQTTSuccess )
    {
        /* The user would have already seen a log for the previous error */
        LogError( ( "No data was sent because of a previous error." ) );
        MQTTBytesWritten = 0;

        if( bufToSend.pBuffer != NULL )
        {
            IotMqtt_FreeMessage( bufToSend.pBuffer );
        }
    }
    else
    {
        CBORbytesWritten = IotBleDataTransfer_Send( pContext->pChannel, bufToSend.pBuffer, packetSize );
    }

    if( CBORbytesWritten == 0U )
    {
        LogError( ( "No data was sent because of the channel. Ensure that the channel is initialized and ready to send data." ) );
        MQTTBytesWritten = 0;
    }
    else
    {
        IotMqtt_FreeMessage( bufToSend.pBuffer );
        LogInfo( ( "Packet sent successfully over the BLE channel" ) );
    }

    return MQTTBytesWritten;
}

void IotBleMqttTransportAcceptData( const NetworkContext_t * pContext )
{
    MQTTStatus_t status = MQTTSuccess;
    MQTTPacketInfo_t packet;
    MQTTFixedBuffer_t pBuffer;

    /* Sub ack is the largest packet size expected except for publish, which has its own buffer allocation */
    uint8_t sharedBuffer[ SIZE_OF_SUB_ACK ];

    pBuffer.pBuffer = sharedBuffer;
    pBuffer.size = SIZE_OF_SUB_ACK;

    packet.type = IotBleMqtt_GetPacketType( pContext->pChannel );
    IotBleDataTransfer_PeekReceiveBuffer( pContext->pChannel, ( const uint8_t ** ) &packet.pRemainingData, &packet.remainingLength );

    LogDebug( ( "Receiving a packet from the server." ) );

    switch( packet.type )
    {
        case MQTT_PACKET_TYPE_CONNACK:
            status = handleIncomingConnack( &packet, &pBuffer );
            break;

        case MQTT_PACKET_TYPE_PUBLISH:
            status = handleIncomingPublish( &packet, &pBuffer );
            break;

        case MQTT_PACKET_TYPE_PUBACK:
            status = handleIncomingPuback( &packet, &pBuffer );
            break;

        case MQTT_PACKET_TYPE_SUBACK:
            status = handleIncomingSuback( &packet, &pBuffer );
            break;

        case MQTT_PACKET_TYPE_UNSUBACK:
            status = handleIncomingUnsuback( &packet, &pBuffer );
            break;

        case MQTT_PACKET_TYPE_PINGRESP:
            status = handleIncomingPingresp( &packet, &pBuffer );
            break;

        /* QoS 2 cases, currently not supported by BLE */
        case MQTT_PACKET_TYPE_PUBREC:
        case MQTT_PACKET_TYPE_PUBREL:
        case MQTT_PACKET_TYPE_PUBCOMP:
            status = MQTTRecvFailed;

            LogError( ( "Only Qos 0 and 1 are supported over BLE." ) );
            break;

        /* Server tries to send a client to server only packet */
        default:
            LogError( ( "Client received a client to server only packet." ) );
            status = MQTTBadParameter;
            break;
    }

    if( status != MQTTSuccess )
    {
        LogError( ( "An error occured when receiving data from the channel. No data was recorded." ) );
    }
    else
    {
        /* Flush the data from the channel */
        ( void ) IotBleDataTransfer_Receive( pContext->pChannel, NULL, packet.remainingLength );
    }
}


/**
 * @brief Transport interface read prototype.
 *
 * @param[in] context An opaque used by transport interface.
 * @param[out] buf A pointer to a buffer where incoming data will be stored.
 * @param[in] bytesToRead number of bytes to read from the transport layer.
 */
int32_t IotBleMqttTransportReceive( NetworkContext_t * pContext,
                                    void * buf,
                                    size_t bytesToRead )
{
    ( void ) pContext;
    return ( int32_t ) xStreamBufferReceive( streamBuffer, buf, bytesToRead, pdMS_TO_TICKS( RECV_TIMEOUT_MS ) );
}
