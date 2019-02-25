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
 *  Implements initialisation and other management functions for WWD system
 *
 */

#include <string.h>
#include "wwd_management.h"
#include "wwd_events.h"
#include "wwd_wlioctl.h"
#include "wwd_assert.h"
#include "wwd_wifi.h"
#include "platform/wwd_bus_interface.h"
#include "platform/wwd_platform_interface.h"
#include "network/wwd_network_interface.h"
#include "network/wwd_buffer_interface.h"
#include "RTOS/wwd_rtos_interface.h"
#include "internal/wwd_sdpcm.h"
#include "internal/wwd_internal.h"
#include "internal/wwd_thread.h"
#include "internal/bus_protocols/wwd_bus_protocol_interface.h"
#ifdef WWD_DOWNLOAD_CLM_BLOB
#include "internal/wwd_clm.h"
#endif


/******************************************************
 *             Constants
 ******************************************************/

#define MAX_POST_SET_COUNTRY_RETRY  3

/******************************************************
 *             Static Variables
 ******************************************************/

/******************************************************
 *             Function definitions
 ******************************************************/

/**
 * Initialize Wi-Fi platform
 *
 * - Initializes the required parts of the hardware platform
 *   i.e. pins for SDIO/SPI, interrupt, reset, power etc.
 *
 * - Initializes the WWD thread which arbitrates access
 *   to the SDIO/SPI bus
 *
 * @return WWD_SUCCESS if initialization is successful, error code otherwise
 */
wwd_result_t wwd_management_wifi_platform_init( wiced_country_code_t country, wiced_bool_t resume_after_deep_sleep )
{
    wwd_result_t retval;

    wwd_wlan_status.country_code = country;

    retval = (wwd_result_t)host_platform_init( );
    if ( retval != WWD_SUCCESS )
    {
        WPRINT_WWD_INFO(("Could not initialize platform interface\n"));
        return retval;
    }

    retval = (wwd_result_t)host_platform_bus_init( );
    if ( retval != WWD_SUCCESS )
    {
        WPRINT_WWD_INFO(("Could not initialize platform bus interface\n"));
        return retval;
    }

    /* Enable 32K WLAN sleep clock */
    host_platform_init_wlan_powersave_clock();
    if ( resume_after_deep_sleep == WICED_TRUE )
    {
        retval = ( wwd_result_t ) wwd_bus_resume_after_deep_sleep( );
    }
    else
    {
        wwd_bus_init_stats( );
        retval = ( wwd_result_t ) wwd_bus_init( );
    }

    if ( retval != WWD_SUCCESS )
    {
        /* May have been due to user abort */
        WPRINT_WWD_INFO(("Could not initialize bus\n"));
        return retval;
    }

    /* WLAN device is now powered up. Change state from OFF to DOWN */
    wwd_wlan_status.state = WLAN_DOWN;

    retval = wwd_thread_init( );
    if ( retval != WWD_SUCCESS )
    {
        WPRINT_WWD_ERROR(("Could not initialize WWD thread\n"));
        return retval;
    }

    host_platform_bus_enable_interrupt( );

    return WWD_SUCCESS;
}

/*
* Halt resource download and abort init
*/
wwd_result_t wwd_management_wifi_platform_init_halt( wiced_bool_t halt )
{
    wwd_bus_set_resource_download_halt( halt );
    return WWD_SUCCESS;
}

/**
 * Turn on the Wi-Fi device
 *
 * - Initialize Wi-Fi device
 *
 * - Program various WiFi parameters and modes
 *
 * @return WWD_SUCCESS if initialization is successful, error code otherwise
 */
