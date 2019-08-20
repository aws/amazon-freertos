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

#ifndef FREERTOS_DNS_H
#define FREERTOS_DNS_H

#ifdef __cplusplus
extern "C" {
#endif

/* Application level configuration options. */
#include "FreeRTOSIPConfig.h"
#include "IPTraceMacroDefaults.h"


/* The Link-local Multicast Name Resolution (LLMNR)
 * is included.
 * Note that a special MAC address is required in addition to the NIC's actual
 * MAC address: 01:00:5E:00:00:FC
 *
 * The target IP address will be 224.0.0.252
 */
#if( ipconfigBYTE_ORDER == pdFREERTOS_BIG_ENDIAN )
	#define	ipLLMNR_IP_ADDR			0xE00000FCuL
#else
	#define	ipLLMNR_IP_ADDR			0xFC0000E0uL
#endif /* ipconfigBYTE_ORDER == pdFREERTOS_BIG_ENDIAN */

#define	ipLLMNR_PORT	5355 /* Standard LLMNR port. */
#define	ipDNS_PORT		53	/* Standard DNS port. */
#define	ipDHCP_CLIENT	67
#define	ipDHCP_SERVER	68
#define	ipNBNS_PORT		137	/* NetBIOS Name Service. */
#define	ipNBDGM_PORT	138 /* Datagram Service, not included. */

#define dnsTYPE_A_HOST						0x0001u
#define dnsTYPE_AAAA_HOST					0x001Cu

struct freertos_addrinfo
{
   BaseType_t				ai_flags;
   BaseType_t				ai_family;
   BaseType_t				ai_socktype;
   BaseType_t				ai_protocol;
   socklen_t				ai_addrlen;
   struct freertos_sockaddr *ai_addr;
   char						*ai_canonname;
   struct {
		/* In order to avoid allocations, reserve space here for *ai_addr and *ai_canonname. */
		#if( ipconfigUSE_IPv6 != 0 )
			struct freertos_sockaddr6 sockaddr6;
		#else
			struct freertos_sockaddr sockaddr4;
		#endif
		char ucName[ ipconfigDNS_CACHE_NAME_LENGTH ];
	} xPrivateStorage;
};

/*
 * The following function should be provided by the user and return true if it
 * matches the domain name.
 */
struct xNetworkEndPoint;
extern BaseType_t xApplicationDNSQueryHook( struct xNetworkEndPoint *pxEndPoint, const char *pcName );

/*
 * LLMNR is very similar to DNS, so is handled by the DNS routines.
 */
uint32_t ulDNSHandlePacket( NetworkBufferDescriptor_t *pxNetworkBuffer );

#if( ipconfigUSE_LLMNR == 1 )
	/* The LLMNR MAC address is 01:00:5e:00:00:fc */
	extern const MACAddress_t xLLMNR_MacAdress;
#endif /* ipconfigUSE_LLMNR */

#if( ipconfigUSE_LLMNR == 1 ) && ( ipconfigUSE_IPv6 != 0 )

	/* The LLMNR IPv6 address is ff02::1:3 */
	extern const IPv6_Address_t ipLLMNR_IP_ADDR_IPv6;

	/* The LLMNR IPv6 MAC address is 33:33:00:01:00:03 */
	extern const MACAddress_t xLLMNR_MacAdressIPv6;
#endif /* ipconfigUSE_LLMNR */

#if( ipconfigUSE_NBNS != 0 )

	/*
	 * Inspect a NetBIOS Names-Service message.  If the name matches with ours
	 * (xApplicationDNSQueryHook returns true) an answer will be sent back.
	 * Note that LLMNR is a better protocol for name services on a LAN as it is
	 * less polluted
	 */
	uint32_t ulNBNSHandlePacket (NetworkBufferDescriptor_t *pxNetworkBuffer );

#endif /* ipconfigUSE_NBNS */

#if( ipconfigUSE_DNS_CACHE != 0 )

	uint32_t FreeRTOS_dnslookup( const char *pcHostName );
	#if( ipconfigUSE_IPv6 != 0 )
		uint32_t FreeRTOS_dnslookup6( const char *pcHostName, IPv6_Address_t *pxAddress_IPv6 );
	#endif
    void FreeRTOS_dnsclear( void );

#endif /* ipconfigUSE_DNS_CACHE != 0 */

#if( ipconfigDNS_USE_CALLBACKS != 0 )

	/*
	 * Users may define this type of function as a callback.
	 * It will be called when a DNS reply is received or when a timeout has been reached.
	 */
	#if( ipconfigUSE_IPv6 != 0 )
		typedef void (* FOnDNSEvent ) ( const char * /* pcName */, void * /* pvSearchID */, struct freertos_sockaddr6 * /* pxAddress6 */ );
	#else
		typedef void (* FOnDNSEvent ) ( const char * /* pcName */, void * /* pvSearchID */, uint32_t /* ulIPAddress */ );
	#endif

	/*
	 * Asynchronous version of gethostbyname()
	 * xTimeout is in units of ms.
	 */
	uint32_t FreeRTOS_gethostbyname_a( const char *pcHostName, FOnDNSEvent pCallback, void *pvSearchID, TickType_t xTimeout );
	void FreeRTOS_gethostbyname_cancel( void *pvSearchID );

	BaseType_t FreeRTOS_getaddrinfo_a( const char *pcName,					/* The name of the node or device */
								 const char *pcService,						/* Ignored for now. */
								 const struct freertos_addrinfo *pxHints,	/* If not NULL: preferences. */
								 struct freertos_addrinfo **ppxResult,		/* An allocated struct, containing the results. */
								 FOnDNSEvent pCallback,
								 void *pvSearchID,
								 TickType_t xTimeout );

#endif

/*
 * Lookup a IPv4 node in a blocking-way.
 */
uint32_t FreeRTOS_gethostbyname( const char *pcHostName );

BaseType_t FreeRTOS_getaddrinfo( const char *pcName,						/* The name of the node or device */
								 const char *pcService,						/* Ignored for now. */
								 const struct freertos_addrinfo *pxHints,	/* If not NULL: preferences. */
								 struct freertos_addrinfo **ppxResult );	/* An allocated struct, containing the results. */

void FreeRTOS_freeaddrinfo(struct freertos_addrinfo *pxResult);


#ifdef __cplusplus
}	/* extern "C" */
#endif

#endif /* FREERTOS_DNS_H */













