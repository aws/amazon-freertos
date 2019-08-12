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
#include "queue.h"
#include "semphr.h"

/* FreeRTOS+TCP includes. */
#include "FreeRTOS_IP.h"
#include "FreeRTOS_Sockets.h"
#include "FreeRTOS_IP_Private.h"
#include "FreeRTOS_ARP.h"
#include "FreeRTOS_UDP_IP.h"
#include "FreeRTOS_DHCP.h"
#if( ipconfigUSE_LLMNR == 1 )
	#include "FreeRTOS_DNS.h"
#endif /* ipconfigUSE_LLMNR */
#include "NetworkInterface.h"
#include "NetworkBufferManagement.h"
#include "FreeRTOS_Routing.h"

#define ICMPv6_FLAG_SOLICITED				0x40000000ul
#define ICMPv6_FLAG_UPDATE					0x20000000ul

#define	ipICMP_SOURCE_LINK_LAYER_ADDRESS	1
#define	ipICMP_TARGET_LINK_LAYER_ADDRESS	2

/* A list of all network end-points: */
NetworkEndPoint_t *pxNetworkEndPoints = NULL;

/* A list of all network interfaces: */
NetworkInterface_t *pxNetworkInterfaces = NULL;

RoutingStats_t xRoutingStats;

/*-----------------------------------------------------------*/

NetworkInterface_t *FreeRTOS_AddNetworkInterface( NetworkInterface_t *pxInterface )
{
NetworkInterface_t *pxIterator = NULL;

	/* This interface will be added to the end of the list of interfaces, so
	there is no pxNext yet. */
	pxInterface->pxNext = NULL;

	/* The end point for this interface has not yet been set. */
	/*_RB_ As per other comments, why not set the end point at the same time? */
	pxInterface->pxEndPoint = NULL;

	if( pxNetworkInterfaces == NULL )
	{
		/* No other interfaces are set yet, so this is the first in the list. */
		pxNetworkInterfaces = pxInterface;
	}
	else
	{
		/* Other interfaces are already defined, so iterate to the end of the
		list. */
		/*_RB_ Question - if ipconfigMULTI_INTERFACE is used to define the
		maximum number of interfaces, would it be more efficient to have an
		array of interfaces rather than a linked list of interfaces? */
		pxIterator = pxNetworkInterfaces;
		while( pxIterator->pxNext != NULL )
		{
			pxIterator = pxIterator->pxNext;
		}

		pxIterator->pxNext = pxInterface;
	}

	return pxInterface;
}
/*-----------------------------------------------------------*/

/* Get the first Network Interface. */
NetworkInterface_t *FreeRTOS_FirstNetworkInterface( void )
{
	return pxNetworkInterfaces;
}
/*-----------------------------------------------------------*/

/* Get the next Network Interface. */
NetworkInterface_t *FreeRTOS_NextNetworkInterface( NetworkInterface_t *pxInterface )
{
	if( pxInterface != NULL )
	{
		pxInterface = pxInterface->pxNext;
	}
	return pxInterface;
}
/*-----------------------------------------------------------*/

NetworkEndPoint_t *FreeRTOS_AddEndPoint( NetworkInterface_t *pxInterface, NetworkEndPoint_t *pxEndPoint )
{
NetworkEndPoint_t *pxIterator = NULL;

	/* This end point will go to the end of the list, so there is no pxNext
	yet. */
	pxEndPoint->pxNext = NULL;

	/* Double link between the NetworkInterface_t that is using the addressing
	defined by this NetworkEndPoint_t structure. */
	pxEndPoint->pxNetworkInterface = pxInterface;
	if( pxInterface->pxEndPoint == NULL )
	{
		/*_RB_ When would pxInterface->pxEndPoint ever not be NULL unless this is called twice? */
		/*_HT_ It may be called twice. */
		pxInterface->pxEndPoint = pxEndPoint;
	}

	if( pxNetworkEndPoints == NULL )
	{
		/* No other end points are defined yet - so this is the first in the
		list. */
		pxNetworkEndPoints = pxEndPoint;
	}
	else
	{
		/* Other end points are already defined so iterate to the end of the
		list. */
		pxIterator = pxNetworkEndPoints;
		while( pxIterator->pxNext != NULL )
		{
			pxIterator = pxIterator->pxNext;
		}

		pxIterator->pxNext = pxEndPoint;
	}

	FreeRTOS_printf( ( "FreeRTOS_AddEndPoint: MAC: %02x-%02x IPv4: %lxip\n",
		pxEndPoint->xMACAddress.ucBytes[ 4 ],
		pxEndPoint->xMACAddress.ucBytes[ 5 ],
		FreeRTOS_ntohl( pxEndPoint->ulDefaultIPAddress ) ) );

	return pxEndPoint;
}
/*-----------------------------------------------------------*/

