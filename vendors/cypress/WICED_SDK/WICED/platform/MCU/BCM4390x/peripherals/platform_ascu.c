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
 */

#include "platform_ascu.h"

#include "internal/wwd_sdpcm.h"

/******************************************************
 *                      Macros
 ******************************************************/

#define ASCU_INT_MASK_TO_CC_INT_MASK( ascu_int_mask, cc_int_mask ) \
    do                                                             \
    {                                                              \
        if ( ascu_int_mask & ASCU_TX_START_AVB_INT_MASK )          \
        {                                                          \
            cc_int_mask |= ASCU_TX_CC_INT_STATUS_MASK;             \
        }                                                          \
                                                                   \
        if ( ascu_int_mask & ASCU_RX_START_AVB_INT_MASK )          \
        {                                                          \
            cc_int_mask |= ASCU_RX_CC_INT_STATUS_MASK;             \
        }                                                          \
                                                                   \
        if ( ascu_int_mask & ASCU_ASTP_INT_MASK )                  \
        {                                                          \
            cc_int_mask |= ASCU_ASTP_CC_INT_STATUS_MASK;           \
        }                                                          \
    } while(0)

/******************************************************
 *                    Constants
 ******************************************************/

/******************************************************
 *                   Enumerations
 ******************************************************/

/******************************************************
 *                 Type Definitions
 ******************************************************/

/******************************************************
 *                    Structures
 ******************************************************/

/******************************************************
 *               Function Declarations
 ******************************************************/

extern wiced_result_t platform_audio_timer_set_frame_sync( void );

/******************************************************
 *               Variables Definitions
 ******************************************************/

static volatile wlc_avb_timestamp_t* g_avb_timestamp;

/* ASCU resides in ChipCommon core and accessed from Apps core via AXI backplane */
static ascu_register_t* const ascu_base = ( ascu_register_t* )( ASCU_REGBASE );

/******************************************************
 *               Function Definitions
 ******************************************************/

static inline void platform_ascu_force_fw_timer_sample(void)
{
     ascu_base->ascu_control |= ASCU_CONTROL_FW_TIMER_SAMPLE;
}


static inline void platform_ascu_enable_offset_update(void)
{
    /* bit[0], enable_update, write a 0->1 transition to enable the offset adjustment (not self-clearing) */
    ascu_base->ascu_control &= (~ASCU_CONTROL_ENABLE_UPDATE_MASK);
    ascu_base->ascu_control |=   ASCU_CONTROL_ENABLE_UPDATE_MASK;
}


platform_result_t platform_ascu_set_frame_sync_period( uint32_t frame_count )
{
    uint32_t fsync_bit_sel = 0;
    uint32_t bitsel = ascu_base->ascu_bitsel_control;

    while ( WICED_TRUE )
    {
        frame_count = frame_count >> 1;
        if ( frame_count == 0 )
        {
            break;
        }
        fsync_bit_sel++;
    }

    fsync_bit_sel                 += (ASCU_FSYNC_POWER_OF_TWO_FACTOR - 1);
    fsync_bit_sel                  = ((fsync_bit_sel << ASCU_BITSEL_CONTROL_FSYNC_SHIFT) & ASCU_BITSEL_CONTROL_FSYNC_MASK);
    ascu_base->ascu_bitsel_control = (bitsel & ~ASCU_BITSEL_CONTROL_FSYNC_MASK) | fsync_bit_sel;

    return PLATFORM_SUCCESS;
}


platform_result_t platform_ascu_set_frame_sync_offset( uint32_t offset_hi, uint32_t offset_lo )
{
    ascu_base->master_clk_offset_hi = offset_hi;
    ascu_base->master_clk_offset_lo = offset_lo;
    platform_ascu_enable_offset_update();
    return PLATFORM_SUCCESS;
}


platform_result_t platform_ascu_get_audio_timer_resolution( uint32_t audio_sample_rate, uint32_t *ticks_per_sec )
{
    platform_result_t result = PLATFORM_SUCCESS;

    switch (audio_sample_rate)
    {
        case 8000:
        case 12000:
        case 16000:
        case 24000:
        case 32000:
        case 48000:
        case 64000:
        case 96000:
        case 192000:
            *ticks_per_sec = AUDIO_TIMER_TICKS_PER_SEC_8000_HZ;
            break;

        case 11025:
        case 22050:
        case 44100:
        case 88200:
        case 176400:
            *ticks_per_sec = AUDIO_TIMER_TICKS_PER_SEC_11025_HZ;
            break;

        default:
            result = PLATFORM_ERROR;
            break;
    }

    return result;
}


platform_result_t platform_ascu_read_frame_sync_audio_timer( uint32_t *time_hi, uint32_t *time_lo )
{
    *time_hi = ascu_base->audio_timer_frame_sync_hi;
    *time_lo = ascu_base->audio_timer_frame_sync_lo;
    return PLATFORM_SUCCESS;
}


