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
 * Port of siutils.c and aiutils.c. Simplified functionality.
 */

#include "typedefs.h"
#include "bcmutils.h"
#include "siutils.h"
#include "sbchipc.h"
#include "hndsoc.h"
#include "hndpmu.h"
#include "bcmdevs.h"
#include "wiced_osl.h"

#include "platform_map.h"
#include "platform_mcu_peripheral.h"

#define SI_INFO(sih) ((si_info_t*)(void*)(sih))

#define NOREV        ((uint)-1)

#define PMU_CORE_REV            26
#if ( defined(BCMCHIPREV) && (BCMCHIPREV == 0) )
#define CC_CORE_REV             50
#elif ( defined(BCMCHIPREV) && ((BCMCHIPREV == 1) || (BCMCHIPREV == 2)) )
#define CC_CORE_REV             55
#else
#error CC_CORE_REV not defined!
#endif
#define I2S_CORE_REV             5
#define GMAC_CORE_REV           6
#define DDR_CONTROLLER_CORE_REV 0
#define M2MDMA_CORE_REV         0
#define CRYPTO_CORE_REV         0
#define GCI_CORE_REV            4
#define USB20H_CORE_REV         5
#define USB20D_CORE_REV         23
#define SDIOH_CORE_REV          3

typedef struct si_private
{
    uint coreid;
    uint corerev;
    void *curmap;
    void *curwrap;
} si_private_t;

typedef struct si_info
{
    struct si_pub pub;
    struct si_private priv;
    uint curidx;
} si_info_t;

/* Consider reading this info from chip */
static si_private_t core_info[] =
{
    {
        /*
         * There are TWO constants on all HND chips: SI_ENUM_BASE
         * and chipcommon being the first core.
         */
        .curmap  = (void*)PLATFORM_CHIPCOMMON_REGBASE(0x0),
        .curwrap = (void*)PLATFORM_CHIPCOMMON_MASTER_WRAPPER_REGBASE(0x0),
        .coreid  = CC_CORE_ID,
        .corerev = CC_CORE_REV,
    },
    {
        .curmap  = (void*)PLATFORM_I2S0_REGBASE(0x0),
        .curwrap = (void*)PLATFORM_I2S0_MASTER_WRAPPER_REGBASE(0x0),
        .coreid  = I2S_CORE_ID,
        .corerev = I2S_CORE_REV,
    },
    {
        .curmap  = (void*)PLATFORM_I2S1_REGBASE(0x0),
        .curwrap = (void*)PLATFORM_I2S1_MASTER_WRAPPER_REGBASE(0x0),
        .coreid  = I2S_CORE_ID,
        .corerev = I2S_CORE_REV,
    },
    {
        .curmap  = (void*)PLATFORM_GMAC_REGBASE(0x0),
        .curwrap = (void*)PLATFORM_GMAC_MASTER_WRAPPER_REGBASE(0x0),
        .coreid  = GMAC_CORE_ID,
        .corerev = GMAC_CORE_REV,
    },
    {
        .curmap  = (void*)PLATFORM_I2S0_REGBASE(0x0),
        .curwrap = (void*)PLATFORM_I2S0_MASTER_WRAPPER_REGBASE(0x0),
        .coreid  = I2S_CORE_ID,
        .corerev = I2S_CORE_REV,
    },
    {
        .curmap  = (void*)PLATFORM_PMU_REGBASE(0x0),
        .curwrap = NULL,
        .coreid  = PMU_CORE_ID,
        .corerev = PMU_CORE_REV,
    },
    {
        .curmap  = (void*)PLATFORM_DDR_CONTROLLER_REGBASE(0x0),
        .curwrap = (void*)PLATFORM_DDR_CONTROLLER_SLAVE_WRAPPER_REGBASE(0x0),
        .coreid  = DMEMC_CORE_ID, /* may be not what constants used in EPROM */
        .corerev = DDR_CONTROLLER_CORE_REV,
    },
    {
        .curmap  = (void*)PLATFORM_M2M_REGBASE(0x0),
        .curwrap = (void*)PLATFORM_M2M_MASTER_WRAPPER_REGBASE(0x0),
        .coreid  = M2MDMA_CORE_ID,
        .corerev = M2MDMA_CORE_REV,
    },
    {
        .curmap  = (void*)PLATFORM_CRYPTO_REGBASE(0x0),
        .curwrap = (void*)PLATFORM_CRYPTO_MASTER_WRAPPER_REGBASE(0x0),
        .coreid  = CRYPTO_CORE_ID,
        .corerev = CRYPTO_CORE_REV,
    },
    {
        .curmap  = (void*)PLATFORM_GCI_REGBASE(0x0),
        .curwrap = NULL,
        .coreid  = GCI_CORE_ID,
        .corerev = GCI_CORE_REV,
    },
    {
        .curmap  = (void*)PLATFORM_EHCI_REGBASE(0x0),
        .curwrap = (void*)PLATFORM_USB20H_MASTER_WRAPPER_REGBASE(0x0),
        .coreid  = USB20H_CORE_ID,
        .corerev = USB20H_CORE_REV,
    },
    {
        .curmap  = (void*)PLATFORM_USB20D_REGBASE(0x0),
        .curwrap = (void*)PLATFORM_USB20D_MASTER_WRAPPER_REGBASE(0x0),
        .coreid  = USB20D_CORE_ID,
        .corerev = USB20D_CORE_REV,
    },
    {
        .curmap  = (void*)PLATFORM_SDIOH_REGBASE(0x0),
        .curwrap = (void*)PLATFORM_SDIOH_MASTER_WRAPPER_REGBASE(0x0),
        .coreid  = SDIOH_CORE_ID,
        .corerev = SDIOH_CORE_REV,
    },
    {
        .curmap  = NULL,
        .curwrap = NULL,
        .coreid  = NODEV_CORE_ID,
        .corerev = NOREV,
    },
};

