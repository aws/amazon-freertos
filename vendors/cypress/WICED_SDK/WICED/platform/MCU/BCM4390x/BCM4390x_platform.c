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
#include "platform_config.h"
#include "platform_init.h"
#include "platform_m2m.h"
#include "platform_toolchain.h"
#include "platform/wwd_platform_interface.h"

#include "typedefs.h"

#include "wiced_platform.h"
#include "wiced_framework.h"
#include "wiced_low_power.h"
#include "wiced_osl.h"

#include "sbpcmcia.h"

#ifdef MODUSTOOLBOX
#include "generated_mac_address.txt"
#else
#include "../generated_mac_address.txt"
#endif

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
 *               Variables Declarations
 ******************************************************/

extern platform_hibernation_t hibernation_config;

/******************************************************
 *               Variables Definitions
 ******************************************************/

#if PLATFORM_DDR_HEAP_SIZE_CONFIG
#if PLATFORM_NO_DDR
#error "Misconfiguration: to use external heap DDR has to be enabled"
#endif
uint8_t PLATFORM_DDR_HEAP_SECTION( ddr_heap_array )[ PLATFORM_DDR_HEAP_SIZE_CONFIG ];
#endif

static wiced_bool_t platform_inited = WICED_FALSE;

uint32_t platform_capabilities_word = 0;

/*
 * BCM4390x Platform Capabilities Table:
 *
 * Part      PkgOpt[3]   PkgOpt[2:0]   sdio   gmac   usb   hsic   i2s   i2c   uart   ddr   spi   jtag
 * 43909     0           0,1           x      x      x     x      x     x     x      x     x     x
 * 43907     0           2             x      x      x            x     x     x            x     x
 * 43906     0           3             x                          x     x     x            x     x
 * 43905     0           4             x                                x     x            x     x
 * 43904     0           5                           x     x            x     x            x     x
 * 43903,01  0           6                                              x     x            x     x
 *
 * 43909     1           0,1                  x                   x     x     x      x     x
 * 43907     1           2                    x                   x     x     x            x
 * 43906     1           3                                        x     x     x            x
 * 43905     1           4                                              x     x            x
 * 43904     1           5                                              x     x            x
 * 43903,01  1           6                                              x     x            x
 */
static uint32_t platform_capabilities_table[] =
{
    /* 43909 */
    [0]  = (PLATFORM_CAPS_COMMON | PLATFORM_CAPS_SDIO | PLATFORM_CAPS_GMAC | PLATFORM_CAPS_USB |
            PLATFORM_CAPS_HSIC   | PLATFORM_CAPS_I2S  | PLATFORM_CAPS_DDR  | PLATFORM_CAPS_JTAG),
    /* 43909 */
    [1]  = (PLATFORM_CAPS_COMMON | PLATFORM_CAPS_SDIO | PLATFORM_CAPS_GMAC | PLATFORM_CAPS_USB |
            PLATFORM_CAPS_HSIC   | PLATFORM_CAPS_I2S  | PLATFORM_CAPS_DDR  | PLATFORM_CAPS_JTAG),
    /* 43907 */
    [2]  = (PLATFORM_CAPS_COMMON | PLATFORM_CAPS_SDIO | PLATFORM_CAPS_GMAC | PLATFORM_CAPS_USB |
            PLATFORM_CAPS_I2S    | PLATFORM_CAPS_JTAG),
    /* 43906 */
    [3]  = (PLATFORM_CAPS_COMMON | PLATFORM_CAPS_SDIO | PLATFORM_CAPS_I2S  | PLATFORM_CAPS_JTAG),
    /* 43905 */
    [4]  = (PLATFORM_CAPS_COMMON | PLATFORM_CAPS_SDIO | PLATFORM_CAPS_JTAG),
    /* 43904 */
    [5]  = (PLATFORM_CAPS_COMMON | PLATFORM_CAPS_USB  | PLATFORM_CAPS_HSIC | PLATFORM_CAPS_JTAG),
    /* 43903,01 */
    [6]  = (PLATFORM_CAPS_COMMON | PLATFORM_CAPS_JTAG),
    [7]  = (0),
    /* 43909 Secure */
    [8]  = (PLATFORM_CAPS_COMMON | PLATFORM_CAPS_GMAC | PLATFORM_CAPS_I2S  | PLATFORM_CAPS_DDR),
    /* 43909 Secure */
    [9]  = (PLATFORM_CAPS_COMMON | PLATFORM_CAPS_GMAC | PLATFORM_CAPS_I2S  | PLATFORM_CAPS_DDR),
    /* 43907 Secure */
    [10] = (PLATFORM_CAPS_COMMON | PLATFORM_CAPS_GMAC | PLATFORM_CAPS_I2S),
    /* 43906 Secure */
    [11] = (PLATFORM_CAPS_COMMON | PLATFORM_CAPS_I2S),
    /* 43905 Secure */
    [12] = (PLATFORM_CAPS_COMMON),
    /* 43904 Secure */
    [13] = (PLATFORM_CAPS_COMMON),
    /* 43903,01 Secure */
    [14] = (PLATFORM_CAPS_COMMON),
    [15] = (0)
};

