/*
 * Amazon FreeRTOS
 * Copyright (C) 2019 Amazon.com, Inc. or its affiliates.  All Rights Reserved.
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

#include "FreeRTOSConfig.h"
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <limits.h>

#include "iot_wifi.h"
#include "aws_wifi_hal.h"
#include "aws_wifi_log.h"

#include "wifi_api.h"
#include "wifi_nvdm_config.h"
#include "wifi_lwip_helper.h"
#include "wifi_country_code.h"
#include "lwip/dns.h"

#define MTK_NVDM_ENABLE
#include "nvdm.h"

#define WIFI_MODE_NOT_SUPPORTED (0)

//
// Internal data structures
//

// For each project, wificonfigMAX_SCAN_RESULTS should be defined in aws_wifi_config.h
#ifndef wificonfigMAX_SCAN_RESULTS
#define wificonfigMAX_SCAN_RESULTS  (20)
#endif

static uint8_t scanList_[ sizeof(wifi_scan_list_item_t) * wificonfigMAX_SCAN_RESULTS ];
static wifi_sta_list_t staList_[ WIFI_MAX_NUMBER_OF_STA ];
static WIFIEventHandler_t eventHandlers_[ eWiFiEventMax ];

static enum {
    WIFI_DISCONNECTED_EVENT_FILTER_NONE,
    WIFI_DISCONNECTED_EVENT_FILTER_SOFTAP,
} wifi_disconnected_event_filter;


#define MACSTR_FMT "%02x:%02x:%02x:%02x:%02x:%02x"
#define MACSTR(x) x[0], x[1], x[2], x[3], x[4], x[5]


// Default world-safe country code
static const char* defaultCountryCode_ = "00";

//
// Internal helper functions
//
static inline WIFISecurity_t WIFI_HAL_Security( wifi_auth_mode_t authMode )
{
    switch (authMode)
    {
        case WIFI_AUTH_MODE_OPEN:
            return eWiFiSecurityOpen;
        case WIFI_AUTH_MODE_SHARED:
        case WIFI_AUTH_MODE_AUTO_WEP:
            return eWiFiSecurityWEP;
        case WIFI_AUTH_MODE_WPA:
        case WIFI_AUTH_MODE_WPA_PSK:
        case WIFI_AUTH_MODE_WPA_None:
            return eWiFiSecurityWPA;
        case WIFI_AUTH_MODE_WPA2:
        case WIFI_AUTH_MODE_WPA2_PSK:
        case WIFI_AUTH_MODE_WPA_WPA2:
        case WIFI_AUTH_MODE_WPA_PSK_WPA2_PSK:
            return eWiFiSecurityWPA2;
        default:
            return eWiFiSecurityNotSupported;
    }
}


static inline uint8_t WIFI_HAL_OpMode( WIFIDeviceMode_t xDeviceMode )
{
    switch (xDeviceMode)
    {
        case eWiFiModeStation:
            return WIFI_MODE_STA_ONLY;
        case eWiFiModeAP:
            return WIFI_MODE_AP_ONLY;
    }
    return WIFI_MODE_NOT_SUPPORTED;
}

static inline WIFIDeviceMode_t WIFI_HAL_DeviceMode( uint8_t opMode )
{
    switch (opMode)
    {
        case WIFI_MODE_STA_ONLY:
            return eWiFiModeStation;
        case WIFI_MODE_AP_ONLY:
            return eWiFiModeAP;
    }
    return eWiFiModeNotSupported;
}

static inline WIFIReason_t WIFI_HAL_ReasonCode( wifi_reason_code_t reason )
{
    switch (reason)
    {
        case WIFI_REASON_CODE_FIND_AP_FAIL:
            return eWiFiReasonAPNotFound;
        case WIFI_REASON_CODE_PREV_AUTH_INVALID:
            return eWiFiReasonAuthExpired;
        case WIFI_REASON_CODE_DEAUTH_LEAVING_BSS:
            return eWiFiReasonAuthLeaveBSS;
        case WIFI_REASON_CODE_DISASSOC_INACTIVITY:
            return eWiFiReasonAssocExpired;
        case WIFI_REASON_CODE_DISASSOC_AP_OVERLOAD:
            return eWiFiReasonAssocTooMany;
        case WIFI_REASON_CODE_CLASS_2_ERR:
        case WIFI_REASON_CODE_CLASS_3_ERR:
        case WIFI_REASON_CODE_DISASSOC_LEAVING_BSS: //?
        case WIFI_REASON_CODE_ASSOC_BEFORE_AUTH:
        case WIFI_REASON_CODE_DISASSOC_UNSPECIFIED_QOS:
        case WIFI_REASON_CODE_DISASSOC_LACK_OF_BANDWIDTH:
        case WIFI_REASON_CODE_DISASSOC_TX_OUTSIDE_TXOP_LIMIT:
            return eWiFiReasonAssocFailed;
        case WIFI_REASON_CODE_DISASSOC_PWR_CAP_UNACCEPTABLE:
            return eWiFiReasonAssocPowerCapBad;
        case WIFI_REASON_CODE_DISASSOC_SUP_CHS_UNACCEPTABLE:
            return eWiFiReasonAssocSupChanBad;
        case WIFI_REASON_CODE_INVALID_INFO_ELEM:
            return eWiFiReasonIEInvalid;
        case WIFI_REASON_CODE_4_WAY_HANDSHAKE_TIMEOUT:
            return eWiFiReason4WayTimeout;
        case WIFI_REASON_CODE_GROUP_KEY_UPDATE_TIMEOUT:
            return eWiFiReasonGroupKeyUpdateTimeout;
        case WIFI_REASON_CODE_DIFFERENT_INFO_ELEM:
            return eWiFiReason4WayIEDiffer;
        case WIFI_REASON_CODE_MULTICAST_CIPHER_NOT_VALID:
            return eWiFiReasonGroupCipherInvalid;
        case WIFI_REASON_CODE_UNICAST_CIPHER_NOT_VALID:
        case WIFI_REASON_CODE_PEER_CIPHER_UNSUPPORTED:
            return eWiFiReasonPairwiseCipherInvalid;
        case WIFI_REASON_CODE_AKMP_NOT_VALID:
            return eWiFiReasonAKMPInvalid;
        case WIFI_REASON_CODE_UNSUPPORTED_RSNE_VERSION:
            return eWiFiReasonRSNVersionInvalid;
        case WIFI_REASON_CODE_INVALID_RSNE_CAPABILITIES:
            return eWiFiReasonRSNCapInvalid;
        case WIFI_REASON_CODE_IEEE_802_1X_AUTH_FAILED:
            return eWiFiReason8021XAuthFailed;
        case WIFI_REASON_CODE_CIPHER_REJECT_SEC_POLICY:
            return eWiFiReasonCipherSuiteRejected;
        case WIFI_REASON_CODE_DISASSOC_ACK_LOST_POOR_CHANNEL:
            return eWiFiReasonLinkFailed;
        case WIFI_REASON_CODE_PASSWORD_ERROR:
            return eWiFiReasonAuthFailed;
    }
    return eWiFiReasonUnspecified;
}

static uint16_t WIFI_HAL_ConvertScanResults( void )
{
    wifi_scan_list_item_t* item = (wifi_scan_list_item_t*)scanList_;
    WIFIScanResult_t* result = (WIFIScanResult_t*)scanList_;
    WIFIScanResult_t temp;
    uint16_t count = 0;
    uint8_t i;

    for (i = 0; i < wificonfigMAX_SCAN_RESULTS; i++)
    {
        if (item->is_valid)
        {
            memcpy(temp.ucSSID, item->ssid, item->ssid_length);
            temp.ucSSIDLength = item->ssid_length;
            memcpy(temp.ucBSSID, item->bssid, wificonfigMAX_BSSID_LEN);
            temp.xSecurity = WIFI_HAL_Security(item->auth_mode);
            temp.cRSSI = item->rssi;
            temp.ucChannel = item->channel;

            memcpy(result++, &temp, sizeof(temp));
            count++;
        }
        item++;
    }
    return count;
}

//
// Internal event handlers
//
static int32_t WIFI_HAL_EventHandler( wifi_event_t wifi_event,
                                      uint8_t* data,
                                      uint32_t len )
{
    AWS_WIFI_LOGD("%s: event = %d\n", __FUNCTION__, wifi_event);
    WIFIEvent_t event;
    uint16_t reason;

    switch (wifi_event)
    {
        case WIFI_EVENT_IOT_INIT_COMPLETE:
            AWS_WIFI_LOGD("SDK event init complete\n");
            event.xEventType = eWiFiEventReady;
            break;

        case WIFI_EVENT_IOT_SCAN_COMPLETE:
            AWS_WIFI_LOGD("SDK event scan done\n");
            WIFI_HAL_StopScan();
            event.xEventType = eWiFiEventScanDone;
            event.xInfo.xScanDone.pxScanResults = (WIFIScanResult_t*)scanList_;
            event.xInfo.xScanDone.usNumScanResults = WIFI_HAL_ConvertScanResults();
            break;

        case WIFI_EVENT_IOT_PORT_SECURE:
            if (data[6] == WIFI_PORT_STA)
            {
                AWS_WIFI_LOGD("SDK event STA connected to " MACSTR_FMT "\n", MACSTR(data));
                memset(&event, 0, sizeof(event));
                event.xEventType = eWiFiEventConnected;
                memcpy(event.xInfo.xConnected.xConnectionInfo.ucBSSID, data, wificonfigMAX_BSSID_LEN);
            }
            else
            {
                AWS_WIFI_LOGD("SDK event AP got station " MACSTR_FMT "\n", MACSTR(data));
                event.xEventType = eWiFiEventAPStationConnected;
                memcpy(event.xInfo.xAPStationConnected.ucMac, data, wificonfigMAX_BSSID_LEN);
            }
            break;

        case WIFI_EVENT_IOT_DISCONNECTED:
            if (data[6] == WIFI_PORT_STA)
            {
                AWS_WIFI_LOGD("SDK event STA disconnected from " MACSTR_FMT "\n", MACSTR(data));

                // Per MTK: the SDK will generate 3 disconnected events for 1 disconnect_ap command,
                // 1 will have the correct MAC address and 2 will have all 0 MAC address, so we filter them here
                if(data[0] == 0 && data[1] == 0 && data[2] == 0 && data[3] == 0 && data[4] == 0 && data[5] == 0) {
                    return 0;
                }

                // Filter disconnected events with special local meaning
                if (wifi_disconnected_event_filter == WIFI_DISCONNECTED_EVENT_FILTER_SOFTAP)
                {
                    // Reset the filter to let WIFI_HAL_StartAP() continue
                    wifi_disconnected_event_filter = WIFI_DISCONNECTED_EVENT_FILTER_NONE;
                }
            }
            else
            {
                AWS_WIFI_LOGD("SDK event AP lost station" MACSTR_FMT "\n", MACSTR(data));
                event.xEventType = eWiFiEventAPStationDisconnected;
                memcpy(event.xInfo.xAPStationDisconnected.ucMac, data, wificonfigMAX_BSSID_LEN);
            }
            // Why doesn't the SDK provide a reason for disconnect?
            event.xInfo.xAPStationDisconnected.xReason = eWiFiReasonUnspecified;
            break;

        case WIFI_EVENT_IOT_CONNECTION_FAILED:
            reason = (data[2]<<8) + data[1];

            if (data[0] == WIFI_PORT_STA)
            {
                AWS_WIFI_LOGE("SDK event STA failed to connect, reason = %d\n", reason);
            }
            else
            {
                AWS_WIFI_LOGE("SDK event AP failed to connect, reason = %d\n", reason);
            }

            event.xEventType = eWiFiEventConnectionFailed;
            event.xInfo.xConnectionFailed.xReason = WIFI_HAL_ReasonCode(reason);
            break;

        default:
            AWS_WIFI_LOGE("SDK event %u not handled\n", wifi_event);
            return 0;
    }

    if (eventHandlers_[event.xEventType])
    {
        eventHandlers_[event.xEventType](&event);
    }
    else
    {
        AWS_WIFI_LOGE("SDK event %u not registered\n", wifi_event);
    }
    return 0;
}

static void WIFI_HAL_NetEventHandler(const wifi_lwip_helper_ip_addr_t * ip_addr)
{
    WIFIEvent_t event;
    WIFIIPAddress_t* ipAddr = &event.xInfo.xIPReady.xIPAddress;

    event.xEventType = eWiFiEventIPReady;
    switch (ip_addr->type)
    {
        case LWIP_HELPER_IPADDR_TYPE_V4:
            ipAddr->xType = eWiFiIPAddressTypeV4;
            memcpy(ipAddr->ulAddress, ip_addr->ip_addr, sizeof(uint32_t));
            break;
#if 0
        case LWIP_HELPER_IPADDR_TYPE_V6:
            ipAddr->xType = eWiFiIPAddressTypeV6;
            memcpy(ipAddr->ulAddress, ip_addr->ip_addr, 4 * sizeof(uint32_t));
            break;
#endif
        default:
            AWS_WIFI_LOGE("SDK IP address type %u not handled\n", ip_addr->type);
            return;
    }
    if (eventHandlers_[eWiFiEventIPReady])
    {
        eventHandlers_[eWiFiEventIPReady](&event);
    }
}

//
// AWS WIFI HAL API functions
//
WIFIReturnCode_t WIFI_HAL_Init()
{
    AWS_WIFI_LOGD("%s\n", __FUNCTION__);
    int32_t retVal;

    /* The WiFi initialization procedure is as follows:
     * (1) Read WiFi parameters from NVDM (wifi_config)
     * (2) Modify parameters as necessary (config, config_ext)
     * (3) Call wifi_init(), which applies the modifications
     *     and starts driver (inband) and minisupp threads
     * (4) Initialize the LwIP stack
     * (5) Register handlers for WiFi events
     */

    /* (1) Read wifi_config from NVDM */
    wifi_cfg_t wifi_config = {0};
    retVal = wifi_config_init(&wifi_config);
    if (retVal < 0)
    {
        AWS_WIFI_LOGE("SDK config init failed");
        return eWiFiFailure;
    }

    wifi_config_t config = {0};
    wifi_config_ext_t config_ext = {0};

    /* (2) Modify parameters as desired */
    config.opmode = WIFI_MODE_STA_ONLY;
    config_ext.sta_auto_connect_present = 1;
    config_ext.sta_auto_connect = 0;

    // Original MTK sample code, keep them for reference
