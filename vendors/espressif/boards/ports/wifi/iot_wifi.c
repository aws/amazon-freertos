// Copyright 2018 Espressif Systems (Shanghai) PTE LTD
//
// FreeRTOS Wi-Fi for ESP32-DevKitC ESP-WROVER-KIT V1.0.1
// Copyright (C) 2020 Amazon.com, Inc. or its affiliates.  All Rights Reserved.
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
 * @file iot_wifi.c
 * @brief WiFi Interface.
 */

/* Socket and WiFi interface includes. */
#include "iot_wifi.h"

/* WiFi configuration includes. */
#include "aws_wifi_config.h"

#include "string.h"
#include "esp_wifi.h"
#include "esp_log.h"
#include "esp_event.h"
#include "event_groups.h"
#if AFR_ESP_LWIP
#include "lwip/dns.h"
#include "lwip/netdb.h"
#else
#include "FreeRTOS_IP.h"
#include "FreeRTOS_Sockets.h"
#endif
#include "semphr.h"
#include "esp_smartconfig.h"
#include "nvs_flash.h"
#include "nvs.h"


static const char *TAG = "WIFI";
static EventGroupHandle_t wifi_event_group;
const int CONNECTED_BIT = BIT0;
const int DISCONNECTED_BIT = BIT1;
const int STARTED_BIT = BIT2;
const int STOPPED_BIT = BIT3;

const int AP_STARTED_BIT = BIT4;
const int AP_STOPPED_BIT = BIT5;
const int ESPTOUCH_DONE_BIT = BIT6;
static bool wifi_conn_state;
static bool wifi_ap_state;
static bool wifi_auth_failure;
static bool wifi_started;

static esp_netif_t *esp_netif_info;
static esp_netif_t *esp_softap_netif_info;

#define WIFI_FLASH_NS     "WiFi"
#define MAX_WIFI_KEY_WIDTH         ( 5 )
#define MAX_SECURITY_MODE_LEN      ( 1 )
#define MAX_AP_CONNECTIONS         ( 4 )

typedef struct StorageRegistry
{
	uint16_t usNumNetworks;
	uint16_t usNextStorageIdx;
	uint16_t usStorageIdx[ wificonfigMAX_NETWORK_PROFILES ];
} StorageRegistry_t;
static BaseType_t xIsRegistryInit = pdFALSE;
static StorageRegistry_t xRegistry = { 0 };
static WIFINetworkParams_t xConnectedAP;

/**
 * @brief Semaphore for WiFI module.
 */
static SemaphoreHandle_t xWiFiSem; /**< WiFi module semaphore. */

/**
 * @brief Maximum time to wait in ticks for obtaining the WiFi semaphore
 * before failing the operation.
 */
static const TickType_t xSemaphoreWaitTicks = pdMS_TO_TICKS( wificonfigMAX_SEMAPHORE_WAIT_TIME_MS );

static WIFIEventHandler_t xWifiEventHandlers[ eWiFiEventMax ]; 

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

static WIFIReason_t convertReasonCode( uint8_t reasonCode )
{
    WIFIReason_t retCode;
    switch( reasonCode )
    {
        case WIFI_REASON_AUTH_EXPIRE:
            retCode = eWiFiReasonAuthExpired;
            break;
        case WIFI_REASON_ASSOC_EXPIRE:
            retCode = eWiFiReasonAssocExpired;
            break;
        case WIFI_REASON_AUTH_LEAVE:
            retCode = eWiFiReasonAuthLeaveBSS;
            break;
        case WIFI_REASON_4WAY_HANDSHAKE_TIMEOUT:
            retCode = eWiFiReason4WayTimeout;
            break;
        case WIFI_REASON_BEACON_TIMEOUT:
            retCode = eWiFiReasonBeaconTimeout;
            break;
        case WIFI_REASON_AUTH_FAIL:
            retCode = eWiFiReasonAuthFailed;
            break;
        case WIFI_REASON_ASSOC_FAIL:
            retCode = eWiFiReasonAssocFailed;
            break;
        case WIFI_REASON_NO_AP_FOUND:
            retCode = eWiFiReasonAPNotFound;
            break;
        default:
            retCode = eWiFiReasonUnspecified;
            break;
    }
    return retCode;
} 
static void event_handler(void* arg, esp_event_base_t event_base, int32_t event_id, void* event_data)
{
    /* For accessing reason codes in case of disconnection */
    system_event_info_t *info = &((system_event_t*) event_data)->event_info;
    WIFIEvent_t eventInfo = { 0 };
    if (event_base == WIFI_EVENT) {
        switch(event_id) {
            case WIFI_EVENT_STA_START:
                ESP_LOGI(TAG, "WIFI_EVENT_STA_START");
                xEventGroupClearBits(wifi_event_group, STOPPED_BIT);
                xEventGroupSetBits(wifi_event_group, STARTED_BIT);
                break;
            case WIFI_EVENT_STA_STOP:
                ESP_LOGI(TAG, "WIFI_EVENT_STA_STOP");
                xEventGroupClearBits(wifi_event_group, STARTED_BIT);
                xEventGroupSetBits(wifi_event_group, STOPPED_BIT);
                break;
            case WIFI_EVENT_STA_CONNECTED:
                ESP_LOGI(TAG, "WIFI_EVENT_STA_CONNECTED");
                break;
            case WIFI_EVENT_STA_DISCONNECTED:
                ESP_LOGI(TAG, "WIFI_EVENT_STA_DISCONNECTED: %d", info->disconnected.reason);
                wifi_auth_failure = false;

                /* Set code corresponding to the reason for disconnection */
                switch (info->disconnected.reason) {
                    case WIFI_REASON_AUTH_EXPIRE:
                    case WIFI_REASON_ASSOC_EXPIRE:
                    case WIFI_REASON_AUTH_LEAVE:
                    case WIFI_REASON_4WAY_HANDSHAKE_TIMEOUT:
                    case WIFI_REASON_BEACON_TIMEOUT:
                    case WIFI_REASON_AUTH_FAIL:
                    case WIFI_REASON_ASSOC_FAIL:
                    case WIFI_REASON_HANDSHAKE_TIMEOUT:
                        ESP_LOGD(TAG, "STA Auth Error");
                        wifi_auth_failure = true;
                        break;
                    case WIFI_REASON_NO_AP_FOUND:
                        ESP_LOGD(TAG, "STA AP Not found");
                        wifi_auth_failure = true;
                        break;
                    default:
                        break;
                }

                wifi_conn_state = false;
                xEventGroupClearBits(wifi_event_group, CONNECTED_BIT);
                xEventGroupSetBits(wifi_event_group, DISCONNECTED_BIT);
                if( xWifiEventHandlers[ eWiFiEventDisconnected ] != NULL )
                {
                    eventInfo.xEventType = eWiFiEventDisconnected;
                    eventInfo.xInfo.xDisconnected.xReason = convertReasonCode( info->disconnected.reason );
                    xWifiEventHandlers[ eWiFiEventDisconnected ]( &eventInfo );
                }
                break;
            case WIFI_EVENT_AP_START:
                ESP_LOGI(TAG, "WIFI_EVENT_AP_START");
                wifi_ap_state = true;
                xEventGroupClearBits(wifi_event_group, AP_STOPPED_BIT);
                xEventGroupSetBits(wifi_event_group, AP_STARTED_BIT);
                break;
            case WIFI_EVENT_AP_STOP:
                ESP_LOGI(TAG, "WIFI_EVENT_AP_STOP");
                wifi_ap_state = false;
                xEventGroupClearBits(wifi_event_group, AP_STARTED_BIT);
                xEventGroupSetBits(wifi_event_group, AP_STOPPED_BIT);
                break;
            case WIFI_EVENT_AP_STACONNECTED:
                ESP_LOGI(TAG, "WIFI_EVENT_AP_STACONNECTED");
                break;
            case WIFI_EVENT_AP_STADISCONNECTED:
                ESP_LOGI(TAG, "WIFI_EVENT_AP_STADISCONNECTED");
                break;
            default:
                break;
        }
    } else if(event_base == IP_EVENT) {
            switch(event_id) {
                case IP_EVENT_STA_GOT_IP:
                    ESP_LOGI(TAG, "SYSTEM_EVENT_STA_GOT_IP");
                    wifi_conn_state = true;
                    xEventGroupClearBits(wifi_event_group, DISCONNECTED_BIT);
                    xEventGroupSetBits(wifi_event_group, CONNECTED_BIT);
                    if( xWifiEventHandlers[ eWiFiEventIPReady ] != NULL )
                    {
                        eventInfo.xEventType = eWiFiEventIPReady;
                        eventInfo.xInfo.xIPReady.xIPAddress.ulAddress[ 0 ] = info->got_ip.ip_info.ip.addr;
                        xWifiEventHandlers[ eWiFiEventIPReady ]( &eventInfo );
                    }
                    break;

                default:
                    break;
            }
    }
}
/*-----------------------------------------------------------*/

