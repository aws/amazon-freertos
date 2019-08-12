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
#include <stdlib.h>
#include <stdarg.h>

/* FreeRTOS includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"

/* FreeRTOS+TCP includes. */
#include "FreeRTOS_IP.h"
#include "FreeRTOS_Sockets.h"
#include "FreeRTOS_IP_Private.h"
#include "FreeRTOS_DNS.h"
#include "NetworkBufferManagement.h"
#include "NetworkInterface.h"
#include "FreeRTOS_Routing.h"

#include "sam4e_xplained_pro.h"
#include "hr_gettime.h"
#include "conf_eth.h"
#include "ksz8851snl.h"
#include "ksz8851snl_reg.h"

/* Some files from the Atmel Software Framework */
#include <sysclk.h>
#include <pdc/pdc.h>
#include <spi/spi.h>

/*
	Sending a packet:

		1) Called by UP-task, add buffer to the TX-list:
			xNetworkInterfaceOutput()
				tx_buffers[ us_tx_head ] = pxNetworkBuffer;
				tx_busy[ us_tx_head ] = pdTRUE;
				us_tx_head++;

		2) Called by EMAC-Task: start SPI transfer
			ksz8851snl_update()
			if( ul_spi_pdc_status == SPI_PDC_IDLE )
			{
				if( ( tx_busy[ us_tx_tail ] != pdFALSE ) &&
					( us_pending_frame == 0 ) &&
					( ul_had_intn_interrupt == 0 ) )
				{
					// disable all interrupts.
					ksz8851_reg_write( REG_INT_MASK, 0 );
					Bring KSZ8851SNL_CSN_GPIO low
					ksz8851_fifo_write( pxNetworkBuffer->pucEthernetBuffer, xLength, xLength );
					ul_spi_pdc_status = SPI_PDC_TX_START;
					tx_cur_buffer = pxNetworkBuffer;
				}
			}
		3) Wait for SPI RXBUFF interrupt
			SPI_Handler()
				if( ul_spi_pdc_status == SPI_PDC_TX_START )
				{
					if( SPI_Status & SPI_SR_RXBUFF )
					{
						ul_spi_pdc_status = SPI_PDC_TX_COMPLETE;
					}
				}

		4) Called by EMAC-Task: finish SPI transfer
			ksz8851snl_update()
				if( ul_spi_pdc_status == SPI_PDC_TX_COMPLETE )
				{
					ul_spi_pdc_status = SPI_PDC_IDLE;
					Bring KSZ8851SNL_CSN_GPIO high
					// TX step12: disable TXQ write access.
					ksz8851_reg_clrbits( REG_RXQ_CMD, RXQ_START );
					// TX step12.1: enqueue frame in TXQ.
					ksz8851_reg_setbits( REG_TXQ_CMD, TXQ_ENQUEUE );

					// RX step13: enable INT_RX flag.
					ksz8851_reg_write( REG_INT_MASK, INT_RX );

					// Buffer sent, free the corresponding buffer and mark descriptor as owned by software.
					vReleaseNetworkBufferAndDescriptor( pxNetworkBuffer );

					tx_buffers[ us_tx_tail ] = NULL;
					tx_busy[ us_tx_tail ] = pdFALSE;
					us_tx_tail++
				}

	Receiving a packet:

		1) Wait for a INTN interrupt
			INTN_Handler()
				ul_had_intn_interrupt = 1
				vTaskNotifyGiveFromISR();	// Wake up the EMAC task

		2) Called by EMAC-Task: check for new fragments and start SPI transfer
			ksz8851snl_update()
				if( ul_spi_pdc_status == SPI_PDC_IDLE )
				{
					if( ( ul_had_intn_interrupt != 0 ) || ( us_pending_frame > 0 ) )
					{
						if( us_pending_frame == 0 )
						{
							us_pending_frame = ksz8851_reg_read(REG_RX_FRAME_CNT_THRES) >> 8;
							if( us_pending_frame == 0 )
							{
								break;
							}
						}
						// RX step2: disable all interrupts.
						ksz8851_reg_write( REG_INT_MASK, 0 );
						Check if there is a valid packet: REG_RX_FHR_STATUS
						Read the length of the next fragment: REG_RX_FHR_BYTE_CNT
						ul_spi_pdc_status = SPI_PDC_RX_START;
						gpio_set_pin_low(KSZ8851SNL_CSN_GPIO);
						// Start SPI data transfer
						ksz8851_fifo_read( pxNetworkBuffer->pucEthernetBuffer, xReadLength );
					}
				}

		3) Wait for SPI RXBUFF interrupt
			SPI_Handler()
			if( ul_spi_pdc_status == SPI_PDC_RX_START:
			{
				if( ( ulCurrentSPIStatus & SPI_SR_RXBUFF ) != 0 )
				{
					// Transfer complete, disable SPI RXBUFF interrupt.
					spi_disable_interrupt( KSZ8851SNL_SPI, SPI_IDR_RXBUFF );

					ul_spi_pdc_status = SPI_PDC_RX_COMPLETE;
				}
			}
		}

		4) Finish SPI transfer
			ksz8851snl_update()
				if( ul_spi_pdc_status == SPI_PDC_RX_COMPLETE )
				{
					ul_spi_pdc_status = SPI_PDC_IDLE;
					Bring KSZ8851SNL_CSN_GPIO high
					// RX step21: end RXQ read access.
					ksz8851_reg_clrbits(REG_RXQ_CMD, RXQ_START);
					// RX step22-23: update frame count to be read.
					us_pending_frame--
					// RX step24: enable INT_RX flag if transfer complete.
					if( us_pending_frame == 0 )
					{
						// Allow more RX interrupts.
						ksz8851_reg_write( REG_INT_MASK, INT_RX );
					}

					// Mark descriptor ready to be read.
					rx_ready[ rxHead ] = pdTRUE;
					rxHead++
				}
*/

#define PHY_REG_00_BMCR            0x00 // Basic mode control register
#define PHY_REG_01_BMSR            0x01 // Basic mode status register
#define PHY_REG_02_PHYSID1         0x02 // PHYS ID 1
#define PHY_REG_03_PHYSID2         0x03 // PHYS ID 2
#define PHY_REG_04_ADVERTISE       0x04 // Advertisement control reg
#define PHY_REG_05_LPA             0x05 // Link partner ability reg
#define	PHY_REG_06_ANER            0x06 //	6	RW		Auto-Negotiation Expansion Register
#define	PHY_REG_07_ANNPTR          0x07 //	7	RW		Auto-Negotiation Next Page TX
#define	PHY_REG_08_RESERVED0       0x08 // 0x08..0x0Fh	8-15	RW		RESERVED

#define BMSR_LINK_STATUS            0x0004  //!< Link status

#ifndef	PHY_LS_HIGH_CHECK_TIME_MS
	/* Check if the LinkSStatus in the PHY is still high after 15 seconds of not
	receiving packets. */
	#define PHY_LS_HIGH_CHECK_TIME_MS	15000
#endif

#ifndef	PHY_LS_LOW_CHECK_TIME_MS
	/* Check if the LinkSStatus in the PHY is still low every second. */
	#define PHY_LS_LOW_CHECK_TIME_MS	1000
#endif

/* Interrupt events to process.  Currently only the Rx event is processed
although code for other events is included to allow for possible future
expansion. */
#define EMAC_IF_RX_EVENT				1UL
#define EMAC_IF_TX_EVENT				2UL
#define EMAC_IF_ERR_EVENT				4UL
#define EMAC_IF_ALL_EVENT				( EMAC_IF_RX_EVENT | EMAC_IF_TX_EVENT | EMAC_IF_ERR_EVENT )

#define ETHERNET_CONF_PHY_ADDR	BOARD_GMAC_PHY_ADDR

#ifdef ipconfigHAS_TX_CRC_OFFLOADING
	#undef ipconfigHAS_TX_CRC_OFFLOADING
#endif
/* Override this define because the KSZ8851 is programmed to set all outgoing CRC's */
#define	ipconfigHAS_TX_CRC_OFFLOADING	1

