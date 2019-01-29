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
 *
 *
 */

/**
 *
 * @file aws_iot_serializer_json_decoder.c
 * @brief Implements APIs to parse and decode data from JSON format. Supports JSON primitives such as
 * Numbers, Strings, boolean and null types, container types such as arrays and maps.
 * A special type called binary string is also supported as a value type. By default
 * binary strings are base-64 decoded.
 * The file implements decoder interface in aws_iot_serialize.h.
 */

#include <string.h>

#include "aws_iot_serializer.h"
#include "mbedtls/base64.h"

#define _MINIMUM_CONTAINER_LENGTH    ( 2 )
#define _JSON_INT64_MAX_LENGTH       ( 20 )


#define _STOP_CHAR_ARRAY             ']'
#define _STOP_CHAR_MAP               '}'

#define _isValidContainer( decoder )                              \
    ( ( decoder ) &&                                              \
      ( decoder )->type >= AWS_IOT_SERIALIZER_CONTAINER_STREAM && \
      ( decoder )->type <= AWS_IOT_SERIALIZER_CONTAINER_MAP )

#define _castDecoderObjectToJsonContainer( pDecoderObject )    ( ( pDecoderObject )->pHandle )

#define _castDecoderIteratorToJsonContainer( iterator )        ( ( ( AwsIotSerializerDecoderObject_t * ) iterator )->pHandle )

static AwsIotSerializerError_t _init( AwsIotSerializerDecoderObject_t * pDecoderObject,
                                      const uint8_t * pDataBuffer,
                                      size_t maxSize );

static AwsIotSerializerError_t _find( AwsIotSerializerDecoderObject_t * pDecoderObject,
                                      const char * pKey,
                                      AwsIotSerializerDecoderObject_t * pValueObject );

static AwsIotSerializerError_t _get( AwsIotSerializerDecoderIterator_t iterator,
                                     AwsIotSerializerDecoderObject_t * pValueObject );

static AwsIotSerializerError_t _stepIn( AwsIotSerializerDecoderObject_t * pDecoderObject,
                                        AwsIotSerializerDecoderIterator_t * pIterator );

static bool _isEndOfContainer( AwsIotSerializerDecoderIterator_t iterator );

static AwsIotSerializerError_t _next( AwsIotSerializerDecoderIterator_t iterator );

static AwsIotSerializerError_t _stepOut( AwsIotSerializerDecoderIterator_t iterator,
                                         AwsIotSerializerDecoderObject_t * pDecoderObject );

static void _destroy( AwsIotSerializerDecoderObject_t * pDecoderObject );

AwsIotSerializerDecodeInterface_t _AwsIotSerializerJsonDecoder =
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

