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
 *  Platform cores power related code
 */
#include "platform_toolchain.h"
#include "platform_mcu_peripheral.h"
#include "platform_appscr4.h"
#include "platform_map.h"

#include "typedefs.h"
#include "sbchipc.h"
#include "aidmp.h"
#include "hndsoc.h"

#include "wiced_osl.h"
#include "wiced_low_power.h"

/******************************************************
 *                      Macros
 ******************************************************/

#define PLATFORM_CORES_POWERSAVE_WLAN_RESOURCES          \
    ( PMU_RES_MASK( PMU_RES_SR_CLK_START )             | \
      PMU_RES_MASK( PMU_RES_WL_CORE_READY )            | \
      PMU_RES_MASK( PMU_RES_WL_CORE_READY_BUF )        | \
      PMU_RES_MASK( PMU_RES_MINI_PMU )                 | \
      PMU_RES_MASK( PMU_RES_RADIO_PU )                 | \
      PMU_RES_MASK( PMU_RES_SR_CLK_STABLE )            | \
      PMU_RES_MASK( PMU_RES_SR_SAVE_RESTORE )          | \
      PMU_RES_MASK( PMU_RES_SR_VDDM_PWRSW )            | \
      PMU_RES_MASK( PMU_RES_SR_SUBCORE_AND_PHY_PWRSW ) | \
      PMU_RES_MASK( PMU_RES_SR_SLEEP )                 | \
      PMU_RES_MASK( PMU_RES_MAC_PHY_CLK_AVAIL ) )

#define PLATFORM_CORES_POWERSAVE_ROM_BOOTLOADER_DATA_BEGIN     0x554000
#define PLATFORM_CORES_POWERSAVE_ROM_BOOTLOADER_DATA_END       0x560000

#define PLATFORM_CORES_POWERSAVE_ROM_BOOTLOADER_STACK_BEGIN    0x660000
#define PLATFORM_CORES_POWERSAVE_ROM_BOOTLOADER_STACK_END      0x6A0000

#define PLATFORM_CORES_POWERSAVE_BOOTLOADER_BEGIN              0x660000
#define PLATFORM_CORES_POWERSAVE_BOOTLOADER_END                0x6A0000

/******************************************************
 *                    Constants
 ******************************************************/

/******************************************************
 *                   Enumerations
 ******************************************************/

/******************************************************
 *                 Type Definitions
 ******************************************************/

typedef struct
{
    uint32_t        core_addr;
    uint32_t        wrapper_addr;
    wiced_bool_t    is_enabled;
} platform_cores_powersave_clock_gate_core_t;

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

#if PLATFORM_CORES_POWERSAVE

static void
platform_cores_powersave_clock_gate_core( platform_cores_powersave_clock_gate_core_t* core )
{
    /*
     * The clock gating only works when the core is out of reset.
     * But when the core is out of reset it always requests HT.
     * Let's bring core out of reset (so that clock gating works and the
     * clock tree within the core is not running) but also write to ClockControlStatus
     * to force off the HT request.
     * This reduce power consumption as long as core is not used.
     * When driver is going to use core it resets core and cancel changes done here.
     */

    volatile aidmp_t* dmp                        = (aidmp_t*)core->wrapper_addr;
    volatile clock_control_status_t* core_status = PLATFORM_CLOCKSTATUS_REG( core->core_addr );
    uint32_t dummy;

    UNUSED_VARIABLE( dummy );

    dmp->ioctrl    = SICF_FGC | SICF_CLOCK_EN; /* turn on clocks */
    dmp->resetctrl = AIRC_RESET;               /* turn on reset */
    dummy = dmp->resetctrl;                    /* read back to ensure write propagated */
    OSL_DELAY( 1 );

    dmp->resetctrl = 0;                        /* turn off reset */
    dummy = dmp->resetctrl;                    /* read back to ensure write propagated */
    OSL_DELAY( 1 );

    core_status->bits.force_hw_clock_req_off = 1; /* assert ForceHWClockReqOff */
    dummy = core_status->raw;                     /* read back to ensure write propagated */
    OSL_DELAY( 1 );

    dmp->ioctrl = SICF_CLOCK_EN; /* turn off force clock */
    dmp->ioctrl = 0;             /* turn off clock */
}

#ifdef WICED_NO_WIFI

