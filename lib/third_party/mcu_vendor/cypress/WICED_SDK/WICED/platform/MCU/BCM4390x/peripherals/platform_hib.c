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

/* float_t types conflict with the same typedefs from the standard ANSI-C
 * math.h header file. Don't re-typedef them in typedefs.h header file.
 */
#define TYPEDEF_FLOAT_T
#include "math.h"
#include "typedefs.h"
#include "wiced_osl.h"
#undef TYPEDEF_FLOAT_T

#include "platform_peripheral.h"
#include "platform_appscr4.h"
#include "platform_config.h"
#include "wwd_assert.h"

/******************************************************
 *                      Macros
 ******************************************************/

#define PLATFORM_HIBERNATE_CLOCK_INTERNAL_FREQ_128KHZ  128000
#define PLATFORM_HIBERNATE_CLOCK_INTERNAL_FREQ_32KHZ   32000
#define PLATFORM_HIBERNATE_CLOCK_INTERNAL_FREQ_16KHZ   16000
#define PLATFORM_HIBERNATE_CLOCK_EXTERNAL_DEFAULT_FREQ 32768

#define PLATFORM_HIBERNATE_CLOCK_AS_EXT_LPO            ( PLATFORM_LPO_CLOCK_EXT && PLATFORM_HIB_CLOCK_AS_EXT_LPO )
#define PLATFORM_HIBERNATE_CLOCK_INIT                  ( PLATFORM_HIB_ENABLE || PLATFORM_HIBERNATE_CLOCK_AS_EXT_LPO )
#define PLATFORM_HIBERNATE_CLOCK_POWER_UP              ( PLATFORM_HIBERNATE_CLOCK_INIT || PLATFORM_HIB_CLOCK_POWER_UP )

#define HIB_CLOCK_INTERNAL_RC_CODES_COUNT              ((GCI_CHIPCONTROL_HIB_WAKE_CTL_LPO_RC_MASK >> GCI_CHIPCONTROL_HIB_WAKE_CTL_LPO_RC_SHIFT) + 1)
#define HIB_CLOCK_INTERNAL_RC_CODE_VALID(rc_code)      ((rc_code >= 0) && (rc_code < HIB_CLOCK_INTERNAL_RC_CODES_COUNT))
#define HIB_CLOCK_INTERNAL_ERROR_VALUE(val, set)       ((val > set) ? (val - set) : (set - val))

/* HIB internal clock frequency settling time (us) */
#define HIB_CLOCK_INTERNAL_SETTLING_TIME_US            100

/* Low threshold for HIB internal clock frequency in Hz */
#define HIB_CLOCK_INTERNAL_FREQ_THRESHOLD_LOW(set)     (set - 1500)

/* High threshold for HIB internal clock frequency in Hz */
#define HIB_CLOCK_INTERNAL_FREQ_THRESHOLD_HIGH(set)    (set + 500)

/* Check for HIB internal clock frequency in valid range */
#define HIB_CLOCK_INTERNAL_FREQ_VALID(val, set)       ((val >= HIB_CLOCK_INTERNAL_FREQ_THRESHOLD_LOW(set)) && (val <= HIB_CLOCK_INTERNAL_FREQ_THRESHOLD_HIGH(set)))

#define HIB_CLOCK_INTERNAL_RC_CODE_INIT(rc_code)                                                 \
    do                                                                                           \
    {                                                                                            \
        uint32_t wake_ctl_val = 0x0;                                                             \
        uint32_t wake_ctl_mask = 0x0;                                                            \
        wake_ctl_val  |= GCI_CHIPCONTROL_HIB_WAKE_CTL_LPO_RC_VAL(rc_code);                       \
        wake_ctl_mask |= GCI_CHIPCONTROL_HIB_WAKE_CTL_LPO_RC_MASK;                               \
        platform_gci_chipcontrol(GCI_CHIPCONTROL_HIB_WAKE_CTL_REG, wake_ctl_mask, wake_ctl_val); \
    }                                                                                            \
    while( 0 )

#define HIB_CLOCK_INTERNAL_FREQ_MEASURE                 \
    do                                                  \
    {                                                   \
        OSL_DELAY(HIB_CLOCK_INTERNAL_SETTLING_TIME_US); \
        osl_ilp_clock_measure(platform_cpu_clock_hz);   \
    }                                                   \
    while( 0 )

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

