/*
 * Amazon FreeRTOS Wi-Fi for LPC54018 IoT Module V1.0.1
 * Copyright (C) 2017 Amazon.com, Inc. or its affiliates.  All Rights Reserved.
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
 * @file aws_WIFI.c
 * @brief WiFi Interface.
 */

#include "aws_wifi.h"
#include "wifi_common.h"
#include "qcom_api.h"
#include "atheros_wifi.h"

/* This is here because the maximum DNS name length is defined in aws_secure_sockets.h.
 * Wifi must not have a dependency on aws_secure_sockets.h
 */
#define wifiMAX_DNS_NAME_LENGTH 253

/* Only 1 WiFi module is present at the time */
static uint8_t g_devid = 0;

/* NOTE: Could be located on stack */
static QCOM_SSID g_ssid = {0};

/* NOTE: Could be located on stack */
static QCOM_PASSPHRASE g_passphr = {0};

/* Semaphore for connection */
static SemaphoreHandle_t g_connect_semaph;

/* Protect API */
static SemaphoreHandle_t g_wifi_semaph;

/* WiFi/HW hardware configuration */
QCA_CONTEXT_STRUCT g_wifi_ctx = {0};

extern const QCA_MAC_IF_STRUCT ATHEROS_WIFI_IF;

/* WiFi interface object */
const QCA_IF_STRUCT g_wifi_if = {
    .MAC_IF         = &ATHEROS_WIFI_IF,
    .MAC_NUMBER     = 0,
    .PHY_NUMBER     = 0,
    .PHY_ADDRESS    = 0,
};

/* WiFi params */
const QCA_PARAM_STRUCT g_wifi_params = {
    .QCA_IF         = &g_wifi_if,
    .MODE           = Auto_Negotiate,
    .OPTIONS        = 0,
    .NUM_RX_PCBS    = WLAN_CONFIG_NUM_PRE_ALLOC_RX_BUFFERS,
};

/* Singleton, provides WiFi context structure */
QCA_CONTEXT_STRUCT *wlan_get_context(void)
{
    return &g_wifi_ctx;
}

static void ip_to_pxIPAddr(uint32_t ip, uint8_t *pxIPAddr)
{
    pxIPAddr[0] = (uint8_t)(ip >> 24);
    pxIPAddr[1] = (uint8_t)(ip >> 16);
    pxIPAddr[2] = (uint8_t)(ip >> 8);
    pxIPAddr[3] = (uint8_t)(ip);
}

static void pxIPAddr_to_ip(uint8_t *pxIPAddr, uint32_t *ip32)
{
    *ip32 = (((pxIPAddr[0]) & 0xFF) << 24) | \
            (((pxIPAddr[1]) & 0xFF) << 16) | \
            (((pxIPAddr[2]) & 0xFF) << 8) | \
            (((pxIPAddr[3]) & 0xFF));
}

/* Invoked from 'driver_task', on SUCCESS post semaphore */
static void aws_connect_cb(QCOM_ONCONNECT_EVENT event, uint8_t devid, QCOM_BSSID bssid, boolean bss_conn)
{
    BaseType_t result = pdFALSE;
    (void)result;
    if (QCOM_ONCONNECT_EVENT_SUCCESS == event)
    {
        result = xSemaphoreGive(g_connect_semaph);
    }
}

static WIFIReturnCode_t conv_security_to_qcom(WIFISecurity_t api_sec, WLAN_AUTH_MODE *qcom_auth, WLAN_CRYPT_TYPE *qcom_crypt)
{
    switch (api_sec)
    {
        case eWiFiSecurityOpen:
            *qcom_crypt = WLAN_CRYPT_NONE;
            *qcom_auth = WLAN_AUTH_NONE;
            break;
        case eWiFiSecurityWEP:
            *qcom_crypt = WLAN_CRYPT_WEP_CRYPT;
            *qcom_auth = WLAN_AUTH_WEP;
            break;
        case eWiFiSecurityWPA:
            // *qcom_crypt = WLAN_CRYPT_TKIP_CRYPT;
            *qcom_crypt = WLAN_CRYPT_AES_CRYPT;
            *qcom_auth = WLAN_AUTH_WPA_PSK;
            break;
        case eWiFiSecurityWPA2:
            // *qcom_crypt = WLAN_CRYPT_TKIP_CRYPT;
            *qcom_crypt = WLAN_CRYPT_AES_CRYPT;
            *qcom_auth = WLAN_AUTH_WPA2_PSK;
            break;
        default:
            return eWiFiFailure;
    }
    return eWiFiSuccess;
}

