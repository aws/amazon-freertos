/*
 * Amazon FreeRTOS V1.4.7
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

/* FreeRTOS includes. */
#include "FreeRTOSConfig.h"
#include "FreeRTOS.h"
#include "task.h"

#ifdef CY_USE_LWIP
#include "lwip/tcpip.h"
#endif

/* AWS library includes. */
#include "iot_system_init.h"
#include "iot_logging_task.h"
#include "iot_wifi.h"

/* BSP & Abstraction inclues */
#include "cybsp.h"
#include "cy_retarget_io.h"
#include "cyabs_rtos.h"

//#include "aws_clientcredential.h"

#ifdef CY_TFM_PSA_SUPPORTED
#include "tfm_multi_core_api.h"
#include "tfm_ns_interface.h"
#include "tfm_ns_mailbox.h"
#endif

/* Logging Task Defines. */
#define mainLOGGING_MESSAGE_QUEUE_LENGTH    ( 200 )
#define mainLOGGING_TASK_STACK_SIZE         ( configMINIMAL_STACK_SIZE * 8 )

/* The task delay for allowing the lower priority logging task to print out Wi-Fi
 * failure status before blocking indefinitely. */
#define mainLOGGING_WIFI_STATUS_DELAY       pdMS_TO_TICKS( 5000 )

/* Default MAC address configuration.  The application creates a virtual network
 * connection that uses this MAC address by accessing the raw Ethernet data
 * to and from a real network connection on the host PC.  See the
 * configNETWORK_INTERFACE_TO_USE definition for information on how to configure
 * the real network connection to use. */
const uint8_t ucMACAddress[ 6 ] =
{
    configMAC_ADDR0,
    configMAC_ADDR1,
    configMAC_ADDR2,
    configMAC_ADDR3,
    configMAC_ADDR4,
    configMAC_ADDR5
};

char *security_string[] =
{
        "None     \0",
        "WEP      \0",
        "WPA      \0",
        "WPA2     \0",
        "WPA2-ent \0",
        "Unknown  \0"
};

/**
 * @brief Application task startup hook for applications using Wi-Fi. If you are not
 * using Wi-Fi, then start network dependent applications in the vApplicationIPNetorkEventHook
 * function. If you are not using Wi-Fi, this hook can be disabled by setting
 * configUSE_DAEMON_TASK_STARTUP_HOOK to 0.
 */
void vApplicationDaemonTaskStartupHook( void );

/**
 * @brief Performs WiFi Scanning in a separate thread
 */
void wifi_scan_thread( cy_thread_arg_t arg);
#define MAX_SCAN_NUMBER         100
WIFIScanResult_t ScanResults[ MAX_SCAN_NUMBER ];
cy_thread_t wifi_thread_ptr;

/**
 * @brief Initializes the board.
 */
static void prvMiscInitialization( void );

extern int uxTopUsedPriority;

#ifdef CY_TFM_PSA_SUPPORTED
static struct ns_mailbox_queue_t ns_mailbox_queue;

static void tfm_ns_multi_core_boot(void)
{
    int32_t ret;

    printf("Non-secure code running on non-secure core.");

    if (tfm_ns_wait_for_s_cpu_ready()) {
        printf("Error sync'ing with secure core.");

        /* Avoid undefined behavior after multi-core sync-up failed */
        for (;;) {
        }
    }

    ret = tfm_ns_mailbox_init(&ns_mailbox_queue);
    if (ret != MAILBOX_SUCCESS) {
        printf("Non-secure mailbox initialization failed.");

        /* Avoid undefined behavior after NS mailbox initialization failed */
        for (;;) {
        }
    }
}
#endif

/*-----------------------------------------------------------*/

/**
 * @brief Application runtime entry point.
 */
