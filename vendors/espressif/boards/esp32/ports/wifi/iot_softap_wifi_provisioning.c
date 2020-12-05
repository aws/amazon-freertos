#include "FreeRTOS.h"
#include "task.h"

#include "iot_wifi.h"
#include "iot_softap_wifi_provisioning.h"
#include "aws_wifi_config.h"
#include "stdbool.h"

#include <string.h>
#include <esp_system.h>
#include <esp_wifi.h>

#define LOG_LOCAL_LEVEL ESP_LOG_INFO

#include <esp_log.h>
#include <nvs_flash.h>
#include <lwip/err.h>
#include <lwip/sys.h>
#include <protocomm.h>
#include <protocomm_httpd.h>
#include <protocomm_security0.h>
#include <protocomm_security1.h>
#include <wifi_provisioning/wifi_config.h>

#ifndef IOT_SOFTAP_WIFI_PROVISIONING_MAX_SAVED_NETWORKS
    #define IOT_SOFTAP_WIFI_PROVISIONING_MAX_SAVED_NETWORKS 8
#endif

#ifndef CONFIG_POP
    #define CONFIG_POP "abcd1234"
#endif

static const char *TAG = "WIFI_PROVISION";

static protocomm_t *pxProtocomm = NULL;

static WIFINetworkProfile_t xProvisionedParams = {
    .ucBSSID = { 0 },
    .ucSSID = { 0 },
    .ucSSIDLength = 0,
    .xSecurity = eWiFiSecurityWPA2,
    .cPassword = { 0 },
    .ucPasswordLength = 0
};

static protocomm_security_pop_t xPop = { 
    .data = ( uint8_t * ) CONFIG_POP,
    .len = ( sizeof( CONFIG_POP )- 1 )
};  

static esp_err_t prvGetProvisionStatus(wifi_prov_config_get_data_t *pxResponse, wifi_prov_ctx_t **ppxUserContext);
static esp_err_t prvSetProvisionConfig(const wifi_prov_config_set_data_t *pxRequest, wifi_prov_ctx_t **ppxUserContext);
static esp_err_t prvApplyProvisionConfig(wifi_prov_ctx_t **ppxUserContext);

wifi_prov_config_handlers_t xWifiProvisionHandlers = {
    .get_status_handler   = prvGetProvisionStatus,
    .set_config_handler   = prvSetProvisionConfig,
    .apply_config_handler = prvApplyProvisionConfig,
    .ctx = NULL
};

/* -------------------------------------------------------------------- */
/*                          PRIVATE HELPERS                             */
/* -------------------------------------------------------------------- */
static esp_err_t prvStartProtocomHTTP(void)
{   
    esp_err_t xReturnCode = ESP_OK;

    /* Create new protocomm instance */
    pxProtocomm = protocomm_new();
    if (pxProtocomm == NULL) 
    {
        ESP_LOGE(TAG, "Failed to create new protocomm instance");
        xReturnCode = ESP_FAIL;
    }

    /* Start protocomm server on top of HTTP */
    if ( xReturnCode == ESP_OK ) 
    {
        protocomm_httpd_config_t xProtocommServiceConfig = {
            .data = {
                .config = PROTOCOMM_HTTPD_DEFAULT_CONFIG()
            }
        };

        if( protocomm_httpd_start(pxProtocomm, &xProtocommServiceConfig) != ESP_OK ) {
            ESP_LOGE(TAG, "Failed to start protocomm HTTP server");
            xReturnCode = ESP_FAIL;
        }
    }

    /* Setup and start the URI handling service that uses protobufs to communicate 
     * with provisioning app */
    if ( xReturnCode == ESP_OK )
    {
        protocomm_set_version(pxProtocomm, "proto-ver", "V0.1");
        protocomm_set_security(pxProtocomm, "prov-session", &protocomm_security1, &xPop);        
        if (ESP_OK != protocomm_add_endpoint( pxProtocomm, 
                                    "prov-config",
                                    wifi_prov_config_data_handler,
                                    (void *) &xWifiProvisionHandlers))
        {
            ESP_LOGE(TAG, "Failed to set provisioning endpoint");
            protocomm_httpd_stop(pxProtocomm);
            xReturnCode =  ESP_FAIL;
        }
    }
    
    return xReturnCode;
}

static void prvDeleteAllSavedNetworks()
{
    uint32_t ulCountSavedNetworks = IotWifiSoftAPProv_GetNumNetworks();
    for( uint32_t i = 0; i < ulCountSavedNetworks; i++)
    {
        WIFI_NetworkDelete(i);
    }
}

static inline void prvNetworkProfile2Params( WIFINetworkProfile_t const * const pxNetworkProfile, WIFINetworkParams_t * pxNetworkParams)
{
    memset(pxNetworkParams, 0u, sizeof(WIFINetworkParams_t));
    pxNetworkParams->ucSSIDLength = pxNetworkProfile->ucSSIDLength;
    memcpy( pxNetworkParams->ucSSID, pxNetworkProfile->ucSSID, pxNetworkProfile->ucSSIDLength );
    pxNetworkParams->xPassword.xWPA.ucLength = pxNetworkProfile->ucPasswordLength;
    memcpy( pxNetworkParams->xPassword.xWPA.cPassphrase, pxNetworkProfile->cPassword, pxNetworkProfile->ucPasswordLength );
    pxNetworkParams->xSecurity = eWiFiSecurityWPA2;
    pxNetworkParams->ucChannel = 0;
}

