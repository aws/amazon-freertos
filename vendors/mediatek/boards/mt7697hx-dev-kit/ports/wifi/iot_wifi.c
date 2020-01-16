/* Copyright Statement:
 *
 * FreeRTOS Wi-Fi glue layer.
 * (c) 2018 MediaTek Inc. or its affiliates. All Rights Reserved.
 * 
 * FreeRTOS Wi-Fi for MT7697Hx-Dev-Kit V1.0.1
 * Copyright (C) 2020 Amazon.com, Inc. or its affiliates.  All Rights Reserved.
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
 * @brief WiFi Interface.
 */

#include "FreeRTOS.h"
#include "event_groups.h"

#include <string.h>

#include "iot_logging_task.h"

/* Socket and WiFi interface includes. */
#include "iot_wifi.h"

/* WiFi configuration includes. */
#include "aws_wifi_config.h"
#include "aws_wifi_hal.h"

/* MTK Wi-Fi includes. */
#include "wifi_api.h"

#include "wifi_scan.h"

#include "wifi_private_api.h"

#include "ethernetif.h"

#include "lwip/dns.h"

#include "lwip/inet.h"

#include "lwip/dhcp.h"

#include "wifi_lwip_helper.h"

#include "lwip/netdb.h"

#include "ping_api.h"

#include "syslog.h"

#include "dhcpd.h"
/*-----------------------------------------------------------
 *
 * Constants and Macros
 *
 *-----------------------------------------------------------*/


#define MAX_SSID_SEARCH_COUNT       10

#if 0

#undef  LOG_E
#define LOG_E(module, msg...) configPRINTF(("! " msg))


#undef  LOG_W
#define LOG_W(module, msg...) configPRINTF(("w " msg))


#undef  LOG_I
#define LOG_I(module, msg...) configPRINTF(("i " msg))

#endif


/*-----------------------------------------------------------*/


#if 1
#define ENTER
#define EXIT
#else
#define ENTER   LOG_I( wifi, "%s: enter\n", __FUNCTION__ );
#define EXIT    LOG_I( wifi, "%s: exit (%u)\n", __FUNCTION__, __LINE__ );
#endif


/*-----------------------------------------------------------*/


#define D( X )  vLoggingPrintf X


/*-----------------------------------------------------------*/


#define PRIMARY_DNS   ("8.8.8.8")

#define SECONDARY_DNS ("192.168.2.1")

#define IP_POOL_START ("192.168.2.2")

#define IP_POOL_END   ("192.168.2.5")


/*-----------------------------------------------------------
 *
 * Data structure declaration.
 *
 *-----------------------------------------------------------*/


/*
 * Manipulation functions:
 *
 * struct handshake * handshake_create( void );
 *
 * void handshake_take( struct handshake * handle );
 *
 * void handshake_clear( struct handshake * handle );
 *
 * void handshake_take_and_free( struct handshake * handle );
 *
 * void handshake_free( struct handshake * handle );
 *
 * void handshake_give( struct handshake * handle );
 */
struct handshake
{
    SemaphoreHandle_t   sem;
};


/*-----------------------------------------------------------*/


/**
 * 1. fw_ready
 *
 *    Before we could do anything to Wi-Fi, F/W must be downloaded to network
 *    processor. When it is done, fw_ready is set to true. Because Amazon
 *    FreeRTOS has no specific API for firmware download, we hide download
 *    procedure in WIFI_On() and it will be done only once per boot up. The
 *    boolean flag fw_ready is a flag to be set when it is done.
 *
 * 2. on
 *
 *    When RF radio is on, this flag is set.
 *    When RF radio is off, this flag is cleared.
 *
 * 3. op_mode
 *
 *    The mode of the Wi-Fi hardware and stack.
 *
 *    Amazon has 3 modes in FreeRTOS Wi-Fi API:
 *
 *          eWiFiModeStation
 *          eWiFiModeAP
 *          eWiFiModeP2P
 *
 *    MediaTek supports 4 modes in Wi-Fi API:
 *
 *          WIFI_MODE_MONITOR
 *
 *          WIFI_MODE_STA_ONLY, supported interface is: WIFI_PORT_STA
 *
 *          WIFI_MODE_AP_ONLY,  supported interface is: WIFI_PORT_AP
 *
 *          WIFI_MODE_P2P_ONLY, supported interface is: ?
 */
struct mtk_wifi_state_t
{
    bool                fw_ready;

    bool                on;

    bool                connected;

    uint16_t            ssid_not_found;

    uint16_t            connect_fail_result_code;
	
		uint8_t 						ucSSID[ wificonfigMAX_SSID_LEN ];
		uint8_t 						ucSSIDLength;
    
		WIFISecurity_t 			xSecurity;

    bool                ap_ip_up;

    bool                sta_ip_up;
    sta_ip_mode_t       sta_ip_mode; /* STA_IP_MODE_DHCP, or STA_IP_MODE_STATIC */
    bool                sta_ip_ready_wait;
    bool                sta_ip_ready;

    SemaphoreHandle_t   critical_section;

    struct handshake *  handshake_init_complete;
    struct handshake *  handshake_wifi_connected;
    struct handshake *  handshake_wifi_disconnected;
    struct handshake *  handshake_ip_addr_ready;
    struct handshake *  handshake_scan_complete;
};


/*-----------------------------------------------------------
 *
 * Global Variables.
 *
 *-----------------------------------------------------------*/


/*
 * the aggregated status of Wi-Fi stack and hardware.
 */
static struct mtk_wifi_state_t      _g_state;


/*-----------------------------------------------------------
 *
 * Handshake Functions
 *
 *-----------------------------------------------------------*/


struct handshake * handshake_create( void )
{
    struct handshake * handle = pvPortMalloc( sizeof( *handle ) );
    configASSERT( handle != NULL );

    handle->sem = xSemaphoreCreateBinary();

    configASSERT( handle->sem != NULL );

    return handle;
}


/*-----------------------------------------------------------*/

void handshake_take( struct handshake * handle )
{
    configASSERT( handle );

    xSemaphoreTake( handle->sem, portMAX_DELAY );
}


/*-----------------------------------------------------------*/

void handshake_clear( struct handshake * handle )
{
    configASSERT( handle );

    xSemaphoreTake( handle->sem, ( TickType_t  ) 0 );
}


/*-----------------------------------------------------------*/


void handshake_take_and_free( struct handshake * handle )
{
    handshake_take( handle );

    vSemaphoreDelete( handle->sem );
    handle->sem = NULL;
    vPortFree( handle );
}


/*-----------------------------------------------------------*/


void handshake_free( struct handshake * handle )
{
    vSemaphoreDelete( handle->sem );
    handle->sem = NULL;
    vPortFree( handle );
}


/*-----------------------------------------------------------*/


void handshake_give( struct handshake * handle )
{
    configASSERT( handle );
    xSemaphoreGive( handle->sem );
}


/*-----------------------------------------------------------
 *
 * IP Stack Wrapping Functions
 *
 *-----------------------------------------------------------*/


/**
  * @brief  dhcp got ip will callback this function.
  * @param[in] struct netif *netif: which network interface got ip.
  * @retval None
  */
static void _ip_ready_callback( struct netif *netif )
{
    if ( ! ip4_addr_isany_val( netif->ip_addr ) )
    {
        char ip_addr[ 17 ] = { 0 };

        if( NULL != inet_ntoa( netif->ip_addr ) )
        {
            strcpy( ip_addr, inet_ntoa( netif->ip_addr ) );
            LOG_I( common, "************************\n" );
            LOG_I( common, "DHCP got IP:%s\n", ip_addr  );
            LOG_I( common, "************************\n" );
        } else {
            LOG_E( common, "DHCP failed\n" );
        }

#ifdef MTK_WIFI_REPEATER_ENABLE
        uint8_t         op_mode = 0;
        struct netif    *ap_if;

        wifi_config_get_opmode( &op_mode );

        if( WIFI_MODE_REPEATER == op_mode )
        {
            ap_if  = netif_find_by_type( NETIF_TYPE_AP );
            netif_set_addr( ap_if, &netif->ip_addr, &netif->netmask, &netif->gw );
        }
#endif

       /*This is a private API , which used to inform IP is ready to wifi driver
        *In present, WiFi Driver will do some operation when this API is invoked, such as:
        *Do WiFi&BLE Coexstence relative behavior if BLE is enabled and do Power Saving Status change.
        *This API will be improved, user may need to use new API to replace it in future*/
        wifi_connection_inform_ip_ready();
    }

    _g_state.sta_ip_ready = true;
    handshake_give( _g_state.handshake_ip_addr_ready );
}

/*-----------------------------------------------------------*/

static void tcpip_config_init(lwip_tcpip_config_t *tcpip_config)
{
    memset( tcpip_config, 0, sizeof(*tcpip_config) );

    IP4_ADDR( &tcpip_config->sta_addr,    192, 168,   1,   1 );
    IP4_ADDR( &tcpip_config->sta_mask,    255, 255, 255,   0 );
    IP4_ADDR( &tcpip_config->sta_gateway, 192, 168,   1, 254 );

    IP4_ADDR( &tcpip_config->ap_addr,     192, 168,   2,   1 );
    IP4_ADDR( &tcpip_config->ap_mask,     255, 255, 255,   0 );
    IP4_ADDR( &tcpip_config->ap_gateway,  192, 168,   2,   1 );
}


