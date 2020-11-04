/*
 * FreeRTOS Common V1.2.0
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
 * @file iot_mqtt_metrics.c
 * @brief Source code for generating device metrics for AWS IOT.
 * The generated metrics will be included within the username field of MQTT CONNECT message.
 */
#include <string.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <stdio.h>

#include "iot_config.h"

#include "platform/iot_clock.h"
#include "iot_atomic.h"

/*
 * @brief Total length of the hash in bytes.
 */
#define MD5_HASH_LENGTH_BYTES            ( 16 )

/*
 * Encode Length of one byte for the identifier.
 */
#define IDENTIFIER_BYTE_ENCODE_LENGTH    ( 2 )

/*
 * @brief Length in bytes of each chunk used for MD5 hash computation.
 */
#define MD5_CHUNK_LENGTH                 ( 64 )

/*
 * @brief Number of bytes used for padding message length to input bytes.
 */
#define MD5_MSGLEN_PADDING_LENGTH        ( 8 )

/*
 * @brief The first byte of the bit padding ( Most significant bit should be 1 ).
 */
#define MD5_BIT_PADDING_FIRST_BYTE       ( 0x80 )

/*
 * @brief Total length of the input after padding.
 */
#define MD5_PADDED_LENGTH( length )    ( ( length + MD5_MSGLEN_PADDING_LENGTH + MD5_CHUNK_LENGTH ) & ~( MD5_CHUNK_LENGTH - 1 ) )

/*
 * @brief Length of device identifier.
 * Device identifier is represented as hex string of MD5 hash of the device certificate.
 */
#define AWS_IOT_DEVICE_IDENTIFIER_LENGTH    ( MD5_HASH_LENGTH_BYTES * 2 )

/**
 * @brief Device metrics name format
 */
#define AWS_IOT_METRICS_FORMAT              "?SDK=" IOT_SDK_NAME "&Version=" IOT_SDK_VERSION "&Platform=" IOT_PLATFORM_NAME "&AFRDevID=%.*s"

/**
 * @brief Length of #AWS_IOT_METRICS_FORMAT.
 */
#define AWS_IOT_METRICS_FORMAT_LENGTH       ( ( uint16_t ) ( sizeof( AWS_IOT_METRICS_FORMAT ) + AWS_IOT_DEVICE_IDENTIFIER_LENGTH ) )

/*
 * @brief Macro for left rotatation at each round of MD5 hash operation.
 */
#define LEFT_ROTATE( x, c )    ( ( x << c ) | ( x >> ( 32 - c ) ) )


/*
 * @brief Generates 128-bit MD5 hash from the input data.
 * The hash algorithm is adapted from wikipedia and does not dependent on any third party libraries.
 *
 * @param[in] pData Data on which hash needs to be calculated.
 * @param[in] dataLength Length in bytes of the input data
 * @param[in] pHash   Memory location where the hash value is stored.
 * @param[out] hashLength Total size of the memory location for hash storage.
 */
void Utils_generateMD5Hash( const char * pData,
                            size_t dataLength,
                            uint8_t * pHash,
                            size_t hashLength );

/*
 * @brief Generates a unique identifier string from the input data.
 * Uses MD5 hash algorithm to generate a 128-bit unique identifier and
 * encodes as a hexadecimal string.
 */

static void _generateUniqueIdentifier( const char * pInput,
                                       size_t length,
                                       char * pIdentifier,
                                       size_t identifierLength );

/*
 * @brief generates device metrics from the device identifier
 */
static void _generateDeviceMetrics( const char * pDeviceIdentifier,
                                    size_t deviceIdentifierLength,
                                    char * pDeviceMetrics,
                                    size_t deviceMetricsLength );

/*
 * @brief Spinlock lock implementation using atomic compare and swap.
 */
static void _atomicSpinlock_lock( uint32_t * lock );

/*
 * @brief Spinlock unlock implementation using atomic compare and swap.
 */
static void _atomicSpinlock_unlock( uint32_t * lock );

/**
 * @brief Metrics for the device.
 */
static char deviceMetrics[ AWS_IOT_METRICS_FORMAT_LENGTH + 1 ] = { 0 };

/**
 * @brief Unique identifier for the device.
 */
static char deviceIdentifier[ AWS_IOT_DEVICE_IDENTIFIER_LENGTH + 1 ] = { 0 };

/*
 * @brief Constants used for left rotation.
 */
static const unsigned char S[ 64 ] =
{
    7, 12, 17, 22, 7, 12, 17, 22, 7, 12, 17, 22, 7, 12, 17, 22,
    5, 9,  14, 20, 5, 9,  14, 20, 5, 9,  14, 20, 5, 9,  14, 20,
    4, 11, 16, 23, 4, 11, 16, 23, 4, 11, 16, 23, 4, 11, 16, 23,
    6, 10, 15, 21, 6, 10, 15, 21, 6, 10, 15, 21, 6, 10, 15, 21
};

/*
 * @brief Constants representing the binary integer part of the sines of integers from 0 to 63.
 */
