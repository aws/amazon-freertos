
#ifndef CYCLONE_EMAC_H

#define CYCLONE_EMAC_H

#define SPEED_10		10
#define SPEED_100		100
#define SPEED_1000		1000

#define EMAC_ID_0_ADDRESS	0xFF700000u
#define EMAC_ID_1_ADDRESS	0xFF702000u

#define BITS_PER_LONG				32
#define BIT( n )					( 1u << ( n ) )
#define GENMASK(h, l)				( ( ( ~0UL ) << ( l ) ) & ( ~0UL >> ( BITS_PER_LONG - 1 - ( h ) ) ) )

/* SGMII/RGMII status register */
#define GMAC_RGSMIIIS_LNKMODE		BIT(0)
#define GMAC_RGSMIIIS_SPEED			GENMASK(2, 1)
#define GMAC_RGSMIIIS_SPEED_SHIFT	1
#define GMAC_RGSMIIIS_LNKSTS		BIT(3)
#define GMAC_RGSMIIIS_JABTO			BIT(4)
#define GMAC_RGSMIIIS_FALSECARDET	BIT(5)
#define GMAC_RGSMIIIS_SMIDRXS		BIT(16)
/* LNKMOD */
#define GMAC_RGSMIIIS_LNKMOD_MASK	0x1
/* LNKSPEED */
#define GMAC_RGSMIIIS_SPEED_125		0x2
#define GMAC_RGSMIIIS_SPEED_25		0x1
#define GMAC_RGSMIIIS_SPEED_2_5		0x0

#define GMAC_CONTROL		0x00000000	/* Configuration */
#define GMAC_FRAME_FILTER	0x00000004	/* Frame Filter */
#define GMAC_HASH_HIGH		0x00000008	/* Multicast Hash Table High */
#define GMAC_HASH_LOW		0x0000000c	/* Multicast Hash Table Low */
#define GMAC_MII_ADDR		0x00000010	/* MII Address */
#define GMAC_MII_DATA		0x00000014	/* MII Data */
#define GMAC_FLOW_CTRL		0x00000018	/* Flow Control */
#define GMAC_VLAN_TAG		0x0000001c	/* VLAN Tag */
#define GMAC_VERSION		0x00000020	/* GMAC CORE Version */
#define GMAC_DEBUG			0x00000024	/* GMAC debug register */
#define GMAC_WAKEUP_FILTER	0x00000028	/* Wake-up Frame Filter */
#define GMAC_PCS_BASE		0x000000c0	/* PCS register base */
#define GMAC_RGSMIIIS		0x000000d8	/* RGMII/SMII status */

#define GMAC_INT_STATUS			0x00000038	/* interrupt status register */
#define GMAC_INT_STATUS_PMT		BIT(3)
#define GMAC_INT_STATUS_MMCIS	BIT(4)
#define GMAC_INT_STATUS_MMCRIS	BIT(5)
#define GMAC_INT_STATUS_MMCTIS	BIT(6)
#define GMAC_INT_STATUS_MMCCSUM	BIT(7)
#define GMAC_INT_STATUS_TSTAMP	BIT(9)
#define GMAC_INT_STATUS_LPIIS	BIT(10)

#define GMAC_CORE_INIT (GMAC_CONTROL_JD | GMAC_CONTROL_PS | GMAC_CONTROL_ACS | \
			GMAC_CONTROL_BE | GMAC_CONTROL_DCRS)

#define GMAC_MMC_CTRL              0x100
#define GMAC_MMC_RX_INTR           0x104
#define GMAC_MMC_TX_INTR           0x108
#define GMAC_MMC_RX_INTR_MASK      0x10C
#define GMAC_MMC_TX_INTR_MASK      0x110

#define GMAC_MMC_RX_CSUM_OFFLOAD   0x208

