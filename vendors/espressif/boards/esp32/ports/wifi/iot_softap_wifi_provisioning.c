/* OS */
//#include "FreeRTOS.h"

/* HW abstracted APIs*/
#include "iot_wifi.h"
#include "iot_softap_wifi_provisioning.h"
#include "aws_wifi_config.h"
#include "stdbool.h"

/* Port specific */
#include "esp_wifi.h"

#ifdef CONFIG_POP
    #undef CONFIG_POP
#endif
#define CONFIG_POP "abcd1234"

#define DEBUG


/* TODO:
 *  - Should probably refactor ble/softap provisioning to share a code base 
 *      - Storage of WIFI credentials should be refactored out
 *  - Need to use AFR logging API 
 *  - Update all APIs to single point of return 
 *  - Fix the build/bug issue with coreHTTP 
 *  - At init `xProvisionedParams` should be reset to default (sec=wpa2)
 * */

/* -------------------------------------------------------------------- */
/* TODO:   ***************************** This code should be refactored out of ble-wifi-prov                  */
/* -------------------------------------------------------------------- */
uint32_t IotBleWifiProv_GetNumNetworks( void );

/* -------------------------------------------------------------------- */
/*                              PRIVATE                                 */
/* -------------------------------------------------------------------- */

/* SoftAP based Provisioning Example
   This example code is in the Public Domain (or CC0 licensed, at your option.)
   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/

#include <string.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <esp_system.h>
#include <esp_wifi.h>
#include <esp_event_loop.h>

#define LOG_LOCAL_LEVEL ESP_LOG_DEBUG

#include <esp_log.h>
#include <nvs_flash.h>

#include <lwip/err.h>
#include <lwip/sys.h>


#include <protocomm.h>
#include <protocomm_httpd.h>
#include <protocomm_security0.h>
#include <protocomm_security1.h>
#include <wifi_provisioning/wifi_config.h>

#include "iot_softap_wifi_provisioning.h"

static const char *TAG = "app";


static protocomm_t *pxProtocomm = NULL;

static WIFINetworkProfile_t xProvisionedParams = {
    .ucBSSID = { 0 },
    .ucSSID = { 0 },
    .ucSSIDLength = 0,
    .xSecurity = eWiFiSecurityWPA2,
    .cPassword = { 0 },
    .ucPasswordLength = 0
};

static protocomm_security_pop_t pop = { 
    .data = (uint8_t *) CONFIG_POP,
    .len = (sizeof(CONFIG_POP)-1)
};  

/* -------------------------------------------------------------------- */
/*                            PROV HANDLERS                             */
/* -------------------------------------------------------------------- */
static inline void prvNetworkProfile2Params( WIFINetworkProfile_t const * const pxNetworkProfile, WIFINetworkParams_t * pxNetworkParams)
{
    pxNetworkParams->ucSSIDLength = pxNetworkProfile->ucSSIDLength;
    memcpy( pxNetworkParams->ucSSID, pxNetworkProfile->ucSSID, pxNetworkProfile->ucSSIDLength );
    pxNetworkParams->xPassword.xWPA.ucLength = pxNetworkProfile->ucPasswordLength;
    memcpy( pxNetworkParams->xPassword.xWPA.cPassphrase, pxNetworkProfile->cPassword, pxNetworkProfile->ucPasswordLength );
    pxNetworkParams->xSecurity = eWiFiSecurityWPA2;
    pxNetworkParams->ucChannel = 0;
}

static esp_err_t get_status_handler(wifi_prov_config_get_data_t *resp_data, wifi_prov_ctx_t **ctx)
{
    ESP_LOGI(TAG, "Requested wifi prov status");
    if (WIFI_IsConnected( NULL ) && xProvisionedParams.ucSSIDLength > 0 )
    {
        ESP_LOGI(TAG, "wifi prov status is connected");
        memset(resp_data, 0, sizeof(wifi_prov_config_get_data_t));
        strlcpy(resp_data->conn_info.ssid, (char *)xProvisionedParams.ucSSID, xProvisionedParams.ucSSIDLength);
        resp_data->wifi_state = WIFI_PROV_STA_CONNECTED;
    }

    return ESP_OK;
}

