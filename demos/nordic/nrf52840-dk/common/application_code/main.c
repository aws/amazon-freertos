/*
 * Amazon FreeRTOS
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

/* Demo includes */
#include "aws_demo_runner.h"

/* AWS library includes. */
#include "aws_system_init.h"
#include "aws_logging_task.h"
#include "aws_wifi.h"
#include "aws_clientcredential.h"

/* Nordic BSP includes */
#include "bsp.h"
#include "nordic_common.h"
#include "nrf_drv_clock.h"
#include "nrf_log_ctrl.h"
#include "nrf_log_default_backends.h"
#include "nrf_sdh.h"
#include "nrf_sdh_soc.h"
#include "nrf_sdh_ble.h"
#include "nrf_sdh_freertos.h"
#include "sensorsim.h"
#include "ble_srv_common.h"
#include "ble_advdata.h"
#include "ble_advertising.h"
#include "timers.h"
#include "app_timer.h"
#include "ble_conn_state.h"
#include "nrf_drv_clock.h"
#include "nrf_ble_gatt.h"
#include "nrf_ble_qwr.h"
#include "ble_conn_params.h"
#include "peer_manager.h"
#include "peer_manager_handler.h"
#include "fds.h"
#include "bsp_btn_ble.h"

#include "aws_ble_hal_dis.h"

#include "nrf_log.h"
#include "nrf_log_ctrl.h"
#include "nrf_log_default_backends.h"
#include "ble_types.h"
#include "nrf_ble_lesc.h"

#include "bt_hal_manager_adapter_ble.h"
#include "bt_hal_manager.h"
#include "bt_hal_gatt_server.h"
#include "aws_ble_services_init.h"
#include "app_uart.h"

/* MQTT v4 include. */
#include "aws_iot_mqtt.h"

#include <aws_ble.h>

#include "SEGGER_RTT.h"

#if defined (UART_PRESENT)
#include "nrf_uart.h"
#endif
#if defined (UARTE_PRESENT)
#include "nrf_uarte.h"
#endif

/* clang-format off */

/* Logging Task Defines. */
#define mainLOGGING_MESSAGE_QUEUE_LENGTH    ( 15 )
#define mainLOGGING_TASK_STACK_SIZE         ( configMINIMAL_STACK_SIZE * 4 )

/* BLE Lib defines. */
#define mainBLE_DEVICE_NAME                 "nRF52840-dk"                               /**< Name of device. Will be included in the advertising data. */
#define mainBLE_MTU                         ( BLE_GATTS_EVT_EXCHANGE_MTU_REQUEST );
#define mainBLE_SERVER_UUID                 { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 }

/* UART Buffer sizes. */
#define UART_TX_BUF_SIZE               ( 256 )                                      /**< UART TX buffer size. */
#define UART_RX_BUF_SIZE               ( 256 )                                       /**< UART RX buffer size. */

/* LED-associated defines */
#define LED_ONE                             BSP_LED_0_MASK
#define LED_TWO                             BSP_LED_1_MASK
#define LED_THREE                           BSP_LED_2_MASK
#define LED_FOUR                            BSP_LED_3_MASK
#define ALL_APP_LED                     \
    ( BSP_LED_0_MASK | BSP_LED_1_MASK | \
      BSP_LED_2_MASK | BSP_LED_3_MASK )                                             /**< Define used for simultaneous operation of all application LEDs. */
#define LED_BLINK_INTERVAL_MS               ( 300 )                                 /**< LED blinking interval. */                     

/* Connection Params defines */
#define APP_BLE_OBSERVER_PRIO               3                                       /**< Application's BLE observer priority. You shouldn't need to modify this value. */
#define APP_BLE_CONN_CFG_TAG                1                                       /**< A tag identifying the SoftDevice BLE configuration. */

#define APP_ADV_INTERVAL                    300                                     /**< The advertising interval (in units of 0.625 ms. This value corresponds to 187.5 ms). */
#define APP_ADV_DURATION                    18000                                   /**< The advertising duration (180 seconds) in units of 10 milliseconds. */

#define MIN_CONN_INTERVAL                   MSEC_TO_UNITS( 400, UNIT_1_25_MS )     /**< Minimum acceptable connection interval (0.4 seconds). */
#define MAX_CONN_INTERVAL                   MSEC_TO_UNITS( 650, UNIT_1_25_MS )     /**< Maximum acceptable connection interval (0.65 second). */
#define SLAVE_LATENCY                       0                                       /**< Slave latency. */
#define CONN_SUP_TIMEOUT                    MSEC_TO_UNITS( 4000, UNIT_10_MS )       /**< Connection supervisory time-out (4 seconds). */

#define FIRST_CONN_PARAMS_UPDATE_DELAY      APP_TIMER_TICKS(5000)                   /**< Time from initiating event (connect or start of notification) to first time sd_ble_gap_conn_param_update is called (5 seconds). */
#define NEXT_CONN_PARAMS_UPDATE_DELAY       APP_TIMER_TICKS(30000)                  /**< Time between each call to sd_ble_gap_conn_param_update after the first call (30 seconds). */
#define MAX_CONN_PARAMS_UPDATE_COUNT        3                                       /**< Number of attempts before giving up the connection parameter negotiation. */

