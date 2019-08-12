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

/* FreeRTOS includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"

/* FreeRTOS+TCP includes. */
#include "FreeRTOS_IP.h"
#include "FreeRTOS_Sockets.h"
#include "FreeRTOS_IP_Private.h"
#include "FreeRTOS_UDP_IP.h"
#include "FreeRTOS_TCP_IP.h"
#include "FreeRTOS_DHCP.h"
#include "FreeRTOS_ARP.h"
#include "NetworkInterface.h"
#include "NetworkBufferManagement.h"
#include "FreeRTOS_Routing.h"

/* Exclude the entire file if DHCP is not enabled. */
#if( ipconfigUSE_DHCP != 0 )

#if ( ipconfigUSE_DHCP != 0 ) && ( ipconfigNETWORK_MTU < 586 )
	/* DHCP must be able to receive an options field of 312 bytes, the fixed
	part of the DHCP packet is 240 bytes, and the IP/UDP headers take 28 bytes. */
	#error ipconfigNETWORK_MTU needs to be at least 586 to use DHCP
#endif

/* Parameter widths in the DHCP packet. */
#define dhcpCLIENT_HARDWARE_ADDRESS_LENGTH		16
#define dhcpSERVER_HOST_NAME_LENGTH				64
#define dhcpBOOT_FILE_NAME_LENGTH 				128

/* Timer parameters */
#ifndef dhcpINITIAL_DHCP_TX_PERIOD
	#define dhcpINITIAL_TIMER_PERIOD			( pdMS_TO_TICKS( 250 ) )
	#define dhcpINITIAL_DHCP_TX_PERIOD			( pdMS_TO_TICKS( 5000 ) )
#endif

/* Codes of interest found in the DHCP options field. */
#define dhcpIPv4_ZERO_PAD_OPTION_CODE			( 0 )
#define dhcpIPv4_SUBNET_MASK_OPTION_CODE		( 1 )
#define dhcpIPv4_GATEWAY_OPTION_CODE			( 3 )
#define dhcpIPv4_DNS_SERVER_OPTIONS_CODE		( 6 )
#define dhcpIPv4_DNS_HOSTNAME_OPTIONS_CODE		( 12 )
#define dhcpIPv4_REQUEST_IP_ADDRESS_OPTION_CODE	( 50 )
#define dhcpIPv4_LEASE_TIME_OPTION_CODE			( 51 )
#define dhcpIPv4_MESSAGE_TYPE_OPTION_CODE		( 53 )
#define dhcpIPv4_SERVER_IP_ADDRESS_OPTION_CODE	( 54 )
#define dhcpIPv4_PARAMETER_REQUEST_OPTION_CODE	( 55 )
#define dhcpIPv4_CLIENT_IDENTIFIER_OPTION_CODE	( 61 )

/* The four DHCP message types of interest. */
#define dhcpMESSAGE_TYPE_DISCOVER				( 1 )
#define dhcpMESSAGE_TYPE_OFFER					( 2 )
#define dhcpMESSAGE_TYPE_REQUEST				( 3 )
#define dhcpMESSAGE_TYPE_ACK					( 5 )
#define dhcpMESSAGE_TYPE_NACK					( 6 )

/* Offsets into the transmitted DHCP options fields at which various parameters
are located. */
#define dhcpCLIENT_IDENTIFIER_OFFSET			( 5 )
#define dhcpREQUESTED_IP_ADDRESS_OFFSET			( 13 )
#define dhcpDHCP_SERVER_IP_ADDRESS_OFFSET		( 19 )

/* Values used in the DHCP packets. */
#define dhcpREQUEST_OPCODE						( 1 )
#define dhcpREPLY_OPCODE						( 2 )
#define dhcpADDRESS_TYPE_ETHERNET				( 1 )
#define dhcpETHERNET_ADDRESS_LENGTH				( 6 )

#define dhcpIPv6_OPTION_CLIENTID				( 1 )	/* Client Identifier Option */
#define dhcpIPv6_OPTION_SERVERID				( 2 )	/* Server Identifier Option */
#define dhcpIPv6_OPTION_IA_NA					( 3 )	/* Identity Association for Non-temporary Addresses Option */
#define dhcpIPv6_OPTION_IA_TA					( 4 )	/* Identity Association for Temporary Addresses Option */
#define dhcpIPv6_OPTION_IAADDR					( 5 )	/* IA Address Option */
#define dhcpIPv6_OPTION_ORO						( 6 )	/* Option Request Option */
#define dhcpIPv6_OPTION_PREFERENCE				( 7 )	/* Preference Option */
#define dhcpIPv6_OPTION_ELAPSED_TIME			( 8 )	/* Elapsed Time Option */
#define dhcpIPv6_OPTION_RELAY_MSG				( 9 )	/* Relay Message Option */
#define dhcpIPv6_OPTION_AUTH					( 11 )	/* Authentication Option */
#define dhcpIPv6_OPTION_UNICAST					( 12 )	/* Server Unicast Option */
#define dhcpIPv6_OPTION_STATUS_CODE				( 13 )	/* Status Code Option */
#define dhcpIPv6_OPTION_RAPID_COMMIT			( 14 )	/* Rapid Commit Option */
#define dhcpIPv6_OPTION_USER_CLASS				( 15 )	/* User Class Option */
#define dhcpIPv6_OPTION_VENDOR_CLASS			( 16 )	/* Vendor Class Option */
#define dhcpIPv6_OPTION_VENDOR_OPTS				( 17 )	/* Vendor-specific Information Option */
#define dhcpIPv6_OPTION_INTERFACE_ID			( 18 )	/* Interface-Id Option */
#define dhcpIPv6_OPTION_RECONF_MSG				( 19 )	/* Reconfigure Message Option */
#define dhcpIPv6_OPTION_RECONF_ACCEPT			( 20 )	/* Reconfigure Accept Option */
#define dhcpIPv6_OPTION_IA_PD					( 25 )	/* Identity association for prefix delegation

/* If a lease time is not received, use the default of two days. */
/* 48 hours in ticks.  Can not use pdMS_TO_TICKS() as integer overflow can occur. */
#define dhcpDEFAULT_LEASE_TIME					( ( 48UL * 60UL * 60UL ) * configTICK_RATE_HZ )

/* Don't allow the lease time to be too short. */
#define dhcpMINIMUM_LEASE_TIME					( pdMS_TO_TICKS( 60000UL ) )	/* 60 seconds in ticks. */

/* Marks the end of the variable length options field in the DHCP packet. */
#define dhcpOPTION_END_BYTE 0xff

/* Offset into a DHCP message at which the first byte of the options is
located. */
#define dhcpFIRST_OPTION_BYTE_OFFSET			( 0xf0 )

/* When walking the variable length options field, the following value is used
to ensure the walk has not gone past the end of the valid options.  2 bytes is
made up of the length byte, and minimum one byte value. */
#define dhcpMAX_OPTION_LENGTH_OF_INTEREST		( 2L )