#if 0
    memcpy(config.sta_config.ssid, wifi_config.sta_ssid, 32);
    config.sta_config.ssid_length = wifi_config.sta_ssid_len;
    config.sta_config.bssid_present = 0;
    memcpy(config.sta_config.password, wifi_config.sta_wpa_psk, 64);
    config.sta_config.password_length = wifi_config.sta_wpa_psk_len;
    config_ext.sta_wep_key_index_present = 1;
    config_ext.sta_wep_key_index = wifi_config.sta_default_key_id;

    if (wifi_config.opmode == WIFI_MODE_REPEATER)
        config_ext.sta_auto_connect = 0;
    else
        config_ext.sta_auto_connect = 1;

    memcpy(config.ap_config.ssid, wifi_config.ap_ssid, 32);
    config.ap_config.ssid_length = wifi_config.ap_ssid_len;
    memcpy(config.ap_config.password, wifi_config.ap_wpa_psk, 64);
    config.ap_config.password_length = wifi_config.ap_wpa_psk_len;
    config.ap_config.auth_mode = (wifi_auth_mode_t)wifi_config.ap_auth_mode;
    config.ap_config.encrypt_type = (wifi_encrypt_type_t)wifi_config.ap_encryp_type;
    config.ap_config.channel = wifi_config.ap_channel;
    config.ap_config.bandwidth = wifi_config.ap_bw;
    config.ap_config.bandwidth_ext = WIFI_BANDWIDTH_EXT_40MHZ_UP;
    config_ext.ap_wep_key_index_present = 1;
    config_ext.ap_wep_key_index = wifi_config.ap_default_key_id;
    config_ext.ap_hidden_ssid_enable_present = 1;
    config_ext.ap_hidden_ssid_enable = wifi_config.ap_hide_ssid;
