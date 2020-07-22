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
 * @file aws_iot_serializer_tinycbor_decoder.c
 * @brief Implements APIs to parse and decode data from CBOR format. The file relies on tiny
 * CBOR library to parse and decode data from CBOR format. Supports all major data types within
 * the CBOR format.
 * The file implements the decoder interface in aws_iot_serialize.h.
 */

#include "iot_serializer.h"
#include "cbor.h"

#define _castDecoderObjectToCborValue( pDecoderObject )    ( ( pDecoderObject )->u.pHandle )

#define _castDecoderIteratorToCborValue( iterator )        ( ( ( IotSerializerDecoderObject_t * ) iterator )->u.pHandle )

#define _isArrayOrMap( dataType )                          ( ( ( dataType ) == IOT_SERIALIZER_CONTAINER_ARRAY ) || ( ( dataType ) == IOT_SERIALIZER_CONTAINER_MAP ) )

static IotSerializerError_t _init( IotSerializerDecoderObject_t * pDecoderObject,
                                   const uint8_t * pDataBuffer,
                                   size_t maxSize );
static IotSerializerError_t _get( IotSerializerDecoderIterator_t iterator,
                                  IotSerializerDecoderObject_t * pValueObject );

static IotSerializerError_t _find( IotSerializerDecoderObject_t * pDecoderObject,
                                   const char * pKey,
                                   IotSerializerDecoderObject_t * pValueObject );
static IotSerializerError_t _stepIn( IotSerializerDecoderObject_t * pDecoderObject,
                                     IotSerializerDecoderIterator_t * pIterator );
static IotSerializerError_t _stepOut( IotSerializerDecoderIterator_t iterator,
                                      IotSerializerDecoderObject_t * pDecoderObject );

static IotSerializerError_t _next( IotSerializerDecoderIterator_t iterator );

static bool _isEndOfContainer( IotSerializerDecoderIterator_t iterator );

static void _destroy( IotSerializerDecoderObject_t * pDecoderObject );

IotSerializerDecodeInterface_t _IotSerializerCborDecoder =
{
    .init             = _init,
    .get              = _get,
    .find             = _find,
    .stepIn           = _stepIn,
    .stepOut          = _stepOut,
    .next             = _next,
    .isEndOfContainer = _isEndOfContainer,
    .destroy          = _destroy
};

/* Wrapper CborValue with additional fields. */
typedef struct _cborValueWrapper
{
    CborValue cborValue;
    bool isOutermost;
} _cborValueWrapper_t;

/*-----------------------------------------------------------*/