/* Private SI Handle */
static si_info_t ksii;

static si_info_t*
si_doattach(si_info_t *sii, osl_t *osh)
{
    chipcregs_t *cc = (void*)PLATFORM_CHIPCOMMON_REGBASE(0x0);

    if (sii == NULL)
    {
        return NULL;
    }

    memset(sii, 0, sizeof(*sii));

    sii->pub.chip = BCMCHIPID;
    sii->pub.bustype = SI_BUS;

    sii->pub.ccrev = CC_CORE_REV;
    sii->pub.chipst = R_REG(NULL, &cc->chipstatus);
    sii->pub.cccaps = R_REG(NULL, &cc->capabilities);
    sii->pub.cccaps_ext = R_REG(NULL, &cc->capabilities_ext);

    if (sii->pub.cccaps & CC_CAP_PMU)
    {
        if (AOB_ENAB(&sii->pub))
        {
            pmuregs_t *pmu = (void*)PLATFORM_PMU_REGBASE(0x0);
            sii->pub.pmucaps = R_REG(NULL, &pmu->pmucapabilities);
        }
        else
        {
            sii->pub.pmucaps = R_REG(NULL, &cc->pmucapabilities);
        }
        sii->pub.pmurev = sii->pub.pmucaps & PCAP_REV_MASK;
    }

    sii->priv = core_info[ARRAYSIZE(core_info) - 1];
    sii->curidx = BADIDX;

    return sii;
}

si_t*
si_attach(uint pcidev, osl_t *osh, void *regs, uint bustype, void *sdh, char **vars, uint *varsz)
{
    si_info_t *sii = MALLOC(osh, sizeof (si_info_t));

    si_doattach(sii, osh);

    return (si_t*)sii;
}

si_t*
si_kattach(osl_t *osh)
{
    static bool ksii_attached = FALSE;

    if (!ksii_attached)
    {
        si_doattach(&ksii, osh);
        ksii_attached = TRUE;
    }

    return (si_t*)&ksii;
}

void
si_detach(si_t *sih)
{
    si_info_t *sii = SI_INFO(sih);

    if (sii == NULL)
    {
        return;
    }

    MFREE(0, sii, 0);
}

void *
si_osh(si_t *sih)
{
    /* sih currently does not maintain osh */
    return SI_OSH;
}

uint
si_corerev(si_t *sih)
{
    si_info_t *sii = SI_INFO(sih);
    return sii->priv.corerev;
}

uint
si_coreid(si_t *sih)
{
    si_info_t *sii = SI_INFO(sih);
    return sii->priv.coreid;
}

uint
si_coreidx(si_t *sih)
{
    si_info_t *sii = SI_INFO(sih);
    return sii->curidx;
}

