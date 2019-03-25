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
 * @file aws_wifi.c
 * @brief Wi-Fi Interface.
 */

/* Socket and Wi-Fi interface includes. */
#include "FreeRTOS.h"
#include "aws_wifi.h"

/* Wi-Fi configuration includes. */
#include "aws_wifi_config.h"

#include <string.h>
#include "wm_config.h"
#include "wm_type_def.h"
#include "wm_efuse.h"
#include "wm_wifi.h"
#include "wm_osal.h"
#include "wm_netif2.0.3.h"
#include "wm_mem.h"
#include "wm_flash_map.h"
#include "wm_internal_flash.h"

/*-----------------------------------------------------------*/

struct aws_wifi_conf {
    u8 used;
    u8 pad;
    WIFINetworkProfile_t conf;
};

#define WIFI_NETWORK_CONF_BASE      (USER_ADDR_START + ((2048 + 4) * 3))/* pkcs11 */
#define WIFI_NETWORK_CONF_NUM_MAX   ((USER_AREA_LEN - ((2048 + 4) * 3)) / sizeof(struct aws_wifi_conf))

static u8 aws_wifi_inited = 0;
static tls_os_sem_t *aws_wifi_status_sem = NULL;
static tls_os_sem_t *aws_wifi_scan_sem = NULL;
static tls_os_sem_t *aws_wifi_action_sem = NULL;

static WIFIPMMode_t glastPMModeType = eWiFiPMNormal;

static void aws_wifi_net_status(u8 status)
{
    struct netif *netif = tls_get_netif();

	switch(status)
	{
	    case NETIF_WIFI_JOIN_FAILED:
	        tls_os_sem_release(aws_wifi_status_sem);
	        configPRINTF(("sta join net failed\r\n"));
			break;
		case NETIF_WIFI_DISCONNECTED:
		    //tls_os_sem_release(aws_wifi_status_sem);
	        configPRINTF(("sta net disconnected\r\n"));
			break;
		case NETIF_IP_NET_UP:
		    tls_os_sem_release(aws_wifi_status_sem);
            configPRINTF(("sta ip\n"));
			break;
	    case NETIF_WIFI_SOFTAP_FAILED:
            configPRINTF(("softap create failed\r\n"));
	        break;
        case NETIF_WIFI_SOFTAP_CLOSED:
            configPRINTF(("softap closed\r\n"));
	        break;
        case NETIF_IP_NET2_UP:
            configPRINTF(("softap ip\r\n"));
	        break;
		default:
			break;
	}
}

static void aws_wifi_scan_finish(void)
{
    if (aws_wifi_scan_sem)
        tls_os_sem_release(aws_wifi_scan_sem);
}

WIFIReturnCode_t WIFI_On( void )
{
    int ret;

    // Check if WiFi is already initialized
    if (aws_wifi_inited) {
        return eWiFiSuccess;
    }

    ret = tls_os_sem_create(&aws_wifi_status_sem, 0);
    if (TLS_OS_SUCCESS != ret)
        return eWiFiFailure;

    ret = tls_os_sem_create(&aws_wifi_action_sem, 1);
    if (TLS_OS_SUCCESS != ret)
    {
        tls_os_sem_delete(aws_wifi_status_sem);
        aws_wifi_status_sem = NULL;
        return eWiFiFailure;
    }

    tls_netif_add_status_event(aws_wifi_net_status);
    aws_wifi_inited = 1;
    return eWiFiSuccess;
}
/*-----------------------------------------------------------*/

WIFIReturnCode_t WIFI_Off( void )
{
    if (!aws_wifi_inited)
    {
        if (eWiFiSuccess != WIFI_On())
            return eWiFiFailure;
    }

    tls_wifi_disconnect();
    tls_wifi_softap_destroy();
    return eWiFiSuccess;
}
/*-----------------------------------------------------------*/

