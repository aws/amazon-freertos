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
 * @file aws_ble_config_defaults.h
 * @brief BLE config options.
 *
 * Ensures that the config options for BLE are set to sensible
 * default values if the user does not provide one.
 */

#ifndef _AWS_BLE_CONFIG_DEFAULTS_H_
#define _AWS_BLE_CONFIG_DEFAULTS_H_

/**
 * @brief UUID used to uniquely identify a GATT server instance
 *
 * Currently only one server instance is supported.
 *
 */
#define bleconfigSERVER_UUID                { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 }

/**
 *
 * @brief UUID of Amazon FreeRTOS Device Information Service.
 *
 * This 128 bit UUID will be advertised by all Amazon FreeRTOS devices. The companion IOS/Android SDK filters Amazon FreeRTOS devices
 * using this UUID.
 *
 */
#define bleconfigDEVICE_INFO_SERVICE_UUID   { 0x00, 0xFF, 0x32, 0xF9, 0x79, 0xE6, 0xB5, 0x83, 0xFB, 0x4E, 0xAF, 0x48, 0x68, 0x11, 0x7F, 0x8A }


/**
 *
 * @brief Device name as it appears on the BLE network.
 *
 */
#ifndef bleconfigDEVICE_NAME
#define bleconfigDEVICE_NAME                      "BLE"
#endif

/**
 * @brief Preferred MTU size for the device for a BLE connection.
 *
 * Upon new BLE connection both peers will negotiate their preferred MTU size. MTU size for the ble connection will be set to minimum
 * of the negotiated values.
 *
 */
#ifndef bleconfigPREFERRED_MTU_SIZE
#define bleconfigPREFERRED_MTU_SIZE               ( 512 )
#endif

/**
 * @brief The flag to enable bonding for the device.
 *
 * By default bonding will be enabled on all device.
 */
#ifndef bleconfigENABLE_BONDING
#define bleconfigENABLE_BONDING                   ( 1 )
#endif

/**
 * @brief The flag to enable secure connection for the device.
 *
 * By default secure connection will be enable on all device.
 */
#ifndef bleconfigENABLE_SECURE_CONNECTION
#define bleconfigENABLE_SECURE_CONNECTION         ( 1 )
#endif

/* Config if set, requires encryption to access services and characteristics */

/**
 * @brief Configuration to force encryption to access all characteristics of services.
 */
#ifndef bleconfigENCRYPTION_REQUIRED
#define bleconfigENCRYPTION_REQUIRED              ( 1 )
#endif

/**
 * @brief Configuration to enable numeric comparison for authentication.
 *
 * If the configuration is set to 0 and #bleconfigENABLE_SECURE_CONNECTION is set to 1, then
 * device will use just works pairing.
 */
#ifndef bleconfigENABLE_NUMERIC_COMPARISON
#define bleconfigENABLE_NUMERIC_COMPARISON        ( 1 )
#endif

/**
 * @brief Timeout in seconds used for BLE numeric comparison.
 */
#ifndef bleconfigNUMERIC_COMPARISON_TIMEOUT_SEC
#define bleconfigNUMERIC_COMPARISON_TIMEOUT_SEC   ( 30 )
#endif

/**
 * @brief Enable WIFI provisioning GATT service.
 *
 * By default WIFI provisioning will be disabled.
 */
#ifndef bleconfigENABLE_WIFI_PROVISIONING
#define bleconfigENABLE_WIFI_PROVISIONING         ( 0 )
#endif

/**
 * @brief Controls the number of network that can be discovered for WIFI provisionning
 *
 * A higher number will consume more stack space. The size increase in multiple of sizeof(WIFIScanResult_t)
 */
#ifndef bleconfigMAX_NETWORK
    #define bleconfigMAX_NETWORK                  ( 50 )
#endif


/**
 * @brief Enable sample GATT service
 *
 * By default the GATT demo service will be enabled.
 *
 */
#ifndef bleconfigENABLE_GATT_DEMO
#define bleconfigENABLE_GATT_DEMO                 ( 0 )
#endif

#if ( bleconfigENCRYPTION_REQUIRED == 1  )
#if ( bleconfigENABLE_NUMERIC_COMPARISON == 1)
#define bleconfigCHAR_READ_PERM             eBTPermReadEncryptedMitm
#define bleconfigCHAR_WRITE_PERM            eBTPermWriteEncryptedMitm
#else
#define bleconfigCHAR_READ_PERM             eBTPermReadEncrypted
#define bleconfigCHAR_WRITE_PERM            eBTPermWriteEncrypted
#endif
#else
#define bleconfigCHAR_READ_PERM             eBTPermRead
#define bleconfigCHAR_WRITE_PERM            eBTPermWrite
#endif

#define bleConfigENABLE_JSON_ENCODING           ( 1 )

#define bleConfigENABLE_CBOR_ENCODING           ( 0 )

#endif /* _AWS_BLE_CONFIG_DEFAULTS_H_ */