#ifndef	EMAC_MAX_BLOCK_TIME_MS
	#define	EMAC_MAX_BLOCK_TIME_MS	100ul
#endif

/* Default the size of the stack used by the EMAC deferred handler task to 4x
the size of the stack used by the idle task - but allow this to be overridden in
FreeRTOSConfig.h as configMINIMAL_STACK_SIZE is a user definable constant. */
#ifndef configEMAC_TASK_STACK_SIZE
	#define configEMAC_TASK_STACK_SIZE ( 6 * configMINIMAL_STACK_SIZE )
#endif

#define SPI_PDC_IDLE			0
#define SPI_PDC_RX_START		1
#define SPI_PDC_TX_ERROR		2
#define SPI_PDC_RX_COMPLETE		3
#define SPI_PDC_TX_START		4
#define SPI_PDC_RX_ERROR		5
#define SPI_PDC_TX_COMPLETE		6

/**
 * ksz8851snl driver structure.
 */
typedef struct {
	/** Set to 1 when owner is software (ready to read), 0 for Micrel. */
	uint32_t rx_ready[MICREL_RX_BUFFERS];
	/** Set to 1 when owner is Micrel, 0 for software. */
	uint32_t tx_busy[MICREL_TX_BUFFERS];
	/** RX NetworkBufferDescriptor_t pointer list */
	NetworkBufferDescriptor_t *rx_buffers[MICREL_RX_BUFFERS];
	/** TX NetworkBufferDescriptor_t pointer list */
	NetworkBufferDescriptor_t *tx_buffers[MICREL_TX_BUFFERS];
	NetworkBufferDescriptor_t *tx_cur_buffer;

	/** Circular buffer head pointer for packet received. */
	uint32_t us_rx_head;
	/** Circular buffer tail pointer for packet to be read. */
	uint32_t us_rx_tail;
	/** Circular buffer head pointer by upper layer (buffer to be sent). */
	uint32_t us_tx_head;
	/** Circular buffer tail pointer incremented by handlers (buffer sent). */
	uint32_t us_tx_tail;

	uint32_t ul_total_tx;
	uint32_t ul_total_rx;
	uint32_t tx_space;

	/** Still experimental: hash table to allow certain multicast addresses. */
	uint16_t pusHashTable[ 4 ];

	/* ul_spi_pdc_status has "SPI_PDC_xxx" values. */
	volatile uint32_t ul_spi_pdc_status;

	/* ul_had_intn_interrupt becomes true within the INTN interrupt. */
	volatile uint32_t ul_had_intn_interrupt;

	uint16_t us_pending_frame;
} xKSZ8851_Device_t;

/* SPI PDC register base.
Declared in ASF\sam\components\ksz8851snl\ksz8851snl.c */
extern Pdc *g_p_spi_pdc;

/* Temporary buffer for PDC reception.
declared in ASF\sam\components\ksz8851snl\ksz8851snl.c */
extern uint8_t tmpbuf[1536];

COMPILER_ALIGNED(8)
static xKSZ8851_Device_t xMicrelDevice;

static TaskHandle_t xTransmitHandle;

/*-----------------------------------------------------------*/

/*
 * Wait a fixed time for the link status to indicate the network is up.
 */
static BaseType_t xGMACWaitLS( TickType_t xMaxTime );

#define	RX_LOGGING	1
#define	TX_LOGGING	2
unsigned logFlags = RX_LOGGING | TX_LOGGING;

void dbg_add_line( const char *pcFormat, ... );

/*
 * A deferred interrupt handler task that processes GMAC interrupts.
 */
static void prvEMACHandlerTask( void *pvParameters );

/*
 * Try to obtain an Rx packet from the hardware.
 */
static uint32_t prvEMACRxPoll( void );

static inline unsigned long ulReadMDIO( unsigned uAddress );

static void ksz8851snl_low_level_init( void );

static NetworkBufferDescriptor_t *ksz8851snl_low_level_input( void );

static BaseType_t xMicrelNetworkInterfaceInitialise( NetworkInterface_t *pxInterface );
static BaseType_t xMicrelGetPhyLinkStatus( NetworkInterface_t *pxInterface );
static BaseType_t xMicrelNetworkInterfaceOutput( NetworkInterface_t *pxInterface, NetworkBufferDescriptor_t * const pxNetworkBuffer, BaseType_t bReleaseAfterSend );

/*-----------------------------------------------------------*/

/* Bit map of outstanding ETH interrupt events for processing.  Currently only
the Rx interrupt is handled, although code is included for other events to
enable future expansion. */
static volatile uint32_t ulISREvents;

/* A copy of PHY register 1: 'PHY_REG_01_BMSR' */
static uint32_t ulPHYLinkStatus = 0;
static volatile BaseType_t xGMACSwitchRequired;

static void ksz8851snl_update( void );

static void ksz8851snl_rx_init( void );

static void ksz8851snl_tx_init( void );

/* Holds the handle of the task used as a deferred interrupt processor.  The
handle is used so direct notifications can be sent to the task for all EMAC/DMA
related interrupts. */
static TaskHandle_t xMicrelTaskHandle = NULL;

static NetworkInterface_t *pxMyInterface = NULL;

/*-----------------------------------------------------------*/

static BaseType_t xMicrelNetworkInterfaceInitialise( NetworkInterface_t *pxInterface )
{
const TickType_t x5_Seconds = 5000UL;

	if( xMicrelTaskHandle == NULL )
	{
		pxMyInterface = pxInterface;
		ksz8851snl_low_level_init();

		/* Wait at most 5 seconds for a Link Status in the PHY. */
		xGMACWaitLS( pdMS_TO_TICKS( x5_Seconds ) );

		/* The handler task is created at the highest possible priority to
		ensure the interrupt handler can return directly to it. */
		xTaskCreate( prvEMACHandlerTask, "KSZ8851", configEMAC_TASK_STACK_SIZE, NULL, configMAX_PRIORITIES - 1, &xMicrelTaskHandle );
		configASSERT( xMicrelTaskHandle );
/*
#warning Using for debugging with scope
		gpio_configure_pin( PIO_PA24_IDX, PIO_OUTPUT_1 );
		gpio_set_pin_low(PIO_PA24_IDX);
		gpio_configure_pin( PIO_PA15_IDX, PIO_OUTPUT_1 );
		gpio_set_pin_low(PIO_PA15_IDX);
*/
	}

	/* When returning non-zero, the stack will become active and
    start DHCP (in configured) */
	ulPHYLinkStatus = ulReadMDIO( PHY_REG_01_BMSR );

	return ( ulPHYLinkStatus & BMSR_LINK_STATUS ) != 0;
}
/*-----------------------------------------------------------*/

/* pxMicrel_FillInterfaceDescriptor() goes into the NetworkInterface.c of the Micrel driver. */

NetworkInterface_t *pxMicrel_FillInterfaceDescriptor( BaseType_t xEMACIndex, NetworkInterface_t *pxInterface )
{
/* This function pxMicrel_FillInterfaceDescriptor() adds a network-interface.
Make sure that the object pointed to by 'pxInterface'
is declared static or global, and that it will remain to exist. */

	memset( pxInterface, '\0', sizeof( *pxInterface ) );
	pxInterface->pcName				= "Micrel";	/* Just for logging, debugging. */
	pxInterface->pvArgument			= (void*)xEMACIndex;		/* Has only meaning for the driver functions. */
	pxInterface->pfInitialise		= xMicrelNetworkInterfaceInitialise;
	pxInterface->pfOutput			= xMicrelNetworkInterfaceOutput;
	pxInterface->pfGetPhyLinkStatus = xMicrelGetPhyLinkStatus;

	return pxInterface;
}
/*-----------------------------------------------------------*/

static BaseType_t xMicrelGetPhyLinkStatus( NetworkInterface_t *pxInterface )
{
BaseType_t xResult;

	/* This function returns true if the Link Status in the PHY is high. */
	if( ( ulPHYLinkStatus & BMSR_LINK_STATUS ) != 0 )
	{
		xResult = pdTRUE;
	}
	else
	{
		xResult = pdFALSE;
	}

	return xResult;
}
/*-----------------------------------------------------------*/