static void _translateErrorCode( CborError cborError,
                                 IotSerializerError_t * pSerializerError )
{
    /* TODO: assert cborError == 0 || *pSerializerError == 0 */

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

static IotSerializerDataType_t _toSerializerType( CborType type )
{
    switch( type )
    {
        case CborIntegerType:

            return IOT_SERIALIZER_SCALAR_SIGNED_INT;

        case CborBooleanType:

            return IOT_SERIALIZER_SCALAR_BOOL;

        case CborByteStringType:

            return IOT_SERIALIZER_SCALAR_BYTE_STRING;

        case CborTextStringType:

            return IOT_SERIALIZER_SCALAR_TEXT_STRING;

        case CborArrayType:

            return IOT_SERIALIZER_CONTAINER_ARRAY;

        case CborMapType:

            return IOT_SERIALIZER_CONTAINER_MAP;

        default:

            return IOT_SERIALIZER_UNDEFINED;
    }
}

/*-----------------------------------------------------------*/

/* Construct DecoderObject based on the wrapper of CborValue. */
static IotSerializerError_t _createDecoderObject( _cborValueWrapper_t * pCborValueWrapper,
                                                  IotSerializerDecoderObject_t * pDecoderObject )
{
    IotSerializerError_t returnedError = IOT_SERIALIZER_SUCCESS;
    CborError cborError = CborNoError;

    CborValue * pCborValue = &pCborValueWrapper->cborValue;
    CborValue next = { 0 };

    /* Get serializer data type from CborValue. */
    IotSerializerDataType_t dataType = _toSerializerType( cbor_value_get_type( pCborValue ) );

    if( dataType == IOT_SERIALIZER_UNDEFINED )
    {
        return IOT_SERIALIZER_UNDEFINED_TYPE;
    }
    else
    {
        /* Set type to decoder object as long as it is defined. */
        pDecoderObject->type = dataType;
    }

    /* If this is a map or array. */
    if( _isArrayOrMap( dataType ) )
    {
        /* Save to decoder object's handle. */
        pDecoderObject->u.pHandle = IotSerializer_MallocCborValue( sizeof( _cborValueWrapper_t ) );

        if( pDecoderObject->u.pHandle == NULL )
        {
            return IOT_SERIALIZER_OUT_OF_MEMORY;
        }

        memcpy( pDecoderObject->u.pHandle, pCborValueWrapper, sizeof( _cborValueWrapper_t ) );
    }
    else /* Create scalar object. */
    {
        switch( dataType )
        {
            case IOT_SERIALIZER_SCALAR_BOOL:
               {
                   bool value = false;
                   cborError = cbor_value_get_boolean( pCborValue, &( value ) );

                   if( cborError == CborNoError )
                   {
                       pDecoderObject->u.value.u.booleanValue = value;
                   }
                   else
                   {
                       returnedError = IOT_SERIALIZER_INTERNAL_FAILURE;
                   }

                   break;
               }

            case IOT_SERIALIZER_SCALAR_SIGNED_INT:
               {
                   int i = 0;
                   cborError = cbor_value_get_int( pCborValue, &i );

                   if( cborError == CborNoError )
                   {
                       pDecoderObject->u.value.u.signedInt = i;
                   }
                   else
                   {
                       returnedError = IOT_SERIALIZER_INTERNAL_FAILURE;
                   }

                   break;
               }

            case IOT_SERIALIZER_SCALAR_BYTE_STRING:
            case IOT_SERIALIZER_SCALAR_TEXT_STRING:

                if( dataType == IOT_SERIALIZER_SCALAR_BYTE_STRING )
                {
                    cborError = cbor_value_copy_byte_string(
                        pCborValue,
                        pDecoderObject->u.value.u.string.pString,
                        &( pDecoderObject->u.value.u.string.length ),
                        &next );
                }
                else
                {
                    cborError = cbor_value_copy_text_string(
                        pCborValue,
                        ( char * ) pDecoderObject->u.value.u.string.pString,
                        &( pDecoderObject->u.value.u.string.length ),
                        &next );
                }

                if( cborError != CborNoError )
                {
                    if( ( cborError == CborErrorOutOfMemory ) &&
                        ( pDecoderObject->u.value.u.string.pString == NULL ) &&
                        ( cbor_value_is_length_known( pCborValue ) ) )

                    {
                        /*
                         * If its a finite length text/byte string, and user have passed a null length buffer,
                         * we avoid copying the string by storing pointer to the start of the string.
                         */
                        pDecoderObject->u.value.u.string.pString = ( uint8_t * ) ( cbor_value_get_next_byte( &next ) - ( pDecoderObject->u.value.u.string.length ) );
                    }
                    else
                    {
                        returnedError = IOT_SERIALIZER_INTERNAL_FAILURE;
                    }
                }

                break;

            default:
                /* Other scalar types are not supported. */
                returnedError = IOT_SERIALIZER_NOT_SUPPORTED;
        }
    }

    return returnedError;
}

/*-----------------------------------------------------------*/

static IotSerializerError_t _init( IotSerializerDecoderObject_t * pDecoderObject,
                                   const uint8_t * pDataBuffer,
                                   size_t maxSize )
{
    CborParser * pCborParser = IotSerializer_MallocCborParser( sizeof( CborParser ) );
    _cborValueWrapper_t cborValueWrapper = { .isOutermost = 0 };

    if( pCborParser == NULL )
    {
        return IOT_SERIALIZER_OUT_OF_MEMORY;
    }

    IotSerializerError_t returnedError = IOT_SERIALIZER_SUCCESS;
    CborError cborError = CborNoError;

    cborError = cbor_parser_init(
        pDataBuffer,
        maxSize,
        0,
        pCborParser,
        &cborValueWrapper.cborValue );

    /* If init succeeds, create the decoder object. */
    if( cborError == CborNoError )
    {
        cborValueWrapper.isOutermost = true;

        returnedError = _createDecoderObject( &cborValueWrapper, pDecoderObject );
    }

    /* If there is any error or decoder object is a scalar type, free the cbor resources. */
    if( cborError || returnedError )
    {
        /* pDecoderObject is untouched. */

        IotSerializer_FreeCborParser( pCborParser );

        if( _isArrayOrMap( pDecoderObject->type ) &&
            ( pDecoderObject->u.pHandle != NULL ) )
        {
            IotSerializer_FreeCborValue( pDecoderObject->u.pHandle );
        }
    }

    _translateErrorCode( cborError, &returnedError );

    return returnedError;
}

/*-----------------------------------------------------------*/

static void _destroy( IotSerializerDecoderObject_t * pDecoderObject )
{
    /* If it is not an container, no action. */
    if( !IotSerializer_IsContainer( pDecoderObject ) )
    {
        return;
    }

    _cborValueWrapper_t * pCborValueWrapper = _castDecoderObjectToCborValue( pDecoderObject );

    /* If this is outmost object, the parser's memory needs to be freed. */
    if( pCborValueWrapper->isOutermost )
    {
        IotSerializer_FreeCborParser( ( void * ) ( pCborValueWrapper->cborValue.parser ) );
    }

    IotSerializer_FreeCborValue( pCborValueWrapper );

    /* Reset decoder object's handle to NULL. */
    pDecoderObject->u.pHandle = NULL;
}

/*-----------------------------------------------------------*/

static IotSerializerError_t _get( IotSerializerDecoderIterator_t iterator,
                                  IotSerializerDecoderObject_t * pValueObject )
{
    _cborValueWrapper_t * pCborValueWrapper = _castDecoderIteratorToCborValue( iterator );

    return _createDecoderObject( pCborValueWrapper, pValueObject );
}

/*-----------------------------------------------------------*/

static IotSerializerError_t _find( IotSerializerDecoderObject_t * pDecoderObject,
                                   const char * pKey,
                                   IotSerializerDecoderObject_t * pValueObject )
{
    _cborValueWrapper_t newCborValueWrapper;

    if( pDecoderObject->type != IOT_SERIALIZER_CONTAINER_MAP )
    {
        return IOT_SERIALIZER_INVALID_INPUT;
    }

    IotSerializerError_t returnedError = IOT_SERIALIZER_SUCCESS;
    CborError cborError = CborNoError;

    _cborValueWrapper_t * pCborValueWrapper = _castDecoderObjectToCborValue( pDecoderObject );

    /* Set this object not to be outermost one. */
    newCborValueWrapper.isOutermost = false;

    cborError = cbor_value_map_find_value(
        &pCborValueWrapper->cborValue,
        pKey,
        &newCborValueWrapper.cborValue );

    if( cborError == CborNoError )
    {
        /* If not found the element in map. */
        if( newCborValueWrapper.cborValue.type == CborInvalidType )
        {
            /* pValueObject is untouched. */

            returnedError = IOT_SERIALIZER_NOT_FOUND;
        }
        else
        {
            returnedError = _createDecoderObject( &newCborValueWrapper, pValueObject );
        }
    }

    _translateErrorCode( cborError, &returnedError );

    return returnedError;
}


/*-----------------------------------------------------------*/

static IotSerializerError_t _stepIn( IotSerializerDecoderObject_t * pDecoderObject,
                                     IotSerializerDecoderIterator_t * pIterator )
{
    IotSerializerError_t returnedError = IOT_SERIALIZER_SUCCESS;
    CborError cborError = CborNoError;

    _cborValueWrapper_t * pCborValueWrapper = _castDecoderObjectToCborValue( pDecoderObject );
    _cborValueWrapper_t * pNewCborValueWrapper = IotSerializer_MallocCborValue( sizeof( _cborValueWrapper_t ) );

    cborError = cbor_value_enter_container(
        &pCborValueWrapper->cborValue,
        &pNewCborValueWrapper->cborValue );

    if( cborError == CborNoError )
    {
        IotSerializerDecoderObject_t * pNewObject = IotSerializer_MallocDecoderObject( sizeof( IotSerializerDecoderObject_t ) );

        pNewObject->type = pDecoderObject->type;

        pNewCborValueWrapper->isOutermost = false;
        pNewObject->u.pHandle = ( void * ) pNewCborValueWrapper;

        *pIterator = ( IotSerializerDecoderIterator_t ) pNewObject;

        returnedError = IOT_SERIALIZER_SUCCESS;
    }

    _translateErrorCode( cborError, &returnedError );

    return returnedError;
}

/*-----------------------------------------------------------*/

static IotSerializerError_t _stepOut( IotSerializerDecoderIterator_t iterator,
                                      IotSerializerDecoderObject_t * pDecoderObject )
{
    IotSerializerError_t returnedError = IOT_SERIALIZER_SUCCESS;
    CborError cborError = CborNoError;

    _cborValueWrapper_t * pOuterCborValueWrapper = _castDecoderObjectToCborValue( pDecoderObject );
    _cborValueWrapper_t * pInnerCborValueWrapper = _castDecoderIteratorToCborValue( iterator );

    cborError = cbor_value_leave_container(
        &pOuterCborValueWrapper->cborValue,
        &pInnerCborValueWrapper->cborValue );

    if( cborError == CborNoError )
    {
        IotSerializer_FreeCborValue( pInnerCborValueWrapper );
        IotSerializer_FreeDecoderObject( iterator );
    }

    _translateErrorCode( cborError, &returnedError );

    return returnedError;
}

/*-----------------------------------------------------------*/

static IotSerializerError_t _next( IotSerializerDecoderIterator_t iterator )
{
    IotSerializerError_t returnedError = IOT_SERIALIZER_SUCCESS;
    CborError cborError = CborNoError;

    _cborValueWrapper_t * pCborValueWrapper = _castDecoderIteratorToCborValue( iterator );

    cborError = cbor_value_advance( &pCborValueWrapper->cborValue );

    _translateErrorCode( cborError, &returnedError );

    return returnedError;
}

/*-----------------------------------------------------------*/

static bool _isEndOfContainer( IotSerializerDecoderIterator_t iterator )
{
    _cborValueWrapper_t * pCborValueWrapper = _castDecoderIteratorToCborValue( iterator );

    return cbor_value_at_end( &pCborValueWrapper->cborValue );
}
