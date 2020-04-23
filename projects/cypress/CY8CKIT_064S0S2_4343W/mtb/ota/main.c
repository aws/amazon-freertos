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
#include "FreeRTOS.h"
#include "FreeRTOSConfig.h"
#include "task.h"
#ifdef CY_BOOT_USE_EXTERNAL_FLASH
#include "flash_qspi.h"
#include "cy_smif_psoc6.h"
#endif

#ifdef CY_USE_LWIP
#include "lwip/tcpip.h"
#endif

/* Demo includes */
#include "aws_demo.h"

/* AWS library includes. */
#include "iot_system_init.h"
#include "iot_logging_task.h"
#include "iot_wifi.h"
#include "aws_clientcredential.h"
#include "aws_application_version.h"
#include "./include/aws_dev_mode_key_provisioning.h"

/* BSP & Abstraction inclues */
#include "cybsp.h"
#include "cyhal_gpio.h"
#include "cy_retarget_io.h"
#include "sysflash.h"

#ifdef CY_TFM_PSA_SUPPORTED
#include "tfm_multi_core_api.h"
#include "tfm_ns_interface.h"
#include "tfm_ns_mailbox.h"
#endif

#if (BLINK_FREQ_UPDATE_OTA == 0)
#define OTA_APP_BLINKY_FREQ     pdMS_TO_TICKS( 1000 )
#else
#define OTA_APP_BLINKY_FREQ     pdMS_TO_TICKS( 250 )
#endif

/* Logging Task Defines. */
#define mainLOGGING_MESSAGE_QUEUE_LENGTH    ( 200 )
#define mainLOGGING_TASK_STACK_SIZE         ( configMINIMAL_STACK_SIZE * 8 )

/* The task delay for allowing the lower priority logging task to print out Wi-Fi
 * failure status before blocking indefinitely. */
#define mainLOGGING_WIFI_STATUS_DELAY       pdMS_TO_TICKS( 1000 )

/* Unit test defines. */
#define mainTEST_RUNNER_TASK_STACK_SIZE     ( configMINIMAL_STACK_SIZE * 16 )

/* The name of the devices for xApplicationDNSQueryHook. */
#define mainDEVICE_NICK_NAME				"cypress_demo" /* FIX ME.*/

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

/**
 * @brief Application task startup hook for applications using Wi-Fi. If you are not
 * using Wi-Fi, then start network dependent applications in the vApplicationIPNetorkEventHook
 * function. If you are not using Wi-Fi, this hook can be disabled by setting
 * configUSE_DAEMON_TASK_STARTUP_HOOK to 0.
 */
void vApplicationDaemonTaskStartupHook( void );

/**
 * @brief Connects to Wi-Fi.
 */
static void prvWifiConnect( void );

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
#ifdef CY_TFM_PSA_SUPPORTED
    tfm_ns_multi_core_boot();
#endif

    /* Perform any hardware initialization that does not require the RTOS to be
     * running.  */
    prvMiscInitialization();

    /* Create tasks that are not dependent on the Wi-Fi being initialized. */
    xLoggingTaskInitialize( mainLOGGING_TASK_STACK_SIZE,
                            tskIDLE_PRIORITY,
                            mainLOGGING_MESSAGE_QUEUE_LENGTH );

#ifdef CY_TFM_PSA_SUPPORTED
    /* Initialize TFM interface */
    tfm_ns_interface_init();
#endif

    /* FIX ME: If you are using Ethernet network connections and the FreeRTOS+TCP stack,
     * uncomment the initialization function, FreeRTOS_IPInit(), below. */
    /*FreeRTOS_IPInit( ucIPAddress,
     *                 ucNetMask,
     *                 ucGatewayAddress,
     *                 ucDNSServerAddress,
     *                 ucMACAddress );
     */

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
        printf( "Retarget IO initialization failed \r\n" );
    }

    /*    printf("\x1b[2J\x1b[H");    clear screen */
    printf("\r\nCypress FreeRTOS OTA Demo - App Version: %d.%d.%d\r\n", APP_VERSION_MAJOR, APP_VERSION_MINOR, APP_VERSION_BUILD );

    /* Initialize the User LED */
    cyhal_gpio_init((cyhal_gpio_t) CYBSP_USER_LED1, CYHAL_GPIO_DIR_OUTPUT, CYHAL_GPIO_DRIVE_STRONG, CYBSP_LED_STATE_OFF);

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

        /* Connect to the Wi-Fi before running the tests. */
        prvWifiConnect();

        /* Provision the device with AWS certificate and private key. */
        vDevModeKeyProvisioning();

        /* Start the demo tasks. */
        DEMO_RUNNER_RunDemos();
    }
}
/*-----------------------------------------------------------*/

void prvWifiConnect( void )
{
    WIFINetworkParams_t xNetworkParams;
    WIFIReturnCode_t xWifiStatus;
    uint8_t ucTempIp[4] = { 0 };

    xWifiStatus = WIFI_On();

    if( xWifiStatus == eWiFiSuccess )
    {

        configPRINTF( ( "Wi-Fi module initialized. Connecting to AP...\r\n" ) );
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
        /* Connection failed, configure SoftAP. */
        configPRINTF( ( "Wi-Fi failed to connect to AP %s.\r\n", xNetworkParams.pcSSID ) );

        xNetworkParams.pcSSID = wificonfigACCESS_POINT_SSID_PREFIX;
        xNetworkParams.pcPassword = wificonfigACCESS_POINT_PASSKEY;
        xNetworkParams.xSecurity = wificonfigACCESS_POINT_SECURITY;
        xNetworkParams.cChannel = wificonfigACCESS_POINT_CHANNEL;

        configPRINTF( ( "Connect to SoftAP %s using password %s. \r\n",
                        xNetworkParams.pcSSID, xNetworkParams.pcPassword ) );

        while( WIFI_ConfigureAP( &xNetworkParams ) != eWiFiSuccess )
        {
            configPRINTF( ( "Connect to SoftAP %s using password %s and configure Wi-Fi. \r\n",
                            xNetworkParams.pcSSID, xNetworkParams.pcPassword ) );
        }

        configPRINTF( ( "Wi-Fi configuration successful. \r\n" ) );
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

    static TickType_t xLastToggle = 0;
    TickType_t xTimeNow;
    const TickType_t xBlinkyFrequency = OTA_APP_BLINKY_FREQ;

    xTimeNow = xTaskGetTickCount();

    if( ( xTimeNow - xLastToggle ) > xBlinkyFrequency )
    {
        /* Invert the USER LED state */
        cyhal_gpio_toggle((cyhal_gpio_t) CYBSP_USER_LED1);

        xLastToggle = xTimeNow;
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
