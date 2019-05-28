/*
 * Copyright 2019, Cypress Semiconductor Corporation or a subsidiary of
 * Cypress Semiconductor Corporation. All Rights Reserved.
 * 
 * This software, associated documentation and materials ("Software")
 * is owned by Cypress Semiconductor Corporation,
 * or one of its subsidiaries ("Cypress") and is protected by and subject to
 * worldwide patent protection (United States and foreign),
 * United States copyright laws and international treaty provisions.
 * Therefore, you may use this Software only as provided in the license
 * agreement accompanying the software package from which you
 * obtained this Software ("EULA").
 * If no EULA applies, Cypress hereby grants you a personal, non-exclusive,
 * non-transferable license to copy, modify, and compile the Software
 * source code solely for use in connection with Cypress's
 * integrated circuit products. Any reproduction, modification, translation,
 * compilation, or representation of this Software except as specified
 * above is prohibited without the express written permission of Cypress.
 *
 * Disclaimer: THIS SOFTWARE IS PROVIDED AS-IS, WITH NO WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING, BUT NOT LIMITED TO, NONINFRINGEMENT, IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE. Cypress
 * reserves the right to make changes to the Software without notice. Cypress
 * does not assume any liability arising out of the application or use of the
 * Software or any product or circuit described in the Software. Cypress does
 * not authorize its products for use in any products where a malfunction or
 * failure of the Cypress product may reasonably be expected to result in
 * significant property damage, injury or death ("High Risk Product"). By
 * including Cypress's product in a High Risk Product, the manufacturer
 * of such system or application assumes all risk of such use and in doing
 * so agrees to indemnify Cypress against all liability.
 *
 * Generic Broadcom Home Networking Division (HND) DMA module.
 * This supports the following chips: BCM42xx, 44xx, 47xx .
 *
 * $Id: hnddma.c 340821 2012-06-25 12:10:33Z deepakd $
 */
//#define _HNDRTE_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <bcm_cfg.h>
#include <typedefs.h>
#include <bcmdefs.h>
#include <bcmdevs.h>
#include <osl.h>
#include <bcmendian.h>
#include <hndsoc.h>
#include <bcmutils.h>
#include <siutils.h>
#include <sbhnddma.h>
#include <m2m_hnddma.h>

#include "wwd_assert.h"

#define DEBUG_DMA

//#define TRACE_PRINT()		do {printf("%s:L%d\n", __FUNCTION__, __LINE__); } while (0)
#define TRACE_PRINT()

#ifdef DEBUG_DMA
#define DMA_DBG(...)		printf(__VA_ARGS__)
#else
#define DMA_DBG(...)
#endif


//#define BCMDBG_DUMP
//typedef unsigned int si_t;
//typedef unsigned int osl_t;
//#define OSL_PKTTAG_SZ 32

//#include <hndrte_lbuf.h>
//#include "dma_porting.h"
//extern int
//bt_printf(const char *fmt, ...);
//#define bt_printf printf

/* debug/trace */
#ifdef BCMDBG
#define    DMA_ERROR(args) if (!(*di->msg_level & 1)); else printf args
#define    DMA_TRACE(args) if (!(*di->msg_level & 2)); else printf args
#elif defined(BCMDBG_ERR)
#define    DMA_ERROR(args) if (!(*di->msg_level & 1)); else printf args
#define DMA_TRACE(args)
#else
//#define    DMA_ERROR(args) bt_printf args
//#define    DMA_TRACE(args) bt_printf args
#define    DMA_ERROR(args)
#define    DMA_TRACE(args)

#endif /* BCMDBG */

#define    DMA_NONE(args)


#define d32txregs    dregs.d32_u.txregs_32
#define d32rxregs    dregs.d32_u.rxregs_32
#define txd32        dregs.d32_u.txd_32
#define rxd32        dregs.d32_u.rxd_32

#define d64txregs    dregs.d64_u.txregs_64
#define d64rxregs    dregs.d64_u.rxregs_64
#define txd64        dregs.d64_u.txd_64
#define rxd64        dregs.d64_u.rxd_64

//volatile int bufs_in_dma;



/* default dma message level (if input msg_level pointer is null in m2m_dma_attach()) */
static uint m2m_dma_msg_level =
#ifdef BCMDBG_ERR
1;
#else
0;
#endif /* BCMDBG_ERR */

#define    MAXNAMEL    8        /* 8 char names */

#define    DI_INFO(dmah)    ((dma_info_t *)dmah)

#define M2M_D64_XS0_CD_MASK     0x00001fff
#define M2M_D64_XS1_AD_MASK     0x00001fff
#define M2M_D64_RS0_CD_MASK     0x00001fff


/* dma engine software state */
typedef struct dma_info {
    struct m2m_hnddma_pub hnddma;    /* exported structure, don't use m2m_hnddma_t,
     * which could be const
     */
    uint        *msg_level;    /* message level pointer */
    char        name[MAXNAMEL];    /* callers name for diag msgs */

    void        *osh;        /* os handle */
    si_t        *sih;        /* sb handle */

    bool        dma64;        /* this dma engine is operating in 64-bit mode */
    bool        addrext;    /* this dma engine supports DmaExtendedAddrChanges */

    union {
        struct {
            dma32regs_t    *txregs_32;    /* 32-bit dma tx engine registers */
            dma32regs_t    *rxregs_32;    /* 32-bit dma rx engine registers */
            dma32dd_t    *txd_32;    /* pointer to dma32 tx descriptor ring */
            dma32dd_t    *rxd_32;    /* pointer to dma32 rx descriptor ring */
        } d32_u;
        struct {
            dma64regs_t    *txregs_64;    /* 64-bit dma tx engine registers */
            dma64regs_t    *rxregs_64;    /* 64-bit dma rx engine registers */
            dma64dd_t    *txd_64;    /* pointer to dma64 tx descriptor ring */
            dma64dd_t    *rxd_64;    /* pointer to dma64 rx descriptor ring */
        } d64_u;
    } dregs;

    uint16        dmadesc_align;    /* alignment requirement for dma descriptors */

    uint16        ntxd;        /* # tx descriptors tunable */
    uint16        txin;        /* index of next descriptor to reclaim */
    uint16        txout;        /* index of next descriptor to post */
    void        **txp;        /* pointer to parallel array of pointers to packets */
    osldma_t     *tx_dmah;    /* DMA TX descriptor ring handle */
    hnddma_seg_map_t    *txp_dmah;    /* DMA MAP meta-data handle */
    dmaaddr_t    txdpa;        /* Aligned physical address of descriptor ring */
    dmaaddr_t    txdpaorig;    /* Original physical address of descriptor ring */
    uint16        txdalign;    /* #bytes added to alloc'd mem to align txd */
    uint32        txdalloc;    /* #bytes allocated for the ring */
    uint32        xmtptrbase;    /* When using unaligned descriptors, the ptr register
     * is not just an index, it needs all 13 bits to be
     * an offset from the addr register.
     */

    uint16        nrxd;        /* # rx descriptors tunable */
    uint16        rxin;        /* index of next descriptor to reclaim */
    uint16        rxout;        /* index of next descriptor to post */
    void        **rxp;        /* pointer to parallel array of pointers to packets */
    osldma_t     *rx_dmah;    /* DMA RX descriptor ring handle */
    hnddma_seg_map_t    *rxp_dmah;    /* DMA MAP meta-data handle */
    dmaaddr_t    rxdpa;        /* Aligned physical address of descriptor ring */
    dmaaddr_t    rxdpaorig;    /* Original physical address of descriptor ring */
    uint16        rxdalign;    /* #bytes added to alloc'd mem to align rxd */
    uint32        rxdalloc;    /* #bytes allocated for the ring */
    uint32        rcvptrbase;    /* Base for ptr reg when using unaligned descriptors */

    /* tunables */
    uint16        rxbufsize;    /* rx buffer size in bytes,
     * not including the extra headroom
     */
    uint        rxextrahdrroom;    /* extra rx headroom, reverseved to assist upper stack
     *  e.g. some rx pkt buffers will be bridged to tx side
     *  without byte copying. The extra headroom needs to be
     *  large enough to fit txheader needs.
     *  Some dongle driver may not need it.
     */
    uint        nrxpost;    /* # rx buffers to keep posted */
    uint        rxoffset;    /* rxcontrol offset */
    uint        ddoffsetlow;    /* add to get dma address of descriptor ring, low 32 bits */
    uint        ddoffsethigh;    /*   high 32 bits */
    uint        dataoffsetlow;    /* add to get dma address of data buffer, low 32 bits */
    uint        dataoffsethigh;    /*   high 32 bits */
    bool        aligndesc_4k;    /* descriptor base need to be aligned or not */
    uint8        rxburstlen;    /* burstlen field for rx (for cores supporting burstlen) */
    uint8        txburstlen;    /* burstlen field for tx (for cores supporting burstlen) */
    uint8        txmultioutstdrd;     /* tx multiple outstanding reads */
    uint8         txprefetchctl;    /* prefetch control for tx */
    uint8         txprefetchthresh;    /* prefetch threshold for tx */
    uint8         rxprefetchctl;    /* prefetch control for rx */
    uint8         rxprefetchthresh;    /* prefetch threshold for rx */
    pktpool_t    *pktpool;    /* pktpool */
    uint        dma_avoidance_cnt;
} dma_info_t;


