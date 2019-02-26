/*
 * Amazon FreeRTOS
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
 *
 * http://aws.amazon.com/freertos
 * http://www.FreeRTOS.org
 */

/**
 * @file aws_mqtt_lib_ble.c
 * @brief MQTT library for BLE.
 */
/* Build using a config header, if provided. */
#ifdef IOT_CONFIG_FILE
    #include IOT_CONFIG_FILE
#endif

/* Standard includes. */
#include <string.h>
#include <stdio.h>

/* FreeRTOS includes. */
#include "FreeRTOS.h"

/* MQTT internal includes. */
#include "iot_ble_config.h"
#include "aws_iot_serializer.h"
#include "private/iot_ble_mqtt_serialize.h"
#include "private/iot_mqtt_internal.h"

#define _INVALID_MQTT_PACKET_TYPE        ( 0xF0 )


#define _IS_VALID_SERIALIZER_RET( ret, pSerializerBuf )                                \
    (  ( ret == AWS_IOT_SERIALIZER_SUCCESS ) ||                                        \
          (  ( !pSerializerBuf ) && ( ret == AWS_IOT_SERIALIZER_BUFFER_TOO_SMALL ) ) )

#define _NUM_CONNECT_PARMAS                   ( 4 )
#define _NUM_DEFAULT_PUBLISH_PARMAS           ( 4 )
#define _NUM_PUBACK_PARMAS                    ( 2 )
#define _NUM_SUBACK_PARAMS                    ( 4 )
#define _NUM_UNSUBACK_PARAMS                  ( 3 )
#define _NUM_DISCONNECT_PARAMS                ( 1 )
/**
 * @brief Guards access to the packet identifier counter.
 *
 * Each packet should have a unique packet identifier. This mutex ensures that only
 * one thread at a time may read the global packet identifer.
 */


/**
 * @brief Generates a monotonically increasing identifier used in  MQTT message
 *
 * @return Identifier for the MQTT message
 */
static uint16_t _nextPacketIdentifier( void );


static inline uint16_t _getNumPublishParams( const IotMqttPublishInfo_t * const pPublish )
{
   return ( pPublish->qos > 0 ) ?  ( _NUM_DEFAULT_PUBLISH_PARMAS + 1 ) : _NUM_DEFAULT_PUBLISH_PARMAS;
}

static AwsIotSerializerError_t _serializeConnect( const IotMqttConnectInfo_t * const pConnectInfo,
                                       const IotMqttPublishInfo_t * const pWillInfo,
                                       uint8_t* const pBuffer,
                                       size_t* const pSize );
static AwsIotSerializerError_t _serializePublish( const IotMqttPublishInfo_t * const pPublishInfo,
                                                  uint8_t * pBuffer,
                                                  size_t  * pSize,
                                                  uint16_t packetIdentifier );
static AwsIotSerializerError_t _serializePubAck( uint16_t packetIdentifier,
                                      uint8_t * pBuffer,
                                      size_t  * pSize );



static AwsIotSerializerError_t _serializeSubscribe( const IotMqttSubscription_t * const pSubscriptionList,
                                               size_t subscriptionCount,
                                               uint8_t * const pBuffer,
                                               size_t * const pSize,
                                               uint16_t packetIdentifier );

static AwsIotSerializerError_t _serializeUnSubscribe( const IotMqttSubscription_t * const pSubscriptionList,
                                               size_t subscriptionCount,
                                               uint8_t * const pBuffer,
                                               size_t * const pSize,
                                               uint16_t packetIdentifier );

static AwsIotSerializerError_t _serializeDisconnect( uint8_t * const pBuffer,
                                                size_t * const pSize );

#if _LIBRARY_LOG_LEVEL > AWS_IOT_LOG_NONE

/**
 * @brief If logging is enabled, define a log configuration that only prints the log
 * string. This is used when printing out details of deserialized MQTT packets.
 */
static const AwsIotLogConfig_t _logHideAll =
{
    .hideLibraryName = true,
    .hideLogLevel    = true,
    .hideTimestring  = true
};
#endif


static AwsIotMutex_t _packetIdentifierMutex;


/* Declaration of snprintf. The header stdio.h is not included because it
 * includes conflicting symbols on some platforms. */
extern int snprintf( char * s,
                     size_t n,
                     const char * format,
                     ... );

/*-----------------------------------------------------------*/

static uint16_t _nextPacketIdentifier( void )
{
    static uint16_t nextPacketIdentifier = 1;
    uint16_t newPacketIdentifier = 0;

    /* Lock the packet identifier mutex so that only one thread may read and
         * modify nextPacketIdentifier. */
     AwsIotMutex_Lock( &_packetIdentifierMutex );

    /* Read the next packet identifier. */
    newPacketIdentifier = nextPacketIdentifier;

    /* The next packet identifier will be greater by 2. This prevents packet
     * identifiers from ever being 0, which is not allowed by MQTT 3.1.1. Packet
     * identifiers will follow the sequence 1,3,5...65535,1,3,5... */
    nextPacketIdentifier = ( uint16_t ) ( nextPacketIdentifier + ( ( uint16_t ) 2 ) );

    /* Unlock the packet identifier mutex. */
    AwsIotMutex_Unlock( &_packetIdentifierMutex );

    return newPacketIdentifier;
}

static AwsIotSerializerError_t _serializeConnect( const IotMqttConnectInfo_t * const pConnectInfo,
                                       const IotMqttPublishInfo_t * const pWillInfo,
                                       uint8_t* const pBuffer,
                                       size_t* const pSize )
{
    AwsIotSerializerError_t error = AWS_IOT_SERIALIZER_SUCCESS;
    AwsIotSerializerEncoderObject_t encoderObj = AWS_IOT_SERIALIZER_ENCODER_CONTAINER_INITIALIZER_STREAM ;
    AwsIotSerializerEncoderObject_t connectMap = AWS_IOT_SERIALIZER_ENCODER_CONTAINER_INITIALIZER_MAP;
    AwsIotSerializerScalarData_t data = { 0 };

    error = IOT_BLE_MESG_ENCODER.init( &encoderObj, pBuffer, *pSize );
    if( error == AWS_IOT_SERIALIZER_SUCCESS )
    {

        error = IOT_BLE_MESG_ENCODER.openContainer(
                &encoderObj,
                &connectMap,
                _NUM_CONNECT_PARMAS );
    }

    if( _IS_VALID_SERIALIZER_RET( error, pBuffer ) )
    {
        data.type = AWS_IOT_SERIALIZER_SCALAR_SIGNED_INT;
        data.value.signedInt = IOT_BLE_MQTT_MSG_TYPE_CONNECT;
        error = IOT_BLE_MESG_ENCODER.appendKeyValue( &connectMap, IOT_BLE_MQTT_MSG_TYPE, data );
    }

    if( _IS_VALID_SERIALIZER_RET( error, pBuffer ) )
    {
        data.type = AWS_IOT_SERIALIZER_SCALAR_TEXT_STRING;
        data.value.pString = ( uint8_t * ) pConnectInfo->pClientIdentifier;
        data.value.stringLength = pConnectInfo->clientIdentifierLength;
        error = IOT_BLE_MESG_ENCODER.appendKeyValue( &connectMap, IOT_BLE_MQTT_CLIENT_ID, data );
    }
    if( _IS_VALID_SERIALIZER_RET( error, pBuffer ) )
    {
        data.type = AWS_IOT_SERIALIZER_SCALAR_TEXT_STRING;
        data.value.pString = ( uint8_t * ) clientcredentialMQTT_BROKER_ENDPOINT;
        data.value.stringLength = strlen( clientcredentialMQTT_BROKER_ENDPOINT );
        error = IOT_BLE_MESG_ENCODER.appendKeyValue( &connectMap, IOT_BLE_MQTT_BROKER_EP, data );
    }
    if( _IS_VALID_SERIALIZER_RET( error, pBuffer ) )
    {
        data.type = AWS_IOT_SERIALIZER_SCALAR_BOOL;
        data.value.booleanValue = pConnectInfo->cleanSession;
        error = IOT_BLE_MESG_ENCODER.appendKeyValue( &connectMap, IOT_BLE_MQTT_CLEAN_SESSION, data );
    }
    if( _IS_VALID_SERIALIZER_RET( error, pBuffer ) )
    {
        error = IOT_BLE_MESG_ENCODER.closeContainer( &encoderObj, &connectMap );
    }

    if( _IS_VALID_SERIALIZER_RET( error, pBuffer ) )
    {
        if( pBuffer == NULL )
        {
            *pSize = IOT_BLE_MESG_ENCODER.getExtraBufferSizeNeeded( &encoderObj );
        }
        else
        {
            *pSize = IOT_BLE_MESG_ENCODER.getEncodedSize( &encoderObj, pBuffer );
        }

        IOT_BLE_MESG_ENCODER.destroy( &encoderObj );
        error = AWS_IOT_SERIALIZER_SUCCESS;

    }

    return error;
}