/******************************************************
 *               Function Definitions
 ******************************************************/

static platform_result_t platform_hibernate_clock_power_up( const platform_hibernation_t* hib )
{
#if PLATFORM_HIBERNATE_CLOCK_POWER_UP

    const wiced_bool_t external_clock = ( hib->clock == PLATFORM_HIBERNATION_CLOCK_EXTERNAL );
    uint32_t wake_ctl_mask            = 0x0;
    uint32_t wake_ctl_val             = 0x0;

    if ( !external_clock )
    {
        switch ( hib->clock )
        {
            case PLATFORM_HIBERNATION_CLOCK_INTERNAL_128KHZ:
                wake_ctl_val |= GCI_CHIPCONTROL_HIB_WAKE_CTL_LPO_FREQ_128KHZ;
                break;

            case PLATFORM_HIBERNATION_CLOCK_INTERNAL_32KHZ:
                wake_ctl_val |= GCI_CHIPCONTROL_HIB_WAKE_CTL_LPO_FREQ_32KHZ;
                break;

            case PLATFORM_HIBERNATION_CLOCK_INTERNAL_16KHZ:
                wake_ctl_val |= GCI_CHIPCONTROL_HIB_WAKE_CTL_LPO_FREQ_16KHZ;
                break;

            default:
                wiced_assert( "unknown requested clock", 0 );
                return PLATFORM_BADARG;
        }

        wake_ctl_mask |= GCI_CHIPCONTROL_HIB_WAKE_CTL_LPO_FREQ_MASK;
    }

    if ( hib->rc_code >= 0 )
    {
        wake_ctl_val  |= GCI_CHIPCONTROL_HIB_WAKE_CTL_LPO_RC_VAL( (uint32_t)hib->rc_code );
        wake_ctl_mask |= GCI_CHIPCONTROL_HIB_WAKE_CTL_LPO_RC_MASK;
    }

    if ( wake_ctl_mask || wake_ctl_val )
    {
        platform_gci_chipcontrol( GCI_CHIPCONTROL_HIB_WAKE_CTL_REG, wake_ctl_mask, wake_ctl_val );
    }

    if ( external_clock )
    {
        platform_gci_chipcontrol( GCI_CHIPCONTROL_HIB_WAKE_CTL_REG, GCI_CHIPCONTROL_HIB_WAKE_CTL_XTAL_DOWN_MASK, 0x0 );
    }
    else
    {
        platform_gci_chipcontrol( GCI_CHIPCONTROL_HIB_WAKE_CTL_REG, GCI_CHIPCONTROL_HIB_WAKE_CTL_LPO_DOWN_MASK, 0x0 );
    }

#else

    UNUSED_PARAMETER( hib );

#endif /* PLATFORM_HIBERNATE_CLOCK_POWER_UP */

    return PLATFORM_SUCCESS;
}

#if PLATFORM_HIBERNATE_CLOCK_INIT

static uint32_t platform_hibernation_get_raw_status( uint32_t selector )
{
    platform_gci_chipcontrol( GCI_CHIPCONTROL_HIB_READ_SEL_REG,
                              GCI_CHIPCONTROL_HIB_READ_SEL_MASK,
                              selector);

    return ( platform_gci_chipstatus( GCI_CHIPSTATUS_HIB_READ_REG ) & GCI_CHIPSTATUS_HIB_READ_MASK ) >> GCI_CHIPSTATUS_HIB_READ_SHIFT;
}

static hib_status_t platform_hibernation_get_status( void )
{
    hib_status_t status = { .raw = platform_hibernation_get_raw_status( GCI_CHIPCONTROL_HIB_READ_SEL_STATUS ) };
    return status;
}

