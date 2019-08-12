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

/* FreeRTOS includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "list.h"
#include "semphr.h"

/* FreeRTOS+TCP includes. */
#include "FreeRTOS_IP.h"
#include "FreeRTOS_Sockets.h"
#include "FreeRTOS_IP_Private.h"
#include "FreeRTOS_UDP_IP.h"
#include "FreeRTOS_DNS.h"
#include "FreeRTOS_DHCP.h"
#include "NetworkBufferManagement.h"
#include "NetworkInterface.h"
#include "IPTraceMacroDefaults.h"
#include "FreeRTOS_Routing.h"

/* Exclude the entire file if DNS is not enabled. */
#if( ipconfigUSE_DNS != 0 )

#if( ipconfigBYTE_ORDER == pdFREERTOS_LITTLE_ENDIAN )
	#define dnsDNS_PORT						0x3500u
	#define dnsONE_QUESTION					0x0100u
	#define dnsOUTGOING_FLAGS				0x0001u /* Standard query. */
	#define dnsRX_FLAGS_MASK				0x0f80u /* The bits of interest in the flags field of incoming DNS messages. */
	#define dnsEXPECTED_RX_FLAGS			0x0080u /* Should be a response, without any errors. */
#else
	#define dnsDNS_PORT						0x0035u
	#define dnsONE_QUESTION					0x0001u
	#define dnsOUTGOING_FLAGS				0x0100u /* Standard query. */
	#define dnsRX_FLAGS_MASK				0x800fu /* The bits of interest in the flags field of incoming DNS messages. */
	#define dnsEXPECTED_RX_FLAGS			0x8000u /* Should be a response, without any errors. */

#endif /* ipconfigBYTE_ORDER */

/* The maximum number of times a DNS request should be sent out if a response
is not received, before giving up. */
#ifndef ipconfigDNS_REQUEST_ATTEMPTS
	#define ipconfigDNS_REQUEST_ATTEMPTS		5
#endif

/* If the top two bits in the first character of a name field are set then the
name field is an offset to the string, rather than the string itself. */
#define dnsNAME_IS_OFFSET					( ( uint8_t ) 0xc0 )

/* NBNS flags. */
#define dnsNBNS_FLAGS_RESPONSE				0x8000u
#define dnsNBNS_FLAGS_OPCODE_MASK			0x7800u
#define dnsNBNS_FLAGS_OPCODE_QUERY			0x0000u
#define dnsNBNS_FLAGS_OPCODE_REGISTRATION	0x2800u

/* Host types. */
#define dnsTYPE_A_HOST						0x0001u
#define dnsTYPE_AAAA_HOST					0x001Cu
#define dnsCLASS_IN							0x0001u

/* LLMNR constants. */
#define dnsLLMNR_TTL_VALUE					300000u
#define dnsLLMNR_FLAGS_IS_REPONSE  			0x8000u

/* NBNS constants. */
#define dnsNBNS_TTL_VALUE					3600u /* 1 hour valid */
#define dnsNBNS_TYPE_NET_BIOS				0x0020u
#define dnsNBNS_CLASS_IN					0x0001u
#define dnsNBNS_NAME_FLAGS					0x6000u
#define dnsNBNS_ENCODED_NAME_LENGTH			32

/* If the queried NBNS name matches with the device's name,
the query will be responded to with these flags: */
#define dnsNBNS_QUERY_RESPONSE_FLAGS		0x8500u

/*
 * Create a socket and bind it to the standard DNS port number.  Return the
 * the created socket - or NULL if the socket could not be created or bound.
 */
static Socket_t prvCreateDNSSocket( void );

/*
 * Create the DNS message in the zero copy buffer passed in the first parameter.
 */
static size_t prvCreateDNSMessage( uint8_t *pucUDPPayloadBuffer, const char *pcHostName, TickType_t xIdentifier );

/*
 * Simple routine that jumps over the NAME field of a resource record.
 */
static uint8_t *prvSkipNameField( uint8_t *pucByte );

/*
 * Process a response packet from a DNS server.
 */
static uint32_t prvParseDNSReply( uint8_t *pucUDPPayloadBuffer, TickType_t xIdentifier );

/*
 * Prepare and send a message to a DNS server.  'xReadTimeOut_ms' will be passed as
 * zero, in case the user has supplied a call-back function.
 */
static uint32_t prvGetHostByName( const char *pcHostName, TickType_t xIdentifier, TickType_t xReadTimeOut_ms );

/*
 * The NBNS and the LLMNR protocol share this reply function.
 */
#if( ( ipconfigUSE_NBNS == 1 ) || ( ipconfigUSE_LLMNR == 1 ) )
	static void prvReplyDNSMessage( NetworkBufferDescriptor_t *pxNetworkBuffer, BaseType_t lNetLength );
#endif

#if( ipconfigUSE_NBNS == 1 )
	static portINLINE void prvTreatNBNS( uint8_t *pucUDPPayloadBuffer, uint32_t ulIPAddress );
#endif /* ipconfigUSE_NBNS */

#if( ipconfigUSE_DNS_CACHE == 1 )
	static uint8_t *prvReadNameField( uint8_t *pucByte, char *pcName, BaseType_t xLen );
	static void prvProcessDNSCache( const char *pcName, uint32_t *pulIP, BaseType_t xLookUp );

	typedef struct xDNS_CACHE_TABLE_ROW
	{
		uint32_t ulIPAddress;		/* The IP address of an ARP cache entry. */
		char pcName[ipconfigDNS_CACHE_NAME_LENGTH];  /* The name of the host */
		uint8_t ucAge;				/* A value that is periodically decremented but can also be refreshed by active communication.  The ARP cache entry is removed if the value reaches zero. */
	} DNSCacheRow_t;

	static DNSCacheRow_t xDNSCache[ ipconfigDNS_CACHE_ENTRIES ];
#endif /* ipconfigUSE_DNS_CACHE == 1 */

#if( ipconfigUSE_LLMNR == 1 ) && ( ipconfigUSE_IPv6 != 0 )
	const IPv6_Address_t ipLLMNR_IP_ADDR_IPv6 =
	{
		ucBytes: {	/* ff02::1:3 */
			0xff, 0x02,
			0x00, 0x00,
			0x00, 0x00,
			0x00, 0x00,
			0x00, 0x00,
			0x00, 0x00,
			0x00, 0x01,
			0x00, 0x03,
		}
	};
#endif /* ipconfigUSE_IPv6 */

#if( ipconfigUSE_LLMNR == 1 )
	const MACAddress_t xLLMNR_MacAdress = { { 0x01, 0x00, 0x5e, 0x00, 0x00, 0xfc } };
#endif	/* ipconfigUSE_LLMNR == 1 */

#if( ipconfigUSE_LLMNR == 1 ) && ( ipconfigUSE_IPv6 != 0 )
	const MACAddress_t xLLMNR_MacAdressIPv6 = { { 0x33, 0x33, 0x00, 0x01, 0x00, 0x03 } };
#endif /* ipconfigUSE_LLMNR && ipconfigUSE_IPv6 */

/*-----------------------------------------------------------*/

#include "pack_struct_start.h"
struct xDNSMessage
{
	uint16_t usIdentifier;
	uint16_t usFlags;
	uint16_t usQuestions;
	uint16_t usAnswers;
	uint16_t usAuthorityRRs;
	uint16_t usAdditionalRRs;
}
#include "pack_struct_end.h"
typedef struct xDNSMessage DNSMessage_t;

/* A DNS query consists of a header, as described in 'struct xDNSMessage'
It is followed by 1 or more queries, each one consisting of a name and a tail,
with two fields: type and class
*/
#include "pack_struct_start.h"
struct xDNSTail
{
	uint16_t usType;
	uint16_t usClass;
}
#include "pack_struct_end.h"
typedef struct xDNSTail DNSTail_t;

