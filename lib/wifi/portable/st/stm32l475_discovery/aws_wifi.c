/*
 * Amazon FreeRTOS Wi-Fi STM32L4 Discovery kit IoT node V1.0.2
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
 * @file aws_wifi.c
 * @brief Wi-Fi Interface.
 */

/* FreeRTOS includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"

/* Wi-Fi driver includes. */
#include "es_wifi.h"
#include "es_wifi_io.h"

/* Socket and Wi-Fi interface includes. */
#include "aws_wifi.h"


/**
 * @brief Represents the Wi-Fi module.
 *
 * Since there is only one Wi-Fi module on the ST board, only
 * one instance of this type is needed. All the operations on
 * the Wi-Fi module must be serialized because a single operation
 * (like socket connect, send etc) consists of multiple AT Commands
 * sent over the same SPI bus. A semaphore is therefore used to
 * serialize all the operations.
 */
typedef struct STWiFiModule
{
    ES_WIFIObject_t xWifiObject;        /**< Internal Wi-Fi object. */
    SemaphoreHandle_t xSemaphoreHandle; /**< Semaphore used to serialize all the operations on the Wi-Fi module. */
} STWiFiModule_t;

STWiFiModule_t xWiFiModule;

/**
 * @brief Wi-Fi initialization status.
 */
static BaseType_t xWIFIInitDone;

/**
 * @brief Maximum time to wait in ticks for obtaining the Wi-Fi semaphore
 * before failing the operation.
 */

static const TickType_t xSemaphoreWaitTicks = pdMS_TO_TICKS( wificonfigMAX_SEMAPHORE_WAIT_TIME_MS );

/**
 * @brief Maps the given abstracted security type to ST specific one.
 *
 * @param[in] xSecurity The given abstracted security type.
 *
 * @return Corresponding ST specific security type.
 */
static ES_WIFI_SecurityType_t prvConvertSecurityFromAbstractedToST( WIFISecurity_t xSecurity )
{
    ES_WIFI_SecurityType_t xConvertedSecurityType = ES_WIFI_SEC_UNKNOWN;

    switch( xSecurity )
    {
        case eWiFiSecurityOpen:
            xConvertedSecurityType = ES_WIFI_SEC_OPEN;
            break;

        case eWiFiSecurityWEP:
            xConvertedSecurityType = ES_WIFI_SEC_WEP;
            break;

        case eWiFiSecurityWPA:
            xConvertedSecurityType = ES_WIFI_SEC_WPA;
            break;

        case eWiFiSecurityWPA2:
            xConvertedSecurityType = ES_WIFI_SEC_WPA2;
            break;

        case eWiFiSecurityNotSupported:
            xConvertedSecurityType = ES_WIFI_SEC_UNKNOWN;
            break;
    }

    return xConvertedSecurityType;
}

/*-----------------------------------------------------------*/

/**
 * @brief Maps the ST security type to abstracted security type.
 *
 * @param[in] xSecurity The ST security type.
 *
 * @return Corresponding abstracted security type.
 */
static WIFISecurity_t prvConvertSecurityFromSTToAbstracted( ES_WIFI_SecurityType_t xSecurity )
{
    WIFISecurity_t xConvertedSecurityType = eWiFiSecurityNotSupported;

    switch( xSecurity )
    {
        case ES_WIFI_SEC_OPEN:
            xConvertedSecurityType = eWiFiSecurityOpen;
            break;

        case ES_WIFI_SEC_WEP:
            xConvertedSecurityType = eWiFiSecurityWEP;
            break;

        case ES_WIFI_SEC_WPA:
            xConvertedSecurityType = eWiFiSecurityWPA;
            break;

        case ES_WIFI_SEC_WPA2:
            xConvertedSecurityType = eWiFiSecurityWPA2;
            break;

        default:
            xConvertedSecurityType = eWiFiSecurityNotSupported;
            break;
    }

    return xConvertedSecurityType;
}

/*-----------------------------------------------------------*/

