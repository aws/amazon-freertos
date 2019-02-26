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
 * Port of hndpmu.c. Simplified functionality.
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

#define PMUREGS_ILP_SENSITIVE(regoff) \
        ((regoff) == OFFSETOF(pmuregs_t, pmutimer) || \
         (regoff) == OFFSETOF(pmuregs_t, pmuwatchdog) || \
         (regoff) == OFFSETOF(pmuregs_t, res_req_timer))

#define CHIPCREGS_ILP_SENSITIVE(regoff) \
        ((regoff) == OFFSETOF(chipcregs_t, pmutimer) || \
         (regoff) == OFFSETOF(chipcregs_t, pmuwatchdog) || \
         (regoff) == OFFSETOF(chipcregs_t, res_req_timer))

#define PMU1_XTALTAB0_960_37400K        14

#ifdef BCMDBG_ERR
#define PMU_ERROR(args) printf args
#else
#define PMU_ERROR(args)
#endif  /* BCMDBG_ERR */

#ifdef BCMDBG
#define PMU_MSG(args)   printf args
#else
#define PMU_MSG(args)
#endif  /* BCMDBG */

/* To check in verbose debugging messages not intended
 * to be on except on private builds.
 */
#define PMU_NONE(args)

/** contains resource bit positions for a specific chip */
struct rsc_per_chip_s {
    uint8 otp_pu;
};

typedef struct rsc_per_chip_s rsc_per_chip_t;

/* setup pll and query clock speed */
typedef struct {
    uint16  fref;   /* x-tal frequency in [hz] */
    uint8   xf;     /* x-tal index as contained in PMU control reg, see PMU programmers guide */
    uint8   p1div;
    uint8   p2div;
    uint8   ndiv_int;
    uint32  ndiv_frac;
} pmu1_xtaltab0_t;

static const pmu1_xtaltab0_t BCMINITDATA(pmu1_xtaltab0_960)[] = {
/*      fref      xf       p1div   p2div  ndiv_int  ndiv_frac */
        {12000,   1,       1,      1,     0x50,   0x0     }, /* array index 0 */
        {13000,   2,       1,      1,     0x49,   0xD89D89},
        {14400,   3,       1,      1,     0x42,   0xAAAAAA},
        {15360,   4,       1,      1,     0x3E,   0x800000},
        {16200,   5,       1,      1,     0x3B,   0x425ED0},
        {16800,   6,       1,      1,     0x39,   0x249249},
        {19200,   7,       1,      1,     0x32,   0x0     },
        {19800,   8,       1,      1,     0x30,   0x7C1F07},
        {20000,   9,       1,      1,     0x30,   0x0     },
        {24000,   10,      1,      1,     0x28,   0x0     },
        {25000,   11,      1,      1,     0x26,   0x666666}, /* array index 10 */
        {26000,   12,      1,      1,     0x24,   0xEC4EC4},
        {30000,   13,      1,      1,     0x20,   0x0     },
        {33600,   14,      1,      1,     0x1C,   0x924924},
        {37400,   15,      2,      1,     0x33,   0x563EF9},
        {38400,   16,      2,      1,     0x32,   0x0     },
        {40000,   17,      2,      1,     0x30,   0x0     },
        {48000,   18,      2,      1,     0x28,   0x0     },
        {52000,   19,      2,      1,     0x24,   0xEC4EC4}, /* array index 18 */
        {0,       0,       0,      0,     0,      0       }
};

int si_pmu_wait_for_steady_state(si_t *sih, osl_t *osh, chipcregs_t *cc);
uint32 si_pmu_get_pmutime_diff(si_t *sih, osl_t *osh, chipcregs_t *cc, uint32 *prev);
bool si_pmu_wait_for_res_pending(si_t *sih, osl_t *osh, chipcregs_t *cc, uint usec, bool cond, uint32 *elapsed_time);
uint32 si_pmu_get_pmutimer(si_t *sih, osl_t *osh, chipcregs_t *cc);