static appscr4_core_status_reg_t appscr4_saved_core_status;

uint32_t platform_ddr_size;

/******************************************************
 *               Function Definitions
 ******************************************************/

static void platform_capabilities_init( void )
{
    uint32_t package_options = 0;
    platform_result_t result;

    /* Extract the platform package options */
    result = platform_otp_package_options( &package_options );
    if ( result != PLATFORM_SUCCESS )
    {
        wiced_assert( "Platform Capabilities Initialization Error", 0 );
    }

    /* Initialize the platform capabilities */
    platform_capabilities_word = platform_capabilities_table[package_options];
}

static void platform_early_misc_init( void )
{
    /* Fixup max resource mask register. */
    platform_common_chipcontrol( &PLATFORM_PMU->max_res_mask,
                                 PMU_RES_MAX_CLEAR_MASK,
                                 PMU_RES_MAX_SET_MASK );

    /*
     * Boot strap pins may force ILP be always requested. This prevents deep sleep from working properly.
     * Current register modification clears this request.
     * Boot strap pin issue was observed with FTDI chip which shared strap pin and pulled its up during board reset.
     */
    platform_common_chipcontrol( &PLATFORM_PMU->pmucontrol_ext,
                                 PMU_CONTROL_EXT_ILP_ON_BP_DEBUG_MODE_MASK,
                                 0x0 );

    /*
     * Make sure that board is waking up after reset pin reboot triggered.
     * Software drops min_res_mask to low value for deep sleep, need to restore mask during board resetting.
     * Below statement forces chip to restore min_res_mask to default value when reset pin reboot triggered.
     */
    platform_common_chipcontrol( &PLATFORM_PMU->pmucontrol,
                                 PMU_CONTROL_RESETCONTROL_MASK,
                                 PMU_CONTROL_RESETCONTROL_RESTORE_RES_MASKS );
}

static void platform_slow_clock_init( void )
{
    pmu_slowclkperiod_t period     = { .raw = 0 };
    uint32_t            clock      = platform_reference_clock_get_freq( PLATFORM_REFERENCE_CLOCK_ALP );
    unsigned long       alp_period = ( ( 16UL * 1000000UL ) / ( clock / 1024UL ) ); /* 1 usec period of ALP clock measured in 1/16384 usec of ALP clock */

    period.bits.one_mhz_toggle_en = 1;
    period.bits.alp_period        = alp_period & PMU_SLOWCLKPERIOD_ALP_PERIOD_MASK;

    PLATFORM_PMU->slowclkperiod.raw = period.raw;
}

static void platform_boot_status_init( void )
{
    /*
     * Save and clear core status.
     * Warm-boot especially require that bits which indicate reset happened to be cleared.
     */
    appscr4_saved_core_status.raw     = PLATFORM_APPSCR4->core_status.raw;
#ifndef BOOTLOADER
    PLATFORM_APPSCR4->core_status.raw = appscr4_saved_core_status.raw;
#endif
}

void platform_init_system_clocks( void )
{
    wiced_bool_t result;

    platform_watchdog_init( );

    platform_boot_status_init( );

    platform_mcu_powersave_warmboot_init( );

    platform_early_misc_init( );

    platform_backplane_init( );

    platform_hibernation_init( &hibernation_config );

    /*
     * It is important to make sure that the below
     * functions get called in this specific order.
     * (1) platform_cpu_clock_init()
     * (2) platform_hibernation_calibrate_clock()
     * (3) platform_tick_timer_init()
     *
     * (2) has a dependency on (1), and (3)
     * makes sure the tick timer gets updated
     * after the HIB clock is calibrated in (2).
     */
    result = platform_cpu_clock_init( PLATFORM_CPU_CLOCK_FREQUENCY );
    wiced_assert( "must always succeed", result == WICED_TRUE );

    platform_slow_clock_init( );

    /* HIB clock calibration for imprecise internal oscillator */
    platform_hibernation_calibrate_clock( &hibernation_config );

    /* Re-initialize tick timer after HIB clock calibration */
    platform_tick_timer_init();
}

