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
#ifdef OTA_DATA_OVER_MQTT
#include "mqtt/aws_iot_ota_mqtt.h"
#endif

#ifdef OTA_DATA_OVER_HTTP
#include "http/aws_iot_ota_http.h"
#endif


void prvSetControlInterface(OTA_Interface_t * pxInterface)
{

	pxInterface->xControlInterface.prvRequestJob = prvRequestJob_Mqtt;
	pxInterface->xControlInterface.prvUpdateJobStatus = prvUpdateJobStatus_Mqtt;

}

void prvSetDataInterface(OTA_Interface_t * pxInterface, uint8_t *  pucProtocol)
{

   if(NULL != strstr((const char*)pucProtocol, OTA_PRIMARY_DATA_PROTOCOL ))
   {

	   pxInterface->xDataInterface.prvInitFileTransfer = prvInitFileTransfer_Mqtt;
	   pxInterface->xDataInterface.prvRequestFileBlock = prvRequestFileBlock_Mqtt;
	   pxInterface->xDataInterface.prvDecodeFileBlock = prvDecodeFileBlock_Mqtt;
	   pxInterface->xDataInterface.prvCleanup = prvCleanup_Mqtt;
   }
#ifdef OTA_DATA_OVER_HTTP
   else if(NULL != strstr((const char*)pucProtocol, OTA_SECONDARY_DATA_PROTOCOL))
   {
	   pxInterface->xDataInterface.prvInitFileTransfer = prvInitFileTransfer_Http;
	   pxInterface->xDataInterface.prvRequestFileBlock = prvRequestFileBlock_Http;
	   pxInterface->xDataInterface.prvDecodeFileBlock = prvDecodeFileBlock_Http;
       pxInterface->xDataInterface.prvCleanup = prvCleanup_Http;
   }
#endif

}
