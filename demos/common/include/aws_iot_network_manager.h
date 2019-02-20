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
 * @brief Header file containing the network management APIs
 */
#ifndef _AWS_IOT_NETWORK_MANAGER_H_
#define _AWS_IOT_NETWORK_MANAGER_H_

#include <stdint.h>
#include <stdbool.h>
#include "FreeRTOS.h"
#include "aws_doubly_linked_list.h"
#include "semphr.h"
#include "aws_iot_network.h"

/**
 * @brief Unique handle for a Subscription.
 */
typedef void * SubscriptionHandle_t;

/**
 * @brief Initializer for a subscription handle.
 */
#define AWSIOT_NETWORK_SUBSCRIPTION_HANDLE_INITIALIZER   ( NULL )


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
 * @brief Blocks for atleast one network connection. Returns back the connected networks on atleast one successful connection.
 * @return Network Types of all connected networks.
 */
uint32_t AwsIotNetworkManager_WaitForNetworkConnection( void );

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

#endif /* _AWS_IOT_NETWORK_MANAGER_H_ */
