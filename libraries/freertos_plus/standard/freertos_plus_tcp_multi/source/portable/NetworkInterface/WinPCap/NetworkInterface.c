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

/* WinPCap includes. */
#define HAVE_REMOTE
#include "pcap.h"

/* FreeRTOS includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"

/* FreeRTOS+TCP includes. */
#include "FreeRTOS_IP.h"
#include "FreeRTOS_IP_Private.h"
#include "FreeRTOS_Routing.h"
#include "NetworkBufferManagement.h"

/* Thread-safe circular buffers are being used to pass data to and from the PCAP
access functions. */
#include "Win32-Extensions.h"
#include "FreeRTOS_Stream_Buffer.h"

/* Sizes of the thread safe circular buffers used to pass data to and from the
WinPCAP Windows threads. */
#define xSEND_BUFFER_SIZE  32768
#define xRECV_BUFFER_SIZE  32768

/* If ipconfigETHERNET_DRIVER_FILTERS_FRAME_TYPES is set to 1, then the Ethernet
driver will filter incoming packets and only pass the stack those packets it
considers need processing. */
#if( ipconfigETHERNET_DRIVER_FILTERS_FRAME_TYPES == 0 )
	#define ipCONSIDER_FRAME_FOR_PROCESSING( pucEthernetBuffer ) eProcessBuffer
#else
	#define ipCONSIDER_FRAME_FOR_PROCESSING( pucEthernetBuffer ) eConsiderFrameForProcessing( ( pucEthernetBuffer ) )
#endif

/* Used to insert test code only. */
#define niDISRUPT_PACKETS	0

/*-----------------------------------------------------------*/

/*
 * Windows threads that are outside of the control of the FreeRTOS simulator are
 * used to interface with the WinPCAP libraries.
 */
DWORD WINAPI prvWinPcapRecvThread( void *pvParam );
DWORD WINAPI prvWinPcapSendThread( void *pvParam );

/*
 * Print out a numbered list of network interfaces that are available on the
 * host computer.
 */
static pcap_if_t * prvPrintAvailableNetworkInterfaces( void );

/*
 * Open the network interface.  The number of the interface to be opened is set
 * by the configNETWORK_INTERFACE_TO_USE constant in FreeRTOSConfig.h.
 */
static void prvOpenSelectedNetworkInterface( pcap_if_t *pxAllNetworkInterfaces );
static void prvOpenInterface( const char *pucName );

/*
 * Configure the capture filter to allow blocking reads, and to filter out
 * packets that are not of interest to this demo.
 */
static void prvConfigureCaptureBehaviour( void );

/*
 * A function that simulates Ethernet interrupts by periodically polling the
 * WinPCap interface for new data.
 */
static void prvInterruptSimulatorTask( void *pvParameters );

/*
 * Create the buffers that are used to pass data between the FreeRTOS simulator
 * and the Win32 threads that manage WinPCAP.
 */
static void prvCreateThreadSafeBuffers( void );

/*
 * Utility function used to format print messages only.
 */
static const char *prvRemoveSpaces( char *pcBuffer, int aBuflen, const char *pcMessage );

/*
 * Initialises the WinPCap interface.
 */
static BaseType_t prvNetworkInterfaceInitialise( struct xNetworkInterface *pxInterface );

/*
 * Writes data to the network.
 */
static BaseType_t prvNetworkInterfaceOutput( void *pvNetworkInterfaceIndex, NetworkBufferDescriptor_t * const pxNetworkBuffer, BaseType_t bReleaseAfterSend );

/*
 * The WinPCap interface is not a real network interface in its own right, and
 * has no PHY to return the status of.
 */
static BaseType_t prvGetLinkStatus( NetworkInterface_t *pxInterface );

/*
 * Search the array of NetworkInterface_t structures managed by this driver for
 * one that contains the MAC address passed in the pucMACAddressBytes parameter.
 * Return a pointer to the matching NetworkInterface_t structure if one is
 * found, or NULL if a match is not found.
 */
