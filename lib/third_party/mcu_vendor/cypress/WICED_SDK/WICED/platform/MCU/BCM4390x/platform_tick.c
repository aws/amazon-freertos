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
 * BCM43909 CPU initialization and RTOS ticks
 */

#include "platform_isr.h"
#include "platform_isr_interface.h"
#include "platform_appscr4.h"
#include "platform_config.h"
#include "platform_mcu_peripheral.h"

#include "cr4.h"

#include "wwd_assert.h"
#include "wwd_rtos.h"

#include "wiced_low_power.h"

#include "typedefs.h"
#include "wiced_osl.h"
#include "sbchipc.h"

/******************************************************
 *                      Macros
 ******************************************************/

#ifndef PLATFORM_CLOCK_HZ_EMUL_SLOW_COEFF
#define PLATFORM_CLOCK_HZ_EMUL_SLOW_COEFF       1
#endif

#define PLATFORM_CLOCK_HZ_DEFINE(clock_hz)      ( (clock_hz) / PLATFORM_CLOCK_HZ_EMUL_SLOW_COEFF )
#define PLATFORM_CLOCK_HZ_24MHZ                 PLATFORM_CLOCK_HZ_DEFINE( 24000000 )
#define PLATFORM_CLOCK_HZ_48MHZ                 PLATFORM_CLOCK_HZ_DEFINE( 48000000 )
#define PLATFORM_CLOCK_HZ_60MHZ                 PLATFORM_CLOCK_HZ_DEFINE( 60000000 )
#define PLATFORM_CLOCK_HZ_80MHZ                 PLATFORM_CLOCK_HZ_DEFINE( 80000000 )
#define PLATFORM_CLOCK_HZ_120MHZ                PLATFORM_CLOCK_HZ_DEFINE( 120000000 )
#define PLATFORM_CLOCK_HZ_160MHZ                PLATFORM_CLOCK_HZ_DEFINE( 160000000 )
#define PLATFORM_CLOCK_HZ_320MHZ                PLATFORM_CLOCK_HZ_DEFINE( 320000000 )
#if defined(PLATFORM_4390X_OVERCLOCK)
#define PLATFORM_CLOCK_HZ_480MHZ                PLATFORM_CLOCK_HZ_DEFINE( 480000000 )
#endif /* PLATFORM_4390X_OVERCLOCK */

#define CPU_CLOCK_HZ_DEFAULT                    PLATFORM_CLOCK_HZ_160MHZ

#define CYCLES_PER_TICK(clock_hz)               ( (clock_hz) / (SYSTICK_FREQUENCY) )
#define CYCLES_PMU_PER_TICK                     CYCLES_PER_TICK( platform_reference_clock_get_freq( PLATFORM_REFERENCE_CLOCK_ILP ) )
#define CYCLES_CPU_PER_TICK                     CYCLES_PER_TICK( platform_reference_clock_get_freq( PLATFORM_REFERENCE_CLOCK_CPU ) )
#define CYCLES_TINY_PER_TICK                    CYCLES_PER_TICK( CPU_CLOCK_HZ )
#define CYCLES_PMU_INIT_PER_TICK                CYCLES_PER_TICK( ILP_CLOCK )
#define CYCLES_CPU_INIT_PER_TICK                CYCLES_PER_TICK( CPU_CLOCK_HZ_DEFAULT )

#define PLATFORM_CPU_CLOCK_SLEEPING             PLATFORM_CPU_CLOCK_SLEEPING_NO_REQUEST
#define PLATFORM_CPU_CLOCK_NOT_SLEEPING         PLATFORM_CPU_CLOCK_NOT_SLEEPING_HT_REQUEST

#define TICK_CPU_MAX_COUNTER                    0xFFFFFFFF
#define TICK_PMU_MAX_COUNTER                    0x00FFFFFF

#define TICKS_MAX(max_counter, cycles_per_tick) MIN( (max_counter) / (cycles_per_tick) - 1, 0xFFFFFFFF / (cycles_per_tick) - 2 )

#define TICK_PMU_TIMER_ASSERT_SEC               1

#ifndef PLATFORM_TICK_CPU_TICKLESS_THRESH
#define PLATFORM_TICK_CPU_TICKLESS_THRESH       1
#endif

#ifndef PLATFORM_TICK_PMU_TICKLESS_THRESH
#define PLATFORM_TICK_PMU_TICKLESS_THRESH       5
#endif

#ifndef PLATFORM_TICK_CPU_DEEP_SLEEP_THRESH
#define PLATFORM_TICK_CPU_DEEP_SLEEP_THRESH     1
#endif

#ifndef PLATFORM_TICK_PMU_DEEP_SLEEP_THRESH
#define PLATFORM_TICK_PMU_DEEP_SLEEP_THRESH     100
#endif

#define PLATFORM_CPU_CLOCK_FREQ_CONFIG_DEFINE( cpu_freq, backplane_freq, source, source_only, pll_ch1, pll_ch2 ) \
    { \
        config->apps_cpu_freq_reg_value = (cpu_freq);         \
        config->apps_bp_freq_reg_value  = (backplane_freq);   \
        config->clock_source            = (source);           \
        config->clock_source_only       = (source_only);      \
        channels[0]                     = (uint8_t)(pll_ch1); \
        channels[1]                     = (uint8_t)(pll_ch2); \
    }

#if defined(PLATFORM_4390X_OVERCLOCK)
#define PLATFORM_CPU_CLOCK_FREQ_PLL_CONFIG_DEFINE( channel, divider ) \
    { \
        config->pll_channel = (channel); \
        config->pll_divider = (divider); \
    }
#endif /* PLATFORM_4390X_OVERCLOCK */

#if ( PLATFORM_BACKPLANE_ON_CPU_CLOCK_ENABLE != 0 )
#define PLATFORM_CLOCK_BP_CLK_FROM_ARMCR4_REG_VALUE    GCI_CHIPCONTROL_BP_CLK_FROM_ARMCR4_CLK_SET
#else
#define PLATFORM_CLOCK_BP_CLK_FROM_ARMCR4_REG_VALUE    0x0
#endif /* PLATFORM_BACKPLANE_ON_CPU_CLOCK_ENABLE != 0 */

/* Verify CPU overclocking configuration. */
#if defined(PLATFORM_4390X_OVERCLOCK) && (PLATFORM_BACKPLANE_ON_CPU_CLOCK_ENABLE == 0)
#error "Overclocking unimplemented when local div-2 disabled."
#endif

/******************************************************
 *                    Constants
 ******************************************************/

/******************************************************
 *                   Enumerations
 ******************************************************/

typedef enum
{
    PLATFORM_CPU_CLOCK_NOT_SLEEPING_ALP_REQUEST               = 0x0,
    PLATFORM_CPU_CLOCK_NOT_SLEEPING_HT_REQUEST                = 0x1,
    PLATFORM_CPU_CLOCK_NOT_SLEEPING_ALP_REQUEST_HT_AVAILABLE  = 0x2,
    PLATFORM_CPU_CLOCK_NOT_SLEEPING_HT_AVAILABLE              = 0x3,
} platform_cpu_clock_not_sleeping_t;

typedef enum
{
    PLATFORM_CPU_CLOCK_SLEEPING_NO_REQUEST   = 0x0,
    PLATFORM_CPU_CLOCK_SLEEPING_ILP_REQUEST  = 0x1,
    PLATFORM_CPU_CLOCK_SLEEPING_ALP_REQUEST  = 0x2,
    PLATFORM_CPU_CLOCK_SLEEPING_HT_AVAILABLE = 0x3,
} platform_cpu_clock_sleeping_t;

typedef enum
{
    PLATFORM_CPU_CLOCK_SOURCE_BACKPLANE = 0x0, /* CPU runs on backplane clock */
    PLATFORM_CPU_CLOCK_SOURCE_ARM       = 0x4  /* CPU runs on own ARM clock */
} platform_cpu_clock_source_t;

typedef enum
{
    PLATFORM_TICK_RESTART_ACTION_BEGIN_TICKLESS_MODE,
    PLATFORM_TICK_RESTART_ACTION_OPPORTUNISTIC_BEGIN_TICKLESS_MODE,
    PLATFORM_TICK_RESTART_ACTION_END_TICKLESS_MODE,
    PLATFORM_TICK_RESTART_ACTION_BEGIN_NORMAL_MODE
} platform_tick_restart_action_t;

/******************************************************
 *                 Type Definitions
 ******************************************************/

/******************************************************
 *                    Structures
 ******************************************************/

