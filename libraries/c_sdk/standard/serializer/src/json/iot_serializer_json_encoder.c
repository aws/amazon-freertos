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
 * @file iot_serializer_json_encoder.c
 * @brief Implements APIs to serialize data in JSON format. Supports JSON primitives-
 * Numbers, Strings, boolean and null types, container types - arrays and maps.
 * A special type called binary string is also supported as a value type. By default
 * binary strings are base-64 encoded.
 * The file implements encoder interface in aws_iot_serialize.h.
 */

#include <string.h>
#include <stdint.h>
#include <stdio.h>

#include "iot_serializer.h"
#include "mbedtls/base64.h"

#define _JSON_OBJECT_START_CHAR           '{'
#define _JSON_OBJECT_END_CHAR             '}'

#define _JSON_ARRAY_START_CHAR            '['
#define _JSON_ARRAY_END_CHAR              ']'

#define _JSON_STRING_WRAPPER              '"'

#define _JSON_KEY_VALUE_PAIR_SEPARATOR    ':'

#define _JSON_VALUE_SEPARATOR             ','

#define _JSON_BOOL_TRUE                   "true"
#define _JSON_BOOL_FALSE                  "false"
#define _JSON_NULL_VALUE                  "null"

#define _JSON_BOOL_TRUE_LENGTH            ( 4 )
#define _JSON_BOOL_FALSE_LENGTH           ( 5 )
#define _JSON_NULL_VALUE_LENGTH           ( 4 )

#define _JSON_INT8_LENGTH                 ( 4 )
#define _JSON_INT16_LENGTH                ( 6 )
#define _JSON_INT32_LENGTH                ( 11 )
#define _JSON_INT64_LENGTH                ( 20 )

#define _jsonIntegerLength( value )                                         \
    (                                                                       \
        ( value >= INT8_MIN && value <= INT8_MAX ) ? _JSON_INT8_LENGTH :    \
        ( value >= INT16_MIN && value <= INT16_MAX ) ? _JSON_INT16_LENGTH : \
        ( value >= INT32_MIN && value <= INT32_MAX ) ? _JSON_INT32_LENGTH : \
        _JSON_INT64_LENGTH )

#define _jsonBoolLength( value )           ( ( value == true ) ? _JSON_BOOL_TRUE_LENGTH : _JSON_BOOL_FALSE_LENGTH )

#define _jsonStringLength( length )        ( ( length ) + 2 )

#define _base64EncodedLength( length )     ( 4 * ( ( ( length ) + 2 ) / 3 ) )

#define _jsonByteStringLength( length )    ( _jsonStringLength( _base64EncodedLength( length ) ) )

#define _jsonEmptyContainerLength    ( 2 )

#define _jsonKeyValuePairLength( keyLength, valueLength )    ( _jsonStringLength( keyLength ) + 1 + ( valueLength ) )

#define _jsonContainerPointer( pContainer )                  ( ( pContainer )->pBuffer + ( pContainer )->offset )

#define _jsonIsValidScalar( data )                           ( ( ( data )->type >= IOT_SERIALIZER_SCALAR_NULL ) && ( ( data )->type <= IOT_SERIALIZER_SCALAR_BYTE_STRING ) )

#define _jsonIsValidContainer( container )                            \
    (                                                                 \
        ( container != NULL ) &&                                      \
        ( ( container )->type >= IOT_SERIALIZER_CONTAINER_STREAM ) && \
        ( ( container )->type <= IOT_SERIALIZER_CONTAINER_MAP ) )

typedef struct _jsonContainer
{
    IotSerializerDataType_t containerType;
    bool isEmpty;
    uint8_t * pBuffer;
    size_t offset;
    size_t remainingLength;
    size_t maxLength;
    size_t overflowLength;
} _jsonContainer_t;

/**
 * @brief Returns the size of the encoded JSON data,
 *
 * Returns the actual JSON encoded size at that point. If a NULL buffer is used or a different
 * buffer is passed as input,  the function returns zero.
 *
 * @param[in] pEncoderObject Pointer to the outermost JSON encoder
 * @param[in] pDataBuffer Pointer to the data buffer passed in the init function.
 *
 * @return Size of the encoded buffer in bytes
 */
