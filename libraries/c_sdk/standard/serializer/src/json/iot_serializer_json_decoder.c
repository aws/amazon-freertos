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
 *
 * @file iot_serializer_json_decoder.c
 * @brief Implements APIs to parse and decode data from JSON format. Supports JSON primitives such as
 * Numbers, Strings, boolean and null types, container types such as arrays and maps.
 * A special type called binary string is also supported as a value type. By default
 * binary strings are base-64 decoded.
 * The file implements decoder interface in aws_iot_serialize.h.
 */

#include <string.h>

#include "iot_serializer.h"
#include "mbedtls/base64.h"

#define _MINIMUM_CONTAINER_LENGTH    ( 2 )
#define _JSON_INT64_MAX_LENGTH       ( 20 )


#define _START_CHAR_ARRAY            '['
#define _STOP_CHAR_ARRAY             ']'
#define _START_CHAR_MAP              '{'
#define _STOP_CHAR_MAP               '}'
#define _STRING_QUOTE                '"'
#define _QUOTE_ESCAPE                '\\'

#define _isValidContainer( decoder )                          \
    ( ( decoder ) &&                                          \
      ( decoder )->type >= IOT_SERIALIZER_CONTAINER_STREAM && \
      ( decoder )->type <= IOT_SERIALIZER_CONTAINER_MAP )

#define _castDecoderObjectToJsonContainer( pDecoderObject )    ( ( pDecoderObject )->pHandle )

#define _castDecoderIteratorToJsonContainer( iterator )        ( ( ( IotSerializerDecoderObject_t * ) iterator )->u.pHandle )

static IotSerializerError_t _init( IotSerializerDecoderObject_t * pDecoderObject,
                                   const uint8_t * pDataBuffer,
                                   size_t maxSize );

static IotSerializerError_t _find( IotSerializerDecoderObject_t * pDecoderObject,
                                   const char * pKey,
                                   IotSerializerDecoderObject_t * pValueObject );

static IotSerializerError_t _get( IotSerializerDecoderIterator_t iterator,
                                  IotSerializerDecoderObject_t * pValueObject );

static IotSerializerError_t _stepIn( IotSerializerDecoderObject_t * pDecoderObject,
                                     IotSerializerDecoderIterator_t * pIterator );

static bool _isEndOfContainer( IotSerializerDecoderIterator_t iterator );

static IotSerializerError_t _next( IotSerializerDecoderIterator_t iterator );

static IotSerializerError_t _stepOut( IotSerializerDecoderIterator_t iterator,
                                      IotSerializerDecoderObject_t * pDecoderObject );

static void _destroy( IotSerializerDecoderObject_t * pDecoderObject );

static void parseTextString( const char * pBuffer,
                             const size_t bufLength,
                             size_t * pOffset );

IotSerializerDecodeInterface_t _IotSerializerJsonDecoder =
{
    .init             = _init,
    .find             = _find,
    .stepIn           = _stepIn,
    .isEndOfContainer = _isEndOfContainer,
    .get              = _get,
    .next             = _next,
    .stepOut          = _stepOut,
    .destroy          = _destroy
};

typedef struct _jsonContainer
{
    const char * pStart;
    size_t length;
} _jsonContainer_t;

/*-----------------------------------------------------------*/

static IotSerializerDataType_t _getTokenType( const char * pBuffer,
                                              size_t offset )
{
    IotSerializerDataType_t tokenType = IOT_SERIALIZER_UNDEFINED;

    switch( pBuffer[ offset ] )
    {
        case _START_CHAR_MAP:
            tokenType = IOT_SERIALIZER_CONTAINER_MAP;
            break;

        case _START_CHAR_ARRAY:
            tokenType = IOT_SERIALIZER_CONTAINER_ARRAY;
            break;

        case _STRING_QUOTE:
            tokenType = IOT_SERIALIZER_SCALAR_TEXT_STRING;
            break;

        case '-':
        case '0':
        case '1':
        case '2':
        case '3':
        case '4':
        case '5':
        case '6':
        case '7':
        case '8':
        case '9':
            tokenType = IOT_SERIALIZER_SCALAR_SIGNED_INT;
            break;

        case 't':
        case 'f':
            tokenType = IOT_SERIALIZER_SCALAR_BOOL;
            break;

        case 'n':
            tokenType = IOT_SERIALIZER_SCALAR_NULL;
            break;

        default:
            tokenType = IOT_SERIALIZER_UNDEFINED;
            break;
    }

    return tokenType;
}