#if( ipconfigUSE_LLMNR == 1 )

	#include "pack_struct_start.h"
	struct xLLMNRAnswer
	{
		uint8_t ucNameCode;
		uint8_t ucNameOffset;	/* The name is not repeated in the answer, only the offset is given with "0xc0 <offs>" */
		uint16_t usType;
		uint16_t usClass;
		uint32_t ulTTL;
		uint16_t usDataLength;
		uint32_t ulIPAddress;
	}
	#include "pack_struct_end.h"
	typedef struct xLLMNRAnswer LLMNRAnswer_t;

#endif /* ipconfigUSE_LLMNR == 1 */

#if( ipconfigUSE_NBNS == 1 )

	#include "pack_struct_start.h"
	struct xNBNSRequest
	{
		uint16_t usRequestId;
		uint16_t usFlags;
		uint16_t ulRequestCount;
		uint16_t usAnswerRSS;
		uint16_t usAuthRSS;
		uint16_t usAdditionalRSS;
		uint8_t ucNameSpace;
		uint8_t ucName[ dnsNBNS_ENCODED_NAME_LENGTH ];
		uint8_t ucNameZero;
		uint16_t usType;
		uint16_t usClass;
	}
	#include "pack_struct_end.h"
	typedef struct xNBNSRequest NBNSRequest_t;

	#include "pack_struct_start.h"
	struct xNBNSAnswer
	{
		uint16_t usType;
		uint16_t usClass;
		uint32_t ulTTL;
		uint16_t usDataLength;
		uint16_t usNbFlags;		/* NetBIOS flags 0x6000 : IP-address, big-endian */
		uint32_t ulIPAddress;
	}
	#include "pack_struct_end.h"
	typedef struct xNBNSAnswer NBNSAnswer_t;

#endif /* ipconfigUSE_NBNS == 1 */

/*-----------------------------------------------------------*/

#if( ipconfigUSE_DNS_CACHE == 1 )
	uint32_t FreeRTOS_dnslookup( const char *pcHostName )
	{
	uint32_t ulIPAddress = 0UL;
		prvProcessDNSCache( pcHostName, &ulIPAddress, pdTRUE );
		return ulIPAddress;
	}
#endif /* ipconfigUSE_DNS_CACHE == 1 */
/*-----------------------------------------------------------*/

#if( ipconfigDNS_USE_CALLBACKS != 0 )

	typedef struct xDNS_Callback {
		TickType_t xRemaningTime;		/* Timeout in ms */
		FOnDNSEvent pCallbackFunction;	/* Function to be called when the address has been found or when a timeout has beeen reached */
		TimeOut_t xTimeoutState;
		void *pvSearchID;
		struct xLIST_ITEM xListItem;
		char pcName[ 1 ];
	} DNSCallback_t;

	static List_t xCallbackList;

	/* Define FreeRTOS_gethostbyname() as a normal blocking call. */
	uint32_t FreeRTOS_gethostbyname( const char *pcHostName )
	{
		return FreeRTOS_gethostbyname_a( pcHostName, ( FOnDNSEvent ) NULL, ( void* )NULL, 0 );
	}
	/*-----------------------------------------------------------*/

	/* Initialise the list of call-back structures. */
	void vDNSInitialise( void );
	void vDNSInitialise( void )
	{
		vListInitialise( &xCallbackList );
	}
	/*-----------------------------------------------------------*/

	/* Iterate through the list of call-back structures and remove
	old entries which have reached a timeout.
	As soon as the list hase become empty, the DNS timer will be stopped
	In case pvSearchID is supplied, the user wants to cancel a DNS request
	*/
	void vDNSCheckCallBack( void *pvSearchID );
	void vDNSCheckCallBack( void *pvSearchID )
	{
	const ListItem_t *pxIterator;
	const MiniListItem_t* xEnd = ( const MiniListItem_t* )listGET_END_MARKER( &xCallbackList );

		vTaskSuspendAll();
		{
			for( pxIterator  = ( const ListItem_t * ) listGET_NEXT( xEnd );
				 pxIterator != ( const ListItem_t * ) xEnd;
				  )
			{
				DNSCallback_t *pxCallback = ( DNSCallback_t * ) listGET_LIST_ITEM_OWNER( pxIterator );
				/* Move to the next item because we might remove this item */
				pxIterator  = ( const ListItem_t * ) listGET_NEXT( pxIterator );
				if( ( pvSearchID != NULL ) && ( pvSearchID == pxCallback->pvSearchID ) )
				{
					uxListRemove( &pxCallback->xListItem );
					vPortFree( pxCallback );
				}
				else if( xTaskCheckForTimeOut( &pxCallback->xTimeoutState, &pxCallback->xRemaningTime ) != pdFALSE )
				{
					pxCallback->pCallbackFunction( pxCallback->pcName, pxCallback->pvSearchID, 0 );
					uxListRemove( &pxCallback->xListItem );
					vPortFree( ( void * ) pxCallback );
				}
			}
		}
		xTaskResumeAll();

		if( listLIST_IS_EMPTY( &xCallbackList ) )
		{
			vIPSetDnsTimerEnableState( pdFALSE );
		}
	}
	/*-----------------------------------------------------------*/

	void FreeRTOS_gethostbyname_cancel( void *pvSearchID )
	{
		/* _HT_ Should better become a new API call to have the IP-task remove the callback */
		vDNSCheckCallBack( pvSearchID );
	}
	/*-----------------------------------------------------------*/

	/* FreeRTOS_gethostbyname_a() was called along with callback parameters.
	Store them in a list for later reference. */
	static void vDNSSetCallBack( const char *pcHostName, void *pvSearchID, FOnDNSEvent pCallbackFunction, TickType_t xTimeout, TickType_t xIdentifier );
	static void vDNSSetCallBack( const char *pcHostName, void *pvSearchID, FOnDNSEvent pCallbackFunction, TickType_t xTimeout, TickType_t xIdentifier )
	{
		size_t lLength = strlen( pcHostName );
		DNSCallback_t *pxCallback = ( DNSCallback_t * )pvPortMalloc( sizeof( *pxCallback ) + lLength );

		/* Translate from ms to number of clock ticks. */
		xTimeout /= portTICK_PERIOD_MS;
		if( pxCallback != NULL )
		{
			if( listLIST_IS_EMPTY( &xCallbackList ) )
			{
				/* This is the first one, start the DNS timer to check for timeouts */
				vIPReloadDNSTimer( FreeRTOS_min_uint32( 1000U, xTimeout ) );
			}
			strcpy( pxCallback->pcName, pcHostName );
			pxCallback->pCallbackFunction = pCallbackFunction;
			pxCallback->pvSearchID = pvSearchID;
			pxCallback->xRemaningTime = xTimeout;
			vTaskSetTimeOutState( &pxCallback->xTimeoutState );
			listSET_LIST_ITEM_OWNER( &( pxCallback->xListItem ), ( void* ) pxCallback );
			listSET_LIST_ITEM_VALUE( &( pxCallback->xListItem ), xIdentifier );
			vTaskSuspendAll();
			{
				vListInsertEnd( &xCallbackList, &pxCallback->xListItem );
			}
			xTaskResumeAll();
		}
	}
	/*-----------------------------------------------------------*/

	/* A DNS reply was received, see if there is any matching entry and
	call the handler. */
	static void vDNSDoCallback( TickType_t xIdentifier, const char *pcName, uint32_t ulIPAddress );
	static void vDNSDoCallback( TickType_t xIdentifier, const char *pcName, uint32_t ulIPAddress )
	{
		const ListItem_t *pxIterator;
		const MiniListItem_t* xEnd = ( const MiniListItem_t* )listGET_END_MARKER( &xCallbackList );

		vTaskSuspendAll();
		{
			for( pxIterator  = ( const ListItem_t * ) listGET_NEXT( xEnd );
				 pxIterator != ( const ListItem_t * ) xEnd;
				 pxIterator  = ( const ListItem_t * ) listGET_NEXT( pxIterator ) )
			{
				if( listGET_LIST_ITEM_VALUE( pxIterator ) == xIdentifier )
				{
					DNSCallback_t *pxCallback = ( DNSCallback_t * ) listGET_LIST_ITEM_OWNER( pxIterator );
					pxCallback->pCallbackFunction( pcName, pxCallback->pvSearchID, ulIPAddress );
					uxListRemove( &pxCallback->xListItem );
					vPortFree( pxCallback );
					if( listLIST_IS_EMPTY( &xCallbackList ) )
					{
						vIPSetDnsTimerEnableState( pdFALSE );
					}
					break;
				}
			}
		}
		xTaskResumeAll();
	}