static void sc_callback(void* arg, esp_event_base_t event_base, int32_t event_id, void* event_data)
{
    switch (event_id) {
        case SC_EVENT_SCAN_DONE:
            ESP_LOGI(TAG, "SC_EVENT_SCAN_DONE");
            break;
        case SC_EVENT_FOUND_CHANNEL:
            ESP_LOGI(TAG, "SC_EVENT_FOUND_CHANNEL");
            break;
        case SC_EVENT_GOT_SSID_PSWD:
            ESP_LOGI(TAG, "SC_EVENT_GOT_SSID_PSWD");
            smartconfig_event_got_ssid_pswd_t *evt = (smartconfig_event_got_ssid_pswd_t *)event_data;
            wifi_config_t wifi_config;
            uint8_t ssid[33] = { 0 };
            uint8_t password[65] = { 0 };

            bzero(&wifi_config, sizeof(wifi_config_t));
            memcpy(wifi_config.sta.ssid, evt->ssid, sizeof(wifi_config.sta.ssid));
            memcpy(wifi_config.sta.password, evt->password, sizeof(wifi_config.sta.password));
            wifi_config.sta.bssid_set = evt->bssid_set;
            if (wifi_config.sta.bssid_set == true) {
                memcpy(wifi_config.sta.bssid, evt->bssid, sizeof(wifi_config.sta.bssid));
            }

            memcpy(ssid, evt->ssid, sizeof(evt->ssid));
            memcpy(password, evt->password, sizeof(evt->password));
            ESP_LOGI(TAG, "SSID:%s", ssid);
            esp_wifi_set_config(ESP_IF_WIFI_STA, &wifi_config);
            /* Use a private function to reset the memory block instead of memset, so that compiler wont optimize away the function call. */
            prvMemzero( &wifi_config, sizeof( wifi_config_t ) );
            prvMemzero( password, sizeof( password ) );
            esp_wifi_connect();
            break;
        case SC_EVENT_SEND_ACK_DONE:
            ESP_LOGI(TAG, "SC_EVENT_SEND_ACK_DONE");
            xEventGroupSetBits(wifi_event_group, ESPTOUCH_DONE_BIT);
            break;
        default:
            break;
    }
}

static esp_err_t esp_hal_wifi_start(void)
{
    esp_err_t ret;
    ret = esp_wifi_start();
    if (ret != ESP_OK) {
        return ret;
    }
    if (xEventGroupWaitBits(wifi_event_group, STARTED_BIT | AP_STARTED_BIT, pdFALSE, pdFALSE, portMAX_DELAY)) {
        wifi_started = true;
        return ESP_OK;
    }
    return ESP_FAIL;
}

static esp_err_t esp_hal_wifi_stop(void)
{
    esp_err_t ret;
    ret = esp_wifi_stop();
    if (ret != ESP_OK) {
        return ret;
    }
    if (xEventGroupWaitBits(wifi_event_group, STOPPED_BIT | AP_STOPPED_BIT, pdFALSE, pdFALSE, portMAX_DELAY)) {
        wifi_started = false;
        return ESP_OK;
    }
    return ESP_FAIL;
}