/*-----------------------------------------------------------*/

static _jsonContainer_t * _createContainer( const char * pBuffer,
                                            const size_t length )
{
    _jsonContainer_t * pContainer = pvPortMalloc( sizeof( _jsonContainer_t ) );

    if( pContainer != NULL )
    {
        pContainer->pStart = pBuffer;
        pContainer->length = length;
    }

    return pContainer;
}

/*-----------------------------------------------------------*/

static void _skipWhiteSpacesAndDelimeters( const char * pBuffer,
                                           const size_t bufLength,
                                           size_t * pOffset )
{
    size_t offset = *pOffset;

    for( ; offset < bufLength; offset++ )
    {
        if( ( pBuffer[ offset ] != ' ' ) &&
            ( pBuffer[ offset ] != '\r' ) &&
            ( pBuffer[ offset ] != '\n' ) &&
            ( pBuffer[ offset ] != '\t' ) &&
            ( pBuffer[ offset ] != ':' ) &&
            ( pBuffer[ offset ] != ',' ) )
        {
            break;
        }
    }

    *pOffset = offset;
}

/*-----------------------------------------------------------*/
static void parseContainer( const char * pBuffer,
                            const size_t bufLength,
                            size_t * pOffset,
                            const char containerStopChar )
{
    size_t offset = *pOffset;

    for( ; offset < bufLength && pBuffer[ offset ] != containerStopChar; offset++ )
    {
        switch( pBuffer[ offset ] )
        {
            case _START_CHAR_MAP:
                offset++;
                parseContainer( pBuffer, bufLength, &offset, _STOP_CHAR_MAP );
                break;

            case _START_CHAR_ARRAY:
                offset++;
                parseContainer( pBuffer, bufLength, &offset, _STOP_CHAR_ARRAY );
                break;

            case _STRING_QUOTE:
                offset++;
                parseTextString( pBuffer, bufLength, &offset );
                break;
        }
    }

    *pOffset = offset;
}

/*-----------------------------------------------------------*/

static int64_t parseNumber( const char * pBuffer,
                            const size_t bufLength,
                            size_t * pOffset )
{
    size_t offset = *pOffset;
    int64_t val = atoi( pBuffer + offset );

    /*Skip -, + or first digit */
    offset++;

    for( ; offset < bufLength; offset++ )
    {
        if( ( pBuffer[ offset ] < '0' ) ||
            ( pBuffer[ offset ] > '9' ) )
        {
            break;
        }
    }

    *pOffset = offset;

    return val;
}

/*-----------------------------------------------------------*/

static void parseTextString( const char * pBuffer,
                             const size_t bufLength,
                             size_t * pOffset )

{
    size_t offset = *pOffset;

    for( ; offset < bufLength && pBuffer[ offset ] != _STRING_QUOTE; offset++ )
    {
        /* Backslash: Quoted symbol expected */
        if( ( offset < bufLength - 1 ) &&
            ( pBuffer[ offset ] == _QUOTE_ESCAPE ) &&
            ( pBuffer[ offset + 1 ] == _STRING_QUOTE ) )
        {
            offset++;
        }
    }

    *pOffset = offset;
}

/*-----------------------------------------------------------*/