static NetworkInterface_t *prvFindVirtualInterface( const uint8_t *pucMACAddressBytes );

/*-----------------------------------------------------------*/

/* Required by the WinPCap library. */
static char cErrorBuffer[ PCAP_ERRBUF_SIZE ];

/* An event used to wake up the Win32 thread that sends data through the WinPCAP
library. */
static void *pvSendEvent = NULL;

/* _HT_ made the PCAP interface number configurable through the program's
parameters in order to test in different machines. */
static BaseType_t xConfigNextworkInterfaceToUse = configNETWORK_INTERFACE_TO_USE;

/* Handles to the Windows threads that handle the PCAP IO. */
static HANDLE vWinPcapRecvThreadHandle = NULL;
static HANDLE vWinPcapSendThreadHandle = NULL;;

/* The interface being used by WinPCap. */
static pcap_t *pxOpenedInterfaceHandle = NULL;

/* Circular buffers used by the PCAP Win32 threads. */
static StreamBuffer_t *xSendBuffer = NULL;
static StreamBuffer_t *xRecvBuffer = NULL;

/* The MAC address initially set to the constants defined in FreeRTOSConfig.h. */
extern uint8_t ucMACAddress[ 6 ];

/* Logs the number of WinPCAP send failures, for viewing in the debugger only. */
static volatile uint32_t ulWinPCAPSendFailures = 0;

/* This is not a 'normal' network driver, but instead creates one or more
virtual interfaces on an existing network by accessing the raw data on the
network.  xNetworkInterfaces[] stores a NetworkInterface_t structure for each
virtual node on the network.  ulInterfaceOutputCalls[] just keeps a count of the
number of times each network interface has called the output function (the
count is for test and debug purposes only). */
static NetworkInterface_t *pxVirtualNetworkInterefaces[ ipconfigMULTI_INTERFACE ] = { 0 };
static uint32_t ulInterfaceOutputCalls[ ipconfigMULTI_INTERFACE ] = { 0 };

/*-----------------------------------------------------------*/

static BaseType_t prvNetworkInterfaceInitialise( struct xNetworkInterface *pxInterface )
{
BaseType_t xReturn = pdFALSE;
pcap_if_t *pxAllNetworkInterfaces;

	/* This driver does not use the pxInterface parameter.  Remove the compiler
	warning. */
	( void ) pxInterface; /*_RB_ Are any drivers going to use this parameter? */

	/* Query the computer the simulation is being executed on to find the
	network interfaces it has installed. */
	pxAllNetworkInterfaces = prvPrintAvailableNetworkInterfaces();

	/* Open the network interface.  The number of the interface to be opened is
	set by the configNETWORK_INTERFACE_TO_USE constant in FreeRTOSConfig.h.
	Calling this function will set the pxOpenedInterfaceHandle variable.  If,
	after calling this function, pxOpenedInterfaceHandle is equal to NULL, then
	the interface could not be opened. */
	if( pxAllNetworkInterfaces != NULL )
	{
		prvOpenSelectedNetworkInterface( pxAllNetworkInterfaces );
	}

	if( pxOpenedInterfaceHandle != NULL )
	{
		xReturn = pdPASS;
	}

	return xReturn;
}
/*-----------------------------------------------------------*/

