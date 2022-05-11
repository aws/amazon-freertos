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
 * @file iot_ble_wifi_provisioning.h
 * @brief WiFi provisioning Gatt service.
 */

#ifndef IOT_BLE_WIFI_PROVISIONING_H_
#define IOT_BLE_WIFI_PROVISIONING_H_

#include "iot_ble.h"
#include "iot_wifi.h"

/**
 * @ingroup ble_data_types_enums
 * @brief This enumeration defines the different types of request processed by the WiFi provisioning library.
 */
typedef enum
{
    IotBleWiFiProvRequestInvalid = 0,       /**< Type used to denote an invalid request. */
    IotBleWiFiProvRequestListNetwork = 1,   /**< Request sent from a WiFi provisioning app to the device to list the access points. */
    IotBleWiFiProvRequestAddNetwork = 2,    /**< Request sent from a WiFi provisioning app to the device to provision an acess point. */
    IotBleWiFiProvRequestEditNetwork = 3,   /**< Request sent from a WiFi provisioning app to the device to change access point priority. */
    IotBleWiFiProvRequestDeleteNetwork = 4, /**< Request sent from a WiFi provisioning app to the device to delete an access point. */
    IotBleWiFiProvRequestStop = 5           /**< Request sent from an application task to stop WiFi provisioning loop. */
} IotBleWiFiProvRequest_t;

/**
 * @ingroup ble_datatypes_structs
 * @brief Defines the list wifi networks request message structure sent from the provisioining app to the device.
 * The request is used to list already provisioned networks as well as to scan nearby access points.
 */
typedef struct
{
    int16_t scanSize;      /**< Max WiFi access points to scan in one request */
    int16_t scanTimeoutMS; /**< Timeout in milliseconds for scanning */
} IotBleWifiProvListNetworksRequest_t;

/**
 * @ingroup ble_datatypes_structs
 * @brief Defines add wifi network request message structure sent from the provisioining app to the device.
 * The request is used to provision a new access point with the credentials or to connect to an already provisioned
 * access point. Inorder to connect to an already provisioned access point, a valid accessPointStoredIndex needs
 * to be provided. Setting flag connectToAccessPoint to true results in the  device first connecting to access point successfully before
 * provisioning it in the flash.
 */
typedef struct
{
    union
    {
        WIFINetworkProfile_t network; /**< The configuration for the new WIFI access point. */
        int16_t index;                /**< Prirority index of an already provisioned WiFi access point. */
    } info;
    bool isProvisioned : 1;           /**< true if the newtwork is already provisioned, a valid pirority index should be provided. */
    bool shouldConnect : 1;           /**< true if the access point needs to be connected before provisioning. */
} IotBleWifiProvAddNetworkRequest_t;

/**
 * @ingroup ble_datatypes_structs
 * @brief Defines edit wifi network request message structure sent from provisioning app to the device. The request
 * is used to change the priority index for a provisioned access point. Priority index ranges from 0 (Max priority) to
 * wificonfigMAX_NETWORK_PROFILES - 1 (Minimum priority).
 */
typedef struct
{
    int16_t currentPriorityIndex; /**< Current Priority index of the provisioned WiFi network.*/
    int16_t newPriorityIndex;     /**< New priority index of the provisioned WiFi network. */
} IotBleWifiProvEditNetworkRequest_t;

/**
 * @ingroup ble_datatypes_structs
 * @brief Defines delete access point request message structure sent from provisioning app to the device. The request
 * is used to delete a provisioned access point at an index. Index ranges from 0 (Max priority) to
 * wificonfigMAX_NETWORK_PROFILES - 1 (Minimum priority)
 */
typedef struct
{
    int16_t priorityIndex; /**< Priority index of the provisioned access point to be deleted. */
} IotBleWifiProvDeleteNetworkRequest_t;

/**
 * @ingroup ble_datatypes_structs
 * @brief Defines the structure used to hold a scanned or saved network information.
 */
typedef struct
{
    union
    {
        WIFIScanResult_t * pScannedNetwork;   /**< Details of the scanned Network. */
        WIFINetworkProfile_t * pSavedNetwork; /**< Details of the provisioned Network. */
    } info;
    uint16_t index;                           /**< The index (priority) at which network is provisioned. */
    bool isSavedNetwork : 1;                  /**< A flag to signify whether this network is a saved network or scanned network. */
    bool isConnected : 1;                     /**< A flag to signify whether this network is connected. */
    bool isHidden : 1;                        /**< A flag to signify whether this is an hidden network. */
    bool isLast : 1;                          /**< A flag to signify if this is last in the list of scanned or saved networks. */
} IotBleWifiProvNetworkInfo_t;

/**
 * @ingroup ble_datatypes_structs
 * @brief Defines the structure used to hold the wifi provisioning response.
 */
typedef struct IotBleWifiProvResponse
{
    IotBleWiFiProvRequest_t requestType;     /** Request type for which this response is sent. */
    WIFIReturnCode_t status;                 /**< The status of the response. */
    IotBleWifiProvNetworkInfo_t networkInfo; /**< Network info sent as part of response for list request. */
    bool statusOnly : 1;                     /**< A flag to signify if its a status only response. */
} IotBleWifiProvResponse_t;

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


