/*
 * FreeRTOS WiFi V2.0.0
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
 * @file iot_wifi.h
 * @brief Wi-Fi Interface.
 */

#ifndef _AWS_WIFI_H_
#define _AWS_WIFI_H_

#include <stdint.h>

/* FreeRTOS include for BaseType_t. */
#include "portmacro.h"

/* Wi-Fi configuration includes. */
#include "aws_wifi_config.h"

/**
 * @brief IPV6 length in 32-bit words
 *
 * @note This is the constant for IPV6 length in 32-bit words
 */
#define IPV6_LENGTH                  4

/**
 * @brief Wi-Fi lower level supported feature mask.
 *
 * @see WIFICapabilityInfo_t.
 */
#define WIFI_WPS_SUPPORTED           0x0001
#define WIFI_ENTERPRISE_SUPPORTED    0x0002
#define WIFI_P2P_SUPPORTED           0x0004
#define WIFI_TDLS_SUPPORTED          0x0008

/**
 * @brief Return code denoting API status.
 *
 * @note Codes other than eWiFiSuccess are failure codes.
 * @ingroup WiFi_datatypes_enums
 */
typedef enum
{
    eWiFiSuccess = 0,      /**< Success. */
    eWiFiFailure = 1,      /**< Failure. */
    eWiFiTimeout = 2,      /**< Timeout. */
    eWiFiNotSupported = 3, /**< Not supported. */
} WIFIReturnCode_t;

/**
 * @brief Wi-Fi Security types.
 *
 * @ingroup WiFi_datatypes_enums
 */
typedef enum
{
    eWiFiSecurityOpen = 0,    /**< Open - No Security. */
    eWiFiSecurityWEP,         /**< WEP Security. */
    eWiFiSecurityWPA,         /**< WPA Security. */
    eWiFiSecurityWPA2,        /**< WPA2 Security. */
    eWiFiSecurityWPA2_ent,    /**< WPA2 Enterprise Security. */
    eWiFiSecurityWPA3,        /**< WPA3 Security. */
    eWiFiSecurityNotSupported /**< Unknown Security. */
} WIFISecurity_t;

/**
 * @brief Wi-Fi device modes.
 *
 * Device roles/modes supported.
 * @ingroup WiFi_datatypes_enums
 */
typedef enum
{
    eWiFiModeStation = 0, /**< Station mode. */
    eWiFiModeAP,          /**< Access point mode. */
    eWiFiModeP2P,         /**< P2P mode. */
    eWiFiModeAPStation,   /**< AP+Station (repeater) mode. */
    eWiFiModeNotSupported /**< Unsupported mode. */
} WIFIDeviceMode_t;

/**
 * @brief Wi-Fi device power management modes.
 *
 * Device power management modes supported.
 * @ingroup WiFi_datatypes_enums
 */
typedef enum
{
    eWiFiPMNormal = 0,  /**< Normal mode. */
    eWiFiPMLowPower,    /**< Low Power mode. */
    eWiFiPMAlwaysOn,    /**< Always On mode. */
    eWiFiPMNotSupported /**< Unsupported PM mode. */
} WIFIPMMode_t;


/**
 * @brief Wi-Fi WEP keys (64- and 128-bit keys only)
 */
typedef struct
{
    char cKey[ wificonfigMAX_WEPKEY_LEN ]; /**< WEP key (binary array, not C-string). */
    uint8_t ucLength;                      /**< Key length. */
} WIFIWEPKey_t;

/**
 * @brief Wi-Fi WPA/WPA2 passphrase
 */
typedef struct
{
    char cPassphrase[ wificonfigMAX_PASSPHRASE_LEN ]; /**< WPA passphrase (binary array, not C-string). */
    uint8_t ucLength;                                 /**< Passphrase length (must be between 8 and 64 inclusive). */
} WIFIWPAPassphrase_t;

/**
 * @brief Parameters passed to the WIFI_ConnectAP API for connection.
 *
 * @see WIFI_ConnectAP
 *
 * @ingroup WiFi_datatypes_paramstructs
 */
typedef struct
{
    uint8_t ucSSID[ wificonfigMAX_SSID_LEN ]; /**< SSID of the Wi-Fi network (binary array, not C-string). */
    uint8_t ucSSIDLength;                     /**< SSID length. */
    WIFISecurity_t xSecurity;                 /**< Wi-Fi Security. */
    union
    {
        WIFIWEPKey_t xWEP[ wificonfigMAX_WEPKEYS ]; /**< WEP keys. */
        WIFIWPAPassphrase_t xWPA;                   /**< WPA/WPA2 passphrase. */
    } xPassword;
    uint8_t ucDefaultWEPKeyIndex;                   /**< Default WEP key index. */
    uint8_t ucChannel;                              /**< Channel number. */
} WIFINetworkParams_t;

/**
 * @brief Wi-Fi scan configuration.
 */
