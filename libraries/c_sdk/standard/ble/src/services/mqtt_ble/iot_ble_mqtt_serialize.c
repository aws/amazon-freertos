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
 * @file iot_ble_mqtt_serialize.c
 * @brief MQTT library for BLE.
 */

/* The config header is always included first. */
#include "iot_config.h"

/* Standard includes. */
#include <string.h>
#include <stdio.h>

/* FreeRTOS includes. */
#include "FreeRTOS.h"
#include "iot_ble_config.h"

/* MQTT internal includes. */
#include "iot_serializer.h"
#include "iot_ble_data_transfer.h"
#include "iot_ble_mqtt_serialize.h"

/**************************************************/
/******* DO NOT CHANGE the following order ********/
/**************************************************/

/* Include logging header files and define logging macros in the following order:
 * 1. Include the header file "logging_levels.h".
 * 2. Define the LIBRARY_LOG_NAME and LIBRARY_LOG_LEVEL macros depending on
 * the logging configuration for DEMO.
 * 3. Include the header file "logging_stack.h", if logging is enabled for DEMO.
 */

#include "logging_levels.h"

/* Logging configuration for the Demo. */
#ifndef LIBRARY_LOG_NAME
    #define LIBRARY_LOG_NAME    "MQTTBLE"
#endif

#ifndef LIBRARY_LOG_LEVEL
    #define LIBRARY_LOG_LEVEL    LOG_INFO
#endif
#include "logging_stack.h"

#define _IS_VALID_SERIALIZER_RET( ret, pSerializerBuf ) \
    ( ( ret == IOT_SERIALIZER_SUCCESS ) ||              \
      ( ( !pSerializerBuf ) && ( ret == IOT_SERIALIZER_BUFFER_TOO_SMALL ) ) )

#define _NUM_CONNECT_PARMAS            ( 4 )
#define _NUM_DEFAULT_PUBLISH_PARMAS    ( 4 )
#define _NUM_PUBACK_PARMAS             ( 2 )
#define _NUM_SUBACK_PARAMS             ( 4 )
#define _NUM_UNSUBACK_PARAMS           ( 3 )
#define _NUM_DISCONNECT_PARAMS         ( 1 )
#define _NUM_PINGREQUEST_PARAMS        ( 1 )


static inline uint16_t _getNumPublishParams( const MQTTBLEPublishInfo_t * const pPublish )
{
    return ( pPublish->qos > 0 ) ? ( _NUM_DEFAULT_PUBLISH_PARMAS + 1 ) : _NUM_DEFAULT_PUBLISH_PARMAS;
}

static IotSerializerError_t _serializeConnect( const MQTTBLEConnectInfo_t * const pConnectInfo,
                                               uint8_t * const pBuffer,
                                               size_t * const pSize );
static IotSerializerError_t _serializePublish( const MQTTBLEPublishInfo_t * const pPublishInfo,
                                               uint8_t * pBuffer,
                                               size_t * pSize,
                                               uint16_t packetIdentifier );
static IotSerializerError_t _serializePubAck( uint16_t packetIdentifier,
                                              uint8_t * pBuffer,
                                              size_t * pSize );

static IotSerializerError_t _serializeSubscribe( const MQTTBLESubscribeInfo_t * const pSubscriptionList,
                                                 size_t subscriptionCount,
                                                 uint8_t * const pBuffer,
                                                 size_t * const pSize,
                                                 uint16_t packetIdentifier );

static IotSerializerError_t _serializeUnSubscribe( const MQTTBLESubscribeInfo_t * const pSubscriptionList,
                                                   size_t subscriptionCount,
                                                   uint8_t * const pBuffer,
                                                   size_t * const pSize,
                                                   uint16_t packetIdentifier );

static IotSerializerError_t _serializeDisconnect( uint8_t * const pBuffer,
                                                  size_t * const pSize );


static IotSerializerError_t _serializePingRequest( uint8_t * const pBuffer,
                                                   size_t * const pSize );


/* Declaration of snprintf. The header stdio.h is not included because it
 * includes conflicting symbols on some platforms. */
extern int snprintf( char * s,
                     size_t n,
                     const char * format,
                     ... );

/*-----------------------------------------------------------*/

static IotSerializerError_t _serializeConnect( const MQTTBLEConnectInfo_t * pConnectInfo,
                                               uint8_t * const pBuffer,
                                               size_t * const pSize )
{
    IotSerializerError_t error = IOT_SERIALIZER_SUCCESS;
    IotSerializerEncoderObject_t encoderObj = IOT_SERIALIZER_ENCODER_CONTAINER_INITIALIZER_STREAM;
    IotSerializerEncoderObject_t connectMap = IOT_SERIALIZER_ENCODER_CONTAINER_INITIALIZER_MAP;
    IotSerializerScalarData_t data = { 0 };

    error = IOT_BLE_MESG_ENCODER.init( &encoderObj, pBuffer, *pSize );

    if( error == IOT_SERIALIZER_SUCCESS )
    {
        error = IOT_BLE_MESG_ENCODER.openContainer(
            &encoderObj,
            &connectMap,
            _NUM_CONNECT_PARMAS );
    }

    if( _IS_VALID_SERIALIZER_RET( error, pBuffer ) )
    {
        data.type = IOT_SERIALIZER_SCALAR_SIGNED_INT;
        data.value.u.signedInt = IOT_BLE_MQTT_MSG_TYPE_CONNECT;
        error = IOT_BLE_MESG_ENCODER.appendKeyValue( &connectMap, IOT_BLE_MQTT_MSG_TYPE, data );
    }

    if( _IS_VALID_SERIALIZER_RET( error, pBuffer ) )
    {
        data.type = IOT_SERIALIZER_SCALAR_TEXT_STRING;
        data.value.u.string.pString = ( uint8_t * ) pConnectInfo->pClientIdentifier;
        data.value.u.string.length = pConnectInfo->clientIdentifierLength;
        error = IOT_BLE_MESG_ENCODER.appendKeyValue( &connectMap, IOT_BLE_MQTT_CLIENT_ID, data );
    }

    if( _IS_VALID_SERIALIZER_RET( error, pBuffer ) )
    {
        data.type = IOT_SERIALIZER_SCALAR_TEXT_STRING;
        data.value.u.string.pString = ( uint8_t * ) clientcredentialMQTT_BROKER_ENDPOINT;
        data.value.u.string.length = strlen( clientcredentialMQTT_BROKER_ENDPOINT );
        error = IOT_BLE_MESG_ENCODER.appendKeyValue( &connectMap, IOT_BLE_MQTT_BROKER_EP, data );
    }

    if( _IS_VALID_SERIALIZER_RET( error, pBuffer ) )
    {
        data.type = IOT_SERIALIZER_SCALAR_BOOL;
        data.value.u.booleanValue = pConnectInfo->cleanSession;
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
        error = IOT_SERIALIZER_SUCCESS;
    }

    return error;
}

