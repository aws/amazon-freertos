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
#include "FreeRTOS_UDP_IP.h"
#include "FreeRTOS_ARP.h"
#include "FreeRTOS_DHCP.h"
#include "NetworkBufferManagement.h"
#include "FreeRTOS_Routing.h"
#include "FreeRTOS_ND.h"

#if( ipconfigUSE_DNS == 1 ) || ( ipconfigUSE_NBNS == 1 )
	#include "FreeRTOS_DNS.h"
#endif

/* The expected IP version and header length coded into the IP header itself. */
#define ipIP_VERSION_AND_HEADER_LENGTH_BYTE ( ( uint8_t ) 0x45 )

/* Part of the Ethernet and IP headers are always constant when sending an IPv4
UDP packet.  This array defines the constant parts, allowing this part of the
packet to be filled in using a simple memcpy() instead of individual writes. */
UDPPacketHeader_t xDefaultPartUDPPacketHeader =
{
#ifdef _lint
	.ucBytes =
#endif
	{
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 	/* Ethernet source MAC address. */
		0x08, 0x00, 							/* Ethernet frame type. */
		ipIP_VERSION_AND_HEADER_LENGTH_BYTE, 	/* ucVersionHeaderLength. */
		0x00, 									/* ucDifferentiatedServicesCode. */
		0x00, 0x00, 							/* usLength. */
		0x00, 0x00, 							/* usIdentification. */
		0x00, 0x00, 							/* usFragmentOffset. */
		ipconfigUDP_TIME_TO_LIVE, 				/* ucTimeToLive */
		ipPROTOCOL_UDP, 						/* ucProtocol. */
		0x00, 0x00, 							/* usHeaderChecksum. */
		0x00, 0x00, 0x00, 0x00 					/* Source IP address. */
	}
};	/*lint !e9018 declaration of symbol with union based type [MISRA 2012 Rule 19.2, advisory] */
/*-----------------------------------------------------------*/