static AwsIotSerializerError_t _serializePublish( const IotMqttPublishInfo_t * const pPublishInfo,
                                                  uint8_t * pBuffer,
                                                  size_t  * pSize,
                                                  uint16_t packetIdentifier )
{
    AwsIotSerializerError_t error = AWS_IOT_SERIALIZER_SUCCESS;
    AwsIotSerializerEncoderObject_t encoderObj = AWS_IOT_SERIALIZER_ENCODER_CONTAINER_INITIALIZER_STREAM ;
    AwsIotSerializerEncoderObject_t publishMap = AWS_IOT_SERIALIZER_ENCODER_CONTAINER_INITIALIZER_MAP;
    AwsIotSerializerScalarData_t data = { 0 };
    uint16_t numPublishParams = _getNumPublishParams( pPublishInfo );

    error = IOT_BLE_MESG_ENCODER.init( &encoderObj, pBuffer, *pSize );

    if( error == AWS_IOT_SERIALIZER_SUCCESS )
    {


        error = IOT_BLE_MESG_ENCODER.openContainer(
                &encoderObj,
                &publishMap,
                numPublishParams );
    }


    if( _IS_VALID_SERIALIZER_RET( error, pBuffer ) )
    {
        data.type = AWS_IOT_SERIALIZER_SCALAR_SIGNED_INT;
        data.value.signedInt = IOT_BLE_MQTT_MSG_TYPE_PUBLISH;
        error = IOT_BLE_MESG_ENCODER.appendKeyValue( &publishMap, IOT_BLE_MQTT_MSG_TYPE, data );
    }

    if( _IS_VALID_SERIALIZER_RET( error, pBuffer ) )
    {
        data.type = AWS_IOT_SERIALIZER_SCALAR_TEXT_STRING;
        data.value.pString = ( uint8_t * ) pPublishInfo->pTopicName;
        data.value.stringLength = pPublishInfo->topicNameLength;
        error = IOT_BLE_MESG_ENCODER.appendKeyValue( &publishMap, IOT_BLE_MQTT_TOPIC, data );
    }

    if( _IS_VALID_SERIALIZER_RET( error, pBuffer ) )
    {

        data.type = AWS_IOT_SERIALIZER_SCALAR_SIGNED_INT;
        data.value.signedInt = pPublishInfo->qos;
        error = IOT_BLE_MESG_ENCODER.appendKeyValue( &publishMap, IOT_BLE_MQTT_QOS, data );
    }

    if( _IS_VALID_SERIALIZER_RET( error, pBuffer ) )
    {
        data.type = AWS_IOT_SERIALIZER_SCALAR_BYTE_STRING;
        data.value.pString = ( uint8_t * ) pPublishInfo->pPayload;
        data.value.stringLength = pPublishInfo->payloadLength;
        error = IOT_BLE_MESG_ENCODER.appendKeyValue( &publishMap, IOT_BLE_MQTT_PAYLOAD, data );
    }


    if( _IS_VALID_SERIALIZER_RET( error, pBuffer ) )
    {
        if( pPublishInfo->qos != 0 )
        {
            data.type = AWS_IOT_SERIALIZER_SCALAR_SIGNED_INT;
            data.value.signedInt = packetIdentifier;
            error = IOT_BLE_MESG_ENCODER.appendKeyValue( &publishMap, IOT_BLE_MQTT_MESSAGE_ID, data );

        }
    }

    if( _IS_VALID_SERIALIZER_RET( error, pBuffer ) )
    {

        error = IOT_BLE_MESG_ENCODER.closeContainer( &encoderObj, &publishMap );
    }

    if( _IS_VALID_SERIALIZER_RET( error, pBuffer ) )
    {
        if( pBuffer == NULL )
        {
            *pSize = IOT_BLE_MESG_ENCODER.getExtraBufferSizeNeeded( &encoderObj );

        }
        else
        {
            *pSize = IOT_BLE_MESG_ENCODER.getEncodedSize( &encoderObj, pBuffer );
        }
        IOT_BLE_MESG_ENCODER.destroy( &encoderObj );
        error = AWS_IOT_SERIALIZER_SUCCESS;
    }


    return error;
}

static AwsIotSerializerError_t _serializePubAck( uint16_t packetIdentifier,
                                      uint8_t * pBuffer,
                                      size_t  * pSize )

