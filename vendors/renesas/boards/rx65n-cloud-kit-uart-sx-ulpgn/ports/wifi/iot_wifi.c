/*
 * Amazon FreeRTOS Wi-Fi V1.0.0
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
 * @file aws_wifi.c
 * @brief Wi-Fi Interface.
 */
#include <stdio.h>
#include <string.h>
/* FreeRTOS includes. */
#include "FreeRTOS.h"

/* Socket and WiFi interface includes. */
#include "iot_wifi.h"

/* WiFi configuration includes. */
#include "aws_wifi_config.h"

/* WiFi configuration includes. */
#include "platform.h"
#include "r_sci_rx_if.h"
#include "r_wifi_sx_ulpgn_if.h"
#include "FreeRTOSIPConfig.h"

/**
 * @brief Wi-Fi initialization status.
 */
static BaseType_t xWIFIInitDone;
static uint32_t prvConvertSecurityFromSilexAT( WIFISecurity_t xSecurity );

static uint32_t prvConvertSecurityFromSilexAT( WIFISecurity_t xSecurity )
{
    uint32_t xConvertedSecurityType = WIFI_SECURITY_UNDEFINED;

    switch( xSecurity )
    {
        case eWiFiSecurityOpen:
            xConvertedSecurityType = WIFI_SECURITY_OPEN;
            break;

        case eWiFiSecurityWEP:
            xConvertedSecurityType = WIFI_SECURITY_WEP;
            break;

        case eWiFiSecurityWPA:
            xConvertedSecurityType = WIFI_SECURITY_WPA;
            break;

        case eWiFiSecurityWPA2:
            xConvertedSecurityType = WIFI_SECURITY_WPA2;
            break;

        case eWiFiSecurityNotSupported:
            xConvertedSecurityType = WIFI_SECURITY_UNDEFINED;
            break;
    }

    return xConvertedSecurityType;
}

/*-----------------------------------------------------------*/

WIFIReturnCode_t WIFI_On( void )
{
    /* FIX ME. */
    WIFIReturnCode_t xRetVal = eWiFiFailure;

    /* One time Wi-Fi initialization */
    if( xWIFIInitDone == pdFALSE )
    {
#if 0
    	/* This buffer is used to store the semaphore's data structure
         * and therefore must be static. */
        static StaticSemaphore_t xSemaphoreBuffer;

        /* Start with all the zero. */
        memset( &( xWiFiModule ), 0, sizeof( xWiFiModule ) );

        /* Create the semaphore used to serialize Wi-Fi module operations. */
        xWiFiModule.xSemaphoreHandle = xSemaphoreCreateMutexStatic( &( xSemaphoreBuffer ) );

        /* Initialize semaphore. */
        xSemaphoreGive( xWiFiModule.xSemaphoreHandle );
#endif
        /* Wi-Fi init done*/
        xWIFIInitDone = pdTRUE;
    }
    if(WIFI_SUCCESS == R_WIFI_SX_ULPGN_Open())
    {
    	xRetVal = eWiFiSuccess;
    }

	return xRetVal;
}
/*-----------------------------------------------------------*/

WIFIReturnCode_t WIFI_Off( void )
{
    /* FIX ME. */
    R_WIFI_SX_ULPGN_Close();

    return eWiFiSuccess;
}
/*-----------------------------------------------------------*/

//eWiFiSecurityOpen = 0, /**< Open - No Security. */
//eWiFiSecurityWEP,      /**< WEP Security. */
//eWiFiSecurityWPA,      /**< WPA Security. */
//eWiFiSecurityWPA2,     /**< WPA2 Security. */

