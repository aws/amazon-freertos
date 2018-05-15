// Copyright 2018 Espressif Systems (Shanghai) PTE LTD
//
// Amazon FreeRTOS Wi-Fi for ESP32-DevKitC ESP-WROVER-KIT V1.0.0
// Copyright (C) 2018 Amazon.com, Inc. or its affiliates.  All Rights Reserved.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

/**
 * @file aws_wifi.c
 * @brief WiFi Interface.
 */

/* Socket and WiFi interface includes. */
#include "aws_wifi.h"

/* WiFi configuration includes. */
#include "aws_wifi_config.h"

#include "string.h"
#include "esp_wifi.h"
#include "esp_log.h"
#include "esp_event_loop.h"
#include "event_groups.h"
#include "FreeRTOS_IP.h"
#include "FreeRTOS_Sockets.h"
#include "semphr.h"

static const char *TAG = "WIFI";
static EventGroupHandle_t wifi_event_group;
const int CONNECTED_BIT = BIT0;
const int DISCONNECTED_BIT = BIT1;
const int STARTED_BIT = BIT2;
static bool wifi_conn_state;

/**
 * @brief Semaphore for WiFI module.
 */
static SemaphoreHandle_t xWiFiSem; /**< WiFi module semaphore. */

/**
 * @brief Maximum time to wait in ticks for obtaining the WiFi semaphore
 * before failing the operation.
 */
static const TickType_t xSemaphoreWaitTicks = pdMS_TO_TICKS( wificonfigMAX_SEMAPHORE_WAIT_TIME_MS );

static esp_err_t event_handler(void *ctx, system_event_t *event)
{
    switch(event->event_id) {
        case SYSTEM_EVENT_STA_START:
            ESP_LOGI(TAG, "SYSTEM_EVENT_STA_START");
            xEventGroupSetBits(wifi_event_group, STARTED_BIT);
            break;
        case SYSTEM_EVENT_STA_CONNECTED:
            ESP_LOGI(TAG, "SYSTEM_EVENT_STA_CONNECTED");
            break;
        case SYSTEM_EVENT_STA_GOT_IP:
            ESP_LOGI(TAG, "SYSTEM_EVENT_STA_GOT_IP");
            wifi_conn_state = true;
            xEventGroupClearBits(wifi_event_group, DISCONNECTED_BIT);
            xEventGroupSetBits(wifi_event_group, CONNECTED_BIT);
            break;
        case SYSTEM_EVENT_STA_DISCONNECTED:
            ESP_LOGI(TAG, "SYSTEM_EVENT_STA_DISCONNECTED");
            wifi_conn_state = false;
            xEventGroupClearBits(wifi_event_group, CONNECTED_BIT);
            xEventGroupSetBits(wifi_event_group, DISCONNECTED_BIT);
            break;
        default:
            break;
    }
    return ESP_OK;
}
/*-----------------------------------------------------------*/

BaseType_t WIFI_IsConnected( void )
{
    BaseType_t xRetVal = pdFALSE;

    /* Try to acquire the semaphore. */
    if( xSemaphoreTake( xWiFiSem, xSemaphoreWaitTicks ) == pdTRUE )
    {
        if (wifi_conn_state == true)
        {
            xRetVal = pdTRUE;
        }
        /* Return the semaphore. */
        xSemaphoreGive( xWiFiSem );
    }
    return xRetVal;
}

WIFIReturnCode_t WIFI_Off( void )
{
    return eWiFiSuccess;
}
/*-----------------------------------------------------------*/

WIFIReturnCode_t WIFI_On( void )
{
    static bool wifi_inited;

    // Check if WiFi is already initialized
    if (wifi_inited == true) {
        return eWiFiSuccess;
    }

    esp_err_t ret = esp_event_loop_init(event_handler, NULL);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "%s: Failed to init event loop %d", __func__, ret);
        goto err;
    }

    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ret = esp_wifi_init(&cfg);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "%s: Failed to init wifi %d", __func__, ret);
        goto err;
    }

    ret = esp_wifi_set_storage(WIFI_STORAGE_RAM);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "%s: Failed to set wifi storage %d", __func__, ret);
        goto err;
    }

    wifi_event_group = xEventGroupCreate();
    if (wifi_event_group == NULL) {
        ESP_LOGE(TAG, "%s: Failed to create eventgroup", __func__);
        goto err;
    }

    /* Create sync mutex */
    static StaticSemaphore_t xSemaphoreBuffer;
    xWiFiSem = xSemaphoreCreateMutexStatic( &( xSemaphoreBuffer ) );

    wifi_inited = true;
    return eWiFiSuccess;
