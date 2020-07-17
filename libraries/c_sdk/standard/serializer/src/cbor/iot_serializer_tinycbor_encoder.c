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

/**
 * @file aws_iot_serializer_tinycbor_encoder.c
 * @brief Implements APIs to serialize data in CBOR format. The file relies on tiny
 * CBOR library to serialize data into CBOR format. Supports all major data types within
 * the CBOR format.
 * The file implements the encoder interface in aws_iot_serialize.h.
 */

#include "iot_serializer.h"
#include "cbor.h"

/* Translate cbor error to serializer error. */
static void _translateErrorCode( CborError cborError,
                                 IotSerializerError_t * pSerializerError );

static size_t _getEncodedSize( IotSerializerEncoderObject_t * pEncoderObject,
                               uint8_t * pDataBuffer );
static size_t _getExtraBufferSizeNeeded( IotSerializerEncoderObject_t * pEncoderObject );
static IotSerializerError_t _init( IotSerializerEncoderObject_t * pEncoderObject,
                                   uint8_t * pDataBuffer,
                                   size_t maxSize );
static void _destroy( IotSerializerEncoderObject_t * pEncoderObject );
static IotSerializerError_t _openContainer( IotSerializerEncoderObject_t * pEncoderObject,
                                            IotSerializerEncoderObject_t * pNewEncoderObject,
                                            size_t length );

static IotSerializerError_t _openContainerWithKey( IotSerializerEncoderObject_t * pEncoderObject,
                                                   const char * pKey,
                                                   IotSerializerEncoderObject_t * pNewEncoderObject,
                                                   size_t length );
static IotSerializerError_t _closeContainer( IotSerializerEncoderObject_t * pEncoderObject,
                                             IotSerializerEncoderObject_t * pNewEncoderObject );
static IotSerializerError_t _append( IotSerializerEncoderObject_t * pEncoderObject,
                                     IotSerializerScalarData_t scalarData );
static IotSerializerError_t _appendKeyValue( IotSerializerEncoderObject_t * pEncoderObject,
                                             const char * pKey,
                                             IotSerializerScalarData_t scalarData );


IotSerializerEncodeInterface_t _IotSerializerCborEncoder =
{
    .getEncodedSize           = _getEncodedSize,
    .getExtraBufferSizeNeeded = _getExtraBufferSizeNeeded,
    .init                     = _init,
    .destroy                  = _destroy,
    .openContainer            = _openContainer,
    .openContainerWithKey     = _openContainerWithKey,
    .closeContainer           = _closeContainer,
    .append                   = _append,
    .appendKeyValue           = _appendKeyValue,
};

/*-----------------------------------------------------------*/

static void _translateErrorCode( CborError cborError,
                                 IotSerializerError_t * pSerializerError )
{
    /* This function translate cbor error to serializer error.
     * It doesn't make sense that both of them are of error (greater than 0).
     */
    IotSerializer_Assert( cborError == 0 || *pSerializerError == 0 );

    /* Only translate if there is no error on serializer currently. */
    if( *pSerializerError == IOT_SERIALIZER_SUCCESS )
    {
        switch( cborError )
        {
            case CborNoError:
                *pSerializerError = IOT_SERIALIZER_SUCCESS;
                break;

            case CborErrorOutOfMemory:
                *pSerializerError = IOT_SERIALIZER_BUFFER_TOO_SMALL;
                break;

            default:
                *pSerializerError = IOT_SERIALIZER_INTERNAL_FAILURE;
        }
    }
}

/*-----------------------------------------------------------*/

static size_t _getEncodedSize( IotSerializerEncoderObject_t * pEncoderObject,
                               uint8_t * pDataBuffer )
{
    CborEncoder * pCborEncoder = ( CborEncoder * ) pEncoderObject->pHandle;

    return cbor_encoder_get_buffer_size( pCborEncoder, pDataBuffer );
}

