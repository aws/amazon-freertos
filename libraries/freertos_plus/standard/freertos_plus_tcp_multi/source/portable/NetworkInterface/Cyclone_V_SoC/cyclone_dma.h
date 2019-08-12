#ifndef CYCLONE_DMA_H

#define CYCLONE_DMA_H

/* DMA CRS Control and Status Register Mapping */
#define DMA_BUS_MODE		0x00001000	/* Bus Mode */
#define DMA_XMT_POLL_DEMAND	0x00001004	/* Transmit Poll Demand */
#define DMA_RCV_POLL_DEMAND	0x00001008	/* Received Poll Demand */
#define DMA_RCV_BASE_ADDR	0x0000100c	/* Receive List Base */
#define DMA_TX_BASE_ADDR	0x00001010	/* Transmit List Base */
#define DMA_STATUS			0x00001014	/* Status Register */
#define DMA_CONTROL			0x00001018	/* Ctrl (Operational Mode) */
#define DMA_INTR_ENA		0x0000101c	/* Interrupt Enable */
#define DMA_MISSED_FRAME_CTR	0x00001020	/* Missed Frame Counter */
#define DMA_AXI_BUS_MODE	0x00001028

/*--- DMA BLOCK defines ---*/
/* DMA Bus Mode register defines */
#define DMA_BUS_MODE_DA		0x00000002	/* Arbitration scheme */
#define DMA_BUS_MODE_DSL_MASK	0x0000007c	/* Descriptor Skip Length */
#define DMA_BUS_MODE_DSL_SHIFT	2		/*   (in DWORDS)      */
/* Programmable burst length (passed thorugh platform)*/
#define DMA_BUS_MODE_PBL_MASK	0x00003f00	/* Programmable Burst Len */
#define DMA_BUS_MODE_PBL_SHIFT	8
#define DMA_BUS_MODE_ATDS	0x00000080	/* Alternate Descriptor Size */

enum rx_tx_priority_ratio {
	double_ratio = 0x00004000,	/* 2:1 */
	triple_ratio = 0x00008000,	/* 3:1 */
	quadruple_ratio = 0x0000c000,	/* 4:1 */
};

#define DMA_BUS_MODE_FB		0x00010000	/* Fixed burst */
#define DMA_BUS_MODE_MB		0x04000000	/* Mixed burst */
#define DMA_BUS_MODE_RPBL_MASK	0x007e0000	/* Rx-Programmable Burst Len */
#define DMA_BUS_MODE_RPBL_SHIFT	17
#define DMA_BUS_MODE_USP	0x00800000
#define DMA_BUS_MODE_MAXPBL	0x01000000
#define DMA_BUS_MODE_AAL	0x02000000

/* DMA Control register defines */
#define DMA_CONTROL_ST		0x00002000	/* Start/Stop Transmission */
#define DMA_CONTROL_SR		0x00000002	/* Start/Stop Receive */

/* DMA Normal interrupt */
#define DMA_INTR_ENA_NIE 0x00010000	/* Normal Summary */
#define DMA_INTR_ENA_TIE 0x00000001	/* Transmit Interrupt */
#define DMA_INTR_ENA_TUE 0x00000004	/* Transmit Buffer Unavailable */
#define DMA_INTR_ENA_RIE 0x00000040	/* Receive Interrupt */
#define DMA_INTR_ENA_ERE 0x00004000	/* Early Receive */

#define DMA_INTR_NORMAL	(DMA_INTR_ENA_NIE | DMA_INTR_ENA_RIE | \
			DMA_INTR_ENA_TIE)

/* DMA Abnormal interrupt */
#define DMA_INTR_ENA_AIE 0x00008000	/* Abnormal Summary */
#define DMA_INTR_ENA_FBE 0x00002000	/* Fatal Bus Error */
#define DMA_INTR_ENA_ETE 0x00000400	/* Early Transmit */
#define DMA_INTR_ENA_RWE 0x00000200	/* Receive Watchdog */
#define DMA_INTR_ENA_RSE 0x00000100	/* Receive Stopped */
#define DMA_INTR_ENA_RUE 0x00000080	/* Receive Buffer Unavailable */
#define DMA_INTR_ENA_UNE 0x00000020	/* Tx Underflow */
#define DMA_INTR_ENA_OVE 0x00000010	/* Receive Overflow */
#define DMA_INTR_ENA_TJE 0x00000008	/* Transmit Jabber */
#define DMA_INTR_ENA_TSE 0x00000002	/* Transmit Stopped */