/* Standard DHCP port numbers and magic cookie value.
DHCPv6 uses UDP port number  68 for clients and port number  67 for servers.
DHCPv6 uses UDP port number 546 for clients and port number 547 for servers.
*/
#if( ipconfigBYTE_ORDER == pdFREERTOS_LITTLE_ENDIAN )
	#define dhcpCLIENT_PORT_IPv4	0x4400
	#define dhcpSERVER_PORT_IPv4	0x4300
	#define dhcpCLIENT_PORT_IPv6 	0x2202
	#define dhcpSERVER_PORT_IPv6 	0x2302
	#define dhcpCOOKIE				0x63538263
	#define dhcpBROADCAST			0x0080
#else
	#define dhcpCLIENT_PORT_IPv4 	0x0044
	#define dhcpSERVER_PORT_IPv4 	0x0043
	#define dhcpCLIENT_PORT_IPv6 	0x0222
	#define dhcpSERVER_PORT_IPv6 	0x0223
	#define dhcpCOOKIE				0x63825363
	#define dhcpBROADCAST			0x8000
#endif /* ipconfigBYTE_ORDER */

#include "pack_struct_start.h"
struct xDHCPMessage_IPv6
{
	uint8_t ucOpcode;
	uint8_t pucTransactionID[ 3 ];

	uint16_t usClientID;		/*  4 +  2 =  6   0x01 */
	uint16_t usIDLength;		/*  6  + 2 =  8   14 */
	uint8_t pucDUID[ 14 ];		/*  8 + 14 = 22   eg. 00 01 00 01 1c 38 26 2d 08 00 27 fe 8f 95 (where MAC = 08:00:27:fe:8f:95 )*/
	uint16_t usDUIDType;		/* 22 +  2 = 24   eg. 1 = LL address + time */
	uint16_t usHardwareType;	/* 24 +  2 = 26   1 = Ethernet */
	uint16_t ulDUIDTime;		/* 26 +  2 = 28   eg. 0x1c38262d */
	uint8_t pucMacAddress[ 6 ];	/* 28 +  6 = 34   eg. 08:00:27:fe:8f:95 */
	uint8_t ucOptions[ 4 ];		/* 34 ...         A list of options. */
}
#include "pack_struct_end.h"
typedef struct xDHCPMessage_IPv6 DHCPMessage_IPv6_t;

#if( ipconfigDHCP_FALL_BACK_AUTO_IP != 0 )
	/* Define the Link Layer IP address: 169.254.x.x */
	#define LINK_LAYER_ADDRESS_0	169
	#define LINK_LAYER_ADDRESS_1	254

	/* Define the netmask used: 255.255.0.0 */
	#define LINK_LAYER_NETMASK_0	255
	#define LINK_LAYER_NETMASK_1	255
	#define LINK_LAYER_NETMASK_2	0
	#define LINK_LAYER_NETMASK_3	0
#endif

static Socket_t xDHCPSocket;
static BaseType_t xDHCPSocketUserCount;

/*
 * Generate a DHCP discover message and send it on the DHCP socket.
 */
static void prvSendDHCPDiscover( NetworkEndPoint_t *pxEndPoint );

/*
 * Interpret message received on the DHCP socket.
 */
static BaseType_t prvProcessDHCPReplies( BaseType_t xExpectedMessageType, NetworkEndPoint_t *pxEndPoint );

/*
 * Generate a DHCP request packet, and send it on the DHCP socket.
 */
static void prvSendDHCPRequest( NetworkEndPoint_t *pxEndPoint );

/*
 * Prepare to start a DHCP transaction.  This initialises some state variables
 * and creates the DHCP socket if necessary.
 */
static void prvInitialiseDHCP( NetworkEndPoint_t *pxEndPoint );

/*
 * Creates the part of outgoing DHCP messages that are common to all outgoing
 * DHCP messages.
 */
static uint8_t *prvCreatePartDHCPMessage( struct freertos_sockaddr *pxAddress, BaseType_t xOpcode, const uint8_t * const pucOptionsArray,
	size_t *pxOptionsArraySize, NetworkEndPoint_t *pxEndPoint );

/*
 * Create the DHCP socket, if it has not been created already.
 */
static void prvCreateDHCPSocket( NetworkEndPoint_t *pxEndPoint );

/*
 * Close the DHCP socket, only when not in use anymore (i.e. xDHCPSocketUserCount = 0).
 */
static void prvCloseDHCPSocket( NetworkEndPoint_t *pxEndPoint );

static void vDHCPProcessEndPoint( BaseType_t xReset, BaseType_t xDoCheck, NetworkEndPoint_t *pxEndPoint );
/*
 * After DHCP has failed to answer, prepare everything to start searching
 * for (trying-out) LinkLayer IP-addresses, using the random method: Send
 * a gratuitos ARP request and wait if another device responds to it.
 */
#if( ipconfigDHCP_FALL_BACK_AUTO_IP != 0 )
	static void prvPrepareLinkLayerIPLookUp( void );
#endif

/*-----------------------------------------------------------*/

BaseType_t xIsDHCPSocket( Socket_t xSocket )
{
BaseType_t xReturn;

	if( xDHCPSocket == xSocket )
	{
		xReturn = pdTRUE;
	}
	else
	{
		xReturn = pdFALSE;
	}

	return xReturn;
}
/*-----------------------------------------------------------*/

#if( ipconfigHAS_DEBUG_PRINTF != 0 )
	static const char *pcDHCPStateName( eDHCPState_t eState )
	{
		switch( eState )
		{
			case eWaitingSendFirstDiscover:	return "eWaitingSendFirstDiscover";
			case eWaitingOffer:				return "eWaitingOffer";
			case eWaitingAcknowledge:		return "eWaitingAcknowledge";
		#if( ipconfigDHCP_FALL_BACK_AUTO_IP != 0 )
			case eGetLinkLayerAddress:		return "eGetLinkLayerAddress";
		#endif
			case eLeasedAddress:			return "eLeasedAddress";
			case eNotUsingLeasedAddress:	return "eNotUsingLeasedAddress";			/* DHCP failed, and a default IP address is being used. */
		}
		return "Unknown";
	}
#endif /* ipconfigHAS_DEBUG_PRINTF */
/*-----------------------------------------------------------*/

