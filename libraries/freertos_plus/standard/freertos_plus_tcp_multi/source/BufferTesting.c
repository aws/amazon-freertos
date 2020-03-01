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

/* Standard includes. */
#include <stdint.h>

/* FreeRTOS includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"

/* FreeRTOS+TCP includes. */
#include "FreeRTOS_UDP_IP.h"
#include "FreeRTOS_IP.h"
#include "FreeRTOS_Sockets.h"
#include "FreeRTOS_IP_Private.h"
#include "BufferTesting.h"

#ifndef PRIO_BUFFER_TASK
	#define PRIO_BUFFER_TASK	tskIDLE_PRIORITY
#endif

#ifndef STACK_BUFFER_TASK
	#define STACK_BUFFER_TASK	( 2 * configMINIMAL_STACK_SIZE )
#endif

#ifndef BUFFER_IO_SIZE
	#define BUFFER_IO_SIZE	2048
#endif


static StreamBuffer_t *pxBuffers[ 2 ] = { NULL, NULL };
static uint8_t *ioBuffers[ 2 ] = { NULL, NULL };

static void vBufferTask( void *pvParameter );

BaseType_t xBufferTestingPrepare( size_t uxSize )
{
BaseType_t xIndex;
size_t uxLength;
BaseType_t xReturn = pdPASS;

	uxLength = uxSize;

	/* Add an extra 4 (or 8) bytes. */
	uxLength += sizeof( size_t );

	/* And make the length a multiple of sizeof( size_t ). */
	uxLength &= ~( sizeof( size_t ) - 1u );

	uxSize = sizeof( *pxBuffers[ 0 ] ) - sizeof( pxBuffers[ 0 ]->ucArray ) + uxLength;

	for( xIndex = 0; xIndex < ARRAY_SIZE( pxBuffers ); xIndex++ )
	{
		pxBuffers[ xIndex ] = ( StreamBuffer_t * ) pvPortMallocLarge( uxSize );
		ioBuffers[ xIndex ] = ( uint8_t * ) pvPortMallocLarge( BUFFER_IO_SIZE + 2 * sizeof( size_t ) );

		if( ( pxBuffers[ xIndex ] == NULL ) && ( ioBuffers[ xIndex ] != NULL ) )
		{
			FreeRTOS_printf( ( "xBufferTestingPrepare: malloc failed\n" ) );
			xReturn = pdFAIL;
			break;
		}
		else
		{
			/* Clear the markers of the stream */
			memset( pxBuffers[ xIndex ], 0, sizeof( *pxBuffers[ xIndex ] ) - sizeof( pxBuffers[ xIndex ]->ucArray ) );
			pxBuffers[ xIndex ]->LENGTH = uxLength;
			FreeRTOS_printf( ( "xBufferTestingPrepare: malloc %u OK\n", ( unsigned )uxSize ) );
		}
	}
	if( xReturn == pdPASS )
	{
		for( xIndex = 0; xIndex < ARRAY_SIZE( pxBuffers ); xIndex++ )
		{
			void *pvParameter = ( void * ) xIndex;
			if( xTaskCreate( vBufferTask, "vBufferTask", STACK_BUFFER_TASK, pvParameter, PRIO_BUFFER_TASK, NULL ) != pdPASS )
			{
				xReturn = pdFAIL;
				break;
			}
		}
	}
	if( xReturn == pdFAIL )
	{
		for( xIndex = 0; xIndex < ARRAY_SIZE( pxBuffers ); xIndex++ )
		{
			vPortFree( pxBuffers[ xIndex ] );
			vPortFree( ioBuffers[ xIndex ] );
		}
	}
	return xReturn;
}

#if( USE_SOFT_WDT != 0 )
	extern void clearWDT( void );
#endif

size_t xBufferSendCounts[ 2 ], xBufferRecvCounts[ 2 ];
BaseType_t xBufferTestingContentsCheck = pdTRUE;
BaseType_t xBufferTestingStop = pdFALSE;

