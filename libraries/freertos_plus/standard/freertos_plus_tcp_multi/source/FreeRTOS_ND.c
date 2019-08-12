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


/* FreeRTOS includes. */
#include "FreeRTOS.h"
#include "task.h"

/* FreeRTOS+TCP includes. */
#include "FreeRTOS_IP.h"
#include "FreeRTOS_Sockets.h"
#include "FreeRTOS_IP_Private.h"
#include "FreeRTOS_ARP.h"
#include "FreeRTOS_UDP_IP.h"
#include "FreeRTOS_Routing.h"
#include "FreeRTOS_ND.h"
#if( ipconfigUSE_LLMNR == 1 )
	#include "FreeRTOS_DNS.h"
#endif /* ipconfigUSE_LLMNR */
#include "NetworkBufferManagement.h"

#if( ipconfigUSE_IPv6 != 0 )

#define ndICMPv6_FLAG_SOLICITED					0x40000000uL
#define ndICMPv6_FLAG_UPDATE					0x20000000uL

/* Options that can be send after the ICMPv6 header. */
#define	ndICMP_SOURCE_LINK_LAYER_ADDRESS		1
#define	ndICMP_TARGET_LINK_LAYER_ADDRESS		2
#define	ndICMP_PREFIX_INFORMATION				3
#define	ndICMP_REDIRECTED_HEADER				4
#define	ndICMP_MTU_OPTION						5


/* A block time of 0 simply means "don't block". */
#define ndDONT_BLOCK				( ( TickType_t ) 0 )

#define ADVERTISE_PACKET_SIZE	64

/* The character used to fill ICMP echo requests, and therefore also the
character expected to fill ICMP echo replies. */
#define ndECHO_DATA_FILL_BYTE						'x'

// All nodes on the local network segment
static const uint8_t pcLOCAL_NETWORK_MULTICAST_IP[ 16 ] = { 0xff, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01 };
static const uint8_t pcLOCAL_NETWORK_MULTICAST_MAC[ ipMAC_ADDRESS_LENGTH_BYTES ] = { 0x01, 0x80, 0xC2, 0x00, 0x00, 0x00 };

/*
 * Lookup an MAC address in the ND cache from the IP address.
 */
static eARPLookupResult_t prvCacheLookup( IPv6_Address_t *pxAddressToLookup, MACAddress_t * const pxMACAddress, NetworkEndPoint_t **ppxEndPoint );

/*-----------------------------------------------------------*/

/* The ND cache. */
static NDCacheRow_t xNDCache[ ipconfigND_CACHE_ENTRIES ];


eARPLookupResult_t eNDGetCacheEntry( IPv6_Address_t *pxIPAddress, MACAddress_t * const pxMACAddress, struct xNetworkEndPoint **ppxEndPoint )
{
eARPLookupResult_t eReturn;
/* For testing now, fill in Hein's laptop: 9c 5c 8e 38 06 6c */
//static const unsigned char testMAC[] = { 0x9C, 0x5C, 0x8E, 0x38, 0x06, 0x6C };
//
//	memcpy( pxMACAddress->ucBytes, testMAC, sizeof testMAC );

	eReturn = prvCacheLookup( pxIPAddress, pxMACAddress, ppxEndPoint );

	return eReturn;
}
/*-----------------------------------------------------------*/

void vNDRefreshCacheEntry( const MACAddress_t * pxMACAddress, const IPv6_Address_t *pxIPAddress, NetworkEndPoint_t *pxEndPoint )
{
BaseType_t x;
BaseType_t xFreeEntry = -1, xEntryFound = -1;
	/* For each entry in the ARP cache table. */
	for( x = 0; x < ipconfigND_CACHE_ENTRIES; x++ )
	{
		if( xNDCache[ x ].ucValid == ( uint8_t )pdFALSE )
		{
			if( xFreeEntry == -1 )
			{
				xFreeEntry = x;
			}
		}
		else if( memcmp( xNDCache[ x ].xIPAddress.ucBytes, pxIPAddress->ucBytes, sizeof( IPv6_Address_t ) ) == 0 )
		{
			xEntryFound = x;
			break;
		}
	}
	if( xEntryFound < 0 )
	{
		xEntryFound = xFreeEntry;
	}
	if( xEntryFound >= 0 )
	{
		/* Copy the IP-address. */
		memcpy( xNDCache[ xEntryFound ].xIPAddress.ucBytes, pxIPAddress->ucBytes, sizeof( IPv6_Address_t ) );
		/* Copy the MAC-address. */
		memcpy( xNDCache[ xEntryFound ].xMACAddress.ucBytes, pxMACAddress->ucBytes, sizeof( MACAddress_t ) );
		xNDCache[ xEntryFound ].pxEndPoint = pxEndPoint;
		xNDCache[ xEntryFound ].ucAge = ( uint8_t ) ipconfigMAX_ARP_AGE;
		xNDCache[ xEntryFound ].ucValid = ( uint8_t ) pdTRUE;
		/* The format %pip will print a IPv6 address ( if printf-stdarg.c is included ). */
		FreeRTOS_printf( ( "vNDRefreshCacheEntry[ %d ] %pip with %02x:%02x:%02x\n",
			( int ) xEntryFound,
			pxIPAddress->ucBytes,
			pxMACAddress->ucBytes[ 3 ],
			pxMACAddress->ucBytes[ 4 ],
			pxMACAddress->ucBytes[ 5 ] ) );
	}
	else
	{
		FreeRTOS_printf( ( "vNDRefreshCacheEntry: %pip not found\n", pxIPAddress->ucBytes ) );
	}
}
/*-----------------------------------------------------------*/