/* Security Params defines */
#define LESC_DEBUG_MODE                 0                                           /**< Set to 1 to use the LESC debug keys. The debug mode allows you to use a sniffer to inspect traffic. */
#define LESC_MITM_NC                    1                                           /**< Use MITM (Numeric Comparison). */

#define SEC_PARAMS_BOND                 1                                            /**< Perform bonding. */
#if LESC_MITM_NC
#define SEC_PARAMS_MITM                 1                                            /**< Man In The Middle protection required. */
#define SEC_PARAMS_IO_CAPABILITIES      BLE_GAP_IO_CAPS_DISPLAY_YESNO                /**< Display Yes/No to force Numeric Comparison. */
#else
#define SEC_PARAMS_MITM                 0                                            /**< Man In The Middle protection required. */
#define SEC_PARAMS_IO_CAPABILITIES      BLE_GAP_IO_CAPS_NONE                         /**< No I/O caps. */
#endif
#define SEC_PARAMS_LESC                 1                                            /**< LE Secure Connections pairing required. */
#define SEC_PARAMS_KEYPRESS             0                                            /**< Keypress notifications not required. */
#define SEC_PARAMS_OOB                  0                                            /**< Out Of Band data not available. */
#define SEC_PARAMS_MIN_KEY_SIZE         7                                            /**< Minimum encryption key size in octets. */
#define SEC_PARAMS_MAX_KEY_SIZE         16                                           /**< Maximum encryption key size in octets. */

/* Instances used in the app. */
BLE_CUS_DEF( m_cus );
NRF_BLE_GATT_DEF( m_gatt );           /**< GATT module instance. */
NRF_BLE_QWR_DEF( m_qwr );             /**< Context for the Queued Write module.*/
BLE_ADVERTISING_DEF( m_advertising ); /**< Advertising module instance. */

static ble_evt_t g_ble_evt;
static volatile uint16_t  m_conn_handle_num_comp_peripheral  = BLE_CONN_HANDLE_INVALID;  /**< Connection handle for the peripheral that needs a numeric comparison button press. */
static uint16_t m_ble_nus_max_data_len = BLE_GATT_ATT_MTU_DEFAULT - 3;
static uint16_t m_conn_handle = BLE_CONN_HANDLE_INVALID; /**< Handle of the current connection. */

/* UUID's used. */
#define AWS_DEV_INFO_BASE_UUID                                                                           \
    { { 0x00, 0xFF, 0x32, 0xF9, 0x79, 0xE6, 0xB5, 0x83, 0xFB, 0x4E, 0xAF, 0x48, 0x68, 0x11, 0x7F, 0x8A } \
    }                                 

#define BLE_UUID_CUS_SERVICE    0x1168 

static ble_uuid_t m_adv_uuids[] = /**< Universally unique service identifier. */
{
    { BLE_UUID_CUS_SERVICE, BLE_UUID_TYPE_VENDOR_BEGIN }
};

static char * roles_str[] =
{
    "INVALID_ROLE",
    "PERIPHERAL",
    "CENTRAL",
};

#if NRF_LOG_ENABLED
    static TaskHandle_t m_logger_thread;                            /**< Definition of Logger thread. */
#endif


/* The task delay for allowing the lower priority logging task to print out Wi-Fi
 * failure status before blocking indefinitely. */
#define mainLOGGING_WIFI_STATUS_DELAY      pdMS_TO_TICKS( 1000 )

/* Unit test defines. */
#define mainTEST_RUNNER_TASK_STACK_SIZE    ( configMINIMAL_STACK_SIZE * 0 )

/* clang-format on */

/**
 * @brief Application task startup hook for applications using Wi-Fi. If you are not
 * using Wi-Fi, then start network dependent applications in the vApplicationIPNetorkEventHook
 * function. If you are not using Wi-Fi, this hook can be disabled by setting
 * configUSE_DAEMON_TASK_STARTUP_HOOK to 0.
 */
void vApplicationDaemonTaskStartupHook( void );

static void advertising_start( void * p_erase_bonds );

/**
 * @brief Initializes the board.
 */
static void prvMiscInitialization( void );

/*-----------------------------------------------------------*/

TaskHandle_t prvLedHandle;

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

/**@brief Function for initializing the nrf log module.
 */
static void log_init( void )
{
    ret_code_t err_code = NRF_LOG_INIT( NULL );

    APP_ERROR_CHECK( err_code );

    NRF_LOG_DEFAULT_BACKENDS_INIT();
}

/**@brief Function for initializing the clock.
 */
static void clock_init( void )
{
    ret_code_t err_code = nrf_drv_clock_init();

    APP_ERROR_CHECK( err_code );
}


static void num_comp_reply(uint16_t conn_handle, bool accept)
{
    uint8_t    key_type;
    ret_code_t err_code;

    if (accept)
    {
        NRF_LOG_INFO("Numeric Match. Conn handle: %d", conn_handle);
        key_type = BLE_GAP_AUTH_KEY_TYPE_PASSKEY;
    }
    else
    {
        NRF_LOG_INFO("Numeric REJECT. Conn handle: %d", conn_handle);
        key_type = BLE_GAP_AUTH_KEY_TYPE_NONE;
    }

    err_code = sd_ble_gap_auth_key_reply(conn_handle,
                                         key_type,
                                         NULL);
    APP_ERROR_CHECK(err_code);
}


