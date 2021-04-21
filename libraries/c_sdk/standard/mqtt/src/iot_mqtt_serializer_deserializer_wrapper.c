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
 * https://github.com/freertos
 * https://www.FreeRTOS.org
 */

/**
 * @file iot_mqtt_serializer_deserializer_wrapper.c
 * @brief Implements serializer and deserializer wrapper functions for the shim.
 */
/* The config header is always included first. */
#include "iot_config.h"

/* Standard includes. */
#include <string.h>

/* Error handling include. */
#include "private/iot_error.h"

/* MQTT internal includes. */
#include "private/iot_mqtt_internal.h"

/* Atomic operations. */
#include "iot_atomic.h"

/*-----------------------------------------------------------*/

/* Size of Puback packet. */
#define MQTT_PACKET_PUBACK_SIZE       ( 4 )

/**
 * @brief Per the MQTT 3.1.1 spec, the largest "Remaining Length" of an MQTT
 * packet is this value.
 */
#define MQTT_MAX_REMAINING_LENGTH     ( 268435455UL )

/*
 * @brief Return status codes for subscription as per the MQTT 3.1.1 spec.
 */
#define SUBSCRIPTION_ACCEPTED_QOS0    ( 0x00 )               /**< @brief  Status code for accepted QOS0 subscription. */
#define SUBSCRIPTION_ACCEPTED_QOS1    ( 0x01 )               /**< @brief  Status code for accepted QOS1 subscription. */
#define SUBSCRIPTION_ACCEPTED_QOS2    ( 0x02 )               /**< @brief  Status code for accepted QOS1 subscription. */
#define SUBSCRIPTION_REFUSED          ( 0x80 )               /**< @brief  Status code for refused subscription. */

/*-----------------------------------------------------------*/

/* Generate Id for packet. */
static uint16_t _nextPacketIdentifier( void );

/*-----------------------------------------------------------*/

/* Generate Id for packet. */
static uint16_t _nextPacketIdentifier( void )
{
    /* MQTT specifies 2 bytes for the packet identifier; however, operating on
     * 32-bit integers is generally faster. */
    static uint32_t nextPacketIdentifier = 1;

    /* The next packet identifier will be greater by 2. This prevents packet
     * identifiers from ever being 0, which is not allowed by MQTT 3.1.1. Packet
     * identifiers will follow the sequence 1,3,5...65535,1,3,5... */
    return ( uint16_t ) Atomic_Add_u32( &nextPacketIdentifier, 2 );
}

/*-----------------------------------------------------------*/

/**
 * @brief Calculate the number of bytes required to encode an MQTT
 * "Remaining length" field.
 *
 * @param[in] length The value of the "Remaining length" to encode.
 *
 * @return The size of the encoding of length. This is always `1`, `2`, `3`, or `4`.
 */
static size_t _remainingLengthEncodedSize( size_t length );

/*-----------------------------------------------------------*/

static size_t _remainingLengthEncodedSize( size_t length )
{
    size_t encodedSize = 0;

    /* length should have already been checked before calling this function. */
    IotMqtt_Assert( length <= MQTT_MAX_REMAINING_LENGTH );

    /* Determine how many bytes are needed to encode length.
     * The values below are taken from the MQTT 3.1.1 spec. */

    /* 1 byte is needed to encode lengths between 0 and 127. */
    if( length < 128 )
    {
        encodedSize = 1;
    }
    /* 2 bytes are needed to encode lengths between 128 and 16,383. */
    else if( length < 16384 )
    {
        encodedSize = 2;
    }
    /* 3 bytes are needed to encode lengths between 16,384 and 2,097,151. */
    else if( length < 2097152 )
    {
        encodedSize = 3;
    }
    /* 4 bytes are needed to encode lengths between 2,097,152 and 268,435,455. */
    else
    {
        encodedSize = 4;
    }

    return encodedSize;
}

/*-----------------------------------------------------------*/

uint8_t _IotMqtt_GetPacketType( void * pNetworkConnection,
                                const IotNetworkInterface_t * pNetworkInterface )
{
    uint8_t packetType = 0xff;

    /* The MQTT packet type is in the first byte of the packet. */
    ( void ) _IotMqtt_GetNextByte( pNetworkConnection,
                                   pNetworkInterface,
                                   &packetType );

    return packetType;
}

