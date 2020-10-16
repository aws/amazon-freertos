/*
 * FreeRTOS Wi-Fi for Cypress CYW943907AEVAL1F development kit V1.0.0
 * Copyright (C) 2020 Amazon.com, Inc. or its affiliates.  All Rights Reserved.
 *
 * Copyright 2019, Cypress Semiconductor Corporation or a subsidiary of
 * Cypress Semiconductor Corporation. All Rights Reserved.
 * 
 * This software, associated documentation and materials ("Software")
 * is owned by Cypress Semiconductor Corporation,
 * or one of its subsidiaries ("Cypress") and is protected by and subject to
 * worldwide patent protection (United States and foreign),
 * United States copyright laws and international treaty provisions.
 * Therefore, you may use this Software only as provided in the license
 * agreement accompanying the software package from which you
 * obtained this Software ("EULA").
 * If no EULA applies, Cypress hereby grants you a personal, non-exclusive,
 * non-transferable license to copy, modify, and compile the Software
 * source code solely for use in connection with Cypress's
 * integrated circuit products. Any reproduction, modification, translation,
 * compilation, or representation of this Software except as specified
 * above is prohibited without the express written permission of Cypress.
 *
 * Disclaimer: THIS SOFTWARE IS PROVIDED AS-IS, WITH NO WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING, BUT NOT LIMITED TO, NONINFRINGEMENT, IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE. Cypress
 * reserves the right to make changes to the Software without notice. Cypress
 * does not assume any liability arising out of the application or use of the
 * Software or any product or circuit described in the Software. Cypress does
 * not authorize its products for use in any products where a malfunction or
 * failure of the Cypress product may reasonably be expected to result in
 * significant property damage, injury or death ("High Risk Product"). By
 * including Cypress's product in a High Risk Product, the manufacturer
 * of such system or application assumes all risk of such use and in doing
 * so agrees to indemnify Cypress against all liability.
 */

/**
 * @file iot_wifi.c
 * @brief Wi-Fi Interface.
 */

/* Socket and Wi-Fi interface includes. */
#include "FreeRTOS.h"
#include "iot_wifi.h"

/* Wi-Fi configuration includes. */
#include "aws_wifi_config.h"

/* CY includes */
#include "wiced.h"
#include "rtos.h"
#include "lwip/netdb.h"
BaseType_t isConnected;
static char pcConnectedSSID[ wificonfigMAX_SSID_LEN + 1 ] = { 0 };
wiced_mutex_t serializeWifi; /**< Mutex used to serialize all the operations on the Wi-Fi module. */

#define configPRINTF( X ) //printf X

typedef struct
{
    wiced_semaphore_t   semaphore;      /* Semaphore used for signaling scan complete */
    WIFIScanResult_t *  pxBuffer;
    uint8_t             ucNumNetworks;   /* Count to measure the total scan results    */
} scan_arg_t;

/**
 * @brief Maps the given abstracted security type to CY specific one.
 *
 * @param[in] xSecurity The given abstracted security type.
 *
 * @return Corresponding CY specific security type.
 */
static wiced_security_t prvConvertSecurityFromAbstractedToCY( WIFISecurity_t xSecurity )
{
    wiced_security_t xConvertedSecurityType = WICED_SECURITY_UNKNOWN;

    switch( xSecurity )
    {
        case eWiFiSecurityOpen:
            xConvertedSecurityType = WICED_SECURITY_OPEN;
            break;

        case eWiFiSecurityWEP:
            xConvertedSecurityType = WICED_SECURITY_WEP_SHARED;
            break;

        case eWiFiSecurityWPA:
            xConvertedSecurityType = WICED_SECURITY_WPA_MIXED_PSK;
            break;

        case eWiFiSecurityWPA2:
            xConvertedSecurityType = WICED_SECURITY_WPA2_MIXED_PSK;
            break;

        case eWiFiSecurityNotSupported:
            xConvertedSecurityType = WICED_SECURITY_UNKNOWN;
            break;
    }

    return xConvertedSecurityType;
}

/*-----------------------------------------------------------*/

/**
 * @brief Copies byte array into char array, appending '\0'. Assumes char array can hold length of byte array + 1.
 *        Null terminator is guaranteed so long as xLen > 0. A maximum of xCap - 1 pucSrc bytes will be copied into pcDest,
 *        therefore truncation is possible.
 *
 * @param[in] pcDest The string to copy pucSrc contents into
 *
 * @param[in] pucSrc The byte array to copy into pcDest
 *
 * @param[in] xLen The queried number of byte to copy from pucSrc to pcDest
 *
 * @param[in] xCap Capacity of pcDest i.e. max characters it can store
 *
 */
