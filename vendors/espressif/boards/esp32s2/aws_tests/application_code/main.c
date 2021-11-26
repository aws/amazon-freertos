/*
 * FreeRTOS V1.1.4
 * Copyright (C) 2020 Amazon.com, Inc. or its affiliates.  All Rights Reserved.
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
#include "string.h"

/* Test includes */
#include "aws_test_runner.h"

#include "iot_uart.h"
#include "driver/uart.h"

/* AWS library includes. */
#include "iot_system_init.h"
#include "iot_logging_task.h"
#include "iot_wifi.h"
#include "aws_clientcredential.h"
#include "aws_dev_mode_key_provisioning.h"
#include "nvs_flash.h"
#if !AFR_ESP_LWIP
#include "FreeRTOS_IP.h"
#include "FreeRTOS_Sockets.h"
#endif
#include "esp_netif.h"
#include "aws_test_utils.h"
#include "esp_system.h"
#include "esp_wifi.h"
#include "esp_interface.h"
/* Logging Task Defines. */
#define mainLOGGING_MESSAGE_QUEUE_LENGTH    ( 32 )
#define mainLOGGING_TASK_STACK_SIZE         ( configMINIMAL_STACK_SIZE * 6 )

/* Unit test defines. */
#define mainTEST_RUNNER_TASK_STACK_SIZE     ( configMINIMAL_STACK_SIZE * 12 )
#define mainDEVICE_NICK_NAME                "Espressif_Demo"


/* Static arrays for FreeRTOS+TCP stack initialization for Ethernet network connections
 * are use are below. If you are using an Ethernet connection on your MCU device it is
 * recommended to use the FreeRTOS+TCP stack. The default values are defined in
 * FreeRTOSConfig.h. */

/* Default MAC address configuration.  The demo creates a virtual network
 * connection that uses this MAC address by accessing the raw Ethernet data
 * to and from a real network connection on the host PC.  See the
 * configNETWORK_INTERFACE_TO_USE definition for information on how to configure
 * the real network connection to use. */
uint8_t ucMACAddress[ 6 ] =
{
    configMAC_ADDR0,
    configMAC_ADDR1,
    configMAC_ADDR2,
    configMAC_ADDR3,
    configMAC_ADDR4,
    configMAC_ADDR5
};

/* The default IP and MAC address used by the demo.  The address configuration
 * defined here will be used if ipconfigUSE_DHCP is 0, or if ipconfigUSE_DHCP is
 * 1 but a DHCP server could not be contacted.  See the online documentation for
 * more information.  In both cases the node can be discovered using
 * "ping RTOSDemo". */
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
 * @brief Application task startup hook for applications using Wi-Fi. If you are not
 * using Wi-Fi, then start network dependent applications in the vApplicationIPNetorkEventHook
 * function. If you are not using Wi-Fi, this hook can be disabled by setting
 * configUSE_DAEMON_TASK_STARTUP_HOOK to 0.
 */
void vApplicationDaemonTaskStartupHook( void );

/**
 * @brief Connects to WiFi.
 */
static void prvWifiConnect( void );

/**
 * @brief Initializes the board.
 */
static void prvMiscInitialization( void );

/*-----------------------------------------------------------*/

IotUARTHandle_t xConsoleUart;

static void iot_uart_init( void )
{
    IotUARTConfig_t xUartConfig;
    int32_t status = IOT_UART_SUCCESS;

    xConsoleUart = iot_uart_open( UART_NUM_0 );
    configASSERT( xConsoleUart );

    status = iot_uart_ioctl( xConsoleUart, eUartGetConfig, &xUartConfig );
    configASSERT( status == IOT_UART_SUCCESS );

    xUartConfig.ulBaudrate = 115200;
    xUartConfig.xParity = eUartParityNone;
    xUartConfig.xStopbits = eUartStopBitsOne;
/*
 * Application does not boot if flow control is enabled on
 * the same UART port as that of IDF console. Hence, disable
 * flow control if IDF console is set to UART 0.
 */
#if (CONFIG_ESP_CONSOLE_UART_NUM != 0)
        xUartConfig.ucFlowControl = true;
#endif

    status = iot_uart_ioctl( xConsoleUart, eUartSetConfig, &xUartConfig );
    configASSERT( status == IOT_UART_SUCCESS );
}

