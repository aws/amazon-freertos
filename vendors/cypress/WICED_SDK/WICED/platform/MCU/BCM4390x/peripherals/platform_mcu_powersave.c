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
 *  MCU powersave implementation
 */

#include "platform_config.h"
#include "platform_peripheral.h"
#include "platform_appscr4.h"

#include "wwd_assert.h"
#include "wwd_rtos.h"

#include "cr4.h"

#include "typedefs.h"
#include "wiced_osl.h"
#include "sbchipc.h"

#include "wiced_low_power.h"
#include "wiced_time.h"
#include "wiced_power_logger.h"

/******************************************************
 *                      Macros
 ******************************************************/

#define POWERSAVE_MOVING_AVERAGE_PARAM 128
#if PLATFORM_TICK_STATS
#define POWERSAVE_MOVING_AVERAGE_CALC( var, add ) \
    do \
    { \
        (var) *= POWERSAVE_MOVING_AVERAGE_PARAM - 1; \
        (var) += (add); \
        (var) /= POWERSAVE_MOVING_AVERAGE_PARAM; \
    } \
    while ( 0 )
#else
#define POWERSAVE_MOVING_AVERAGE_CALC( var, add )
#endif /* PLATFORM_TICK_STATS */

#define PMU_RES_WLAN_UP_EVENT_MASK            PMU_RES_MASK( PMU_RES_WLAN_UP_EVENT )

#ifdef DEBUG
#define POWERSAVE_WLAN_EVENT_WAIT_MS          1000
#define POWERSAVE_WLAN_EVENT_ASSERT_SEC       1
#define POWERSAVE_WLAN_TIMER_ASSERT_SEC       1
#define POWERSAVE_APPS_CLOCK_WAIT_ASSERT_SEC  1
#else
#define POWERSAVE_WLAN_EVENT_WAIT_MS          NEVER_TIMEOUT
#endif

#define POWERSAVE_CLOCKSTATUS_REG             PLATFORM_CLOCKSTATUS_REG(PLATFORM_APPSCR4_REGBASE(0x0))

#define POWERSAVE_MCU_GPIO_WAKE_UP_PIN        PIN_GPIO_0 /* Only this single pin can be used to wake-up if GCI GPIOs are not used */

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

static wiced_bool_t                  powersave_is_warmboot              = WICED_FALSE;

#if PLATFORM_TICK_POWERSAVE && PLATFORM_TICK_STATS
/* Statistic variables */
float                                powersave_stat_avg_sleep_ticks     = 0;
float                                powersave_stat_avg_run_ticks       = 0;
float                                powersave_stat_avg_load            = 0;
float                                powersave_stat_avg_ticks_req       = 0;
float                                powersave_stat_avg_ticks_adj       = 0;
uint32_t                             powersave_stat_call_number         = 0;
uint32_t                             powersave_stat_power_down_perm     = 0;
uint32_t                             powersave_stat_power_down_not_perm = 0;
#endif /* PLATFORM_TICK_POWERSAVE && PLATFORM_TICK_STATS */

#if PLATFORM_TICK_POWERSAVE || PLATFORM_APPS_POWERSAVE
static int                           powersave_disable_counter          = PLATFORM_MCU_POWERSAVE_INIT_STATE;
#endif

#if PLATFORM_TICK_POWERSAVE
static platform_tick_powersave_mode_t powersave_tick_mode               = PLATFORM_TICK_POWERSAVE_MODE_INIT;
#endif

#if PLATFORM_APPS_POWERSAVE
static platform_mcu_powersave_mode_t powersave_mcu_mode                 = PLATFORM_MCU_POWERSAVE_MODE_MAX; /* Initial state is unknown */
static uint32_t                      powersave_mcu_clock_ref_counters[ PLATFORM_MCU_POWERSAVE_CLOCK_MAX ];
#endif /* PLATFORM_APPS_POWERSAVE */

#if PLATFORM_WLAN_POWERSAVE
static int                           powersave_wlan_res_ref_counter     = 0;
static host_semaphore_type_t         powersave_wlan_res_event;
static uint32_t                      powersave_wlan_res_event_ack_stamp = 0;
static host_semaphore_type_t         powersave_wlan_res_lock;
#if PLATFORM_WLAN_POWERSAVE_STATS
static uint32_t                      powersave_wlan_res_call_counter    = 0;
static uint32_t                      powersave_wlan_res_up_begin_stamp  = 0;
static uint32_t                      powersave_wlan_res_up_time         = 0;
static uint32_t                      powersave_wlan_res_wait_up_time    = 0;
#endif /* PLATFORM_WLAN_POWERSAVE_STATS */
#endif /* PLATFORM_WLAN_POWERSAVE */

/******************************************************
 *               Function Definitions
 ******************************************************/

wiced_bool_t
platform_mcu_powersave_is_permitted( void )
{
#if PLATFORM_TICK_POWERSAVE || PLATFORM_APPS_POWERSAVE
    return ( powersave_disable_counter == 0 ) ? WICED_TRUE : WICED_FALSE;
#else
    return ( PLATFORM_MCU_POWERSAVE_INIT_STATE == 0 ) ? WICED_TRUE : WICED_FALSE;
#endif
}

static void
platform_mcu_powersave_fire_event( void )
{
    const wiced_bool_t            permission = platform_mcu_powersave_is_permitted();
    const platform_tick_command_t command    = permission ? PLATFORM_TICK_COMMAND_POWERSAVE_BEGIN : PLATFORM_TICK_COMMAND_POWERSAVE_END;

    platform_tick_execute_command( command );
}

static platform_result_t
platform_mcu_powersave_add_disable_counter( int add )
{
#if PLATFORM_TICK_POWERSAVE || PLATFORM_APPS_POWERSAVE
    uint32_t flags;

    WICED_SAVE_INTERRUPTS( flags );

    powersave_disable_counter += add;
    wiced_assert("unbalanced powersave calls", powersave_disable_counter >= 0);

    platform_mcu_powersave_fire_event();

    WICED_RESTORE_INTERRUPTS( flags );

    return PLATFORM_SUCCESS;
#else
    return PLATFORM_FEATURE_DISABLED;
#endif /* #if PLATFORM_TICK_POWERSAVE || PLATFORM_APPS_POWERSAVE */
}

#if PLATFORM_APPS_POWERSAVE
static wiced_bool_t
platform_mcu_powersave_is_clock_ready( platform_mcu_powersave_clock_t clock )
{
    volatile clock_control_status_t* reg    = POWERSAVE_CLOCKSTATUS_REG;
    uint32_t                         result = 0;

    switch ( clock )
    {
        case PLATFORM_MCU_POWERSAVE_CLOCK_ALP_AVAILABLE:
            result = reg->bits.alp_clock_available;
            break;

        case PLATFORM_MCU_POWERSAVE_CLOCK_HT_AVAILABLE:
            result = reg->bits.ht_clock_available;
            break;

        case PLATFORM_MCU_POWERSAVE_CLOCK_BACKPLANE_ON_ILP:
            result = 1;
            break;

        case PLATFORM_MCU_POWERSAVE_CLOCK_BACKPLANE_ON_ALP:
            result = reg->bits.bp_on_alp || reg->bits.bp_on_ht;
            break;

        case PLATFORM_MCU_POWERSAVE_CLOCK_BACKPLANE_ON_HT:
            result = reg->bits.bp_on_ht;
            break;

        default:
            wiced_assert( "unhandled element" , 0 );
            break;
    }

    return result ? WICED_TRUE : WICED_FALSE;
}