static size_t prvByteArrayToString( char *pcDest, const uint8_t *pucSrc, size_t xLen, size_t xCap )
{
    configASSERT( pcDest );
    configASSERT( pucSrc );

    if ( xLen > ( xCap - 1 ) )
    {
        xLen = xCap - 1;
    }

    memcpy( pcDest, pucSrc, xLen );
    pcDest[ xLen ] = '\0';

    return xLen;
}

/*-----------------------------------------------------------*/

WIFIReturnCode_t WIFI_On( void )
{
    /* FIX ME. */

    /* Initialize mutex. */
    wiced_rtos_init_mutex( &serializeWifi );

    return eWiFiSuccess;
}
/*-----------------------------------------------------------*/

WIFIReturnCode_t WIFI_Off( void )
{
    /* FIX ME. */

    /* De-Initialize mutex. */
    wiced_rtos_deinit_mutex( &serializeWifi );

    return eWiFiSuccess;
}
/*-----------------------------------------------------------*/

WIFIReturnCode_t WIFI_ConnectAP( const WIFINetworkParams_t * const pxNetworkParams )
{
    wiced_result_t      result;
    wiced_scan_result_t ap_info;
    wiced_ap_info_t     ap;
    char pcSSID[ wificonfigMAX_SSID_LEN + 1 ];

    if( (pxNetworkParams == NULL) || (pxNetworkParams->ucSSIDLength == 0 ) || ( pxNetworkParams->ucSSIDLength > wificonfigMAX_SSID_LEN ) )
    {
        return eWiFiFailure;
    }

    if( (pxNetworkParams->xSecurity != eWiFiSecurityOpen) && (pxNetworkParams->xPassword.xWPA.ucLength == 0) )
    {
        return eWiFiFailure;
    }

    configPRINTF( ( "WIFI_ConnectAP...\r\n") );

    wiced_rtos_lock_mutex( &serializeWifi );

    /* Disconnect first if we are connected, to connect to the input network. */
    if( isConnected )
    {
        result = wiced_network_down( WICED_STA_INTERFACE );
        if (WICED_SUCCESS != result)
        {
            configPRINTF( ("wiced_network_down failed %d \n", result) );
            goto fail_exit;
        }

        isConnected = 0;
    }

    prvByteArrayToString( pcSSID,
                          pxNetworkParams->ucSSID,
                          pxNetworkParams->ucSSIDLength,
                          wificonfigMAX_SSID_LEN );
    result = wiced_wifi_find_ap( pcSSID, &ap_info, NULL );
    if( result != WICED_SUCCESS)
    {
        configPRINTF( ( "wifi_utils_join failed %d...\r\n", result ) );
        goto fail_exit;
    }

    memcpy( &ap, &ap_info, sizeof(wiced_ap_info_t) );
    ap.next = NULL;

    result = wiced_join_ap_specific( &ap, pxNetworkParams->xPassword.xWPA.ucLength, pxNetworkParams->xPassword.xWPA.cPassphrase);
    if( result != WICED_SUCCESS)
    {
        configPRINTF( ( "wifi_utils_join failed %d...\r\n", result ) );
        goto fail_exit;
    }

    result = wiced_ip_up( WICED_STA_INTERFACE, WICED_USE_EXTERNAL_DHCP_SERVER, NULL );
    if( result != WICED_SUCCESS)
    {
        configPRINTF( ( "wifi_utils_join failed %d...\r\n", result ) );
        goto fail_exit;
    }

    isConnected = 1;
    memcpy( pcConnectedSSID, pcSSID, strlen(pcSSID) + 1 );

    /* Unlock the mutex. */
    wiced_rtos_unlock_mutex( &serializeWifi );

    return eWiFiSuccess;

fail_exit:
    wiced_rtos_unlock_mutex( &serializeWifi );
    return eWiFiFailure;

}
/*-----------------------------------------------------------*/

WIFIReturnCode_t WIFI_Disconnect( void )
{
    wiced_result_t result;
    configPRINTF( ( "WIFI_Disconnect...\r\n") );

    wiced_rtos_lock_mutex( &serializeWifi );

    result = wiced_network_down( WICED_STA_INTERFACE );
    if (WICED_SUCCESS != result)
    {
        configPRINTF( ("wiced_network_down failed %d \n", result) );
        wiced_rtos_unlock_mutex( &serializeWifi );
        return eWiFiFailure;
    }
    isConnected = 0;

    wiced_rtos_unlock_mutex( &serializeWifi );

    configPRINTF( ( "WIFI_Disconnect success...\r\n") );
    return eWiFiSuccess;
}
/*-----------------------------------------------------------*/

