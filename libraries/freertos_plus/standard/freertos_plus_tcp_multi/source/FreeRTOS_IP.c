/*
 * FreeRTOS+TCP V2.0.10
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

/* Standard includes. */
#include <stdint.h>
#include <stdio.h>	/*lint !e829*/
#include <string.h>	/*lint !e829*/

/* FreeRTOS includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"

/* FreeRTOS+TCP includes. */
#include "FreeRTOS_IP.h"
#include "FreeRTOS_Sockets.h"
#include "FreeRTOS_IP_Private.h"
#include "FreeRTOS_ARP.h"
#include "FreeRTOS_UDP_IP.h"
#include "FreeRTOS_DHCP.h"
#include "NetworkBufferManagement.h"
#include "FreeRTOS_DNS.h"
#include "FreeRTOS_Routing.h"
#include "FreeRTOS_ND.h"

#include "eventLogging.h"

/* Used to ensure the structure packing is having the desired effect.  The
'volatile' is used to prevent compiler warnings about comparing a constant with
a constant. */
#ifndef _lint
	#define ipEXPECTED_EthernetHeader_t_SIZE	( ( size_t ) 14 )
	#define ipEXPECTED_ARPHeader_t_SIZE			( ( size_t ) 28 )
	#define ipEXPECTED_IPHeader_t_SIZE			( ( size_t ) 20 )
	#define ipEXPECTED_IGMPHeader_t_SIZE		( ( size_t ) 8 )
	#define ipEXPECTED_ICMPHeader_t_SIZE		( ( size_t ) 8 )
	#define ipEXPECTED_UDPHeader_t_SIZE			( ( size_t ) 8 )
	#define ipEXPECTED_TCPHeader_t_SIZE			( ( size_t ) 20 )
#endif

/* ICMP protocol definitions. */
#define ipICMP_ECHO_REQUEST				( ( uint8_t ) 8 )
#define ipICMP_ECHO_REPLY				( ( uint8_t ) 0 )

#if( ipconfigUSE_IPv6 != 0 )
	/* IPv6 multicast constants. */
	#define ipMULTICAST_MAC_ADDRESS_IPv6_0	0x33u
	#define ipMULTICAST_MAC_ADDRESS_IPv6_1	0x33u
#endif

/* Time delay between repeated attempts to initialise the network hardware. */
#ifndef ipINITIALISATION_RETRY_DELAY
	#define ipINITIALISATION_RETRY_DELAY	( pdMS_TO_TICKS( 3000 ) )
#endif

/* Defines how often the ARP timer callback function is executed.  The time is
shorted in the Windows simulator as simulated time is not real time. */
#ifndef	ipARP_TIMER_PERIOD_MS
	#ifdef _WINDOWS_
		#define ipARP_TIMER_PERIOD_MS	( 500 ) /* For windows simulator builds. */
	#else
		#define ipARP_TIMER_PERIOD_MS	( 10000 )
	#endif
#endif

/*lint -e717 to allow for do {} while( 0 ) loops. */
#ifndef iptraceIP_TASK_STARTING
	#define	iptraceIP_TASK_STARTING()	do {} while( 0 )
#endif

#if( ( ipconfigUSE_TCP == 1 ) && !defined( ipTCP_TIMER_PERIOD_MS ) )
	/* When initialising the TCP timer,
	give it an initial time-out of 1 second. */
	#define ipTCP_TIMER_PERIOD_MS	( 1000 )
#endif

/* If ipconfigETHERNET_DRIVER_FILTERS_FRAME_TYPES is set to 1, then the Ethernet
driver will filter incoming packets and only pass the stack those packets it
considers need processing.  In this case ipCONSIDER_FRAME_FOR_PROCESSING() can
be #defined away.  If ipconfigETHERNET_DRIVER_FILTERS_FRAME_TYPES is set to 0
then the Ethernet driver will pass all received packets to the stack, and the
stack must do the filtering itself.  In this case ipCONSIDER_FRAME_FOR_PROCESSING
needs to call eConsiderFrameForProcessing. */
#if ipconfigETHERNET_DRIVER_FILTERS_FRAME_TYPES == 0
	#define ipCONSIDER_FRAME_FOR_PROCESSING( pucEthernetBuffer ) eConsiderFrameForProcessing( ( pucEthernetBuffer ) )
#else
	#define ipCONSIDER_FRAME_FOR_PROCESSING( pucEthernetBuffer ) eProcessBuffer
#endif

/* The character used to fill ICMP echo requests, and therefore also the
character expected to fill ICMP echo replies. */
#define ipECHO_DATA_FILL_BYTE						'x'

#if( ipconfigETHERNET_DRIVER_FILTERS_PACKETS == 0 )
	#if( ipconfigBYTE_ORDER == pdFREERTOS_LITTLE_ENDIAN )
		/* The bits in the two byte IP header field that make up the fragment offset value. */
		#define ipFRAGMENT_OFFSET_BIT_MASK				( ( uint16_t ) 0xff0f )
	#else
		/* The bits in the two byte IP header field that make up the fragment offset value. */
		#define ipFRAGMENT_OFFSET_BIT_MASK				( ( uint16_t ) 0x0fff )
	#endif /* ipconfigBYTE_ORDER */
#endif /* ipconfigETHERNET_DRIVER_FILTERS_PACKETS */

/* The maximum time the IP task is allowed to remain in the Blocked state if no
events are posted to the network event queue. */
#ifndef	ipconfigMAX_IP_TASK_SLEEP_TIME
	#define ipconfigMAX_IP_TASK_SLEEP_TIME ( pdMS_TO_TICKS( 10000uL ) )
#endif

/* When a new TCP connection is established, the value of
'ulNextInitialSequenceNumber' will be used as the initial sequence number.  It
is very important that at start-up, 'ulNextInitialSequenceNumber' contains a
random value.  Also its value must be increased continuously in time, to prevent
a third party guessing the next sequence number and take-over a TCP connection.
It is advised to increment it by 1 ever 4us, which makes about 256 times
per ms: */
#define ipINITIAL_SEQUENCE_NUMBER_FACTOR	256UL

/* Returned as the (invalid) checksum when the protocol being checked is not
handled.  The value is chosen simply to be easy to spot when debugging. */
#define ipUNHANDLED_PROTOCOL		0x4321u

/* Returned to indicate a valid checksum when the checksum does not need to be
calculated. */
#define ipCORRECT_CRC				0xffffu

/* Returned as the (invalid) checksum when the length of the data being checked
had an invalid length. */
#define ipINVALID_LENGTH			0x1234u

/*-----------------------------------------------------------*/

/* Used in checksum calculation. */
typedef union _xUnion32
{
	uint32_t u32;
	uint16_t u16[ 2 ];
	uint8_t u8[ 4 ];
} xUnion32;

/* Used in checksum calculation. */
typedef union _xUnionPtr
{
	uint32_t *u32ptr;
	uint16_t *u16ptr;
	uint8_t *u8ptr;
} xUnionPtr;

/*-----------------------------------------------------------*/

/*
 * The main TCP/IP stack processing task.  This task receives commands/events
 * from the network hardware drivers and tasks that are using sockets.  It also
 * maintains a set of protocol timers.
 */
static void prvIPTask( void *pvParameters );

/*
 * Called when new data is available from the network interface.
 */
static void prvProcessEthernetPacket( NetworkBufferDescriptor_t * const pxNetworkBuffer );

/*
 * Process incoming IP packets.
 */
static eFrameProcessingResult_t prvProcessIPPacket( IPPacket_t * pxIPPacket, NetworkBufferDescriptor_t * const pxNetworkBuffer );

#if ( ipconfigREPLY_TO_INCOMING_PINGS == 1 ) || ( ipconfigSUPPORT_OUTGOING_PINGS == 1 )
	/*
	 * Process incoming ICMP packets.
	 */
	static eFrameProcessingResult_t prvProcessICMPPacket( ICMPPacket_t * const pxICMPPacket );
#endif /* ( ipconfigREPLY_TO_INCOMING_PINGS == 1 ) || ( ipconfigSUPPORT_OUTGOING_PINGS == 1 ) */

/*
 * Turns around an incoming ping request to convert it into a ping reply.
 */
#if ( ipconfigREPLY_TO_INCOMING_PINGS == 1 )
	static eFrameProcessingResult_t prvProcessICMPEchoRequest( ICMPPacket_t * const pxICMPPacket );
#endif /* ipconfigREPLY_TO_INCOMING_PINGS */

/*
 * Processes incoming ping replies.  The application callback function
 * vApplicationPingReplyHook() is called with the results.
 */
#if ( ipconfigSUPPORT_OUTGOING_PINGS == 1 )
	static void prvProcessICMPEchoReply( ICMPPacket_t * const pxICMPPacket );
#endif /* ipconfigSUPPORT_OUTGOING_PINGS */

/*
 * Called to create a network connection when the stack is first started, or
 * when the network connection is lost.
 */
static void prvProcessNetworkDownEvent( NetworkInterface_t *pxInterface );

/*
 * Checks the ARP, DHCP and TCP timers to see if any periodic or timeout
 * processing is required.
 */
static void prvCheckNetworkTimers( void );

/*
 * Determine how long the IP task can sleep for, which depends on when the next
 * periodic or timeout processing must be performed.
 */
static TickType_t prvCalculateSleepTime( void );

/*
 * The network card driver has received a packet.  In the case that it is part
 * of a linked packet chain, walk through it to handle every message.
 */
static void prvHandleEthernetPacket( NetworkBufferDescriptor_t *pxBuffer );

/*
 * Utility functions for the light weight IP timers.
 */
static void prvIPTimerStart( IPTimer_t *pxTimer, TickType_t xTime );
static BaseType_t prvIPTimerCheck( IPTimer_t *pxTimer );
static void prvIPTimerReload( IPTimer_t *pxTimer, TickType_t xTime );

/*_RB_ Comment required. */
static eFrameProcessingResult_t prvAllowIPPacketIPv4( const IPPacket_t * const pxIPPacket,
	NetworkBufferDescriptor_t * const pxNetworkBuffer, UBaseType_t uxHeaderLength );

#if( ipconfigUSE_IPv6 != 0 )
	eFrameProcessingResult_t prvAllowIPPacketIPv6( const IPHeader_IPv6_t * const pxIPv6Header,
		NetworkBufferDescriptor_t * const pxNetworkBuffer, UBaseType_t uxHeaderLength );
#endif

/*-----------------------------------------------------------*/

/* The queue used to pass events into the IP-task for processing. */
QueueHandle_t xNetworkEventQueue = NULL;/*lint !e9075*/

/*_RB_ Requires comment. */
uint16_t usPacketIdentifier = 0U;

/* For convenience, a MAC address of all 0xffs is defined const for quick
reference. */
const MACAddress_t xBroadcastMACAddress = { { 0xff, 0xff, 0xff, 0xff, 0xff, 0xff } };

/* Structure that stores the netmask, gateway address and DNS server addresses.
This will be obsolete when multiple interfaces are fully implemented, so will be
removed at that time. */
NetworkAddressingParameters_t xNetworkAddressing = { 0, 0, 0, 0, 0 };

/* Default values for the above struct in case DHCP
does not lead to a confirmed request. */
NetworkAddressingParameters_t xDefaultAddressing = { 0, 0, 0, 0, 0 };

/* Used to ensure network down events cannot be missed when they cannot be
posted to the network event queue because the network event queue is already
full. */
/*_RB_ Does this need to be an array so it can store more than one interface at a time? */
static NetworkInterface_t *pxNetworkDownEventPendingInterface = NULL;

/* Stores the handle of the task that handles the stack.  The handle is used
(indirectly) by some utility function to determine if the utility function is
being called by a task (in which case it is ok to block) or by the IP task
itself (in which case it is not ok to block). */
static TaskHandle_t xIPTaskHandle = NULL;

#if( ipconfigUSE_TCP != 0 )
	/* Set to a non-zero value if one or more TCP message have been processed
	within the last round. */
	static BaseType_t xProcessedTCPMessage;
#endif

/* 'xAllNetworksUp' becomes pdTRUE as soon as all network interfaces have
been initialised. */
static BaseType_t xAllNetworksUp = pdFALSE;

/* As long as not all networks are up, repeat initialisation by calling the
xNetworkInterfaceInitialise() function of the interfaces that are not ready. */
static IPTimer_t xNetworkTimer;

/*
A timer for each of the following processes, all of which need attention on a
regular basis:
	1. ARP, to check its table entries
	2. DPHC, to send requests and to renew a reservation
	3. TCP, to check for timeouts, resends
	4. DNS, to check for timeouts when looking-up a domain.
 */
static IPTimer_t xARPTimer;
#if( ipconfigUSE_TCP != 0 )
	static IPTimer_t xTCPTimer;
#endif
#if( ipconfigDNS_USE_CALLBACKS != 0 )
	static IPTimer_t xDNSTimer;
#endif

/* Set to pdTRUE when the IP task is ready to start processing packets. */
static BaseType_t xIPTaskInitialised = pdFALSE;

#if( ipconfigCHECK_IP_QUEUE_SPACE != 0 )
	/* Keep track of the lowest amount of space in 'xNetworkEventQueue'. */
	static UBaseType_t uxQueueMinimumSpace = ipconfigEVENT_QUEUE_LENGTH;
#endif

/*-----------------------------------------------------------*/

void nd_test();
BaseType_t call_nd_test;