#endif

    /* (3) Start the WiFi init procedure. Note: init will wait for system task
     * scheduler to start because it needs to run some OS tasks. Once finished
     * the WIFI_EVENT_IOT_INIT_COMPLETE event will be triggered */
    wifi_init(&config, &config_ext);

    /* (4) Initialize tcpip, net interface, and dhcp client / server
     * NOTE: This can also be done dynamically upon WiFi connection.
     * Here we take advantage of tools defined in wifi_lwip_helper.c */
    lwip_network_init(config.opmode);
    lwip_net_start(config.opmode);

    /* (5) Reset event handlers. Upper layer should register them immediately */
    memset(eventHandlers_, 0, sizeof(eventHandlers_));

    return eWiFiSuccess;
}

WIFIReturnCode_t WIFI_HAL_DeInit( void )
{
    AWS_WIFI_LOGD("%s\n", __FUNCTION__);
    int32_t retVal;

    retVal = wifi_connection_scan_deinit();
    if (retVal < 0)
    {
        AWS_WIFI_LOGE("SDK scan deinit failed\n");
        return eWiFiFailure;
    }
    return eWiFiSuccess;
}

WIFIReturnCode_t WIFI_HAL_SetRadio( uint8_t on )
{
    AWS_WIFI_LOGD("%s\n", __FUNCTION__);
    int32_t retVal;

    retVal = wifi_config_set_radio(on);
    if (retVal < 0)
    {
        AWS_WIFI_LOGE("SDK set radio %d failed\n", on);
        return eWiFiFailure;
    }
    return eWiFiSuccess;
}

