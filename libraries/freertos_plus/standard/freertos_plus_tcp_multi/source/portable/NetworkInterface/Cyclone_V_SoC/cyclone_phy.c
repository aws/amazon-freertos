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

#include "UDPLoggingPrintf.h"

#ifndef ARRAY_SIZE
	#define ARRAY_SIZE( x )		( int ) ( sizeof( ( x ) ) / sizeof( ( x )[ 0 ] ) )
#endif

#define PHY_RESET_DELAY			 2000				/* Wait around  2 ms							*/
#define PHY_CONFIG_DELAY		25000				/* Wait around 25 ms							*/

/*-----------------------------------------------------------*/

#define PHY_READ_TO							(  100000)	/* Max wait time (us) when reading from PHY	*/
#define PHY_WRITE_TO						(  100000)	/* Max wait time (us) when reading from PHY	*/
#define PHY_LINKUP_TO						( 5000000)	/* Max wait time (us) for link to come up	*/
#define PHY_AUTON_TO						(15000000)	/* Max wait time (us) for auto-ng to succeed*/

#define PHY_BCR_REG							 0					/* Transceiver Basic Control Regs	*/
 #define PHY_RESET							((uint16_t)0x8000)	/* PHY Reset						*/
 #define PHY_AUTO_NEGOTIATION				((uint16_t)0x1000)	/* Enable auto-negotiation function	*/
 #define PHY_AUTO_NEGOTIATION_RESTART		((uint16_t)0x0200)
 #define PHY_SPEED_1000M					((uint16_t)0x0040)
 #define PHY_SPEED_100M						((uint16_t)0x2000)
 #define PHY_SPEED_10M						((uint16_t)0x0000)
 #define PHY_FULL_DUPLEX					((uint16_t)0x0100)
#define PHY_BSR_REG							 1					/* Transceiver Basic Status Regs	*/
 #define PHY_AUTO_NEGOTIATION_COMPLETE		((uint16_t)0x0020)	/* Auto-Negotiation process done	*/
 #define PHY_LINK_STATUS					((uint16_t)0x0004)	/* Valid link established			*/
#define PHY_ID1_REG							 2			/* PHY Identifier Register					*/
#define PHY_ID2_REG							 3			/* PHY Identifier Register					*/
#define PHY_AUTON_REG						 4			/* PHY AutoNegotiate Register				*/
 #define AUTON_ADV_HALFDUPLEX_10M			(1<< 5)		/* Try for 10mbps half-duplex				*/
 #define AUTON_ADV_FULLDUPLEX_10M			(1<< 6)		/* Try for 10mbps full-duplex				*/
 #define AUTON_ADV_HALFDUPLEX_100M			(1<< 7)		/* Try for 100mbps half-duplex				*/
 #define AUTON_ADV_FULLDUPLEX_100M			(1<< 8)		/* Try for 100mbps full-duplex				*/
 #define AUTOON_PAUSE						(1<<10)
 #define AUTOON_ASYMMETRIC_PAUSE			(1<<11)
 #define AUTOON_AUTONEG_ERROR				(1<<15)
#define PHY_AUTON_PARTN_REG					 5			/* PHY AutoNegotiate Partner Ability Reg	*/
#define PHY_1GCTL_REG						 9			/* PHY 1000T Control Register				*/
 #define PHY_1GCTL_FULL_DUPLEX				(1<<9)
 #define PHY_1GCTL_HALF_DUPLEX				(1<<8)
 #define PHY_1GCTL_ADV_1000					(3<<8)
#define PHY_1GSTS_REG						10			/* PHY 1000T Status Register				*/
 #define _1GSTS_PARTN_FULL_1000M			(1<<11)
 #define _1GSTS_PARTN_HALF_1000M			(1<<10)
#define PHY_EXTCTL_REG						11			/* PHY Extended Control Register			*/
#define PHY_EXTW_REG						12			/* PHY Extended Write Register				*/
#define PHY_EXTR_REG						13			/* PHY Extended Read Register				*/

