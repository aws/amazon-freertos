/*
 * Amazon FreeRTOS V1.4.4
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
 * @brief The demo task is used to connect to the list of provisioned WIFI networks in priority order.
 * The task also handles WIFI disconnection and reconnect to the provisioned APs upto a configured number of retries.
 */

#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
#include "aws_demo_config.h"
#include "aws_wifi_connect_task.h"
#include "aws_iot_network_manager.h"
#include "aws_ble_wifi_provisioning.h"


#define wifiConnectTASK_NAME              "WiFiConnectTask"

/**
 * @brief Delay in milliseconds between connecting to the provisioned WiFi networks.
 */
#define wifiConnectDELAY_MILLISECONDS     ( 1000 )

/**
 * @brief Number of retries in connecting to the provisioned networks.
 */
#define wifiConnectNUM_RETRIES            ( 500 )


/**
 * @brief Task for WiFi Reconnection. The task loops through the provisioned wifi networks in priority order
 * until retry limit is reached or a network is connected. Task gets triggered on every WiFi disconnection.
 */
static void prvWiFiConnectTask( void * pvParams );

/**
 * @brief WIFI network state change callback.
 * The callback is used to trigger reconnection to the provisioned WiFi networks on a network disconnect.
 */

static void prvWiFiNetworkStateChangeCallback( uint32_t ulNetworkType, AwsIotNetworkState_t xNetworkState, void* pvContext );

/**
 * @brief Semaphore used to trigger reconnection to WiFi networks.
 */
static SemaphoreHandle_t xWiFiConnectLock = NULL;
static TaskHandle_t xWifiConnectionTask = NULL;
static SubscriptionHandle_t xHandle = NULL;

static void prvWiFiNetworkStateChangeCallback( uint32_t ulNetworkType, AwsIotNetworkState_t xNetworkState, void* pvContext )
{
    if( xNetworkState == eNetworkStateDisabled )
    {
        xSemaphoreGive( xWiFiConnectLock );
    }
}

void prvWiFiConnectTask( void * pvParams )
{
    uint16_t ulNumNetworks, ulNetworkIndex, ulNumRetries;
    TickType_t xWifiConnectionDelay = pdMS_TO_TICKS( wifiConnectDELAY_MILLISECONDS );
    BaseType_t xWiFiConnected;

    for(;;)
    {
        if( xSemaphoreTake( xWiFiConnectLock, portMAX_DELAY ) == pdTRUE )
        {
            xWiFiConnected = pdFALSE;
            for( ulNumRetries = 0;
                    ( ulNumRetries < wifiConnectNUM_RETRIES ) &&
                            ( xWiFiConnected == pdFALSE );
                    ulNumRetries++ )
            {
                ulNumNetworks = WIFI_PROVISION_GetNumNetworks();
                for( ulNetworkIndex = 0; ulNetworkIndex < ulNumNetworks; ulNetworkIndex++ )
                {
                    if( WIFI_IsConnected() == pdFALSE )
                    {
                        xWiFiConnected = WIFI_PROVISION_Connect( ulNetworkIndex );
                    }
                    else
                    {
                        xWiFiConnected = pdTRUE;
                    }

                    if( xWiFiConnected == pdTRUE )
                    {
                        break;
                    }
                    vTaskDelay( xWifiConnectionDelay );
                }
            }
        }
    }

    vTaskDelete( NULL );
}

BaseType_t xWiFiConnectTaskInitialize( void )
{
    BaseType_t xRet = pdTRUE;
    xWiFiConnectLock = xSemaphoreCreateBinary();
    if( xWiFiConnectLock != NULL )
    {
        xRet = xSemaphoreGive( xWiFiConnectLock );
    }
    else
    {
        xRet = pdFALSE;
    }

    if( xRet == pdTRUE )
    {

        xRet = AwsIotNetworkManager_SubscribeForStateChange(
                AWSIOT_NETWORK_TYPE_WIFI,
                prvWiFiNetworkStateChangeCallback,
                NULL,
                &xHandle );
    }

    if( xRet == pdTRUE )
    {
        xRet = xTaskCreate(
                prvWiFiConnectTask,
                wifiConnectTASK_NAME,
                democonfigWIFI_CONNECT_TASK_STACK_SIZE,
                NULL,
                democonfigWIFI_CONNECT_TASK_PRIORITY,
                &xWifiConnectionTask );
    }
    return xRet;
}

void vWiFiConnectTaskDestroy( void )
{
    if( xHandle != NULL )
    {
        AwsIotNetworkManager_RemoveSubscription( xHandle );
        xHandle = NULL;
    }

    if( xWifiConnectionTask != NULL )
    {
        vTaskDelete( xWifiConnectionTask );
        xWifiConnectionTask = NULL;
    }

    if( xWiFiConnectLock != NULL )
    {
        vSemaphoreDelete( xWiFiConnectLock );
        xWiFiConnectLock = NULL;
    }
}
