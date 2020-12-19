/*
 * FreeRTOS BLE V2.2.0
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

#include "FreeRTOS.h"
#include "stream_buffer.h"
#include "iot_ble.h"
#include "iot_ble_mqtt_transport.h"
#include "iot_ble_mqtt_serialize.h"

/*-----------------------------------------------------------*/

/* Size of CONNACK, PUBACK, UNSUBACK. */
#define SIZE_OF_SIMPLE_ACK             4U

/* Size of DISCONNECT, PINGRESP and PINGREQ. */
#define SIZE_OF_PING                   2U

/* Size of SUBACK.
 * As of now, the only one ACK is received for any request
 * and the size is always 5.
 */
#define SIZE_OF_SUB_ACK                5U

/*
 * Since only one ACK is received for all the subscriptions
 * the remaining length is fixed to 3 bytes.
 */
#define SUB_ACK_REMAINING_LENGTH       3U

/*
 * Simple ack such as PUBACK contains only packet identifier
 * so the remaining length is fixed to two bytes.
 */
#define SIMPLE_ACK_REMAINING_LENGTH    2U

/**
 * Maximum size of the publish header is 5 bytes, since the maximum
 * size of encoded remaining length is 4 bytes as per MQTT spec v3.1.1.
 */
#define SIZE_OF_PUBLISH_HEADER         5U

/**
 * @brief CONNACK Server Refused return code.
 */
#define CONNACK_REFUSED                ( 0x05 )

/**
 * Creates a mask for the bit at position bitpos.
 */
#define BIT_MASK( bitpos )    ( ( uint8_t ) ( 1 << bitpos ) )

/* Define masks for each flag in Connect packets */
#define CLEAN_SESSION_MASK          0x02U

#define WILL_FLAG_MASK              0X04U

#define WILL_QOS_MASK               0x18U

#define WILL_RETAIN_MASK            0x20U

#define PASSWORD_MASK               0x40U

#define USERNAME_MASK               0x80U

/*
 * @brief Defines for the position masks of PUBLISH flags.
 */
#define PUBLISH_FLAG_RETAIN_MASK    BIT_MASK( 0 )                           /**< @brief MQTT PUBLISH retain flag. */
#define PUBLISH_FLAG_QOS_MASK       ( BIT_MASK( 1 ) | BIT_MASK( 2 ) )       /**< @brief MQTT PUBLISH QoS1 flag. */
#define PUBLISH_FLAG_DUP_MASK       BIT_MASK( 2 )                           /**< @brief MQTT PUBLISH duplicate flag. */
#define PUBLISH_FLAG_QOS1_MASK      BIT_MASK( 1 )                           /**< @brief MQTT PUBLISH QoS1 flag. */
#define PUBLISH_FLAG_QOS2_MASK      BIT_MASK( 2 )                           /**< @brief MQTT PUBLISH QoS2 flag. */

/**
 * @brief The value denotes the length which is greater than maximum value
 * that can be decoded from an MQTT packet as per MQTT spec v3.1.1.
 */
#define REMAINING_LENGTH_INVALID    ( ( size_t ) 268435456 )

/**
 * @brief Macro to encode the packet type into MQTT serialized packet.
 */
#define ENCODE_PACKET_TYPE( packetType )    ( ( uint8_t ) ( packetType << 4 ) )

/**
 * @brief Macro to decode the packet type from MQTT serialized packet.
 */
#define DECODE_PACKET_TYPE( packetFlag )    ( ( uint8_t ) ( packetFlag >> 4 ) )

/**
 * @brief Macro for checking if a bit is set in a 1-byte unsigned int.
 *
 * @param[in] x The unsigned int to check.
 * @param[in] position Which bit to check.
 */
#define UINT8_CHECK_BIT( x, position )      ( ( ( x ) & ( 0x01U << ( position ) ) ) == ( 0x01U << ( position ) ) )

/* @brief Macro for decoding a 2-byte unsigned int from a sequence of bytes.
 *
 * @param[in] ptr A uint8_t* that points to the high byte.
 */
#define UINT16_DECODE( ptr )                                \
    ( uint16_t ) ( ( ( ( uint16_t ) ( *( ptr ) ) ) << 8 ) | \
                   ( ( uint16_t ) ( *( ( ptr ) + 1 ) ) ) )

/*-----------------------------------------------------------*/

/**
 * @brief Each compilation unit that consumes the NetworkContext must define it.
 * It should contain a single pointer to the type of your desired transport.
 * When using multiple transports in the same compilation unit, define this pointer as void *.
 *
 * @note Transport stacks are defined in amazon-freertos/libraries/c_sdk/standard/ble/include/iot_ble_mqtt_transport.h.
 */
struct NetworkContext
{
    BleTransportParams_t * pParams;
};

/*-----------------------------------------------------------*/

/**
 * @brief Returns the integer represented by two length bytes.
 *
 * @param[in] buf A pointer to the MSB of the integer.
 */
static uint16_t getIntFromTwoBytes( const uint8_t * buf );


/**
 * @brief Converts the given integer to MQTTBLEQoS_t
 *
 * @param[in] incomingQos, the integer to convert
 */
static MQTTBLEQoS_t convertIntToQos( const uint8_t incomingQos );

/**
 * @brief Sets flags in connectConfig according to the packet in buf.
 *
 * @param[in] connectConfig, expected to be empty.
 * @param[in] buf, which points to the MQTT packet.
 * @return MQTTBLESuccess or MQTTBLEBadParameter
 */