typedef struct platform_tick_timer
{
    void         ( *init_func                )( struct platform_tick_timer* timer );
    uint32_t     ( *freerunning_current_func )( struct platform_tick_timer* timer );
    void         ( *start_func               )( struct platform_tick_timer* timer, uint32_t cycles_till_fire );
    void         ( *reset_func               )( struct platform_tick_timer* timer, uint32_t cycles_till_fire ); /* expect to be called from Timer ISR only when Timer is fired and not ticking anymore */
    void         ( *restart_func             )( struct platform_tick_timer* timer, platform_tick_restart_action_t action, uint32_t cycles_till_fire );
    void         ( *stop_func                )( struct platform_tick_timer* timer );
    void         ( *sleep_invoke_func        )( struct platform_tick_timer* timer, platform_tick_sleep_idle_func idle_func );

    uint32_t     deep_sleep_threshold;
    uint32_t     tickless_threshold;
    uint32_t     cycles_per_tick;
    uint32_t     ticks_max;
    uint32_t     freerunning_start_stamp;
    uint32_t     freerunning_stop_stamp;
    uint32_t     cycles_till_fire;
    wiced_bool_t started;
    uint32_t     cycles_per_sec;
    uint32_t     tick_count;
} platform_tick_timer_t;

typedef struct platform_cpu_clock_freq_config
{
    uint32_t                    apps_cpu_freq_reg_value;
    uint32_t                    apps_bp_freq_reg_value;
    platform_cpu_clock_source_t clock_source;
    wiced_bool_t                clock_source_only;
#if defined(PLATFORM_4390X_OVERCLOCK)
    uint8_t                     pll_channel;
    uint8_t                     pll_divider;
#endif /* PLATFORM_4390X_OVERCLOCK */
} platform_cpu_clock_freq_config_t;

/******************************************************
 *               Function Declarations
 ******************************************************/

#if !PLATFORM_TICK_TINY
static void platform_tick_init( void );
#endif /* !PLATFORM_TICK_TINY */

/******************************************************
 *               Variables Definitions
 ******************************************************/

#if !PLATFORM_TICK_TINY
static platform_tick_timer_t* platform_tick_current_timer  = NULL;
#endif /* !PLATFORM_TICK_TINY */

uint32_t                      platform_cpu_clock_hz        = CPU_CLOCK_HZ_DEFAULT;

static uint32_t               platform_pmu_cycles_per_tick = CYCLES_PMU_INIT_PER_TICK;
static uint32_t               WICED_DEEP_SLEEP_SAVED_VAR( platform_pmu_last_sleep_stamp );
static uint32_t               platform_pmu_last_deep_sleep_stamp;

static wiced_bool_t           WICED_DEEP_SLEEP_SAVED_VAR( platform_lpo_clock_inited );

#if PLATFORM_APPS_POWERSAVE
static pmu_ext_wakeup_t       platform_pmu_stored_ext_wakeup;
#endif

/******************************************************
 *               Clock Function Definitions
 ******************************************************/

static void
platform_cpu_wait_ht( void )
{
    uint32_t timeout;
    for ( timeout = 100000000; timeout != 0; timeout-- )
    {
        if ( PLATFORM_CLOCKSTATUS_REG(PLATFORM_APPSCR4_REGBASE(0x0))->bits.bp_on_ht != 0 )
        {
            break;
        }
    }
    wiced_assert( "timed out", timeout != 0 );
}

void
platform_cpu_core_init( void )
{
    appscr4_core_ctrl_reg_t core_ctrl;

    /* Initialize core control register. */

    core_ctrl.raw = PLATFORM_APPSCR4->core_ctrl.raw;

    core_ctrl.bits.force_clock_source     = 0;
    core_ctrl.bits.wfi_clk_stop           = 1;
    core_ctrl.bits.s_error_int_en         = 1;
    core_ctrl.bits.pclk_dbg_stop          = 0;
    core_ctrl.bits.sleeping_clk_req       = PLATFORM_CPU_CLOCK_SLEEPING;
    core_ctrl.bits.not_sleeping_clk_req_0 = PLATFORM_CPU_CLOCK_NOT_SLEEPING & 0x1;
    core_ctrl.bits.not_sleeping_clk_req_1 = (PLATFORM_CPU_CLOCK_NOT_SLEEPING >> 1) & 0x1;

    PLATFORM_APPSCR4->core_ctrl.raw = core_ctrl.raw;

    /* Core is configured to request HT clock. Let's wait backplane actually run on HT. */
    platform_cpu_wait_ht();
}

static platform_cpu_clock_source_t
platform_cpu_clock_get_source( void )
{
    return PLATFORM_APPSCR4->core_ctrl.bits.clock_source;
}

static void
platform_cpu_clock_source( platform_cpu_clock_source_t clock_source )
{
    /*
     * Source change actually happen when WFI instruction is executed and _only_ if WFI
     * actually wait for interrupts. I.e. if interrupt was pending before WFI, then no
     * switch happen; interrupt must fire after WFI executed.
     *
     * When switching into 2:1 mode (PLATFORM_CPU_CLOCK_SOURCE_ARM) SW must ensure that the HT clock is
     * available and backplane runs on HT.
     * All clock values are valid provided there is always a pair of clocks; one at 2x and one at 1x.
     * Apps CPU needs to downshift to 1:1 (PLATFORM_CPU_CLOCK_SOURCE_BACKPLANE) before going to ALP.
     */

    const uint32_t          clockstable_mask = IRQN2MASK( ClockStable_ExtIRQn );
    uint32_t                irq_mask;
    uint32_t                int_mask;
    appscr4_core_ctrl_reg_t core_ctrl, core_ctrl_restore;

    /* Return if already use requested source */
    if ( platform_cpu_clock_get_source() == clock_source )
    {
        return;
    }

    /* Save masks and disable all interrupts */
    irq_mask = PLATFORM_APPSCR4->irq_mask;
    int_mask = PLATFORM_APPSCR4->int_mask;
    PLATFORM_APPSCR4->irq_mask = 0;
    PLATFORM_APPSCR4->int_mask = 0;

    /* Tell hw which source want */
    core_ctrl.raw = PLATFORM_APPSCR4->core_ctrl.raw;
    core_ctrl.bits.clock_source = clock_source;
    core_ctrl_restore.raw = core_ctrl.raw;
    core_ctrl.bits.wfi_clk_stop = 1;
    PLATFORM_APPSCR4->core_ctrl.raw = core_ctrl.raw;

    /*
     * Ack, unmask ClockStable interrupt and call WFI.
     * We should not have pending interrupts at the moment, so calling should
     * trigger clock switch. At the end of switch CPU will be woken up
     * by ClockStable interrupt.
     */
    PLATFORM_APPSCR4->fiqirq_status = clockstable_mask;
    PLATFORM_APPSCR4->irq_mask = clockstable_mask;
    cpu_wait_for_interrupt();
    wiced_assert("no clockstable irq", PLATFORM_APPSCR4->fiqirq_status & clockstable_mask );
    PLATFORM_APPSCR4->fiqirq_status = clockstable_mask;

    /* Restore interrupt masks */
    PLATFORM_APPSCR4->core_ctrl.raw = core_ctrl_restore.raw;
    PLATFORM_APPSCR4->int_mask      = int_mask;
    PLATFORM_APPSCR4->irq_mask      = irq_mask;
}

static wiced_bool_t
platform_cpu_clock_drives_backplane_clock( void )
{
    const uint32_t val = platform_gci_chipcontrol( GCI_CHIPCONTROL_BP_CLK_FROM_ARMCR4_CLK_REG, 0x0, 0x0);
    return ( ( val & GCI_CHIPCONTROL_BP_CLK_FROM_ARMCR4_CLK_MASK ) == GCI_CHIPCONTROL_BP_CLK_FROM_ARMCR4_CLK_SET ) ? WICED_TRUE : WICED_FALSE;
}