void FreeRTOS_ClearND( void )
{
	memset( xNDCache, '\0', sizeof( xNDCache ) );
}
/*-----------------------------------------------------------*/

static eARPLookupResult_t prvCacheLookup( IPv6_Address_t *pxAddressToLookup, MACAddress_t * const pxMACAddress, NetworkEndPoint_t **ppxEndPoint )
{
BaseType_t x;
eARPLookupResult_t eReturn = eARPCacheMiss;
	/* For each entry in the ARP cache table. */
	for( x = 0; x < ipconfigND_CACHE_ENTRIES; x++ )
	{
		if( xNDCache[ x ].ucValid == ( uint8_t )pdFALSE )
		{
		}
		else if( memcmp( xNDCache[ x ].xIPAddress.ucBytes, pxAddressToLookup->ucBytes, sizeof( IPv6_Address_t ) ) == 0 )
		{
			memcpy( pxMACAddress->ucBytes, xNDCache[ x ].xMACAddress.ucBytes, sizeof( MACAddress_t ) );
			eReturn = eARPCacheHit;
			*ppxEndPoint = xNDCache[ x ].pxEndPoint;
			FreeRTOS_printf( ( "prvCacheLookup[ %d ] %pip with %02x:%02x:%02x\n",
				( int ) x,
				pxAddressToLookup->ucBytes,
				pxMACAddress->ucBytes[ 3 ],
				pxMACAddress->ucBytes[ 4 ],
				pxMACAddress->ucBytes[ 5 ] ) );
			break;
		}
	}
	if( eReturn == eARPCacheMiss )
	{
		FreeRTOS_printf( ( "prvCacheLookup %pip Miss\n", pxAddressToLookup->ucBytes ) );
		*ppxEndPoint = NULL;
	}
	return eReturn;
}

#if( ( ipconfigHAS_PRINTF != 0 ) || ( ipconfigHAS_DEBUG_PRINTF != 0 ) )

	void FreeRTOS_PrintNDCache( void )
	{
	BaseType_t x, xCount = 0;

		/* Loop through each entry in the ARP cache. */
		for( x = 0; x < ipconfigARP_CACHE_ENTRIES; x++ )
		{
			if( xNDCache[ x ].ucValid )
			{
				/* See if the MAC-address also matches, and we're all happy */
				FreeRTOS_printf( ( "ND %2ld: %3u - %pip : %02x:%02x:%02x : %02x:%02x:%02x\n",
					x,
					xNDCache[ x ].ucAge,
					xNDCache[ x ].xIPAddress.ucBytes,
					xNDCache[ x ].xMACAddress.ucBytes[0],
					xNDCache[ x ].xMACAddress.ucBytes[1],
					xNDCache[ x ].xMACAddress.ucBytes[2],
					xNDCache[ x ].xMACAddress.ucBytes[3],
					xNDCache[ x ].xMACAddress.ucBytes[4],
					xNDCache[ x ].xMACAddress.ucBytes[5] ) );
				xCount++;
			}
		}

		FreeRTOS_printf( ( "Arp has %ld entries\n", xCount ) );
	}

#endif /* ( ipconfigHAS_PRINTF != 0 ) || ( ipconfigHAS_DEBUG_PRINTF != 0 ) */

