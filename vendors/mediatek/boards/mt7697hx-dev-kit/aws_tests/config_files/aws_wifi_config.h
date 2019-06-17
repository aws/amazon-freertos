/*
 * Amazon FreeRTOS V1.1.4
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
 * @file aws_wifi_config.h
 * @brief WiFi module configuration parameters.
 */

#ifndef _AWS_WIFI_CONFIG_H_
#define _AWS_WIFI_CONFIG_H_

/**
 * @brief Max SSID length
 */
#define wificonfigMAX_SSID_LEN                ( 32 )

/**
 * @brief Max BSSID length
 */
#define wificonfigMAX_BSSID_LEN               ( 6 )

/**
 * @brief Max passphrase length
 */
#define wificonfigMAX_PASSPHRASE_LEN          ( 32 )

/**
 * @brief Soft Access point SSID
 */
#define wificonfigACCESS_POINT_SSID_PREFIX    ( "Enter SSID for Soft AP" )

/**
 * @brief Soft Access point Passkey
 */
#define wificonfigACCESS_POINT_PASSKEY        ( "Enter Password for Soft AP" )

/**
 * @brief Soft Access point Channel
 */
#define wificonfigACCESS_POINT_CHANNEL        ( 11 )

/**
 * @brief WiFi semaphore timeout
 */
#define wificonfigMAX_SEMAPHORE_WAIT_TIME_MS  ( 120000 )

/**
 * @brief Soft Access point security
 * WPA2 Security, see WIFISecurity_t
 * other values are - eWiFiSecurityOpen, eWiFiSecurityWEP, eWiFiSecurityWPA
 */
#define wificonfigACCESS_POINT_SECURITY       ( eWiFiSecurityWPA2 )

#endif /* _AWS_WIFI_CONFIG_H_ */
