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

#ifndef FREERTOS_ND_H
#define FREERTOS_ND_H

#ifdef __cplusplus
extern "C" {
#endif

/* Application level configuration options. */
#include "FreeRTOSIPConfig.h"
#include "FreeRTOSIPConfigDefaults.h"
#include "IPTraceMacroDefaults.h"

#if( ipconfigUSE_IPv6 != 0 )
/*-----------------------------------------------------------*/
/* Miscellaneous structure and definitions. */
/*-----------------------------------------------------------*/

typedef struct xND_CACHE_TABLE_ROW
{
	IPv6_Address_t xIPAddress;	/* The IP address of an ND cache entry. */
	MACAddress_t xMACAddress;	/* The MAC address of an ND cache entry. */
	uint8_t ucAge;				/* A value that is periodically decremented but can also be refreshed by active communication.  The ND cache entry is removed if the value reaches zero. */
    uint8_t ucValid;			/* pdTRUE: xMACAddress is valid, pdFALSE: waiting for ND reply */
} NDCacheRow_t;

/*
 * If ulIPAddress is already in the ND cache table then reset the age of the
 * entry back to its maximum value.  If ulIPAddress is not already in the ND
 * cache table then add it - replacing the oldest current entry if there is not
 * a free space available.
 */
void vNDRefreshCacheEntry( const MACAddress_t * pxMACAddress, const IPv6_Address_t *pxIPAddress );

#if( ipconfigUSE_ARP_REMOVE_ENTRY != 0 )

	/*
	 * In some rare cases, it might be useful to remove a ND cache entry of a
	 * known MAC address to make sure it gets refreshed.
	 */
	uint32_t ulNDRemoveCacheEntryByMac( const MACAddress_t * pxMACAddress );

#endif /* ipconfigUSE_ARP_REMOVE_ENTRY != 0 */

/*
 * Look for ulIPAddress in the ND cache.  If the IP address exists, copy the
 * associated MAC address into pxMACAddress, refresh the ND cache entry's
 * age, and return eARPCacheHit.  If the IP address does not exist in the ND
 * cache return eARPCacheMiss.  If the packet cannot be sent for any reason
 * (maybe DHCP is still in process, or the addressing needs a gateway but there
 * isn't a gateway defined) then return eCantSendPacket.
 */
eARPLookupResult_t eNDGetCacheEntry( IPv6_Address_t *pxIPAddress, MACAddress_t * const pxMACAddress );

#if( ipconfigUSE_ARP_REVERSED_LOOKUP != 0 )

	/* Lookup an IP-address if only the MAC-address is known */
	eARPLookupResult_t eNDGetCacheEntryByMac( MACAddress_t * const pxMACAddress, IPv6_Address_t *pxIPAddress );

#endif
/*
 * Reduce the age count in each entry within the ND cache.  An entry is no
 * longer considered valid and is deleted if its age reaches zero.
 */
void vNDAgeCache( void );

/*
 * Send out an ND request for the IP address contained in pxNetworkBuffer, and
 * add an entry into the ND table that indicates that an ND reply is
 * outstanding so re-transmissions can be generated.
 */
void vNDGenerateRequestPacket( NetworkBufferDescriptor_t * const pxNetworkBuffer );

/*
 * After DHCP is ready and when changing IP address, force a quick send of our new IP
 * address
 */
void vNDSendGratuitous( void );

void FreeRTOS_PrintNDCache( void );

#endif /* ipconfigUSE_IPv6 != 0 */

#ifdef __cplusplus
} // extern "C"
#endif

#endif /* FREERTOS_ND_H */