/** @brief Function for handling a numeric comparison match request. */
static void on_match_request(uint16_t conn_handle, uint8_t role)
{
    // Mark the appropriate conn_handle as pending.
    NRF_LOG_INFO("Enter y/Y \n\r");

    m_conn_handle_num_comp_peripheral = conn_handle;
   
}


/** @brief Function for handling button presses for numeric comparison match requests. */
static void on_num_comp(bool accept)
{
    /* Check whether any links have pending match requests, and if so, send a reply. */
    num_comp_reply(m_conn_handle_num_comp_peripheral, accept);
    m_conn_handle_num_comp_peripheral = BLE_CONN_HANDLE_INVALID;

}

static void uart_write(uint8_t *pucData , uint32_t ulLength )
{
     uint32_t err_code;

     for (uint32_t i = 0; i < ulLength; i++)
        {
            do
            {
                err_code = app_uart_put(pucData[i]);
                if ((err_code != NRF_SUCCESS) && (err_code != NRF_ERROR_BUSY))
                {
                    APP_ERROR_CHECK(err_code);
                }
            } while (err_code == NRF_ERROR_BUSY);
        }

}

/**@brief Function for handling BLE events.
 *
 * @param[in]   p_ble_evt   Bluetooth stack event.
 * @param[in]   p_context   Unused.
 */
static void ble_evt_handler( ble_evt_t const * p_ble_evt,
                             void * p_context )
{
    uint32_t err_code;
    char        passkey[BLE_GAP_PASSKEY_LEN + 1];
    uint16_t    role = ble_conn_state_role(p_ble_evt->evt.gap_evt.conn_handle);

    SEGGER_RTT_printf(0,"BLE Event Received - %d %x ",p_ble_evt->header.evt_id );

    switch( p_ble_evt->header.evt_id )
    {
        case BLE_GAP_EVT_CONNECTED:
            NRF_LOG_INFO( "Connected\n" );
            uart_write("Connected\r\n", sizeof("Connected\n\n") );
            err_code = bsp_indication_set( BSP_INDICATE_CONNECTED );
            APP_ERROR_CHECK( err_code );
            m_conn_handle = p_ble_evt->evt.gap_evt.conn_handle;
            err_code = nrf_ble_qwr_conn_handle_assign( &m_qwr, m_conn_handle );
            APP_ERROR_CHECK( err_code );
            break;

        case BLE_GAP_EVT_DISCONNECTED:
            NRF_LOG_INFO( "Disconnected\n" );
            uart_write("Disconnected\r\n", sizeof("Disconnected\r\n") );
            /* LED indication will be changed when advertising starts. */
            m_conn_handle = BLE_CONN_HANDLE_INVALID;
            break;

        case BLE_GAP_EVT_PASSKEY_DISPLAY:
            memcpy(passkey, p_ble_evt->evt.gap_evt.params.passkey_display.passkey, BLE_GAP_PASSKEY_LEN);
            passkey[BLE_GAP_PASSKEY_LEN] = 0x00;
            SEGGER_RTT_printf(0,"%s: BLE_GAP_EVT_PASSKEY_DISPLAY: passkey=%s match_req=%d",
                              nrf_log_push(roles_str[role]),
                              nrf_log_push(passkey),
                              p_ble_evt->evt.gap_evt.params.passkey_display.match_request);

            uart_write("Passkey : ", sizeof("Passkey : ") );
            uart_write(p_ble_evt->evt.gap_evt.params.passkey_display.passkey, BLE_GAP_PASSKEY_LEN );
            uart_write("\r\n", sizeof("\r\n") );
            uart_write("Press Y to accept or N to reject\r\n", sizeof("Press Y to accept or N to reject\r\n") );

            if (p_ble_evt->evt.gap_evt.params.passkey_display.match_request)
            {
                on_match_request(m_conn_handle, role);
            }
            break;

        case BLE_GAP_EVT_AUTH_KEY_REQUEST:
            SEGGER_RTT_printf(0,"%s: BLE_GAP_EVT_AUTH_KEY_REQUEST", nrf_log_push(roles_str[role]));
            break;

        case BLE_GAP_EVT_LESC_DHKEY_REQUEST:
             SEGGER_RTT_printf(0,"%s: BLE_GAP_EVT_LESC_DHKEY_REQUEST", nrf_log_push(roles_str[role]));
            break;

         case BLE_GAP_EVT_AUTH_STATUS:
             SEGGER_RTT_printf(0, "%s: BLE_GAP_EVT_AUTH_STATUS: status=0x%x bond=0x%x lv4: %d kdist_own:0x%x kdist_peer:0x%x",
                               nrf_log_push(roles_str[role]),
                               p_ble_evt->evt.gap_evt.params.auth_status.auth_status,
                               p_ble_evt->evt.gap_evt.params.auth_status.bonded,
                               p_ble_evt->evt.gap_evt.params.auth_status.sm1_levels.lv4,
                               *((uint8_t *)&p_ble_evt->evt.gap_evt.params.auth_status.kdist_own),
                               *((uint8_t *)&p_ble_evt->evt.gap_evt.params.auth_status.kdist_peer));


              g_ble_evt.evt.gatts_evt.conn_handle = p_ble_evt->evt.gap_evt.conn_handle;
              g_ble_evt.header.evt_id = BLE_GATTS_EVT_EXCHANGE_MTU_REQUEST;
              g_ble_evt.evt.gatts_evt.params.exchange_mtu_request.client_rx_mtu = NRF_SDH_BLE_GATT_MAX_MTU_SIZE;

              prvGATTeventHandler( &g_ble_evt );
            break;

        case BLE_GAP_EVT_PHY_UPDATE_REQUEST:
        {
             SEGGER_RTT_printf(0,"PHY update request.");
            ble_gap_phys_t const phys =
            {
                .rx_phys = BLE_GAP_PHY_AUTO,
                .tx_phys = BLE_GAP_PHY_AUTO,
            };
            ret_code_t err_code = sd_ble_gap_phy_update(p_ble_evt->evt.gap_evt.conn_handle, &phys);
            APP_ERROR_CHECK(err_code);
        } 

        case BLE_GAP_EVT_SEC_PARAMS_REQUEST:
#ifndef LESC_MITM_NC
            /* Pairing not supported */
            err_code = sd_ble_gap_sec_params_reply( m_conn_handle, BLE_GAP_SEC_STATUS_PAIRING_NOT_SUPP, NULL, NULL );
            APP_ERROR_CHECK( err_code );
#endif
            break;

        case BLE_GATTS_EVT_SYS_ATTR_MISSING:
            /* No system attributes have been stored. */
            err_code = sd_ble_gatts_sys_attr_set( m_conn_handle, NULL, 0, 0 );
            APP_ERROR_CHECK( err_code );
            break;

        case BLE_GATTC_EVT_TIMEOUT:
            /* Disconnect on GATT Client timeout event. */
            err_code = sd_ble_gap_disconnect( p_ble_evt->evt.gattc_evt.conn_handle,
                                              BLE_HCI_REMOTE_USER_TERMINATED_CONNECTION );
            APP_ERROR_CHECK( err_code );
            break;

        case BLE_GATTS_EVT_TIMEOUT:
            /* Disconnect on GATT Server timeout event. */
            err_code = sd_ble_gap_disconnect( p_ble_evt->evt.gatts_evt.conn_handle,
                                              BLE_HCI_REMOTE_USER_TERMINATED_CONNECTION );
            APP_ERROR_CHECK( err_code );
            break;

        default:
            /* No implementation needed. */
            break;
    }

    prvGATTeventHandler( p_ble_evt );

    err_code = nrf_ble_lesc_request_handler();
    APP_ERROR_CHECK(err_code);
}

