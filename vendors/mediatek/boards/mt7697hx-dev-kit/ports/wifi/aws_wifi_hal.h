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


/**
 * @file aws_wifi_hal.h
 * @brief Wi-Fi HAL Interface.
 */

#ifndef _AWS_WIFI_HAL_H_
#define _AWS_WIFI_HAL_H_

#include <FreeRTOS.h>
#include <aws_wifi_config.h>
#include "iot_wifi.h"

/**
 * @brief Initialize the Wi-Fi subsystem.
 *
 * @note The Wi-Fi subsystem includes the firmware, driver, supplicant, and the
 * TCP/IP stack (including DHCP client or server for respective Wi-Fi modes).
 *
 * @return eWiFiSuccess if Wi-Fi was initialized successfully, failure code otherwise.
 */
WIFIReturnCode_t WIFI_HAL_Init( void );

/**
 * @brief De-initialize the Wi-Fi subsystem.
 *
 * Disable the Wi-Fi subsystem Releases all resources allocated by WIFI_HAL_Init().
 *
 * @return eWiFiSuccess if Wi-Fi was deinitialized successfully, failure code otherwise.
 */
WIFIReturnCode_t WIFI_HAL_DeInit( void );

/**
 * @brief Turn the Wi-Fi radio on / off.
 *
 * @param[in] on - 1 to turn the Wi-Fi radio on, 0 to turn it off.
 *
 * @return eWiFiSuccess if Wi-Fi radio was set successfully, failure code otherwise.
 */
WIFIReturnCode_t WIFI_HAL_SetRadio( uint8_t on );

/**
 * @brief Set the Wi-Fi mode.
 *
 * @param[in] xDeviceMode - Mode of the device.
 *
 * @return eWiFiSuccess if Wi-Fi mode was set successfully, failure code otherwise.
 */
WIFIReturnCode_t WIFI_HAL_SetMode( WIFIDeviceMode_t xDeviceMode );

/**
 * @brief Get the Wi-Fi mode.
 *
 * @param[out] pxDeviceMode - Mode of the device.
 *
 * @return eWiFiSuccess if Wi-Fi mode was got successfully, failure code otherwise.
 */
WIFIReturnCode_t WIFI_HAL_GetMode( WIFIDeviceMode_t * pxDeviceMode);

/**
 * @brief Get the Wi-Fi MAC address.
 *
 * The MAC address is the station MAC address. The SoftAP mode MAC address (i.e. BSSID)
 * will be the same MAC address but with the local bit set when when calling WIFI_HAL_GetMode
 *
 * @param[out] pucMac - Station MAC address.
 *
 * @return eWiFiSuccess if MAC address was got successfully, failure code otherwise.
 */
WIFIReturnCode_t WIFI_HAL_GetMacAddress( uint8_t * pucMac );

/**
 * @brief Set the Wi-Fi MAC addresses.
 *
 * The given MAC address will become the station MAC address. The SoftAP mode MAC
 * address (i.e. BSSID) will be the same MAC address but with the local bit set.
 *
 * @param[in] pucMac - Station MAC address.
 *
 * @return eWiFiSuccess if MAC address was set successfully, failure code otherwise.
 *
 * @note On some platforms the change of MAC address may only take effect after
 * reboot, but once set it must be persistent.
 */
WIFIReturnCode_t WIFI_HAL_SetMacAddress( uint8_t * pucMac );

/**
 * @brief Get the IP configuration.
 *
 * @param[out] pxIPInfo - Current IP configuration.
 *
 * @return eWiFiSuccess if IP info was got successfully, failure code otherwise.
 */
WIFIReturnCode_t WIFI_HAL_GetIPInfo( WIFIIPConfiguration_t * pxIPInfo );

/**
 * @brief Start a Wi-Fi scan.
 *
 * This is an asynchronous call, the result will be notified by event eWiFiEventScanDone.
 *
 * @param[in] pxScanConfig - Parameters for the scan, NULL for default scan mode
 * (i.e. broadcast scan on all channels).
 *
 * @return eWiFiSuccess if scan was started successfully, failure code otherwise.
 *
 * @note The Wi-Fi subsystem needs to maintain an internal scan buffer. When scan
 * is done, the buffer address and number of found AP are indicated in the event.
 */
WIFIReturnCode_t WIFI_HAL_StartScan( WIFIScanConfig_t * pxScanConfig );

/**
 * @brief Stop a Wi-Fi scan.
 *
 * @return eWiFiSuccess if scan was stopped successfully, failure code otherwise.
 */
WIFIReturnCode_t WIFI_HAL_StopScan( void );

/**
 * @brief Wi-Fi station connect to the specified AP.
 *
 * This is an asynchronous call, the result will be notified by event eWiFiEventConnected
 * or eWiFiEventConnectionFailed, followed by eWiFiEventIPReady on DHCP success.
 *
 * @param[in] pxNetworkParams - Configuration of the targeted AP.
 *
 * @return eWiFiSuccess if connection was started successfully, failure code otherwise.
 */
WIFIReturnCode_t WIFI_HAL_StartConnectAP( const WIFINetworkParams_t * pxNetworkParams );

/**
 * @brief Wi-Fi station disconnect from AP.
 *
 * This is an asynchronous call, the result will be notified by event eWiFiEventDisconnected.
 *
 * @return eWiFiSuccess if disconnection was started successfully, failure code otherwise.
 *
 * @note If the Wi-Fi station is attempting to connect to an AP but not yet connected, this
 * function should stop the connection attempts.
 */
WIFIReturnCode_t WIFI_HAL_StartDisconnectAP( void );