static void
platform_mcu_powersave_set_clock( platform_mcu_powersave_clock_t clock, wiced_bool_t set )
{
    volatile clock_control_status_t* reg = POWERSAVE_CLOCKSTATUS_REG;

    switch ( clock )
    {
        case PLATFORM_MCU_POWERSAVE_CLOCK_ALP_AVAILABLE:
            reg->bits.alp_avail_request = set ? 1 : 0;
            break;

        case PLATFORM_MCU_POWERSAVE_CLOCK_HT_AVAILABLE:
            reg->bits.ht_avail_request = set ? 1 : 0;
            break;

        case PLATFORM_MCU_POWERSAVE_CLOCK_BACKPLANE_ON_ILP:
            reg->bits.force_ilp_request = set ? 1 : 0;
            break;

        case PLATFORM_MCU_POWERSAVE_CLOCK_BACKPLANE_ON_ALP:
            reg->bits.force_alp_request = set ? 1 : 0;
            break;

        case PLATFORM_MCU_POWERSAVE_CLOCK_BACKPLANE_ON_HT:
            reg->bits.force_ht_request = set ? 1 : 0;
            break;

        default:
            wiced_assert( "unhandled element" , 0 );
            break;
    }
}
#endif /* PLATFORM_APPS_POWERSAVE */

void
platform_mcu_powersave_request_clock( platform_mcu_powersave_clock_t clock )
{
#if PLATFORM_APPS_POWERSAVE
    uint32_t flags;

    wiced_assert( "wrong clock" , clock < PLATFORM_MCU_POWERSAVE_CLOCK_MAX );

    WICED_SAVE_INTERRUPTS( flags );

    if ( powersave_mcu_clock_ref_counters[ clock ] == 0 )
    {
        platform_mcu_powersave_set_clock( clock, WICED_TRUE );
    }

    powersave_mcu_clock_ref_counters[ clock ]++;

    wiced_assert( "wrapped counter", powersave_mcu_clock_ref_counters[ clock ] != 0 );

    WICED_RESTORE_INTERRUPTS( flags );

    PLATFORM_TIMEOUT_BEGIN( start_stamp );
    while ( !platform_mcu_powersave_is_clock_ready( clock ) )
    {
        PLATFORM_TIMEOUT_SEC_ASSERT( "wait clock for too long", start_stamp, platform_mcu_powersave_is_clock_ready( clock ), POWERSAVE_APPS_CLOCK_WAIT_ASSERT_SEC );
    }
#endif /* PLATFORM_APPS_POWERSAVE */
}

void
platform_mcu_powersave_release_clock( platform_mcu_powersave_clock_t clock )
{
#if PLATFORM_APPS_POWERSAVE
    uint32_t flags;

    wiced_assert( "wrong clock" , clock < PLATFORM_MCU_POWERSAVE_CLOCK_MAX );

    WICED_SAVE_INTERRUPTS( flags );

    wiced_assert( "unbalanced counter", powersave_mcu_clock_ref_counters[ clock ] != 0 );

    powersave_mcu_clock_ref_counters[ clock ]--;

    if ( powersave_mcu_clock_ref_counters[ clock ] == 0 )
    {
        platform_mcu_powersave_set_clock( clock, WICED_FALSE );
    }

    WICED_RESTORE_INTERRUPTS( flags );
#endif /* PLATFORM_APPS_POWERSAVE */
}

uint32_t
platform_mcu_powersave_get_clock_request_counter( platform_mcu_powersave_clock_t clock )
{
#if PLATFORM_APPS_POWERSAVE
    return powersave_mcu_clock_ref_counters[ clock ];
#else
    return 0;
#endif
}

void
platform_mcu_powersave_set_mode( platform_mcu_powersave_mode_t mode )
{
#if PLATFORM_APPS_POWERSAVE
    const wiced_bool_t deep_sleep = ( mode == PLATFORM_MCU_POWERSAVE_MODE_DEEP_SLEEP ) ? WICED_TRUE : WICED_FALSE;

    /*
     * Force power switches on for non deep-sleep modes, otherwise clear forcing.
     * If forced on then APPS is not going into deep-sleep even if all resources are down and clocks switched off.
     */
    platform_pmu_chipcontrol( PMU_CHIPCONTROL_APP_DIGITAL_POWER_FORCE_REG,
                              PMU_CHIPCONTROL_APP_DIGITAL_POWER_FORCE_MASK,
                              deep_sleep ? 0x0 : PMU_CHIPCONTROL_APP_DIGITAL_POWER_FORCE_EN );
    platform_pmu_chipcontrol( PMU_CHIPCONTROL_APP_SOCSRAM_POWER_FORCE_REG,
                              PMU_CHIPCONTROL_APP_SOCSRAM_POWER_FORCE_MASK,
                              deep_sleep ? 0x0 : PMU_CHIPCONTROL_APP_SOCSRAM_POWER_FORCE_EN );

#ifdef WICED_NO_WIFI
    /*
     * If there is WLAN firmware, expected it drops resources low.
     * Otherwise we do it here.
     */
    PLATFORM_PMU->min_res_mask = PMU_RES_DEEP_SLEEP_MASK;
#endif

    powersave_mcu_mode = mode;
#else
    UNUSED_PARAMETER( mode );
#endif /* PLATFORM_APPS_POWERSAVE */
}

platform_mcu_powersave_mode_t
platform_mcu_powersave_get_mode( void )
{
#if PLATFORM_APPS_POWERSAVE
    return powersave_mcu_mode;
#else
    return PLATFORM_MCU_POWERSAVE_MODE_MAX;
#endif
}

void
platform_mcu_powersave_set_tick_mode( platform_tick_powersave_mode_t mode )
{
#if PLATFORM_TICK_POWERSAVE
    powersave_tick_mode = mode;
#else
    UNUSED_PARAMETER( mode );
#endif
}

platform_tick_powersave_mode_t
platform_mcu_powersave_get_tick_mode( void )
{
#if PLATFORM_TICK_POWERSAVE
    return powersave_tick_mode;
#else
    return PLATFORM_TICK_POWERSAVE_MODE_MAX;
#endif
}