{
    AwsIotSerializerError_t error = AWS_IOT_SERIALIZER_SUCCESS;
    AwsIotSerializerEncoderObject_t encoderObj = AWS_IOT_SERIALIZER_ENCODER_CONTAINER_INITIALIZER_STREAM ;
    AwsIotSerializerEncoderObject_t pubAckMap = AWS_IOT_SERIALIZER_ENCODER_CONTAINER_INITIALIZER_MAP;
    AwsIotSerializerScalarData_t data = { 0 };

    error = IOT_BLE_MESG_ENCODER.init( &encoderObj, pBuffer, *pSize );

    if( error == AWS_IOT_SERIALIZER_SUCCESS )
    {

        error = IOT_BLE_MESG_ENCODER.openContainer(
                &encoderObj,
                &pubAckMap,
                _NUM_PUBACK_PARMAS );
    }

    if( _IS_VALID_SERIALIZER_RET( error, pBuffer ) )
    {
        data.type = AWS_IOT_SERIALIZER_SCALAR_SIGNED_INT;
        data.value.signedInt = IOT_BLE_MQTT_MSG_TYPE_PUBACK;
        error = IOT_BLE_MESG_ENCODER.appendKeyValue( &pubAckMap, IOT_BLE_MQTT_MSG_TYPE, data );
    }

    if( _IS_VALID_SERIALIZER_RET( error, pBuffer ) )
    {
        data.type = AWS_IOT_SERIALIZER_SCALAR_SIGNED_INT;
        data.value.signedInt = packetIdentifier;
        error = IOT_BLE_MESG_ENCODER.appendKeyValue( &pubAckMap, IOT_BLE_MQTT_MESSAGE_ID, data );
    }
    if( _IS_VALID_SERIALIZER_RET( error, pBuffer ) )
    {
        error = IOT_BLE_MESG_ENCODER.closeContainer( &encoderObj, &pubAckMap );
    }

    if( _IS_VALID_SERIALIZER_RET( error, pBuffer ) )
    {
        if( pBuffer == NULL )
        {
            *pSize = IOT_BLE_MESG_ENCODER.getExtraBufferSizeNeeded( &encoderObj );
        }
        else
        {
            *pSize = IOT_BLE_MESG_ENCODER.getEncodedSize( &encoderObj, pBuffer );
        }
        IOT_BLE_MESG_ENCODER.destroy( &encoderObj );
        error = AWS_IOT_SERIALIZER_SUCCESS;
    }

    return error;
}


static AwsIotSerializerError_t _serializeSubscribe( const IotMqttSubscription_t * const pSubscriptionList,
                                               size_t subscriptionCount,
                                               uint8_t * const pBuffer,
                                               size_t * const pSize,
                                               uint16_t packetIdentifier )
{
    AwsIotSerializerError_t error = AWS_IOT_SERIALIZER_SUCCESS;
    AwsIotSerializerEncoderObject_t encoderObj = AWS_IOT_SERIALIZER_ENCODER_CONTAINER_INITIALIZER_STREAM ;
    AwsIotSerializerEncoderObject_t subscribeMap = AWS_IOT_SERIALIZER_ENCODER_CONTAINER_INITIALIZER_MAP;
    AwsIotSerializerEncoderObject_t subscriptionArray = AWS_IOT_SERIALIZER_ENCODER_CONTAINER_INITIALIZER_ARRAY;
    AwsIotSerializerScalarData_t data = { 0 };
    uint16_t idx;

    error = IOT_BLE_MESG_ENCODER.init( &encoderObj, pBuffer, *pSize );

    if( error == AWS_IOT_SERIALIZER_SUCCESS )
    {
        error = IOT_BLE_MESG_ENCODER.openContainer(
                &encoderObj,
                &subscribeMap,
                _NUM_SUBACK_PARAMS );
    }

    if( _IS_VALID_SERIALIZER_RET( error, pBuffer ) )
    {
        data.type = AWS_IOT_SERIALIZER_SCALAR_SIGNED_INT;
        data.value.signedInt = IOT_BLE_MQTT_MSG_TYPE_SUBSCRIBE;
        error = IOT_BLE_MESG_ENCODER.appendKeyValue( &subscribeMap, IOT_BLE_MQTT_MSG_TYPE, data );
    }

    if( _IS_VALID_SERIALIZER_RET( error, pBuffer ) )
    {

        error = IOT_BLE_MESG_ENCODER.openContainerWithKey(
                &subscribeMap,
                IOT_BLE_MQTT_TOPIC_LIST,
                &subscriptionArray,
                subscriptionCount );
    }

    for( idx = 0; idx < subscriptionCount; idx++ )
    {
        if( _IS_VALID_SERIALIZER_RET( error, pBuffer ) )
        {
            data.type = AWS_IOT_SERIALIZER_SCALAR_TEXT_STRING;
            data.value.pString = ( uint8_t * ) pSubscriptionList[ idx ].pTopicFilter;
            data.value.stringLength = pSubscriptionList[ idx ].topicFilterLength;
            error = IOT_BLE_MESG_ENCODER.append( &subscriptionArray, data );
        }
        else
        {
            break;
        }
    }

    if( _IS_VALID_SERIALIZER_RET( error, pBuffer ) )
    {
        error = IOT_BLE_MESG_ENCODER.closeContainer( &subscribeMap, &subscriptionArray );
    }

    if( _IS_VALID_SERIALIZER_RET( error, pBuffer ) )
    {

        error = IOT_BLE_MESG_ENCODER.openContainerWithKey(
                &subscribeMap,
                IOT_BLE_MQTT_QOS_LIST,
                &subscriptionArray,
                subscriptionCount );
    }


    for( idx = 0; idx < subscriptionCount; idx++ )
    {
        if( _IS_VALID_SERIALIZER_RET( error, pBuffer ) )
        {

            data.type = AWS_IOT_SERIALIZER_SCALAR_SIGNED_INT;
            data.value.signedInt = pSubscriptionList[ idx ].qos;
            error = IOT_BLE_MESG_ENCODER.append( &subscriptionArray, data );
        }
        else
        {
            break;
        }
    }

    if( _IS_VALID_SERIALIZER_RET( error, pBuffer ) )
    {
        error = IOT_BLE_MESG_ENCODER.closeContainer( &subscribeMap, &subscriptionArray );
    }

    if( _IS_VALID_SERIALIZER_RET( error, pBuffer ) )
    {

        data.type = AWS_IOT_SERIALIZER_SCALAR_SIGNED_INT;
        data.value.signedInt = packetIdentifier;
        error = IOT_BLE_MESG_ENCODER.appendKeyValue( &subscribeMap, IOT_BLE_MQTT_MESSAGE_ID, data );
    }

    if( _IS_VALID_SERIALIZER_RET( error, pBuffer ) )
    {
        error = IOT_BLE_MESG_ENCODER.closeContainer( &encoderObj, &subscribeMap );
    }

    if( _IS_VALID_SERIALIZER_RET( error, pBuffer ) )
    {
        if( pBuffer == NULL )
        {
            *pSize = IOT_BLE_MESG_ENCODER.getExtraBufferSizeNeeded( &encoderObj );
        }
        else
        {
            *pSize = IOT_BLE_MESG_ENCODER.getEncodedSize( &encoderObj, pBuffer );
        }

        IOT_BLE_MESG_ENCODER.destroy( &encoderObj );
        error = AWS_IOT_SERIALIZER_SUCCESS;
    }
    return error;
}