err:
    return eWiFiFailure;
}
/*-----------------------------------------------------------*/

#define CHECK_VALID_SSID_LEN(x) \
        ((x) > 0 && (x) <=  wificonfigMAX_SSID_LEN)
#define CHECK_VALID_PASSPHRASE_LEN(x) \
        ((x) > 0 && (x) <= wificonfigMAX_PASSPHRASE_LEN)

WIFIReturnCode_t WIFI_ConnectAP( const WIFINetworkParams_t * const pxNetworkParams )
{
    wifi_config_t wifi_config = { 0 };
    esp_err_t ret;
    WIFIReturnCode_t wifi_ret = eWiFiFailure;

    if (pxNetworkParams == NULL || pxNetworkParams->pcSSID == NULL
            || (pxNetworkParams->xSecurity != eWiFiSecurityOpen && pxNetworkParams->pcPassword == NULL)) {
        return wifi_ret;
    }

    if (!CHECK_VALID_SSID_LEN(pxNetworkParams->ucSSIDLength) ||
        (pxNetworkParams->xSecurity != eWiFiSecurityOpen && !CHECK_VALID_PASSPHRASE_LEN(pxNetworkParams->ucPasswordLength))) {
        return wifi_ret;
    }

    /* Try to acquire the semaphore. */
    if( xSemaphoreTake( xWiFiSem, xSemaphoreWaitTicks ) == pdTRUE )
    {
        // Check if WiFi is already connected
        if (wifi_conn_state == true) {
            esp_err_t ret = esp_wifi_disconnect();
            if (ret == ESP_OK)
            {
                // Wait for wifi disconnected event
                xEventGroupWaitBits(wifi_event_group, DISCONNECTED_BIT, pdTRUE, pdFALSE, portMAX_DELAY);
            }
        }

        /* Security is wildcard, only ssid/password is required */
        strlcpy((char *) &wifi_config.sta.ssid, pxNetworkParams->pcSSID, pxNetworkParams->ucSSIDLength);
        if (pxNetworkParams->xSecurity != eWiFiSecurityOpen) {
            strlcpy((char *) &wifi_config.sta.password, pxNetworkParams->pcPassword, pxNetworkParams->ucPasswordLength);
        }

        ret = esp_wifi_set_mode(WIFI_MODE_STA);
        if (ret != ESP_OK) {
            ESP_LOGE(TAG, "%s: Failed to set wifi mode %d", __func__, ret);
            xSemaphoreGive( xWiFiSem );
            return wifi_ret;
        }

        ret = esp_wifi_set_config(ESP_IF_WIFI_STA, &wifi_config);
        if (ret != ESP_OK) {
            ESP_LOGE(TAG, "%s: Failed to set wifi config %d", __func__, ret);
            xSemaphoreGive( xWiFiSem );
            return wifi_ret;
        }

        ret = esp_wifi_start();
        if (ret != ESP_OK) {
            ESP_LOGE(TAG, "%s: Failed to start wifi %d", __func__, ret);
            xSemaphoreGive( xWiFiSem );
            return wifi_ret;
        }

        // Wait for wifi started event
        xEventGroupWaitBits(wifi_event_group, STARTED_BIT, pdFALSE, pdFALSE, portMAX_DELAY);

        esp_wifi_connect();

        // Wait for wifi connected or disconnected event
        xEventGroupWaitBits(wifi_event_group, CONNECTED_BIT | DISCONNECTED_BIT, pdTRUE, pdFALSE, portMAX_DELAY);
        if (wifi_conn_state == true) {
            wifi_ret = eWiFiSuccess;
        }
        /* Return the semaphore. */
        xSemaphoreGive( xWiFiSem );

    }

    return wifi_ret;
}
/*-----------------------------------------------------------*/

WIFIReturnCode_t WIFI_Disconnect( void )
{
    WIFIReturnCode_t xRetVal = eWiFiFailure;

    /* Try to acquire the semaphore. */
    if( xSemaphoreTake( xWiFiSem, xSemaphoreWaitTicks ) == pdTRUE )
    {
        // Check if WiFi is already disconnected
        if (wifi_conn_state == false) {
            xSemaphoreGive( xWiFiSem );
            return eWiFiSuccess;
        }

        esp_err_t ret = esp_wifi_disconnect();
        if (ret == ESP_OK)
        {
            // Wait for wifi disconnected event
            xEventGroupWaitBits(wifi_event_group, DISCONNECTED_BIT, pdTRUE, pdFALSE, portMAX_DELAY);
            xRetVal = eWiFiSuccess;
        }
        /* Return the semaphore. */
        xSemaphoreGive( xWiFiSem );
    }
    else
    {
        xRetVal = eWiFiTimeout;
    }
    return xRetVal;
}
/*-----------------------------------------------------------*/