platform_result_t
platform_mcu_powersave_init( void )
{
    platform_result_t res = PLATFORM_FEATURE_DISABLED;

#if PLATFORM_APPS_POWERSAVE
    /* Define resource mask used to wake up application domain. */
    PLATFORM_PMU->res_req_mask1 = PMU_RES_APPS_UP_MASK;

    if ( !powersave_is_warmboot )
    {
        /* To reduce deep-sleep current reduce voltage used in deep-sleep. */
        platform_pmu_regulatorcontrol( PMU_REGULATOR_LPLDO1_REG,
                                       PMU_REGULATOR_LPLDO1_MASK,
                                       PLATFORM_LPLDO_VOLTAGE );

        /*
         * Spread over the time power-switch up-delays.
         * This should reduce inrush current which may break digital logic functionality.
         */
        platform_pmu_chipcontrol( PMU_CHIPCONTROL_APP_POWER_UP_DELAY_REG,
                                  PMU_CHIPCONTROL_APP_POWER_UP_DELAY_DIGITAL_MASK |
                                  PMU_CHIPCONTROL_APP_POWER_UP_DELAY_SOCSRAM_MASK |
                                  PMU_CHIPCONTROL_APP_POWER_UP_DELAY_VDDM_MASK,
                                  PMU_CHIPCONTROL_APP_POWER_UP_DELAY_DIGITAL_VAL(0xF) |
                                  PMU_CHIPCONTROL_APP_POWER_UP_DELAY_SOCSRAM_VAL(0x1) |
                                  PMU_CHIPCONTROL_APP_POWER_UP_DELAY_VDDM_VAL(0x1) );

        /* Increase time-up to accomodate above change which spreads power-switch up-delays. */
        platform_pmu_res_updown_time( PMU_RES_APP_DIGITAL_PWRSW,
                                      PMU_RES_UPDOWN_TIME_UP_MASK,
                                      PMU_RES_UPDOWN_TIME_UP_VAL(38) );

        /* Increase VDDM pwrsw up time and make digital pwrsw to depend on VDDM pwrsw to reduce inrush current */
        platform_pmu_res_updown_time( PMU_RES_APP_VDDM_PWRSW,
                                      PMU_RES_UPDOWN_TIME_UP_MASK,
                                      PMU_RES_UPDOWN_TIME_UP_VAL(12) );
        platform_pmu_res_dep_mask( PMU_RES_APP_DIGITAL_PWRSW,
                                   PMU_RES_MASK(PMU_RES_APP_VDDM_PWRSW),
                                   PMU_RES_MASK(PMU_RES_APP_VDDM_PWRSW) );

        /* Force app always-on memory on. */
        platform_pmu_chipcontrol( PMU_CHIPCONTROL_APP_VDDM_POWER_FORCE_REG,
                                  PMU_CHIPCONTROL_APP_VDDM_POWER_FORCE_MASK,
                                  PMU_CHIPCONTROL_APP_VDDM_POWER_FORCE_EN );

        /*
         * Set deep-sleep flag.
         * It is reserved for software.
         * Does not trigger any hardware reaction.
         * Used by software during warm boot to know whether it should go normal boot path or warm boot.
         */
        platform_gci_chipcontrol( GCI_CHIPCONTROL_SW_DEEP_SLEEP_FLAG_REG,
                                  GCI_CHIPCONTROL_SW_DEEP_SLEEP_FLAG_MASK,
                                  GCI_CHIPCONTROL_SW_DEEP_SLEEP_FLAG_SET );
    }

    platform_mcu_powersave_set_mode( PLATFORM_MCU_POWERSAVE_MODE_INIT );

    res = PLATFORM_SUCCESS;
#endif /* PLATFORM_APPS_POWERSAVE */

#if PLATFORM_WLAN_POWERSAVE
    host_rtos_init_semaphore( &powersave_wlan_res_lock );
    host_rtos_set_semaphore ( &powersave_wlan_res_lock, WICED_FALSE );

    host_rtos_init_semaphore( &powersave_wlan_res_event );

    if ( !powersave_is_warmboot )
    {
        /* When wait WLAN up this is resources we are waiting for and for which need interrupt to be generated. */
        PLATFORM_PMU->res_event1 = PMU_RES_WLAN_UP_EVENT_MASK;

        /*
         * Resource requesting via writing to res_req_timer1 has latency.
         * WLAN due to this latency has few ILP clocks to go down after the APPS write to res_req_timer1 and check that PMU_RES_WL_CORE_READY_BUF is up.
         * PMU_RES_WL_CORE_READY_BUF can go down, this is fine, but PMU_RES_WL_CORE_READY must not.
         * To avoid this let's make sure PMU_RES_WL_CORE_READY_BUF down time is large enough to compensate latency.
         */
        platform_pmu_res_updown_time( PMU_RES_WL_CORE_READY_BUF,
                                      PMU_RES_UPDOWN_TIME_DOWN_MASK,
                                      MAX( platform_pmu_res_updown_time( PMU_RES_WL_CORE_READY_BUF, 0, 0) & PMU_RES_UPDOWN_TIME_DOWN_MASK,
                                           PMU_RES_UPDOWN_TIME_DOWN_VAL( PMU_MAX_WRITE_LATENCY_ILP_TICKS ) ) );
    }
#endif /* PLATFORM_WLAN_POWERSAVE */

    platform_mcu_powersave_fire_event();

    return res;
}

void
platform_mcu_powersave_warmboot_init( void )
{
    powersave_is_warmboot = WICED_FALSE;

    if ( platform_boot_is_reset() )
    {
        /* Board was resetted. */
    }
    else if ( ( platform_gci_chipcontrol( GCI_CHIPCONTROL_SW_DEEP_SLEEP_FLAG_REG,
                                          0x0,
                                          0x0) & GCI_CHIPCONTROL_SW_DEEP_SLEEP_FLAG_MASK ) != GCI_CHIPCONTROL_SW_DEEP_SLEEP_FLAG_SET )
    {
        /* Previously booted software does not want us to run warm reboot sequence. */
    }
    else
    {
        powersave_is_warmboot = WICED_TRUE;
    }
}

wiced_bool_t
platform_mcu_powersave_is_warmboot( void )
{
    return powersave_is_warmboot;
}

platform_result_t
platform_mcu_powersave_disable( void )
{
    return platform_mcu_powersave_add_disable_counter( 1 );
}

platform_result_t
platform_mcu_powersave_enable( void )
{
    return platform_mcu_powersave_add_disable_counter( -1 );
}

#if PLATFORM_TICK_POWERSAVE

inline static uint32_t
platform_mcu_appscr4_stamp( void )
{
    return platform_tick_get_time( PLATFORM_TICK_GET_FAST_TIME_STAMP );
}

inline static uint32_t
platform_mcu_pmu_stamp( void )
{
    uint32_t time = platform_tick_get_time( PLATFORM_TICK_GET_SLOW_TIME_STAMP );
    return (uint64_t)CPU_CLOCK_HZ * time / platform_reference_clock_get_freq( PLATFORM_REFERENCE_CLOCK_ILP );
}

