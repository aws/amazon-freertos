/*
 * FreeRTOS BLE V2.1.0
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
 * @file iot_ble_wifi_prov_test_access_define.h
 * @brief Definitions for functions that access private methods in aws_ble_wifi_provisioning.c
 *
 * Required to test the private methods in  aws_ble_wifi_provisioning.c
 */
#ifndef IOT_BLE_WIFI_PROV_TEST_ACCESS_DEFINE_H_
#define IOT_BLE_WIFI_PROV_TEST_ACCESS_DEFINE_H_



BaseType_t test_HandleListNetworkRequest( uint8_t * data,
                                          size_t length )
{
    return _handleListNetworkRequest( data, length );
}

BaseType_t test_HandleSaveNetworkRequest( uint8_t * data,
                                          size_t length )
{
    return _handleSaveNetworkRequest( data, length );
}


BaseType_t test_HandleEditNetworkRequest( uint8_t * data,
                                          size_t length )
{
    return _handleEditNetworkRequest( data, length );
}


BaseType_t test_HandleDeleteNetworkRequest( uint8_t * data,
                                            size_t length )
{
    return _handleDeleteNetworkRequest( data, length );
}

WIFIReturnCode_t test_AppendNetwork( WIFINetworkProfile_t * pProfile )
{
    return _appendNetwork( pProfile );
}

WIFIReturnCode_t test_AddNewNetwork( WIFINetworkProfile_t * pProfile,
                                     bool connect )
{
    return _addNewNetwork( pProfile, connect );
}

WIFIReturnCode_t test_PopNetwork( uint16_t index,
                                  WIFINetworkProfile_t * pProfile )
{
    return _popNetwork( index, pProfile );
}

WIFIReturnCode_t test_MoveNetwork( uint16_t usCurrentIndex,
                                   uint16_t usNewIndex )
{
    return _moveNetwork( usCurrentIndex, usNewIndex );
}

WIFIReturnCode_t test_GetSavedNetwork( uint16_t index,
                                       WIFINetworkProfile_t * pProfile )
{
    return _getSavedNetwork( index, pProfile );
}

WIFIReturnCode_t test_ConnectSavedNetwork( uint16_t index )
{
    return _connectSavedNetwork( index );
}

BaseType_t test_GetConnectedNetwork( WIFINetworkProfile_t * pNetwork )
{
    BaseType_t ret = pdFALSE;

    if( _getSavedNetwork( wifiProvisioning.connectedIdx, pNetwork ) == eWiFiSuccess )
    {
        ret = pdTRUE;
    }

    return ret;
}

#endif /* IOT_BLE_WIFI_PROV_TEST_ACCESS_DEFINE_H_ */