platform_result_t platform_ascu_read_fw_audio_timer( uint32_t *time_hi, uint32_t *time_lo )
{
    platform_ascu_force_fw_timer_sample();
    *time_hi = ascu_base->audio_timer_fw_hi;
    *time_lo = ascu_base->audio_timer_fw_lo;
    return PLATFORM_SUCCESS;
}


platform_result_t platform_ascu_read_fw_audio_talker_timer( uint32_t *time_hi, uint32_t *time_lo )
{
    platform_ascu_force_fw_timer_sample();
    *time_hi = ascu_base->audio_talker_timer_fw_hi;
    *time_lo = ascu_base->audio_talker_timer_fw_lo;
    return PLATFORM_SUCCESS;
}


platform_result_t platform_ascu_read_fw_timers( ascu_network_audio_time_t *fw_timers )
{
    platform_ascu_force_fw_timer_sample();
    fw_timers->ntimer_hi             = ascu_base->network_timer_fw_hi;
    fw_timers->ntimer_lo             = ascu_base->network_timer_fw_lo;
    fw_timers->audio_timer_hi        = ascu_base->audio_timer_fw_hi;
    fw_timers->audio_timer_lo        = ascu_base->audio_timer_fw_lo;
    fw_timers->audio_talker_timer_hi = ascu_base->audio_talker_timer_fw_hi;
    fw_timers->audio_talker_timer_lo = ascu_base->audio_talker_timer_fw_lo;
    return PLATFORM_SUCCESS;
}


static wlc_avb_timestamp_t* get_avb_timestamp_addr(void)
{
    wiced_buffer_t buffer;
    wiced_buffer_t response;
    uint32_t*      data;

    data = (uint32_t*)wwd_sdpcm_get_iovar_buffer(&buffer, (uint16_t)4, "avb_timestamp_addr");

    if (data == NULL)
    {
        return NULL;
    }

    if (wwd_sdpcm_send_iovar(SDPCM_GET, buffer, &response, WWD_STA_INTERFACE) != WWD_SUCCESS)
    {
        return NULL;
    }

    data = (uint32_t*)host_buffer_get_current_piece_data_pointer(response);

    host_buffer_release(response, WWD_NETWORK_RX);

    return (wlc_avb_timestamp_t *)*data;
}


platform_result_t platform_ascu_convert_ntimer(uint32_t ntimer_hi, uint32_t ntimer_lo, uint32_t *secs, uint32_t *nanosecs)
{
    uint64_t remainder;
    uint64_t ntimer = ((uint64_t)ntimer_hi << 32) | (uint64_t)ntimer_lo;

    if (ntimer > 0)
    {
        /* The vast majority of correction calculations will be < second */
        if (ntimer > NET_TIMER_TICKS_PER_SEC)
        {
            *secs     = (uint32_t)((uint64_t)(ntimer / NET_TIMER_TICKS_PER_SEC));
            remainder = ntimer - ((uint64_t)(*secs) * NET_TIMER_TICKS_PER_SEC);
        }
        else if (ntimer == NET_TIMER_TICKS_PER_SEC)
        {
            *secs     = 1UL;
            remainder = 0ULL;
        }
        else
        {
            *secs     = 0UL;
            remainder = ntimer;
        }

        if (remainder > 0)
        {
            *nanosecs = (uint32_t)((double)((double)remainder * NET_TIMER_NANOSECS_PER_TICK));
        }
        else
        {
            *nanosecs = 0UL;
        }
    }
    else
    {
        *secs = 0;
        *nanosecs = 0;
    }

    return PLATFORM_SUCCESS;
}


platform_result_t platform_ascu_convert_atimer(uint32_t sample_rate, uint32_t atimer_hi, uint32_t atimer_lo, uint32_t *secs, uint32_t *nanosecs)
{
    uint64_t remainder;
    uint32_t ticks_per_sec;
    uint64_t atimer;

    if (platform_ascu_get_audio_timer_resolution(sample_rate, &ticks_per_sec) != PLATFORM_SUCCESS)
    {
        return PLATFORM_ERROR;
    }

    atimer = ((uint64_t)atimer_hi << 32) | (uint64_t)atimer_lo;

    if (atimer > 0)
    {
        /* The vast majority of correction calculations will be < second */
        if (atimer > (uint64_t)ticks_per_sec)
        {
            *secs     = (uint32_t)((uint64_t)(atimer / (uint64_t)ticks_per_sec));
            remainder = atimer - ((uint64_t)(*secs) * (uint64_t)ticks_per_sec);
        }
        else if (atimer == (uint64_t)ticks_per_sec)
        {
            *secs     = 1UL;
            remainder = 0ULL;
        }
        else
        {
            *secs     = 0UL;
            remainder = atimer;
        }

        if (remainder > 0)
        {
            *nanosecs = (uint32_t)((double)((double)remainder * ((double)ONE_BILLION_RAW/(double)ticks_per_sec)));
        }
        else
        {
            *nanosecs = 0UL;
        }
    }
    else
    {
        *secs = 0;
        *nanosecs = 0;
    }

    return PLATFORM_SUCCESS;
}


