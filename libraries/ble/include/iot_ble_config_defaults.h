/*
 * FreeRTOS BLE V2.2.0
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
#define IOT_BLE_SERVER_UUID    { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 }

/**
 *
 * @brief UUID of FreeRTOS Device Information Service.
 *
 * This 128 bit UUID used by all FreeRTOS for devices information.
 * using this UUID.
 *
 * The UUID is defined as a base + a mask. The mask is used to compute all the 128 bit UUIDs in the attribute table
 *
 */
#ifndef IOT_BLE_DEVICE_INFO_SERVICE_UUID_MASK
    #define IOT_BLE_DEVICE_INFO_SERVICE_UUID_MASK    0x32, 0xF9, 0x79, 0xE6, 0xB5, 0x83, 0xFB, 0x4E, 0xAF, 0x48, 0x68, 0x11, 0x7F, 0x8A
#endif
#define IOT_BLE_DEVICE_INFO_SERVICE_BASE_UUID        0x00, 0xFF
#define IOT_BLE_DEVICE_INFO_SERVICE_UUID             { IOT_BLE_DEVICE_INFO_SERVICE_BASE_UUID, IOT_BLE_DEVICE_INFO_SERVICE_UUID_MASK }


/**
 * @brief Set to true if user wants to send its own advertisement message.
 * If this configuration is set to 1 then user needs to implement IotBle_SetCustomAdvCb
 * hook in the application.
 */
#ifndef IOT_BLE_SET_CUSTOM_ADVERTISEMENT_MSG
    #define IOT_BLE_SET_CUSTOM_ADVERTISEMENT_MSG    ( 0 )
#endif


/**
 *
 * @brief Define the UUID that is going to be advertised.
 *
 */
#ifndef IOT_BLE_ADVERTISING_UUID
    #define IOT_BLE_ADVERTISING_UUID         IOT_BLE_DEVICE_INFO_SERVICE_UUID
    #define IOT_BLE_ADVERTISING_UUID_SIZE    16
#endif

/**
 * @brief Define the connection interval.
 *
 */
#if ( !defined( IOT_BLE_ADVERTISING_CONN_INTERVAL_MIN ) ) || ( !defined( IOT_BLE_ADVERTISING_CONN_INTERVAL_MAX ) )
    #ifdef IOT_BLE_ADVERTISING_CONN_INTERVAL_MIN
        #error "IOT_BLE_ADVERTISING_CONN_INTERVAL_MAX need to be defined"
    #endif
    #ifdef IOT_BLE_ADVERTISING_CONN_INTERVAL_MAX
        #error "IOT_BLE_ADVERTISING_CONN_INTERVAL_MIN need to be defined"
    #endif
    #define IOT_BLE_ADVERTISING_CONN_INTERVAL_MIN    0x20
    #define IOT_BLE_ADVERTISING_CONN_INTERVAL_MAX    0x40
#endif

/**< The advertising interval (in units of 0.625 ms. This value corresponds to 187.5 ms). */
#ifndef IOT_BLE_ADVERTISING_INTERVAL
    #define IOT_BLE_ADVERTISING_INTERVAL    300
#endif

/**< Min encryption key size. */
#ifndef IOT_BLE_ENCRYPT_KEY_SIZE_MIN
    #define IOT_BLE_ENCRYPT_KEY_SIZE_MIN    16
#endif


/**
 * @brief Appearance of the device when advertising.
 *
 */
#ifndef IOT_BLE_ADVERTISING_APPEARANCE
    #define IOT_BLE_ADVERTISING_APPEARANCE    0
#endif

/**
 * @brief Supported input/output of the device.
 * This define needs to be of type BTIOtypes_t.
 */
#ifndef IOT_BLE_INPUT_OUTPUT
    #define IOT_BLE_INPUT_OUTPUT    eBTIODisplayYesNo
#endif

/**
 *
 * @brief Device name that is broadcasted in advertising message. It is the truncated complete local name
 *
 */
#ifndef IOT_BLE_DEVICE_SHORT_LOCAL_NAME_SIZE
    #define IOT_BLE_DEVICE_SHORT_LOCAL_NAME_SIZE    4
#endif

/**
 *
 * @brief Device name that can be read from the name characteristic.
 *
 */
#ifndef IOT_BLE_DEVICE_COMPLETE_LOCAL_NAME
    #define IOT_BLE_DEVICE_COMPLETE_LOCAL_NAME    "BLE"
#endif

/**
 * @brief Max length BLE local device name.
 * As per BLE4.2 SPEC, device name length can be between 0 and 248 octet in length.
 *
 */
#ifndef IOT_BLE_DEVICE_LOCAL_NAME_MAX_LENGTH
    #define IOT_BLE_DEVICE_LOCAL_NAME_MAX_LENGTH    ( 248 )
#endif

/**
 * @brief Preferred MTU size for the device for a BLE connection.
 *
 * Upon new BLE connection both peers will negotiate their preferred MTU size. MTU size for the ble connection will be set to minimum
 * of the negotiated values.
 *
 */
