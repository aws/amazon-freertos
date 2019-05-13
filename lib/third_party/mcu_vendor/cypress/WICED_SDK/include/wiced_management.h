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
 *  Defines functions to manage the WICED system
 */

#pragma once

#include "network/wwd_network_interface.h"
#include "wiced_tcpip.h"
#include "wiced_wifi.h"

#ifdef __cplusplus
extern "C" {
#endif

/******************************************************
 *                     Macros
 ******************************************************/

/******************************************************
 *                    Constants
 ******************************************************/

/******************************************************
 *                   Enumerations
 ******************************************************/

/** IP address configuration options */
typedef enum
{
    WICED_USE_EXTERNAL_DHCP_SERVER,        /**< Client interface: use an external DHCP server  */
    WICED_USE_STATIC_IP,                   /**< Client interface: use a fixed IP address       */
    WICED_USE_INTERNAL_DHCP_SERVER,        /**< softAP interface: use the internal DHCP server */
    WICED_USE_EXTERNAL_DHCP_SERVER_RESTORE /**< Client interface: use an external DHCP server; restore last saved DHCP state */
} wiced_network_config_t;


/** DCT app section configuration item data type */
typedef enum
{
    CONFIG_STRING_DATA,       /**< String data type */
    CONFIG_UINT8_DATA,        /**< uint8 data type  */
    CONFIG_UINT16_DATA,       /**< uint16 data type */
    CONFIG_UINT32_DATA        /**< uint32 data type */
} configuration_data_type_t;


/** WICED Network link subscription types denote whether
 *  to subscribe for link up or link down events */
typedef enum
{
    WICED_LINK_UP_SUBSCRIPTION,     /**< Link up event subscription   */
    WICED_LINK_DOWN_SUBSCRIPTION    /**< Link down event subscription */
} wiced_link_subscription_t;

/** WICED Network link status */
typedef enum
{
    WICED_LINK_UP,   /**< Link status up   */
    WICED_LINK_DOWN  /**< Link status down */
} wiced_link_status_t;

typedef enum
{
    WICED_NETWORK_PACKET_TX,     /**< Network packet for data transmission */
    WICED_NETWORK_PACKET_RX      /**< Network packet for data reception    */
} wiced_network_packet_dir_t;

/******************************************************
 *                 Type Definitions
 ******************************************************/

/** Network link callback */
typedef void (*wiced_network_link_callback_t)(void *user_data);

/******************************************************
 *                    Structures
 ******************************************************/

/** IP address settings */
typedef struct
{
    wiced_ip_address_t ip_address;  /**< IP address      */
    wiced_ip_address_t gateway;     /**< Gateway address */
    wiced_ip_address_t netmask;     /**< Netmask         */
} wiced_ip_setting_t;


/** DCT app section configuration item entry */
typedef struct
{
    char*                     name;        /**< Name of the entry              */
    uint32_t                  dct_offset;  /**< Offset of the entry in the DCT */
    uint32_t                  data_size;   /**< Size of the entry              */
    configuration_data_type_t data_type;   /**< Type of the entry              */
} configuration_entry_t;

/**
 * Structure describing a list of associated softAP clients' ip-address
 */
typedef struct
{
    uint32_t                count;                  /**< Number of IP addresses in the list    */
    wiced_ip_address_t      ip_address_list[1];     /**< Variable length array of IP addresses */
} wiced_ip_address_list_t;

/******************************************************
 *                 Global Variables
 ******************************************************/

/******************************************************
 *               Function Declarations
 *
 ******************************************************/

/*****************************************************************************/
/** @defgroup mgmt       Management
 *
 *  WICED Management Functions provides WICED API's to initialize the WICED Platform,
 *  Network, enable/disable power save and utility functions to bring network up/
 *  down/suspend and De-initialize WICED  by freeing up resources
 *
 *  - WICED initialization, de-initialization
 *  - WICED network initialization, de-initialization
 *  - WICED enable, disable power-save
 *  - WICED configure, re-configure DCT variables
 *  - WICED set-hostname, get-hostname
 *  - WICED network-up, network-down, suspend, resume and utility functions
 *  - WICED core-init, de-init.
 *
 */
/*****************************************************************************/


/*****************************************************************************/
/** @addtogroup wicedcore       Initialization & configuration
 *  @ingroup mgmt
 *
 * Initialization/De-initialization of WICED and device configuration functions and
 * Initialization/De-initialization of WICED Network Interface
 *
 *  @{
 */
/*****************************************************************************/


/** Initializes the WICED system
 *
 * This function sets up the system by :
 *  \li initializing the platform interface
 *  \li initializing the RTOS & Network Stack
 *  \li initializing the WLAN driver and chip
 *  \li starting the event processing thread
 *
 *  @param[in] : void
 *
 *  @return @ref wiced_result_t WICED_SUCCESS : on success.
 *                              WICED_ERROR   : if an error occurred
 */
extern wiced_result_t wiced_init( void );


/** De-initializes the WICED system
 *
 * This function de-initializes the WICED system by :
 *  \li bringing down all network interfaces
 *  \li deleting all packet pools
 *  \li tearing down the event thread
 *  \li powering down the WLAN chip
 *
 *  @param[in] : void
 *
 *  @return @ref wiced_result_t WICED_SUCCESS : on success.
 *                              WICED_ERROR   : if an error occurred
 */
extern wiced_result_t wiced_deinit( void );


/** Initializes network sub-system only
 *
 *  @param[in] : void
 *
 *  @return @ref wiced_result_t WICED_SUCCESS : on success.
 *                              WICED_ERROR   : if an error occurred
 *
 */
extern wiced_result_t wiced_network_init  ( void );


/** De-initializes network sub-system only
 *
 *  @param[in] : void
 *
 *  @return @ref wiced_result_t WICED_SUCCESS : on success.
 *                              WICED_ERROR   : if an error occurred
 */
extern wiced_result_t wiced_network_deinit( void );

/** Enables all power-save features
 *
 *  This is a convenience function that calls each of the power-save related functions listed below \n
 *  Please review the documentation for each function for further information
 *  \li @ref wiced_platform_mcu_enable_powersave()
 *  \li @ref wiced_wifi_enable_powersave()
 *  \li @ref wiced_network_suspend()
 *
 *  @param[in] : void
 *
 *  @return @ref wiced_result_t WICED_SUCCESS : on success.
 *                              WICED_ERROR   : if an error occurred
 *
 */
extern wiced_result_t wiced_enable_powersave( void );


/** Disables all power-save features
 *
 *  This is a convenience functions that calls each of the power-save related functions listed below \n
 *  Please review the documentation for each function for further information
 *  \li @ref wiced_platform_mcu_disable_powersave()
 *  \li @ref wiced_wifi_disable_powersave()
 *  \li @ref wiced_network_resume()
 *
 *  @param[in] : void
 *
 *  @return @ref wiced_result_t WICED_SUCCESS : on success.
 *                              WICED_ERROR   : if an error occurred
 */
extern wiced_result_t wiced_disable_powersave( void );


/** Runs device configuration (if required)
 *
 * @param[in] config  : An array of user configurable variables in configuration_entry_t format.
 *                      The array must be terminated with a "null" entry {0,0,0,0}
 *
 * @return @ref wiced_result_t WICED_SUCCESS : on success.
 *                             WICED_ERROR   : if an error occurred
 */
extern wiced_result_t wiced_configure_device( const configuration_entry_t* config );


/** Re-runs device configuration
 *
 * @param[in] config  : An array of user configurable variables in configuration_entry_t format.
 *                      The array must be terminated with a "null" entry {0,0,0,0}
 *
 * @return @ref wiced_result_t WICED_SUCCESS : on success.
 *                             WICED_ERROR   : if an error occurred
 */
extern wiced_result_t wiced_reconfigure_device( const configuration_entry_t* config );

/** @} */


/*****************************************************************************/
/** @addtogroup netmgmt       Network management
 *  @ingroup mgmt
 *
 * Functions to manage the network interfaces
 *
 *  @{
 */
/*****************************************************************************/

/** Set network hostname in DCT
 *
 *  NOTE: This function will change the DCT.
 *
 * @param[in] name          : A null terminated string (Note: this will be truncated to a maximum of 32 characters)
 *
 * @return    @ref wiced_result_t
 */
extern wiced_result_t wiced_network_set_hostname( const char* name );

/** Get network hostname from DCT
 *
 * @param[in] name          : A pointer to a wiced_hostname_t object to store the hostname
 *
 * @return @ref wiced_result_t WICED_SUCCESS : on success.
 *                             WICED_ERROR   : if an error occurred
 */
extern wiced_result_t wiced_network_get_hostname( wiced_hostname_t* name );

/** Brings up a network interface
 *
 *
 * @param[in] interface     : The interface to bring up
 * @param[in] config        : The network IP configuration
 * @param[in] ip_settings   : Static IP settings that are mandatory for the AP interface,
 *                            but are optional for the STA interface
 *
 * @return @ref wiced_result_t WICED_SUCCESS : on success.
 *                             WICED_ERROR   : if an error occurred
 */
extern wiced_result_t wiced_network_up( wiced_interface_t interface, wiced_network_config_t config, const wiced_ip_setting_t* ip_settings );


/** Creates a network packet pool from a chunk of memory
 *
 * @param[in] memory_pointer : Pointer to a chunk of memory
 * @param[in] memory_size    : Size of the memory chunk
 * @param[in] direction      : Network packet reception or transmission
 *
 * @return @ref wiced_result_t WICED_SUCCESS : on success.
 *                             WICED_ERROR   : if an error occurred
 */
extern wiced_result_t wiced_network_create_packet_pool( uint8_t* memory_pointer, uint32_t memory_size, wiced_network_packet_dir_t direction );


/** Brings down a network interface
 *
 * @param[in] interface : The interface to bring down
 *
 * @return @ref wiced_result_t WICED_SUCCESS : on success.
 *                             WICED_ERROR   : if an error occurred
 */
extern wiced_result_t wiced_network_down( wiced_interface_t interface );


/** Suspends network services and disables all network related timers
 *
 * This function must only be called before entering deep sleep. Prior to calling this function, ensure all
 * network sockets are in a disconnected state. After calling this function, networking functions
 * should not be used. To resume network operation, use the @ref wiced_network_resume() function.
 *
 * Example usage:
 *   @code
 *      wiced_network_suspend();
 *      wiced_rtos_delay_milliseconds(DEEP_SLEEP_TIME);
 *      wiced_network_resume();
 *   @endcode
 *
 * @return    WICED_SUCCESS : Network services are suspended.
 *            WICED_ERROR   : Network services were unable to be suspended, active sockets still exist
 */
extern wiced_result_t wiced_network_suspend( void );


/** Resumes network services
 *
 * This function resumes network services after suspension
 * with the wiced_network_suspend() function. After calling this function, all network functions
 * are available for use.
 *
 * Example usage:
 *   @code
 *      wiced_network_suspend();
 *      wiced_rtos_delay_milliseconds(DEEP_SLEEP_TIME);
 *      wiced_network_resume();
 *   @endcode
 *
 * @param[in] : void
 *
 * @return @ref wiced_result_t WICED_SUCCESS : on success.
 *                             WICED_ERROR   : if an error occurred
 */
extern wiced_result_t wiced_network_resume( void );


/** Checks if a network interface is up at the 802.11 link layer
 *
 * @param[in] interface : The interface to check
 *
 * @return @ref wiced_bool_t
 */
extern wiced_bool_t wiced_network_is_up( wiced_interface_t interface );

/** Checks if a network interface is up at the IP layer
 *
 * @param[in] interface : The interface to check
 *
 * @return @ref wiced_result_t WICED_SUCCESS : on success.
 *                             WICED_ERROR   : if an error occurred
 */
extern wiced_bool_t wiced_network_is_ip_up( wiced_interface_t interface );

/** Brings up a network interface after deep-sleep
 *
 * @param[in] interface     : The interface to bring up
 * @param[in] config        : The network IP configuration
 * @param[in] ip_settings   : Static IP settings that are mandatory for the AP interface,
 *                            but are optional for the STA interface
 *
 * @return @ref wiced_result_t
 */
extern wiced_result_t wiced_network_resume_after_deep_sleep( wiced_interface_t interface, wiced_network_config_t config, const wiced_ip_setting_t* ip_settings );

/** Save packets before going into deep sleep.
 *
 * @note Currently this is implemented for 4390x platforms only.
 *
 * @param[in] buffer             : Pointer to the packet buffer to be saved.
 * @param[in] interface          : The network interface (AP or STA) to which the specified packet belongs.
 *
 * @return WICED_TRUE if the packet buffer is successfully saved, otherwise WICED_FALSE *
 */
wiced_bool_t wiced_deep_sleep_save_packet( wiced_buffer_t buffer, wwd_interface_t interface );

/** Turn off deep sleep packet buffering.  This is done on warm-boot to allow DHCP packets up the stack
 * without replaying all buffered traffic.
 *
 * @note Currently this is implemented for 4390x platforms only.
 *
 * @return
 */
void        wiced_deep_sleep_disable_packet_buffering( void );

/** Notify application that network interface is ready and push all saved packets up to stack.
 *
 * @note Currently this is implemented for 4390x platforms only.
 *
 * @return void
 */
void         wiced_deep_sleep_set_networking_ready( void );

/** Check whether there are packets pending before going to deep sleep
 *
 * @note Currently this is implemented for 4390x platforms only.
 *
 * @param[in] interface : The network interface (AP or STA) to be checked for pending packets
 *
 * @return WICED_FALSE if any packets pending, otherwise WICED_TRUE *
 */
wiced_bool_t wiced_deep_sleep_is_networking_idle( wiced_interface_t interface );

/** Reads default network interface from DCT and brings up network
 *
 * @param[out]              : Result from reading DCT is stored in interface
 * @param[in] ip_settings   : Static IP settings that are mandatory for the AP interface,
 *                            but are not used for the STA or ETHERNET interfaces.
 * @return @ref wiced_result_t WICED_SUCCESS : on success.
 *                             WICED_ERROR   : if an error occurred
 */
extern wiced_result_t wiced_network_up_default( wiced_interface_t* interface, const wiced_ip_setting_t* ap_ip_settings );

/** Returns the default ready interface
 *
 * @param[out] interface : Pointer to variable for returning the default ready interface.

 * @return @ref wiced_result_t WICED_SUCCESS : on success.
 *                             WICED_ERROR   : if an error occurred
 */
extern wiced_result_t wiced_get_default_ready_interface( wiced_interface_t* interface );

/** Returns the currently active interface for iperf application
 *
 * @return @ref wiced_result_t
 */
extern wiced_interface_t wiced_get_interface( void );
/** Sets the currently active interface for iperf application
 *
 * @param[in] interface : the interface to set
 *
 * @return @ref wiced_result_t
 */
extern wiced_result_t wiced_set_interface( wiced_interface_t interface );

/** Register callback function/s that gets called when a change in network link status occurs
 *
 * @param[in] link_up_callback   : The optional callback function to register for the link up event
 * @param[in] link_up_user_data  : The user data to be provided in the link up callback function
 * @param[in] link_down_callback : The optional callback function to register for the link down event
 * @param[in] link_down_user_data: The user data to be provided in the link down callback function
 * @param[in] interface          : The interface to use for registering the callbacks.
 *
 * @return    WICED_SUCCESS : on success.
 * @return    WICED_ERROR   : if an error occurred with any step
 */
extern wiced_result_t wiced_network_register_link_callback( wiced_network_link_callback_t link_up_callback, void *link_up_user_data, wiced_network_link_callback_t link_down_callback, void *link_down_user_data, wiced_interface_t interface );

/** De-register network link status callback function/s
 *
 * @param[in] link_up_callback   : the optional callback function to deregister for the link up event
 * @param[in] link_down_callback : the optional callback function to deregister for the link down event
 * @param[in] interface          : The interface to use for de-registering the callbacks.
 *
 * @return @ref wiced_result_t WICED_SUCCESS : on success.
 *                             WICED_ERROR   : if an error occurred
 */
extern wiced_result_t wiced_network_deregister_link_callback( wiced_network_link_callback_t link_up_callback, wiced_network_link_callback_t link_down_callback, wiced_interface_t interface );

/** Fetches list of IP-addresses of associated clients.
 *
 * @param[in] ip_address_list    : the list to be filled with IP-addresses of associated clients.
 *
 * @return @ref wiced_result_t
 */
extern wiced_result_t wiced_network_get_clients_ip_address_list( void* ip_address_list );
/** @} */

/*****************************************************************************/
/** @addtogroup wicedcore       Wiced Initialization
 *  @ingroup mgmt
 *
 * Functions to initialize WICED in a more modular way.
 * wiced_init()    =  wiced_core_init()   + wiced_wlan_connectivity_init()
 * wiced_deinit()  =  wiced_core_deinit() + wiced_wlan_connectivity_deinit()
 *
 *  @{
 */
/*****************************************************************************/

/** Initializes the core parts of WICED without starting any WLAN systems
 *
 * @param[in] : void
 *
 * @return @ref wiced_result_t WICED_SUCCESS : on success.
 *                             WICED_ERROR   : if an error occurred
 */
wiced_result_t wiced_core_init               ( void );

/** De-initializes the core parts of WICED without touching any WLAN systems
 *
 * @note: WLAN should be already de-inited when this function is called
 *
 * @param[in] : void
 *
 * @return @ref wiced_result_t WICED_SUCCESS : on success.
 *                             WICED_ERROR   : if an error occurred
 */
wiced_result_t wiced_core_deinit             ( void );

/** @} */



#ifdef __cplusplus
} /*extern "C" */
#endif
