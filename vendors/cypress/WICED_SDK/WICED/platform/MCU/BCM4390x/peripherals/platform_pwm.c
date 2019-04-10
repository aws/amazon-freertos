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
#include <stdint.h>
#include "typedefs.h"

#include "osl.h"
#include "hndsoc.h"

#include "platform_peripheral.h"
#include "platform_appscr4.h"
#include "platform_toolchain.h"
#include "platform_pinmux.h"

#include "wwd_assert.h"

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

static wiced_bool_t     pwm_core_inited = WICED_FALSE;
static platform_clock_t pwm_core_clock;

/******************************************************
 *               Function Definitions
 ******************************************************/

static wiced_bool_t
pwm_present( void )
{
    return ( PLATFORM_CHIPCOMMON->clock_control.capabilities_extension.bits.pulse_width_modulation_present != 0 );
}

static volatile pwm_channel_t*
pwm_get_channel( platform_pin_function_t func )
{
    if ( pwm_present() != WICED_TRUE )
    {
        return NULL;
    }

    switch (func)
    {
        case PIN_FUNCTION_PWM0:
            return &PLATFORM_CHIPCOMMON->pwm.pwm_channels[0];

        case PIN_FUNCTION_PWM1:
            return &PLATFORM_CHIPCOMMON->pwm.pwm_channels[1];

        case PIN_FUNCTION_PWM2:
            return &PLATFORM_CHIPCOMMON->pwm.pwm_channels[2];

        case PIN_FUNCTION_PWM3:
            return &PLATFORM_CHIPCOMMON->pwm.pwm_channels[3];

        case PIN_FUNCTION_PWM4:
            return &PLATFORM_CHIPCOMMON->pwm.pwm_channels[4];

        case PIN_FUNCTION_PWM5:
            return &PLATFORM_CHIPCOMMON->pwm.pwm_channels[5];

        default:
            wiced_assert("wrong func", 0);
            return NULL;
    }
}

static wiced_bool_t
pwm_set_clock_src( platform_clock_t clock )
{
    if ( ( clock == CLOCK_HT ) || ( clock == CLOCK_BACKPLANE ) )
    {
        platform_pmu_chipcontrol( PMU_CHIPCONTROL_PWM_CLK_SLOW_REG,
                                  PMU_CHIPCONTROL_PWM_CLK_SLOW_MASK,
                                  0x0 );
    }
    else if ( clock == CLOCK_ALP )
    {
        platform_pmu_chipcontrol( PMU_CHIPCONTROL_PWM_CLK_SLOW_REG,
                                  PMU_CHIPCONTROL_PWM_CLK_SLOW_MASK,
                                  PMU_CHIPCONTROL_PWM_CLK_SLOW_MASK );
    }
    else
    {
        wiced_assert("wrong clock", 0);
        return WICED_FALSE;
    }

    return WICED_TRUE;
}

static uint32_t
pwm_get_clock_freq( void )
{
    if ( (platform_pmu_chipcontrol( PMU_CHIPCONTROL_PWM_CLK_SLOW_REG, 0x0, 0x0 ) & PMU_CHIPCONTROL_PWM_CLK_SLOW_MASK ) == 0 )
    {
        return platform_reference_clock_get_freq( PLATFORM_REFERENCE_CLOCK_BACKPLANE );
    }
    else
    {
        return platform_reference_clock_get_freq( PLATFORM_REFERENCE_CLOCK_ALP );
    }
}

static void
pwm_enable_clock( wiced_bool_t enable )
{
    static uint32_t ref_counter = 0;
    uint32_t        flags;

    WICED_SAVE_INTERRUPTS( flags );

    if ( enable )
    {
        if ( ref_counter == 0 )
        {
            platform_pmu_chipcontrol( PMU_CHIPCONTROL_PWM_CLK_EN_REG,
                                      PMU_CHIPCONTROL_PWM_CLK_EN_MASK,
                                      PMU_CHIPCONTROL_PWM_CLK_EN_MASK );
            OSL_DELAY( 10 ); /* let it stabilize */
        }

        ref_counter++;

        wiced_assert( "overflow", ref_counter != 0 );
    }
    else
    {
        wiced_assert( "unbalanced", ref_counter != 0 );

        ref_counter--;

        if ( ref_counter == 0 )
        {
            platform_pmu_chipcontrol( PMU_CHIPCONTROL_PWM_CLK_EN_REG,
                                      PMU_CHIPCONTROL_PWM_CLK_EN_MASK,
                                      0x0 );
        }
    }

    WICED_RESTORE_INTERRUPTS( flags );
}