NetworkEndPoint_t *FreeRTOS_FirstEndPoint( NetworkInterface_t *pxInterface )
{
NetworkEndPoint_t *pxEndPoint = pxNetworkEndPoints;

	/* Find and return the NetworkEndPoint_t structure that is associated with
	the pxInterface NetworkInterface_t. *//*_RB_ Could this be made a two way link, so the NetworkEndPoint_t can just be read from the NetworkInterface_t structure?  Looks like there is a pointer in the struct already. */
	if( pxInterface != NULL )
	{
		while( pxEndPoint != NULL )
		{
			if( pxEndPoint->pxNetworkInterface == pxInterface )
			{
				break;
			}
			pxEndPoint = pxEndPoint->pxNext;
		}
	}

	return pxEndPoint;
}
/*-----------------------------------------------------------*/

NetworkEndPoint_t *FreeRTOS_NextEndPoint( NetworkInterface_t *pxInterface, NetworkEndPoint_t *pxEndPoint )
{
	if( pxEndPoint != NULL )
	{
		pxEndPoint = pxEndPoint->pxNext;
		if( pxInterface != NULL )
		{
			while( pxEndPoint != NULL )
			{
				if( pxEndPoint->pxNetworkInterface == pxInterface )
				{
					break;
				}
				pxEndPoint = pxEndPoint->pxNext;
			}
		}
	}

	return pxEndPoint;
}
/*-----------------------------------------------------------*/

/* Find the end-point with given IP-address. */
NetworkEndPoint_t *FreeRTOS_FindEndPointOnIP( uint32_t ulIPAddress, uint32_t ulWhere )
{
NetworkEndPoint_t *pxEndPoint = pxNetworkEndPoints;

	xRoutingStats.ulOnIp++;
	if( ulWhere < ARRAY_SIZE( xRoutingStats.ulLocationsIP ) )
	{
		xRoutingStats.ulLocationsIP[ ulWhere ]++;
	}
	while( pxEndPoint != NULL )
	{
		if( pxEndPoint->ulIPAddress == ulIPAddress )
		{
			break;
		}

		pxEndPoint = pxEndPoint->pxNext;
	}

	return pxEndPoint;
}
/*-----------------------------------------------------------*/
/*_RB_ Is it best, from the end user's perspective, to pass in an end point here, or for data hiding purposes, is it best to pass in a interface number (0, 1, etc.)? */
void FreeRTOS_GetAddressConfiguration( NetworkEndPoint_t *pxEndPoint, uint32_t *pulIPAddress, uint32_t *pulNetMask, uint32_t *pulGatewayAddress, uint32_t *pulDNSServerAddress )
{
	configASSERT( pxEndPoint );

	if( pxEndPoint != NULL )
	{
		if( pulIPAddress != NULL )
		{
			*pulIPAddress = pxEndPoint->ulIPAddress;
		}

		if( pulNetMask != NULL )
		{
			*pulNetMask = pxEndPoint->ulNetMask;
		}

		if( pulGatewayAddress != NULL )
		{
			*pulGatewayAddress = pxEndPoint->ulGatewayAddress;
		}

		if( pulDNSServerAddress != NULL )
		{
			*pulDNSServerAddress = pxEndPoint->ulDNSServerAddresses[ 0 ]; /*_RB_ Only returning the address of the first DNS server. */
		}
	}
}
/*-----------------------------------------------------------*/