static WIFIReturnCode_t conv_qcom_to_mode(QCOM_WLAN_DEV_MODE dev_mode, WIFIDeviceMode_t *pxDeviceMode)
{
    switch (dev_mode)
    {
        case QCOM_WLAN_DEV_MODE_STATION:
            *pxDeviceMode = eWiFiModeStation;
            break;
        case QCOM_WLAN_DEV_MODE_AP:
            *pxDeviceMode = eWiFiModeAP;
            break;
        case QCOM_WLAN_DEV_MODE_ADHOC:
            *pxDeviceMode = eWiFiModeP2P;
            break;
        default:
            return eWiFiFailure;
    }
    return eWiFiSuccess;
}

static WIFIReturnCode_t conv_mode_to_qcom(WIFIDeviceMode_t xDeviceMode, QCOM_WLAN_DEV_MODE *dev_mode)
{
    switch (xDeviceMode)
    {
        case eWiFiModeStation:
            *dev_mode = QCOM_WLAN_DEV_MODE_STATION;
            break;
        case eWiFiModeAP:
            *dev_mode = QCOM_WLAN_DEV_MODE_AP;
            break;
        case eWiFiModeP2P:
            *dev_mode = QCOM_WLAN_DEV_MODE_ADHOC;
            break;
        default:
            return eWiFiFailure;
    }
    return eWiFiSuccess;
}

/**
 * @brief Initializes the WiFi module.
 *
 * This function must be called exactly once before any other
 * WiFi functions (including socket functions) can be used.
 *
 * @return eWiFiSuccess if everything succeeds, eWiFiFailure otherwise.
 */
WIFIReturnCode_t WIFI_On( void )
{
    A_STATUS result;

    /* Initialize WIFI shield */
    result = (A_STATUS)WIFISHIELD_Init();
    if (A_OK != result)
        return eWiFiFailure;

    /* Power off the WLAN and wait 30ms */
    CUSTOM_HW_POWER_UP_DOWN(NULL, false);
    vTaskDelay(MSEC_TO_TICK(30));

    g_wifi_ctx.PARAM_PTR = &g_wifi_params;
    if (A_OK != ATHEROS_WIFI_IF.INIT(&g_wifi_ctx))
        return eWiFiFailure;

    /* Disable low power mode to avoid SPI bus flood */
    qcom_power_set_mode(0, MAX_PERF_POWER, USER);

    /* Create a on_connect semaphore, */
    g_wifi_semaph = xSemaphoreCreateBinary();
    if (NULL == g_wifi_semaph)
        return eWiFiFailure;
    xSemaphoreGive(g_wifi_semaph);

    /* Create a on_connect semaphore, */
    g_connect_semaph = xSemaphoreCreateBinary();
    if (NULL == g_connect_semaph)
        return eWiFiFailure;

    /* Wait for Wifi */
    vTaskDelay(MSEC_TO_TICK(100));

    return eWiFiSuccess;
}

WIFIReturnCode_t WIFI_Off( void )
{
	return eWiFiNotSupported;
}

/**
 * @brief Connects to Access Point.
 *
 * @param[in] pxJoinAPParams Configuration to join AP.
 *
 * @return eWiFiSuccess if connection is successful, eWiFiFailure otherwise.
 */
WIFIReturnCode_t WIFI_ConnectAP( const WIFINetworkParams_t * const pxNetworkParams )
{
    WLAN_AUTH_MODE auth_mode;
    WLAN_CRYPT_TYPE crypt_type;
    WIFIReturnCode_t status = eWiFiFailure;
    const TickType_t xTimeout = pdMS_TO_TICKS( 10000UL );

    /* Check params */
    if (NULL == pxNetworkParams || NULL == pxNetworkParams->pcSSID || NULL == pxNetworkParams->pcPassword)
        return eWiFiFailure;

    /* Acquire semaphore */
    if (xSemaphoreTake(g_wifi_semaph, portMAX_DELAY) == pdTRUE)
    {
        do {
            /* Set WiFi to device mode */
            if (A_OK != (A_STATUS)qcom_op_set_mode(g_devid, QCOM_WLAN_DEV_MODE_STATION))
                break;

            /* Set SSID, must be done before auth, cipher and passphrase */
            strncpy(g_ssid.ssid, pxNetworkParams->pcSSID, sizeof(g_ssid));
            if (A_OK != (A_STATUS)qcom_set_ssid(g_devid, &g_ssid))
                break;

            /* Convert 'WIFISecurity_t' to 'WLAN_AUTH_MODE', 'WLAN_CRYPT_TYPE' */
            if (eWiFiSuccess != conv_security_to_qcom(pxNetworkParams->xSecurity, &auth_mode, &crypt_type))
                break;

            /* Set encyption mode */
            if (A_OK != (A_STATUS)qcom_sec_set_encrypt_mode(g_devid, crypt_type))
                break;

            /* Set auth mode */
            if (A_OK != qcom_sec_set_auth_mode(g_devid, auth_mode))
                break;

            /* Set passphrase */
            strncpy(g_passphr.passphrase, pxNetworkParams->pcPassword, sizeof(g_passphr));
            if (A_OK != qcom_sec_set_passphrase(g_devid, &g_passphr))
                break;

            /* Set channel */
            if (0 != pxNetworkParams->cChannel)
            {
                if (A_OK != qcom_set_channel(g_devid, pxNetworkParams->cChannel))
                  break;
            }

            /* Set connect_callback */
            if (A_OK != qcom_set_connect_callback(g_devid, (void *)aws_connect_cb))
                break;

            /* Commit settings to WiFi module */
            if (A_OK != qcom_commit(g_devid))
                break;

            /* Wait for callback, that is invoked from 'driver_task' context */
            if (pdTRUE != xSemaphoreTake(g_connect_semaph, xTimeout))
                break;

            /* Remove callback ?? */
            if (A_OK != qcom_set_connect_callback(g_devid, NULL))
                break;

            /* Everything is OK */
            status = eWiFiSuccess;
        } while (0);

        /* Release semaphore */
        xSemaphoreGive(g_wifi_semaph);
    }
    return status;
}