#endif	/* ipconfigDNS_USE_CALLBACKS != 0 */
/*-----------------------------------------------------------*/
/*_RB_ How to determine which interface to perform the DNS look up on? */
#if( ipconfigDNS_USE_CALLBACKS == 0 )
uint32_t FreeRTOS_gethostbyname( const char *pcHostName )
#else
uint32_t FreeRTOS_gethostbyname_a( const char *pcHostName, FOnDNSEvent pCallback, void *pvSearchID, TickType_t xTimeout )
#endif
{
uint32_t ulIPAddress = 0UL;
TickType_t xReadTimeOut_ms = 1200U;
/* Generate a unique identifier for this query. Keep it in a local variable
 as gethostbyname() may be called from different threads */
TickType_t xIdentifier = 0;

	/* If a DNS cache is used then check the cache before issuing another DNS
	request. */
	#if( ipconfigUSE_DNS_CACHE == 1 )
	{
		if( ulIPAddress == 0UL )
		{
			ulIPAddress = FreeRTOS_dnslookup( pcHostName );
			if( ulIPAddress != 0 )
			{
				FreeRTOS_debug_printf( ( "FreeRTOS_gethostbyname: found '%s' in cache: %lxip\n", pcHostName, ulIPAddress ) );
			}
			else
			{
				/* prvGetHostByName will be called to start a DNS lookup. */
			}
		}
	}
	#endif /* ipconfigUSE_DNS_CACHE == 1 */

	/* Generate a unique identifier. */
	if( 0 == ulIPAddress )
	{
		/* DNS identifiers are 16-bit. */
		xIdentifier = ( TickType_t )( ipconfigRAND32( ) & 0xffff );
	}

	#if( ipconfigDNS_USE_CALLBACKS != 0 )
	{
		if( pCallback != NULL )
		{
			if( ulIPAddress == 0UL )
			{
				/* The user has provided a callback function, so do not block on recvfrom() */
				if( 0 != xIdentifier )
				{
					xReadTimeOut_ms  = 0;
					vDNSSetCallBack( pcHostName, pvSearchID, pCallback, xTimeout, ( TickType_t ) xIdentifier );
				}
			}
			else
			{
				/* The IP address is known, do the call-back now. */
				pCallback( pcHostName, pvSearchID, ulIPAddress );
			}
		}
	}
	#endif

	if( ( ulIPAddress == 0UL ) && ( 0 != xIdentifier ) )
	{
		ulIPAddress = prvGetHostByName( pcHostName, xIdentifier, xReadTimeOut_ms );
	}

	return ulIPAddress;
}
/*-----------------------------------------------------------*/

static uint32_t prvGetHostByName( const char *pcHostName, TickType_t xIdentifier, TickType_t xReadTimeOut_ms )
{
struct freertos_sockaddr xAddress;
Socket_t xDNSSocket;
uint32_t ulIPAddress = 0UL;
uint8_t *pucUDPPayloadBuffer;
static uint32_t ulAddressLength;
BaseType_t xAttempt;
int32_t lBytes;
size_t xPayloadLength, xExpectedPayloadLength;
TickType_t xWriteTimeOut_ms = 100U;

#if( ipconfigUSE_LLMNR == 1 )
	BaseType_t bHasDot = pdFALSE;
#endif /* ipconfigUSE_LLMNR == 1 */

	/* If LLMNR is being used then determine if the host name includes a '.' -
	if not then LLMNR can be used as the lookup method. */
	#if( ipconfigUSE_LLMNR == 1 )
	{
		const char *pucPtr;
		for( pucPtr = pcHostName; *pucPtr; pucPtr++ )
		{
			if( *pucPtr == '.' )
			{
				bHasDot = pdTRUE;
				break;
			}
		}
	}
	#endif /* ipconfigUSE_LLMNR == 1 */

	/* Two is added at the end for the count of characters in the first
	subdomain part and the string end byte. */
	xExpectedPayloadLength = sizeof( DNSMessage_t ) + strlen( pcHostName ) + sizeof( uint16_t ) + sizeof( uint16_t ) + 2u;

	xDNSSocket = prvCreateDNSSocket();

	if( xDNSSocket != NULL )
	{
		FreeRTOS_setsockopt( xDNSSocket, 0, FREERTOS_SO_SNDTIMEO, ( void * ) &xWriteTimeOut_ms, sizeof( TickType_t ) );
		FreeRTOS_setsockopt( xDNSSocket, 0, FREERTOS_SO_RCVTIMEO, ( void * ) &xReadTimeOut_ms,  sizeof( TickType_t ) );

		for( xAttempt = 0; xAttempt < ipconfigDNS_REQUEST_ATTEMPTS; xAttempt++ )
		{
			/* Get a buffer.  This uses a maximum delay, but the delay will be
			capped to ipconfigUDP_MAX_SEND_BLOCK_TIME_TICKS so the return value
			still needs to be tested. */
			#if( ipconfigUSE_IPv6 != 0 )
			{
				pucUDPPayloadBuffer = ( uint8_t * ) FreeRTOS_GetUDPPayloadBuffer( xExpectedPayloadLength, portMAX_DELAY, ipTYPE_IPv4 );
			}
			#else
			{
				pucUDPPayloadBuffer = ( uint8_t * ) FreeRTOS_GetUDPPayloadBuffer( xExpectedPayloadLength, portMAX_DELAY );
			}
			#endif
			if( pucUDPPayloadBuffer != NULL )
			{

				/* Create the message in the obtained buffer. */
				xPayloadLength = prvCreateDNSMessage( pucUDPPayloadBuffer, pcHostName, xIdentifier );

				iptraceSENDING_DNS_REQUEST();

				/* Send the DNS message. */
#if( ipconfigUSE_LLMNR == 1 )
				if( bHasDot == pdFALSE )
				{
					/* Use LLMNR addressing. */
					( ( DNSMessage_t * ) pucUDPPayloadBuffer) -> usFlags = 0;
					xAddress.sin_addr = ipLLMNR_IP_ADDR;	/* Is in network byte order. */
					xAddress.sin_port = FreeRTOS_ntohs( ipLLMNR_PORT );
				}
				else
#endif
				{
				NetworkEndPoint_t *pxEndPoint;

					ulIPAddress = 0ul;
					/* Obtain the DNS server address. */
					for( pxEndPoint = FreeRTOS_FirstEndPoint( NULL );
						pxEndPoint != NULL;
						pxEndPoint = FreeRTOS_NextEndPoint( NULL, pxEndPoint ) )
					{
						FreeRTOS_GetAddressConfiguration( pxEndPoint, NULL, NULL, NULL, &ulIPAddress );
						if( ( ulIPAddress != ( uint32_t )0u ) && ( ulIPAddress != ~( uint32_t )0u ) )
						{
							break;
						}
					}
					/* Use DNS server. */
					xAddress.sin_addr = ulIPAddress;
					xAddress.sin_port = dnsDNS_PORT;
				}

				ulIPAddress = 0UL;

				if( FreeRTOS_sendto( xDNSSocket, pucUDPPayloadBuffer, xPayloadLength, FREERTOS_ZERO_COPY, &xAddress, sizeof( xAddress ) ) != 0 )
				{
					/* Wait for the reply. */
					lBytes = FreeRTOS_recvfrom( xDNSSocket, &pucUDPPayloadBuffer, 0, FREERTOS_ZERO_COPY, &xAddress, &ulAddressLength );

					if( lBytes > 0 )
					{
						/* The reply was received.  Process it. */
						ulIPAddress = prvParseDNSReply( pucUDPPayloadBuffer, xIdentifier );

						/* Finished with the buffer.  The zero copy interface
						is being used, so the buffer must be freed by the
						task. */
						FreeRTOS_ReleaseUDPPayloadBuffer( ( void * ) pucUDPPayloadBuffer );

						if( ulIPAddress != 0UL )
						{
							/* All done. */
							break;
						}
					}
				}
				else
				{
					/* The message was not sent so the stack will not be
					releasing the zero copy - it must be released here. */
					FreeRTOS_ReleaseUDPPayloadBuffer( ( void * ) pucUDPPayloadBuffer );
				}
			}
		}

		/* Finished with the socket. */
		FreeRTOS_closesocket( xDNSSocket );
	}

	return ulIPAddress;
}
/*-----------------------------------------------------------*/

