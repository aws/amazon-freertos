/*
 * Amazon FreeRTOS CBOR Library V1.0.1
 * Copyright (C) 2018 Amazon.com, Inc. or its affiliates.  All Rights Reserved.
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
#include "aws_cbor_internals.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/**
 * @brief Appends a key value pair to to the destination string
 *
 * @note str will get reallocated if there is insufficient space for the key.
 *
 * Reads the key value pair from the CBOR data and appends them to the end of
 * the string
 *
 * @param  str       Destination string
 * @param  cbor_data CBOR handle with cursor pointed at key to append
 * @param  buf_len   Pointer to length of the str buf
 * @return           New pointer to str
 */
static char * CBOR_CatKeyValue( char * pcStr,
                                cbor_handle_t pxCbor_data,
                                cbor_ssize_t * pxBuf_len );

/**
 * @brief Append key from cbor data to destination string
 *
 * @note str will get reallocated if there is insufficient space for the key.
 *
 * Reads the key from the CBOR structure and concatonates it to the str.  It
 * will also append a ":".
 *
 * @param  str       Destination string
 * @param  cbor_data CBOR handle with cursor pointed at key to append
 * @param  buf_len   Pointer to length of the str buf
 * @return           New pointer to str
 */
static char * CBOR_CatKey( char * pcStr,
                           cbor_handle_t pxCbor_data,
                           cbor_ssize_t * pxBuf_len );

/**
 * @brief Append value from cbor data to destination string
 *
 * @note str will get reallocated if there is insufficient space for the value.
 *
 * @param  str       Destination string
 * @param  cbor_data CBOR handle with cursor pointed at value to append
 * @param  buf_len   Pointer to length of the str buf
 * @return           New pointer to str
 */
static char * CBOR_CatValue( char * pcStr,
                             cbor_handle_t pxCbor_data,
                             cbor_ssize_t * pxBuf_len );

/**
 * @brief Return string representation of integer at cursor location
 *
 * @note allocates data memory for string return
 *
 * @param  cbor_data handle for cbor object
 * @return           String representation of integer
 */
static char * CBOR_IntAsString( cbor_handle_t pxCbor_data );

/**
 * @brief concatenates two strings together
 *
 * @note allocates data memory for string return
 *
 * Concatenates two strings together (dest + src).  If there is insufficient
 * space in the destination, the destination is reallocated with sufficient
 * space.
 *
 * @param  dest Pointer to destination string.
 * @note The dest pointer may be eallocated, so the user must use the returned
 *     pointer
 * @param  src  [description]
 * @param  len  [description]
 * @return      [description]
 */
static char * CBOR_StrCat( char * pcDest,
                           char const * pcSrc,
                           cbor_ssize_t * pxLen );

char * CBOR_AsString( cbor_handle_t pxCbor_data )
{
    CBOR_SetCursor( pxCbor_data, 0 );
    cbor_ssize_t xBuf_len = 16;
    char * pcStr = pxCBOR_malloc( xBuf_len );
    pcStr[ 0 ] = '{';
    pcStr[ 1 ] = 0;
    CBOR_NextKey( pxCbor_data );

    while( !( CBOR_BREAK == *( pxCbor_data->cursor ) ) )
    {
        pcStr = CBOR_CatKeyValue( pcStr, pxCbor_data, &xBuf_len );
        CBOR_NextKey( pxCbor_data );

        if( !( CBOR_BREAK == *( pxCbor_data->cursor ) ) )
        {
            pcStr = CBOR_StrCat( pcStr, ",", &xBuf_len );
        }
    }

    pcStr = CBOR_StrCat( pcStr, "}", &xBuf_len );

    return pcStr;
}

static char * CBOR_CatKeyValue( char * pcStr,
                                cbor_handle_t pxCbor_data,
                                cbor_ssize_t * pxBuf_len )
{
    pcStr = CBOR_CatKey( pcStr, pxCbor_data, pxBuf_len );
    pcStr = CBOR_CatValue( pcStr, pxCbor_data, pxBuf_len );

    return pcStr;
}

static char * CBOR_CatKey( char * pcStr,
                           cbor_handle_t pxCbor_data,
                           cbor_ssize_t * pxBuf_len )
{
    pcStr = CBOR_StrCat( pcStr, "\"", pxBuf_len );
    char * pcKey = CBOR_ReadString( pxCbor_data );
    pcStr = CBOR_StrCat( pcStr, pcKey, pxBuf_len );
    pxCBOR_free( pcKey );
    pcStr = CBOR_StrCat( pcStr, "\":", pxBuf_len );

    return pcStr;
}

static char * CBOR_CatValue( char * pcStr,
                             cbor_handle_t pxCbor_data,
                             cbor_ssize_t * pxBuf_len )
{
    cbor_byte_t * pxKey_pos = pxCbor_data->cursor;

    CBOR_Next( pxCbor_data );
    cbor_byte_t xJump_type = *pxCbor_data->cursor;
    cbor_byte_t xMajor_type = ( CBOR_MAJOR_TYPE_MASK & xJump_type );

    switch( xMajor_type )
    {
        case CBOR_POS_INT:
        case CBOR_NEG_INT:
           {
               char * pcVal = CBOR_IntAsString( pxCbor_data );
               pcStr = CBOR_StrCat( pcStr, pcVal, pxBuf_len );
               pxCBOR_free( pcVal );
               break;
           }

        case CBOR_BYTE_STRING:
        case CBOR_STRING:
           {
               pcStr = CBOR_StrCat( pcStr, "\"", pxBuf_len );
               char * pcVal = CBOR_ReadString( pxCbor_data );
               pcStr = CBOR_StrCat( pcStr, pcVal, pxBuf_len );
               pxCBOR_free( pcVal );
               pcStr = CBOR_StrCat( pcStr, "\"", pxBuf_len );
               break;
           }

        case CBOR_MAP:
           {
               cbor_handle_t pxInner_map = CBOR_ReadMap( pxCbor_data );
               char * pcVal = CBOR_AsString( pxInner_map );
               CBOR_Delete( &pxInner_map );
               pcStr = CBOR_StrCat( pcStr, pcVal, pxBuf_len );
               pxCBOR_free( pcVal );
               break;
           }
    }

    pxCbor_data->cursor = pxKey_pos;

    return pcStr;
}

static char * CBOR_IntAsString( cbor_handle_t pxCbor_data )
{
    int xVal = CBOR_ReadInt( pxCbor_data );
    int xLen = snprintf( NULL, 0, "%d", xVal ) + 1;
    char * pcStr = pxCBOR_malloc( xLen );

    ( void ) snprintf( pcStr, xLen, "%d", xVal );

    return pcStr;
}

static char * CBOR_StrCat( char * pcDest,
                           char const * pcSrc,
                           cbor_ssize_t * pxLen )
{
    cbor_ssize_t xDest_len = strlen( pcDest );
    cbor_ssize_t xSrc_len = strlen( pcSrc );

    if( *pxLen <= xDest_len + xSrc_len )
    {
        do
        {
            *pxLen *= 3;
            *pxLen /= 2;

            /* Inefficient, but necessary to reallocate in each loop, since some
             * embedded platforms (e.g. FreeRTOS) do not implement realloc. */
            pcDest = pxCBOR_realloc( pcDest, *pxLen );
        } while( *pxLen <= xDest_len + xSrc_len );
    }

    strncat( pcDest, pcSrc, ( *pxLen - xDest_len ) );

    return pcDest;
}