/*-----------------------------------------------------------*/

size_t _IotMqtt_GetRemainingLength( void * pNetworkConnection,
                                    const IotNetworkInterface_t * pNetworkInterface )
{
    uint8_t encodedByte = 0;
    size_t remainingLength = 0, multiplier = 1, bytesDecoded = 0, expectedSize = 0;

    /* This algorithm is copied from the MQTT v3.1.1 spec. */
    do
    {
        if( multiplier > 2097152 ) /* 128 ^ 3 */
        {
            remainingLength = MQTT_REMAINING_LENGTH_INVALID;
            break;
        }
        else
        {
            if( _IotMqtt_GetNextByte( pNetworkConnection,
                                      pNetworkInterface,
                                      &encodedByte ) == true )
            {
                remainingLength += ( encodedByte & 0x7F ) * multiplier;
                multiplier *= 128;
                bytesDecoded++;
            }
            else
            {
                remainingLength = MQTT_REMAINING_LENGTH_INVALID;
                break;
            }
        }
    } while( ( encodedByte & 0x80 ) != 0 );

    /* Check that the decoded remaining length conforms to the MQTT specification. */
    if( remainingLength != MQTT_REMAINING_LENGTH_INVALID )
    {
        expectedSize = _remainingLengthEncodedSize( remainingLength );

        if( bytesDecoded != expectedSize )
        {
            remainingLength = MQTT_REMAINING_LENGTH_INVALID;
        }
        else
        {
            /* Valid remaining length should be at most 4 bytes. */
            IotMqtt_Assert( bytesDecoded <= 4 );
        }
    }
    else
    {
        EMPTY_ELSE_MARKER;
    }

    return remainingLength;
}

/*-----------------------------------------------------------*/

/* Connect Serialize Wrapper. */
IotMqttError_t _IotMqtt_connectSerializeWrapper( const IotMqttConnectInfo_t * pConnectInfo,
                                                 uint8_t ** pConnectPacket,
                                                 size_t * pPacketSize )
{
    IotMqttError_t status = IOT_MQTT_BAD_PARAMETER;
    size_t remainingLength = 0UL;
    MQTTConnectInfo_t connectInfo = { 0 };
    MQTTFixedBuffer_t networkBuffer = { 0 };
    MQTTStatus_t managedMqttStatus = MQTTBadParameter;
    MQTTPublishInfo_t willInfo;

    /* Null Check for connectInfo. */
    IotMqtt_Assert( pConnectInfo != NULL );
    IotMqtt_Assert( pConnectPacket != NULL );
    IotMqtt_Assert( pPacketSize != NULL );

    connectInfo.cleanSession = pConnectInfo->cleanSession;
    connectInfo.keepAliveSeconds = pConnectInfo->keepAliveSeconds;
    connectInfo.pClientIdentifier = pConnectInfo->pClientIdentifier;
    connectInfo.clientIdentifierLength = pConnectInfo->clientIdentifierLength;
    connectInfo.pUserName = pConnectInfo->pUserName;
    connectInfo.userNameLength = pConnectInfo->userNameLength;
    connectInfo.pPassword = pConnectInfo->pPassword;
    connectInfo.passwordLength = pConnectInfo->passwordLength;
    const MQTTPublishInfo_t * pWillInfo = pConnectInfo->pWillInfo != NULL ? &willInfo : NULL;

    /* NULL Check willInfo. */
    if( pWillInfo != NULL )
    {
        willInfo.retain = pConnectInfo->pWillInfo->retain;
        willInfo.pTopicName = pConnectInfo->pWillInfo->pTopicName;
        willInfo.topicNameLength = pConnectInfo->pWillInfo->topicNameLength;
        willInfo.pPayload = pConnectInfo->pWillInfo->pPayload;
        willInfo.payloadLength = pConnectInfo->pWillInfo->payloadLength;
        willInfo.qos = ( MQTTQoS_t ) pConnectInfo->pWillInfo->qos;
    }

    /* Getting Connect packet size using MQTT V4_beta2 API. */

    managedMqttStatus = MQTT_GetConnectPacketSize( &connectInfo,
                                                   pWillInfo,
                                                   &remainingLength,
                                                   pPacketSize );
    /* Converting status code from MQTT v4_beta2 status to MQTT v4_beta 1 status. */
    status = convertReturnCode( managedMqttStatus );

    if( status == IOT_MQTT_SUCCESS )
    {
        /* Allocating memory for Connect packet. */
        networkBuffer.pBuffer = IotMqtt_MallocMessage( *pPacketSize );
        networkBuffer.size = *pPacketSize;

        /* Serializing the connect packet and validating the serialize parameters using MQTT V4_beta2 API. */
        managedMqttStatus = MQTT_SerializeConnect( &connectInfo,
                                                   pWillInfo,
                                                   remainingLength,
                                                   &( networkBuffer ) );

        /* Converting status code from MQTT v4_beta2 status to MQTT v4_beta 1 status. */
        status = convertReturnCode( managedMqttStatus );
    }

    if( status == IOT_MQTT_SUCCESS )
    {
        *pConnectPacket = networkBuffer.pBuffer;
    }

    return status;
}

