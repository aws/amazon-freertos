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

/**
 * @file cy_wifi_notify.c
 * @brief Wi-Fi Interface.
 */

/* Socket and Wi-Fi interface includes. */
#include <FreeRTOS.h>
#include <task.h>
#include <iot_wifi.h>
#include <iot_wifi_common.h>

/* Wi-Fi Host driver includes. */
#include "whd.h"
#include "whd_wifi_api.h"
#include "whd_network_types.h"
#include "whd_buffer_api.h"

/* Board and Abstraction layer includes */
#include <cy_result.h>
#include "cy_worker_thread.h"

/* lwIP stack includes */
#include <lwipopts.h>
#include <lwip/netif.h>
#include <lwip/netifapi.h>
#include <cy_wifi_notify.h>

cy_mutex_t cy_app_cb_Mutex;
/*-----------------------------------------------------------*/
/* Macros/globals */
/*-----------------------------------------------------------*/
/** Maxmium number of callbacks can be registered */
#define CY_NETWORK_MAXIMUM_CALLBACKS_COUNT          (5)
#define WLC_EVENT_MSG_LINK                          (0x01)

#define WCM_HANDSHAKE_TIMEOUT_MS                    (3000)
#define DEFAULT_RETRY_BACKOFF_TIMEOUT_IN_MS         (1000)
#define MAX_RETRY_BACKOFF_TIMEOUT_IN_MS             (DEFAULT_RETRY_BACKOFF_TIMEOUT_IN_MS * 32)
#define JOIN_RETRY_ATTEMPTS                         (3)
#define WORKER_THREAD_PRIORITY                      (CY_RTOS_PRIORITY_ABOVENORMAL)
#define WORKER_THREAD_STACK_SIZE                    (10 * 1024)

cy_worker_thread_info_t           cy_worker_thread;
cy_timer_t cy_sta_handshake_timer;
cy_timer_t cy_sta_retry_timer;
bool cy_sta_link_up = false;
cy_ap_details cy_connected_ap_details;
whd_security_t   cy_sta_security_type;
static const whd_event_num_t  cy_whd_link_events[] = {WLC_E_LINK, WLC_E_DEAUTH_IND, WLC_E_DISASSOC_IND, WLC_E_PSK_SUP, WLC_E_NONE};
static uint16_t cy_event_handler_index    = 0xFF;
static bool cy_link_up_event_received     = false;
static bool cy_too_many_ie_error          = false;
static uint32_t cy_retry_backoff_timeout  = DEFAULT_RETRY_BACKOFF_TIMEOUT_IN_MS;
static cy_network_event_callback_t cy_network_event_handler[CY_NETWORK_MAXIMUM_CALLBACKS_COUNT];

/*-----------------------------------------------------------*/
/* Function prototype */
/*-----------------------------------------------------------*/
static void cy_network_invoke_app_callbacks(cy_network_event_t event_type);
static void *cy_link_events_handler(whd_interface_t ifp, const whd_event_header_t *event_header, const uint8_t *event_data, void *handler_user_data);
static void cy_net_link_up(void);
static void cy_net_link_down(void);
static void cy_handshake_error_callback(void);
void cy_handshake_timeout_handler(cy_timer_callback_arg_t arg);
void cy_handshake_retry_timer(cy_timer_callback_arg_t arg);

/*-----------------------------------------------------------*/
/* Functions */
/*-----------------------------------------------------------*/
static void cy_notify_ip_change_handler(void* arg)
{
    UNUSED_PARAMETER(arg);
    struct netif *net = cy_lwip_get_interface();
    /* Callback functions are called for IP change notification from LWIP */
    if (net->ip_addr.u_addr.ip4.addr != 0)
    {
        configPRINTF(("Notify application that IP is changed!\n"));
        cy_network_invoke_app_callbacks(CY_NETWORK_EVENT_IP_CHANGED);
    }
}

