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
#include "semphr.h"

/* FreeRTOS+TCP includes. */
#include "FreeRTOS_IP.h"
#include "FreeRTOS_Sockets.h"
#include "FreeRTOS_IP_Private.h"
#include "FreeRTOS_UDP_IP.h"
#include "FreeRTOS_DNS.h"
#include "FreeRTOS_DHCP.h"
#include "NetworkBufferManagement.h"
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
#if( ipconfigUSE_NBNS == 1 )
	#define dnsNBNS_FLAGS_RESPONSE				0x8000u
	#define dnsNBNS_FLAGS_OPCODE_MASK			0x7800u
	#define dnsNBNS_FLAGS_OPCODE_QUERY			0x0000u
	#define dnsNBNS_FLAGS_OPCODE_REGISTRATION	0x2800u
#endif

/* Host types. */
#define dnsCLASS_IN							0x01u

/* LLMNR constants. */
#ifndef _lint
	#define dnsLLMNR_TTL_VALUE					300000uL
	#define dnsLLMNR_FLAGS_IS_REPONSE  			0x8000u
#endif

/* NBNS constants. */
#if( ipconfigUSE_NBNS != 0 )
	#define dnsNBNS_TTL_VALUE					3600uL /* 1 hour valid */
	#define dnsNBNS_TYPE_NET_BIOS				0x0020u
	#define dnsNBNS_CLASS_IN					0x01u
	#define dnsNBNS_NAME_FLAGS					0x6000u
	#define dnsNBNS_ENCODED_NAME_LENGTH			32

	/* If the queried NBNS name matches with the device's name,
	the query will be responded to with these flags: */
	#define dnsNBNS_QUERY_RESPONSE_FLAGS		( 0x8500u )
#endif	/* ipconfigUSE_NBNS */

/* Flag DNS parsing errors in situations where an IPv4 address is the return
type. */
#define dnsPARSE_ERROR					  0uL

/*
 * Create a socket and bind it to the standard DNS port number.  Return the
 * the created socket - or NULL if the socket could not be created or bound.
 */
static Socket_t prvCreateDNSSocket( void );

/*
 * Create the DNS message in the zero copy buffer passed in the first parameter.
 */
static size_t prvCreateDNSMessage( uint8_t *pucUDPPayloadBuffer,
								   const char *pcHostName,
								   TickType_t uxIdentifier,
								   UBaseType_t uxHostType );

/*
 * Simple routine that jumps over the NAME field of a resource record.
 * It returns the number of bytes read.
 */
static size_t prvSkipNameField( uint8_t *pucByte,
								size_t uxSourceLen );

/*
 * Process a response packet from a DNS server.
 * The parameter 'xExpected' indicates whether the identifier in the reply
 * was expected, and thus if the DNS cache may be updated with the reply.
 */
static uint32_t prvParseDNSReply( uint8_t *pucUDPPayloadBuffer,
								  size_t uxBufferLength,
								  struct freertos_sockaddr *pxAddress,
								  BaseType_t xExpected );

/*
 * Check if hostname is already known. If not, call prvGetHostByName() to send a DNS request.
 */
#if( ipconfigDNS_USE_CALLBACKS == 1 )
	static uint32_t prvPrepareLookup( const char *pcHostName,
									  struct freertos_sockaddr *pxAddress,
									  FOnDNSEvent pCallback,
									  void *pvSearchID,
									  TickType_t xTimeout );
#else
	static uint32_t prvPrepareLookup( const char *pcHostName,
									  struct freertos_sockaddr *pxAddress );
#endif

/*
 * Prepare and send a message to a DNS server.  'uxReadTimeOut_ticks' will be passed as
 * zero, in case the user has supplied a call-back function.
 */
static uint32_t prvGetHostByName( const char *pcHostName,
								  TickType_t uxIdentifier,
								  TickType_t uxReadTimeOut_ticks,
								  struct freertos_sockaddr *pxAddress );

#if( ipconfigUSE_IPv6 != 0 )
	static void vDNSSetCallBack( const char *pcHostName, void *pvSearchID, FOnDNSEvent pCallbackFunction, TickType_t xTimeout, TickType_t uxIdentifier, BaseType_t xIsIPv6 );
#else
	static void vDNSSetCallBack( const char *pcHostName, void *pvSearchID, FOnDNSEvent pCallbackFunction, TickType_t xTimeout, TickType_t uxIdentifier );
#endif

/*
 * The NBNS and the LLMNR protocol share this reply function.
 */
#if( ( ipconfigUSE_NBNS == 1 ) || ( ipconfigUSE_LLMNR == 1 ) )
	static void prvReplyDNSMessage( NetworkBufferDescriptor_t *pxNetworkBuffer,
									BaseType_t lNetLength );
#endif

#if( ipconfigUSE_NBNS == 1 )
	static portINLINE void prvTreatNBNS( uint8_t *pucUDPPayloadBuffer,
										 size_t uxBufferLength,
										 uint32_t ulIPAddress );
#endif /* ipconfigUSE_NBNS */


#if( ipconfigUSE_DNS_CACHE == 1 ) || ( ipconfigDNS_USE_CALLBACKS == 1 )
	static size_t prvReadNameField( uint8_t *pucByte,
									size_t uxSourceLen,
									char *pcName,
									size_t uxDestLen );
#endif	/* ipconfigUSE_DNS_CACHE || ipconfigDNS_USE_CALLBACKS */

#if( ipconfigUSE_DNS_CACHE == 1 )
	typedef struct xIPv46_Address {
		/* A struct that can hold either an IPv4 or an IPv6 address. */
		union {
			uint32_t ulIPAddress;
		#if( ipconfigUSE_IPv6 != 0 )
			IPv6_Address_t xAddress_IPv6;
		#endif
		} u;
		#if( ipconfigUSE_IPv6 != 0 )
		/* pdTRUE if the IPv6 member is used. */
		uint8_t ucIs_IPv6;
		#endif
	} IPv46_Address_t;

	static BaseType_t prvProcessDNSCache( const char *pcName,
										  IPv46_Address_t *pxIP,
										  uint32_t ulTTL,
										  BaseType_t xLookUp );

	typedef struct xDNS_CACHE_TABLE_ROW
	{
		IPv46_Address_t xAddr;		/* The IP address of a DSN cache entry. */
		char pcName[ ipconfigDNS_CACHE_NAME_LENGTH ];  /* The name of the host */
		uint32_t ulTTL; /* Time-to-Live (in seconds) from the DNS server. */
		uint32_t ulTimeWhenAddedInSeconds;
	} DNSCacheRow_t;

	static DNSCacheRow_t xDNSCache[ ipconfigDNS_CACHE_ENTRIES ];
	static BaseType_t xFreeEntry = 0;

	void FreeRTOS_dnsclear()
	{
		memset( xDNSCache, 0x0, sizeof( xDNSCache ) );
		xFreeEntry = 0;
	}
#endif /* ipconfigUSE_DNS_CACHE == 1 */

#if( ipconfigUSE_LLMNR == 1 )
	const MACAddress_t xLLMNR_MacAdress = { { 0x01, 0x00, 0x5e, 0x00, 0x00, 0xfc } };
#endif	/* ipconfigUSE_LLMNR == 1 */

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
	};/*lint !e9018 declaration of symbol 'ipLLMNR_IP_ADDR_IPv6' with union based type 'const IPv6_Address_t' [MISRA 2012 Rule 19.2, advisory]. */
	const MACAddress_t xLLMNR_MacAdressIPv6 = { { 0x33, 0x33, 0x00, 0x01, 0x00, 0x03 } };
#endif /* ipconfigUSE_LLMNR && ipconfigUSE_IPv6 */

/*-----------------------------------------------------------*/

/*lint -e754 -e830 -e766*/
#include "pack_struct_start.h"	/*lint !e537 !e451 !e9019 !e766*/
struct xDNSMessage
{
	uint16_t usIdentifier;
	uint16_t usFlags;
	uint16_t usQuestions;
	uint16_t usAnswers;
	uint16_t usAuthorityRRs;	
	uint16_t usAdditionalRRs;	/*lint !e754 !e830*/
}	/*lint !e659 */
#include "pack_struct_end.h"	/*lint !e537 !e451 !e9019 !e766*/
typedef struct xDNSMessage DNSMessage_t;

/* A DNS query consists of a header, as described in 'struct xDNSMessage'
It is followed by 1 or more queries, each one consisting of a name and a tail,
with two fields: type and class
*/
#include "pack_struct_start.h"	/*lint !e537 !e451 !e9019 !e766*/
struct xDNSTail
{
	uint16_t usType;
	uint16_t usClass;
}	/*lint !e659 */
#include "pack_struct_end.h"	/*lint !e537 !e451 !e9019 !e766*/
typedef struct xDNSTail DNSTail_t;

/* DNS answer record header. */
#include "pack_struct_start.h"	/*lint !e537 !e451 !e9019 !e766*/
struct xDNSAnswerRecord
{
	uint16_t usType;
	uint16_t usClass;
	uint32_t ulTTL;
	uint16_t usDataLength;
}	/*lint !e659 */
#include "pack_struct_end.h"	/*lint !e537 !e451 !e9019 !e766*/
typedef struct xDNSAnswerRecord DNSAnswerRecord_t;

#if( ipconfigUSE_LLMNR == 1 )

	#include "pack_struct_start.h"	/*lint !e537 !e451 !e9019 !e766*/
	struct xLLMNRAnswer
	{
		uint8_t ucNameCode;
		uint8_t ucNameOffset;	/* The name is not repeated in the answer, only the offset is given with "0xc0 <offs>" */
		uint16_t usType;
		uint16_t usClass;
		uint32_t ulTTL;
		uint16_t usDataLength;
		uint32_t ulIPAddress;
	}	/*lint !e659 */
	#include "pack_struct_end.h"	/*lint !e537 !e451 !e9019 !e766*/
	typedef struct xLLMNRAnswer LLMNRAnswer_t;

#endif /* ipconfigUSE_LLMNR == 1 */

#if( ipconfigUSE_NBNS == 1 )

	#include "pack_struct_start.h"	/*lint !e537 !e451 !e9019 !e766*/
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
	}	/*lint !e659 */
	#include "pack_struct_end.h"	/*lint !e537 !e451 !e9019 !e766*/
	typedef struct xNBNSRequest NBNSRequest_t;

	#include "pack_struct_start.h"	/*lint !e537 !e451 !e9019 !e766*/
	struct xNBNSAnswer
	{
		uint16_t usType;
		uint16_t usClass;
		uint32_t ulTTL;
		uint16_t usDataLength;
		uint16_t usNbFlags;		/* NetBIOS flags 0x6000 : IP-address, big-endian */
		uint32_t ulIPAddress;
	}	/*lint !e659 */
	#include "pack_struct_end.h"	/*lint !e537 !e451 !e9019 !e766*/
	typedef struct xNBNSAnswer NBNSAnswer_t;

