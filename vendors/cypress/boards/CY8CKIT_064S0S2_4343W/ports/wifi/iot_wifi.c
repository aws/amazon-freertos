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
 * @file iot_wifi.c
 * @brief Wi-Fi Interface.
 */

/* Socket and Wi-Fi interface includes. */
#include <FreeRTOS.h>
#include <task.h>
#include <iot_wifi.h>
#include <iot_wifi_common.h>
#include <cy_wifi_notify.h>

/* Wi-Fi configuration includes. */
#include "aws_wifi_config.h"

/* Board and Abstraction layer includes */
#include <cy_result.h>
#include <cybsp_wifi.h>
#include <cyabs_rtos.h>
#include <kvstore.h>

#define CMP_MAC( a, b )  (((((unsigned char*)a)[0])==(((unsigned char*)b)[0]))&& \
                          ((((unsigned char*)a)[1])==(((unsigned char*)b)[1]))&& \
                          ((((unsigned char*)a)[2])==(((unsigned char*)b)[2]))&& \
                          ((((unsigned char*)a)[3])==(((unsigned char*)b)[3]))&& \
                          ((((unsigned char*)a)[4])==(((unsigned char*)b)[4]))&& \
                          ((((unsigned char*)a)[5])==(((unsigned char*)b)[5])))

#ifndef CY_FIRST_WIFI_PROFILE
#define CY_FIRST_WIFI_PROFILE           (128)
#endif

#ifndef CY_TOTAL_WIFI_PROFILES
#define CY_TOTAL_WIFI_PROFILES          (8)
#endif

/* This is the number of characters in WIFI_PROFILE_KEY_COMMON, a space, up to three digits
 * (CY_FIRST_WIFI_PROFILE + CY_TOTAL_WIFI_PROFILES), and the string termination character
 * (see get_wifi_profile_key fuction below) */
#define WIFI_PROFILE_KEY_SIZE 21
#define WIFI_PROFILE_KEY_COMMON "Wifi Profile Key"


#define DEFAULT_SLEEP_DELAY_MS          (200)

typedef struct
{
    cy_semaphore_t *sema;
    WIFIScanResult_t *aps;
    unsigned count;
    unsigned offset;
    bool scanInProgress;
} whd_scan_userdata_t;

whd_interface_t primaryInterface;
cy_mutex_t wifiMutex;
WIFIDeviceMode_t devMode = eWiFiModeNotSupported;
bool isConnected = false;
bool isPoweredUp = false;
bool isMutexInitialized = false;
IotNetworkStateChangeEventCallback_t userCb = NULL;

static whd_scan_userdata_t internalScanData;
static whd_scan_result_t internalScanResult;
static cy_semaphore_t scanSema;

/* This function will be overridden by LPA Library
*/
__WEAK cy_rslt_t cy_olm_create(void *ifp, void *oflds_list)
{
    return CY_RSLT_SUCCESS;
}

static WIFIPMMode_t whd_topmmode(uint32_t mode)
{
    switch(mode)
    {
        case NO_POWERSAVE_MODE:
            return eWiFiPMAlwaysOn;
        case PM1_POWERSAVE_MODE:
            return eWiFiPMLowPower;
        case PM2_POWERSAVE_MODE:
            return eWiFiPMNormal;
        default:
            return eWiFiPMNotSupported;
    }
}

static whd_security_t whd_fromsecurity(WIFISecurity_t sec)
{
    switch (sec) {
        case eWiFiSecurityOpen:
            return WHD_SECURITY_OPEN;
        case eWiFiSecurityWEP:
            return WHD_SECURITY_WEP_PSK;
        case eWiFiSecurityWPA:
            return WHD_SECURITY_WPA_MIXED_PSK;
        case eWiFiSecurityWPA2:
            return WHD_SECURITY_WPA2_MIXED_PSK;
        case eWiFiSecurityWPA3:
            return WHD_SECURITY_WPA3_SAE;
        default:
            return WHD_SECURITY_UNKNOWN;
    }
}