bool
si_pmu_is_ilp_sensitive(uint32 idx, uint regoff)
{
    if (idx == SI_CC_IDX)
    {
        return CHIPCREGS_ILP_SENSITIVE(regoff);
    }

    return PMUREGS_ILP_SENSITIVE(regoff);
}

uint
si_pmu_corereg(si_t *sih, uint32 idx, uint regoff, uint mask, uint val)
{
    int pmustatus_offset;

    /* prevent backplane stall on double write to 'ILP domain' registers in the PMU */
    if (mask != 0 && sih->pmurev >= 22 && si_pmu_is_ilp_sensitive(idx, regoff))
    {
        pmustatus_offset = AOB_ENAB(sih) ? OFFSETOF(pmuregs_t, pmustatus) : OFFSETOF(chipcregs_t, pmustatus);
        while (si_corereg(sih, idx, pmustatus_offset, 0, 0) & PST_SLOW_WR_PENDING);
    }

    return si_corereg(sih, idx, regoff, mask, val);
}

uint32
si_pmu_chipcontrol(si_t *sih, uint reg, uint32 mask, uint32 val)
{
    pmu_corereg(sih, SI_CC_IDX, chipcontrol_addr, ~0, reg);
    return pmu_corereg(sih, SI_CC_IDX, chipcontrol_data, mask, val);
}

uint32
si_pmu_pllcontrol(si_t *sih, uint reg, uint32 mask, uint32 val)
{
    pmu_corereg(sih, SI_CC_IDX, pllcontrol_addr, ~0, reg);
    return pmu_corereg(sih, SI_CC_IDX, pllcontrol_data, mask, val);
}

/**
 * The chip has one or more PLLs/FLLs (e.g. baseband PLL, USB PHY PLL). The settings of each PLL are
 * contained within one or more 'PLL control' registers. Since the PLL hardware requires that
 * changes for one PLL are committed at once, the PMU has a provision for 'updating' all PLL control
 * registers at once.
 *
 * When software wants to change the any PLL parameters, it withdraws requests for that PLL clock,
 * updates the PLL control registers being careful not to alter any control signals for the other
 * PLLs, and then writes a 1 to PMUCtl.PllCtnlUpdate to commit the changes. Best usage model would
 * be bring PLL down then update the PLL control register.
 */
void
si_pmu_pllupd(si_t *sih)
{
    pmu_corereg(sih, SI_CC_IDX, pmucontrol,
               PCTL_PLL_PLLCTL_UPD, PCTL_PLL_PLLCTL_UPD);
}