static BaseType_t xMicrelNetworkInterfaceOutput( NetworkInterface_t *pxInterface, NetworkBufferDescriptor_t * const pxNetworkBuffer, BaseType_t bReleaseAfterSend )
{
BaseType_t xResult = pdFALSE;
int txHead = xMicrelDevice.us_tx_head;

	/* Make sure the next descriptor is free. */
	if( xMicrelDevice.tx_busy[ txHead ] != pdFALSE )
	{
//		dbg_add_line( "Output: tail=%u head=%u", xMicrelDevice.us_tx_tail, txHead );
	}
	else if( ( ulPHYLinkStatus & BMSR_LINK_STATUS ) == 0 )
	{
		dbg_add_line( "Output: LS low" );
	}
	else
	{
		if( logFlags & TX_LOGGING )
		{
			dbg_add_line( "[T1] Output[%d]: %d byte", txHead, pxNetworkBuffer->xDataLength );
		}
		/* Pass the packet. */
		xMicrelDevice.tx_buffers[ txHead ] = pxNetworkBuffer;
		/* The descriptor is now owned by Micrel. */
		xMicrelDevice.tx_busy[ txHead ] = pdTRUE;

		/* Move the head pointer. */
		if( ++txHead == MICREL_TX_BUFFERS )
		{
			txHead = 0;
		}
		xMicrelDevice.us_tx_head = txHead;
		if( xMicrelTaskHandle != NULL )
		{
			xTaskNotifyGive( xMicrelTaskHandle );
		}

	#if( ipconfigZERO_COPY_TX_DRIVER != 1 )
		#warning Please ipconfigZERO_COPY_TX_DRIVER as 1
	#endif
		configASSERT( bReleaseAfterSend != pdFALSE );
		xResult = pdTRUE;
	}
	if( ( xResult == pdFALSE ) && ( bReleaseAfterSend  != pdFALSE ) )
	{
		vReleaseNetworkBufferAndDescriptor( pxNetworkBuffer );
	}
	return xResult;
}
/*-----------------------------------------------------------*/

/* This Micrel has numbered it's PHY registers in a different way.
Translate the register index. */
static int ks8851_phy_reg( int reg )
{
	switch (reg) {
	case PHY_REG_00_BMCR:
		return REG_PHY_CNTL;	// P1MBCR;
	case PHY_REG_01_BMSR:
		return REG_PHY_STATUS;
	case PHY_REG_02_PHYSID1:
		return REG_PHY_ID_LOW;
	case PHY_REG_03_PHYSID2:
		return REG_PHY_ID_HIGH;
	case PHY_REG_04_ADVERTISE:
		return REG_PHY_AUTO_NEGOTIATION;
	case PHY_REG_05_LPA:
		return REG_PHY_REMOTE_CAPABILITY;
	}

	return 0x0;
}
/*-----------------------------------------------------------*/

static inline unsigned long ulReadMDIO( unsigned uAddress )
{
uint16_t usPHYStatus;
int ks8851_reg = ks8851_phy_reg( uAddress );

	if( ks8851_reg != 0 )
	{
		usPHYStatus = ksz8851_reg_read( ks8851_reg );
	}
	else
	{
		/* Other addresses not yet implemented. */
		usPHYStatus = 0;
	}
	return usPHYStatus;
}
/*-----------------------------------------------------------*/

static BaseType_t xGMACWaitLS( TickType_t xMaxTime )
{
TickType_t xStartTime = xTaskGetTickCount();
TickType_t xEndTime;
BaseType_t xReturn;
const TickType_t xShortTime = pdMS_TO_TICKS( 100UL );
const uint32_t ulHz_Per_MHz = 1000000UL;

	for( ;; )
	{
		xEndTime = xTaskGetTickCount();

		if( ( xEndTime - xStartTime ) > xMaxTime )
		{
			/* Wated more than xMaxTime, return. */
			xReturn = pdFALSE;
			break;
		}

		/* Check the link status again. */
		ulPHYLinkStatus = ulReadMDIO( PHY_REG_01_BMSR );

		if( ( ulPHYLinkStatus & BMSR_LINK_STATUS ) != 0 )
		{
			/* Link is up - return. */
			xReturn = pdTRUE;
			break;
		}

		/* Link is down - wait in the Blocked state for a short while (to allow
		other tasks to execute) before checking again. */
		vTaskDelay( xShortTime );
	}

	FreeRTOS_printf( ( "xGMACWaitLS: %ld freq %lu Mz\n",
		xReturn,
		sysclk_get_cpu_hz() / ulHz_Per_MHz ) );

	return xReturn;
}
/*-----------------------------------------------------------*/

static void vPioSetPinHigh(uint32_t ul_pin)
{
	Pio *p_pio = (Pio *)((uint32_t)PIOA + (PIO_DELTA * (ul_pin >> 5)));
	// Value to be driven on the I/O line: 1.
	p_pio->PIO_SODR = 1 << (ul_pin & 0x1F);
}

extern uint32_t ff_write_status;
int tx_error_count, rx_error_count;
int spi_hand_stop;
/**
 * \brief Handler for SPI interrupt.
 */
void SPI_Handler(void)
{
BaseType_t xDoWakeup = pdFALSE;
BaseType_t xKSZTaskWoken = pdFALSE;
uint32_t ulCurrentSPIStatus;
uint32_t ulEnabledSPIStatus;

gpio_set_pin_high( PIO_PA15_IDX );
pio_get_pin_value( PIO_PA15_IDX );
if( spi_hand_stop != 0 )
{
	spi_hand_stop = 0;
}
	ulCurrentSPIStatus = spi_read_status( KSZ8851SNL_SPI );
	ulEnabledSPIStatus = spi_read_interrupt_mask( KSZ8851SNL_SPI );
	ulCurrentSPIStatus &= ulEnabledSPIStatus;
	spi_disable_interrupt( KSZ8851SNL_SPI, ulCurrentSPIStatus );

//dbg_add_line( "SPI %X (%X)", ulCurrentSPIStatus, ulEnabledSPIStatus);

	switch( xMicrelDevice.ul_spi_pdc_status )
	{
		case SPI_PDC_RX_START:
		{
			if( ( ulCurrentSPIStatus & SPI_SR_OVRES ) != 0 )
			{
				pdc_disable_transfer( g_p_spi_pdc, PERIPH_PTCR_RXTDIS | PERIPH_PTCR_TXTDIS );
gpio_set_pin_high(PIO_PA24_IDX);
				dbg_add_line( "[R3] RX OVRES %X", ff_write_status );
				xMicrelDevice.ul_spi_pdc_status = SPI_PDC_RX_ERROR;
rx_error_count++;
				xDoWakeup = pdTRUE;
			}
			else
			{
				if( ( ulCurrentSPIStatus & SPI_SR_RXBUFF ) != 0 )
				{
					if( logFlags & RX_LOGGING )
					{
						dbg_add_line( "[R3] SPI_Hand RX_COMPLETE" );
					}
					//gpio_set_pin_high( KSZ8851SNL_CSN_GPIO );
					xMicrelDevice.ul_spi_pdc_status = SPI_PDC_RX_COMPLETE;
					xDoWakeup = pdTRUE;
				}
				else
				{
					if( ulCurrentSPIStatus != SPI_SR_ENDRX )
					{
						dbg_add_line( "SPI_Hand status 0x%02X", ulCurrentSPIStatus );
					}
				}
			}
		}
		break;

		case SPI_PDC_TX_START:
		{
			/* Middle of TX. */
			if( ( ulCurrentSPIStatus & SPI_SR_OVRES ) != 0 )
			{
				pdc_disable_transfer( g_p_spi_pdc, PERIPH_PTCR_RXTDIS | PERIPH_PTCR_TXTDIS );
gpio_set_pin_high(PIO_PA24_IDX);
				dbg_add_line( "[T3] TX OVRES %X", ff_write_status );

				xMicrelDevice.ul_spi_pdc_status = SPI_PDC_TX_ERROR;
tx_error_count++;
				xDoWakeup = pdTRUE;
			}
			else
			{
				if( ( ulCurrentSPIStatus & SPI_SR_ENDRX ) != 0 )
				{
					/* Enable RX complete interrupt. */
					spi_enable_interrupt( KSZ8851SNL_SPI, SPI_IER_RXBUFF );
				}
				/* End of TX. */
				if( ( ulCurrentSPIStatus & SPI_END_OF_TX ) != 0 )
				{
					//gpio_set_pin_high( KSZ8851SNL_CSN_GPIO );
					if( logFlags & TX_LOGGING )
					{
						dbg_add_line( "[T3] SPI_Hand TX_COMPLETE %d", xMicrelDevice.us_tx_tail);
					}
					xMicrelDevice.ul_spi_pdc_status = SPI_PDC_TX_COMPLETE;
					xDoWakeup = pdTRUE;
				}
				else if( ulCurrentSPIStatus != SPI_SR_ENDRX )
				{
					dbg_add_line( "SPI_Hand status 0x%X", ulCurrentSPIStatus );
				}
			}
		}
		break;
	}	/* switch( xMicrelDevice.ul_spi_pdc_status ) */

	if( xDoWakeup != pdFALSE )
	{
		if( xMicrelTaskHandle != NULL )
		{
			vTaskNotifyGiveFromISR( xMicrelTaskHandle, ( BaseType_t * ) &xKSZTaskWoken );
		}
	}
	else
	{
	}
gpio_set_pin_low( PIO_PA15_IDX );
pio_get_pin_value( PIO_PA15_IDX );
	portEND_SWITCHING_ISR( xKSZTaskWoken );
}
/*-----------------------------------------------------------*/

