/*
 * Amazon FreeRTOS BLE HAL V1.0.0
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
 * @file iot_test_ble_hal_afqp.h
 * @brief
 */

#ifndef _IOT_TEST_BLE_HAL_AFQP_H_
#define _IOT_TEST_BLE_HAL_AFQP_H_

#include "iot_test_ble_hal_common.h"

typedef struct
{
    size_t xLength;
    uint8_t ucBuffer[ bletestsSTRINGYFIED_UUID_SIZE ];
} response_t;

void prvBLESetUp( void );
void prvWriteCheckAndResponse( bletestAttSrvB_t xAttribute,
                               bool bNeedRsp,
                               bool IsPrep,
                               uint16_t usOffset );
void prvReadCheckAndResponse( bletestAttSrvB_t xAttribute );
void pushToQueue( IotLink_t * pEventList );
void prvSetGetProperty( BTProperty_t * pxProperty,
                        bool bIsSet );
void prvSetAdvertisement( BTGattAdvertismentParams_t * pxParams,
                          uint16_t usServiceDataLen,
                          char * pcServiceData,
                          BTUuid_t * pxServiceUuid,
                          size_t xNbServices );
void prvStartStopAdvCheck( bool start );
BTStatus_t bleStackInit( void );

#endif /* _IOT_TEST_BLE_HAL_AFQP_H_ */
