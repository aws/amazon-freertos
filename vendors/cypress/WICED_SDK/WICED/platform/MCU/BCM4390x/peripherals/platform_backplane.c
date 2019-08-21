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

#include "platform_peripheral.h"
#include "platform_map.h"
#include "platform_assert.h"
#include "platform_config.h"

#include "typedefs.h"
#include "sbchipc.h"
#include "aidmp.h"
#include "hndsoc.h"
#include "wiced_osl.h"

#include "wiced_low_power.h"

#include <stdio.h>

/******************************************************
 *                      Macros
 ******************************************************/

#define PLATFORM_BACKPLANE_MIN_TIMEOUT 4
#define PLATFORM_BACKPLANE_MAX_TIMEOUT 19

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

typedef union
{
    uint32_t raw;
    struct
    {
        unsigned bus_error_reset      : 1;
        unsigned bus_error_interrupt  : 1;
        unsigned timeout_reset        : 1;
        unsigned timeout_interrupt    : 1;
        unsigned timeout_exponent     : 5;
        unsigned timeout_enable       : 1;
        unsigned __reserved           : 22;
    } bits;
} dmp_error_log_control_t;

typedef enum
{
    DMP_ERROR_LOG_STATUS_CAUSE_NO_ERROR      = 0x0,
    DMP_ERROR_LOG_STATUS_CAUSE_SLAVE_ERROR   = 0x1,
    DMP_ERROR_LOG_STATUS_CAUSE_TIMEOUT_ERROR = 0x2,
    DMP_ERROR_LOG_STATUS_CAUSE_DECODE_ERROR  = 0x3,
} dmp_error_log_status_cause_t;

typedef union
{
    uint32_t raw;
    struct
    {
        dmp_error_log_status_cause_t cause : 2;
        unsigned overflow                  : 1;
        unsigned __reserved                : 29;
    } bits;
} dmp_error_log_status_t;

typedef enum
{
    PLATFORM_WRAPPER_SLAVE,
    PLATFORM_WRAPPER_MASTER
} platform_wrapper_type_t;

typedef struct
{
    uint32_t                addr;
    platform_backplane_t    bp;
    platform_wrapper_type_t type;
} platform_wrapper_descr_t;

/******************************************************
 *               Function Declarations
 ******************************************************/

/******************************************************
 *               Variables Definitions
 ******************************************************/

/******************************************************
 *               Function Definitions
 ******************************************************/

