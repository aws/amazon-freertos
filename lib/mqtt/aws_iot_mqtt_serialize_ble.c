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

/* FreeRTOS includes. */
#include "FreeRTOS.h"

/* MQTT internal includes. */
#include "private/aws_iot_mqtt_serialize_ble.h"
#include "aws_json_utils.h"
#include "mbedtls/base64.h"
#include "private/aws_iot_mqtt_internal.h"
/**
 * @brief Does an in place Base64 decoding copying the decoded data back to the input buffer.
 *
 * @param [in]  Pointer to the buffer containing the encoded data.
 * @param [in]  Length of the encoded data
 * @param [out] Length of the decoded data
 *
 * @return pdTRUE if successful, pdFALSE if failed
 */
BaseType_t  prxDecodeInPlace( uint8_t * const pData, const size_t dataLen, size_t * const pDecodedLen );

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


/* Do an in place decoding */
BaseType_t prxDecodeInPlace( uint8_t * const pData, const size_t dataLen, size_t * const pDecodedLen )
{
	uint8_t *pDecodeBuffer = NULL;
	size_t decodedLen;

	(void) mbedtls_base64_decode( NULL, 0, &decodedLen,
										( const unsigned char *) pData, dataLen );

	AwsIotMqtt_Assert(( decodedLen <= dataLen ));

	pDecodeBuffer = AwsIotMqtt_MallocMessage( decodedLen );

	if( pDecodeBuffer == NULL )
	{
		return pdFALSE;
	}

	(void) mbedtls_base64_decode( pDecodeBuffer, decodedLen, &decodedLen,
											( const unsigned char *) pData, dataLen );

	memcpy( pData, pDecodeBuffer, decodedLen );
	*pDecodedLen = decodedLen;

	AwsIotMqtt_FreeMessage( pDecodeBuffer );

	return pdTRUE;
}

/* Serialize topic array */
AwsIotMqttError_t prxSerializeTopicFilterArray( const AwsIotMqttSubscription_t* const pSubscriptions,
		size_t subscriptionCount,
		uint8_t** pTopicArrayBuffer,
		size_t* pTopicArrayBufLen )
{
	size_t x, idx = 0;
	size_t topicLen, bufLen = JSON_STR_ARR_LEN( subscriptionCount );
	uint8_t* pBuffer;

	for( x = 0; x < subscriptionCount; x++ )
	{
		bufLen += BASE64_ENCODED_LENGTH( pSubscriptions[x].topicFilterLength ) + 1; /* Encoded length including '\0' */
	}

	pBuffer = AwsIotMqtt_MallocMessage( bufLen );
	if( pBuffer == NULL )
	{
		return AWS_IOT_MQTT_NO_MEMORY;
	}
	*pTopicArrayBuffer = pBuffer;
	pBuffer[ idx++ ] = '[';
	for( x = 0; x < subscriptionCount; x++ )
	{
		pBuffer[ idx++ ] = '"';

        (void) mbedtls_base64_encode(
                &pBuffer[ idx ],
                ( bufLen - idx ),
                &topicLen,
                ( const unsigned char *) pSubscriptions[x].pTopicFilter,
                pSubscriptions[x].topicFilterLength );
		idx += topicLen;
        AwsIotMqtt_Assert( idx <= ( bufLen - 2 ) );

        pBuffer[idx++] = '"';
		if ( x != ( subscriptionCount - 1 ) )
		{
			pBuffer[idx++] = ',';
		}
	}
	pBuffer[idx++] = ']';
	*pTopicArrayBufLen =  idx;

	return AWS_IOT_MQTT_SUCCESS;
}


