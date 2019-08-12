/*
 * Access functions to the hard-wired EMAC in Cyclone V SoC
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
#include "FreeRTOS_IP_Private.h"
#include "NetworkBufferManagement.h"
#include "NetworkInterface.h"

#include "cyclone_dma.h"
#include "cyclone_emac.h"
#include "cyclone_phy.h"

#include "socal/alt_rstmgr.h"

#include "UDPLoggingPrintf.h"

#ifndef ARRAY_SIZE
	#define ARRAY_SIZE(x)	(BaseType_t)(sizeof(x)/sizeof(x)[0])
#endif

/* The lowest 8 bits contain the IP's version number. We expect 0x37. */
#define HAS_GMAC4		( ( ulEMACVersion & 0xff ) >= 0x40 )

uint32_t ulEMACVersion;

/* Enable disable MAC RX/TX */
void gmac_enable_transmission(int iMacID, bool enable)
{
uint8_t *ioaddr = ucFirstIOAddres( iMacID );
uint32_t value;

	value = readl(ioaddr + MAC_CTRL_REG);

	if (enable)
	{
		value |= MAC_ENABLE_RX | MAC_ENABLE_TX;
	}
	else
	{
		value &= ~( MAC_ENABLE_TX | MAC_ENABLE_RX );
	}

	writel(value, ioaddr + MAC_CTRL_REG);
}

void gmac_set_MAC_address(int iMacID, const uint8_t *ucMACAddress, uint32_t ulIndex )
{
uint8_t *ioaddr = ucFirstIOAddres( iMacID );
uint32_t ulData;

	ulData =
		( ( ( uint32_t ) ucMACAddress[ 5 ] ) <<  8 ) |
		  ( ( uint32_t ) ucMACAddress[ 4 ] );
	/* For MAC Addr registers we have to set the Address Enable (AE)
	 * bit that has no effect on the High Reg 0 where the bit 31 (MO)
	 * is RO.
	 */
	writel( ulData | GMAC_HI_REG_AE, ioaddr + GMAC_ADDR_HIGH( ulIndex ) );

	ulData =
		( ( ( uint32_t ) ucMACAddress[ 3 ] ) << 24 ) |
		( ( ( uint32_t ) ucMACAddress[ 2 ] ) << 16 ) |
		( ( ( uint32_t ) ucMACAddress[ 1 ] ) <<  8 ) |
		  ( ( uint32_t ) ucMACAddress[ 0 ] );

	writel( ulData, ioaddr + GMAC_ADDR_LOW( ulIndex ) );
}

/**
 * gmac_mdio_read
 * @bus: points to the mii_bus structure
 * @phyaddr: MII addr
 * @phyreg: MII reg
 * Description: it reads data from the MII register from within the phy device.
 * For the 7111 GMAC, we must set the bit 0 in the MII address register while
 * accessing the PHY registers.
 * Fortunately, it seems this has no drawback for the 7109 MAC.
 */
/*
	mac->mii.addr = GMAC_MII_ADDR;
	mac->mii.data = GMAC_MII_DATA;
	mac->mii.addr_shift = 11;
	mac->mii.addr_mask = 0x0000F800;
	mac->mii.reg_shift = 6;
	mac->mii.reg_mask = 0x000007C0;
	mac->mii.clk_csr_shift = 2;
	mac->mii.clk_csr_mask = GENMASK(5, 2);
*/
#define GMAC_MDIO_ADDR_SHIFT		11
#define GMAC_MDIO_ADDR_MASK			0x0000F800u

#define GMAC_MDIO_REG_SHIFT			6
#define GMAC_MDIO_REG_MASK			0x000007C0u
#define GMAC_MDIO_CLK_CSR_SHIFT		2
#define GMAC_MDIO_CLK_CSR_MASK		GENMASK(5, 2)

/* Define the macros for CSR clock range parameters to be passed by
 * platform code.
 * This could also be configured at run time using CPU freq framework. */

/* MDC Clock Selection define*/
#define	STMMAC_CSR_60_100M	0x0	/* MDC = clk_scr_i/42 */
#define	STMMAC_CSR_100_150M	0x1	/* MDC = clk_scr_i/62 */
#define	STMMAC_CSR_20_35M	0x2	/* MDC = clk_scr_i/16 */
#define	STMMAC_CSR_35_60M	0x3	/* MDC = clk_scr_i/26 */
#define	STMMAC_CSR_150_250M	0x4	/* MDC = clk_scr_i/102 */
#define	STMMAC_CSR_250_300M	0x5	/* MDC = clk_scr_i/122 */