static const uint32_t K[ 64 ] =
{
    0xd76aa478, 0xe8c7b756, 0x242070db, 0xc1bdceee,
    0xf57c0faf, 0x4787c62a, 0xa8304613, 0xfd469501,
    0x698098d8, 0x8b44f7af, 0xffff5bb1, 0x895cd7be,
    0x6b901122, 0xfd987193, 0xa679438e, 0x49b40821,
    0xf61e2562, 0xc040b340, 0x265e5a51, 0xe9b6c7aa,
    0xd62f105d, 0x02441453, 0xd8a1e681, 0xe7d3fbc8,
    0x21e1cde6, 0xc33707d6, 0xf4d50d87, 0x455a14ed,
    0xa9e3e905, 0xfcefa3f8, 0x676f02d9, 0x8d2a4c8a,
    0xfffa3942, 0x8771f681, 0x6d9d6122, 0xfde5380c,
    0xa4beea44, 0x4bdecfa9, 0xf6bb4b60, 0xbebfbc70,
    0x289b7ec6, 0xeaa127fa, 0xd4ef3085, 0x04881d05,
    0xd9d4d039, 0xe6db99e5, 0x1fa27cf8, 0xc4ac5665,
    0xf4292244, 0x432aff97, 0xab9423a7, 0xfc93a039,
    0x655b59c3, 0x8f0ccc92, 0xffeff47d, 0x85845dd1,
    0x6fa87e4f, 0xfe2ce6e0, 0xa3014314, 0x4e0811a1,
    0xf7537e82, 0xbd3af235, 0x2ad7d2bb, 0xeb86d391
};


void Utils_generateMD5Hash( const char * pData,
                            size_t dataLength,
                            uint8_t * pHash,
                            size_t hashLength )
{
    uint8_t chunk[ MD5_CHUNK_LENGTH ] = { 0 };
    uint32_t * pOutput = ( uint32_t * ) pHash;
    const uint32_t * pCurrent = NULL;
    size_t paddingOffset, bytesConsumed = 0;
    size_t paddingLength, totalLength = MD5_PADDED_LENGTH( dataLength );
    uint64_t numBits = ( dataLength * 8 );
    bool isPadding = false;
    int i;

    uint32_t A, B, C, D, F, G;

    /*Initialize variables */
    pOutput[ 0 ] = 0x67452301;
    pOutput[ 1 ] = 0xefcdab89;
    pOutput[ 2 ] = 0x98badcfe;
    pOutput[ 3 ] = 0x10325476;

    configASSERT( hashLength >= MD5_HASH_LENGTH_BYTES );

    while( bytesConsumed < totalLength )
    {
        if( ( bytesConsumed + MD5_CHUNK_LENGTH ) <= dataLength )
        {
            pCurrent = ( uint32_t * ) pData;
            pData += MD5_CHUNK_LENGTH;
            bytesConsumed += MD5_CHUNK_LENGTH;
        }
        else
        {
            if( bytesConsumed < dataLength )
            {
                memcpy( chunk, pData, ( dataLength - bytesConsumed ) );
                paddingOffset = ( dataLength - bytesConsumed );
            }
            else
            {
                paddingOffset = 0;
            }

            paddingLength = ( MD5_CHUNK_LENGTH - paddingOffset );

            if( ( paddingLength >= 1 ) && ( isPadding == false ) )
            {
                chunk[ paddingOffset++ ] = MD5_BIT_PADDING_FIRST_BYTE;
                paddingLength--;
                isPadding = true;
            }

            if( paddingLength > MD5_MSGLEN_PADDING_LENGTH )
            {
                memset( &chunk[ paddingOffset ], 0x00, ( paddingLength - MD5_MSGLEN_PADDING_LENGTH ) );
                paddingOffset += ( paddingLength - MD5_MSGLEN_PADDING_LENGTH );
                paddingLength = MD5_MSGLEN_PADDING_LENGTH;
            }

            if( paddingLength == MD5_MSGLEN_PADDING_LENGTH )
            {
                chunk[ paddingOffset++ ] = ( numBits & 0xFF );
                chunk[ paddingOffset++ ] = ( ( numBits >> 8 ) & 0xFF );
                chunk[ paddingOffset++ ] = ( ( numBits >> 16 ) & 0xFF );
                chunk[ paddingOffset++ ] = ( ( numBits >> 24 ) & 0xFF );
                chunk[ paddingOffset++ ] = ( ( numBits >> 32 ) & 0xFF );
                chunk[ paddingOffset++ ] = ( ( numBits >> 40 ) & 0xFF );
                chunk[ paddingOffset++ ] = ( ( numBits >> 48 ) & 0xFF );
                chunk[ paddingOffset++ ] = ( ( numBits >> 56 ) & 0xFF );
            }

            pCurrent = ( uint32_t * ) chunk;
            bytesConsumed += MD5_CHUNK_LENGTH;
        }

        A = pOutput[ 0 ];
        B = pOutput[ 1 ];
        C = pOutput[ 2 ];
        D = pOutput[ 3 ];

        for( i = 0; i < 64; i++ )
        {
            if( i < 16 )
            {
                F = ( B & C ) | ( ( ~B ) & D );
                G = i;
            }
            else if( i < 32 )
            {
                F = ( D & B ) | ( ( ~D ) & C );
                G = ( ( 5 * i ) + 1 ) % 16;
            }
            else if( i < 48 )
            {
                F = ( B ^ C ) ^ D;
                G = ( ( 3 * i ) + 5 ) % 16;
            }
            else
            {
                F = C ^ ( B | ( ~D ) );
                G = ( 7 * i ) % 16;
            }

            F = F + A + K[ i ] + pCurrent[ G ];
            A = D;
            D = C;
            C = B;
            B = B + LEFT_ROTATE( F, S[ i ] );
        }

        pOutput[ 0 ] += A;
        pOutput[ 1 ] += B;
        pOutput[ 2 ] += C;
        pOutput[ 3 ] += D;
    }
}