AwsIotMqttError_t prxSerializeQoSArray( const AwsIotMqttSubscription_t* const pSubscriptions,
		size_t subscriptionCount,
		uint8_t** pQosArrayBuffer,
		size_t* pQosArrayBufLen )
{
	size_t x, idx = 0;
	size_t bufLen = JSON_PRIMITIVE_ARR_LEN( subscriptionCount, mqttBLEQOS_WIDTH );
	uint8_t* pBuffer;

	pBuffer = AwsIotMqtt_MallocMessage( bufLen );
	if( pBuffer == NULL )
	{
		return AWS_IOT_MQTT_NO_MEMORY;
	}
	*pQosArrayBuffer = pBuffer;
	*pQosArrayBufLen = bufLen;

	pBuffer[ idx++ ] = '[';
	for( x = 0; x < subscriptionCount; x++ )
	{
		switch( pSubscriptions[x].QoS )
		{
		case 0:
			pBuffer[idx++] = '0';
			break;
		case 1:
			pBuffer[idx++] = '1';
			break;
		case 2:
			pBuffer[idx++] = '2';
			break;
		}
		if ( x != ( subscriptionCount - 1 ) )
		{
			pBuffer[idx++] = ',';
		}
	}
	pBuffer[idx] = ']';

	return AWS_IOT_MQTT_SUCCESS;
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
	size_t bufLen = mqttBLECONNECT_MSG_LEN( pConnectInfo );

	pBuffer = AwsIotMqtt_MallocMessage( bufLen );

	/* If Memory cannot be allocated log an error and return */
    if( pBuffer == NULL )
    {
        AwsIotLogError( "Failed to allocate memory for CONNECT packet." );
        return AWS_IOT_MQTT_NO_MEMORY;
    }

    *pConnectPacket = pBuffer;
	*pPacketSize = snprintf( ( char* ) pBuffer,
			bufLen,
			mqttBLECONNECT_MSG_FORMAT,
			mqttBLEMSG_TYPE_CONNECT,
			pConnectInfo->clientIdentifierLength,
			pConnectInfo->pClientIdentifier,
			sizeof( clientcredentialMQTT_BROKER_ENDPOINT ),
			clientcredentialMQTT_BROKER_ENDPOINT,
			clientcredentialMQTT_BROKER_PORT,
			pConnectInfo->userNameLength,
			pConnectInfo->pUserName,
			mqttBLETRUE );

	return AWS_IOT_MQTT_SUCCESS;
}

AwsIotMqttError_t AwsIotMqttBLE_DeserializeConnack( const uint8_t * const pConnackStart,
                                                         size_t dataLength,
                                                         size_t * const pBytesProcessed )
{
	int16_t numTokens;
	jsmntok_t tokens[ mqttBLEMAX_MESG_TOKENS ];
	BaseType_t respCodeFound;
	int16_t respCode;

	numTokens = JsonUtils_Parse( ( const char* ) pConnackStart, dataLength, tokens, mqttBLEMAX_MESG_TOKENS );

	if( numTokens <= 0 )
	{
		*pBytesProcessed = 0;
		AwsIotLogError( "Malformed MQTT packet received." );
		return AWS_IOT_MQTT_BAD_RESPONSE;
	}

	*pBytesProcessed = tokens[0].end;

	respCodeFound = JsonUtils_GetInt16Value( ( const char *) pConnackStart,
			tokens,
			numTokens,
			mqttBLESTATUS,
			strlen( mqttBLESTATUS ),
			&respCode );

	if( respCodeFound == pdFALSE )
	{
		AwsIotLogError( "Malformed CONNACK packet, response code not found" );

		return AWS_IOT_MQTT_BAD_RESPONSE;
	}


	if( respCode != eMQTTBLEStatusConnecting  && respCode != eMQTTBLEStatusConnected )
	{
		AwsIotLogError( "Connection refused by the BLE proxy, response code %d", respCode  );
		return AWS_IOT_MQTT_SERVER_REFUSED;
	}

	return AWS_IOT_MQTT_SUCCESS;
}