static void prvCreateThreadSafeBuffers( void )
{
	/* The buffer used to pass data to be transmitted from a FreeRTOS task to
	the Win32 thread that sends via the WinPCAP library. */
	if( xSendBuffer == NULL)
	{
		xSendBuffer = ( StreamBuffer_t * ) malloc( sizeof( *xSendBuffer ) - sizeof( xSendBuffer->ucArray ) + xSEND_BUFFER_SIZE + 1 );
		configASSERT( xSendBuffer );
		memset( xSendBuffer, '\0', sizeof( *xSendBuffer ) - sizeof( xSendBuffer->ucArray ) );
		xSendBuffer->LENGTH = xSEND_BUFFER_SIZE + 1;
	}

	/* The buffer used to pass received data from the Win32 thread that receives
	via the WinPCAP library to the FreeRTOS task. */
	if( xRecvBuffer == NULL)
	{
		xRecvBuffer = ( StreamBuffer_t * ) malloc( sizeof( *xRecvBuffer ) - sizeof( xRecvBuffer->ucArray ) + xRECV_BUFFER_SIZE + 1 );
		configASSERT( xRecvBuffer );
		memset( xRecvBuffer, '\0', sizeof( *xRecvBuffer ) - sizeof( xRecvBuffer->ucArray ) );
		xRecvBuffer->LENGTH = xRECV_BUFFER_SIZE + 1;
	}
}
/*-----------------------------------------------------------*/

/*_RB_ First parameter should be struct xNetworkInterface, but void * might be more convenient. */
static BaseType_t prvNetworkInterfaceOutput( void *pvNetworkInterfaceIndex, NetworkBufferDescriptor_t * const pxNetworkBuffer, BaseType_t bReleaseAfterSend )
{
size_t xSpace;
uint32_t ulInterfaceIndex = ( uint32_t ) pvNetworkInterfaceIndex;

	iptraceNETWORK_INTERFACE_TRANSMIT();
	configASSERT( xIsCallingFromIPTask() == pdTRUE );

	/* For debugging purposes only - count the number of times each interface
	calls the output function. */
	configASSERT( ulInterfaceIndex < ipconfigMULTI_INTERFACE );
	if( ulInterfaceIndex < ipconfigMULTI_INTERFACE )
	{
		( ulInterfaceOutputCalls[ ulInterfaceIndex ] )++;
	}

	/* Both the length of the data being sent and the actual data being sent
	are placed in the thread safe buffer used to pass data between the FreeRTOS
	tasks and the Win32 thread that sends data via the WinPCAP library.  Drop
	the packet if there is insufficient space in the buffer to hold both. */
	xSpace = uxStreamBufferGetSpace( xSendBuffer );

	if( ( pxNetworkBuffer->xDataLength <= ( ipconfigNETWORK_MTU + ipSIZE_OF_ETH_HEADER ) ) &&
		( xSpace >= ( pxNetworkBuffer->xDataLength + sizeof( pxNetworkBuffer->xDataLength ) ) ) )
	{
		/* First write in the length of the data, then write in the data
		itself. */
		uxStreamBufferAdd( xSendBuffer, 0, ( const uint8_t * ) &( pxNetworkBuffer->xDataLength ), sizeof( pxNetworkBuffer->xDataLength ) );
		uxStreamBufferAdd( xSendBuffer, 0, ( const uint8_t * ) pxNetworkBuffer->pucEthernetBuffer, pxNetworkBuffer->xDataLength );
	}
	else
	{
		FreeRTOS_debug_printf( ( "xNetworkInterfaceOutput: send buffers full to store %lu\n", pxNetworkBuffer->xDataLength ) );
	}

	/* Kick the Tx task in either case in case it doesn't know the buffer is
	full. */
	SetEvent( pvSendEvent );

	/* The buffer has been sent so can be released. */
	if( bReleaseAfterSend != pdFALSE )
	{
		vReleaseNetworkBufferAndDescriptor( pxNetworkBuffer );
	}

	return pdPASS;
}
/*-----------------------------------------------------------*/