static size_t prvCreateDNSMessage( uint8_t *pucUDPPayloadBuffer, const char *pcHostName, TickType_t xIdentifier )
{
DNSMessage_t *pxDNSMessageHeader;
uint8_t *pucStart, *pucByte;
DNSTail_t *pxTail;
static const DNSMessage_t xDefaultPartDNSHeader =
{
	0,					/* The identifier will be overwritten. */
	dnsOUTGOING_FLAGS,	/* Flags set for standard query. */
	dnsONE_QUESTION,	/* One question is being asked. */
	0,					/* No replies are included. */
	0,					/* No authorities. */
	0					/* No additional authorities. */
};

	/* Copy in the const part of the header. */
	memcpy( ( void * ) pucUDPPayloadBuffer, ( void * ) &xDefaultPartDNSHeader, sizeof( xDefaultPartDNSHeader ) );

	/* Write in a unique identifier. */
	pxDNSMessageHeader = ( DNSMessage_t * ) pucUDPPayloadBuffer;
	pxDNSMessageHeader->usIdentifier = ( uint16_t ) xIdentifier;

	/* Create the resource record at the end of the header.  First
	find the end of the header. */
	pucStart = pucUDPPayloadBuffer + sizeof( xDefaultPartDNSHeader );

	/* Leave a gap for the first length bytes. */
	pucByte = pucStart + 1;

	/* Copy in the host name. */
	strcpy( ( char * ) pucByte, pcHostName );

	/* Mark the end of the string. */
	pucByte += strlen( pcHostName );
	*pucByte = 0x00u;

	/* Walk the string to replace the '.' characters with byte counts.
	pucStart holds the address of the byte count.  Walking the string
	starts after the byte count position. */
	pucByte = pucStart;

	do
	{
		pucByte++;

		while( ( *pucByte != 0x00 ) && ( *pucByte != '.' ) )
		{
			pucByte++;
		}

		/* Fill in the byte count, then move the pucStart pointer up to
		the found byte position. */
		*pucStart = ( uint8_t ) ( ( uint32_t ) pucByte - ( uint32_t ) pucStart );
		( *pucStart )--;

		pucStart = pucByte;

	} while( *pucByte != 0x00 );

	/* Finish off the record. */

	pxTail = (DNSTail_t *)( pucByte + 1 );

	vSetField16( pxTail, DNSTail_t, usType, dnsTYPE_A_HOST );	/* Type A: host */
	vSetField16( pxTail, DNSTail_t, usClass, dnsCLASS_IN );	/* 1: Class IN */

	/* Return the total size of the generated message, which is the space from
	the last written byte to the beginning of the buffer. */
	return ( ( uint32_t ) pucByte - ( uint32_t ) pucUDPPayloadBuffer + 1 ) + sizeof( *pxTail );
}
/*-----------------------------------------------------------*/

#if( ipconfigUSE_DNS_CACHE == 1 )

	static uint8_t *prvReadNameField( uint8_t *pucByte, char *pcName, BaseType_t xLen )
	{
	BaseType_t xNameLen = 0;
		/* Determine if the name is the fully coded name, or an offset to the name
		elsewhere in the message. */
		if( ( *pucByte & dnsNAME_IS_OFFSET ) == dnsNAME_IS_OFFSET )
		{
			/* Jump over the two byte offset. */
			pucByte += sizeof( uint16_t );

		}
		else
		{
			/* pucByte points to the full name.  Walk over the string. */
			while( *pucByte != 0x00 )
			{
				BaseType_t xCount;
				if( xNameLen && xNameLen < xLen - 1 )
					pcName[xNameLen++] = '.';
				for( xCount = *(pucByte++); xCount--; pucByte++ )
				{
					if( xNameLen < xLen - 1 )
						pcName[xNameLen++] = *( ( char * ) pucByte );
				}
			}

			pucByte++;
		}

		return pucByte;
	}
#endif	/* ipconfigUSE_DNS_CACHE == 1 */
/*-----------------------------------------------------------*/

static uint8_t *prvSkipNameField( uint8_t *pucByte )
{
	/* Determine if the name is the fully coded name, or an offset to the name
	elsewhere in the message. */
	if( ( *pucByte & dnsNAME_IS_OFFSET ) == dnsNAME_IS_OFFSET )
	{
		/* Jump over the two byte offset. */
		pucByte += sizeof( uint16_t );

	}
	else
	{
		/* pucByte points to the full name.  Walk over the string. */
		while( *pucByte != 0x00 )
		{
			/* The number of bytes to jump for each name section is stored in the byte
			before the name section. */
			pucByte += ( *pucByte + 1 );
		}

		pucByte++;
	}

	return pucByte;
}
/*-----------------------------------------------------------*/

uint32_t ulDNSHandlePacket( NetworkBufferDescriptor_t *pxNetworkBuffer )
{
uint8_t *pucUDPPayloadBuffer = pxNetworkBuffer->pucEthernetBuffer + ipSIZE_OF_ETH_HEADER + xIPHeaderSize( pxNetworkBuffer ) + ipSIZE_OF_UDP_HEADER;
DNSMessage_t *pxDNSMessageHeader = ( DNSMessage_t * ) pucUDPPayloadBuffer;

	prvParseDNSReply( pucUDPPayloadBuffer, ( uint32_t ) pxDNSMessageHeader->usIdentifier );

	/* The packet was not consumed. */
	return pdFAIL;
}
/*-----------------------------------------------------------*/

#if( ipconfigUSE_NBNS == 1 )

	uint32_t ulNBNSHandlePacket (NetworkBufferDescriptor_t *pxNetworkBuffer )
	{
	UDPPacket_t *pxUDPPacket = ( UDPPacket_t * ) pxNetworkBuffer->pucEthernetBuffer;
	uint8_t *pucUDPPayloadBuffer = pxNetworkBuffer->pucEthernetBuffer + sizeof( *pxUDPPacket );

		prvTreatNBNS( pucUDPPayloadBuffer, pxUDPPacket->xIPHeader.ulSourceIPAddress );

		/* The packet was not consumed. */
		return pdFAIL;
	}