/* -------------------------------------------------------------------- */
/*                       PROVISIONING HANDLERS                          */
/* -------------------------------------------------------------------- */
static esp_err_t prvGetProvisionStatus(wifi_prov_config_get_data_t *pxResponse, wifi_prov_ctx_t **ppxUserContext)
{
    memset(pxResponse, 0, sizeof(wifi_prov_config_get_data_t));
    if( WIFI_IsConnected( NULL ) )
    {
        pxResponse->wifi_state = WIFI_PROV_STA_CONNECTED;
        strlcpy(pxResponse->conn_info.ssid, (char *)xProvisionedParams.ucSSID, xProvisionedParams.ucSSIDLength);
    }

    return ESP_OK;
}

static esp_err_t prvSetProvisionConfig(const wifi_prov_config_set_data_t *pxRequest, wifi_prov_ctx_t **ppxUserContext)
{
    ESP_LOGI(TAG, "WiFi Credentials Received : \n\tssid %s \n\tpassword %s", pxRequest->ssid, pxRequest->password);

    /* Cache provisioned SSID/password */
    xProvisionedParams.ucSSIDLength = strnlen( pxRequest->ssid, wificonfigMAX_SSID_LEN );
    memcpy( &xProvisionedParams.ucSSID, pxRequest->ssid, xProvisionedParams.ucSSIDLength );
    xProvisionedParams.ucPasswordLength = strnlen( pxRequest->password, wificonfigMAX_PASSPHRASE_LEN );
    memcpy( &xProvisionedParams.cPassword, pxRequest->password, xProvisionedParams.ucPasswordLength );

    return ESP_OK;
}

static esp_err_t prvApplyProvisionConfig(wifi_prov_ctx_t **ppxUserContext)
{
    esp_err_t xReturnCode = ESP_FAIL;

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
            xReturnCode = ESP_OK;
            ESP_LOGI(TAG, "WiFi Credentials[%d] Saved", usIndex);
        }
        else
        {
            ESP_LOGE(TAG, "Failed to save provisioned credentials to flash");
        }
    }
    else
    {
        ESP_LOGE(TAG, "Failed to connect to '%s'", (char *)xAttemptParams.ucSSID );
    }

    return xReturnCode;
}

/* -------------------------------------------------------------------- */
/*                            PUBLIC (API)                              */
/* -------------------------------------------------------------------- */
uint32_t IotWifiSoftAPProv_Init( void )
{
    uint32_t ulReturnCode = pdFAIL;

    /* Reinit any cached credentials */
    memset(&xProvisionedParams, 0u, sizeof(xProvisionedParams));
    xProvisionedParams.xSecurity = eWiFiSecurityWPA2;
    
    /* Configure AP  */
    WIFINetworkParams_t xAPConfig = {
        .ucChannel = wificonfigACCESS_POINT_CHANNEL,
        .xSecurity = wificonfigACCESS_POINT_SECURITY,
    };

    char pcMAC[ wificonfigMAX_BSSID_LEN ] = { 0 };
    WIFI_GetMAC(&pcMAC);
    xAPConfig.ucSSIDLength = snprintf( (char *)xAPConfig.ucSSID, 
                                       sizeof(xAPConfig.ucSSID), 
                                       "%s%02X%02X%02X", 
                                       wificonfigACCESS_POINT_SSID_PREFIX,
                                       pcMAC[3], 
                                       pcMAC[4], 
                                       pcMAC[5] );
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
        ESP_LOGE(TAG, "Failed to switch to APSTA");
    }
    else if(eWiFiSuccess != WIFI_StartAP())
    {
        ESP_LOGE(TAG, "Failed to start AP");
    }
    else if( ESP_OK != prvStartProtocomHTTP())
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

uint32_t IotWifiSoftAPProv_GetNumNetworks( void )
{
    WIFINetworkProfile_t xProfile;
    uint32_t i = 0;

    for( i = 0; i < IOT_SOFTAP_WIFI_PROVISIONING_MAX_SAVED_NETWORKS; i++ )
    {
        if ( eWiFiSuccess != WIFI_NetworkGet( &xProfile, i ))
        {
            break;
        }
    }

    return i;
}

uint32_t IotWifiSoftAPProv_Connect( uint32_t ulNetworkIndex )
{
    uint32_t ulReturnCode = pdFAIL;
    WIFINetworkProfile_t xSavedNetworkProfile;
    WIFINetworkParams_t xAttemptParams;

    /* Retrieve the ssid/password stored in indexed flash. Then attempt connection */
    if( eWiFiSuccess == WIFI_NetworkGet( &xSavedNetworkProfile, ulNetworkIndex) )
    {
        prvNetworkProfile2Params(&xSavedNetworkProfile, &xAttemptParams);
        if( eWiFiSuccess == WIFI_ConnectAP( &xAttemptParams ))
        {
            ulReturnCode = pdPASS;
            ESP_LOGI(TAG, "Successfully connected to saved network profile[%d]", ulNetworkIndex );
        }
        else
        {
            ESP_LOGE(TAG, "Failed to connect to saved network profile[%d]", ulNetworkIndex );
        }
    }
    else
    {
        ESP_LOGE(TAG, "Failed to read saved network profile[%d] from flash", ulNetworkIndex );
    }
    return ulReturnCode;
}

void IotWifiSoftAPProv_Deinit( void )
{
    protocomm_remove_endpoint(pxProtocomm, "prov-config");
    protocomm_unset_security(pxProtocomm, "prov-session");
    protocomm_unset_version(pxProtocomm, "proto-ver");
    protocomm_httpd_stop(pxProtocomm);
    protocomm_delete(pxProtocomm);
}