static IotSerializerError_t _serializePublish( const MQTTBLEPublishInfo_t * const pPublishInfo,
                                               uint8_t * pBuffer,
                                               size_t * pSize,
                                               uint16_t packetIdentifier )
{
    IotSerializerError_t error = IOT_SERIALIZER_SUCCESS;
    IotSerializerEncoderObject_t encoderObj = IOT_SERIALIZER_ENCODER_CONTAINER_INITIALIZER_STREAM;
    IotSerializerEncoderObject_t publishMap = IOT_SERIALIZER_ENCODER_CONTAINER_INITIALIZER_MAP;
    IotSerializerScalarData_t data = { 0 };
    uint16_t numPublishParams = _getNumPublishParams( pPublishInfo );

    error = IOT_BLE_MESG_ENCODER.init( &encoderObj, pBuffer, *pSize );

    if( error == IOT_SERIALIZER_SUCCESS )
    {
        error = IOT_BLE_MESG_ENCODER.openContainer(
            &encoderObj,
            &publishMap,
            numPublishParams );
    }

    if( _IS_VALID_SERIALIZER_RET( error, pBuffer ) )
    {
        data.type = IOT_SERIALIZER_SCALAR_SIGNED_INT;
        data.value.u.signedInt = IOT_BLE_MQTT_MSG_TYPE_PUBLISH;
        error = IOT_BLE_MESG_ENCODER.appendKeyValue( &publishMap, IOT_BLE_MQTT_MSG_TYPE, data );
    }

    if( _IS_VALID_SERIALIZER_RET( error, pBuffer ) )
    {
        data.type = IOT_SERIALIZER_SCALAR_TEXT_STRING;
        data.value.u.string.pString = ( uint8_t * ) pPublishInfo->pTopicName;
        data.value.u.string.length = pPublishInfo->topicNameLength;
        error = IOT_BLE_MESG_ENCODER.appendKeyValue( &publishMap, IOT_BLE_MQTT_TOPIC, data );
    }

    if( _IS_VALID_SERIALIZER_RET( error, pBuffer ) )
    {
        data.type = IOT_SERIALIZER_SCALAR_SIGNED_INT;
        data.value.u.signedInt = pPublishInfo->qos;
        error = IOT_BLE_MESG_ENCODER.appendKeyValue( &publishMap, IOT_BLE_MQTT_QOS, data );
    }

    if( _IS_VALID_SERIALIZER_RET( error, pBuffer ) )
    {
        data.type = IOT_SERIALIZER_SCALAR_BYTE_STRING;
        data.value.u.string.pString = ( uint8_t * ) pPublishInfo->pPayload;
        data.value.u.string.length = pPublishInfo->payloadLength;
        error = IOT_BLE_MESG_ENCODER.appendKeyValue( &publishMap, IOT_BLE_MQTT_PAYLOAD, data );
    }

    if( _IS_VALID_SERIALIZER_RET( error, pBuffer ) )
    {
        if( pPublishInfo->qos != 0 )
        {
            data.type = IOT_SERIALIZER_SCALAR_SIGNED_INT;
            data.value.u.signedInt = packetIdentifier;
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
        error = IOT_SERIALIZER_SUCCESS;
    }

    return error;
}

static IotSerializerError_t _serializePubAck( uint16_t packetIdentifier,
                                              uint8_t * pBuffer,
                                              size_t * pSize )

{
    IotSerializerError_t error = IOT_SERIALIZER_SUCCESS;
    IotSerializerEncoderObject_t encoderObj = IOT_SERIALIZER_ENCODER_CONTAINER_INITIALIZER_STREAM;
    IotSerializerEncoderObject_t pubAckMap = IOT_SERIALIZER_ENCODER_CONTAINER_INITIALIZER_MAP;
    IotSerializerScalarData_t data = { 0 };

    error = IOT_BLE_MESG_ENCODER.init( &encoderObj, pBuffer, *pSize );

    if( error == IOT_SERIALIZER_SUCCESS )
    {
        error = IOT_BLE_MESG_ENCODER.openContainer(
            &encoderObj,
            &pubAckMap,
            _NUM_PUBACK_PARMAS );
    }

    if( _IS_VALID_SERIALIZER_RET( error, pBuffer ) )
    {
        data.type = IOT_SERIALIZER_SCALAR_SIGNED_INT;
        data.value.u.signedInt = IOT_BLE_MQTT_MSG_TYPE_PUBACK;
        error = IOT_BLE_MESG_ENCODER.appendKeyValue( &pubAckMap, IOT_BLE_MQTT_MSG_TYPE, data );
    }

    if( _IS_VALID_SERIALIZER_RET( error, pBuffer ) )
    {
        data.type = IOT_SERIALIZER_SCALAR_SIGNED_INT;
        data.value.u.signedInt = packetIdentifier;
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
        error = IOT_SERIALIZER_SUCCESS;
    }

    return error;
}


static IotSerializerError_t _serializeSubscribe( const MQTTBLESubscribeInfo_t * const pSubscriptionList,
                                                 size_t subscriptionCount,
                                                 uint8_t * const pBuffer,
                                                 size_t * const pSize,
                                                 uint16_t packetIdentifier )
{
    IotSerializerError_t error = IOT_SERIALIZER_SUCCESS;
    IotSerializerEncoderObject_t encoderObj = IOT_SERIALIZER_ENCODER_CONTAINER_INITIALIZER_STREAM;
    IotSerializerEncoderObject_t subscribeMap = IOT_SERIALIZER_ENCODER_CONTAINER_INITIALIZER_MAP;
    IotSerializerEncoderObject_t subscriptionArray = IOT_SERIALIZER_ENCODER_CONTAINER_INITIALIZER_ARRAY;
    IotSerializerScalarData_t data = { 0 };
    uint16_t idx;

    error = IOT_BLE_MESG_ENCODER.init( &encoderObj, pBuffer, *pSize );

    if( error == IOT_SERIALIZER_SUCCESS )
    {
        error = IOT_BLE_MESG_ENCODER.openContainer(
            &encoderObj,
            &subscribeMap,
            _NUM_SUBACK_PARAMS );
    }

    if( _IS_VALID_SERIALIZER_RET( error, pBuffer ) )
    {
        data.type = IOT_SERIALIZER_SCALAR_SIGNED_INT;
        data.value.u.signedInt = IOT_BLE_MQTT_MSG_TYPE_SUBSCRIBE;
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
            data.type = IOT_SERIALIZER_SCALAR_TEXT_STRING;
            data.value.u.string.pString = ( uint8_t * ) pSubscriptionList[ idx ].pTopicFilter;
            data.value.u.string.length = pSubscriptionList[ idx ].topicFilterLength;
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
            data.type = IOT_SERIALIZER_SCALAR_SIGNED_INT;
            data.value.u.signedInt = pSubscriptionList[ idx ].qos;
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
        data.type = IOT_SERIALIZER_SCALAR_SIGNED_INT;
        data.value.u.signedInt = packetIdentifier;
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
        error = IOT_SERIALIZER_SUCCESS;
    }

    return error;
}

static IotSerializerError_t _serializeUnSubscribe( const MQTTBLESubscribeInfo_t * const pSubscriptionList,
                                                   size_t subscriptionCount,
                                                   uint8_t * const pBuffer,
                                                   size_t * const pSize,
                                                   uint16_t packetIdentifier )
{
    IotSerializerError_t error = IOT_SERIALIZER_SUCCESS;
    IotSerializerEncoderObject_t encoderObj = IOT_SERIALIZER_ENCODER_CONTAINER_INITIALIZER_STREAM;
    IotSerializerEncoderObject_t subscribeMap = IOT_SERIALIZER_ENCODER_CONTAINER_INITIALIZER_MAP;
    IotSerializerEncoderObject_t subscriptionArray = IOT_SERIALIZER_ENCODER_CONTAINER_INITIALIZER_ARRAY;
    IotSerializerScalarData_t data = { 0 };
    uint16_t idx;

    error = IOT_BLE_MESG_ENCODER.init( &encoderObj, pBuffer, *pSize );

    if( error == IOT_SERIALIZER_SUCCESS )
    {
        error = IOT_BLE_MESG_ENCODER.openContainer(
            &encoderObj,
            &subscribeMap,
            _NUM_UNSUBACK_PARAMS );
    }

    if( _IS_VALID_SERIALIZER_RET( error, pBuffer ) )
    {
        data.type = IOT_SERIALIZER_SCALAR_SIGNED_INT;
        data.value.u.signedInt = IOT_BLE_MQTT_MSG_TYPE_UNSUBSCRIBE;
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
            data.type = IOT_SERIALIZER_SCALAR_TEXT_STRING;
            data.value.u.string.pString = ( uint8_t * ) pSubscriptionList[ idx ].pTopicFilter;
            data.value.u.string.length = pSubscriptionList[ idx ].topicFilterLength;
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
        data.type = IOT_SERIALIZER_SCALAR_SIGNED_INT;
        data.value.u.signedInt = packetIdentifier;
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
        error = IOT_SERIALIZER_SUCCESS;
    }

    return error;
}

static IotSerializerError_t _serializeDisconnect( uint8_t * const pBuffer,
                                                  size_t * const pSize )
{
    IotSerializerError_t error = IOT_SERIALIZER_SUCCESS;
    IotSerializerEncoderObject_t encoderObj = IOT_SERIALIZER_ENCODER_CONTAINER_INITIALIZER_STREAM;
    IotSerializerEncoderObject_t disconnectMap = IOT_SERIALIZER_ENCODER_CONTAINER_INITIALIZER_MAP;
    IotSerializerScalarData_t data = { 0 };

    error = IOT_BLE_MESG_ENCODER.init( &encoderObj, pBuffer, *pSize );

    if( error == IOT_SERIALIZER_SUCCESS )
    {
        error = IOT_BLE_MESG_ENCODER.openContainer(
            &encoderObj,
            &disconnectMap,
            _NUM_DISCONNECT_PARAMS );
    }

    if( _IS_VALID_SERIALIZER_RET( error, pBuffer ) )
    {
        data.type = IOT_SERIALIZER_SCALAR_SIGNED_INT;
        data.value.u.signedInt = IOT_BLE_MQTT_MSG_TYPE_DISCONNECT;
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
        error = IOT_SERIALIZER_SUCCESS;
    }

    return error;
}

static IotSerializerError_t _serializePingRequest( uint8_t * const pBuffer,
                                                   size_t * const pSize )
{
    IotSerializerError_t error = IOT_SERIALIZER_SUCCESS;
    IotSerializerEncoderObject_t encoderObj = IOT_SERIALIZER_ENCODER_CONTAINER_INITIALIZER_STREAM;
    IotSerializerEncoderObject_t pingRequest = IOT_SERIALIZER_ENCODER_CONTAINER_INITIALIZER_MAP;
    IotSerializerScalarData_t data = { 0 };

    error = IOT_BLE_MESG_ENCODER.init( &encoderObj, pBuffer, *pSize );

    if( error == IOT_SERIALIZER_SUCCESS )
    {
        error = IOT_BLE_MESG_ENCODER.openContainer(
            &encoderObj,
            &pingRequest,
            _NUM_PINGREQUEST_PARAMS );
    }

    if( _IS_VALID_SERIALIZER_RET( error, pBuffer ) )
    {
        data.type = IOT_SERIALIZER_SCALAR_SIGNED_INT;
        data.value.u.signedInt = IOT_BLE_MQTT_MSG_TYPE_PINGREQ;
        error = IOT_BLE_MESG_ENCODER.appendKeyValue( &pingRequest, IOT_BLE_MQTT_MSG_TYPE, data );
    }

    if( _IS_VALID_SERIALIZER_RET( error, pBuffer ) )
    {
        error = IOT_BLE_MESG_ENCODER.closeContainer( &encoderObj, &pingRequest );
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
        error = IOT_SERIALIZER_SUCCESS;
    }

    return error;
}

MQTTBLEStatus_t IotBleMqtt_SerializeConnect( const MQTTBLEConnectInfo_t * const pConnectInfo,
                                             uint8_t ** const pConnectPacket,
                                             size_t * const pPacketSize )
{
    uint8_t * pBuffer = NULL;
    size_t bufLen = 0;
    IotSerializerError_t error;
    MQTTBLEStatus_t ret = MQTTBLESuccess;


    error = _serializeConnect( pConnectInfo, NULL, &bufLen );

    if( error != IOT_SERIALIZER_SUCCESS )
    {
        LogError( ( "Failed to find length of serialized CONNECT message, error = %d", error ) );
        ret = MQTTBLEBadParameter;
    }

    if( ret == MQTTBLESuccess )
    {
        pBuffer = IotMqtt_MallocMessage( bufLen );

        /* If Memory cannot be allocated log an error and return */
        if( pBuffer == NULL )
        {
            LogError( ( "Failed to allocate memory for CONNECT packet." ) );
            ret = MQTTBLENoMemory;
        }
    }

    if( ret == MQTTBLESuccess )
    {
        error = _serializeConnect( pConnectInfo, pBuffer, &bufLen );

        if( error != IOT_SERIALIZER_SUCCESS )
        {
            LogError( ( "Failed to serialize CONNECT message, error = %d", error ) );
            ret = MQTTBLEBadParameter;
        }
    }

    if( ret == MQTTBLESuccess )
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
            IotMqtt_FreeMessage( pBuffer );
        }
    }

    return ret;
}

