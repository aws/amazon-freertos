/*
 * FreeRTOS Kernel V10.2.0
 * Copyright (C) 2019 Amazon.com, Inc. or its affiliates.  All Rights Reserved.
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
 * http://www.FreeRTOS.org
 * http://aws.amazon.com/freertos
 *
 * 1 tab == 4 spaces!
 */


#ifndef FREERTOS_CONFIG_H
#define FREERTOS_CONFIG_H

/*-----------------------------------------------------------
 * Application specific definitions.
 *
 * These definitions should be adjusted for your particular hardware and
 * application requirements.
 *
 * THESE PARAMETERS ARE DESCRIBED WITHIN THE 'CONFIGURATION' SECTION OF THE
 * FreeRTOS API DOCUMENTATION AVAILABLE ON THE FreeRTOS.org WEB SITE.
 *
 * See http://www.freertos.org/a00110.html
 *----------------------------------------------------------*/

/* Ensure definitions are only used by the compiler, and not the assembler. */
#if defined(__STDC__) || defined(__cplusplus__)

#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include "cycfg_system.h"

extern uint32_t SystemCoreClock;

#if (CY_CFG_PWR_SYS_IDLE_MODE == CY_CFG_PWR_MODE_SLEEP) || (CY_CFG_PWR_SYS_IDLE_MODE == CY_CFG_PWR_MODE_DEEPSLEEP)
extern void vApplicationSleep(uint32_t xExpectedIdleTime);
#endif

/* The function that implements FreeRTOS printf style output, and the macro
 * that maps the configPRINTF() macros to that function. */
extern void vLoggingPrintf(const char * pcFormat, ...);
#define configPRINTF(X)    vLoggingPrintf X

/* Non-format version thread-safe print */
extern void vLoggingPrint(const char * pcMessage);
#define configPRINT(X)     vLoggingPrint(X)

/* Assert call defined for debug builds. */
extern void vAssertCalled(const char * pcFile,
                           uint32_t ulLine);
#define configASSERT(x)    if((x) == 0) vAssertCalled(__FILE__, __LINE__)

#endif /* __STDC__ || __cplusplus__ */

#define configENABLE_BACKWARD_COMPATIBILITY         1
#define configUSE_PORT_OPTIMISED_TASK_SELECTION     1
#define configUSE_PREEMPTION                        1
#define configMAX_PRIORITIES                        7
#define configCPU_CLOCK_HZ                          (SystemCoreClock)
#define configTICK_RATE_HZ                          ((TickType_t)1000)
#define configUSE_16_BIT_TICKS                      0
#define configMINIMAL_STACK_SIZE                    ((unsigned short)130)
#define configMAX_TASK_NAME_LEN                     10
#define configIDLE_SHOULD_YIELD                     1
#define configUSE_MUTEXES                           1
#define configUSE_RECURSIVE_MUTEXES                 1
#define configQUEUE_REGISTRY_SIZE                   8
#define configUSE_COUNTING_SEMAPHORES               1
#define configUSE_APPLICATION_TASK_TAG              1
#define configENABLE_FPU                            1
#define configNUM_THREAD_LOCAL_STORAGE_POINTERS     16
#define configUSE_NEWLIB_REENTRANT                  1
#define configUSE_TRACE_FACILITY                    1
#define configGENERATE_RUN_TIME_STATS               0

/* Hook function related definitions. */
#define configUSE_DAEMON_TASK_STARTUP_HOOK          1
#define configUSE_IDLE_HOOK                         1
#define configUSE_TICK_HOOK                         1
#define configUSE_MALLOC_FAILED_HOOK                1
#define configCHECK_FOR_STACK_OVERFLOW              2

/* Memory allocation configuration */
#define configSUPPORT_DYNAMIC_ALLOCATION            1
#define configSUPPORT_STATIC_ALLOCATION             1
/* The default heap allocation scheme is heap_3.c. Per FreeRTOS documenation https://www.freertos.org/a00111.html#heap_3
 * the following define has no effect. If the heap allocation scheme is changed uncomment the following line.
 */
//#define configTOTAL_HEAP_SIZE                       ((size_t)(208 * 1024))