#endif /* ipconfigUSE_NBNS */
/*-----------------------------------------------------------*/

static NetworkEndPoint_t *prvFindEndPointOnNetMask( NetworkBufferDescriptor_t *pxNetworkBuffer )
{
NetworkEndPoint_t *pxEndPoint;

#if( ipconfigUSE_IPv6 != 0 )
	IPPacket_IPv6_t *xIPPacket_IPv6 = ( IPPacket_IPv6_t * ) pxNetworkBuffer->pucEthernetBuffer;

	if( xIPPacket_IPv6->xEthernetHeader.usFrameType == ipIPv6_FRAME_TYPE )
	{
		pxEndPoint = FreeRTOS_FindEndPointOnNetMask_IPv6( &xIPPacket_IPv6->xIPHeader_IPv6.xSourceIPv6Address );
	}
	else					
#endif
	{
	IPPacket_t *xIPPacket = ( IPPacket_t * ) pxNetworkBuffer->pucEthernetBuffer;

		pxEndPoint = FreeRTOS_FindEndPointOnNetMask( xIPPacket->xIPHeader.ulSourceIPAddress, 6 );
	}

	if( pxEndPoint != NULL )
	{
		pxNetworkBuffer->pxEndPoint = pxEndPoint;
	}

	return pxEndPoint;
}
/*-----------------------------------------------------------*/