static size_t _getEncodedSize( IotSerializerEncoderObject_t * pEncoderObject,
                               uint8_t * pDataBuffer );

/**
 * @brief Returns the extra buffer size required, if the encoded data overflows the buffer provided.
 *
 * The function can also be used to pre-calculate the size of the buffer needed to encode a JSON object,
 * by initializing the encoder object with a null buffer.
 *
 * @param[in] pEncoderObject: Pointer to the outermost JSON encoder object
 *
 * @return Additional buffer size needed in bytes.
 */
static size_t _getExtraBufferSizeNeeded( IotSerializerEncoderObject_t * pEncoderObject );

/**
 * @brief Initializes the JSON encoder object
 *
 * User can pass as input a NULL buffer to pre-calculate the size of the encoded JSON data.
 *
 * @param[in] pEncoderObject: Pointer to the JSON encoder object
 * @param[in] pBuffer Pointer to the buffer
 * @param[in] maxSize Max Size of the buffer
 *
 * @return IOT_SERIALIZER_SUCCESS on success
 */
static IotSerializerError_t _init( IotSerializerEncoderObject_t * pEncoderObject,
                                   uint8_t * pDataBuffer,
                                   size_t maxSize );

/**
 * @brief Destroys the encoder object
 *
 * Cleanup any internal buffer associated with the encoder object.
 *
 * @param pEncoderObject Pointer to the encoder object
 * @return IOT_SERIALIZER_SUCCESS on success
 */
static void _destroy( IotSerializerEncoderObject_t * pEncoderObject );

/**
 * @brief Open a new child container within a parent container.
 *
 * User needs to set the container type for the child container. In JSON, containers can only be either
 * map/object (IOT_SERIALIZER_CONTAINER_MAP)  or arrays (IOT_SERIALIZER_CONTAINER_ARRAY)
 *
 * @param[in] pEncoderObject Object representing the parent container
 * @param[in] pNewEncoderObject Object representing the child container
 * @param[in] length Length of the container
 * @return IOT_SERIALIZER_SUCCESS on success
 */
static IotSerializerError_t _openContainer( IotSerializerEncoderObject_t * pEncoderObject,
                                            IotSerializerEncoderObject_t * pNewEncoderObject,
                                            size_t length );

/**
 * @brief Open a new child container as a key value pair within a parent container.
 *
 * User needs to set the container type for the child container. In JSON, containers can only be either
 * map/object (IOT_SERIALIZER_CONTAINER_MAP)  or arrays (IOT_SERIALIZER_CONTAINER_ARRAY).
 * Parent container can only be a map ( IOT_SERIALIZER_CONTAINER_MAP )
 *
 * @param[in] pEncoderObject Object representing the parent container
 * @param[in] pNewEncoderObject Object representing the child container
 * @param[in] length Length of the container
 * @return IOT_SERIALIZER_SUCCESS on success
 */
static IotSerializerError_t _openContainerWithKey( IotSerializerEncoderObject_t * pEncoderObject,
                                                   const char * pKey,
                                                   IotSerializerEncoderObject_t * pNewEncoderObject,
                                                   size_t length );

/**
 * @brief Closes a child container.
 *
 * @param[in] pEncoderObject Object representing the parent container
 * @param[in] pNewEncoderObject Object representing the child container
 * @return IOT_SERIALIZER_SUCCESS on success
 */
static IotSerializerError_t _closeContainer( IotSerializerEncoderObject_t * pEncoderObject,
                                             IotSerializerEncoderObject_t * pNewEncoderObject );

/**
 * @brief Appends a scalar value to the JSON container
 *
 * @param[in] pEncoderObject Pointer to the container object
 * @param[in] scalarData scalar value to be appended
 * @return IOT_SERIALIZER_SUCCESS on success
 */
static IotSerializerError_t _append( IotSerializerEncoderObject_t * pEncoderObject,
                                     IotSerializerScalarData_t scalarData );