static AwsIotSerializerError_t _serializeUnSubscribe( const IotMqttSubscription_t * const pSubscriptionList,
                                               size_t subscriptionCount,
                                               uint8_t * const pBuffer,
                                               size_t * const pSize,
                                               uint16_t packetIdentifier )
{
    AwsIotSerializerError_t error = AWS_IOT_SERIALIZER_SUCCESS;
    AwsIotSerializerEncoderObject_t encoderObj = AWS_IOT_SERIALIZER_ENCODER_CONTAINER_INITIALIZER_STREAM;
    AwsIotSerializerEncoderObject_t subscribeMap = AWS_IOT_SERIALIZER_ENCODER_CONTAINER_INITIALIZER_MAP;
    AwsIotSerializerEncoderObject_t subscriptionArray = AWS_IOT_SERIALIZER_ENCODER_CONTAINER_INITIALIZER_ARRAY;
    AwsIotSerializerScalarData_t data = { 0 };
    uint16_t idx;

    error = IOT_BLE_MESG_ENCODER.init( &encoderObj, pBuffer, *pSize );

    if( error == AWS_IOT_SERIALIZER_SUCCESS )
    {

        error = IOT_BLE_MESG_ENCODER.openContainer(
                &encoderObj,
                &subscribeMap,
                _NUM_UNSUBACK_PARAMS );
    }


    if( _IS_VALID_SERIALIZER_RET( error, pBuffer ) )
    {
        data.type = AWS_IOT_SERIALIZER_SCALAR_SIGNED_INT;
        data.value.signedInt = IOT_BLE_MQTT_MSG_TYPE_UNSUBSCRIBE;
        error = IOT_BLE_MESG_ENCODER.appendKeyValue( &subscribeMap, IOT_BLE_MQTT_MSG_TYPE, data );
    }

    if( _IS_VALID_SERIALIZER_RET( error, pBuffer ) )
    {
        error = IOT_BLE_MESG_ENCODER.openContainerWithKey (
                &subscribeMap,
                IOT_BLE_MQTT_TOPIC_LIST,
                &subscriptionArray,
                subscriptionCount );
    }

    for( idx = 0; idx < subscriptionCount; idx++ )
    {
        if( _IS_VALID_SERIALIZER_RET( error, pBuffer ) )
        {
            data.type = AWS_IOT_SERIALIZER_SCALAR_TEXT_STRING;
            data.value.pString = ( uint8_t * ) pSubscriptionList[ idx ].pTopicFilter;
            data.value.stringLength = pSubscriptionList[ idx ].topicFilterLength;
            error = IOT_BLE_MESG_ENCODER.append( &subscriptionArray, data );
        }
        else
        {
            break;
        }
    }

    if( _IS_VALID_SERIALIZER_RET( error, pBuffer ) )
    {
        error = IOT_BLE_MESG_ENCODER.closeContainer( &subscribeMap, &subscriptionArray );
    }

    if( _IS_VALID_SERIALIZER_RET( error, pBuffer ) )
    {
        data.type = AWS_IOT_SERIALIZER_SCALAR_SIGNED_INT;
        data.value.signedInt = packetIdentifier;
        error = IOT_BLE_MESG_ENCODER.appendKeyValue( &subscribeMap, IOT_BLE_MQTT_MESSAGE_ID, data );
    }

    if( _IS_VALID_SERIALIZER_RET( error, pBuffer ) )
    {
        error = IOT_BLE_MESG_ENCODER.closeContainer( &encoderObj, &subscribeMap );
    }

    if( _IS_VALID_SERIALIZER_RET( error, pBuffer ) )
    {
        if( pBuffer == NULL )
        {
            *pSize = IOT_BLE_MESG_ENCODER.getExtraBufferSizeNeeded( &encoderObj );

        }
        else
        {
            *pSize = IOT_BLE_MESG_ENCODER.getEncodedSize( &encoderObj, pBuffer );
        }
        IOT_BLE_MESG_ENCODER.destroy( &encoderObj );
        error = AWS_IOT_SERIALIZER_SUCCESS;
    }

    return error;
}

static AwsIotSerializerError_t _serializeDisconnect( uint8_t * const pBuffer,
                                                size_t * const pSize )
{
    AwsIotSerializerError_t error = AWS_IOT_SERIALIZER_SUCCESS;
    AwsIotSerializerEncoderObject_t encoderObj = AWS_IOT_SERIALIZER_ENCODER_CONTAINER_INITIALIZER_STREAM;
    AwsIotSerializerEncoderObject_t disconnectMap = AWS_IOT_SERIALIZER_ENCODER_CONTAINER_INITIALIZER_MAP;
    AwsIotSerializerScalarData_t data = { 0 };

    error = IOT_BLE_MESG_ENCODER.init( &encoderObj, pBuffer, *pSize );

    if( error == AWS_IOT_SERIALIZER_SUCCESS )
    {
        error = IOT_BLE_MESG_ENCODER.openContainer(
                &encoderObj,
                &disconnectMap,
                _NUM_DISCONNECT_PARAMS );
    }

    if( _IS_VALID_SERIALIZER_RET( error, pBuffer ) )
    {
        data.type = AWS_IOT_SERIALIZER_SCALAR_SIGNED_INT;
        data.value.signedInt = IOT_BLE_MQTT_MSG_TYPE_DISCONNECT;
        error = IOT_BLE_MESG_ENCODER.appendKeyValue( &disconnectMap, IOT_BLE_MQTT_MSG_TYPE, data );
    }

    if( _IS_VALID_SERIALIZER_RET( error, pBuffer ) )
    {
        error = IOT_BLE_MESG_ENCODER.closeContainer( &encoderObj, &disconnectMap );
    }

    if( _IS_VALID_SERIALIZER_RET( error, pBuffer ) )
    {
        if( pBuffer == NULL )
        {
            *pSize = IOT_BLE_MESG_ENCODER.getExtraBufferSizeNeeded( &encoderObj );
        }
        else
        {
            *pSize = IOT_BLE_MESG_ENCODER.getEncodedSize( &encoderObj, pBuffer );
        }
        IOT_BLE_MESG_ENCODER.destroy( &encoderObj );
        error = AWS_IOT_SERIALIZER_SUCCESS;
    }

    return error;
}


bool IotBleMqtt_InitSerialize( void )
{
	/* Create the packet identifier mutex. */
	return IotMutex_Create( &_packetIdentifierMutex );
}

void IotBleMqtt_CleanupSerialize( void )
{
	/* Destroy the packet identifier mutex */
	AwsIotMutex_Destroy( &_packetIdentifierMutex );
}

IotMqttError_t IotBleMqtt_SerializeConnect( const IotMqttConnectInfo_t * const pConnectInfo,
                                                           const IotMqttPublishInfo_t * const pWillInfo,
                                                           uint8_t ** const pConnectPacket,
                                                           size_t * const pPacketSize )
{
	uint8_t * pBuffer = NULL;
	size_t bufLen = 0;
	AwsIotSerializerError_t error;
	IotMqttError_t ret = AWS_IOT_MQTT_SUCCESS;


	error = _serializeConnect( pConnectInfo, pWillInfo, NULL, &bufLen );
	if( error != AWS_IOT_SERIALIZER_SUCCESS )
	{
	    AwsIotLogError( "Failed to find length of serialized CONNECT message, error = %d", error );
	    ret = AWS_IOT_MQTT_BAD_PARAMETER;
	}

	if( ret == AWS_IOT_MQTT_SUCCESS )
	{

	    pBuffer = AwsIotMqtt_MallocMessage( bufLen );

	    /* If Memory cannot be allocated log an error and return */
	    if( pBuffer == NULL )
	    {
	        AwsIotLogError( "Failed to allocate memory for CONNECT packet." );
	        ret =  AWS_IOT_MQTT_NO_MEMORY;
	    }
	}

	if( ret == AWS_IOT_MQTT_SUCCESS )
	{
	    error = _serializeConnect( pConnectInfo, pWillInfo, pBuffer, &bufLen );
	    if( error != AWS_IOT_SERIALIZER_SUCCESS )
	    {
	        AwsIotLogError( "Failed to serialize CONNECT message, error = %d", error );
	        ret = AWS_IOT_MQTT_BAD_PARAMETER;
	    }
	}

	if( ret == AWS_IOT_MQTT_SUCCESS )
	{
	    *pConnectPacket = pBuffer;
	    *pPacketSize = bufLen;
	}
	else
	{
	    *pConnectPacket = NULL;
	    *pPacketSize = 0;
	    if( pBuffer != NULL )
	    {
	        AwsIotMqtt_FreeMessage( pBuffer );
	    }
	}

    return ret;
}

