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
 */

/** @file
 *
 * 43909 WICED OTP Driver
 */

#include "wiced_platform.h"

#include <bcm_cfg.h>
#include <typedefs.h>
#include <bcmdefs.h>
#include <osl.h>
#include <bcmdevs.h>
#include <bcmutils.h>
#include <siutils.h>
#include <bcmendian.h>
#include <hndsoc.h>
#include <sbchipc.h>
#include <sbpcmcia.h>

/*
 * The driver supports the following:
 * 1. New IPX OTP controller
 * 2. ChipCommon revision >= 49
 * 3. 40nm wrapper type layout
 */

#ifndef BCMIPXOTP
#define BCMIPXOTP   1
#endif

#ifndef OTP_READ_DIRECT_ACCESS
#define OTP_READ_DIRECT_ACCESS 1
#endif

/* What device ID should be passed to si_attach? */
#define OTP_DEV_ID  0

#define OTPTYPE_IPX(ccrev)  ((ccrev) == 21 || (ccrev) >= 23)
#define OTPPROWMASK(ccrev)  ((ccrev >= 49) ? OTPP_ROW_MASK9 : OTPP_ROW_MASK)

#define PLATFORM_OTP_BASE   (PLATFORM_GCI_COREBASE(0x1000))

#define OTP_ERR_VAL 0x0001
#define OTP_MSG_VAL 0x0002
#define OTP_DBG_VAL 0x0004
uint32_t otp_msg_level = OTP_ERR_VAL;

#define OTP_PRINT(args) do { if (platform_is_init_completed()) {printf args;} else {wiced_assert("Printing before init completed", 0);} } while(0)

#if defined(OTP_DEBUG) || defined(OTP_DEBUG_ERR)
#define OTP_ERR(args)   do {if (otp_msg_level & OTP_ERR_VAL) OTP_PRINT(args);} while (0)
#else
#define OTP_ERR(args)
#endif

#ifdef OTP_DEBUG
#define OTP_MSG(args)   do {if (otp_msg_level & OTP_MSG_VAL) OTP_PRINT(args);} while (0)
#define OTP_DBG(args)   do {if (otp_msg_level & OTP_DBG_VAL) OTP_PRINT(args);} while (0)
#else
#define OTP_MSG(args)
#define OTP_DBG(args)
#endif

#define OTPP_TRIES          10000000    /* # of tries for OTPP */

#ifdef BCMIPXOTP
#define OTPWSIZE        16  /* word size */

/* OTP layout */
/* Subregion word offsets in General Use region */
#define OTPGU_HSB_OFF       0
#define OTPGU_SFB_OFF       1
#define OTPGU_CI_OFF        2
#define OTPGU_P_OFF         3
#define OTPGU_SROM_OFF      4

/* Flag bit offsets in General Use region  */
#define OTPGU_NEWCISFORMAT_OFF  59
#define OTPGU_HWP_OFF           60
#define OTPGU_SWP_OFF           61
#define OTPGU_CIP_OFF           62
#define OTPGU_FUSEP_OFF         63
#define OTPGU_CIP_MSK           0x4000
#define OTPGU_P_MSK             0xf000
#define OTPGU_P_SHIFT           (OTPGU_HWP_OFF % 16)

/* OTP Size */
#define OTP_SZ_FU_608       ((ROUNDUP(608, 16))/8)
#define OTP_SZ_CHECKSUM     (16/8)      /* 16 bits */

/* Fixed size subregions sizes in words */
#define OTPGU_CI_SZ     2

/* OTP BT shared region (pre-allocated) */
#define OTP_BT_BASE_43909    (0)    /* 43909 does not need OTP BT region */
#define OTP_BT_END_43909     (0)

#define MAXNUMRDES          9    /* Maximum OTP redundancy entries */
#endif /* BCMIPXOTP */

#define OTP_HW_REGION_SDIO_HW_HEADER_SIZE    12

#define PLATFORM_OTP_SECURE_BIT_OFFSET 387

/* OTP common registers */
typedef struct {
    volatile uint32_t *otpstatus;
    volatile uint32_t *otpcontrol;
    volatile uint32_t *otpprog;
    volatile uint32_t *otplayout;
    volatile uint32_t *otpcontrol1;
    volatile uint32_t *otplayoutextension;
} otpregs_t;

/* OTP common function type */
typedef void*    (*otp_init_t)(si_t *sih);
typedef uint32_t (*otp_status_t)(void *oh);
typedef int      (*otp_size_t)(void *oh);
typedef uint16_t (*otp_read_bit_t)(void *oh, unsigned int off);
typedef int      (*otp_read_word_t)(void *oh, unsigned int wn, uint16_t *data);
typedef int      (*otp_read_region_t)(si_t *sih, platform_otp_region_t region, uint16_t *data, unsigned int *wlen);
typedef int      (*otp_get_region_t)(void *oh, platform_otp_region_t region, unsigned int *wbase, unsigned int *wlen);
typedef bool     (*otp_isunified_t)(void *oh);
typedef uint16_t (*otp_avsbitslen_t)(void *oh);
typedef void     (*otp_dump_t)(void *oh);
typedef int      (*otp_lock_t)(si_t *sih);
typedef int      (*otp_nvread_t)(void *oh, char *data, unsigned int *len);
typedef int      (*otp_nvwrite_t)(void *oh, uint16_t *data, unsigned int wlen);
typedef int      (*otp_write_bits_t)(void *oh, int bn, int bits, uint8_t* data);
typedef int      (*otp_write_word_t)(void *oh, unsigned int wn, uint16_t data);
typedef int      (*otp_write_region_t)(void *oh, int region, uint16_t *data, unsigned int wlen, unsigned int flags);
typedef int      (*otp_cis_append_region_t)(si_t *sih, int region, char *vars, int count);

/* OTP function struct */
typedef struct otp_fn_s {
    otp_size_t              size;
    otp_read_bit_t          read_bit;
    otp_dump_t              dump;
    otp_status_t            status;
    otp_init_t              init;
    otp_read_region_t       read_region;
    otp_get_region_t        get_region;
    otp_nvread_t            nvread;
    otp_write_region_t      write_region;
    otp_cis_append_region_t cis_append_region;
    otp_lock_t              lock;
    otp_nvwrite_t           nvwrite;
    otp_write_word_t        write_word;
    otp_read_word_t         read_word;
    otp_write_bits_t        write_bits;
    otp_isunified_t         isunified;
    otp_avsbitslen_t        avsbitslen;
} otp_fn_t;