/**
 * @brief Inserts a key value pair to the JSON container.
 *
 * @param[in] pEncoderObject Pointer to the container object
 * @param[in] scalarData scalar value to be appended
 * @return IOT_SERIALIZER_SUCCESS on success
 */
static IotSerializerError_t _appendKeyValue( IotSerializerEncoderObject_t * pEncoderObject,
                                             const char * pKey,
                                             IotSerializerScalarData_t scalarData );


static size_t _getSerializedLength( _jsonContainer_t * pContainer,
                                    IotSerializerDataType_t dataType,
                                    IotSerializerScalarData_t * pScalarData );


static size_t _getKeyValueLength( _jsonContainer_t * pContainer,
                                  size_t keyLength,
                                  IotSerializerDataType_t valueType,
                                  IotSerializerScalarData_t * pScalarValue );

static size_t _getValueLength( _jsonContainer_t * pContainer,
                               IotSerializerDataType_t valueType,
                               IotSerializerScalarData_t * pScalarValue );

static void _stopContainer( _jsonContainer_t * pContainer,
                            IotSerializerDataType_t containerType );
static void _appendTextString( _jsonContainer_t * pContainer,
                               const char * pStr,
                               size_t strLength );
static void _appendByteString( _jsonContainer_t * pContainer,
                               uint8_t * pByteString,
                               size_t stringLength );
static void _appendInteger( _jsonContainer_t * pContainer,
                            int64_t signedInteger );
static void _appendBoolean( _jsonContainer_t * pContainer,
                            bool boolValue );
static void _appendData( _jsonContainer_t * pContainer,
                         IotSerializerDataType_t dataType,
                         IotSerializerScalarData_t * pScalarData );
static void _appendJsonValue( _jsonContainer_t * pContainer,
                              IotSerializerDataType_t dataType,
                              IotSerializerScalarData_t * pScalarData );
static void _appendJsonKeyValuePair( _jsonContainer_t * pContainer,
                                     const char * pKey,
                                     size_t keyLength,
                                     IotSerializerDataType_t valueType,
                                     IotSerializerScalarData_t * pScalarValue );

IotSerializerEncodeInterface_t _IotSerializerJsonEncoder =
{
    .getEncodedSize           = _getEncodedSize,
    .getExtraBufferSizeNeeded = _getExtraBufferSizeNeeded,
    .init                     = _init,
    .destroy                  = _destroy,
    .openContainer            = _openContainer,
    .openContainerWithKey     = _openContainerWithKey,
    .closeContainer           = _closeContainer,
    .append                   = _append,
    .appendKeyValue           = _appendKeyValue
};

/*-----------------------------------------------------------*/

static void _stopContainer( _jsonContainer_t * pContainer,
                            IotSerializerDataType_t containerType )
{
    if( containerType == IOT_SERIALIZER_CONTAINER_ARRAY )
    {
        pContainer->pBuffer[ pContainer->offset++ ] = _JSON_ARRAY_END_CHAR;
    }
    else if( containerType == IOT_SERIALIZER_CONTAINER_MAP )
    {
        pContainer->pBuffer[ pContainer->offset++ ] = _JSON_OBJECT_END_CHAR;
    }
}
/*-----------------------------------------------------------*/

static size_t _getSerializedLength( _jsonContainer_t * pContainer,
                                    IotSerializerDataType_t dataType,
                                    IotSerializerScalarData_t * pScalarData )

{
    size_t length;

    ( void ) pContainer;

    switch( dataType )
    {
        case IOT_SERIALIZER_CONTAINER_MAP:
        case IOT_SERIALIZER_CONTAINER_ARRAY:
            length = _jsonEmptyContainerLength;
            break;

        case IOT_SERIALIZER_SCALAR_TEXT_STRING:
            length = _jsonStringLength( pScalarData->value.u.string.length );
            break;

        case IOT_SERIALIZER_SCALAR_BYTE_STRING:
            length = _jsonByteStringLength( pScalarData->value.u.string.length );
            break;

        case IOT_SERIALIZER_SCALAR_SIGNED_INT:
            length = _jsonIntegerLength( pScalarData->value.u.signedInt );
            break;

        case IOT_SERIALIZER_SCALAR_BOOL:
            length = _jsonBoolLength( pScalarData->value.u.booleanValue );
            break;

        case IOT_SERIALIZER_SCALAR_NULL:
            length = _JSON_NULL_VALUE_LENGTH;
            break;

        default:
            length = 0;
            break;
    }

    return length;
}