typedef struct
{
    uint8_t ucSSID[ wificonfigMAX_SSID_LEN ]; /**< SSID for targeted scan (binary array, not C-string). */
    uint8_t ucSSIDLength;                     /**< SSID length, 0 if broadcast scan. */
    uint8_t ucChannel;                        /**< Channel to scan (0 means all channels). */
} WIFIScanConfig_t;

/**
 * @brief Wi-Fi scan results.
 *
 * Structure to store the Wi-Fi scan results.
 *
 * @note The size of char arrays are the MAX lengths + 1 to
 * account for possible null terminating at the end of the
 * strings.
 *
 * @see WIFI_Scan
 *
 * @ingroup WiFi_datatypes_returnstructs
 */
typedef struct
{
    uint8_t ucSSID[ wificonfigMAX_SSID_LEN ];   /**< SSID of the Wi-Fi network (binary array, not C-string). */
    uint8_t ucSSIDLength;                       /**< SSID length. */
    uint8_t ucBSSID[ wificonfigMAX_BSSID_LEN ]; /**< BSSID of the Wi-Fi network (binary array, not C-string). */
    WIFISecurity_t xSecurity;                   /**< Security type of the Wi-Fi network. */
    int8_t cRSSI;                               /**< Signal strength of the Wi-Fi network. */
    uint8_t ucChannel;                          /**< Channel of the Wi-Fi network. */
} WIFIScanResult_t;

/**
 * @brief Wi-Fi SoftAP connected station info.
 */
typedef struct
{
    uint8_t ucMAC[ wificonfigMAX_BSSID_LEN ]; /**< MAC address of Wi-Fi station */
} WIFIStationInfo_t;


/**
 * @brief Wi-Fi network parameters passed to the WIFI_NetworkAdd API.
 *
 * @note The size of char arrays are the MAX lengths + 1 to
 * account for possible null terminating at the end of the
 * strings.
 *
 * @ingroup WiFi_datatypes_paramstructs
 */
typedef struct
{
    uint8_t ucSSID[ wificonfigMAX_SSID_LEN ];       /**< SSID of the Wi-Fi network to join with a NULL termination. */
    uint8_t ucSSIDLength;                           /**< SSID length not including NULL termination. */
    uint8_t ucBSSID[ wificonfigMAX_BSSID_LEN ];     /**< BSSID of the Wi-Fi network. */
    char cPassword[ wificonfigMAX_PASSPHRASE_LEN ]; /**< Password needed to join the AP. */
    uint8_t ucPasswordLength;                       /**< Password length not including null termination. */
    WIFISecurity_t xSecurity;                       /**< Wi-Fi Security. @see WIFISecurity_t. */
} WIFINetworkProfile_t;

/**
 * @brief Wi-Fi station IP address type.
 */
typedef enum
{
    eWiFiIPAddressTypeV4,
    eWiFiIPAddressTypeV6,
    eWiFiIPAddressTypeNotSupported,
} WIFIIPAddressType_t;

/**
 * @brief Wi-Fi station IP address format.
 */
typedef struct
{
    WIFIIPAddressType_t xType;         /**< IP address type (only eWiFiIPAddressTypeV4 is currently supported). */
    uint32_t ulAddress[ IPV6_LENGTH ]; /**< IP address in binary form, use inet_ntop/inet_pton for conversion. */
} WIFIIPAddress_t;

/**
 * @brief IP address configuration.
 */
typedef struct
{
    WIFIIPAddress_t xIPAddress; /**< IP address */
    WIFIIPAddress_t xNetMask;   /**< Network mask */
    WIFIIPAddress_t xGateway;   /**< Gateway IP address */
    WIFIIPAddress_t xDns1;      /**< First DNS server IP address */
    WIFIIPAddress_t xDns2;      /**< Second DNS server IP address */
} WIFIIPConfiguration_t;

/**
 * @brief Wi-Fi info of the connected AP.
 *
 * Structure to store info of the connected AP.
 */
typedef struct
{
    uint8_t ucSSID[ wificonfigMAX_SSID_LEN ];   /**< SSID of the Wi-Fi network (binary array, not C-string). */
    uint8_t ucSSIDLength;                       /**< SSID length. */
    uint8_t ucBSSID[ wificonfigMAX_BSSID_LEN ]; /**< BSSID of the Wi-Fi network (binary array, not C-string). */
    WIFISecurity_t xSecurity;                   /**< Wi-Fi Security. */
    uint8_t ucChannel;                          /**< Channel info. */
} WIFIConnectionInfo_t;

/**
 * @brief Wi-Fi protocol reason codes.
 */