/**@brief Clear bond information from persistent storage. */
static void delete_bonds( void )
{
    ret_code_t err_code;

    NRF_LOG_INFO( "Erase bonds!" );
  
    err_code = pm_peers_delete();
    APP_ERROR_CHECK( err_code );
}

/**@brief   Function for handling app_uart events.
 *
 * @details This function will receive a single character from the app_uart module and append it to
 *          a string. The string will be be sent over BLE when the last character received was a
 *          'new line' '\n' (hex 0x0A) or if the string has reached the maximum data length.
 */
/**@snippet [Handling the data received over UART] */
void uart_event_handle(app_uart_evt_t * p_event)
{
    static uint8_t data_array;
    static uint8_t index = 0;
    uint32_t       err_code;
    uint8_t ucRxByte = 0;

    switch (p_event->evt_type)
    {
        case APP_UART_DATA_READY:
            app_uart_get(&ucRxByte);
            app_uart_put(ucRxByte);
            if(ucRxByte == 'y' || ucRxByte == 'Y')
            {
              on_num_comp(true);
            }

            if(ucRxByte == 'd' || ucRxByte == 'D')
            {
              delete_bonds();
            }

            break;

        case APP_UART_COMMUNICATION_ERROR:
            APP_ERROR_HANDLER(p_event->data.error_communication);
            break;

        case APP_UART_FIFO_ERROR:
            APP_ERROR_HANDLER(p_event->data.error_code);
            break;

        default:
            break;
    }
}




/**@brief Function for handling the Battery measurement timer time-out.
 *
 * @details This function will be called each time the battery level measurement timer expires.
 *
 * @param[in] xTimer Handler to the timer that called this function.
 *                   You may get identifier given to the function xTimerCreate using pvTimerGetTimerID.
 */
static void battery_level_meas_timeout_handler( TimerHandle_t xTimer )
{
    UNUSED_PARAMETER( xTimer );
}

#if NRF_LOG_ENABLED

/**@brief Thread for handling the logger.
 *
 * @details This thread is responsible for processing log entries if logs are deferred.
 *          Thread flushes all log entries and suspends. It is resumed by idle task hook.
 *
 * @param[in]   arg   Pointer used for passing some arbitrary information (context) from the
 *                    osThreadCreate() call to the thread.
 */
    static void logger_thread( void * arg )
    {
        UNUSED_PARAMETER( arg );

        while( 1 )
        {
            NRF_LOG_FLUSH();

            vTaskSuspend( NULL ); /* Suspend myself */
        }
    }
#endif /*NRF_LOG_ENABLED */

/**@brief Function for initializing the BLE stack.
 *
 * @details Initializes the SoftDevice and the BLE event interrupt.
 */
