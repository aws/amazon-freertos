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
 * WICED OSL.
 */

#include "typedefs.h"
#include "bcmutils.h"
#include "siutils.h"
#include "sbchipc.h"
#include "hndsoc.h"
#include "hndpmu.h"
#include "bcmdevs.h"
#include "wiced_osl.h"
#include "wiced_low_power.h"
#include "wwd_wlioctl.h"

#include "platform_map.h"
#include "platform_cache.h"
#include "platform_ethernet.h"
#include "platform_appscr4.h"

#include <stdarg.h>

#if !defined (NO_MALLOC_H)
#include <malloc.h>
#endif /* NO_MALLOC_H */

#ifndef PLATFORM_L1_CACHE_SHIFT
#define PLATFORM_L1_CACHE_SHIFT   0
#endif

#define PMUTICK_CALC_COUNT_SHIFT  4 /* 1<<4 times around loop) */

#define CLZ(x)                    __builtin_clzl(x)

static uint32_t WICED_DEEP_SLEEP_SAVED_VAR(ilpcycles_per_sec) = ILP_CLOCK;

void
osl_udelay(unsigned usec)
{
    uint32_t last_stamp  = PLATFORM_APPSCR4->cycle_cnt;
    uint32_t wait_cycles = (CPU_CLOCK_HZ + 500000) / 1000000 * usec;

    while (wait_cycles != 0)
    {
        uint32_t current_stamp = PLATFORM_APPSCR4->cycle_cnt;
        uint32_t passed_cycles = current_stamp - last_stamp;

        if (passed_cycles >= wait_cycles)
        {
            break;
        }

        wait_cycles -= passed_cycles;
        last_stamp = current_stamp;
    }
}

void*
osl_malloc_align(uint size, uint align_bits)
{
    return memalign(
        1 << MAX(align_bits, PLATFORM_L1_CACHE_SHIFT),
        PLATFORM_L1_CACHE_ROUND_UP(size));
}

void*
osl_dma_alloc_consistent(uint size, uint16 align_bits, uint *alloced, dmaaddr_t *descpa)
{
    void *cached_p = MALLOC_ALIGN(0, size, align_bits);
    void *uncached_p;

    if (cached_p == NULL)
    {
        return NULL;
    }

    platform_dcache_inv_range(cached_p, size); /* prevent any write backs */

    uncached_p = platform_addr_cached_to_uncached(cached_p);

    *alloced = size;
    *descpa = (dmaaddr_t)uncached_p;

    return uncached_p;
}

void
osl_dma_free_consistent(void *p)
{
    MFREE(0, platform_addr_uncached_to_cached(p), 0);
}

dmaaddr_t
osl_dma_map(void *p, uint size, int direction)
{
    if (direction == DMA_TX)
    {
        platform_dcache_clean_and_inv_range(p, size);
    }
    else if (direction == DMA_RX)
    {
        platform_dcache_inv_range(p, size);
    }
    else
    {
        p = 0;
    }

    return platform_addr_cpu_to_dma(p);
}

void*
osl_pktget(void *osh, uint len, bool send)
{
    wiced_osh_t *wiced_osh = osh;
    wiced_buffer_t buffer = NULL;
    wwd_result_t result = host_buffer_get(&buffer, send ? WWD_NETWORK_TX : WWD_NETWORK_RX, len, WICED_FALSE);

    if (result != WWD_SUCCESS)
    {
        WPRINT_PLATFORM_DEBUG(("PKTGET failed!!\n"));
        return NULL;
    }

    if (wiced_osh && wiced_osh->rx_pktget_add_remove && !send)
    {
        host_buffer_add_remove_at_front(&buffer, wiced_osh->rx_pktget_add_remove);
    }

    return (void *)buffer;
}

void
osl_pktfree(void *osh, void *p, bool send)
{
    wiced_buffer_t buffer = p;
    wiced_osh_t *wiced_osh = osh;

    if (wiced_osh && wiced_osh->tx_pktfree_add_remove && send)
    {
        host_buffer_add_remove_at_front(&buffer, wiced_osh->tx_pktfree_add_remove);
    }
    host_buffer_release(buffer, send ? WWD_NETWORK_TX : WWD_NETWORK_RX);
}