static void
platform_mcu_suspend( void )
{
#if PLATFORM_APPS_POWERSAVE
    wiced_bool_t permission = platform_mcu_powersave_is_permitted();
#endif

    WICED_SLEEP_CALL_EVENT_HANDLERS( permission, WICED_SLEEP_EVENT_ENTER );

#if PLATFORM_TICK_STATS

    static uint32_t begin_stamp_appscr4, end_stamp_appscr4;
#if PLATFORM_TICK_PMU
    uint32_t begin_stamp_pmu = platform_mcu_pmu_stamp();
#endif
    uint32_t sleep_ticks;
    float total_ticks;

    begin_stamp_appscr4 = platform_mcu_appscr4_stamp();

    if ( end_stamp_appscr4 != 0 )
    {
        POWERSAVE_MOVING_AVERAGE_CALC( powersave_stat_avg_run_ticks, begin_stamp_appscr4 - end_stamp_appscr4 );
    }

    if ( permission )
        WICED_POWER_LOGGER( EVENT_PROC_ID_MCU, EVENT_ID_POWERSTATE, powersave_mcu_mode ? EVENT_DESC_POWER_SLEEP : EVENT_DESC_POWER_DEEPSLEEP );


    cpu_wait_for_interrupt();

    if(permission)
    {
#if PLATFORM_APPS_POWERSAVE
        WICED_POWER_LOGGER( EVENT_PROC_ID_MCU, EVENT_ID_POWERSTATE, EVENT_DESC_POWER_ACTIVE1 );
#else
        WICED_POWER_LOGGER( EVENT_PROC_ID_MCU, EVENT_ID_POWERSTATE, EVENT_DESC_POWER_ACTIVE2 );
#endif
    }

    end_stamp_appscr4 = platform_mcu_appscr4_stamp();

#if PLATFORM_TICK_PMU
    sleep_ticks = platform_mcu_pmu_stamp() - begin_stamp_pmu;
#else
    sleep_ticks = end_stamp_appscr4 - begin_stamp_appscr4;
#endif
    POWERSAVE_MOVING_AVERAGE_CALC( powersave_stat_avg_sleep_ticks, sleep_ticks );

    total_ticks             = powersave_stat_avg_run_ticks + powersave_stat_avg_sleep_ticks;
    powersave_stat_avg_load = ( total_ticks > 0.0 ) ? ( powersave_stat_avg_run_ticks / total_ticks ) : 0.0;

    powersave_stat_call_number++;

#else

    cpu_wait_for_interrupt();

#endif /* PLATFORM_TICK_STATS */

    WICED_SLEEP_CALL_EVENT_HANDLERS( permission, WICED_SLEEP_EVENT_CANCEL );
}

#endif /* PLATFORM_TICK_POWERSAVE */

void
platform_mcu_powersave_sleep( uint32_t ticks, platform_tick_sleep_force_interrupts_mode_t mode )
{
#if PLATFORM_TICK_POWERSAVE
    wiced_time_t curr_time_ms;
    uint32_t     sleep_ticks;
    uint32_t     flags;

    WICED_SAVE_INTERRUPTS( flags );

    if ( ticks == 0 )
    {
        /*
         * When ticks is zero, we ask to sleep indefinitely, not wake-up by timer.
         * We probably don't want be reset via watchdog while sleeping.
         * If instead want to have watchdog during sleep, then please specify time to sleep, perhaps large one.
         */
        platform_watchdog_stop( );
    }
    else
    {
        platform_watchdog_kick_milliseconds( 2 * ticks / SYSTICK_FREQUENCY * 1000 );
    }

    wiced_time_get_time( &curr_time_ms );

    sleep_ticks = platform_tick_sleep_force( platform_mcu_suspend, ticks, mode );

    curr_time_ms += sleep_ticks * 1000 / SYSTICK_FREQUENCY;

    wiced_time_set_time( &curr_time_ms );

    platform_watchdog_kick();

    WICED_RESTORE_INTERRUPTS( flags );
#else
    UNUSED_PARAMETER( ticks );
#endif /* PLATFORM_TICK_POWERSAVE */
}

/* Expect to be called by RTOS with interrupts disabled */
void
platform_idle_hook( void )
{
#if PLATFORM_TICK_POWERSAVE
    (void)platform_tick_sleep_rtos( platform_mcu_suspend, 0, platform_mcu_powersave_is_permitted() );
#endif
}

/*
 * Expect to be called by RTOS with interrupts disabled.
 * Function to be called only if platform_power_down_permission() returned true.
 * "ticks" parameter is number of ticks after the nearest periodic tick.
 */
uint32_t
platform_power_down_hook( uint32_t ticks )
{
#if PLATFORM_TICK_POWERSAVE
    uint32_t ret = platform_tick_sleep_rtos( platform_mcu_suspend, ticks, platform_mcu_powersave_is_permitted() );

    POWERSAVE_MOVING_AVERAGE_CALC( powersave_stat_avg_ticks_req, ticks );
    POWERSAVE_MOVING_AVERAGE_CALC( powersave_stat_avg_ticks_adj, ret );

    return ret;
#else
    UNUSED_PARAMETER( ticks );
    return 0;
#endif /* PLATFORM_TICK_POWERSAVE */
}

/* Expect to be called by RTOS with interrupts disabled */
int
platform_power_down_permission( void )
{
#if PLATFORM_TICK_POWERSAVE
    int          ret = 1;
    wiced_bool_t permission = platform_mcu_powersave_is_permitted();

    if ( ( powersave_tick_mode == PLATFORM_TICK_POWERSAVE_MODE_TICKLESS_NEVER ) ||
         ( ( powersave_tick_mode == PLATFORM_TICK_POWERSAVE_MODE_TICKLESS_IF_MCU_POWERSAVE_ENABLED ) && !permission ) )
    {
        (void)platform_tick_sleep_rtos( platform_mcu_suspend, 0, permission );
        ret = 0;
    }

#if PLATFORM_TICK_STATS
    if ( ret )
    {
        powersave_stat_power_down_perm++;
    }
    else
    {
        powersave_stat_power_down_not_perm++;
    }
#endif /* PLATFORM_TICK_STATS */

    return ret;
#else
    return 0;
#endif /* PLATFORM_TICK_POWERSAVE */
}

#if PLATFORM_APPS_POWERSAVE
platform_pin_config_t
platform_mcu_powersave_gpio_wakeup_config_to_pin_config( platform_mcu_powersave_gpio_wakeup_config_t config )
{
    switch ( config )
    {
        default:
            wiced_assert( "bad config", 0 );
        case PLATFORM_MCU_POWERSAVE_GPIO_WAKEUP_CONFIG_HIGH_IMPEDANCE:
            return INPUT_HIGH_IMPEDANCE;

        case PLATFORM_MCU_POWERSAVE_GPIO_WAKEUP_CONFIG_PULL_UP:
            return INPUT_PULL_UP;

        case PLATFORM_MCU_POWERSAVE_GPIO_WAKEUP_CONFIG_PULL_DOWN:
            return INPUT_PULL_DOWN;
    }

}

static platform_gci_gpio_irq_trigger_t
platform_mcu_powersave_gpio_irq_trigger_to_gci_gpio_irq_trigger( platform_gpio_irq_trigger_t trigger )
{
    switch ( trigger )
    {
        default:
            wiced_assert( "bad config ", 0 );
        case IRQ_TRIGGER_RISING_EDGE:
            return PLATFORM_GCI_GPIO_IRQ_TRIGGER_RISING_EDGE;
        case IRQ_TRIGGER_FALLING_EDGE:
             return PLATFORM_GCI_GPIO_IRQ_TRIGGER_FALLING_EDGE;
        case IRQ_TRIGGER_BOTH_EDGES:
             return ( PLATFORM_GCI_GPIO_IRQ_TRIGGER_RISING_EDGE | PLATFORM_GCI_GPIO_IRQ_TRIGGER_FALLING_EDGE );
        case IRQ_TRIGGER_LEVEL_HIGH:
             return PLATFORM_GCI_GPIO_IRQ_TRIGGER_LEVEL_HIGH;
        case IRQ_TRIGGER_LEVEL_LOW:
             return PLATFORM_GCI_GPIO_IRQ_TRIGGER_LEVEL_HIGH;
    }
}

