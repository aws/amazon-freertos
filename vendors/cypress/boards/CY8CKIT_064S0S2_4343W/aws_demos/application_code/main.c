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

/* Demo includes */
#include "aws_demo.h"

/* AWS library includes. */
#include "iot_system_init.h"
#include "iot_logging_task.h"
#include "iot_wifi.h"
#include "aws_clientcredential.h"
#include "aws_dev_mode_key_provisioning.h"

/* BSP & Abstraction inclues */
#include "cybsp.h"
#include "cy_retarget_io.h"

#include "iot_network_manager_private.h"

#if BLE_SUPPORTED
    #include "bt_hal_manager_adapter_ble.h"
    #include "bt_hal_manager.h"
    #include "bt_hal_gatt_server.h"

    #include "iot_ble.h"
    #include "iot_ble_config.h"
    #include "iot_ble_wifi_provisioning.h"
    #include "iot_ble_numericComparison.h"

    #include "cyhal_uart.h"

    #define INPUT_MSG_ALLOC_SIZE             (50u)
    #define DELAY_BETWEEN_GETC_IN_TICKS      (1500u)
#endif

#ifdef CY_TFM_PSA_SUPPORTED
#include "tfm_multi_core_api.h"
#include "tfm_ns_interface.h"
#include "tfm_ns_mailbox.h"
#endif

/* For Watchdog */
#include "cyhal_wdt.h"

/* Logging Task Defines. */
#define mainLOGGING_MESSAGE_QUEUE_LENGTH    ( 90 )
#define mainLOGGING_TASK_STACK_SIZE         ( configMINIMAL_STACK_SIZE * 8 )

/* The task delay for allowing the lower priority logging task to print out Wi-Fi
 * failure status before blocking indefinitely. */
#define mainLOGGING_WIFI_STATUS_DELAY       pdMS_TO_TICKS( 1000 )

/* Unit test defines. */
#define mainTEST_RUNNER_TASK_STACK_SIZE     ( configMINIMAL_STACK_SIZE * 16 )

/* The name of the devices for xApplicationDNSQueryHook. */
#define mainDEVICE_NICK_NAME				"cypress_demo" /* FIX ME.*/


/* Static arrays for FreeRTOS-Plus-TCP stack initialization for Ethernet network
 * connections are declared below. If you are using an Ethernet connection on your MCU
 * device it is recommended to use the FreeRTOS+TCP stack. The default values are
 * defined in FreeRTOSConfig.h. */

#ifdef CY_USE_FREERTOS_TCP
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

/**
 * @brief Watchdog object and setting value.
 * Watchdog timer initialized at start, must be cleared / reset before timer goes off.
 *
 * For IDT testing, this is needed for CONFIG_OTA_UPDATE_DEMO_ENABLED
 */
#define CY_WATCHDOG_TIMER_MILLISECONDS      (cyhal_wdt_get_max_timeout_ms())
#define CY_WATCHDOG_KICK_TIME_MILLISECONDS  (CY_WATCHDOG_TIMER_MILLISECONDS / 2)    /* time between kicking WDT */
#define CY_WATCHDOG_INITIAL_KICK_COUNT      (4)                                     /* number of times we kick WDT before giving up */
static cyhal_wdt_t prvWatchdogTimerObj;
static TaskHandle_t prvWatchdogTaskHandle;

/**
 * @brief Application task startup hook for applications using Wi-Fi. If you are not
 * using Wi-Fi, then start network dependent applications in the vApplicationIPNetorkEventHook
 * function. If you are not using Wi-Fi, this hook can be disabled by setting
 * configUSE_DAEMON_TASK_STARTUP_HOOK to 0.
 */
void vApplicationDaemonTaskStartupHook( void );

/**
 * @brief Connects to Wi-Fi.
 *
 * returns eWiFiSuccess
 *         eWiFiFailure
 */
static WIFIReturnCode_t prvWifiConnect( void );

