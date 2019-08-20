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

/* Possible values of ucFlags in a neighbour advertisement. */
#define ndFlags_IsRouter		0x8000u
#define ndFlags_Solicited		0x4000u
#define ndFlags_Override		0x2000u

/* A block time of 0 simply means "don't block". */
#define ndDONT_BLOCK				( ( TickType_t ) 0 )

#define ADVERTISE_PACKET_SIZE	64

/* The character used to fill ICMP echo requests, and therefore also the
character expected to fill ICMP echo replies. */
#define ndECHO_DATA_FILL_BYTE						'x'

#include "pack_struct_start.h"
struct xICMPPrefixOption_IPv6
{
	uint8_t ucType;					/*  0 +  1 =  1 */
	uint8_t ucLength;				/*  1 +  1 =  2 */
	uint8_t ucPrefixLength;			/*  2 +  1 =  3 */
	uint8_t ucFlags;				/*  3 +  1 =  4 */
	uint32_t ulValidLifeTime;		/*  4 +  4 =  8 */
	uint32_t ulPreferredLifeTime;	/*  8 +  4 = 12 */
	uint32_t ulReserved;			/* 12 +  4 = 16 */
	uint8_t ucPrefix[16];			/* 16 + 16 = 32 */
}
#include "pack_struct_end.h"
typedef struct xICMPPrefixOption_IPv6 ICMPPrefixOption_IPv6_t;

#include "pack_struct_start.h"
struct xRASolicitationRequest_IPv6 {
	uint8_t ucTypeOfMessage;       /*  0 +  1 =  1 */
	uint8_t ucTypeOfService;       /*  1 +  1 =  2 */
	uint16_t usChecksum;           /*  2 +  2 =  4 */
	uint32_t ulReserved;           /*  4 +  4 =  8 */
//	uint8_t ucOptionType;          /*  8 +  1 =  9 */
//	uint8_t ucOptionLength;        /*  9 +  1 = 10 */
//	uint8_t ucOptionBytes[6];      /* 10 +  6 = 16 */
}
#include "pack_struct_end.h"
typedef struct xRASolicitationRequest_IPv6 RASolicitationRequest_IPv6_t;

// All nodes on the local network segment
static const uint8_t pcLOCAL_NETWORK_MULTICAST_IP[ 16 ] = { 0xff, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01 };
static const uint8_t pcLOCAL_NETWORK_MULTICAST_MAC[ ipMAC_ADDRESS_LENGTH_BYTES ] = { 0x33, 0x33, 0x00, 0x00, 0x00, 0x01 };

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
NetworkEndPoint_t *pxEndPoint;
/* For testing now, fill in Hein's laptop: 9c 5c 8e 38 06 6c */
//static const unsigned char testMAC[] = { 0x9C, 0x5C, 0x8E, 0x38, 0x06, 0x6C };
//
//	memcpy( pxMACAddress->ucBytes, testMAC, sizeof testMAC );

	eReturn = prvCacheLookup( pxIPAddress, pxMACAddress, ppxEndPoint );
	if( eReturn == eARPCacheMiss )
	{
		pxEndPoint = FreeRTOS_FindEndPointOnIP_IPv6( pxIPAddress );
		if( pxEndPoint == NULL )
		{
			pxEndPoint = FreeRTOS_FindGateWay( ipTYPE_IPv6 );
			if( pxEndPoint != NULL )
			{
			NetworkEndPoint_t *pxKeep;
				pxKeep = pxEndPoint;
				memcpy( pxIPAddress->ucBytes, pxEndPoint->ipv6.xGatewayAddress.ucBytes, ipSIZE_OF_IPv6_ADDRESS );
				FreeRTOS_printf( ( "eNDGetCacheEntry: Using gw %pip", pxIPAddress->ucBytes ) );	/* access to 'ipv6' is checked. */
				eReturn = prvCacheLookup( pxIPAddress, pxMACAddress, ppxEndPoint );
				if( *( ppxEndPoint ) == NULL )
				{
					*( ppxEndPoint ) = pxKeep;
				}
			}
		}
	}

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
		else if( memcmp( xNDCache[ x ].xIPAddress.ucBytes, pxIPAddress->ucBytes, ipSIZE_OF_IPv6_ADDRESS ) == 0 )
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
		memcpy( xNDCache[ xEntryFound ].xIPAddress.ucBytes, pxIPAddress->ucBytes, ipSIZE_OF_IPv6_ADDRESS );
		/* Copy the MAC-address. */
		memcpy( xNDCache[ xEntryFound ].xMACAddress.ucBytes, pxMACAddress->ucBytes, sizeof( MACAddress_t ) );
		xNDCache[ xEntryFound ].pxEndPoint = pxEndPoint;
		xNDCache[ xEntryFound ].ucAge = ( uint8_t ) ipconfigMAX_ARP_AGE;
		xNDCache[ xEntryFound ].ucValid = ( uint8_t ) pdTRUE;
		/* The format %pip will print a IPv6 address ( if printf-stdarg.c is included ). */
