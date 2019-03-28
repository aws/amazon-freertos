/*
 * Amazon FreeRTOS V1.4.7
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
#ifdef IOT_CONFIG_FILE
    #include IOT_CONFIG_FILE
#endif
/* FreeRTOS includes. */

#include "FreeRTOS.h"
#include "task.h"

/* Demo includes */
#include "aws_demo_runner.h"
#include "aws_dev_mode_key_provisioning.h"

/* AWS System includes. */
#include "bt_hal_manager.h"
#include "aws_system_init.h"
#include "aws_logging_task.h"
#include "aws_wifi.h"
#include "aws_clientcredential.h"
#include "nvs_flash.h"
#include "FreeRTOS_IP.h"
#include "FreeRTOS_Sockets.h"

#include "esp_system.h"
#include "esp_wifi.h"
#include "esp_interface.h"
#include "esp_gap_ble_api.h"
#include "esp_bt.h"
#include "esp_bt_main.h"
#include "bt_hal_manager_adapter_ble.h"
#include "bt_hal_manager.h"
#include "bt_hal_gatt_server.h"
#include "semphr.h"
#include "driver/uart.h"
#include "aws_application_version.h"
#include "iot_network_manager_private.h"


#if BLE_ENABLED
#include "iot_ble.h"
#include "iot_ble_config.h"
#include "iot_ble_wifi_provisioning.h"
#include <iot_ble_numericComparison.h>
#endif

/* Logging Task Defines. */
#define mainLOGGING_MESSAGE_QUEUE_LENGTH    ( 32 )
#define mainLOGGING_TASK_STACK_SIZE         ( configMINIMAL_STACK_SIZE * 4 )
#define mainDEVICE_NICK_NAME                "Espressif_Demo"

/* Declare the firmware version structure for all to see. */
const AppVersion32_t xAppFirmwareVersion = {
   .u.x.ucMajor = APP_VERSION_MAJOR,
   .u.x.ucMinor = APP_VERSION_MINOR,
   .u.x.usBuild = APP_VERSION_BUILD,
};

QueueHandle_t spp_uart_queue = NULL;

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
 * @brief Application task startup hook.
 */
void vApplicationDaemonTaskStartupHook( void );

/**
 * @brief Initializes the board.
 */
static void prvMiscInitialization( void );

#if BLE_ENABLED
/* Initializes bluetooth */
static esp_err_t prvBLEStackInit( void );

static void spp_uart_init(void);

#endif
/*-----------------------------------------------------------*/

/**
 * @brief Application runtime entry point.
 */
int app_main( void )
{
	uint32_t ulEnabledNetworks;
	 /* Perform any hardware initialization that does not require the RTOS to be
	     * running.  */

	/* Create tasks that are not dependent on the WiFi being initialized. */
    xLoggingTaskInitialize( mainLOGGING_TASK_STACK_SIZE,
							tskIDLE_PRIORITY+5,
							mainLOGGING_MESSAGE_QUEUE_LENGTH );
    FreeRTOS_IPInit( ucIPAddress,
            ucNetMask,
            ucGatewayAddress,
            ucDNSServerAddress,
            ucMACAddress );

    prvMiscInitialization();

    if( SYSTEM_Init() == pdPASS )
    {

        /* A simple example to demonstrate key and certificate provisioning in
        * microcontroller flash using PKCS#11 interface. This should be replaced
        * by production ready key provisioning mechanism. */
        vDevModeKeyProvisioning();

#if BLE_ENABLED
        /* Initialize BLE. */
        if( prvBLEStackInit() != eBTStatusSuccess )
        {
        	configPRINTF(("Failed to initialize the bluetooth stack\n "));
        	while( 1 )
        	{

        	}
        }
#endif
        if( AwsIotNetworkManager_Init() != pdPASS )
        {
        	configPRINTF(("Failed to initialize the network manager \n "));
        	while( 1 )
        	{

        	}
        }

        ulEnabledNetworks = AwsIotNetworkManager_EnableNetwork( configENABLED_NETWORKS );
        if( ( ulEnabledNetworks & configENABLED_NETWORKS ) !=  configENABLED_NETWORKS )
        {
        	configPRINTF(("Failed to enable all the networks, enabled networks: %08x\n ", ulEnabledNetworks ));
        	while( 1 )
        	{

        	}
        }       
        /* Run all demos. */
        DEMO_RUNNER_RunDemos();
    }

    /* Start the scheduler.  Initialization that requires the OS to be running,
     * including the WiFi initialization, is performed in the RTOS daemon task
     * startup hook. */
    // Following is taken care by initialization code in ESP IDF
    // vTaskStartScheduler();
    return 0;
}

/*-----------------------------------------------------------*/

static void prvMiscInitialization( void )
{


	// Initialize NVS
	esp_err_t ret = nvs_flash_init();
	if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
		ESP_ERROR_CHECK(nvs_flash_erase());
		ret = nvs_flash_init();
	}
	ESP_ERROR_CHECK( ret );

   #if BLE_ENABLED
   NumericComparisonInit();
   spp_uart_init();
   #endif
}



