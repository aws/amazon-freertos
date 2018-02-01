/*
 * Amazon FreeRTOS Wi-Fi V1.0.0
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
 * @brief WiFi Interface.
 */

/* Socket and WiFi interface includes. */
#include "aws_wifi.h"

/* WiFi configuration includes. */
#include "aws_wifi_config.h"

/*-----------------------------------------------------------*/

WIFIReturnCode_t WIFI_On( void )
{
    /* FIX ME. */
    return eWiFiFailure;
}
/*-----------------------------------------------------------*/

WIFIReturnCode_t WIFI_ConnectAP( const WIFINetworkParams_t * const pxNetworkParams )
{
    /* FIX ME. */
    return eWiFiFailure;
}
/*-----------------------------------------------------------*/

WIFIReturnCode_t WIFI_Disconnect( void )
{
    /* FIX ME. */
    return eWiFiFailure;
}
/*-----------------------------------------------------------*/

WIFIReturnCode_t WIFI_Reset( void )
{
    /* FIX ME. */
    return eWiFiFailure;
}
/*-----------------------------------------------------------*/

WIFIReturnCode_t WIFISetMode( WIFIDeviceMode_t xDeviceMode )
{
    /* FIX ME. */
    return eWiFiFailure;
}
/*-----------------------------------------------------------*/

WIFIReturnCode_t WIFIGetMode( WIFIDeviceMode_t * pxDeviceMode )
{
    /* FIX ME. */
    return eWiFiFailure;
}
/*-----------------------------------------------------------*/

int16_t WIFINetworkAdd( WIFINetworkParams_t * pxNetworkParams )
{
    /* FIX ME. */
    return eWiFiFailure;
}
/*-----------------------------------------------------------*/

WIFIReturnCode_t WIFINetworkGet( WIFINetworkParams_t * pxNetworkParams,
                                            uint16_t uxIndex )

{
    /* FIX ME. */
    return eWiFiFailure;
}
/*-----------------------------------------------------------*/

WIFIReturnCode_t WIFINetworkDelete( uint16_t uxIndex )
{
    /* FIX ME. */
    return eWiFiFailure;
}
/*-----------------------------------------------------------*/

WIFIReturnCode_t WIFI_Ping( uint8_t * pxIPAddr,
                                       uint16_t xCount,
                                       uint32_t xIntervalMS )
{
    /* FIX ME. */
    return eWiFiFailure;
}
/*-----------------------------------------------------------*/

WIFIReturnCode_t WIFI_GetIP( uint8_t * pxIPAddr )
{
    /* FIX ME. */
    return eWiFiFailure;
}
/*-----------------------------------------------------------*/

WIFIReturnCode_t WIFI_GetMAC( uint8_t * pxMac )
{
    /* FIX ME. */
    return eWiFiFailure;
}
/*-----------------------------------------------------------*/

WIFIReturnCode_t WIFI_GetHostIP( char * pxHost,
                                            uint8_t * pxIPAddr )
{
    /* FIX ME. */
    return eWiFiFailure;
}
/*-----------------------------------------------------------*/

WIFIReturnCode_t WIFI_Scan( WIFIScanResult_t * pxBuffer,
                                       uint8_t uxNumNetworks )
{
    /* FIX ME. */
    return eWiFiFailure;
}
/*-----------------------------------------------------------*/

WIFIReturnCode_t WIFI_ConfigureAP( const WIFINetworkParams_t * pxNetworkParams )
{
    /* FIX ME. */
    return eWiFiFailure;
}
/*-----------------------------------------------------------*/
