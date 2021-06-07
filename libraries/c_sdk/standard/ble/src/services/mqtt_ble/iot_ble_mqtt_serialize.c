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
#include "iot_ble_data_transfer.h"
#include "iot_ble_mqtt_serialize.h"
#include "cbor.h"

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

#define SERIALIZER_STATUS( status, pBuffer ) \
    ( ( status == CborNoError ) ||           \
      ( ( !pBuffer ) && ( status == CborErrorOutOfMemory ) ) )

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

static CborError prvSerializeConnect( const MQTTBLEConnectInfo_t * const pConnectInfo,
                                      uint8_t * const pBuffer,
                                      size_t * const pSize );
static CborError prvSerializePublish( const MQTTBLEPublishInfo_t * const pPublishInfo,
                                      uint8_t * pBuffer,
                                      size_t * pSize,
                                      uint16_t packetIdentifier );
static CborError prvSerializePubAck( uint16_t packetIdentifier,
                                     uint8_t * pBuffer,
                                     size_t * pSize );

static CborError prvSerializeSubscribe( const MQTTBLESubscribeInfo_t * const pSubscriptionList,
                                        size_t subscriptionCount,
                                        uint8_t * const pBuffer,
                                        size_t * const pSize,
                                        uint16_t packetIdentifier );

static CborError prvSerializeUnSubscribe( const MQTTBLESubscribeInfo_t * const pSubscriptionList,
                                          size_t subscriptionCount,
                                          uint8_t * const pBuffer,
                                          size_t * const pSize,
                                          uint16_t packetIdentifier );

static CborError prvSerializeDisconnect( uint8_t * const pBuffer,
                                         size_t * const pSize );


static CborError prvSerializePingRequest( uint8_t * const pBuffer,
                                          size_t * const pSize );

/*-----------------------------------------------------------*/

static CborError prvSerializeConnect( const MQTTBLEConnectInfo_t * pConnectInfo,
                                      uint8_t * const pBuffer,
                                      size_t * const pLength )
{
    size_t length = *pLength;
    CborError status = CborErrorInternalError, result = CborErrorInternalError;
    CborEncoder encoder = { 0 }, mapEncoder = { 0 };

    cbor_encoder_init( &encoder, pBuffer, length, 0 );

    status = cbor_encoder_create_map( &encoder, &mapEncoder, _NUM_CONNECT_PARMAS );

    if( SERIALIZER_STATUS( status, pBuffer ) == true )
    {
        status = cbor_encode_text_string( &mapEncoder, IOT_BLE_MQTT_MSG_TYPE, strlen( IOT_BLE_MQTT_MSG_TYPE ) );

        if( SERIALIZER_STATUS( status, pBuffer ) == true )
        {
            status = cbor_encode_int( &mapEncoder, IOT_BLE_MQTT_MSG_TYPE_CONNECT );
        }
    }

    if( SERIALIZER_STATUS( status, pBuffer ) == true )
    {
        status = cbor_encode_text_string( &mapEncoder, IOT_BLE_MQTT_CLIENT_ID, strlen( IOT_BLE_MQTT_CLIENT_ID ) );

        if( SERIALIZER_STATUS( status, pBuffer ) == true )
        {
            status = cbor_encode_text_string( &mapEncoder, pConnectInfo->pClientIdentifier, pConnectInfo->clientIdentifierLength );
        }
    }

    if( SERIALIZER_STATUS( status, pBuffer ) == true )
    {
        status = cbor_encode_text_string( &mapEncoder, IOT_BLE_MQTT_BROKER_EP, strlen( IOT_BLE_MQTT_BROKER_EP ) );

        if( SERIALIZER_STATUS( status, pBuffer ) == true )
        {
            status = cbor_encode_text_string( &mapEncoder, clientcredentialMQTT_BROKER_ENDPOINT, strlen( clientcredentialMQTT_BROKER_ENDPOINT ) );
        }
    }

    if( SERIALIZER_STATUS( status, pBuffer ) == true )
    {
        status = cbor_encode_text_string( &mapEncoder, IOT_BLE_MQTT_CLEAN_SESSION, strlen( IOT_BLE_MQTT_CLEAN_SESSION ) );

        if( SERIALIZER_STATUS( status, pBuffer ) == true )
        {
            status = cbor_encode_boolean( &mapEncoder, pConnectInfo->cleanSession );
        }
    }

    if( SERIALIZER_STATUS( status, pBuffer ) == true )
    {
        status = cbor_encoder_close_container( &encoder, &mapEncoder );
    }

    if( pBuffer == NULL )
    {
        *pLength = cbor_encoder_get_extra_bytes_needed( &encoder );
    }
    else
    {
        *pLength = cbor_encoder_get_buffer_size( &encoder, pBuffer );
    }

    if( SERIALIZER_STATUS( status, pBuffer ) == true )
    {
        result = CborNoError;
    }
    else
    {
        result = status;
    }

    return result;
}

