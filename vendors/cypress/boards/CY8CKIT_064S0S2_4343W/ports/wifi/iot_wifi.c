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

/* Wi-Fi configuration includes. */
#include "aws_wifi_config.h"

/* Board and Abstraction layer includes */
#include <cy_result.h>
#include <cybsp_wifi.h>
#include <cyabs_rtos.h>
#include <cyobjstore.h>

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

static whd_scan_userdata_t internalScanData;
static whd_scan_result_t internalScanResult;
static cy_semaphore_t scanSema;

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
    *key = (uint8_t*)pxNetworkParams->pcPassword;
    *keylen = strlen(pxNetworkParams->pcPassword);

    ssid->length = strlen(pxNetworkParams->pcSSID);
    memcpy(ssid->value, pxNetworkParams->pcSSID, ssid->length);

    *security = whd_fromsecurity(pxNetworkParams->xSecurity);

    *channel = (uint8_t)pxNetworkParams->cChannel;
}

void cy_check_network_params(const WIFINetworkParams_t * const pxNetworkParams)
{
    configASSERT(pxNetworkParams != NULL);
    configASSERT(pxNetworkParams->pcSSID != NULL);
    configASSERT(pxNetworkParams->ucSSIDLength < wificonfigMAX_SSID_LEN);
    if (pxNetworkParams->xSecurity != eWiFiSecurityOpen)
    {
        configASSERT(pxNetworkParams->pcPassword != NULL);
    }
    configASSERT(pxNetworkParams->ucPasswordLength < wificonfigMAX_PASSPHRASE_LEN);
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
    for (unsigned int i = 0; i < data->offset; i++) {
        if (CMP_MAC(data->aps[i].ucBSSID, record->BSSID.octet)) {
            return;
        }
    }   

    WIFIScanResult_t ap;
    uint8_t length = (record->SSID.length > sizeof(ap.cSSID) - 1) 
        ? sizeof(ap.cSSID) - 1
        : record->SSID.length;
    memcpy(ap.cSSID, record->SSID.value, length);
    ap.cSSID[length] = '\0';
    memcpy(ap.ucBSSID, record->BSSID.octet, sizeof(ap.ucBSSID));
    ap.xSecurity = whd_tosecurity(record->security);
    ap.cRSSI = record->signal_strength;
    ap.cChannel = record->channel;
    ap.ucHidden = 0;
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

static WIFIReturnCode_t add_profile(const WIFINetworkProfile_t * const pxNetworkProfile, uint16_t * pusIndex)
{
    for(uint32_t i = CY_FIRST_WIFI_PROFILE; i < CY_FIRST_WIFI_PROFILE + CY_TOTAL_WIFI_PROFILES; i++)
    {
        if (cy_objstore_find_object(i, NULL, NULL) == CY_OBJSTORE_NO_SUCH_OBJECT)
        {
            /* Found an empty slot */
            cy_rslt_t res;

            res = cy_objstore_store_object(i, (const uint8_t *)pxNetworkProfile, sizeof(WIFINetworkProfile_t));
            if (res != CY_RSLT_SUCCESS)
            {
                return eWiFiFailure;
            }
            *pusIndex = i - CY_FIRST_WIFI_PROFILE;
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
        if (cy_objstore_is_initialized() == CY_OBJSTORE_NOT_INITIALIZED &&
            cy_objstore_initialize(false, 1) != CY_RSLT_SUCCESS)
        {
            cy_rtos_set_mutex(&wifiMutex);
            return eWiFiFailure;
        }

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

WIFIReturnCode_t WIFI_NetworkGet( WIFINetworkProfile_t * pxNetworkProfile, uint16_t usIndex )
{
    configASSERT(pxNetworkProfile != NULL);
    if (cy_rtos_get_mutex(&wifiMutex, wificonfigMAX_SEMAPHORE_WAIT_TIME_MS) == CY_RSLT_SUCCESS)
    {
        if (cy_objstore_is_initialized() == CY_OBJSTORE_NOT_INITIALIZED &&
            cy_objstore_initialize(false, 1) != CY_RSLT_SUCCESS)
        {
            cy_rtos_set_mutex(&wifiMutex);
            return eWiFiFailure;
        }

        if (usIndex >= CY_TOTAL_WIFI_PROFILES ||
            cy_objstore_read_object(usIndex + CY_FIRST_WIFI_PROFILE, (uint8_t *)pxNetworkProfile, sizeof(WIFINetworkProfile_t)))
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

WIFIReturnCode_t WIFI_NetworkDelete( uint16_t usIndex )
{
    uint32_t index; 
    uint32_t size;
    if (cy_rtos_get_mutex(&wifiMutex, wificonfigMAX_SEMAPHORE_WAIT_TIME_MS) == CY_RSLT_SUCCESS)
    {
        if (cy_objstore_is_initialized() == CY_OBJSTORE_NOT_INITIALIZED &&
            cy_objstore_initialize(false, 1) != CY_RSLT_SUCCESS)
        {
            cy_rtos_set_mutex(&wifiMutex);
            return eWiFiFailure;
        }

        if (usIndex >= CY_TOTAL_WIFI_PROFILES ||
            (cy_objstore_find_object(usIndex + CY_FIRST_WIFI_PROFILE, &index, &size) != CY_OBJSTORE_NO_SUCH_OBJECT &&
            cy_objstore_delete_object(usIndex + CY_FIRST_WIFI_PROFILE) != CY_RSLT_SUCCESS))
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

BaseType_t WIFI_IsConnected(void)
{
    return isConnected ? pdTRUE : pdFALSE;
}

WIFIReturnCode_t WIFI_RegisterNetworkStateChangeEventCallback( IotNetworkStateChangeEventCallback_t xCallback )
{
    /* FIX ME. */
    return eWiFiNotSupported;
}