/**
 * @brief Application runtime entry point.
 */
int app_main( void )
{
    /* Perform any hardware initialization that does not require the RTOS to be
     * running.  */
    prvMiscInitialization();

    /* Create tasks that are not dependent on the WiFi being initialized. */
    xLoggingTaskInitialize( mainLOGGING_TASK_STACK_SIZE,
                            tskIDLE_PRIORITY + 5,
                            mainLOGGING_MESSAGE_QUEUE_LENGTH );

#if AFR_ESP_LWIP
    configPRINTF( ("Initializing lwIP TCP stack\r\n") );
    esp_netif_init();
#else /* AFR_ESP_LWIP */
    configPRINTF( ("Initializing FreeRTOS TCP stack\r\n") );
    FreeRTOS_IPInit( ucIPAddress,
                     ucNetMask,
                     ucGatewayAddress,
                     ucDNSServerAddress,
                     ucMACAddress );
#endif /* !AFR_ESP_LWIP */

    if( SYSTEM_Init() == pdPASS )
    {
        /* Connect to the wifi before running the tests. */
        prvWifiConnect();

        /* A simple example to demonstrate key and certificate provisioning in
         * microcontroller flash using PKCS#11 interface. This should be replaced
         * by production ready key provisioning mechanism. */
        vDevModeKeyProvisioning();

        /* Create the task to run unit tests. */
        xTaskCreate( TEST_RUNNER_RunTests_task,
                     "RunTests_task",
                     mainTEST_RUNNER_TASK_STACK_SIZE,
                     NULL,
                     tskIDLE_PRIORITY + 5,
                     NULL );
    }

    /* Start the scheduler.  Initialization that requires the OS to be running,
     * including the WiFi initialization, is performed in the RTOS daemon task
     * startup hook. */
    /* Following is taken care by initialization code in ESP IDF */
    /* vTaskStartScheduler(); */

    return 0;
}
/*-----------------------------------------------------------*/

static void prvMiscInitialization( void )
{
    /* Initialize UART. */
    iot_uart_init();

    /* Initialize NVS */
    esp_err_t ret = nvs_flash_init();

    if( ( ret == ESP_ERR_NVS_NO_FREE_PAGES ) || ( ret == ESP_ERR_NVS_NEW_VERSION_FOUND ) )
    {
        ESP_ERROR_CHECK( nvs_flash_erase() );
        ret = nvs_flash_init();
    }

    ESP_ERROR_CHECK( ret );
}
/*-----------------------------------------------------------*/

void vApplicationDaemonTaskStartupHook( void )
{
}
/*-----------------------------------------------------------*/

