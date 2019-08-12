/*
 * FreeRTOS+TCP Labs Build 200417 (C) 2016 Real Time Engineers ltd.
 * Authors include Hein Tibosch and Richard Barry
 *
 *******************************************************************************
 ***** NOTE ******* NOTE ******* NOTE ******* NOTE ******* NOTE ******* NOTE ***
 ***                                                                         ***
 ***                                                                         ***
 ***   FREERTOS+TCP IS STILL IN THE LAB (mainly because the FTP and HTTP     ***
 ***   demos have a dependency on FreeRTOS+FAT, which is only in the Labs    ***
 ***   download):                                                            ***
 ***                                                                         ***
 ***   FreeRTOS+TCP is functional and has been used in commercial products   ***
 ***   for some time.  Be aware however that we are still refining its       ***
 ***   design, the source code does not yet quite conform to the strict      ***
 ***   coding and style standards mandated by Real Time Engineers ltd., and  ***
 ***   the documentation and testing is not necessarily complete.            ***
 ***                                                                         ***
 ***   PLEASE REPORT EXPERIENCES USING THE SUPPORT RESOURCES FOUND ON THE    ***
 ***   URL: http://www.FreeRTOS.org/contact  Active early adopters may, at   ***
 ***   the sole discretion of Real Time Engineers Ltd., be offered versions  ***
 ***   under a license other than that described below.                      ***
 ***                                                                         ***
 ***                                                                         ***
 ***** NOTE ******* NOTE ******* NOTE ******* NOTE ******* NOTE ******* NOTE ***
 *******************************************************************************
 *
 * FreeRTOS+TCP can be used under two different free open source licenses.  The
 * license that applies is dependent on the processor on which FreeRTOS+TCP is
 * executed, as follows:
 *
 * If FreeRTOS+TCP is executed on one of the processors listed under the Special
 * License Arrangements heading of the FreeRTOS+TCP license information web
 * page, then it can be used under the terms of the FreeRTOS Open Source
 * License.  If FreeRTOS+TCP is used on any other processor, then it can be used
 * under the terms of the GNU General Public License V2.  Links to the relevant
 * licenses follow:
 *
 * The FreeRTOS+TCP License Information Page: http://www.FreeRTOS.org/tcp_license
 * The FreeRTOS Open Source License: http://www.FreeRTOS.org/license
 * The GNU General Public License Version 2: http://www.FreeRTOS.org/gpl-2.0.txt
 *
 * FreeRTOS+TCP is distributed in the hope that it will be useful.  You cannot
 * use FreeRTOS+TCP unless you agree that you use the software 'as is'.
 * FreeRTOS+TCP is provided WITHOUT ANY WARRANTY; without even the implied
 * warranties of NON-INFRINGEMENT, MERCHANTABILITY or FITNESS FOR A PARTICULAR
 * PURPOSE. Real Time Engineers Ltd. disclaims all conditions and terms, be they
 * implied, expressed, or statutory.
 *
 * 1 tab == 4 spaces!
 *
 * http://www.FreeRTOS.org
 * http://www.FreeRTOS.org/plus
 * http://www.FreeRTOS.org/labs
 *
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
#include "NetworkBufferManagement.h"
#include "NetworkInterface.h"
#include "FreeRTOS_DHCP.h"
#include "FreeRTOS_DNS.h"
#include "FreeRTOS_Routing.h"


#include "hr_gettime.h"

/* Interrupt events to process.  Currently only the Rx event is processed
although code for other events is included to allow for possible future
expansion. */
#define EMAC_IF_RX_EVENT        1UL
#define EMAC_IF_TX_EVENT        2UL
#define EMAC_IF_ERR_EVENT       4UL
#define EMAC_IF_ALL_EVENT       ( EMAC_IF_RX_EVENT | EMAC_IF_TX_EVENT | EMAC_IF_ERR_EVENT )

#include "socal/hps.h"
#include "socal/alt_rstmgr.h"
#include "alt_cache.h"

#include "cyclone_dma.h"
#include "cyclone_emac.h"
#include "cyclone_phy.h"

#include "socal/alt_emac.h"

#include "eventLogging.h"

#define NETWORK_BUFFERS_CACHED	1
#define NETWORK_BUFFER_HEADER_SIZE	( ipconfigPACKET_FILLER_SIZE + 8 )

/* The bits in the two byte IP header field that make up the fragment offset value. */
#define ipFRAGMENT_OFFSET_BIT_MASK				( FreeRTOS_ntohs( ( ( uint16_t ) 0x0fff ) ) )

#define niBMSR_LINK_STATUS         0x0004ul

#ifndef	PHY_LS_HIGH_CHECK_TIME_MS
	/* Check if the LinkSStatus in the PHY is still high after 15 seconds of not
	receiving packets. */
	#define PHY_LS_HIGH_CHECK_TIME_MS	15000
#endif

#ifndef	PHY_LS_LOW_CHECK_TIME_MS
	/* Check if the LinkSStatus in the PHY is still low every second. */
	#define PHY_LS_LOW_CHECK_TIME_MS	1000
#endif

/* Naming and numbering of PHY registers. */
#define PHY_REG_01_BMSR			0x01	/* Basic mode status register */

#ifndef iptraceEMAC_TASK_STARTING
	#define iptraceEMAC_TASK_STARTING()	do { } while( 0 )
#endif

/* Default the size of the stack used by the EMAC deferred handler task to twice
the size of the stack used by the idle task - but allow this to be overridden in
FreeRTOSConfig.h as configMINIMAL_STACK_SIZE is a user definable constant. */
#ifndef configEMAC_TASK_STACK_SIZE
	#define configEMAC_TASK_STACK_SIZE ( 640 )
#endif

#define __DSB()		__asm volatile ( "DSB" )

#define ISR_MASK	( 0xFC01FFFF )

#define NETWORK_BUFFER_SIZE	1536

#define CYCLONE_EMAC_COUNT	4

/*-----------------------------------------------------------*/

/*
 * Look for the link to be up every few milliseconds until either xMaxTime time
 * has passed or a link is found.
 */
static BaseType_t prvGMACWaitLS( TickType_t xMaxTime );

/*
 * A deferred interrupt handler for all MAC/DMA interrupt sources.
 */
static void prvEMACHandlerTask( void *pvParameters );

/* FreeRTOS+TCP/multi :
Each network device has 3 access functions:
- initialise the device
- output a network packet
- return the PHY link-status (LS)
They can be defined as static because their are addresses will be
stored in struct NetworkInterface_t. */

static BaseType_t xCycloneNetworkInterfaceInitialise( NetworkInterface_t *pxInterface );

static BaseType_t xCycloneNetworkInterfaceOutput( NetworkInterface_t *pxInterface, NetworkBufferDescriptor_t * const pxBuffer, BaseType_t bReleaseAfterSend );

static BaseType_t xCycloneGetPhyLinkStatus( NetworkInterface_t *pxInterface );

/*-----------------------------------------------------------*/

extern int phy_detected;

/* A copy of PHY register 1: 'PHY_REG_01_BMSR' */
static uint32_t ulPHYLinkStatus = 0;

#if( ipconfigUSE_LLMNR == 1 )
	static const uint8_t xLLMNR_MACAddress[] = { 0x01, 0x00, 0x5E, 0x00, 0x00, 0xFC };