/**
 * @brief Disconnects from Access Point.
 *
 * @param[in] None.
 *
 * @return eWiFiSuccess if everything succeeds, failure code otherwise.
 */
WIFIReturnCode_t WIFI_Disconnect( void )
{
    //TODO: vSemaphoreDelete
    //      wait for disconnect cb ??
    WIFIReturnCode_t status = eWiFiFailure;

    /* Acquire semaphore */
    if (xSemaphoreTake(g_wifi_semaph, portMAX_DELAY) == pdTRUE)
    {
        if (A_OK == qcom_disconnect(g_devid))
            status = eWiFiSuccess;
        /* Release semaphore */
        xSemaphoreGive(g_wifi_semaph);
    }
    return status;
}

/**
 * @brief Resets the WiFi Module.
 *
 * @param[in] None.
 *
 * @return eWiFiSuccess if everything succeeds, failure code otherwise.
 */
WIFIReturnCode_t WIFI_Reset( void )
{
    return eWiFiNotSupported;
}

/**
 * @brief Sets wifi mode.
 *
 * @param[in] xDeviceMode - Mode of the device Station / Access Point /P2P.
 *
 * @return eWiFiSuccess if everything succeeds, failure code otherwise.
 */
WIFIReturnCode_t WIFI_SetMode( WIFIDeviceMode_t xDeviceMode )
{
    QCOM_WLAN_DEV_MODE dev_mode = QCOM_WLAN_DEV_MODE_INVALID;
    WIFIReturnCode_t status = eWiFiFailure;

    /* Acquire semaphore */
    if (xSemaphoreTake(g_wifi_semaph, portMAX_DELAY) == pdTRUE)
    {
        do {
            if (eWiFiSuccess != conv_mode_to_qcom(xDeviceMode, &dev_mode))
                break;
            if (A_OK != (A_STATUS)qcom_op_set_mode(g_devid, dev_mode))
                break;
            status = eWiFiSuccess;
        } while (0);
        /* Release semaphore */
        xSemaphoreGive(g_wifi_semaph);
    }
    return status;;
}

/**
 * @brief Gets wifi mode.
 *
 * @param[in] pxDeviceMode - return mode Station / Access Point /P2P
 *
 * @return eWiFiSuccess if everything succeeds, failure code otherwise.
 */
WIFIReturnCode_t WIFI_GetMode( WIFIDeviceMode_t * pxDeviceMode )
{
    QCOM_WLAN_DEV_MODE dev_mode = QCOM_WLAN_DEV_MODE_INVALID;
    WIFIReturnCode_t status = eWiFiFailure;

    /* Check params */
    if (NULL == pxDeviceMode)
        return eWiFiFailure;

    /* Acquire semaphore */
    if (xSemaphoreTake(g_wifi_semaph, portMAX_DELAY) == pdTRUE)
    {
        do {
            if (A_OK != (A_STATUS)qcom_op_get_mode(g_devid, &dev_mode))
                break;
            if (eWiFiSuccess != conv_qcom_to_mode(dev_mode, pxDeviceMode))
                break;
            status = eWiFiSuccess;
        } while(0);
        /* Release semaphore */
        xSemaphoreGive(g_wifi_semaph);
    }
    return status;
}

/**
 * @brief WiFi Add Network profile.
 *
 * Adds wifi network to network list in NV memory
 *
 * @param[in] pxNetworkProfile - network profile parameters
 * @param[out] pusIndex - network profile index
 *
 * @return Profile stored index on success, or negative error code on failure..
 *
 * @see WIFINetworkParams_t
 */