void
si_core_disable(si_t *sih, uint32 bits)
{
    si_info_t *sii = SI_INFO(sih);
    aidmp_t *ai = sii->priv.curwrap;
    volatile uint32 dummy;
    uint32 status;

    ASSERT(ai);

    /* if core is already in reset, just return */
    if (R_REG(NULL, &ai->resetctrl) & AIRC_RESET)
    {
        return;
    }

    /*
     * Ensure there are no pending backplane operations.
     * 300usecs was sufficient to allow backplane ops to clear for big hammer
     * during driver load we may need more time.
     * If still pending ops, continue on and try disable anyway.
     */
    SPINWAIT(((status = R_REG(NULL, &ai->resetstatus)) != 0), 10300);

    W_REG(NULL, &ai->resetctrl, AIRC_RESET);
    dummy = R_REG(NULL, &ai->resetctrl);
    BCM_REFERENCE(dummy);
    OSL_DELAY(1);

    W_REG(NULL, &ai->ioctrl, bits);
    dummy = R_REG(NULL, &ai->ioctrl);
    BCM_REFERENCE(dummy);
    OSL_DELAY(10);
}

uint32
si_core_cflags(si_t *sih, uint32 mask, uint32 val)
{
    si_info_t *sii = SI_INFO(sih);
    aidmp_t *ai = sii->priv.curwrap;

    ASSERT(ai);
    ASSERT((val & ~mask) == 0);

    if (mask || val)
    {
        uint32_t w = (R_REG(NULL, &ai->ioctrl) & ~mask) | val;
        W_REG(NULL, &ai->ioctrl, w);
    }

    return R_REG(NULL, &ai->ioctrl);
}

uint32
si_core_sflags(si_t *sih, uint32 mask, uint32 val)
{
    si_info_t *sii = SI_INFO(sih);
    aidmp_t *ai = sii->priv.curwrap;

    ASSERT(ai);
    ASSERT((val & ~mask) == 0);
    ASSERT((mask & ~SISF_CORE_BITS) == 0);

    if (mask || val)
    {
        uint32_t w = (R_REG(NULL, &ai->iostatus) & ~mask) | val;
        W_REG(NULL, &ai->iostatus, w);
    }

    return R_REG(NULL, &ai->iostatus);
}

bool
si_iscoreup_wrapper(void *wrapper)
{
    aidmp_t *ai = wrapper;

    ASSERT(ai);

    return (((R_REG(NULL, &ai->ioctrl) & (SICF_FGC | SICF_CLOCK_EN)) == SICF_CLOCK_EN) &&
           ((R_REG(NULL, &ai->resetctrl) & AIRC_RESET) == 0));
}

bool
si_iscoreup(si_t *sih)
{
    si_info_t *sii = SI_INFO(sih);
    return si_iscoreup_wrapper(sii->priv.curwrap);
}

uint32
si_clock(si_t *sih)
{
    chipcregs_t *cc = (void*)PLATFORM_CHIPCOMMON_REGBASE(0x0);
    uint32 n, m;
    uint32 pll_type, rate;

    if (PMUCTL_ENAB(sih))
    {
        rate = si_pmu_si_clock(sih, NULL);
        goto exit;
    }

    n = R_REG(NULL, &cc->clockcontrol_n);
    pll_type = sih->cccaps & CC_CAP_PLL_MASK;
    if (pll_type == PLL_TYPE6)
    {
        m = R_REG(NULL, &cc->clockcontrol_m3);
    }
    else if (pll_type == PLL_TYPE3)
    {
        m = R_REG(NULL, &cc->clockcontrol_m2);
    }
    else
    {
        m = R_REG(NULL, &cc->clockcontrol_sb);
    }

    /* calculate rate */
    rate = si_clock_rate(pll_type, n, m);

    if (pll_type == PLL_TYPE3)
    {
        rate = rate / 2;
    }

exit:
    return rate;
}

static uint32
factor6(uint32 x)
{
    switch (x)
    {
        case CC_F6_2: return 2;
        case CC_F6_3: return 3;
        case CC_F6_4: return 4;
        case CC_F6_5: return 5;
        case CC_F6_6: return 6;
        case CC_F6_7: return 7;
        default:      return 0;
    }
}