static platform_result_t platform_hibernation_clock_init( const platform_hibernation_t* hib, uint32_t* hib_ext_clock_freq )
{
    const wiced_bool_t      external_clock  = ( hib->clock == PLATFORM_HIBERNATION_CLOCK_EXTERNAL );
    const platform_result_t power_up_result = platform_hibernate_clock_power_up( hib );

    if ( power_up_result != PLATFORM_SUCCESS )
    {
        return power_up_result;
    }

    if ( external_clock )
    {
        /* If you are hung here, check whether your platform supports an external clock! */
        while ( platform_hibernation_get_status( ).bits.pmu_ext_lpo_avail == 0 );

        platform_gci_chipcontrol( GCI_CHIPCONTROL_HIB_FORCE_EXT_LPO_REG, GCI_CHIPCONTROL_HIB_FORCE_EXT_LPO_MASK, GCI_CHIPCONTROL_HIB_FORCE_EXT_LPO_EXEC );
        platform_gci_chipcontrol( GCI_CHIPCONTROL_HIB_FORCE_INT_LPO_REG, GCI_CHIPCONTROL_HIB_FORCE_INT_LPO_MASK, 0x0 );

        platform_gci_chipcontrol( GCI_CHIPCONTROL_HIB_WAKE_CTL_REG, GCI_CHIPCONTROL_HIB_WAKE_CTL_LPO_DOWN_MASK, GCI_CHIPCONTROL_HIB_WAKE_CTL_LPO_DOWN_EXEC );
    }
    else
    {
        platform_gci_chipcontrol( GCI_CHIPCONTROL_HIB_FORCE_INT_LPO_REG, GCI_CHIPCONTROL_HIB_FORCE_INT_LPO_MASK, GCI_CHIPCONTROL_HIB_FORCE_INT_LPO_EXEC );
        platform_gci_chipcontrol( GCI_CHIPCONTROL_HIB_FORCE_EXT_LPO_REG, GCI_CHIPCONTROL_HIB_FORCE_EXT_LPO_MASK, 0x0 );

        platform_gci_chipcontrol( GCI_CHIPCONTROL_HIB_WAKE_CTL_REG, GCI_CHIPCONTROL_HIB_WAKE_CTL_XTAL_DOWN_MASK, GCI_CHIPCONTROL_HIB_WAKE_CTL_XTAL_DOWN_EXEC );
    }

    if ( hib_ext_clock_freq != NULL )
    {
        *hib_ext_clock_freq = ( hib->hib_ext_clock_freq == 0 ) ? PLATFORM_HIBERNATE_CLOCK_EXTERNAL_DEFAULT_FREQ : hib->hib_ext_clock_freq;
    }

    return PLATFORM_SUCCESS;
}

#else

static platform_result_t platform_hibernation_clock_init( const platform_hibernation_t* hib, uint32_t* hib_ext_clock_freq )
{
    platform_result_t result = platform_hibernate_clock_power_up( hib );

    UNUSED_PARAMETER( hib_ext_clock_freq );

    return result;
}

#endif /* PLATFORM_HIBERNATE_CLOCK_INIT */

#if PLATFORM_HIB_ENABLE

static uint32_t platform_hibernation_ext_clock_freq;

#if PLATFORM_HIBERNATE_CLOCK_AS_EXT_LPO
/*
 * HIB internal clock RC codes in monotonically
 * increasing order of LPO output frequencies.
 */
static uint8_t  hib_clock_internal_rc_codes_mono[HIB_CLOCK_INTERNAL_RC_CODES_COUNT] = { 19, 18, 17, 16, 23, 22, 21, 20, 27, 26, 25, 24, 31, 30, 29, 28, 3, 2, 1, 0, 7, 6, 5, 4, 11, 10, 9, 8, 15, 14, 13, 12 };

static uint32_t hib_clock_measure_error( uint32_t hib_clock_set )
{
    uint32_t hib_clock_val;
    uint32_t hib_clock_err_val;

    HIB_CLOCK_INTERNAL_FREQ_MEASURE;
    hib_clock_val = osl_ilp_clock();
    hib_clock_err_val = HIB_CLOCK_INTERNAL_ERROR_VALUE( hib_clock_val, hib_clock_set );

    return hib_clock_err_val;
}

/*
 * Calibration of HIB internal clock by calculating the
 * RC code using a quadratic polynomial derived from
 * statistical curve fitting of observed data points.
 */