static IotSerializerError_t parseTokenValue( const char * pBuffer,
                                             const size_t bufLength,
                                             size_t * pOffset,
                                             IotSerializerDataType_t tokenType,
                                             IotSerializerDecoderObject_t * pValue )
{
    IotSerializerError_t error = IOT_SERIALIZER_SUCCESS;

    switch( tokenType )
    {
        case IOT_SERIALIZER_CONTAINER_MAP:
           {
               size_t start = ++( *pOffset ), length;
               parseContainer( pBuffer, bufLength, pOffset, _STOP_CHAR_MAP );
               length = ( *pOffset - start ) + 1; /* Include MAP stop char in the length */

               if( pValue )
               {
                   pValue->type = tokenType;
                   pValue->u.pHandle = _createContainer( ( pBuffer + start ), length );
               }

               ( *pOffset )++; /* Skip past the token */
               break;
           }

        case IOT_SERIALIZER_CONTAINER_ARRAY:
           {
               size_t start = ++( *pOffset ), length;
               parseContainer( pBuffer, bufLength, pOffset, _STOP_CHAR_ARRAY );
               length = ( *pOffset - start ) + 1; /* Include array stop char ']' in the length */

               if( pValue )
               {
                   pValue->type = tokenType;
                   pValue->u.pHandle = _createContainer( ( pBuffer + start ), length );
               }

               ( *pOffset )++; /* Skip past the token after parsing */
               break;
           }

        case IOT_SERIALIZER_SCALAR_SIGNED_INT:
           {
               int64_t val = parseNumber( pBuffer, bufLength, pOffset );

               if( pValue )
               {
                   pValue->type = tokenType;
                   pValue->u.value.u.signedInt = val;
               }

               break;
           }

        case IOT_SERIALIZER_SCALAR_BOOL:
           {
               bool val;

               if( pBuffer[ *pOffset ] == 't' )
               {
                   val = true;
                   ( *pOffset ) += 5; /* Skip past the token after parsing  */
               }
               else
               {
                   val = false;
                   ( *pOffset ) += 6; /* Skip past the token after parsing */
               }

               if( pValue != NULL )
               {
                   pValue->type = tokenType;
                   pValue->u.value.u.booleanValue = val;
               }

               break;
           }

        case IOT_SERIALIZER_SCALAR_NULL:
            *pOffset += 5; /* Skip past the token after parsing */

            if( pValue != NULL )
            {
                pValue->type = tokenType;
            }

            break;

        case IOT_SERIALIZER_SCALAR_BYTE_STRING:
        case IOT_SERIALIZER_SCALAR_TEXT_STRING:
           {
               size_t start = ++( *pOffset ), length;
               int decodeRet;
               parseTextString( pBuffer, bufLength, pOffset );
               length = ( *pOffset ) - start; /* Don't include the last quotes of a string */

               if( pValue != NULL )
               {
                   if( pValue->type == IOT_SERIALIZER_SCALAR_BYTE_STRING )
                   {
                       decodeRet = mbedtls_base64_decode( ( unsigned char * ) ( pValue->u.value.u.string.pString ),
                                                          pValue->u.value.u.string.length,
                                                          &( pValue->u.value.u.string.length ),
                                                          ( const unsigned char * ) ( pBuffer + start ), length );

                       switch( decodeRet )
                       {
                           case MBEDTLS_ERR_BASE64_INVALID_CHARACTER:
                               error = IOT_SERIALIZER_INTERNAL_FAILURE;
                               break;

                           case MBEDTLS_ERR_BASE64_BUFFER_TOO_SMALL:
                               error = IOT_SERIALIZER_BUFFER_TOO_SMALL;
                               break;

                           default:
                               break;
                       }
                   }
                   else
                   {
                       pValue->type = tokenType;
                       pValue->u.value.u.string.pString = ( uint8_t * ) ( pBuffer + start );
                       pValue->u.value.u.string.length = length;
                   }
               }

               ( *pOffset )++; /* Skip past the token after parsing */
               break;
           }

        default:
            error = IOT_SERIALIZER_UNDEFINED_TYPE;
            break;
    }

    return error;
}

/*-----------------------------------------------------------*/