//		FreeRTOS_printf( ( "vNDRefreshCacheEntry[ %d ] %pip with %02x:%02x:%02x\n",
//			( int ) xEntryFound,
//			pxIPAddress->ucBytes,
//			pxMACAddress->ucBytes[ 3 ],
//			pxMACAddress->ucBytes[ 4 ],
//			pxMACAddress->ucBytes[ 5 ] ) );
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
		else if( memcmp( xNDCache[ x ].xIPAddress.ucBytes, pxAddressToLookup->ucBytes, ipSIZE_OF_IPv6_ADDRESS ) == 0 )
		{
			memcpy( pxMACAddress->ucBytes, xNDCache[ x ].xMACAddress.ucBytes, sizeof( MACAddress_t ) );
			eReturn = eARPCacheHit;
			*ppxEndPoint = xNDCache[ x ].pxEndPoint;
			FreeRTOS_printf( ( "prvCacheLookup6[ %d ] %pip with %02x:%02x:%02x:%02x:%02x:%02x\n",
				( int ) x,
				pxAddressToLookup->ucBytes,
				pxMACAddress->ucBytes[ 0 ],
				pxMACAddress->ucBytes[ 1 ],
				pxMACAddress->ucBytes[ 2 ],
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

	configASSERT( pxEndPoint != NULL );
	configASSERT( pxEndPoint->bits.bIPv6 != pdFALSE_UNSIGNED );
	
	memcpy( pxICMPPacket->xIPHeader.xDestinationIPv6Address.ucBytes, pxICMPPacket->xIPHeader.xSourceIPv6Address.ucBytes, ipSIZE_OF_IPv6_ADDRESS );
	memcpy( pxICMPPacket->xIPHeader.xSourceIPv6Address.ucBytes, pxEndPoint->ipv6.xIPAddress.ucBytes, ipSIZE_OF_IPv6_ADDRESS );
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
//BaseType_t xIndex;
//const char *ip_address[] = {
//	"fe80::9355:69c7:585a:afe7",	/* raspberry ff02::1:ff5a:afe7, 33:33:ff:5a:af:e7 */
//	"fe80::6816:5e9b:80a0:9edb",	/* laptop Hein */
//};
//	for( xIndex = 0; xIndex < ARRAY_SIZE( ip_address ); xIndex++ )
//	{
//		FreeRTOS_inet_pton6( ip_address[ xIndex ], xIPAddress.ucBytes );
//		FreeRTOS_printf( ( "nd_test: Looking up %pip\n", xIPAddress.ucBytes ) );
//		pxEndPoint = FreeRTOS_FindEndPointOnIP_IPv6( &xIPAddress );
//		if( pxEndPoint != NULL )
//		{
//			uxNeededSize = ( size_t ) ( ipSIZE_OF_ETH_HEADER + ipSIZE_OF_IPv6_HEADER + sizeof( ICMPHeader_IPv6_t ) );
//			pxNetworkBuffer = pxGetNetworkBufferWithDescriptor( uxNeededSize, ndDONT_BLOCK );
//			if( pxNetworkBuffer != NULL )
//			{
//				pxNetworkBuffer->pxEndPoint = pxEndPoint;
//				vNDGenerateRequestPacket( pxNetworkBuffer, &xIPAddress );
//			}
//		}
//		else
//		{
//			FreeRTOS_printf( ( "nd_test: can not find and endpoint for %pip\n", xIPAddress.ucBytes ) );
//		}
//	}
	{
		// ff:02::2
		memset( xIPAddress.ucBytes, '\0', sizeof xIPAddress.ucBytes );
		xIPAddress.ucBytes[  0 ] = 0xff;
		xIPAddress.ucBytes[  1 ] = 0x02;
		xIPAddress.ucBytes[ 15 ] = 0x02;
		pxEndPoint = FreeRTOS_FindEndPointOnIP_IPv6( &xIPAddress );
		if( pxEndPoint != NULL )
		{
			uxNeededSize = ( size_t ) ( ipSIZE_OF_ETH_HEADER + ipSIZE_OF_IPv6_HEADER + sizeof( RASolicitationRequest_IPv6_t ) );
			pxNetworkBuffer = pxGetNetworkBufferWithDescriptor( uxNeededSize, ndDONT_BLOCK );
			if( pxNetworkBuffer != NULL )
			{
				pxNetworkBuffer->pxEndPoint = pxEndPoint;
				vNDGenerateRASolicitationPacket( pxNetworkBuffer, &( xIPAddress ) );
			}
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

	configASSERT( pxEndPoint != NULL );
	configASSERT( pxEndPoint->bits.bIPv6 != pdFALSE_UNSIGNED );

	memcpy( pxICMPPacket->xIPHeader.xSourceIPv6Address.ucBytes, pxEndPoint->ipv6.xIPAddress.ucBytes, 16 );

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


//struct xRASolicitationRequest_IPv6 {
//	uint8_t ucType;	// 133
//	uint8_t ucCode;	// 0
//	uint16_t usChecksum;
//	uint32_t usReserved;
//
//	uint8_t ucOptionType;
//	uint8_t ucOptionLength;
//	uint8_t ucOptionBytes[6];	/* a MAC address */
//};

void vNDGenerateRASolicitationPacket( NetworkBufferDescriptor_t * const pxNetworkBuffer, IPv6_Address_t *pxIPAddress )
{
ICMPPacket_IPv6_t *pxICMPPacket;
RASolicitationRequest_IPv6_t *xRASolicitationRequest;
NetworkEndPoint_t *pxEndPoint = pxNetworkBuffer->pxEndPoint;
size_t uxNeededSize;
MACAddress_t xMultiCastMacAddress;
NetworkBufferDescriptor_t *pxDescriptor = pxNetworkBuffer;

	configASSERT( pxEndPoint != NULL );
	uxNeededSize = ( size_t ) ( ipSIZE_OF_ETH_HEADER + ipSIZE_OF_IPv6_HEADER + sizeof( RASolicitationRequest_IPv6_t ) );
	if( pxDescriptor->xDataLength < uxNeededSize )
	{
		pxDescriptor = pxDuplicateNetworkBufferWithDescriptor( pxDescriptor, uxNeededSize );
		if( pxDescriptor == NULL )
		{
			return;
		}
	}
	pxICMPPacket = ( ICMPPacket_IPv6_t * )pxDescriptor->pucEthernetBuffer;
	xRASolicitationRequest = ( RASolicitationRequest_IPv6_t * )&( pxICMPPacket->xICMPHeader_IPv6 );

	pxDescriptor->xDataLength = uxNeededSize;

	xMultiCastMacAddress.ucBytes[ 0 ] = 0x33;
	xMultiCastMacAddress.ucBytes[ 1 ] = 0x33;
	xMultiCastMacAddress.ucBytes[ 2 ] = 0x00;
	xMultiCastMacAddress.ucBytes[ 3 ] = 0x00;
	xMultiCastMacAddress.ucBytes[ 4 ] = 0x00;
	xMultiCastMacAddress.ucBytes[ 5 ] = 0x02;

	/* Set Ethernet header. Will be swapped. */
	memcpy( pxICMPPacket->xEthernetHeader.xSourceAddress.ucBytes, xMultiCastMacAddress.ucBytes, ipMAC_ADDRESS_LENGTH_BYTES );
	memcpy( pxICMPPacket->xEthernetHeader.xDestinationAddress.ucBytes, pxEndPoint->xMACAddress.ucBytes, ipMAC_ADDRESS_LENGTH_BYTES );
	pxICMPPacket->xEthernetHeader.usFrameType = ipIPv6_FRAME_TYPE;

	/* Set IP-header. */
	pxICMPPacket->xIPHeader.ucVersionTrafficClass = 0x60;
	pxICMPPacket->xIPHeader.ucTrafficClassFlow = 0;
	pxICMPPacket->xIPHeader.usFlowLabel = 0;
	pxICMPPacket->xIPHeader.usPayloadLength = FreeRTOS_htons( sizeof( RASolicitationRequest_IPv6_t ) );
	pxICMPPacket->xIPHeader.ucNextHeader = ipPROTOCOL_ICMP_IPv6;
	pxICMPPacket->xIPHeader.ucHopLimit = 255;

	configASSERT( pxEndPoint != NULL );
	configASSERT( pxEndPoint->bits.bIPv6 != pdFALSE_UNSIGNED );
	memcpy( pxICMPPacket->xIPHeader.xSourceIPv6Address.ucBytes, pxEndPoint->ipv6.xIPAddress.ucBytes, 16 );

	//ff02::1:ff5a:afe7
	// ff02::2
	memcpy( pxICMPPacket->xIPHeader.xDestinationIPv6Address.ucBytes, pxIPAddress->ucBytes, 16 );
/*
	uint8_t ucTypeOfMessage;       //  0 +  1 =  1
	uint8_t ucTypeOfService;       //  1 +  1 =  2
	uint16_t usChecksum;           //  2 +  2 =  4
	uint32_t ulReserved;           //  4 +  4 =  8
	uint8_t ucOptionType;          //  8 +  1 =  9
	uint8_t ucOptionLength;        //  9 +  1 = 10
	uint8_t ucOptionBytes[6];      // 10 +  6 = 16
*/

	/* Set ICMP header. */
	memset( xRASolicitationRequest, '\0', sizeof( *xRASolicitationRequest ) );
	xRASolicitationRequest->ucTypeOfMessage = ipICMP_ROUTER_SOLICITATION_IPv6;

/*
	xRASolicitationRequest->ucOptionType = ndICMP_SOURCE_LINK_LAYER_ADDRESS;
	xRASolicitationRequest->ucOptionLength = 1;
	memcpy( xRASolicitationRequest->ucOptionBytes, pxEndPoint->xMACAddress.ucBytes, ipMAC_ADDRESS_LENGTH_BYTES );
*/
	/* Checmsums. */
	xRASolicitationRequest->usChecksum = 0;
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
			pxEndPoint = FreeRTOS_FindGateWay( ipTYPE_IPv6 );
			if( pxEndPoint == NULL )
			{
				FreeRTOS_printf( ( "SendPingRequestIPv6: No routing/gw found" ) );
				return 0;
			}
			FreeRTOS_printf( ( "SendPingRequestIPv6: Using gw %pip", pxEndPoint->ipv6.xGatewayAddress.ucBytes ) );	/* access to 'ipv6' is checked. */
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

				configASSERT( pxEndPoint != NULL );
				configASSERT( pxEndPoint->bits.bIPv6 != pdFALSE_UNSIGNED );

				pxICMPPacket->xIPHeader.usPayloadLength = FreeRTOS_htons( sizeof( ICMPEcho_IPv6_t ) + xNumberOfBytesToSend );
				memcpy( pxICMPPacket->xIPHeader.xDestinationIPv6Address.ucBytes, pxIPAddress->ucBytes, ipSIZE_OF_IPv6_ADDRESS );
				memcpy( pxICMPPacket->xIPHeader.xSourceIPv6Address.ucBytes, pxEndPoint->ipv6.xIPAddress.ucBytes, ipSIZE_OF_IPv6_ADDRESS );
				pxICMPPacket->xIPHeader.ucVersionTrafficClass = 0x60;
				pxICMPPacket->xIPHeader.ucNextHeader = ipPROTOCOL_ICMP_IPv6;
				pxICMPPacket->xIPHeader.ucHopLimit = 128;

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

static const char *pcMessageType( BaseType_t xType )
{
	const char *pcReturn = "unknown";
	switch( xType )
	{
		case ipICMP_DEST_UNREACHABLE_IPv6:        pcReturn = "DEST_UNREACHABLE"; break;
		case ipICMP_PACKET_TOO_BIG_IPv6:          pcReturn = "PACKET_TOO_BIG"; break;
		case ipICMP_TIME_EXEEDED_IPv6:            pcReturn = "TIME_EXEEDED"; break;
		case ipICMP_PARAMETER_PROBLEM_IPv6:       pcReturn = "PARAMETER_PROBLEM"; break;
		case ipICMP_PING_REQUEST_IPv6:            pcReturn = "PING_REQUEST"; break;
		case ipICMP_PING_REPLY_IPv6:              pcReturn = "PING_REPLY"; break;
		case ipICMP_ROUTER_SOLICITATION_IPv6:     pcReturn = "ROUTER_SOL"; break;
		case ipICMP_ROUTER_ADVERTISEMENT_IPv6:    pcReturn = "ROUTER_ADV"; break;
		case ipICMP_NEIGHBOR_SOLICITATION_IPv6:   pcReturn = "NEIGHBOR_SOL"; break;
		case ipICMP_NEIGHBOR_ADVERTISEMENT_IPv6:  pcReturn = "NEIGHBOR_ADV"; break;
	}
	return pcReturn;
}

eFrameProcessingResult_t prvProcessICMPMessage_IPv6( NetworkBufferDescriptor_t * const pxNetworkBuffer )
{
ICMPPacket_IPv6_t *pxICMPPacket = ( ICMPPacket_IPv6_t * )pxNetworkBuffer->pucEthernetBuffer;
ICMPHeader_IPv6_t *xICMPHeader_IPv6 = ( ICMPHeader_IPv6_t * )&( pxICMPPacket->xICMPHeader_IPv6 );
NetworkEndPoint_t *pxEndPoint = pxNetworkBuffer->pxEndPoint;
size_t uxNeededSize;

	//if( xICMPHeader_IPv6->ucTypeOfMessage != ipICMP_PING_REQUEST_IPv6 )
	{
		FreeRTOS_printf( ( "ICMPv6 %d (%s) from %pip to %pip\n",
			xICMPHeader_IPv6->ucTypeOfMessage,
			pcMessageType( ( BaseType_t ) xICMPHeader_IPv6->ucTypeOfMessage ),
			pxICMPPacket->xIPHeader.xSourceIPv6Address.ucBytes,
			pxICMPPacket->xIPHeader.xDestinationIPv6Address.ucBytes ) );
	}
	if( ( pxEndPoint != NULL ) && ( pxEndPoint->bits.bIPv6 != pdFALSE_UNSIGNED ) )
	{
		switch( xICMPHeader_IPv6->ucTypeOfMessage )
		{
			default:
				{
					FreeRTOS_printf( ("prvProcessICMPMessage_IPv6: unknown type %u\n", ( unsigned ) xICMPHeader_IPv6->ucTypeOfMessage ) );
				}
				break;
			case ipICMP_DEST_UNREACHABLE_IPv6:
				{
					FreeRTOS_printf( ("prvProcessICMPMessage_IPv6: DEST_UNREACHABLE\n" ) );
				}
				break;
			case ipICMP_PACKET_TOO_BIG_IPv6:
				{
					FreeRTOS_printf( ("prvProcessICMPMessage_IPv6: PACKET_TOO_BIG\n" ) );
				}
				break;
			case ipICMP_TIME_EXEEDED_IPv6:
				{
					FreeRTOS_printf( ("prvProcessICMPMessage_IPv6: TIME_EXEEDED\n" ) );
				}
				break;
			case ipICMP_PARAMETER_PROBLEM_IPv6:
				{
					FreeRTOS_printf( ("prvProcessICMPMessage_IPv6: PARAMETER_PROBLEM\n" ) );
				}
				break;
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
					memcpy( xICMPHeader_IPv6->xIPv6_Address.ucBytes, pxEndPoint->ipv6.xIPAddress.ucBytes, sizeof( xICMPHeader_IPv6->xIPv6_Address.ucBytes ) );

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
			case ipICMP_ROUTER_SOLICITATION_IPv6:
				{
				}
				break;
			case ipICMP_ROUTER_ADVERTISEMENT_IPv6:
				{
				ICMPRouterAdvertisement_IPv6_t *pxAdvertisement = ipPOINTER_CAST( ICMPRouterAdvertisement_IPv6_t *, &( pxICMPPacket->xICMPHeader_IPv6 ) );
				size_t uxIndex;
				size_t uxLast;
				size_t uxICMPSize;
				uint8_t *pucBytes;
	/*
		uint8_t ucTypeOfMessage;       //  0 +  1 =  1
		uint8_t ucTypeOfService;       //  1 +  1 =  2
		uint16_t usChecksum;           //  2 +  2 =  4
		uint8_t ucHopLimit;            //  4 +  1 =  5
		uint8_t ucFlags;               //  5 +  1 =  6
		uint16_t usLifetime;           //  6 +  2 =  8
		uint16_t usReachableTime[2];   //  8 +  4 = 12
		uint16_t usRetransTime[2];     // 12 +  4 = 16
		ICMPRouterAdvertisement_IPv6_t
	*/
					uxICMPSize = sizeof( ICMPRouterAdvertisement_IPv6_t );
					uxNeededSize = ( size_t ) ( ipSIZE_OF_ETH_HEADER + ipSIZE_OF_IPv6_HEADER + uxICMPSize );
					if( uxNeededSize > pxNetworkBuffer->xDataLength )
					{
						FreeRTOS_printf( ("Too small\n" ) );
						break;
					}
//	14 + 40 + 16 = 70
//	Total length 118
//	118 - 70 = 48
					FreeRTOS_printf( ( "RA: Type %02x Srv %02x Checksum %04x Hops %d Flags %02x Life %d\n",
						pxAdvertisement->ucTypeOfMessage,
						pxAdvertisement->ucTypeOfService,
						FreeRTOS_ntohs( pxAdvertisement->usChecksum ),
						pxAdvertisement->ucHopLimit,
						pxAdvertisement->ucFlags,
						FreeRTOS_ntohs( pxAdvertisement->usLifetime ) ) );
					uxIndex = 0;
					/* uxLast points to the first byte after the buffer. */
					uxLast = pxNetworkBuffer->xDataLength - uxNeededSize;
					pucBytes = pxNetworkBuffer->pucEthernetBuffer + uxNeededSize;
					while( ( uxIndex + 1 ) < uxLast )
					{
						uint8_t ucType = pucBytes[ uxIndex ];
						size_t uxLength = ( size_t ) pucBytes[ uxIndex + 1 ] * 8uL;
						if( uxIndex + uxLength > uxLast )
						{
							FreeRTOS_printf( ( "RA: Not enough bytes ( %u > %u )\n", ( unsigned ) uxIndex + uxLength, ( unsigned ) uxLast ) );
							break;
						}
						switch( ucType )
						{ 
							case ndICMP_SOURCE_LINK_LAYER_ADDRESS:	// 1
								{
									FreeRTOS_printf( ( "RA: Source = %02x-%02x-%02x-%02x-%02x-%02x\n",
										pucBytes[ uxIndex + 2 ],
										pucBytes[ uxIndex + 3 ],
										pucBytes[ uxIndex + 4 ],
										pucBytes[ uxIndex + 5 ],
										pucBytes[ uxIndex + 6 ],
										pucBytes[ uxIndex + 7 ] ) );
								}
								break;
							case ndICMP_TARGET_LINK_LAYER_ADDRESS:	// 2
								{
								}
								break;
							case ndICMP_PREFIX_INFORMATION:			// 3
								{
								ICMPPrefixOption_IPv6_t *pxPrefixOption = ipPOINTER_CAST( ICMPPrefixOption_IPv6_t *, &( pucBytes[ uxIndex ] ) );

									FreeRTOS_printf( ( "RA: Prefix len %d Life %lu, %lu (%pip)\n",
										pxPrefixOption->ucPrefixLength,
										FreeRTOS_ntohl( pxPrefixOption->ulValidLifeTime ),
										FreeRTOS_ntohl( pxPrefixOption->ulPreferredLifeTime ),
										pxPrefixOption->ucPrefix ) );
 								}
								break;
							case ndICMP_REDIRECTED_HEADER:			// 4
								{
								}
								break;
							case ndICMP_MTU_OPTION:					// 5
								{
								uint32_t ulMTU;

									/* ulChar2u32 returns host-endian numbers. */
									ulMTU = ulChar2u32 ( &( pucBytes[ uxIndex + 4 ] ) );
									FreeRTOS_printf( ( "RA: MTU = %lu\n",  ulMTU ) );
								}
								break;
							default:
								{
									FreeRTOS_printf( ( "RA: Type %02x not implemented\n", ucType ) );
								}
								break;
						}
						uxIndex = uxIndex + uxLength;
					}
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
		pxICMPPacket->xIPHeader.usPayloadLength = FreeRTOS_htons( sizeof( ICMPHeader_IPv6_t ) );
		pxICMPPacket->xIPHeader.ucNextHeader = ipPROTOCOL_ICMP_IPv6;
		pxICMPPacket->xIPHeader.ucHopLimit = 255;
		memcpy( pxICMPPacket->xIPHeader.xSourceIPv6Address.ucBytes, pxEndPoint->ipv6.xIPAddress.ucBytes, 16 );
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
		memcpy( pxICMPHeader_IPv6->xIPv6_Address.ucBytes, pxEndPoint->ipv6.xIPAddress.ucBytes, sizeof( pxICMPHeader_IPv6->xIPv6_Address.ucBytes ) );

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


BaseType_t FreeRTOS_CreateIPv6Address( IPv6_Address_t *pxIPAddress, const IPv6_Address_t *pxPrefix, size_t uxPrefixLength, BaseType_t xDoRandom )
{
uint32_t ulRandom[ 4 ];
uint8_t *pucSource;
BaseType_t xIndex, xResult = pdPASS;

	if( xDoRandom != pdFALSE )
	{
		for( xIndex = 0; xIndex < ARRAY_SIZE( ulRandom ); xIndex++ )
		{
			if( xRandom32( ulRandom + xIndex ) == pdFAIL )
			{
				xResult = pdFAIL;
				break;
			}
		}
	}
	else
	{
		memset( ulRandom, '\0', sizeof ulRandom );
	}
	if( xResult == pdPASS )
	{
		configASSERT( ( uxPrefixLength > 0 ) && ( uxPrefixLength < ( 8u * ipSIZE_OF_IPv6_ADDRESS ) ) );
		memcpy( pxIPAddress->ucBytes, pxPrefix->ucBytes, ( uxPrefixLength + 7 ) / 8 );
		pucSource = ( uint8_t * ) &ulRandom;
		size_t uxIndex = uxPrefixLength / 8u;
		if( ( uxPrefixLength % 8u ) != 0 )
		{
		size_t uxHostLen = 8u - ( uxPrefixLength % 8u );
		uint8_t uxHostMask = ( ( ( size_t ) 1u ) << uxHostLen ) - 1u;
		uint8_t uxNetMask = ~( uxHostMask );

			pxIPAddress->ucBytes[ uxIndex ] &= uxNetMask;
			pxIPAddress->ucBytes[ uxIndex ] |= pucSource[ 0 ] & uxHostMask;
			pucSource = &( pucSource[ 1 ] );
			uxIndex++;
		}
		if( uxIndex < ipSIZE_OF_IPv6_ADDRESS )
		{
			memcpy( &( pxIPAddress->ucBytes[ uxIndex ] ), pucSource, ipSIZE_OF_IPv6_ADDRESS - uxIndex );
		}
	}
	return xResult;
}
/*-----------------------------------------------------------*/

#endif /* ipconfigUSE_IPv6 */

