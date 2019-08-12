/*
FreeRTOS+TCP V2.0.10
Copyright (C) 2017 Amazon.com, Inc. or its affiliates.  All Rights Reserved.

Permission is hereby granted, free of charge, to any person obtaining a copy of
this software and associated documentation files (the "Software"), to deal in
the Software without restriction, including without limitation the rights to
use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
the Software, and to permit persons to whom the Software is furnished to do so,
subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

 http://aws.amazon.com/freertos
 http://www.FreeRTOS.org
 */

/* Standard includes. */
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

/* FreeRTOS includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"

/* FreeRTOS+TCP includes. */
#include "FreeRTOS_IP.h"
#include "FreeRTOS_Sockets.h"
#include "FreeRTOS_IP_Private.h"
#include "FreeRTOS_ARP.h"
#include "NetworkBufferManagement.h"
#include "NetworkInterface.h"
#include "FreeRTOS_DHCP.h"
#include "FreeRTOS_DNS.h"
#include "FreeRTOS_Routing.h"

/* Xilinx library files. */
#include <xemacps.h>
#include "Zynq/x_topology.h"
#include "Zynq/x_emacpsif.h"
#include "Zynq/x_emacpsif_hw.h"

/* Provided memory configured as uncached. */
#include "uncached_memory.h"

#ifndef	niBMSR_LINK_STATUS
	#define niBMSR_LINK_STATUS            0x0004UL
#endif

#ifndef	PHY_LS_HIGH_CHECK_TIME_MS
	/* Check if the LinkSStatus in the PHY is still high after 15 seconds of not
	receiving packets. */
	#define PHY_LS_HIGH_CHECK_TIME_MS	15000
#endif

#ifndef	PHY_LS_LOW_CHECK_TIME_MS
	/* Check if the LinkSStatus in the PHY is still low every second. */
	#define PHY_LS_LOW_CHECK_TIME_MS	1000
#endif

/* The size of each buffer when BufferAllocation_1 is used:
http://www.freertos.org/FreeRTOS-Plus/FreeRTOS_Plus_TCP/Embedded_Ethernet_Buffer_Management.html */
#define niBUFFER_1_PACKET_SIZE		1536

/* Naming and numbering of PHY registers. */
#define PHY_REG_01_BMSR			0x01	/* Basic mode status register */

#ifndef iptraceEMAC_TASK_STARTING
	#define iptraceEMAC_TASK_STARTING()	do { } while( 0 )
#endif

/* Default the size of the stack used by the EMAC deferred handler task to twice
the size of the stack used by the idle task - but allow this to be overridden in
FreeRTOSConfig.h as configMINIMAL_STACK_SIZE is a user definable constant. */
#ifndef configEMAC_TASK_STACK_SIZE
	#define configEMAC_TASK_STACK_SIZE ( 2 * configMINIMAL_STACK_SIZE )
#endif

static NetworkInterface_t *pxMyInterfaces[ XPAR_XEMACPS_NUM_INSTANCES ];

/*-----------------------------------------------------------*/

/*
 * Look for the link to be up every few milliseconds until either xMaxTime time
 * has passed or a link is found.
 */
static BaseType_t prvGMACWaitLS( BaseType_t xEMACIndex, TickType_t xMaxTime );

/*
 * A deferred interrupt handler for all MAC/DMA interrupt sources.
 */
static void prvEMACHandlerTask( void *pvParameters );

/* FreeRTOS+TCP/multi :
Each network device has 3 access functions:
- initialise the device
- output a network packet
- return the PHY link-status (LS)
They can be defined as static because their addresses will be
stored in struct NetworkInterface_t. */

static BaseType_t xZynqNetworkInterfaceInitialise( NetworkInterface_t *pxInterface );

static BaseType_t xZynqNetworkInterfaceOutput( NetworkInterface_t *pxInterface, NetworkBufferDescriptor_t * const pxBuffer, BaseType_t bReleaseAfterSend );

static BaseType_t xZynqGetPhyLinkStatus( NetworkInterface_t *pxInterface );

/*-----------------------------------------------------------*/