typedef enum
{
    eWiFiReasonUnspecified,           /**< Unspecified error */
    eWiFiReasonAPNotFound,            /**< Cannot find the target AP. */
    eWiFiReasonAuthExpired,           /**< Previous auth invalid. */
    eWiFiReasonAuthLeaveBSS,          /**< Deauth leaving BSS. */
    eWiFiReasonAuthFailed,            /**< All other AUTH errors. */
    eWiFiReasonAssocExpired,          /**< Disassoc due to inactivity. */
    eWiFiReasonAssocTooMany,          /**< AP is overloaded. */
    eWiFiReasonAssocPowerCapBad,      /**< Power capability unacceptable. */
    eWiFiReasonAssocSupChanBad,       /**< Supported channel unacceptable. */
    eWiFiReasonAssocFailed,           /**< All other ASSOC errors. */
    eWiFiReasonIEInvalid,             /**< Management frame IE invalid. */
    eWiFiReasonMICFailure,            /**< MIC failure detected. */
    eWiFiReason4WayTimeout,           /**< 4WAY handshake timeout. */
    eWiFiReason4WayIEDiffer,          /**< 4WAY handshake IE error. */
    eWiFiReason4WayFailed,            /**< All other 4WAY errors. */
    eWiFiReasonAKMPInvalid,           /**< AKMP invalid. */
    eWiFiReasonPairwiseCipherInvalid, /**< Pairwise cipher invalid. */
    eWiFiReasonGroupCipherInvalid,    /**< Group cipher invalid. */
    eWiFiReasonRSNVersionInvalid,     /**< RSN version invalid. */
    eWiFiReasonRSNCapInvalid,         /**< RSN capability invalid. */
    eWiFiReasonGroupKeyUpdateTimeout, /**< Group key update timeout. */
    eWiFiReasonCipherSuiteRejected,   /**< Cipher violates security policy. */
    eWiFiReason8021XAuthFailed,       /**< 802.1X auth errors. */
    eWiFiReasonBeaconTimeout,         /**< Beacon timeout. */
    eWiFiReasonLinkFailed,            /**< All other link errors. */
    eWiFiReasonDHCPExpired,           /**< DHCP license expired. */
    eWiFiReasonDHCPFailed,            /**< All other DHCP errors. */
    eWiFiReasonMax
} WIFIReason_t;

/**
 * @brief Wi-Fi event types.
 */
typedef enum
{
    eWiFiEventReady,                 /**< Wi-Fi is initialized or was reset in the lower layer. */
    eWiFiEventScanDone,              /**< Scan is finished. */
    eWiFiEventConnected,             /**< Station is connected to the AP. */
    eWiFiEventDisconnected,          /**< Station is disconnected from the AP. */
    eWiFiEventConnectionFailed,      /**< Station connection has failed. */
    eWiFiEventIPReady,               /**< DHCP is successful. */
    eWiFiEventAPStateChanged,        /**< SoftAP state changed. */
    eWiFiEventAPStationConnected,    /**< SoftAP got a new station. */
    eWiFiEventAPStationDisconnected, /**< SoftAP lost a new station. */
    eWiFiEventWPSSuccess,            /**< WPS is completed successfully. */
    eWiFiEventWPSFailed,             /**< WPS has failed. */
    eWiFiEventWPSTimeout,            /**< WPS has timeout. */
    eWiFiEventMax
} WIFIEventType_t;

/**
 * @brief Wi-Fi event info for WI-FI ready.
 */
typedef struct
{
    uint8_t dummy;
} WiFiEventInfoReady_t;

/**
 * @brief Wi-Fi event info for scan done.
 */
typedef struct
{
    WIFIScanResult_t * pxScanResults;
    uint16_t usNumScanResults;
} WiFiEventInfoScanDone_t;

/**
 * @brief Wi-Fi event info for station connected to AP.
 */
typedef struct
{
    WIFIConnectionInfo_t xConnectionInfo;
} WiFiEventInfoConnected_t;

/**
 * @brief Wi-Fi event info for station disconnected from AP.
 */
typedef struct
{
    WIFIReason_t xReason; /**< Reason code for station disconnection. */
} WiFiEventInfoDisconnected_t;

/**
 * @brief Wi-Fi event info for station connection failure.
 */
typedef struct
{
    WIFIReason_t xReason; /**< Reason code for connection failure. */
} WiFiEventInfoConnectionFailed_t;

/**
 * @brief Wi-Fi event info for IP ready.
 */
typedef struct
{
    WIFIIPAddress_t xIPAddress; /**< Station IP address from DHCP. */
} WiFiEventInfoIPReady_t;

/**
 * @brief Wi-Fi event info for AP state change.
 */
typedef struct
{
    uint8_t ucState; /**< AP state: 0 = DOWN, 1 = UP. */
} WiFiEventInfoAPStateChanged_t;

/**
 * @brief Wi-Fi event info for AP got a connected station.
 */
typedef struct
{
    uint8_t ucMac[ wificonfigMAX_BSSID_LEN ]; /**< MAC address of connected station. */
} WiFiEventInfoAPStationConnected_t;

/**
 * @brief Wi-Fi event info for AP got a disconnected station.
 */
typedef struct
{
    uint8_t ucMac[ wificonfigMAX_BSSID_LEN ]; /**< MAC address of disconnected station. */
    WIFIReason_t xReason;                     /**< Reason code for the disconnection. */
} WiFiEventInfoAPStationDisconnected_t;

/**
 * @brief Wi-Fi event info for receiving a frame in monitor mode (or normal mode
 * with RX filter).
 */