static void _ip_init( uint8_t op_mode )
{
    lwip_tcpip_config_t     tcpip_config;

    _g_state.handshake_wifi_connected    = handshake_create();
    _g_state.handshake_wifi_disconnected = handshake_create();
    _g_state.handshake_ip_addr_ready     = handshake_create();
    _g_state.sta_ip_mode                 = STA_IP_MODE_DHCP;
    _g_state.sta_ip_ready_wait           = false;
    _g_state.sta_ip_ready                = false;

    tcpip_config_init( &tcpip_config );
    lwip_tcpip_init  ( &tcpip_config, op_mode );
}


/*
 * Wait Wi-Fi and IP ready. Only useful in station and repeater mode.
 */
static bool _mtk_sta_ip_is_ready( void )
{
    _g_state.sta_ip_ready_wait = true;
    handshake_take( _g_state.handshake_wifi_connected );

    if( ! _g_state.connected )
    {
        LOG_W( wifi, "%s: IP not ready (wifi not connected.)\n", __FUNCTION__ );

        switch( _g_state.connect_fail_result_code )
        {

        case WIFI_REASON_CODE_FIND_AP_FAIL:
        {
            LOG_W( common, "%s: stop scan\n", __FUNCTION__ );

            /* SSID and corresponding security type not found, stop scan */
            wifi_connection_stop_scan();
            break;
        }

        case WIFI_REASON_CODE_PREV_AUTH_INVALID:
        {
            LOG_W( common, "%s: WIFI_REASON_CODE_PREV_AUTH_INVALID\n", __FUNCTION__ );
            break;
        }

        case WIFI_REASON_CODE_PASSWORD_ERROR:
        {
            LOG_W( common, "%s: WIFI_REASON_CODE_PASSWORD_ERROR\n", __FUNCTION__ );
            break;
        }

        default:
        {
            LOG_E( common, "%s: uncognized result code (%d)\n", __FUNCTION__,
                                                                _g_state.connect_fail_result_code );
            break;
        }

        }

        return false;
    }

    handshake_take( _g_state.handshake_ip_addr_ready  );

    if( ! _g_state.sta_ip_ready )
    {
        LOG_W( wifi, "%s: ip not ready (dhcp not done).\n", __FUNCTION__ );
        return false;
    }

    _g_state.sta_ip_ready_wait = false;

    return true;
}

/****************************************************************************/


static void _mtk_sta_ip_down( struct mtk_wifi_state_t * state )
{
    struct netif *sta_if;

    LOG_I( wifi, "%s %s\n", __FUNCTION__, ( ! state->sta_ip_up ) ? "skip" : "do" );

    if( ! state->sta_ip_up )
        return;

    sta_if = netif_find_by_type( NETIF_TYPE_STA );

    netif_set_status_callback( sta_if, NULL );

    dhcp_release( sta_if );

    dhcp_stop( sta_if );

    netif_set_link_down( sta_if );

    state->sta_ip_up = false;
}

static void _mtk_sta_ip_up( struct mtk_wifi_state_t * state )
{
    struct netif *sta_if;

    LOG_I( wifi, "%s %s\n", __FUNCTION__, ( state->sta_ip_up ) ? "skip" : "do" );

    if( state->sta_ip_up )
        return;

    sta_if = netif_find_by_type( NETIF_TYPE_STA );

    netif_set_default( sta_if );

    netif_set_status_callback( sta_if, _ip_ready_callback );

    dhcp_start( sta_if );

    state->sta_ip_up = true;
}


/****************************************************************************/


static void _mtk_ap_dhcpd_settings_init( const lwip_tcpip_config_t * tcpip_config,
                                           dhcpd_settings_t * dhcpd_settings )
{
    strncpy( dhcpd_settings->dhcpd_server_address,
             ip4addr_ntoa( &tcpip_config->ap_addr ),
             IP4ADDR_STRLEN_MAX );

    strncpy( dhcpd_settings->dhcpd_netmask,
             ip4addr_ntoa( &tcpip_config->ap_mask ),
             IP4ADDR_STRLEN_MAX );

    strncpy( dhcpd_settings->dhcpd_gateway,
             (char *)dhcpd_settings->dhcpd_server_address,
             IP4ADDR_STRLEN_MAX );

    strcpy( ( char * )dhcpd_settings->dhcpd_primary_dns,   PRIMARY_DNS   );
    strcpy( ( char * )dhcpd_settings->dhcpd_secondary_dns, SECONDARY_DNS );
    strcpy( ( char * )dhcpd_settings->dhcpd_ip_pool_start, IP_POOL_START );
    strcpy( ( char * )dhcpd_settings->dhcpd_ip_pool_end,   IP_POOL_END   );
}


/****************************************************************************/


static void _mtk_ap_ip_down( struct mtk_wifi_state_t * state )
{
    struct netif        *ap_if;

    LOG_I( wifi, "%s %s\n", __FUNCTION__, ( ! state->ap_ip_up ) ? "skip" : "do" );

    if( ! state->ap_ip_up )
        return;

    dhcpd_stop();

    ap_if = netif_find_by_type( NETIF_TYPE_AP );
    netif_set_link_down( ap_if );

    state->ap_ip_up = false;
}

static void _mtk_ap_ip_up( struct mtk_wifi_state_t * state )
{
    struct netif        *ap_if;
    dhcpd_settings_t    dhcpd_settings = { {0},{0},{0},{0},{0},{0},{0} };
    lwip_tcpip_config_t tcpip_config;

    LOG_I( wifi, "%s %s\n", __FUNCTION__, ( state->ap_ip_up ) ? "skip" : "do" );

    if( state->ap_ip_up )
        return;

    tcpip_config_init( &tcpip_config );

    _mtk_ap_dhcpd_settings_init( &tcpip_config, &dhcpd_settings );

    ap_if = netif_find_by_type( NETIF_TYPE_AP );

    netif_set_default( ap_if );
    netif_set_link_up( ap_if );

    dhcpd_start( &dhcpd_settings );

    state->ap_ip_up = true;
}


/*-----------------------------------------------------------*/

/**
 * @brief  wifi disconnected will call this callback function. set lwip status in this function
 * @param[in] wifi_event_t event: not used.
 * @param[in] uint8_t *payload: not used.
 * @param[in] uint32_t length: not used.
 * @retval None
 */
static int32_t _wifi_evt_disconnect_cbk(
    wifi_event_t    event,
    uint8_t         *payload,
    uint32_t        length )
{
    uint8_t         opmode;

    wifi_config_get_opmode( &opmode );

    if ( WIFI_MODE_AP_ONLY != opmode )
    {
        uint8_t     link_status = 1;

        // should check link status, it will emit this event when smartphone
        // disconnect with host under repeater mode.

        wifi_connection_get_link_status( &link_status );

        if ( link_status == 0 )
        {
            struct netif *sta_if = netif_find_by_type( NETIF_TYPE_STA );

            netif_set_link_down( sta_if );

            if ( STA_IP_MODE_DHCP == STA_IP_MODE_DHCP )
                netif_set_addr( sta_if, IP4_ADDR_ANY, IP4_ADDR_ANY, IP4_ADDR_ANY );

            LOG_I( common, "%s: disconnected\n", __FUNCTION__ );
        }
    }

    _g_state.connected = false;
    _g_state.sta_ip_ready  = false;

    handshake_give( _g_state.handshake_wifi_disconnected );

    return 1;
}

/*-----------------------------------------------------------*/

static int32_t _wifi_evt_init_done_cbk( wifi_event_t event,
                                           uint8_t * payload,
                                            uint32_t length )
{
    switch( event )
    {
    case WIFI_EVENT_IOT_INIT_COMPLETE:

        LOG_I( wifi, "%s: init complete\n", __FUNCTION__ );

        handshake_give( _g_state.handshake_init_complete );
        break;

    default:

        LOG_I( wifi, "%s: event %u\n", __FUNCTION__, event );
        break;
    }

    return 0;
}

/*-----------------------------------------------------------*/

/**
 * Handles the Wi-Fi connection failure event.
 */
static int32_t _wifi_evt_portfail_cbk( wifi_event_t event,
                                          uint8_t * payload,
                                           uint32_t length )
{
    uint16_t reason_code;

    (void)event;

    reason_code = *(payload + 1) + ((*payload) << 8);

    switch (reason_code)
    {

    case WIFI_REASON_CODE_FIND_AP_FAIL:
    {
        _g_state.ssid_not_found++;

        if( ( _g_state.ssid_not_found >= MAX_SSID_SEARCH_COUNT ) &&
            ( _g_state.sta_ip_ready_wait ) )
        {
            _g_state.ssid_not_found           = 0;
            _g_state.connected                = false;
            _g_state.sta_ip_ready             = false;
            _g_state.connect_fail_result_code = WIFI_REASON_CODE_FIND_AP_FAIL;

            LOG_W( common, "%s: connection fail: %s\n", __FUNCTION__,
                                                        "WIFI_REASON_CODE_FIND_AP_FAIL" );

            handshake_give( _g_state.handshake_wifi_connected );
        }

        break;
    }

    case WIFI_REASON_CODE_PREV_AUTH_INVALID:
    {
        LOG_W( common, "%s: connection fail: %s\n", __FUNCTION__,
                                                    "WIFI_REASON_CODE_PREV_AUTH_INVALID" );

        if( _g_state.sta_ip_ready_wait )
        {
            _g_state.ssid_not_found           = 0;
            _g_state.connected                = false;
            _g_state.sta_ip_ready             = false;
            _g_state.connect_fail_result_code = WIFI_REASON_CODE_PREV_AUTH_INVALID;

            handshake_give( _g_state.handshake_wifi_connected );
        }
        break;
    }

    case WIFI_REASON_CODE_PASSWORD_ERROR:
    {
        LOG_W( common, "%s: connection fail: %s\n", __FUNCTION__,
                                                    "WIFI_REASON_CODE_PASSWORD_ERROR" );

        if( _g_state.sta_ip_ready_wait )
        {
            _g_state.ssid_not_found           = 0;
            _g_state.connected                = false;
            _g_state.sta_ip_ready             = false;
            _g_state.connect_fail_result_code = WIFI_REASON_CODE_PASSWORD_ERROR;

            handshake_give( _g_state.handshake_ip_addr_ready );
        }
        break;
    }

    default:
    {
        LOG_I( common, "%s: ignore reason code %d\n", __FUNCTION__, reason_code );
        break;
    }

    } /* switch( reason_code ) */

    return 0;
}