WIFIReturnCode_t WIFI_ConnectAP( const WIFINetworkParams_t * const pxNetworkParams )
{
	wifi_err_t ret;
	wifi_ip_configuration_t ipconfig;
	uint8_t dhcp_enable;
	uint32_t convert_security;
	convert_security = prvConvertSecurityFromSilexAT(pxNetworkParams->xSecurity);

    if( NULL == pxNetworkParams || NULL == pxNetworkParams->pcSSID || NULL == pxNetworkParams->pcPassword )
    {
        return eWiFiFailure;
    }

    if ( pxNetworkParams->xSecurity >= eWiFiSecurityNotSupported)
    {
        return eWiFiFailure;
    }

    if( NULL == pxNetworkParams->pcPassword &&
        eWiFiSecurityOpen != pxNetworkParams->xSecurity )
    {
        return eWiFiFailure;
    }

    if ( pxNetworkParams->ucSSIDLength > wificonfigMAX_SSID_LEN )
    {
        return eWiFiFailure;
    }

    if ( pxNetworkParams->ucPasswordLength > wificonfigMAX_PASSPHRASE_LEN )
    {
        return eWiFiFailure;
    }

    convert_security = prvConvertSecurityFromSilexAT(
            pxNetworkParams->xSecurity);
    if(pdTRUE == WIFI_IsConnected())
    {
    	WIFI_Disconnect();
    }

#if	(ipconfigUSE_DHCP == 0)
	dhcp_enable = 0;
	ipconfig.ipaddress = ((uint32_t)configIP_ADDR0 << 24) | ((uint32_t)configIP_ADDR1 << 16) | ((uint32_t)configIP_ADDR2 << 8) | (uint32_t)configIP_ADDR3;
	ipconfig.subnetmask = ((uint32_t)configNET_MASK0 << 24) | ((uint32_t)configNET_MASK1 << 16) | ((uint32_t)configNET_MASK2 << 8) | (uint32_t)configNET_MASK3;
	ipconfig.gateway = ((uint32_t)configGATEWAY_ADDR0 << 24) | ((uint32_t)configGATEWAY_ADDR1 << 16) | ((uint32_t)configGATEWAY_ADDR2 << 8) | (uint32_t)configGATEWAY_ADDR3;
#else
	dhcp_enable = 1;
#endif
	ret = R_WIFI_SX_ULPGN_Connect(
			(uint8_t *)pxNetworkParams->pcSSID,
			(uint8_t *)pxNetworkParams->pcPassword,
			convert_security,
			dhcp_enable,
			&ipconfig
	);

	if(WIFI_SUCCESS != ret)
	{
		return eWiFiFailure;
	}


	return eWiFiSuccess;
}
/*-----------------------------------------------------------*/

WIFIReturnCode_t WIFI_Disconnect( void )
{
    /* FIX ME. */
	R_WIFI_SX_ULPGN_Disconnect();
    return eWiFiSuccess;
}
/*-----------------------------------------------------------*/

WIFIReturnCode_t WIFI_Reset( void )
{
    /* FIX ME. */
	WIFIReturnCode_t ret;
    WIFI_Off();
    ret = WIFI_On();
    return ret;
}
/*-----------------------------------------------------------*/

WIFIReturnCode_t WIFI_Scan( WIFIScanResult_t * pxBuffer,
                            uint8_t ucNumNetworks )
{
    WIFIReturnCode_t result = eWiFiFailure;
    wifi_err_t ret;
    uint32_t list;

	ret = R_WIFI_SX_ULPGN_Scan(pxBuffer, ucNumNetworks, &list);
	if(WIFI_SUCCESS == ret)
	{
		result = eWiFiSuccess;
	}
    return result;
}
/*-----------------------------------------------------------*/

WIFIReturnCode_t WIFI_SetMode( WIFIDeviceMode_t xDeviceMode )
{
    /* FIX ME. */
    return eWiFiNotSupported;
}
/*-----------------------------------------------------------*/

WIFIReturnCode_t WIFI_GetMode( WIFIDeviceMode_t * pxDeviceMode )
{
    /* FIX ME. */
    return eWiFiNotSupported;
}
/*-----------------------------------------------------------*/

WIFIReturnCode_t WIFI_NetworkAdd( const WIFINetworkProfile_t * const pxNetworkProfile,
                                  uint16_t * pusIndex )
{
    /* FIX ME. */
    return eWiFiNotSupported;
}
/*-----------------------------------------------------------*/

WIFIReturnCode_t WIFI_NetworkGet( WIFINetworkProfile_t * pxNetworkProfile,
                                  uint16_t usIndex )
{
    /* FIX ME. */
    return eWiFiNotSupported;
}
/*-----------------------------------------------------------*/

WIFIReturnCode_t WIFI_NetworkDelete( uint16_t usIndex )
{
    /* FIX ME. */
    return eWiFiNotSupported;
}
/*-----------------------------------------------------------*/