WIFIReturnCode_t WIFI_NetworkAdd(const WIFINetworkProfile_t * const pxNetworkProfile, uint16_t * pusIndex )
{
    return eWiFiNotSupported;
}


/**
 * @brief WiFi Get Network profile .
 *
 * Gets wifi network params at given index from network list in Non volatile memory
 *
 * @param[out] pxNetworkProfile - pointer to return network profile parameters
 * @param[in] usIndex - Index of the network profile, must be between 0 to wificonfigMAX_NETWORK_PROFILES
 * @return eWiFiSuccess if success, eWiFiFailure otherwise.
 *
 * @see WIFINetworkParams_t
 */
WIFIReturnCode_t WIFI_NetworkGet( WIFINetworkProfile_t * pxNetworkProfile,
                                            uint16_t uxIndex )
{
    return eWiFiNotSupported;
}

/**
 * @brief WiFi Delete Network profile .
 *
 * Deletes wifi network from network list at given index in NV memory
 *
 * @param[in] usIndex - Index of the network profile, must be between 0 to wificonfigMAX_NETWORK_PROFILES
 *                      wificonfigMAX_NETWORK_PROFILES as index will delete all network profiles
 *
 * @return eWiFiSuccess if deleted, eWiFiFailure otherwise.
 *
 */
WIFIReturnCode_t WIFI_NetworkDelete( uint16_t uxIndex )
{
    return eWiFiNotSupported;
}

/**
 * @brief Ping an IP address in the network.
 *
 * @param[in] IP Address to ping.
 * @param[in] Number of times to ping
 * @param[in] Interval in mili seconds for ping operation
 *
 * @note 'WIFI_GetIP' must be invoked before
 * @return eWiFiSuccess if everything succeeds, failure code otherwise.
 */
WIFIReturnCode_t WIFI_Ping( uint8_t * pxIPAddr,
                                       uint16_t xCount,
                                       uint32_t xIntervalMS )
{
    uint32_t failed_cnt = 0;
    uint32_t ip4_addr = 0;

    /* Check params */
    if ((NULL == pxIPAddr) || (0 == xCount))
        return eWiFiFailure;

    /* Acquire semaphore */
    if (xSemaphoreTake(g_wifi_semaph, portMAX_DELAY) == pdTRUE)
    {
        pxIPAddr_to_ip(pxIPAddr, &ip4_addr);
        for (uint16_t i = 0; i < xCount; i++)
        {
            if (xIntervalMS == 0)
                xIntervalMS = 20000; // max 20 seconds
            if (A_OK != qcom_ping_ms(ip4_addr, 32, xIntervalMS))
            {
                failed_cnt++;
            }
        }
        /* Release semaphore */
        xSemaphoreGive(g_wifi_semaph);
    }
    /* Report failure if all attempts failed */
    return failed_cnt == xCount ? eWiFiFailure : eWiFiSuccess;
}

/**
 * @brief Retrieves the WiFi interface's IP address.
 *
 * @param[in] IP Address buffer.
 *
 * @return eWiFiSuccess if everything succeeds, failure code otherwise.
 */
WIFIReturnCode_t WIFI_GetIP( uint8_t * pxIPAddr )
{
    uint32_t ip4_addr = 0, ip4_mask = 0, ip4_gw = 0;
    WIFIReturnCode_t status = eWiFiFailure;

    /* Check params */
    if (NULL == pxIPAddr)
        return eWiFiFailure;

    /* Acquire semaphore */
    if (xSemaphoreTake(g_wifi_semaph, portMAX_DELAY) == pdTRUE)
    {
        do {
            /* Call DHCP after connection */
            if (A_OK != qcom_ipconfig(g_devid, QCOM_IPCONFIG_DHCP, &ip4_addr, &ip4_mask, &ip4_gw))
                break;
            /* Retrieve IP info */
            if (A_OK != qcom_ipconfig(g_devid, QCOM_IPCONFIG_QUERY, &ip4_addr, &ip4_mask, &ip4_gw))
                break;
            ip_to_pxIPAddr(ip4_addr, pxIPAddr);
            status = eWiFiSuccess;
        } while(0);
        /* Release semaphore */
        xSemaphoreGive(g_wifi_semaph);
    }
    return status;
}

/**
 * @brief Retrieves the WiFi interface's MAC address.
 *
 * @param[in] MAC Address buffer.
 *
 * @return eWiFiSuccess if everything succeeds, failure code otherwise.
 */