/*-----------------------------------------------------------*/

/**
 * Handles the Wi-Fi connection establishment event.
 *
 * When a Wi-Fi connection is established, this callback function is invoked
 * to set network interface status to Link-Up. If the IP address mode is
 * static, signaling the IP address is ready. If the IP address mode is
 * dynamic (DHCP), let the DHCP to notify the IP address is ready.
 *
 * @param event     not used.
 * @param payload   not used.
 * @param length    not used.
 *
 * @return 0
 */
static int32_t _wifi_evt_port_secure_cbk( wifi_event_t event,
                                             uint8_t * payload,
                                              uint32_t length )
{
    struct netif *sta_if;

    sta_if = netif_find_by_type( NETIF_TYPE_STA );

    netif_set_link_up( sta_if );

    switch( event )
    {
    case WIFI_EVENT_IOT_CONNECTED:
    {
        _g_state.ssid_not_found  = 0;
        LOG_I( common, "%s: connected\n", __FUNCTION__ );
        break;
    }

    case WIFI_EVENT_IOT_PORT_SECURE:
    {
        _g_state.ssid_not_found  = 0;
        LOG_I( common, "%s: port secured\n", __FUNCTION__ );
        break;
    }

    default:
        LOG_I( common, "%s: unknown event %u\n", __FUNCTION__,
                                                 ( unsigned int )event );
        break;
    }

    if ( STA_IP_MODE_STATIC == STA_IP_MODE_STATIC ) {
       /*
        * This is a private API, used to inform IP ready to Wi-Fi driver
        * In present, Wi-Fi Driver will do some operation when this API is
        * invoked, such as:
        *
        *   Do Wi-Fi & BLE coexstence relative behavior if BLE is enabled and
        *   do Power Saving Status change.
        *
        * This API will be improved, user may need to use new API to replace
        * it in future
        */
        wifi_connection_inform_ip_ready();
    }

    LOG_I( common, "%s: wifi connected\n", __FUNCTION__ );

    _g_state.connected = true;

    handshake_give( _g_state.handshake_wifi_connected );

    return 0;
}

/*-----------------------------------------------------------*/

static bool _mtk_wifi_mode_switch_dup( uint8_t op_mode )
{
    uint8_t     mode;

    if( wifi_config_get_opmode( &mode ) < 0 )
    {
        LOG_E( wifi, "%s: get op_mode fail\n", __FUNCTION__ );
        return false;
    }

    return mode == op_mode;
}

/*-----------------------------------------------------------*/

static bool _mtk_wifi_mode_switch_wrapper( uint8_t op_mode )
{
    if( _mtk_wifi_mode_switch_dup( op_mode ) )
        return true;

    if( wifi_config_set_opmode( op_mode ) < 0 )
    {
        LOG_E( wifi, "%s: set op_mode fail\n", __FUNCTION__ );
        return false;
    }

    return true;
}

/*-----------------------------------------------------------*/

static WIFISecurity_t _mtk_security_to_aws_security( wifi_auth_mode_t    security,
                                                     wifi_encrypt_type_t encrypt_type )
{
    switch( security )
    {
    case WIFI_AUTH_MODE_OPEN:

        if( WIFI_ENCRYPT_TYPE_WEP_ENABLED == encrypt_type )
            return eWiFiSecurityWEP;
        else
            return eWiFiSecurityOpen;

    case WIFI_AUTH_MODE_WPA_PSK:                         /**< WPA_PSK.       */

        return eWiFiSecurityWPA;

    case WIFI_AUTH_MODE_WPA2_PSK:                        /**< WPA2_PSK.      */
    case WIFI_AUTH_MODE_WPA_PSK_WPA2_PSK:                /**< Mixture mode.  */

        return eWiFiSecurityWPA2;

    case WIFI_AUTH_MODE_SHARED:                          /**< Not supported. */
    case WIFI_AUTH_MODE_AUTO_WEP:                        /**< Not supported. */
    case WIFI_AUTH_MODE_WPA:                             /**< Not supported. */
    case WIFI_AUTH_MODE_WPA_None:                        /**< Not supported. */
    case WIFI_AUTH_MODE_WPA2:                            /**< Not supported. */
    case WIFI_AUTH_MODE_WPA_WPA2:                        /**< Not supported. */

        return eWiFiSecurityNotSupported;
    }

    return eWiFiSecurityNotSupported;
}

/*-----------------------------------------------------------*/

static wifi_auth_mode_t _aws_security_to_mtk_auth( WIFISecurity_t security )
{
    switch( security )
    {
    case eWiFiSecurityOpen:
    case eWiFiSecurityWEP:
        return WIFI_AUTH_MODE_OPEN;

    case eWiFiSecurityWPA:
        return WIFI_AUTH_MODE_WPA_PSK;

    case eWiFiSecurityWPA2:
        return WIFI_AUTH_MODE_WPA2_PSK;

    case eWiFiSecurityNotSupported:
    default:
        break;
    }

#ifdef configASSERT
    configASSERT( 0 );
#else
    return 0; /* should be non-reachable */
#endif
}

/*-----------------------------------------------------------*/

static wifi_encrypt_type_t _aws_security_to_mtk_encrypt( WIFISecurity_t security )
{
    switch( security )
    {
    case eWiFiSecurityOpen:
        return WIFI_ENCRYPT_TYPE_WEP_DISABLED;

    case eWiFiSecurityWEP:
        return WIFI_ENCRYPT_TYPE_WEP_ENABLED;

    case eWiFiSecurityWPA:
    case eWiFiSecurityWPA2:
        return WIFI_ENCRYPT_TYPE_TKIP_AES_MIX;

    case eWiFiSecurityNotSupported:
        break;
    }

#ifdef configASSERT
    configASSERT( 0 );
#else
    return 0; /* should be non-reachable */
#endif
}

/*-----------------------------------------------------------*/

static bool _mtk_wifi_apply_setting( uint8_t port,
           const WIFINetworkParams_t * const pxNetworkParams )
{
    wifi_auth_mode_t    mtk_auth;
    wifi_encrypt_type_t mtk_ciph;

    /* check ssid */

    if( pxNetworkParams->ucSSIDLength == 0 ||
        pxNetworkParams->ucSSIDLength > ( wificonfigMAX_SSID_LEN + 1 ) )
    {
        return false;
    }

    /* check security */
    if( eWiFiSecurityOpen != pxNetworkParams->xSecurity )
    {
        if( pxNetworkParams->xSecurity != eWiFiSecurityWEP &&
            pxNetworkParams->xSecurity != eWiFiSecurityWPA &&
            pxNetworkParams->xSecurity != eWiFiSecurityWPA2 )
				{
            return false;
				}
        if( pxNetworkParams->xPassword.xWPA.ucLength == 0
            || pxNetworkParams->xPassword.xWPA.ucLength > wificonfigMAX_PASSPHRASE_LEN )
				{
						return false;
				}
    }

    mtk_auth = _aws_security_to_mtk_auth   ( pxNetworkParams->xSecurity );
    mtk_ciph = _aws_security_to_mtk_encrypt( pxNetworkParams->xSecurity );

    if( port == WIFI_PORT_AP && wifi_config_set_channel( port, pxNetworkParams->ucChannel ) < 0 )
    {
        LOG_E( wifi, "%s: set CHANNEL fail\n", __FUNCTION__ );
        return false;
    }

    if( wifi_config_set_ssid( port,
                              (uint8_t *) pxNetworkParams->ucSSID,
                              (uint8_t) pxNetworkParams->ucSSIDLength ) < 0 )
    {
        LOG_E( wifi, "%s: set SSID fail\n", __FUNCTION__ );
        return false;
    }

    if( wifi_config_set_security_mode( port, mtk_auth, mtk_ciph ) < 0 )
    {
        LOG_E( wifi, "%s: set SECURITY MODE fail\n", __FUNCTION__ );
        return false;
    }

    if( WIFI_ENCRYPT_TYPE_WEP_ENABLED  == mtk_ciph ||
        WIFI_ENCRYPT_TYPE_WEP_DISABLED == mtk_ciph )
    {
        wifi_wep_key_t  wep_keys;

        memset( &wep_keys, 0, sizeof( wep_keys ) );

        if( WIFI_ENCRYPT_TYPE_WEP_ENABLED == mtk_ciph &&
            0 != pxNetworkParams->xPassword.xWPA.ucLength )
        {
            memcpy( &wep_keys.wep_key[ 0 ],
                    pxNetworkParams->xPassword.xWPA.cPassphrase,
                    pxNetworkParams->xPassword.xWPA.ucLength );

            wep_keys.wep_key_length[ 0 ] = pxNetworkParams->xPassword.xWPA.ucLength;
            wep_keys.wep_tx_key_index    = 0;
        }

        if( wifi_config_set_wep_key( port, &wep_keys ) < 0 )
        {
            LOG_E( wifi, "%s: set WEP KEY fail\n", __FUNCTION__ );
            return false;
        }
    }
    else if( WIFI_ENCRYPT_TYPE_TKIP_AES_MIX == mtk_ciph )
    {
        if( wifi_config_set_wpa_psk_key( port,
                                         (uint8_t *)pxNetworkParams->xPassword.xWPA.cPassphrase,
                                         (uint8_t)pxNetworkParams->xPassword.xWPA.ucLength ) < 0 )
        {
            LOG_E( wifi, "%s: set WPA/WPA2 PSK KEY fail\n", __FUNCTION__ );
            return false;
        }
    }

    return true;
}