static wiced_bool_t
platform_mcu_powersave_gci_gpio_wakeup_is_valid_pin( platform_pin_t pin )
{
    return ( ( pin >= PIN_GPIO_0 ) && ( pin <= PIN_GPIO_15 ) && ( pin != PIN_GPIO_13 ) ) ? WICED_TRUE : WICED_FALSE;
}

static unsigned int
platform_mcu_powersave_gpio_number( platform_pin_t gpio_pin )
{
    return ( gpio_pin - PIN_GPIO_0 );
}

typedef enum
{
    PLATFORM_MCU_POWERSAVE_MISC_WAKEUP_CONFIG_PMU_WL_DEV_WAKE,
    PLATFORM_MCU_POWERSAVE_MISC_WAKEUP_CONFIG_PMU_GCI2WL_WAKE,
    PLATFORM_MCU_POWERSAVE_MISC_WAKEUP_CONFIG_PMU_GCI_WAKE,
    PLATFORM_MCU_POWERSAVE_MISC_WAKEUP_CONFIG_GCI_GPIO_WAKE,
    PLATFORM_MCU_POWERSAVE_MISC_WAKEUP_CONFIG_MAX /* not valid, use to denote number of elements */
} platform_mcu_powersave_misc_wakeup_config_t;

static void
platform_mcu_powersave_misc_wakeup_config_enable( platform_mcu_powersave_misc_wakeup_config_t config, wiced_bool_t enable )
{
    static uint32_t WICED_DEEP_SLEEP_SAVED_VAR( ref_counters[PLATFORM_MCU_POWERSAVE_MISC_WAKEUP_CONFIG_MAX] ) = {0};
    uint32_t flags;

    wiced_assert( "bad config", config < PLATFORM_MCU_POWERSAVE_MISC_WAKEUP_CONFIG_MAX );

    WICED_SAVE_INTERRUPTS( flags );

    if ( enable )
    {
        if ( ref_counters[ config ] == 0 )
        {
            switch ( config )
            {
                case PLATFORM_MCU_POWERSAVE_MISC_WAKEUP_CONFIG_PMU_WL_DEV_WAKE:
                    platform_pmu_chipcontrol( PMU_CHIPCONTROL_WL_DEV_WAKE_REG,
                                              PMU_CHIPCONTROL_WL_DEV_WAKE_MASK,
                                              PMU_CHIPCONTROL_WL_DEV_WAKE_EN );
                    break;

                case PLATFORM_MCU_POWERSAVE_MISC_WAKEUP_CONFIG_PMU_GCI2WL_WAKE:
                    platform_pmu_chipcontrol( PMU_CHIPCONTROL_GCI2WL_WAKE_REG,
                                              PMU_CHIPCONTROL_GCI2WL_WAKE_MASK,
                                              PMU_CHIPCONTROL_GCI2WL_WAKE_EN );
                    break;

                case PLATFORM_MCU_POWERSAVE_MISC_WAKEUP_CONFIG_PMU_GCI_WAKE:
                    PLATFORM_PMU->ext_wake_mask1.bits.gci = 1;
                    break;

                case PLATFORM_MCU_POWERSAVE_MISC_WAKEUP_CONFIG_GCI_GPIO_WAKE:
                    GCI_WAKEMASK_REG->bits.gci_gpio_wake = 1;
                    break;

                default:
                    wiced_assert( "not supported config", 0 );
                    break;
            }
        }

        ref_counters[ config ]++;

        wiced_assert( "wrapped counter", ref_counters[ config ] != 0 );
    }
    else
    {
        wiced_assert( "unbalanced counter", ref_counters[ config ] != 0 );

        ref_counters[ config ]--;

        if ( ref_counters[ config ] == 0 )
        {
            switch ( config )
            {
                case PLATFORM_MCU_POWERSAVE_MISC_WAKEUP_CONFIG_PMU_WL_DEV_WAKE:
                    platform_pmu_chipcontrol( PMU_CHIPCONTROL_WL_DEV_WAKE_REG,
                                              PMU_CHIPCONTROL_WL_DEV_WAKE_MASK,
                                              0x0 );
                    break;

                case PLATFORM_MCU_POWERSAVE_MISC_WAKEUP_CONFIG_PMU_GCI2WL_WAKE:
                    platform_pmu_chipcontrol( PMU_CHIPCONTROL_GCI2WL_WAKE_REG,
                                              PMU_CHIPCONTROL_GCI2WL_WAKE_MASK,
                                              0x0 );
                    break;

                case PLATFORM_MCU_POWERSAVE_MISC_WAKEUP_CONFIG_PMU_GCI_WAKE:
                    PLATFORM_PMU->ext_wake_mask1.bits.gci = 0;
                    break;

                case PLATFORM_MCU_POWERSAVE_MISC_WAKEUP_CONFIG_GCI_GPIO_WAKE:
                    GCI_WAKEMASK_REG->bits.gci_gpio_wake = 0;
                    break;

                default:
                    wiced_assert( "not supported config", 0 );
                    break;
            }
        }
    }

    WICED_RESTORE_INTERRUPTS( flags );
}
#endif /* PLATFORM_APPS_POWERSAVE */

platform_result_t
platform_mcu_powersave_gpio_wakeup_enable( platform_mcu_powersave_gpio_wakeup_config_t config, platform_mcu_powersave_gpio_wakeup_trigger_t trigger )
{
#if PLATFORM_APPS_POWERSAVE
    /*
     * Acking may be required after the wakeup enabled.
     * This is due to enabling also programs pull up/down, and this may trigger interrupt.
     * Triggering is driven by ILP clock so some small delay may be seen before interrupt triggered.
     */

    const platform_gpio_t gpio = { POWERSAVE_MCU_GPIO_WAKE_UP_PIN };
    platform_result_t     result;

    result = platform_gpio_init( &gpio, platform_mcu_powersave_gpio_wakeup_config_to_pin_config( config ) );
    if ( result != PLATFORM_SUCCESS )
    {
        return result;
    }

    platform_mcu_powersave_misc_wakeup_config_enable( PLATFORM_MCU_POWERSAVE_MISC_WAKEUP_CONFIG_PMU_WL_DEV_WAKE, WICED_TRUE );

    platform_mcu_powersave_misc_wakeup_config_enable( PLATFORM_MCU_POWERSAVE_MISC_WAKEUP_CONFIG_PMU_GCI_WAKE, WICED_TRUE );

    switch ( trigger )
    {
        case PLATFORM_MCU_POWERSAVE_GPIO_WAKEUP_TRIGGER_RISING_EDGE:
            platform_pmu_chipcontrol( PMU_CHIPCONTROL_INVERT_GPIO0_POLARITY_REG,
                                      PMU_CHIPCONTROL_INVERT_GPIO0_POLARITY_MASK,
                                      0x0 );
            break;

        case PLATFORM_MCU_POWERSAVE_GPIO_WAKEUP_TRIGGER_FALLING_EDGE:
            platform_pmu_chipcontrol( PMU_CHIPCONTROL_INVERT_GPIO0_POLARITY_REG,
                                      PMU_CHIPCONTROL_INVERT_GPIO0_POLARITY_MASK,
                                      PMU_CHIPCONTROL_INVERT_GPIO0_POLARITY_EN );
            break;

        default:
            wiced_assert( "unhandled trigger type", 0 );
            break;
    }

    return PLATFORM_SUCCESS;
#else
    return PLATFORM_FEATURE_DISABLED;
#endif /* PLATFORM_APPS_POWERSAVE */
}