uint32
osl_alp_clock(void)
{
    uint32 alp_clock_value = 0;

    /* Attach private SI handle */
    si_t* si = si_kattach(NULL);

    if (si != NULL)
    {
        /* Calculate PMU ALP Clock */
        alp_clock_value = si_pmu_alp_clock(si, NULL);
    }

    return alp_clock_value;
}

uint32
osl_ilp_clock(void)
{
    return ilpcycles_per_sec;
}

void
osl_ilp_clock_measure(uint32 cpu_freq)
{
    uint32 pmutimer;
    uint32 c0counts_per_ms;
    uint32 cycles, startcycles;
    uint32 pmuticks_per_ms, pmuticks_per_ms_scale;
    uint64 ticks64;
    uint32 ticks, ticks_high, ticks_low;
    int i;

    /* Initialize timer */
    c0counts_per_ms = (cpu_freq + 999) / 1000;

    /*
     * Compute the pmu ticks per ms.  This is done by counting a
     * few PMU timer transitions and using the ARM cyclecounter as
     * a more accurate clock to measure the PMU tick interval.
     */

    /* Loop until we see a change */
    pmutimer = PLATFORM_PMU->pmutimer;
    while (pmutimer == PLATFORM_PMU->pmutimer) {}
    /* There is a clock boundary crosssing so do one more read */
    pmutimer = PLATFORM_PMU->pmutimer;

    /* The PMU timer just changed - start the cyclecount timer */
    startcycles = PLATFORM_APPSCR4->cycle_cnt;

    for (i = 0; i < (1 << PMUTICK_CALC_COUNT_SHIFT); i++)
    {
        while (pmutimer == PLATFORM_PMU->pmutimer) {}
        pmutimer = PLATFORM_PMU->pmutimer;
    }

    cycles = PLATFORM_APPSCR4->cycle_cnt - startcycles;

    /*
     * Calculate the pmuticks_per_ms with scaling for greater
     * accuracy.  We scale by the amount needed to shift the
     * c0counts_per_ms so the leading bit is set.  Since the
     * divisor (cycles counted) is implicity shifted by
     * PMUTICK_CALC_COUNT_SHIFT so that reduces the scale.
     *
     * We round up because we want the first tick AFTER the
     * requested ms - otherwise we will get an extraneuous
     * interrupt one tick early.
     */
    pmuticks_per_ms_scale = CLZ(c0counts_per_ms) - PMUTICK_CALC_COUNT_SHIFT;
    pmuticks_per_ms =  ((c0counts_per_ms << CLZ(c0counts_per_ms)) / cycles);
    pmuticks_per_ms++;              /* Round up */

    /* Calculate the PMU clock frequency and set it */
    ticks64 = ((uint64) 1000) * pmuticks_per_ms;    /* ticks per sec */
    ticks_high = ticks64 >> 32;
    ticks_low = (uint32) ticks64;
    ticks = ticks_low >> pmuticks_per_ms_scale;
    ticks += ticks_high << (32 - pmuticks_per_ms_scale);

    ilpcycles_per_sec = ticks;
}

void
osl_set_ext_lpoclk(uint32 cpu_freq)
{
    while ((PLATFORM_PMU->pmustatus & EXT_LPO_AVAIL) == 0);

    platform_gci_chipcontrol(CHIPCTRLREG6, EXT_LPO_SEL | INT_LPO_SEL, EXT_LPO_SEL);

    while ((PLATFORM_PMU->pmucontrol & LPO_SEL) != 0);

    osl_ilp_clock_measure(cpu_freq);
}

void
osl_set_int_lpoclk(uint32 cpu_freq)
{
    platform_gci_chipcontrol(CHIPCTRLREG6, EXT_LPO_SEL | INT_LPO_SEL, INT_LPO_SEL);

    while ((PLATFORM_PMU->pmucontrol & LPO_SEL) == 0);

    osl_ilp_clock_measure(cpu_freq);
}

