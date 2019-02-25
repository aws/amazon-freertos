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
#pragma once

#include <stdint.h>
#include "platform_toolchain.h"
#include "wwd_constants.h"
#include "wiced_constants.h"
#include "wiced_result.h"
#include "platform_config.h"

#ifdef __cplusplus
extern "C" {
#endif

/******************************************************
 *                    Callback types
 ******************************************************/

typedef enum
{
    WICED_SLEEP_EVENT_ENTER,
    WICED_SLEEP_EVENT_CANCEL,
    WICED_SLEEP_EVENT_LEAVE,
    WICED_SLEEP_EVENT_WLAN_RESUME
} wiced_sleep_event_type_t;

typedef void( *wiced_sleep_event_handler_t )( wiced_sleep_event_type_t event );

/******************************************************
 *                 Platform definitions
 ******************************************************/

#define PLATFORM_LOW_POWER_HEADER_INCLUDED
#include "platform_low_power.h"

/******************************************************
 *                      Macros
 ******************************************************/

typedef struct
{
    wiced_sleep_event_handler_t handler;
} wiced_sleep_event_registration_t;

#ifndef WICED_ENABLE_LOW_POWER_EVENT_HANDLERS
#if !defined(WICED_DISABLE_MCU_POWERSAVE) && defined(PLATFORM_SUPPORTS_LOW_POWER_MODES) && !defined(__IAR_SYSTEMS_ICC__)
#define WICED_ENABLE_LOW_POWER_EVENT_HANDLERS 1
#else
#define WICED_ENABLE_LOW_POWER_EVENT_HANDLERS 0
#endif /* !defined(WICED_DISABLE_MCU_POWERSAVE) && defined(PLATFORM_SUPPORTS_LOW_POWER_MODES) */
#endif /* WICED_ENABLE_LOW_POWER_EVENT_HANDLERS */

#if WICED_ENABLE_LOW_POWER_EVENT_HANDLERS

#ifndef WICED_SLEEP_STR_EXPAND
#define WICED_SLEEP_STR_EXPAND( name )                      #name
#endif

#ifndef WICED_SLEEP_SECTION_NAME_EVENT_HANDLER
#define WICED_SLEEP_SECTION_NAME_EVENT_HANDLER( name )      ".sleep_event_handlers."WICED_SLEEP_STR_EXPAND( name )
#endif

#ifndef WICED_SLEEP_SECTION_NAME_EVENT_REGISTRATION
#define WICED_SLEEP_SECTION_NAME_EVENT_REGISTRATION( name ) ".sleep_event_registrations."WICED_SLEEP_STR_EXPAND( name )
#endif

#ifndef WICED_SLEEP_EVENT_HANDLER
#define WICED_SLEEP_EVENT_HANDLER( func_name ) \
    static void SECTION( WICED_SLEEP_SECTION_NAME_EVENT_HANDLER( func_name ) ) func_name( wiced_sleep_event_type_t event ); \
    const wiced_sleep_event_registration_t SECTION( WICED_SLEEP_SECTION_NAME_EVENT_REGISTRATION( func_name ) ) func_name##_registration = { .handler = &func_name }; \
    static void MAY_BE_UNUSED func_name( wiced_sleep_event_type_t event )
#endif

void SECTION( WICED_SLEEP_SECTION_NAME_EVENT_HANDLER( wiced_sleep_call_event_handlers ) ) wiced_sleep_call_event_handlers( wiced_sleep_event_type_t event );

#ifndef WICED_SLEEP_CALL_EVENT_HANDLERS
#define WICED_SLEEP_CALL_EVENT_HANDLERS( cond, event ) \
    do { if ( cond ) wiced_sleep_call_event_handlers( event ); } while( 0 )
#endif


extern wiced_sleep_event_registration_t link_sleep_event_registrations_location;
extern wiced_sleep_event_registration_t link_sleep_event_registrations_end;



#endif /* WICED_ENABLE_LOW_POWER_EVENT_HANDLERS */

#ifndef WICED_DEEP_SLEEP_SAVED_VAR
#define WICED_DEEP_SLEEP_SAVED_VAR( var )                                               var
#endif

#ifndef WICED_SLEEP_EVENT_HANDLER
#ifdef  __IAR_SYSTEMS_ICC__
#define IAR_ROOT_FUNC __root
#else
#define IAR_ROOT_FUNC
#endif /* __IAR_SYSTEMS_ICC__ */
#define WICED_SLEEP_EVENT_HANDLER( func_name ) \
    static IAR_ROOT_FUNC void MAY_BE_UNUSED func_name( wiced_sleep_event_type_t event )
#endif /* ifndef WICED_SLEEP_EVENT_HANDLER */

#ifndef WICED_SLEEP_CALL_EVENT_HANDLERS
#define WICED_SLEEP_CALL_EVENT_HANDLERS( cond, event )
#endif

#ifndef WICED_DEEP_SLEEP_IS_WARMBOOT
#define WICED_DEEP_SLEEP_IS_WARMBOOT( )                                            0
#endif

#ifndef WICED_DEEP_SLEEP_IS_ENABLED
#define WICED_DEEP_SLEEP_IS_ENABLED( )                                             0
#endif

#ifndef WICED_DEEP_SLEEP_IS_WARMBOOT_HANDLE
#define WICED_DEEP_SLEEP_IS_WARMBOOT_HANDLE( )                                     ( WICED_DEEP_SLEEP_IS_ENABLED( ) && WICED_DEEP_SLEEP_IS_WARMBOOT( ) )
#endif

#ifndef WICED_DEEP_SLEEP_SAVE_PACKETS_NUM
#define WICED_DEEP_SLEEP_SAVE_PACKETS_NUM                                           0
#endif

/* The following #defines are deprecated from WICED 5.3 onwards, they
 * exist for backward compatibility with past WICED releases *
 */
#define wiced_deep_sleep_event_type_t       wiced_sleep_event_type_t
#define WICED_DEEP_SLEEP_EVENT_ENTER        WICED_SLEEP_EVENT_ENTER
#define WICED_DEEP_SLEEP_EVENT_CANCEL       WICED_SLEEP_EVENT_CANCEL
#define WICED_DEEP_SLEEP_EVENT_LEAVE        WICED_SLEEP_EVENT_LEAVE
#define WICED_DEEP_SLEEP_EVENT_HANDLER      WICED_SLEEP_EVENT_HANDLER

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
 *                 Global Variables
 ******************************************************/

/******************************************************
 *               Function Declarations
 ******************************************************/

/*****************************************************************************/
/** @addtogroup initconf       Deep-sleep related functions
 *  @ingroup deep-sleep
 *
 * Functions to resume WICED after the deep-sleep.
 * Platform is not necessary to support deep-sleep mode.
 *
 *  @{
 */
/*****************************************************************************/

/** Resumes the WICED system after deep-sleep
 *
 * This function sets up the system same way as wiced_init
 * and has to be used when system resumes from deep-sleep
 *
 * @return @ref wiced_result_t
 */
extern wiced_result_t wiced_resume_after_deep_sleep( void );


/** Return the time spent during deep sleep.
 *
 * @note Currently this is implemented for 4390x platforms only.
 *
 * @return time in system ticks
 */
uint32_t     wiced_deep_sleep_ticks_since_enter( void );

/** @} */

#ifdef __cplusplus
} /* extern "C" */
#endif
