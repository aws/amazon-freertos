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
	
/* FreeRTOS+TCP includes. */
#include "FreeRTOS_IP.h"
#include "FreeRTOS_Sockets.h"
#include "FreeRTOS_IP_Private.h"
#include "FreeRTOS_ARP.h"
#include "FreeRTOS_UDP_IP.h"
#include "FreeRTOS_DHCP.h"
/*lint -e766 -e595 */
#include "NetworkBufferManagement.h"
#if( ipconfigUSE_LLMNR == 1 )
	#include "FreeRTOS_DNS.h"
#endif /* ipconfigUSE_LLMNR */
#include "FreeRTOS_Routing.h"

/* A list of all network end-points: */
struct xNetworkEndPoint *pxNetworkEndPoints = NULL;

/* A list of all network interfaces: */
struct xNetworkInterface *pxNetworkInterfaces = NULL;

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
NetworkInterface_t *pxReturn;

	if( pxInterface != NULL )
	{
		pxReturn = pxInterface->pxNext;
	}
	else
	{
		pxReturn = NULL;
	}
	return pxReturn;
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
NetworkEndPoint_t *pxResult = pxEndPoint;

	if( pxResult != NULL )
	{
		pxResult = pxResult->pxNext;
		if( pxInterface != NULL )
		{
			while( pxResult != NULL )
			{
				if( pxResult->pxNetworkInterface == pxInterface )
				{
					break;
				}
				pxResult = pxResult->pxNext;
			}
		}
	}

	return pxResult;
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

#ifndef _lint
	/* Lint would complain about this test. It is there in case configASSERT it not defined. */
	if( pxEndPoint != NULL )
#endif
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
		if( ( pxInterface == NULL ) || ( pxInterface == pxEndPoint->pxNetworkInterface ) )
		{
			if( memcmp( pxEndPoint->xMACAddress.ucBytes, pxMACAddress->ucBytes, ipMAC_ADDRESS_LENGTH_BYTES ) == 0 )
			{
				break;
			}
		}
		pxEndPoint = pxEndPoint->pxNext;
	}

	return pxEndPoint;
}
/*-----------------------------------------------------------*/

NetworkEndPoint_t *FreeRTOS_FindEndPointOnNetMask( uint32_t ulIPAddress, uint32_t ulWhere )
{
	/* The 'ulWhere' parameter is only for debugging puposes. */
	return FreeRTOS_InterfaceEndPointOnNetMask( NULL, ulIPAddress, ulWhere );
}

