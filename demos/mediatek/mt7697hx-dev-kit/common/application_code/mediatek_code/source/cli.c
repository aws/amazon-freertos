/*
 * Amazon FreeRTOS WiFi V1.4.5
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
 * copies or substantial portions of the Software. If you wish to use our Amazon
 * FreeRTOS name, please do so in a fair use way that does not cause confusion.
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
 * This file contains the CLI commands.
 */

#include <stdio.h>

#include <stdint.h>

#include <os.h>

#include "FreeRTOS.h"

#include "FreeRTOSIPConfig.h"

#include "task.h"

#include "cli.h"

#include "toi.h"

#include "io_def.h"

#include "aws_wifi.h"

#include "lwip/inet.h"

#include "syslog.h"

#include "task_def.h"

/*-----------------------------------------------------------*/

/* the length of a line of remembered history command line. */
#define HISTORY_LINE_MAX        (256)

/*-----------------------------------------------------------*/

/* the lines of remembered CLI commands. */
#define HISTORY_LINES           (5)

/*-----------------------------------------------------------*/

//#define MINICLI_TASK_NAME       "cli"

/*-----------------------------------------------------------*/

//#define MINICLI_TASK_STACKSIZE  (4096)

/*-----------------------------------------------------------*/

//#define MINICLI_TASK_PRIO       (1)

/*-----------------------------------------------------------*/

static const char *_aws_wifi_strerror( WIFIReturnCode_t r )
{
    switch( r )
    {
        case eWiFiSuccess:      return "eWiFiSuccess";
        case eWiFiFailure:      return "eWiFiFailure";
        case eWiFiTimeout:      return "eWiFiTimeout";
        case eWiFiNotSupported: return "eWiFiNotSupported";
    }

    return "UNKNOWN CODE";
}

/*-----------------------------------------------------------*/

static WIFISecurity_t _str_to_aws_security(const char *str)
{
    if ( 0 == strcmp( str, "open" ) ) return eWiFiSecurityOpen;
    if ( 0 == strcmp( str, "wep"  ) ) return eWiFiSecurityWEP;
    if ( 0 == strcmp( str, "wpa"  ) ) return eWiFiSecurityWPA;
    if ( 0 == strcmp( str, "wpa2" ) ) return eWiFiSecurityWPA2;

    return eWiFiSecurityNotSupported;
}

/*-----------------------------------------------------------*/

static const char *_device_mode_to_str( WIFIDeviceMode_t xDeviceMode )
{
    switch( xDeviceMode )
    {
    case eWiFiModeStation:      return "station";  
    case eWiFiModeAP:           return "ap";       
    case eWiFiModeP2P:          return "p2p";      
    case eWiFiModeNotSupported: return "not-supported-mode";
    default:
        return "unknown-mode";
    }
}

/*-----------------------------------------------------------*/

#define HANDLE_STATUS(_statement_)                          \
    do                                                      \
    {                                                       \
        WIFIReturnCode_t _r = ( _statement_ );              \
        printf( "%s\n", _aws_wifi_strerror( _r ) );         \
    } while ( 0 )

/*-----------------------------------------------------------*/

static uint8_t _do_WIFI_On( uint8_t len, char *param[] )
{
    HANDLE_STATUS( WIFI_On() );
    return 0;
}

/*-----------------------------------------------------------*/

static uint8_t _do_WIFI_Off( uint8_t len, char *param[] )
{
    HANDLE_STATUS( WIFI_Off() );
    return 0;
}

/*-----------------------------------------------------------*/