static void ascu_chipcommon_interrupt_mask(uint32_t clear_mask, uint32_t set_mask)
{
    uint32_t cc_int_mask;
    uint32_t flags;

    WICED_SAVE_INTERRUPTS(flags);
    cc_int_mask = PLATFORM_CHIPCOMMON->interrupt.mask.raw;

    cc_int_mask = (cc_int_mask & ~clear_mask) | set_mask;

    PLATFORM_CHIPCOMMON->interrupt.mask.raw = cc_int_mask;
    WICED_RESTORE_INTERRUPTS(flags);
}


void platform_ascu_disable_interrupts(uint32_t int_mask)
{
    uint32_t cc_int_mask = 0;

    ASCU_INT_MASK_TO_CC_INT_MASK( int_mask, cc_int_mask );

    /* Disable ASCU interrupt in ChipCommon interrupt mask */
    ascu_chipcommon_interrupt_mask( cc_int_mask, 0 );

    ascu_base->interrupt_mask &= ~int_mask;
}


void platform_ascu_enable_interrupts(uint32_t int_mask)
{
    uint32_t cc_int_mask = 0;

    /*
     * If we haven't gotten the address of the shared AVB timestamp
     * structure from the driver, do it now.
     */

    if ( (g_avb_timestamp == NULL) && (int_mask & (ASCU_TX_START_AVB_INT_MASK | ASCU_RX_START_AVB_INT_MASK)) )
    {
        g_avb_timestamp = get_avb_timestamp_addr();
    }

    ASCU_INT_MASK_TO_CC_INT_MASK( int_mask, cc_int_mask );

    /*
     * Enable those interrupts.
     */

    ascu_base->interrupt_mask |= int_mask;

    /* Enable ASCU interrupt in ChipCommon interrupt mask */
    ascu_chipcommon_interrupt_mask( 0x0, cc_int_mask );
}

platform_result_t platform_ascu_read_raw_ntimer(uint32_t *timer_hi, uint32_t *timer_lo)
{

    *timer_hi = ascu_base->network_timer_rx_hi;
    *timer_lo = ascu_base->network_timer_rx_lo;

    return PLATFORM_SUCCESS;
}

int platform_ascu_read_ntimer(uint32_t *secs, uint32_t *nanosecs)
{

    return platform_ascu_convert_ntimer(ascu_base->network_timer_rx_hi,
                                        ascu_base->network_timer_rx_lo,
                                        secs, nanosecs);
}

platform_result_t platform_ascu_read_raw_fw_ntimer(uint32_t *timer_hi, uint32_t *timer_lo)
{
    platform_ascu_force_fw_timer_sample();
    *timer_hi = ascu_base->network_timer_fw_hi;
    *timer_lo = ascu_base->network_timer_fw_lo;

    return PLATFORM_SUCCESS;
}

int platform_ascu_read_fw_ntimer(uint32_t *secs, uint32_t *nanosecs)
{

    platform_ascu_force_fw_timer_sample();
    return platform_ascu_convert_ntimer(ascu_base->network_timer_fw_hi,
                                        ascu_base->network_timer_fw_lo,
                                        secs, nanosecs);
}

volatile wlc_avb_timestamp_t* platform_ascu_get_avb_ts(void)
{
    return g_avb_timestamp;
}

platform_result_t platform_ascu_init(void)
{
    osl_core_enable( CC_CORE_ID ); /* ASCU is in chipcommon. Enable core before trying to access. */

    /*
     * TODO: any one time initialization. Right now there's nothing to do.
     */

    return PLATFORM_SUCCESS;
}

/******************************************************
 *            IRQ Handlers Definition
 ******************************************************/

void platform_ascu_irq(uint32_t intr_status)
{
    uint16_t int_mask   = ascu_base->interrupt_mask;
    uint16_t int_status = (uint16_t)(intr_status >> ASCU_INTR_BIT_SHIFT_OFFSET);

    /* Clear the interrupt(s) */
    ascu_base->interrupt_status = int_mask;
    ascu_base->interrupt_status = 0;

    /* Turn off ASCU interrupts */
    platform_ascu_disable_interrupts( int_mask );

    if ( (int_status & ASCU_RX_START_AVB_INT_MASK) && (int_mask & ASCU_RX_START_AVB_INT_MASK) )
    {
        /* Rx Start AVB interrupt fired. */

        if (g_avb_timestamp)
        {
            /* WLAN driver sets net_timer_rxlo/rxhi fields*/
            g_avb_timestamp->as_net_timer_rx_lo = ascu_base->network_timer_rx_lo;
            g_avb_timestamp->as_net_timer_rx_hi = ascu_base->network_timer_rx_hi;
        }
    }

    if ( (int_status & ASCU_TX_START_AVB_INT_MASK) && (int_mask & ASCU_TX_START_AVB_INT_MASK) )
    {
        /* Tx Start AVB interrupt fired. */
    }

    if( (int_status & ASCU_ASTP_INT_MASK) && (int_mask & ASCU_ASTP_INT_MASK) )
    {
        /* ASCU frame sync interrupt */
        platform_audio_timer_set_frame_sync( );
    }
    /* Turn ASCU interrupts back on */
    platform_ascu_enable_interrupts( int_mask );
}