void vWinPCapFillInterfaceDescriptor( BaseType_t xInterfaceNumber, NetworkInterface_t *pxInterface )
{
static const char *pcInterfaceNames[] =
{
	"WinPCap_1",
	"WinPCap_2",
	"WinPCap_3"
};
BaseType_t x;

	/* Check xInterfaceNumber is not going to overflow the pcInterfaceNames[]
	array. */
	configASSERT( xInterfaceNumber < ( sizeof( pcInterfaceNames ) / sizeof( char * ) ) );

	/* Populate the NetworkInterface_t structure with the information needed to
	send to and receive from the network interface.  The object pointed to by
	pxInterface must be persistent (global or static, and definitely not
	declared on the stack of a function. */
	memset( pxInterface, '\0', sizeof( NetworkInterface_t ) );

	/* The name is only used for debugging and logging. */
	pxInterface->pcName				= pcInterfaceNames[ xInterfaceNumber ];

	/* The driver can use the pvArgument member for its own purposes.  In this
	case it is used to store an index into the pxVirtualNetworkInterfaces[]
	array that is later used to count the number of times each virtual interface
	calls the output function. */
	pxInterface->pvArgument			= ( void * ) xInterfaceNumber;

	/* Functions to initialise, write to, and check the link status of the
	network interface respectively. */
	pxInterface->pfInitialise		= prvNetworkInterfaceInitialise;
	pxInterface->pfOutput			= prvNetworkInterfaceOutput;
	pxInterface->pfGetPhyLinkStatus = prvGetLinkStatus;

	/* As this single driver handles multiple virtual network interfaces (nodes
	on the network) it is necessary to remember each NetworkInterface_t
	structure so packets that arrive on the real network can be allocated to or
	from one of the multiple virtual nodes. */
	for( x = 0; x < ipconfigMULTI_INTERFACE; x++ )
	{
		if( pxVirtualNetworkInterefaces[ x ] == NULL )
		{
			/* Found a space in the pxVirtualNetworkInterefaces array - store a
			pointer to the NetworkInterface_t structure currently being
			populated. */
			pxVirtualNetworkInterefaces[ x ] = pxInterface;
			break;
		}
	}
}
/*-----------------------------------------------------------*/

static BaseType_t prvGetLinkStatus( NetworkInterface_t *pxInterface )
{
	/* This driver does not use the parameter.  Remove the compiler warning. */
	( void ) pxInterface; /*_RB_ Are any drivers going to use this parameter? */

	/* The WinPCap interface is not a real network interface in its own right,
	and has no PHY to return the status of. */
	return pdTRUE;
}
/*-----------------------------------------------------------*/

static pcap_if_t * prvPrintAvailableNetworkInterfaces( void )
{
pcap_if_t * pxAllNetworkInterfaces = NULL, *xInterface;
int32_t lInterfaceNumber = 1;
char cBuffer[ 512 ];

	if( pcap_findalldevs_ex( PCAP_SRC_IF_STRING, NULL, &pxAllNetworkInterfaces, cErrorBuffer ) == -1 )
	{
		printf( "Could not obtain a list of network interfaces\n%s\n", cErrorBuffer );
		pxAllNetworkInterfaces = NULL;
	}

	if( pxAllNetworkInterfaces != NULL )
	{
		/* Print out the list of network interfaces.  The first in the list
		is interface '1', not interface '0'. */
		for( xInterface = pxAllNetworkInterfaces; xInterface != NULL; xInterface = xInterface->next )
		{
			/* The descriptions of the devices can be full of spaces, clean them
			a little.  printf() can only be used here because the network is not
			up yet - so no other network tasks will be running. */
			printf( "%d. %s\n", lInterfaceNumber, prvRemoveSpaces( cBuffer, sizeof cBuffer, xInterface->name ) );
			printf( "   (%s)\n", prvRemoveSpaces(cBuffer, sizeof cBuffer, xInterface->description ? xInterface->description : "No description" ) );
			printf( "\n" );
			lInterfaceNumber++;
		}
	}

	if( lInterfaceNumber == 1 )
	{
		/* The interface number was never incremented, so the above for() loop
		did not execute meaning no interfaces were found. */
		printf( " \nNo network interfaces were found.\n" );
		pxAllNetworkInterfaces = NULL;
	}

	printf( "The interface that will be opened is set by\n" );
	printf( "\"configNETWORK_INTERFACE_TO_USE\" which should be defined in FreeRTOSConfig.h\n" );
	printf( "Attempting to open interface number %d.\n", xConfigNextworkInterfaceToUse );

	if( ( xConfigNextworkInterfaceToUse < 1L ) || ( xConfigNextworkInterfaceToUse > lInterfaceNumber ) )
	{
		printf( "configNETWORK_INTERFACE_TO_USE is not in the valid range.\n" );

		if( pxAllNetworkInterfaces != NULL )
		{
			/* Free the device list, as no devices are going to be opened. */
			pcap_freealldevs( pxAllNetworkInterfaces );
			pxAllNetworkInterfaces = NULL;
		}
	}

	return pxAllNetworkInterfaces;
}
/*-----------------------------------------------------------*/