static int _parse_aws_net_params( WIFINetworkParams_t * pxNetworkParams,
                                                uint8_t len,
                                                 char * param [] )
{
    if( len < 2 || len > 3 )
    {
        printf("params: <ssid> [ <open|wep|wpa|wpa2> [ <password/key> ] ]\n");
        return 1;
    }

    pxNetworkParams->pcSSID       = param[0];
    pxNetworkParams->ucSSIDLength = os_strlen( param[0] );
    pxNetworkParams->xSecurity    = _str_to_aws_security( param[1] );

    if( eWiFiSecurityNotSupported == pxNetworkParams->xSecurity )
    {
        printf( "support security: open/wep/wpa/wpa2\n" );
        return 2;
    }

    if( eWiFiSecurityOpen != pxNetworkParams->xSecurity )
    {
        if( len < 3 )
        {
            printf( "security %s require a key\n", param[1] );
            return 3;
        }

        pxNetworkParams->pcPassword       = param[2];
        pxNetworkParams->ucPasswordLength = os_strlen( param[2] );
    }

    return 0;
}

/*-----------------------------------------------------------*/

static uint8_t _do_WIFI_ConnectAP( uint8_t len, char *param[] )
{
    WIFINetworkParams_t xNetworkParams;

    if( _parse_aws_net_params( &xNetworkParams, len, param ) )
    {
        return 1;
    }

    HANDLE_STATUS( WIFI_ConnectAP( &xNetworkParams ) );

    return 0;
}

/*-----------------------------------------------------------*/

static uint8_t _do_WIFI_Disconnect( uint8_t len, char *param[] )
{
    HANDLE_STATUS( WIFI_Disconnect() );
    return 0;
}

/*-----------------------------------------------------------*/

static uint8_t _do_WIFI_Reset( uint8_t len, char *param[] )
{
    HANDLE_STATUS( WIFI_Reset() );
    return 0;
}

/*-----------------------------------------------------------*/

static uint8_t _do_WIFI_SetMode( uint8_t len, char *param[] )
{
    WIFIDeviceMode_t xDeviceMode;

    if( len > 0 )
    {
        if( !os_strcmp( param[ 0 ], "sta" ) )
        {
            xDeviceMode = eWiFiModeStation;
        }
        else
        if( !os_strcmp( param[ 0 ], "ap" ) )
        {
            xDeviceMode = eWiFiModeAP;
        }
        else
        if( !os_strcmp( param[ 0 ], "p2p" ) )
        {
            xDeviceMode = eWiFiModeP2P;
        }
        else
        {
            xDeviceMode = eWiFiModeNotSupported;
        }

    }
    else
    {
        xDeviceMode = eWiFiModeStation;
        printf( "use default mode\n" );
    }

    printf( "set device mode to %s\n", _device_mode_to_str( xDeviceMode ) );

    HANDLE_STATUS( WIFI_SetMode( xDeviceMode ) );

    return 0;
}

/*-----------------------------------------------------------*/

static uint8_t _do_WIFI_GetMode( uint8_t len, char *param[] )
{
    WIFIDeviceMode_t xDeviceMode;

    HANDLE_STATUS( WIFI_GetMode( &xDeviceMode ) );

    printf( "device mode is %s\n", _device_mode_to_str( xDeviceMode ) );

    return 0;
}

/*-----------------------------------------------------------*/

static uint8_t _do_WIFI_NetworkAdd( uint8_t len, char *param[] )
{
    printf( "not supported\n" );
    return 0;
}

/*-----------------------------------------------------------*/

static uint8_t _do_WIFI_NetworkGet( uint8_t len, char *param[] )
{
    printf( "not supported\n" );
    return 0;
}

/*-----------------------------------------------------------*/

static uint8_t _do_WIFI_NetworkDelete( uint8_t len, char *param[] )
{
    printf( "not supported\n" );
    return 0;
}

/*-----------------------------------------------------------*/