static void INTN_Handler(uint32_t id, uint32_t mask)
{
BaseType_t xKSZTaskWoken = pdFALSE;

	if( logFlags & RX_LOGGING )
	{
		dbg_add_line( "[R1] INTN_Hand %d %d %d",
			xMicrelDevice.ul_had_intn_interrupt, xMicrelDevice.ul_spi_pdc_status, xMicrelDevice.us_pending_frame);
	}

	if( ( id == INTN_ID ) &&
		( mask == INTN_PIN_MSK ) )
	{
		/* Clear the PIO interrupt flags. */
		pio_get_interrupt_status( INTN_PIO );

		/* Set the INTN flag. */
		xMicrelDevice.ul_had_intn_interrupt++;
		if( xMicrelTaskHandle != NULL )
		{
			vTaskNotifyGiveFromISR( xMicrelTaskHandle, &( xKSZTaskWoken ) );
		}
	}
	portEND_SWITCHING_ISR( xKSZTaskWoken );
}
/*-----------------------------------------------------------*/

/**
 * \brief Populate the RX descriptor ring buffers with pbufs.
 *
 * \param p_ksz8851snl_dev Pointer to driver data structure.
 */
static void ksz8851snl_rx_populate_queue( void )
{
	uint32_t ul_index = 0;
	NetworkBufferDescriptor_t *pxNetworkBuffer;

	/* Set up the RX descriptors */
	for (ul_index = 0; ul_index < MICREL_RX_BUFFERS; ul_index++) {
		if( xMicrelDevice.rx_buffers[ ul_index ] == NULL )
		{
			/* Allocate a new NetworkBufferDescriptor_t with the maximum size. */
			pxNetworkBuffer = pxGetNetworkBufferWithDescriptor( ipconfigNETWORK_MTU + 36, 100 );
			if( pxNetworkBuffer == NULL )
			{
				FreeRTOS_printf( ( "ksz8851snl_rx_populate_queue: NetworkBufferDescriptor_t allocation failure\n" ) );
				configASSERT( 1 == 2 );
			}

			/* Make sure lwIP is well configured so one NetworkBufferDescriptor_t can contain the maximum packet size. */
			//LWIP_ASSERT("ksz8851snl_rx_populate_queue: NetworkBufferDescriptor_t size too small!", pbuf_clen(pxNetworkBuffer) <= 1);

			/* Save NetworkBufferDescriptor_t pointer to be sent to lwIP upper layer. */
			xMicrelDevice.rx_buffers[ ul_index ] = pxNetworkBuffer;
			/* Pass it to Micrel for reception. */
			xMicrelDevice.rx_ready[ ul_index ] = pdFALSE;
		}
	}
}

static int stopCount = -1;

unsigned tx_space, wait_tx_space, tx_status, fhr_status;
unsigned rx_debug = 0;
/**
 * \brief Update Micrel state machine and perform required actions.
 *
 * \param netif the lwIP network interface structure for this ethernetif.
 */