/*-----------------------------------------------------------*/

void vApplicationDaemonTaskStartupHook( void )
{
}
/*-----------------------------------------------------------*/

#if BLE_ENABLED
static esp_err_t prvBLEStackInit( void )
{
    /* Initialize BLE */
    esp_err_t xRet = ESP_OK;
    esp_bt_controller_config_t xBtCfg = BT_CONTROLLER_INIT_CONFIG_DEFAULT();


    ESP_ERROR_CHECK( esp_bt_controller_mem_release( ESP_BT_MODE_CLASSIC_BT ) );

    xRet = esp_bt_controller_init( &xBtCfg );

    if( xRet == ESP_OK )
    {
        xRet = esp_bt_controller_enable( ESP_BT_MODE_BLE );
    }
    else
    {
        configPRINTF( ( "Failed to initialize bt controller, err = %d", xRet ) );
    }

    if( xRet == ESP_OK )
    {
        xRet = esp_bluedroid_init();
    }
    else
    {
        configPRINTF( ( "Failed to initialize bluedroid stack, err = %d", xRet ) );
    }

    if( xRet == ESP_OK )
    {
        xRet = esp_bluedroid_enable();
    }


    return xRet;
}


#endif

/*-----------------------------------------------------------*/
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
    static StackType_t uxIdleTaskStack[ configIDLE_TASK_STACK_SIZE ];

    /* Pass out a pointer to the StaticTask_t structure in which the Idle
     * task's state will be stored. */
    *ppxIdleTaskTCBBuffer = &xIdleTaskTCB;

    /* Pass out the array that will be used as the Idle task's stack. */
    *ppxIdleTaskStackBuffer = uxIdleTaskStack;

    /* Pass out the size of the array pointed to by *ppxIdleTaskStackBuffer.
     * Note that, as the array is necessarily of type StackType_t,
     * configMINIMAL_STACK_SIZE is specified in words, not bytes. */
    *pulIdleTaskStackSize = configIDLE_TASK_STACK_SIZE;
}
/*-----------------------------------------------------------*/

/* configUSE_STATIC_ALLOCATION is set to 1, so the application must provide an
 * implementation of vApplicationGetTimerTaskMemory() to provide the memory that is
 * used by the RTOS daemon/time task. */
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

    /* Loop forever */
    for( ; ; )
    {
    }
}
/*-----------------------------------------------------------*/
extern void esp_vApplicationTickHook();
void IRAM_ATTR vApplicationTickHook()
{
    esp_vApplicationTickHook();
}

/*-----------------------------------------------------------*/
extern void esp_vApplicationIdleHook();
void vApplicationIdleHook()
{
    esp_vApplicationIdleHook();
}

/*-----------------------------------------------------------*/
void vApplicationIPNetworkEventHook( eIPCallbackEvent_t eNetworkEvent )
{
    uint32_t ulIPAddress, ulNetMask, ulGatewayAddress, ulDNSServerAddress;
    system_event_t evt;

    if (eNetworkEvent == eNetworkUp) {
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
        esp_event_send(&evt);
    }
}
#if BLE_ENABLED
static void spp_uart_init(void)
{
    uart_config_t uart_config = {
        .baud_rate = 115200,
        .data_bits = UART_DATA_8_BITS,
        .parity = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_RTS,
        .rx_flow_ctrl_thresh = 122,
    };

    /* Set UART parameters */
    uart_param_config(UART_NUM_0, &uart_config);
    //Set UART pins
    uart_set_pin(UART_NUM_0, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE);
    //Install UART driver, and get the queue.
    uart_driver_install(UART_NUM_0, 4096, 8192, 10,&spp_uart_queue,0);
}

BaseType_t getUserMessage( INPUTMessage_t * pxINPUTmessage, TickType_t xAuthTimeout )
{
	uart_event_t xEvent;
	BaseType_t xReturnMessage = pdFALSE;

	if (xQueueReceive(spp_uart_queue, (void * )&xEvent, (portTickType) xAuthTimeout )) {
		switch (xEvent.type) {
		//Event of UART receiving data
		case UART_DATA:
			if (xEvent.size) {
				pxINPUTmessage->pcData = (uint8_t *)malloc(sizeof(uint8_t)*xEvent.size);
				if(pxINPUTmessage->pcData != NULL){
					memset(pxINPUTmessage->pcData,0x0,xEvent.size);
					uart_read_bytes(UART_NUM_0, (uint8_t *)pxINPUTmessage->pcData,xEvent.size,portMAX_DELAY);
					xReturnMessage = pdTRUE;
				}else
				{
					configPRINTF(("Malloc failed in main.c\n"));
				}
			}
			break;
		default:
			break;
		}
	}

	return xReturnMessage;
}
#endif
