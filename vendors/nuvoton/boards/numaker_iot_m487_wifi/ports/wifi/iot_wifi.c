/*
 * Amazon FreeRTOS Wi-Fi for for Nuvoton NuMaker-IoT-M487 V1.0.0
 * Copyright (C) 2019 Amazon.com, Inc. or its affiliates.  All Rights Reserved.
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
 * @file aws_wifi.c
 * @brief Wi-Fi Interface.
 */

/* FreeRTOS includes. */
#include "FreeRTOS.h"
#include "semphr.h"
#include "task.h"

/* Socket and Wi-Fi interface includes. */
#include "iot_wifi.h"

/* Wi-Fi configuration includes. */
#include "aws_wifi_config.h"

/* Wi-Fi driver includes. */
#include "esp8266_wifi.h"


/* WiFi module definition */
typedef struct NuWiFiModule
{
    ESP_WIFI_Object_t xWifiObject;
    SemaphoreHandle_t xWifiSem;
} NuWiFiModule_t;
NuWiFiModule_t xNuWiFi;

#define ESP_UART            UART1;
#define ESP_UART_IRQ        UART1_IRQn;
#define ESP_UART_BAUDRATE   115200;
#define ESP_TIMEOUT_IN_MS   20000;

static BaseType_t xWIFI_IsInitialized;
static const TickType_t xSemaphoreWaitTicks = pdMS_TO_TICKS(wificonfigMAX_SEMAPHORE_WAIT_TIME_MS);

/*-----------------------------------------------------------*/


WIFIReturnCode_t WIFI_On( void )
{
    WIFIReturnCode_t xWiFiRet = eWiFiFailure;

    if (xWIFI_IsInitialized == pdFALSE) {
        static StaticSemaphore_t xSemaphoreBuffer;

        memset(&xNuWiFi, 0, sizeof(xNuWiFi));

        /* Create the sync semaphore for Wi-Fi module operations */
        xNuWiFi.xWifiSem = xSemaphoreCreateMutexStatic(&xSemaphoreBuffer);
        /* Initialize semaphore */
        xSemaphoreGive(xNuWiFi.xWifiSem);

        xNuWiFi.xWifiObject.Uart = ESP_UART;
        xNuWiFi.xWifiObject.UartIrq = ESP_UART_IRQ;
        xNuWiFi.xWifiObject.UartBaudRate = ESP_UART_BAUDRATE;
        xNuWiFi.xWifiObject.Timeout = ESP_TIMEOUT_IN_MS;
        xNuWiFi.xWifiObject.IsMultiConn = pdFALSE;
        xNuWiFi.xWifiObject.ActiveCmd = CMD_NONE;

        xWIFI_IsInitialized = pdTRUE;
    }

    /* Initialize Wi-Fi module */
    if (ESP_WIFI_Init(&xNuWiFi.xWifiObject) == ESP_WIFI_STATUS_OK) {
        xWiFiRet = eWiFiSuccess;
    }

    return xWiFiRet;
}
/*-----------------------------------------------------------*/

WIFIReturnCode_t WIFI_Off( void )
{
    WIFIReturnCode_t xWiFiRet = eWiFiNotSupported;

    return xWiFiRet;
}
/*-----------------------------------------------------------*/

WIFIReturnCode_t WIFI_ConnectAP( const WIFINetworkParams_t * const pxNetworkParams )
{
    WIFIReturnCode_t xWiFiRet = eWiFiFailure;
    uint32_t i;

    if (pxNetworkParams == NULL || pxNetworkParams->pcSSID == NULL || 
        (pxNetworkParams->xSecurity != eWiFiSecurityOpen && pxNetworkParams->pcPassword == NULL)) {
        return xWiFiRet;
    }

    configPRINTF(("current task name is \"%s\"\n", pcTaskGetName(xTaskGetCurrentTaskHandle())));
    if (xSemaphoreTake(xNuWiFi.xWifiSem, xSemaphoreWaitTicks) == pdTRUE) {
        /* Disconnect first if the network is connected */
        if (ESP_WIFI_IsConnected(&xNuWiFi.xWifiObject)) {
            if (ESP_WIFI_Disconnect(&xNuWiFi.xWifiObject) == ESP_WIFI_STATUS_OK) {
                xWiFiRet = eWiFiSuccess;
            }
        } else {
            xWiFiRet = eWiFiSuccess;
        }

        if (xWiFiRet == eWiFiSuccess) {
            xWiFiRet = eWiFiFailure;

            for (i = 0 ; i < wificonfigNUM_CONNECTION_RETRY ; i++) {
                /* Connect to AP */
                if (ESP_WIFI_Connect(&xNuWiFi.xWifiObject, pxNetworkParams->pcSSID,
                                     pxNetworkParams->pcPassword) == ESP_WIFI_STATUS_OK) {
                    /* Store network settings. */
                    if (ESP_WIFI_GetNetStatus(&xNuWiFi.xWifiObject) == ESP_WIFI_STATUS_OK) {
                        xWiFiRet = eWiFiSuccess;
                        break;
                    }
                }
            }
        }
        xSemaphoreGive(xNuWiFi.xWifiSem);
    } else {
        xWiFiRet = eWiFiTimeout;
    }

    return xWiFiRet;
}
/*-----------------------------------------------------------*/