typedef struct
{
    uint8_t * pucData; /**< Data buffer of received raw frame. */
    uint32_t ulLength; /**< Length of the raw frame. */
} WiFiEventInfoRxDone_t;

/**
 * @brief Wi-Fi event info for finishing transmitting an injection frame.
 */
typedef struct
{
    uint8_t dummy;
} WiFiEventInfoTxDone_t;

/**
 * @brief Wi-Fi combined event data structure.
 */
typedef struct
{
    WIFIEventType_t xEventType;
    union
    {
        WiFiEventInfoReady_t xReady;
        WiFiEventInfoScanDone_t xScanDone;
        WiFiEventInfoConnected_t xConnected;
        WiFiEventInfoIPReady_t xIPReady;
        WiFiEventInfoDisconnected_t xDisconnected;
        WiFiEventInfoConnectionFailed_t xConnectionFailed;
        WiFiEventInfoAPStateChanged_t xAPStateChanged;
        WiFiEventInfoAPStationConnected_t xAPStationConnected;
        WiFiEventInfoAPStationDisconnected_t xAPStationDisconnected;
        WiFiEventInfoRxDone_t xRxDone;
        WiFiEventInfoTxDone_t xTxDone;
    } xInfo;
} WIFIEvent_t;


/**
 * @brief Wi-Fi Statistic info.
 */
typedef struct
{
    uint32_t ulTxSuccessCount;  /**< Number of TX successes, 0 if unavailable */
    uint32_t ulTxRetryCount;    /**< Number of TX retries, 0 if unavailable */
    uint32_t ulTxFailCount;     /**< Number of TX failures, 0 if unavailable */
    uint32_t ulRxSuccessCount;  /**< Number of RX successes, 0 if unavailable */
    uint32_t ulRxCRCErrorCount; /**< Number of RX FCS errors, 0 if unavailable */
    uint32_t ulMICErrorCount;   /**< Number of MIC errors, 0 if unavailable */
    int8_t cNoise;              /**< Average noise level in dBm, -128 if unavailable */
    uint16_t usPhyRate;         /**< Maximum used PHY rate, 0 if unavailable */
    uint16_t usTxRate;          /**< Average used TX rate, 0 if unavailable */
    uint16_t usRxRate;          /**< Average used RX rate, 0 if unavailable */
    int8_t cRssi;               /**< Average beacon RSSI in dBm, -128 if unavailable */
    uint8_t ucBandwidth;        /**< Average used bandwidth, 0 if unavailable */
    uint8_t ucIdleTimePer;      /**< Percent of idle time, 0 if unavailable */
} WIFIStatisticInfo_t;

/**
 * @brief Wi-Fi band.
 */
typedef enum
{
    eWiFiBand2G = 0, /**< 2.4G band */
    eWiFiBand5G,     /**< 5G band */
    eWiFiBandDual,   /**< Dual band */
    eWiFiBandMax     /**< Unsupported */
} WIFIBand_t;

/**
 * @brief Wi-Fi PHY mode.
 */
typedef enum
{
    eWiFiPhy11b = 0, /**< 11B */
    eWiFiPhy11g,     /**< 11G */
    eWiFiPhy11n,     /**< 11N */
    eWiFiPhy11ac,    /**< 11AC */
    eWiFiPhy11ax,    /**< 11AX */
    eWiFiPhyMax,     /**< Unsupported */
} WIFIPhyMode_t;

/**
 * @brief Wi-Fi bandwidth.
 */
typedef enum
{
    eWiFiBW20 = 0, /**< 20MHz only */
    eWiFiBW40,     /**< Max 40MHz */
    eWiFiBW80,     /**< Max 80MHz */
    eWiFiBW160,    /**< Max 80+80 or 160MHz */
    eWiFiBWMax     /**< Unsupported */
} WIFIBandwidth_t;

/**
 * @brief Wi-Fi capabilities.
 *
 * @note Applications can use it to check whether the Wi-Fi interface can meet their
 * requirements (features like P2P, or performance as shown by xPhyMode and xBandwidth).
 */
typedef struct
{
    WIFIBand_t xBand;             /**< Supported band */
    WIFIPhyMode_t xPhyMode;       /**< Supported PHY mode */
    WIFIBandwidth_t xBandwidth;   /**< Supported bandwidth */
    uint32_t ulMaxAggr;           /**< Max aggregation length, 0 if no aggregation support */
    uint16_t usSupportedFeatures; /**< Supported features bitmap, e.g., WIFI_WPS_SUPPORTED */
} WIFICapabilityInfo_t;

/**
 * @brief Wi-Fi event handler definition.
 *
 * @param[in] xEvent - Wi-Fi event data structure.
 *
 * @return None.
 */
typedef void (* WIFIEventHandler_t)( WIFIEvent_t * xEvent );

/**
 * @brief Turns on Wi-Fi.
 *
 * This function turns on Wi-Fi module,initializes the drivers and must be called
 * before calling any other Wi-Fi API
 *
 * @return @ref eWiFiSuccess if Wi-Fi module was successfully turned on, failure code otherwise.
 */