IotMqttError_t IotBleMqtt_DeserializeConnack( const uint8_t * const pConnackStart,
                                                         size_t dataLength,
                                                         size_t * const pBytesProcessed )
{

    AwsIotSerializerDecoderObject_t decoderObj = { 0 }, decoderValue = { 0 };
    AwsIotSerializerError_t error;
    IotMqttError_t ret = AWS_IOT_MQTT_SUCCESS;
    int64_t respCode = 0L;

    ( *pBytesProcessed ) = 0;

    error = IOT_BLE_MESG_DECODER.init( &decoderObj, ( uint8_t * ) pConnackStart, dataLength );
    if( ( error != AWS_IOT_SERIALIZER_SUCCESS )
            || ( decoderObj.type != AWS_IOT_SERIALIZER_CONTAINER_MAP ) )
    {
        AwsIotLogError( "Malformed CONNACK, decoding the packet failed, decoder error = %d, type: %d", error, decoderObj.type );
        ret = AWS_IOT_MQTT_BAD_RESPONSE;
    }


    if( ret == AWS_IOT_MQTT_SUCCESS )
    {

        error = IOT_BLE_MESG_DECODER.find( &decoderObj, IOT_BLE_MQTT_STATUS, &decoderValue );
        if ( ( error != AWS_IOT_SERIALIZER_SUCCESS ) ||
                ( decoderValue.type != AWS_IOT_SERIALIZER_SCALAR_SIGNED_INT ) )
        {
            AwsIotLogError( "Invalid CONNACK, response code decode failed, error = %d, decoded value type = %d", error, decoderValue.type );
            ret = AWS_IOT_MQTT_BAD_RESPONSE;
        }
        else
        {

            respCode =  decoderValue.value.signedInt;
            if( ( respCode != IOT_BLE_MQTT_STATUS_CONNECTING )
                    && ( respCode != IOT_BLE_MQTT_STATUS_CONNECTED ) )
            {
                ret = AWS_IOT_MQTT_SERVER_REFUSED;
            }

            ( *pBytesProcessed ) = dataLength;
        }
    }

    IOT_BLE_MESG_DECODER.destroy( &decoderObj );

    return ret;
}

IotMqttError_t IotBleMqtt_SerializePublish( const IotMqttPublishInfo_t * const pPublishInfo,
                                                  uint8_t ** const pPublishPacket,
                                                  size_t * const pPacketSize,
                                                  uint16_t * const pPacketIdentifier )
{

    uint8_t * pBuffer = NULL;
    size_t bufLen = 0;
    uint16_t usPacketIdentifier = 0;
    AwsIotSerializerError_t error;
    IotMqttError_t ret = AWS_IOT_MQTT_SUCCESS;

    if( pPublishInfo->QoS != 0 )
    {
        usPacketIdentifier = _nextPacketIdentifier();
    }

    error = _serializePublish( pPublishInfo, NULL, &bufLen, usPacketIdentifier );
    if( error != AWS_IOT_SERIALIZER_SUCCESS  )
    {
        AwsIotLogError( "Failed to find size of serialized PUBLISH message, error = %d", error );
        ret = AWS_IOT_MQTT_BAD_PARAMETER;
    }

    if( ret == AWS_IOT_MQTT_SUCCESS )
    {

        pBuffer = AwsIotMqtt_MallocMessage( bufLen );
        /* If Memory cannot be allocated log an error and return */
        if( pBuffer == NULL )
        {
            AwsIotLogError( "Failed to allocate memory for PUBLISH packet." );
            ret =  AWS_IOT_MQTT_NO_MEMORY;
        }
    }

    if( ret == AWS_IOT_MQTT_SUCCESS )
    {

        error = _serializePublish( pPublishInfo, pBuffer, &bufLen, usPacketIdentifier );
        if( error != AWS_IOT_SERIALIZER_SUCCESS )
        {
            AwsIotLogError( "Failed to serialize PUBLISH message, error = %d", error );
            ret = AWS_IOT_MQTT_BAD_PARAMETER;
        }
    }

    if( ret == AWS_IOT_MQTT_SUCCESS )
    {
        *pPublishPacket = pBuffer;
        *pPacketSize = bufLen;
        *pPacketIdentifier = usPacketIdentifier;
    }
    else
    {
        if( pBuffer != NULL )
        {
            AwsIotMqtt_FreeMessage( pBuffer );
        }
        *pPublishPacket = NULL;
        *pPacketSize = 0;
    }

    return ret;
}

void IotBleMqtt_PublishSetDup( bool awsIotMqttMode, uint8_t * const pPublishPacket, uint16_t * const pNewPacketIdentifier )
{
	/** TODO: Currently DUP flag is not supported by BLE SDKs **/
}

