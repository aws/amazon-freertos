/*
 * Amazon FreeRTOS V1.2.7
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
#include "task.h"

/* Test includes */
#include "aws_test_runner.h"

/* Demo includes */
#include "aws_demo_runner.h"

/* AWS library includes. */
#include "aws_system_init.h"
#include "aws_logging_task.h"
#include "aws_wifi.h"
#include "aws_clientcredential.h"
#include "aws_demo_config.h"

/* AWS BLE headers */
#include "aws_ble.h"
#include "bt_hal_manager.h"
#include "bt_hal_manager_adapter_ble.h"

/* Nordic BSP includes */
#include "nordic_common.h"
#include "nrf.h"
#include "app_error.h"
#include "ble.h"
#include "ble_hci.h"
#include "ble_srv_common.h"
#include "ble_advdata.h"
#include "ble_advertising.h"
#include "ble_bas.h"
#include "ble_hrs.h"
#include "ble_dis.h"
#include "nrf_ble_qwr.h"
#include "sensorsim.h"
#include "nrf_sdh.h"
#include "nrf_sdh_soc.h"
#include "nrf_sdh_ble.h"
#include "nrf_sdh_freertos.h"
#include "app_timer.h"
#include "peer_manager.h"
#include "peer_manager_handler.h"
#include "bsp.h"
#include "bsp_btn_ble.h"
#include "FreeRTOS.h"
#include "task.h"
#include "timers.h"
#include "semphr.h"
#include "fds.h"
#include "ble_conn_state.h"
#include "nrf_drv_clock.h"
#include "nrf_ble_gatt.h"
#include "nrf_ble_lesc.h"


#include "nrf_log.h"
#include "nrf_log_ctrl.h"
#include "nrf_log_default_backends.h"

/* clang-format off */

/* Logging Task Defines. */
#define mainLOGGING_MESSAGE_QUEUE_LENGTH    ( 15 )
#define mainLOGGING_TASK_STACK_SIZE         ( configMINIMAL_STACK_SIZE * 8 )

/* LED-associated defines */
#define LED_ONE                             BSP_LED_0_MASK
#define LED_TWO                             BSP_LED_1_MASK
#define LED_THREE                           BSP_LED_2_MASK
#define LED_FOUR                            BSP_LED_3_MASK
#define ALL_APP_LED                     \
    ( BSP_LED_0_MASK | BSP_LED_1_MASK | \
      BSP_LED_2_MASK | BSP_LED_3_MASK )             /**< Define used for simultaneous operation of all application LEDs. */
#define LED_BLINK_INTERVAL_MS               ( 300 ) /**< LED blinking interval. */


/* The task delay for allowing the lower priority logging task to print out Wi-Fi
 * failure status before blocking indefinitely. */
#define mainLOGGING_WIFI_STATUS_DELAY      pdMS_TO_TICKS( 1000 )

/* Unit test defines. */
#define mainTEST_RUNNER_TASK_STACK_SIZE    ( configMINIMAL_STACK_SIZE * 16 )

/* clang-format on */


static bool bRRIntervalEnabled = true; /**< Flag for enabling and disabling the registration of new RR interval measurements (the purpose of disabling this is just to test sending HRM without RR interval data. */


static BTInterface_t * pxBTinterface;

static bool erase_bonds;

static void prvAdvertisingStart( void * p_erase_bonds );

static void prvOpenCb( uint16_t usConnId,
                       BTStatus_t xStatus,
                       uint8_t ucAdapterIf,
                       BTBdaddr_t * pxBda )
{
    ret_code_t err_code;

    NRF_LOG_INFO( "Connected" );
    err_code = bsp_indication_set( BSP_INDICATE_CONNECTED );
    APP_ERROR_CHECK( err_code );
}

static BTBleAdapterCallbacks_t prvCallbacks =
{
    .pxOpenCb = prvOpenCb
};

/**
 * @brief Application task startup hook for applications using Wi-Fi. If you are not
 * using Wi-Fi, then start network dependent applications in the vApplicationIPNetorkEventHook
 * function. If you are not using Wi-Fi, this hook can be disabled by setting
 * configUSE_DAEMON_TASK_STARTUP_HOOK to 0.
 */