#endif /* ipconfigUSE_NBNS == 1 */

/*-----------------------------------------------------------*/

#if( ipconfigUSE_DNS_CACHE == 1 )
	uint32_t FreeRTOS_dnslookup( const char *pcHostName )
	{
	IPv46_Address_t xIPv46_Address;

		memset( &xIPv46_Address, 0, sizeof xIPv46_Address );
		/* Also the fields 'ucIs_IPv6' and 'ulIPAddress' have been cleared. */
		( void ) prvProcessDNSCache( pcHostName, &xIPv46_Address, 0, pdTRUE );

		return xIPv46_Address.u.ulIPAddress;
	}
#endif /* ipconfigUSE_DNS_CACHE == 1 */
/*-----------------------------------------------------------*/

#if( ipconfigUSE_DNS_CACHE == 1 ) && ( ipconfigUSE_IPv6 != 0 )
	uint32_t FreeRTOS_dnslookup6( const char *pcHostName, IPv6_Address_t *pxAddress_IPv6 )
	{
	IPv46_Address_t xIPv46_Address;
	BaseType_t xResult;
	uint32_t ulReturn;

		memset( &xIPv46_Address, 0, sizeof xIPv46_Address );
		/* Let prvProcessDNSCache only return IPv6 addresses. */
		xIPv46_Address.ucIs_IPv6 = ( uint8_t ) 1u;
		xResult = prvProcessDNSCache( pcHostName, &xIPv46_Address, 0, pdTRUE );
		if( xResult != pdFALSE )
		{
			memcpy( pxAddress_IPv6, xIPv46_Address.u.xAddress_IPv6.ucBytes, ipSIZE_OF_IPv6_ADDRESS );
		}

		if( xResult != pdFALSE )
		{
			ulReturn = 1uL;
		}
		else
		{
			ulReturn = 0uL;
		}
		return ulReturn;
	}
#endif /* ( ipconfigUSE_DNS_CACHE == 1 ) && ( ipconfigUSE_IPv6 != 0 ) */
/*-----------------------------------------------------------*/

