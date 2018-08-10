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

/*
 * Section: Included Files
 */

#include "aws_cbor_internals.h"
#include <assert.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

/** @brief Checks if CBOR cursor is out of bounds */
#define CursorOutOfBounds( cbor_data ) \
    OverflowOccured( ( cbor_data ) ) || UnderflowOccured( ( cbor_data ) )

/** @brief Checks if CBOR cursor is past the end of the buffer */
#define OverflowOccured( cbor_data ) \
    ( ( cbor_data )->buffer_end < ( cbor_data )->cursor )

/** @brief Checks if CBOR cursor is before the start of the buffer */
#define UnderflowOccured( cbor_data ) \
    ( ( cbor_data )->buffer_start > ( cbor_data )->cursor )

/** @brief Calculates the size of the CBOR buffer */
#define BufferSize( cbor_data ) \
    ( ( cbor_data )->buffer_end - ( cbor_data )->buffer_start + 1 )

/**
 * @brief Doubles the size of the CBOR buffer
 *
 * Reallocates the CBOR buffer with double the size of the current buffer size.
 * Sets err if unable to reallocate the space.
 */
static void CBOR_Reallocate( cbor_handle_t pxCbor_data )
{
    assert( NULL != pxCbor_data );

    /* Multiply by 1.5, more efficient on some compilers than just doing *1.5 */
    /* https://github.com/facebook/folly/blob/master/folly/docs/FBVector.md */
    cbor_ssize_t xNew_size = BufferSize( pxCbor_data ) * 3;
    xNew_size /= 2;

    cbor_byte_t * pxNew_start = pxCBOR_realloc( pxCbor_data->buffer_start, xNew_size );

    if( NULL == pxNew_start )
    {
        pxCbor_data->err = eCBOR_ERR_INSUFFICENT_SPACE;

        return;
    }

    /* Adjust pointers to new location */
    cbor_ssize_t xCursor_index = pxCbor_data->cursor - pxCbor_data->buffer_start;
    pxCbor_data->buffer_start = pxNew_start;
    pxCbor_data->buffer_end = pxCbor_data->buffer_start + xNew_size - 1;
    pxCbor_data->cursor = pxCbor_data->buffer_start + xCursor_index;
}

/**
 * @brief Copies memory into the CBOR buffer "front to back."
 *
 * Copies memory starting from the lowest address and ending with the highest
 * address.
 */
static void CBOR_MemCopyLowToHigh( cbor_handle_t pxCbor_data,
                                   const cbor_byte_t * pxSource,
                                   cbor_ssize_t xLength )
{
    assert( NULL != pxCbor_data );
    assert( NULL != pxSource );
    assert( 0 <= xLength );
    assert( pxCbor_data->cursor < pxSource );

    for( cbor_int_t xI = 0; xI < xLength; xI++ )
    {
        CBOR_AssignAndIncrementCursor( pxCbor_data, *pxSource++ );

        if( pxCbor_data->err )
        {
            return;
        }
    }
}

/**
 * @brief Copies memory into the CBOR buffer "back to front."
 *
 * Copies memory starting from the highest address and ending with the lowest
 * address.
 */
static void CBOR_MemCopyHighToLow( cbor_handle_t pxCbor_data,
                                   const cbor_byte_t * pxSource,
                                   cbor_ssize_t xLength )
{
    assert( NULL != pxCbor_data );
    assert( NULL != pxSource );
    assert( 0 <= xLength );
    assert( pxCbor_data->cursor > pxSource );

    pxSource += ( xLength - 1 );
    pxCbor_data->cursor += ( xLength - 1 );

    for( cbor_int_t xI = 0; xI < xLength; xI++ )
    {
        CBOR_AssignAndDecrementCursor( pxCbor_data, *pxSource-- );
        /* assert, because else case is unreachable, unlike in 'LowToHigh' */
        assert( eCBOR_ERR_NO_ERROR == pxCbor_data->err );
    }

    pxCbor_data->cursor += xLength + 1;
}