int main( void )
{
    /* Perform any hardware initialization that does not require the RTOS to be
     * running.  */
    prvMiscInitialization();

#ifdef CY_TFM_PSA_SUPPORTED
    /* Sync with the core running SPE TFM */
    tfm_ns_multi_core_boot();
#endif

    /* Create tasks that are not dependent on the Wi-Fi being initialized. */
    xLoggingTaskInitialize( mainLOGGING_TASK_STACK_SIZE,
                            tskIDLE_PRIORITY,
                            mainLOGGING_MESSAGE_QUEUE_LENGTH );

#ifdef CY_TFM_PSA_SUPPORTED
    /* Initialize TFM interface */
    tfm_ns_interface_init();
#endif

    /* Start the scheduler.  Initialization that requires the OS to be running,
     * including the Wi-Fi initialization, is performed in the RTOS daemon task
     * startup hook. */
    vTaskStartScheduler();

    return 0;
}
/*-----------------------------------------------------------*/

static void prvMiscInitialization( void )
{
    cy_rslt_t result = cybsp_init();
    if (result != CY_RSLT_SUCCESS)
    {
        printf(  "BSP initialization failed \r\n" );
    }
    result = cy_retarget_io_init(CYBSP_DEBUG_UART_TX, CYBSP_DEBUG_UART_RX, CY_RETARGET_IO_BAUDRATE);
    if (result != CY_RSLT_SUCCESS)
    {
        printf( "Retarget IO initializatoin failed \r\n" );
    }
}
/*-----------------------------------------------------------*/
void vApplicationDaemonTaskStartupHook( void )
{
    /* Need to export this var to elf so that debugger can work properly */
    (void)uxTopUsedPriority;

    /* FIX ME: Perform any hardware initialization, that require the RTOS to be
     * running, here. */

    /* FIX ME: If your MCU is using Wi-Fi, delete surrounding compiler directives to
     * enable the unit tests and after MQTT, Bufferpool, and Secure Sockets libraries
     * have been imported into the project. If you are not using Wi-Fi, see the
     * vApplicationIPNetworkEventHook function. */
    if( SYSTEM_Init() == pdPASS )
    {
#ifdef CY_USE_LWIP
        /* Initialize lwIP stack. This needs the RTOS to be up since this function will spawn
         * the tcp_ip thread.
         */
        tcpip_init(NULL, NULL);
#endif

        /* Turn on Wi-Fi before scanning. */
        WIFIReturnCode_t ret = WIFI_On();
        if( ret == eWiFiSuccess)
        {
            if( cy_rtos_create_thread(&wifi_thread_ptr,
                                      (cy_thread_entry_fn_t)wifi_scan_thread,
                                      "WiFiScan", NULL, 1024,
                                      CY_RTOS_PRIORITY_NORMAL, (cy_thread_arg_t)NULL)
                    != CY_RSLT_SUCCESS)
            {
                /* Could not start thread */
                configPRINTF( ("Could not start WiFi Scanner thread\n") );
            }
        }
        else
        {
            printf("WiFi_ON returned:0x%x\r\n", ret);
        }
    }
}


/*-----------------------------------------------------------*/

void wifi_scan_thread( cy_thread_arg_t arg)
{
    WIFIReturnCode_t WiFiStatus;
    printf("\r\nCypress Wifi Scan\r\n");

    while(1)
    {
        memset( ScanResults, 0x00, sizeof(ScanResults));
        WiFiStatus = WIFI_Scan( ScanResults, MAX_SCAN_NUMBER );
        if ( WiFiStatus == eWiFiSuccess)
        {
            int i;
            printf("\r\nindex   CH   RSSI   Security          BSSID         SSID\r\n");
            for (i = 0; i < MAX_SCAN_NUMBER; i++)
            {
                if ((ScanResults[i].ucBSSID[0] == 0) && (ScanResults[i].ucBSSID[1] == 0) && (ScanResults[i].ucBSSID[2] == 0) &&
                    (ScanResults[i].ucBSSID[3] == 0) && (ScanResults[i].ucBSSID[4] == 0) && (ScanResults[i].ucBSSID[5] == 0) )
                {
                    break;
                }
                printf("[%3i]  %3d   %3d    (%d)%s  %02x.%02x.%02x.%02x.%02x.%02x  %s\r\n",
                        i, ScanResults[i].cChannel, ScanResults[i].cRSSI, ScanResults[i].xSecurity,
                        (ScanResults[i].xSecurity < eWiFiSecurityNotSupported ) ? security_string[ScanResults[i].xSecurity] : "Unknown  ",
                        ScanResults[i].ucBSSID[0], ScanResults[i].ucBSSID[1], ScanResults[i].ucBSSID[2],
                        ScanResults[i].ucBSSID[3], ScanResults[i].ucBSSID[4], ScanResults[i].ucBSSID[5],
                        ScanResults[i].cSSID);
            }
        }
        vTaskDelay( mainLOGGING_WIFI_STATUS_DELAY  );
    }
}