static uint32_t
platform_cpu_clock_get_freq_for_source( platform_cpu_clock_source_t source )
{
    /*
     * TODO
     * 1.Consider to read PLL dividers and calculate frequency using PLL base frequency.
     * 2.Instead of GCI_CHIPCONTROL_APPS_CPU_FREQ_* constants use PLL clock output number constants.
     *   And another software layer which maps output number to frequency by reading PLL configuration.
     */

    uint32_t ret = PLATFORM_CLOCK_HZ_160MHZ;

    switch ( source )
    {
        case PLATFORM_CPU_CLOCK_SOURCE_ARM:
            switch( platform_gci_chipcontrol( GCI_CHIPCONTROL_APPS_CPU_FREQ_REG, 0x0, 0x0 ) & GCI_CHIPCONTROL_APPS_CPU_FREQ_MASK )
            {
#if defined(PLATFORM_4390X_OVERCLOCK)
                case GCI_CHIPCONTROL_APPS_CPU_FREQ_320_480:
                    switch ( platform_pmu_pllcontrol_mdiv_get( PLL_FREQ_320_480_MHZ_CHANNEL ) )
                    {
                        case PLL_FREQ_480_MHZ_DIVIDER:
                            ret = PLATFORM_CLOCK_HZ_480MHZ;
                            break;

                        case PLL_FREQ_320_MHZ_DIVIDER:
                            ret = PLATFORM_CLOCK_HZ_320MHZ;
                            break;

                        default:
                            wiced_assert( "unknown divider", 0 );
                            break;
                    }
                    break;
#else  /* PLATFORM_4390X_OVERCLOCK */
                case GCI_CHIPCONTROL_APPS_CPU_FREQ_320:
                    ret = PLATFORM_CLOCK_HZ_320MHZ;
                    break;
#endif /* PLATFORM_4390X_OVERCLOCK */
                case GCI_CHIPCONTROL_APPS_CPU_FREQ_160:
                    ret = PLATFORM_CLOCK_HZ_160MHZ;
                    break;

                case GCI_CHIPCONTROL_APPS_CPU_FREQ_120:
                    ret = PLATFORM_CLOCK_HZ_120MHZ;
                    break;

                case GCI_CHIPCONTROL_APPS_CPU_FREQ_80:
                    ret = PLATFORM_CLOCK_HZ_80MHZ;
                    break;

                case GCI_CHIPCONTROL_APPS_CPU_FREQ_60:
                    ret = PLATFORM_CLOCK_HZ_60MHZ;
                    break;

                case GCI_CHIPCONTROL_APPS_CPU_FREQ_48:
                    ret = PLATFORM_CLOCK_HZ_48MHZ;
                    break;

                case GCI_CHIPCONTROL_APPS_CPU_FREQ_24:
                    ret = PLATFORM_CLOCK_HZ_24MHZ;
                    break;

                default:
                    wiced_assert( "unknown cpu frequency", 0 );
                    break;
            }
            break;

        case PLATFORM_CPU_CLOCK_SOURCE_BACKPLANE:
            switch( platform_gci_chipcontrol( GCI_CHIPCONTROL_APPS_BP_FREQ_REG, 0x0, 0x0 ) & GCI_CHIPCONTROL_APPS_BP_FREQ_MASK )
            {
                case GCI_CHIPCONTROL_APPS_BP_FREQ_160:
                    ret = PLATFORM_CLOCK_HZ_160MHZ;
                    break;

                case GCI_CHIPCONTROL_APPS_BP_FREQ_120:
                    ret = PLATFORM_CLOCK_HZ_120MHZ;
                    break;

                case GCI_CHIPCONTROL_APPS_BP_FREQ_80:
                    ret = PLATFORM_CLOCK_HZ_80MHZ;
                    break;

                case GCI_CHIPCONTROL_APPS_BP_FREQ_60:
                    ret = PLATFORM_CLOCK_HZ_60MHZ;
                    break;

                case GCI_CHIPCONTROL_APPS_BP_FREQ_48:
                    ret = PLATFORM_CLOCK_HZ_48MHZ;
                    break;

                case GCI_CHIPCONTROL_APPS_BP_FREQ_24:
                    ret = PLATFORM_CLOCK_HZ_24MHZ;
                    break;

                default:
                    wiced_assert( "unknown backplane frequency", 0 );
                    break;

            }
            break;

        default: /* switch ( source ) */
            wiced_assert( "unknown source", 0 );
            break;
    }

    return ret;
}

static uint32
platform_backplane_clock_get_freq( void )
{
    uint32_t freq;
    uint32_t flags;

    WICED_SAVE_INTERRUPTS( flags );

    if ( platform_cpu_clock_drives_backplane_clock() )
    {
        freq = platform_cpu_clock_get_freq_for_source( PLATFORM_CPU_CLOCK_SOURCE_ARM ) / 2;
    }
    else
    {
        freq = platform_cpu_clock_get_freq_for_source( PLATFORM_CPU_CLOCK_SOURCE_BACKPLANE );
    }

    WICED_RESTORE_INTERRUPTS( flags );

    return freq;
}

static uint32_t
platform_cpu_clock_get_freq( void )
{
    uint32_t freq;
    uint32_t flags;

    WICED_SAVE_INTERRUPTS( flags );

    if ( platform_cpu_clock_get_source() == PLATFORM_CPU_CLOCK_SOURCE_ARM )
    {
        freq = platform_cpu_clock_get_freq_for_source( PLATFORM_CPU_CLOCK_SOURCE_ARM );
    }
    else
    {
        freq = platform_backplane_clock_get_freq();
    }

    WICED_RESTORE_INTERRUPTS( flags );

    return freq;
}

static platform_result_t
platform_cpu_clock_freq_to_config( platform_cpu_clock_frequency_t freq, platform_cpu_clock_freq_config_t* config )
{
    uint8_t  channels[2];
    unsigned i;

#if defined(PLATFORM_4390X_OVERCLOCK)
    PLATFORM_CPU_CLOCK_FREQ_PLL_CONFIG_DEFINE( PLL_FREQ_320_480_MHZ_CHANNEL, PLL_FREQ_320_MHZ_DIVIDER );
#endif /* PLATFORM_4390X_OVERCLOCK */

    switch ( freq )
    {
#if PLATFORM_BCM4390X_APPS_CPU_FREQ_24_AND_48_MHZ_ENABLED
        case PLATFORM_CPU_CLOCK_FREQUENCY_24_MHZ:
            PLATFORM_CPU_CLOCK_FREQ_CONFIG_DEFINE( GCI_CHIPCONTROL_APPS_CPU_FREQ_24, GCI_CHIPCONTROL_APPS_BP_FREQ_24, PLATFORM_CPU_CLOCK_SOURCE_BACKPLANE, WICED_FALSE, 5, -1 );
            break;

        case PLATFORM_CPU_CLOCK_FREQUENCY_48_MHZ:
            PLATFORM_CPU_CLOCK_FREQ_CONFIG_DEFINE( GCI_CHIPCONTROL_APPS_CPU_FREQ_48, GCI_CHIPCONTROL_APPS_BP_FREQ_48, PLATFORM_CPU_CLOCK_SOURCE_BACKPLANE, WICED_FALSE, 5, -1 );
            break;
#endif /* PLATFORM_BCM4390X_APPS_CPU_FREQ_24_AND_48_MHZ_ENABLED */

        case PLATFORM_CPU_CLOCK_FREQUENCY_60_MHZ:
            PLATFORM_CPU_CLOCK_FREQ_CONFIG_DEFINE( GCI_CHIPCONTROL_APPS_CPU_FREQ_60, GCI_CHIPCONTROL_APPS_BP_FREQ_60, PLATFORM_CPU_CLOCK_SOURCE_BACKPLANE, WICED_FALSE, 2, -1 );
            break;

        case PLATFORM_CPU_CLOCK_FREQUENCY_80_MHZ:
            PLATFORM_CPU_CLOCK_FREQ_CONFIG_DEFINE( GCI_CHIPCONTROL_APPS_CPU_FREQ_80, GCI_CHIPCONTROL_APPS_BP_FREQ_80, PLATFORM_CPU_CLOCK_SOURCE_BACKPLANE, WICED_FALSE, 3, -1 );
            break;

        case PLATFORM_CPU_CLOCK_FREQUENCY_120_MHZ:
            PLATFORM_CPU_CLOCK_FREQ_CONFIG_DEFINE( GCI_CHIPCONTROL_APPS_CPU_FREQ_120, GCI_CHIPCONTROL_APPS_BP_FREQ_120, PLATFORM_CPU_CLOCK_SOURCE_BACKPLANE, WICED_FALSE, 2, -1 );
            break;

        case PLATFORM_CPU_CLOCK_FREQUENCY_160_MHZ:
            if ( platform_cpu_clock_get_freq() == PLATFORM_CLOCK_HZ_320MHZ )
            {
                PLATFORM_CPU_CLOCK_FREQ_CONFIG_DEFINE( 0, 0, PLATFORM_CPU_CLOCK_SOURCE_BACKPLANE, WICED_TRUE, -1, -1 );
            }
            else
            {
                PLATFORM_CPU_CLOCK_FREQ_CONFIG_DEFINE( GCI_CHIPCONTROL_APPS_CPU_FREQ_160, GCI_CHIPCONTROL_APPS_BP_FREQ_160, PLATFORM_CPU_CLOCK_SOURCE_BACKPLANE, WICED_FALSE, 3, -1 );
            }
            break;

        case PLATFORM_CPU_CLOCK_FREQUENCY_320_MHZ:
#if defined(PLATFORM_4390X_OVERCLOCK)
            PLATFORM_CPU_CLOCK_FREQ_CONFIG_DEFINE( GCI_CHIPCONTROL_APPS_CPU_FREQ_320_480, GCI_CHIPCONTROL_APPS_BP_FREQ_160, PLATFORM_CPU_CLOCK_SOURCE_ARM, WICED_FALSE, 1, 3 );
#else
            PLATFORM_CPU_CLOCK_FREQ_CONFIG_DEFINE( GCI_CHIPCONTROL_APPS_CPU_FREQ_320, GCI_CHIPCONTROL_APPS_BP_FREQ_160, PLATFORM_CPU_CLOCK_SOURCE_ARM, WICED_FALSE, 1, 3 );
#endif /* PLATFORM_4390X_OVERCLOCK */
            break;

#if defined(PLATFORM_4390X_OVERCLOCK)
        case PLATFORM_CPU_CLOCK_FREQUENCY_480_MHZ:
            /* 480MHZ must run on ARM clock source and backplane use div by 2 ARM clock */
            PLATFORM_CPU_CLOCK_FREQ_CONFIG_DEFINE( GCI_CHIPCONTROL_APPS_CPU_FREQ_320_480, GCI_CHIPCONTROL_APPS_BP_FREQ_160, PLATFORM_CPU_CLOCK_SOURCE_ARM, WICED_FALSE, 1, 3 );
            PLATFORM_CPU_CLOCK_FREQ_PLL_CONFIG_DEFINE( PLL_FREQ_320_480_MHZ_CHANNEL, PLL_FREQ_480_MHZ_DIVIDER );
            break;
#endif /* PLATFORM_4390X_OVERCLOCK */

        default:
            wiced_assert( "Unsupported frequency", 0 );
            return PLATFORM_BADARG;
    }

    for ( i = 0; i < ARRAYSIZE(channels); i++)
    {
        if ( channels[i] != (uint8_t)-1 )
        {
            uint32_t field = ( ( platform_pmu_pllcontrol( PLL_CONTROL_ENABLE_CHANNEL_REG, 0, 0 ) & PLL_CONTROL_ENABLE_CHANNEL_MASK ) >> PLL_CONTROL_ENABLE_CHANNEL_SHIFT );

            if ( ( ( 1U << ( channels[i] - 1 ) ) & field ) != 0 )
            {
                /* Channel disabled */
                return PLATFORM_BADARG;
            }
        }
    }

    return PLATFORM_SUCCESS;
}

