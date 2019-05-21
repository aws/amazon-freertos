#include <stdint.h>

/* FreeRTOS includes. */
#include "FreeRTOS.h"
#include "task.h"

/* FreeRTOS+TCP includes. */
#include "FreeRTOS_IP.h"
#include "FreeRTOS_IP_Private.h"

void vNondetSetCurrentTCB( void );
void vSetGlobalVariables( void );
void vPrepareTaskLists( void );
TaskHandle_t *pxNondetSetTaskHandle( void );
char *pcNondetSetString( size_t xSizeLength );

void harness()
{
	TaskFunction_t pxTaskCode;
	char * pcName;
	configSTACK_DEPTH_TYPE usStackDepth = STACK_DEPTH;
	void * pvParameters;
	UBaseType_t uxPriority;
	TaskHandle_t * pxCreatedTask;

	vNondetSetCurrentTCB();
	vSetGlobalVariables();
	vPrepareTaskLists();

	pxCreatedTask = pxNondetSetTaskHandle();
	pcName = pcNondetSetString( configMAX_TASK_NAME_LEN );

	xTaskCreate(pxTaskCode,
				pcName,
				usStackDepth,
				pvParameters,
				uxPriority,
				pxCreatedTask );
}
