/*
 * Amazon FreeRTOS System Initialization
 * Copyright (C) 2017 Amazon.com, Inc. or its affiliates.  All Rights Reserved.
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

#include "aws_iot_serializer.h"
#include "cbor.h"

#define _castDecoderObjectToCborValue( pDecoderObject )    ( ( pDecoderObject )->pHandle )

#define _castDecoderIteratorToCborValue( iterator )        ( ( ( AwsIotSerializerDecoderObject_t * ) iterator )->pHandle )

#define _isArrayOrMap( dataType )                          ( ( ( dataType ) == AWS_IOT_SERIALIZER_CONTAINER_ARRAY ) || ( ( dataType ) == AWS_IOT_SERIALIZER_CONTAINER_MAP ) )

static AwsIotSerializerError_t _init( AwsIotSerializerDecoderObject_t * pDecoderObject,
                                      const uint8_t * pDataBuffer,
                                      size_t maxSize );
static AwsIotSerializerError_t _get( AwsIotSerializerDecoderIterator_t iterator,
                                     AwsIotSerializerDecoderObject_t * pValueObject );

static AwsIotSerializerError_t _find( AwsIotSerializerDecoderObject_t * pDecoderObject,
                                      const char * pKey,
                                      AwsIotSerializerDecoderObject_t * pValueObject );
static AwsIotSerializerError_t _stepIn( AwsIotSerializerDecoderObject_t * pDecoderObject,
                                        AwsIotSerializerDecoderIterator_t * pIterator );
static AwsIotSerializerError_t _stepOut( AwsIotSerializerDecoderIterator_t iterator,
                                         AwsIotSerializerDecoderObject_t * pDecoderObject );

static AwsIotSerializerError_t _next( AwsIotSerializerDecoderIterator_t iterator );

static bool _isEndOfContainer( AwsIotSerializerDecoderIterator_t iterator );

static void _destroy( AwsIotSerializerDecoderObject_t * pDecoderObject );

AwsIotSerializerDecodeInterface_t _AwsIotSerializerCborDecoder =
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
                                 AwsIotSerializerError_t * pSerializerError )
{
    /* TODO: assert cborError == 0 || *pSerializerError == 0 */

    /* Only translate if there is no error on serizlier currently. */
    if( *pSerializerError == AWS_IOT_SERIALIZER_SUCCESS )
    {
        switch( cborError )
        {
            case CborNoError:
                *pSerializerError = AWS_IOT_SERIALIZER_SUCCESS;
                break;

            case CborErrorOutOfMemory:
                *pSerializerError = AWS_IOT_SERIALIZER_BUFFER_TOO_SMALL;
                break;

            default:
                *pSerializerError = AWS_IOT_SERIALIZER_INTERNAL_FAILURE;
        }
    }
}

/*-----------------------------------------------------------*/

static AwsIotSerializerDataType_t _toSerializerType( CborType type )
{
    switch( type )
    {
        case CborIntegerType:

            return AWS_IOT_SERIALIZER_SCALAR_SIGNED_INT;

        case CborBooleanType:

            return AWS_IOT_SERIALIZER_SCALAR_BOOL;

        case CborByteStringType:

            return AWS_IOT_SERIALIZER_SCALAR_BYTE_STRING;

        case CborTextStringType:

            return AWS_IOT_SERIALIZER_SCALAR_TEXT_STRING;

        case CborArrayType:

            return AWS_IOT_SERIALIZER_CONTAINER_ARRAY;

        case CborMapType:

            return AWS_IOT_SERIALIZER_CONTAINER_MAP;

        default:

            return AWS_IOT_SERIALIZER_UNDEFINED;
    }
}

/*-----------------------------------------------------------*/