void vDHCPProcess( BaseType_t xReset, NetworkEndPoint_t *pxEndPoint )
{
	/* If there is a socket, check for incoming messasges first. */
	if( xDHCPSocket != NULL )
	{
	uint8_t *pucUDPPayload;
	DHCPMessage_IPv4_t *pxDHCPMessage;
	BaseType_t lBytes;

		for( ;; )
		{
		NetworkEndPoint_t *pxIterator = NULL;
			/* Peek the next UDP message. */
			lBytes = FreeRTOS_recvfrom( xDHCPSocket, ( void * ) &pucUDPPayload, 0, FREERTOS_ZERO_COPY | FREERTOS_MSG_PEEK, NULL, NULL );
			if( lBytes <= 0 )
			{
				if( ( lBytes < 0 ) && ( lBytes != -pdFREERTOS_ERRNO_EAGAIN ) )
				{
					FreeRTOS_printf( ( "vDHCPProcess: FreeRTOS_recvfrom returns %d\n", lBytes ) );
				}
				break;
			}
			/* Map a DHCP structure onto the received data. */
			pxDHCPMessage = ( DHCPMessage_IPv4_t * ) ( pucUDPPayload );

			/* Sanity check. */
			if( ( pxDHCPMessage->ulDHCPCookie == dhcpCOOKIE ) && ( pxDHCPMessage->ucOpcode == dhcpREPLY_OPCODE ) )
			{
				pxIterator = pxNetworkEndPoints;
				/* Find the end-point with given transaction ID. */
				while( pxIterator != NULL )
				{
					if( pxDHCPMessage->ulTransactionID == FreeRTOS_htonl( pxIterator->xDHCPData.ulTransactionId ) )
					{
						break;
					}
					pxIterator = pxIterator->pxNext;
				}
			}

			if( ( pxIterator != NULL ) && ( pxIterator->xDHCPData.eDHCPState == eLeasedAddress ) )
			{
				pxIterator = NULL;
			}

			if( pxIterator != NULL )
			{
				#if( ipconfigHAS_DEBUG_PRINTF != 0 )
				{
					FreeRTOS_debug_printf( ( "vDHCPProcess[%02x-%02x] ID %04X: state %s found xReset %d\n",
						pxIterator->xMACAddress.ucBytes[ 4 ],
						pxIterator->xMACAddress.ucBytes[ 5 ],
						pxIterator->xDHCPData.ulTransactionId,
						pcDHCPStateName( pxIterator->xDHCPData.eDHCPState ) ) );
				}
				#endif /* ipconfigHAS_DEBUG_PRINTF */
				/* The second parameter pdTRUE tells to check for a UDP message. */
				vDHCPProcessEndPoint( pdFALSE, pdTRUE, pxIterator );
				if( pxEndPoint == pxIterator )
				{
					pxEndPoint = NULL;
				}
			}
			else
			{
				/* Target not found, fetch the message and delete it. */
				lBytes = FreeRTOS_recvfrom( xDHCPSocket, ( void * ) &pucUDPPayload, 0, FREERTOS_ZERO_COPY, NULL, NULL );
				if( lBytes > 0 )
				{
					/* Remove it now, destination not found. */
					FreeRTOS_ReleaseUDPPayloadBuffer( ( void * ) pucUDPPayload );
					FreeRTOS_printf( ( "vDHCPProcess: Removed a %d-byte message: target not found\n", lBytes ) );
				}
			}
		}
	}

	if( pxEndPoint != NULL )
	{
		vDHCPProcessEndPoint( xReset, pdFALSE, pxEndPoint );
	}
}