typedef struct
{
    bool ( * getRequestType )( const uint8_t * pMessage,
                               size_t length,
                               IotBleWiFiProvRequest_t * pRequeustType );

    bool ( * deserializeListNetworkRequest )( const uint8_t * pData,
                                              size_t length,
                                              IotBleWifiProvListNetworksRequest_t * pListNetworksRequest );

    bool ( * deserializeAddNetworkRequest )( const uint8_t * pData,
                                             size_t length,
                                             IotBleWifiProvAddNetworkRequest_t * pAddNetworkRequest );

    bool ( * deserializeEditNetworkRequest )( const uint8_t * pData,
                                              size_t length,
                                              IotBleWifiProvEditNetworkRequest_t * pEditNetworkRequest );

    bool ( * deserializeDeleteNetworkRequest )( const uint8_t * pData,
                                                size_t length,
                                                IotBleWifiProvDeleteNetworkRequest_t * pDeleteNetworkRequest );

    bool ( * getSerializedSizeForResponse )( const IotBleWifiProvResponse_t * pResponse,
                                             size_t * length );

    bool ( * serializeResponse )( const IotBleWifiProvResponse_t * pResponse,
                                  uint8_t * pBuffer,
                                  size_t length );
} IotBleWifiProvSerializer_t;

/**
 * @functions_page{iotblewifiprov, WiFi Provisioning}
 * @functions_brief{GATT Service for WiFi Provisioning}
 * - @function_name{iotblewifiprov_function_init}
 * @function_brief{iotblewifiprov_function_init}
 * - @function_name{iotblewifiprov_function_start}
 * @function_brief{iotblewifiprov_function_start}
 * - @function_name{iotblewifiprov_function_stop}
 * @function_brief{iotblewifiprov_function_stop}
 * - @function_name{iotblewifiprov_function_getnumnetworks}
 * @function_brief{iotblewifiprov_function_getnumnetworks}
 * - @function_name{iotblewifiprov_function_connect}
 * @function_brief{iotblewifiprov_function_connect}
 * - @function_name{iotblewifiprov_function_eraseallnetworks}
 * @function_brief{iotblewifiprov_function_eraseallnetworks}
 * - @function_name{iotblewifiprov_function_delete}
 * @function_brief{iotblewifiprov_function_delete}
 */

/**
 * @function_page{IotBleWifiProv_Init,iotblewifiprov,init}
 * @function_snippet{iotblewifiprov,init,this}
 * @copydoc IotBleWifiProv_Init
 * @function_page{IotBleWifiProv_GetNumNetworks,iotblewifiprov,getnumnetworks}
 * @function_snippet{iotblewifiprov,getnumnetworks,this}
 * @copydoc IotBleWifiProv_GetNumNetworks
 * @function_page{IotBleWifiProv_Connect,iotblewifiprov,connect}
 * @function_snippet{iotblewifiprov,connect,this}
 * @copydoc IotBleWifiProv_Connect
 * @function_page{IotBleWifiProv_EraseAllNetworks,iotblewifiprov,eraseallnetworks}
 * @function_snippet{iotblewifiprov,eraseallnetworks,this}
 * @copydoc IotBleWifiProv_EraseAllNetworks
 * @function_page{IotBleWifiProv_Delete,iotblewifiprov,delete}
 * @function_snippet{iotblewifiprov,delete,this}
 * @copydoc IotBleWifiProv_Delete
 */

/**
 * @brief Initialize wifi provisioning over BLE service.
 *
 * Creates necessary data structures for the service. Opens ble data transfer channel and listens
 * for incoming messages from the channel.
 *
 * @return true if the initialization succeeded.
 *         false if the initialization failed.
 */
/* @[declare_iotblewifiprov_init] */
bool IotBleWifiProv_Init( void );
/* @[declare_iotblewifiprov_init] */

/**
 * @brief Function which runs the process loop for Wifi provisioning.
 * Process loop can be run within a task, it waits for the incoming requests from the
 * transport interface as well as commands from the user application. Process loop terminates when
 * a stop command is sent from the application.
 *
 * @return true if the process loop function ran successfully. false if process loop terminated due
 *         to an error.
 */
/* @[declare_iotblewifiprov_init] */
bool IotBleWifiProv_RunProcessLoop( void );

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
 * @brief Stop the WiFi provisionig process loop function.
 * This enqueues a command to stop the WiFi provisioning process loop function.
 *
 * @return true if succesfully enqueued command to stop WiFi provisioning loop.
 */
/* @[declare_iotblewifiprov_stop] */
bool IotBleWifiProv_Stop( void );

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
 */
/* @[declare_iotblewifiprov_delete] */
void IotBleWifiProv_Deinit( void );
/* @[declare_iotblewifiprov_delete] */

/**
 * @brief Gets the serializer interface used to serialize/deserialize packets over BLE.
 * By default it uses the cbor based serialization compatible with FreeRTOS wiFi provisioning mobile App. User
 * can provide a serializer for their custom message format as well.
 *
 * @return Serializer interface used
 */
/* @[declare_iotblewifiprov_getserializer] */
IotBleWifiProvSerializer_t * IotBleWifiProv_GetSerializer( void );
/* @[declare_iotblewifiprov_getserializer] */

#endif /* _AWS_BLE_WIFI_PROVISIONING_H_ */
