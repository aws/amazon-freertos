/*
FreeRTOS Kernel V1.1.3
Copyright (C) 2017 Amazon.com, Inc. or its affiliates.  All Rights Reserved.

Permission is hereby granted, free of charge, to any person obtaining a copy of
this software and associated documentation files (the "Software"), to deal in
the Software without restriction, including without limitation the rights to
use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
the Software, and to permit persons to whom the Software is furnished to do so,
subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

 http://aws.amazon.com/freertos
 http://www.FreeRTOS.org
*/

#ifndef FREERTOS_CONFIG_H
#define FREERTOS_CONFIG_H

/* Unity includes. */
#include "unity_internals.h"

#ifndef UART_PRINT
    #include "uart_term.h"
    #define UART_PRINT    Report
#endif

/*-----------------------------------------------------------
* Application specific definitions.
*
* These definitions should be adjusted for your particular hardware and
* application requirements.
*
* THESE PARAMETERS ARE DESCRIBED WITHIN THE 'CONFIGURATION' SECTION OF THE
* FreeRTOS API DOCUMENTATION AVAILABLE ON THE FreeRTOS.org WEB SITE.
*
* See http://www.freertos.org/a00110.html.
*----------------------------------------------------------*/

#define configCPU_CLOCK_HZ                       ( ( unsigned long ) 80000000 )

#define configUSE_DAEMON_TASK_STARTUP_HOOK       1
#define configUSE_PREEMPTION                     1
#define configUSE_TIME_SLICING                   0
#define configUSE_IDLE_HOOK                      1
#define configUSE_TICK_HOOK                      0
#define configTICK_RATE_HZ                       ( ( TickType_t ) 1000 )
#define configMINIMAL_STACK_SIZE                 ( ( unsigned short ) 256 )
#define configTOTAL_HEAP_SIZE                    ( ( size_t ) ( 0x10000 ) )
#define configMAX_TASK_NAME_LEN                  ( 12 )
#define configUSE_TRACE_FACILITY                 1
#define configUSE_16_BIT_TICKS                   0
#define configIDLE_SHOULD_YIELD                  1
#define configUSE_CO_ROUTINES                    0
#define configUSE_MUTEXES                        1
#define configUSE_RECURSIVE_MUTEXES              1
#define configUSE_QUEUE_SETS                     0
#define configUSE_COUNTING_SEMAPHORES            1
#define configUSE_POSIX_ERRNO                    1

#define configMAX_PRIORITIES                     ( 10UL )
#define configQUEUE_REGISTRY_SIZE                0

/* Constants provided for debugging. */
#define configUSE_MALLOC_FAILED_HOOK             1
#define configCHECK_FOR_STACK_OVERFLOW           2
#define configASSERT( x )                                       \
    if( ( x ) == 0 ) TEST_ABORT()

#define configUSE_TICKLESS_IDLE                  1

#define configEXPECTED_IDLE_TIME_BEFORE_SLEEP    5

/* Timer related defines. */
#define configUSE_TIMERS                         1
#define configTIMER_TASK_PRIORITY                5
#define configTIMER_QUEUE_LENGTH                 20
#define configTIMER_TASK_STACK_DEPTH             ( configMINIMAL_STACK_SIZE * 2 )

#define configENABLE_BACKWARD_COMPATIBILITY      0

/* Needed for POSIX/pthread. */
#define configUSE_APPLICATION_TASK_TAG           1

/* The function that implements FreeRTOS printf style output, and the macro
 * that maps the configPRINTF() macros to that function. */
extern void vLoggingPrintf( const char * pcFormat,
                            ... );
#define configPRINTF( X )    vLoggingPrintf X

/* Non-format version thread-safe print */
extern void vLoggingPrint( const char * pcMessage );
#define configPRINT( X )     vLoggingPrint( X )

/* Map the logging task's printf to the board specific output function. */
#define configPRINT_STRING( x )    Report( x );