WIFIReturnCode_t WIFI_ConnectAP( const WIFINetworkParams_t * const pxNetworkParams )
{
    int ret;
    u8 ssidlen;
    u8 pwdlen;
    struct tls_ethif *ethif;
    WIFIReturnCode_t err = eWiFiFailure;

    if (!aws_wifi_inited)
    {
        if (eWiFiSuccess != WIFI_On())
            return eWiFiFailure;
    }

    if (!pxNetworkParams)
        return eWiFiFailure;

    ret = tls_os_sem_acquire(aws_wifi_action_sem, 0);
    if (TLS_OS_SUCCESS != ret)
        return eWiFiFailure;

    if (WM_WIFI_DISCONNECTED != tls_wifi_get_state())
        tls_wifi_disconnect();

    ssidlen = pxNetworkParams->ucSSIDLength;
    pwdlen  = pxNetworkParams->ucPasswordLength;

    if ((ssidlen > 0) && (ssidlen < 33) && pxNetworkParams->pcSSID)
    {
        if (pxNetworkParams->pcSSID[ssidlen - 1] == '\0')
            ssidlen--;
    }
    else
    {
        goto out;
    }

    if ((pwdlen > 0) && (pwdlen < 65) && pxNetworkParams->pcPassword)
    {
        if (pxNetworkParams->pcPassword[pwdlen - 1] == '\0')
            pwdlen--;
    }
    else
    {
        goto out;
    }

    ret = tls_wifi_connect((u8 *)pxNetworkParams->pcSSID, ssidlen,
                           (u8 *)pxNetworkParams->pcPassword, pwdlen);
    if (WM_SUCCESS != ret)
        goto out;

    ret = tls_os_sem_acquire(aws_wifi_status_sem, 0);
    if (TLS_OS_SUCCESS != ret)
        goto out;

    ethif = tls_netif_get_ethif();
    if (ethif->status)
        err = eWiFiSuccess;
out:
        tls_os_sem_release(aws_wifi_action_sem);
        return err;
}
/*-----------------------------------------------------------*/

WIFIReturnCode_t WIFI_Disconnect( void )
{
    if (!aws_wifi_inited)
    {
        if (eWiFiSuccess != WIFI_On())
            return eWiFiFailure;
    }

    int ret = tls_os_sem_acquire(aws_wifi_action_sem, 0);
    tls_wifi_disconnect();
    if (TLS_OS_SUCCESS == ret)
        tls_os_sem_release(aws_wifi_action_sem);
    return eWiFiSuccess;
}
/*-----------------------------------------------------------*/

WIFIReturnCode_t WIFI_Reset( void )
{
    //printf("!!! %s !!!\r\n", __func__);
    return eWiFiNotSupported;
}
/*-----------------------------------------------------------*/

WIFIReturnCode_t WIFI_Scan( WIFIScanResult_t * pxBuffer,
                            uint8_t ucNumNetworks )
{
    int ret;
    int i;
    char *buf = NULL;
    struct tls_scan_bss_t *wsr;
	struct tls_bss_info_t *bss_info;

	if (!pxBuffer || !ucNumNetworks)
        return eWiFiFailure;

    if (aws_wifi_scan_sem)
        return eWiFiFailure;

    ret = tls_os_sem_create(&aws_wifi_scan_sem, 0);
    if (TLS_OS_SUCCESS != ret)
        return eWiFiFailure;

    tls_wifi_scan_result_cb_register(aws_wifi_scan_finish);
	while (WM_SUCCESS != tls_wifi_scan())
	{
	    //configPRINTF(("wscan failed, retrying...\r\n"));
        tls_os_time_delay(HZ / 10);
    }

    ret = tls_os_sem_acquire(aws_wifi_scan_sem, 0);
    if (TLS_OS_SUCCESS != ret)
    {
        tls_wifi_scan_result_cb_register(NULL);
        tls_os_sem_delete(aws_wifi_scan_sem);
	    aws_wifi_scan_sem = NULL;
        return eWiFiFailure;
    }

    ret = (sizeof(struct tls_scan_bss_t) - sizeof(struct tls_bss_info_t)) + 
            ucNumNetworks * sizeof(struct tls_bss_info_t);
    buf = tls_mem_alloc(ret);
    if (!buf)
    {
        tls_wifi_scan_result_cb_register(NULL);
        tls_os_sem_delete(aws_wifi_scan_sem);
	    aws_wifi_scan_sem = NULL;
        return eWiFiFailure;
    }

    ret = tls_wifi_get_scan_rslt((u8 *)buf, ret);
    if (WM_SUCCESS != ret)
    {
        tls_mem_free(buf);
        tls_wifi_scan_result_cb_register(NULL);
        tls_os_sem_delete(aws_wifi_scan_sem);
	    aws_wifi_scan_sem = NULL;
        return eWiFiFailure;
    }

    wsr = (struct tls_scan_bss_t *)buf;
    bss_info = wsr->bss;
    for (i = 0; i < wsr->count; i++)
    {
        pxBuffer->cChannel = bss_info->channel;
        pxBuffer->cRSSI = -(char)(0x100 - bss_info->rssi);
        memcpy(pxBuffer->cSSID, bss_info->ssid, bss_info->ssid_len);
        pxBuffer->cSSID[bss_info->ssid_len] = '\0';
        memcpy(pxBuffer->ucBSSID, bss_info->bssid, 6);
        pxBuffer->ucHidden = (bss_info->ssid_len == 0) ? 1 : 0;

        if (WM_WIFI_AUTH_MODE_OPEN == bss_info->privacy)
            pxBuffer->xSecurity = eWiFiSecurityOpen;
        else if (WM_WIFI_AUTH_MODE_WEP_AUTO & bss_info->privacy)
            pxBuffer->xSecurity = eWiFiSecurityWEP;
        else if (WM_WIFI_AUTH_MODE_WPA_PSK_AUTO & bss_info->privacy)
            pxBuffer->xSecurity = eWiFiSecurityWPA;
        else if (WM_WIFI_AUTH_MODE_WPA2_PSK_AUTO & bss_info->privacy)
            pxBuffer->xSecurity = eWiFiSecurityWPA2;
        else
            pxBuffer->xSecurity = eWiFiSecurityNotSupported;

        pxBuffer++;
        bss_info++;
    }

    tls_mem_free(buf);
	tls_wifi_scan_result_cb_register(NULL);
	tls_os_sem_delete(aws_wifi_scan_sem);
	aws_wifi_scan_sem = NULL;
    return eWiFiSuccess;
}
/*-----------------------------------------------------------*/

