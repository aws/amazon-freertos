#include "runtimeTimer.h"

#include "FreeRTOS.h"
#include "FreeRTOSIPConfig.h"
#include "task.h"
#include "timers.h"

#define MS_PER_MINUTE (SECONDS_PER_MINUTE * MS_PER_SECOND)
#define SECONDS_PER_MINUTE (60)
#define MS_PER_SECOND (1000)

#define RUNTIME_TASK_INTERRUPT_NUMBER (3)

volatile unsigned long long runtime_ms = 0;
static StaticTimer_t xRuntimeTimerBuffer[configMINIMAL_STACK_SIZE];
static unsigned long runtimeTimerId = 1004; // Arbitrary value
// portTICK_PERIOD_MS is 1. Windows Simulator Tick is 1kHZ.
const TickType_t xDelay = 1 / portTICK_PERIOD_MS; 

uint32_t runtimeInterrupt(void)
{
    runtime_ms += xDelay;
    return 0;
}

void prvRuntimeTimerCallback(TimerHandle_t xTimer)
{
    vPortGenerateSimulatedInterrupt(RUNTIME_TASK_INTERRUPT_NUMBER);
}

void initializeRuntimeTimer(void) {
    BaseType_t xResult;
    runtime_ms = 0;

    TimerHandle_t runtimeTimer = xTimerCreateStatic(
        "Runtime", /*lint !e971 Unqualified char types are allowed for strings and single characters only. */
        xDelay,
        pdTRUE,
        (void *) &runtimeTimerId,
        prvRuntimeTimerCallback,
        xRuntimeTimerBuffer);
    vPortSetInterruptHandler(RUNTIME_TASK_INTERRUPT_NUMBER, runtimeInterrupt);

    // Initialize runtime timer before starting scheduler, to avoid using a block time
    xResult = xTimerStart(runtimeTimer, 0); 
    FreeRTOS_printf( (xResult == pdPASS ?
        "Runtime Timer Initialized" :
        "Runtime Timer Init Failed") );
}

void printRuntime(void) {
    // Copy the global variable to avoid race condition during processing.
    unsigned long long local_copy = runtime_ms;

    unsigned long rt_1 = portGET_RUN_TIME_COUNTER_VALUE();

    unsigned long minutes = (unsigned long) local_copy / MS_PER_MINUTE;
    unsigned int seconds = (unsigned int) (local_copy % MS_PER_MINUTE) / MS_PER_SECOND;
    unsigned int milliseconds = (unsigned int) local_copy % MS_PER_SECOND;
    FreeRTOS_printf( ("Timer-Based Runtime: %ld:%02ld:%03d\r\n", minutes, seconds, milliseconds) );

    unsigned long rt_2= portGET_RUN_TIME_COUNTER_VALUE();
    FreeRTOS_printf(("Latency: %ld.%02ldms\r\n", (rt_2 - rt_1) / 100, (rt_2 - rt_1) % 100));

    unsigned long  windows_ticks_ms = rt_1 / 100;
    minutes = (unsigned long)windows_ticks_ms / MS_PER_MINUTE;
    seconds = (unsigned int)(windows_ticks_ms % MS_PER_MINUTE) / MS_PER_SECOND;
    milliseconds = (unsigned int)windows_ticks_ms % MS_PER_SECOND;
    FreeRTOS_printf(("Windows-Tick Runtime: %ld:%02ld:%03d\r\n", minutes, seconds, milliseconds));
}