static uint32_t prvParseDNSReply( uint8_t *pucUDPPayloadBuffer, TickType_t xIdentifier )
{
DNSMessage_t *pxDNSMessageHeader;
uint32_t ulIPAddress = 0UL;
#if( ipconfigUSE_LLMNR == 1 )
	char *pcRequestedName = NULL;
#endif
uint8_t *pucByte;
uint16_t x, usDataLength, usQuestions;
#if( ipconfigUSE_LLMNR == 1 )
	uint16_t usType = 0, usClass = 0;
#endif
#if( ipconfigUSE_DNS_CACHE == 1 )
	char pcName[ 128 ] = ""; /*_RB_ What is the significance of 128?  Probably too big to go on the stack for a small MCU but don't know how else it could be made re-entrant.  Might be necessary. */
#endif

	pxDNSMessageHeader = ( DNSMessage_t * ) pucUDPPayloadBuffer;

	/* Introduce a do {} while (0) to allow the use of breaks. */
	do
	{
		if( pxDNSMessageHeader->usIdentifier != ( uint16_t ) xIdentifier )
		{
			break;
		}
		/* Start at the first byte after the header. */
		pucByte = pucUDPPayloadBuffer + sizeof( DNSMessage_t );

		/* Skip any question records. */
		usQuestions = FreeRTOS_ntohs( pxDNSMessageHeader->usQuestions );
		for( x = 0; x < usQuestions; x++ )
		{
			#if( ipconfigUSE_LLMNR == 1 )
			{
				if( x == 0 )
				{
					pcRequestedName = ( char * ) pucByte;
				}
			}
			#endif

#if( ipconfigUSE_DNS_CACHE == 1 )
			if( x == 0 )
			{
				pucByte = prvReadNameField( pucByte, pcName, sizeof( pcName ) );
			}
			else
#endif /* ipconfigUSE_DNS_CACHE */
			{
				/* Skip the variable length pcName field. */
				pucByte = prvSkipNameField( pucByte );
			}

			#if( ipconfigUSE_LLMNR == 1 )
			{
				/* usChar2u16 returns value in host endianness. */
				usType = usChar2u16( pucByte );
				usClass = usChar2u16( pucByte + 2 );
			}
			#endif /* ipconfigUSE_LLMNR */

			/* Skip the type and class fields. */
			pucByte += sizeof( uint32_t );
		}

		/* Search through the answers records. */
		pxDNSMessageHeader->usAnswers = FreeRTOS_ntohs( pxDNSMessageHeader->usAnswers );

		if( ( pxDNSMessageHeader->usFlags & dnsRX_FLAGS_MASK ) == dnsEXPECTED_RX_FLAGS )
		{
			for( x = 0; x < pxDNSMessageHeader->usAnswers; x++ )
			{
				pucByte = prvSkipNameField( pucByte );

				/* Is the type field that of an A record? */
				if( usChar2u16( pucByte ) == dnsTYPE_A_HOST )
				{
					/* This is the required record.  Skip the type, class, and
					time to live fields, plus the first byte of the data
					length. */
					pucByte += ( sizeof( uint32_t ) + sizeof( uint32_t ) + sizeof( uint8_t ) );

					/* Sanity check the data length. */
					if( ( size_t ) *pucByte == sizeof( uint32_t ) )
					{
						/* Skip the second byte of the length. */
						pucByte++;

						/* Copy the IP address out of the record. */
						memcpy( ( void * ) &ulIPAddress, ( void * ) pucByte, sizeof( uint32_t ) );

						#if( ipconfigUSE_DNS_CACHE == 1 )
						{
							prvProcessDNSCache( pcName, &ulIPAddress, pdFALSE );
						}
						#endif /* ipconfigUSE_DNS_CACHE */
						#if( ipconfigDNS_USE_CALLBACKS != 0 )
						{
							/* See if any asynchronous call was made to FreeRTOS_gethostbyname_a() */
							vDNSDoCallback( ( TickType_t ) pxDNSMessageHeader->usIdentifier, pcName, ulIPAddress );
						}
						#endif	/* ipconfigDNS_USE_CALLBACKS != 0 */
					}

					break;
				}
				else
				{
					/* Skip the type, class and time to live fields. */
					pucByte += ( sizeof( uint32_t ) + sizeof( uint32_t ) );

					/* Determine the length of the data in the field. */
					memcpy( ( void * ) &usDataLength, ( void * ) pucByte, sizeof( uint16_t ) );
					usDataLength = FreeRTOS_ntohs( usDataLength );

					/* Jump over the data length bytes, and the data itself. */
					pucByte += usDataLength + sizeof( uint16_t );
				}
			}
		} /* if( ( pxDNSMessageHeader->usFlags & dnsRX_FLAGS_MASK ) == dnsEXPECTED_RX_FLAGS ) */

#if( ipconfigUSE_LLMNR == 1 )
		else if( ( usQuestions != ( uint16_t )0u ) && ( ( usType == ( uint16_t )dnsTYPE_A_HOST ) || ( usType == dnsTYPE_AAAA_HOST ) ) && ( usClass == ( uint16_t )dnsCLASS_IN ) )
		{
		/* Find the descriptor located before the payload in the buffer. */
		NetworkBufferDescriptor_t *pxNetworkBuffer;
		NetworkEndPoint_t *pxEndPoint, xEndPoint;
		int16_t usLength;
		LLMNRAnswer_t *pxAnswer;
		BaseType_t xDataLength;
		BaseType_t xExtraLength;
		size_t uxOffsets[ 3 ];

			#if( ipconfigUSE_IPv6 == 0 )
			if( usType != dnsTYPE_A_HOST )
			{
				/* Only allow IPv4 format. */
				break;
			}
			#endif /* ipconfigUSE_IPv6 */

			pxNetworkBuffer = pxUDPPayloadBuffer_to_NetworkBuffer( pucUDPPayloadBuffer );
			/* This test could be replaced with a assert(). */
			if( pxNetworkBuffer == NULL )
			{
				break;
			}
			if( pxNetworkBuffer->pxEndPoint == NULL )
			{
				/* NetworkInterface is obliged to set 'pxEndPoint' in every received packet,
				but in case this has not be done, set it here. */
FreeRTOS_printf( ( "prvParseDNSReply: No pxEndPoint yet???\n" ) );
				pxNetworkBuffer->pxEndPoint = prvFindEndPointOnNetMask( pxNetworkBuffer );
				if( pxNetworkBuffer->pxEndPoint == NULL )
				{
					break;
				}
			}
			pxEndPoint = pxNetworkBuffer->pxEndPoint;//prvFindEndPointOnNetMask( pxNetworkBuffer );

			/* Make a copy of the end-point because xApplicationDNSQueryHook() is allowed
			to write into it. */
			memcpy( &xEndPoint, pxEndPoint, sizeof( xEndPoint ) );
			#if( ipconfigUSE_IPv6 != 0 )
			{
				FreeRTOS_printf( ( "prvParseDNSReply: type %04X\n", usType ) );
				if( usType == dnsTYPE_AAAA_HOST )
				{
					xEndPoint.bits.bIPv6 = pdTRUE_UNSIGNED;
				}
				else
				{
					xEndPoint.bits.bIPv6 = pdFALSE_UNSIGNED;
				}
			}
			#endif

			if( xApplicationDNSQueryHook ( &xEndPoint, pcRequestedName + 1 ) == pdFALSE )
			{
				/* This LLMNR request needs no reply from this unit. */
				break;
			}

			xDataLength = sizeof( EthernetHeader_t ) + xIPHeaderSize( pxNetworkBuffer ) + sizeof( UDPHeader_t ) + pxNetworkBuffer->xDataLength;

			#if( ipconfigUSE_IPv6 != 0 )
			if( usType == dnsTYPE_AAAA_HOST )
			{
				xExtraLength = sizeof( LLMNRAnswer_t ) - sizeof( pxAnswer->ulIPAddress ) + ipSIZE_OF_IPv6_ADDRESS;
			}
			else
			#endif
			{
				xExtraLength = sizeof( LLMNRAnswer_t );
			}

			/* The field xDataLength was set to the length of the UDP
			payload.  The answer (reply) will be longer than the
			request, so the packet must be resized. */
			uxOffsets[ 0 ] = ( size_t ) ( pucUDPPayloadBuffer - pxNetworkBuffer->pucEthernetBuffer );
			uxOffsets[ 1 ] = ( size_t ) ( pcRequestedName - ( ( char * ) pxNetworkBuffer->pucEthernetBuffer ) );
			uxOffsets[ 2 ] = ( size_t ) ( pucByte - pxNetworkBuffer->pucEthernetBuffer );

			/* Restore the 'xDataLength' field. */
			pxNetworkBuffer->xDataLength = xDataLength;
			pxNetworkBuffer = pxResizeNetworkBufferWithDescriptor( pxNetworkBuffer, xDataLength + xExtraLength );

			if( pxNetworkBuffer == NULL )
			{
				break;
			}
			pucUDPPayloadBuffer = pxNetworkBuffer->pucEthernetBuffer + uxOffsets[ 0 ];
			pcRequestedName = ( char * ) ( pxNetworkBuffer->pucEthernetBuffer + uxOffsets[ 1 ] );
			pucByte = pxNetworkBuffer->pucEthernetBuffer + uxOffsets[ 2 ];
			pxDNSMessageHeader = ( DNSMessage_t * ) pucUDPPayloadBuffer;

			pxAnswer = ( LLMNRAnswer_t * ) pucByte;

			/* Leave 'usIdentifier' and 'usQuestions' untouched. */
			vSetField16( pxDNSMessageHeader, DNSMessage_t, usFlags, dnsLLMNR_FLAGS_IS_REPONSE );	/* Set the response flag */
			vSetField16( pxDNSMessageHeader, DNSMessage_t, usAnswers, 1 );	/* Provide a single answer */
			vSetField16( pxDNSMessageHeader, DNSMessage_t, usAuthorityRRs, 0 );	/* No authority */
			vSetField16( pxDNSMessageHeader, DNSMessage_t, usAdditionalRRs, 0 );	/* No additional info */

			pxAnswer->ucNameCode = dnsNAME_IS_OFFSET;
			pxAnswer->ucNameOffset = ( uint8_t )( pcRequestedName - ( char * ) pucUDPPayloadBuffer );

			vSetField16( pxAnswer, LLMNRAnswer_t, usType, usType );	/* Type A: host */
			vSetField16( pxAnswer, LLMNRAnswer_t, usClass, dnsCLASS_IN );	/* 1: Class IN */
			vSetField32( pxAnswer, LLMNRAnswer_t, ulTTL, dnsLLMNR_TTL_VALUE );
			#if( ipconfigUSE_IPv6 != 0 )
			if( usType == dnsTYPE_AAAA_HOST )
			{
				vSetField16( pxAnswer, LLMNRAnswer_t, usDataLength, ipSIZE_OF_IPv6_ADDRESS );
				memcpy( &( pxAnswer->ulIPAddress ), pxEndPoint->ulIPAddress_IPv6.ucBytes, ipSIZE_OF_IPv6_ADDRESS );
FreeRTOS_printf( ( "LLMNR return IPv6 %pip\n", pxEndPoint->ulIPAddress_IPv6.ucBytes ) );
				usLength = ( int16_t ) ( sizeof( *pxAnswer ) + ( size_t ) ( pucByte - pucUDPPayloadBuffer ) -
						   sizeof( pxAnswer->ulIPAddress ) + ipSIZE_OF_IPv6_ADDRESS );
			}
			else
			#endif
			{
FreeRTOS_printf( ( "LLMNR return IPv4 %xip\n", FreeRTOS_ntohl( xEndPoint.ulIPAddress ) ) );
				vSetField16( pxAnswer, LLMNRAnswer_t, usDataLength, sizeof( pxAnswer->ulIPAddress ) );
				//vSetField32( pxAnswer, LLMNRAnswer_t, ulIPAddress, FreeRTOS_ntohl( pxEndPoint->ulIPAddress ) );
				vSetField32( pxAnswer, LLMNRAnswer_t, ulIPAddress, FreeRTOS_ntohl( xEndPoint.ulIPAddress ) );
				
				usLength = ( int16_t ) ( sizeof( *pxAnswer ) + ( size_t ) ( pucByte - pucUDPPayloadBuffer ) );
			}
			#if( ipconfigUSE_IPv6 == 0 )
			if( usType == dnsTYPE_A_HOST )
			#else
			if( ( usType == dnsTYPE_A_HOST ) || ( usType == dnsTYPE_AAAA_HOST ) )
			#endif /* ipconfigUSE_IPv6 */
			{
				prvReplyDNSMessage( pxNetworkBuffer, usLength );
			}
		}
#endif /* ipconfigUSE_LLMNR == 1 */
	} while( 0 );

	return ulIPAddress;
}
/*-----------------------------------------------------------*/