/**
 * @brief Initializes the board.
 */
static void prvMiscInitialization( void );

#ifdef CY_TFM_PSA_SUPPORTED
static struct ns_mailbox_queue_t ns_mailbox_queue;

static void tfm_ns_multi_core_boot(void)
{
    int32_t ret;
    if (tfm_ns_wait_for_s_cpu_ready()) {
        /* Error sync'ing with secure core */
        /* Avoid undefined behavior after multi-core sync-up failed */
        for (;;) {
        }
    }

    ret = tfm_ns_mailbox_init(&ns_mailbox_queue);
    if (ret != MAILBOX_SUCCESS) {
        /* Non-secure mailbox initialization failed. */
        /* Avoid undefined behavior after NS mailbox initialization failed */
        for (;;) {
        }
    }
}
#endif

/*-----------------------------------------------------------*/

/**
 * @brief Watchdog Timer Task
 *
 *  Loop prvWatchdogKickCount times, kick WDT every CY_WATCHDOG_KICK_TIME_MILLISECONDS.
 *  When counter runs out, free WDT and exit.
 *
 */
void prvWatchdogTask(void *arg)
{
    int prvWatchdogKickCount = CY_WATCHDOG_INITIAL_KICK_COUNT;
    while(prvWatchdogKickCount-- > 0)
    {
        vTaskDelay(CY_WATCHDOG_KICK_TIME_MILLISECONDS);
        cyhal_wdt_kick(&prvWatchdogTimerObj);
    }
    prvWatchdogTaskHandle = NULL;
    printf("Exit Watchdog Kick task\n");
    vTaskDelete(NULL);
}

/**
 * @brief Start Watchdog Timer Task
 *
 * returns CY_RSLT_SUCCESS
 *         CY_RSLT_TYPE_ERROR
 */
cy_rslt_t prvWatchdogTaskStart(void)
{
    /* Initialize WDT */
    cy_rslt_t result;

    configPRINTF(("Create Watchdog Timer\n"));
    result = cyhal_wdt_init(&prvWatchdogTimerObj, CY_WATCHDOG_TIMER_MILLISECONDS);
    if (result == CY_RSLT_WDT_ALREADY_INITIALIZED)
    {
        /* watchdog already started, just kick it */
        cyhal_wdt_kick(&prvWatchdogTimerObj);
        result = CY_RSLT_SUCCESS;
    }
    if (result == CY_RSLT_SUCCESS)
    {
        BaseType_t create_result;
        create_result = xTaskCreate( prvWatchdogTask, "WDT", configMINIMAL_STACK_SIZE, NULL, IOT_THREAD_DEFAULT_PRIORITY, &prvWatchdogTaskHandle);
        if (create_result != pdPASS)
        {
            /* failed to start the watchdog thread */
            result = CY_RSLT_TYPE_ERROR;
        }
    }
    return result;
}

/**
 * @brief Stop Watchdog Timer Task
 */
void prvWatchdogTaskStop(void)
{
    if ( prvWatchdogTaskHandle != NULL)
    {
        vTaskDelete(prvWatchdogTaskHandle);
        cyhal_wdt_free(&prvWatchdogTimerObj);
    }
    prvWatchdogTaskHandle = NULL;
}

/**
 * @brief Application runtime entry point.
 */
