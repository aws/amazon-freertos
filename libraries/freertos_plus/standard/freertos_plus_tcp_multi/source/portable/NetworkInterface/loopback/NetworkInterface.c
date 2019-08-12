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
#include <stdio.h>
#include <stdlib.h>

/* FreeRTOS includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"

/* FreeRTOS+TCP includes. */
#include "FreeRTOS_IP.h"
#include "FreeRTOS_Sockets.h"
#include "FreeRTOS_IP_Private.h"
#include "FreeRTOS_DNS.h"
#include "FreeRTOS_Routing.h"
#include "NetworkBufferManagement.h"
#include "NetworkInterface.h"

#if( ipconfigUSE_LOOPBACK == 0 )
	#error Please define ipconfigUSE_LOOPBACK as 1 if you want to use the loop-back interface
#endif

#define ipICMP_ECHO_REQUEST				( ( uint8_t ) 8 )
#define ipICMP_ECHO_REPLY				( ( uint8_t ) 0 )

/*-----------------------------------------------------------*/

NetworkInterface_t *xLoopbackInterface;

static BaseType_t prvLoopback_Initialise( NetworkInterface_t *pxInterface );
static BaseType_t prvLoopback_Output( NetworkInterface_t *pxInterface, NetworkBufferDescriptor_t * const pxBuffer, BaseType_t bReleaseAfterSend );
static BaseType_t prvLoopback_GetPhyLinkStatus( NetworkInterface_t *pxInterface );

/*-----------------------------------------------------------*/

static BaseType_t prvLoopback_Initialise( NetworkInterface_t *pxInterface )
{
	/* When returning non-zero, the stack will become active and
    start DHCP (in configured) */
	return pdTRUE;
}
/*-----------------------------------------------------------*/

/* pxLoopback_FillInterfaceDescriptor() goes into the NetworkInterface.c of the SAM4e driver. */

NetworkInterface_t *pxLoopback_FillInterfaceDescriptor( BaseType_t xEMACIndex, NetworkInterface_t *pxInterface )
{
/* This function pxLoopback_FillInterfaceDescriptor() adds a network-interface.
Make sure that the object pointed to by 'pxInterface'
is declared static or global, and that it will remain to exist. */

	memset( pxInterface, '\0', sizeof( *pxInterface ) );
	pxInterface->pcName				= "Loopback";	/* Just for logging, debugging. */
	pxInterface->pvArgument			= (void*)xEMACIndex;		/* Has only meaning for the driver functions. */
	pxInterface->pfInitialise		= prvLoopback_Initialise;
	pxInterface->pfOutput			= prvLoopback_Output;
	pxInterface->pfGetPhyLinkStatus = prvLoopback_GetPhyLinkStatus;
	xLoopbackInterface = pxInterface;

	return pxInterface;
}
/*-----------------------------------------------------------*/

static BaseType_t prvLoopback_GetPhyLinkStatus( NetworkInterface_t *pxInterface )
{
	/* This function returns true if the Link Status in the PHY is high. */
	return pdTRUE;
}
/*-----------------------------------------------------------*/

static BaseType_t prvLoopback_Output( NetworkInterface_t *pxInterface, NetworkBufferDescriptor_t * const pxDescriptor, BaseType_t bReleaseAfterSend )
{
	if( bReleaseAfterSend == pdFALSE )
	{
		NetworkBufferDescriptor_t *pxNewDescriptor =
			pxDuplicateNetworkBufferWithDescriptor( pxDescriptor, pxDescriptor->xDataLength );
		*( ( NetworkBufferDescriptor_t ** ) &pxDescriptor ) = pxNewDescriptor;
	}
	if( pxDescriptor )
	{
	IPStackEvent_t xRxEvent;

		xRxEvent.eEventType = eNetworkRxEvent;
		xRxEvent.pvData = ( void * ) pxDescriptor;
		if( xSendEventStructToIPTask( &xRxEvent, 0u ) != pdTRUE )
		{
			vReleaseNetworkBufferAndDescriptor( pxDescriptor );
			iptraceETHERNET_RX_EVENT_LOST();
			FreeRTOS_printf( ( "prvEMACRxPoll: Can not queue return packet!\n" ) );
		}
	}
	/* The return value is actually ignored by the IP-stack. */
	return pdTRUE;
}
/*-----------------------------------------------------------*/