AwsIotMqttError_t AwsIotMqttBLE_SerializePublish( const AwsIotMqttPublishInfo_t * const pPublishInfo,
                                                       uint8_t ** const pPublishPacket,
                                                       size_t * const pPacketSize,
                                                       uint16_t * const pPacketIdentifier )
{

	size_t bufLen, topicLen, dataLen, packetLen;
	uint8_t* pTopicBuffer, *pBuffer;


	topicLen = BASE64_ENCODED_LENGTH( pPublishInfo->topicNameLength ) + 1;
	dataLen  = BASE64_ENCODED_LENGTH( pPublishInfo->payloadLength ) + 1;

	bufLen = mqttBLEPUBLISH_MSG_LEN( topicLen, dataLen );

	pTopicBuffer = AwsIotMqtt_MallocMessage( topicLen );
	if( pTopicBuffer == NULL )
	{
		AwsIotLogError( "Failed to allocate memory for encoding topic" );
		return AWS_IOT_MQTT_NO_MEMORY;
	}

	pBuffer = AwsIotMqtt_MallocMessage( bufLen );
	if( pBuffer == NULL )
	{
		AwsIotMqtt_FreeMessage( pTopicBuffer );
		AwsIotLogError( "Failed to allocate memory for PUBLISH message, topic = %.*s", pPublishInfo->topicNameLength,  pPublishInfo->pTopicName );
		return AWS_IOT_MQTT_NO_MEMORY;
	}

	( void ) mbedtls_base64_encode( pTopicBuffer, topicLen, &topicLen,
			( const unsigned char *) pPublishInfo->pTopicName, pPublishInfo->topicNameLength );


	if ( pPublishInfo->QoS != 0 )
	{
		*pPacketIdentifier = prusNextPacketIdentifier();
		packetLen = snprintf( ( char* ) pBuffer,
				bufLen,
				mqttBLEPUBLISH_MSG_HEADER,
				mqttBLEMSG_TYPE_PUBLISH,
				topicLen,
				(const char *) pTopicBuffer,
				pPublishInfo->QoS,
				( *pPacketIdentifier ) );
	}
	else
	{
		packetLen = snprintf( ( char* ) pBuffer,
				bufLen,
				mqttBLEPUBLISH_MSG_HEADER,
				mqttBLEMSG_TYPE_PUBLISH,
				topicLen,
				(const char *) pTopicBuffer,
				pPublishInfo->QoS,
				0 );
	}

	( void ) mbedtls_base64_encode( ( pBuffer + packetLen ), dataLen, &dataLen,
			( const unsigned char *) pPublishInfo->pPayload, pPublishInfo->payloadLength );
	packetLen += dataLen;

	strcpy( (char *) ( pBuffer + packetLen ), mqttBLEPUBLISH_MSG_TRAILER );
	packetLen += strlen(mqttBLEPUBLISH_MSG_TRAILER);

	*pPublishPacket = pBuffer;
	*pPacketSize = packetLen;

	AwsIotMqtt_FreeMessage( pTopicBuffer );

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
	int16_t numTokens;
	jsmntok_t tokens[ mqttBLEMAX_MESG_TOKENS ];
	BaseType_t result;

	numTokens = JsonUtils_Parse( ( const char* ) pPublishStart, dataLength, tokens, mqttBLEMAX_MESG_TOKENS );

	if( numTokens <= 0 )
	{
		*pBytesProcessed = 0;
		AwsIotLogError( "Malformed MQTT packet received." );
		return AWS_IOT_MQTT_BAD_RESPONSE;
	}

	*pBytesProcessed = tokens[0].end;

	result = JsonUtils_GetInt16Value( ( const char* ) pPublishStart,
			tokens,
			numTokens, mqttBLEQOS,
			strlen( mqttBLEQOS ),
			(int16_t*) &pOutput->QoS );

	if( result == pdFALSE )
	{
		AwsIotLogError( "Cannot find QoS field in publish packet, identifier:%d", ( *pPacketIdentifier ) );
		return AWS_IOT_MQTT_BAD_RESPONSE;
	}

	if( pOutput->QoS != 0 )
	{
		result = JsonUtils_GetInt16Value( ( const char* ) pPublishStart,
				tokens,
				numTokens, mqttBLEMESSAGE_ID,
				strlen( mqttBLEMESSAGE_ID ),
				(int16_t*) pPacketIdentifier );

		if( result == pdFALSE )
		{
			AwsIotLogError( "Cannot find packet identifier field in publish packet ");
			return AWS_IOT_MQTT_BAD_RESPONSE;
		}
	}

	/* Extract published topic */
	JsonUtils_GetStrValue( ( const char* ) pPublishStart,
			tokens,
			numTokens,
			mqttBLETOPIC,
			strlen( mqttBLETOPIC ),
			(const char**) &pOutput->pTopicName,
			(uint32_t*) &pOutput->topicNameLength );

	if( pOutput->topicNameLength == 0 )
	{
		AwsIotLogError( "Cannot find topic field in publish packet, identifier:%d", ( *pPacketIdentifier ) );
		return AWS_IOT_MQTT_BAD_RESPONSE;
	}

	result = prxDecodeInPlace( ( uint8_t *) pOutput->pTopicName, ( size_t ) pOutput->topicNameLength, ( size_t* ) &pOutput->topicNameLength );

	if( result == pdFALSE )
	{
		AwsIotLogError( "Cannot decode topic field in publish packet, identifier:%d", ( *pPacketIdentifier ) );
		return AWS_IOT_MQTT_NO_MEMORY;
	}

	/* Extract published data */
	JsonUtils_GetStrValue( ( const char* ) pPublishStart,
			tokens,
			numTokens,
			mqttBLEPAYLOAD,
			strlen( mqttBLEPAYLOAD ),
			(const char**) &pOutput->pPayload,
			( uint32_t* ) &pOutput->payloadLength );

	if( pOutput->payloadLength == 0 )
	{
		AwsIotLogError( "Cannot find topic field in publish packet, identifier:%d", ( *pPacketIdentifier ) );
		return AWS_IOT_MQTT_BAD_RESPONSE;
	}

	result = prxDecodeInPlace( ( uint8_t *) pOutput->pPayload, ( size_t ) pOutput->payloadLength, ( size_t * )&pOutput->payloadLength );

	if( result == pdFALSE )
	{
		AwsIotLogError( "Cannot decode data in publish packet, identifier:%d", ( *pPacketIdentifier ) );
		return AWS_IOT_MQTT_NO_MEMORY;
	}

	pOutput->retain = false;

	return AWS_IOT_MQTT_SUCCESS;
}

