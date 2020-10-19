/*
 * FreeRTOS Kernel V10.2.0
 * Copyright (C) 2018 Amazon.com, Inc. or its affiliates.  All Rights Reserved.
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
 * http://aws.amazon.com/freertos
 * http://www.FreeRTOS.org
 */

#ifndef FREERTOS_CONFIG_H
#define FREERTOS_CONFIG_H

/* The ESP32 development environment defines application configurations in this header.
 * The configurations are updated in the "Espressif IDF Configuration" menu that appears
 * when a "make menuconfig" is run from a 32 bit GNU compatible environment.
 */
#include "sdkconfig.h"

/* Required for configuration-dependent settings */
#include "xtensa_config.h"

#ifndef __ASSEMBLER__
    #include <stdlib.h> /* for abort() */
    #include "rom/ets_sys.h"
    #include <sys/reent.h>

    #if CONFIG_SYSVIEW_ENABLE
        #include "SEGGER_SYSVIEW_FreeRTOS.h"
        #undef INLINE // to avoid redefinition
    #endif
#endif /* def __ASSEMBLER__ */

/*-----------------------------------------------------------
 * Application specific definitions.
 *
 * These definitions should be adjusted for your particular hardware and
 * application requirements.
 *
 * Note that the default heap size is deliberately kept small so that
 * the build is more likely to succeed for configurations with limited
 * memory.
 *
 * THESE PARAMETERS ARE DESCRIBED WITHIN THE 'CONFIGURATION' SECTION OF THE
 * FreeRTOS API DOCUMENTATION AVAILABLE ON THE FreeRTOS.org WEB SITE.
 *----------------------------------------------------------*/

#define configUSE_PREEMPTION			1
#define configUSE_IDLE_HOOK				( CONFIG_FREERTOS_LEGACY_IDLE_HOOK )
#define configUSE_TICK_HOOK				( CONFIG_FREERTOS_LEGACY_TICK_HOOK )
#define configTICK_RATE_HZ				( CONFIG_FREERTOS_HZ )
#define configUSE_DAEMON_TASK_STARTUP_HOOK 1

/* Use the default clock rate for simulator. */
/* #define configCPU_CLOCK_HZ				80000000 */

/* This has impact on speed of search for highest priority. */
#ifdef SMALL_TEST
    #define configMAX_PRIORITIES			( 7 )
#else
    #define configMAX_PRIORITIES			( 25 )
#endif

#ifndef CONFIG_ESP32_APPTRACE_ENABLE
    #define configMINIMAL_STACK_SIZE		768
#else
    /* The apptrace module requires at least 2KB of stack per task. */
    #define configMINIMAL_STACK_SIZE		2048
#endif

#define configUSE_MUTEXES				1
#define configUSE_RECURSIVE_MUTEXES		1
#define configUSE_COUNTING_SEMAPHORES	1
#define configUSE_16_BIT_TICKS			0
#define configIDLE_SHOULD_YIELD			0
#define configQUEUE_REGISTRY_SIZE		CONFIG_FREERTOS_QUEUE_REGISTRY_SIZE
#define configMAX_TASK_NAME_LEN			( CONFIG_FREERTOS_MAX_TASK_NAME_LEN )
#define configUSE_POSIX_ERRNO			1

#if CONFIG_FREERTOS_CHECK_STACKOVERFLOW_NONE
    #define configCHECK_FOR_STACK_OVERFLOW	0
#elif CONFIG_FREERTOS_CHECK_STACKOVERFLOW_PTRVAL
    #define configCHECK_FOR_STACK_OVERFLOW	1
#elif CONFIG_FREERTOS_CHECK_STACKOVERFLOW_CANARY
    #define configCHECK_FOR_STACK_OVERFLOW	2
#endif

/* We define the heap to span all of the non-statically-allocated shared RAM.
 * TODO: Make sure there is some space left for the app and main cpu when running outside
 * of a thread. */
#define configAPPLICATION_ALLOCATED_HEAP 1
#define configTOTAL_HEAP_SIZE			(&_heap_end - &_heap_start)//( ( size_t ) (64 * 1024) )

#ifndef configIDLE_TASK_STACK_SIZE
    #define configIDLE_TASK_STACK_SIZE CONFIG_FREERTOS_IDLE_TASK_STACKSIZE
#endif

/* The Xtensa port uses a separate interrupt stack. Adjust the stack size */
/* to suit the needs of your specific application.                        */
#ifndef configISR_STACK_SIZE
    #define configISR_STACK_SIZE			CONFIG_FREERTOS_ISR_STACKSIZE
#endif

/* configASSERT behaviour */
#if defined(CONFIG_FREERTOS_ASSERT_DISABLE)
    #define configASSERT(a) /* assertions disabled */
#elif defined(CONFIG_FREERTOS_ASSERT_FAIL_PRINT_CONTINUE)
    #define configASSERT(a) if (!(a)) {                                     \
                ( void ) ets_printf("%s:%d (%s)- assert failed!\n",         \
                                    __FILE__, __LINE__, __FUNCTION__);      \
            }
