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
 * http://aws.amazon.com/freertos
 * http://www.FreeRTOS.org
 */

/**
 * @file iot_mqtt_serialize.c
 * @brief Implements functions that generate and decode MQTT network packets.
 */

/* The config header is always included first. */
#include "iot_config.h"

/* Standard includes. */
#include <string.h>

/* Error handling include. */
#include "private/iot_error.h"

/* MQTT internal includes. */
#include "private/iot_mqtt_internal.h"

/* Platform layer includes. */
#include "platform/iot_threads.h"

/* Atomic operations. */
#include "iot_atomic.h"

/*-----------------------------------------------------------*/

/*
 * Macros for reading the high and low byte of a 2-byte unsigned int.
 */
#define UINT16_HIGH_BYTE( x )    ( ( uint8_t ) ( x >> 8 ) )            /**< @brief Get high byte. */
#define UINT16_LOW_BYTE( x )     ( ( uint8_t ) ( x & 0x00ff ) )        /**< @brief Get low byte. */

/**
 * @brief Macro for decoding a 2-byte unsigned int from a sequence of bytes.
 *
 * @param[in] ptr A uint8_t* that points to the high byte.
 */
#define UINT16_DECODE( ptr )                                \
    ( uint16_t ) ( ( ( ( uint16_t ) ( *( ptr ) ) ) << 8 ) | \
                   ( ( uint16_t ) ( *( ptr + 1 ) ) ) )

/**
 * @brief Macro for setting a bit in a 1-byte unsigned int.
 *
 * @param[in] x The unsigned int to set.
 * @param[in] position Which bit to set.
 */
#define UINT8_SET_BIT( x, position )      ( x = ( uint8_t ) ( x | ( 0x01 << position ) ) )

/**
 * @brief Macro for checking if a bit is set in a 1-byte unsigned int.
 *
 * @param[in] x The unsigned int to check.
 * @param[in] position Which bit to check.
 */
#define UINT8_CHECK_BIT( x, position )    ( ( x & ( 0x01 << position ) ) == ( 0x01 << position ) )

/*
 * Positions of each flag in the first byte of an MQTT PUBLISH packet's
 * fixed header.
 */
#define MQTT_PUBLISH_FLAG_DUP    ( 3 )                     /**< @brief Duplicate message. */

/*-----------------------------------------------------------*/

/**
 * @brief Generate and return a 2-byte packet identifier.
 *
 * This packet identifier will be nonzero.
 *
 * @return The packet identifier.
 */
static uint16_t _nextPacketIdentifier( void );

/*-----------------------------------------------------------*/

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

void _IotMqtt_PublishSetDup( uint8_t * pPublishPacket,
                             uint8_t * pPacketIdentifierHigh,
                             uint16_t * pNewPacketIdentifier )
{
    uint16_t newPacketIdentifier = 0;

    /* For an AWS IoT MQTT server, change the packet identifier. */
    if( pPacketIdentifierHigh != NULL )
    {
        /* Output parameter for new packet identifier must be provided. */
        IotMqtt_Assert( pNewPacketIdentifier != NULL );

        /* Generate a new packet identifier. */
        newPacketIdentifier = _nextPacketIdentifier();

        IotLogDebug( "Changing PUBLISH packet identifier %hu to %hu.",
                     UINT16_DECODE( pPacketIdentifierHigh ),
                     newPacketIdentifier );

        /* Replace the packet identifier. */
        *pPacketIdentifierHigh = UINT16_HIGH_BYTE( newPacketIdentifier );
        *( pPacketIdentifierHigh + 1 ) = UINT16_LOW_BYTE( newPacketIdentifier );
        *pNewPacketIdentifier = newPacketIdentifier;
    }
    else
    {
        /* For a compliant MQTT 3.1.1 server, set the DUP flag. */
        UINT8_SET_BIT( *pPublishPacket, MQTT_PUBLISH_FLAG_DUP );

        IotLogDebug( "PUBLISH DUP flag set." );
    }
}

/*-----------------------------------------------------------*/
