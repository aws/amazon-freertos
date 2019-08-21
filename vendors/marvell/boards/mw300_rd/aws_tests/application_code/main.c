/*
 * Amazon FreeRTOS V1.4.2
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

/* FreeRTOS includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "FreeRTOS_IP.h" /* FIX ME: Delete if you are not using the FreeRTOS-Plus-TCP library. */

/* Marvell Includes */
#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <lowlevel_drivers.h>
#include <board.h>
#include <mw300_uart.h>
#include "psm-v2.h"
#include "partition.h"
#include "wifi.h"

/* Test includes */
#include "aws_test_runner.h"

/* AWS library includes. */
#include "iot_system_init.h"
#include "iot_logging_task.h"
#include "iot_wifi.h"
#include "aws_clientcredential.h"
#include "aws_application_version.h"
#include "aws_dev_mode_key_provisioning.h"

/* Declare the firmware version structure for all to see.
const AppVersion32_t xAppFirmwareVersion = {
	.u.x.ucMajor = APP_VERSION_MAJOR,
	.u.x.ucMinor = APP_VERSION_MINOR,
	.u.x.usBuild = APP_VERSION_BUILD,
};*/

/* Logging Task Defines. */
#define mainLOGGING_MESSAGE_QUEUE_LENGTH    ( 15 )
#define mainLOGGING_TASK_STACK_SIZE         ( configMINIMAL_STACK_SIZE * 8 )

/* Unit test defines. */
#define mainTEST_RUNNER_TASK_STACK_SIZE     ( configMINIMAL_STACK_SIZE * 16 )
/* Startup defines. */
#define mainSTARTUP_TASK_STACK_SIZE     ( configMINIMAL_STACK_SIZE * 4 )

/* The task delay for allowing the lower priority logging task to print out Wi-Fi
 * failure status before blocking indefinitely. */
#define mainLOGGING_WIFI_STATUS_DELAY       pdMS_TO_TICKS( 1000 )

/* The name of the devices for xApplicationDNSQueryHook. */
#define mainDEVICE_NICK_NAME				"mw300_rd" /* FIX ME.*/

/* Marvell Specific definitions */
#define OTA_SELF_TEST_TIMEOUT 50000
os_timer_t ota_timer;
static char ll_msg_buf_[MAX_MSG_LEN] = "Hello_world";
psm_hnd_t psm_hnd;
flash_desc_t fl;

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


/**
 * wm_printf should be used to print debug messages in isr and os debug prints
 *
 * @param format
 *
 * @return uint32_t
 * */

void init_uart()
{
    UART_CFG_Type uartCfg = {115200,
	UART_DATABITS_8,
	UART_PARITY_NONE,
	UART_STOPBITS_1,
	DISABLE};

    board_uart_pin_config(UART0_ID);
    CLK_SetUARTClkSrc(CLK_UART_ID_0, CLK_UART_SLOW);
    CLK_ModuleClkEnable(CLK_UART0);
    UART_Disable(UART0_ID);
    UART_Enable(UART0_ID);
    UART_Init(UART0_ID, &uartCfg);
}

void deinit_uart()
{
    UART_Disable(UART0_ID);
    CLK_ModuleClkDisable(CLK_UART0);
}

void wm_flush()
{
	while (UART_GetLineStatus(UART0_ID, UART_LINESTATUS_TEMT)
	       != SET)
		;
}

void wm_putchar(int ch_)
{
    UART_SendData(UART0_ID, ch_);
    wm_flush();
}

void wm_printf(const char *format, ...)
{
    wm_putchar('\n');
    wm_putchar('\r');
    snprintf(ll_msg_buf_, sizeof(ll_msg_buf_),
		&format[0]);
    UART_WriteLine(UART0_ID, (uint8_t *) ll_msg_buf_);
}
/**
 * @brief Application task startup hook for applications using Wi-Fi. If you are not
 * using Wi-Fi, then start network dependent applications in the vApplicationIPNetorkEventHook
 * function. If you are not using Wi-Fi, this hook can be disabled by setting
 * configUSE_DAEMON_TASK_STARTUP_HOOK to 0.
 */
//void vApplicationDaemonTaskStartupHook( void );
void vStartupHook( void *pvParameters);

/**
 * @brief Application IP network event hook called by the FreeRTOS+TCP stack for
 * applications using Ethernet. If you are not using Ethernet and the FreeRTOS+TCP stack,
 * start network dependent applications in vApplicationDaemonTaskStartupHook after the
 * network status is up.
 */
void vApplicationIPNetworkEventHook( eIPCallbackEvent_t eNetworkEvent );

/**
 * @brief Connects to Wi-Fi.
 */
static void prvWifiConnect( void );

/**
 * @brief Initializes the board.
 */
static void prvMiscInitialization( void );

/*-----------------------------------------------------------*/

/**
 * @brief Application runtime entry point.
 */