static CborError prvSerializePublish( const MQTTBLEPublishInfo_t * const pPublishInfo,
                                      uint8_t * pBuffer,
                                      size_t * pLength,
                                      uint16_t packetIdentifier )
{
    size_t length = *pLength;
    CborError status = CborErrorInternalError, result = CborErrorInternalError;
    CborEncoder encoder = { 0 }, mapEncoder = { 0 };
    uint16_t numPublishParams = _getNumPublishParams( pPublishInfo );

    cbor_encoder_init( &encoder, pBuffer, length, 0 );

    status = cbor_encoder_create_map( &encoder, &mapEncoder, numPublishParams );

    if( SERIALIZER_STATUS( status, pBuffer ) == true )
    {
        status = cbor_encode_text_string( &mapEncoder, IOT_BLE_MQTT_MSG_TYPE, strlen( IOT_BLE_MQTT_MSG_TYPE ) );

        if( SERIALIZER_STATUS( status, pBuffer ) == true )
        {
            status = cbor_encode_int( &mapEncoder, IOT_BLE_MQTT_MSG_TYPE_PUBLISH );
        }
    }

    if( SERIALIZER_STATUS( status, pBuffer ) == true )
    {
        status = cbor_encode_text_string( &mapEncoder, IOT_BLE_MQTT_TOPIC, strlen( IOT_BLE_MQTT_TOPIC ) );

        if( SERIALIZER_STATUS( status, pBuffer ) == true )
        {
            status = cbor_encode_text_string( &mapEncoder, pPublishInfo->pTopicName, pPublishInfo->topicNameLength );
        }
    }

    if( SERIALIZER_STATUS( status, pBuffer ) == true )
    {
        status = cbor_encode_text_string( &mapEncoder, IOT_BLE_MQTT_QOS, strlen( IOT_BLE_MQTT_QOS ) );

        if( SERIALIZER_STATUS( status, pBuffer ) == true )
        {
            status = cbor_encode_int( &mapEncoder, pPublishInfo->qos );
        }
    }

    if( SERIALIZER_STATUS( status, pBuffer ) == true )
    {
        status = cbor_encode_text_string( &mapEncoder, IOT_BLE_MQTT_PAYLOAD, strlen( IOT_BLE_MQTT_PAYLOAD ) );

        if( SERIALIZER_STATUS( status, pBuffer ) == true )
        {
            status = cbor_encode_byte_string( &mapEncoder, pPublishInfo->pPayload, pPublishInfo->payloadLength );
        }
    }

    if( pPublishInfo->qos != 0 )
    {
        if( SERIALIZER_STATUS( status, pBuffer ) == true )
        {
            status = cbor_encode_text_string( &mapEncoder, IOT_BLE_MQTT_MESSAGE_ID, strlen( IOT_BLE_MQTT_MESSAGE_ID ) );

            if( SERIALIZER_STATUS( status, pBuffer ) == true )
            {
                status = cbor_encode_int( &mapEncoder, packetIdentifier );
            }
        }
    }

    if( SERIALIZER_STATUS( status, pBuffer ) == true )
    {
        status = cbor_encoder_close_container( &encoder, &mapEncoder );
    }

    if( pBuffer == NULL )
    {
        *pLength = cbor_encoder_get_extra_bytes_needed( &encoder );
    }
    else
    {
        *pLength = cbor_encoder_get_buffer_size( &encoder, pBuffer );
    }

    if( SERIALIZER_STATUS( status, pBuffer ) == true )
    {
        result = CborNoError;
    }
    else
    {
        result = status;
    }

    return result;
}

static CborError prvSerializePubAck( uint16_t packetIdentifier,
                                     uint8_t * pBuffer,
                                     size_t * pLength )

{
    size_t length = *pLength;
    CborError status = CborErrorInternalError, result = CborErrorInternalError;
    CborEncoder encoder = { 0 }, mapEncoder = { 0 };

    cbor_encoder_init( &encoder, pBuffer, length, 0 );

    status = cbor_encoder_create_map( &encoder, &mapEncoder, _NUM_PUBACK_PARMAS );

    if( SERIALIZER_STATUS( status, pBuffer ) == true )
    {
        status = cbor_encode_text_string( &mapEncoder, IOT_BLE_MQTT_MSG_TYPE, strlen( IOT_BLE_MQTT_MSG_TYPE ) );

        if( SERIALIZER_STATUS( status, pBuffer ) == true )
        {
            status = cbor_encode_int( &mapEncoder, IOT_BLE_MQTT_MSG_TYPE_PUBACK );
        }
    }

    if( SERIALIZER_STATUS( status, pBuffer ) == true )
    {
        status = cbor_encode_text_string( &mapEncoder, IOT_BLE_MQTT_MESSAGE_ID, strlen( IOT_BLE_MQTT_MESSAGE_ID ) );

        if( SERIALIZER_STATUS( status, pBuffer ) == true )
        {
            status = cbor_encode_int( &mapEncoder, packetIdentifier );
        }
    }

    if( SERIALIZER_STATUS( status, pBuffer ) == true )
    {
        status = cbor_encoder_close_container( &encoder, &mapEncoder );
    }

    if( pBuffer == NULL )
    {
        *pLength = cbor_encoder_get_extra_bytes_needed( &encoder );
    }
    else
    {
        *pLength = cbor_encoder_get_buffer_size( &encoder, pBuffer );
    }

    if( SERIALIZER_STATUS( status, pBuffer ) == true )
    {
        result = CborNoError;
    }
    else
    {
        result = status;
    }

    return result;
}