uint32
si_pmu_cal_fvco(si_t *sih, osl_t *osh)
{
    uint32 xf, ndiv_int, ndiv_frac, fvco, pll_reg, p1_div_scale;
    uint32 r_high, r_low, int_part, frac_part, rounding_const;
    uint8 p1_div;

    xf = si_pmu_alp_clock(sih, osh)/1000;

    pll_reg = si_pmu_pllcontrol(sih, PMU1_PLL0_PLLCTL2, 0, 0);

    p1_div = (pll_reg & PMU4335_PLL0_PC2_P1DIV_MASK) >> PMU4335_PLL0_PC2_P1DIV_SHIFT;
    if (p1_div == 0)
    {
        ASSERT(p1_div != 0);
        return 0;
    }

    ndiv_int = (pll_reg & PMU4335_PLL0_PC2_NDIV_INT_MASK) >> PMU4335_PLL0_PC2_NDIV_INT_SHIFT;

    pll_reg = si_pmu_pllcontrol(sih, PMU1_PLL0_PLLCTL3, 0, 0);

    ndiv_frac = (pll_reg & PMU1_PLL0_PC3_NDIV_FRAC_MASK) >> PMU1_PLL0_PC3_NDIV_FRAC_SHIFT;

    /* Actual expression is as below */
    /* fvco1 = (100 * (xf * 1/p1_div) * (ndiv_int + (ndiv_frac * 1/(1 << 24)))) */
    /* * 1/(1000 * 100); */

    /* Representing 1/p1_div as a 12 bit number */
    /* Reason for the choice of 12: */
    /* ndiv_int is represented by 9 bits */
    /* so (ndiv_int << 24) needs 33 bits */
    /* xf needs 16 bits for the worst case of 52MHz clock */
    /* So (xf * (ndiv << 24)) needs atleast 49 bits */
    /* So remaining bits for uint64 : 64 - 49 = 15 bits */
    /* So, choosing 12 bits, with 3 bits of headroom */
    int_part = xf * ndiv_int;

    rounding_const = 1 << (BBPLL_NDIV_FRAC_BITS - 1);
    bcm4390x_uint64_multiple_add(&r_high, &r_low, ndiv_frac, xf, rounding_const);
    bcm4390x_uint64_right_shift(&frac_part, r_high, r_low, BBPLL_NDIV_FRAC_BITS);

    p1_div_scale = (1 << P1_DIV_SCALE_BITS) / p1_div;
    rounding_const = 1 << (P1_DIV_SCALE_BITS - 1);

    bcm4390x_uint64_multiple_add(&r_high, &r_low, (int_part + frac_part), p1_div_scale, rounding_const);
    bcm4390x_uint64_right_shift(&fvco, r_high, r_low, P1_DIV_SCALE_BITS);

    return fvco;
}


static uint32
si_pmu1_pllfvco0(si_t *sih)
{
    switch (CHIPID(sih->chip))
    {
        case BCM43909_CHIP_ID:
            return si_pmu_cal_fvco(sih, NULL);
    }

    return 0;
}

static uint32
si_pmu1_cpuclk0(si_t *sih, osl_t *osh, chipcregs_t *cc)
{
    uint32 tmp, mdiv = 1;
    uint32 FVCO = si_pmu1_pllfvco0(sih); /* in [hz] units */

    switch (CHIPID(sih->chip))
    {
        case BCM43909_CHIP_ID:
            /* Read m3div from pllcontrol[1] */
            W_REG(NULL, PMUREG(sih, pllcontrol_addr), PMU1_PLL0_PLLCTL1);
            tmp = R_REG(NULL, PMUREG(sih, pllcontrol_data));
            mdiv = (tmp & PMU1_PLL0_PC1_M3DIV_MASK) >> PMU1_PLL0_PC1_M3DIV_SHIFT;
            break;
    }

    return FVCO / mdiv * 1000;
}

uint32
si_pmu_si_clock(si_t *sih, osl_t *osh)
{
    chipcregs_t *cc = (void*)PLATFORM_CHIPCOMMON_REGBASE(0x0);
    uint32 clock = HT_CLOCK; /* in [hz] units */

    switch (CHIPID(sih->chip))
    {
        case BCM43909_CHIP_ID:
            clock = si_pmu1_cpuclk0(sih, osh, cc);
            break;
    }

    return clock;
}

static const pmu1_xtaltab0_t *
si_pmu1_xtaltab0(si_t *sih)
{
    switch (CHIPID(sih->chip))
    {
        case BCM43909_CHIP_ID:
            return pmu1_xtaltab0_960;
    }

    return NULL;
}

static const pmu1_xtaltab0_t *
si_pmu1_xtaldef0(si_t *sih)
{
    switch (CHIPID(sih->chip))
    {
        case BCM43909_CHIP_ID:
            return &pmu1_xtaltab0_960[PMU1_XTALTAB0_960_37400K];
    }

    return NULL;
}