AwsIotMqttError_t AwsIotMqttBLE_SerializePuback( uint16_t packetIdentifier,
                                                      uint8_t ** const pPubackPacket,
                                                      size_t * const pPacketSize )
{
	uint8_t * pBuffer;
	pBuffer = AwsIotMqtt_MallocMessage( mqttBLEPUBACK_MSG_LEN );

	/* If Memory cannot be allocated log an error and return */
	if( pBuffer == NULL )
	{
		AwsIotLogError( "Failed to allocate memory for PUBACK packet, packet identifier = %d", packetIdentifier );
		return AWS_IOT_MQTT_NO_MEMORY;
	}

	*pPubackPacket = pBuffer;
	( *pPacketSize ) = snprintf( ( char* ) pBuffer,
			mqttBLEPUBACK_MSG_LEN,
			mqttBLEPUBACK_MSG_FORMAT,
			mqttBLEMSG_TYPE_PUBACK,
			packetIdentifier
	);

	return AWS_IOT_MQTT_SUCCESS;

}

AwsIotMqttError_t AwsIotMqttBLE_DeserializePuback( const uint8_t * const pPubackStart,
                                                        size_t dataLength,
                                                        uint16_t * const pPacketIdentifier,
                                                        size_t * const pBytesProcessed )
{
	int16_t numTokens;
	jsmntok_t tokens[ mqttBLEMAX_MESG_TOKENS ];
	BaseType_t result;

	numTokens = JsonUtils_Parse( ( const char* ) pPubackStart, dataLength, tokens, mqttBLEMAX_MESG_TOKENS );

	if( numTokens <= 0 )
	{
		*pBytesProcessed = 0;
		AwsIotLogError( "Malformed MQTT packet received" );
		return AWS_IOT_MQTT_BAD_RESPONSE;
	}

	*pBytesProcessed = tokens[0].end;

	result = JsonUtils_GetInt16Value( ( const char* ) pPubackStart, tokens,
				numTokens, mqttBLEMESSAGE_ID, strlen( mqttBLEMESSAGE_ID ),
				( int16_t* ) pPacketIdentifier );

	if( result == pdFALSE )
	{
		AwsIotLogError( "Cannot find packet identifier in PUBACK packet" );
		return AWS_IOT_MQTT_BAD_RESPONSE;
	}

    return AWS_IOT_MQTT_SUCCESS;
}