/* GMAC Configuration defines */
#define GMAC_CONTROL_2K 0x08000000	/* IEEE 802.3as 2K packets */
#define GMAC_CONTROL_TC	0x01000000	/* Transmit Conf. in RGMII/SGMII */
#define GMAC_CONTROL_WD	0x00800000	/* Disable Watchdog on receive */
#define GMAC_CONTROL_JD	0x00400000	/* Jabber disable */
#define GMAC_CONTROL_BE	0x00200000	/* Frame Burst Enable */
#define GMAC_CONTROL_JE	0x00100000	/* Jumbo frame */
enum inter_frame_gap {
	GMAC_CONTROL_IFG_88 = 0x00040000,
	GMAC_CONTROL_IFG_80 = 0x00020000,
	GMAC_CONTROL_IFG_40 = 0x000e0000,
};
#define GMAC_CONTROL_DCRS	0x00010000	/* Disable carrier sense */
#define GMAC_CONTROL_PS		0x00008000	/* Port Select 0:GMI 1:MII */
#define GMAC_CONTROL_FES	0x00004000	/* Speed 0:10 1:100 */
#define GMAC_CONTROL_DO		0x00002000	/* Disable Rx Own */
#define GMAC_CONTROL_LM		0x00001000	/* Loop-back mode */
#define GMAC_CONTROL_DM		0x00000800	/* Duplex Mode */
#define GMAC_CONTROL_IPC	0x00000400	/* Checksum Offload */
#define GMAC_CONTROL_DR		0x00000200	/* Disable Retry */
#define GMAC_CONTROL_LUD	0x00000100	/* Link up/down */
#define GMAC_CONTROL_ACS	0x00000080	/* Auto Pad/FCS Stripping */
#define GMAC_CONTROL_DC		0x00000010	/* Deferral Check */
#define GMAC_CONTROL_TE		0x00000008	/* Transmitter Enable */
#define GMAC_CONTROL_RE		0x00000004	/* Receiver Enable */

/* interrupt mask register */
#define	GMAC_INT_MASK				0x0000003c
#define	GMAC_INT_DISABLE_RGMII		BIT(0)
#define	GMAC_INT_DISABLE_PCSLINK	BIT(1)
#define	GMAC_INT_DISABLE_PCSAN		BIT(2)
#define	GMAC_INT_DISABLE_PMT		BIT(3)
#define	GMAC_INT_DISABLE_TIMESTAMP	BIT(9)
#define	GMAC_INT_DISABLE_LPI		BIT(10)
#define	GMAC_INT_DISABLE_PCS		( GMAC_INT_DISABLE_RGMII | \
									  GMAC_INT_DISABLE_PCSLINK | \
									  GMAC_INT_DISABLE_PCSAN )
#define	GMAC_INT_DEFAULT_MASK		( GMAC_INT_DISABLE_TIMESTAMP | GMAC_INT_DISABLE_PCS )

/* GMAC TX FIFO is 8K, Rx FIFO is 16K */
#define BUF_SIZE_16KiB 16384
#define BUF_SIZE_8KiB 8192
#define BUF_SIZE_4KiB 4096
#define BUF_SIZE_2KiB 2048

/* Power Down and WOL */
#define PMT_NOT_SUPPORTED 0
#define PMT_SUPPORTED 1

/* Common MAC defines */
#define MAC_CTRL_REG		0x00000000	/* MAC Control */
#define MAC_ENABLE_TX		0x00000008	/* Transmitter Enable */
#define MAC_ENABLE_RX		0x00000004	/* Receiver Enable */

/* Default LPI timers */
#define STMMAC_DEFAULT_LIT_LS	0x3E8
#define STMMAC_DEFAULT_TWT_LS	0x1E

#define STMMAC_CHAIN_MODE	0x1
#define STMMAC_RING_MODE	0x2

#define JUMBO_LEN		9000

/* Energy Efficient Ethernet (EEE)
 *
 * LPI status, timer and control register offset
 */
#define LPI_CTRL_STATUS	0x0030
#define LPI_TIMER_CTRL	0x0034

/* LPI control and status defines */
#define LPI_CTRL_STATUS_LPITXA	0x00080000	/* Enable LPI TX Automate */
#define LPI_CTRL_STATUS_PLSEN	0x00040000	/* Enable PHY Link Status */
#define LPI_CTRL_STATUS_PLS		0x00020000	/* PHY Link Status */
#define LPI_CTRL_STATUS_LPIEN	0x00010000	/* LPI Enable */
#define LPI_CTRL_STATUS_RLPIST	0x00000200	/* Receive LPI state */
#define LPI_CTRL_STATUS_TLPIST	0x00000100	/* Transmit LPI state */
#define LPI_CTRL_STATUS_RLPIEX	0x00000008	/* Receive LPI Exit */
#define LPI_CTRL_STATUS_RLPIEN	0x00000004	/* Receive LPI Entry */
#define LPI_CTRL_STATUS_TLPIEX	0x00000002	/* Transmit LPI Exit */
#define LPI_CTRL_STATUS_TLPIEN	0x00000001	/* Transmit LPI Entry */