static void prvOpenInterface( const char *pucName )
{
static char pucInterfaceName[ 256 ];

	if( pucName != NULL )
	{
		strncpy( pucInterfaceName, pucName, sizeof( pucInterfaceName ) );
	}

	pxOpenedInterfaceHandle = pcap_open(	pucInterfaceName,          	/* The name of the selected interface. */
											ipTOTAL_ETHERNET_FRAME_SIZE, /* The size of the packet to capture. */
											PCAP_OPENFLAG_PROMISCUOUS,	/* Open in promiscuous mode as the MAC and
																		IP address is going to be "simulated", and
																		not be the real MAC and IP address.  This allows
																		traffic to the simulated IP address to be routed
																		to uIP, and traffic to the real IP address to be
																		routed to the Windows TCP/IP stack. */
											100,
											NULL,             			/* No authentication is required as this is
																		not a remote capture session. */
											cErrorBuffer
									   );

	if ( pxOpenedInterfaceHandle == NULL )
	{
		printf( "\n%s is not supported by WinPcap and cannot be opened\n", pucInterfaceName );
	}
	else
	{
		/* Configure the capture filter to allow blocking reads, and to filter
		out packets that are not of interest to this demo. */
		prvConfigureCaptureBehaviour();
	}
}
/*-----------------------------------------------------------*/

static void prvOpenSelectedNetworkInterface( pcap_if_t *pxAllNetworkInterfaces )
{
pcap_if_t *xInterface;
int32_t x;

	/* Walk the list of devices until the selected device is located. */
	xInterface = pxAllNetworkInterfaces;
	for( x = 0L; x < ( xConfigNextworkInterfaceToUse - 1L ); x++ )
	{
		xInterface = xInterface->next;
	}

	/* Open the selected interface. */
	prvOpenInterface( xInterface->name );

	/* The device list is no longer required. */
	pcap_freealldevs( pxAllNetworkInterfaces );
}
/*-----------------------------------------------------------*/