void vApplicationDaemonTaskStartupHook( void );

/**
 * @brief Initializes the board.
 */
static void prvMiscInitialization( bool * );

TaskHandle_t prvLedHandle;

/*-----------------------------------------------------------*/

/**
 * @brief Task for LED's blinking
 */
static void prvLedTask( void * parameters )
{
    ( void * ) parameters;

    LEDS_CONFIGURE( ALL_APP_LED );
    LEDS_OFF( ALL_APP_LED );

    for( ; ; )
    {
        LEDS_ON( LED_FOUR );
        vTaskDelay( LED_BLINK_INTERVAL_MS );
        LEDS_OFF( LED_FOUR );
        vTaskDelay( LED_BLINK_INTERVAL_MS );
    }
}

/*-----------------------------------------------------------*/

/**@brief Callback function for asserts in the SoftDevice.
 *
 * @details This function will be called in case of an assert in the SoftDevice.
 *
 * @warning This handler is an example only and does not fit a final product. You need to analyze
 *          how your product is supposed to react in case of Assert.
 * @warning On assert from the SoftDevice, the system can only recover on reset.
 *
 * @param[in]   line_num   Line number of the failing ASSERT call.
 * @param[in]   file_name  File name of the failing ASSERT call.
 */
void assert_nrf_callback( uint16_t line_num,
                          const uint8_t * p_file_name )
{
    app_error_handler( DEAD_BEEF, line_num, p_file_name );
}

/*-----------------------------------------------------------*/

/**@brief Function for putting the chip into sleep mode.
 *
 * @note This function will not return.
 */
static void prvSleepModeEnter( void )
{
    ret_code_t err_code;

    err_code = bsp_indication_set( BSP_INDICATE_IDLE );
    APP_ERROR_CHECK( err_code );

    /* Prepare wakeup buttons. */
    err_code = bsp_btn_ble_sleep_mode_prepare();
    APP_ERROR_CHECK( err_code );

    /* Go to system-off mode (this function will not return; wakeup will cause a reset). */
    err_code = sd_power_system_off();
    APP_ERROR_CHECK( err_code );
}


/*-----------------------------------------------------------*/

/**@brief Function for initializing the BLE stack.
 *
 * @details Initializes the SoftDevice and the BLE event interrupt.
 */
static BTStatus_t prvBLEInit( void )
{
    BTUuid_t xUUID =
    {
        .ucType   = eBTuuidType128,
        .uu.uu128 = { 0x00,        0x00,0x00, 0x00, 0x00, 0x00, 0x00, 0x00 }
    };

    const bool bIsBondable = false;
    const bool bIsSecure = true;

    const uint32_t usMtu = 23;

    BTProperty_t pxProperties[ 4 ] =
    {
        {
            .xType = eBTpropertyBdname,
            .xLen = strlen( DEVICE_NAME ),
            .pvVal = DEVICE_NAME
        },
        {
            .xType = eBTpropertyBondable,
            .xLen = 1,
            .pvVal = ( void * ) &bIsBondable
        },
        {
            .xType = eBTpropertyLocalMTUSize,
            .xLen = 1,
            .pvVal = ( void * ) &usMtu
        },
        {
            .xType = eBTpropertySecureConnectionOnly,
            .xLen = 1,
            .pvVal = ( void * ) &bIsSecure
        },
    };

    pxBTinterface = ( BTInterface_t * ) BTGetBluetoothInterface();
    BTStatus_t xStatus = eBTStatusSuccess;
    xStatus = BLE_Init( &xUUID, pxProperties, 4 );

    return xStatus;
}

/*-----------------------------------------------------------*/

/**@brief Function for handling events from the BSP module.
 *
 * @param[in]   event   Event generated by button press.
 */
static void prvBSPEventHAndler( bsp_event_t event )
{
    ret_code_t err_code;
    BTStatus_t xStatus;

    switch( event )
    {
        case BSP_EVENT_SLEEP:
            prvSleepModeEnter();
            break;

        case BSP_EVENT_DISCONNECT:
            break;

        case BSP_EVENT_WHITELIST_OFF:
            break;

        default:
            break;
    }
}