WIFIReturnCode_t WIFI_Disconnect( void )
{
    WIFIReturnCode_t xWiFiRet = eWiFiFailure;

    if (xSemaphoreTake(xNuWiFi.xWifiSem, xSemaphoreWaitTicks ) == pdTRUE) {
        if (ESP_WIFI_Disconnect(&xNuWiFi.xWifiObject) == ESP_WIFI_STATUS_OK) {
            xWiFiRet = eWiFiSuccess;
        }
        xSemaphoreGive(xNuWiFi.xWifiSem);
    } else {
        xWiFiRet = eWiFiTimeout;
    }

    return xWiFiRet;
}
/*-----------------------------------------------------------*/

WIFIReturnCode_t WIFI_Reset( void )
{
    WIFIReturnCode_t xWiFiRet = eWiFiFailure;

    if (xSemaphoreTake(xNuWiFi.xWifiSem, xSemaphoreWaitTicks) == pdTRUE) {
        if (ESP_WIFI_Reset(&xNuWiFi.xWifiObject) == ESP_WIFI_STATUS_OK) {
            xWiFiRet = eWiFiSuccess;
        }
        xSemaphoreGive(xNuWiFi.xWifiSem);
    } else {
        xWiFiRet = eWiFiTimeout;
    }

    return xWiFiRet;
}
/*-----------------------------------------------------------*/

WIFIReturnCode_t WIFI_Scan( WIFIScanResult_t * pxBuffer, uint8_t ucNumNetworks )
{
    WIFIReturnCode_t xWiFiRet = eWiFiFailure;

    if (pxBuffer == NULL) {
        return xWiFiRet;
    }

    if (xSemaphoreTake(xNuWiFi.xWifiSem, xSemaphoreWaitTicks) == pdTRUE) {
        if (ESP_WIFI_Scan(&xNuWiFi.xWifiObject, pxBuffer, ucNumNetworks) == ESP_WIFI_STATUS_OK) {
            xWiFiRet = eWiFiSuccess;
        }
        xSemaphoreGive(xNuWiFi.xWifiSem);
    } else {
        xWiFiRet = eWiFiTimeout;
    }

    return xWiFiRet;
}
/*-----------------------------------------------------------*/

WIFIReturnCode_t WIFI_SetMode( WIFIDeviceMode_t xDeviceMode )
{
    WIFIReturnCode_t xWiFiRet = eWiFiNotSupported;

    return xWiFiRet;
}
/*-----------------------------------------------------------*/

WIFIReturnCode_t WIFI_GetMode( WIFIDeviceMode_t * pxDeviceMode )
{
    WIFIReturnCode_t xWiFiRet = eWiFiNotSupported;

    return xWiFiRet;
}
/*-----------------------------------------------------------*/

WIFIReturnCode_t WIFI_NetworkAdd( const WIFINetworkProfile_t * const pxNetworkProfile,
                                  uint16_t * pusIndex )
{
    WIFIReturnCode_t xWiFiRet = eWiFiNotSupported;

    return xWiFiRet;
}
/*-----------------------------------------------------------*/

WIFIReturnCode_t WIFI_NetworkGet( WIFINetworkProfile_t * pxNetworkProfile, uint16_t usIndex )
{
    WIFIReturnCode_t xWiFiRet = eWiFiNotSupported;

    return xWiFiRet;
}
/*-----------------------------------------------------------*/

WIFIReturnCode_t WIFI_NetworkDelete( uint16_t usIndex )
{
    WIFIReturnCode_t xWiFiRet = eWiFiNotSupported;

    return xWiFiRet;
}
/*-----------------------------------------------------------*/

WIFIReturnCode_t WIFI_Ping( uint8_t * pucIPAddr, uint16_t usCount, uint32_t ulIntervalMS )
{
    WIFIReturnCode_t xWiFiRet = eWiFiFailure;
    uint32_t i;

    if (pucIPAddr == NULL || usCount == 0) {
        return xWiFiRet;
    }

    if (xSemaphoreTake(xNuWiFi.xWifiSem, xSemaphoreWaitTicks) == pdTRUE) {
        for (i = 0 ; i < usCount ; i++) {
            if (ESP_WIFI_Ping(&xNuWiFi.xWifiObject, pucIPAddr) == ESP_WIFI_STATUS_OK) {
                xWiFiRet = eWiFiSuccess;
                if (i < usCount - 1) {
                    vTaskDelay(pdMS_TO_TICKS(ulIntervalMS));
                }
            }
        }
        xSemaphoreGive(xNuWiFi.xWifiSem);
    } else {
        xWiFiRet = eWiFiTimeout;
    }

    return xWiFiRet;
}
/*-----------------------------------------------------------*/

