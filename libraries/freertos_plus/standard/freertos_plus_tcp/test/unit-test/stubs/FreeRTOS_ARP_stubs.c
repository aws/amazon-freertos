/* IPv4 multi-cast addresses range from 224.0.0.0.0 to 240.0.0.0. */
#define	ipFIRST_MULTI_CAST_IPv4		0xE0000000UL
#define	ipLAST_MULTI_CAST_IPv4		0xF0000000UL

/* For convenience, a MAC address of all 0xffs is defined const for quick
reference. */
const MACAddress_t xBroadcastMACAddress = { { 0xff, 0xff, 0xff, 0xff, 0xff, 0xff } };

/* Structure that stores the netmask, gateway address and DNS server addresses. */
NetworkAddressingParameters_t xNetworkAddressing = { 0, 0, 0, 0, 0 };

/* The expected IP version and header length coded into the IP header itself. */
#define ipIP_VERSION_AND_HEADER_LENGTH_BYTE ( ( uint8_t ) 0x45 )

BaseType_t xIsIPv4Multicast( uint32_t ulIPAddress )
{
BaseType_t xReturn;
uint32_t ulIP = FreeRTOS_ntohl( ulIPAddress );

	if( ( ulIP >= ipFIRST_MULTI_CAST_IPv4 ) && ( ulIP < ipLAST_MULTI_CAST_IPv4 ) )
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

void vSetMultiCastIPv4MacAddress( uint32_t ulIPAddress, MACAddress_t *pxMACAddress )
{
uint32_t ulIP = FreeRTOS_ntohl( ulIPAddress );

	pxMACAddress->ucBytes[ 0 ] = ( uint8_t ) 0x01U;
	pxMACAddress->ucBytes[ 1 ] = ( uint8_t ) 0x00U;
	pxMACAddress->ucBytes[ 2 ] = ( uint8_t ) 0x5EU;
	pxMACAddress->ucBytes[ 3 ] = ( uint8_t ) ( ( ulIP >> 16 ) & 0x7fU );	/* Use 7 bits. */
	pxMACAddress->ucBytes[ 4 ] = ( uint8_t ) ( ( ulIP >>  8 ) & 0xffU );	/* Use 8 bits. */
	pxMACAddress->ucBytes[ 5 ] = ( uint8_t ) ( ( ulIP       ) & 0xffU );	/* Use 8 bits. */
}
/*-----------------------------------------------------------*/

TickType_t xTaskGetTickCount( void )
{
    TickType_t xTicks;

    return xTicks;
}

BaseType_t xSendEventToIPTask( eIPEvent_t eEvent )
{
	return 0;
}
/*-----------------------------------------------------------*/

BaseType_t xNetworkInterfaceOutput( NetworkBufferDescriptor_t * const pxNetworkBuffer, BaseType_t bReleaseAfterSend )
{
	return pdPASS;
}
/*-----------------------------------------------------------*/

NetworkBufferDescriptor_t *pxGetNetworkBufferWithDescriptor( size_t xRequestedSizeBytes, TickType_t xBlockTimeTicks )
{
	return NULL;
}
/*-----------------------------------------------------------*/

BaseType_t xIsCallingFromIPTask( void )
{
BaseType_t xReturn = 0;

	return xReturn;
}
/*-----------------------------------------------------------*/

void vReleaseNetworkBufferAndDescriptor( NetworkBufferDescriptor_t * const pxNetworkBuffer )
{
}
/*-----------------------------------------------------------*/

BaseType_t xSendEventStructToIPTask( const IPStackEvent_t *pxEvent, TickType_t uxTimeout )
{
BaseType_t xReturn;
	return xReturn;
}
/*-----------------------------------------------------------*/

NetworkBufferDescriptor_t *pxDuplicateNetworkBufferWithDescriptor( const NetworkBufferDescriptor_t * const pxNetworkBuffer,
	size_t uxNewLength )
{
NetworkBufferDescriptor_t * pxNewBuffer;

	return pxNewBuffer;
}
/*-----------------------------------------------------------*/


UDPPacketHeader_t xDefaultPartUDPPacketHeader =
{
	/* .ucBytes : */
	{
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 	/* Ethernet source MAC address. */
		0x08, 0x00, 							/* Ethernet frame type. */
		ipIP_VERSION_AND_HEADER_LENGTH_BYTE, 	/* ucVersionHeaderLength. */
		0x00, 									/* ucDifferentiatedServicesCode. */
		0x00, 0x00, 							/* usLength. */
		0x00, 0x00, 							/* usIdentification. */
		0x00, 0x00, 							/* usFragmentOffset. */
		ipconfigUDP_TIME_TO_LIVE, 				/* ucTimeToLive */
		ipPROTOCOL_UDP, 						/* ucProtocol. */
		0x00, 0x00, 							/* usHeaderChecksum. */
		0x00, 0x00, 0x00, 0x00 					/* Source IP address. */
	}
};