int main( void )
{
    /* Perform any hardware initialization that does not require the RTOS to be
     * running.  */
    BaseType_t xReturnMessage;

    prvMiscInitialization();

#ifdef CY_TFM_PSA_SUPPORTED
    tfm_ns_multi_core_boot();
#endif

    /* Create tasks that are not dependent on the Wi-Fi being initialized. */
    xReturnMessage = xLoggingTaskInitialize( mainLOGGING_TASK_STACK_SIZE,
                                             tskIDLE_PRIORITY,
                                             mainLOGGING_MESSAGE_QUEUE_LENGTH );

#ifdef CY_TFM_PSA_SUPPORTED
    /* Initialize TFM interface */
    tfm_ns_interface_init();
#endif

#ifdef CY_USE_FREERTOS_TCP
    if (pdPASS == xReturnMessage)
    {
        xReturnMessage = FreeRTOS_IPInit( ucIPAddress,
                                          ucNetMask,
                                          ucGatewayAddress,
                                          ucDNSServerAddress,
                                          ucMACAddress );
    }
#endif /* CY_USE_FREERTOS_TCP */

    /* Start the scheduler.  Initialization that requires the OS to be running,
     * including the Wi-Fi initialization, is performed in the RTOS daemon task
     * startup hook. */
    if (pdPASS == xReturnMessage)
    {
        vTaskStartScheduler();
    }

    if (pdPASS == xReturnMessage)
    {
        return 0;
    }
    else
    {
        return 1;
    }
}
/*-----------------------------------------------------------*/

static void prvMiscInitialization( void )
{
    cy_rslt_t result = cybsp_init();
    CY_ASSERT(CY_RSLT_SUCCESS == result);
}

/*-----------------------------------------------------------*/
void vApplicationDaemonTaskStartupHook( void )
{
    cy_rslt_t result = cy_retarget_io_init(CYBSP_DEBUG_UART_TX, CYBSP_DEBUG_UART_RX, CY_RETARGET_IO_BAUDRATE);
    if (result != CY_RSLT_SUCCESS)
    {
        printf( "Retarget IO initialization failed \r\n" );
    }

    result = kvstore_init();
    CY_ASSERT(CY_RSLT_SUCCESS == result);

#ifdef CY_BOOT_USE_EXTERNAL_FLASH
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

    /* FIX ME: If your MCU is using Wi-Fi, delete surrounding compiler directives to
     * enable the unit tests and after MQTT, Bufferpool, and Secure Sockets libraries
     * have been imported into the project. If you are not using Wi-Fi, see the
     * vApplicationIPNetworkEventHook function. */
    CK_RV xResult;

    if( SYSTEM_Init() == pdPASS )
    {
#ifdef CY_USE_LWIP
        /* Initialize lwIP stack. This needs the RTOS to be up since this function will spawn
         * the tcp_ip thread.
         */
        tcpip_init(NULL, NULL);
#endif

        result = prvWatchdogTaskStart();
        if (result != CY_RSLT_SUCCESS)
        {
            printf( "Start Watchdog Task Failed.\r\n");
        }

        /* Connect to the Wi-Fi before running the tests. */
        if (prvWifiConnect() == eWiFiSuccess)
        {
            /* We connected to an AP, or we are a SoftAP, stop the watchdog task */
            prvWatchdogTaskStop();
        }

#if ( pkcs11configVENDOR_DEVICE_CERTIFICATE_SUPPORTED == 0 )
        /* Provision the device with AWS certificate and private key. */
        xResult = vDevModeKeyProvisioning();
#endif

        /* Start the demo tasks. */
        if (xResult == CKR_OK)
        {
            DEMO_RUNNER_RunDemos();
        }
    }
}
/*-----------------------------------------------------------*/