WIFIReturnCode_t WIFI_Ping( uint8_t * pucIPAddr,
                            uint16_t usCount,
                            uint32_t ulIntervalMS )
{
    /* FIX ME. */
    WIFIReturnCode_t result = eWiFiFailure;
    wifi_err_t ret;
    uint32_t ipaddress;
    if(NULL == pucIPAddr)
    {
    	return eWiFiFailure;
    }
	ipaddress = ((uint32_t)pucIPAddr[0] << 24) | ((uint32_t)pucIPAddr[1] << 16) | ((uint32_t)pucIPAddr[2] << 8) | ((uint32_t)pucIPAddr[3]);
	ret = R_WIFI_SX_ULPGN_Ping(ipaddress, usCount, ulIntervalMS);
	if(WIFI_SUCCESS == ret)
	{
		result = eWiFiSuccess;
	}
    return result;
}
/*-----------------------------------------------------------*/

WIFIReturnCode_t WIFI_GetIP( uint8_t * pucIPAddr )
{
    /* FIX ME. */
	wifi_ip_configuration_t ipconfig;
	if(NULL == pucIPAddr)
	{
		return eWiFiFailure;
	}
	if(WIFI_SUCCESS != R_WIFI_SX_ULPGN_GetIpAddress(&ipconfig))
	{
		return eWiFiFailure;
	}
	pucIPAddr[0] = (uint8_t)(ipconfig.ipaddress >> 24);
	pucIPAddr[1] = (uint8_t)(ipconfig.ipaddress >> 16);
	pucIPAddr[2] = (uint8_t)(ipconfig.ipaddress >> 8);
	pucIPAddr[3] = (uint8_t)(ipconfig.ipaddress);
	return eWiFiSuccess;
}
/*-----------------------------------------------------------*/

WIFIReturnCode_t WIFI_GetMAC( uint8_t * pucMac )
{
    /* FIX ME. */
	if(NULL == pucMac)
	{
		return eWiFiFailure;
	}
	if(WIFI_SUCCESS != R_WIFI_SX_ULPGN_GetMacAddress(pucMac))
	{
		return eWiFiFailure;
	}
    return eWiFiSuccess;
}
/*-----------------------------------------------------------*/

WIFIReturnCode_t WIFI_GetHostIP( char * pcHost,
                                 uint8_t * pucIPAddr )
{
    /* FIX ME. */
	uint32_t ipaddress;
	if((NULL == pcHost) || (NULL == pucIPAddr))
	{
		return eWiFiFailure;
	}
	if(WIFI_SUCCESS != R_WIFI_SX_ULPGN_DnsQuery(pcHost, &ipaddress))
	{
		return eWiFiFailure;
	}
	pucIPAddr[0] = (uint8_t)(ipaddress >> 24);
	pucIPAddr[1] = (uint8_t)(ipaddress >> 16);
	pucIPAddr[2] = (uint8_t)(ipaddress >> 8);
	pucIPAddr[3] = (uint8_t)(ipaddress);

    return eWiFiSuccess;
}
/*-----------------------------------------------------------*/

WIFIReturnCode_t WIFI_StartAP( void )
{
    /* FIX ME. */
    return eWiFiNotSupported;
}
/*-----------------------------------------------------------*/

WIFIReturnCode_t WIFI_StopAP( void )
{
    /* FIX ME. */
    return eWiFiNotSupported;
}
/*-----------------------------------------------------------*/

WIFIReturnCode_t WIFI_ConfigureAP( const WIFINetworkParams_t * const pxNetworkParams )
{
    /* FIX ME. */
    return eWiFiNotSupported;
}
/*-----------------------------------------------------------*/

WIFIReturnCode_t WIFI_SetPMMode( WIFIPMMode_t xPMModeType,
                                 const void * pvOptionValue )
{
    /* FIX ME. */
    return eWiFiNotSupported;
}
/*-----------------------------------------------------------*/

WIFIReturnCode_t WIFI_GetPMMode( WIFIPMMode_t * pxPMModeType,
                                 void * pvOptionValue )
{
    /* FIX ME. */
    return eWiFiNotSupported;
}
/*-----------------------------------------------------------*/

WIFIReturnCode_t WIFI_RegisterNetworkStateChangeEventCallback( IotNetworkStateChangeEventCallback_t xCallback )
{
    /* FIX ME. */
    return eWiFiNotSupported;
}
/*-----------------------------------------------------------*/


BaseType_t WIFI_IsConnected(void)
{
	BaseType_t ret = pdFALSE;
	/* FIX ME. */
	if(0 ==  R_WIFI_SX_ULPGN_IsConnected())
	{
		ret = pdTRUE;
	}
	return ret;
}
