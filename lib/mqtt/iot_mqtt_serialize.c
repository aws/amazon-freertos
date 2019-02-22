/*
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
 */

/**
 * @file iot_mqtt_serialize.c
 * @brief Implements functions that generate and decode MQTT network packets.
 */

/* Build using a config header, if provided. */
#ifdef IOT_CONFIG_FILE
    #include IOT_CONFIG_FILE
#endif

/* Standard includes. */
#include <string.h>

/* Error handling include. */
#include "private/iot_error.h"

/* MQTT internal includes. */
#include "private/iot_mqtt_internal.h"

/* Platform layer includes. */
#include "platform/iot_threads.h"

/*-----------------------------------------------------------*/

/*
 * Macros for reading the high and low byte of a 2-byte unsigned int.
 */
#define _UINT16_HIGH_BYTE( x )    ( ( uint8_t ) ( x >> 8 ) )            /**< @brief Get high byte. */
#define _UINT16_LOW_BYTE( x )     ( ( uint8_t ) ( x & 0x00ff ) )        /**< @brief Get low byte. */

/**
 * @brief Macro for decoding a 2-byte unsigned int from a sequence of bytes.
 *
 * @param[in] ptr A uint8_t* that points to the high byte.
 */
#define _UINT16_DECODE( ptr )                               \
    ( uint16_t ) ( ( ( ( uint16_t ) ( *( ptr ) ) ) << 8 ) | \
                   ( ( uint16_t ) ( *( ptr + 1 ) ) ) )

/**
 * @brief Macro for setting a bit in a 1-byte unsigned int.
 *
 * @param[in] x The unsigned int to set.
 * @param[in] position Which bit to set.
 */
#define _UINT8_SET_BIT( x, position )      ( x = ( uint8_t ) ( x | ( 0x01 << position ) ) )

/**
 * @brief Macro for checking if a bit is set in a 1-byte unsigned int.
 *
 * @param[in] x The unsigned int to check.
 * @param[in] position Which bit to check.
 */
#define _UINT8_CHECK_BIT( x, position )    ( ( x & ( 0x01 << position ) ) == ( 0x01 << position ) )

/*
 * Positions of each flag in the "Connect Flag" field of an MQTT CONNECT
 * packet.
 */
#define _MQTT_CONNECT_FLAG_CLEAN                     ( 1 ) /**< @brief Clean session. */
#define _MQTT_CONNECT_FLAG_WILL                      ( 2 ) /**< @brief Will present. */
#define _MQTT_CONNECT_FLAG_WILL_QOS1                 ( 3 ) /**< @brief Will QoS1. */
#define _MQTT_CONNECT_FLAG_WILL_QOS2                 ( 4 ) /**< @brief Will QoS2. */
#define _MQTT_CONNECT_FLAG_WILL_RETAIN               ( 5 ) /**< @brief Will retain. */
#define _MQTT_CONNECT_FLAG_PASSWORD                  ( 6 ) /**< @brief Password present. */
#define _MQTT_CONNECT_FLAG_USERNAME                  ( 7 ) /**< @brief Username present. */

/*
 * Positions of each flag in the first byte of an MQTT PUBLISH packet's
 * fixed header.
 */
#define _MQTT_PUBLISH_FLAG_RETAIN                    ( 0 ) /**< @brief Message retain flag. */
#define _MQTT_PUBLISH_FLAG_QOS1                      ( 1 ) /**< @brief Publish QoS 1. */
#define _MQTT_PUBLISH_FLAG_QOS2                      ( 2 ) /**< @brief Publish QoS 2. */
#define _MQTT_PUBLISH_FLAG_DUP                       ( 3 ) /**< @brief Duplicate message. */

/**
 * @brief The constant specifying MQTT version 3.1.1. Placed in the CONNECT packet.
 */
#define _MQTT_VERSION_3_1_1                          ( ( uint8_t ) 4U )

/**
 * @brief Per the MQTT 3.1.1 spec, the largest "Remaining Length" of an MQTT
 * packet is this value.
 */
#define _MQTT_MAX_REMAINING_LENGTH                   ( 268435455UL )

/**
 * @brief The maximum possible size of a CONNECT packet.
 *
 * All strings in a CONNECT packet are constrained to 2-byte lengths, giving a
 * maximum length smaller than the max "Remaining Length" constant above.
 */
#define _MQTT_PACKET_CONNECT_MAX_SIZE                ( 327700UL )

/*
 * Constants relating to CONNACK packets, defined by MQTT 3.1.1 spec.
 */
#define _MQTT_PACKET_CONNACK_SIZE                    ( 4 )                /**< @brief A CONNACK packet is always 4 bytes in size. */
#define _MQTT_PACKET_CONNACK_REMAINING_LENGTH        ( ( uint8_t ) 2 )    /**< @brief A CONNACK packet always has a "Remaining length" of 2. */
#define _MQTT_PACKET_CONNACK_SESSION_PRESENT_MASK    ( ( uint8_t ) 0x01 ) /**< @brief The "Session Present" bit is always the lowest bit. */

/*
 * Constants relating to PUBLISH and PUBACK packets, defined by MQTT
 * 3.1.1 spec.
 */
#define _MQTT_PACKET_PUBLISH_MINIMUM_SIZE            ( 5 )               /**< @brief The size of the smallest valid PUBLISH packet. */
#define _MQTT_PACKET_PUBACK_SIZE                     ( 4 )               /**< @brief A PUBACK packet is always 4 bytes in size. */
#define _MQTT_PACKET_PUBACK_REMAINING_LENGTH         ( ( uint8_t ) 2 )   /**< @brief A PUBACK packet always has a "Remaining length" of 2. */

/*
 * Constants relating to SUBACK and UNSUBACK packets, defined by MQTT
 * 3.1.1 spec.
 */
#define _MQTT_PACKET_SUBACK_MINIMUM_SIZE             ( 5 )               /**< @brief The size of the smallest valid SUBACK packet. */
#define _MQTT_PACKET_UNSUBACK_SIZE                   ( 4 )               /**< @brief An UNSUBACK packet is always 4 bytes in size. */
#define _MQTT_PACKET_UNSUBACK_REMAINING_LENGTH       ( ( uint8_t ) 2 )   /**< @brief An UNSUBACK packet always has a "Remaining length" of 2. */

/*
 * Constants relating to PINGREQ and PINGRESP packets, defined by MQTT 3.1.1 spec.
 */
#define _MQTT_PACKET_PINGREQ_SIZE                    ( 2 ) /**< @brief A PINGREQ packet is always 2 bytes in size. */
#define _MQTT_PACKET_PINGRESP_SIZE                   ( 2 ) /**< @brief A PINGRESP packet is always 2 bytes in size. */
#define _MQTT_PACKET_PINGRESP_REMAINING_LENGTH       ( 0 ) /**< @brief A PINGRESP packet always has a "Remaining length" of 0. */

/*
 * Constants relating to DISCONNECT packets, defined by MQTT 3.1.1 spec.
 */
#define _MQTT_PACKET_DISCONNECT_SIZE                 ( 2 ) /**< @brief A DISCONNECT packet is always 2 bytes in size. */

/*
 * Username for metrics with AWS IoT.
 */
#if AWS_IOT_MQTT_ENABLE_METRICS == 1 || DOXYGEN == 1
    #ifndef IOT_SDK_VERSION
        #error "IOT_SDK_VERSION must be defined."
    #endif

    #define _AWS_IOT_METRICS_USERNAME           ( "?SDK=C&Version=" IOT_SDK_VERSION )                    /**< @brief Specify "C SDK" and SDK version. */
    #define _AWS_IOT_METRICS_USERNAME_LENGTH    ( ( uint16_t ) sizeof( _AWS_IOT_METRICS_USERNAME ) - 1 ) /**< @brief Length of #_AWS_IOT_METRICS_USERNAME. */
#endif

/*-----------------------------------------------------------*/

/**
 * @brief Generate and return a 2-byte packet identifier.
 *
 * This packet identifier will be nonzero.
 *
 * @return The packet identifier.
 */
static uint16_t _nextPacketIdentifier( void );

/**
 * @brief Calculate the number of bytes required to encode an MQTT
 * "Remaining length" field.
 *
 * @param[in] length The value of the "Remaining length" to encode.
 *
 * @return The size of the encoding of length. This is always `1`, `2`, `3`, or `4`.
 */
static size_t _remainingLengthEncodedSize( size_t length );

/**
 * @brief Encode the "Remaining length" field per MQTT spec.
 *
 * @param[out] pDestination Where to write the encoded "Remaining length".
 * @param[in] length The "Remaining length" to encode.
 *
 * @return Pointer to the end of the encoded "Remaining length", which is 1-4
 * bytes greater than `pDestination`.
 *
 * @warning This function does not check the size of `pDestination`! Ensure that
 * `pDestination` is large enough to hold the encoded "Remaining length" using
 * the function #_remainingLengthEncodedSize to avoid buffer overflows.
 */
static uint8_t * _encodeRemainingLength( uint8_t * pDestination,
                                         size_t length );

/**
 * @brief Decodes the "Remaining length" field in an MQTT packet.
 *
 * @param[in] pSource Pointer to the beginning of remaining length.
 * @param[out] pEnd Set to point to the byte after the encoded "Remaining length".
 * @param[out] pLength Set to the decoded remaining length.
 *
 * @return Pointer to the end of the encoded "Remaining length", which is 1-4
 * bytes greater than pSource.
 *
 * @warning This function does not check the size of `pSource`!
 */
static IotMqttError_t _decodeRemainingLength( const uint8_t * pSource,
                                              const uint8_t ** pEnd,
                                              size_t * pLength );

/**
 * @brief Encode a C string as a UTF-8 string, per MQTT 3.1.1 spec.
 *
 * @param[out] pDestination Where to write the encoded string.
 * @param[in] source The string to encode.
 * @param[in] sourceLength The length of source.
 *
 * @return Pointer to the end of the encoded string, which is `sourceLength+2`
 * bytes greater than `pDestination`.
 *
 * @warning This function does not check the size of `pDestination`! Ensure that
 * `pDestination` is large enough to hold `sourceLength+2` bytes to avoid a buffer
 * overflow.
 */
static uint8_t * _encodeString( uint8_t * pDestination,
                                const char * source,
                                uint16_t sourceLength );