WIFIReturnCode_t WIFI_HAL_SetMode( WIFIDeviceMode_t xDeviceMode )
{
    AWS_WIFI_LOGD("%s\n", __FUNCTION__);
    uint8_t opMode;
    int32_t retVal;

    opMode = WIFI_HAL_OpMode(xDeviceMode);
    if (opMode == WIFI_MODE_NOT_SUPPORTED)
    {
        AWS_WIFI_LOGI("SDK set opmode invalid\n");
        return eWiFiNotSupported;
    }

    // This function is in wifi_lwip_helper.c
    retVal = wifi_set_opmode(opMode);
    if (retVal < 0)
    {
        AWS_WIFI_LOGE("SDK set opmode failed\n");
        return eWiFiFailure;
    }
    return eWiFiSuccess;
}

WIFIReturnCode_t WIFI_HAL_GetMode( WIFIDeviceMode_t * xDeviceMode )
{
    AWS_WIFI_LOGD("%s\n", __FUNCTION__);
    int32_t retVal;
    uint8_t opMode;

    retVal = wifi_config_get_opmode(&opMode);
    if (retVal < 0)
    {
        AWS_WIFI_LOGE("SDK get opmode failed\n");
        return eWiFiFailure;
    }
    *xDeviceMode = WIFI_HAL_DeviceMode(opMode);
    return eWiFiSuccess;
}

WIFIReturnCode_t WIFI_HAL_GetMacAddress( uint8_t * pucMac )
{
    AWS_WIFI_LOGD("%s\n", __FUNCTION__);
    int32_t retVal;

    retVal = wifi_config_get_mac_address(WIFI_PORT_STA, pucMac);
    if (retVal < 0)
    {
        AWS_WIFI_LOGE("SDK get mac address failed\n");
        return eWiFiFailure;
    }
    return eWiFiSuccess;
}

WIFIReturnCode_t WIFI_HAL_SetMacAddress( uint8_t * pucMac )
{
    AWS_WIFI_LOGD("%s\n", __FUNCTION__);
    uint8_t macAddress[18];
    nvdm_status_t retVal;

    // Check current MAC address, skip if identical to avoid unnecessary nvdm write
    retVal = wifi_config_get_mac_address(WIFI_PORT_STA, macAddress);
    if (retVal < 0)
    {
        AWS_WIFI_LOGE("SDK get mac address failed\n");
        return eWiFiFailure;
    }
    if (!memcmp(pucMac, macAddress, wificonfigMAX_BSSID_LEN))
    {
        AWS_WIFI_LOGD("SDK set mac address unchanged\n");
        return eWiFiSuccess;
    }

    // Per MTK: it is not possible to change MAC addresses dynamically. This
    // function save the MAC address to nvdm so it will be used after reboot

    // First save STA MAC address
    retVal = snprintf((char*)macAddress, sizeof(macAddress), MACSTR_FMT, MACSTR(pucMac));
    if (retVal != (int)(sizeof(macAddress) - 1))
    {
        AWS_WIFI_LOGE("SDK format STA mac address failed\n");
        return eWiFiFailure;

    }
    retVal = nvdm_write_data_item("STA", "MacAddr", NVDM_DATA_ITEM_TYPE_STRING,
                                  macAddress, retVal);
    if (retVal != NVDM_STATUS_OK)
    {
        AWS_WIFI_LOGE("SDK write STA mac address failed\n");
        return eWiFiFailure;
    }

    // Next save AP MAC address which sets the local bit
    retVal = snprintf((char*)macAddress, sizeof(macAddress), MACSTR_FMT, 0x2|MACSTR(pucMac));
    if (retVal != (int)(sizeof(macAddress) - 1))
    {
        AWS_WIFI_LOGE("SDK format AP mac address failed\n");
        return eWiFiFailure;

    }
    retVal = nvdm_write_data_item("AP", "MacAddr", NVDM_DATA_ITEM_TYPE_STRING,
                                  macAddress, retVal);
    if (retVal != NVDM_STATUS_OK)
    {
        AWS_WIFI_LOGE("SDK write AP mac address failed\n");
        return eWiFiFailure;
    }
    return eWiFiSuccess;
}