static cy_rslt_t cy_lwip_dhcp_renew()
{
    /* Invalidate ARP entries */
    if (netifapi_netif_common(cy_lwip_get_interface(), (netifapi_void_fn) etharp_cleanup_netif, NULL ) != CY_RSLT_SUCCESS)
    {
        return CY_RSLT_TYPE_ERROR;
    }

    /* DHCP renewal*/
    if (netifapi_netif_common(cy_lwip_get_interface(), (netifapi_void_fn)dhcp_renew, NULL) != CY_RSLT_SUCCESS)
    {
        return CY_RSLT_TYPE_ERROR;
    }
    /* Add 50ms delay for DHCP renewal to be completed */
    cy_rtos_delay_milliseconds(50);
    return CY_RSLT_SUCCESS;
}

static void cy_link_up_handler(void* arg)
{
    UNUSED_PARAMETER(arg);
    if (cy_lwip_bringup_interface() != CY_RSLT_SUCCESS)
    {
       configPRINTF(("cy_lwip_bringup_interface failed !\n"));
       cy_network_invoke_app_callbacks(CY_NETWORK_EVENT_LINKUP_FAILURE);
       return;
    }
    configPRINTF(("Notify application that network is connected again!\n"));
    cy_network_invoke_app_callbacks(CY_NETWORK_EVENT_CONNECTED);
}

static void cy_link_up_renew_handler(void* arg)
{
    UNUSED_PARAMETER( arg );
    if (cy_lwip_dhcp_renew() != CY_RSLT_SUCCESS)
    {
        configPRINTF(("cy_lwip_dhcp_renew failed !\n"));
        cy_network_invoke_app_callbacks(CY_NETWORK_DHCP_RESTART_FAILURE);
    }
}

static void cy_net_link_up( void )
{
    cy_rslt_t res = CY_RSLT_SUCCESS;

    /* This flag will discard any stale link-up event generated. It will be set to false when link-down notification
     * is sent to application */
    if(!cy_sta_link_up)
    {
        /* thread enqueue failure will be result in socket level failure and application needs to disconnect/connect
         * back to get the link up again
         */
        if((res = cy_worker_thread_enqueue(&cy_worker_thread, cy_link_up_handler, NULL)) != CY_RSLT_SUCCESS)
        {
            configPRINTF((" L%d : %s() : ERROR : Failed to handle link up event. Err = [%lu]\r\n", __LINE__, __FUNCTION__, res));
            return;
        }
        cy_sta_link_up = true;

    }
    else
    {
        configPRINTF(("Executing DHCP renewal \n"));
        /* This condition will be hit when handshake fails and wcm reconnection is successful through retry timer.
         * thread enqueue failure will be result in socket level failure and application needs to disconnect/connect
         * back to get the link up again
         */
        if((res = cy_worker_thread_enqueue(&cy_worker_thread, cy_link_up_renew_handler, NULL)) != CY_RSLT_SUCCESS)
        {
            configPRINTF((" L%d : %s() : ERROR : Failed to handle link up event. Err = [%lu]\r\n", __LINE__, __FUNCTION__, res));
            return;
        }
    }

}

static void cy_net_link_down(void)
{
    if (cy_sta_link_up)
    {
        if (cy_lwip_bringdown_interface() != CY_RSLT_SUCCESS)
        {
            configPRINTF(("cy_lwip_bringdown_interface failed \n"));
            cy_network_invoke_app_callbacks(CY_NETWORK_EVENT_LINKDOWN_FAILURE);
            return;
        }
        configPRINTF(("Notify application that network is disconnected!\n"));
        cy_network_invoke_app_callbacks(CY_NETWORK_EVENT_DISCONNECTED);
        cy_sta_link_up = false;
    }
}

void cy_handshake_timeout_handler(cy_timer_callback_arg_t arg)
{
    UNUSED_PARAMETER( arg );

    /* This event is for handshake timeout. Will Ignore the timer error and continue with stop and rejoin the AP.
    * Also same timer handle is used, this shouldn't trigger extra timer. The stop may fail, if the timer object
    * passed is invalid (or) the FreeRTOS timer stop call fails due to basic system state has gone bad.*/
    if (cy_rtos_stop_timer(&cy_sta_handshake_timer) != CY_RSLT_SUCCESS)
    {
        configPRINTF(("cy_rtos_stop_timer failed for cy_sta_handshake_timer \n"));
    }
    if (cy_rtos_stop_timer(&cy_sta_retry_timer) != CY_RSLT_SUCCESS)
    {
        configPRINTF(("cy_rtos_stop_timer failed for cy_sta_retry_timer \n"));
    }
    cy_retry_backoff_timeout = DEFAULT_RETRY_BACKOFF_TIMEOUT_IN_MS;
    cy_handshake_error_callback();
}

