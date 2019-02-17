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
 * @file iot_ble_config_defaults.h
 * @brief BLE config options.
 *
 * Ensures that the config options for BLE are set to sensible
 * default values if the user does not provide one.
 */

#ifndef _IOT_BLE_CONFIG_DEFAULTS_H_
#define _IOT_BLE_CONFIG_DEFAULTS_H_

/**
 * @brief UUID used to uniquely identify a GATT server instance
 *
 * Currently only one server instance is supported.
 *
 */
#define IOT_BLE_SERVER_UUID                { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 }

/**
 *
 * @brief UUID of Amazon FreeRTOS Device Information Service.
 *
 * This 128 bit UUID used by all Amazon FreeRTOS for devices information.
 * using this UUID.
 *
 * The UUID is defined as a base + a mask. The mask is used to compute all the 128 bit UUIDs in the attribute table
 *
 */
 #define IOT_BLE_DEVICE_INFO_SERVICE_BASE_UUID    0x00, 0xFF
 #define IOT_BLE_DEVICE_INFO_SERVICE_UUID_MASK    0x32, 0xF9, 0x79, 0xE6, 0xB5, 0x83, 0xFB, 0x4E, 0xAF, 0x48, 0x68, 0x11, 0x7F, 0x8A
 #define IOT_BLE_DEVICE_INFO_SERVICE_UUID  {IOT_BLE_DEVICE_INFO_SERVICE_BASE_UUID, IOT_BLE_DEVICE_INFO_SERVICE_UUID_MASK }

/**
 *
 * @brief Define the UUID that is going to be advertised.
 *
 */
#if (!defined(IOT_BLE_ADVERTISING_UUID)) || ( !defined(IOT_BLE_ADVERTISING_UUID_SIZE))
	#ifdef IOT_BLE_ADVERTISING_UUID
	#error "IOT_BLE_ADVERTISING_UUID_SIZE need to be defined"
	#endif
	#ifdef IOT_BLE_ADVERTISING_UUID_SIZE
	#error "IOT_BLE_ADVERTISING_UUID need to be defined"
	#endif
#define IOT_BLE_ADVERTISING_UUID IOT_BLE_DEVICE_INFO_SERVICE_UUID
#define IOT_BLE_ADVERTISING_UUID_SIZE 16
#endif


/**
 * @brief Define the Advertising interval.
 *
 */
#if (!defined(IOT_BLE_ADVERTISING_INTERVAL_MIN)) || ( !defined(bleconfigADVERTISING_INTERVAL_MAX))
	#ifdef IOT_BLE_ADVERTISING_INTERVAL_MIN
	#error "bleconfigADVERTISING_INTERVAL_MAX need to be defined"
	#endif
	#ifdef bleconfigADVERTISING_INTERVAL_MAX
	#error "IOT_BLE_ADVERTISING_INTERVAL_MIN need to be defined"
	#endif
#define IOT_BLE_ADVERTISING_INTERVAL_MIN 0x20
#define IOT_BLE_ADVERTISING_INTERVAL_MAX 0x40
#endif

/**
 * @brief Appearance of the device when advertising.
 *
 */
#ifndef IOT_BLE_ADVERTISING_APPEARANCE
#define IOT_BLE_ADVERTISING_APPEARANCE 0
#endif

/**
 * @brief Supported input/output of the device.
 * This define needs to be of type BTIOtypes_t.
 */
#ifndef IOT_BLE_INPUT_OUTPUT
#define IOT_BLE_INPUT_OUTPUT eBTIODisplayYesNo
#endif

/**
 *
 * @brief Device name as it appears on the BLE network.
 *
 */
#ifndef IOT_BLE_DEVICE_NAME
#define IOT_BLE_DEVICE_NAME                      "BLE"
#endif

/**
 * @brief Preferred MTU size for the device for a BLE connection.
 *
 * Upon new BLE connection both peers will negotiate their preferred MTU size. MTU size for the ble connection will be set to minimum
 * of the negotiated values.
 *
 */
