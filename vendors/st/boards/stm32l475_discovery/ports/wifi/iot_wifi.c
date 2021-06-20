/*
 * FreeRTOS Wi-Fi STM32L4 Discovery kit IoT node V1.0.3
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
 * @file iot_wifi.c
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
#include "iot_wifi.h"

/**
 * @brief The credential set to use for TLS on the Inventek module.
 *
 * @note This is hard-coded to 3 because we are using re-writable
 * credential slot.
 */
#define wifiOFFLOAD_SSL_CREDS_SLOT      ( 3 )

/*-----------------------------------------------------------*/

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

        case eWiFiSecurityWPA2_ent:
        case eWiFiSecurityWPA3:
        case eWiFiSecurityNotSupported:
            xConvertedSecurityType = ES_WIFI_SEC_UNKNOWN;
            break;
    }

    return xConvertedSecurityType;
}

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
static size_t prvByteArrayToString( char *pcDest, const void *pucSrc, size_t xLen, size_t xCap )
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

    if( pxNetworkParams->ucSSIDLength == 0 
        || pxNetworkParams->ucSSIDLength > wificonfigMAX_SSID_LEN )
    {
        return eWiFiFailure; 
    } 

    if ( pxNetworkParams->xSecurity != eWiFiSecurityOpen )
    {
        if( pxNetworkParams->xPassword.xWPA.ucLength == 0 
            || pxNetworkParams->xPassword.xWPA.ucLength > wificonfigMAX_PASSPHRASE_LEN )
        {
            return eWiFiFailure;
        }
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

            char pcSSID[ pxNetworkParams->ucSSIDLength + 1 ];
            char pcPassword[ wificonfigMAX_PASSPHRASE_LEN + 1];
            prvByteArrayToString( pcSSID,
					     		  pxNetworkParams->ucSSID,
								  pxNetworkParams->ucSSIDLength,
								  wificonfigMAX_SSID_LEN );
            prvByteArrayToString( pcPassword,
								  pxNetworkParams->xPassword.xWPA.cPassphrase,
								  pxNetworkParams->xPassword.xWPA.ucLength,
								  wificonfigMAX_PASSPHRASE_LEN );

            /* Keep trying to connect until all the retries are exhausted. */
            for( x = 0 ; x < wificonfigNUM_CONNECTION_RETRY ; x++ )
            {
                /* Try to connect to Wi-Fi. */
                if( ES_WIFI_Connect( &( xWiFiModule.xWifiObject ),
                                        pcSSID,
                                        pcPassword,
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

    if( ( NULL == pucIPAddr ) || ( 0 == usCount ) )
    {
        return eWiFiFailure;
    }

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

WIFIReturnCode_t WIFI_GetIPInfo( WIFIIPConfiguration_t * xIPConfig )
{
    WIFIReturnCode_t xRetVal = eWiFiFailure;
    
    if( xIPConfig == NULL )
    {
        return eWiFiFailure;
    }    

    memset( xIPConfig, 0, sizeof( WIFIIPConfiguration_t ) );

    /* Try to acquire the semaphore. */
    if( xSemaphoreTake( xWiFiModule.xSemaphoreHandle, xSemaphoreWaitTicks ) == pdTRUE )
    {
        if( xWiFiModule.xWifiObject.NetSettings.IsConnected )
        {
            memcpy( (uint8_t *)&xIPConfig->xIPAddress.ulAddress[0], xWiFiModule.xWifiObject.NetSettings.IP_Addr, 4 );
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
    size_t xHostnameLength;

    configASSERT( pcHost != NULL );
    configASSERT( pucIPAddr != NULL );

    /* The inventek WiFi module supports maximum domain name length of 64.
     * Trying to resolve a longer name leaves the module in a non-responsive
     * state. The following check prevents calling ES_WIFI_DNS_LookUp for
     * longer names, thereby preventing the module from going into a
     * non-responsive state.
     *
     * The following is from the datasheet of the module:
     *
     * 'D0' DNS Lookup:
     * This command performs a DNS lookup of a Domain Name to get its IPv4
     * address. The Domain Name is limited to 64 characters. */
    xHostnameLength = strlen( pcHost );
    if( xHostnameLength > 64 )
    {
        return eWiFiFailure;
    }

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

                strncpy( ( char * ) pxBuffer[ x ].ucSSID,
                         ( char * ) xESWifiAPs.AP[ x ].SSID,
                         wificonfigMAX_SSID_LEN );
                pxBuffer[ x ].ucSSIDLength = strnlen( ( char * ) xESWifiAPs.AP[ x ].SSID, wificonfigMAX_SSID_LEN );

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
    
    if( pxNetworkParams->ucSSIDLength == 0 
        || pxNetworkParams->ucSSIDLength > wificonfigMAX_SSID_LEN )
    {
        return eWiFiFailure; 
    } 

    if ( pxNetworkParams->xSecurity != eWiFiSecurityOpen )
    {
        if( pxNetworkParams->xPassword.xWPA.ucLength == 0 
            || pxNetworkParams->xPassword.xWPA.ucLength > wificonfigMAX_PASSPHRASE_LEN )
        {
            return eWiFiFailure;
        }
    }

    char pcSSID[ pxNetworkParams->ucSSIDLength + 1 ];
    prvByteArrayToString( pcSSID,
			     		  pxNetworkParams->ucSSID,
						  pxNetworkParams->ucSSIDLength,
						  wificonfigMAX_SSID_LEN );
    strncpy( ( char * ) xApConfig.SSID, pcSSID, ES_WIFI_MAX_SSID_NAME_SIZE );

    if ( pxNetworkParams->xSecurity != eWiFiSecurityOpen )
    {
        configASSERT( pxNetworkParams->xPassword.xWPA.ucLength > 0 );

        char pcPassword[ wificonfigMAX_PASSPHRASE_LEN + 1];
        prvByteArrayToString( pcPassword,
    						  pxNetworkParams->xPassword.xWPA.cPassphrase,
    						  pxNetworkParams->xPassword.xWPA.ucLength,
    						  wificonfigMAX_PASSPHRASE_LEN );
        strncpy( ( char * ) xApConfig.Pass, pcPassword, ES_WIFI_MAX_PSWD_NAME_SIZE );
    }

    xApConfig.Channel = pxNetworkParams->ucChannel;
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

BaseType_t WIFI_IsConnected( const WIFINetworkParams_t * pxNetworkParams )
{
    BaseType_t xIsConnected = pdFALSE;
    /* Expected result from ES_WIFI_IsConnected() when the board is connected to Wi-Fi. */
    const uint8_t uConnected = 1;

    /* Try to acquire the semaphore. */
    if( xSemaphoreTake( xWiFiModule.xSemaphoreHandle, xSemaphoreWaitTicks ) == pdTRUE )
    {
        /* Check whether or not the WiFi module is connected to any AP. */
        if ( ES_WIFI_IsConnected( &xWiFiModule.xWifiObject ) == uConnected )
        {
        	if ( pxNetworkParams )
        	{
        		ES_WIFIObject_t xSettings;

        		if ( pxNetworkParams->ucSSIDLength > 0
       				 && pxNetworkParams->ucSSIDLength <= wificonfigMAX_SSID_LEN
        			 &&	ES_WIFI_STATUS_OK == ES_WIFI_GetNetworkSettings( &xSettings )
					 && 0 == memcmp(xSettings.APSettings.SSID, pxNetworkParams->ucSSID, pxNetworkParams->ucSSIDLength ))
				 {
        			xIsConnected = pdTRUE;
				 }
        	}
        	else
        	{
                xIsConnected = pdTRUE;
        	}
        }

        /* Return the semaphore. */
        xSemaphoreGive( xWiFiModule.xSemaphoreHandle );
    }

    return xIsConnected;
}
/*-----------------------------------------------------------*/

#ifdef USE_OFFLOAD_SSL

    /**
     * @brief Stores the provided certificate to the re-writable slot in
     * the Inventak module.
     *
     * @param pucCertificate[in] The ceritificate to store.
     * @param usCertificateLength[in] The length of the above certificate.
     *
     * @return If certificate is stored successfully, eWiFiSuccess is
     * returned. Otherwise an error code indicating the reason of the error
     * is returned.
     */
    WIFIReturnCode_t  WIFI_StoreCertificate( uint8_t * pucCertificate, uint16_t usCertificateLength )
    {
        WIFIReturnCode_t xRetVal = eWiFiFailure;

        /* Try to acquire the semaphore. */
        if( xSemaphoreTake( xWiFiModule.xSemaphoreHandle, xSemaphoreWaitTicks ) == pdTRUE )
        {
            /* Store Certificate. */
            if( ES_WIFI_StoreCertificate( &xWiFiModule.xWifiObject,
                                          ES_WIFI_FUNCTION_TLS,
                                          wifiOFFLOAD_SSL_CREDS_SLOT,
                                          pucCertificate,
                                          usCertificateLength ) == ES_WIFI_STATUS_OK )
            {
                xRetVal = eWiFiSuccess;
            }

            /* Return the semaphore. */
            ( void ) xSemaphoreGive( xWiFiModule.xSemaphoreHandle );
        }
        else
        {
            xRetVal = eWiFiTimeout;
        }

        return xRetVal;
    }

#endif /* USE_OFFLOAD_SSL */
/*-----------------------------------------------------------*/

#ifdef USE_OFFLOAD_SSL

    /**
     * @brief Stores the provided key to the re-writable slot in the
     * Inventak module.
     *
     * @param pucKey[in] The key to store.
     * @param usKeyLength The length of the above key.
     *
     * @return If key is stored successfully, eWiFiSuccess is returned.
     * Otherwise an error code indicating the reason of the error is
     * returned.
     */
    WIFIReturnCode_t  WIFI_StoreKey( uint8_t * pucKey, uint16_t usKeyLength )
    {
        WIFIReturnCode_t xRetVal = eWiFiFailure;

        /* Try to acquire the semaphore. */
        if( xSemaphoreTake( xWiFiModule.xSemaphoreHandle, xSemaphoreWaitTicks ) == pdTRUE )
        {
            /* Store Certificate. */
            if( ES_WIFI_StoreKey( &xWiFiModule.xWifiObject,
                                  ES_WIFI_FUNCTION_TLS,
                                  wifiOFFLOAD_SSL_CREDS_SLOT,
                                  pucKey,
                                  usKeyLength ) == ES_WIFI_STATUS_OK )
            {
                xRetVal = eWiFiSuccess;
            }

            /* Return the semaphore. */
            ( void ) xSemaphoreGive( xWiFiModule.xSemaphoreHandle );
        }
        else
        {
            xRetVal = eWiFiTimeout;
        }

        return xRetVal;
    }

#endif /* USE_OFFLOAD_SSL */
/*-----------------------------------------------------------*/

/**
 * @brief Gets the Inventek module's firmware revision number.
 *
 * @param pucBuffer[out] The output buffer to return the firmware version.
 *
 * @return If firmware version is retrieved successfully, eWiFiSuccess
 * is returned. Otherwise an error code indicating the reason of the
 * error is returned.
 */
WIFIReturnCode_t WIFI_GetFirmwareVersion( uint8_t * pucBuffer )
{
    WIFIReturnCode_t xRetVal = eWiFiFailure;

    /* Try to acquire the semaphore. */
    if( xSemaphoreTake( xWiFiModule.xSemaphoreHandle, xSemaphoreWaitTicks ) == pdTRUE )
    {
        /* Get the firmware version. */
        if( ES_WIFI_GetFWRevID( &xWiFiModule.xWifiObject, pucBuffer ) == ES_WIFI_STATUS_OK )
        {
            xRetVal = eWiFiSuccess;
        }

        /* Return the semaphore. */
        ( void ) xSemaphoreGive( xWiFiModule.xSemaphoreHandle );
    }
    else
    {
        xRetVal = eWiFiTimeout;
    }

    return xRetVal;
}


WIFIReturnCode_t WIFI_RegisterEvent( WIFIEventType_t xEventType, WIFIEventHandler_t xHandler  )
{ 
    /** Needs to implement dispatching network state change events **/
    return eWiFiNotSupported;
}
/*-----------------------------------------------------------*/