static esp_err_t set_config_handler(const wifi_prov_config_set_data_t *req_data, wifi_prov_ctx_t **ctx)
{
    ESP_LOGI(TAG, "WiFi Credentials Received : \n\tssid %s \n\tpassword %s",
             req_data->ssid, 
             req_data->password);

    /* Cache provisioned SSID/password */
    xProvisionedParams.ucSSIDLength = strnlen( req_data->ssid, wificonfigMAX_SSID_LEN );
    memcpy( &xProvisionedParams.ucSSID, req_data->ssid, xProvisionedParams.ucSSIDLength );
    xProvisionedParams.ucPasswordLength = strnlen( req_data->password, wificonfigMAX_PASSPHRASE_LEN );
    memcpy( &xProvisionedParams.cPassword, req_data->password, xProvisionedParams.ucPasswordLength );

    return ESP_OK;
}

static esp_err_t apply_config_handler(wifi_prov_ctx_t **ctx)
{
    ESP_LOGI(TAG, "Applying wifi creds");

    /* Try to connect using provisioned SSID/password */
    WIFINetworkParams_t xAttemptParams;
    prvNetworkProfile2Params(&xProvisionedParams, &xAttemptParams);
    if( eWiFiSuccess == WIFI_ConnectAP( &xAttemptParams ))
    {
        ESP_LOGI(TAG, "Successfully connected to %s", xAttemptParams.ucSSID);

        /* Save the credentials in flash, and for use by NetworkManager */
        uint16_t usIndex = 0;
        if( eWiFiSuccess == WIFI_NetworkAdd( &xProvisionedParams, &usIndex ))
        {
            ESP_LOGI(TAG, "WiFi Credentials[%d] Saved", usIndex);
        }
        else
        {
            ESP_LOGE(TAG, "Failed to save provisioned credentials to flash");
        }
    }

    return ESP_OK;
}

wifi_prov_config_handlers_t wifi_prov_handlers = {
    .get_status_handler   = get_status_handler,
    .set_config_handler   = set_config_handler,
    .apply_config_handler = apply_config_handler,
    .ctx = NULL
};

/* -------------------------------------------------------------------- */
/*                          PRIVATE HELPERS                             */
/* -------------------------------------------------------------------- */
static esp_err_t xStartProtocomHTTP(void)
{   
    /* Create new protocomm instance */
    pxProtocomm = protocomm_new();
    if (pxProtocomm == NULL) 
    {
        ESP_LOGE(TAG, "Failed to create new protocomm instance");
        return ESP_FAIL;
    }

    /* Start protocomm server on top of HTTP */
    protocomm_httpd_config_t pc_config = {
        .data = {
            .config = PROTOCOMM_HTTPD_DEFAULT_CONFIG()
        }
    };

    if (protocomm_httpd_start(pxProtocomm, &pc_config) != ESP_OK) {
        ESP_LOGE(TAG, "Failed to start protocomm HTTP server");
        return ESP_FAIL;
    }

    /* Set protocomm version verification endpoint for protocol */
    protocomm_set_version(pxProtocomm, "proto-ver", "V0.1");
    protocomm_set_security(pxProtocomm, "prov-session", &protocomm_security1, &pop);
    
    /* Add endpoint for provisioning to set wifi station config */
    if (ESP_OK != protocomm_add_endpoint( pxProtocomm, 
                                "prov-config",
                                wifi_prov_config_data_handler,
                                (void *) &wifi_prov_handlers))
    {
        ESP_LOGE(TAG, "Failed to set provisioning endpoint");
        protocomm_httpd_stop(pxProtocomm);
        return ESP_FAIL;
    }
    
    return ESP_OK;
}

static void app_prov_stop_service()
{
    protocomm_remove_endpoint(pxProtocomm, "prov-config");
    protocomm_unset_security(pxProtocomm, "prov-session");
    protocomm_unset_version(pxProtocomm, "proto-ver");
    protocomm_httpd_stop(pxProtocomm);
    protocomm_delete(pxProtocomm);
}

static void prvDeleteAllSavedNetworks()
{
    uint32_t ulCountSavedNetworks = IotWifiSoftAPProv_GetNumNetworks();
    for( uint32_t i = 0; i < ulCountSavedNetworks; i++)
    {
        WIFI_NetworkDelete(i);
    }
}