void cy_handshake_retry_timer(cy_timer_callback_arg_t arg)
{
    UNUSED_PARAMETER(arg);
    cy_handshake_error_callback();
}

static void cy_handshake_error_callback(void)
{
    cy_rslt_t res;
    uint8_t   retries;

    /* This event is for handshake timeout. Will Ignore the timer error and continue with stop and rejoin the AP.
    * Also same timer handle is used, this shouldn't trigger extra timer. The stop may fail, if the timer object
    * passed is invalid (or) the FreeRTOS timer stop call fails due to basic system state has gone bad.*/
    if (cy_rtos_stop_timer(&cy_sta_retry_timer) != CY_RSLT_SUCCESS)
    {
        configPRINTF(("cy_rtos_stop_timer failed for cy_sta_retry_timer \n"));
    }
    cy_retry_backoff_timeout = DEFAULT_RETRY_BACKOFF_TIMEOUT_IN_MS;

    /* De-register the link event handler */
    whd_wifi_deregister_event_handler(primaryInterface, cy_event_handler_index);

    /* Explicitly leave AP and then rejoin */
    whd_wifi_leave(primaryInterface);

    /* 100ms delay added for wlan disassociate to be completed */
    cy_rtos_delay_milliseconds(100);

    for(retries = 0; retries < JOIN_RETRY_ATTEMPTS; retries++)
    {
        cy_rslt_t join_result;
        /** Join to Wi-Fi AP **/
        join_result = whd_wifi_join(primaryInterface, &cy_connected_ap_details.SSID, cy_connected_ap_details.security, cy_connected_ap_details.key, cy_connected_ap_details.keylen);

        if(join_result == CY_RSLT_SUCCESS)
        {
            cy_net_link_up();
            cy_sta_security_type = cy_connected_ap_details.security;

            /* Register for Link events*/
            whd_management_set_event_handler(primaryInterface, cy_whd_link_events, cy_link_events_handler, NULL, &cy_event_handler_index);
            /* Reset retry-backoff-timeout */
            cy_retry_backoff_timeout = DEFAULT_RETRY_BACKOFF_TIMEOUT_IN_MS;
            return;
        }
    }

    /* Register retry with network worker thread */
    configPRINTF((" L%d : %s() : Retrying to join with back-off [%ld secs]\r\n", __LINE__, __FUNCTION__, cy_retry_backoff_timeout));
    res = cy_rtos_start_timer(&cy_sta_retry_timer, cy_retry_backoff_timeout);
    if (res != CY_RSLT_SUCCESS)
    {
        configPRINTF((" L%d : %s() : ERROR : Failed in join retry with back-off. Err = [%ld]\r\n", __LINE__, __FUNCTION__, res));
    }
    /* Update backoff timeout */
    cy_retry_backoff_timeout = (cy_retry_backoff_timeout < MAX_RETRY_BACKOFF_TIMEOUT_IN_MS)? (uint32_t)(cy_retry_backoff_timeout * 2) : MAX_RETRY_BACKOFF_TIMEOUT_IN_MS;

}

