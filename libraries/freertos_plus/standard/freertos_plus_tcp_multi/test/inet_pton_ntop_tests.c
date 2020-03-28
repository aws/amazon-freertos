/*
 * Tests for inet_ntop() and inet_pton()
 */


/* Standard includes. */
#include <stdio.h>
#include <time.h>
#include <conio.h>

/* FreeRTOS includes. */
#include <FreeRTOS.h>
#include "task.h"

/* Demo application includes. */

/* FreeRTOS+TCP includes. */
#include "FreeRTOS_IP.h"
#include "FreeRTOS_Sockets.h"

#include "FreeRTOS_Sockets.h"

#include "inet_pton_ntop_tests.h"

#ifndef ARRAY_SIZE
	#define ARRAY_SIZE( x )		( int ) ( sizeof( x ) / sizeof( ( x )[ 0 ] ) )
#endif

typedef struct xIPV4_ADDRESS
{
	uint8_t ucParts[ ipSIZE_OF_IPv4_ADDRESS ];
	char *pcPrinted;
} IPV4_Address_t;

IPV4_Address_t ipv4_samples[] = {
	{
		{ 192, 168, 1, 1 }, "192.168.1.1"
	},
	{
		{ 127, 0, 0, 1 }, "127.0.0.1"
	},
	{
		{ 125, 210, 250, 200 }, "125.210.250.200"
	}
};

#if( ipconfigUSE_IPv6 != 0 )
	typedef struct xIPV6_ADDRESS
	{
		uint8_t ucParts[ ipSIZE_OF_IPv6_ADDRESS ];
		char *pcPrinted;
	} IPV6_Address_t;

	IPV6_Address_t ipv6_samples[] = {
		{
			{ 0xfe, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xb2, 0xa1, 0xa2, 0xff, 0xfe, 0xa3, 0xa4, 0xa5 },
			"fe80::b2a1:a2ff:fea3:a4a5"				/* test shortened zeros */
		},
		{
			{ 0xfe, 0x80, 0x00, 0x00, 0xff, 0x00, 0x00, 0x00, 0xb2, 0xa1, 0xa2, 0xff, 0xfe, 0xa3, 0xa4, 0xa5 },
			"fe80:0:ff00:0:b2a1:a2ff:fea3:a4a5"		/* don't omit single zero blocks */
		},
		{
			{ 0xfe, 0x80, 0x00, 0x00, 0xff, 0x00, 0x00, 0x00, 0xb2, 0x00, 0x00, 0x00, 0x00, 0x00, 0xa4, 0xa5 },
			"fe80:0:ff00:0:b200::a4a5" 				/* omit longest zero block */
		}
	};
#endif	/* ( ipconfigUSE_IPv6 != 0 ) */

static BaseType_t test_inet_pton4( void );
static BaseType_t test_inet_ntop4( void );

#if( ipconfigUSE_IPv6 != 0 )
	static BaseType_t test_inet_pton6( void );
	static BaseType_t test_inet_ntop6( void );
#endif	/* ( ipconfigUSE_IPv6 != 0 ) */

BaseType_t inet_pton_ntop_tests()
{
BaseType_t xResult = 0;

	xResult += test_inet_pton4();
	xResult += test_inet_ntop4();
	#if( ipconfigUSE_IPv6 != 0 )
	{
		xResult += test_inet_pton6();
		xResult += test_inet_ntop6();
	}
	#endif

	return xResult;
}

static BaseType_t test_inet_pton4( void )
{
BaseType_t xIndex;
IPV4_Address_t *pxAddress = ipv4_samples;
BaseType_t xResult;
BaseType_t xSuccessCount = 0;

	for( xIndex = 0; xIndex < ARRAY_SIZE( ipv4_samples ); xIndex++, pxAddress++ )
	{
	uint32_t ulIPAddress = 0uL;
	BaseType_t xMatch;

		FreeRTOS_inet_pton( FREERTOS_AF_INET4, pxAddress->pcPrinted, &( ulIPAddress ) );
		if( memcmp( pxAddress->ucParts, &( ulIPAddress ), ipSIZE_OF_IPv4_ADDRESS ) == 0 )
		{
			xMatch = pdTRUE;
			xSuccessCount++;
		}
		else
		{
			xMatch = pdFALSE;
			const uint8_t *puc1 = ( const uint8_t * ) pxAddress->ucParts;
			const uint8_t *puc2 = ( const uint8_t * ) &( ulIPAddress );
			FreeRTOS_printf( ( "Input  = %d:%d:%d:%d\n", puc1[0], puc1[1], puc1[2], puc1[3] ) );
			FreeRTOS_printf( ( "Output = %d:%d:%d:%d\n", puc2[0], puc2[1], puc2[2], puc2[3] ) );
		}
		FreeRTOS_printf( ( "FreeRTOS_inet_pton4: match = %d, %s = %xip\n", xMatch, pxAddress->pcPrinted, FreeRTOS_ntohl( ulIPAddress ) ) );
	}
	if( xSuccessCount == ARRAY_SIZE( ipv4_samples ) )
	{
		xResult = 1;
	}
	else
	{
		xResult = 0;
	}
	return xResult;
}
/*-----------------------------------------------------------*/