static platform_result_t hib_clock_calibration_curve_fitting( uint32_t hib_clock_set )
{
    double a, b, c, d, o;
    int x0, x1;
    uint32_t rc_code_mid;
    uint32_t rc_code_set;
    uint32_t hib_clock_val;

    if ( hib_clock_set != PLATFORM_HIBERNATE_CLOCK_INTERNAL_FREQ_32KHZ )
    {
        return PLATFORM_UNSUPPORTED;
    }

    /* The RC code corresponding to the median frequency */
    rc_code_mid = (uint32_t)hib_clock_internal_rc_codes_mono[(HIB_CLOCK_INTERNAL_RC_CODES_COUNT / 2)];

    HIB_CLOCK_INTERNAL_RC_CODE_INIT( rc_code_mid );
    HIB_CLOCK_INTERNAL_FREQ_MEASURE;
    hib_clock_val = osl_ilp_clock();

    /* Coefficients derived from curve-fitting a board population */
    a = 0.0106376;
    b = 0.607115;

    /* Coefficient for adjusting the curve-fit to the current board */
    c = -((double)((int)hib_clock_set - (int)hib_clock_val) / 1000);
    d = sqrt((b * b) - (4 * a * c));

    /* Offset to adjust the curve-fit into the valid RC code range */
    o = 16.0;

    /* Perform a quadratic curve-fit using the computed coefficients */
    x0 = (int)(round((-b + d) / (2 * a)) + o);
    x1 = (int)(round((-b - d) / (2 * a)) + o);

    if ((!HIB_CLOCK_INTERNAL_RC_CODE_VALID(x0)) && (!HIB_CLOCK_INTERNAL_RC_CODE_VALID(x1)))
    {
        return PLATFORM_ERROR;
    }

    if ((HIB_CLOCK_INTERNAL_RC_CODE_VALID(x0)) && (HIB_CLOCK_INTERNAL_RC_CODE_VALID(x1)))
    {
        return PLATFORM_ERROR;
    }

    rc_code_set = (uint32_t)hib_clock_internal_rc_codes_mono[(HIB_CLOCK_INTERNAL_RC_CODE_VALID(x0) ? x0 : x1)];

    HIB_CLOCK_INTERNAL_RC_CODE_INIT( rc_code_set );
    HIB_CLOCK_INTERNAL_FREQ_MEASURE;
    hib_clock_val = osl_ilp_clock();

    if ( HIB_CLOCK_INTERNAL_FREQ_VALID( hib_clock_val, hib_clock_set ) )
    {
        return PLATFORM_SUCCESS;
    }
    else
    {
        return PLATFORM_ERROR;
    }
}

/*
 * Calibration of HIB internal clock by calculating
 * the RC code by running feedback-based binary
 * optimization over the entire set of RC codes.
 */
static platform_result_t hib_clock_calibration_binary_scanning(uint32_t hib_clock_set)
{
    platform_result_t ret;
    int rc_code_idx_low = 0;
    int rc_code_idx_mid = 0;
    int rc_code_idx_high = 0;
    int rc_code_idx_set = 0;
    uint32_t hib_clock_val;
    uint32_t hib_clock_err_val = 0xFFFFFFFF;
    uint32_t hib_clock_err_min = 0xFFFFFFFF;

    rc_code_idx_low = 0;
    rc_code_idx_high = HIB_CLOCK_INTERNAL_RC_CODES_COUNT - 1;

    /* Perform a binary scan to determine the RC code with the optimal (closest) frequency */
    while ( rc_code_idx_low <= rc_code_idx_high )
    {
        rc_code_idx_mid = ( rc_code_idx_low + rc_code_idx_high ) / 2;

        HIB_CLOCK_INTERNAL_RC_CODE_INIT( (uint32_t)hib_clock_internal_rc_codes_mono[rc_code_idx_mid] );
        hib_clock_err_val = hib_clock_measure_error( hib_clock_set );
        hib_clock_val = osl_ilp_clock();

        if ( hib_clock_err_val < hib_clock_err_min )
        {
            rc_code_idx_set = rc_code_idx_mid;
            hib_clock_err_min = hib_clock_err_val;
        }

        if ( hib_clock_set == hib_clock_val )
        {
            break;
        }
        else if ( hib_clock_set < hib_clock_val )
        {
            rc_code_idx_high = rc_code_idx_mid - 1;
        }
        else
        {
            rc_code_idx_low = rc_code_idx_mid + 1;
        }
    }

    HIB_CLOCK_INTERNAL_RC_CODE_INIT( (uint32_t)hib_clock_internal_rc_codes_mono[rc_code_idx_set] );
    HIB_CLOCK_INTERNAL_FREQ_MEASURE;
    hib_clock_val = osl_ilp_clock();

    if ( HIB_CLOCK_INTERNAL_FREQ_VALID( hib_clock_val, hib_clock_set ) )
    {
        ret = PLATFORM_SUCCESS;
    }
    else
    {
        /* Check neighbor RC codes for existence of a sub-optimal frequency in the valid range */
        if ( hib_clock_val > HIB_CLOCK_INTERNAL_FREQ_THRESHOLD_HIGH(hib_clock_set) )
        {
            HIB_CLOCK_INTERNAL_RC_CODE_INIT( (uint32_t)hib_clock_internal_rc_codes_mono[rc_code_idx_set - 1] );
        }
        else
        {
            HIB_CLOCK_INTERNAL_RC_CODE_INIT( (uint32_t)hib_clock_internal_rc_codes_mono[rc_code_idx_set + 1] );
        }

        HIB_CLOCK_INTERNAL_FREQ_MEASURE;
        hib_clock_val = osl_ilp_clock();

        if ( HIB_CLOCK_INTERNAL_FREQ_VALID( hib_clock_val, hib_clock_set ) )
        {
            ret = PLATFORM_SUCCESS;
        }
        else
        {
            HIB_CLOCK_INTERNAL_RC_CODE_INIT( (uint32_t)hib_clock_internal_rc_codes_mono[rc_code_idx_set] );
            HIB_CLOCK_INTERNAL_FREQ_MEASURE;

            ret = PLATFORM_ERROR;
        }
    }

    return ret;
}
#endif /* PLATFORM_HIBERNATE_CLOCK_AS_EXT_LPO */

