#include <string.h>
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "aws_ble.h"
#include "aws_ble_config.h"
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
    TickType_t xAuthTimeout = pdMS_TO_TICKS( bleconfigNUMERIC_COMPARISON_TIMEOUT_SEC * 1000 );

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
                        BLE_ConfirmNumericComparisonKeys(&xPassKeyConfirm.xAdress, true);
                    }else
                    {
                    	configPRINTF(("Key Rejected\n"));
                        BLE_ConfirmNumericComparisonKeys(&xPassKeyConfirm.xAdress, false);

                    }

                    vPortFree(xINPUTmessage.pcData);
              }
    	}
    }
    vTaskDelete(NULL);
}

void NumericComparisonInit(void)
{


	#if( bleconfigENABLE_NUMERIC_COMPARISON == 1 )
    /* Create a queue that will pass in the code to the UART task and wait validation from the user. */
    xNumericComparisonQueue = xQueueCreate( 1, sizeof( BLEPassKeyConfirm_t ) );
    xTaskCreate(userInputTask, "InputTask", 2048, NULL, 0, NULL);
    #endif

}