/**
 * @brief Calculate the size and "Remaining length" of a CONNECT packet generated
 * from the given parameters.
 *
 * @param[in] pConnectInfo User-provided CONNECT information struct.
 * @param[out] pRemainingLength Output for calculated "Remaining length" field.
 * @param[out] pPacketSize Output for calculated total packet size.
 *
 * @return `true` if the packet is within the length allowed by MQTT 3.1.1 spec; `false`
 * otherwise. If this function returns `false`, the output parameters should be ignored.
 */
static bool _connectPacketSize( const IotMqttConnectInfo_t * pConnectInfo,
                                size_t * pRemainingLength,
                                size_t * pPacketSize );

/**
 * @brief Calculate the size and "Remaining length" of a PUBLISH packet generated
 * from the given parameters.
 *
 * @param[in] pPublishInfo User-provided PUBLISH information struct.
 * @param[out] pRemainingLength Output for calculated "Remaining length" field.
 * @param[out] pPacketSize Output for calculated total packet size.
 *
 * @return `true` if the packet is within the length allowed by MQTT 3.1.1 spec; `false`
 * otherwise. If this function returns `false`, the output parameters should be ignored.
 */
static bool _publishPacketSize( const IotMqttPublishInfo_t * pPublishInfo,
                                size_t * pRemainingLength,
                                size_t * pPacketSize );

/**
 * @brief Calculate the size and "Remaining length" of a SUBSCRIBE or UNSUBSCRIBE
 * packet generated from the given parameters.
 *
 * @param[in] type Either IOT_MQTT_SUBSCRIBE or IOT_MQTT_UNSUBSCRIBE.
 * @param[in] pSubscriptionList User-provided array of subscriptions.
 * @param[in] subscriptionCount Size of `pSubscriptionList`.
 * @param[out] pRemainingLength Output for calculated "Remaining length" field.
 * @param[out] pPacketSize Output for calculated total packet size.
 *
 * @return `true` if the packet is within the length allowed by MQTT 3.1.1 spec; `false`
 * otherwise. If this function returns `false`, the output parameters should be ignored.
 */
static bool _subscriptionPacketSize( IotMqttOperationType_t type,
                                     const IotMqttSubscription_t * pSubscriptionList,
                                     size_t subscriptionCount,
                                     size_t * pRemainingLength,
                                     size_t * pPacketSize );

/*-----------------------------------------------------------*/

#if _LIBRARY_LOG_LEVEL > IOT_LOG_NONE

/**
 * @brief If logging is enabled, define a log configuration that only prints the log
 * string. This is used when printing out details of deserialized MQTT packets.
 */
    static const IotLogConfig_t _logHideAll =
    {
        .hideLibraryName = true,
        .hideLogLevel    = true,
        .hideTimestring  = true
    };
#endif

/**
 * @brief Guards access to the packet identifier counter.
 *
 * Each packet should have a unique packet identifier. This mutex ensures that only
 * one thread at a time may read the global packet identifer.
 */
static IotMutex_t _packetIdentifierMutex;

/*-----------------------------------------------------------*/

static uint16_t _nextPacketIdentifier( void )
{
    static uint16_t nextPacketIdentifier = 1;
    uint16_t newPacketIdentifier = 0;

    /* Lock the packet identifier mutex so that only one thread may read and
     * modify nextPacketIdentifier. */
    IotMutex_Lock( &( _packetIdentifierMutex ) );

    /* Read the next packet identifier. */
    newPacketIdentifier = nextPacketIdentifier;

    /* The next packet identifier will be greater by 2. This prevents packet
     * identifiers from ever being 0, which is not allowed by MQTT 3.1.1. Packet
     * identifiers will follow the sequence 1,3,5...65535,1,3,5... */
    nextPacketIdentifier = ( uint16_t ) ( nextPacketIdentifier + ( ( uint16_t ) 2 ) );

    /* Unlock the packet identifier mutex. */
    IotMutex_Unlock( &( _packetIdentifierMutex ) );

    return newPacketIdentifier;
}

/*-----------------------------------------------------------*/