static void prvConfigureCaptureBehaviour( void )
{
struct bpf_program xFilterCode;

	/* As is, the string passed to pcap_compile() assumes no more than 3
	interfaces are being emulated. */
	configASSERT( ipconfigMULTI_INTERFACE <= 3 );

	/* Set up a filter so only the packets of interest are passed to the IP
	stack.  cErrorBuffer is used for convenience to create the string.  Don't
	confuse this with an error message.  If more than one interface is used
	then following interfaces have the MAC address of the preceding interface,
	hence the +1 and +2 on the second and third MAC addresses. */
	sprintf( cErrorBuffer, "broadcast or multicast or ether host %x:%x:%x:%x:%x:%x or ether host %x:%x:%x:%x:%x:%x or ether host %x:%x:%x:%x:%x:%x",
		configMAC_ADDR0, configMAC_ADDR1, configMAC_ADDR2, configMAC_ADDR3, configMAC_ADDR4, configMAC_ADDR5,
		configMAC_ADDR0, configMAC_ADDR1, configMAC_ADDR2, configMAC_ADDR3, configMAC_ADDR4, configMAC_ADDR5 + 1,
		configMAC_ADDR0, configMAC_ADDR1, configMAC_ADDR2, configMAC_ADDR3, configMAC_ADDR4, configMAC_ADDR5 + 2 );

	if( pcap_compile( pxOpenedInterfaceHandle, &xFilterCode, cErrorBuffer, 1, 0 ) < 0 )
	{
		printf( "\nThe packet filter string is invalid\n" );
	}
	else
	{
		if( pcap_setfilter( pxOpenedInterfaceHandle, &xFilterCode ) < 0 )
		{
			printf( "\nAn error occurred setting the packet filter.\n" );
		}
	}

	/* Create the buffers used to pass packets between the FreeRTOS simulator
	and the Win32 threads that are handling WinPCAP. */
	prvCreateThreadSafeBuffers();

	if( pvSendEvent == NULL )
	{
		/* Create event used to signal the Win32 WinPCAP Tx thread. */
		pvSendEvent = CreateEvent( NULL, FALSE, TRUE, NULL );

		/* Create the Win32 thread that handles WinPCAP Rx. */
		vWinPcapRecvThreadHandle = CreateThread(
			NULL,	/* Pointer to thread security attributes. */
			0,		/* Initial thread stack size, in bytes. */
			prvWinPcapRecvThread,	/* Pointer to thread function. */
			NULL,	/* Argument for new thread. */
			0,		/* Creation flags. */
			NULL );

		/* Use the cores that are not used by the FreeRTOS tasks. */
		SetThreadAffinityMask( vWinPcapRecvThreadHandle, ~0x01u );

		/* Create the Win32 thread that handlers WinPCAP Tx. */
		vWinPcapSendThreadHandle = CreateThread(
			NULL,	/* Pointer to thread security attributes. */
			0,		/* initial thread stack size, in bytes. */
			prvWinPcapSendThread,	/* Pointer to thread function. */
			NULL,	/* Argument for new thread. */
			0,		/* Creation flags. */
			NULL );

		/* Use the cores that are not used by the FreeRTOS tasks. */
		SetThreadAffinityMask( vWinPcapSendThreadHandle, ~0x01u );

		/* Create a task that simulates an interrupt in a real system.  This will
		block waiting for packets, then send a message to the IP task when data
		is available. */
		xTaskCreate( prvInterruptSimulatorTask, "MAC_ISR", configMINIMAL_STACK_SIZE, NULL, configMAC_ISR_SIMULATOR_PRIORITY, NULL );
	}
}
/*-----------------------------------------------------------*/

/* WinPCAP function. */
void pcap_callback( u_char *user, const struct pcap_pkthdr *pkt_header, const u_char *pkt_data )
{
	(void)user;

	/* THIS IS CALLED FROM A WINDOWS THREAD - DO NOT ATTEMPT ANY FREERTOS CALLS
	OR TO PRINT OUT MESSAGES HERE. */

	/* Pass data to the FreeRTOS simulator on a thread safe circular buffer. */
	if( ( pkt_header->caplen <= ( ipconfigNETWORK_MTU + ipSIZE_OF_ETH_HEADER ) ) &&
		( uxStreamBufferGetSpace( xRecvBuffer ) >= ( ( ( size_t ) pkt_header->caplen ) + sizeof( *pkt_header ) ) ) )
	{
		uxStreamBufferAdd( xRecvBuffer, 0, ( const uint8_t* ) pkt_header, sizeof( *pkt_header ) );
		uxStreamBufferAdd( xRecvBuffer, 0, ( const uint8_t* ) pkt_data, ( size_t ) pkt_header->caplen );
	}
}
/*-----------------------------------------------------------*/

DWORD WINAPI prvWinPcapRecvThread ( void *pvParam )
{
	( void ) pvParam;

	/* THIS IS A WINDOWS THREAD - DO NOT ATTEMPT ANY FREERTOS CALLS	OR TO PRINT
	OUT MESSAGES HERE. */

	for( ;; )
	{
		pcap_dispatch( pxOpenedInterfaceHandle, 1, pcap_callback, ( u_char * ) "mydata" );
	}
}
/*-----------------------------------------------------------*/