/* @[declare_wifi_wifi_on] */
WIFIReturnCode_t WIFI_On( void );
/* @[declare_wifi_wifi_on] */

/**
 * @brief Turns off Wi-Fi.
 *
 * This function turns off the Wi-Fi module. The Wi-Fi peripheral should be put in a
 * low power or off state in this routine.
 *
 * @return @ref eWiFiSuccess if Wi-Fi module was successfully turned off, failure code otherwise.
 */
/* @[declare_wifi_wifi_off] */
WIFIReturnCode_t WIFI_Off( void );
/* @[declare_wifi_wifi_off] */

/**
 * @brief Connects to the Wi-Fi Access Point (AP) specified in the input.
 *
 * The Wi-Fi should stay connected when the same Access Point it is currently connected to
 * is specified. Otherwise, the Wi-Fi should disconnect and connect to the new Access Point
 * specified. If the new Access Point specifed has invalid parameters, then the Wi-Fi should be
 * disconnected.
 *
 * @param[in] pxNetworkParams Configuration to join AP.
 *
 * @return @ref eWiFiSuccess if connection is successful, failure code otherwise.
 *
 * **Example**
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
/* @[declare_wifi_wifi_connectap] */
WIFIReturnCode_t WIFI_ConnectAP( const WIFINetworkParams_t * const pxNetworkParams );
/* @[declare_wifi_wifi_connectap] */

/**
 * @brief Disconnects from the currently connected Access Point.
 *
 * @return @ref eWiFiSuccess if disconnection was successful or if the device is already
 * disconnected, failure code otherwise.
 */
/* @[declare_wifi_wifi_disconnect] */
WIFIReturnCode_t WIFI_Disconnect( void );
/* @[declare_wifi_wifi_disconnect] */

/**
 * @brief Resets the Wi-Fi Module.
 *
 * @return @ref eWiFiSuccess if Wi-Fi module was successfully reset, failure code otherwise.
 */
/* @[declare_wifi_wifi_reset] */
WIFIReturnCode_t WIFI_Reset( void );
/* @[declare_wifi_wifi_reset] */

/**
 * @brief Sets the Wi-Fi mode.
 *
 * @param[in] xDeviceMode - Mode of the device Station / Access Point /P2P.
 *
 * **Example**
 * @code
 * WIFIReturnCode_t xWifiStatus;
 * xWifiStatus = WIFI_SetMode(eWiFiModeStation);
 * if(xWifiStatus == eWiFiSuccess)
 * {
 *     //device Set to station mode
 * }
 * @endcode
 *
 * @return @ref eWiFiSuccess if Wi-Fi mode was set successfully, failure code otherwise.
 */
/* @[declare_wifi_wifi_setmode] */
WIFIReturnCode_t WIFI_SetMode( WIFIDeviceMode_t xDeviceMode );
/* @[declare_wifi_wifi_setmode] */

/**
 * @brief Gets the Wi-Fi mode.
 *
 * @param[out] pxDeviceMode - return mode Station / Access Point /P2P
 *
 * **Example**
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
 * @return @ref eWiFiSuccess if Wi-Fi mode was successfully retrieved, failure code otherwise.
 */
/* @[declare_wifi_wifi_getmode] */
WIFIReturnCode_t WIFI_GetMode( WIFIDeviceMode_t * pxDeviceMode );
/* @[declare_wifi_wifi_getmode] */

/**
 * @brief Add a Wi-Fi Network profile.
 *
 * Adds a Wi-fi network to the network list in Non Volatile memory.
 *
 * @param[in] pxNetworkProfile - Network profile parameters
 * @param[out] pusIndex - Network profile index in storage
 *
 * @return Index of the profile storage on success, or failure return code on failure.
 *
 * **Example**
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
/* @[declare_wifi_wifi_networkadd] */
WIFIReturnCode_t WIFI_NetworkAdd( const WIFINetworkProfile_t * const pxNetworkProfile,
                                  uint16_t * pusIndex );
/* @[declare_wifi_wifi_networkadd] */


/**
 * @brief Get a Wi-Fi network profile.
 *
 * Gets the Wi-Fi network parameters at the given index from network list in non-volatile
 * memory.
 *
 * @note The WIFINetworkProfile_t data returned must have the the SSID and Password lengths
 * specified as the length without a null terminator.
 *
 * @param[out] pxNetworkProfile - pointer to return network profile parameters
 * @param[in] usIndex - Index of the network profile,
 *                      must be between 0 to wificonfigMAX_NETWORK_PROFILES
 *
 * @return @ref eWiFiSuccess if the network profile was successfully retrieved, failure code
 * otherwise.
 *
 * @see WIFINetworkProfile_t
 *
 * **Example**
 * @code
 * WIFINetworkProfile_t xNetworkProfile = {0};
 * uint16_t usIndex = 3;  //Get profile stored at index 3.
 * WIFI_NetworkGet( &xNetworkProfile, usIndex );
 * @endcode
 */