#define DMA_INTR_ABNORMAL	(DMA_INTR_ENA_AIE | DMA_INTR_ENA_FBE | \
				DMA_INTR_ENA_UNE)

/* DMA default interrupt mask */
#define DMA_INTR_DEFAULT_MASK	(DMA_INTR_NORMAL | DMA_INTR_ABNORMAL)

/* DMA Status register defines */
#define DMA_STATUS_GLPII        0x40000000	/* GMAC LPI interrupt */
#define DMA_STATUS_GPI          0x10000000	/* PMT interrupt */
#define DMA_STATUS_GMI          0x08000000	/* MMC interrupt */
#define DMA_STATUS_GLI          0x04000000	/* GMAC Line interface int */
#define DMA_STATUS_EB_MASK      0x00380000	/* Error Bits Mask */
#define DMA_STATUS_EB_TX_ABORT  0x00080000	/* Error Bits - TX Abort */
#define DMA_STATUS_EB_RX_ABORT  0x00100000	/* Error Bits - RX Abort */
#define DMA_STATUS_TS_MASK      0x00700000	/* Transmit Process State */
#define DMA_STATUS_TS_SHIFT     20
#define DMA_STATUS_RS_MASK      0x000e0000	/* Receive Process State */
#define DMA_STATUS_RS_SHIFT     17
#define DMA_STATUS_NIS          0x00010000	/* Normal Interrupt Summary */
#define DMA_STATUS_AIS          0x00008000	/* Abnormal Interrupt Summary */
#define DMA_STATUS_ERI          0x00004000	/* Early Receive Interrupt */
#define DMA_STATUS_FBI          0x00002000	/* Fatal Bus Error Interrupt */
#define DMA_STATUS_ETI          0x00000400	/* Early Transmit Interrupt */
#define DMA_STATUS_RWT          0x00000200	/* Receive Watchdog Timeout */
#define DMA_STATUS_RPS          0x00000100	/* Receive Process Stopped */
#define DMA_STATUS_RU           0x00000080	/* Receive Buffer Unavailable */
#define DMA_STATUS_RI           0x00000040	/* Receive Interrupt */
#define DMA_STATUS_UNF          0x00000020	/* Transmit Underflow */
#define DMA_STATUS_OVF          0x00000010	/* Receive Overflow */
#define DMA_STATUS_TJT          0x00000008	/* Transmit Jabber Timeout */
#define DMA_STATUS_TU           0x00000004	/* Transmit Buffer Unavailable */
#define DMA_STATUS_TPS          0x00000002	/* Transmit Process Stopped */
#define DMA_STATUS_TI           0x00000001	/* Transmit Interrupt */
#define DMA_CONTROL_FTF         0x00100000	/* Flush transmit FIFO */

/* AXI Master Bus Mode */
#define DMA_AXI_BUS_MODE	0x00001028

#define DMA_AXI_EN_LPI		BIT(31)
#define DMA_AXI_LPI_XIT_FRM	BIT(30)
#define DMA_AXI_WR_OSR_LMT	GENMASK(23, 20)
#define DMA_AXI_WR_OSR_LMT_SHIFT	20
#define DMA_AXI_WR_OSR_LMT_MASK	0xf
#define DMA_AXI_RD_OSR_LMT	GENMASK(19, 16)
#define DMA_AXI_RD_OSR_LMT_SHIFT	16
#define DMA_AXI_RD_OSR_LMT_MASK	0xf

#define DMA_AXI_OSR_MAX		0xf
#define DMA_AXI_MAX_OSR_LIMIT ((DMA_AXI_OSR_MAX << DMA_AXI_WR_OSR_LMT_SHIFT) | \
			       (DMA_AXI_OSR_MAX << DMA_AXI_RD_OSR_LMT_SHIFT))
#define	DMA_AXI_1KBBE		BIT(13)
#define DMA_AXI_AAL		BIT(12)
#define DMA_AXI_BLEN256		BIT(7)
#define DMA_AXI_BLEN128		BIT(6)
#define DMA_AXI_BLEN64		BIT(5)
#define DMA_AXI_BLEN32		BIT(4)
#define DMA_AXI_BLEN16		BIT(3)
#define DMA_AXI_BLEN8		BIT(2)
#define DMA_AXI_BLEN4		BIT(1)
#define DMA_BURST_LEN_DEFAULT	(DMA_AXI_BLEN256 | DMA_AXI_BLEN128 | \
				 DMA_AXI_BLEN64 | DMA_AXI_BLEN32 | \
				 DMA_AXI_BLEN16 | DMA_AXI_BLEN8 | \
				 DMA_AXI_BLEN4)