/*-----------------------------------------------------------*/

/* Disconnect Serialize Wrapper. */
IotMqttError_t _IotMqtt_disconnectSerializeWrapper( uint8_t ** pDisconnectPacket,
                                                    size_t * pPacketSize )
{
    IotMqttError_t status = IOT_MQTT_BAD_PARAMETER;
    MQTTFixedBuffer_t networkBuffer = { 0 };
    MQTTStatus_t managedMqttStatus = MQTTBadParameter;

    IotMqtt_Assert( pDisconnectPacket != NULL );
    IotMqtt_Assert( pPacketSize != NULL );

    /* Getting Disconnect packet size using MQTT V4_beta2 API. */
    managedMqttStatus = MQTT_GetDisconnectPacketSize( pPacketSize );

    /* Converting status code from MQTT v4_beta2 status to MQTT v4_beta 1 status. */
    status = convertReturnCode( managedMqttStatus );

    if( status == IOT_MQTT_SUCCESS )
    {
        /* Allocate memory to hold the Disconnect packet. */
        networkBuffer.pBuffer = IotMqtt_MallocMessage( *pPacketSize );
        networkBuffer.size = *pPacketSize;

        /* Serializing the Disconnect packet and validating the serialize parameters using MQTT V4_beta2 API. */
        managedMqttStatus = MQTT_SerializeDisconnect( &( networkBuffer ) );

        /* Converting status code from MQTT v4_beta2 status to MQTT v4_beta 1 status. */
        status = convertReturnCode( managedMqttStatus );
    }

    if( status == IOT_MQTT_SUCCESS )
    {
        *pDisconnectPacket = networkBuffer.pBuffer;
    }

    return status;
}

/*-----------------------------------------------------------*/