MQTTBLEStatus_t IotBleMqtt_DeserializeConnack( const uint8_t * pBuffer,
                                               size_t length )
{
    IotSerializerDecoderObject_t decoderObj = { 0 }, decoderValue = { 0 };
    IotSerializerError_t error;
    MQTTBLEStatus_t ret = MQTTBLESuccess;
    int64_t respCode = 0L;

    error = IOT_BLE_MESG_DECODER.init( &decoderObj, pBuffer, length );

    if( ( error != IOT_SERIALIZER_SUCCESS ) ||
        ( decoderObj.type != IOT_SERIALIZER_CONTAINER_MAP ) )
    {
        LogError( ( "Malformed CONNACK, decoding the packet failed, decoder error = %d, type: %d", error, decoderObj.type ) );
        ret = MQTTBLEBadResponse;
    }

    if( ret == MQTTBLESuccess )
    {
        error = IOT_BLE_MESG_DECODER.find( &decoderObj, IOT_BLE_MQTT_STATUS, &decoderValue );

        if( ( error != IOT_SERIALIZER_SUCCESS ) ||
            ( decoderValue.type != IOT_SERIALIZER_SCALAR_SIGNED_INT ) )
        {
            LogError( ( "Invalid CONNACK, response code decode failed, error = %d, decoded value type = %d", error, decoderValue.type ) );
            ret = MQTTBLEBadResponse;
        }
        else
        {
            respCode = decoderValue.u.value.u.signedInt;

            if( ( respCode != IOT_BLE_MQTT_STATUS_CONNECTING ) &&
                ( respCode != IOT_BLE_MQTT_STATUS_CONNECTED ) )
            {
                ret = MQTTBLEServerRefused;
            }
        }
    }

    IOT_BLE_MESG_DECODER.destroy( &decoderObj );

    return ret;
}
MQTTBLEStatus_t IotBleMqtt_SerializePublish( const MQTTBLEPublishInfo_t * const pPublishInfo,
                                             uint8_t ** const pPublishPacket,
                                             size_t * const pPacketSize,
                                             uint16_t packetIdentifier )
{
    uint8_t * pBuffer = NULL;
    size_t bufLen = 0;
    IotSerializerError_t error;
    MQTTBLEStatus_t ret = MQTTBLESuccess;

    error = _serializePublish( pPublishInfo, NULL, &bufLen, packetIdentifier );

    if( error != IOT_SERIALIZER_SUCCESS )
    {
        LogError( ( "Failed to find size of serialized PUBLISH message, error = %d", error ) );
        ret = MQTTBLEBadParameter;
    }

    if( ret == MQTTBLESuccess )
    {
        pBuffer = IotMqtt_MallocMessage( bufLen );

        /* If Memory cannot be allocated log an error and return */
        if( pBuffer == NULL )
        {
            LogError( ( "Failed to allocate memory for PUBLISH packet." ) );
            ret = MQTTBLENoMemory;
        }
    }

    if( ret == MQTTBLESuccess )
    {
        error = _serializePublish( pPublishInfo, pBuffer, &bufLen, packetIdentifier );

        if( error != IOT_SERIALIZER_SUCCESS )
        {
            LogError( ( "Failed to serialize PUBLISH message, error = %d", error ) );
            ret = MQTTBLEBadParameter;
        }
    }

    if( ret == MQTTBLESuccess )
    {
        *pPublishPacket = pBuffer;
        *pPacketSize = bufLen;
    }
    else
    {
        if( pBuffer != NULL )
        {
            IotMqtt_FreeMessage( pBuffer );
        }

        *pPublishPacket = NULL;
        *pPacketSize = 0;
    }

    return ret;
}