void
platform_mcu_powersave_gpio_wakeup_ack( void )
{
#if PLATFORM_APPS_POWERSAVE
    platform_tick_sleep_clear_ext_wake( );
    PLATFORM_PMU->ext_wakeup_status.bits.gci = 1;
#endif
}

void
platform_mcu_powersave_gpio_wakeup_disable( void )
{
#if PLATFORM_APPS_POWERSAVE
    const platform_gpio_t gpio = { POWERSAVE_MCU_GPIO_WAKE_UP_PIN };

    platform_mcu_powersave_misc_wakeup_config_enable( PLATFORM_MCU_POWERSAVE_MISC_WAKEUP_CONFIG_PMU_GCI_WAKE, WICED_FALSE );

    platform_mcu_powersave_misc_wakeup_config_enable( PLATFORM_MCU_POWERSAVE_MISC_WAKEUP_CONFIG_PMU_WL_DEV_WAKE, WICED_FALSE );

    (void)platform_gpio_deinit( &gpio );
#endif /* PLATFORM_APPS_POWERSAVE */
}

platform_result_t platform_gpio_deepsleep_wakeup_enable( const platform_gpio_t* gpio, platform_gpio_irq_trigger_t trigger )
{
#if PLATFORM_APPS_POWERSAVE
    platform_result_t result;
    platform_gci_gpio_irq_trigger_t gci_gpio_irq_trigger;

    gci_gpio_irq_trigger = platform_mcu_powersave_gpio_irq_trigger_to_gci_gpio_irq_trigger( trigger );
    result = platform_mcu_powersave_gci_gpio_wakeup_enable( gpio->pin, PLATFORM_MCU_POWERSAVE_GPIO_WAKEUP_CONFIG_HIGH_IMPEDANCE, gci_gpio_irq_trigger );

    if ( result != PLATFORM_SUCCESS )
    {
        return PLATFORM_ERROR;
    }

    host_rtos_delay_milliseconds(100);

    /* Ack IRQ that might have triggered in configuring GPIO to wake up from Deep sleep */
    platform_mcu_powersave_gpio_wakeup_ack();

    return result;
#else
    return PLATFORM_UNSUPPORTED;
#endif
}


platform_result_t
platform_mcu_powersave_gci_gpio_wakeup_enable( platform_pin_t gpio_pin, platform_mcu_powersave_gpio_wakeup_config_t config, platform_gci_gpio_irq_trigger_t trigger )
{
#if PLATFORM_APPS_POWERSAVE
    /*
     * Acking may be required after the wakeup enabled.
     * This is due to enabling also programs pull up/down, and this may trigger interrupt.
     * Triggering is driven by ILP clock so some small delay may be seen before interrupt triggered.
     */

    platform_result_t     result;
    const unsigned int    gpio_number     = platform_mcu_powersave_gpio_number( gpio_pin );
    const platform_gpio_t gpio            = { gpio_pin };
    uint32_t              gpio_wake_mask  = 0x0;

    if ( platform_mcu_powersave_gci_gpio_wakeup_is_valid_pin( gpio_pin ) != WICED_TRUE )
    {
        return PLATFORM_BADARG;
    }

    result = platform_gpio_init( &gpio, platform_mcu_powersave_gpio_wakeup_config_to_pin_config( config ) );
    if ( result != PLATFORM_SUCCESS )
    {
        return result;
    }

    if ( trigger & PLATFORM_GCI_GPIO_IRQ_TRIGGER_FAST_EDGE )
    {
        gpio_wake_mask |= GCI_CHIPCONTROL_GPIO_WAKEMASK_FAST_EDGE( gpio_number );
    }
    if ( trigger & PLATFORM_GCI_GPIO_IRQ_TRIGGER_RISING_EDGE )
    {
        gpio_wake_mask |= GCI_CHIPCONTROL_GPIO_WAKEMASK_POS_EDGE( gpio_number );
    }
    if ( trigger & PLATFORM_GCI_GPIO_IRQ_TRIGGER_FALLING_EDGE )
    {
        gpio_wake_mask |= GCI_CHIPCONTROL_GPIO_WAKEMASK_NEG_EDGE( gpio_number );
    }
    if ( trigger & PLATFORM_GCI_GPIO_IRQ_TRIGGER_LEVEL_HIGH )
    {
        gpio_wake_mask |= GCI_CHIPCONTROL_GPIO_WAKEMASK_IN( gpio_number );
    }
    platform_gci_gpiowakemask( GCI_CHIPCONTROL_GPIO_WAKEMASK_REG( gpio_number ),
                               GCI_CHIPCONTROL_GPIO_WAKEMASK_MASK( gpio_number ),
                               gpio_wake_mask );

    platform_gci_gpiocontrol( GCI_CHIPCONTROL_GPIO_CONTROL_REG( gpio_number ),
                              GCI_CHIPCONTROL_GPIO_CONTROL_EXTRA_GPIO_ENABLE_MASK( gpio_number ),
                              GCI_CHIPCONTROL_GPIO_CONTROL_EXTRA_GPIO_ENABLE_SET( gpio_number ) );

    platform_mcu_powersave_misc_wakeup_config_enable( PLATFORM_MCU_POWERSAVE_MISC_WAKEUP_CONFIG_GCI_GPIO_WAKE, WICED_TRUE );

    platform_mcu_powersave_misc_wakeup_config_enable( PLATFORM_MCU_POWERSAVE_MISC_WAKEUP_CONFIG_PMU_GCI2WL_WAKE, WICED_TRUE );

    platform_mcu_powersave_misc_wakeup_config_enable( PLATFORM_MCU_POWERSAVE_MISC_WAKEUP_CONFIG_PMU_GCI_WAKE, WICED_TRUE );

    return PLATFORM_SUCCESS;
#else
    return PLATFORM_FEATURE_DISABLED;
#endif /* PLATFORM_APPS_POWERSAVE */
}

void
platform_mcu_powersave_gci_gpio_wakeup_disable( platform_pin_t gpio_pin )
{
#if PLATFORM_APPS_POWERSAVE
    const unsigned int    gpio_number = platform_mcu_powersave_gpio_number( gpio_pin );
    const platform_gpio_t gpio        = { gpio_pin };

    wiced_assert( "wrong pin", platform_mcu_powersave_gci_gpio_wakeup_is_valid_pin( gpio_pin ) );

    platform_mcu_powersave_misc_wakeup_config_enable( PLATFORM_MCU_POWERSAVE_MISC_WAKEUP_CONFIG_PMU_GCI_WAKE, WICED_FALSE );

    platform_mcu_powersave_misc_wakeup_config_enable( PLATFORM_MCU_POWERSAVE_MISC_WAKEUP_CONFIG_PMU_GCI2WL_WAKE, WICED_FALSE );

    platform_mcu_powersave_misc_wakeup_config_enable( PLATFORM_MCU_POWERSAVE_MISC_WAKEUP_CONFIG_GCI_GPIO_WAKE, WICED_FALSE );

    platform_gci_gpiocontrol( GCI_CHIPCONTROL_GPIO_CONTROL_REG( gpio_number ),
                              GCI_CHIPCONTROL_GPIO_CONTROL_EXTRA_GPIO_ENABLE_MASK( gpio_number ),
                              0x0 );

    platform_gci_gpiowakemask( GCI_CHIPCONTROL_GPIO_WAKEMASK_REG( gpio_number ),
                               GCI_CHIPCONTROL_GPIO_WAKEMASK_MASK( gpio_number ),
                               0x0 );

    (void)platform_gpio_deinit( &gpio );
#endif /* PLATFORM_APPS_POWERSAVE */
}