/*-----------------------------------------------------------*/

static size_t _getKeyValueLength( _jsonContainer_t * pContainer,
                                  size_t keyLength,
                                  IotSerializerDataType_t valueType,
                                  IotSerializerScalarData_t * pScalarValue )
{
    size_t keyValueLength = 0, valueLength;

    valueLength = _getSerializedLength( pContainer, valueType, pScalarValue );
    keyValueLength = _jsonKeyValuePairLength( keyLength, valueLength );

    if( !pContainer->isEmpty )
    {
        keyValueLength += 1;
    }

    return keyValueLength;
}

/*-----------------------------------------------------------*/

static size_t _getValueLength( _jsonContainer_t * pContainer,
                               IotSerializerDataType_t valueType,
                               IotSerializerScalarData_t * pScalarValue )
{
    size_t length = _getSerializedLength( pContainer, valueType, pScalarValue );

    if( !pContainer->isEmpty )
    {
        length += 1;
    }

    return length;
}

/*-----------------------------------------------------------*/

static void _appendTextString( _jsonContainer_t * pContainer,
                               const char * pStr,
                               size_t strLength )
{
    pContainer->pBuffer[ pContainer->offset++ ] = _JSON_STRING_WRAPPER;
    memcpy( ( pContainer->pBuffer + pContainer->offset ), pStr, strLength );
    pContainer->offset += strLength;
    pContainer->pBuffer[ pContainer->offset++ ] = _JSON_STRING_WRAPPER;
}

/*-----------------------------------------------------------*/

static void _appendByteString( _jsonContainer_t * pContainer,
                               uint8_t * pByteString,
                               size_t stringLength )
{
    size_t encodedStrLength = 0;

    pContainer->pBuffer[ pContainer->offset++ ] = _JSON_STRING_WRAPPER;
    mbedtls_base64_encode(
        ( unsigned char * ) ( pContainer->pBuffer + pContainer->offset ),
        pContainer->remainingLength,
        &encodedStrLength,
        ( const unsigned char * ) pByteString,
        stringLength );
    pContainer->offset += encodedStrLength;
    pContainer->pBuffer[ pContainer->offset++ ] = _JSON_STRING_WRAPPER;
}

/*-----------------------------------------------------------*/

static void _appendInteger( _jsonContainer_t * pContainer,
                            int64_t signedInteger )
{
    size_t len = snprintf( ( char * ) _jsonContainerPointer( pContainer ), pContainer->remainingLength, "%lld", signedInteger );

    pContainer->offset += len;
}

/*-----------------------------------------------------------*/

static void _appendBoolean( _jsonContainer_t * pContainer,
                            bool value )
{
    if( value == true )
    {
        memcpy( _jsonContainerPointer( pContainer ), _JSON_BOOL_TRUE, _JSON_BOOL_TRUE_LENGTH );
        pContainer->offset += _JSON_BOOL_TRUE_LENGTH;
    }
    else
    {
        memcpy( _jsonContainerPointer( pContainer ), _JSON_BOOL_FALSE, _JSON_BOOL_FALSE_LENGTH );
        pContainer->offset += _JSON_BOOL_FALSE_LENGTH;
    }
}

/*-----------------------------------------------------------*/