#else /* CONFIG_FREERTOS_ASSERT_FAIL_ABORT */
    #define configASSERT(a) if (!(a)) {                                     \
                ( void ) ets_printf("%s:%d (%s)- assert failed!\n",         \
                                    __FILE__, __LINE__, __FUNCTION__);      \
                abort();                                                    \
            }
#endif


#ifndef __ASSEMBLER__
    /* The function that implements FreeRTOS printf style output, and the macro
     * that maps the configPRINTF() macros to that function. */
    extern void vLoggingPrintf( const char * pcFormat, ... );
    #define configPRINTF( X )    vLoggingPrintf X

    /* Non-format version thread-safe print. */
    extern void vLoggingPrint( const char * pcMessage );
    #define configPRINT( X )     vLoggingPrint( X )

    /* Map the logging task's printf to the board specific output function. */
    #define configPRINT_STRING( x )    printf( x )
#endif /* #ifndef __ASSEMBER__ */

/* Sets the length of the buffers into which logging messages are written - so
 * also defines the maximum length of each log message. */
#define configLOGGING_MAX_MESSAGE_LENGTH            192

/* Set to 1 to prepend each log message with a message number, the task name,
 * and a time stamp. */
#define configLOGGING_INCLUDE_TIME_AND_TASK_NAME    1

/* Co-routine definitions. */
#define configUSE_CO_ROUTINES 			0
#define configMAX_CO_ROUTINE_PRIORITIES ( 2 )

/* Test FreeRTOS timers (with timer task) and more. */
/* Some files don't compile if this flag is disabled */
#define configUSE_TIMERS                    1
#define configTIMER_TASK_PRIORITY           CONFIG_TIMER_TASK_PRIORITY
#define configTIMER_QUEUE_LENGTH            CONFIG_TIMER_QUEUE_LENGTH
#define configTIMER_TASK_STACK_DEPTH        CONFIG_TIMER_TASK_STACK_DEPTH

#define INCLUDE_xTimerPendFunctionCall      1
#define INCLUDE_eTaskGetState               1
#define configUSE_QUEUE_SETS                1

#define configUSE_TICKLESS_IDLE             CONFIG_FREERTOS_USE_TICKLESS_IDLE
#if configUSE_TICKLESS_IDLE
#define configEXPECTED_IDLE_TIME_BEFORE_SLEEP   CONFIG_FREERTOS_IDLE_TIME_BEFORE_SLEEP
#endif //configUSE_TICKLESS_IDLE
#define configENABLE_TASK_SNAPSHOT			1

#define configINCLUDE_FREERTOS_TASK_C_ADDITIONS_H 1

/* Set the following definitions to 1 to include the API function, or zero
   to exclude the API function. */
#define INCLUDE_vTaskPrioritySet			1
#define INCLUDE_uxTaskPriorityGet			1
#define INCLUDE_vTaskDelete					1
#define INCLUDE_vTaskCleanUpResources		0
#define INCLUDE_vTaskSuspend				1
#define INCLUDE_vTaskDelayUntil				1
#define INCLUDE_vTaskDelay					1
#define INCLUDE_uxTaskGetStackHighWaterMark	1
#define INCLUDE_pcTaskGetTaskName			1
#define INCLUDE_xTaskGetIdleTaskHandle      1
#define INCLUDE_pxTaskGetStackStart			1
#define INCLUDE_xSemaphoreGetMutexHolder    1

/* The priority at which the tick interrupt runs.  This should probably be
   kept at 1. */
#define configKERNEL_INTERRUPT_PRIORITY		1

/* The maximum interrupt priority from which FreeRTOS.org API functions can
   be called.  Only API functions that end in ...FromISR() can be used within
   interrupts. */
#define configMAX_SYSCALL_INTERRUPT_PRIORITY	XCHAL_EXCM_LEVEL

#define configUSE_NEWLIB_REENTRANT		1

#define configSUPPORT_DYNAMIC_ALLOCATION    1
#define configSUPPORT_STATIC_ALLOCATION     CONFIG_SUPPORT_STATIC_ALLOCATION

#ifndef __ASSEMBLER__
    extern void vPortCleanUpTCB ( void *pxTCB );
    #define portCLEAN_UP_TCB( pxTCB )           vPortCleanUpTCB( pxTCB )
#endif

#ifdef CONFIG_FREERTOS_USE_TRACE_FACILITY
    #define configUSE_TRACE_FACILITY        1       /* Used by uxTaskGetSystemState(), and other trace facility functions */
#endif

#ifdef CONFIG_FREERTOS_USE_STATS_FORMATTING_FUNCTIONS
    #define configUSE_STATS_FORMATTING_FUNCTIONS    1   /* Used by vTaskList() */
#endif

#ifdef CONFIG_FREERTOS_GENERATE_RUN_TIME_STATS
    #define configGENERATE_RUN_TIME_STATS   1       /* Used by vTaskGetRunTimeStats() */