static uint8_t _do_WIFI_Ping( uint8_t len, char *param[] )
{
    uint8_t             addr[ 4 ];
    int32_t             count;
    int32_t             interval;
    ip4_addr_t          ip_addr;
    WIFIReturnCode_t    r = eWiFiFailure;

    if( len == 0 )
    {
        addr[0] = 127;
        addr[1] = 0;
        addr[2] = 0;
        addr[3] = 1;
        printf( "use default ip address\n" );
    }
    else
    {
        if( inet_aton( param[ 0 ], &ip_addr ) == 1 )
        {
            addr[ 0 ] = ( ntohl( ip_addr.addr ) >> 24 ) & 0xFF;
            addr[ 1 ] = ( ntohl( ip_addr.addr ) >> 16 ) & 0xFF;
            addr[ 2 ] = ( ntohl( ip_addr.addr ) >>  8 ) & 0xFF;
            addr[ 3 ] = ( ntohl( ip_addr.addr )       ) & 0xFF;
        }
        else
        {
            printf( "invalid ip addr\n" );
            return 1;
        }
    }

    if( len > 1 )
    {
        uint8_t err;
        count = toi( param[ 1 ], &err );
        if( err == TOI_ERR || count > 65535 || count < 0)
        {
            printf( "invalid packet count\n" );
            return 2;
        }
    }
    else
    {
        count = 5;
    }

    if( len > 2 )
    {
        uint8_t err;
        interval = toi( param[ 2 ], &err );
        if( err == TOI_ERR || interval < 0 )
        {
            printf( "invalid interval\n" );
            return 3;
        }
    }
    else
    {
        interval = 1000;
    }

    printf( "ping %d.%d.%d.%d\n", addr[ 0 ] & 0xFF,
                                  addr[ 1 ] & 0xFF,
                                  addr[ 2 ] & 0xFF,
                                  addr[ 3 ] & 0xFF );

    HANDLE_STATUS( ( r = WIFI_Ping( addr,
                                    (uint16_t)count,
                                    (uint32_t)interval ) ) );

    if( r == eWiFiSuccess )
    {
        printf( "host ping is in good health\n" );
    }
    else
    {
        printf( "host ping packet lost\n" );
    }


    return 0;
}

/*-----------------------------------------------------------*/

static uint8_t _do_WIFI_GetIP( uint8_t len, char *param[] )
{
    uint8_t ucIPAddr[ 4 ];

    HANDLE_STATUS( WIFI_GetIP( &ucIPAddr[ 0 ] ) );

    printf("ip addr %u.%u.%u.%u\n", ucIPAddr[ 0 ] & 0xFF,
                                    ucIPAddr[ 1 ] & 0xFF,
                                    ucIPAddr[ 2 ] & 0xFF,
                                    ucIPAddr[ 3 ] & 0xFF );

    return 0;
}

/*-----------------------------------------------------------*/

static uint8_t _do_WIFI_GetMAC( uint8_t len, char *param[] )
{
    uint8_t ucMacAddressVal[ wificonfigMAX_BSSID_LEN ];

    HANDLE_STATUS( WIFI_GetMAC( &ucMacAddressVal[0] ) );

    printf("mac addr %02x:%02x:%02x:%02x:%02x:%02x\n",
                                        ucMacAddressVal[ 0 ] & 0xFF,
                                        ucMacAddressVal[ 1 ] & 0xFF,
                                        ucMacAddressVal[ 2 ] & 0xFF,
                                        ucMacAddressVal[ 3 ] & 0xFF,
                                        ucMacAddressVal[ 4 ] & 0xFF,
                                        ucMacAddressVal[ 5 ] & 0xFF );

    return 0;
}

/*-----------------------------------------------------------*/

static uint8_t _do_WIFI_GetHostIP( uint8_t len, char *param[] )
{
    uint8_t             ucIPAddr[ 4 ];
    char                *pcHost;
    WIFIReturnCode_t    r = eWiFiFailure;

    if( len > 0 )
    {
        pcHost = param[0];
    }
    else
    {
        pcHost = "localhost";
        printf( "use default host\n" );
    }

    printf( "resolve host: %s\n", pcHost );

    HANDLE_STATUS( r = WIFI_GetHostIP( pcHost, &ucIPAddr[ 0 ] ) );

    if( r == eWiFiSuccess )
    {
        printf( "ip addr %u.%u.%u.%u\n", ucIPAddr[ 0 ] & 0xFF,
                                         ucIPAddr[ 1 ] & 0xFF,
                                         ucIPAddr[ 2 ] & 0xFF,
                                         ucIPAddr[ 3 ] & 0xFF );
    }
    else
    {
        printf( "dns query failed\n" );
    }

    return 0;
}

