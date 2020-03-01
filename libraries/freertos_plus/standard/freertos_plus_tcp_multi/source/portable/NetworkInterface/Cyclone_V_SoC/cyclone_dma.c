/*
 * Access functions to DMA for the EMAC in Cyclone V SoC
 */

/* Standard includes. */
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

/* FreeRTOS includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"

/* FreeRTOS+TCP includes. */
#include "FreeRTOS_IP.h"
#include "FreeRTOS_Sockets.h"
#include "NetworkInterface.h"
#include "NetworkBufferManagement.h"

#include "UDPLoggingPrintf.h"

#include "cyclone_dma.h"
#include "cyclone_emac.h"

#include "socal/alt_emac.h"

void gmac_dma_axi( int iMacID, struct stmmac_axi *axi)
{
uint8_t *ioaddr = ucFirstIOAddres( iMacID );
uint32_t value;
int i;

	value = readl(ioaddr + DMA_AXI_BUS_MODE);

	if (axi->axi_lpi_en)
	{
		value |= DMA_AXI_EN_LPI;
	}

	if (axi->axi_xit_frm)
	{
		value |= DMA_AXI_LPI_XIT_FRM;
	}

	value &= ~DMA_AXI_WR_OSR_LMT;
	value |= (axi->axi_wr_osr_lmt & DMA_AXI_WR_OSR_LMT_MASK) << DMA_AXI_WR_OSR_LMT_SHIFT;

	value &= ~DMA_AXI_RD_OSR_LMT;
	value |= (axi->axi_rd_osr_lmt & DMA_AXI_RD_OSR_LMT_MASK) << DMA_AXI_RD_OSR_LMT_SHIFT;

	/* Depending on the UNDEF bit the Master AXI will perform any burst
	 * length according to the BLEN programmed (by default all BLEN are
	 * set).
	 */
	for( i = 0; i < AXI_BLEN; i++ )
	{
		switch( axi->axi_blen[ i ] )
		{
		case 256:
			value |= DMA_AXI_BLEN256;
			break;
		case 128:
			value |= DMA_AXI_BLEN128;
			break;
		case 64:
			value |= DMA_AXI_BLEN64;
			break;
		case 32:
			value |= DMA_AXI_BLEN32;
			break;
//		case 16:
//			value |= DMA_AXI_BLEN16;
//			break;
//		case 8:
//			value |= DMA_AXI_BLEN8;
//			break;
//		case 4:
//			value |= DMA_AXI_BLEN4;
//			break;
		}
	}

	writel( value, ioaddr + DMA_AXI_BUS_MODE );
}

void gmac_dma_init( int iMacID, struct stmmac_dma_cfg *dma_cfg )
{
uint8_t *ioaddr = ucFirstIOAddres( iMacID );
uint32_t value;
int txpbl;
int rxpbl;

	value = readl(ioaddr + DMA_BUS_MODE);
	txpbl = dma_cfg->txpbl ?: dma_cfg->pbl;
	rxpbl = dma_cfg->rxpbl ?: dma_cfg->pbl;
	/*
	 * Set the DMA PBL (Programmable Burst Length) mode.
	 *
	 * Note: before stmmac core 3.50 this mode bit was 4xPBL, and
	 * post 3.5 mode bit acts as 8*PBL.
	 */
	if (dma_cfg->pblx8)
	{
		value |= DMA_BUS_MODE_MAXPBL;
	}
	else
	{
		value &= ~( DMA_BUS_MODE_MAXPBL );
	}

//	value |= DMA_BUS_MODE_USP;

	value |= ALT_EMAC_DMA_BUS_MOD_EIGHTXPBL_SET_MSK;
	value &= ~(DMA_BUS_MODE_PBL_MASK | DMA_BUS_MODE_RPBL_MASK);
	value |= (txpbl << DMA_BUS_MODE_PBL_SHIFT);
	value |= (rxpbl << DMA_BUS_MODE_RPBL_SHIFT);

	/* Set the Fixed burst mode */
	if (dma_cfg->fixed_burst)
	{
		value |= DMA_BUS_MODE_FB;
	}
	else
	{
		value &= ~( DMA_BUS_MODE_FB );
	}

	/* Mixed Burst has no effect when fb is set */
	if (dma_cfg->mixed_burst)
	{
		value |= DMA_BUS_MODE_MB;
	}
	else
	{
		value &= ~( DMA_BUS_MODE_MB );
	}

	#if ( USE_ATDS != 0 )
	{
		/* Descriptor size is 32 bytes (8 DWORDS) */
		value |= DMA_BUS_MODE_ATDS;
	}
	#else
	{
		/* Descriptor size is 16 bytes (4 DWORDS) */
		value &= ~( DMA_BUS_MODE_ATDS );
	}
	#endif

	if (dma_cfg->aal)
	{
		value |= DMA_BUS_MODE_AAL;
	}
	else
	{
		value &= ~( DMA_BUS_MODE_AAL );
	}

	writel( value, ioaddr + DMA_BUS_MODE );

	/* Mask interrupts by writing to CSR7.
	Interrupts will be enabled later on.*/
	writel( 0u, ioaddr + DMA_INTR_ENA );
}

