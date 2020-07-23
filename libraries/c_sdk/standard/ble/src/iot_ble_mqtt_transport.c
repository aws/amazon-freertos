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


/*-----------------------------------------------------------*/

/**
 * @brief Returns the integer represented by two length bytes.
 *
 * @param[in] buf A pointer to the MSB of the integer.
 */

static uint16_t getIntFromTwoBytes( uint8_t * buf );

/**
 * @brief Sets flags in connectConfig according to the packet in buf.
 *
 * @param[in] connectConfig, expected to be empty.
 * @param[in] buf, which points to the MQTT packet.
 * @param[in] pSize, which points to packet size.
 * @return MQTTSuccess or MQTTBadParameter
 */
static MQTTStatus_t parseConnect( MQTTConnectInfo_t * connectConfig,
                                  uint8_t * buf,
                                  size_t * pSize );

/**
 * @brief Sets flags in publishConfig according to the packet in buf.
 *
 * @param[in] publishConfig, expected to be empty.
 * @param[in] buf, which points to the MQTT packet.
 * @param[in] pSize, which points to packet size.
 * @param[in] pIdentifier, which points to packet identifier.
 * @return MQTTSuccess or MQTTBadParameter
 */
static MQTTStatus_t parsePublish( MQTTPublishInfo_t * publishConfig,
                                  uint8_t * buf,
                                  size_t * pSize,
                                  uint16_t * pIdentifier );

/**
 * @brief Calculates number of subscriptions requested in a subscribe packet.
 *
 * @param[in] buf, whic points to the MQTT packet.
 * @param[in] remainingLength which is the size of the rest of the packet.
 * @return the number of subscriptions.
 */
static uint16_t calculateNumSubs( uint8_t * buf,
                                  uint8_t remainingLength,
                                  bool subscribe );

/**
 * @brief Sets flags in global var subscriptions according to the packet in buf.
 *
 * @param[in] subscription count, a pointer to a size_t with num subscriptions.
 * @param[in] buf, which points to the MQTT packet.
 * @param[in] pSize, which points to packet size.
 * @param[in] pIdentifier, which points to packet identifier.
 * @param[in] subscribe, true iff you are parsing a subscribe packet, false if unsubscribe.
 * @return MQTTSuccess or MQTTBadParameter
 */
static MQTTStatus_t parseSubscribe( size_t * subscriptionCount,
                                    uint8_t * buf,
                                    size_t * pSize,
                                    uint16_t * pIdentifier,
                                    bool subscribe );

/**
 * @brief Gets the packet identifier from a puback packet.
 *
 * @param[in] buf, which points to the MQTT packet.
 * @param[in] pIdentifier, which points to packet identifier.
 * @return MQTTSuccess or MQTTBadParameter
 */
static MQTTStatus_t parsePubAck( uint8_t * buf,
                                 uint16_t * pIdentifier );

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

/*-----------------------------------------------------------*/

/**
 * @brief Holds the list of topic filters to put into the subscribe packet.
 */
MQTTSubscribeInfo_t subscriptions[ MQTT_MAX_SUBS_PER_PACKET ];

/**
 * @brief Data structure to hold received data before user makes a request for it.
 */
StreamBufferHandle_t streamBuffer;
StaticStreamBuffer_t xStreamBufferStruct;

/*-----------------------------------------------------------*/


void IotBleMqttTransportInit( NetworkContext_t * context )
{
    streamBuffer = xStreamBufferCreateStatic( context->bufSize, 1, context->buf, &xStreamBufferStruct );
}


static uint16_t getIntFromTwoBytes( uint8_t * buf )
{
    uint16_t result = 0;

    result = buf[ 0 ];
    result <<= 8;
    result += buf[ 1 ];
    return result;
}