WIFIReturnCode_t WIFI_HAL_GetIPInfo( WIFIIPConfiguration_t * pxIPInfo )
{
    AWS_WIFI_LOGD("%s\n", __FUNCTION__);
    struct netif* staIF;

    // This function is in etherhetif.c
    staIF = netif_find_by_type(NETIF_TYPE_STA);
    if (staIF == NULL)
    {
        AWS_WIFI_LOGE("SDK netif_find_by_type failed\n");
        return eWiFiFailure;
    }

    memset(pxIPInfo, 0, sizeof(WIFIIPConfiguration_t));

    // Only IPv4 is supported
    pxIPInfo->xIPAddress.xType = eWiFiIPAddressTypeV4;
    memcpy(&(pxIPInfo->xIPAddress.ulAddress), &staIF->ip_addr, 4);

    pxIPInfo->xNetMask.xType = eWiFiIPAddressTypeV4;
    memcpy(&(pxIPInfo->xGateway.ulAddress), &staIF->gw, 4);

    pxIPInfo->xNetMask.xType = eWiFiIPAddressTypeV4;
    memcpy(&(pxIPInfo->xNetMask.ulAddress), &staIF->netmask, 4);

    const ip_addr_t* dnsAddr = NULL;
    dnsAddr = dns_getserver(0);
    if (dnsAddr != NULL) {
        pxIPInfo->xDns1.xType = eWiFiIPAddressTypeV4;
        memcpy(&(pxIPInfo->xDns1.ulAddress), dnsAddr, 4);
    }
    else
    {
        pxIPInfo->xDns1.xType = eWiFiIPAddressTypeNotSupported;
    }

    dnsAddr = dns_getserver(1);
    if (dnsAddr != NULL) {
        pxIPInfo->xDns2.xType = eWiFiIPAddressTypeV4;
        memcpy(&(pxIPInfo->xDns2.ulAddress), dnsAddr, 4);
    }
    else
    {
        pxIPInfo->xDns2.xType = eWiFiIPAddressTypeNotSupported;
    }

    return eWiFiSuccess;
}

WIFIReturnCode_t WIFI_HAL_StartScan( WIFIScanConfig_t * pxScanConfig )
{
    AWS_WIFI_LOGD("%s\n", __FUNCTION__);
    uint8_t* ssid;
    uint8_t ssidLength;
    uint8_t opMode;
    uint8_t scanMode;
    int32_t retVal;

    if (pxScanConfig && pxScanConfig->ucSSIDLength > 0)
    {
        ssid = (uint8_t*)pxScanConfig->ucSSID;
        ssidLength = pxScanConfig->ucSSIDLength;

        // Single channel scan is not implemented for MT7697
        // Per MTK: to do single channel scanning you can use
        // a temporary country code with that channel
    }
    else
    {
        ssid = NULL;
        ssidLength = 0;
    }

    // Full scan in STA mode, partial scan in AP mode to prevent STA loss
    retVal = wifi_config_get_opmode(&opMode);
    if (retVal < 0)
    {
        AWS_WIFI_LOGE("SDK get opmode failed\n");
        return eWiFiFailure;
    }
    scanMode = (opMode == WIFI_MODE_STA_ONLY) ? 0 : 1;
    memset(scanList_, 0, sizeof(scanList_));

    retVal = wifi_connection_scan_init((wifi_scan_list_item_t*)scanList_, wificonfigMAX_SCAN_RESULTS);
    if (retVal < 0)
    {
        AWS_WIFI_LOGE("SDK scan init failed\n");
        return eWiFiFailure;
    }

    // Scan option = 0: active in non-DFS channels, passive in DFS channels
    retVal = wifi_connection_start_scan(ssid, ssidLength, NULL, scanMode, 0);
    if (retVal < 0)
    {
        AWS_WIFI_LOGE("SDK start scan failed\n");
        return eWiFiFailure;
    }
    return eWiFiSuccess;
}

WIFIReturnCode_t WIFI_HAL_StopScan( void )
{
    AWS_WIFI_LOGD("%s\n", __FUNCTION__);
    int32_t retVal;

    retVal = wifi_connection_stop_scan();
    if (retVal < 0)
    {
        AWS_WIFI_LOGE("SDK stop scan failed\n");
        return eWiFiFailure;
    }

    retVal = wifi_connection_scan_deinit();
    if (retVal < 0) {
        AWS_WIFI_LOGE("SDK scan deinit failed");
        return eWiFiFailure;
    }

    return eWiFiSuccess;
}

WIFIReturnCode_t WIFI_HAL_StartDisconnectAP( void )
{
    AWS_WIFI_LOGD("%s\n", __FUNCTION__);
    int32_t retVal;

    retVal = wifi_connection_disconnect_ap();
    if (retVal < 0)
    {
        AWS_WIFI_LOGE("SDK disconnect AP failed\n");
        return eWiFiFailure;
    }
    return eWiFiSuccess;
}

WIFIReturnCode_t WIFI_HAL_GetConnectionInfo( WIFIConnectionInfo_t * pxConnectionInfo )
{
    AWS_WIFI_LOGD("%s\n", __FUNCTION__);
    uint8_t status;
    int32_t retVal;

    retVal = wifi_connection_get_link_status(&status);
    if (retVal < 0)
    {
        AWS_WIFI_LOGE("SDK get link status failed\n");
        return eWiFiFailure;
    }
    if (status == 0)
    {
        AWS_WIFI_LOGE("SDK not connected to AP\n");
        return eWiFiFailure;
    }
    else
    {
        wifi_auth_mode_t authMode;
        wifi_encrypt_type_t encType;

        retVal = wifi_config_get_ssid(WIFI_PORT_STA,
                                      pxConnectionInfo->ucSSID,
                                      &pxConnectionInfo->ucSSIDLength);
        if (retVal < 0)
        {
            AWS_WIFI_LOGE("SDK get ssid failed\n");
            return eWiFiFailure;
        }

        retVal = wifi_config_get_bssid(pxConnectionInfo->ucBSSID);
        if (retVal < 0)
        {
            AWS_WIFI_LOGE("SDK get bssid failed\n");
            return eWiFiFailure;
        }

        retVal = wifi_config_get_channel(WIFI_PORT_STA,
                                         &pxConnectionInfo->ucChannel);
        if (retVal < 0)
        {
            AWS_WIFI_LOGE("SDK get channel failed\n");
            return eWiFiFailure;
        }

        retVal = wifi_config_get_security_mode(WIFI_PORT_STA, &authMode, &encType);
        if (retVal < 0)
        {
            AWS_WIFI_LOGE("SDK get security mode failed\n");
            return eWiFiFailure;
        }
        pxConnectionInfo->xSecurity = WIFI_HAL_Security(authMode);
    }
    return eWiFiSuccess;
}

