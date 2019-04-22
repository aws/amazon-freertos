/*
 * Amazon FreeRTOS
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
 * @file iot_ble_wifi_provisioning.h
 * @brief WiFi provisioning Gatt service.
 */

#ifndef IOT_BLE_WIFI_PROVISIONING_H_
#define IOT_BLE_WIFI_PROVISIONING_H_

#include "FreeRTOS.h"
#include "FreeRTOSConfig.h"
#include "task.h"
#include "semphr.h"
#include "iot_ble.h"
#include "aws_wifi.h"



/**
 * @ingroup ble_datatypes_enums
 * @brief GATT characteristics used by the WiFi provisioning service.
 */
typedef enum
{
    IOT_BLE_WIFI_PROV_SVC = 0,                   /**< IOT_BLE_WIFI_PROV_SVC WIFI provisioning service */
    IOT_BLE_WIFI_PROV_LIST_NETWORK_CHAR,         /**< IOT_BLE_WIFI_PROV_LIST_NETWORK_CHAR Used by the GATT client to list the saved networks and scanned networks */
    IOT_BLE_WIFI_PROV_LIST_NETWORK_CHAR_DESCR,   /**< IOT_BLE_WIFI_PROV_LIST_NETWORK_CHAR_DESCR Client Characteristic Configuration descriptor to enable notifications to send List Network response */
    IOT_BLE_WIFI_PROV_SAVE_NETWORK_CHAR,         /**< IOT_BLE_WIFI_PROV_SAVE_NETWORK_CHAR Used by the GATT client to provision a new WiFi network on the device */
    IOT_BLE_WIFI_PROV_SAVE_NETWORK_CHAR_DESCR,   /**< IOT_BLE_WIFI_PROV_SAVE_NETWORK_CHAR_DESCR Client Characteristic Configuration descriptor to enable notifications to send Save Network response */
    IOT_BLE_WIFI_PROV_EDIT_NETWORK_CHAR,         /**<  IOT_BLE_WIFI_PROV_EDIT_NETWORK_CHAR Used by the GATT client to change the priority order of the saved networks on the device */
    IOT_BLE_WIFI_PROV_EDIT_NETWORK_CHAR_DESCR,   /**<   IOT_BLE_WIFI_PROV_EDIT_NETWORK_CHAR_DESCR Client Characteristic Configuration descriptor to enable notifications to send Edit Network response */
    IOT_BLE_WIFI_PROV_DELETE_NETWORK_CHAR,       /**< IOT_BLE_WIFI_PROV_DELETE_NETWORK_CHAR Used by the GATT client to delete the saved WiFi network on the device */
    IOT_BLE_WIFI_PROV_DELETE_NETWORK_CHAR_DESCR, /**< IOT_BLE_WIFI_PROV_DELETE_NETWORK_CHAR_DESCR Client Characteristic Configuration descriptor to enable notifications to send Delete Network response */
    IOT_BLE_WIFI_PROV_NB_ATTRIBUTES              /**< IOT_BLE_WIFI_PROV_NB_ATTRIBUTES Number of attributes in the enum. */
} IotBleWifiProvAttributes_t;


/**
 * @ingroup ble_datatypes_structs
 * @brief List Network request sent by the GATT client to list saved and scanned networks.
 */
typedef struct IotBleListNetworkRequest
{
    int16_t maxNetworks; /**< Max Networks to scan in one request */
    int16_t timeoutMs;   /**< Timeout in MS for scanning */
} IotBleListNetworkRequest_t;

/**
 * @ingroup ble_datatypes_structs
 * @brief Sent by the GATT client to provision a new WiFi network.
 */
typedef struct IotBleAddNetworkRequest
{
    WIFINetworkProfile_t network; /**< The configuration for the new WIFI network. */
    int16_t savedIdx;             /**< Index if its an already saved WIFI network in the flash. */
    bool connect;                 /**< A flag to indicate whether to connect or not. */
} IotBleAddNetworkRequest_t;

/**
 * @ingroup ble_datatypes_structs
 * @brief Sent by the GATT client to change the saved WiFi networks priority order.
 */
