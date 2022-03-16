/*
 * FreeRTOS Wi-Fi for for Nuvoton NuMaker-IoT-M487 V1.0.0
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
static char pcConnectedSSID[ wificonfigMAX_SSID_LEN + 1 ] = { 0 };

/*-----------------------------------------------------------*/

/**
 * @brief Function to set a memory block to zero.
 * The function sets memory to zero using a volatile pointer so that compiler
 * wont optimize out the function if the buffer to be set to zero is not used further.
 * 
 * @param pBuf Pointer to buffer to be set to zero
 * @param size Length of the buffer to be set zero
 */
static void prvMemzero( void * pBuf, size_t size )
{
    volatile uint8_t * pMem = pBuf;
    uint32_t i;

    for( i = 0U; i < size; i++ )
    {
        pMem[ i ] = 0U;
    }
}

/*-----------------------------------------------------------*/

/**
 * @brief Copies byte array into char array, appending '\0'. Assumes char array can hold length of byte array + 1.
 * 		  Null terminator is guaranteed so long as xLen > 0. A maximum of xCap - 1 pucSrc bytes will be copied into pcDest,
 * 		  therefore truncation is possible.
 *
 * @param[in] pcDest The string to copy pucSrc contents into
 *
 * @param[in] pucSrc The byte array to copy into pcDest
 *
 * @param[in] xLen The queried number of byte to copy from pucSrc to pcDest
 *
 * @param[in] xCap Capacity of pcDest i.e. max characters it can store
 *
 */