static CborError prvSerializeSubscribe( const MQTTBLESubscribeInfo_t * const pSubscriptionList,
                                        size_t subscriptionCount,
                                        uint8_t * const pBuffer,
                                        size_t * const pLength,
                                        uint16_t packetIdentifier )
{
    size_t length = *pLength;
    CborError status = CborErrorInternalError, result = CborErrorInternalError;
    CborEncoder encoder = { 0 }, mapEncoder = { 0 }, arrayEncoder = { 0 };
    uint16_t index;

    cbor_encoder_init( &encoder, pBuffer, length, 0 );

    status = cbor_encoder_create_map( &encoder, &mapEncoder, _NUM_SUBACK_PARAMS );

    if( SERIALIZER_STATUS( status, pBuffer ) == true )
    {
        status = cbor_encode_text_string( &mapEncoder, IOT_BLE_MQTT_MSG_TYPE, strlen( IOT_BLE_MQTT_MSG_TYPE ) );

        if( SERIALIZER_STATUS( status, pBuffer ) == true )
        {
            status = cbor_encode_int( &mapEncoder, IOT_BLE_MQTT_MSG_TYPE_SUBSCRIBE );
        }
    }

    if( SERIALIZER_STATUS( status, pBuffer ) == true )
    {
        status = cbor_encode_text_string( &mapEncoder, IOT_BLE_MQTT_TOPIC_LIST, strlen( IOT_BLE_MQTT_TOPIC_LIST ) );
    }

    if( SERIALIZER_STATUS( status, pBuffer ) == true )
    {
        status = cbor_encoder_create_array( &mapEncoder, &arrayEncoder, subscriptionCount );
    }

    for( index = 0; index < subscriptionCount; index++ )
    {
        if( SERIALIZER_STATUS( status, pBuffer ) )
        {
            status = cbor_encode_text_string( &arrayEncoder, pSubscriptionList[ index ].pTopicFilter, pSubscriptionList[ index ].topicFilterLength );
        }
        else
        {
            break;
        }
    }

    if( SERIALIZER_STATUS( status, pBuffer ) == true )
    {
        status = cbor_encoder_close_container( &mapEncoder, &arrayEncoder );
    }

    if( SERIALIZER_STATUS( status, pBuffer ) == true )
    {
        status = cbor_encode_text_string( &mapEncoder, IOT_BLE_MQTT_QOS_LIST, strlen( IOT_BLE_MQTT_QOS_LIST ) );
    }

    if( SERIALIZER_STATUS( status, pBuffer ) == true )
    {
        status = cbor_encoder_create_array( &mapEncoder, &arrayEncoder, subscriptionCount );
    }

    for( index = 0; index < subscriptionCount; index++ )
    {
        if( SERIALIZER_STATUS( status, pBuffer ) )
        {
            status = cbor_encode_int( &arrayEncoder, pSubscriptionList[ index ].qos );
        }
        else
        {
            break;
        }
    }

    if( SERIALIZER_STATUS( status, pBuffer ) == true )
    {
        status = cbor_encoder_close_container( &mapEncoder, &arrayEncoder );
    }

    if( SERIALIZER_STATUS( status, pBuffer ) == true )
    {
        status = cbor_encode_text_string( &mapEncoder, IOT_BLE_MQTT_MESSAGE_ID, strlen( IOT_BLE_MQTT_MESSAGE_ID ) );

        if( SERIALIZER_STATUS( status, pBuffer ) == true )
        {
            status = cbor_encode_int( &mapEncoder, packetIdentifier );
        }
    }

    if( SERIALIZER_STATUS( status, pBuffer ) == true )
    {
        status = cbor_encoder_close_container( &encoder, &mapEncoder );
    }

    if( pBuffer == NULL )
    {
        *pLength = cbor_encoder_get_extra_bytes_needed( &encoder );
    }
    else
    {
        *pLength = cbor_encoder_get_buffer_size( &encoder, pBuffer );
    }

    if( SERIALIZER_STATUS( status, pBuffer ) == true )
    {
        result = CborNoError;
    }
    else
    {
        result = status;
    }

    return result;
}