WIFIReturnCode_t WIFI_Provision()
{
    WIFIReturnCode_t wifi_ret = eWiFiFailure;
    esp_err_t ret;

    /* Try to acquire the semaphore. */
    if( xSemaphoreTake( xWiFiSem, xSemaphoreWaitTicks ) == pdTRUE )
    {
        ret = esp_wifi_set_mode(WIFI_MODE_STA);
        if (ret != ESP_OK) {
            ESP_LOGE(TAG, "%s: Failed to set wifi mode %d", __func__, ret);
            xSemaphoreGive( xWiFiSem );
            return wifi_ret;
        }

        ret = esp_hal_wifi_start();
        if (ret != ESP_OK) {
            ESP_LOGE(TAG, "%s: Failed to start wifi %d", __func__, ret);
            xSemaphoreGive( xWiFiSem );
            return wifi_ret;
        }

        esp_event_handler_register(SC_EVENT, ESP_EVENT_ANY_ID, &sc_callback, NULL);
        // Wait for wifi started event
        xEventGroupWaitBits(wifi_event_group, STARTED_BIT, pdFALSE, pdFALSE, portMAX_DELAY);
        esp_smartconfig_set_type(SC_TYPE_ESPTOUCH);
        smartconfig_start_config_t cfg = SMARTCONFIG_START_CONFIG_DEFAULT();
        esp_smartconfig_start(&cfg);
        if (ret != ESP_OK) {
            ESP_LOGE(TAG, "%s: Failed to start smartconfig %d", __func__, ret);
            xSemaphoreGive( xWiFiSem );
            return wifi_ret;
        }

        // Wait for wifi connected or disconnected event
        xEventGroupWaitBits(wifi_event_group, ESPTOUCH_DONE_BIT | DISCONNECTED_BIT, pdTRUE, pdFALSE, portMAX_DELAY);
        esp_smartconfig_stop();
        esp_event_handler_unregister(SC_EVENT, ESP_EVENT_ANY_ID, &sc_callback);
        if (wifi_conn_state == true) {
            wifi_ret = eWiFiSuccess;
        }
        /* Return the semaphore. */
        xSemaphoreGive( xWiFiSem );
    }

    return wifi_ret;
}

/*-----------------------------------------------------------*/