WIFIReturnCode_t WIFI_Reset( void )
{
    /* FIX ME. */
    configPRINTF( ( "WIFI_Reset...\r\n") );
    return eWiFiSuccess;
}
/*-----------------------------------------------------------*/

/*
 * Callback function to handle scan results
 */
wiced_result_t scan_result_handler( wiced_scan_handler_result_t* malloced_scan_result )
{
    static int i = 0;

    /* Validate the input arguments */
    wiced_assert("Bad args", malloced_scan_result != NULL);

    if ( malloced_scan_result != NULL )
    {
        scan_arg_t* scan_data  = (scan_arg_t*)malloced_scan_result->user_data;

        malloc_transfer_to_curr_thread( malloced_scan_result );

        if ( malloced_scan_result->status == WICED_SCAN_INCOMPLETE )
        {
            wiced_scan_result_t* record = &malloced_scan_result->ap_details;

            if( i >= scan_data->ucNumNetworks )
            {
                return WICED_SUCCESS;
            }

            scan_data->pxBuffer[ i ].ucSSIDLength = record->SSID.length > wificonfigMAX_SSID_LEN ? wificonfigMAX_SSID_LEN : record->SSID.length;
            memcpy( scan_data->pxBuffer[i].ucSSID, record->SSID.value, scan_data->pxBuffer[ i ].ucSSIDLength );
            memcpy( scan_data->pxBuffer[i].ucBSSID, (uint8_t*)&record->BSSID, sizeof(scan_data->pxBuffer[i].ucBSSID) );

            if( (record->security && WEP_ENABLED ) )
            {
                scan_data->pxBuffer[i].xSecurity = eWiFiSecurityWEP;
            }
            else if( (record->security && WPA_SECURITY ) )
            {
                scan_data->pxBuffer[i].xSecurity = eWiFiSecurityWPA;
            }
            else if( (record->security && WPA2_SECURITY ) )
            {
                scan_data->pxBuffer[i].xSecurity = eWiFiSecurityWPA2;
            }
            else if( (record->security == WICED_SECURITY_OPEN ) )
            {
                scan_data->pxBuffer[i].xSecurity = eWiFiSecurityOpen;
            }
            else
            {
                scan_data->pxBuffer[i].xSecurity = eWiFiSecurityNotSupported;
            }

            scan_data->pxBuffer[i].cRSSI = (int8_t)record->signal_strength;
            scan_data->pxBuffer[i].ucChannel = (int8_t)record->channel;
            i++;

            //print_scan_result(record);
        }
        else
        {
            i = 0;
            wiced_rtos_set_semaphore( &scan_data->semaphore );
        }

        free( malloced_scan_result );
    }
    return WICED_SUCCESS;
}

WIFIReturnCode_t WIFI_Scan( WIFIScanResult_t * pxBuffer,
                            uint8_t ucNumNetworks )
{
    scan_arg_t scan_data;

    if( pxBuffer == NULL || ucNumNetworks == 0 )
    {
        return eWiFiFailure;
    }

    configPRINTF( ("\nScan starts.\n") );
    /* Initialize the semaphore that will tell us when the scan is complete */
    wiced_rtos_init_semaphore(&scan_data.semaphore);
    scan_data.ucNumNetworks = ucNumNetworks;
    scan_data.pxBuffer = pxBuffer;

    /* Start the scan */
    wiced_wifi_scan_networks(scan_result_handler, &scan_data );

    /* Wait until scan is complete */
    wiced_rtos_get_semaphore(&scan_data.semaphore, WICED_WAIT_FOREVER);

    configPRINTF( ("\nScan complete\n") );

    /* Clean up */
    wiced_rtos_deinit_semaphore(&scan_data.semaphore);
    return eWiFiSuccess;
}
/*-----------------------------------------------------------*/

WIFIReturnCode_t WIFI_SetMode( WIFIDeviceMode_t xDeviceMode )
{
    /* FIX ME. */
    configPRINTF( ( "WIFI_SetMode...\r\n") );
    return eWiFiNotSupported;
}
/*-----------------------------------------------------------*/

WIFIReturnCode_t WIFI_GetMode( WIFIDeviceMode_t * pxDeviceMode )
{
    /* FIX ME. */
    configPRINTF( ( "WIFI_GetMode...\r\n") );
    return eWiFiNotSupported;
}
/*-----------------------------------------------------------*/

WIFIReturnCode_t WIFI_NetworkAdd( const WIFINetworkProfile_t * const pxNetworkProfile,
                                  uint16_t * pusIndex )
{
    /* FIX ME. */
    configPRINTF( ( "WIFI_NetworkAdd...\r\n") );
    return eWiFiNotSupported;
}
/*-----------------------------------------------------------*/