static void _generateDeviceMetrics( const char * pDeviceIdentifier,
                                    size_t deviceIdentifierLength,
                                    char * pDeviceMetrics,
                                    size_t deviceMetricsLength )
{
    size_t length;

    length = snprintf( pDeviceMetrics,
                       deviceMetricsLength,
                       AWS_IOT_METRICS_FORMAT,
                       deviceIdentifierLength,
                       pDeviceIdentifier );

    configASSERT( length > 0 );
}

static void _atomicSpinlock_lock( uint32_t * lock )
{
    uint32_t current = 0;
    uint32_t store = 1;

    for( ; ; )
    {
        if( Atomic_CompareAndSwap_u32( lock, store, current ) == 1 )
        {
            break;
        }

        /* Does context switch with negligible delay. */
        IotClock_SleepMs( 1 );
    }
}

static void _atomicSpinlock_unlock( uint32_t * lock )
{
    uint32_t current = 1;
    uint32_t store = 0;

    for( ; ; )
    {
        if( Atomic_CompareAndSwap_u32( lock, store, current ) == 1 )
        {
            break;
        }

        /* Does context switch with negligible delay. */
        IotClock_SleepMs( 1 );
    }
}

static void _generateUniqueIdentifier( const char * pInput,
                                       size_t length,
                                       char * pIdentifier,
                                       size_t identifierLength )
{
    uint8_t hash[ MD5_HASH_LENGTH_BYTES ] = { 0 };
    int i;
    size_t ret;

    configASSERT( identifierLength >= ( 2 * MD5_HASH_LENGTH_BYTES ) );
    Utils_generateMD5Hash( pInput, length, hash, MD5_HASH_LENGTH_BYTES );

    for( i = 0; i < MD5_HASH_LENGTH_BYTES; i++ )
    {
        ret = snprintf( pIdentifier, ( IDENTIFIER_BYTE_ENCODE_LENGTH + 1 ), "%02X", hash[ i ] );
        configASSERT( ret > 0 );
        pIdentifier += IDENTIFIER_BYTE_ENCODE_LENGTH;
    }
}

/*
 * @brief Retrieves the device identifier.
 */
const char * getDeviceIdentifier( void )
{
    const char * pCertificate = IOT_DEVICE_CERTIFICATE;
    static uint32_t lock = 0;

    if( deviceIdentifier[ 0 ] == '\0' )
    {
        _atomicSpinlock_lock( &lock );

        if( deviceIdentifier[ 0 ] == '\0' )
        {
            if( ( pCertificate != NULL ) &&
                ( strcmp( pCertificate, "" ) != 0 ) )
            {
                _generateUniqueIdentifier( pCertificate, strlen( pCertificate ), deviceIdentifier, sizeof( deviceIdentifier ) );
            }
            else
            {
                strncpy( deviceIdentifier, "Unknown", sizeof( deviceIdentifier ) );
            }
        }

        _atomicSpinlock_unlock( &lock );
    }

    return deviceIdentifier;
}

/*
 * @brief Retrieves the device metrics.
 */
const char * getDeviceMetrics( void )
{
    const char * pDeviceIdentifier = NULL;
    static uint32_t lock = 0;

    if( deviceMetrics[ 0 ] == '\0' )
    {
        _atomicSpinlock_lock( &lock );

        if( deviceMetrics[ 0 ] == '\0' )
        {
            pDeviceIdentifier = getDeviceIdentifier();
            _generateDeviceMetrics( pDeviceIdentifier, strlen( pDeviceIdentifier ), deviceMetrics, sizeof( deviceMetrics ) );
        }

        _atomicSpinlock_unlock( &lock );
    }

    return deviceMetrics;
}

/*
 * @brief Retrieves the device metrics length.
 */
uint16_t getDeviceMetricsLength( void )
{
    const char * pDeviceMetrics = getDeviceMetrics();

    return ( uint16_t ) ( strlen( pDeviceMetrics ) );
}