/* EMAC data/descriptions. */
static xemacpsif_s xEMACpsifs[ XPAR_XEMACPS_NUM_INSTANCES ];
struct xtopology_t xXTopologies[ XPAR_XEMACPS_NUM_INSTANCES ] =
{
	[0] = {
		.emac_baseaddr = XPAR_PS7_ETHERNET_0_BASEADDR,
		.emac_type = xemac_type_emacps,
		.intc_baseaddr = 0x0,
		.intc_emac_intr = 0x0,
		.scugic_baseaddr = XPAR_PS7_SCUGIC_0_BASEADDR,
		.scugic_emac_intr = 0x36,
	},
	[1] = {
		.emac_baseaddr = XPAR_PS7_ETHERNET_1_BASEADDR,
		.emac_type = xemac_type_emacps,
		.intc_baseaddr = 0x0,
		.intc_emac_intr = 0x0,
		.scugic_baseaddr = XPAR_PS7_SCUGIC_0_BASEADDR,
		.scugic_emac_intr = 0x4D,	// See "7.2.3 Shared Peripheral Interrupts (SPI)"
	},
};

XEmacPs_Config mac_configs[ XPAR_XEMACPS_NUM_INSTANCES ] =
{
	[0] = {
		.DeviceId = XPAR_PS7_ETHERNET_0_DEVICE_ID,	/**< Unique ID  of device, used for 'xEMACIndex' */
		.BaseAddress = XPAR_PS7_ETHERNET_0_BASEADDR /**< Physical base address of IPIF registers */
	},
	[1] = {
		.DeviceId = XPAR_PS7_ETHERNET_1_DEVICE_ID,	/**< Unique ID  of device */
		.BaseAddress = XPAR_PS7_ETHERNET_1_BASEADDR /**< Physical base address of IPIF registers */
	},
};

extern int phy_detected[2];

/* A copy of PHY register 1: 'PHY_REG_01_BMSR' */
static uint32_t ulPHYLinkStates[ XPAR_XEMACPS_NUM_INSTANCES ];

#if( CONFIG_USE_LWIP != 0 )
	extern unsigned char ucLWIP_Mac_Address[6];// = { 0x00, 0x0a, 0x35, 0x00, 0x01, 0x02 };
#endif

/* Holds the handle of the task used as a deferred interrupt processor.  The
handle is used so direct notifications can be sent to the task for all EMAC/DMA
related interrupts. */
TaskHandle_t xEMACTaskHandles[ XPAR_XEMACPS_NUM_INSTANCES ];

/*-----------------------------------------------------------*/

void vInitialiseOnIndex( int iIndex )
{
NetworkInterface_t *pxInterface;
BaseType_t xEMACIndex;

/* Get the first Network Interface. */
	for( pxInterface = FreeRTOS_FirstNetworkInterface();
		pxInterface != NULL;
		pxInterface = FreeRTOS_NextNetworkInterface( pxInterface ) )
	{
		xEMACIndex = ( BaseType_t )pxInterface->pvArgument;
		if( xEMACIndex == (BaseType_t ) iIndex )
		{
			xZynqNetworkInterfaceInitialise( pxInterface );
			break;
		}
	}
}
/*-----------------------------------------------------------*/