/* Subscribe Serialize Wrapper. */
IotMqttError_t _IotMqtt_subscribeSerializeWrapper( const IotMqttSubscription_t * pSubscriptionList,
                                                   size_t subscriptionCount,
                                                   uint8_t ** pSubscribePacket,
                                                   size_t * pPacketSize,
                                                   uint16_t * pPacketIdentifier )
{
    IotMqttError_t status = IOT_MQTT_BAD_PARAMETER;
    MQTTStatus_t managedMqttStatus = MQTTBadParameter;
    size_t remainingLength = 0UL;
    MQTTSubscribeInfo_t * subscriptionList = NULL;
    size_t i = 0;
    MQTTFixedBuffer_t networkBuffer = { 0 };
    uint16_t packetId = 0;

    IotMqtt_Assert( pSubscriptionList != NULL );
    IotMqtt_Assert( pSubscribePacket != NULL );
    IotMqtt_Assert( pPacketSize != NULL );
    IotMqtt_Assert( pPacketIdentifier != NULL );

    /* Allocating Memory for subscription List. */
    subscriptionList = IotMqtt_MallocMessage( sizeof( MQTTSubscribeInfo_t ) * subscriptionCount );

    if( subscriptionList == NULL )
    {
        IotLogError( "Failed to allocate memory for subscription list." );
        status = IOT_MQTT_NO_MEMORY;
    }
    else
    {
        EMPTY_ELSE_MARKER;
    }

    if( subscriptionList != NULL )
    {
        for( i = 0; i < subscriptionCount; i++ )
        {
            subscriptionList[ i ].qos = ( MQTTQoS_t ) ( pSubscriptionList + i )->qos;
            subscriptionList[ i ].pTopicFilter = ( pSubscriptionList + i )->pTopicFilter;
            subscriptionList[ i ].topicFilterLength = ( pSubscriptionList + i )->topicFilterLength;
        }

        /* Getting Subscribe packet size  using MQTT V4_beta2 API. */
        managedMqttStatus = MQTT_GetSubscribePacketSize( subscriptionList,
                                                         subscriptionCount,
                                                         &remainingLength,
                                                         pPacketSize );

        /* Converting status code from MQTT v4_beta2 status to MQTT v4_beta 1 status. */
        status = convertReturnCode( managedMqttStatus );
    }

    if( status == IOT_MQTT_SUCCESS )
    {
        /* Generating the packet id for subscribe packet. */
        packetId = _nextPacketIdentifier();


        /* Allocating memory for subscribe packet. */
        networkBuffer.pBuffer = IotMqtt_MallocMessage( *pPacketSize );
        networkBuffer.size = *pPacketSize;

        /* Serializing the Subscribe packet and validating the serialize parameters using MQTT V4_beta2 API. */
        managedMqttStatus = MQTT_SerializeSubscribe( subscriptionList,
                                                     subscriptionCount,
                                                     packetId,
                                                     remainingLength,
                                                     &( networkBuffer ) );

        /* Converting status code from MQTT v4_beta2 status to MQTT v4_beta 1 status. */
        status = convertReturnCode( managedMqttStatus );
    }

    if( status == IOT_MQTT_SUCCESS )
    {
        *pSubscribePacket = networkBuffer.pBuffer;
        *pPacketIdentifier = packetId;
    }

    /* Free allocated memory as the packet was serialized. */
    IotMqtt_FreeMessage( subscriptionList );

    return status;
}

/*-----------------------------------------------------------*/

/* Unsubscribe Serialize Wrapper. */
IotMqttError_t _IotMqtt_unsubscribeSerializeWrapper( const IotMqttSubscription_t * pUnsubscriptionList,
                                                     size_t unsubscriptionCount,
                                                     uint8_t ** pUnsubscribePacket,
                                                     size_t * pPacketSize,
                                                     uint16_t * pPacketIdentifier )
{
    IotMqttError_t status = IOT_MQTT_BAD_PARAMETER;
    MQTTStatus_t managedMqttStatus = MQTTBadParameter;
    size_t remainingLength = 0UL;
    MQTTSubscribeInfo_t * unsubscriptionList = NULL;
    size_t i = 0;
    uint16_t packetId = 0;
    MQTTFixedBuffer_t networkBuffer = { 0 };

    IotMqtt_Assert( pUnsubscriptionList != NULL );
    IotMqtt_Assert( pUnsubscribePacket != NULL );
    IotMqtt_Assert( pPacketSize != NULL );
    IotMqtt_Assert( pPacketIdentifier != NULL );

    /* Allocating Memory for unsubscription List. */
    unsubscriptionList = IotMqtt_MallocMessage( sizeof( MQTTSubscribeInfo_t ) * unsubscriptionCount );

    if( unsubscriptionList == NULL )
    {
        IotLogError( "Failed to allocate memory for unsubscription list." );
        status = IOT_MQTT_NO_MEMORY;
    }
    else
    {
        EMPTY_ELSE_MARKER;
    }

    if( unsubscriptionList != NULL )
    {
        for( i = 0; i < unsubscriptionCount; i++ )
        {
            unsubscriptionList[ i ].qos = ( MQTTQoS_t ) ( pUnsubscriptionList + i )->qos;
            unsubscriptionList[ i ].pTopicFilter = ( pUnsubscriptionList + i )->pTopicFilter;
            unsubscriptionList[ i ].topicFilterLength = ( pUnsubscriptionList + i )->topicFilterLength;
        }

        /* Getting Unsubscribe packet size  using MQTT V4_beta2 API. */
        managedMqttStatus = MQTT_GetUnsubscribePacketSize( unsubscriptionList,
                                                           unsubscriptionCount,
                                                           &remainingLength,
                                                           pPacketSize );

        /* Converting status code from MQTT v4_beta2 status to MQTT v4_beta 1 status. */
        status = convertReturnCode( managedMqttStatus );
    }

    if( status == IOT_MQTT_SUCCESS )
    {
        /* Generating the packet id for subscribe packet. */
        packetId = _nextPacketIdentifier();

        /* Allocating memory for unsubscribe packet. */
        networkBuffer.pBuffer = IotMqtt_MallocMessage( *pPacketSize );
        networkBuffer.size = *pPacketSize;

        /* Serializing the Unsubscribe packet and validate the serialize parameters using MQTT V4_beta2 API. */
        managedMqttStatus = MQTT_SerializeUnsubscribe( unsubscriptionList,
                                                       unsubscriptionCount,
                                                       packetId,
                                                       remainingLength,
                                                       &( networkBuffer ) );

        /* Converting status code from MQTT v4_beta2 status to MQTT v4_beta 1 status. */
        status = convertReturnCode( managedMqttStatus );
    }

    if( status == IOT_MQTT_SUCCESS )
    {
        *pUnsubscribePacket = networkBuffer.pBuffer;
        *pPacketIdentifier = packetId;
    }

    /* Free allocated memory as the packet was serialized. */
    IotMqtt_FreeMessage( unsubscriptionList );

    return status;
}