WIFIReturnCode_t WIFI_GetIP( uint8_t * pucIPAddr )
{
    WIFIReturnCode_t xWiFiRet = eWiFiFailure;

    if (pucIPAddr == NULL) {
        return xWiFiRet;
    }

    if (xSemaphoreTake(xNuWiFi.xWifiSem, xSemaphoreWaitTicks) == pdTRUE) {
        if (xNuWiFi.xWifiObject.IsConnected == pdTRUE) {
            memcpy(pucIPAddr, xNuWiFi.xWifiObject.StaIpAddr, 4);
            xWiFiRet = eWiFiSuccess;
        }
        xSemaphoreGive(xNuWiFi.xWifiSem);
    } else {
        xWiFiRet = eWiFiTimeout;
    }

    return xWiFiRet;
}
/*-----------------------------------------------------------*/

WIFIReturnCode_t WIFI_GetMAC( uint8_t * pucMac )
{
    WIFIReturnCode_t xWiFiRet = eWiFiFailure;

    if (pucMac == NULL) {
        return xWiFiRet;
    }

    if (xSemaphoreTake(xNuWiFi.xWifiSem, xSemaphoreWaitTicks) == pdTRUE) {
        if (xNuWiFi.xWifiObject.IsConnected == pdTRUE) {
            memcpy(pucMac, xNuWiFi.xWifiObject.StaMacAddr, 6);
            xWiFiRet = eWiFiSuccess;
        }
        xSemaphoreGive(xNuWiFi.xWifiSem);
    } else {
        xWiFiRet = eWiFiTimeout;
    }

    return xWiFiRet;
}
/*-----------------------------------------------------------*/

WIFIReturnCode_t WIFI_GetHostIP( char * pcHost, uint8_t * pucIPAddr )
{
    WIFIReturnCode_t xWiFiRet = eWiFiFailure;

    if (pcHost == NULL || pucIPAddr == NULL) {
        return xWiFiRet;
    }

    if (xSemaphoreTake(xNuWiFi.xWifiSem, xSemaphoreWaitTicks) == pdTRUE) {
        if (ESP_WIFI_GetHostIP(&xNuWiFi.xWifiObject, pcHost, pucIPAddr) == ESP_WIFI_STATUS_OK) {
            xWiFiRet = eWiFiSuccess;
        }
        xSemaphoreGive(xNuWiFi.xWifiSem);
    } else {
        xWiFiRet = eWiFiTimeout;
    }

    return xWiFiRet;
}
/*-----------------------------------------------------------*/

WIFIReturnCode_t WIFI_StartAP( void )
{
    WIFIReturnCode_t xWiFiRet = eWiFiNotSupported;

    return xWiFiRet;
}
/*-----------------------------------------------------------*/

WIFIReturnCode_t WIFI_StopAP( void )
{
    WIFIReturnCode_t xWiFiRet = eWiFiNotSupported;

    return xWiFiRet;
}
/*-----------------------------------------------------------*/

WIFIReturnCode_t WIFI_ConfigureAP( const WIFINetworkParams_t * const pxNetworkParams )
{
    WIFIReturnCode_t xWiFiRet = eWiFiNotSupported;

    return xWiFiRet;
}
/*-----------------------------------------------------------*/

WIFIReturnCode_t WIFI_SetPMMode( WIFIPMMode_t xPMModeType, const void * pvOptionValue )
{
    WIFIReturnCode_t xWiFiRet = eWiFiNotSupported;

    return xWiFiRet;
}
/*-----------------------------------------------------------*/

WIFIReturnCode_t WIFI_GetPMMode( WIFIPMMode_t * pxPMModeType, void * pvOptionValue )
{
    WIFIReturnCode_t xWiFiRet = eWiFiNotSupported;

    return xWiFiRet;
}
/*-----------------------------------------------------------*/

BaseType_t WIFI_IsConnected( void )
{
    BaseType_t xIsConnected = pdFALSE;

    if (xSemaphoreTake(xNuWiFi.xWifiSem, xSemaphoreWaitTicks) == pdTRUE) {
        if (ESP_WIFI_IsConnected(&xNuWiFi.xWifiObject) == pdTRUE) {
            xIsConnected = pdTRUE;
        }
        xSemaphoreGive(xNuWiFi.xWifiSem);
    }

    return xIsConnected;
}

WIFIReturnCode_t WIFI_RegisterNetworkStateChangeEventCallback( IotNetworkStateChangeEventCallback_t xCallback  )
{
    /** Needs to implement dispatching network state change events **/
    return eWiFiNotSupported;
}