void IotBleMqtt_PublishSetDup( uint8_t * const pPublishPacket,
                               uint8_t * pPacketIdentifierHigh,
                               uint16_t * const pNewPacketIdentifier )
{
    /** TODO: Currently DUP flag is not supported by BLE SDKs **/
}

MQTTBLEStatus_t IotBleMqtt_DeserializePublish( uint8_t * pBuffer,
                                               size_t length,
                                               MQTTBLEPublishInfo_t * publishInfo,
                                               uint16_t * packetIdentifier )
{
    IotSerializerDecoderObject_t decoderObj = { 0 }, decoderValue = { 0 };
    IotSerializerError_t xSerializerRet;
    MQTTBLEStatus_t ret = MQTTBLESuccess;

    xSerializerRet = IOT_BLE_MESG_DECODER.init( &decoderObj, pBuffer, length );

    if( ( xSerializerRet != IOT_SERIALIZER_SUCCESS ) ||
        ( decoderObj.type != IOT_SERIALIZER_CONTAINER_MAP ) )
    {
        LogError( ( "Decoding PUBLISH packet failed, decoder error = %d, object type = %d", xSerializerRet, decoderObj.type ) );
        ret = MQTTBLEBadResponse;
    }

    if( ret == MQTTBLESuccess )
    {
        xSerializerRet = IOT_BLE_MESG_DECODER.find( &decoderObj, IOT_BLE_MQTT_QOS, &decoderValue );

        if( ( xSerializerRet != IOT_SERIALIZER_SUCCESS ) ||
            ( decoderValue.type != IOT_SERIALIZER_SCALAR_SIGNED_INT ) )
        {
            LogError( ( "QOS Value decode failed, error = %d, decoded value type = %d", xSerializerRet, decoderValue.type ) );
            ret = MQTTBLEBadResponse;
        }
        else
        {
            publishInfo->qos = decoderValue.u.value.u.signedInt;
        }
    }

    if( ret == MQTTBLESuccess )
    {
        decoderValue.u.value.u.string.pString = NULL;
        decoderValue.u.value.u.string.length = 0;
        xSerializerRet = IOT_BLE_MESG_DECODER.find( &decoderObj, IOT_BLE_MQTT_TOPIC, &decoderValue );

        if( ( xSerializerRet != IOT_SERIALIZER_SUCCESS ) ||
            ( decoderValue.type != IOT_SERIALIZER_SCALAR_TEXT_STRING ) )
        {
            LogError( ( "Topic value decode failed, error = %d", xSerializerRet ) );
            ret = MQTTBLEBadResponse;
        }
        else
        {
            publishInfo->pTopicName = ( const char * ) decoderValue.u.value.u.string.pString;
            publishInfo->topicNameLength = decoderValue.u.value.u.string.length;
        }
    }

    if( ret == MQTTBLESuccess )
    {
        decoderValue.u.value.u.string.pString = NULL;
        decoderValue.u.value.u.string.length = 0;
        xSerializerRet = IOT_BLE_MESG_DECODER.find( &decoderObj, IOT_BLE_MQTT_PAYLOAD, &decoderValue );

        if( ( xSerializerRet != IOT_SERIALIZER_SUCCESS ) ||
            ( decoderValue.type != IOT_SERIALIZER_SCALAR_BYTE_STRING ) )
        {
            LogError( ( "Payload value decode failed, error = %d", xSerializerRet ) );
            ret = MQTTBLEBadResponse;
        }
        else
        {
            publishInfo->pPayload = ( const char * ) decoderValue.u.value.u.string.pString;
            publishInfo->payloadLength = decoderValue.u.value.u.string.length;
        }
    }

    if( ret == MQTTBLESuccess )
    {
        if( publishInfo->qos != 0 )
        {
            xSerializerRet = IOT_BLE_MESG_DECODER.find( &decoderObj, IOT_BLE_MQTT_MESSAGE_ID, &decoderValue );

            if( ( xSerializerRet != IOT_SERIALIZER_SUCCESS ) ||
                ( decoderValue.type != IOT_SERIALIZER_SCALAR_SIGNED_INT ) )
            {
                LogError( ( "Message identifier decode failed, error = %d, decoded value type = %d", xSerializerRet, decoderValue.type ) );
                ret = MQTTBLEBadResponse;
            }
            else
            {
                *packetIdentifier = ( uint16_t ) decoderValue.u.value.u.signedInt;
            }
        }
    }

    if( ret == MQTTBLESuccess )
    {
        publishInfo->retain = false;
    }

    IOT_BLE_MESG_DECODER.destroy( &decoderObj );

    return ret;
}