/*-----------------------------------------------------------*/

static const char * _security_to_str(WIFISecurity_t security)
{
    switch( security )
    {
    case eWiFiSecurityOpen:         return "open"; 
    case eWiFiSecurityWEP:          return "wep";  
    case eWiFiSecurityWPA:          return "wpa";  
    case eWiFiSecurityWPA2:         return "wpa2"; 
    case eWiFiSecurityNotSupported: return "not-supported";
    default:
        return "unknown-security";
    }
}

/*-----------------------------------------------------------*/

static uint8_t _do_WIFI_Scan( uint8_t len, char *param[] )
{
    const uint8_t ucNumNetworks = 10; //Get 10 scan results
    WIFIScanResult_t * xScanResults;

    xScanResults = pvPortMalloc( sizeof( WIFIScanResult_t ) * ucNumNetworks );
    if( NULL == xScanResults )
    {
        printf( "Not enough memory\n" );
        return 1;
    }

    HANDLE_STATUS( WIFI_Scan( xScanResults,
                              ucNumNetworks ) );

    vTaskDelay( 1000 );

    for( int i = 0; i < ucNumNetworks; i++ )
    {
        WIFIScanResult_t *p = &xScanResults[ i ];

        for( int j = 0; j < strlen( p->cSSID ); j++ )
            p->cSSID[ j ] &= 0x7F;

        printf( "ssid       %s\n", p->cSSID );
        printf( "bssid      %02x:%02x:%02x:%02x:%02x:%02x:\n", p->ucBSSID[ 0 ],
                                                               p->ucBSSID[ 1 ],
                                                               p->ucBSSID[ 2 ],
                                                               p->ucBSSID[ 3 ],
                                                               p->ucBSSID[ 4 ],
                                                               p->ucBSSID[ 5 ] );
        printf( "security   %s\n", _security_to_str( p->xSecurity ) );
        printf( "rssi       %d\n", (int)p->cRSSI );
        printf( "channel    %d\n", (int)p->cChannel );
        printf( "visibility %s\n", (int)p->ucHidden ? "hidden" : "visible" );
    }

    vPortFree( xScanResults );

    return 0;
}

/*-----------------------------------------------------------*/

static uint8_t _do_WIFI_StartAP( uint8_t len, char *param[] )
{
    printf( "not supported\n" );
    return 0;
}

/*-----------------------------------------------------------*/

static uint8_t _do_WIFI_StopAP( uint8_t len, char *param[] )
{
    printf( "not supported\n" );
    return 0;
}

/*-----------------------------------------------------------*/

static uint8_t _do_WIFI_ConfigureAP( uint8_t len, char *param[] )
{
    WIFINetworkParams_t xNetworkParams;
    uint32_t            val;
    uint8_t             err;

    if( len < 1 )
    {
        printf("missing channel number\n");
        return 1;
    }

    val = toi( param[0], &err );

    if( err == TOI_ERR || val < 1 || val > 14 )
    {
        printf( "invalid channel number: %s\n", param[0] );
        return 2;
    }

    xNetworkParams.cChannel = (int8_t)val;

    if( _parse_aws_net_params( &xNetworkParams, len - 1, &param[1] ) )
    {
        return 2;
    }

    HANDLE_STATUS( WIFI_ConfigureAP( &xNetworkParams ) );

    return 0;
}

/*-----------------------------------------------------------*/

static uint8_t _do_WIFI_SetPMMode( uint8_t len, char *param[] )
{
    printf( "TBD\n" );
    return 0;
}

/*-----------------------------------------------------------*/

static uint8_t _do_WIFI_GetPMMode( uint8_t len, char *param[] )
{
    printf( "TBD\n" );
    return 0;
}