static CborError prvSerializeUnSubscribe( const MQTTBLESubscribeInfo_t * const pSubscriptionList,
                                          size_t subscriptionCount,
                                          uint8_t * const pBuffer,
                                          size_t * const pLength,
                                          uint16_t packetIdentifier )
{
    size_t length = *pLength;
    CborError status = CborErrorInternalError, result = CborErrorInternalError;
    CborEncoder encoder = { 0 }, mapEncoder = { 0 }, arrayEncoder = { 0 };
    uint16_t index;

    cbor_encoder_init( &encoder, pBuffer, length, 0 );

    status = cbor_encoder_create_map( &encoder, &mapEncoder, _NUM_UNSUBACK_PARAMS );

    if( SERIALIZER_STATUS( status, pBuffer ) == true )
    {
        status = cbor_encode_text_string( &mapEncoder, IOT_BLE_MQTT_MSG_TYPE, strlen( IOT_BLE_MQTT_MSG_TYPE ) );

        if( SERIALIZER_STATUS( status, pBuffer ) == true )
        {
            status = cbor_encode_int( &mapEncoder, IOT_BLE_MQTT_MSG_TYPE_UNSUBSCRIBE );
        }
    }

    if( SERIALIZER_STATUS( status, pBuffer ) == true )
    {
        status = cbor_encode_text_string( &mapEncoder, IOT_BLE_MQTT_TOPIC_LIST, strlen( IOT_BLE_MQTT_TOPIC_LIST ) );
    }

    if( SERIALIZER_STATUS( status, pBuffer ) == true )
    {
        status = cbor_encoder_create_array( &mapEncoder, &arrayEncoder, subscriptionCount );
    }

    for( index = 0; index < subscriptionCount; index++ )
    {
        if( SERIALIZER_STATUS( status, pBuffer ) )
        {
            status = cbor_encode_text_string( &arrayEncoder, pSubscriptionList[ index ].pTopicFilter, pSubscriptionList[ index ].topicFilterLength );
        }
        else
        {
            break;
        }
    }

    if( SERIALIZER_STATUS( status, pBuffer ) == true )
    {
        status = cbor_encoder_close_container( &mapEncoder, &arrayEncoder );
    }

    if( SERIALIZER_STATUS( status, pBuffer ) == true )
    {
        status = cbor_encode_text_string( &mapEncoder, IOT_BLE_MQTT_MESSAGE_ID, strlen( IOT_BLE_MQTT_MESSAGE_ID ) );

        if( SERIALIZER_STATUS( status, pBuffer ) == true )
        {
            status = cbor_encode_int( &mapEncoder, packetIdentifier );
        }
    }

    if( SERIALIZER_STATUS( status, pBuffer ) == true )
    {
        status = cbor_encoder_close_container( &encoder, &mapEncoder );
    }

    if( pBuffer == NULL )
    {
        *pLength = cbor_encoder_get_extra_bytes_needed( &encoder );
    }
    else
    {
        *pLength = cbor_encoder_get_buffer_size( &encoder, pBuffer );
    }

    if( SERIALIZER_STATUS( status, pBuffer ) == true )
    {
        result = CborNoError;
    }
    else
    {
        result = status;
    }

    return result;
}

static CborError prvSerializeDisconnect( uint8_t * const pBuffer,
                                         size_t * const pLength )
{
    size_t length = *pLength;
    CborError status = CborErrorInternalError, result = CborErrorInternalError;
    CborEncoder encoder = { 0 }, mapEncoder = { 0 };

    cbor_encoder_init( &encoder, pBuffer, length, 0 );

    status = cbor_encoder_create_map( &encoder, &mapEncoder, _NUM_DISCONNECT_PARAMS );

    if( SERIALIZER_STATUS( status, pBuffer ) == true )
    {
        status = cbor_encode_text_string( &mapEncoder, IOT_BLE_MQTT_MSG_TYPE, strlen( IOT_BLE_MQTT_MSG_TYPE ) );

        if( SERIALIZER_STATUS( status, pBuffer ) == true )
        {
            status = cbor_encode_int( &mapEncoder, IOT_BLE_MQTT_MSG_TYPE_DISCONNECT );
        }
    }

    if( SERIALIZER_STATUS( status, pBuffer ) == true )
    {
        status = cbor_encoder_close_container( &encoder, &mapEncoder );
    }

    if( pBuffer == NULL )
    {
        *pLength = cbor_encoder_get_extra_bytes_needed( &encoder );
    }
    else
    {
        *pLength = cbor_encoder_get_buffer_size( &encoder, pBuffer );
    }

    if( SERIALIZER_STATUS( status, pBuffer ) == true )
    {
        result = CborNoError;
    }
    else
    {
        result = status;
    }

    return result;
}

static CborError prvSerializePingRequest( uint8_t * const pBuffer,
                                          size_t * const pLength )
{
    size_t length = *pLength;
    CborError status = CborErrorInternalError, result = CborErrorInternalError;
    CborEncoder encoder = { 0 }, mapEncoder = { 0 };

    cbor_encoder_init( &encoder, pBuffer, length, 0 );

    status = cbor_encoder_create_map( &encoder, &mapEncoder, _NUM_PINGREQUEST_PARAMS );

    if( SERIALIZER_STATUS( status, pBuffer ) == true )
    {
        status = cbor_encode_text_string( &mapEncoder, IOT_BLE_MQTT_MSG_TYPE, strlen( IOT_BLE_MQTT_MSG_TYPE ) );

        if( SERIALIZER_STATUS( status, pBuffer ) == true )
        {
            status = cbor_encode_int( &mapEncoder, IOT_BLE_MQTT_MSG_TYPE_PINGREQ );
        }
    }

    if( SERIALIZER_STATUS( status, pBuffer ) == true )
    {
        status = cbor_encoder_close_container( &encoder, &mapEncoder );
    }

    if( pBuffer == NULL )
    {
        *pLength = cbor_encoder_get_extra_bytes_needed( &encoder );
    }
    else
    {
        *pLength = cbor_encoder_get_buffer_size( &encoder, pBuffer );
    }

    if( SERIALIZER_STATUS( status, pBuffer ) == true )
    {
        result = CborNoError;
    }
    else
    {
        result = status;
    }

    return result;
}