/*-----------------------------------------------------------*/
#define PHY_ID_KSZ9021						(0x16100022)	/* ((PHY_ID2&0xFFF0) << 16) | PHY_ID1	*/
#define PHY_SR_REG_KSZ9021					(31)			/* Vendor specific auto neg results		*/
#define MII_KSZ9021_EXT_CTRL				0x0B
#define MII_KSZ9021_EXT_DATAW				0x0C
#define MII_KSZ9021_EXT_DATAR				0x0D
#define MII_KSZ9021_PHY_CTL					0x1F
#define MII_KSZ9021_EXT_RGMII_CLOCK_SKEW	0x104
 #ifndef KSZ9021_CLOCK_SKEW
  #define KSZ9021_CLOCK_SKEW				0xA0D0
 #endif
#define MII_KSZ9021_EXT_RGMII_RX_DATA_SKEW	0x105
 #ifndef KSZ9021_RX_SKEW
  #define KSZ9021_RX_SKEW					0
 #endif
#define MII_KSZ9021_EXT_RGMII_TX_DATA_SKEW	0x106
 #ifndef KSZ9021_TX_SKEW
  #define KSZ9021_TX_SKEW					0
 #endif

/*-----------------------------------------------------------*/
#define PHY_ID_KSZ9031						(0x16200022)	/* ((PHY_ID2&0xFFF0) << 16) | PHY_ID1	*/
#define PHY_SR_REG_KSZ9031					(31)			/* Vendor specific auto neg results		*/
#define MII_KSZ9031_EXT_CTRL				0x0D
#define MII_KSZ9031_EXT_DATA				0x0E
#define MII_KSZ9031_EXT_STATUS				0x1F
#define MII_KSZ9031_EXT_RGMII_PAD_SKEW		0x4
 #ifndef KSZ9031_PAD_SKEW
  #define KSZ9031_PAD_SKEW					0
 #endif
#define MII_KSZ9031_EXT_RGMII_CLOCK_SKEW	0x8
 #ifndef KSZ9031_CLOCK_SKEW
  #define KSZ9031_CLOCK_SKEW				0x3FF
 #endif

/*-----------------------------------------------------------*/
#define PHY_ID_PEF7071						(0xA400D565)	/* ((PHY_ID2&0xFFF0) << 16) | PHY_ID1	*/
#define PHY_MIICTRL_REG_PEF7071				(0x17)
 #define PHY_TX_SKEW_MASK_PEF7071			(0x0300)
 #define PHY_TX_SKEW_SHF_PEF7071			(8)
 #ifndef PEF7071_TX_SKEW
  #define PEF7071_TX_SKEW					3
 #endif
 #define PHY_RX_SKEW_MASK_PEF7071			(0x3000)
 #define PHY_RX_SKEW_SHF_PEF7071			(12)
 #ifndef PEF7071_RX_SKEW
  #define PEF7071_RX_SKEW					3
 #endif
#define PHY_SR_REG_PEF7071					(0x18)			/* Vendor specific auto neg results		*/
#define PHY_IMASK_REG_PEF7071				(0x19)

/*-----------------------------------------------------------*/
#define PHY_ID_MARV88E1518					(0x0DD00141)	/* ((PHY_ID2&0xFFF0) << 16) | PHY_ID1	*/
#define PHY_COPPER_REG1_88E1518				16
#define PHY_SS_REG_88E1518					17
#define PHY_COPPER_REG2_88E1518				19
#define PHY_CTRL_MAC_REG_88E1518			21
 #ifndef TXRX_CLOCK_DELAYED_88E1518
  #define TXRX_CLOCK_DELAYED_88E1518		0x0030
 #endif
#define PHY_PAGE_ADDR_REG_88E1518			22

/*-----------------------------------------------------------*/

uint32_t ulUsePHYAddress;

static int gmac_mdio_read_ext(int PHYid, int iMacID, int PHYaddr, int PHYreg);
static int gmac_mdio_write_ext(int PHYid, int iMacID, int PHYaddr, int PHYreg, int PHYval);

struct SPHYType {
	uint32_t ulPHYId;
	const char *pcName;
};

struct SPHYType xPHYTypes[] = {
	{ PHY_ID_MARV88E1518, "Marvell 88E1518 PHY" },
	{ PHY_ID_KSZ9021,     "Micrel KSZ9021 PHY" },
	{ PHY_ID_KSZ9031,     "Micrel KSZ9031/KSZ9071 PHY" },
	{ PHY_ID_PEF7071,     "Lantiq PEF7071 PHY" },
};