/* Sets the length of the buffers into which logging messages are written - so
 * also defines the maximum length of each log message. */
#define configLOGGING_MAX_MESSAGE_LENGTH            256

/* Set to 1 to prepend each log message with a message number, the task name,
 * and a time stamp. */
#define configLOGGING_INCLUDE_TIME_AND_TASK_NAME    1

/* Cortex-M3/4 interrupt priority configuration follows...................... */

/* Use the system definition, if there is one. */
#ifdef __NVIC_PRIO_BITS
    #define configPRIO_BITS       __NVIC_PRIO_BITS
#else
    #define configPRIO_BITS       3     /* 8 priority levels */
#endif


/* The lowest interrupt priority that can be used in a call to a "set priority"
function. */
#define configLIBRARY_LOWEST_INTERRUPT_PRIORITY         0x07

/*
 * The highest interrupt priority that can be used by any interrupt service
 * routine that makes calls to interrupt safe FreeRTOS API functions.  DO NOT CALL
 * INTERRUPT SAFE FREERTOS API FUNCTIONS FROM ANY INTERRUPT THAT HAS A HIGHER
 * PRIORITY THAN THIS! (higher priorities are lower numeric values.
 */
#define configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY    1

/*
 *  Priority 7 (shifted 5 since only the top 3 bits are implemented).
 *  Priority 7 is the lowest priority.
 */
#define configKERNEL_INTERRUPT_PRIORITY (configLIBRARY_LOWEST_INTERRUPT_PRIORITY << (8 - configPRIO_BITS))

/*
 *  Priority 1 (shifted 5 since only the top 3 bits are implemented).
 *  Priority 1 is the second highest priority.
 *  Priority 0 is the highest priority.
 *  !!!! configMAX_SYSCALL_INTERRUPT_PRIORITY must not be set to zero !!!!
 *  See http://www.FreeRTOS.org/RTOS-Cortex-M3-M4.html.
 */
#define configMAX_SYSCALL_INTERRUPT_PRIORITY (configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY << (8 - configPRIO_BITS))

/*
 * Use the Cortex-M3 optimised task selection rather than the generic C code
 * version.
 */
#define configUSE_PORT_OPTIMISED_TASK_SELECTION     1

#define configSUPPORT_STATIC_ALLOCATION             1

/*
 * Set the following definitions to 1 to include the API function, or zero
 * to exclude the API function.
 */
#define INCLUDE_vTaskPrioritySet                    1
#define INCLUDE_uxTaskPriorityGet                   1
#define INCLUDE_vTaskDelete                         1
#define INCLUDE_vTaskSuspend                        1
#define INCLUDE_vTaskDelayUntil                     1
#define INCLUDE_vTaskDelay                          1
#define INCLUDE_uxTaskGetStackHighWaterMark         0
#define INCLUDE_xTaskGetSchedulerState              1
#define INCLUDE_xTaskGetIdleTaskHandle              0
#define INCLUDE_eTaskGetState                       1
#define INCLUDE_xSemaphoreGetMutexHolder            1
#define INCLUDE_xTaskGetCurrentTaskHandle           1

/* The address of an echo server that will be used by the two demo echo client
 * tasks.
 * http://www.freertos.org/FreeRTOS-Plus/FreeRTOS_Plus_TCP/TCP_Echo_Clients.html
 * http://www.freertos.org/FreeRTOS-Plus/FreeRTOS_Plus_TCP/UDP_Echo_Clients.html. */
#define configECHO_SERVER_ADDR0                     192
#define configECHO_SERVER_ADDR1                     168
#define configECHO_SERVER_ADDR2                     2
#define configECHO_SERVER_ADDR3                     6
#define configTCP_ECHO_CLIENT_PORT                  7

/* The platform FreeRTOS is running on. */
#define configPLATFORM_NAME    "TICC3220"

#endif /* FREERTOS_CONFIG_H */