static BaseType_t xZynqNetworkInterfaceInitialise( NetworkInterface_t *pxInterface )
{
uint32_t ulLinkSpeed, ulDMAReg;
BaseType_t xStatus, xLinkStatus;
XEmacPs *pxEMAC_PS;
const TickType_t xWaitLinkDelay = pdMS_TO_TICKS( 7000UL ), xWaitRelinkDelay = pdMS_TO_TICKS( 1000UL );
NetworkEndPoint_t *pxEndPoint;
BaseType_t xEMACIndex = ( BaseType_t )pxInterface->pvArgument;

	/* This function will be called by the IP-task repeatedly
	until it returns pdTRUE.
	So check if the task has already been created. */
	if( xEMACTaskHandles[ xEMACIndex ] == NULL )
	{
	const char *pcTaskName;

		pxMyInterfaces[ xEMACIndex ] = pxInterface;

		pxEMAC_PS = &( xEMACpsifs[ xEMACIndex ].emacps );
		memset( &xEMACpsifs[ xEMACIndex ], '\0', sizeof( xEMACpsifs[ xEMACIndex ] ) );

		xStatus = XEmacPs_CfgInitialize( pxEMAC_PS, &( mac_configs[ xEMACIndex ] ), mac_configs[ xEMACIndex ].BaseAddress);
		if( xStatus != XST_SUCCESS )
		{
			FreeRTOS_printf( ( "xEMACInit: EmacPs Configuration Failed....\n" ) );
		}

		pxEndPoint = FreeRTOS_FirstEndPoint( pxInterface );
		configASSERT( pxEndPoint != NULL );

		/* Initialize the mac and set the MAC address at position 1. */
		XEmacPs_SetMacAddress( pxEMAC_PS, ( void * ) pxEndPoint->xMACAddress.ucBytes, 1 );

		#if( ipconfigUSE_LLMNR == 1 )
		{
			/* Also add LLMNR multicast MAC address. */
			#if( ipconfigUSE_IPv6 == 0 )
			{
				XEmacPs_SetHash( pxEMAC_PS, ( void * )xLLMNR_MacAdress.ucBytes );
			}
			#else
			{
			NetworkEndPoint_t *pxEndPoint;
			NetworkInterface_t *pxInterface = pxMyInterfaces[ xEMACIndex ];

				for( pxEndPoint = FreeRTOS_FirstEndPoint( pxInterface );
					pxEndPoint != NULL;
					pxEndPoint = FreeRTOS_NextEndPoint( pxInterface, pxEndPoint ) )
				{
					if( pxEndPoint->bits.bIPv6 != pdFALSE_UNSIGNED )
					{
						unsigned char ucMACAddress[ 6 ] = { 0x33, 0x33, 0xff, 0, 0, 0 };
						ucMACAddress[ 3 ] = pxEndPoint->ulIPAddress_IPv6.ucBytes[ 13 ];
						ucMACAddress[ 4 ] = pxEndPoint->ulIPAddress_IPv6.ucBytes[ 14 ];
						ucMACAddress[ 5 ] = pxEndPoint->ulIPAddress_IPv6.ucBytes[ 15 ];
						XEmacPs_SetHash( pxEMAC_PS, ( void * )ucMACAddress );
					}
				}
				XEmacPs_SetHash( pxEMAC_PS, ( void * )xLLMNR_MacAdressIPv6.ucBytes );
			}
			#endif
		}
		#endif	/* ipconfigUSE_LLMNR == 1 */

		#if( CONFIG_USE_LWIP == 1 )
		{
			/* Also add LLMNR multicast MAC address. */
			XEmacPs_SetMacAddress( pxEMAC_PS, ( void * )ucLWIP_Mac_Address, 4 );
		}
		#endif	/* CONFIG_USE_LWIP == 1 */

		pxEndPoint = FreeRTOS_NextEndPoint( pxInterface, pxEndPoint );
		if( pxEndPoint != NULL )
		{
			/* If there is a second end-point, store the MAC
			address at position 4.*/
			XEmacPs_SetMacAddress( pxEMAC_PS, ( void * ) pxEndPoint->xMACAddress.ucBytes, 4 );
		}

		// MDIO goes via ETH0 only
		XEmacPs_SetMdioDivisor( pxEMAC_PS, MDC_DIV_224 );
		ulLinkSpeed = Phy_Setup( pxEMAC_PS );
//		XEmacPs_SetOperatingSpeed( pxEMAC_PS, ulLinkSpeed); /* Duplicate */

		/* Setting the operating speed of the MAC needs a delay. */
		vTaskDelay( pdMS_TO_TICKS( 25UL ) );

		ulDMAReg = XEmacPs_ReadReg( pxEMAC_PS->Config.BaseAddress, XEMACPS_DMACR_OFFSET);

		{
		uint32_t ulValue = XEmacPs_ReadReg( pxEMAC_PS->Config.BaseAddress, XEMACPS_NWCFG_OFFSET);
			/* Allow the use of hased MAC addresses. */
			ulValue |= XEMACPS_NWCFG_MCASTHASHEN_MASK;
			XEmacPs_WriteReg( pxEMAC_PS->Config.BaseAddress, XEMACPS_NWCFG_OFFSET, ulValue);
		}
		/* DISC_WHEN_NO_AHB: when set, the GEM DMA will automatically discard receive
		packets from the receiver packet buffer memory when no AHB resource is available. */
		XEmacPs_WriteReg( pxEMAC_PS->Config.BaseAddress, XEMACPS_DMACR_OFFSET,
			ulDMAReg | XEMACPS_DMACR_DISC_WHEN_NO_AHB_MASK);

		setup_isr( &( xEMACpsifs[ xEMACIndex ] ) );
		init_dma( &( xEMACpsifs[ xEMACIndex ] ) );
		start_emacps( &( xEMACpsifs[ xEMACIndex ] ) );

		prvGMACWaitLS( xEMACIndex, xWaitLinkDelay );

		/* The deferred interrupt handler task is created at the highest
		possible priority to ensure the interrupt handler can return directly
		to it.  The task's handle is stored in xEMACTaskHandles[] so interrupts can
		notify the task when there is something to process. */
		if( xEMACIndex == 0 )
		{
			pcTaskName = "GEM0";
		}
		else
		{
			pcTaskName = "GEM1";
		}
		xTaskCreate( prvEMACHandlerTask, pcTaskName, configEMAC_TASK_STACK_SIZE, ( void * )xEMACIndex, configMAX_PRIORITIES - 1, &xEMACTaskHandles[ xEMACIndex ] );
	}
	else
	{
		/* Initialisation was already performed, just wait for the link. */
		prvGMACWaitLS( xEMACIndex, xWaitRelinkDelay );
	}

	/* Only return pdTRUE when the Link Status of the PHY is high, otherwise the
	DHCP process and all other communication will fail. */
	xLinkStatus = xZynqGetPhyLinkStatus( pxInterface );

//	return ( xLinkStatus != pdFALSE );
	return pdTRUE; // Workaround because network buffers are not freed when xZynqNetworkInterfaceInitialise() did not complete
}
/*-----------------------------------------------------------*/