static void ble_stack_init( void )
{
    ret_code_t err_code;

    err_code = nrf_sdh_enable_request();
    APP_ERROR_CHECK( err_code );

    /* Configure the BLE stack using the default settings. */
    /* Fetch the start address of the application RAM. */
    uint32_t ram_start = 0;
    err_code = nrf_sdh_ble_default_cfg_set( APP_BLE_CONN_CFG_TAG, &ram_start );
    APP_ERROR_CHECK( err_code );

    /* Enable BLE stack. */
    err_code = nrf_sdh_ble_enable( &ram_start );
    APP_ERROR_CHECK( err_code );

    /* Register a handler for BLE events. */
    NRF_SDH_BLE_OBSERVER( m_ble_observer, APP_BLE_OBSERVER_PRIO, ble_evt_handler, NULL );
}

/**@brief  Function for initializing the UART module.
 */
/**@snippet [UART Initialization] */
static void uart_init(void)
{
    uint32_t                     err_code;
    app_uart_comm_params_t const comm_params =
    {
        .rx_pin_no    = RX_PIN_NUMBER,
        .tx_pin_no    = TX_PIN_NUMBER,
        .rts_pin_no   = RTS_PIN_NUMBER,
        .cts_pin_no   = CTS_PIN_NUMBER,
        .flow_control = APP_UART_FLOW_CONTROL_DISABLED,
        .use_parity   = false,
#if defined (UART_PRESENT)
        .baud_rate    = NRF_UART_BAUDRATE_115200
#else
        .baud_rate    = NRF_UARTE_BAUDRATE_115200
#endif
    };

    APP_UART_FIFO_INIT(&comm_params,
                       UART_RX_BUF_SIZE,
                       UART_TX_BUF_SIZE,
                       uart_event_handle,
                       APP_IRQ_PRIORITY_LOWEST,
                       err_code);
    APP_ERROR_CHECK(err_code);
}

/**@brief Function for putting the chip into sleep mode.
 * /**@brief Function for putting the chip into sleep mode
 *
 * @note This function will not return.
 */
static void sleep_mode_enter( void )
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

/**@brief Function for handling events from the BSP module.
 *
 * @param[in]   event   Event generated by button press.
 */
static void bsp_event_handler( bsp_event_t event )
{
    ret_code_t err_code;

    switch( event )
    {
        case BSP_EVENT_SLEEP:
            sleep_mode_enter();
            break;

        case BSP_EVENT_DISCONNECT:
            err_code = sd_ble_gap_disconnect( m_conn_handle,
                                              BLE_HCI_REMOTE_USER_TERMINATED_CONNECTION );

            if( err_code != NRF_ERROR_INVALID_STATE )
            {
                APP_ERROR_CHECK( err_code );
            }

            break;

        case BSP_EVENT_WHITELIST_OFF:

            if( m_conn_handle == BLE_CONN_HANDLE_INVALID )
            {
                err_code = ble_advertising_restart_without_whitelist( &m_advertising );

                if( err_code != NRF_ERROR_INVALID_STATE )
                {
                    APP_ERROR_CHECK( err_code );
                }
            }

            break;

        default:
            break;
    }
}


/**@brief Function for the Timer initialization.
 *
 * @details Initializes the timer module. This creates and starts application timers.
 */
static void timers_init( void )
{
    /* Initialize timer module. */
    ret_code_t err_code = app_timer_init();

    APP_ERROR_CHECK( err_code );
}

/**@brief Function for initializing buttons and leds.
 *
 * @param[out] p_erase_bonds  Will be true if the clear bonding button was pressed to wake the application up.
 */
static void buttons_leds_init( bool * p_erase_bonds )
{
    ret_code_t err_code;
    bsp_event_t startup_event;

    err_code = bsp_init( BSP_INIT_LEDS | BSP_INIT_BUTTONS, bsp_event_handler );
    APP_ERROR_CHECK( err_code );

    err_code = bsp_btn_ble_init( NULL, &startup_event );
    APP_ERROR_CHECK( err_code );

    *p_erase_bonds = ( startup_event == BSP_EVENT_CLEAR_BONDING_DATA );
}

/**@brief Function for the GAP initialization.
 *
 * @details This function sets up all the necessary GAP (Generic Access Profile) parameters of the
 *          device including the device name, appearance, and the preferred connection parameters.
 */
static void gap_params_init( void )
{
    ret_code_t err_code;
    ble_gap_conn_params_t gap_conn_params;
    ble_gap_conn_sec_mode_t sec_mode;

    BLE_GAP_CONN_SEC_MODE_SET_OPEN( &sec_mode );

    err_code = sd_ble_gap_device_name_set( &sec_mode,
                                           ( const uint8_t * ) mainBLE_DEVICE_NAME,
                                           strlen( mainBLE_DEVICE_NAME ) );
    APP_ERROR_CHECK( err_code );

    err_code = sd_ble_gap_appearance_set( BLE_APPEARANCE_HEART_RATE_SENSOR_HEART_RATE_BELT );
    APP_ERROR_CHECK( err_code );

    memset( &gap_conn_params, 0, sizeof( gap_conn_params ) );

    gap_conn_params.min_conn_interval = MIN_CONN_INTERVAL;
    gap_conn_params.max_conn_interval = MAX_CONN_INTERVAL;
    gap_conn_params.slave_latency = SLAVE_LATENCY;
    gap_conn_params.conn_sup_timeout = CONN_SUP_TIMEOUT;

    err_code = sd_ble_gap_ppcp_set( &gap_conn_params );
    APP_ERROR_CHECK( err_code );
}