typedef struct {
    unsigned int  ccrev;            /* chipc revision */
    otp_fn_t      *fn;              /* OTP functions */
    si_t          *sih;             /* Saved sb handle */
    osl_t         *osh;

#ifdef BCMIPXOTP
    /* IPX OTP section */
    uint16_t      wsize;              /* Size of otp in words */
    uint16_t      rows;               /* Geometry */
    uint16_t      cols;               /* Geometry */
    uint32_t      status;             /* Flag bits (lock/prog/rv).
                                       * (Reflected only when OTP is power cycled)
                                       */
    uint16_t      hwbase;             /* hardware subregion offset */
    uint16_t      hwlim;              /* hardware subregion boundary */
    uint16_t      swbase;             /* software subregion offset */
    uint16_t      swlim;              /* software subregion boundary */
    uint16_t      fbase;              /* fuse subregion offset */
    uint16_t      flim;               /* fuse subregion boundary */
    int           otpgu_base;         /* offset to General Use Region */
    uint16_t      fusebits;           /* num of fusebits */
    bool          buotp;              /* Unified OTP flag */
    unsigned int  usbmanfid_offset;   /* Offset of the usb manfid inside the sdio CIS */
    struct {
        uint8_t width;                /* entry width in bits */
        uint8_t val_shift;            /* value bit offset in the entry */
        uint8_t offsets;              /* # entries */
        uint8_t stat_shift;           /* valid bit in otpstatus */
        uint16_t offset[MAXNUMRDES];  /* entry offset in OTP */
    } rde_cb;                         /* OTP redundancy control blocks */
    uint16_t      rde_idx;
#endif /* BCMIPXOTP */

    volatile uint16_t *otpbase;       /* Cache OTP Base address */
    uint16_t      avsbitslen;         /* Number of bits used for AVS in sw region */

    uint16_t arr_cache_word;
    uint16_t arr_cache_word_number;

} otpinfo_t;

typedef struct {
    uint8_t tag;
    void* data;
    uint16_t byte_len;
} otp_read_tag_context_t;

static otpinfo_t otpinfo;

static otpinfo_t *
get_otpinfo(void)
{
    return (otpinfo_t *)&otpinfo;
}

static void
otp_initregs(si_t *sih, void *coreregs, otpregs_t *otpregs)
{
    if (AOB_ENAB(sih))
    {
        otpregs->otpstatus = (volatile uint32_t *)(PLATFORM_GCI_REGBASE(0x310));
        otpregs->otpcontrol = (volatile uint32_t *)(PLATFORM_GCI_REGBASE(0x314));
        otpregs->otpprog = (volatile uint32_t *)(PLATFORM_GCI_REGBASE(0x318));
        otpregs->otplayout = (volatile uint32_t *)(PLATFORM_GCI_REGBASE(0x31C));
        otpregs->otpcontrol1 = (volatile uint32_t *)(PLATFORM_GCI_REGBASE(0x324));
        otpregs->otplayoutextension = (volatile uint32_t *)(PLATFORM_GCI_REGBASE(0x320));
    }
    else
    {
        otpregs->otpstatus = (volatile uint32_t *)(&PLATFORM_CHIPCOMMON->otp.otp_status);
        otpregs->otpcontrol = (volatile uint32_t *)(&PLATFORM_CHIPCOMMON->otp.otp_ctrl);
        otpregs->otpprog = (volatile uint32_t *)(&PLATFORM_CHIPCOMMON->otp.otp_prog);
        otpregs->otplayout = (volatile uint32_t *)(&PLATFORM_CHIPCOMMON->otp.otp_layout);
        otpregs->otpcontrol1 = (volatile uint32_t *)(&PLATFORM_CHIPCOMMON->otp.otp_ctrl_1);
        otpregs->otplayoutextension = (volatile uint32_t *)(&PLATFORM_CHIPCOMMON->otp.otp_layout_extensions);
    }
}

/*
 * IPX OTP Code
 *
 *   Exported functions:
 *  ipxotp_status()
 *  ipxotp_size()
 *  ipxotp_init()
 *  ipxotp_read_bit()
 *  ipxotp_read_region()
 *  ipxotp_read_word()
 *  ipxotp_dump()
 *  ipxotp_isunified()
 *  ipxotp_avsbitslen()
 *
 */

#ifdef BCMIPXOTP
static otp_fn_t* get_ipxotp_fn(void);

static uint32_t
ipxotp_status(void *oh)
{
    otpinfo_t *oi = (otpinfo_t *)oh;
    return oi->status;
}

/** Returns size in bytes */
static int
ipxotp_size(void *oh)
{
    otpinfo_t *oi = (otpinfo_t *)oh;
    return (int)oi->wsize * 2;
}

/** Returns if otp is unified */
static bool
ipxotp_isunified(void *oh)
{
    otpinfo_t *oi = (otpinfo_t *)oh;
    return oi->buotp;
}

/** Returns number of bits used for avs at the end of sw region */
static uint16_t
ipxotp_avsbitslen(void *oh)
{
    otpinfo_t *oi = (otpinfo_t *)oh;
    return oi->avsbitslen;
}

#ifdef OTP_READ_DIRECT_ACCESS
static uint16_t
ipxotp_read_word_direct(void *oh, otpregs_t *otpregs, unsigned int wn)
{
    otpinfo_t *oi;
    uint16_t val;

    (void) otpregs; // Unused

    oi = (otpinfo_t *)oh;

    ASSERT(wn < oi->wsize);

    val = R_REG(oi->osh, &oi->otpbase[wn]);

    return val;
}

static uint16_t
ipxotp_read_bit_direct(void *oh, otpregs_t *otpregs, unsigned int off)
{
    unsigned int word_num, bit_off;
    uint16_t val = 0;
    uint16_t bit = 0;

    word_num = off / OTPWSIZE;
    bit_off = off % OTPWSIZE;

    val = ipxotp_read_word_direct(oh, otpregs, word_num);

    bit = (val >> bit_off) & 0x1;

    return bit;
}
#else
static uint16_t
ipxotp_read_bit_indirect(void *oh, otpregs_t *otpregs, unsigned int off)
{
    otpinfo_t *oi = (otpinfo_t *)oh;
    unsigned int k, row, col;
    uint32_t otpp, st;
    unsigned int otpwt;

    W_REG(oi->osh, otpregs->otpcontrol, 0);

    AND_REG(oi->osh, otpregs->otpcontrol1, (OTPC1_CLK_EN_MASK | OTPC1_CLK_DIV_MASK));

    otpwt = (R_REG(oi->osh, otpregs->otplayout) & OTPL_WRAP_TYPE_MASK) >> OTPL_WRAP_TYPE_SHIFT;

    row = off / oi->cols;
    col = off % oi->cols;

    otpp = OTPP_START_BUSY |
        ((((otpwt == OTPL_WRAP_TYPE_40NM)? OTPPOC_READ_40NM :
        OTPPOC_READ) << OTPP_OC_SHIFT) & OTPP_OC_MASK) |
            ((row << OTPP_ROW_SHIFT) & OTPPROWMASK(oi->sih->ccrev)) |
            ((col << OTPP_COL_SHIFT) & OTPP_COL_MASK);
    OTP_DBG(("%s: off = %u, row = %u, col = %u, otpp = 0x%x",
             __FUNCTION__, off, row, col, (unsigned int)otpp));
    W_REG(oi->osh, otpregs->otpprog, otpp);

    for (k = 0;
         ((st = R_REG(oi->osh, otpregs->otpprog)) & OTPP_START_BUSY) && (k < OTPP_TRIES);
         k ++)
        ;
    if (k >= OTPP_TRIES) {
        OTP_ERR(("\n%s: BUSY stuck: st=0x%x, count=%u\n", __FUNCTION__, (unsigned int)st, k));
        return 0xffff;
    }
    if (st & OTPP_READERR) {
        OTP_ERR(("\n%s: Could not read OTP bit %u\n", __FUNCTION__, off));
        return 0xffff;
    }
    st = (st & OTPP_VALUE_MASK) >> OTPP_VALUE_SHIFT;

    OTP_DBG((" => %lu\n", st));

    return (uint16_t)st;
}