static CborError prvGetIntegerValueFromMap( CborValue * pMap,
                                            const char * key,
                                            int64_t * pValue )
{
    CborError status = CborErrorInternalError;
    CborValue value = { 0 };

    status = cbor_value_map_find_value( pMap, key, &value );

    if( status == CborNoError )
    {
        if( cbor_value_is_integer( &value ) )
        {
            status = cbor_value_get_int64( &value, pValue );
        }
        else
        {
            status = CborErrorImproperValue;
        }
    }

    return status;
}

/**
 * The function is used to get the pointer to a text string value instead of copying the contents to a buffer.
 * This function only works if the string value is encoded within a definite length map and the text string encoded is
 * of definite length. Function returns the pointer to the start of the string and the length of the string.
 */
static CborError prvGetTextStringValueFromDefiniteLengthMap( CborValue * pMap,
                                                             const char * key,
                                                             const char ** pValue,
                                                             size_t * valueLength )
{
    CborError status = CborErrorInternalError;
    size_t length;
    CborValue value = { 0 }, next = { 0 };

    status = cbor_value_map_find_value( pMap, key, &value );

    if( status == CborNoError )
    {
        if( cbor_value_is_text_string( &value ) )
        {
            if( cbor_value_is_length_known( &value ) )
            {
                status = cbor_value_get_string_length( &value, &length );
            }
            else
            {
                status = CborErrorImproperValue;
            }
        }
        else
        {
            status = CborErrorImproperValue;
        }

        if( status == CborNoError )
        { /* Advance to the next byte after this string value. */
            status = cbor_value_copy_text_string( &value, NULL, &length, &next );

            if( status == CborNoError )
            {
                ( *pValue ) = ( char * ) ( cbor_value_get_next_byte( &next ) - length );
                ( *valueLength ) = length;
            }
        }
    }

    return status;
}

/**
 * The function is used to get the pointer to a byte string value instead of copying the contents to a buffer.
 * This function only works if the string value is encoded within a definite length map and the byte string encoded is
 * of definite length. Function returns the pointer to the start of the string and the length of the string.
 */
static CborError prvGetByteStringValueFromDefiniteLengthMap( CborValue * pMap,
                                                             const char * key,
                                                             const void ** pValue,
                                                             size_t * valueLength )
{
    CborError status = CborErrorInternalError;
    size_t length = 0;
    CborValue value = { 0 }, next = { 0 };

    status = cbor_value_map_find_value( pMap, key, &value );

    if( status == CborNoError )
    {
        if( cbor_value_is_byte_string( &value ) )
        {
            if( cbor_value_is_length_known( &value ) )
            {
                status = cbor_value_get_string_length( &value, &length );
            }
            else
            {
                status = CborErrorImproperValue;
            }
        }
        else
        {
            status = CborErrorImproperValue;
        }

        if( status == CborNoError )
        {
            /* Advance to the next byte after this string value. */
            status = cbor_value_copy_byte_string( &value, NULL, &length, &next );

            if( status == CborNoError )
            {
                ( *pValue ) = ( void * ) ( cbor_value_get_next_byte( &next ) - length );
                ( *valueLength ) = length;
            }
        }
    }

    return status;
}


MQTTBLEStatus_t IotBleMqtt_SerializeConnect( const MQTTBLEConnectInfo_t * const pConnectInfo,
                                             uint8_t ** const pConnectPacket,
                                             size_t * const pPacketSize )
{
    uint8_t * pBuffer = NULL;
    size_t bufLen = 0;
    CborError status = CborErrorInternalError;
    MQTTBLEStatus_t ret = MQTTBLEBadParameter;

    ( *pConnectPacket ) = NULL;
    ( *pPacketSize ) = 0;

    status = prvSerializeConnect( pConnectInfo, NULL, &bufLen );

    if( status == CborNoError )
    {
        pBuffer = IotMqtt_MallocMessage( bufLen );

        /* If Memory cannot be allocated log an error and return */
        if( pBuffer != NULL )
        {
            status = prvSerializeConnect( pConnectInfo, pBuffer, &bufLen );

            if( status == CborNoError )
            {
                *pConnectPacket = pBuffer;
                *pPacketSize = bufLen;
                ret = MQTTBLESuccess;
            }
            else
            {
                IotMqtt_FreeMessage( pBuffer );
                LogError( ( "Failed to serialize CONNECT message, error = %d", status ) );
            }
        }
        else
        {
            LogError( ( "Failed to allocate memory for CONNECT packet." ) );
            ret = MQTTBLENoMemory;
        }
    }
    else
    {
        LogError( ( "Failed to find length of serialized CONNECT message, error = %d", status ) );
    }

    return ret;
}

MQTTBLEStatus_t IotBleMqtt_DeserializeConnack( const uint8_t * pMessage,
                                               size_t length )
{
    CborParser parser = { 0 };
    CborValue map = { 0 };
    CborError status = CborErrorInternalError;
    int64_t respCode = 0;
    MQTTBLEStatus_t ret = MQTTBLESuccess;

    status = cbor_parser_init( pMessage, length, 0, &parser, &map );

    if( status == CborNoError )
    {
        if( !cbor_value_is_map( &map ) )
        {
            status = CborErrorImproperValue;
        }
    }

    if( status == CborNoError )
    {
        status = prvGetIntegerValueFromMap( &map, IOT_BLE_MQTT_STATUS, &respCode );

        if( status == CborNoError )
        {
            if( ( respCode != IOT_BLE_MQTT_STATUS_CONNECTING ) &&
                ( respCode != IOT_BLE_MQTT_STATUS_CONNECTED ) )
            {
                ret = MQTTBLEServerRefused;
            }
        }
    }

    if( status != CborNoError )
    {
        LogError( ( "Decode connack failed with error = %d", status ) );
        ret = MQTTBLEBadResponse;
    }

    return ret;
}

