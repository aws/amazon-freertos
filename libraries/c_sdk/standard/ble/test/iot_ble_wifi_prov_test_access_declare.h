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
 * @file iot_ble_wifi_prov_test_access_declare.h
 * @brief Declarations for functions that access private methods in aws_ble_wifi_provisioning.c
 *
 * Required to test the private methods in  aws_ble_wifi_provisioning.c
 */

#ifndef IOT_BLE_WIFI_PROV_TEST_ACCESS_DECLARE_H_
#define IOT_BLE_WIFI_PROV_TEST_ACCESS_DECLARE_H_

#include <stdint.h>
#include <stddef.h>
#include "iot_wifi.h"


BaseType_t test_HandleListNetworkRequest( uint8_t * pucData,
                                          size_t xLength );

BaseType_t test_HandleSaveNetworkRequest( uint8_t * pucData,
                                          size_t xLength );

BaseType_t test_HandleEditNetworkRequest( uint8_t * pucData,
                                          size_t xLength );

BaseType_t test_HandleDeleteNetworkRequest( uint8_t * pucData,
                                            size_t xLength );


WIFIReturnCode_t test_AppendNetwork( WIFINetworkProfile_t * pxProfile );

WIFIReturnCode_t test_AddNewNetwork( WIFINetworkProfile_t * pxProfile,
                                     bool connect );

WIFIReturnCode_t test_PopNetwork( uint16_t usIndex,
                                  WIFINetworkProfile_t * pxProfile );

WIFIReturnCode_t test_MoveNetwork( uint16_t usCurrentIndex,
                                   uint16_t usNewIndex );

WIFIReturnCode_t test_ConnectSavedNetwork( uint16_t usIndex );

WIFIReturnCode_t test_GetSavedNetwork( uint16_t usIndex,
                                       WIFINetworkProfile_t * pxProfile );

BaseType_t test_GetConnectedNetwork( WIFINetworkProfile_t * pxNetwork );

#endif /* IOT_BLE_WIFI_PROV_TEST_ACCESS_DECLARE_H_ */