wiced_bool_t
platform_cpu_clock_init( platform_cpu_clock_frequency_t freq )
{
    /*
     * This function to be called during system initialization, before RTOS ticks
     * start to run. If there is need to change CPU frequency later, then it can be done
     * using following sequence:
     *    platform_tick_stop();
     *    platform_cpu_clock_init(...);
     *    platform_tick_start();
     * This is to ensure ticks and cycles per ticks are updated correctly.
     */

    platform_cpu_clock_freq_config_t config;
    uint32_t                         cpu_freq;
    uint32_t                         flags;
    platform_result_t                result;

    result = platform_cpu_clock_freq_to_config( freq, &config );
    if ( result != PLATFORM_SUCCESS )
    {
        return WICED_FALSE;
    }

    WICED_SAVE_INTERRUPTS( flags );

    platform_cpu_core_init();

#if defined(PLATFORM_4390X_OVERCLOCK)
    if ( platform_pmu_pllcontrol_mdiv_get( config.pll_channel ) != config.pll_divider )
    {
        platform_pmu_pllcontrol_mdiv_set( config.pll_channel, config.pll_divider );
        platform_cpu_wait_ht();
    }
#endif /* PLATFORM_4390X_OVERCLOCK */

    if ( config.clock_source_only )
    {
        platform_cpu_clock_source( config.clock_source );
    }
    else if ( config.clock_source == PLATFORM_CPU_CLOCK_SOURCE_ARM )
    {
        /* Moving into ARMCR4 2:1 mode. */

        /* Select ARM frequency from selection mux. */
        platform_gci_chipcontrol( GCI_CHIPCONTROL_APPS_CPU_FREQ_REG,
                                  GCI_CHIPCONTROL_APPS_CPU_FREQ_MASK,
                                  config.apps_cpu_freq_reg_value );

        /* Select source of backplane clock, from:
         * (1) local divide-by-2 of APPS CPU clock selection; or
         * (2) APP backplane clock selection.
         * Div-2 intended to source backplane/CPU from a single clock,
         * where skew might be an issue.
         * Beware: There is a silicon bug where div-2 is 180-degrees
         * phase shifted causing issues with some peripherals (like USB),
         * which are intolerant to phase differences.
         */
        platform_gci_chipcontrol( GCI_CHIPCONTROL_BP_CLK_FROM_ARMCR4_CLK_REG,
                                  GCI_CHIPCONTROL_BP_CLK_FROM_ARMCR4_CLK_MASK,
                                  PLATFORM_CLOCK_BP_CLK_FROM_ARMCR4_REG_VALUE );

        /* Select backplane frequency from selection mux.
         * This setting is superfluous if local div-2 is enabled.
         */
        platform_gci_chipcontrol( GCI_CHIPCONTROL_APPS_BP_FREQ_REG,
                                  GCI_CHIPCONTROL_APPS_BP_FREQ_MASK,
                                  config.apps_bp_freq_reg_value );

        /* Change clock source.
         * Do this last, so that backplane and CPU frequencies are
         * already set appropriately.
        */
        platform_cpu_clock_source( config.clock_source );
    }
    else if ( config.clock_source == PLATFORM_CPU_CLOCK_SOURCE_BACKPLANE )
    {
        /* Moving into ARMCR4 1:1 mode. */

        /* Change source.
         * After this point, ARM and backplane clocks will be running from
         * apps_backplane_clk (ARM 1:1 ratio).
         * Do this first!  Moving directly from 2:1 to 1:1 (120/120) wedges
         * when not using local div-2.
         *
         */
        platform_cpu_clock_source( config.clock_source );

        /* Select backplane frequency from selection mux. */
        platform_gci_chipcontrol( GCI_CHIPCONTROL_APPS_BP_FREQ_REG,
                                  GCI_CHIPCONTROL_APPS_BP_FREQ_MASK,
                                  config.apps_bp_freq_reg_value );

        /* Ensure backplane clock is sourced from backplane BBPLL path and
         * not from the local div-2.
         */
        platform_gci_chipcontrol( GCI_CHIPCONTROL_BP_CLK_FROM_ARMCR4_CLK_REG,
                                  GCI_CHIPCONTROL_BP_CLK_FROM_ARMCR4_CLK_MASK,
                                  0x0 );

        /* No need to select ARM frequency from selection mux, since
         * ARM 1:1 ratio will only make use of apps_backplane_clk.
         */
    }
    else
    {
        wiced_assert( "unknown clock source", 0 );
    }

    cpu_freq = platform_cpu_clock_get_freq();

    if ( !platform_lpo_clock_inited )
    {
        /* During initialization ILP clock measured using CPU clock as reference. Let's do this after CPU clock programmed. */
#if PLATFORM_LPO_CLOCK_EXT
        osl_set_ext_lpoclk( cpu_freq );
#else
        osl_set_int_lpoclk( cpu_freq );
#endif /* PLATFORM_LPO_CLK_EXT */

        platform_lpo_clock_inited = WICED_TRUE;
    }

    platform_cpu_clock_hz = cpu_freq;

#if !PLATFORM_TICK_TINY
    platform_tick_init();
#endif /* !PLATFORM_TICK_TINY */

    WICED_RESTORE_INTERRUPTS( flags );

    return WICED_TRUE;
}

uint32_t
platform_reference_clock_get_freq( platform_reference_clock_t clock )
{
    switch ( clock )
    {
        case PLATFORM_REFERENCE_CLOCK_CPU:
            return platform_cpu_clock_get_freq();

        case PLATFORM_REFERENCE_CLOCK_BACKPLANE:
            return platform_backplane_clock_get_freq();

        case PLATFORM_REFERENCE_CLOCK_ALP:
            return osl_alp_clock();

        case PLATFORM_REFERENCE_CLOCK_ILP:
            return osl_ilp_clock();

        case PLATFORM_REFERENCE_CLOCK_FAST_UART:
            return PLATFORM_CLOCK_HZ_160MHZ; /* use fixed PLL output */

        default:
            wiced_assert( "unknown reference clock", 0 );
            return 0;
    }
}

/******************************************************
 *               PMU Timer Function Definitions
 ******************************************************/

static uint32_t
platform_tick_pmu_get_freerunning_stamp( void )
{
    uint32_t time = PLATFORM_PMU->pmutimer;

    if ( time != PLATFORM_PMU->pmutimer )
    {
        time = PLATFORM_PMU->pmutimer;
    }

    return time;
}

static wiced_bool_t
platform_tick_pmu_slow_write_pending( void )
{
    return ( PLATFORM_PMU->pmustatus & PST_SLOW_WR_PENDING ) ? WICED_TRUE : WICED_FALSE;
}

static void
platform_tick_pmu_slow_write_barrier( wiced_bool_t never_timeout )
{
    /*
     * Ensure that previous write to slow (ILP) register is completed.
     * Write placed in intermediate buffer and return immediately.
     * Then it takes few slow ILP clocks to move write from intermediate
     * buffer to actual register. If subsequent write happen before previous
     * moved to actual register, then second write may take long time and also
     * occupy bus. This busy loop try to ensure that previous write is completed.
     * It can be used to avoid long writes, and also as synchronization barrier
     * to ensure that previous register update is completed.
     */

    PLATFORM_TIMEOUT_BEGIN( start_stamp );

    while ( platform_tick_pmu_slow_write_pending() )
    {
        if ( never_timeout == WICED_FALSE )
        {
            PLATFORM_TIMEOUT_SEC_ASSERT( "wait writing for too long", start_stamp, !platform_tick_pmu_slow_write_pending(), TICK_PMU_TIMER_ASSERT_SEC );
        }
    }
}

static void
platform_tick_pmu_slow_write( volatile uint32_t* reg, uint32_t val )
{
    platform_tick_pmu_slow_write_barrier( WICED_FALSE );
    *reg = val;
}