IotMqttError_t IotBleMqtt_DeserializePublish( const uint8_t * const pPublishStart,
                                                         size_t dataLength,
                                                         IotMqttPublishInfo_t * const pOutput,
                                                         uint16_t * const pPacketIdentifier,
                                                         size_t * const pBytesProcessed )
{

    AwsIotSerializerDecoderObject_t decoderObj = { 0 }, decoderValue = { 0 };
    AwsIotSerializerError_t xSerializerRet;
    IotMqttError_t ret = AWS_IOT_MQTT_SUCCESS;

    xSerializerRet = IOT_BLE_MESG_DECODER.init( &decoderObj, ( uint8_t * ) pPublishStart, dataLength );
    if ( (xSerializerRet != AWS_IOT_SERIALIZER_SUCCESS ) ||
            ( decoderObj.type != AWS_IOT_SERIALIZER_CONTAINER_MAP ) )
    {

        AwsIotLogError( "Decoding PUBLISH packet failed, decoder error = %d, object type = %d", xSerializerRet, decoderObj.type );
        ret = AWS_IOT_MQTT_BAD_RESPONSE;
    }

    if( ret == AWS_IOT_MQTT_SUCCESS )
    {
        xSerializerRet = IOT_BLE_MESG_DECODER.find( &decoderObj, IOT_BLE_MQTT_QOS, &decoderValue );
        if ( ( xSerializerRet != AWS_IOT_SERIALIZER_SUCCESS ) ||
                 ( decoderValue.type != AWS_IOT_SERIALIZER_SCALAR_SIGNED_INT ) )
        {
            AwsIotLogError( "QOS Value decode failed, error = %d, decoded value type = %d", xSerializerRet, decoderValue.type );
            ret = AWS_IOT_MQTT_BAD_RESPONSE;
        }
        else
        {
            pOutput->QoS = decoderValue.value.signedInt;
        }
    }

    if( ret == AWS_IOT_MQTT_SUCCESS )
    {
        decoderValue.value.pString = NULL;
        decoderValue.value.stringLength = 0;
        xSerializerRet = IOT_BLE_MESG_DECODER.find( &decoderObj, IOT_BLE_MQTT_TOPIC, &decoderValue );

        if( ( xSerializerRet != AWS_IOT_SERIALIZER_SUCCESS ) ||
                ( decoderValue.type != AWS_IOT_SERIALIZER_SCALAR_TEXT_STRING ) )
        {
            AwsIotLogError( "Topic value decode failed, error = %d", xSerializerRet );
            ret = AWS_IOT_MQTT_BAD_RESPONSE;
        }
        else
        {
            pOutput->pTopicName = ( const char* ) decoderValue.value.pString;
            pOutput->topicNameLength = decoderValue.value.stringLength;
        }
    }

    if( ret == AWS_IOT_MQTT_SUCCESS )
    {
        decoderValue.value.pString = NULL;
        decoderValue.value.stringLength = 0;
        xSerializerRet = IOT_BLE_MESG_DECODER.find( &decoderObj, IOT_BLE_MQTT_PAYLOAD, &decoderValue );

        if( ( xSerializerRet != AWS_IOT_SERIALIZER_SUCCESS ) ||
                ( decoderValue.type != AWS_IOT_SERIALIZER_SCALAR_BYTE_STRING ) )
        {
            AwsIotLogError( "Payload value decode failed, error = %d", xSerializerRet );
            ret = AWS_IOT_MQTT_BAD_RESPONSE;
        }
        else
        {
            pOutput->pPayload = ( const char* ) decoderValue.value.pString;
            pOutput->payloadLength = decoderValue.value.stringLength;
        }
    }

    if( ret == AWS_IOT_MQTT_SUCCESS )
    {
        if( pOutput->QoS != 0 )
        {
            xSerializerRet = IOT_BLE_MESG_DECODER.find( &decoderObj, IOT_BLE_MQTT_MESSAGE_ID, &decoderValue );
            if ( ( xSerializerRet != AWS_IOT_SERIALIZER_SUCCESS ) ||
                    ( decoderValue.type != AWS_IOT_SERIALIZER_SCALAR_SIGNED_INT ) )
            {
                AwsIotLogError( "Message identifier decode failed, error = %d, decoded value type = %d", xSerializerRet, decoderValue.type );
                ret = AWS_IOT_MQTT_BAD_RESPONSE;
            }
            else
            {
                *pPacketIdentifier = ( uint16_t ) decoderValue.value.signedInt;
            }
        }
    }

    if( ret == AWS_IOT_MQTT_SUCCESS  )
    {
        *pBytesProcessed = dataLength;
        pOutput->retain = false;
    }
    else
    {
        *pBytesProcessed = 0;
    }

    IOT_BLE_MESG_DECODER.destroy( &decoderObj );

    return ret;
}

IotMqttError_t IotBleMqtt_SerializePuback( uint16_t packetIdentifier,
                                                      uint8_t ** const pPubackPacket,
                                                      size_t * const pPacketSize )
{
    uint8_t * pBuffer = NULL;
    size_t bufLen = 0;
    AwsIotSerializerError_t error;
    IotMqttError_t ret = AWS_IOT_MQTT_SUCCESS;

    error = _serializePubAck( packetIdentifier, NULL, &bufLen );

    if( error != AWS_IOT_SERIALIZER_SUCCESS )
    {
        AwsIotLogError( "Failed to find size of serialized PUBACK message, error = %d", error );
        ret = AWS_IOT_MQTT_BAD_PARAMETER;
    }


    if( ret == AWS_IOT_MQTT_SUCCESS )
    {
        pBuffer = AwsIotMqtt_MallocMessage( bufLen );

        /* If Memory cannot be allocated log an error and return */
        if( pBuffer == NULL )
        {
            AwsIotLogError( "Failed to allocate memory for PUBACK packet, packet identifier = %d", packetIdentifier );
            ret = AWS_IOT_MQTT_NO_MEMORY;
        }
    }


    if( ret == AWS_IOT_MQTT_SUCCESS )
    {
        error = _serializePubAck( packetIdentifier, pBuffer, &bufLen );

        if( error != AWS_IOT_SERIALIZER_SUCCESS )
        {
            AwsIotLogError( "Failed to find size of serialized PUBACK message, error = %d", error );
            ret = AWS_IOT_MQTT_BAD_PARAMETER;
        }
    }


    if( ret == AWS_IOT_MQTT_SUCCESS )
    {
        *pPubackPacket = pBuffer;
        *pPacketSize = bufLen;
    }
    else
    {
        if( pBuffer != NULL )
        {
            AwsIotMqtt_FreeMessage( pBuffer );
        }

        *pPubackPacket = NULL;
        *pPacketSize = 0;
    }

	return ret;

}

IotMqttError_t IotBleMqtt_DeserializePuback( const uint8_t * const pPubackStart,
                                                        size_t dataLength,
                                                        uint16_t * const pPacketIdentifier,
                                                        size_t * const pBytesProcessed )
{

    AwsIotSerializerDecoderObject_t decoderObj = { 0 }, decoderValue = { 0 };
    AwsIotSerializerError_t error;
    IotMqttError_t ret = AWS_IOT_MQTT_SUCCESS;

    error = IOT_BLE_MESG_DECODER.init( &decoderObj, ( uint8_t * ) pPubackStart, dataLength );

    if ( ( error != AWS_IOT_SERIALIZER_SUCCESS )
            || ( decoderObj.type != AWS_IOT_SERIALIZER_CONTAINER_MAP ) )
    {
        AwsIotLogError( "Malformed PUBACK, decoding the packet failed, decoder error = %d", error );
        ret = AWS_IOT_MQTT_BAD_RESPONSE;
    }


    if( ret == AWS_IOT_MQTT_SUCCESS )
    {

        error = IOT_BLE_MESG_DECODER.find( &decoderObj, IOT_BLE_MQTT_MESSAGE_ID, &decoderValue );

        if ( ( error != AWS_IOT_SERIALIZER_SUCCESS ) ||
                ( decoderValue.type != AWS_IOT_SERIALIZER_SCALAR_SIGNED_INT ) )
        {
            AwsIotLogError( "Message ID decode failed, error = %d, decoded value type = %d", error, decoderValue.type );
            ret = AWS_IOT_MQTT_BAD_RESPONSE;
        }
        else
        {
            *pPacketIdentifier = ( uint16_t ) decoderValue.value.signedInt;
        }
    }

    if( ret == AWS_IOT_MQTT_SUCCESS )
    {
        ( *pBytesProcessed ) = dataLength;
    }
    else
    {
        ( *pBytesProcessed )  = 0;
    }

    IOT_BLE_MESG_DECODER.destroy( &decoderObj );

    return AWS_IOT_MQTT_SUCCESS;

}