/*-----------------------------------------------------------*/

/* Publish Serialize Wrapper. */
IotMqttError_t _IotMqtt_publishSerializeWrapper( const IotMqttPublishInfo_t * pPublishInfo,
                                                 uint8_t ** pPublishPacket,
                                                 size_t * pPacketSize,
                                                 uint16_t * pPacketIdentifier,
                                                 uint8_t ** pPacketIdentifierHigh )
{
    IotMqttError_t status = IOT_MQTT_BAD_PARAMETER;
    size_t remainingLength = 0UL;
    uint8_t * pBuffer = NULL;
    MQTTStatus_t managedMqttStatus = MQTTBadParameter;
    MQTTPublishInfo_t publishInfo;
    uint16_t packetId = 0;
    MQTTFixedBuffer_t networkBuffer = { 0 };

    /* Unused parameter. */
    ( void ) pPacketIdentifierHigh;

    /* Null Check for publishInfo. */
    IotMqtt_Assert( pPublishInfo != NULL );
    IotMqtt_Assert( pPublishPacket != NULL );
    IotMqtt_Assert( pPacketSize != NULL );
    IotMqtt_Assert( pPacketIdentifier != NULL );

    publishInfo.retain = pPublishInfo->retain;
    publishInfo.pTopicName = pPublishInfo->pTopicName;
    publishInfo.topicNameLength = pPublishInfo->topicNameLength;
    publishInfo.pPayload = pPublishInfo->pPayload;
    publishInfo.payloadLength = pPublishInfo->payloadLength;
    publishInfo.qos = ( MQTTQoS_t ) pPublishInfo->qos;
    publishInfo.dup = false;

    /* Getting publish packet size  using MQTT V4_beta2 API. */
    managedMqttStatus = MQTT_GetPublishPacketSize( &publishInfo,
                                                   &remainingLength,
                                                   pPacketSize );

    /* Converting status code from MQTT v4_beta2 status to MQTT v4_beta 1 status. */
    status = convertReturnCode( managedMqttStatus );

    if( status == IOT_MQTT_SUCCESS )
    {
        /* Generating the packet id for publish packet. */
        packetId = _nextPacketIdentifier();

        /* Allocating memory to hold publish packet. */
        pBuffer = IotMqtt_MallocMessage( *pPacketSize );
        networkBuffer.pBuffer = pBuffer;
        networkBuffer.size = *pPacketSize;

        /* Serializing the publish packet and validating the serialize parameters using MQTT V4_beta2 API. */
        managedMqttStatus = MQTT_SerializePublish( &publishInfo,
                                                   packetId,
                                                   remainingLength,
                                                   &( networkBuffer ) );

        /* Converting status code from MQTT v4_beta2 status to MQTT v4_beta 1 status. */
        status = convertReturnCode( managedMqttStatus );
    }

    if( status == IOT_MQTT_SUCCESS )
    {
        *pPublishPacket = networkBuffer.pBuffer;
        *pPacketIdentifier = packetId;
    }

    return status;
}

/*-----------------------------------------------------------*/

