/*
 * FreeRTOS V1.4.7
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
 * @file aws_wifi_config.h
 * @brief WiFi module configuration parameters.
 */

#ifndef _AWS_WIFI_CONFIG_H_
#define _AWS_WIFI_CONFIG_H_

/**
 * @brief Maximum number of sockets that can be created simultaneously.
 */
#define wificonfigMAX_SOCKETS                 ( 5 )

/**
 * @brief Maximum number of connection retries.
 */
#define wificonfigNUM_CONNECTION_RETRY        ( 3 )

/**
 * @brief Maximum number of connected station in Access Point mode.
 */
#define wificonfigMAX_CONNECTED_STATIONS      ( 4 )

/**
 * @brief Max number of network profiles stored in Non Volatile memory,
 * set to zero if not supported.
 */
#define wificonfigMAX_NETWORK_PROFILES        ( 0 )

/**
 * @brief Max SSID length
 */
#define wificonfigMAX_SSID_LEN                ( 32 )

/**
 * @brief Max BSSID length
 */
#define wificonfigMAX_BSSID_LEN               ( 6 )

/**
 * @brief Max number of WEP keys supported.
 */
#define wificonfigMAX_WEPKEYS                  ( 4 )

/**
 * @brief Max WEP key length
 */
#define wificonfigMAX_WEPKEY_LEN              ( 26 )

/**
 * @brief Max passphrase length
 */
#define wificonfigMAX_PASSPHRASE_LEN          ( 32 )

/**
 * @brief Soft Access point SSID
 */
#define wificonfigACCESS_POINT_SSID_PREFIX    ( "ConfigureMe" )

/**
 * @brief Soft Access point Passkey
 */
#define wificonfigACCESS_POINT_PASSKEY        ( "awsiotdevice" )

/**
 * @brief Soft Access point Channel
 */
#define wificonfigACCESS_POINT_CHANNEL        ( 11 )

/**
 * @brief WiFi semaphore timeout
 */
#define wificonfigMAX_SEMAPHORE_WAIT_TIME_MS  ( 60000 )

/**
 * @brief Soft Access point security
 * WPA2 Security, see WIFISecurity_t
 * other values are - eWiFiSecurityOpen, eWiFiSecurityWEP, eWiFiSecurityWPA
 */
#define wificonfigACCESS_POINT_SECURITY       ( eWiFiSecurityWPA2 )

/**
 * @brief Maximum size of WiFi AT command data buffer
 */
#define wificonfigMAX_AT_COMMAND_SIZE         ( 1024 * 3 )

/**
 * @brief Maximum size of WiFi send packet
 */
#define wificonfigMAX_SEND_SIZE               ( 1200 )

/**
 * @brief Maximum size of WiFi receive buffer
 */
#define wificonfigMAX_RECV_BUF_SIZE           ( 1024 * 4 )

/**
 * @brief Enable WiFi H/W reset
 */
#define wificonfigHW_RESET                    ( 1 )

/**
 * @brief Receive WiFi TCP packet using Passive mode
 */
#define wificonfigTCP_PASSIVE_MODE            ( 0 )

/**
 * @brief Enable UART H/W flow control
 */
#define wificonfigSERIAL_FC                   ( 0 )

/**
 * @brief Set UART baud rate
 *
 * Could be select 115200/230400/460800/921600/1152000/2304000
 * Must enable wificonfigSERIAL_FC to support this feature, or it always runs as default 115200
 */
#define wificonfigBAUD_RATE                   ( 115200 )

#endif /* _AWS_WIFI_CONFIG_H_ */
