/*
 * Amazon FreeRTOS
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
 * @file aws_wifi.h
 * @brief WiFi Interface.
 */

#ifndef _AWS_WIFI_H_
#define _AWS_WIFI_H_

#include <stdint.h>

/* WiFi configuration includes. */
#include "aws_wifi_config.h"

/**
 * @brief Return code from various APIs.
 */
typedef enum
{
    eWiFiSuccess = 0,      /**< Success. */
    eWiFiFailure = 1,      /**< Failure. */
    eWiFiTimeout = 2,      /**< Timeout. */
    eWiFiNotSupported = 3, /**< Not supported. */
} WIFIReturnCode_t;

/**
 * @brief WiFi Security types.
 */
typedef enum
{
    eWiFiSecurityOpen = 0, /**< Open - No Security. */
    eWiFiSecurityWEP,      /**< WEP Security. */
    eWiFiSecurityWPA,      /**< WPA Security. */
    eWiFiSecurityWPA2,     /**< WPA2 Security. */
    eWiFiSecurityNotSupported   /**< Unknown Security. */
} WIFISecurity_t;

/**
 * @brief WiFi device modes.
 *
 * Device roles/modes supported.
 */
typedef enum
{
    eWiFiModeStation = 0, /**< Station mode. */
    eWiFiModeAP,          /**< Access point mode. */
    eWiFiModeP2P,         /**< P2P mode. */
    eWiFiModeNotSupported /**< Unsupported mode. */
} WIFIDeviceMode_t;

/**
 * @brief WiFi device power management modes.
 *
 * Device power management mode supported.
 */
typedef enum
{
    eWiFiPMNormal = 0,  /**< Normal mode. */
    eWiFiPMLowPower,    /**< Low Power mode. */
    eWiFiPMAlwaysOn,    /**< Always On mode. */
    eWiFiPMNotSupported /**< Unsupported PM mode. */
} WIFIPMMode_t;


/**
 * @brief Parameters passed to the WIFI_ConnectAP API.
 */
typedef struct
{
    const char * pcSSID;      /**< SSID of the WiFi network to join. */
    uint8_t ucSSIDLength;     /**< SSID length. */
    const char * pcPassword;  /**< Password needed to join the AP. */
    uint8_t ucPasswordLength; /**< Password length. */
    WIFISecurity_t xSecurity; /**< WiFi Security. @see WIFISecurity_t. */
    int8_t cChannel;          /**< Channel number. */
} WIFINetworkParams_t;

/**
 * @brief WiFi scan results.
 *
 * structure to store the WiFi scan results.
 */
typedef struct
{
    char cSSID[ wificonfigMAX_SSID_LEN ];       /**< SSID of the WiFi network. */
    uint8_t ucBSSID[ wificonfigMAX_BSSID_LEN ]; /**< BSSID of the WiFi network. */
    WIFISecurity_t xSecurity;                   /**< WiFi Security. @see WIFISecurity_t. */
    int8_t cRSSI;                               /**< Signal Strength. */
    int8_t cChannel;                            /**< Channel number. */
    uint8_t ucHidden;                           /**< Hidden channel. */
} WIFIScanResult_t;

/**
 * @brief Parameters passed to the WIFI_ConnectAP API.
 */
typedef struct
{
    char cSSID[ wificonfigMAX_SSID_LEN ];           /**< SSID of the WiFi network to join. */
    uint8_t ucSSIDLength;                           /**< SSID length. */
    uint8_t ucBSSID[ wificonfigMAX_BSSID_LEN ];     /**< BSSID of the WiFi network. */
    char cPassword[ wificonfigMAX_PASSPHRASE_LEN ]; /**< Password needed to join the AP. */
    uint8_t ucPasswordLength;                       /**< Password length. */
    WIFISecurity_t xSecurity;                       /**< WiFi Security. @see WIFISecurity_t. */
} WIFINetworkProfile_t;

/**
 * @brief Turns on WiFi.
 *
 * This function turns on WiFi module,initializes the drivers and must be called
 * before calling any other WiFi API
 *
 * @return eWiFiSuccess if everything succeeds, failure code otherwise.
 */
WIFIReturnCode_t WIFI_On( void );

/**
 * @brief Turns off WiFi.
 *
 * This function turns off WiFi
 *
 * @return eWiFiSuccess if everything succeeds, failure code otherwise.
 */
WIFIReturnCode_t WIFI_Off( void );

/**
 * @brief Connects to Access Point.
 *
 * @param[in] pxNetworkParams Configuration to join AP.
 *
 * @return eWiFiSuccess if connection is successful, failure code otherwise.
 *
 * @code
 * WIFINetworkParams_t xNetworkParams;
 * WIFIReturnCode_t xWifiStatus;
 * xNetworkParams.pcSSID = "SSID String";
 * xNetworkParams.ucSSIDLength = SSIDLen;
 * xNetworkParams.pcPassword = "Password String";
 * xNetworkParams.ucPasswordLength = PassLength;
 * xNetworkParams.xSecurity = eWiFiSecurityWPA2;
 * xWifiStatus = WIFI_ConnectAP( &( xNetworkParams ) );
 * if(xWifiStatus == eWiFiSuccess)
 * {
 *     //Connected to AP.
 * }
 * @endcode
 *
 * @see WIFINetworkParams_t
 */