WIFIReturnCode_t WIFI_Reset( void )
{
    return eWiFiNotSupported;
}
/*-----------------------------------------------------------*/

WIFIReturnCode_t WIFI_Scan( WIFIScanResult_t * pxBuffer,
                            uint8_t ucNumNetworks )
{
    WIFIReturnCode_t xRetVal = eWiFiFailure;

    if (pxBuffer == NULL) {
        return eWiFiFailure;
    }

    wifi_scan_config_t scanConf = {
        .ssid = NULL,
        .bssid = NULL,
        .channel = 0,
        .show_hidden = false
    };

    /* Try to acquire the semaphore. */
    if( xSemaphoreTake( xWiFiSem, xSemaphoreWaitTicks ) == pdTRUE )
    {
        esp_err_t ret = esp_wifi_scan_start(&scanConf, true);
        if (ret == ESP_OK)
        {
            wifi_ap_record_t *ap_info = calloc(1, sizeof(wifi_ap_record_t) * ucNumNetworks);
            if (ap_info != NULL)
            {
                uint16_t num_aps = ucNumNetworks;
                esp_wifi_scan_get_ap_records(&num_aps, ap_info);
                for (int i = 0; i < num_aps; i++) {
                    strlcpy(pxBuffer[i].cSSID, (const char *)ap_info[i].ssid, wificonfigMAX_SSID_LEN);
                    memcpy(pxBuffer[i].ucBSSID, ap_info[i].bssid, wificonfigMAX_BSSID_LEN);
                    pxBuffer[i].cRSSI = ap_info[i].rssi;
                    pxBuffer[i].cChannel = ap_info[i].primary;
                }
                free(ap_info);
                xRetVal = eWiFiSuccess;
            }
        }

        /* Return the semaphore. */
        xSemaphoreGive( xWiFiSem );
    }
    else
    {
        xRetVal = eWiFiTimeout;
    }

    return xRetVal;
}
/*-----------------------------------------------------------*/

WIFIReturnCode_t WIFI_SetMode( WIFIDeviceMode_t xDeviceMode )
{
    esp_err_t ret;
    WIFIReturnCode_t xRetVal;

    /* Try to acquire the semaphore. */
    if( xSemaphoreTake( xWiFiSem, xSemaphoreWaitTicks ) == pdTRUE )
    {
        switch (xDeviceMode) {
            case eWiFiModeStation:
                ret = esp_wifi_set_mode(WIFI_MODE_STA);
                xRetVal = (ret == ESP_OK) ? eWiFiSuccess : eWiFiFailure;
                break;
            case eWiFiModeAP:
                ret = esp_wifi_set_mode(WIFI_MODE_AP);
                xRetVal = (ret == ESP_OK) ? eWiFiSuccess : eWiFiFailure;
                break;
            default:
                xRetVal = eWiFiNotSupported;
                break;
        }
        /* Return the semaphore. */
        xSemaphoreGive( xWiFiSem );
    }
    else
    {
        xRetVal = eWiFiTimeout;
    }
    return xRetVal;
}
/*-----------------------------------------------------------*/

WIFIReturnCode_t WIFI_GetMode( WIFIDeviceMode_t * pxDeviceMode )
{
    esp_err_t ret;
    wifi_mode_t mode;
    WIFIReturnCode_t xRetVal = eWiFiFailure;

    if (pxDeviceMode == NULL)
    {
        return eWiFiFailure;
    }

    /* Try to acquire the semaphore. */
    if( xSemaphoreTake( xWiFiSem, xSemaphoreWaitTicks ) == pdTRUE )
    {
        ret = esp_wifi_get_mode(&mode);
        if (ret == ESP_OK)
        {
            if (mode == WIFI_MODE_STA)
            {
                *pxDeviceMode = eWiFiModeStation;
            } else if (mode == WIFI_MODE_AP)
            {
                *pxDeviceMode = eWiFiModeAP;
            }
            xRetVal = eWiFiSuccess;
        }
        /* Return the semaphore. */
        xSemaphoreGive( xWiFiSem );
    }
    else
    {
        xRetVal = eWiFiTimeout;
    }
    return xRetVal;
}
/*-----------------------------------------------------------*/