wwd_result_t wwd_management_wifi_on( wiced_country_code_t country )
{
    wl_country_t*  country_struct;
    uint32_t*      ptr;
    wwd_result_t   retval;
    wiced_buffer_t buffer;
    uint8_t*       event_mask;
    uint32_t*      data;
    uint32_t       counter;
#ifdef WICED_DISABLE_AMPDU_TX
    uint8_t        i;
#endif
#if defined(MAC_ADDRESS_SET_BY_HOST) || defined(WICED_USE_WIFI_TWO_STA_INTERFACE)
    wiced_mac_t mac_address;
#endif

    if ( wwd_wlan_status.state == WLAN_UP )
    {
        return WWD_SUCCESS;
    }

    wwd_init_stats();

    retval = wwd_management_wifi_platform_init( country, WICED_FALSE );
    if ( retval != WWD_SUCCESS )
    {
        WPRINT_WWD_INFO(("Could not initialize wifi platform\n"));
        return retval;
    }

    /* Download blob file if exists */
#ifdef WWD_DOWNLOAD_CLM_BLOB
    retval = wwd_process_clm_data();
    if ( retval != WWD_SUCCESS )
    {
        WPRINT_MACRO(("****************************************************\n"
                      "** ERROR: WLAN: could not download clm_blob file\n"
                      "** FATAL ERROR: system unusable, CLM blob file not found or corrupted.\n"
                      "****************************************************\n"));
        return retval;
    }
#endif

#ifdef MAC_ADDRESS_SET_BY_HOST
    /* See <WICED-SDK>/generated_mac_address.txt for info about setting the MAC address  */
    host_platform_get_mac_address(&mac_address);
    wwd_wifi_set_mac_address(mac_address, WWD_STA_INTERFACE);
#endif

    /* Turn off SDPCM TX Glomming */
    /* Note: This is only required for later chips.
     * The 4319 has glomming off by default however the 43362 has it on by default.
     */
    data = (uint32_t*) wwd_sdpcm_get_iovar_buffer( &buffer, (uint16_t) 4, IOVAR_STR_TX_GLOM );
    if ( data == NULL )
    {
        wiced_assert( "Could not get buffer for IOVAR", 0 != 0 );
        /*@-unreachable@*/ /* Lint: Reachable after hitting assert */
        return WWD_BUFFER_ALLOC_FAIL;
        /*@-unreachable@*/
    }
    *data = 0;
    retval = wwd_sdpcm_send_iovar( SDPCM_SET, buffer, 0, WWD_STA_INTERFACE );
    if (( retval != WWD_SUCCESS ) && (retval != WWD_WLAN_UNSUPPORTED))
    {
        /* Note: System may time out here if bus interrupts are not working properly */
        WPRINT_WWD_ERROR(("Could not turn off TX glomming\n"));
        return retval;
    }

    /* Turn APSTA on */
    data = (uint32_t*) wwd_sdpcm_get_iovar_buffer( &buffer, (uint16_t) sizeof(*data), IOVAR_STR_APSTA );
    if ( data == NULL )
    {
        wiced_assert( "Could not get buffer for IOVAR", 0 != 0 );
        return WWD_BUFFER_ALLOC_FAIL;
    }
    *data = (uint32_t) 1;
    /* This will fail on manufacturing test build since it does not have APSTA available */
    retval = wwd_sdpcm_send_iovar( SDPCM_SET, buffer, 0, WWD_STA_INTERFACE );
    if ( ( retval != WWD_SUCCESS ) && ( retval != WWD_WLAN_UNSUPPORTED ) )
    {
        /* Could not turn on APSTA */
        WPRINT_WWD_DEBUG( ("Could not turn on APSTA\n") );
    }

#ifdef WICED_DISABLE_AMPDU_TX
    /* Disable AMPDU for TX */
    for ( i = 0; i < 8; ++i )
    {
        struct ampdu_tid_control* ampdu_tid;
        ampdu_tid = (struct ampdu_tid_control*) wwd_sdpcm_get_iovar_buffer( &buffer, (uint16_t) sizeof(struct ampdu_tid_control), IOVAR_STR_AMPDU_TID );
        if ( ampdu_tid == NULL )
        {
            wiced_assert( "Could not get buffer for IOVAR", 0 != 0 );
            return WWD_BUFFER_ALLOC_FAIL;
        }
        ampdu_tid->tid    = i;
        ampdu_tid->enable = 0;
        retval = wwd_sdpcm_send_iovar( SDPCM_SET, buffer, 0, WWD_STA_INTERFACE );
        if ( retval != WWD_SUCCESS )
        {
            WPRINT_WWD_ERROR( ("Could not disable ampdu for tid: %u\n", i) );
        }
    }
#endif

    retval = wwd_wifi_set_ampdu_parameters();
    if ( retval != WWD_SUCCESS )
    {
        WPRINT_WWD_ERROR(("Could not set AMPDU parameters\r\n"));
        return retval;
    }

#ifdef WICED_STARTUP_DELAY
    (void) host_rtos_delay_milliseconds( (uint32_t) WWD_STARTUP_DELAY );
#endif

    /* Send set country command */
    /* This is the first time that the WLAN chip is required to respond
     * in it's normal run mode.
     * If you are porting a new system and it stalls here, it could
     * be one of the following problems:
     *   - Bus interrupt not triggering properly - the WLAN chip is unable to signal the host that there is data available.
     *   - Timing problems - if the timeouts on semaphores are not working correctly, then the
     *                       system might think that the IOCTL has timed out much faster than it should do.
     *
     */

    country_struct = (wl_country_t*) wwd_sdpcm_get_iovar_buffer( &buffer, (uint16_t) sizeof(wl_country_t), IOVAR_STR_COUNTRY );
    if ( country_struct == NULL )
    {
        wiced_assert( "Could not get buffer for IOCTL", 0 != 0 );
        return WWD_BUFFER_ALLOC_FAIL;
    }
    memset(country_struct, 0, sizeof(wl_country_t));

    ptr  = (uint32_t*)country_struct->ccode;
    *ptr = (uint32_t) wwd_wlan_status.country_code & 0x0000ffff;
    ptr  = (uint32_t*)country_struct->country_abbrev;
    *ptr = (uint32_t) wwd_wlan_status.country_code & 0x0000ffff;
    country_struct->rev = (int32_t) ( ( wwd_wlan_status.country_code & 0xffff0000 ) >> 16 );

    /* if regrev is 0 then set regrev to -1 so the FW will use any NVRAM/OTP configured aggregate
     * to choose the regrev.  If there is no aggregate configured then the FW will try to use regrev 0.
     */
    if ( country_struct->rev == 0 )
    {
        country_struct->rev = -1;
    }
    retval = wwd_sdpcm_send_iovar( SDPCM_SET, buffer, 0, WWD_STA_INTERFACE );
    if ( retval != WWD_SUCCESS )
    {
        /* Could not set wifi country */
        WPRINT_WWD_ERROR(("Could not set Country code\n"));
        return retval;
    }

#ifdef WICED_USE_WIFI_TWO_STA_INTERFACE
    /* Create second STA interface (on the so called AP_INTERFACE) and give it a new mac address */
    if (wwd_wifi_start_sta(WWD_AP_INTERFACE) != WWD_SUCCESS) {
        WPRINT_MACRO(("%s: Unable to create STA\n", __FUNCTION__));
    }
    if (wwd_wifi_get_mac_address(&mac_address, WWD_STA_INTERFACE ) != WWD_SUCCESS)
        WPRINT_MACRO(("%s: Unable to get mac addr\n", __FUNCTION__));

    /* Make new address different from original */
    mac_address.octet[4] ^= 1;

    if (wwd_wifi_set_AP_mac_address(mac_address ) != WWD_SUCCESS)
        WPRINT_MACRO(("%s: Unable to SET mac addr on second STA\n", __FUNCTION__));
#endif

    /* NOTE: The set country command requires time to process on the WLAN firmware and the following IOCTL may fail on initial attempts therefore we try a few times */

    /* Set the event mask, indicating initially we do not want any asynchronous events */
    for ( counter = 0, retval = WWD_PENDING; retval != WWD_SUCCESS && counter < (uint32_t)MAX_POST_SET_COUNTRY_RETRY; ++counter )
    {
        event_mask = (uint8_t*) wwd_sdpcm_get_iovar_buffer( &buffer, (uint16_t) WL_EVENTING_MASK_LEN, IOVAR_STR_EVENT_MSGS );
        if ( event_mask == NULL )
        {
            wiced_assert( "Could not get buffer for IOVAR", 0 != 0 );
            return WWD_BUFFER_ALLOC_FAIL;
        }
        memset( event_mask, 0, (size_t) WL_EVENTING_MASK_LEN );
        retval = wwd_sdpcm_send_iovar( SDPCM_SET, buffer, 0, WWD_STA_INTERFACE );
    }
    if ( retval != WWD_SUCCESS )
    {
        WPRINT_WWD_ERROR(("Could not set Event mask\n"));
        return retval;
    }

    /* Send UP command */
    wwd_wifi_set_up( );

    /* Set the GMode */
    data = (uint32_t*) wwd_sdpcm_get_ioctl_buffer( &buffer, (uint16_t) 4 );
    if ( data == NULL )
    {
        wiced_assert( "Could not get buffer for IOCTL", 0 != 0 );
        return WWD_BUFFER_ALLOC_FAIL;
    }
    *data = (uint32_t) GMODE_AUTO;
    retval = wwd_sdpcm_send_ioctl( SDPCM_SET, WLC_SET_GMODE, buffer, 0, WWD_STA_INTERFACE );
    if ( retval != WWD_SUCCESS )
    {
        /* Note: System may time out here if bus interrupts are not working properly */
        WPRINT_WWD_ERROR(("Error setting gmode\n"));
        return retval;
    }

    return WWD_SUCCESS;
}