static void ksz8851snl_update()
{
	uint16_t txmir = 0;

gpio_set_pin_low( PIO_PA15_IDX );
/* Check for free PDC. */
	switch( xMicrelDevice.ul_spi_pdc_status )
	{
	case SPI_PDC_TX_ERROR:
		{
		uint32_t ulValue;
	//		/* TX step11: end TX transfer. */
			gpio_set_pin_high( KSZ8851SNL_CSN_GPIO );

			vTaskDelay( 2 ); gpio_set_pin_low( KSZ8851SNL_CSN_GPIO );
			vTaskDelay( 1 ); gpio_set_pin_high( KSZ8851SNL_CSN_GPIO );
			vTaskDelay( 1 );

			/* Disable asynchronous transfer mode. */
			xMicrelDevice.ul_spi_pdc_status = SPI_PDC_IDLE;

			/* TX step12: disable TXQ write access. */
			ksz8851_reg_clrbits( REG_RXQ_CMD, RXQ_START );

			ulValue = ksz8851snl_reset_tx();

			xMicrelDevice.tx_space = ksz8851_reg_read( REG_TX_MEM_INFO ) & TX_MEM_AVAILABLE_MASK;
			dbg_add_line("SPI_PDC_TX_ERROR %02X", ulValue );
			FreeRTOS_printf( ("SPI_PDC_TX_ERROR %02X\n", ulValue ) );
			if (stopCount == -1)
				stopCount = 6;
		}
		break;

	case SPI_PDC_RX_ERROR:
		{
		uint32_t ulValue;
			/* TX step11: end TX transfer. */
			gpio_set_pin_high( KSZ8851SNL_CSN_GPIO );

			vTaskDelay( 2 ); gpio_set_pin_low( KSZ8851SNL_CSN_GPIO );
			vTaskDelay( 1 ); gpio_set_pin_high( KSZ8851SNL_CSN_GPIO );
			vTaskDelay( 1 );

			/* Disable asynchronous transfer mode. */
			xMicrelDevice.ul_spi_pdc_status = SPI_PDC_IDLE;

			/* TX step12: disable TXQ write access. */
			ksz8851_reg_clrbits( REG_RXQ_CMD, RXQ_START );

			//ulValue = ksz8851snl_reset_rx();
			ulValue = ksz8851snl_reinit();

			xGMACWaitLS( pdMS_TO_TICKS( 5000UL ) );

			dbg_add_line("SPI_PDC_RX_ERROR %02X", ulValue);
			FreeRTOS_printf( ("SPI_PDC_RX_ERROR %02X\n", ulValue ) );
			if (stopCount == -1)
				stopCount = 6;
		}
		break;
	}
	if( stopCount > 0 )
	{
		stopCount--;
		if( stopCount == 0 )
		{
			dbg_add_line("Stop_debugger");
			FreeRTOS_printf( ("SPI_PDC Stop_debugger\n" ) );
		}
	}
	switch( xMicrelDevice.ul_spi_pdc_status )
	{
		case SPI_PDC_IDLE:
		{
		int txTail = xMicrelDevice.us_tx_tail;

			/*
			 * ========================== Handle RX ==========================
			 */
			if( ( xMicrelDevice.ul_had_intn_interrupt != 0 ) || ( xMicrelDevice.us_pending_frame > 0 ) )
			{
			int rxHead = xMicrelDevice.us_rx_head;
			NetworkBufferDescriptor_t *pxNetworkBuffer;
#warning try
				xMicrelDevice.ul_had_intn_interrupt = 0;

				if( xMicrelDevice.us_pending_frame == 0 )
				{
				uint16_t int_status;
					/* RX step1: read interrupt status for INT_RX flag. */
					int_status = ksz8851_reg_read( REG_INT_STATUS );

					if( ( int_status & INT_RX ) == 0 )
					{
						dbg_add_line("missed INT_RX?");
						//return;
					}

					/* RX step2: disable all interrupts. */
					ksz8851_reg_write( REG_INT_MASK, 0 );

					/* RX step3: clear INT_RX flag. */
					ksz8851_reg_setbits( REG_INT_STATUS, INT_RX );

					/* RX step4-5: check for received frames. */
					xMicrelDevice.us_pending_frame = ksz8851_reg_read(REG_RX_FRAME_CNT_THRES) >> 8;
					if( xMicrelDevice.us_pending_frame == 0 )
					{
						/* RX step24: enable INT_RX flag. */
						ksz8851_reg_write(REG_INT_MASK, INT_RX);
						return;
					}
				}
#warning try
				xMicrelDevice.ul_had_intn_interrupt = 0;

				/* Now xMicrelDevice.us_pending_frame != 0 */

				/* Don't break Micrel state machine, wait for a free descriptor first! */
				if( xMicrelDevice.rx_ready[ rxHead ] != pdFALSE )
				{
					FreeRTOS_printf( ( "ksz8851snl_update: out of free descriptor! [tail=%u head=%u]\n",
							xMicrelDevice.us_rx_tail, rxHead ) );
					return;
				}
				pxNetworkBuffer = xMicrelDevice.rx_buffers[ rxHead ];

				if( pxNetworkBuffer == NULL )
				{
					ksz8851snl_rx_populate_queue();
					FreeRTOS_printf( ( "ksz8851snl_update: no buffer set [head=%u]\n", rxHead ) );
					return;
				}

				/* RX step6: get RX packet status. */
				fhr_status = ksz8851_reg_read( REG_RX_FHR_STATUS );
				if( ( ( fhr_status & RX_VALID ) == 0 ) || ( ( fhr_status & RX_ERRORS ) != 0 ) )
				{
					ksz8851_reg_setbits(REG_RXQ_CMD, RXQ_CMD_FREE_PACKET);
					FreeRTOS_printf( ( "ksz8851snl_update: RX packet error!\n" ) );

					/* RX step4-5: check for received frames. */
					xMicrelDevice.us_pending_frame = ksz8851_reg_read(REG_RX_FRAME_CNT_THRES) >> 8;
					if( xMicrelDevice.us_pending_frame == 0 )
					{
						/* RX step24: enable INT_RX flag. */
						ksz8851_reg_write(REG_INT_MASK, INT_RX);
					}
					ulISREvents |= EMAC_IF_ERR_EVENT;
				}
				else
				{
				size_t xLength;
					/* RX step7: read frame length. */
					xLength = ksz8851_reg_read(REG_RX_FHR_BYTE_CNT) & RX_BYTE_CNT_MASK;

					/* RX step8: Drop packet if len is invalid or no descriptor available. */
					if( xLength == 0 )
					{
						ksz8851_reg_setbits( REG_RXQ_CMD, RXQ_CMD_FREE_PACKET );
						FreeRTOS_printf( ( "ksz8851snl_update: RX bad len!\n" ) );
						ulISREvents |= EMAC_IF_ERR_EVENT;
					}
					else
					{
					size_t xReadLength = xLength;

						xMicrelDevice.ul_total_rx++;
						/* RX step9: reset RX frame pointer. */
						ksz8851_reg_clrbits(REG_RX_ADDR_PTR, ADDR_PTR_MASK);

						/* RX step10: start RXQ read access. */
						ksz8851_reg_setbits(REG_RXQ_CMD, RXQ_START);
						if( logFlags & RX_LOGGING )
						{
							dbg_add_line( "[R2] RX_START %d byte", xLength );
						}
						/* RX step11-17: start asynchronous FIFO read operation. */
						xMicrelDevice.ul_spi_pdc_status = SPI_PDC_RX_START;
						gpio_set_pin_low( KSZ8851SNL_CSN_GPIO );
						if( ( xReadLength & ( sizeof( size_t ) - 1 ) ) != 0 )
						{
							xReadLength = ( xReadLength | ( sizeof( size_t ) - 1 ) ) + 1;
						}

						/* Pass the buffer minus 2 bytes, see ksz8851snl.c: RXQ_TWOBYTE_OFFSET. */
						ksz8851_fifo_read( pxNetworkBuffer->pucEthernetBuffer - 2, xReadLength );
						if( ( xReadLength & 0x03 ) != 0 )
						{
							dbg_add_line( "bad length %d", xReadLength );
						}

						/* Remove CRC and update buffer length. */
						xLength -= 4;
						pxNetworkBuffer->xDataLength = xLength;
						/* Wait for SPI interrupt to set status 'SPI_PDC_RX_COMPLETE'. */
					}
				}
				break;
			} /* ul_had_intn_interrupt || us_pending_frame */
			/*
			 * ========================== Handle TX ==========================
			 */

			/* Fetch next packet to be sent. */
			if( ( xMicrelDevice.tx_busy[ txTail ] != pdFALSE ) &&
				( xMicrelDevice.us_pending_frame == 0 ) &&
				( xMicrelDevice.ul_had_intn_interrupt == 0 ) )
			{
			NetworkBufferDescriptor_t *pxNetworkBuffer = xMicrelDevice.tx_buffers[ txTail ];
			size_t xOrgLength = pxNetworkBuffer->xDataLength;
			size_t xFIFOLength = pxNetworkBuffer->xDataLength;
			int iIndex = xFIFOLength;

				xFIFOLength = 4 * ( ( xFIFOLength + 3 ) / 4 );
				while( iIndex < ( int ) xFIFOLength )
				{
					pxNetworkBuffer->pucEthernetBuffer[ iIndex ] = '\0';
					iIndex++;
				}
				pxNetworkBuffer->xDataLength = xFIFOLength;

				/* TX step1: check if TXQ memory size is available for transmit. */
				txmir = ksz8851_reg_read( REG_TX_MEM_INFO );
				txmir = txmir & TX_MEM_AVAILABLE_MASK;

				if( txmir < ( xFIFOLength + 8 ) )
				{
					if( wait_tx_space == pdFALSE )
					{
						tx_status = ksz8851_reg_read( REG_TX_STATUS );
						fhr_status = ksz8851_reg_read( REG_RX_FHR_STATUS );
						dbg_add_line("[T2] space %d < %d", txmir, ( xFIFOLength + 8 ) );
						dbg_add_line("Status %04X FHR %04X", tx_status, fhr_status );
						wait_tx_space = pdTRUE;
					}
					//return;
					rx_debug = 1;
					tx_space = txmir;
				}
				else
				{
					tx_space = txmir;
//gpio_set_pin_high( PIO_PA15_IDX );
					/* TX step2: disable all interrupts. */
					ksz8851_reg_write( REG_INT_MASK, 0 );
if( logFlags & TX_LOGGING )
{
	fhr_status = ksz8851_reg_read( REG_RX_FHR_STATUS );
}
					xMicrelDevice.tx_space -= xFIFOLength;

					/* TX step3: enable TXQ write access. */
					ksz8851_reg_setbits( REG_RXQ_CMD, RXQ_START );
					if( logFlags & TX_LOGGING )
					{
						dbg_add_line( "[T2] TX_START %d sp %d len %d", txTail, txmir, xFIFOLength );
					}
					/* TX step4-8: perform FIFO write operation. */
					xMicrelDevice.ul_spi_pdc_status = SPI_PDC_TX_START;
					xMicrelDevice.tx_cur_buffer = pxNetworkBuffer;
					/* Bring SPI SS low. */
					gpio_set_pin_low( KSZ8851SNL_CSN_GPIO );
					xMicrelDevice.ul_total_tx++;

					ksz8851_fifo_write( pxNetworkBuffer->pucEthernetBuffer, xOrgLength, xFIFOLength );
				}
			}
		}
		break;	/* SPI_PDC_IDLE */

	case SPI_PDC_RX_COMPLETE:
		{
		int rxHead = xMicrelDevice.us_rx_head;
			/* RX step18-19: pad with dummy data to keep dword alignment. */
			/* Packet lengths will be rounded up to a multiple of "sizeof size_t". */
//			xLength = xMicrelDevice.rx_buffers[ rxHead ]->xDataLength & 3;
//			if( xLength != 0 )
//			{
//				ksz8851_fifo_dummy( 4 - xLength );
//			}

			/* RX step20: end RX transfer. */
			gpio_set_pin_high( KSZ8851SNL_CSN_GPIO );

			/* Disable asynchronous transfer mode. */
			xMicrelDevice.ul_spi_pdc_status = SPI_PDC_IDLE;

			/* RX step21: end RXQ read access. */
			ksz8851_reg_clrbits(REG_RXQ_CMD, RXQ_START);

			/* RX step22-23: update frame count to be read. */
			xMicrelDevice.us_pending_frame -= 1;
			if( logFlags & RX_LOGGING )
			{
				dbg_add_line( "[R4] RX_COMPL %d len %d pnd %d",
					rxHead, xMicrelDevice.rx_buffers[ rxHead ]->xDataLength, xMicrelDevice.us_pending_frame );
			}

			/* RX step24: enable INT_RX flag if transfer complete. */
			if( xMicrelDevice.us_pending_frame == 0 )
			{
				ksz8851_reg_write(REG_INT_MASK, INT_RX);
			}

			/* Mark descriptor ready to be read. */
			xMicrelDevice.rx_ready[ rxHead ] = pdTRUE;
			if( ++rxHead == MICREL_RX_BUFFERS )
			{
				rxHead = 0;
			}
			xMicrelDevice.us_rx_head = rxHead;
			if( rx_debug != 0 )
			{
			uint32_t txmir;
				rx_debug = 0;
				txmir = ksz8851_reg_read( REG_TX_MEM_INFO );
				txmir = txmir & TX_MEM_AVAILABLE_MASK;
				dbg_add_line( "RX Done %d", txmir );
			}
			/* Tell prvEMACHandlerTask that RX packets are available. */
			ulISREvents |= EMAC_IF_RX_EVENT;
		}	/* case SPI_PDC_RX_COMPLETE */
		break;

	case SPI_PDC_TX_COMPLETE:
		{
		int txTail = xMicrelDevice.us_tx_tail;
		NetworkBufferDescriptor_t *pxNetworkBuffer = xMicrelDevice.tx_buffers[ txTail ];

		size_t xLength;
			/* TX step9-10: pad with dummy data to keep dword alignment. */
			/* Not necessary: length is already a multiple of 4. */
			xLength = pxNetworkBuffer->xDataLength & 3;
			if( xLength != 0 )
			{
//				ksz8851_fifo_dummy( 4 - xLength );
			}

//			/* TX step11: end TX transfer. */
			gpio_set_pin_high( KSZ8851SNL_CSN_GPIO );

			/* Disable asynchronous transfer mode. */
			xMicrelDevice.ul_spi_pdc_status = SPI_PDC_IDLE;

			/* TX step12: disable TXQ write access. */
			ksz8851_reg_clrbits( REG_RXQ_CMD, RXQ_START );

			xMicrelDevice.tx_space = ksz8851_reg_read( REG_TX_MEM_INFO ) & TX_MEM_AVAILABLE_MASK;

			/* TX step12.1: enqueue frame in TXQ. */
			ksz8851_reg_setbits( REG_TXQ_CMD, TXQ_ENQUEUE );
//vTaskDelay( 3 );
#warning move this downward
			/* RX step13: enable INT_RX flag. */
//			ksz8851_reg_write( REG_INT_MASK, INT_RX );
//gpio_set_pin_low( PIO_PA15_IDX );
			/* Buffer sent, free the corresponding buffer and mark descriptor as owned by software. */
			vReleaseNetworkBufferAndDescriptor( pxNetworkBuffer );

			xMicrelDevice.tx_buffers[ txTail ] = NULL;
			xMicrelDevice.tx_busy[ txTail ] = pdFALSE;
			if( ++txTail == MICREL_TX_BUFFERS )
			{
				txTail = 0;
			}
			if( logFlags & TX_LOGGING )
			{
			uint32_t spi_status = spi_read_status( KSZ8851SNL_SPI );
				dbg_add_line( "[T4] TX_COMPLETE %d sp %d %04X", xMicrelDevice.us_tx_tail, xMicrelDevice.tx_space, spi_status );
				if( xMicrelDevice.tx_space == 0 )
				{
					vTaskDelay( 20 );
					xMicrelDevice.tx_space = ksz8851_reg_read( REG_TX_MEM_INFO ) & TX_MEM_AVAILABLE_MASK;
					dbg_add_line( "sp %d", xMicrelDevice.us_tx_tail, xMicrelDevice.tx_space );
				}

			}
			xMicrelDevice.us_tx_tail = txTail;
			/* Experiment. */
			//xMicrelDevice.ul_had_intn_interrupt = 1;
			if( xTransmitHandle != NULL )
			{
				dbg_add_line("TX Notify");
				xTaskNotifyGive( xTransmitHandle );
			}
#warning moved downward
			/* RX step13: enable INT_RX flag. */
			ksz8851_reg_write( REG_INT_MASK, INT_RX );
			/* Prevent the EMAC task from sleeping a single time. */
			ulISREvents |= EMAC_IF_TX_EVENT;
		}	/* case SPI_PDC_TX_COMPLETE */
		break;
	}	/* switch( xMicrelDevice.ul_spi_pdc_status ) */
}