/* Weak attribute is intentional in case a specific platform (board) needs to override this */
WEAK void platform_init_memory( void )
{
}

void platform_init_mcu_infrastructure( void )
{
    /* Initialize platform capabilities */
    platform_capabilities_init( );

    /* Initialize MCU powersave */
    platform_mcu_powersave_init( );

    /* Initialize cores powersave */
    platform_cores_powersave_init( );

    /* Initialize interrupts */
    platform_irq_init( );

    /* Initialise external serial flash */
    platform_sflash_init( );

    /* Initialize deep-sleep support */
    platform_deep_sleep_init( );
}

void platform_init_complete( void )
{
#if !PLATFORM_NO_DDR
    wiced_assert( "Something wrong with DDR BSS", platform_ddr_get_size( ) >= PLATFORM_DDR_BSS_SIZE );
#endif

    /* Platform initialization done */
    platform_inited = WICED_TRUE;

    /* Notify if returned from deep-sleep */
    WICED_SLEEP_CALL_EVENT_HANDLERS( WICED_DEEP_SLEEP_IS_WARMBOOT( ), WICED_SLEEP_EVENT_LEAVE );
}

wiced_bool_t platform_is_init_completed( void )
{
    return platform_inited;
}

wiced_bool_t platform_boot_is_reset( void )
{
    if ( appscr4_saved_core_status.bits.s_error_log || appscr4_saved_core_status.bits.s_bp_reset_log )
    {
        return WICED_TRUE;
    }

    return WICED_FALSE;
}

uint32_t platform_ddr_get_size( void )
{
    return platform_ddr_size;
}

void platform_udelay( uint32_t usec )
{
    OSL_DELAY( usec );
}

/******************************************************
 *                 DCT Functions
 ******************************************************/

wwd_result_t host_platform_get_mac_address( wiced_mac_t* mac )
{
#ifndef WICED_DISABLE_BOOTLOADER
    wiced_mac_t* temp_mac;
    wiced_result_t result;
    result = wiced_dct_read_lock( (void**)&temp_mac, WICED_FALSE, DCT_WIFI_CONFIG_SECTION, OFFSETOF(platform_dct_wifi_config_t, mac_address), sizeof(mac->octet) );
    if ( result != WICED_SUCCESS )
    {
        return (wwd_result_t) result;
    }
    memcpy( mac->octet, temp_mac, sizeof(mac->octet) );
    wiced_dct_read_unlock( temp_mac, WICED_FALSE );
#else
    UNUSED_PARAMETER( mac );
#endif
    return WWD_SUCCESS;
}

#ifdef WICED_USE_ETHERNET_INTERFACE
wwd_result_t host_platform_get_ethernet_mac_address( wiced_mac_t* mac )
{
    uint16_t mac_size;
    wiced_mac_t* temp_mac;
    platform_result_t result;
    wiced_result_t wiced_result;

    mac_size = sizeof(mac->octet);
    result = platform_otp_read_tag( PLATFORM_OTP_HW_RGN, HNBU_MACADDR, mac->octet, &mac_size );
    if ( result == PLATFORM_SUCCESS )
    {
        if ( mac_size == sizeof(mac->octet) )
        {
            mac->octet[mac_size - 1]++;
            return WWD_SUCCESS;
        }
        wiced_assert( "OTP has bad MAC address", 0 );
    }
    wiced_assert( "OTP parsing failed", result != PLATFORM_PARTIAL_RESULTS );

#ifdef WICED_DISABLE_BOOTLOADER
    UNUSED_VARIABLE( temp_mac );
    UNUSED_VARIABLE( wiced_result );
    memcpy( mac->octet, DCT_GENERATED_ETHERNET_MAC_ADDRESS, sizeof(mac->octet) );
#else
    wiced_result = wiced_dct_read_lock( (void**) &temp_mac, WICED_FALSE, DCT_ETHERNET_CONFIG_SECTION, OFFSETOF( platform_dct_ethernet_config_t, mac_address ), sizeof(mac->octet) );
    if ( wiced_result != WICED_SUCCESS )
    {
        return (wwd_result_t) wiced_result;
    }
    memcpy( mac->octet, temp_mac, sizeof(mac->octet) );
    wiced_dct_read_unlock( temp_mac, WICED_FALSE );
#endif

    return WWD_SUCCESS;
}
#endif /* WICED_USE_ETHERNET_INTERFACE */

/******************************************************
 *            Interrupt Service Routines
 ******************************************************/