static void prvReturnICMP_IPv6( NetworkBufferDescriptor_t * const pxNetworkBuffer, size_t uxICMPSize )
{
NetworkEndPoint_t *pxEndPoint = pxNetworkBuffer->pxEndPoint;
ICMPPacket_IPv6_t *pxICMPPacket = ( ICMPPacket_IPv6_t * )pxNetworkBuffer->pucEthernetBuffer;
ICMPHeader_IPv6_t *pxICMPHeader_IPv6 = ( ICMPHeader_IPv6_t * )&( pxICMPPacket->xICMPHeader_IPv6 );

	memcpy( pxICMPPacket->xIPHeader.xDestinationIPv6Address.ucBytes, pxICMPPacket->xIPHeader.xSourceIPv6Address.ucBytes, sizeof( IPv6_Address_t ) );
	memcpy( pxICMPPacket->xIPHeader.xSourceIPv6Address.ucBytes, pxEndPoint->ulIPAddress_IPv6.ucBytes, sizeof( IPv6_Address_t ) );
	pxICMPPacket->xIPHeader.usPayloadLength = FreeRTOS_htons( uxICMPSize );

	/* Important: tell NIC driver how many bytes must be sent */
	pxNetworkBuffer->xDataLength = ( size_t ) ( ipSIZE_OF_ETH_HEADER + ipSIZE_OF_IPv6_HEADER + uxICMPSize );

	pxICMPHeader_IPv6->usChecksum = 0;
	/* calculate the UDP checksum for outgoing package */
	usGenerateProtocolChecksum( ( uint8_t* ) pxNetworkBuffer->pucEthernetBuffer, pxNetworkBuffer->xDataLength, pdTRUE );

// FreeRTOS_printf( ( "ICMPv6 return %d bytes\n", pxNetworkBuffer->xDataLength ) );
	/* This function will fill in the eth addresses and send the packet */
	vReturnEthernetFrame( pxNetworkBuffer, pdFALSE );
}
/*-----------------------------------------------------------*/

void nd_test()
{
NetworkBufferDescriptor_t *pxNetworkBuffer;
size_t uxNeededSize;
IPv6_Address_t xIPAddress;
NetworkEndPoint_t *pxEndPoint;
BaseType_t xIndex;
const char *ip_address[] = {
	"fe80::9355:69c7:585a:afe7",	/* raspberry ff02::1:ff5a:afe7, 33:33:ff:5a:af:e7 */
	"fe80::6816:5e9b:80a0:9edb",	/* laptop Hein */
};
	for( xIndex = 0; xIndex < ARRAY_SIZE( ip_address ); xIndex++ )
	{
		FreeRTOS_inet_pton6( ip_address[ xIndex ], xIPAddress.ucBytes );
FreeRTOS_printf( ( "nd_test: Looking up %pip\n", xIPAddress.ucBytes ) );
		pxEndPoint = FreeRTOS_FindEndPointOnIP_IPv6( &xIPAddress );
		if( pxEndPoint != NULL )
		{
			uxNeededSize = ( size_t ) ( ipSIZE_OF_ETH_HEADER + ipSIZE_OF_IPv6_HEADER + sizeof( ICMPHeader_IPv6_t ) );
			pxNetworkBuffer = pxGetNetworkBufferWithDescriptor( uxNeededSize, ndDONT_BLOCK );
			if( pxNetworkBuffer != NULL )
			{
				pxNetworkBuffer->pxEndPoint = pxEndPoint;
				vNDGenerateRequestPacket( pxNetworkBuffer, &xIPAddress );
			}
		}
		else
		{
			FreeRTOS_printf( ( "nd_test: can not find and endpoint for %pip\n", xIPAddress.ucBytes ) );
		}
	}
}

/*
 * Send out an ND request for the IPv6 address contained in pxNetworkBuffer, and
 * add an entry into the ND table that indicates that an ND reply is
 * outstanding so re-transmissions can be generated.
 */