/**
 * Turn off the Wi-Fi device
 *
 * - De-Initialises the required parts of the hardware platform
 *   i.e. pins for SDIO/SPI, interrupt, reset, power etc.
 *
 * - De-Initialises the WWD thread which arbitrates access
 *   to the SDIO/SPI bus
 *
 * @return WWD_SUCCESS if deinitialization is successful, error code otherwise
 */
wwd_result_t wwd_management_wifi_off( void )
{
    wwd_result_t retval;

    if ( wwd_wlan_status.state == WLAN_OFF )
    {
        return WWD_SUCCESS;
    }

    host_platform_bus_disable_interrupt( );

    wwd_thread_quit( );

    retval = wwd_bus_deinit( );
    if ( retval != WWD_SUCCESS )
    {
        WPRINT_WWD_DEBUG(("Error de-initializing bus\n"));
        return retval;
    }

    retval = host_platform_bus_deinit( );
    if ( retval != WWD_SUCCESS )
    {
        WPRINT_WWD_DEBUG(("Error de-initializing platform bus interface\n"));
        return retval;
    }

    retval = host_platform_deinit( );
    if ( retval != WWD_SUCCESS )
    {
        WPRINT_WWD_DEBUG(("Error de-initializing platform interface\n"));
        return retval;
    }

    /* Disable 32K WLAN sleep clock */
    host_platform_deinit_wlan_powersave_clock();

    wwd_wlan_status.state = WLAN_OFF;
    return WWD_SUCCESS;
}