static void _appendData( _jsonContainer_t * pContainer,
                         IotSerializerDataType_t dataType,
                         IotSerializerScalarData_t * pScalarData )
{
    size_t stringLength;
    char * pString;

    switch( dataType )
    {
        case IOT_SERIALIZER_CONTAINER_MAP:
            pContainer->pBuffer[ pContainer->offset++ ] = _JSON_OBJECT_START_CHAR;
            break;

        case IOT_SERIALIZER_CONTAINER_ARRAY:
            pContainer->pBuffer[ pContainer->offset++ ] = _JSON_ARRAY_START_CHAR;
            break;

        case IOT_SERIALIZER_SCALAR_TEXT_STRING:
            pString = ( char * ) ( pScalarData->value.u.string.pString );
            stringLength = ( pScalarData->value.u.string.length == 0 ) ? strlen( pString ) : pScalarData->value.u.string.length;
            _appendTextString( pContainer, pString, stringLength );
            break;

        case IOT_SERIALIZER_SCALAR_BYTE_STRING:
            _appendByteString( pContainer, pScalarData->value.u.string.pString, pScalarData->value.u.string.length );
            break;

        case IOT_SERIALIZER_SCALAR_SIGNED_INT:
            _appendInteger( pContainer, pScalarData->value.u.signedInt );
            break;

        case IOT_SERIALIZER_SCALAR_BOOL:
            _appendBoolean( pContainer, pScalarData->value.u.booleanValue );
            break;

        case IOT_SERIALIZER_SCALAR_NULL:
            memcpy( _jsonContainerPointer( pContainer ), _JSON_NULL_VALUE, _JSON_NULL_VALUE_LENGTH );
            pContainer->offset += _JSON_NULL_VALUE_LENGTH;
            break;

        default:
            break;
    }
}

/*-----------------------------------------------------------*/

static void _appendJsonValue( _jsonContainer_t * pContainer,
                              IotSerializerDataType_t xType,
                              IotSerializerScalarData_t * pxScalarData )
{
    if( !pContainer->isEmpty )
    {
        pContainer->pBuffer[ pContainer->offset++ ] = _JSON_VALUE_SEPARATOR;
    }

    _appendData( pContainer, xType, pxScalarData );
}

/*-----------------------------------------------------------*/

static void _appendJsonKeyValuePair( _jsonContainer_t * pContainer,
                                     const char * pcKey,
                                     size_t keyLength,
                                     IotSerializerDataType_t xValType,
                                     IotSerializerScalarData_t * pxScalarValue )
{
    if( !pContainer->isEmpty )
    {
        pContainer->pBuffer[ pContainer->offset++ ] = _JSON_VALUE_SEPARATOR;
    }

    _appendTextString( pContainer, pcKey, keyLength );
    pContainer->pBuffer[ pContainer->offset++ ] = _JSON_KEY_VALUE_PAIR_SEPARATOR;
    _appendData( pContainer, xValType, pxScalarValue );
}

/*-----------------------------------------------------------*/

static IotSerializerError_t _init( IotSerializerEncoderObject_t * pEncoderObject,
                                   uint8_t * pDataBuffer,
                                   size_t maxSize )
{
    _jsonContainer_t * pContainer;
    IotSerializerError_t error = IOT_SERIALIZER_SUCCESS;

    /* Create a new JSON container */
    pContainer = pvPortMalloc( sizeof( _jsonContainer_t ) );

    if( pContainer != NULL )
    {
        pContainer->pBuffer = pDataBuffer;
        pContainer->maxLength = ( pDataBuffer != NULL ) ? maxSize : 0;
        pContainer->remainingLength = pContainer->maxLength;
        pContainer->offset = 0;
        pContainer->overflowLength = 0;
        pContainer->isEmpty = true;

        /* Set the outermost container default type as stream */
        pEncoderObject->type = IOT_SERIALIZER_CONTAINER_STREAM;

        /* Store the container pointer within the handle */
        pEncoderObject->pHandle = ( void * ) pContainer;
    }
    else
    {
        error = IOT_SERIALIZER_OUT_OF_MEMORY;
    }

    return error;
}

/*-----------------------------------------------------------*/

static void _destroy( IotSerializerEncoderObject_t * pEncoderObject )
{
    _jsonContainer_t * pContainer;

    pContainer = ( _jsonContainer_t * ) ( pEncoderObject->pHandle );

    vPortFree( pContainer );
    pEncoderObject->pHandle = NULL;
}

/*-----------------------------------------------------------*/