static uint32
si_pmu1_alpclk0(si_t *sih, osl_t *osh, chipcregs_t *cc)
{
    const pmu1_xtaltab0_t *xt;
    uint32 xf;

    /* Find the frequency in the table */
    xf = (R_REG(osh, PMUREG(sih, pmucontrol)) & PCTL_XTALFREQ_MASK) >> PCTL_XTALFREQ_SHIFT;
    for (xt = si_pmu1_xtaltab0(sih); xt != NULL && xt->fref != 0; xt ++)
    {
        if (xt->xf == xf)
        {
            break;
        }
    }

    /* Could not find it so assign a default value */
    if (xt == NULL || xt->fref == 0)
    {
        xt = si_pmu1_xtaldef0(sih);
    }
    ASSERT(xt != NULL && xt->fref != 0);

    return xt->fref * 1000;
}

uint32
si_pmu_alp_clock(si_t *sih, osl_t *osh)
{
    chipcregs_t *cc = (void*)PLATFORM_CHIPCOMMON_REGBASE(0x0);
    uint32 clock = ALP_CLOCK;

    switch (CHIPID(sih->chip))
    {
        case BCM43909_CHIP_ID:
            clock = si_pmu1_alpclk0(sih, osh, cc);
            break;
    }

    return clock;
}

static rsc_per_chip_t rsc_43909 =  {RES43909_OTP_PU};

/**
* For each chip, location of resource bits (e.g., ht bit) in resource mask registers may differ.
* This function abstracts the bit position of commonly used resources, thus making the rest of the
* code in hndpmu.c cleaner.
*/
static rsc_per_chip_t* si_pmu_get_rsc_positions(si_t *sih)
{
    rsc_per_chip_t *rsc = NULL;

    switch (CHIPID(sih->chip))
    {
        case BCM43909_CHIP_ID:
            rsc = &rsc_43909;
            break;
        default:
            ASSERT(0);
            break;
    }

    return rsc;
}; /* si_pmu_get_rsc_positions */

/* Returns current value of PMUTimer.
 */
uint32
si_pmu_get_pmutimer(si_t *sih, osl_t *osh, chipcregs_t *cc)
{
    uint32 start;
    start = R_REG(osh, PMUREG(sih, pmutimer));
    if (start != R_REG(osh, PMUREG(sih, pmutimer)))
        start = R_REG(osh, PMUREG(sih, pmutimer));
    return (start);
}

/* Returns
 * a) diff between a 'prev' value of pmu timer and current value
 * b) the current pmutime value in 'prev'
 * So, 'prev' is an IO parameter.
 */
uint32
si_pmu_get_pmutime_diff(si_t *sih, osl_t *osh, chipcregs_t *cc, uint32 *prev)
{
    uint32 pmutime_diff = 0, pmutime_val = 0;
    uint32 prev_val = *prev;

    /* read current value */
    pmutime_val = si_pmu_get_pmutimer(sih, osh, cc);
    /* diff btween prev and current value, take on wraparound case as well. */
    pmutime_diff = pmutime_val - prev_val;

    *prev = pmutime_val;
    return pmutime_diff;
}

/** Wait for usec for the res_pending register to change. */
/*
 *  NOTE: usec SHOULD be > 32uS
 *  if cond = TRUE, res_pending will be read until it becomes == 0;
 *  If cond = FALSE, res_pending will be read until it becomes != 0;
 *  returns TRUE if timedout.
 *  returns elapsed time in this loop in elapsed_time
 */