WIFIReturnCode_t WIFI_On( void )
{
    WIFIReturnCode_t xRetVal = eWiFiFailure;

    /* One time Wi-Fi initialization */
    if( xWIFIInitDone == pdFALSE )
    {
        /* This buffer is used to store the semaphore's data structure
         * and therefore must be static. */
        static StaticSemaphore_t xSemaphoreBuffer;

        /* Start with all the zero. */
        memset( &( xWiFiModule ), 0, sizeof( xWiFiModule ) );

        /* Create the semaphore used to serialize Wi-Fi module operations. */
        xWiFiModule.xSemaphoreHandle = xSemaphoreCreateMutexStatic( &( xSemaphoreBuffer ) );

        /* Initialize semaphore. */
        xSemaphoreGive( xWiFiModule.xSemaphoreHandle );

        /* Wi-Fi init done*/
        xWIFIInitDone = pdTRUE;
    }

    /* Register function pointers for carrying out SPI operations. */
    if( ES_WIFI_RegisterBusIO( &( xWiFiModule.xWifiObject ),
                               &( SPI_WIFI_Init ),
                               &( SPI_WIFI_DeInit ),
                               &( SPI_WIFI_Delay ),
                               &( SPI_WIFI_SendData ),
                               &( SPI_WIFI_ReceiveData ) ) == ES_WIFI_STATUS_OK )
    {
        /* Initialize the Wi-Fi module. */
        if( ES_WIFI_Init( &( xWiFiModule.xWifiObject ) ) == ES_WIFI_STATUS_OK )
        {
            /* Initialization successful. */
            xRetVal = eWiFiSuccess;
        }
    }

    return xRetVal;
}

/*-----------------------------------------------------------*/

WIFIReturnCode_t WIFI_Off( void )
{
    /*command not implemented in ES Wi-Fi drivers. */
    WIFIReturnCode_t xRetVal = eWiFiNotSupported;

    return xRetVal;
}
/*-----------------------------------------------------------*/

WIFIReturnCode_t WIFI_ConnectAP( const WIFINetworkParams_t * const pxNetworkParams )
{
    WIFIReturnCode_t xRetVal = eWiFiFailure;
    uint32_t x;

    configASSERT( pxNetworkParams != NULL );
    configASSERT( pxNetworkParams->pcSSID != NULL );

    if ( pxNetworkParams->xSecurity != eWiFiSecurityOpen ) 
    {
        configASSERT( pxNetworkParams->pcPassword != NULL );
    }

    /* Try to acquire the semaphore. */
    if( xSemaphoreTake( xWiFiModule.xSemaphoreHandle, xSemaphoreWaitTicks ) == pdTRUE )
    {
        /* Disconnect first if we are connected, to connect to the input network. */
        if( ES_WIFI_IsConnected( &xWiFiModule.xWifiObject ) )
        {
            if( ES_WIFI_Disconnect( &( xWiFiModule.xWifiObject ) ) ==  ES_WIFI_STATUS_OK )
            {
                xRetVal = eWiFiSuccess;
            }
        }
        else
        {
            xRetVal = eWiFiSuccess;
        }

        if ( xRetVal == eWiFiSuccess )
        {
            /* Reset the return value to failure to catch errors in connection. */
            xRetVal = eWiFiFailure;

            /* Keep trying to connect until all the retries are exhausted. */
            for( x = 0 ; x < wificonfigNUM_CONNECTION_RETRY ; x++ )
            {
                /* Try to connect to Wi-Fi. */
                if( ES_WIFI_Connect( &( xWiFiModule.xWifiObject ),
                                        pxNetworkParams->pcSSID,
                                        pxNetworkParams->pcPassword,
                                        prvConvertSecurityFromAbstractedToST( pxNetworkParams->xSecurity ) ) == ES_WIFI_STATUS_OK )
                {
                    /* Store network settings. */
                    if( ES_WIFI_GetNetworkSettings( &( xWiFiModule.xWifiObject ) ) == ES_WIFI_STATUS_OK )
                    {
                        /* Connection successful. */
                        xRetVal = eWiFiSuccess;

                        /* No more retries needed. */
                        break;
                    }
                }
            }
        }

        /* Return the semaphore. */
        xSemaphoreGive( xWiFiModule.xSemaphoreHandle );
    }
    else
    {
        xRetVal = eWiFiTimeout;
    }

    return xRetVal;
}