WIFIReturnCode_t WIFI_HAL_GetRSSI( int8_t * pcRSSI )
{
    AWS_WIFI_LOGD("%s\n", __FUNCTION__);
    int32_t retVal;

    retVal = wifi_connection_get_rssi(pcRSSI);
    if (retVal < 0)
    {
        AWS_WIFI_LOGE("SDK get rssi failed\n");
        return eWiFiFailure;
    }
    return eWiFiSuccess;
}

WIFIReturnCode_t WIFI_HAL_StartAP( const WIFINetworkParams_t * pxNetworkParams )
{
    AWS_WIFI_LOGD("%s\n", __FUNCTION__);
    WIFIEvent_t event;
    int32_t retVal;

    if(WIFI_IsConnected(NULL) == pdTRUE) {
        // Per MTK: Need to disable STA mode scanning before changing to SoftAP mode
        wifi_disconnected_event_filter = WIFI_DISCONNECTED_EVENT_FILTER_SOFTAP;
        retVal = wifi_connection_disconnect_ap();
        if (retVal < 0)
        {
            AWS_WIFI_LOGE("SDK disconnect AP failed\n");
            return eWiFiFailure;
        }
        // Wait for the indication that disconnect has completed
        while (wifi_disconnected_event_filter == WIFI_DISCONNECTED_EVENT_FILTER_SOFTAP)
        {
            vTaskDelay(1);
        }
    }

    // This function is in wifi_lwip_helper.h, not wifi_api.h
    retVal = wifi_set_opmode(WIFI_MODE_AP_ONLY);
    if (retVal < 0)
    {
        AWS_WIFI_LOGE("SDK set opmode (AP) failed\n");
        return eWiFiFailure;
    }

    retVal = wifi_config_set_ssid(WIFI_PORT_AP,
                                  (uint8_t*)pxNetworkParams->ucSSID,    // cast away const
                                  pxNetworkParams->ucSSIDLength);
    if (retVal < 0)
    {
        AWS_WIFI_LOGE("SDK set ssid failed\n");
        return eWiFiFailure;
    }

    // Per MTK: MT7697 SDK can only set BSSID in wifi_init(), not dynamically

    switch (pxNetworkParams->xSecurity)
    {
        case eWiFiSecurityOpen:
            retVal = wifi_config_set_security_mode(WIFI_PORT_AP,
                         WIFI_AUTH_MODE_OPEN,
                         WIFI_ENCRYPT_TYPE_WEP_DISABLED);
            if (retVal < 0)
            {
                AWS_WIFI_LOGE("SDK set security mode (open) failed\n");
                return eWiFiFailure;
            }
            break;
        case eWiFiSecurityWPA:
        case eWiFiSecurityWPA2:
            retVal = wifi_config_set_security_mode(WIFI_PORT_AP,
                        WIFI_AUTH_MODE_WPA2_PSK,
                        WIFI_ENCRYPT_TYPE_AES_ENABLED);
            if (retVal < 0)
            {
                AWS_WIFI_LOGE("SDK set security mode (psk) failed\n");
                return eWiFiFailure;
            }
            retVal = wifi_config_set_wpa_psk_key(WIFI_PORT_AP,
                        (uint8_t*)pxNetworkParams->xPassword.xWPA.cPassphrase,
                        pxNetworkParams->xPassword.xWPA.ucLength);
            if (retVal < 0)
            {
                AWS_WIFI_LOGE("SDK set wpa psk key failed\n");
                return eWiFiFailure;
            }
            break;
        default:
            // No WEP support for SoftAP mode
            return eWiFiNotSupported;
    }

    retVal = wifi_config_set_channel(WIFI_PORT_AP, pxNetworkParams->ucChannel);
    if (retVal < 0)
    {
        AWS_WIFI_LOGE("SDK set channel failed\n");
        return eWiFiFailure;
    }

    retVal = wifi_config_reload_setting();
    if (retVal < 0)
    {
        AWS_WIFI_LOGE("SDK set reload settings (AP) failed\n");
        return eWiFiFailure;
    }

    // Per MTK: once reload_setting() is done, SoftAP is done and there is
    // no event from the SDK. So we post the event to upper layer here
    if (eventHandlers_[eWiFiEventAPStateChanged])
    {
        event.xInfo.xAPStateChanged.ucState = 1;
        eventHandlers_[eWiFiEventAPStateChanged](&event);
    }
    return eWiFiSuccess;
}

WIFIReturnCode_t WIFI_HAL_StopAP( void )
{
    AWS_WIFI_LOGD("%s\n", __FUNCTION__);
    WIFIEvent_t event;
    int32_t retVal;

    // This function is in wifi_lwip_helper, not wifi_api.h
    retVal = wifi_set_opmode(WIFI_MODE_STA_ONLY);
    if (retVal < 0)
    {
        AWS_WIFI_LOGE("SDK set opmode (STA) failed\n");
        return eWiFiFailure;
    }

    // Per MTK: once reload_setting() is done, SoftAP is done and there is
    // no event from the SDK. So we post the event to upper layer here
    if (eventHandlers_[eWiFiEventAPStateChanged])
    {
        event.xInfo.xAPStateChanged.ucState = 0;
        eventHandlers_[eWiFiEventAPStateChanged](&event);
    }
    return eWiFiSuccess;
}

