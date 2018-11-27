/*
 * Amazon FreeRTOS System Initialization
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
#include "FreeRTOS.h"
#include "aws_ble_config.h"
#include "aws_ble_services_init.h"

extern BaseType_t AFRDeviceInfoSvc_Init( void );

#if( bleconfigENABLE_GATT_DEMO == 1 )
extern BaseType_t vGattDemoSvcInit( void );
#endif

extern BaseType_t AwsIotMqttBLE_Init( void );

#if ( bleconfigENABLE_WIFI_PROVISIONING == 1 )
extern BaseType_t WIFI_PROVISION_Init( void );
#endif

/*-----------------------------------------------------------*/


/**
 * @brief Initializes Amazon FreeRTOS libraries.
 */
BaseType_t BLE_SERVICES_Init()
{
    BaseType_t xResult = pdPASS;

    xResult = AFRDeviceInfoSvc_Init();

#if ( bleconfigENABLE_WIFI_PROVISIONING == 1 )
    if( xResult == pdPASS )
    {
        xResult = WIFI_PROVISION_Init();
    }
#endif

    if( xResult == pdPASS )
    {
        xResult = AwsIotMqttBLE_Init();
    }

#if( bleconfigENABLE_GATT_DEMO == 1 )
    if( xResult == pdPASS )
    {
        xResult = vGattDemoSvcInit();
    }
#endif

    return xResult;
}