BaseType_t WIFI_IsConnected( const WIFINetworkParams_t * pxNetworkParams )
{
    BaseType_t xRetVal = pdFALSE;

    /* Try to acquire the semaphore. */
    if( xSemaphoreTake( xWiFiSem, xSemaphoreWaitTicks ) == pdTRUE )
    {
        if( ( wifi_conn_state == true) && 
            ( ( pxNetworkParams == NULL ) ||
              ( memcmp( pxNetworkParams->ucSSID, xConnectedAP.ucSSID, pxNetworkParams->ucSSIDLength ) == 0 ) ) )
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
    esp_err_t ret;
    if( xSemaphoreTake( xWiFiSem, xSemaphoreWaitTicks ) == pdTRUE )
    {
        if (wifi_conn_state == true) {
            ret = esp_wifi_disconnect();
            if (ret == ESP_OK) {
                // Wait for wifi disconnected event
                xEventGroupWaitBits(wifi_event_group, DISCONNECTED_BIT, pdTRUE, pdFALSE, portMAX_DELAY);
            } else {
                ESP_LOGE(TAG, "%s:Failed to disconnect wifi %d", __func__, ret);
                goto err;
            }
        }

        if ((ret = esp_wifi_deinit()) != ESP_OK) {
            if (ret == ESP_ERR_WIFI_NOT_STOPPED) {
                ret = esp_hal_wifi_stop();
                if (ret != ESP_OK) {
                    ESP_LOGE(TAG, "%s:Failed to stop wifi %d", __func__, ret);
                    goto err;
                }
                if (esp_wifi_deinit() != ESP_OK) {
                    ESP_LOGE(TAG, "%s:Failed to deinit %d", __func__, ret);
                    goto err;
                }
            } else {
                ESP_LOGE(TAG, "%s:Failed to deinit %d", __func__, ret);
                goto err;
            }
        }
        if (wifi_event_group) {
            vEventGroupDelete(wifi_event_group);
            wifi_event_group = NULL;
        }
        xSemaphoreGive( xWiFiSem );
        return eWiFiSuccess;
err:
        xSemaphoreGive( xWiFiSem );
        return eWiFiFailure;
    }
    return eWiFiFailure;
}
/*-----------------------------------------------------------*/

WIFIReturnCode_t WIFI_On( void )
{
    static bool event_loop_inited;
    esp_err_t ret;
    // Check if Event Loop is already initialized
    if (event_loop_inited == false) {
        ret = esp_event_loop_create_default();
        esp_netif_info = esp_netif_create_default_wifi_sta();
        esp_softap_netif_info = esp_netif_create_default_wifi_ap();
        if (ret != ESP_OK) {
            ESP_LOGE(TAG, "%s: Failed to init event loop %d", __func__, ret);
            goto err;
        }
        esp_event_handler_instance_t instance_any_id;
        esp_event_handler_instance_t instance_got_ip;
        esp_event_handler_instance_register(WIFI_EVENT, ESP_EVENT_ANY_ID, &event_handler, NULL, &instance_any_id);
        esp_event_handler_instance_register(IP_EVENT, IP_EVENT_STA_GOT_IP, &event_handler, NULL, &instance_got_ip);
        event_loop_inited = true;
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
    if (xWiFiSem == NULL) {
        xWiFiSem = xSemaphoreCreateMutexStatic( &( xSemaphoreBuffer ) );
    }

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
    wifi_mode_t xCurMode;
    esp_err_t ret;
    WIFIReturnCode_t wifi_ret = eWiFiFailure;

    if( pxNetworkParams == NULL )
    {
        return wifi_ret;
    }

    if( !CHECK_VALID_SSID_LEN(pxNetworkParams->ucSSIDLength) )
    {
        return wifi_ret;
    }

    if( pxNetworkParams->xSecurity == eWiFiSecurityWEP )
    {
        return eWiFiNotSupported;
    }

    if( pxNetworkParams->xSecurity != eWiFiSecurityOpen && !CHECK_VALID_PASSPHRASE_LEN(pxNetworkParams->xPassword.xWPA.ucLength))
    {
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
        strlcpy((char *) &wifi_config.sta.ssid, (char *) pxNetworkParams->ucSSID, pxNetworkParams->ucSSIDLength + 1);

        ret = esp_wifi_get_mode( &xCurMode );
        if (ret != ESP_OK) {
            ESP_LOGE(TAG, "%s: Failed to get wifi mode %d", __func__, ret);
            xSemaphoreGive( xWiFiSem );
        	return eWiFiFailure;
        }

        if( xCurMode != WIFI_MODE_STA && xCurMode != WIFI_MODE_APSTA )
        {
            ret = esp_wifi_set_mode(WIFI_MODE_STA);
            if (ret != ESP_OK) {
        		ESP_LOGE(TAG, "%s: Failed to set wifi mode %d", __func__, ret);
                xSemaphoreGive( xWiFiSem );
                return wifi_ret;
            }
        }
        if (!wifi_started) {
            ret = esp_hal_wifi_start();
            if (ret != ESP_OK) {
                ESP_LOGE(TAG, "%s: Failed to start wifi %d", __func__, ret);
                xSemaphoreGive( xWiFiSem );
                return wifi_ret;
            }
        }

        if (pxNetworkParams->xSecurity != eWiFiSecurityOpen) {
            strlcpy((char *) &wifi_config.sta.password, pxNetworkParams->xPassword.xWPA.cPassphrase, pxNetworkParams->xPassword.xWPA.ucLength + 1);
        }

        ret = esp_wifi_set_config(ESP_IF_WIFI_STA, &wifi_config);
        /* Use a private function to reset the memory block instead of memset, so that compiler wont optimize away the function call. */
        prvMemzero( &wifi_config, sizeof( wifi_config_t ) );
        if (ret != ESP_OK) {
            ESP_LOGE(TAG, "%s: Failed to set wifi config %d", __func__, ret);
            xSemaphoreGive( xWiFiSem );
            return wifi_ret;
        }

        esp_wifi_connect();

        // Wait for wifi connected or disconnected event
        xEventGroupWaitBits(wifi_event_group, CONNECTED_BIT | DISCONNECTED_BIT, pdTRUE, pdFALSE, portMAX_DELAY);
        if (wifi_conn_state == true) {
            xConnectedAP = *pxNetworkParams;
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
    wifi_config_t wifi_config = { 0 };
    esp_err_t ret;
    wifi_mode_t xCurMode;
 
    if (pxBuffer == NULL || ucNumNetworks == 0) {
        return eWiFiFailure;
    }

    wifi_scan_config_t scanConf = {
        .ssid = NULL,
        .bssid = NULL,
        .channel = 0,
        .show_hidden = false
    };

    wifi_config.sta.scan_method = WIFI_ALL_CHANNEL_SCAN;

    /* Try to acquire the semaphore. */
    if( xSemaphoreTake( xWiFiSem, xSemaphoreWaitTicks ) == pdTRUE )
    {
    	ret = esp_wifi_get_mode( &xCurMode );
    	if (ret != ESP_OK) {
        		ESP_LOGE(TAG, "%s: Failed to set wifi mode %d", __func__, ret);
        		xSemaphoreGive( xWiFiSem );
        		return eWiFiFailure;
        }

    	if( xCurMode != WIFI_MODE_STA )
    	{

    		ret = esp_wifi_set_mode(WIFI_MODE_STA);
    		if (ret != ESP_OK) {
    			ESP_LOGE(TAG, "%s: Failed to set wifi mode %d", __func__, ret);
    			xSemaphoreGive( xWiFiSem );
    			return eWiFiFailure;
    		}

    		ret = esp_wifi_set_config(ESP_IF_WIFI_STA, &wifi_config);
    		if (ret != ESP_OK) {
    			ESP_LOGE(TAG, "%s: Failed to set wifi config %d", __func__, ret);
    			xSemaphoreGive( xWiFiSem );
    			return eWiFiFailure;
    		}
    	}

        if (!wifi_started) {
            ret = esp_hal_wifi_start();
            if (ret != ESP_OK) {
                ESP_LOGE(TAG, "%s: Failed to start wifi %d", __func__, ret);
                xSemaphoreGive( xWiFiSem );
                return eWiFiFailure;
            }
        }

        if ( wifi_conn_state == false && wifi_auth_failure == true )
        {
            /* It seems that WiFi needs explicit disassoc before scan request post
             * attempt to connect to invalid network name or SSID.
             */
            esp_wifi_disconnect();
            xEventGroupWaitBits(wifi_event_group, DISCONNECTED_BIT, pdTRUE, pdFALSE, 30);
        }


    	ret = esp_wifi_scan_start(&scanConf, true);
        if (ret == ESP_OK)
        {
            wifi_ap_record_t *ap_info = calloc(1, sizeof(wifi_ap_record_t) * ucNumNetworks);
            if (ap_info != NULL)
            {
                uint16_t num_aps = ucNumNetworks;
                esp_wifi_scan_get_ap_records(&num_aps, ap_info);
                for (int i = 0; i < num_aps; i++) {
                    pxBuffer[i].ucSSIDLength = strlcpy((char *)pxBuffer[i].ucSSID, (const char *)ap_info[i].ssid, wificonfigMAX_SSID_LEN + 1);
                    memcpy(pxBuffer[i].ucBSSID, ap_info[i].bssid, wificonfigMAX_BSSID_LEN);
                    pxBuffer[i].cRSSI = ap_info[i].rssi;
                    pxBuffer[i].ucChannel = ap_info[i].primary;
                    switch(ap_info[i].authmode)
                    {
                    case WIFI_AUTH_OPEN:
                    	pxBuffer[i].xSecurity = eWiFiSecurityOpen;
                    	break;
                    case WIFI_AUTH_WEP:
                    	pxBuffer[i].xSecurity =  eWiFiSecurityWEP;
                    	break;
                    case WIFI_AUTH_WPA_PSK:
                    	pxBuffer[i].xSecurity =  eWiFiSecurityWPA;
                    	break;
                    case WIFI_AUTH_WPA_WPA2_PSK:
                    case WIFI_AUTH_WPA2_PSK:
                    	pxBuffer[i].xSecurity =  eWiFiSecurityWPA2;
                    	break;
                    case WIFI_AUTH_WPA2_ENTERPRISE:
                    default:
                    	pxBuffer[i].xSecurity  = eWiFiSecurityNotSupported;
                    	break;
                    }
                }
                free(ap_info);
                xRetVal = eWiFiSuccess;
            }
            else
            {
                ESP_LOGE(TAG, "%s: Failed to alloc %lu bytes for WIFI provisionning", __func__, (long unsigned int)(sizeof(wifi_ap_record_t) * ucNumNetworks));
                ESP_LOGE(TAG, "%s: Current heap: %lu, min heap %lu", __func__, (long unsigned int)xPortGetFreeHeapSize(),(long unsigned int)xPortGetMinimumEverFreeHeapSize());
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


esp_err_t prxSerializeWifiNetworkProfile( const WIFINetworkProfile_t * const pxNetwork, uint8_t *pucBuffer, uint32_t* pulSize )
{
	esp_err_t xRet = ESP_OK;
	uint32_t ulSize;

	if( pucBuffer == NULL )
	{
        /**
         * Add size with null termination in the flash.
         */
		ulSize = pxNetwork->ucSSIDLength + 1;
		ulSize += pxNetwork->ucPasswordLength + 1;
		ulSize += wificonfigMAX_BSSID_LEN;
		ulSize += MAX_SECURITY_MODE_LEN;

		*pulSize = ulSize;
	}
	else
	{
		ulSize = *pulSize;
		memset(pucBuffer, 0x00, ulSize);
		if( pxNetwork->ucSSIDLength < ulSize )
		{
			memcpy(pucBuffer, pxNetwork->ucSSID, pxNetwork->ucSSIDLength );
			pucBuffer += ( pxNetwork->ucSSIDLength + 1 );
			ulSize -= ( pxNetwork->ucSSIDLength + 1 );
		}
		else
		{
			xRet = ESP_FAIL;
		}

		if( xRet == ESP_OK )
		{
			if( pxNetwork->ucPasswordLength < ulSize )
			{
				memcpy(pucBuffer, pxNetwork->cPassword, pxNetwork->ucPasswordLength );
				pucBuffer += (pxNetwork->ucPasswordLength+1);
				ulSize -= (pxNetwork->ucPasswordLength+1);
			}
			else
			{
				xRet = ESP_FAIL;
			}
		}


		if( xRet == ESP_OK )
		{
			if( wificonfigMAX_BSSID_LEN < ulSize )
			{
				memcpy(pucBuffer, pxNetwork->ucBSSID, wificonfigMAX_BSSID_LEN  );
				pucBuffer += wificonfigMAX_BSSID_LEN;
				ulSize -= wificonfigMAX_BSSID_LEN;
			}
			else
			{
				xRet = ESP_FAIL;
			}
		}

		if( xRet == ESP_OK )
		{
			if( ulSize >= MAX_SECURITY_MODE_LEN )
			{
				*pucBuffer = (uint8_t) pxNetwork->xSecurity;
			}
			else
			{
				xRet = ESP_FAIL;
			}
		}

	}

	return xRet;

}


esp_err_t prxDeSerializeWifiNetworkProfile( WIFINetworkProfile_t * const pxNetwork, uint8_t *pucBuffer, uint32_t ulSize )
{
	esp_err_t xRet = ESP_OK;
	uint32_t ulLen;

	ulLen = strlen( ( char * )pucBuffer );
	if( ulLen <= wificonfigMAX_SSID_LEN )
	{
		memcpy(pxNetwork->ucSSID, pucBuffer, ulLen );
		pxNetwork->ucSSIDLength = ulLen;
		pucBuffer += ( ulLen + 1 );
	}
	else
	{
		xRet = ESP_FAIL;
	}

	if( xRet == ESP_OK )
	{
		ulLen = strlen( ( char *) pucBuffer );
		if( ulLen <= wificonfigMAX_PASSPHRASE_LEN )
		{
			memcpy(pxNetwork->cPassword, pucBuffer, ulLen );
			pxNetwork->ucPasswordLength = ulLen;
			pucBuffer += ( ulLen + 1 );
		}
		else
		{
			xRet = ESP_FAIL;
		}
	}

	if( xRet == ESP_OK )
	{
		memcpy(pxNetwork->ucBSSID, pucBuffer, wificonfigMAX_BSSID_LEN );
		pucBuffer += wificonfigMAX_BSSID_LEN;
		pxNetwork->xSecurity = ( WIFISecurity_t ) *pucBuffer;
	}


	return xRet;
}

esp_err_t prvStoreWIFINetwork( nvs_handle xNvsHandle, const WIFINetworkProfile_t* pxNetwork, uint16_t usIndex )
{
	uint32_t ulSize = 0;
	uint8_t *pucBuffer;
    char cWifiKey[ MAX_WIFI_KEY_WIDTH ] = { 0 };
	esp_err_t xRet = ESP_FAIL;

	(void) prxSerializeWifiNetworkProfile( pxNetwork, NULL, &ulSize );
	pucBuffer = calloc( 1,  ulSize );

	if( pucBuffer != NULL )
	{
		xRet = prxSerializeWifiNetworkProfile( pxNetwork, pucBuffer, &ulSize );
	}

	if( xRet == ESP_OK )
	{
		snprintf(cWifiKey, MAX_WIFI_KEY_WIDTH, "%d", usIndex );
		xRet = nvs_set_blob( xNvsHandle, cWifiKey, pucBuffer, ulSize );
		free(pucBuffer);
	}


	return xRet;
}

esp_err_t prvGetWIFINetwork( nvs_handle xNvsHandle, WIFINetworkProfile_t* pxNetwork, uint16_t usIndex )
{
	uint32_t ulSize = 0;
	uint8_t *pucBuffer = NULL;
    char cWifiKey[ MAX_WIFI_KEY_WIDTH ] = { 0 };
	esp_err_t xRet = ESP_FAIL;

	if( pxNetwork != NULL )
	{
		snprintf(cWifiKey, MAX_WIFI_KEY_WIDTH, "%d", usIndex);
		xRet = nvs_get_blob( xNvsHandle, cWifiKey, NULL, &ulSize );

		if( xRet == ESP_OK )
		{
			pucBuffer = calloc( 1,  ulSize );
			if( pucBuffer != NULL )
			{
				xRet = nvs_get_blob( xNvsHandle, cWifiKey, pucBuffer, &ulSize );
			}
			else
			{
				xRet = ESP_FAIL;
			}
		}

		if( xRet == ESP_OK )
		{
			xRet =  prxDeSerializeWifiNetworkProfile( pxNetwork, pucBuffer, ulSize );
			free(pucBuffer);
		}
	}

	return xRet;
}

esp_err_t prvInitRegistry( nvs_handle xNvsHandle )
{
	esp_err_t xRet;
	uint32_t ulSize = sizeof( StorageRegistry_t );
	xRet = nvs_get_blob( xNvsHandle, "registry", &xRegistry, &ulSize );


	if( xRet == ESP_ERR_NVS_NOT_FOUND )
	{
		memset( &xRegistry, 0x00, sizeof( StorageRegistry_t ) );
		xRet = ESP_OK;
	}
	if( xRet == ESP_OK )
	{
		xIsRegistryInit = pdTRUE;
	}

	return xRet;
}


WIFIReturnCode_t WIFI_NetworkAdd( const WIFINetworkProfile_t * const pxNetworkProfile,
                                  uint16_t * pusIndex )
{
    WIFIReturnCode_t xWiFiRet = eWiFiFailure;
    esp_err_t xRet;
    nvs_handle xNvsHandle;
    BaseType_t xOpened = pdFALSE;

    if( pxNetworkProfile != NULL && pusIndex != NULL )
    {
    	/* Try to acquire the semaphore. */
    	if( xSemaphoreTake( xWiFiSem, xSemaphoreWaitTicks ) == pdTRUE )
    	{
    		xRet = nvs_open(WIFI_FLASH_NS, NVS_READWRITE, &xNvsHandle);
    		if( xRet == ESP_OK )
    		{
    			xOpened = pdTRUE;
    			if( xIsRegistryInit == pdFALSE )
    			{
    				xRet = prvInitRegistry( xNvsHandle );
    			}
    		}
    		if( xRet == ESP_OK )
    		{
    			if( xRegistry.usNumNetworks == wificonfigMAX_NETWORK_PROFILES )
    			{
    				xRet = ESP_FAIL;
    			}
    		}

    		if( xRet == ESP_OK )
    		{
    			xRet = prvStoreWIFINetwork( xNvsHandle, pxNetworkProfile, xRegistry.usNextStorageIdx );
    		}


    		if( xRet == ESP_OK )
    		{

    			xRegistry.usStorageIdx[ xRegistry.usNumNetworks ] =  xRegistry.usNextStorageIdx;
    			xRegistry.usNextStorageIdx++;
    			xRegistry.usNumNetworks++;
    			xRet = nvs_set_blob( xNvsHandle, "registry", &xRegistry, sizeof( xRegistry ) );
    		}

    		// Commit
    		if( xRet == ESP_OK )
    		{
    			xRet = nvs_commit( xNvsHandle );
    		}

    		if( xRet == ESP_OK )
    		{
    			*pusIndex = ( xRegistry.usNumNetworks - 1 );
    			xWiFiRet = eWiFiSuccess;
    		}

    		// Close
    		if( xOpened )
    		{
    			nvs_close( xNvsHandle );
    		}

    		xSemaphoreGive( xWiFiSem );

    	}
    }

    return xWiFiRet;
}
/*-----------------------------------------------------------*/

WIFIReturnCode_t WIFI_NetworkGet( WIFINetworkProfile_t * pxNetworkProfile,
                                  uint16_t usIndex )
{
	WIFIReturnCode_t xWiFiRet = eWiFiFailure;
	esp_err_t xRet;
	nvs_handle xNvsHandle;
	BaseType_t xOpened = pdFALSE;

	/* Try to acquire the semaphore. */
	if( pxNetworkProfile != NULL )
	{
		if( xSemaphoreTake( xWiFiSem, xSemaphoreWaitTicks ) == pdTRUE )
		{
			xRet = nvs_open(WIFI_FLASH_NS, NVS_READWRITE, &xNvsHandle);
			if( xRet == ESP_OK )
			{
				xOpened = pdTRUE;
				if( xIsRegistryInit == pdFALSE )
				{
					xRet = prvInitRegistry( xNvsHandle );
				}
			}
			if( xRet == ESP_OK )
			{
				if( usIndex < xRegistry.usNumNetworks )
				{
					xRet = prvGetWIFINetwork( xNvsHandle, pxNetworkProfile, xRegistry.usStorageIdx[ usIndex ] );
				}
				else
				{
					xRet = ESP_FAIL;
				}
			}

			if( xRet == ESP_OK )
			{
				xWiFiRet = eWiFiSuccess;
			}
			// Close
			if( xOpened )
			{
				nvs_close( xNvsHandle );
			}
			xSemaphoreGive( xWiFiSem );
		}
	}

	return xWiFiRet;
}
/*-----------------------------------------------------------*/

WIFIReturnCode_t WIFI_NetworkDelete( uint16_t usIndex )
{
	WIFIReturnCode_t xWiFiRet = eWiFiFailure;
	esp_err_t xRet;
	nvs_handle xNvsHandle;
	char cWifiKey[ MAX_WIFI_KEY_WIDTH ] = { 0 };
	BaseType_t xOpened = pdFALSE;
	uint16_t usIdx;

	/* Try to acquire the semaphore. */
	if( xSemaphoreTake( xWiFiSem, xSemaphoreWaitTicks ) == pdTRUE )
	{
		xRet = nvs_open(WIFI_FLASH_NS, NVS_READWRITE, &xNvsHandle);
		if( xRet == ESP_OK )
		{
			xOpened = pdTRUE;
			if( xIsRegistryInit == pdFALSE )
			{
				xRet = prvInitRegistry( xNvsHandle );
			}
		}
		if( xRet == ESP_OK && usIndex < xRegistry.usNumNetworks )
		{
			snprintf(cWifiKey, MAX_WIFI_KEY_WIDTH, "%d", xRegistry.usStorageIdx[ usIndex ]);
			xRet = nvs_erase_key( xNvsHandle, cWifiKey );

			if( xRet == ESP_OK )
			{
				for( usIdx = usIndex + 1; usIdx < xRegistry.usNumNetworks; usIdx++ )
				{
					xRegistry.usStorageIdx[ usIdx - 1 ] = xRegistry.usStorageIdx[ usIdx ];
				}
				xRegistry.usNumNetworks--;
				xRet = nvs_set_blob( xNvsHandle, "registry", &xRegistry, sizeof( xRegistry ) );
			}

			if( xRet == ESP_OK )
			{
				xRet = nvs_commit( xNvsHandle );
			}
		}

		if( xRet == ESP_OK )
		{
			xWiFiRet = eWiFiSuccess;
		}

		// Close
		if( xOpened )
		{
			nvs_close( xNvsHandle );
		}
		xSemaphoreGive( xWiFiSem );
	}

	return xWiFiRet;
}
/*-----------------------------------------------------------*/

WIFIReturnCode_t WIFI_Ping( uint8_t * pucIPAddr,
                            uint16_t usCount,
                            uint32_t ulIntervalMS )
{
    return eWiFiNotSupported;
}
/*-----------------------------------------------------------*/
WIFIReturnCode_t WIFI_GetIPInfo( WIFIIPConfiguration_t * pxIPInfo )
{
    WIFIReturnCode_t xRetVal = eWiFiFailure;
    uint8_t *pucIPAddr;

    if (pxIPInfo == NULL) {
        return xRetVal;
    }

    memset( pxIPInfo, 0x00, sizeof( WIFIIPConfiguration_t ) );
    /* Try to acquire the semaphore. */
    if( xSemaphoreTake( xWiFiSem, xSemaphoreWaitTicks ) == pdTRUE )
    {
#if !AFR_ESP_LWIP
        pxIPInfo->xIPAddress.ulAddress[0] = FreeRTOS_GetIPAddress();
        pucIPAddr = ( uint8_t * ) ( &pxIPInfo->xIPAddress.ulAddress[0] );
        xRetVal = eWiFiSuccess;
        configPRINTF(("%s: local ip address is %d.%d.%d.%d\n",
                     __FUNCTION__,
                     pucIPAddr[0],
                     pucIPAddr[1],
                     pucIPAddr[2],
                     pucIPAddr[3]));
#else /* running lwip */
        esp_netif_ip_info_t ipInfo;
        int ret;

        ret = esp_netif_get_ip_info(esp_netif_info, &ipInfo);
        if (ret == ESP_OK)
        {
            xRetVal = eWiFiSuccess;
            pxIPInfo->xIPAddress.ulAddress[0] = ipInfo.ip.addr;
            pucIPAddr = ( uint8_t * ) ( &pxIPInfo->xIPAddress.ulAddress[0] );
            configPRINTF(("%s: local ip address is %d.%d.%d.%d\n",
                          __FUNCTION__,
                          pucIPAddr[0],
                          pucIPAddr[1],
                          pucIPAddr[2],
                          pucIPAddr[3]));
        }
        else
        {
            configPRINTF(("%s: esp_netif_get_ip_info_error:  %d",
                        __FUNCTION__,
                        ret));
        }
#endif
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
        wifi_mode_t mode;
        esp_err_t ret = esp_wifi_get_mode(&mode);
        if( ret == ESP_OK )
        {
            if( mode == WIFI_MODE_STA )
            {
                ret = esp_wifi_get_mac(WIFI_IF_STA, pucMac);
            }
            else if( mode == WIFI_MODE_AP )
            {
                ret = esp_wifi_get_mac(WIFI_IF_AP, pucMac);
            }
            else
            {
                ret = ESP_ERR_INVALID_ARG;
            }
        }
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

    if (pcHost == NULL || pucIPAddr == NULL) {
        return xRetVal;
    }

    /* Try to acquire the semaphore. */
    if( xSemaphoreTake( xWiFiSem, xSemaphoreWaitTicks ) == pdTRUE )
    {
#if AFR_ESP_LWIP
        struct hostent *he;
        struct in_addr **addr_list;
        he = gethostbyname(pcHost);
        if (he != NULL) {
            addr_list = (struct in_addr **)he->h_addr_list;
            memcpy(pucIPAddr, addr_list[0], sizeof(uint32_t));
            xRetVal = eWiFiSuccess;
        }
#else
        uint32_t IPAddr;
        IPAddr = FreeRTOS_gethostbyname( pcHost );
        if (IPAddr != 0UL)
        {
            *( ( uint32_t * ) pucIPAddr ) = IPAddr;
            xRetVal = eWiFiSuccess;
        }
#endif
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
    WIFIReturnCode_t wifi_ret = eWiFiFailure;
    if ( xSemaphoreTake( xWiFiSem, xSemaphoreWaitTicks ) == pdTRUE )
    {
        if (wifi_ap_state == true) {
            xSemaphoreGive( xWiFiSem );
            return eWiFiSuccess;
        }
        if (!wifi_started) {
            esp_err_t ret = esp_hal_wifi_start();
            if (ret != ESP_OK) {
                ESP_LOGE(TAG, "%s: Failed to start wifi %d", __func__, ret);
                xSemaphoreGive( xWiFiSem );
                return wifi_ret;
            }
        }

        /* Enable DHCP server, if it's not already enabled */
        if ( ESP_ERR_ESP_NETIF_INVALID_PARAMS == esp_netif_dhcps_start(esp_softap_netif_info))
        {
            ESP_LOGE(TAG, "%s: Failed to start DHCP server", __func__);
            return eWiFiFailure;
        }

        // Wait for wifi started event
        xEventGroupWaitBits(wifi_event_group, AP_STARTED_BIT, pdFALSE, pdFALSE, portMAX_DELAY);
        wifi_ret = eWiFiSuccess;

        /* Return the semaphore. */
        xSemaphoreGive( xWiFiSem );
    }
    return wifi_ret;
}
/*-----------------------------------------------------------*/

WIFIReturnCode_t WIFI_StopAP( void )
{
     WIFIReturnCode_t xRetVal = eWiFiFailure;

    /* Try to acquire the semaphore. */
    if( xSemaphoreTake( xWiFiSem, xSemaphoreWaitTicks ) == pdTRUE )
    {
        // Check if AP is already stopped
        if (wifi_ap_state == false) {
            xSemaphoreGive( xWiFiSem );
            return eWiFiSuccess;
        }

        esp_err_t ret = esp_hal_wifi_stop();
        if (ret == ESP_OK)
        {
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

static esp_err_t WIFI_SetSecurity( WIFISecurity_t securityMode, wifi_auth_mode_t * authmode)
{
    switch( securityMode )
    {
        case eWiFiSecurityOpen:
            *authmode = WIFI_AUTH_OPEN;
            break;
        case eWiFiSecurityWEP:
            *authmode = WIFI_AUTH_WEP;
            break;
        case eWiFiSecurityWPA:
            *authmode = WIFI_AUTH_WPA_PSK;
            break;
        case eWiFiSecurityWPA2:
            *authmode = WIFI_AUTH_WPA2_PSK;
            break;
        default:
            return ESP_FAIL;
    }
    return ESP_OK;
}

WIFIReturnCode_t WIFI_ConfigureAP( const WIFINetworkParams_t * const pxNetworkParams )
{
    wifi_config_t wifi_config = {
        .ap = {
            .max_connection = MAX_AP_CONNECTIONS,
        },
    };
    esp_err_t ret;
    WIFIReturnCode_t wifi_ret = eWiFiFailure;

    if (pxNetworkParams == NULL )
    {
        return wifi_ret;
    }

    if (pxNetworkParams->xSecurity == eWiFiSecurityWEP )
    {
        return eWiFiSecurityNotSupported;
    }

    if (!CHECK_VALID_SSID_LEN(pxNetworkParams->ucSSIDLength))
    {
        return wifi_ret;
    }

    if( pxNetworkParams->xSecurity != eWiFiSecurityOpen && !CHECK_VALID_PASSPHRASE_LEN(pxNetworkParams->xPassword.xWPA.ucLength))
    {
        return wifi_ret;
    }

    if( xSemaphoreTake( xWiFiSem, xSemaphoreWaitTicks ) == pdTRUE )
    {
        // Check if AP is already started
        if (wifi_ap_state == true) {
            esp_err_t ret = esp_hal_wifi_stop();
            if (ret != ESP_OK)
            {
                ESP_LOGE(TAG, "%s:Failed to stop wifi %d", __func__, ret);
            }
        }

        /* ssid/password is required */
        /* SSID can be a non NULL terminated string if ssid_len is specified.
         * Hence, memcpy is used to support 32 character long SSID name.
         */
        memcpy((char *) &wifi_config.ap.ssid, pxNetworkParams->ucSSID, pxNetworkParams->ucSSIDLength);
        wifi_config.ap.ssid_len = pxNetworkParams->ucSSIDLength;

        ret = WIFI_SetSecurity(pxNetworkParams->xSecurity, &wifi_config.ap.authmode);
        if (ret != ESP_OK) {
            ESP_LOGE(TAG, "%s: Invalid security mode %d", __func__, ret);
            xSemaphoreGive( xWiFiSem );
            return wifi_ret;
        }

        ret = esp_wifi_set_mode(WIFI_MODE_AP);
        if (ret != ESP_OK) {
            ESP_LOGE(TAG, "%s: Failed to set wifi mode %d", __func__, ret);
            xSemaphoreGive( xWiFiSem );
            return wifi_ret;
        }

        if (pxNetworkParams->xSecurity != eWiFiSecurityOpen) {
            strlcpy((char *) &wifi_config.ap.password, pxNetworkParams->xPassword.xWPA.cPassphrase, pxNetworkParams->xPassword.xWPA.ucLength + 1);
        }

        ret = esp_wifi_set_config(ESP_IF_WIFI_AP, &wifi_config);
        /* Use a private function to reset the memory block instead of memset, so that compiler wont optimize away the function call. */
        prvMemzero( &wifi_config, sizeof( wifi_config_t ) );
        if (ret != ESP_OK) {
            ESP_LOGE(TAG, "%s: Failed to set wifi config %d", __func__, ret);
            xSemaphoreGive( xWiFiSem );
            return wifi_ret;
        }

        wifi_ret = eWiFiSuccess;
        /* Return the semaphore. */
        xSemaphoreGive( xWiFiSem );
    }

    return wifi_ret;
}
/*-----------------------------------------------------------*/

WIFIReturnCode_t WIFI_SetPMMode( WIFIPMMode_t xPMModeType,
                                 const void * pvOptionValue )
{
    esp_err_t ret;
    WIFIReturnCode_t wifi_ret = eWiFiFailure;
    wifi_ps_type_t wifi_pm_mode = WIFI_PS_NONE;

    configASSERT( pvOptionValue != NULL );

    if( xSemaphoreTake( xWiFiSem, xSemaphoreWaitTicks ) == pdTRUE )
    {
        switch (xPMModeType)
        {
            case eWiFiPMNormal:
                wifi_pm_mode = WIFI_PS_MIN_MODEM;
                break;
            case eWiFiPMLowPower:
                wifi_pm_mode = WIFI_PS_MAX_MODEM;
                break;
            case eWiFiPMAlwaysOn:
                wifi_pm_mode = WIFI_PS_NONE;
                break;
            case eWiFiPMNotSupported:
                return eWiFiNotSupported;
                break;
        }
        ret = esp_wifi_set_ps( wifi_pm_mode );
        if (ret != ESP_OK) {
            ESP_LOGE(TAG, "%s: Failed to set wifi power mode %d", __func__, ret);
            xSemaphoreGive( xWiFiSem );
            return wifi_ret;
        }
        wifi_ret = eWiFiSuccess;
        xSemaphoreGive( xWiFiSem );
    }
    else
    {
        wifi_ret = eWiFiTimeout;
    }
    return wifi_ret;
}
/*-----------------------------------------------------------*/

WIFIReturnCode_t WIFI_GetPMMode( WIFIPMMode_t * pxPMModeType,
                                 void * pvOptionValue )
{
    esp_err_t ret;
    WIFIReturnCode_t wifi_ret = eWiFiFailure;
    wifi_ps_type_t wifi_pm_mode;

    configASSERT( pxPMModeType != NULL );
    configASSERT( pvOptionValue != NULL );

    if( xSemaphoreTake( xWiFiSem, xSemaphoreWaitTicks ) == pdTRUE )
    {
        ret = esp_wifi_get_ps( &wifi_pm_mode );
        if (ret != ESP_OK) {
            ESP_LOGE(TAG, "%s: Failed to set wifi power mode %d", __func__, ret);
            xSemaphoreGive( xWiFiSem );
            return wifi_ret;
        }
        else{
            switch ( wifi_pm_mode )
            {
                case WIFI_PS_NONE:
                    *pxPMModeType = eWiFiPMAlwaysOn;
                    break;
                case WIFI_PS_MIN_MODEM:
                    *pxPMModeType = eWiFiPMNormal;
                    break;
                case WIFI_PS_MAX_MODEM:
                    *pxPMModeType = eWiFiPMLowPower;
                    break;
                default:
                    *pxPMModeType = eWiFiPMNotSupported;
                    break;
            }
            wifi_ret = eWiFiSuccess;
            xSemaphoreGive( xWiFiSem );
        }
    }
    else
    {
        wifi_ret = eWiFiTimeout;
    }
    return wifi_ret;
}
/*-----------------------------------------------------------*/
WIFIReturnCode_t WIFI_RegisterEvent( WIFIEventType_t xEventType,
                                     WIFIEventHandler_t xHandler )
{
    WIFIReturnCode_t xWiFiRet;

    switch( xEventType )
    {
        case eWiFiEventIPReady:
        case eWiFiEventDisconnected:
            xWifiEventHandlers[ xEventType ] = xHandler;
            xWiFiRet = eWiFiSuccess;
            break;
        default:
            xWiFiRet = eWiFiNotSupported;
            break;
    }


    return xWiFiRet;
}
/*-----------------------------------------------------------*/