int cyclone_phy_init(int iMacID, int Rate)
{
int      ii;
uint32_t MACset;
int iPHYAddress;
uint32_t PHYidHigh, PHYidLow, PHYid;
int      RegVal;
int      RegTmp;
int      Tout;
uint8_t *ioaddr = ucFirstIOAddres( iMacID );

	for( iPHYAddress= 0 ; iPHYAddress < 32 ; iPHYAddress++ )
	{
	int rc;
		/* Find the PHY address using the part ID		*/
		rc = gmac_mdio_read( iMacID, iPHYAddress, PHY_ID1_REG );
		if( rc < 0 )
		{
			FreeRTOS_printf( ( "PHY : gmac_mdio_read failed\n" ) );
			return -1;
		}
		PHYidLow = ( uint32_t )rc;
		if( ( PHYidLow == 0xfffful ) || ( PHYidLow == 0x0000ul ) )
		{
			continue;
		}
		PHYidHigh = gmac_mdio_read( iMacID, iPHYAddress, PHY_ID2_REG );

		PHYid =   PHYidLow | ((PHYidHigh & 0xFFF0) << 16);

		FreeRTOS_printf( ( "PHY : Checking address %2d (ID: 0x%08X)\n", iPHYAddress, (unsigned int)PHYid ) );
		for( ii = 0; ii < ARRAY_SIZE( xPHYTypes ); ii++ )
		{
			if( xPHYTypes[ ii ].ulPHYId == PHYid )
			{
				FreeRTOS_printf( ( "PHY : Found %s at address %d\n", xPHYTypes[ ii ].pcName, iPHYAddress ) );
				break;
			}
		}
		if( ii < ARRAY_SIZE( xPHYTypes ) )
		{
			/* A PHY has been found. */
			break;
		}
	}

	if( iPHYAddress >= 32 )							/* If did not find a recognized part, error		*/
	{
		FreeRTOS_printf( ( "PHY : No recognized PHY found\n" ) );
		return -1;
	}
	ulUsePHYAddress = iPHYAddress;
													/* Marvell validates 1000BASE-T settings after	*/
	if( Rate >= 1000 )								/* a reset only. Do this for all PHY			*/
	{
		RegVal  = (Rate & 1)
		        ? PHY_1GCTL_HALF_DUPLEX
		        : PHY_1GCTL_FULL_DUPLEX;
		gmac_mdio_write( iMacID, iPHYAddress, PHY_1GCTL_REG, RegVal);
	}
													/* Reset the PHY								*/
	gmac_mdio_write( iMacID, iPHYAddress, PHY_BCR_REG, PHY_RESET);

	for( ii=32; ii>0; ii-- )							/* Wait for the reset to terminate				*/
	{
		if( ( gmac_mdio_read( iMacID, iPHYAddress, PHY_BCR_REG ) & PHY_RESET ) == 0 )
		{
			break;
		}
		vTaskDelay( 2 );
	}
	if( ii <= 0 )
	{									/* Got a problem as the PHY never terminates	*/
		FreeRTOS_printf( ( "PHY : Reset timeout\n" ) );
		while(1);
	}

	switch( PHYid ) /* Configure the PHY							*/
	{
	case PHY_ID_KSZ9021:
		gmac_mdio_write_ext(PHYid, iMacID, iPHYAddress, MII_KSZ9021_EXT_RGMII_RX_DATA_SKEW, KSZ9021_RX_SKEW);
		gmac_mdio_write_ext(PHYid, iMacID, iPHYAddress, MII_KSZ9021_EXT_RGMII_TX_DATA_SKEW, KSZ9021_TX_SKEW);
		gmac_mdio_write_ext(PHYid, iMacID, iPHYAddress, MII_KSZ9021_EXT_RGMII_CLOCK_SKEW, KSZ9021_CLOCK_SKEW);

		gmac_mdio_read_ext(PHYid, iMacID, iPHYAddress, MII_KSZ9021_EXT_RGMII_RX_DATA_SKEW);
		gmac_mdio_read_ext(PHYid, iMacID, iPHYAddress, MII_KSZ9021_EXT_RGMII_TX_DATA_SKEW);
		gmac_mdio_read_ext(PHYid, iMacID, iPHYAddress, MII_KSZ9021_EXT_RGMII_CLOCK_SKEW);
		break;
	case PHY_ID_KSZ9031:
		gmac_mdio_write_ext(PHYid, iMacID, iPHYAddress, MII_KSZ9031_EXT_RGMII_PAD_SKEW, KSZ9031_PAD_SKEW);
		gmac_mdio_write_ext(PHYid, iMacID, iPHYAddress, MII_KSZ9031_EXT_RGMII_CLOCK_SKEW, KSZ9031_CLOCK_SKEW);

		gmac_mdio_read_ext(PHYid, iMacID, iPHYAddress, MII_KSZ9031_EXT_RGMII_PAD_SKEW);
		gmac_mdio_read_ext(PHYid, iMacID, iPHYAddress, MII_KSZ9031_EXT_RGMII_CLOCK_SKEW);
		break;
	case PHY_ID_PEF7071:
		gmac_mdio_write( iMacID, iPHYAddress, PHY_IMASK_REG_PEF7071, 0);
		gmac_mdio_read( iMacID, iPHYAddress, PHY_IMASK_REG_PEF7071);
		RegVal = gmac_mdio_read( iMacID, iPHYAddress, PHY_1GCTL_REG);
		RegVal |= 1<<10;
		gmac_mdio_write( iMacID, iPHYAddress, PHY_1GCTL_REG, RegVal);

		RegVal = gmac_mdio_read( iMacID, iPHYAddress, PHY_MIICTRL_REG_PEF7071);
		RegVal &= (~(PHY_RX_SKEW_MASK_PEF7071 | PHY_TX_SKEW_MASK_PEF7071));
		RegVal |= ((PEF7071_RX_SKEW)<<(PHY_RX_SKEW_SHF_PEF7071))
			   |  ((PEF7071_TX_SKEW)<<(PHY_TX_SKEW_SHF_PEF7071));
		gmac_mdio_write( iMacID, iPHYAddress, PHY_MIICTRL_REG_PEF7071, RegVal);
		break;
	case PHY_ID_MARV88E1518:
		gmac_mdio_write( iMacID, iPHYAddress, PHY_PAGE_ADDR_REG_88E1518, 2);
		RegVal = gmac_mdio_read( iMacID, iPHYAddress, PHY_CTRL_MAC_REG_88E1518);
		RegVal |= (TXRX_CLOCK_DELAYED_88E1518);
		gmac_mdio_write( iMacID, iPHYAddress, PHY_CTRL_MAC_REG_88E1518, RegVal);
		gmac_mdio_write( iMacID, iPHYAddress, PHY_PAGE_ADDR_REG_88E1518, 0);
		break;
	}
									/* Prepare to configure the MAC */
	MACset =  GMAC_CONTROL_IPC		/* Checksum Offload */
			| GMAC_CONTROL_JD		/* Jabber Disable */
			| GMAC_CONTROL_PS		/* Port Select = MII */
			| GMAC_CONTROL_BE		/* Frame Burst Enable */
			| GMAC_CONTROL_DCRS		/* Disable carrier sense ( half-duplex option ) */
			| GMAC_CONTROL_ACS;		/* Enable Automatic Pad CRC Stripping */
//			| GMAC_CONTROL_WD;		/* Watchdog Disable */

	if( Rate >= 0 )								/* Request to set the Ethernet speed			*/
	{
		if( Rate < 1000 )							/* 10 & 100 Mbps, disable all 1000 Mbps stuff	*/
		{
			gmac_mdio_write( iMacID, iPHYAddress, PHY_1GCTL_REG, 0);
		}
		else										/* 1000 Mbps set duplexing mode					*/
		{
			RegVal = (Rate & 1)
			        ? PHY_1GCTL_HALF_DUPLEX
			        : PHY_1GCTL_FULL_DUPLEX;
			gmac_mdio_write( iMacID, iPHYAddress, PHY_1GCTL_REG, RegVal);
		}

		FreeRTOS_printf( ( "PHY : Configuring link at %d Mbps %s duplex\n", Rate & ~1,
			                                                   (Rate & 1) ? "half" : "full" ) );

		RegVal  = gmac_mdio_read( iMacID, iPHYAddress, PHY_BCR_REG);
		RegVal &= ~(PHY_SPEED_1000M					/* Clear all bits related to speed / duplex		*/
		          | PHY_SPEED_100M
		          | PHY_SPEED_10M
		          | PHY_FULL_DUPLEX
		          | PHY_AUTO_NEGOTIATION);

		if( Rate >= 1000 )							/* Set the speed bits							*/
		{
			RegVal |= PHY_SPEED_1000M;
		}
		else if( Rate >= 100 )
		{
			RegVal |= PHY_SPEED_100M;
		}
		else
		{
			RegVal |= PHY_SPEED_10M;
		}

		if( ( Rate & 1 ) == 0 )						/* Set half /  full duplex bit					*/
		{
			RegVal |= PHY_FULL_DUPLEX;
		}

		gmac_mdio_write( iMacID, iPHYAddress, PHY_BCR_REG, RegVal);

		for( Tout=PHY_LINKUP_TO/100 ; Tout>0 ; Tout-- )
		{
			if( ( gmac_mdio_read( iMacID, iPHYAddress, PHY_BSR_REG) & PHY_LINK_STATUS ) != 0 )
			{
				break;
			}
			/* Wait at least 100 uS. */
			vTaskDelay( 2 );
		}
		if( Tout <= 0 )
		{
			FreeRTOS_printf( ( "PHY : Timeout setting link speed\n" ) );
			return -1;
		}
		FreeRTOS_printf( ( "PHY : Link ready\n" ) );
	}
	else {											/* Rate not specified, auto-negotiation			*/
		RegVal  = gmac_mdio_read( iMacID, iPHYAddress, PHY_AUTON_REG);
		RegVal |= 0x01E1;							/* Advertise 10 / 100 & 1000 mbps				*/
		gmac_mdio_write( iMacID, iPHYAddress, PHY_AUTON_REG, RegVal);

		RegVal  = gmac_mdio_read( iMacID, iPHYAddress, PHY_1GCTL_REG);
		RegVal |= PHY_1GCTL_ADV_1000;
		gmac_mdio_write( iMacID, iPHYAddress, PHY_1GCTL_REG, RegVal);		
	  #if 0											/* This does not seem to be needed				*/
		if (PHYid == PHY_ID_MARV88E1518)
		{
			gmac_mdio_write( iMacID, iPHYAddress, PHY_PAGE_ADDR_REG_88E1518, 0);
			RegVal = gmac_mdio_read( iMacID, iPHYAddress, PHY_COPPER_REG1_88E1518);
			RegVal |= (7 << 12)						/* Max number of tries @ 1Gigabit				*/
			       |  (1 << 11);					/* Allow downspeeding							*/
			gmac_mdio_write( iMacID, iPHYAddress, PHY_COPPER_REG1_88E1518, RegVal);				
		}
	  #endif
		FreeRTOS_printf( ( "PHY : Starting auto-negotiation\n" ) );

		RegVal = gmac_mdio_read( iMacID, iPHYAddress, PHY_BCR_REG);
		RegVal |= PHY_AUTO_NEGOTIATION
		       |  PHY_AUTO_NEGOTIATION_RESTART;
		gmac_mdio_write( iMacID, iPHYAddress, PHY_BCR_REG, RegVal);

		for (Tout=PHY_LINKUP_TO/100 ; Tout>0 ; Tout--) {
			if (gmac_mdio_read( iMacID, iPHYAddress, PHY_BSR_REG) & PHY_LINK_STATUS) {
				break;								/* Wait for the link to be up					*/
			}
			/* Wait at least 100 uS. */
			vTaskDelay( 2 );
		}
		if( Tout <= 0 )
		{
			FreeRTOS_printf( ( "PHY : Auto-negotiation start timeout\n" ) );
			return -1;
		}
		FreeRTOS_printf( ( "PHY : Link is up\n" ) );
													/* Enable Auto-Negotiation						*/
		if( gmac_mdio_write( iMacID, iPHYAddress, PHY_BCR_REG, PHY_AUTO_NEGOTIATION | PHY_AUTO_NEGOTIATION_RESTART) < 0 )
		{
			return -1;
		}
		FreeRTOS_printf( ( "PHY : Enabling auto-negotiation\n" ) );

		for( Tout=PHY_AUTON_TO/10 ; Tout>0 ; Tout-- )
		{
			if (gmac_mdio_read( iMacID, iPHYAddress, PHY_BSR_REG) & PHY_AUTO_NEGOTIATION_COMPLETE) {
				break;								/* Wait for auto-negotiation to complete		*/
			}
			/* Wait 10 uS. */
			vTaskDelay( 2 );
		}
		if( Tout <= 0 )
		{
			FreeRTOS_printf( ( "PHY : Auto-negotiation timeout\n" ) );
			return -1;
		}

		FreeRTOS_printf( ( "PHY : Link Partner Capabilities :\n" ) );
		RegVal = gmac_mdio_read( iMacID, iPHYAddress, PHY_1GSTS_REG);
		FreeRTOS_printf( ( "PHY :       1000 Mbps Full Duplex (%s)\n", (RegVal & _1GSTS_PARTN_FULL_1000M) ? "YES" : "NO" ) );
		FreeRTOS_printf( ( "PHY :       1000 Mbps Half Duplex (%s)\n", (RegVal & _1GSTS_PARTN_HALF_1000M) ? "YES" : "NO" ) );
		RegVal = gmac_mdio_read( iMacID, iPHYAddress, PHY_AUTON_PARTN_REG);	
		FreeRTOS_printf( ( "PHY :        100 Mbps Full Duplex (%s)\n", (RegVal & AUTON_ADV_FULLDUPLEX_100M) ? "YES" : "NO" ) );
		FreeRTOS_printf( ( "PHY :        100 Mbps Half Duplex (%s)\n", (RegVal & AUTON_ADV_HALFDUPLEX_100M) ? "YES" : "NO" ) );
		FreeRTOS_printf( ( "PHY :         10 Mbps Full Duplex (%s)\n", (RegVal & AUTON_ADV_FULLDUPLEX_10M) ? "YES" : "NO" ) );
		FreeRTOS_printf( ( "PHY :         10 Mbps Half Duplex (%s)\n", (RegVal & AUTON_ADV_HALFDUPLEX_10M) ? "YES" : "NO" ) );
	}

	Rate = 10;
	switch( PHYid )
	{
	case PHY_ID_KSZ9021:					/* Extract from the PHY the rate it is at  		*/
	case PHY_ID_KSZ9031:					/* Same register for 9021 & 9031				*/
		RegVal = gmac_mdio_read( iMacID, iPHYAddress, PHY_SR_REG_KSZ9021 );
		RegTmp = ( RegVal >> 4 ) & 7;
		if( RegTmp & ( 1 << 2 ) )
		{
			Rate = 1000;
		}
		else if( RegTmp & ( 1 << 1 ) )
		{
			Rate = 100;
		} 
		if( ( RegVal & ( 1 << 3 ) ) == 0 )			/* If Full Duplex								*/
		{
			Rate |= 1; 
		}
		break;
	case PHY_ID_PEF7071:
		RegVal = gmac_mdio_read( iMacID, iPHYAddress, PHY_SR_REG_PEF7071);
		RegTmp = RegVal & 3;
		if( RegTmp == 2 )
		{
			Rate = 1000;
		}
		else if( RegTmp == 1 )
		{
			Rate = 100;
		} 
		if( ( RegVal & ( 1 << 3 ) ) == 0 )			/* If Full Duplex								*/
		{
			Rate |= 1;
		}
		break;
	case PHY_ID_MARV88E1518:
		RegVal = gmac_mdio_read( iMacID, iPHYAddress, PHY_SS_REG_88E1518 );
		RegTmp = ( RegVal >> 14 ) & 3;
		if( RegTmp == 2 )
		{
			Rate = 1000;
		}
		else if( RegTmp == 1 )
		{
			Rate = 100;
		}
		if( ( RegVal & (1<<13)) == 0 )				/* If Full Duplex								*/
		{
			Rate |= 1;
		}
		break;
	}

	FreeRTOS_printf( ( "PHY : Link is up at %d Mbps %s duplex\n", Rate & ~1, (Rate & 1) ? "half" : "full" ) );

													/* Read SMII status to clear changed bit		*/
	if( Rate >= 1000 )								/* Set MAC requested speed & duplexing info		*/
	{
		MACset &= ~GMAC_CONTROL_PS;
	}
	else if( Rate >= 100 )
	{
		MACset |= GMAC_CONTROL_FES;
	}
	if( ( Rate & 1 ) == 0 )
	{
		MACset |= GMAC_CONTROL_DM;
	}

	/* Just read the PHY Control Status register. */
	if( readl( ioaddr + GMAC_RGSMIIIS ) ) {}

	/* Disable MAC interrupts						*/
	gmac_set_emac_interrupt_disable( iMacID, GMAC_INT_DISABLE_TIMESTAMP | GMAC_INT_DISABLE_LPI );
	writel( MACset, ioaddr + GMAC_CONTROL );					/* Set the MAC Configuration Register			*/

	writel( DMA_BUS_MODE_ATDS	/* Alternate Desc Size. */
			| DMA_BUS_MODE_MAXPBL
			| ( ( 8 << DMA_BUS_MODE_PBL_SHIFT )  & DMA_BUS_MODE_PBL_MASK )
			| ( ( 1 << DMA_BUS_MODE_RPBL_SHIFT ) & DMA_BUS_MODE_RPBL_MASK ),
			ioaddr + DMA_BUS_MODE );

	/* Set the DMA Operation Mode Register			*/
	writel( DMA_CONTROL_OSF			/* Operate on 2nd Frame					*/
	        | DMA_CONTROL_TSF		/* TX Store and Forward					*/
	        | DMA_CONTROL_RSF,		/* RX Store and Forward					*/
			ioaddr + DMA_CONTROL );	/* Operational mode. */

	writel( ( 1 << DMA_AXI_WR_OSR_LMT_SHIFT ) |
			( 1 << DMA_AXI_RD_OSR_LMT_SHIFT ),
			ioaddr + DMA_AXI_BUS_MODE );

	return Rate;
}
/*-----------------------------------------------------------*/