wiced_bool_t
platform_mcu_powersave_gci_gpio_wakeup_ack( platform_pin_t gpio_pin )
{
#if PLATFORM_APPS_POWERSAVE
    const unsigned int gpio_number      = platform_mcu_powersave_gpio_number( gpio_pin );
    const uint32_t     edge_wakeup_mask = platform_gci_gpiowakemask( GCI_CHIPCONTROL_GPIO_WAKEMASK_REG( gpio_number ), 0x0, 0x0 ) & GCI_CHIPCONTROL_GPIO_WAKEMASK_ANY_EDGE( gpio_number );

    platform_tick_sleep_clear_ext_wake( );

    /* Need to disable before clearing otherwise falling edge status is not cleared if high voltage applied to pin. */
    platform_gci_gpiocontrol( GCI_CHIPCONTROL_GPIO_CONTROL_REG( gpio_number ),
                              GCI_CHIPCONTROL_GPIO_CONTROL_EXTRA_GPIO_ENABLE_MASK( gpio_number ),
                              0x0 );

    /* Clear status */
    platform_gci_gpiostatus( GCI_CHIPCONTROL_GPIO_WAKEMASK_REG( gpio_number ),
                             GCI_CHIPCONTROL_GPIO_WAKEMASK_MASK( gpio_number ),
                             GCI_CHIPCONTROL_GPIO_WAKEMASK_MASK( gpio_number ) );

    /* Enable back */
    platform_gci_gpiocontrol( GCI_CHIPCONTROL_GPIO_CONTROL_REG( gpio_number ),
                              GCI_CHIPCONTROL_GPIO_CONTROL_EXTRA_GPIO_ENABLE_MASK( gpio_number ),
                              GCI_CHIPCONTROL_GPIO_CONTROL_EXTRA_GPIO_ENABLE_SET( gpio_number ) );

    /* Enabling back can cause raising edge triggered, clear it */
    platform_gci_gpiostatus( GCI_CHIPCONTROL_GPIO_WAKEMASK_REG( gpio_number ),
                             GCI_CHIPCONTROL_GPIO_WAKEMASK_MASK( gpio_number ),
                             GCI_CHIPCONTROL_GPIO_WAKEMASK_MASK( gpio_number ) );

    PLATFORM_PMU->ext_wakeup_status.bits.gci = 1;

    if ( ( platform_gci_gpiostatus( GCI_CHIPCONTROL_GPIO_WAKEMASK_REG( gpio_number ), 0x0, 0x0 ) & edge_wakeup_mask ) != 0 )
    {
        /* Failed to acknowledge or re-triggered */
        return WICED_FALSE;
    }
#endif /* PLATFORM_APPS_POWERSAVE */

    return WICED_TRUE;
}

platform_result_t
platform_mcu_powersave_bluetooth_sleep_handler( const platform_gpio_t* bt_pin_device_wake )
{
    /* DEV_WAKE needs to be asserted to put the BT chip to LP mode */
    platform_gpio_output_high( bt_pin_device_wake );
    return PLATFORM_SUCCESS;
}

platform_result_t
platform_mcu_powersave_bluetooth_wakeup_handler( const platform_gpio_t* bt_pin_device_wake, const platform_gpio_t* bt_pin_host_wake )
{
    if ( bt_pin_host_wake != NULL )
    {
        /* Acknowledge WAKEUP , so IRQ is not triggered again */
        platform_mcu_powersave_gci_gpio_wakeup_ack( bt_pin_host_wake->pin );

        /* Disable WAKEUP from BT_HOST_WAKE */
        platform_mcu_powersave_gci_gpio_wakeup_disable( bt_pin_host_wake->pin );
    }

    if ( ( platform_mcu_powersave_get_mode( ) == PLATFORM_MCU_POWERSAVE_MODE_SLEEP ) )
    {
        platform_gpio_output_low( bt_pin_device_wake );
    }

    return PLATFORM_SUCCESS;
}

#if PLATFORM_WLAN_POWERSAVE

static wiced_bool_t
platform_wlan_powersave_pmu_timer_slow_write_pending( void )
{
    return ( PLATFORM_PMU->pmustatus & PST_SLOW_WR_PENDING ) ? WICED_TRUE : WICED_FALSE;
}

static void
platform_wlan_powersave_pmu_timer_slow_write( uint32_t val )
{
    PLATFORM_TIMEOUT_BEGIN( start_stamp );

    while ( platform_wlan_powersave_pmu_timer_slow_write_pending() )
    {
        PLATFORM_TIMEOUT_SEC_ASSERT( "wait status before write for too long", start_stamp,
            !platform_wlan_powersave_pmu_timer_slow_write_pending(), POWERSAVE_WLAN_TIMER_ASSERT_SEC );
    }

    PLATFORM_PMU->res_req_timer1.raw = val;

    while ( platform_wlan_powersave_pmu_timer_slow_write_pending() )
    {
        PLATFORM_TIMEOUT_SEC_ASSERT( "wait status after write for too long", start_stamp,
            !platform_wlan_powersave_pmu_timer_slow_write_pending(), POWERSAVE_WLAN_TIMER_ASSERT_SEC );
    }
}

static void
platform_wlan_powersave_res_ack_event( void )
{
    pmu_intstatus_t status;

    status.raw                  = 0;
    status.bits.rsrc_event_int1 = 1;

    PLATFORM_PMU->pmuintstatus.raw = status.raw;

    powersave_wlan_res_event_ack_stamp = platform_tick_get_time( PLATFORM_TICK_GET_SLOW_TIME_STAMP );
}

static wiced_bool_t
platform_wlan_powersave_res_is_event_unmask_permitted( void )
{
    /*
     * ISR acks interrupt, but it take few ILP cycles to do it.
     * As result if resource mask is changing quickly we may ack and lost some
     * of the subsequent interrupts and hang by waiting on semaphore.
     * To avoid this let's do not try to enable interrupts till ack is settled.
     */
    uint32_t now = platform_tick_get_time( PLATFORM_TICK_GET_SLOW_TIME_STAMP );
    if ( powersave_wlan_res_event_ack_stamp == 0 )
    {
        powersave_wlan_res_event_ack_stamp = now;
        return WICED_FALSE;
    }
    else if ( now - powersave_wlan_res_event_ack_stamp < (uint32_t)PMU_MAX_WRITE_LATENCY_ILP_TICKS )
    {
        return WICED_FALSE;
    }

    return WICED_TRUE;
}

static wiced_bool_t
platform_wlan_powersave_res_mask_event( wiced_bool_t enable )
{
    pmu_intstatus_t mask;

    if ( enable && !platform_wlan_powersave_res_is_event_unmask_permitted() )
    {
        return WICED_FALSE;
    }

    mask.raw                  = 0;
    mask.bits.rsrc_event_int1 = 1;

    if ( enable )
    {
        platform_common_chipcontrol( &PLATFORM_PMU->pmuintmask1.raw, 0x0, mask.raw );
    }
    else
    {
        platform_common_chipcontrol( &PLATFORM_PMU->pmuintmask1.raw, mask.raw, 0x0 );
    }

    return WICED_TRUE;
}