static MQTTBLEStatus_t parseConnect( MQTTBLEConnectInfo_t * connectConfig,
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
 * @return MQTTBLESuccess or MQTTBLEBadParameter
 */
static MQTTBLEStatus_t parseSubscribe( size_t * subscriptionCount,
                                       const uint8_t * buf,
                                       uint16_t * pIdentifier,
                                       bool subscribe );

/*-----------------------------------------------------------*/

/**
 * @brief Holds the list of topic filters to put into the subscribe packet.
 */
static MQTTBLESubscribeInfo_t _subscriptions[ MQTT_MAX_SUBS_PER_PACKET ];

/*-----------------------------------------------------------*/


bool IotBleMqttTransportInit( void * pBuffer,
                              size_t bufSize,
                              NetworkContext_t * pContext )
{
    bool status = true;
    BleTransportParams_t * pBleTransportParams = NULL;

    pBleTransportParams = pContext->pParams;
    pBleTransportParams->xStreamBuffer = xStreamBufferCreateStatic( bufSize, 1, pBuffer, &( pBleTransportParams->xStreamBufferStruct ) );

    if( pBleTransportParams->xStreamBuffer == NULL )
    {
        LogError( ( "BLE teransport layer buffer could not be created.  Check the buffer and buffer size passed to network context." ) );
        status = false;
    }

    return status;
}


void IotBleMqttTransportCleanup( const NetworkContext_t * pContext )
{
    BleTransportParams_t * pBleTransportParams = NULL;

    pBleTransportParams = pContext->pParams;
    vStreamBufferDelete( pBleTransportParams->xStreamBuffer );
}


static uint16_t getIntFromTwoBytes( const uint8_t * buf )
{
    uint16_t result = 0;

    result = buf[ 0 ];
    result <<= 8;
    result += buf[ 1 ];
    return result;
}


static MQTTBLEQoS_t convertIntToQos( const uint8_t incomingQos )
{
    MQTTBLEQoS_t qosValue = MQTTBLEQoS0;

    switch( incomingQos )
    {
        case 0U:
            qosValue = MQTTBLEQoS0;
            break;

        case 1U:
            qosValue = MQTTBLEQoS1;
            break;

        default:
            LogError( ( "QoS 2 is not supported by MQTT over BLE. Defaulting to Qos 1." ) );
            qosValue = MQTTBLEQoS1;
            break;
    }

    return qosValue;
}


static MQTTBLEStatus_t parseConnect( MQTTBLEConnectInfo_t * connectConfig,
                                     const uint8_t * buf )
{
    bool willFlag = false;
    bool willRetain = false;
    bool passwordFlag = false;
    bool usernameFlag = false;
    MQTTBLEStatus_t status = MQTTBLESuccess;
    MQTTBLEQoS_t willQos = 0;
    uint8_t connectionFlags = 0;
    size_t bufferIndex = 0;

    if( status == MQTTBLESuccess )
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
            status = MQTTBLEBadParameter;
        }

        /* Increment bufferIndex to be the start of the payload. */
        bufferIndex++;
        /* Second byte after payload is connection flags. */
        connectionFlags = buf[ bufferIndex ];

        /* The LSB is reserved and must be 0. */
        if( ( connectionFlags & 0x01U ) != 0U )
        {
            LogError( ( "LSB of Connect Flags byte must be 0, see [MQTT-3.1.2-3]." ) );
            status = MQTTBLEBadParameter;
        }
    }

    if( status == MQTTBLESuccess )
    {
        /* Get flag data from the flag byte. */
        connectConfig->cleanSession = ( ( connectionFlags & CLEAN_SESSION_MASK ) == CLEAN_SESSION_MASK );
        willFlag = ( ( connectionFlags & WILL_FLAG_MASK ) == WILL_FLAG_MASK );
        willQos = convertIntToQos( ( connectionFlags & WILL_QOS_MASK ) >> 3 );
        willRetain = ( ( connectionFlags & WILL_RETAIN_MASK ) == WILL_RETAIN_MASK );
        passwordFlag = ( ( connectionFlags & PASSWORD_MASK ) == PASSWORD_MASK );
        usernameFlag = ( ( connectionFlags & USERNAME_MASK ) == USERNAME_MASK );

        /* The will retain flag is currently unused */
        ( void ) willRetain;
    }

    if( status == MQTTBLESuccess )
    {
        /* Third and fourth bytes are keep alive time */
        connectConfig->keepAliveSeconds = getIntFromTwoBytes( &buf[ bufferIndex + 1U ] );

        /* Start of the payload */
        bufferIndex += 3U;

        /* Client identifier is required, 2 length bytes and then identifier */
        connectConfig->clientIdentifierLength = getIntFromTwoBytes( &buf[ bufferIndex ] );
        connectConfig->pClientIdentifier = ( const char * ) &buf[ bufferIndex + 2U ];
    }

    if( ( status == MQTTBLESuccess ) && ( connectConfig->clientIdentifierLength == 0U ) )
    {
        LogError( ( "A client identifier must be present in a connect packet [MQTT-3.1.3-3]." ) );
        status = MQTTBLEBadParameter;
    }

    /* Set to start of rest of payload. */
    bufferIndex = bufferIndex + 2U + connectConfig->clientIdentifierLength;

    if( ( status == MQTTBLESuccess ) && ( willFlag == true ) )
    {
        /* Populate Last Will header information. */
        MQTTBLEPublishInfo_t willInfo;
        willInfo.qos = willQos;
        willInfo.retain = willRetain;
        willInfo.topicNameLength = getIntFromTwoBytes( &buf[ bufferIndex ] );
        willInfo.pTopicName = ( const char * ) &buf[ bufferIndex + 2U ];

        if( willInfo.topicNameLength == 0U )
        {
            LogError( ( "The will flag was set but no will topic was given." ) );
            status = MQTTBLEBadParameter;
        }
        else
        {
            bufferIndex = bufferIndex + 2U + willInfo.topicNameLength;

            /* Populate Last Will payload information. */
            willInfo.payloadLength = getIntFromTwoBytes( &buf[ bufferIndex ] );
            willInfo.pPayload = ( const char * ) &buf[ bufferIndex + 2U ];

            bufferIndex = bufferIndex + 2U + willInfo.payloadLength;
        }
    }

    if( ( status == MQTTBLESuccess ) && ( usernameFlag == true ) )
    {
        /* Populate Username header information. */
        connectConfig->userNameLength = getIntFromTwoBytes( &buf[ bufferIndex ] );
        connectConfig->pUserName = ( const char * ) &buf[ bufferIndex + 2U ];

        if( connectConfig->userNameLength == 0U )
        {
            LogError( ( "The username flag was set but no username was given." ) );
            status = MQTTBLEBadParameter;
        }

        bufferIndex = bufferIndex + 2U + connectConfig->userNameLength;
    }

    if( ( status == MQTTBLESuccess ) && ( passwordFlag == true ) )
    {
        /* Populate Password header information. */
        connectConfig->passwordLength = getIntFromTwoBytes( &buf[ bufferIndex ] );
        connectConfig->pPassword = ( const char * ) &buf[ bufferIndex + 2U ];

        if( connectConfig->passwordLength == 0U )
        {
            LogError( ( "The password flag was set but no password was given." ) );
            status = MQTTBLEBadParameter;
        }

        bufferIndex = bufferIndex + 2U + connectConfig->passwordLength;
    }

    return status;
}

