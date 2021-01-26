// Copyright 2018 Espressif Systems (Shanghai) PTE LTD
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include "FreeRTOS.h"
#include "esp_err.h"
#include "queue.h"
#include "task.h"
#include "string.h"

extern void pthread_local_storage_cleanup(TaskHandle_t task);

struct _reent *_impure_ptr;

void esp_tasks_c_additions_init()
{
}

int xTaskCreatePinnedToCore( void (*pxTaskCode)(void*),
                            const char * const pcName,
                            const uint32_t usStackDepth,
                            void * const pvParameters,
                            unsigned uxPriority,
                            void * const pxCreatedTask,
                            const int xCoreID )
{
	return xTaskCreate(pxTaskCode, pcName, usStackDepth, pvParameters, uxPriority, pxCreatedTask);
}

void vPortCleanUpTCB ( void *pxTCB )
{
	pthread_local_storage_cleanup(pxTCB);
}

void  __attribute__((weak)) vApplicationStackOverflowHook( TaskHandle_t xTask, char *pcTaskName )
{
	#define ERR_STR1 "***ERROR*** A stack overflow in task "
	#define ERR_STR2 " has been detected."
	const char *str[] = {ERR_STR1, pcTaskName, ERR_STR2};

	char buf[sizeof(ERR_STR1) + CONFIG_FREERTOS_MAX_TASK_NAME_LEN + sizeof(ERR_STR2) + 1 /* null char */] = { 0 };

	char *dest = buf;
	for (int i = 0 ; i < sizeof(str)/ sizeof(str[0]); i++) {
		dest = strcat(dest, str[i]);
	}
	esp_system_abort(buf);
}