static pmu_res_req_timer_t
platform_tick_pmu_timer_init( uint32_t ticks )
{
    pmu_res_req_timer_t timer;

    timer.raw                   = 0x0;
    timer.bits.time             = ( ticks & TICK_PMU_MAX_COUNTER );
    timer.bits.int_enable       = 1;
    timer.bits.force_ht_request = 1;
    timer.bits.clkreq_group_sel = pmu_res_clkreq_apps_group;

    return timer;
}

static pmu_intstatus_t
platform_tick_pmu_intstatus_init( void )
{
    pmu_intstatus_t status;

    status.raw                      = 0;
    status.bits.rsrc_req_timer_int1 = 1;

    return status;
}

static void
platform_tick_pmu_program_timer_to_stop( platform_tick_timer_t* timer )
{
    platform_tick_pmu_slow_write( &PLATFORM_PMU->res_req_timer1.raw, platform_tick_pmu_timer_init( 0x0 ).raw );

    platform_tick_pmu_slow_write_barrier( WICED_FALSE );

    PLATFORM_PMU->pmuintstatus.raw = platform_tick_pmu_intstatus_init().raw;

    if ( timer != NULL )
    {
        timer->freerunning_stop_stamp = timer->freerunning_current_func( timer );
        timer->tick_count = 0;
    }
}

#if PLATFORM_TICK_PMU

static uint32_t
platform_tick_pmu_wait_stop_complete( platform_tick_timer_t* timer )
{
    uint32_t start_stamp = timer->freerunning_current_func( timer );
    uint32_t curr_stamp  = start_stamp;

    while ( 1 )
    {
        if ( curr_stamp - timer->freerunning_stop_stamp >= PMU_MAX_WRITE_LATENCY_ILP_TICKS )
        {
            return curr_stamp - start_stamp;
        }

        curr_stamp = timer->freerunning_current_func( timer );
    }
}

static void
platform_tick_pmu_program_timer_to_run( platform_tick_timer_t* timer, uint32_t cycles_till_fire )
{
    uint32_t wait_ticks  = platform_tick_pmu_wait_stop_complete( timer );
    uint32_t timer_ticks = ( cycles_till_fire > wait_ticks ) ? ( cycles_till_fire - wait_ticks ) : 1;

    PLATFORM_PMU->pmuintstatus.raw = platform_tick_pmu_intstatus_init().raw;

    platform_tick_pmu_slow_write( &PLATFORM_PMU->res_req_timer1.raw, platform_tick_pmu_timer_init( timer_ticks ).raw );
}

static void
platform_tick_pmu_init( platform_tick_timer_t* timer )
{
    timer->cycles_per_tick = CYCLES_PMU_PER_TICK;
    timer->cycles_per_sec  = platform_reference_clock_get_freq( PLATFORM_REFERENCE_CLOCK_ILP );
    timer->ticks_max       = TICKS_MAX( TICK_PMU_MAX_COUNTER, 33 );
    timer->tick_count = 0;
}

static uint32_t
platform_tick_pmu_freerunning_current( platform_tick_timer_t* timer )
{
    UNUSED_PARAMETER( timer );

    return platform_tick_pmu_get_freerunning_stamp();
}

static void
platform_tick_pmu_start( platform_tick_timer_t* timer, uint32_t cycles_till_fire )
{
    platform_tick_pmu_program_timer_to_run( timer, cycles_till_fire );

    platform_common_chipcontrol( &PLATFORM_PMU->pmuintmask1.raw, 0x0, platform_tick_pmu_intstatus_init().raw );

    timer->started = WICED_TRUE;
}

static void
platform_tick_pmu_reset( platform_tick_timer_t* timer, uint32_t cycles_till_fire )
{
    /*
     * PMU timer runs off the ILP clock, which is derived from External/Internal HIB
     * clock, running at ~32.768Khz, this is rounded off to 32 in pmu_timer->cycles_per_tick,
     * to compensate for the .768Khz, 768(700+60+8) Ticks are added per 32000 ticks
     *
     * cycles_till_fire (set to 32 at the entry of this function)
     * is adjusted every 10, 100 and 1000 ticks to account for the 768 ticks per 32000 ticks.
     */
    timer->tick_count++;
    if ( ( timer->tick_count % 10 ) == 0 )
    {
        cycles_till_fire += 7;
    }

    if ( ( timer->tick_count % 100 ) == 0 )
    {
        cycles_till_fire += 6;
    }

    if ( ( timer->tick_count % 1000 ) == 0 )
    {
        cycles_till_fire += 8;
        timer->tick_count = 0;
    }

    timer->cycles_till_fire = cycles_till_fire;
    platform_tick_pmu_program_timer_to_run( timer, cycles_till_fire );
}

static void
platform_tick_pmu_restart( platform_tick_timer_t* timer, platform_tick_restart_action_t action, uint32_t cycles_till_fire )
{
    switch ( action )
    {
        case PLATFORM_TICK_RESTART_ACTION_BEGIN_TICKLESS_MODE:
            platform_tick_pmu_program_timer_to_stop( timer );
            platform_tick_pmu_program_timer_to_run ( timer, cycles_till_fire );
            break;

        case PLATFORM_TICK_RESTART_ACTION_OPPORTUNISTIC_BEGIN_TICKLESS_MODE:
            /*
             * Command to be used to implement RTOS timer.
             * Guaranteed timer stopping is expensive operation.
             * Be opportunistic here and just program timer without stopping it.
             * As programming (writing) has latency, previous programming may trigger interrupt before new settings take effect.
             * Such spontaneous interrupt will be handled gracefully because reported to RTOS number of ticks spent in tickless mode
             * is based on free-running timer.
             * The price for this opportunistic programming sometimes system may leave sleep mode earlier then it could,
             * but it will back to sleep immediately after that.
             */
            platform_tick_pmu_slow_write( &PLATFORM_PMU->res_req_timer1.raw, platform_tick_pmu_timer_init( cycles_till_fire ).raw );
            break;

        case PLATFORM_TICK_RESTART_ACTION_END_TICKLESS_MODE:
            /* Stop timer. Stopping has latency, so later let's wait for operation completed. */
            platform_tick_pmu_program_timer_to_stop( timer );
            break;

        case PLATFORM_TICK_RESTART_ACTION_BEGIN_NORMAL_MODE:
            /* Before programming timer let's wait till previous stop completed. */
            platform_tick_pmu_program_timer_to_run( timer, cycles_till_fire );
            break;

        default:
            wiced_assert( "bad action", 0 );
    }
}

static void
platform_tick_pmu_stop( platform_tick_timer_t* timer )
{
    timer->started = WICED_FALSE;

    platform_common_chipcontrol( &PLATFORM_PMU->pmuintmask1.raw, platform_tick_pmu_intstatus_init().raw, 0x0 );

    platform_tick_pmu_program_timer_to_stop( timer );
}

static void
platform_tick_pmu_sleep_invoke( platform_tick_timer_t* timer, platform_tick_sleep_idle_func idle_func )
{
    /*
     * When CPU runs on ARM clock, it also request HT clock.
     * So to drop HT clock we must to switch back to backplane clock.
     */

    const wiced_bool_t switch_to_bp = ( platform_cpu_clock_get_source() == PLATFORM_CPU_CLOCK_SOURCE_ARM );

    UNUSED_PARAMETER( timer );

    if ( switch_to_bp )
    {
        platform_cpu_clock_source( PLATFORM_CPU_CLOCK_SOURCE_BACKPLANE );
    }

    idle_func();

    if ( switch_to_bp )
    {
        platform_cpu_clock_source( PLATFORM_CPU_CLOCK_SOURCE_ARM );
    }
}

static platform_tick_timer_t platform_tick_pmu_timer =
{
    .init_func                = platform_tick_pmu_init,
    .freerunning_current_func = platform_tick_pmu_freerunning_current,
    .start_func               = platform_tick_pmu_start,
    .reset_func               = platform_tick_pmu_reset,
    .restart_func             = platform_tick_pmu_restart,
    .stop_func                = platform_tick_pmu_stop,
    .sleep_invoke_func        = platform_tick_pmu_sleep_invoke,
    .deep_sleep_threshold     = PLATFORM_TICK_PMU_DEEP_SLEEP_THRESH,
    .tickless_threshold       = PLATFORM_TICK_PMU_TICKLESS_THRESH,
    .cycles_per_tick          = CYCLES_PMU_INIT_PER_TICK,
    .ticks_max                = TICKS_MAX( TICK_PMU_MAX_COUNTER, CYCLES_PMU_INIT_PER_TICK ),
    .freerunning_start_stamp  = 0,
    .freerunning_stop_stamp   = 0,
    .cycles_till_fire         = 0,
    .started                  = WICED_FALSE
};

#endif /* PLATFORM_TICK_PMU */

/******************************************************
 *               CPU Timer Function Definitions
 ******************************************************/

static inline uint32_t
platform_tick_cpu_get_freerunning_stamp( void )
{
    return PLATFORM_APPSCR4->cycle_cnt;
}