void gmac_dma_start_tx( int iMacID, uint32_t chan )
{
uint8_t *ioaddr = ucFirstIOAddres( iMacID );
uint32_t value;

	/* DMA_CONTROL, also called Operation Mode. */
	value = readl( ioaddr + DMA_CONTROL );
	/* Transmission in Run State */
	value |= DMA_CONTROL_ST;
	writel( value, ioaddr + DMA_CONTROL );
}

void gmac_dma_stop_tx( int iMacID, uint32_t chan )
{
uint8_t *ioaddr = ucFirstIOAddres( iMacID );
uint32_t value;

	value = readl( ioaddr + DMA_CONTROL );
	/* Transmission Stopped State */
	value &= ~DMA_CONTROL_ST;
	writel( value, ioaddr + DMA_CONTROL );
}

void gmac_dma_start_rx( int iMacID, uint32_t chan )
{
uint8_t *ioaddr = ucFirstIOAddres( iMacID );
uint32_t value;

	value = readl( ioaddr + DMA_CONTROL );
	/* Rx DMA operation is started */
	value |= DMA_CONTROL_SR;
	writel( value, ioaddr + DMA_CONTROL );
}

void gmac_dma_stop_rx( int iMacID, uint32_t chan )
{
uint8_t *ioaddr = ucFirstIOAddres( iMacID );
uint32_t value;

	value = readl( ioaddr + DMA_CONTROL );
	/* Rx DMA operation is stopped */
	value &= ~DMA_CONTROL_SR;
	writel( value, ioaddr + DMA_CONTROL );
}

static uint32_t gmac_configure_fc(uint32_t csr6 )
{
	csr6 &= ~DMA_CONTROL_RFA_MASK;
	csr6 &= ~DMA_CONTROL_RFD_MASK;

	/* Leave flow control disabled if receive fifo size is less than
	 * 4K or 0. Otherwise, send XOFF when fifo is 1K less than full,
	 * and send XON when 2K less than full.
	 */
	csr6 |= DMA_CONTROL_EFC;
	csr6 |= RFA_FULL_MINUS_1K;
	csr6 |= RFD_FULL_MINUS_2K;

	return csr6;
}

void gmac_dma_operation_mode( int iMacID, int txmode, int rxmode )
{
uint8_t *ioaddr = ucFirstIOAddres( iMacID );
uint32_t csr6 = readl( ioaddr + DMA_CONTROL );

	if( txmode == SF_DMA_MODE )
	{
		lUDPLoggingPrintf("GMAC: enable TX store and forward mode\n");
		/* Transmit COE type 2 cannot be done in cut-through mode. */
		csr6 |= DMA_CONTROL_TSF;
		/* Operating on second frame increase the performance
		 * especially when transmit store-and-forward is used.
		 */
		csr6 |= DMA_CONTROL_OSF;
	}
	else
	{
		lUDPLoggingPrintf("GMAC: disabling TX SF (threshold %d)\n", txmode);
		csr6 &= ~DMA_CONTROL_TSF;
		csr6 &= DMA_CONTROL_TC_TX_MASK;
		/* Set the transmit threshold */
		if (txmode <= 32)
			csr6 |= DMA_CONTROL_TTC_32;
		else if (txmode <= 64)
			csr6 |= DMA_CONTROL_TTC_64;
		else if (txmode <= 128)
			csr6 |= DMA_CONTROL_TTC_128;
		else if (txmode <= 192)
			csr6 |= DMA_CONTROL_TTC_192;
		else
			csr6 |= DMA_CONTROL_TTC_256;
	}

	if( rxmode == SF_DMA_MODE )
	{
		lUDPLoggingPrintf("GMAC: enable RX store and forward mode\n");
		csr6 |= DMA_CONTROL_RSF;
	}
	else
	{
		lUDPLoggingPrintf("GMAC: disable RX SF mode (threshold %d)\n", rxmode);
		csr6 &= ~DMA_CONTROL_RSF;
		csr6 &= DMA_CONTROL_TC_RX_MASK;
		if (rxmode <= 32)
			csr6 |= DMA_CONTROL_RTC_32;
		else if (rxmode <= 64)
			csr6 |= DMA_CONTROL_RTC_64;
		else if (rxmode <= 96)
			csr6 |= DMA_CONTROL_RTC_96;
		else
			csr6 |= DMA_CONTROL_RTC_128;
	}

	/* Configure flow control based on rx fifo size */
	csr6 = gmac_configure_fc( csr6 );

	writel( csr6, ioaddr + DMA_CONTROL );

	writel( 255, ioaddr + DMA_RX_WATCHDOG );
}