static void
platform_cores_powersave_down_core( uint32_t wrapper_addr )
{
    volatile aidmp_t* dmp = (aidmp_t*)wrapper_addr;
    uint32_t dummy;

    UNUSED_VARIABLE( dummy );

    dmp->resetctrl = AIRC_RESET; /* turn on reset */
    dmp->ioctrl    = 0;          /* turn off clocks */

    dummy = dmp->resetctrl;      /* read back to ensure write propagated */
    OSL_DELAY( 1 );
}

static void
platform_cores_powersave_down_all_wlan_cores( void )
{
    uint32_t wlan_domain_wrappers[] =
    {
        PLATFORM_WLANCR4_MASTER_WRAPPER_REGBASE(0x0),
        PLATFORM_DOT11MAC_MASTER_WRAPPER_REGBASE(0x0)
    };
    unsigned i;

    /*
     * Pull up wlan resource before accessing to WLAN domain
     */
    PLATFORM_PMU->min_res_mask = PLATFORM_CORES_POWERSAVE_WLAN_RESOURCES;
    for ( i = 0; i < ARRAYSIZE( wlan_domain_wrappers ); ++i )
    {
        platform_cores_powersave_down_core( wlan_domain_wrappers[i] );
    }
}

static void
platform_cores_powersave_init_wlan_domain( void )
{
    /* No WiFi needed. Let's down whole domain. */

    if ( !WICED_DEEP_SLEEP_IS_WARMBOOT( ) )
    {
        platform_cores_powersave_down_all_wlan_cores( );
    }

    PLATFORM_PMU->min_res_mask &= ~PLATFORM_CORES_POWERSAVE_WLAN_RESOURCES;
    PLATFORM_PMU->max_res_mask &= ~PLATFORM_CORES_POWERSAVE_WLAN_RESOURCES;
}

#else

static void
platform_cores_powersave_init_wlan_domain( void )
{
}

#endif /* WICED_NO_WIFI */

#if !PLATFORM_NO_SOCSRAM_POWERDOWN

static uint32_t powersave_control_memory_begin = 0;
static uint32_t powersave_control_memory_end   = 0;

static wiced_bool_t
platform_cores_powersave_is_powerdown_permitted( uint32_t begin, uint32_t end )
{
    /*
     * ROM bootloader uses some SOCSRAM memory.
     * It cannot be powered down because reset is not restoring memory power state (not power up).
     */

    static const uint32_t bad_ranges[][2] =
    {
        { PLATFORM_CORES_POWERSAVE_ROM_BOOTLOADER_DATA_BEGIN,  PLATFORM_CORES_POWERSAVE_ROM_BOOTLOADER_DATA_END },
        { PLATFORM_CORES_POWERSAVE_ROM_BOOTLOADER_STACK_BEGIN, PLATFORM_CORES_POWERSAVE_ROM_BOOTLOADER_STACK_END },
        { PLATFORM_CORES_POWERSAVE_BOOTLOADER_BEGIN,           PLATFORM_CORES_POWERSAVE_BOOTLOADER_END }
    };
    unsigned i;

    for ( i = 0; i < ARRAYSIZE(bad_ranges); i++ )
    {
        if ( ( begin >= bad_ranges[i][0] ) && ( begin < bad_ranges[i][1] ) )
        {
            return WICED_FALSE;
        }
        if ( ( end > bad_ranges[i][0] ) && ( end <= bad_ranges[i][1] ) )
        {
            return WICED_FALSE;
        }
    }

    return WICED_TRUE;
}