AwsIotMqttError_t AwsIotMqttBLE_SerializeSubscribe( const AwsIotMqttSubscription_t * const pSubscriptionList,
                                                         size_t subscriptionCount,
                                                         uint8_t ** const pSubscribePacket,
                                                         size_t * const pPacketSize,
                                                         uint16_t * const pPacketIdentifier )
{
	uint8_t *pBuffer, *pTopicArrayBuf, *pQosArrayBuf;
	size_t bufLen, topicArrayLen, qosArrayLen;
	AwsIotMqttError_t result;


	result = prxSerializeTopicFilterArray( pSubscriptionList, subscriptionCount, &pTopicArrayBuf, &topicArrayLen );
	if( result != AWS_IOT_MQTT_SUCCESS )
	{
		AwsIotLogError( "Cannot serialize topic filter array" );
		return result;
	}


	result = prxSerializeQoSArray( pSubscriptionList, subscriptionCount, &pQosArrayBuf, &qosArrayLen );
	if( result != AWS_IOT_MQTT_SUCCESS )
	{
		AwsIotMqtt_FreeMessage( pTopicArrayBuf );
		AwsIotLogError( "Cannot serialize QoS array" );
		return result;
	}

	bufLen = mqttBLESUBSCRIBE_MSG_LEN( topicArrayLen, qosArrayLen );
	pBuffer = AwsIotMqtt_MallocMessage( bufLen );
	if( pBuffer == NULL )
	{
		AwsIotMqtt_FreeMessage( pTopicArrayBuf );
		AwsIotMqtt_FreeMessage( pQosArrayBuf );
		AwsIotLogError( "Failed to allocate memory for SUBSCRIBE message" );
		return AWS_IOT_MQTT_NO_MEMORY;
	}

	*pPacketIdentifier = prusNextPacketIdentifier();
	( *pPacketSize ) = snprintf( ( char* ) pBuffer,
			bufLen,
			mqttBLESUBSCRIBE_MSG_FORMAT,
			mqttBLEMSG_TYPE_SUBSCRIBE,
			topicArrayLen,
			(const char*) pTopicArrayBuf,
			qosArrayLen,
			(const char*) pQosArrayBuf,
			( *pPacketIdentifier ) );
	*pSubscribePacket = pBuffer;

	AwsIotMqtt_FreeMessage( pTopicArrayBuf );
	AwsIotMqtt_FreeMessage( pQosArrayBuf );

	return AWS_IOT_MQTT_SUCCESS;
}