#if( ipconfigUSE_IPv6 != 0 )
	/* Find the end-point with given IP-address. */
	NetworkEndPoint_t *FreeRTOS_FindEndPointOnIP_IPv6( const IPv6_Address_t *pxIPAddress )
	{
	NetworkEndPoint_t *pxEndPoint = pxNetworkEndPoints;

		while( pxEndPoint != NULL )
		{
			if( xCompareIPv6_Address( &( pxEndPoint->ulIPAddress_IPv6 ), pxIPAddress ) == 0 )
			{
				break;
			}
			pxEndPoint = pxEndPoint->pxNext;
		}

		return pxEndPoint;
	}
#endif /* ipconfigUSE_IPv6 */
/*-----------------------------------------------------------*/

NetworkEndPoint_t *FreeRTOS_FindEndPointOnMAC( const MACAddress_t *pxMACAddress, NetworkInterface_t *pxInterface )
{
NetworkEndPoint_t *pxEndPoint = pxNetworkEndPoints;

	xRoutingStats.ulOnMAC++;
	/*_RB_ Question - would it be more efficient to store the mac addresses in
	uin64_t variables for direct comparison instead of using memcmp()?  [don't
	know if there is a quick way of creating a 64-bit number from the 48-byte
	MAC address without getting junk in the top 2 bytes]. */

	/* Find the end-point with given MAC-address. */
	while( pxEndPoint != NULL )
	{
		if( ( ( pxInterface == NULL ) || ( pxInterface == pxEndPoint->pxNetworkInterface ) ) &&
			( memcmp( pxEndPoint->xMACAddress.ucBytes, pxMACAddress->ucBytes, ipMAC_ADDRESS_LENGTH_BYTES ) == 0 ) )
		{
			break;
		}

		pxEndPoint = pxEndPoint->pxNext;
	}

	return pxEndPoint;
}
/*-----------------------------------------------------------*/

NetworkEndPoint_t *FreeRTOS_FindEndPointOnNetMask( uint32_t ulIPAddress, uint32_t ulWhere )
{
	/* The 'ulWhere' parameter is only for debugging puposes. */
	return FreeRTOS_InterfaceEndPointOnNetMask( ( NetworkInterface_t * )NULL, ulIPAddress, ulWhere );
}

NetworkEndPoint_t *FreeRTOS_InterfaceEndPointOnNetMask( NetworkInterface_t *pxInterface, uint32_t ulIPAddress, uint32_t ulWhere )
{
NetworkEndPoint_t *pxEndPoint = pxNetworkEndPoints;
NetworkEndPoint_t *pxDefault = NULL;

	xRoutingStats.ulOnNetMask++;
	if( ulWhere < ARRAY_SIZE( xRoutingStats.ulLocations ) )
	{
		xRoutingStats.ulLocations[ ulWhere ]++;
	}
	/* Find the best fitting end-point to reach a given IP-address. */
	/*_RB_ Presumably then a broadcast reply could go out on a different end point to that on which the broadcast was received - although that should not be an issue if the nodes are on the same LAN it could be an issue if the nodes are on separate LANs. */

	while( pxEndPoint != NULL )
	{
		if( ( pxInterface == NULL ) || ( pxEndPoint->pxNetworkInterface == pxInterface ) )
		{
			if( pxEndPoint->bits.bIsDefault != pdFALSE_UNSIGNED )
			{
				pxDefault = pxEndPoint;
			}

			if( ( ulIPAddress & pxEndPoint->ulNetMask ) == ( pxEndPoint->ulIPAddress & pxEndPoint->ulNetMask ) )
			{
				/* Found a match. */
				break;
			}
		}

		pxEndPoint = pxEndPoint->pxNext;
	}

	if( pxEndPoint == NULL )
	{
		if( pxDefault != NULL )
		{
			pxEndPoint = pxDefault;
			FreeRTOS_printf( ( "FreeRTOS_FindEndPointOnNetMask: No match for %lxip using %lxip\n",
				FreeRTOS_ntohl( ulIPAddress ), FreeRTOS_ntohl( pxDefault->ulIPAddress ) ) );
		}
		else
		{
			FreeRTOS_printf( ( "FreeRTOS_FindEndPointOnNetMask: No match for %lxip\n", FreeRTOS_ntohl( ulIPAddress ) ) );
		}
	}

	return pxEndPoint;
}
/*-----------------------------------------------------------*/