static void prvIPTask( void *pvParameters )
{
IPStackEvent_t xReceivedEvent;
TickType_t xNextIPSleep;
FreeRTOS_Socket_t *pxSocket;
#if( ipconfigUSE_IPv6 != 0 )
	struct freertos_sockaddr6 xAddress;
#else
	struct freertos_sockaddr xAddress;
#endif
NetworkInterface_t *pxInterface;

eventLogAdd("prvIPTask");

	/* Just to prevent compiler warnings about unused parameters. */
	( void ) pvParameters;

	/* A possibility to set some additional task properties. */
	iptraceIP_TASK_STARTING();

	/* Generate a dummy message to say that the network connection has gone
	down.  This will cause this task to initialise the network interface.  After
	this it is the responsibility of the network interface hardware driver to
	send this message if a previously connected network is disconnected. */

	prvIPTimerReload( &( xNetworkTimer ), pdMS_TO_TICKS( ipINITIALISATION_RETRY_DELAY ) );
	for( pxInterface = pxNetworkInterfaces; pxInterface != NULL; pxInterface = pxInterface->pxNext )
	{
		/* Post a 'eNetworkDownEvent' for every interface. */
		FreeRTOS_NetworkDown( pxInterface );
	}

	#if( ipconfigUSE_TCP == 1 )
	{
		/* Initialise the TCP timer. */
		prvIPTimerReload( &xTCPTimer, pdMS_TO_TICKS( ipTCP_TIMER_PERIOD_MS ) );
	}
	#endif

	#if( ipconfigDNS_USE_CALLBACKS != 0 )
	{
		/* The following function is declared in FreeRTOS_DNS.c	and 'private' to
		this library */
		vDNSInitialise();
	}
	#endif /* ipconfigDNS_USE_CALLBACKS != 0 */

	/* Initialisation is complete and events can now be processed. */
	xIPTaskInitialised = pdTRUE;

	FreeRTOS_debug_printf( ( "prvIPTask started\n" ) );

eventLogAdd("prvIPTask start");
	/* Loop, processing IP events. */
	for( ;; )
	{
		ipconfigWATCHDOG_TIMER();

		/* Check the ARP, DHCP and TCP timers to see if there is any periodic
		or timeout processing to perform. */
		prvCheckNetworkTimers();

		/* Calculate the acceptable maximum sleep time. */
		xNextIPSleep = prvCalculateSleepTime();

		/* Wait until there is something to do. If the following call exits
		 * due to a time out rather than a message being received, set a
		 * 'NoEvent' value. */
		if( xQueueReceive( xNetworkEventQueue, &xReceivedEvent, xNextIPSleep ) == pdFALSE )
		{
			xReceivedEvent.eEventType = eNoEvent;
		}

if (call_nd_test) {
	call_nd_test--;
	nd_test();
}

		#if( ipconfigCHECK_IP_QUEUE_SPACE != 0 )
		{
			if( xReceivedEvent.eEventType != eNoEvent )
			{
			UBaseType_t uxCount;

				uxCount = uxQueueSpacesAvailable( xNetworkEventQueue );
				if( uxQueueMinimumSpace > uxCount )
				{
					uxQueueMinimumSpace = uxCount;
				}
			}
		}
		#endif /* ipconfigCHECK_IP_QUEUE_SPACE */

		iptraceNETWORK_EVENT_RECEIVED( xReceivedEvent.eEventType );

		switch( xReceivedEvent.eEventType )
		{
			case eNetworkDownEvent :
				/* Attempt to establish a connection. */
				prvProcessNetworkDownEvent( ipPOINTER_CAST( NetworkInterface_t *, xReceivedEvent.pvData ) );
				break;

			case eNetworkRxEvent:
				/* The network hardware driver has received a new packet.  A
				pointer to the received buffer is located in the pvData member
				of the received event structure. */
				prvHandleEthernetPacket( ipPOINTER_CAST( NetworkBufferDescriptor_t *, xReceivedEvent.pvData ) );
				break;

			case eARPTimerEvent :
				/* The ARP timer has expired, process the ARP cache. */
				vARPAgeCache();
				break;

			case eSocketBindEvent:
				/* FreeRTOS_bind (a user API) wants the IP-task to bind a socket
				to a port. The port number is communicated in the socket field
				usLocalPort. vSocketBind() will actually bind the socket and the
				API will unblock as soon as the eSOCKET_BOUND event is
				triggered. */
				pxSocket = ipPOINTER_CAST( FreeRTOS_Socket_t *, xReceivedEvent.pvData );
				xAddress.sin_len = sizeof( xAddress );
				#if( ipconfigUSE_IPv6 != 0 )
				if( pxSocket->bits.bIsIPv6 != pdFALSE_UNSIGNED )
				{
					xAddress.sin_family = FREERTOS_AF_INET6;
					memcpy( xAddress.sin_addrv6.ucBytes, pxSocket->xLocalAddress_IPv6.ucBytes, sizeof( xAddress.sin_addrv6.ucBytes ) );
				}
				else
				#endif
				{
				struct freertos_sockaddr *pxAddress = ( struct freertos_sockaddr * )&xAddress;

					pxAddress->sin_family = FREERTOS_AF_INET;
					pxAddress->sin_addr = FreeRTOS_htonl( pxSocket->ulLocalAddress );
				}
				xAddress.sin_port = FreeRTOS_htons( pxSocket->usLocalPort );
				/* 'ulLocalAddress' and 'usLocalPort' will be set again by vSocketBind(). */
				pxSocket->ulLocalAddress = 0;
				pxSocket->usLocalPort = 0;
				( void ) vSocketBind( pxSocket, ipPOINTER_CAST( struct freertos_sockaddr *, &xAddress ), sizeof( xAddress ), pdFALSE );

				/* Before 'eSocketBindEvent' was sent it was tested that
				( xEventGroup != NULL ) so it can be used now to wake up the
				user. */
				pxSocket->xEventBits |= ( EventBits_t ) eSOCKET_BOUND;
				vSocketWakeUpUser( pxSocket );
				break;

			case eSocketCloseEvent :
				/* The user API FreeRTOS_closesocket() has sent a message to the
				IP-task to actually close a socket. This is handled in
				vSocketClose().  As the socket gets closed, there is no way to
				report back to the API, so the API won't wait for the result */
				( void ) vSocketClose( ipPOINTER_CAST( FreeRTOS_Socket_t *, xReceivedEvent.pvData ) );
				break;

			case eStackTxEvent :
				/* The network stack has generated a packet to send.  A
				pointer to the generated buffer is located in the pvData
				member of the received event structure. */
				vProcessGeneratedUDPPacket( ipPOINTER_CAST( NetworkBufferDescriptor_t *, xReceivedEvent.pvData ) );
				break;

			case eDHCPEvent:
				/* The DHCP state machine needs processing. */
				#if( ipconfigUSE_DHCP == 1 )
				{
					/* Process DHCP messages for a given end-point. */
					vDHCPProcess( pdFALSE, ipPOINTER_CAST( NetworkEndPoint_t *, xReceivedEvent.pvData ) );
				}
				#endif /* ipconfigUSE_DHCP */
				break;

			case eSocketSelectEvent :
				/* FreeRTOS_select() has got unblocked by a socket event,
				vSocketSelect() will check which sockets actually have an event
				and update the socket field xSocketBits. */
				#if( ipconfigSUPPORT_SELECT_FUNCTION == 1 )
				{
					vSocketSelect( ipPOINTER_CAST( SocketSelect_t *, xReceivedEvent.pvData ) );
				}
				#endif /* ipconfigSUPPORT_SELECT_FUNCTION == 1 */
				break;

			case eSocketSignalEvent :
				#if( ipconfigSUPPORT_SIGNALS != 0 )
				{
					/* Some task wants to signal the user of this socket in
					order to interrupt a call to recv() or a call to select(). */
					( void ) FreeRTOS_SignalSocket( ipPOINTER_CAST( Socket_t, xReceivedEvent.pvData ) );
				}
				#endif /* ipconfigSUPPORT_SIGNALS */
				break;

			case eTCPTimerEvent :
				#if( ipconfigUSE_TCP == 1 )
				{
					/* Simply mark the TCP timer as expired so it gets processed
					the next time prvCheckNetworkTimers() is called. */
					xTCPTimer.bExpired = pdTRUE_UNSIGNED;
				}
				#endif /* ipconfigUSE_TCP */
				break;

			case eTCPAcceptEvent:
				/* The API FreeRTOS_accept() was called, the IP-task will now
				check if the listening socket (communicated in pvData) actually
				received a new connection. */
				#if( ipconfigUSE_TCP == 1 )
				{
					pxSocket = ipPOINTER_CAST( FreeRTOS_Socket_t *, xReceivedEvent.pvData );

					if( xTCPCheckNewClient( pxSocket ) != pdFALSE )
					{
						pxSocket->xEventBits |= ( EventBits_t ) eSOCKET_ACCEPT;
						vSocketWakeUpUser( pxSocket );
					}
				}
				#endif /* ipconfigUSE_TCP */
				break;

			case eTCPNetStat:
				/* FreeRTOS_netstat() was called to have the IP-task print an
				overview of all sockets and their connections */
				#if( ( ipconfigUSE_TCP == 1 ) && ( ipconfigHAS_PRINTF == 1 ) )
				{
					vTCPNetStat();
				}
				#endif /* ipconfigUSE_TCP */
				break;

			case eNoEvent:
				/* xQueueReceive() returned because of a normal time-out. */
				break;

			default :
				/* Should not get here. */
				break;
		}

		if( pxNetworkDownEventPendingInterface != NULL )
		{
			/* A network down event could not be posted to the network event
			queue because the queue was full.  Try posting again. */
			FreeRTOS_NetworkDown( pxNetworkDownEventPendingInterface );
		}
	}
}
/*-----------------------------------------------------------*/

