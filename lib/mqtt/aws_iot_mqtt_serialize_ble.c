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
#ifdef AWS_IOT_CONFIG_FILE
    #include AWS_IOT_CONFIG_FILE
#endif

/* Standard includes. */
#include <string.h>
#include <stdio.h>

/* FreeRTOS includes. */
#include "FreeRTOS.h"

/* MQTT internal includes. */
#include "iot_ble_config.h"
#include "aws_iot_serializer.h"
#include "private/aws_iot_mqtt_serialize_ble.h"
#include "private/aws_iot_mqtt_internal.h"

/* internal includes. */
#include "private/aws_iot_mqtt_internal.h"

#define _INVALID_MQTT_PACKET_TYPE        ( 0xF0 )

#if ( IOT_BLE_ENABLE_CBOR_ENCODING == 1 )
#define _MQTT_BLE_ENCODER  ( _AwsIotSerializerCborEncoder )
#define _MQTT_BLE_DECODER  ( _AwsIotSerializerCborDecoder )
#elif ( IOT_BLE_ENABLE_JSON_ENCODING == 1 )
#define _MQTT_BLE_ENCODER  ( _AwsIotSerializerJsonEncoder )
#define _MQTT_BLE_DECODER  ( _AwsIotSerializerJsonDecoder )
#endif

#define _validateSerializerResult( encoder, result )                    \
        if( ( result != AWS_IOT_SERIALIZER_SUCCESS ) &&                 \
                ( result !=  AWS_IOT_SERIALIZER_BUFFER_TOO_SMALL ) )    \
        {                                                               \
            IOT_BLE_MESG_ENCODER.destroy( &xEncoderObj );                 \
            return result;                                              \
        }

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
static uint16_t prusNextPacketIdentifier( void );


static inline uint16_t prusGetNumPublishParams( const AwsIotMqttPublishInfo_t * const pPublish )
{
   return ( pPublish->QoS > 0 ) ?  ( _NUM_DEFAULT_PUBLISH_PARMAS + 1 ) : _NUM_DEFAULT_PUBLISH_PARMAS;
}

static AwsIotSerializerError_t prxSerializeConnect( const AwsIotMqttConnectInfo_t * const pConnectInfo,
                                       const AwsIotMqttPublishInfo_t * const pWillInfo,
                                       uint8_t* const pBuffer,
                                       size_t* const pSize );
static AwsIotSerializerError_t prxSerializePublish( const AwsIotMqttPublishInfo_t * const pPublishInfo,
                                                  uint8_t * pBuffer,
                                                  size_t  * pSize,
                                                  uint16_t packetIdentifier );
static AwsIotSerializerError_t prxSerializePubAck( uint16_t packetIdentifier,
                                      uint8_t * pBuffer,
                                      size_t  * pSize );



static AwsIotSerializerError_t prxSerializeSubscribe( const AwsIotMqttSubscription_t * const pSubscriptionList,
                                               size_t subscriptionCount,
                                               uint8_t * const pBuffer,
                                               size_t * const pSize,
                                               uint16_t packetIdentifier );

static AwsIotSerializerError_t prxSerializeUnSubscribe( const AwsIotMqttSubscription_t * const pSubscriptionList,
                                               size_t subscriptionCount,
                                               uint8_t * const pBuffer,
                                               size_t * const pSize,
                                               uint16_t packetIdentifier );