#endif

///* ucMACAddress as it appears in main.c */
//extern const uint8_t ucMACAddress[ 6 ];

/* Holds the handle of the task used as a deferred interrupt processor.  The
handle is used so direct notifications can be sent to the task for all EMAC/DMA
related interrupts. */
static TaskHandle_t xEMACTaskHandles[ CYCLONE_EMAC_COUNT ];

static volatile uint32_t ulISREvents[ CYCLONE_EMAC_COUNT ];

EMACInterface_t xEMACif;

static int iMacID = 1;

__attribute__ ( ( aligned( 64 ) ) ) __attribute__ ((section (".oc_ram"))) gmac_tx_descriptor_t txDescriptors[ GMAC_TX_BUFFERS ];
__attribute__ ( ( aligned( 64 ) ) ) __attribute__ ((section (".oc_ram"))) gmac_rx_descriptor_t rxDescriptors[ GMAC_RX_BUFFERS ];

#if( NETWORK_BUFFERS_CACHED )
	static uint8_t __attribute__ ( ( aligned( 32 ) ) )
		ucNetworkPackets[ ipconfigNUM_NETWORK_BUFFER_DESCRIPTORS * NETWORK_BUFFER_SIZE ] ;
#else
	static uint8_t __attribute__ ( ( aligned( 32 ) ) ) __attribute__ ((section (".oc_ram")))
		ucNetworkPackets[ ipconfigNUM_NETWORK_BUFFER_DESCRIPTORS * NETWORK_BUFFER_SIZE ] ;
#endif

static gmac_tx_descriptor_t *pxNextTxDesc = txDescriptors;
static gmac_tx_descriptor_t *DMATxDescToClear = txDescriptors;

static gmac_rx_descriptor_t *pxNextRxDesc = rxDescriptors;

/* xTXDescriptorSemaphore is a counting semaphore with
a maximum count of GMAC_TX_BUFFERS, which is the number of
DMA TX descriptors. */
static SemaphoreHandle_t xTXDescriptorSemaphore = NULL;

extern int tcp_min_rx_buflen;

static NetworkInterface_t *pxMyInterfaces[ CYCLONE_EMAC_COUNT ];

/*
 * Check if a given packet should be accepted.
 */
static BaseType_t xMayAcceptPacket( uint8_t *pcBuffer, NetworkEndPoint_t **ppxEndPoint );

/*!
 * The callback to use when an interrupt needs to be serviced.
 *
 * \param       icciar          The Interrupt Controller CPU Interrupt
 *                              Acknowledgement Register value (ICCIAR) value
 *                              corresponding to the current interrupt.
 *
 * \param       context         The user provided context.
 */
void vEMACInterrupthandler( uint32_t ulICCIAR, void * pvContext );
/*-----------------------------------------------------------*/

static void vClearTXBuffers()
{
gmac_tx_descriptor_t  *pxLastDescriptor = pxNextTxDesc;
size_t uxCount = ( ( UBaseType_t ) GMAC_TX_BUFFERS ) - uxSemaphoreGetCount( xTXDescriptorSemaphore );
#if( ipconfigZERO_COPY_TX_DRIVER != 0 )
	NetworkBufferDescriptor_t *pxNetworkBuffer;
	uint8_t *ucPayLoad;
#endif

	/* This function is called after a TX-completion interrupt.
	It will release each Network Buffer used in xCycloneNetworkInterfaceOutput().
	'uxCount' represents the number of descriptors given to DMA for transmission.
	After sending a packet, the DMA will clear the own bit. */
	while( ( uxCount > 0 ) && ( DMATxDescToClear->own == 0 ) )
	{
		if( ( DMATxDescToClear == pxLastDescriptor ) && ( uxCount != GMAC_TX_BUFFERS ) )
		{
			break;
		}
		#if( ipconfigZERO_COPY_TX_DRIVER != 0 )
		{
			ucPayLoad = ( uint8_t * )DMATxDescToClear->buf1_address;

			if( ucPayLoad != NULL )
			{
				pxNetworkBuffer = pxPacketBuffer_to_NetworkBuffer( ucPayLoad );
				if( pxNetworkBuffer != NULL )
				{
					vReleaseNetworkBufferAndDescriptor( pxNetworkBuffer ) ;
				}
				DMATxDescToClear->buf1_address = ( uint32_t )0u;
eventLogAdd("TX Clear %d", (int)(DMATxDescToClear - txDescriptors));			}
		}
		#endif /* ipconfigZERO_COPY_TX_DRIVER */

		DMATxDescToClear = ( gmac_tx_descriptor_t * )( DMATxDescToClear->next_descriptor );

		uxCount--;
		/* Tell the counting semaphore that one more TX descriptor is available. */
		xSemaphoreGive( xTXDescriptorSemaphore );
	}
}
/*-----------------------------------------------------------*/

#define RESET_MANAGER_ADDR		0xFFD05000
volatile ALT_RSTMGR_t *pxResetManager;


SYSMGR_EMACGRP_t * sysmgr_emacgrp;
volatile DMA_STATUS_REG_t *dma_status_reg;
volatile DMA_STATUS_REG_t *dma_enable_reg;