WIFIReturnCode_t prvWifiConnect( void )
{
    WIFINetworkParams_t xNetworkParams = { 0 };
    WIFIReturnCode_t xWifiStatus = eWiFiSuccess;
    WIFIIPConfiguration_t xIpConfig;
    uint8_t *pucIPV4Byte;
    size_t xSSIDLength, xPasswordLength;

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

    /* Setup WiFi parameters to connect to access point. */
    if( clientcredentialWIFI_SSID != NULL )
    {
        xSSIDLength = strlen( clientcredentialWIFI_SSID );
        if( ( xSSIDLength > 0 ) && ( xSSIDLength <= wificonfigMAX_SSID_LEN ) )
        {
            xNetworkParams.ucSSIDLength = xSSIDLength;
            memcpy( xNetworkParams.ucSSID, clientcredentialWIFI_SSID, xSSIDLength );
        }
        else
        {
            configPRINTF(( "Invalid WiFi SSID configured, empty or exceeds allowable length %d.\n", wificonfigMAX_SSID_LEN ));
            xWifiStatus = eWiFiFailure;
        }
    }
    else
    {
        configPRINTF(( "WiFi SSID is not configured.\n" ));
        xWifiStatus = eWiFiFailure;
    }

    xNetworkParams.xSecurity = clientcredentialWIFI_SECURITY;
    if( clientcredentialWIFI_SECURITY == eWiFiSecurityWPA2 )
    {
        if( clientcredentialWIFI_PASSWORD != NULL )
        {
            xPasswordLength = strlen( clientcredentialWIFI_PASSWORD );
            if( ( xPasswordLength > 0 ) && ( xSSIDLength <= wificonfigMAX_PASSPHRASE_LEN ) )
            {
                xNetworkParams.xPassword.xWPA.ucLength = xPasswordLength;
                memcpy( xNetworkParams.xPassword.xWPA.cPassphrase, clientcredentialWIFI_PASSWORD, xPasswordLength );
            }
            else
            {
                configPRINTF(( "Invalid WiFi password configured, empty password or exceeds allowable password length %d.\n", wificonfigMAX_PASSPHRASE_LEN ));
                xWifiStatus = eWiFiFailure;
            }
        }
        else
        {
            configPRINTF(( "WiFi password is not configured.\n" ));
            xWifiStatus = eWiFiFailure;
        }
    }
    xNetworkParams.ucChannel = 0;

    if( xWifiStatus == eWiFiSuccess )
    {
        /* Try connecting using provided wifi credentials. */
        xWifiStatus = WIFI_ConnectAP( &( xNetworkParams ) );
        if( xWifiStatus == eWiFiSuccess )
        {
            configPRINTF( ( "WiFi connected to AP %.*s.\r\n", xNetworkParams.ucSSIDLength, ( char * ) xNetworkParams.ucSSID ) );

            /* Get IP address of the device. */
            xWifiStatus = WIFI_GetIPInfo( &xIpConfig );
            if( xWifiStatus == eWiFiSuccess )
            {
                pucIPV4Byte = ( uint8_t * ) ( &xIpConfig.xIPAddress.ulAddress[0] );
                configPRINTF( ( "IP Address acquired %d.%d.%d.%d.\r\n",
                        pucIPV4Byte[ 0 ], pucIPV4Byte[ 1 ], pucIPV4Byte[ 2 ], pucIPV4Byte[ 3 ] ) );
            }
        }
        else
        {
            /* Connection failed configure softAP to allow user to set wifi credentials. */
            configPRINTF( ( "WiFi failed to connect to AP %.*s.\r\n", xNetworkParams.ucSSIDLength, ( char * ) xNetworkParams.ucSSID  ) );
        }
    }

    if( xWifiStatus != eWiFiSuccess )
    {
        /* Enter SOFT AP mode to provision access point credentials. */
        configPRINTF(( "Entering soft access point WiFi provisioning mode.\n" ));
        xWifiStatus = eWiFiSuccess;
        if( wificonfigACCESS_POINT_SSID_PREFIX != NULL )
        {
            xSSIDLength = strlen( wificonfigACCESS_POINT_SSID_PREFIX );
            if( ( xSSIDLength > 0 ) && ( xSSIDLength <= wificonfigMAX_SSID_LEN ) )
            {
                xNetworkParams.ucSSIDLength = xSSIDLength;
                memcpy( xNetworkParams.ucSSID, clientcredentialWIFI_SSID, xSSIDLength );
            }
            else
            {
                configPRINTF(( "Invalid AP SSID configured, empty or exceeds allowable length %d.\n", wificonfigMAX_SSID_LEN ));
                xWifiStatus = eWiFiFailure;
            }
        }
        else
        {
            configPRINTF(( "WiFi AP SSID is not configured.\n" ));
            xWifiStatus = eWiFiFailure;
        }

        xNetworkParams.xSecurity = wificonfigACCESS_POINT_SECURITY;
        if( wificonfigACCESS_POINT_SECURITY == eWiFiSecurityWPA2 )
        {
            if( wificonfigACCESS_POINT_PASSKEY != NULL )
            {
                xPasswordLength = strlen( wificonfigACCESS_POINT_PASSKEY );
                if( ( xPasswordLength > 0 ) && ( xSSIDLength <= wificonfigMAX_PASSPHRASE_LEN ) )
                {
                    xNetworkParams.xPassword.xWPA.ucLength = xPasswordLength;
                    memcpy( xNetworkParams.xPassword.xWPA.cPassphrase, wificonfigACCESS_POINT_PASSKEY, xPasswordLength );
                }
                else
                {
                    configPRINTF(( "Invalid WiFi AP password, empty or exceeds allowable password length %d.\n", wificonfigMAX_PASSPHRASE_LEN ));
                    xWifiStatus = eWiFiFailure;
                }
            }
            else
            {
                configPRINTF(( "WiFi AP password is not configured.\n" ));
                xWifiStatus = eWiFiFailure;
            }
        }
        xNetworkParams.ucChannel = wificonfigACCESS_POINT_CHANNEL;
        while( WIFI_ConfigureAP( &xNetworkParams ) != eWiFiSuccess )
        {
            configPRINTF( ( "Connect to SoftAP %s using password %s and configure Wi-Fi. \r\n",
                            xNetworkParams.ucSSID, xNetworkParams.xPassword.xWPA.cPassphrase ) );
        }
        xWifiStatus = WIFI_StartAP();
        if (xWifiStatus != eWiFiSuccess)
        {
            configPRINTF( ( "SoftAP Start failed \r\n") );
        }
    }

    return xWifiStatus;
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
        configPRINT( "." );
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
    const uint32_t ulLongSleep = 1000UL;
    volatile uint32_t ulBlockVariable = 0UL;
    volatile char * pcFileName = (volatile char *)pcFile;
    volatile uint32_t ulLineNumber = ulLine;

    (void)pcFileName;
    (void)ulLineNumber;

    configPRINTF( ("vAssertCalled %s, %ld\n", pcFile, (long)ulLine) );

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

#if BLE_SUPPORTED

    int32_t xPortGetUserInput( uint8_t * pMessage,
                           uint32_t messageLength,
                           TickType_t timeoutTicks )
    {
        TickType_t xTimeOnEntering;
        uint8_t *ptr;
        uint32_t numBytes = 0;
        uint8_t bytesRead = 0;
        cy_rslt_t result = CY_RSLT_SUCCESS;

        /* ptr points to the memory location where the next character is to be stored. */
        ptr = pMessage;

        /* Store the current tick value to implement a timeout. */
        xTimeOnEntering = xTaskGetTickCount();

        while( ( bytesRead < messageLength ) &&
               ( (xTaskGetTickCount() - xTimeOnEntering) < timeoutTicks ) )
        {
            /* Check for data in the UART buffer with zero timeout. */
            numBytes = cyhal_uart_readable(&cy_retarget_io_uart_obj);
            if (numBytes > 0)
            {
                /* Get a single character from UART buffer. */
                result = cyhal_uart_getc(&cy_retarget_io_uart_obj, ptr, 0);
                if (CY_RSLT_SUCCESS != result)
                {
                    bytesRead = -1;
                    break;
                }
                else
                {
                    ptr++;
                    bytesRead++;
                }
            }
            /* Yield to other tasks while waiting for user data. */
            vTaskDelay( DELAY_BETWEEN_GETC_IN_TICKS );
        }

        return bytesRead;
    }

#endif /* if BLE_ENABLED */
