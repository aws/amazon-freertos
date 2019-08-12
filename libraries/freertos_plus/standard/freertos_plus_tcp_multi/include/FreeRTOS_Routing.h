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

#ifndef FREERTOS_ROUTING_H
#define FREERTOS_ROUTING_H

#ifdef __cplusplus
extern "C" {
#endif

#include "FreeRTOS_DHCP.h"

/* Every NetworkInterface needs a set of access functions: */

/* Forward declaration of 'struct xNetworkInterface'. */
struct xNetworkInterface;

/* Initialise the interface. */
typedef BaseType_t ( * NetworkInterfaceInitialiseFunction_t ) (
	struct xNetworkInterface * /* pxDescriptor */ );

/* Send out an Ethernet packet. */
typedef BaseType_t ( * NetworkInterfaceOutputFunction_t ) (
	struct xNetworkInterface * /* pxDescriptor */,
	NetworkBufferDescriptor_t * const /* pxNetworkBuffer */,
	BaseType_t /* xReleaseAfterSend */ );

/* Return true as long as the LinkStatus on the PHY is present. */
typedef BaseType_t ( * GetPhyLinkStatusFunction_t ) (
	struct xNetworkInterface * /* pxDescriptor */ );

/* These NetworkInterface access functions are collected in a struct: */

typedef struct xNetworkInterface
{
	const char *pcName;	/* Just for logging, debugging. */
	void *pvArgument;	/* Will be passed to the access functions. */
	NetworkInterfaceInitialiseFunction_t pfInitialise;
	NetworkInterfaceOutputFunction_t pfOutput;
	GetPhyLinkStatusFunction_t pfGetPhyLinkStatus;
	struct
	{
		uint32_t
			bInterfaceUp : 1;
	} bits;

	struct xNetworkEndPoint *pxEndPoint;
	struct xNetworkInterface *pxNext;
} NetworkInterface_t;

/*
	// As an example:
	NetworkInterface_t xZynqDescriptor = {
		.pcName					= "Zynq-GEM",
		.pvArgument				= ( void * )1,
		.pfInitialise           = xZynqGEMInitialise,
		.pfOutput               = xZynqGEMOutput,
		.pfGetPhyLinkStatus     = xZynqGEMGetPhyLinkStatus,
	};
*/

typedef struct xNetworkEndPoint
{
	uint32_t ulDefaultIPAddress;	/* Use this address in case DHCP has failed. */
#if( ipconfigUSE_IPv6 != 0 )
	IPv6_Address_t ulIPAddress_IPv6;
#endif
	uint32_t ulIPAddress;			/* The actual IPv4 address. Will be 0 as long as end-point is still down. */
	uint32_t ulNetMask;
	uint32_t ulGatewayAddress;
	uint32_t ulDNSServerAddresses[ ipconfigENDPOINT_DNS_ADDRESS_COUNT ];
	uint32_t ulBroadcastAddress;
	MACAddress_t xMACAddress;
	struct
	{
		uint32_t
			bIsDefault : 1,
			bWantDHCP : 1,
			#if( ipconfigUSE_IPv6 != 0 )
				bIPv6 : 1,
			#endif /* ipconfigUSE_IPv6 */
			#if( ipconfigUSE_NETWORK_EVENT_HOOK != 0 )
				bCallDownHook : 1,
			#endif /* ipconfigUSE_NETWORK_EVENT_HOOK */
			bEndPointUp : 1;
	} bits;
#if( ipconfigUSE_DHCP != 0 )
	IPTimer_t xDHCPTimer;
	DHCPData_t xDHCPData;
#endif
	NetworkInterface_t *pxNetworkInterface;
	struct xNetworkEndPoint *pxNext;
} NetworkEndPoint_t;

/*
 * Add a new physical Network Interface.  The object pointed to by 'pxInterface'
 * must continue to exist.
 */
NetworkInterface_t *FreeRTOS_AddNetworkInterface( NetworkInterface_t *pxInterface );

/*
 * Add a new IP-address to a Network Interface.  The object pointed to by
 * 'pxEndPoint' must continue to exist.
 */
NetworkEndPoint_t *FreeRTOS_AddEndPoint( NetworkInterface_t *pxInterface, NetworkEndPoint_t *pxEndPoint );

/*
 * Get the first Network Interface.
 */
NetworkInterface_t *FreeRTOS_FirstNetworkInterface( void );

/*
 * Get the next Network Interface.
 */
NetworkInterface_t *FreeRTOS_NextNetworkInterface( NetworkInterface_t *pxInterface );

/*
 * Get the first end-point belonging to a given interface.  When pxInterface is
 * NULL, the very first end-point will be returned.
 */
NetworkEndPoint_t *FreeRTOS_FirstEndPoint( NetworkInterface_t *pxInterface );

/*
 * Get the next end-point.  When pxInterface is null, all end-points can be
 * iterated.
 */
NetworkEndPoint_t *FreeRTOS_NextEndPoint( NetworkInterface_t *pxInterface, NetworkEndPoint_t *pxEndPoint );

/*
 * Find the end-point with given IP-address.
 */
NetworkEndPoint_t *FreeRTOS_FindEndPointOnIP( uint32_t ulIPAddress, uint32_t ulWhere );

#if( ipconfigUSE_IPv6 != 0 )
	/* Find the end-point with given IP-address. */
	NetworkEndPoint_t *FreeRTOS_FindEndPointOnIP_IPv6( const IPv6_Address_t *pxIPAddress );
#endif /* ipconfigUSE_IPv6 */

/*
 * Find the end-point with given MAC-address.
 */
NetworkEndPoint_t *FreeRTOS_FindEndPointOnMAC( const MACAddress_t *pxMACAddress, NetworkInterface_t *pxInterface );

/*
 * Returns the addresses stored in an end point structure.
 */
void FreeRTOS_GetAddressConfiguration( NetworkEndPoint_t *pxEndPoint, uint32_t *pulIPAddress, uint32_t *pulNetMask, uint32_t *pulGatewayAddress, uint32_t *pulDNSServerAddress );

/*
 * Find the best fitting end-point to reach a given IP-address.
 */
NetworkEndPoint_t *FreeRTOS_FindEndPointOnNetMask( uint32_t ulIPAddress, uint32_t ulWhere );

/*
 * Find the best fitting end-point to reach a given IP-address on a given interface
 */
NetworkEndPoint_t *FreeRTOS_InterfaceEndPointOnNetMask( NetworkInterface_t *pxInterface, uint32_t ulIPAddress, uint32_t ulWhere );

#if( ipconfigUSE_IPv6 != 0 )
	NetworkEndPoint_t *FreeRTOS_FindEndPointOnNetMask_IPv6( IPv6_Address_t *pxIPv6Address );
#endif /* ipconfigUSE_IPv6 */

#if( ipconfigUSE_IPv6 != 0 )
	/* Get the first end-point belonging to a given interface.
	When pxInterface is NULL, the very first end-point will be returned. */
	NetworkEndPoint_t *FreeRTOS_FirstEndPoint_IPv6( NetworkInterface_t *pxInterface );
#endif /* ipconfigUSE_IPv6 */

#if( ipconfigUSE_IPv6 != 0 )
	void FreeRTOS_OutputAdvertiseIPv6( NetworkEndPoint_t *pxEndPoint );
#endif

/* A ethernet packet has come in on 'pxMyInterface'. Find the best matching end-point. */
NetworkEndPoint_t *FreeRTOS_MatchingEndpoint( NetworkInterface_t *pxMyInterface, uint8_t *pucEthernetBuffer );

/* Return the default end-point. */
NetworkEndPoint_t *FreeRTOS_FindDefaultEndPoint( void );

/* Fill-in the end-point structure. */
void FreeRTOS_FillEndPoint(	NetworkEndPoint_t *pxNetworkEndPoint,
	const uint8_t ucIPAddress[ ipIP_ADDRESS_LENGTH_BYTES ],
	const uint8_t ucNetMask[ ipIP_ADDRESS_LENGTH_BYTES ],
	const uint8_t ucGatewayAddress[ ipIP_ADDRESS_LENGTH_BYTES ],
	const uint8_t ucDNSServerAddress[ ipIP_ADDRESS_LENGTH_BYTES ],
	const uint8_t ucMACAddress[ ipMAC_ADDRESS_LENGTH_BYTES ] );

/* Return pdTRUE if all end-points are up.
When pxInterface is null, all end-points can be iterated. */
BaseType_t FreeRTOS_AllEndPointsUp( NetworkInterface_t *pxInterface );

typedef struct xRoutingStats
{
	UBaseType_t ulOnIp;
	UBaseType_t ulOnMAC;
	UBaseType_t ulOnNetMask;
	UBaseType_t ulDefault;
	UBaseType_t ulMatching;
	UBaseType_t ulLocations[ 14 ];
	UBaseType_t ulLocationsIP[ 8 ];
} RoutingStats_t;

extern RoutingStats_t xRoutingStats;

NetworkEndPoint_t *pxGetSocketEndpoint( Socket_t xSocket );
void vSetSocketEndpoint( Socket_t xSocket, NetworkEndPoint_t *pxEndPoint );

#ifdef __cplusplus
} // extern "C"
#endif

#endif /* FREERTOS_ROUTING_H */