/* MAC HW ADDR regs */
#define GMAC_HI_DCS				GENMASK(18, 16)
#define GMAC_HI_DCS_SHIFT		16
#define GMAC_HI_REG_AE			BIT(31)

/* GMAC HW ADDR regs */
#define GMAC_ADDR_HIGH(reg)	(((reg > 15) ? 0x00000800 : 0x00000040) + \
				(reg * 8))
#define GMAC_ADDR_LOW(reg)	(((reg > 15) ? 0x00000804 : 0x00000044) + \
				(reg * 8))
#define GMAC_MAX_PERFECT_ADDRESSES	1

/* PCS registers (AN/TBI/SGMII/RGMII) offsets */
#define GMAC_AN_CTRL(x)		(x)		/* AN control */
#define GMAC_AN_STATUS(x)	(x + 0x4)	/* AN status */
#define GMAC_ANE_ADV(x)		(x + 0x8)	/* ANE Advertisement */
#define GMAC_ANE_LPA(x)		(x + 0xc)	/* ANE link partener ability */
#define GMAC_ANE_EXP(x)		(x + 0x10)	/* ANE expansion */
#define GMAC_TBI(x)			(x + 0x14)	/* TBI extend status */

/* AN Configuration defines */
#define GMAC_AN_CTRL_RAN	BIT(9)	/* Restart Auto-Negotiation */
#define GMAC_AN_CTRL_ANE	BIT(12)	/* Auto-Negotiation Enable */
#define GMAC_AN_CTRL_ELE	BIT(14)	/* External Loopback Enable */
#define GMAC_AN_CTRL_ECD	BIT(16)	/* Enable Comma Detect */
#define GMAC_AN_CTRL_LR		BIT(17)	/* Lock to Reference */
#define GMAC_AN_CTRL_SGMRAL	BIT(18)	/* SGMII RAL Control */

/* AN Status defines */
#define GMAC_AN_STATUS_LS	BIT(2)	/* Link Status 0:down 1:up */
#define GMAC_AN_STATUS_ANA	BIT(3)	/* Auto-Negotiation Ability */
#define GMAC_AN_STATUS_ANC	BIT(5)	/* Auto-Negotiation Complete */
#define GMAC_AN_STATUS_ES	BIT(8)	/* Extended Status */

/* ADV and LPA defines */
#define GMAC_ANE_FD			BIT(5)
#define GMAC_ANE_HD			BIT(6)
#define GMAC_ANE_PSE		GENMASK(8, 7)
#define GMAC_ANE_PSE_SHIFT	7
#define GMAC_ANE_RFE		GENMASK(13, 12)
#define GMAC_ANE_RFE_SHIFT	12
#define GMAC_ANE_ACK		BIT(14)

struct xEMACStats
{
	uint32_t irq_rgmii_n;
	uint32_t pcs_speed;
	uint32_t pcs_duplex;
	uint32_t pcs_link;
};

typedef struct xEMACStats EMACStats_t;

struct xEMAC_config {
	unsigned
		prelen          : 2,			/*  0  prelen */
		rx_enable       : 1,			/*  2  re  */
		tx_enable       : 1,			/*  3  te  */
		deferral_check  : 1,			/*  4  dc  */
		back_off_limit  : 2,			/*  5  bl  */
		auto_crc_strip  : 1,			/*  7  acs */
		link_up_down    : 1,			/*  8  lud */
		single_transmis : 1,			/*  9  dr  */
		ip_checksum     : 1,			/* 10  ipc */
		full_duplex_mode: 1,			/* 11  dm  */
		loop_back_mode  : 1,			/* 12  lm  */
		disable_rx      : 1,			/* 13  do  */
		link_speed      : 1,			/* 14  fes */
		use_MII         : 1,			/* 15  ps  */
		dcrss           : 1,			/* 16  dcrss */
		inter_frame_gap : 3,			/* 17  */
		ignore_jumbo_err: 1,			/* 20  je  */
		burts_enable    : 1,			/* 21  be  */
		jabber_timer_dis: 1,			/* 22  jd  */
		disable_watchdog: 1,			/* 23  wd  */
		trans_dupl_mode : 1,			/* 24  tc  */
		strip_eth_frames: 1,			/* 25  cst */
		reserved_1      : 1,			/* 26  */
		twokpe          : 1,			/* 27  */
		reserved_2      : 4;			/* 28  */
};