/* @[declare_wifi_wifi_networkget] */
WIFIReturnCode_t WIFI_NetworkGet( WIFINetworkProfile_t * pxNetworkProfile,
                                  uint16_t usIndex );
/* @[declare_wifi_wifi_networkget] */

/**
 * @brief Delete a Wi-Fi Network profile.
 *
 * Deletes the Wi-Fi network profile from the network profile list at given index in
 * non-volatile memory
 *
 * @param[in] usIndex - Index of the network profile, must be between 0 to
 *                      wificonfigMAX_NETWORK_PROFILES.
 *
 *                      If wificonfigMAX_NETWORK_PROFILES is the index, then all
 *                      network profiles will be deleted.
 *
 * @return @ref eWiFiSuccess if successful, failure code otherwise. If successful, the
 * interface IP address is copied into the IP address buffer.
 *
 * **Example**
 * @code
 * uint16_t usIndex = 2; //Delete profile at index 2
 * WIFI_NetworkDelete( usIndex );
 * @endcode
 *
 */
/* @[declare_wifi_wifi_networkdelete] */
WIFIReturnCode_t WIFI_NetworkDelete( uint16_t usIndex );
/* @[declare_wifi_wifi_networkdelete] */

/**
 * @brief Ping an IP address in the network.
 *
 * @param[in] pucIPAddr IP Address array to ping.
 * @param[in] usCount Number of times to ping
 * @param[in] ulIntervalMS Interval in mili-seconds for ping operation
 *
 * @return @ref eWiFiSuccess if ping was successful, other failure code otherwise.
 */
/* @[declare_wifi_wifi_ping] */
WIFIReturnCode_t WIFI_Ping( uint8_t * pucIPAddr,
                            uint16_t usCount,
                            uint32_t ulIntervalMS );
/* @[declare_wifi_wifi_ping] */

/**
 * @brief Get IP configuration (IP address, NetworkMask, Gateway and
 *        DNS server addresses).
 *
 * @param[out] pxIPInfo - Current IP configuration.
 *
 * @return @ref eWiFiSuccess if successful and IP Address buffer has the interface's IP address,
 * failure code otherwise.
 *
 * **Example**
 * @code
 * WIFIIPConfiguration_t xIPInfo;
 * WIFI_GetIPInfo( &xIPInfo );
 * @endcode
 */
/* @[declare_wifi_wifi_getip] */
WIFIReturnCode_t WIFI_GetIPInfo( WIFIIPConfiguration_t * pxIPInfo );
/* @[declare_wifi_wifi_getip] */

/**
 * @brief Retrieves the Wi-Fi interface's MAC address.
 *
 * @param[out] pucMac MAC Address buffer sized 6 bytes.
 *
 * **Example**
 * @code
 * uint8_t ucMacAddressVal[ wificonfigMAX_BSSID_LEN ];
 * WIFI_GetMAC( &ucMacAddressVal[0] );
 * @endcode
 *
 * @return @ref eWiFiSuccess if the MAC address was successfully retrieved, failure code
 * otherwise. The returned MAC address must be 6 consecutive bytes with no delimitters.
 */
/* @[declare_wifi_wifi_getmac] */
WIFIReturnCode_t WIFI_GetMAC( uint8_t * pucMac );
/* @[declare_wifi_wifi_getmac] */

/**
 * @brief Retrieves the host IP address from a host name using DNS.
 *
 * @param[in] pcHost - Host (node) name.
 * @param[in] pucIPAddr - IP Address buffer.
 *
 * @return @ref eWiFiSuccess if the host IP address was successfully retrieved, failure code
 * otherwise.
 *
 * **Example**
 * @code
 * uint8_t ucIPAddr[ 4 ];
 * WIFI_GetHostIP( "amazon.com", &ucIPAddr[0] );
 * @endcode
 */
/* @[declare_wifi_wifi_gethostip] */
WIFIReturnCode_t WIFI_GetHostIP( char * pcHost,
                                 uint8_t * pucIPAddr );
/* @[declare_wifi_wifi_gethostip] */

/**
 * @brief Perform a Wi-Fi network Scan.
 *
 * @param[in] pxBuffer - Buffer for scan results.
 * @param[in] ucNumNetworks - Number of networks to retrieve in scan result.
 *
 * @return @ref eWiFiSuccess if the Wi-Fi network scan was successful, failure code otherwise.
 *
 * @note The input buffer will have the results of the scan.
 *
 * **Example**
 * @code
 * const uint8_t ucNumNetworks = 10; //Get 10 scan results
 * WIFIScanResult_t xScanResults[ ucNumNetworks ];
 * WIFI_Scan( xScanResults, ucNumNetworks );
 * @endcode
 */
/* @[declare_wifi_wifi_scan] */
WIFIReturnCode_t WIFI_Scan( WIFIScanResult_t * pxBuffer,
                            uint8_t ucNumNetworks );
/* @[declare_wifi_wifi_scan] */