/* Find the first IP-address of the default network interface. */
NetworkEndPoint_t *FreeRTOS_FindDefaultEndPoint()
{
NetworkEndPoint_t *pxEndPoint = pxNetworkEndPoints;

	xRoutingStats.ulDefault++;
	while( pxEndPoint != NULL )
	{
		if( pxEndPoint->bits.bIsDefault != pdFALSE_UNSIGNED )
		{
			break;
		}
		pxEndPoint = pxEndPoint->pxNext;
	}

	return pxEndPoint;
}
/*-----------------------------------------------------------*/

/* Return pdTRUE if all end-points are up.
When pxInterface is null, all end-points can be iterated. */
BaseType_t FreeRTOS_AllEndPointsUp( NetworkInterface_t *pxInterface )
{
BaseType_t xResult = pdTRUE;
NetworkEndPoint_t *pxEndPoint = pxNetworkEndPoints;

	while( pxEndPoint != NULL )
	{
		if( ( pxInterface == NULL ) ||
			( pxEndPoint->pxNetworkInterface == pxInterface ) )

		{
			if( pxEndPoint->bits.bEndPointUp == pdFALSE_UNSIGNED )
			{
				xResult = pdFALSE;
				break;
			}
		}
		pxEndPoint = pxEndPoint->pxNext;
	}

	return xResult;
}
/*-----------------------------------------------------------*/

	/* A helper function to fill in a network end-point,
	and add it to a network interface. */
void FreeRTOS_FillEndPoint(	NetworkEndPoint_t *pxNetworkEndPoint,
	const uint8_t ucIPAddress[ ipIP_ADDRESS_LENGTH_BYTES ],
	const uint8_t ucNetMask[ ipIP_ADDRESS_LENGTH_BYTES ],
	const uint8_t ucGatewayAddress[ ipIP_ADDRESS_LENGTH_BYTES ],
	const uint8_t ucDNSServerAddress[ ipIP_ADDRESS_LENGTH_BYTES ],
	const uint8_t ucMACAddress[ ipMAC_ADDRESS_LENGTH_BYTES ] )
{
	/* Fill in and add an end-point to a network interface.
	Make sure that the object pointed to by 'pxNetworkEndPoint'
	will remain to exist. */
	memset( pxNetworkEndPoint, '\0', sizeof( *pxNetworkEndPoint ) );
	pxNetworkEndPoint->ulDefaultIPAddress      = FreeRTOS_inet_addr_quick( ucIPAddress[ 0 ], ucIPAddress[ 1 ], ucIPAddress[ 2 ], ucIPAddress[ 3 ] );
	pxNetworkEndPoint->ulNetMask               = FreeRTOS_inet_addr_quick( ucNetMask[ 0 ], ucNetMask[ 1 ], ucNetMask[ 2 ], ucNetMask[ 3 ] );
	pxNetworkEndPoint->ulGatewayAddress        = FreeRTOS_inet_addr_quick( ucGatewayAddress[ 0 ], ucGatewayAddress[ 1 ], ucGatewayAddress[ 2 ], ucGatewayAddress[ 3 ] );
	pxNetworkEndPoint->ulDNSServerAddresses[ 0 ] = FreeRTOS_inet_addr_quick( ucDNSServerAddress[ 0 ], ucDNSServerAddress[ 1 ], ucDNSServerAddress[ 2 ], ucDNSServerAddress[ 3 ] );
	memcpy( pxNetworkEndPoint->xMACAddress.ucBytes, ucMACAddress, sizeof( pxNetworkEndPoint->xMACAddress ) );

	pxNetworkEndPoint->ulBroadcastAddress      = pxNetworkEndPoint->ulDefaultIPAddress | ~( pxNetworkEndPoint->ulNetMask );
}
/*-----------------------------------------------------------*/