static void vDHCPProcessEndPoint( BaseType_t xReset, BaseType_t xDoCheck, NetworkEndPoint_t *pxEndPoint )
{
BaseType_t xGivingUp = pdFALSE;
#if( ipconfigDHCP_USES_USER_HOOK != 0 )
	eDHCPCallbackAnswer_t eAnswer;
#endif	/* ipconfigDHCP_USES_USER_HOOK */

	configASSERT( pxEndPoint != NULL );

	/* Is DHCP starting over? */
	if( xReset != pdFALSE )
	{
		pxEndPoint->xDHCPData.eDHCPState = eWaitingSendFirstDiscover;
	}

	switch( pxEndPoint->xDHCPData.eDHCPState )
	{
		case eWaitingSendFirstDiscover :
			/* Ask the user if a DHCP discovery is required. */
		#if( ipconfigDHCP_USES_USER_HOOK != 0 )

			eAnswer = xApplicationDHCPUserHook( eDHCPOffer, pxEndPoint->ulDefaultIPAddress, pxEndPoint->ulNetMask );
			if( eAnswer == eDHCPContinue )
		#endif	/* ipconfigDHCP_USES_USER_HOOK */
			{
				/* Initial state.  Create the DHCP socket, timer, etc. if they
				have not already been created. */
				prvInitialiseDHCP( pxEndPoint );
				/* Put 'ulIPAddress' to zero to indicate that the end-point is down. */
				pxEndPoint->ulIPAddress = 0UL;

				/* Send the first discover request. */
				if( xDHCPSocket != NULL )
				{
					pxEndPoint->xDHCPData.xDHCPTxTime = xTaskGetTickCount();
					prvSendDHCPDiscover( pxEndPoint );
					pxEndPoint->xDHCPData.eDHCPState = eWaitingOffer;
				}
			}
		#if( ipconfigDHCP_USES_USER_HOOK != 0 )
			else
			{
				if( eAnswer == eDHCPUseDefaults )
				{
					memcpy( &xNetworkAddressing, &xDefaultAddressing, sizeof xNetworkAddressing );
				}
				/* The user indicates that the DHCP process does not continue. */
				xGivingUp = pdTRUE;
			}
		#endif	/* ipconfigDHCP_USES_USER_HOOK */
			break;

		case eWaitingOffer :

			xGivingUp = pdFALSE;

			/* Look for offers coming in. */
			if( ( xDoCheck != pdFALSE ) && ( prvProcessDHCPReplies( dhcpMESSAGE_TYPE_OFFER, pxEndPoint ) == pdPASS ) )
			{
			#if( ipconfigDHCP_USES_USER_HOOK != 0 )
				/* Ask the user if a DHCP request is required. */
				eAnswer = xApplicationDHCPUserHook( eDHCPRequest, pxEndPoint->xDHCPData.ulOfferedIPAddress, xNetworkAddressing.ulNetMask );
				if( eAnswer == eDHCPContinue )
			#endif	/* ipconfigDHCP_USES_USER_HOOK */
				{
					/* An offer has been made, the user wants to continue,
					generate the request. */
					pxEndPoint->xDHCPData.xDHCPTxTime = xTaskGetTickCount();
					pxEndPoint->xDHCPData.xDHCPTxPeriod = dhcpINITIAL_DHCP_TX_PERIOD;
					prvSendDHCPRequest( pxEndPoint );
					pxEndPoint->xDHCPData.eDHCPState = eWaitingAcknowledge;
					break;
				}
			#if( ipconfigDHCP_USES_USER_HOOK != 0 )
				if( eAnswer == eDHCPUseDefaults )
				{
					memcpy( &xNetworkAddressing, &xDefaultAddressing, sizeof xNetworkAddressing );
					pxEndPoint->ulIPAddress = pxEndPoint->ulDefaultIPAddress;	/* Use this address in case DHCP has failed. */
				}
				/* The user indicates that the DHCP process does not continue. */
				xGivingUp = pdTRUE;
			#endif	/* ipconfigDHCP_USES_USER_HOOK */
			}

			/* Is it time to send another Discover? */
			else if( ( xTaskGetTickCount() - pxEndPoint->xDHCPData.xDHCPTxTime ) > pxEndPoint->xDHCPData.xDHCPTxPeriod )
			{
				/* Increase the time period, and if it has not got to the
				point of giving up - send another discovery. */
				pxEndPoint->xDHCPData.xDHCPTxPeriod <<= 1;

				if( pxEndPoint->xDHCPData.xDHCPTxPeriod <= ipconfigMAXIMUM_DISCOVER_TX_PERIOD )
				{
					pxEndPoint->xDHCPData.ulTransactionId++;
					pxEndPoint->xDHCPData.xDHCPTxTime = xTaskGetTickCount();
					pxEndPoint->xDHCPData.xUseBroadcast = !pxEndPoint->xDHCPData.xUseBroadcast;
					prvSendDHCPDiscover( pxEndPoint );
					FreeRTOS_printf( ( "vDHCPProcess[%02x-%02x]: timeout %lu ticks\n",
						pxEndPoint->xMACAddress.ucBytes[ 4 ],
						pxEndPoint->xMACAddress.ucBytes[ 5 ],
						pxEndPoint->xDHCPData.xDHCPTxPeriod ) );
				}
				else
				{
					FreeRTOS_printf( ( "vDHCPProcess[%02x-%02x]: giving up %lu > %lu ticks\n",
						pxEndPoint->xMACAddress.ucBytes[ 4 ],
						pxEndPoint->xMACAddress.ucBytes[ 5 ],
						pxEndPoint->xDHCPData.xDHCPTxPeriod, ipconfigMAXIMUM_DISCOVER_TX_PERIOD ) );

					#if( ipconfigDHCP_FALL_BACK_AUTO_IP != 0 )
					{
						/* only use fake ack if default IP address = 0x00 and link local addressing is used. */
						/* Start searching a free LinkLayer IP-address.
						Next state will be 'eGetLinkLayerAddress'. */
						prvPrepareLinkLayerIPLookUp();
						pxEndPoint->xDHCPData.eDHCPState = eGetLinkLayerAddress;		/* setting IP address manually so set to not using leased address mode. */
					}
					#else
					{
						xGivingUp = pdTRUE;
					}
					#endif /* ipconfigDHCP_FALL_BACK_AUTO_IP */
				}
			}
			break;

		case eWaitingAcknowledge :

			/* Look for acks coming in. */
			if( ( xDoCheck != pdFALSE ) && ( prvProcessDHCPReplies( dhcpMESSAGE_TYPE_ACK, pxEndPoint ) == pdPASS ) )
			{
				FreeRTOS_printf( ( "vDHCPProcess[%02x-%02x]: acked %lxip\n",
					pxEndPoint->xMACAddress.ucBytes[ 4 ],
					pxEndPoint->xMACAddress.ucBytes[ 5 ],
					FreeRTOS_ntohl( pxEndPoint->xDHCPData.ulOfferedIPAddress ) ) );

				/* DHCP completed.  The IP address can now be used, and the
				timer set to the lease timeout time. */
				pxEndPoint->ulIPAddress = pxEndPoint->xDHCPData.ulOfferedIPAddress;

				/* Setting the 'local' broadcast address, something like 192.168.1.255' */
				xNetworkAddressing.ulBroadcastAddress = pxEndPoint->xDHCPData.ulOfferedIPAddress |  ~xNetworkAddressing.ulNetMask;
				pxEndPoint->ulBroadcastAddress = pxEndPoint->xDHCPData.ulOfferedIPAddress | ~( pxEndPoint->ulNetMask );

				pxEndPoint->xDHCPData.eDHCPState = eLeasedAddress;

				iptraceDHCP_SUCCEDEED( pxEndPoint->xDHCPData.ulOfferedIPAddress );

				/* DHCP failed, the default configured IP-address will be used
				Now call vIPNetworkUpCalls() to send the network-up event, start Nabto
				and start the ARP timer*/
				vIPNetworkUpCalls( pxEndPoint );

				/* Close socket to ensure packets don't queue on it. */
				prvCloseDHCPSocket( pxEndPoint );

				if( pxEndPoint->xDHCPData.ulLeaseTime == 0UL )
				{
					pxEndPoint->xDHCPData.ulLeaseTime = dhcpDEFAULT_LEASE_TIME;
				}
				else if( pxEndPoint->xDHCPData.ulLeaseTime < dhcpMINIMUM_LEASE_TIME )
				{
					pxEndPoint->xDHCPData.ulLeaseTime = dhcpMINIMUM_LEASE_TIME;
				}
				else
				{
					/* The lease time is already valid. */
				}

				/* Check for clashes. */
				vARPSendGratuitous();
				vIPReloadDHCPTimer( pxEndPoint, pxEndPoint->xDHCPData.ulLeaseTime );
			}
			else
			{
				/* Is it time to send another Discover? */
				if( ( xTaskGetTickCount() - pxEndPoint->xDHCPData.xDHCPTxTime ) > pxEndPoint->xDHCPData.xDHCPTxPeriod )
				{
					/* Increase the time period, and if it has not got to the
					point of giving up - send another request. */
					pxEndPoint->xDHCPData.xDHCPTxPeriod <<= 1;

					if( pxEndPoint->xDHCPData.xDHCPTxPeriod <= ipconfigMAXIMUM_DISCOVER_TX_PERIOD )
					{
						pxEndPoint->xDHCPData.xDHCPTxTime = xTaskGetTickCount();
						prvSendDHCPRequest( pxEndPoint );
					}
					else
					{
						/* Give up, start again. */
						pxEndPoint->xDHCPData.eDHCPState = eWaitingSendFirstDiscover;
					}
				}
			}
			break;

#if( ipconfigDHCP_FALL_BACK_AUTO_IP != 0 )
		case eGetLinkLayerAddress:
			if( ( xTaskGetTickCount() - pxEndPoint->xDHCPData.xDHCPTxTime ) > pxEndPoint->xDHCPData.xDHCPTxPeriod )
			{
				if( xARPHadIPClash == pdFALSE )
				{
					/* ARP OK. proceed. */
					iptraceDHCP_SUCCEDEED( pxEndPoint->xDHCPData.ulOfferedIPAddress );

					/* Auto-IP succeeded, the default configured IP-address will be used
					Now call vIPNetworkUpCalls() to send the network-up event, start Nabto
					and start the ARP timer*/
					vIPNetworkUpCalls( pxEndPoint );
					pxEndPoint->xDHCPData.eDHCPState = eNotUsingLeasedAddress;
				}
				else
				{
					/* ARP clashed - try another IP address. */
					prvPrepareLinkLayerIPLookUp();
					pxEndPoint->xDHCPData.eDHCPState = eGetLinkLayerAddress;		/* setting IP address manually so set to not using leased address mode. */
				}
			}
			break;
#endif	/* ipconfigDHCP_FALL_BACK_AUTO_IP */

		case eLeasedAddress :

			/* Resend the request at the appropriate time to renew the lease. */
			prvCreateDHCPSocket( pxEndPoint );

			if( xDHCPSocket != NULL )
			{
				pxEndPoint->xDHCPData.xDHCPTxTime = xTaskGetTickCount();
				pxEndPoint->xDHCPData.xDHCPTxPeriod = dhcpINITIAL_DHCP_TX_PERIOD;
				prvSendDHCPRequest( pxEndPoint );
				pxEndPoint->xDHCPData.eDHCPState = eWaitingAcknowledge;
				/* From now on, we should be called more often */
				vIPReloadDHCPTimer( pxEndPoint, dhcpINITIAL_TIMER_PERIOD );
			}
			break;

		case eNotUsingLeasedAddress:

			vIPSetDHCPTimerEnableState( pxEndPoint, pdFALSE );
			break;
	}

	if( xGivingUp != pdFALSE )
	{
		/* xGivingUp became true either because of a time-out, or because
		xApplicationDHCPUserHook() returned another value than 'eDHCPContinue',
		meaning that the conversion is cancelled from here. */

		/* Revert to static IP address. */
		taskENTER_CRITICAL();
		{
			pxEndPoint->ulIPAddress = xNetworkAddressing.ulDefaultIPAddress;

			iptraceDHCP_REQUESTS_FAILED_USING_DEFAULT_IP_ADDRESS( xNetworkAddressing.ulDefaultIPAddress );
		}
		taskEXIT_CRITICAL();

		pxEndPoint->xDHCPData.eDHCPState = eNotUsingLeasedAddress;
		vIPSetDHCPTimerEnableState( pxEndPoint, pdFALSE );

		/* DHCP failed, the default configured IP-address will be used
		Now call vIPNetworkUpCalls() to send the network-up event, start Nabto
		and start the ARP timer*/
		vIPNetworkUpCalls( pxEndPoint );

		/* Close socket to ensure packets don't queue on it. */
		prvCloseDHCPSocket( pxEndPoint );
	}
}
/*-----------------------------------------------------------*/