/*-----------------------------------------------------------*/

static size_t _getExtraBufferSizeNeeded( IotSerializerEncoderObject_t * pEncoderObject )
{
    CborEncoder * pCborEncoder = ( CborEncoder * ) pEncoderObject->pHandle;

    return cbor_encoder_get_extra_bytes_needed( pCborEncoder );
}

/*-----------------------------------------------------------*/

static IotSerializerError_t _init( IotSerializerEncoderObject_t * pEncoderObject,
                                   uint8_t * pDataBuffer,
                                   size_t maxSize )
{
    IotSerializerError_t returnedError = IOT_SERIALIZER_SUCCESS;

    /* Unused flags for tinycbor init. */
    int unusedCborFlags = 0;

    CborEncoder * pCborEncoder = IotSerializer_MallocCborEncoder( sizeof( CborEncoder ) );

    if( pCborEncoder != NULL )
    {
        /* Store the CborEncoder pointer to handle. */
        pEncoderObject->pHandle = pCborEncoder;

        /* Always set outmost type to IOT_SERIALIZER_CONTAINER_STREAM. */
        pEncoderObject->type = IOT_SERIALIZER_CONTAINER_STREAM;

        /* Perform the tinycbor init. */
        cbor_encoder_init( pCborEncoder, pDataBuffer, maxSize, unusedCborFlags );
    }
    else
    {
        /* pEncoderObject is untouched. */

        returnedError = IOT_SERIALIZER_OUT_OF_MEMORY;
    }

    return returnedError;
}

/*-----------------------------------------------------------*/

static void _destroy( IotSerializerEncoderObject_t * pEncoderObject )
{
    CborEncoder * pCborEncoder = ( CborEncoder * ) pEncoderObject->pHandle;

    /* Free the memory allocated in init function. */
    IotSerializer_FreeCborEncoder( pCborEncoder );

    /* Reset pHandle to be NULL. */
    pEncoderObject->pHandle = NULL;
}

/*-----------------------------------------------------------*/

static IotSerializerError_t _openContainer( IotSerializerEncoderObject_t * pEncoderObject,
                                            IotSerializerEncoderObject_t * pNewEncoderObject,
                                            size_t length )
{
    /* New object must be a container of map or array. */
    if( ( pNewEncoderObject->type != IOT_SERIALIZER_CONTAINER_ARRAY ) &&
        ( pNewEncoderObject->type != IOT_SERIALIZER_CONTAINER_MAP ) )
    {
        return IOT_SERIALIZER_INVALID_INPUT;
    }

    IotSerializerError_t returnedError = IOT_SERIALIZER_SUCCESS;
    CborError cborError = CborNoError;

    CborEncoder * pOuterEncoder = ( CborEncoder * ) pEncoderObject->pHandle;
    CborEncoder * pInnerEncoder = IotSerializer_MallocCborEncoder( sizeof( CborEncoder ) );

    if( pInnerEncoder != NULL )
    {
        /* Store the CborEncoder pointer to handle. */
        pNewEncoderObject->pHandle = pInnerEncoder;

        switch( pNewEncoderObject->type )
        {
            case IOT_SERIALIZER_CONTAINER_MAP:
                cborError = cbor_encoder_create_map( pOuterEncoder, pInnerEncoder, length );
                break;

            case IOT_SERIALIZER_CONTAINER_ARRAY:
                cborError = cbor_encoder_create_array( pOuterEncoder, pInnerEncoder, length );
                break;

            default:
                IotSerializer_Assert( 0 );
        }
    }
    else
    {
        /* pEncoderObject is untouched. */
        returnedError = IOT_SERIALIZER_OUT_OF_MEMORY;
    }

    _translateErrorCode( cborError, &returnedError );

    return returnedError;
}

/*-----------------------------------------------------------*/