AwsIotMqttError_t AwsIotMqttBLE_DeserializeSuback( AwsIotMqttConnection_t mqttConnection,
                                                        const uint8_t * const pSubackStart,
                                                        size_t dataLength,
                                                        uint16_t * const pPacketIdentifier,
                                                        size_t * const pBytesProcessed )
{

	int16_t numTokens;
	jsmntok_t tokens[ mqttBLEMAX_MESG_TOKENS ];
	BaseType_t result;
	int16_t subscriptionStatus;
	AwsIotMqttError_t status;
	_mqttConnection_t * pMqttConnection = ( _mqttConnection_t * ) mqttConnection;

	numTokens = JsonUtils_Parse( ( const char* ) pSubackStart, dataLength, tokens, mqttBLEMAX_MESG_TOKENS );

	if( numTokens <= 0 )
	{
		*pBytesProcessed = 0;
		AwsIotLogError( "Malformed MQTT packet received" );
		return AWS_IOT_MQTT_BAD_RESPONSE;
	}

	*pBytesProcessed = tokens[0].end;

	result = JsonUtils_GetInt16Value(
			( const char* ) pSubackStart,
			tokens,
			numTokens,
			mqttBLEMESSAGE_ID,
			strlen( mqttBLEMESSAGE_ID ),
			( int16_t* ) pPacketIdentifier );

	if( result == pdFALSE )
	{
		AwsIotLogError( "Cannot find packet identifier field in SUBACK packet" );
		return AWS_IOT_MQTT_BAD_RESPONSE;
	}

	/* Extract the return code from the packet. */
	result = JsonUtils_GetInt16Value( ( const char* ) pSubackStart,
			tokens,
			numTokens,
			mqttBLESTATUS,
			strlen( mqttBLESTATUS ),
			&subscriptionStatus );

	if( result == pdFALSE )
	{
		AwsIotLogError( "Cannot find response status field in SUBACK packet, id:%d", ( *pPacketIdentifier  ) );
		return AWS_IOT_MQTT_BAD_RESPONSE;
	}

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
		AwsIotMqttInternal_RemoveSubscriptionByPacket( pMqttConnection,
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

	uint8_t *pBuffer, *pTopicArrayBuf;
	size_t bufLen, topicArrayLen;
	AwsIotMqttError_t result;

	result = prxSerializeTopicFilterArray( pSubscriptionList, subscriptionCount, &pTopicArrayBuf, &topicArrayLen );
	if( result != AWS_IOT_MQTT_SUCCESS )
	{
		AwsIotLogError( "Cannot serialize topic filter array" );
		return result;
	}

	bufLen = mqttBLEUNSUBSCRIBE_MSG_LEN( topicArrayLen );
	pBuffer = AwsIotMqtt_MallocMessage( bufLen );
	if( pBuffer == NULL )
	{
		AwsIotMqtt_FreeMessage( pTopicArrayBuf );
		AwsIotLogError( "Failed to allocate memory for SUBSCRIBE message" );
		return AWS_IOT_MQTT_NO_MEMORY;
	}

	*pPacketIdentifier = prusNextPacketIdentifier();
	( *pPacketSize ) = snprintf( ( char* ) pBuffer,
			bufLen,
			mqttBLEUNSUBSCRIBE_MSG_FORMAT,
			mqttBLEMSG_TYPE_UNSUBSCRIBE,
			topicArrayLen,
			(const char *) pTopicArrayBuf,
			( *pPacketIdentifier ));
	*pUnsubscribePacket = pBuffer;

	AwsIotMqtt_FreeMessage( pTopicArrayBuf );

	return AWS_IOT_MQTT_SUCCESS;
}

AwsIotMqttError_t AwsIotMqttBLE_DeserializeUnsuback( const uint8_t * const pUnsubackStart,
                                                          size_t dataLength,
                                                          uint16_t * const pPacketIdentifier,
                                                          size_t * const pBytesProcessed )
{
	int16_t numTokens;
	jsmntok_t tokens[ mqttBLEMAX_MESG_TOKENS ];
	BaseType_t result;

	numTokens = JsonUtils_Parse( ( const char* ) pUnsubackStart, dataLength, tokens, mqttBLEMAX_MESG_TOKENS );

	if( numTokens <= 0 )
	{
		*pBytesProcessed = 0;
		AwsIotLogError( "Malformed MQTT packet received" );
		return AWS_IOT_MQTT_BAD_RESPONSE;
	}

	*pBytesProcessed = tokens[0].end;

	result = JsonUtils_GetInt16Value(
			( const char* ) pUnsubackStart,
			tokens,
			numTokens,
			mqttBLEMESSAGE_ID,
			strlen( mqttBLEMESSAGE_ID ),
			( int16_t* ) pPacketIdentifier );

	if( result == pdFALSE )
	{
		AwsIotLogError( "Cannot find packet identifier field in UnSubACK packet" );
		return AWS_IOT_MQTT_BAD_RESPONSE;
	}

	return AWS_IOT_MQTT_SUCCESS;
}

AwsIotMqttError_t AwsIotMqttBLE_SerializeDisconnect( uint8_t ** const pDisconnectPacket,
                                                          size_t * const pPacketSize )
{
	uint8_t *pBuffer;
	pBuffer = AwsIotMqtt_MallocMessage( mqttBLEDISCONNECT_MSG_LEN );
	if( pBuffer == NULL )
	{
		AwsIotLogError( "Failed to allocate memory for DISCONNECT message" );
		return AWS_IOT_MQTT_NO_MEMORY;
	}

	( *pPacketSize ) = snprintf( ( char* ) pBuffer, mqttBLEDISCONNECT_MSG_LEN, mqttBLEDISCONNECT_MSG_FORMAT, mqttBLEMSG_TYPE_DISCONNECT );
	*pDisconnectPacket = pBuffer;

	return AWS_IOT_MQTT_SUCCESS;
}


uint8_t AwsIotMqttBLE_GetPacketType( const uint8_t * const pPacket, size_t packetSize )
{

	int16_t numTokens;
	jsmntok_t tokens[ mqttBLEMAX_MESG_TOKENS ];
	BaseType_t result;
	uint16_t packetType;

	numTokens = JsonUtils_Parse( ( const char* ) pPacket, packetSize, tokens, mqttBLEMAX_MESG_TOKENS );

	if( numTokens <= 0 )
	{
		AwsIotLogError( "Malformed MQTT packet received" );
		return ( uint8_t ) -1;
	}

	result = JsonUtils_GetInt16Value(
			( const char* ) pPacket,
			tokens,
			numTokens,
			mqttBLEMSG_TYPE,
			strlen( mqttBLEMSG_TYPE ),
			( int16_t* ) &packetType );

	if( result == pdFALSE )
	{
		AwsIotLogError( "No packet type found" );
		return ( uint8_t ) -1;
	}
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