void prvWifiConnect( void )
{
    WIFINetworkParams_t xJoinAPParams;
    WIFIReturnCode_t eWiFiStatus;
    uint32_t ulInitialRetryPeriodMs = 500;
    BaseType_t xMaxRetries = 6;
    size_t xSSIDLength = 0, xPasswordLength = 0;
    const char *pcSSID = ( const char * ) clientcredentialWIFI_SSID;
    const char *pcPassword = ( const char * ) clientcredentialWIFI_PASSWORD;

    eWiFiStatus = WIFI_On();

    if( eWiFiStatus == eWiFiSuccess )
    {
        configPRINTF( ( "WiFi module initialized. Connecting to AP %s\r\n", clientcredentialWIFI_SSID ) );
    }
    else
    {
        configPRINTF( ( "WiFi module failed to initialize.\r\n" ) );

        while( 1 )
        {
        }
    }


    /* Setup parameters. */
    if( ( pcSSID == NULL ) || ( strcmp( pcSSID, "") == 0 ) )
    {
        configPRINTF(( "[Error] WiFi SSID is not configured (either null or empty).\r\n" ));
        while( 1 )
        {
        }
    }
    else
    {
        xSSIDLength = strlen( pcSSID );

        if( xSSIDLength > sizeof( xJoinAPParams.ucSSID ) )
        {
            configPRINTF(( "[Error] WiFi SSID length exceeeds allowable size of %u bytes.", sizeof( xJoinAPParams.ucSSID ) ));
            while( 1 )
            {
            }
        } 
    }

    memcpy( xJoinAPParams.ucSSID, pcSSID, xSSIDLength );
    xJoinAPParams.ucSSIDLength = xSSIDLength;
    xJoinAPParams.xSecurity = clientcredentialWIFI_SECURITY;

    if ( ( xJoinAPParams.xSecurity == eWiFiSecurityWPA2 ) ||
         ( xJoinAPParams.xSecurity == eWiFiSecurityWPA ) )
    {
        if( pcPassword != NULL )
        {
            xPasswordLength = strlen( pcPassword );
            if( xPasswordLength > sizeof( xJoinAPParams.xPassword.xWPA.cPassphrase ) )
            {
                configPRINTF(( "[Error] WiFi password exceeds allowable size of %u bytes.\r\n", sizeof( xJoinAPParams.xPassword.xWPA.cPassphrase ) ));
                while( 1 )
                {
                }
            }
            memcpy( xJoinAPParams.xPassword.xWPA.cPassphrase, pcPassword, xPasswordLength );
            xJoinAPParams.xPassword.xWPA.ucLength = xPasswordLength;
        }
        else
        {
            configPRINTF(( "[Error] WiFi security is configured as WPA2 but password is not provided.\r\n" ));
            while( 1 )
            {
            }
        }
        
    }

    RETRY_EXPONENTIAL( eWiFiStatus = WIFI_ConnectAP( &( xJoinAPParams ) ),
                       eWiFiSuccess, ulInitialRetryPeriodMs, xMaxRetries );

    if( eWiFiStatus == eWiFiSuccess )
    {
        configPRINTF( ( "WiFi Connected to AP. Creating tasks which use network...\r\n" ) );
    }
    else
    {
        configPRINTF( ( "WiFi failed to connect to AP %s.\r\n", clientcredentialWIFI_SSID ) );

        while( 1 )
        {
        }
    }
}
/*-----------------------------------------------------------*/

#if ( ipconfigUSE_LLMNR != 0 ) || ( ipconfigUSE_NBNS != 0 ) || ( ipconfigDHCP_REGISTER_HOSTNAME == 1 )

    const char * pcApplicationHostnameHook( void )
    {
        /* This function will be called during the DHCP: the machine will be registered
         * with an IP address plus this name. */
        return clientcredentialIOT_THING_NAME;
    }

#endif
/*-----------------------------------------------------------*/

#if ( ipconfigUSE_LLMNR != 0 ) || ( ipconfigUSE_NBNS != 0 )

    BaseType_t xApplicationDNSQueryHook( const char * pcName )
    {
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

#if !AFR_ESP_LWIP
/*-----------------------------------------------------------*/
void vApplicationIPNetworkEventHook( eIPCallbackEvent_t eNetworkEvent )
{
    uint32_t ulIPAddress, ulNetMask, ulGatewayAddress, ulDNSServerAddress;
    system_event_t evt;

    if( eNetworkEvent == eNetworkUp )
    {
        /* Print out the network configuration, which may have come from a DHCP
         * server. */
        FreeRTOS_GetAddressConfiguration(
            &ulIPAddress,
            &ulNetMask,
            &ulGatewayAddress,
            &ulDNSServerAddress );

        evt.event_id = SYSTEM_EVENT_STA_GOT_IP;
        evt.event_info.got_ip.ip_changed = true;
        evt.event_info.got_ip.ip_info.ip.addr = ulIPAddress;
        evt.event_info.got_ip.ip_info.netmask.addr = ulNetMask;
        evt.event_info.got_ip.ip_info.gw.addr = ulGatewayAddress;
        esp_event_send( &evt );
    }
}
#endif /* !AFR_ESP_LWIP */