BaseType_t xCycloneNetworkInterfaceInitialise( NetworkInterface_t *pxInterface )
{
const TickType_t xWaitLinkDelay = pdMS_TO_TICKS( 7000UL ), xWaitRelinkDelay = pdMS_TO_TICKS( 1000UL );
BaseType_t xLinkStatus;
BaseType_t xEMACIndex = ( BaseType_t )pxInterface->pvArgument;
int iMACAddressIndex = 0;
NetworkEndPoint_t *pxEndPoint;

	pxMyInterfaces[ xEMACIndex ] = pxInterface;

	/* Guard against the init function being called more than once. */
	if( xEMACTaskHandles[ xEMACIndex ] == NULL )
	{
	char pcTaskName[ 16 ];
		#warning Debugging : just checking the reset flags of all modules, need emac1 and dma
		pxResetManager = ( volatile ALT_RSTMGR_t * ) RESET_MANAGER_ADDR;
		pxResetManager->permodrst.emac1 = 1;

		sysmgr_emacgrp = ( SYSMGR_EMACGRP_t * ) ( ALT_SYSMGR_OFST + 0x60 );
		sysmgr_emacgrp->physel_0 = SYSMGR_EMACGRP_CTRL_PHYSEL_ENUM_RGMII;
		sysmgr_emacgrp->physel_1 = SYSMGR_EMACGRP_CTRL_PHYSEL_ENUM_RGMII;
		dma_status_reg = ( volatile DMA_STATUS_REG_t * ) ( ucFirstIOAddres( iMacID ) + DMA_STATUS );
		dma_enable_reg = ( volatile DMA_STATUS_REG_t * ) ( ucFirstIOAddres( iMacID ) + DMA_INTR_ENA );

		vTaskDelay( 10ul );
		pxResetManager->permodrst.emac1 = 0;
		vTaskDelay( 10ul );

		if( xTXDescriptorSemaphore == NULL )
		{
			xTXDescriptorSemaphore = xSemaphoreCreateCounting( ( UBaseType_t ) GMAC_TX_BUFFERS, ( UBaseType_t ) GMAC_TX_BUFFERS );
			configASSERT( xTXDescriptorSemaphore );
		}

		gmac_init( iMacID );

		cyclone_phy_init( iMacID, -1 );

		/* Write the main MAC address at position 0 */
		for( pxEndPoint = FreeRTOS_FirstEndPoint( pxInterface );
			 pxEndPoint != NULL;
			 pxEndPoint = FreeRTOS_NextEndPoint( pxInterface, pxEndPoint ) )
		{
			gmac_set_MAC_address( iMacID, pxEndPoint->xMACAddress.ucBytes, iMACAddressIndex );
			iMACAddressIndex++;
		}
		#if( ipconfigUSE_LLMNR == 1 )
		{
			/* Write the LLMNR MAC address at position 1 */
			gmac_set_MAC_address( iMacID, xLLMNR_MACAddress, iMACAddressIndex );
		}
		#endif

		gmac_dma_stop_tx( iMacID, 0 );
		gmac_dma_stop_rx( iMacID, 0 );

		gmac_tx_descriptor_init( iMacID, txDescriptors, ( uint8_t *) NULL, GMAC_TX_BUFFERS );
		gmac_rx_descriptor_init( iMacID, rxDescriptors, ( uint8_t *) NULL, GMAC_RX_BUFFERS );


		/* Set the TxDesc pointer with the first one of the txDescriptors list */
		gmac_set_tx_table( iMacID, txDescriptors );

		/* Set the RxDesc pointer with the first one of the rxDescriptors list */
		gmac_set_rx_table( iMacID, rxDescriptors );

		gmac_dma_start_tx( iMacID, 0 );
		gmac_dma_start_rx( iMacID, 0 );
alt_cache_l2_sync();
		gmac_enable_transmission( iMacID, pdTRUE );

		gmac_dma_reception_poll( iMacID );

		prvGMACWaitLS( xWaitLinkDelay );

//#define EMAC_INT_PRIOITY		( ( 15u ) << portPRIORITY_SHIFT )
//#define EMAC_INT_PRIOITY		( ( 2u ) << portPRIORITY_SHIFT )
#define EMAC_INT_PRIOITY		( ( configMAX_API_CALL_INTERRUPT_PRIORITY + 1 ) << portPRIORITY_SHIFT )

		vRegisterIRQHandler( ALT_INT_INTERRUPT_EMAC1_IRQ, vEMACInterrupthandler, ( void *)&xEMACif );
		alt_int_dist_priority_set( ALT_INT_INTERRUPT_EMAC1_IRQ, EMAC_INT_PRIOITY );
		alt_int_dist_enable( ALT_INT_INTERRUPT_EMAC1_IRQ );
		alt_int_dist_trigger_set( ALT_INT_INTERRUPT_EMAC1_IRQ, ALT_INT_TRIGGER_AUTODETECT );
		alt_int_dist_target_set( ALT_INT_INTERRUPT_EMAC1_IRQ, (alt_int_cpu_target_t)0x01);

		/* Clear EMAC interrupt status. */
		gmac_clear_emac_interrupt_status( iMacID, 0x0001fffful );
		/* Clear DMA interrupt status. */
		gmac_clear_dma_interrupt_status( iMacID, ISR_MASK );
		/* Enable all interrupts. */

		gmac_set_emac_interrupt_disable( iMacID, GMAC_INT_DISABLE_TIMESTAMP | GMAC_INT_DISABLE_LPI );
		//#define DMA_INTR_NORMAL	( DMA_INTR_ENA_NIE | DMA_INTR_ENA_RIE | DMA_INTR_ENA_TIE )
		gmac_set_dma_interrupt_enable( iMacID, DMA_INTR_NORMAL );


//		{
//			/* HT: I don't think this really helps against getting
//			DMA_STATUS_GMI interrupts. */
//			uint8_t *ioaddr = ucFirstIOAddres( iMacID );
//			writel( ( uint32_t ) ~0ul, ioaddr + GMAC_MMC_RX_INTR_MASK );
//			writel( ( uint32_t ) ~0ul, ioaddr + GMAC_MMC_TX_INTR_MASK );
//		}

		gmac_dma_transmit_poll( iMacID );
		#if( NETWORK_BUFFERS_CACHED != 0 )
		{
			/* Purge: A term used in this API as a short form for clean and invalidate.
			 * This operation cleans and invalidates a cache line in that order, as a
			 * single command to the cache controller.
			 */
			alt_cache_system_purge( ucNetworkPackets, sizeof( ucNetworkPackets ) );
		}
		#endif
		tcp_min_rx_buflen = 8;

		/* The deferred interrupt handler task is created at the highest
		possible priority to ensure the interrupt handler can return directly
		to it.  The task's handle is stored in xEMACTaskHandle so interrupts can
		notify the task when there is something to process. */
		sprintf( pcTaskName, "GMAC%ld", xEMACIndex );
		xTaskCreate( prvEMACHandlerTask, pcTaskName, configEMAC_TASK_STACK_SIZE, ( void * )xEMACIndex, configMAX_PRIORITIES - 1, &xEMACTaskHandles[ xEMACIndex ] );
	}
	else
	{
		/* Initialisation was already performed, just wait for the link. */
		prvGMACWaitLS( xWaitRelinkDelay );
	}

	/* Only return pdTRUE when the Link Status of the PHY is high, otherwise the
	DHCP process and all other communication will fail. */
	xLinkStatus = xCycloneGetPhyLinkStatus( pxInterface );

	return ( xLinkStatus != pdFALSE );
}
/*-----------------------------------------------------------*/

static volatile uint32_t ulLastDMAStatus, ulLastEMACStatus;
/* Variable must be set by main.c as soon as +TCP is up. */
BaseType_t xPlusTCPStarted;

