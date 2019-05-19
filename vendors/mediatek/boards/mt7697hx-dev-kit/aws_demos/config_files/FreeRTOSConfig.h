/*
FreeRTOS Kernel V10.0.1
Copyright (C) 2018 Amazon.com, Inc. or its affiliates.  All Rights Reserved.

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

/*-----------------------------------------------------------
 * Application specific definitions.
 *
 * These definitions should be adjusted for your particular hardware and
 * application requirements.
 *
 * THESE PARAMETERS ARE DESCRIBED WITHIN THE 'CONFIGURATION' SECTION OF THE
 * FreeRTOS API DOCUMENTATION AVAILABLE ON THE FreeRTOS.org WEB SITE.
 *
 * http://www.freertos.org/a00110.html
 *----------------------------------------------------------*/

/* Invoke vApplicationDaemonTaskStartupHook when timer task starts */
#define configUSE_DAEMON_TASK_STARTUP_HOOK         0

/*
 *  Ensure stdint is only used by the compiler, and not the assembler.
 *
 * These definitions are IAR, Keil and GCC compiler option, the sections in
 * this scope will not be compiled by Assembler. This module can not be
 * disabled.
 */
#if defined(__ICCARM__) || defined(__CC_ARM) || defined(__GNUC__)
#include <stdint.h>
#include <stdio.h>
#include "os_port_callback.h"
#include "os_trace_callback.h"

extern uint32_t SystemCoreClock;
#endif


#define configENABLE_BACKWARD_COMPATIBILITY        1
#define configUSE_PREEMPTION                       1

/* Hook function related definitions. */
#define configUSE_TICK_HOOK                        0
#define configUSE_IDLE_HOOK                        0
#define configUSE_MALLOC_FAILED_HOOK               0
#define configCHECK_FOR_STACK_OVERFLOW             2      /* Not applicable to the Win32 port. */

#define configUSE_PORT_OPTIMISED_TASK_SELECTION    0

#define configMAX_PRIORITIES                       ( 20 )
#define configTICK_RATE_HZ                         ( ( TickType_t ) 1000 )
#define configMINIMAL_STACK_SIZE                   ( ( unsigned short ) 256 )
#if MTK_WIFI_OS_MM_OPTION == 2
#define configTOTAL_HEAP_SIZE                      ( ( size_t ) ( 135U * 1024U ) )
#else
#define configTOTAL_HEAP_SIZE                      ( ( size_t ) ( ( 135U + 35U ) * 1024U ) )
#endif

#define configMAX_TASK_NAME_LEN                    ( 15 )
#define configUSE_TRACE_FACILITY                   1
#define configUSE_16_BIT_TICKS                     0
#define configIDLE_SHOULD_YIELD                    1
#define configUSE_MUTEXES                          1
#define configUSE_RECURSIVE_MUTEXES                1
#define configQUEUE_REGISTRY_SIZE                  0
#define configUSE_APPLICATION_TASK_TAG             0
#define configUSE_COUNTING_SEMAPHORES              1
#define configUSE_ALTERNATIVE_API                  0
#define configNUM_THREAD_LOCAL_STORAGE_POINTERS    3      /* FreeRTOS+FAT requires 2 pointers if a CWD is supported. */
#define configRECORD_STACK_HIGH_ADDRESS            1

/* Software timer related definitions. */
#define configUSE_TIMERS                           1
#define configTIMER_TASK_PRIORITY                  ( configMAX_PRIORITIES - 1 )
#define configTIMER_QUEUE_LENGTH                   10
#define configTIMER_TASK_STACK_DEPTH               ( configMINIMAL_STACK_SIZE * 2 )

/* Event group related definitions. */
#define configUSE_EVENT_GROUPS                     1

/* Run time stats gathering definitions. */
/* FIX ME: Uncomment if you plan to use Tracealyzer.
unsigned long ulGetRunTimeCounterValue( void );
void vConfigureTimerForRunTimeStats( void );
#define configGENERATE_RUN_TIME_STATS    1
#define portCONFIGURE_TIMER_FOR_RUN_TIME_STATS()    vConfigureTimerForRunTimeStats()
#define portGET_RUN_TIME_COUNTER_VALUE()            ulGetRunTimeCounterValue()
*/

/* Co-routine definitions. */
#define configUSE_CO_ROUTINES                   0
#define configMAX_CO_ROUTINE_PRIORITIES         ( 2 )

/* Currently the TCP/IP stack is using dynamic allocation, and the MQTT task is
 * using static allocation. */
#define configSUPPORT_DYNAMIC_ALLOCATION        1
#define configSUPPORT_STATIC_ALLOCATION         1

/* Set the following definitions to 1 to include the API function, or zero
 * to exclude the API function. */