MQTTBLEStatus_t IotBleMqtt_SerializePublish( const MQTTBLEPublishInfo_t * const pPublishInfo,
                                             uint8_t ** const pPublishPacket,
                                             size_t * const pPacketSize,
                                             uint16_t packetIdentifier )
{
    uint8_t * pBuffer = NULL;
    size_t bufLen = 0;
    CborError status = CborErrorInternalError;
    MQTTBLEStatus_t ret = MQTTBLEBadParameter;

    ( *pPublishPacket ) = NULL;
    ( *pPacketSize ) = 0;

    status = prvSerializePublish( pPublishInfo, NULL, &bufLen, packetIdentifier );

    if( status == CborNoError )
    {
        pBuffer = IotMqtt_MallocMessage( bufLen );

        /* If Memory cannot be allocated log an error and return */
        if( pBuffer != NULL )
        {
            status = prvSerializePublish( pPublishInfo, pBuffer, &bufLen, packetIdentifier );

            if( status == CborNoError )
            {
                ( *pPublishPacket ) = pBuffer;
                ( *pPacketSize ) = bufLen;
                ret = MQTTBLESuccess;
            }
            else
            {
                IotMqtt_FreeMessage( pBuffer );
                LogError( ( "Failed to serialize PUBLISH message, error = %d", status ) );
            }
        }
        else
        {
            LogError( ( "Failed to allocate memory for PUBLISH packet." ) );
            ret = MQTTBLENoMemory;
        }
    }
    else
    {
        LogError( ( "Failed to find size of serialized PUBLISH message, error = %d", status ) );
    }

    return ret;
}

void IotBleMqtt_PublishSetDup( uint8_t * const pPublishPacket,
                               uint8_t * pPacketIdentifierHigh,
                               uint16_t * const pNewPacketIdentifier )
{
    /** TODO: Currently DUP flag is not supported by BLE SDKs **/
}

MQTTBLEStatus_t IotBleMqtt_DeserializePublish( uint8_t * pMessage,
                                               size_t length,
                                               MQTTBLEPublishInfo_t * publishInfo,
                                               uint16_t * packetIdentifier )
{
    CborParser parser = { 0 };
    CborValue map = { 0 };
    CborError status = CborNoError;
    int64_t val = 0;
    MQTTBLEStatus_t ret = MQTTBLESuccess;

    memset( publishInfo, 0x00, sizeof( MQTTBLEPublishInfo_t ) );

    status = cbor_parser_init( pMessage, length, 0, &parser, &map );

    if( status == CborNoError )
    {
        if( !cbor_value_is_map( &map ) )
        {
            status = CborErrorImproperValue;
        }
    }

    if( status == CborNoError )
    {
        status = prvGetIntegerValueFromMap( &map, IOT_BLE_MQTT_QOS, &val );

        if( status == CborNoError )
        {
            publishInfo->qos = ( MQTTBLEQoS_t ) val;
        }
    }

    if( status == CborNoError )
    {
        status = prvGetTextStringValueFromDefiniteLengthMap( &map, IOT_BLE_MQTT_TOPIC, &publishInfo->pTopicName, ( size_t * ) &publishInfo->topicNameLength );
    }

    if( status == CborNoError )
    {
        status = prvGetByteStringValueFromDefiniteLengthMap( &map, IOT_BLE_MQTT_PAYLOAD, &publishInfo->pPayload, &publishInfo->payloadLength );
    }

    if( status == CborNoError )
    {
        if( publishInfo->qos != 0 )
        {
            status = prvGetIntegerValueFromMap( &map, IOT_BLE_MQTT_MESSAGE_ID, &val );

            if( status == CborNoError )
            {
                ( *packetIdentifier ) = ( uint16_t ) ( val );
            }
        }
    }

    if( status != CborNoError )
    {
        LogError( ( "Decode cbor publish message failed with error = %d", status ) );
        ret = MQTTBLEBadResponse;
    }

    return ret;
}

MQTTBLEStatus_t IotBleMqtt_SerializePuback( uint16_t packetIdentifier,
                                            uint8_t ** const pPubackPacket,
                                            size_t * const pPacketSize )
{
    uint8_t * pBuffer = NULL;
    size_t bufLen = 0;
    CborError status = CborErrorInternalError;
    MQTTBLEStatus_t ret = MQTTBLEBadParameter;

    ( *pPubackPacket ) = NULL;
    ( *pPacketSize ) = 0;

    status = prvSerializePubAck( packetIdentifier, NULL, &bufLen );

    if( status == CborNoError )
    {
        pBuffer = IotMqtt_MallocMessage( bufLen );

        /* If Memory cannot be allocated log an error and return */
        if( pBuffer != NULL )
        {
            status = prvSerializePubAck( packetIdentifier, pBuffer, &bufLen );

            if( status == CborNoError )
            {
                ( *pPubackPacket ) = pBuffer;
                ( *pPacketSize ) = bufLen;
                ret = MQTTBLESuccess;
            }
            else
            {
                IotMqtt_FreeMessage( pBuffer );
                LogError( ( "Failed to serialize PUBACK message, error = %d", status ) );
            }
        }
        else
        {
            LogError( ( "Failed to allocate memory for PUBACK packet." ) );
            ret = MQTTBLENoMemory;
        }
    }
    else
    {
        LogError( ( "Failed to find size of serialized PUBACK message, error = %d", status ) );
    }

    return ret;
}

