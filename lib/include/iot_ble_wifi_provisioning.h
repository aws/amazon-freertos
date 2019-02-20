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
 * @brief GATT Service, characteristic and descriptor UUIDs used by the WiFi provisioning service.
 */
#define IOT_BLE_WIFI_PROV_SVC_UUID_MASK               0x1B, 0xE1, 0x14, 0xC6, 0x83, 0xAA, 0x9A, 0x4F, 0x9F, 0x4B, 0x87, 0xA1, 0x13, 0x31
#define IOT_BLE_WIFI_PROV_SVC_UUID                    {0x00, 0xFF, IOT_BLE_WIFI_PROV_SVC_UUID_MASK}
#define IOT_BLE_WIFI_PROV_LIST_NETWORK_CHAR_UUID      {0x01, 0xFF, IOT_BLE_WIFI_PROV_SVC_UUID_MASK}
#define IOT_BLE_WIFI_PROV_SAVE_NETWORK_CHAR_UUID      {0x02, 0xFF, IOT_BLE_WIFI_PROV_SVC_UUID_MASK}
#define IOT_BLE_WIFI_PROV_EDIT_NETWORK_CHAR_UUID      {0x03, 0xFF, IOT_BLE_WIFI_PROV_SVC_UUID_MASK}
#define IOT_BLE_WIFI_PROV_DELETE_NETWORK_CHAR_UUID    {0x04, 0xFF, IOT_BLE_WIFI_PROV_SVC_UUID_MASK}
#define IOT_BLE_WIFI_PROV_CLIENT_CHAR_CFG_UUID        ( 0x2902 )

/**
 * @brief Number of characteristics, descriptors and included services used by WiFi provisioning.
 */
#define IOT_BLE_WIFI_PROV_NUM_CHARS                   ( 4 )
#define IOT_BLE_WIFI_PROV_NUM_DESCRS                  ( 4 )
#define IOT_BLE_WIFI_PROV_NUM_INCL_SERVICES           ( 0 )

/**
 * @brief GATT characteristics used by the WiFi provisioning service.
 */
typedef enum
{
	IOT_BLE_WIFI_PROV_SVC = 0,  /**< IOT_BLE_WIFI_PROV_SVC WIFI provisioning service */
    IOT_BLE_WIFI_PROV_LIST_NETWORK_CHAR, /**< IOT_BLE_WIFI_PROV_LIST_NETWORK_CHAR Used by the GATT client to list the saved networks and scanned networks */
    IOT_BLE_WIFI_PROV_LIST_NETWORK_CHAR_DESCR, /**< IOT_BLE_WIFI_PROV_LIST_NETWORK_CHAR_DESCR Client Characteristic Configuration descriptor to enable notifications to send List Network response */
    IOT_BLE_WIFI_PROV_SAVE_NETWORK_CHAR,     /**< IOT_BLE_WIFI_PROV_SAVE_NETWORK_CHAR Used by the GATT client to provision a new WiFi network on the device */
    IOT_BLE_WIFI_PROV_SAVE_NETWORK_CHAR_DESCR,     /**< IOT_BLE_WIFI_PROV_SAVE_NETWORK_CHAR_DESCR Client Characteristic Configuration descriptor to enable notifications to send Save Network response */
    IOT_BLE_WIFI_PROV_EDIT_NETWORK_CHAR,     /**<  IOT_BLE_WIFI_PROV_EDIT_NETWORK_CHAR Used by the GATT client to change the priority order of the saved networks on the device */
    IOT_BLE_WIFI_PROV_EDIT_NETWORK_CHAR_DESCR,     /**<   IOT_BLE_WIFI_PROV_EDIT_NETWORK_CHAR_DESCR Client Characteristic Configuration descriptor to enable notifications to send Edit Network response */
	IOT_BLE_WIFI_PROV_DELETE_NETWORK_CHAR,   /**< IOT_BLE_WIFI_PROV_DELETE_NETWORK_CHAR Used by the GATT client to delete the saved WiFi network on the device */
    IOT_BLE_WIFI_PROV_DELETE_NETWORK_CHAR_DESCR,   /**< IOT_BLE_WIFI_PROV_DELETE_NETWORK_CHAR_DESCR Client Characteristic Configuration descriptor to enable notifications to send Delete Network response */
	IOT_BLE_WIFI_PROV_NB_ATTRIBUTES             /**< IOT_BLE_WIFI_PROV_NB_ATTRIBUTES Number of attributes in the enum. */
} IotBleWifiProvAttributes_t;

/**
 * @brief GATT descriptors used by the WiFi provisioning service.
 */
