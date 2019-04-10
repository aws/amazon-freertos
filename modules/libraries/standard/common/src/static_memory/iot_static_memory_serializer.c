/*
 * Copyright (C) 2019 Amazon.com, Inc. or its affiliates.  All Rights Reserved.
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

/* Build using a config header, if provided. */
#ifdef IOT_CONFIG_FILE
    #include IOT_CONFIG_FILE
#endif

/* This file should only be compiled if dynamic memory allocation is forbidden. */
#if IOT_STATIC_MEMORY_ONLY == 1

/* Standard includes. */
    #include <stdbool.h>
    #include <stddef.h>
    #include <string.h>

/* Static memory include. */
    #include "private/iot_static_memory.h"

/* Metrics include. */
    #include "iot_serializer.h"

    #include "cbor.h"

    #ifndef IOT_SERIALIZER_CBOR_ENCODERS
        #define IOT_SERIALIZER_CBOR_ENCODERS    ( 10 )
    #endif

    #ifndef IOT_SERIALIZER_CBOR_PARSERS
        #define IOT_SERIALIZER_CBOR_PARSERS    ( 10 )
    #endif

    #ifndef IOT_SERIALIZER_CBOR_VALUES
        #define IOT_SERIALIZER_CBOR_VALUES    ( 10 )
    #endif

    #ifndef IOT_SERIALIZER_DECODER_OBJECTS
        #define IOT_SERIALIZER_DECODER_OBJECTS    ( 10 )
    #endif

/* Validate static memory configuration settings. */
    #if IOT_SERIALIZER_CBOR_ENCODERS <= 0
        #error "IOT_SERIALIZER_CBOR_ENCODERS cannot be 0 or negative."
    #endif

    #if IOT_SERIALIZER_CBOR_PARSERS <= 0
        #error "IOT_SERIALIZER_CBOR_PARSERS cannot be 0 or negative."
    #endif

    #if IOT_SERIALIZER_CBOR_VALUES <= 0
        #error "IOT_SERIALIZER_CBOR_VALUES cannot be 0 or negative."
    #endif

    #if IOT_SERIALIZER_DECODER_OBJECTS <= 0
        #error "IOT_SERIALIZER_DECODER_OBJECTS cannot be 0 or negative."
    #endif

    typedef struct _cborValueWrapper
    {
        CborValue cborValue;
        bool isOutermost;
    } _cborValueWrapper_t;

/*-----------------------------------------------------------*/

/* Extern declarations of common static memory functions in iot_static_memory_common.c
 * Because these functions are specific to this static memory implementation, they are
 * not placed in the common static memory header file. */
    extern int IotStaticMemory_FindFree( bool * const pInUse,
                                         int limit );
    extern void IotStaticMemory_ReturnInUse( void * ptr,
                                             void * const pPool,
                                             bool * const pInUse,
                                             int limit,
                                             size_t elementSize );

/*-----------------------------------------------------------*/

/*
 * Static memory buffers and flags, allocated and zeroed at compile-time.
 */
    static bool _inUseCborEncoders[ IOT_SERIALIZER_CBOR_ENCODERS ] = { 0 };
    static CborEncoder _cborEncoders[ IOT_SERIALIZER_CBOR_ENCODERS ] = { { 0 } };

    static bool _inUseCborParsers[ IOT_SERIALIZER_CBOR_PARSERS ] = { 0 };
    static CborParser _cborParsers[ IOT_SERIALIZER_CBOR_PARSERS ] = { { 0 } };

    static bool _inUseCborValues[ IOT_SERIALIZER_CBOR_VALUES ] = { 0 };
    static _cborValueWrapper_t _cborValues[IOT_SERIALIZER_CBOR_VALUES] = { { 0 } };

    static bool _inUseDecoderObjects[ IOT_SERIALIZER_DECODER_OBJECTS ] = { 0 };
    static IotSerializerDecoderObject_t _decoderObjects[ IOT_SERIALIZER_DECODER_OBJECTS ] = { { 0 } };