/* This define is Cypress specific (not specified by FreeRTOS)and is internally
 * used by the Cypress clib_support library to choose whether to supspend the scheduler (heap_3 scheme) or
 * uses mutexes (for other schemes) to guard direct memory allocation (malloc and free) calls depending on
 * the heap allocation scheme. The default configured heap allocation scheme is heap_3.c. If a different
 * allocation scheme is selected this define should be removed for better performance.
 */
#define configHEAP_ALLOCATION_SCHEME                (HEAP_ALLOCATION_TYPE3)

/* Tickless idle configuration */
#if (CY_CFG_PWR_SYS_IDLE_MODE == CY_CFG_PWR_MODE_SLEEP) || (CY_CFG_PWR_SYS_IDLE_MODE == CY_CFG_PWR_MODE_DEEPSLEEP)
#define portSUPPRESS_TICKS_AND_SLEEP(xIdleTime)     vApplicationSleep(xIdleTime)
#define configUSE_TICKLESS_IDLE                     2
#endif

#if CY_CFG_PWR_DEEPSLEEP_LATENCY > 0
#define configEXPECTED_IDLE_TIME_BEFORE_SLEEP       (CY_CFG_PWR_DEEPSLEEP_LATENCY)
#endif

/* Software timer definitions. */
#define configUSE_TIMERS                            1
#define configTIMER_TASK_PRIORITY                   2
#define configTIMER_QUEUE_LENGTH                    10
#define configTIMER_TASK_STACK_DEPTH                (configMINIMAL_STACK_SIZE * 4)

/* Co-routine definitions. */
#define configUSE_CO_ROUTINES                       0
#define configMAX_CO_ROUTINE_PRIORITIES             2

/* Set the following definitions to 1 to include the API function, or zero
to exclude the API function. */
#define INCLUDE_vTaskPrioritySet                    1
#define INCLUDE_uxTaskPriorityGet                   1
#define INCLUDE_vTaskDelete                         1
#define INCLUDE_vTaskCleanUpResources               1
#define INCLUDE_vTaskSuspend                        1
#define INCLUDE_vTaskDelayUntil                     1
#define INCLUDE_vTaskDelay                          1
#define INCLUDE_xTaskGetSchedulerState              1
#define INCLUDE_xTaskIsTaskFinished                 1
#define INCLUDE_xTaskGetCurrentTaskHandle           1
#define INCLUDE_uxTaskGetStackHighWaterMark         0
#define INCLUDE_xTaskGetIdleTaskHandle              0
#define INCLUDE_eTaskGetState                       0
#define INCLUDE_xTimerPendFunctionCall              1
#define INCLUDE_xTaskAbortDelay                     1
#define INCLUDE_xTaskGetHandle                      0
#define INCLUDE_xTaskResumeFromISR                  1

/*
Interrupt nesting behavior configuration.
This is explained here: http://www.freertos.org/a00110.html

Priorities are controlled by two macros:
- configKERNEL_INTERRUPT_PRIORITY determines the priority of the RTOS daemon task
- configMAX_API_CALL_INTERRUPT_PRIORITY dictates the priority of ISRs that make API calls

Notes:
1. Interrupts that do not call API functions should be >= configKERNEL_INTERRUPT_PRIORITY
   and will nest.
2. Interrupts that call API functions must have priority between KERNEL_INTERRUPT_PRIORITY
   and MAX_API_CALL_INTERRUPT_PRIORITY (inclusive).
3. Interrupts running above MAX_API_CALL_INTERRUPT_PRIORITY are never delayed by the OS.
*/
/*
PSoC 6 __NVIC_PRIO_BITS = 3

0 (high)
1           MAX_API_CALL_INTERRUPT_PRIORITY 001xxxxx (0x3F)
2
3
4
5
6
7 (low)     KERNEL_INTERRUPT_PRIORITY       111xxxxx (0xFF)

!!!! configMAX_SYSCALL_INTERRUPT_PRIORITY must not be set to zero !!!!
See http://www.FreeRTOS.org/RTOS-Cortex-M3-M4.html

*/