static WIFISecurity_t whd_tosecurity(whd_security_t sec)
{
    switch (sec) {
        case WHD_SECURITY_OPEN:
            return eWiFiSecurityOpen;
        case WHD_SECURITY_WEP_PSK:
        case WHD_SECURITY_WEP_SHARED:
            return eWiFiSecurityWEP;
        case WHD_SECURITY_WPA_TKIP_PSK:
        case WHD_SECURITY_WPA_TKIP_ENT:
            return eWiFiSecurityWPA;
        case WHD_SECURITY_WPA2_MIXED_PSK:
        case WHD_SECURITY_WPA2_AES_PSK:
        case WHD_SECURITY_WPA2_AES_ENT:
        case WHD_SECURITY_WPA2_FBT_PSK:
        case WHD_SECURITY_WPA2_FBT_ENT:
            return eWiFiSecurityWPA2;
        case WHD_SECURITY_WPA3_SAE:
            return eWiFiSecurityWPA3;
        default:
            return eWiFiSecurityNotSupported;
    }
}

void cy_convert_network_params(
    const WIFINetworkParams_t * const pxNetworkParams,
    whd_ssid_t *ssid,
    uint8_t **key,
    uint8_t *keylen,
    whd_security_t *security,
    uint8_t *channel)
{
    *key = (uint8_t*)pxNetworkParams->xPassword.xWPA.cPassphrase;
    *keylen = pxNetworkParams->xPassword.xWPA.ucLength;

    ssid->length = pxNetworkParams->ucSSIDLength;
    memcpy(ssid->value, pxNetworkParams->ucSSID, ssid->length);

    *security = whd_fromsecurity(pxNetworkParams->xSecurity);

    *channel = (uint8_t)pxNetworkParams->ucChannel;
}

void cy_check_network_params(const WIFINetworkParams_t * const pxNetworkParams)
{
    configASSERT(pxNetworkParams != NULL);
    configASSERT(pxNetworkParams->ucSSIDLength != 0);
    configASSERT(pxNetworkParams->ucSSIDLength < wificonfigMAX_SSID_LEN);
    if (pxNetworkParams->xSecurity != eWiFiSecurityOpen)
    {
        configASSERT(pxNetworkParams->xPassword.xWPA.ucLength != 0);
    }
    configASSERT(pxNetworkParams->xPassword.xWPA.ucLength <= wificonfigMAX_PASSPHRASE_LEN);
    configASSERT(pxNetworkParams->xSecurity != eWiFiSecurityNotSupported);
}

/*-----------------------------------------------------------*/

WIFIReturnCode_t WIFI_On( void )
{
    if (!isPoweredUp)
    {
        if (cy_rtos_init_mutex(&wifiMutex) != CY_RSLT_SUCCESS)
        {
            return eWiFiFailure;
        }
        if (cybsp_wifi_init_primary(&primaryInterface) != CY_RSLT_SUCCESS)
        {
            cy_rtos_deinit_mutex(&wifiMutex);
            return eWiFiFailure;
        }

        /* create a worker thread */
        if(cy_wifi_worker_thread_create() != CY_RSLT_SUCCESS)
        {
            cy_rtos_deinit_mutex(&wifiMutex);
            return eWiFiFailure;
        }
        if (cy_rtos_init_mutex(&cy_app_cb_Mutex) != CY_RSLT_SUCCESS)
        {
            cy_rtos_deinit_mutex(&wifiMutex);
            return eWiFiFailure;
        }
        /* Offload Manager create : This is entry function for LPA Offload
         * Manager and definition of the function is added as WEAK
         * and will be replaced by strong function in LPA Library when it is included.
         */
        if (cy_olm_create(primaryInterface, NULL) != CY_RSLT_SUCCESS)
        {
            cy_rtos_deinit_mutex(&wifiMutex);
            cy_rtos_deinit_mutex(&cy_app_cb_Mutex);
            return eWiFiFailure;
        }
        isMutexInitialized = true;
        isPoweredUp = true;

    }

    return eWiFiSuccess;
}

/*-----------------------------------------------------------*/