#define GMAC_MDIO_CLK_CSR			STMMAC_CSR_250_300M

#define MII_BUSY		0x00000001
#define MII_WRITE		0x00000002

/* GMAC4 defines */
#define MII_GMAC4_GOC_SHIFT		2
#define MII_GMAC4_WRITE			(1 << MII_GMAC4_GOC_SHIFT)
#define MII_GMAC4_READ			(3 << MII_GMAC4_GOC_SHIFT)

static int waitNotBusy( uint8_t *pucAddress, uint32_t timeout_ms )
{
uint32_t value;
TickType_t xStart = xTaskGetTickCount(), xEnd;

	for( ;; )
	{
		value = readl( pucAddress );
		if( ( value & MII_BUSY ) == 0u )
		{
			break;
		}
		xEnd = xTaskGetTickCount();
		if( ( xEnd - xStart ) > timeout_ms )
		{
			/* Time-out reached. */
			return 0;
		}
	}
	return 1;
}

int gmac_mdio_read( int iMacID, int phyaddr, int phyreg)
{
uint8_t *ioaddr = ucFirstIOAddres( iMacID );
uint32_t mii_address = GMAC_MII_ADDR;
uint32_t mii_data = GMAC_MII_DATA;
int data;
uint32_t value = MII_BUSY;

	value |= ( phyaddr << GMAC_MDIO_ADDR_SHIFT ) & GMAC_MDIO_ADDR_MASK;

	value |= ( phyreg << GMAC_MDIO_REG_SHIFT ) & GMAC_MDIO_REG_MASK;
	value |= ( GMAC_MDIO_CLK_CSR << GMAC_MDIO_CLK_CSR_SHIFT ) & GMAC_MDIO_CLK_CSR_MASK;

	if( HAS_GMAC4 != 0 )
	{
		value |= MII_GMAC4_READ;
	}
	else
	{
		/* bit-1 remains zero when reading. */
	}

	if( !waitNotBusy( ioaddr + mii_address, 10 ) )
	{
		return -1;
	}

	writel(value, ioaddr + mii_address);

	if ( !waitNotBusy( ioaddr + mii_address, 10 ) )
	{
		return -1;
	}

	/* Read the data from the MII data register */
	data = (int)readl( ioaddr + mii_data );

	return data;
}

/**
 * gmac_mdio_write
 * @bus: points to the mii_bus structure
 * @phyaddr: MII addr
 * @phyreg: MII reg
 * @phydata: phy data
 * Description: it writes the data into the MII register from within the device.
 */
int gmac_mdio_write( int iMacID, int phyaddr, int phyreg, uint16_t phydata )
{
uint8_t *ioaddr = ucFirstIOAddres( iMacID );
uint32_t mii_address = GMAC_MII_ADDR;
uint32_t mii_data = GMAC_MII_DATA;
uint32_t value = MII_BUSY;

	value |= ( phyaddr << GMAC_MDIO_ADDR_SHIFT ) & GMAC_MDIO_ADDR_MASK;
	value |= ( phyreg << GMAC_MDIO_REG_SHIFT ) & GMAC_MDIO_REG_MASK;

	value |= ( GMAC_MDIO_CLK_CSR << GMAC_MDIO_CLK_CSR_SHIFT ) & GMAC_MDIO_CLK_CSR_MASK;

	if( HAS_GMAC4 != 0 )
	{
		value |= MII_GMAC4_WRITE;
	}
	else
	{
		value |= MII_WRITE;
	}

	/* Wait until any existing MII operation is complete */
	if( !waitNotBusy( ioaddr + mii_address, 10 ) )
	{
		return -1;
	}

	/* Set the MII address register to write */
	writel( phydata, ioaddr + mii_data );
	writel( value, ioaddr + mii_address );

	/* Wait until any existing MII operation is complete */
	if( !waitNotBusy( ioaddr + mii_address, 10 ) )
	{
		return -1;
	}
	return 0;
}

void gmac_set_emac_interrupt_disable( int iMacID, uint32_t ulMask )
{
uint8_t *ioaddr = ucFirstIOAddres( iMacID );

	writel( ulMask, ioaddr + GMAC_INT_MASK );
}