#ifndef IOT_BLE_PREFERRED_MTU_SIZE
    #define IOT_BLE_PREFERRED_MTU_SIZE    ( 512 )
#endif

/**
 * @brief The flag to enable bonding for the device.
 *
 * By default bonding will be enabled on all device.
 */
#ifndef IOT_BLE_ENABLE_BONDING
    #define IOT_BLE_ENABLE_BONDING    ( 1 )
#endif

/**
 * @brief The flag to enable secure connection for the device.
 *
 * By default secure connection will be enable on all device.
 */
#ifndef IOT_BLE_ENABLE_SECURE_CONNECTION
    #define IOT_BLE_ENABLE_SECURE_CONNECTION    ( 1 )
#endif

/* Config if set, requires encryption to access services and characteristics */

/**
 * @brief Configuration to force encryption to access all characteristics of services.
 */
#ifndef IOT_BLE_ENCRYPTION_REQUIRED
    #define IOT_BLE_ENCRYPTION_REQUIRED    ( 1 )
#endif

/**
 * @brief Configuration to enable numeric comparison for authentication.
 *
 * If the configuration is set to 0 and #IOT_BLE_ENABLE_SECURE_CONNECTION is set to 1, then
 * device will use just works pairing.
 */
#ifndef IOT_BLE_ENABLE_NUMERIC_COMPARISON
    #define IOT_BLE_ENABLE_NUMERIC_COMPARISON    ( 1 )
#endif

/**
 * @brief Timeout in seconds used for BLE numeric comparison.
 */
#ifndef IOT_BLE_NUMERIC_COMPARISON_TIMEOUT_SEC
    #define IOT_BLE_NUMERIC_COMPARISON_TIMEOUT_SEC    ( 30 )
#endif

/**
 * @brief Maximum number of device this device can be bonded with.
 */
#ifndef IOT_BLE_MAX_BONDED_DEVICES
    #define IOT_BLE_MAX_BONDED_DEVICES    ( 5 )
#endif

#if ( IOT_BLE_ENCRYPTION_REQUIRED == 1 )
    #if ( IOT_BLE_ENABLE_NUMERIC_COMPARISON == 1 )
        #define IOT_BLE_CHAR_READ_PERM     eBTPermReadEncryptedMitm
        #define IOT_BLE_CHAR_WRITE_PERM    eBTPermWriteEncryptedMitm
    #else
        #define IOT_BLE_CHAR_READ_PERM     eBTPermReadEncrypted
        #define IOT_BLE_CHAR_WRITE_PERM    eBTPermWriteEncrypted
    #endif
#else
    #define IOT_BLE_CHAR_READ_PERM         eBTPermRead
    #define IOT_BLE_CHAR_WRITE_PERM        eBTPermWrite
#endif

/**
 * @brief This configuration flag can be used to enable or disable all FreeRTOS GATT services.
 * Configuration is useful if a custom GATT service is used instead of the default GATT services.
 */
#ifndef IOT_BLE_ENABLE_FREERTOS_GATT_SERVICES
    #define IOT_BLE_ENABLE_FREERTOS_GATT_SERVICES    ( 1 )
#endif

/**
 * @brief Set to true if user wants to add its own custom GATT services.
 */
#ifndef IOT_BLE_ADD_CUSTOM_SERVICES
    #define IOT_BLE_ADD_CUSTOM_SERVICES    ( 0 )
#endif


/**
 * @brief Flag to enable FreeRTOS Device Information Service.
 *
 * Device Information service is used by the FreeRTOS mobile SDK to fetch device related information.
 */
#if ( IOT_BLE_ENABLE_FREERTOS_GATT_SERVICES == 1 )
    #ifndef IOT_BLE_ENABLE_DEVICE_INFO_SERVICE
        #define IOT_BLE_ENABLE_DEVICE_INFO_SERVICE    ( 1 )
    #endif
#else
    #define IOT_BLE_ENABLE_DEVICE_INFO_SERVICE        ( 0 )
#endif


/**
 * @brief Enable WIFI provisioning GATT service.
 *
 * By default WIFI provisioning will be disabled. The flag will enable the GATT
 * service which communicates with FreeRTOS Mobile SDK to provision WiFi Networks.
 *
 */
#if ( IOT_BLE_ENABLE_FREERTOS_GATT_SERVICES == 1 )
    #ifndef IOT_BLE_ENABLE_WIFI_PROVISIONING
        #define IOT_BLE_ENABLE_WIFI_PROVISIONING    ( 0 )
    #endif
#else
    #define IOT_BLE_ENABLE_WIFI_PROVISIONING        ( 0 )
#endif

/**
 * @brief Enable MQTT over BLE GATT service.
 *
 * The flag will enable the GATT service which communicates with FreeRTOS Mobile SDK,
 * which acts as a proxy to forward MQTT packets to AWS IoT.
 */
#if ( IOT_BLE_ENABLE_FREERTOS_GATT_SERVICES == 1 )
    #ifndef IOT_BLE_ENABLE_MQTT
        #define IOT_BLE_ENABLE_MQTT    ( 1 )
    #endif