WIFIReturnCode_t WIFI_Reset( void )
{
    if ((devMode == eWiFiModeStation && WIFI_Disconnect() != eWiFiSuccess) ||
        (devMode == eWiFiModeAP && WIFI_StopAP() != eWiFiSuccess))
    {
        return eWiFiFailure;
    }

    if (cy_rtos_get_mutex(&wifiMutex, wificonfigMAX_SEMAPHORE_WAIT_TIME_MS) == CY_RSLT_SUCCESS)
    {
        if ((whd_wifi_set_down(primaryInterface) != CY_RSLT_SUCCESS) ||
            (whd_wifi_set_up(primaryInterface) != CY_RSLT_SUCCESS))
        {
            cy_rtos_set_mutex(&wifiMutex);
            return eWiFiFailure;
        }
        devMode = eWiFiModeNotSupported;
        if (cy_rtos_set_mutex(&wifiMutex) != CY_RSLT_SUCCESS)
        {
            return eWiFiFailure;
        }
    }
    else
    {
        return eWiFiTimeout;
    }
    return eWiFiSuccess;
}


/*-----------------------------------------------------------*/
static void whd_scan_handler(whd_scan_result_t **result_ptr,
                             void *user_data, whd_scan_status_t status)
{
    whd_scan_userdata_t *data = (whd_scan_userdata_t *)user_data;
    /* Even after stopping scan, some results will still come as results are already present in the queue */
    if (data->scanInProgress == false)
    {
        return;
    }

    if (status != WHD_SCAN_INCOMPLETE)
    {
        data->scanInProgress = false;
        cy_rtos_set_semaphore(data->sema, false);
        return;
    }

    if ((data->aps == NULL) ||
        (data->count <= 0) ||
        ((data->count > 0) && (data->offset >= data->count)))
    {
        /* We can not abort the scan as this function is getting executed in WHD context,
           Note that to call any WHD API, caller function should not in WHD context */
        return;
    }

    const whd_scan_result_t *record = *result_ptr;
    
    /* Filter Duplicates */
    for (unsigned int i = 0; i < data->offset; i++)
    {
        if (CMP_MAC(data->aps[i].ucBSSID, record->BSSID.octet))
        {
            return;
        }
    }   

    WIFIScanResult_t ap;

    ap.ucSSIDLength = record->SSID.length;
    if( ap.ucSSIDLength > wificonfigMAX_SSID_LEN )
    {
        ap.ucSSIDLength = wificonfigMAX_SSID_LEN;
    }
    memcpy( ap.ucSSID, record->SSID.value, ap.ucSSIDLength );
    memcpy(ap.ucBSSID, record->BSSID.octet, sizeof(ap.ucBSSID));
    ap.xSecurity = whd_tosecurity(record->security);
    ap.cRSSI = record->signal_strength;
    ap.ucChannel = record->channel;
    data->aps[data->offset] = ap;
    data->offset++;  
}

WIFIReturnCode_t WIFI_Scan( WIFIScanResult_t * pxBuffer, uint8_t ucNumNetworks )
{
    configASSERT(pxBuffer != NULL);

    if (cy_rtos_get_mutex(&wifiMutex, wificonfigMAX_SEMAPHORE_WAIT_TIME_MS) == CY_RSLT_SUCCESS)
    {
        cy_rtos_init_semaphore(&scanSema, 1, 0);

        internalScanData.sema = &scanSema;
        internalScanData.aps = pxBuffer;
        internalScanData.count = ucNumNetworks;
        internalScanData.scanInProgress = true;
        internalScanData.offset = 0;

        uint32_t res = whd_wifi_scan(primaryInterface, WHD_SCAN_TYPE_ACTIVE, WHD_BSS_TYPE_ANY,
                                            NULL, NULL, NULL, NULL, whd_scan_handler, &internalScanResult, &internalScanData);
        if (res != WHD_SUCCESS)
        {
            cy_rtos_set_mutex(&wifiMutex);
            return eWiFiFailure;
        }

        /* This is a blocking call. If you are stuck here the scan handler did not get called. */
        cy_rtos_get_semaphore(internalScanData.sema, CY_RTOS_NEVER_TIMEOUT, false);

        cy_rtos_deinit_semaphore(internalScanData.sema);
        if (cy_rtos_set_mutex(&wifiMutex) != CY_RSLT_SUCCESS)
        {
            return eWiFiFailure;
        }
    }
    else
    {
        return eWiFiTimeout;
    }
    return eWiFiSuccess;
}
/*-----------------------------------------------------------*/