static MQTTStatus_t parseConnect( MQTTConnectInfo_t * connectConfig,
                                  uint8_t * buf,
                                  size_t * pSize )
{
    bool willFlag = false;
    bool willRetain = false;
    bool passwordFlag = false;
    bool usernameFlag = false;
    uint8_t willQos = 0;
    uint8_t connectionFlags = 0;
    size_t index = 0;
    uint16_t payloadIndex = 0;


    if( ( buf[ 0 ] & 0xf0 ) != MQTT_PACKET_TYPE_CONNECT )
    {
        return MQTTBadParameter;
    }

    /* Skips the remaining length, which is not needed in connect packet.  Finds first 'M in 'MQTT'. */
    while( buf[ index ] != 'M' )
    {
        index++;
    }

    /* Skips 'MQTT'. */
    index += 4;

    /* The service level of the packet. Must be 4. */
    if( ( buf[ index ] != 4 ) )
    {
        LogError( ( "The service level of a connect packet must be 4, see [MQTT-3.1.2-2]" ) );
        return MQTTBadParameter;
    }

    index++;

    /* Second byte after payload is connection flags. */
    connectionFlags = buf[ index ];

    /* The LSB is reserved and must be 0. */
    if( ( connectionFlags & 0x01 ) != 0 )
    {
        LogError( ( "LSB of Connect Flags byte must be 0, see [MQTT-3.1.2-3]" ) );
        return MQTTBadParameter;
    }

    /* Get flag data from the flag byte. */
    connectConfig->cleanSession = connectionFlags & 0x02;
    willFlag = connectionFlags & 0x04;
    willQos = connectionFlags & 0x18;
    willRetain = connectionFlags & 0x20;
    passwordFlag = connectionFlags & 0x40;
    usernameFlag = connectionFlags & 0x80;

    if( willQos > 1 )
    {
        LogError( ( "Qos 2 or higher is not currently supported by this library" ) );
        return MQTTBadParameter;
    }

    /* Third and fourth bytes are keep alive time */
    connectConfig->keepAliveSeconds = getIntFromTwoBytes( &buf[ index + 1 ] );

    /* Index variable for each payload byte */
    payloadIndex = index + 3;

    /* Client identifier is required, 2 length bytes and then identifier */
    connectConfig->clientIdentifierLength = getIntFromTwoBytes( &buf[ payloadIndex ] );
    connectConfig->pClientIdentifier = ( char * ) &buf[ payloadIndex + 2 ];

    if( connectConfig->clientIdentifierLength == 0 )
    {
        LogError( ( "A client identifier must be present in a connect packet [MQTT-3.1.3-3]" ) );
        return MQTTBadParameter;
    }

    /* Set to start of rest of payload. */
    payloadIndex = payloadIndex + 2 + connectConfig->clientIdentifierLength;

    if( willFlag == true )
    {
        /* Populate Last Will header information. */
        MQTTPublishInfo_t willInfo;
        willInfo.qos = willQos;
        willInfo.retain = willRetain;
        willInfo.topicNameLength = getIntFromTwoBytes( &buf[ payloadIndex ] );
        willInfo.pTopicName = ( char * ) &buf[ payloadIndex + 2 ];

        if( willInfo.topicNameLength == 0 )
        {
            LogError( ( "The will flag was set but no will topic was given" ) );
            return MQTTBadParameter;
        }

        payloadIndex = payloadIndex + 2 + willInfo.topicNameLength;

        /* Populate Last Will payload information. */
        willInfo.payloadLength = getIntFromTwoBytes( &buf[ payloadIndex ] );
        willInfo.pPayload = ( char * ) &buf[ payloadIndex + 2 ];

        payloadIndex = payloadIndex + 2 + willInfo.payloadLength;
    }

    if( usernameFlag == true )
    {
        /* Populate Username header information. */
        connectConfig->userNameLength = getIntFromTwoBytes( &buf[ payloadIndex ] );
        connectConfig->pUserName = ( char * ) &buf[ payloadIndex + 2 ];

        if( connectConfig->passwordLength == 0 )
        {
            LogError( ( "The username flag was set but no username was given" ) );
            return MQTTBadParameter;
        }

        payloadIndex = payloadIndex + 2 + connectConfig->userNameLength;
    }

    if( passwordFlag == true )
    {
        /* Populate Password header information. */
        connectConfig->passwordLength = getIntFromTwoBytes( &buf[ payloadIndex ] );
        connectConfig->pPassword = ( char * ) &buf[ payloadIndex + 2 ];

        if( connectConfig->passwordLength == 0 )
        {
            LogError( ( "The password flag was set but no password was given" ) );
            return MQTTBadParameter;
        }

        payloadIndex = payloadIndex + 2 + connectConfig->passwordLength;
    }

    /* Add one since we are zero indexed. */
    *pSize = payloadIndex + 1;
    return MQTTSuccess;
}