#define DMA_AXI_UNDEF		BIT(0)

#define AXI_BLEN	7

#define ETH_MAX_PACKET_SIZE		( ( uint32_t ) 1524U )    /*!< ETH_HEADER + ETH_EXTRA + ETH_VLAN_TAG + ETH_MAX_ETH_PAYLOAD + ETH_CRC */
#define ETH_TX_BUF_SIZE			ETH_MAX_PACKET_SIZE
#define ETH_RX_BUF_SIZE         ETH_MAX_PACKET_SIZE 

/* Receive flow control activation field
 * RFA field in DMA control register, bits 23,10:9
 */
#define DMA_CONTROL_RFA_MASK	0x00800600

/* Receive flow control deactivation field
 * RFD field in DMA control register, bits 22,12:11
 */
#define DMA_CONTROL_RFD_MASK	0x00401800

#define DMA_CONTROL_EFC		0x00000100
#define DMA_CONTROL_FEF		0x00000080
#define DMA_CONTROL_FUF		0x00000040

#define RFA_FULL_MINUS_1K	0x00000000
#define RFA_FULL_MINUS_2K	0x00000200
#define RFA_FULL_MINUS_3K	0x00000400
#define RFA_FULL_MINUS_4K	0x00000600
#define RFA_FULL_MINUS_5K	0x00800000
#define RFA_FULL_MINUS_6K	0x00800200
#define RFA_FULL_MINUS_7K	0x00800400

#define RFD_FULL_MINUS_1K	0x00000000
#define RFD_FULL_MINUS_2K	0x00000800
#define RFD_FULL_MINUS_3K	0x00001000
#define RFD_FULL_MINUS_4K	0x00001800
#define RFD_FULL_MINUS_5K	0x00400000
#define RFD_FULL_MINUS_6K	0x00400800
#define RFD_FULL_MINUS_7K	0x00401000

/* Rx watchdog register */
#define DMA_RX_WATCHDOG		0x00001024

enum rtc_control {
	DMA_CONTROL_RTC_64 = 0x00000000,
	DMA_CONTROL_RTC_32 = 0x00000008,
	DMA_CONTROL_RTC_96 = 0x00000010,
	DMA_CONTROL_RTC_128 = 0x00000018,
};
#define DMA_CONTROL_TC_RX_MASK	0xffffffe7

#define SF_DMA_MODE 1		/* DMA STORE-AND-FORWARD Operation Mode */

#define DMA_CONTROL_TSF	0x00200000	/* Transmit  Store and Forward */

#define DMA_CONTROL_OSF	0x00000004	/* Operate on second frame */

/* DMA operation mode defines (start/stop tx/rx are placed in common header)*/
/* Disable Drop TCP/IP csum error */
#define DMA_CONTROL_DT		0x04000000
#define DMA_CONTROL_RSF		0x02000000	/* Receive Store and Forward */
#define DMA_CONTROL_DFF		0x01000000	/* Disaable flushing */


#define USE_ATDS			1

enum ttc_control {
	DMA_CONTROL_TTC_64 = 0x00000000,
	DMA_CONTROL_TTC_128 = 0x00004000,
	DMA_CONTROL_TTC_192 = 0x00008000,
	DMA_CONTROL_TTC_256 = 0x0000c000,
	DMA_CONTROL_TTC_40 = 0x00010000,
	DMA_CONTROL_TTC_32 = 0x00014000,
	DMA_CONTROL_TTC_24 = 0x00018000,
	DMA_CONTROL_TTC_16 = 0x0001c000,
};
#define DMA_CONTROL_TC_TX_MASK	0xfffe3fff

