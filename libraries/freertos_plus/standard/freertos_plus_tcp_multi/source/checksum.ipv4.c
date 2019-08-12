uint16_t usGenerateProtocolChecksum( const uint8_t * const pucEthernetBuffer, size_t uxBufferLength, BaseType_t xOutgoingPacket )
{
uint32_t ulLength;
uint16_t usChecksum, *pusChecksum;
const IPPacket_t * pxIPPacket;
UBaseType_t uxIPHeaderLength;
ProtocolPacket_t *pxProtPack;
uint8_t ucProtocol;
#if( ipconfigHAS_DEBUG_PRINTF != 0 )
	const char *pcType;
#endif

int location = 0;

	/* Check for minimum packet size. */
	if( uxBufferLength < sizeof( IPPacket_t ) )
	{
	 	usChecksum = ipINVALID_LENGTH;
		location = 1;
	 	goto eror_exit;
	}

	/* Parse the packet length. */
	pxIPPacket = ( const IPPacket_t * ) pucEthernetBuffer;

	/* Per https://tools.ietf.org/html/rfc791, the four-bit Internet Header
	Length field contains the length of the internet header in 32-bit words. */
	uxIPHeaderLength = ( UBaseType_t ) ( sizeof( uint32_t ) * ( pxIPPacket->xIPHeader.ucVersionHeaderLength & 0x0Fu ) );

	/* Check for minimum packet size. */
	if( uxBufferLength < sizeof( IPPacket_t ) + uxIPHeaderLength - ipSIZE_OF_IPv4_HEADER )
	{
	 	usChecksum = ipINVALID_LENGTH;
		location = 4;
	 	goto eror_exit;
	}
	if( uxBufferLength < ( size_t ) ( ipSIZE_OF_ETH_HEADER + FreeRTOS_ntohs( pxIPPacket->xIPHeader.usLength ) ) )
	{
	 	usChecksum = ipINVALID_LENGTH;
		location = 5;
	 	goto eror_exit;
	}

	/* Identify the next protocol. */
	ucProtocol = pxIPPacket->xIPHeader.ucProtocol;

	/* N.B., if this IP packet header includes Options, then the following
	assignment results in a pointer into the protocol packet with the Ethernet
	and IP headers incorrectly aligned. However, either way, the "third"
	protocol (Layer 3 or 4) header will be aligned, which is the convenience
	of this calculation. */
	pxProtPack = ( ProtocolPacket_t * ) ( pucEthernetBuffer + ( uxIPHeaderLength - ipSIZE_OF_IPv4_HEADER ) );

	/* Switch on the Layer 3/4 protocol. */
	if( ucProtocol == ( uint8_t ) ipPROTOCOL_UDP )
	{
		if( ( usPayloadLength < ipSIZE_OF_UDP_HEADER ) ||
			( uxBufferLength < ( ipSIZE_OF_ETH_HEADER + uxIPHeaderLength + ipSIZE_OF_UDP_HEADER ) ) )
		{
			usChecksum = ipINVALID_LENGTH;
			location = 6;
			goto eror_exit;
		}

		pusChecksum = ( uint16_t * ) ( &( pxProtPack->xUDPPacket.xUDPHeader.usChecksum ) );
		#if( ipconfigHAS_DEBUG_PRINTF != 0 )
		{
			pcType = "UDP";
		}
		#endif	/* ipconfigHAS_DEBUG_PRINTF != 0 */
	}
	else if( ucProtocol == ( uint8_t ) ipPROTOCOL_TCP )
	{
		if( ( usPayloadLength < ipSIZE_OF_TCP_HEADER ) ||
			( uxBufferLength < ( ipSIZE_OF_ETH_HEADER + uxIPHeaderLength + ipSIZE_OF_TCP_HEADER ) ) )
		{
			usChecksum = ipINVALID_LENGTH;
			location = 7;
			goto eror_exit;
		}

		pusChecksum = ( uint16_t * ) ( &( pxProtPack->xTCPPacket.xTCPHeader.usChecksum ) );
		#if( ipconfigHAS_DEBUG_PRINTF != 0 )
		{
			pcType = "TCP";
		}
		#endif	/* ipconfigHAS_DEBUG_PRINTF != 0 */
	}
	else if( ( ucProtocol == ( uint8_t ) ipPROTOCOL_ICMP ) ||
			( ucProtocol == ( uint8_t ) ipPROTOCOL_IGMP ) )
	{
		if( ( usPayloadLength < ipSIZE_OF_ICMPv4_HEADER ) ||
			( uxBufferLength < ( ipSIZE_OF_ETH_HEADER + uxIPHeaderLength + ipSIZE_OF_ICMPv4_HEADER ) ) )
		{
			usChecksum = ipINVALID_LENGTH;
			location = 8;
			goto eror_exit;
		}

		pusChecksum = ( uint16_t * ) ( &( pxProtPack->xICMPPacket.xICMPHeader.usChecksum ) );
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
	else
	{
		/* Unhandled protocol, other than ICMP, IGMP, UDP, or TCP. */
		usChecksum = ipUNHANDLED_PROTOCOL;
		location = 10;
		goto eror_exit;
	}

	/* The protocol and checksum field have been identified. Check the direction
	of the packet. */
	if( xOutgoingPacket != pdFALSE )
	{
		/* This is an outgoing packet. Before calculating the checksum, set it
		to zero. */
		*( pusChecksum ) = 0u;
	}
	else if( ( *pusChecksum == 0u ) && ( ucProtocol == ( uint8_t ) ipPROTOCOL_UDP ) )
	{
		/* Sender hasn't set the checksum, no use to calculate it. */
		usChecksum = ipCORRECT_CRC;
		location = 11;
		goto eror_exit;
	}

	ulLength = ( uint32_t )
		( FreeRTOS_ntohs( pxIPPacket->xIPHeader.usLength ) - ( ( uint16_t ) uxIPHeaderLength ) ); /* normally minus 20 */

	if( ( ulLength < sizeof( pxProtPack->xUDPPacket.xUDPHeader ) ) ||
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
		usChecksum = ipINVALID_LENGTH;
		location = 12;
		goto eror_exit;
	}
	if( ucProtocol <= ( uint8_t ) ipPROTOCOL_IGMP )
	{
		/* ICMP/IGMP do not have a pseudo header for CRC-calculation. */
		usChecksum = ( uint16_t )
			( ~usGenerateChecksum( 0UL,
				( uint8_t * ) &( pxProtPack->xTCPPacket.xTCPHeader ), ( size_t ) ulLength ) );
	}
	else
	{
		/* For UDP and TCP, sum the pseudo header, i.e. IP protocol + length
		fields */
		usChecksum = ( uint16_t ) ( ulLength + ( ( uint16_t ) ucProtocol ) );

		/* And then continue at the IPv4 source and destination addresses. */
		usChecksum = ( uint16_t )
			( ~usGenerateChecksum( ( uint32_t ) usChecksum, ( uint8_t * )&( pxIPPacket->xIPHeader.ulSourceIPAddress ),
				( 2u * sizeof( pxIPPacket->xIPHeader.ulSourceIPAddress ) + ulLength ) ) );

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
		FreeRTOS_debug_printf( ( "usGenerateProtocolChecksum[%s]: ID %04X: from %lxip to %lxip bad crc: %04X\n",
			pcType,
			FreeRTOS_ntohs( pxIPPacket->xIPHeader.usIdentification ),
			FreeRTOS_ntohl( pxIPPacket->xIPHeader.ulSourceIPAddress ),
			FreeRTOS_ntohl( pxIPPacket->xIPHeader.ulDestinationIPAddress ),
			FreeRTOS_ntohs( *pusChecksum ) ) );
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