WIFIReturnCode_t WIFI_SetMode( WIFIDeviceMode_t xDeviceMode )
{
    WIFIReturnCode_t ret = eWiFiSuccess;
    if (devMode != xDeviceMode)
    {
        switch(xDeviceMode)
        {
            case eWiFiModeAP:
            case eWiFiModeStation:
                ret = WIFI_Reset();
                if (ret == eWiFiSuccess)
                {
                    devMode = xDeviceMode;
                }
                break;
            case eWiFiModeP2P:
            case eWiFiModeNotSupported:
            default:
                ret = eWiFiNotSupported;
        }
    }
    return ret;
}
/*-----------------------------------------------------------*/

WIFIReturnCode_t WIFI_GetMode( WIFIDeviceMode_t * pxDeviceMode )
{
    configASSERT(pxDeviceMode != NULL);
    if (cy_rtos_get_mutex(&wifiMutex, wificonfigMAX_SEMAPHORE_WAIT_TIME_MS) == CY_RSLT_SUCCESS)
    {
        *pxDeviceMode = devMode;
        if (cy_rtos_set_mutex(&wifiMutex) != CY_RSLT_SUCCESS)
        {
            return eWiFiFailure;
        }
    }
    else
    {
        return eWiFiTimeout;
    }
    return eWiFiSuccess;
}
/*-----------------------------------------------------------*/

static void get_wifi_profile_key(uint8_t index, char *wifi_profile_key)
{
    char wifi_profile_key_common[] = WIFI_PROFILE_KEY_COMMON;

    sprintf(wifi_profile_key, "%s %d", wifi_profile_key_common, index);
}
/*-----------------------------------------------------------*/

static WIFIReturnCode_t add_profile(const WIFINetworkProfile_t * const pxNetworkProfile,
    uint16_t *pusIndex)
{
    uint32_t index;
    char wifi_profile_key[WIFI_PROFILE_KEY_SIZE];

    for (index = 0; index < CY_TOTAL_WIFI_PROFILES; index++)
    {
        get_wifi_profile_key(index + CY_FIRST_WIFI_PROFILE, wifi_profile_key);

        if (mtb_kvstore_read(&kvstore_obj, wifi_profile_key, NULL, NULL) != CY_RSLT_SUCCESS)
        {
            /* Found an empty slot */
            if (mtb_kvstore_write(&kvstore_obj, wifi_profile_key, (const uint8_t *)pxNetworkProfile,
                                  sizeof(WIFINetworkProfile_t)) != CY_RSLT_SUCCESS)
            {
                return eWiFiFailure;
            }
            *pusIndex = index;
            return eWiFiSuccess;
        }
    }
    return eWiFiFailure;
}

WIFIReturnCode_t WIFI_NetworkAdd( const WIFINetworkProfile_t * const pxNetworkProfile, uint16_t * pusIndex )
{
    configASSERT(pxNetworkProfile != NULL && pusIndex != NULL);
    if (cy_rtos_get_mutex(&wifiMutex, wificonfigMAX_SEMAPHORE_WAIT_TIME_MS) == CY_RSLT_SUCCESS)
    {
        if (add_profile(pxNetworkProfile, pusIndex) != eWiFiSuccess)
        {
            cy_rtos_set_mutex(&wifiMutex);
            return eWiFiFailure;
        }

        if (cy_rtos_set_mutex(&wifiMutex) != CY_RSLT_SUCCESS)
        {
            return eWiFiFailure;
        }
    }
    else
    {
        return eWiFiTimeout;
    }
    return eWiFiSuccess;
}
/*-----------------------------------------------------------*/

WIFIReturnCode_t WIFI_NetworkGet(WIFINetworkProfile_t *pxNetworkProfile, uint16_t usIndex)
{
    char wifi_profile_key[WIFI_PROFILE_KEY_SIZE];
    uint32_t size = sizeof(WIFINetworkProfile_t);

    configASSERT(pxNetworkProfile != NULL);
    configASSERT(usIndex < CY_TOTAL_WIFI_PROFILES);

    get_wifi_profile_key(usIndex + CY_FIRST_WIFI_PROFILE, wifi_profile_key);

    if (cy_rtos_get_mutex(&wifiMutex, wificonfigMAX_SEMAPHORE_WAIT_TIME_MS) == CY_RSLT_SUCCESS)
    {
        if (mtb_kvstore_read(&kvstore_obj, wifi_profile_key, (uint8_t *)pxNetworkProfile, &size) !=
            CY_RSLT_SUCCESS)
        {
            cy_rtos_set_mutex(&wifiMutex);
            return eWiFiFailure;
        }
        if (cy_rtos_set_mutex(&wifiMutex) != CY_RSLT_SUCCESS)
        {
            return eWiFiFailure;
        }
    }
    else
    {
        return eWiFiTimeout;
    }
    return eWiFiSuccess;
}
/*-----------------------------------------------------------*/