static uint16_t
ipxotp_read_word_indirect(void *oh, otpregs_t *otpregs, unsigned int wn)
{
    unsigned int base, i;
    uint16_t val;
    uint16_t bit;

    base = wn * OTPWSIZE;
    val = 0;

    for (i = 0; i < OTPWSIZE; i++) {
        if ((bit = ipxotp_read_bit_indirect(oh, otpregs, base + i)) == 0xffff) {
            break;
        }
        val = val | (bit << i);
    }

    if (i < OTPWSIZE) {
        val = 0xffff;
    }

    return val;
}
#endif

static uint16_t
ipxotp_read_bit_common(void *oh, otpregs_t *otpregs, unsigned int off)
{
    uint16_t bit;

#ifdef OTP_READ_DIRECT_ACCESS
    bit = ipxotp_read_bit_direct(oh, otpregs, off);
#else
    bit = ipxotp_read_bit_indirect(oh, otpregs, off);
#endif

    return bit;
}

static uint16_t
ipxotp_read_bit(void *oh, unsigned int off)
{
    otpinfo_t *oi = (otpinfo_t *)oh;
    si_t *sih = oi->sih;
    unsigned int idx;
    void *regs;
    otpregs_t otpregs;
    uint16_t val16;

    idx = si_coreidx(sih);
    if (AOB_ENAB(sih)) {
        regs = si_setcore(sih, GCI_CORE_ID, 0);
    } else {
        regs = si_setcoreidx(sih, SI_CC_IDX);
    }
    ASSERT(regs != NULL);
    otp_initregs(sih, regs, &otpregs);

    val16 = ipxotp_read_bit_common(oh, &otpregs, off);

    si_setcoreidx(sih, idx);
    return (val16);
}

/** OTP BT region size */
static void
ipxotp_bt_region_get(otpinfo_t *oi, uint16_t *start, uint16_t *end)
{
    *start = *end = 0;
    switch (CHIPID(oi->sih->chip)) {
    case BCM43909_CHIP_ID:
        *start = OTP_BT_BASE_43909;
        *end = OTP_BT_END_43909;
        break;
    }
}

/**
 * Calculate max HW/SW region byte size by subtracting fuse region and checksum size,
 * osizew is oi->wsize (OTP size - GU size) in words.
 */
static int
ipxotp_max_rgnsz(otpinfo_t *oi)
{
    int osizew = oi->wsize;
    int ret = 0;
    uint16_t checksum;
    si_t *sih = oi->sih;
    unsigned int idx;
    void *regs;
    otpregs_t otpregs;

    idx = si_coreidx(sih);
    if (AOB_ENAB(sih)) {
        regs = si_setcore(sih, GCI_CORE_ID, 0);
    } else {
        regs = si_setcoreidx(sih, SI_CC_IDX);
    }
    ASSERT(regs != NULL);
    otp_initregs(sih, regs, &otpregs);

    checksum = OTP_SZ_CHECKSUM;

    /* for new chips, fusebit is available from cc register */
    oi->fusebits = R_REG(oi->osh, otpregs.otplayoutextension)
                    & OTPLAYOUTEXT_FUSE_MASK;
    oi->fusebits = ROUNDUP(oi->fusebits, 8);
    oi->fusebits >>= 3; /* bytes */

    si_setcoreidx(sih, idx);

    switch (CHIPID(sih->chip)) {
    case BCM43909_CHIP_ID:
        oi->fusebits = OTP_SZ_FU_608;
        break;
    default:
        if (oi->fusebits == 0)
            ASSERT(0);  /* Don't know about this chip */
    }

    ret = osizew*2 - oi->fusebits - checksum;

    OTP_MSG(("max region size %d bytes\n", ret));
    return ret;
}

/**  OTP sizes for 40nm */
static int
ipxotp_otpsize_set_40nm(otpinfo_t *oi, unsigned int otpsz)
{
    /* Check for otp size */
    switch (otpsz) {
    case 1: /* 64x32: 2048 bits */
        oi->rows = 64;
        oi->cols = 32;
        break;
    case 2: /* 96x32: 3072 bits */
        oi->rows = 96;
        oi->cols = 32;
        break;
    case 3: /* 128x32: 4096 bits */
        oi->rows = 128;
        oi->cols = 32;
        break;
    case 4: /* 160x32: 5120 bits */
        oi->rows = 160;
        oi->cols = 32;
        break;
    case 5: /* 192x32: 6144 bits */
        oi->rows = 192;
        oi->cols = 32;
        break;
    case 7: /* 256x32: 8192 bits */
        oi->rows = 256;
        oi->cols = 32;
        break;
    case 11: /* 384x32: 12288 bits */
        oi->rows = 384;
        oi->cols = 32;
        break;
    default:
        /* Don't know the geometry */
        OTP_ERR(("%s: unknown OTP geometry\n", __FUNCTION__));
    }

    oi->wsize = (oi->cols * oi->rows)/OTPWSIZE;
    return 0;
}

static uint16_t
ipxotp_read_word_common(void *oh, otpregs_t *otpregs, unsigned int wn)
{
    uint16_t word;

#ifdef OTP_READ_DIRECT_ACCESS
    word = ipxotp_read_word_direct(oh, otpregs, wn);
#else
    word = ipxotp_read_word_indirect(oh, otpregs, wn);
#endif

    return word;
}

static int
ipxotp_read_word(void *oh, unsigned int wn, uint16_t *data)
{
    otpinfo_t *oi = (otpinfo_t *)oh;
    unsigned int idx;
    void *regs;
    otpregs_t otpregs;
    si_t *sih = oi->sih;

    idx = si_coreidx(sih);
    if (AOB_ENAB(sih)) {
        regs = si_setcore(sih, GCI_CORE_ID, 0);
    } else {
        regs = si_setcoreidx(sih, SI_CC_IDX);
    }
    ASSERT(regs != NULL);
    otp_initregs(sih, regs, &otpregs);

    /* Read the data */
    *data = ipxotp_read_word_common(oh, &otpregs, wn);

    si_setcoreidx(sih, idx);
    return 0;
}

