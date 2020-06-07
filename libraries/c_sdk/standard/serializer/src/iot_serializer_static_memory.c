/*
 * FreeRTOS Serializer V1.1.2
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

/* The config header is always included first. */
#include "iot_config.h"

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

/* TinyCBOR include. */
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

/**
 * @todo Placeholder.
 */
    typedef struct _cborValueWrapper
    {
        CborValue cborValue; /**< @brief Placeholder. */
        bool isOutermost;    /**< @brief Placeholder. */
    } _cborValueWrapper_t;

/*-----------------------------------------------------------*/

/*
 * Static memory buffers and flags, allocated and zeroed at compile-time.
 */
    static bool _inUseCborEncoders[ IOT_SERIALIZER_CBOR_ENCODERS ] = { 0 };
    static CborEncoder _cborEncoders[ IOT_SERIALIZER_CBOR_ENCODERS ] = { { .data = { 0 } } };

    static bool _inUseCborParsers[ IOT_SERIALIZER_CBOR_PARSERS ] = { 0 };
    static CborParser _cborParsers[ IOT_SERIALIZER_CBOR_PARSERS ] = { { 0 } };

    static bool _inUseCborValues[ IOT_SERIALIZER_CBOR_VALUES ] = { 0 };
    static _cborValueWrapper_t _cborValues[ IOT_SERIALIZER_CBOR_VALUES ] = { { .isOutermost = false } };

    static bool _inUseDecoderObjects[ IOT_SERIALIZER_DECODER_OBJECTS ] = { 0 };
    static IotSerializerDecoderObject_t _decoderObjects[ IOT_SERIALIZER_DECODER_OBJECTS ] = { { 0 } };

/*-----------------------------------------------------------*/

    void * IotSerializer_MallocCborEncoder( size_t size )
    {
        int32_t freeIndex = -1;
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

    void IotSerializer_FreeCborEncoder( void * ptr )
    {
        IotStaticMemory_ReturnInUse( ptr,
                                     _cborEncoders,
                                     _inUseCborEncoders,
                                     IOT_SERIALIZER_CBOR_ENCODERS,
                                     sizeof( CborEncoder ) );
    }

/*-----------------------------------------------------------*/

    void * IotSerializer_MallocCborParser( size_t size )
    {
        int32_t freeIndex = -1;
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

    void IotSerializer_FreeCborParser( void * ptr )
    {
        IotStaticMemory_ReturnInUse( ptr,
                                     _cborParsers,
                                     _inUseCborParsers,
                                     IOT_SERIALIZER_CBOR_PARSERS,
                                     sizeof( CborParser ) );
    }

/*-----------------------------------------------------------*/

    void * IotSerializer_MallocCborValue( size_t size )
    {
        int32_t freeIndex = -1;
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

    void IotSerializer_FreeCborValue( void * ptr )
    {
        IotStaticMemory_ReturnInUse( ptr,
                                     _cborValues,
                                     _inUseCborValues,
                                     IOT_SERIALIZER_CBOR_VALUES,
                                     sizeof( _cborValueWrapper_t ) );
    }

/*-----------------------------------------------------------*/

    void * IotSerializer_MallocDecoderObject( size_t size )
    {
        int32_t freeIndex = -1;
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

    void IotSerializer_FreeDecoderObject( void * ptr )
    {
        IotStaticMemory_ReturnInUse( ptr,
                                     _decoderObjects,
                                     _inUseDecoderObjects,
                                     IOT_SERIALIZER_DECODER_OBJECTS,
                                     sizeof( IotSerializerDecoderObject_t ) );
    }

#endif /* if IOT_STATIC_MEMORY_ONLY == 1 */