void vEMACInterrupthandler( uint32_t ulICCIAR, void * pvContext )
{
uint32_t ulDMAStatus;
DMA_STATUS_REG_t reg;
BaseType_t xHigherPriorityTaskWoken = 0ul;
uint32_t ulEMACStatus;
int iInterruptMacID = 0;
uint8_t *ioaddr;

	switch( ulICCIAR & 0x3FFUL )
	{
	case ALT_INT_INTERRUPT_EMAC0_IRQ: iInterruptMacID = 0; break;
	case ALT_INT_INTERRUPT_EMAC1_IRQ: iInterruptMacID = 1; break;
	return;
	}
	ioaddr = ucFirstIOAddres( iInterruptMacID );

	/* Get DMA interrupt status and clear all bits. */
	ulEMACStatus = gmac_get_emac_interrupt_status( iInterruptMacID, pdFALSE );
	ulDMAStatus  = gmac_get_dma_interrupt_status( iInterruptMacID, pdFALSE );
ulLastEMACStatus = ulEMACStatus;
ulLastDMAStatus = ulDMAStatus;
/*
#define DMA_STATUS_GLPII        0x40000000	// GMAC LPI interrupt
#define DMA_STATUS_GPI          0x10000000	// PMT interrupt
#define DMA_STATUS_GMI          0x08000000	// MMC interrupt
#define DMA_STATUS_GLI          0x04000000	// GMAC Line interface int

#define DMA_STATUS_NIS          0x00010000	// Normal Interrupt Summary
#define DMA_STATUS_AIS          0x00008000	// Abnormal Interrupt Summary
#define DMA_STATUS_ERI          0x00004000	// Early Receive Interrupt
#define DMA_STATUS_FBI          0x00002000	// Fatal Bus Error Interrupt
#define DMA_STATUS_ETI          0x00000400	// Early Transmit Interrupt
#define DMA_STATUS_RWT          0x00000200	// Receive Watchdog Timeout
#define DMA_STATUS_RPS          0x00000100	// Receive Process Stopped
#define DMA_STATUS_RU           0x00000080	// Receive Buffer Unavailable
#define DMA_STATUS_RI           0x00000040	// Receive Interrupt
#define DMA_STATUS_UNF          0x00000020	// Transmit Underflow
#define DMA_STATUS_OVF          0x00000010	// Receive Overflow
#define DMA_STATUS_TJT          0x00000008	// Transmit Jabber Timeout
#define DMA_STATUS_TU           0x00000004	// Transmit Buffer Unavailable
#define DMA_STATUS_TPS          0x00000002	// Transmit Process Stopped
#define DMA_STATUS_TI           0x00000001	// Transmit Interrupt
#define DMA_CONTROL_FTF         0x00100000	// Flush transmit FIFO

                                0x08000084
								0x08680084
										 Transmit Buffer Unavailable ( DMA_STATUS_TU )
                                        Receive Buffer Unavailable ( DMA_STATUS_RU )
								   MMC interrupt ( DMA_STATUS_GMI )
*/
#define RX_MASK		( DMA_STATUS_RI | DMA_STATUS_OVF | DMA_STATUS_ERI )
#define TX_MASK		( DMA_STATUS_TI | DMA_STATUS_TPS | DMA_INTR_ENA_ETE )
	reg.ulValue = ulDMAStatus;
	ulDMAStatus &= ISR_MASK;

	if( ( ulDMAStatus & DMA_STATUS_GMI ) != 0 )
	{
	uint32_t offset;
		if (readl(ioaddr + GMAC_MMC_RX_INTR)) {}
		if (readl(ioaddr + GMAC_MMC_TX_INTR)) {}
		if (readl(ioaddr + GMAC_MMC_RX_CSUM_OFFLOAD)) {}
		/* Read all counters to clear interrupt bits. */
		for (offset = 0x114; offset <= 0x1E4; ) {
			if (readl(ioaddr + offset)) {}
			offset += 4;
		}
		for (offset = 0x210; offset <= 0x284; ) {
			if (readl(ioaddr + offset)) {}
			offset += 4;
		}
		gmac_clear_dma_interrupt_status( iInterruptMacID, DMA_STATUS_GMI );
		gmac_clear_emac_interrupt_status( iInterruptMacID, 0x000000F0 );
	}
//	if( ( ( ulDMAStatus & RX_MASK ) == 0 ) && ( pxNextRxDesc != NULL ) && ( pxNextRxDesc->own == 0 ) )
//	{
//		eventLogAdd("RX_EVENT own=0" );
//		ulDMAStatus |= DMA_STATUS_RI;
//	}
	if( ( ulDMAStatus & RX_MASK ) != 0 )
	{
		eventLogAdd("RX_EVENT%s%s %04lX %X %X %X",
			( ulDMAStatus & DMA_STATUS_RI  ) ? " RI" : "",
			( ulDMAStatus & DMA_STATUS_OVF ) ? " OVF" : "",
			ulDMAStatus,
			reg.rs_recv_state,			/* 17 */
			reg.ts_xmit_state,			/* 20 */
			reg.eb_bus_error);			/* 23 */

		ulISREvents[ iInterruptMacID ] |= EMAC_IF_RX_EVENT;
	}
	if( ( ulDMAStatus & TX_MASK ) != 0 )
	{
		eventLogAdd("TX_EVENT%s%s%s %04lX %X %X %X",
			( ulDMAStatus & DMA_STATUS_TI  ) ? " TI" : "",
			( ulDMAStatus & DMA_STATUS_TPS ) ? " TPS" : "",
			( ulDMAStatus & DMA_INTR_ENA_ETE ) ? " ETE" : "",
			ulDMAStatus,
			reg.rs_recv_state,			/* 17 */
			reg.ts_xmit_state,			/* 20 */
			reg.eb_bus_error);			/* 23 */
		ulISREvents[ iInterruptMacID ] |= EMAC_IF_TX_EVENT;
	}

	if( ( ulISREvents[ iInterruptMacID ] != 0 ) && ( xEMACTaskHandles[ iInterruptMacID ] != NULL ) )
	{
		vTaskNotifyGiveFromISR( xEMACTaskHandles[ iInterruptMacID ], &xHigherPriorityTaskWoken );
	}
	if( ulDMAStatus & ( TX_MASK | RX_MASK | DMA_STATUS_NIS ) )
	{
	uint32_t ulmask;
		ulmask = gmac_clear_dma_interrupt_status( iInterruptMacID, ulDMAStatus & ( TX_MASK | RX_MASK | DMA_STATUS_NIS ) ) & ( TX_MASK | RX_MASK | DMA_STATUS_NIS );
		if( ulmask != 0ul)
		{
			ulmask = gmac_clear_dma_interrupt_status( iInterruptMacID, ulmask & ( TX_MASK | RX_MASK | DMA_STATUS_NIS ) ) & ( TX_MASK | RX_MASK | DMA_STATUS_NIS );
			if( ulmask != 0ul)
			{
				ulmask = gmac_clear_dma_interrupt_status( iInterruptMacID, ulmask & ( TX_MASK | RX_MASK | DMA_STATUS_NIS ) ) & ( TX_MASK | RX_MASK | DMA_STATUS_NIS );
			}
		}
	}

	portYIELD_FROM_ISR( xHigherPriorityTaskWoken );
}
/*-----------------------------------------------------------*/