static int
ipxotp_get_region(void *oh, platform_otp_region_t region, unsigned int *wbase, unsigned int *wlen)
{
    otpinfo_t *oi = (otpinfo_t *)oh;

    /* Validate region selection */
    switch (region) {
    case PLATFORM_OTP_HW_RGN:
        /* OTP unification: For unified OTP sz=flim-hwbase */
        if (oi->buotp)
            *wlen = (uint)oi->flim - oi->hwbase;
        else
            *wlen = (uint)oi->hwlim - oi->hwbase;
        if (!(oi->status & OTPS_GUP_HW)) {
            OTP_ERR(("%s: h/w region not programmed\n", __FUNCTION__));
            return BCME_NOTFOUND;
        }
        *wbase = oi->hwbase;
        break;
    case PLATFORM_OTP_SW_RGN:
        /* OTP unification: For unified OTP sz=flim-swbase */
        if (oi->buotp)
            *wlen = ((uint)oi->flim - oi->swbase);
        else
            *wlen = ((uint)oi->swlim - oi->swbase);
        if (!(oi->status & OTPS_GUP_SW)) {
            OTP_ERR(("%s: s/w region not programmed\n", __FUNCTION__));
            return BCME_NOTFOUND;
        }
        *wbase = oi->swbase;
        break;
    case PLATFORM_OTP_CI_RGN:
        *wlen = OTPGU_CI_SZ;
        if (!(oi->status & OTPS_GUP_CI)) {
            OTP_ERR(("%s: chipid region not programmed\n", __FUNCTION__));
            return BCME_NOTFOUND;
        }
        *wbase = oi->otpgu_base + OTPGU_CI_OFF;
        break;
    case PLATFORM_OTP_FUSE_RGN:
        *wlen = (uint)oi->flim - oi->fbase;
        if (!(oi->status & OTPS_GUP_FUSE)) {
            OTP_ERR(("%s: fuse region not programmed\n", __FUNCTION__));
            return BCME_NOTFOUND;
        }
        *wbase = oi->fbase;
        break;
    case PLATFORM_OTP_ALL_RGN:
        *wlen = ((uint)oi->flim - oi->hwbase);
        if (!(oi->status & (OTPS_GUP_HW | OTPS_GUP_SW))) {
            OTP_ERR(("%s: h/w & s/w region not programmed\n", __FUNCTION__));
            return BCME_NOTFOUND;
        }
        *wbase = oi->hwbase;
        break;
    default:
        OTP_ERR(("%s: reading region %d is not supported\n", __FUNCTION__, region));
        return BCME_BADARG;
    }

    return 0;
}

static int
ipxotp_read_region(void *oh, platform_otp_region_t region, uint16_t *data, unsigned int *wlen)
{
    otpinfo_t *oi = (otpinfo_t *)oh;
    unsigned int idx;
    unsigned int wbase = 0, i, sz = 0;
    si_t *sih = oi->sih;
    void *regs;
    otpregs_t otpregs;
    int res;

    res = ipxotp_get_region(oh, region, &wbase, &sz);
    if (res != 0) {
        *wlen = sz;
        return res;
    }
    if (*wlen < sz) {
        OTP_ERR(("%s: buffer too small, should be at least %u\n",
                 __FUNCTION__, sz));
        *wlen = sz;
        return BCME_BUFTOOSHORT;
    }
    *wlen = sz;

    idx = si_coreidx(sih);
    if (AOB_ENAB(sih)) {
        regs = si_setcore(sih, GCI_CORE_ID, 0);
    } else {
        regs = si_setcoreidx(sih, SI_CC_IDX);
    }
    ASSERT(regs != NULL);
    otp_initregs(sih, regs, &otpregs);

    /* Read the data */
    for (i = 0; i < sz; i ++)
        data[i] = ipxotp_read_word_common(oh, &otpregs, wbase + i);

    si_setcoreidx(sih, idx);
    *wlen = sz;
    return 0;
}

static void
_ipxotp_init(otpinfo_t *oi, otpregs_t *otpregs)
{
    uint16_t btsz, btbase = 0, btend = 0;

    /* record word offset of General Use Region */
    oi->otpgu_base = (R_REG(oi->osh, otpregs->otplayout) & OTPL_HWRGN_OFF_MASK)
                    >> OTPL_HWRGN_OFF_SHIFT;
    ASSERT((oi->otpgu_base - (OTPGU_SROM_OFF * OTPWSIZE)) > 0);
    oi->otpgu_base >>= 4; /* words */
    oi->otpgu_base -= OTPGU_SROM_OFF;

    /* Read OTP lock bits and subregion programmed indication bits */
    oi->status = R_REG(oi->osh, otpregs->otpstatus);

    /* WAR for PR 65487:
     * OTP status is not updated before power-cycle, so we need
     * to read the subregion programmed bit from OTP directly
     */
    if ((CHIPID(oi->sih->chip) == BCM43909_CHIP_ID) || 0) {
        uint32_t p_bits;
        p_bits = (ipxotp_read_word_common(oi, otpregs, oi->otpgu_base + OTPGU_P_OFF) & OTPGU_P_MSK)
            >> OTPGU_P_SHIFT;
        oi->status |= (p_bits << OTPS_GUP_SHIFT);
    }
    OTP_DBG(("%s: status 0x%x\n", __FUNCTION__, (unsigned int)oi->status));

    /* OTP unification */
    oi->buotp = FALSE; /* Initialize it to false, until its explicitely set true. */
    if ((oi->status & (OTPS_GUP_HW | OTPS_GUP_SW)) == (OTPS_GUP_HW | OTPS_GUP_SW)) {
        switch (CHIPID(oi->sih->chip)) {
            /* Add cases for supporting chips */
            default:
                OTP_ERR(("chip=0x%x does not support Unified OTP.\n",
                    CHIPID(oi->sih->chip)));
                break;
        }
    }

    /* if AVS is part of s/w region, update how many bits are used for AVS */
    switch (CHIPID(oi->sih->chip)) {
        case BCM43909_CHIP_ID:
            oi->avsbitslen = 0;
            break;
        default:
            oi->avsbitslen = 0;
            break;
    }

    /*
     * h/w region base and fuse region limit are fixed to the top and
     * the bottom of the general use region. Everything else can be flexible.
     */
    oi->hwbase = oi->otpgu_base + OTPGU_SROM_OFF;
    oi->hwlim = oi->wsize;
    oi->flim = oi->wsize;

    ipxotp_bt_region_get(oi, &btbase, &btend);
    btsz = btend - btbase;
    if (btsz > 0) {
        /* default to not exceed BT base */
        oi->hwlim = btbase;

        /* With BT shared region, swlim and fbase are fixed */
        oi->swlim = btbase;
        oi->fbase = btend;
        /* if avs bits are part of swregion, subtract that from the sw/hw limit */
        oi->hwlim -= oi->avsbitslen;
        oi->swlim -= oi->avsbitslen;
    }

    /* Update hwlim and swbase */
    if (oi->status & OTPS_GUP_HW) {
        uint16_t swbase;
        OTP_DBG(("%s: hw region programmed\n", __FUNCTION__));
        swbase = ipxotp_read_word_common(oi, otpregs, oi->otpgu_base + OTPGU_HSB_OFF) / 16;
        if (swbase) {
            oi->hwlim =  swbase;
        }
        oi->swbase = oi->hwlim;
    } else
        oi->swbase = oi->hwbase;

    /* Update swlim and fbase only if no BT region */
    if (btsz == 0) {
        /* subtract fuse and checksum from beginning */
        oi->swlim = ipxotp_max_rgnsz(oi) / 2;

        if (oi->status & OTPS_GUP_SW) {
            OTP_DBG(("%s: sw region programmed\n", __FUNCTION__));
            oi->swlim = ipxotp_read_word_common(oi, otpregs, oi->otpgu_base + OTPGU_SFB_OFF) / 16;
            oi->fbase = oi->swlim;
        }
        else
            oi->fbase = oi->swbase;
        /* if avs bits are part of swregion, subtract that from the sw limit */
        oi->swlim -= oi->avsbitslen;
    }

    OTP_DBG(("%s: OTP limits---\n"
        "hwbase %d/%d hwlim %d/%d\n"
        "swbase %d/%d swlim %d/%d\n"
        "fbase %d/%d flim %d/%d\n", __FUNCTION__,
        oi->hwbase, oi->hwbase * 16, oi->hwlim, oi->hwlim * 16,
        oi->swbase, oi->swbase * 16, oi->swlim, oi->swlim * 16,
        oi->fbase, oi->fbase * 16, oi->flim, oi->flim * 16));
}

