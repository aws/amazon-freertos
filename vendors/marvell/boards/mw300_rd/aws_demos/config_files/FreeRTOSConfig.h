/*
 * FreeRTOS Kernel V10.2.0
 * Copyright (C) 2017 Amazon.com, Inc. or its affiliates.  All Rights Reserved.
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
/*
 * Copyright (C) 2011-2018, Marvell International Ltd.
 * All Rights Reserved.
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
 * See http://www.freertos.org/a00110.html.
 *----------------------------------------------------------*/

#define configUSE_PREEMPTION			1
#define configUSE_IDLE_HOOK			0
#define configUSE_TICK_HOOK			0
/* #define configCPU_CLOCK_HZ			( ( unsigned long ) 200000000 ) */
#define configCPU_CLOCK_HZ              	( board_cpu_freq( ) )
#define configTICK_RATE_HZ			( ( portTickType ) 1000 )
#define configMAX_PRIORITIES			( 7 )
#define configMINIMAL_STACK_SIZE		( ( unsigned short ) 256 )
/* #define configTOTAL_HEAP_SIZE		( ( size_t ) ( 72 * 1024 ) ) */
#define configMAX_TASK_NAME_LEN			( 16 )
#define configUSE_TRACE_FACILITY		1
#define configUSE_STATS_FORMATTING_FUNCTIONS	1
#define configUSE_16_BIT_TICKS			0
#define configIDLE_SHOULD_YIELD			1
#define configUSE_MUTEXES			1
#if ( CONFIG_ENABLE_STACK_OVERFLOW_CHECK == 1 )
#define configCHECK_FOR_STACK_OVERFLOW		2
#else
#define configCHECK_FOR_STACK_OVERFLOW  	0
#endif
#define configUSE_MALLOC_FAILED_HOOK		0
#define configUSE_RECURSIVE_MUTEXES		1
#define configUSE_COUNTING_SEMAPHORES		1

/* Currently the TCP/IP stack is using dynamic allocation, and the MQTT task is
 * using static allocation. */
#define configSUPPORT_DYNAMIC_ALLOCATION        1
#define configSUPPORT_STATIC_ALLOCATION         1

/* Assert call defined for debug builds. */
void vAssertCalled( const char * pcFile,
                    uint32_t ulLine );

/* Board CPU frequency */
int board_cpu_freq();

#define configAPPLICATION_ALLOCATED_HEAP 1
#define configASSERT( x )    if( ( x ) == 0 )  { taskDISABLE_INTERRUPTS(); for(;;) {;} }

/* Enable this if run time statistics are to be enabled. The support
   functions are already added in WMSDK */
#if ( CONFIG_ENABLE_RUNTIME_STATS == 1 )
#define configGENERATE_RUN_TIME_STATS   	1
#else
#define configGENERATE_RUN_TIME_STATS   	0
#endif

#if ( configGENERATE_RUN_TIME_STATS == 1 )
/* wmsdk: Prototype of function defined in wmsdk */
void portWMSDK_CONFIGURE_TIMER_FOR_RUN_TIME_STATS(void);
unsigned long portWMSDK_GET_RUN_TIME_COUNTER_VALUE(void);

#define portCONFIGURE_TIMER_FOR_RUN_TIME_STATS		\
	portWMSDK_CONFIGURE_TIMER_FOR_RUN_TIME_STATS
#define portGET_RUN_TIME_COUNTER_VALUE		\
	portWMSDK_GET_RUN_TIME_COUNTER_VALUE
#endif /* configGENERATE_RUN_TIME_STATS */

#define configUSE_CO_ROUTINES 			0
#define configMAX_CO_ROUTINE_PRIORITIES 	( 2 )

/* Use application defined heap region */
#define configAPPLICATION_ALLOCATED_HEAP	1

/*  Software timer definitions. */
#define configUSE_TIMERS			1
#define configTIMER_TASK_PRIORITY		( configMAX_PRIORITIES - 1  )
#define configTIMER_QUEUE_LENGTH		5
#define configTIMER_TASK_STACK_DEPTH		( configMINIMAL_STACK_SIZE )

/* Set the following definitions to 1 to include the API function, or zero
to exclude the API function. */
#define INCLUDE_vTaskPrioritySet		1
#define INCLUDE_uxTaskPriorityGet		1
#define INCLUDE_vTaskDelete			1
#define INCLUDE_vTaskCleanUpResources		0
#define INCLUDE_vTaskSuspend			1
#define INCLUDE_vTaskDelayUntil			1
#define INCLUDE_vTaskDelay			1
#define INCLUDE_uxTaskGetStackHighWaterMark	0
#define INCLUDE_pcTaskGetTaskName		1