DWORD WINAPI prvWinPcapSendThread( void *pvParam )
{
size_t xLength;
uint8_t ucBuffer[ ipconfigNETWORK_MTU + ipSIZE_OF_ETH_HEADER ];
static char cErrorMessage[ 1024 ];
const DWORD xMaxMSToWait = 1000;

	/* THIS IS A WINDOWS THREAD - DO NOT ATTEMPT ANY FREERTOS CALLS	OR TO PRINT
	OUT MESSAGES HERE. */

	/* Remove compiler warnings about unused parameters. */
	( void ) pvParam;

	for( ;; )
	{
		/* Wait until notified of something to send. */
		WaitForSingleObject( pvSendEvent, xMaxMSToWait );

		/* Is there more than the length value stored in the circular buffer
		used to pass data from the FreeRTOS simulator into this Win32 thread? */
		while( uxStreamBufferGetSize( xSendBuffer ) > sizeof( xLength ) )
		{
			uxStreamBufferGet( xSendBuffer, 0, ( uint8_t * ) &xLength, sizeof( xLength ), pdFALSE );
			uxStreamBufferGet( xSendBuffer, 0, ( uint8_t* ) ucBuffer, xLength, pdFALSE );
			if( pcap_sendpacket( pxOpenedInterfaceHandle, ucBuffer, xLength  ) != 0 )
			{
				ulWinPCAPSendFailures++;
			}
		}
	}
}
/*-----------------------------------------------------------*/

static void prvInterruptSimulatorTask( void *pvParameters )
{
struct pcap_pkthdr xHeader;
static struct pcap_pkthdr *pxHeader;
const uint8_t *pucPacketData;
uint8_t ucRecvBuffer[ ipconfigNETWORK_MTU + ipSIZE_OF_ETH_HEADER ];
NetworkBufferDescriptor_t *pxNetworkBuffer;
IPStackEvent_t xRxEvent = { eNetworkRxEvent, NULL };
eFrameProcessingResult_t eResult;

	/* Remove compiler warnings about unused parameters. */
	( void ) pvParameters;

	for( ;; )
	{
		/* Does the circular buffer used to pass data from the Win32 thread that
		handles WinPCAP Rx into the FreeRTOS simulator contain another packet? */
		if( uxStreamBufferGetSize( xRecvBuffer ) > sizeof( xHeader ) )
		{
			/* Get the next packet. */
			uxStreamBufferGet( xRecvBuffer, 0, (uint8_t*)&xHeader, sizeof( xHeader ), pdFALSE );
			uxStreamBufferGet( xRecvBuffer, 0, (uint8_t*)ucRecvBuffer, ( size_t ) xHeader.len, pdFALSE );
			pucPacketData = ucRecvBuffer;
			pxHeader = &xHeader;

			iptraceNETWORK_INTERFACE_RECEIVE();

			eResult = ipCONSIDER_FRAME_FOR_PROCESSING( pucPacketData );
			if( eResult == eProcessBuffer )
			{
				/* Will the data fit into the frame buffer? */
				if( pxHeader->len <= ipTOTAL_ETHERNET_FRAME_SIZE )
				{
					/* Obtain a buffer into which the data can be placed.  This
					is only	an interrupt simulator, not a real interrupt, so it
					is ok to call the task level function here, but note that
					some buffer implementations cannot be called from a real
					interrupt. */
					pxNetworkBuffer = pxGetNetworkBufferWithDescriptor( pxHeader->len, 0 );

					if( pxNetworkBuffer != NULL )
					{
						memcpy( pxNetworkBuffer->pucEthernetBuffer, pucPacketData, pxHeader->len );
						pxNetworkBuffer->xDataLength = ( size_t ) pxHeader->len;

						/* This single real interface in used to create multiple
						virtual interfaces by accessing the raw Ethernet data.
						Use the MAC address of the received frame (which is at
						the start of the Ethernet data) to look up which of the
						virtual interfaces this packet is for. */
						pxNetworkBuffer->pxInterface = prvFindVirtualInterface( pucPacketData );

						/* If a broadcast or multicast MAC address was used then
						an interface will not have been found. */
						if( pxNetworkBuffer->pxInterface != NULL )
						{
							pxNetworkBuffer->pxEndPoint = pxNetworkBuffer->pxInterface->pxEndPoint; /*_RB_ I'm not sure what I'm supposed to fill in here.  The SAM driver completes pxInterface, but prvProcessEthernetPacket() checks pxEndPoint. */
						}

						#if( niDISRUPT_PACKETS == 1 )
						{
							pxNetworkBuffer = vRxFaultInjection( pxNetworkBuffer, pucPacketData );
						}
						#endif /* niDISRUPT_PACKETS */

						if( pxNetworkBuffer != NULL )
						{
							xRxEvent.pvData = ( void * ) pxNetworkBuffer;

							/* Data was received and stored.  Send a message to
							the IP task to let it know. */
							if( xSendEventStructToIPTask( &xRxEvent, ( TickType_t ) 0 ) == pdFAIL )
							{
								/* The buffer could not be sent to the stack so
								must be released again.  This is only an
								interrupt simulator, not a real interrupt, so it
								is ok to use the task level function here, but
								note no all buffer implementations will allow
								this function to be executed from a real
								interrupt. */
								vReleaseNetworkBufferAndDescriptor( pxNetworkBuffer );
								iptraceETHERNET_RX_EVENT_LOST();
							}
						}
						else
						{
							/* The packet was already released or stored inside
							vRxFaultInjection().  Don't release it here. */
						}
					}
					else
					{
						iptraceETHERNET_RX_EVENT_LOST();
					}
				}
				else
				{
					/* Log that a packet was dropped because it would have
					overflowed the buffer, but there may be more buffers to
					process. */
				}
			}
		}
		else
		{
			/* There is no real way of simulating an interrupt.  Make sure
			other tasks can run. */
			vTaskDelay( configWINDOWS_MAC_INTERRUPT_SIMULATOR_DELAY );
		}
	}
}
/*-----------------------------------------------------------*/