#if( ipconfigUSE_IPv6 != 0 )
	static NetworkEndPoint_t *prvFindFirstAddress_IPv6( void )
	{
		NetworkEndPoint_t *pxEndPoint = pxNetworkEndPoints;
		while( pxEndPoint != NULL )
		{
			if( pxEndPoint->bits.bIPv6 != pdFALSE_UNSIGNED )
			{
				break;
			}
			pxEndPoint = pxEndPoint->pxNext;
		}
		return pxEndPoint;
	}
#endif /* ipconfigUSE_IPv6 */
/*-----------------------------------------------------------*/

#if( ipconfigUSE_IPv6 != 0 )
	NetworkEndPoint_t *FreeRTOS_FindEndPointOnNetMask_IPv6( IPv6_Address_t *pxIPv6Address )
	{
		return prvFindFirstAddress_IPv6();
	}
#endif /* ipconfigUSE_IPv6 */
/*-----------------------------------------------------------*/

NetworkEndPoint_t *FreeRTOS_MatchingEndpoint( NetworkInterface_t *pxNetworkInterface, uint8_t *pucEthernetBuffer )
{
NetworkEndPoint_t *pxEndPoint;
ProtocolPacket_t *pxPacket;

	xRoutingStats.ulMatching++;
	/* Remove compiler warning when ipconfigUSE_IPv6 is not set. */
	( void ) pxNetworkInterface;

	pxPacket = ( ProtocolPacket_t * ) pucEthernetBuffer;

	pxEndPoint = FreeRTOS_FindEndPointOnMAC( &( pxPacket->xUDPPacket.xEthernetHeader.xDestinationAddress ), NULL );

	if( pxEndPoint != NULL )
	{
		/* This is the most precise determination, of match of the destination
		MAC address. */
	}
	else if( pxPacket->xUDPPacket.xEthernetHeader.usFrameType == ipARP_FRAME_TYPE )
	{
		pxEndPoint = FreeRTOS_FindEndPointOnIP( pxPacket->xARPPacket.xARPHeader.ulTargetProtocolAddress, 5 );
	}
#if( ipconfigUSE_IPv6 != 0 )
	else if( pxPacket->xUDPPacket.xEthernetHeader.usFrameType == ipIPv6_FRAME_TYPE )
	{
		IPPacket_IPv6_t *pxIPPacket_IPv6 = ( IPPacket_IPv6_t * )pucEthernetBuffer;

		pxEndPoint = pxNetworkEndPoints;
		while( pxEndPoint != NULL )
		{
			if( ( pxEndPoint->pxNetworkInterface == pxNetworkInterface ) &&
				( xCompareIPv6_Address( &( pxEndPoint->ulIPAddress_IPv6 ), &( pxIPPacket_IPv6->xIPHeader_IPv6.xDestinationIPv6Address ) ) == 0 ) )
			{
				break;
			}
			pxEndPoint = pxEndPoint->pxNext;
		}

		if( ( pxEndPoint == NULL ) && ( xCompareIPv6_Address( &( ipLLMNR_IP_ADDR_IPv6 ), &( pxIPPacket_IPv6->xIPHeader_IPv6.xDestinationIPv6Address ) ) == 0 ) )
		{
			pxEndPoint = FreeRTOS_FirstEndPoint_IPv6( pxNetworkInterface );
		}
	}
#endif /* ipconfigUSE_IPv6 */
	else
	{
		/* An IPv4 UDP or TCP packet. */
		pxEndPoint = FreeRTOS_FindEndPointOnIP( pxPacket->xUDPPacket.xIPHeader.ulDestinationIPAddress, 6 );
	}

	return pxEndPoint;
}
/*-----------------------------------------------------------*/

#if( ipconfigUSE_IPv6 != 0 )
	/* Get the first end-point belonging to a given interface.
	When pxInterface is NULL, the very first end-point will be returned. */
	NetworkEndPoint_t *FreeRTOS_FirstEndPoint_IPv6( NetworkInterface_t *pxInterface )
	{
	NetworkEndPoint_t *pxEndPoint = pxNetworkEndPoints;

		if( pxInterface != NULL )
		{
			while( pxEndPoint != NULL )
			{
				if( ( pxEndPoint->pxNetworkInterface == pxInterface ) && ( pxEndPoint->bits.bIPv6 != pdFALSE_UNSIGNED ) )
				{
					break;
				}
				pxEndPoint = pxEndPoint->pxNext;
			}
		}

		return pxEndPoint;
	}