WIFIReturnCode_t WIFI_SetMode( WIFIDeviceMode_t xDeviceMode )
{
    u8 wmode = IEEE80211_MODE_INFRA;

    if (xDeviceMode > eWiFiModeP2P)
        return eWiFiNotSupported;

    if (xDeviceMode == eWiFiModeAP)
        wmode = IEEE80211_MODE_AP;
    else if (xDeviceMode == eWiFiModeP2P)
        wmode = IEEE80211_MODE_IBSS;
    else
        wmode = IEEE80211_MODE_INFRA;

    tls_param_set(TLS_PARAM_ID_WPROTOCOL, (void *)&wmode, TRUE);
    return eWiFiSuccess;
}
/*-----------------------------------------------------------*/

WIFIReturnCode_t WIFI_GetMode( WIFIDeviceMode_t * pxDeviceMode )
{
    u8 wmode = IEEE80211_MODE_INFRA;

    if (!pxDeviceMode)
        return eWiFiFailure;
    
    tls_param_get(TLS_PARAM_ID_WPROTOCOL, (void *)&wmode, TRUE);
    if (IEEE80211_MODE_AP == wmode)
        *pxDeviceMode = eWiFiModeAP;
    else if (IEEE80211_MODE_IBSS == wmode)
        *pxDeviceMode = eWiFiModeP2P;
    else
        *pxDeviceMode = eWiFiModeStation;
    return eWiFiSuccess;
}
/*-----------------------------------------------------------*/

WIFIReturnCode_t WIFI_NetworkAdd( const WIFINetworkProfile_t * const pxNetworkProfile,
                                  uint16_t * pusIndex )
{
    int ret;
    int i;
    int index = -1;
    struct aws_wifi_conf *wificonf;

    if (!pusIndex || !pxNetworkProfile)
        return eWiFiFailure;

    wificonf = tls_mem_alloc(sizeof(struct aws_wifi_conf));
    if (!wificonf)
        return eWiFiFailure;

    for (i = 0; i < WIFI_NETWORK_CONF_NUM_MAX; i++)
    {
        ret = tls_fls_read(WIFI_NETWORK_CONF_BASE + i * (sizeof(struct aws_wifi_conf)), 
                           (u8 *)wificonf, sizeof(struct aws_wifi_conf));
        if (ret != TLS_FLS_STATUS_OK)
            break;

        if (wificonf->used != 1)
        {
            index = i;
            break;
        }
    }

    if (-1 == index)
    {
        tls_mem_free(wificonf);
        return eWiFiFailure;
    }

    wificonf->used = 1;
    memcpy(&wificonf->conf, pxNetworkProfile, sizeof(WIFINetworkProfile_t));
    ret = tls_fls_write(WIFI_NETWORK_CONF_BASE + index * (sizeof(struct aws_wifi_conf)), 
                        (u8 *)wificonf, sizeof(struct aws_wifi_conf));
    tls_mem_free(wificonf);

    if (TLS_FLS_STATUS_OK == ret)
    {
        *pusIndex = index;
        return eWiFiSuccess;
    }
    else
    {
        return eWiFiFailure;
    }
}
/*-----------------------------------------------------------*/