static IotSerializerError_t _openContainer( IotSerializerEncoderObject_t * pEncoderObject,
                                            IotSerializerEncoderObject_t * pNewEncoderObject,
                                            size_t length )
{
    _jsonContainer_t * pContainer;
    size_t serializedLength = 0;
    IotSerializerError_t error = IOT_SERIALIZER_SUCCESS;

    ( void ) length;

    if( _jsonIsValidContainer( pEncoderObject ) &&
        _jsonIsValidContainer( pNewEncoderObject ) )
    {
        pContainer = ( _jsonContainer_t * ) pEncoderObject->pHandle;
        serializedLength = _getValueLength( pContainer, pNewEncoderObject->type, NULL );

        if( pContainer->remainingLength >= serializedLength )
        {
            _appendJsonValue( pContainer, pNewEncoderObject->type, NULL );
            pContainer->remainingLength -= serializedLength;
        }
        else
        {
            pContainer->overflowLength += ( serializedLength - pContainer->remainingLength );
            pContainer->remainingLength = 0;
            error = IOT_SERIALIZER_BUFFER_TOO_SMALL;
        }

        pContainer->isEmpty = true;
        pContainer->containerType = pNewEncoderObject->type;
        pNewEncoderObject->pHandle = ( void * ) pContainer;
    }
    else
    {
        error = IOT_SERIALIZER_INVALID_INPUT;
    }

    return error;
}

/*-----------------------------------------------------------*/

static IotSerializerError_t _openContainerWithKey( IotSerializerEncoderObject_t * pEncoderObject,
                                                   const char * pKey,
                                                   IotSerializerEncoderObject_t * pNewEncoderObject,
                                                   size_t length )
{
    IotSerializerError_t error = IOT_SERIALIZER_SUCCESS;
    _jsonContainer_t * pContainer = NULL;
    size_t serializedLength, keyLength = strlen( pKey );

    ( void ) length;

    if( _jsonIsValidContainer( pEncoderObject ) &&
        _jsonIsValidContainer( pNewEncoderObject ) &&
        ( pEncoderObject->type == IOT_SERIALIZER_CONTAINER_MAP ) )
    {
        pContainer = ( _jsonContainer_t * ) pEncoderObject->pHandle;
        serializedLength = _getKeyValueLength( pContainer, keyLength, pNewEncoderObject->type, NULL );

        if( pContainer->remainingLength >= serializedLength )
        {
            _appendJsonKeyValuePair( pContainer, pKey, keyLength, pNewEncoderObject->type, NULL );
            pContainer->remainingLength -= serializedLength;
        }
        else
        {
            pContainer->overflowLength += ( serializedLength - pContainer->remainingLength );
            pContainer->remainingLength = 0;
            error = IOT_SERIALIZER_BUFFER_TOO_SMALL;
        }

        pContainer->isEmpty = true;
        pContainer->containerType = pNewEncoderObject->type;
        pNewEncoderObject->pHandle = ( void * ) pContainer;
    }
    else
    {
        error = IOT_SERIALIZER_INVALID_INPUT;
    }

    return error;
}

/*-----------------------------------------------------------*/

static IotSerializerError_t _closeContainer( IotSerializerEncoderObject_t * pEncoderObject,
                                             IotSerializerEncoderObject_t * pNewEncoderObject )
{
    IotSerializerError_t error = IOT_SERIALIZER_SUCCESS;
    _jsonContainer_t * pContainer;

    if( _jsonIsValidContainer( pEncoderObject ) &&
        _jsonIsValidContainer( pNewEncoderObject ) )
    {
        pContainer = ( _jsonContainer_t * ) ( pNewEncoderObject->pHandle );

        if( pContainer->pBuffer != NULL )
        {
            _stopContainer( pContainer, pNewEncoderObject->type );
        }
        else
        {
            error = IOT_SERIALIZER_BUFFER_TOO_SMALL;
        }

        pContainer->containerType = pEncoderObject->type;
        pContainer->isEmpty = false;
        pEncoderObject->pHandle = ( void * ) pContainer;
        pNewEncoderObject->pHandle = NULL;
    }
    else
    {
        error = IOT_SERIALIZER_INVALID_INPUT;
    }

    return error;
}