WIFIReturnCode_t WIFI_NetworkDelete(uint16_t usIndex)
{
    char wifi_profile_key[WIFI_PROFILE_KEY_SIZE];

    configASSERT(usIndex < CY_TOTAL_WIFI_PROFILES);

    get_wifi_profile_key(usIndex + CY_FIRST_WIFI_PROFILE, wifi_profile_key);

    if (cy_rtos_get_mutex(&wifiMutex, wificonfigMAX_SEMAPHORE_WAIT_TIME_MS) == CY_RSLT_SUCCESS)
    {
        if ((mtb_kvstore_read(&kvstore_obj, wifi_profile_key, NULL, NULL) != CY_RSLT_SUCCESS) ||
            (mtb_kvstore_delete(&kvstore_obj, wifi_profile_key) != CY_RSLT_SUCCESS))
        {
            cy_rtos_set_mutex(&wifiMutex);
            return eWiFiFailure;
        }
        if (cy_rtos_set_mutex(&wifiMutex) != CY_RSLT_SUCCESS)
        {
            return eWiFiFailure;
        }
    }
    else
    {
        return eWiFiTimeout;
    }
    return eWiFiSuccess;
}
/*-----------------------------------------------------------*/

WIFIReturnCode_t WIFI_Ping( uint8_t * pucIPAddr,
                            uint16_t usCount,
                            uint32_t ulIntervalMS )
{
    return eWiFiNotSupported;
}
/*-----------------------------------------------------------*/

WIFIReturnCode_t WIFI_GetMAC( uint8_t * pucMac )
{
    configASSERT(pucMac != NULL);
    whd_mac_t mac;
    if (cy_rtos_get_mutex(&wifiMutex, wificonfigMAX_SEMAPHORE_WAIT_TIME_MS) == CY_RSLT_SUCCESS)
    {
        if (whd_wifi_get_mac_address(primaryInterface, &mac) != CY_RSLT_SUCCESS)
        {
            cy_rtos_set_mutex(&wifiMutex);
            return eWiFiFailure;
        }
        memcpy(pucMac, &mac, sizeof(mac));
        if (cy_rtos_set_mutex(&wifiMutex) != CY_RSLT_SUCCESS)
        {
            return eWiFiFailure;
        }
    }
    else
    {
        return eWiFiTimeout;
    }
    return eWiFiSuccess;
}

/*-----------------------------------------------------------*/

WIFIReturnCode_t WIFI_ConfigureAP( const WIFINetworkParams_t * const pxNetworkParams )
{
    whd_ssid_t ssid;
    uint8_t *key;
    uint8_t keylen;
    whd_security_t security;
    uint8_t channel;

    cy_check_network_params(pxNetworkParams);

    if (devMode != eWiFiModeAP &&
        devMode != eWiFiModeNotSupported &&
        WIFI_SetMode(eWiFiModeAP) != eWiFiSuccess)
    {
        return eWiFiFailure;
    }

    if (cy_rtos_get_mutex(&wifiMutex, wificonfigMAX_SEMAPHORE_WAIT_TIME_MS) == CY_RSLT_SUCCESS)
    {
        cy_convert_network_params(pxNetworkParams, &ssid, &key, &keylen, &security, &channel);

        if (whd_wifi_init_ap(primaryInterface, &ssid, security, key, keylen, channel) != CY_RSLT_SUCCESS)
        {
            cy_rtos_set_mutex(&wifiMutex);
            return eWiFiFailure;
        }
        devMode = eWiFiModeAP;
        if (cy_rtos_set_mutex(&wifiMutex) != CY_RSLT_SUCCESS)
        {
            return eWiFiFailure;
        }
    }
    else
    {
        return eWiFiTimeout;
    }
    return eWiFiSuccess;
}
/*-----------------------------------------------------------*/