static AwsIotSerializerDataType_t _getTokenType( const char * pBuffer,
                                                 size_t offset )
{
    AwsIotSerializerDataType_t tokenType = AWS_IOT_SERIALIZER_UNDEFINED;

    switch( pBuffer[ offset ] )
    {
        case '{':
            tokenType = AWS_IOT_SERIALIZER_CONTAINER_MAP;
            break;

        case '[':
            tokenType = AWS_IOT_SERIALIZER_CONTAINER_ARRAY;
            break;

        case '\"':
            tokenType = AWS_IOT_SERIALIZER_SCALAR_TEXT_STRING;
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
            tokenType = AWS_IOT_SERIALIZER_SCALAR_SIGNED_INT;
            break;

        case 't':
        case 'f':
            tokenType = AWS_IOT_SERIALIZER_SCALAR_BOOL;
            break;

        case 'n':
            tokenType = AWS_IOT_SERIALIZER_SCALAR_NULL;
            break;

        default:
            tokenType = AWS_IOT_SERIALIZER_UNDEFINED;
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

    for( ; offset < bufLength; offset++ )
    {
        if( pBuffer[ offset ] == containerStopChar )
        {
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

    for( ; offset < bufLength && pBuffer[ offset ] != '\"'; offset++ )
    {
        /* Backslash: Quoted symbol expected */
        if( ( offset < bufLength - 1 ) &&
            ( pBuffer[ offset ] == '\\' ) &&
            ( pBuffer[ offset + 1 ] == '\"' ) )
        {
            offset++;
        }
    }

    *pOffset = offset;
}

/*-----------------------------------------------------------*/

static AwsIotSerializerError_t parseTokenValue( const char * pBuffer,
                                                const size_t bufLength,
                                                size_t * pOffset,
                                                AwsIotSerializerDataType_t tokenType,
                                                AwsIotSerializerDecoderObject_t * pValue )
{
    AwsIotSerializerError_t error = AWS_IOT_SERIALIZER_SUCCESS;

    switch( tokenType )
    {
        case AWS_IOT_SERIALIZER_CONTAINER_MAP:
           {
               size_t start = ++( *pOffset ), length;
               parseContainer( pBuffer, bufLength, pOffset, _STOP_CHAR_MAP );
               length = ( *pOffset - start ) + 1; /* Include MAP stop char in the length */

               if( pValue )
               {
                   pValue->type = tokenType;
                   pValue->pHandle = _createContainer( ( pBuffer + start ), length );
               }

               ( *pOffset )++; /* Skip past the token */
               break;
           }

        case AWS_IOT_SERIALIZER_CONTAINER_ARRAY:
           {
               size_t start = ++( *pOffset ), length;
               parseContainer( pBuffer, bufLength, pOffset, _STOP_CHAR_ARRAY );
               length = ( *pOffset - start ) + 1; /* Include array stop char ']' in the length */

               if( pValue )
               {
                   pValue->type = tokenType;
                   pValue->pHandle = _createContainer( ( pBuffer + start ), length );
               }

               ( *pOffset )++; /* Skip past the token after parsing */
               break;
           }

        case AWS_IOT_SERIALIZER_SCALAR_SIGNED_INT:
           {
               int64_t val = parseNumber( pBuffer, bufLength, pOffset );

               if( pValue )
               {
                   pValue->type = tokenType;
                   pValue->value.signedInt = val;
               }

               break;
           }

        case AWS_IOT_SERIALIZER_SCALAR_BOOL:
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
                   pValue->value.booleanValue = val;
               }

               break;
           }

        case AWS_IOT_SERIALIZER_SCALAR_NULL:
            *pOffset += 5; /* Skip past the token after parsing */

            if( pValue != NULL )
            {
                pValue->type = tokenType;
            }

            break;

        case AWS_IOT_SERIALIZER_SCALAR_BYTE_STRING:
        case AWS_IOT_SERIALIZER_SCALAR_TEXT_STRING:
           {
               size_t start = ++( *pOffset ), length;
               int decodeRet;
               parseTextString( pBuffer, bufLength, pOffset );
               length = ( *pOffset ) - start; /* Don't include the last quotes of a string */

               if( pValue != NULL )
               {
                   if( pValue->type == AWS_IOT_SERIALIZER_SCALAR_BYTE_STRING )
                   {
                       decodeRet = mbedtls_base64_decode( ( unsigned char * ) ( pValue->value.pString ), pValue->value.stringLength,
                                                          &( pValue->value.stringLength ),
                                                          ( const unsigned char * ) ( pBuffer + start ), length );

                       switch( decodeRet )
                       {
                           case MBEDTLS_ERR_BASE64_INVALID_CHARACTER:
                               error = AWS_IOT_SERIALIZER_INTERNAL_FAILURE;
                               break;

                           case MBEDTLS_ERR_BASE64_BUFFER_TOO_SMALL:
                               error = AWS_IOT_SERIALIZER_BUFFER_TOO_SMALL;
                               break;

                           default:
                               break;
                       }
                   }
                   else
                   {
                       pValue->type = tokenType;
                       pValue->value.pString = ( uint8_t * ) ( pBuffer + start );
                       pValue->value.stringLength = length;
                   }
               }

               ( *pOffset )++; /* Skip past the token after parsing */
               break;
           }

        default:
            error = AWS_IOT_SERIALIZER_UNDEFINED_TYPE;
            break;
    }

    return error;
}

/*-----------------------------------------------------------*/

static AwsIotSerializerError_t _findKeyValue( _jsonContainer_t * pObject,
                                              const char * pKey,
                                              size_t keyLength,
                                              AwsIotSerializerDecoderObject_t * pValue )
{
    size_t offset = 0;
    AwsIotSerializerDataType_t tokenType;
    AwsIotSerializerDecoderObject_t key = { .type = AWS_IOT_SERIALIZER_SCALAR_TEXT_STRING };
    AwsIotSerializerError_t ret = AWS_IOT_SERIALIZER_NOT_FOUND;

    bool isValue = false;
    bool isKeyFound = false;

    _skipWhiteSpacesAndDelimeters( pObject->pStart, pObject->length, &offset );

    for( ; ( offset < pObject->length ) && ( pObject->pStart[ offset ] != '}' ) && ( ret == AWS_IOT_SERIALIZER_NOT_FOUND ); )
    {
        tokenType = _getTokenType( pObject->pStart, offset );

        switch( tokenType )
        {
            case AWS_IOT_SERIALIZER_CONTAINER_MAP:
            case AWS_IOT_SERIALIZER_CONTAINER_ARRAY:
            case AWS_IOT_SERIALIZER_SCALAR_SIGNED_INT:
            case AWS_IOT_SERIALIZER_SCALAR_BOOL:
            case AWS_IOT_SERIALIZER_SCALAR_NULL:
            case AWS_IOT_SERIALIZER_SCALAR_BYTE_STRING:

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
                    ret = AWS_IOT_SERIALIZER_INTERNAL_FAILURE;
                }

                break;

            case AWS_IOT_SERIALIZER_SCALAR_TEXT_STRING:

                if( !isValue )
                {
                    /*
                     * Parser is expecting a key. Parse the JSON key and compare with the
                     * input key provided.
                     */
                    ( void ) parseTokenValue( pObject->pStart, pObject->length, &offset, tokenType, &key );

                    if( strncmp( pKey, ( const char * ) key.value.pString, key.value.stringLength ) == 0 )
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
                ret = AWS_IOT_SERIALIZER_INTERNAL_FAILURE;
        }
    }

    /*
     * If offset went past the container during parsing, return error.
     */
    if( offset >= pObject->length )
    {
        ret = AWS_IOT_SERIALIZER_INVALID_INPUT;
    }

    return ret;
}


/*-----------------------------------------------------------*/

static AwsIotSerializerError_t _init( AwsIotSerializerDecoderObject_t * pDecoderObject,
                                      const uint8_t * pDataBuffer,
                                      size_t maxSize )
{
    AwsIotSerializerDataType_t tokenType;
    _jsonContainer_t * pContainer;
    const char * pStart = ( const char * ) pDataBuffer;
    size_t length = strlen( pStart );
    AwsIotSerializerError_t error = AWS_IOT_SERIALIZER_SUCCESS;

    length = ( length < maxSize ) ? length : maxSize;

    tokenType = _getTokenType( pStart, 0 );

    if( ( ( tokenType != AWS_IOT_SERIALIZER_CONTAINER_MAP ) &&
          ( tokenType != AWS_IOT_SERIALIZER_CONTAINER_ARRAY ) ) ||
        ( length < _MINIMUM_CONTAINER_LENGTH ) )
    {
        error = AWS_IOT_SERIALIZER_INVALID_INPUT;
    }

    if( error == AWS_IOT_SERIALIZER_SUCCESS )
    {

        pContainer = _createContainer( pStart + 1, length - 1 );

        if( pContainer != NULL )
        {
            pDecoderObject->type = tokenType;
            pDecoderObject->pHandle = ( void * ) pContainer;
        }
        else
        {
            error = AWS_IOT_SERIALIZER_OUT_OF_MEMORY;
        }
    }

    return error;
}


/*-----------------------------------------------------------*/

static AwsIotSerializerError_t _find( AwsIotSerializerDecoderObject_t * pDecoderObject,
                                      const char * pKey,
                                      AwsIotSerializerDecoderObject_t * pValueObject )
{
    _jsonContainer_t * pContainer;
    AwsIotSerializerError_t error = AWS_IOT_SERIALIZER_SUCCESS;

    if( pDecoderObject->type == AWS_IOT_SERIALIZER_CONTAINER_MAP )
    {
        pContainer = ( _jsonContainer_t * ) pDecoderObject->pHandle;
        error = _findKeyValue(
                pContainer,
                pKey,
                strlen( pKey ),
                pValueObject );
    }
    else
    {
        error = AWS_IOT_SERIALIZER_INVALID_INPUT;
    }

    return error;
}

/*-----------------------------------------------------------*/

static AwsIotSerializerError_t _stepIn( AwsIotSerializerDecoderObject_t * pDecoderObject,
                                        AwsIotSerializerDecoderIterator_t * pIterator )
{
    AwsIotSerializerDecoderObject_t * pNewObject;
    _jsonContainer_t * pContainer, * pNewContainer;
    size_t offset = 0;
    AwsIotSerializerError_t error = AWS_IOT_SERIALIZER_SUCCESS;

    if( _isValidContainer( pDecoderObject ) )
    {
        pContainer = pDecoderObject->pHandle;

        _skipWhiteSpacesAndDelimeters( pContainer->pStart, pContainer->length, &offset );
        if( offset >= pContainer->length )
        {
            error = AWS_IOT_SERIALIZER_INTERNAL_FAILURE;
        }

        if( error == AWS_IOT_SERIALIZER_SUCCESS )
        {
            pNewContainer = _createContainer( ( pContainer->pStart + offset ), pContainer->length );
            if( pNewContainer != NULL )
            {
                pNewObject = pvPortMalloc( sizeof( AwsIotSerializerDecoderObject_t ) );
                if( pNewObject != NULL )
                {
                    pNewObject->type = pDecoderObject->type;
                    pNewObject->pHandle = pNewContainer;
                    *pIterator = ( AwsIotSerializerDecoderIterator_t ) pNewObject;
                }
                else
                {
                    vPortFree( pNewContainer );
                    error = AWS_IOT_SERIALIZER_OUT_OF_MEMORY;
                }
            }
            else
            {
                error = AWS_IOT_SERIALIZER_OUT_OF_MEMORY;
            }
        }
    }
    else
    {
        error = AWS_IOT_SERIALIZER_INVALID_INPUT;
    }


    return error;
}

/*-----------------------------------------------------------*/

static bool _isEOF( const char * pBuffer,
                    AwsIotSerializerDataType_t containerType )
{
    bool isEOF;

    switch( containerType )
    {
        case AWS_IOT_SERIALIZER_CONTAINER_MAP:
            isEOF = ( pBuffer[ 0 ] == '}' );
            break;

        case AWS_IOT_SERIALIZER_CONTAINER_ARRAY:
            isEOF = ( pBuffer[ 0 ] == ']' );
            break;

        default:
            isEOF = false;
            break;
    }

    return isEOF;
}

/*-----------------------------------------------------------*/

static bool _isEndOfContainer( AwsIotSerializerDecoderIterator_t iterator )
{
    AwsIotSerializerDecoderObject_t * pObject = ( AwsIotSerializerDecoderObject_t * ) iterator;
    _jsonContainer_t * pContainer;
    bool ret = false;

    if( _isValidContainer( pObject ) )
    {
        pContainer = pObject->pHandle;
        ret = _isEOF( pContainer->pStart, pObject->type );
    }
    return ret;
}

/*-----------------------------------------------------------*/

static AwsIotSerializerError_t _get( AwsIotSerializerDecoderIterator_t iterator,
                                     AwsIotSerializerDecoderObject_t * pValueObject )
{
    AwsIotSerializerDecoderObject_t * pDecoder = ( AwsIotSerializerDecoderObject_t * ) iterator;
    _jsonContainer_t * pContainer;
    AwsIotSerializerDataType_t type;
    size_t offset = 0;
    AwsIotSerializerError_t error = AWS_IOT_SERIALIZER_SUCCESS;

    if( _isValidContainer( pDecoder ) )
    {
        pContainer = _castDecoderIteratorToJsonContainer( iterator );
        type = _getTokenType( pContainer->pStart, offset );
        if( type != AWS_IOT_SERIALIZER_UNDEFINED )
        {
            parseTokenValue( pContainer->pStart, pContainer->length, &offset, type, pValueObject );
            if( offset >= pContainer->length )
            {
                error = AWS_IOT_SERIALIZER_BUFFER_TOO_SMALL;
            }
        }
        else
        {
            error = AWS_IOT_SERIALIZER_INTERNAL_FAILURE;
        }
    }
    else
    {
        error = AWS_IOT_SERIALIZER_INVALID_INPUT;
    }

    return error;
}


/*-----------------------------------------------------------*/

static AwsIotSerializerError_t _next( AwsIotSerializerDecoderIterator_t iterator )
{
    AwsIotSerializerDecoderObject_t * pObject = ( AwsIotSerializerDecoderObject_t * ) iterator;
    _jsonContainer_t * pContainer;
    AwsIotSerializerDataType_t type;
    size_t offset = 0;
    AwsIotSerializerError_t error = AWS_IOT_SERIALIZER_SUCCESS;

    if( _isValidContainer( pObject ) )
    {
        pContainer = pObject->pHandle;
        type = _getTokenType( pContainer->pStart, offset );
        parseTokenValue( pContainer->pStart, pContainer->length, &offset, type, NULL );
        _skipWhiteSpacesAndDelimeters( pContainer->pStart, pContainer->length, &offset );

        if( offset < pContainer->length )
        {
            pContainer->pStart += offset;
        }
        else
        {
            error = AWS_IOT_SERIALIZER_BUFFER_TOO_SMALL;
        }
    }
    else
    {
        error = AWS_IOT_SERIALIZER_INVALID_INPUT;

    }

    return error;
}

/*-----------------------------------------------------------*/

static AwsIotSerializerError_t _stepOut( AwsIotSerializerDecoderIterator_t iterator,
                                         AwsIotSerializerDecoderObject_t * pDecoderObject )
{
    AwsIotSerializerDecoderObject_t * pIterObject = ( AwsIotSerializerDecoderObject_t * ) iterator;
    _jsonContainer_t * pContainer, * pIterContainer;
    AwsIotSerializerError_t error = AWS_IOT_SERIALIZER_SUCCESS;

    if( _isValidContainer( pIterObject ) && _isValidContainer( pDecoderObject ) )
    {

        pContainer = pDecoderObject->pHandle;
        pIterContainer = pIterObject->pHandle;
        if( _isEOF( pIterContainer->pStart, pIterObject->type ) )
        {
            pContainer->pStart = ( pIterContainer->pStart + 1 );
            vPortFree( pIterContainer );
            vPortFree( pIterObject );
        }
        else
        {
            error = AWS_IOT_SERIALIZER_INTERNAL_FAILURE;
        }
    }
    else
    {
        error = AWS_IOT_SERIALIZER_INVALID_INPUT;
    }

    return error;
}

/*-----------------------------------------------------------*/

static void _destroy( AwsIotSerializerDecoderObject_t * pDecoderObject )
{
    if( _isValidContainer( pDecoderObject ) )
    {
        if( pDecoderObject->pHandle != NULL )
        {
            vPortFree( pDecoderObject->pHandle );
            pDecoderObject->pHandle = NULL;
        }
    }
}