static IotSerializerError_t _findKeyValue( _jsonContainer_t * pObject,
                                           const char * pKey,
                                           size_t keyLength,
                                           IotSerializerDecoderObject_t * pValue )
{
    size_t offset = 0;
    IotSerializerDataType_t tokenType;
    IotSerializerDecoderObject_t key = { .type = IOT_SERIALIZER_SCALAR_TEXT_STRING };
    IotSerializerError_t ret = IOT_SERIALIZER_NOT_FOUND;

    ( void ) keyLength;

    bool isValue = false;
    bool isKeyFound = false;

    _skipWhiteSpacesAndDelimeters( pObject->pStart, pObject->length, &offset );

    for( ; ( offset < pObject->length ) && ( pObject->pStart[ offset ] != '}' ) && ( ret == IOT_SERIALIZER_NOT_FOUND ); )
    {
        tokenType = _getTokenType( pObject->pStart, offset );

        switch( tokenType )
        {
            case IOT_SERIALIZER_CONTAINER_MAP:
            case IOT_SERIALIZER_CONTAINER_ARRAY:
            case IOT_SERIALIZER_SCALAR_SIGNED_INT:
            case IOT_SERIALIZER_SCALAR_BOOL:
            case IOT_SERIALIZER_SCALAR_NULL:
            case IOT_SERIALIZER_SCALAR_BYTE_STRING:

                /* JSON key can only be text string, so return error */
                if( isValue )
                {
                    /* If key parsed successfully, parse and store the value, else skip past the JSON value */
                    if( isKeyFound )
                    {
                        ret = parseTokenValue( pObject->pStart, pObject->length, &offset, tokenType, pValue );
                    }
                    else
                    {
                        ( void ) parseTokenValue( pObject->pStart, pObject->length, &offset, tokenType, NULL );

                        /* Skip any white space characters or delimeters after the token */
                        _skipWhiteSpacesAndDelimeters( pObject->pStart, pObject->length, &offset );

                        /* Set isValue to false as we are expecting a key now */
                        isValue = false;
                    }
                }
                else
                {
                    ret = IOT_SERIALIZER_INTERNAL_FAILURE;
                }

                break;

            case IOT_SERIALIZER_SCALAR_TEXT_STRING:

                if( !isValue )
                {
                    /*
                     * Parser is expecting a key. Parse the JSON key and compare with the
                     * input key provided.
                     */
                    ( void ) parseTokenValue( pObject->pStart, pObject->length, &offset, tokenType, &key );

                    if( strncmp( pKey, ( const char * ) key.u.value.u.string.pString, key.u.value.u.string.length ) == 0 )
                    {
                        isKeyFound = true;
                    }

                    /* Set the isValue to true as the parser expects a value now */
                    isValue = true;
                }
                else
                {
                    /*
                     * Parser is expecting a value. If key already found, parse and store the value.
                     * Else skip the value.
                     */
                    if( isKeyFound )
                    {
                        /* If key already found, store the string as value, and return success */
                        ret = parseTokenValue( pObject->pStart, pObject->length, &offset, tokenType, pValue );
                    }
                    else
                    {
                        /* Skip the value */
                        ( void ) parseTokenValue( pObject->pStart, pObject->length, &offset, tokenType, NULL );
                    }

                    /* Set the isValue to true as the parser expects a key now */
                    isValue = false;
                }

                /* Skip any white space characters or delimeters */
                _skipWhiteSpacesAndDelimeters( pObject->pStart, pObject->length, &offset );
                break;

            default:
                /* Any other character other than start of a token is an invalid character */
                ret = IOT_SERIALIZER_INTERNAL_FAILURE;
        }
    }

    /*
     * If offset went past the container during parsing, return error.
     */
    if( offset >= pObject->length )
    {
        ret = IOT_SERIALIZER_INVALID_INPUT;
    }

    return ret;
}


/*-----------------------------------------------------------*/