platform_result_t platform_hibernation_init( const platform_hibernation_t* hib )
{
    return platform_hibernation_clock_init( hib, &platform_hibernation_ext_clock_freq );
}

platform_result_t platform_hibernation_start( uint32_t ticks_to_wakeup )
{
    platform_gci_chipcontrol( GCI_CHIPCONTROL_HIB_WAKE_COUNT_REG,
                              GCI_CHIPCONTROL_HIB_WAKE_COUNT_MASK,
                              GCI_CHIPCONTROL_HIB_WAKE_COUNT_VAL( ticks_to_wakeup ) );

    platform_gci_chipcontrol( GCI_CHIPCONTROL_HIB_START_REG,
                              GCI_CHIPCONTROL_HIB_START_MASK,
                              GCI_CHIPCONTROL_HIB_START_EXEC );
    /*
     * Hibernation mode essentially power off all components, except special hibernation block.
     * This block wakes up whole system when its timer expired.
     * Returning from hibernation mode is same as powering up, and whole boot sequence is involved.
     * So if we continue here it means system not entered hibernation mode, and so return error.
     */
    return PLATFORM_ERROR;
}

wiced_bool_t platform_hibernation_is_returned_from( void )
{
    return !platform_boot_is_reset( ) && ( platform_hibernation_get_status( ).bits.boot_from_wake != 0 );
}

uint32_t platform_hibernation_get_ticks_spent( void )
{
    return (platform_hibernation_get_raw_status( GCI_CHIPCONTROL_HIB_READ_SEL_COUNT_0_7   ) << 0  ) |
           (platform_hibernation_get_raw_status( GCI_CHIPCONTROL_HIB_READ_SEL_COUNT_15_8  ) << 8  ) |
           (platform_hibernation_get_raw_status( GCI_CHIPCONTROL_HIB_READ_SEL_COUNT_23_16 ) << 16 ) |
           (platform_hibernation_get_raw_status( GCI_CHIPCONTROL_HIB_READ_SEL_COUNT_31_24 ) << 24 );
}