/* descriptor bumping macros */
#define    XXD(x, n)    ((x) & ((n) - 1))    /* faster than %, but n must be power of 2 */
#define    TXD(x)        XXD((x), di->ntxd)
#define    RXD(x)        XXD((x), di->nrxd)
#define    NEXTTXD(i)    TXD((i) + 1)
#define    PREVTXD(i)    TXD((i) - 1)
#define    NEXTRXD(i)    RXD((i) + 1)
#define    PREVRXD(i)    RXD((i) - 1)

#define    NTXDACTIVE(h, t)    TXD((t) - (h))
#define    NRXDACTIVE(h, t)    RXD((t) - (h))

/* macros to convert between byte offsets and indexes */
#define    B2I(bytes, type)    ((bytes) / sizeof(type))
#define    I2B(index, type)    ((index) * sizeof(type))

#define    PCI32ADDR_HIGH        0xc0000000    /* address[31:30] */
#define    PCI32ADDR_HIGH_SHIFT    30        /* address[31:30] */

#define    PCI64ADDR_HIGH        0x80000000    /* address[63] */
#define    PCI64ADDR_HIGH_SHIFT    31        /* address[63] */

/* Common prototypes */
static bool m2m_dma_isaddrext(dma_info_t *di);
static bool m2m_dma_descriptor_align(dma_info_t *di);
static bool m2m_dma_alloc(dma_info_t *di, uint direction);
static void m2m_dma_ddtable_init(dma_info_t *di, uint direction, dmaaddr_t pa);
static void m2m_dma_rxenable(dma_info_t *di);
static void *m2m_dma_getnextrxp(dma_info_t *di, bool forceall);
static uint8 m2m_dma_align_sizetobits(uint size);
static void *m2m_dma_ringalloc(osl_t *osh, uint32 boundary, uint size, uint16 *alignbits, uint* alloced,
dmaaddr_t *descpa, osldma_t **dmah);

/* Prototypes for 64-bit routines */
static bool m2m_dma64_alloc(dma_info_t *di, uint direction);
static void *m2m_dma64_getnextrxp(dma_info_t *di, bool forceall);

//static bool dma64_rxidle(dma_info_t *di);



static bool m2m_dma64_addrext(osl_t *osh, dma64regs_t *dma64regs);

STATIC INLINE uint32 m2m_parity32(uint32 data);