static void *
ipxotp_init(si_t *sih)
{
    unsigned int idx, otpsz, otpwt;
    void *regs;
    otpregs_t otpregs;
    otpinfo_t *oi = NULL;

    OTP_MSG(("%s: Use IPX OTP controller\n", __FUNCTION__));

    /* Make sure we're running IPX OTP */
    ASSERT(OTPTYPE_IPX(sih->ccrev));
    if (!OTPTYPE_IPX(sih->ccrev))
        return NULL;

    /* Make sure chipcommon rev >= 49 */
    if (sih->ccrev < 49) {
        OTP_ERR(("%s: unsupported OTP revision\n", __FUNCTION__));
        return NULL;
    }

    /* Make sure OTP is not disabled */
    if (si_is_otp_disabled(sih)) {
        OTP_MSG(("%s: OTP is disabled\n", __FUNCTION__));
        return NULL;
    }

    /* Make sure OTP is powered up */
    if (!si_is_otp_powered(sih)) {
        OTP_ERR(("%s: OTP is powered down\n", __FUNCTION__));
        return NULL;
    }

    /* Retrieve OTP region info */
    idx = si_coreidx(sih);
    if (AOB_ENAB(sih)) {
        regs = si_setcore(sih, GCI_CORE_ID, 0);
    } else {
        regs = si_setcoreidx(sih, SI_CC_IDX);
    }
    ASSERT(regs != NULL);
    otp_initregs(sih, regs, &otpregs);

    oi = get_otpinfo();

    otpsz = (R_REG(oi->osh, otpregs.otplayout) & OTPL_ROW_SIZE_MASK) >> OTPL_ROW_SIZE_SHIFT;

    if (otpsz == 0) {
        OTP_ERR(("%s: Unsupported OTP\n", __FUNCTION__));
        oi = NULL;
        goto exit;
    }

    otpwt = (R_REG(oi->osh, otpregs.otplayout) & OTPL_WRAP_TYPE_MASK) >> OTPL_WRAP_TYPE_SHIFT;

    if (otpwt == OTPL_WRAP_TYPE_40NM) {
        ipxotp_otpsize_set_40nm(oi, otpsz);
    } else {
        OTP_ERR(("%s: Unsupported wrap type: %d\n", __FUNCTION__, otpwt));
        oi = NULL;
        goto exit;
    }

    OTP_MSG(("%s: rows %u cols %u wsize %u\n", __FUNCTION__, oi->rows, oi->cols, oi->wsize));

    if (AOB_ENAB(sih)) {
        uint32_t otpaddr;
        otpaddr = PLATFORM_OTP_BASE;
        oi->otpbase = (uint16_t *)REG_MAP(otpaddr, SI_CORE_SIZE);
        OTP_MSG(("%s: mapping otpbase at 0x%08x to 0x%p\n", __FUNCTION__, (unsigned int)otpaddr, oi->otpbase));
    } else {
        unsigned int idx2;
        /* Take offset of OTP Base address from GCI CORE */
        idx2 = si_coreidx(sih);
        oi->otpbase = (uint16_t *)si_setcore(sih, GCI_CORE_ID, 0);
        /* Reset si handler curidx to CC */
        si_setcoreidx(sih, idx2);
    }

    _ipxotp_init(oi, &otpregs);

exit:
    si_setcoreidx(sih, idx);

    return (void *)oi;
}

#ifdef OTP_DEBUG
static void
ipxotp_dump(void *oh)
{
    otpinfo_t *oi = (otpinfo_t *)oh;
    si_t *sih = oi->sih;
    void *regs;
    otpregs_t otpregs;
    unsigned int idx, i, count;
    uint16_t val;

    idx = si_coreidx(sih);
    if (AOB_ENAB(sih)) {
        regs = si_setcore(sih, GCI_CORE_ID, 0);
    } else {
        regs = si_setcoreidx(sih, SI_CC_IDX);
    }
    otp_initregs(sih, regs, &otpregs);

    count = ipxotp_size(oh);

    for (i = 0; i < count / 2; i++) {
        if (!(i % 4))
            OTP_ERR(("\n0x%04x:", 2 * i));
        val = ipxotp_read_word_common(oi, &otpregs, i);
        OTP_ERR((" 0x%04x", val));
    }
    OTP_ERR(("\n"));

    si_setcoreidx(oi->sih, idx);
}
#endif /* OTP_DEBUG */

static otp_fn_t ipxotp_fn = {
    (otp_size_t)ipxotp_size,
    (otp_read_bit_t)ipxotp_read_bit,
    (otp_dump_t)NULL,
    (otp_status_t)ipxotp_status,
    (otp_init_t)ipxotp_init,
    (otp_read_region_t)ipxotp_read_region,
    (otp_get_region_t)ipxotp_get_region,
    (otp_nvread_t)NULL,
    (otp_write_region_t)NULL,
    (otp_cis_append_region_t)NULL,
    (otp_lock_t)NULL,
    (otp_nvwrite_t)NULL,
    (otp_write_word_t)NULL,
    (otp_read_word_t)ipxotp_read_word,
    (otp_write_bits_t)NULL,
    (otp_isunified_t)ipxotp_isunified,
    (otp_avsbitslen_t)ipxotp_avsbitslen
};

static otp_fn_t*
get_ipxotp_fn(void)
{
    return &ipxotp_fn;
}
#endif /* BCMIPXOTP */

static host_semaphore_type_t*
platform_otp_init_sem( void )
{
    static wiced_bool_t done = WICED_FALSE;
    static host_semaphore_type_t sem;
    uint32_t flags;

    WICED_SAVE_INTERRUPTS(flags);

    if (done != WICED_TRUE)
    {
        host_rtos_init_semaphore(&sem);
        host_rtos_set_semaphore(&sem, WICED_FALSE);
        done = WICED_TRUE;
    }

    WICED_RESTORE_INTERRUPTS(flags);

    return &sem;
}