void vNDGenerateRequestPacket( NetworkBufferDescriptor_t * const pxNetworkBuffer, IPv6_Address_t *pxIPAddress )
{
ICMPPacket_IPv6_t *pxICMPPacket;
ICMPHeader_IPv6_t *xICMPHeader_IPv6;
NetworkEndPoint_t *pxEndPoint = pxNetworkBuffer->pxEndPoint;
size_t uxNeededSize;
IPv6_Address_t xTargetIPAddress;
MACAddress_t xMultiCastMacAddress;
NetworkBufferDescriptor_t *pxDescriptor = pxNetworkBuffer;

	configASSERT( pxEndPoint != NULL );
	uxNeededSize = ( size_t ) ( ipSIZE_OF_ETH_HEADER + ipSIZE_OF_IPv6_HEADER + sizeof( ICMPHeader_IPv6_t ) );
	if( pxDescriptor->xDataLength < uxNeededSize )
	{
		pxDescriptor = pxDuplicateNetworkBufferWithDescriptor( pxDescriptor, uxNeededSize );
		if( pxDescriptor == NULL )
		{
			return;
		}
	}
	pxICMPPacket = ( ICMPPacket_IPv6_t * )pxDescriptor->pucEthernetBuffer;
	xICMPHeader_IPv6 = ( ICMPHeader_IPv6_t * )&( pxICMPPacket->xICMPHeader_IPv6 );

	pxDescriptor->xDataLength = uxNeededSize;

	/* Set the multi-cast MAC-address. */
	xMultiCastMacAddress.ucBytes[ 0 ] = 0x33;
	xMultiCastMacAddress.ucBytes[ 1 ] = 0x33;
	xMultiCastMacAddress.ucBytes[ 2 ] = 0xff;
	xMultiCastMacAddress.ucBytes[ 3 ] = pxIPAddress->ucBytes[ 13 ];
	xMultiCastMacAddress.ucBytes[ 4 ] = pxIPAddress->ucBytes[ 14 ];
	xMultiCastMacAddress.ucBytes[ 5 ] = pxIPAddress->ucBytes[ 15 ];

	/* Set Ethernet header. Will be swapped. */
	memcpy( pxICMPPacket->xEthernetHeader.xSourceAddress.ucBytes, xMultiCastMacAddress.ucBytes, ipMAC_ADDRESS_LENGTH_BYTES );
	memcpy( pxICMPPacket->xEthernetHeader.xDestinationAddress.ucBytes, pxEndPoint->xMACAddress.ucBytes, ipMAC_ADDRESS_LENGTH_BYTES );
	pxICMPPacket->xEthernetHeader.usFrameType = ipIPv6_FRAME_TYPE;

	/* Set IP-header. */
	pxICMPPacket->xIPHeader.ucVersionTrafficClass = 0x60;
	pxICMPPacket->xIPHeader.ucTrafficClassFlow = 0;
	pxICMPPacket->xIPHeader.usFlowLabel = 0;
	pxICMPPacket->xIPHeader.usPayloadLength = FreeRTOS_htons( 32 );
	pxICMPPacket->xIPHeader.ucNextHeader = ipPROTOCOL_ICMP_IPv6;
	pxICMPPacket->xIPHeader.ucHopLimit = 255;

	memcpy( pxICMPPacket->xIPHeader.xSourceIPv6Address.ucBytes, pxEndPoint->ulIPAddress_IPv6.ucBytes, 16 );

	//ff02::1:ff5a:afe7
	memset( xTargetIPAddress.ucBytes, '\0', sizeof( xTargetIPAddress.ucBytes ) );
	xTargetIPAddress.ucBytes[  0 ] = 0xff;
	xTargetIPAddress.ucBytes[  1 ] = 0x02;
	xTargetIPAddress.ucBytes[ 11 ] = 0x01;
	xTargetIPAddress.ucBytes[ 12 ] = 0xff;
	xTargetIPAddress.ucBytes[ 13 ] = pxIPAddress->ucBytes[ 13 ];
	xTargetIPAddress.ucBytes[ 14 ] = pxIPAddress->ucBytes[ 14 ];
	xTargetIPAddress.ucBytes[ 15 ] = pxIPAddress->ucBytes[ 15 ];
	memcpy( pxICMPPacket->xIPHeader.xDestinationIPv6Address.ucBytes, xTargetIPAddress.ucBytes, 16 );

	/* Set ICMP header. */
	memset( xICMPHeader_IPv6, '\0', sizeof( *xICMPHeader_IPv6 ) );
	xICMPHeader_IPv6->ucTypeOfMessage = ipICMP_NEIGHBOR_SOLICITATION_IPv6;
	memcpy( xICMPHeader_IPv6->xIPv6_Address.ucBytes, pxIPAddress->ucBytes, ipSIZE_OF_IPv6_ADDRESS );
	xICMPHeader_IPv6->ucOptionType = ndICMP_SOURCE_LINK_LAYER_ADDRESS;
	xICMPHeader_IPv6->ucOptionLength = 1;	/* times 8 bytes. */
	memcpy( xICMPHeader_IPv6->ucOptionBytes, pxEndPoint->xMACAddress.ucBytes, ipMAC_ADDRESS_LENGTH_BYTES );

	/* Checmsums. */
	xICMPHeader_IPv6->usChecksum = 0;
	/* calculate the UDP checksum for outgoing package */
	usGenerateProtocolChecksum( ( uint8_t* ) pxDescriptor->pucEthernetBuffer, pxDescriptor->xDataLength, pdTRUE );

// FreeRTOS_printf( ( "ICMPv6 return %d bytes\n", pxDescriptor->xDataLength ) );
	/* This function will fill in the eth addresses and send the packet */
	vReturnEthernetFrame( pxDescriptor, pdTRUE );
}
/*-----------------------------------------------------------*/