m2m_hnddma_t *
m2m_dma_attach(osl_t *osh, const char *name, si_t *sih,
volatile void *dmaregstx, volatile void *dmaregsrx,
uint ntxd, uint nrxd, uint rxbufsize, int rxextheadroom, uint nrxpost, uint rxoffset,
uint *msg_level)
{
    dma_info_t *di = NULL;
    uint size;

    DMA_ERROR(("DMA: attach  osh=%p, sih=%p, regstx=%p, regsrx=%p\n"
		"	ntxd=%u, nrxd=%u, bufsz=%u, hroom=%d\n",
			osh, sih, dmaregstx, dmaregsrx, ntxd, nrxd, rxbufsize, rxextheadroom));

    /* allocate private info structure */
    if ((di = MALLOC(osh, sizeof (dma_info_t))) == NULL) {
#ifdef BCMDBG
        DMA_ERROR(("%s: out of memory, malloced %d bytes\n", __FUNCTION__, MALLOCED(osh)));
#endif
        return (NULL);
    }

    bzero(di, sizeof(dma_info_t));

    di->msg_level = msg_level ? msg_level : &m2m_dma_msg_level;

    /* old chips w/o sb is no longer supported */
//    ASSERT(sih != NULL);

    /*  */
    di->dma64 = 1;
    /* check arguments */
    ASSERT(ISPOWEROF2(ntxd));
    ASSERT(ISPOWEROF2(nrxd));

    if (nrxd == 0)
        ASSERT(dmaregsrx == NULL);
    if (ntxd == 0)
        ASSERT(dmaregstx == NULL);
    /* init dma reg pointer */
	ASSERT(ntxd <= D64MAXDD);
	ASSERT(nrxd <= D64MAXDD);
	di->d64txregs = (dma64regs_t *)dmaregstx;
	di->d64rxregs = (dma64regs_t *)dmaregsrx;

    /* Default flags (which can be changed by the driver calling m2m_dma_ctrlflags
     * before enable): For backwards compatibility both Rx Overflow Continue
     * and Parity are DISABLED.
     * supports it.
     */
    m2m_dma_ctrlflags(&di->hnddma, DMA_CTRL_ROC | DMA_CTRL_PEN, 0);

    /* Get the default values (POR) of the burstlen. This can be overridden by the modules
     * if this has to be different. Otherwise this value will be used to program the control
     * register after the reset or during the init.
     */
    if (dmaregsrx) {
		di->rxburstlen = (R_REG(di->osh,
		&di->d64rxregs->control) & D64_RC_BL_MASK) >> D64_RC_BL_SHIFT;
		//    if (CHIPID(sih->chip) == BCM4335_CHIP_ID) {
		if (1) {
			/* Default set to 0 - RX getting stuck with higher values */
			di->rxprefetchctl = 2;
			di->rxprefetchthresh = 0;
		} else {
			di->rxprefetchctl = (R_REG(di->osh,
			&di->d64rxregs->control) & D64_RC_PC_MASK) >>
			D64_RC_PC_SHIFT;
			di->rxprefetchthresh = (R_REG(di->osh,
			&di->d64rxregs->control) & D64_RC_PT_MASK) >>
			D64_RC_PT_SHIFT;
		}
    }

    if (dmaregstx) {
//		di->txburstlen = (R_REG(di->osh,
//		&di->d64txregs->control) & D64_XC_BL_MASK) >> D64_XC_BL_SHIFT;
        di->txburstlen = 0x2;
		di->txmultioutstdrd = (R_REG(di->osh,
		&di->d64txregs->control) & D64_XC_MR_MASK) >> D64_XC_MR_SHIFT;
//		di->txprefetchctl = (R_REG(di->osh,
//		&di->d64txregs->control) & D64_XC_PC_MASK) >> D64_XC_PC_SHIFT;
		di->txprefetchctl = 0x1;
		di->txprefetchthresh = (R_REG(di->osh,
		&di->d64txregs->control) & D64_XC_PT_MASK) >> D64_XC_PT_SHIFT;
    }

    DMA_ERROR(("%s: %s: %s osh %p flags 0x%x ntxd %d nrxd %d rxbufsize %d "
    "rxextheadroom %d nrxpost %d rxoffset %d dmaregstx %p dmaregsrx %p\n",
    name, __FUNCTION__, "DMA64",
    osh, di->hnddma.dmactrlflags, ntxd, nrxd,
    rxbufsize, rxextheadroom, nrxpost, rxoffset, dmaregstx, dmaregsrx));

    /* make a private copy of our callers name */
    strncpy(di->name, name, MAXNAMEL);
    di->name[MAXNAMEL-1] = '\0';

    di->osh = osh;
    di->sih = sih;

    /* save tunables */
    di->ntxd = (uint16)ntxd;
    di->nrxd = (uint16)nrxd;

    /* the actual dma size doesn't include the extra headroom */
    di->rxextrahdrroom = (rxextheadroom == -1) ? BCMEXTRAHDROOM : rxextheadroom;
    if (rxbufsize > BCMEXTRAHDROOM)
        di->rxbufsize = (uint16)(rxbufsize - di->rxextrahdrroom);
    else
        di->rxbufsize = (uint16)rxbufsize;

    di->nrxpost = (uint16)nrxpost;
    di->rxoffset = (uint8)rxoffset;

    /*
     * figure out the DMA physical address offset for dd and data
     *     PCI/PCIE: they map silicon backplace address to zero based memory, need offset
     *     Other bus: use zero
     *     SI_BUS BIGENDIAN kludge: use sdram swapped region for data buffer, not descriptor
     */
    di->ddoffsetlow = 0;
    di->dataoffsetlow = 0;


    /* SDIO core */
    di->addrext = m2m_dma_isaddrext(di);
    /* does the descriptors need to be aligned and if yes, on 4K/8K or not */
    di->aligndesc_4k = m2m_dma_descriptor_align(di);
    if (di->aligndesc_4k) {
		di->dmadesc_align = D64RINGALIGN_BITS;
		if ((ntxd < D64MAXDD / 2) && (nrxd < D64MAXDD / 2)) {
			/* for smaller dd table, HW relax the alignment requirement */
			di->dmadesc_align = D64RINGALIGN_BITS  - 1;
		}
    } else {
        /* The start address of descriptor table should be algined to cache line size,
         * or other structure may share a cache line with it, which can lead to memory
         * overlapping due to cache write-back operation. In the case of MIPS 74k, the
         * cache line size is 32 bytes.
         */
        di->dmadesc_align = 4;    /* 16 byte alignment */
    }

//    bt_printf("DMA descriptor align_needed %d, align %d\n",
//    di->aligndesc_4k, di->dmadesc_align);

    /* allocate tx packet pointer vector */
    if (ntxd) {
        size = ntxd * sizeof(void *);
        if ((di->txp = MALLOC(osh, size)) == NULL) {
            DMA_ERROR(("%s: %s: out of tx memory, malloced %d bytes\n",
            di->name, __FUNCTION__, MALLOCED(osh)));
            goto fail;
        }
        bzero(di->txp, size);
    }

    /* allocate rx packet pointer vector */
    if (nrxd) {
        size = nrxd * sizeof(void *);
        if ((di->rxp = MALLOC(osh, size)) == NULL) {
            DMA_ERROR(("%s: %s: out of rx memory, malloced %d bytes\n",
            di->name, __FUNCTION__, MALLOCED(osh)));
            goto fail;
        }
        bzero(di->rxp, size);
    }

    /* allocate transmit descriptor ring, only need ntxd descriptors but it must be aligned */
    if (ntxd) {
        if (!m2m_dma_alloc(di, DMA_TX))
            goto fail;
    }

    /* allocate receive descriptor ring, only need nrxd descriptors but it must be aligned */
    if (nrxd) {
        if (!m2m_dma_alloc(di, DMA_RX))
            goto fail;
    }

    if ((di->ddoffsetlow != 0) && !di->addrext) {
        if (PHYSADDRLO(di->txdpa) > SI_PCI_DMA_SZ) {
            DMA_ERROR(("%s: %s: txdpa 0x%x: addrext not supported\n",
            di->name, __FUNCTION__, (uint32)PHYSADDRLO(di->txdpa)));
            goto fail;
        }
        if (PHYSADDRLO(di->rxdpa) > SI_PCI_DMA_SZ) {
            DMA_ERROR(("%s: %s: rxdpa 0x%x: addrext not supported\n",
            di->name, __FUNCTION__, (uint32)PHYSADDRLO(di->rxdpa)));
            goto fail;
        }
    }

    DMA_TRACE(("ddoffsetlow 0x%x ddoffsethigh 0x%x dataoffsetlow 0x%x dataoffsethigh "
    "0x%x addrext %d\n", di->ddoffsetlow, di->ddoffsethigh, di->dataoffsetlow,
    di->dataoffsethigh, di->addrext));
TRACE_PRINT();

    return ((m2m_hnddma_t *)di);

    fail:
TRACE_PRINT();
    m2m_dma_detach( (m2m_hnddma_t*)di );
    return (NULL);
}


#if 1

/* Check for odd number of 1's */
STATIC INLINE uint32 m2m_parity32(uint32 data)
{
    data ^= data >> 16;
    data ^= data >> 8;
    data ^= data >> 4;
    data ^= data >> 2;
    data ^= data >> 1;

    return (data & 1);
}

#define DMA64_DD_PARITY(dd)  m2m_parity32((dd)->addrlow ^ (dd)->addrhigh ^ (dd)->ctrl1 ^ (dd)->ctrl2)

/* init the tx or rx descriptor
 * XXX - how to handle native 64-bit addressing AND bit64 extension
 */