static void prvCloseDHCPSocket( NetworkEndPoint_t *pxEndPoint )
{
	if( (pxEndPoint->xDHCPData.xDHCPSocket == NULL ) || ( pxEndPoint->xDHCPData.xDHCPSocket != xDHCPSocket ) )
	{
		/* the socket can not be closed. */
	}
	else if( xDHCPSocketUserCount > 0 )
	{
		xDHCPSocketUserCount--;
		if( xDHCPSocketUserCount == 0 )
		{
			FreeRTOS_closesocket( xDHCPSocket );
			xDHCPSocket = NULL;
		}
		pxEndPoint->xDHCPData.xDHCPSocket = NULL;
	}
FreeRTOS_printf( ("DHCP-socket[%02x-%02x]: closed, user count %d\n",
	pxEndPoint->xMACAddress.ucBytes[ 4 ],
	pxEndPoint->xMACAddress.ucBytes[ 5 ],
	xDHCPSocketUserCount ) );
}

static void prvCreateDHCPSocket( NetworkEndPoint_t *pxEndPoint )
{
struct freertos_sockaddr xAddress;
BaseType_t xReturn;
TickType_t xTimeoutTime = 0;

	if( ( xDHCPSocket != NULL ) && ( pxEndPoint->xDHCPData.xDHCPSocket == xDHCPSocket ) )
	{
		/* the socket is still valid. */
	}
	else if( xDHCPSocket == NULL ) /* Create the socket, if it has not already been created. */
	{
		xDHCPSocket = FreeRTOS_socket( FREERTOS_AF_INET, FREERTOS_SOCK_DGRAM, FREERTOS_IPPROTO_UDP );
		configASSERT( ( xDHCPSocket != FREERTOS_INVALID_SOCKET ) );

		/* Ensure the Rx and Tx timeouts are zero as the DHCP executes in the
		context of the IP task. */
		FreeRTOS_setsockopt( xDHCPSocket, 0, FREERTOS_SO_RCVTIMEO, ( void * ) &xTimeoutTime, sizeof( TickType_t ) );
		FreeRTOS_setsockopt( xDHCPSocket, 0, FREERTOS_SO_SNDTIMEO, ( void * ) &xTimeoutTime, sizeof( TickType_t ) );

		/* Bind to the standard DHCP client port. */
		xAddress.sin_port = dhcpCLIENT_PORT_IPv4;
		xReturn = vSocketBind( xDHCPSocket, &xAddress, sizeof( xAddress ), pdFALSE );
		configASSERT( xReturn == 0 );
		xDHCPSocketUserCount = 1;
		FreeRTOS_printf( ("DHCP-socket[%02x-%02x]: DHCP Socket Create\n",
			pxEndPoint->xMACAddress.ucBytes[ 4 ],
			pxEndPoint->xMACAddress.ucBytes[ 5 ] ) );

		/* Remove compiler warnings if configASSERT() is not defined. */
		( void ) xReturn;
	}
	else
	{
		xDHCPSocketUserCount++;
	}
	pxEndPoint->xDHCPData.xDHCPSocket = xDHCPSocket;
	FreeRTOS_printf( ("DHCP-socket[%02x-%02x]: opened, user count %d\n",
		pxEndPoint->xMACAddress.ucBytes[ 4 ],
		pxEndPoint->xMACAddress.ucBytes[ 5 ],
		xDHCPSocketUserCount ) );
}
/*-----------------------------------------------------------*/

static void prvInitialiseDHCP( NetworkEndPoint_t *pxEndPoint )
{
	/* Initialise the parameters that will be set by the DHCP process. */
	if( pxEndPoint->xDHCPData.ulTransactionId == 0 )
	{
		pxEndPoint->xDHCPData.ulTransactionId = ipconfigRAND32();
	}
	else
	{
		pxEndPoint->xDHCPData.ulTransactionId++;
	}

	pxEndPoint->xDHCPData.xUseBroadcast = 0;
	pxEndPoint->xDHCPData.ulOfferedIPAddress = 0UL;
	pxEndPoint->xDHCPData.ulDHCPServerAddress = 0UL;
	pxEndPoint->xDHCPData.xDHCPTxPeriod = dhcpINITIAL_DHCP_TX_PERIOD;

	/* Create the DHCP socket if it has not already been created. */
	prvCreateDHCPSocket( pxEndPoint );
	FreeRTOS_printf( ( "prvInitialiseDHCP[%02x:%02x]: start after %lu ticks\n",
		pxEndPoint->xMACAddress.ucBytes[ 4 ],
		pxEndPoint->xMACAddress.ucBytes[ 5 ],
		dhcpINITIAL_TIMER_PERIOD ) );
	vIPReloadDHCPTimer( pxEndPoint, dhcpINITIAL_TIMER_PERIOD );
}
/*-----------------------------------------------------------*/