/* Put KERNEL_INTERRUPT_PRIORITY in top __NVIC_PRIO_BITS bits of CM4 register */
#define configKERNEL_INTERRUPT_PRIORITY         0xFF
/*
Put MAX_SYSCALL_INTERRUPT_PRIORITY in top __NVIC_PRIO_BITS bits of CM4 register
NOTE For IAR compiler make sure that changes of this macro is reflected in
file portable\IAR\CM4F\portasm.s in PendSV_Handler: routine
*/
#define configMAX_SYSCALL_INTERRUPT_PRIORITY    0x3F
/* configMAX_API_CALL_INTERRUPT_PRIORITY is a new name for configMAX_SYSCALL_INTERRUPT_PRIORITY
 that is used by newer ports only. The two are equivalent. */
#define configMAX_API_CALL_INTERRUPT_PRIORITY   configMAX_SYSCALL_INTERRUPT_PRIORITY

/* Definitions that map the FreeRTOS port interrupt handlers to their CMSIS standard names. */
#define vPortSVCHandler       SVC_Handler
#define xPortPendSVHandler    PendSV_Handler
#define xPortSysTickHandler   SysTick_Handler

/* Sets the length of the buffers into which logging messages are written - so
 * also defines the maximum length of each log message. */
#define configLOGGING_MAX_MESSAGE_LENGTH            255

/* Set to 1 to prepend each log message with a message number, the task name,
 * and a time stamp. */
#define configLOGGING_INCLUDE_TIME_AND_TASK_NAME    1

/*
 * With retarget support enabled, printf gets redirected to the UART
 * that connects via USB to the developers machine.
 */
#define configPRINT_STRING(X) fputs((X), stdout)

/* The address of an echo server that will be used by the two demo echo client
 * tasks:
 * http://www.freertos.org/FreeRTOS-Plus/FreeRTOS_Plus_TCP/TCP_Echo_Clients.html,
 * http://www.freertos.org/FreeRTOS-Plus/FreeRTOS_Plus_TCP/UDP_Echo_Clients.html. */
#define configECHO_SERVER_ADDR0                     157
#define configECHO_SERVER_ADDR1                     95
#define configECHO_SERVER_ADDR2                     10
#define configECHO_SERVER_ADDR3                     63
#define configTCP_ECHO_CLIENT_PORT                  9001

/* Default MAC address configuration.  The demo creates a virtual network
 * connection that uses this MAC address by accessing the raw Ethernet/WiFi data
 * to and from a real network connection on the host PC.  See the
 * configNETWORK_INTERFACE_TO_USE definition above for information on how to
 * configure the real network connection to use. */
#define configMAC_ADDR0                             0xA0
#define configMAC_ADDR1                             0xC9
#define configMAC_ADDR2                             0xA0
#define configMAC_ADDR3                             0x46
#define configMAC_ADDR4                             0xEA
#define configMAC_ADDR5                             0x74

/* Default IP address configuration.  Used in ipconfigUSE_DHCP is set to 0, or
 * ipconfigUSE_DHCP is set to 1 but a DNS server cannot be contacted. */
#define configIP_ADDR0                              192
#define configIP_ADDR1                              168
#define configIP_ADDR2                              1
#define configIP_ADDR3                              4

/* Default gateway IP address configuration.  Used in ipconfigUSE_DHCP is set to
 * 0, or ipconfigUSE_DHCP is set to 1 but a DNS server cannot be contacted. */
#define configGATEWAY_ADDR0                         192
#define configGATEWAY_ADDR1                         168
#define configGATEWAY_ADDR2                         1
#define configGATEWAY_ADDR3                         1

/* Default DNS server configuration.  OpenDNS addresses are 208.67.222.222 and
 * 208.67.220.220.  Used in ipconfigUSE_DHCP is set to 0, or ipconfigUSE_DHCP is
 * set to 1 but a DNS server cannot be contacted.*/
#define configDNS_SERVER_ADDR0                      208
#define configDNS_SERVER_ADDR1                      67
#define configDNS_SERVER_ADDR2                      222
#define configDNS_SERVER_ADDR3                      222

/* Default netmask configuration.  Used in ipconfigUSE_DHCP is set to 0, or
 * ipconfigUSE_DHCP is set to 1 but a DNS server cannot be contacted. */
#define configNET_MASK0                             255
#define configNET_MASK1                             255
#define configNET_MASK2                             255
#define configNET_MASK3                             0

/* The UDP port to which print messages are sent. */
#define configPRINT_PORT                            15000

#endif /* FREERTOS_CONFIG_H */