static size_t prvByteArrayToString( char *pcDest, const uint8_t *pucSrc, size_t xLen, size_t xCap )
{
	configASSERT( pcDest );
	configASSERT( pucSrc );

	if ( xLen > ( xCap - 1 ) )
	{
		xLen = xCap - 1;
	}

	memcpy( pcDest, pucSrc, xLen );
	pcDest[ xLen ] = '\0';

	return xLen;
}


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
        xNuWiFi.xWifiObject.HeapUsage = 0;
        xNuWiFi.xWifiObject.IsPassiveMode = pdFALSE;
        xNuWiFi.xWifiObject.ActiveSocket = 0;

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

    if (pxNetworkParams == NULL 
        || pxNetworkParams->ucSSIDLength == 0 || pxNetworkParams->ucSSIDLength > wificonfigMAX_SSID_LEN  
        || (pxNetworkParams->xSecurity != eWiFiSecurityOpen && pxNetworkParams->xPassword.xWPA.ucLength == 0)) {
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

						char pcSSID[ pxNetworkParams->ucSSIDLength + 1];
						char pcPassword [ wificonfigMAX_PASSPHRASE_LEN + 1 ];
						prvByteArrayToString( pcSSID, 
																	pxNetworkParams->ucSSID, 
																	pxNetworkParams->ucSSIDLength, 
																	wificonfigMAX_SSID_LEN );
						prvByteArrayToString( pcPassword, 
																	pxNetworkParams->xPassword.xWPA.cPassphrase, 
																	pxNetworkParams->xPassword.xWPA.ucLength, 
																	wificonfigMAX_PASSPHRASE_LEN );
            for (i = 0 ; i < wificonfigNUM_CONNECTION_RETRY ; i++) {
                /* Connect to AP */
                if (ESP_WIFI_Connect(&xNuWiFi.xWifiObject, pcSSID, pcPassword) == ESP_WIFI_STATUS_OK) {
                    /* Store network settings. */
                    if (ESP_WIFI_GetNetStatus(&xNuWiFi.xWifiObject) == ESP_WIFI_STATUS_OK) {
												memcpy( pcConnectedSSID, pcSSID, strlen( pcSSID ) + 1);
                        xWiFiRet = eWiFiSuccess;
                        break;
                    }
                }
            }
            /* Use a private function to reset the memory block instead of memset, so that compiler wont optimize away the function call. */
            prvMemzero( pcPassword, sizeof( pcPassword ) );
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

WIFIReturnCode_t WIFI_GetIPInfo( WIFIIPConfiguration_t * pxIPConfig )
{
    WIFIReturnCode_t xWiFiRet = eWiFiFailure;

    if ( pxIPConfig == NULL) {
        return xWiFiRet;
    }

    if (xSemaphoreTake(xNuWiFi.xWifiSem, xSemaphoreWaitTicks) == pdTRUE) {
        if (xNuWiFi.xWifiObject.IsConnected == pdTRUE) {
            memcpy( &pxIPConfig->xIPAddress.ulAddress[ 0 ], &xNuWiFi.xWifiObject.StaIpAddr[ 0 ], 4 );
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

BaseType_t WIFI_IsConnected( const WIFINetworkParams_t * pxNetworkParams )
{
    BaseType_t xIsConnected = pdFALSE;

    if (xSemaphoreTake(xNuWiFi.xWifiSem, xSemaphoreWaitTicks) == pdTRUE) {
        if (ESP_WIFI_IsConnected(&xNuWiFi.xWifiObject) == pdTRUE) {
						if ( pxNetworkParams )
						{
								if( pxNetworkParams->ucSSIDLength > 0 
										&& pxNetworkParams->ucSSIDLength <= wificonfigMAX_SSID_LEN
										&& 0 == memcmp( pxNetworkParams->ucSSID, pcConnectedSSID, pxNetworkParams->ucSSIDLength )
										&& pxNetworkParams->ucSSIDLength == strlen( pcConnectedSSID ) ) 
								{
										xIsConnected = pdTRUE;
								}
						}
						else
						{            
								xIsConnected = pdTRUE;
						}
        }
        xSemaphoreGive(xNuWiFi.xWifiSem);
    }

    return xIsConnected;
}

/*-----------------------------------------------------------*/

WIFIReturnCode_t WIFI_RegisterEvent( WIFIEventType_t xEventType, WIFIEventHandler_t xHandler )
{
    /** Needs to implement dispatching network state change events **/
    return eWiFiNotSupported;
}

/*-----------------------------------------------------------*/

WIFIReturnCode_t WIFI_StartScan( WIFIScanConfig_t * pxScanConfig )
{
		return eWiFiNotSupported;
}

/*-----------------------------------------------------------*/

WIFIReturnCode_t WIFI_GetScanResults( const WIFIScanResult_t ** pxBuffer,
                                      uint16_t * ucNumNetworks )
{
		return eWiFiNotSupported;
}

/*-----------------------------------------------------------*/

WIFIReturnCode_t WIFI_StartConnectAP( const WIFINetworkParams_t * pxNetworkParams )
{
		return eWiFiNotSupported;
}

/*-----------------------------------------------------------*/

WIFIReturnCode_t WIFI_StartDisconnect( void )
{
		return eWiFiNotSupported;
}

/*-----------------------------------------------------------*/

WIFIReturnCode_t WIFI_GetRSSI( int8_t * pcRSSI )
{
		return eWiFiNotSupported; 
}

/*-----------------------------------------------------------*/

WIFIReturnCode_t WIFI_GetStationList( WIFIStationInfo_t * pxStationList,
                                      uint8_t * pcStationListSize )
{
		return eWiFiNotSupported; 
}

/*-----------------------------------------------------------*/

WIFIReturnCode_t WIFI_StartDisconnectStation( uint8_t * pucMac )
{
		return eWiFiNotSupported; 
}

/*-----------------------------------------------------------*/


WIFIReturnCode_t WIFI_SetMAC( uint8_t * pucMac )
{
		return eWiFiNotSupported; 
}

/*-----------------------------------------------------------*/


WIFIReturnCode_t WIFI_SetCountryCode( const char * pcCountryCode )
{
		return eWiFiNotSupported; 
}

/*-----------------------------------------------------------*/


WIFIReturnCode_t WIFI_GetCountryCode( char * pcCountryCode )
{
		return eWiFiNotSupported; 
}

/*-----------------------------------------------------------*/


WIFIReturnCode_t WIFI_GetStatistic( WIFIStatisticInfo_t * pxStats )
{
		return eWiFiNotSupported; 
}

/*-----------------------------------------------------------*/

WIFIReturnCode_t WIFI_GetCapability( WIFICapabilityInfo_t * pxCaps )
{
		return eWiFiNotSupported; 
}