WIFIReturnCode_t WIFI_SetPMMode( WIFIPMMode_t xPMModeType,
                                 const void * pvOptionValue )
{
    cy_rslt_t result = CY_RSLT_SUCCESS;
    uint32_t internal_mode;
    WIFIPMMode_t current_mode;
    uint16_t sleep_delay;

    if (xPMModeType == eWiFiPMNormal)
    {
        configASSERT(pvOptionValue != NULL);
    }

    if (cy_rtos_get_mutex(&wifiMutex, wificonfigMAX_SEMAPHORE_WAIT_TIME_MS) == CY_RSLT_SUCCESS)
    {
        result = whd_wifi_get_powersave_mode(primaryInterface, &internal_mode);
        current_mode = whd_topmmode(internal_mode);
        if (result == CY_RSLT_SUCCESS && xPMModeType != current_mode)
        {
            switch(xPMModeType)
            {
                case eWiFiPMLowPower:
                    result = whd_wifi_enable_powersave(primaryInterface);
                    break;
                case eWiFiPMNormal:
                    sleep_delay = (*((uint16_t *)pvOptionValue) == 0)
                                            ? DEFAULT_SLEEP_DELAY_MS
                                            : *((uint16_t *)pvOptionValue);
                    result = whd_wifi_enable_powersave_with_throughput(primaryInterface, sleep_delay);
                    break;
                case eWiFiPMAlwaysOn:
                    result = whd_wifi_disable_powersave(primaryInterface);
                    break;
                case eWiFiPMNotSupported:
                default:
                    cy_rtos_set_mutex(&wifiMutex);
                    return eWiFiNotSupported;
            }
        }
        if (cy_rtos_set_mutex(&wifiMutex) != CY_RSLT_SUCCESS)
        {
            return eWiFiFailure;
        }
    }
    else
    {
        return eWiFiTimeout;
    }
    return (result == CY_RSLT_SUCCESS) ? eWiFiSuccess : eWiFiFailure;
}
/*-----------------------------------------------------------*/

WIFIReturnCode_t WIFI_GetPMMode( WIFIPMMode_t * pxPMModeType,
                                 void * pvOptionValue )
{
    configASSERT(pxPMModeType != NULL);
    configASSERT(pvOptionValue != NULL);
    uint32_t internal_mode;
    if (cy_rtos_get_mutex(&wifiMutex, wificonfigMAX_SEMAPHORE_WAIT_TIME_MS) == CY_RSLT_SUCCESS)
    {
        if (whd_wifi_get_powersave_mode(primaryInterface, &internal_mode) != CY_RSLT_SUCCESS)
        {
            cy_rtos_set_mutex(&wifiMutex);
            return eWiFiFailure;
        }
        *pxPMModeType = whd_topmmode(internal_mode);
        if (cy_rtos_set_mutex(&wifiMutex) != CY_RSLT_SUCCESS)
        {
            return eWiFiFailure;
        }
    }
    else
    {
        return eWiFiTimeout;
    }
    return eWiFiSuccess;
}
/*-----------------------------------------------------------*/

BaseType_t WIFI_IsConnected( const WIFINetworkParams_t * pxNetworkParams )
{
    BaseType_t xIsConnected = pdFALSE;

    if ( isConnected )
    {
        if( pxNetworkParams )
        {
            whd_bss_info_t xAPInfo;
            whd_security_t xSecurity;
            if ( pxNetworkParams->ucSSIDLength > 0
                 && pxNetworkParams->ucSSIDLength <= wificonfigMAX_SSID_LEN
                 && WHD_SUCCESS == whd_wifi_get_ap_info( primaryInterface, &xAPInfo, &xSecurity)
                 && 0 == memcmp( pxNetworkParams->ucSSID, xAPInfo.SSID, pxNetworkParams->ucSSIDLength )
                 && pxNetworkParams->ucSSIDLength == xAPInfo.SSID_len )
            {
                xIsConnected = pdTRUE;
            }
        }
        else
        {
            xIsConnected = pdTRUE;
        }
    }

    return xIsConnected;
}

WIFIReturnCode_t WIFI_RegisterNetworkStateChangeEventCallback( IotNetworkStateChangeEventCallback_t xCallback )
{
    userCb = xCallback;
    return eWiFiSuccess;
}

WIFIReturnCode_t WIFI_RegisterEvent( WIFIEventType_t xEventType, WIFIEventHandler_t xHandler )
{
    /* FIX ME. */
    return eWiFiNotSupported;
}