/*-----------------------------------------------------------*/

WIFIReturnCode_t WIFI_Disconnect( void )
{
    WIFIReturnCode_t xRetVal = eWiFiFailure;

    /* Try to acquire the semaphore. */
    if( xSemaphoreTake( xWiFiModule.xSemaphoreHandle, xSemaphoreWaitTicks ) == pdTRUE )
    {
        if( ES_WIFI_Disconnect( &( xWiFiModule.xWifiObject ) ) == ES_WIFI_STATUS_OK )
        {
            /* This variable is not updated by the driver after a disconnect. */
        	xWiFiModule.xWifiObject.NetSettings.IsConnected = 0;

            /* Store network settings. After a disconnect the IP address under NetSettings becomes 0.0.0.0 */
            if( ES_WIFI_GetNetworkSettings( &( xWiFiModule.xWifiObject ) ) == ES_WIFI_STATUS_OK )
            {
                /* Disconnection successful. */
                xRetVal = eWiFiSuccess;
            }
        }

        /* Return the semaphore. */
        xSemaphoreGive( xWiFiModule.xSemaphoreHandle );
    }
    else
    {
        xRetVal = eWiFiTimeout;
    }

    return xRetVal;
}

/*-----------------------------------------------------------*/

WIFIReturnCode_t WIFI_Reset( void )
{
    WIFIReturnCode_t xRetVal = eWiFiFailure;

    /* Try to acquire the semaphore. */
    if( xSemaphoreTake( xWiFiModule.xSemaphoreHandle, xSemaphoreWaitTicks ) == pdTRUE )
    {
        /* Reset command gives error so hard resetting */
        ES_WIFI_Init( &( xWiFiModule.xWifiObject ) );
        xRetVal = eWiFiSuccess;

        /* Return the semaphore. */
        xSemaphoreGive( xWiFiModule.xSemaphoreHandle );
    }
    else
    {
        xRetVal = eWiFiTimeout;
    }

    return xRetVal;
}


/*-----------------------------------------------------------*/

WIFIReturnCode_t WIFI_SetMode( WIFIDeviceMode_t xDeviceMode )
{
    WIFIReturnCode_t xRetVal = eWiFiNotSupported;

    return xRetVal;
}

/*-----------------------------------------------------------*/
WIFIReturnCode_t WIFI_GetMode( WIFIDeviceMode_t * pxDeviceMode )
{
    WIFIReturnCode_t xRetVal = eWiFiNotSupported;

    return xRetVal;
}
/*-----------------------------------------------------------*/


WIFIReturnCode_t WIFI_NetworkAdd( const WIFINetworkProfile_t * const pxNetworkProfile,
                                  uint16_t * pusIndex )
{
    WIFIReturnCode_t xRetVal = eWiFiNotSupported;

    return xRetVal;
}

/*-----------------------------------------------------------*/

WIFIReturnCode_t WIFI_NetworkGet( WIFINetworkProfile_t * pxNetworkProfile,
                                  uint16_t usIndex )

{
    WIFIReturnCode_t xRetVal = eWiFiNotSupported;

    return xRetVal;
}

/*-----------------------------------------------------------*/

WIFIReturnCode_t WIFI_NetworkDelete( uint16_t usIndex )
{
    WIFIReturnCode_t xRetVal = eWiFiNotSupported;

    return xRetVal;
}

/*-----------------------------------------------------------*/

WIFIReturnCode_t WIFI_Ping( uint8_t * pucIPAddr,
                            uint16_t usCount,
                            uint32_t ulIntervalMS )
{
    WIFIReturnCode_t xRetVal = eWiFiFailure;

    configASSERT( pucIPAddr != NULL );

    /* Try to acquire the semaphore. */
    if( xSemaphoreTake( xWiFiModule.xSemaphoreHandle, xSemaphoreWaitTicks ) == pdTRUE )
    {
        if( ES_WIFI_Ping( &xWiFiModule.xWifiObject, pucIPAddr, usCount, ulIntervalMS ) == ES_WIFI_STATUS_OK )
        {
            xRetVal = eWiFiSuccess;
        }

        /* Return the semaphore. */
        xSemaphoreGive( xWiFiModule.xSemaphoreHandle );
    }
    else
    {
        xRetVal = eWiFiTimeout;
    }

    return xRetVal;
}