static NetworkInterface_t *prvFindVirtualInterface( const uint8_t *pucMACAddressBytes )
{
BaseType_t x;
NetworkInterface_t *pxReturn = NULL;

	for( x = 0; x < ipconfigMULTI_INTERFACE; x++ )
	{
		if( pxVirtualNetworkInterefaces[ x ] == NULL )
		{
			/* No more interfaces are defined. */
			break;
		}

		configASSERT( pxVirtualNetworkInterefaces[ x ]->pxEndPoint );

		/* Does the MAC address of this interface match the MAC address passed
		in pucMACAddressBytes. */
		if( memcmp( ( void * ) pucMACAddressBytes, ( void * ) pxVirtualNetworkInterefaces[ x ]->pxEndPoint->xMACAddress.ucBytes, ipMAC_ADDRESS_LENGTH_BYTES ) == 0 )
		{
			pxReturn = pxVirtualNetworkInterefaces[ x ];
			break;
		}
	}

	return pxReturn;
}
/*-----------------------------------------------------------*/

static const char *prvRemoveSpaces( char *pcBuffer, int aBuflen, const char *pcMessage )
{
	char *pcTarget = pcBuffer;

	/* Utility function used to for map messages being printed only. */
	while( ( *pcMessage != 0 ) && ( pcTarget < ( pcBuffer + aBuflen - 1 ) ) )
	{
		*( pcTarget++ ) = *pcMessage;

		if( isspace( *pcMessage ) != pdFALSE )
		{
			while( isspace( *pcMessage ) != pdFALSE )
			{
				pcMessage++;
			}
		}
		else
		{
			pcMessage++;
		}
	}

	*pcTarget = '\0';

	return pcBuffer;
}
/*-----------------------------------------------------------*/