#if( ipconfigDNS_USE_CALLBACKS == 1 )

	typedef struct xDNS_Callback {
		TickType_t xRemaningTime;		/* Timeout in ms */
		FOnDNSEvent pCallbackFunction;	/* Function to be called when the address has been found or when a timeout has beeen reached */
		TimeOut_t xTimeoutState;
		void *pvSearchID;
		struct xLIST_ITEM xListItem;
		#if( ipconfigUSE_IPv6 != 0 )
			/* Remeber if this was a IPv6 lookup. */
			BaseType_t xIsIPv6;
		#endif
		char pcName[ 1 ];
	} DNSCallback_t;

	static List_t xCallbackList;

	/* Define FreeRTOS_gethostbyname() as a normal blocking call. */
	uint32_t FreeRTOS_gethostbyname( const char *pcHostName )
	{
		return FreeRTOS_gethostbyname_a( pcHostName, NULL, NULL, 0 );
	}
	/*-----------------------------------------------------------*/

	/* Initialise the list of call-back structures. */
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
	void vDNSCheckCallBack( void *pvSearchID )
	{
	const ListItem_t *pxIterator;
	const MiniListItem_t* xEnd = ipPOINTER_CAST( const MiniListItem_t *, listGET_END_MARKER( &xCallbackList ) );
//	const MiniListItem_t* xEnd = ( const MiniListItem_t * ) listGET_END_MARKER( &xCallbackList );

		vTaskSuspendAll();
		{
			for( pxIterator  = ( const ListItem_t * ) listGET_NEXT( xEnd );
				 pxIterator != ipPOINTER_CAST( const ListItem_t *, xEnd );
				  )
			{
				DNSCallback_t *pxCallback = ipPOINTER_CAST( DNSCallback_t *, listGET_LIST_ITEM_OWNER( pxIterator ) );
				/* Move to the next item because we might remove this item */
				pxIterator  = ( const ListItem_t * ) listGET_NEXT( pxIterator );
				if( ( pvSearchID != NULL ) && ( pvSearchID == pxCallback->pvSearchID ) )
				{
					configPRINTF( ( "vDNSCheckCallBack Remove '%p'\n", pvSearchID ) );
					( void ) uxListRemove( &pxCallback->xListItem );
					vPortFree( pxCallback );
				}
				else if( xTaskCheckForTimeOut( &pxCallback->xTimeoutState, &pxCallback->xRemaningTime ) != pdFALSE )
				{
					/* A time-out occurred in the asynchronous search.
					Cal the application hook with the proper information. */
					#if( ipconfigUSE_IPv6 != 0 )
					{
						struct freertos_sockaddr6 xAddress6;
						memset( &( xAddress6 ), 0, sizeof xAddress6 );
						xAddress6.sin_len = ( uint8_t ) sizeof( xAddress6 );
						if( pxCallback->xIsIPv6 != pdFALSE )
						{
							xAddress6.sin_family = FREERTOS_AF_INET6;
						}
						else
						{
							xAddress6.sin_family = FREERTOS_AF_INET4;
						}
						pxCallback->pCallbackFunction( pxCallback->pcName, pxCallback->pvSearchID, &( xAddress6 ) );
					}
					#else
					{
						pxCallback->pCallbackFunction( pxCallback->pcName, pxCallback->pvSearchID, 0uL );
					}
					#endif
					( void )uxListRemove( &pxCallback->xListItem );
					vPortFree( pxCallback );
				}
				else
				{
					/* This call-back is still waiting for a reply or a time-out. */
				}
			}
		}
		( void ) xTaskResumeAll();

		if( listLIST_IS_EMPTY( &xCallbackList ) != pdFALSE )
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
	#if( ipconfigUSE_IPv6 != 0 )
		static void vDNSSetCallBack( const char *pcHostName, void *pvSearchID, FOnDNSEvent pCallbackFunction, TickType_t xTimeout, TickType_t uxIdentifier, BaseType_t xIsIPv6 )
	#else
		static void vDNSSetCallBack( const char *pcHostName, void *pvSearchID, FOnDNSEvent pCallbackFunction, TickType_t xTimeout, TickType_t uxIdentifier )
	#endif
	{
		size_t uxLength = strlen( pcHostName );
		DNSCallback_t *pxCallback = ipPOINTER_CAST( DNSCallback_t *, pvPortMalloc( sizeof( *pxCallback ) + uxLength ) );

		/* Translate from ms to number of clock ticks. */
		xTimeout /= portTICK_PERIOD_MS;
		if( pxCallback != NULL )
		{
			if( listLIST_IS_EMPTY( &xCallbackList ) != pdFALSE )
			{
				/* This is the first one, start the DNS timer to check for timeouts */
				vIPReloadDNSTimer( FreeRTOS_min_uint32( 1000U, xTimeout ) );
			}
			strcpy( pxCallback->pcName, pcHostName );
			pxCallback->pCallbackFunction = pCallbackFunction;
			pxCallback->pvSearchID = pvSearchID;
			pxCallback->xRemaningTime = xTimeout;
			#if( ipconfigUSE_IPv6 != 0 )
			{
				pxCallback->xIsIPv6 = xIsIPv6;
			}
			#endif
			vTaskSetTimeOutState( &pxCallback->xTimeoutState );
			listSET_LIST_ITEM_OWNER( &( pxCallback->xListItem ), ipPOINTER_CAST( void *, pxCallback ) );
			listSET_LIST_ITEM_VALUE( &( pxCallback->xListItem ), uxIdentifier );
			vTaskSuspendAll();
			{
				vListInsertEnd( &xCallbackList, &pxCallback->xListItem );
			}
			( void ) xTaskResumeAll();
			configPRINTF( ( "vDNSSetCallBack (0x%04lX) for '%s'\n", uxIdentifier, pxCallback->pcName ) );
		}
	}
	/*-----------------------------------------------------------*/

	/* A DNS reply was received, see if there is any matching entry and
	call the handler.  Returns pdTRUE if uxIdentifier was recognised. */
#if( ipconfigUSE_IPv6 != 0 )
	static BaseType_t xDNSDoCallback( TickType_t uxIdentifier, const char *pcName, struct freertos_sockaddr6 *pxAddress );
	static BaseType_t xDNSDoCallback( TickType_t uxIdentifier, const char *pcName, struct freertos_sockaddr6 *pxAddress )
#else
	static BaseType_t xDNSDoCallback( TickType_t uxIdentifier, const char *pcName, uint32_t ulIPAddress );
	static BaseType_t xDNSDoCallback( TickType_t uxIdentifier, const char *pcName, uint32_t ulIPAddress )
#endif
	{
		BaseType_t xResult = pdFALSE;
		const ListItem_t *pxIterator;
		const ListItem_t* xEnd = ipPOINTER_CAST( const ListItem_t *, listGET_END_MARKER( &xCallbackList ) );

		vTaskSuspendAll();
		{
			for( pxIterator  = ( const ListItem_t * ) listGET_NEXT( xEnd );
				 pxIterator != ( const ListItem_t * ) xEnd;
				 pxIterator  = ( const ListItem_t * ) listGET_NEXT( pxIterator ) )
			{
				if( listGET_LIST_ITEM_VALUE( pxIterator ) == uxIdentifier )
				{
					DNSCallback_t *pxCallback = ipPOINTER_CAST( DNSCallback_t *, listGET_LIST_ITEM_OWNER( pxIterator ) );
					configPRINTF( ( "xDNSDoCallback (0x%04lX) for '%s'\n", uxIdentifier, pcName ) );
					#if( ipconfigUSE_IPv6 != 0 )
					pxCallback->pCallbackFunction( pcName, pxCallback->pvSearchID, pxAddress );
					#else
					pxCallback->pCallbackFunction( pcName, pxCallback->pvSearchID, ulIPAddress );
					#endif
					( void ) uxListRemove( &pxCallback->xListItem );
					vPortFree( pxCallback );
					if( listLIST_IS_EMPTY( &xCallbackList ) != pdFALSE )
					{
						vIPSetDnsTimerEnableState( pdFALSE );
					}
					xResult = pdTRUE;
					break;
				}
			}
		}
		( void ) xTaskResumeAll();
		return xResult;
	}

#endif	/* ipconfigDNS_USE_CALLBACKS == 1 */
/*-----------------------------------------------------------*/


#if( ipconfigDNS_USE_CALLBACKS == 1 )
	BaseType_t FreeRTOS_getaddrinfo(const char *pcName,							/* The name of the node or device */
									const char *pcService,						/* Ignored for now. */
									const struct freertos_addrinfo *pxHints,	/* If not NULL: preferences. */
									struct freertos_addrinfo **ppxResult)		/* An allocated struct, containing the results. */
	{
		/* Call the asynchronous version with NULL parameters. */
		return FreeRTOS_getaddrinfo_a( pcName, pcService, pxHints, ppxResult, NULL, NULL, 0uL );
	}
#endif
/*-----------------------------------------------------------*/

#if( ipconfigDNS_USE_CALLBACKS == 1 )
	BaseType_t FreeRTOS_getaddrinfo_a( const char *pcName,					/* The name of the node or device */
								 const char *pcService,						/* Ignored for now. */
								 const struct freertos_addrinfo *pxHints,	/* If not NULL: preferences. */
								 struct freertos_addrinfo **ppxResult,		/* An allocated struct, containing the results. */
								 FOnDNSEvent pCallback,
								 void *pvSearchID,
								 TickType_t xTimeout )
#else
	BaseType_t FreeRTOS_getaddrinfo(const char *pcName,							/* The name of the node or device */
									const char *pcService,						/* Ignored for now. */
									const struct freertos_addrinfo *pxHints,	/* If not NULL: preferences. */
									struct freertos_addrinfo **ppxResult)		/* An allocated struct, containing the results. */
#endif	/* ipconfigDNS_USE_CALLBACKS == 1 */
{
BaseType_t xReturn = -pdFREERTOS_ERRNO_ENOENT;
uint32_t ulResult;
struct freertos_addrinfo *pxAddrInfo;
#if( ipconfigUSE_IPv6 != 0 )
struct freertos_sockaddr6 xAddress;
#else
struct freertos_sockaddr xAddress;
#endif

	( void ) pcService;
	( void ) pxHints;

	*( ppxResult ) = NULL;
	memset( &( xAddress ), 0, sizeof( xAddress ) );
	#if( ipconfigUSE_IPv6 != 0 )
	if( ( pxHints != NULL ) && ( pxHints->ai_family == FREERTOS_AF_INET6 ) )
	{
		xAddress.sin_family = FREERTOS_AF_INET6;
	}
	else
	#endif /* ( ipconfigUSE_IPv6 != 0 ) */
	{
		xAddress.sin_family = FREERTOS_AF_INET4;
	}

	#if( ipconfigDNS_USE_CALLBACKS == 1 )
	{
		ulResult = prvPrepareLookup( pcName, ipPOINTER_CAST( struct freertos_sockaddr *, &( xAddress ) ), pCallback, pvSearchID, xTimeout );
	}
	#else
	{
		ulResult = prvPrepareLookup( pcName, ipPOINTER_CAST( struct freertos_sockaddr *, &( xAddress ) ) );
	}
	#endif
	if( ulResult != 0uL )
	{
		pxAddrInfo = ipPOINTER_CAST( struct freertos_addrinfo *, pvPortMalloc( sizeof( *pxAddrInfo ) ) );
		if( pxAddrInfo != NULL )
		{
			memset( pxAddrInfo, 0, sizeof( *pxAddrInfo ) );
			pxAddrInfo->ai_canonname = pxAddrInfo->xPrivateStorage.ucName;
			strncpy( pxAddrInfo->xPrivateStorage.ucName, pcName, sizeof( pxAddrInfo->xPrivateStorage.ucName ) );

			#if( ipconfigUSE_IPv6 != 0 )
			{
				pxAddrInfo->ai_addr = ipPOINTER_CAST( struct freertos_sockaddr *, &( pxAddrInfo->xPrivateStorage.sockaddr6 ) );
			}
			#else
			{
				pxAddrInfo->ai_addr = &( pxAddrInfo->xPrivateStorage.sockaddr4 );
			}
			#endif

			#if( ipconfigUSE_IPv6 != 0 )
			if( xAddress.sin_family == ( uint8_t ) FREERTOS_AF_INET6 )
			{
				pxAddrInfo->ai_family =  FREERTOS_AF_INET6;
				pxAddrInfo->ai_addrlen = ipSIZE_OF_IPv6_ADDRESS;
				memcpy( pxAddrInfo->xPrivateStorage.sockaddr6.sin_addrv6.ucBytes, xAddress.sin_addrv6.ucBytes, ipSIZE_OF_IPv6_ADDRESS );
			}
			else
			#endif
			{
				pxAddrInfo->ai_addr->sin_addr = ulResult;
				pxAddrInfo->ai_family =  FREERTOS_AF_INET4;
				pxAddrInfo->ai_addrlen = ipSIZE_OF_IPv4_ADDRESS;
			}
			*( ppxResult ) = pxAddrInfo;
			xReturn = 0;
		}
		else
		{
			/* The node was found but a malloc failed. */
			xReturn = -pdFREERTOS_ERRNO_ENOMEM;
		}
	}
	return xReturn;
}
/*-----------------------------------------------------------*/

void FreeRTOS_freeaddrinfo(struct freertos_addrinfo *pxResult)
{
	if( pxResult != NULL )
	{
		vPortFree( pxResult );
	}
}
/*-----------------------------------------------------------*/

/*_RB_ How to determine which interface to perform the DNS look up on? */
#if( ipconfigDNS_USE_CALLBACKS == 0 )
	uint32_t FreeRTOS_gethostbyname( const char *pcHostName )
	{
		return prvPrepareLookup( pcHostName, NULL );
	}
#else
	uint32_t FreeRTOS_gethostbyname_a( const char *pcHostName,
								   FOnDNSEvent pCallback,
								   void *pvSearchID,
								   TickType_t xTimeout )
	{
		return prvPrepareLookup( pcHostName, NULL, pCallback, pvSearchID, xTimeout );
	}
#endif

#if( ipconfigDNS_USE_CALLBACKS == 1 )
	static uint32_t prvPrepareLookup( const char *pcHostName,
									  struct freertos_sockaddr *pxAddress,
									  FOnDNSEvent pCallback,
									  void *pvSearchID,
									  TickType_t xTimeout )
#else
	static uint32_t prvPrepareLookup( const char *pcHostName,
									  struct freertos_sockaddr *pxAddress )
#endif
{
uint32_t ulIPAddress;
TickType_t uxReadTimeOut_ticks = ipconfigDNS_RECEIVE_BLOCK_TIME_TICKS;
/* Generate a unique identifier for this query. Keep it in a local variable
 as gethostbyname() may be called from different threads */
TickType_t uxIdentifier = 0;
#if( ipconfigUSE_IPv6 != 0 )
	BaseType_t xIsIPv6;
#endif

#if( ipconfigUSE_IPv6 != 0 )
	if( ( pxAddress != NULL ) && ( pxAddress->sin_family == ( uint8_t ) FREERTOS_AF_INET6 ) )
	{
		xIsIPv6 = pdTRUE;
	}
	else
	{
		xIsIPv6 = pdFALSE;
	}
#endif

	#if( ipconfigINCLUDE_FULL_INET_ADDR == 1 )
	{
		#if( ipconfigUSE_IPv6 != 0 )
		if( xIsIPv6 != pdFALSE )
		{
		IPv6_Address_t xAddress_IPv6;

			/* ulIPAddress does not represent an IPv4 address here. It becomes non-zero when the look-up succeeds. */
			ulIPAddress = ( uint32_t ) FreeRTOS_inet_pton6( pcHostName, xAddress_IPv6.ucBytes );
			if( ( ulIPAddress != 0uL ) && ( pxAddress != NULL ) )
			{
			struct freertos_sockaddr6 *pxAddress6 = ipPOINTER_CAST(struct freertos_sockaddr6 *, pxAddress );

				memcpy( pxAddress6->sin_addrv6.ucBytes, xAddress_IPv6.ucBytes, ipSIZE_OF_IPv6_ADDRESS );
			}
		}
		else
		#endif
		{
			ulIPAddress = FreeRTOS_inet_addr( pcHostName );
		}
	}
	#endif

	/* If a DNS cache is used then check the cache before issuing another DNS
	request. */
	#if( ipconfigUSE_DNS_CACHE == 1 )
	{
		#if( ipconfigUSE_IPv6 != 0 )
		if( xIsIPv6 != pdFALSE )
		{
			if( ( pxAddress != NULL ) && ( ulIPAddress == 0UL ) )
			{
			struct freertos_sockaddr6 *pxAddress6 = ipPOINTER_CAST( struct freertos_sockaddr6 *, pxAddress );

				ulIPAddress = FreeRTOS_dnslookup6( pcHostName, &( pxAddress6->sin_addrv6 ) );
				if( ulIPAddress != 0uL )
				{
				}
			}
		}
		else
		#endif
		{
			if( ulIPAddress == 0UL )
			{
				ulIPAddress = FreeRTOS_dnslookup( pcHostName );

				if( ulIPAddress != 0uL )
				{
					FreeRTOS_debug_printf( ( "FreeRTOS_gethostbyname: found '%s' in cache: %lxip\n", pcHostName, ulIPAddress ) );
				}
				else
				{
					/* prvGetHostByName will be called to start a DNS lookup. */
				}
			}
		}
	}
	#endif /* ipconfigUSE_DNS_CACHE == 1 */

	/* Generate a unique identifier. */
	if( ulIPAddress == 0uL )
	{
		/* DNS identifiers are 16-bit. */
		uxIdentifier = ( TickType_t ) ( ipconfigRAND32() & 0xffffu );
		/* ipconfigRAND32() may not return a non-zero value. */
	}

	#if( ipconfigDNS_USE_CALLBACKS == 1 )
	{
		if( pCallback != NULL )
		{
			if( ulIPAddress == 0UL )
			{
				/* The user has provided a callback function, so do not block on recvfrom() */
				if( uxIdentifier != ( TickType_t ) 0u )
				{
					uxReadTimeOut_ticks = 0;
					#if( ipconfigUSE_IPv6 != 0 )
					{
						vDNSSetCallBack( pcHostName, pvSearchID, pCallback, xTimeout, ( TickType_t )uxIdentifier, xIsIPv6 );
					}
					#else
					{
						vDNSSetCallBack( pcHostName, pvSearchID, pCallback, xTimeout, ( TickType_t )uxIdentifier );
					}
					#endif
				}
			}
			else
			{
				/* The IP address is known, do the call-back now. */
				#if( ipconfigUSE_IPv6 != 0 )
				if( xIsIPv6 != pdFALSE )
				{
					pCallback( pcHostName, pvSearchID, ipPOINTER_CAST( struct freertos_sockaddr6 *, pxAddress ) );
				}
				else if( pxAddress != NULL )
				{
					pxAddress->sin_family = FREERTOS_AF_INET4;
					pxAddress->sin_addr = ulIPAddress;
					pCallback( pcHostName, pvSearchID, ipPOINTER_CAST( struct freertos_sockaddr6 *, pxAddress ) );
				}
				else
				{
					/* No comment. */
				}
				#else
				{
					if( pxAddress != NULL )
					{
						pxAddress->sin_family = FREERTOS_AF_INET4;
						pxAddress->sin_addr = ulIPAddress;
					}
					pCallback( pcHostName, pvSearchID, ulIPAddress );
				}
				#endif
			}
		}
	}
	#endif /* if ( ipconfigDNS_USE_CALLBACKS == 1 ) */

	if( ( ulIPAddress == 0UL ) && ( uxIdentifier != ( TickType_t ) 0u ) )
	{
		ulIPAddress = prvGetHostByName( pcHostName, uxIdentifier, uxReadTimeOut_ticks, pxAddress );
	}

	return ulIPAddress;
}
/*-----------------------------------------------------------*/

static uint32_t prvGetHostByName( const char *pcHostName,
								  TickType_t uxIdentifier,
								  TickType_t uxReadTimeOut_ticks,
								  struct freertos_sockaddr *pxAddress )
{
struct freertos_sockaddr xAddress;
Socket_t xDNSSocket;
uint32_t ulIPAddress = 0UL;
uint8_t *pucUDPPayloadBuffer = NULL, *pucReceiveBuffer;
uint32_t ulAddressLength = sizeof( struct freertos_sockaddr );
BaseType_t xAttempt;
int32_t lBytes;
size_t uxPayloadLength, uxExpectedPayloadLength;
TickType_t uxWriteTimeOut_ticks = ipconfigDNS_SEND_BLOCK_TIME_TICKS;
UBaseType_t uxHostType;
#if( ipconfigUSE_LLMNR == 1 )
	BaseType_t bHasDot = pdFALSE;
#endif /* ipconfigUSE_LLMNR == 1 */

	/* If LLMNR is being used then determine if the host name includes a '.' -
	if not then LLMNR can be used as the lookup method. */
	#if( ipconfigUSE_LLMNR == 1 )
	{
		const char *pucPtr;

		for( pucPtr = pcHostName; *pucPtr != ( char ) 0; pucPtr++ )
		{
			if( *pucPtr == '.' )
			{
				bHasDot = pdTRUE;
				break;
			}
		}
	}
	#endif /* ipconfigUSE_LLMNR == 1 */

	#if( ipconfigUSE_IPv6 != 0 )
	if( ( pxAddress != NULL ) && ( pxAddress->sin_family == ( uint8_t ) FREERTOS_AF_INET6 ) )
	{
		uxHostType = dnsTYPE_AAAA_HOST;
	}
	else	
	#endif
	{
		uxHostType = dnsTYPE_A_HOST;
	}

	/* Two is added at the end for the count of characters in the first
	subdomain part and the string end byte. */
	uxExpectedPayloadLength = sizeof( DNSMessage_t ) + strlen( pcHostName ) + sizeof( uint16_t ) + sizeof( uint16_t ) + 2u;
	#if( ipconfigUSE_IPv6 != 0 )
	{
		if( uxHostType == dnsTYPE_AAAA_HOST )
		{
			uxExpectedPayloadLength += ipSIZE_OF_IPv6_ADDRESS;
		}
	}
	#endif

	xDNSSocket = prvCreateDNSSocket();

	if( xDNSSocket != NULL )
	{
		( void ) FreeRTOS_setsockopt( xDNSSocket, 0, FREERTOS_SO_SNDTIMEO, &uxWriteTimeOut_ticks, sizeof( TickType_t ) );
		( void ) FreeRTOS_setsockopt( xDNSSocket, 0, FREERTOS_SO_RCVTIMEO, &uxReadTimeOut_ticks,  sizeof( TickType_t ) );

		for( xAttempt = 0; xAttempt < ipconfigDNS_REQUEST_ATTEMPTS; xAttempt++ )
		{
		size_t uxHeaderBytes;
		NetworkBufferDescriptor_t *pxNetworkBuffer;
			/* Get a buffer.  This uses a maximum delay, but the delay will be
			capped to ipconfigUDP_MAX_SEND_BLOCK_TIME_TICKS so the return value
			still needs to be tested. */
			configPRINTF( ( "DNS lookup: %s\n", pcHostName ) );

			uxHeaderBytes = ipSIZE_OF_ETH_HEADER + ipSIZE_OF_IPv4_HEADER + ipSIZE_OF_UDP_HEADER;

			pxNetworkBuffer = pxGetNetworkBufferWithDescriptor( uxHeaderBytes + uxExpectedPayloadLength, 0uL );
			if( pxNetworkBuffer != NULL )
			{
				pucUDPPayloadBuffer = &( pxNetworkBuffer->pucEthernetBuffer[ uxHeaderBytes ] );
				pucUDPPayloadBuffer[ - ( ( int ) ipUDP_PAYLOAD_IP_TYPE_OFFSET ) ] = ( uint8_t ) ipTYPE_IPv4;
			}

			if( pucUDPPayloadBuffer != NULL )
			{
				/* Create the message in the obtained buffer. */
				uxPayloadLength = prvCreateDNSMessage( pucUDPPayloadBuffer, pcHostName, uxIdentifier, uxHostType );

				iptraceSENDING_DNS_REQUEST();

				/* Send the DNS message. */
#if( ipconfigUSE_LLMNR == 1 )
				if( bHasDot == pdFALSE )
				{
					/* Use LLMNR addressing. */
					( ipPOINTER_CAST( DNSMessage_t *, pucUDPPayloadBuffer ) ) -> usFlags = 0;
					xAddress.sin_addr = ipLLMNR_IP_ADDR;	/* Is in network byte order. */
					xAddress.sin_port = ipLLMNR_PORT;
					xAddress.sin_port = FreeRTOS_ntohs( xAddress.sin_port );
				}
				else
#endif
				{
				NetworkEndPoint_t *pxEndPoint;

					ulIPAddress = 0uL;
					/* Obtain the DNS server address. */
					for( pxEndPoint = FreeRTOS_FirstEndPoint( NULL );
						pxEndPoint != NULL;
						pxEndPoint = FreeRTOS_NextEndPoint( NULL, pxEndPoint ) )
					{
						/* lint e506 e774 : the macro 'ENDPOINT_IS_IPv4' may return a const
						has a const value when IPv6 is not enabled. */
						if( ENDPOINT_IS_IPv4( pxEndPoint ) )
						{
							FreeRTOS_GetAddressConfiguration( pxEndPoint, NULL, NULL, NULL, &ulIPAddress );
							if( ( ulIPAddress != ( uint32_t )0u ) && ( ulIPAddress != ~( uint32_t )0u ) )
							{
								break;	/*lint !e9011: (Note -- more than one 'break' terminates loop [MISRA 2012 Rule 15.4, advisory]. */
							}
						}
					}
					/* Use DNS server. */
					xAddress.sin_len = ( uint8_t ) sizeof( xAddress );
					xAddress.sin_family = FREERTOS_AF_INET;
					xAddress.sin_addr = ulIPAddress;
					xAddress.sin_port = dnsDNS_PORT;
				}

				ulIPAddress = 0UL;

				if( FreeRTOS_sendto( xDNSSocket, pucUDPPayloadBuffer, uxPayloadLength, FREERTOS_ZERO_COPY, &xAddress, sizeof( xAddress ) ) != 0 )
				{
					/* Wait for the reply. */
					lBytes = FreeRTOS_recvfrom( xDNSSocket, &pucReceiveBuffer, 0, FREERTOS_ZERO_COPY, &xAddress, &ulAddressLength );

					if( lBytes > 0 )
					{
					BaseType_t xExpected;
					DNSMessage_t *pxDNSMessageHeader = ipPOINTER_CAST( DNSMessage_t *, pucReceiveBuffer );

						/* See if the identifiers match. */
						if( uxIdentifier == ( TickType_t ) pxDNSMessageHeader->usIdentifier )
						{
							xExpected = pdTRUE;
						}
						else
						{
							/* The reply was not expected. */
							xExpected = pdFALSE;
						}

						/* The reply was received.  Process it. */
					#if( ipconfigDNS_USE_CALLBACKS == 0 )
						/* It is useless to analyse the unexpected reply
						unless asynchronous look-ups are enabled. */
						if( xExpected != pdFALSE )
					#endif /* ipconfigDNS_USE_CALLBACKS == 0 */
						{
							ulIPAddress = prvParseDNSReply( pucReceiveBuffer, ( size_t ) lBytes, pxAddress, xExpected );
						}

						/* Finished with the buffer.  The zero copy interface
						is being used, so the buffer must be freed by the
						task. */
						FreeRTOS_ReleaseUDPPayloadBuffer( pucUDPPayloadBuffer );
						//vReleaseNetworkBufferAndDescriptor( pxNetworkBuffer );

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
					//FreeRTOS_ReleaseUDPPayloadBuffer( pucUDPPayloadBuffer );
					vReleaseNetworkBufferAndDescriptor( pxNetworkBuffer );
				}
			}

			if( uxReadTimeOut_ticks == 0u )
			{
				/* This DNS lookup is asynchronous, using a call-back:
				send the request only once. */
				break;	/*lint !e9011 more than one 'break' terminates loop [MISRA 2012 Rule 15.4, advisory] */
			}
		}

		/* Finished with the socket. */
		( void ) FreeRTOS_closesocket( xDNSSocket );
	}

	return ulIPAddress;
}
/*-----------------------------------------------------------*/

static size_t prvCreateDNSMessage( uint8_t *pucUDPPayloadBuffer,
								   const char *pcHostName,
								   TickType_t uxIdentifier,
								   UBaseType_t uxHostType )
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
	memcpy( pucUDPPayloadBuffer, &xDefaultPartDNSHeader, sizeof( xDefaultPartDNSHeader ) );

	/* Write in a unique identifier. */
	pxDNSMessageHeader = ipPOINTER_CAST( DNSMessage_t *, pucUDPPayloadBuffer );
	pxDNSMessageHeader->usIdentifier = ( uint16_t ) uxIdentifier;

	/* Create the resource record at the end of the header.  First
	find the end of the header. */
	pucStart = &( pucUDPPayloadBuffer[ sizeof( xDefaultPartDNSHeader ) ] );

	/* Leave a gap for the first length bytes. */
	pucByte = &( pucStart[ 1 ] );

	/* Copy in the host name. */
	strcpy( ( char * ) pucByte, pcHostName );

	/* Mark the end of the string. */
	pucByte = &( pucByte[ strlen( pcHostName ) ] );
	*pucByte = 0x00u;

	/* Walk the string to replace the '.' characters with byte counts.
	pucStart holds the address of the byte count.  Walking the string
	starts after the byte count position. */
	pucByte = pucStart;

	do
	{
		pucByte++;

		while( ( *pucByte != ( uint8_t ) 0u ) && ( *pucByte != ( uint8_t ) '.' ) )
		{
			pucByte++;
		}

		/* Fill in the byte count, then move the pucStart pointer up to
		the found byte position. */
		*pucStart = ( uint8_t ) ( ( uint32_t ) pucByte - ( uint32_t ) pucStart );/*lint !e9078 !e923 */
		( *pucStart )--;

		pucStart = pucByte;
	} while( *pucByte != ( uint8_t ) 0u );

	/* Finish off the record. */

	pxTail = ipPOINTER_CAST(DNSTail_t *, &( pucByte[ 1 ] ) );

	#ifdef _lint
	( void ) pxTail;
	( void ) uxHostType;
	#else
	vSetField16( pxTail, DNSTail_t, usType, ( uint16_t ) uxHostType );	/*lint !e78 !e830 !e40 !e835 !e572 !e778 !e530 !e830*/  /* Type A: host */
	vSetField16( pxTail, DNSTail_t, usClass, dnsCLASS_IN );		/*lint !e78 !e830 !e40 !e835 !e572 !e778 !e530 !e830*/  /* 1: Class IN */
	#endif

	/* Return the total size of the generated message, which is the space from
	the last written byte to the beginning of the buffer. */
	return ( ( uint32_t ) pucByte - ( uint32_t ) pucUDPPayloadBuffer + 1u ) + sizeof( DNSTail_t );/*lint !e9078 !e923 !e834 !e946 !e947 !e438 !e830 */
}
/*-----------------------------------------------------------*/

#if( ipconfigUSE_DNS_CACHE == 1 ) || ( ipconfigDNS_USE_CALLBACKS == 1 )

	static size_t prvReadNameField( uint8_t *pucByte,
									size_t uxSourceLen,
									char *pcName,
									size_t uxDestLen )
	{
	size_t uxNameLen = 0;
	size_t uxCount;
	size_t uxIndex = 0;

		if( uxSourceLen == ( size_t ) 0u )
		{
			/* Return 0 value in case of error. */
			uxIndex = 0;
		}
		/* Determine if the name is the fully coded name, or an offset to the name
		elsewhere in the message. */
		else if( ( pucByte[ uxIndex ] & dnsNAME_IS_OFFSET ) == dnsNAME_IS_OFFSET )
		{
			/* Jump over the two byte offset. */
			if( uxSourceLen > sizeof( uint16_t ) )
			{
				uxIndex += sizeof( uint16_t );
			}
			else
			{
				uxIndex = 0;
			}
		}
		else
		{
			/* 'uxIndex' points to the full name. Walk over the string. */
			while( ( uxIndex < ( uxSourceLen - 1u ) ) && ( pucByte[ uxIndex ] != ( uint8_t )0x00u ) )
			{
				/* If this is not the first time through the loop, then add a
				separator in the output. */
				if( ( uxNameLen > 0u ) && ( uxNameLen < ( uxDestLen - 1u ) ) )
				{
					pcName[ uxNameLen ] = '.';
					uxNameLen++;
				}

				/* Process the first/next sub-string. */
				uxCount = ( size_t ) pucByte[ uxIndex ];
				uxSourceLen--;
				uxIndex++;
				while( ( uxCount-- != 0u ) && ( uxSourceLen > 1u ) )
				{
					if( uxNameLen < ( uxDestLen - 1u ) )
					{
						pcName[ uxNameLen ] = ( char ) pucByte[ uxIndex ];
						uxNameLen++;
					}
					else
					{
						/* DNS name is too big for the provided buffer. */
						uxIndex = 0;
						break;
					}
					uxIndex++;
					uxSourceLen--;
				}
			}

			/* Confirm that a fully formed name was found. */
			if( uxIndex > 0u )
			{
				if( pucByte[ uxIndex ] == 0u )
				{
					uxIndex++;
					pcName[ uxNameLen ] = '\0';
				}
				else
				{
					uxIndex = 0;
				}
			}
		}

		return uxIndex;
	}
#endif	/* ipconfigUSE_DNS_CACHE || ipconfigDNS_USE_CALLBACKS */
/*-----------------------------------------------------------*/

static size_t prvSkipNameField( uint8_t *pucByte,
								size_t uxSourceLen )
{
	size_t uxChunkLength;
	size_t uxIndex = 0;

	if( uxSourceLen == 0u )
	{
		uxIndex = 0;
	}
	/* Determine if the name is the fully coded name, or an offset to the name
	elsewhere in the message. */
	else if( ( pucByte[ uxIndex ] & dnsNAME_IS_OFFSET ) == dnsNAME_IS_OFFSET )
	{
		/* Jump over the two byte offset. */
		if( uxSourceLen > sizeof( uint16_t ) )
		{
			uxIndex += sizeof( uint16_t );
		}
		else
		{
			uxIndex = 0;
		}
	}
	else
	{
		/* pucByte points to the full name. Walk over the string. */
		while( ( pucByte[ uxIndex ] != 0u ) && ( uxSourceLen > 1u ) )
		{
			uxChunkLength = ( size_t ) pucByte[ uxIndex ] + 1u;

			if( uxSourceLen > uxChunkLength )
			{
				uxSourceLen -= uxChunkLength;
				uxIndex += uxChunkLength;
			}
			else
			{
				uxIndex = 0;
				break;
			}
		}

		/* Confirm that a fully formed name was found. */
		if( uxIndex > 0u )
		{
			if( 0u == pucByte[ uxIndex ] )
			{
				uxIndex++;
			}
			else
			{
				uxIndex = 0;
			}
		}
	}

	return uxIndex;
}
/*-----------------------------------------------------------*/

#if( ipconfigDNS_USE_CALLBACKS == 1 ) || ( ipconfigUSE_LLMNR == 1 )
	uint32_t ulDNSHandlePacket( NetworkBufferDescriptor_t *pxNetworkBuffer )
	{
	DNSMessage_t *pxDNSMessageHeader;

		if( pxNetworkBuffer->xDataLength >= sizeof( DNSMessage_t ) )
		{
		#if( ipconfigUSE_IPv6 != 0 )
		struct freertos_sockaddr6 xAddress;
		#else
		struct freertos_sockaddr xAddress;
		#endif

			configPRINTF( ( "ulDNSHandlePacket: received reply\n" ) );
			memset( &xAddress, 0, sizeof xAddress );
			pxDNSMessageHeader =
				ipPOINTER_CAST( DNSMessage_t *, &( pxNetworkBuffer->pucEthernetBuffer [ sizeof( UDPPacket_t ) ] ) );

			( void ) prvParseDNSReply( ( uint8_t * )pxDNSMessageHeader,
									   pxNetworkBuffer->xDataLength,
									   ipPOINTER_CAST( struct freertos_sockaddr *, &( xAddress ) ),
									   pdFALSE );
		}

		/* The packet was not consumed. */
		return pdFAIL;
	}
	/*-----------------------------------------------------------*/
#endif /* if ( ipconfigDNS_USE_CALLBACKS == 1 ) || ( ipconfigUSE_LLMNR == 1 ) */

#if( ipconfigUSE_NBNS == 1 )

	uint32_t ulNBNSHandlePacket (NetworkBufferDescriptor_t *pxNetworkBuffer )
	{
	UDPPacket_t *pxUDPPacket = ( UDPPacket_t * ) pxNetworkBuffer->pucEthernetBuffer;
	uint8_t *pucUDPPayloadBuffer = pxNetworkBuffer->pucEthernetBuffer + sizeof( *pxUDPPacket );

		prvTreatNBNS( pucUDPPayloadBuffer,
					  pxNetworkBuffer->xDataLength,
					  pxUDPPacket->xIPHeader.ulSourceIPAddress );

		/* The packet was not consumed. */
		return pdFAIL;
	}

#endif /* ipconfigUSE_NBNS */
/*-----------------------------------------------------------*/

static NetworkEndPoint_t *prvFindEndPointOnNetMask( NetworkBufferDescriptor_t *pxNetworkBuffer )
{
NetworkEndPoint_t *pxEndPoint;

#if( ipconfigUSE_IPv6 != 0 )
	IPPacket_IPv6_t *xIPPacket_IPv6 = ipPOINTER_CAST( IPPacket_IPv6_t *, pxNetworkBuffer->pucEthernetBuffer );

	if( xIPPacket_IPv6->xEthernetHeader.usFrameType == ipIPv6_FRAME_TYPE )
	{
		pxEndPoint = FreeRTOS_FindEndPointOnNetMask_IPv6( &xIPPacket_IPv6->xIPHeader.xSourceAddress );
	}
	else					
#endif
	{
	IPPacket_t *xIPPacket = ipPOINTER_CAST( IPPacket_t *, pxNetworkBuffer->pucEthernetBuffer );

		pxEndPoint = FreeRTOS_FindEndPointOnNetMask( xIPPacket->xIPHeader.ulSourceIPAddress, 6 );
	}

	if( pxEndPoint != NULL )
	{
		pxNetworkBuffer->pxEndPoint = pxEndPoint;
	}

	return pxEndPoint;
}
/*-----------------------------------------------------------*/

static uint32_t prvParseDNSReply( uint8_t *pucUDPPayloadBuffer, size_t uxBufferLength, struct freertos_sockaddr *pxAddress, BaseType_t xExpected )
{
DNSMessage_t *pxDNSMessageHeader;
DNSAnswerRecord_t *pxDNSAnswerRecord;
uint8_t *pucNewBuffer;
uint32_t ulIPAddress = 0UL;
#if( ipconfigUSE_LLMNR == 1 )
	char *pcRequestedName = NULL;
#endif
uint8_t *pucByte;
size_t uxSourceBytesRemaining;
uint16_t x, usDataLength, usQuestions;
BaseType_t xDoStore = xExpected;
uint16_t usType = 0, usClass = 0;
#if( ipconfigUSE_DNS_CACHE == 1 ) || ( ipconfigDNS_USE_CALLBACKS == 1 )
	char pcName[ ipconfigDNS_CACHE_NAME_LENGTH ] = "";
#endif
IPv46_Address_t xIP_Address;

	/* Ensure that the buffer is of at least minimal DNS message length. */
	if( uxBufferLength < sizeof( DNSMessage_t ) )
	{
		return dnsPARSE_ERROR;	/*lint !e904 Return statement before end of function */
	}
	uxSourceBytesRemaining = uxBufferLength;

	/* Parse the DNS message header. */
	pxDNSMessageHeader = ipPOINTER_CAST( DNSMessage_t *, pucUDPPayloadBuffer );

	/* Introduce a do {} while (0) to allow the use of breaks. */
	do
	{
	size_t uxBytesRead = 0;
	size_t uxResult;
		/* Start at the first byte after the header. */
		pucByte = &( pucUDPPayloadBuffer [ sizeof( DNSMessage_t ) ] );
		uxSourceBytesRemaining -= sizeof( DNSMessage_t );

		/* Skip any question records. */
		usQuestions = FreeRTOS_ntohs( pxDNSMessageHeader->usQuestions );
		for( x = 0; x < usQuestions; x++ )
		{
			#if( ipconfigUSE_LLMNR == 1 )
			{
				if( x == 0u )
				{
					pcRequestedName = ( char * ) pucByte;
				}
			}
			#endif

#if( ipconfigUSE_DNS_CACHE == 1 ) || ( ipconfigDNS_USE_CALLBACKS == 1 )
			if( x == 0u )
			{
				uxResult = prvReadNameField( pucByte,
											uxSourceBytesRemaining,
											pcName,
											sizeof( pcName ) );

				/* Check for a malformed response. */
				if( uxResult == 0u )
				{
					return dnsPARSE_ERROR;/*lint !e904 Return statement before end of function */
				}
				uxBytesRead += uxResult;
				pucByte = &( pucByte[ uxResult ] );
				uxSourceBytesRemaining -= uxResult;
			}
			else
#endif /* ipconfigUSE_DNS_CACHE || ipconfigDNS_USE_CALLBACKS */
			{
				/* Skip the variable length pcName field. */
				uxResult = prvSkipNameField( pucByte,
											uxSourceBytesRemaining );

				/* Check for a malformed response. */
				if( uxResult == 0u )
				{
					return dnsPARSE_ERROR;/*lint !e904 Return statement before end of function */
				}
				uxBytesRead += uxResult;
				pucByte = &( pucByte[ uxResult ] );
				uxSourceBytesRemaining -= uxResult;
			}

			/* Check the remaining buffer size. */
			if( uxSourceBytesRemaining >= sizeof( uint32_t ) )
			{
				/* usChar2u16 returns value in host endianness. */
				usType = usChar2u16( pucByte );
				usClass = usChar2u16( &( pucByte[ 2 ] ) );

				/* Skip the type and class fields. */
				pucByte = &( pucByte[ sizeof( uint32_t ) ] );
				uxSourceBytesRemaining -= sizeof( uint32_t );
			}
			else
			{
				/* Malformed response. */
				return dnsPARSE_ERROR;/*lint !e904 Return statement before end of function */
			}
		}

		/* Search through the answer records. */
		pxDNSMessageHeader->usAnswers = FreeRTOS_ntohs( pxDNSMessageHeader->usAnswers );

		if( ( pxDNSMessageHeader->usFlags & dnsRX_FLAGS_MASK ) == dnsEXPECTED_RX_FLAGS )
		{
			for( x = 0; x < pxDNSMessageHeader->usAnswers; x++ )
			{
			BaseType_t xDoAccept;
			size_t uxAddressLength;

				uxResult = prvSkipNameField( pucByte,
											uxSourceBytesRemaining );

				/* Check for a malformed response. */
				if( uxResult == 0u )
				{
					return dnsPARSE_ERROR;/*lint !e904 */
				}
				uxBytesRead += uxResult;
				pucByte = &( pucByte[ uxResult ] );
				uxSourceBytesRemaining -= uxResult;


				/* Is there enough data for an IPv4 A record answer and, if so,
				is this an A record? */
				usType = usChar2u16( pucByte );

				#if( ipconfigUSE_IPv6 != 0 )
				if( usType == ( uint16_t ) dnsTYPE_AAAA_HOST )
				{
					uxAddressLength = ipSIZE_OF_IPv6_ADDRESS;
					if( uxSourceBytesRemaining >= ( sizeof( DNSAnswerRecord_t ) + uxAddressLength ) )
					{
						xDoAccept = pdTRUE;
					}
					else
					{
						xDoAccept = pdFALSE;
					}
				}
				else
				#endif	/* #if( ipconfigUSE_IPv6 != 0 ) */
				if( usType == ( uint16_t ) dnsTYPE_A_HOST )
				{
					uxAddressLength = ipSIZE_OF_IPv4_ADDRESS;
					if( uxSourceBytesRemaining >= ( sizeof( DNSAnswerRecord_t ) + uxAddressLength ) )
					{
						xDoAccept = pdTRUE;
					}
					else
					{
						xDoAccept = pdFALSE;
					}
				}
				else
				{
					xDoAccept = pdFALSE;
				}
				if( xDoAccept != pdFALSE )
				{
					/* This is the required record type and is of sufficient size. */
					pxDNSAnswerRecord = ipPOINTER_CAST( DNSAnswerRecord_t *, pucByte );

					/* Sanity check the data length of an IPv4 answer. */
					/* lint: uxAddressLength is defined because xDoAccept = true. */
					if( FreeRTOS_ntohs( pxDNSAnswerRecord->usDataLength ) == ( uint16_t ) uxAddressLength )/*lint !e644 Variable 'uxAddressLength' (line 1380) may not have been initialized [MISRA 2012 Rule 9.1, mandatory]). */
					{
						/* Copy the IP address out of the record. */
						#if( ipconfigUSE_IPv6 != 0 )
						if( usType == ( uint16_t ) dnsTYPE_AAAA_HOST )
						{
							memcpy( xIP_Address.u.xAddress_IPv6.ucBytes,
									&( pucByte [ sizeof( DNSAnswerRecord_t ) ] ),
									ipSIZE_OF_IPv6_ADDRESS );
							if( pxAddress != NULL )
							{
							struct freertos_sockaddr6 *pxSockaddr6 = ipPOINTER_CAST( struct freertos_sockaddr6 *, pxAddress );

								memcpy( pxSockaddr6->sin_addrv6.ucBytes,
										&( pucByte [ sizeof( DNSAnswerRecord_t ) ] ),
										ipSIZE_OF_IPv6_ADDRESS );
							}
							xIP_Address.ucIs_IPv6 = ( uint8_t ) 1u;
							ulIPAddress = 1uL;
						}
						else
						#endif	/* ( ipconfigUSE_IPv6 != 0 ) */
						{
							memcpy( &ulIPAddress,
									&( pucByte [ sizeof( DNSAnswerRecord_t ) ] ),
									ipSIZE_OF_IPv4_ADDRESS );
							if( pxAddress != NULL )
							{
								pxAddress->sin_addr = ulIPAddress;
							}
							xIP_Address.u.ulIPAddress = ulIPAddress;
						#if( ipconfigUSE_IPv6 != 0 )
							xIP_Address.ucIs_IPv6 = ( uint8_t ) 0u;
						#endif
						}

						#if( ipconfigDNS_USE_CALLBACKS == 1 )
						{
						BaseType_t xCallbackResult;
							#if( ipconfigUSE_IPv6 != 0 )
							{
								xCallbackResult = xDNSDoCallback( ( TickType_t ) pxDNSMessageHeader->usIdentifier, pcName, ipPOINTER_CAST( struct freertos_sockaddr6 *, pxAddress ) );
							}
							#else
							{
								xCallbackResult = xDNSDoCallback( ( TickType_t ) pxDNSMessageHeader->usIdentifier, pcName, ulIPAddress );
							}
							#endif
							/* See if any asynchronous call was made to FreeRTOS_gethostbyname_a() */
							if( xCallbackResult != pdFALSE )
							{
								/* This device has requested this DNS look-up.
								The result may be stored in the DNS cache. */
								xDoStore = pdTRUE;
							}
						}
						#endif	/* ipconfigDNS_USE_CALLBACKS == 1 */
						#if( ipconfigUSE_DNS_CACHE == 1 )
						{
							/* The reply will only be stored in the DNS cache when the
							request was issued by this device. */
							if( xDoStore != pdFALSE )
							{
								( void ) prvProcessDNSCache( pcName, &xIP_Address, pxDNSAnswerRecord->ulTTL, pdFALSE );
							}
							#if( ipconfigUSE_IPv6 != 0 )
							if( usType == ( uint16_t ) dnsTYPE_AAAA_HOST )
							{
								FreeRTOS_printf( ( "DNS[0x%04X]: The answer to '%s' (%pip) has%s been stored\n",
									( unsigned )pxDNSMessageHeader->usIdentifier,
									pcName,
									xIP_Address.u.xAddress_IPv6.ucBytes,
									( xDoStore != 0 ) ? "" : " NOT" ) );
								/* return non-zero. */
								//ulIPAddress = 1uL;
							}
							else
							#endif
							{
								FreeRTOS_printf( ( "DNS[0x%04X]: The answer to '%s' (%xip) has%s been stored\n",
									( unsigned )pxDNSMessageHeader->usIdentifier,
									pcName,
									( unsigned )FreeRTOS_ntohl( ulIPAddress ),
									( xDoStore != 0 ) ? "" : " NOT" ) );
							}

						}
						#endif /* ipconfigUSE_DNS_CACHE */
					}

					pucByte = &( pucByte[ sizeof( DNSAnswerRecord_t ) + uxAddressLength ] );
					uxSourceBytesRemaining -= ( sizeof( DNSAnswerRecord_t ) + uxAddressLength );
					break;
				}
				else if( uxSourceBytesRemaining >= sizeof( DNSAnswerRecord_t ) )
				{
					/* It's not an A or an AAAA record, so skip it. Get the header location
					and then jump over the header. */
					pxDNSAnswerRecord = ipPOINTER_CAST( DNSAnswerRecord_t *, pucByte );
					pucByte = &( pucByte[ sizeof( DNSAnswerRecord_t ) ] );
					uxSourceBytesRemaining -= sizeof( DNSAnswerRecord_t );

					/* Determine the length of the answer data from the header. */
					usDataLength = FreeRTOS_ntohs( pxDNSAnswerRecord->usDataLength );

					/* Jump over the answer. */
					if( uxSourceBytesRemaining >= usDataLength )
					{
						pucByte = &( pucByte[ usDataLength ] );
						uxSourceBytesRemaining -= usDataLength;
					}
					else
					{
						/* Malformed response. */
						return dnsPARSE_ERROR;/*lint !e904 Return statement before end of function [MISRA 2012 Rule 15.5, advisory]) */
					}
				}
				else
				{
					/* End has been reached. */
				}
			}
		}
#if( ipconfigUSE_LLMNR == 1 )
		else if( ( usQuestions != ( uint16_t )0u ) && ( ( usType == ( uint16_t )dnsTYPE_A_HOST ) || ( usType == dnsTYPE_AAAA_HOST ) ) && ( usClass == ( uint16_t )dnsCLASS_IN ) )
		{
		/* Find the descriptor located before the payload in the buffer. */
		NetworkBufferDescriptor_t *pxNetworkBuffer;
		NetworkEndPoint_t *pxEndPoint, xEndPoint;
		int16_t usLength;
		LLMNRAnswer_t *pxAnswer;
		size_t uxDataLength;
		size_t uxExtraLength;
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
				break;	/*lint !e9011 more than one 'break' terminates loop [MISRA 2012 Rule 15.4, advisory]. */
			}
			if( pxNetworkBuffer->pxEndPoint == NULL )
			{
				/* NetworkInterface is obliged to set 'pxEndPoint' in every received packet,
				but in case this has not be done, set it here. */

				pxNetworkBuffer->pxEndPoint = prvFindEndPointOnNetMask( pxNetworkBuffer );
				#ifndef _lint
				FreeRTOS_printf( ( "prvParseDNSReply: No pxEndPoint yet? Using %lxip\n",
					FreeRTOS_ntohl( pxNetworkBuffer->pxEndPoint ? pxNetworkBuffer->pxEndPoint->ipv4_settings.ulIPAddress : 0uL ) ) );
				#endif

				if( pxNetworkBuffer->pxEndPoint == NULL )
				{
					break;	/*lint !e9011 more than one 'break' terminates loop [MISRA 2012 Rule 15.4, advisory]. */
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

			if( pcRequestedName != NULL )
			{
				if( xApplicationDNSQueryHook ( &xEndPoint, &( pcRequestedName [ 1 ] ) ) == pdFALSE )
				{
					/* This LLMNR request needs no reply from this unit. */
					break;	/*lint !e9011 */
				}
			}

			/* Using 'ipSIZE_OF_IPv4_HEADER' because all LLMNR/DNS packets are transmitted in IPv4 for now. */
			uxDataLength = sizeof( EthernetHeader_t ) + ipSIZE_OF_IPv4_HEADER + sizeof( UDPHeader_t ) + pxNetworkBuffer->xDataLength;

			#if( ipconfigUSE_IPv6 != 0 )
			if( usType == dnsTYPE_AAAA_HOST )
			{
				uxExtraLength = sizeof( LLMNRAnswer_t ) + ipSIZE_OF_IPv6_ADDRESS - sizeof( pxAnswer->ulIPAddress );
			}
			else
			#endif
			{
				uxExtraLength = sizeof( LLMNRAnswer_t );
			}

			/* The field xDataLength was set to the length of the UDP
			payload.  The answer (reply) will be longer than the
			request, so the packet must be resized. */
			uxOffsets[ 0 ] = ( size_t ) ( pucUDPPayloadBuffer - pxNetworkBuffer->pucEthernetBuffer );/*lint !e9033 !e946 !e947 !e613 !e831 */
			uxOffsets[ 1 ] = ( size_t ) ( pcRequestedName - ( ( char * ) pxNetworkBuffer->pucEthernetBuffer ) );/*lint !e9033 !e946 !e947 !e613 !e831 */
			uxOffsets[ 2 ] = ( size_t ) ( pucByte - pxNetworkBuffer->pucEthernetBuffer );/*lint !e9033 !e946 !e947 !e613 !e831 */

			/* Restore the 'xDataLength' field. */
			pxNetworkBuffer->xDataLength = uxDataLength;
			pxNetworkBuffer = pxResizeNetworkBufferWithDescriptor( pxNetworkBuffer, uxDataLength + uxExtraLength );

			if( pxNetworkBuffer == NULL )
			{
				break;	/*lint !e9011 */
			}
			pucNewBuffer = &( pxNetworkBuffer->pucEthernetBuffer [ ( BaseType_t ) uxOffsets[ 0 ] ] );
			pcRequestedName = ( char * ) &( pxNetworkBuffer->pucEthernetBuffer [ uxOffsets[ 1 ] ] );
			pucByte = &( pxNetworkBuffer->pucEthernetBuffer [ uxOffsets[ 2 ] ] );
			pxDNSMessageHeader = ipPOINTER_CAST( DNSMessage_t *, pucNewBuffer );

			pxAnswer = ipPOINTER_CAST( LLMNRAnswer_t *, pucByte );

			/* Leave 'usIdentifier' and 'usQuestions' untouched. */

			#ifndef _lint
			vSetField16( pxDNSMessageHeader, DNSMessage_t, usFlags, dnsLLMNR_FLAGS_IS_REPONSE );/* Set the response flag */
			vSetField16( pxDNSMessageHeader, DNSMessage_t, usAnswers, 1 );	/* Provide a single answer */
			vSetField16( pxDNSMessageHeader, DNSMessage_t, usAuthorityRRs, 0 );	/* No authority */
			vSetField16( pxDNSMessageHeader, DNSMessage_t, usAdditionalRRs, 0 );	/* No additional info */
			#endif /* lint */
			
			pxAnswer->ucNameCode = dnsNAME_IS_OFFSET;
			pxAnswer->ucNameOffset = ( uint8_t )( pcRequestedName - ( char * ) pucNewBuffer );	/*lint !e9033 !e946 !e947 */

			#ifndef _lint
			vSetField16( pxAnswer, LLMNRAnswer_t, usType, usType );	/* Type A: host */
			vSetField16( pxAnswer, LLMNRAnswer_t, usClass, dnsCLASS_IN );	/* 1: Class IN */
			vSetField32( pxAnswer, LLMNRAnswer_t, ulTTL, dnsLLMNR_TTL_VALUE );
			#endif /* lint */

			#if( ipconfigUSE_IPv6 != 0 )
			if( usType == dnsTYPE_AAAA_HOST )
			{
			size_t uxDistance;
			NetworkEndPoint_t *pxReplyEndpoint = FreeRTOS_FirstEndPoint_IPv6( NULL );
				if( pxReplyEndpoint == NULL )
				{
					break;	/*lint !e9011 */
				}
				#ifndef _lint
				vSetField16( pxAnswer, LLMNRAnswer_t, usDataLength, ipSIZE_OF_IPv6_ADDRESS );
				#endif /* lint */
				memcpy( &( pxAnswer->ulIPAddress ), pxReplyEndpoint->ipv6_settings.xIPAddress.ucBytes, ipSIZE_OF_IPv6_ADDRESS );/*lint !e419 Warning -- Apparent data overrun for function , argument 3 (size=16) exceeds argument 1 (size=4) [MISRA 2012 Rule 1.3, required]). */
				FreeRTOS_printf( ( "LLMNR return IPv6 %pip\n", pxReplyEndpoint->ipv6_settings.xIPAddress.ucBytes ) );
				uxDistance = ( size_t ) ( pucByte - pucNewBuffer );/*lint !e9033 !e946, !e947 */
				usLength = ipNUMERIC_CAST( int16_t, sizeof( *pxAnswer ) + uxDistance + ipSIZE_OF_IPv6_ADDRESS - sizeof( pxAnswer->ulIPAddress ) );
			}
			else
			#endif
			{
				FreeRTOS_printf( ( "LLMNR return IPv4 %lxip\n", FreeRTOS_ntohl( xEndPoint.ipv4_settings.ulIPAddress ) ) );
				#ifndef _lint
				vSetField16( pxAnswer, LLMNRAnswer_t, usDataLength, sizeof( pxAnswer->ulIPAddress ) );
				//vSetField32( pxAnswer, LLMNRAnswer_t, ulIPAddress, FreeRTOS_ntohl( pxEndPoint->ulIPAddress ) );
				vSetField32( pxAnswer, LLMNRAnswer_t, ulIPAddress, FreeRTOS_ntohl( xEndPoint.ipv4_settings.ulIPAddress ) );
				#endif /* lint */
				
				usLength = ( int16_t ) ( sizeof( *pxAnswer ) + ( size_t ) ( pucByte - pucNewBuffer ) );	/*lint !e9033 !e946 !e947 */
			}
			#if( ipconfigUSE_IPv6 == 0 )
			if( usType == dnsTYPE_A_HOST )	/*lint !e774 */
			#else
			if( ( usType == dnsTYPE_A_HOST ) || ( usType == dnsTYPE_AAAA_HOST ) )
			#endif /* ipconfigUSE_IPv6 */
			{
				prvReplyDNSMessage( pxNetworkBuffer, usLength );
			}
		}
		else
		{
			/* Not an expected reply. */
		}
		/* Did not read it yet. */
		( void ) uxBytesRead;
#endif /* ipconfigUSE_LLMNR == 1 */
	} while( 0 );	/*lint !e9036 !e717 (Info -- do ... while(0);) */

	if( xExpected == pdFALSE )
	{
		/* Do not return a valid IP-address in case the reply was not expected. */
		ulIPAddress = 0uL;
	}

	return ulIPAddress;
}
/*-----------------------------------------------------------*/

#if( ipconfigUSE_NBNS == 1 ) /*_RB_ Not yet converted to multi interfaces. */

	static void prvTreatNBNS( uint8_t *pucUDPPayloadBuffer,
							  size_t uxBufferLength,
							  uint32_t ulIPAddress )
	{
	uint16_t usFlags, usType, usClass;
	uint8_t *pucSource, *pucTarget;
	uint8_t ucByte;
	uint8_t ucNBNSName[ 17 ];
	NetworkBufferDescriptor_t *pxNetworkBuffer;

		/* Check for minimum buffer size. */
		if( uxBufferLength < sizeof( NBNSRequest_t ) )
		{
			return;
		}

		/* Read the request flags in host endianness. */
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
				BaseType_t uxDataLength = pxNetworkBuffer->xDataLength + sizeof( UDPHeader_t ) +
					sizeof( EthernetHeader_t ) + sizeof( IPHeader_t );

					/* The field xDataLength was set to the length of the UDP payload.
					The answer (reply) will be longer than the request, so the packet
					must be duplicated into a bigger buffer */
					pxNetworkBuffer->xDataLength = uxDataLength;
					pxNewBuffer = pxDuplicateNetworkBufferWithDescriptor( pxNetworkBuffer, uxDataLength + 16 );

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
Socket_t xSocket;
struct freertos_sockaddr xAddress;
BaseType_t xReturn;

	/* This must be the first time this function has been called.  Create
	the socket. */
	xSocket = FreeRTOS_socket( FREERTOS_AF_INET, FREERTOS_SOCK_DGRAM, FREERTOS_IPPROTO_UDP );

	/* Auto bind the port. */
	xAddress.sin_port = 0u;
	xReturn = FreeRTOS_bind( xSocket, &xAddress, sizeof( xAddress ) );

	/* Check the bind was successful, and clean up if not. */
	if( xReturn != 0 )
	{
		( void ) FreeRTOS_closesocket( xSocket );
		xSocket = NULL;
	}
	else
	{
		/* The send and receive timeouts will be set later on. */
	}

	return xSocket;
}
/*-----------------------------------------------------------*/

#if( ( ipconfigUSE_NBNS == 1 ) || ( ipconfigUSE_LLMNR == 1 ) )

	static void prvReplyDNSMessage( NetworkBufferDescriptor_t *pxNetworkBuffer,
									BaseType_t lNetLength )
	{
	UDPPacket_t *pxUDPPacket;
	IPHeader_t *pxIPHeader;
	UDPHeader_t *pxUDPHeader;
	NetworkEndPoint_t *pxEndPoint = prvFindEndPointOnNetMask( pxNetworkBuffer );
	size_t uxDataLength;

		pxUDPPacket = ipPOINTER_CAST( UDPPacket_t *, pxNetworkBuffer->pucEthernetBuffer );
		pxIPHeader = &pxUDPPacket->xIPHeader;

	#if( ipconfigUSE_IPv6 != 0 )
		if( ( pxIPHeader->ucVersionHeaderLength & 0xf0u ) == 0x60u )
		{
		UDPPacket_IPv6_t *xUDPPacket_IPv6;
		IPHeader_IPv6_t *pxIPHeader_IPv6;

			xUDPPacket_IPv6 = ipPOINTER_CAST( UDPPacket_IPv6_t *,pxNetworkBuffer->pucEthernetBuffer );
			pxIPHeader_IPv6 = &( xUDPPacket_IPv6->xIPHeader );
			pxUDPHeader = &xUDPPacket_IPv6->xUDPHeader;

			pxIPHeader_IPv6->usPayloadLength = FreeRTOS_htons( ( uint16_t ) lNetLength + ipSIZE_OF_UDP_HEADER );

			{
				memcpy( pxIPHeader_IPv6->xDestinationAddress.ucBytes, pxIPHeader_IPv6->xSourceAddress.ucBytes, ipSIZE_OF_IPv6_ADDRESS );
				memcpy( pxIPHeader_IPv6->xSourceAddress.ucBytes, pxEndPoint->ipv6_settings.xIPAddress.ucBytes, ipSIZE_OF_IPv6_ADDRESS );
			}

			FreeRTOS_printf( ( "DNSreturn to %pip\n", pxEndPoint->ipv6_settings.xIPAddress.ucBytes ) );

			xUDPPacket_IPv6->xUDPHeader.usLength = FreeRTOS_htons( ( uint16_t ) lNetLength + ipSIZE_OF_UDP_HEADER );
			vFlip_16( pxUDPHeader->usSourcePort, pxUDPHeader->usDestinationPort );
			uxDataLength = ( size_t ) lNetLength + ipSIZE_OF_IPv6_HEADER + ipSIZE_OF_UDP_HEADER + ipSIZE_OF_ETH_HEADER;
		}
		else
	#endif /* ipconfigUSE_IPv6 */
		{
			pxUDPHeader = &pxUDPPacket->xUDPHeader;
			/* HT: started using defines like 'ipSIZE_OF_xxx' */
			pxIPHeader->usLength				= FreeRTOS_htons( ( uint16_t ) lNetLength + ipSIZE_OF_IPv4_HEADER + ipSIZE_OF_UDP_HEADER );
			/* HT:endian: should not be translated, copying from packet to packet */
			pxIPHeader->ulDestinationIPAddress	= pxIPHeader->ulSourceIPAddress;
			pxIPHeader->ulSourceIPAddress		= ( pxEndPoint != NULL ) ? pxEndPoint->ipv4_settings.ulIPAddress : 0uL;
			pxIPHeader->ucTimeToLive			= ipconfigUDP_TIME_TO_LIVE;
			pxIPHeader->usIdentification		= FreeRTOS_htons( usPacketIdentifier );
			usPacketIdentifier++;
			pxUDPHeader->usLength				= FreeRTOS_htons( ( uint32_t ) lNetLength + ipSIZE_OF_UDP_HEADER );
			vFlip_16( pxUDPHeader->usSourcePort, pxUDPHeader->usDestinationPort );	/*lint !e717  !e9036  do ... while(0); */
			uxDataLength = ( size_t ) ( ( uint32_t ) lNetLength + ipSIZE_OF_IPv4_HEADER + ipSIZE_OF_UDP_HEADER + ipSIZE_OF_ETH_HEADER );
		}

		#if( ipconfigDRIVER_INCLUDED_TX_IP_CHECKSUM == 0 )
		{
			#if( ipconfigUSE_IPv6 != 0 )
			/* IPv6 IP-headers have no checmsum field. */
			if( ( pxIPHeader->ucVersionHeaderLength & 0xf0u ) != 0x60u )
			#endif
			{
				/* Calculate the IP header checksum. */
				pxIPHeader->usHeaderChecksum	= 0x00u;
				pxIPHeader->usHeaderChecksum	= usGenerateChecksum( 0u, ( uint8_t * ) &( pxIPHeader->ucVersionHeaderLength ), ipSIZE_OF_IPv4_HEADER );
				pxIPHeader->usHeaderChecksum	= ~FreeRTOS_htons( pxIPHeader->usHeaderChecksum );
			}

			/* calculate the UDP checksum for outgoing package */
			( void ) usGenerateProtocolChecksum( ( uint8_t * ) pxUDPPacket, uxDataLength, pdTRUE );
		}
		#endif

		/* Important: tell NIC driver how many bytes must be sent */
		pxNetworkBuffer->xDataLength = uxDataLength;

		/* This function will fill in the eth addresses and send the packet */
		vReturnEthernetFrame( pxNetworkBuffer, pdFALSE );
	}

#endif /* ipconfigUSE_NBNS == 1 || ipconfigUSE_LLMNR == 1 */
/*-----------------------------------------------------------*/

#if( ipconfigUSE_DNS_CACHE == 1 )

	static BaseType_t prvProcessDNSCache( const char *pcName,
										  IPv46_Address_t *pxIP,
										  uint32_t ulTTL,
										  BaseType_t xLookUp )
	{
	BaseType_t x;
	BaseType_t xFound = pdFALSE;
	uint32_t ulCurrentTimeSeconds = ( xTaskGetTickCount() / portTICK_PERIOD_MS ) / 1000UL;

		/* For each entry in the DNS cache table. */
		for( x = 0; x < ipconfigDNS_CACHE_ENTRIES; x++ )
		{
			if( xDNSCache[ x ].pcName[ 0 ] == ( char ) 0 )
			{
				continue;
			}

			if( strcmp( xDNSCache[ x ].pcName, pcName ) == 0 )
			{
				#if( ipconfigUSE_IPv6 != 0 )
				if( pxIP->ucIs_IPv6 == xDNSCache[ x ].xAddr.ucIs_IPv6 )
				#endif	/* ipconfigUSE_IPv6 != 0 */
				{
					/* Is this function called for a lookup or to add/update an IP address? */
					if( xLookUp != pdFALSE )
					{
						/* Confirm that the record is still fresh. */
						if( ulCurrentTimeSeconds < ( xDNSCache[ x ].ulTimeWhenAddedInSeconds + FreeRTOS_ntohl( xDNSCache[ x ].ulTTL ) ) )
						{
							memcpy( pxIP, &( xDNSCache[ x ].xAddr ), sizeof( *pxIP ) );
						}
						else
						{
							/* Age out the old cached record. */
							xDNSCache[ x ].pcName[ 0 ] = ( char ) 0;
						}
					}
					else
					{
						memcpy( &( xDNSCache[ x ].xAddr ), pxIP, sizeof( *pxIP ) );
						xDNSCache[ x ].ulTTL = ulTTL;
						xDNSCache[ x ].ulTimeWhenAddedInSeconds = ulCurrentTimeSeconds;
					}

					xFound = pdTRUE;
					break;
				}
			}
		}

		#warning This logging should be taken away later
		if( xLookUp != 0 )
		{
			#if( ipconfigUSE_IPv6 != 0 )
				FreeRTOS_printf( ( "prvProcessDNSCache: lookup-%d '%s': found = %d\n",
								   ( pxIP->ucIs_IPv6 != 0u ) ? 6 : 4,
								   pcName,
								   ( int ) xFound ) );
			#else
				FreeRTOS_printf( ( "prvProcessDNSCache: lookup-4 '%s': found = %d\n",
								   pcName,
								   ( int ) xFound ) );
			#endif
		}
		else
		{
			#if( ipconfigUSE_IPv6 != 0 )
			if( pxIP->ucIs_IPv6 != 0u )
			{
				FreeRTOS_printf( ( "prvProcessDNSCache: add-6 '%s': %pip: found = %d\n",
								   pcName,
								   pxIP->u.xAddress_IPv6.ucBytes,
								   ( int ) xFound ) );
			}
			else
			#endif
			{
				FreeRTOS_printf( ( "prvProcessDNSCache: add-4 '%s': %lxip: found = %d\n",
								   pcName,
								   FreeRTOS_ntohl( pxIP->u.ulIPAddress ),
								   ( int ) xFound ) );
			}
		}
		if( xFound == pdFALSE )
		{
			if( xLookUp != pdFALSE )
			{
				pxIP->u.ulIPAddress = 0;
			}
			else
			{
				/* Add or update the item. */
				if( strlen( pcName ) < ( size_t ) ipconfigDNS_CACHE_NAME_LENGTH )
				{
					strcpy( xDNSCache[ xFreeEntry ].pcName, pcName );
					memcpy( &( xDNSCache[ xFreeEntry ].xAddr ), pxIP, sizeof( *pxIP ) );
					xDNSCache[ xFreeEntry ].ulTTL = ulTTL;
					xDNSCache[ xFreeEntry ].ulTimeWhenAddedInSeconds = ulCurrentTimeSeconds;

					xFreeEntry++;
					if( xFreeEntry == ipconfigDNS_CACHE_ENTRIES )
					{
						xFreeEntry = 0;
					}
				}
			}
		}

		if( ( xLookUp == 0 ) || ( pxIP->u.ulIPAddress != 0uL ) )
		{
			FreeRTOS_debug_printf( ( "prvProcessDNSCache: %s: '%s' @ %lxip\n", ( xLookUp != 0 ) ? "look-up" : "add", pcName, FreeRTOS_ntohl( pxIP->u.ulIPAddress ) ) );
		}
		return xFound;
	}

#endif /* ipconfigUSE_DNS_CACHE */

#endif /* ipconfigUSE_DNS != 0 */