WIFIReturnCode_t WIFI_GetMAC( uint8_t * pxMac )
{
    WIFIReturnCode_t status = eWiFiFailure;
    uint8_t mac_addr[ATH_MAC_LEN] = {0};

    /* Check params */
    if (NULL == pxMac)
        return eWiFiFailure;

    /* Acquire semaphore */
    if (xSemaphoreTake(g_wifi_semaph, portMAX_DELAY) == pdTRUE)
    {
        do {
            if (A_OK != qcom_get_bssid(g_devid, mac_addr))
                break;
            memcpy(pxMac, mac_addr, ATH_MAC_LEN);
            status = eWiFiSuccess;
        } while (0);
        /* Release semaphore */
        xSemaphoreGive(g_wifi_semaph);
    }
    return status;
}


#define WIFI_DNS_HEADER_FLAGS_QR    (0x8000U)
#define WIFI_DNS_HEADER_FLAGS_AA    (0x0400U)
#define WIFI_DNS_HEADER_FLAGS_TC    (0x0200U)
#define WIFI_DNS_HEADER_FLAGS_RD    (0x0100U)
#define WIFI_DNS_HEADER_FLAGS_RA    (0x0080U)
#define WIFI_DNS_TYPE_A             (0x0001U)
#define WIFI_DNS_HEADER_CLASS_IN    (0x01U)


/* Taken from FNET */
typedef PREPACK struct
{
    uint16_t id;
    uint16_t flags;
    uint16_t qdcount;
    uint16_t ancount;
    uint16_t nscount;
    uint16_t arcount;
} POSTPACK wifi_dns_header_t;


/* Taken from FNET */
typedef PREPACK struct
{
    uint8_t   zero_length;
    uint16_t  qtype;
    uint16_t  qclass;

} POSTPACK wifi_dns_q_tail_t;


/* Taken from FNET */
typedef PREPACK struct
{
    union
    {
        uint8_t   name_ptr_c[2];
        uint16_t  name_ptr;
    } name;
    uint16_t   type;
    uint16_t   rr_class;
    uint32_t   ttl;
    uint16_t   rdlength;
    uint32_t   rdata;

} POSTPACK wifi_dns_rr_header_t;


static int32_t wifi_dns_assemble_request(char *buffer, char *hostname, int32_t buff_len)
{
    uint32_t total_length = 0, label_length = 0;
    wifi_dns_q_tail_t   *q_tail;
    /* Separate strings by '\0'*/
    uint32_t hostname_len = strlen(hostname);

    /* Buffer too small to handle 'hostname' */
    if ((buff_len < 0) || (buff_len <= hostname_len + sizeof(wifi_dns_q_tail_t)))
        return -1;

    strcpy(&buffer[1], hostname);
    buffer[0] = '\0';

    /* Force '\0' delimiter */
    for (int32_t i = 0; i < hostname_len + 1; i++)
    {
        if ('.' == buffer[i])
            buffer[i] = '\0';
    }

    /* Replace '\0' with string length */
    total_length = 0;
    while (label_length = strlen(&buffer[total_length + 1]))
    {
        buffer[total_length] = label_length;
        total_length += label_length + 1;
    }

    q_tail = (wifi_dns_q_tail_t *)&buffer[total_length];

    /* QTYPE */
    q_tail->qtype = A_CPU2BE16(WIFI_DNS_TYPE_A);

    /* QCLASS */
    q_tail->qclass = A_CPU2BE16(WIFI_DNS_HEADER_CLASS_IN);

    return (total_length + sizeof(wifi_dns_q_tail_t));
}


static uint16_t wifi_id_cnt = 1;