#ifndef IOT_BLE_PREFERRED_MTU_SIZE
#define IOT_BLE_PREFERRED_MTU_SIZE               ( 512 )
#endif

/**
 * @brief The flag to enable bonding for the device.
 *
 * By default bonding will be enabled on all device.
 */
#ifndef IOT_BLE_ENABLE_BONDING
#define IOT_BLE_ENABLE_BONDING                   ( 1 )
#endif

/**
 * @brief The flag to enable secure connection for the device.
 *
 * By default secure connection will be enable on all device.
 */
#ifndef IOT_BLE_ENABLE_SECURE_CONNECTION
#define IOT_BLE_ENABLE_SECURE_CONNECTION         ( 1 )
#endif

/* Config if set, requires encryption to access services and characteristics */

/**
 * @brief Configuration to force encryption to access all characteristics of services.
 */
#ifndef IOT_BLE_ENCRYPTION_REQUIRED
#define IOT_BLE_ENCRYPTION_REQUIRED              ( 1 )
#endif

/**
 * @brief Configuration to enable numeric comparison for authentication.
 *
 * If the configuration is set to 0 and #IOT_BLE_ENABLE_SECURE_CONNECTION is set to 1, then
 * device will use just works pairing.
 */
#ifndef IOT_BLE_ENABLE_NUMERIC_COMPARISON
#define IOT_BLE_ENABLE_NUMERIC_COMPARISON        ( 1 )
#endif

/**
 * @brief Timeout in seconds used for BLE numeric comparison.
 */
#ifndef IOT_BLE_NUMERIC_COMPARISON_TIMEOUT_SEC
#define IOT_BLE_NUMERIC_COMPARISON_TIMEOUT_SEC   ( 30 )
#endif

/**
 * @brief Enable WIFI provisioning GATT service.
 *
 * By default WIFI provisioning will be disabled.
 */
#ifndef IOT_BLE_ENABLE_WIFI_PROVISIONING
#define IOT_BLE_ENABLE_WIFI_PROVISIONING         ( 0 )
#endif

/**
 * @brief Controls the number of network that can be discovered for WIFI provisionning
 *
 * A higher number will consume more stack space. The size increase in multiple of sizeof(WIFIScanResult_t)
 */
#ifndef IOT_BLE_MAX_NETWORK
    #define IOT_BLE_MAX_NETWORK                  ( 50 )
#endif

/**
 * @brief Set to true if user wants to add its own custom services.
 */
#ifndef IOT_BLE_ADD_CUSTOM_SERVICES
    #define IOT_BLE_ADD_CUSTOM_SERVICES        ( 0 )
#endif

/**
 * @brief Maximum number of device this device can be bonded with.
 */
#ifndef IOT_BLE_MAX_BONDED_DEVICES
#define IOT_BLE_MAX_BONDED_DEVICES                ( 5 )
#endif

#if ( IOT_BLE_ENCRYPTION_REQUIRED == 1  )
#if ( IOT_BLE_ENABLE_NUMERIC_COMPARISON == 1)
#define IOT_BLE_CHAR_READ_PERM             eBTPermReadEncryptedMitm
#define IOT_BLE_CHAR_WRITE_PERM            eBTPermWriteEncryptedMitm
#else
#define IOT_BLE_CHAR_READ_PERM             eBTPermReadEncrypted
#define IOT_BLE_CHAR_WRITE_PERM            eBTPermWriteEncrypted
#endif
#else
#define IOT_BLE_CHAR_READ_PERM             eBTPermRead
#define IOT_BLE_CHAR_WRITE_PERM            eBTPermWrite
#endif

#define IOT_BLE_MESG_ENCODER           ( _AwsIotSerializerCborEncoder )
#define IOT_BLE_MESG_DECODER           ( _AwsIotSerializerCborDecoder )

#endif /* _IOT_BLE_CONFIG_DEFAULTS_H_ */
