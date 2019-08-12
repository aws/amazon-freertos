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
#include "FreeRTOS_UDP_IP.h"
#include "FreeRTOS_ARP.h"
#include "FreeRTOS_DHCP.h"
#include "NetworkInterface.h"
#include "NetworkBufferManagement.h"
#include "FreeRTOS_Routing.h"

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
	/* .ucBytes : */
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
};
/*-----------------------------------------------------------*/

void vProcessGeneratedUDPPacket( NetworkBufferDescriptor_t * const pxNetworkBuffer )
{
UDPPacket_t *pxUDPPacket;
IPHeader_t *pxIPHeader;
eARPLookupResult_t eReturned;
uint32_t ulIPAddress = pxNetworkBuffer->ulIPAddress;
NetworkInterface_t *pxInterface;

	/* Map the UDP packet onto the start of the frame. */
	pxUDPPacket = ( UDPPacket_t * ) pxNetworkBuffer->pucEthernetBuffer;

	/* Create short cuts to the data within the packet. */
	pxIPHeader = &( pxUDPPacket->xIPHeader );

	/* Determine the ARP cache status for the requested IP address.  This may
	change the ulIPAddress to the router address.
	Beside the MAC-address, a reference to the network Interface will
    be return. */
	eReturned = eARPGetCacheEntry( &( ulIPAddress ), &( pxUDPPacket->xEthernetHeader.xDestinationAddress ), &( pxInterface ) );

	if( eReturned != eCantSendPacket )
	{
		if( eReturned == eARPCacheHit )
		{
			#if( ipconfigDRIVER_INCLUDED_TX_IP_CHECKSUM == 0 )
				uint8_t ucSocketOptions;
			#endif
			iptraceSENDING_UDP_PACKET( pxNetworkBuffer->ulIPAddress );

		#if ( ipconfigSUPPORT_OUTGOING_PINGS == 1 )
			/* Is it possible that the packet is not actually a UDP packet
			after all, but an ICMP packet. */
			if( pxNetworkBuffer->usPort != ipPACKET_CONTAINS_ICMP_DATA )
		#endif /* ipconfigSUPPORT_OUTGOING_PINGS */
			{
			UDPHeader_t *pxUDPHeader;

				pxUDPHeader = &( pxUDPPacket->xUDPHeader );

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

			memcpy( ( void *) &( pxUDPPacket->xEthernetHeader.xSourceAddress ), ( void * ) xDefaultPartUDPPacketHeader.ucBytes, sizeof( xDefaultPartUDPPacketHeader ) );

		#if ipconfigSUPPORT_OUTGOING_PINGS == 1
			if( pxNetworkBuffer->usPort == ipPACKET_CONTAINS_ICMP_DATA )
			{
				pxIPHeader->ucProtocol = ipPROTOCOL_ICMP;
				pxIPHeader->usLength = ( uint16_t ) ( pxNetworkBuffer->xDataLength + sizeof( IPHeader_t ) );
			}
			else
		#endif /* ipconfigSUPPORT_OUTGOING_PINGS */
			{
				pxIPHeader->usLength = ( uint16_t ) ( pxNetworkBuffer->xDataLength + sizeof( IPHeader_t ) + sizeof( UDPHeader_t ) );
			}

			/* The total transmit size adds on the Ethernet header. */
			pxNetworkBuffer->xDataLength = pxIPHeader->usLength + sizeof( EthernetHeader_t );
			pxIPHeader->usLength = FreeRTOS_htons( pxIPHeader->usLength );
			pxIPHeader->ulDestinationIPAddress = pxNetworkBuffer->ulIPAddress;

			if( pxInterface != NULL )
			{
				if( pxNetworkBuffer->pxEndPoint != NULL )
				{
					FreeRTOS_printf( ( "vProcessGeneratedUDPPacket: End-point already known\n" ) );
				}
				else
				{
					pxNetworkBuffer->pxEndPoint = FreeRTOS_InterfaceEndPointOnNetMask( pxInterface, pxNetworkBuffer->ulIPAddress, 20 );
//					FreeRTOS_printf( ( "vProcessGeneratedUDPPacket: remote IP = %lxip end-point = %lxip\n",
//						FreeRTOS_htonl( pxNetworkBuffer->ulIPAddress ),
//						FreeRTOS_htonl( pxNetworkBuffer->pxEndPoint != 0 ? pxNetworkBuffer->pxEndPoint->ulIPAddress : 0x0ul ) ) );
				}
			}
			/* Which end point should the ping go out on? */
			if( pxNetworkBuffer->pxEndPoint == NULL )
			{
				pxNetworkBuffer->pxEndPoint = FreeRTOS_FindEndPointOnNetMask( pxNetworkBuffer->ulIPAddress, 10 );
				if( pxNetworkBuffer->pxEndPoint == NULL )
				{
					FreeRTOS_printf( ( "vProcessGeneratedUDPPacket: No pxEndPoint found???\n" ) );
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
				else if( pxNetworkBuffer->pxEndPoint != NULL )
				{
					pxIPHeader->ulSourceIPAddress = pxNetworkBuffer->pxEndPoint->ulIPAddress;
				}
			}
			#else
			{
				if( pxNetworkBuffer->pxEndPoint != NULL )
				{
					pxIPHeader->ulSourceIPAddress = pxNetworkBuffer->pxEndPoint->ulIPAddress;
				}
			}
			#endif

			#if( ipconfigDRIVER_INCLUDED_TX_IP_CHECKSUM == 0 )
			{
				#if( ipconfigUSE_IPv6 != 0 )
				if( pxUDPPacket->xEthernetHeader.usFrameType != ipIPv6_FRAME_TYPE )
				#endif
				{
					pxIPHeader->usHeaderChecksum = 0u;
					pxIPHeader->usHeaderChecksum = usGenerateChecksum( 0UL, ( uint8_t * ) &( pxIPHeader->ucVersionHeaderLength ), ipSIZE_OF_IP_HEADER_IPv4 );
					pxIPHeader->usHeaderChecksum = ~FreeRTOS_htons( pxIPHeader->usHeaderChecksum );
				}

				if( ( ucSocketOptions & ( uint8_t ) FREERTOS_SO_UDPCKSUM_OUT ) != 0u )
				{
					usGenerateProtocolChecksum( (uint8_t*)pxUDPPacket, pdTRUE );
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
			/* Add an entry to the ARP table with a null hardware address.
			This allows the ARP timer to know that an ARP reply is
			outstanding, and perform retransmissions if necessary. */
			vARPRefreshCacheEntry( NULL, ulIPAddress, NULL );

			/* Generate an ARP for the required IP address. */
			iptracePACKET_DROPPED_TO_GENERATE_ARP( pxNetworkBuffer->ulIPAddress );
			pxNetworkBuffer->ulIPAddress = ulIPAddress;

			/* 'ulIPAddress' might have become the address of the Gateway.
			Find the route again. */
			pxNetworkBuffer->pxEndPoint = FreeRTOS_FindEndPointOnNetMask( ulIPAddress, 11 );
			vARPGenerateRequestPacket( pxNetworkBuffer );
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

			memcpy( ( void * ) &( pxUDPPacket->xEthernetHeader.xSourceAddress), ( void * ) pxNetworkBuffer->pxEndPoint->xMACAddress.ucBytes, ( size_t ) ipMAC_ADDRESS_LENGTH_BYTES );
			#if defined( ipconfigETHERNET_MINIMUM_PACKET_BYTES )
			{
				if( pxNetworkBuffer->xDataLength < ( size_t ) ipconfigETHERNET_MINIMUM_PACKET_BYTES )
				{
				BaseType_t xIndex;
	
					FreeRTOS_printf( ( "vProcessGeneratedUDPPacket: length %lu\n", pxNetworkBuffer->xDataLength ) );
					for( xIndex = ( BaseType_t ) pxNetworkBuffer->xDataLength; xIndex < ( BaseType_t ) ipconfigETHERNET_MINIMUM_PACKET_BYTES; xIndex++ )
					{
						pxNetworkBuffer->pucEthernetBuffer[ xIndex ] = 0u;
					}
					pxNetworkBuffer->xDataLength = ( size_t ) ipconfigETHERNET_MINIMUM_PACKET_BYTES;
				}
			}
			#endif

			pxInterface->pfOutput( pxInterface, pxNetworkBuffer, pdTRUE );
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
ProtocolHeaders_t *pxProtocolHeaders;
UDPPacket_t *pxUDPPacket = (UDPPacket_t *) pxNetworkBuffer->pucEthernetBuffer;

	pxSocket = pxUDPSocketLookup( usPort );

	pxProtocolHeaders = ( ProtocolHeaders_t * ) ( pxNetworkBuffer->pucEthernetBuffer + ipSIZE_OF_ETH_HEADER + xIPHeaderSize( pxNetworkBuffer ) );

	if( pxSocket != NULL )
	{

		/* When refreshing the ARP cache with received UDP packets we must be
		careful;  hundreds of broadcast messages may pass and if we're not
		handling them, no use to fill the ARP cache with those IP addresses. */
		vARPRefreshCacheEntry( &( pxUDPPacket->xEthernetHeader.xSourceAddress ), pxUDPPacket->xIPHeader.ulSourceIPAddress, pxNetworkBuffer->pxInterface );

		#if( ipconfigUSE_CALLBACKS == 1 )
		{
			/* Did the owner of this socket register a reception handler ? */
			if( ipconfigIS_VALID_PROG_ADDRESS( pxSocket->u.xUDP.pxHandleReceive ) )
			{
				struct freertos_sockaddr xSourceAddress, destinationAddress;
				void *pcData = ( void * )( ( ( uint8_t * ) &( pxProtocolHeaders->xUDPHeader ) ) + ipSIZE_OF_UDP_HEADER );
				FOnUDPReceive_t xHandler = ( FOnUDPReceive_t ) pxSocket->u.xUDP.pxHandleReceive;
				xSourceAddress.sin_port = pxNetworkBuffer->usPort;
				xSourceAddress.sin_addr = pxNetworkBuffer->ulIPAddress;
				destinationAddress.sin_port = usPort;
				destinationAddress.sin_addr = pxUDPPacket->xIPHeader.ulDestinationIPAddress;

				if( xHandler( ( Socket_t * ) pxSocket, ( void* ) pcData, ( size_t ) pxNetworkBuffer->xDataLength,
					&xSourceAddress, &destinationAddress ) != pdFALSE )
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
				/* Add the network packet to the list of packets to be
				processed by the socket. */
				vListInsertEnd( &( pxSocket->u.xUDP.xWaitingPacketsList ), &( pxNetworkBuffer->xBufferListItem ) );
			}
			xTaskResumeAll();

			/* Set the socket's receive event */
			if( pxSocket->xEventGroup != NULL )
			{
				xEventGroupSetBits( pxSocket->xEventGroup, eSOCKET_RECEIVE );
			}

			#if( ipconfigSUPPORT_SELECT_FUNCTION == 1 )
			{
				if( ( pxSocket->pxSocketSet != NULL ) && ( ( pxSocket->xSelectBits & eSELECT_READ ) != 0 ) )
				{
					xEventGroupSetBits( pxSocket->pxSocketSet->xSelectGroup, eSELECT_READ );
				}
			}
			#endif

			#if( ipconfigSOCKET_HAS_USER_SEMAPHORE == 1 )
			{
				if( pxSocket->pxUserSemaphore != NULL )
				{
					xSemaphoreGive( pxSocket->pxUserSemaphore );
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

					xSendEventStructToIPTask( &xEventMessage, xDontBlock );
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
						pxNetworkBuffer->pxInterface );
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
						pxNetworkBuffer->pxInterface );
				}

				xReturn = ( BaseType_t ) ulDNSHandlePacket( pxNetworkBuffer );
			}
			else
		#endif /* ipconfigUSE_LLMNR */

		#if( ipconfigUSE_NBNS == 1 )
			/* a NetBIOS request, check for the destination port */
			if( ( usPort == FreeRTOS_ntohs( ipNBNS_PORT ) ) ||
				( pxProtocolHeaders->xUDPHeader.usSourcePort == FreeRTOS_ntohs( ipNBNS_PORT ) ) )
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