/**
 * @brief Check if the Wi-Fi is connected and the AP configuration matches the query.
 *
 * param[in] pxNetworkParams - Network parameters to query, if NULL then just check the
 * Wi-Fi link status.
 *
 * @return pdTRUE if Wi-Fi is connected, pdFalse otherwise.
 */
BaseType_t WIFI_HAL_IsConnected( const WIFINetworkParams_t * pxNetworkParams );

/**
 * @brief Get the Wi-Fi info of the connected AP.
 *
 * This is a synchronous call.
 *
 * @param[out] pxConnectionInfo - Wi-Fi info of the connected AP.
 *
 * @return eWiFiSuccess if connection info was got successfully, failure code otherwise.
 */
WIFIReturnCode_t WIFI_HAL_GetConnectionInfo( WIFIConnectionInfo_t * pxConnectionInfo );

/**
 * @brief Get the RSSI of the connected AP.
 *
 * This only works when the station is connected.
 *
 * @param[out] pcRSSI - RSSI of the connected AP.
 *
 * @return eWiFiSuccess if RSSI is got successfully, failure code otherwise.
 */
WIFIReturnCode_t WIFI_HAL_GetRSSI( int8_t * pcRSSI );

/**
 * @brief Start the Wi-Fi SoftAP mode.
 *
 * This is an asynchronous call, the result will be notified by event
 * eWiFiEventAPStateChanged.
 *
 * @param[in] pxNetworkParams - Wi-Fi SoftAP configuration.
 *
 * @return eWiFiSuccess if SoftAP was started successfully, failure code otherwise.
 */
WIFIReturnCode_t WIFI_HAL_StartAP( const WIFINetworkParams_t * pxNetworkParams );

/**
 * @brief Stop the Wi-Fi SoftAP mode.
 *
 * This is an asynchronous call, the result will be notified by event
 * eWiFiEventAPStateChanged.
 *
 * @return eWiFiSuccess if SoftAP was stopped successfully, failure code otherwise.
 */
WIFIReturnCode_t WIFI_HAL_StopAP( void );

/**
 * @brief Get the list of stations connected to the SoftAP.
 *
 * @param[out] pxStationList - Buffer for station list.
 * @param[out] pcStationListSize - Number of connected stations.
 *
 * @return eWiFiSuccess if station list was got successfully (including 0),
 * failure code otherwise.
 */
WIFIReturnCode_t WIFI_HAL_GetStationList( WIFIStationInfo_t * pxStationList,
                                          uint8_t * puStationListSize );

/**
 * @brief Disconnect a station from the SoftAP.
 *
 * This is an asynchronous call, the result will be notified by event
 * eWiFiEventAPStationDisconnected.
 *
 * @param[in] pucMac - MAC Address of the station to be disconnected.
 *
 * @return eWiFiSuccess if disconnection was started successfully, failure code otherwise.
 */
WIFIReturnCode_t WIFI_HAL_DisconnectStation( uint8_t * pucMac );

/**
 * @brief Register a Wi-Fi event Handler.
 *
 * @param[in] xEventType - Wi-Fi event type. @see WIFIEventType_t.
 * @param[in] xHandler - Wi-Fi event handler. @see WIFIEvent_t.
 *
 * @return eWiFiSuccess if registration was successful, failure code otherwise.
 */
WIFIReturnCode_t WIFI_HAL_RegisterEvent( WIFIEventType_t xEventType,
                                         WIFIEventHandler_t xHandler );

/**
 * @brief Set the regulatory parameters (e.g. channel list, power tables) for a country.
 *
 * @param[in] pcCountryCode - Country code string (e.g. "US", "CN").
 * Use "00" for unknown but world-safe code.
 *
 * @return eWiFiSuccess if Country Code was set successfully, failure code otherwise.
 *
 * @note On some platforms the regulatory paramsters may only take effect after reboot,
 * but once set it must be persistent.
 */
WIFIReturnCode_t WIFI_HAL_SetCountryCode( const char * pcCountryCode );

/**
 * @brief Get the currently used country code.
 *
 * @param[out] pcCountryCode - String to hold the country code, must be at least 4 bytes.
 *
 * @return eWiFiSuccess if Country Code was got successfully, failure code otherwise.
 */
WIFIReturnCode_t WIFI_HAL_GetCountryCode( char * pcCountryCode );

/**
 * @brief Get the WiFi statistics.
 *
 * @param[out] pxStats - Structure to hold the WiFi statistics.
 * @return eWiFiSuccess if Country Code is retrieved successfully, failure code otherwise.
 */
WIFIReturnCode_t WIFI_HAL_GetStatistic( WIFIStatisticInfo_t * pxStats );

/**
 * @brief Get the WiFi capability.
 *
 * @param[out] pxCaps - Structure to hold the WiFi capabilities.
 * @return eWiFiSuccess if Country Code is retrieved successfully, failure code otherwise.
 */
WIFIReturnCode_t WIFI_HAL_GetCapability( WIFICapabilityInfo_t * pxCaps);

/**
 * @brief Set the Wi-Fi power management mode.
 *
 * @param[in] xPMModeType - Power mode type.
 *
 * @return eWiFiSuccess if Wi-Fi power mode was set successfully, failure code otherwise.
 */
WIFIReturnCode_t WIFI_HAL_SetPMMode( WIFIPMMode_t xPMModeType );

/**
 * @brief Get the Wi-Fi power management mode.
 *
 * @param[out] xPMModeType - Buffer to hold the current power mode.
 *
 * @return eWiFiSuccess if the power mode was successfully retrieved, failure code otherwise.
 */
WIFIReturnCode_t WIFI_HAL_GetPMMode( WIFIPMMode_t * pxPMModeType );


#endif  // _AWS_WIFI_HAL_H_