static WIFIReturnCode_t wifi_dns_resolver(IP_ADDR_T dns_ip, char * hostname, IP_ADDR_T *host_ip, uint32_t timeout_ms)
{
    int32_t dns_sock = -1;
    SOCKADDR_T dns_addr = {0};
    char * tx_buffer = NULL;
    uint32_t tx_len = 0;
    char * rx_buffer = NULL;
    wifi_dns_rr_header_t *dns_resp = NULL;
    wifi_dns_header_t *dns_hdr = NULL;
    int32_t result = 0;
    uint16_t dns_addr_len = sizeof(dns_addr);
    WIFIReturnCode_t status = eWiFiFailure;
    int32_t tmp_len = 0;

    const uint32_t rx_buffer_max = 512;
    /* The total DNS query message is the size of the header + a null character before the host name + the maximum host name
     * length + the size of the tail */
    const uint32_t tx_buffer_max = sizeof(wifi_dns_header_t) + 1 + wifiMAX_DNS_NAME_LENGTH + sizeof(wifi_dns_q_tail_t);

    do {
        /* Try to allocate buffer for request, must be fill with '\0' */
        tx_buffer = (char *)CUSTOM_ALLOC(tx_buffer_max);
        memset(tx_buffer, 0, tx_buffer_max);
        if (NULL == tx_buffer)
            break;

        /* Set DNS address */
        /* qcom code is expecting the port, address, and family in host order */
        dns_addr.sin_port = 53;
        dns_addr.sin_addr.s_addr = dns_ip.s_addr;
        dns_addr.sin_family = ATH_AF_INET;

        /* Create UDP socket */
        dns_sock = qcom_socket(ATH_AF_INET, SOCK_DGRAM_TYPE, 0);
        if (-1 == dns_sock)
            break;

        /* Prepare request header */
        dns_hdr = (wifi_dns_header_t*)&tx_buffer[0];
        dns_hdr->id = wifi_id_cnt++;
        dns_hdr->flags = A_CPU2BE16(WIFI_DNS_HEADER_FLAGS_RD);
        dns_hdr->qdcount = A_CPU2BE16(1U);

        /* Prepare request body */
        tx_len = sizeof(wifi_dns_header_t);
        if (0 > (tmp_len = wifi_dns_assemble_request(&tx_buffer[ tx_len ], hostname, tx_buffer_max - tx_len)))
            break;
        tx_len += tmp_len;

        /* Send data over UDP */
        result = qcom_sendto(dns_sock, tx_buffer, tx_len, 0, (void *)(&dns_addr), sizeof(dns_addr));
        if (-1 == result)
            break;

        /* Free TX resources*/
        CUSTOM_FREE(tx_buffer);
        tx_buffer = NULL;

        /* Obtain WiFi context */
        QCA_CONTEXT_STRUCT *qcaCtx = wlan_get_context();
        if (NULL == qcaCtx)
            break;

        /* Block on receive max 'timeout_ms' */
        result = t_select(qcaCtx, dns_sock, timeout_ms);
        if (A_OK != result)
            break;

        /* Receive data from DNS server */
        result = qcom_recvfrom(dns_sock, &rx_buffer, rx_buffer_max, 0, (struct sockaddr *)&dns_addr, &dns_addr_len);
        if (-1 == result)
          break;

        /* Typecast header */
        dns_hdr = (wifi_dns_header_t*)rx_buffer;

        /* Try to find response (should be always at addr '&rx_buffer[ tx_len ]') */
        for (tx_len = sizeof(wifi_dns_header_t); tx_len < result; tx_len++)
        {
            /* However, more stable solution is to find DNS_NAME_COMPRESSED_MASK (0xC) in stream (according to FNET) */
            if (0xC0 == rx_buffer[tx_len])
            {
                /* Typecast response */
                dns_resp = (wifi_dns_rr_header_t*)&rx_buffer[ tx_len ];
                /* Expected IPv4 response*/
                if (
                    (dns_resp->rr_class == A_CPU2BE16(WIFI_DNS_HEADER_CLASS_IN)) &&
                    (dns_resp->type == A_CPU2BE16(WIFI_DNS_TYPE_A))
                 )
                {
                    /* Get IPv4 address */
                    /* Uncomment in case of wrong byte order */
                    // host_ip->s_addr = A_CPU2BE32(dns_resp->rdata);
                    host_ip->s_addr = dns_resp->rdata;
                    status = eWiFiSuccess;
                    break;
                }
            }
        }
    } while(0);

    /* Cleanup */
    /* Free TX buffer */
    if (NULL != tx_buffer)
    {
        CUSTOM_FREE(tx_buffer);
        tx_buffer = NULL;
    }
    /* Free RxBuffer */
    if (NULL != rx_buffer)
    {
        zero_copy_free(rx_buffer);
        rx_buffer = NULL;
    }
    /* Close socket */
    if (-1 != dns_sock)
    {
        qcom_socket_close(dns_sock);
        dns_sock = -1;
    }

    return status;
}


/**
 * @brief Retrieves host IP address from URL using DNS
 *
 * @param[in] pxHost - Host URL.
 * @param[in] pxIPAddr - IP Address buffer.
 *
 * @return eWiFiSuccess if everything succeeds, failure code otherwise.
 */