WIFIReturnCode_t WIFI_NetworkGet( WIFINetworkProfile_t * pxNetworkProfile,
                                  uint16_t usIndex )
{
    int ret;
    struct aws_wifi_conf *wificonf;

    if (usIndex >= WIFI_NETWORK_CONF_NUM_MAX)
        return eWiFiFailure;

    if (!pxNetworkProfile)
        return eWiFiFailure;

    wificonf = tls_mem_alloc(sizeof(struct aws_wifi_conf));
    if (!wificonf)
        return eWiFiFailure;

    memset(wificonf, 0, sizeof(struct aws_wifi_conf));
    ret = tls_fls_read(WIFI_NETWORK_CONF_BASE + usIndex * (sizeof(struct aws_wifi_conf)), 
                           (u8 *)wificonf, sizeof(struct aws_wifi_conf));
    
    if (TLS_FLS_STATUS_OK == ret)
    {
        if (1 == wificonf->used)
        {
            memcpy(pxNetworkProfile, &wificonf->conf, sizeof(WIFINetworkProfile_t));
            tls_mem_free(wificonf);
            return eWiFiSuccess;
        }
        else
        {
            tls_mem_free(wificonf);
            return eWiFiFailure;
        }
    }
    else
    {
        tls_mem_free(wificonf);
        return eWiFiFailure;
    }
}
/*-----------------------------------------------------------*/

WIFIReturnCode_t WIFI_NetworkDelete( uint16_t usIndex )
{
    int ret;
    struct aws_wifi_conf *wificonf;

    if (usIndex >= WIFI_NETWORK_CONF_NUM_MAX)
        return eWiFiFailure;

    wificonf = tls_mem_alloc(sizeof(struct aws_wifi_conf));
    if (!wificonf)
        return eWiFiFailure;

    memset(wificonf, 0, sizeof(struct aws_wifi_conf));
    ret = tls_fls_read(WIFI_NETWORK_CONF_BASE + usIndex * (sizeof(struct aws_wifi_conf)), 
                           (u8 *)wificonf, sizeof(struct aws_wifi_conf));
    
    if (TLS_FLS_STATUS_OK == ret)
    {
        if (1 != wificonf->used)
        {
            tls_mem_free(wificonf);
            return eWiFiSuccess;
        }

        wificonf->used = 0;
        ret = tls_fls_write(WIFI_NETWORK_CONF_BASE + usIndex * (sizeof(struct aws_wifi_conf)), 
                           (u8 *)wificonf, sizeof(struct aws_wifi_conf));
        tls_mem_free(wificonf);
        return (TLS_FLS_STATUS_OK == ret) ? eWiFiSuccess : eWiFiFailure;
    }
    else
    {
        tls_mem_free(wificonf);
        return eWiFiFailure;
    }
}
/*-----------------------------------------------------------*/

WIFIReturnCode_t WIFI_Ping( uint8_t * pucIPAddr,
                            uint16_t usCount,
                            uint32_t ulIntervalMS )
{
    /* FIX ME. */
    //printf("!!! %s !!!\r\n", __func__);
    return eWiFiNotSupported;
}
/*-----------------------------------------------------------*/

WIFIReturnCode_t WIFI_GetIP( uint8_t * pucIPAddr )
{
    struct tls_ethif *ethif;

    if (!pucIPAddr)
        return eWiFiFailure;
    
	ethif = tls_netif_get_ethif();
	if (ethif->status)
        memcpy(pucIPAddr, &ethif->ip_addr.addr, 4);
    else
        memset(pucIPAddr, 0, 4);
    return eWiFiSuccess;
}
/*-----------------------------------------------------------*/

WIFIReturnCode_t WIFI_GetMAC( uint8_t * pucMac )
{
    if (!pucMac)
        return eWiFiFailure;

    return (tls_get_mac_addr(pucMac) == TLS_EFUSE_STATUS_OK) ? eWiFiSuccess : eWiFiFailure;
}
/*-----------------------------------------------------------*/

WIFIReturnCode_t WIFI_GetHostIP( char * pcHost,
                                 uint8_t * pucIPAddr )
{
    if (!pcHost || !pucIPAddr)
        return eWiFiFailure;

    struct hostent* host = lwip_gethostbyname(pcHost);
    if (host)
    {
        memcpy(pucIPAddr, host->h_addr, 4);
        return eWiFiSuccess;
    }
    else
    {
        return eWiFiFailure;
    }
}
/*-----------------------------------------------------------*/

