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
 * Audio timer clock functionality for the BCM43909.
 */

#include "wiced_result.h"
#include "wiced_utilities.h"
#include "platform_peripheral.h"
#include "platform_ascu.h"

/******************************************************
 *                      Macros
 ******************************************************/

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

/******************************************************
 *               Variables Definitions
 ******************************************************/

static wiced_bool_t          audio_timer_enabled   = WICED_FALSE;
static host_semaphore_type_t audio_timer_semaphore;

/******************************************************
 *               Function Definitions
 ******************************************************/

wiced_result_t platform_audio_timer_enable( uint32_t audio_frame_count )
{
    wiced_result_t result = WICED_ERROR;

    wiced_jump_when_not_true( (audio_timer_enabled == WICED_FALSE), _exit );

    result = host_rtos_init_semaphore( &audio_timer_semaphore );
    wiced_jump_when_not_true( (result == WICED_SUCCESS), _exit );

    platform_ascu_disable_interrupts   ( ASCU_ASTP_INT_MASK );
    if ( audio_frame_count > 0 )
    {
        platform_ascu_set_frame_sync_period( audio_frame_count );
        platform_ascu_enable_interrupts    ( ASCU_ASTP_INT_MASK );
    }
    platform_pmu_chipcontrol(PMU_CHIPCONTROL_PWM_CLK_ASCU_REG, 0, PMU_CHIPCONTROL_PWM_CLK_ASCU_MASK);

    audio_timer_enabled = WICED_TRUE;

_exit:
    return result;
}


wiced_result_t platform_audio_timer_disable( void )
{
    wiced_result_t result = WICED_SUCCESS;

    wiced_jump_when_not_true( (audio_timer_enabled == WICED_TRUE), _exit );

    audio_timer_enabled = WICED_FALSE;

    platform_ascu_disable_interrupts   ( ASCU_ASTP_INT_MASK );

    host_rtos_set_semaphore( &audio_timer_semaphore, WICED_FALSE );
    result = host_rtos_deinit_semaphore( &audio_timer_semaphore );

_exit:
    return result;
}


wiced_result_t platform_audio_timer_get_frame_sync( uint32_t timeout_msecs )
{
    wiced_result_t result = WICED_ERROR;

    wiced_jump_when_not_true( (audio_timer_enabled == WICED_TRUE), _exit );

    result = host_rtos_get_semaphore( &audio_timer_semaphore, timeout_msecs, WICED_TRUE );

_exit:
    return result;
}


wiced_result_t platform_audio_timer_set_frame_sync( void )
{
    return host_rtos_set_semaphore( &audio_timer_semaphore, WICED_TRUE );
}


wiced_result_t platform_audio_timer_get_time( uint32_t* time_hi, uint32_t* time_lo )
{
    return (wiced_result_t) platform_ascu_read_fw_audio_timer( time_hi, time_lo );
}


wiced_result_t platform_audio_timer_get_nanoseconds( uint32_t audio_sample_rate, uint32_t* audio_time_secs, uint32_t* audio_time_nanosecs )
{
    platform_result_t result;
    uint32_t          audio_timer_hi;
    uint32_t          audio_timer_lo;

    result  = platform_ascu_read_fw_audio_timer( &audio_timer_hi, &audio_timer_lo );
    result |= platform_ascu_convert_atimer(audio_sample_rate, audio_timer_hi, audio_timer_lo, audio_time_secs, audio_time_nanosecs);

    return (wiced_result_t) result;
}


wiced_result_t platform_audio_timer_get_resolution( uint32_t audio_sample_rate, uint32_t *ticks_per_sec )
{
    return (wiced_result_t) platform_ascu_get_audio_timer_resolution( audio_sample_rate, ticks_per_sec );
}