/**@brief Function for handling events from the GATT library. */
void gatt_evt_handler( nrf_ble_gatt_t * p_gatt,
                       nrf_ble_gatt_evt_t const * p_evt )
{
    if( ( m_conn_handle == p_evt->conn_handle ) && ( p_evt->evt_id == NRF_BLE_GATT_EVT_ATT_MTU_UPDATED ) )
    {
        m_ble_nus_max_data_len = p_evt->params.att_mtu_effective;
        SEGGER_RTT_printf( 0, "Data len is set to 0x%X(%d) \n\r", m_ble_nus_max_data_len, m_ble_nus_max_data_len );
    }

    SEGGER_RTT_printf( 0, "ATT MTU exchange completed. central 0x%x peripheral 0x%x \n\r",
                       p_gatt->att_mtu_desired_central,
                       p_gatt->att_mtu_desired_periph );
}


/**@brief Function for initializing the GATT module. */
static void gatt_init( void )
{
    ret_code_t err_code = nrf_ble_gatt_init( &m_gatt, gatt_evt_handler );

    APP_ERROR_CHECK( err_code );

    err_code = nrf_ble_gatt_att_mtu_periph_set( &m_gatt, NRF_SDH_BLE_GATT_MAX_MTU_SIZE );
    APP_ERROR_CHECK( err_code );
}



/**@brief Function for handling advertising events.
 *
 * @details This function will be called for advertising events which are passed to the application.
 *
 * @param[in] ble_adv_evt  Advertising event.
 */
static void on_adv_evt( ble_adv_evt_t ble_adv_evt )
{
    uint32_t err_code;

    switch( ble_adv_evt )
    {
        case BLE_ADV_EVT_FAST:
            NRF_LOG_INFO( "Fast advertising." );
            uart_write("Fast Advertising\r\n", sizeof("Fast Advertising\r\n"));
            err_code = bsp_indication_set( BSP_INDICATE_ADVERTISING );
            APP_ERROR_CHECK( err_code );
            break;

        case BLE_ADV_EVT_IDLE:
            sleep_mode_enter();
            break;

        default:
            break;
    }
}


/**@brief Function for initializing the Advertising functionality. */
static void advertising_init( void )
{
    ret_code_t err_code;
    ble_advertising_init_t init;
    ble_uuid128_t aws_base_uuid = AWS_DEV_INFO_BASE_UUID;
    static uint8_t uuidtype;

    memset( &init, 0, sizeof( init ) );

    init.advdata.name_type = BLE_ADVDATA_FULL_NAME;
    init.advdata.include_appearance = true;
    init.advdata.flags = BLE_GAP_ADV_FLAGS_LE_ONLY_GENERAL_DISC_MODE;
    init.srdata.uuids_complete.uuid_cnt = sizeof( m_adv_uuids ) / sizeof( m_adv_uuids[ 0 ] );
    init.srdata.uuids_complete.p_uuids = m_adv_uuids;

    init.config.ble_adv_fast_enabled = true;
    init.config.ble_adv_fast_interval = APP_ADV_INTERVAL;
    init.config.ble_adv_fast_timeout = APP_ADV_DURATION;

    init.evt_handler = on_adv_evt;

    err_code = ble_advertising_init( &m_advertising, &init );
    APP_ERROR_CHECK( err_code );

    ble_advertising_conn_cfg_tag_set( &m_advertising, APP_BLE_CONN_CFG_TAG );
}


/**@brief Function for handling Queued Write Module errors.
 *
 * @details A pointer to this function will be passed to each service which may need to inform the
 *          application about an error.
 *
 * @param[in]   nrf_error   Error code containing information about what went wrong.
 */
static void nrf_qwr_error_handler( uint32_t nrf_error )
{
    APP_ERROR_HANDLER( nrf_error );
}


/**@brief Function for initializing services that will be used by the application.
 *
 * @details Initialize services.
 */
static void services_init( void )
{
    ret_code_t err_code;
    ble_cus_init_t cus_init;
    nrf_ble_qwr_init_t qwr_init = { 0 };

    /* Initialize Queued Write Module. */
    qwr_init.error_handler = nrf_qwr_error_handler;

    err_code = nrf_ble_qwr_init( &m_qwr, &qwr_init );
    APP_ERROR_CHECK( err_code );
}


/**@brief Function for handling the Connection Parameters Module.
 *
 * @details This function will be called for all events in the Connection Parameters Module which
 *          are passed to the application.
 *          @note All this function does is to disconnect. This could have been done by simply
 *                setting the disconnect_on_fail config parameter, but instead we use the event
 *                handler mechanism to demonstrate its use.
 *
 * @param[in]   p_evt   Event received from the Connection Parameters Module.
 */