/* -------------------------------------------------------------------- */
/*                            PUBLIC (API)                              */
/* -------------------------------------------------------------------- */
uint32_t IotWifiSoftAPProv_Init( void )
{
    uint32_t ulReturnCode = pdFAIL;
    memset(&xProvisionedParams, 0u, sizeof(xProvisionedParams));
#ifdef DEBUG
    prvDeleteAllSavedNetworks();
#endif
    /* Configure AP  */
    WIFINetworkParams_t xAPConfig = {
        .ucChannel = wificonfigACCESS_POINT_CHANNEL,
        .xSecurity = wificonfigACCESS_POINT_SECURITY,
    };

    uint8_t eth_mac[6];
    esp_wifi_get_mac(WIFI_IF_STA, eth_mac);
    xAPConfig.ucSSIDLength = snprintf( (char *)xAPConfig.ucSSID, 
                                       sizeof(xAPConfig.ucSSID), 
                                       "%s%02X%02X%02X", 
                                       wificonfigACCESS_POINT_SSID_PREFIX,
                                       eth_mac[3], 
                                       eth_mac[4], 
                                       eth_mac[5] );
    xAPConfig.xPassword.xWPA.ucLength = strnlen( wificonfigACCESS_POINT_PASSKEY, wificonfigMAX_PASSPHRASE_LEN );
    memcpy(xAPConfig.xPassword.xWPA.cPassphrase, wificonfigACCESS_POINT_PASSKEY, xAPConfig.xPassword.xWPA.ucLength );
    
    /* Configure/Start AP, then start the protocommn HTTP URI handlers */
    if(eWiFiSuccess != WIFI_ConfigureAP(&xAPConfig))
    {
        ESP_LOGE(TAG, "Failed to configure AP");
    }
    else if(ESP_OK != esp_wifi_set_mode(WIFI_MODE_APSTA))
    {
        /* Currently WIFI_ConfigureAP configures as AP only. So we tweak to APSTA */
        ESP_LOGE(TAG, "Failed to configure APSTA");
    }
    else if(eWiFiSuccess != WIFI_StartAP())
    {
        ESP_LOGE(TAG, "Failed to start AP");
    }
    else if( ESP_OK != xStartProtocomHTTP())
    {
        ESP_LOGE(TAG, "Failed to start http server");
    }
    else
    {
        ESP_LOGI(TAG, "SoftAP Provisioning started with SSID '%s', Password '%s'", xAPConfig.ucSSID, xAPConfig.xPassword.xWPA.cPassphrase);
        ulReturnCode = pdPASS;
    }

    return ulReturnCode;
}

/* TODO: The common code should be refactored out */
uint32_t IotWifiSoftAPProv_GetNumNetworks( void )
{
    return IotBleWifiProv_GetNumNetworks();
}

uint32_t IotWifiSoftAPProv_Connect( uint32_t networkIndex )
{
    uint32_t ulReturnCode = pdFAIL;
    WIFINetworkProfile_t xSavedNetworkProfile;
    WIFINetworkParams_t xAttemptParams;

    /* Retrieve the ssid/password stored in indexed flash. Then attempt connection */
    if( eWiFiSuccess != WIFI_NetworkGet( &xSavedNetworkProfile, networkIndex) )
    {
        ESP_LOGE(TAG, "Failed to read saved network profile[%d] from flash", networkIndex );
        ulReturnCode = pdFAIL;
    }

    /* Try the connection */
    prvNetworkProfile2Params(&xProvisionedParams, &xAttemptParams);
    if( eWiFiSuccess != WIFI_ConnectAP( &xAttemptParams ))
    {
        ESP_LOGE(TAG, "Failed to connect to saved network profile[%d]", networkIndex );
        ulReturnCode = pdFAIL;
    }
    else
    {
        ESP_LOGI(TAG, "Successfully connected to saved network profile[%d]", networkIndex );
        ulReturnCode = pdPASS;
    }

    return ulReturnCode;
}

void IotWifiSoftAPProv_Deinit( void )
{
    app_prov_stop_service();
}