WIFIReturnCode_t WIFI_StartAP( void )
{
    int ret;
    struct tls_param_ssid *ssid;
    struct tls_softap_info_t *apinfo;
    struct tls_ip_info_t *ipinfo;
    struct tls_param_ip *ip_param;
    struct tls_param_key *key;

    if (!aws_wifi_inited)
    {
        if (eWiFiSuccess != WIFI_On())
            return eWiFiFailure;
    }

    apinfo = tls_mem_alloc(sizeof(struct tls_softap_info_t));
    if (apinfo == NULL)
    {
        return eWiFiFailure;
    }
    ipinfo = tls_mem_alloc(sizeof(struct tls_ip_info_t));
    if (ipinfo == NULL)
    {
        tls_mem_free(apinfo);
        return eWiFiFailure;
    }

    ssid = tls_mem_alloc(sizeof(struct tls_param_ssid));
    if (!ssid)
    {
        tls_mem_free(apinfo);
        tls_mem_free(ipinfo);
        return eWiFiFailure;
    }
    tls_param_get(TLS_PARAM_ID_SOFTAP_SSID, (void*) ssid, TRUE);
    memcpy(apinfo->ssid, ssid->ssid, ssid->ssid_len);
    apinfo->ssid[ssid->ssid_len] = '\0';
    tls_mem_free(ssid);

    tls_param_get(TLS_PARAM_ID_SOFTAP_ENCRY, (void*) &apinfo->encrypt, TRUE);
    tls_param_get(TLS_PARAM_ID_SOFTAP_CHANNEL, (void*) &apinfo->channel, TRUE);

    key = tls_mem_alloc(sizeof(struct tls_param_key));
    if (!key)
    {
        tls_mem_free(apinfo);
        tls_mem_free(ipinfo);
        return eWiFiFailure;
    }
    tls_param_get(TLS_PARAM_ID_SOFTAP_KEY, (void*) key, TRUE);
    apinfo->keyinfo.key_len = key->key_length;
    apinfo->keyinfo.format = key->key_format;
    apinfo->keyinfo.index = key->key_index;
    memcpy(apinfo->keyinfo.key, key->psk, key->key_length);
    tls_mem_free(key);

    ip_param = tls_mem_alloc(sizeof(struct tls_param_ip));
    if (!ip_param)
    {
        tls_mem_free(apinfo);
        tls_mem_free(ipinfo);
        return eWiFiFailure;
    }
    tls_param_get(TLS_PARAM_ID_SOFTAP_IP, ip_param, TRUE);
    /*ip配置信息:ip地址，掩码，dns名称*/
    memcpy(ipinfo->ip_addr, ip_param->ip, 4);
    memcpy(ipinfo->netmask, ip_param->netmask, 4);
    tls_mem_free(ip_param);

    tls_param_get(TLS_PARAM_ID_DNSNAME, ipinfo->dnsname, FALSE);

    ret = tls_wifi_softap_create(apinfo, ipinfo);
    tls_mem_free(apinfo);
    tls_mem_free(ipinfo);

    return (WM_SUCCESS == ret) ? eWiFiSuccess : eWiFiFailure;
}
/*-----------------------------------------------------------*/

WIFIReturnCode_t WIFI_StopAP( void )
{
    if (!aws_wifi_inited)
    {
        if (eWiFiSuccess != WIFI_On())
            return eWiFiFailure;
    }

    tls_wifi_softap_destroy();
    return eWiFiSuccess;
}
/*-----------------------------------------------------------*/