WIFIReturnCode_t WIFI_ConnectAP( const WIFINetworkParams_t * const pxNetworkParams );

/**
 * @brief Disconnects from Access Point.
 *
 * @param[in] None.
 *
 * @return eWiFiSuccess if everything succeeds, failure code otherwise.
 */
WIFIReturnCode_t WIFI_Disconnect( void );

/**
 * @brief Resets the WiFi Module.
 *
 * @param[in] None.
 *
 * @return eWiFiSuccess if everything succeeds, failure code otherwise.
 */
WIFIReturnCode_t WIFI_Reset( void );

/**
 * @brief Sets WiFi mode.
 *
 * @param[in] xDeviceMode - Mode of the device Station / Access Point /P2P.
 *
 * @code
 * WIFIReturnCode_t xWifiStatus;
 * xWifiStatus = WIFI_SetMode(eWiFiModeStation);
 * if(xWifiStatus == eWiFiSuccess)
 * {
 *     //device Set to station mode
 * }
 * @endcode
 *
 * @return eWiFiSuccess if everything succeeds, failure code otherwise.
 */
WIFIReturnCode_t WIFI_SetMode( WIFIDeviceMode_t xDeviceMode );

/**
 * @brief Gets WiFi mode.
 *
 * @param[out] pxDeviceMode - return mode Station / Access Point /P2P
 *
 * @code
 * WIFIReturnCode_t xWifiStatus;
 * WIFIDeviceMode_t xDeviceMode;
 * xWifiStatus = WIFI_GetMode(&xDeviceMode);
 * if(xWifiStatus == eWiFiSuccess)
 * {
 *    //device mode is xDeviceMode
 * }
 * @endcode
 *
 * @return eWiFiSuccess if everything succeeds, failure code otherwise.
 */
WIFIReturnCode_t WIFI_GetMode( WIFIDeviceMode_t * pxDeviceMode );

/**
 * @brief WiFi Add Network profile.
 *
 * Adds wifi network to network list in Non Volatile memory
 *
 * @param[in] pxNetworkProfile - network profile parameters
 * @param[out] pusIndex - network profile index
 *
 * @return Profile stored index on success, or negative error code on failure.
 *
 * @code
 * WIFINetworkProfile_t xNetworkProfile = {0};
 * WIFIReturnCode_t xWiFiStatus;
 * uint16_t usIndex;
 * strncpy( xNetworkProfile.cSSID, "SSID_Name", SSIDLen));
 * xNetworkProfile.ucSSIDLength = SSIDLen;
 * strncpy( xNetworkProfile.cPassword, "PASSWORD",PASSLen );
 * xNetworkProfile.ucPasswordLength = PASSLen;
 * xNetworkProfile.xSecurity = eWiFiSecurityWPA2;
 * WIFI_NetworkAdd( &xNetworkProfile, &usIndex );
 * @endcode
 */
WIFIReturnCode_t WIFI_NetworkAdd( const WIFINetworkProfile_t * const pxNetworkProfile,
                                  uint16_t * pusIndex );


/**
 * @brief WiFi Get Network profile .
 *
 * Gets WiFi network parameters at given index from network list in Non volatile memory
 *
 * @param[out] pxNetworkProfile - pointer to return network profile parameters
 * @param[in] usIndex - Index of the network profile,
 *                       must be between 0 to wificonfigMAX_NETWORK_PROFILES
 *
 * @return eWiFiSuccess if everything succeeds, failure code otherwise.
 * @see WIFINetworkProfile_t
 *
 * @code
 * WIFINetworkProfile_t xNetworkProfile = {0};
 * uint16_t usIndex = 3;  //Get profile stored at index 3.
 * WIFI_NetworkGet( &xNetworkProfile, usIndex );
 * @endcode
 */
WIFIReturnCode_t WIFI_NetworkGet( WIFINetworkProfile_t * pxNetworkProfile,
                                  uint16_t usIndex );

/**
 * @brief WiFi Delete Network profile .
 *
 * Deletes WiFi network from network list at given index in Non volatile memory
 *
 * @param[in] usIndex - Index of the network profile, must be between 0 to wificonfigMAX_NETWORK_PROFILES
 *                      wificonfigMAX_NETWORK_PROFILES as index will delete all network profiles
 *
 * @return eWiFiSuccess if everything succeeds, failure code otherwise.
 *
 * @code
 * uint16_t usIndex = 2; //Delete profile at index 2
 * WIFI_NetworkDelete( usIndex );
 * @endcode
 *
 */