#if( ipconfigUSE_NBNS == 1 ) /*_RB_ Not yet converted to multi interfaces. */

	static void prvTreatNBNS( uint8_t *pucUDPPayloadBuffer, uint32_t ulIPAddress )
	{
	uint16_t usFlags, usType, usClass;
	uint8_t *pucSource, *pucTarget;
	uint8_t ucByte;
	uint8_t ucNBNSName[ 17 ];
	NetworkBufferDescriptor_t *pxNetworkBuffer;

		usFlags = usChar2u16( pucUDPPayloadBuffer + offsetof( NBNSRequest_t, usFlags ) );

		if( ( usFlags & dnsNBNS_FLAGS_OPCODE_MASK ) == dnsNBNS_FLAGS_OPCODE_QUERY )
		{
		NetworkEndPoint_t xEndPoint;
			usType  = usChar2u16( pucUDPPayloadBuffer + offsetof( NBNSRequest_t, usType ) );
			usClass = usChar2u16( pucUDPPayloadBuffer + offsetof( NBNSRequest_t, usClass ) );

			/* Not used for now */
			( void )usClass;
			/* For NBNS a name is 16 bytes long, written with capitals only.
			Make sure that the copy is terminated with a zero. */
			pucTarget = ucNBNSName + sizeof(ucNBNSName ) - 2;
			pucTarget[ 1 ] = '\0';

			/* Start with decoding the last 2 bytes. */
			pucSource = pucUDPPayloadBuffer + ( offsetof( NBNSRequest_t, ucName ) + ( dnsNBNS_ENCODED_NAME_LENGTH - 2 ) );

			for( ;; )
			{
				ucByte = ( uint8_t ) ( ( ( pucSource[ 0 ] - 0x41 ) << 4 ) | ( pucSource[ 1 ] - 0x41 ) );

				/* Make sure there are no trailing spaces in the name. */
				if( ( ucByte == ' ' ) && ( pucTarget[ 1 ] == '\0' ) )
				{
					ucByte = '\0';
				}

				*pucTarget = ucByte;

				if( pucTarget == ucNBNSName )
				{
					break;
				}

				pucTarget -= 1;
				pucSource -= 2;
			}

			#if( ipconfigUSE_DNS_CACHE == 1 )
			{
				if( ( usFlags & dnsNBNS_FLAGS_RESPONSE ) != 0 )
				{
					/* If this is a response from another device,
					add the name to the DNS cache */
					prvProcessDNSCache( ( char * ) ucNBNSName, &ulIPAddress, pdFALSE );
				}
			}
			#else
			{
				/* Avoid compiler warnings. */
				( void ) ulIPAddress;
			}
			#endif /* ipconfigUSE_DNS_CACHE */

			pxNetworkBuffer = pxUDPPayloadBuffer_to_NetworkBuffer( pucUDPPayloadBuffer );

			if( pxNetworkBuffer->pxEndPoint == NULL )
			{
				pxNetworkBuffer->pxEndPoint = prvFindEndPointOnNetMask( pxNetworkBuffer );
			}
			if( pxNetworkBuffer->pxEndPoint != NULL )
			{
				memcpy( &xEndPoint, pxNetworkBuffer->pxEndPoint, sizeof( xEndPoint ) );
			}
			#if( ipconfigUSE_IPv6 != 0 )
			{
				xEndPoint.bits.bIPv6 = pdFALSE_UNSIGNED;
			}
			#endif

			if( ( ( usFlags & dnsNBNS_FLAGS_RESPONSE ) == 0 ) &&
				( usType == dnsNBNS_TYPE_NET_BIOS ) &&
				( xApplicationDNSQueryHook( &xEndPoint, ( const char * ) ucNBNSName ) != pdFALSE ) )
			{
			uint16_t usLength;
			DNSMessage_t *pxMessage;
			NBNSAnswer_t *pxAnswer;

				/* Someone is looking for a device with ucNBNSName,
				prepare a positive reply. */

				if( ( xBufferAllocFixedSize == pdFALSE ) && ( pxNetworkBuffer != NULL ) )
				{
				NetworkBufferDescriptor_t *pxNewBuffer;
				BaseType_t xDataLength = pxNetworkBuffer->xDataLength + sizeof( UDPHeader_t ) +

					sizeof( EthernetHeader_t ) + sizeof( IPHeader_t );

					/* The field xDataLength was set to the length of the UDP payload.
					The answer (reply) will be longer than the request, so the packet
					must be duplicated into a bigger buffer */
					pxNetworkBuffer->xDataLength = xDataLength;
					pxNewBuffer = pxDuplicateNetworkBufferWithDescriptor( pxNetworkBuffer, xDataLength + 16 );
					if( pxNewBuffer != NULL )
					{
						pucUDPPayloadBuffer = pxNewBuffer->pucEthernetBuffer + sizeof( UDPPacket_t );
						pxNetworkBuffer = pxNewBuffer;
					}
					else
					{
						/* Just prevent that a reply will be sent */
						pxNetworkBuffer = NULL;
					}
				}

				/* Should not occur: pucUDPPayloadBuffer is part of a xNetworkBufferDescriptor */
				if( pxNetworkBuffer != NULL )
				{
					pxMessage = (DNSMessage_t *)pucUDPPayloadBuffer;

					/* As the fields in the structures are not word-aligned, we have to
					copy the values byte-by-byte using macro's vSetField16() and vSetField32() */
					vSetField16( pxMessage, DNSMessage_t, usFlags, dnsNBNS_QUERY_RESPONSE_FLAGS ); /* 0x8500 */
					vSetField16( pxMessage, DNSMessage_t, usQuestions, 0 );
					vSetField16( pxMessage, DNSMessage_t, usAnswers, 1 );
					vSetField16( pxMessage, DNSMessage_t, usAuthorityRRs, 0 );
					vSetField16( pxMessage, DNSMessage_t, usAdditionalRRs, 0 );

					pxAnswer = (NBNSAnswer_t *)( pucUDPPayloadBuffer + offsetof( NBNSRequest_t, usType ) );

					vSetField16( pxAnswer, NBNSAnswer_t, usType, usType );	/* Type */
					vSetField16( pxAnswer, NBNSAnswer_t, usClass, dnsNBNS_CLASS_IN );	/* Class */
					vSetField32( pxAnswer, NBNSAnswer_t, ulTTL, dnsNBNS_TTL_VALUE );
					vSetField16( pxAnswer, NBNSAnswer_t, usDataLength, 6 ); /* 6 bytes including the length field */
					vSetField16( pxAnswer, NBNSAnswer_t, usNbFlags, dnsNBNS_NAME_FLAGS );
					vSetField32( pxAnswer, NBNSAnswer_t, ulIPAddress, FreeRTOS_ntohl( xEndPoint.ulIPAddress ) );

					usLength = ( uint16_t ) ( offsetof( NBNSRequest_t, usType ) + sizeof( NBNSAnswer_t ) );

					prvReplyDNSMessage( pxNetworkBuffer, usLength );
				}
			}
		}
	}

#endif	/* ipconfigUSE_NBNS */
/*-----------------------------------------------------------*/

static Socket_t prvCreateDNSSocket( void )
{
static Socket_t xSocket = NULL;
struct freertos_sockaddr xAddress;
BaseType_t xReturn;
TickType_t xTimeoutTime = pdMS_TO_TICKS( 200 );

	/* This must be the first time this function has been called.  Create
	the socket. */
	xSocket = FreeRTOS_socket( FREERTOS_AF_INET, FREERTOS_SOCK_DGRAM, FREERTOS_IPPROTO_UDP );

	/* Auto bind the port. */
	xAddress.sin_port = 0u;
	xReturn = FreeRTOS_bind( xSocket, &xAddress, sizeof( xAddress ) );

	/* Check the bind was successful, and clean up if not. */
	if( xReturn != 0 )
	{
		FreeRTOS_closesocket( xSocket );
		xSocket = NULL;
	}
	else
	{
		/* Set the send and receive timeouts. */
		FreeRTOS_setsockopt( xSocket, 0, FREERTOS_SO_RCVTIMEO, ( void * ) &xTimeoutTime, sizeof( TickType_t ) );
		FreeRTOS_setsockopt( xSocket, 0, FREERTOS_SO_SNDTIMEO, ( void * ) &xTimeoutTime, sizeof( TickType_t ) );
	}

	return xSocket;
}
/*-----------------------------------------------------------*/