/* Pingreq Serialize Wrapper. */
IotMqttError_t _IotMqtt_pingreqSerializeWrapper( uint8_t ** pPingreqPacket,
                                                 size_t * pPacketSize )
{
    IotMqttError_t serializeStatus = IOT_MQTT_BAD_PARAMETER;
    MQTTFixedBuffer_t networkBuffer = { 0 };
    MQTTStatus_t managedMqttStatus = MQTTBadParameter;

    IotMqtt_Assert( pPingreqPacket != NULL );
    IotMqtt_Assert( pPacketSize != NULL );

    /* Getting pingrequest packet size  using MQTT V4_beta2 API. */
    managedMqttStatus = MQTT_GetPingreqPacketSize( pPacketSize );

    /* Converting status code from MQTT v4_beta2 status to MQTT v4_beta 1 status. */
    serializeStatus = convertReturnCode( managedMqttStatus );

    if( serializeStatus == IOT_MQTT_SUCCESS )
    {
        /* Allocate memory to hold the Pingrequest packet. */
        networkBuffer.pBuffer = IotMqtt_MallocMessage( *pPacketSize );
        networkBuffer.size = *pPacketSize;

        /* Serializing the pingrequest packet and validating the serialize parameters using MQTT V4_beta2 API. */
        managedMqttStatus = MQTT_SerializePingreq( &( networkBuffer ) );

        /* Converting status code from MQTT v4_beta2 status to MQTT v4_beta 1 status. */
        serializeStatus = convertReturnCode( managedMqttStatus );
    }

    if( serializeStatus == IOT_MQTT_SUCCESS )
    {
        *pPingreqPacket = networkBuffer.pBuffer;
    }

    return serializeStatus;
}

/*-----------------------------------------------------------*/

/* Deserialize Connack Wrapper. */
IotMqttError_t _IotMqtt_deserializeConnackWrapper( _mqttPacket_t * pConnack )
{
    IotMqttError_t status = IOT_MQTT_BAD_PARAMETER;
    MQTTStatus_t managedMqttStatus = MQTTBadParameter;
    MQTTPacketInfo_t pIncomingPacket;
    bool sessionPresent = false;

    /* Null Check for connack packet. */
    IotMqtt_Assert( pConnack != NULL );

    pIncomingPacket.type = pConnack->type;
    pIncomingPacket.pRemainingData = pConnack->pRemainingData;
    pIncomingPacket.remainingLength = pConnack->remainingLength;

    /* Deserializing Connack packet received from the network. */
    managedMqttStatus = MQTT_DeserializeAck( &pIncomingPacket, &( pConnack->packetIdentifier ), &sessionPresent );
    status = convertReturnCode( managedMqttStatus );
    return status;
}

/*-----------------------------------------------------------*/

/* Deserializer Suback wrapper. */
IotMqttError_t _IotMqtt_deserializeSubackWrapper( _mqttPacket_t * pSuback )
{
    IotMqttError_t status = IOT_MQTT_BAD_PARAMETER;
    MQTTStatus_t managedMqttStatus = MQTTBadParameter;
    MQTTPacketInfo_t pIncomingPacket;
    size_t i = 0;
    uint8_t subscriptionStatus = 0;
    size_t remainingLength = pSuback->remainingLength;
    const uint8_t * pVariableHeader = pSuback->pRemainingData;

    /* Null Check for suback packet. */
    IotMqtt_Assert( pSuback != NULL );

    pIncomingPacket.type = pSuback->type;
    pIncomingPacket.pRemainingData = pSuback->pRemainingData;
    pIncomingPacket.remainingLength = pSuback->remainingLength;

    /* Deserializing SUBACK packet received from the network. */
    managedMqttStatus = MQTT_DeserializeAck( &pIncomingPacket, &( pSuback->packetIdentifier ), NULL );
    status = convertReturnCode( managedMqttStatus );

    /* Remove rejected subscription as the MQTT LTS library do not remove them, it has to handled in shim */
    if( status == IOT_MQTT_SERVER_REFUSED )
    {
        /* Iterate through each status byte in the SUBACK packet. */
        for( i = 0; i < remainingLength - sizeof( uint16_t ); i++ )
        {
            /* Read a single status byte in SUBACK. */
            subscriptionStatus = *( pVariableHeader + sizeof( uint16_t ) + i );

            /* MQTT 3.1.1 defines the following values as status codes. */
            switch( subscriptionStatus )
            {
                case SUBSCRIPTION_ACCEPTED_QOS0:
                case SUBSCRIPTION_ACCEPTED_QOS1:
                case SUBSCRIPTION_ACCEPTED_QOS2:
                    IotLogDebug( "Topic filter %lu accepted, max QoS %hhu.",
                                 ( unsigned long ) i, subscriptionStatus );
                    break;

                case SUBSCRIPTION_REFUSED:
                    IotLogDebug( "Topic filter %lu refused.",
                                 ( unsigned long ) i );

                    /* Remove a rejected subscription from the subscription manager. */
                    _IotMqtt_RemoveSubscriptionByPacket( pSuback->u.pMqttConnection,
                                                         pSuback->packetIdentifier,
                                                         ( int32_t ) i );

                    status = IOT_MQTT_SERVER_REFUSED;

                    break;

                default:
                    IotLogDebug( "Bad SUBSCRIBE status %hhu.",
                                 subscriptionStatus );

                    status = IOT_MQTT_BAD_RESPONSE;

                    break;
            }
        }
    }

    return status;
}