/*-----------------------------------------------------------*/

static WIFIReturnCode_t _mtk_wifi_register_callbacks( void )
{
    struct _entry
    {
        wifi_event_t    event;
        int32_t         (*callback)( wifi_event_t, uint8_t *, uint32_t );
    };

    static const struct _entry table[] =
    {
        { WIFI_EVENT_IOT_INIT_COMPLETE,     _wifi_evt_init_done_cbk   },
        { WIFI_EVENT_IOT_CONNECTED,         _wifi_evt_port_secure_cbk },
        { WIFI_EVENT_IOT_PORT_SECURE,       _wifi_evt_port_secure_cbk },
        { WIFI_EVENT_IOT_DISCONNECTED,      _wifi_evt_disconnect_cbk  },
        { WIFI_EVENT_IOT_CONNECTION_FAILED, _wifi_evt_portfail_cbk    }
    };

    const struct _entry *entry;

    for( entry = &table[ 0 ];
         entry < &table[ sizeof(table) / sizeof(struct _entry) ];
         entry ++ )
    {
        if( wifi_connection_register_event_handler( entry->event,
                                                    entry->callback ) == 0 )
        {
            continue;
        }

        LOG_E( wifi, "WIFI_On: register %u fail\n",
                     ( unsigned int )entry->event );

        return eWiFiFailure;
    }

    return eWiFiSuccess;
}

/*-----------------------------------------------------------*/

static WIFIReturnCode_t _mtk_wifi_bootstrap( void )
{
    wifi_config_t       config      = { 0 };
    wifi_config_ext_t   config_ext  = { 0 };

    memset( &config, 0, sizeof( config ) );

    config.opmode                           = WIFI_MODE_MONITOR;

    config.sta_config.ssid_length           = 0;
    config.sta_config.bssid_present         = 0;
    config.sta_config.password_length       = 64;

    memset( &config_ext, 0, sizeof( config_ext ) );

    config_ext.sta_wep_key_index_present    = 0;
    config_ext.sta_wep_key_index            = 0;
    config_ext.sta_auto_connect_present     = 1;
		config_ext.sta_auto_connect             = 0;

    config_ext.ap_wep_key_index_present     = 0;
    config_ext.ap_wep_key_index             = 0;
    config_ext.ap_hidden_ssid_enable_present= 0;
    config_ext.ap_hidden_ssid_enable        = 0;

    LOG_W( wifi, "WIFI_On: init ip stack\n" );

    _g_state.handshake_init_complete = handshake_create();
		
		_g_state.ucSSIDLength = 0;
		memset( _g_state.ucSSID, 0u, sizeof( _g_state.ucSSID ) );
		_g_state.xSecurity = eWiFiSecurityOpen;

    wifi_init( &config, &config_ext );

    LOG_W( wifi, "WIFI_On: wifi_init done\n" );

    if( _mtk_wifi_register_callbacks() )
    {
        return eWiFiFailure;
    }

    _ip_init ( WIFI_MODE_STA_ONLY );

    handshake_take_and_free( _g_state.handshake_init_complete );
    _g_state.handshake_init_complete = NULL;

    _g_state.critical_section = xSemaphoreCreateBinary();
    xSemaphoreGive( _g_state.critical_section );

    return eWiFiSuccess;
}

/*-----------------------------------------------------------*/

WIFIReturnCode_t WIFI_On( void )
{
    if( ! _g_state.fw_ready )
    {
        if ( _mtk_wifi_bootstrap() != eWiFiSuccess )
        {
            return eWiFiFailure;
        }

        _g_state.fw_ready = true;
    }
    else if( wifi_config_set_radio( 1 ) < 0 )
    {
        LOG_E( wifi, "%s: turn on fail\n", __FUNCTION__ );
        return eWiFiFailure;
    }

    _g_state.on = true;

    return eWiFiSuccess;
}

/*-----------------------------------------------------------*/

WIFIReturnCode_t WIFI_Off( void )
{
    xSemaphoreTake( _g_state.critical_section, portMAX_DELAY );

    if( ! _mtk_wifi_mode_switch_wrapper( WIFI_MODE_STA_ONLY ) )
    {
        LOG_E( wifi, "%s: mode switch fail!\n", __FUNCTION__ );
        xSemaphoreGive( _g_state.critical_section );
        return eWiFiFailure;
    }

    xSemaphoreGive( _g_state.critical_section );

    if( wifi_config_set_radio( 0 ) < 0 )
    {
        LOG_E( wifi, "%s: turn off fail\n", __FUNCTION__ );
        return eWiFiFailure;
    }
    else
    {
         _g_state.on = false;
    }

    return eWiFiSuccess;
}

/*-----------------------------------------------------------*/

WIFIReturnCode_t WIFI_ConnectAP( const WIFINetworkParams_t * const pxNetworkParams )
{
    LOG_I( wifi, "%s: connecting\n", __FUNCTION__ );

    if( NULL == pxNetworkParams )
    {
        return eWiFiFailure;
    }

    if ( pxNetworkParams->xSecurity >= eWiFiSecurityNotSupported)
    {
        return eWiFiFailure;
    }

    if( pxNetworkParams->xPassword.xWPA.ucLength == 0 && eWiFiSecurityOpen != pxNetworkParams->xSecurity )
    {
        return eWiFiFailure;
    }

    if ( pxNetworkParams->ucSSIDLength == 0 
				 || pxNetworkParams->ucSSIDLength > wificonfigMAX_SSID_LEN )
    {
        return eWiFiFailure;
    }

    if ( pxNetworkParams->xPassword.xWPA.ucLength == 0 
				 ||	pxNetworkParams->xPassword.xWPA.ucLength > wificonfigMAX_PASSPHRASE_LEN )
    {
        return eWiFiFailure;
    }

    xSemaphoreTake( _g_state.critical_section, portMAX_DELAY );

    _mtk_ap_ip_down ( &_g_state );
    _mtk_sta_ip_down( &_g_state );

    if( ! _mtk_wifi_mode_switch_wrapper( WIFI_MODE_STA_ONLY ) )
    {
        LOG_E( wifi, "%s: mode switch fail!\n", __FUNCTION__ );

        xSemaphoreGive( _g_state.critical_section );

        return eWiFiFailure;
    }

    if( ! _mtk_wifi_apply_setting( WIFI_PORT_STA, pxNetworkParams ) )
    {
        LOG_E( wifi, "%s: apply setting fail!\n", __FUNCTION__ );

        xSemaphoreGive( _g_state.critical_section );

        return eWiFiFailure;
    }

    handshake_clear( _g_state.handshake_wifi_connected );

    if( wifi_config_reload_setting() < 0 )
    {
        LOG_E( wifi, "%s: reload setting fail!\n", __FUNCTION__ );

        xSemaphoreGive( _g_state.critical_section );

        return eWiFiFailure;
    }

    LOG_I( wifi, "%s: wait IP ready\n", __FUNCTION__ );

    _mtk_sta_ip_up( &_g_state );

    if (_mtk_sta_ip_is_ready() == false )
    {
        LOG_E( wifi, "%s: IP not ready\n", __FUNCTION__ );
        xSemaphoreGive( _g_state.critical_section );
        return eWiFiFailure;
    }

		/* Update internal data */
    memcpy( _g_state.ucSSID, pxNetworkParams->ucSSID, wificonfigMAX_SSID_LEN);
    _g_state.ucSSIDLength = pxNetworkParams->ucSSIDLength;
    _g_state.xSecurity = pxNetworkParams->xSecurity;
		
    LOG_I( wifi, "%s: IP ready\n", __FUNCTION__ );

    xSemaphoreGive( _g_state.critical_section );

    return eWiFiSuccess;
}

/*-----------------------------------------------------------*/