/* Read a PHY extended register																		*/

static int gmac_mdio_read_ext(int PHYid, int iMacID, int PHYaddr, int PHYreg)
{
	switch( PHYid )
	{
	case PHY_ID_KSZ9021:
		gmac_mdio_write(iMacID, PHYaddr, MII_KSZ9021_EXT_CTRL, PHYreg);
		return gmac_mdio_read( iMacID, PHYaddr, MII_KSZ9021_EXT_DATAR );
	case PHY_ID_KSZ9031:
		gmac_mdio_write(iMacID, PHYaddr, MII_KSZ9031_EXT_CTRL, 2);
		gmac_mdio_write(iMacID, PHYaddr, MII_KSZ9031_EXT_DATA, PHYreg);
		gmac_mdio_write(iMacID, PHYaddr, MII_KSZ9031_EXT_CTRL, 0x8002);
		return gmac_mdio_read(iMacID, PHYaddr, MII_KSZ9031_EXT_DATA );
	default:
		break;
	}
	return 0;
}
/*-----------------------------------------------------------*/

/* Write to a PHY extended register. */

static int gmac_mdio_write_ext(int PHYid, int iMacID, int PHYaddr, int PHYreg, int PHYval)
{
int RetVal; 

	RetVal = 0;

	if (PHYid == PHY_ID_KSZ9021) {
		RetVal  = gmac_mdio_write(iMacID, PHYaddr, MII_KSZ9021_EXT_CTRL, 0x8000 | PHYreg);
		RetVal |= gmac_mdio_write(iMacID, PHYaddr, MII_KSZ9021_EXT_DATAW, PHYval);
	}
	else if (PHYid == PHY_ID_KSZ9031) {
		RetVal  = gmac_mdio_write(iMacID, PHYaddr, MII_KSZ9031_EXT_CTRL, 2);
		RetVal |= gmac_mdio_write(iMacID, PHYaddr, MII_KSZ9031_EXT_DATA, PHYreg);
		RetVal |= gmac_mdio_write(iMacID, PHYaddr, MII_KSZ9031_EXT_CTRL, 0x4002);
		RetVal |= gmac_mdio_write(iMacID, PHYaddr, MII_KSZ9031_EXT_DATA, PHYval);
	}
    return(RetVal);
}
/*-----------------------------------------------------------*/