/* Construct DecoderObject based on the wrapper of CborValue. */
static AwsIotSerializerError_t _createDecoderObject( _cborValueWrapper_t * pCborValueWrapper,
                                                     AwsIotSerializerDecoderObject_t * pDecoderObject )
{
    AwsIotSerializerError_t returnedError = AWS_IOT_SERIALIZER_SUCCESS;
    CborError cborError = CborNoError;

    CborValue * pCborValue = &pCborValueWrapper->cborValue;
    CborValue next = { 0 };

    /* Get serializer data type from CborValue. */
    AwsIotSerializerDataType_t dataType = _toSerializerType( cbor_value_get_type( pCborValue ) );

    if( dataType == AWS_IOT_SERIALIZER_UNDEFINED )
    {
        return AWS_IOT_SERIALIZER_UNDEFINED_TYPE;
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
        pDecoderObject->pHandle = AwsIotSerializer_MallocCborValue( sizeof( _cborValueWrapper_t ) );
        if( pDecoderObject->pHandle == NULL )
        {
            return AWS_IOT_SERIALIZER_OUT_OF_MEMORY;
        }
        memcpy( pDecoderObject->pHandle, pCborValueWrapper, sizeof( _cborValueWrapper_t ) );
    }
    else /* Create scalar object. */
    {
        switch( dataType )
        {
            case AWS_IOT_SERIALIZER_SCALAR_SIGNED_INT:
               {
                   int i = 0;
                   cborError = cbor_value_get_int( pCborValue, &i );

                   /* TODO: assert no error on _createDecoderObject */

                   pDecoderObject->value.signedInt = i;

                   break;
               }

            case AWS_IOT_SERIALIZER_SCALAR_BYTE_STRING:
            case AWS_IOT_SERIALIZER_SCALAR_TEXT_STRING:
            {

                if( dataType == AWS_IOT_SERIALIZER_SCALAR_BYTE_STRING )
                {
                    cborError = cbor_value_copy_byte_string(
                            pCborValue,
                            pDecoderObject->value.pString,
                            &( pDecoderObject->value.stringLength ),
                            &next );
                }
                else
                {
                    cborError = cbor_value_copy_text_string(
                            pCborValue,
                            ( char* ) pDecoderObject->value.pString,
                            &( pDecoderObject->value.stringLength ),
                            &next );
                }

                if ( cborError != CborNoError )
                {

                    if( ( cborError == CborErrorOutOfMemory ) &&
                            ( pDecoderObject->value.pString == NULL ) &&
                            (  cbor_value_is_length_known( pCborValue ) ) )

                    {
                        /*
                         * If its a finite length text/byte string, and user have passed a null length buffer,
                         * we avoid copying the string by storing pointer to the start of the string.
                         */
                         pDecoderObject->value.pString = ( uint8_t *)( cbor_value_get_next_byte( &next ) - ( pDecoderObject->value.stringLength ) );
                    }
                    else
                    {
                        returnedError = AWS_IOT_SERIALIZER_INTERNAL_FAILURE;

                    }

                }

                break;
            }
            default:
                /* Other scalar types are not supported. */
                returnedError = AWS_IOT_SERIALIZER_NOT_SUPPORTED;
        }
    }

    return returnedError;
}

/*-----------------------------------------------------------*/

static AwsIotSerializerError_t _init( AwsIotSerializerDecoderObject_t * pDecoderObject,
                                      const uint8_t * pDataBuffer,
                                      size_t maxSize )
{
    CborParser * pCborParser = AwsIotSerializer_MallocCborParser( sizeof( CborParser ) );
    _cborValueWrapper_t cborValueWrapper = { 0 };

    if(  pCborParser == NULL  )
    {
        return AWS_IOT_SERIALIZER_OUT_OF_MEMORY;
    }

    AwsIotSerializerError_t returnedError = AWS_IOT_SERIALIZER_SUCCESS;
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

        AwsIotSerializer_FreeCborParser( pCborParser );
        if( _isArrayOrMap( pDecoderObject->type )
                && ( pDecoderObject->pHandle != NULL ) )
        {
            AwsIotSerializer_FreeCborValue( pDecoderObject->pHandle );
        }

    }

    _translateErrorCode( cborError, &returnedError );

    return returnedError;
}

/*-----------------------------------------------------------*/

static void _destroy( AwsIotSerializerDecoderObject_t * pDecoderObject )
{
    /* If it is not an container, no action. */
    if( !AwsIotSerializer_IsContainer( pDecoderObject ) )
    {
        return;
    }

    _cborValueWrapper_t * pCborValueWrapper = _castDecoderObjectToCborValue( pDecoderObject );

    /* If this is outmost object, the parser's memory needs to be freed. */
    if( pCborValueWrapper->isOutermost )
    {
        AwsIotSerializer_FreeCborParser( ( void * ) ( pCborValueWrapper->cborValue.parser ) );
    }

    AwsIotSerializer_FreeCborValue( pCborValueWrapper );

    /* Reset decoder object's handle to NULL. */
    pDecoderObject->pHandle = NULL;
}

/*-----------------------------------------------------------*/