static void
platform_cores_powersave_control_memory_power( uint32_t begin, uint32_t end, wiced_bool_t powerdown )
{
    uint32_t sram_addr = PLATFORM_SOCSRAM_CH0_RAM_BASE(0x0);
    unsigned i;

    wiced_assert( "bad range", end >= begin );
    if ( begin >= end )
    {
        return;
    }

    for ( i = 0; i < SOCSRAM_BANK_NUMBER; i++ )
    {
        uint32_t bank_size;

        if ( sram_addr >= end )
        {
            break;
        }

        PLATFORM_SOCSRAM->bank_x_index.bits.bank_number = i;

        bank_size = SOCSRAM_BANK_SIZE;

        if ( sram_addr >= begin )
        {
            uint32_t pda = PLATFORM_SOCSRAM->bank_x_pda.bits.pda & SOCSRAM_BANK_PDA_MASK;

            if ( powerdown )
            {
                if ( ( sram_addr + bank_size <= end ) && ( pda != SOCSRAM_BANK_PDA_MASK ) && platform_cores_powersave_is_powerdown_permitted( sram_addr, sram_addr + bank_size ) )
                {
                    PLATFORM_SOCSRAM->bank_x_pda.bits.pda = SOCSRAM_BANK_PDA_MASK;
                }
            }
            else
            {
                unsigned j;

                for ( j = 0; ( pda != 0 ) && ( j < 32 ); j++ )
                {
                    uint32_t new_pda = pda & ~( 1 << j );

                    if ( new_pda != pda )
                    {
                        PLATFORM_SOCSRAM->bank_x_pda.bits.pda = new_pda;
                        pda = PLATFORM_SOCSRAM->bank_x_pda.bits.pda; /* read it back to ensure write completed */

                        /*
                         * From ASIC: wait time of 10ns between each PDA switch turn-on (a PDA bit going 1 to 0)
                         *            is adequate to limit inrush current accumulation due to multiple switches turning on.
                         * We have no delay function with nanosecond resolution, but it can be created.
                         * 10ns is just few CPU cycles delay and we can just add small delay loop here.
                         * Maybe this is too cautious, but I use existed function and 1us delay, which should be more then enough.
                         */
                        OSL_DELAY(1);
                    }
                }
            }
        }

        sram_addr += bank_size;
    }
}

void
platform_toolchain_sbrk_prepare( void* ptr, int incr )
{
    if ( incr != 0 )
    {
        uint32_t begin = (uint32_t)( ptr );
        uint32_t end   = (uint32_t)( ptr + incr );

        uint32_t     powersave_begin;
        uint32_t     powersave_end;
        wiced_bool_t powerdown;

        if ( end > begin )
        {
            powersave_begin = powersave_control_memory_begin;
            powersave_end   = end;
            powerdown       = WICED_FALSE;
        }
        else
        {
            powersave_begin = end;
            powersave_end   = powersave_control_memory_end;
            powerdown       = WICED_TRUE;
        }

        if ( powersave_begin < powersave_end )
        {
            platform_cores_powersave_control_memory_power( powersave_begin, powersave_end, powerdown );
        }
    }
}

static void
platform_cores_powersave_powerdown_memory( void )
{
    extern unsigned char _heap[];
    extern unsigned char _eheap[];

    powersave_control_memory_begin = (uint32_t)&_heap[ 0 ];
    powersave_control_memory_end   = (uint32_t)&_eheap[ 0 ];

    wiced_assert( "bad configuration", powersave_control_memory_end >= powersave_control_memory_begin );

    platform_cores_powersave_control_memory_power( powersave_control_memory_begin, powersave_control_memory_end, WICED_TRUE );
}

static void
platform_cores_powersave_powerup_memory( void )
{
    if ( powersave_control_memory_end > powersave_control_memory_begin )
    {
        platform_cores_powersave_control_memory_power( powersave_control_memory_begin, powersave_control_memory_end, WICED_FALSE );
    }
}

#else

static void
platform_cores_powersave_powerdown_memory( void )
{
}

static void
platform_cores_powersave_powerup_memory( void )
{
}

#endif /* !PLATFORM_NO_SOCSRAM_POWERDOWN */