static platform_result_t
platform_otp_init_internal(void)
{
    si_t * sih = NULL;
    otpinfo_t *oi = NULL;
    void *ret = NULL;

    oi = get_otpinfo();

    bzero(oi, sizeof(otpinfo_t));

    /* Get SI handle */
    sih = si_kattach(oi->osh);
    if (sih == NULL)
    {
        OTP_ERR(("%s: Cannot get SI handle\n", __FUNCTION__));
        return PLATFORM_ERROR;
    }

    oi->ccrev = sih->ccrev;

#ifdef BCMIPXOTP
    if (OTPTYPE_IPX(oi->ccrev))
    {
#ifdef OTP_DEBUG
        get_ipxotp_fn()->dump = ipxotp_dump;
#endif /* OTP_DEBUG */
        oi->fn = get_ipxotp_fn();
    }
#endif /* BCMIPXOTP */

    if (oi->fn == NULL)
    {
        bzero(oi, sizeof(otpinfo_t));

        OTP_ERR(("%s: unsupported OTP type\n", __FUNCTION__));
        return PLATFORM_ERROR;
    }

    oi->sih = sih;
    oi->osh = si_osh(oi->sih);

    /* 43909 OTP is always powered up */
    if (!si_is_otp_powered(sih))
    {
        bzero(oi, sizeof(otpinfo_t));

        OTP_ERR(("%s: OTP not powered on\n", __FUNCTION__));
        return PLATFORM_ERROR;
    }

    ret = (oi->fn->init)(sih);

    if (ret == NULL)
    {
        bzero(oi, sizeof(otpinfo_t));

        OTP_ERR(("%s: OTP initialization error\n", __FUNCTION__));
        return PLATFORM_ERROR;
    }

    return PLATFORM_SUCCESS;
}

static platform_result_t
platform_otp_status_internal(uint32_t *status)
{
    otpinfo_t *oi = get_otpinfo();

    if ((oi->sih == NULL) || (oi->fn == NULL))
    {
        OTP_ERR(("%s: OTP not initialized\n", __FUNCTION__));
        return PLATFORM_ERROR;
    }

    if (oi->fn->status == NULL)
    {
        OTP_ERR(("%s: Unsupported OTP operation\n", __FUNCTION__));
        return PLATFORM_ERROR;
    }

    *status = oi->fn->status((void *)oi);

    return PLATFORM_SUCCESS;
}

static platform_result_t
platform_otp_size_internal(uint16_t *size)
{
    otpinfo_t *oi = get_otpinfo();

    if ((oi->sih == NULL) || (oi->fn == NULL))
    {
        OTP_ERR(("%s: OTP not initialized\n", __FUNCTION__));
        return PLATFORM_ERROR;
    }

    if (oi->fn->size == NULL)
    {
        OTP_ERR(("%s: Unsupported OTP operation\n", __FUNCTION__));
        return PLATFORM_ERROR;
    }

    *size = oi->fn->size((void *)oi);

    return PLATFORM_SUCCESS;
}

static platform_result_t
platform_otp_read_bit_internal(uint16_t bit_number, uint16_t *read_bit)
{
    otpinfo_t *oi = get_otpinfo();

    if ((oi->sih == NULL) || (oi->fn == NULL))
    {
        OTP_ERR(("%s: OTP not initialized\n", __FUNCTION__));
        return PLATFORM_ERROR;
    }

    if (oi->fn->read_bit == NULL)
    {
        OTP_ERR(("%s: Unsupported OTP operation\n", __FUNCTION__));
        return PLATFORM_ERROR;
    }

    *read_bit = (uint16_t)oi->fn->read_bit((void *)oi, bit_number);

    if (*read_bit == 0xffff)
    {
        return PLATFORM_ERROR;
    }

    return PLATFORM_SUCCESS;
}

static platform_result_t
platform_otp_read_word_internal(uint16_t word_number, uint16_t *read_word)
{
    otpinfo_t *oi;
    int err = 0;

    oi = get_otpinfo();

    if ((oi->sih == NULL) || (oi->fn == NULL))
    {
        OTP_ERR(("%s: OTP not initialized\n", __FUNCTION__));
        return PLATFORM_ERROR;
    }

    if (oi->fn->read_word == NULL)
    {
        OTP_ERR(("%s: Unsupported OTP operation\n", __FUNCTION__));
        return PLATFORM_ERROR;
    }

    err = (oi->fn->read_word)((void *)oi, word_number, read_word);

    if (err != 0)
    {
        return PLATFORM_ERROR;
    }

    return PLATFORM_SUCCESS;
}

static platform_result_t
platform_otp_read_array_internal(uint16_t byte_number, void* data, uint16_t byte_len)
{
    otpinfo_t *oi = get_otpinfo();
    uint8_t *p = data;
    platform_result_t result = PLATFORM_SUCCESS;

    while (byte_len)
    {
        uint16_t word_number = byte_number / 2;
        uint16_t word = oi->arr_cache_word;

        if (oi->arr_cache_word_number != word_number)
        {
            result = platform_otp_read_word_internal(word_number, &word);
            if (result != PLATFORM_SUCCESS)
            {
                break;
            }

            oi->arr_cache_word = word;
            oi->arr_cache_word_number = word_number;
        }

        if ((byte_number & 0x1) == 0)
        {
            *p++ = (uint8_t)(word & 0xFF);
            byte_number += 1;
            byte_len -= 1;
        }

        if (byte_len)
        {
            *p++ = (uint8_t)((word >> 8) & 0xFF);
            byte_number += 1;
            byte_len -= 1;
        }
    }

    return result;
}

static platform_result_t
platform_otp_get_region_internal(platform_otp_region_t region, uint16_t *word_number, uint16_t *word_len)
{
    otpinfo_t *oi;
    unsigned int wnumber = 0;
    unsigned int wlen = 0;
    int err = 0;

    oi = get_otpinfo();

    if ((oi->sih == NULL) || (oi->fn == NULL))
    {
        OTP_ERR(("%s: OTP not initialized\n", __FUNCTION__));
        return PLATFORM_ERROR;
    }

    if (oi->fn->get_region == NULL)
    {
        OTP_ERR(("%s: Unsupported OTP operation\n", __FUNCTION__));
        return PLATFORM_ERROR;
    }

    err = (oi->fn->get_region)((void *)oi, region, &wnumber, &wlen);
    if (err != 0)
    {
        return PLATFORM_ERROR;
    }

    *word_number = wnumber;
    *word_len = wlen;

    return PLATFORM_SUCCESS;
}

static platform_result_t
platform_otp_read_region_internal(platform_otp_region_t region, uint16_t *data, uint16_t *word_len)
{
    otpinfo_t *oi;
    int err = 0;
    unsigned int wlen = *word_len;

    oi = get_otpinfo();

    if ((oi->sih == NULL) || (oi->fn == NULL))
    {
        OTP_ERR(("%s: OTP not initialized\n", __FUNCTION__));
        return PLATFORM_ERROR;
    }

    if (oi->fn->read_region == NULL)
    {
        OTP_ERR(("%s: Unsupported OTP operation\n", __FUNCTION__));
        return PLATFORM_ERROR;
    }

    err = (oi->fn->read_region)((void *)oi, region, data, &wlen);

    *word_len = wlen;

    if (err == BCME_BUFTOOSHORT)
    {
        return PLATFORM_BADARG;
    }
    else if (err != 0)
    {
        return PLATFORM_ERROR;
    }

    return PLATFORM_SUCCESS;
}

static platform_result_t
platform_otp_newcis_internal(uint16_t *newcis_bit)
{
    platform_result_t ret = PLATFORM_ERROR;

#ifdef BCMIPXOTP
    otpinfo_t *oi = get_otpinfo();

    if ((oi->sih == NULL) || (oi->fn == NULL))
    {
        OTP_ERR(("%s: OTP not initialized\n", __FUNCTION__));
        return PLATFORM_ERROR;
    }

    if (oi->fn->read_bit == NULL)
    {
        OTP_ERR(("%s: Unsupported OTP operation\n", __FUNCTION__));
        return PLATFORM_ERROR;
    }

    int otpgu_bit_base = oi->otpgu_base * 16;

    ret = platform_otp_read_bit(otpgu_bit_base + OTPGU_NEWCISFORMAT_OFF, newcis_bit);

    OTP_MSG(("New Cis format bit %d value: %x\n", otpgu_bit_base + OTPGU_NEWCISFORMAT_OFF, *newcis_bit));
#endif

    return ret;
}