/**
 * \brief Set up the RX descriptor ring buffers.
 *
 * This function sets up the descriptor list used for RX packets.
 *
 */
static void ksz8851snl_rx_init()
{
	uint32_t ul_index = 0;

	/* Init pointer index. */
	xMicrelDevice.us_rx_head = 0;
	xMicrelDevice.us_rx_tail = 0;

	/* Set up the RX descriptors. */
	for (ul_index = 0; ul_index < MICREL_RX_BUFFERS; ul_index++) {
		xMicrelDevice.rx_buffers[ul_index] = NULL;
		xMicrelDevice.rx_ready[ul_index] = pdFALSE;
	}

	/* Build RX buffer and descriptors. */
	ksz8851snl_rx_populate_queue();
}

/**
 * \brief Set up the TX descriptor ring buffers.
 *
 * This function sets up the descriptor list used for TX packets.
 *
 */
static void ksz8851snl_tx_init()
{
	uint32_t ul_index = 0;

	/* Init TX index pointer. */
	xMicrelDevice.us_tx_head = 0;
	xMicrelDevice.us_tx_tail = 0;

	/* Set up the TX descriptors */
	for( ul_index = 0; ul_index < MICREL_TX_BUFFERS; ul_index++ )
	{
		xMicrelDevice.tx_busy[ul_index] = pdFALSE;
	}
	xMicrelDevice.tx_space = 6144;
}
/*

static uint32_t const ulHashPolynomial = 0x04c11db7U;

static inline uint32_t prvCalcHasCrc( int length, unsigned char *data )
{
int32_t lCRC = -1;

	while( length-- > 0 )
	{
	uint8_t ucByte = *data++;
	int iBit;

		for( iBit = 0; iBit < 8; iBit++, ucByte >>= 1 )
		{
			lCRC = ( lCRC << 1 ) ^
				( ( lCRC < 0 ) ^ ( ucByte & 1 ) ? ulHashPolynomial : 0 );
		}
	}
	return ( uint32_t ) lCRC;
}

static void prvSetHash( const uint8_t *ucMacAddress )
{
	uint32_t ulCRC;

	// accept some multicast

	ulCRC = prvCalcHasCrc( ipMAC_ADDRESS_LENGTH_BYTES, ucMacAddress );
	ulCRC >>= (32 - 6);  // get top six bits

	xMicrelDevice.pusHashTable[ ulCRC >> 4 ] |= ( 1 << ( ulCRC & 0xf ) );
}
*/
static uint32_t prvGenerateCRC32( const uint8_t *ucAddress )
{
unsigned int j;
const uint32_t Polynomial = 0xEDB88320;
uint32_t crc = ~0ul;
const uint8_t *pucCurrent = ( const uint8_t * ) ucAddress;
const uint8_t *pucLast = pucCurrent + 6;

    /* Calculate  normal CRC32 */
    while( pucCurrent < pucLast )
    {
        crc ^= *( pucCurrent++ );
        for( j = 0; j < 8; j++ )
        {
            if( ( crc & 1 ) != 0 )
            {
                crc = (crc >> 1) ^ Polynomial;
            }
            else
            {
                crc >>= 1;
            }
        }
    }
    return ~crc;
}
/*-----------------------------------------------------------*/