#endif /* ipconfigUSE_IPv6 */
/*-----------------------------------------------------------*/

#if( ipconfigUSE_IPv6 != 0 )
	static void prvReturnICMP_IPv6( NetworkBufferDescriptor_t * const pxNetworkBuffer, size_t xICMPSize )
	{
	NetworkEndPoint_t *pxEndPoint = pxNetworkBuffer->pxEndPoint;
	ICMPPacket_IPv6_t *pxICMPPacket = ( ICMPPacket_IPv6_t * )pxNetworkBuffer->pucEthernetBuffer;
	ICMPHeader_IPv6_t *ICMPHeader_IPv6 = ( ICMPHeader_IPv6_t * )&( pxICMPPacket->xICMPHeader );

		memcpy( pxICMPPacket->xIPHeader.xDestinationIPv6Address.ucBytes, pxICMPPacket->xIPHeader.xSourceIPv6Address.ucBytes, sizeof( IPv6_Address_t ) );
		memcpy( pxICMPPacket->xIPHeader.xSourceIPv6Address.ucBytes, pxEndPoint->ulIPAddress_IPv6.ucBytes, sizeof( IPv6_Address_t ) );
		pxICMPPacket->xIPHeader.usPayloadLength = FreeRTOS_htons( xICMPSize );

		/* Important: tell NIC driver how many bytes must be sent */
		pxNetworkBuffer->xDataLength = ( size_t ) ( ipSIZE_OF_ETH_HEADER + ipSIZE_OF_IP_HEADER_IPv6 + xICMPSize );

		ICMPHeader_IPv6->usChecksum = 0;
		/* calculate the UDP checksum for outgoing package */
		usGenerateProtocolChecksum( ( uint8_t* ) pxNetworkBuffer->pucEthernetBuffer, pxNetworkBuffer->xDataLength, pdTRUE );

// FreeRTOS_printf( ( "ICMPv6 return %d bytes\n", pxNetworkBuffer->xDataLength ) );
		/* This function will fill in the eth addresses and send the packet */
		vReturnEthernetFrame( pxNetworkBuffer, pdFALSE );
	}
#endif /* ipconfigUSE_IPv6 */
/*-----------------------------------------------------------*/