static AwsIotSerializerError_t _get( AwsIotSerializerDecoderIterator_t iterator,
                                     AwsIotSerializerDecoderObject_t * pValueObject )
{
    _cborValueWrapper_t * pCborValueWrapper = _castDecoderIteratorToCborValue( iterator );

    return _createDecoderObject( pCborValueWrapper, pValueObject );
}

/*-----------------------------------------------------------*/

static AwsIotSerializerError_t _find( AwsIotSerializerDecoderObject_t * pDecoderObject,
                                      const char * pKey,
                                      AwsIotSerializerDecoderObject_t * pValueObject )
{
    _cborValueWrapper_t newCborValueWrapper;

    if( pDecoderObject->type != AWS_IOT_SERIALIZER_CONTAINER_MAP )
    {
        return AWS_IOT_SERIALIZER_INVALID_INPUT;
    }

    AwsIotSerializerError_t returnedError = AWS_IOT_SERIALIZER_SUCCESS;
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

            returnedError = AWS_IOT_SERIALIZER_NOT_FOUND;
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

static AwsIotSerializerError_t _stepIn( AwsIotSerializerDecoderObject_t * pDecoderObject,
                                        AwsIotSerializerDecoderIterator_t * pIterator )
{
    AwsIotSerializerError_t returnedError = AWS_IOT_SERIALIZER_SUCCESS;
    CborError cborError = CborNoError;

    _cborValueWrapper_t * pCborValueWrapper = _castDecoderObjectToCborValue( pDecoderObject );
    _cborValueWrapper_t * pNewCborValueWrapper = AwsIotSerializer_MallocCborValue( sizeof( _cborValueWrapper_t ) );

    cborError = cbor_value_enter_container(
        &pCborValueWrapper->cborValue,
        &pNewCborValueWrapper->cborValue );

    if( cborError == CborNoError )
    {
        AwsIotSerializerDecoderObject_t * pNewObject = AwsIotSerializer_MallocDecoderObject( sizeof( AwsIotSerializerDecoderObject_t ) );

        pNewObject->type = pDecoderObject->type;

        pNewCborValueWrapper->isOutermost = false;
        pNewObject->pHandle = ( void * ) pNewCborValueWrapper;

        *pIterator = ( AwsIotSerializerDecoderIterator_t ) pNewObject;

        returnedError = AWS_IOT_SERIALIZER_SUCCESS;
    }

    _translateErrorCode( cborError, &returnedError );

    return returnedError;
}

/*-----------------------------------------------------------*/

static AwsIotSerializerError_t _stepOut( AwsIotSerializerDecoderIterator_t iterator,
                                         AwsIotSerializerDecoderObject_t * pDecoderObject )
{
    AwsIotSerializerError_t returnedError = AWS_IOT_SERIALIZER_SUCCESS;
    CborError cborError = CborNoError;

    _cborValueWrapper_t * pOuterCborValueWrapper = _castDecoderObjectToCborValue( pDecoderObject );
    _cborValueWrapper_t * pInnerCborValueWrapper = _castDecoderIteratorToCborValue( iterator );

    cborError = cbor_value_leave_container(
        &pOuterCborValueWrapper->cborValue,
        &pInnerCborValueWrapper->cborValue );

    if( cborError == CborNoError )
    {
        AwsIotSerializer_FreeCborValue( pInnerCborValueWrapper );
        AwsIotSerializer_FreeDecoderObject( iterator );
    }

    _translateErrorCode( cborError, &returnedError );

    return returnedError;
}

/*-----------------------------------------------------------*/

static AwsIotSerializerError_t _next( AwsIotSerializerDecoderIterator_t iterator )
{
    AwsIotSerializerError_t returnedError = AWS_IOT_SERIALIZER_SUCCESS;
    CborError cborError = CborNoError;

    _cborValueWrapper_t * pCborValueWrapper = _castDecoderIteratorToCborValue( iterator );

    cborError = cbor_value_advance( &pCborValueWrapper->cborValue );

    _translateErrorCode( cborError, &returnedError );

    return returnedError;
}

/*-----------------------------------------------------------*/

static bool _isEndOfContainer( AwsIotSerializerDecoderIterator_t iterator )
{
    _cborValueWrapper_t * pCborValueWrapper = _castDecoderIteratorToCborValue( iterator );

    return cbor_value_at_end( &pCborValueWrapper->cborValue );
}
