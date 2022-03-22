/*
 * FreeRTOS V202203.00
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
 * @file aws_wifi_config.h
 * @brief WiFi configuration options.
 */

#ifndef AWS_INC_WIFI_CONFIG_H_
#define AWS_INC_WIFI_CONFIG_H_

/**
 * @brief Wifi Scan default settings
 *
 * Settings for WiFi scan operation
 */

#define wificonfigSCAN_INTERVAL_SEC      ( 10 )
#define wificonfigSCAN_DURATION_SEC      ( 3 )
#define wificonfigSCAN_HIDDEN_CHANNEL    ( 1 )        /* 1 - Enable Hidden Channel Scan
                                                       * 0 - Disable Hidden Channel Scan.*/

/**
 * @brief Simple link Task Config
 *
 * Simple link Task Configuration parameters
 */

#define wificonfigSL_TASK_STACK_SIZE            ( 2048 )   /* Simple Link task stack size.*/
#define wificonfigSL_TASK_PRIORITY              ( 9 )      /* Simple Link task stack priority.*/

/**
 * @brief Unknown security type
 */
#define wificonfigSEC_TYPE_UNKNOWN              ( 0xFF )

/**
 * @brief Max SSID length
 */
#define wificonfigMAX_SSID_LEN                  ( 32 )

/**
 * @brief Max BSSID length
 */
#define wificonfigMAX_BSSID_LEN                 ( 6 )

/**
 * @brief Max passphrase length
 *
 * This is the max Passphrase length expected by sl_WlanSet().
 */
#define wificonfigMAX_PASSPHRASE_LEN            ( 63 )

/**
 * @brief Soft Access point SSID
 */
#define wificonfigACCESS_POINT_SSID_PREFIX      ( "ConfigureMe" )

/**
 * @brief Soft Access point Passkey
 */
#define wificonfigACCESS_POINT_PASSKEY          ( "awsiotdevice" )

/**
 * @brief Soft Access point Channel
 */
#define wificonfigACCESS_POINT_CHANNEL          ( 11 )

/**
 * @brief Maximum number of network profiles stored.
 */
#define wificonfigMAX_NETWORK_PROFILES          ( 7 )

/**
 * @brief WiFi semaphore timeout
 */
#define wificonfigMAX_SEMAPHORE_WAIT_TIME_MS    ( 60000 )

/**
 * @brief Soft Access security
 * WPA2 Security, see WIFISecurity_t
 * other values are - eWiFiSecurityOpen, eWiFiSecurityWEP, eWiFiSecurityWPA
 */
#define wificonfigACCESS_POINT_SECURITY         ( eWiFiSecurityWPA2 )

#endif /* AWS_INC_WIFI_CONFIG_H_ */