static void* cy_link_events_handler(whd_interface_t ifp, const whd_event_header_t *event_header, const uint8_t *event_data, void *handler_user_data)
{
    switch (event_header->event_type)
    {
        case WLC_E_LINK:
            if ((event_header->flags & WLC_EVENT_MSG_LINK) != 0)
            {
                switch (cy_sta_security_type)
                {
                    case WHD_SECURITY_OPEN:
                    case WHD_SECURITY_IBSS_OPEN:
                    case WHD_SECURITY_WPS_SECURE:
                    case WHD_SECURITY_WEP_PSK:
                    case WHD_SECURITY_WEP_SHARED:
                    {
                        /* Advertise link-up immediately as no EAPOL is required */
                        cy_net_link_up();
                        break;
                    }
                    case WHD_SECURITY_WPA_TKIP_PSK:
                    case WHD_SECURITY_WPA_AES_PSK:
                    case WHD_SECURITY_WPA_MIXED_PSK:
                    case WHD_SECURITY_WPA2_AES_PSK:
                    case WHD_SECURITY_WPA2_TKIP_PSK:
                    case WHD_SECURITY_WPA2_MIXED_PSK:
                    case WHD_SECURITY_WPA_TKIP_ENT:
                    case WHD_SECURITY_WPA_AES_ENT:
                    case WHD_SECURITY_WPA_MIXED_ENT:
                    case WHD_SECURITY_WPA2_TKIP_ENT:
                    case WHD_SECURITY_WPA2_AES_ENT:
                    case WHD_SECURITY_WPA2_MIXED_ENT:
                    case WHD_SECURITY_WPA2_FBT_PSK:
                    case WHD_SECURITY_WPA2_FBT_ENT:
                    case WHD_SECURITY_WPA3_SAE:
                    case WHD_SECURITY_WPA3_WPA2_PSK:
                    {
                        cy_link_up_event_received = WHD_TRUE;
                        /* Start a timer and wait for WLC_E_PSK_SUP event */
                        if (cy_rtos_start_timer(&cy_sta_handshake_timer, WCM_HANDSHAKE_TIMEOUT_MS) != CY_RSLT_SUCCESS)
                        {
                            configPRINTF(("cy_rtos_start_timer failed for cy_sta_handshake_timer \r\n"));
                            /* Even though the timer fails, will be getting the event WLC_E_PSK_SUP where network link-up is done.
                             * hence ignoring this failure. */
                        }
                        break;
                    }
                    case WHD_SECURITY_UNKNOWN:
                    case WHD_SECURITY_FORCE_32_BIT:
                    default:
                    {
                        configPRINTF(("Bad Security type\r\n"));
                        break;
                    }
                }
            }
            else
            {
                /* Check if the link down event is followed by too many RSN IE Error. If yes, try join again */
                if (cy_too_many_ie_error)
                {
                    /* Try to join the AP again */
                    cy_handshake_timeout_handler(0);

                    /* Clear the error flag */
                    cy_too_many_ie_error = WHD_FALSE;
                }
                else
                {
                    /* Stop the handshake timer */
                    /* This event is to bring the link down. Will Ignore this error and continue bringing link down.
                    * Also same timer handle is used, this shouldn't trigger extra timer. The stop may fail, if the timer object
                    * passed is invalid (or) the FreeRTOS timer stop call fails due to basic system state has gone bad.*/
                    if (cy_rtos_stop_timer(&cy_sta_handshake_timer) != CY_RSLT_SUCCESS)
                    {
                        configPRINTF(("cy_rtos_stop_timer failed for cy_sta_handshake_timer \n"));
                    }
                    /* Stop the retry timer */
                    if (cy_rtos_stop_timer(&cy_sta_retry_timer) != CY_RSLT_SUCCESS)
                    {
                        configPRINTF(("cy_rtos_stop_timer failed for cy_sta_retry_timer \n"));
                    }
                    cy_retry_backoff_timeout = DEFAULT_RETRY_BACKOFF_TIMEOUT_IN_MS;

                    cy_net_link_down();

                    /* Clear the global variable cy_connected_ap_details since the link is down. */
                    memset( &cy_connected_ap_details, 0, sizeof( cy_connected_ap_details ) );
                }
            }
            break;

        case WLC_E_DEAUTH_IND:
        case WLC_E_DISASSOC_IND:
            cy_net_link_down();
            break;

        case WLC_E_PSK_SUP:
            {
                configPRINTF(("recieved WLC_E_PSK_SUP \n"));
                /* WLC_E_PSK_SUP is received. Check for status and reason. */
                if ( event_header->status == WLC_SUP_KEYED && event_header->reason == WLC_E_SUP_OTHER && cy_link_up_event_received == WHD_TRUE )
                {
                    /* Successful WPA key exchange. Stop timer and announce link up event to application */

                    /* Stop handshake  */
                    /* This event is to bring the link up. Will Ignore this error and continue bringing link up.
                    * Also same timer handle is used, this shouldn't trigger extra timer. The stop may fail, if the timer object
                    * passed is invalid (or) the FreeRTOS timer stop call fails due to basic system state has gone bad.*/
                    if (cy_rtos_stop_timer(&cy_sta_handshake_timer) != CY_RSLT_SUCCESS)
                    {
                        configPRINTF(("cy_rtos_stop_timer failed for cy_sta_handshake_timer \n"));
                    }
                    /* Stop retry Timer */
                    if (cy_rtos_stop_timer(&cy_sta_retry_timer) != CY_RSLT_SUCCESS)
                    {
                        configPRINTF(("cy_rtos_stop_timer failed for cy_sta_retry_timer \n"));
                    }

                    cy_retry_backoff_timeout = DEFAULT_RETRY_BACKOFF_TIMEOUT_IN_MS;

                    cy_net_link_up();
                    cy_link_up_event_received = WHD_FALSE;

                    /* Clear the global variable cy_connected_ap_details since the link is up. */
                    memset( &cy_connected_ap_details, 0, sizeof( cy_connected_ap_details ) );
                }
                else if ( event_header->reason == WLC_E_SUP_MSG3_TOO_MANY_IE )
                {
                    /* Wifi firmware will do disassoc internally and will not retry to join the AP.
                     * Set a flag to indicate the join should be retried (from the host side).
                     */
                    cy_too_many_ie_error = WHD_TRUE;
                }
                break;
            }

        default:
            configPRINTF(( "Received event which was not registered\n"));
            break;
    }
    return handler_user_data;
}