static BaseType_t xZynqNetworkInterfaceOutput( NetworkInterface_t *pxInterface, NetworkBufferDescriptor_t * const pxBuffer, BaseType_t bReleaseAfterSend )
{
BaseType_t xEMACIndex = ( BaseType_t ) pxInterface->pvArgument;

	if( ( ulPHYLinkStates[ xEMACIndex ] & niBMSR_LINK_STATUS ) != 0 )
	{
		iptraceNETWORK_INTERFACE_TRANSMIT();
		/* emacps_send_message() will delete the network buffer if necessary. */
		emacps_send_message( &( xEMACpsifs[ xEMACIndex ] ), pxBuffer, bReleaseAfterSend );
	}
	else if( bReleaseAfterSend != pdFALSE )
	{
		/* No link. */
		vReleaseNetworkBufferAndDescriptor( pxBuffer );
	}

	return pdTRUE;
}
/*-----------------------------------------------------------*/

static inline unsigned long ulReadMDIO( BaseType_t xEMACIndex, unsigned ulRegister )
{
uint16_t usValue;

	// Always ETH0 because both PHYs are connected to ETH0 MDIO
	XEmacPs_PhyRead( &( xEMACpsifs[ 0 ].emacps ), phy_detected[xEMACIndex], ulRegister, &usValue );
	return usValue;
}
/*-----------------------------------------------------------*/

static BaseType_t prvGMACWaitLS( BaseType_t xEMACIndex, TickType_t xMaxTime )
{
TickType_t xStartTime, xEndTime;
const TickType_t xShortDelay = pdMS_TO_TICKS( 20UL );
BaseType_t xReturn;

	xStartTime = xTaskGetTickCount();

	for( ;; )
	{
		xEndTime = xTaskGetTickCount();

		if( xEndTime - xStartTime > xMaxTime )
		{
			xReturn = pdFALSE;
			break;
		}
		ulPHYLinkStates[ xEMACIndex ] = ulReadMDIO( xEMACIndex, PHY_REG_01_BMSR );

		if( ( ulPHYLinkStates[ xEMACIndex ] & niBMSR_LINK_STATUS ) != 0 )
		{
			xReturn = pdTRUE;
			break;
		}

		vTaskDelay( xShortDelay );
	}

	return xReturn;
}
/*-----------------------------------------------------------*/