static BaseType_t xCycloneNetworkInterfaceOutput( NetworkInterface_t *pxInterface, NetworkBufferDescriptor_t * const pxDescriptor, BaseType_t bReleaseAfterSend )
{
BaseType_t xReturn = pdFAIL;
uint32_t ulTransmitSize = 0;
gmac_tx_descriptor_t *pxDmaTxDesc;
/* Do not wait too long for a free TX DMA buffer. */
const TickType_t xBlockTimeTicks = pdMS_TO_TICKS( 200u );
BaseType_t xEMACIndex = ( BaseType_t ) pxInterface->pvArgument;

	#if( ipconfigDRIVER_INCLUDED_TX_IP_CHECKSUM != 0 )
	{
	ProtocolPacket_t *pxPacket;

		#if( ipconfigZERO_COPY_RX_DRIVER != 0 )
		{
			configASSERT( bReleaseAfterSend != 0 );
		}
		#endif /* ipconfigZERO_COPY_RX_DRIVER */

		/* If the peripheral must calculate the checksum, it wants
		the protocol checksum to have a value of zero. */
		pxPacket = ( ProtocolPacket_t * ) ( pxDescriptor->pucEthernetBuffer );

		if( pxPacket->xICMPPacket.xIPHeader.ucProtocol == ipPROTOCOL_ICMP )
		{
			pxPacket->xICMPPacket.xICMPHeader.usChecksum = ( uint16_t )0u;
		}
	}
	#endif

	/* Open a do {} while ( 0 ) loop to be able to call break. */
	do
	{
		if( xCycloneGetPhyLinkStatus( pxInterface ) != 0 )
		{
			if( uxQueueMessagesWaiting( xTXDescriptorSemaphore ) == 0 )
			{
				ulISREvents[ xEMACIndex ] |= EMAC_IF_TX_EVENT;
				xTaskNotifyGive( xEMACTaskHandles[ xEMACIndex ] );
			}
			if( xSemaphoreTake( xTXDescriptorSemaphore, xBlockTimeTicks ) != pdPASS )
			{
				/* Time-out waiting for a free TX descriptor. */
				break;
			}

			/* This function does the actual transmission of the packet. The packet is
			contained in 'pxDescriptor' that is passed to the function. */
			pxDmaTxDesc = pxNextTxDesc;

			/* Is this buffer available? */
			configASSERT ( pxDmaTxDesc->own == 0 );

			{
				/* Is this buffer available? */
				/* Get bytes in current buffer. */
				ulTransmitSize = pxDescriptor->xDataLength;

				if( ulTransmitSize > ETH_TX_BUF_SIZE )
				{
					ulTransmitSize = ETH_TX_BUF_SIZE;
				}

				#if( ipconfigZERO_COPY_TX_DRIVER == 0 )
				{
					/* Copy the bytes. */
					memcpy( ( void * ) pxDmaTxDesc->buf1_address, pxDescriptor->pucEthernetBuffer, ulTransmitSize );
				}
				#else
				{
					/* Move the buffer. */
					pxDmaTxDesc->buf1_address = ( uint32_t )pxDescriptor->pucEthernetBuffer;
					/* The Network Buffer has been passed to DMA, no need to release it. */
					bReleaseAfterSend = pdFALSE;
				}
				#endif /* ipconfigZERO_COPY_TX_DRIVER */

				/* Ask to set the IPv4 checksum.
				Also need an Interrupt on Completion so that 'vClearTXBuffers()' will be called.. */
				pxDmaTxDesc->int_on_completion = 1;
				pxDmaTxDesc->checksum_mode = 0x03;

				/* Prepare transmit descriptors to give to DMA. */

				/* Set LAST and FIRST segment */
				pxDmaTxDesc->first_segment = 1;
				pxDmaTxDesc->last_segment = 1;

				/* Set frame size */
				pxDmaTxDesc->buf1_byte_count = ulTransmitSize;
				#if( NETWORK_BUFFERS_CACHED	!= 0 )
				{
				BaseType_t xlength = ALT_CACHE_LINE_SIZE * ( ( ulTransmitSize + NETWORK_BUFFER_HEADER_SIZE + ALT_CACHE_LINE_SIZE - 1 ) / ALT_CACHE_LINE_SIZE );
//					alt_cache_system_clean( pxDescriptor->pucEthernetBuffer - NETWORK_BUFFER_HEADER_SIZE, xlength );
					alt_cache_system_purge( pxDescriptor->pucEthernetBuffer - NETWORK_BUFFER_HEADER_SIZE, xlength );
				}
				#endif

				/* Set Own bit of the Tx descriptor Status: gives the buffer back to ETHERNET DMA */
				pxDmaTxDesc->own = 1;
eventLogAdd("TX Send  %d (%lu)", (int)(pxDmaTxDesc - txDescriptors), ulTransmitSize);
				/* Point to next descriptor */
				pxNextTxDesc = ( gmac_tx_descriptor_t * ) ( pxNextTxDesc->next_descriptor );
				/* Ensure completion of memory access */
				__DSB();
//alt_cache_l2_sync();

//				gmac_dma_start_tx( iMacID, 0 );
				if( ( gmac_reg_read( iMacID, DMA_STATUS) & DMA_STATUS_TU ) != 0ul )
				{
					/* Transmit Buffer Unavailable, send a poll command to resume. */
					do
					{
						gmac_reg_write( iMacID, DMA_STATUS, DMA_STATUS_TU );
					}
					while( ( gmac_reg_read( iMacID, DMA_STATUS) & DMA_STATUS_TU ) != 0ul );

					/* Resume DMA transmission*/
					gmac_dma_transmit_poll( iMacID );
				}

				iptraceNETWORK_INTERFACE_TRANSMIT();
				xReturn = pdPASS;
			}
		}
		else
		{
			/* The PHY has no Link Status, packet shall be dropped. */
		}
	} while( 0 );
	/* The buffer has been sent so can be released. */
	if( bReleaseAfterSend != pdFALSE )
	{
		vReleaseNetworkBufferAndDescriptor( pxDescriptor );
	}

	return xReturn;
}
/*-----------------------------------------------------------*/

static inline uint32_t ulReadMDIO( uint32_t ulRegister )
{
uint32_t ulValue = gmac_mdio_read( iMacID, ulUsePHYAddress, ulRegister);

	return ulValue;
}
/*-----------------------------------------------------------*/

static inline void ulWriteMDIO( uint32_t ulRegister, uint32_t ulValue )
{
	gmac_mdio_write( iMacID, ulUsePHYAddress, ulRegister, ( uint16_t ) ulValue );
}
/*-----------------------------------------------------------*/

static BaseType_t prvGMACWaitLS( TickType_t xMaxTime )
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
		ulPHYLinkStatus = ulReadMDIO( PHY_REG_01_BMSR );

		if( ( ulPHYLinkStatus & niBMSR_LINK_STATUS ) != 0 )
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
uint8_t *ucRAMBuffer = ucNetworkPackets;
uint32_t ul;

	for( ul = 0; ul < ipconfigNUM_NETWORK_BUFFER_DESCRIPTORS; ul++ )
	{
		pxNetworkBuffers[ ul ].pucEthernetBuffer = ucRAMBuffer + ipBUFFER_PADDING;
		*( ( unsigned * ) ucRAMBuffer ) = ( unsigned ) ( &( pxNetworkBuffers[ ul ] ) );
		ucRAMBuffer += NETWORK_BUFFER_SIZE;
	}
}
/*-----------------------------------------------------------*/