WIFIReturnCode_t WIFI_ConfigureAP( const WIFINetworkParams_t * const pxNetworkParams )
{
    u8 encrypt;
    struct tls_param_ssid *ssid;
    struct tls_param_key *key;
    struct tls_param_ip *ip_param;
    u8 *dnsname;

    if (!aws_wifi_inited)
    {
        if (eWiFiSuccess != WIFI_On())
            return eWiFiFailure;
    }

    if (!pxNetworkParams)
        return eWiFiFailure;

    if (eWiFiSecurityWEP == pxNetworkParams->xSecurity)
        return eWiFiNotSupported;

    ssid = tls_mem_alloc(sizeof(struct tls_param_ssid));
    if (!ssid)
        return eWiFiFailure;

    memcpy(ssid->ssid, pxNetworkParams->pcSSID, pxNetworkParams->ucSSIDLength);
    ssid->ssid_len = pxNetworkParams->ucSSIDLength;
    tls_param_set(TLS_PARAM_ID_SOFTAP_SSID, (void*)ssid, TRUE);
    tls_mem_free(ssid);

    if (eWiFiSecurityWPA == pxNetworkParams->xSecurity)
        encrypt = IEEE80211_ENCRYT_CCMP_WPA;
    else if (eWiFiSecurityWPA2 == pxNetworkParams->xSecurity)
        encrypt = IEEE80211_ENCRYT_CCMP_WPA2;
    else
        encrypt = IEEE80211_ENCRYT_NONE;

    tls_param_set(TLS_PARAM_ID_SOFTAP_ENCRY, (void*) &encrypt, TRUE);

    tls_param_set(TLS_PARAM_ID_SOFTAP_CHANNEL, (void*) &pxNetworkParams->cChannel, TRUE);

    key = tls_mem_alloc(sizeof(struct tls_param_key));
    if (!key)
        return eWiFiFailure;

    key->key_format = 1;/* assic */
    key->key_index = 1;
    key->key_length = pxNetworkParams->ucPasswordLength;
    memcpy(key->psk, pxNetworkParams->pcPassword, pxNetworkParams->ucPasswordLength);
    tls_param_set(TLS_PARAM_ID_SOFTAP_KEY, (void*)key, TRUE);
    tls_mem_free(key);

    ip_param = tls_mem_alloc(sizeof(struct tls_param_ip));
    if (!ip_param)
        return eWiFiFailure;

    ip_param->dhcp_enable = 1;
    ip_param->ip[0] = 192;
	ip_param->ip[1] = 168;
	ip_param->ip[2] = 48;
	ip_param->ip[3] = 1;
	ip_param->netmask[0] = 255;
	ip_param->netmask[1] = 255;
	ip_param->netmask[2] = 255;
	ip_param->netmask[3] = 0;
	ip_param->gateway[0] = ip_param->ip[0];
	ip_param->gateway[1] = ip_param->ip[1];
	ip_param->gateway[2] = ip_param->ip[2];
	ip_param->gateway[3] = ip_param->ip[3];
	ip_param->dns1[0] = ip_param->ip[0];
	ip_param->dns1[1] = ip_param->ip[1];
	ip_param->dns1[2] = ip_param->ip[2];
	ip_param->dns1[3] = ip_param->ip[3];
	ip_param->dns2[0] = 0;
	ip_param->dns2[1] = 0;
	ip_param->dns2[2] = 0;
	ip_param->dns2[3] = 0;
    tls_param_set(TLS_PARAM_ID_SOFTAP_IP, (void*)ip_param, TRUE);
    tls_mem_free(ip_param);

    dnsname = tls_mem_alloc(32);
    if (!dnsname)
        return eWiFiFailure;

    strcpy((char *)dnsname, "local.wm");
    tls_param_set(TLS_PARAM_ID_DNSNAME, dnsname, FALSE);
    tls_mem_free(dnsname);

    WIFI_StartAP();

    return eWiFiSuccess;
}
/*-----------------------------------------------------------*/

WIFIReturnCode_t WIFI_SetPMMode( WIFIPMMode_t xPMModeType,
                                 const void * pvOptionValue )
{
    if (!pvOptionValue)
        return eWiFiFailure;

    switch (xPMModeType)
    {
        case eWiFiPMNormal:
        case eWiFiPMLowPower:
            tls_wifi_set_psflag(TRUE, FALSE);
            break;
        case eWiFiPMAlwaysOn:
            tls_wifi_set_psflag(FALSE, FALSE);
            break;
        default:
            break;
    }

    glastPMModeType = xPMModeType;
    return eWiFiSuccess;
}
/*-----------------------------------------------------------*/

WIFIReturnCode_t WIFI_GetPMMode( WIFIPMMode_t * pxPMModeType,
                                 void * pvOptionValue )
{
    if (!pxPMModeType || !pvOptionValue)
        return eWiFiFailure;

    //if (tls_wifi_get_psflag())
    //    *pxPMModeType = eWiFiPMLowPower;
    //else
    //    *pxPMModeType = eWiFiPMAlwaysOn;
    *pxPMModeType = glastPMModeType;
    return eWiFiSuccess;
}
/*-----------------------------------------------------------*/

BaseType_t WIFI_IsConnected(void)
{
	struct tls_ethif *ethif;
	ethif = tls_netif_get_ethif();
    if (ethif->status)
        return pdTRUE;
    else
	    return pdFALSE;
}