static void
platform_cores_powersave_shutoff_usb20d( void )
{
    /*
     * Shutoff USB PHY using USB20D core.
     * Need to do it once only during cold boot.
     */

    if ( WICED_DEEP_SLEEP_IS_WARMBOOT( ) || !PLATFORM_CAPABILITY_ENAB( PLATFORM_CAPS_USB  ) )
    {
        return;
    }

    platform_gci_chipcontrol( GCI_CHIPCONTROL_USBPHY_MODE_OVR_VAL_REG, GCI_CHIPCONTROL_USBPHY_MODE_OVR_VAL_MASK, GCI_CHIPCONTROL_USBPHY_MODE_OVR_VAL_USB );

    platform_gci_chipcontrol( GCI_CHIPCONTROL_USBPHY_MODE_OVR_EN_REG, GCI_CHIPCONTROL_USBPHY_MODE_OVR_EN_MASK, GCI_CHIPCONTROL_USBPHY_MODE_OVR_EN_SET );

    osl_wrapper_enable( (void*)PLATFORM_USB20D_MASTER_WRAPPER_REGBASE(0x0) );

    platform_common_chipcontrol( (void*)PLATFORM_USB20D_PHY_UTMI_CTL1_REG, PLATFORM_USB20D_PHY_UTMI1_CTL_PHY_SHUTOFF_MASK, PLATFORM_USB20D_PHY_UTMI1_CTL_PHY_SHUTOFF_DISABLE );
    OSL_DELAY(50);
    platform_common_chipcontrol( (void*)PLATFORM_USB20D_PHY_UTMI_CTL1_REG, PLATFORM_USB20D_PHY_UTMI1_CTL_PHY_SHUTOFF_MASK, PLATFORM_USB20D_PHY_UTMI1_CTL_PHY_SHUTOFF_ENABLE );

    osl_wrapper_disable( (void*)PLATFORM_USB20D_MASTER_WRAPPER_REGBASE(0x0) );

    platform_gci_chipcontrol( GCI_CHIPCONTROL_USBPHY_MODE_OVR_EN_REG, GCI_CHIPCONTROL_USBPHY_MODE_OVR_EN_MASK, 0x0 );
}

static void
platform_cores_powersave_enable_memory_clock_gating( void )
{
    PLATFORM_SOCSRAM_POWERCONTROL_REG->bits.enable_mem_clk_gate = 1;
}

static void
platform_cores_powersave_init_apps_domain( void )
{
    platform_cores_powersave_clock_gate_core_t cores[] =
    {
        { .core_addr = PLATFORM_GMAC_REGBASE(0x0), .wrapper_addr = PLATFORM_GMAC_MASTER_WRAPPER_REGBASE(0x0), PLATFORM_CAPABILITY_ENAB(PLATFORM_CAPS_GMAC) },
        { .core_addr = PLATFORM_I2S0_REGBASE(0x0), .wrapper_addr = PLATFORM_I2S0_MASTER_WRAPPER_REGBASE(0x0), PLATFORM_CAPABILITY_ENAB(PLATFORM_CAPS_I2S) },
        { .core_addr = PLATFORM_I2S1_REGBASE(0x0), .wrapper_addr = PLATFORM_I2S1_MASTER_WRAPPER_REGBASE(0x0), PLATFORM_CAPABILITY_ENAB(PLATFORM_CAPS_I2S) }
    };
    unsigned i;

    for ( i = 0; i < ARRAYSIZE( cores ); ++i )
    {
        if ( cores[i].is_enabled == WICED_TRUE )
        {
            platform_cores_powersave_clock_gate_core( &cores[i] );
        }
    }

    platform_cores_powersave_shutoff_usb20d( );

    platform_cores_powersave_enable_memory_clock_gating( );

    platform_cores_powersave_powerdown_memory( );
}

static void
platform_cores_powersave_disable_wl_reg_on_pulldown( wiced_bool_t disable )
{
    platform_pmu_regulatorcontrol( PMU_REGULATOR_WL_REG_ON_PULLDOWN_REG, PMU_REGULATOR_WL_REG_ON_PULLDOWN_MASK,
                                   disable ? PMU_REGULATOR_WL_REG_ON_PULLDOWN_DIS : PMU_REGULATOR_WL_REG_ON_PULLDOWN_EN );
}

void
platform_cores_powersave_init( void )
{
    platform_cores_powersave_disable_wl_reg_on_pulldown( WICED_TRUE );
    platform_cores_powersave_init_apps_domain( );
    platform_cores_powersave_init_wlan_domain( );
}

void
platform_cores_powersave_deinit( void )
{
    platform_cores_powersave_powerup_memory( );
}

WICED_SLEEP_EVENT_HANDLER( deep_sleep_cores_powersave_event_handler )
{
    if ( event == WICED_SLEEP_EVENT_ENTER )
    {
        platform_cores_powersave_disable_wl_reg_on_pulldown( WICED_FALSE );
    }
    else if ( event == WICED_SLEEP_EVENT_CANCEL )
    {
        platform_cores_powersave_disable_wl_reg_on_pulldown( WICED_TRUE );
    }
}

#else

void
platform_cores_powersave_init( void )
{
}

void
platform_cores_powersave_deinit( void )
{
}

#endif /* PLATFORM_CORES_POWERSAVE */
