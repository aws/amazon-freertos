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
 * @file aws_iot_network.h
 * @brief Header file which defines all supported network types for Freertos.
 */
#ifndef AWS_IOT_NETWORK_H_
#define AWS_IOT_NETWORK_H_

#include "iot_network_types.h"
#include "aws_iot_network_config.h"

#ifndef configSUPPORTED_NETWORKS
#error "Flag 'configSUPPORTED_NETWORKS' is not defined. Please define the flag in aws_iot_network_config.h with the list of all networks supported by the board"
#endif

#ifndef configENABLED_NETWORKS
#error "Flag 'configENABLED_NETWORKS' not defined. Please define the flag in aws_iot_network_config.h with all the networks that needs to be enabled"
#endif


/**
 * @brief Flag used to get all the network types.
 */
#define AWSIOT_NETWORK_TYPE_ALL      ( AWSIOT_NETWORK_TYPE_WIFI | AWSIOT_NETWORK_TYPE_BLE )


#if defined( configENABLED_NETWORKS ) && defined( configSUPPORTED_NETWORKS )
#if ( ( configENABLED_NETWORKS | configSUPPORTED_NETWORKS ) != configSUPPORTED_NETWORKS )
#error "Enabled networks should always be a subset of supported networks"
#endif
#endif

/**
 * @brief Compile time flag which can be used to check if BLE is enabled.
 */
#define BLE_ENABLED        ( ( configENABLED_NETWORKS &  AWSIOT_NETWORK_TYPE_BLE ) == AWSIOT_NETWORK_TYPE_BLE )

/**
 * @brief Compile time flag which can be used to check if WIFI is enabled.
 */
#define WIFI_ENABLED       ( ( configENABLED_NETWORKS &  AWSIOT_NETWORK_TYPE_WIFI ) == AWSIOT_NETWORK_TYPE_WIFI )

/**
 * @brief Initializer for a subscription handle.
 */
#define AWSIOT_NETWORK_SUBSCRIPTION_HANDLE_INITIALIZER   ( NULL )


/**
 * @brief Unique handle for a Subscription.
 */
typedef void * SubscriptionHandle_t;


/**
 * @brief Callback used for passing state change events from network layer to application layer.
 *
 * Application can register  the callback with network layer such as WIFI or BLE.
 *
 * @param[in] ulNetworkType Network type to be passed within the callback
 * @param[in] pvContext The context passed in at the time of registering.
 */
typedef void ( *AwsIotNetworkStateChangeCb_t ) ( uint32_t ulNetworkType, AwsIotNetworkState_t xNetworkState, void* pvContext );


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
BaseType_t AwsIotNetworkManager_SubscribeForStateChange( uint32_t ulNetworkTypes, AwsIotNetworkStateChangeCb_t xCallback, void * pvContext, SubscriptionHandle_t* pxHandle );


/**
 * @brief Removes a subscription for network state change
 * @param xHandle Handle to the subscription
 * @return pdTRUE or pdFALSE
 */
BaseType_t AwsIotNetworkManager_RemoveSubscription(  SubscriptionHandle_t xHandle );


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
 * API to enable network manager for different networks
 * @param ulNetworkTypes Network types
 * @return FLag indicating the network types enabled
 */
uint32_t AwsIotNetworkManager_EnableNetwork( uint32_t ulNetworkTypes );

/**
 * API to disable network manager for different networks
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
