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
 * 802.1AS clock functionality for the BCM43909.
 */

#include "platform_peripheral.h"
#include "platform_ascu.h"
#include "platform_cache.h"

/******************************************************
 *                      Macros
 ******************************************************/

#define NUM_NSECONDS_IN_SECOND      ((uint64_t)1000000000)

/******************************************************
 *                    Constants
 ******************************************************/

#define MASTER_TIME_GAP_THRESHOLD_MAX_NSECS (5000)
#define MASTER_TIME_GAP_THRESHOLD_MIN_NSECS (-5000)

/******************************************************
 *                   Enumerations
 ******************************************************/

typedef enum
{
    ASCU_AUDIO_TIMER_TYPE_NONE      = 0,
    ASCU_AUDIO_TIMER_TYPE_FW        = (1 << 0),
    ASCU_AUDIO_TIMER_TYPE_TALKER_FW = (1 << 1)
} ascu_audio_timer_type_t;

/******************************************************
 *                 Type Definitions
 ******************************************************/

/******************************************************
 *                    Structures
 ******************************************************/

/******************************************************
 *               Function Declarations
 ******************************************************/

/******************************************************
 *               Variables Definitions
 ******************************************************/

#ifdef ENABLE_8021AS_TIMING_LOG
static uint32_t g_avb_secs = 0;
static uint32_t g_avb_nanosecs = 0;
static uint32_t g_timer_hi;
static uint32_t g_timer_lo;
static uint32_t g_master_secs;
static uint32_t g_master_nanosecs;
static uint64_t g_cpu_time;
#endif /* ENABLE_8021AS_TIMING_LOG */

/******************************************************
 *               Function Definitions
 ******************************************************/

/**
 * Enable the 802.1AS time functionality.
 *
 * @return @ref platform_result_t
 */
platform_result_t platform_time_enable_8021as(void)
{
    platform_ascu_enable_interrupts( ASCU_TX_START_AVB_INT_MASK | ASCU_RX_START_AVB_INT_MASK );

    /*
     * Enable the ascu_avb_clk.
     */

    platform_pmu_chipcontrol(PMU_CHIPCONTROL_PWM_CLK_ASCU_REG, 0, PMU_CHIPCONTROL_PWM_CLK_ASCU_MASK);

    return PLATFORM_SUCCESS;
}


/**
 * Disable the 802.1AS time functionality.
 *
 * @return @ref platform_result_t
 */
platform_result_t platform_time_disable_8021as(void)
{
    platform_ascu_disable_interrupts( ASCU_TX_START_AVB_INT_MASK | ASCU_RX_START_AVB_INT_MASK );

    return PLATFORM_SUCCESS;
}


/**
 * Read the 802.1AS time.
 *
 * Retrieve the origin timestamp in the last sync message, correct for the
 * intervening interval and return the corrected time in seconds + nanoseconds.
 * Optionally, if audio sample rate is non-zero, return the correlated audio time.
 *
 * @return @ref platform_result_t
 */