MQTTBLEStatus_t IotBleMqtt_SerializePuback( uint16_t packetIdentifier,
                                            uint8_t ** const pPubackPacket,
                                            size_t * const pPacketSize )
{
    uint8_t * pBuffer = NULL;
    size_t bufLen = 0;
    IotSerializerError_t error;
    MQTTBLEStatus_t ret = MQTTBLESuccess;

    error = _serializePubAck( packetIdentifier, NULL, &bufLen );

    if( error != IOT_SERIALIZER_SUCCESS )
    {
        LogError( ( "Failed to find size of serialized PUBACK message, error = %d", error ) );
        ret = MQTTBLEBadParameter;
    }

    if( ret == MQTTBLESuccess )
    {
        pBuffer = IotMqtt_MallocMessage( bufLen );

        /* If Memory cannot be allocated log an error and return */
        if( pBuffer == NULL )
        {
            LogError( ( "Failed to allocate memory for PUBACK packet, packet identifier = %d", packetIdentifier ) );
            ret = MQTTBLENoMemory;
        }
    }

    if( ret == MQTTBLESuccess )
    {
        error = _serializePubAck( packetIdentifier, pBuffer, &bufLen );

        if( error != IOT_SERIALIZER_SUCCESS )
        {
            LogError( ( "Failed to find size of serialized PUBACK message, error = %d", error ) );
            ret = MQTTBLEBadParameter;
        }
    }

    if( ret == MQTTBLESuccess )
    {
        *pPubackPacket = pBuffer;
        *pPacketSize = bufLen;
    }
    else
    {
        if( pBuffer != NULL )
        {
            IotMqtt_FreeMessage( pBuffer );
        }

        *pPubackPacket = NULL;
        *pPacketSize = 0;
    }

    return ret;
}