typedef struct xEMAC_config EMAC_config_t;

struct xEMAC_interrupt {
	uint32_t
		rgsmiiis : 1,	/*  0  Link change */
		pcslchgis : 1,	/*  1  AN status */
		pcsancis : 1,	/*  2  AN complete */
		resrv_1 : 1,	/*  3  Reserved. */
		mmcis : 1,		/*  4  set high when any of the Bits [7:5] is set high. */
		mmcrxis : 1,	/*  5  MMC Receive Interrupt Status. */
		mmctxis : 1,	/*  6  MMC Transmit Interrupt. */
		mmcrxipis : 1,	/*  7  MMC Receive Checksum Offload Interrupt. */
		resrv_2 : 1,	/*  8  Reserved. */
		tsis : 1,		/*  9  Timestamp Interrupt Status */
		lpiis : 1,		/* 10  LPI Interrupt Status */
		resrv_3 : 21;	/* 11  Reserved. */
};

typedef struct xEMAC_interrupt EMAC_interrupt_t;

#define SYSMGR_EMACGRP_CTRL_PHYSEL_ENUM_GMII_MII 0x0
#define SYSMGR_EMACGRP_CTRL_PHYSEL_ENUM_RGMII 0x1
#define SYSMGR_EMACGRP_CTRL_PHYSEL_ENUM_RMII 0x2
#define SYSMGR_EMACGRP_CTRL_PHYSEL_WIDTH 2
#define SYSMGR_EMACGRP_CTRL_PHYSEL_MASK 0x00000003
#define SYSMGR_EMACGRP_CTRL_PTP_REF_CLK_MASK 0x00000010

struct xSYSMGR_EMACGRP {
	uint32_t
		physel_0 : 2,
		physel_1 : 2,
		ptpclksel_0 : 1,
		ptpclksel_1 : 1,
		reserved : 26;
};

typedef struct xSYSMGR_EMACGRP SYSMGR_EMACGRP_t;

void gmac_init( int iMacID );
void gmac_set_MAC_address(int iMacID, const uint8_t *ucMACAddress, uint32_t ulIndex );

/* Enable disable MAC RX/TX */
void gmac_enable_transmission(int iMacID, bool enable);

void gmac_set_emac_interrupt_disable( int iMacID, uint32_t ulMask );
void gmac_clear_emac_interrupt_status( int iMacID, uint32_t ulMask );
uint32_t gmac_get_emac_interrupt_status( int iMacID, int iClear );

static __inline uint8_t *ucFirstIOAddres(int iMacID)
{
uint8_t *ucReturn;

	if( iMacID == 0 )
	{
		ucReturn = ( uint8_t * )EMAC_ID_0_ADDRESS;
	}
	else if( iMacID == 1 )
	{
		ucReturn = ( uint8_t * )EMAC_ID_1_ADDRESS;
	}
	else
	{
		ucReturn = ( uint8_t * )NULL;
	}
	return ucReturn;
}

static __inline uint32_t readl( uint8_t *pucAddress )
{
	return *( ( volatile uint32_t *) pucAddress );
}

static __inline void writel( uint32_t ulValue, uint8_t *pucAddress )
{
	*( ( volatile uint32_t *) pucAddress ) = ( volatile uint32_t )ulValue;
}

int gmac_mdio_read( int iMacID, int phyaddr, int phyreg);

int gmac_mdio_write( int iMacID, int phyaddr, int phyreg, uint16_t phydata );

uint32_t Phy_Setup( EMACInterface_t *pxEMACif );

uint32_t gmac_reg_read( int iMacID, uint32_t ulRegOffset );
void gmac_reg_write( int iMacID, uint32_t ulRegOffset, uint32_t ulValue );

#endif /* CYCLONE_EMAC_H */
