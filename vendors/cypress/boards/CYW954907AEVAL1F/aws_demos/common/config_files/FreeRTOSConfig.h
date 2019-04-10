/*
FreeRTOS Kernel V10.0.1
Copyright (C) 2019 Amazon.com, Inc. or its affiliates.  All Rights Reserved.

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

#include "platform_config.h"
#include "wwd_FreeRTOS_systick.h"
#include "wiced_defaults.h"

#ifdef ENABLE_TASK_TRACE
#include "FreeRTOS_trace.h"
#endif /* ifdef ENABLE_TASK_TRACE */

#if defined ( __IAR_SYSTEMS_ICC__ )
/* This file is included from the IAR portasm.s, so must avoid C
declarations in that case */
#include "platform_sleep.h"
#endif /* if defined ( __IAR_SYSTEMS_ICC__ ) */

#ifdef __cplusplus
extern "C" {
#endif
/*-----------------------------------------------------------
* Application specific definitions.
*
* These definitions should be adjusted for your particular hardware and
* application requirements.
*
* THESE PARAMETERS ARE DESCRIBED WITHIN THE 'CONFIGURATION' SECTION OF THE
* FreeRTOS API DOCUMENTATION AVAILABLE ON THE FreeRTOS.org WEB SITE.
* http://www.freertos.org/a00110.html
*
* The bottom of this file contains some constants specific to running the UDP
* stack in this demo.  Constants specific to FreeRTOS+TCP itself (rather than
* the demo) are contained in FreeRTOSIPConfig.h.
*----------------------------------------------------------*/

/* FIX ME: Uncomment and set to the specifications for your MCU. */
#define configCPU_CLOCK_HZ                         ( ( unsigned long ) CPU_CLOCK_HZ )

#define configUSE_DAEMON_TASK_STARTUP_HOOK         1
#define configENABLE_BACKWARD_COMPATIBILITY        0
#define configUSE_PREEMPTION                       1
#define configUSE_PORT_OPTIMISED_TASK_SELECTION    1
#define configMAX_PRIORITIES                       ( 10 )
#define configTICK_RATE_HZ                         ( ( TickType_t ) SYSTICK_FREQUENCY )
#define configMINIMAL_STACK_SIZE                   ( ( uint16_t ) 256 )
#define configTOTAL_HEAP_SIZE                      ( ( size_t ) ( 2048U * 1024U ) )
#define configMAX_TASK_NAME_LEN                    ( 16 )
#ifndef configUSE_TRACE_FACILITY
#define configUSE_TRACE_FACILITY                   ( 0 )
#endif /* configUSE_TRACE_FACILITY */
#define configUSE_16_BIT_TICKS                     0
#define configIDLE_SHOULD_YIELD                    1
#define configUSE_CO_ROUTINES                      0
#ifndef configUSE_MUTEXES
#define configUSE_MUTEXES                          ( 1 )
#endif /* ifndef configUSE_MUTEXES */
#define configUSE_RECURSIVE_MUTEXES                1
#define configQUEUE_REGISTRY_SIZE                  0
#define configUSE_APPLICATION_TASK_TAG             0
#define configUSE_COUNTING_SEMAPHORES              1
#define configUSE_ALTERNATIVE_API                  0
#define configNUM_THREAD_LOCAL_STORAGE_POINTERS    3      /* FreeRTOS+FAT requires 2 pointers if a CWD is supported. */
#define configRECORD_STACK_HIGH_ADDRESS            1
#define configUSE_POSIX_ERRNO                      0
/* Hook function related definitions. */
#define configUSE_TICK_HOOK                        0
#define configUSE_IDLE_HOOK                        0
#define configUSE_MALLOC_FAILED_HOOK               1
#define configCHECK_FOR_STACK_OVERFLOW             2      /* Not applicable to the Win32 port. */

/* Software timer related definitions. */
#define configUSE_TIMERS                           1
#define configTIMER_TASK_PRIORITY                  ( configMAX_PRIORITIES - 1 )
#define configTIMER_QUEUE_LENGTH                   5
#define configTIMER_TASK_STACK_DEPTH               ( ( unsigned short ) (6144 / sizeof( portSTACK_TYPE )) )

/* Event group related definitions. */
#define configUSE_EVENT_GROUPS                     1
#define configNO_MALLOC                             ( 0 )
#define configUSE_NEWLIB_MALLOC_LOCK                ( 0 )



/* Co-routine definitions. */
#define configUSE_CO_ROUTINES                   0
#define configMAX_CO_ROUTINE_PRIORITIES         ( 2 )

/* add user defined functionality and user defined initialization
 * to be added to FreeRTOS's tasks.c source code*/
#define configINCLUDE_FREERTOS_TASK_C_ADDITIONS_H   ( 1 )

/* Currently the TCP/IP stack is using dynamic allocation, and the MQTT task is
 * using static allocation. */
#define configSUPPORT_DYNAMIC_ALLOCATION        1
#define configSUPPORT_STATIC_ALLOCATION         1

/* Set the following definitions to 1 to include the API function, or zero
 to exclude the API function. */

#define INCLUDE_vTaskPrioritySet                1
#define INCLUDE_uxTaskPriorityGet               1
#define INCLUDE_vTaskDelete                     1
#define INCLUDE_vTaskCleanUpResources           0
#define INCLUDE_vTaskSuspend                    1
#define INCLUDE_vTaskDelayUntil                 1
#define INCLUDE_vTaskDelay                      1

#define INCLUDE_xTaskGetCurrentThread               ( 1 )
#define INCLUDE_eTaskGetState                       ( 1 )
#define INCLUDE_xTaskAbortDelay                 1
#define INCLUDE_vTaskGetStackInfo                   ( 1 )
#define INCLUDE_xTaskIsTaskFinished                 ( 1 )

/* Assert call defined for debug builds. */
extern void vAssertCalled( const char * pcFile,
                           uint32_t ulLine );
#define configASSERT( x )    if( ( x ) == 0 ) vAssertCalled( __FILE__, __LINE__ )

/* The function that implements FreeRTOS printf style output, and the macro
 * that maps the configPRINTF() macros to that function. */
extern void vLoggingPrintf( const char * pcFormat, ... );
//#define configPRINTF( X )    vLoggingPrintf X
#define configPRINTF( X )    printf X

/* Non-format version thread-safe print */
extern void vLoggingPrint( const char * pcMessage );
#define configPRINT( X )     vLoggingPrint( X )

/* Map the logging task's printf to the board specific output function. */
#include <stdio.h>
#define configPRINT_STRING( X )    printf( X ); /* FIX ME: Change to your devices console print acceptance function. */
/* Sets the length of the buffers into which logging messages are written - so
 * also defines the maximum length of each log message. */
#define configLOGGING_MAX_MESSAGE_LENGTH            100

/* Set to 1 to prepend each log message with a message number, the task name,
 * and a time stamp. */
#define configLOGGING_INCLUDE_TIME_AND_TASK_NAME    1

/* The priority at which the tick interrupt runs.  This should probably be kept at 1. */
#define configKERNEL_INTERRUPT_PRIORITY             ( 0xF0 )

/* The maximum interrupt priority from which FreeRTOS API functions can be called.
 * Only API functions that end in ...FromISR() can be used within interrupts. */
#define configMAX_SYSCALL_INTERRUPT_PRIORITY        ( 0x20 )

/* This is the value being used as per the ST library which permits 16
 priority values, 0 to 15.  This must correspond to the
 configKERNEL_INTERRUPT_PRIORITY setting.  Here 15 corresponds to the lowest
 NVIC value of 255. */
#define configLIBRARY_KERNEL_INTERRUPT_PRIORITY     ( 15 )


#if defined( DEBUG ) && ( ! defined( UNIT_TESTER ) )
#define configASSERT( expr )   wiced_assert( "FreeRTOS assert", expr )
#endif /* ifdef DEBUG */

#ifdef WICED_DISABLE_MCU_POWERSAVE

#define configUSE_IDLE_SLEEP_HOOK ( 1 )

#else /* ifdef WICED_DISABLE_MCU_POWERSAVE */

#define configUSE_TICKLESS_IDLE ( 1 )

#endif /* ifdef WICED_DISABLE_MCU_POWERSAVE */

/* platform-specific tick functions */
#define configSETUP_TICK_INTERRUPT                  platform_tick_start
/* FreeRTOS generally needs this because it does not expect the platform-specific
 irq handler to clear the interrupt, but ours, being RTOS-independent, does.
 So we have to explicitly #define this to nothing.  */
#define configCLEAR_TICK_INTERRUPT()
/* fiqirq_status */
#define configEOI_ADDRESS                           PLATFORM_APPSCR4_REGBASE(0x10)
#ifdef NETWORK_NOTIFY_RELEASED_PACKETS

#define MEMP_FREE_NOTIFY
extern void memp_free_notify( unsigned int type );

#endif /* ifdef NETWORK_NOTIFY_RELEASED_PACKETS */

#define configPLATFORM_NAME    "CYW954907AEVAL1F"
#ifdef __cplusplus
} /*extern "C" */
#endif

#endif /* FREERTOS_CONFIG_H */