WIFIReturnCode_t WIFI_NetworkGet( WIFINetworkProfile_t * pxNetworkProfile,
                                  uint16_t usIndex )
{
    /* FIX ME. */
    configPRINTF( ( "WIFI_NetworkGet...\r\n") );
    return eWiFiNotSupported;
}
/*-----------------------------------------------------------*/

WIFIReturnCode_t WIFI_NetworkDelete( uint16_t usIndex )
{
    /* FIX ME. */
    configPRINTF( ( "WIFI_NetworkDelete...\r\n") );
    return eWiFiNotSupported;
}
/*-----------------------------------------------------------*/

WIFIReturnCode_t WIFI_Ping( uint8_t * pucIPAddr,
                            uint16_t usCount,
                            uint32_t ulIntervalMS )
{
    /* FIX ME. */
    configPRINTF( ( "WIFI_Ping...\r\n") );
    return eWiFiNotSupported;
}
/*-----------------------------------------------------------*/

WIFIReturnCode_t WIFI_GetIPInfo( WIFIIPConfiguration_t * pxIPConfig )
{
    wiced_ip_address_t ip_address;
    wiced_result_t result;
    configPRINTF( ( "WIFI_GetIP...\r\n") );
    if( pxIPConfig == NULL )
    {
        return eWiFiFailure;
    }
    result = wiced_ip_get_ipv4_address( WICED_STA_INTERFACE, &ip_address );
    if( result != WICED_SUCCESS)
    {
        configPRINTF( ( "wiced_ip_get_ipv4_address failed %d...\r\n", result ) );
        return eWiFiFailure;
    }
    
    memcpy( &pxIPConfig->xIPAddress.ulAddress[ 0 ], &ip_address.ip.v4, 4 );

    return eWiFiSuccess;
}
/*-----------------------------------------------------------*/

WIFIReturnCode_t WIFI_GetMAC( uint8_t * pucMac )
{
    /* FIX ME. */
    configPRINTF( ( "WIFI_GetMAC...\r\n") );
    if( pucMac == NULL )
    {
        return eWiFiFailure;
    }
    wiced_wifi_get_mac_address( pucMac );
    return eWiFiSuccess;
}
/*-----------------------------------------------------------*/

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

WIFIReturnCode_t WIFI_StartAP( void )
{
    /* FIX ME. */
    configPRINTF( ( "WIFI_StartAP...\r\n") );
    return eWiFiNotSupported;
}
/*-----------------------------------------------------------*/

WIFIReturnCode_t WIFI_StopAP( void )
{
    /* FIX ME. */
    configPRINTF( ( "WIFI_StopAP...\r\n") );
    return eWiFiNotSupported;
}
/*-----------------------------------------------------------*/

WIFIReturnCode_t WIFI_ConfigureAP( const WIFINetworkParams_t * const pxNetworkParams )
{
    /* FIX ME. */
    configPRINTF( ( "WIFI_ConfigureAP...\r\n") );
    return eWiFiNotSupported;
}
/*-----------------------------------------------------------*/

WIFIReturnCode_t WIFI_SetPMMode( WIFIPMMode_t xPMModeType,
                                 const void * pvOptionValue )
{
    /* FIX ME. */
    configPRINTF( ( "WIFI_SetPMMode...\r\n") );
    return eWiFiNotSupported;
}
/*-----------------------------------------------------------*/

WIFIReturnCode_t WIFI_GetPMMode( WIFIPMMode_t * pxPMModeType,
                                 void * pvOptionValue )
{
    /* FIX ME. */
    configPRINTF( ( "WIFI_GetPMMode...\r\n") );
    return eWiFiNotSupported;
}
/*-----------------------------------------------------------*/

BaseType_t WIFI_IsConnected( const WIFINetworkParams_t * pxNetworkParams )
{
    /* FIX ME. */
    configPRINTF( ( "WIFI_IsConnected...\r\n") );

    BaseType_t xIsConnected = pdFALSE;

    if ( isConnected )
    {
        if ( pxNetworkParams )
        {
            if ( pxNetworkParams->ucSSIDLength > 0
                 && pxNetworkParams->ucSSIDLength <= wificonfigMAX_SSID_LEN
                 && memcmp( pxNetworkParams->ucSSID, pcConnectedSSID, pxNetworkParams->ucSSIDLength)
                 && pxNetworkParams->ucSSIDLength == strlen( pcConnectedSSID ) )
            {
                xIsConnected = pdTRUE;
            }
        }
        else
        {
            xIsConnected = pdTRUE;
        }
    }

    return xIsConnected;
}

WIFIReturnCode_t WIFI_RegisterEvent( WIFIEventType_t xEventType, WIFIEventHandler_t xHandler )
{
    /** Needs to implement dispatching network state change events **/
    return eWiFiNotSupported;
}