WIFIReturnCode_t WIFI_NetworkDelete( uint16_t usIndex );

/**
 * @brief Ping an IP address in the network.
 *
 * @param[in] IP Address array to ping.
 * @param[in] Number of times to ping
 * @param[in] Interval in mili-seconds for ping operation
 *
 * @return eWiFiSuccess if disconnected, eWiFiFailure otherwise.
 */
WIFIReturnCode_t WIFI_Ping( uint8_t * pucIPAddr,
                            uint16_t usCount,
                            uint32_t ulIntervalMS );

/**
 * @brief Retrieves the WiFi interface's IP address.
 *
 * @param[out] IP Address buffer.
 *
 * @return eWiFiSuccess if disconnected, eWiFiFailure otherwise.
 *
 * @code
 * uint8_t ucIPAddr[ 4 ];
 * WIFI_GetIP( &ucIPAddr[0] );
 * @endcode
 */
WIFIReturnCode_t WIFI_GetIP( uint8_t * pucIPAddr );

/**
 * @brief Retrieves the WiFi interface's MAC address.
 *
 * @param[out] MAC Address buffer.
 *
 * @code
 * uint8_t ucMacAddressVal[ wificonfigMAX_BSSID_LEN ];
 * WIFI_GetMAC( &ucMacAddressVal[0] );
 * @endcode
 *
 * @return eWiFiSuccess if disconnected, eWiFiFailure otherwise.
 */
WIFIReturnCode_t WIFI_GetMAC( uint8_t * pucMac );

/**
 * @brief Retrieves host IP address from URL using DNS
 *
 * @param[in] pxHost - Host URL.
 * @param[in] pxIPAddr - IP Address buffer.
 *
 * @return eWiFiSuccess if disconnected, eWiFiFailure otherwise.
 *
 * @code
 * uint8_t ucIPAddr[ 4 ];
 * WIFI_GetHostIP( "amazon.com", &ucIPAddr[0] );
 * @endcode
 */
WIFIReturnCode_t WIFI_GetHostIP( char * pcHost,
                                 uint8_t * pucIPAddr );

/**
 * @brief Perform WiF Scan
 *
 * @param[in] pxBuffer - Buffer for scan results.
 * @param[in] uxNumNetworks - Number of networks in scan result.
 *
 * @return eWiFiSuccess if disconnected, eWiFiFailure otherwise.
 *
 * @code
 * const uint8_t ucNumNetworks = 10; //Get 10 scan results
 * WIFIScanResult_t xScanResults[ ucNumNetworks ];
 * WIFI_Scan( xScanResults, ucNumNetworks );
 * @endcode
 */
WIFIReturnCode_t WIFI_Scan( WIFIScanResult_t * pxBuffer,
                            uint8_t ucNumNetworks );

/**
 * @brief Start SoftAP.
 *
 * @param[in] none.
 *
 * @return eWiFiSuccess if disconnected,  failure code otherwise.
 */
WIFIReturnCode_t WIFI_StartAP( void );

/**
 * @brief Stop SoftAP.
 *
 * @param[in] None.
 *
 * @return eWiFiSuccess if disconnected,  failure code otherwise.
 */
WIFIReturnCode_t WIFI_StopAP( void );

/**
 * @brief Configure SoftAP.
 *
 * @param[in] pxNetworkParams - Network parameters to configure AP.
 *
 * @return eWiFiSuccess if disconnected,  failure code otherwise.
 *
 * @code
 * WIFINetworkParams_t xNetworkParams;
 * xNetworkParams.pcSSID = "SSID_Name";
 * xNetworkParams.pcPassword = "PASSWORD";
 * xNetworkParams.xSecurity = eWiFiSecurityWPA2;
 * xNetworkParams.cChannel = ChannelNum;
 * WIFI_ConfigureAP( &xNetworkParams );
 * @endcode
 */
WIFIReturnCode_t WIFI_ConfigureAP( const WIFINetworkParams_t * const pxNetworkParams );

/**
 * @brief Set power management mode
 *
 * @param[in] xPMModeType - Power mode type.
 *
 * @param[in] pvOptionValue - A buffer containing the value of the option to set
 *                            depends on the mode type
 *                            example - beacon interval in sec
 *
 * @return eWiFiSuccess if disconnected, failure code otherwise.
 */
WIFIReturnCode_t WIFI_SetPMMode( WIFIPMMode_t xPMModeType,
                                 const void * pvOptionValue );

/**
 * @brief Get power management mode
 *
 * @param[out] xPMModeType - pointer to get current power mode set.
 *
 * @param[out] pvOptionValue - optional value
 *
 * @return eWiFiSuccess if disconnected, failure code otherwise.
 */
WIFIReturnCode_t WIFI_GetPMMode( WIFIPMMode_t * pxPMModeType,
                                 void * pvOptionValue );

#endif /* _AWS_WIFI_H_ */