void CBOR_AssignAndIncrementCursor( cbor_handle_t pxCbor_data,
                                    cbor_byte_t xInput )
{
    assert( NULL != pxCbor_data );
    assert( !UnderflowOccured( pxCbor_data ) );

    while( OverflowOccured( pxCbor_data ) )
    {
        CBOR_Reallocate( pxCbor_data );

        if( eCBOR_ERR_NO_ERROR != pxCbor_data->err )
        {
            return;
        }
    }

    *( pxCbor_data->cursor )++ = xInput;
    ( pxCbor_data->err ) = eCBOR_ERR_NO_ERROR;
}

void CBOR_AssignAndDecrementCursor( cbor_handle_t pxCbor_data,
                                    cbor_byte_t xInput )
{
    assert( NULL != pxCbor_data );
    assert( !UnderflowOccured( pxCbor_data ) );

    while( OverflowOccured( pxCbor_data ) )
    {
        CBOR_Reallocate( pxCbor_data );

        if( eCBOR_ERR_NO_ERROR != pxCbor_data->err )
        {
            return;
        }
    }

    *( pxCbor_data->cursor )-- = xInput;
    ( pxCbor_data->err ) = eCBOR_ERR_NO_ERROR;
}

void CBOR_MemCopy( cbor_handle_t pxCbor_data,
                   const void * pxInput,
                   cbor_ssize_t xLength )
{
    assert( NULL != pxCbor_data );
    assert( NULL != pxInput );
    assert( 0 <= xLength );

    const cbor_byte_t * pxSource = pxInput;

    if( pxCbor_data->cursor < pxSource )
    {
        CBOR_MemCopyLowToHigh( pxCbor_data, pxSource, xLength );
    }
    else if( pxCbor_data->cursor > pxSource )
    {
        CBOR_MemCopyHighToLow( pxCbor_data, pxSource, xLength );
    }
    else
    {
        /* Copying from a location into itself, so we can skip the copy part and
         * just move the cursor.  Maybe a user error, but not necessarily*/
        pxCbor_data->cursor += xLength;
    }
}

cbor_ssize_t CBOR_DataItemSize( cbor_handle_t pxCbor_data )
{
    assert( NULL != pxCbor_data );

    return CBOR_DataItemSizePtr( pxCbor_data->cursor );
}

cbor_ssize_t CBOR_DataItemSizePtr( const cbor_byte_t * pxPtr )
{
    assert( NULL != pxPtr );

    cbor_byte_t xData_head = *pxPtr;
    cbor_byte_t xMajor_type = xData_head & CBOR_MAJOR_TYPE_MASK;

    switch( xMajor_type )
    {
        case CBOR_POS_INT:

            return CBOR_IntSize( pxPtr );

        case CBOR_STRING:

            return CBOR_StringSize( pxPtr );

        case CBOR_MAP:

            return CBOR_MapSize( pxPtr );

        default:
            break;
    }

    return 0;
}

void CBOR_ValueResize( cbor_handle_t pxCbor_data,
                       cbor_ssize_t xNew_size )
{
    assert( NULL != pxCbor_data );

    cbor_ssize_t xOld_size = CBOR_DataItemSize( pxCbor_data );

    if( 0 == xOld_size )
    {
        return;
    }

    cbor_byte_t * pxCurrent_place = pxCbor_data->cursor;
    cbor_byte_t * pxNext_key = pxCurrent_place + xOld_size;
    cbor_byte_t * pxKey_position = pxCurrent_place + xNew_size;
    cbor_ssize_t xRemaining_length = pxCbor_data->map_end - pxNext_key + 1;
    assert( 0 <= xRemaining_length );
    pxCbor_data->cursor = pxKey_position;
    CBOR_MemCopy( pxCbor_data, pxNext_key, xRemaining_length );
    assert( eCBOR_ERR_NO_ERROR == pxCbor_data->err );
    pxCbor_data->cursor = pxCurrent_place;
}