#if( ipconfigUSE_IPv6 != 0 )
	eFrameProcessingResult_t prvProcessICMPMessage_IPv6( NetworkBufferDescriptor_t * const pxNetworkBuffer )
	{
	ICMPPacket_IPv6_t *pxICMPPacket = ( ICMPPacket_IPv6_t * )pxNetworkBuffer->pucEthernetBuffer;
	ICMPHeader_IPv6_t *ICMPHeader_IPv6 = ( ICMPHeader_IPv6_t * )&( pxICMPPacket->xICMPHeader );
	NetworkEndPoint_t *pxEndPoint = pxNetworkBuffer->pxEndPoint;
	size_t xNeededSize;

//		if( ICMPHeader_IPv6->ucTypeOfMessage != ipICMP_PING_REQUEST_IPv6 )
//		{
//			FreeRTOS_printf( ( "ICMPv6 message %d from %02x:%02x:%02x:%02x:%02x:%02x\n",
//				ICMPHeader_IPv6->ucTypeOfMessage,
//				ICMPHeader_IPv6->ucOptions[ 2 ],
//				ICMPHeader_IPv6->ucOptions[ 3 ],
//				ICMPHeader_IPv6->ucOptions[ 4 ],
//				ICMPHeader_IPv6->ucOptions[ 5 ],
//				ICMPHeader_IPv6->ucOptions[ 6 ],
//				ICMPHeader_IPv6->ucOptions[ 7 ] ) );
//		}
		if( pxEndPoint != NULL )
		{
			switch( ICMPHeader_IPv6->ucTypeOfMessage )
			{
		/*
			uint8_t ucTypeOfMessage;   // 0 + 1 = 1
			uint8_t ucTypeOfService;   // 1 + 1 = 2
			uint16_t usChecksum;       // 2 + 2 = 4
			uint16_t usIdentifier;     // 4 + 2 = 6
			uint16_t usSequenceNumber; // 6 + 2 = 8
		*/
				case ipICMP_PING_REQUEST_IPv6 :
					if( pxEndPoint != NULL )
					{
					size_t xICMPSize = FreeRTOS_ntohs( pxICMPPacket->xIPHeader.usPayloadLength );

						xNeededSize = ( size_t ) ( ipSIZE_OF_ETH_HEADER + ipSIZE_OF_IP_HEADER_IPv6 + xICMPSize );
						if( xNeededSize > pxNetworkBuffer->xDataLength )
						{
							FreeRTOS_printf( ("Too small\n" ) );
							break;
						}

						ICMPHeader_IPv6->ucTypeOfMessage = ipICMP_PING_REPLY_IPv6;
						prvReturnICMP_IPv6( pxNetworkBuffer, xICMPSize );
					}
					break;
				case ipICMP_NEIGHBOR_SOLICITATION_IPv6 :
					if( pxEndPoint != NULL )
					{
					size_t xICMPSize;
		/*
			uint8_t ucTypeOfMessage;       //  0 +  1 = 1
			uint8_t ucTypeOfService;       //  1 +  1 = 2
			uint16_t usChecksum;           //  2 +  2 = 4
			uint32_t ulReserved;           //  4 +  4 = 8
			IPv6_Address_t xIPv6_Address;  //  8 + 16 = 24
			uint8_t ucOptions[8];          // 24 +  8 = 32
		*/
						xICMPSize = sizeof( ICMPHeader_IPv6_t );
						xNeededSize = ( size_t ) ( ipSIZE_OF_ETH_HEADER + ipSIZE_OF_IP_HEADER_IPv6 + xICMPSize );
						if( xNeededSize > pxNetworkBuffer->xDataLength )
						{
							FreeRTOS_printf( ("Too small\n" ) );
							break;
						}
						ICMPHeader_IPv6->ucTypeOfMessage = ipICMP_NEIGHBOR_ADVERTISEMENT_IPv6;
						ICMPHeader_IPv6->ucTypeOfService = 0;
						ICMPHeader_IPv6->ulReserved = FreeRTOS_htonl( ICMPv6_FLAG_SOLICITED | ICMPv6_FLAG_UPDATE );

						/* Type of option. */
						ICMPHeader_IPv6->ucOptions[ 0 ] = ipICMP_TARGET_LINK_LAYER_ADDRESS;
						/* Length of option in units of 8 bytes. */
						ICMPHeader_IPv6->ucOptions[ 1 ] = 1;
						memcpy( ICMPHeader_IPv6->ucOptions + 2, pxEndPoint->xMACAddress.ucBytes, sizeof( MACAddress_t ) );
						pxICMPPacket->xIPHeader.ucHopLimit = 255;
						memcpy( ICMPHeader_IPv6->xIPv6_Address.ucBytes, pxEndPoint->ulIPAddress_IPv6.ucBytes, sizeof( ICMPHeader_IPv6->xIPv6_Address.ucBytes ) );

						prvReturnICMP_IPv6( pxNetworkBuffer, xICMPSize );
					}

					break;
			}
		}

		return eReleaseBuffer;
	//	return eFrameConsumed;
	//	return eReturnEthernetFrame;
	}
#endif /* ipconfigUSE_IPv6 */
/*-----------------------------------------------------------*/