typedef struct IotBleEditNetworkRequest
{
    int16_t curIdx; /**< Current priority of the saved WiFi network. */
    int16_t newIdx; /**< New priority of the saved WiFi network. */
} IotBleEditNetworkRequest_t;

/**
 * @ingroup ble_datatypes_structs
 * @brief Sent by the GATT client to delete a saved WIFI network from flash.
 *
 */
typedef struct IotBleDeleteNetworkRequest
{
    int16_t idx; /**< Index/priority of the saved WiFi network */
} IotBleDeleteNetworkRequest_t;

/**
 * @ingroup ble_datatypes_structs
 * @brief Response type used to send a WIFI network
 */
typedef struct IotBleWifiNetworkInfo
{
    const char * pSSID;      /**< @brief The SSID of the WiFi network. */
    size_t SSIDLength;       /**< @brief The SSID length in bytes. */
    const uint8_t * pBSSID;  /**< BSSID of the Wi-Fi network. */
    size_t BSSIDLength;      /**< BSSID of the Wi-Fi network. */
    int32_t savedIdx;        /**< The index at which to save the network. */
    int8_t RSSI;             /**< The signal strength of the Wi-Fi network. */
    WIFISecurity_t security; /**< The security type of the Wi-Fi network. */
    WIFIReturnCode_t status; /**< The status of the Wi-Fi network. */
    bool hidden;             /**< A flag to signify whether this is an hidden network. */
    bool connected;          /**< A flag to signify whether this network is connected. */
} IotBleWifiNetworkInfo_t;

/**
 * @ingroup ble_datatypes_enums
 * @brief Events Used by the WIFI provisioning service.
 */
typedef enum
{
    IOT_BLE_WIFI_PROV_CONNECT = 0x01,   /**< IOT_BLE_WIFI_PROV_CONNECT Set When WiFi provisioning service connects to one of the saved networks in the flash */ /*!< IOT_BLE_WIFI_PROV_CONNECT */
    IOT_BLE_WIFI_PROV_CONNECTED = 0x02, /**< IOT_BLE_WIFI_PROV_CONNECTED Set when successfully connected to a WiFi Network */                                 /*!< IOT_BLE_WIFI_PROV_CONNECTED */
    IOT_BLE_WIFI_PROV_DELETED = 0x4,    /**<  eWIFIProvStopped Set when WiFi provisioning is deleted */                                                          /*!< IOT_BLE_WIFI_PROV_DELETED */
    IOT_BLE_WIFI_PROV_FAILED = 0x8      /**< IOT_BLE_WIFI_PROV_FAILED Set When WiFi provisioning failed */                                                         /*!< IOT_BLE_WIFI_PROV_FAILED */
} IotBleWifiProvEvent_t;

/**
 * @ingroup ble_datatypes_structs
 * @brief Structure used for WiFi provisioning service.
 */
typedef struct IotBleWifiProvService
{
    void * pBleConnection;           /**< A pointer to the ble connection object. */
    SemaphoreHandle_t lock;          /**< A lock to protect the WiFi provisioning service. */
    uint16_t numNetworks;            /**< The number of networks. */
    int16_t connectedIdx;            /**< The index of the network that is connected. */
    bool init;                       /**< A flag to indicate if the service has been initialized. */

    IotBleListNetworkRequest_t   listNetworkRequest;
    IotBleAddNetworkRequest_t    addNetworkRequest;
    IotBleEditNetworkRequest_t   editNetworkRequest;
    IotBleDeleteNetworkRequest_t deleteNetworkRequest;

} IotBleWifiProvService_t;

/**
 * @functionspage{iotblewifiprov,GATT Service for WiFi Provisioning,WiFi Provisioning}
 * - @functionname{iotblewifiprov_function_init}
 * - @functionname{iotblewifiprov_function_start}
 * - @functionname{iotblewifiprov_function_stop}
 * - @functionname{iotblewifiprov_function_getnumnetworks}
 * - @functionname{iotblewifiprov_function_connect}
 * - @functionname{iotblewifiprov_function_eraseallnetworks}
 * - @functionname{iotblewifiprov_function_delete}
 */

