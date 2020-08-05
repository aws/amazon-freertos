/*
 * Copyright 2019-2020 Cypress Semiconductor Corporation
 * SPDX-License-Identifier: Apache-2.0
 * 
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 * 
 *     http://www.apache.org/licenses/LICENSE-2.0
 * 
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

/** @file
 * 
 * This is a collection of network helper functions which would be used by various Cypress Middleware libraries.
 * 
 */
#pragma once

#include <stdint.h>
#include <stdbool.h>


#if defined(__cplusplus)
extern "C" {
#endif

#define cy_assert( error_string, assertion )         do { (void)(assertion); } while(0)

typedef uintptr_t cy_nw_ip_interface_t;


/******************************************************
 *                   Enumerations
 ******************************************************/
/******************************************************************************/
/** \addtogroup group_nwhelper_enums *//** \{ */
/******************************************************************************/
/** IP version */
typedef enum nw_ip_version
{
    NW_IP_IPV4 = 4,          /**< IPv4 version */
    NW_IP_IPV6 = 6,          /**< IPv6 version */
    NW_IP_INVALID_IP = (-1), /**< Invalid IP version */
} cy_nw_ip_version_t;

/** Network interface type */
typedef enum
{
    CY_NW_INF_TYPE_WIFI = 0, /**< Wi-Fi network interface */
    CY_NW_INF_TYPE_ETH       /**< Ethernet network interface */
} cy_network_interface_type_t;

/** \} */

/******************************************************************************/
/** \addtogroup group_nwhelper_structures *//** \{ */
/******************************************************************************/
/** Network IP status change callback function
 *
 * @param[in] iface : Pointer to the network interface for which the callback is invoked.
 * @param[in] arg   : User data object provided during the status change callback registration.

 * @return none
 */
typedef void (cy_nw_ip_status_change_callback_func_t)(cy_nw_ip_interface_t iface, void *arg);

/** Network IP status change callback info */
typedef struct cy_nw_ip_status_change_callback
{
    cy_nw_ip_status_change_callback_func_t *cb_func; /**< IP address status change callback function */
    void *arg;                                    /**< User data */
    void *priv;                                   /**< NW interface */
} cy_nw_ip_status_change_callback_t;

/**
 * IP addr info
 */
typedef struct cy_nw_ip_address
{
    cy_nw_ip_version_t version; /**< IP version */

    union
    {
        uint32_t v4;         /**< IPv4 address info */
        uint32_t v6[4];      /**< IPv6 address info */
    } ip;                    /**< Union of IPv4 and IPv6 address info */
} cy_nw_ip_address_t;

/** Network interface object */
typedef void* cy_network_interface_object_t;

/** MAC Address info */
typedef struct cy_nw_ip_mac
{
    uint8_t    mac[6];              /**< MAC address                */
} cy_nw_ip_mac_t;

/** ARP Cache Entry info */
typedef struct cy_nw_arp_cache_entry
{
    cy_nw_ip_address_t    ip;         /**< IP address                 */
    cy_nw_ip_mac_t        mac;        /**< MAC address                */
} cy_nw_arp_cache_entry_t;

/**
 * Network interface info structure
 */
typedef struct
{
    cy_network_interface_type_t     type;     /**< Network interface type */
    cy_network_interface_object_t   object;   /**< Pointer to the network interface object */
} cy_network_interface_t;

/** \} */

/*****************************************************************************/
/**
 *
 *  @addtogroup group_nwhelper_func
 *
 * This is a collection of network helper functions which would be used by various Cypress Middleware libraries.
 *
 *  @{
 */
/*****************************************************************************/

/** Initialize status change callback
 *
 * Initialize @ref cy_nw_ip_status_change_callback_t instead of
 * directly manipulating the callback struct.
 *
 * @param[in, out] info : Pointer to network IP status change callback information structure which would be filled upon return
 * @param[in] cbf       : Pointer to callback function to be invoked during network status change
 * @param[in] arg       : User data object to be sent sent in the callback function
 *
 * @return none
 */
void cy_nw_ip_initialize_status_change_callback(cy_nw_ip_status_change_callback_t *info, cy_nw_ip_status_change_callback_func_t *cbf, void *arg);

/** Retrieves the IPv4 address for an interface
 *
 * Retrieves the IPv4 address for an interface (AP or STA) if it
 * exists.
 *
 * @param[in]  iface : Pointer to network interface object
 * @param[out] addr  : Pointer to the IP information sturcture in which the results to be stored
 *
 * @return true  : if IP address is present
 * @return false : otherwise
 */
bool cy_nw_ip_get_ipv4_address(cy_nw_ip_interface_t iface, cy_nw_ip_address_t *addr);

/** Convert IPv4 string to an IP address structure.
 *
 * @param[in]  ip_str  :  IPv4 address string.
 * @param[out] address :  Pointer to the IP info structure in which the IPv4 address to be stored
 *
 * @return    0 : if successful
 * @return   -1 : if failed
 */
int cy_nw_str_to_ipv4(const char *ip_str, cy_nw_ip_address_t *address);

/** Registers for callback function to be invoked during IP status change
 *
 * @param[in] iface : Pointer to network interface object
 * @param[in] info  : Pointer to the status change information structure
 *
 * @return none
 */
void cy_nw_ip_register_status_change_callback(cy_nw_ip_interface_t iface, cy_nw_ip_status_change_callback_t *info);

/** Un-registers IP status change callback
 *
 * @param[in] iface : Pointer to network interface object
 * @param[in] info  : Pointer to the status change information structure
 *
 * @return none
 */
void cy_nw_ip_unregister_status_change_callback(cy_nw_ip_interface_t iface, cy_nw_ip_status_change_callback_t *info);

/** Clears the ARP cache for the interface
 * NOTE: in LwIP, we need the netif (NetworkInterface) to do things, we can find using wifi interface.
 *
 * @param[in] iface : Pointer to network interface object
 *
 *  @return 0 : success
 *          1 : fail
 */
int cy_nw_host_arp_cache_clear( cy_nw_ip_interface_t iface );

/** Gets the ARP cache list for the interface
 *
 * @param[in]  iface      : Pointer to network interface object
 * @param[in, out] list   : Pointer to @ref cy_nw_arp_cache_entry_t array
 * @param[in]  count      : Number of entries in the array passed in `list`
 * @param[in, out] filled : Pointer to get the number of entries filled in the array pointed by 'list'
 *
 * @return  0 : success
 *          1 : fail
 */
int cy_nw_host_arp_cache_get_list( cy_nw_ip_interface_t iface, cy_nw_arp_cache_entry_t *list, uint32_t count, uint32_t *filled );

/** Send ARP request
 * NOTE: in LwIP, we need the netif (NetworkInterface) to do things, we can find using wifi interface.
 *
 * @param[in]  iface     : Pointer to network interface object
 * @param[in]  ip_string : Pointer to the IPv4 address string (Ex: "192.168.1.1") to which the ARP request to be sent
 *
 * @return 0 : success
 *         1 : failed to send ARP request
 */
int cy_nw_host_send_arp_request( cy_nw_ip_interface_t iface, const char *ip_string );

/** GET time in milliseconds
 *
 * @return time in milliseconds
 * */
uint32_t cy_nw_get_time (void);

/** @} */

#if defined(__cplusplus)
}
#endif