static void
platform_wrappers_foreach( void (*callback)(volatile aidmp_t*, void*), void* ptr, uint32_t type_mask, uint32_t backplane_mask )
{
    static const platform_wrapper_descr_t wrappers[] =
    {
        { .addr = PLATFORM_DDR_CONTROLLER_SLAVE_WRAPPER_REGBASE(0x0), .bp = PLATFORM_BACKPLANE_APPS, .type = PLATFORM_WRAPPER_SLAVE  },
        { .addr = PLATFORM_SOCSRAM_CH0_SLAVE_WRAPPER_REGBASE(0x0),    .bp = PLATFORM_BACKPLANE_APPS, .type = PLATFORM_WRAPPER_SLAVE  },
        { .addr = PLATFORM_SOCSRAM_CH1_SLAVE_WRAPPER_REGBASE(0x0),    .bp = PLATFORM_BACKPLANE_APPS, .type = PLATFORM_WRAPPER_SLAVE  },
        { .addr = PLATFORM_APPSCR4_SLAVE_WRAPPER_REGBASE(0x0),        .bp = PLATFORM_BACKPLANE_APPS, .type = PLATFORM_WRAPPER_SLAVE  },
        { .addr = PLATFORM_CHIPCOMMON_SLAVE_WRAPPER_REGBASE(0x0),     .bp = PLATFORM_BACKPLANE_APPS, .type = PLATFORM_WRAPPER_SLAVE  },
#ifndef WICED_NO_WIFI
        { .addr = PLATFORM_WLANCR4_SLAVE_WRAPPER_REGBASE(0x0),        .bp = PLATFORM_BACKPLANE_WLAN, .type = PLATFORM_WRAPPER_SLAVE  },
        { .addr = PLATFORM_WLAN_APB0_SLAVE_WRAPPER_REGBASE(0x0),      .bp = PLATFORM_BACKPLANE_WLAN, .type = PLATFORM_WRAPPER_SLAVE  },
        { .addr = PLATFORM_WLAN_DEFAULT_SLAVE_WRAPPER_REGBASE(0x0),   .bp = PLATFORM_BACKPLANE_WLAN, .type = PLATFORM_WRAPPER_SLAVE  },
#endif
        { .addr = PLATFORM_AON_APB0_SLAVE_WRAPPER_REGBASE(0x0),       .bp = PLATFORM_BACKPLANE_AON,  .type = PLATFORM_WRAPPER_SLAVE  },
        { .addr = PLATFORM_APPS_APB0_SLAVE_WRAPPER_REGBASE(0x0),      .bp = PLATFORM_BACKPLANE_APPS, .type = PLATFORM_WRAPPER_SLAVE  },
        { .addr = PLATFORM_APPS_DEFAULT_SLAVE_WRAPPER_REGBASE(0x0),   .bp = PLATFORM_BACKPLANE_APPS, .type = PLATFORM_WRAPPER_SLAVE  },
        { .addr = PLATFORM_CHIPCOMMON_MASTER_WRAPPER_REGBASE(0x0),    .bp = PLATFORM_BACKPLANE_APPS, .type = PLATFORM_WRAPPER_MASTER },
        { .addr = PLATFORM_I2S0_MASTER_WRAPPER_REGBASE(0x0),          .bp = PLATFORM_BACKPLANE_APPS, .type = PLATFORM_WRAPPER_MASTER },
        { .addr = PLATFORM_I2S1_MASTER_WRAPPER_REGBASE(0x0),          .bp = PLATFORM_BACKPLANE_APPS, .type = PLATFORM_WRAPPER_MASTER },
        { .addr = PLATFORM_APPSCR4_MASTER_WRAPPER_REGBASE(0x0),       .bp = PLATFORM_BACKPLANE_APPS, .type = PLATFORM_WRAPPER_MASTER },
        { .addr = PLATFORM_M2M_MASTER_WRAPPER_REGBASE(0x0),           .bp = PLATFORM_BACKPLANE_APPS, .type = PLATFORM_WRAPPER_MASTER },
        { .addr = PLATFORM_GMAC_MASTER_WRAPPER_REGBASE(0x0),          .bp = PLATFORM_BACKPLANE_APPS, .type = PLATFORM_WRAPPER_MASTER },
        { .addr = PLATFORM_USB20H_MASTER_WRAPPER_REGBASE(0x0),        .bp = PLATFORM_BACKPLANE_APPS, .type = PLATFORM_WRAPPER_MASTER },
        { .addr = PLATFORM_USB20D_MASTER_WRAPPER_REGBASE(0x0),        .bp = PLATFORM_BACKPLANE_APPS, .type = PLATFORM_WRAPPER_MASTER },
        { .addr = PLATFORM_SDIOH_MASTER_WRAPPER_REGBASE(0x0),         .bp = PLATFORM_BACKPLANE_APPS, .type = PLATFORM_WRAPPER_MASTER },
        { .addr = PLATFORM_SDIOD_MASTER_WRAPPER_REGBASE(0x0),         .bp = PLATFORM_BACKPLANE_APPS, .type = PLATFORM_WRAPPER_MASTER },
        { .addr = PLATFORM_CRYPTO_MASTER_WRAPPER_REGBASE(0x0),        .bp = PLATFORM_BACKPLANE_APPS, .type = PLATFORM_WRAPPER_MASTER },
#ifndef WICED_NO_WIFI
        { .addr = PLATFORM_DOT11MAC_MASTER_WRAPPER_REGBASE(0x0),      .bp = PLATFORM_BACKPLANE_WLAN, .type = PLATFORM_WRAPPER_MASTER },
        { .addr = PLATFORM_WLANCR4_MASTER_WRAPPER_REGBASE(0x0),       .bp = PLATFORM_BACKPLANE_WLAN, .type = PLATFORM_WRAPPER_MASTER },
#endif
    };

    unsigned int i;

    for ( i = 0; i < ARRAYSIZE( wrappers ); ++i )
    {
        const platform_wrapper_descr_t* wrapper = &wrappers[i];

        if ( ( type_mask & ( 1 << wrapper->type ) ) && ( backplane_mask & ( 1 << wrapper->bp ) ) )
        {
            callback( (volatile aidmp_t*)wrapper->addr, ptr );
        }
    }
}