#define INCLUDE_vTaskPrioritySet                1
#define INCLUDE_uxTaskPriorityGet               1
#define INCLUDE_vTaskDelete                     1
#define INCLUDE_vTaskCleanUpResources           1
#define INCLUDE_vTaskSuspend                    1
#define INCLUDE_vTaskDelayUntil                 1
#define INCLUDE_vTaskDelay                      1
#define INCLUDE_uxTaskGetStackHighWaterMark     1
#define INCLUDE_xTaskGetSchedulerState          1
#define INCLUDE_xTimerGetTimerTaskHandle        0
#define INCLUDE_xTaskGetIdleTaskHandle          0
#define INCLUDE_xQueueGetMutexHolder            1
#define INCLUDE_eTaskGetState                   1
#define INCLUDE_xEventGroupSetBitsFromISR       1
#define INCLUDE_xTimerPendFunctionCall          1
#define INCLUDE_xTaskGetCurrentTaskHandle       1
#define INCLUDE_xTaskAbortDelay                 1

/* Cortex-M specific definitions. */
#ifdef __NVIC_PRIO_BITS
    /* __BVIC_PRIO_BITS will be specified when CMSIS is being used. */
    #define configPRIO_BITS    __NVIC_PRIO_BITS
#else
    #define configPRIO_BITS    3                                 /* 8 priority levels. */
#endif

/* This demo makes use of one or more example stats formatting functions.  These
 * format the raw data provided by the uxTaskGetSystemState() function in to human
 * readable ASCII form.  See the notes in the implementation of vTaskList() within
 * FreeRTOS/Source/tasks.c for limitations.  configUSE_STATS_FORMATTING_FUNCTIONS
 * is set to 2 so the formatting functions are included without the stdio.h being
 * included in tasks.c.  That is because this project defines its own sprintf()
 * functions. */
#define configUSE_STATS_FORMATTING_FUNCTIONS    1

/* Assert call defined for debug builds. */
void vAssertCalled( const char * pcFile,
                    uint32_t ulLine );

/* The lowest interrupt priority that can be used in a call to a "set priority"
 * function. */
#define configLIBRARY_LOWEST_INTERRUPT_PRIORITY         0xf

/* The highest interrupt priority that can be used by any interrupt service
 * routine that makes calls to interrupt safe FreeRTOS API functions.  DO NOT CALL
 * INTERRUPT SAFE FREERTOS API FUNCTIONS FROM ANY INTERRUPT THAT HAS A HIGHER
 * PRIORITY THAN THIS! (higher priorities are lower numeric values. */
#define configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY    0x4

/* Interrupt priorities used by the kernel port layer itself.  These are generic
to all Cortex-M ports, and do not rely on any particular library functions. */
#define configKERNEL_INTERRUPT_PRIORITY         (( configLIBRARY_LOWEST_INTERRUPT_PRIORITY << (8 - configPRIO_BITS)) & 0xFF )

/* !!!! configMAX_SYSCALL_INTERRUPT_PRIORITY must not be set to zero !!!!
See http://www.FreeRTOS.org/RTOS-Cortex-M3-M4.html. */
#define configMAX_SYSCALL_INTERRUPT_PRIORITY    ( configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY << (8 - configPRIO_BITS) )

/* Normal assert() semantics without relying on the provision of an assert.h
header file. */
//#define configASSERT( x ) if( ( x ) == 0 ) { taskDISABLE_INTERRUPTS(); printf("\n%s : %s #%d\n", __FILE__, __FUNCTION__, __LINE__);for( ;; ); }


/* Definitions that map the FreeRTOS port interrupt handlers to their CMSIS
standard names. */
#define vPortSVCHandler SVC_Handler
#define xPortPendSVHandler PendSV_Handler
#define xPortSysTickHandler SysTick_Handler

/* configUSE_TICKLESS_IDLE = 1: enable FreeRTOS's built in tickless idle functionality.
   configUSE_TICKLESS_IDLE = 2: enable vendor's tickless idle functionality.
*/
#if configUSE_TICKLESS_IDLE == 1
#define portSUPPRESS_TICKS_AND_SLEEP( xExpectedIdleTime ) vPortSuppressTicksAndSleep( xExpectedIdleTime )
#elif configUSE_TICKLESS_IDLE == 2
/*These definitions are IAR, Keil and GCC compiler option, the sections in this scope will not be compiled by Assembler. This module can not be disabled.*/
#if defined(__ICCARM__) || defined(__CC_ARM) || defined(__GNUC__)
extern void tickless_handler( uint32_t xExpectedIdleTime );
#endif /*#if defined(__ICCARM__) || defined(__CC_ARM) || defined(__GNUC__)*/
#define portSUPPRESS_TICKS_AND_SLEEP( xExpectedIdleTime ) tickless_handler( xExpectedIdleTime )
#endif