void vNetworkInterfaceAllocateRAMToBuffers( NetworkBufferDescriptor_t pxNetworkBuffers[ ipconfigNUM_NETWORK_BUFFER_DESCRIPTORS ] )
{
static uint8_t ucNetworkPackets[ ipconfigNUM_NETWORK_BUFFER_DESCRIPTORS * niBUFFER_1_PACKET_SIZE ] __attribute__ ( ( aligned( 32 ) ) );
uint8_t *ucRAMBuffer = ucNetworkPackets;
uint32_t ul;

	for( ul = 0; ul < ipconfigNUM_NETWORK_BUFFER_DESCRIPTORS; ul++ )
	{
		pxNetworkBuffers[ ul ].pucEthernetBuffer = ucRAMBuffer + ipBUFFER_PADDING;
		*( ( unsigned * ) ucRAMBuffer ) = ( unsigned ) ( &( pxNetworkBuffers[ ul ] ) );
		ucRAMBuffer += niBUFFER_1_PACKET_SIZE;
	}
}
/*-----------------------------------------------------------*/

static BaseType_t xZynqGetPhyLinkStatus( NetworkInterface_t *pxInterface )
{
BaseType_t xReturn;
BaseType_t xEMACIndex = ( BaseType_t )pxInterface->pvArgument;

	if( ( ulPHYLinkStates[ xEMACIndex ] & niBMSR_LINK_STATUS ) == 0 )
	{
		xReturn = pdFALSE;
	}
	else
	{
		xReturn = pdTRUE;
	}

	return xReturn;
}
/*-----------------------------------------------------------*/

UBaseType_t uxLastMinBufferCount = 0;
UBaseType_t uxCurrentBufferCount = 0;

/* pxZynq_FillInterfaceDescriptor() goes into the NetworkInterface.c of the Zynq driver. */

NetworkInterface_t *pxZynq_FillInterfaceDescriptor( BaseType_t xEMACIndex, NetworkInterface_t *pxInterface )
{
static char pcNames[ XPAR_XEMACPS_NUM_INSTANCES ][ 8 ];
/* This function pxZynq_FillInterfaceDescriptor() adds a network-interface.
Make sure that the object pointed to by 'pxInterface'
is declared static or global, and that it will remain to exist. */

	snprintf( pcNames[ xEMACIndex ], sizeof( pcNames[ xEMACIndex ] ), "eth%ld", xEMACIndex );

	memset( pxInterface, '\0', sizeof( *pxInterface ) );
	pxInterface->pcName				= pcNames[ xEMACIndex ];	/* Just for logging, debugging. */
	pxInterface->pvArgument			= (void*)xEMACIndex;		/* Has only meaning for the driver functions. */
	pxInterface->pfInitialise		= xZynqNetworkInterfaceInitialise;
	pxInterface->pfOutput			= xZynqNetworkInterfaceOutput;
	pxInterface->pfGetPhyLinkStatus = xZynqGetPhyLinkStatus;

	FreeRTOS_AddNetworkInterface( pxInterface );

	return pxInterface;
}
/*-----------------------------------------------------------*/