WIFIReturnCode_t WIFI_Disconnect( void )
{
    uint8_t link_status;

    LOG_I( wifi, "%s: disconnect\n", __FUNCTION__ );

    xSemaphoreTake( _g_state.critical_section, portMAX_DELAY );

    /*
     * early exit if already disconnected
     */
    if( _g_state.connected == false )
    {
        LOG_I( wifi, "%s: Already disconnected.\n", __FUNCTION__ );
        xSemaphoreGive( _g_state.critical_section );
        return eWiFiSuccess;
    }
    else
    {
        LOG_I( wifi, "%s: Connected. Proceeding to disconnect.\n", __FUNCTION__ );
    }

    /*
     * stop ip stack
     */

    _mtk_sta_ip_down( &_g_state );

    /*
     * disconnect AP
     */

    handshake_clear( _g_state.handshake_wifi_disconnected );

    if( wifi_connection_disconnect_ap() < 0 )
    {
        LOG_E( wifi, "%s: disconnect fail\n", __FUNCTION__ );

        xSemaphoreGive( _g_state.critical_section );

        return eWiFiFailure;
    }

    handshake_take( _g_state.handshake_wifi_disconnected );

    xSemaphoreGive( _g_state.critical_section );

    return eWiFiSuccess;
}

/*-----------------------------------------------------------*/

WIFIReturnCode_t WIFI_Reset( void )
{
    _mtk_sta_ip_down( &_g_state );
    _mtk_ap_ip_down ( &_g_state );

    if( WIFI_Off() == eWiFiFailure )
    {
        LOG_E( wifi, "Turn off Wi-Fi failed!\n" );
        return eWiFiFailure;
    }

    if( WIFI_On() == eWiFiFailure )
    {
        LOG_E( wifi, "Turn on Wi-Fi failed!\n" );
        return eWiFiFailure;
    }

    return eWiFiSuccess;
}

/*-----------------------------------------------------------*/

WIFIReturnCode_t WIFI_SetMode( WIFIDeviceMode_t xDeviceMode )
{
    WIFIReturnCode_t    status = eWiFiSuccess;
    uint8_t             new_op_mode = WIFI_MODE_STA_ONLY;

    switch( xDeviceMode )
    {
    case eWiFiModeStation:  new_op_mode = WIFI_MODE_STA_ONLY; break;
    case eWiFiModeAP:       new_op_mode = WIFI_MODE_AP_ONLY;  break;
    case eWiFiModeP2P:      new_op_mode = WIFI_MODE_P2P_ONLY; break;
    default:
        return eWiFiFailure;
    }

    LOG_I( wifi, "%s: switch mode to %s\n", __FUNCTION__,
                 new_op_mode == WIFI_MODE_STA_ONLY ? "STATION" :
                 new_op_mode == WIFI_MODE_AP_ONLY  ? "AP" : "P2P" );

    _mtk_sta_ip_down( &_g_state );
    _mtk_ap_ip_down ( &_g_state );

    if( ! _mtk_wifi_mode_switch_wrapper( new_op_mode ) )
    {
        LOG_E( wifi, "%s: switch mode fail\n", __FUNCTION__ );
        return eWiFiFailure;
    }

    if( new_op_mode == WIFI_MODE_STA_ONLY ||
        new_op_mode == WIFI_MODE_P2P_ONLY )
    {
        _mtk_sta_ip_up( &_g_state );
    }
    else if( new_op_mode == WIFI_MODE_AP_ONLY )
    {
        _mtk_ap_ip_up( &_g_state );
    }

    return status;
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
    int         count;
    ip_addr_t   ping_addr;

    if( NULL == pucIPAddr )
    {
        return eWiFiFailure;
    }

#if LWIP_IPV4 && LWIP_IPV6
    ping_addr.type = IPADDR_TYPE_V4;
    ping_addr.u_addr.ip4.addr = ( ( pucIPAddr[ 3 ] & 0xFF ) << 24 ) |
                                ( ( pucIPAddr[ 2 ] & 0xFF ) << 16 ) |
                                ( ( pucIPAddr[ 1 ] & 0xFF ) << 8  ) |
                                ( ( pucIPAddr[ 0 ] & 0xFF )       ) ;

    printf("ping %d.%d.%d.%d\n", pucIPAddr[ 3 ] & 0xFF,
                                 pucIPAddr[ 2 ] & 0xFF,
                                 pucIPAddr[ 1 ] & 0xFF,
                                 pucIPAddr[ 0 ] & 0xFF );
#elif LWIP_IPV4
    ping_addr.addr = ( ( pucIPAddr[ 3 ] & 0xFF ) << 24 ) |
                     ( ( pucIPAddr[ 2 ] & 0xFF ) << 16 ) |
                     ( ( pucIPAddr[ 1 ] & 0xFF ) << 8  ) |
                     ( ( pucIPAddr[ 0 ] & 0xFF )       ) ;

    printf("ping %d.%d.%d.%d\n", pucIPAddr[ 0 ] & 0xFF,
                                 pucIPAddr[ 1 ] & 0xFF,
                                 pucIPAddr[ 2 ] & 0xFF,
                                 pucIPAddr[ 3 ] & 0xFF );
#else
    /* not supported */
    memset( &ping_addr, 0, sizeof( ping_addr ) );
#endif

    // const ip_addr_t* ping_addr
    count = ping_custom_sync(&ping_addr,
                             (uint32_t)usCount,
                             ulIntervalMS);

    if( count <= 0 )
        return eWiFiFailure;

    if( count != usCount )
    {
        printf("WIFI_Ping: only %d pings reached out of %d\n", count, usCount);
    }

    return eWiFiSuccess;
}

/*-----------------------------------------------------------*/

/**
 * Return a MAC address based on current operation mode.
 *
 * @param pxMac a pointer to write MAC address (6-bytes needed).
 * @retval eWiFiFailure if something went wrong.
 * @retval eWiFiSuccess succeed, MAC address in pxMac.
 *
 * @note MT7697 supports two interfaces: an AP and a station. And they shall
 *       have different MAC addresses.
 */
WIFIReturnCode_t WIFI_GetMAC( uint8_t * pxMac )
{
    uint8_t opmode;

    if( NULL == pxMac )
    {
        return eWiFiFailure;
    }

    if( wifi_config_get_opmode( &opmode ) < 0 )
    {
        return eWiFiFailure;
    }

    if( WIFI_MODE_AP_ONLY == opmode )
    {
        if( wifi_config_get_mac_address( WIFI_PORT_AP, pxMac ) < 0 )
        {
            return eWiFiFailure;
        }
    }
    else
    {
        if( wifi_config_get_mac_address( WIFI_PORT_STA, pxMac ) < 0 )
        {
            return eWiFiFailure;
        }
    }

    return eWiFiSuccess;
}

/*-----------------------------------------------------------*/

/**
 * Resolve IP address from hostname.
 */
WIFIReturnCode_t WIFI_GetHostIP( char * pxHost,
                              uint8_t * pxIPAddr )
{
    int                 s;
    struct addrinfo     hints;
    struct addrinfo     *result;
    struct sockaddr_in  *addr_in;

    if( NULL == pxHost || NULL == pxIPAddr )
    {
        return eWiFiFailure;
    }

    memset(&hints, 0, sizeof(hints));
    hints.ai_family     = AF_INET;
    hints.ai_socktype   = SOCK_DGRAM;
    hints.ai_flags      = 0;
    hints.ai_protocol   = 0;

    s = getaddrinfo( pxHost, NULL, &hints, &result );
    if( s != 0 )
    {
        return eWiFiFailure;
    }

    if( result )
    {
        /*
         * ai_addr is of struct sockaddr type.
         *
            struct sockaddr {
              u8_t        sa_len;
              sa_family_t sa_family;
              char        sa_data[14];
            };
         */

        addr_in = (struct sockaddr_in *)result->ai_addr;

        memcpy( pxIPAddr, &addr_in->sin_addr.s_addr,
                sizeof( addr_in->sin_addr.s_addr ) );
    }
    else
    {
        return eWiFiFailure;
    }

    freeaddrinfo( result );

    return eWiFiSuccess;
}

/*-----------------------------------------------------------*/

static int32_t _aws_wifi_scan_complete_handler( wifi_event_t event,
                                                   uint8_t * payload,
                                                    uint32_t length )
{
    configASSERT( event == WIFI_EVENT_IOT_SCAN_COMPLETE );
    handshake_give( _g_state.handshake_scan_complete );

    return 0; /* return value is ignored by Wi-Fi stack */
}

/*-----------------------------------------------------------*/

