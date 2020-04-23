/*
 * Amazon FreeRTOS V1.1.4
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

/* FIX ME: If you are using Ethernet network connections and the FreeRTOS+TCP stack,
 * uncomment the define:
 * #define CY_USE_FREERTOS_TCP 1
 */

/* FreeRTOS includes. */
#include "FreeRTOS.h"
#include "task.h"
#ifdef CY_BOOT_USE_EXTERNAL_FLASH
#include "flash_qspi.h"
#include "cy_smif_psoc6.h"
#endif

#ifdef CY_USE_LWIP
#include "lwip/tcpip.h"
#endif
//#include "FreeRTOS_IP.h" /* FIX ME: Delete if you are not using the FreeRTOS-Plus-TCP library. */

/* Test includes */
#include "aws_test_runner.h"

/* AWS library includes. */
#include "iot_system_init.h"
#include "iot_logging_task.h"
#include "iot_wifi.h"
#include "aws_clientcredential.h"
#include "aws_dev_mode_key_provisioning.h"

/* BSP & Abstraction inclues */
#include "cybsp.h"
#include "cy_retarget_io.h"

#ifdef CY_TFM_PSA_SUPPORTED
#include "tfm_multi_core_api.h"
#include "tfm_ns_interface.h"
#include "tfm_ns_mailbox.h"
#if ( testrunnerFULL_TFM_ENABLED == 1 )
#include "test_framework_integ_test.h"
#include "tfm_secure_client_service_api.h"
#endif
#endif

#if (CY_BLE_SUPPORTED == 1)
#include "bt_hal_manager_types.h"
#endif

/* Logging Task Defines. */
#define mainLOGGING_MESSAGE_QUEUE_LENGTH    ( 15 )
#define mainLOGGING_TASK_STACK_SIZE         ( configMINIMAL_STACK_SIZE * 8 )

/* Unit test defines. */
#define mainTEST_RUNNER_TASK_STACK_SIZE     ( configMINIMAL_STACK_SIZE * 16 )

#if (testrunnerFULL_BLE_ENABLED == 1)
#define mainTEST_RUNNER_TASK_PRIORITY     ( tskIDLE_PRIORITY + 6 )
#else
#define mainTEST_RUNNER_TASK_PRIORITY     ( 2 )
#endif

/* The task delay for allowing the lower priority logging task to print out Wi-Fi
 * failure status before blocking indefinitely. */
#define mainLOGGING_WIFI_STATUS_DELAY       pdMS_TO_TICKS( 1000 )

/* The name of the devices for xApplicationDNSQueryHook. */
#define mainDEVICE_NICK_NAME				"vendor_demo" /* FIX ME.*/

#ifdef CY_USE_FREERTOS_TCP
/* Static arrays for FreeRTOS-Plus-TCP stack initialization for Ethernet network
 * connections are declared below. If you are using an Ethernet connection on your MCU
 * device it is recommended to use the FreeRTOS+TCP stack. The default values are
 * defined in FreeRTOSConfig.h. */

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

/* The default IP and MAC address used by the application.  The address configuration
 * defined here will be used if ipconfigUSE_DHCP is 0, or if ipconfigUSE_DHCP is
 * 1 but a DHCP server could not be contacted.  See the online documentation for
 * more information. */
static const uint8_t ucIPAddress[ 4 ] =
{
    configIP_ADDR0,
    configIP_ADDR1,
    configIP_ADDR2,
    configIP_ADDR3
};
static const uint8_t ucNetMask[ 4 ] =
{
    configNET_MASK0,
    configNET_MASK1,
    configNET_MASK2,
    configNET_MASK3
};
static const uint8_t ucGatewayAddress[ 4 ] =
{
    configGATEWAY_ADDR0,
    configGATEWAY_ADDR1,
    configGATEWAY_ADDR2,
    configGATEWAY_ADDR3
};
static const uint8_t ucDNSServerAddress[ 4 ] =
{
    configDNS_SERVER_ADDR0,
    configDNS_SERVER_ADDR1,
    configDNS_SERVER_ADDR2,
    configDNS_SERVER_ADDR3
};
#endif /* CY_USE_FREERTOS_TCP */

#if ( defined(CY_TFM_PSA_SUPPORTED) && (testrunnerFULL_TFM_ENABLED == 1) )
void tfm_test_task( void * arg );
void test_crypto_random(void);
#endif

/**
 * @brief Application task startup hook for applications using Wi-Fi. If you are not
 * using Wi-Fi, then start network dependent applications in the vApplicationIPNetorkEventHook
 * function. If you are not using Wi-Fi, this hook can be disabled by setting
 * configUSE_DAEMON_TASK_STARTUP_HOOK to 0.
 */