WIFIReturnCode_t WIFI_GetHostIP( char * pxHost,
                                            uint8_t * pxIPAddr )
{
    WIFIReturnCode_t status = eWiFiFailure;
    uint32_t result = A_OK;
    IP_ADDR_T tmp_ip = {0};
    IP_ADDR_T dns_ip = {0};
    uint32_t dns_servers[MAX_DNSADDRS] = {0};
    uint32_t dns_servers_num = 0;

    /* Check params */
    if ((NULL == pxIPAddr) || (NULL == pxHost))
        return eWiFiFailure;

    /* Acquire semaphore */
    if (xSemaphoreTake(g_wifi_semaph, portMAX_DELAY) == pdTRUE)
    {
        do {
            /* Obtain DNS IP from DHCP result */
            result = qcom_dns_server_address_get(dns_servers, &dns_servers_num);
            if ((A_OK != result) || (dns_servers_num == 0))
                break;

            /* Perform DNS/UDP request, loop over DNS servers until first success */
            for (uint32_t dns_idx = 0; dns_idx < dns_servers_num; dns_idx++)
            {
                dns_ip.s_addr = dns_servers[dns_idx];
                if (eWiFiSuccess != wifi_dns_resolver(dns_ip, pxHost, &tmp_ip, wificonfigDNS_QUERY_TIMEOUT))
                    continue;

                /* Copy to output format and terminate loop*/
                pxIPAddr[0] = (uint8_t)(tmp_ip.s_addr >> 24);
                pxIPAddr[1] = (uint8_t)(tmp_ip.s_addr >> 16);
                pxIPAddr[2] = (uint8_t)(tmp_ip.s_addr >> 8);
                pxIPAddr[3] = (uint8_t)(tmp_ip.s_addr);
                status = eWiFiSuccess;
                break;
            }
        } while(0);
        /* Release semaphore */
        xSemaphoreGive(g_wifi_semaph);
    }

    return status;
}

/**
 * @brief Retrieves IP address in Access Point mode
 *
 * @param[in] pxIPAddr - IP Address buffer.
 *
 * @return eWiFiSuccess if everything succeeds, failure code otherwise.
 */
WIFIReturnCode_t WIFI_GetAccessPointIP( uint8_t * pxIPAddr )
{
    uint32_t ip4_addr = 0, ip4_mask = 0, ip4_gw = 0;
    WIFIReturnCode_t status = eWiFiFailure;

    /* Check params */
    if (NULL == pxIPAddr)
        return eWiFiFailure;

    /* Acquire semaphore */
    if (xSemaphoreTake(g_wifi_semaph, portMAX_DELAY) == pdTRUE)
    {
        do {
            /* Call DHCP after connection */
            if (A_OK != qcom_ipconfig(g_devid, QCOM_IPCONFIG_DHCP, &ip4_addr, &ip4_mask, &ip4_gw))
                break;
            /* Retrieve IP info */
            if (A_OK != qcom_ipconfig(g_devid, QCOM_IPCONFIG_QUERY, &ip4_addr, &ip4_mask, &ip4_gw))
                break;
            ip_to_pxIPAddr(ip4_addr, pxIPAddr);
            status = eWiFiSuccess;
        } while (0);
        /* Release semaphore */
        xSemaphoreGive(g_wifi_semaph);
    }
    return status;
}

/**
 * @brief Perform WiF Scan
 *
 * @param[in] pxBuffer - Buffer for scan results.
 * @param[in] uxNumNetworks - Number of networks in scan result.
 *
 * @return eWiFiSuccess if everything succeeds, failure code otherwise.
 */