// All nodes on the local network segment
static const uint8_t pcLOCAL_NETWORK_MULTICAST_IP[ 16 ] = { 0xff, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01 };
static const uint8_t pcLOCAL_NETWORK_MULTICAST_MAC[ ipMAC_ADDRESS_LENGTH_BYTES ] = { 0x01, 0x80, 0xC2, 0x00, 0x00, 0x00 };
#if( ipconfigUSE_IPv6 != 0 )
	#define ADVERTISE_PACKET_SIZE	64

	void FreeRTOS_OutputAdvertiseIPv6( NetworkEndPoint_t *pxEndPoint )
	{
	NetworkBufferDescriptor_t *pxNetworkBuffer;
	ICMPPacket_IPv6_t *pxICMPPacket;
	NetworkInterface_t *pxInterface;
	ICMPHeader_IPv6_t *ICMPHeader_IPv6;
	size_t xICMPSize;
	size_t xPacketSize;

		xPacketSize = ( size_t ) ( ipSIZE_OF_ETH_HEADER + ipSIZE_OF_IP_HEADER_IPv6 + sizeof( ICMPHeader_IPv6_t ) );

		/* This is called from the context of the IP event task, so a block time
		must not be used. */
		pxNetworkBuffer = pxGetNetworkBufferWithDescriptor( xPacketSize, 0 );
		if( pxNetworkBuffer != NULL )
		{
			pxNetworkBuffer->ulIPAddress = 0;
			pxNetworkBuffer->pxEndPoint = pxEndPoint;

			pxInterface = pxNetworkBuffer->pxEndPoint->pxNetworkInterface;

			configASSERT( pxInterface != NULL );

			pxICMPPacket = ( ICMPPacket_IPv6_t * )pxNetworkBuffer->pucEthernetBuffer;
			ICMPHeader_IPv6 = ( ICMPHeader_IPv6_t * )&( pxICMPPacket->xICMPHeader );

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

			xICMPSize = sizeof( ICMPHeader_IPv6_t );
			ICMPHeader_IPv6->ucTypeOfMessage = ipICMP_NEIGHBOR_ADVERTISEMENT_IPv6;
			ICMPHeader_IPv6->ucTypeOfService = 0;
			ICMPHeader_IPv6->ulReserved = FreeRTOS_htonl( ICMPv6_FLAG_SOLICITED | ICMPv6_FLAG_UPDATE );

			/* Type of option. */
			ICMPHeader_IPv6->ucOptions[ 0 ] = ipICMP_TARGET_LINK_LAYER_ADDRESS;
			/* Length of option in units of 8 bytes. */
			ICMPHeader_IPv6->ucOptions[ 1 ] = 1;
			memcpy( ICMPHeader_IPv6->ucOptions + 2, pxEndPoint->xMACAddress.ucBytes, sizeof( MACAddress_t ) );
			pxICMPPacket->xIPHeader.ucHopLimit = 255;
			memcpy( ICMPHeader_IPv6->xIPv6_Address.ucBytes, pxEndPoint->ulIPAddress_IPv6.ucBytes, sizeof( ICMPHeader_IPv6->xIPv6_Address.ucBytes ) );

			/* Important: tell NIC driver how many bytes must be sent */
			pxNetworkBuffer->xDataLength = ( size_t ) ( ipSIZE_OF_ETH_HEADER + ipSIZE_OF_IP_HEADER_IPv6 + xICMPSize );

			ICMPHeader_IPv6->usChecksum = 0;
			/* calculate the UDP checksum for outgoing package */
			usGenerateProtocolChecksum( ( uint8_t* ) pxNetworkBuffer->pucEthernetBuffer, pxNetworkBuffer->xDataLength, pdTRUE );

			/* Set the parameter 'bReleaseAfterSend'. */
			pxInterface->pfOutput( pxInterface, pxNetworkBuffer, pdTRUE );
		}
	}
#endif
/*-----------------------------------------------------------*/


NetworkEndPoint_t *pxGetSocketEndpoint( Socket_t xSocket )
{
FreeRTOS_Socket_t *pxSocket = ( FreeRTOS_Socket_t * ) xSocket;
NetworkEndPoint_t *pxResult;

	if( pxSocket )
	{
		pxResult = pxSocket->pxEndPoint;
	}
	else
	{
		pxResult = NULL;
	}

	return pxResult;	
}
/*-----------------------------------------------------------*/

void vSetSocketEndpoint( Socket_t xSocket, NetworkEndPoint_t *pxEndPoint )
{
FreeRTOS_Socket_t *pxSocket = ( FreeRTOS_Socket_t * ) xSocket;

	pxSocket->pxEndPoint = pxEndPoint;
}
/*-----------------------------------------------------------*/

