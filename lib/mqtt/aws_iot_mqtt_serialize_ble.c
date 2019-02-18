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


#define IS_VALID_SERIALIZER_RET( ret, pxSerializerBuf )                                \
    (  ( ret == AWS_IOT_SERIALIZER_SUCCESS ) ||                                        \
          (  ( !pxSerializerBuf ) && ( ret == AWS_IOT_SERIALIZER_BUFFER_TOO_SMALL ) ) )

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
    if( xError == AWS_IOT_SERIALIZER_SUCCESS )
    {

        xError = IOT_BLE_MESG_ENCODER.openContainer(
                &xEncoderObj,
                &xConnectMap,
                _NUM_CONNECT_PARMAS );
    }

    if( IS_VALID_SERIALIZER_RET( xError, pBuffer ) )
    {
        xData.type = AWS_IOT_SERIALIZER_SCALAR_SIGNED_INT;
        xData.value.signedInt = mqttBLEMSG_TYPE_CONNECT;
        xError = IOT_BLE_MESG_ENCODER.appendKeyValue( &xConnectMap, mqttBLEMSG_TYPE, xData );
    }

    if( IS_VALID_SERIALIZER_RET( xError, pBuffer ) )
    {
        xData.type = AWS_IOT_SERIALIZER_SCALAR_TEXT_STRING;
        xData.value.pString = ( uint8_t * ) pConnectInfo->pClientIdentifier;
        xData.value.stringLength = pConnectInfo->clientIdentifierLength;
        xError = IOT_BLE_MESG_ENCODER.appendKeyValue( &xConnectMap, mqttBLECLIENT_ID, xData );
    }
    if( IS_VALID_SERIALIZER_RET( xError, pBuffer ) )
    {
        xData.type = AWS_IOT_SERIALIZER_SCALAR_TEXT_STRING;
        xData.value.pString = ( uint8_t * ) clientcredentialMQTT_BROKER_ENDPOINT;
        xData.value.stringLength = strlen( clientcredentialMQTT_BROKER_ENDPOINT );
        xError = IOT_BLE_MESG_ENCODER.appendKeyValue( &xConnectMap, mqttBLEBROKER_EP, xData );
    }
    if( IS_VALID_SERIALIZER_RET( xError, pBuffer ) )
    {
        xData.type = AWS_IOT_SERIALIZER_SCALAR_BOOL;
        xData.value.booleanValue = pConnectInfo->cleanSession;
        xError = IOT_BLE_MESG_ENCODER.appendKeyValue( &xConnectMap, mqttBLECLEAN_SESSION, xData );
    }
    if( IS_VALID_SERIALIZER_RET( xError, pBuffer ) )
    {
        xError = IOT_BLE_MESG_ENCODER.closeContainer( &xEncoderObj, &xConnectMap );
    }

    if( IS_VALID_SERIALIZER_RET( xError, pBuffer ) )
    {
        if( pBuffer == NULL )
        {
            *pSize = IOT_BLE_MESG_ENCODER.getExtraBufferSizeNeeded( &xEncoderObj );
        }
        else
        {
            *pSize = IOT_BLE_MESG_ENCODER.getEncodedSize( &xEncoderObj, pBuffer );
        }

        IOT_BLE_MESG_ENCODER.destroy( &xEncoderObj );
        xError = AWS_IOT_SERIALIZER_SUCCESS;

    }

    return xError;
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

    if( xError == AWS_IOT_SERIALIZER_SUCCESS )
    {


        xError = IOT_BLE_MESG_ENCODER.openContainer(
                &xEncoderObj,
                &xPublishMap,
                usNumPublishParams );
    }


    if( IS_VALID_SERIALIZER_RET( xError, pBuffer ) )
    {
        xData.type = AWS_IOT_SERIALIZER_SCALAR_SIGNED_INT;
        xData.value.signedInt = mqttBLEMSG_TYPE_PUBLISH;
        xError = IOT_BLE_MESG_ENCODER.appendKeyValue( &xPublishMap, mqttBLEMSG_TYPE, xData );
    }

    if( IS_VALID_SERIALIZER_RET( xError, pBuffer ) )
    {
        xData.type = AWS_IOT_SERIALIZER_SCALAR_TEXT_STRING;
        xData.value.pString = ( uint8_t * ) pPublishInfo->pTopicName;
        xData.value.stringLength = pPublishInfo->topicNameLength;
        xError = IOT_BLE_MESG_ENCODER.appendKeyValue( &xPublishMap, mqttBLETOPIC, xData );
    }

    if( IS_VALID_SERIALIZER_RET( xError, pBuffer ) )
    {

        xData.type = AWS_IOT_SERIALIZER_SCALAR_SIGNED_INT;
        xData.value.signedInt = pPublishInfo->QoS;
        xError = IOT_BLE_MESG_ENCODER.appendKeyValue( &xPublishMap, mqttBLEQOS, xData );
    }

    if( IS_VALID_SERIALIZER_RET( xError, pBuffer ) )
    {
        xData.type = AWS_IOT_SERIALIZER_SCALAR_BYTE_STRING;
        xData.value.pString = ( uint8_t * ) pPublishInfo->pPayload;
        xData.value.stringLength = pPublishInfo->payloadLength;
        xError = IOT_BLE_MESG_ENCODER.appendKeyValue( &xPublishMap, mqttBLEPAYLOAD, xData );
    }


    if( IS_VALID_SERIALIZER_RET( xError, pBuffer ) )
    {
        if( pPublishInfo->QoS != 0 )
        {
            xData.type = AWS_IOT_SERIALIZER_SCALAR_SIGNED_INT;
            xData.value.signedInt = packetIdentifier;
            xError = IOT_BLE_MESG_ENCODER.appendKeyValue( &xPublishMap, mqttBLEMESSAGE_ID, xData );

        }
    }

    if( IS_VALID_SERIALIZER_RET( xError, pBuffer ) )
    {

        xError = IOT_BLE_MESG_ENCODER.closeContainer( &xEncoderObj, &xPublishMap );
    }

    if( IS_VALID_SERIALIZER_RET( xError, pBuffer ) )
    {
        if( pBuffer == NULL )
        {
            *pSize = IOT_BLE_MESG_ENCODER.getExtraBufferSizeNeeded( &xEncoderObj );

        }
        else
        {
            *pSize = IOT_BLE_MESG_ENCODER.getEncodedSize( &xEncoderObj, pBuffer );
        }
        IOT_BLE_MESG_ENCODER.destroy( &xEncoderObj );
        xError = AWS_IOT_SERIALIZER_SUCCESS;
    }


    return xError;
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

    if( xError == AWS_IOT_SERIALIZER_SUCCESS )
    {

        xError = IOT_BLE_MESG_ENCODER.openContainer(
                &xEncoderObj,
                &xPubAckMap,
                _NUM_PUBACK_PARMAS );
    }

    if( IS_VALID_SERIALIZER_RET( xError, pBuffer ) )
    {
        xData.type = AWS_IOT_SERIALIZER_SCALAR_SIGNED_INT;
        xData.value.signedInt = mqttBLEMSG_TYPE_PUBACK;
        xError = IOT_BLE_MESG_ENCODER.appendKeyValue( &xPubAckMap, mqttBLEMSG_TYPE, xData );
    }

    if( IS_VALID_SERIALIZER_RET( xError, pBuffer ) )
    {
        xData.type = AWS_IOT_SERIALIZER_SCALAR_SIGNED_INT;
        xData.value.signedInt = packetIdentifier;
        xError = IOT_BLE_MESG_ENCODER.appendKeyValue( &xPubAckMap, mqttBLEMESSAGE_ID, xData );
    }
    if( IS_VALID_SERIALIZER_RET( xError, pBuffer ) )
    {
        xError = IOT_BLE_MESG_ENCODER.closeContainer( &xEncoderObj, &xPubAckMap );
    }

    if( IS_VALID_SERIALIZER_RET( xError, pBuffer ) )
    {
        if( pBuffer == NULL )
        {
            *pSize = IOT_BLE_MESG_ENCODER.getExtraBufferSizeNeeded( &xEncoderObj );
        }
        else
        {
            *pSize = IOT_BLE_MESG_ENCODER.getEncodedSize( &xEncoderObj, pBuffer );
        }
        IOT_BLE_MESG_ENCODER.destroy( &xEncoderObj );
        xError = AWS_IOT_SERIALIZER_SUCCESS;
    }

    return xError;
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

    if( xError == AWS_IOT_SERIALIZER_SUCCESS )
    {
        xError = IOT_BLE_MESG_ENCODER.openContainer(
                &xEncoderObj,
                &xSubscribeMap,
                _NUM_SUBACK_PARAMS );
    }

    if( IS_VALID_SERIALIZER_RET( xError, pBuffer ) )
    {
        xData.type = AWS_IOT_SERIALIZER_SCALAR_SIGNED_INT;
        xData.value.signedInt = mqttBLEMSG_TYPE_SUBSCRIBE;
        xError = IOT_BLE_MESG_ENCODER.appendKeyValue( &xSubscribeMap, mqttBLEMSG_TYPE, xData );
    }

    if( IS_VALID_SERIALIZER_RET( xError, pBuffer ) )
    {

        xError = IOT_BLE_MESG_ENCODER.openContainerWithKey(
                &xSubscribeMap,
                mqttBLETOPIC_LIST,
                &xSubscriptionArray,
                subscriptionCount );
    }

    for( usIdx = 0; usIdx < subscriptionCount; usIdx++ )
    {
        if( IS_VALID_SERIALIZER_RET( xError, pBuffer ) )
        {
            xData.type = AWS_IOT_SERIALIZER_SCALAR_TEXT_STRING;
            xData.value.pString = ( uint8_t * ) pSubscriptionList[ usIdx ].pTopicFilter;
            xData.value.stringLength = pSubscriptionList[ usIdx ].topicFilterLength;
            xError = IOT_BLE_MESG_ENCODER.append( &xSubscriptionArray, xData );
        }
        else
        {
            break;
        }
    }

    if( IS_VALID_SERIALIZER_RET( xError, pBuffer ) )
    {
        xError = IOT_BLE_MESG_ENCODER.closeContainer( &xSubscribeMap, &xSubscriptionArray );
    }

    if( IS_VALID_SERIALIZER_RET( xError, pBuffer ) )
    {

        xError = IOT_BLE_MESG_ENCODER.openContainerWithKey(
                &xSubscribeMap,
                mqttBLEQOS_LIST,
                &xSubscriptionArray,
                subscriptionCount );
    }


    for( usIdx = 0; usIdx < subscriptionCount; usIdx++ )
    {
        if( IS_VALID_SERIALIZER_RET( xError, pBuffer ) )
        {

            xData.type = AWS_IOT_SERIALIZER_SCALAR_SIGNED_INT;
            xData.value.signedInt = pSubscriptionList[ usIdx ].QoS;
            xError = IOT_BLE_MESG_ENCODER.append( &xSubscriptionArray, xData );
        }
        else
        {
            break;
        }
    }

    if( IS_VALID_SERIALIZER_RET( xError, pBuffer ) )
    {
        xError = IOT_BLE_MESG_ENCODER.closeContainer( &xSubscribeMap, &xSubscriptionArray );
    }

    if( IS_VALID_SERIALIZER_RET( xError, pBuffer ) )
    {

        xData.type = AWS_IOT_SERIALIZER_SCALAR_SIGNED_INT;
        xData.value.signedInt = packetIdentifier;
        xError = IOT_BLE_MESG_ENCODER.appendKeyValue( &xSubscribeMap, mqttBLEMESSAGE_ID, xData );
    }

    if( IS_VALID_SERIALIZER_RET( xError, pBuffer ) )
    {
        xError = IOT_BLE_MESG_ENCODER.closeContainer( &xEncoderObj, &xSubscribeMap );
    }

    if( IS_VALID_SERIALIZER_RET( xError, pBuffer ) )
    {
        if( pBuffer == NULL )
        {
            *pSize = IOT_BLE_MESG_ENCODER.getExtraBufferSizeNeeded( &xEncoderObj );
        }
        else
        {
            *pSize = IOT_BLE_MESG_ENCODER.getEncodedSize( &xEncoderObj, pBuffer );
        }

        IOT_BLE_MESG_ENCODER.destroy( &xEncoderObj );
        xError = AWS_IOT_SERIALIZER_SUCCESS;
    }
    return xError;
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

    if( xError == AWS_IOT_SERIALIZER_SUCCESS )
    {

        xError = IOT_BLE_MESG_ENCODER.openContainer(
                &xEncoderObj,
                &xSubscribeMap,
                _NUM_UNSUBACK_PARAMS );
    }


    if( IS_VALID_SERIALIZER_RET( xError, pBuffer ) )
    {
        xData.type = AWS_IOT_SERIALIZER_SCALAR_SIGNED_INT;
        xData.value.signedInt = mqttBLEMSG_TYPE_UNSUBSCRIBE;
        xError = IOT_BLE_MESG_ENCODER.appendKeyValue( &xSubscribeMap, mqttBLEMSG_TYPE, xData );
    }

    if( IS_VALID_SERIALIZER_RET( xError, pBuffer ) )
    {
        xError = IOT_BLE_MESG_ENCODER.openContainerWithKey (
                &xSubscribeMap,
                mqttBLETOPIC_LIST,
                &xSubscriptionArray,
                subscriptionCount );
    }

    for( usIdx = 0; usIdx < subscriptionCount; usIdx++ )
    {
        if( IS_VALID_SERIALIZER_RET( xError, pBuffer ) )
        {
            xData.type = AWS_IOT_SERIALIZER_SCALAR_TEXT_STRING;
            xData.value.pString = ( uint8_t * ) pSubscriptionList[ usIdx ].pTopicFilter;
            xData.value.stringLength = pSubscriptionList[ usIdx ].topicFilterLength;
            xError = IOT_BLE_MESG_ENCODER.append( &xSubscriptionArray, xData );
        }
        else
        {
            break;
        }
    }

    if( IS_VALID_SERIALIZER_RET( xError, pBuffer ) )
    {
        xError = IOT_BLE_MESG_ENCODER.closeContainer( &xSubscribeMap, &xSubscriptionArray );
    }

    if( IS_VALID_SERIALIZER_RET( xError, pBuffer ) )
    {
        xData.type = AWS_IOT_SERIALIZER_SCALAR_SIGNED_INT;
        xData.value.signedInt = packetIdentifier;
        xError = IOT_BLE_MESG_ENCODER.appendKeyValue( &xSubscribeMap, mqttBLEMESSAGE_ID, xData );
    }

    if( IS_VALID_SERIALIZER_RET( xError, pBuffer ) )
    {
        xError = IOT_BLE_MESG_ENCODER.closeContainer( &xEncoderObj, &xSubscribeMap );
    }

    if( IS_VALID_SERIALIZER_RET( xError, pBuffer ) )
    {
        if( pBuffer == NULL )
        {
            *pSize = IOT_BLE_MESG_ENCODER.getExtraBufferSizeNeeded( &xEncoderObj );

        }
        else
        {
            *pSize = IOT_BLE_MESG_ENCODER.getEncodedSize( &xEncoderObj, pBuffer );
        }
        IOT_BLE_MESG_ENCODER.destroy( &xEncoderObj );
        xError = AWS_IOT_SERIALIZER_SUCCESS;
    }

    return xError;
}