WIFIReturnCode_t WIFI_Scan( WIFIScanResult_t * pxBuffer,
                                       uint8_t uxNumNetworks )
{
    WIFIReturnCode_t status = eWiFiFailure;
    QCOM_BSS_SCAN_INFO *scan_result = NULL;
    int16_t scan_result_num = 0;

    /* Check params */
    if ((NULL == pxBuffer) || (0 == uxNumNetworks))
        return eWiFiFailure;

    /* Acquire semaphore */
    if (xSemaphoreTake(g_wifi_semaph, portMAX_DELAY) == pdTRUE)
    {
        do {
            /* Note: won't work if SSID is set already */
            if (A_OK != qcom_set_scan(g_devid, NULL))
                break;
            if (qcom_get_scan(g_devid, &scan_result, &scan_result_num))
                break;
            /* Get MIN(scan_result_num, uxNumNetworks) */
            if (scan_result_num > (int16_t)uxNumNetworks)
                scan_result_num = (int16_t)uxNumNetworks;
            for (int16_t i  = 0 ; i < scan_result_num; i++)
            {
                strncpy((char*)pxBuffer->cSSID, (char const*)scan_result[i].ssid, wificonfigMAX_SSID_LEN);
                strncpy((char*)pxBuffer->ucBSSID, (char const*)scan_result[i].bssid, wificonfigMAX_BSSID_LEN);
                pxBuffer->cRSSI = scan_result[i].rssi;
                pxBuffer->cChannel = scan_result[i].channel;
                pxBuffer->ucHidden = 0;
                if (!scan_result[i].security_enabled)
                {
                    pxBuffer->xSecurity = eWiFiSecurityOpen;
                }
                else if (
                    (0 == scan_result[i].rsn_cipher) ||
                    (ATH_CIPHER_TYPE_WEP & scan_result[i].rsn_cipher)
                )
                {
                    pxBuffer->xSecurity = eWiFiSecurityWEP;
                }
                else if (ATH_CIPHER_TYPE_CCMP & scan_result[i].rsn_cipher)
                {
                    pxBuffer->xSecurity = eWiFiSecurityWPA2;
                }
                else
                {
                    /* TODO: Expect WPA */
                    pxBuffer->xSecurity = eWiFiSecurityWPA;
                }
#if 0
                PRINTF("-----------------------\r\n");
                PRINTF("channel: %d \r\n", scan_result[i].channel);
                PRINTF("ssid_len: %d \r\n", scan_result[i].ssid_len);
                PRINTF("rssi: %d \r\n", scan_result[i].rssi);
                PRINTF("security_enabled: %d \r\n", scan_result[i].security_enabled);
                PRINTF("beacon_period: %d \r\n", scan_result[i].beacon_period);
                PRINTF("preamble: %d \r\n", scan_result[i].preamble);
                PRINTF("bss_type: %d \r\n", scan_result[i].bss_type);
                PRINTF("bssid: %x%x%x%x%x%x \r\n",
                    scan_result[i].bssid[0],
                    scan_result[i].bssid[1],
                    scan_result[i].bssid[2],
                    scan_result[i].bssid[3],
                    scan_result[i].bssid[4],
                    scan_result[i].bssid[5]
                );
                PRINTF("ssid: %s \r\n", scan_result[i].ssid);
                PRINTF("rsn_cipher: %d \r\n", scan_result[i].rsn_cipher);
                PRINTF("rsn_auth: %d \r\n", scan_result[i].rsn_auth);
                PRINTF("wpa_cipher: %d \r\n", scan_result[i].wpa_cipher);
                PRINTF("wpa_auth: %d \r\n", scan_result[i].wpa_auth);
#endif

                pxBuffer += 1;
            }
            status = eWiFiSuccess;
        } while (0);
        /* Release semaphore */
        xSemaphoreGive(g_wifi_semaph);
    }
    return status;
}

/**
 * @brief Configure SoftAP
 *
 * @param[in] pxNetworkParams - Network params to configure AP.
 *
 * @return eWiFiSuccess if everything succeeds, failure code otherwise.
 */
WIFIReturnCode_t WIFI_ConfigureAP(const WIFINetworkParams_t * const pxNetworkParams )
{
    WIFIReturnCode_t status = eWiFiFailure;
    WLAN_AUTH_MODE auth_mode = WLAN_AUTH_INVALID;
    WLAN_CRYPT_TYPE crypt_type = WLAN_CRYPT_INVALID;

    /* Check params */
    if (NULL == pxNetworkParams || NULL == pxNetworkParams->pcSSID || NULL == pxNetworkParams->pcPassword)
        return eWiFiFailure;

    /* Acquire semaphore */
    if (xSemaphoreTake(g_wifi_semaph, portMAX_DELAY) == pdTRUE)
    {
        do {
            /* Set WiFi to device mode */
            if (A_OK != (A_STATUS)qcom_op_set_mode(g_devid, QCOM_WLAN_DEV_MODE_AP))
                break;

            /* Set SSID, must be done before auth, cipher and passphrase */
            strncpy(g_ssid.ssid, pxNetworkParams->pcSSID, sizeof(g_ssid));
            if (A_OK != (A_STATUS)qcom_set_ssid(g_devid, &g_ssid))
                break;

            /* Convert 'WIFISecurity_t' to 'WLAN_AUTH_MODE', 'WLAN_CRYPT_TYPE' */
            if (eWiFiSuccess != conv_security_to_qcom(pxNetworkParams->xSecurity, &auth_mode, &crypt_type))
                break;

            /* Set encyption mode */
            if (A_OK != (A_STATUS)qcom_sec_set_encrypt_mode(g_devid, crypt_type))
                break;

            /* Set auth mode */
            if (A_OK != qcom_sec_set_auth_mode(g_devid, auth_mode))
                break;

            /* Set passphrase */
            strncpy(g_passphr.passphrase, pxNetworkParams->pcPassword, sizeof(g_passphr));
            if (A_OK != qcom_sec_set_passphrase(g_devid, &g_passphr))
                break;

            status = eWiFiSuccess;
        } while (0);

        /* Release semaphore */
        xSemaphoreGive(g_wifi_semaph);
    }

    return status;
}

WIFIReturnCode_t WIFI_SetPMMode( WIFIPMMode_t xPMModeType,
                                 const void * pvOptionValue )
{
	return eWiFiNotSupported;
}

WIFIReturnCode_t WIFI_GetPMMode( WIFIPMMode_t * pxPMModeType,
                                 void * pvOptionValue )
{
	return eWiFiNotSupported;
}