bool
si_pmu_wait_for_res_pending(si_t *sih, osl_t *osh, chipcregs_t *cc, uint usec,
    bool cond, uint32 *elapsed_time)
{
    uint countdown = usec;
    uint32 pmutime_prev = 0, pmutime_elapsed = 0, res_pend;
    bool pending = FALSE;

    /* PMU timer is driven by ILP clock */
    uint pmu_us_steps = (uint)(1000000 / osl_ilp_clock()) + 1;

    /* store current time */
    pmutime_prev = si_pmu_get_pmutimer(sih, osh, cc);
    while (1)
    {
        res_pend = R_REG(osh, PMUREG(sih, res_pending));

        /* based on the condition, check */
        if (cond == TRUE)
        {
            if (res_pend == 0) break;
        }
        else
        {
            if (res_pend != 0) break;
        }

        /* if required time over */
        if ((pmutime_elapsed * pmu_us_steps) >= countdown)
        {
            /* timeout. so return as still pending */
            pending = TRUE;
            break;
        }

        /* get elapsed time after adding diff between prev and current
        * pmutimer value
        */
        pmutime_elapsed += si_pmu_get_pmutime_diff(sih, osh, cc, &pmutime_prev);
    }

    *elapsed_time = pmutime_elapsed * pmu_us_steps;
    return pending;
} /* si_pmu_wait_for_res_pending */

/**
 *  The algorithm for pending check is that,
 *  step1:  wait till (res_pending !=0) OR pmu_max_trans_timeout.
 *          if max_trans_timeout, flag error and exit.
 *          wait for 1 ILP clk [64uS] based on pmu timer,
 *          polling to see if res_pending again goes high.
 *          if res_pending again goes high, go back to step1.
 *  Note: res_pending is checked repeatedly because, in between switching
 *  of dependent
 *  resources, res_pending resets to 0 for a short duration of time before
 *  it becomes 1 again.
 *  Note: return 0 is GOOD, 1 is BAD [mainly timeout].
 */
int si_pmu_wait_for_steady_state(si_t *sih, osl_t *osh, chipcregs_t *cc)
{
    int stat = 0;
    bool timedout = FALSE;
    uint32 elapsed = 0, pmutime_total_elapsed = 0;

    while (1)
    {
        /* wait until all resources are settled down [till res_pending becomes 0] */
        timedout = si_pmu_wait_for_res_pending(sih, osh, cc,
            PMU_MAX_TRANSITION_DLY, TRUE, &elapsed);

        if (timedout)
        {
            stat = 1;
            break;
        }

        pmutime_total_elapsed += elapsed;
        /* wait to check if any resource comes back to non-zero indicating
        * that it pends again. The res_pending goes 0 for 1 ILP clock before
        * getting set for next resource in the sequence , so if res_pending
        * is 0 for more than 1 ILP clk it means nothing is pending
        * to indicate some pending dependency.
        */
        timedout = si_pmu_wait_for_res_pending(sih, osh, cc,
            64, FALSE, &elapsed);

        pmutime_total_elapsed += elapsed;
        /* Here, we can also check timedout, but we make sure that,
        * we read the res_pending again.
        */
        if (timedout)
        {
            stat = 0;
            break;
        }

        /* Total wait time for all the waits above added should be
        * less than  PMU_MAX_TRANSITION_DLY
        */
        if (pmutime_total_elapsed >= PMU_MAX_TRANSITION_DLY)
        {
            /* timeout. so return as still pending */
            stat = 1;
            break;
        }
    }
    return stat;
} /* si_pmu_wait_for_steady_state */

bool
si_pmu_is_otp_powered(si_t *sih, osl_t *osh)
{
    uint idx;
    chipcregs_t *cc;
    bool st;
    rsc_per_chip_t *rsc;        /* chip specific resource bit positions */

    /* Remember original core before switch to chipc */
    idx = si_coreidx(sih);
    cc = si_setcoreidx(sih, SI_CC_IDX);
    ASSERT(cc != NULL);

    si_pmu_wait_for_steady_state(sih, osh, cc);

    switch (CHIPID(sih->chip))
    {
    case BCM43909_CHIP_ID:
        rsc = si_pmu_get_rsc_positions(sih);
        st = (R_REG(osh, PMUREG(sih, res_state)) & PMURES_BIT(rsc->otp_pu)) != 0;
        break;
    default:
        st = TRUE;
        break;
    }

    /* Return to original core */
    si_setcoreidx(sih, idx);

    return st;
} /* si_pmu_is_otp_powered */
