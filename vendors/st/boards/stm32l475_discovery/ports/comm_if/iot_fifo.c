/*
 * Amazon FreeRTOS CELLULAR Preview Release
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

#include "iot_config.h"

/* Standard includes. */
#include <string.h>

#include "iot_fifo.h"

void IotFifo_Init( IotFifo_t * pFifo, void * pBuffer, uint32_t ulLength, uint32_t ulItemSize )
{
    if( pFifo != NULL )
    {
        pFifo->ulTail = 0UL;
        pFifo->ulHead = 0UL;
        pFifo->ulLength = ulLength;
        pFifo->ulItemSize = ulItemSize;
        pFifo->pBuffer = pBuffer;
    }
}

void IotFifo_DeInit( IotFifo_t * pFifo )
{
    if( pFifo != NULL )
    {
        pFifo->ulLength = 0UL;
        pFifo->ulItemSize = 0UL;
        pFifo->ulTail = 0UL;
        pFifo->ulHead = 0UL;
        pFifo->pBuffer = NULL;
    }
}

bool IotFifo_Put( IotFifo_t * pFifo, const void * pData )
{
    bool ret = false;
    uint32_t ulNext;
	uint8_t * pHead;

    if( ( pFifo != NULL ) && ( pFifo->pBuffer != NULL ) && ( pData != NULL ) )
    {
        ulNext = pFifo->ulHead + pFifo->ulItemSize;
        if( ulNext >= ( pFifo->ulLength * pFifo->ulItemSize ) )
        {
            ulNext = 0UL;
        }
        if( ulNext != pFifo->ulTail )
        {
            pHead = & ( ( ( uint8_t * ) pFifo->pBuffer )[ pFifo->ulHead ] );
            ( void ) memcpy( ( void * )pHead, pData, pFifo->ulItemSize );
            pFifo->ulHead = ulNext;
            ret = true;
        }
    }

    return ret;
}

bool IotFifo_Get( IotFifo_t * pFifo, void * pData )
{
    bool ret = false;
    uint32_t ulNext;
	const uint8_t * pTail;

    if( ( pFifo != NULL ) && ( pFifo->pBuffer != NULL ) && ( pData != NULL ) )
    {
        if( pFifo->ulHead != pFifo->ulTail )
        {
            ulNext = pFifo->ulTail + pFifo->ulItemSize;
            if( ulNext >= ( pFifo->ulLength * pFifo->ulItemSize ) )
            {
                ulNext = 0UL;
            }
            pTail = & ( ( ( uint8_t * ) pFifo->pBuffer )[ pFifo->ulTail ] );
            ( void ) memcpy( pData, ( const void * ) pTail, pFifo->ulItemSize );
            pFifo->ulTail = ulNext;
            ret = true;
        }
    }

    return ret;
}