static size_t _remainingLengthEncodedSize( size_t length )
{
    size_t encodedSize = 0;

    /* length should have already been checked before calling this function. */
    IotMqtt_Assert( length <= _MQTT_MAX_REMAINING_LENGTH );

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

static uint8_t * _encodeRemainingLength( uint8_t * pDestination,
                                         size_t length )
{
    uint8_t lengthByte = 0, * pLengthEnd = pDestination;

    /* This algorithm is copied from the MQTT v3.1.1 spec. */
    do
    {
        lengthByte = length % 128;
        length = length / 128;

        /* Set the high bit of this byte, indicating that there's more data. */
        if( length > 0 )
        {
            _UINT8_SET_BIT( lengthByte, 7 );
        }
        else
        {
            _EMPTY_ELSE_MARKER;
        }

        /* Output a single encoded byte. */
        *pLengthEnd = lengthByte;
        pLengthEnd++;
    } while( length > 0 );

    return pLengthEnd;
}

/*-----------------------------------------------------------*/

static IotMqttError_t _decodeRemainingLength( const uint8_t * pSource,
                                              const uint8_t ** pEnd,
                                              size_t * const pLength )
{
    IotMqttError_t status = IOT_MQTT_SUCCESS;
    uint8_t encodedByte = 0;
    size_t remainingLength = 0, multiplier = 1, bytesDecoded = 0, expectedSize = 0;

    /* This algorithm is copied from the MQTT v3.1.1 spec. */
    do
    {
        if( multiplier > 2097152 ) /* 128 ^ 3 */
        {
            status = IOT_MQTT_BAD_PARAMETER;
            break;
        }
        else
        {
            encodedByte = *pSource;
            pSource++;

            remainingLength += ( encodedByte & 0x7F ) * multiplier;
            multiplier *= 128;
            bytesDecoded++;
        }
    } while( ( encodedByte & 0x80 ) != 0 );

    if( status == IOT_MQTT_SUCCESS )
    {
        expectedSize = _remainingLengthEncodedSize( remainingLength );

        if( bytesDecoded != expectedSize )
        {
            status = IOT_MQTT_BAD_PARAMETER;
        }
        else
        {
            /* Valid remaining length should be at most 4 bytes. */
            IotMqtt_Assert( bytesDecoded <= 4 );

            /* Set the output parameters. */
            if( pLength != NULL )
            {
                *pLength = remainingLength;
            }
            else
            {
                _EMPTY_ELSE_MARKER;
            }

            if( pEnd != NULL )
            {
                *pEnd = pSource;
            }
            else
            {
                _EMPTY_ELSE_MARKER;
            }
        }
    }
    else
    {
        _EMPTY_ELSE_MARKER;
    }

    return status;
}

/*-----------------------------------------------------------*/

static uint8_t * _encodeString( uint8_t * pDestination,
                                const char * source,
                                uint16_t sourceLength )
{
    /* The first byte of a UTF-8 string is the high byte of the string length. */
    *pDestination = _UINT16_HIGH_BYTE( sourceLength );
    pDestination++;

    /* The second byte of a UTF-8 string is the low byte of the string length. */
    *pDestination = _UINT16_LOW_BYTE( sourceLength );
    pDestination++;

    /* Copy the string into pDestination. */
    ( void ) memcpy( pDestination, source, sourceLength );

    /* Return the pointer to the end of the encoded string. */
    pDestination += sourceLength;

    return pDestination;
}

/*-----------------------------------------------------------*/

static bool _connectPacketSize( const IotMqttConnectInfo_t * pConnectInfo,
                                size_t * pRemainingLength,
                                size_t * pPacketSize )
{
    bool status = true;
    size_t connectPacketSize = 0, remainingLength = 0;

    /* The CONNECT packet will always include a 10-byte variable header. */
    connectPacketSize += 10U;

    /* Add the length of the client identifier if provided. */
    if( pConnectInfo->clientIdentifierLength > 0 )
    {
        connectPacketSize += pConnectInfo->clientIdentifierLength + sizeof( uint16_t );
    }
    else
    {
        _EMPTY_ELSE_MARKER;
    }

    /* Add the lengths of the will message and topic name if provided. */
    if( pConnectInfo->pWillInfo != NULL )
    {
        connectPacketSize += pConnectInfo->pWillInfo->topicNameLength + sizeof( uint16_t ) +
                             pConnectInfo->pWillInfo->payloadLength + sizeof( uint16_t );
    }
    else
    {
        _EMPTY_ELSE_MARKER;
    }

    /* Depending on the status of metrics, add the length of the metrics username
     * or the user-provided username. */
    if( pConnectInfo->awsIotMqttMode == true )
    {
        #if AWS_IOT_MQTT_ENABLE_METRICS == 1
            connectPacketSize += _AWS_IOT_METRICS_USERNAME_LENGTH + sizeof( uint16_t );
        #endif
    }
    else
    {
        /* Add the lengths of the username and password if provided and not
         * connecting to an AWS IoT MQTT server. */
        if( pConnectInfo->pUserName != NULL )
        {
            connectPacketSize += pConnectInfo->userNameLength + sizeof( uint16_t );
        }
        else
        {
            _EMPTY_ELSE_MARKER;
        }

        if( pConnectInfo->pPassword != NULL )
        {
            connectPacketSize += pConnectInfo->passwordLength + sizeof( uint16_t );
        }
        else
        {
            _EMPTY_ELSE_MARKER;
        }
    }

    /* At this point, the "Remaining Length" field of the MQTT CONNECT packet has
     * been calculated. */
    remainingLength = connectPacketSize;

    /* Calculate the full size of the MQTT CONNECT packet by adding the size of
     * the "Remaining Length" field plus 1 byte for the "Packet Type" field. */
    connectPacketSize += 1 + _remainingLengthEncodedSize( connectPacketSize );

    /* Check that the CONNECT packet is within the bounds of the MQTT spec. */
    if( connectPacketSize > _MQTT_PACKET_CONNECT_MAX_SIZE )
    {
        status = false;
    }
    else
    {
        *pRemainingLength = remainingLength;
        *pPacketSize = connectPacketSize;
    }

    return status;
}

/*-----------------------------------------------------------*/

static bool _publishPacketSize( const IotMqttPublishInfo_t * pPublishInfo,
                                size_t * pRemainingLength,
                                size_t * pPacketSize )
{
    bool status = true;
    size_t publishPacketSize = 0;

    /* The variable header of a PUBLISH packet always contains the topic name. */
    publishPacketSize += pPublishInfo->topicNameLength + sizeof( uint16_t );

    /* The variable header of a QoS 1 or 2 PUBLISH packet contains a 2-byte
     * packet identifier. */
    if( pPublishInfo->qos > IOT_MQTT_QOS_0 )
    {
        publishPacketSize += sizeof( uint16_t );
    }
    else
    {
        _EMPTY_ELSE_MARKER;
    }

    /* Add the length of the PUBLISH payload. At this point, the "Remaining length"
     * has been calculated. Return error if the "Remaining length" exceeds what is
     * allowed by MQTT 3.1.1. Otherwise, set the output parameter. */
    publishPacketSize += pPublishInfo->payloadLength;

    if( publishPacketSize > _MQTT_MAX_REMAINING_LENGTH )
    {
        status = false;
    }
    else
    {
        *pRemainingLength = publishPacketSize;

        /* Calculate the full size of the MQTT PUBLISH packet by adding the size of
         * the "Remaining Length" field plus 1 byte for the "Packet Type" field. Set
         * the pPacketSize output parameter. */
        publishPacketSize += 1 + _remainingLengthEncodedSize( publishPacketSize );
        *pPacketSize = publishPacketSize;
    }

    return status;
}

/*-----------------------------------------------------------*/

static bool _subscriptionPacketSize( IotMqttOperationType_t type,
                                     const IotMqttSubscription_t * pSubscriptionList,
                                     size_t subscriptionCount,
                                     size_t * pRemainingLength,
                                     size_t * pPacketSize )
{
    bool status = true;
    size_t i = 0, subscriptionPacketSize = 0;

    /* Only SUBSCRIBE and UNSUBSCRIBE operations should call this function. */
    IotMqtt_Assert( ( type == IOT_MQTT_SUBSCRIBE ) || ( type == IOT_MQTT_UNSUBSCRIBE ) );

    /* The variable header of a subscription packet consists of a 2-byte packet
     * identifier. */
    subscriptionPacketSize += sizeof( uint16_t );

    /* Sum the lengths of all subscription topic filters; add 1 byte for each
     * subscription's QoS if type is IOT_MQTT_SUBSCRIBE. */
    for( i = 0; i < subscriptionCount; i++ )
    {
        /* Add the length of the topic filter. */
        subscriptionPacketSize += pSubscriptionList[ i ].topicFilterLength + sizeof( uint16_t );

        /* Only SUBSCRIBE packets include the QoS. */
        if( type == IOT_MQTT_SUBSCRIBE )
        {
            subscriptionPacketSize += 1;
        }
        else
        {
            _EMPTY_ELSE_MARKER;
        }
    }

    /* At this point, the "Remaining length" has been calculated. Return error
     * if the "Remaining length" exceeds what is allowed by MQTT 3.1.1. Otherwise,
     * set the output parameter.*/
    if( subscriptionPacketSize > _MQTT_MAX_REMAINING_LENGTH )
    {
        status = false;
    }
    else
    {
        *pRemainingLength = subscriptionPacketSize;

        /* Calculate the full size of the subscription packet by adding the size of the
         * "Remaining length" field plus 1 byte for the "Packet type" field. Set the
         * pPacketSize output parameter. */
        subscriptionPacketSize += 1 + _remainingLengthEncodedSize( subscriptionPacketSize );
        *pPacketSize = subscriptionPacketSize;
    }

    return status;
}

/*-----------------------------------------------------------*/

IotMqttError_t _IotMqtt_InitSerialize( void )
{
    _IOT_FUNCTION_ENTRY( IotMqttError_t, IOT_MQTT_SUCCESS );
    bool packetMutexCreated = false;

    /* Create the packet identifier mutex. */
    packetMutexCreated = IotMutex_Create( &( _packetIdentifierMutex ), false );

    if( packetMutexCreated == false )
    {
        _IOT_SET_AND_GOTO_CLEANUP( IOT_MQTT_INIT_FAILED );
    }
    else
    {
        _EMPTY_ELSE_MARKER;
    }

    /* Call any additional serializer initialization function if serializer
     * overrides are enabled. */
    #if IOT_MQTT_ENABLE_SERIALIZER_OVERRIDES == 1
        #ifdef _IotMqtt_InitSerializeAdditional
            if( _IotMqtt_InitSerializeAdditional() == false )
            {
                _IOT_SET_AND_GOTO_CLEANUP( IOT_MQTT_INIT_FAILED );
            }
            else
            {
                _EMPTY_ELSE_MARKER;
            }
        #endif
    #endif

    _IOT_FUNCTION_CLEANUP_BEGIN();

    /* Clean up on error. */
    if( status != IOT_MQTT_SUCCESS )
    {
        if( packetMutexCreated == true )
        {
            IotMutex_Destroy( &( _packetIdentifierMutex ) );
        }
        else
        {
            _EMPTY_ELSE_MARKER;
        }
    }
    else
    {
        _EMPTY_ELSE_MARKER;
    }

    _IOT_FUNCTION_CLEANUP_END();
}

/*-----------------------------------------------------------*/

void _IotMqtt_CleanupSerialize( void )
{
    /* Destroy the packet identifier mutex. */
    IotMutex_Destroy( &( _packetIdentifierMutex ) );

    /* Call any additional serializer cleanup initialization function is serializer
     * overrides are enabled. */
    #if IOT_MQTT_ENABLE_SERIALIZER_OVERRIDES == 1
        #ifdef _IotMqtt_CleanupSerializeAdditional
            _IotMqtt_CleanupSerializeAdditional();
        #endif
    #endif
}

/*-----------------------------------------------------------*/

uint8_t _IotMqtt_GetPacketType( const uint8_t * pPacket,
                                size_t packetSize )
{
    uint8_t packetType = 0;

    if( packetSize > 0 )
    {
        /* The MQTT packet type is in the first byte of the packet. Mask out the
         * lower bits to ignore flags. */
        packetType = *pPacket & 0xf0;
    }
    else
    {
        _EMPTY_ELSE_MARKER;
    }

    return packetType;
}

/*-----------------------------------------------------------*/

IotMqttError_t _IotMqtt_SerializeConnect( const IotMqttConnectInfo_t * pConnectInfo,
                                          uint8_t ** pConnectPacket,
                                          size_t * pPacketSize )
{
    _IOT_FUNCTION_ENTRY( IotMqttError_t, IOT_MQTT_SUCCESS );
    uint8_t connectFlags = 0;
    size_t remainingLength = 0, connectPacketSize = 0;
    uint8_t * pBuffer = NULL;

    /* Calculate the "Remaining length" field and total packet size. If it exceeds
     * what is allowed in the MQTT standard, return an error. */
    if( _connectPacketSize( pConnectInfo, &remainingLength, &connectPacketSize ) == false )
    {
        IotLogError( "Connect packet length exceeds %lu, which is the maximum"
                     " size allowed by MQTT 3.1.1.",
                     _MQTT_PACKET_CONNECT_MAX_SIZE );

        _IOT_SET_AND_GOTO_CLEANUP( IOT_MQTT_BAD_PARAMETER );
    }
    else
    {
        _EMPTY_ELSE_MARKER;
    }

    /* Total size of the connect packet should be larger than the "Remaining length"
     * field. */
    IotMqtt_Assert( connectPacketSize > remainingLength );

    /* Allocate memory to hold the CONNECT packet. */
    pBuffer = IotMqtt_MallocMessage( connectPacketSize );

    /* Check that sufficient memory was allocated. */
    if( pBuffer == NULL )
    {
        IotLogError( "Failed to allocate memory for CONNECT packet." );

        _IOT_SET_AND_GOTO_CLEANUP( IOT_MQTT_NO_MEMORY );
    }
    else
    {
        _EMPTY_ELSE_MARKER;
    }

    /* Set the output parameters. The remainder of this function always succeeds. */
    *pConnectPacket = pBuffer;
    *pPacketSize = connectPacketSize;

    /* The first byte in the CONNECT packet is the control packet type. */
    *pBuffer = _MQTT_PACKET_TYPE_CONNECT;
    pBuffer++;

    /* The remaining length of the CONNECT packet is encoded starting from the
     * second byte. The remaining length does not include the length of the fixed
     * header or the encoding of the remaining length. */
    pBuffer = _encodeRemainingLength( pBuffer, remainingLength );

    /* The string "MQTT" is placed at the beginning of the CONNECT packet's variable
     * header. This string is 4 bytes long. */
    pBuffer = _encodeString( pBuffer, "MQTT", 4 );

    /* The MQTT protocol version is the second byte of the variable header. */
    *pBuffer = _MQTT_VERSION_3_1_1;
    pBuffer++;

    /* Set the CONNECT flags based on the given parameters. */
    if( pConnectInfo->cleanSession == true )
    {
        _UINT8_SET_BIT( connectFlags, _MQTT_CONNECT_FLAG_CLEAN );
    }
    else
    {
        _EMPTY_ELSE_MARKER;
    }

    /* Username and password depend on MQTT mode. */
    if( pConnectInfo->awsIotMqttMode == true )
    {
        /* Set the username flag for AWS IoT metrics. The AWS IoT MQTT server
         * never uses a password. */
        #if AWS_IOT_MQTT_ENABLE_METRICS == 1
            _UINT8_SET_BIT( connectFlags, _MQTT_CONNECT_FLAG_USERNAME );
        #endif
    }
    else
    {
        /* Set the flags for username and password if provided. */
        if( pConnectInfo->pUserName != NULL )
        {
            _UINT8_SET_BIT( connectFlags, _MQTT_CONNECT_FLAG_USERNAME );
        }
        else
        {
            _EMPTY_ELSE_MARKER;
        }

        if( pConnectInfo->pPassword != NULL )
        {
            _UINT8_SET_BIT( connectFlags, _MQTT_CONNECT_FLAG_PASSWORD );
        }
        else
        {
            _EMPTY_ELSE_MARKER;
        }
    }

    /* Set will flag if an LWT is provided. */
    if( pConnectInfo->pWillInfo != NULL )
    {
        _UINT8_SET_BIT( connectFlags, _MQTT_CONNECT_FLAG_WILL );

        /* Flags only need to be changed for will QoS 1 and 2. */
        switch( pConnectInfo->pWillInfo->qos )
        {
            case IOT_MQTT_QOS_1:
                _UINT8_SET_BIT( connectFlags, _MQTT_CONNECT_FLAG_WILL_QOS1 );
                break;

            case IOT_MQTT_QOS_2:
                _UINT8_SET_BIT( connectFlags, _MQTT_CONNECT_FLAG_WILL_QOS2 );
                break;

            default:
                break;
        }

        if( pConnectInfo->pWillInfo->retain == true )
        {
            _UINT8_SET_BIT( connectFlags, _MQTT_CONNECT_FLAG_WILL_RETAIN );
        }
        else
        {
            _EMPTY_ELSE_MARKER;
        }
    }
    else
    {
        _EMPTY_ELSE_MARKER;
    }

    *pBuffer = connectFlags;
    pBuffer++;

    /* Write the 2 bytes of the keep alive interval into the CONNECT packet. */
    *pBuffer = _UINT16_HIGH_BYTE( pConnectInfo->keepAliveSeconds );
    *( pBuffer + 1 ) = _UINT16_LOW_BYTE( pConnectInfo->keepAliveSeconds );
    pBuffer += 2;

    /* Write the client identifier into the CONNECT packet. */
    pBuffer = _encodeString( pBuffer,
                             pConnectInfo->pClientIdentifier,
                             pConnectInfo->clientIdentifierLength );

    /* Write the will topic name and message into the CONNECT packet if provided. */
    if( pConnectInfo->pWillInfo != NULL )
    {
        pBuffer = _encodeString( pBuffer,
                                 pConnectInfo->pWillInfo->pTopicName,
                                 pConnectInfo->pWillInfo->topicNameLength );

        pBuffer = _encodeString( pBuffer,
                                 pConnectInfo->pWillInfo->pPayload,
                                 ( uint16_t ) pConnectInfo->pWillInfo->payloadLength );
    }
    else
    {
        _EMPTY_ELSE_MARKER;
    }

    /* If metrics are enabled, write the metrics username into the CONNECT packet.
     * Otherwise, write the username and password only when not connecting to an
     * AWS IoT MQTT server. */
    if( pConnectInfo->awsIotMqttMode == true )
    {
        #if AWS_IOT_MQTT_ENABLE_METRICS == 1
            IotLogInfo( "Anonymous metrics (SDK language, SDK version) will be provided to AWS IoT. "
                        "Recompile with AWS_IOT_MQTT_ENABLE_METRICS set to 0 to disable." );

            pBuffer = _encodeString( pBuffer,
                                     _AWS_IOT_METRICS_USERNAME,
                                     _AWS_IOT_METRICS_USERNAME_LENGTH );
        #endif
    }
    else
    {
        if( pConnectInfo->pUserName != NULL )
        {
            pBuffer = _encodeString( pBuffer,
                                     pConnectInfo->pUserName,
                                     pConnectInfo->userNameLength );
        }
        else
        {
            _EMPTY_ELSE_MARKER;
        }

        if( pConnectInfo->pPassword != NULL )
        {
            pBuffer = _encodeString( pBuffer,
                                     pConnectInfo->pPassword,
                                     pConnectInfo->passwordLength );
        }
        else
        {
            _EMPTY_ELSE_MARKER;
        }
    }

    /* Ensure that the difference between the end and beginning of the buffer
     * is equal to connectPacketSize, i.e. pBuffer did not overflow. */
    IotMqtt_Assert( ( size_t ) ( pBuffer - *pConnectPacket ) == connectPacketSize );

    /* Print out the serialized CONNECT packet for debugging purposes. */
    IotLog_PrintBuffer( "MQTT CONNECT packet:", *pConnectPacket, connectPacketSize );

    _IOT_FUNCTION_EXIT_NO_CLEANUP();
}

/*-----------------------------------------------------------*/

IotMqttError_t _IotMqtt_DeserializeConnack( const uint8_t * pConnackStart,
                                            size_t dataLength,
                                            size_t * pBytesProcessed )
{
    _IOT_FUNCTION_ENTRY( IotMqttError_t, IOT_MQTT_SUCCESS );

    /* If logging is enabled, declare the CONNACK response code strings. The
     * fourth byte of CONNACK indexes into this array for the corresponding response. */
    #if _LIBRARY_LOG_LEVEL > IOT_LOG_NONE
        static const char * pConnackResponses[ 6 ] =
        {
            "Connection accepted.",                               /* 0 */
            "Connection refused: unacceptable protocol version.", /* 1 */
            "Connection refused: identifier rejected.",           /* 2 */
            "Connection refused: server unavailable",             /* 3 */
            "Connection refused: bad user name or password.",     /* 4 */
            "Connection refused: not authorized."                 /* 5 */
        };
    #endif

    /* According to MQTT 3.1.1, CONNACK packets are all 4 bytes in size. If the
     * data stream has fewer than 4 bytes, then the CONNACK packet is incomplete. */
    if( dataLength < _MQTT_PACKET_CONNACK_SIZE )
    {
        IotLog( IOT_LOG_ERROR,
                &_logHideAll,
                "CONNACK less than %d bytes in size.",
                _MQTT_PACKET_CONNACK_SIZE );
        *pBytesProcessed = 0;

        _IOT_SET_AND_GOTO_CLEANUP( IOT_MQTT_BAD_RESPONSE );
    }
    else
    {
        _EMPTY_ELSE_MARKER;
    }

    /* The next 4 bytes will be processed by this function. */
    *pBytesProcessed = _MQTT_PACKET_CONNACK_SIZE;

    /* Check that the control packet type is 0x20. */
    if( pConnackStart[ 0 ] != _MQTT_PACKET_TYPE_CONNACK )
    {
        IotLog( IOT_LOG_ERROR,
                &_logHideAll,
                "Bad control packet type 0x%02x.",
                pConnackStart[ 0 ] );

        _IOT_SET_AND_GOTO_CLEANUP( IOT_MQTT_BAD_RESPONSE );
    }
    else
    {
        _EMPTY_ELSE_MARKER;
    }

    /* According to MQTT 3.1.1, the second byte of CONNACK must specify a
     * "Remaining length" of 2. */
    if( pConnackStart[ 1 ] != _MQTT_PACKET_CONNACK_REMAINING_LENGTH )
    {
        IotLog( IOT_LOG_ERROR,
                &_logHideAll,
                "CONNACK does not have remaining length of %d.",
                _MQTT_PACKET_CONNACK_REMAINING_LENGTH );

        _IOT_SET_AND_GOTO_CLEANUP( IOT_MQTT_BAD_RESPONSE );
    }
    else
    {
        _EMPTY_ELSE_MARKER;
    }

    /* Check the reserved bits in CONNACK. The high 7 bits of the second byte
     * in CONNACK must be 0. */
    if( ( pConnackStart[ 2 ] | 0x01 ) != 0x01 )
    {
        IotLog( IOT_LOG_ERROR,
                &_logHideAll,
                "Reserved bits in CONNACK incorrect." );

        _IOT_SET_AND_GOTO_CLEANUP( IOT_MQTT_BAD_RESPONSE );
    }
    else
    {
        _EMPTY_ELSE_MARKER;
    }

    /* Determine if the "Session Present" bit it set. This is the lowest bit of
     * the second byte in CONNACK. */
    if( ( pConnackStart[ 2 ] & _MQTT_PACKET_CONNACK_SESSION_PRESENT_MASK )
        == _MQTT_PACKET_CONNACK_SESSION_PRESENT_MASK )
    {
        IotLog( IOT_LOG_DEBUG,
                &_logHideAll,
                "CONNACK session present bit set." );

        /* MQTT 3.1.1 specifies that the fourth byte in CONNACK must be 0 if the
         * "Session Present" bit is set. */
        if( pConnackStart[ 3 ] != 0 )
        {
            _IOT_SET_AND_GOTO_CLEANUP( IOT_MQTT_BAD_RESPONSE );
        }
        else
        {
            _EMPTY_ELSE_MARKER;
        }
    }
    else
    {
        IotLog( IOT_LOG_DEBUG,
                &_logHideAll,
                "CONNACK session present bit not set." );
    }

    /* In MQTT 3.1.1, only values 0 through 5 are valid CONNACK response codes. */
    if( pConnackStart[ 3 ] > 5 )
    {
        IotLog( IOT_LOG_DEBUG,
                &_logHideAll,
                "CONNACK response %hhu is not valid.",
                pConnackStart[ 3 ] );

        _IOT_SET_AND_GOTO_CLEANUP( IOT_MQTT_BAD_RESPONSE );
    }
    else
    {
        _EMPTY_ELSE_MARKER;
    }

    /* Print the appropriate message for the CONNACK response code if logs are
     * enabled. */
    #if _LIBRARY_LOG_LEVEL > IOT_LOG_NONE
        IotLog( IOT_LOG_DEBUG,
                &_logHideAll,
                "%s",
                pConnackResponses[ pConnackStart[ 3 ] ] );
    #endif

    /* A nonzero CONNACK response code means the connection was refused. */
    if( pConnackStart[ 3 ] > 0 )
    {
        _IOT_SET_AND_GOTO_CLEANUP( IOT_MQTT_SERVER_REFUSED );
    }
    else
    {
        _EMPTY_ELSE_MARKER;
    }

    _IOT_FUNCTION_EXIT_NO_CLEANUP();
}

/*-----------------------------------------------------------*/

IotMqttError_t _IotMqtt_SerializePublish( const IotMqttPublishInfo_t * pPublishInfo,
                                          uint8_t ** pPublishPacket,
                                          size_t * pPacketSize,
                                          uint16_t * pPacketIdentifier )
{
    _IOT_FUNCTION_ENTRY( IotMqttError_t, IOT_MQTT_SUCCESS );
    uint8_t publishFlags = 0;
    uint16_t packetIdentifier = 0;
    size_t remainingLength = 0, publishPacketSize = 0;
    uint8_t * pBuffer = NULL;

    /* Calculate the "Remaining length" field and total packet size. If it exceeds
     * what is allowed in the MQTT standard, return an error. */
    if( _publishPacketSize( pPublishInfo, &remainingLength, &publishPacketSize ) == false )
    {
        IotLogError( "Publish packet remaining length exceeds %lu, which is the "
                     "maximum size allowed by MQTT 3.1.1.",
                     _MQTT_MAX_REMAINING_LENGTH );

        _IOT_SET_AND_GOTO_CLEANUP( IOT_MQTT_BAD_PARAMETER );
    }
    else
    {
        _EMPTY_ELSE_MARKER;
    }

    /* Total size of the publish packet should be larger than the "Remaining length"
     * field. */
    IotMqtt_Assert( publishPacketSize > remainingLength );

    /* Allocate memory to hold the PUBLISH packet. */
    pBuffer = IotMqtt_MallocMessage( publishPacketSize );

    /* Check that sufficient memory was allocated. */
    if( pBuffer == NULL )
    {
        IotLogError( "Failed to allocate memory for PUBLISH packet." );

        _IOT_SET_AND_GOTO_CLEANUP( IOT_MQTT_NO_MEMORY );
    }
    else
    {
        _EMPTY_ELSE_MARKER;
    }

    /* Set the output parameters. The remainder of this function always succeeds. */
    *pPublishPacket = pBuffer;
    *pPacketSize = publishPacketSize;

    /* The first byte of a PUBLISH packet contains the packet type and flags. */
    publishFlags = _MQTT_PACKET_TYPE_PUBLISH;

    if( pPublishInfo->qos == IOT_MQTT_QOS_1 )
    {
        _UINT8_SET_BIT( publishFlags, _MQTT_PUBLISH_FLAG_QOS1 );
    }
    else if( pPublishInfo->qos == IOT_MQTT_QOS_2 )
    {
        _UINT8_SET_BIT( publishFlags, _MQTT_PUBLISH_FLAG_QOS2 );
    }
    else
    {
        _EMPTY_ELSE_MARKER;
    }

    if( pPublishInfo->retain == true )
    {
        _UINT8_SET_BIT( publishFlags, _MQTT_PUBLISH_FLAG_RETAIN );
    }
    else
    {
        _EMPTY_ELSE_MARKER;
    }

    *pBuffer = publishFlags;
    pBuffer++;

    /* The "Remaining length" is encoded from the second byte. */
    pBuffer = _encodeRemainingLength( pBuffer, remainingLength );

    /* The topic name is placed after the "Remaining length". */
    pBuffer = _encodeString( pBuffer,
                             pPublishInfo->pTopicName,
                             pPublishInfo->topicNameLength );

    /* A packet identifier is required for QoS 1 and 2 messages. */
    if( pPublishInfo->qos > IOT_MQTT_QOS_0 )
    {
        /* Get the next packet identifier. It should always be nonzero. */
        packetIdentifier = _nextPacketIdentifier();
        *pPacketIdentifier = packetIdentifier;
        IotMqtt_Assert( packetIdentifier != 0 );

        /* Place the packet identifier into the PUBLISH packet. */
        *pBuffer = _UINT16_HIGH_BYTE( packetIdentifier );
        *( pBuffer + 1 ) = _UINT16_LOW_BYTE( packetIdentifier );
        pBuffer += 2;
    }
    else
    {
        _EMPTY_ELSE_MARKER;
    }

    /* The payload is placed after the packet identifier. */
    if( pPublishInfo->payloadLength > 0 )
    {
        ( void ) memcpy( pBuffer, pPublishInfo->pPayload, pPublishInfo->payloadLength );
        pBuffer += pPublishInfo->payloadLength;
    }
    else
    {
        _EMPTY_ELSE_MARKER;
    }

    /* Ensure that the difference between the end and beginning of the buffer
     * is equal to publishPacketSize, i.e. pBuffer did not overflow. */
    IotMqtt_Assert( ( size_t ) ( pBuffer - *pPublishPacket ) == publishPacketSize );

    /* Print out the serialized PUBLISH packet for debugging purposes. */
    IotLog_PrintBuffer( "MQTT PUBLISH packet:", *pPublishPacket, publishPacketSize );

    _IOT_FUNCTION_EXIT_NO_CLEANUP();
}

/*-----------------------------------------------------------*/

void _IotMqtt_PublishSetDup( bool awsIotMqttMode,
                             uint8_t * pPublishPacket,
                             uint16_t * pNewPacketIdentifier )
{
    const uint8_t * pTopicNameLength = NULL;
    uint8_t * pPacketIdentifier = NULL;
    uint16_t topicNameLength = 0, newPacketIdentifier = _nextPacketIdentifier();

    /* For an AWS IoT MQTT server, change the packet identifier. */
    if( awsIotMqttMode == true )
    {
        /* Decode the "Remaining length" to find where it ends. Because the
         * "Remaining length" was not received from the network, it is "trusted"
         * so the return value of this function isn't checked. */
        ( void ) _decodeRemainingLength( pPublishPacket + 1,
                                         &pTopicNameLength,
                                         NULL );

        /* Decode the topic name length and calculate the address of the packet identifier. */
        topicNameLength = _UINT16_DECODE( pTopicNameLength );
        pPacketIdentifier = ( uint8_t * ) ( pTopicNameLength + topicNameLength + sizeof( uint16_t ) );

        IotLogDebug( "Changing PUBLISH packet identifier %hu to %hu.",
                     _UINT16_DECODE( pPacketIdentifier ),
                     newPacketIdentifier );

        /* Replace the packet identifier. */
        *pPacketIdentifier = _UINT16_HIGH_BYTE( newPacketIdentifier );
        *( pPacketIdentifier + 1 ) = _UINT16_LOW_BYTE( newPacketIdentifier );
        *pNewPacketIdentifier = newPacketIdentifier;
    }
    else
    {
        /* For a compliant MQTT 3.1.1 server, set the DUP flag. */
        _UINT8_SET_BIT( *pPublishPacket, _MQTT_PUBLISH_FLAG_DUP );

        IotLogDebug( "PUBLISH DUP flag set." );
    }
}

/*-----------------------------------------------------------*/

IotMqttError_t _IotMqtt_DeserializePublish( const uint8_t * pPublishStart,
                                            size_t dataLength,
                                            IotMqttPublishInfo_t * pOutput,
                                            uint16_t * pPacketIdentifier,
                                            size_t * pBytesProcessed )
{
    _IOT_FUNCTION_ENTRY( IotMqttError_t, IOT_MQTT_SUCCESS );
    size_t remainingLength = 0, packetSize = 0;
    uint8_t publishFlags = 0;
    uint16_t packetIdentifier = 0;
    const uint8_t * pVariableHeader = NULL, * pPacketIdentifierHigh = NULL;

    /* Ensure that at least 5 bytes are available. If not, this is an incomplete
     * PUBLISH packet. */
    if( dataLength < _MQTT_PACKET_PUBLISH_MINIMUM_SIZE )
    {
        IotLogError( "PUBLISH size %lu is smaller than the smallest possible "
                     "size %d.",
                     ( unsigned long ) dataLength,
                     _MQTT_PACKET_PUBLISH_MINIMUM_SIZE );
        *pBytesProcessed = 0;

        _IOT_SET_AND_GOTO_CLEANUP( IOT_MQTT_BAD_RESPONSE );
    }
    else
    {
        _EMPTY_ELSE_MARKER;
    }

    /* Decode the "Remaining length", which is the second byte in PUBLISH. */
    status = _decodeRemainingLength( pPublishStart + 1, &pVariableHeader, &remainingLength );

    if( status != IOT_MQTT_SUCCESS )
    {
        IotLog( IOT_LOG_DEBUG,
                &_logHideAll,
                "Bad remaining length." );

        /* If the "Remaining length" couldn't be determined, invalidate the rest
         * of the data stream by marking it processed. */
        *pBytesProcessed = dataLength;

        _IOT_SET_AND_GOTO_CLEANUP( IOT_MQTT_BAD_RESPONSE );
    }
    else
    {
        _EMPTY_ELSE_MARKER;
    }

    IotLog( IOT_LOG_DEBUG,
            &_logHideAll,
            "Remaining length %lu.", ( unsigned long ) remainingLength );

    /* Calculate the packet size and set the output parameter. */
    packetSize = remainingLength + _remainingLengthEncodedSize( remainingLength ) + 1;
    *pBytesProcessed = packetSize;

    /* Ensure that the PUBLISH packet fits within the data stream. If it doesn't,
     * then this is a partial packet. */
    if( packetSize > dataLength )
    {
        IotLog( IOT_LOG_DEBUG,
                &_logHideAll,
                "PUBLISH packet size %lu exceeds data length %lu.",
                ( unsigned long ) packetSize,
                ( unsigned long ) dataLength );
        *pBytesProcessed = 0;

        _IOT_SET_AND_GOTO_CLEANUP( IOT_MQTT_BAD_RESPONSE );
    }
    else
    {
        _EMPTY_ELSE_MARKER;
    }

    /* The flags are the lower 4 bits of the first byte in PUBLISH. */
    publishFlags = *pPublishStart;

    /* Parse the Retain bit. */
    pOutput->retain = _UINT8_CHECK_BIT( publishFlags, _MQTT_PUBLISH_FLAG_RETAIN );

    IotLog( IOT_LOG_DEBUG,
            &_logHideAll,
            "Retain bit is %d.", pOutput->retain );

    /* Check for QoS 2. */
    if( _UINT8_CHECK_BIT( publishFlags, _MQTT_PUBLISH_FLAG_QOS2 ) == true )
    {
        /* PUBLISH packet is invalid if both QoS 1 and QoS 2 bits are set. */
        if( _UINT8_CHECK_BIT( publishFlags, _MQTT_PUBLISH_FLAG_QOS1 ) == true )
        {
            IotLog( IOT_LOG_DEBUG,
                    &_logHideAll,
                    "Bad QoS: 3." );

            _IOT_SET_AND_GOTO_CLEANUP( IOT_MQTT_BAD_RESPONSE );
        }
        else
        {
            _EMPTY_ELSE_MARKER;
        }

        pOutput->qos = IOT_MQTT_QOS_2;
    }
    /* Check for QoS 1. */
    else if( _UINT8_CHECK_BIT( publishFlags, _MQTT_PUBLISH_FLAG_QOS1 ) == true )
    {
        pOutput->qos = IOT_MQTT_QOS_1;
    }
    /* If the PUBLISH isn't QoS 1 or 2, then it's QoS 0. */
    else
    {
        pOutput->qos = IOT_MQTT_QOS_0;
    }

    IotLog( IOT_LOG_DEBUG,
            &_logHideAll,
            "QoS is %d.", pOutput->qos );

    /* Parse the DUP bit. */
    if( _UINT8_CHECK_BIT( publishFlags, _MQTT_PUBLISH_FLAG_DUP ) == true )
    {
        IotLog( IOT_LOG_DEBUG,
                &_logHideAll,
                "DUP is 1." );
    }
    else
    {
        IotLog( IOT_LOG_DEBUG,
                &_logHideAll,
                "DUP is 0." );
    }

    /* Sanity checks for "Remaining length". */
    if( pOutput->qos == IOT_MQTT_QOS_0 )
    {
        /* A QoS 0 PUBLISH must have a remaining length of at least 3 to accommodate
         * topic name length (2 bytes) and topic name (at least 1 byte). */
        if( remainingLength < 3 )
        {
            IotLog( IOT_LOG_DEBUG,
                    &_logHideAll,
                    "QoS 0 PUBLISH cannot have a remaining length less than 3." );
            *pBytesProcessed = dataLength;

            _IOT_SET_AND_GOTO_CLEANUP( IOT_MQTT_BAD_RESPONSE );
        }
        else
        {
            _EMPTY_ELSE_MARKER;
        }
    }
    else
    {
        /* A QoS 1 or 2 PUBLISH must have a remaining length of at least 5 to
         * accommodate a packet identifier as well as the topic name length and
         * topic name. */
        if( remainingLength < 5 )
        {
            IotLog( IOT_LOG_DEBUG,
                    &_logHideAll,
                    "QoS 1 or 2 PUBLISH cannot have a remaining length less than 5." );
            *pBytesProcessed = dataLength;

            _IOT_SET_AND_GOTO_CLEANUP( IOT_MQTT_BAD_RESPONSE );
        }
        else
        {
            _EMPTY_ELSE_MARKER;
        }
    }

    /* Extract the topic name starting from the first byte of the variable header.
     * The topic name string starts at byte 3 in the variable header. */
    pOutput->topicNameLength = _UINT16_DECODE( pVariableHeader );

    /* Sanity checks for topic name length and "Remaining length". */
    if( pOutput->qos == IOT_MQTT_QOS_0 )
    {
        /* Check that the "Remaining length" is at least as large as the variable
         * header. */
        if( remainingLength < pOutput->topicNameLength + sizeof( uint16_t ) )
        {
            IotLog( IOT_LOG_DEBUG,
                    &_logHideAll,
                    "Remaining length cannot be less than variable header length." );
            *pBytesProcessed = dataLength;

            _IOT_SET_AND_GOTO_CLEANUP( IOT_MQTT_BAD_RESPONSE );
        }
        else
        {
            _EMPTY_ELSE_MARKER;
        }
    }
    else
    {
        /* Check that the "Remaining length" is at least as large as the variable
         * header. */
        if( remainingLength < pOutput->topicNameLength + 2 * sizeof( uint16_t ) )
        {
            IotLog( IOT_LOG_DEBUG,
                    &_logHideAll,
                    "Remaining length cannot be less than variable header length." );
            *pBytesProcessed = dataLength;

            _IOT_SET_AND_GOTO_CLEANUP( IOT_MQTT_BAD_RESPONSE );
        }
        else
        {
            _EMPTY_ELSE_MARKER;
        }
    }

    /* Parse the topic. */
    pOutput->pTopicName = ( const char * ) ( pVariableHeader + sizeof( uint16_t ) );

    IotLog( IOT_LOG_DEBUG,
            &_logHideAll,
            "Topic name length %hu: %.*s",
            pOutput->topicNameLength,
            pOutput->topicNameLength,
            pOutput->pTopicName );

    /* Extract the packet identifier for QoS 1 or 2 PUBLISH packets. Packet
     * identifier starts immediately after the topic name. */
    pPacketIdentifierHigh = ( const uint8_t * ) ( pOutput->pTopicName + pOutput->topicNameLength );

    if( pOutput->qos > IOT_MQTT_QOS_0 )
    {
        packetIdentifier = _UINT16_DECODE( pPacketIdentifierHigh );
        *pPacketIdentifier = packetIdentifier;

        IotLog( IOT_LOG_DEBUG,
                &_logHideAll,
                "Packet identifier %hu.", packetIdentifier );

        /* Packet identifier cannot be 0. */
        if( packetIdentifier == 0 )
        {
            _IOT_SET_AND_GOTO_CLEANUP( IOT_MQTT_BAD_RESPONSE );
        }
        else
        {
            _EMPTY_ELSE_MARKER;
        }
    }
    else
    {
        _EMPTY_ELSE_MARKER;
    }

    /* Calculate the length of the payload. QoS 1 or 2 PUBLISH packets contain
     * a packet identifer, but QoS 0 PUBLISH packets do not. */
    if( pOutput->qos == IOT_MQTT_QOS_0 )
    {
        pOutput->payloadLength = ( uint16_t ) ( remainingLength - pOutput->topicNameLength - sizeof( uint16_t ) );
        pOutput->pPayload = pPacketIdentifierHigh;
    }
    else
    {
        pOutput->payloadLength = ( uint16_t ) ( remainingLength - pOutput->topicNameLength - 2 * sizeof( uint16_t ) );
        pOutput->pPayload = pPacketIdentifierHigh + sizeof( uint16_t );
    }

    IotLog( IOT_LOG_DEBUG,
            &_logHideAll,
            "Payload length %hu. Payload:", pOutput->payloadLength );
    IotLog_PrintBuffer( NULL, pOutput->pPayload, pOutput->payloadLength );

    _IOT_FUNCTION_EXIT_NO_CLEANUP();
}

/*-----------------------------------------------------------*/

IotMqttError_t _IotMqtt_SerializePuback( uint16_t packetIdentifier,
                                         uint8_t ** pPubackPacket,
                                         size_t * pPacketSize )
{
    IotMqttError_t status = IOT_MQTT_SUCCESS;

    /* Allocate memory for PUBACK. */
    uint8_t * pBuffer = IotMqtt_MallocMessage( _MQTT_PACKET_PUBACK_SIZE );

    if( pBuffer == NULL )
    {
        IotLogError( "Failed to allocate memory for PUBACK packet" );

        status = IOT_MQTT_NO_MEMORY;
    }
    else
    {
        /* Set the output parameters. The remainder of this function always succeeds. */
        *pPubackPacket = pBuffer;
        *pPacketSize = _MQTT_PACKET_PUBACK_SIZE;

        /* Set the 4 bytes in PUBACK. */
        pBuffer[ 0 ] = _MQTT_PACKET_TYPE_PUBACK;
        pBuffer[ 1 ] = _MQTT_PACKET_PUBACK_REMAINING_LENGTH;
        pBuffer[ 2 ] = _UINT16_HIGH_BYTE( packetIdentifier );
        pBuffer[ 3 ] = _UINT16_LOW_BYTE( packetIdentifier );

        /* Print out the serialized PUBACK packet for debugging purposes. */
        IotLog_PrintBuffer( "MQTT PUBACK packet:", *pPubackPacket, _MQTT_PACKET_PUBACK_SIZE );
    }

    return status;
}

/*-----------------------------------------------------------*/

IotMqttError_t _IotMqtt_DeserializePuback( const uint8_t * pPubackStart,
                                           size_t dataLength,
                                           uint16_t * pPacketIdentifier,
                                           size_t * pBytesProcessed )
{
    _IOT_FUNCTION_ENTRY( IotMqttError_t, IOT_MQTT_SUCCESS );
    uint16_t packetIdentifier = 0;

    /* According to MQTT 3.1.1, PUBACK packets are all 4 bytes in size. If the
     * data stream has fewer than 4 bytes, then the PUBACK packet is incomplete. */
    if( dataLength < _MQTT_PACKET_PUBACK_SIZE )
    {
        IotLog( IOT_LOG_ERROR,
                &_logHideAll,
                "PUBACK less than %d bytes in size.",
                _MQTT_PACKET_PUBACK_SIZE );
        *pBytesProcessed = 0;

        _IOT_SET_AND_GOTO_CLEANUP( IOT_MQTT_BAD_RESPONSE );
    }
    else
    {
        _EMPTY_ELSE_MARKER;
    }

    /* The next 4 bytes will be processed by this function. */
    *pBytesProcessed = _MQTT_PACKET_PUBACK_SIZE;

    /* Extract the packet identifier (third and fourth bytes) from PUBACK. */
    packetIdentifier = _UINT16_DECODE( pPubackStart + 2 );
    *pPacketIdentifier = packetIdentifier;

    IotLog( IOT_LOG_DEBUG,
            &_logHideAll,
            "Packet identifier %hu.", packetIdentifier );

    /* Packet identifier cannot be 0. */
    if( packetIdentifier == 0 )
    {
        _IOT_SET_AND_GOTO_CLEANUP( IOT_MQTT_BAD_RESPONSE );
    }
    else
    {
        _EMPTY_ELSE_MARKER;
    }

    /* Check that the control packet type is 0x40. */
    if( pPubackStart[ 0 ] != _MQTT_PACKET_TYPE_PUBACK )
    {
        IotLog( IOT_LOG_ERROR,
                &_logHideAll,
                "Bad control packet type 0x%02x.",
                pPubackStart[ 0 ] );

        _IOT_SET_AND_GOTO_CLEANUP( IOT_MQTT_BAD_RESPONSE );
    }
    else
    {
        _EMPTY_ELSE_MARKER;
    }

    /* Check the "Remaining length" (second byte) of the received PUBACK. */
    if( pPubackStart[ 1 ] != _MQTT_PACKET_PUBACK_REMAINING_LENGTH )
    {
        IotLog( IOT_LOG_ERROR,
                &_logHideAll,
                "PUBACK does not have remaining length of %d.",
                _MQTT_PACKET_PUBACK_REMAINING_LENGTH );

        _IOT_SET_AND_GOTO_CLEANUP( IOT_MQTT_BAD_RESPONSE );
    }
    else
    {
        _EMPTY_ELSE_MARKER;
    }

    _IOT_FUNCTION_EXIT_NO_CLEANUP();
}

/*-----------------------------------------------------------*/

IotMqttError_t _IotMqtt_SerializeSubscribe( const IotMqttSubscription_t * pSubscriptionList,
                                            size_t subscriptionCount,
                                            uint8_t ** pSubscribePacket,
                                            size_t * pPacketSize,
                                            uint16_t * pPacketIdentifier )
{
    _IOT_FUNCTION_ENTRY( IotMqttError_t, IOT_MQTT_SUCCESS );
    size_t i = 0, subscribePacketSize = 0, remainingLength = 0;
    uint16_t packetIdentifier = 0;
    uint8_t * pBuffer = NULL;

    /* Calculate the "Remaining length" field and total packet size. If it exceeds
     * what is allowed in the MQTT standard, return an error. */
    if( _subscriptionPacketSize( IOT_MQTT_SUBSCRIBE,
                                 pSubscriptionList,
                                 subscriptionCount,
                                 &remainingLength,
                                 &subscribePacketSize ) == false )
    {
        IotLogError( "Subscribe packet remaining length exceeds %lu, which is the "
                     "maximum size allowed by MQTT 3.1.1.",
                     _MQTT_MAX_REMAINING_LENGTH );

        _IOT_SET_AND_GOTO_CLEANUP( IOT_MQTT_BAD_PARAMETER );
    }
    else
    {
        _EMPTY_ELSE_MARKER;
    }

    /* Total size of the subscribe packet should be larger than the "Remaining length"
     * field. */
    IotMqtt_Assert( subscribePacketSize > remainingLength );

    /* Allocate memory to hold the SUBSCRIBE packet. */
    pBuffer = IotMqtt_MallocMessage( subscribePacketSize );

    /* Check that sufficient memory was allocated. */
    if( pBuffer == NULL )
    {
        IotLogError( "Failed to allocate memory for SUBSCRIBE packet." );

        _IOT_SET_AND_GOTO_CLEANUP( IOT_MQTT_NO_MEMORY );
    }
    else
    {
        _EMPTY_ELSE_MARKER;
    }

    /* Set the output parameters. The remainder of this function always succeeds. */
    *pSubscribePacket = pBuffer;
    *pPacketSize = subscribePacketSize;

    /* The first byte in SUBSCRIBE is the packet type. */
    *pBuffer = _MQTT_PACKET_TYPE_SUBSCRIBE;
    pBuffer++;

    /* Encode the "Remaining length" starting from the second byte. */
    pBuffer = _encodeRemainingLength( pBuffer, remainingLength );

    /* Get the next packet identifier. It should always be nonzero. */
    packetIdentifier = _nextPacketIdentifier();
    *pPacketIdentifier = packetIdentifier;
    IotMqtt_Assert( packetIdentifier != 0 );

    /* Place the packet identifier into the SUBSCRIBE packet. */
    *pBuffer = _UINT16_HIGH_BYTE( packetIdentifier );
    *( pBuffer + 1 ) = _UINT16_LOW_BYTE( packetIdentifier );
    pBuffer += 2;

    /* Serialize each subscription topic filter and QoS. */
    for( i = 0; i < subscriptionCount; i++ )
    {
        pBuffer = _encodeString( pBuffer,
                                 pSubscriptionList[ i ].pTopicFilter,
                                 pSubscriptionList[ i ].topicFilterLength );

        /* Place the QoS in the SUBSCRIBE packet. */
        *pBuffer = ( uint8_t ) ( pSubscriptionList[ i ].qos );
        pBuffer++;
    }

    /* Ensure that the difference between the end and beginning of the buffer
     * is equal to subscribePacketSize, i.e. pBuffer did not overflow. */
    IotMqtt_Assert( ( size_t ) ( pBuffer - *pSubscribePacket ) == subscribePacketSize );

    /* Print out the serialized SUBSCRIBE packet for debugging purposes. */
    IotLog_PrintBuffer( "MQTT SUBSCRIBE packet:", *pSubscribePacket, subscribePacketSize );

    _IOT_FUNCTION_EXIT_NO_CLEANUP();
}

/*-----------------------------------------------------------*/

IotMqttError_t _IotMqtt_DeserializeSuback( IotMqttConnection_t mqttConnection,
                                           const uint8_t * pSubackStart,
                                           size_t dataLength,
                                           uint16_t * pPacketIdentifier,
                                           size_t * pBytesProcessed )
{
    _IOT_FUNCTION_ENTRY( IotMqttError_t, IOT_MQTT_SUCCESS );
    size_t i = 0, remainingLength = 0, packetSize = 0;
    uint16_t packetIdentifier = 0;
    uint8_t subscriptionStatus = 0;
    const uint8_t * pVariableHeader = NULL;
    _mqttConnection_t * pMqttConnection = ( _mqttConnection_t * ) mqttConnection;

    /* Ensure that at least 5 bytes are available. If not, this is an incomplete
     * SUBACK packet. */
    if( dataLength < _MQTT_PACKET_SUBACK_MINIMUM_SIZE )
    {
        IotLogError( "SUBACK size %lu is smaller than the smallest possible "
                     "size %d.",
                     ( unsigned long ) dataLength,
                     _MQTT_PACKET_SUBACK_MINIMUM_SIZE );
        *pBytesProcessed = 0;

        _IOT_SET_AND_GOTO_CLEANUP( IOT_MQTT_BAD_RESPONSE );
    }
    else
    {
        _EMPTY_ELSE_MARKER;
    }

    /* Decode the "Remaining length" in SUBACK, which starts at byte 2. */
    if( _decodeRemainingLength( pSubackStart + 1,
                                &pVariableHeader,
                                &remainingLength ) != IOT_MQTT_SUCCESS )
    {
        IotLog( IOT_LOG_DEBUG,
                &_logHideAll,
                "Bad remaining length." );

        /* If the "Remaining length" couldn't be determined, invalidate the rest
         * of the data stream by marking it processed. */
        *pBytesProcessed = dataLength;

        _IOT_SET_AND_GOTO_CLEANUP( IOT_MQTT_BAD_RESPONSE );
    }
    else
    {
        _EMPTY_ELSE_MARKER;
    }

    IotLog( IOT_LOG_DEBUG,
            &_logHideAll,
            "Remaining length %lu.", ( unsigned long ) remainingLength );

    /* A SUBACK must have a remaining length of at least 3 to accommodate the
     * packet identifer and at least 1 return code. */
    if( remainingLength < 3 )
    {
        IotLog( IOT_LOG_DEBUG,
                &_logHideAll,
                "SUBACK cannot have a remaining length less than 3." );
        *pBytesProcessed = dataLength;

        _IOT_SET_AND_GOTO_CLEANUP( IOT_MQTT_BAD_RESPONSE );
    }
    else
    {
        _EMPTY_ELSE_MARKER;
    }

    /* Calculate the packet size and set the output parameter. */
    packetSize = remainingLength + _remainingLengthEncodedSize( remainingLength ) + 1;
    *pBytesProcessed = packetSize;

    /* Ensure that the SUBACK packet fits within the data stream. If it doesn't,
     * then this is a partial packet. */
    if( packetSize > dataLength )
    {
        IotLog( IOT_LOG_DEBUG,
                &_logHideAll,
                "SUBACK packet size %lu exceeds data length %lu.",
                ( unsigned long ) packetSize,
                ( unsigned long ) dataLength );
        *pBytesProcessed = 0;

        _IOT_SET_AND_GOTO_CLEANUP( IOT_MQTT_BAD_RESPONSE );
    }
    else
    {
        _EMPTY_ELSE_MARKER;
    }

    /* Extract the packet identifier (first 2 bytes of variable header) from SUBACK. */
    packetIdentifier = _UINT16_DECODE( pVariableHeader );
    *pPacketIdentifier = packetIdentifier;

    IotLog( IOT_LOG_DEBUG,
            &_logHideAll,
            "Packet identifier %hu.", packetIdentifier );

    /* Check that the control packet type is 0x90. */
    if( pSubackStart[ 0 ] != _MQTT_PACKET_TYPE_SUBACK )
    {
        IotLog( IOT_LOG_ERROR,
                &_logHideAll,
                "Bad control packet type 0x%02x.",
                pSubackStart[ 0 ] );

        _IOT_SET_AND_GOTO_CLEANUP( IOT_MQTT_BAD_RESPONSE );
    }
    else
    {
        _EMPTY_ELSE_MARKER;
    }

    /* Iterate through each status byte in the SUBACK packet. */
    for( i = 0; i < remainingLength - sizeof( uint16_t ); i++ )
    {
        /* Read a single status byte in SUBACK. */
        subscriptionStatus = *( pVariableHeader + sizeof( uint16_t ) + i );

        /* MQTT 3.1.1 defines the following values as status codes. */
        switch( subscriptionStatus )
        {
            case 0x00:
            case 0x01:
            case 0x02:
                IotLog( IOT_LOG_DEBUG,
                        &_logHideAll,
                        "Topic filter %lu accepted, max QoS %hhu.",
                        ( unsigned long ) i, subscriptionStatus );
                break;

            case 0x80:
                IotLog( IOT_LOG_DEBUG,
                        &_logHideAll,
                        "Topic filter %lu refused.", ( unsigned long ) i );

                /* Remove a rejected subscription from the subscription manager. */
                _IotMqtt_RemoveSubscriptionByPacket( pMqttConnection,
                                                     packetIdentifier,
                                                     ( long ) i );

                status = IOT_MQTT_SERVER_REFUSED;

                break;

            default:
                IotLog( IOT_LOG_DEBUG,
                        &_logHideAll,
                        "Bad SUBSCRIBE status %hhu.", subscriptionStatus );

                status = IOT_MQTT_BAD_RESPONSE;

                break;
        }

        /* Stop parsing the subscription statuses if a bad response was received. */
        if( status == IOT_MQTT_BAD_RESPONSE )
        {
            break;
        }
        else
        {
            _EMPTY_ELSE_MARKER;
        }
    }

    _IOT_FUNCTION_EXIT_NO_CLEANUP();
}

/*-----------------------------------------------------------*/

IotMqttError_t _IotMqtt_SerializeUnsubscribe( const IotMqttSubscription_t * pSubscriptionList,
                                              size_t subscriptionCount,
                                              uint8_t ** pUnsubscribePacket,
                                              size_t * pPacketSize,
                                              uint16_t * pPacketIdentifier )
{
    _IOT_FUNCTION_ENTRY( IotMqttError_t, IOT_MQTT_SUCCESS );
    size_t i = 0, unsubscribePacketSize = 0, remainingLength = 0;
    uint16_t packetIdentifier = 0;
    uint8_t * pBuffer = NULL;

    /* Calculate the "Remaining length" field and total packet size. If it exceeds
     * what is allowed in the MQTT standard, return an error. */
    if( _subscriptionPacketSize( IOT_MQTT_UNSUBSCRIBE,
                                 pSubscriptionList,
                                 subscriptionCount,
                                 &remainingLength,
                                 &unsubscribePacketSize ) == false )
    {
        IotLogError( "Unsubscribe packet remaining length exceeds %lu, which is the "
                     "maximum size allowed by MQTT 3.1.1.",
                     _MQTT_MAX_REMAINING_LENGTH );

        _IOT_SET_AND_GOTO_CLEANUP( IOT_MQTT_BAD_PARAMETER );
    }
    else
    {
        _EMPTY_ELSE_MARKER;
    }

    /* Total size of the unsubscribe packet should be larger than the "Remaining length"
     * field. */
    IotMqtt_Assert( unsubscribePacketSize > remainingLength );

    /* Allocate memory to hold the UNSUBSCRIBE packet. */
    pBuffer = IotMqtt_MallocMessage( unsubscribePacketSize );

    /* Check that sufficient memory was allocated. */
    if( pBuffer == NULL )
    {
        IotLogError( "Failed to allocate memory for UNSUBSCRIBE packet." );

        _IOT_SET_AND_GOTO_CLEANUP( IOT_MQTT_NO_MEMORY );
    }
    else
    {
        _EMPTY_ELSE_MARKER;
    }

    /* Set the output parameters. The remainder of this function always succeeds. */
    *pUnsubscribePacket = pBuffer;
    *pPacketSize = unsubscribePacketSize;

    /* The first byte in UNSUBSCRIBE is the packet type. */
    *pBuffer = _MQTT_PACKET_TYPE_UNSUBSCRIBE;
    pBuffer++;

    /* Encode the "Remaining length" starting from the second byte. */
    pBuffer = _encodeRemainingLength( pBuffer, remainingLength );

    /* Get the next packet identifier. It should always be nonzero. */
    packetIdentifier = _nextPacketIdentifier();
    *pPacketIdentifier = packetIdentifier;
    IotMqtt_Assert( packetIdentifier != 0 );

    /* Place the packet identifier into the UNSUBSCRIBE packet. */
    *pBuffer = _UINT16_HIGH_BYTE( packetIdentifier );
    *( pBuffer + 1 ) = _UINT16_LOW_BYTE( packetIdentifier );
    pBuffer += 2;

    /* Serialize each subscription topic filter. */
    for( i = 0; i < subscriptionCount; i++ )
    {
        pBuffer = _encodeString( pBuffer,
                                 pSubscriptionList[ i ].pTopicFilter,
                                 pSubscriptionList[ i ].topicFilterLength );
    }

    /* Ensure that the difference between the end and beginning of the buffer
     * is equal to unsubscribePacketSize, i.e. pBuffer did not overflow. */
    IotMqtt_Assert( ( size_t ) ( pBuffer - *pUnsubscribePacket ) == unsubscribePacketSize );

    /* Print out the serialized UNSUBSCRIBE packet for debugging purposes. */
    IotLog_PrintBuffer( "MQTT UNSUBSCRIBE packet:", *pUnsubscribePacket, unsubscribePacketSize );

    _IOT_FUNCTION_EXIT_NO_CLEANUP();
}

/*-----------------------------------------------------------*/

IotMqttError_t _IotMqtt_DeserializeUnsuback( const uint8_t * pUnsubackStart,
                                             size_t dataLength,
                                             uint16_t * pPacketIdentifier,
                                             size_t * pBytesProcessed )
{
    _IOT_FUNCTION_ENTRY( IotMqttError_t, IOT_MQTT_SUCCESS );
    uint16_t packetIdentifier = 0;

    /* According to MQTT 3.1.1, UNSUBACK packets are all 4 bytes in size. If the
     * data stream has fewer than 4 bytes, then the UNSUBACK packet is incomplete. */
    if( dataLength < _MQTT_PACKET_UNSUBACK_SIZE )
    {
        IotLog( IOT_LOG_ERROR,
                &_logHideAll,
                "UNSUBACK less than %d bytes in size.",
                _MQTT_PACKET_UNSUBACK_SIZE );
        *pBytesProcessed = 0;

        _IOT_SET_AND_GOTO_CLEANUP( IOT_MQTT_BAD_RESPONSE );
    }
    else
    {
        _EMPTY_ELSE_MARKER;
    }

    /* The next 4 bytes will be processed by this function. */
    *pBytesProcessed = _MQTT_PACKET_UNSUBACK_SIZE;

    /* Extract the packet identifier (third and fourth bytes) from UNSUBACK. */
    packetIdentifier = _UINT16_DECODE( pUnsubackStart + 2 );
    *pPacketIdentifier = packetIdentifier;

    /* Packet identifier cannot be 0. */
    if( packetIdentifier == 0 )
    {
        _IOT_SET_AND_GOTO_CLEANUP( IOT_MQTT_BAD_RESPONSE );
    }
    else
    {
        _EMPTY_ELSE_MARKER;
    }

    /* Check that the control packet type is 0xb0. */
    if( pUnsubackStart[ 0 ] != _MQTT_PACKET_TYPE_UNSUBACK )
    {
        IotLog( IOT_LOG_ERROR,
                &_logHideAll,
                "Bad control packet type 0x%02x.",
                pUnsubackStart[ 0 ] );

        _IOT_SET_AND_GOTO_CLEANUP( IOT_MQTT_BAD_RESPONSE );
    }
    else
    {
        _EMPTY_ELSE_MARKER;
    }

    /* Check the "Remaining length" (second byte) of the received UNSUBACK. */
    if( pUnsubackStart[ 1 ] != _MQTT_PACKET_UNSUBACK_REMAINING_LENGTH )
    {
        IotLog( IOT_LOG_ERROR,
                &_logHideAll,
                "UNSUBACK does not have remaining length of %d.",
                _MQTT_PACKET_UNSUBACK_REMAINING_LENGTH );

        _IOT_SET_AND_GOTO_CLEANUP( IOT_MQTT_BAD_RESPONSE );
    }
    else
    {
        _EMPTY_ELSE_MARKER;
    }

    IotLog( IOT_LOG_DEBUG,
            &_logHideAll,
            "Packet identifier %hu.", packetIdentifier );

    _IOT_FUNCTION_EXIT_NO_CLEANUP();
}

/*-----------------------------------------------------------*/

IotMqttError_t _IotMqtt_SerializePingreq( uint8_t ** pPingreqPacket,
                                          size_t * pPacketSize )
{
    /* PINGREQ packets are always the same. */
    static const uint8_t pPingreq[ _MQTT_PACKET_PINGREQ_SIZE ] =
    {
        _MQTT_PACKET_TYPE_PINGREQ,
        0x00
    };

    /* Set the output parameters. */
    *pPingreqPacket = ( uint8_t * ) pPingreq;
    *pPacketSize = _MQTT_PACKET_PINGREQ_SIZE;

    /* Print out the PINGREQ packet for debugging purposes. */
    IotLog_PrintBuffer( "MQTT PINGREQ packet:", pPingreq, _MQTT_PACKET_PINGREQ_SIZE );

    return IOT_MQTT_SUCCESS;
}

/*-----------------------------------------------------------*/

IotMqttError_t _IotMqtt_DeserializePingresp( const uint8_t * pPingrespStart,
                                             size_t dataLength,
                                             size_t * pBytesProcessed )
{
    _IOT_FUNCTION_ENTRY( IotMqttError_t, IOT_MQTT_SUCCESS );

    /* According to MQTT 3.1.1, PINGRESP packets are all 2 bytes in size. If the
     * data stream has fewer than 2 bytes, then the PINGRESP packet is incomplete. */
    if( dataLength < _MQTT_PACKET_PINGRESP_SIZE )
    {
        IotLog( IOT_LOG_ERROR,
                &_logHideAll,
                "PINGRESP less than %d bytes in size.",
                _MQTT_PACKET_PINGRESP_SIZE );
        *pBytesProcessed = 0;

        _IOT_SET_AND_GOTO_CLEANUP( IOT_MQTT_BAD_RESPONSE );
    }
    else
    {
        _EMPTY_ELSE_MARKER;
    }

    /* The next 2 bytes will be processed by this function. */
    *pBytesProcessed = _MQTT_PACKET_PINGRESP_SIZE;

    /* Check that the control packet type is 0xd0. */
    if( pPingrespStart[ 0 ] != _MQTT_PACKET_TYPE_PINGRESP )
    {
        IotLog( IOT_LOG_ERROR,
                &_logHideAll,
                "Bad control packet type 0x%02x.",
                pPingrespStart[ 0 ] );

        _IOT_SET_AND_GOTO_CLEANUP( IOT_MQTT_BAD_RESPONSE );
    }
    else
    {
        _EMPTY_ELSE_MARKER;
    }

    /* Check the "Remaining length" (second byte) of the received PINGRESP. */
    if( pPingrespStart[ 1 ] != _MQTT_PACKET_PINGRESP_REMAINING_LENGTH )
    {
        IotLog( IOT_LOG_ERROR,
                &_logHideAll,
                "PINGRESP does not have remaining length of %d.",
                _MQTT_PACKET_PINGRESP_REMAINING_LENGTH );

        _IOT_SET_AND_GOTO_CLEANUP( IOT_MQTT_BAD_RESPONSE );
    }
    else
    {
        _EMPTY_ELSE_MARKER;
    }

    _IOT_FUNCTION_EXIT_NO_CLEANUP();
}

/*-----------------------------------------------------------*/

IotMqttError_t _IotMqtt_SerializeDisconnect( uint8_t ** pDisconnectPacket,
                                             size_t * pPacketSize )
{
    /* DISCONNECT packets are always the same. */
    static const uint8_t pDisconnect[ _MQTT_PACKET_DISCONNECT_SIZE ] =
    {
        _MQTT_PACKET_TYPE_DISCONNECT,
        0x00
    };

    /* Set the output parameters. */
    *pDisconnectPacket = ( uint8_t * ) pDisconnect;
    *pPacketSize = _MQTT_PACKET_DISCONNECT_SIZE;

    /* Print out the DISCONNECT packet for debugging purposes. */
    IotLog_PrintBuffer( "MQTT DISCONNECT packet:", pDisconnect, _MQTT_PACKET_DISCONNECT_SIZE );

    return IOT_MQTT_SUCCESS;
}

/*-----------------------------------------------------------*/

void _IotMqtt_FreePacket( uint8_t * pPacket )
{
    uint8_t packetType = *pPacket;

    /* Don't call free on DISCONNECT and PINGREQ; those are allocated from static
     * memory. */
    if( packetType != _MQTT_PACKET_TYPE_DISCONNECT )
    {
        if( packetType != _MQTT_PACKET_TYPE_PINGREQ )
        {
            IotMqtt_FreeMessage( pPacket );
        }
        else
        {
            _EMPTY_ELSE_MARKER;
        }
    }
    else
    {
        _EMPTY_ELSE_MARKER;
    }

}

/*-----------------------------------------------------------*/

/* If the MQTT library is being tested, include a file that allows access to
 * internal functions and variables. */
#if IOT_MQTT_TEST == 1
    #include "iot_test_access_mqtt_serialize.c"
#endif