static inline void
platform_tick_cpu_write_sync( volatile uint32_t* reg, uint32_t val )
{
    *reg = val;
    (void)*reg; /* read back to ensure write completed */
}

static inline void
platform_tick_cpu_start_base( uint32_t cycles_per_tick )
{
    PLATFORM_APPSCR4->int_status = IRQN2MASK( Timer_IRQn );
    PLATFORM_APPSCR4->int_timer  = cycles_per_tick;

    platform_irq_enable_int( Timer_IRQn );

    /*
     * To make sure cr4_get_cycle_counter() and nanosecond clock implemented on top of it is running while CPU is in WFI.
     * This affects power consumption, but CPU timer should be used when power-saving disabled, for full power mode.
     */
    PLATFORM_APPSCR4->core_ctrl.bits.wfi_clk_stop = 0;
}

static inline void
platform_tick_cpu_reset_base( uint32_t cycles_till_fire )
{
    PLATFORM_APPSCR4->int_status = IRQN2MASK( Timer_IRQn );
    PLATFORM_APPSCR4->int_timer  = cycles_till_fire;
}

static inline void
platform_tick_cpu_stop_base( void )
{
    /* Restore default value. */
    PLATFORM_APPSCR4->core_ctrl.bits.wfi_clk_stop = 1;

    platform_irq_disable_int( Timer_IRQn );

    platform_tick_cpu_write_sync( &PLATFORM_APPSCR4->int_timer, 0 );
    PLATFORM_APPSCR4->int_status = IRQN2MASK( Timer_IRQn );
}

#if PLATFORM_TICK_CPU

static void
platform_tick_cpu_init( platform_tick_timer_t* timer )
{
    timer->cycles_per_tick = CYCLES_CPU_PER_TICK;
    timer->cycles_per_sec  = platform_reference_clock_get_freq( PLATFORM_REFERENCE_CLOCK_CPU );
    timer->ticks_max       = TICKS_MAX( TICK_CPU_MAX_COUNTER, timer->cycles_per_tick );
}

static uint32_t
platform_tick_cpu_freerunning_current( platform_tick_timer_t* timer )
{
    UNUSED_PARAMETER( timer );

    return platform_tick_cpu_get_freerunning_stamp();
}

static void
platform_tick_cpu_start( platform_tick_timer_t* timer, uint32_t cycles_till_fire )
{
    platform_tick_cpu_start_base( cycles_till_fire );

    timer->started = WICED_TRUE;
}

static void
platform_tick_cpu_reset( platform_tick_timer_t* timer, uint32_t cycles_till_fire )
{
    UNUSED_PARAMETER( timer );

    platform_tick_cpu_reset_base( cycles_till_fire );
}

static void
platform_tick_cpu_restart( platform_tick_timer_t* timer, platform_tick_restart_action_t action, uint32_t cycles_till_fire )
{
    UNUSED_PARAMETER( timer );
    UNUSED_PARAMETER( action );

    if ( cycles_till_fire )
    {
        platform_tick_cpu_write_sync( &PLATFORM_APPSCR4->int_timer, 0 );
        PLATFORM_APPSCR4->int_status = IRQN2MASK( Timer_IRQn );
        PLATFORM_APPSCR4->int_timer  = cycles_till_fire;
    }
}

static void
platform_tick_cpu_stop( platform_tick_timer_t* timer )
{
    timer->started = WICED_FALSE;

    platform_tick_cpu_stop_base();
}

static void
platform_tick_cpu_sleep_invoke( platform_tick_timer_t* timer, platform_tick_sleep_idle_func idle_func )
{
    UNUSED_PARAMETER( timer );

    /*
     * When CPU timer used for ticks we always keep HT clock running.
     * No need to drop request to HT clock.
     */

    idle_func();
}

static platform_tick_timer_t platform_tick_cpu_timer =
{
    .init_func                = platform_tick_cpu_init,
    .freerunning_current_func = platform_tick_cpu_freerunning_current,
    .start_func               = platform_tick_cpu_start,
    .reset_func               = platform_tick_cpu_reset,
    .restart_func             = platform_tick_cpu_restart,
    .stop_func                = platform_tick_cpu_stop,
    .sleep_invoke_func        = platform_tick_cpu_sleep_invoke,
    .deep_sleep_threshold     = PLATFORM_TICK_CPU_DEEP_SLEEP_THRESH,
    .tickless_threshold       = PLATFORM_TICK_CPU_TICKLESS_THRESH,
    .cycles_per_tick          = CYCLES_CPU_INIT_PER_TICK,
    .ticks_max                = TICKS_MAX( TICK_CPU_MAX_COUNTER, CYCLES_CPU_INIT_PER_TICK ),
    .freerunning_start_stamp  = 0,
    .freerunning_stop_stamp   = 0,
    .cycles_till_fire         = 0,
    .started                  = WICED_FALSE
};

#endif /* PLATFORM_TICK_CPU */

/******************************************************
 *               Timer API Function Definitions
 ******************************************************/

#if PLATFORM_TICK_TINY

static uint32_t
platform_tick_cycles_per_tick( void )
{
    return CYCLES_TINY_PER_TICK;
}

void
platform_tick_timer_init( void )
{

}

void
platform_tick_start( void )
{
    platform_tick_cpu_start_base( platform_tick_cycles_per_tick() );
}

void
platform_tick_stop( void )
{
    platform_tick_cpu_stop_base();
}

wiced_bool_t
platform_tick_irq_handler( void )
{
    platform_tick_cpu_reset_base( platform_tick_cycles_per_tick() );
    return WICED_TRUE;
}

#else

static wiced_bool_t
platform_tick_any_interrupt_pending( void )
{
    return ( ( PLATFORM_APPSCR4->irq_mask & PLATFORM_APPSCR4->fiqirq_status ) != 0 );
}

static void
platform_tick_init( void )
{
    platform_pmu_cycles_per_tick = CYCLES_PMU_PER_TICK;

#if PLATFORM_TICK_CPU
    platform_tick_cpu_timer.init_func( &platform_tick_cpu_timer );
#endif /* PLATFORM_TICK_CPU */

#if PLATFORM_TICK_PMU
    platform_tick_pmu_timer.init_func( &platform_tick_pmu_timer );
#endif /* PLATFORM_TICK_PMU */

    if ( platform_tick_current_timer == NULL )
    {
#if PLATFORM_TICK_CPU
        /* Use CPU clock as first choice as handling incur less overhead and results are more precise. */
        platform_tick_current_timer = &platform_tick_cpu_timer;
#elif PLATFORM_TICK_PMU
        platform_tick_current_timer = &platform_tick_pmu_timer;
#endif /* PLATFORM_TICK_CPU */
    }
}

#if PLATFORM_TICK_CPU && PLATFORM_TICK_PMU

static void
platform_tick_switch_timer( platform_tick_timer_t* timer )
{
    platform_tick_timer_t* timer_curr = platform_tick_current_timer;

    wiced_assert( "timers must be non-NULL", timer && timer_curr );

    if ( ( timer != timer_curr ) && timer_curr->started )
    {
        const float    convert_coeff = (float)timer->cycles_per_tick / (float)timer_curr->cycles_per_tick;
        const uint32_t restart_curr  = timer_curr->cycles_till_fire;
        const uint32_t passed_curr   = timer_curr->freerunning_current_func( timer_curr ) - timer_curr->freerunning_start_stamp;
        const uint32_t restart_new   = MIN( MAX( ( uint32_t )( ( float )restart_curr * convert_coeff + 0.5f ), 1 ), timer->cycles_per_tick );
        const uint32_t passed_new    = MIN( ( uint32_t )( ( float )passed_curr * convert_coeff + 0.5f ), restart_new - 1 );

        timer->freerunning_start_stamp = timer->freerunning_current_func( timer ) - passed_new;
        timer->cycles_till_fire        = restart_new;

        timer_curr->stop_func( timer_curr );

        timer->start_func( timer, restart_new - passed_new );
    }

    platform_tick_current_timer = timer;
}

void platform_tick_execute_command( platform_tick_command_t command )
{
    static wiced_bool_t powersave_enabled  = WICED_FALSE;
    static wiced_bool_t pmu_timer_released = WICED_FALSE;
    uint32_t flags;

    WICED_SAVE_INTERRUPTS( flags );

    switch ( command )
    {
        case PLATFORM_TICK_COMMAND_POWERSAVE_BEGIN:
            powersave_enabled = WICED_TRUE;
            break;

        case PLATFORM_TICK_COMMAND_POWERSAVE_END:
            powersave_enabled = WICED_FALSE;
            break;

        case PLATFORM_TICK_COMMAND_RELEASE_PMU_TIMER_BEGIN:
            pmu_timer_released = WICED_TRUE;
            break;

        case PLATFORM_TICK_COMMAND_RELEASE_PMU_TIMER_END:
            pmu_timer_released = WICED_FALSE;
            break;

        default:
            wiced_assert( "unknown event", 0 );
            break;
    }

    if ( powersave_enabled && !pmu_timer_released )
    {
        platform_tick_switch_timer( &platform_tick_pmu_timer );
    }
    else
    {
        platform_tick_switch_timer( &platform_tick_cpu_timer );
    }

    WICED_RESTORE_INTERRUPTS( flags );
}