static BaseType_t xCycloneGetPhyLinkStatus( NetworkInterface_t *pxInterface )
{
BaseType_t xReturn;

	if( ( ulPHYLinkStatus & niBMSR_LINK_STATUS ) == 0 )
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

static BaseType_t xMayAcceptPacket( uint8_t *pcBuffer, NetworkEndPoint_t **ppxEndPoint )
{
const ProtocolPacket_t *pxProtPacket = ( const ProtocolPacket_t * )pcBuffer;

	*ppxEndPoint = NULL;
	switch( pxProtPacket->xTCPPacket.xEthernetHeader.usFrameType )
	{
	case ipARP_FRAME_TYPE:
		/* Check it later. */
		return pdTRUE;
	case ipIPv4_FRAME_TYPE:
		/* Check it here. */
		break;
	default:
		/* Refuse the packet. */
		return pdFALSE;
	}

	#if( ipconfigETHERNET_DRIVER_FILTERS_PACKETS == 1 )
	{
		const IPHeader_t *pxIPHeader = &(pxProtPacket->xTCPPacket.xIPHeader);
		uint32_t ulDestinationIPAddress;
		NetworkEndPoint_t *pxEndPoint;
		BaseType_t xEndPointFound;

		/* Ensure that the incoming packet is not fragmented (only outgoing packets
		 * can be fragmented) as these are the only handled IP frames currently. */
		if( ( pxIPHeader->usFragmentOffset & ipFRAGMENT_OFFSET_BIT_MASK ) != 0U )
		{
			return pdFALSE;
		}
		/* HT: Might want to make the following configurable because
		 * most IP messages have a standard length of 20 bytes */

		/* 0x45 means: IPv4 with an IP header of 5 x 4 = 20 bytes
		 * 0x47 means: IPv4 with an IP header of 7 x 4 = 28 bytes */
		if( pxIPHeader->ucVersionHeaderLength < 0x45 || pxIPHeader->ucVersionHeaderLength > 0x4F )
		{
			return pdFALSE;
		}

		ulDestinationIPAddress = pxIPHeader->ulDestinationIPAddress;

		/* Is it a broadcast address x.x.x.255 ? */
		if( ( FreeRTOS_ntohl( ulDestinationIPAddress ) & 0xff ) == 0xff )
		{
			xEndPointFound = pdTRUE;
		}
		else
		{
			xEndPointFound = pdFALSE;
			for( pxEndPoint = FreeRTOS_FirstEndPoint( NULL );
				pxEndPoint != NULL;
				pxEndPoint = FreeRTOS_NextEndPoint( NULL, pxEndPoint ) )
			{
				if( ulDestinationIPAddress == pxEndPoint->ulIPAddress )
				{
					xEndPointFound = pdTRUE;
					*ppxEndPoint = pxEndPoint;
					break;
				}
			}
		}

		/* Is the packet for this node? */
		if( ( xEndPointFound == pdFALSE ) &&
		#if( ipconfigUSE_LLMNR == 1 )
			( ulDestinationIPAddress != ipLLMNR_IP_ADDR ) &&
		#endif
			( *ipLOCAL_IP_ADDRESS_POINTER != 0 ) ) {
			FreeRTOS_printf( ( "Drop IP %lxip\n", FreeRTOS_ntohl( ulDestinationIPAddress ) ) );
			return pdFALSE;
		}

		if( pxIPHeader->ucProtocol == ipPROTOCOL_UDP )
		{
			uint16_t port = pxProtPacket->xUDPPacket.xUDPHeader.usDestinationPort;

			if( ( xPortHasUDPSocket( port ) == pdFALSE )
			#if ipconfigUSE_LLMNR == 1
				&& ( port != FreeRTOS_ntohs( ipLLMNR_PORT ) )
			#endif
			#if ipconfigUSE_NBNS == 1
				&& ( port != FreeRTOS_ntohs( ipNBNS_PORT ) )
			#endif
			#if ipconfigUSE_DNS == 1
				&& ( pxProtPacket->xUDPPacket.xUDPHeader.usSourcePort != FreeRTOS_ntohs( ipDNS_PORT ) )
			#endif
				) {
				/* Drop this packet, not for this device. */
				return pdFALSE;
			}
		}
	}
	#endif	/* ipconfigETHERNET_DRIVER_FILTERS_PACKETS */
	return pdTRUE;
}
/*-----------------------------------------------------------*/

struct xFrameInfo
{
	gmac_rx_descriptor_t *FSRxDesc;          /*!< First Segment Rx Desc */
	gmac_rx_descriptor_t *LSRxDesc;          /*!< Last Segment Rx Desc */
	uint32_t  SegCount;                    /*!< Segment count */
	uint32_t length;                       /*!< Frame length */
	uint32_t buffer;                       /*!< Frame buffer */
};
typedef struct xFrameInfo FrameInfo_t;

static BaseType_t xGetReceivedFrame( FrameInfo_t *pxFrame );

static BaseType_t xGetReceivedFrame( FrameInfo_t *pxFrame )
{
uint32_t ulCounter = 0;
gmac_rx_descriptor_t *pxDescriptor = pxNextRxDesc;
BaseType_t xResult = -1;

	/* Scan descriptors owned by CPU */
	while( ( pxDescriptor->own == 0u ) && ( ulCounter < GMAC_RX_BUFFERS ) )
	{
		/* Just for security. */
		ulCounter++;

		if( pxDescriptor->last_descriptor == 0u )
		{
			if( pxDescriptor->first_descriptor != 0u )
			{
				/* First segment in frame, but not the last. */
				pxFrame->FSRxDesc = pxDescriptor;
				pxFrame->LSRxDesc = ( gmac_rx_descriptor_t *)NULL;
				pxFrame->SegCount = 1;
			}
			else
			{
				/* This is an intermediate segment, not first, not last. */
				/* Increment segment count. */
				pxFrame->SegCount++;
			}
			/* Point to next descriptor. */
			pxDescriptor = (gmac_rx_descriptor_t*) (pxDescriptor->next_descriptor);
			pxNextRxDesc = pxDescriptor;
		}
		/* Must be a last segment */
		else
		{
			if( pxDescriptor->first_descriptor != 0u )
			{
				pxFrame->SegCount = 0;
				/* Remember the first segment. */
				pxFrame->FSRxDesc = pxDescriptor;
			}

			/* Increment segment count */
			pxFrame->SegCount++;

			/* Remember the last segment. */
			pxFrame->LSRxDesc = pxDescriptor;

			/* Get the Frame Length of the received packet: substruct 4 bytes of the CRC */
			pxFrame->length = pxDescriptor->frame_length - 4;

			/* Get the address of the buffer start address */
			pxFrame->buffer = pxDescriptor->buf1_address;

			/* Point to next descriptor */
			pxNextRxDesc = ( gmac_rx_descriptor_t * ) pxDescriptor->next_descriptor;

			/* Return OK status: a packet was received. */
			xResult = pxFrame->length;
			break;
		}
	}

	/* Return function status */
	return xResult;
}
/*-----------------------------------------------------------*/

#if( ipconfigUSE_LINKED_RX_MESSAGES != 0 )

static NetworkBufferDescriptor_t *pxFirstPacket = NULL;
static NetworkBufferDescriptor_t *pxLastPacket = NULL;

static void vPassPackets (void)
{
//	if( pxFirstPacket == NULL ) /* has been checked */
//		return;
	IPStackEvent_t xRxEvent;

	xRxEvent.eEventType = eNetworkRxEvent;
	xRxEvent.pvData = ( void * ) pxFirstPacket;

	if( xSendEventStructToIPTask( &xRxEvent, ( portTickType ) 1000 ) != pdPASS )
	{
		/* The buffer could not be sent to the stack so
		must be released again.  This is only an interrupt
		simulator, not a real interrupt, so it is ok to use
		the task level function here. */
		do
		{
			NetworkBufferDescriptor_t *pxNext = pxFirstPacket->pxNextBuffer;
			vReleaseNetworkBufferAndDescriptor( pxFirstPacket );
			pxFirstPacket = pxNext;
		} while( pxFirstPacket != NULL );
		iptraceETHERNET_RX_EVENT_LOST();
		FreeRTOS_printf( ( "vPassPackets: Can not queue return packet!\n" ) );
	}
	pxFirstPacket = NULL;
	pxLastPacket = NULL;
}
#endif

int gmac_check_rx( NetworkInterface_t *pxInterface )
{
FrameInfo_t frameInfo;
BaseType_t xReceivedLength, xAccepted;
#if( ipconfigUSE_LINKED_RX_MESSAGES == 0 )
	xIPStackEvent_t xRxEvent = { eNetworkRxEvent, NULL };
#endif
const TickType_t xDescriptorWaitTime = pdMS_TO_TICKS( 250 );
uint8_t *pucBuffer;
gmac_rx_descriptor_t *pxRxDescriptor;
NetworkBufferDescriptor_t *pxCurNetworkBuffer;
NetworkBufferDescriptor_t *pxNewNetworkBuffer = NULL;

	xReceivedLength = xGetReceivedFrame( &frameInfo );

	if( xReceivedLength > 0 )
	{
	NetworkEndPoint_t *pxEndPoint = NULL;
		pucBuffer = ( uint8_t * )frameInfo.buffer;
		#if( NETWORK_BUFFERS_CACHED	!= 0 )
		{
			if( pucBuffer != NULL )
			{
			BaseType_t xlength = ALT_CACHE_LINE_SIZE * ( ( xReceivedLength + NETWORK_BUFFER_HEADER_SIZE + ALT_CACHE_LINE_SIZE - 1 ) / ALT_CACHE_LINE_SIZE );
			uint32_t *ptr = ( uint32_t * )( pucBuffer - NETWORK_BUFFER_HEADER_SIZE );
			uint32_t ulStore[ 2 ];

				ulStore[ 0 ] = ptr[ 0 ];
				ulStore[ 1 ] = ptr[ 1 ];
				alt_cache_system_invalidate( pucBuffer - NETWORK_BUFFER_HEADER_SIZE, xlength );
				ptr[ 0 ] = ulStore[ 0 ];
				ptr[ 1 ] = ulStore[ 1 ];
			}
		}
		#endif

		/* Update the ETHERNET DMA global Rx descriptor with next Rx descriptor */
		/* Chained Mode */    
		/* Selects the next DMA Rx descriptor list for next buffer to read */ 
		pxRxDescriptor = ( gmac_rx_descriptor_t* )frameInfo.FSRxDesc;
eventLogAdd("RX Recv  %d (%lu)", (int)(pxRxDescriptor - rxDescriptors), xReceivedLength);

		/* In order to make the code easier and faster, only packets in a single buffer
		will be accepted.  This can be done by making the buffers large enough to
		hold a complete Ethernet packet (1536 bytes).
		Therefore, two sanity checks: */
		configASSERT( xReceivedLength <= ETH_RX_BUF_SIZE );

		if( pxRxDescriptor->error_summary != 0u )
		{
			/* Not an Ethernet frame-type or a checmsum error. */
			xAccepted = pdFALSE;
		}
		else
		{
			/* See if this packet must be handled. */
			xAccepted = xMayAcceptPacket( pucBuffer, &( pxEndPoint ) );
		}

		if( xAccepted != pdFALSE )
		{
			/* The packet wil be accepted, but check first if a new Network Buffer can
			be obtained. If not, the packet will still be dropped. */
			pxNewNetworkBuffer = pxGetNetworkBufferWithDescriptor( ETH_RX_BUF_SIZE, xDescriptorWaitTime );

			if( pxNewNetworkBuffer == NULL )
			{
				/* A new descriptor can not be allocated now. This packet will be dropped. */
				xAccepted = pdFALSE;
			}
		}

		#if( ipconfigZERO_COPY_RX_DRIVER != 0 )
		{
			/* Find out which Network Buffer was originally passed to the descriptor. */
			pxCurNetworkBuffer = pxPacketBuffer_to_NetworkBuffer( pucBuffer );
			configASSERT( pxCurNetworkBuffer != NULL );
			pxCurNetworkBuffer->pxInterface = pxInterface;
			if( pxEndPoint != NULL )
			{
				pxCurNetworkBuffer->pxEndPoint = pxEndPoint;
			}
			else
			{
				pxCurNetworkBuffer->pxEndPoint = pxInterface->pxEndPoint;
			}
		}
		#else
		{
			/* In this mode, the two descriptors are the same. */
			pxCurNetworkBuffer = pxNewNetworkBuffer;
			if( pxNewNetworkBuffer != NULL )
			{
				/* The packet is acepted and a new Network Buffer was created,
				copy data to the Network Bufffer. */
				memcpy( pxNewNetworkBuffer->pucEthernetBuffer, pucBuffer, xReceivedLength );
			}
		}
		#endif

		if( xAccepted != pdFALSE )
		{
			pxCurNetworkBuffer->xDataLength = xReceivedLength;
			#if( ipconfigUSE_LINKED_RX_MESSAGES != 0 )
			{
				// This option increases performance about 7%
				pxCurNetworkBuffer->pxNextBuffer = NULL;
				if( pxFirstPacket == NULL )
				{
					// Becomes the first message
					pxFirstPacket = pxCurNetworkBuffer;
				}
				else if( pxLastPacket != NULL )
				{
					// Add to the tail
					pxLastPacket->pxNextBuffer = pxCurNetworkBuffer;
				}
				pxLastPacket = pxCurNetworkBuffer;
			}
			#else
			{
				xRxEvent.pvData = ( void * ) pxCurNetworkBuffer;

				/* Pass the data to the TCP/IP task for processing. */
				if( xSendEventStructToIPTask( &xRxEvent, xDescriptorWaitTime ) == pdFALSE )
				{
					/* Could not send the descriptor into the TCP/IP stack, it
					must be released. */
					vReleaseNetworkBufferAndDescriptor( pxCurNetworkBuffer );
					iptraceETHERNET_RX_EVENT_LOST();
				}
				else
				{
					iptraceNETWORK_INTERFACE_RECEIVE();
				}
			}
			#endif /* ipconfigUSE_LINKED_RX_MESSAGES */
		}

		/* Set Buffer1 address pointer */
		if( pxNewNetworkBuffer != NULL )
		{
			pxRxDescriptor->buf1_address = (uint32_t)pxNewNetworkBuffer->pucEthernetBuffer;
		}
		else
		{
			/* The packet was dropped and the same Network
			Buffer will be used to receive a new packet. */
		}

		pxRxDescriptor->desc0 = 0u;
		/* Set Buffer1 size and Second Address Chained bit */
		pxRxDescriptor->buf1_byte_count = ETH_RX_BUF_SIZE;
		pxRxDescriptor->second_address_chained = pdTRUE;
		pxRxDescriptor->own = pdTRUE;

		/* Ensure completion of memory access */
		__DSB();
		if( ( gmac_reg_read( iMacID, DMA_STATUS) & DMA_STATUS_RU ) != 0ul )
		{
			/* Receive Buffer Unavailable, send a poll command to resume. */
			do
			{
				gmac_reg_write( iMacID, DMA_STATUS, DMA_STATUS_RU );
			}
			while( ( gmac_reg_read( iMacID, DMA_STATUS) & DMA_STATUS_RU ) != 0ul );

			/* Resume DMA transmission*/
			gmac_dma_reception_poll( iMacID );
		}
	}

	return ( xReceivedLength > 0 );
}
/*-----------------------------------------------------------*/

NetworkInterface_t *pxCyclone_FillInterfaceDescriptor( BaseType_t xEMACIndex, NetworkInterface_t *pxInterface )
{
static char pcNames[ CYCLONE_EMAC_COUNT ][ 8 ];
/* This function pxCyclone_FillInterfaceDescriptor() adds a network-interface.
Make sure that the object pointed to by 'pxInterface'
is declared static or global, and that it will remain to exist. */

	snprintf( pcNames[ xEMACIndex ], sizeof( pcNames[ xEMACIndex ] ), "eth%ld", xEMACIndex );

	memset( pxInterface, '\0', sizeof( *pxInterface ) );
	pxInterface->pcName				= pcNames[ xEMACIndex ];	/* Just for logging, debugging. */
	pxInterface->pvArgument			= (void*)xEMACIndex;		/* Has only meaning for the driver functions. */
	pxInterface->pfInitialise		= xCycloneNetworkInterfaceInitialise;
	pxInterface->pfOutput			= xCycloneNetworkInterfaceOutput;
	pxInterface->pfGetPhyLinkStatus = xCycloneGetPhyLinkStatus;

	FreeRTOS_AddNetworkInterface( pxInterface );

	return pxInterface;
}
/*-----------------------------------------------------------*/
UBaseType_t uxCurrentIPQueueSpace;
UBaseType_t uxLastMinBufferCount = 0;
UBaseType_t uxCurrentBufferCount = 0;
UBaseType_t uxLowestSemCount = ( UBaseType_t ) GMAC_TX_BUFFERS - 1;
UBaseType_t uxCurrentSemCount = 0;

void emac_show_buffers( void );
void emac_show_buffers()
{
	FreeRTOS_printf( ( "Queue space %lu Buff space %lu (%lu) TX space %lu (%lu)\n",
		uxCurrentIPQueueSpace,
		uxGetNumberOfFreeNetworkBuffers(),
		uxLastMinBufferCount,
		uxCurrentSemCount,
		uxLowestSemCount) ) ;
}

TaskGuard_t xEmacTask;
static void prvEMACHandlerTask( void *pvParameters )
{
const TickType_t ulMaxBlockTime = pdMS_TO_TICKS( 500UL );
TimeOut_t xPhyTime;
TickType_t xPhyRemTime;
BaseType_t xEMACIndex = ( BaseType_t )pvParameters;
volatile uint32_t *pulISREvents = ulISREvents + xEMACIndex;

	vTask_init( &xEmacTask, 15 );

	/* Remove compiler warnings about unused parameters. */
	( void ) pvParameters;

	/* A possibility to set some additional task properties like calling
	portTASK_USES_FLOATING_POINT() */
	iptraceEMAC_TASK_STARTING();

	vTaskSetTimeOutState( &xPhyTime );
	xPhyRemTime = pdMS_TO_TICKS( PHY_LS_LOW_CHECK_TIME_MS );

	for( ;; )
	{
	UBaseType_t uxResult = 0ul;
		uxCurrentBufferCount = uxGetMinimumFreeNetworkBuffers();
		if( uxLastMinBufferCount != uxCurrentBufferCount )
		{
			/* The logging produced below may be helpful
			while tuning +TCP: see how many buffers are in use. */
			uxLastMinBufferCount = uxCurrentBufferCount;
			FreeRTOS_printf( ( "Network buffers: %lu lowest %lu\n",
				uxGetNumberOfFreeNetworkBuffers(), uxCurrentBufferCount ) );
		}

		if( xTXDescriptorSemaphore != NULL )
		{
			uxCurrentSemCount = uxSemaphoreGetCount( xTXDescriptorSemaphore );
			if( uxLowestSemCount > uxCurrentSemCount )
			{
				uxLowestSemCount = uxCurrentSemCount;
				FreeRTOS_printf( ( "TX DMA buffers: lowest %lu\n", uxLowestSemCount ) );
			}

		}

		#if( ipconfigCHECK_IP_QUEUE_SPACE != 0 )
		{
		static UBaseType_t uxLastMinQueueSpace = 0;

			uxCurrentIPQueueSpace = uxGetMinimumIPQueueSpace();
			if( uxLastMinQueueSpace != uxCurrentIPQueueSpace )
			{
				/* The logging produced below may be helpful
				while tuning +TCP: see how many buffers are in use. */
				uxLastMinQueueSpace = uxCurrentIPQueueSpace;
				FreeRTOS_printf( ( "Queue space: lowest %lu\n", uxCurrentIPQueueSpace ) );
			}
		}
		#endif /* ipconfigCHECK_IP_QUEUE_SPACE */

		if( ( *( pulISREvents ) & EMAC_IF_ALL_EVENT ) == 0 )
		{
		vTask_finish( &xEmacTask );
			/* No events to process now, wait for the next. */
			ulTaskNotifyTake( pdFALSE, ulMaxBlockTime );
		vTask_start( &xEmacTask );
		}

		if( ( *( pulISREvents ) & EMAC_IF_RX_EVENT ) != 0 )
		{
			*( pulISREvents ) &= ~EMAC_IF_RX_EVENT;
			while ( gmac_check_rx( pxMyInterfaces[ xEMACIndex ] ) > 0 ) { uxResult++; }
			#if( ipconfigUSE_LINKED_RX_MESSAGES != 0 )
			{
				if( pxFirstPacket != NULL )
				{
					vPassPackets();
				}
			}
			#endif
		}

		if( ( *( pulISREvents ) & EMAC_IF_TX_EVENT ) != 0 )
		{
			/* Code to release TX buffers if zero-copy is used. */
			*( pulISREvents ) &= ~EMAC_IF_TX_EVENT;
			/* Check if DMA packets have been delivered. */
			vClearTXBuffers();
		}

		if( ( *( pulISREvents ) & EMAC_IF_ERR_EVENT ) != 0 )
		{
			*( pulISREvents ) &= ~EMAC_IF_ERR_EVENT;
			gmac_check_errors( &xEMACif );
		}

		if( uxResult != 0ul )
		{
			/* A packet was received. No need to check for the PHY status now,
			but set a timer to check it later on. */
			vTaskSetTimeOutState( &xPhyTime );
			xPhyRemTime = pdMS_TO_TICKS( PHY_LS_HIGH_CHECK_TIME_MS );
		}
		else if( xTaskCheckForTimeOut( &xPhyTime, &xPhyRemTime ) != pdFALSE )
		{
		uint32_t ulStatus;
			ulStatus = ulReadMDIO( PHY_REG_01_BMSR );

			if( ( ulPHYLinkStatus & niBMSR_LINK_STATUS ) != ( ulStatus & niBMSR_LINK_STATUS ) )
			{
				ulPHYLinkStatus = ulStatus;
				FreeRTOS_printf( ( "prvEMACHandlerTask: PHY LS now %d\n", ( ulPHYLinkStatus & niBMSR_LINK_STATUS ) != 0 ) );
			}

			vTaskSetTimeOutState( &xPhyTime );
			if( ( ulPHYLinkStatus & niBMSR_LINK_STATUS ) != 0 )
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