#else
    #define IOT_BLE_ENABLE_MQTT        ( 0 )
#endif


/**
 * @brief Flag to enable data transfer service.
 * Data transfer service can be used to exchange raw data between  FreeRTOS device and FreeRTOS Mobile SDK.
 */
#if ( IOT_BLE_ENABLE_FREERTOS_GATT_SERVICES == 1 )
    #ifndef IOT_BLE_ENABLE_DATA_TRANSFER_SERVICE
        #define IOT_BLE_ENABLE_DATA_TRANSFER_SERVICE    ( 1 )
    #endif
#else
    #define IOT_BLE_ENABLE_DATA_TRANSFER_SERVICE        ( 0 )
#endif


/**
 * @brief Controls the number of network that can be discovered for WIFI provisioning.
 *
 * A higher number will consume more stack space. The size increase in multiple of sizeof(WIFIScanResult_t).
 */
#ifndef IOT_BLE_WIFI_PROVISIONIG_MAX_SCAN_NETWORKS
    #define IOT_BLE_WIFI_PROVISIONIG_MAX_SCAN_NETWORKS    ( 10 )
#endif


/* @brief Controls the number of network that can be saved using WIFI provisioning.
 *
 * The number should be set according to amount of flash space available on the device.
 * The size increase in multiple of sizeof(WIFINetworkProfile_t).
 */
#ifndef IOT_BLE_WIFI_PROVISIONING_MAX_SAVED_NETWORKS
    #define IOT_BLE_WIFI_PROVISIONING_MAX_SAVED_NETWORKS    ( 8 )
#endif

/**
 * @brief Waiting time between checks for connection established.
 */
#ifndef IOT_BLE_MQTT_CREATE_CONNECTION_WAIT_MS
    #define IOT_BLE_MQTT_CREATE_CONNECTION_WAIT_MS    ( 1000 * IOT_BLE_NUMERIC_COMPARISON_TIMEOUT_SEC )
#endif

/**
 * @brief Number of retries if connection is not established.
 */
#ifndef IOT_BLE_MQTT_CREATE_CONNECTION_RETRY
    #define IOT_BLE_MQTT_CREATE_CONNECTION_RETRY    ( 60 )
#endif


/*
 * @brief UUID mask for data transfer services.
 */
#define IOT_BLE_DATA_TRANSFER_SERVICE_UUID_MASK                 0xC3, 0x4C, 0x04, 0x48, 0x02, 0xA0, 0xA9, 0x40, 0x2E, 0xD7, 0x6A, 0x16, 0xD7, 0xA9

/**
 * @brief Type for MQTT data transfer service.
 *  Type will be part of the service UUID.
 */
#define IOT_BLE_DATA_TRANSFER_SERVICE_TYPE_MQTT                 0x00

/**
 * @brief Type for wifi provisioning data transfer service.
 * Type will be part of the service UUID.
 */
#define IOT_BLE_DATA_TRANSFER_SERVICE_TYPE_WIFI_PROVISIONING    0x01

/**
 *@brief Size of the buffer to store pending bytes to be sent out through data transfer service.
 */
#ifndef IOT_BLE_DATA_TRANSFER_TX_BUFFER_SIZE
    #define IOT_BLE_DATA_TRANSFER_TX_BUFFER_SIZE    ( 1024 )
#endif

/**
 * @brief Initial size of the buffer used to store the data received through  data transfer service.
 * The buffer size grows exponentially ( powers of 2 ).
 */
#ifndef IOT_BLE_DATA_TRANSFER_RX_BUFFER_SIZE
    #define IOT_BLE_DATA_TRANSFER_RX_BUFFER_SIZE    ( 1024 )
#endif

#ifndef IOT_BLE_NETWORK_INTERFACE_BUFFER_SIZE
    #define IOT_BLE_NETWORK_INTERFACE_BUFFER_SIZE    ( 256U )
#endif

/**
 * @brief The  timeout in milliseconds for sending a message through data transfer service.
 */
#ifndef IOT_BLE_DATA_TRANSFER_TIMEOUT_MS
    #define IOT_BLE_DATA_TRANSFER_TIMEOUT_MS    ( 2000 )
#endif

#define IOT_BLE_MESG_ENCODER                    ( _IotSerializerCborEncoder )
#define IOT_BLE_MESG_DECODER                    ( _IotSerializerCborDecoder )

/**
 * @brief Default configuration for memory allocation of data transfer service buffers.
 */
#ifndef IotBle_MallocDataBuffer
    #define IotBle_MallocDataBuffer    malloc
#endif

#ifndef IotBle_FreeDataBuffer
    #define IotBle_FreeDataBuffer    free
#endif

/**
 * @brief Default configuration for assert statements.
 */
#ifndef IotBle_Assert
    #include <assert.h>
    #define IotBle_Assert( expression )    assert( expression )
#endif

#endif /* _IOT_BLE_CONFIG_DEFAULTS_H_ */