WIFIReturnCode_t WIFI_HAL_GetStationList( WIFIStationInfo_t * pxStationList,
                                          uint8_t * puStationListSize )
{
    AWS_WIFI_LOGD("%s\n", __FUNCTION__);
    uint8_t count = WIFI_MAX_NUMBER_OF_STA;
    int32_t retVal;
    int32_t i;

    retVal = wifi_connection_get_sta_list(&count, staList_);
    if (retVal < 0)
    {
        AWS_WIFI_LOGE("SDK get sta list failed\n");
        return eWiFiFailure;
    }
    if (*puStationListSize > count) *puStationListSize = count;

    for (i = 0; i < *puStationListSize; i++)
    {
        memcpy(pxStationList[i].ucMAC, staList_[i].mac_address, wificonfigMAX_BSSID_LEN);
    }
    return eWiFiSuccess;
}

WIFIReturnCode_t WIFI_HAL_DisconnectStation( uint8_t * pucMac )
{
    AWS_WIFI_LOGD("%s\n", __FUNCTION__);
    int32_t retVal;

    retVal = wifi_connection_disconnect_sta(pucMac);
    if (retVal < 0)
    {
        AWS_WIFI_LOGE("SDK disconnection sta failed\n");
        return eWiFiFailure;
    }
    return eWiFiSuccess;
}

WIFIReturnCode_t WIFI_HAL_RegisterEvent( WIFIEventType_t xEventType,
                                         WIFIEventHandler_t xHandler )
{
    AWS_WIFI_LOGD("%s %u\n", __FUNCTION__, xEventType);
    wifi_event_t wifi_event;
    int32_t retVal;

    switch (xEventType)
    {
        case eWiFiEventReady:
            wifi_event = WIFI_EVENT_IOT_INIT_COMPLETE;
            break;

        case eWiFiEventScanDone:
            wifi_event = WIFI_EVENT_IOT_SCAN_COMPLETE;
            break;

        case eWiFiEventConnected:
            wifi_event = WIFI_EVENT_IOT_PORT_SECURE;
            break;

        case eWiFiEventDisconnected:
            wifi_event = WIFI_EVENT_IOT_DISCONNECTED;
            break;

        case eWiFiEventConnectionFailed:
            wifi_event = WIFI_EVENT_IOT_CONNECTION_FAILED;
            break;

        case eWiFiEventIPReady:
            // This function is in wifi_lwip_helper.c
            retVal = wifi_lwip_helper_register_ip_ready_callback(WIFI_HAL_NetEventHandler);
            if (retVal < 0)
            {
                AWS_WIFI_LOGE("SDK register event %u failed\n", xEventType);
                return eWiFiFailure;
            }
            eventHandlers_[xEventType] = xHandler;
            return eWiFiSuccess;

        case eWiFiEventAPStateChanged:
            // SDK does not have event for AP state
            eventHandlers_[xEventType] = xHandler;
            return eWiFiSuccess;

        case eWiFiEventAPStationConnected:
            // SDK reuses the WIFI_EVENT_IOT_PORT_SECURE for this
            eventHandlers_[xEventType] = xHandler;
            return eWiFiSuccess;

        case eWiFiEventAPStationDisconnected:
            // SDK reuses the WIFI_EVENT_IOT_DISCONNECTED for this
            eventHandlers_[xEventType] = xHandler;
            return eWiFiSuccess;

        default:
            AWS_WIFI_LOGE("SDK register event %d invalid\n", xEventType);
            return eWiFiNotSupported;
    }

    retVal = wifi_connection_register_event_handler(wifi_event, WIFI_HAL_EventHandler);
    if (retVal < 0)
    {
        AWS_WIFI_LOGE("SDK register event %u failed\n", wifi_event);
        return eWiFiFailure;
    }
    else
    {
        eventHandlers_[xEventType] = xHandler;
        return eWiFiSuccess;
    }
}

WIFIReturnCode_t WIFI_HAL_SetCountryCode( const char * pcCountryCode )
{
    AWS_WIFI_LOGD("%s: %s\n", __FUNCTION__, pcCountryCode);
    const wifi_country_code_ext_t* cc;
    const wifi_channel_table_t* ct;
    uint8_t buf[16];
    int32_t retVal;

    // MTK SDK uses "WS" for world-safe country code
    if (!strncmp(pcCountryCode, defaultCountryCode_, sizeof(defaultCountryCode_)))
    {
        pcCountryCode = "WS";
    }

    // This function is in wifi_country_code.c
    cc = WIFI_HAL_FindCountryCode(pcCountryCode);
    if (cc == NULL)
    {
        AWS_WIFI_LOGE("SDK country code %s invalid\n", pcCountryCode);
        return eWiFiFailure;
    }

    // This sets the channel list and max tx power for the regulatory domain
    // The actual tx power is set in the NVDM entries SingleSKU2G_n
    retVal = wifi_config_set_country_code((wifi_country_code_t*)cc);
    if (retVal < 0)
    {
        AWS_WIFI_LOGE("SDK set country code %s failed\n", pcCountryCode);
        return eWiFiFailure;
    }

    // Set SingleSKU_index for the regulatory domain. We don't check the existence
    // of the corresponding SingleSKU2G_n entry, but just assuming they are all
    // set properly at the factory
    retVal = nvdm_write_data_item("common", "SingleSKU_index", NVDM_DATA_ITEM_TYPE_STRING,
                                  &(cc->reg_domain), sizeof("0"));
    if (retVal != NVDM_STATUS_OK)
    {
        AWS_WIFI_LOGE("SDK write SingleSKU_index failed\n");
        return eWiFiFailure;
    }

    // Set BGChannelTable so the channel table will be used after reboot
    ct = &cc->channel_table[0];     // 0 is always for BG channels
    retVal = snprintf((char*)buf, sizeof(buf), "%u,%u,0|", ct->first_channel, ct->num_of_channel);
    if (retVal < 0 || retVal > (int)(sizeof(buf) - 1))
    {
        AWS_WIFI_LOGE("SDK format BGChannelTable failed\n");
        return eWiFiFailure;
    }
    retVal = nvdm_write_data_item("common", "BGChannelTable", NVDM_DATA_ITEM_TYPE_STRING,
                                  buf, retVal);
    if (retVal != NVDM_STATUS_OK)
    {
        AWS_WIFI_LOGE("SDK write BGChannelTable failed\n");
        return eWiFiFailure;
    }

    // TODO: For AChannelTable we need to loop over the remaining channel tables
    // and create a concatenated string "first,num,dfs|first,num,dfs|" etc.
    // See wifi_nvdm_config.c for examples

    // For testing only. Remove later when this feature is stable
#if 0
    uint32_t len;

    memset(buf, 0, sizeof(buf));
    WIFI_HAL_GetCountryCode((char*)buf);
    AWS_WIFI_LOGD("SDK test country code %s\n", (char*)buf);

    memset(buf, 0, sizeof(buf));
    len = sizeof(buf);
    retVal = nvdm_read_data_item("common", "SingleSKU_index", buf, &len);
    if (retVal != NVDM_STATUS_OK) {
        AWS_WIFI_LOGE("SDK test read SingleSKU_index failed\n");
        return eWiFiFailure;
    }
    AWS_WIFI_LOGD("SingleSKU_index %u\n", (uint8_t)buf[0]);

    memset(buf, 0, sizeof(buf));
    len = sizeof(buf);
    retVal = nvdm_read_data_item("common", "BGChannelTable", buf, &len);
    if (retVal != NVDM_STATUS_OK) {
        AWS_WIFI_LOGE("SDK test read BGChannelTable failed\n");
        return eWiFiFailure;
    }
    AWS_WIFI_LOGD("BGChannelTable %s\n", buf);
#endif
    return eWiFiSuccess;
}