static inline platform_result_t platform_time_read_8021as_extended(ascu_audio_timer_type_t type, platform_8021as_time_t* as_time)
{
    platform_result_t             result              = PLATFORM_SUCCESS;
    ascu_network_audio_time_t     ascu_timers;
    uint64_t                      correction;
    uint32_t                      correction_secs;
    uint32_t                      correction_nanosecs;
    uint32_t                      avb_secs;
    uint32_t                      avb_nanosecs;
    uint32_t                      net_timer_rxhi;
    uint32_t                      net_timer_rxlo;
    unsigned int                  lock;
    volatile wlc_avb_timestamp_t* p_avb;

#ifdef ENABLE_8021AS_TIMING_LOG
    wiced_bool_t                  got_update          = WICED_FALSE;
    wiced_bool_t                  perform_check       = WICED_TRUE;
    uint32_t                      prev_avb_secs;
    uint32_t                      prev_avb_nanosecs;
    uint32_t                      prev_timer_hi;
    uint32_t                      prev_timer_lo;
    uint64_t                      prev_cpu_time;
    uint32_t                      prev_master_secs;
    uint32_t                      prev_master_nanosecs;
    uint64_t                      cpu_tick;
    uint64_t                      cpu_tack;
    uint64_t                      cpu_tock;
#endif /* ENABLE_8021AS_TIMING_LOG */

    if (as_time == NULL)
    {
        return PLATFORM_ERROR;
    }

    p_avb = platform_ascu_get_avb_ts();
    if (p_avb == NULL)
    {
        as_time->master_secs         = 0;
        as_time->master_nanosecs     = 0;
        as_time->local_secs          = 0;
        as_time->local_nanosecs      = 0;
        as_time->audio_time_secs     = 0;
        as_time->audio_time_nanosecs = 0;

        return PLATFORM_ERROR;
    }

    WICED_DISABLE_INTERRUPTS();

    /*
     * Seqlock implemented in WLAN driver per references in
     * https://en.wikipedia.org/wiki/Seqlock.
     * Check lock before and after reading AVB time values from the
     * structure.  If the lock is odd at either point, the WLAN CPU
     * may have been updating them while this code was reading. If not,
     * the WLAN  could have started and completed the write between
     * the two checks of the lock, in which case the values of the
     * lock will not be equal.
     */

#ifdef ENABLE_8021AS_TIMING_LOG
    cpu_tick = platform_get_nanosecond_clock_value();
#endif /* ENABLE_8021AS_TIMING_LOG */

    do
    {
        /* invalidate the timestamp for reading & read the pertinent values */
        //platform_dcache_inv_range( (volatile void *)p_avb, sizeof(*p_avb));

        lock = p_avb->lock;
        if (!(lock & 1))
        {
            avb_secs        = p_avb->as_seconds;
            avb_nanosecs    = p_avb->as_nanosecs;
            net_timer_rxhi  = p_avb->net_timer_rxhi;
            net_timer_rxlo  = p_avb->net_timer_rxlo;

            /* Get the current time for the delta since receipt of Sync */
            platform_ascu_read_fw_timers(&ascu_timers);

            /* invalidate the timestamp struct for checking the lock flag again. */
            //platform_dcache_inv_range( (volatile void *)p_avb, sizeof(*p_avb));
        }
    } while ((lock & 1) || (p_avb->lock & 1) || (lock != p_avb->lock));

#ifdef ENABLE_8021AS_TIMING_LOG
    cpu_tack = platform_get_nanosecond_clock_value();

    if ((avb_secs != g_avb_secs) || (avb_nanosecs != g_avb_nanosecs))
    {
        got_update = WICED_TRUE;

        if (g_avb_secs == 0 && g_avb_nanosecs == 0)
        {
            perform_check = WICED_FALSE;
        }
        else
        {
            prev_avb_secs        = g_avb_secs;
            prev_avb_nanosecs    = g_avb_nanosecs;
            prev_timer_hi        = g_timer_hi;
            prev_timer_lo        = g_timer_lo;
            prev_cpu_time        = g_cpu_time;
            prev_master_secs     = g_master_secs;
            prev_master_nanosecs = g_master_nanosecs;
        }

        g_avb_secs     = avb_secs;
        g_avb_nanosecs = avb_nanosecs;
        g_timer_hi     = net_timer_rxhi;
        g_timer_lo     = net_timer_rxlo;
    }

    g_cpu_time = platform_get_nanosecond_clock_value();
#endif /* ENABLE_8021AS_TIMING_LOG */

    WICED_ENABLE_INTERRUPTS();

    /* Deal with audio time if we have a sample rate */
    if ((as_time->audio_sample_rate > 0) && (type != ASCU_AUDIO_TIMER_TYPE_NONE) )
    {
        switch (type)
        {
            case ASCU_AUDIO_TIMER_TYPE_FW:
                result = platform_ascu_convert_atimer(as_time->audio_sample_rate, ascu_timers.audio_timer_hi, ascu_timers.audio_timer_lo,
                                                      &as_time->audio_time_secs, &as_time->audio_time_nanosecs);
                break;

            case ASCU_AUDIO_TIMER_TYPE_TALKER_FW:
                result = platform_ascu_convert_atimer(as_time->audio_sample_rate, ascu_timers.audio_talker_timer_hi, ascu_timers.audio_talker_timer_lo,
                                                      &as_time->audio_time_secs, &as_time->audio_time_nanosecs);
                break;

            default:
                result = PLATFORM_ERROR;
                break;
        }

        if (result != PLATFORM_SUCCESS)
        {
            return result;
        }
    }

    /* Compute the local time */
    platform_ascu_convert_ntimer(ascu_timers.ntimer_hi, ascu_timers.ntimer_lo, &as_time->local_secs, &as_time->local_nanosecs);

    /* Perform the correction calculation in 64-bit math */
    correction = ((((uint64_t)ascu_timers.ntimer_hi << 32) & 0xFFFFFFFF00000000ULL) | ((uint64_t)ascu_timers.ntimer_lo & 0xFFFFFFFFULL)) -
                 ((((uint64_t)net_timer_rxhi << 32) & 0xFFFFFFFF00000000ULL) | ((uint64_t)net_timer_rxlo & 0xFFFFFFFFULL));

    platform_ascu_convert_ntimer((uint32_t)((correction >> 32) & 0xFFFFFFFFULL), (uint32_t) (correction & 0xFFFFFFFFULL), &correction_secs, &correction_nanosecs);

    as_time->master_nanosecs = avb_nanosecs + correction_nanosecs;
    while (as_time->master_nanosecs >= (uint32_t)ONE_BILLION_RAW)
    {
        correction_secs++;
        as_time->master_nanosecs -= (uint32_t)ONE_BILLION_RAW;
    }
    as_time->master_secs = avb_secs + correction_secs;

#ifdef ENABLE_8021AS_TIMING_LOG
    g_master_secs     = as_time->master_secs;
    g_master_nanosecs = as_time->master_nanosecs;

    if (got_update && perform_check)
    {
        int64_t  master_time_diff;
        uint64_t cpu_time_diff;
        int64_t  update_diff;
        int64_t  fw_update_diff;

        master_time_diff = (((int64_t)g_master_secs - (int64_t)prev_master_secs) * (int64_t)ONE_BILLION) + ((int64_t)g_master_nanosecs - (int64_t)prev_master_nanosecs);
        cpu_time_diff    = g_cpu_time - prev_cpu_time;
        update_diff      = master_time_diff - (int64_t)cpu_time_diff;

        master_time_diff = (((int64_t)g_avb_secs - (int64_t)prev_avb_secs) * (int64_t)ONE_BILLION) + ((int64_t)g_avb_nanosecs - (int64_t)prev_avb_nanosecs);
        correction = (((uint64_t)g_timer_hi << 32) | (uint64_t)g_timer_lo) - (((uint64_t)prev_timer_hi << 32) | (uint64_t)prev_timer_lo);
        platform_ascu_convert_ntimer((uint32_t)((correction >> 32) & 0xFFFFFFFFULL), (uint32_t) (correction & 0xFFFFFFFFULL), &correction_secs, &correction_nanosecs);
        fw_update_diff = master_time_diff - (((int64_t)correction_secs * (int64_t)ONE_BILLION) + (int64_t)correction_nanosecs);

        if ((fw_update_diff > MASTER_TIME_GAP_THRESHOLD_MAX_NSECS) || (fw_update_diff < MASTER_TIME_GAP_THRESHOLD_MIN_NSECS) ||
            (update_diff > MASTER_TIME_GAP_THRESHOLD_MAX_NSECS) || (update_diff < MASTER_TIME_GAP_THRESHOLD_MIN_NSECS))
        {
            cpu_tock = platform_get_nanosecond_clock_value();
            printf("%lu,%ld,%ld,%lu,%lu,%lu,%ld\n",
                   (uint32_t)(g_cpu_time/ONE_BILLION), (int32_t)(update_diff/1000LL),(int32_t)(fw_update_diff/1000LL),
                   (uint32_t)((cpu_tack - cpu_tick)/1000ULL), (uint32_t)((g_cpu_time - cpu_tack)/1000ULL), (uint32_t)((cpu_tock - g_cpu_time)/1000ULL),
                   (int32_t)(master_time_diff/1000LL));
        }
    }
#endif /* ENABLE_8021AS_TIMING_LOG */

    return PLATFORM_SUCCESS;
}


platform_result_t platform_time_read_8021as(platform_8021as_time_t* as_time)
{
    return platform_time_read_8021as_extended(ASCU_AUDIO_TIMER_TYPE_FW, as_time);
}