#define configKERNEL_INTERRUPT_PRIORITY 	0xf0
#define configMAX_SYSCALL_INTERRUPT_PRIORITY 	0xa0 /* equivalent to 0xa0, or priority 5. */

#define xPortPendSVHandler	PendSV_IRQHandler
#define xPortSysTickHandler	SysTick_IRQHandler
#define vPortSVCHandler		SVC_IRQHandler


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
#define configECHO_SERVER_ADDR2              1
#define configECHO_SERVER_ADDR3              4
#define configTCP_ECHO_CLIENT_PORT           7

/* Default MAC address configuration.  The demo creates a virtual network
 * connection that uses this MAC address by accessing the raw Ethernet/WiFi data
 * to and from a real network connection on the host PC.  See the
 * configNETWORK_INTERFACE_TO_USE definition above for information on how to
 * configure the real network connection to use. */
#define configMAC_ADDR0                      0x00
#define configMAC_ADDR1                      0x50
#define configMAC_ADDR2                      0x43
#define configMAC_ADDR3                      0x21
#define configMAC_ADDR4                      0xe5
#define configMAC_ADDR5                      0xc7

/* Default IP address configuration.  Used in ipconfigUSE_DHCP is set to 0, or
 * ipconfigUSE_DHCP is set to 1 but a DNS server cannot be contacted. */
#define configIP_ADDR0                       0
#define configIP_ADDR1                       0
#define configIP_ADDR2                       0
#define configIP_ADDR3                       0

/* Default gateway IP address configuration.  Used in ipconfigUSE_DHCP is set to
 * 0, or ipconfigUSE_DHCP is set to 1 but a DNS server cannot be contacted. */
#define configGATEWAY_ADDR0                  0
#define configGATEWAY_ADDR1                  0
#define configGATEWAY_ADDR2                  0
#define configGATEWAY_ADDR3                  0

/* Default DNS server configuration.  OpenDNS addresses are 208.67.222.222 and
 * 208.67.220.220.  Used in ipconfigUSE_DHCP is set to 0, or ipconfigUSE_DHCP is
 * set to 1 but a DNS server cannot be contacted.*/
#define configDNS_SERVER_ADDR0               0
#define configDNS_SERVER_ADDR1               0
#define configDNS_SERVER_ADDR2               0
#define configDNS_SERVER_ADDR3               0

/* Default netmask configuration.  Used in ipconfigUSE_DHCP is set to 0, or
 * ipconfigUSE_DHCP is set to 1 but a DNS server cannot be contacted. */
#define configNET_MASK0                      255
#define configNET_MASK1                      255
#define configNET_MASK2                      255
#define configNET_MASK3                      0

/* The UDP port to which print messages are sent. */
#define configPRINT_PORT                     ( 15000 )

#define configPROFILING                      ( 0 )

/* Pseudo random number generater used by some demo tasks. */
extern uint32_t ulRand();
#define configRAND32()    ulRand()

/* FIX ME: The platform FreeRTOS is running on. */
#define configPLATFORM_NAME    "MW320"

/* Header required for the tracealyzer recorder library. */
/* #include "trcRecorder.h" */

#define ENABLE_LOGGING
#ifdef ENABLE_LOGGING
/* The function that implements FreeRTOS printf style output, and the macro
 * that maps the configPRINTF() macros to that function. */
extern void vLoggingPrintf( const char * pcFormat, ... );
#define configPRINTF( X )    vLoggingPrintf X

/* Non-format version thread-safe print */
extern void vLoggingPrint( const char * pcMessage );
#define configPRINT( X )     vLoggingPrint( X )
#else
#define configPRINTF( X )
#define configPRINT( X )
#endif

/* Map the logging task's printf to the board specific output function. */
#include <stdio.h>
void wm_printf(const char *format, ...);
#define configPRINT_STRING( X )    wm_printf( X ); /* FIX ME: Change to your devices console print acceptance function. */
/* Sets the length of the buffers into which logging messages are written - so
 * also defines the maximum length of each log message. */
#define configLOGGING_MAX_MESSAGE_LENGTH            100

/* Set to 1 to prepend each log message with a message number, the task name,
 * and a time stamp. */
#define configLOGGING_INCLUDE_TIME_AND_TASK_NAME    1

#endif /* FREERTOS_CONFIG_H */