cy_rslt_t cy_network_register_event_callback(cy_network_event_callback_t event_callback)
{
    uint8_t i;
    if( event_callback == NULL )
    {
        return CY_WIFI_FAILURE;
    }

    if (cy_rtos_get_mutex(&cy_app_cb_Mutex, wificonfigMAX_SEMAPHORE_WAIT_TIME_MS) == CY_RSLT_SUCCESS)
    {
        for ( i = 0; i < CY_NETWORK_MAXIMUM_CALLBACKS_COUNT; i++ )
        {
            if ( cy_network_event_handler[i] == NULL )
            {
                cy_network_event_handler[i] = event_callback;
                cy_rtos_set_mutex(&cy_app_cb_Mutex);
                return CY_RSLT_SUCCESS;
            }
        }
    }
    cy_rtos_set_mutex(&cy_app_cb_Mutex);
    configPRINTF(("Out of CY_NETWORK_MAXIMUM_CALLBACKS_COUNT \r\n"));
    return CY_WIFI_FAILURE;
}
/*-----------------------------------------------------------*/

cy_rslt_t cy_network_deregister_event_callback(cy_network_event_callback_t event_callback)
{
    uint8_t i;
    if( event_callback == NULL )
    {
        return CY_WIFI_FAILURE;
    }

    if (cy_rtos_get_mutex(&cy_app_cb_Mutex, wificonfigMAX_SEMAPHORE_WAIT_TIME_MS) == CY_RSLT_SUCCESS)
    {
        for ( i = 0; i < CY_NETWORK_MAXIMUM_CALLBACKS_COUNT; i++ )
        {
            if ( cy_network_event_handler[i] == event_callback )
            {
                memset( &cy_network_event_handler[i], 0, sizeof( cy_network_event_callback_t ) );
                cy_rtos_set_mutex(&cy_app_cb_Mutex);
                return CY_RSLT_SUCCESS;
            }
        }
    }
    cy_rtos_set_mutex(&cy_app_cb_Mutex);
    configPRINTF(("Unable to find callback to deregister \r\n"));
    return CY_WIFI_FAILURE;
}
/*-----------------------------------------------------------*/