/* CSR1 enables the transmit DMA to check for new descriptor */
void gmac_dma_transmit_poll( int iMacID )
{
uint8_t *ioaddr = ucFirstIOAddres( iMacID );

	/* Write any number to "Transmit Poll Demand". */
	writel( 1, ioaddr + DMA_XMT_POLL_DEMAND );
}

/* CSR2 enables the transmit DMA to check for new descriptor */
void gmac_dma_reception_poll( int iMacID )
{
uint8_t *ioaddr = ucFirstIOAddres( iMacID );

	/* Write any number to "Received Poll Demand". */
	writel( 1, ioaddr + DMA_RCV_POLL_DEMAND );
}

gmac_rx_descriptor_t *gmac_get_rx_table( int iMacID )
{
uint8_t *ioaddr = ucFirstIOAddres( iMacID );
gmac_rx_descriptor_t *pxDMADescriptor;

	/* Set the RxDesc pointer with the first one of the pxDMATable list */
	pxDMADescriptor = ( gmac_rx_descriptor_t * )readl( ioaddr + DMA_RCV_BASE_ADDR );

	return pxDMADescriptor;
}

gmac_tx_descriptor_t *gmac_get_tx_table( int iMacID )
{
uint8_t *ioaddr = ucFirstIOAddres( iMacID );
gmac_tx_descriptor_t *pxDMADescriptor;

	/* Set the TxDesc pointer with the first one of the pxDMATable list */
	pxDMADescriptor = ( gmac_tx_descriptor_t * )readl( ioaddr + DMA_TX_BASE_ADDR );

	return pxDMADescriptor;
}

void gmac_set_rx_table( int iMacID, gmac_rx_descriptor_t * pxDMATable )
{
uint8_t *ioaddr = ucFirstIOAddres( iMacID );

	/* Set the RxDesc pointer with the first one of the pxDMATable list */
	writel( ( uint32_t ) pxDMATable, ioaddr + DMA_RCV_BASE_ADDR );
}

void gmac_set_tx_table( int iMacID, gmac_tx_descriptor_t * pxDMATable )
{
uint8_t *ioaddr = ucFirstIOAddres( iMacID );

	/* Set the TxDesc pointer with the first one of the pxDMATable list */
	writel( ( uint32_t ) pxDMATable, ioaddr + DMA_TX_BASE_ADDR );
}

BaseType_t gmac_tx_descriptor_init( int iMacID, gmac_tx_descriptor_t *pxDMATable, uint8_t *ucDataBuffer, uint32_t ulBufferCount )
{
uint32_t i = 0;
gmac_tx_descriptor_t *pxDMADescriptor;

	memset( pxDMATable, '\0', ulBufferCount * sizeof( *pxDMATable ) );

	/* Fill each DMA descriptor with the right values */
	for( i = 0; i < ulBufferCount; i++ )
	{
		/* Get the pointer on the ith member of the descriptor list */
		pxDMADescriptor = pxDMATable + i;

		/* Set Second Address Chained bit */
		pxDMADescriptor->second_address_chained = 1;

		/* Set Buffer1 address pointer */
		if( ucDataBuffer != NULL )
		{
			pxDMADescriptor->buf1_address = ( uint32_t )( &ucDataBuffer[ i * ETH_TX_BUF_SIZE ] );
		}
		else
		{
			/* Buffer space is not provided because it uses zero-copy transmissions. */
			pxDMADescriptor->buf1_address = ( uint32_t )0u;
		}

		#if( ipconfigHAS_TX_CRC_OFFLOADING != 0 )
		{
			/* Set the DMA Tx descriptors checksum insertion for TCP, UDP, and ICMP */
			pxDMADescriptor->checksum_mode = 0x03;
		}
		#endif

		/* Initialize the next descriptor with the Next Descriptor Polling Enable */
		if(i < ( ulBufferCount - 1 ) )
		{
			/* Set next descriptor address register with next descriptor base address */
			pxDMADescriptor->next_descriptor = ( uint32_t ) ( pxDMATable + i + 1 );
		}
		else
		{
			/* For last descriptor, set next descriptor address register equal to the first descriptor base address */
			pxDMADescriptor->next_descriptor = ( uint32_t ) pxDMATable;
		}
	}
	return ( BaseType_t ) 1;
}