static INLINE void
dma64_dd_upd(dma_info_t *di, dma64dd_t *ddring, dmaaddr_t pa, uint outidx, uint32 *flags,
uint32 bufcount)
{
    uint32 ctrl2 = bufcount & D64_CTRL2_BC_MASK;

    /* PCI bus with big(>1G) physical address, use address extension */
        if ((di->dataoffsetlow == 0) || !(PHYSADDRLO(pa) & PCI32ADDR_HIGH)) {
            /* XXX This is where 64-bit addr ext will come into picture but most likely
             * nobody will be around by the time we have full 64-bit memory addressing
             * requirement
             */
            ASSERT((PHYSADDRHI(pa) & PCI64ADDR_HIGH) == 0);
            DMA_ERROR(("low: %p, low: %p, ctrl1: %p, ctrl2: %p\n", (void *) PHYSADDRLO(pa),
                    (void *) di->dataoffsetlow, (void *) BUS_SWAP32(*flags), (void *) BUS_SWAP32(ctrl2)));
            W_SM(&ddring[outidx].addrlow, BUS_SWAP32(PHYSADDRLO(pa) + di->dataoffsetlow));
            W_SM(&ddring[outidx].addrhigh, BUS_SWAP32(PHYSADDRHI(pa) + di->dataoffsethigh));
            W_SM(&ddring[outidx].ctrl1, BUS_SWAP32(*flags));
            W_SM(&ddring[outidx].ctrl2, BUS_SWAP32(ctrl2));
        } else {
            /* address extension for 32-bit PCI */
            uint32 ae;
            ASSERT(di->addrext);

            ae = (PHYSADDRLO(pa) & PCI32ADDR_HIGH) >> PCI32ADDR_HIGH_SHIFT;
            PHYSADDRLO(pa) &= ~PCI32ADDR_HIGH;
            ASSERT(PHYSADDRHI(pa) == 0);

            ctrl2 |= (ae << D64_CTRL2_AE_SHIFT) & D64_CTRL2_AE;
            W_SM(&ddring[outidx].addrlow, BUS_SWAP32(PHYSADDRLO(pa) + di->dataoffsetlow));
            W_SM(&ddring[outidx].addrhigh, BUS_SWAP32(0 + di->dataoffsethigh));
            W_SM(&ddring[outidx].ctrl1, BUS_SWAP32(*flags));
            W_SM(&ddring[outidx].ctrl2, BUS_SWAP32(ctrl2));
        }
        if (di->hnddma.dmactrlflags & DMA_CTRL_PEN) {
            if (DMA64_DD_PARITY(&ddring[outidx])) {
                W_SM(&ddring[outidx].ctrl2, BUS_SWAP32(ctrl2 | D64_CTRL2_PARITY));
            }
        }
    }


    static bool
    m2m_dma_alloc(dma_info_t *di, uint direction)
    {
		return m2m_dma64_alloc(di, direction);
    }

    /* !! may be called with core in reset */
    void m2m_dma_detach(m2m_hnddma_t *di_in)
    {
    	dma_info_t *di = (dma_info_t *) di_in;
TRACE_PRINT();
        DMA_TRACE(("%s: m2m_dma_detach\n", di->name));

        /* shouldn't be here if descriptors are unreclaimed */
        ASSERT(di->txin == di->txout);
        ASSERT(di->rxin == di->rxout);

        /* free dma descriptor rings */
		if (di->txd64)
			DMA_FREE_CONSISTENT(di->osh, ((int8 *)(uintptr)di->txd64 - di->txdalign),
			di->txdalloc, (di->txdpaorig), &di->tx_dmah);
		if (di->rxd64)
			DMA_FREE_CONSISTENT(di->osh, ((int8 *)(uintptr)di->rxd64 - di->rxdalign),
			di->rxdalloc, (di->rxdpaorig), &di->rx_dmah);

        /* free packet pointer vectors */
        if (di->txp)
            MFREE(di->osh, (void *)di->txp, (di->ntxd * sizeof(void *)));
        if (di->rxp)
            MFREE(di->osh, (void *)di->rxp, (di->nrxd * sizeof(void *)));

        /* free tx packet DMA handles */
        if (di->txp_dmah)
            MFREE(di->osh, (void *)di->txp_dmah, di->ntxd * sizeof(hnddma_seg_map_t));

        /* free rx packet DMA handles */
        if (di->rxp_dmah)
            MFREE(di->osh, (void *)di->rxp_dmah, di->nrxd * sizeof(hnddma_seg_map_t));

        /* free our private info structure */
        MFREE(di->osh, (void *)di, sizeof(dma_info_t));

    }

    static bool
    m2m_dma_descriptor_align(dma_info_t *di)
    {
		uint32 addrl;

		/* Check to see if the descriptors need to be aligned on 4K/8K or not */
		if (di->d64txregs != NULL) {
			W_REG(di->osh, &di->d64txregs->addrlow, 0xff0);
			addrl = R_REG(di->osh, &di->d64txregs->addrlow);
			if (addrl != 0)
				return FALSE;
		} else if (di->d64rxregs != NULL) {
			W_REG(di->osh, &di->d64rxregs->addrlow, 0xff0);
			addrl = R_REG(di->osh, &di->d64rxregs->addrlow);
			if (addrl != 0)
				return FALSE;
		}
        return TRUE;
    }

    /* return TRUE if this dma engine supports DmaExtendedAddrChanges, otherwise FALSE */
    static bool
    m2m_dma_isaddrext(dma_info_t *di)
    {
		/* DMA64 supports full 32- or 64-bit operation. AE is always valid */

		/* not all tx or rx channel are available */
		if (di->d64txregs != NULL) {
			if (!m2m_dma64_addrext(di->osh, di->d64txregs)) {
				DMA_ERROR(("%s: _dma_isaddrext: DMA64 tx doesn't have AE set\n",
				di->name));
				ASSERT(0);
			}
			return TRUE;
		} else if (di->d64rxregs != NULL) {
			if (!m2m_dma64_addrext(di->osh, di->d64rxregs)) {
				DMA_ERROR(("%s: _dma_isaddrext: DMA64 rx doesn't have AE set\n",
				di->name));
				ASSERT(0);
			}
			return TRUE;
		}
		return FALSE;
    }

    /* initialize descriptor table base address */
    static void
    m2m_dma_ddtable_init(dma_info_t *di, uint direction, dmaaddr_t pa)
    {
		if (!di->aligndesc_4k) {
			if (direction == DMA_TX)
				di->xmtptrbase = PHYSADDRLO(pa);
			else
				di->rcvptrbase = PHYSADDRLO(pa);
		}

		if ((di->ddoffsetlow == 0) || !(PHYSADDRLO(pa) & PCI32ADDR_HIGH)) {
			if (direction == DMA_TX) {
				W_REG(di->osh, &di->d64txregs->addrlow, (PHYSADDRLO(pa) +
				di->ddoffsetlow));
				W_REG(di->osh, &di->d64txregs->addrhigh, (PHYSADDRHI(pa) +
				di->ddoffsethigh));
			} else {
				W_REG(di->osh, &di->d64rxregs->addrlow, (PHYSADDRLO(pa) +
				di->ddoffsetlow));
				W_REG(di->osh, &di->d64rxregs->addrhigh, (PHYSADDRHI(pa) +
				di->ddoffsethigh));
			}
		} else {
			/* DMA64 32bits address extension */
			uint32 ae;
			ASSERT(di->addrext);
			ASSERT(PHYSADDRHI(pa) == 0);

			/* shift the high bit(s) from pa to ae */
			ae = (PHYSADDRLO(pa) & PCI32ADDR_HIGH) >> PCI32ADDR_HIGH_SHIFT;
			PHYSADDRLO(pa) &= ~PCI32ADDR_HIGH;

			if (direction == DMA_TX) {
				W_REG(di->osh, &di->d64txregs->addrlow, (PHYSADDRLO(pa) +
				di->ddoffsetlow));
				W_REG(di->osh, &di->d64txregs->addrhigh, di->ddoffsethigh);
				SET_REG(di->osh, &di->d64txregs->control, D64_XC_AE,
				(ae << D64_XC_AE_SHIFT));
			} else {
				W_REG(di->osh, &di->d64rxregs->addrlow, (PHYSADDRLO(pa) +
				di->ddoffsetlow));
				W_REG(di->osh, &di->d64rxregs->addrhigh, di->ddoffsethigh);
				SET_REG(di->osh, &di->d64rxregs->control, D64_RC_AE,
				(ae << D64_RC_AE_SHIFT));
			}
		}
    }