static size_t getRemainingLength( const uint8_t * buf,
                                  size_t * pEncodedLength )
{
    size_t remainingLength = 0, multiplier = 1;
    uint8_t encodedByte = 0;
    size_t encodedLength = 0;

    /* This algorithm is adapted from the MQTT v3.1.1 spec. */
    do
    {
        if( multiplier > 2097152U ) /* 128 ^ 3 */
        {
            remainingLength = REMAINING_LENGTH_INVALID;
            break;
        }
        else
        {
            encodedByte = buf[ encodedLength ];
            remainingLength += ( ( size_t ) encodedByte & 0x7FU ) * multiplier;
            multiplier *= 128U;
            encodedLength++;
        }
    } while( ( encodedByte & 0x80U ) != 0U );

    *pEncodedLength = encodedLength;

    return remainingLength;
}


static bool parsePublish( const uint8_t * buf,
                          size_t length,
                          MQTTBLEPublishInfo_t * pPublishInfo,
                          uint16_t * pPacketIdentifier )
{
    uint8_t publishFlags = 0;
    size_t encodedLength = 0, remainingLength = length;
    size_t index = 0;
    bool ret = false;
    void * topicBuffer = NULL;

    /* Parse the publish header. */
    publishFlags = ( buf[ index ] & 0x0FU );

    pPublishInfo->dup = ( ( publishFlags & PUBLISH_FLAG_DUP_MASK ) == PUBLISH_FLAG_DUP_MASK );
    pPublishInfo->retain = ( ( publishFlags & PUBLISH_FLAG_RETAIN_MASK ) == PUBLISH_FLAG_RETAIN_MASK );
    pPublishInfo->qos = convertIntToQos( ( publishFlags & PUBLISH_FLAG_QOS_MASK ) >> 1 );
    index++;

    remainingLength = getRemainingLength( &buf[ index ], &encodedLength );
    configASSERT( ( remainingLength != REMAINING_LENGTH_INVALID ) );
    index += encodedLength;

    pPublishInfo->topicNameLength = UINT16_DECODE( &buf[ index ] );
    index += sizeof( uint16_t );

    topicBuffer = IotMqtt_MallocMessage( pPublishInfo->topicNameLength );

    if( topicBuffer == NULL )
    {
        LogError( ( "Failed to allocate buffer for topic name." ) );
    }
    else
    {
        memcpy( topicBuffer, &buf[ index ], pPublishInfo->topicNameLength );
        pPublishInfo->pTopicName = topicBuffer;
    }

    index += pPublishInfo->topicNameLength;

    if( pPublishInfo->qos > MQTTBLEQoS0 )
    {
        *pPacketIdentifier = UINT16_DECODE( &buf[ index ] );
        index += sizeof( uint16_t );
    }
    else
    {
        *pPacketIdentifier = 0U;
    }

    pPublishInfo->payloadLength = ( remainingLength + 1U + encodedLength - index );

    if( index < length )
    {
        pPublishInfo->pPayload = &buf[ index ];
    }
    else
    {
        if( pPublishInfo->payloadLength > 0 )
        {
            ret = true;
        }
    }

    return ret;
}