static uint32_t prvGetHashIndex( const uint8_t *ucAddress )
{
uint32_t ulCrc = prvGenerateCRC32( ucAddress );
uint32_t ulIndex = 0ul;
BaseType_t xCount = 6;

    /* Take the lowest 6 bits of the CRC32 and reverse them */
    while( xCount-- )
    {
        ulIndex <<= 1;
        ulIndex |= ( ulCrc & 1 );
        ulCrc >>= 1;
    }

    /* This is the has value of 'ucAddress' */
    return ulIndex;
}
/*-----------------------------------------------------------*/

static void prvSetHash( const uint8_t* ucMacAddress )
{
BaseType_t xIndex;

    xIndex = prvGetHashIndex( ucMacAddress );
	xMicrelDevice.pusHashTable[ xIndex / 16 ] = ( 1u << ( xIndex % 16 ) );
}
/*-----------------------------------------------------------*/

/**
 * \brief Initialize ksz8851snl ethernet controller.
 *
 * \note Called from ethernetif_init().
 *
 * \param netif the lwIP network interface structure for this ethernetif.
 */
static void ksz8851snl_low_level_init( void )
{
	ksz8851snl_rx_init();
	ksz8851snl_tx_init();

	/* Enable NVIC interrupts. */
	NVIC_SetPriority(SPI_IRQn, INT_PRIORITY_SPI);
	NVIC_EnableIRQ(SPI_IRQn);

	/* Initialize SPI link. */
	if( ksz8851snl_init() < 0 )
	{
		FreeRTOS_printf( ( "ksz8851snl_low_level_init: failed to initialize the Micrel driver!\n" ) );
		configASSERT(0 == 1);
	}
	#if( ipconfigUSE_LLMNR == 1 )
	{
		prvSetHash( xLLMNR_MacAdress.ucBytes );
	}
	#endif /* ipconfigUSE_LLMNR */

	#if( ipconfigUSE_IPv6 != 0 )
	{
//		prvSetHash( xIPv6_MulticastAddress.ucBytes );
		NetworkEndPoint_t *pxEndPoint;

		for( pxEndPoint = FreeRTOS_FirstEndPoint( pxMyInterface );
			pxEndPoint != NULL;
			pxEndPoint = FreeRTOS_NextEndPoint( pxMyInterface, pxEndPoint ) )
		{
			if( pxEndPoint->bits.bIPv6 != pdFALSE_UNSIGNED )
			{
			unsigned char ucMACAddress[ 6 ] = { 0x33, 0x33, 0xff, 0, 0, 0 };

				ucMACAddress[ 3 ] = pxEndPoint->ulIPAddress_IPv6.ucBytes[ 13 ];
				ucMACAddress[ 4 ] = pxEndPoint->ulIPAddress_IPv6.ucBytes[ 14 ];
				ucMACAddress[ 5 ] = pxEndPoint->ulIPAddress_IPv6.ucBytes[ 15 ];
				prvSetHash( ucMACAddress );
			}
		}
		#if( ipconfigUSE_LLMNR == 1 )
		{
			prvSetHash( xLLMNR_MacAdressIPv6.ucBytes );
		}
		#endif /* ipconfigUSE_LLMNR */
	}
	#endif
//	ksz8851_reg_write( REG_MAC_HASH_0, xMicrelDevice.pusHashTable[ 0 ] );
//	ksz8851_reg_write( REG_MAC_HASH_2, xMicrelDevice.pusHashTable[ 1 ] );
//	ksz8851_reg_write( REG_MAC_HASH_4, xMicrelDevice.pusHashTable[ 2 ] );
//	ksz8851_reg_write( REG_MAC_HASH_6, xMicrelDevice.pusHashTable[ 3 ] );
	memset( xMicrelDevice.pusHashTable, 255, sizeof( xMicrelDevice.pusHashTable ) );
	ksz8851_reg_write( REG_MAC_HASH_0, FreeRTOS_htons( xMicrelDevice.pusHashTable[ 0 ] ) );
	ksz8851_reg_write( REG_MAC_HASH_2, FreeRTOS_htons( xMicrelDevice.pusHashTable[ 1 ] ) );
	ksz8851_reg_write( REG_MAC_HASH_4, FreeRTOS_htons( xMicrelDevice.pusHashTable[ 2 ] ) );
	ksz8851_reg_write( REG_MAC_HASH_6, FreeRTOS_htons( xMicrelDevice.pusHashTable[ 3 ] ) );

	/* Initialize interrupt line INTN. */
	configure_intn( INTN_Handler );
}

/**
 * \brief Use pre-allocated pbuf as DMA source and return the incoming packet.
 *
 * \param netif the lwIP network interface structure for this ethernetif.
 *
 * \return a pbuf filled with the received packet (including MAC header).
 * 0 on memory error.
 */
static NetworkBufferDescriptor_t *ksz8851snl_low_level_input( void )
{
NetworkBufferDescriptor_t *pxNetworkBuffer = NULL;
int rxTail = xMicrelDevice.us_rx_tail;

	/* Check that descriptor is owned by software (ie packet received). */
	if( xMicrelDevice.rx_ready[ rxTail ] != pdFALSE )
	{

		/* Fetch pre-allocated buffer */
		pxNetworkBuffer = xMicrelDevice.rx_buffers[ rxTail ];

		/* Remove this pbuf from its descriptor. */
		xMicrelDevice.rx_buffers[ rxTail ] = NULL;

		/* Clears rx_ready and sets rx_buffers. */
		ksz8851snl_rx_populate_queue();

		if( ++rxTail == MICREL_RX_BUFFERS )
		{
			rxTail = 0;
		}
		xMicrelDevice.us_rx_tail = rxTail;
	}

	return pxNetworkBuffer;
}
/*-----------------------------------------------------------*/

static const char *pcProtocolName( uint8_t ucProtocol )
{
	switch( ucProtocol )
	{
		case ipPROTOCOL_ICMP: return "ICMP";
		case ipPROTOCOL_IGMP: return "IGMP";
		case ipPROTOCOL_TCP: return "TCP";
		case ipPROTOCOL_UDP: return "UDP";
	}
	return "Unknown";
}