//    void dma_fifoloopbackenable(m2m_hnddma_t *di_in)
//    {
//    	dma_info_t *di = (dma_info_t *) di_in;
//        DMA_TRACE(("%s: dma_fifoloopbackenable\n", di->name));
//
//		OR_REG(di->osh, &di->d64txregs->control, D64_XC_LE);
//    }

    void m2m_dma_rxinit(m2m_hnddma_t *di_in)
    {
    	dma_info_t *di = (dma_info_t *) di_in;
	uint32 addrlow;

        DMA_TRACE(("%s: m2m_dma_rxinit\n", di->name));

        if (di->nrxd == 0)
            return;

        di->rxin = di->rxout = 0;

        /* clear rx descriptor ring */
		BZERO_SM((void *)(uintptr)di->rxd64, (di->nrxd * sizeof(dma64dd_t)));

		/* DMA engine with out alignment requirement requires table to be inited
		 * before enabling the engine
		 */
		if (!di->aligndesc_4k)
			m2m_dma_ddtable_init(di, DMA_RX, (di->rxdpa));

		addrlow = R_REG(di->osh, &di->d64rxregs->addrlow);
		addrlow &= 0xffff;
		W_REG(di->osh, &di->d64rxregs->ptr, addrlow);

		m2m_dma_rxenable(di);

		if (di->aligndesc_4k)
			m2m_dma_ddtable_init(di, DMA_RX, (di->rxdpa));

		addrlow = R_REG(di->osh, &di->d64rxregs->addrlow);
		addrlow &= 0xffff;
		W_REG(di->osh, &di->d64rxregs->ptr, addrlow);
    }

    static void
    m2m_dma_rxenable(dma_info_t *di)
    {
        uint dmactrlflags = di->hnddma.dmactrlflags;

        DMA_TRACE(("%s: dma_rxenable\n", di->name));

		uint32 control = (R_REG(di->osh, &di->d64rxregs->control) & D64_RC_AE) | D64_RC_RE;

		if ((dmactrlflags & DMA_CTRL_PEN) == 0)
			control |= D64_RC_PD;

		if (dmactrlflags & DMA_CTRL_ROC)
			control |= D64_RC_OC;

		/* These bits 20:18 (burstLen) of control register can be written but will take
		 * effect only if these bits are valid. So this will not affect previous versions
		 * of the DMA. They will continue to have those bits set to 0.
		 */
		control &= ~D64_RC_BL_MASK;
		control |= (di->rxburstlen << D64_RC_BL_SHIFT);

		control &= ~D64_RC_PC_MASK;
		control |= (di->rxprefetchctl << D64_RC_PC_SHIFT);

		control &= ~D64_RC_PT_MASK;
		control |= (di->rxprefetchthresh << D64_RC_PT_SHIFT);

		W_REG(di->osh, &di->d64rxregs->control,
		((di->rxoffset << D64_RC_RO_SHIFT) | control));

//		W_REG(di->osh, &di->d64rxregs->control,
//		      0xc09);


//s_printf("@@rxoffset: 0x%x, control : 0x%x\n", (di->rxoffset << D64_RC_RO_SHIFT), control);
    }


    /* !! rx entry routine
     * returns a pointer to the next frame received, or NULL if there are no more
     *   if DMA_CTRL_RXMULTI is defined, DMA scattering(multiple buffers) is supported
     *      with pkts chain
     *   otherwise, it's treated as giant pkt and will be tossed.
     *   The DMA scattering starts with normal DMA header, followed by first buffer data.
     *   After it reaches the max size of buffer, the data continues in next DMA descriptor
     *   buffer WITHOUT DMA header
     */
    void * BCMFASTPATH m2m_dma_rx(m2m_hnddma_t *di_in)
    {
    	dma_info_t *di = (dma_info_t *) di_in;
        void *head;
        uint len;
        uint pkt_len;
        int resid = 0;
        dma64regs_t *dregs = di->d64rxregs;

        head = m2m_dma_getnextrxp(di, FALSE);
        if (head == NULL)
            return (NULL);

//        bufs_in_dma = NRXDACTIVE(di->rxin, di->rxout);

        if ((R_REG(di->osh, &dregs->control) & D64_RC_GE)) {
            /* In case of glommed pkt get length from hwheader */
            len = ltoh16(*((uint16 *)(PKTDATA(di->osh, head)) + di->rxoffset/2 + 2)) + 4;

            *(uint16 *)(PKTDATA(di->osh, head)) = len;
        } else {
            len = ltoh16(*(uint16 *)(PKTDATA(di->osh, head)));
        }

        DMA_TRACE(("%s: m2m_dma_rx len %d\n", di->name, len));

        /* set actual length */
        pkt_len = MIN((di->rxoffset + len), di->rxbufsize);
        PKTSETLEN(di->osh, head, pkt_len);
        resid = len - (di->rxbufsize - di->rxoffset);

        wiced_assert("Need more buffer to RX the packet\n", resid <= 0 );
        if (resid > 0)
        {
            DMA_ERROR(("Need more buffer to RX the packet!\n"));
        }
        return (head);
    }

    /* post receive buffers
     *  return FALSE is refill failed completely and ring is empty
     *  this will stall the rx dma and user might want to call rxfill again asap
     *  This unlikely happens on memory-rich NIC, but often on memory-constrained dongle
     */
    bool BCMFASTPATH m2m_dma_rxfill(m2m_hnddma_t *di_in)
    {
    	dma_info_t *di = (dma_info_t *) di_in;
        void *p;
        uint16 rxin, rxout;
        uint32 flags = 0;
        uint n;
        uint i;
        dmaaddr_t pa;
        uint extra_offset = 0, extra_pad;
        bool ring_empty;
        uint alignment_req = (di->hnddma.dmactrlflags & DMA_CTRL_USB_BOUNDRY4KB_WAR) ?
        16 : 1;    /* MUST BE POWER of 2 */

        ring_empty = FALSE;

        /*
         * Determine how many receive buffers we're lacking
         * from the full complement, allocate, initialize,
         * and post them, then update the chip rx lastdscr.
         */

        rxin = di->rxin;
        rxout = di->rxout;

        n = di->nrxpost - NRXDACTIVE(rxin, rxout);

        if (di->rxbufsize > BCMEXTRAHDROOM)
            extra_offset = di->rxextrahdrroom;

        DMA_TRACE(("%s: m2m_dma_rxfill: req post %d\n", di->name, n));

        i = 0;
        while ( ( i < n ) && ( ( p = PKTGET(di->osh, (di->rxbufsize + extra_offset +  alignment_req - 1), FALSE) ) != NULL ) )
        {
            /* reserve an extra headroom, if applicable */
            if (di->hnddma.dmactrlflags & DMA_CTRL_USB_BOUNDRY4KB_WAR) {
                extra_pad = ((alignment_req - (uint)(((unsigned long)PKTDATA(di->osh, p) -
                (unsigned long)(uchar *)0))) & (alignment_req - 1));
            } else
                extra_pad = 0;

            if (extra_offset + extra_pad)
                PKTPULL(di->osh, p, extra_offset + extra_pad);

#ifdef CTFMAP
            /* mark as ctf buffer for fast mapping */
            if (CTF_ENAB(kcih)) {
                ASSERT((((uint32)PKTDATA(di->osh, p)) & 31) == 0);
                PKTSETCTF(di->osh, p);
            }
#endif /* CTFMAP */

            /* PR3263 & PR3387 & PR4642 war: rxh.len=0 means dma writes not complete */
            /* Do a cached write instead of uncached write since DMA_MAP
             * will flush the cache.
             */
            *(uint32 *)(PKTDATA(di->osh, p)) = 0;

            pa = DMA_MAP(di->osh, PKTDATA(di->osh, p),
            di->rxbufsize, DMA_RX, p,
            &di->rxp_dmah[rxout]);

            ASSERT(ISALIGNED(PHYSADDRLO(pa), 4));


            /* save the free packet pointer */
            ASSERT(di->rxp[rxout] == NULL);
            di->rxp[rxout] = p;

            /* reset flags for each descriptor */
            flags = 0;
			if (rxout == (di->nrxd - 1))
				flags = D64_CTRL1_EOT;

			dma64_dd_upd(di, di->rxd64, (pa), rxout, &flags, di->rxbufsize);
            rxout = NEXTRXD(rxout);

            i++;
        }

        DMA_TRACE(("%s: m2m_dma_rxfill: posted %d\n", di->name, i));

        ring_empty = ( 0 == NRXDACTIVE(rxin, rxout) && ( 0 == i ) );

        di->rxout = rxout;
#if 1

        /* update the chip lastdscr pointer */
		W_REG(di->osh, &di->d64rxregs->ptr, (di->rcvptrbase + I2B(rxout, dma64dd_t)));
		//        bt_printf("############ rxptr: 0x%p, rcvptrbase: 0x%x, value: 0x%x\n", &di->d64rxregs->ptr,
		//        di->rcvptrbase, I2B(rxout, dma64dd_t));
#endif
        return ring_empty;
    }


    int m2m_dma_rxactive(m2m_hnddma_t *di_in)
    {
        dma_info_t *di = (dma_info_t *) di_in;
        return NRXDACTIVE(di->rxin, di->rxout);
    }

    void m2m_dma_rxreclaim(m2m_hnddma_t *di_in)
    {
    	dma_info_t *di = (dma_info_t *) di_in;
        void *p;
//        bool origcb = TRUE;

#ifndef EFI
        /* "unused local" warning suppression for OSLs that
         * define PKTFREE() without using the di->osh arg
         */
        di = di;
#endif /* EFI */

        DMA_TRACE(("%s: m2m_dma_rxreclaim\n", di->name));

        while ((p = m2m_dma_getnextrxp(di, TRUE)))
            PKTFREE(di->osh, p, FALSE);

    }

    static void * BCMFASTPATH
    m2m_dma_getnextrxp(dma_info_t *di, bool forceall)
    {
        if (di->nrxd == 0)
            return (NULL);

		return m2m_dma64_getnextrxp(di, forceall);
    }


    uint m2m_dma_ctrlflags(m2m_hnddma_t *di_in, uint mask, uint flags)
    {
    	dma_info_t *di = (dma_info_t *) di_in;
        uint dmactrlflags;

        if (!di) {
            DMA_ERROR(("_m2m_dma_ctrlflags: NULL dma handle\n"));
            return (0);
        }

        dmactrlflags = di->hnddma.dmactrlflags;
        ASSERT((flags & ~mask) == 0);

        dmactrlflags &= ~mask;
        dmactrlflags |= flags;

        /* If trying to enable parity, check if parity is actually supported */
        if (dmactrlflags & DMA_CTRL_PEN) {
            uint32 control;

			control = R_REG(di->osh, &di->d64txregs->control);
			W_REG(di->osh, &di->d64txregs->control, control | D64_XC_PD);
			if (R_REG(di->osh, &di->d64txregs->control) & D64_XC_PD) {
				/* We *can* disable it so it is supported,
				 * restore control register
				 */
				W_REG(di->osh, &di->d64txregs->control, control);
			} else {
				/* Not supported, don't allow it to be enabled */
				dmactrlflags &= ~DMA_CTRL_PEN;
			}
        }

        di->hnddma.dmactrlflags = dmactrlflags;

        return (dmactrlflags);
    }


    static
    uint8 m2m_dma_align_sizetobits(uint size)
    {
        uint8 bitpos = 0;
        ASSERT(size);
        ASSERT(!(size & (size-1)));
        while (size >>= 1) {
            bitpos ++;
        }
        return (bitpos);
    }

    /* This function ensures that the DMA descriptor ring will not get allocated
     * across Page boundary. If the allocation is done across the page boundary
     * at the first time, then it is freed and the allocation is done at
     * descriptor ring size aligned location. This will ensure that the ring will
     * not cross page boundary
     */
    static void *
    m2m_dma_ringalloc(osl_t *osh, uint32 boundary, uint size, uint16 *alignbits, uint* alloced,
    dmaaddr_t *descpa, osldma_t **dmah)
    {
        void * va;
        uint32 desc_strtaddr;
        uint32 alignbytes = 1 << *alignbits;
        *alignbits = m2m_dma_align_sizetobits(size);

        if ((va = DMA_ALLOC_CONSISTENT(osh, size, *alignbits, alloced, descpa, dmah)) == NULL)
            return NULL;

        desc_strtaddr = (uint32)ROUNDUP((uint)PHYSADDRLO(*descpa), alignbytes);
        if (((desc_strtaddr + size - 1) & boundary) !=
        (desc_strtaddr & boundary)) {
            *alignbits = m2m_dma_align_sizetobits(size);
            DMA_FREE_CONSISTENT(osh, va,
            size, *descpa, dmah);
            va = DMA_ALLOC_CONSISTENT(osh, size, *alignbits, alloced, descpa, dmah);
        }
        return va;
    }


    /* 64-bit DMA functions */

    void m2m_dma_txinit(m2m_hnddma_t *di_in)
    {
    	dma_info_t *di = (dma_info_t *) di_in;
        uint32 control;
	uint32 addrlow;

        DMA_TRACE(("%s: m2m_dma_txinit\n", di->name));

        if (di->ntxd == 0)
            return;

        di->txin = di->txout = 0;
        di->hnddma.txavail = di->ntxd - 1;

        /* clear tx descriptor ring */
        BZERO_SM((void *)(uintptr)di->txd64, (di->ntxd * sizeof(dma64dd_t)));

        /* These bits 20:18 (burstLen) of control register can be written but will take
         * effect only if these bits are valid. So this will not affect previous versions
         * of the DMA. They will continue to have those bits set to 0.
         */
        control = R_REG(di->osh, &di->d64txregs->control);
        control = (control & ~D64_XC_BL_MASK) | (di->txburstlen << D64_XC_BL_SHIFT);
        control = (control & ~D64_XC_MR_MASK) | (di->txmultioutstdrd << D64_XC_MR_SHIFT);
        control = (control & ~D64_XC_PC_MASK) | (di->txprefetchctl << D64_XC_PC_SHIFT);
        control = (control & ~D64_XC_PT_MASK) | (di->txprefetchthresh << D64_XC_PT_SHIFT);
        W_REG(di->osh, &di->d64txregs->control, control);

        control = D64_XC_XE;
        /* DMA engine with out alignment requirement requires table to be inited
         * before enabling the engine
         */
//        if (!di->aligndesc_4k)
            m2m_dma_ddtable_init(di, DMA_TX, di->txdpa);

  addrlow = R_REG(di->osh, &di->d64txregs->addrlow);
	addrlow &= 0xffff;
	W_REG(di->osh, &di->d64txregs->ptr, addrlow);

        if ((di->hnddma.dmactrlflags & DMA_CTRL_PEN) == 0)
            control |= D64_XC_PD;

//      control &= 0xfffffff0;
        OR_REG(di->osh, &di->d64txregs->control, control);

        /* DMA engine with alignment requirement requires table to be inited
         * before enabling the engine
         */
//        if (di->aligndesc_4k)
//            _dma_ddtable_init(di, DMA_TX, di->txdpa);

//	addrlow = R_REG(di->osh, &di->d64txregs->addrlow);
//	addrlow &= 0xffff;
//	W_REG(di->osh, &di->d64txregs->ptr, addrlow);
    }


    void BCMFASTPATH m2m_dma_txreclaim(m2m_hnddma_t *di_in, txd_range_t range)
    {
    	dma_info_t *di = (dma_info_t *) di_in;
        void *p;

        DMA_TRACE(("%s: m2m_dma_txreclaim %s\n", di->name,
        (range == HNDDMA_RANGE_ALL) ? "all" :
        ((range == HNDDMA_RANGE_TRANSMITTED) ? "transmitted" : "transfered")));

        if (di->txin == di->txout)
            return;

        while ((p = m2m_dma_getnexttxp((m2m_hnddma_t*)di, range))) {
            /* For unframed data, we don't have any packets to free */
            if (!(di->hnddma.dmactrlflags & DMA_CTRL_UNFRAMED))
                //PKTFREE(di->osh, p, TRUE);
                PKTFREE(di->osh, p, FALSE);
        }
    }

    int m2m_dma_txactive(m2m_hnddma_t *di_in)
    {
        dma_info_t *di = (dma_info_t *) di_in;
        return NTXDACTIVE(di->txin, di->txout);
    }

    static bool
    m2m_dma64_alloc(dma_info_t *di, uint direction)
    {
        uint16 size;
        uint ddlen;
        void *va;
        uint alloced = 0;
        uint16 align;
        uint16 align_bits;

        ddlen = sizeof(dma64dd_t);

        size = (direction == DMA_TX) ? (di->ntxd * ddlen) : (di->nrxd * ddlen);
        align_bits = di->dmadesc_align;

        if (direction == DMA_TX) {
            if ((va = m2m_dma_ringalloc(di->osh, D64RINGBOUNDARY, size, &align_bits, &alloced,
            &di->txdpaorig, &di->tx_dmah)) == NULL) {
                DMA_ERROR(("%s: dma64_alloc: DMA_ALLOC_CONSISTENT(ntxd) failed\n",
                di->name));
                return FALSE;
            }
            align = (1 << align_bits);

            /* adjust the pa by rounding up to the alignment */
            PHYSADDRLOSET(di->txdpa, ROUNDUP(PHYSADDRLO(di->txdpaorig), align));
            PHYSADDRHISET(di->txdpa, PHYSADDRHI(di->txdpaorig));

            /* Make sure that alignment didn't overflow */
            ASSERT(PHYSADDRLO(di->txdpa) >= PHYSADDRLO(di->txdpaorig));

            /* find the alignment offset that was used */
            di->txdalign = (uint)(PHYSADDRLO(di->txdpa) - PHYSADDRLO(di->txdpaorig));

            /* adjust the va by the same offset */
            di->txd64 = (dma64dd_t *)((uintptr)va + di->txdalign);

            di->txdalloc = alloced;
            ASSERT(ISALIGNED(PHYSADDRLO(di->txdpa), align));
        } else {
            if ((va = m2m_dma_ringalloc(di->osh, D64RINGBOUNDARY, size, &align_bits, &alloced,
            &di->rxdpaorig, &di->rx_dmah)) == NULL) {
                DMA_ERROR(("%s: dma64_alloc: DMA_ALLOC_CONSISTENT(nrxd) failed\n",
                di->name));
                return FALSE;
            }
            align = (1 << align_bits);

            /* adjust the pa by rounding up to the alignment */
            PHYSADDRLOSET(di->rxdpa, ROUNDUP(PHYSADDRLO(di->rxdpaorig), align));
            PHYSADDRHISET(di->rxdpa, PHYSADDRHI(di->rxdpaorig));

            /* Make sure that alignment didn't overflow */
            ASSERT(PHYSADDRLO(di->rxdpa) >= PHYSADDRLO(di->rxdpaorig));

            /* find the alignment offset that was used */
            di->rxdalign = (uint)(PHYSADDRLO(di->rxdpa) - PHYSADDRLO(di->rxdpaorig));

            /* adjust the va by the same offset */
            di->rxd64 = (dma64dd_t *)((uintptr)va + di->rxdalign);

            di->rxdalloc = alloced;
            ASSERT(ISALIGNED(PHYSADDRLO(di->rxdpa), align));
        }

        return TRUE;
    }

