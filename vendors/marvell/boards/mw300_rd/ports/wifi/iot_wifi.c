/*
 * FreeRTOS Wi-Fi
 *
 * (C) Copyright 2018-2019 Marvell International Ltd. All Rights Reserved.
 *
 * MARVELL CONFIDENTIAL
 * The source code contained or described herein and all documents related to
 * the source code ("Material") are owned by Marvell International Ltd or its
 * suppliers or licensors. Title to the Material remains with Marvell
 * International Ltd or its suppliers and licensors. The Material contains
 * trade secrets and proprietary and confidential information of Marvell or its
 * suppliers and licensors. The Material is protected by worldwide copyright
 * and trade secret laws and treaty provisions. No part of the Material may be
 * used, copied, reproduced, modified, published, uploaded, posted,
 * transmitted, distributed, or disclosed in any way without Marvell's prior
 * express written permission.
 *
 * No license under any patent, copyright, trade secret or other intellectual
 * property right is granted to or conferred upon you by disclosure or delivery
 * of the Materials, either expressly, by implication, inducement, estoppel or
 * otherwise. Any license under such intellectual property rights must be
 * express and approved by Marvell in writing.
 */

/**
 * @file iot_wifi.c
 * @brief Wi-Fi Interface.
 */

/* Socket and Wi-Fi interface includes. */
#include "FreeRTOS.h"
#include "list.h"
#include "FreeRTOS_IP.h"
#include "FreeRTOS_DNS.h"
#include "iot_wifi.h"

/* Wi-Fi configuration includes. */
#include "aws_wifi_config.h"

#include <wlan.h>
#include <wm_wlan.h>
#include <wmlog.h>
#include <wm_net.h>