/*
 * Function return synchronized state of 2 registers.
 * As resources can go down let's remove pending one from current resource mask.
 */
static uint32_t
platform_wlan_powersave_res_get_current_resources( uint32 mask )
{
    while ( WICED_TRUE )
    {
        uint32_t res_state   = PLATFORM_PMU->res_state & mask;
        uint32_t res_pending = PLATFORM_PMU->res_pending & mask;

        if ( res_state != ( PLATFORM_PMU->res_state & mask ) )
        {
            continue;
        }

        if ( res_pending != ( PLATFORM_PMU->res_pending & mask ) )
        {
            continue;
        }

        return ( res_state & ~res_pending );
    }
}

static void
platform_wlan_powersave_res_wait_event( void )
{
    PLATFORM_TIMEOUT_BEGIN( start_stamp );

    while ( WICED_TRUE )
    {
        uint32_t mask = platform_wlan_powersave_res_get_current_resources( PMU_RES_WLAN_UP_MASK );

        if ( mask == PMU_RES_WLAN_UP_MASK )
        {
            /* Done */
            break;
        }

        PLATFORM_TIMEOUT_SEC_ASSERT( "wait res for too long", start_stamp,
            platform_wlan_powersave_res_get_current_resources( PMU_RES_WLAN_UP_MASK ) == PMU_RES_WLAN_UP_MASK, POWERSAVE_WLAN_EVENT_ASSERT_SEC );

        if ( ( mask | PMU_RES_WLAN_UP_EVENT_MASK ) == PMU_RES_WLAN_UP_MASK )
        {
            /* Nearly here, switch to polling */
            continue;
        }

        if ( platform_wlan_powersave_res_mask_event( WICED_TRUE ) )
        {
            if ( host_rtos_get_semaphore( &powersave_wlan_res_event, POWERSAVE_WLAN_EVENT_WAIT_MS, WICED_TRUE ) != WWD_SUCCESS )
            {
                wiced_assert( "powersave event timed out", 0 );
            }
        }
    }
}

void
platform_wlan_powersave_res_event( void )
{
    platform_wlan_powersave_res_ack_event();

    platform_wlan_powersave_res_mask_event( WICED_FALSE );

    host_rtos_set_semaphore( &powersave_wlan_res_event, WICED_TRUE );
}

wiced_bool_t
platform_wlan_powersave_res_up( void )
{
    wiced_bool_t res_up = WICED_FALSE;

    host_rtos_get_semaphore( &powersave_wlan_res_lock, NEVER_TIMEOUT, WICED_FALSE );

    powersave_wlan_res_ref_counter++;

    wiced_assert( "wrapped around zero", powersave_wlan_res_ref_counter > 0 );

    if ( powersave_wlan_res_ref_counter == 1 )
    {
        res_up = WICED_TRUE;
    }

    if ( res_up )
    {
        pmu_res_req_timer_t timer;

        timer.raw                   = 0;
        timer.bits.req_active       = 1;
        timer.bits.force_ht_request = 1;
        timer.bits.clkreq_group_sel = pmu_res_clkreq_apps_group;

#if PLATFORM_WLAN_POWERSAVE_STATS
        uint32_t wait_up_begin_stamp      = platform_tick_get_time( PLATFORM_TICK_GET_SLOW_TIME_STAMP );
        powersave_wlan_res_up_begin_stamp = wait_up_begin_stamp;
        powersave_wlan_res_call_counter++;
#endif /* PLATFORM_WLAN_POWERSAVE_STATS */

        platform_tick_execute_command( PLATFORM_TICK_COMMAND_RELEASE_PMU_TIMER_BEGIN );

        PLATFORM_PMU->res_req_mask1 = PMU_RES_WLAN_UP_MASK;

        platform_wlan_powersave_pmu_timer_slow_write( timer.raw );

        platform_wlan_powersave_res_wait_event();

#if PLATFORM_WLAN_POWERSAVE_STATS
        powersave_wlan_res_wait_up_time += platform_tick_get_time( PLATFORM_TICK_GET_SLOW_TIME_STAMP ) - wait_up_begin_stamp;
#endif
    }

    host_rtos_set_semaphore( &powersave_wlan_res_lock, WICED_FALSE );

    return res_up;
}

wiced_bool_t
platform_wlan_powersave_res_down( wiced_bool_t(*check_ready)(void), wiced_bool_t force )
{
    wiced_bool_t res_down = WICED_FALSE;

    host_rtos_get_semaphore( &powersave_wlan_res_lock, NEVER_TIMEOUT, WICED_FALSE );

    if ( !force )
    {
        wiced_assert( "unbalanced call", powersave_wlan_res_ref_counter > 0 );
        powersave_wlan_res_ref_counter--;
        if ( powersave_wlan_res_ref_counter == 0 )
        {
            res_down = WICED_TRUE;
        }
    }
    else if ( powersave_wlan_res_ref_counter != 0 )
    {
        wiced_assert( "unbalanced call", 0 ); /* in case of 'force' forgive unbalanced call, but in release build only */
        powersave_wlan_res_ref_counter = 0;
        res_down                       = WICED_TRUE;
    }

    if ( res_down )
    {
        if ( check_ready )
        {
            while ( !check_ready() );
        }

        platform_wlan_powersave_pmu_timer_slow_write( 0x0 );

        PLATFORM_PMU->res_req_mask1 = PMU_RES_APPS_UP_MASK;

        platform_tick_execute_command( PLATFORM_TICK_COMMAND_RELEASE_PMU_TIMER_END );

#if PLATFORM_WLAN_POWERSAVE_STATS
        powersave_wlan_res_up_time += platform_tick_get_time( PLATFORM_TICK_GET_SLOW_TIME_STAMP ) - powersave_wlan_res_up_begin_stamp;
#endif
    }

    host_rtos_set_semaphore( &powersave_wlan_res_lock, WICED_FALSE );

    return res_down;
}

wiced_bool_t platform_wlan_powersave_is_res_up( void )
{
    return ( powersave_wlan_res_ref_counter != 0 ) ? WICED_TRUE : WICED_FALSE;
}

uint32_t
platform_wlan_powersave_get_stats( platform_wlan_powersave_stats_t which_counter )
{
#if PLATFORM_WLAN_POWERSAVE_STATS
    switch ( which_counter )
    {
       case PLATFORM_WLAN_POWERSAVE_STATS_CALL_NUM:
           return powersave_wlan_res_call_counter;

       case PLATFORM_WLAN_POWERSAVE_STATS_UP_TIME:
           return powersave_wlan_res_up_time;

       case PLATFORM_WLAN_POWERSAVE_STATS_WAIT_UP_TIME:
           return powersave_wlan_res_wait_up_time;

       default:
           wiced_assert( "unhandled case", 0 );
           return 0;
    }
#else
    UNUSED_PARAMETER( which_counter );
    return 0;
#endif /* PLATFORM_WLAN_POWERSAVE_STATS */
}

#endif /* PLATFORM_WLAN_POWERSAVE */