/*-----------------------------------------------------------*/

/* Deserializer Unsuback wrapper. */
IotMqttError_t _IotMqtt_deserializeUnsubackWrapper( _mqttPacket_t * pUnsuback )
{
    IotMqttError_t status = IOT_MQTT_BAD_PARAMETER;
    MQTTStatus_t managedMqttStatus = MQTTBadParameter;
    MQTTPacketInfo_t pIncomingPacket;

    /* Null Check for unsuback packet  */
    IotMqtt_Assert( pUnsuback != NULL );

    pIncomingPacket.type = pUnsuback->type;
    pIncomingPacket.pRemainingData = pUnsuback->pRemainingData;
    pIncomingPacket.remainingLength = pUnsuback->remainingLength;
    /* Deserializing UNSUBACK packet received from the network. */
    managedMqttStatus = MQTT_DeserializeAck( &pIncomingPacket, &( pUnsuback->packetIdentifier ), NULL );
    status = convertReturnCode( managedMqttStatus );

    return status;
}

/*-----------------------------------------------------------*/

/* Deserializer Puback wrapper. */
IotMqttError_t _IotMqtt_deserializePubackWrapper( _mqttPacket_t * pPuback )
{
    IotMqttError_t status = IOT_MQTT_BAD_PARAMETER;
    MQTTStatus_t managedMqttStatus = MQTTBadParameter;
    MQTTPacketInfo_t pIncomingPacket;

    /* Null Check for puback packet. */
    IotMqtt_Assert( pPuback != NULL );

    pIncomingPacket.type = pPuback->type;
    pIncomingPacket.pRemainingData = pPuback->pRemainingData;
    pIncomingPacket.remainingLength = pPuback->remainingLength;
    /* Deserializing PUBACK packet received from the network. */
    managedMqttStatus = MQTT_DeserializeAck( &pIncomingPacket, &( pPuback->packetIdentifier ), NULL );
    status = convertReturnCode( managedMqttStatus );
    return status;
}

/*-----------------------------------------------------------*/

/* Deserializer Ping Response wrapper. */
IotMqttError_t _IotMqtt_deserializePingrespWrapper( _mqttPacket_t * pPingresp )
{
    IotMqttError_t status = IOT_MQTT_BAD_PARAMETER;
    MQTTStatus_t managedMqttStatus = MQTTBadParameter;
    MQTTPacketInfo_t pIncomingPacket;

    /* Null Check for Pingresponse packet. */
    IotMqtt_Assert( pPingresp != NULL );

    pIncomingPacket.type = pPingresp->type;
    pIncomingPacket.pRemainingData = pPingresp->pRemainingData;
    pIncomingPacket.remainingLength = pPingresp->remainingLength;
    /* Deserializing PINGRESP packet received from the network. */
    managedMqttStatus = MQTT_DeserializeAck( &pIncomingPacket, &( pPingresp->packetIdentifier ), NULL );
    status = convertReturnCode( managedMqttStatus );
    return status;
}

/*-----------------------------------------------------------*/