/*-----------------------------------------------------------*/

    void * Iot_MallocSerializerCborEncoder( size_t size )
    {
        int freeIndex = -1;
        void * pNewCborEncoder = NULL;

        if( size == sizeof( CborEncoder ) )
        {
            freeIndex = IotStaticMemory_FindFree( _inUseCborEncoders,
                                                  IOT_SERIALIZER_CBOR_ENCODERS );

            if( freeIndex != -1 )
            {
                pNewCborEncoder = &( _cborEncoders[ freeIndex ] );
            }
        }

        return pNewCborEncoder;
    }

/*-----------------------------------------------------------*/

    void Iot_FreeSerializerCborEncoder( void * ptr )
    {
        IotStaticMemory_ReturnInUse( ptr,
                                     _cborEncoders,
                                     _inUseCborEncoders,
                                     IOT_SERIALIZER_CBOR_ENCODERS,
                                     sizeof( CborEncoder ) );
    }

/*-----------------------------------------------------------*/

    void * Iot_MallocSerializerCborParser( size_t size )
    {
        int freeIndex = -1;
        void * pNewCborParser = NULL;

        if( size == sizeof( CborParser ) )
        {
            freeIndex = IotStaticMemory_FindFree( _inUseCborParsers,
                                                  IOT_SERIALIZER_CBOR_PARSERS );

            if( freeIndex != -1 )
            {
                pNewCborParser = &( _cborParsers[ freeIndex ] );
            }
        }

        return pNewCborParser;
    }

/*-----------------------------------------------------------*/

    void Iot_FreeSerializerCborParser( void * ptr )
    {
        IotStaticMemory_ReturnInUse( ptr,
                                     _cborParsers,
                                     _inUseCborParsers,
                                     IOT_SERIALIZER_CBOR_PARSERS,
                                     sizeof( CborParser ) );
    }

/*-----------------------------------------------------------*/

    void * Iot_MallocSerializerCborValue( size_t size )
    {
        int freeIndex = -1;
        void * pNewCborValue = NULL;

        if( size == sizeof( _cborValueWrapper_t ) )
        {
            freeIndex = IotStaticMemory_FindFree( _inUseCborValues,
                                                  IOT_SERIALIZER_CBOR_VALUES );

            if( freeIndex != -1 )
            {
                pNewCborValue = &( _cborValues[ freeIndex ] );
            }
        }

        return pNewCborValue;
    }

/*-----------------------------------------------------------*/

    void Iot_FreeSerializerCborValue( void * ptr )
    {
        IotStaticMemory_ReturnInUse( ptr,
                                     _cborValues,
                                     _inUseCborValues,
                                     IOT_SERIALIZER_CBOR_VALUES,
                                     sizeof( _cborValueWrapper_t ) );
    }

/*-----------------------------------------------------------*/

    void * Iot_MallocSerializerDecoderObject( size_t size )
    {
        int freeIndex = -1;
        void * pNewDecoderObject = NULL;

        if( size == sizeof( IotSerializerDecoderObject_t ) )
        {
            freeIndex = IotStaticMemory_FindFree( _inUseDecoderObjects,
                                                  IOT_SERIALIZER_DECODER_OBJECTS );

            if( freeIndex != -1 )
            {
                pNewDecoderObject = &( _decoderObjects[ freeIndex ] );
            }
        }

        return pNewDecoderObject;
    }

/*-----------------------------------------------------------*/

    void Iot_FreeSerializerDecoderObject( void * ptr )
    {
        IotStaticMemory_ReturnInUse( ptr,
                                     _decoderObjects,
                                     _inUseDecoderObjects,
                                     IOT_SERIALIZER_DECODER_OBJECTS,
                                     sizeof( IotSerializerDecoderObject_t ) );
    }

#endif /* if IOT_STATIC_MEMORY_ONLY == 1 */
