/*
 * Basic FreeRTOS configuration properties
 * Author: Sergey Shcherbakov <shchers@gmail.com>
 *
 * See http://www.freertos.org/a00110.html.
 */


#ifndef FREERTOS_CONFIG_H
#define FREERTOS_CONFIG_H

#include <avr/io.h>

/* Ensure stdint is only used by the compiler, and not the assembler. */
#if defined( __ICCARM__ ) || defined( __ARMCC_VERSION ) || defined( __GNUC__ )
    #include <stdint.h>
    extern uint32_t SystemCoreClock;
       extern int DbgConsole_Printf( const char *fmt_s, ... );
#endif


#define configSUPPORT_STATIC_ALLOCATION              0

#define configUSE_PREEMPTION                         1
#define configUSE_IDLE_HOOK                          0
#define configUSE_TICK_HOOK                          0
#define configUSE_TICKLESS_IDLE                      0
#define configUSE_DAEMON_TASK_STARTUP_HOOK           0
#define configCPU_CLOCK_HZ                           ( ( unsigned long ) 16000000 )
#define configTICK_RATE_HZ                           ( ( TickType_t ) 1000 )
#define configMAX_PRIORITIES                         ( 4 )
#define configMINIMAL_STACK_SIZE                     ( ( unsigned short ) 128 )
#define configTOTAL_HEAP_SIZE                        ( (size_t ) ( 512 ) )
#define configMAX_TASK_NAME_LEN                      ( 8 )
#define configUSE_TRACE_FACILITY                     0
#define configUSE_16_BIT_TICKS                       1
#define configIDLE_SHOULD_YIELD                      1
#define configQUEUE_REGISTRY_SIZE                    0

/* Co-routine definitions. */
#define configUSE_CO_ROUTINES                        1
#define configMAX_CO_ROUTINE_PRIORITIES              ( 2 )

/* Set the following definitions to 1 to include the API function, or zero
 * to exclude the API function. */
#define INCLUDE_vTaskPrioritySet                     0
#define INCLUDE_uxTaskPriorityGet                    0
#define INCLUDE_vTaskDelete                          1
#define INCLUDE_vTaskCleanUpResources                0
#define INCLUDE_vTaskSuspend                         0
#define INCLUDE_vTaskDelayUntil                      1
#define INCLUDE_vTaskDelay                           1

#endif /* FREERTOS_CONFIG_H */