void vApplicationDaemonTaskStartupHook( void );

/**
 * @brief Application IP network event hook called by the FreeRTOS+TCP stack for
 * applications using Ethernet. If you are not using Ethernet and the FreeRTOS+TCP stack,
 * start network dependent applications in vApplicationDaemonTaskStartupHook after the
 * network status is up.
 */
//void vApplicationIPNetworkEventHook( eIPCallbackEvent_t eNetworkEvent );

/**
 * @brief Connects to Wi-Fi.
 */
static void prvWifiConnect( void );

/**
 * @brief Initializes the board.
 */
static void prvMiscInitialization( void );

#ifdef CY_TFM_PSA_SUPPORTED
static struct ns_mailbox_queue_t ns_mailbox_queue;

static void tfm_ns_multi_core_boot(void)
{
    int32_t ret;

    printf("Non-secure code running on non-secure core.\r\n");

    if (tfm_ns_wait_for_s_cpu_ready()) {
        printf("Error sync'ing with secure core.\r\n");

        /* Avoid undefined behavior after multi-core sync-up failed */
        for (;;) {
        }
    }

    ret = tfm_ns_mailbox_init(&ns_mailbox_queue);
    if (ret != MAILBOX_SUCCESS) {
        printf("Non-secure mailbox initialization failed.\r\n");

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

#ifdef CY_USE_FREERTOS_TCP
    FreeRTOS_IPInit( ucIPAddress,
                     ucNetMask,
                     ucGatewayAddress,
                     ucDNSServerAddress,
                     ucMACAddress );
#endif /* CY_USE_FREERTOS_TCP */

    /* Start the scheduler.  Initialization that requires the OS to be running,
     * including the Wi-Fi initialization, is performed in the RTOS daemon task
     * startup hook. */
    vTaskStartScheduler();

    return 0;
}

#if (CY_BLE_SUPPORTED == 1)
BTStatus_t bleStackInit( void )
{
    return eBTStatusSuccess;
}
#endif
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
        printf( "Retarget IO initialization failed \r\n" );
    }

#ifdef CY_BOOT_USE_EXTERNAL_FLASH
    __enable_irq();

#ifdef PDL_CODE
    if (qspi_init_sfdp(1) < 0)
    {
        printf("QSPI Init failed\r\n");
        while (1);
    }
#else   /* PDL_CODE */
    if (psoc6_qspi_init() != 0)
    {
       printf("psoc6_qspi_init() FAILED!!\r\n");
    }

#endif /* PDL_CODE */
#endif /* CY_BOOT_USE_EXTERNAL_FLASH */
}
/*-----------------------------------------------------------*/

void vApplicationDaemonTaskStartupHook( void )
{
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

#if ( defined(CY_TFM_PSA_SUPPORTED) && (testrunnerFULL_TFM_ENABLED == 1) )
        /* Create the task to run TFM tests. */
        xTaskCreate( tfm_test_task,
                        "RunTests_task",
                        mainTEST_RUNNER_TASK_STACK_SIZE,
                        NULL,
                        mainTEST_RUNNER_TASK_PRIORITY,
                        NULL );
#else /* #if ( defined(CY_TFM_PSA_SUPPORTED) && (testrunnerFULL_TFM_ENABLED == 1) ) */
        /* Connect to the Wi-Fi before running the tests. */
        prvWifiConnect();

        /* Provision the device with AWS certificate and private key. */
        vDevModeKeyProvisioning();

        /* Create the task to run unit tests. */
        xTaskCreate( TEST_RUNNER_RunTests_task,
                        "RunTests_task",
                        mainTEST_RUNNER_TASK_STACK_SIZE,
                        NULL,
                        mainTEST_RUNNER_TASK_PRIORITY,
                        NULL );
#endif /* #if ( defined(CY_TFM_PSA_SUPPORTED) && (testrunnerFULL_TFM_ENABLED == 1) ) */
    }
}
/*-----------------------------------------------------------*/

// void vApplicationIPNetworkEventHook( eIPCallbackEvent_t eNetworkEvent )
// {
//     /* FIX ME: If your application is using Ethernet network connections and the
//      * FreeRTOS+TCP stack, delete the surrounding compiler directives to enable the
//      * unit tests and after MQTT, Bufferpool, and Secure Sockets libraries have been
//      * imported into the project. If you are not using Ethernet see the
//      * vApplicationDaemonTaskStartupHook function. */
//     #if 0
//     static BaseType_t xTasksAlreadyCreated = pdFALSE;

