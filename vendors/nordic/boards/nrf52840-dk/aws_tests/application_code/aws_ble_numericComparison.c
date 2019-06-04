/*
 * Amazon FreeRTOS
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


#include <string.h>
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "iot_ble.h"
#include "iot_ble_config.h"
#include "aws_ble_numericComparison.h"

typedef struct{
	uint32_t ulPassKey;
	BTBdaddr_t xAdress;
} BLEPassKeyConfirm_t;

QueueHandle_t xNumericComparisonQueue = NULL;

void BLEGAPPairingStateChangedCb( BTStatus_t xStatus,
                                     BTBdaddr_t * pxRemoteBdAddr,
                                     BTSecurityLevel_t xSecurityLevel,
                                     BTAuthFailureReason_t xReason )
{
}

void BLENumericComparisonCb(BTBdaddr_t * pxRemoteBdAddr, uint32_t ulPassKey)
{
	BLEPassKeyConfirm_t xPassKeyConfirm;

	if(pxRemoteBdAddr != NULL)
	{
		xPassKeyConfirm.ulPassKey = ulPassKey;
		memcpy(&xPassKeyConfirm.xAdress, pxRemoteBdAddr, sizeof(BTBdaddr_t));

		xQueueSend(xNumericComparisonQueue, (void * )&xPassKeyConfirm, (portTickType)portMAX_DELAY);
	}
}

void userInputTask(void *pvParameters)
{
	INPUTMessage_t xINPUTmessage;
    BLEPassKeyConfirm_t xPassKeyConfirm;
    TickType_t xAuthTimeout = pdMS_TO_TICKS( IOT_BLE_NUMERIC_COMPARISON_TIMEOUT_SEC * 1000 );

    for (;;) {
    	if (xQueueReceive(xNumericComparisonQueue, (void * )&xPassKeyConfirm, portMAX_DELAY ))
    	{
              configPRINTF(("Numeric comparison:%ld\n", xPassKeyConfirm.ulPassKey ));
              configPRINTF(("Press 'y' to confirm\n"));
              /* Waiting for UART event. */
              if ( getUserMessage( &xINPUTmessage, xAuthTimeout ) == pdTRUE ) {
                    if((xINPUTmessage.pcData[0] == 'y')||(xINPUTmessage.pcData[0] == 'Y'))
                    {
                    	configPRINTF(("Key accepted\n"));
                        IotBle_ConfirmNumericComparisonKeys(&xPassKeyConfirm.xAdress, true);
                    }else
                    {
                    	configPRINTF(("Key Rejected\n"));
                        IotBle_ConfirmNumericComparisonKeys(&xPassKeyConfirm.xAdress, false);

                    }

                    vPortFree(xINPUTmessage.pcData);
              }
    	}
    }
    vTaskDelete(NULL);
}

void NumericComparisonInit(void)
{


	#if( IOT_BLE_ENABLE_NUMERIC_COMPARISON == 1 )
    /* Create a queue that will pass in the code to the UART task and wait validation from the user. */
    xNumericComparisonQueue = xQueueCreate( 1, sizeof( BLEPassKeyConfirm_t ) );
	#endif

    xTaskCreate(userInputTask, "InputTask", 2048, NULL, 0, NULL);

}