/**
 * @brief Start SoftAP mode.
 *
 * @return @ref eWiFiSuccess if SoftAP was successfully started, failure code otherwise.
 */
/* @[declare_wifi_wifi_startap] */
WIFIReturnCode_t WIFI_StartAP( void );
/* @[declare_wifi_wifi_startap] */

/**
 * @brief Stop SoftAP mode.
 *
 * @return @ref eWiFiSuccess if the SoftAP was successfully stopped, failure code otherwise.
 */
/* @[declare_wifi_wifi_stopap] */
WIFIReturnCode_t WIFI_StopAP( void );
/* @[declare_wifi_wifi_stopap] */

/**
 * @brief Configure SoftAP.
 *
 * @param[in] pxNetworkParams - Network parameters to configure AP.
 *
 * @return @ref eWiFiSuccess if SoftAP was successfully configured, failure code otherwise.
 *
 * **Example**
 * @code
 * WIFINetworkParams_t xNetworkParams;
 * xNetworkParams.pcSSID = "SSID_Name";
 * xNetworkParams.pcPassword = "PASSWORD";
 * xNetworkParams.xSecurity = eWiFiSecurityWPA2;
 * xNetworkParams.cChannel = ChannelNum;
 * WIFI_ConfigureAP( &xNetworkParams );
 * @endcode
 */
/* @[declare_wifi_wifi_configureap] */
WIFIReturnCode_t WIFI_ConfigureAP( const WIFINetworkParams_t * const pxNetworkParams );
/* @[declare_wifi_wifi_configureap] */

/**
 * @brief Set the Wi-Fi power management mode.
 *
 * @param[in] xPMModeType - Power mode type.
 *
 * @param[in] pvOptionValue - A buffer containing the value of the option to set
 *                            depends on the mode type
 *                            example - beacon interval in sec
 *
 * @return @ref eWiFiSuccess if the power mode was successfully configured, failure code otherwise.
 */
/* @[declare_wifi_wifi_setpmmode] */
WIFIReturnCode_t WIFI_SetPMMode( WIFIPMMode_t xPMModeType,
                                 const void * pvOptionValue );
/* @[declare_wifi_wifi_setpmmode] */

/**
 * @brief Get the Wi-Fi power management mode
 *
 * @param[out] pxPMModeType - pointer to get current power mode set.
 *
 * @param[out] pvOptionValue - optional value
 *
 * @return @ref eWiFiSuccess if the power mode was successfully retrieved, failure code otherwise.
 */
/* @[declare_wifi_wifi_getpmmode] */
WIFIReturnCode_t WIFI_GetPMMode( WIFIPMMode_t * pxPMModeType,
                                 void * pvOptionValue );
/* @[declare_wifi_wifi_getpmmode] */


/**
 * @brief Register a Wi-Fi event Handler.
 *
 * @param[in] xEventType - Wi-Fi event type.
 * @param[in] xHandler - Wi-Fi event handler.
 *
 * @return eWiFiSuccess if registration is successful, failure code otherwise.
 */
/* @[declare_wifi_registerevent] */
WIFIReturnCode_t WIFI_RegisterEvent( WIFIEventType_t xEventType,
                                     WIFIEventHandler_t xHandler );
/* @[declare_wifi_registerevent] */

/**
 *
 * @brief Check if the Wi-Fi is connected and the AP configuration matches the query.
 *
 * param[in] pxNetworkParams - Network parameters to query, if NULL then just check the
 * Wi-Fi link status.
 */
/* @[declare_wifi_wifi_isconnected] */
BaseType_t WIFI_IsConnected( const WIFINetworkParams_t * pxNetworkParams );
/* @[declare_wifi_wifi_isconnected] */

/**
 * @brief Start a Wi-Fi scan.
 *
 * This is an asynchronous call, the result will be notified by an event.
 * @see WiFiEventInfoScanDone_t.
 *
 * @param[in] pxScanConfig - Parameters for scan, NULL if default scan
 * (i.e. broadcast scan on all channels).
 *
 * @return eWiFiSuccess if scan was started successfully, failure code otherwise.
 */
WIFIReturnCode_t WIFI_StartScan( WIFIScanConfig_t * pxScanConfig );

/**
 * @brief Get Wi-Fi scan results. It should be called only after scan is completed.  Scan results are sorted in decreasing rssi order.
 *
 * @param[out] pxBuffer - Handle to the READ ONLY scan results buffer.
 * @param[out] ucNumNetworks - Actual number of networks in the scan results.
 *
 * @return eWiFiSuccess if the scan results were got successfully, failure code otherwise.
 */
WIFIReturnCode_t WIFI_GetScanResults( const WIFIScanResult_t ** pxBuffer,
                                      uint16_t * ucNumNetworks );