static void on_conn_params_evt( ble_conn_params_evt_t * p_evt )
{
    ret_code_t err_code;

    if( p_evt->evt_type == BLE_CONN_PARAMS_EVT_FAILED )
    {
        err_code = sd_ble_gap_disconnect( m_conn_handle, BLE_HCI_CONN_INTERVAL_UNACCEPTABLE );
        APP_ERROR_CHECK( err_code );
    }
}


/**@brief Function for handling a Connection Parameters error.
 *
 * @param[in]   nrf_error   Error code containing information about what went wrong.
 */
static void conn_params_error_handler( uint32_t nrf_error )
{
    APP_ERROR_HANDLER( nrf_error );
}


/**@brief Function for initializing the Connection Parameters module. */
static void conn_params_init( void )
{
    ret_code_t             err_code;
    ble_conn_params_init_t cp_init;

    memset(&cp_init, 0, sizeof(cp_init));

    cp_init.p_conn_params                  = NULL;
    cp_init.first_conn_params_update_delay = FIRST_CONN_PARAMS_UPDATE_DELAY;
    cp_init.next_conn_params_update_delay  = NEXT_CONN_PARAMS_UPDATE_DELAY;
    cp_init.max_conn_params_update_count   = MAX_CONN_PARAMS_UPDATE_COUNT;
    cp_init.start_on_notify_cccd_handle    = BLE_GATT_HANDLE_INVALID; // Start upon connection.
    cp_init.disconnect_on_fail             = true;
    cp_init.evt_handler                    = NULL;  // Ignore events.
    cp_init.error_handler                  = conn_params_error_handler;

    err_code = ble_conn_params_init(&cp_init);
    APP_ERROR_CHECK(err_code);
}

/**@brief Function for handling Peer Manager events.
 *
 * @param[in] p_evt  Peer Manager event.
 */
static void pm_evt_handler( pm_evt_t const * p_evt )
{
    bool delete_bonds = false;
    pm_conn_sec_config_t conn_sec_config; 

    pm_handler_on_pm_evt(p_evt);
    pm_handler_disconnect_on_sec_failure(p_evt);
    pm_handler_flash_clean(p_evt);

    switch( p_evt->evt_id )
    {
        case PM_EVT_PEERS_DELETE_SUCCEEDED:
            //advertising_start( &delete_bonds );
            break;

        case PM_EVT_CONN_SEC_CONFIG_REQ:
            /* Allow pairing request from an already bonded peer.*/
            conn_sec_config.allow_repairing = true;
            pm_conn_sec_config_reply(p_evt->conn_handle, &conn_sec_config);
            break;

        default:
            break;
    }
}

/**@brief Function for the Peer Manager initialization. */
static void peer_manager_init( void )
{
    ble_gap_sec_params_t sec_params;
    ret_code_t err_code;

    err_code = pm_init();
    APP_ERROR_CHECK(err_code);

    memset(&sec_params, 0, sizeof(ble_gap_sec_params_t));

    // Security parameters to be used for all security procedures.
    sec_params.bond           = SEC_PARAMS_BOND;
    sec_params.mitm           = SEC_PARAMS_MITM;
    sec_params.lesc           = SEC_PARAMS_LESC;
    sec_params.keypress       = SEC_PARAMS_KEYPRESS;
    sec_params.io_caps        = SEC_PARAMS_IO_CAPABILITIES;
    sec_params.oob            = SEC_PARAMS_OOB;
    sec_params.min_key_size   = SEC_PARAMS_MIN_KEY_SIZE;
    sec_params.max_key_size   = SEC_PARAMS_MAX_KEY_SIZE;
    sec_params.kdist_own.enc  = 1;
    sec_params.kdist_own.id   = 1;
    sec_params.kdist_peer.enc = 1;
    sec_params.kdist_peer.id  = 1;

    err_code = pm_sec_params_set(&sec_params);
    APP_ERROR_CHECK(err_code);

    err_code = pm_register(pm_evt_handler);
    APP_ERROR_CHECK(err_code);
}

void prvBLEGAPPairingStateChangedCb( BTStatus_t xStatus,
                                     BTBdaddr_t * pxRemoteBdAddr,
                                     BTSecurityLevel_t xSecurityLevel,
                                     BTAuthFailureReason_t xReason )
{
}

void prvNumericComparisonCb( BTBdaddr_t * pxRemoteBdAddr,
                             uint32_t ulPassKey )
{
}
/*-----------------------------------------------------------*/
#define MAX_PROPERTIES    ( 5 )