MQTTBLEStatus_t IotBleMqtt_DeserializePuback( uint8_t * pBuffer,
                                              size_t length,
                                              uint16_t * packetIdentifier )
{
    IotSerializerDecoderObject_t decoderObj = { 0 }, decoderValue = { 0 };
    IotSerializerError_t error;
    MQTTBLEStatus_t ret = MQTTBLESuccess;

    error = IOT_BLE_MESG_DECODER.init( &decoderObj, pBuffer, length );

    if( ( error != IOT_SERIALIZER_SUCCESS ) ||
        ( decoderObj.type != IOT_SERIALIZER_CONTAINER_MAP ) )
    {
        LogError( ( "Malformed PUBACK, decoding the packet failed, decoder error = %d, object type: %d", error, decoderObj.type ) );
        ret = MQTTBLEBadResponse;
    }

    if( ret == MQTTBLESuccess )
    {
        error = IOT_BLE_MESG_DECODER.find( &decoderObj, IOT_BLE_MQTT_MESSAGE_ID, &decoderValue );

        if( ( error != IOT_SERIALIZER_SUCCESS ) ||
            ( decoderValue.type != IOT_SERIALIZER_SCALAR_SIGNED_INT ) )
        {
            LogError( ( "Message ID decode failed, error = %d, decoded value type = %d", error, decoderValue.type ) );
            ret = MQTTBLEBadResponse;
        }
        else
        {
            *packetIdentifier = ( uint16_t ) decoderValue.u.value.u.signedInt;
        }
    }

    IOT_BLE_MESG_DECODER.destroy( &decoderObj );

    return ret;
}

MQTTBLEStatus_t IotBleMqtt_SerializeSubscribe( const MQTTBLESubscribeInfo_t * const pSubscriptionList,
                                               size_t subscriptionCount,
                                               uint8_t ** const pSubscribePacket,
                                               size_t * const pPacketSize,
                                               uint16_t * const pPacketIdentifier )
{
    uint8_t * pBuffer = NULL;
    size_t bufLen = 0;
    IotSerializerError_t error;
    MQTTBLEStatus_t ret = MQTTBLESuccess;

    error = _serializeSubscribe( pSubscriptionList, subscriptionCount, NULL, &bufLen, *pPacketIdentifier );

    if( error != IOT_SERIALIZER_SUCCESS )
    {
        LogError( ( "Failed to find serialized length of SUBSCRIBE message, error = %d", error ) );
        ret = MQTTBLEBadParameter;
    }

    if( ret == MQTTBLESuccess )
    {
        pBuffer = IotMqtt_MallocMessage( bufLen );

        /* If Memory cannot be allocated log an error and return */
        if( pBuffer == NULL )
        {
            LogError( ( "Failed to allocate memory for SUBSCRIBE message." ) );
            ret = MQTTBLENoMemory;
        }
    }

    if( ret == MQTTBLESuccess )
    {
        error = _serializeSubscribe( pSubscriptionList, subscriptionCount, pBuffer, &bufLen, *pPacketIdentifier );

        if( error != IOT_SERIALIZER_SUCCESS )
        {
            LogError( ( "Failed to serialize SUBSCRIBE message, error = %d", error ) );
            ret = MQTTBLEBadParameter;
        }
    }

    if( ret == MQTTBLESuccess )
    {
        *pSubscribePacket = pBuffer;
        *pPacketSize = bufLen;
    }
    else
    {
        if( pBuffer != NULL )
        {
            IotMqtt_FreeMessage( pBuffer );
        }

        *pSubscribePacket = NULL;
        *pPacketSize = 0;
    }

    return ret;
}