/*-----------------------------------------------------------*/

WIFIReturnCode_t WIFI_GetIP( uint8_t * pucIPAddr )
{
    WIFIReturnCode_t xRetVal = eWiFiFailure;

    configASSERT( pucIPAddr != NULL );

    /* Try to acquire the semaphore. */
    if( xSemaphoreTake( xWiFiModule.xSemaphoreHandle, xSemaphoreWaitTicks ) == pdTRUE )
    {
        if( xWiFiModule.xWifiObject.NetSettings.IsConnected )
        {
            memcpy( pucIPAddr, xWiFiModule.xWifiObject.NetSettings.IP_Addr, 4 );
            xRetVal = eWiFiSuccess;
        }

        /* Return the semaphore. */
        xSemaphoreGive( xWiFiModule.xSemaphoreHandle );
    }
    else
    {
        xRetVal = eWiFiTimeout;
    }

    return xRetVal;
}

/*-----------------------------------------------------------*/

WIFIReturnCode_t WIFI_GetMAC( uint8_t * pucMac )
{
    WIFIReturnCode_t xRetVal = eWiFiFailure;

    configASSERT( pucMac != NULL );

    /* Try to acquire the semaphore. */
    if( xSemaphoreTake( xWiFiModule.xSemaphoreHandle, xSemaphoreWaitTicks ) == pdTRUE )
    {
        if( ES_WIFI_GetMACAddress( &xWiFiModule.xWifiObject, pucMac ) == ES_WIFI_STATUS_OK )
        {
            xRetVal = eWiFiSuccess;
        }

        /* Return the semaphore. */
        xSemaphoreGive( xWiFiModule.xSemaphoreHandle );
    }
    else
    {
        xRetVal = eWiFiTimeout;
    }

    return xRetVal;
}

/*-----------------------------------------------------------*/

WIFIReturnCode_t WIFI_GetHostIP( char * pcHost,
                                 uint8_t * pucIPAddr )
{
    WIFIReturnCode_t xRetVal = eWiFiFailure;

    configASSERT( pcHost != NULL );
    configASSERT( pucIPAddr != NULL );

    /* Try to acquire the semaphore. */
    if( xSemaphoreTake( xWiFiModule.xSemaphoreHandle, xSemaphoreWaitTicks ) == pdTRUE )
    {
        if( ES_WIFI_DNS_LookUp( &xWiFiModule.xWifiObject, pcHost, pucIPAddr ) == ES_WIFI_STATUS_OK )
        {
            xRetVal = eWiFiSuccess;
        }

        /* Return the semaphore. */
        xSemaphoreGive( xWiFiModule.xSemaphoreHandle );
    }
    else
    {
        xRetVal = eWiFiTimeout;
    }

    return xRetVal;
}

/*-----------------------------------------------------------*/

/*Scan fails if the command buffer is too small to fit in scan result and it returns
 * IO error, see es_wifi_conf.h*/
WIFIReturnCode_t WIFI_Scan( WIFIScanResult_t * pxBuffer,
                            uint8_t ucNumNetworks )
{
    WIFIReturnCode_t xRetVal = eWiFiFailure;
    uint32_t x;

    ES_WIFI_APs_t xESWifiAPs;

    configASSERT( pxBuffer != NULL );

    /* Try to acquire the semaphore. */
    if( xSemaphoreTake( xWiFiModule.xSemaphoreHandle, xSemaphoreWaitTicks ) == pdTRUE )
    {
        if( ES_WIFI_ListAccessPoints( &xWiFiModule.xWifiObject, &xESWifiAPs ) == ES_WIFI_STATUS_OK )
        {
            for( x = 0 ; x < ucNumNetworks ; x++ )
            {
                pxBuffer[ x ].xSecurity = prvConvertSecurityFromSTToAbstracted( xESWifiAPs.AP[ x ].Security );

                strncpy( ( char * ) pxBuffer[ x ].cSSID,
                         ( char * ) xESWifiAPs.AP[ x ].SSID,
                         wificonfigMAX_SSID_LEN );

                pxBuffer[ x ].cRSSI = xESWifiAPs.AP[ x ].RSSI;

                memcpy( pxBuffer[ x ].ucBSSID,
                		xESWifiAPs.AP[ x ].MAC,
                        wificonfigMAX_BSSID_LEN );
            }

            xRetVal = eWiFiSuccess;
        }

        /* Return the semaphore. */
        xSemaphoreGive( xWiFiModule.xSemaphoreHandle );
    }
    else
    {
        xRetVal = eWiFiTimeout;
    }

    return xRetVal;
}