static void vBufferTask( void *pvParameter )
{
int iIndex = ( int ) pvParameter;
size_t x;
uint8_t ucSendByte = 0, ucRecvByte = 0;
BaseType_t xErrorFound = pdFALSE;
uint8_t *ioBuffer = ioBuffers[ iIndex ];
StreamBuffer_t *pxSendBuffer, *pxRecvBuffer;

	FreeRTOS_printf( ( "vBufferTask[%d] started\n", iIndex ) );

	pxSendBuffer = pxBuffers[ iIndex ];
	if( iIndex == 0 )
	{
		/* Point to the other buffer. */
		pxRecvBuffer = pxBuffers[ 1 ];
	}
	else
	{
		pxRecvBuffer = pxBuffers[ 0 ];
	}
	
	

	while( ( xErrorFound == pdFALSE ) && ( xBufferTestingStop == pdFALSE ) )
	{
	size_t uxCount, uxResult, uxLength;

		/* Send data bytes. */
		if( xBufferTestingContentsCheck != pdFALSE )
		{
			/* Test on contents with varable sizes. */
			uxCount = 1 + ( rand( ) % BUFFER_IO_SIZE );
		}
		else
		{
			/* Test on speed with fixed sizes. */
			uxCount = BUFFER_IO_SIZE / 2;
		}
		if( uxCount <= uxStreamBufferGetSpace( pxSendBuffer ) )
		{
			if( xBufferTestingContentsCheck != pdFALSE )
			{
				for( x = 0; x < uxCount; x++ )
				{
					ioBuffer[ x ] = ucSendByte;
					ucSendByte++;
				}
			}
			
			uxResult = uxStreamBufferAdd( pxSendBuffer, 0ul, ioBuffer, uxCount );
			if( uxResult != uxCount )
			{
				FreeRTOS_printf( ( "vBufferTask[%d]: uxStreamBufferAdd: %u != %u after %u bytes\n",
					iIndex,
					( unsigned ) uxResult,
					( unsigned ) uxCount,
					( unsigned ) xBufferSendCounts[ iIndex ] ) );
				xErrorFound = pdTRUE;
				break;
			}
			xBufferSendCounts[ iIndex ] += uxCount;
		}

		/* Receive data bytes. */
		uxLength = uxStreamBufferGetSize( pxRecvBuffer );
		if( uxLength > 0 )
		{
			uxResult = uxStreamBufferGet( pxRecvBuffer, 0ul, ioBuffer, uxLength, pdFALSE );
			if( uxResult != uxLength )
			{
				FreeRTOS_printf( ( "vBufferTask[%d]: uxStreamBufferGet: %u != %u after %u bytes\n",
					iIndex,
					( unsigned ) uxResult,
					( unsigned ) uxLength,
					( unsigned ) xBufferRecvCounts[ iIndex ] ) );
				xErrorFound = pdTRUE;
				break;
			}
			xBufferRecvCounts[ iIndex ] += uxResult;
			if( xBufferTestingContentsCheck != pdFALSE )
			{
				for( x = 0; x < uxResult; x++ )
				{
					if( ioBuffer[ x ] != ucRecvByte )
					{
						FreeRTOS_printf( ( "vBufferTask[%d]: 0x%02X != 0x%02X after %u bytes x = %u\n",
							iIndex,
							ioBuffer[ x ],
							ucRecvByte,
							( unsigned ) xBufferRecvCounts[ iIndex ],
							( unsigned ) x ) );
						xErrorFound = pdTRUE;
						break;
					}
					ucRecvByte++;
				}
			}
		}
		#if( USE_SOFT_WDT != 0 )
		{
			clearWDT();
		}
		#endif	/* USE_SOFT_WDT */
	}
	for( ;; )
	{
		vTaskDelay( 10000 );
		#if( USE_SOFT_WDT != 0 )
		{
			clearWDT();
		}
		#endif	/* USE_SOFT_WDT */
	}
}

