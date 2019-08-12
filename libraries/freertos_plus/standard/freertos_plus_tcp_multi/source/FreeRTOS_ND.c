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
#include "FreeRTOS_ND.h"
#include "FreeRTOS_UDP_IP.h"
#if( ipconfigUSE_LLMNR == 1 )
	#include "FreeRTOS_DNS.h"
#endif /* ipconfigUSE_LLMNR */

#if( ipconfigUSE_IPv6 != 0 )

/*
 * Lookup an MAC address in the ND cache from the IP address.
 */
static eARPLookupResult_t prvCacheLookup( IPv6_Address_t *pxAddressToLookup, MACAddress_t * const pxMACAddress );

/*-----------------------------------------------------------*/

/* The ND cache. */
static NDCacheRow_t xNDCache[ ipconfigND_CACHE_ENTRIES ];


eARPLookupResult_t eNDGetCacheEntry( IPv6_Address_t *pxIPAddress, MACAddress_t * const pxMACAddress )
{
eARPLookupResult_t eReturn;
/* For testing now, fill in Hein's laptop: 9c 5c 8e 38 06 6c */
//static const unsigned char testMAC[] = { 0x9C, 0x5C, 0x8E, 0x38, 0x06, 0x6C };
//
//	memcpy( pxMACAddress->ucBytes, testMAC, sizeof testMAC );

	eReturn = prvCacheLookup( pxIPAddress, pxMACAddress );

	return eReturn;
}
/*-----------------------------------------------------------*/

#if( ipconfigUSE_IPv6 != 0 )
	void vNDRefreshCacheEntry( const MACAddress_t * pxMACAddress, const IPv6_Address_t *pxIPAddress )
	{
	BaseType_t x;
	BaseType_t xEntryFound = ( BaseType_t )-1;
		/* For each entry in the ARP cache table. */
		for( x = 0; x < ipconfigND_CACHE_ENTRIES; x++ )
		{
			if( xNDCache[ x ].ucValid == ( uint8_t )pdFALSE )
			{
				if( xEntryFound == ( BaseType_t )-1 )
				{
					xEntryFound = x;
				}
			}
			else if( memcmp( xNDCache[ x ].xIPAddress.ucBytes, pxIPAddress->ucBytes, sizeof( IPv6_Address_t ) ) == 0 )
			{
				xEntryFound = x;
				break;
			}
		}
		if( xEntryFound >= ( BaseType_t )0 )
		{
			/* Copy the IP-address. */
			memcpy( xNDCache[ xEntryFound ].xIPAddress.ucBytes, pxIPAddress->ucBytes, sizeof( IPv6_Address_t ) );
			/* Copy the MAC-address. */
			memcpy( xNDCache[ xEntryFound ].xMACAddress.ucBytes, pxMACAddress->ucBytes, sizeof( MACAddress_t ) );
			xNDCache[ xEntryFound ].ucAge = ( uint8_t ) ipconfigMAX_ARP_AGE;
			xNDCache[ xEntryFound ].ucValid = ( uint8_t ) pdTRUE;
			FreeRTOS_printf( ( "vNDRefreshCacheEntry[ %d ] %pip with %02x:%02x:%02x\n",
				( int ) xEntryFound,
				pxIPAddress->ucBytes,
				pxMACAddress->ucBytes[ 3 ],
				pxMACAddress->ucBytes[ 4 ],
				pxMACAddress->ucBytes[ 5 ] ) );
		}
	}
#endif /* ipconfigUSE_IPv6 */
/*-----------------------------------------------------------*/

void FreeRTOS_ClearND( void )
{
	memset( xNDCache, '\0', sizeof( xNDCache ) );
}
/*-----------------------------------------------------------*/

static eARPLookupResult_t prvCacheLookup( IPv6_Address_t *pxAddressToLookup, MACAddress_t * const pxMACAddress )
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
				FreeRTOS_printf( ( "prvCacheLookup[ %d ] %pip with %02x:%02x:%02x\n",
					( int ) x,
					pxAddressToLookup->ucBytes,
					pxMACAddress->ucBytes[ 3 ],
					pxMACAddress->ucBytes[ 4 ],
					pxMACAddress->ucBytes[ 5 ] ) );
			break;
		}
	}
	return eReturn;
}

#if( ipconfigUSE_IPv6 != 0 ) && ( ( ipconfigHAS_PRINTF != 0 ) || ( ipconfigHAS_DEBUG_PRINTF != 0 ) )

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

/* The module is only included in case ipconfigUSE_IPv6 != 0. */

#endif /* ipconfigUSE_IPv6 */