MQTTBLEStatus_t IotBleMqtt_DeserializeSuback( const uint8_t * pBuffer,
                                              size_t length,
                                              uint16_t * packetIdentifier,
                                              uint8_t * pStatusCode )
{
    IotSerializerDecoderObject_t decoderObj = { 0 }, decoderValue = { 0 };
    IotSerializerError_t error;
    MQTTBLEStatus_t ret = MQTTBLESuccess;

    error = IOT_BLE_MESG_DECODER.init( &decoderObj, pBuffer, length );

    if( ( error != IOT_SERIALIZER_SUCCESS ) ||
        ( decoderObj.type != IOT_SERIALIZER_CONTAINER_MAP ) )
    {
        LogError( ( "Malformed SUBACK, decoding the packet failed, decoder error = %d, type: %d", error, decoderObj.type ) );
        ret = MQTTBLEBadResponse;
    }

    if( ret == MQTTBLESuccess )
    {
        error = IOT_BLE_MESG_DECODER.find( &decoderObj, IOT_BLE_MQTT_MESSAGE_ID, &decoderValue );

        if( ( error != IOT_SERIALIZER_SUCCESS ) ||
            ( decoderValue.type != IOT_SERIALIZER_SCALAR_SIGNED_INT ) )
        {
            LogError( ( "Message ID decode failed, error = %d, decoded value type = %d", error, decoderValue.type ) );
            ret = MQTTBLEBadResponse;
        }
        else
        {
            *packetIdentifier = ( uint16_t ) decoderValue.u.value.u.signedInt;
        }
    }

    if( ret == MQTTBLESuccess )
    {
        error = IOT_BLE_MESG_DECODER.find( &decoderObj, IOT_BLE_MQTT_STATUS, &decoderValue );

        if( ( error != IOT_SERIALIZER_SUCCESS ) ||
            ( decoderValue.type != IOT_SERIALIZER_SCALAR_SIGNED_INT ) )
        {
            LogError( ( "Status code decode failed, error = %d, decoded value type = %d", error, decoderValue.type ) );
            ret = MQTTBLEBadResponse;
        }
        else
        {
            *pStatusCode = ( uint8_t ) ( decoderValue.u.value.u.signedInt );
        }
    }

    IOT_BLE_MESG_DECODER.destroy( &decoderObj );

    return ret;
}

MQTTBLEStatus_t IotBleMqtt_SerializeUnsubscribe( const MQTTBLESubscribeInfo_t * const pSubscriptionList,
                                                 size_t subscriptionCount,
                                                 uint8_t ** const pUnsubscribePacket,
                                                 size_t * const pPacketSize,
                                                 uint16_t * const pPacketIdentifier )
{
    uint8_t * pBuffer = NULL;
    size_t bufLen = 0;
    IotSerializerError_t error;
    MQTTBLEStatus_t ret = MQTTBLESuccess;

    error = _serializeUnSubscribe( pSubscriptionList, subscriptionCount, NULL, &bufLen, *pPacketIdentifier );

    if( error != IOT_SERIALIZER_SUCCESS )
    {
        LogError( ( "Failed to find serialized length of UNSUBSCRIBE message, error = %d", error ) );
        ret = MQTTBLEBadParameter;
    }

    if( ret == MQTTBLESuccess )
    {
        pBuffer = IotMqtt_MallocMessage( bufLen );

        /* If Memory cannot be allocated log an error and return */
        if( pBuffer == NULL )
        {
            LogError( ( "Failed to allocate memory for UNSUBSCRIBE message." ) );
            ret = MQTTBLENoMemory;
        }
    }

    if( ret == MQTTBLESuccess )
    {
        error = _serializeUnSubscribe( pSubscriptionList, subscriptionCount, pBuffer, &bufLen, *pPacketIdentifier );

        if( error != IOT_SERIALIZER_SUCCESS )
        {
            LogError( ( "Failed to serialize UNSUBSCRIBE message, error = %d", error ) );
            ret = MQTTBLEBadParameter;
        }
    }

    if( ret == MQTTBLESuccess )
    {
        *pUnsubscribePacket = pBuffer;
        *pPacketSize = bufLen;
    }
    else
    {
        if( pBuffer != NULL )
        {
            IotMqtt_FreeMessage( pBuffer );
        }

        *pUnsubscribePacket = NULL;
        *pPacketSize = 0;
    }

    return ret;
}

MQTTBLEStatus_t IotBleMqtt_DeserializeUnsuback( uint8_t * pBuffer,
                                                size_t length,
                                                uint16_t * packetIdentifier )
{
    IotSerializerDecoderObject_t decoderObj = { 0 }, decoderValue = { 0 };
    IotSerializerError_t error;
    MQTTBLEStatus_t ret = MQTTBLESuccess;

    error = IOT_BLE_MESG_DECODER.init( &decoderObj, pBuffer, length );

    if( ( error != IOT_SERIALIZER_SUCCESS ) ||
        ( decoderObj.type != IOT_SERIALIZER_CONTAINER_MAP ) )
    {
        LogError( ( "Malformed UNSUBACK, decoding the packet failed, decoder error = %d, type:%d ", error, decoderObj.type ) );
        ret = MQTTBLEBadResponse;
    }

    if( ret == MQTTBLESuccess )
    {
        error = IOT_BLE_MESG_DECODER.find( &decoderObj, IOT_BLE_MQTT_MESSAGE_ID, &decoderValue );

        if( ( error != IOT_SERIALIZER_SUCCESS ) ||
            ( decoderValue.type != IOT_SERIALIZER_SCALAR_SIGNED_INT ) )
        {
            LogError( ( "UNSUBACK Message identifier decode failed, error = %d, decoded value type = %d", error, decoderValue.type ) );
            ret = MQTTBLEBadResponse;
        }
        else
        {
            *packetIdentifier = ( uint16_t ) decoderValue.u.value.u.signedInt;
        }
    }

    IOT_BLE_MESG_DECODER.destroy( &decoderObj );

    return ret;
}