uint32
si_clock_rate(uint32 pll_type, uint32 n, uint32 m)
{
    uint32 n1, n2, clock, m1, m2, m3, mc;
    wiced_bool_t valid_clock_divisor = WICED_TRUE;

    n1 = n & CN_N1_MASK;
    n2 = (n & CN_N2_MASK) >> CN_N2_SHIFT;

    if (pll_type == PLL_TYPE6)
    {
        if (m & CC_T6_MMASK)
        {
            return CC_T6_M1;
        }
        else
        {
            return CC_T6_M0;
        }
    }
    else if ((pll_type == PLL_TYPE1) || (pll_type == PLL_TYPE3) || (pll_type == PLL_TYPE4) || (pll_type == PLL_TYPE7))
    {
        n1 = factor6(n1);
        n2 += CC_F5_BIAS;
    }
    else if (pll_type == PLL_TYPE2)
    {
        n1 += CC_T2_BIAS;
        n2 += CC_T2_BIAS;
        ASSERT((n1 >= 2) && (n1 <= 7));
        ASSERT((n2 >= 5) && (n2 <= 23));
    }
    else if (pll_type == PLL_TYPE5)
    {
        /* XXX: 5365 */
        return (100000000);
    }
    else
    {
        ASSERT(0);
    }

    /* PLL types 3 and 7 use BASE2 (25Mhz) */
    if ((pll_type == PLL_TYPE3) || (pll_type == PLL_TYPE7))
    {
        clock = CC_CLOCK_BASE2 * n1 * n2;
    }
    else
    {
        clock = CC_CLOCK_BASE1 * n1 * n2;
    }
    if (clock == 0)
    {
        return 0;
    }

    m1 = m & CC_M1_MASK;
    m2 = (m & CC_M2_MASK) >> CC_M2_SHIFT;
    m3 = (m & CC_M3_MASK) >> CC_M3_SHIFT;
    mc = (m & CC_MC_MASK) >> CC_MC_SHIFT;

    if ((pll_type == PLL_TYPE1) || (pll_type == PLL_TYPE3) || (pll_type == PLL_TYPE4) || (pll_type == PLL_TYPE7))
    {
        m1 = factor6(m1);
        if ((pll_type == PLL_TYPE1) || (pll_type == PLL_TYPE3))
        {
            m2 += CC_F5_BIAS;
        }
        else
        {
            m2 = factor6(m2);
        }
        m3 = factor6(m3);

        /* need to error check due to possible misconfiguration or corruption */
        switch (mc)
        {
            case CC_MC_M1:
                valid_clock_divisor = ( m1 != 0 );
                break;
            case CC_MC_M1M2:
                valid_clock_divisor = ( m1 != 0 && m2 != 0 );
                break;
            case CC_MC_M1M2M3:
                valid_clock_divisor = ( m1 != 0 && m2 != 0 && m3 != 0 );
                break;
            case CC_MC_M1M3:
                valid_clock_divisor = ( m1 != 0 && m3 != 0 );
                break;
            case CC_MC_BYPASS: /* FALLSTHROUGH */
            default:
                break; /* No reliance on clock divisors here */
        }

        /* catch bad data here */
        if ( WICED_FALSE == valid_clock_divisor )
        {
            wiced_assert("invalid clock divisor", 0 != 0);
            WPRINT_WICED_ERROR(( "Invalid clock divisor pll=%d, m1 = %d m2 = %d m3 = %d\n", mc, m1, m2, m3 ));

            return 0;
        }

        /* use clock divisor */
        switch (mc)
        {
            case CC_MC_BYPASS: return (clock);
            case CC_MC_M1:     return (clock / m1);
            case CC_MC_M1M2:   return (clock / (m1 * m2));
            case CC_MC_M1M2M3: return (clock / (m1 * m2 * m3));
            case CC_MC_M1M3:   return (clock / (m1 * m3));
            default:           return (0);
        }
    }
    else
    {
        ASSERT(pll_type == PLL_TYPE2);

        m1 += CC_T2_BIAS;
        m2 += CC_T2M2_BIAS;
        m3 += CC_T2_BIAS;
        ASSERT((m1 >= 2) && (m1 <= 7));
        ASSERT((m2 >= 3) && (m2 <= 10));
        ASSERT((m3 >= 2) && (m3 <= 7));

        if ((mc & CC_T2MC_M1BYP) == 0)
        {
            clock /= m1;
        }
        if ((mc & CC_T2MC_M2BYP) == 0)
        {
            clock /= m2;
        }
        if ((mc & CC_T2MC_M3BYP) == 0)
        {
            clock /= m3;
        }

        return clock;
    }
}

void
si_core_reset_set_wrapper(void *wrapper, uint32 bits, uint32 resetbits)
{
    aidmp_t *ai = wrapper;
    volatile uint32 dummy;

    ASSERT(ai);

    /* ensure there are no pending backplane operations */
    SPINWAIT(((dummy = R_REG(NULL, &ai->resetstatus)) != 0), 300);

    /* put core into reset state */
    W_REG(NULL, &ai->resetctrl, AIRC_RESET);
    OSL_DELAY(10);

    /* ensure there are no pending backplane operations */
    SPINWAIT((R_REG(NULL, &ai->resetstatus) != 0), 300);

    W_REG(NULL, &ai->ioctrl, (bits | resetbits | SICF_FGC | SICF_CLOCK_EN));
    dummy = R_REG(NULL, &ai->ioctrl);
    BCM_REFERENCE(dummy);
}

