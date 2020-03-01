/*
 * FreeRTOS+TCP Multi Interface Labs Build 180222
 * Copyright (C) 2018 Amazon.com, Inc. or its affiliates.  All Rights Reserved.
 * Authors include Hein Tibosch and Richard Barry
 *
 *******************************************************************************
 ***** NOTE ******* NOTE ******* NOTE ******* NOTE ******* NOTE ******* NOTE ***
 ***                                                                         ***
 ***                                                                         ***
 ***   This is a version of FreeRTOS+TCP that supports multiple network      ***
 ***   interfaces, and includes basic IPv6 functionality.  Unlike the base   ***
 ***   version of FreeRTOS+TCP, THE MULTIPLE INTERFACE VERSION IS STILL IN   ***
 ***   THE LAB.  While it is functional and has been used in commercial      ***
 ***   products we are still refining its design, the source code does not   ***
 ***   yet quite conform to the strict coding and style standards, and the   ***
 ***   documentation and testing is not complete.                            ***
 ***                                                                         ***
 ***   PLEASE REPORT EXPERIENCES USING THE SUPPORT RESOURCES FOUND ON THE    ***
 ***   URL: http://www.FreeRTOS.org/contact                                  ***
 ***                                                                         ***
 ***                                                                         ***
 ***** NOTE ******* NOTE ******* NOTE ******* NOTE ******* NOTE ******* NOTE ***
 *******************************************************************************
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
 *	FreeRTOS_Stream_Buffer.h
 *
 *	A cicular character buffer
 *	An implementation of a circular buffer without a length field
 *	If LENGTH defines the size of the buffer, a maximum of (LENGT-1) bytes can be stored
 *	In order to add or read data from the buffer, memcpy() will be called at most 2 times
 */

#ifndef FREERTOS_STREAM_BUFFER_H
#define	FREERTOS_STREAM_BUFFER_H

