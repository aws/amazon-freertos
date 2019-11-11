/*
 * Amazon FreeRTOS OTA V1.0.4
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
 * @file aws_iot_ota_interface.c
 * @brief .
 */

/* Standard library includes. */
#include <string.h>

/* OTA inteface includes. */
#include "aws_iot_ota_interface.h"

/* OTA transport inteface includes. */

#if ( configENABLED_DATA_PROTOCOLS & OTA_DATA_OVER_MQTT )
    #include "mqtt/aws_iot_ota_mqtt.h"
#endif

#if ( configENABLED_DATA_PROTOCOLS & OTA_DATA_OVER_HTTP )
    #include "http/aws_iot_ota_http.h"
#endif


void prvSetControlInterface( OTA_ControlInterface_t * pxControlInterface )
{

#if ( configENABLED_CONTROL_PROTOCOL == OTA_CONTROL_OVER_MQTT )
	pxControlInterface->prvRequestJob = prvRequestJob_Mqtt;
	pxControlInterface->prvUpdateJobStatus = prvUpdateJobStatus_Mqtt;
#else
    #error "Enable MQTT control as control operations are only supported over MQTT."
#endif

}

void prvSetDataInterface( OTA_DataInterface_t * pxDataInterface, const uint8_t *  pucProtocol )
{

#if ( configENABLED_DATA_PROTOCOLS & OTA_DATA_OVER_MQTT )
   if(NULL != strstr((const char*)pucProtocol, OTA_PRIMARY_DATA_PROTOCOL ))
   {
	   pxDataInterface->prvInitFileTransfer = prvInitFileTransfer_Mqtt;
	   pxDataInterface->prvRequestFileBlock = prvRequestFileBlock_Mqtt;
	   pxDataInterface->prvDecodeFileBlock = prvDecodeFileBlock_Mqtt;
	   pxDataInterface->prvCleanup = prvCleanup_Mqtt;
   }
#elif ( configENABLED_DATA_PROTOCOLS & OTA_DATA_OVER_HTTP )
   else if(NULL != strstr((const char*)pucProtocol, OTA_SECONDARY_DATA_PROTOCOL))
   {
       pxDataInterface->prvInitFileTransfer = prvInitFileTransfer_Http;
       pxDataInterface->prvRequestFileBlock = prvRequestFileBlock_Http;
       pxDataInterface->prvDecodeFileBlock = prvDecodeFileBlock_Http;
       pxDataInterface->prvCleanup = prvCleanup_Http;
   }
#else
    #error "Enable atleast one protocol for data operations."
#endif

}