static wiced_bool_t
pwm_start_clock( wiced_bool_t start )
{
    platform_mcu_powersave_clock_t reference_clock;

    if ( ( pwm_core_clock == CLOCK_HT ) || ( pwm_core_clock == CLOCK_BACKPLANE ) )
    {
        reference_clock = PLATFORM_MCU_POWERSAVE_CLOCK_BACKPLANE_ON_HT;
    }
    else if ( pwm_core_clock == CLOCK_ALP )
    {
        reference_clock = PLATFORM_MCU_POWERSAVE_CLOCK_ALP_AVAILABLE;
    }
    else
    {
        wiced_assert( "wrong clock", 0 );
        return WICED_FALSE;
    }

    if ( start )
    {
        platform_mcu_powersave_request_clock( reference_clock );
        pwm_enable_clock( WICED_TRUE );
    }
    else
    {
        pwm_enable_clock( WICED_FALSE );
        platform_mcu_powersave_release_clock( reference_clock );
    }

    return WICED_TRUE;
}

static wiced_bool_t
pwm_enable_core( void )
{
    wiced_bool_t ret;
    uint32_t     flags;

    WICED_SAVE_INTERRUPTS( flags );

    if ( pwm_core_inited != WICED_TRUE )
    {
        pwm_core_clock = platform_pwm_getclock();

        osl_core_enable( CC_CORE_ID ); /* PWM is in chipcommon. Enable core before try to access. */

        pwm_core_inited = pwm_set_clock_src( pwm_core_clock );
    }

    ret = pwm_core_inited;

    WICED_RESTORE_INTERRUPTS( flags );

    return ret;
}

platform_result_t
platform_pwm_init( const platform_pwm_t* pwm, uint32_t frequency, float duty_cycle )
{
    platform_result_t       ret;
    volatile pwm_channel_t* chan;
    uint32_t                high_period;
    uint32_t                low_period;
    uint32_t                total_period;
    uint32_t                clock_freq;
    pwm_channel_cycle_cnt_t cycle_cnt;

    wiced_assert( "bad argument", pwm != NULL );

    chan = pwm_get_channel( pwm->func );
    if ( chan == NULL )
    {
        return PLATFORM_BADARG;
    }

    if ( pwm_enable_core() != WICED_TRUE )
    {
        return PLATFORM_ERROR;
    }

    clock_freq = pwm_get_clock_freq();
    if ( clock_freq == 0 )
    {
        return PLATFORM_ERROR;
    }

    total_period =  (clock_freq + frequency / 2) / frequency;
    high_period  =  MIN(total_period, (total_period * duty_cycle / 100.0f + 0.5f));
    low_period   =  total_period - high_period;
    if ( ( high_period > 0xFFFF ) || ( low_period > 0xFFFF ) || ( high_period == 0x0 ) || ( low_period == 0x0 ) )
    {
        return PLATFORM_BADARG;
    }

    ret = platform_pinmux_init( pwm->pin, pwm->func );
    if ( ret != PLATFORM_SUCCESS )
    {
        return ret;
    }

    /* Configure dead time. */
    chan->dead_time.raw             = pwm->dead_time.raw;

    /* Configure cycle timings. */
    cycle_cnt.bits.low_cycle_time   = low_period;
    cycle_cnt.bits.high_cycle_time  = high_period;
    chan->cycle_cnt.raw             = cycle_cnt.raw;

    return PLATFORM_SUCCESS;
}

platform_result_t
platform_pwm_start( const platform_pwm_t* pwm )
{
    volatile pwm_channel_t* chan;
    pwm_channel_ctrl_t      ctrl;

    wiced_assert( "bad argument", pwm != NULL );
    wiced_assert( "not inited", pwm_core_inited == WICED_TRUE );

    chan = pwm_get_channel( pwm->func );
    if ( chan == NULL )
    {
        return PLATFORM_BADARG;
    }

    if ( pwm_start_clock( WICED_TRUE ) != WICED_TRUE )
    {
        return PLATFORM_ERROR;
    }

    ctrl.raw                 = 0;
    ctrl.bits.enable         = 1;
    ctrl.bits.update         = pwm->update_all ? 0 : 1;
    ctrl.bits.update_all     = pwm->update_all ? 1 : 0;
    ctrl.bits.is_single_shot = pwm->is_single_shot ? 1 : 0;
    ctrl.bits.invert         = pwm->invert ? 1 : 0;
    ctrl.bits.alignment      = pwm->alignment;

    chan->ctrl.raw           = ctrl.raw;

    return PLATFORM_SUCCESS;
}

platform_result_t
platform_pwm_stop( const platform_pwm_t* pwm )
{
    volatile pwm_channel_t* chan;

    wiced_assert( "bad argument", pwm != NULL );

    chan = pwm_get_channel( pwm->func );
    if ( chan == NULL )
    {
        return PLATFORM_BADARG;
    }

    chan->ctrl.raw = 0;

    if ( pwm_start_clock( WICED_FALSE ) != WICED_TRUE )
    {
        return PLATFORM_ERROR;
    }

    return PLATFORM_SUCCESS;
}

WEAK NEVER_INLINE platform_clock_t platform_pwm_getclock( void )
{
    return CLOCK_ALP;
}