bool m2m_dma_txreset_inner(void *osh, void *txregs)
{
    uint32 status;
    dma64regs_t *pd64txregs = (dma64regs_t *) txregs;

    /* address PR8249/PR7577 issue */
    /* suspend tx DMA first */
    W_REG(osh, &pd64txregs->control, D64_XC_SE);
    SPINWAIT(((status = (R_REG(osh, &pd64txregs->status0) & D64_XS0_XS_MASK)) !=
              D64_XS0_XS_DISABLED) &&
              (status != D64_XS0_XS_IDLE) &&
              (status != D64_XS0_XS_STOPPED),
              10000);

        /* PR2414 WAR: DMA engines are not disabled until transfer finishes */
    W_REG(osh, &pd64txregs->control, 0);
    SPINWAIT(((status = (R_REG(osh, &pd64txregs->status0) & D64_XS0_XS_MASK)) !=
              D64_XS0_XS_DISABLED),
              10000);

    /* We should be disabled at this point */
    if (status != D64_XS0_XS_DISABLED) {
        /* cannot call DMA_ERROR in BCMDBG because di is not available */
        // DMA_ERROR(("%s: status != D64_XS0_XS_DISABLED 0x%x\n", __FUNCTION__, status));
        ASSERT(status == D64_XS0_XS_DISABLED);
        OSL_DELAY(300);
    }

    return (status == D64_XS0_XS_DISABLED);
}