/*-----------------------------------------------------------*/

/**
 * @brief User defined Idle task function.
 *
 * @note Do not make any blocking operations in this function.
 */
void vApplicationIdleHook( void )
{
    /* FIX ME. If necessary, update to application idle periodic actions. */

    static TickType_t xLastPrint = 0;
    TickType_t xTimeNow;
    const TickType_t xPrintFrequency = pdMS_TO_TICKS( 5000 );

    xTimeNow = xTaskGetTickCount();

    if( ( xTimeNow - xLastPrint ) > xPrintFrequency )
    {
        xLastPrint = xTimeNow;
    }
}

void vApplicationTickHook()
{
}

/*-----------------------------------------------------------*/

/**
* @brief User defined application hook to process names returned by the DNS server.
*/
#if ( ipconfigUSE_LLMNR != 0 ) || ( ipconfigUSE_NBNS != 0 )
    BaseType_t xApplicationDNSQueryHook( const char * pcName )
    {
        /* FIX ME. If necessary, update to applicable DNS name lookup actions. */

        BaseType_t xReturn;

        /* Determine if a name lookup is for this node.  Two names are given
         * to this node: that returned by pcApplicationHostnameHook() and that set
         * by mainDEVICE_NICK_NAME. */
        if( strcmp( pcName, pcApplicationHostnameHook() ) == 0 )
        {
            xReturn = pdPASS;
        }
        else if( strcmp( pcName, mainDEVICE_NICK_NAME ) == 0 )
        {
            xReturn = pdPASS;
        }
        else
        {
            xReturn = pdFAIL;
        }

        return xReturn;
    }

#endif /* if ( ipconfigUSE_LLMNR != 0 ) || ( ipconfigUSE_NBNS != 0 ) */
/*-----------------------------------------------------------*/

/**
 * @brief User defined assertion call. This function is plugged into configASSERT.
 * See FreeRTOSConfig.h to define configASSERT to something different.
 */
void vAssertCalled(const char * pcFile,
	uint32_t ulLine)
{
    /* FIX ME. If necessary, update to applicable assertion routine actions. */

	const uint32_t ulLongSleep = 1000UL;
	volatile uint32_t ulBlockVariable = 0UL;
	volatile char * pcFileName = (volatile char *)pcFile;
	volatile uint32_t ulLineNumber = ulLine;

	(void)pcFileName;
	(void)ulLineNumber;

	printf("vAssertCalled %s, %ld\n", pcFile, (long)ulLine);
	fflush(stdout);

	/* Setting ulBlockVariable to a non-zero value in the debugger will allow
	* this function to be exited. */
	taskDISABLE_INTERRUPTS();
	{
		while (ulBlockVariable == 0UL)
		{
			vTaskDelay( pdMS_TO_TICKS( ulLongSleep ) );
		}
	}
	taskENABLE_INTERRUPTS();
}
/*-----------------------------------------------------------*/

/**
 * @brief User defined application hook need by the FreeRTOS-Plus-TCP library.
 */
#if ( ipconfigUSE_LLMNR != 0 ) || ( ipconfigUSE_NBNS != 0 ) || ( ipconfigDHCP_REGISTER_HOSTNAME == 1 )
    const char * pcApplicationHostnameHook(void)
    {
        /* FIX ME: If necessary, update to applicable registration name. */

        /* This function will be called during the DHCP: the machine will be registered
         * with an IP address plus this name. */
        return clientcredentialIOT_THING_NAME;
    }

#endif