/**
 * @functionpage{IotBleWifiProv_Init,iotblewifiprov,init}
 * @functionpage{IotBleWifiProv_Start,iotblewifiprov,start}
 * @functionpage{IotBleWifiProv_Stop,iotblewifiprov,stop}
 * @functionpage{IotBleWifiProv_GetNumNetworks,iotblewifiprov,getnumnetworks}
 * @functionpage{IotBleWifiProv_Connect,iotblewifiprov,connect}
 * @functionpage{IotBleWifiProv_EraseAllNetworks,iotblewifiprov,eraseallnetworks}
 * @functionpage{IotBleWifiProv_Delete,iotblewifiprov,delete}
 */

/**
 * @brief Initialize the wifi provisioning service.
 *
 * Creates GATT service and characteristics required for WiFi provisioning over BLE. User needs to call it once
 * before starting the BLE advertisement.
 *
 * @return true if the initialization succeeded.
 *         false if the initialization failed.
 */
/* @[declare_iotblewifiprov_init] */
bool IotBleWifiProv_Init( void );
/* @[declare_iotblewifiprov_init] */

/**
 * @brief Starts WiFi provisioning service.
 * Starts the BLE service to provision new WIFI networks. Registers a callback invoked whenever a new network is provisioned.
 *
 *
 * @return true if successfully started
 *         false if start failed
 */
/* @[declare_iotblewifiprov_start] */
bool IotBleWifiProv_Start( void );
/* @[declare_iotblewifiprov_start] */

/**
 * @brief Stops WIFI provisioning BLE service
 *
 * Stops GATT service, Pauses the background task which connects to saved WiFi networks.
 *
 * @return true if GATT service is stopped successufly, false otherwise.
 */
/* @[declare_iotblewifiprov_stop] */
bool IotBleWifiProv_Stop( void );
/* @[declare_iotblewifiprov_stop] */

/**
 * @brief Gets the total number of provisioned networks.
 *
 *
 * @return Number of provisioned networks
 */
/* @[declare_iotblewifiprov_getnumnetworks] */
uint32_t IotBleWifiProv_GetNumNetworks( void );
/* @[declare_iotblewifiprov_getnumnetworks] */

/**
 * @brief Connects to one of the saved networks in priority order.
 *
 * Example usage of this API to connect to provisioned networks in priority order:
 * <pre>
 *
 * uint32_t numNetworks =  IotBleWifiProv_GetNumNetworks(); //Gets the number of provisioned networks
 * uint16_t priority;
 * TickType_t  xNetworkConnectionDelay = pdMS_TO_TICKS( 1000 ); //1 Second delay
 *
 * for( priority = 0; priority < numNetworks; priority++ ) //Priority is always in descending order/0 being highest priority.
 * {
 *      bool ret = IotBleWifiProv_Connect( priority );
 *
 *      if( ret == true )
 *      {
 *          break;
 *      }
 *
 *      vTaskDelay( xNetworkConnectionDelay );
 * }
 * </pre>
 *
 * @return Returns the connected network index in the flash.
 */
/* @[declare_iotblewifiprov_connect] */
bool IotBleWifiProv_Connect( uint32_t networkIndex );
/* @[declare_iotblewifiprov_connect] */

/**
 * @brief Erase all wifi networks.
 *
 * @return true if success, false otherwise.
 */
/* @[declare_iotblewifiprov_eraseallnetworks] */
bool IotBleWifiProv_EraseAllNetworks( void );
/* @[declare_iotblewifiprov_eraseallnetworks] */

/**
 * @brief Tear down WIFI provisioning service
 *
 * Deletes the GATT service and deletes the background task which connects to saved WiFi networks.
 * @return true if delete succeeds, false otherwise.
 */
/* @[declare_iotblewifiprov_delete] */
bool IotBleWifiProv_Delete( void );
/* @[declare_iotblewifiprov_delete] */

#endif /* _AWS_BLE_WIFI_PROVISIONING_H_ */