static BaseType_t test_inet_ntop4( void )
{
BaseType_t xResult;
BaseType_t xIndex;
IPV4_Address_t *pxAddress = ipv4_samples;
BaseType_t xSuccessCount = 0;

	for( xIndex = 0; xIndex < ARRAY_SIZE( ipv4_samples ); xIndex++, pxAddress++ )
	{
	char pcPrinted[ 16 ];
	BaseType_t xMatch;
	uint32_t ulIPAddress;

		pcPrinted[ 0 ] = '\0';
		memcpy( &( ulIPAddress ) , pxAddress->ucParts, sizeof ulIPAddress );
		FreeRTOS_inet_ntop( FREERTOS_AF_INET, pxAddress->ucParts, pcPrinted, sizeof( pcPrinted ) );
		if( strcmp( pxAddress->pcPrinted, pcPrinted ) == 0 )
		{
			xMatch = pdTRUE;
			xSuccessCount++;
		}
		else
		{
			xMatch = pdFALSE;
			FreeRTOS_printf( ( "Input  = %s\n", pxAddress->pcPrinted ) );
			FreeRTOS_printf( ( "Output = %s\n", pcPrinted ) );
		}
		FreeRTOS_printf( ( "FreeRTOS_inet_ntop4: match = %d, %xip = %s\n", xMatch, FreeRTOS_ntohl( ulIPAddress ), pcPrinted ) );
	}
	if( xSuccessCount == ARRAY_SIZE( ipv4_samples ) )
	{
		xResult = 1;
	}
	else
	{
		xResult = 0;
	}
	return xResult;
}
/*-----------------------------------------------------------*/

#if( ipconfigUSE_IPv6 != 0 )
	static BaseType_t test_inet_pton6( void )
	{
	BaseType_t xResult;
	BaseType_t xIndex;
	IPV6_Address_t *pxAddress = ipv6_samples;
	BaseType_t xSuccessCount = 0;

		for( xIndex = 0; xIndex < ARRAY_SIZE( ipv6_samples ); xIndex++, pxAddress++ )
		{
		uint16_t pusAddress[ 8 ];
		BaseType_t xMatch;

			memset( pusAddress, 0, sizeof pusAddress );
			FreeRTOS_inet_pton( FREERTOS_AF_INET6, pxAddress->pcPrinted, pusAddress );
			if( memcmp( pxAddress->ucParts, pusAddress, ipSIZE_OF_IPv6_ADDRESS ) == 0 )
			{
				xMatch = pdTRUE;
				xSuccessCount++;
			}
			else
			{
				xMatch = pdFALSE;
				const uint8_t *puc1 = ( const uint8_t * ) pxAddress->ucParts;
				const uint8_t *puc2 = ( const uint8_t * ) pusAddress;
				FreeRTOS_printf( ( "Input  = %02x:%02x:%02x:%02x:%02x:%02x:%02x:%02x\n",
					puc1[0], puc1[1], puc1[2], puc1[3], puc1[4], puc1[5], puc1[6], puc1[7] ) );
				FreeRTOS_printf( ( "Output = %02x:%02x:%02x:%02x:%02x:%02x:%02x:%02x\n",
					puc2[0], puc2[1], puc2[2], puc2[3], puc2[4], puc2[5], puc2[6], puc2[7] ) );
			}
			FreeRTOS_printf( ( "FreeRTOS_inet_pton6: match = %d, %s = %pip\n", xMatch, pxAddress->pcPrinted, pusAddress ) );
		}
		if( xSuccessCount == ARRAY_SIZE( ipv6_samples ) )
		{
			xResult = 1;
		}
		else
		{
			xResult = 0;
		}
		return xResult;
	}
#endif	/* ( ipconfigUSE_IPv6 != 0 ) */
/*-----------------------------------------------------------*/


#if( ipconfigUSE_IPv6 != 0 )
	static BaseType_t test_inet_ntop6( void )
	{
	BaseType_t xResult;
	BaseType_t xIndex;
	IPV6_Address_t *pxAddress = ipv6_samples;
	BaseType_t xSuccessCount = 0;

		for( xIndex = 0; xIndex < ARRAY_SIZE( ipv6_samples ); xIndex++, pxAddress++ )
		{
		char pcPrinted[ 40 ];
		BaseType_t xMatch;

			pcPrinted[ 0 ] = '\0';
			FreeRTOS_inet_ntop( FREERTOS_AF_INET6, pxAddress->ucParts, pcPrinted, sizeof( pcPrinted ) );
			if( strcmp( pxAddress->pcPrinted, pcPrinted ) == 0 )
			{
				xMatch = pdTRUE;
				xSuccessCount++;
			}
			else
			{
				xMatch = pdFALSE;
				FreeRTOS_printf( ( "Input  = %s\n", pxAddress->pcPrinted ) );
				FreeRTOS_printf( ( "Output = %s\n", pcPrinted ) );
			}
			FreeRTOS_printf( ( "FreeRTOS_inet_ntop6: match = %d, %pip = %s\n", xMatch, pxAddress->ucParts, pcPrinted ) );
		}
		if( xSuccessCount == ARRAY_SIZE( ipv6_samples ) )
		{
			xResult = 1;
		}
		else
		{
			xResult = 0;
		}
		return xResult;
	}
#endif	/* ( ipconfigUSE_IPv6 != 0 ) */
/*-----------------------------------------------------------*/