/*-----------------------------------------------------------*/
WIFIReturnCode_t WIFI_StartAP( void )
{
    /*WIFI_ConfigureAP configures and start the soft AP . */
    WIFIReturnCode_t xRetVal = eWiFiNotSupported;

    return xRetVal;
}

/*-----------------------------------------------------------*/
WIFIReturnCode_t WIFI_StopAP( void )
{
    /*SoftAP mode stops after a timeout. */
    WIFIReturnCode_t xRetVal = eWiFiNotSupported;

    return xRetVal;
}

/*-----------------------------------------------------------*/

WIFIReturnCode_t WIFI_ConfigureAP( const WIFINetworkParams_t * const pxNetworkParams )
{
    WIFIReturnCode_t xRetVal = eWiFiFailure;
    ES_WIFI_APConfig_t xApConfig;

    configASSERT( pxNetworkParams != NULL );
    configASSERT( pxNetworkParams->pcSSID != NULL );

    strncpy( ( char * ) xApConfig.SSID,
             ( char * ) pxNetworkParams->pcSSID,
             ES_WIFI_MAX_SSID_NAME_SIZE );

    if ( pxNetworkParams->xSecurity != eWiFiSecurityOpen ) 
    {
        configASSERT( pxNetworkParams->pcPassword != NULL );

        strncpy( ( char * ) xApConfig.Pass,
                 ( char * ) pxNetworkParams->pcPassword,
                 ES_WIFI_MAX_PSWD_NAME_SIZE );
    }
    
    xApConfig.Channel = pxNetworkParams->cChannel;
    xApConfig.MaxConnections = wificonfigMAX_CONNECTED_STATIONS;
    xApConfig.Security = prvConvertSecurityFromAbstractedToST( pxNetworkParams->xSecurity );

    /* Try to acquire the semaphore. */
    if( xSemaphoreTake( xWiFiModule.xSemaphoreHandle, xSemaphoreWaitTicks ) == pdTRUE )
    {
        /* Activate Soft AP. */
        if( ES_WIFI_ActivateAP( &xWiFiModule.xWifiObject, &xApConfig ) == ES_WIFI_STATUS_OK )
        {
            xRetVal = eWiFiSuccess;
        }

        /* Return the semaphore. */
        xSemaphoreGive( xWiFiModule.xSemaphoreHandle );
    }
    else
    {
        xRetVal = eWiFiTimeout;
    }

    return xRetVal;
}
/*-----------------------------------------------------------*/

WIFIReturnCode_t WIFI_SetPMMode( WIFIPMMode_t xPMModeType,
                                 const void * pvOptionValue )
{
    WIFIReturnCode_t xRetVal = eWiFiNotSupported;

    return xRetVal;
}
/*-----------------------------------------------------------*/

WIFIReturnCode_t WIFI_GetPMMode( WIFIPMMode_t * pxPMModeType,
                                 void * pvOptionValue )
{
    WIFIReturnCode_t xRetVal = eWiFiNotSupported;

    return xRetVal;
}
/*-----------------------------------------------------------*/

BaseType_t WIFI_IsConnected( void )
{
    BaseType_t xIsConnected = pdFALSE;
    /* Expected result from ES_WIFI_IsConnected() when the board is connected to Wi-Fi. */
    const uint8_t uConnected = 1;

    if ( ES_WIFI_IsConnected( &xWiFiModule.xWifiObject ) == uConnected ) {
        xIsConnected = pdTRUE;
    }

    return xIsConnected;
}