static void cy_network_invoke_app_callbacks(cy_network_event_t event_type)
{
    if (cy_rtos_get_mutex(&cy_app_cb_Mutex, wificonfigMAX_SEMAPHORE_WAIT_TIME_MS) == CY_RSLT_SUCCESS)
    {
        for ( int i = 0; i < CY_NETWORK_MAXIMUM_CALLBACKS_COUNT; i++ )
        {
            if ( cy_network_event_handler[i] != NULL )
            {
                cy_network_event_handler[i](event_type);
            }
        }
    }
    cy_rtos_set_mutex(&cy_app_cb_Mutex);

}
/*-----------------------------------------------------------*/

void cy_network_ip_change_callback (struct netif *netif)
{
    UNUSED_PARAMETER(netif);
    cy_rslt_t result;
    /* thread enqueue failure will be result in socket level failure and application needs to disconnect/connect
     * back to get the link up again
     */
    result = cy_worker_thread_enqueue(&cy_worker_thread, cy_notify_ip_change_handler, NULL);
    if (result != CY_RSLT_SUCCESS)
    {
        configPRINTF((" L%d : %s() : Failed to send async event to n/w worker thread. Err = [%lu]\r\n", __LINE__, __FUNCTION__, result));
    }
}

cy_rslt_t cy_wifi_link_event_register( whd_interface_t interface )
{
    cy_rslt_t res = CY_RSLT_SUCCESS;
    /* Register for Link events*/
    res = whd_management_set_event_handler(interface, cy_whd_link_events, cy_link_events_handler, NULL, &cy_event_handler_index);
    return res;
}

void cy_wifi_link_event_deregister( whd_interface_t interface )
{
    /* Deregister the link event handler */
    whd_wifi_deregister_event_handler(interface, cy_event_handler_index);
}

cy_rslt_t cy_wifi_worker_thread_create( void )
{
    cy_worker_thread_params_t params;
    memset(&params, 0, sizeof(params));
    params.name = "IOT-Wifi-Worker";
    params.priority = WORKER_THREAD_PRIORITY;
    params.stack = NULL;
    params.stack_size = WORKER_THREAD_STACK_SIZE;
    params.num_entries = 0;

    if (cy_rtos_init_timer(&cy_sta_handshake_timer, CY_TIMER_TYPE_ONCE, (cy_timer_callback_t)cy_handshake_timeout_handler, 0) != CY_RSLT_SUCCESS)
    {
        configPRINTF((" cy_rtos_init_timer failed for handshake_timeout_handler \r\n"));
        return CY_WIFI_FAILURE;
    }
    if (cy_rtos_init_timer(&cy_sta_retry_timer, CY_TIMER_TYPE_ONCE, (cy_timer_callback_t)cy_handshake_retry_timer, 0) != CY_RSLT_SUCCESS)
    {
        configPRINTF((" cy_rtos_init_timer failed for cy_handshake_retry_timer \r\n"));
        cy_rtos_deinit_timer(&cy_sta_handshake_timer);
        return CY_WIFI_FAILURE;
    }
    /* create a worker thread */
    if (cy_worker_thread_create(&cy_worker_thread, &params) != CY_RSLT_SUCCESS)
    {
        cy_rtos_deinit_timer(&cy_sta_handshake_timer);
        cy_rtos_deinit_timer(&cy_sta_retry_timer);
        return CY_WIFI_FAILURE;
    }
    return CY_RSLT_SUCCESS;
}

void cy_wifi_worker_thread_delete( void )
{
    if (cy_rtos_deinit_timer(&cy_sta_handshake_timer) != CY_RSLT_SUCCESS)
    {
        configPRINTF((" cy_rtos_deinit_timer failed for cy_sta_handshake_timer \r\n"));
        configASSERT(0);
    }
    if (cy_rtos_deinit_timer(&cy_sta_retry_timer) != CY_RSLT_SUCCESS)
    {
        configPRINTF((" cy_rtos_deinit_timer failed for cy_sta_retry_timer \r\n"));
        configASSERT(0);
    }
    cy_retry_backoff_timeout = DEFAULT_RETRY_BACKOFF_TIMEOUT_IN_MS;
    if (cy_worker_thread_delete(&cy_worker_thread) != CY_RSLT_SUCCESS)
    {
        configPRINTF((" cy_worker_thread_delete failed \r\n"));
        configASSERT(0);
    }
}