static BTStatus_t prvBLEInit( void )
{
    BLEEventsCallbacks_t xEventCb;
    BTUuid_t xServerUUID =
    {
        .ucType   = eBTuuidType128,
        .uu.uu128 = mainBLE_SERVER_UUID
    };

    const bool bIsBondable = false;
    const uint32_t usMtu = mainBLE_MTU;
    const BTIOtypes_t xIO = eBTIONone;
    const bool bSecureConnectionOnly = false;

    BTProperty_t xDeviceProperties[ MAX_PROPERTIES ] =
    {
        {
            .xType = eBTpropertyBdname,
            .xLen = strlen( mainBLE_DEVICE_NAME ),
            .pvVal = ( void * ) mainBLE_DEVICE_NAME
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
            .xType = eBTpropertyIO,
            .xLen = 1,
            .pvVal = ( void * ) &xIO
        },
        {
            .xType = eBTpropertySecureConnectionOnly,
            .xLen = 1,
            .pvVal = ( void * ) &bSecureConnectionOnly
        },
    };


    BTStatus_t xStatus = eBTStatusSuccess;
    const BTInterface_t * pxIface = BTGetBluetoothInterface();

    /* Initialize Adapter Interface */
    if( xStatus == eBTStatusSuccess )
    {
        if( pxIface != NULL )
        {
            xStatus = pxIface->pxEnable( 0 );
        }
        else
        {
            xStatus = eBTStatusFail;
        }
    }

    /* Initialize BLE GAP Layer */
    if( xStatus == eBTStatusSuccess )
    {
        xStatus = BLE_Init( &xServerUUID, xDeviceProperties, MAX_PROPERTIES );

        if( xStatus == eBTStatusSuccess )
        {
            xEventCb.pxGAPPairingStateChangedCb = &prvBLEGAPPairingStateChangedCb;
            xStatus = BLE_RegisterEventCb( eBLEPairingStateChanged, xEventCb );
        }

        if( xStatus == eBTStatusSuccess )
        {
            xEventCb.pxNumericComparisonCb = &prvNumericComparisonCb;
            xStatus = BLE_RegisterEventCb( eBLENumericComparisonCallback, xEventCb );
        }
    }

    /* Initialize BLE Services */
    if( xStatus == eBTStatusSuccess )
    {
        /*Initialize bluetooth services */
        if( BLE_SERVICES_Init() != pdPASS )
        {
            xStatus = eBTStatusFail;
        }
    }

    return xStatus;
}

/**@brief Function for starting advertising. */
static void advertising_start( void * p_erase_bonds )
{
    uint32_t ulEnabledNetworks = 0;

    /* Start AWS BLE lib */
    prvBLEInit();

    if( AwsIotNetworkManager_Init() != pdPASS )
    {
        SEGGER_RTT_printf( 0, "Failed to initialize the network manager \n " );

        while( 1 )
        {
        }
    }

    ulEnabledNetworks = AwsIotNetworkManager_EnableNetwork( configENABLED_NETWORKS );

    if( ( ulEnabledNetworks & configENABLED_NETWORKS ) != configENABLED_NETWORKS )
    {
        SEGGER_RTT_printf( 0, "Failed to enable all the networks, enabled networks: %08x\n ", ulEnabledNetworks );

        while( 1 )
        {
        }
    }

    bool erase_bonds = *( bool * ) p_erase_bonds;

    if( erase_bonds )
    {
        delete_bonds();
        /* Advertising is started by PM_EVT_PEERS_DELETE_SUCCEEDED event. */
    }
    else
    {
        advertising_init();

        ret_code_t err_code = ble_advertising_start( &m_advertising, BLE_ADV_MODE_FAST );
        APP_ERROR_CHECK( err_code );
    }
}


/*-----------------------------------------------------------*/

static void prvMiscInitialization( void )
{
    bool erase_bonds;

    /* Initialize modules. */
    uart_init();
    log_init();
    clock_init();

    #if NRF_LOG_ENABLED
        if( pdPASS != xTaskCreate( logger_thread, "LOGGER", 256, NULL, 1, &m_logger_thread ) )
        {
            APP_ERROR_HANDLER( NRF_ERROR_NO_MEM );
        }
    #endif

    /* Activate deep sleep mode. */
    SCB->SCR |= SCB_SCR_SLEEPDEEP_Msk;

    /* Configure and initialize the BLE stack. */
    ble_stack_init();

    /* Initialize modules. */
    timers_init();
    buttons_leds_init( &erase_bonds );
    gap_params_init();
    gatt_init();
    services_init();
    conn_params_init();
    peer_manager_init();
    /* application_timers_start(); */

    /* Create a FreeRTOS task for the BLE stack.
     * The task will run advertising_start() before entering its loop. */

    nrf_sdh_freertos_init( advertising_start, &erase_bonds );

    NRF_LOG_INFO( "Amazon FreeRTOS BLE demo on nRF52840\r\n" );
    uart_write( "Amazon FreeRTOS BLE demo on nRF52840\r\n", sizeof("Amazon FreeRTOS BLE demo on nRF52840\r\n") );
    uart_write("Press D/d to delete bond information\r\n", sizeof("Press D/d to delete bond information\r\n") );
    uart_write("If bond info is deleted please delete bond on device as well\r\n", sizeof("If bond info is deleted please delete bond on device as well\r\n") );
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

    /* Create tasks that are not dependent on the Wi-Fi being initialized. */
    xLoggingTaskInitialize( mainLOGGING_TASK_STACK_SIZE,
                            tskIDLE_PRIORITY,
                            mainLOGGING_MESSAGE_QUEUE_LENGTH );


    vTaskStartScheduler();

    return 0;
}
/*-----------------------------------------------------------*/
void vApplicationDaemonTaskStartupHook( void )
{
    /* FIX ME: Perform any hardware initialization, that require the RTOS to be
     * running, here. */

    BaseType_t xStatus = pdFALSE;

    if( AwsIotMqtt_Init() == AWS_IOT_MQTT_SUCCESS )
    {
        xStatus = pdTRUE;
    }

    /* Start the demo tasks. */
    DEMO_RUNNER_RunDemos();
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