WIFIReturnCode_t WIFI_Scan( WIFIScanResult_t * pxBuffer,
                                       uint8_t uxNumNetworks )
{
    wifi_scan_list_item_t * pxApList;
    uint8_t                 ssid[ WIFI_MAX_LENGTH_OF_SSID ];
    uint8_t                 ssidLen;
    int                     i, j;

    if( NULL == pxBuffer || 0 == uxNumNetworks )
    {
        return eWiFiFailure;
    }

    if( wifi_config_get_ssid( WIFI_PORT_STA, &ssid[ 0 ], &ssidLen ) < 0 )
    {
        return eWiFiFailure;
    }

    pxApList = pvPortMalloc( sizeof( *pxApList ) * uxNumNetworks );
    if( NULL == pxApList )
    {
        return eWiFiFailure;
    }

    /* register scan complete event and enable it */
    if( wifi_api_set_event_handler( 1,
                                    WIFI_EVENT_IOT_SCAN_COMPLETE,
                                    _aws_wifi_scan_complete_handler ) < 0 )
    {
        // already registered by someone else
        return eWiFiFailure;
    }

    if( wifi_connection_scan_init( pxApList, ( uint32_t )uxNumNetworks ) < 0 )
    {
        vPortFree( pxApList );
        return eWiFiFailure;
    }

    _g_state.handshake_scan_complete = handshake_create();

    if( wifi_connection_start_scan( ssid,
                                    ssidLen,
                                    NULL,           /* not specify BSSID */
                                    0,              /* full scan */
                                    0 ) )           /* active scan */
    {
        vPortFree( pxApList );

        handshake_free( _g_state.handshake_scan_complete );

        configASSERT( wifi_connection_scan_deinit() >= 0 );

        return eWiFiFailure;
    }

    handshake_take_and_free( _g_state.handshake_scan_complete );

    wifi_connection_stop_scan();

    configASSERT( wifi_connection_scan_deinit() >= 0 );

    if( wifi_api_set_event_handler( 0,
                                    WIFI_EVENT_IOT_SCAN_COMPLETE,
                                    _aws_wifi_scan_complete_handler ) < 0 )
    {
#ifdef configASSERT
        configASSERT( 0 );
#else
        return eWiFiFailure;
#endif
    }

    for( j = 0, i = 0; i < uxNumNetworks; i++ )
    {
        if( pxApList[ i ].is_valid ) {
            /* SSID */
            memcpy( &pxBuffer[ j ].ucSSID[ 0 ],
                    &pxApList[ i ].ssid [ 0 ],
                    wificonfigMAX_SSID_LEN > WIFI_MAX_LENGTH_OF_SSID ? WIFI_MAX_LENGTH_OF_SSID : wificonfigMAX_SSID_LEN );
            /* BSSID */
            memcpy( &pxBuffer[ j ].ucBSSID[ 0 ],
                    &pxApList[ i ].bssid  [ 0 ],
                    wificonfigMAX_BSSID_LEN );

            /* security mode */
            pxBuffer[ j ].xSecurity = _mtk_security_to_aws_security( pxApList[ i ].auth_mode,
                                                                     pxApList[ i ].encrypt_type );

            /* RSSI */
            pxBuffer[ j ].cRSSI = pxApList[ i ].rssi;

            /* channel */
            pxBuffer[ j ].ucChannel = pxApList[ i ].channel;

            j++;
        }
    }

    if( j < uxNumNetworks )
    {
        memset( &pxBuffer[ j ], 0, sizeof( WIFIScanResult_t ) * ( uxNumNetworks - j ) );
    }

    vPortFree( pxApList );

    return eWiFiSuccess;
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

BaseType_t WIFI_IsConnected( const WIFINetworkParams_t * pxNetworkParams )
{
		return _g_state.sta_ip_ready == true;

}


BaseType_t WIFI_HAL_IsConnected( const WIFINetworkParams_t * pxNetworkParams )
{
    // wifi_connection_get_link_status() reports TRUE
    // sometimes when device is not connected, for example
    // it's doing connecting.
    if ( !_g_state.sta_ip_ready )
    {
        return pdFALSE;
    }

    // Check if parameters match the currently connected AP
    // Matching is limited to SSID and Security only
    if (pxNetworkParams != NULL)
    {
        if (pxNetworkParams->ucSSIDLength != _g_state.ucSSIDLength ||
            memcmp(pxNetworkParams->ucSSID, _g_state.ucSSID, pxNetworkParams->ucSSIDLength))
        {
            return pdFALSE;
        }

        if ( _g_state.xSecurity != pxNetworkParams->xSecurity)
        {
            return pdFALSE;
        }
    }
		
    return pdTRUE;
}

//
// Internal data structures
//
static WIFIScanResult_t* wifiScanResults_;
static uint16_t wifiNumScanResults_;
static WIFINetworkParams_t wifiAPConfig_;

//
// Internal semaphore for higher-layer synchronous calls
//
static SemaphoreHandle_t wifiSemaphore_;
static StaticSemaphore_t wifiSemaphoreBuffer_;
static const TickType_t wifiSemaphoreWaitTicks_ = pdMS_TO_TICKS(wificonfigMAX_SEMAPHORE_WAIT_TIME_MS);

//
// Internal event handlers
//
static WIFIEventHandler_t wifiEventHandlers_[ eWiFiEventMax ];
static bool wifiInitDone_ = false;

//
// Event group for synchronization with lower-layer events
//
#define WIFI_EVENT_INIT_DONE         (1 << 0)
#define WIFI_EVENT_SCAN_DONE         (1 << 1)
#define WIFI_EVENT_CONNECTED         (1 << 2)
#define WIFI_EVENT_DISCONNECTED      (1 << 3)
#define WIFI_EVENT_AP_STATE_CHANGED  (1 << 4)
#define WIFI_EVENT_IP_READY          (1 << 5)
#define WIFI_EVENT_CONNECTION_FAILED (1 << 6)

static uint32_t gSsidNotFound;
static EventGroupHandle_t wifiEvent_;
static StaticEventGroup_t wifiEventBuffer_;
static const TickType_t wifiEventWaitTicks_ = pdMS_TO_TICKS(wificonfigMAX_EVENT_WAIT_TIME_MS);

static void WIFI_ShowScanResults( void )
{
    WIFIScanResult_t* result = wifiScanResults_;
    char ssid[33];
    uint16_t i;

    for (i = 0; i < wifiNumScanResults_; i++, result++)
    {
        if (result->ucSSIDLength > 32)
        {
            LOG_I(wifi, "WIFI ShowScanResults invalid SSID length %u\n", result->ucSSIDLength);
        }
        else
        {
            memset(ssid, 0, sizeof(ssid));
            memcpy(ssid, result->ucSSID, result->ucSSIDLength);
            LOG_I(wifi, "[%2d]: Channel=%2u, BSSID=%02X:%02X:%02X:%02X:%02X:%02X, "
                  "RSSI=%d, Security=%u, Len=%2u, SSID=%s\n",
                  i,
                  result->ucChannel,
                  result->ucBSSID[0],
                  result->ucBSSID[1],
                  result->ucBSSID[2],
                  result->ucBSSID[3],
                  result->ucBSSID[4],
                  result->ucBSSID[5],
                  result->cRSSI,
                  result->xSecurity,
                  result->ucSSIDLength,
                  ssid);
        }
    }
}

static void WIFI_InitEvent( void )
{
    wifiEvent_ = xEventGroupCreateStatic(&wifiEventBuffer_);
    configASSERT(wifiEvent_);
}

static void WIFI_ClearEvent( const EventBits_t bits )
{
    xEventGroupClearBits(wifiEvent_, bits);
}

static void WIFI_PostEvent( const EventBits_t bits )
{
    // Signal that event bits have occured
    xEventGroupSetBits(wifiEvent_, bits);
}

static bool WIFI_WaitEvent( const EventBits_t bits )
{
    EventBits_t waitBits;

    // Wait for any bit in bits. No auto-clear---user must call WIFI_ClearEvent() first!
    waitBits = xEventGroupWaitBits(wifiEvent_, bits, pdFALSE, pdFALSE, wifiEventWaitTicks_);
    return (waitBits & bits) != 0;
}

static inline void WIFI_InitSemaphore( void )
{
    wifiSemaphore_ = xSemaphoreCreateBinaryStatic(&wifiSemaphoreBuffer_);
    configASSERT(wifiSemaphore_);

    // Make semaphore available by default
    xSemaphoreGive(wifiSemaphore_);
}

static inline bool WIFI_TakeSemaphore( void )
{
    return xSemaphoreTake(wifiSemaphore_, wifiSemaphoreWaitTicks_) == pdTRUE;
}

static inline WIFIReturnCode_t WIFI_GiveSemaphore( WIFIReturnCode_t retCode )
{
    if ( xSemaphoreGive( wifiSemaphore_ ) != pdTRUE )
    {
        retCode = eWiFiFailure;
    }

    return retCode;
}

//
// Internal helper functions
//
static WIFIReturnCode_t WIFI_ValidateNetworkParams( const WIFINetworkParams_t * pxNetworkParams )
{
    LOG_I(wifi, "%s\n", __FUNCTION__);
    uint8_t length;
    uint8_t i;

    if( NULL == pxNetworkParams )
    {
        return eWiFiFailure;
    }

    if (pxNetworkParams->ucSSIDLength > wificonfigMAX_SSID_LEN)
    {
        LOG_E(wifi, "WIFI ssid length %u invalid\n", pxNetworkParams->ucSSIDLength);
        return eWiFiFailure;
    }

    switch (pxNetworkParams->xSecurity)
    {
        case eWiFiSecurityOpen:
            break;
        case eWiFiSecurityWPA:
        case eWiFiSecurityWPA2:
            length = pxNetworkParams->xPassword.xWPA.ucLength;
            if (length < wificonfigMIN_PASSPHRASE_LEN || length > wificonfigMAX_PASSPHRASE_LEN)
            {
                LOG_E(wifi, "WIFI psk length %u invalid\n", length);
                return eWiFiFailure;
            }
            break;
        case eWiFiSecurityWEP:
            for (i = 0; i < wificonfigMAX_WEPKEYS; i++)
            {
                // Check that WEP keys have one of the valid lengths. Not all WEP keys are required,
                // so check length only if the key is used (i.e. has the index set)
                length = pxNetworkParams->xPassword.xWEP[i].ucLength;
                if (!(length == 5 || length == 10 || length == 13 || length == 26))
                {
                    LOG_E(wifi, "WIFI wep key %u length %u invalid\n", i, length);
                    if (i == pxNetworkParams->ucDefaultWEPKeyIndex)
                    {
                        return eWiFiFailure;
                    }
                }
            }
            break;
        default:
            return eWiFiNotSupported;
    }
    return eWiFiSuccess;
}

WIFIReturnCode_t WIFI_ConfigureAP( const WIFINetworkParams_t * const pxNetworkParams )
{
    LOG_I(wifi, "%s\n", __FUNCTION__);
    WIFIReturnCode_t retCode;

    if (WIFI_TakeSemaphore())
    {
        retCode = WIFI_ValidateNetworkParams(pxNetworkParams);
        if (retCode != eWiFiSuccess)
        {
            LOG_E(wifi, "WIFI ConfigureAP validation failed\n");
        }
        else
        {
            memcpy(&wifiAPConfig_, pxNetworkParams, sizeof(wifiAPConfig_));
        }
        return WIFI_GiveSemaphore(retCode);
    }
    LOG_I(wifi, "WIFI ConfigureAP timeout\n");
    return eWiFiTimeout;
}

WIFIReturnCode_t WIFI_GetCapability( WIFICapabilityInfo_t * pxCapabilities)
{
	
    LOG_I(wifi, "%s\n", __FUNCTION__);
    WIFIReturnCode_t retCode;
	
    if (WIFI_TakeSemaphore())
    {
        retCode = WIFI_HAL_GetCapability(pxCapabilities);

        if (retCode != eWiFiSuccess)
        {
            LOG_E(wifi, "WIFI GetCapability failed\n");
        }
        return WIFI_GiveSemaphore(retCode);

    }

    LOG_E(wifi, "WIFI GetCapability timeout\n");

    return eWiFiTimeout;
}

WIFIReturnCode_t WIFI_GetCountryCode( char * pcCountryCode )
{
    LOG_I(wifi, "%s\n", __FUNCTION__);
    WIFIReturnCode_t retCode;

    if (WIFI_TakeSemaphore())
    {
        retCode = WIFI_HAL_GetCountryCode(pcCountryCode);
        if (retCode != eWiFiSuccess)
        {
            LOG_I(wifi, "WIFI GetCountryCode failed\n");
        }
        return WIFI_GiveSemaphore(retCode);
    }
    LOG_I(wifi, "WIFI GetCountryCode timeout\n");
    return eWiFiTimeout;
}

WIFIReturnCode_t WIFI_GetIPInfo( WIFIIPConfiguration_t * pxIPInfo )
{
    LOG_I(wifi, "%s\n", __FUNCTION__);
    WIFIReturnCode_t retCode;

    if (WIFI_TakeSemaphore())
    {
        retCode = WIFI_HAL_GetIPInfo(pxIPInfo);
        if (retCode != eWiFiSuccess)
        {
            LOG_I(wifi, "WIFI Get IP info failed\n");
        }
        return WIFI_GiveSemaphore(retCode);
    }
    LOG_I(wifi, "WIFI Get IP info timeout\n");
    return eWiFiTimeout;
}

WIFIReturnCode_t WIFI_GetMode( WIFIDeviceMode_t * pxDeviceMode )
{
    LOG_I(wifi, "%s\n", __FUNCTION__);
    WIFIReturnCode_t retCode;

    if (pxDeviceMode == NULL) {
        LOG_I(wifi, "WIFI GetModeExt failed because of NULL pointer\n");
        return eWiFiFailure;
    }

    if (WIFI_TakeSemaphore())
    {
        retCode = WIFI_HAL_GetMode(pxDeviceMode);
        if (retCode != eWiFiSuccess)
        {
            LOG_I(wifi, "WIFI GetModeExt failed\n");
        }
        return WIFI_GiveSemaphore(retCode);
    }
    LOG_I(wifi, "WIFI GetModeExt timeout\n");
    return eWiFiTimeout;
}

WIFIReturnCode_t WIFI_GetRSSI( int8_t * pcRSSI )
{
    LOG_I(wifi, "%s\n", __FUNCTION__);
    WIFIReturnCode_t retCode;

    if (WIFI_TakeSemaphore())
    {
        if (WIFI_HAL_IsConnected(NULL))
        {
            retCode = WIFI_HAL_GetRSSI(pcRSSI);
            if (retCode != eWiFiSuccess)
            {
                LOG_I(wifi, "WIFI GetRSSI failed\n");
            }
        }
        else
        {
            LOG_I(wifi, "WIFI GetRSSI not connected\n");
            retCode = eWiFiFailure;
        }
        return WIFI_GiveSemaphore(retCode);
    }
    LOG_I(wifi, "WIFI GetRSSI timeout\n");
    return eWiFiTimeout;
}

WIFIReturnCode_t WIFI_GetScanResults( const WIFIScanResult_t ** pxBuffer,
                                      uint16_t * ucNumNetworks )
{
    LOG_I(wifi, "%s, numScanResults %u\n", __FUNCTION__, wifiNumScanResults_);

    if (WIFI_TakeSemaphore())
    {
        *pxBuffer = wifiScanResults_;
        *ucNumNetworks = wifiNumScanResults_;

        WIFI_ShowScanResults();
        return WIFI_GiveSemaphore(eWiFiSuccess);
    }
    LOG_I(wifi, "WIFI GetScanResults timeout\n");
    return eWiFiTimeout;
}

WIFIReturnCode_t WIFI_GetStationList( WIFIStationInfo_t * pxStationList,
                                      uint8_t * puStationListSize )
{
    LOG_I(wifi, "%s\n", __FUNCTION__);
    WIFIReturnCode_t retCode;

    if(pxStationList == NULL || puStationListSize == NULL || *puStationListSize == 0) {
        return eWiFiFailure;
    }

    if (WIFI_TakeSemaphore())
    {
        retCode = WIFI_HAL_GetStationList(pxStationList, puStationListSize);
        if (retCode != eWiFiSuccess)
        {
            LOG_I(wifi, "WIFI GetStationList failed\n");
            return eWiFiFailure;
        }
        return WIFI_GiveSemaphore(retCode);
    }
    LOG_I(wifi, "WIFI GetStationList timeout\n");
    return eWiFiTimeout;
}

WIFIReturnCode_t WIFI_GetStatistic( WIFIStatisticInfo_t * pxStatistics )
{
    LOG_I(wifi, "%s\n", __FUNCTION__);
    WIFIReturnCode_t retCode;

    if (WIFI_TakeSemaphore())
    {
        retCode = WIFI_HAL_GetStatistic(pxStatistics);
        if (retCode != eWiFiSuccess)
        {
            LOG_I(wifi, "WIFI GetStatistic failed\n");
        }
        return WIFI_GiveSemaphore(retCode);
    }
    LOG_I(wifi, "WIFI GetStatistic timeout\n");
    return eWiFiTimeout;
}

WIFIReturnCode_t WIFI_RegisterEvent( WIFIEventType_t xEventType,
                                     WIFIEventHandler_t xHandler )
{
    LOG_I(wifi, "%s %u\n", __FUNCTION__, xEventType);
    WIFIEvent_t event;
    WIFIReturnCode_t retCode;

    if (xEventType >= eWiFiEventMax)
    {
        LOG_I(wifi, "WIFI RegisterEvent %u invalid\n", xEventType);
        return eWiFiFailure;
    }
    wifiEventHandlers_[xEventType] = xHandler;

    switch (xEventType)
    {
        // These events are used by the sync API so will be registered automatically
        case eWiFiEventReady:
            if (wifiInitDone_)
            {
                // WIFI init has already happened
                event.xEventType = eWiFiEventReady;
                xHandler(&event);
            }
            break;
        case eWiFiEventScanDone:
        case eWiFiEventConnected:
        case eWiFiEventDisconnected:
        case eWiFiEventAPStateChanged:
        case eWiFiEventIPReady:
            break;
        default:
            // All other events are passed to WIFI_HAL layer
            retCode = WIFI_HAL_RegisterEvent(xEventType, xHandler);
            if (retCode != eWiFiSuccess)
            {
                LOG_I(wifi, "WIFI RegisterEvent %u failed\n", xEventType);
                return eWiFiFailure;
            }
    }
    return eWiFiSuccess;
}

WIFIReturnCode_t WIFI_SetCountryCode( const char * pcCountryCode )
{
    LOG_I(wifi, "%s\n", __FUNCTION__);
    WIFIReturnCode_t retCode;

    if (WIFI_TakeSemaphore())
    {
        retCode = WIFI_HAL_SetCountryCode(pcCountryCode);
        if (retCode != eWiFiSuccess)
        {
            LOG_I(wifi, "WIFI SetCountryCode failed\n");
        }
        return WIFI_GiveSemaphore(retCode);
    }
    LOG_I(wifi, "WIFI SetCountryCode timeout\n");
    return eWiFiTimeout;
}

WIFIReturnCode_t WIFI_SetModeExt( WIFIDeviceMode_t xDeviceMode )
{
    LOG_I(wifi, "%s\n", __FUNCTION__);
    WIFIReturnCode_t retCode;

    if (WIFI_TakeSemaphore())
    {
        retCode = WIFI_HAL_SetMode(xDeviceMode);
        if (retCode != eWiFiSuccess)
        {
            LOG_I(wifi, "WIFI SetMode failed\n");
        }
        return WIFI_GiveSemaphore(retCode);
    }
    LOG_I(wifi, "WIFI SetMode timeout\n");
    return eWiFiTimeout;
}

WIFIReturnCode_t WIFI_StartAP( void )
{
    LOG_I(wifi, "%s\n", __FUNCTION__);
    WIFIReturnCode_t retCode;

    if (WIFI_TakeSemaphore())
    {
        // Clear event before the async procedure to avoid race condition!
        WIFI_ClearEvent(WIFI_EVENT_AP_STATE_CHANGED);

        // We don't maintain AP state machine here. Upper layer has that responsibility
        retCode = WIFI_HAL_StartAP(&wifiAPConfig_);
        if (retCode != eWiFiSuccess)
        {
            LOG_I(wifi, "WIFI StartAP failed\n");
        }
        else
        {
            if (WIFI_WaitEvent(WIFI_EVENT_AP_STATE_CHANGED))
            {
                LOG_I(wifi, "WIFI StartAP succeess\n");
            }
            else
            {
                LOG_I(wifi, "WIFI StartAP event timeout\n");
                retCode = eWiFiTimeout;
            }
        }
        return WIFI_GiveSemaphore(retCode);
    }
    LOG_I(wifi, "WIFI StartAP timeout\n");
    return eWiFiTimeout;
}

WIFIReturnCode_t WIFI_StartConnectAP( const WIFINetworkParams_t * pxNetworkParams )
{
    LOG_I(wifi, "%s\n", __FUNCTION__);
    WIFIReturnCode_t retCode;

    retCode = WIFI_ValidateNetworkParams(pxNetworkParams);
    if (retCode != eWiFiSuccess)
    {
        LOG_I(wifi, "WIFI StartConnectAP validation failed\n");
        return eWiFiFailure;
    }

    if (WIFI_TakeSemaphore())
    {
        // If already connected to the same AP, do nothing
        if (WIFI_HAL_IsConnected(pxNetworkParams))
        {
            LOG_I(wifi, "WIFI StartConnectAP already connected\n");
            retCode = eWiFiSuccess;
        }
        else
        {
            retCode = WIFI_HAL_StartConnectAP(pxNetworkParams);
            if (retCode != eWiFiSuccess)
            {
                LOG_I(wifi, "WIFI StartConnectAP failed\n");
            }
        }
        return WIFI_GiveSemaphore(retCode);
    }
    LOG_I(wifi, "WIFI StartConnectAP timeout\n");
    return eWiFiTimeout;
}

WIFIReturnCode_t WIFI_StartDisconnect( void )
{
    LOG_I(wifi, "%s\n", __FUNCTION__);
    WIFIReturnCode_t retCode;

    if (WIFI_TakeSemaphore())
    {
        retCode = WIFI_HAL_StartDisconnectAP();
        if (retCode != eWiFiSuccess)
        {
            LOG_I(wifi, "WIFI StartDisconnect failed\n");
        }
        return WIFI_GiveSemaphore(retCode);
    }
    LOG_I(wifi, "WIFI StartDisconnect timeout\n");
    return eWiFiTimeout;
}

WIFIReturnCode_t WIFI_StartScan( WIFIScanConfig_t * pxScanConfig )
{
    LOG_I(wifi, "%s\n", __FUNCTION__);
    WIFIReturnCode_t retCode;

    if (WIFI_TakeSemaphore())
    {
        // Invalidate cached scan results
        wifiNumScanResults_ = 0;

        retCode = WIFI_HAL_StartScan(pxScanConfig);
        if (retCode != eWiFiSuccess)
        {
            LOG_I(wifi, "WIFI StartScan failed\n");
        }
        return WIFI_GiveSemaphore(retCode);
    }
    LOG_I(wifi, "WIFI StartScan timeout\n");
    return eWiFiTimeout;
}

/* --------------------------------------- Extended */

static inline uint8_t WIFI_HAL_H2B( char c )
{
    if (c >= '0' && c <= '9')
    {
        return (c - '0');
    }
    if (c >= 'A' && c <= 'F')
    {
        return (c - 'A' + 0xa);
    }
    if (c >= 'a' && c <= 'f')
    {
        return (c - 'a' + 0xa);
    }
    return 0;
}

static void WIFI_HAL_Hex2Bin( uint8_t * dst, const char * src, uint8_t dstLength )
{
    for (int i = 0; i < dstLength; i++)
    {
        dst[i] = (WIFI_HAL_H2B(src[i*2]) << 4) + WIFI_HAL_H2B(src[i*2 + 1]);
    }
}

static inline wifi_auth_mode_t WIFI_HAL_AuthMode( WIFISecurity_t security )
{
    switch (security)
    {
        case eWiFiSecurityOpen:
            return WIFI_AUTH_MODE_OPEN;
        case eWiFiSecurityWEP:
            return WIFI_AUTH_MODE_AUTO_WEP;
        case eWiFiSecurityWPA:
        case eWiFiSecurityWPA2:
            return WIFI_AUTH_MODE_WPA_PSK_WPA2_PSK;
        default:
            return WIFI_AUTH_MODE_WPA_None;
    }
}

static WIFIReturnCode_t WIFI_HAL_FormatWEPKey( const WIFINetworkParams_t * pxNetworkParams,
                                               wifi_wep_key_t * pxWEPKey )
{
    memset(pxWEPKey, 0, sizeof(wifi_wep_key_t));

    for (int i = 0; i < wificonfigMAX_WEPKEYS; i++)
    {
        const WIFIWEPKey_t* inKey = &pxNetworkParams->xPassword.xWEP[i];
        uint8_t* outKey = &pxWEPKey->wep_key[i][0];
        uint8_t length = inKey->ucLength;

        if (length == 10 || length == 26)
        {
            // Need to convert hes string to binary
            pxWEPKey->wep_key_length[i] = length / 2;
            WIFI_HAL_Hex2Bin(outKey, inKey->cKey, length / 2);
        }
        else if (length == 5 || length == 13)
        {
            pxWEPKey->wep_key_length[i] = length;
            memcpy(outKey, inKey->cKey, length);
        }
        else
        {
            // Not all WEP keys are required, so reject only the used key
            if (i == pxNetworkParams->ucDefaultWEPKeyIndex)
            {
                return eWiFiFailure;
            }
        }
    }
    pxWEPKey->wep_tx_key_index = pxNetworkParams->ucDefaultWEPKeyIndex;
    return eWiFiSuccess;
}


WIFIReturnCode_t WIFI_HAL_StartConnectAP( const WIFINetworkParams_t * pxNetworkParams )
{
    wifi_wep_key_t wepKey;
    WIFIReturnCode_t retCode;
    int32_t retVal;
    wifi_auth_mode_t authMode = WIFI_AUTH_MODE_WPA_None;

    // Note this function is in wifi_lwip_helper.h, not wifi_api.h
    retVal = wifi_set_opmode(WIFI_MODE_STA_ONLY);
    if (retVal < 0)
    {
        return eWiFiFailure;
    }

    retVal = wifi_config_set_ssid(WIFI_PORT_STA,
                                  (uint8_t*)pxNetworkParams->ucSSID,    // cast away const
                                  pxNetworkParams->ucSSIDLength);
    if (retVal < 0)
    {
        return eWiFiFailure;
    }

    authMode = WIFI_HAL_AuthMode(pxNetworkParams->xSecurity);
    switch (pxNetworkParams->xSecurity)
    {
        case eWiFiSecurityWPA:
        case eWiFiSecurityWPA2:
            retVal = wifi_config_set_security_mode(WIFI_PORT_STA,
                                            authMode,
                                            WIFI_ENCRYPT_TYPE_TKIP_AES_MIX);
            if (retVal < 0)
            {
                LOG_E(common, "SDK set security mode failed\n");
                return eWiFiFailure;
            }

            retVal = wifi_config_set_wpa_psk_key(WIFI_PORT_STA,
                        (uint8_t*)pxNetworkParams->xPassword.xWPA.cPassphrase,
                        pxNetworkParams->xPassword.xWPA.ucLength);
            if (retVal < 0)
            {
                LOG_E(common, "SDK set wpa psk failed\n");
                return eWiFiFailure;
            }
            break;
        case eWiFiSecurityWEP:
            retVal = wifi_config_set_security_mode(WIFI_PORT_STA,
                                            authMode,
                                            WIFI_ENCRYPT_TYPE_WEP_ENABLED);
            if (retVal < 0)
            {
                LOG_E(common, "SDK set security mode failed\n");
                return eWiFiFailure;
            }

            retCode = WIFI_HAL_FormatWEPKey(pxNetworkParams, &wepKey);
            if (retCode != eWiFiSuccess)
            {
                return retCode;
            }

            retVal = wifi_config_set_wep_key(WIFI_PORT_STA, &wepKey);
            if (retVal < 0)
            {
                LOG_E(common, "SDK set wep key failed\n");
                return eWiFiFailure;
            }
            break;
        case eWiFiSecurityOpen:
            retVal = wifi_config_set_security_mode(WIFI_PORT_STA,
                                            authMode,
                                            WIFI_ENCRYPT_TYPE_WEP_DISABLED);
            if (retVal < 0)
            {
                LOG_E(common, "SDK set security mode failed\n");
                return eWiFiFailure;
            }
            break;
        default:
            LOG_E(common, "SDK security invalid\n");
            return eWiFiNotSupported;
    }
    retVal = wifi_config_reload_setting();
    if (retVal < 0)
    {
        LOG_E(common, "SDK set reload setting (sta) failed\n");
        return eWiFiFailure;
    }

    // save connect info
    memcpy(_g_state.ucSSID, pxNetworkParams->ucSSID, wificonfigMAX_SSID_LEN);
    _g_state.ucSSIDLength = pxNetworkParams->ucSSIDLength;
    _g_state.xSecurity = pxNetworkParams->xSecurity;
		
    return eWiFiSuccess;
}