int main( void )
{
    /* Perform any hardware initialization that does not require the RTOS to be
     * running.  */
    prvMiscInitialization();

    /* Define ENABLE_LOGGING in FreeRTOSConfig.h to get a:FreeRTOS logs */
#ifdef ENABLE_LOGGING
    /* Create tasks that are not dependent on the Wi-Fi being initialized. */
    xLoggingTaskInitialize( mainLOGGING_TASK_STACK_SIZE,
                            tskIDLE_PRIORITY + 5,
                            mainLOGGING_MESSAGE_QUEUE_LENGTH );
#endif

    /* FIX ME: If you are using Ethernet network connections and the FreeRTOS+TCP stack,
     * uncomment the initialization function, FreeRTOS_IPInit(), below. */
    FreeRTOS_IPInit( ucIPAddress,
                     ucNetMask,
                     ucGatewayAddress,
                     ucDNSServerAddress,
                     ucMACAddress );

    /* Create the task to run unit tests. */
    xTaskCreate( vStartupHook,
                "Startup Hook",
		mainSTARTUP_TASK_STACK_SIZE,
                NULL,
                tskIDLE_PRIORITY + 2,
                NULL );

    /* Start the scheduler.  Initialization that requires the OS to be running,
     * including the Wi-Fi initialization, is performed in the RTOS daemon task
     * startup hook. */
    vTaskStartScheduler();

    return 0;
}

/*
 * This overrides os_init from WMSDK, we will not initialize scheduler and
 * just execute application in standalone environment.
 */
#include <wm_os.h>

int os_init()
{
    int ret;
    extern os_mutex_t biglock;

    /* Create a global mutex here, this is done to avoid inter dependency
     * between modules where they can't create or initialize mutex.
     */
    ret = os_mutex_create(&biglock, "big_lock", OS_MUTEX_INHERIT);
    if (ret != WM_SUCCESS)
        os_thread_self_complete(NULL);

    /* Add memory bank from SRAM1 if available */
    add_heap_2_bank();

    main();

    /* Nothing to do, wait forever */
    for (;;)
        ;
}
/*-----------------------------------------------------------*/

static void prvMiscInitialization( void )
{
    /* FIX ME: Perform any hardware initializations, that don't require the RTOS to be
     * running, here
     */
    int ret;
    init_uart(0, 0);

    /* Enable following to get log messages from Marvell */
#if 0
    wmstdio_init(UART0_ID, 0);
#endif

    /* Set the TCP/UDP Packet Send Retry Count to 10 */
    wifi_set_packet_retry_count(3);

    ret = part_get_desc_from_id(FC_COMP_PSM, &fl);
    if (ret != WM_SUCCESS) {
	configPRINT("Unable to get flash desc from id\r\n");
    }
    ret = psm_module_init(&fl, &psm_hnd, NULL);
    if (ret != 0) {
	configPRINT("Failed to initialize psm module\r\n");
    }

    configPRINT_STRING("FreeRTOS Started\r\n");
}
/*-----------------------------------------------------------*/

void vStartupHook( void *pvParameters)
{
    /* FIX ME: Perform any hardware initialization, that require the RTOS to be
     * running, here. */

    configPRINT("\r\nApplication Daemon Startup \r\n");

    /* A simple example to demonstrate key and certificate provisioning in
     * flash using PKCS#11 interface. This should be replaced
     * by production ready key provisioning mechanism. */
    vDevModeKeyProvisioning();

    if( SYSTEM_Init() == pdPASS ) {
        /* Connect to the Wi-Fi before running the tests. */
        prvWifiConnect();
    } else {
	configPRINT("\r\nSyatem Init failed \r\n");
    }
    xTaskCreate( TEST_RUNNER_RunTests_task,
                 "RunTests_task",
                 mainTEST_RUNNER_TASK_STACK_SIZE,
                 NULL,
                 tskIDLE_PRIORITY + 5,
                 NULL );

    vTaskDelete(NULL);
}
/*-----------------------------------------------------------*/
#include <FreeRTOS_Sockets.h>
#include <FreeRTOS_DHCP.h>