static uint32_t prvEMACRxPoll( void )
{
NetworkBufferDescriptor_t *pxNetworkBuffer;
IPStackEvent_t xRxEvent = { eNetworkRxEvent, NULL };
uint32_t ulReturnValue = 0;

	for( ;; )
	{
	/* Only for logging. */
	int rxTail = xMicrelDevice.us_rx_tail;
	EthernetHeader_t *pxEthernetHeader;
	IPPacket_t *pxIPPacket_t;

	pxNetworkBuffer = ksz8851snl_low_level_input();

		if( pxNetworkBuffer == NULL )
		{
			break;
		}
		pxEthernetHeader = ( EthernetHeader_t * )pxNetworkBuffer->pucEthernetBuffer;

		if( ( pxEthernetHeader->usFrameType != ipIPv4_FRAME_TYPE ) &&
			( pxEthernetHeader->usFrameType != ipARP_FRAME_TYPE	) )
		{
			if( pxEthernetHeader->usFrameType != ipIPv6_FRAME_TYPE )
			{
				FreeRTOS_printf( ( "ksz8851: Frame type %02X received\n", pxEthernetHeader->usFrameType ) );
			}
		}
		else
		{
			pxIPPacket_t = ( IPPacket_t * )pxNetworkBuffer->pucEthernetBuffer;
			//if( pxIPPacket_t->xIPHeader.ucProtocol == ipPROTOCOL_ICMP )
			if( /* ( pxEthernetHeader->usFrameType == ipIPv4_FRAME_TYPE ) && */ ( pxIPPacket_t->xIPHeader.ucProtocol == ipPROTOCOL_TCP ) )
			{
				//FreeRTOS_printf( ( "ksz8851: TCP\n" ) );
			}
			if( ( pxEthernetHeader->usFrameType != ipIPv4_FRAME_TYPE ) ||
				( ( pxIPPacket_t->xIPHeader.ucProtocol != ipPROTOCOL_UDP ) && ( pxIPPacket_t->xIPHeader.ucProtocol != ipPROTOCOL_TCP ) ) )
			{
				if( pxEthernetHeader->usFrameType != ipARP_FRAME_TYPE )
				{
/*
					FreeRTOS_printf( ( "ksz8851: Recv frame %04X prot %02X  (%s)\n",
						( unsigned )pxEthernetHeader->usFrameType,
						( unsigned )pxIPPacket_t->xIPHeader.ucProtocol,
						pcProtocolName( pxIPPacket_t->xIPHeader.ucProtocol ) ) );
*/
				}
			}
		}
		ulReturnValue++;

		pxNetworkBuffer->pxInterface = pxMyInterface;
		pxNetworkBuffer->pxEndPoint = FreeRTOS_MatchingEndpoint( pxMyInterface, pxNetworkBuffer->pucEthernetBuffer );

		if( logFlags & RX_LOGGING )
		{
			unsigned char *ptr = pxNetworkBuffer->pucEthernetBuffer;
			dbg_add_line("RX[%d]  %02X:%02X:%02X  %02X:%02X:%02X",
				rxTail,
				ptr[ 3 ], ptr[ 4 ], ptr[ 5 ], ptr[ 9 ], ptr[ 10 ], ptr[ 11 ]);
		}
		xRxEvent.pvData = ( void * )pxNetworkBuffer;
		/* Send the descriptor to the IP task for processing. */
		if( xSendEventStructToIPTask( &xRxEvent, 100UL ) != pdTRUE )
		{
			vReleaseNetworkBufferAndDescriptor( pxNetworkBuffer );
			iptraceETHERNET_RX_EVENT_LOST();
			FreeRTOS_printf( ( "prvEMACRxPoll: Can not queue return packet!\n" ) );
		}
	}

	return ulReturnValue;
}
/*-----------------------------------------------------------*/

static void prvEMACHandlerTask( void *pvParameters )
{
TimeOut_t xPhyTime;
TickType_t xPhyRemTime;
TickType_t xLoggingTime;
UBaseType_t uxLastMinBufferCount = 0;
UBaseType_t uxCurrentCount;
BaseType_t xResult = 0;
uint32_t xStatus;
const TickType_t ulMaxBlockTime = pdMS_TO_TICKS( EMAC_MAX_BLOCK_TIME_MS );
#if( ipconfigCHECK_IP_QUEUE_SPACE != 0 )
	UBaseType_t uxLastMinQueueSpace = 0;
#endif

	/* Remove compiler warnings about unused parameters. */
	( void ) pvParameters;

	configASSERT( xMicrelTaskHandle );

	vTaskSetTimeOutState( &xPhyTime );
	xPhyRemTime = pdMS_TO_TICKS( PHY_LS_LOW_CHECK_TIME_MS );
	xLoggingTime = xTaskGetTickCount();

	for( ;; )
	{
		uxCurrentCount = uxGetMinimumFreeNetworkBuffers();
		if( uxLastMinBufferCount != uxCurrentCount )
		{
			/* The logging produced below may be helpful
			while tuning +TCP: see how many buffers are in use. */
			uxLastMinBufferCount = uxCurrentCount;
			if( uxCurrentCount < 10 )
			{
				FreeRTOS_printf( ( "Network buffers: %lu lowest %lu\n",
					uxGetNumberOfFreeNetworkBuffers(), uxCurrentCount ) );
			}
		}

		#if( ipconfigCHECK_IP_QUEUE_SPACE != 0 )
		{
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

		ksz8851snl_update();

		if( ( ulISREvents & EMAC_IF_ALL_EVENT ) == 0 )
		{
			/* No events to process now, wait for the next. */
			ulTaskNotifyTake( pdTRUE, ulMaxBlockTime );
		}

//		dbg_add_line( "Now" );
		if( ( xTaskGetTickCount() - xLoggingTime ) > 10000 )
		{
			xLoggingTime += 10000;
//			FreeRTOS_printf( ( "Now Tx/Rx %7d /%7d Errors %d/%d\n",
//				xMicrelDevice.ul_total_tx, xMicrelDevice.ul_total_rx, tx_error_count, rx_error_count ) );
		}

		if( ( ulISREvents & EMAC_IF_RX_EVENT ) != 0 )
		{
			ulISREvents &= ~EMAC_IF_RX_EVENT;

			/* Wait for the EMAC interrupt to indicate that another packet has been
			received. */
			xResult = prvEMACRxPoll();
		}

		if( ( ulISREvents & EMAC_IF_TX_EVENT ) != 0 )
		{
			/* Future extension: code to release TX buffers if zero-copy is used. */
			ulISREvents &= ~EMAC_IF_TX_EVENT;
		}

		if( ( ulISREvents & EMAC_IF_ERR_EVENT ) != 0 )
		{
			/* Future extension: logging about errors that occurred. */
			ulISREvents &= ~EMAC_IF_ERR_EVENT;
		}

		if( xResult > 0 )
		{
			/* As long as packets are being received, assume that
			the Link Status is high. */
			ulPHYLinkStatus |= BMSR_LINK_STATUS;
			/* A packet was received. No need to check for the PHY status now,
			but set a timer to check it later on. */
			vTaskSetTimeOutState( &xPhyTime );
			xPhyRemTime = pdMS_TO_TICKS( PHY_LS_HIGH_CHECK_TIME_MS );
			xResult = 0;
		}
		else if( ( xTaskCheckForTimeOut( &xPhyTime, &xPhyRemTime ) != pdFALSE ) &&
			( xMicrelDevice.ul_spi_pdc_status == SPI_PDC_IDLE ) )
		{
			/* Check the link status again. */
			xStatus = ulReadMDIO( PHY_REG_01_BMSR );

			if( ( ulPHYLinkStatus & BMSR_LINK_STATUS ) != ( xStatus & BMSR_LINK_STATUS ) )
			{
				ulPHYLinkStatus = xStatus;
				FreeRTOS_printf( ( "prvEMACHandlerTask: PHY LS now %d\n", ( ulPHYLinkStatus & BMSR_LINK_STATUS ) != 0 ) );
			}

			vTaskSetTimeOutState( &xPhyTime );
			if( ( ulPHYLinkStatus & BMSR_LINK_STATUS ) != 0 )
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

#define EVENT_COUNT		64

char ev[ EVENT_COUNT ][ 52 ];
char *textArray[ EVENT_COUNT ];

volatile int eventHead = 0;

void dbg_add_line( const char *pcFormat, ... )
{
int rc;
uint64_t ullCurTime = ullGetHighResolutionTime();
uint32_t ulSec, ulMs, ulUs;
int head = eventHead;

	if( ++eventHead == EVENT_COUNT )
	{
		eventHead = 0;
	}
	ulMs = ( uint32_t ) ( ullCurTime / 1000 );
	ulUs = ( uint32_t ) ( ullCurTime % 1000 );
	ulSec = ulMs / 1000;
	ulMs = ulMs % 1000;
	va_list args;
	va_start( args, pcFormat );
	rc = snprintf( ev[ head ], sizeof ev[ head ], "%4lu.%03lu.%03lu ", ulSec, ulMs, ulUs );
	vsnprintf( ev[ head ] + rc, sizeof ev[ head ] - rc, pcFormat, args );
	textArray[ head ] = ev[ head ];
	va_end( args );
}