MQTTBLEStatus_t IotBleMqtt_DeserializePuback( uint8_t * pMessage,
                                              size_t length,
                                              uint16_t * packetIdentifier )
{
    CborParser parser = { 0 };
    CborValue map = { 0 };
    CborError status = CborNoError;
    int64_t val = 0;
    MQTTBLEStatus_t ret = MQTTBLESuccess;

    status = cbor_parser_init( pMessage, length, 0, &parser, &map );

    if( status == CborNoError )
    {
        if( !cbor_value_is_map( &map ) )
        {
            status = CborErrorImproperValue;
        }
    }

    if( status == CborNoError )
    {
        status = prvGetIntegerValueFromMap( &map, IOT_BLE_MQTT_MESSAGE_ID, &val );

        if( status == CborNoError )
        {
            ( *packetIdentifier ) = ( uint16_t ) val;
        }
    }

    if( status != CborNoError )
    {
        LogError( ( "Decode cbor puback message failed with error = %d", status ) );
        ret = MQTTBLEBadResponse;
    }

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
    CborError status = CborErrorInternalError;
    MQTTBLEStatus_t ret = MQTTBLEBadParameter;

    ( *pSubscribePacket ) = NULL;
    ( *pPacketSize ) = 0;

    status = prvSerializeSubscribe( pSubscriptionList, subscriptionCount, NULL, &bufLen, *pPacketIdentifier );

    if( status == CborNoError )
    {
        pBuffer = IotMqtt_MallocMessage( bufLen );

        /* If Memory cannot be allocated log an error and return */
        if( pBuffer != NULL )
        {
            status = prvSerializeSubscribe( pSubscriptionList, subscriptionCount, pBuffer, &bufLen, *pPacketIdentifier );

            if( status == CborNoError )
            {
                ( *pSubscribePacket ) = pBuffer;
                ( *pPacketSize ) = bufLen;
                ret = MQTTBLESuccess;
            }
            else
            {
                IotMqtt_FreeMessage( pBuffer );
                LogError( ( "Failed to serialize SUBSCRIBE message, error = %d", status ) );
            }
        }
        else
        {
            LogError( ( "Failed to allocate memory for SUBSCRIBE packet." ) );
            ret = MQTTBLENoMemory;
        }
    }
    else
    {
        LogError( ( "Failed to find size of serialized SUBSCRIBE message, error = %d", status ) );
    }

    return ret;
}

MQTTBLEStatus_t IotBleMqtt_DeserializeSuback( const uint8_t * pMessage,
                                              size_t length,
                                              uint16_t * packetIdentifier,
                                              uint8_t * pStatusCode )
{
    CborParser parser = { 0 };
    CborValue map = { 0 };
    CborError status = CborNoError;
    int64_t val = 0;
    MQTTBLEStatus_t ret = MQTTBLESuccess;

    status = cbor_parser_init( pMessage, length, 0, &parser, &map );

    if( status == CborNoError )
    {
        if( !cbor_value_is_map( &map ) )
        {
            status = CborErrorImproperValue;
        }
    }

    if( status == CborNoError )
    {
        status = prvGetIntegerValueFromMap( &map, IOT_BLE_MQTT_MESSAGE_ID, &val );

        if( status == CborNoError )
        {
            ( *packetIdentifier ) = ( uint16_t ) val;
        }
    }

    if( status == CborNoError )
    {
        status = prvGetIntegerValueFromMap( &map, IOT_BLE_MQTT_STATUS, &val );

        if( status == CborNoError )
        {
            ( *pStatusCode ) = ( uint8_t ) val;
        }
    }

    if( status != CborNoError )
    {
        LogError( ( "Decode cbor suback message failed with error = %d", status ) );
        ret = MQTTBLEBadResponse;
    }

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
    CborError status = CborErrorInternalError;
    MQTTBLEStatus_t ret = MQTTBLEBadParameter;

    ( *pUnsubscribePacket ) = NULL;
    ( *pPacketSize ) = 0;

    status = prvSerializeUnSubscribe( pSubscriptionList, subscriptionCount, NULL, &bufLen, *pPacketIdentifier );

    if( status == CborNoError )
    {
        pBuffer = IotMqtt_MallocMessage( bufLen );

        /* If Memory cannot be allocated log an error and return */
        if( pBuffer != NULL )
        {
            status = prvSerializeUnSubscribe( pSubscriptionList, subscriptionCount, pBuffer, &bufLen, *pPacketIdentifier );

            if( status == CborNoError )
            {
                ( *pUnsubscribePacket ) = pBuffer;
                ( *pPacketSize ) = bufLen;
                ret = MQTTBLESuccess;
            }
            else
            {
                IotMqtt_FreeMessage( pBuffer );
                LogError( ( "Failed to serialize UNSUBSCRIBE message, error = %d", status ) );
            }
        }
        else
        {
            LogError( ( "Failed to allocate memory for UNSUBSCRIBE packet." ) );
            ret = MQTTBLENoMemory;
        }
    }
    else
    {
        LogError( ( "Failed to find size of serialized UNSUBSCRIBE message, error = %d", status ) );
    }

    return ret;
}

