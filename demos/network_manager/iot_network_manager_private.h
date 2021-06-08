/*
 * FreeRTOS V202012.00
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
 * @file aws_iot_network.h
 * @brief Header file which defines all supported network types for Freertos.
 */
#ifndef AWS_IOT_NETWORK_H_
#define AWS_IOT_NETWORK_H_

#include "types/iot_network_types.h"
#include "aws_iot_network_config.h"

#include "platform/iot_network.h"

#ifndef configSUPPORTED_NETWORKS
    #error "Flag 'configSUPPORTED_NETWORKS' is not defined. Please define the flag in aws_iot_network_config.h with the list of all networks supported by the board"
#endif

#ifndef configENABLED_NETWORKS
    #error "Flag 'configENABLED_NETWORKS' not defined. Please define the flag in aws_iot_network_config.h with all the networks that needs to be enabled"
#endif


#if defined( configENABLED_NETWORKS ) && defined( configSUPPORTED_NETWORKS )
    #if ( ( configENABLED_NETWORKS | configSUPPORTED_NETWORKS ) != configSUPPORTED_NETWORKS )
        #error "Enabled networks should always be a subset of supported networks"
    #endif
#endif

/**
 * @brief Compile time flag which can be used to check if BLE is enabled.
 */
#define BLE_ENABLED                                     ( ( configENABLED_NETWORKS & AWSIOT_NETWORK_TYPE_BLE ) == AWSIOT_NETWORK_TYPE_BLE )

/**
 * @brief Compile time flag which can be used to check if WIFI is enabled.
 */
#define WIFI_ENABLED                                    ( ( configENABLED_NETWORKS & AWSIOT_NETWORK_TYPE_WIFI ) == AWSIOT_NETWORK_TYPE_WIFI )

/**
 * @brief Compile time flag which can be used to check if Ethernet is enabled.
 */
#define ETH_ENABLED                                     ( ( configENABLED_NETWORKS & AWSIOT_NETWORK_TYPE_ETH ) == AWSIOT_NETWORK_TYPE_ETH )

#define TCPIP_NETWORK_ENABLED                           ( ( configENABLED_NETWORKS & AWSIOT_NETWORK_TYPE_TCP_IP ) != 0 )

/**
 * @brief Initializer for a subscription handle.
 */
#define IOT_NETWORK_MANAGER_SUBSCRIPTION_INITIALIZER    ( NULL )


/**
 * @brief Unique handle for a network manager user callback subscription.
 */
typedef void * IotNetworkManagerSubscription_t;


/**
 * @brief Callback used for passing state change events from network layer to application layer.
 *
 * Application can register  the callback with network layer such as WIFI or BLE.
 *
 * @param[in] ulNetworkType Network type to be passed within the callback
 * @param[in] pvContext The context passed in at the time of registering.
 */
typedef void ( * AwsIotNetworkStateChangeCb_t ) ( uint32_t ulNetworkType,
                                                  AwsIotNetworkState_t xNetworkState,
                                                  void * pvContext );


/**
 * @brief API  to subscribe for the network state change event.
 *
 * Application can register multiple callbacks, single callback for multiple network types
 * or separate callbacks for each network type.
 *
 * @param ulNetworkTpe Network type for the callback
 * @param xCallback Callback for receiving the network state change event
 * @param pvContext Context passed in as it is by the callback
 * @param pxHandle Handle to the subscription
 * @return pdTRUE or pdFALSE
 */
BaseType_t AwsIotNetworkManager_SubscribeForStateChange( uint32_t ulNetworkTypes,
                                                         AwsIotNetworkStateChangeCb_t xCallback,
                                                         void * pvContext,
                                                         IotNetworkManagerSubscription_t * pxHandle );


/**
 * @brief Removes a subscription for network state change
 * @param xHandle Handle to the subscription
 * @return pdTRUE or pdFALSE
 */
BaseType_t AwsIotNetworkManager_RemoveSubscription( IotNetworkManagerSubscription_t xHandle );


/**
 * @brief Returns all network types which are connected.
 * @return Network Types of all connected networks.
 */
uint32_t AwsIotNetworkManager_GetConnectedNetworks( void );

/**
 * @brief Gets all supported networks
 * @return Flags indicating all configured networks
 */
uint32_t AwsIotNetworkManager_GetConfiguredNetworks( void );


/**
 * @brief Get the networks which are enabled currently.
 * @return Flags indicating all enabled networks.
 */
uint32_t AwsIotNetworkManager_GetEnabledNetworks( void );

/**
 * @brief Get the network interface for a given network.
 * The network interface can be used to create/delete a network connection, send/receive data over the connection.
 *
 * @param[in] networkType The type of the network
 * @return  NetworkInterface stucture for the network type.
 *
 */
const IotNetworkInterface_t * AwsIotNetworkManager_GetNetworkInterface( uint32_t networkType );

/**
 * @brief Gets the credentials associated with the network.
 * Credentials can then be passed in to the network interface create API to create a secure connection over the network.
 *
 * @param[in] networkType Type of the network
 * @return  Blob representing the credentials associated with a network. NULL if no credentials are provided.
 */
void * AwsIotNetworkManager_GetCredentials( uint32_t networkType );

/**
 * @brief Gets the connection parameters associated with a network.
 * Connection parameters can the be passed in to the network interface create API to create a connection over the network.
 *
 * @param[in] networkType Type of the network
 * @return Blob representing the parameters for connection over a network.
 */
void * AwsIotNetworkManager_GetConnectionParams( uint32_t networkType );

/**
 * API to enable network manager for different networks
 * @param ulNetworkTypes Network types
 * @return FLag indicating the network types enabled
 */
uint32_t AwsIotNetworkManager_EnableNetwork( uint32_t ulNetworkTypes );

/**
 * API to disable network manager for different networks
 *
 * @param ulNetworkTypes Network types
 * @return FLag indicating the network types disabled
 */
uint32_t AwsIotNetworkManager_DisableNetwork( uint32_t ulNetworkTypes );

/**
 * Initializes the network manager
 * @return pdTRUE or pdFALSE
 */
BaseType_t AwsIotNetworkManager_Init( void );

#endif /* LIB_AWS_INCLUDE_AWS_IOT_NETWORK_H_ */