static BaseType_t prvProcessDHCPReplies( BaseType_t xExpectedMessageType, NetworkEndPoint_t *pxEndPoint )
{
uint8_t *pucUDPPayload, *pucLastByte;
int32_t lBytes;
DHCPMessage_IPv4_t *pxDHCPMessage;
uint8_t *pucByte, ucOptionCode, ucLength;
uint32_t ulProcessed, ulParameter;
BaseType_t xReturn = pdFALSE;
const uint32_t ulMandatoryOptions = 2; /* DHCP server address, and the correct DHCP message type must be present in the options. */

	lBytes = FreeRTOS_recvfrom( xDHCPSocket, ( void * ) &pucUDPPayload, 0, FREERTOS_ZERO_COPY, NULL, NULL );
	if( lBytes != -pdFREERTOS_ERRNO_EAGAIN )
	{
		FreeRTOS_printf( ( "prvProcessDHCPReplies[%02x-%02x]: lBytes %ld\n",
			pxEndPoint->xMACAddress.ucBytes[ 4 ],
			pxEndPoint->xMACAddress.ucBytes[ 5 ],
			lBytes ) );
	}
	if( lBytes > 0 )
	{
		/* Map a DHCP structure onto the received data. */
		pxDHCPMessage = ( DHCPMessage_IPv4_t * ) ( pucUDPPayload );

		/* Sanity check. */
		if( ( pxDHCPMessage->ulDHCPCookie != dhcpCOOKIE ) || ( pxDHCPMessage->ucOpcode != dhcpREPLY_OPCODE ) )
		{
			FreeRTOS_printf( ( "prvProcessDHCPReplies[%02x-%02x]: Invalid DHCP message: Cookie 0x%X Opcode 0x%X\n",
				pxEndPoint->xMACAddress.ucBytes[ 4 ],
				pxEndPoint->xMACAddress.ucBytes[ 5 ],
				pxDHCPMessage->ulDHCPCookie, pxDHCPMessage->ucOpcode ) );
		}
		else if( ( pxDHCPMessage->ulTransactionID != FreeRTOS_htonl( pxEndPoint->xDHCPData.ulTransactionId ) ) )
		{
			FreeRTOS_printf( ( "prvProcessDHCPReplies[%02x-%02x]: DHCP message not for me 0x%X != 0x%X\n",
				pxEndPoint->xMACAddress.ucBytes[ 4 ],
				pxEndPoint->xMACAddress.ucBytes[ 5 ],
				pxDHCPMessage->ulTransactionID, FreeRTOS_htonl( pxEndPoint->xDHCPData.ulTransactionId ) ) );
		}
		else /* Looks like a valid DHCP response, with the same transaction ID as the current end-point. */
		{
			if( memcmp( ( void * ) &( pxDHCPMessage->ucClientHardwareAddress ), ( void * ) pxEndPoint->xMACAddress.ucBytes, sizeof( MACAddress_t ) ) != 0 )
			{
				FreeRTOS_printf( ( "prvProcessDHCPReplies[%02x-%02x]: For other MAC %02x-%02x\n",
					pxEndPoint->xMACAddress.ucBytes[ 4 ],
					pxEndPoint->xMACAddress.ucBytes[ 5 ],
					pxDHCPMessage->ucClientHardwareAddress[ 4 ],
					pxDHCPMessage->ucClientHardwareAddress[ 5 ] ) );
			}
			else
			{
				/* None of the essential options have been processed yet. */
				ulProcessed = 0;

				FreeRTOS_printf( ( "prvProcessDHCPReplies[%02x-%02x]: For me\n",
					pxEndPoint->xMACAddress.ucBytes[ 4 ],
					pxEndPoint->xMACAddress.ucBytes[ 5 ] ) );

				/* Walk through the options until the dhcpOPTION_END_BYTE byte
				is found, taking care not to walk off the end of the options. */
				pucByte = &( pxDHCPMessage->ucFirstOptionByte );
				pucLastByte = &( pucUDPPayload[ lBytes - dhcpMAX_OPTION_LENGTH_OF_INTEREST ] );

				while( pucByte < pucLastByte )
				{
					ucOptionCode = pucByte[ 0 ];
					if( ucOptionCode == dhcpOPTION_END_BYTE )
					{
						/* Ready, the last byte has been seen. */
						break;
					}
					if( ucOptionCode == dhcpIPv4_ZERO_PAD_OPTION_CODE )
					{
						/* The value zero is used as a pad byte,
						it is not followed by a length byte. */
						pucByte += 1;
						continue;
					}
					ucLength = pucByte[ 1 ];
					pucByte += 2;

					/* In most cases, a 4-byte network-endian parameter follows,
					just get it once here and use later */
					memcpy( ( void * ) &( ulParameter ), ( void * ) pucByte, ( size_t ) sizeof( ulParameter ) );

					switch( ucOptionCode )
					{
						case dhcpIPv4_MESSAGE_TYPE_OPTION_CODE	:

							if( *pucByte == ( uint8_t ) xExpectedMessageType )
							{
								/* The message type is the message type the
								state machine is expecting. */
								ulProcessed++;
							}
							else if( *pucByte == dhcpMESSAGE_TYPE_NACK )
							{
								if( dhcpMESSAGE_TYPE_ACK == ( uint8_t ) xExpectedMessageType)
								{
									/* Start again. */
									pxEndPoint->xDHCPData.eDHCPState = eWaitingSendFirstDiscover;
								}
							}
							else
							{
								/* Don't process other message types. */
							}
							break;

						case dhcpIPv4_SUBNET_MASK_OPTION_CODE :

							if( ucLength == sizeof( uint32_t ) )
							{
								xNetworkAddressing.ulNetMask = ulParameter;
								pxEndPoint->ulNetMask = ulParameter;
							}
							break;

						case dhcpIPv4_GATEWAY_OPTION_CODE :

							if( ucLength == sizeof( uint32_t ) )
							{
								/* ulProcessed is not incremented in this case
								because the gateway is not essential. */
								xNetworkAddressing.ulGatewayAddress = ulParameter;
								pxEndPoint->ulGatewayAddress = ulParameter;
							}
							break;

						case dhcpIPv4_DNS_SERVER_OPTIONS_CODE :

							/* ulProcessed is not incremented in this case
							because the DNS server is not essential.  Only the
							first DNS server address is taken. */
							xNetworkAddressing.ulDNSServerAddress = ulParameter;
							pxEndPoint->ulDNSServerAddresses[ 0 ] = ulParameter;
							break;

						case dhcpIPv4_SERVER_IP_ADDRESS_OPTION_CODE :

							if( ucLength == sizeof( uint32_t ) )
							{
								if( dhcpMESSAGE_TYPE_OFFER == ( uint8_t ) xExpectedMessageType )
								{
									/* Offers state the replying server. */
									ulProcessed++;
									pxEndPoint->xDHCPData.ulDHCPServerAddress = ulParameter;
								}
								else
								{
									/* The ack must come from the expected server. */
									if( pxEndPoint->xDHCPData.ulDHCPServerAddress == ulParameter )
									{
										ulProcessed++;
									}
								}
							}
							break;

						case dhcpIPv4_LEASE_TIME_OPTION_CODE :

							if( ucLength == sizeof( &( pxEndPoint->xDHCPData.ulLeaseTime ) ) )
							{
								/* ulProcessed is not incremented in this case
								because the lease time is not essential. */
								/* The DHCP parameter is in seconds, convert
								to host-endian format. */
								pxEndPoint->xDHCPData.ulLeaseTime = FreeRTOS_ntohl( ulParameter );

								/* Divide the lease time by two to ensure a renew
								request is sent before the lease actually expires. */
								pxEndPoint->xDHCPData.ulLeaseTime >>= 1UL;

								/* Multiply with configTICK_RATE_HZ to get clock ticks. */
								pxEndPoint->xDHCPData.ulLeaseTime = configTICK_RATE_HZ * pxEndPoint->xDHCPData.ulLeaseTime;
							}
							break;

						default :

							/* Not interested in this field. */

							break;
					}

					/* Jump over the data to find the next option code. */
					if( ucLength == 0 )
					{
						break;
					}
					else
					{
						pucByte += ucLength;
					}
				}

				/* Were all the mandatory options received? */
				if( ulProcessed >= ulMandatoryOptions )
				{
					/* HT:endian: used to be network order */
					pxEndPoint->xDHCPData.ulOfferedIPAddress = pxDHCPMessage->ulYourIPAddress_yiaddr;
					FreeRTOS_printf( ( "vDHCPProcess[%02x-%02x]: offer %lxip\n",
						pxEndPoint->xMACAddress.ucBytes[ 4 ],
						pxEndPoint->xMACAddress.ucBytes[ 5 ],
						FreeRTOS_ntohl( pxEndPoint->xDHCPData.ulOfferedIPAddress ) ) );
					xReturn = pdPASS;
				}
			}
		}
		FreeRTOS_ReleaseUDPPayloadBuffer( ( void * ) pucUDPPayload );
	} /* if( lBytes > 0 ) */

	return xReturn;
}
/*-----------------------------------------------------------*/