BaseType_t xIsCallingFromIPTask( void )
{
BaseType_t xReturn;

	if( xTaskGetCurrentTaskHandle() == xIPTaskHandle )
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

static void prvHandleEthernetPacket( NetworkBufferDescriptor_t *pxBuffer )
{
	#if( ipconfigUSE_LINKED_RX_MESSAGES == 0 )
	{
		/* When ipconfigUSE_LINKED_RX_MESSAGES is not set to 0 then only one
		buffer will be sent at a time.  This is the default way for +TCP to pass
		messages from the MAC to the TCP/IP stack. */
		prvProcessEthernetPacket( pxBuffer );
	}
	#else /* ipconfigUSE_LINKED_RX_MESSAGES */
	{
	NetworkBufferDescriptor_t *pxNextBuffer;

		/* An optimisation that is useful when there is high network traffic.
		Instead of passing received packets into the IP task one at a time the
		network interface can chain received packets together and pass them into
		the IP task in one go.  The packets are chained using the pxNextBuffer
		member.  The loop below walks through the chain processing each packet
		in the chain in turn. */
		do
		{
			/* Store a pointer to the buffer after pxBuffer for use later on. */
			pxNextBuffer = pxBuffer->pxNextBuffer;

			/* Make it NULL to avoid using it later on. */
			pxBuffer->pxNextBuffer = NULL;

			prvProcessEthernetPacket( pxBuffer );
			pxBuffer = pxNextBuffer;

		/* While there is another packet in the chain. */
		} while( pxBuffer != NULL );
	}
	#endif /* ipconfigUSE_LINKED_RX_MESSAGES */
}
/*-----------------------------------------------------------*/

static TickType_t prvCalculateSleepTime( void )
{
TickType_t xMaximumSleepTime;

	/* Start with the maximum sleep time, then check this against the remaining
	time in any other timers that are active. */
	xMaximumSleepTime = ipconfigMAX_IP_TASK_SLEEP_TIME;

	if( xARPTimer.bActive != pdFALSE_UNSIGNED )
	{
		if( xARPTimer.ulRemainingTime < xMaximumSleepTime )
		{
			xMaximumSleepTime = xARPTimer.ulReloadTime;
		}
	}

	#if( ipconfigUSE_DHCP == 1 )
	{
	NetworkEndPoint_t *pxEndPoint = pxNetworkEndPoints;

		while( pxEndPoint != NULL )
		{
			if( pxEndPoint->xDHCPTimer.bActive != pdFALSE )
			{
				if( pxEndPoint->xDHCPTimer.ulRemainingTime < xMaximumSleepTime )
				{
					xMaximumSleepTime = pxEndPoint->xDHCPTimer.ulRemainingTime;
				}
			}
			pxEndPoint = pxEndPoint->pxNext;
		}
	}
	#endif /* ipconfigUSE_DHCP */

	#if( ipconfigUSE_TCP == 1 )
	{
		if( xTCPTimer.ulRemainingTime < xMaximumSleepTime )
		{
			xMaximumSleepTime = xTCPTimer.ulRemainingTime;
		}
	}
	#endif

	#if( ipconfigDNS_USE_CALLBACKS != 0 )
	{
		if( xDNSTimer.bActive != pdFALSE )
		{
			if( xDNSTimer.ulRemainingTime < xMaximumSleepTime )
			{
				xMaximumSleepTime = xDNSTimer.ulRemainingTime;
			}
		}
	}
	#endif

	return xMaximumSleepTime;
}
/*-----------------------------------------------------------*/

static void prvCheckNetworkTimers( void )
{
NetworkInterface_t *pxInterface;

	/* Is it time for ARP processing? */
	if( prvIPTimerCheck( &xARPTimer ) != pdFALSE )
	{
		( void ) xSendEventToIPTask( eARPTimerEvent );
	}

	#if( ipconfigUSE_DHCP == 1 )
	{
	/* Is it time for DHCP processing? */
	NetworkEndPoint_t *pxEndPoint = pxNetworkEndPoints;

		while( pxEndPoint != NULL )
		{
			if( prvIPTimerCheck( &( pxEndPoint->xDHCPTimer ) ) != pdFALSE )
			{
			IPStackEvent_t xEventMessage;
			const TickType_t xDontBlock = 0;

				xEventMessage.eEventType = eDHCPEvent;
				xEventMessage.pvData = pxEndPoint;

				( void ) xSendEventStructToIPTask( &xEventMessage, xDontBlock );
			}

			pxEndPoint = pxEndPoint->pxNext;
		}
	}
	#endif /* ipconfigUSE_DHCP */

	#if( ipconfigDNS_USE_CALLBACKS != 0 )
	{
		/* Is it time for DNS processing? */
		if( prvIPTimerCheck( &xDNSTimer ) != pdFALSE )
		{
			vDNSCheckCallBack( NULL );
		}
	}
	#endif /* ipconfigDNS_USE_CALLBACKS */

	#if( ipconfigUSE_TCP == 1 )
	{
	BaseType_t xWillSleep;
	/* xStart keeps a copy of the last time this function was active,
	and during every call it will be updated with xTaskGetTickCount()
	'0' means: not yet initialised (although later '0' might be returned
	by xTaskGetTickCount(), which is no problem). */
	static TickType_t xStart = ( TickType_t ) 0;
	TickType_t xTimeNow, xNextTime;
	BaseType_t xCheckTCPSockets;

		/* If the IP task has messages waiting to be processed then it will not
		sleep in any case. */
		if( uxQueueMessagesWaiting( xNetworkEventQueue ) == 0u )
		{
			xWillSleep = pdTRUE;
		}
		else
		{
			xWillSleep = pdFALSE;
		}

		xTimeNow = xTaskGetTickCount();

		if( xStart != ( TickType_t ) 0 )
		{
			/* It is advised to increment the Initial Sequence Number every 4
			microseconds which makes 250 times per ms.  This will make it harder
			for a third party to 'guess' our sequence number and 'take over'
			a TCP connection */
			ulNextInitialSequenceNumber += ipINITIAL_SEQUENCE_NUMBER_FACTOR * ( ( xTimeNow - xStart ) * portTICK_PERIOD_MS );
		}

		xStart = xTimeNow;

		/* Sockets need to be checked if the TCP timer has expired. */
		xCheckTCPSockets = prvIPTimerCheck( &xTCPTimer );

		/* Sockets will also be checked if there are TCP messages but the
		message queue is empty (indicated by xWillSleep being true). */
		if( ( xProcessedTCPMessage != pdFALSE ) && ( xWillSleep != pdFALSE ) )
		{
			xCheckTCPSockets = pdTRUE;
		}

		if( xCheckTCPSockets != pdFALSE )
		{
			/* Attend to the sockets, returning the period after which the
			check must be repeated. */
			xNextTime = xTCPTimerCheck( xWillSleep );
			prvIPTimerStart( &xTCPTimer, xNextTime );
			xProcessedTCPMessage = 0;
		}
	}
	#endif /* ipconfigUSE_TCP == 1 */

	/* Is it time to trigger the repeated NetworkDown events? */
	if( xAllNetworksUp == pdFALSE )
	{
		if( prvIPTimerCheck( &( xNetworkTimer ) ) != pdFALSE )
		{
		BaseType_t xUp = pdTRUE;

			for( pxInterface = pxNetworkInterfaces; pxInterface != NULL; pxInterface = pxInterface->pxNext )
			{
				if( pxInterface->bits.bInterfaceUp == pdFALSE_UNSIGNED )
				{
					xUp = pdFALSE;
					FreeRTOS_NetworkDown( pxInterface );
				}
			}
			xAllNetworksUp = xUp;
		}
	}
}
/*-----------------------------------------------------------*/

static void prvIPTimerStart( IPTimer_t *pxTimer, TickType_t xTime )
{
	vTaskSetTimeOutState( &pxTimer->xTimeOut );
	pxTimer->ulRemainingTime = xTime;

	if( xTime == ( TickType_t ) 0 )
	{
		pxTimer->bExpired = pdTRUE_UNSIGNED;
	}
	else
	{
		pxTimer->bExpired = pdFALSE_UNSIGNED;
	}

	pxTimer->bActive = pdTRUE_UNSIGNED;
}
/*-----------------------------------------------------------*/

static void prvIPTimerReload( IPTimer_t *pxTimer, TickType_t xTime )
{
	pxTimer->ulReloadTime = xTime;
	prvIPTimerStart( pxTimer, xTime );
}
/*-----------------------------------------------------------*/

static BaseType_t prvIPTimerCheck( IPTimer_t *pxTimer )
{
BaseType_t xReturn;

	if( pxTimer->bActive == pdFALSE_UNSIGNED )
	{
		/* The timer is not enabled. */
		xReturn = pdFALSE;
	}
	else
	{
		/* The timer might have set the bExpired flag already, if not, check the
		value of xTimeOut against ulRemainingTime. */
		if( pxTimer->bExpired == pdFALSE_UNSIGNED )
		{
			// BaseType_t xTaskCheckForTimeOut( TimeOut_t * const pxTimeOut, TickType_t * const pxTicksToWait ) PRIVILEGED_FUNCTION;
			pxTimer->bExpired = ( uint32_t ) xTaskCheckForTimeOut( &( pxTimer->xTimeOut ), &( pxTimer->ulRemainingTime ) );
		}
		if( pxTimer->bExpired != pdFALSE_UNSIGNED )
		{
			prvIPTimerStart( pxTimer, pxTimer->ulReloadTime );
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

void FreeRTOS_NetworkDown( struct xNetworkInterface *pxNetworkInterface )
{
IPStackEvent_t xNetworkDownEvent;
const TickType_t xDontBlock = 0;

	pxNetworkInterface->bits.bInterfaceUp = pdFALSE_UNSIGNED;
	xNetworkDownEvent.eEventType = eNetworkDownEvent;
	xNetworkDownEvent.pvData = pxNetworkInterface;

	/* Simply send the network task the appropriate event. */
	if( xSendEventStructToIPTask( &xNetworkDownEvent, xDontBlock ) != pdPASS )
	{
		/* Could not send the message, so it is still pending. */
		pxNetworkDownEventPendingInterface = pxNetworkInterface;
	}
	else
	{
		/* Message was sent so it is not pending. */
		pxNetworkDownEventPendingInterface = NULL;
	}

	iptraceNETWORK_DOWN();
}
/*-----------------------------------------------------------*/

BaseType_t FreeRTOS_NetworkDownFromISR( struct xNetworkInterface *pxNetworkInterface )
{
static IPStackEvent_t xNetworkDownEvent;
BaseType_t xHigherPriorityTaskWoken = pdFALSE;

	xNetworkDownEvent.eEventType = eNetworkDownEvent;
	xNetworkDownEvent.pvData = pxNetworkInterface;

	/* Simply send the network task the appropriate event. */
	if( xQueueSendToBackFromISR( xNetworkEventQueue, &xNetworkDownEvent, &xHigherPriorityTaskWoken ) != pdPASS )
	{
		pxNetworkDownEventPendingInterface = pxNetworkInterface;
	}
	else
	{
		pxNetworkDownEventPendingInterface = NULL;
	}

	iptraceNETWORK_DOWN();

	return xHigherPriorityTaskWoken;
}
/*-----------------------------------------------------------*/

#if( ipconfigUSE_IPv6 != 0 )
/* The last parameter is either ipTYPE_IPv4 (0x40) or ipTYPE_IPv6 (0x60) */
void * FreeRTOS_GetUDPPayloadBuffer( size_t xRequestedSizeBytes, TickType_t xBlockTimeTicks, uint8_t ucIPType )
#else
void * FreeRTOS_GetUDPPayloadBuffer( size_t xRequestedSizeBytes, TickType_t xBlockTimeTicks )
#endif
{
NetworkBufferDescriptor_t *pxNetworkBuffer;
void *pvReturn;

	/* Cap the block time.  The reason for this is explained where
	ipconfigUDP_MAX_SEND_BLOCK_TIME_TICKS is defined (assuming an official
	FreeRTOSIPConfig.h header file is being used). */
	if( xBlockTimeTicks > ipconfigUDP_MAX_SEND_BLOCK_TIME_TICKS )
	{
		xBlockTimeTicks = ipconfigUDP_MAX_SEND_BLOCK_TIME_TICKS;/*lint !e9044 function parameter modified [MISRA 2012 Rule 17.8, advisory] */
	}

	/* Obtain a network buffer with the required amount of storage. */
	pxNetworkBuffer = pxGetNetworkBufferWithDescriptor( sizeof( UDPPacket_t ) + xRequestedSizeBytes, xBlockTimeTicks );

	if( pxNetworkBuffer != NULL )
	{
		#if( ipconfigUSE_IPv6 != 0 )
		{
		uint8_t *pucIPType;

			/* Skip 3 headers. */
			pvReturn = ( void * ) ( pxNetworkBuffer->pucEthernetBuffer +
				ipSIZE_OF_ETH_HEADER + xIPHeaderSize( pxNetworkBuffer ) + ipSIZE_OF_UDP_HEADER );
			/* Later a pointer to a UDP payload is used to retrieve a NetworkBuffer.
			Store the packet type at 48 bytes before the start of the UDP payload. */
			pucIPType = ( ( uint8_t * ) pvReturn ) - ipUDP_PAYLOAD_IP_TYPE_OFFSET;
			*pucIPType = ucIPType;
		}
		#else
		{
			/* Skip 3 headers. */
			pvReturn = &( pxNetworkBuffer->pucEthernetBuffer [ ipSIZE_OF_ETH_HEADER + ipSIZE_OF_IPv4_HEADER + ipSIZE_OF_UDP_HEADER ] );
		}
		#endif
	}
	else
	{
		pvReturn = NULL;
	}

	return ( void * ) pvReturn;
}
/*-----------------------------------------------------------*/

NetworkBufferDescriptor_t *pxDuplicateNetworkBufferWithDescriptor( NetworkBufferDescriptor_t * const pxNetworkBuffer,
	size_t uxNewLength )
{
NetworkBufferDescriptor_t * pxNewBuffer;

	/* This function is only used when 'ipconfigZERO_COPY_TX_DRIVER' is set to 1.
	The transmit routine wants to have ownership of the network buffer
	descriptor, because it will pass the buffer straight to DMA. */
	pxNewBuffer = pxGetNetworkBufferWithDescriptor( uxNewLength, ( TickType_t ) 0 );

	if( pxNewBuffer != NULL )
	{
		pxNewBuffer->ulIPAddress = pxNetworkBuffer->ulIPAddress;
		pxNewBuffer->usPort = pxNetworkBuffer->usPort;
		pxNewBuffer->usBoundPort = pxNetworkBuffer->usBoundPort;
		pxNewBuffer->pxEndPoint = pxNetworkBuffer->pxEndPoint;
		memcpy( pxNewBuffer->pucEthernetBuffer, pxNetworkBuffer->pucEthernetBuffer, pxNetworkBuffer->xDataLength );
	}

	return pxNewBuffer;
}
/*-----------------------------------------------------------*/

#if( ipconfigZERO_COPY_TX_DRIVER != 0 ) || ( ipconfigZERO_COPY_RX_DRIVER != 0 )

	NetworkBufferDescriptor_t *pxPacketBuffer_to_NetworkBuffer( const void *pvBuffer )
	{
	uint8_t *pucBuffer;
	NetworkBufferDescriptor_t *pxResult;

		if( pvBuffer == NULL )
		{
			pxResult = NULL;
		}
		else
		{
			/* Obtain the network buffer from the zero copy pointer. */
			pucBuffer = ipPOINTER_CAST( uint8_t *, pvBuffer );

			/* The input here is a pointer to a payload buffer.  Subtract the
			size of the header in the network buffer, usually 8 + 2 bytes. */
			pucBuffer -= ipBUFFER_PADDING;

			/* Here a pointer was placed to the network descriptor.  As a
			pointer is dereferenced, make sure it is well aligned. */
			if( ( ( ( size_t ) pucBuffer ) & ( sizeof( pucBuffer ) - 1u ) ) == ( size_t ) 0u )	/*lint !e9078 !e923*/
			{
				pxResult = * ( ipPOINTER_CAST( NetworkBufferDescriptor_t **, pucBuffer ) );
			}
			else
			{
				pxResult = NULL;
			}
		}

		return pxResult;
	}

#endif /* ipconfigZERO_COPY_TX_DRIVER != 0 */
/*-----------------------------------------------------------*/

NetworkBufferDescriptor_t *pxUDPPayloadBuffer_to_NetworkBuffer( void *pvBuffer )
{
uint8_t *pucBuffer;
NetworkBufferDescriptor_t *pxResult;

	if( pvBuffer == NULL )
	{
		pxResult = NULL;
	}
	else
	{
		/* The input here is a pointer to a payload buffer.  Subtract
		the total size of a UDP/IP header plus the size of the header in
		the network buffer, usually 8 + 2 bytes. */
		#if( ipconfigUSE_IPv6 != 0 )
		{
		uint8_t *pucIPType;
		uint8_t ucIPType;
			/* When IPv6 is supported, find out the type of the packet.
			It is stored 48 bytes before the payload buffer as 0x40 or 0x60. */
			pucIPType = ( ( uint8_t * ) pvBuffer ) - ipUDP_PAYLOAD_IP_TYPE_OFFSET;
			ucIPType = *pucIPType & 0xf0;

			/* To help the translation from a UDP payload pointer to a networkBuffer,
			a byte was stored at a certain negative offset (-48 bytes).
			It must have a value of either 0x4x or 0x6x. */
			configASSERT( ( ucIPType == ipTYPE_IPv4 ) || ( ucIPType == ipTYPE_IPv6 ) );

			if( ucIPType == ipTYPE_IPv6 )
			{
				pucBuffer = ( ( uint8_t *) pvBuffer ) - ( sizeof( UDPPacket_IPv6_t ) + ipBUFFER_PADDING );
			}
			else /* ucIPType == ipTYPE_IPv4 */
			{
				pucBuffer = ( ( uint8_t *) pvBuffer ) - ( sizeof( UDPPacket_t ) + ipBUFFER_PADDING );
			}
		}
		#else
		{
			/* Obtain the network buffer from the zero copy pointer. */
			pucBuffer = ipPOINTER_CAST( uint8_t *, pvBuffer );

			/* The input here is a pointer to a payload buffer.  Subtract
		the total size of a UDP/IP header plus the size of the header in
			the network buffer, usually 8 + 2 bytes. */
			pucBuffer -= ( sizeof( UDPPacket_t ) + ipBUFFER_PADDING );
		}
		#endif /* ipconfigUSE_IPv6 */

		/* Here a pointer was placed to the network descriptor,
		As a pointer is dereferenced, make sure it is well aligned */
		if( ( ( ( size_t ) pucBuffer ) & ( sizeof( pucBuffer ) - 1 ) ) == 0 )	/*lint !e9078 !e923*/
		{
			/* The following statement may trigger a:
			warning: cast increases required alignment of target type [-Wcast-align].
			It has been confirmed though that the alignment is suitable. */
			pxResult = * ( ipPOINTER_CAST( NetworkBufferDescriptor_t **, pucBuffer ) );
		}
		else
		{
			pxResult = NULL;
		}
	}

	return pxResult;
}
/*-----------------------------------------------------------*/

uint8_t *pcNetworkBuffer_to_UDPPayloadBuffer( NetworkBufferDescriptor_t *pxNetworkBuffer )
{
uint8_t *pcResult;
	#if( ipconfigUSE_IPv6 != 0 )
	{
		if( ( ( EthernetHeader_t * ) ( pxNetworkBuffer->pucEthernetBuffer ) )->usFrameType == ipIPv6_FRAME_TYPE )
		{
			pcResult = pxNetworkBuffer->pucEthernetBuffer + ipUDP_PAYLOAD_OFFSET_IPv6;
		}
		else
		{
			pcResult = pxNetworkBuffer->pucEthernetBuffer + ipUDP_PAYLOAD_OFFSET_IPv4;
		}
	}
	#else
	{
		pcResult = &( pxNetworkBuffer->pucEthernetBuffer[ ipUDP_PAYLOAD_OFFSET_IPv4 ] );
	}
	#endif

	return pcResult;
}
/*-----------------------------------------------------------*/

void FreeRTOS_ReleaseUDPPayloadBuffer( void *pvBuffer )
{
NetworkBufferDescriptor_t *pxBuffer;

	pxBuffer = pxUDPPayloadBuffer_to_NetworkBuffer( pvBuffer );
	configASSERT( pxBuffer != NULL );
	vReleaseNetworkBufferAndDescriptor( pxBuffer );
}
/*-----------------------------------------------------------*/

BaseType_t FreeRTOS_IPStart( void )
{
BaseType_t xReturn = pdFALSE;
NetworkInterface_t *pxFirstNetwork;
NetworkEndPoint_t *pxFirstEndPoint, *pxEndPoint;

eventLogAdd("IPStart");
	/* There must be at least one interface and one end-point. */
	pxFirstNetwork = FreeRTOS_FirstNetworkInterface();
	configASSERT( pxFirstNetwork != NULL );

	pxFirstEndPoint = FreeRTOS_FirstEndPoint( pxFirstNetwork );
	configASSERT( pxFirstEndPoint != NULL );

	/* This function should only be called once. */
	configASSERT( xIPIsNetworkTaskReady() == pdFALSE );
	configASSERT( xNetworkEventQueue == NULL );
	configASSERT( xIPTaskHandle == NULL );

	#ifndef _lint
	{
		/* Check structure packing is correct. */
		configASSERT( sizeof( EthernetHeader_t ) == ipEXPECTED_EthernetHeader_t_SIZE );
		configASSERT( sizeof( ARPHeader_t ) == ipEXPECTED_ARPHeader_t_SIZE );
		configASSERT( sizeof( IPHeader_t ) == ipEXPECTED_IPHeader_t_SIZE );
		configASSERT( sizeof( ICMPHeader_t ) == ipEXPECTED_ICMPHeader_t_SIZE );
		configASSERT( sizeof( UDPHeader_t ) == ipEXPECTED_UDPHeader_t_SIZE );
		#if ipconfigUSE_TCP == 1
		{
			configASSERT( sizeof( TCPHeader_t ) == ( ipEXPECTED_TCPHeader_t_SIZE + ipSIZE_TCP_OPTIONS ) );
		}
		#endif
		configASSERT( sizeof( ICMPHeader_t ) == ipEXPECTED_ICMPHeader_t_SIZE );
		configASSERT( sizeof( IGMPHeader_t ) == ipEXPECTED_IGMPHeader_t_SIZE );
	}
	#endif
	/* Attempt to create the queue used to communicate with the IP task. */
	xNetworkEventQueue = xQueueCreate( ( UBaseType_t ) ipconfigEVENT_QUEUE_LENGTH, ( UBaseType_t )sizeof( IPStackEvent_t ) );
	configASSERT( xNetworkEventQueue );

	if( xNetworkEventQueue != NULL )
	{
		#if ( configQUEUE_REGISTRY_SIZE > 0 )
		{
			/* A queue registry is normally used to assist a kernel aware
			debugger.  If one is in use then it will be helpful for the debugger
			to show information about the network event queue. */
			vQueueAddToRegistry( xNetworkEventQueue, "NetEvnt" );
		}
		#endif /* configQUEUE_REGISTRY_SIZE */

		if( xNetworkBuffersInitialise() == pdPASS )
		{
			/* _HT_ : 'xNetworkAddressing' obsolete since the multi version. */
			/* Store the local IP and MAC address.
			'xNetworkAddressing' will be dropped in the next release. */
			xNetworkAddressing.ulDefaultIPAddress = pxFirstEndPoint->ulDefaultIPAddress;
			xNetworkAddressing.ulNetMask = pxFirstEndPoint->ulNetMask;
			xNetworkAddressing.ulDNSServerAddress = pxFirstEndPoint->ulDNSServerAddresses[ 0 ];
			xNetworkAddressing.ulBroadcastAddress = pxFirstEndPoint->ulBroadcastAddress;

			memcpy( &xDefaultAddressing, &xNetworkAddressing, sizeof( xDefaultAddressing ) );

			/* Start with each endpoint using its default IP address. */
			pxEndPoint = pxFirstEndPoint;
			while( pxEndPoint != NULL )
			{
			#if( ipconfigUSE_DHCP == 1 )
				if( pxEndPoint->bits.bWantDHCP != pdFALSE_UNSIGNED )
				{
					pxEndPoint->ulIPAddress = pxEndPoint->ulDefaultIPAddress;
				}
				else
			#endif
				{
					pxEndPoint->ulIPAddress = pxEndPoint->ulDefaultIPAddress;
				}

				if( pxEndPoint->ulGatewayAddress != 0uL )
				{
					xNetworkAddressing.ulGatewayAddress = pxEndPoint->ulGatewayAddress;
				}
				pxEndPoint = pxEndPoint->pxNext;
			}

			#if ipconfigUSE_DHCP == 1
			{
				/* _HT_ : 'ipLOCAL_IP_ADDRESS_POINTER' obsolete since the multi version. */
				/* The IP address is not set until DHCP completes. */
				*ipLOCAL_IP_ADDRESS_POINTER = 0x00uL;
			}
			#else
			{
				/* The IP address is set from the value passed in.
				_RB_ As _HT_ comments, ipLOCAL_IP_ADDRESS_POINTER will be
				obsolete when multiple interfaces are fully supported. */
				*ipLOCAL_IP_ADDRESS_POINTER = xNetworkAddressing.ulDefaultIPAddress;

				/* Added to prevent ARP flood to gateway.  Ensure the
				gateway is on the same subnet as the IP	address. */
//				configASSERT( ( ( *ipLOCAL_IP_ADDRESS_POINTER ) & xNetworkAddressing.ulNetMask ) == ( xNetworkAddressing.ulGatewayAddress & xNetworkAddressing.ulNetMask ) );
			}
			#endif /* ipconfigUSE_DHCP == 1 */

			/* The MAC address is stored in the start of the default packet
			header fragment, which is used when sending UDP packets. */
			/*_RB_ Does this need replicating for all end points? */
			memcpy( ipLOCAL_MAC_ADDRESS, pxFirstEndPoint->xMACAddress.ucBytes, ( size_t ) ipMAC_ADDRESS_LENGTH_BYTES );

			/* Prepare the sockets interface. */
			vNetworkSocketsInit();

			/* Create the task that processes Ethernet and stack events. */
			xReturn = xTaskCreate( prvIPTask, "IP-task", ( uint16_t ) ipconfigIP_TASK_STACK_SIZE_WORDS, NULL, ( UBaseType_t ) ipconfigIP_TASK_PRIORITY, &xIPTaskHandle );
		}
		else
		{
			FreeRTOS_debug_printf( ( "FreeRTOS_IPStart: xNetworkBuffersInitialise() failed\n") );

			/* Clean up. */
			vQueueDelete( xNetworkEventQueue );
			xNetworkEventQueue = NULL;
		}
	}
	else
	{
		FreeRTOS_debug_printf( ( "FreeRTOS_IPStart: Network event queue could not be created\n") );
	}

	return xReturn;
}
/*-----------------------------------------------------------*/

void FreeRTOS_SetAddressConfiguration( const uint32_t *pulIPAddress, const uint32_t *pulNetMask, const uint32_t *pulGatewayAddress, const uint32_t *pulDNSServerAddress )
{
	/* Update the address configuration. */

	if( pulIPAddress != NULL )
	{
		*ipLOCAL_IP_ADDRESS_POINTER = *pulIPAddress;
	}

	if( pulNetMask != NULL )
	{
		xNetworkAddressing.ulNetMask = *pulNetMask;
	}

	if( pulGatewayAddress != NULL )
	{
		xNetworkAddressing.ulGatewayAddress = *pulGatewayAddress;
	}

	if( pulDNSServerAddress != NULL )
	{
		xNetworkAddressing.ulDNSServerAddress = *pulDNSServerAddress;
	}
}
/*-----------------------------------------------------------*/

#if ( ipconfigSUPPORT_OUTGOING_PINGS == 1 )

	BaseType_t FreeRTOS_SendPingRequest( uint32_t ulIPAddress, size_t xNumberOfBytesToSend, TickType_t xBlockTimeTicks )
	{
	NetworkBufferDescriptor_t *pxNetworkBuffer;
	ICMPHeader_t *pxICMPHeader;
	BaseType_t xReturn = pdFAIL;
	static uint16_t usSequenceNumber = 0;
	uint8_t *pucChar;
	IPStackEvent_t xStackTxEvent = { eStackTxEvent, NULL };

		pxNetworkBuffer = pxGetNetworkBufferWithDescriptor( xNumberOfBytesToSend + sizeof( ICMPPacket_t ), xBlockTimeTicks );

		if( pxNetworkBuffer != NULL )
		{
		BaseType_t xEnoughSpace;

			xEnoughSpace = xNumberOfBytesToSend < ( ( ipconfigNETWORK_MTU - sizeof( IPHeader_t ) ) - sizeof( ICMPHeader_t ) );
			if( ( uxGetNumberOfFreeNetworkBuffers() >= 3 ) && ( xNumberOfBytesToSend >= 1 ) && ( xEnoughSpace != pdFALSE ) )
			{
				pxICMPHeader = ipPOINTER_CAST( ICMPHeader_t *, &( pxNetworkBuffer->pucEthernetBuffer[ ipIP_PAYLOAD_OFFSET ] ) );
				usSequenceNumber++;

				/* Fill in the basic header information. */
				pxICMPHeader->ucTypeOfMessage = ipICMP_ECHO_REQUEST;
				pxICMPHeader->ucTypeOfService = 0;
				pxICMPHeader->usIdentifier = usSequenceNumber;
				pxICMPHeader->usSequenceNumber = usSequenceNumber;

				/* Find the start of the data. */
				pucChar = ( uint8_t * ) pxICMPHeader;
				pucChar = &(pucChar[ sizeof( ICMPHeader_t ) ] );

				/* Just memset the data to a fixed value. */
				memset( pucChar, ( int ) ipECHO_DATA_FILL_BYTE, xNumberOfBytesToSend );

				/* The message is complete, IP and checksum's are handled by
				vProcessGeneratedUDPPacket */
				pxNetworkBuffer->pucEthernetBuffer[ ipSOCKET_OPTIONS_OFFSET ] = FREERTOS_SO_UDPCKSUM_OUT;
				pxNetworkBuffer->ulIPAddress = ulIPAddress;
				pxNetworkBuffer->usPort = ipPACKET_CONTAINS_ICMP_DATA;
				pxNetworkBuffer->xDataLength = xNumberOfBytesToSend + sizeof( ICMPHeader_t );

				/* Send to the stack. */
				xStackTxEvent.pvData = pxNetworkBuffer;

				if( xSendEventStructToIPTask( &xStackTxEvent, xBlockTimeTicks) != pdPASS )
				{
					vReleaseNetworkBufferAndDescriptor( pxNetworkBuffer );
					iptraceSTACK_TX_EVENT_LOST( ipSTACK_TX_EVENT );
				}
				else
				{
					xReturn = usSequenceNumber;
				}
			}
		}
		else
		{
			/* The requested number of bytes will not fit in the available space
			in the network buffer. */
		}

		return xReturn;
	}

#endif /* ipconfigSUPPORT_OUTGOING_PINGS == 1 */
/*-----------------------------------------------------------*/

BaseType_t xSendEventToIPTask( eIPEvent_t eEvent )
{
IPStackEvent_t xEventMessage;
const TickType_t xDontBlock = ( TickType_t ) 0;

	xEventMessage.eEventType = eEvent;
	xEventMessage.pvData = ( void* )NULL;

	return xSendEventStructToIPTask( &xEventMessage, xDontBlock );
}
/*-----------------------------------------------------------*/

BaseType_t xSendEventStructToIPTask( const IPStackEvent_t *pxEvent, TickType_t xTimeout )
{
BaseType_t xReturn, xSendMessage;
TickType_t xUseTimeout = xTimeout;

	if( ( xIPIsNetworkTaskReady() == pdFALSE ) && ( pxEvent->eEventType != eNetworkDownEvent ) )
	{
		/* Only allow eNetworkDownEvent events if the IP task is not ready
		yet.  Not going to attempt to send the message so the send failed. */
		xReturn = pdFAIL;
	}
	else
	{
		xSendMessage = pdTRUE;

		#if( ipconfigUSE_TCP == 1 )
		{
			if( pxEvent->eEventType == eTCPTimerEvent )
			{
				/* TCP timer events are sent to wake the timer task when
				xTCPTimer has expired, but there is no point sending them if the
				IP task is already awake processing other message. */
				xTCPTimer.bExpired = pdTRUE_UNSIGNED;

				if( uxQueueMessagesWaiting( xNetworkEventQueue ) != 0u )
				{
					/* Not actually going to send the message but this is not a
					failure as the message didn't need to be sent. */
					xSendMessage = pdFALSE;
				}
			}
		}
		#endif /* ipconfigUSE_TCP */

		if( xSendMessage != pdFALSE )
		{
			/* The IP task cannot block itself while waiting for itself to
			respond. */
			if( xIsCallingFromIPTask() == pdTRUE )
			{
				xUseTimeout = ( TickType_t ) 0;
			}

			xReturn = xQueueSendToBack( xNetworkEventQueue, pxEvent, xUseTimeout );

			if( xReturn == pdFAIL )
			{
				/* A message should have been sent to the IP task, but wasn't. */
				FreeRTOS_debug_printf( ( "xSendEventStructToIPTask: CAN NOT ADD %d\n", pxEvent->eEventType ) );
				iptraceSTACK_TX_EVENT_LOST( pxEvent->eEventType );
			}
		}
		else
		{
			/* It was not necessary to send the message to process the event so
			even though the message was not sent the call was successful. */
			xReturn = pdPASS;
		}
	}

	return xReturn;
}
/*-----------------------------------------------------------*/

eFrameProcessingResult_t eConsiderFrameForProcessing( const uint8_t * const pucEthernetBuffer )
{
eFrameProcessingResult_t eReturn;
const EthernetHeader_t *pxEthernetHeader;
NetworkEndPoint_t *pxEndPoint;

	/* The Ethernet header is at the head of the packet. */
	pxEthernetHeader = ipPOINTER_CAST( const EthernetHeader_t *, pucEthernetBuffer );

	/* Examine the destination MAC from the Ethernet header to see if it matches
	that of an end point managed by FreeRTOS+TCP. */
	pxEndPoint = FreeRTOS_FindEndPointOnMAC( &( pxEthernetHeader->xDestinationAddress ), NULL );

	if( pxEndPoint != NULL )
	{
		/* The packet was directed to this node - process it. */
		eReturn = eProcessBuffer;
	}
	else if( memcmp( xBroadcastMACAddress.ucBytes, pxEthernetHeader->xDestinationAddress.ucBytes, sizeof( MACAddress_t ) ) == 0 )
	{
		/* The packet was a broadcast - process it. */
		eReturn = eProcessBuffer;
	}
	else
	#if( ipconfigUSE_LLMNR == 1 )
	if( memcmp( xLLMNR_MacAdress.ucBytes, pxEthernetHeader->xDestinationAddress.ucBytes, sizeof( MACAddress_t ) ) == 0 )
	{
		/* The packet is a request for LLMNR - process it. */
		eReturn = eProcessBuffer;
	}
	else
	#endif /* ipconfigUSE_LLMNR */

	#if( ipconfigUSE_IPv6 != 0 )
	if( ( pxEthernetHeader->xDestinationAddress.ucBytes[ 0 ] == ipMULTICAST_MAC_ADDRESS_IPv6_0 ) &&
		( pxEthernetHeader->xDestinationAddress.ucBytes[ 1 ] == ipMULTICAST_MAC_ADDRESS_IPv6_1 ) )
	{
		/* The packet is a request for LLMNR - process it. */
		eReturn = eProcessBuffer;
	}
	else
	#endif /* ipconfigUSE_IPv6 */
	{
		/* The packet was not a broadcast, or for this node, just release
		the buffer without taking any other action. */
		eReturn = eReleaseBuffer;
	}

	#if( ipconfigFILTER_OUT_NON_ETHERNET_II_FRAMES == 1 )
	{
	uint16_t usFrameType;

		if( eReturn == eProcessBuffer )
		{
			usFrameType = pxEthernetHeader->usFrameType;
			usFrameType = FreeRTOS_ntohs( usFrameType );

			if( usFrameType <= 0x600U )
			{
				/* Not an Ethernet II frame. */
				eReturn = eReleaseBuffer;
			}
		}
	}
	#endif /* ipconfigFILTER_OUT_NON_ETHERNET_II_FRAMES == 1  */

	#if( ipconfigHAS_DEBUG_PRINTF != 0 )
	{
		if( eReturn != eProcessBuffer )
		{
			FreeRTOS_debug_printf( ( "eConsiderFrameForProcessing: Drop MAC %02x:%02x:%02x:%02x:%02x:%02x\n",
				pxEthernetHeader->xDestinationAddress.ucBytes[ 0 ],
				pxEthernetHeader->xDestinationAddress.ucBytes[ 1 ],
				pxEthernetHeader->xDestinationAddress.ucBytes[ 2 ],
				pxEthernetHeader->xDestinationAddress.ucBytes[ 3 ],
				pxEthernetHeader->xDestinationAddress.ucBytes[ 4 ],
				pxEthernetHeader->xDestinationAddress.ucBytes[ 5 ] ) );
		}
	}
	#endif /* ipconfigHAS_DEBUG_PRINTF */

	return eReturn;
}
/*-----------------------------------------------------------*/

static void prvProcessNetworkDownEvent( NetworkInterface_t *pxInterface )
{
NetworkEndPoint_t *pxEndPoint;

	configASSERT( pxInterface != NULL );
	configASSERT( pxInterface->pfInitialise != NULL );


	/* The first network down event is generated by the IP stack itself to
	initialise the network hardware, so do not call the network down event
	the first time through. */
	/*_RB_ Similarly it is not clear to me why there is not a one to one mapping between the interface and the end point, which would negate the need for this loop.  Likewise the loop further down the same function. */
	for( pxEndPoint = FreeRTOS_FirstEndPoint( pxInterface );
		 pxEndPoint != NULL;
		 pxEndPoint = FreeRTOS_NextEndPoint( pxInterface, pxEndPoint ) )
	{
		/* The bit 'bEndPointUp' stays low until vIPNetworkUpCalls() is called. */
		pxEndPoint->bits.bEndPointUp = pdFALSE_UNSIGNED;
		#if ipconfigUSE_NETWORK_EVENT_HOOK == 1
		{
			if( pxEndPoint->bits.bCallDownHook != pdFALSE_UNSIGNED )
			{
				vApplicationIPNetworkEventHook( eNetworkDown, pxEndPoint );
			}
			else
			{
				/* The next time NetworkEventHook will be called for this end-point. */
				pxEndPoint->bits.bCallDownHook = pdTRUE_UNSIGNED;
			}
		}
		#endif /* ipconfigUSE_NETWORK_EVENT_HOOK */
	}

	/* The network has been disconnected (or is being initialised for the first
	time).  Perform whatever hardware processing is necessary to bring it up
	again, or wait for it to be available again.  This is hardware dependent. */

	if( pxInterface->pfInitialise( pxInterface ) == pdPASS )
	{
		pxInterface->bits.bInterfaceUp = pdTRUE_UNSIGNED;
		/* Set remaining time to 0 so it will become active immediately. */
		/* The network is not up until DHCP has completed.
		Start it now for all associated end-points. */

		for( pxEndPoint = FreeRTOS_FirstEndPoint( pxInterface );
			 pxEndPoint != NULL;
			 pxEndPoint = FreeRTOS_NextEndPoint( pxInterface, pxEndPoint ) )
		{
			#if ipconfigUSE_DHCP == 1
			if( pxEndPoint->bits.bWantDHCP != pdFALSE_UNSIGNED )
			{
			IPStackEvent_t xEventMessage;
			const TickType_t xDontBlock = 0;

				/* Reset the DHCP process for this end-point. */
				vDHCPProcess( pdTRUE, pxEndPoint );

				xEventMessage.eEventType = eDHCPEvent;
				xEventMessage.pvData = pxEndPoint;

				/* And start processing. */
				( void ) xSendEventStructToIPTask( &xEventMessage, xDontBlock );
			}
			else
			#endif
			{
				/* DHCP is not enabled for this end-point.
				Perform any necessary 'network up' processing. */
				vIPNetworkUpCalls( pxEndPoint );
			}
		}
	}
	else
	{
		/* Nothing to do. When the 'xNetworkTimer' expires, all interfaces
		with bits.bInterfaceUp cleared will get a new 'eNetworkDownEvent' */
	}
}
/*-----------------------------------------------------------*/

void vIPNetworkUpCalls( struct xNetworkEndPoint *pxEndPoint )
{
	pxEndPoint->bits.bEndPointUp = pdTRUE_UNSIGNED;

	#if( ipconfigUSE_NETWORK_EVENT_HOOK == 1 )
	{
		vApplicationIPNetworkEventHook( eNetworkUp, pxEndPoint );
	}
	#endif /* ipconfigUSE_NETWORK_EVENT_HOOK */

	/* Static configuration is being used, so the network is now up. */
	#if( ipconfigFREERTOS_PLUS_NABTO == 1 )
	{
		vStartNabtoTask();
	}
	#endif /* ipconfigFREERTOS_PLUS_NABTO */

	#if( ipconfigDNS_USE_CALLBACKS != 0 )
	{
		/* The following function is declared in FreeRTOS_DNS.c	and 'private' to
		this library */
		/* _HT_ Do not do this here but earlier
		extern void vDNSInitialise( void );
		vDNSInitialise();
		*/
	}
	#endif /* ipconfigDNS_USE_CALLBACKS != 0 */

	/* Set remaining time to 0 so it will become active immediately. */
	prvIPTimerReload( &xARPTimer, pdMS_TO_TICKS( ipARP_TIMER_PERIOD_MS ) );
}
/*-----------------------------------------------------------*/

static void prvProcessEthernetPacket( NetworkBufferDescriptor_t * const pxNetworkBuffer )
{
EthernetHeader_t *pxEthernetHeader;
volatile eFrameProcessingResult_t eReturned; /* Volatile to prevent complier warnings when ipCONSIDER_FRAME_FOR_PROCESSING just sets it to eProcessBuffer. */

	configASSERT( pxNetworkBuffer );

	/* Interpret the Ethernet frame.  It is best to filter frames in the MAC
	driver to prevent frames of no interest being passed into the IP stack -
	when that is done ipCONSIDER_FRAME_FOR_PROCESSING() will just return
	eProcessBuffer in all cases. */
	eReturned = ipCONSIDER_FRAME_FOR_PROCESSING( pxNetworkBuffer->pucEthernetBuffer );
	pxEthernetHeader = ipPOINTER_CAST( EthernetHeader_t *, pxNetworkBuffer->pucEthernetBuffer );

	if( eReturned == eProcessBuffer )
	{
		/* Interpret the received Ethernet packet. */
		switch( pxEthernetHeader->usFrameType )
		{
			case ipARP_FRAME_TYPE :
				/* The Ethernet frame contains an ARP packet. */
				eReturned = eARPProcessPacket( pxNetworkBuffer );
				break;

			case ipIPv4_FRAME_TYPE :
		#if( ipconfigUSE_IPv6 != 0 )
			case ipIPv6_FRAME_TYPE :
		#endif
				/* The Ethernet frame contains an IP packet. */
				eReturned = prvProcessIPPacket( ipPOINTER_CAST( IPPacket_t *, pxNetworkBuffer->pucEthernetBuffer ), pxNetworkBuffer );
				break;

			default :
				/* No other packet types are handled.  Nothing to do. */
				eReturned = eReleaseBuffer;
				break;
		}
	}
	#if( ipconfigHAS_DEBUG_PRINTF != 0 )
	else
	{
		FreeRTOS_debug_printf( ( "prvProcessEthernetPacket: Drop MAC %02x:%02x:%02x:%02x:%02x:%02x\n",
			pxEthernetHeader->xDestinationAddress.ucBytes[ 0 ],
			pxEthernetHeader->xDestinationAddress.ucBytes[ 1 ],
			pxEthernetHeader->xDestinationAddress.ucBytes[ 2 ],
			pxEthernetHeader->xDestinationAddress.ucBytes[ 3 ],
			pxEthernetHeader->xDestinationAddress.ucBytes[ 4 ],
			pxEthernetHeader->xDestinationAddress.ucBytes[ 5 ] ) );
	}
	#endif /* ipconfigHAS_DEBUG_PRINTF */

	/* Perform any actions that resulted from processing the Ethernet frame. */
	switch( eReturned )
	{
		case eReturnEthernetFrame :
			/* The Ethernet frame will have been updated (maybe it was
			an ARP request or a PING request?) and should be sent back to
			its source.  The pdTRUE parameter indicated the buffer must be
			released once the frame has been transmitted. */
			vReturnEthernetFrame( pxNetworkBuffer, pdTRUE );
			break;

		case eFrameConsumed :
			/* The frame is in use somewhere, don't release the buffer
			yet. */
			break;

		case eProcessBuffer:
		case eReleaseBuffer:
		default :
			/* The frame is not being used anywhere, and the
			NetworkBufferDescriptor_t structure containing the frame should
			just be	released back to the list of free buffers. */
			vReleaseNetworkBufferAndDescriptor( pxNetworkBuffer );
			break;
	}
}
/*-----------------------------------------------------------*/

#if( ipconfigETHERNET_DRIVER_FILTERS_PACKETS == 0 )
	static BaseType_t prvIsIPv4Multicast( uint32_t ulIPAddress )
	{
	BaseType_t xReturn;

		ulIPAddress = FreeRTOS_ntohl( ulIPAddress );
	#define	ipFIRST_MULTI_CAST_IPv4		0xE0000000uL
	#define	ipLAST_MULTI_CAST_IPv4		0xF0000000uL
		if( ( ulIPAddress >= ipFIRST_MULTI_CAST_IPv4 ) && ( ulIPAddress < ipLAST_MULTI_CAST_IPv4 ) )
		{
			xReturn = pdTRUE;
		}
		else
		{
			xReturn = pdFALSE;
		}
		return xReturn;
	}
#endif	/* ipconfigETHERNET_DRIVER_FILTERS_PACKETS == 0 */
/*-----------------------------------------------------------*/

#if( ipconfigUSE_IPv6 != 0 )
	static BaseType_t prvIsIPv6Multicast( const IPv6_Address_t *pxIPAddress )
	{
	BaseType_t xReturn;

		if( pxIPAddress->ucBytes[ 0 ] == 0xff )
		{
			xReturn = pdTRUE;
		}
		else
		{
			xReturn = pdFALSE;
		}
		return xReturn;
	}
#endif /* ipconfigUSE_IPv6 */
/*-----------------------------------------------------------*/

#if( ipconfigUSE_IPv6 != 0 )
	BaseType_t xCompareIPv6_Address( const IPv6_Address_t *pxLeft, const IPv6_Address_t *pxRight )
	{
	BaseType_t xResult;

		// 0    2    4    6    8    10   12   14
		// ff02:0000:0000:0000:0000:0001:ff66:4a81
		if( ( pxRight->ucBytes[ 0 ] == 0xff ) &&
			( pxRight->ucBytes[ 1 ] == 0x02 ) &&
			( pxRight->ucBytes[ 12 ] == 0xff ) )
		{
			xResult = memcmp( pxLeft->ucBytes + 13, pxRight->ucBytes + 13, 3 );
		}
		else
		if( ( pxRight->ucBytes[ 0 ] == 0xfe ) &&
			( pxRight->ucBytes[ 1 ] == 0x80 ) &&
			( pxLeft ->ucBytes[ 0 ] == 0xfe ) &&
			( pxLeft ->ucBytes[ 1 ] == 0x80 ) )
		{
			/* Both are local addresses. */
			xResult = 0;
		}
		else
		{
			xResult = memcmp( pxLeft->ucBytes, pxRight->ucBytes, sizeof( pxLeft->ucBytes ) );
		}

		return xResult;
	}
#endif /* ipconfigUSE_IPv6 */

#if( ipconfigUSE_IPv6 != 0 )
	eFrameProcessingResult_t prvAllowIPPacketIPv6( const IPHeader_IPv6_t * const pxIPv6Header,
		NetworkBufferDescriptor_t * const pxNetworkBuffer, UBaseType_t uxHeaderLength )
	{
	eFrameProcessingResult_t eReturn = eReleaseBuffer;

		#if( ipconfigETHERNET_DRIVER_FILTERS_PACKETS == 0 )
		{
			/* In systems with a very small amount of RAM, it might be advantageous
			to have incoming messages checked earlier, by the network card driver.
			This method may decrease the usage of sparse network buffers. */
			const IPv6_Address_t *pxDestinationIPAddress = &( pxIPv6Header->xDestinationIPv6Address );

				/* Is the packet for this IP address? */
				if( ( FreeRTOS_FindEndPointOnIP_IPv6( pxDestinationIPAddress ) != NULL ) ||
					/* Is it the multicast address FF00::/8 ? */
					( prvIsIPv6Multicast ( pxDestinationIPAddress ) != pdFALSE ) ||
					/* Or (during DHCP negotiation) we have no IP-address yet? */
					( *ipLOCAL_IP_ADDRESS_POINTER == 0 ) )
				{
					/* Packet is not for this node, release it */
					eReturn = eProcessBuffer;
				}
				else
				{
					FreeRTOS_printf( ( "prvAllowIPPacketIPv6: drop %pip\n", pxDestinationIPAddress->ucBytes ) );
				}
		}
		#else
		{
			/* The packet has been checked by the network interface. */
			eReturn = eProcessBuffer;
		}
		#endif /* ipconfigETHERNET_DRIVER_FILTERS_PACKETS */

		#if( ipconfigDRIVER_INCLUDED_RX_IP_CHECKSUM == 0 )
		{
			/* Some drivers of NIC's with checksum-offloading will enable the above
			define, so that the checksum won't be checked again here */
			if (eReturn == eProcessBuffer )
			{
			const IPPacket_t * pxIPPacket = ipPOINTER_CAST( const IPPacket_t *, pxNetworkBuffer->pucEthernetBuffer );
			NetworkEndPoint_t *pxEndPoint = FreeRTOS_FindEndPointOnMAC( &( pxIPPacket->xEthernetHeader.xSourceAddress ), ( NetworkInterface_t * )NULL );
				/* IPv6 does not have a separate checmsum in the IP-header */
				/* Is the upper-layer checksum (TCP/UDP/ICMP) correct? */
				/* Do not check the checksum of loop-back messages. */
				if( ( pxEndPoint == NULL ) &&
					( usGenerateProtocolChecksum( ( uint8_t * )( pxNetworkBuffer->pucEthernetBuffer ), pxNetworkBuffer->xDataLength, pdFALSE ) != ipCORRECT_CRC ) )
				{
					/* Protocol checksum not accepted. */
					eReturn = eReleaseBuffer;
				}
			}
		}
		#else
		{
			/* to avoid warning unused parameters */
			( void ) pxNetworkBuffer;
			( void ) uxHeaderLength;
		}
		#endif /* ipconfigDRIVER_INCLUDED_RX_IP_CHECKSUM == 0 */

		return eReturn;
	}
#endif /* ipconfigUSE_IPv6 */
/*-----------------------------------------------------------*/

static eFrameProcessingResult_t prvAllowIPPacketIPv4( const IPPacket_t * const pxIPPacket,
	NetworkBufferDescriptor_t * const pxNetworkBuffer, UBaseType_t uxHeaderLength )
{
eFrameProcessingResult_t eReturn = eProcessBuffer;

#if( ( ipconfigETHERNET_DRIVER_FILTERS_PACKETS == 0 ) || ( ipconfigDRIVER_INCLUDED_RX_IP_CHECKSUM == 0 ) )
	const IPHeader_t * pxIPHeader = &( pxIPPacket->xIPHeader );
#else
	/* or else, the parameter won't be used and the function will be optimised
	away */
	( void ) pxIPPacket;
#endif

	#if( ipconfigETHERNET_DRIVER_FILTERS_PACKETS == 0 )
	{
		/* In systems with a very small amount of RAM, it might be advantageous
		to have incoming messages checked earlier, by the network card driver.
		This method may decrease the usage of sparse network buffers. */
		uint32_t ulDestinationIPAddress = pxIPHeader->ulDestinationIPAddress;

			/* Ensure that the incoming packet is not fragmented (fragmentation
			was only supported for outgoing packets, and is not currently
			not supported at all). */
			if( ( pxIPHeader->usFragmentOffset & ipFRAGMENT_OFFSET_BIT_MASK ) != 0U )
			{
				/* Can not handle, fragmented packet. */
				eReturn = eReleaseBuffer;
			}
			/* 0x45 means: IPv4 with an IP header of 5 x 4 = 20 bytes
			 * 0x47 means: IPv4 with an IP header of 7 x 4 = 28 bytes */
			else if( ( pxIPHeader->ucVersionHeaderLength < 0x45u ) || ( pxIPHeader->ucVersionHeaderLength > 0x4Fu ) )
			{
				/* 0x45 means: IPv4 with an IP header of 5 x 4 = 20 bytes, 0x47
				means: IPv4 with an IP header of 7 x 4 = 28 bytes.  Can not
				handle, unknown or invalid header version. */
				/*_RB_ Why is 0x4f used in the 'else if' above?  These should be #defined at the top of the file. */
				eReturn = eReleaseBuffer;
			}
			else if(
				( pxNetworkBuffer->pxEndPoint == NULL ) &&
				( FreeRTOS_FindEndPointOnIP( ulDestinationIPAddress, 4 ) == NULL ) &&
				/* Is it an IPv4 broadcast address x.x.x.255 ? */
				( ( FreeRTOS_ntohl( ulDestinationIPAddress ) & 0xff ) != 0xff ) &&
				( prvIsIPv4Multicast( ulDestinationIPAddress ) == pdFALSE ) &&
				/* Or (during DHCP negotiation) we have no IP-address yet? */
				( *ipLOCAL_IP_ADDRESS_POINTER != 0uL ) ) /* Should this last test (ipLOCAL_IP_ADDRESS_POINTER) be removed now? */
			{
FreeRTOS_printf( ( "prvAllowIPPacketIPv4: drop %lxip => %lxip\n", FreeRTOS_ntohl( pxIPHeader->ulDestinationIPAddress ), FreeRTOS_ntohl( ulDestinationIPAddress ) ) );
				/* Packet is not for this node, release it */
				eReturn = eReleaseBuffer;
			}
	}
	#endif /* ipconfigETHERNET_DRIVER_FILTERS_PACKETS */

	#if( ipconfigDRIVER_INCLUDED_RX_IP_CHECKSUM == 0 )
	{
		/* Some drivers of NIC's with checksum-offloading will enable the above
		define, so that the checksum won't be checked again here */
		if (eReturn == eProcessBuffer )	/*lint !e774 Boolean within 'if' always evaluates to True [MISRA 2012 Rule 14.3, required]. */
		{
			NetworkEndPoint_t *pxEndPoint = FreeRTOS_FindEndPointOnMAC( &( pxIPPacket->xEthernetHeader.xSourceAddress ), NULL );
			/* Do not check the checksum of loop-back messages. */
			if( pxEndPoint == NULL )
			{
				/* Is the IP header checksum correct? */
				if( ( pxIPHeader->ucProtocol != ( uint8_t ) ipPROTOCOL_ICMP ) &&
					( usGenerateChecksum( 0uL, ipPOINTER_CAST( uint8_t *, &( pxIPHeader->ucVersionHeaderLength ) ), ( size_t ) uxHeaderLength ) != ipCORRECT_CRC ) )	/*lint !e9007 side effects on right hand of logical operator, ''&&'' [MISRA 2012 Rule 13.5, required]. */
				{
					/* Check sum in IP-header not correct. */
					eReturn = eReleaseBuffer;
				}
				/* Is the upper-layer checksum (TCP/UDP/ICMP) correct? */
				else if( usGenerateProtocolChecksum( ( uint8_t * )( pxNetworkBuffer->pucEthernetBuffer ), pxNetworkBuffer->xDataLength, pdFALSE ) != ipCORRECT_CRC )
				{
					/* Protocol checksum not accepted. */
					eReturn = eReleaseBuffer;
				}
				else
				{
					/* The checksum of the received packet is OK. */
				}
			}
		}
	}
	#else
	{
		/* to avoid warning unused parameters */
		( void ) pxNetworkBuffer;
		( void ) uxHeaderLength;
	}
	#endif /* ipconfigDRIVER_INCLUDED_RX_IP_CHECKSUM == 0 */

	return eReturn;
}
/*-----------------------------------------------------------*/

static eFrameProcessingResult_t prvProcessIPPacket( IPPacket_t * pxIPPacket, NetworkBufferDescriptor_t * const pxNetworkBuffer )
{
eFrameProcessingResult_t eReturn;
IPHeader_t * pxIPHeader = &( pxIPPacket->xIPHeader );
ProtocolHeaders_t *pxProtocolHeaders;/*lint !e9018 declaration of symbol with union based type [MISRA 2012 Rule 19.2, advisory] */
#if( ipconfigUSE_IPv6 != 0 )
	const IPHeader_IPv6_t * pxIPHeader_IPv6;
#endif
UBaseType_t uxHeaderLength;
uint8_t ucProtocol;

	#if( ipconfigUSE_IPv6 != 0 )
	pxIPHeader_IPv6 = ( const IPHeader_IPv6_t * )( pxNetworkBuffer->pucEthernetBuffer + ipSIZE_OF_ETH_HEADER );
	if( pxIPPacket->xEthernetHeader.usFrameType == ipIPv6_FRAME_TYPE )
	{
		uxHeaderLength = ipSIZE_OF_IPv6_HEADER;
		ucProtocol = pxIPHeader_IPv6->ucNextHeader;
		pxProtocolHeaders = ( ProtocolHeaders_t * ) ( pxNetworkBuffer->pucEthernetBuffer + ipSIZE_OF_ETH_HEADER + ipSIZE_OF_IPv6_HEADER );
		eReturn = prvAllowIPPacketIPv6( ( IPHeader_IPv6_t * )&( pxIPPacket->xIPHeader ), pxNetworkBuffer, uxHeaderLength );
		/* The IP-header type is copied to a location 6 bytes before the messages
		starts.  It might be needed later on when a UDP-payload buffer is being
		used. */
		pxNetworkBuffer->pucEthernetBuffer[ - ipIP_TYPE_OFFSET ] = pxIPHeader_IPv6->ucVersionTrafficClass;
	}
	else
	#endif
	{
		/* Check if the IP headers are acceptable and if it has our destination.
		The lowest four bits of 'ucVersionHeaderLength' indicate the IP-header
		length in multiples of 4. */
		uxHeaderLength = ( pxIPHeader->ucVersionHeaderLength & 0x0F) << 2;
		ucProtocol = pxIPPacket->xIPHeader.ucProtocol;
		pxProtocolHeaders = ipPOINTER_CAST( ProtocolHeaders_t *, pxNetworkBuffer->pucEthernetBuffer + ipSIZE_OF_ETH_HEADER + uxHeaderLength );
		eReturn = prvAllowIPPacketIPv4( pxIPPacket, pxNetworkBuffer, uxHeaderLength );
		#if( ipconfigUSE_IPv6 != 0 )
		{
			/* The IP-header type is copied to a location 6 bytes before the
			messages starts.  It might be needed later on when a UDP-payload
			buffer is being used. */
			pxNetworkBuffer->pucEthernetBuffer[ - ipIP_TYPE_OFFSET ] = pxIPHeader->ucVersionHeaderLength;
		}
		#endif /* ipconfigUSE_IPv6 */
	}

	if( eReturn == eProcessBuffer )
	{
		if(
			#if( ipconfigUSE_IPv6 != 0 )
				( pxIPPacket->xEthernetHeader.usFrameType != ipIPv6_FRAME_TYPE ) &&
			#endif /* ipconfigUSE_IPv6 */
			( uxHeaderLength > ipSIZE_OF_IPv4_HEADER ) )
		{
			/* All structs of headers expect an IP header size of 20 bytes.  IP
			header options were included, ignore them and cut them out.
			Note: IP options are mostly use in Multi-cast protocols. */
			const size_t optlen = ( ( size_t ) uxHeaderLength ) - ipSIZE_OF_IPv4_HEADER;

			/* From: the previous start of UDP/ICMP/TCP data. */
			uint8_t *pucSource = ( uint8_t * ) pxIPHeader;
			pucSource = &( pucSource[ uxHeaderLength ] );	/*lint !e662*/

			/* To: the usual start of UDP/ICMP/TCP data at offset 20 from the IP
			header. */
//			uint8_t *pucTarget = ( ( uint8_t * ) pxIPHeader ) + ipSIZE_OF_IPv4_HEADER;
			uint8_t *pucTarget = ( uint8_t * ) pxIPHeader;
			pucTarget = &( pucTarget[ ipSIZE_OF_IPv4_HEADER ] );

			/* How many: total length minus the options and the lower headers. */
			const size_t  xMoveLen = pxNetworkBuffer->xDataLength - ( optlen + ipSIZE_OF_IPv4_HEADER + ipSIZE_OF_ETH_HEADER );

			memmove( pucTarget, pucSource, xMoveLen );
			pxNetworkBuffer->xDataLength -= optlen;
		}

		/* Add the IP and MAC addresses to the ARP table if they are not already
		there - otherwise refresh the age of the existing entry. */
		if( ucProtocol != ipPROTOCOL_UDP )
		{
			/* Refresh the ARP cache with the IP/MAC-address of the received
			packet.  For UDP packets, this will be done later in
			xProcessReceivedUDPPacket(), as soon as its know that the message
			will be handled.  This will prevent the ARP cache getting
			overwritten with the IP address of useless broadcast packets. */
#if( ipconfigUSE_IPv6 != 0 )
			if( pxIPPacket->xEthernetHeader.usFrameType == ipIPv6_FRAME_TYPE )
			{
				vNDRefreshCacheEntry( &( pxIPPacket->xEthernetHeader.xSourceAddress ), &( pxIPHeader_IPv6->xSourceIPv6Address ), pxNetworkBuffer->pxEndPoint );
			}
			else
#endif /* ipconfigUSE_IPv6 */
			{
				vARPRefreshCacheEntry( &( pxIPPacket->xEthernetHeader.xSourceAddress ), pxIPHeader->ulSourceIPAddress, pxNetworkBuffer->pxEndPoint );
			}
		}

		switch( ucProtocol )
		{
			case ipPROTOCOL_ICMP :
				/* The IP packet contained an ICMP frame.  Don't bother checking
				the ICMP checksum, as if it is wrong then the wrong data will
				also be returned, and the source of the ping will know something
				went wrong because it will not be able to validate what it
				receives. */
				#if ( ipconfigREPLY_TO_INCOMING_PINGS == 1 ) || ( ipconfigSUPPORT_OUTGOING_PINGS == 1 )
				{
					ICMPPacket_t *pxICMPPacket = ipPOINTER_CAST( ICMPPacket_t *, pxNetworkBuffer->pucEthernetBuffer );
					eReturn = prvProcessICMPPacket( pxICMPPacket );
				}
				#endif /* ( ipconfigREPLY_TO_INCOMING_PINGS == 1 ) || ( ipconfigSUPPORT_OUTGOING_PINGS == 1 ) */
				break;

#if( ipconfigUSE_IPv6 != 0 )
			case ipPROTOCOL_ICMP_IPv6:
				eReturn = prvProcessICMPMessage_IPv6( pxNetworkBuffer );
				break;
#endif

			case ipPROTOCOL_UDP :
				{
					/* The IP packet contained a UDP frame. */
					UDPPacket_t *pxUDPPacket = ipPOINTER_CAST( UDPPacket_t *, pxNetworkBuffer->pucEthernetBuffer );

					/* Note the header values required prior to the checksum
					generation as the checksum pseudo header may clobber some of
					these values. */
					pxNetworkBuffer->xDataLength = FreeRTOS_ntohs( pxProtocolHeaders->xUDPHeader.usLength ) - sizeof( UDPHeader_t );
					pxNetworkBuffer->usPort = pxProtocolHeaders->xUDPHeader.usSourcePort;
					pxNetworkBuffer->ulIPAddress = pxUDPPacket->xIPHeader.ulSourceIPAddress;

					/* ipconfigDRIVER_INCLUDED_RX_IP_CHECKSUM:
					In some cases, the upper-layer checksum has been calculated
					by the NIC driver. */

					/* Pass the packet payload to the UDP sockets
					implementation. */
					if( xProcessReceivedUDPPacket( pxNetworkBuffer, pxProtocolHeaders->xUDPHeader.usDestinationPort ) == pdPASS )
					{
						eReturn = eFrameConsumed;
					}
				}
				break;

#if ipconfigUSE_TCP == 1
			case ipPROTOCOL_TCP :
				{

					if( xProcessReceivedTCPPacket( pxNetworkBuffer ) == pdPASS )
					{
						eReturn = eFrameConsumed;
					}

					/* Setting this variable will cause xTCPTimerCheck()
					to be called just before the IP-task blocks. */
					xProcessedTCPMessage++;
				}
				break;
#endif
			default	:
				/* Not a supported frame type. */
				break;
		}
	}

	return eReturn;
}
/*-----------------------------------------------------------*/

#if ( ipconfigSUPPORT_OUTGOING_PINGS == 1 )

	static void prvProcessICMPEchoReply( ICMPPacket_t * const pxICMPPacket )
	{
	ePingReplyStatus_t eStatus = eSuccess;
	uint16_t usDataLength, usCount;
	uint8_t *pucByte;

		/* Find the total length of the IP packet. */
		usDataLength = pxICMPPacket->xIPHeader.usLength;
		usDataLength = FreeRTOS_ntohs( usDataLength );

		/* Remove the length of the IP headers to obtain the length of the ICMP
		message itself. */
		usDataLength = ( uint16_t ) ( ( ( uint32_t ) usDataLength ) - ipSIZE_OF_IPv4_HEADER );

		/* Remove the length of the ICMP header, to obtain the length of
		data contained in the ping. */
		usDataLength = ( uint16_t ) ( ( ( uint32_t ) usDataLength ) - ipSIZE_OF_ICMPv4_HEADER );

		/* Checksum has already been checked before in prvProcessIPPacket */

		/* Find the first byte of the data within the ICMP packet. */
		pucByte = ( uint8_t * ) pxICMPPacket;
		pucByte = &( pucByte[ sizeof( ICMPPacket_t ) ] );

		/* Check each byte. */
		for( usCount = 0; usCount < usDataLength; usCount++ )
		{
			if( *pucByte != ipECHO_DATA_FILL_BYTE )
			{
				eStatus = eInvalidData;
				break;
			}

			pucByte++;
		}

		/* Call back into the application to pass it the result. */
		vApplicationPingReplyHook( eStatus, pxICMPPacket->xICMPHeader.usIdentifier );
	}

#endif
/*-----------------------------------------------------------*/

#if ( ipconfigREPLY_TO_INCOMING_PINGS == 1 )

	static eFrameProcessingResult_t prvProcessICMPEchoRequest( ICMPPacket_t * const pxICMPPacket )
	{
	ICMPHeader_t *pxICMPHeader;
	IPHeader_t *pxIPHeader;
	uint16_t usRequest;
	uint32_t ulIPAddress;

		pxICMPHeader = &( pxICMPPacket->xICMPHeader );
		pxIPHeader = &( pxICMPPacket->xIPHeader );

		/* HT:endian: changed back */
		iptraceSENDING_PING_REPLY( pxIPHeader->ulSourceIPAddress );

		/* The checksum can be checked here - but a ping reply should be
		returned even if the checksum is incorrect so the other end can
		tell that the ping was received - even if the ping reply contains
		invalid data. */
		pxICMPHeader->ucTypeOfMessage = ( uint8_t ) ipICMP_ECHO_REPLY;
		ulIPAddress = pxIPHeader->ulDestinationIPAddress;
		pxIPHeader->ulDestinationIPAddress = pxIPHeader->ulSourceIPAddress;
		pxIPHeader->ulSourceIPAddress = ulIPAddress;

		/* Update the checksum because the ucTypeOfMessage member in the header
		has been changed to ipICMP_ECHO_REPLY.  This is faster than calling
		usGenerateChecksum(). */

		/* due to compiler warning "integer operation result is out of range" */

		usRequest = ( uint16_t ) ( ( uint16_t )ipICMP_ECHO_REQUEST << 8 );

		if( pxICMPHeader->usChecksum >= FreeRTOS_htons( 0xFFFFu - usRequest ) )
		{
			pxICMPHeader->usChecksum = ( uint16_t )
				( ( ( uint32_t ) pxICMPHeader->usChecksum ) +
					FreeRTOS_htons( usRequest + 1UL ) );
		}
		else
		{
			pxICMPHeader->usChecksum = ( uint16_t )
				( ( ( uint32_t ) pxICMPHeader->usChecksum ) +
					FreeRTOS_htons( usRequest ) );
		}
		return eReturnEthernetFrame;
	}

#endif /* ipconfigREPLY_TO_INCOMING_PINGS == 1 */
/*-----------------------------------------------------------*/

#if ( ipconfigREPLY_TO_INCOMING_PINGS == 1 ) || ( ipconfigSUPPORT_OUTGOING_PINGS == 1 )

	static eFrameProcessingResult_t prvProcessICMPPacket( ICMPPacket_t * const pxICMPPacket )
	{
	eFrameProcessingResult_t eReturn = eReleaseBuffer;

		iptraceICMP_PACKET_RECEIVED();
		switch( pxICMPPacket->xICMPHeader.ucTypeOfMessage )
		{
			case ipICMP_ECHO_REQUEST	:
				#if ( ipconfigREPLY_TO_INCOMING_PINGS == 1 )
				{
					eReturn = prvProcessICMPEchoRequest( pxICMPPacket );
				}
				#endif /* ( ipconfigREPLY_TO_INCOMING_PINGS == 1 ) */
				break;

			case ipICMP_ECHO_REPLY		:
				#if ( ipconfigSUPPORT_OUTGOING_PINGS == 1 )
				{
					prvProcessICMPEchoReply( pxICMPPacket );
				}
				#endif /* ipconfigSUPPORT_OUTGOING_PINGS */
				break;

			default	:
				/* Other types of ICMP packets are not handled yet. */
				break;
		}

		return eReturn;
	}

#endif /* ( ipconfigREPLY_TO_INCOMING_PINGS == 1 ) || ( ipconfigSUPPORT_OUTGOING_PINGS == 1 ) */
/*-----------------------------------------------------------*/

uint16_t usGenerateProtocolChecksum_new( const uint8_t * const pucEthernetBuffer, size_t uxBufferLength, BaseType_t xOutgoingPacket );

#include "checksum.ipv6.c"	/*lint !e9019 declaration of '_xUnion32' before #include [MISRA 2012 Rule 20.1, advisory]. */

uint16_t usGenerateProtocolChecksum( const uint8_t * const pucEthernetBuffer, size_t uxBufferLength, BaseType_t xOutgoingPacket )
{
uint32_t ulLength;
uint16_t usChecksum, *pusChecksum, usPayloadLength;
const IPPacket_t * pxIPPacket;
BaseType_t xIPHeaderLength;
ProtocolHeaders_t *pxProtocolHeaders;	/*lint !e9018 declaration of symbol with union based type [MISRA 2012 Rule 19.2, advisory]. */
uint8_t ucProtocol;
#if( ipconfigUSE_IPv6 != 0 )
	BaseType_t xIsIPv6;
	const IPHeader_IPv6_t * pxIPPacket_IPv6;
	uint32_t pulHeader[ 2 ];
#endif

#if( ipconfigHAS_DEBUG_PRINTF != 0 )
	const char *pcType;
#endif
	if( !xOutgoingPacket )
	{
		( void ) usGenerateProtocolChecksum_new( pucEthernetBuffer, uxBufferLength, xOutgoingPacket );
	}

	pxIPPacket = ipPOINTER_CAST( const IPPacket_t *, pucEthernetBuffer );

#if( ipconfigUSE_IPv6 != 0 )
	pxIPPacket_IPv6 = ( const IPHeader_IPv6_t * )( pucEthernetBuffer + ipSIZE_OF_ETH_HEADER );
	if( pxIPPacket->xEthernetHeader.usFrameType == ipIPv6_FRAME_TYPE )
	{
		xIsIPv6 = pdTRUE;
	}
	else
	{
		xIsIPv6 = pdFALSE;
	}

	if( xIsIPv6 != pdFALSE )
	{
		xIPHeaderLength = ipSIZE_OF_IPv6_HEADER;
		ucProtocol = pxIPPacket_IPv6->ucNextHeader;
		pxProtocolHeaders = ( ProtocolHeaders_t * ) ( pucEthernetBuffer + ipSIZE_OF_ETH_HEADER + ipSIZE_OF_IPv6_HEADER );
		usPayloadLength = FreeRTOS_ntohs( pxIPPacket_IPv6->usPayloadLength );
	}
	else
#endif
	{
		xIPHeaderLength = 4 * ( pxIPPacket->xIPHeader.ucVersionHeaderLength & 0x0F ); /*_RB_ Why 4? */
		ucProtocol = pxIPPacket->xIPHeader.ucProtocol;
		pxProtocolHeaders = ipPOINTER_CAST( ProtocolHeaders_t *, &( pucEthernetBuffer[ ipSIZE_OF_ETH_HEADER + ( size_t ) xIPHeaderLength ] ) );
		usPayloadLength = FreeRTOS_ntohs( pxIPPacket->xIPHeader.usLength );
	}

	if( ucProtocol == ( uint8_t ) ipPROTOCOL_UDP )
	{
		pusChecksum = ( uint16_t * ) ( &( pxProtocolHeaders->xUDPHeader.usChecksum ) );
		#if( ipconfigHAS_DEBUG_PRINTF != 0 )
		{
			pcType = "UDP";
		}
		#endif	/* ipconfigHAS_DEBUG_PRINTF != 0 */
	}
	else if( ucProtocol == ( uint8_t ) ipPROTOCOL_TCP )
	{
		pusChecksum = ( uint16_t * ) ( &( pxProtocolHeaders->xTCPHeader.usChecksum ) );
		#if( ipconfigHAS_DEBUG_PRINTF != 0 )
		{
			pcType = "TCP";
		}
		#endif	/* ipconfigHAS_DEBUG_PRINTF != 0 */
	}
	else if( ( ucProtocol == ( uint8_t ) ipPROTOCOL_ICMP ) ||
			( ucProtocol == ( uint8_t ) ipPROTOCOL_IGMP ) )
	{
		pusChecksum = ( uint16_t * ) ( &( pxProtocolHeaders->xICMPHeader.usChecksum ) );

		#if( ipconfigHAS_DEBUG_PRINTF != 0 )
		{
			if( ucProtocol == ( uint8_t ) ipPROTOCOL_ICMP )
			{
				pcType = "ICMP";
			}
			else
			{
				pcType = "IGMP";
			}
		}
		#endif	/* ipconfigHAS_DEBUG_PRINTF != 0 */
	}
	#if( ipconfigUSE_IPv6 != 0 )
	else if( ucProtocol == ipPROTOCOL_ICMP_IPv6 )
	{
		pusChecksum = ( uint16_t * ) ( &( pxProtocolHeaders->xICMPHeader.usChecksum ) );
		#if( ipconfigHAS_DEBUG_PRINTF != 0 )
		{
			pcType = "ICMP_IPv6";
		}
		#endif	/* ipconfigHAS_DEBUG_PRINTF != 0 */
	}
	#endif
	else
	{
		/* Unhandled protocol, other than ICMP, IGMP, UDP, or TCP. */
		return ipUNHANDLED_PROTOCOL;	/*lint !e904 Return statement before end of function [MISRA 2012 Rule 15.5, advisory]. */
	}

	if( xOutgoingPacket != pdFALSE )
	{
		/* This is an outgoing packet. Before calculating the checksum, set it
		to zero. */
		*( pusChecksum ) = 0u;
	}
	else if( ( *pusChecksum == 0u ) && ( ucProtocol == ( uint8_t ) ipPROTOCOL_UDP ) )
	{
		/* Sender hasn't set the checksum, no use to calculate it. */
		return ipCORRECT_CRC;	/*lint !e904 Return statement before end of function [MISRA 2012 Rule 15.5, advisory]. */
	}
	else
	{
		/* This is an incoming packet, not being an UDP packet without a checksum. */
	}

	#if( ipconfigUSE_IPv6 != 0 )
	if( xIsIPv6 != pdFALSE )
	{
		ulLength = ( uint32_t ) usPayloadLength;
		/* IPv6 has a 40-byte pseudo header:
		 0..15 Source IPv6 address
		16..31 Target IPv6 address
		32..35 Length of payload
		36..38 three zero's
		39 Next Header, i.e. the protocol type. */

		pulHeader[ 0 ] = FreeRTOS_htonl( ( uint32_t ) usPayloadLength );
		pulHeader[ 1 ] = FreeRTOS_htonl( ( uint32_t ) pxIPPacket_IPv6->ucNextHeader );

		usChecksum = usGenerateChecksum( ( uint32_t ) 0, ( uint8_t * )&( pxIPPacket_IPv6->xSourceIPv6Address ),
				( BaseType_t )( 2 * sizeof( pxIPPacket_IPv6->xSourceIPv6Address ) ) );

		usChecksum = usGenerateChecksum( ( uint32_t ) usChecksum, ( uint8_t * )&( pulHeader ),
				( BaseType_t )( sizeof( pulHeader ) ) );
	}
	else
	#endif
	{
		ulLength = ( uint32_t )
			( usPayloadLength - ( ( uint16_t ) xIPHeaderLength ) ); /* normally minus 20 */
		usChecksum = 0;
	}

	if( ( ulLength < sizeof( pxProtocolHeaders->xUDPHeader ) ) ||
		( ulLength > ( uint32_t )( ipconfigNETWORK_MTU - xIPHeaderLength ) ) )
	{
		#if( ipconfigHAS_DEBUG_PRINTF != 0 )
		{
			FreeRTOS_debug_printf( ( "usGenerateProtocolChecksum[%s]: len invalid: %lu\n", pcType, ulLength ) );
		}
		#endif	/* ipconfigHAS_DEBUG_PRINTF != 0 */

		/* Again, in a 16-bit return value there is no space to indicate an
		error.  For incoming packets, 0x1234 will cause dropping of the packet.
		For outgoing packets, there is a serious problem with the
		format/length */
		return ipINVALID_LENGTH;	/*lint !e904 Return statement before end of function [MISRA 2012 Rule 15.5, advisory]. */
	}
	if( ucProtocol <= ( uint8_t ) ipPROTOCOL_IGMP )
	{
		/* ICMP/IGMP do not have a pseudo header for CRC-calculation. */
		usChecksum = ( uint16_t )
			( ~usGenerateChecksum( 0UL,
				( uint8_t * ) &( pxProtocolHeaders->xICMPHeader ), ( size_t ) ulLength ) );
	}
	#if( ipconfigUSE_IPv6 != 0 )
	else if( ucProtocol == ipPROTOCOL_ICMP_IPv6 )
	{
		usChecksum = ( uint16_t )
			( ~usGenerateChecksum( usChecksum,
				( uint8_t * ) &( pxProtocolHeaders->xTCPHeader ), ( BaseType_t ) ulLength ) );
	}
	#endif /* ipconfigUSE_IPv6 */
	else
	{
		#if( ipconfigUSE_IPv6 != 0 )
		if( xIsIPv6 != pdFALSE )
		{
			/* The CRC of the IPv6 pseudo-header has already been calculated. */
			usChecksum = ( uint16_t )
				( ~usGenerateChecksum( ( uint32_t ) usChecksum, ( uint8_t * )&( pxProtocolHeaders->xUDPHeader.usSourcePort ),
					( BaseType_t )( ulLength ) ) );
		}
		else
		#endif /* ipconfigUSE_IPv6 */
		{
			/* For UDP and TCP, sum the pseudo header, i.e. IP protocol + length
			fields */
			usChecksum = ( uint16_t ) ( ulLength + ( ( uint16_t ) ucProtocol ) );

			/* And then continue at the IPv4 source and destination addresses. */
			usChecksum = ( uint16_t )
				( ~usGenerateChecksum( ( uint32_t ) usChecksum, ipPOINTER_CAST( uint8_t *, &( pxIPPacket->xIPHeader.ulSourceIPAddress ) ),
					( size_t )( ( 2u * sizeof( pxIPPacket->xIPHeader.ulSourceIPAddress ) ) + ulLength ) ) );
		}
		/* Sum TCP header and data. */
	}

	if( xOutgoingPacket == pdFALSE )
	{
		/* This is in incoming packet. If the CRC is correct, it should be zero. */
		if( usChecksum == 0u )
		{
			usChecksum = ( uint16_t )ipCORRECT_CRC;
		}
	}
	else
	{
		if( ( usChecksum == 0u ) && ( ucProtocol == ( uint8_t ) ipPROTOCOL_UDP ) )
		{
			/* In case of UDP, a calculated checksum of 0x0000 is transmitted
			as 0xffff. A value of zero would mean that the checksum is not used. */
			#if( ipconfigHAS_DEBUG_PRINTF != 0 )
			{
				if( xOutgoingPacket != pdFALSE )
				{
					FreeRTOS_debug_printf( ( "usGenerateProtocolChecksum[%s]: crc swap: %04X\n", pcType, usChecksum ) );
				}
			}
			#endif	/* ipconfigHAS_DEBUG_PRINTF != 0 */

			usChecksum = ( uint16_t )0xffffu;
		}
	}
	usChecksum = FreeRTOS_htons( usChecksum );

	if( xOutgoingPacket != pdFALSE )
	{
		*( pusChecksum ) = usChecksum;
	}
	#if( ipconfigHAS_DEBUG_PRINTF != 0 )
	else if( ( xOutgoingPacket == pdFALSE ) && ( usChecksum != ipCORRECT_CRC ) )
	{
	uint16_t usGot, usCalculated;
		usGot = *pusChecksum;
		usCalculated = ~usGenerateProtocolChecksum( pucEthernetBuffer, uxBufferLength, pdTRUE );
		FreeRTOS_debug_printf( ( "usGenerateProtocolChecksum[%s]: len %ld ID %04X: from %lxip to %lxip cal %04X got %04X\n",
			pcType,
			ulLength,
			FreeRTOS_ntohs( pxIPPacket->xIPHeader.usIdentification ),
			FreeRTOS_ntohl( pxIPPacket->xIPHeader.ulSourceIPAddress ),
			FreeRTOS_ntohl( pxIPPacket->xIPHeader.ulDestinationIPAddress ),
			FreeRTOS_ntohs( usCalculated ),
			FreeRTOS_ntohs( usGot ) ) );
	}
	#endif	/* ipconfigHAS_DEBUG_PRINTF != 0 */

	return usChecksum;
}
/*-----------------------------------------------------------*/

uint16_t usGenerateChecksum( uint32_t ulSum, const uint8_t * pucNextData, size_t uxDataLengthBytes )
{
xUnion32 xSum2, xSum, xTerm;	/*lint !e9018*/	
xUnionPtr xSource;				/*lint !e9018*/	/* Points to first byte */
xUnionPtr xLastSource;			/*lint !e9018*/	/* Points to last byte plus one */
uint32_t ulAlignBits, ulCarry = 0uL;

	/* Small MCUs often spend up to 30% of the time doing checksum calculations
	This function is optimised for 32-bit CPUs; Each time it will try to fetch
	32-bits, sums it with an accumulator and counts the number of carries. */

	/* Swap the input (little endian platform only). */
	xSum.u32 = FreeRTOS_ntohs( ulSum );
	xTerm.u32 = 0uL;

	xSource.u8ptr = ipPOINTER_CAST( uint8_t *, pucNextData );
	ulAlignBits = ( ( ( uint32_t ) pucNextData ) & 0x03u ); /*lint !e9078 !e923*/	/* gives 0, 1, 2, or 3 */

	/* If byte (8-bit) aligned... */
	if( ( ( ulAlignBits & 1uL ) != 0uL ) && ( uxDataLengthBytes >= ( size_t ) 1 ) )
	{
		xTerm.u8[ 1 ] = *( xSource.u8ptr );
		xSource.u8ptr++;
		uxDataLengthBytes--;
		/* Now xSource is word (16-bit) aligned. */
	}

	/* If half-word (16-bit) aligned... */
	if( ( ( ulAlignBits == 1u ) || ( ulAlignBits == 2u ) ) && ( uxDataLengthBytes >= 2u ) )
	{
		xSum.u32 += *(xSource.u16ptr);
		xSource.u16ptr++;
		uxDataLengthBytes -= 2u;
		/* Now xSource is word (32-bit) aligned. */
	}

	/* Word (32-bit) aligned, do the most part. */
	xLastSource.u32ptr = ( xSource.u32ptr + ( uxDataLengthBytes / 4u ) ) - 3u;/*lint !e9016 */

	/* In this loop, four 32-bit additions will be done, in total 16 bytes.
	Indexing with constants (0,1,2,3) gives faster code than using
	post-increments. */
	while( xSource.u32ptr < xLastSource.u32ptr )	/*lint !e946*/
	{
		/* Use a secondary Sum2, just to see if the addition produced an
		overflow. */
		xSum2.u32 = xSum.u32 + xSource.u32ptr[ 0 ];
		if( xSum2.u32 < xSum.u32 )
		{
			ulCarry++;
		}

		/* Now add the secondary sum to the major sum, and remember if there was
		a carry. */
		xSum.u32 = xSum2.u32 + xSource.u32ptr[ 1 ];
		if( xSum2.u32 > xSum.u32 )
		{
			ulCarry++;
		}

		/* And do the same trick once again for indexes 2 and 3 */
		xSum2.u32 = xSum.u32 + xSource.u32ptr[ 2 ];
		if( xSum2.u32 < xSum.u32 )
		{
			ulCarry++;
		}

		xSum.u32 = xSum2.u32 + xSource.u32ptr[ 3 ];

		if( xSum2.u32 > xSum.u32 )
		{
			ulCarry++;
		}

		/* And finally advance the pointer 4 * 4 = 16 bytes. */
		xSource.u32ptr += 4;/*lint !e9016 */
	}

	/* Now add all carries. */
	xSum.u32 = ( uint32_t )xSum.u16[ 0 ] + xSum.u16[ 1 ] + ulCarry;

	uxDataLengthBytes %= 16u;
	xLastSource.u8ptr = ( uint8_t * ) ( xSource.u8ptr + ( uxDataLengthBytes & ~( ( size_t ) 1 ) ) );/*lint !e9016 */

	/* Half-word aligned. */
	while( xSource.u16ptr < xLastSource.u16ptr )/*lint !e946 */
	{
		/* At least one more short. */
		xSum.u32 += xSource.u16ptr[ 0 ];
		xSource.u16ptr++;
	}

	if( ( uxDataLengthBytes & ( size_t ) 1 ) != 0u )	/* Maybe one more ? */
	{
		xTerm.u8[ 0 ] = xSource.u8ptr[ 0 ];
	}
	xSum.u32 += xTerm.u32;

	/* Now add all carries again. */
	xSum.u32 = ( uint32_t ) xSum.u16[ 0 ] + xSum.u16[ 1 ];

	/* The previous summation might have given a 16-bit carry. */
	xSum.u32 = ( uint32_t ) xSum.u16[ 0 ] + xSum.u16[ 1 ];

	if( ( ulAlignBits & 1u ) != 0u )
	{
		/* Quite unlikely, but pucNextData might be non-aligned, which would
		 mean that a checksum is calculated starting at an odd position. */
		xSum.u32 = ( ( xSum.u32 & 0xffu ) << 8 ) | ( ( xSum.u32 & 0xff00u ) >> 8 );
	}

	/* swap the output (little endian platform only). */
	return FreeRTOS_htons( ( (uint16_t) xSum.u32 ) );
}
/*-----------------------------------------------------------*/

void vReturnEthernetFrame( NetworkBufferDescriptor_t * pxNetworkBuffer, BaseType_t xReleaseAfterSend )
{
IPPacket_t *pxIPPacket;

#if( ipconfigZERO_COPY_TX_DRIVER != 0 )
	NetworkBufferDescriptor_t *pxNewBuffer;
#endif

	#if defined( ipconfigETHERNET_MINIMUM_PACKET_BYTES )
	{
		if( pxNetworkBuffer->xDataLength < ( size_t ) ipconfigETHERNET_MINIMUM_PACKET_BYTES )
		{
		BaseType_t xIndex;

			FreeRTOS_printf( ( "vReturnEthernetFrame: length %u\n", pxNetworkBuffer->xDataLength ) );
			for( xIndex = ( BaseType_t ) pxNetworkBuffer->xDataLength; xIndex < ( BaseType_t ) ipconfigETHERNET_MINIMUM_PACKET_BYTES; xIndex++ )
			{
				pxNetworkBuffer->pucEthernetBuffer[ xIndex ] = 0u;
			}
			pxNetworkBuffer->xDataLength = ( size_t ) ipconfigETHERNET_MINIMUM_PACKET_BYTES;
		}
	}
	#endif

#if( ipconfigZERO_COPY_TX_DRIVER != 0 )

	if( xReleaseAfterSend == pdFALSE )
	{
		pxNewBuffer = pxDuplicateNetworkBufferWithDescriptor( pxNetworkBuffer, pxNetworkBuffer->xDataLength );
		xReleaseAfterSend = pdTRUE;/*lint !e9044 */
		/* Want no rounding up. */
		pxNewBuffer->xDataLength = pxNetworkBuffer->xDataLength;
		pxNetworkBuffer = pxNewBuffer;/*lint !e9044 */
	}

	if( pxNetworkBuffer != NULL )
#endif
	{
		pxIPPacket = ipPOINTER_CAST( IPPacket_t *, pxNetworkBuffer->pucEthernetBuffer );

		/* Send! */
		if( pxNetworkBuffer->pxEndPoint == NULL )
		{
			/* _HT_ I wonder if this ad-hoc search of an end-point it necessary. */
			FreeRTOS_printf( ( "vReturnEthernetFrame: No pxEndPoint yet for %lxip?\n", FreeRTOS_ntohl( pxIPPacket->xIPHeader.ulDestinationIPAddress ) ) );
			#if( ipconfigUSE_IPv6 != 0 )
			if( ( ( EthernetHeader_t * ) ( pxNetworkBuffer->pucEthernetBuffer ) )->usFrameType == ipIPv6_FRAME_TYPE )
			{
				// To do
			}
			else
			#endif /* ipconfigUSE_IPv6 */
			{
				pxNetworkBuffer->pxEndPoint = FreeRTOS_FindEndPointOnNetMask( pxIPPacket->xIPHeader.ulDestinationIPAddress, 7 );
			}
		}

		if( pxNetworkBuffer->pxEndPoint != NULL )
		{
		NetworkInterface_t *pxInterface = pxNetworkBuffer->pxEndPoint->pxNetworkInterface; /*_RB_ Why not use the pxNetworkBuffer->pxNetworkInterface directly? */

			/* Swap source and destination MAC addresses. */
			memcpy( &( pxIPPacket->xEthernetHeader.xDestinationAddress ), &( pxIPPacket->xEthernetHeader.xSourceAddress ), sizeof( pxIPPacket->xEthernetHeader.xDestinationAddress ) );
			memcpy( &( pxIPPacket->xEthernetHeader.xSourceAddress), pxNetworkBuffer->pxEndPoint->xMACAddress.ucBytes, ( size_t ) ipMAC_ADDRESS_LENGTH_BYTES );
			( void ) pxInterface->pfOutput( pxInterface, pxNetworkBuffer, xReleaseAfterSend );
		}
	}
}
/*-----------------------------------------------------------*/

uint32_t FreeRTOS_GetIPAddress( void )
{
NetworkEndPoint_t *pxEndPoint = FreeRTOS_FindDefaultEndPoint();
uint32_t ulIPAddress;

	/* Returns the IP address of the NIC. */
	if( pxEndPoint == NULL )
	{
		ulIPAddress = 0uL;
	}
	else if( pxEndPoint->ulIPAddress != 0uL )
	{
		ulIPAddress = pxEndPoint->ulIPAddress;
	}
	else
	{
		ulIPAddress = pxEndPoint->ulDefaultIPAddress;
	}

	return ulIPAddress;
}
/*-----------------------------------------------------------*/

void FreeRTOS_SetIPAddress( uint32_t ulIPAddress )
{
	/* Sets the IP address of the NIC. */
	*ipLOCAL_IP_ADDRESS_POINTER = ulIPAddress;
}
/*-----------------------------------------------------------*/

uint32_t FreeRTOS_GetGatewayAddress( void )
{
	return xNetworkAddressing.ulGatewayAddress;
}
/*-----------------------------------------------------------*/

uint32_t FreeRTOS_GetDNSServerAddress( void )
{
	return xNetworkAddressing.ulDNSServerAddress;
}
/*-----------------------------------------------------------*/

uint32_t FreeRTOS_GetNetmask( void )
{
	return xNetworkAddressing.ulNetMask;
}
/*-----------------------------------------------------------*/

const uint8_t * FreeRTOS_GetMACAddress( void )
{
	return ipLOCAL_MAC_ADDRESS;
}
/*-----------------------------------------------------------*/

void FreeRTOS_SetNetmask ( uint32_t ulNetmask )
{
	xNetworkAddressing.ulNetMask = ulNetmask;
}
/*-----------------------------------------------------------*/

void FreeRTOS_SetGatewayAddress ( uint32_t ulGatewayAddress )
{
	xNetworkAddressing.ulGatewayAddress = ulGatewayAddress;
}
/*-----------------------------------------------------------*/

#if( ipconfigUSE_DHCP == 1 )
	void vIPSetDHCPTimerEnableState( struct xNetworkEndPoint *pxEndPoint, BaseType_t xEnableState )
	{
		pxEndPoint->xDHCPTimer.bActive = ( xEnableState != 0 );
	}
#endif /* ipconfigUSE_DHCP */
/*-----------------------------------------------------------*/

#if( ipconfigUSE_DHCP == 1 )
	void vIPReloadDHCPTimer( struct xNetworkEndPoint *pxEndPoint, uint32_t ulLeaseTime )
	{
		prvIPTimerReload( &pxEndPoint->xDHCPTimer, ulLeaseTime );
	}
#endif /* ipconfigUSE_DHCP */
/*-----------------------------------------------------------*/

#if( ipconfigDNS_USE_CALLBACKS == 1 )
	void vIPSetDnsTimerEnableState( BaseType_t xEnableState )
	{
		if( xEnableState != 0 )
		{
			xDNSTimer.bActive = pdTRUE;
		}
		else
		{
			xDNSTimer.bActive = pdFALSE;
		}
	}
#endif /* ipconfigUSE_DHCP */
/*-----------------------------------------------------------*/

#if( ipconfigDNS_USE_CALLBACKS != 0 )
	void vIPReloadDNSTimer( uint32_t ulCheckTime )
	{
		prvIPTimerReload( &xDNSTimer, ulCheckTime );
	}
#endif /* ipconfigDNS_USE_CALLBACKS != 0 */
/*-----------------------------------------------------------*/

BaseType_t xIPIsNetworkTaskReady( void )
{
	return xIPTaskInitialised;
}
/*-----------------------------------------------------------*/

BaseType_t FreeRTOS_IsNetworkUp( struct xNetworkEndPoint *pxEndPoint )
{
BaseType_t xReturn;

	if( pxEndPoint != NULL )
	{
		/* Is this particular end-point up? */
		xReturn	= pxEndPoint->bits.bEndPointUp;
	}
	else
	{
		/* Are all end-points up? */
		xReturn = FreeRTOS_AllEndPointsUp( NULL );
	}
	return xReturn;
}
/*-----------------------------------------------------------*/

#if( ipconfigCHECK_IP_QUEUE_SPACE != 0 )
	UBaseType_t uxGetMinimumIPQueueSpace( void )
	{
		return uxQueueMinimumSpace;
	}
#endif
/*-----------------------------------------------------------*/