WIFIReturnCode_t WIFI_HAL_GetCountryCode( char * pcCountryCode )
{
    AWS_WIFI_LOGD("%s\n", __FUNCTION__);

    // Buffer must hold the largest wifi_country_code_t (channel table size = 5)
    uint8_t buffer[ sizeof(wifi_country_code_t) + sizeof(wifi_channel_table_t) * 5];
    wifi_country_code_t* cc = (wifi_country_code_t*)buffer;
    int32_t retVal;

    retVal = wifi_config_get_country_code(cc);
    if (retVal < 0)
    {
        AWS_WIFI_LOGE("SDK get country code failed\n");
        return eWiFiFailure;
    }

    // Convert internal world-safe code "WS" to default country code
    if (!strncmp((char*)cc->country_code, "WS", sizeof("WS")))
    {
        strncpy(pcCountryCode, defaultCountryCode_, sizeof(defaultCountryCode_));
    }
    else
    {
        strncpy(pcCountryCode, (char*)cc->country_code, 4);
    }
    return eWiFiSuccess;
}

WIFIReturnCode_t WIFI_HAL_GetStatistic( WIFIStatisticInfo_t * pxStats )
{
    AWS_WIFI_LOGD("%s\n", __FUNCTION__);
    uint8_t opMode;

    memset(pxStats, 0, sizeof(WIFIStatisticInfo_t));

    int32_t retVal = wifi_config_get_opmode(&opMode);
    if (WIFI_MODE_STA_ONLY != opMode) {
        AWS_WIFI_LOGE("WIFI GetStatistic is only supported in STA mode\n");
        return eWiFiFailure;
    }

    wifi_statistic_t statistic = {0};
    retVal = wifi_config_get_statistic(&statistic);
    if (retVal < 0) {
        AWS_WIFI_LOGE("SDK get statistics failed\n");
        return eWiFiFailure;
    }

    pxStats->ulTxSuccessCount = statistic.Tx_Success_Count;
    pxStats->ulTxRetryCount = statistic.Tx_Retry_Count;
    pxStats->ulTxFailCount = statistic.Tx_Fail_Count;
    pxStats->ulRxSuccessCount = statistic.Rx_Success_Count;
    pxStats->ulRxCRCErrorCount = statistic.Rx_with_CRC;
    pxStats->ulMICErrorCount = statistic.MIC_Error_Count;
    pxStats->cNoise = statistic.Rssi - statistic.SNR;
    pxStats->usPhyRate = statistic.MAX_PHY_Rate;
    pxStats->usTxRate = statistic.REAL_TX_PHY_Rate;
    pxStats->usRxRate = statistic.REAL_RX_PHY_Rate;
    pxStats->cRssi = statistic.Rssi;
    pxStats->ucBandwidth = statistic.BBPCurrentBW;
    pxStats->ucIdleTimePer = 0;   // Not supported by SDK

    return eWiFiSuccess;
}

WIFIReturnCode_t WIFI_HAL_GetCapability( WIFICapabilityInfo_t * pxCaps)
{
    AWS_WIFI_LOGD("%s\n", __FUNCTION__);

    memset(pxCaps, 0, sizeof(WIFICapabilityInfo_t));

    pxCaps->xBand = eWiFiBand2G;
    pxCaps->xPhyMode = eWiFiPhy11n;
    pxCaps->xBandwidth = eWiFiBW20;
    pxCaps->ulMaxAggr = 0;
    // Example for testing:
    //pxCaps->usSupportedFeatures = WIFI_WPS_SUPPORTED | WIFI_P2P_SUPPORTED | WIFI_TDLS_SUPPORTED;
    pxCaps->usSupportedFeatures = 0;  //unknown

   return eWiFiSuccess;
}

WIFIReturnCode_t WIFI_HAL_SetPMMode( WIFIPMMode_t xPMModeType )
{
    AWS_WIFI_LOGD("%s\n", __FUNCTION__);
    return eWiFiNotSupported;
}

WIFIReturnCode_t WIFI_HAL_GetPMMode( WIFIPMMode_t * pxPMModeType )
{
    AWS_WIFI_LOGD("%s\n", __FUNCTION__);
    return eWiFiNotSupported;
}