uint32_t gmac_get_emac_interrupt_status( int iMacID, int iClear )
{
uint8_t *ioaddr = ucFirstIOAddres( iMacID );
uint32_t ulValue;

	ulValue = readl( ioaddr + GMAC_INT_STATUS );
	if( iClear != 0 )
	{
		writel( ( uint32_t )~0ul, ioaddr + GMAC_INT_STATUS );
	}
	return ulValue;
}

void gmac_clear_emac_interrupt_status( int iMacID, uint32_t ulMask )
{
uint8_t *ioaddr = ucFirstIOAddres( iMacID );

	writel( ulMask, ioaddr + GMAC_INT_STATUS );
}

void gmac_init( int iMacID )
{
uint8_t *ioaddr = ucFirstIOAddres( iMacID );
int iIndex;

	{
		struct stmmac_axi xAXI;
		memset( &xAXI, '\0', sizeof xAXI );
		for( iIndex = 0; iIndex < ARRAY_SIZE( xAXI.axi_blen ); iIndex++ )
		{
			/* Get an array of 4, 8, 16, 32, 64, 128, and 256. */
			xAXI.axi_blen[ iIndex ] = 0x04u << iIndex;
		}
		/* AXI Maximum Write OutStanding Request Limit. */
		xAXI.axi_wr_osr_lmt = 1;
		/* This value limits the maximum outstanding request
		 * on the AXI read interface. Maximum outstanding
		 * requests = RD_OSR_LMT+1 */
		xAXI.axi_rd_osr_lmt = 1;
		/* When set to 1, this bit enables the LPI mode (Low Poer Idle) */
		xAXI.axi_lpi_en = 0;

		/*  1 KB Boundary Crossing Enable for the GMAC-AXI
		 * Master When set, the GMAC-AXI Master performs
		 * burst transfers that do not cross 1 KB boundary.
		 * When reset, the GMAC-AXI Master performs burst
		 * transfers that do not cross 4 KB boundary.*/
		xAXI.axi_kbbe = 0;
		/* When set to 1, this bit enables the GMAC-AXI to
		 * come out of the LPI mode only when the Remote
		 * Wake Up Packet is received. When set to 0, this bit
		 * enables the GMAC-AXI to come out of LPI mode
		 * when any frame is received. This bit must be set to 0. */
		xAXI.axi_xit_frm = 0;

		gmac_dma_axi( iMacID, &xAXI );
	}
	{
		struct stmmac_dma_cfg dma_cfg;
		memset( &dma_cfg, '\0', sizeof dma_cfg );

		dma_cfg.txpbl = 8;
		dma_cfg.rxpbl = 1;
		dma_cfg.pblx8 = pdFALSE;
		dma_cfg.fixed_burst = pdFALSE;
		dma_cfg.mixed_burst = pdFALSE;
		/* When this bit is set high and the FB bit is equal to 1,
		the AHB or AXI interface generates all bursts aligned
		to the start address LS bits. If the FB bit is equal to 0,
		the first burst (accessing the data buffer's start
		address) is not aligned, but subsequent bursts are
		aligned to the address */
		dma_cfg.aal = 0;
		gmac_dma_init( iMacID, &dma_cfg );
		gmac_dma_operation_mode( iMacID, SF_DMA_MODE, SF_DMA_MODE );
	}

	ulEMACVersion = readl(ioaddr + GMAC_VERSION);

#define GMAC_EXPECTED_VERSION	0x1037ul
	if( ulEMACVersion != GMAC_EXPECTED_VERSION )
	{
		lUDPLoggingPrintf( "Wrong version : %04lX ( expected %04lX )\n", ulEMACVersion, GMAC_EXPECTED_VERSION );
		return;
	}
}

uint32_t gmac_reg_read( int iMacID, uint32_t ulRegOffset )
{
uint8_t *ioaddr = ucFirstIOAddres( iMacID );
uint32_t ulValue;

	ulValue = readl( ioaddr + ulRegOffset );
	return ulValue;
}

void gmac_reg_write( int iMacID, uint32_t ulRegOffset, uint32_t ulValue )
{
uint8_t *ioaddr = ucFirstIOAddres( iMacID );

	writel( ulValue, ioaddr + ulRegOffset );
}