IotMqttError_t IotBleMqtt_SerializeSubscribe( const IotMqttSubscription_t * const pSubscriptionList,
                                                         size_t subscriptionCount,
                                                         uint8_t ** const pSubscribePacket,
                                                         size_t * const pPacketSize,
                                                         uint16_t * const pPacketIdentifier )
{
    uint8_t * pBuffer = NULL;
    size_t bufLen = 0;
    uint16_t usPacketIdentifier = 0;
    AwsIotSerializerError_t error;
    IotMqttError_t ret = AWS_IOT_MQTT_SUCCESS;

    usPacketIdentifier = _nextPacketIdentifier();

    error = _serializeSubscribe( pSubscriptionList, subscriptionCount, NULL, &bufLen, usPacketIdentifier );
    if( error != AWS_IOT_SERIALIZER_SUCCESS )
    {
        AwsIotLogError( "Failed to find serialized length of SUBSCRIBE message, error = %d", error );
        ret = AWS_IOT_MQTT_BAD_PARAMETER;
    }

    if( ret == AWS_IOT_MQTT_SUCCESS )
    {
        pBuffer = AwsIotMqtt_MallocMessage( bufLen );

        /* If Memory cannot be allocated log an error and return */
        if( pBuffer == NULL )
        {
            AwsIotLogError( "Failed to allocate memory for SUBSCRIBE message." );
            ret = AWS_IOT_MQTT_NO_MEMORY;
        }
    }

    if( ret == AWS_IOT_MQTT_SUCCESS )
    {
        error = _serializeSubscribe( pSubscriptionList, subscriptionCount, pBuffer, &bufLen, usPacketIdentifier );
        if( error != AWS_IOT_SERIALIZER_SUCCESS )
        {
            AwsIotLogError( "Failed to serialize SUBSCRIBE message, error = %d", error );
            ret = AWS_IOT_MQTT_BAD_PARAMETER;
        }
    }

    if( ret == AWS_IOT_MQTT_SUCCESS )
    {
        *pSubscribePacket = pBuffer;
        *pPacketSize = bufLen;
        *pPacketIdentifier = usPacketIdentifier;
    }
    else
    {
        if( pBuffer != NULL )
        {
            AwsIotMqtt_FreeMessage( pBuffer );
        }

        *pSubscribePacket = NULL;
        *pPacketSize = 0;
    }

    return ret;
}

IotMqttError_t IotBleMqtt_DeserializeSuback( IotMqttConnection_t mqttConnection,
                                                        const uint8_t * const pSubackStart,
                                                        size_t dataLength,
                                                        uint16_t * const pPacketIdentifier,
                                                        size_t * const pBytesProcessed )
{

    AwsIotSerializerDecoderObject_t decoderObj = { 0 }, decoderValue = { 0 };
    AwsIotSerializerError_t error;
    int64_t subscriptionStatus;
    IotMqttError_t ret = AWS_IOT_MQTT_SUCCESS;

    ( *pBytesProcessed ) = 0;

    error = IOT_BLE_MESG_DECODER.init( &decoderObj, ( uint8_t * ) pSubackStart, dataLength );

    if ( ( error != AWS_IOT_SERIALIZER_SUCCESS )
            || ( decoderObj.type != AWS_IOT_SERIALIZER_CONTAINER_MAP ) )
    {
        AwsIotLogError( "Malformed SUBACK, decoding the packet failed, decoder error = %d, type: %d", error, decoderObj.type );
        ret = AWS_IOT_MQTT_BAD_RESPONSE;
    }

    if( ret == AWS_IOT_MQTT_SUCCESS )
    {

        error = IOT_BLE_MESG_DECODER.find( &decoderObj, IOT_BLE_MQTT_MESSAGE_ID, &decoderValue );
        if ( ( error != AWS_IOT_SERIALIZER_SUCCESS ) ||
                ( decoderValue.type != AWS_IOT_SERIALIZER_SCALAR_SIGNED_INT ) )
        {
            AwsIotLogError( "Message ID decode failed, error = %d, decoded value type = %d", error, decoderValue.type );
            ret = AWS_IOT_MQTT_BAD_RESPONSE;
        }
        else
        {
            *pPacketIdentifier = ( uint16_t ) decoderValue.value.signedInt;
        }
    }

    if( ret == AWS_IOT_MQTT_SUCCESS )
    {
        error = IOT_BLE_MESG_DECODER.find( &decoderObj, IOT_BLE_MQTT_STATUS, &decoderValue );
        if ( ( error != AWS_IOT_SERIALIZER_SUCCESS ) ||
                ( decoderValue.type != AWS_IOT_SERIALIZER_SCALAR_SIGNED_INT ) )
        {
            AwsIotLogError( "Status code decode failed, error = %d, decoded value type = %d", error, decoderValue.type );
            ret = AWS_IOT_MQTT_BAD_RESPONSE;
        }
        else
        {
            subscriptionStatus = ( uint16_t ) decoderValue.value.signedInt;
            switch( subscriptionStatus )
            {
                case 0x00:
                case 0x01:
                case 0x02:
                    AwsIotLog( AWS_IOT_LOG_DEBUG,
                               &_logHideAll,
                               "Topic accepted, max QoS %hhu.", subscriptionStatus );
                    ret = AWS_IOT_MQTT_SUCCESS;
                    break;
                case 0x80:
                    AwsIotLog( AWS_IOT_LOG_DEBUG,
                               &_logHideAll,
                               "Topic refused." );

                    /* Remove a rejected subscription from the subscription manager. */
                    AwsIotMqttInternal_RemoveSubscriptionByPacket(
                            ( _mqttConnection_t * ) mqttConnection,
                            ( *pPacketIdentifier ),
                            0 );
                    ret = AWS_IOT_MQTT_SERVER_REFUSED;
                    break;
                default:
                    AwsIotLog( AWS_IOT_LOG_DEBUG,
                               &_logHideAll,
                               "Bad SUBSCRIBE status %hhu.", subscriptionStatus );

                    ret = AWS_IOT_MQTT_BAD_RESPONSE;
                    break;
            }

            ( *pBytesProcessed ) = dataLength;
        }

    }

    IOT_BLE_MESG_DECODER.destroy( &decoderObj );

	return ret;
}

IotMqttError_t IotBleMqtt_SerializeUnsubscribe( const IotMqttSubscription_t * const pSubscriptionList,
		size_t subscriptionCount,
		uint8_t ** const pUnsubscribePacket,
		size_t * const pPacketSize,
		uint16_t * const pPacketIdentifier )
{

	uint8_t * pBuffer = NULL;
    size_t bufLen = 0;
    uint16_t usPacketIdentifier = 0;
    AwsIotSerializerError_t error;
    IotMqttError_t ret = AWS_IOT_MQTT_SUCCESS;

    usPacketIdentifier = _nextPacketIdentifier();

    error = _serializeUnSubscribe( pSubscriptionList, subscriptionCount, NULL, &bufLen, usPacketIdentifier );
    if( error != AWS_IOT_SERIALIZER_SUCCESS )
    {
        AwsIotLogError( "Failed to find serialized length of UNSUBSCRIBE message, error = %d", error );
        ret = AWS_IOT_MQTT_BAD_PARAMETER;
    }

    if( ret == AWS_IOT_MQTT_SUCCESS )
    {
        pBuffer = AwsIotMqtt_MallocMessage( bufLen );

        /* If Memory cannot be allocated log an error and return */
        if( pBuffer == NULL )
        {
            AwsIotLogError( "Failed to allocate memory for UNSUBSCRIBE message." );
            ret = AWS_IOT_MQTT_NO_MEMORY;
        }
    }

    if( ret == AWS_IOT_MQTT_SUCCESS )
    {
        error = _serializeUnSubscribe( pSubscriptionList, subscriptionCount, pBuffer, &bufLen, usPacketIdentifier );
        if( error != AWS_IOT_SERIALIZER_SUCCESS )
        {
            AwsIotLogError( "Failed to serialize UNSUBSCRIBE message, error = %d", error );
            ret = AWS_IOT_MQTT_BAD_PARAMETER;
        }
    }

    if( ret == AWS_IOT_MQTT_SUCCESS )
    {
        *pUnsubscribePacket = pBuffer;
        *pPacketSize = bufLen;
        *pPacketIdentifier = usPacketIdentifier;
    }
    else
    {
        if( pBuffer != NULL )
        {
            AwsIotMqtt_FreeMessage( pBuffer );
        }

        *pUnsubscribePacket = NULL;
        *pPacketSize = 0;
    }

    return ret;
}