MQTTBLEStatus_t IotBleMqtt_SerializeDisconnect( uint8_t ** const pDisconnectPacket,
                                                size_t * const pPacketSize )
{
    uint8_t * pBuffer = NULL;
    size_t bufLen = 0;
    IotSerializerError_t error;
    MQTTBLEStatus_t ret = MQTTBLESuccess;

    error = _serializeDisconnect( NULL, &bufLen );

    if( error != IOT_SERIALIZER_SUCCESS )
    {
        LogError( ( "Failed to find serialized length of DISCONNECT message, error = %d", error ) );
        ret = MQTTBLEBadParameter;
    }

    if( ret == MQTTBLESuccess )
    {
        pBuffer = IotMqtt_MallocMessage( bufLen );

        /* If Memory cannot be allocated log an error and return */
        if( pBuffer == NULL )
        {
            LogError( ( "Failed to allocate memory for DISCONNECT message." ) );
            ret = MQTTBLENoMemory;
        }
    }

    if( ret == MQTTBLESuccess )
    {
        error = _serializeDisconnect( pBuffer, &bufLen );

        if( error != IOT_SERIALIZER_SUCCESS )
        {
            LogError( ( "Failed to serialize DISCONNECT message, error = %d", error ) );
            ret = MQTTBLEBadParameter;
        }
    }

    if( ret == MQTTBLESuccess )
    {
        *pDisconnectPacket = pBuffer;
        *pPacketSize = bufLen;
    }
    else
    {
        if( pBuffer != NULL )
        {
            IotMqtt_FreeMessage( pBuffer );
        }

        *pDisconnectPacket = NULL;
        *pPacketSize = 0;
    }

    return ret;
}

size_t IotBleMqtt_GetRemainingLength( IotBleDataTransferChannel_t * pNetworkConnection )
{
    const uint8_t * pBuffer;
    size_t length;

    IotBleDataTransfer_PeekReceiveBuffer( pNetworkConnection, &pBuffer, &length );

    return length;
}


uint8_t IotBleMqtt_GetPacketType( const uint8_t * pBuffer,
                                  size_t length )
{
    IotSerializerDecoderObject_t decoderObj = { 0 }, decoderValue = { 0 };
    IotSerializerError_t error;
    uint8_t value, packetType = IOT_BLE_MQTT_MSG_TYPE_INVALID;

    error = IOT_BLE_MESG_DECODER.init( &decoderObj, pBuffer, length );

    if( ( error == IOT_SERIALIZER_SUCCESS ) &&
        ( decoderObj.type == IOT_SERIALIZER_CONTAINER_MAP ) )
    {
        error = IOT_BLE_MESG_DECODER.find( &decoderObj, IOT_BLE_MQTT_MSG_TYPE, &decoderValue );

        if( ( error == IOT_SERIALIZER_SUCCESS ) &&
            ( decoderValue.type == IOT_SERIALIZER_SCALAR_SIGNED_INT ) )
        {
            value = ( uint16_t ) decoderValue.u.value.u.signedInt;
            packetType = value;
        }
        else
        {
            LogError( ( "Packet type decode failed, error = %d, decoded value type = %d", error, decoderValue.type ) );
        }
    }
    else
    {
        LogError( ( "Decoding the packet failed, decoder error = %d, type = %d", error, decoderObj.type ) );
    }

    IOT_BLE_MESG_DECODER.destroy( &decoderObj );

    return packetType;
}

MQTTBLEStatus_t IotBleMqtt_SerializePingreq( uint8_t ** const pPingreqPacket,
                                             size_t * const pPacketSize )
{
    uint8_t * pBuffer = NULL;
    size_t bufLen = 0;
    IotSerializerError_t error;
    MQTTBLEStatus_t ret = MQTTBLESuccess;

    error = _serializePingRequest( NULL, &bufLen );

    if( error != IOT_SERIALIZER_SUCCESS )
    {
        LogError( ( "Failed to find serialized length of DISCONNECT message, error = %d", error ) );
        ret = MQTTBLEBadParameter;
    }

    if( ret == MQTTBLESuccess )
    {
        pBuffer = IotMqtt_MallocMessage( bufLen );

        /* If Memory cannot be allocated log an error and return */
        if( pBuffer == NULL )
        {
            LogError( ( "Failed to allocate memory for DISCONNECT message." ) );
            ret = MQTTBLENoMemory;
        }
    }

    if( ret == MQTTBLESuccess )
    {
        error = _serializePingRequest( pBuffer, &bufLen );

        if( error != IOT_SERIALIZER_SUCCESS )
        {
            LogError( ( "Failed to serialize DISCONNECT message, error = %d", error ) );
            ret = MQTTBLEBadParameter;
        }
    }

    if( ret == MQTTBLESuccess )
    {
        *pPingreqPacket = pBuffer;
        *pPacketSize = bufLen;
    }
    else
    {
        if( pBuffer != NULL )
        {
            IotMqtt_FreeMessage( pBuffer );
        }

        *pPingreqPacket = NULL;
        *pPacketSize = 0;
    }

    return ret;
}

MQTTBLEStatus_t IotBleMqtt_DeserializePingresp( const uint8_t * pBuffer,
                                                size_t length )
{
    /* Ping Response for BLE contains only packet type field in CBOR, which is already decoded
     * in IotBleMqtt_GetPacketType() function. Returning MQTTSuccess. */
    return MQTTBLESuccess;
}

void IotBleMqtt_FreePacket( uint8_t * pPacket )
{
    IotMqtt_FreeMessage( pPacket );
}