static platform_result_t
platform_otp_isunified_internal(wiced_bool_t *is_unified)
{
    otpinfo_t *oi = get_otpinfo();
    bool otp_is_unified = FALSE;

    if ((oi->sih == NULL) || (oi->fn == NULL))
    {
        OTP_ERR(("%s: OTP not initialized\n", __FUNCTION__));
        return PLATFORM_ERROR;
    }

    if (oi->fn->isunified == NULL)
    {
        OTP_ERR(("%s: Unsupported OTP operation\n", __FUNCTION__));
        return PLATFORM_ERROR;
    }

    otp_is_unified = oi->fn->isunified((void *)oi);

    if (otp_is_unified == TRUE)
    {
        *is_unified = WICED_TRUE;
    }
    else
    {
        *is_unified = WICED_FALSE;
    }

    return PLATFORM_SUCCESS;
}

static platform_result_t
platform_otp_avsbitslen_internal(uint16_t *avsbitslen)
{
    otpinfo_t *oi = get_otpinfo();

    if ((oi->sih == NULL) || (oi->fn == NULL))
    {
        OTP_ERR(("%s: OTP not initialized\n", __FUNCTION__));
        return PLATFORM_ERROR;
    }

    if (oi->fn->avsbitslen == NULL)
    {
        OTP_ERR(("%s: Unsupported OTP operation\n", __FUNCTION__));
        return PLATFORM_ERROR;
    }

    *avsbitslen = oi->fn->avsbitslen((void *)oi);

    return PLATFORM_SUCCESS;
}

/*
 * Common Code:
 *  platform_otp_init()
 *  platform_otp_status()
 *  platform_otp_size()
 *  platform_otp_read_bit()
 *  platform_otp_read_word()
 *  platform_otp_read_region()
 *  platform_otp_newcis()
 *  platform_otp_isunified()
 *  platform_otp_avsbitslen()
 *
 *  platform_otp_dump()
 *  platform_otp_dumpstats()
 */

static platform_result_t
platform_otp_init_unsafe(void)
{
    static wiced_bool_t done = WICED_FALSE;
    platform_result_t ret = PLATFORM_SUCCESS;

    if (done != WICED_TRUE)
    {
        ret = platform_otp_init_internal();
        if (ret == PLATFORM_SUCCESS)
        {
            done = WICED_TRUE;
        }
    }

    get_otpinfo()->arr_cache_word_number = 1; /* any odd number */

    return ret;
}

platform_result_t
platform_otp_init(void)
{
    host_semaphore_type_t* sem = platform_otp_init_sem();
    platform_result_t ret = PLATFORM_SUCCESS;

    host_rtos_get_semaphore(sem, NEVER_TIMEOUT, WICED_FALSE);

    ret = platform_otp_init_unsafe();

    host_rtos_set_semaphore(sem, WICED_FALSE);

    return ret;
}

platform_result_t
platform_otp_status(uint32_t *status)
{
    host_semaphore_type_t* sem = platform_otp_init_sem();
    platform_result_t ret;

    host_rtos_get_semaphore(sem, NEVER_TIMEOUT, WICED_FALSE);

    ret = platform_otp_status_internal(status);

    host_rtos_set_semaphore(sem, WICED_FALSE);

    return ret;
}

platform_result_t
platform_otp_size(uint16_t *size)
{
    host_semaphore_type_t* sem = platform_otp_init_sem();
    platform_result_t ret;

    host_rtos_get_semaphore(sem, NEVER_TIMEOUT, WICED_FALSE);

    ret = platform_otp_size_internal(size);

    host_rtos_set_semaphore(sem, WICED_FALSE);

    return ret;
}

platform_result_t
platform_otp_read_bit(uint16_t bit_number, uint16_t *read_bit)
{
    host_semaphore_type_t* sem = platform_otp_init_sem();
    platform_result_t ret;

    host_rtos_get_semaphore(sem, NEVER_TIMEOUT, WICED_FALSE);

    ret = platform_otp_read_bit_internal(bit_number, read_bit);

    host_rtos_set_semaphore(sem, WICED_FALSE);

    return ret;
}

platform_result_t
platform_otp_read_word(uint16_t word_number, uint16_t *read_word)
{
    host_semaphore_type_t* sem = platform_otp_init_sem();
    platform_result_t ret;

    host_rtos_get_semaphore(sem, NEVER_TIMEOUT, WICED_FALSE);

    ret = platform_otp_read_word_internal(word_number, read_word);

    host_rtos_set_semaphore(sem, WICED_FALSE);

    return ret;
}

platform_result_t
platform_otp_read_array(uint16_t byte_number, void* data, uint16_t byte_len)
{
    host_semaphore_type_t* sem = platform_otp_init_sem();
    platform_result_t ret;

    host_rtos_get_semaphore(sem, NEVER_TIMEOUT, WICED_FALSE);

    ret = platform_otp_read_array_internal(byte_number, data, byte_len);

    host_rtos_set_semaphore(sem, WICED_FALSE);

    return ret;
}

platform_result_t
platform_otp_get_region(platform_otp_region_t region, uint16_t *word_number, uint16_t *word_len)
{
    host_semaphore_type_t* sem = platform_otp_init_sem();
    platform_result_t ret;

    host_rtos_get_semaphore(sem, NEVER_TIMEOUT, WICED_FALSE);

    ret = platform_otp_get_region_internal(region, word_number, word_len);

    host_rtos_set_semaphore(sem, WICED_FALSE);

    return ret;
}

platform_result_t
platform_otp_read_region(platform_otp_region_t region, uint16_t *data, uint16_t *word_len)
{
    host_semaphore_type_t* sem = platform_otp_init_sem();
    platform_result_t ret;

    host_rtos_get_semaphore(sem, NEVER_TIMEOUT, WICED_FALSE);

    ret = platform_otp_read_region_internal(region, data, word_len);

    host_rtos_set_semaphore(sem, WICED_FALSE);

    return ret;
}

platform_result_t
platform_otp_newcis(uint16_t *newcis_bit)
{
    host_semaphore_type_t* sem = platform_otp_init_sem();
    platform_result_t ret;

    host_rtos_get_semaphore(sem, NEVER_TIMEOUT, WICED_FALSE);

    ret = platform_otp_newcis_internal(newcis_bit);

    host_rtos_set_semaphore(sem, WICED_FALSE);

    return ret;
}

platform_result_t
platform_otp_isunified(wiced_bool_t *is_unified)
{
    host_semaphore_type_t* sem = platform_otp_init_sem();
    platform_result_t ret;

    host_rtos_get_semaphore(sem, NEVER_TIMEOUT, WICED_FALSE);

    ret = platform_otp_isunified_internal(is_unified);

    host_rtos_set_semaphore(sem, WICED_FALSE);

    return ret;
}