static IotSerializerError_t _init( IotSerializerDecoderObject_t * pDecoderObject,
                                   const uint8_t * pDataBuffer,
                                   size_t maxSize )
{
    IotSerializerDataType_t tokenType;
    _jsonContainer_t * pContainer;
    const char * pStart = ( const char * ) pDataBuffer;
    size_t length = strlen( pStart );
    IotSerializerError_t error = IOT_SERIALIZER_SUCCESS;

    length = ( length < maxSize ) ? length : maxSize;

    tokenType = _getTokenType( pStart, 0 );

    if( ( ( tokenType != IOT_SERIALIZER_CONTAINER_MAP ) &&
          ( tokenType != IOT_SERIALIZER_CONTAINER_ARRAY ) ) ||
        ( length < _MINIMUM_CONTAINER_LENGTH ) )
    {
        error = IOT_SERIALIZER_INVALID_INPUT;
    }

    if( error == IOT_SERIALIZER_SUCCESS )
    {
        pContainer = _createContainer( pStart + 1, length - 1 );

        if( pContainer != NULL )
        {
            pDecoderObject->type = tokenType;
            pDecoderObject->u.pHandle = ( void * ) pContainer;
        }
        else
        {
            error = IOT_SERIALIZER_OUT_OF_MEMORY;
        }
    }

    return error;
}


/*-----------------------------------------------------------*/

static IotSerializerError_t _find( IotSerializerDecoderObject_t * pDecoderObject,
                                   const char * pKey,
                                   IotSerializerDecoderObject_t * pValueObject )
{
    _jsonContainer_t * pContainer;
    IotSerializerError_t error = IOT_SERIALIZER_SUCCESS;

    if( pDecoderObject->type == IOT_SERIALIZER_CONTAINER_MAP )
    {
        pContainer = ( _jsonContainer_t * ) pDecoderObject->u.pHandle;
        error = _findKeyValue(
            pContainer,
            pKey,
            strlen( pKey ),
            pValueObject );
    }
    else
    {
        error = IOT_SERIALIZER_INVALID_INPUT;
    }

    return error;
}

/*-----------------------------------------------------------*/

static IotSerializerError_t _stepIn( IotSerializerDecoderObject_t * pDecoderObject,
                                     IotSerializerDecoderIterator_t * pIterator )
{
    IotSerializerDecoderObject_t * pNewObject;
    _jsonContainer_t * pContainer, * pNewContainer;
    size_t offset = 0;
    IotSerializerError_t error = IOT_SERIALIZER_SUCCESS;

    if( _isValidContainer( pDecoderObject ) )
    {
        pContainer = pDecoderObject->u.pHandle;

        _skipWhiteSpacesAndDelimeters( pContainer->pStart, pContainer->length, &offset );

        if( offset >= pContainer->length )
        {
            error = IOT_SERIALIZER_INTERNAL_FAILURE;
        }

        if( error == IOT_SERIALIZER_SUCCESS )
        {
            pNewContainer = _createContainer( ( pContainer->pStart + offset ), pContainer->length - offset );

            if( pNewContainer != NULL )
            {
                pNewObject = pvPortMalloc( sizeof( IotSerializerDecoderObject_t ) );

                if( pNewObject != NULL )
                {
                    pNewObject->type = pDecoderObject->type;
                    pNewObject->u.pHandle = pNewContainer;
                    *pIterator = ( IotSerializerDecoderIterator_t ) pNewObject;
                }
                else
                {
                    vPortFree( pNewContainer );
                    error = IOT_SERIALIZER_OUT_OF_MEMORY;
                }
            }
            else
            {
                error = IOT_SERIALIZER_OUT_OF_MEMORY;
            }
        }
    }
    else
    {
        error = IOT_SERIALIZER_INVALID_INPUT;
    }

    return error;
}

/*-----------------------------------------------------------*/

static bool _isEOF( const char * pBuffer,
                    IotSerializerDataType_t containerType )
{
    bool isEOF;

    switch( containerType )
    {
        case IOT_SERIALIZER_CONTAINER_MAP:
            isEOF = ( pBuffer[ 0 ] == '}' );
            break;

        case IOT_SERIALIZER_CONTAINER_ARRAY:
            isEOF = ( pBuffer[ 0 ] == ']' );
            break;

        default:
            isEOF = false;
            break;
    }

    return isEOF;
}

/*-----------------------------------------------------------*/