/**
 * @brief Connect to the Wi-Fi Access Point (AP) specified in the input.
 *
 * This is an asynchronous call, the result will be notified by an event.
 * @see WiFiEventInfoConnected_t.
 *
 * The Wi-Fi should stay connected when the specified AP is the same as the
 * currently connected AP. Otherwise, the Wi-Fi should disconnect and connect
 * to the specified AP. If the specified AP has invalid parameters, the Wi-Fi
 * should be disconnected.
 *
 * @param[in] pxNetworkParams - Configuration of the targeted AP.
 *
 * @return eWiFiSuccess if connection was started successfully, failure code otherwise.
 */
WIFIReturnCode_t WIFI_StartConnectAP( const WIFINetworkParams_t * pxNetworkParams );

/**
 * @brief Wi-Fi station disconnects from AP.
 *
 * This is an asynchronous call. The result will be notified by an event.
 * @see WiFiEventInfoDisconnected_t.
 *
 * @return eWiFiSuccess if disconnection was started successfully, failure code otherwise.
 */
WIFIReturnCode_t WIFI_StartDisconnect( void );

/**
 * @brief Get Wi-Fi info of the connected AP.
 *
 * This is a synchronous call.
 *
 * @param[out] pxConnectionInfo - Wi-Fi info of the connected AP.
 *
 * @return eWiFiSuccess if connection info was got successfully, failure code otherwise.
 */
WIFIReturnCode_t WIFI_GetConnectionInfo( WIFIConnectionInfo_t * pxConnectionInfo );

/**
 * @brief Get IP configuration (IP address, NetworkMask, Gateway and
 *        DNS server addresses).
 *
 * This is a synchronous call.
 *
 * @param[out] pxIPInfo - Current IP configuration.
 *
 * @return eWiFiSuccess if connection info was got successfully, failure code otherwise.
 */
WIFIReturnCode_t WIFI_GetIPInfo( WIFIIPConfiguration_t * pxIPInfo );

/**
 * @brief Get the RSSI of the connected AP.
 *
 * This only works when the station is connected.
 *
 * @param[out] pcRSSI - RSSI of the connected AP.
 *
 * @return eWiFiSuccess if RSSI was got successfully, failure code otherwise.
 */
WIFIReturnCode_t WIFI_GetRSSI( int8_t * pcRSSI );

/**
 * @brief SoftAP mode get connected station list.
 *
 * @param[out] pxStationList - Buffer for station list, supplied by the caller.
 * @param[in, out] pcStationListSize - Input size of the list, output number of connected stations.
 *
 * @return eWiFiSuccess if stations were got successfully (manybe none),
 * failure code otherwise.
 */
WIFIReturnCode_t WIFI_GetStationList( WIFIStationInfo_t * pxStationList,
                                      uint8_t * pcStationListSize );

/**
 * @brief AP mode disconnecting a station.
 *
 * This is an asynchronous call, the result will be notified by an event.
 * @see WiFiEventInfoAPStationDisconnected_t.
 *
 * @param[in] pucMac - MAC Address of the station to be disconnected.
 *
 * @return eWiFiSuccess if disconnection was started successfully, failure code otherwise.
 */
WIFIReturnCode_t WIFI_StartDisconnectStation( uint8_t * pucMac );

/**
 * @brief Set Wi-Fi MAC addresses.
 *
 * The given MAC address will become the station MAC address. The AP MAC address
 * (i.e. BSSID) will be the same MAC address but with the local bit set.
 *
 * @param[in] pucMac - Station MAC address.
 *
 * @return eWiFiSuccess if MAC address was set successfully, failure code otherwise.
 *
 * @note On some platforms the change of MAC address can only take effect after reboot.
 */
WIFIReturnCode_t WIFI_SetMAC( uint8_t * pucMac );

/**
 * @brief Set country based configuration (including channel list, power table)
 *
 * @param[in] pcCountryCode - Country code (null-terminated string, e.g. "US", "CN". See ISO-3166).
 *
 * @return eWiFiSuccess if Country Code is set successfully, failure code otherwise.
 */
WIFIReturnCode_t WIFI_SetCountryCode( const char * pcCountryCode );

/**
 * @brief Get the currently configured country code.
 *
 * @param[out] pcCountryCode - Null-terminated string to hold the country code (see ISO-3166).
 * Must be at least 4 bytes.
 *
 * @return eWiFiSuccess if Country Code is retrieved successfully, failure code otherwise.
 */
WIFIReturnCode_t WIFI_GetCountryCode( char * pcCountryCode );

/**
 * @brief Get the Wi-Fi statistics.
 *
 * @param[out] pxStats - Structure to hold the WiFi statistics.
 *
 * @return eWiFiSuccess if statistics are retrieved successfully, failure code otherwise.
 */
WIFIReturnCode_t WIFI_GetStatistic( WIFIStatisticInfo_t * pxStats );

/**
 * @brief Get the Wi-Fi capability.
 *
 * @param[out] pxCaps - Structure to hold the Wi-Fi capabilities.
 *
 * @return eWiFiSuccess if capabilities are retrieved successfully, failure code otherwise.
 */
WIFIReturnCode_t WIFI_GetCapability( WIFICapabilityInfo_t * pxCaps );

#endif /* _AWS_WIFI_H_ */