static MQTTStatus_t parsePublish( MQTTPublishInfo_t * publishConfig,
                                  uint8_t * buf,
                                  size_t * pSize,
                                  uint16_t * pIdentifier )
{
    uint8_t remainingLength = 0;

    if( ( buf[ 0 ] & 0xf0 ) != MQTT_PACKET_TYPE_PUBLISH )
    {
        return MQTTBadParameter;
    }

    /* Get flag information from fixed header. */
    publishConfig->dup = buf[ 0 ] & 0x08;
    publishConfig->qos = buf[ 0 ] & 0x06;

    if( publishConfig->qos > 1 )
    {
        LogError( ( "Only Qos 0 and 1 are supported over BLE" ) );
        return MQTTBadParameter;
    }

    publishConfig->retain = buf[ 0 ] & 0x01;
    remainingLength = buf[ 1 ];

    /* Get information from the variable header. */
    publishConfig->topicNameLength = getIntFromTwoBytes( &buf[ 2 ] );
    publishConfig->pTopicName = ( char * ) &buf[ 4 ];
    *pIdentifier = getIntFromTwoBytes( &buf[ 4 + publishConfig->topicNameLength ] );

    /* Populate the payload information. */
    publishConfig->payloadLength = ( remainingLength - ( 2 + publishConfig->topicNameLength ) );
    publishConfig->pPayload = &buf[ 4 + publishConfig->topicNameLength ];

    /* Add 2 for the fixed header size. */
    *pSize = remainingLength + 2;

    return MQTTSuccess;
}


