/*
* FreeRTOS
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
#include "semphr.h"
#include "queue.h"

/* Demo includes */
#include "aws_test_runner.h"

/* AWS library includes. */
#include "iot_logging_task.h"
#include "aws_clientcredential.h"
#include "iot_mqtt.h"
#include "iot_init.h"

/* Nordic BSP includes */
#include "bsp.h"
#include "nrf_log_ctrl.h"
#include "nrf_log_default_backends.h"
#include "nrf_sdh.h"
#include "nrf_sdh_soc.h"
#include "nrf_sdh_ble.h"
#include "nrf_sdh_freertos.h"
#include "sensorsim.h"
#include "timers.h"
#include "app_timer.h"
#include "ble_conn_state.h"
#include "nrf_drv_clock.h"
#include "nrf_ble_gatt.h"
#include "nrf_ble_qwr.h"
#include "ble_conn_params.h"
#include "peer_manager.h"
#include "peer_manager_handler.h"
#include "bsp_btn_ble.h"
#include "app_uart.h"

#include <iot_ble.h>
#include "SEGGER_RTT.h"
#include "aws_application_version.h"
#if defined( UART_PRESENT )
    #include "nrf_uart.h"
#endif
#if defined( UARTE_PRESENT )
    #include "nrf_uarte.h"
#endif

/* clang-format off */
#define mainTEST_RUNNER_TASK_STACK_SIZE     ( configMINIMAL_STACK_SIZE * 12 )

/* Logging Task Defines. */
#define mainLOGGING_MESSAGE_QUEUE_LENGTH    ( 15 )
#define mainLOGGING_TASK_STACK_SIZE         ( configMINIMAL_STACK_SIZE * 4 )

/* BLE Lib defines. */
#define mainBLE_SERVER_UUID                 { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 }

/* UART Buffer sizes. */
#define UART_TX_BUF_SIZE                    ( 256 )                                 /**< UART TX buffer size. */
#define UART_RX_BUF_SIZE                    ( 256 )                                 /**< UART RX buffer size. */

/* LED-associated defines */
#define LED_ONE                             BSP_LED_0_MASK
#define LED_TWO                             BSP_LED_1_MASK
#define LED_THREE                           BSP_LED_2_MASK
#define LED_FOUR                            BSP_LED_3_MASK
#define ALL_APP_LED                     \
    ( BSP_LED_0_MASK | BSP_LED_1_MASK | \
      BSP_LED_2_MASK | BSP_LED_3_MASK )                                        /**< Define used for simultaneous operation of all application LEDs. */
#define LED_BLINK_INTERVAL_MS               ( 300 )                            /**< LED blinking interval. */

SemaphoreHandle_t xUARTTxComplete;
QueueHandle_t UARTqueue = NULL;

/* clang-format on */
extern void IotTestNetwork_SelectNetworkType( uint16_t networkType );
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
static void prvMiscInitialization( void );

/*-----------------------------------------------------------*/

void vUartWrite( uint8_t * pucData )
{
    uint32_t xErrCode;

    SEGGER_RTT_WriteString(0, pucData);
    for( uint32_t i = 0; i < configLOGGING_MAX_MESSAGE_LENGTH; i++ )
    {
        if(pucData[ i ] == 0)
        {
            break;
        }

        do
        {
            xErrCode = app_uart_put(pucData[ i ]);
            if(xErrCode == NRF_ERROR_NO_MEM)
            {
            xErrCode = 0;
            }
        } while( xErrCode == NRF_ERROR_BUSY );
        xSemaphoreTake(xUARTTxComplete, portMAX_DELAY );

    }
}

/*-----------------------------------------------------------*/

/**@brief Function for putting the chip into sleep mode.
 * /**@brief Function for putting the chip into sleep mode
 *
 * @note This function will not return.
 */
static void prvSleeModeEnter( void )
{
    ret_code_t xErrCode;

    xErrCode = bsp_indication_set( BSP_INDICATE_IDLE );
    APP_ERROR_CHECK( xErrCode );

    /* Prepare wakeup buttons. */
    xErrCode = bsp_btn_ble_sleep_mode_prepare();
    APP_ERROR_CHECK( xErrCode );

    /* Go to system-off mode (this function will not return; wakeup will cause a reset). */
    xErrCode = sd_power_system_off();
    APP_ERROR_CHECK( xErrCode );
}

/*-----------------------------------------------------------*/



/*-----------------------------------------------------------*/

/**@brief Function for handling a Connection Parameters error.
 *
 * @param[in]   nrf_error   Error code containing information about what went wrong.
 */
static void conn_params_error_handler( uint32_t nrf_error )
{
    APP_ERROR_HANDLER( nrf_error );
}

/*-----------------------------------------------------------*/

/**@brief   Function for handling uart events.
 *
 * /**@snippet [Handling the data received over UART] */
 