struct gmac_tx_descriptor
{
	union {
		struct {
			uint32_t
				deferred_bit : 1,			/*  0 */
				underflow_error : 1,        /*  1 */
				excessive_deferral : 1,     /*  2 */
				collision_count : 4,        /*  3 */
				vlan_frame : 1,             /*  7 */
				excessive_collision : 1,    /*  8 */
				late_collision : 1,         /*  9 */
				no_carrier : 1,             /* 10 */
				loss_of_carrier : 1,        /* 11 */
				IP_payload_error : 1,       /* 12 */
				frame_flushed : 1,          /* 13 */
				jabber_timeout : 1,         /* 14 */
				error_summary : 1,          /* 15 */
				ip_header_error : 1,        /* 16 */
				transmit_stamp_status : 1,	/* 17 This field is used as a status bit to indicate
												  that a timestamp was captured for the described transmit frame */
				reserved_2 : 2,				/* 18 */
				second_address_chained : 1,	/* 20 When set, this bit indicates that the second address
												  in the descriptor is the Next descriptor
												  address rather than the second buffer address */
				transmit_end_of_ring : 1,	/* 21 When set, this bit indicates that the descriptor list
												  reached its final descriptor */
				checksum_mode : 2,			/* 22 0x0 : none. 0x1: IP-header, 2: IP & payload, 3: complete */
				reserved_1 : 1,				/* 24 */
				timestamp_enable : 1		/* 25 */,
				disable_padding : 1,		/* 26 When set, the MAC does not add padding to a frame shorter than 64 bytes. */
				disable_crc : 1,			/* 27 */
				first_segment : 1,			/* 28 */
				last_segment : 1,			/* 29 */
				int_on_completion : 1,		/* 30 */
				own : 1;					/* 31 */
			uint32_t
				buf1_byte_count : 13,
				res1 : 3,
				buf2_byte_count : 13,
				res2 : 3;
			uint32_t
				buf1_address;
			uint32_t
				next_descriptor;
			#if ( USE_ATDS != 0 )
				uint32_t reserved_32_1;
				uint32_t reserved_32_2;
				uint32_t time_stamp_low;
				uint32_t time_stamp_high;
			#endif
		};
		struct {
			uint32_t desc0;
			uint32_t desc1;
			uint32_t desc2;
			uint32_t desc3;
		#if ( USE_ATDS != 0 )
			uint32_t desc4;
			uint32_t desc5;
			uint32_t desc6;
			uint32_t desc7;
		#endif
		};
	};
};

struct gmac_rx_descriptor
{
	union {
		struct {
			uint32_t
				ext_status_available : 1,	/*  0 */
				crc_error : 1,				/*  1 */
				dribble_bit_error : 1,		/*  2 */
				receive_error : 1,			/*  3 */
				recv_wdt_timeout : 1,		/*  4 */
				frame_type : 1,				/*  5 */
				late_collision : 1,			/*  6 */
				time_stamp_available : 1,	/*  7 */
				last_descriptor : 1,		/*  8 */
				first_descriptor : 1,		/*  9 */
				vlan_tag : 1,				/* 10 */
				overflow_error : 1,			/* 11 */
				length_error : 1,			/* 12 */
				source_addr_filter_fail : 1,/* 13 Source Address Filter Fail */
				description_error : 1,		/* 14 */
				error_summary : 1,			/* 15 */
				frame_length : 14,			/* 16 */
				dest_addr_filter_fail : 1,	/* 30 Destination Address Filter Fail */
				own : 1;					/* 31 */
			uint32_t
				buf1_byte_count : 13,		/*  0  RBS1: Receive Buffer 1 Size */
				res1 : 1,					/* 13 */
				second_address_chained : 1,	/* 14  RCH: Second Address Chained */
				receive_end_of_ring : 1,	/* 15  RER: Receive End of Ring */
				buf2_byte_count : 13,		/* 16 */
				res2 : 2,					/* 29 */
				disable_int_on_compl : 1;	/* 31  DIC: Disable Interrupt on Completion */
			uint32_t
				buf1_address;
			uint32_t
				next_descriptor;
			#if ( USE_ATDS != 0 )
				uint32_t reserved_32_1;
				uint32_t reserved_32_2;
				uint32_t time_stamp_low;
				uint32_t time_stamp_high;
			#endif
		};

		struct {
			uint32_t desc0;
			uint32_t desc1;
			uint32_t desc2;
			uint32_t desc3;
		#if ( USE_ATDS != 0 )
			uint32_t desc4;
			uint32_t desc5;
			uint32_t desc6;
			uint32_t desc7;
		#endif
		};
	};
};

typedef struct gmac_tx_descriptor gmac_tx_descriptor_t;
typedef struct gmac_rx_descriptor gmac_rx_descriptor_t;

