#include "commandTask.h"

#include <stdio.h>

#include "FreeRTOS.h"
#include "FreeRTOSIPConfig.h"
#include "runtimeTimer.h"
#include "task.h"

#define MAX_COMMAND_LENGTH 3 // only supported command is "rt"
static uint8_t ucParameterToPass = 0; // Required as part of task creation.
TaskHandle_t commandTaskHandle = NULL;

void prvCommandTaskCode(void* pvParameters) {
    char input[MAX_COMMAND_LENGTH];

    for (;;) {
        scanf("%s", input);
        if (strcmp(input, "rt") == 0) printRuntime();
    }
}

void initializeCommandTask(void)
{   
    xTaskCreate(
        prvCommandTaskCode,
        "USER_COMMANDS",
        configMINIMAL_STACK_SIZE,
        &ucParameterToPass, //ucParameterToPass must exist for the lifetime of the task, even if unused
        tskIDLE_PRIORITY,
        &commandTaskHandle);
    configASSERT(commandTaskHandle);
}