#define PLATFORM_TICK_EXECUTE_COMMAND_IMPLEMENTED

#endif /* PLATFORM_TICK_CPU && PLATFORM_TICK_PMU */

static void
platform_tick_restart( platform_tick_timer_t* timer, platform_tick_restart_action_t action, uint32_t cycles_till_fire )
{
    if ( cycles_till_fire )
    {
        timer->freerunning_start_stamp = timer->freerunning_current_func( timer );
        timer->cycles_till_fire        = cycles_till_fire;
    }

    timer->restart_func( timer, action, cycles_till_fire );
}

void
platform_tick_timer_init( void )
{
    platform_tick_init();
}

void
platform_tick_start( void )
{
    platform_tick_timer_t* timer = platform_tick_current_timer;

    wiced_assert( "no current timer set", timer );

    timer->freerunning_start_stamp = timer->freerunning_current_func( timer );
    timer->cycles_till_fire        = timer->cycles_per_tick;

    timer->start_func( timer, timer->cycles_per_tick );
}

void
platform_tick_stop( void )
{
    platform_tick_timer_t* timer = platform_tick_current_timer;

    wiced_assert( "no current timer set", timer );

    timer->stop_func( timer );
}

static void
platform_tick_reset( platform_tick_timer_t* timer )
{
    const uint32_t current          = timer->freerunning_current_func( timer );
    const uint32_t passed           = current - timer->freerunning_start_stamp;
    const uint32_t next_sched       = timer->cycles_till_fire + timer->cycles_per_tick;
    const uint32_t cycles_till_fire = ( passed < next_sched ) ? ( next_sched - passed ) : 1;

    timer->freerunning_start_stamp = current;
    timer->cycles_till_fire        = cycles_till_fire;

    timer->reset_func( timer, cycles_till_fire );
}

wiced_bool_t
platform_tick_irq_handler( void )
{
    wiced_bool_t res = WICED_FALSE;

#if PLATFORM_APPS_POWERSAVE
    if ( PLATFORM_PMU->ext_wakeup_status.raw & PLATFORM_PMU->ext_wake_mask1.raw )
    {
        /*
         * External wake-up event triggered.
         * We don't want to have storm of interrupts, so ack it here.
         * Wake-up event sets same bit in pmu intstatus register as timer.
         * So clear here wake-up bit and if pmu intstatus bit not disappear -
         * it means we also have timer interrupt, and as result ISR will be re-triggered.
         *
         * To wake-up event be able to triggered again, it need to be acked first.
         * Please check platform_mcu_powersave_gpio_wakeup_ack() and similar.
         */
        platform_pmu_stored_ext_wakeup.raw  = PLATFORM_PMU->ext_wakeup_status.raw & PLATFORM_PMU->ext_wake_mask1.raw;
        PLATFORM_PMU->ext_wakeup_status.raw = platform_pmu_stored_ext_wakeup.raw;
        res = WICED_TRUE;
    }
#if PLATFORM_TICK_PMU
    else
#endif
#endif /* PLATFORM_APPS_POWERSAVE */
#if PLATFORM_TICK_PMU
    if ( PLATFORM_PMU->pmuintstatus.bits.rsrc_req_timer_int1 && PLATFORM_PMU->pmuintmask1.bits.rsrc_req_timer_int1 )
    {
        wiced_assert( "not current timer", platform_tick_current_timer == &platform_tick_pmu_timer );
        platform_tick_reset( &platform_tick_pmu_timer );
        res = WICED_TRUE;
    }
#endif /* PLATFORM_TICK_PMU */

#if PLATFORM_TICK_CPU
    if ( PLATFORM_APPSCR4->int_status & PLATFORM_APPSCR4->int_mask & IRQN2MASK( Timer_IRQn ) )
    {
        wiced_assert( "not current timer", platform_tick_current_timer == &platform_tick_cpu_timer );
        platform_tick_reset( &platform_tick_cpu_timer );
        res = WICED_TRUE;
    }
#endif /* PLATFORM_TICK_CPU */

#if PLATFORM_WLAN_POWERSAVE
    if ( PLATFORM_PMU->pmuintstatus.bits.rsrc_event_int1 && PLATFORM_PMU->pmuintmask1.bits.rsrc_event_int1 )
    {
        platform_wlan_powersave_res_event();
    }
#endif /* PLATFORM_WLAN_POWERSAVE */

    return res;
}

void platform_tick_sleep_clear_ext_wake( void )
{
#if PLATFORM_APPS_POWERSAVE
    platform_pmu_stored_ext_wakeup.raw = 0;
#endif
}

static uint32_t
platform_tick_sleep_common( platform_tick_sleep_idle_func idle_func, uint32_t ticks, wiced_bool_t opportunistic )
{
    platform_tick_timer_t* timer           = platform_tick_current_timer;
    const uint32_t         cycles_per_tick = timer->cycles_per_tick;
    const wiced_bool_t     tickless        = ( ticks != 0 ) ? WICED_TRUE : WICED_FALSE;
    uint32_t               ret             = 0;
    double ticks_float = (double)(timer->cycles_per_sec)/SYSTICK_FREQUENCY;

#if PLATFORM_APPS_POWERSAVE
    /* Return immediately if external wake-up event detected. */
    if ( platform_pmu_stored_ext_wakeup.raw != 0 )
    {
        return 0;
    }
#endif

    /* Reconfigure timer if tickless mode. */
    if ( tickless )
    {
        const uint32_t old_restart = timer->cycles_till_fire;
        const uint32_t requested   = (uint32_t)((ticks_float * MIN( ticks, timer->ticks_max )));
        const uint32_t passed      = timer->freerunning_current_func( timer ) - timer->freerunning_start_stamp;
        uint32_t       new_restart;

        if ( requested + old_restart > passed )
        {
            new_restart = requested + old_restart - passed;
        }
        else
        {
            new_restart = cycles_per_tick - ( ( passed - requested - old_restart ) % cycles_per_tick );
        }

        if ( passed >= old_restart )
        {
            ret = 1 + (passed - old_restart) / cycles_per_tick;
        }

        if ( opportunistic )
        {
            platform_tick_restart( timer, PLATFORM_TICK_RESTART_ACTION_OPPORTUNISTIC_BEGIN_TICKLESS_MODE, new_restart );
        }
        else
        {
            platform_tick_restart( timer, PLATFORM_TICK_RESTART_ACTION_BEGIN_TICKLESS_MODE, new_restart );
        }
    }

    /* Here CPU is going to enter sleep mode. */
    platform_pmu_last_sleep_stamp = platform_tick_pmu_get_freerunning_stamp();
    timer->sleep_invoke_func( timer, idle_func );

    /* Notify tickless mode completed */
    if ( tickless )
    {
        platform_tick_restart( timer, PLATFORM_TICK_RESTART_ACTION_END_TICKLESS_MODE, 0 );
    }

    /* Reconfigure timer back. */
    if ( tickless )
    {
        const uint32_t old_restart = timer->cycles_till_fire;
        const uint32_t passed      = timer->freerunning_current_func( timer ) - timer->freerunning_start_stamp;
        uint32_t       new_restart;

        ret += (uint32_t)(( old_restart + ticks_float - 1 ) / (double)ticks_float);
        if ( passed <= old_restart )
        {
            const uint32_t remain  = old_restart - passed;
            ret                   -= (uint32_t)(( remain + ticks_float - 1 ) / ticks_float);
            new_restart            = remain % cycles_per_tick;
        }
        else
        {
            const uint32_t wrapped        = passed - old_restart;
            const double wrapped_ticks    = (double)wrapped / ticks_float;
            const double wrapped_rem      = wrapped - wrapped_ticks * ticks_float;
            ret                          += (uint32_t)wrapped_ticks;
            new_restart                   = (uint32_t)(ticks_float - wrapped_rem);
        }

        platform_tick_restart( timer, PLATFORM_TICK_RESTART_ACTION_BEGIN_NORMAL_MODE, new_restart ? new_restart : cycles_per_tick );
    }

    return ret;
}