struct xDMA_STATUS_REG
{
	union {
		uint32_t ulValue;
		struct {
			uint32_t
				ti_trans_complete : 1,		/*  0 */
				tps_trans_stopped : 1,		/*  1 */
				tu_cannot_acquire : 1,		/*  2 */
				tjt_tr_jabber_timer : 1,	/*  3 */
				ovf_recv_overflow : 1,		/*  4 */
				unf_tr_underflow : 1,		/*  5 */
				ri_recv_complete : 1,		/*  6 */
				ru_cannot_acquire : 1,		/*  7 */
				rps_recv_proc_stop : 1,		/*  8 */
				rwt_recv_long_frame : 1,	/*  9 */
				eti_tr_to_fifo : 1,			/* 10 */
				reserved_1 : 2,				/* 11 */
				fbi_bus_error : 1,			/* 13 */
				eri_rev_first_buf : 1,		/* 14 */
				ais_abnormal_int : 1,		/* 15 */
				nis_normal_int : 1,			/* 16 */
				rs_recv_state : 3,			/* 17 */
				ts_xmit_state : 3,			/* 20 */
				eb_bus_error : 3,			/* 23 */
				gli_GMAC_line_int : 1,		/* 26 */
				gmi_GMAC_MMC_int : 1,		/* 27 */
				reserved_2 : 1,				/* 28 */
				tti_stamp_trig_int : 1,		/* 29 */
				glpii_GMAC_LPI_int : 1,		/* 30 */
				reserved_3 : 1;				/* 31 */
		};
	};
};

typedef struct xDMA_STATUS_REG DMA_STATUS_REG_t;

struct xEMACInterface {
};

typedef struct xEMACInterface EMACInterface_t;

BaseType_t gmac_tx_descriptor_init( int iMacID, gmac_tx_descriptor_t *pxDMATable, uint8_t *ucDataBuffer, uint32_t ulBufferCount );
BaseType_t gmac_rx_descriptor_init( int iMacID, gmac_rx_descriptor_t *pxDMATable, uint8_t *ucDataBuffer, uint32_t ulBufferCount );

void gmac_dma_start_tx( int iMacID, uint32_t chan );
void gmac_dma_stop_tx( int iMacID, uint32_t chan );
void gmac_dma_start_rx( int iMacID, uint32_t chan );
void gmac_dma_stop_rx( int iMacID, uint32_t chan );

/* Pass the address of the RX descriptor table to DMA. */
void gmac_set_rx_table( int iMacID, gmac_rx_descriptor_t * pxDMATable );

/* Pass the address of the TX descriptor table to DMA. */
void gmac_set_tx_table( int iMacID, gmac_tx_descriptor_t * pxDMATable );

gmac_rx_descriptor_t *gmac_get_rx_table( int iMacID );
gmac_tx_descriptor_t *gmac_get_tx_table( int iMacID );

void gmac_dma_transmit_poll( int iMacID );
void gmac_dma_reception_poll( int iMacID );

void gmac_check_errors( EMACInterface_t *pxEMACif );

struct stmmac_axi
{
	/* When set to 1, this bit enables the LPI mode */
	bool axi_lpi_en;
	/* When set to 1, this bit enables the GMAC-AXI to
	 * come out of the LPI mode only when the Remote
	 * Wake Up Packet is received. When set to 0, this bit
	 * enables the GMAC-AXI to come out of LPI mode
	 * when any frame is received. This bit must be set to 0. */
	bool axi_xit_frm;
	/* AXI Maximum Write OutStanding Request Limit. */
	uint32_t axi_wr_osr_lmt;
	/* This value limits the maximum outstanding request
	 * on the AXI read interface. Maximum outstanding
	 * requests = RD_OSR_LMT+1 */
	uint32_t axi_rd_osr_lmt;
	/*  1 KB Boundary Crossing Enable for the GMAC-AXI
	 * Master When set, the GMAC-AXI Master performs
	 * burst transfers that do not cross 1 KB boundary.
	 * When reset, the GMAC-AXI Master performs burst
	 * transfers that do not cross 4 KB boundary.*/
	bool axi_kbbe;
	uint32_t axi_blen[AXI_BLEN];
};

struct stmmac_dma_cfg
{
	int pbl;
	int txpbl;
	int rxpbl;
	bool pblx8;
	int fixed_burst;
	int mixed_burst;
	bool aal;
};

extern void gmac_dma_axi( int iMacID, struct stmmac_axi *axi );
extern void gmac_dma_init( int iMacID, struct stmmac_dma_cfg *dma_cfg );

extern void gmac_dma_operation_mode( int iMacID, int txmode, int rxmode );

extern void gmac_set_dma_interrupt_enable( int iMacID, uint32_t ulMask );
extern uint32_t gmac_get_dma_interrupt_status( int iMacID, int iClear );
extern uint32_t gmac_clear_dma_interrupt_status( int iMacID, uint32_t ulMask );


#endif /* CYCLONE_DMA_H */