/*-----------------------------------------------------------*/
    /**
     * @brief Warn user if pvPortMalloc fails.
     *
     * Called if a call to pvPortMalloc() fails because there is insufficient
     * free memory available in the FreeRTOS heap.  pvPortMalloc() is called
     * internally by FreeRTOS API functions that create tasks, queues, software
     * timers, and semaphores.  The size of the FreeRTOS heap is set by the
     * configTOTAL_HEAP_SIZE configuration constant in FreeRTOSConfig.h.
     *
     */
    void vApplicationMallocFailedHook()
    {
        configPRINTF( ( "ERROR: Malloc failed to allocate memory\r\n" ) );
        taskDISABLE_INTERRUPTS();

        /* Loop forever */
        for( ; ; )
        {
        }
    }

/* configUSE_STATIC_ALLOCATION is set to 1, so the application must provide an
 * implementation of vApplicationGetIdleTaskMemory() to provide the memory that is
 * used by the Idle task. */
void vApplicationGetIdleTaskMemory( StaticTask_t ** ppxIdleTaskTCBBuffer,
                                    StackType_t ** ppxIdleTaskStackBuffer,
                                    uint32_t * pulIdleTaskStackSize )
{
    /* If the buffers to be provided to the Idle task are declared inside this
     * function then they must be declared static - otherwise they will be allocated on
     * the stack and so not exists after this function exits. */
    static StaticTask_t xIdleTaskTCB;
    static StackType_t uxIdleTaskStack[ configMINIMAL_STACK_SIZE ];

    /* Pass out a pointer to the StaticTask_t structure in which the Idle
     * task's state will be stored. */
    *ppxIdleTaskTCBBuffer = &xIdleTaskTCB;

    /* Pass out the array that will be used as the Idle task's stack. */
    *ppxIdleTaskStackBuffer = uxIdleTaskStack;

    /* Pass out the size of the array pointed to by *ppxIdleTaskStackBuffer.
     * Note that, as the array is necessarily of type StackType_t,
     * configMINIMAL_STACK_SIZE is specified in words, not bytes. */
    *pulIdleTaskStackSize = configMINIMAL_STACK_SIZE;
}
/*-----------------------------------------------------------*/

/**
 * @brief This is to provide the memory that is used by the RTOS daemon/time task.
 *
 * If configUSE_STATIC_ALLOCATION is set to 1, so the application must provide an
 * implementation of vApplicationGetTimerTaskMemory() to provide the memory that is
 * used by the RTOS daemon/time task.
 */
void vApplicationGetTimerTaskMemory( StaticTask_t ** ppxTimerTaskTCBBuffer,
                                     StackType_t ** ppxTimerTaskStackBuffer,
                                     uint32_t * pulTimerTaskStackSize )
{
    /* If the buffers to be provided to the Timer task are declared inside this
     * function then they must be declared static - otherwise they will be allocated on
     * the stack and so not exists after this function exits. */
    static StaticTask_t xTimerTaskTCB;
    static StackType_t uxTimerTaskStack[ configTIMER_TASK_STACK_DEPTH ];

    /* Pass out a pointer to the StaticTask_t structure in which the Idle
     * task's state will be stored. */
    *ppxTimerTaskTCBBuffer = &xTimerTaskTCB;

    /* Pass out the array that will be used as the Timer task's stack. */
    *ppxTimerTaskStackBuffer = uxTimerTaskStack;

    /* Pass out the size of the array pointed to by *ppxTimerTaskStackBuffer.
     * Note that, as the array is necessarily of type StackType_t,
     * configMINIMAL_STACK_SIZE is specified in words, not bytes. */
    *pulTimerTaskStackSize = configTIMER_TASK_STACK_DEPTH;
}
/*-----------------------------------------------------------*/

/**
 * @brief Loop forever if stack overflow is detected.
 *
 * If configCHECK_FOR_STACK_OVERFLOW is set to 1,
 * this hook provides a location for applications to
 * define a response to a stack overflow.
 *
 * Use this hook to help identify that a stack overflow
 * has occurred.
 *
 */
void vApplicationStackOverflowHook( TaskHandle_t xTask,
                                    char * pcTaskName )
{
    configPRINTF( ( "ERROR: stack overflow with task %s\r\n", pcTaskName ) );
    portDISABLE_INTERRUPTS();

    /* Unused Parameters */
    ( void ) xTask;

    /* Loop forever */
    for( ; ; )
    {
    }
}
