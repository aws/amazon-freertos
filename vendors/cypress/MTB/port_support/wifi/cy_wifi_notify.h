/*
 * Amazon FreeRTOS Wi-Fi V1.0.0
 * Copyright (C) 2018 Amazon.com, Inc. or its affiliates.  All Rights Reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of
 * this software and associated documentation files (the "Software"), to deal in
 * the Software without restriction, including without limitation the rights to
 * use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
 * the Software, and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 * http://aws.amazon.com/freertos
 * http://www.FreeRTOS.org
 */
#pragma once

/* Wi-Fi Host driver includes. */
#include <whd.h>
#include <whd_wifi_api.h>
#include <whd_network_types.h>

/* Variable to store Wi-Fi security which will be used during WHD event processing */
extern whd_security_t   cy_sta_security_type;

/* Variable to store station link status which will be used for LWIP bringup/down sequence */
extern bool cy_sta_link_up;

/*  This is mutex to sychronize application callback */
extern cy_mutex_t cy_app_cb_Mutex;

/* Connected AP details which is required for re association cases */
typedef struct
{
    whd_ssid_t SSID;
    uint8_t key[wificonfigMAX_PASSPHRASE_LEN];
    uint8_t keylen;
    whd_security_t security;
}cy_ap_details;

extern cy_ap_details cy_connected_ap_details;

#define CY_RSLT_MODULE_WIFI_BASE    (CY_RSLT_MODULE_MIDDLEWARE_BASE+11)
#define CY_WIFI_FAILURE             CY_RSLT_CREATE(CY_RSLT_TYPE_ERROR, CY_RSLT_MODULE_WIFI_BASE, 1)

/**
 * @brief Enum types representing different event changes.
 */
typedef enum
{
    CY_NETWORK_EVENT_CONNECTED = 0,     /**< Denotes STA connected to AP */
    CY_NETWORK_EVENT_DISCONNECTED,      /**< Denotes STA disconnected with AP */
    CY_NETWORK_EVENT_IP_CHANGED,        /**< Denotes IP address change */
    CY_NETWORK_EVENT_LINKUP_FAILURE,    /**< Denotes Network link up failure */
    CY_NETWORK_EVENT_LINKDOWN_FAILURE,  /**< Denotes Network link down failure */
    CY_NETWORK_DHCP_RESTART_FAILURE     /**< Denotes DHCP restart failure */
} cy_network_event_t;

/**
 * Network event callback function pointer type, events are invoked when WHD posts events to application.
 * @param[in] event            : Network events.
 */
typedef void (*cy_network_event_callback_t)(cy_network_event_t event);

/**
 * Register for whd link events.
 *
 * @param[in]  interface  : pointer to whd interface.
 *
 * @return CY_RSLT_SUCCESS if whd link events register was successful, failure code otherwise.
*/
extern cy_rslt_t cy_wifi_link_event_register( whd_interface_t interface );

/**
 * De-register whd link events.
 *
 * @param[in]  interface  : pointer to whd interface.
 *
*/
extern void cy_wifi_link_event_deregister( whd_interface_t interface );

/**
 * Callback for link notification support
 *
 * @param[in]  netif  : The lwIP network interface.
 *
 */
extern void cy_network_ip_change_callback (struct netif *netif);

/**
 * Create worker thread for wifi link up/link down notification support.
 *
 */
extern cy_rslt_t cy_wifi_worker_thread_create( void );

/**
 * Deinit worker thread created after wifi off.
 *
*/
extern void cy_wifi_worker_thread_delete( void );

/**
 * Register an event callback to receive network events.
 *
 * @param[in]  event_callback  : Callback function to be invoked for event notification.
 *
 * @return CY_RSLT_SUCCESS if application callback registration was successful, failure code otherwise.
*/
extern cy_rslt_t cy_network_register_event_callback(cy_network_event_callback_t event_callback);

/**
 * De-register an event callback.
 *
 * @param[in]  event_callback  : Callback function to de-register from getting notifications.
 *
 * @return CY_RSLT_SUCCESS if application callback de-registration was successful, failure code otherwise.
*/
extern cy_rslt_t cy_network_deregister_event_callback(cy_network_event_callback_t event_callback);