uint32_t
platform_tick_sleep_rtos( platform_tick_sleep_idle_func idle_func, uint32_t ticks, wiced_bool_t powersave_permission )
{
    /* Function expect to be called with interrupts disabled. */

    platform_tick_timer_t* timer = platform_tick_current_timer;

    UNUSED_PARAMETER( powersave_permission );

    /*
     * If any interrupt pending better not try to sleep as sleep would immediately return.
     * Entering sleep (especially tickless) is expensive operation while check for pending interrupts is quite cheap.
     */
    if ( platform_tick_any_interrupt_pending() )
    {
        return 0;
    }

    /* Check tickless threshold */
    if ( ticks < timer->tickless_threshold )
    {
        /* Disable tickless mode */
        ticks = 0;
    }

#if PLATFORM_APPS_POWERSAVE
    if ( !powersave_permission || ( ticks < timer->deep_sleep_threshold ) )
    {
        if ( ( platform_mcu_powersave_get_mode() == PLATFORM_MCU_POWERSAVE_MODE_DEEP_SLEEP ) )
        {
            /*
             * Sleeping (executing WFI) now can put overall system into deep-sleep.
             * Powersave is disabled or ticks to sleep are below quite large treshold, so temporarily switch to safe mode.
             */
            uint32_t ret = 0;

            platform_mcu_powersave_set_mode( PLATFORM_MCU_POWERSAVE_MODE_SLEEP );

            ret = platform_tick_sleep_common( idle_func, ticks, WICED_TRUE );

            platform_mcu_powersave_set_mode( PLATFORM_MCU_POWERSAVE_MODE_DEEP_SLEEP );

            return ret;
        }
    }
#endif /* PLATFORM_APPS_POWERSAVE */

    return platform_tick_sleep_common( idle_func, ticks, WICED_TRUE );
}

uint32_t
platform_tick_sleep_force( platform_tick_sleep_idle_func idle_func, uint32_t ticks, platform_tick_sleep_force_interrupts_mode_t mode )
{
    /*
     * Function expect to be called with interrupts disabled.
     *
     * Zero ticks parameter means sleep indefinitely.
     *
     * Wake-up is either via timer or GPIO which both trigger same PMU timer interrupt.
     * So even if tick timer stopped and timer interrupts are masked, we need to unmask timer interrupt.
     * In this case mask is restored before exit and so interrupt would be not triggered.
     * We rely on tick driver not only mask interrupt when stopped but also acknowledge pending interrupt if any,
     * otherwise sleep will be exited earlier.
     */

    const uint32_t  pmu_timer_intmask = platform_tick_pmu_intstatus_init().raw;
    const uint32_t  irq_timer_mask    = IRQN2MASK(Timer_ExtIRQn);
    uint32_t        pmu_intmask;
    uint32_t        irq_mask;
    uint32_t        pmu_new_intmask;
    uint32_t        irq_new_mask;
    uint32_t        ret;

    /* Sleep for indefinite time, so let's stop ticking to not wake-up. */
    if ( ticks == 0 )
    {
        platform_tick_stop( );
    }

    /* Read current interrupts masks */
    pmu_intmask = PLATFORM_PMU->pmuintmask1.raw;
    irq_mask    = PLATFORM_APPSCR4->irq_mask;

    /* Update interrupt masks */
    if ( ( mode == PLATFORM_TICK_SLEEP_FORCE_INTERRUPTS_OFF ) || ( mode == PLATFORM_TICK_SLEEP_FORCE_INTERRUPTS_WLAN_ON ) )
    {
        /* Enable PMU timer interrupt only */
        pmu_new_intmask = pmu_timer_intmask;

        /* Enable APPSCR4 timer interrupt only and conditionally WLAN */
        irq_new_mask = irq_timer_mask;
        if ( mode == PLATFORM_TICK_SLEEP_FORCE_INTERRUPTS_WLAN_ON )
        {
            irq_new_mask |= irq_mask & ( IRQN2MASK(M2M_ExtIRQn) | IRQN2MASK(SW0_ExtIRQn) );
        }
    }
    else
    {
        pmu_new_intmask = pmu_intmask | pmu_timer_intmask;
        irq_new_mask    = irq_mask | irq_timer_mask;
    }

    /* Set masks */
    if ( pmu_intmask != pmu_new_intmask )
    {
        PLATFORM_PMU->pmuintmask1.raw = pmu_new_intmask;
    }
    if ( irq_mask != irq_new_mask )
    {
        PLATFORM_APPSCR4->irq_mask = irq_new_mask;
    }

    /* Go to sleep */
    ret = platform_tick_sleep_common( idle_func, ticks, WICED_FALSE );

    /* Restore masks */
    if ( irq_mask != irq_new_mask )
    {
        PLATFORM_APPSCR4->irq_mask = irq_mask;
    }
    if ( pmu_intmask != pmu_new_intmask )
    {
        PLATFORM_PMU->pmuintmask1.raw = pmu_intmask;
    }

    /* Start ticks back */
    if ( ticks == 0 )
    {
        platform_tick_start( );
    }

    return ret;
}

#endif /* PLATFORM_TICK_TINY */

#ifndef PLATFORM_TICK_EXECUTE_COMMAND_IMPLEMENTED

void platform_tick_execute_command( platform_tick_command_t command )
{
    UNUSED_PARAMETER( command );
}

#endif /* PLATFORM_TICK_EXECUTE_COMMAND_IMPLEMENTED */

void
platform_tick_irq_init( void )
{
    platform_pmu_last_deep_sleep_stamp = platform_pmu_last_sleep_stamp;

    if ( WICED_DEEP_SLEEP_IS_WARMBOOT( ) )
    {
        /* Cleanup interrupts state which can survived warm reboot. */

        pmu_intstatus_t status;

        PLATFORM_PMU->pmuintmask1.raw   = 0x0;

#if PLATFORM_TICK_PMU
        platform_tick_pmu_program_timer_to_stop( &platform_tick_pmu_timer );
#else
        platform_tick_pmu_program_timer_to_stop( NULL );
#endif

        status.raw                      = 0;
        status.bits.rsrc_event_int1     = 1;
        status.bits.rsrc_req_timer_int1 = 1;
        PLATFORM_PMU->pmuintstatus.raw  = status.raw;
    }
    else
    {
        pmu_intctrl_t intctrl;

        /* Make sure that when PMU interrupt triggered it is requested correct domain clocks. */

        intctrl.raw                   = 0x0;
        intctrl.bits.force_ht_request = 1;

        intctrl.bits.clkreq_group_sel = pmu_res_clkreq_wlan_group;
        PLATFORM_PMU->pmuintctrl0.raw = intctrl.raw;

#if !PLATFORM_WLAN_ASSISTED_WAKEUP
        /*
         * When PMU interrupt is triggered PMU itself pulls-up resource needed
         * to run corresponded to group CPU. So normally we should use
         * APPS group for pmuintctrl1 which used by APPS domain.
         * But for WLAN assisted wake-up interrupt must not run ACPU
         * straight away and instead WCPU will make ACPU run when ready.
         * So for WLAN assisted wakeup let's use WLAN group and let
         * WCPU run instead.
         *
         * This clock group select feature to run CPU is of pmuintctrlN register only.
         */
        intctrl.bits.clkreq_group_sel = pmu_res_clkreq_apps_group;
#endif
        PLATFORM_PMU->pmuintctrl1.raw = intctrl.raw;
    }

    /*
     * Let's have APPSCR4 timer and PMU interrupts (timer1 and event1) share same OOB line and be delivered to same Timer_ExtIRQn interrupt status bit.
     *
     * First, let's make APPSCR4 timer output irq requests to same bus line as PMU timer1.
     */
    platform_irq_remap_source( PLATFORM_APPSCR4_MASTER_WRAPPER_REGBASE(0x0),
                               OOB_APPSCR4_TIMER_IRQ_NUM,
                               OOB_AOUT_PMU_INTR1 );
    /* Second, let's route this bus line to Timer_ExtIRQn bit. */
    platform_irq_remap_sink( OOB_AOUT_PMU_INTR1, Timer_ExtIRQn );
    /* Third, enable this line. */
    platform_irq_enable_irq( Timer_ExtIRQn );
}

uint32_t platform_tick_get_time( platform_tick_times_t which_time )
{
    uint32_t time = 0;

    switch ( which_time )
    {
        case PLATFORM_TICK_GET_SLOW_TIME_STAMP:
            time = platform_tick_pmu_get_freerunning_stamp();
            break;

        case PLATFORM_TICK_GET_FAST_TIME_STAMP:
            time = platform_tick_cpu_get_freerunning_stamp();
            break;

        case PLATFORM_TICK_GET_AND_RESET_SLOW_TIME_STAMP:
            time = platform_tick_pmu_get_freerunning_stamp();
            platform_tick_pmu_slow_write( &PLATFORM_PMU->pmutimer, 0 );
            platform_tick_pmu_slow_write_barrier( WICED_TRUE );
            break;

        case PLATFORM_TICK_GET_AND_RESET_FAST_TIME_STAMP:
            time = platform_tick_cpu_get_freerunning_stamp();
            PLATFORM_APPSCR4->cycle_cnt = 0;
            break;

        case PLATFORM_TICK_GET_TICKS_SINCE_LAST_SLEEP:
            time = ( platform_tick_pmu_get_freerunning_stamp() - platform_pmu_last_sleep_stamp ) / platform_pmu_cycles_per_tick;
            break;

        case PLATFORM_TICK_GET_TICKS_SINCE_LAST_DEEP_SLEEP:
            time = ( platform_tick_pmu_get_freerunning_stamp() - platform_pmu_last_deep_sleep_stamp ) / platform_pmu_cycles_per_tick;
            break;

        default:
            wiced_assert( "unhandled case", 0 );
            break;
    }

    return time;
}