//     /* If the network has just come up...*/
//     if( eNetworkEvent == eNetworkUp )
//     {
//         if( ( xTasksAlreadyCreated == pdFALSE ) && ( SYSTEM_Init() == pdPASS ) )
//         {
//             xTaskCreate( TEST_RUNNER_RunTests_task,
//                          "TestRunner",
//                          TEST_RUNNER_TASK_STACK_SIZE,
//                          NULL,
//                          tskIDLE_PRIORITY, NULL );

//             xTasksAlreadyCreated = pdTRUE;
//         }
//     }
//     #endif /* if 0 */
// }
/*-----------------------------------------------------------*/

void prvWifiConnect( void )
{
    /* FIX ME: Delete surrounding compiler directives when the Wi-Fi library is ported. */
    WIFINetworkParams_t xNetworkParams;
    WIFIReturnCode_t xWifiStatus;
    uint8_t ucTempIp[4] = { 0 };

    xWifiStatus = WIFI_On();

    if( xWifiStatus == eWiFiSuccess )
    {
        configPRINTF( ( "Wi-Fi module initialized. Connecting to AP %s...\r\n", clientcredentialWIFI_SSID ) );
    }
    else
    {
        configPRINTF( ( "Wi-Fi module failed to initialize.\r\n" ) );

        /* Delay to allow the lower priority logging task to print the above status.
            * The while loop below will block the above printing. */
        vTaskDelay( mainLOGGING_WIFI_STATUS_DELAY );

        while( 1 )
        {
        }
    }

    /* Setup parameters. */
    xNetworkParams.pcSSID = clientcredentialWIFI_SSID;
    xNetworkParams.ucSSIDLength = sizeof( clientcredentialWIFI_SSID ) - 1;
    xNetworkParams.pcPassword = clientcredentialWIFI_PASSWORD;
    xNetworkParams.ucPasswordLength = sizeof( clientcredentialWIFI_PASSWORD ) - 1;
    xNetworkParams.xSecurity = clientcredentialWIFI_SECURITY;
    xNetworkParams.cChannel = 0;

    xWifiStatus = WIFI_ConnectAP( &( xNetworkParams ) );

    if( xWifiStatus == eWiFiSuccess )
    {
        configPRINTF( ( "Wi-Fi Connected to AP. Creating tasks which use network...\r\n" ) );

        xWifiStatus = WIFI_GetIP( ucTempIp );
        if ( eWiFiSuccess == xWifiStatus )
        {
            configPRINTF( ( "IP Address acquired %d.%d.%d.%d\r\n",
                            ucTempIp[ 0 ], ucTempIp[ 1 ], ucTempIp[ 2 ], ucTempIp[ 3 ] ) );
        }
    }
    else
    {
        configPRINTF( ( "Wi-Fi failed to connect to AP.\r\n" ) );

        /* Delay to allow the lower priority logging task to print the above status.
            * The while loop below will block the above printing. */
        vTaskDelay( mainLOGGING_WIFI_STATUS_DELAY );

        while( 1 )
        {
        }
    }
}
/*-----------------------------------------------------------*/

/**
 * @brief This is to provide memory that is used by the Idle task.
 *
 * If configUSE_STATIC_ALLOCATION is set to 1, then the application must provide an
 * implementation of vApplicationGetIdleTaskMemory() in order to provide memory to
 * the Idle task.
 */
// void vApplicationGetIdleTaskMemory( StaticTask_t ** ppxIdleTaskTCBBuffer,
//                                     StackType_t ** ppxIdleTaskStackBuffer,
//                                     uint32_t * pulIdleTaskStackSize )
// {
//     /* If the buffers to be provided to the Idle task are declared inside this
//      * function then they must be declared static - otherwise they will be allocated on
//      * the stack and so not exists after this function exits. */
//     static StaticTask_t xIdleTaskTCB;
//     static StackType_t uxIdleTaskStack[ configMINIMAL_STACK_SIZE ];

//     /* Pass out a pointer to the StaticTask_t structure in which the Idle
//      * task's state will be stored. */
//     *ppxIdleTaskTCBBuffer = &xIdleTaskTCB;

//     /* Pass out the array that will be used as the Idle task's stack. */
//     *ppxIdleTaskStackBuffer = uxIdleTaskStack;

//     /* Pass out the size of the array pointed to by *ppxIdleTaskStackBuffer.
//      * Note that, as the array is necessarily of type StackType_t,
//      * configMINIMAL_STACK_SIZE is specified in words, not bytes. */
//     *pulIdleTaskStackSize = configMINIMAL_STACK_SIZE;
// }
// /*-----------------------------------------------------------*/