typedef enum
{
    IOT_BLE_WIFI_PROV_LIST_NETWORK_CHAR_CCFG = 0, /**< IOT_BLE_WIFI_PROV_LIST_NETWORK_CHAR_CCFG Client Characteristic Configuration descriptor to enable notifications to send List Network response */
    IOT_BLE_WIFI_PROV_SAVE_NETWORK_CHAR_CCFG,     /**< IOT_BLE_WIFI_PROV_SAVE_NETWORK_CHAR_CCFG Client Characteristic Configuration descriptor to enable notifications to send Save Network response */
    IOT_BLE_WIFI_PROV_EDIT_NETWORK_CHAR_CCFG,     /**<  IOT_BLE_WIFI_PROV_EDIT_NETWORK_CHAR_CCFG Client Characteristic Configuration descriptor to enable notifications to send Edit Network response */
    IOT_BLE_WIFI_PROV_DELETE_NETWORK_CHAR_CCFG,   /**< IOT_BLE_WIFI_PROV_DELETE_NETWORK_CHAR_CCFG Client Characteristic Configuration descriptor to enable notifications to send Delete Network response */
    IOT_BLE_WIFI_PROV_MAX_DESCR                /**< IOT_BLE_WIFI_PROV_MAX_DESCR */
} IotBleWifiProvDescriptor_t;


/**
 * @brief Parameters used in WIFI provisioning message serialization. 
 */
#define IOT_BLE_WIFI_PROV_MAX_NETWORKS_KEY    "h"
#define IOT_BLE_WIFI_PROV_SCAN_TIMEOUT_KEY    "t"
#define IOT_BLE_WIFI_PROV_KEY_MGMT_KEY        "q"
#define IOT_BLE_WIFI_PROV_SSID_KEY            "r"
#define IOT_BLE_WIFI_PROV_BSSID_KEY           "b"
#define IOT_BLE_WIFI_PROV_RSSI_KEY            "p"
#define IOT_BLE_WIFI_PROV_PSK_KEY             "m"
#define IOT_BLE_WIFI_PROV_STATUS_KEY          "s"
#define IOT_BLE_WIFI_PROV_HIDDEN_KEY          "f"
#define IOT_BLE_WIFI_PROV_CONNECTED_KEY       "e"
#define IOT_BLE_WIFI_PROV_INDEX_KEY           "g"
#define IOT_BLE_WIFI_PROV_NEWINDEX_KEY        "j"
#define IOT_BLE_WIFI_PROV_CONNECT_KEY         "y"

#define IOT_BLE_WIFI_PROV_NUM_NETWORK_INFO_MESG_PARAMS  ( 8 )

#define IOT_BLE_WIFI_PROV_NUM_STATUS_MESG_PARAMS        ( 1 )

#define IOT_BLE_WIFI_PROV_DEFAULT_ALWAYS_CONNECT        ( true )

/**
 * @brief List Network request sent by the GATT client to list saved and scanned networks.
 */
typedef struct IotBleListNetworkRequest
{
    int16_t maxNetworks; /**< Max Networks to scan in one request */
    int16_t timeoutMs;   /**< Timeout in MS for scanning */
} IotBleListNetworkRequest_t;

/**
 * @brief Sent by the GATT client to provision a new WiFi network.
 */
typedef struct IotBleAddNetworkRequest
{
    WIFINetworkProfile_t network; /**< The configuration for the new WIFI network */
    int16_t savedIdx;             /**< Index if its an already saved WIFI network in the flash */
    bool connect;
} IotBleAddNetworkRequest_t;

/**
 * @brief Sent by the GATT client to change the saved WiFi networks priority order.
 */
typedef struct IotBleEditNetworkRequest
{
    int16_t curIdx; /**< Current priority of the saved WiFi network */
    int16_t newIdx; /**< New priority of the saved WiFi network */
} IotBleEditNetworkRequest_t;

/**
 * @brief Sent by the GATT client to delete a saved WIFI network from flash.
 *
 */
typedef struct IotBleDeleteNetworkRequest
{
    int16_t idx; /**< Index/priority of the saved WiFi network */
} IotBleDeleteNetworkRequest_t;

/**
 * @brief Response type used to send a WIFI network
 */

typedef struct IotBleWifiNetworkInfo
{
    WIFIReturnCode_t status;
	const char* pSSID;
	size_t SSIDLength;
	const uint8_t* pBSSID;
	size_t BSSIDLength;
	WIFISecurity_t security;
	int8_t RSSI;
	bool hidden;
	bool connected;
    int32_t savedIdx;
} IotBleWifiNetworkInfo_t;

/**
 * @brief Events Used by the WIFI provisioning service.
 */
typedef enum
{
    IOT_BLE_WIFI_PROV_CONNECT = 0x01,   /**< IOT_BLE_WIFI_PROV_CONNECT Set When WiFi provisioning service connects to one of the saved networks in the flash *///!< IOT_BLE_WIFI_PROV_CONNECT
    IOT_BLE_WIFI_PROV_CONNECTED = 0x02, /**< IOT_BLE_WIFI_PROV_CONNECTED Set when successfully connected to a WiFi Network */                                  //!< IOT_BLE_WIFI_PROV_CONNECTED
    IOT_BLE_WIFI_PROV_DELETED = 0x4,   /**<  eWIFIProvStopped Set when WiFi provisioning is deleted */                                               //!< IOT_BLE_WIFI_PROV_DELETED
    IOT_BLE_WIFI_PROV_FAILED = 0x8     /**< IOT_BLE_WIFI_PROV_FAILED Set When WiFi provisioning failed */                                                     //!< IOT_BLE_WIFI_PROV_FAILED
} IotBleWifiProvEvent_t;