IotMqttError_t IotBleMqtt_DeserializeUnsuback( const uint8_t * const pUnsubackStart,
                                                          size_t dataLength,
                                                          uint16_t * const pPacketIdentifier,
                                                          size_t * const pBytesProcessed )
{
    AwsIotSerializerDecoderObject_t decoderObj = { 0 }, decoderValue = { 0 };
    AwsIotSerializerError_t error;
    IotMqttError_t ret = AWS_IOT_MQTT_SUCCESS;

    error = IOT_BLE_MESG_DECODER.init( &decoderObj, ( uint8_t * ) pUnsubackStart, dataLength );
    if( ( error != AWS_IOT_SERIALIZER_SUCCESS )
            || ( decoderObj.type != AWS_IOT_SERIALIZER_CONTAINER_MAP ) )
    {
        AwsIotLogError( "Malformed UNSUBACK, decoding the packet failed, decoder error = %d, type:%d ", error, decoderObj.type );
        ret = AWS_IOT_MQTT_BAD_RESPONSE;
    }

    if( ret == AWS_IOT_MQTT_SUCCESS )
    {
        error = IOT_BLE_MESG_DECODER.find( &decoderObj, IOT_BLE_MQTT_MESSAGE_ID, &decoderValue );
        if ( ( error != AWS_IOT_SERIALIZER_SUCCESS ) ||
                ( decoderValue.type != AWS_IOT_SERIALIZER_SCALAR_SIGNED_INT ) )
        {
            AwsIotLogError( "UNSUBACK Message identifier decode failed, error = %d, decoded value type = %d", error, decoderValue.type );
            ret = AWS_IOT_MQTT_BAD_RESPONSE;

        }
        else
        {
            *pPacketIdentifier = ( uint16_t ) decoderValue.value.signedInt;
        }
    }

    if( ret == AWS_IOT_MQTT_SUCCESS )
    {
        ( *pBytesProcessed ) = dataLength;
    }
    else
    {
        ( *pBytesProcessed ) = 0;
    }

    IOT_BLE_MESG_DECODER.destroy( &decoderObj );

	return AWS_IOT_MQTT_SUCCESS;
}

IotMqttError_t IotBleMqtt_SerializeDisconnect( uint8_t ** const pDisconnectPacket,
                                                          size_t * const pPacketSize )
{
	uint8_t *pBuffer = NULL;
	size_t bufLen = 0;
	AwsIotSerializerError_t error;
	IotMqttError_t ret = AWS_IOT_MQTT_SUCCESS;

	error = _serializeDisconnect( NULL, &bufLen);
	if( error != AWS_IOT_SERIALIZER_SUCCESS )
	{
	    AwsIotLogError( "Failed to find serialized length of DISCONNECT message, error = %d", error );
	    ret = AWS_IOT_MQTT_BAD_PARAMETER;
	}

	if( ret == AWS_IOT_MQTT_SUCCESS )
	{
	    pBuffer = AwsIotMqtt_MallocMessage( bufLen );

	    /* If Memory cannot be allocated log an error and return */
	    if( pBuffer == NULL )
	    {
	        AwsIotLogError( "Failed to allocate memory for DISCONNECT message." );
	        ret = AWS_IOT_MQTT_NO_MEMORY;
	    }
	}


	if( ret == AWS_IOT_MQTT_SUCCESS )
	{
	    error = _serializeDisconnect( pBuffer, &bufLen );
	    if( error != AWS_IOT_SERIALIZER_SUCCESS )
	    {
	        AwsIotLogError( "Failed to serialize DISCONNECT message, error = %d", error );
	        ret = AWS_IOT_MQTT_BAD_PARAMETER;
	    }
	}

	if( ret == AWS_IOT_MQTT_SUCCESS )
	{
	    *pDisconnectPacket = pBuffer;
	    *pPacketSize = bufLen;
	}
	else
	{
	    if( pBuffer != NULL )
	    {
	        AwsIotMqtt_FreeMessage( pBuffer );
	    }

	    *pDisconnectPacket = NULL;
	    *pPacketSize = 0;
	}

	return ret;
}


uint8_t IotBleMqtt_GetPacketType( const uint8_t * const pPacket, size_t packetSize )
{

    AwsIotSerializerDecoderObject_t decoderObj = { 0 }, decoderValue = { 0 };
    AwsIotSerializerError_t error;
    uint8_t value, packetType = _INVALID_MQTT_PACKET_TYPE;

    error = IOT_BLE_MESG_DECODER.init( &decoderObj, ( uint8_t* ) pPacket, packetSize );

    if( ( error == AWS_IOT_SERIALIZER_SUCCESS )
            && ( decoderObj.type == AWS_IOT_SERIALIZER_CONTAINER_MAP ) )
    {

        error = IOT_BLE_MESG_DECODER.find( &decoderObj, IOT_BLE_MQTT_MSG_TYPE, &decoderValue );

        if ( ( error == AWS_IOT_SERIALIZER_SUCCESS ) &&
                ( decoderValue.type == AWS_IOT_SERIALIZER_SCALAR_SIGNED_INT ) )
        {
            value = ( uint16_t ) decoderValue.value.signedInt;

            /** Left shift by 4 bits as MQTT library expects packet type to be upper 4 bits **/
            packetType = value << 4;
        }
        else
        {
            AwsIotLogError( "Packet type decode failed, error = %d, decoded value type = %d", error, decoderValue.type );
        }
    }
    else
    {
        AwsIotLogError( "Decoding the packet failed, decoder error = %d, type = %d", error, decoderObj.type );
    }

    IOT_BLE_MESG_DECODER.destroy( &decoderObj );

    return packetType;
}

IotMqttError_t IotBleMqtt_SerializePingreq( uint8_t ** const pPingreqPacket,
                                                       size_t * const pPacketSize )
{
    return AWS_IOT_MQTT_NO_MEMORY;
}

IotMqttError_t IotBleMqtt_DeserializePingresp( const uint8_t * const pPingrespStart,
                                                          size_t dataLength,
                                                          size_t * const pBytesProcessed )
{
    return AWS_IOT_MQTT_BAD_RESPONSE;
}

void IotBleMqtt_FreePacket( uint8_t * pPacket )
{
    AwsIotMqtt_FreeMessage( pPacket );
}