void vProcessGeneratedUDPPacket( NetworkBufferDescriptor_t * const pxNetworkBuffer )
{
UDPPacket_t *pxUDPPacket;
IPHeader_t *pxIPHeader;
#if( ipconfigUSE_IPv6 != 0 )
	UDPPacket_IPv6_t *pxUDPPacket_IPv6;
	IPHeader_IPv6_t *pxIPHeader_IPv6;
#endif
eARPLookupResult_t eReturned;
uint32_t ulIPAddress = pxNetworkBuffer->ulIPAddress;
NetworkEndPoint_t *pxEndPoint = pxNetworkBuffer->pxEndPoint;
BaseType_t xIsIPV6 = pdFALSE;

	/* Map the UDP packet onto the start of the frame. */
	pxUDPPacket = ipPOINTER_CAST( UDPPacket_t *, pxNetworkBuffer->pucEthernetBuffer );

	/* Create short cuts to the data within the packet. */
	pxIPHeader = &( pxUDPPacket->xIPHeader );

	#if( ipconfigUSE_IPv6 != 0 )
	pxUDPPacket_IPv6 = ipPOINTER_CAST( UDPPacket_IPv6_t *, pxNetworkBuffer->pucEthernetBuffer );
	if( pxUDPPacket->xEthernetHeader.usFrameType == ipIPv6_FRAME_TYPE )
	{
		xIsIPV6 = pdTRUE;
		pxIPHeader_IPv6 = &( pxUDPPacket_IPv6->xIPHeader );
	}
	else
	#endif
	{
		pxUDPPacket->xEthernetHeader.usFrameType = ipIPv4_FRAME_TYPE;

		memset( pxIPHeader, '\0', sizeof *pxIPHeader );
		pxIPHeader->ucVersionHeaderLength = ipIP_VERSION_AND_HEADER_LENGTH_BYTE;
		pxIPHeader->ucTimeToLive = ipconfigUDP_TIME_TO_LIVE;
	}

	/* Determine the ARP cache status for the requested IP address.  This may
	change the ulIPAddress to the router address.
	Beside the MAC-address, a reference to the network Interface will
    be return. */
	#if( ipconfigUSE_IPv6 != 0 )
	if( xIsIPV6 )
	{
		eReturned = eNDGetCacheEntry( &( pxNetworkBuffer->xIPv6_Address ), &( pxUDPPacket->xEthernetHeader.xDestinationAddress ), &( pxEndPoint ) );
		if( pxNetworkBuffer->pxEndPoint == NULL )
		{
			pxNetworkBuffer->pxEndPoint = pxEndPoint;
		}
	}
	else
	#endif
	if( ( ( FreeRTOS_ntohl( ulIPAddress ) & 0xff ) == 0xff ) && ( pxEndPoint != NULL ) )
    {
        memset( pxUDPPacket->xEthernetHeader.xDestinationAddress.ucBytes, 0xff, ipMAC_ADDRESS_LENGTH_BYTES );
        eReturned = eARPCacheHit;
    }
    else
    {
    	eReturned = eARPGetCacheEntry( &( ulIPAddress ), &( pxUDPPacket->xEthernetHeader.xDestinationAddress ), &( pxEndPoint ) );
		if( pxNetworkBuffer->pxEndPoint == NULL )
		{
			pxNetworkBuffer->pxEndPoint = pxEndPoint;
		}
    }

	if( eReturned != eCantSendPacket )
	{
		if( eReturned == eARPCacheHit )
		{
			#if( ipconfigDRIVER_INCLUDED_TX_IP_CHECKSUM == 0 )
				uint8_t ucSocketOptions;
			#endif
			iptraceSENDING_UDP_PACKET( pxNetworkBuffer->ulIPAddress );

			/* Is it possible that the packet is not actually a UDP packet
			after all, but an ICMP packet. */
			if( pxNetworkBuffer->usPort != ipPACKET_CONTAINS_ICMP_DATA )
			{
			UDPHeader_t *pxUDPHeader;

				#if( ipconfigUSE_IPv6 != 0 )
				if( xIsIPV6 )
				{
					pxUDPHeader = &( pxUDPPacket_IPv6->xUDPHeader );

					pxUDPPacket_IPv6->xIPHeader.ucVersionTrafficClass = 0x60;
					pxUDPPacket_IPv6->xIPHeader.ucTrafficClassFlow = 0;
					pxUDPPacket_IPv6->xIPHeader.usFlowLabel = 0;
					pxUDPPacket_IPv6->xIPHeader.ucHopLimit = 255;
				}
				else
				#endif
				{
					pxUDPHeader = &( pxUDPPacket->xUDPHeader );
				}

				pxUDPHeader->usDestinationPort = pxNetworkBuffer->usPort;
				pxUDPHeader->usSourcePort = pxNetworkBuffer->usBoundPort;
				pxUDPHeader->usLength = ( uint16_t ) ( pxNetworkBuffer->xDataLength + sizeof( UDPHeader_t ) );
				pxUDPHeader->usLength = FreeRTOS_htons( pxUDPHeader->usLength );
				pxUDPHeader->usChecksum = 0u;
			}

			/* memcpy() the constant parts of the header information into
			the	correct location within the packet.  This fills in:
				xEthernetHeader.xSourceAddress
				xEthernetHeader.usFrameType
				xIPHeader.ucVersionHeaderLength
				xIPHeader.ucDifferentiatedServicesCode
				xIPHeader.usLength
				xIPHeader.usIdentification
				xIPHeader.usFragmentOffset
				xIPHeader.ucTimeToLive
				xIPHeader.ucProtocol
			and
				xIPHeader.usHeaderChecksum
			*/

			/* Save options now, as they will be overwritten by memcpy */
			#if( ipconfigDRIVER_INCLUDED_TX_IP_CHECKSUM == 0 )
			{
				ucSocketOptions = pxNetworkBuffer->pucEthernetBuffer[ ipSOCKET_OPTIONS_OFFSET ];
			}
			#endif

			/* lint Warning -- Apparent data overrun for function 'memcpy()', argument 3 (size=24) exceeds argument 1 (size=6)  [MISRA 2012 Rule 1.3, required] */

			if( pxNetworkBuffer->usPort == ipPACKET_CONTAINS_ICMP_DATA )
			{
				#if( ipconfigUSE_IPv6 != 0 )
				if( xIsIPV6 != pdFALSE )
				{
					pxIPHeader_IPv6->ucNextHeader = ipPROTOCOL_ICMP_IPv6;
				}
				else
				#endif	/* ( ipconfigUSE_IPv6 != 0 ) */
				{
					pxIPHeader->ucProtocol = ipPROTOCOL_ICMP;
					pxIPHeader->usLength = pxNetworkBuffer->xDataLength - ipSIZE_OF_ETH_HEADER;
					pxIPHeader->usLength = FreeRTOS_htons( pxIPHeader->usLength );
					pxIPHeader->ulDestinationIPAddress = pxNetworkBuffer->ulIPAddress;
				}
			}
			else
			{
				#if( ipconfigUSE_IPv6 != 0 )
				if( xIsIPV6 != pdFALSE )
				{
					pxUDPPacket_IPv6->xIPHeader.ucNextHeader = ipPROTOCOL_UDP;
					pxUDPPacket_IPv6->xIPHeader.usPayloadLength = ( uint16_t ) ( pxNetworkBuffer->xDataLength + sizeof( UDPHeader_t ) );
					/* The total transmit size adds on the Ethernet header. */
					pxNetworkBuffer->xDataLength = sizeof( EthernetHeader_t ) + sizeof( IPHeader_IPv6_t ) + pxUDPPacket_IPv6->xIPHeader.usPayloadLength;
					pxUDPPacket_IPv6->xIPHeader.usPayloadLength = FreeRTOS_htons( pxUDPPacket_IPv6->xIPHeader.usPayloadLength );
					memcpy( pxUDPPacket_IPv6->xIPHeader.xDestinationIPv6Address.ucBytes, pxNetworkBuffer->xIPv6_Address.ucBytes, ipSIZE_OF_IPv6_ADDRESS );
				}
				else
				#endif
				{
					pxIPHeader->ucProtocol = ipPROTOCOL_UDP;
					pxIPHeader->usLength = ( uint16_t ) ( pxNetworkBuffer->xDataLength + sizeof( IPHeader_t ) + sizeof( UDPHeader_t ) );
					/* The total transmit size adds on the Ethernet header. */
					pxNetworkBuffer->xDataLength = pxIPHeader->usLength + sizeof( EthernetHeader_t );
					pxIPHeader->usLength = FreeRTOS_htons( pxIPHeader->usLength );
					pxIPHeader->ulDestinationIPAddress = pxNetworkBuffer->ulIPAddress;
				}
			}

			if( pxNetworkBuffer->pxEndPoint != NULL )
			{
//				FreeRTOS_printf( ( "vProcessGeneratedUDPPacket: End-point already known\n" ) );
			}
			else if( pxEndPoint != NULL )
			{
				/* Using the end-point found in the ARP table. */
				pxNetworkBuffer->pxEndPoint = pxEndPoint;
//				FreeRTOS_printf( ( "vProcessGeneratedUDPPacket: remote IP = %lxip end-point = %lxip\n",
//					FreeRTOS_htonl( pxNetworkBuffer->ulIPAddress ),
//					FreeRTOS_htonl( pxNetworkBuffer->pxEndPoint != 0 ? pxNetworkBuffer->pxEndPoint->ulIPAddress : 0x0ul ) ) );
			}
			else
			{
				/* Look it up. */
			}

			/* Which end point should the ping go out on? */
			if( pxNetworkBuffer->pxEndPoint == NULL )
			{
				pxNetworkBuffer->pxEndPoint = FreeRTOS_FindEndPointOnNetMask( pxNetworkBuffer->ulIPAddress, 10 );
				if( pxNetworkBuffer->pxEndPoint == NULL )
				{
					pxNetworkBuffer->pxEndPoint = FreeRTOS_FirstEndPoint( NULL );
					if( pxNetworkBuffer->pxEndPoint == NULL )
					{
						FreeRTOS_printf( ( "vProcessGeneratedUDPPacket: No pxEndPoint found? Using %lxip\n",
							pxNetworkBuffer->pxEndPoint ? FreeRTOS_ntohl( pxNetworkBuffer->pxEndPoint->ipv4.ulIPAddress ) : 0uL ) );	/*lint !e9027: Unpermitted operand to operator '?' [MISRA 2012 Rule 10.1, required] */
					}
				}
			}

			#if( ipconfigUSE_LLMNR == 1 )
			{
				/* LLMNR messages are typically used on a LAN and they're
				 * not supposed to cross routers */
				if( pxNetworkBuffer->ulIPAddress == ipLLMNR_IP_ADDR )
				{
					pxIPHeader->ucTimeToLive = 0x01;
				}
			}
			#endif
			if( pxNetworkBuffer->pxEndPoint != NULL )
			{
				#if( ipconfigUSE_IPv6 != 0 )
				if( xIsIPV6 != pdFALSE )
				{
					memcpy( pxIPHeader_IPv6->xSourceIPv6Address.ucBytes, pxNetworkBuffer->pxEndPoint->ipv6.xIPAddress.ucBytes, ipSIZE_OF_IPv6_ADDRESS );
				}
				else
				#endif
				{
					pxIPHeader->ulSourceIPAddress = pxNetworkBuffer->pxEndPoint->ipv4.ulIPAddress;
				}
			}
			#if( ipconfigDRIVER_INCLUDED_TX_IP_CHECKSUM == 0 )
			{
				#if( ipconfigUSE_IPv6 != 0 )
				if( xIsIPV6 == pdFALSE )
				#endif
				{
					pxIPHeader->usHeaderChecksum = 0u;
					pxIPHeader->usHeaderChecksum = usGenerateChecksum( 0UL, ( uint8_t * ) &( pxIPHeader->ucVersionHeaderLength ), ipSIZE_OF_IPv4_HEADER );
					pxIPHeader->usHeaderChecksum = ~FreeRTOS_htons( pxIPHeader->usHeaderChecksum );
				}

				if( ( ucSocketOptions & ( uint8_t ) FREERTOS_SO_UDPCKSUM_OUT ) != 0u )
				{
					( void ) usGenerateProtocolChecksum( (uint8_t*)pxUDPPacket, pxNetworkBuffer->xDataLength, pdTRUE );
				}
				else
				{
					pxUDPPacket->xUDPHeader.usChecksum = 0u;
				}
			}
			#endif
		}
		else if( eReturned == eARPCacheMiss )
		{
			#if( ipconfigUSE_IPv6 != 0 )
			if( xIsIPV6 )
			{
//				pxNetworkBuffer->pxEndPoint = FreeRTOS_FindEndPointOnNetMask_IPv6( &( pxNetworkBuffer->xIPv6_Address ) );
FreeRTOS_printf( ( "Looking up %pip with%s end-point\n", pxNetworkBuffer->xIPv6_Address.ucBytes, ( pxNetworkBuffer->pxEndPoint != NULL ) ? "" : "out" ) );
				if( pxNetworkBuffer->pxEndPoint )
				{
					vNDGenerateRequestPacket( pxNetworkBuffer, &( pxNetworkBuffer->xIPv6_Address ) );
					memcpy( pxIPHeader_IPv6->xDestinationIPv6Address.ucBytes, pxNetworkBuffer->xIPv6_Address.ucBytes, ipSIZE_OF_IPv6_ADDRESS );
				}
			}
			else
			#endif
			{
				FreeRTOS_printf( ( "Looking up %lxip%s with%s end-point\n",
					FreeRTOS_ntohl( ulIPAddress ),
					( pxNetworkBuffer->ulIPAddress != ulIPAddress ) ? " (gateway)" : "",
					( pxNetworkBuffer->pxEndPoint != NULL ) ? "" : "out" ) );

				/* Add an entry to the ARP table with a null hardware address.
				This allows the ARP timer to know that an ARP reply is
				outstanding, and perform retransmissions if necessary. */
				vARPRefreshCacheEntry( NULL, ulIPAddress, NULL );

				/* Generate an ARP for the required IP address. */
				iptracePACKET_DROPPED_TO_GENERATE_ARP( pxNetworkBuffer->ulIPAddress );
				pxNetworkBuffer->ulIPAddress = ulIPAddress;

				/* 'ulIPAddress' might have become the address of the Gateway.
				Find the route again. */
				pxNetworkBuffer->pxEndPoint = FreeRTOS_FindEndPointOnNetMask( ulIPAddress, 11 );	/* ARP request */
				vARPGenerateRequestPacket( pxNetworkBuffer );
			}
		}
		else
		{
			/* The lookup indicated that an ARP request has already been
			sent out for the queried IP address. */
			eReturned = eCantSendPacket;
		}
	}

	if( eReturned != eCantSendPacket )
	{
		/* The network driver is responsible for freeing the network buffer
		after the packet has been sent. */
		if( pxNetworkBuffer->pxEndPoint != NULL )
		{
		NetworkInterface_t *pxInterface = pxNetworkBuffer->pxEndPoint->pxNetworkInterface;

			memcpy( &( pxUDPPacket->xEthernetHeader.xSourceAddress), pxNetworkBuffer->pxEndPoint->xMACAddress.ucBytes, ( size_t ) ipMAC_ADDRESS_LENGTH_BYTES );
			#if defined( ipconfigETHERNET_MINIMUM_PACKET_BYTES )
			{
				if( pxNetworkBuffer->xDataLength < ( size_t ) ipconfigETHERNET_MINIMUM_PACKET_BYTES )
				{
				BaseType_t xIndex;
	
					//FreeRTOS_printf( ( "vProcessGeneratedUDPPacket: length %u\n", pxNetworkBuffer->xDataLength ) );
					for( xIndex = ( BaseType_t ) pxNetworkBuffer->xDataLength; xIndex < ( BaseType_t ) ipconfigETHERNET_MINIMUM_PACKET_BYTES; xIndex++ )
					{
						pxNetworkBuffer->pucEthernetBuffer[ xIndex ] = 0u;
					}
					pxNetworkBuffer->xDataLength = ( size_t ) ipconfigETHERNET_MINIMUM_PACKET_BYTES;
				}
			}
			#endif

			#if( ipconfigUSE_IPv6 != 0 )
//			if( ( xIsIPV6 != pdFALSE ) && ( pxNetworkBuffer->usPort == ipPACKET_CONTAINS_ICMP_DATA ) )
			if( xIsIPV6 != pdFALSE )
			{
				FreeRTOS_printf( ( "From %02x:%02x:%02x:%02x:%02x:%02x %pip\n",
					pxUDPPacket->xEthernetHeader.xSourceAddress.ucBytes[ 0 ],
					pxUDPPacket->xEthernetHeader.xSourceAddress.ucBytes[ 1 ],
					pxUDPPacket->xEthernetHeader.xSourceAddress.ucBytes[ 2 ],
					pxUDPPacket->xEthernetHeader.xSourceAddress.ucBytes[ 3 ],
					pxUDPPacket->xEthernetHeader.xSourceAddress.ucBytes[ 4 ],
					pxUDPPacket->xEthernetHeader.xSourceAddress.ucBytes[ 5 ],
					pxIPHeader_IPv6->xSourceIPv6Address.ucBytes ) );
				FreeRTOS_printf( ( "To   %02x:%02x:%02x:%02x:%02x:%02x %pip\n",
					pxUDPPacket->xEthernetHeader.xDestinationAddress.ucBytes[ 0 ],
					pxUDPPacket->xEthernetHeader.xDestinationAddress.ucBytes[ 1 ],
					pxUDPPacket->xEthernetHeader.xDestinationAddress.ucBytes[ 2 ],
					pxUDPPacket->xEthernetHeader.xDestinationAddress.ucBytes[ 3 ],
					pxUDPPacket->xEthernetHeader.xDestinationAddress.ucBytes[ 4 ],
					pxUDPPacket->xEthernetHeader.xDestinationAddress.ucBytes[ 5 ],
					pxIPHeader_IPv6->xDestinationIPv6Address.ucBytes ) );
			}
			#endif

			( void ) pxInterface->pfOutput( pxInterface, pxNetworkBuffer, pdTRUE );
		}
		else
		{
			/* The packet can't be sent (no route found).  Drop the packet. */
			vReleaseNetworkBufferAndDescriptor( pxNetworkBuffer );
		}
	}
	else
	{
		/* The packet can't be sent (DHCP not completed?).  Just drop the
		packet. */
		vReleaseNetworkBufferAndDescriptor( pxNetworkBuffer );
	}
}
/*-----------------------------------------------------------*/