WIFIReturnCode_t WIFI_NetworkAdd( const WIFINetworkProfile_t * const pxNetworkProfile,
                                  uint16_t * pusIndex )
{
    return eWiFiNotSupported;
}
/*-----------------------------------------------------------*/

WIFIReturnCode_t WIFI_NetworkGet( WIFINetworkProfile_t * pxNetworkProfile,
                                  uint16_t usIndex )
{
    return eWiFiNotSupported;
}
/*-----------------------------------------------------------*/

WIFIReturnCode_t WIFI_NetworkDelete( uint16_t usIndex )
{
    return eWiFiNotSupported;
}
/*-----------------------------------------------------------*/

WIFIReturnCode_t WIFI_Ping( uint8_t * pucIPAddr,
                            uint16_t usCount,
                            uint32_t ulIntervalMS )
{
    return eWiFiNotSupported;
}
/*-----------------------------------------------------------*/

WIFIReturnCode_t WIFI_GetIP( uint8_t * pucIPAddr )
{
    WIFIReturnCode_t xRetVal;

    if (pucIPAddr == NULL) {
        return eWiFiFailure;
    }

    /* Try to acquire the semaphore. */
    if( xSemaphoreTake( xWiFiSem, xSemaphoreWaitTicks ) == pdTRUE )
    {
        *( ( uint32_t * ) pucIPAddr ) = FreeRTOS_GetIPAddress();
        xRetVal = eWiFiSuccess;
        /* Return the semaphore. */
        xSemaphoreGive( xWiFiSem );
    }
    else
    {
        xRetVal = eWiFiTimeout;
    }

    return xRetVal;
}
/*-----------------------------------------------------------*/

WIFIReturnCode_t WIFI_GetMAC( uint8_t * pucMac )
{
    WIFIReturnCode_t xRetVal;

    if (pucMac == NULL) {
        return eWiFiFailure;
    }

    /* Try to acquire the semaphore. */
    if( xSemaphoreTake( xWiFiSem, xSemaphoreWaitTicks ) == pdTRUE )
    {
        esp_err_t ret = esp_wifi_get_mac(WIFI_MODE_STA, pucMac);
        xRetVal = (ret == ESP_OK) ? eWiFiSuccess : eWiFiFailure;
        /* Return the semaphore. */
        xSemaphoreGive( xWiFiSem );
    }
    else
    {
        xRetVal = eWiFiTimeout;
    }

    return xRetVal;
}
/*-----------------------------------------------------------*/

WIFIReturnCode_t WIFI_GetHostIP( char * pcHost,
                                 uint8_t * pucIPAddr )
{
    WIFIReturnCode_t xRetVal = eWiFiFailure;
    uint32_t IPAddr;

    if (pcHost == NULL || pucIPAddr == NULL) {
        return xRetVal;
    }

    /* Try to acquire the semaphore. */
    if( xSemaphoreTake( xWiFiSem, xSemaphoreWaitTicks ) == pdTRUE )
    {
        IPAddr = FreeRTOS_gethostbyname( pcHost );
        if (IPAddr != 0UL)
        {
            *( ( uint32_t * ) pucIPAddr ) = IPAddr;
            xRetVal = eWiFiSuccess;
        }
        /* Return the semaphore. */
        xSemaphoreGive( xWiFiSem );
    }
    else
    {
        xRetVal = eWiFiTimeout;
    }

    return xRetVal;
}
/*-----------------------------------------------------------*/

WIFIReturnCode_t WIFI_StartAP( void )
{
    return eWiFiNotSupported;
}
/*-----------------------------------------------------------*/

WIFIReturnCode_t WIFI_StopAP( void )
{
    return eWiFiNotSupported;
}
/*-----------------------------------------------------------*/

WIFIReturnCode_t WIFI_ConfigureAP( const WIFINetworkParams_t * const pxNetworkParams )
{
    return eWiFiNotSupported;
}
/*-----------------------------------------------------------*/

WIFIReturnCode_t WIFI_SetPMMode( WIFIPMMode_t xPMModeType,
                                 const void * pvOptionValue )
{
    return eWiFiNotSupported;
}
/*-----------------------------------------------------------*/

WIFIReturnCode_t WIFI_GetPMMode( WIFIPMMode_t * pxPMModeType,
                                 void * pvOptionValue )
{
    return eWiFiNotSupported;
}
/*-----------------------------------------------------------*/