#define ALL_EVENTS    (  IOT_BLE_WIFI_PROV_CONNECT | IOT_BLE_WIFI_PROV_CONNECTED | IOT_BLE_WIFI_PROV_DELETED | IOT_BLE_WIFI_PROV_FAILED )

/**
 * @brief Structure used for WiFi provisioning service.
 */
typedef struct IotBleWifiProvService
{
    struct BTService * pGattService;
    uint16_t notifyClientEnabled;
    uint16_t BLEConnId;
    SemaphoreHandle_t lock;
    uint16_t numNetworks;
    int16_t connectedIdx;
    BaseType_t init;
} IotBleWifiProvService_t;

#define IOT_BLE_WIFI_PROV_IS_SUCCESS( btstatus )    ( ( btstatus ) == eBTStatusSuccess )


#define IOT_BLE_WIFI_PROV_INVALID_NETWORK_RSSI      ( -100 )


#define IOT_BLE_WIFI_PROV_INVALID_NETWORK_INDEX     ( -1 )


/**
 * @brief Maximum number of WiFi networks that can be provisioned
 */
#define IOT_BLE_WIFI_PROV_MAX_SAVED_NETWORKS    ( 8 )

/**
 * @brief Delay between connecting to the saved list of WiFi networks
 */
#define IOT_BLE_WIFI_PROV_NETWORKS_CONNECTION_INTERVAL_MS ( 1000 )

/**
 * @brief Base priority for all the tasks
 */
#define IOT_BLE_WIFI_PROV_TASK_PRIORITY_BASE      ( tskIDLE_PRIORITY )

/**
 * @brief Priority for the task to list all the WiFi networks.
 */
#define IOT_BLE_WIFI_PROV_TASK_PRIORITY  ( IOT_BLE_WIFI_PROV_TASK_PRIORITY_BASE + 1 )

/**
 * @brief Priority for the task to modify WiFi networks.
 */
#define IOT_BLE_WIFI_PROV_MODIFY_NETWORK_TASK_PRIORITY  ( IOT_BLE_WIFI_PROV_TASK_PRIORITY_BASE + 2 )


/**
 * @brief Initialize the wifi provisioning service.
 *
 * Creates GATT service and characteristics required for WiFi provisioning over BLE. User needs to call it once
 * before starting the BLE advertisement.
 *
 * @return pdTRUE if the initialization succeeded.
 *         pdFALSE if the initialization failed.
 */
BaseType_t IotBleWifiProv_Init( void );

/**
 * @brief Starts WiFi provisioning service.
 * Starts the BLE service to provision new WIFI networks. Registers a callback invoked whenever a new network is provisioned.
 *
 *
 * @return pdTRUE if successfully started
 *         pdFALSE if start failed
 */
BaseType_t IotBleWifiProv_Start( void );

/**
 * @brief Gets the total number of provisioned networks.
 *
 *
 * @return Number of provisioned networks
 */
uint16_t IotBleWifiProv_GetNumNetworks( void );

/**
 * @brief Connects to one of the saved networks in priority order.
 *
 * Example usage of this API to connect to provisioned networks in priority order:
   <pre>

   uint16_t numNetworks =  IotBleWifiProv_GetNumNetworks(); //Gets the number of provisioned networks
   uint16_t priority;
   TickType_t  xNetworkConnectionDelay = pdMS_TO_TICKS( 1000 ); //1 Second delay

   for( priority = 0; priority < numNetworks; priority++ ) //Priority is always in descending order/0 being highest priority.
   {
        xRet = IotBleWifiProv_Connect( priority );
        if( xRet == pdTRUE )
        {
            break;
        }
        vTaskDelay( xNetworkConnectionDelay );
   }
   </pre>
 *
 * @return Returns the connected network index in the flash.
 */
BaseType_t IotBleWifiProv_Connect( uint16_t networkIndex );


/**
 * @brief Erase all wifi networks.
 *
 * @return pdTRUE if success.
 */

BaseType_t IotBleWifiProv_EraseAllNetworks( void );


/**
 * @brief Stops WIFI provisioning BLE service
 *
 * Stops GATT service, Pauses the background task which connects to saved WiFi networks.
 *
 * @return pdTRUE if GATT service is stopped successufly
 */
BaseType_t IotBleWifiProv_Stop( void );

/**
 * @brief Tear down WIFI provisioning service
 *
 * Deletes the GATT service and deletes the background task which connects to saved WiFi networks.
 * @return pdTRUE if delete succeeds, pdFALSE otherwise
 */
BaseType_t IotBleWifiProv_Delete( void );


#endif /* _AWS_BLE_WIFI_PROVISIONING_H_ */