uint32_t platform_hibernation_get_clock_freq( void )
{
#if PLATFORM_HIBERNATE_CLOCK_AS_EXT_LPO

    return platform_reference_clock_get_freq( PLATFORM_REFERENCE_CLOCK_ILP );

#else

    hib_status_t status = platform_hibernation_get_status( );

    if ( status.bits.clk_sel_out_int_lpo )
    {
        uint32_t freq_code = platform_gci_chipcontrol( GCI_CHIPCONTROL_HIB_WAKE_CTL_REG, 0x0, 0x0 ) & GCI_CHIPCONTROL_HIB_WAKE_CTL_LPO_FREQ_MASK;

        switch ( freq_code )
        {
            case GCI_CHIPCONTROL_HIB_WAKE_CTL_LPO_FREQ_128KHZ:
                return PLATFORM_HIBERNATE_CLOCK_INTERNAL_FREQ_128KHZ;

            case GCI_CHIPCONTROL_HIB_WAKE_CTL_LPO_FREQ_32KHZ:
                return PLATFORM_HIBERNATE_CLOCK_INTERNAL_FREQ_32KHZ;

            case GCI_CHIPCONTROL_HIB_WAKE_CTL_LPO_FREQ_16KHZ:
                return PLATFORM_HIBERNATE_CLOCK_INTERNAL_FREQ_16KHZ;

            default:
                wiced_assert( "unknown code", 0 );
                return 0;
        }
    }
    else if ( status.bits.clk_sel_out_ext_lpo )
    {
        return platform_hibernation_ext_clock_freq;
    }
    else
    {
        wiced_assert( "unknown clock", 0 );
        return 0;
    }

#endif /* PLATFORM_HIBERNATE_CLOCK_AS_EXT_LPO */
}

uint32_t platform_hibernation_get_max_ticks( void )
{
    return ( GCI_CHIPCONTROL_HIB_WAKE_COUNT_MASK >> GCI_CHIPCONTROL_HIB_WAKE_COUNT_SHIFT );
}

/*
 * Calibrates the imprecise internal HIB clock by selecting the optimal RC code that
 * compensates for the clock drift error. Assumes the CPU clock has been previously
 * initialized. Assumes the precise external HIB clock does not require any calibration.
 */
platform_result_t platform_hibernation_calibrate_clock( const platform_hibernation_t* hib )
{
#if PLATFORM_HIBERNATE_CLOCK_AS_EXT_LPO

    uint32_t hib_clock_set;
    uint32_t hib_clock_val;

    if ( hib == NULL )
    {
        return PLATFORM_UNSUPPORTED;
    }

    if ( hib->clock != PLATFORM_HIBERNATION_CLOCK_INTERNAL_32KHZ )
    {
        return PLATFORM_UNSUPPORTED;
    }

    hib_clock_set = PLATFORM_HIBERNATE_CLOCK_INTERNAL_FREQ_32KHZ;

    HIB_CLOCK_INTERNAL_FREQ_MEASURE;
    hib_clock_val = osl_ilp_clock();

    /* Check if internal HIB clock is within error threshold */
    if ( HIB_CLOCK_INTERNAL_FREQ_VALID( hib_clock_val, hib_clock_set ) )
    {
        return PLATFORM_SUCCESS;
    }

    /* Calibrate the internal HIB clock using curve-fitting */
    if ( hib_clock_calibration_curve_fitting( hib_clock_set ) == PLATFORM_SUCCESS )
    {
        return PLATFORM_SUCCESS;
    }

    /* Calibrate the internal HIB clock using binary-scanning */
    if ( hib_clock_calibration_binary_scanning( hib_clock_set ) == PLATFORM_SUCCESS )
    {
        return PLATFORM_SUCCESS;
    }

    return PLATFORM_ERROR;

#else

    UNUSED_PARAMETER( hib );
    return PLATFORM_FEATURE_DISABLED;

#endif /* PLATFORM_HIBERNATE_CLOCK_AS_EXT_LPO */
}

#else

platform_result_t platform_hibernation_init( const platform_hibernation_t* hib )
{
    platform_result_t result = platform_hibernation_clock_init( hib, NULL );
    return ( result == PLATFORM_SUCCESS ) ? PLATFORM_FEATURE_DISABLED : result;
}

platform_result_t platform_hibernation_start( uint32_t ticks_to_wakeup )
{
    UNUSED_PARAMETER( ticks_to_wakeup );
    return PLATFORM_FEATURE_DISABLED;
}

wiced_bool_t platform_hibernation_is_returned_from( void )
{
    return WICED_FALSE;
}

uint32_t platform_hibernation_get_ticks_spent( void )
{
    return 0;
}

uint32_t platform_hibernation_get_clock_freq( void )
{
    return 0;
}

uint32_t platform_hibernation_get_max_ticks( void )
{
    return 0;
}

platform_result_t platform_hibernation_calibrate_clock( const platform_hibernation_t* hib )
{
    UNUSED_PARAMETER( hib );
    return PLATFORM_FEATURE_DISABLED;
}

#endif /* PLATFORM_HIB_ENABLE */