BaseType_t xProcessReceivedUDPPacket( NetworkBufferDescriptor_t *pxNetworkBuffer, uint16_t usPort )
{
BaseType_t xReturn = pdPASS;
FreeRTOS_Socket_t *pxSocket;
ProtocolHeaders_t *pxProtocolHeaders;	/*lint !e9018 declaration of symbol  with union based type [MISRA 2012 Rule 19.2, advisory] */
UDPPacket_t *pxUDPPacket = ipPOINTER_CAST( UDPPacket_t *, pxNetworkBuffer->pucEthernetBuffer );
#if( ipconfigUSE_IPv6 != 0 )
	UDPPacket_IPv6_t *pxUDPPacket_IPv6;
#endif
BaseType_t xIsIPV6 = pdFALSE;

	pxSocket = pxUDPSocketLookup( usPort );

	pxProtocolHeaders = ipPOINTER_CAST( ProtocolHeaders_t *, &( pxNetworkBuffer->pucEthernetBuffer[ ipSIZE_OF_ETH_HEADER + xIPHeaderSize( pxNetworkBuffer ) ] ) );
	#if( ipconfigUSE_IPv6 != 0 )
	pxUDPPacket_IPv6 = ipPOINTER_CAST( UDPPacket_IPv6_t *, pxNetworkBuffer->pucEthernetBuffer );
	if( pxUDPPacket->xEthernetHeader.usFrameType == ipIPv6_FRAME_TYPE )
	{
		xIsIPV6 = pdTRUE;
	}
	#endif
	//25353 -> 25097
	//if( usPort != 25097u )
/*
	{
		FreeRTOS_printf( ( "UDP from %lxip port %u -> %u (%u)\n",
			FreeRTOS_ntohl( pxUDPPacket->xIPHeader.ulSourceIPAddress ),
			FreeRTOS_ntohs( pxProtocolHeaders->xUDPHeader.usSourcePort ),
			FreeRTOS_ntohs( pxProtocolHeaders->xUDPHeader.usDestinationPort ),
			usPort ) );
	}
*/
	if( pxSocket != NULL )
	{

		#if( ipconfigUSE_IPv6 != 0 )
		if( xIsIPV6 )
		{
			vNDRefreshCacheEntry( &( pxUDPPacket_IPv6->xEthernetHeader.xSourceAddress ), &( pxUDPPacket_IPv6->xIPHeader.xSourceIPv6Address ), pxNetworkBuffer->pxEndPoint );
		}
		else
		#endif
		{
			/* When refreshing the ARP cache with received UDP packets we must be
			careful;  hundreds of broadcast messages may pass and if we're not
			handling them, no use to fill the ARP cache with those IP addresses. */
			vARPRefreshCacheEntry( &( pxUDPPacket->xEthernetHeader.xSourceAddress ), pxUDPPacket->xIPHeader.ulSourceIPAddress, pxNetworkBuffer->pxEndPoint );
		}

		#if( ipconfigUSE_CALLBACKS == 1 )
		{
			/* Did the owner of this socket register a reception handler ? */
			if( ipconfigIS_VALID_PROG_ADDRESS( pxSocket->u.xUDP.pxHandleReceive ) )
			{
				void *pcData = &( pxNetworkBuffer->pucEthernetBuffer[ ipSIZE_OF_UDP_HEADER ] );
				FOnUDPReceive_t xHandler = ( FOnUDPReceive_t ) pxSocket->u.xUDP.pxHandleReceive;
			#if( ipconfigUSE_IPv6 != 0 )
				struct freertos_sockaddr6 xSourceAddress, destinationAddress;
			#else
				struct freertos_sockaddr xSourceAddress, destinationAddress;
			#endif

				xSourceAddress.sin_port = pxNetworkBuffer->usPort;
				destinationAddress.sin_port = usPort;
				
				#if( ipconfigUSE_IPv6 != 0 )
				if( xIsIPV6 )
				{

					memcpy( xSourceAddress.sin_addrv6.ucBytes,     pxUDPPacket_IPv6->xIPHeader.xSourceIPv6Address.ucBytes,      ipSIZE_OF_IPv6_ADDRESS );
					memcpy( destinationAddress.sin_addrv6.ucBytes, pxUDPPacket_IPv6->xIPHeader.xDestinationIPv6Address.ucBytes, ipSIZE_OF_IPv6_ADDRESS );
					xSourceAddress.sin_family     = FREERTOS_AF_INET6;
					destinationAddress.sin_family = FREERTOS_AF_INET6;
					xSourceAddress.sin_len        = sizeof( xSourceAddress );
					destinationAddress.sin_len    = sizeof( destinationAddress );
				}
				else
				{
				struct freertos_sockaddr *xSourceAddress4     = ipPOINTER_CAST( struct freertos_sockaddr *, &( xSourceAddress ) );
				struct freertos_sockaddr *destinationAddress4 = ipPOINTER_CAST( struct freertos_sockaddr *, &( destinationAddress ) );
					xSourceAddress4->sin_addr       = pxNetworkBuffer->ulIPAddress;
					destinationAddress4->sin_addr   = pxUDPPacket->xIPHeader.ulDestinationIPAddress;
					xSourceAddress4->sin_family     = FREERTOS_AF_INET;
					destinationAddress4->sin_family = FREERTOS_AF_INET;
					xSourceAddress4->sin_len        = sizeof( xSourceAddress );
					destinationAddress4->sin_len    = sizeof( destinationAddress );
				}
				#else
				{
					xSourceAddress.sin_addr = pxNetworkBuffer->ulIPAddress;
					destinationAddress.sin_addr = pxUDPPacket->xIPHeader.ulDestinationIPAddress;
					xSourceAddress.sin_family = FREERTOS_AF_INET4;
					destinationAddress.sin_family = FREERTOS_AF_INET4;
					xSourceAddress.sin_len        = sizeof( xSourceAddress );
					destinationAddress.sin_len    = sizeof( destinationAddress );
				}
				#endif	/* ( ipconfigUSE_IPv6 != 0 ) */

				if( xHandler( ( Socket_t ) pxSocket, ( void* ) pcData, ( size_t ) pxNetworkBuffer->xDataLength,
					( struct freertos_sockaddr * ) &xSourceAddress, ( struct freertos_sockaddr * ) &destinationAddress ) != pdFALSE )
				{
					xReturn = pdFAIL; /* xHandler has consumed the data, do not add it to .xWaitingPacketsList'. */
				}
			}
		}
		#endif /* ipconfigUSE_CALLBACKS */

		#if( ipconfigUDP_MAX_RX_PACKETS > 0 )
		{
			if( xReturn == pdPASS )
			{
				if ( listCURRENT_LIST_LENGTH( &( pxSocket->u.xUDP.xWaitingPacketsList ) ) >= pxSocket->u.xUDP.uxMaxPackets )
				{
					FreeRTOS_debug_printf( ( "xProcessReceivedUDPPacket: buffer full %ld >= %ld port %u\n",
						listCURRENT_LIST_LENGTH( &( pxSocket->u.xUDP.xWaitingPacketsList ) ),
						pxSocket->u.xUDP.uxMaxPackets, pxSocket->usLocalPort ) );
					xReturn = pdFAIL; /* we did not consume or release the buffer */
				}
			}
		}
		#endif

		if( xReturn == pdPASS )
		{
			vTaskSuspendAll();
			{
				/* lint 774: (Info -- Boolean within 'if' always evaluates to True [MISRA 2012 Rule 14.3, required], 
				depending on ipconfig. */
				if( xReturn == pdPASS )	/*lint !e774 */
				{
					taskENTER_CRITICAL();
					{
						/* Add the network packet to the list of packets to be
						processed by the socket. */
						vListInsertEnd( &( pxSocket->u.xUDP.xWaitingPacketsList ), &( pxNetworkBuffer->xBufferListItem ) );
					}
					taskEXIT_CRITICAL();
				}
			}
			( void ) xTaskResumeAll();

			/* Set the socket's receive event */
			if( pxSocket->xEventGroup != NULL )
			{
				( void ) xEventGroupSetBits( pxSocket->xEventGroup, ( EventBits_t ) eSOCKET_RECEIVE );
			}

			#if( ipconfigSUPPORT_SELECT_FUNCTION == 1 )
			{
				if( ( pxSocket->pxSocketSet != NULL ) && ( ( pxSocket->xSelectBits & ( ( EventBits_t ) eSELECT_READ ) ) != 0 ) )
				{
					( void ) xEventGroupSetBits( pxSocket->pxSocketSet->xSelectGroup, ( EventBits_t ) eSELECT_READ );
				}
			}
			#endif

			#if( ipconfigSOCKET_HAS_USER_SEMAPHORE == 1 )
			{
				if( pxSocket->pxUserSemaphore != NULL )
				{
					( void ) xSemaphoreGive( pxSocket->pxUserSemaphore );
				}
			}
			#endif

			#if( ipconfigUSE_DHCP == 1 )
			{
				if( xIsDHCPSocket( pxSocket ) )
				{
				IPStackEvent_t xEventMessage;
				const TickType_t xDontBlock = 0;

					xEventMessage.eEventType = eDHCPEvent;
					xEventMessage.pvData = ( void* )NULL;

					( void ) xSendEventStructToIPTask( &xEventMessage, xDontBlock );
				}
			}
			#endif
		}
	}
	else
	{
		/* There is no socket listening to the target port, but still it might
		be for this node. */

		#if( ipconfigUSE_DNS == 1 )
			/* A DNS reply, check for the source port.  Although the DNS client
			does open a UDP socket to send a messages, this socket will be
			closed after a short timeout.  Messages that come late (after the
			socket is closed) will be treated here. */
			if( FreeRTOS_ntohs( pxProtocolHeaders->xUDPHeader.usSourcePort ) == ipDNS_PORT )
			{
				#if( ipconfigUSE_IPv6 != 0 )
				if( pxUDPPacket->xEthernetHeader.usFrameType == ipIPv6_FRAME_TYPE )
				{
				}
				else
				#endif
				{
					vARPRefreshCacheEntry( &( pxUDPPacket->xEthernetHeader.xSourceAddress ), pxUDPPacket->xIPHeader.ulSourceIPAddress,
						pxNetworkBuffer->pxEndPoint );
				}
				xReturn = ( BaseType_t )ulDNSHandlePacket( pxNetworkBuffer );
			}
			else
		#endif

		#if( ipconfigUSE_LLMNR == 1 )
			/* A LLMNR request, check for the destination port. */
			if( ( usPort == FreeRTOS_ntohs( ipLLMNR_PORT ) ) ||
				( pxProtocolHeaders->xUDPHeader.usSourcePort == FreeRTOS_ntohs( ipLLMNR_PORT ) ) )
			{
				#if( ipconfigUSE_IPv6 != 0 )
				if( pxUDPPacket->xEthernetHeader.usFrameType == ipIPv6_FRAME_TYPE )
				{
				}
				else
				#endif
				{
					vARPRefreshCacheEntry( &( pxUDPPacket->xEthernetHeader.xSourceAddress ), pxUDPPacket->xIPHeader.ulSourceIPAddress,
						pxNetworkBuffer->pxEndPoint );
				}

				xReturn = ( BaseType_t ) ulDNSHandlePacket( pxNetworkBuffer );
			}
			else
		#endif /* ipconfigUSE_LLMNR */

		#if( ipconfigUSE_NBNS == 1 )
			/* a NetBIOS request, check for the destination port */
			if( ( usPort == FreeRTOS_ntohs( ipNBNS_PORT ) ) ||
				( pxProtocolHeaders->xUDPHeader.usSourcePort == FreeRTOS_ntohs( ipNBNS_PORT ) ) )/*lint !e9007 */
			{
				#if( ipconfigUSE_IPv6 != 0 )
				if( pxUDPPacket->xEthernetHeader.usFrameType == ipIPv6_FRAME_TYPE )
				{
				}
				else
				#endif
				{
					vARPRefreshCacheEntry( &( pxUDPPacket->xEthernetHeader.xSourceAddress ), pxUDPPacket->xIPHeader.ulSourceIPAddress,
						pxNetworkBuffer->pxInterface );
				}
				xReturn = ( BaseType_t )ulNBNSHandlePacket( pxNetworkBuffer );
			}
			else
		#endif /* ipconfigUSE_NBNS */
			{
				xReturn = pdFAIL;
			}
	}

	return xReturn;
}
/*-----------------------------------------------------------*/