static uint16_t calculateNumSubs( uint8_t * buf,
                                  uint8_t remainingLength,
                                  bool subscribe )
{
    uint16_t numSubs = 0, totalSize = 0;

    /* Loop through the rest of the packet. */
    while( remainingLength - totalSize > 0U )
    {
        totalSize += ( getIntFromTwoBytes( &buf[ totalSize ] ) + 2 );

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
                                    uint8_t * buf,
                                    size_t * pSize,
                                    uint16_t * pIdentifier,
                                    bool subscribe )
{
    uint8_t remainingLength = 0;
    uint16_t bufferIndex = 0;
    size_t index = 0;

    *pIdentifier = getIntFromTwoBytes( &buf[ 2 ] );

    /* Stores the remaining length after the identifier */
    remainingLength = buf[ 1 ] - 2;

    /* Add 2 for fixed header and 2 for the identifier */
    *pSize = remainingLength + 4;

    *subscriptionCount = calculateNumSubs( &buf[ 4 ], remainingLength, subscribe );

    bufferIndex = 4;

    for( index = 0; index < *subscriptionCount; ++index )
    {
        /* Populate the name of the topic to (un)subscribe to. */
        subscriptions[ index ].topicFilterLength = getIntFromTwoBytes( &buf[ bufferIndex ] );
        subscriptions[ index ].pTopicFilter = ( const char * ) &buf[ bufferIndex + 2 ];
        bufferIndex += subscriptions[ index ].topicFilterLength + 2;

        /* For subscribe packets only, increment past qos byte. */
        if( subscribe )
        {
            subscriptions[ index ].qos = buf[ bufferIndex ];
            bufferIndex++;

            if( subscriptions[ index ].qos > 1 )
            {
                LogError( ( "Only Qos 0 and 1 are supported over BLE" ) );
                return MQTTBadParameter;
            }
        }
    }

    return MQTTSuccess;
}


static MQTTStatus_t parsePubAck( uint8_t * buf,
                                 uint16_t * pIdentifier )
{
    /* For publish ACKs, the second byte must be 2. */
    if( ( buf[ 0 ] & 0xf0 ) != MQTT_PACKET_TYPE_PUBACK )
    {
        return MQTTBadParameter;
    }

    if( ( buf[ 1 ] != 0x02 ) )
    {
        return MQTTBadParameter;
    }

    /* Returns the packet identifier */
    *pIdentifier = getIntFromTwoBytes( &buf[ 2 ] );
    return MQTTSuccess;
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


static MQTTStatus_t handleOutgoingConnect( void * buf,
                                           MQTTFixedBuffer_t * bufToSend,
                                           size_t * pSize )
{
    MQTTStatus_t status = MQTTSuccess;
    MQTTConnectInfo_t connectConfig;

    status = parseConnect( &connectConfig, buf, pSize );

    if( status == MQTTSuccess )
    {
        status = IotBleMqtt_SerializeConnect( &connectConfig, &bufToSend->pBuffer, pSize );
    }

    return status;
}

static MQTTStatus_t handleOutgoingPublish( void * buf,
                                           MQTTFixedBuffer_t * bufToSend,
                                           size_t * pSize )
{
    MQTTStatus_t status = MQTTSuccess;
    uint16_t packetIdentifier = 0;
    MQTTPublishInfo_t publishConfig;

    status = parsePublish( &publishConfig, buf, pSize, &packetIdentifier );

    if( status == MQTTSuccess )
    {
        status = IotBleMqtt_SerializePublish( &publishConfig, &bufToSend->pBuffer, pSize, &packetIdentifier, 0 );
    }

    return status;
}


static MQTTStatus_t handleOutgoingPuback( void * buf,
                                          MQTTFixedBuffer_t * bufToSend,
                                          size_t * pSize )
{
    MQTTStatus_t status = MQTTSuccess;
    uint16_t packetIdentifier = 0;

    status = parsePubAck( buf, &packetIdentifier );

    if( status == MQTTSuccess )
    {
        /* Pub Ack packet sizes are always 4 bytes (control + remaining length + 2 identifier) */
        *pSize = SIZE_OF_SIMPLE_ACK;
        status = IotBleMqtt_SerializePuback( packetIdentifier, &bufToSend->pBuffer, pSize );
    }

    return status;
}


static MQTTStatus_t handleOutgoingSubscribe( void * buf,
                                             MQTTFixedBuffer_t * bufToSend,
                                             size_t * pSize )
{
    MQTTStatus_t status = MQTTSuccess;
    uint16_t packetIdentifier = 0;
    size_t subscriptionCount = 0;

    status = parseSubscribe( &subscriptionCount, buf, pSize, &packetIdentifier, true );

    if( status == MQTTSuccess )
    {
        status = IotBleMqtt_SerializeSubscribe( subscriptions, subscriptionCount, &bufToSend->pBuffer, pSize, &packetIdentifier );
    }

    return status;
}


static MQTTStatus_t handleOutgoingUnsubscribe( void * buf,
                                               MQTTFixedBuffer_t * bufToSend,
                                               size_t * pSize )
{
    MQTTStatus_t status = MQTTSuccess;
    uint16_t packetIdentifier = 0;
    size_t subscriptionCount = 0;

    status = parseSubscribe( &subscriptionCount, buf, pSize, &packetIdentifier, false );

    if( status == MQTTSuccess )
    {
        status = IotBleMqtt_SerializeUnsubscribe( subscriptions, subscriptionCount, &bufToSend->pBuffer, pSize, &packetIdentifier );
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

static MQTTStatus_t handleIncomingConnack( MQTTPacketInfo_t * packet,
                                           MQTTFixedBuffer_t * pBuffer )
{
    MQTTStatus_t status = MQTTSuccess;

    LogDebug( ( "Processing incoming CONNACK from channel" ) );

    status = IotBleMqtt_DeserializeConnack( packet );

    if( status == MQTTSuccess )
    {
        /* Packet ID is not used in connack. */
        status = transportSerializeAck( pBuffer, packet->type, 0 );
    }

    if( status == MQTTSuccess )
    {
        xStreamBufferSend( streamBuffer, pBuffer->pBuffer, SIZE_OF_SIMPLE_ACK, pdMS_TO_TICKS( 100 ) );
    }

    return status;
}

static MQTTStatus_t handleIncomingPuback( MQTTPacketInfo_t * packet,
                                          MQTTFixedBuffer_t * pBuffer )
{
    MQTTStatus_t status = MQTTSuccess;
    uint16_t packetIdentifier = 0;

    LogDebug( ( "Processing incoming PUBACK from channel" ) );
    status = IotBleMqtt_DeserializePuback( packet, &packetIdentifier );

    if( status == MQTTSuccess )
    {
        status = transportSerializeAck( pBuffer, packet->type, packetIdentifier );
    }

    if( status == MQTTSuccess )
    {
        xStreamBufferSend( streamBuffer, pBuffer->pBuffer, SIZE_OF_SIMPLE_ACK, pdMS_TO_TICKS( 100 ) );
    }

    return status;
}

static MQTTStatus_t handleIncomingPublish( MQTTPacketInfo_t * packet,
                                           MQTTFixedBuffer_t * pBuffer )
{
    MQTTStatus_t status = MQTTSuccess;
    MQTTPublishInfo_t publishInfo;
    uint16_t packetIdentifier = 0;

    LogDebug( ( "Processing incoming PUBLISH from channel" ) );

    status = IotBleMqtt_DeserializePublish( packet, &publishInfo, &packetIdentifier );

    if( status == MQTTSuccess )
    {
        status = MQTT_SerializePublish( &publishInfo, packetIdentifier, packet->remainingLength, pBuffer );
    }

    if( status == MQTTSuccess )
    {
        xStreamBufferSend( streamBuffer, pBuffer->pBuffer, packet->remainingLength + 2, pdMS_TO_TICKS( 100 ) );
    }

    return status;
}


static MQTTStatus_t handleIncomingSuback( MQTTPacketInfo_t * packet,
                                          MQTTFixedBuffer_t * pBuffer )
{
    MQTTStatus_t status = MQTTSuccess;
    uint16_t packetIdentifier = 0;

    LogDebug( ( "Processing incoming SUBACK from channel" ) );
    status = IotBleMqtt_DeserializeSuback( packet, &packetIdentifier );

    if( status == MQTTSuccess )
    {
        status = transportSerializeSuback( pBuffer, packet->type, packetIdentifier );
    }

    if( status == MQTTSuccess )
    {
        xStreamBufferSend( streamBuffer, pBuffer->pBuffer, SIZE_OF_SUB_ACK, pdMS_TO_TICKS( 100 ) );
    }

    return status;
}

static MQTTStatus_t handleIncomingUnsuback( MQTTPacketInfo_t * packet,
                                            MQTTFixedBuffer_t * pBuffer )
{
    MQTTStatus_t status = MQTTSuccess;
    uint16_t packetIdentifier = 0;

    LogDebug( ( "Processing incoming UNSUBACK from channel" ) );
    status = IotBleMqtt_DeserializeUnsuback( packet, &packetIdentifier );

    if( status == MQTTSuccess )
    {
        status = transportSerializeAck( pBuffer, packet->type, packetIdentifier );
    }

    if( status == MQTTSuccess )
    {
        xStreamBufferSend( streamBuffer, pBuffer->pBuffer, SIZE_OF_SUB_ACK, pdMS_TO_TICKS( 100 ) );
    }

    return status;
}


static MQTTStatus_t handleIncomingPingresp( MQTTPacketInfo_t * packet,
                                            MQTTFixedBuffer_t * pBuffer )
{
    MQTTStatus_t status = MQTTSuccess;

    LogDebug( ( "Processing incoming PINGRESP from channel" ) );
    status = IotBleMqtt_DeserializePingresp( packet );

    if( status == MQTTSuccess )
    {
        status = transportSerializePingresp( pBuffer );
    }

    if( status == MQTTSuccess )
    {
        xStreamBufferSend( streamBuffer, pBuffer->pBuffer, SIZE_OF_PING, pdMS_TO_TICKS( 100 ) );
    }

    return status;
}

/**
 * @brief Transport interface write prototype.
 *
 * @param[in] context An opaque used by transport interface.
 * @param[in] buf A pointer to a buffer containing data to be sent out.
 * @param[in] bytesToWrite number of bytes to write from the buffer.
 */
int32_t IotBleMqttTransportSend( NetworkContext_t context,
                                 void * buf,
                                 const size_t bytesToWrite )
{
    int32_t bytesWritten = 0;
    uint8_t packetType = *( ( uint8_t * ) buf );
    MQTTStatus_t status = MQTTSuccess;
    MQTTFixedBuffer_t bufToSend;
    size_t pSize = 0;

    uint8_t sharedBuffer[ RECV_PACKET_BUFFER_SIZE ];

    bufToSend.pBuffer = sharedBuffer;
    bufToSend.size = RECV_PACKET_BUFFER_SIZE;

    IotBleMqtt_InitSerialize();

    switch( packetType )
    {
        case MQTT_PACKET_TYPE_CONNECT:
            status = handleOutgoingConnect( buf, &bufToSend, &pSize );
            break;

        case MQTT_PACKET_TYPE_PUBLISH:
            status = handleOutgoingPublish( buf, &bufToSend, &pSize );
            break;

        case MQTT_PACKET_TYPE_PUBACK:
            status = handleOutgoingPuback( buf, &bufToSend, &pSize );
            break;

        case MQTT_PACKET_TYPE_SUBSCRIBE:
            status = handleOutgoingSubscribe( buf, &bufToSend, &pSize );
            break;

        case MQTT_PACKET_TYPE_UNSUBSCRIBE:
            status = handleOutgoingUnsubscribe( buf, &bufToSend, &pSize );
            break;

        case MQTT_PACKET_TYPE_PINGREQ:
            status = handleOutgoingPingReq( &bufToSend, &pSize );
            break;

        case MQTT_PACKET_TYPE_DISCONNECT:
            status = handleOutgoingDisconnect( &bufToSend, &pSize );
            break;

        /* QoS 2 cases, currently not supported by BLE */
        case MQTT_PACKET_TYPE_PUBREC:
        case MQTT_PACKET_TYPE_PUBREL:
        case MQTT_PACKET_TYPE_PUBCOMP:
            status = MQTTSendFailed;
            LogError( ( "Only Qos 0 and 1 are supported over BLE" ) );
            assert( false );
            break;

        /* Client tries to send a server to client only packet */
        default:
            status = MQTTBadParameter;
            LogError( ( "A server to client only packet was sent. Check packet type or ensure Qos < 2" ) );
            LogError( ( "Your packet type was %i", packetType ) );
            assert( false );
            break;
    }

    IotBleMqtt_CleanupSerialize();

    if( status != MQTTSuccess )
    {
        /* The user would have already seen a log for the previous error */
        LogError( ( "No data was sent because of a previous error" ) );
        return 0;
    }

    bytesWritten = IotBleDataTransfer_Send( context.pChannel, bufToSend.pBuffer, pSize );

    if( bytesWritten == 0 )
    {
        LogError( ( "No data was sent because of the channel. Is the channel initialized and ready to send data?" ) );
        return 0;
    }

    return bytesToWrite;
}

void IotBleMqttTransportAcceptData( NetworkContext_t * pContext )
{
    MQTTStatus_t status = MQTTSuccess;
    MQTTPacketInfo_t packet;
    MQTTFixedBuffer_t pBuffer;

    uint8_t sharedBuffer[ RECV_PACKET_BUFFER_SIZE ];

    pBuffer.pBuffer = sharedBuffer;
    pBuffer.size = RECV_PACKET_BUFFER_SIZE;

    packet.type = IotBleMqtt_GetPacketType( pContext->pChannel );
    IotBleDataTransfer_PeekReceiveBuffer( pContext->pChannel, ( const uint8_t ** ) &packet.pRemainingData, &packet.remainingLength );

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

            LogError( ( "Only Qos 0 and 1 are supported over BLE" ) );
            assert( false );
            break;

        /* Server tries to send a client to server only packet */
        default:
            LogError( ( "Client received a client to server only packet" ) );
            status = MQTTBadParameter;
            assert( false );
            break;
    }

    if( status != MQTTSuccess )
    {
        LogError( ( "An error occured when receiving data from the channel. No data was sent." ) )
    }
    else
    {
        /* Flush the data from the channel */
        IotBleDataTransfer_Receive( pContext->pChannel, NULL, packet.remainingLength );
    }
}


/**
 * @brief Transport interface read prototype.
 *
 * @param[in] context An opaque used by transport interface.
 * @param[out] buf A pointer to a buffer where incoming data will be stored.
 * @param[in] bytesToRead number of bytes to read from the transport layer.
 */
int32_t IotBleMqttTransportReceive( NetworkContext_t context,
                                    void * buf,
                                    const size_t bytesToRead )
{
    return xStreamBufferReceive( streamBuffer, buf, bytesToRead, pdMS_TO_TICKS( 100 ) );
}