bool m2m_dma_txreset(m2m_hnddma_t *di_in)
{
    	dma_info_t *di = (dma_info_t *) di_in;

	if (di->ntxd == 0)
		return TRUE;

	return (m2m_dma_txreset_inner(di->osh, (void *) di->d64txregs));
}

bool m2m_dma_rxreset_inner(void *osh, void *rxregs)
{
    dma64regs_t *pd64txregs = (dma64regs_t *) rxregs;
    uint32 status;

    /* PR2414 WAR: DMA engines are not disabled until transfer finishes */
    W_REG(osh, &pd64txregs->control, 0);
    SPINWAIT(((status = (R_REG(osh, &pd64txregs->status0) & D64_RS0_RS_MASK)) !=
    D64_RS0_RS_DISABLED), 10000);

    return (status == D64_RS0_RS_DISABLED);
}

//    static bool
//    dma64_rxidle(dma_info_t *di)
//    {
//        DMA_TRACE(("%s: m2m_dma_rxidle\n", di->name));
//
//        if (di->nrxd == 0)
//            return TRUE;
//
//        return ((R_REG(di->osh, &di->d64rxregs->status0) & M2M_D64_RS0_CD_MASK) ==
//        (R_REG(di->osh, &di->d64rxregs->ptr) & M2M_D64_RS0_CD_MASK));
//    }

    bool m2m_dma_rxreset(m2m_hnddma_t *di_in)
    {
    	dma_info_t *di = (dma_info_t *) di_in;

        if (di->nrxd == 0)
            return TRUE;

        return (m2m_dma_rxreset_inner(di->osh, (void *)di->d64rxregs));
    }


    /* !! tx entry routine
     * WARNING: call must check the return value for error.
     *   the error(toss frames) could be fatal and cause many subsequent hard to debug problems
     */
    int BCMFASTPATH m2m_dma_txfast(m2m_hnddma_t *di_in, void *p0, bool commit)
    {
       	dma_info_t *di = (dma_info_t *) di_in;
        void *next;
        uchar *data;
        uint len;
        uint16 txout;
        uint32 flags = 0;
        dmaaddr_t pa;

        DMA_TRACE(("%s: m2m_dma_txfast\n", di->name));
        txout = di->txout;

        /*
         * Walk the chain of packet buffers
         * allocating and initializing transmit descriptor entries.
         */
//        for (p = p0; p; p = next)
        {
            data = PKTDATA( di->osh, p0 ) + sizeof(wwd_buffer_header_t);
            len  = PKTLEN(di->osh, p0) - sizeof(wwd_buffer_header_t);
#ifdef BCM_DMAPAD
//            len += PKTDMAPAD(di->osh, p);
#endif /* BCM_DMAPAD */
            next = NULL;

            /* return nonzero if out of tx descriptors */
            if (NEXTTXD(txout) == di->txin)
            {
                DMA_ERROR(("%s: m2m_dma_txfast: NEXTTXD(txout) == di->txin  (both %d)", __FUNCTION__,
                                                di->txin ));
                goto outoftxd;
            }

            /* PR988 - skip zero length buffers */
//            if (len == 0)
//                continue;

            /* get physical address of buffer start */

            pa = DMA_MAP(di->osh, data, len, DMA_TX, p0, &di->txp_dmah[txout]);

			flags = 0;

			flags |= D64_CTRL1_SOF;

			/* With a DMA segment list, Descriptor table is filled
			 * using the segment list instead of looping over
			 * buffers in multi-chain DMA. Therefore, EOF for SGLIST is when
			 * end of segment list is reached.
			 */
			if ( next == NULL )
				flags |= (D64_CTRL1_IOC | D64_CTRL1_EOF);
			if (txout == (di->ntxd - 1))
				flags |= D64_CTRL1_EOT;

DMA_ERROR(("APPS->WLAN pa: %p, len: %d\n", p0, len));
			dma64_dd_upd(di, di->txd64, pa , txout, &flags, len);
			ASSERT(di->txp[txout] == NULL);

			txout = NEXTTXD(txout);
			/* return nonzero if out of tx descriptors */
			if (txout == di->txin) {
				DMA_ERROR(("%s: m2m_dma_txfast: Out-of-DMA descriptors"
				" (txin %d txout %d )\n", __FUNCTION__,
				di->txin, di->txout));
				goto outoftxd;
			}

        }

        /* if last txd eof not set, fix it */
        if (!(flags & D64_CTRL1_EOF))
            W_SM(&di->txd64[PREVTXD(txout)].ctrl1,
            BUS_SWAP32(flags | D64_CTRL1_IOC | D64_CTRL1_EOF));

        /* save the packet */
        di->txp[PREVTXD(txout)] = p0;

        /* bump the tx descriptor index */
        di->txout = txout;

        /* kick the chip */
        if (commit)
            W_REG(di->osh, &di->d64txregs->ptr, (di->xmtptrbase + I2B(txout, dma64dd_t)));

        /* tx flow control */
        di->hnddma.txavail = di->ntxd - NTXDACTIVE(di->txin, di->txout) - 1;

        return (0);

        outoftxd:
        DMA_ERROR(("%s: m2m_dma_txfast: out of txds !!!\n", di->name));
        PKTFREE(di->osh, p0, TRUE);
        di->hnddma.txavail = 0;
        di->hnddma.txnobuf++;
        return (-1);
    }

    /*
     * Reclaim next completed txd (txds if using chained buffers) in the range
     * specified and return associated packet.
     * If range is HNDDMA_RANGE_TRANSMITTED, reclaim descriptors that have be
     * transmitted as noted by the hardware "CurrDescr" pointer.
     * If range is HNDDMA_RANGE_TRANSFERED, reclaim descriptors that have be
     * transfered by the DMA as noted by the hardware "ActiveDescr" pointer.
     * If range is HNDDMA_RANGE_ALL, reclaim all txd(s) posted to the ring and
     * return associated packet regardless of the value of hardware pointers.
     */
    void * BCMFASTPATH m2m_dma_getnexttxp(m2m_hnddma_t *di_in, txd_range_t range)
    {
    	dma_info_t *di = (dma_info_t *) di_in;
        uint16 start, end, i;
        uint16 active_desc;
        void *txp;

        DMA_TRACE(("%s: m2m_dma_getnexttxp %s\n", di->name,
        (range == HNDDMA_RANGE_ALL) ? "all" :
        ((range == HNDDMA_RANGE_TRANSMITTED) ? "transmitted" : "transfered")));

        if (di->ntxd == 0)
            return (NULL);

        txp = NULL;

        start = di->txin;
        if (range == HNDDMA_RANGE_ALL)
            end = di->txout;
        else {
            dma64regs_t *dregs = di->d64txregs;

            end = (uint16)(B2I(((R_REG(di->osh, &dregs->status0) & M2M_D64_XS0_CD_MASK) -
            di->xmtptrbase) & M2M_D64_XS0_CD_MASK, dma64dd_t));

            if (range == HNDDMA_RANGE_TRANSFERED) {
                active_desc = (uint16)(R_REG(di->osh, &dregs->status1) & M2M_D64_XS1_AD_MASK);
                active_desc = (active_desc - di->xmtptrbase) & M2M_D64_XS0_CD_MASK;
                active_desc = B2I(active_desc, dma64dd_t);
                if (end != active_desc)
                    end = PREVTXD(active_desc);
            }
        }

        /* PR4738 - xmt disable/re-enable does not clear CURR */
        if ((start == 0) && (end > di->txout))
            goto bogus;

        for (i = start; i != end && !txp; i = NEXTTXD(i)) {
            /* XXX - dma 64-bit */
            dmaaddr_t pa;
            uint size;

            PHYSADDRLOSET(pa, (BUS_SWAP32(R_SM(&di->txd64[i].addrlow)) - di->dataoffsetlow));
            PHYSADDRHISET(pa, (BUS_SWAP32(R_SM(&di->txd64[i].addrhigh)) - di->dataoffsethigh));


			size = (BUS_SWAP32(R_SM(&di->txd64[i].ctrl2)) & D64_CTRL2_BC_MASK);

			W_SM(&di->txd64[i].addrlow, 0xdeadbeef);
			W_SM(&di->txd64[i].addrhigh, 0xdeadbeef);

			txp = di->txp[i];
			di->txp[i] = NULL;

            DMA_UNMAP(di->osh, pa, size, DMA_TX, txp, 0);
            (void) size;
            (void) pa;
        }

        di->txin = i;

        /* tx flow control */
        di->hnddma.txavail = di->ntxd - NTXDACTIVE(di->txin, di->txout) - 1;

        return (txp);

        bogus:
        DMA_NONE(("m2m_dma_getnexttxp: bogus curr: start %d end %d txout %d force %d\n",
        start, end, di->txout, forceall));
        return (NULL);
    }

    static void * BCMFASTPATH
    m2m_dma64_getnextrxp(dma_info_t *di, bool forceall)
    {
        uint i, curr;
        void *rxp;
        dmaaddr_t pa;

        /* if forcing, dma engine must be disabled */
//        ASSERT(!forceall || !dma64_rxenabled(di));

        i = di->rxin;

        /* return if no packets posted */
        if (i == di->rxout)
            return (NULL);

        curr = B2I(((R_REG(di->osh, &di->d64rxregs->status0) & M2M_D64_RS0_CD_MASK) -
        di->rcvptrbase) & M2M_D64_RS0_CD_MASK, dma64dd_t);

        /* ignore curr if forceall */
        if (!forceall && (i == curr))
            return (NULL);

        /* get the packet pointer that corresponds to the rx descriptor */
        rxp = di->rxp[i];
        ASSERT(rxp);
        di->rxp[i] = NULL;

        PHYSADDRLOSET(pa, (BUS_SWAP32(R_SM(&di->rxd64[i].addrlow)) - di->dataoffsetlow));
        PHYSADDRHISET(pa, (BUS_SWAP32(R_SM(&di->rxd64[i].addrhigh)) - di->dataoffsethigh));

        /* clear this packet from the descriptor ring */
        DMA_UNMAP(di->osh, pa,
        di->rxbufsize, DMA_RX, rxp, &di->rxp_dmah[i]);

        W_SM(&di->rxd64[i].addrlow, 0xdeadbeef);
        W_SM(&di->rxd64[i].addrhigh, 0xdeadbeef);

        di->rxin = NEXTRXD(i);

        return (rxp);
    }

    static bool
    m2m_dma64_addrext(osl_t *osh, dma64regs_t *dma64regs)
    {
        uint32 w;
        OR_REG(osh, &dma64regs->control, D64_XC_AE);
        w = R_REG(osh, &dma64regs->control);
        AND_REG(osh, &dma64regs->control, ~D64_XC_AE);
        return ((w & D64_XC_AE) == D64_XC_AE);
    }


#endif