static uint8_t *prvCreatePartDHCPMessage( struct freertos_sockaddr *pxAddress, BaseType_t xOpcode, const uint8_t * const pucOptionsArray,
	size_t *pxOptionsArraySize, NetworkEndPoint_t *pxEndPoint )
{
DHCPMessage_IPv4_t *pxDHCPMessage;
size_t xRequiredBufferSize = sizeof( DHCPMessage_IPv4_t ) + *pxOptionsArraySize;
NetworkBufferDescriptor_t *pxNetworkBuffer;
uint8_t *pucUDPPayloadBuffer;
#if( ipconfigUSE_IPv6 != 0 )
	uint8_t *pucIPType;
#endif

#if( ipconfigDHCP_REGISTER_HOSTNAME == 1 )
	const char *pucHostName = pcApplicationHostnameHook ();
	size_t xNameLength = strlen( pucHostName );
	uint8_t *pucPtr;

	xRequiredBufferSize += ( 2 + xNameLength );
#endif

	/* Get a buffer.  This uses a maximum delay, but the delay will be capped
	to ipconfigUDP_MAX_SEND_BLOCK_TIME_TICKS so the return value still needs to
	be test. */
	do
	{
		/* Obtain a network buffer with the required amount of storage. */
		pxNetworkBuffer = pxGetNetworkBufferWithDescriptor( sizeof( UDPPacket_t ) + xRequiredBufferSize, portMAX_DELAY );
	} while( pxNetworkBuffer == NULL );

		/* Leave space for the UPD header. */
	pucUDPPayloadBuffer = pxNetworkBuffer->pucEthernetBuffer + ipUDP_PAYLOAD_OFFSET_IPv6;
	pxDHCPMessage = ( DHCPMessage_IPv4_t * ) pucUDPPayloadBuffer;

	#if( ipconfigUSE_IPv6 != 0 )
	{
		pucIPType = pucUDPPayloadBuffer - ipUDP_PAYLOAD_IP_TYPE_OFFSET;
		*pucIPType = ipTYPE_IPv4;
	}
	#endif /* ipconfigUSE_IPv6 */

	/* Most fields need to be zero. */
	memset( ( void * ) pxDHCPMessage, 0x00, sizeof( DHCPMessage_IPv4_t ) );

	/* Create the message. */
	pxDHCPMessage->ucOpcode = ( uint8_t ) xOpcode;
	pxDHCPMessage->ucAddressType = dhcpADDRESS_TYPE_ETHERNET;
	pxDHCPMessage->ucAddressLength = dhcpETHERNET_ADDRESS_LENGTH;

	/* ulTransactionID doesn't really need a htonl() translation, but when DHCP
	times out, it is nicer to see an increasing number in this ID field */
	pxDHCPMessage->ulTransactionID = FreeRTOS_htonl( pxEndPoint->xDHCPData.ulTransactionId );
	pxDHCPMessage->ulDHCPCookie = dhcpCOOKIE;
	if( pxEndPoint->xDHCPData.xUseBroadcast != pdFALSE )
	{
		pxDHCPMessage->usFlags = dhcpBROADCAST;
	}
	else
	{
		pxDHCPMessage->usFlags = 0;
	}

	memcpy( ( void * ) &( pxDHCPMessage->ucClientHardwareAddress[ 0 ] ), ( void * ) pxEndPoint->xMACAddress.ucBytes, sizeof( MACAddress_t ) );

	/* Copy in the const part of the options options. */
	memcpy( ( void * ) &( pucUDPPayloadBuffer[ dhcpFIRST_OPTION_BYTE_OFFSET ] ), ( void * ) pucOptionsArray, *pxOptionsArraySize );

	#if( ipconfigDHCP_REGISTER_HOSTNAME == 1 )
	{
		/* With this option, the hostname can be registered as well which makes
		it easier to lookup a device in a router's list of DHCP clients. */

		/* Point to where the OPTION_END was stored to add data. */
		pucPtr = &( pucUDPPayloadBuffer[ dhcpFIRST_OPTION_BYTE_OFFSET + ( *pxOptionsArraySize - 1 ) ] );
		pucPtr[ 0 ] = dhcpIPv4_DNS_HOSTNAME_OPTIONS_CODE;
		pucPtr[ 1 ] = ( uint8_t ) xNameLength;
		memcpy( ( void *) ( pucPtr + 2 ), pucHostName, xNameLength );
		pucPtr[ 2 + xNameLength ] = dhcpOPTION_END_BYTE;
		*pxOptionsArraySize += ( 2 + xNameLength );
	}
	#endif

	/* Map in the client identifier. */
	memcpy( ( void * ) &( pucUDPPayloadBuffer[ dhcpFIRST_OPTION_BYTE_OFFSET + dhcpCLIENT_IDENTIFIER_OFFSET ] ),
		( void * ) pxEndPoint->xMACAddress.ucBytes, sizeof( MACAddress_t ) );

	/* Set the addressing. */
	pxAddress->sin_addr = ipBROADCAST_IP_ADDRESS;
	pxAddress->sin_port = ( uint16_t ) dhcpSERVER_PORT_IPv4;
	pxNetworkBuffer->pxEndPoint = pxEndPoint;

	return pucUDPPayloadBuffer;
}
/*-----------------------------------------------------------*/

