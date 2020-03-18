/*
 * $ Copyright Cypress Semiconductor $
 */

/** @file
 *  Implements functions for controlling the Wi-Fi system in Amazon FreeRTOS
 *
 *  This file provides functions which allow actions such as turning on,
 *  joining Wi-Fi networks, getting the Wi-Fi connection status, etc
 *
 */
#include "cy_wifi_abstraction.h"

#define ENTERPRISE_SECUTIRY_WIFI_ABSTRACTION_INFO(x) printf x

void wifi_on_ent( void )
{
    WIFIReturnCode_t xWifiStatus;

    xWifiStatus = WIFI_On();

    if( xWifiStatus != eWiFiSuccess )
    {
        ENTERPRISE_SECUTIRY_WIFI_ABSTRACTION_INFO( ( "Wi-Fi module failed to initialize, err=%d\r\n", xWifiStatus ) );
        return;
    }

    ENTERPRISE_SECUTIRY_WIFI_ABSTRACTION_INFO( ( "Wi-Fi module initialized. Connecting to AP...\r\n" ) );
}

int connect_ent( const char *ssid, uint8_t ssid_length,
                 const char *password, uint8_t password_length,
                 cy_supplicant_core_security_t auth_type )
{
    WIFIReturnCode_t xWifiStatus;
    uint8_t ucTempIp[ 4 ] = { 0 };
    WIFINetworkParams_t xNetworkParams;

    /* Setup parameters. */
    xNetworkParams.pcSSID = ssid;
    xNetworkParams.ucSSIDLength = ssid_length;
    xNetworkParams.pcPassword = NULL;
    xNetworkParams.ucPasswordLength = 0;
    if( auth_type == WHD_SECURITY_WPA2_MIXED_ENT )
    {
        xNetworkParams.xSecurity = eWiFiSecurityWPA2;
    }
    else
    {
        ENTERPRISE_SECUTIRY_WIFI_ABSTRACTION_INFO( ( "The auth type is invalid\r\n" ) );
        return CY_SUPPLICANT_STATUS_FAIL;
    }
    xNetworkParams.cChannel = 0;

    xWifiStatus = WIFI_ConnectAP( &( xNetworkParams ) );

    if( xWifiStatus != eWiFiSuccess )
    {
        ENTERPRISE_SECUTIRY_WIFI_ABSTRACTION_INFO( ( "Wi-Fi unable to connect, err=%d\r\n", xWifiStatus ) );
        return CY_SUPPLICANT_STATUS_JOIN_FAILURE;
    }

    ENTERPRISE_SECUTIRY_WIFI_ABSTRACTION_INFO( ( "Wi-Fi Connected to AP. Creating tasks which use network...\r\n" ) );

    xWifiStatus = WIFI_GetIP( ucTempIp );
    if( xWifiStatus != eWiFiSuccess )
    {
        ENTERPRISE_SECUTIRY_WIFI_ABSTRACTION_INFO( ( "Unable to get IP address, err=%d\r\n", xWifiStatus ) );
        return CY_SUPPLICANT_STATUS_JOIN_FAILURE;
    }

    ENTERPRISE_SECUTIRY_WIFI_ABSTRACTION_INFO( ( "IP Address acquired %d.%d.%d.%d\r\n", ucTempIp[ 0 ], ucTempIp[ 1 ], ucTempIp[ 2 ], ucTempIp[ 3 ] ) );
    return CY_SUPPLICANT_STATUS_PASS;
}

int disconnect_ent( void )
{
    WIFIReturnCode_t xWifiStatus = WIFI_Disconnect();
    if( xWifiStatus != eWiFiSuccess )
    {
        ENTERPRISE_SECUTIRY_WIFI_ABSTRACTION_INFO( ( "Not Successfully DisConnected from AP, err=%d.\r\n", xWifiStatus ) );
        return CY_SUPPLICANT_STATUS_FAIL;
    }

    ENTERPRISE_SECUTIRY_WIFI_ABSTRACTION_INFO( ( "Successfully DisConnected from AP.\r\n" ) );
    return CY_SUPPLICANT_STATUS_PASS;
}

wifi_connection_status_t is_wifi_connected( void )
{
    BaseType_t status = WIFI_IsConnected();
    if( status == pdTRUE )
    {
        return WIFI_CONNECTED;
    }
    else
    {
        return WIFI_NOT_CONNECTED;
    }
}