#define wm_wlan_e(...)                             \
    wmlog_e("wm_wlan", ##__VA_ARGS__)
#define wm_wlan_w(...)                             \
    wmlog_w("wm_wlan", ##__VA_ARGS__)
#define wm_wlan_d(...)                             \
    wmlog("wm_wlan", ##__VA_ARGS__)

static os_semaphore_t scan_protection_sem;

static os_semaphore_t wlan_init_sem;
static os_mutex_t wlan_mtx;

/*-----------------------------------------------------------*/

/**
 * @brief Function to set a memory block to zero.
 * The function sets memory to zero using a volatile pointer so that compiler
 * wont optimize out the function if the buffer to be set to zero is not used further.
 * 
 * @param pBuf Pointer to buffer to be set to zero
 * @param size Length of the buffer to be set zero
 */
static void prvMemzero( void * pBuf, size_t size )
{
    volatile uint8_t * pMem = pBuf;
    uint32_t i;

    for( i = 0U; i < size; i++ )
    {
        pMem[ i ] = 0U;
    }
}

/*-----------------------------------------------------------*/

static int wlan_event_callback(enum wlan_event_reason event, void *data)
{
    int ret;
    struct wlan_ip_config addr;
    char ip[16];

    /* After successful wlan subsytem initialization */
    if (event == WLAN_REASON_INITIALIZED) {
        wmprintf("WLAN_REASON_INITIALIZED\r\n");

        ret = os_mutex_create(&wlan_mtx, "wlanmtx",
                OS_MUTEX_INHERIT);
        if (ret != WM_SUCCESS) {
            return eWiFiFailure;
        }
        os_semaphore_put(&wlan_init_sem);
    }

    if (event == WLAN_REASON_SUCCESS) {
        ret = wlan_get_address(&addr);
        if (ret != WM_SUCCESS) {
            wmprintf("failed to get IP address\r\n");
            return 0;
        }

        net_inet_ntoa(addr.ipv4.address, ip);

        wmprintf("Connected to with IP = [%s]\r\n", ip);
    }

    if (event == WLAN_REASON_USER_DISCONNECT) {
	    wmprintf("Dis-connected\r\n");
    }

    return 0;
}

/*-----------------------------------------------------------*/
WIFIReturnCode_t WIFI_On( void )
{
    int rv;
    static bool wifi_init_done = false;

    if(wifi_init_done)
	return eWiFiSuccess;

    rv = os_semaphore_create(&wlan_init_sem, "wlansem");
    if (rv != WM_SUCCESS) {
            return eWiFiFailure;
    }

    /* Consume so that 'get' blocks when used later */
    os_semaphore_get(&wlan_init_sem, OS_WAIT_FOREVER);

    rv = wm_wlan_init();
    if (rv != WM_SUCCESS) {
	os_semaphore_delete(&wlan_init_sem);
	return eWiFiFailure;
    }

    rv = wlan_start(wlan_event_callback);
    if (rv != WM_SUCCESS) {
        os_semaphore_delete(&wlan_init_sem);
        return eWiFiFailure;
    }

    /* Wait till app framework is initialized */
    rv = os_semaphore_get(&wlan_init_sem, OS_WAIT_FOREVER);
    if (rv != WM_SUCCESS) {
        os_semaphore_delete(&wlan_init_sem);
        return eWiFiFailure;
    }

    os_semaphore_delete(&wlan_init_sem);

    wifi_init_done = true;
    return eWiFiSuccess;
}
/*-----------------------------------------------------------*/

WIFIReturnCode_t WIFI_Off( void )
{
    return eWiFiSuccess;
}
/*-----------------------------------------------------------*/

#define IS_SSID_LEN_VALID(x) \
        ((x) <=  wificonfigMAX_SSID_LEN && (x) > 0)
#define IS_PASSPHRASE_LEN_VALID(x) \
        ((x) <= wificonfigMAX_PASSPHRASE_LEN && (x) > 0)

WIFIReturnCode_t WIFI_ConnectAP( const WIFINetworkParams_t * const pxNetworkParams )
{
    int ret = eWiFiFailure;
    struct wlan_network home_nw;
    enum wlan_connection_state state;

    if (os_mutex_get(&wlan_mtx, OS_WAIT_FOREVER) != WM_SUCCESS)
           return eWiFiFailure;

    if(pxNetworkParams == NULL)
        goto fail;

    if (pxNetworkParams->ucSSIDLength == 0
        || pxNetworkParams->xSecurity == eWiFiSecurityNotSupported) {
        goto fail;
    }

    if (pxNetworkParams->xSecurity != eWiFiSecurityOpen
        && pxNetworkParams->xPassword.xWPA.ucLength == 0) {
        goto fail;
    }

    if (!IS_SSID_LEN_VALID(pxNetworkParams->ucSSIDLength) 
        || (pxNetworkParams->xSecurity != eWiFiSecurityOpen 
            && !IS_PASSPHRASE_LEN_VALID(pxNetworkParams->xPassword.xWPA.ucLength))) {
    goto fail;
    }

    if (wlan_get_connection_state(&state)) {
		wmprintf("Error: unable to get "
				"connection state\r\n");
	goto fail;
    }

    if (state == WLAN_CONNECTED) {
        int connect_retry = 200;
        ret = wlan_disconnect();
        if (ret != WM_SUCCESS)
            goto fail;

        do {
            /* wait for interface up */
            os_thread_sleep(os_msec_to_ticks(50));

            if (wlan_get_connection_state(&state)) {
            wmprintf("Error: unable to get "
                    "connection state\r\n");
                continue;
            }

            if (state == WLAN_DISCONNECTED || connect_retry == 0)
                break;
        } while (connect_retry--);

        if (state != WLAN_DISCONNECTED) {
            wmprintf("Error: Not connected.\r\n");
            goto fail;
        }
    }

    memset(&home_nw, 0, sizeof(home_nw));

    strncpy(home_nw.name, "aws_network", WLAN_NETWORK_NAME_MAX_LENGTH);

    /* Set SSID as passed by the user */
    if (pxNetworkParams->ucSSIDLength > 0)
    {
        char pcSSID[pxNetworkParams->ucSSIDLength + 1];
        memcpy(pcSSID, pxNetworkParams->ucSSID, pxNetworkParams->ucSSIDLength);
        pcSSID[pxNetworkParams->ucSSIDLength] = '\0';

        strlcpy(home_nw.ssid, pcSSID, IEEEtypes_SSID_SIZE + 1);
    }

    if(pxNetworkParams->xPassword.xWPA.ucLength > 0) {
        /* Ensure one extra character for NULL. */
        if( pxNetworkParams->xPassword.xWPA.ucLength >= WLAN_PSK_MAX_LENGTH )
        {
            goto fail;
        }

        memcpy(home_nw.security.psk, pxNetworkParams->xPassword.xWPA.cPassphrase, pxNetworkParams->xPassword.xWPA.ucLength);
        home_nw.security.psk[pxNetworkParams->xPassword.xWPA.ucLength] = '\0';

        /* Set the passphrase length */
        home_nw.security.psk_len = strlen(home_nw.security.psk);
    }

    home_nw.channel = pxNetworkParams->ucChannel;

    if (pxNetworkParams->xSecurity == eWiFiSecurityOpen)
        home_nw.security.type = WLAN_SECURITY_NONE;
    else if (pxNetworkParams->xSecurity == eWiFiSecurityWEP)
        home_nw.security.type = WLAN_SECURITY_WEP_OPEN;
    else if (pxNetworkParams->xSecurity == eWiFiSecurityWPA)
        home_nw.security.type = WLAN_SECURITY_WPA;
    else if (pxNetworkParams->xSecurity == eWiFiSecurityWPA2)
        home_nw.security.type = WLAN_SECURITY_WPA2;

    home_nw.type = WLAN_BSS_TYPE_STA;
    home_nw.role = WLAN_BSS_ROLE_STA;
    home_nw.ip.ipv4.addr_type = ADDR_TYPE_DHCP;

    ret = wlan_remove_network("aws_network");
    if (ret != WM_SUCCESS && ret != -WM_E_INVAL) {
	wmprintf(" Failed to add network %d\r\n", ret);
	goto fail;
    }

    ret = wlan_add_network(&home_nw);
    if (ret != WM_SUCCESS) {
	wmprintf(" Failed to add network %d\r\n", ret);
	goto fail;
    }

    int retry_cnt = 3;
    int connect_retry = 10;

    do {
        wmprintf("Connecting to %s .....\r\n", home_nw.ssid);

        ret = wlan_connect(home_nw.name);

        if (ret != 0) {
            wmprintf("Failed to connect %d\r\n", ret);
            goto retry;
        }

        connect_retry = 2000;

        do {
            /* wait for interface up */
            os_thread_sleep(os_msec_to_ticks(50));

            if (wlan_get_connection_state(&state)) {
		    wmprintf("Error: unable to get "
				    "connection state\r\n");
		    continue;
	    }

	    if (state == WLAN_CONNECTED || connect_retry == 0)
		    break;
	} while (connect_retry--);

	if (state != WLAN_CONNECTED) {
		wmprintf("Error: Not connected.\r\n");
		goto retry;
	}

        /* WIFI state connected. Clear the home_nw. */
        /* Use a private function to reset the memory block instead of memset, so that compiler wont optimize away the function call. */
        prvMemzero( &home_nw, sizeof( struct wlan_network ) );

	if (os_mutex_put(&wlan_mtx) != WM_SUCCESS)
            return eWiFiFailure;

        return eWiFiSuccess;
retry:
        retry_cnt--;
        wmprintf("Connect to AP FAIL ! Retrying ..... \r\n");

    } while (retry_cnt != 0);

    /* Retry exhausted. Clear the home_nw. */
    /* Use a private function to reset the memory block instead of memset, so that compiler wont optimize away the function call. */
    prvMemzero( &home_nw, sizeof( struct wlan_network ) );

    if (retry_cnt == 0) {
        os_thread_sleep(os_msec_to_ticks(500));

	wmprintf("Connection Failed !\r\n");

	ret = wlan_disconnect();
	if (ret != 0) {
	    wmprintf("Failed to disconnect \r\n");
	    if (os_mutex_put(&wlan_mtx) != WM_SUCCESS)
                return eWiFiFailure;
        return eWiFiFailure;
        }
        if (os_mutex_put(&wlan_mtx) != WM_SUCCESS)
            return eWiFiFailure;
        return eWiFiFailure;
    }

fail:
    /* Operation failed. Clear the home_nw. */
    /* Use a private function to reset the memory block instead of memset, so that compiler wont optimize away the function call. */
    prvMemzero( &home_nw, sizeof( struct wlan_network ) );

    if (os_mutex_put(&wlan_mtx) != WM_SUCCESS)
        return eWiFiFailure;

    return ret;
}
/*-----------------------------------------------------------*/

WIFIReturnCode_t WIFI_Disconnect( void )
{
    int ret = eWiFiFailure;
    enum wlan_connection_state state;
    int connect_retry = 200;

    if (os_mutex_get(&wlan_mtx, OS_WAIT_FOREVER) != WM_SUCCESS)
           return eWiFiFailure;

    ret = wlan_disconnect();
    if (ret != WM_SUCCESS)
	    goto fail;

    do {
        /* wait for interface up */
        os_thread_sleep(os_msec_to_ticks(50));

	if (wlan_get_connection_state(&state)) {
	    wmprintf("Error: unable to get "
			"connection state\r\n");
	    continue;
	}

            if (state == WLAN_DISCONNECTED || connect_retry == 0)
	    break;
    } while (connect_retry--);

    if (state != WLAN_DISCONNECTED) {
	wmprintf("Error: Not connected.\r\n");
	goto fail;
    }
fail:

    if (os_mutex_put(&wlan_mtx) != WM_SUCCESS)
           return eWiFiFailure;

    return ret;
}
/*-----------------------------------------------------------*/

WIFIReturnCode_t WIFI_Reset( void )
{
    return eWiFiNotSupported;
}
/*-----------------------------------------------------------*/

#define CONFIG_MAX_AP_ENTRIES 20
#define MAX_SITES CONFIG_MAX_AP_ENTRIES

struct site_survey {
    struct wlan_scan_result sites[MAX_SITES];
    unsigned int num_sites;
    os_mutex_t lock;
} survey;

static int prov_handle_scan_results(unsigned int count)
{
    int i, j = 0;

    /* WLAN firmware does not provide Noise Floor (NF) value with
     * every scan result. We call the function below to update a
     * global NF value so that /sys/scan can provide the latest NF value */
    wlan_get_current_nf();

    /* lock the scan results */
    if (os_mutex_get(&survey.lock, OS_WAIT_FOREVER) != WM_SUCCESS)
        return 0;

    /* clear out and refresh the site survey table */
    memset(survey.sites, 0, sizeof(survey.sites));

    /*
     * Loop through till we have results to process or
     * run out of space in survey.sites
     */
    for (i = 0; i < count && j < MAX_SITES; i++) {
        if (wlan_get_scan_result(i, &survey.sites[j]))
            break;

        if (survey.sites[j].ssid[0] != 0)
            j++;
    }
    survey.num_sites = j;

    /* unlock the site survey table */
    os_mutex_put(&survey.lock);
    /* Give up the semaphore to process next scan/prov_stop request */
    os_semaphore_put(&scan_protection_sem);
    return 0;
}

#define DEFAULT_SCAN_PROTECTION_TIMEOUT 1000

WIFIReturnCode_t WIFI_Scan( WIFIScanResult_t * pxBuffer,
                            uint8_t ucNumNetworks )
{
    int ret = 0;
    int i = 0;

    if (pxBuffer == NULL) {
           return eWiFiFailure;
    }

    memset(pxBuffer, 0, sizeof(WIFIScanResult_t) * ucNumNetworks);

    ret = os_semaphore_create(&scan_protection_sem, "scanprotsem");
    if (ret != WM_SUCCESS) {
        return eWiFiFailure;
    }

    ret = os_mutex_create(&survey.lock, "survey-mtx",
			OS_MUTEX_INHERIT);
    if (ret != WM_SUCCESS) {
        os_semaphore_delete(&scan_protection_sem);
        return eWiFiFailure;
    }

    ret = os_semaphore_get(&scan_protection_sem, os_msec_to_ticks(
            DEFAULT_SCAN_PROTECTION_TIMEOUT * 1000));

    if (ret != WM_SUCCESS) {
        wm_wlan_e("Scan protection timeout occurred."
                   " Failed to get the scan protection for scan start"
                   " semaphore", ret);
        os_mutex_delete(&survey.lock);
        os_semaphore_delete(&scan_protection_sem);
        return eWiFiFailure;
    }

    wlan_scan(prov_handle_scan_results);

    ret = os_semaphore_get(&scan_protection_sem, os_msec_to_ticks(
            DEFAULT_SCAN_PROTECTION_TIMEOUT * 1000));
    if (ret != WM_SUCCESS) {
        wm_wlan_e("Scan protection timeout occurred."
                   " Failed to get the scan protection for scan result"
                   " semaphore", ret);
        os_mutex_delete(&survey.lock);
        os_semaphore_delete(&scan_protection_sem);
        return eWiFiFailure;
    }

    if(survey.num_sites < ucNumNetworks ) {
        ucNumNetworks = survey.num_sites;
    }

    for(i = 0; i < ucNumNetworks; i++) {
        pxBuffer[i].ucChannel = survey.sites[i].channel;
        pxBuffer[i].cRSSI = survey.sites[i].rssi;

        if (survey.sites[i].ssid_len) {
            pxBuffer[i].ucSSIDLength = strnlen( survey.sites[i].ssid, wificonfigMAX_SSID_LEN );
            memcpy(pxBuffer[i].ucSSID, survey.sites[i].ssid, pxBuffer[i].ucSSIDLength);
        }

        strncpy((char *)pxBuffer[i].ucBSSID, survey.sites[i].bssid, wificonfigMAX_BSSID_LEN);

        if(survey.sites[i].wep) {
             pxBuffer[i].xSecurity = eWiFiSecurityWEP;
        }
        else if (survey.sites[i].wpa) {
             pxBuffer[i].xSecurity = eWiFiSecurityWPA;
        }
        else if (survey.sites[i].wpa2) {
             pxBuffer[i].xSecurity = eWiFiSecurityWPA2;
        }
        else if (survey.sites[i].wpa2_entp || survey.sites[i].wpa3_sae) {
             pxBuffer[i].xSecurity = eWiFiSecurityNotSupported;
        }
        else {
             pxBuffer[i].xSecurity = eWiFiSecurityOpen;
        }
    }

    os_mutex_delete(&survey.lock);
    os_semaphore_delete(&scan_protection_sem);

    return eWiFiSuccess;
}
/*-----------------------------------------------------------*/

WIFIReturnCode_t WIFI_SetMode( WIFIDeviceMode_t xDeviceMode )
{
    /* FIX ME. */
    return eWiFiNotSupported;
}
/*-----------------------------------------------------------*/

WIFIReturnCode_t WIFI_GetMode( WIFIDeviceMode_t * pxDeviceMode )
{
    /* FIX ME. */
    return eWiFiNotSupported;
}
/*-----------------------------------------------------------*/

WIFIReturnCode_t WIFI_NetworkAdd( const WIFINetworkProfile_t * const pxNetworkProfile,
                                  uint16_t * pusIndex )
{
    /* FIX ME. */
    return eWiFiNotSupported;
}
/*-----------------------------------------------------------*/

WIFIReturnCode_t WIFI_NetworkGet( WIFINetworkProfile_t * pxNetworkProfile,
                                  uint16_t usIndex )
{
    /* FIX ME. */
    return eWiFiNotSupported;
}
/*-----------------------------------------------------------*/

WIFIReturnCode_t WIFI_NetworkDelete( uint16_t usIndex )
{
    /* FIX ME. */
    return eWiFiNotSupported;
}
/*-----------------------------------------------------------*/

WIFIReturnCode_t WIFI_Ping( uint8_t * pucIPAddr,
                            uint16_t usCount,
                            uint32_t ulIntervalMS )
{
    /* FIX ME. */
    return eWiFiNotSupported;
}
/*-----------------------------------------------------------*/

WIFIReturnCode_t WIFI_GetIPInfo( WIFIIPConfiguration_t * xIPConfig )
{
    struct wlan_ip_config ip_addr;
    int ret;

    if (xIPConfig == NULL) {
        return eWiFiFailure;
    }
    memset(xIPConfig, 0, sizeof(xIPConfig));

    ret = wlan_get_address(&ip_addr);
    if (ret != WM_SUCCESS) {
        wm_wlan_e("app_network_config: failed to get IP address");
        return eWiFiFailure;
     }

    memcpy( &xIPConfig->xIPAddress.ulAddress[0], &ip_addr.ipv4.address, 4 );

    return eWiFiSuccess;
}
/*-----------------------------------------------------------*/

WIFIReturnCode_t WIFI_GetMAC( uint8_t * pucMac )
{
    if (pucMac == NULL) {
        return eWiFiFailure;
    }

    if (WM_SUCCESS != wlan_get_mac_address(pucMac)) {
        return eWiFiFailure;
    }

    return eWiFiSuccess;
}
/*-----------------------------------------------------------*/

WIFIReturnCode_t WIFI_GetHostIP( char * pcHost,
                                 uint8_t * pucIPAddr )
{
    WIFIReturnCode_t ret = eWiFiFailure;
    uint32_t IPAddr;

    if ((pcHost == NULL) || (pucIPAddr == NULL)) {
        return ret;
    }

    if (os_mutex_get(&wlan_mtx, OS_WAIT_FOREVER) != WM_SUCCESS)
           return eWiFiTimeout;

    IPAddr = FreeRTOS_gethostbyname(pcHost);
    if (IPAddr != 0UL) {
        *( ( uint32_t * ) pucIPAddr ) = IPAddr;
        ret = eWiFiSuccess;
    }

    if (os_mutex_put(&wlan_mtx) != WM_SUCCESS)
           return eWiFiFailure;

    return ret;
}
/*-----------------------------------------------------------*/

WIFIReturnCode_t WIFI_StartAP( void )
{
    /* FIX ME. */
    return eWiFiNotSupported;
}
/*-----------------------------------------------------------*/

WIFIReturnCode_t WIFI_StopAP( void )
{
    /* FIX ME. */
    return eWiFiNotSupported;
}
/*-----------------------------------------------------------*/

WIFIReturnCode_t WIFI_ConfigureAP( const WIFINetworkParams_t * const pxNetworkParams )
{
    /* FIX ME. */
    return eWiFiNotSupported;
}
/*-----------------------------------------------------------*/

WIFIReturnCode_t WIFI_SetPMMode( WIFIPMMode_t xPMModeType,
                                 const void * pvOptionValue )
{
    /* FIX ME. */
    return eWiFiNotSupported;
}
/*-----------------------------------------------------------*/

WIFIReturnCode_t WIFI_GetPMMode( WIFIPMMode_t * pxPMModeType,
                                 void * pvOptionValue )
{
    /* FIX ME. */
    return eWiFiNotSupported;
}
/*-----------------------------------------------------------*/

BaseType_t WIFI_IsConnected( const WIFINetworkParams_t * pxNetworkParams )
{
    if (pxNetworkParams && pxNetworkParams->ucSSIDLength > 0)
    {
        struct wlan_network xCurrentNetwork;
        if ( WM_SUCCESS == wlan_get_current_network( &xCurrentNetwork ) )
        {
            if (0 == memcmp(pxNetworkParams->ucSSID, xCurrentNetwork.ssid, pxNetworkParams->ucSSIDLength))
            {
                return pdTRUE;
            }
        }
    }
    else if(is_sta_connected())
    {
        return pdTRUE;
    }


    return pdFALSE;
}
/*-----------------------------------------------------------*/

WIFIReturnCode_t WIFI_RegisterEvent( WIFIEventType_t xEventType, WIFIEventHandler_t xHandler )
{
    /** Needs to implement dispatching network state change events **/
    return eWiFiNotSupported;
}