/* Deserializer Publish wrapper. */
IotMqttError_t _IotMqtt_deserializePublishWrapper( _mqttPacket_t * pPublish )
{
    IotMqttError_t status = IOT_MQTT_BAD_PARAMETER;
    MQTTStatus_t managedMqttStatus = MQTTBadParameter;
    MQTTPacketInfo_t pIncomingPacket;
    MQTTPublishInfo_t publishInfo;

    /* Null Check for Publish packet. */
    IotMqtt_Assert( pPublish != NULL );

    pIncomingPacket.type = pPublish->type;
    pIncomingPacket.pRemainingData = pPublish->pRemainingData;
    pIncomingPacket.remainingLength = pPublish->remainingLength;


    /* Deserializing publish packet received from the network. */
    managedMqttStatus = MQTT_DeserializePublish( &pIncomingPacket, &( pPublish->packetIdentifier ), &publishInfo );

    status = convertReturnCode( managedMqttStatus );

    if( status == IOT_MQTT_SUCCESS )
    {
        pPublish->u.pIncomingPublish->u.publish.publishInfo.qos = ( IotMqttQos_t ) publishInfo.qos;
        pPublish->u.pIncomingPublish->u.publish.publishInfo.payloadLength = publishInfo.payloadLength;
        pPublish->u.pIncomingPublish->u.publish.publishInfo.pPayload = publishInfo.pPayload;
        pPublish->u.pIncomingPublish->u.publish.publishInfo.pTopicName = publishInfo.pTopicName;
        pPublish->u.pIncomingPublish->u.publish.publishInfo.topicNameLength = publishInfo.topicNameLength;
        pPublish->u.pIncomingPublish->u.publish.publishInfo.retain = publishInfo.retain;
    }

    return status;
}

/*-----------------------------------------------------------*/

/* Suback Serializer Wrapper. */
IotMqttError_t _IotMqtt_pubackSerializeWrapper( uint16_t packetIdentifier,
                                                uint8_t ** pPubackPacket,
                                                size_t * pPacketSize )
{
    IotMqttError_t status = IOT_MQTT_BAD_PARAMETER;
    MQTTStatus_t managedMqttStatus = MQTTBadParameter;
    MQTTFixedBuffer_t networkBuffer;
    uint8_t packetTypeByte = MQTT_PACKET_TYPE_PUBACK;

    IotMqtt_Assert( pPacketSize != NULL );
    IotMqtt_Assert( pPubackPacket != NULL );

    /* Initializing network buffer. */
    networkBuffer.pBuffer = IotMqtt_MallocMessage( MQTT_PACKET_PUBACK_SIZE );
    networkBuffer.size = MQTT_PACKET_PUBACK_SIZE;
    *pPacketSize = MQTT_PACKET_PUBACK_SIZE;

    /* Serializing puback packet and validating the serialize parameters to be sent on the network. */
    managedMqttStatus = MQTT_SerializeAck( &( networkBuffer ),
                                           packetTypeByte,
                                           packetIdentifier );
    status = convertReturnCode( managedMqttStatus );

    if( status == IOT_MQTT_SUCCESS )
    {
        *pPubackPacket = networkBuffer.pBuffer;
    }

    return status;
}

/*-----------------------------------------------------------*/

IotMqttError_t convertReturnCode( MQTTStatus_t managedMqttStatus )
{
    IotMqttError_t status = IOT_MQTT_SUCCESS;

    switch( managedMqttStatus )
    {
        case MQTTSuccess:
            status = IOT_MQTT_SUCCESS;
            break;

        case MQTTBadParameter:
            status = IOT_MQTT_BAD_PARAMETER;
            break;

        case MQTTNoMemory:
            status = IOT_MQTT_NO_MEMORY;
            break;

        case MQTTSendFailed:
        case MQTTRecvFailed:
            status = IOT_MQTT_NETWORK_ERROR;
            break;

        case MQTTBadResponse:
            status = IOT_MQTT_BAD_RESPONSE;
            break;

        case MQTTServerRefused:
            status = IOT_MQTT_SERVER_REFUSED;
            break;

        case MQTTNoDataAvailable:
        case MQTTKeepAliveTimeout:
            status = IOT_MQTT_TIMEOUT;

        case MQTTIllegalState:
        case MQTTStateCollision:
            status = IOT_MQTT_BAD_RESPONSE;

        default:
            status = IOT_MQTT_SUCCESS;
            break;
    }

    return status;
}

/*-----------------------------------------------------------*/
