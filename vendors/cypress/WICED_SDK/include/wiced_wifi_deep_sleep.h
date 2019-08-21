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

/**
 * @file
 *  Defines functions to perform Wi-Fi Deep Sleep (DS1) operations
 */

#pragma once

#include "wiced_utilities.h"
#include "wwd_wifi.h"
#include "wwd_debug.h"
#include "wiced_rtos.h"
#include "wiced_tcpip.h"
#ifdef __cplusplus
extern "C" {
#endif

/******************************************************
 *                   Enumerations
 ******************************************************/
/**
 * ds1 mode Offload types
 */
typedef enum
{
    WICED_OFFLOAD_PATTERN    = 0,   /**< Pattern    */
    WICED_OFFLOAD_KEEP_ALIVE = 1,   /**< Keep Alive */
    WICED_OFFLOAD_ARP_HOSTIP = 2,   /**< Host IP    */
    WICED_OFFLOAD_MAGIC      = 3,   /**< Magic packet wake */
    WICED_OFFLOAD_GTK        = 4,   /**< GTK (Temporal key) refresh */
    WICED_OFFLOAD_DEAUTH     = 5,   /**< Deauth */
    WICED_OFFLOAD_ALL        = 6,   /**< ALL available */
} wiced_offload_t;

/******************************************************
 *                 Type Definitions
 ******************************************************/

/*
 *  Callback when wifi module goes into the DS1 state (may enter/exit ds1 multiple times after calling enter ds1, depending on WOWL settings and TX/RX needs)
 */
typedef void (*wiced_wifi_ds1_complete_callback_t)( void* user_parameter );

/******************************************************
 *                    Structures
 ******************************************************/
/**
 * Ds1 debug information
 */
typedef struct
{
    wiced_bool_t    wowl_valid;     /**< Wowl has a valid override   */
    uint32_t        wowl;           /**< Wowl override   */
    wiced_bool_t    wowl_os_valid;  /**< Wowl_os has a valid override   */
    uint32_t        wowl_os;        /**< Wowl_os override   */
} wiced_ds1_debug_t;

/**
 * Packet pattern
 */
typedef struct
{
    uint32_t match_offset;  /**< offset in packet to start looking */
    uint32_t mask_size;     /**< size of mask */
    uint8_t  *mask;         /**< pointer to memory holding the mask */
    uint32_t pattern_size;  /**< size of pattern in bytes */
    uint8_t  *pattern;      /**< pointer to memory holding the pattern */
} wiced_packet_pattern_t;

/**
 * Offload value information
 */
typedef union
{
    wiced_keep_alive_packet_t keep_alive_packet_info;   /**< Keep Alive info    */
    wiced_packet_pattern_t    pattern;                  /**< Packet Pattern     */
    wiced_ip_address_t        ipv4_address;             /**< IPv4 address       */
} wiced_offload_value_t;

typedef struct
{
    uint8_t                 num_offloads;               /**< Number of offloads and number of elements in types array and values array */
    wiced_offload_t         *types;                     /**< Types of offloads in values array */
    wiced_offload_value_t   *values;                    /**< array of offloads values */
} wiced_offloads_container_t;

/******************************************************
 *               Function Declarations
 ******************************************************/

/*****************************************************************************/
/** @addtogroup wifideepsleep     WiFi Deep Sleep Functions
 *  @ingroup wifi
 *  WICED Wi-Fi functions for DS1 (Wi-Fi Deep Sleep) Entry/Exit and Debug.
 *
 *  @{
 */
/****************************************************************************/

/**
 * Set configuration for entering the Wi-Fi deep sleep (DS1) state.  Will be used to enter DS1 after wiced_wifi_ds1_enable has been called, when all preconditions are satisfied.
 * @param[in] interface : interface to use for offload start and DS1 enter
 * @param[in] offload_type : type of offload to init
 * @param[in] offload_value : parameters for the offload; this memory must not be freed until after wiced_wifi_ds1_disable is invoked
 * @param[in] ulp_wait_milliseconds : seconds to wait prior to entering DS1
 *
 * @return @ref wiced_result_t
 */
extern wiced_result_t wiced_wifi_ds1_config( wiced_interface_t interface, wiced_offloads_container_t *offload_value, uint32_t ulp_wait_milliseconds );

/**
 * Enable deep sleep 1 (DS1) state.  Put firmware in state to enter Wi-Fi deep sleep whenever possible.
 * Preconditions for entering DS1: STA is associated; no other interfaces are active for 802.11.  (AP and GO must both be disabled.)
 * @param[in] interface : interface to use for offload start and DS1 enter
 *
 * @return @ref wiced_result_t
 */
extern wiced_result_t wiced_wifi_ds1_enable( wiced_interface_t interface );

/**
 * Disable deep sleep 1 (DS1) state.  Put WICED in state to never use Wi-Fi deep sleeping.
 * WICED will leave Wi-Fi deep sleep if already sleeping and will not re-enter until wiced_wifi_ds1_enable is called again.
 * @param[in] interface : interface to use for offload start and DS1 enter
 *
 * @return @ref wiced_result_t
 */
extern wiced_result_t wiced_wifi_ds1_disable( wiced_interface_t interface );

/**
 * Return a string description of the WICED DS1 state (string describes whether DS1 is configured, enabled on host and firmware or not)
 * @param[out] output : null delimited string is written into this buffer
 * @param[in]   max_output_length : maximum amount of chars, including null that the output string can contain
 */
extern void wiced_wifi_deep_sleep_get_status_string( uint8_t *output, uint16_t max_output_length );

/**
 * Enter deep sleep 1 (DS1) state.
 * First init specified offload type, wait the number of seconds, then enter DS1.
 * @param[in] interface : interface to use for offload start and DS1 enter
 * @param[in] offload_type : type of offload to init
 * @param[in] offload_value : parameters for the offload
 * @param[in] ulp_wait_milliseconds : seconds to wait prior to entering DS1
 * @note: Calling this after Equivalent to calling configure and then immediate attempted entry into DS1
 * (Deep sleep is suspended, config is updated, and then attempt is made to enter DS1)
 * Failure to enter deepsleep leaves WICED state as DS1 enabled.
 * @return @ref wiced_result_t
 */
extern wiced_result_t wiced_wifi_enter_ds1( wiced_interface_t interface, wiced_offload_t offload_type, wiced_offload_value_t *offload_value, uint32_t ulp_wait_milliseconds );

/**
 * Enter deep sleep 1 (DS1) state.
 *    First init specified offload type, wait the number of seconds, then enter DS1.
 * @param[in] interface : interface to use for offload start and DS1 enter
 * @param[in] offload_type : type of offload to init
 * @param[in] offload_value : parameters for the offload
 * @param[in] ulp_wait_milliseconds : seconds to wait prior to entering DS1
 * @param[in] debug_options : parameters used for controlling low level debug
 *
 * @return @ref wiced_result_t
 */
extern wiced_result_t wiced_wifi_enter_ds1_debug( wiced_interface_t interface, wiced_offload_t offload_type, wiced_offload_value_t *offload_value, uint32_t ulp_wait_milliseconds, wiced_ds1_debug_t *debug_overrides );

/**
 * Exit deep sleep 1 (DS1) state.
 * @param[in] interface : interface to use for DS1 exit
 * @note Calling this when DS1 is not enabled or when not deep sleeping results in an error return.
 * @return @ref wiced_result_t
 */
extern wiced_result_t wiced_wifi_wake_ds1( wiced_interface_t interface );

/**
 * Get a callback when the wifi module has entered the deep sleep 1 (DS1) state.
 * @param[in] callback : function to call whenever DS1 entry is detected
 * @param[in] user_parameter: parameter to pass to the callback function when the callback is made
 *
 * @return @ref wiced_result_t
 */
extern wiced_result_t wiced_wifi_ds1_set_complete_callback( wiced_wifi_ds1_complete_callback_t callback, void *user_parameter );

/**
 * This API is called when a STA, AP, GO, or GC have a connection created or destroyed
 */
extern void wiced_wifi_ds1_notify_link_change( void );

/** @} */

#ifdef __cplusplus
} /*extern "C" */
#endif