/*-----------------------------------------------------------*/

static IotSerializerError_t _append( IotSerializerEncoderObject_t * pEncoderObject,
                                     IotSerializerScalarData_t scalarData )
{
    IotSerializerError_t error = IOT_SERIALIZER_SUCCESS;
    _jsonContainer_t * pContainer;
    size_t serializedLength;

    if( _jsonIsValidContainer( pEncoderObject ) &&
        ( pEncoderObject->type == IOT_SERIALIZER_CONTAINER_ARRAY ) &&
        _jsonIsValidScalar( &scalarData ) )
    {
        pContainer = ( _jsonContainer_t * ) ( pEncoderObject->pHandle );
        serializedLength = _getValueLength( pContainer, scalarData.type, &scalarData );

        if( pContainer->remainingLength >= serializedLength )
        {
            _appendJsonValue( pContainer, scalarData.type, &scalarData );
            pContainer->remainingLength -= serializedLength;
        }
        else
        {
            pContainer->overflowLength += ( serializedLength - pContainer->remainingLength );
            pContainer->remainingLength = 0;
            error = IOT_SERIALIZER_BUFFER_TOO_SMALL;
        }

        pContainer->isEmpty = false;
    }
    else
    {
        error = IOT_SERIALIZER_INVALID_INPUT;
    }

    return error;
}

/*-----------------------------------------------------------*/

static IotSerializerError_t _appendKeyValue( IotSerializerEncoderObject_t * pEncoderObject,
                                             const char * pKey,
                                             IotSerializerScalarData_t scalarData )
{
    IotSerializerError_t error = IOT_SERIALIZER_SUCCESS;
    _jsonContainer_t * pContainer;
    size_t serializedLength, keyLength = strlen( pKey );


    if( _jsonIsValidContainer( pEncoderObject ) &&
        ( pEncoderObject->type == IOT_SERIALIZER_CONTAINER_MAP ) &&
        _jsonIsValidScalar( &scalarData ) )
    {
        pContainer = ( _jsonContainer_t * ) ( pEncoderObject->pHandle );
        serializedLength = _getKeyValueLength( pContainer, keyLength, scalarData.type, &scalarData );

        if( pContainer->remainingLength >= serializedLength )
        {
            _appendJsonKeyValuePair( pContainer, pKey, keyLength, scalarData.type, &scalarData );
            pContainer->remainingLength -= serializedLength;
        }
        else
        {
            pContainer->overflowLength += ( serializedLength - pContainer->remainingLength );
            pContainer->remainingLength = 0;
            error = IOT_SERIALIZER_BUFFER_TOO_SMALL;
        }

        pContainer->isEmpty = false;
    }
    else
    {
        error = IOT_SERIALIZER_INVALID_INPUT;
    }

    return error;
}

/*-----------------------------------------------------------*/

static size_t _getEncodedSize( IotSerializerEncoderObject_t * pEncoderObject,
                               uint8_t * pDataBuffer )
{
    size_t encodedSize = 0;
    _jsonContainer_t * pContainer = NULL;

    if( _jsonIsValidContainer( pEncoderObject ) )
    {
        pContainer = ( _jsonContainer_t * ) ( pEncoderObject->pHandle );

        if( ( pContainer != NULL ) && ( pDataBuffer == pContainer->pBuffer ) )
        {
            encodedSize = pContainer->offset;
        }
    }

    return encodedSize;
}

/*-----------------------------------------------------------*/

static size_t _getExtraBufferSizeNeeded( IotSerializerEncoderObject_t * pEncoderObject )
{
    size_t extraSizeNeeded = 0;
    _jsonContainer_t * pContainer = NULL;

    if( _jsonIsValidContainer( pEncoderObject ) )
    {
        pContainer = ( _jsonContainer_t * ) ( pEncoderObject->pHandle );

        if( pContainer != NULL )
        {
            extraSizeNeeded = pContainer->overflowLength;
        }
    }

    return extraSizeNeeded;
}