platform_result_t
platform_otp_avsbitslen(uint16_t *avsbitslen)
{
    host_semaphore_type_t* sem = platform_otp_init_sem();
    platform_result_t ret;

    host_rtos_get_semaphore(sem, NEVER_TIMEOUT, WICED_FALSE);

    ret = platform_otp_avsbitslen_internal(avsbitslen);

    host_rtos_set_semaphore(sem, WICED_FALSE);

    return ret;
}

platform_result_t
platform_otp_cis_parse(platform_otp_region_t region, platform_otp_cis_parse_callback_func callback, void *context)
{
    uint16_t word_number;
    uint16_t word_len;
    uint16_t byte_number;
    uint16_t byte_len;
    platform_result_t result;

    result = platform_otp_get_region(region, &word_number, &word_len);
    if (result != PLATFORM_SUCCESS)
    {
        return result;
    }

    byte_number = 2 * word_number;
    byte_len = 2 * word_len;

    if (region == PLATFORM_OTP_HW_RGN)
    {
        if (byte_len < OTP_HW_REGION_SDIO_HW_HEADER_SIZE)
        {
            return PLATFORM_ERROR;
        }
        byte_number += OTP_HW_REGION_SDIO_HW_HEADER_SIZE;
        byte_len -= OTP_HW_REGION_SDIO_HW_HEADER_SIZE;
    }

    while (byte_len)
    {
        uint8_t tag;
        uint8_t tag_len;
        uint8_t brcm_tag = 0;

        result = platform_otp_read_array(byte_number, &tag, 1);
        if (result != PLATFORM_SUCCESS)
        {
            return result;
        }
        byte_number++;
        byte_len--;

        if (tag == CISTPL_NULL)
        {
            continue;
        }

        if (tag == CISTPL_END)
        {
            break;
        }

        if (byte_len == 0)
        {
            return PLATFORM_ERROR;
        }
        result = platform_otp_read_array(byte_number, &tag_len, 1);
        if (result != PLATFORM_SUCCESS)
        {
            return result;
        }
        byte_number++;
        byte_len--;

        if (tag == CISTPL_BRCM_HNBU)
        {
            if ((byte_len == 0) || (tag_len == 0))
            {
                return PLATFORM_ERROR;
            }
            result = platform_otp_read_array(byte_number, &brcm_tag, 1);
            if (result != PLATFORM_SUCCESS)
            {
                return result;
            }
            byte_number++;
            byte_len--;
            tag_len--;
        }

        if (tag_len > byte_len)
        {
            return PLATFORM_ERROR;
        }

        result = (*callback)(context, tag, brcm_tag, byte_number, tag_len);
        if (result != PLATFORM_PARTIAL_RESULTS)
        {
            return result;
        }

        byte_number += tag_len;
        byte_len -= tag_len;
    }

    return PLATFORM_PARTIAL_RESULTS;
}

static platform_result_t
platform_otp_read_tag_callback( void* context, uint8_t tag, uint8_t brcm_tag, uint16_t offset, uint8_t size )
{
    otp_read_tag_context_t *parse_context = context;
    uint16_t byte_len = parse_context->byte_len;

    if ( ( tag == CISTPL_BRCM_HNBU ) && ( brcm_tag == parse_context->tag ) )
    {
        parse_context->byte_len = size;

        if ( size > byte_len )
        {
            return PLATFORM_BADARG;
        }

        if ( platform_otp_read_array( offset, parse_context->data, size ) != PLATFORM_SUCCESS )
        {
            return PLATFORM_ERROR;
        }

        return PLATFORM_SUCCESS;
    }

    return PLATFORM_PARTIAL_RESULTS;
}

platform_result_t
platform_otp_read_tag(platform_otp_region_t region, uint8_t tag, void *data, uint16_t *byte_len)
{
    otp_read_tag_context_t context = { .tag = tag, .data = data, .byte_len = *byte_len };
    platform_result_t result;

    result = platform_otp_init();
    if (result != PLATFORM_SUCCESS)
    {
        return result;
    }

    result = platform_otp_cis_parse(region, platform_otp_read_tag_callback, &context);
    *byte_len = context.byte_len;
    return result;
}

platform_result_t
platform_otp_package_options( uint32_t *package_options )
{
    uint16_t secure_bit = 0;

    platform_otp_read_bit_unprotected(PLATFORM_OTP_SECURE_BIT_OFFSET, &secure_bit);

    /* Extract package options bits[2:0] from ChipCommon ChipID register */
    *package_options = (uint32_t)(PLATFORM_CHIPCOMMON->core_ctrl_status.chip_id.bits.package_option) & 0x7;

    *package_options |= (uint32_t)(secure_bit << 3);

    return PLATFORM_SUCCESS;
}

/* Small Memory-Footprint function for OTP Read */
platform_result_t platform_otp_read_word_unprotected(uint32_t word_number, uint16_t *read_word)
{
    volatile uint16_t *otpbase;

    otpbase = (uint16_t*)PLATFORM_OTP_BASE;

    *read_word = R_REG(NULL, &otpbase[word_number]);

    return PLATFORM_SUCCESS;
}

platform_result_t platform_otp_read_bit_unprotected(uint16_t bit_num, uint16_t *read_bit)
{
    unsigned int word_num, bit_off;
    uint16_t word_val = 0;

    word_num = bit_num / OTPWSIZE;
    bit_off  = bit_num % OTPWSIZE;

    platform_otp_read_word_unprotected(word_num, &word_val);

    *read_bit = (word_val >> bit_off) & 0x1;

    return PLATFORM_SUCCESS;
}

#ifdef OTP_DEBUG
platform_result_t
platform_otp_dump(void)
{
    otpinfo_t *oi = get_otpinfo();

    if ((oi->sih == NULL) || (oi->fn == NULL))
    {
        OTP_ERR(("platform_otp_dump: OTP not initialized\n"));
        return PLATFORM_ERROR;
    }

    if (oi->fn->dump == NULL)
    {
        OTP_ERR(("platform_otp_dump: Unsupported OTP operation\n"));
        return PLATFORM_ERROR;
    }

    oi->fn->dump((void *)oi);

    return PLATFORM_SUCCESS;
}

platform_result_t
platform_otp_dumpstats(void)
{
    otpinfo_t *oi = get_otpinfo();

    if ((oi->sih == NULL) || (oi->fn == NULL))
    {
        OTP_ERR(("platform_otp_dumpstats: OTP not initialized\n"));
        return PLATFORM_ERROR;
    }

    OTP_ERR(("\nOTP, ccrev 0x%04x\n", oi->ccrev));
#ifdef BCMIPXOTP
    OTP_ERR(("wsize %d rows %d cols %d\n", oi->wsize, oi->rows, oi->cols));
    OTP_ERR(("hwbase %d hwlim %d swbase %d swlim %d fusebase %d fuselim %d fusebits %d\n",
        oi->hwbase, oi->hwlim, oi->swbase, oi->swlim, oi->fbase, oi->flim, oi->fusebits));
    OTP_ERR(("otpgu_base %d status %lu\n", oi->otpgu_base, oi->status));
#endif
    OTP_ERR(("\n"));

    return PLATFORM_SUCCESS;
}
#endif /* OTP_DEBUG */