#if( ( ipconfigUSE_NBNS == 1 ) || ( ipconfigUSE_LLMNR == 1 ) )

	static void prvReplyDNSMessage( NetworkBufferDescriptor_t *pxNetworkBuffer, BaseType_t lNetLength )
	{
	UDPPacket_t *pxUDPPacket;
	IPHeader_t *pxIPHeader;
	UDPHeader_t *pxUDPHeader;
	NetworkEndPoint_t *pxEndPoint = prvFindEndPointOnNetMask( pxNetworkBuffer );
	size_t xDataLength;

		pxUDPPacket = (UDPPacket_t *) pxNetworkBuffer->pucEthernetBuffer;
		pxIPHeader = &pxUDPPacket->xIPHeader;

	#if( ipconfigUSE_IPv6 != 0 )
		if( ( pxIPHeader->ucVersionHeaderLength & 0xf0 ) == 0x60 )
		{
		UDPPacket_IPv6_t *xUDPPacket_IPv6;
		IPHeader_IPv6_t *pxIPHeader_IPv6;

			xUDPPacket_IPv6 = ( UDPPacket_IPv6_t * )pxNetworkBuffer->pucEthernetBuffer;
			pxIPHeader_IPv6 = &( xUDPPacket_IPv6->xIPHeader );
			pxUDPHeader = &xUDPPacket_IPv6->xUDPHeader;

			pxIPHeader_IPv6->usPayloadLength = FreeRTOS_htons( lNetLength + ipSIZE_OF_UDP_HEADER );

			{
				memcpy( pxIPHeader_IPv6->xDestinationIPv6Address.ucBytes, pxIPHeader_IPv6->xSourceIPv6Address.ucBytes, sizeof( IPv6_Address_t ) );
				memcpy( pxIPHeader_IPv6->xSourceIPv6Address.ucBytes, pxEndPoint->ulIPAddress_IPv6.ucBytes, sizeof( IPv6_Address_t ) );
			}
	//		memcpy( pxIPHeader_IPv6->xDestinationIPv6Address.ucBytes, pxIPHeader_IPv6->xSourceIPv6Address.ucBytes, sizeof( pxIPHeader_IPv6->xDestinationIPv6Address ) );
	//		memcpy( pxIPHeader_IPv6->xSourceIPv6Address.ucBytes, pxEndPoint->ulIPAddress_IPv6.ucBytes, sizeof( pxIPHeader_IPv6->xSourceIPv6Address ) );
	FreeRTOS_printf( ( "DNSreturn to %pip\n", pxEndPoint->ulIPAddress_IPv6.ucBytes ) );
			xUDPPacket_IPv6->xUDPHeader.usLength = FreeRTOS_htons( lNetLength + ipSIZE_OF_UDP_HEADER );
			vFlip_16( pxUDPHeader->usSourcePort, pxUDPHeader->usDestinationPort );
			xDataLength = ( size_t ) ( lNetLength + ipSIZE_OF_IP_HEADER_IPv6 + ipSIZE_OF_UDP_HEADER + ipSIZE_OF_ETH_HEADER );
		}
		else
	#endif /* ipconfigUSE_IPv6 */
		{
			pxUDPHeader = &pxUDPPacket->xUDPHeader;
			/* HT: started using defines like 'ipSIZE_OF_xxx' */
			pxIPHeader->usLength               = FreeRTOS_htons( lNetLength + ipSIZE_OF_IP_HEADER_IPv4 + ipSIZE_OF_UDP_HEADER );
			/* HT:endian: should not be translated, copying from packet to packet */
			pxIPHeader->ulDestinationIPAddress = pxIPHeader->ulSourceIPAddress;
			pxIPHeader->ulSourceIPAddress      = pxEndPoint ? pxEndPoint->ulIPAddress : 0;
			pxIPHeader->ucTimeToLive           = ipconfigUDP_TIME_TO_LIVE;
			pxIPHeader->usIdentification       = FreeRTOS_htons( usPacketIdentifier );
			usPacketIdentifier++;
			pxUDPHeader->usLength              = FreeRTOS_htons( lNetLength + ipSIZE_OF_UDP_HEADER );
			vFlip_16( pxUDPHeader->usSourcePort, pxUDPHeader->usDestinationPort );
			xDataLength = ( size_t ) ( lNetLength + ipSIZE_OF_IP_HEADER_IPv4 + ipSIZE_OF_UDP_HEADER + ipSIZE_OF_ETH_HEADER );
		}

		#if( ipconfigDRIVER_INCLUDED_TX_IP_CHECKSUM == 0 )
		{
			#if( ipconfigUSE_IPv6 != 0 )
			/* IPv6 IP-headers have no checmsum field. */
			if( ( pxIPHeader->ucVersionHeaderLength & 0xf0 ) != 0x60 )
			#endif
			{
				/* Calculate the IP header checksum. */
				pxIPHeader->usHeaderChecksum       = 0x00;
				pxIPHeader->usHeaderChecksum       = usGenerateChecksum( 0UL, ( uint8_t * ) &( pxIPHeader->ucVersionHeaderLength ), ipSIZE_OF_IP_HEADER_IPv4 );
				pxIPHeader->usHeaderChecksum       = ~FreeRTOS_htons( pxIPHeader->usHeaderChecksum );
			}

			/* calculate the UDP checksum for outgoing package */
			usGenerateProtocolChecksum( ( uint8_t* ) pxUDPPacket, pdTRUE );
		}
		#endif

		/* Important: tell NIC driver how many bytes must be sent */
		pxNetworkBuffer->xDataLength = xDataLength;

		/* This function will fill in the eth addresses and send the packet */
		vReturnEthernetFrame( pxNetworkBuffer, pdFALSE );
	}

#endif /* ipconfigUSE_NBNS == 1 || ipconfigUSE_LLMNR == 1 */
/*-----------------------------------------------------------*/

#if( ipconfigUSE_DNS_CACHE == 1 )

	static void prvProcessDNSCache( const char *pcName, uint32_t *pulIP, BaseType_t xLookUp )
	{
	BaseType_t x;
	BaseType_t xFound = pdFALSE;
	static BaseType_t xFreeEntry = 0;

		/* For each entry in the DNS cache table. */
		for( x = 0; x < ipconfigDNS_CACHE_ENTRIES; x++ )
		{
			if( xDNSCache[ x ].pcName[ 0 ] == 0 )
			{
				break;
			}

			if( strncmp( xDNSCache[ x ].pcName, pcName, sizeof( xDNSCache[ x ].pcName ) ) == 0 )
			{
				/* Is this function called for a lookup or to add/update an IP address? */
				if( xLookUp != pdFALSE )
				{
					*pulIP = xDNSCache[ x ].ulIPAddress;
				}
				else
				{
					xDNSCache[ x ].ulIPAddress = *pulIP;
				}

				xFound = pdTRUE;
				break;
			}
		}

		if( xFound == pdFALSE )
		{
			if( xLookUp != pdFALSE )
			{
				*pulIP = 0;
			}
			else
			{
				/* Called to add or update an item */
				strncpy( xDNSCache[ xFreeEntry ].pcName, pcName, sizeof( xDNSCache[ xFreeEntry ].pcName ) );
				xDNSCache[ xFreeEntry ].ulIPAddress = *pulIP;

				xFreeEntry++;
				if( xFreeEntry == ipconfigDNS_CACHE_ENTRIES )
				{
					xFreeEntry = 0;
				}
			}
		}

		if( ( xLookUp == 0 ) || ( *pulIP != 0 ) )
		{
			FreeRTOS_debug_printf( ( "prvProcessDNSCache: %s: '%s' @ %lxip\n", xLookUp ? "look-up" : "add", pcName, FreeRTOS_ntohl( *pulIP ) ) );
		}
	}

#endif /* ipconfigUSE_DNS_CACHE */

#endif /* ipconfigUSE_DNS != 0 */