static AwsIotSerializerError_t prxSerializeDisconnect( uint8_t * const pBuffer,
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


static AwsIotMutex_t xPacketIdentifierMutex;


/* Declaration of snprintf. The header stdio.h is not included because it
 * includes conflicting symbols on some platforms. */
extern int snprintf( char * s,
                     size_t n,
                     const char * format,
                     ... );

/*-----------------------------------------------------------*/

static uint16_t prusNextPacketIdentifier( void )
{
    static uint16_t nextPacketIdentifier = 1;
    uint16_t newPacketIdentifier = 0;

    /* Lock the packet identifier mutex so that only one thread may read and
         * modify nextPacketIdentifier. */
     AwsIotMutex_Lock( &xPacketIdentifierMutex );

    /* Read the next packet identifier. */
    newPacketIdentifier = nextPacketIdentifier;

    /* The next packet identifier will be greater by 2. This prevents packet
     * identifiers from ever being 0, which is not allowed by MQTT 3.1.1. Packet
     * identifiers will follow the sequence 1,3,5...65535,1,3,5... */
    nextPacketIdentifier = ( uint16_t ) ( nextPacketIdentifier + ( ( uint16_t ) 2 ) );

    /* Unlock the packet identifier mutex. */
    AwsIotMutex_Unlock( &xPacketIdentifierMutex );

    return newPacketIdentifier;
}

static AwsIotSerializerError_t prxSerializeConnect( const AwsIotMqttConnectInfo_t * const pConnectInfo,
                                       const AwsIotMqttPublishInfo_t * const pWillInfo,
                                       uint8_t* const pBuffer,
                                       size_t* const pSize )
{
    AwsIotSerializerError_t xError = AWS_IOT_SERIALIZER_SUCCESS;
    AwsIotSerializerEncoderObject_t xEncoderObj = AWS_IOT_SERIALIZER_ENCODER_CONTAINER_INITIALIZER_STREAM ;
    AwsIotSerializerEncoderObject_t xConnectMap = AWS_IOT_SERIALIZER_ENCODER_CONTAINER_INITIALIZER_MAP;
    AwsIotSerializerScalarData_t xData = { 0 };

    xError = IOT_BLE_MESG_ENCODER.init( &xEncoderObj, pBuffer, *pSize );
    if( xError != AWS_IOT_SERIALIZER_SUCCESS )
    {
        return xError;
    }

    xError = IOT_BLE_MESG_ENCODER.openContainer(
            &xEncoderObj,
            &xConnectMap,
            _NUM_CONNECT_PARMAS );
    _validateSerializerResult( &xEncoderObj, xError );

    xData.type = AWS_IOT_SERIALIZER_SCALAR_SIGNED_INT;
    xData.value.signedInt = mqttBLEMSG_TYPE_CONNECT;
    xError = IOT_BLE_MESG_ENCODER.appendKeyValue( &xConnectMap, mqttBLEMSG_TYPE, xData );
    _validateSerializerResult( &xEncoderObj, xError );

    xData.type = AWS_IOT_SERIALIZER_SCALAR_TEXT_STRING;
    xData.value.pString = ( uint8_t * ) pConnectInfo->pClientIdentifier;
    xData.value.stringLength = pConnectInfo->clientIdentifierLength;
    xError = IOT_BLE_MESG_ENCODER.appendKeyValue( &xConnectMap, mqttBLECLIENT_ID, xData );
    _validateSerializerResult( &xEncoderObj, xError );

    xData.type = AWS_IOT_SERIALIZER_SCALAR_TEXT_STRING;
    xData.value.pString = ( uint8_t * ) clientcredentialMQTT_BROKER_ENDPOINT;
    xData.value.stringLength = strlen( clientcredentialMQTT_BROKER_ENDPOINT );
    xError = IOT_BLE_MESG_ENCODER.appendKeyValue( &xConnectMap, mqttBLEBROKER_EP, xData );
    _validateSerializerResult( &xEncoderObj, xError );

    xData.type = AWS_IOT_SERIALIZER_SCALAR_BOOL;
    xData.value.booleanValue = pConnectInfo->cleanSession;
    xError = IOT_BLE_MESG_ENCODER.appendKeyValue( &xConnectMap, mqttBLECLEAN_SESSION, xData );
    _validateSerializerResult( &xEncoderObj, xError );

    xError = IOT_BLE_MESG_ENCODER.closeContainer( &xEncoderObj, &xConnectMap );
    _validateSerializerResult( &xEncoderObj, xError );

    if( pBuffer == NULL )
    {
        *pSize = IOT_BLE_MESG_ENCODER.getExtraBufferSizeNeeded( &xEncoderObj );
    }
    else
    {
        *pSize = IOT_BLE_MESG_ENCODER.getEncodedSize( &xEncoderObj, pBuffer );
    }

    IOT_BLE_MESG_ENCODER.destroy( &xEncoderObj );

    return AWS_IOT_SERIALIZER_SUCCESS;
}

static AwsIotSerializerError_t prxSerializePublish( const AwsIotMqttPublishInfo_t * const pPublishInfo,
                                                  uint8_t * pBuffer,
                                                  size_t  * pSize,
                                                  uint16_t packetIdentifier )
{
    AwsIotSerializerError_t xError = AWS_IOT_SERIALIZER_SUCCESS;
    AwsIotSerializerEncoderObject_t xEncoderObj = AWS_IOT_SERIALIZER_ENCODER_CONTAINER_INITIALIZER_STREAM ;
    AwsIotSerializerEncoderObject_t xPublishMap = AWS_IOT_SERIALIZER_ENCODER_CONTAINER_INITIALIZER_MAP;
    AwsIotSerializerScalarData_t xData = { 0 };
    uint16_t usNumPublishParams = prusGetNumPublishParams( pPublishInfo );

    xError = IOT_BLE_MESG_ENCODER.init( &xEncoderObj, pBuffer, *pSize );

    if( xError != AWS_IOT_SERIALIZER_SUCCESS )
    {
        return xError;
    }

    xError = IOT_BLE_MESG_ENCODER.openContainer(
            &xEncoderObj,
            &xPublishMap,
            usNumPublishParams );
    _validateSerializerResult( &xEncoderObj, xError );

    xData.type = AWS_IOT_SERIALIZER_SCALAR_SIGNED_INT;
    xData.value.signedInt = mqttBLEMSG_TYPE_PUBLISH;
    xError = IOT_BLE_MESG_ENCODER.appendKeyValue( &xPublishMap, mqttBLEMSG_TYPE, xData );
    _validateSerializerResult( &xEncoderObj, xError );

    xData.type = AWS_IOT_SERIALIZER_SCALAR_TEXT_STRING;
    xData.value.pString = ( uint8_t * ) pPublishInfo->pTopicName;
    xData.value.stringLength = pPublishInfo->topicNameLength;
    xError = IOT_BLE_MESG_ENCODER.appendKeyValue( &xPublishMap, mqttBLETOPIC, xData );
    _validateSerializerResult( &xEncoderObj, xError );

    xData.type = AWS_IOT_SERIALIZER_SCALAR_SIGNED_INT;
    xData.value.signedInt = pPublishInfo->QoS;
    xError = IOT_BLE_MESG_ENCODER.appendKeyValue( &xPublishMap, mqttBLEQOS, xData );
    _validateSerializerResult( &xEncoderObj, xError );


    xData.type = AWS_IOT_SERIALIZER_SCALAR_BYTE_STRING;
    xData.value.pString = ( uint8_t * ) pPublishInfo->pPayload;
    xData.value.stringLength = pPublishInfo->payloadLength;
    xError = IOT_BLE_MESG_ENCODER.appendKeyValue( &xPublishMap, mqttBLEPAYLOAD, xData );
    _validateSerializerResult( &xEncoderObj, xError );


    if( pPublishInfo->QoS != 0 )
    {
        xData.type = AWS_IOT_SERIALIZER_SCALAR_SIGNED_INT;
        xData.value.signedInt = packetIdentifier;
        xError = IOT_BLE_MESG_ENCODER.appendKeyValue( &xPublishMap, mqttBLEMESSAGE_ID, xData );
        _validateSerializerResult( &xEncoderObj, xError );
    }

    xError = IOT_BLE_MESG_ENCODER.closeContainer( &xEncoderObj, &xPublishMap );
    _validateSerializerResult( &xEncoderObj, xError );

    if( pBuffer == NULL )
    {
        *pSize = IOT_BLE_MESG_ENCODER.getExtraBufferSizeNeeded( &xEncoderObj );

    }
    else
    {
        *pSize = IOT_BLE_MESG_ENCODER.getEncodedSize( &xEncoderObj, pBuffer );
    }
    IOT_BLE_MESG_ENCODER.destroy( &xEncoderObj );

    return AWS_IOT_SERIALIZER_SUCCESS;
}

static AwsIotSerializerError_t prxSerializePubAck( uint16_t packetIdentifier,
                                      uint8_t * pBuffer,
                                      size_t  * pSize )

{
    AwsIotSerializerError_t xError = AWS_IOT_SERIALIZER_SUCCESS;
    AwsIotSerializerEncoderObject_t xEncoderObj = AWS_IOT_SERIALIZER_ENCODER_CONTAINER_INITIALIZER_STREAM ;
    AwsIotSerializerEncoderObject_t xPubAckMap = AWS_IOT_SERIALIZER_ENCODER_CONTAINER_INITIALIZER_MAP;
    AwsIotSerializerScalarData_t xData = { 0 };

    xError = IOT_BLE_MESG_ENCODER.init( &xEncoderObj, pBuffer, *pSize );

    if( xError != AWS_IOT_SERIALIZER_SUCCESS )
    {
        return xError;
    }
    xError = IOT_BLE_MESG_ENCODER.openContainer(
            &xEncoderObj,
            &xPubAckMap,
            _NUM_PUBACK_PARMAS );
    _validateSerializerResult( &xEncoderObj, xError );

    xData.type = AWS_IOT_SERIALIZER_SCALAR_SIGNED_INT;
    xData.value.signedInt = mqttBLEMSG_TYPE_PUBACK;
    xError = IOT_BLE_MESG_ENCODER.appendKeyValue( &xPubAckMap, mqttBLEMSG_TYPE, xData );
    _validateSerializerResult( &xEncoderObj, xError );

    xData.type = AWS_IOT_SERIALIZER_SCALAR_SIGNED_INT;
    xData.value.signedInt = packetIdentifier;
    xError = IOT_BLE_MESG_ENCODER.appendKeyValue( &xPubAckMap, mqttBLEMESSAGE_ID, xData );
    _validateSerializerResult( &xEncoderObj, xError );

    xError = IOT_BLE_MESG_ENCODER.closeContainer( &xEncoderObj, &xPubAckMap );
    _validateSerializerResult( &xEncoderObj, xError );

    if( pBuffer == NULL )
    {
        *pSize = IOT_BLE_MESG_ENCODER.getExtraBufferSizeNeeded( &xEncoderObj );
    }
    else
    {
        *pSize = IOT_BLE_MESG_ENCODER.getEncodedSize( &xEncoderObj, pBuffer );
    }
    IOT_BLE_MESG_ENCODER.destroy( &xEncoderObj );

    return AWS_IOT_SERIALIZER_SUCCESS;
}


static AwsIotSerializerError_t prxSerializeSubscribe( const AwsIotMqttSubscription_t * const pSubscriptionList,
                                               size_t subscriptionCount,
                                               uint8_t * const pBuffer,
                                               size_t * const pSize,
                                               uint16_t packetIdentifier )
{
    AwsIotSerializerError_t xError = AWS_IOT_SERIALIZER_SUCCESS;
    AwsIotSerializerEncoderObject_t xEncoderObj = AWS_IOT_SERIALIZER_ENCODER_CONTAINER_INITIALIZER_STREAM ;
    AwsIotSerializerEncoderObject_t xSubscribeMap = AWS_IOT_SERIALIZER_ENCODER_CONTAINER_INITIALIZER_MAP;
    AwsIotSerializerEncoderObject_t xSubscriptionArray = AWS_IOT_SERIALIZER_ENCODER_CONTAINER_INITIALIZER_ARRAY;
    AwsIotSerializerScalarData_t xData = { 0 };
    uint16_t usIdx;

    xError = IOT_BLE_MESG_ENCODER.init( &xEncoderObj, pBuffer, *pSize );

    if( xError != AWS_IOT_SERIALIZER_SUCCESS )
    {
        return xError;
    }

    xError = IOT_BLE_MESG_ENCODER.openContainer(
            &xEncoderObj,
            &xSubscribeMap,
            _NUM_SUBACK_PARAMS );
    _validateSerializerResult( &xEncoderObj, xError );

    xData.type = AWS_IOT_SERIALIZER_SCALAR_SIGNED_INT;
    xData.value.signedInt = mqttBLEMSG_TYPE_SUBSCRIBE;
    xError = IOT_BLE_MESG_ENCODER.appendKeyValue( &xSubscribeMap, mqttBLEMSG_TYPE, xData );
    _validateSerializerResult( &xEncoderObj, xError );

    xError = IOT_BLE_MESG_ENCODER.openContainerWithKey(
            &xSubscribeMap,
            mqttBLETOPIC_LIST,
            &xSubscriptionArray,
            subscriptionCount );
    _validateSerializerResult( &xEncoderObj, xError );

    for( usIdx = 0; usIdx < subscriptionCount; usIdx++ )
    {
        xData.type = AWS_IOT_SERIALIZER_SCALAR_TEXT_STRING;
        xData.value.pString = ( uint8_t * ) pSubscriptionList[ usIdx ].pTopicFilter;
        xData.value.stringLength = pSubscriptionList[ usIdx ].topicFilterLength;
        xError = IOT_BLE_MESG_ENCODER.append( &xSubscriptionArray, xData );
        _validateSerializerResult( &xEncoderObj, xError );
    }

    xError = IOT_BLE_MESG_ENCODER.closeContainer( &xSubscribeMap, &xSubscriptionArray );
    _validateSerializerResult( &xEncoderObj, xError );

    xError = IOT_BLE_MESG_ENCODER.openContainerWithKey(
            &xSubscribeMap,
            mqttBLEQOS_LIST,
            &xSubscriptionArray,
            subscriptionCount );
    _validateSerializerResult( &xEncoderObj, xError );

    for( usIdx = 0; usIdx < subscriptionCount; usIdx++ )
    {

        xData.type = AWS_IOT_SERIALIZER_SCALAR_SIGNED_INT;
        xData.value.signedInt = pSubscriptionList[ usIdx ].QoS;
        xError = IOT_BLE_MESG_ENCODER.append( &xSubscriptionArray, xData );
        _validateSerializerResult( &xEncoderObj, xError );
    }

    xError = IOT_BLE_MESG_ENCODER.closeContainer( &xSubscribeMap, &xSubscriptionArray );
    _validateSerializerResult( &xEncoderObj, xError );

    xData.type = AWS_IOT_SERIALIZER_SCALAR_SIGNED_INT;
    xData.value.signedInt = packetIdentifier;
    xError = IOT_BLE_MESG_ENCODER.appendKeyValue( &xSubscribeMap, mqttBLEMESSAGE_ID, xData );
    _validateSerializerResult( &xEncoderObj, xError );

    xError = IOT_BLE_MESG_ENCODER.closeContainer( &xEncoderObj, &xSubscribeMap );
    _validateSerializerResult( &xEncoderObj, xError );

    if( pBuffer == NULL )
    {
        *pSize = IOT_BLE_MESG_ENCODER.getExtraBufferSizeNeeded( &xEncoderObj );
    }
    else
    {
        *pSize = IOT_BLE_MESG_ENCODER.getEncodedSize( &xEncoderObj, pBuffer );
    }

    IOT_BLE_MESG_ENCODER.destroy( &xEncoderObj );
    return AWS_IOT_SERIALIZER_SUCCESS;
}

static AwsIotSerializerError_t prxSerializeUnSubscribe( const AwsIotMqttSubscription_t * const pSubscriptionList,
                                               size_t subscriptionCount,
                                               uint8_t * const pBuffer,
                                               size_t * const pSize,
                                               uint16_t packetIdentifier )
{
    AwsIotSerializerError_t xError = AWS_IOT_SERIALIZER_SUCCESS;
    AwsIotSerializerEncoderObject_t xEncoderObj = AWS_IOT_SERIALIZER_ENCODER_CONTAINER_INITIALIZER_STREAM;
    AwsIotSerializerEncoderObject_t xSubscribeMap = AWS_IOT_SERIALIZER_ENCODER_CONTAINER_INITIALIZER_MAP;
    AwsIotSerializerEncoderObject_t xSubscriptionArray = AWS_IOT_SERIALIZER_ENCODER_CONTAINER_INITIALIZER_ARRAY;
    AwsIotSerializerScalarData_t xData = { 0 };
    uint16_t usIdx;

    xError = IOT_BLE_MESG_ENCODER.init( &xEncoderObj, pBuffer, *pSize );

    if( xError != AWS_IOT_SERIALIZER_SUCCESS )
    {
        return xError;
    }

    xError = IOT_BLE_MESG_ENCODER.openContainer(
            &xEncoderObj,
            &xSubscribeMap,
            _NUM_UNSUBACK_PARAMS );
    _validateSerializerResult( &xEncoderObj, xError );

    xData.type = AWS_IOT_SERIALIZER_SCALAR_SIGNED_INT;
    xData.value.signedInt = mqttBLEMSG_TYPE_UNSUBSCRIBE;
    xError = IOT_BLE_MESG_ENCODER.appendKeyValue( &xSubscribeMap, mqttBLEMSG_TYPE, xData );
    _validateSerializerResult( &xEncoderObj, xError );

    xError = IOT_BLE_MESG_ENCODER.openContainerWithKey (
            &xSubscribeMap,
            mqttBLETOPIC_LIST,
            &xSubscriptionArray,
            subscriptionCount );
    _validateSerializerResult( &xEncoderObj, xError );

    for( usIdx = 0; usIdx < subscriptionCount; usIdx++ )
    {

        xData.type = AWS_IOT_SERIALIZER_SCALAR_TEXT_STRING;
        xData.value.pString = ( uint8_t * ) pSubscriptionList[ usIdx ].pTopicFilter;
        xData.value.stringLength = pSubscriptionList[ usIdx ].topicFilterLength;
        xError = IOT_BLE_MESG_ENCODER.append( &xSubscriptionArray, xData );
        _validateSerializerResult( &xEncoderObj, xError );
    }

    xError = IOT_BLE_MESG_ENCODER.closeContainer( &xSubscribeMap, &xSubscriptionArray );
    _validateSerializerResult( &xEncoderObj, xError );

    xData.type = AWS_IOT_SERIALIZER_SCALAR_SIGNED_INT;
    xData.value.signedInt = packetIdentifier;
    xError = IOT_BLE_MESG_ENCODER.appendKeyValue( &xSubscribeMap, mqttBLEMESSAGE_ID, xData );
    _validateSerializerResult( &xEncoderObj, xError );

    xError = IOT_BLE_MESG_ENCODER.closeContainer( &xEncoderObj, &xSubscribeMap );
    _validateSerializerResult( &xEncoderObj, xError );

    if( pBuffer == NULL )
    {
        *pSize = IOT_BLE_MESG_ENCODER.getExtraBufferSizeNeeded( &xEncoderObj );

    }
    else
    {
        *pSize = IOT_BLE_MESG_ENCODER.getEncodedSize( &xEncoderObj, pBuffer );
    }
    IOT_BLE_MESG_ENCODER.destroy( &xEncoderObj );

    return AWS_IOT_SERIALIZER_SUCCESS;
}

static AwsIotSerializerError_t prxSerializeDisconnect( uint8_t * const pBuffer,
                                                size_t * const pSize )
{
    AwsIotSerializerError_t xError = AWS_IOT_SERIALIZER_SUCCESS;
    AwsIotSerializerEncoderObject_t xEncoderObj = AWS_IOT_SERIALIZER_ENCODER_CONTAINER_INITIALIZER_STREAM;
    AwsIotSerializerEncoderObject_t xDisconnectMap = AWS_IOT_SERIALIZER_ENCODER_CONTAINER_INITIALIZER_MAP;
    AwsIotSerializerScalarData_t xData = { 0 };

    xError = IOT_BLE_MESG_ENCODER.init( &xEncoderObj, pBuffer, *pSize );

    if( xError != AWS_IOT_SERIALIZER_SUCCESS )
    {
        return xError;
    }

    xError = IOT_BLE_MESG_ENCODER.openContainer(
            &xEncoderObj,
            &xDisconnectMap,
            _NUM_DISCONNECT_PARAMS );
    _validateSerializerResult( &xEncoderObj, xError );

    xData.type = AWS_IOT_SERIALIZER_SCALAR_SIGNED_INT;
    xData.value.signedInt = mqttBLEMSG_TYPE_DISCONNECT;
    xError = IOT_BLE_MESG_ENCODER.appendKeyValue( &xDisconnectMap, mqttBLEMSG_TYPE, xData );
    _validateSerializerResult( &xEncoderObj, xError );

    xError = IOT_BLE_MESG_ENCODER.closeContainer( &xEncoderObj, &xDisconnectMap );
    _validateSerializerResult( &xEncoderObj, xError );

    if( pBuffer == NULL )
    {
        *pSize = IOT_BLE_MESG_ENCODER.getExtraBufferSizeNeeded( &xEncoderObj );
    }
    else
    {
        *pSize = IOT_BLE_MESG_ENCODER.getEncodedSize( &xEncoderObj, pBuffer );
    }
    IOT_BLE_MESG_ENCODER.destroy( &xEncoderObj );

    return AWS_IOT_SERIALIZER_SUCCESS;
}


bool AwsIotMqttBLE_InitSerialize( void )
{
	/* Create the packet identifier mutex. */
	return AwsIotMutex_Create( &xPacketIdentifierMutex );
}

void AwsIotMqttBLE_CleanupSerialize( void )
{
	/* Destroy the packet identifier mutex */
	AwsIotMutex_Destroy( &xPacketIdentifierMutex );
}

AwsIotMqttError_t AwsIotMqttBLE_SerializeConnect( const AwsIotMqttConnectInfo_t * const pConnectInfo,
                                                           const AwsIotMqttPublishInfo_t * const pWillInfo,
                                                           uint8_t ** const pConnectPacket,
                                                           size_t * const pPacketSize )
{
	uint8_t * pBuffer = NULL;
	size_t xBufLen = 0;
	AwsIotSerializerError_t xError;

	xError = prxSerializeConnect( pConnectInfo, pWillInfo, NULL, &xBufLen );
	if( xError != AWS_IOT_SERIALIZER_SUCCESS )
	{
	    AwsIotLogError( "Failed to find length of serialized CONNECT message, error = %d", xError );
	    return AWS_IOT_MQTT_BAD_PARAMETER;
	}

	pBuffer = AwsIotMqtt_MallocMessage( xBufLen );

	/* If Memory cannot be allocated log an error and return */
    if( pBuffer == NULL )
    {
        AwsIotLogError( "Failed to allocate memory for CONNECT packet." );
        return AWS_IOT_MQTT_NO_MEMORY;
    }

    xError = prxSerializeConnect( pConnectInfo, pWillInfo, pBuffer, &xBufLen );
    if( xError != AWS_IOT_SERIALIZER_SUCCESS )
    {
        AwsIotLogError( "Failed to serialize CONNECT message, error = %d", xError );
        return AWS_IOT_MQTT_BAD_PARAMETER;
    }

    *pConnectPacket = pBuffer;
    *pPacketSize = xBufLen;

    return AWS_IOT_MQTT_SUCCESS;
}

AwsIotMqttError_t AwsIotMqttBLE_DeserializeConnack( const uint8_t * const pConnackStart,
                                                         size_t dataLength,
                                                         size_t * const pBytesProcessed )
{

    AwsIotSerializerDecoderObject_t xDecoderObj, xValue = { 0 };
    AwsIotSerializerError_t xError;
    AwsIotMqttError_t xConnectionStatus = AWS_IOT_MQTT_SERVER_REFUSED;
    int64_t respCode = 0L;

    xError = IOT_BLE_MESG_DECODER.init( &xDecoderObj, ( uint8_t * ) pConnackStart, dataLength );

    if( xError != AWS_IOT_SERIALIZER_SUCCESS )
    {
        AwsIotLogError( "Malformed CONNACK, decoding the packet failed, decoder error = %d", xError );
        return AWS_IOT_MQTT_BAD_RESPONSE;
    }

    if( xDecoderObj.type != AWS_IOT_SERIALIZER_CONTAINER_MAP )
    {
        AwsIotLogError( "Invalid CONNACK, should be a map of key value pairs, decoded object type = %d", xDecoderObj.type );
        ( *pBytesProcessed ) = 0;
        IOT_BLE_MESG_DECODER.destroy( &xDecoderObj );
        return AWS_IOT_MQTT_BAD_RESPONSE;
    }

    xError = IOT_BLE_MESG_DECODER.find( &xDecoderObj, mqttBLESTATUS, &xValue );

    if ( ( xError != AWS_IOT_SERIALIZER_SUCCESS ) ||
         ( xValue.type != AWS_IOT_SERIALIZER_SCALAR_SIGNED_INT ) )
    {
        AwsIotLogError( "Invalid CONNACK, response code decode failed, error = %d, decoded value type = %d", xError, xValue.type );
        ( *pBytesProcessed ) = 0;
        IOT_BLE_MESG_DECODER.destroy( &xDecoderObj );
        return AWS_IOT_MQTT_BAD_RESPONSE;
    }

    respCode =  xValue.value.signedInt;
    if( ( respCode == eMQTTBLEStatusConnecting )
            || ( respCode == eMQTTBLEStatusConnected ) )
    {
        xConnectionStatus = AWS_IOT_MQTT_SUCCESS;
    }

    IOT_BLE_MESG_DECODER.destroy( &xDecoderObj );

    ( *pBytesProcessed ) = dataLength;

    return xConnectionStatus;
}

AwsIotMqttError_t AwsIotMqttBLE_SerializePublish( const AwsIotMqttPublishInfo_t * const pPublishInfo,
                                                  uint8_t ** const pPublishPacket,
                                                  size_t * const pPacketSize,
                                                  uint16_t * const pPacketIdentifier )
{

    uint8_t * pBuffer = NULL;
    size_t xBufLen = 0;
    uint16_t usPacketIdentifier = 0;
    AwsIotSerializerError_t xError;

    if( pPublishInfo->QoS != 0 )
    {
        usPacketIdentifier = prusNextPacketIdentifier();
    }

    xError = prxSerializePublish( pPublishInfo, NULL, &xBufLen, usPacketIdentifier );
    if( xError != AWS_IOT_SERIALIZER_SUCCESS )
    {
        AwsIotLogError( "Failed to find size of serialized PUBLISH message, error = %d", xError );
        return AWS_IOT_MQTT_BAD_PARAMETER;
    }

    pBuffer = AwsIotMqtt_MallocMessage( xBufLen );

    /* If Memory cannot be allocated log an error and return */
    if( pBuffer == NULL )
    {
        AwsIotLogError( "Failed to allocate memory for PUBLISH packet." );
        return AWS_IOT_MQTT_NO_MEMORY;
    }

    xError = prxSerializePublish( pPublishInfo, pBuffer, &xBufLen, usPacketIdentifier );
    if( xError != AWS_IOT_SERIALIZER_SUCCESS )
    {
        AwsIotLogError( "Failed to serialize PUBLISH message, error = %d", xError );
        return AWS_IOT_MQTT_BAD_PARAMETER;
    }

    *pPublishPacket = pBuffer;
    *pPacketSize = xBufLen;
    *pPacketIdentifier = usPacketIdentifier;

    return AWS_IOT_MQTT_SUCCESS;
}

void AwsIotMqttBLE_PublishSetDup( bool awsIotMqttMode, uint8_t * const pPublishPacket, uint16_t * const pNewPacketIdentifier )
{
	/** TODO: Currently DUP flag is not supported by BLE SDKs **/
}

AwsIotMqttError_t AwsIotMqttBLE_DeserializePublish( const uint8_t * const pPublishStart,
                                                         size_t dataLength,
                                                         AwsIotMqttPublishInfo_t * const pOutput,
                                                         uint16_t * const pPacketIdentifier,
                                                         size_t * const pBytesProcessed )
{

    AwsIotSerializerDecoderObject_t xDecoderObj = { 0 }, xValue = { 0 };
    AwsIotSerializerError_t xSerializerRet;
    AwsIotMqttError_t xRet = AWS_IOT_MQTT_SUCCESS;

    xSerializerRet = IOT_BLE_MESG_DECODER.init( &xDecoderObj, ( uint8_t * ) pPublishStart, dataLength );
    if ( (xSerializerRet != AWS_IOT_SERIALIZER_SUCCESS ) ||
            ( xDecoderObj.type != AWS_IOT_SERIALIZER_CONTAINER_MAP ) )
    {

        AwsIotLogError( "Decoding PUBLISH packet failed, decoder error = %d, object type = %d", xSerializerRet, xDecoderObj.type );
        xRet = AWS_IOT_MQTT_BAD_RESPONSE;
    }

    if( xRet == AWS_IOT_MQTT_SUCCESS )
    {
        xSerializerRet = IOT_BLE_MESG_DECODER.find( &xDecoderObj, mqttBLEQOS, &xValue );
        if ( ( xSerializerRet != AWS_IOT_SERIALIZER_SUCCESS ) ||
                 ( xValue.type != AWS_IOT_SERIALIZER_SCALAR_SIGNED_INT ) )
        {
            AwsIotLogError( "QOS Value decode failed, error = %d, decoded value type = %d", xSerializerRet, xValue.type );
            xRet = AWS_IOT_MQTT_BAD_RESPONSE;
        }
        else
        {
            pOutput->QoS = xValue.value.signedInt;
        }
    }

    if( xRet == AWS_IOT_MQTT_SUCCESS )
    {
        xValue.value.pString = NULL;
        xValue.value.stringLength = 0;
        xSerializerRet = IOT_BLE_MESG_DECODER.find( &xDecoderObj, mqttBLETOPIC, &xValue );

        if( ( xSerializerRet != AWS_IOT_SERIALIZER_SUCCESS ) ||
                ( xValue.type != AWS_IOT_SERIALIZER_SCALAR_TEXT_STRING ) )
        {
            AwsIotLogError( "Topic value decode failed, error = %d", xSerializerRet );
            xRet = AWS_IOT_MQTT_BAD_RESPONSE;
        }
        else
        {
            pOutput->pTopicName = ( const char* ) xValue.value.pString;
            pOutput->topicNameLength = xValue.value.stringLength;
        }
    }

    if( xRet == AWS_IOT_MQTT_SUCCESS )
    {
        xValue.value.pString = NULL;
        xValue.value.stringLength = 0;
        xSerializerRet = IOT_BLE_MESG_DECODER.find( &xDecoderObj, mqttBLEPAYLOAD, &xValue );

        if( ( xSerializerRet != AWS_IOT_SERIALIZER_SUCCESS ) ||
                ( xValue.type != AWS_IOT_SERIALIZER_SCALAR_BYTE_STRING ) )
        {
            AwsIotLogError( "Payload value decode failed, error = %d", xSerializerRet );
            xRet = AWS_IOT_MQTT_BAD_RESPONSE;
        }
        else
        {
            pOutput->pPayload = ( const char* ) xValue.value.pString;
            pOutput->payloadLength = xValue.value.stringLength;
        }
    }

    if( xRet == AWS_IOT_MQTT_SUCCESS )
    {
        if( pOutput->QoS != 0 )
        {
            xSerializerRet = IOT_BLE_MESG_DECODER.find( &xDecoderObj, mqttBLEMESSAGE_ID, &xValue );
            if ( ( xSerializerRet != AWS_IOT_SERIALIZER_SUCCESS ) ||
                    ( xValue.type != AWS_IOT_SERIALIZER_SCALAR_SIGNED_INT ) )
            {
                AwsIotLogError( "Message identifier decode failed, error = %d, decoded value type = %d", xSerializerRet, xValue.type );
                xRet = AWS_IOT_MQTT_BAD_RESPONSE;
            }
            else
            {
                *pPacketIdentifier = ( uint16_t ) xValue.value.signedInt;
            }
        }
    }

    if( xRet == AWS_IOT_MQTT_SUCCESS  )
    {
        *pBytesProcessed = dataLength;
        pOutput->retain = false;
    }
    else
    {
        *pBytesProcessed = 0;
    }

    IOT_BLE_MESG_DECODER.destroy( &xDecoderObj );

    return xRet;
}

AwsIotMqttError_t AwsIotMqttBLE_SerializePuback( uint16_t packetIdentifier,
                                                      uint8_t ** const pPubackPacket,
                                                      size_t * const pPacketSize )
{
	uint8_t * pBuffer;
	size_t xBufLen = 0;
	AwsIotSerializerError_t xError;

	xError = prxSerializePubAck( packetIdentifier, NULL, &xBufLen );

	if( xError != AWS_IOT_SERIALIZER_SUCCESS )
	{
	    AwsIotLogError( "Failed to find size of serialized PUBACK message, error = %d", xError );
	    return AWS_IOT_MQTT_BAD_PARAMETER;
	}


	pBuffer = AwsIotMqtt_MallocMessage( xBufLen );

	/* If Memory cannot be allocated log an error and return */
	if( pBuffer == NULL )
	{
		AwsIotLogError( "Failed to allocate memory for PUBACK packet, packet identifier = %d", packetIdentifier );
		return AWS_IOT_MQTT_NO_MEMORY;
	}

	xError = prxSerializePubAck( packetIdentifier, pBuffer, &xBufLen );

	if( xError != AWS_IOT_SERIALIZER_SUCCESS )
	{
	    AwsIotLogError( "Failed to find size of serialized PUBACK message, error = %d", xError );
	    return AWS_IOT_MQTT_BAD_PARAMETER;
	}

	*pPubackPacket = pBuffer;
	*pPacketSize = xBufLen;

	return AWS_IOT_MQTT_SUCCESS;

}

AwsIotMqttError_t AwsIotMqttBLE_DeserializePuback( const uint8_t * const pPubackStart,
                                                        size_t dataLength,
                                                        uint16_t * const pPacketIdentifier,
                                                        size_t * const pBytesProcessed )
{

    AwsIotSerializerDecoderObject_t xDecoderObj, xValue = { 0 };
    AwsIotSerializerError_t xError;

    xError = IOT_BLE_MESG_DECODER.init( &xDecoderObj, ( uint8_t * ) pPubackStart, dataLength );

    if( xError != AWS_IOT_SERIALIZER_SUCCESS )
    {
        AwsIotLogError( "Malformed PUBACK, decoding the packet failed, decoder error = %d", xError );
        return AWS_IOT_MQTT_BAD_RESPONSE;
    }

    if( xDecoderObj.type != AWS_IOT_SERIALIZER_CONTAINER_MAP )
    {
        AwsIotLogError( "Invalid PUBACK, should be a map of key value pairs, decoded object type = %d", xDecoderObj.type );
        IOT_BLE_MESG_DECODER.destroy( &xDecoderObj );
        ( *pBytesProcessed ) = 0;
        return AWS_IOT_MQTT_BAD_RESPONSE;
    }

    xError = IOT_BLE_MESG_DECODER.find( &xDecoderObj, mqttBLEMESSAGE_ID, &xValue );

    if ( ( xError != AWS_IOT_SERIALIZER_SUCCESS ) ||
            ( xValue.type != AWS_IOT_SERIALIZER_SCALAR_SIGNED_INT ) )
    {
        AwsIotLogError( "Message ID decode failed, error = %d, decoded value type = %d", xError, xValue.type );
        IOT_BLE_MESG_DECODER.destroy( &xDecoderObj );
        ( *pBytesProcessed ) = 0;
        return AWS_IOT_MQTT_BAD_RESPONSE;
    }
    *pPacketIdentifier = ( uint16_t ) xValue.value.signedInt;

    IOT_BLE_MESG_DECODER.destroy( &xDecoderObj );

    ( *pBytesProcessed ) = dataLength;

    return AWS_IOT_MQTT_SUCCESS;

}

AwsIotMqttError_t AwsIotMqttBLE_SerializeSubscribe( const AwsIotMqttSubscription_t * const pSubscriptionList,
                                                         size_t subscriptionCount,
                                                         uint8_t ** const pSubscribePacket,
                                                         size_t * const pPacketSize,
                                                         uint16_t * const pPacketIdentifier )
{
    uint8_t * pBuffer = NULL;
    size_t xBufLen = 0;
    uint16_t usPacketIdentifier = 0;
    AwsIotSerializerError_t xError;

    usPacketIdentifier = prusNextPacketIdentifier();

    xError = prxSerializeSubscribe( pSubscriptionList, subscriptionCount, NULL, &xBufLen, usPacketIdentifier );
    if( xError != AWS_IOT_SERIALIZER_SUCCESS )
    {
        AwsIotLogError( "Failed to find serialized length of SUBSCRIBE message, error = %d", xError );
        return AWS_IOT_MQTT_BAD_PARAMETER;
    }

    pBuffer = AwsIotMqtt_MallocMessage( xBufLen );

    /* If Memory cannot be allocated log an error and return */
    if( pBuffer == NULL )
    {
        AwsIotLogError( "Failed to allocate memory for SUBSCRIBE message." );
        return AWS_IOT_MQTT_NO_MEMORY;
    }

    xError = prxSerializeSubscribe( pSubscriptionList, subscriptionCount, pBuffer, &xBufLen, usPacketIdentifier );
    if( xError != AWS_IOT_SERIALIZER_SUCCESS )
    {
        AwsIotLogError( "Failed to serialize SUBSCRIBE message, error = %d", xError );
        return AWS_IOT_MQTT_BAD_PARAMETER;
    }

    *pSubscribePacket = pBuffer;
    *pPacketSize = xBufLen;
    *pPacketIdentifier = usPacketIdentifier;

    return AWS_IOT_MQTT_SUCCESS;
}

AwsIotMqttError_t AwsIotMqttBLE_DeserializeSuback( AwsIotMqttConnection_t mqttConnection,
                                                        const uint8_t * const pSubackStart,
                                                        size_t dataLength,
                                                        uint16_t * const pPacketIdentifier,
                                                        size_t * const pBytesProcessed )
{

    AwsIotSerializerDecoderObject_t xDecoderObj, xValue = { 0 };
    AwsIotSerializerError_t xError;
    int64_t subscriptionStatus;
    AwsIotMqttError_t status;

    xError = IOT_BLE_MESG_DECODER.init( &xDecoderObj, ( uint8_t * ) pSubackStart, dataLength );

    if( xError != AWS_IOT_SERIALIZER_SUCCESS )
    {
        AwsIotLogError( "Malformed SUBACK, decoding the packet failed, decoder error = %d", xError );
        return AWS_IOT_MQTT_BAD_RESPONSE;
    }

    if( xDecoderObj.type != AWS_IOT_SERIALIZER_CONTAINER_MAP )
    {
        AwsIotLogError( "Invalid SUBACK, should be a map of key value pairs, decoded object type = %d", xDecoderObj.type );
        IOT_BLE_MESG_DECODER.destroy( &xDecoderObj );
        ( *pBytesProcessed ) = 0;
        return AWS_IOT_MQTT_BAD_RESPONSE;
    }

    xError = IOT_BLE_MESG_DECODER.find( &xDecoderObj, mqttBLEMESSAGE_ID, &xValue );
    if ( ( xError != AWS_IOT_SERIALIZER_SUCCESS ) ||
            ( xValue.type != AWS_IOT_SERIALIZER_SCALAR_SIGNED_INT ) )
    {
        AwsIotLogError( "Message ID decode failed, error = %d, decoded value type = %d", xError, xValue.type );
        IOT_BLE_MESG_DECODER.destroy( &xDecoderObj );
        ( *pBytesProcessed ) = 0;
        return AWS_IOT_MQTT_BAD_RESPONSE;
    }
    *pPacketIdentifier = ( uint16_t ) xValue.value.signedInt;

    xError = IOT_BLE_MESG_DECODER.find( &xDecoderObj, mqttBLESTATUS, &xValue );
    if ( ( xError != AWS_IOT_SERIALIZER_SUCCESS ) ||
            ( xValue.type != AWS_IOT_SERIALIZER_SCALAR_SIGNED_INT ) )
    {
        AwsIotLogError( "Status code decode failed, error = %d, decoded value type = %d", xError, xValue.type );
        IOT_BLE_MESG_DECODER.destroy( &xDecoderObj );
        ( *pBytesProcessed ) = 0;
        return AWS_IOT_MQTT_BAD_RESPONSE;
    }
    subscriptionStatus = ( uint16_t ) xValue.value.signedInt;

    IOT_BLE_MESG_DECODER.destroy( &xDecoderObj );

    ( *pBytesProcessed ) = dataLength;

	switch( subscriptionStatus )
	{
	case 0x00:
	case 0x01:
	case 0x02:
		AwsIotLog( AWS_IOT_LOG_DEBUG,
				&_logHideAll,
				"Topic accepted, max QoS %hhu.", subscriptionStatus );
		status = AWS_IOT_MQTT_SUCCESS;
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
		status = AWS_IOT_MQTT_SERVER_REFUSED;
		break;
	default:
		AwsIotLog( AWS_IOT_LOG_DEBUG,
				&_logHideAll,
				"Bad SUBSCRIBE status %hhu.", subscriptionStatus );

		status = AWS_IOT_MQTT_BAD_RESPONSE;
		break;
	}

	return status;
}

AwsIotMqttError_t AwsIotMqttBLE_SerializeUnsubscribe( const AwsIotMqttSubscription_t * const pSubscriptionList,
		size_t subscriptionCount,
		uint8_t ** const pUnsubscribePacket,
		size_t * const pPacketSize,
		uint16_t * const pPacketIdentifier )
{

	uint8_t * pBuffer = NULL;
    size_t xBufLen = 0;
    uint16_t usPacketIdentifier = 0;
    AwsIotSerializerError_t xError;

    usPacketIdentifier = prusNextPacketIdentifier();

    xError = prxSerializeUnSubscribe( pSubscriptionList, subscriptionCount, NULL, &xBufLen, usPacketIdentifier );
    if( xError != AWS_IOT_SERIALIZER_SUCCESS )
    {
        AwsIotLogError( "Failed to find serialized length of UNSUBSCRIBE message, error = %d", xError );
        return AWS_IOT_MQTT_BAD_PARAMETER;
    }

    pBuffer = AwsIotMqtt_MallocMessage( xBufLen );

    /* If Memory cannot be allocated log an error and return */
    if( pBuffer == NULL )
    {
        AwsIotLogError( "Failed to allocate memory for UNSUBSCRIBE message." );
        return AWS_IOT_MQTT_NO_MEMORY;
    }

    xError = prxSerializeUnSubscribe( pSubscriptionList, subscriptionCount, pBuffer, &xBufLen, usPacketIdentifier );
    if( xError != AWS_IOT_SERIALIZER_SUCCESS )
    {
        AwsIotLogError( "Failed to serialize UNSUBSCRIBE message, error = %d", xError );
        return AWS_IOT_MQTT_BAD_PARAMETER;
    }

    *pUnsubscribePacket = pBuffer;
    *pPacketSize = xBufLen;
    *pPacketIdentifier = usPacketIdentifier;

    return AWS_IOT_MQTT_SUCCESS;
}

AwsIotMqttError_t AwsIotMqttBLE_DeserializeUnsuback( const uint8_t * const pUnsubackStart,
                                                          size_t dataLength,
                                                          uint16_t * const pPacketIdentifier,
                                                          size_t * const pBytesProcessed )
{
    AwsIotSerializerDecoderObject_t xDecoderObj, xValue = { 0 };
    AwsIotSerializerError_t xError;

    xError = IOT_BLE_MESG_DECODER.init( &xDecoderObj, ( uint8_t * ) pUnsubackStart, dataLength );

    if( xError != AWS_IOT_SERIALIZER_SUCCESS )
    {
        AwsIotLogError( "Malformed UNSUBACK, decoding the packet failed, decoder error = %d", xError );
        return AWS_IOT_MQTT_BAD_RESPONSE;
    }

    if( xDecoderObj.type != AWS_IOT_SERIALIZER_CONTAINER_MAP )
    {
        AwsIotLogError( "Invalid UNSUBACK, should be a map of key value pairs, decoded object type = %d", xDecoderObj.type );
        IOT_BLE_MESG_DECODER.destroy( &xDecoderObj );
        ( *pBytesProcessed ) = 0;
        return AWS_IOT_MQTT_BAD_RESPONSE;
    }

    xError = IOT_BLE_MESG_DECODER.find( &xDecoderObj, mqttBLEMESSAGE_ID, &xValue );
    if ( ( xError != AWS_IOT_SERIALIZER_SUCCESS ) ||
            ( xValue.type != AWS_IOT_SERIALIZER_SCALAR_SIGNED_INT ) )
    {
        AwsIotLogError( "UNSUBACK Message identifier decode failed, error = %d, decoded value type = %d", xError, xValue.type );
        IOT_BLE_MESG_DECODER.destroy( &xDecoderObj );
        ( *pBytesProcessed ) = 0;
        return AWS_IOT_MQTT_BAD_RESPONSE;
    }
    *pPacketIdentifier = ( uint16_t ) xValue.value.signedInt;

    IOT_BLE_MESG_DECODER.destroy( &xDecoderObj );

    ( *pBytesProcessed ) = dataLength;

	return AWS_IOT_MQTT_SUCCESS;
}

AwsIotMqttError_t AwsIotMqttBLE_SerializeDisconnect( uint8_t ** const pDisconnectPacket,
                                                          size_t * const pPacketSize )
{
	uint8_t *pBuffer = NULL;
	size_t xBufLen = 0;
	AwsIotSerializerError_t xError;

	xError = prxSerializeDisconnect( NULL, &xBufLen);
	if( xError != AWS_IOT_SERIALIZER_SUCCESS )
	{
	    AwsIotLogError( "Failed to find serialized length of DISCONNECT message, error = %d", xError );
	    return AWS_IOT_MQTT_BAD_PARAMETER;
	}

	pBuffer = AwsIotMqtt_MallocMessage( xBufLen );

	/* If Memory cannot be allocated log an error and return */
	if( pBuffer == NULL )
	{
	    AwsIotLogError( "Failed to allocate memory for DISCONNECT message." );
	    return AWS_IOT_MQTT_NO_MEMORY;
	}

	xError = prxSerializeDisconnect( pBuffer, &xBufLen );
	if( xError != AWS_IOT_SERIALIZER_SUCCESS )
	{
	    AwsIotLogError( "Failed to serialize DISCONNECT message, error = %d", xError );
	    return AWS_IOT_MQTT_BAD_PARAMETER;
	}

	*pDisconnectPacket = pBuffer;
	*pPacketSize = xBufLen;

	return AWS_IOT_MQTT_SUCCESS;
}


uint8_t AwsIotMqttBLE_GetPacketType( const uint8_t * const pPacket, size_t packetSize )
{

    AwsIotSerializerDecoderObject_t xDecoderObj, xValue = { 0 };
    AwsIotSerializerError_t xError;
    uint8_t packetType = _INVALID_MQTT_PACKET_TYPE;

    xError = IOT_BLE_MESG_DECODER.init( &xDecoderObj, ( uint8_t* ) pPacket, packetSize );

    if( xError != AWS_IOT_SERIALIZER_SUCCESS )
    {
        AwsIotLogError( "Decoding the packet failed, decoder error = %d", xError );
        return _INVALID_MQTT_PACKET_TYPE;
    }

    if( xDecoderObj.type != AWS_IOT_SERIALIZER_CONTAINER_MAP )
    {
        AwsIotLogError( "Invalid packet, should be a map of key value pairs, decoded object type = %d", xDecoderObj.type );
        IOT_BLE_MESG_DECODER.destroy( &xDecoderObj );
        return _INVALID_MQTT_PACKET_TYPE;
    }

    xError = IOT_BLE_MESG_DECODER.find( &xDecoderObj, mqttBLEMSG_TYPE, &xValue );
    if ( ( xError != AWS_IOT_SERIALIZER_SUCCESS ) ||
            ( xValue.type != AWS_IOT_SERIALIZER_SCALAR_SIGNED_INT ) )
    {
        AwsIotLogError( "Packet type decode failed, error = %d, decoded value type = %d", xError, xValue.type );
        IOT_BLE_MESG_DECODER.destroy( &xDecoderObj );
        return _INVALID_MQTT_PACKET_TYPE;
    }
    packetType = ( uint16_t ) xValue.value.signedInt;


    IOT_BLE_MESG_DECODER.destroy( &xDecoderObj );

    return ( packetType << 4 );
}

AwsIotMqttError_t AwsIotMqttBLE_SerializePingreq( uint8_t ** const pPingreqPacket,
                                                       size_t * const pPacketSize )
{
    return AWS_IOT_MQTT_NO_MEMORY;
}

AwsIotMqttError_t AwsIotMqttBLE_DeserializePingresp( const uint8_t * const pPingrespStart,
                                                          size_t dataLength,
                                                          size_t * const pBytesProcessed )
{
    return AWS_IOT_MQTT_BAD_RESPONSE;
}

void AwsIotMqttBLE_FreePacket( uint8_t * pPacket )
{
    AwsIotMqtt_FreeMessage( pPacket );
}