static uint16_t calculateNumSubs( const uint8_t * buf,
                                  uint8_t remainingLength,
                                  bool subscribe )
{
    uint16_t numSubs = 0, totalSize = 0;

    /* Loop through the rest of the packet. */
    while( ( remainingLength - totalSize ) > 0 )
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

static MQTTBLEStatus_t parseSubscribe( size_t * subscriptionCount,
                                       const uint8_t * buf,
                                       uint16_t * pIdentifier,
                                       bool subscribe )
{
    MQTTBLEStatus_t status = MQTTBLESuccess;
    uint8_t remainingLength = 0;
    uint16_t bufferIndex = 0;
    size_t subscriptionIndex = 0;

    *pIdentifier = getIntFromTwoBytes( &buf[ 2 ] );

    /* Stores the remaining length after the identifier */
    remainingLength = buf[ 1 ] - 2U;

    *subscriptionCount = calculateNumSubs( &buf[ 4 ], remainingLength, subscribe );

    if( *subscriptionCount == 0U )
    {
        LogError( ( "Topic filters must exist in a subscribe packet.  See [MQTT-3.8.3-3]." ) );
        status = MQTTBLEBadParameter;
    }

    if( status == MQTTBLESuccess )
    {
        bufferIndex = 4U;

        for( subscriptionIndex = 0; subscriptionIndex < *subscriptionCount; ++subscriptionIndex )
        {
            /* Populate the name of the topic to (un)subscribe to. */
            _subscriptions[ subscriptionIndex ].topicFilterLength = getIntFromTwoBytes( &buf[ bufferIndex ] );
            _subscriptions[ subscriptionIndex ].pTopicFilter = ( const char * ) &buf[ bufferIndex + 2U ];
            bufferIndex += _subscriptions[ subscriptionIndex ].topicFilterLength + 2U;

            /* For subscribe packets only, increment past qos byte. */
            if( subscribe == true )
            {
                _subscriptions[ subscriptionIndex ].qos = convertIntToQos( buf[ bufferIndex ] & 0x03U );
                bufferIndex++;
            }
        }
    }

    return status;
}


static uint16_t encodeRemainingLength( uint8_t * pBuffer,
                                       size_t length )
{
    uint16_t encodeLength = 0;
    uint8_t encodeByte;

    /* This algorithm is copied from the MQTT v3.1.1 spec. */
    do
    {
        encodeByte = ( uint8_t ) ( length % 128U );
        length = length / 128U;

        /* Set the high bit of this byte, indicating that there's more data. */
        if( length > 0U )
        {
            encodeByte |= BIT_MASK( 7U );
        }

        /* Output a single encoded byte. */
        *pBuffer = encodeByte;
        pBuffer++;
        encodeLength++;
    } while( length > 0U );

    return encodeLength;
}

static MQTTBLEStatus_t transportSerializePublish( StreamBufferHandle_t streamBuffer,
                                                  MQTTBLEPublishInfo_t * pPublishConfig,
                                                  uint16_t packetId )
{
    MQTTBLEStatus_t status = MQTTBLESuccess;
    uint8_t header[ SIZE_OF_PUBLISH_HEADER ] = { 0 };
    uint8_t serializedArray[ 2 ];
    uint8_t publishFlags = ENCODE_PACKET_TYPE( IOT_BLE_MQTT_MSG_TYPE_PUBLISH );
    size_t remainingLength, encodedLength;

    if( pPublishConfig->qos > MQTTBLEQoS1 )
    {
        /* BLE does not support QOS2 publishes. */
        status = MQTTBLEBadParameter;
    }
    else if( ( pPublishConfig->qos == MQTTBLEQoS1 ) && ( packetId == 0 ) )
    {
        status = MQTTBLEBadParameter;
    }
    else
    {
        if( pPublishConfig->dup == true )
        {
            publishFlags |= PUBLISH_FLAG_DUP_MASK;
        }

        if( pPublishConfig->retain == true )
        {
            publishFlags |= PUBLISH_FLAG_RETAIN_MASK;
        }

        if( pPublishConfig->qos == MQTTBLEQoS1 )
        {
            publishFlags |= PUBLISH_FLAG_QOS1_MASK;
        }

        header[ 0 ] = publishFlags;

        remainingLength = 2U + pPublishConfig->topicNameLength + pPublishConfig->payloadLength;

        if( pPublishConfig->qos > MQTTBLEQoS0 )
        {
            remainingLength += 2U;
        }

        encodedLength = encodeRemainingLength( &header[ 1 ], remainingLength );
    }

    if( status == MQTTBLESuccess )
    {
        /* Send the Fixed header + encoded length into the buffer */
        ( void ) xStreamBufferSend( streamBuffer, ( void * ) header, encodedLength + 1, pdMS_TO_TICKS( RECV_TIMEOUT_MS ) );

        /* Send the topic length to the buffer */
        serializedArray[ 0 ] = ( uint8_t ) ( ( pPublishConfig->topicNameLength & 0xFF00U ) >> 8 );
        serializedArray[ 1 ] = ( uint8_t ) ( pPublishConfig->topicNameLength & 0x00FFU );
        ( void ) xStreamBufferSend( streamBuffer, ( void * ) serializedArray, 2U, pdMS_TO_TICKS( RECV_TIMEOUT_MS ) );

        /* Send the topic into the buffer */
        ( void ) xStreamBufferSend( streamBuffer, ( void * ) pPublishConfig->pTopicName, pPublishConfig->topicNameLength, pdMS_TO_TICKS( RECV_TIMEOUT_MS ) );

        if( pPublishConfig->qos >= MQTTBLEQoS1 )
        {
            /* Send the packet identifier to the buffer; need to represent as two seperate bytes to resolve endianness. */
            serializedArray[ 0 ] = ( uint8_t ) ( ( packetId & 0xFF00U ) >> 8 );
            serializedArray[ 1 ] = ( uint8_t ) ( packetId & 0x00FFU );
            ( void ) xStreamBufferSend( streamBuffer, ( void * ) serializedArray, 2U, pdMS_TO_TICKS( RECV_TIMEOUT_MS ) );
        }

        /* Send the payload itself into the buffer */
        ( void ) xStreamBufferSend( streamBuffer, ( void * ) pPublishConfig->pPayload, pPublishConfig->payloadLength, pdMS_TO_TICKS( RECV_TIMEOUT_MS ) );
    }

    return status;
}

/*-----------------------------------------------------------*/

static MQTTBLEStatus_t handleOutgoingConnect( const void * buf,
                                              uint8_t ** pSerializedBuf,
                                              size_t * pSerializedBufLength )
{
    MQTTBLEStatus_t status = MQTTBLESuccess;
    MQTTBLEConnectInfo_t connectConfig = { 0 };

    LogDebug( ( "Processing outgoing CONNECT." ) );

    status = parseConnect( &connectConfig, buf );

    if( status == MQTTBLESuccess )
    {
        status = IotBleMqtt_SerializeConnect( &connectConfig, pSerializedBuf, pSerializedBufLength );
    }

    return status;
}

static MQTTBLEStatus_t handleOutgoingPublish( MQTTBLEPublishInfo_t * pPublishInfo,
                                              const void * buf,
                                              size_t bytesToSend,
                                              uint8_t ** pSerializedBuf,
                                              size_t * pSerializedBufLength )
{
    MQTTBLEStatus_t status = MQTTBLESuccess;

    LogDebug( ( "Processing outgoing PUBLISH." ) );

    if( pPublishInfo->pending == true )
    {
        configASSERT( pPublishInfo->payloadLength == bytesToSend );
        pPublishInfo->pPayload = buf;
        pPublishInfo->pending = false;
    }
    else
    {
        pPublishInfo->pending = parsePublish( buf,
                                              bytesToSend,
                                              pPublishInfo,
                                              &pPublishInfo->packetIdentifier );
    }

    LogDebug( ( "IotBleMqtt_SerializePublish before if" ) );

    if( pPublishInfo->pending == false )
    {
        status = IotBleMqtt_SerializePublish( pPublishInfo,
                                              pSerializedBuf,
                                              pSerializedBufLength,
                                              pPublishInfo->packetIdentifier );

        if( pPublishInfo->pTopicName != NULL )
        {
            IotMqtt_FreeMessage( ( void * ) pPublishInfo->pTopicName );
        }

        memset( pPublishInfo, 0x00, sizeof( MQTTBLEPublishInfo_t ) );
    }
    else
    {
        *pSerializedBuf = NULL;
        *pSerializedBufLength = 0;
    }

    return status;
}


static MQTTBLEStatus_t handleOutgoingPuback( const void * buf,
                                             uint8_t ** pSerializedBuf,
                                             size_t * pSerializedBufLength )
{
    MQTTBLEStatus_t status = MQTTBLESuccess;
    uint16_t packetIdentifier = 0;
    uint8_t * buffer = ( uint8_t * ) buf;

    LogDebug( ( "Processing outgoing PUBACK." ) );

    configASSERT( buffer[ 1 ] == ( uint8_t ) SIMPLE_ACK_REMAINING_LENGTH );

    packetIdentifier = UINT16_DECODE( &buffer[ 2 ] );

    status = IotBleMqtt_SerializePuback( packetIdentifier,
                                         pSerializedBuf,
                                         pSerializedBufLength );

    return status;
}


static MQTTBLEStatus_t handleOutgoingSubscribe( const void * buf,
                                                uint8_t ** pSerializedBuf,
                                                size_t * pSerializedBufLength )
{
    MQTTBLEStatus_t status = MQTTBLESuccess;
    uint16_t packetIdentifier = 0;
    size_t subscriptionCount = 0;

    LogDebug( ( "Processing outgoing SUBSCRIBE." ) );

    status = parseSubscribe( &subscriptionCount, buf, &packetIdentifier, true );

    if( status == MQTTBLESuccess )
    {
        status = IotBleMqtt_SerializeSubscribe( _subscriptions, subscriptionCount, pSerializedBuf, pSerializedBufLength, &packetIdentifier );
    }

    return status;
}


static MQTTBLEStatus_t handleOutgoingUnsubscribe( const void * buf,
                                                  uint8_t ** pSerializedBuf,
                                                  size_t * pSerializedBufLength )
{
    MQTTBLEStatus_t status = MQTTBLESuccess;
    uint16_t packetIdentifier = 0;
    size_t subscriptionCount = 0;

    LogDebug( ( "Processing outgoing UNSUBSCRIBE." ) );

    status = parseSubscribe( &subscriptionCount, buf, &packetIdentifier, false );

    if( status == MQTTBLESuccess )
    {
        status = IotBleMqtt_SerializeUnsubscribe( _subscriptions, subscriptionCount, pSerializedBuf, pSerializedBufLength, &packetIdentifier );
    }

    return status;
}


static MQTTBLEStatus_t handleOutgoingPingReq( uint8_t ** pSerializedBuf,
                                              size_t * pSerializedBufLength )
{
    MQTTBLEStatus_t status = MQTTBLESuccess;

    LogDebug( ( "Processing outgoing PINGREQ." ) );
    status = IotBleMqtt_SerializePingreq( pSerializedBuf, pSerializedBufLength );
    return status;
}

static MQTTBLEStatus_t handleOutgoingDisconnect( uint8_t ** pSerializedBuf,
                                                 size_t * pSerializedBufLength )
{
    MQTTBLEStatus_t status = MQTTBLESuccess;

    LogDebug( ( "Processing outgoing DISCONNECT." ) );

    status = IotBleMqtt_SerializeDisconnect( pSerializedBuf, pSerializedBufLength );
    return status;
}

/*-----------------------------------------------------------*/

static MQTTBLEStatus_t handleIncomingConnack( StreamBufferHandle_t streamBuffer,
                                              uint8_t * pPacket,
                                              size_t length )
{
    MQTTBLEStatus_t status = MQTTBLESuccess;
    uint8_t buffer[ SIZE_OF_SIMPLE_ACK ] = { 0 };

    LogDebug( ( "Processing incoming CONNACK from channel." ) );

    status = IotBleMqtt_DeserializeConnack( pPacket, length );

    if( status != MQTTBLEBadResponse )
    {
        buffer[ 0 ] = ENCODE_PACKET_TYPE( IOT_BLE_MQTT_MSG_TYPE_CONNACK );
        /* Remaining length is always 2 for CONNACK. */
        buffer[ 1 ] = 2;
        /* Ack flags 1-7 bits are reserved. 0th bit is session present which is false for BLE. */
        buffer[ 2 ] = 0;

        /* Return code is either success or 0x05 for Connection refused */
        if( status == MQTTBLESuccess )
        {
            buffer[ 3 ] = 0;
        }
        else
        {
            buffer[ 3 ] = CONNACK_REFUSED;
        }

        ( void ) xStreamBufferSend( streamBuffer, buffer, SIZE_OF_SIMPLE_ACK, pdMS_TO_TICKS( RECV_TIMEOUT_MS ) );
    }

    return status;
}

static MQTTBLEStatus_t handleIncomingPuback( StreamBufferHandle_t streamBuffer,
                                             uint8_t * pPacket,
                                             size_t length )
{
    MQTTBLEStatus_t status = MQTTBLESuccess;
    uint16_t packetIdentifier = 0;
    uint8_t buffer[ SIZE_OF_SIMPLE_ACK ] = { 0 };

    LogDebug( ( "Processing incoming PUBACK from channel." ) );
    status = IotBleMqtt_DeserializePuback( pPacket, length, &packetIdentifier );

    if( status == MQTTBLESuccess )
    {
        buffer[ 0 ] = ENCODE_PACKET_TYPE( IOT_BLE_MQTT_MSG_TYPE_PUBACK );
        buffer[ 1 ] = 2;
        buffer[ 2 ] = ( uint8_t ) ( packetIdentifier >> 8 );
        buffer[ 3 ] = ( uint8_t ) ( packetIdentifier & 0x00FFU );

        ( void ) xStreamBufferSend( streamBuffer, buffer, SIZE_OF_SIMPLE_ACK, pdMS_TO_TICKS( RECV_TIMEOUT_MS ) );
    }

    return status;
}

static MQTTBLEStatus_t handleIncomingPublish( StreamBufferHandle_t streamBuffer,
                                              uint8_t * pPacket,
                                              size_t length )
{
    MQTTBLEStatus_t status = MQTTBLESuccess;
    MQTTBLEPublishInfo_t publishInfo;
    uint16_t packetIdentifier = 0;

    LogDebug( ( "Processing incoming PUBLISH from channel." ) );

    status = IotBleMqtt_DeserializePublish( pPacket, length, &publishInfo, &packetIdentifier );

    if( status == MQTTBLESuccess )
    {
        status = transportSerializePublish( streamBuffer, &publishInfo, packetIdentifier );
    }

    return status;
}


static MQTTBLEStatus_t handleIncomingSuback( StreamBufferHandle_t streamBuffer,
                                             uint8_t * pPacket,
                                             size_t length )
{
    MQTTBLEStatus_t status = MQTTBLESuccess;
    uint16_t packetIdentifier = 0;
    uint8_t buffer[ SIZE_OF_SUB_ACK ] = { 0 };
    uint8_t statusCode;

    LogDebug( ( "Processing incoming SUBACK from channel." ) );
    status = IotBleMqtt_DeserializeSuback( pPacket, length, &packetIdentifier, &statusCode );

    if( status == MQTTBLESuccess )
    {
        buffer[ 0 ] = ENCODE_PACKET_TYPE( IOT_BLE_MQTT_MSG_TYPE_SUBACK );

        /*
         * Only one subscription status ( succeeded / failed ) is sent from the peer for all subscriptions.
         * Hence setting the remaining length to a constant value of 3 bytes
         */
        buffer[ 1 ] = SUB_ACK_REMAINING_LENGTH;
        buffer[ 2 ] = ( uint8_t ) ( packetIdentifier >> 8 );
        buffer[ 3 ] = ( uint8_t ) ( packetIdentifier & 0x00FFU );
        buffer[ 4 ] = statusCode;

        ( void ) xStreamBufferSend( streamBuffer,
                                    buffer,
                                    SIZE_OF_SUB_ACK,
                                    pdMS_TO_TICKS( RECV_TIMEOUT_MS ) );
    }

    return status;
}

static MQTTBLEStatus_t handleIncomingUnsuback( StreamBufferHandle_t streamBuffer,
                                               uint8_t * pPacket,
                                               size_t length )
{
    MQTTBLEStatus_t status = MQTTBLESuccess;
    uint16_t packetIdentifier = 0;
    uint8_t buffer[ SIZE_OF_SIMPLE_ACK ] = { 0 };

    LogDebug( ( "Processing incoming UNSUBACK from channel." ) );
    status = IotBleMqtt_DeserializeUnsuback( pPacket, length, &packetIdentifier );

    if( status == MQTTBLESuccess )
    {
        buffer[ 0 ] = ENCODE_PACKET_TYPE( IOT_BLE_MQTT_MSG_TYPE_UNSUBACK );
        buffer[ 1 ] = 2;
        buffer[ 2 ] = ( uint8_t ) ( packetIdentifier >> 8 );
        buffer[ 3 ] = ( uint8_t ) ( packetIdentifier & 0x00FFU );

        ( void ) xStreamBufferSend( streamBuffer, buffer, SIZE_OF_SIMPLE_ACK, pdMS_TO_TICKS( RECV_TIMEOUT_MS ) );
    }

    return status;
}


static MQTTBLEStatus_t handleIncomingPingresp( StreamBufferHandle_t streamBuffer,
                                               uint8_t * pPacket,
                                               size_t length )
{
    MQTTBLEStatus_t status = MQTTBLESuccess;
    uint8_t buffer[ SIZE_OF_PING ] = { 0 };

    LogDebug( ( "Processing incoming PINGRESP from channel." ) );
    status = IotBleMqtt_DeserializePingresp( pPacket, length );

    if( status == MQTTBLESuccess )
    {
        buffer[ 0 ] = ENCODE_PACKET_TYPE( IOT_BLE_MQTT_MSG_TYPE_PINGRESP );
        buffer[ 1 ] = 0;

        ( void ) xStreamBufferSend( streamBuffer, buffer, SIZE_OF_PING, pdMS_TO_TICKS( RECV_TIMEOUT_MS ) );
    }

    return status;
}

/*-----------------------------------------------------------*/


/**
 * @brief Transport interface send API implementation.
 *
 * @param[in] context An opaque used by transport interface.
 * @param[in] pBuffer A pointer to a buffer containing data to be sent out.
 * @param[in] bytesToWrite number of bytes to write from the buffer.
 */
int32_t IotBleMqttTransportSend( NetworkContext_t * pContext,
                                 const void * pBuffer,
                                 size_t bytesToWrite )
{
    size_t bytesSent = 0;
    uint8_t * pBuf = ( uint8_t * ) pBuffer;
    MQTTBLEStatus_t status = MQTTBLESuccess;
    uint8_t * pSerializedPacket = NULL;
    size_t serializedLength = 0;
    uint8_t packetType;
    BleTransportParams_t * pBleTransportParams = NULL;

    /* The send function returns the CBOR bytes written, so need to return 0 or full amount of bytes sent. */
    int32_t bytesWritten = ( int32_t ) bytesToWrite;

    /*
     * The payload of publish can be send as a separate packet from the header. The flag is used to check for pending publish
     * payload information and process the payload part of the publish.
     */
    pBleTransportParams = pContext->pParams;

    if( pBleTransportParams->publishInfo.pending == true )
    {
        status = handleOutgoingPublish( ( MQTTBLEPublishInfo_t * ) &pBleTransportParams->publishInfo,
                                        pBuffer,
                                        bytesToWrite,
                                        &pSerializedPacket,
                                        &serializedLength );
    }
    else
    {
        packetType = DECODE_PACKET_TYPE( pBuf[ 0 ] );

        LogDebug( ( "Outgoing packet type is %d ", packetType ) );

        switch( packetType )
        {
            case IOT_BLE_MQTT_MSG_TYPE_CONNECT:
                status = handleOutgoingConnect( pBuffer,
                                                &pSerializedPacket,
                                                &serializedLength );
                break;

            case IOT_BLE_MQTT_MSG_TYPE_PUBLISH:
                status = handleOutgoingPublish( ( MQTTBLEPublishInfo_t * ) &pBleTransportParams->publishInfo,
                                                pBuffer,
                                                bytesToWrite,
                                                &pSerializedPacket,
                                                &serializedLength );
                break;

            case IOT_BLE_MQTT_MSG_TYPE_PUBACK:
                status = handleOutgoingPuback( pBuffer,
                                               &pSerializedPacket,
                                               &serializedLength );
                break;

            case IOT_BLE_MQTT_MSG_TYPE_SUBSCRIBE:
                status = handleOutgoingSubscribe( pBuffer,
                                                  &pSerializedPacket,
                                                  &serializedLength );
                break;

            case IOT_BLE_MQTT_MSG_TYPE_UNSUBSCRIBE:
                status = handleOutgoingUnsubscribe( pBuffer,
                                                    &pSerializedPacket,
                                                    &serializedLength );
                break;

            case IOT_BLE_MQTT_MSG_TYPE_PINGREQ:
                status = handleOutgoingPingReq( &pSerializedPacket,
                                                &serializedLength );
                break;

            case IOT_BLE_MQTT_MSG_TYPE_DISCONNECT:
                status = handleOutgoingDisconnect( &pSerializedPacket,
                                                   &serializedLength );
                break;

            /* QoS 2 cases, currently not supported by BLE */
            case IOT_BLE_MQTT_MSG_TYPE_PUBREC:
            case IOT_BLE_MQTT_MSG_TYPE_PUBREL:
            case IOT_BLE_MQTT_MSG_TYPE_PUBCOMP:
                status = MQTTBLEBadParameter;
                LogError( ( "Only Qos 0 and 1 are supported over BLE." ) );
                break;

            /* Client tries to send a server to client only packet */
            default:
                status = MQTTBLEBadParameter;
                LogError( ( "A server to client only packet was sent. Check packet type or ensure Qos < 2." ) );
                LogError( ( "Your packet type was %i", packetType ) );
                break;
        }
    }

    if( status == MQTTBLESuccess )
    {
        if( serializedLength > 0 )
        {
            bytesSent = IotBleDataTransfer_Send( pBleTransportParams->pChannel,
                                                 pSerializedPacket,
                                                 serializedLength );

            if( bytesSent != serializedLength )
            {
                LogError( ( "Cannot send %lu bytes through BLE channel, sent %lu bytes.",
                            serializedLength, bytesSent ) );
                bytesWritten = 0;
            }
            else
            {
                LogDebug( ( "Successfully sent %d bytes through BLE channel.",
                            serializedLength ) );
            }

            IotMqtt_FreeMessage( pSerializedPacket );
        }
    }
    else
    {
        /* The user would have already seen a log for the previous error */
        bytesWritten = 0;
    }

    return bytesWritten;
}

MQTTBLEStatus_t IotBleMqttTransportAcceptData( const NetworkContext_t * pContext )
{
    MQTTBLEStatus_t status = MQTTBLESuccess;
    uint8_t packetType;
    uint8_t * pPacket;
    size_t packetLength;
    BleTransportParams_t * pBleTransportParams = NULL;

    configASSERT( pContext != NULL );
    pBleTransportParams = pContext->pParams;
    configASSERT( pBleTransportParams != NULL );

    IotBleDataTransfer_PeekReceiveBuffer( pBleTransportParams->pChannel,
                                          ( const uint8_t ** ) &pPacket,
                                          &packetLength );

    packetType = IotBleMqtt_GetPacketType( pPacket, packetLength );

    switch( packetType )
    {
        case IOT_BLE_MQTT_MSG_TYPE_CONNACK:
            status = handleIncomingConnack( pBleTransportParams->xStreamBuffer, pPacket, packetLength );
            break;

        case IOT_BLE_MQTT_MSG_TYPE_PUBLISH:
            status = handleIncomingPublish( pBleTransportParams->xStreamBuffer, pPacket, packetLength );
            break;

        case IOT_BLE_MQTT_MSG_TYPE_PUBACK:
            status = handleIncomingPuback( pBleTransportParams->xStreamBuffer, pPacket, packetLength );
            break;

        case IOT_BLE_MQTT_MSG_TYPE_SUBACK:
            status = handleIncomingSuback( pBleTransportParams->xStreamBuffer, pPacket, packetLength );
            break;

        case IOT_BLE_MQTT_MSG_TYPE_UNSUBACK:
            status = handleIncomingUnsuback( pBleTransportParams->xStreamBuffer, pPacket, packetLength );
            break;

        case IOT_BLE_MQTT_MSG_TYPE_PINGRESP:
            status = handleIncomingPingresp( pBleTransportParams->xStreamBuffer, pPacket, packetLength );
            break;

        /* QoS 2 cases, currently not supported by BLE */
        case IOT_BLE_MQTT_MSG_TYPE_PUBREC:
        case IOT_BLE_MQTT_MSG_TYPE_PUBREL:
        case IOT_BLE_MQTT_MSG_TYPE_PUBCOMP:
            status = MQTTBLEBadParameter;

            LogError( ( "Only Qos 0 and 1 are supported over BLE." ) );
            break;

        /* Server tries to send a client to server only packet */
        default:
            LogError( ( "Client received a client to server only packet." ) );
            status = MQTTBLEBadParameter;
            break;
    }

    if( status != MQTTBLESuccess )
    {
        LogError( ( "An error occured when receiving data from the channel. No data was recorded." ) );
    }
    else
    {
        /* Flush the data from the channel */
        ( void ) IotBleDataTransfer_Receive( pBleTransportParams->pChannel, NULL, packetLength );
    }

    return status;
}


/**
 * @brief Transport interface read prototype.
 *
 * @param[in] context An opaque used by transport interface.
 * @param[out] buf A pointer to a buffer where incoming data will be stored.
 * @param[in] bytesToRead number of bytes to read from the transport layer.
 */
int32_t IotBleMqttTransportReceive( NetworkContext_t * pContext,
                                    void * pBuffer,
                                    size_t bytesToRead )
{
    BleTransportParams_t * pBleTransportParams = NULL;

    pBleTransportParams = pContext->pParams;
    return ( int32_t ) xStreamBufferReceive( pBleTransportParams->xStreamBuffer, pBuffer, bytesToRead, pdMS_TO_TICKS( RECV_TIMEOUT_MS ) );
}