MQTTBLEStatus_t IotBleMqtt_DeserializeUnsuback( uint8_t * pMessage,
                                                size_t length,
                                                uint16_t * packetIdentifier )
{
    CborParser parser = { 0 };
    CborValue map = { 0 };
    CborError status = CborNoError;
    int64_t val = 0;
    MQTTBLEStatus_t ret = MQTTBLESuccess;

    status = cbor_parser_init( pMessage, length, 0, &parser, &map );

    if( status == CborNoError )
    {
        if( !cbor_value_is_map( &map ) )
        {
            status = CborErrorImproperValue;
        }
    }

    if( status == CborNoError )
    {
        status = prvGetIntegerValueFromMap( &map, IOT_BLE_MQTT_MESSAGE_ID, &val );

        if( status == CborNoError )
        {
            ( *packetIdentifier ) = ( uint16_t ) val;
        }
    }

    if( status != CborNoError )
    {
        LogError( ( "Decode cbor unsuback message failed with error = %d", status ) );
        ret = MQTTBLEBadResponse;
    }

    return ret;
}

MQTTBLEStatus_t IotBleMqtt_SerializeDisconnect( uint8_t ** const pDisconnectPacket,
                                                size_t * const pPacketSize )
{
    uint8_t * pBuffer = NULL;
    size_t bufLen = 0;
    CborError status = CborErrorInternalError;
    MQTTBLEStatus_t ret = MQTTBLEBadParameter;

    ( *pDisconnectPacket ) = NULL;
    ( *pPacketSize ) = 0;

    status = prvSerializeDisconnect( NULL, &bufLen );

    if( status == CborNoError )
    {
        pBuffer = IotMqtt_MallocMessage( bufLen );

        /* If Memory cannot be allocated log an error and return */
        if( pBuffer != NULL )
        {
            status = prvSerializeDisconnect( pBuffer, &bufLen );

            if( status == CborNoError )
            {
                ( *pDisconnectPacket ) = pBuffer;
                ( *pPacketSize ) = bufLen;
                ret = MQTTBLESuccess;
            }
            else
            {
                IotMqtt_FreeMessage( pBuffer );
                LogError( ( "Failed to serialize DISCONNECT message, error = %d", status ) );
            }
        }
        else
        {
            LogError( ( "Failed to allocate memory for DISCONNECT packet." ) );
            ret = MQTTBLENoMemory;
        }
    }
    else
    {
        LogError( ( "Failed to find size of serialized DISCONNECT message, error = %d", status ) );
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


uint8_t IotBleMqtt_GetPacketType( const uint8_t * pMessage,
                                  size_t length )
{
    CborParser parser = { 0 };
    CborValue map = { 0 };
    CborError status = CborNoError;
    int64_t val = 0;
    uint8_t packetType = IOT_BLE_MQTT_MSG_TYPE_INVALID;

    status = cbor_parser_init( pMessage, length, 0, &parser, &map );

    if( status == CborNoError )
    {
        if( !cbor_value_is_map( &map ) )
        {
            status = CborErrorImproperValue;
        }
    }

    if( status == CborNoError )
    {
        status = prvGetIntegerValueFromMap( &map, IOT_BLE_MQTT_MSG_TYPE, &val );

        if( status == CborNoError )
        {
            packetType = ( uint8_t ) val;
        }
    }

    if( status != CborNoError )
    {
        LogError( ( "Decode cbor message failed with error = %d", status ) );
    }

    return packetType;
}

MQTTBLEStatus_t IotBleMqtt_SerializePingreq( uint8_t ** const pPingreqPacket,
                                             size_t * const pPacketSize )
{
    uint8_t * pBuffer = NULL;
    size_t bufLen = 0;
    CborError status = CborErrorInternalError;
    MQTTBLEStatus_t ret = MQTTBLEBadParameter;

    ( *pPingreqPacket ) = NULL;
    ( *pPacketSize ) = 0;

    status = prvSerializePingRequest( NULL, &bufLen );

    if( status == CborNoError )
    {
        pBuffer = IotMqtt_MallocMessage( bufLen );

        /* If Memory cannot be allocated log an error and return */
        if( pBuffer != NULL )
        {
            status = prvSerializePingRequest( pBuffer, &bufLen );

            if( status == CborNoError )
            {
                ( *pPingreqPacket ) = pBuffer;
                ( *pPacketSize ) = bufLen;
                ret = MQTTBLESuccess;
            }
            else
            {
                IotMqtt_FreeMessage( pBuffer );
                LogError( ( "Failed to serialize PINGREQ message, error = %d", status ) );
            }
        }
        else
        {
            LogError( ( "Failed to allocate memory for PINGREQ packet." ) );
            ret = MQTTBLENoMemory;
        }
    }
    else
    {
        LogError( ( "Failed to find size of serialized PINGREQ message, error = %d", status ) );
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