NetworkEndPoint_t *FreeRTOS_InterfaceEndPointOnNetMask( NetworkInterface_t *pxInterface, uint32_t ulIPAddress, uint32_t ulWhere )
{
NetworkEndPoint_t *pxEndPoint = pxNetworkEndPoints;
//NetworkEndPoint_t *pxDefault = NULL;

	xRoutingStats.ulOnNetMask++;
	if( ulWhere < ARRAY_SIZE( xRoutingStats.ulLocations ) )
	{
		xRoutingStats.ulLocations[ ulWhere ]++;
	}
	/* Find the best fitting end-point to reach a given IP-address. */
	/*_RB_ Presumably then a broadcast reply could go out on a different end point to that on which the broadcast was received - although that should not be an issue if the nodes are on the same LAN it could be an issue if the nodes are on separate LANs. */

	if( ulWhere == 1 )
	{
	#warning debugging
	//    configPRINTF( ( "ulWhere = 1\n" ) );
	}

	while( pxEndPoint != NULL )
	{
		if( ( pxInterface == NULL ) || ( pxEndPoint->pxNetworkInterface == pxInterface ) )
		{
			if( pxEndPoint->bits.bIsDefault != pdFALSE_UNSIGNED )
			{
				//pxDefault = pxEndPoint;
			}

			if( ( ulIPAddress & pxEndPoint->ulNetMask ) == ( pxEndPoint->ulIPAddress & pxEndPoint->ulNetMask ) )
			{
				/* Found a match. */
				break;
			}
		}

		pxEndPoint = pxEndPoint->pxNext;
	}

	if( ( pxEndPoint == NULL ) && ( ulWhere != 1 ) && ( ulWhere != 2 ) )
	{
//		if( pxDefault != NULL )
//		{
//			pxEndPoint = pxDefault;
//			FreeRTOS_printf( ( "FreeRTOS_FindEndPointOnNetMask[%ld]: No match for %lxip using %lxip\n",
//				ulWhere, FreeRTOS_ntohl( ulIPAddress ), FreeRTOS_ntohl( pxDefault->ulIPAddress ) ) );
//		}
//		else
		{
			FreeRTOS_printf( ( "FreeRTOS_FindEndPointOnNetMask[%ld]: No match for %lxip\n",
                ulWhere, FreeRTOS_ntohl( ulIPAddress ) ) );
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
NetworkEndPoint_t *pxEndPoint = NULL;
ProtocolPacket_t *pxPacket = ipPOINTER_CAST( ProtocolPacket_t *, pucEthernetBuffer );	/*lint !e9018 declaration of symbol with union based type [MISRA 2012 Rule 19.2, advisory]. */

uint32_t ulIPTargetAddress = 0uL;
uint32_t ulIPSourceAddress = 0uL;

/*
	BaseType_t xDoLog = pdTRUE;
*/
	#warning For logging only, take this away
	const char *name = "";
	
	/* An Ethernet packet has been received. Inspect the contents to see which
	 * defined end-point has the best mathc.
	 */

	/* Some stats while developing. */
	xRoutingStats.ulMatching++;


	/* Probably an ARP packet or a broadcast. */
	switch( pxPacket->xUDPPacket.xEthernetHeader.usFrameType )
	{
#if( ipconfigUSE_IPv6 != 0 )
	case ipIPv6_FRAME_TYPE:
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
			#if( ipconfigUSE_LLMNR != 0 )
			{
				if( ( pxEndPoint == NULL ) && ( xCompareIPv6_Address( &( ipLLMNR_IP_ADDR_IPv6 ), &( pxIPPacket_IPv6->xIPHeader_IPv6.xDestinationIPv6Address ) ) == 0 ) )
				{
					pxEndPoint = FreeRTOS_FirstEndPoint_IPv6( pxNetworkInterface );
				}
			}
			#endif
		}
		break;
#endif /* ipconfigUSE_IPv6 */
	case ipARP_FRAME_TYPE:
		{
			memcpy( &ulIPSourceAddress, pxPacket->xARPPacket.xARPHeader.ucSenderProtocolAddress, sizeof( ulIPSourceAddress ) );
			ulIPTargetAddress = pxPacket->xARPPacket.xARPHeader.ulTargetProtocolAddress;
			name = "ARP";
		}
		break;

	case ipIPv4_FRAME_TYPE:
		{
			/* An IPv4 UDP or TCP packet. */
			ulIPSourceAddress = pxPacket->xUDPPacket.xIPHeader.ulSourceIPAddress;
			ulIPTargetAddress = pxPacket->xUDPPacket.xIPHeader.ulDestinationIPAddress;
			name = ( pxPacket->xUDPPacket.xIPHeader.ucProtocol == ipPROTOCOL_UDP ) ? "UDP" : "TCP";
		}
		break;
	default:
		{
			/* Frame type not supported. */
			FreeRTOS_printf( ( "Frametpye %04x not supported.\n", FreeRTOS_ntohs( pxPacket->xUDPPacket.xEthernetHeader.usFrameType ) ) );
		}
		break;
	}	/* switch usFrameType */

	if( ulIPTargetAddress != 0uL )
	{
	static const uint8_t ucAllOnes[ ipMAC_ADDRESS_LENGTH_BYTES ] = { 255, 255, 255, 255, 255, 255 };
	BaseType_t xMACBroadcast;
	BaseType_t xIPBroadcast;
	BaseType_t xDone = pdFALSE;

		xMACBroadcast = memcmp( ucAllOnes,
								pxPacket->xUDPPacket.xEthernetHeader.xDestinationAddress.ucBytes,
								ipMAC_ADDRESS_LENGTH_BYTES ) == 0;
		xIPBroadcast = ( ( FreeRTOS_ntohl( ulIPTargetAddress ) & 0xff ) == 0xff );

		for( pxEndPoint = FreeRTOS_FirstEndPoint( pxNetworkInterface );
			 pxEndPoint != NULL;
			 pxEndPoint = FreeRTOS_NextEndPoint( pxNetworkInterface, pxEndPoint ) )
		{
		//BaseType_t xMacSame = memcmp( pxPacket->xUDPPacket.xEthernetHeader.xDestinationAddress.ucBytes, pxEndPoint->xMACAddress.ucBytes, ipMAC_ADDRESS_LENGTH_BYTES ) == 0;
		
			//if( !xMacSame )
/*
			{
				configPRINTF( ( "Compare[%s] %d,%d mine %-16lxip (%02x-%02x) from %-16lxip to %-16lxip (%02x-%02x)\n",
					name,
					( unsigned ) xMACBroadcast,
					( unsigned ) xIPBroadcast,
					FreeRTOS_ntohl( pxEndPoint->ulIPAddress ),
					pxEndPoint->xMACAddress.ucBytes[0],
					pxEndPoint->xMACAddress.ucBytes[1],
					FreeRTOS_ntohl( ulIPSourceAddress ),
					FreeRTOS_ntohl( ulIPTargetAddress ),
					pxPacket->xUDPPacket.xEthernetHeader.xDestinationAddress.ucBytes[0],
					pxPacket->xUDPPacket.xEthernetHeader.xDestinationAddress.ucBytes[1] ) );
			}
*/
			( void ) xMACBroadcast;
			( void ) name;
			if( pxEndPoint->ulIPAddress == ulIPTargetAddress  )
			{
/*				xDoLog = pdFALSE; */
				xDone = pdTRUE;
			}
			else
			if( ( xIPBroadcast != pdFALSE ) && ( ( pxEndPoint->ulIPAddress & pxEndPoint->ulNetMask ) == ( ulIPTargetAddress & pxEndPoint->ulNetMask ) ) )
			{
				xDone = pdTRUE;
			}
			else
			if( ( xIPBroadcast != pdFALSE ) && ( pxPacket->xUDPPacket.xEthernetHeader.usFrameType == ipARP_FRAME_TYPE ) )
			{
				/* Test for a matching source address like 192.168.1.255 */
				if( ( pxEndPoint->ulIPAddress & pxEndPoint->ulNetMask ) == ( ulIPSourceAddress & pxEndPoint->ulNetMask ) )
				{
					xDone = pdTRUE;
				}
			}
			else
			{
				/* This end-point doesn't match with the packet. */
			}
			if( xDone != pdFALSE )
			{
				break;
			}
		}
		if( ( xIPBroadcast ) && ( pxEndPoint == NULL ) )
		{
			pxEndPoint = FreeRTOS_FirstEndPoint( pxNetworkInterface );
		}
	}
/*
	if( ( xDoLog != pdFALSE ) && ( pxEndPoint != NULL ) )
	{
		configPRINTF( ( "Compare[%s] returning %lxip\n", name, ( pxEndPoint != NULL ) ? FreeRTOS_ntohl( pxEndPoint->ulIPAddress ) : 0uL ) );
	}
*/
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