static void prvSendDHCPRequest( NetworkEndPoint_t *pxEndPoint )
{
uint8_t *pucUDPPayloadBuffer;
struct freertos_sockaddr xAddress;
static const uint8_t ucDHCPRequestOptions[] =
{
	/* Do not change the ordering without also changing
	dhcpCLIENT_IDENTIFIER_OFFSET, dhcpREQUESTED_IP_ADDRESS_OFFSET and
	dhcpDHCP_SERVER_IP_ADDRESS_OFFSET. */
	dhcpIPv4_MESSAGE_TYPE_OPTION_CODE, 1, dhcpMESSAGE_TYPE_REQUEST,		/* Message type option. */
	dhcpIPv4_CLIENT_IDENTIFIER_OPTION_CODE, 6, 0, 0, 0, 0, 0, 0,			/* Client identifier. */
	dhcpIPv4_REQUEST_IP_ADDRESS_OPTION_CODE, 4, 0, 0, 0, 0,				/* The IP address being requested. */
	dhcpIPv4_SERVER_IP_ADDRESS_OPTION_CODE, 4, 0, 0, 0, 0,				/* The IP address of the DHCP server. */
	dhcpOPTION_END_BYTE
};
size_t xOptionsLength = sizeof( ucDHCPRequestOptions );

	pucUDPPayloadBuffer = prvCreatePartDHCPMessage( &xAddress, (uint8_t)dhcpREQUEST_OPCODE, ucDHCPRequestOptions, &xOptionsLength, pxEndPoint );

	/* Copy in the IP address being requested. */
	memcpy( ( void * ) &( pucUDPPayloadBuffer[ dhcpFIRST_OPTION_BYTE_OFFSET + dhcpREQUESTED_IP_ADDRESS_OFFSET ] ),
		( void * ) &( pxEndPoint->xDHCPData.ulOfferedIPAddress ), sizeof( pxEndPoint->xDHCPData.ulOfferedIPAddress ) );

	/* Copy in the address of the DHCP server being used. */
	memcpy( ( void * ) &( pucUDPPayloadBuffer[ dhcpFIRST_OPTION_BYTE_OFFSET + dhcpDHCP_SERVER_IP_ADDRESS_OFFSET ] ),
		( void * ) &( pxEndPoint->xDHCPData.ulDHCPServerAddress ), sizeof( pxEndPoint->xDHCPData.ulDHCPServerAddress ) );

	FreeRTOS_printf( ( "vDHCPProcess[%02x-%02x]: Send request %lxip\n",
		pxEndPoint->xMACAddress.ucBytes[ 4 ],
		pxEndPoint->xMACAddress.ucBytes[ 5 ],
		FreeRTOS_ntohl( pxEndPoint->xDHCPData.ulOfferedIPAddress ) ) );
	iptraceSENDING_DHCP_REQUEST();

	if( FreeRTOS_sendto( xDHCPSocket, pucUDPPayloadBuffer, ( sizeof( DHCPMessage_IPv4_t ) + xOptionsLength ), FREERTOS_ZERO_COPY, &xAddress, sizeof( xAddress ) ) == 0 )
	{
		/* The packet was not successfully queued for sending and must be
		returned to the stack. */
		FreeRTOS_ReleaseUDPPayloadBuffer( pucUDPPayloadBuffer );
	}
}
/*-----------------------------------------------------------*/

static void prvSendDHCPDiscover( NetworkEndPoint_t *pxEndPoint )
{
uint8_t *pucUDPPayloadBuffer;
struct freertos_sockaddr xAddress;
static const uint8_t ucDHCPDiscoverOptions[] =
{
	/* Do not change the ordering without also changing dhcpCLIENT_IDENTIFIER_OFFSET. */
	dhcpIPv4_MESSAGE_TYPE_OPTION_CODE, 1, dhcpMESSAGE_TYPE_DISCOVER,					/* Message type option. */
	dhcpIPv4_CLIENT_IDENTIFIER_OPTION_CODE, 6, 0, 0, 0, 0, 0, 0,						/* Client identifier. */
	dhcpIPv4_PARAMETER_REQUEST_OPTION_CODE, 3, dhcpIPv4_SUBNET_MASK_OPTION_CODE, dhcpIPv4_GATEWAY_OPTION_CODE, dhcpIPv4_DNS_SERVER_OPTIONS_CODE,	/* Parameter request option. */
	dhcpOPTION_END_BYTE
};
size_t xOptionsLength = sizeof( ucDHCPDiscoverOptions );

	pucUDPPayloadBuffer = prvCreatePartDHCPMessage( &xAddress, (uint8_t)dhcpREQUEST_OPCODE, ucDHCPDiscoverOptions, &xOptionsLength, pxEndPoint );

	FreeRTOS_printf( ( "vDHCPProcess[%02x-%02x]: discover\n",
		pxEndPoint->xMACAddress.ucBytes[ 4 ],
		pxEndPoint->xMACAddress.ucBytes[ 5 ] ) );
	iptraceSENDING_DHCP_DISCOVER();

	if( FreeRTOS_sendto( xDHCPSocket, pucUDPPayloadBuffer, ( sizeof( DHCPMessage_IPv4_t ) + xOptionsLength ), FREERTOS_ZERO_COPY, &xAddress, sizeof( xAddress ) ) == 0 )
	{
		/* The packet was not successfully queued for sending and must be
		returned to the stack. */
		FreeRTOS_ReleaseUDPPayloadBuffer( pucUDPPayloadBuffer );
	}
}
/*-----------------------------------------------------------*/

#if( ipconfigDHCP_FALL_BACK_AUTO_IP != 0 )

	static void prvPrepareLinkLayerIPLookUp()
	{
	uint8_t ucLinkLayerIPAddress[ 2 ];

		/* After DHCP has failed to answer, prepare everything to start
		trying-out LinkLayer IP-addresses, using the random method. */
		pxEndPoint->xDHCPData.xDHCPTxTime = xTaskGetTickCount();

		ucLinkLayerIPAddress[ 0 ] = ( uint8_t )1 + ( ipconfigRAND32() % 0xFD );		/* get value 1..254 for IP-address 3rd byte of IP address to try. */
		ucLinkLayerIPAddress[ 1 ] = ( uint8_t )1 + ( ipconfigRAND32() % 0xFD );		/* get value 1..254 for IP-address 4th byte of IP address to try. */

		xNetworkAddressing.ulGatewayAddress = FreeRTOS_htonl( 0xA9FE0203 );

		/* prepare pxEndPoint->xDHCPData with data to test. */
		pxEndPoint->xDHCPData.ulOfferedIPAddress =
			FreeRTOS_inet_addr_quick( LINK_LAYER_ADDRESS_0, LINK_LAYER_ADDRESS_1, ucLinkLayerIPAddress[ 0 ], ucLinkLayerIPAddress[ 1 ] );

		pxEndPoint->xDHCPData.ulLeaseTime = dhcpDEFAULT_LEASE_TIME;	 /*  don't care about lease time. just put anything. */

		xNetworkAddressing.ulNetMask =
			FreeRTOS_inet_addr_quick( LINK_LAYER_NETMASK_0, LINK_LAYER_NETMASK_1, LINK_LAYER_NETMASK_2, LINK_LAYER_NETMASK_3 );

		/* DHCP completed.  The IP address can now be used, and the
		timer set to the lease timeout time. */
		*ipLOCAL_IP_ADDRESS_POINTER = pxEndPoint->xDHCPData.ulOfferedIPAddress;

		/* Setting the 'local' broadcast address, something like 192.168.1.255' */
		xNetworkAddressing.ulBroadcastAddress = ( pxEndPoint->xDHCPData.ulOfferedIPAddress & xNetworkAddressing.ulNetMask ) |  ~xNetworkAddressing.ulNetMask;

		/* Close socket to ensure packets don't queue on it. not needed anymore as DHCP failed. but still need timer for ARP testing. */
		prvCloseDHCPSocket( pxEndPoint );

		pxEndPoint->xDHCPData.xDHCPTxPeriod = pdMS_TO_TICKS( 3000 + ( ipconfigRAND32() & 0x3ff ) ); /*  do ARP test every (3 + 0-1024mS) seconds. */

		xARPHadIPClash = pdFALSE;	   /* reset flag that shows if have ARP clash. */
		vARPSendGratuitous();
	}

#endif /* ipconfigDHCP_FALL_BACK_AUTO_IP */
/*-----------------------------------------------------------*/

#endif /* ipconfigUSE_DHCP != 0 */