static AwsIotSerializerError_t prxSerializeDisconnect( uint8_t * const pBuffer,
                                                size_t * const pSize )
{
    AwsIotSerializerError_t xError = AWS_IOT_SERIALIZER_SUCCESS;
    AwsIotSerializerEncoderObject_t xEncoderObj = AWS_IOT_SERIALIZER_ENCODER_CONTAINER_INITIALIZER_STREAM;
    AwsIotSerializerEncoderObject_t xDisconnectMap = AWS_IOT_SERIALIZER_ENCODER_CONTAINER_INITIALIZER_MAP;
    AwsIotSerializerScalarData_t xData = { 0 };

    xError = IOT_BLE_MESG_ENCODER.init( &xEncoderObj, pBuffer, *pSize );

    if( xError == AWS_IOT_SERIALIZER_SUCCESS )
    {
        xError = IOT_BLE_MESG_ENCODER.openContainer(
                &xEncoderObj,
                &xDisconnectMap,
                _NUM_DISCONNECT_PARAMS );
    }

    if( IS_VALID_SERIALIZER_RET( xError, pBuffer ) )
    {
        xData.type = AWS_IOT_SERIALIZER_SCALAR_SIGNED_INT;
        xData.value.signedInt = mqttBLEMSG_TYPE_DISCONNECT;
        xError = IOT_BLE_MESG_ENCODER.appendKeyValue( &xDisconnectMap, mqttBLEMSG_TYPE, xData );
    }

    if( IS_VALID_SERIALIZER_RET( xError, pBuffer ) )
    {
        xError = IOT_BLE_MESG_ENCODER.closeContainer( &xEncoderObj, &xDisconnectMap );
    }

    if( IS_VALID_SERIALIZER_RET( xError, pBuffer ) )
    {
        if( pBuffer == NULL )
        {
            *pSize = IOT_BLE_MESG_ENCODER.getExtraBufferSizeNeeded( &xEncoderObj );
        }
        else
        {
            *pSize = IOT_BLE_MESG_ENCODER.getEncodedSize( &xEncoderObj, pBuffer );
        }
        IOT_BLE_MESG_ENCODER.destroy( &xEncoderObj );
        xError = AWS_IOT_SERIALIZER_SUCCESS;
    }

    return xError;
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
	AwsIotMqttError_t xRet = AWS_IOT_MQTT_SUCCESS;


	xError = prxSerializeConnect( pConnectInfo, pWillInfo, NULL, &xBufLen );
	if( xError != AWS_IOT_SERIALIZER_SUCCESS )
	{
	    AwsIotLogError( "Failed to find length of serialized CONNECT message, error = %d", xError );
	    xRet = AWS_IOT_MQTT_BAD_PARAMETER;
	}

	if( xRet == AWS_IOT_MQTT_SUCCESS )
	{

	    pBuffer = AwsIotMqtt_MallocMessage( xBufLen );

	    /* If Memory cannot be allocated log an error and return */
	    if( pBuffer == NULL )
	    {
	        AwsIotLogError( "Failed to allocate memory for CONNECT packet." );
	        xRet =  AWS_IOT_MQTT_NO_MEMORY;
	    }
	}

	if( xRet == AWS_IOT_MQTT_SUCCESS )
	{
	    xError = prxSerializeConnect( pConnectInfo, pWillInfo, pBuffer, &xBufLen );
	    if( xError != AWS_IOT_SERIALIZER_SUCCESS )
	    {
	        AwsIotLogError( "Failed to serialize CONNECT message, error = %d", xError );
	        xRet = AWS_IOT_MQTT_BAD_PARAMETER;
	    }
	}

	if( xRet == AWS_IOT_MQTT_SUCCESS )
	{
	    *pConnectPacket = pBuffer;
	    *pPacketSize = xBufLen;
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

    return xRet;
}

AwsIotMqttError_t AwsIotMqttBLE_DeserializeConnack( const uint8_t * const pConnackStart,
                                                         size_t dataLength,
                                                         size_t * const pBytesProcessed )
{

    AwsIotSerializerDecoderObject_t xDecoderObj = { 0 }, xValue = { 0 };
    AwsIotSerializerError_t xError;
    AwsIotMqttError_t xRet = AWS_IOT_MQTT_SUCCESS;
    int64_t respCode = 0L;

    ( *pBytesProcessed ) = 0;

    xError = IOT_BLE_MESG_DECODER.init( &xDecoderObj, ( uint8_t * ) pConnackStart, dataLength );
    if( ( xError != AWS_IOT_SERIALIZER_SUCCESS )
            || ( xDecoderObj.type != AWS_IOT_SERIALIZER_CONTAINER_MAP ) )
    {
        AwsIotLogError( "Malformed CONNACK, decoding the packet failed, decoder error = %d, type: %d", xError, xDecoderObj.type );
        xRet = AWS_IOT_MQTT_BAD_RESPONSE;
    }


    if( xRet == AWS_IOT_MQTT_SUCCESS )
    {

        xError = IOT_BLE_MESG_DECODER.find( &xDecoderObj, mqttBLESTATUS, &xValue );
        if ( ( xError != AWS_IOT_SERIALIZER_SUCCESS ) ||
                ( xValue.type != AWS_IOT_SERIALIZER_SCALAR_SIGNED_INT ) )
        {
            AwsIotLogError( "Invalid CONNACK, response code decode failed, error = %d, decoded value type = %d", xError, xValue.type );
            xRet = AWS_IOT_MQTT_BAD_RESPONSE;
        }
        else
        {

            respCode =  xValue.value.signedInt;
            if( ( respCode != eMQTTBLEStatusConnecting )
                    && ( respCode != eMQTTBLEStatusConnected ) )
            {
                xRet = AWS_IOT_MQTT_SERVER_REFUSED;
            }

            ( *pBytesProcessed ) = dataLength;
        }
    }

    IOT_BLE_MESG_DECODER.destroy( &xDecoderObj );

    return xRet;
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
    AwsIotMqttError_t xRet = AWS_IOT_MQTT_SUCCESS;

    if( pPublishInfo->QoS != 0 )
    {
        usPacketIdentifier = prusNextPacketIdentifier();
    }

    xError = prxSerializePublish( pPublishInfo, NULL, &xBufLen, usPacketIdentifier );
    if( xError != AWS_IOT_SERIALIZER_SUCCESS  )
    {
        AwsIotLogError( "Failed to find size of serialized PUBLISH message, error = %d", xError );
        xRet = AWS_IOT_MQTT_BAD_PARAMETER;
    }

    if( xRet == AWS_IOT_MQTT_SUCCESS )
    {

        pBuffer = AwsIotMqtt_MallocMessage( xBufLen );
        /* If Memory cannot be allocated log an error and return */
        if( pBuffer == NULL )
        {
            AwsIotLogError( "Failed to allocate memory for PUBLISH packet." );
            xRet =  AWS_IOT_MQTT_NO_MEMORY;
        }
    }

    if( xRet == AWS_IOT_MQTT_SUCCESS )
    {

        xError = prxSerializePublish( pPublishInfo, pBuffer, &xBufLen, usPacketIdentifier );
        if( xError != AWS_IOT_SERIALIZER_SUCCESS )
        {
            AwsIotLogError( "Failed to serialize PUBLISH message, error = %d", xError );
            xRet = AWS_IOT_MQTT_BAD_PARAMETER;
        }
    }

    if( xRet == AWS_IOT_MQTT_SUCCESS )
    {
        *pPublishPacket = pBuffer;
        *pPacketSize = xBufLen;
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

    return xRet;
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
    uint8_t * pBuffer = NULL;
    size_t xBufLen = 0;
    AwsIotSerializerError_t xError;
    AwsIotMqttError_t xRet = AWS_IOT_MQTT_SUCCESS;

    xError = prxSerializePubAck( packetIdentifier, NULL, &xBufLen );

    if( xError != AWS_IOT_SERIALIZER_SUCCESS )
    {
        AwsIotLogError( "Failed to find size of serialized PUBACK message, error = %d", xError );
        xRet = AWS_IOT_MQTT_BAD_PARAMETER;
    }


    if( xRet == AWS_IOT_MQTT_SUCCESS )
    {
        pBuffer = AwsIotMqtt_MallocMessage( xBufLen );

        /* If Memory cannot be allocated log an error and return */
        if( pBuffer == NULL )
        {
            AwsIotLogError( "Failed to allocate memory for PUBACK packet, packet identifier = %d", packetIdentifier );
            xRet = AWS_IOT_MQTT_NO_MEMORY;
        }
    }


    if( xRet == AWS_IOT_MQTT_SUCCESS )
    {
        xError = prxSerializePubAck( packetIdentifier, pBuffer, &xBufLen );

        if( xError != AWS_IOT_SERIALIZER_SUCCESS )
        {
            AwsIotLogError( "Failed to find size of serialized PUBACK message, error = %d", xError );
            xRet = AWS_IOT_MQTT_BAD_PARAMETER;
        }
    }


    if( xRet == AWS_IOT_MQTT_SUCCESS )
    {
        *pPubackPacket = pBuffer;
        *pPacketSize = xBufLen;
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

	return xRet;

}

AwsIotMqttError_t AwsIotMqttBLE_DeserializePuback( const uint8_t * const pPubackStart,
                                                        size_t dataLength,
                                                        uint16_t * const pPacketIdentifier,
                                                        size_t * const pBytesProcessed )
{

    AwsIotSerializerDecoderObject_t xDecoderObj = { 0 }, xValue = { 0 };
    AwsIotSerializerError_t xError;
    AwsIotMqttError_t xRet = AWS_IOT_MQTT_SUCCESS;

    xError = IOT_BLE_MESG_DECODER.init( &xDecoderObj, ( uint8_t * ) pPubackStart, dataLength );

    if ( ( xError != AWS_IOT_SERIALIZER_SUCCESS )
            || ( xDecoderObj.type != AWS_IOT_SERIALIZER_CONTAINER_MAP ) )
    {
        AwsIotLogError( "Malformed PUBACK, decoding the packet failed, decoder error = %d", xError );
        xRet = AWS_IOT_MQTT_BAD_RESPONSE;
    }


    if( xRet == AWS_IOT_MQTT_SUCCESS )
    {

        xError = IOT_BLE_MESG_DECODER.find( &xDecoderObj, mqttBLEMESSAGE_ID, &xValue );

        if ( ( xError != AWS_IOT_SERIALIZER_SUCCESS ) ||
                ( xValue.type != AWS_IOT_SERIALIZER_SCALAR_SIGNED_INT ) )
        {
            AwsIotLogError( "Message ID decode failed, error = %d, decoded value type = %d", xError, xValue.type );
            xRet = AWS_IOT_MQTT_BAD_RESPONSE;
        }
        else
        {
            *pPacketIdentifier = ( uint16_t ) xValue.value.signedInt;
        }
    }

    if( xRet == AWS_IOT_MQTT_SUCCESS )
    {
        ( *pBytesProcessed ) = dataLength;
    }
    else
    {
        ( *pBytesProcessed )  = 0;
    }

    IOT_BLE_MESG_DECODER.destroy( &xDecoderObj );

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
    AwsIotMqttError_t xRet = AWS_IOT_MQTT_SUCCESS;

    usPacketIdentifier = prusNextPacketIdentifier();

    xError = prxSerializeSubscribe( pSubscriptionList, subscriptionCount, NULL, &xBufLen, usPacketIdentifier );
    if( xError != AWS_IOT_SERIALIZER_SUCCESS )
    {
        AwsIotLogError( "Failed to find serialized length of SUBSCRIBE message, error = %d", xError );
        xRet = AWS_IOT_MQTT_BAD_PARAMETER;
    }

    if( xRet == AWS_IOT_MQTT_SUCCESS )
    {
        pBuffer = AwsIotMqtt_MallocMessage( xBufLen );

        /* If Memory cannot be allocated log an error and return */
        if( pBuffer == NULL )
        {
            AwsIotLogError( "Failed to allocate memory for SUBSCRIBE message." );
            xRet = AWS_IOT_MQTT_NO_MEMORY;
        }
    }

    if( xRet == AWS_IOT_MQTT_SUCCESS )
    {
        xError = prxSerializeSubscribe( pSubscriptionList, subscriptionCount, pBuffer, &xBufLen, usPacketIdentifier );
        if( xError != AWS_IOT_SERIALIZER_SUCCESS )
        {
            AwsIotLogError( "Failed to serialize SUBSCRIBE message, error = %d", xError );
            xRet = AWS_IOT_MQTT_BAD_PARAMETER;
        }
    }

    if( xRet == AWS_IOT_MQTT_SUCCESS )
    {
        *pSubscribePacket = pBuffer;
        *pPacketSize = xBufLen;
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

    return xRet;
}

AwsIotMqttError_t AwsIotMqttBLE_DeserializeSuback( AwsIotMqttConnection_t mqttConnection,
                                                        const uint8_t * const pSubackStart,
                                                        size_t dataLength,
                                                        uint16_t * const pPacketIdentifier,
                                                        size_t * const pBytesProcessed )
{

    AwsIotSerializerDecoderObject_t xDecoderObj = { 0 }, xValue = { 0 };
    AwsIotSerializerError_t xError;
    int64_t subscriptionStatus;
    AwsIotMqttError_t xRet = AWS_IOT_MQTT_SUCCESS;

    ( *pBytesProcessed ) = 0;

    xError = IOT_BLE_MESG_DECODER.init( &xDecoderObj, ( uint8_t * ) pSubackStart, dataLength );

    if ( ( xError != AWS_IOT_SERIALIZER_SUCCESS )
            || ( xDecoderObj.type != AWS_IOT_SERIALIZER_CONTAINER_MAP ) )
    {
        AwsIotLogError( "Malformed SUBACK, decoding the packet failed, decoder error = %d, type: %d", xError, xDecoderObj.type );
        xRet = AWS_IOT_MQTT_BAD_RESPONSE;
    }

    if( xRet == AWS_IOT_MQTT_SUCCESS )
    {

        xError = IOT_BLE_MESG_DECODER.find( &xDecoderObj, mqttBLEMESSAGE_ID, &xValue );
        if ( ( xError != AWS_IOT_SERIALIZER_SUCCESS ) ||
                ( xValue.type != AWS_IOT_SERIALIZER_SCALAR_SIGNED_INT ) )
        {
            AwsIotLogError( "Message ID decode failed, error = %d, decoded value type = %d", xError, xValue.type );
            xRet = AWS_IOT_MQTT_BAD_RESPONSE;
        }
        else
        {
            *pPacketIdentifier = ( uint16_t ) xValue.value.signedInt;
        }
    }

    if( xRet == AWS_IOT_MQTT_SUCCESS )
    {
        xError = IOT_BLE_MESG_DECODER.find( &xDecoderObj, mqttBLESTATUS, &xValue );
        if ( ( xError != AWS_IOT_SERIALIZER_SUCCESS ) ||
                ( xValue.type != AWS_IOT_SERIALIZER_SCALAR_SIGNED_INT ) )
        {
            AwsIotLogError( "Status code decode failed, error = %d, decoded value type = %d", xError, xValue.type );
            xRet = AWS_IOT_MQTT_BAD_RESPONSE;
        }
        else
        {
            subscriptionStatus = ( uint16_t ) xValue.value.signedInt;
            switch( subscriptionStatus )
            {
                case 0x00:
                case 0x01:
                case 0x02:
                    AwsIotLog( AWS_IOT_LOG_DEBUG,
                               &_logHideAll,
                               "Topic accepted, max QoS %hhu.", subscriptionStatus );
                    xRet = AWS_IOT_MQTT_SUCCESS;
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
                    xRet = AWS_IOT_MQTT_SERVER_REFUSED;
                    break;
                default:
                    AwsIotLog( AWS_IOT_LOG_DEBUG,
                               &_logHideAll,
                               "Bad SUBSCRIBE status %hhu.", subscriptionStatus );

                    xRet = AWS_IOT_MQTT_BAD_RESPONSE;
                    break;
            }

            ( *pBytesProcessed ) = dataLength;
        }

    }

    IOT_BLE_MESG_DECODER.destroy( &xDecoderObj );

	return xRet;
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
    AwsIotMqttError_t xRet = AWS_IOT_MQTT_SUCCESS;

    usPacketIdentifier = prusNextPacketIdentifier();

    xError = prxSerializeUnSubscribe( pSubscriptionList, subscriptionCount, NULL, &xBufLen, usPacketIdentifier );
    if( xError != AWS_IOT_SERIALIZER_SUCCESS )
    {
        AwsIotLogError( "Failed to find serialized length of UNSUBSCRIBE message, error = %d", xError );
        xRet = AWS_IOT_MQTT_BAD_PARAMETER;
    }

    if( xRet == AWS_IOT_MQTT_SUCCESS )
    {
        pBuffer = AwsIotMqtt_MallocMessage( xBufLen );

        /* If Memory cannot be allocated log an error and return */
        if( pBuffer == NULL )
        {
            AwsIotLogError( "Failed to allocate memory for UNSUBSCRIBE message." );
            xRet = AWS_IOT_MQTT_NO_MEMORY;
        }
    }

    if( xRet == AWS_IOT_MQTT_SUCCESS )
    {
        xError = prxSerializeUnSubscribe( pSubscriptionList, subscriptionCount, pBuffer, &xBufLen, usPacketIdentifier );
        if( xError != AWS_IOT_SERIALIZER_SUCCESS )
        {
            AwsIotLogError( "Failed to serialize UNSUBSCRIBE message, error = %d", xError );
            xRet = AWS_IOT_MQTT_BAD_PARAMETER;
        }
    }

    if( xRet == AWS_IOT_MQTT_SUCCESS )
    {
        *pUnsubscribePacket = pBuffer;
        *pPacketSize = xBufLen;
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

    return xRet;
}

AwsIotMqttError_t AwsIotMqttBLE_DeserializeUnsuback( const uint8_t * const pUnsubackStart,
                                                          size_t dataLength,
                                                          uint16_t * const pPacketIdentifier,
                                                          size_t * const pBytesProcessed )
{
    AwsIotSerializerDecoderObject_t xDecoderObj = { 0 }, xValue = { 0 };
    AwsIotSerializerError_t xError;
    AwsIotMqttError_t xRet = AWS_IOT_MQTT_SUCCESS;

    xError = IOT_BLE_MESG_DECODER.init( &xDecoderObj, ( uint8_t * ) pUnsubackStart, dataLength );
    if( ( xError != AWS_IOT_SERIALIZER_SUCCESS )
            || ( xDecoderObj.type != AWS_IOT_SERIALIZER_CONTAINER_MAP ) )
    {
        AwsIotLogError( "Malformed UNSUBACK, decoding the packet failed, decoder error = %d, type:%d ", xError, xDecoderObj.type );
        xRet = AWS_IOT_MQTT_BAD_RESPONSE;
    }

    if( xRet == AWS_IOT_MQTT_SUCCESS )
    {
        xError = IOT_BLE_MESG_DECODER.find( &xDecoderObj, mqttBLEMESSAGE_ID, &xValue );
        if ( ( xError != AWS_IOT_SERIALIZER_SUCCESS ) ||
                ( xValue.type != AWS_IOT_SERIALIZER_SCALAR_SIGNED_INT ) )
        {
            AwsIotLogError( "UNSUBACK Message identifier decode failed, error = %d, decoded value type = %d", xError, xValue.type );
            xRet = AWS_IOT_MQTT_BAD_RESPONSE;

        }
        else
        {
            *pPacketIdentifier = ( uint16_t ) xValue.value.signedInt;
        }
    }

    if( xRet == AWS_IOT_MQTT_SUCCESS )
    {
        ( *pBytesProcessed ) = dataLength;
    }
    else
    {
        ( *pBytesProcessed ) = 0;
    }

    IOT_BLE_MESG_DECODER.destroy( &xDecoderObj );

	return AWS_IOT_MQTT_SUCCESS;
}

AwsIotMqttError_t AwsIotMqttBLE_SerializeDisconnect( uint8_t ** const pDisconnectPacket,
                                                          size_t * const pPacketSize )
{
	uint8_t *pBuffer = NULL;
	size_t xBufLen = 0;
	AwsIotSerializerError_t xError;
	AwsIotMqttError_t xRet = AWS_IOT_MQTT_SUCCESS;

	xError = prxSerializeDisconnect( NULL, &xBufLen);
	if( xError != AWS_IOT_SERIALIZER_SUCCESS )
	{
	    AwsIotLogError( "Failed to find serialized length of DISCONNECT message, error = %d", xError );
	    xRet = AWS_IOT_MQTT_BAD_PARAMETER;
	}

	if( xRet == AWS_IOT_MQTT_SUCCESS )
	{
	    pBuffer = AwsIotMqtt_MallocMessage( xBufLen );

	    /* If Memory cannot be allocated log an error and return */
	    if( pBuffer == NULL )
	    {
	        AwsIotLogError( "Failed to allocate memory for DISCONNECT message." );
	        xRet = AWS_IOT_MQTT_NO_MEMORY;
	    }
	}


	if( xRet == AWS_IOT_MQTT_SUCCESS )
	{
	    xError = prxSerializeDisconnect( pBuffer, &xBufLen );
	    if( xError != AWS_IOT_SERIALIZER_SUCCESS )
	    {
	        AwsIotLogError( "Failed to serialize DISCONNECT message, error = %d", xError );
	        xRet = AWS_IOT_MQTT_BAD_PARAMETER;
	    }
	}

	if( xRet == AWS_IOT_MQTT_SUCCESS )
	{
	    *pDisconnectPacket = pBuffer;
	    *pPacketSize = xBufLen;
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

	return xRet;
}


uint8_t AwsIotMqttBLE_GetPacketType( const uint8_t * const pPacket, size_t packetSize )
{

    AwsIotSerializerDecoderObject_t xDecoderObj = { 0 }, xValue = { 0 };
    AwsIotSerializerError_t xError;
    uint8_t value, packetType = _INVALID_MQTT_PACKET_TYPE;

    xError = IOT_BLE_MESG_DECODER.init( &xDecoderObj, ( uint8_t* ) pPacket, packetSize );

    if( ( xError == AWS_IOT_SERIALIZER_SUCCESS )
            && ( xDecoderObj.type == AWS_IOT_SERIALIZER_CONTAINER_MAP ) )
    {

        xError = IOT_BLE_MESG_DECODER.find( &xDecoderObj, mqttBLEMSG_TYPE, &xValue );

        if ( ( xError == AWS_IOT_SERIALIZER_SUCCESS ) &&
                ( xValue.type == AWS_IOT_SERIALIZER_SCALAR_SIGNED_INT ) )
        {
            value = ( uint16_t ) xValue.value.signedInt;

            /** Left shift by 4 bits as MQTT library expects packet type to be upper 4 bits **/
            packetType = value << 4;
        }
        else
        {
            AwsIotLogError( "Packet type decode failed, error = %d, decoded value type = %d", xError, xValue.type );
        }
    }
    else
    {
        AwsIotLogError( "Decoding the packet failed, decoder error = %d, type = %d", xError, xDecoderObj.type );
    }

    IOT_BLE_MESG_DECODER.destroy( &xDecoderObj );

    return packetType;
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