#if !PLATFORM_NO_BP_INIT
static void
platform_wrapper_set_timeout( volatile aidmp_t* dmp, void* ptr )
{
    uint8_t timeout = *(uint8_t*)ptr;
    dmp_error_log_control_t error_log_control = { .raw = dmp->errlogctrl };

    if ( timeout == 0 )
    {
        error_log_control.bits.timeout_enable = 0;
    }
    else
    {
        timeout = MAX( MIN( timeout, PLATFORM_BACKPLANE_MAX_TIMEOUT ), PLATFORM_BACKPLANE_MIN_TIMEOUT );
        error_log_control.bits.timeout_enable   = 1;
        error_log_control.bits.timeout_exponent = timeout;
    }

    dmp->errlogctrl = error_log_control.raw;
}
#endif /* !PLATFORM_NO_BP_INIT */

static void
platform_wrapper_print_info( volatile aidmp_t* dmp, void* ptr )
{
    int clock = (dmp->ioctrl & SICF_CLOCK_EN) != 0;
    int force = (dmp->ioctrl & SICF_FGC) != 0;
    int reset = (dmp->resetctrl & AIRC_RESET) != 0;

    UNUSED_VARIABLE( ptr );

    printf("%p: clock=%d force=%d reset=%d ioctrl=0x%x resetctrl=0x%x\n",
        dmp, clock, force, reset, dmp->ioctrl, dmp->resetctrl);
}

static void
platform_wrapper_reset( volatile aidmp_t* dmp, void* ptr )
{
    uint32_t* except_wrappers = (uint32_t*)ptr;

    unsigned int i;
    uint32_t dmp_addr = (uint32_t)dmp;

    for ( i = 0; except_wrappers && except_wrappers[i]; ++i )
    {
        if ( dmp_addr == except_wrappers[i] )
        {
            return;
        }
    }

    osl_wrapper_disable((void*)dmp_addr);
}

static void
platform_wrapper_debug( volatile aidmp_t* dmp, void* ptr )
{
    dmp_error_log_status_t error_log_status = { .raw = dmp->errlogstatus };

    UNUSED_VARIABLE( ptr );

    if ( error_log_status.bits.cause != DMP_ERROR_LOG_STATUS_CAUSE_NO_ERROR )
    {
        uint32_t error_log_addr_low  = dmp->errlogaddrlo;
        uint32_t error_log_addr_high = dmp->errlogaddrhi;

        UNUSED_VARIABLE( error_log_addr_low );
        UNUSED_VARIABLE( error_log_addr_high );

        /* Intentional self assignment :
         * This register (errlogdone) uses "write back 1 to clear",
         * So reading from it and writing the same value back to clear those bits set by hardware.
         */
        dmp->errlogdone = dmp->errlogdone;

        WICED_TRIGGER_BREAKPOINT(); /* It may be more then one errors detected. Make sense to continue debugging from next line. */
    }
}

static uint32_t
platform_backplane_all_mask( wiced_bool_t boot )
{
    uint32_t ret = ( 1 << PLATFORM_BACKPLANE_APPS ) | ( 1 << PLATFORM_BACKPLANE_AON );

    if ( boot && !WICED_DEEP_SLEEP_IS_WARMBOOT() )
    {
        ret |= ( 1 << PLATFORM_BACKPLANE_WLAN );
    }

    return ret;
}

platform_result_t
platform_backplane_init( void )
{
#if PLATFORM_NO_BP_INIT

    return PLATFORM_FEATURE_DISABLED;

#else

    uint8_t timeout_power_2_cycles = PLATFORM_BP_TIMEOUT;
    platform_wrappers_foreach( platform_wrapper_set_timeout, &timeout_power_2_cycles, 1 << PLATFORM_WRAPPER_SLAVE, platform_backplane_all_mask( WICED_TRUE ) );
    return PLATFORM_SUCCESS;

#endif /* PLATFORM_NO_BP_INIT */
}

void
platform_backplane_debug( void )
{
    platform_wrappers_foreach( platform_wrapper_debug, NULL, 1 << PLATFORM_WRAPPER_SLAVE, platform_backplane_all_mask( WICED_FALSE ) );
}

void
platform_backplane_print_cores( uint32_t backplane_mask )
{
    platform_wrappers_foreach( platform_wrapper_print_info, NULL, ~0x0, backplane_mask );
}

void
platform_backplane_reset_cores( uint32_t* except_wrappers, uint32_t backplane_mask )
{
    platform_wrappers_foreach( platform_wrapper_reset, except_wrappers, ~0x0, backplane_mask );
}