/*-----------------------------------------------------------*/

static void _cli_def_task(void *param)
{
    while (1) {
        cli_task();
    }
}

/*-----------------------------------------------------------*/

static void vCliHistoryInitialize( cli_history_t *hist )
{
    static char s_history_lines[ HISTORY_LINES ][ HISTORY_LINE_MAX ];
    static char *s_history_ptrs[ HISTORY_LINES ];

    static char s_history_input[ HISTORY_LINE_MAX ];
    static char s_history_parse_token[ HISTORY_LINE_MAX ];

    hist->history = &s_history_ptrs[0];

    for ( int i = 0; i < HISTORY_LINES; i++ )
    {
        s_history_ptrs[i] = s_history_lines[i];
    }

    hist->input             = s_history_input;
    hist->parse_token       = s_history_parse_token;
    hist->history_max       = HISTORY_LINES;
    hist->line_max          = HISTORY_LINE_MAX;
    hist->index             = 0;
    hist->position          = 0;
    hist->full              = 0;
}

/*-----------------------------------------------------------*/

static uint8_t _do_MQTT_demo_task( uint8_t len, char *param[] )
{
    extern void vStartMQTTEchoDemo( void );

    vStartMQTTEchoDemo();

    return 0;
}

/*-----------------------------------------------------------*/

void vCliTaskStartup( void )
{
    static cmd_t root[] =
    {
        { "a", "turn Wi-Fi on",                     _do_WIFI_On,               NULL },
        { "b", "turn Wi-Fi off",                    _do_WIFI_Off,              NULL },
        { "c", "connect to AP",                     _do_WIFI_ConnectAP,        NULL },
        { "d", "disconnect from AP",                _do_WIFI_Disconnect,       NULL },
        { "e", "reset Wi-Fi",                       _do_WIFI_Reset,            NULL },
        { "f", "set mode (sta/ap/p2p (not N/A)",    _do_WIFI_SetMode,          NULL },
        { "g", "get mode",                          _do_WIFI_GetMode,          NULL },
        { "h", "add network",                       _do_WIFI_NetworkAdd,       NULL },
        { "i", "get network",                       _do_WIFI_NetworkGet,       NULL },
        { "j", "del network",                       _do_WIFI_NetworkDelete,    NULL },
        { "k", "ping <ip addr>",                    _do_WIFI_Ping,             NULL },
        { "l", "get ip",                            _do_WIFI_GetIP,            NULL },
        { "m", "get mac",                           _do_WIFI_GetMAC,           NULL },
        { "n", "get host ip addr",                  _do_WIFI_GetHostIP,        NULL },
        { "o", "scan and show result",              _do_WIFI_Scan,             NULL },
        { "p", "start AP running",                  _do_WIFI_StartAP,          NULL },
        { "q", "stop AP running",                   _do_WIFI_StopAP,           NULL },
        { "r", "config AP",                         _do_WIFI_ConfigureAP,      NULL },
        { "s", "set power save mode <norm/lp/aon>", _do_WIFI_SetPMMode,        NULL },
        { "t", "get power save mode",               _do_WIFI_GetPMMode,        NULL },
        { "z", "MQTT demo task",                    _do_MQTT_demo_task,        NULL },
        { NULL, NULL, NULL, NULL }
    };

    static cli_t    cb =
    {
        .state  = 1,
        .echo   = 0,
        .get    = __io_getchar,
        .put    = __io_putchar,
        .cmd    = &root[0]
    };

    vCliHistoryInitialize( &cb.history );

    cli_init (&cb );

    if ( xTaskCreate( _cli_def_task,
                      MINICLI_TASK_NAME,
                      MINICLI_TASK_STACKSIZE / sizeof( portSTACK_TYPE ),
                      NULL,
                      MINICLI_TASK_PRIO,
                      NULL ) != pdPASS )
    {
        configPRINTF( ( "CLI task create failed\n" ) );
    }
}