static bool _isEndOfContainer( IotSerializerDecoderIterator_t iterator )
{
    IotSerializerDecoderObject_t * pObject = ( IotSerializerDecoderObject_t * ) iterator;
    _jsonContainer_t * pContainer;
    bool ret = false;

    if( _isValidContainer( pObject ) )
    {
        pContainer = pObject->u.pHandle;
        ret = _isEOF( pContainer->pStart, pObject->type );
    }

    return ret;
}

/*-----------------------------------------------------------*/

static IotSerializerError_t _get( IotSerializerDecoderIterator_t iterator,
                                  IotSerializerDecoderObject_t * pValueObject )
{
    IotSerializerDecoderObject_t * pDecoder = ( IotSerializerDecoderObject_t * ) iterator;
    _jsonContainer_t * pContainer;
    IotSerializerDataType_t type;
    size_t offset = 0;
    IotSerializerError_t error = IOT_SERIALIZER_SUCCESS;

    if( _isValidContainer( pDecoder ) )
    {
        pContainer = _castDecoderIteratorToJsonContainer( iterator );
        type = _getTokenType( pContainer->pStart, offset );

        if( type != IOT_SERIALIZER_UNDEFINED )
        {
            parseTokenValue( pContainer->pStart, pContainer->length, &offset, type, pValueObject );

            if( offset >= pContainer->length )
            {
                error = IOT_SERIALIZER_BUFFER_TOO_SMALL;
            }
        }
        else
        {
            error = IOT_SERIALIZER_INTERNAL_FAILURE;
        }
    }
    else
    {
        error = IOT_SERIALIZER_INVALID_INPUT;
    }

    return error;
}


/*-----------------------------------------------------------*/

static IotSerializerError_t _next( IotSerializerDecoderIterator_t iterator )
{
    IotSerializerDecoderObject_t * pObject = ( IotSerializerDecoderObject_t * ) iterator;
    _jsonContainer_t * pContainer;
    IotSerializerDataType_t type;
    size_t offset = 0;
    IotSerializerError_t error = IOT_SERIALIZER_SUCCESS;

    if( _isValidContainer( pObject ) )
    {
        pContainer = pObject->u.pHandle;
        type = _getTokenType( pContainer->pStart, offset );
        parseTokenValue( pContainer->pStart, pContainer->length, &offset, type, NULL );
        _skipWhiteSpacesAndDelimeters( pContainer->pStart, pContainer->length, &offset );

        if( offset < pContainer->length )
        {
            pContainer->pStart += offset;
        }
        else
        {
            error = IOT_SERIALIZER_BUFFER_TOO_SMALL;
        }
    }
    else
    {
        error = IOT_SERIALIZER_INVALID_INPUT;
    }

    return error;
}

/*-----------------------------------------------------------*/

static IotSerializerError_t _stepOut( IotSerializerDecoderIterator_t iterator,
                                      IotSerializerDecoderObject_t * pDecoderObject )
{
    IotSerializerDecoderObject_t * pIterObject = ( IotSerializerDecoderObject_t * ) iterator;
    _jsonContainer_t * pContainer, * pIterContainer;
    IotSerializerError_t error = IOT_SERIALIZER_SUCCESS;

    if( _isValidContainer( pIterObject ) && _isValidContainer( pDecoderObject ) )
    {
        pContainer = pDecoderObject->u.pHandle;
        pIterContainer = pIterObject->u.pHandle;

        if( _isEOF( pIterContainer->pStart, pIterObject->type ) )
        {
            pContainer->pStart = ( pIterContainer->pStart + 1 );
            vPortFree( pIterContainer );
            vPortFree( pIterObject );
        }
        else
        {
            error = IOT_SERIALIZER_INTERNAL_FAILURE;
        }
    }
    else
    {
        error = IOT_SERIALIZER_INVALID_INPUT;
    }

    return error;
}

/*-----------------------------------------------------------*/

static void _destroy( IotSerializerDecoderObject_t * pDecoderObject )
{
    if( _isValidContainer( pDecoderObject ) )
    {
        if( pDecoderObject->u.pHandle != NULL )
        {
            vPortFree( pDecoderObject->u.pHandle );
            pDecoderObject->u.pHandle = NULL;
        }
    }
}