#ifdef __cplusplus
extern "C" {
#endif

typedef struct xSTREAM_BUFFER {
	volatile size_t uxTail;		/* next item to read */
	volatile size_t uxMid;		/* iterator within the valid items */
	volatile size_t uxHead;		/* next position store a new item */
	volatile size_t uxFront;	/* iterator within the free space */
	size_t LENGTH;				/* const value: number of reserved elements */
	uint8_t ucArray[ sizeof( size_t ) ];
} Buffer_t;

static portINLINE void vStream_BufferClear( Buffer_t *pxBuffer )
{
	/* Make the circular buffer empty */
	pxBuffer->uxHead = 0;
	pxBuffer->uxTail = 0;
	pxBuffer->uxFront = 0;
	pxBuffer->uxMid = 0;
}
/*-----------------------------------------------------------*/

static portINLINE size_t uxBufferSpace( const Buffer_t *pxBuffer, size_t uxLower, size_t uxUpper )
{
/* Returns the space between lLower and lUpper, which equals to the distance minus 1 */
size_t uxCount;

	uxCount = pxBuffer->LENGTH + uxUpper - uxLower - 1;
	if( uxCount >= pxBuffer->LENGTH )
	{
		uxCount -= pxBuffer->LENGTH;
	}

	return uxCount;
}
/*-----------------------------------------------------------*/

static portINLINE size_t uxBufferDistance( const Buffer_t *pxBuffer, size_t uxLower, size_t uxUpper )
{
/* Returns the distance between lLower and lUpper */
size_t uxCount;

	uxCount = pxBuffer->LENGTH + uxUpper - uxLower;
	if ( uxCount >= pxBuffer->LENGTH )
	{
		uxCount -= pxBuffer->LENGTH;
	}

	return uxCount;
}
/*-----------------------------------------------------------*/

static portINLINE size_t uxBufferGetSpace( const Buffer_t *pxBuffer )
{
	/* Returns the number of items which can still be added to lHead
	before hitting on lTail */
	return uxBufferSpace( pxBuffer, pxBuffer->uxHead, pxBuffer->uxTail );
}
/*-----------------------------------------------------------*/

static portINLINE size_t uxBufferFrontSpace( const Buffer_t *pxBuffer )
{
	/* Distance between lFront and lTail
	or the number of items which can still be added to lFront,
	before hitting on lTail */
	return uxBufferSpace( pxBuffer, pxBuffer->uxFront, pxBuffer->uxTail );
}
/*-----------------------------------------------------------*/

static portINLINE size_t uxBufferGetSize( const Buffer_t *pxBuffer )
{
	/* Returns the number of items which can be read from lTail
	before reaching lHead */
	return uxBufferDistance( pxBuffer, pxBuffer->uxTail, pxBuffer->uxHead );
}
/*-----------------------------------------------------------*/

static portINLINE size_t uxBufferMidSpace( const Buffer_t *pxBuffer )
{
	/* Returns the distance between lHead and lMid */
	return uxBufferDistance( pxBuffer, pxBuffer->uxMid, pxBuffer->uxHead );
}
/*-----------------------------------------------------------*/

static portINLINE void vStream_BufferMoveMid( Buffer_t *pxBuffer, size_t uxCount )
{
/* Increment lMid, but no further than lHead */
size_t uxSize = uxBufferMidSpace( pxBuffer );

	if( uxCount > uxSize )
	{
		uxCount = uxSize;
	}
	pxBuffer->uxMid += uxCount;
	if( pxBuffer->uxMid >= pxBuffer->LENGTH )
	{
		pxBuffer->uxMid -= pxBuffer->LENGTH;
	}
}
/*-----------------------------------------------------------*/

static portINLINE BaseType_t xStream_BufferIsEmpty( const Buffer_t *pxBuffer )
{
BaseType_t xReturn;

	/* True if no item is available */
	if( pxBuffer->uxHead == pxBuffer->uxTail )
	{
		xReturn = pdTRUE;
	}
	else
	{
		xReturn = pdFALSE;
	}
	return xReturn;
}
/*-----------------------------------------------------------*/

static portINLINE BaseType_t xBufferIsFull( const Buffer_t *pxBuffer )
{
	/* True if the available space equals zero. */
	return uxBufferGetSpace( pxBuffer ) == 0;
}
/*-----------------------------------------------------------*/

static portINLINE BaseType_t xBufferLessThenEqual( const Buffer_t *pxBuffer, size_t uxLeft, size_t uxRight )
{
BaseType_t xReturn;
size_t uxTail = pxBuffer->uxTail;

	/* Returns true if ( uxLeft < uxRight ) */
	if( ( uxLeft < uxTail ) ^ ( uxRight < uxTail ) )
	{
		if( uxRight < uxTail )
		{
			xReturn = pdTRUE;
		}
		else
		{
			xReturn = pdFALSE;
		}
	}
	else
	{
		if( uxLeft <= uxRight )
		{
			xReturn = pdTRUE;
		}
		else
		{
			xReturn = pdFALSE;
		}
	}
	return xReturn;
}
/*-----------------------------------------------------------*/

static portINLINE size_t uxBufferGetPtr( Buffer_t *pxBuffer, uint8_t **ppucData )
{
size_t uxNextTail = pxBuffer->uxTail;
size_t uxSize = uxBufferGetSize( pxBuffer );

	*ppucData = pxBuffer->ucArray + uxNextTail;

	return FreeRTOS_min_uint32( uxSize, pxBuffer->LENGTH - uxNextTail );
}

/*
 * Add bytes to a stream buffer.
 *
 * pxBuffer -	The buffer to which the bytes will be added.
 * lOffset -	If lOffset > 0, data will be written at an offset from lHead
 *				while lHead will not be moved yet.
 * pucData -	A pointer to the data to be added.
 * lCount -		The number of bytes to add.
 */
size_t uxBufferAdd( Buffer_t *pxBuffer, size_t uxOffset, const uint8_t *pucData, size_t uxCount );

/*
 * Read bytes from a stream buffer.
 *
 * pxBuffer -	The buffer from which the bytes will be read.
 * lOffset -	Can be used to read data located at a certain offset from 'lTail'.
 * pucData -	A pointer to the buffer into which data will be read.
 * lMaxCount -	The number of bytes to read.
 * xPeek -		If set to pdTRUE the data will remain in the buffer.
 */
size_t uxBufferGet( Buffer_t *pxBuffer, size_t uxOffset, uint8_t *pucData, size_t uxMaxCount, BaseType_t xPeek );

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif	/* !defined( FREERTOS_STREAM_BUFFER_H ) */