void prvUartEventHandler( app_uart_evt_t * pxEvent )
{
    /* Declared as static so it can be pushed into the queue from the ISR. */
    static volatile uint8_t ucRxByte = 0;
   BaseType_t xHigherPriorityTaskWoken;
    switch( pxEvent->evt_type )
    {
        case APP_UART_DATA_READY:
            app_uart_get( (uint8_t *)&ucRxByte );
            app_uart_put( ucRxByte );

       //portYIELD_FROM_ISR (xHigherPriorityTaskWoken);
    
            break;

        case APP_UART_COMMUNICATION_ERROR:
            APP_ERROR_HANDLER( pxEvent->data.error_communication );
            break;

        case APP_UART_FIFO_ERROR:
            APP_ERROR_HANDLER( pxEvent->data.error_code );
            break;

        case APP_UART_TX_EMPTY:
             xSemaphoreGiveFromISR(xUARTTxComplete, &xHigherPriorityTaskWoken);
            break;

        default:
            break;
    }
}

/*-----------------------------------------------------------*/

/**@brief Function for handling events from the BSP module.
 *
 * @param[in]   event   Event generated by button press.
 */
static void prvBSPEventHandler( bsp_event_t xEvent )
{
    ret_code_t xErrCode;
    const BTInterface_t * pxIface = BTGetBluetoothInterface();
    const BTBleAdapter_t * pxAdapter = pxIface->pxGetLeAdapter();

    switch( xEvent )
    {
        case BSP_EVENT_SLEEP:
            prvSleeModeEnter();
            break;

        case BSP_EVENT_DISCONNECT:

            pxAdapter->pxDisconnect( 0, NULL, 0 );

            break;

        default:
            break;
    }
}

/*-----------------------------------------------------------*/


/**@brief  Function for initializing the UART module.
 */
static void prvUartInit( void )
{
    uint32_t xErrCode;

    app_uart_comm_params_t const xConnParams =
    {
        .rx_pin_no        = RX_PIN_NUMBER,
        .tx_pin_no        = TX_PIN_NUMBER,
        .rts_pin_no       = RTS_PIN_NUMBER,
        .cts_pin_no       = CTS_PIN_NUMBER,
        .flow_control     = APP_UART_FLOW_CONTROL_DISABLED,
        .use_parity       = false,
        #if defined( UART_PRESENT )
            .baud_rate    = NRF_UART_BAUDRATE_115200
        #else
               .baud_rate = NRF_UARTE_BAUDRATE_115200
                            #endif
    };

    APP_UART_FIFO_INIT( &xConnParams,
                        UART_RX_BUF_SIZE,
                        UART_TX_BUF_SIZE,
                        prvUartEventHandler,
                        _PRIO_APP_HIGH,
                        xErrCode );
    APP_ERROR_CHECK( xErrCode );
}

/**
 * @brief Initialize BLE stask for Nordic.
 * On Nordic it is just a stub.
 */
BTStatus_t bleStackInit( void )
{
  return eBTStatusSuccess;
}

/*-----------------------------------------------------------*/

/**@brief Function for initializing the nrf log module.
 */
static void prvLogInit( void )
{
    ret_code_t xErrCode = NRF_LOG_INIT( NULL );

    APP_ERROR_CHECK( xErrCode );

    NRF_LOG_DEFAULT_BACKENDS_INIT();
}

/*-----------------------------------------------------------*/

/**@brief Function for initializing the clock.
 */
static void prvClockInit( void )
{
    ret_code_t xErrCode = nrf_drv_clock_init();

    APP_ERROR_CHECK( xErrCode );
}

/*-----------------------------------------------------------*/

/**@brief Function for the Timer initialization.
 *
 * @details Initializes the timer module. This creates and starts application timers.
 */
static void prvTimersInit( void )
{
    /* Initialize timer module. */
    ret_code_t xErrCode = app_timer_init();

    APP_ERROR_CHECK( xErrCode );
}

/*-----------------------------------------------------------*/

static void prvMiscInitialization( void )
{
    nrf_sdh_enable_request();
    /* Initialize modules.*/
    xUARTTxComplete = xSemaphoreCreateBinary();
    prvUartInit();
    prvClockInit();

    /* Activate deep sleep mode. */
    SCB->SCR |= SCB_SCR_SLEEPDEEP_Msk;
    prvTimersInit();
}
/*-----------------------------------------------------------*/

/**
 * @brief Application runtime entry point.
 */
int main( void )
{
    /* Perform any hardware initialization that does not require the RTOS to be
     * running.  */
    prvMiscInitialization();

    xLoggingTaskInitialize( mainLOGGING_TASK_STACK_SIZE,
                            tskIDLE_PRIORITY,
                            mainLOGGING_MESSAGE_QUEUE_LENGTH );

    nrf_sdh_freertos_init( NULL, NULL );
    ret_code_t xErrCode = pm_init();
    vTaskStartScheduler();

    return 0;
}
/*-----------------------------------------------------------*/


/**@brief Clear bond information from persistent storage. */
static void prvDeleteBonds( void )
{
    ret_code_t xErrCode;

    NRF_LOG_INFO( "Erase bonds!" );

    xErrCode = pm_peers_delete();
    APP_ERROR_CHECK( xErrCode );
}
void vApplicationDaemonTaskStartupHook( void )
{
    xTaskCreate( TEST_RUNNER_RunTests_task,
            "RunTests_task",
            mainTEST_RUNNER_TASK_STACK_SIZE,
            NULL,
            tskIDLE_PRIORITY,
            NULL );
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