void
si_core_reset_clear_wrapper(void *wrapper, uint32 bits)
{
    aidmp_t *ai = wrapper;
    volatile uint32 dummy;
    uint loop_counter;

    ASSERT(ai);

    /* ensure there are no pending backplane operations */
    SPINWAIT(((dummy = R_REG(NULL, &ai->resetstatus)) != 0), 300);

    loop_counter = 10;
    while (R_REG(NULL, &ai->resetctrl) != 0 && --loop_counter != 0)
    {
        /* ensure there are no pending backplane operations */
        SPINWAIT(((dummy = R_REG(NULL, &ai->resetstatus)) != 0), 300);

        /* take core out of reset */
        W_REG(NULL, &ai->resetctrl, 0);

        /* ensure there are no pending backplane operations */
        SPINWAIT((R_REG(NULL, &ai->resetstatus) != 0), 300);
    }

    W_REG(NULL, &ai->ioctrl, (bits | SICF_CLOCK_EN));
    dummy = R_REG(NULL, &ai->ioctrl);
    BCM_REFERENCE(dummy);
    OSL_DELAY(1);
}

void
si_core_reset_wrapper(void *wrapper, uint32 bits, uint32 resetbits)
{
    si_core_reset_set_wrapper(wrapper, bits, resetbits);
    si_core_reset_clear_wrapper(wrapper, bits);
}

void
si_core_reset(si_t *sih, uint32 bits, uint32 resetbits)
{
    si_info_t *sii = SI_INFO(sih);
    void *ai = sii->priv.curwrap;

    si_core_reset_wrapper(ai, bits, resetbits);
}

uint
si_findcoreidx(si_t *sih, uint coreid, uint coreunit)
{
    uint found = 0;
    int i;

    for (i = 0; i < ARRAYSIZE(core_info); ++i)
    {
        if (core_info[i].coreid == coreid)
        {
            if (found == coreunit)
            {
                return i;
            }
            found++;
        }
    }

    return BADIDX;
}

void*
si_setcoreidx(si_t *sih, uint coreindex)
{
    si_info_t *sii = SI_INFO(sih);
    void *curmap = NULL;

    if (coreindex < ARRAYSIZE(core_info))
    {
        sii->priv = core_info[coreindex];
        sii->curidx = coreindex;
        curmap = sii->priv.curmap;
    }

    return curmap;
}

void*
si_setcore(si_t *sih, uint coreid, uint coreunit)
{
    uint coreindex = si_findcoreidx(sih, coreid, coreunit);
    return si_setcoreidx(sih, coreindex);
}

uint
si_corereg(si_t *sih, uint coreindex, uint regoff, uint mask, uint val)
{
    uint32 *r;
    uint w;

    ASSERT(coreindex < SI_MAXCORES);
    ASSERT(regoff < SI_CORE_SIZE);
    ASSERT((val & ~mask) == 0);

    if (coreindex >= ARRAYSIZE(core_info))
    {
        return 0;
    }

    r = (uint32 *)((uchar *)core_info[coreindex].curmap + regoff);
    ASSERT(r != NULL);

    /* mask and set */
    if (mask || val)
    {
        w = (R_REG(NULL, r) & ~mask) | val;
        W_REG(NULL, r, w);
    }

    /* readback */
    w = R_REG(NULL, r);

    return w;
}

uint32*
si_corereg_addr(si_t *sih, uint coreindex, uint regoff)
{
    uint32 *r = NULL;

    ASSERT(coreindex < SI_MAXCORES);
    ASSERT(regoff < SI_CORE_SIZE);

    if (coreindex >= ARRAYSIZE(core_info))
    {
        return 0;
    }

    r = (uint32 *)((uchar *)core_info[coreindex].curmap + regoff);
    ASSERT(r != NULL);

    return r;
}

void
si_pci_setup(si_t *sih, uint coremask)
{
}

bool
si_is_otp_disabled(si_t *sih)
{
    switch (CHIPID(sih->chip))
    {
        /* chip-specific behavior specification */
        case BCM43909_CHIP_ID:
            return FALSE;
        default:
            return FALSE;
    }
}

bool
si_is_otp_powered(si_t *sih)
{
    if (PMUCTL_ENAB(sih))
    {
        return si_pmu_is_otp_powered(sih, si_osh(sih));
    }

    return TRUE;
}