void
osl_core_enable(uint coreid)
{
    /* Attach private SI handle */
    si_t* si = si_kattach(NULL);
    uint idx;

    if (si == NULL)
    {
        return;
    }

    idx = si_coreidx(si);

    si_setcore(si, coreid, 0);

    if (!si_iscoreup(si))
    {
        si_core_reset(si, 0, 0);
    }

    si_setcoreidx(si, idx);
}

uint
osl_get_corerev(uint coreid)
{
    /* Attach private SI handle */
    si_t* si = si_kattach(NULL);
    uint idx, corerev;

    if (si == NULL)
    {
        return 0;
    }

    idx = si_coreidx(si);
    si_setcore(si, coreid, 0);
    corerev = si_corerev(si);
    si_setcoreidx(si, idx);
    
    return corerev;
}

void
osl_wrapper_disable(void* wrapper)
{
    si_core_reset_set_wrapper(wrapper, 0x0, 0x0);
}

void
osl_wrapper_enable(void* wrapper)
{
    if (!si_iscoreup_wrapper(wrapper))
    {
        si_core_reset_wrapper(wrapper, 0x0, 0x0);
    }
}

int
bcm4390x_atoi(const char *s)
{
    return atoi(s);
}

/* parse a xx:xx:xx:xx:xx:xx format ethernet address */
int
bcm4390x_ether_atoe(const char *p, struct ether_addr *ea)
{
    int i = 0;
    char *ep;

    for (;;)
    {
        ea->octet[i++] = (char) strtoul(p, &ep, 16);
        p = ep;
        if (!*p++ || i == 6)
        {
            break;
        }
    }

    return (i == 6);
}

/* calculate a * b + c */
void
bcm4390x_uint64_multiple_add(uint32* r_high, uint32* r_low, uint32 a, uint32 b, uint32 c)
{
#define FORMALIZE(var) {cc += (var & 0x80000000) ? 1 : 0; var &= 0x7fffffff;}
    uint32 r1, r0;
    uint32 a1, a0, b1, b0, t, cc = 0;

    a1 = a >> 16;
    a0 = a & 0xffff;
    b1 = b >> 16;
    b0 = b & 0xffff;

    r0 = a0 * b0;
    FORMALIZE(r0);

    t = (a1 * b0) << 16;
    FORMALIZE(t);

    r0 += t;
    FORMALIZE(r0);

    t = (a0 * b1) << 16;
    FORMALIZE(t);

    r0 += t;
    FORMALIZE(r0);

    FORMALIZE(c);

    r0 += c;
    FORMALIZE(r0);

    r0 |= (cc % 2) ? 0x80000000 : 0;
    r1 = a1 * b1 + ((a1 * b0) >> 16) + ((b1 * a0) >> 16) + (cc / 2);

    *r_high = r1;
     *r_low = r0;
}

/* calculate a >> b; and returns only lower 32 bits */
void
bcm4390x_uint64_right_shift(uint32* r, uint32 a_high, uint32 a_low, uint32 b)
{
    uint32 a1 = a_high, a0 = a_low, r0 = 0;

    if (b == 0)
    {
        r0 = a_low;
        *r = r0;
        return;
    }

    if (b < 32)
    {
        a0 = a0 >> b;
        a1 = a1 & ((1 << b) - 1);
        a1 = a1 << (32 - b);
        r0 = a0 | a1;
        *r = r0;
        return;
    }
    else
    {
        r0 = a1 >> (b - 32);
        *r = r0;
        return;
    }
}

void
bcm_binit(struct bcmstrbuf *b, char *buf, uint size)
{
    b->origsize = b->size = size;
    b->origbuf = b->buf = buf;
}

int
bcm_bprintf(struct bcmstrbuf *b, const char *fmt, ...)
{
    va_list ap;
    int r;

    va_start(ap, fmt);

    r = vsnprintf(b->buf, b->size, fmt, ap);

    /* Non Ansi C99 compliant returns -1,
     * Ansi compliant return r >= b->size,
     * bcmstdlib returns 0, handle all
     */
    /* r == 0 is also the case when strlen(fmt) is zero.
     * typically the case when "" is passed as argument.
     */
    if ((r == -1) || (r >= (int)b->size)) {
        b->size = 0;
    } else {
        b->size -= r;
        b->buf += r;
    }

    va_end(ap);

    return r;
}