static IotSerializerError_t _openContainerWithKey( IotSerializerEncoderObject_t * pEncoderObject,
                                                   const char * pKey,
                                                   IotSerializerEncoderObject_t * pNewEncoderObject,
                                                   size_t length )
{
    IotSerializerScalarData_t keyScalarData = IotSerializer_ScalarTextString( pKey );

    IotSerializerError_t returnedError = _append( pEncoderObject, keyScalarData );

    /* Buffer too small is a special error case that serialization should continue. */
    if( ( returnedError == IOT_SERIALIZER_SUCCESS ) || ( returnedError == IOT_SERIALIZER_BUFFER_TOO_SMALL ) )
    {
        returnedError = _openContainer( pEncoderObject, pNewEncoderObject, length );
    }

    return returnedError;
}

/*-----------------------------------------------------------*/

static IotSerializerError_t _closeContainer( IotSerializerEncoderObject_t * pEncoderObject,
                                             IotSerializerEncoderObject_t * pNewEncoderObject )
{
    IotSerializerError_t returnedError = IOT_SERIALIZER_SUCCESS;
    CborError cborError = CborNoError;

    CborEncoder * pOuterEncoder = ( CborEncoder * ) pEncoderObject->pHandle;
    CborEncoder * pInnerEncoder = ( CborEncoder * ) pNewEncoderObject->pHandle;

    cborError = cbor_encoder_close_container( pOuterEncoder, pInnerEncoder );

    /* Free inner encoder's memory regardless the result of "close container". */
    IotSerializer_FreeCborEncoder( pInnerEncoder );

    _translateErrorCode( cborError, &returnedError );

    return returnedError;
}

/*-----------------------------------------------------------*/

static IotSerializerError_t _append( IotSerializerEncoderObject_t * pEncoderObject,
                                     IotSerializerScalarData_t scalarData )
{
    IotSerializerError_t returnedError = IOT_SERIALIZER_SUCCESS;
    CborError cborError = CborNoError;

    CborEncoder * pCborEncoder = ( CborEncoder * ) pEncoderObject->pHandle;

    switch( scalarData.type )
    {
        case IOT_SERIALIZER_SCALAR_SIGNED_INT:
            cborError = cbor_encode_int( pCborEncoder, scalarData.value.u.signedInt );
            break;

        case IOT_SERIALIZER_SCALAR_TEXT_STRING:
            cborError = cbor_encode_text_string( pCborEncoder, ( char * ) scalarData.value.u.string.pString, scalarData.value.u.string.length );
            break;

        case IOT_SERIALIZER_SCALAR_BYTE_STRING:
            cborError = cbor_encode_byte_string( pCborEncoder, scalarData.value.u.string.pString, scalarData.value.u.string.length );
            break;

        case IOT_SERIALIZER_SCALAR_BOOL:
            cborError = cbor_encode_boolean( pCborEncoder, scalarData.value.u.booleanValue );
            break;

        case IOT_SERIALIZER_SCALAR_NULL:
            cborError = cbor_encode_null( pCborEncoder );
            break;

        default:
            returnedError = IOT_SERIALIZER_UNDEFINED_TYPE;
    }

    _translateErrorCode( cborError, &returnedError );

    return returnedError;
}

/*-----------------------------------------------------------*/

static IotSerializerError_t _appendKeyValue( IotSerializerEncoderObject_t * pEncoderObject,
                                             const char * pKey,
                                             IotSerializerScalarData_t scalarData )
{
    IotSerializerScalarData_t keyScalarData = IotSerializer_ScalarTextString( pKey );
    IotSerializerError_t returnedError = _append( pEncoderObject, keyScalarData );

    /* Buffer too small is a special error case that serialization should continue. */
    if( ( returnedError == IOT_SERIALIZER_SUCCESS ) || ( returnedError == IOT_SERIALIZER_BUFFER_TOO_SMALL ) )
    {
        returnedError = _append( pEncoderObject, scalarData );
    }

    return returnedError;
}