#endif


/* This demo creates a virtual network connection by accessing the raw Ethernet
 * or WiFi data to and from a real network connection.  Many computers have more
 * than one real network port, and configNETWORK_INTERFACE_TO_USE is used to tell
 * the demo which real port should be used to create the virtual port.  The ports
 * available are displayed on the console when the application is executed.  For
 * example, on my development laptop setting configNETWORK_INTERFACE_TO_USE to 4
 * results in the wired network being used, while setting
 * configNETWORK_INTERFACE_TO_USE to 2 results in the wireless network being
 * used. */
#define configNETWORK_INTERFACE_TO_USE       2L

/* The address of an echo server that will be used by the two demo echo client
 * tasks:
 * http://www.freertos.org/FreeRTOS-Plus/FreeRTOS_Plus_TCP/TCP_Echo_Clients.html,
 * http://www.freertos.org/FreeRTOS-Plus/FreeRTOS_Plus_TCP/UDP_Echo_Clients.html. */
#define configECHO_SERVER_ADDR0              192
#define configECHO_SERVER_ADDR1              168
#define configECHO_SERVER_ADDR2              2
#define configECHO_SERVER_ADDR3              7
#define configTCP_ECHO_CLIENT_PORT           7

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
#define configPLATFORM_NAME    "EspressifESP32"

#ifndef __ASSEMBLER__
/*-----------------------------------------------------------
 * Segger Sysview FreeRTOS configurations.
 *-----------------------------------------------------------*/
    #ifndef traceISR_EXIT_TO_SCHEDULER
        #define traceISR_EXIT_TO_SCHEDULER()
    #endif

    #ifndef traceISR_EXIT
        #define traceISR_EXIT()
    #endif

    #ifndef traceISR_ENTER
        #define traceISR_ENTER(_n_)
    #endif

/*-----------------------------------------------------------
 * Extra ESP32 Specific Functions defined in extra.c
 *-----------------------------------------------------------*/

    int xTaskCreatePinnedToCore( void (*pxTaskCode)(void*),
                                const char * const pcName,
                                const uint32_t usStackDepth,
                                void * const pvParameters,
                                unsigned uxPriority,
                                void * const pxCreatedTask,
                                const int xCoreID );

    #define xTaskGetIdleTaskHandleForCPU(i) xTaskGetIdleTaskHandle()

    #define xTaskGetCurrentTaskHandleForCPU(i) xTaskGetCurrentTaskHandle()

    static inline int xTaskGetAffinity(void* arg)
    {
        return 0;
    }

    #define tskNO_AFFINITY INT32_MAX

    extern struct _reent *_impure_ptr;

    extern void esp_tasks_c_additions_init();
    #define FREERTOS_TASKS_C_ADDITIONS_INIT() esp_tasks_c_additions_init()

#endif /* #ifndef __ASSEMBLER__ */

/*-----------------------------------------------------------
 * ESP32/Xtensa Specific configurations.
 *-----------------------------------------------------------*/

/* ESP31 and ESP32 are dualcore processors. */
#ifndef CONFIG_FREERTOS_UNICORE
    #define portNUM_PROCESSORS 2
#else
    #define portNUM_PROCESSORS 1
#endif

#define configUSE_TRACE_FACILITY_2      0		/* Provided by Xtensa port patch */
#define configBENCHMARK					0		/* Provided by Xtensa port patch */

#define XT_USE_THREAD_SAFE_CLIB			0
#define configASSERT_2	0
#define configUSE_MUTEX 1
#undef XT_USE_SWPRI

#if CONFIG_FREERTOS_CORETIMER_0
    #define XT_TIMER_INDEX 0
#elif CONFIG_FREERTOS_CORETIMER_1
    #define XT_TIMER_INDEX 1
#endif

#define configNUM_THREAD_LOCAL_STORAGE_POINTERS CONFIG_FREERTOS_THREAD_LOCAL_STORAGE_POINTERS
#define configTHREAD_LOCAL_STORAGE_DELETE_CALLBACKS 1

#ifndef __ASSEMBLER__
    /**
     * This function is defined to provide a deprecation warning whenever
     * XT_CLOCK_FREQ macro is used.
     * Update the code to use esp_clk_cpu_freq function instead.
     * @return current CPU clock frequency, in Hz
     */
    int xt_clock_freq(void) __attribute__((deprecated));
    #define XT_CLOCK_FREQ   (xt_clock_freq())
#endif // __ASSEMBLER__

#define configXT_BOARD                      1   /* Board mode. */
#define configXT_SIMULATOR					0

#if CONFIG_FREERTOS_ASSERT_ON_UNTESTED_FUNCTION
    #define UNTESTED_FUNCTION() { ets_printf("Untested FreeRTOS function %s\r\n", __FUNCTION__); configASSERT(false); } while(0)
#else
    #define UNTESTED_FUNCTION()
#endif

#endif /* #define FREERTOS_CONFIG_H */