static void prvEMACHandlerTask( void *pvParameters )
{
TimeOut_t xPhyTime;
TickType_t xPhyRemTime;
UBaseType_t uxCurrentCount;
BaseType_t xResult = 0;
uint32_t xStatus;
const TickType_t ulMaxBlockTime = pdMS_TO_TICKS( 100UL );
BaseType_t xEMACIndex = ( BaseType_t )pvParameters;

	/* Remove compiler warnings about unused parameters. */
	( void ) pvParameters;

	/* A possibility to set some additional task properties like calling
	portTASK_USES_FLOATING_POINT() */
	iptraceEMAC_TASK_STARTING();

	vTaskSetTimeOutState( &xPhyTime );
	xPhyRemTime = pdMS_TO_TICKS( PHY_LS_LOW_CHECK_TIME_MS );
	FreeRTOS_printf( ( "prvEMACHandlerTask[ %ld ] started running\n", xEMACIndex ) );

	for( ;; )
	{
		uxCurrentBufferCount = uxGetMinimumFreeNetworkBuffers();
		if( uxLastMinBufferCount != uxCurrentBufferCount )
		{
			/* The logging produced below may be helpful
			while tuning +TCP: see how many buffers are in use. */
			uxLastMinBufferCount = uxCurrentBufferCount;
			FreeRTOS_printf( ( "Network buffers: %lu lowest %lu\n",
				uxGetNumberOfFreeNetworkBuffers(), uxCurrentBufferCount ) );
		}

		#if( ipconfigCHECK_IP_QUEUE_SPACE != 0 )
		{
		static UBaseType_t uxLastMinQueueSpace = 0;

			uxCurrentCount = uxGetMinimumIPQueueSpace();
			if( uxLastMinQueueSpace != uxCurrentCount )
			{
				/* The logging produced below may be helpful
				while tuning +TCP: see how many buffers are in use. */
				uxLastMinQueueSpace = uxCurrentCount;
				FreeRTOS_printf( ( "Queue space: lowest %lu\n", uxCurrentCount ) );
			}
		}
		#endif /* ipconfigCHECK_IP_QUEUE_SPACE */

		if( ( xEMACpsifs[ xEMACIndex ].isr_events & EMAC_IF_ALL_EVENT ) == 0 )
		{
			/* No events to process now, wait for the next. */
			ulTaskNotifyTake( pdFALSE, ulMaxBlockTime );
		}

		if( ( xEMACpsifs[ xEMACIndex ].isr_events & EMAC_IF_RX_EVENT ) != 0 )
		{
			xEMACpsifs[ xEMACIndex ].isr_events &= ~EMAC_IF_RX_EVENT;
			xResult = emacps_check_rx( &xEMACpsifs[ xEMACIndex ], pxMyInterfaces[ xEMACIndex ] );
		}

		if( ( xEMACpsifs[ xEMACIndex ].isr_events & EMAC_IF_TX_EVENT ) != 0 )
		{
			xEMACpsifs[ xEMACIndex ].isr_events &= ~EMAC_IF_TX_EVENT;
			emacps_check_tx( &xEMACpsifs[ xEMACIndex ] );
		}

		if( ( xEMACpsifs[ xEMACIndex ].isr_events & EMAC_IF_ERR_EVENT ) != 0 )
		{
			xEMACpsifs[ xEMACIndex ].isr_events &= ~EMAC_IF_ERR_EVENT;
			emacps_check_errors( &xEMACpsifs[ xEMACIndex ] );
		}

		if( xResult > 0 )
		{
			/* A packet was received. No need to check for the PHY status now,
			but set a timer to check it later on. */
			vTaskSetTimeOutState( &xPhyTime );
			xPhyRemTime = pdMS_TO_TICKS( PHY_LS_HIGH_CHECK_TIME_MS );
			xResult = 0;
			ulPHYLinkStates[ xEMACIndex ] |= niBMSR_LINK_STATUS;
		}
		else if( xTaskCheckForTimeOut( &xPhyTime, &xPhyRemTime ) != pdFALSE )
		{
			xStatus = ulReadMDIO( xEMACIndex, PHY_REG_01_BMSR );

			if( ( ulPHYLinkStates[ xEMACIndex ] & niBMSR_LINK_STATUS ) != ( xStatus & niBMSR_LINK_STATUS ) )
			{
				ulPHYLinkStates[ xEMACIndex ] = xStatus;
				FreeRTOS_printf( ( "prvEMACHandlerTask: PHY LS now %d\n", ( ulPHYLinkStates[ xEMACIndex ] & niBMSR_LINK_STATUS ) != 0 ) );
			}

			vTaskSetTimeOutState( &xPhyTime );
			if( ( ulPHYLinkStates[ xEMACIndex ] & niBMSR_LINK_STATUS ) != 0 )
			{
				xPhyRemTime = pdMS_TO_TICKS( PHY_LS_HIGH_CHECK_TIME_MS );
			}
			else
			{
				xPhyRemTime = pdMS_TO_TICKS( PHY_LS_LOW_CHECK_TIME_MS );
			}
		}
	}
}
/*-----------------------------------------------------------*/