/*These definitions are IAR, Keil and GCC compiler option, the sections in this scope will not be compiled by Assembler. This module can not be disabled.*/
#if defined(__ICCARM__) || defined(__CC_ARM) || defined(__GNUC__)

#include "assert.h"

extern void abort(void);

#include "exception_handler.h"

#define configASSERT( x ) if( (x) == 0 ) { platform_assert(#x, __FILE__, __LINE__); }



#define configPRINT_STRING printf

#define configLOGGING_MAX_MESSAGE_LENGTH	(120)

#define configLOGGING_INCLUDE_TIME_AND_TASK_NAME (1)

/* The address of an echo server that will be used by the two demo echo client
 * tasks:
 * http://www.freertos.org/FreeRTOS-Plus/FreeRTOS_Plus_TCP/TCP_Echo_Clients.html
 * http://www.freertos.org/FreeRTOS-Plus/FreeRTOS_Plus_TCP/UDP_Echo_Clients.html
 */
#define configECHO_SERVER_ADDR0              192
#define configECHO_SERVER_ADDR1              168
#define configECHO_SERVER_ADDR2              0
#define configECHO_SERVER_ADDR3              105
#define configTCP_ECHO_CLIENT_PORT           45000

/* Default MAC address configuration.  The demo creates a virtual network
 * connection that uses this MAC address by accessing the raw Ethernet/WiFi data
 * to and from a real network connection on the host PC.  See the
 * configNETWORK_INTERFACE_TO_USE definition above for information on how to
 * configure the real network connection to use. */
#define configMAC_ADDR0                      0x00
#define configMAC_ADDR1                      0x11
#define configMAC_ADDR2                      0x22
#define configMAC_ADDR3                      0x33
#define configMAC_ADDR4                      0x44
#define configMAC_ADDR5                      0x21

/* Default IP address configuration.  Used in ipconfigUSE_DHCP is set to 0, or
 * ipconfigUSE_DHCP is set to 1 but a DNS server cannot be contacted. */
#define configIP_ADDR0                       192
#define configIP_ADDR1                       168
#define configIP_ADDR2                       0
#define configIP_ADDR3                       105

/* Default gateway IP address configuration.  Used in ipconfigUSE_DHCP is set to
 * 0, or ipconfigUSE_DHCP is set to 1 but a DNS server cannot be contacted. */
#define configGATEWAY_ADDR0                  192
#define configGATEWAY_ADDR1                  168
#define configGATEWAY_ADDR2                  0
#define configGATEWAY_ADDR3                  1

/* Default DNS server configuration.  OpenDNS addresses are 208.67.222.222 and
 * 208.67.220.220.  Used in ipconfigUSE_DHCP is set to 0, or ipconfigUSE_DHCP is
 * set to 1 but a DNS server cannot be contacted.*/
#define configDNS_SERVER_ADDR0               208
#define configDNS_SERVER_ADDR1               67
#define configDNS_SERVER_ADDR2               222
#define configDNS_SERVER_ADDR3               222

/* Default netmask configuration.  Used in ipconfigUSE_DHCP is set to 0, or
 * ipconfigUSE_DHCP is set to 1 but a DNS server cannot be contacted. */
#define configNET_MASK0                      255
#define configNET_MASK1                      255
#define configNET_MASK2                      255
#define configNET_MASK3                      0

/* The UDP port to which print messages are sent. */
#define configPRINT_PORT                     ( 15000 )

#define configPROFILING                      ( 0 )

/* The platform FreeRTOS is running on. */
#define configPLATFORM_NAME    "MT7697HxDevKit"

/* Pseudo random number generater used by some demo tasks. */
extern uint32_t ulRand( void );
#define configRAND32()    ulRand()


/* byte order setting */
#define __little_endian__                   1

/* use tickless */
#define configUSE_TICKLESS_IDLE             0

/* MTK customized */
#define configTICKLESS_DEEP_SLEEP           0

/* cpu frequency */
extern uint32_t                             SystemCoreClock;
#define configCPU_CLOCK_HZ                  ( SystemCoreClock )



#if 0
#define configPRINTF( X )                           do { } while ( 0 )
#else
void vLoggingPrintf( const char *pcFormat, ... );
#define configPRINTF( X )                           vLoggingPrintf X
#endif

#endif /*#if defined(__ICCARM__) || defined(__CC_ARM) || defined(__GNUC__)*/

/* override service hander names */
#define vPortSVCHandler                     SVC_Handler
#define xPortPendSVHandler                  PendSV_Handler
#define xPortSysTickHandler                 SysTick_Handler

#define configUSE_POSIX_ERRNO               ( 1 )

/* chip include (also indirectly includes core_cm4.h) */
#include "mt7687.h"

#endif /* FREERTOS_CONFIG_H */