// /**
//  * @brief This is to provide the memory that is used by the RTOS daemon/time task.
//  *
//  * If configUSE_STATIC_ALLOCATION is set to 1, then application must provide an
//  * implementation of vApplicationGetTimerTaskMemory() in order to provide memory
//  * to the RTOS daemon/time task.
//  */
// void vApplicationGetTimerTaskMemory( StaticTask_t ** ppxTimerTaskTCBBuffer,
//                                      StackType_t ** ppxTimerTaskStackBuffer,
//                                      uint32_t * pulTimerTaskStackSize )
// {
//     /* If the buffers to be provided to the Timer task are declared inside this
//      * function then they must be declared static - otherwise they will be allocated on
//      * the stack and so not exists after this function exits. */
//     static StaticTask_t xTimerTaskTCB;
//     static StackType_t uxTimerTaskStack[ configTIMER_TASK_STACK_DEPTH ];

//     /* Pass out a pointer to the StaticTask_t structure in which the Idle
//      * task's state will be stored. */
//     *ppxTimerTaskTCBBuffer = &xTimerTaskTCB;

//     /* Pass out the array that will be used as the Timer task's stack. */
//     *ppxTimerTaskStackBuffer = uxTimerTaskStack;

//     /* Pass out the size of the array pointed to by *ppxTimerTaskStackBuffer.
//      * Note that, as the array is necessarily of type StackType_t,
//      * configMINIMAL_STACK_SIZE is specified in words, not bytes. */
//     *pulTimerTaskStackSize = configTIMER_TASK_STACK_DEPTH;
// }
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
// void vApplicationMallocFailedHook()
// {
//     /* The TCP tests will test behavior when the entire heap is allocated. In
//      * order to avoid interfering with those tests, this function does nothing. */
// }
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
// void vApplicationStackOverflowHook( TaskHandle_t xTask,
//                                     char * pcTaskName )
// {
//     portDISABLE_INTERRUPTS();

//     /* Loop forever */
//     for( ; ; )
//     {
//     }
// }
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
        configPRINT( "." );
        xLastPrint = xTimeNow;
    }
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

#if ( defined(CY_TFM_PSA_SUPPORTED) && (testrunnerFULL_TFM_ENABLED == 1) )
void test_crypto_random(void)
{
    static const unsigned char trail[] = "don't overwrite me";
    unsigned char changed[256] = { 0 };
    unsigned char output[sizeof(changed) + sizeof(trail)];
    size_t i, bytes = sizeof(changed);
    unsigned int run;
    int ret;

    printf("\r\n*** Execute CRYPTO RANDOM TEST ***\r\n");
    memcpy(output + bytes, trail, sizeof(trail));
    /* Run several times, to ensure that every output byte will be
     * nonzero at least once with overwhelming probability
     * (2^(-8*number_of_runs)). */
    for (run = 0; run < 10; run++) {
        memset(output, 0, bytes);
        ret = psa_generate_random(output, bytes);
        if ( PSA_SUCCESS != ret ) {
            printf(" !!! CRYPTO RANDOM TEST FAILED as psa_generate_random() returns error(%x) !!!\r\n", ret);
            return;
        }

        /* Check that no more than 'bytes' have been overwritten */
        for (i = 0; i < sizeof(trail); i++) {
            if (output[bytes + i] != trail[i]) {
                printf(" !!! CRYPTO RANDOM TEST FAILED as psa_generate_random() overwriting the buffer !!!\r\n");
                return;
            }
        }

        for (i = 0; i < bytes; i++) {
            if (0 != output[i]) {
                ++changed[i];
            }
        }
    }

    /* Check that every byte was changed to nonzero at least once. This
     * validates that psa_generate_random is overwriting every byte of
     * the output buffer. */
    for (i = 0; i < bytes; i++) {
        if (0 == changed[i]) {
            printf(" !!! CRYPTO RANDOM TEST FAILED as one of byte is zero !!!\r\n");
            return;
        }
    }
    printf("*** CRYPTO RANDOM TEST has PASSED ***\r\n");
}

/*******************************************************************************
* Function Name: tfm_test_task
********************************************************************************
* Summary:
* This is the FreeRTOS task callback function. It does...
*    1. TFM Regression test
*
* Parameters:
*  void * context passed from main function
*
* Return:
*  void
*
*******************************************************************************/
void tfm_test_task( void * arg )
{
    (void)arg;

    printf(("Start TFM Regression test in AFR ...\n\n"));

    tfm_secure_client_run_tests();

    tfm_non_secure_client_run_tests();

    printf(("===============================\n"));
    printf(("End of TFM Regression test in AFR\n"));
    printf(("===============================\n"));

    test_crypto_random();
}
#endif

