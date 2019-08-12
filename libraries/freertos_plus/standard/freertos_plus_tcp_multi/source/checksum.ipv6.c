uint16_t usGenerateProtocolChecksum_new( const uint8_t * const pucEthernetBuffer, size_t uxBufferLength, BaseType_t xOutgoingPacket )
{
uint32_t ulLength;
uint16_t usChecksum, *pusChecksum, usPayloadLength, usProtolBytes;
const IPPacket_t * pxIPPacket;
size_t uxIPHeaderLength;
ProtocolHeaders_t *pxProtocolHeaders;
uint8_t ucProtocol;
int location = 0;
#if( ipconfigUSE_IPv6 != 0 )
	BaseType_t xIsIPv6;
	const IPHeader_IPv6_t * pxIPPacket_IPv6;
	uint32_t pulHeader[ 2 ];
#endif

#if( ipconfigHAS_DEBUG_PRINTF != 0 )
	const char *pcType;
#endif

	/* Parse the packet length. */
	pxIPPacket = ipPOINTER_CAST( const IPPacket_t *, pucEthernetBuffer );

	#if( ipconfigUSE_IPv6 != 0 )
	pxIPPacket_IPv6 = ( const IPHeader_IPv6_t * )( pucEthernetBuffer + ipSIZE_OF_ETH_HEADER );
	if( pxIPPacket->xEthernetHeader.usFrameType == ipIPv6_FRAME_TYPE )
	{
		xIsIPv6 = pdTRUE;
	}
	else
	{
		xIsIPv6 = pdFALSE;
	}

	if( xIsIPv6 != pdFALSE )
	{
		uxIPHeaderLength = ipSIZE_OF_IPv6_HEADER;

		/* Check for minimum packet size: ipSIZE_OF_ETH_HEADER + ipSIZE_OF_IPv6_HEADER (54 bytes) */
		if( uxBufferLength < sizeof( IPPacket_IPv6_t ) )
		{
			usChecksum = ipINVALID_LENGTH;
			location = 1;
			goto eror_exit;
		}

		ucProtocol = pxIPPacket_IPv6->ucNextHeader;
		pxProtocolHeaders = ( ProtocolHeaders_t * ) ( pucEthernetBuffer + ipSIZE_OF_ETH_HEADER + ipSIZE_OF_IPv6_HEADER );
		usPayloadLength = FreeRTOS_ntohs( pxIPPacket_IPv6->usPayloadLength );
		/* For IPv6, the number of bytes in the protocol is indicated. */
		usProtolBytes = usPayloadLength;
		if( uxBufferLength < ( size_t ) ( ipSIZE_OF_ETH_HEADER + ipSIZE_OF_IPv6_HEADER + usPayloadLength ) )
		{
			usChecksum = ipINVALID_LENGTH;
			location = 5;
			goto eror_exit;
		}
	}
	else
	#endif
	{
		/* Check for minimum packet size ( 14 + 20 bytes ). */
		if( uxBufferLength < sizeof( IPPacket_t ) )
		{
			usChecksum = ipINVALID_LENGTH;
			location = 3;
			goto eror_exit;
		}
		uxIPHeaderLength = ( UBaseType_t ) ( sizeof( uint32_t ) * ( pxIPPacket->xIPHeader.ucVersionHeaderLength & 0x0Fu ) );

		/* Check for minimum packet size. */
		if( uxBufferLength < ( ipSIZE_OF_ETH_HEADER + uxIPHeaderLength ) )
		{
			usChecksum = ipINVALID_LENGTH;
			location = 4;
			goto eror_exit;
		}
		/* xIPHeader.usLength is the total length, minus the Ethernet header. */
		usPayloadLength = FreeRTOS_ntohs( pxIPPacket->xIPHeader.usLength );
		if( uxBufferLength < ( size_t ) ( ipSIZE_OF_ETH_HEADER + usPayloadLength ) )
		{
			usChecksum = ipINVALID_LENGTH;
			location = 5;
			goto eror_exit;
		}

		/* Identify the next protocol. */
		ucProtocol = pxIPPacket->xIPHeader.ucProtocol;
		pxProtocolHeaders = ( ProtocolHeaders_t * ) ( pucEthernetBuffer + ipSIZE_OF_ETH_HEADER + uxIPHeaderLength );
		/* For IPv4, the number of bytes in IP-header + the protocol is indicated. */
		usProtolBytes = usPayloadLength - uxIPHeaderLength;
	}
	/* Compare 'uxBufferLength' with the total expected length of the packet. */
	if( uxBufferLength < ( size_t ) ( ipSIZE_OF_ETH_HEADER + ( usPayloadLength - 0 ) ) )
	{
		usChecksum = ipINVALID_LENGTH;
		location = 2;
		goto eror_exit;
	}

	/* Switch on the Layer 3/4 protocol. */
	if( ucProtocol == ( uint8_t ) ipPROTOCOL_UDP )
	{
		if( ( usProtolBytes < ipSIZE_OF_UDP_HEADER ) ||
			( uxBufferLength < ( ipSIZE_OF_ETH_HEADER + uxIPHeaderLength + ipSIZE_OF_UDP_HEADER ) ) )
		{
			usChecksum = ipINVALID_LENGTH;
			location = 6;
			goto eror_exit;
		}

		pusChecksum = ( uint16_t * ) ( &( pxProtocolHeaders->xUDPHeader.usChecksum ) );
		#if( ipconfigHAS_DEBUG_PRINTF != 0 )
		{
			pcType = "UDP";
		}
		#endif	/* ipconfigHAS_DEBUG_PRINTF != 0 */
	}
	else if( ucProtocol == ( uint8_t ) ipPROTOCOL_TCP )
	{
		if( ( usProtolBytes < ipSIZE_OF_TCP_HEADER ) ||
			( uxBufferLength < ( ipSIZE_OF_ETH_HEADER + uxIPHeaderLength + ipSIZE_OF_TCP_HEADER ) ) )
		{
			usChecksum = ipINVALID_LENGTH;
			location = 7;
			goto eror_exit;
		}

		pusChecksum = ( uint16_t * ) ( &( pxProtocolHeaders->xTCPHeader.usChecksum ) );
		#if( ipconfigHAS_DEBUG_PRINTF != 0 )
		{
			pcType = "TCP";
		}
		#endif	/* ipconfigHAS_DEBUG_PRINTF != 0 */
	}
	else if( ( ucProtocol == ( uint8_t ) ipPROTOCOL_ICMP ) ||
			( ucProtocol == ( uint8_t ) ipPROTOCOL_IGMP ) )
	{
		if( ( usProtolBytes < ipSIZE_OF_ICMPv4_HEADER ) ||
			( uxBufferLength < ( ipSIZE_OF_ETH_HEADER + uxIPHeaderLength + ipSIZE_OF_ICMPv4_HEADER ) ) )
		{
			usChecksum = ipINVALID_LENGTH;
			location = 8;
			goto eror_exit;
		}

		pusChecksum = ( uint16_t * ) ( &( pxProtocolHeaders->xICMPHeader.usChecksum ) );

		#if( ipconfigHAS_DEBUG_PRINTF != 0 )
		{
			if( ucProtocol == ( uint8_t ) ipPROTOCOL_ICMP )
			{
				pcType = "ICMP";
			}
			else
			{
				pcType = "IGMP";
			}
		}
		#endif	/* ipconfigHAS_DEBUG_PRINTF != 0 */
	}
#if( ipconfigUSE_IPv6 != 0 )
	else if( ucProtocol == ipPROTOCOL_ICMP_IPv6 )
	{
	size_t xNeeded;
		switch( pxProtocolHeaders->xICMPHeader_IPv6.ucTypeOfMessage )
		{
			case ipICMP_PING_REQUEST_IPv6:
			case ipICMP_PING_REPLY_IPv6:
				xNeeded = sizeof( ICMPEcho_IPv6_t );
				break;
			default:
				xNeeded = ipSIZE_OF_ICMPv6_HEADER;
				break;
		}
		if( uxBufferLength < ( ipSIZE_OF_ETH_HEADER + uxIPHeaderLength + xNeeded ) )
		{
			usChecksum = ipINVALID_LENGTH;
			location = 9;
			goto eror_exit;
		}
		pusChecksum = ( uint16_t * ) ( &( pxProtocolHeaders->xICMPHeader.usChecksum ) );
		#if( ipconfigHAS_DEBUG_PRINTF != 0 )
		{
			pcType = "ICMP_IPv6";
		}
		#endif	/* ipconfigHAS_DEBUG_PRINTF != 0 */
	}
#endif
	else
	{
		/* Unhandled protocol, other than ICMP, IGMP, UDP, or TCP. */
		//return ipUNHANDLED_PROTOCOL;/*lint !e904 Return statement before end of function [MISRA 2012 Rule 15.5, advisory] */
		usChecksum = ipUNHANDLED_PROTOCOL;
		location = 10;
		goto eror_exit;
	}

	if( xOutgoingPacket != pdFALSE )
	{
		/* This is an outgoing packet. Before calculating the checksum, set it
		to zero. */
		*( pusChecksum ) = 0u;
	}
	else if( ( *pusChecksum == 0u ) && ( ucProtocol == ( uint8_t ) ipPROTOCOL_UDP ) )
	{
		/* Sender hasn't set the checksum, no use to calculate it. */
		//return ipCORRECT_CRC;/*lint !e904 Return statement before end of function [MISRA 2012 Rule 15.5, advisory] */
		usChecksum = ipCORRECT_CRC;
		location = 11;
		goto eror_exit;
	}
	else
	{
		/* This incoming packet has a checksum error. */
	}

	#if( ipconfigUSE_IPv6 != 0 )
	if( xIsIPv6 != pdFALSE )
	{
		ulLength = ( uint32_t ) usPayloadLength;
		/* IPv6 has a 40-byte pseudo header:
		 0..15 Source IPv6 address
		16..31 Target IPv6 address
		32..35 Length of payload
		36..38 three zero's
		39 Next Header, i.e. the protocol type. */

		pulHeader[ 0 ] = FreeRTOS_htonl( ( uint32_t ) usPayloadLength );
		pulHeader[ 1 ] = FreeRTOS_htonl( ( uint32_t ) pxIPPacket_IPv6->ucNextHeader );

		usChecksum = usGenerateChecksum( ( uint32_t ) 0, ( uint8_t * )&( pxIPPacket_IPv6->xSourceIPv6Address ),
				( BaseType_t )( 2 * sizeof( pxIPPacket_IPv6->xSourceIPv6Address ) ) );

		usChecksum = usGenerateChecksum( ( uint32_t ) usChecksum, ( uint8_t * )&( pulHeader ),
				( BaseType_t )( sizeof( pulHeader ) ) );
	}
	else
	#endif
	{
		ulLength = ( uint32_t )
			( usPayloadLength - ( ( uint16_t ) uxIPHeaderLength ) ); /* normally minus 20 */
		usChecksum = 0;
	}

	if( ( ulLength < sizeof( pxProtocolHeaders->xUDPHeader ) ) ||
		( ulLength > ( uint32_t )( ipconfigNETWORK_MTU - uxIPHeaderLength ) ) )
	{
		#if( ipconfigHAS_DEBUG_PRINTF != 0 )
		{
			FreeRTOS_debug_printf( ( "usGenerateProtocolChecksum[%s]: len invalid: %lu\n", pcType, ulLength ) );
		}
		#endif	/* ipconfigHAS_DEBUG_PRINTF != 0 */

		/* Again, in a 16-bit return value there is no space to indicate an
		error.  For incoming packets, 0x1234 will cause dropping of the packet.
		For outgoing packets, there is a serious problem with the
		format/length */
		//return ipINVALID_LENGTH;/*lint !e904 Return statement before end of function [MISRA 2012 Rule 15.5, advisory] */
		usChecksum = ipINVALID_LENGTH;
		location = 12;
		goto eror_exit;
	}
	if( ucProtocol <= ( uint8_t ) ipPROTOCOL_IGMP )
	{
		/* ICMP/IGMP do not have a pseudo header for CRC-calculation. */
		usChecksum = ( uint16_t )
			( ~usGenerateChecksum( 0uL,
				( uint8_t * ) &( pxProtocolHeaders->xICMPHeader ), ( size_t ) ulLength ) );
	}
	#if( ipconfigUSE_IPv6 != 0 )
	else if( ucProtocol == ipPROTOCOL_ICMP_IPv6 )
	{
		usChecksum = ( uint16_t )
			( ~usGenerateChecksum( usChecksum,
				( uint8_t * ) &( pxProtocolHeaders->xTCPHeader ), ( BaseType_t ) ulLength ) );
	}
	#endif /* ipconfigUSE_IPv6 */
	else
	{
		#if( ipconfigUSE_IPv6 != 0 )
		if( xIsIPv6 != pdFALSE )
		{
			/* The CRC of the IPv6 pseudo-header has already been calculated. */
			usChecksum = ( uint16_t )
				( ~usGenerateChecksum( ( uint32_t ) usChecksum, ( uint8_t * )&( pxProtocolHeaders->xUDPHeader.usSourcePort ),
					( BaseType_t )( ulLength ) ) );
		}
		else
		#endif /* ipconfigUSE_IPv6 */
		{
			/* For UDP and TCP, sum the pseudo header, i.e. IP protocol + length
			fields */
			usChecksum = ( uint16_t ) ( ulLength + ( ( uint16_t ) ucProtocol ) );

			/* And then continue at the IPv4 source and destination addresses. */
			usChecksum = ( uint16_t )
				( ~usGenerateChecksum( ( uint32_t ) usChecksum, ( const uint8_t * )&( pxIPPacket->xIPHeader.ulSourceIPAddress ),
					( size_t )( ( 2u * sizeof( pxIPPacket->xIPHeader.ulSourceIPAddress ) ) + ulLength ) ) );
		}
		/* Sum TCP header and data. */
	}

	if( xOutgoingPacket == pdFALSE )
	{
		/* This is in incoming packet. If the CRC is correct, it should be zero. */
		if( usChecksum == 0u )
		{
			usChecksum = ( uint16_t )ipCORRECT_CRC;
		}
	}
	else
	{
		if( ( usChecksum == 0u ) && ( ucProtocol == ( uint8_t ) ipPROTOCOL_UDP ) )
		{
			/* In case of UDP, a calculated checksum of 0x0000 is transmitted
			as 0xffff. A value of zero would mean that the checksum is not used. */
			#if( ipconfigHAS_DEBUG_PRINTF != 0 )
			{
				if( xOutgoingPacket != pdFALSE )
				{
					FreeRTOS_debug_printf( ( "usGenerateProtocolChecksum[%s]: crc swap: %04X\n", pcType, usChecksum ) );
				}
			}
			#endif	/* ipconfigHAS_DEBUG_PRINTF != 0 */

			usChecksum = ( uint16_t )0xffffu;
		}
	}
	usChecksum = FreeRTOS_htons( usChecksum );

	if( xOutgoingPacket != pdFALSE )
	{
		*( pusChecksum ) = usChecksum;
	}
	#if( ipconfigHAS_DEBUG_PRINTF != 0 )
	else if( ( xOutgoingPacket == pdFALSE ) && ( usChecksum != ipCORRECT_CRC ) )
	{
	uint16_t usGot, usCalculated;
		usGot = *pusChecksum;
		usCalculated = ~usGenerateProtocolChecksum( pucEthernetBuffer, uxBufferLength, pdTRUE );
		FreeRTOS_debug_printf( ( "usGenerateProtocolChecksum[%s]: len %ld ID %04X: from %lxip to %lxip cal %04X got %04X\n",
			pcType,
			ulLength,
			FreeRTOS_ntohs( pxIPPacket->xIPHeader.usIdentification ),
			FreeRTOS_ntohl( pxIPPacket->xIPHeader.ulSourceIPAddress ),
			FreeRTOS_ntohl( pxIPPacket->xIPHeader.ulDestinationIPAddress ),
			FreeRTOS_ntohs( usCalculated ),
			FreeRTOS_ntohs( usGot ) ) );
	}
	#endif	/* ipconfigHAS_DEBUG_PRINTF != 0 */

eror_exit:
	if( ( usChecksum == ipUNHANDLED_PROTOCOL ) || 
		( usChecksum == ipINVALID_LENGTH ) )
	{
		FreeRTOS_printf( ( "CRC error: %04x location %d\n", usChecksum, location ) );
	}

	return usChecksum;
}
/*-----------------------------------------------------------*/