/*-----------------------------------------------------------*/

/**@brief Clear bond information from persistent storage. */
static void prvDeleteBonds( void )
{
    ret_code_t err_code;

    NRF_LOG_INFO( "Erase bonds!" );

    err_code = pm_peers_delete();
    APP_ERROR_CHECK( err_code );
}

/*-----------------------------------------------------------*/

void vSetAdvCallback( BTStatus_t xStatus )
{
}

/*-----------------------------------------------------------*/

/**@brief Function for initializing the Advertising functionality. */
static void prvAdvertisingInit( void )
{
    BTStatus_t xStatus;

    ble_advertising_init_t init;

    memset( &init, 0, sizeof( init ) );
    BTUuid_t xHeartRateServiceUUID =
    {
        .ucType  = eBTuuidType16,
        .uu.uu16 = BLE_UUID_HEART_RATE_SERVICE
    };

    BTUuid_t xBatteryServiceUUID =
    {
        .ucType  = eBTuuidType16,
        .uu.uu16 = BLE_UUID_BATTERY_SERVICE
    };

    BLEAdvertismentParams_t xAdvParams =
    {
        .bIncludeTxPower    = false,
        .bIncludeName       = true,
        .bSetScanRsp        = true,
        .ulAppearance       = 0,
        .ulMinInterval      = 0x20,
        .ulMaxInterval      = 0x40,
        .usManufacturerLen  = 0,
        .pcManufacturerData = NULL,
        .pxUUID1            = &xHeartRateServiceUUID,
        .pxUUID2            = &xBatteryServiceUUID
    };

    xStatus = BLE_SetAdvData( BTAdvInd, &xAdvParams, NULL );
    configASSERT( ( xStatus == eBTStatusSuccess ) );
}

/*-----------------------------------------------------------*/

/**@brief Function for initializing the nrf log module.
 */
static void prvLogInit( void )
{
    ret_code_t err_code = NRF_LOG_INIT( NULL );

    APP_ERROR_CHECK( err_code );

    NRF_LOG_DEFAULT_BACKENDS_INIT();
}

/*-----------------------------------------------------------*/

/**@brief Function for initializing buttons and leds.
 *
 * @param[out] p_erase_bonds  Will be true if the clear bonding button was pressed to wake the application up.
 */
static void prvButtonsLedsInit( bool * p_erase_bonds )
{
    ret_code_t err_code;
    bsp_event_t startup_event;

    err_code = bsp_init( BSP_INIT_LEDS | BSP_INIT_BUTTONS, prvBSPEventHAndler );
    APP_ERROR_CHECK( err_code );

    err_code = bsp_btn_ble_init( NULL, &startup_event );
    APP_ERROR_CHECK( err_code );

    *p_erase_bonds = ( startup_event == BSP_EVENT_CLEAR_BONDING_DATA );
}

/*-----------------------------------------------------------*/

/**@brief Function for starting advertising. */
static void prvAdvertisingStart( void * p_erase_bonds )
{
    BTStatus_t xStatus = BLE_StartAdv( NULL );

    configASSERT( ( xStatus == eBTStatusSuccess ) );
}

/*-----------------------------------------------------------*/

/**@brief A function which is hooked to idle task.
 * @note Idle hook must be enabled in FreeRTOS configuration (configUSE_IDLE_HOOK).
 */
void vApplicationIdleHook( void )
{
}

/*-----------------------------------------------------------*/

/**@brief Function for initializing the clock.
 */
static void prvClockInit( void )
{
    ret_code_t err_code = nrf_drv_clock_init();

    APP_ERROR_CHECK( err_code );
}

static void prvLescTask( void * params )
{
    for( ; ; )
    {
        nrf_ble_lesc_request_handler();
        vTaskDelay( 10 );
    }
}

/*-----------------------------------------------------------*/

/**@brief Function for application main entry.
 */