BaseType_t gmac_rx_descriptor_init( int iMacID, gmac_rx_descriptor_t *pxDMATable, uint8_t *ucDataBuffer, uint32_t ulBufferCount )
{
uint32_t i = 0;
BaseType_t xReturn = 1;
gmac_rx_descriptor_t *pxDMADescriptor;

	memset( pxDMATable, '\0', ulBufferCount * sizeof( *pxDMATable ) );

	/* Fill each DMA descriptor with the right values */
	for( i = 0; i < ulBufferCount; i++ )
	{
		/* Get the pointer on the ith member of the descriptor list */
		pxDMADescriptor = pxDMATable + i;

		/* Set Own bit of the Rx descriptor Status */
		pxDMADescriptor->own = 1;

		/* Set Buffer1 size and Second Address Chained bit */
		pxDMADescriptor->second_address_chained = 1;
		pxDMADescriptor->buf1_byte_count = ETH_RX_BUF_SIZE;

		/* Set Buffer1 address pointer */
		if( ucDataBuffer != NULL )
		{
			pxDMADescriptor->buf1_address = ( uint32_t )( &ucDataBuffer[ i * ETH_RX_BUF_SIZE ] );
		}
		else
		{
		NetworkBufferDescriptor_t *pxBuffer;

			pxBuffer = pxGetNetworkBufferWithDescriptor( ETH_RX_BUF_SIZE, ( TickType_t ) 0u );
			if( pxBuffer != NULL )
			{
				/* Buffer space is not provided because it uses zero-copy reception. */
				pxDMADescriptor->buf1_address = ( uint32_t )pxBuffer->pucEthernetBuffer;
			}
			else
			{
				configASSERT( pxBuffer != NULL );
				xReturn = 0;
				break;
			}
		}

		/* Enable Ethernet DMA Rx Descriptor interrupt */
		pxDMADescriptor->disable_int_on_compl = 0;

		/* Initialize the next descriptor with the Next Descriptor Polling Enable */
		if(i < (ulBufferCount-1))
		{
			/* Set next descriptor address register with next descriptor base address */
			pxDMADescriptor->next_descriptor = (uint32_t)(pxDMATable+i+1);
		}
		else
		{
			/* For last descriptor, set next descriptor address register equal to the first descriptor base address */
			pxDMADescriptor->next_descriptor = ( uint32_t ) pxDMATable;
		}
	}

	return xReturn;
}

void gmac_check_errors( EMACInterface_t *pxEMACif )
{
}

void gmac_set_dma_interrupt_enable( int iMacID, uint32_t ulMask )
{
uint8_t *ioaddr = ucFirstIOAddres( iMacID );

	writel( ulMask, ioaddr + DMA_INTR_ENA );
}

uint32_t gmac_clear_dma_interrupt_status( int iMacID, uint32_t ulMask )
{
uint8_t *ioaddr = ucFirstIOAddres( iMacID );
uint32_t ulValue;

	/* Write one's to clear all bits. */
	writel( ulMask, ioaddr + DMA_STATUS );
	ulValue = readl( ioaddr + DMA_STATUS );

	return ulValue;
}

uint32_t gmac_get_dma_interrupt_status( int iMacID, int iClear )
{
uint8_t *ioaddr = ucFirstIOAddres( iMacID );
uint32_t ulValue;

	ulValue = readl( ioaddr + DMA_STATUS );
	if( iClear != 0 )
	{
		/* Write one's to clear all bits. */
		writel( ( uint32_t )~0ul, ioaddr + DMA_STATUS );
	}
	return ulValue;
}