#if ( ipconfigSUPPORT_OUTGOING_PINGS == 1 )

	BaseType_t FreeRTOS_SendPingRequestIPv6( IPv6_Address_t *pxIPAddress, size_t xNumberOfBytesToSend, TickType_t xBlockTimeTicks )
	{
	NetworkBufferDescriptor_t *pxNetworkBuffer;
	EthernetHeader_t *pxEthernetHeader;
	ICMPPacket_IPv6_t *pxICMPPacket;
	ICMPEcho_IPv6_t *pxICMPHeader;
	BaseType_t xReturn = pdFAIL;
	static uint16_t usSequenceNumber = 0;
	uint8_t *pucChar;
	IPStackEvent_t xStackTxEvent = { eStackTxEvent, NULL };
	NetworkEndPoint_t *pxEndPoint;
	size_t uxPacketLength;
	
		pxEndPoint = FreeRTOS_FindEndPointOnIP_IPv6( pxIPAddress );
		if( pxEndPoint == NULL )
		{
			FreeRTOS_printf( ( "SendPingRequestIPv6: No routing found" ) );
			return 0;
		}
		uxPacketLength = sizeof( EthernetHeader_t ) + sizeof( IPHeader_IPv6_t ) + sizeof( ICMPEcho_IPv6_t ) + xNumberOfBytesToSend;
		pxNetworkBuffer = pxGetNetworkBufferWithDescriptor( uxPacketLength, xBlockTimeTicks );

		if( pxNetworkBuffer != NULL )
		{
		BaseType_t xEnoughSpace;

memset( pxNetworkBuffer->pucEthernetBuffer, '\0', pxNetworkBuffer->xDataLength);

			pxNetworkBuffer->pxEndPoint = pxEndPoint;
			pxICMPPacket = ( ICMPPacket_IPv6_t * )pxNetworkBuffer->pucEthernetBuffer;
			xEnoughSpace = xNumberOfBytesToSend < ( ( ipconfigNETWORK_MTU - sizeof( IPHeader_IPv6_t ) ) - sizeof( ICMPEcho_IPv6_t ) );
			if( ( uxGetNumberOfFreeNetworkBuffers() >= 3 ) && ( xNumberOfBytesToSend >= 1 ) && ( xEnoughSpace != pdFALSE ) )
			{
				pxICMPHeader = ipPOINTER_CAST( ICMPEcho_IPv6_t *, &( pxICMPPacket->xICMPHeader_IPv6 ) );
				usSequenceNumber++;

				pxICMPPacket->xEthernetHeader.usFrameType = ipIPv6_FRAME_TYPE;

				pxICMPPacket->xIPHeader.usPayloadLength = FreeRTOS_htons( sizeof( ICMPEcho_IPv6_t ) + xNumberOfBytesToSend );
				memcpy( pxICMPPacket->xIPHeader.xDestinationIPv6Address.ucBytes, pxIPAddress->ucBytes, ipSIZE_OF_IPv6_ADDRESS );
				memcpy( pxICMPPacket->xIPHeader.xSourceIPv6Address.ucBytes, pxEndPoint->ulIPAddress_IPv6.ucBytes, ipSIZE_OF_IPv6_ADDRESS );
				pxICMPPacket->xIPHeader.ucVersionTrafficClass = 0x60;
				pxICMPPacket->xIPHeader.ucNextHeader = ipPROTOCOL_ICMP_IPv6;

				/* Fill in the basic header information. */
				pxICMPHeader->ucTypeOfMessage = ipICMP_PING_REQUEST_IPv6;
				pxICMPHeader->ucTypeOfService = 0;
				pxICMPHeader->usIdentifier = usSequenceNumber;
				pxICMPHeader->usSequenceNumber = usSequenceNumber;

				/* Find the start of the data. */
				pucChar = ( uint8_t * ) pxICMPHeader;
				pucChar = &( pucChar[ sizeof( ICMPEcho_IPv6_t ) ] );

				/* Just memset the data to a fixed value. */
				memset( pucChar, ( int ) ndECHO_DATA_FILL_BYTE, xNumberOfBytesToSend );

				/* The message is complete, IP and checksum's are handled by
				vProcessGeneratedUDPPacket */
				pxNetworkBuffer->pucEthernetBuffer[ ipSOCKET_OPTIONS_OFFSET ] = FREERTOS_SO_UDPCKSUM_OUT;
				pxNetworkBuffer->ulIPAddress = 0uL;
				memcpy( pxNetworkBuffer->xIPv6_Address.ucBytes, pxIPAddress->ucBytes, ipSIZE_OF_IPv6_ADDRESS );
				pxNetworkBuffer->usPort = ipPACKET_CONTAINS_ICMP_DATA;
//				pxNetworkBuffer->xDataLength = xNumberOfBytesToSend + sizeof( ICMPEcho_IPv6_t );
				/* Let vProcessGeneratedUDPPacket() know that this is an ICMP packet. */
				pxNetworkBuffer->usPort = ipPACKET_CONTAINS_ICMP_DATA;

				pxEthernetHeader = ( EthernetHeader_t * ) pxNetworkBuffer->pucEthernetBuffer;
				pxEthernetHeader->usFrameType = ipIPv6_FRAME_TYPE;

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

eFrameProcessingResult_t prvProcessICMPMessage_IPv6( NetworkBufferDescriptor_t * const pxNetworkBuffer )
{
ICMPPacket_IPv6_t *pxICMPPacket = ( ICMPPacket_IPv6_t * )pxNetworkBuffer->pucEthernetBuffer;
ICMPHeader_IPv6_t *xICMPHeader_IPv6 = ( ICMPHeader_IPv6_t * )&( pxICMPPacket->xICMPHeader_IPv6 );
NetworkEndPoint_t *pxEndPoint = pxNetworkBuffer->pxEndPoint;
size_t uxNeededSize;

	if( xICMPHeader_IPv6->ucTypeOfMessage != ipICMP_PING_REQUEST_IPv6 )
	{
		FreeRTOS_printf( ( "ICMPv6 message %d from %02x:%02x:%02x:%02x:%02x:%02x\n",
			xICMPHeader_IPv6->ucTypeOfMessage,
			xICMPHeader_IPv6->ucOptionType,
			xICMPHeader_IPv6->ucOptionLength,
			xICMPHeader_IPv6->ucOptionBytes[ 0 ],
			xICMPHeader_IPv6->ucOptionBytes[ 1 ],
			xICMPHeader_IPv6->ucOptionBytes[ 2 ],
			xICMPHeader_IPv6->ucOptionBytes[ 3 ] ) );
	}
	if( pxEndPoint != NULL )
	{
		switch( xICMPHeader_IPv6->ucTypeOfMessage )
		{
			case ipICMP_PING_REQUEST_IPv6 :
				{
				size_t uxICMPSize = FreeRTOS_ntohs( pxICMPPacket->xIPHeader.usPayloadLength );

					uxNeededSize = ( size_t ) ( ipSIZE_OF_ETH_HEADER + ipSIZE_OF_IPv6_HEADER + uxICMPSize );
					if( uxNeededSize > pxNetworkBuffer->xDataLength )
					{
						FreeRTOS_printf( ("Too small\n" ) );
						break;
					}

					xICMPHeader_IPv6->ucTypeOfMessage = ipICMP_PING_REPLY_IPv6;
					prvReturnICMP_IPv6( pxNetworkBuffer, uxICMPSize );
				}
				break;
			case ipICMP_PING_REPLY_IPv6:
				{
				ePingReplyStatus_t eStatus = eSuccess;
				ICMPEcho_IPv6_t * pxICMPEchoHeader = ( ICMPEcho_IPv6_t * ) xICMPHeader_IPv6;
				size_t usDataLength, usCount;
				const uint8_t * pucByte;
					FreeRTOS_printf( ("ping %pip answered\n", pxICMPPacket->xIPHeader.xSourceIPv6Address.ucBytes ) );

					/* Find the total length of the IP packet. */
					usDataLength = FreeRTOS_ntohs( pxICMPPacket->xIPHeader.usPayloadLength );
					usDataLength = usDataLength - sizeof( *pxICMPEchoHeader );

					/* Find the first byte of the data within the ICMP packet. */
					pucByte = ( const uint8_t * ) pxICMPEchoHeader;
					pucByte = &( pucByte[ sizeof( *pxICMPEchoHeader ) ] );

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
					vApplicationPingReplyHook( eStatus, pxICMPEchoHeader->usIdentifier );
				}
				break;
			case ipICMP_NEIGHBOR_SOLICITATION_IPv6 :
				{
				size_t uxICMPSize;
	/*
		uint8_t ucTypeOfMessage;       //  0 +  1 = 1
		uint8_t ucTypeOfService;       //  1 +  1 = 2
		uint16_t usChecksum;           //  2 +  2 = 4
		uint32_t ulReserved;           //  4 +  4 = 8
		IPv6_Address_t xIPv6_Address;  //  8 + 16 = 24
		uint8_t ucOptionType;          // 24 +  1 = 25
		uint8_t ucOptionLength;        // 25 +  1 = 26
		uint8_t ucOptionBytes[6];      // 26 +  6 = 32
	*/
					uxICMPSize = sizeof( ICMPHeader_IPv6_t );
					uxNeededSize = ( size_t ) ( ipSIZE_OF_ETH_HEADER + ipSIZE_OF_IPv6_HEADER + uxICMPSize );
					if( uxNeededSize > pxNetworkBuffer->xDataLength )
					{
						FreeRTOS_printf( ("Too small\n" ) );
						break;
					}
					xICMPHeader_IPv6->ucTypeOfMessage = ipICMP_NEIGHBOR_ADVERTISEMENT_IPv6;
					xICMPHeader_IPv6->ucTypeOfService = 0;
					xICMPHeader_IPv6->ulReserved = FreeRTOS_htonl( ndICMPv6_FLAG_SOLICITED | ndICMPv6_FLAG_UPDATE );

					/* Type of option. */
					xICMPHeader_IPv6->ucOptionType = ndICMP_TARGET_LINK_LAYER_ADDRESS;
					/* Length of option in units of 8 bytes. */
					xICMPHeader_IPv6->ucOptionLength = 1;
					memcpy( xICMPHeader_IPv6->ucOptionBytes, pxEndPoint->xMACAddress.ucBytes, sizeof( MACAddress_t ) );
					pxICMPPacket->xIPHeader.ucHopLimit = 255;
					memcpy( xICMPHeader_IPv6->xIPv6_Address.ucBytes, pxEndPoint->ulIPAddress_IPv6.ucBytes, sizeof( xICMPHeader_IPv6->xIPv6_Address.ucBytes ) );

					prvReturnICMP_IPv6( pxNetworkBuffer, uxICMPSize );
				}

				break;
			case ipICMP_NEIGHBOR_ADVERTISEMENT_IPv6:
				{
					vNDRefreshCacheEntry( ( MACAddress_t * ) xICMPHeader_IPv6->ucOptionBytes,
										  ( IPv6_Address_t * ) xICMPHeader_IPv6->xIPv6_Address.ucBytes,
										  pxEndPoint );
				}
				break;
		}	/* switch( xICMPHeader_IPv6->ucTypeOfMessage ) */
	}	/* if( pxEndPoint != NULL ) */

	return eReleaseBuffer;
//	return eFrameConsumed;
//	return eReturnEthernetFrame;
}
/*-----------------------------------------------------------*/

void FreeRTOS_OutputAdvertiseIPv6( NetworkEndPoint_t *pxEndPoint )
{
NetworkBufferDescriptor_t *pxNetworkBuffer;
ICMPPacket_IPv6_t *pxICMPPacket;
NetworkInterface_t *pxInterface;
ICMPHeader_IPv6_t *pxICMPHeader_IPv6;
size_t uxICMPSize;
size_t xPacketSize;

	xPacketSize = ( size_t ) ( ipSIZE_OF_ETH_HEADER + ipSIZE_OF_IPv6_HEADER + sizeof( ICMPHeader_IPv6_t ) );

	/* This is called from the context of the IP event task, so a block time
	must not be used. */
	pxNetworkBuffer = pxGetNetworkBufferWithDescriptor( xPacketSize, ndDONT_BLOCK );
	if( pxNetworkBuffer != NULL )
	{
		pxNetworkBuffer->ulIPAddress = 0;
		pxNetworkBuffer->pxEndPoint = pxEndPoint;

		pxInterface = pxEndPoint->pxNetworkInterface;

		configASSERT( pxInterface != NULL );

		pxICMPPacket = ( ICMPPacket_IPv6_t * )pxNetworkBuffer->pucEthernetBuffer;
		pxICMPHeader_IPv6 = ( ICMPHeader_IPv6_t * )&( pxICMPPacket->xICMPHeader_IPv6 );

		memcpy( pxICMPPacket->xEthernetHeader.xDestinationAddress.ucBytes, pcLOCAL_NETWORK_MULTICAST_MAC, ipMAC_ADDRESS_LENGTH_BYTES );
		memcpy( pxICMPPacket->xEthernetHeader.xSourceAddress.ucBytes, pxEndPoint->xMACAddress.ucBytes, ipMAC_ADDRESS_LENGTH_BYTES );
		pxICMPPacket->xEthernetHeader.usFrameType = ipIPv6_FRAME_TYPE;              /* 12 + 2 = 14 */

		pxICMPPacket->xIPHeader.ucVersionTrafficClass = 0x60;
		pxICMPPacket->xIPHeader.ucTrafficClassFlow = 0;
		pxICMPPacket->xIPHeader.usFlowLabel = 0;
		pxICMPPacket->xIPHeader.usPayloadLength = FreeRTOS_htons( 32 );
		pxICMPPacket->xIPHeader.ucNextHeader = ipPROTOCOL_ICMP_IPv6;
		pxICMPPacket->xIPHeader.ucHopLimit = 255;
		memcpy( pxICMPPacket->xIPHeader.xSourceIPv6Address.ucBytes, pxEndPoint->ulIPAddress_IPv6.ucBytes, 16 );
		memcpy( pxICMPPacket->xIPHeader.xDestinationIPv6Address.ucBytes, pcLOCAL_NETWORK_MULTICAST_IP, 16 );

		uxICMPSize = sizeof( ICMPHeader_IPv6_t );
		pxICMPHeader_IPv6->ucTypeOfMessage = ipICMP_NEIGHBOR_ADVERTISEMENT_IPv6;
		pxICMPHeader_IPv6->ucTypeOfService = 0;
		pxICMPHeader_IPv6->ulReserved = FreeRTOS_htonl( ndICMPv6_FLAG_SOLICITED | ndICMPv6_FLAG_UPDATE );

		/* Type of option. */
		pxICMPHeader_IPv6->ucOptionType = ndICMP_TARGET_LINK_LAYER_ADDRESS;
		/* Length of option in units of 8 bytes. */
		pxICMPHeader_IPv6->ucOptionLength = 1;
		memcpy( pxICMPHeader_IPv6->ucOptionBytes, pxEndPoint->xMACAddress.ucBytes, sizeof( MACAddress_t ) );
		pxICMPPacket->xIPHeader.ucHopLimit = 255;
		memcpy( pxICMPHeader_IPv6->xIPv6_Address.ucBytes, pxEndPoint->ulIPAddress_IPv6.ucBytes, sizeof( pxICMPHeader_IPv6->xIPv6_Address.ucBytes ) );

		/* Important: tell NIC driver how many bytes must be sent */
		pxNetworkBuffer->xDataLength = ( size_t ) ( ipSIZE_OF_ETH_HEADER + ipSIZE_OF_IPv6_HEADER + uxICMPSize );

		pxICMPHeader_IPv6->usChecksum = 0;
		/* calculate the UDP checksum for outgoing package */
		usGenerateProtocolChecksum( ( uint8_t* ) pxNetworkBuffer->pucEthernetBuffer, pxNetworkBuffer->xDataLength, pdTRUE );

		/* Set the parameter 'bReleaseAfterSend'. */
		pxInterface->pfOutput( pxInterface, pxNetworkBuffer, pdTRUE );
	}
}
/*-----------------------------------------------------------*/


BaseType_t FreeRTOS_CreateIPv6Address( IPv6_Address_t *pxIPAddress, const IPv6_Address_t *pxPrefix, BaseType_t xPrefixLength )
{
uint32_t ulRandom[ 4 ];
BaseType_t xIndex, xResult = pdPASS;

	for( xIndex = 0; xIndex < ARRAY_SIZE( ulRandom ); xIndex++ )
	{
		if( xRandom32( ulRandom + xIndex ) == pdFAIL )
		{
			xResult = pdFAIL;
			break;
		}
	}
	if( xResult == pdPASS )
	{
		configASSERT( ( xPrefixLength > 0 ) && ( xPrefixLength < ipSIZE_OF_IPv6_ADDRESS ) );
		memcpy( pxIPAddress->ucBytes, pxPrefix->ucBytes, xPrefixLength );
		memcpy( &( pxIPAddress->ucBytes[ xPrefixLength ] ), ulRandom, ipSIZE_OF_IPv6_ADDRESS - xPrefixLength );
	}
	return xResult;
}

/*-----------------------------------------------------------*/

#endif /* ipconfigUSE_IPv6 */