int main( void )
{
    /* Perform any hardware initialization that does not require the RTOS to be
     * running.  */
    prvMiscInitialization( &erase_bonds );

    /* Create a FreeRTOS task for the BLE stack.
     * The task will run advertising_start() before entering its loop. */

    xLoggingTaskInitialize( mainLOGGING_TASK_STACK_SIZE,
                            tskIDLE_PRIORITY + 2,
                            mainLOGGING_MESSAGE_QUEUE_LENGTH );


    nrf_sdh_freertos_init( NULL, NULL );

    xTaskCreate( prvLescTask, "LESK", 300, NULL, tskIDLE_PRIORITY + 3, NULL );

    BaseType_t xReturned = xTaskCreate( TEST_RUNNER_RunTests_task,
                                        "RunTests_task",
                                        mainTEST_RUNNER_TASK_STACK_SIZE,
                                        NULL,
                                        tskIDLE_PRIORITY + 1,
                                        NULL );

    if( xReturned != pdPASS )
    {
        NRF_LOG_ERROR( "Tests task not created." );
        APP_ERROR_HANDLER( NRF_ERROR_NO_MEM );
    }

    vTaskStartScheduler();

    for( ; ; )
    {
        /*    APP_ERROR_HANDLER(NRF_ERROR_FORBIDDEN);*/
    }
}

/*-----------------------------------------------------------*/

static void prvMiscInitialization( bool * erase_bonds )
{
    /* Initialize modules.*/
    prvLogInit();
    prvClockInit();
    /* Activate deep sleep mode. */
    SCB->SCR |= SCB_SCR_SLEEPDEEP_Msk;
    prvButtonsLedsInit( erase_bonds );

    /*    application_timers_start();  /* TODO: Find out what's wrong with timers!!*/
}
/*-----------------------------------------------------------*/

void vApplicationDaemonTaskStartupHook( void )
{
    /* FIX ME: Perform any hardware initialization, that require the RTOS to be
     * running, here. */

/*    NRF_LOG_INFO( "HRS FreeRTOS example started." ); */
    /* Start FreeRTOS scheduler. */

    #if 0
        if( SYSTEM_Init() == pdPASS )
        {
            /* Start the demo tasks. */
            DEMO_RUNNER_RunDemos();
        }
    #endif
}

/*-----------------------------------------------------------*/

/**
 * @brief This is to provide memory that is used by the Idle task.
 *
 * If configUSE_STATIC_ALLOCATION is set to 1, then the application must provide an
 * implementation of vApplicationGetIdleTaskMemory() in order to provide memory to
 * the Idle task.
 */
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
 * If configUSE_STATIC_ALLOCATION is set to 1, then application must provide an
 * implementation of vApplicationGetTimerTaskMemory() in order to provide memory
 * to the RTOS daemon/time task.
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
    taskDISABLE_INTERRUPTS();

    for( ; ; )
    {
    }
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
    portDISABLE_INTERRUPTS();

    /* Loop forever */
    for( ; ; )
    {
    }
}
/*-----------------------------------------------------------*/


/**
 * @brief User defined assertion call. This function is plugged into configASSERT.
 * See FreeRTOSConfig.h to define configASSERT to something different.
 */
void vAssertCalled( const char * pcFile,
                    uint32_t ulLine )
{
    /* FIX ME. If necessary, update to applicable assertion routine actions. */

    const uint32_t ulLongSleep = 1000UL;
    volatile uint32_t ulBlockVariable = 0UL;
    volatile char * pcFileName = ( volatile char * ) pcFile;
    volatile uint32_t ulLineNumber = ulLine;

    ( void ) pcFileName;
    ( void ) ulLineNumber;

    printf( "vAssertCalled %s, %ld\n", pcFile, ( long ) ulLine );
    fflush( stdout );

    /* Setting ulBlockVariable to a non-zero value in the debugger will allow
     * this function to be exited. */
    taskDISABLE_INTERRUPTS();
    {
        while( ulBlockVariable == 0UL )
        {
            vTaskDelay( pdMS_TO_TICKS( ulLongSleep ) );
        }
    }
    taskENABLE_INTERRUPTS();
}

/*-----------------------------------------------------------*/

/**
 * @brief User defined application error handler required by NRF52SDK
 */
void app_error_handler( uint32_t error_code,
                        uint32_t line_num,
                        const uint8_t * p_file_name )
{
}