eDHCPCallbackAnswer_t xApplicationDHCPHook( eDHCPCallbackPhase_t eDHCPPhase,
                                            uint32_t ulIPAddress )
{
eDHCPCallbackAnswer_t eReturn;
uint32_t ulStaticIPAddress, ulStaticNetMask;

  /* This hook is called in a couple of places during the DHCP process, as
  identified by the eDHCPPhase parameter. */
  switch( eDHCPPhase )
  {
    case eDHCPPhasePreDiscover  :
      /* A DHCP discovery is about to be sent out.  eDHCPContinue is
      returned to allow the discovery to go out.

      If eDHCPUseDefaults had been returned instead then the DHCP process
      would be stopped and the statically configured IP address would be
      used.

      If eDHCPStopNoChanges had been returned instead then the DHCP
      process would be stopped and whatever the current network
      configuration was would continue to be used. */
      eReturn = eDHCPContinue;
      break;

    case eDHCPPhasePreRequest  :
      /* An offer has been received from the DHCP server, and the offered
      IP address is passed in the ulIPAddress parameter.  Convert the
      offered and statically allocated IP addresses to 32-bit values. */
      ulStaticIPAddress = FreeRTOS_inet_addr_quick( configIP_ADDR0,
                                                    configIP_ADDR1,
                                                    configIP_ADDR2,
                                                    configIP_ADDR3 );

      ulStaticNetMask = FreeRTOS_inet_addr_quick( configNET_MASK0,
                                                  configNET_MASK1,
                                                  configNET_MASK2,
                                                  configNET_MASK3 );

      /* Mask the IP addresses to leave just the sub-domain octets. */
      ulStaticIPAddress &= ulStaticNetMask;
      ulIPAddress &= ulStaticNetMask;

      /* Are the sub-domains the same? */
      if( ulStaticIPAddress == ulIPAddress ) {
        /* The sub-domains match, so the default IP address can be
        used.  The DHCP process is stopped at this point. */
        eReturn = eDHCPUseDefaults;
      }
      else {
        /* The sub-domains don't match, so continue with the DHCP
        process so the offered IP address is used. */
        eReturn = eDHCPContinue;
      }

      break;

    default :
      /* Cannot be reached, but set eReturn to prevent compiler warnings
      where compilers are disposed to generating one. */
      eReturn = eDHCPContinue;
      break;
  }

  return eReturn;
}
/*-----------------------------------------------------------*/

void vApplicationIPNetworkEventHook( eIPCallbackEvent_t eNetworkEvent )
{
    /* FIX ME: If your application is using Ethernet network connections and the
     * FreeRTOS+TCP stack, delete the surrounding compiler directives to enable the
     * unit tests and after MQTT, Bufferpool, and Secure Sockets libraries have been
     * imported into the project. If you are not using Ethernet see the
     * vApplicationDaemonTaskStartupHook function. */
    if (eNetworkEvent == eNetworkUp) {
        configPRINT("\r\nNetwork connection successful.\r\n");
    }
#if 0
    static BaseType_t xTasksAlreadyCreated = pdFALSE;

    /* If the network has just come up...*/
    if( eNetworkEvent == eNetworkUp )
    {
        if( ( xTasksAlreadyCreated == pdFALSE ) && ( SYSTEM_Init() == pdPASS ) )
        {
            DEMO_RUNNER_RunDemos();
            xTasksAlreadyCreated = pdTRUE;
        }
    }
#endif /* if 0 */
}
/*-----------------------------------------------------------*/

void prvWifiConnect( void )
{

    WIFINetworkParams_t xNetworkParams;
    WIFIReturnCode_t xWifiStatus;
    uint8_t ucTempIp[4] = { 0 };

    configPRINT("\r\nWill attempt to start wlan \r\n");
    xWifiStatus = WIFI_On();

    if( xWifiStatus == eWiFiSuccess ) {
        configPRINT( "\r\nWi-Fi module initialized. Connecting to AP...\r\n" );
    } else {
        configPRINT( "\r\nWi-Fi module failed to initialize.\r\n");

        /* Delay to allow the lower priority logging task to print the above status.
         * The while loop below will block the above printing. */
        vTaskDelay( mainLOGGING_WIFI_STATUS_DELAY );

        while( 1 ) {
        }
    }

    /* Setup parameters. */
    xNetworkParams.pcSSID = clientcredentialWIFI_SSID;
    xNetworkParams.ucSSIDLength = sizeof( clientcredentialWIFI_SSID );
    xNetworkParams.pcPassword = clientcredentialWIFI_PASSWORD;
    xNetworkParams.ucPasswordLength = sizeof( clientcredentialWIFI_PASSWORD );
    xNetworkParams.xSecurity = clientcredentialWIFI_SECURITY;
    xNetworkParams.cChannel = 0;

    xWifiStatus = WIFI_ConnectAP(&xNetworkParams);
    if( xWifiStatus == eWiFiSuccess ) {
        configPRINT( "\r\nWi-Fi Connected to AP. Creating tasks which use network...\r\n" );
        xWifiStatus = WIFI_GetIP( ucTempIp );
        if ( eWiFiSuccess == xWifiStatus ) {
            wmprintf( "IP Address acquired %d.%d.%d.%d\r\n",
        		    ucTempIp[ 0 ], ucTempIp[ 1 ], ucTempIp[ 2 ], ucTempIp[ 3 ] );
        }
    } else {
        /* Connection failed, configure SoftAP. */
        configPRINTF( ( "Wi-Fi failed to connect to AP %s.\r\n", xNetworkParams.pcSSID ) );

        xNetworkParams.pcSSID = wificonfigACCESS_POINT_SSID_PREFIX;
        xNetworkParams.pcPassword = wificonfigACCESS_POINT_PASSKEY;
        xNetworkParams.xSecurity = wificonfigACCESS_POINT_SECURITY;
        xNetworkParams.cChannel = wificonfigACCESS_POINT_CHANNEL;

        configPRINTF( ( "Connect to SoftAP %s using password %s. \r\n",
                            xNetworkParams.pcSSID, xNetworkParams.pcPassword ) );

        while( WIFI_ConfigureAP( &xNetworkParams ) != eWiFiSuccess ) {
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
