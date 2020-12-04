/*
 * $ Copyright Cypress Semiconductor $
 */

/** @file
 *
 * Runtime Bluetooth stack configuration parameters
 *
 */
#include "wiced_bt_cfg.h"

/*****************************************************************************
 * wiced_bt core stack configuration
 ****************************************************************************/
wiced_bt_cfg_settings_t wiced_bt_cfg_settings =
{
    .device_name               = (unsigned char *)"default", /* Set name using pxBTInterface->pxSetDeviceProperty */
    .device_class              = { 0x00, 0x00, 0x00 },        /**< Local device class */
    .security_requirement_mask = BTM_SEC_SC_ONLY,                /**< Security requirements mask (BTM_SEC_NONE, or combination of BTM_SEC_IN_AUTHENTICATE, BTM_SEC_OUT_AUTHENTICATE, BTM_SEC_ENCRYPT (see #wiced_bt_sec_level_e)) */
    .max_simultaneous_links    = 3,                           /**< Maximum number simultaneous links to different devices */

    /* Scan and advertisement configuration */
    .br_edr_scan_cfg =                 /**< BR/EDR scan settings */
    {
        .inquiry_scan_type     = BTM_SCAN_TYPE_STANDARD,                      /**< Inquiry scan type (BTM_SCAN_TYPE_STANDARD or BTM_SCAN_TYPE_INTERLACED) */
        .inquiry_scan_interval = WICED_BT_CFG_DEFAULT_INQUIRY_SCAN_INTERVAL,  /**< Inquiry scan interval (default: WICED_BT_CFG_DEFAULT_INQUIRY_SCAN_INTERVAL) */
        .inquiry_scan_window   = WICED_BT_CFG_DEFAULT_INQUIRY_SCAN_WINDOW,    /**< Inquiry scan window (default: WICED_BT_CFG_DEFAULT_INQUIRY_SCAN_WINDOW) */
        .page_scan_type        = BTM_SCAN_TYPE_STANDARD,                      /**< Page scan type (BTM_SCAN_TYPE_STANDARD or BTM_SCAN_TYPE_INTERLACED) */
        .page_scan_interval    = WICED_BT_CFG_DEFAULT_PAGE_SCAN_INTERVAL,     /**< Page scan interval (default: WICED_BT_CFG_DEFAULT_PAGE_SCAN_INTERVAL) */
        .page_scan_window      = WICED_BT_CFG_DEFAULT_PAGE_SCAN_WINDOW,       /**< Page scan window (default: WICED_BT_CFG_DEFAULT_PAGE_SCAN_WINDOW) */
    },
    .ble_scan_cfg =                    /**< BLE scan settings */
    {
        .scan_mode                    = BTM_BLE_SCAN_MODE_PASSIVE,                         /**< BLE scan mode (BTM_BLE_SCAN_MODE_PASSIVE, BTM_BLE_SCAN_MODE_ACTIVE) */
        .high_duty_scan_interval      = WICED_BT_CFG_DEFAULT_HIGH_DUTY_SCAN_INTERVAL,      /**< High duty scan interval (default: WICED_BT_CFG_DEFAULT_HIGH_DUTY_SCAN_INTERVAL) */
        .high_duty_scan_window        = WICED_BT_CFG_DEFAULT_HIGH_DUTY_SCAN_WINDOW,        /**< High duty scan window (default: WICED_BT_CFG_DEFAULT_HIGH_DUTY_SCAN_WINDOW) */
        .high_duty_scan_duration      = 5,                                                 /**< High duty scan duration in seconds (0 for infinite) */
        .low_duty_scan_interval       = WICED_BT_CFG_DEFAULT_LOW_DUTY_SCAN_INTERVAL,       /**< Low duty scan interval (default: WICED_BT_CFG_DEFAULT_LOW_DUTY_SCAN_INTERVAL) */
        .low_duty_scan_window         = WICED_BT_CFG_DEFAULT_LOW_DUTY_SCAN_WINDOW,         /**< Low duty scan window (default: WICED_BT_CFG_DEFAULT_LOW_DUTY_SCAN_WINDOW) */
        .low_duty_scan_duration       = 5,                                                 /**< Low duty scan duration in seconds (0 for infinite) */
        .high_duty_conn_scan_interval = WICED_BT_CFG_DEFAULT_HIGH_DUTY_CONN_SCAN_INTERVAL, /**< High duty cycle connection scan interval (default: WICED_BT_CFG_DEFAULT_HIGH_DUTY_CONN_SCAN_INTERVAL) */
        .high_duty_conn_scan_window   = WICED_BT_CFG_DEFAULT_HIGH_DUTY_CONN_SCAN_WINDOW,   /**< High duty cycle connection scan window (default: WICED_BT_CFG_DEFAULT_HIGH_DUTY_CONN_SCAN_WINDOW) */
        .high_duty_conn_duration      = 0,                                                 /**< High duty cycle connection duration in seconds (0 for infinite) */
        .low_duty_conn_scan_interval  = WICED_BT_CFG_DEFAULT_LOW_DUTY_CONN_SCAN_INTERVAL,  /**< Low duty cycle connection scan interval (default: WICED_BT_CFG_DEFAULT_LOW_DUTY_CONN_SCAN_INTERVAL) */
        .low_duty_conn_scan_window    = WICED_BT_CFG_DEFAULT_LOW_DUTY_CONN_SCAN_WINDOW,    /**< Low duty cycle connection scan window (default: WICED_BT_CFG_DEFAULT_LOW_DUTY_CONN_SCAN_WINDOW) */
        .low_duty_conn_duration       = 0,                                                 /**< Low duty cycle connection duration in seconds (0 for infinite) */
        .conn_min_interval            = WICED_BT_CFG_DEFAULT_CONN_MIN_INTERVAL,            /**< Minimum connection interval (default: WICED_BT_CFG_DEFAULT_CONN_MIN_INTERVAL) */
        .conn_max_interval            = WICED_BT_CFG_DEFAULT_CONN_MAX_INTERVAL,            /**< Maximum connection interval (default: WICED_BT_CFG_DEFAULT_CONN_MAX_INTERVAL) */
        .conn_latency                 = WICED_BT_CFG_DEFAULT_CONN_LATENCY,                 /**< Connection latency (default: WICED_BT_CFG_DEFAULT_CONN_LATENCY) */
        .conn_supervision_timeout     = WICED_BT_CFG_DEFAULT_CONN_SUPERVISION_TIMEOUT,     /**< Connection link supervision timeout (default: WICED_BT_CFG_DEFAULT_ CONN_SUPERVISION_TIMEOUT) */
    },
    .ble_advert_cfg =                  /**< BLE advertisement settings */
    {
        .channel_map                     = ( BTM_BLE_ADVERT_CHNL_37 | BTM_BLE_ADVERT_CHNL_38 | BTM_BLE_ADVERT_CHNL_39 ),    /**< Advertising channel map (mask of BTM_BLE_ADVERT_CHNL_37, BTM_BLE_ADVERT_CHNL_38, BTM_BLE_ADVERT_CHNL_39) */
        .high_duty_min_interval          = WICED_BT_CFG_DEFAULT_HIGH_DUTY_ADV_MIN_INTERVAL,          /**< High duty undirected connectable advert minimum advertising interval (default: WICED_BT_CFG_DEFAULT_HIGH_DUTY_ADV_MIN_INTERVAL) */
        .high_duty_max_interval          = WICED_BT_CFG_DEFAULT_HIGH_DUTY_ADV_MAX_INTERVAL,          /**< High duty undirected connectable advert maximum advertising interval (default: WICED_BT_CFG_DEFAULT_HIGH_DUTY_ADV_MAX_INTERVAL) */
        .high_duty_duration              = 0,                                                       /**< High duty advertising duration in seconds (0 for infinite) */
        .low_duty_min_interval           = WICED_BT_CFG_DEFAULT_LOW_DUTY_ADV_MIN_INTERVAL,           /**< Low duty undirected connectable advert minimum advertising interval (default: WICED_BT_CFG_DEFAULT_LOW_DUTY_ADV_MIN_INTERVAL) */
        .low_duty_max_interval           = WICED_BT_CFG_DEFAULT_LOW_DUTY_ADV_MAX_INTERVAL,           /**< Low duty undirected connectable advert maximum advertising interval (default: WICED_BT_CFG_DEFAULT_LOW_DUTY_ADV_MAX_INTERVAL) */
        .low_duty_duration               = 0,                                                       /**< Low duty advertising duration in seconds (0 for infinite) */
        .high_duty_directed_min_interval = WICED_BT_CFG_DEFAULT_HIGH_DUTY_DIRECTED_ADV_MIN_INTERVAL, /**< high duty directed adv minimum advertising interval (default: WICED_BT_CFG_DEFAULT_HIGH_DUTY_DIRECTED_ADV_MIN_INTERVAL) */
        .high_duty_directed_max_interval = WICED_BT_CFG_DEFAULT_HIGH_DUTY_DIRECTED_ADV_MAX_INTERVAL, /**< high duty directed adv maximum advertising interval (default: WICED_BT_CFG_DEFAULT_HIGH_DUTY_DIRECTED_ADV_MAX_INTERVAL) */
        .low_duty_directed_min_interval  = WICED_BT_CFG_DEFAULT_LOW_DUTY_DIRECTED_ADV_MIN_INTERVAL,  /**< Low duty directed adv minimum advertising interval (default: WICED_BT_CFG_DEFAULT_LOW_DUTY_DIRECTED_ADV_MIN_INTERVAL) */
        .low_duty_directed_max_interval  = WICED_BT_CFG_DEFAULT_LOW_DUTY_DIRECTED_ADV_MAX_INTERVAL,  /**< Low duty directed adv maximum advertising interval (default: WICED_BT_CFG_DEFAULT_LOW_DUTY_DIRECTED_ADV_MAX_INTERVAL) */
        .low_duty_directed_duration      = 1,                                                       /**< Low duty directed advertising duration in seconds (0 for infinite) */
        .high_duty_nonconn_min_interval  = WICED_BT_CFG_DEFAULT_HIGH_DUTY_NONCONN_ADV_MIN_INTERVAL,  /**< High duty non-connectable adv minimum advertising interval (default: WICED_BT_CFG_DEFAULT_HIGH_DUTY_NONCONN_ADV_MIN_INTERVAL) */
        .high_duty_nonconn_max_interval  = WICED_BT_CFG_DEFAULT_HIGH_DUTY_NONCONN_ADV_MAX_INTERVAL,  /**< High duty non-connectable adv maximum advertising interval (default: WICED_BT_CFG_DEFAULT_HIGH_DUTY_NONCONN_ADV_MAX_INTERVAL) */
        .high_duty_nonconn_duration      = 30,                                                       /**< High duty non-connectable advertising duration in seconds (0 for infinite) */
        .low_duty_nonconn_min_interval   = WICED_BT_CFG_DEFAULT_LOW_DUTY_NONCONN_ADV_MIN_INTERVAL,   /**< Low duty non-connectable adv minimum advertising interval (default: WICED_BT_CFG_DEFAULT_LOW_DUTY_NONCONN_ADV_MIN_INTERVAL) */
        .low_duty_nonconn_max_interval   = WICED_BT_CFG_DEFAULT_LOW_DUTY_NONCONN_ADV_MAX_INTERVAL,   /**< Low duty non-connectable adv maximum advertising interval (default: WICED_BT_CFG_DEFAULT_LOW_DUTY_NONCONN_ADV_MAX_INTERVAL) */
        .low_duty_nonconn_duration       = 0,                                                        /**< Low duty non-connectable advertising duration in seconds (0 for infinite) */
    },

    /* GATT configuration */
    .gatt_cfg =                       /**< GATT settings */
    {
         .appearance       = APPEARANCE_GENERIC_TAG, /**< GATT appearance (see #gatt_appearance_e) */
         .client_max_links = 1,   /**< Client config: maximum number of servers that local client can connect to  */
         .server_max_links = 2,   /**< Server config: maximum number of remote clients connections allowed by the local */
         .max_attr_len     = 360, /**< Maximum attribute length; gki_cfg must have a corresponding buffer pool that can hold this length */
         .max_mtu_size     = 517, /**< Maxiimum MTU size;gki_cfg must have a corresponding buffer pool that can hold this length */
         .max_db_service_modules = 4, /** Maximum number of service modules in the DB */
         .max_gatt_bearers = 0,   /**< Maximum number of allowed gatt bearers */
         .use_gatt_over_br_edr = 0  /**< Set to 1 to enable gatt over be edr */
    },
     /* Application managed l2cap protocol configuration */
    .rfcomm_cfg =
    {
        .max_links = 0,  /**< Maximum number of simultaneous connected remote devices*/
        .max_ports = 0   /**< Maximum number of simultaneous RFCOMM ports */
    },
    .l2cap_application =
     {
         .max_links       = 0, /**< Maximum number of application-managed l2cap links (BR/EDR and LE) */
         /* BR EDR l2cap configuration */
         .max_psm         = 0, /**< Maximum number of application-managed BR/EDR PSMs */
         .max_channels    = 0, /**< Maximum number of application-managed BR/EDR channels  */

         /* LE L2cap connection-oriented channels configuration */
         .max_le_psm      = 0, /**< Maximum number of application-managed LE PSMs */
         .max_le_channels = 0, /**< Maximum number of application-managed LE channels */
        /* LE L2cap fixed channel configuration */
        .max_le_l2cap_fixed_channels    = 0,                                                           /**< Maximum number of application managed fixed channels supported (in addition to mandatory channels 4, 5 and 6). > */
        .max_ertm_chnls = 0,
        .max_ertm_tx_win = 0,

        .max_rx_mtu = 515,
     },
     /* Audio/Video Distribution configuration */
     .avdt_cfg =
     {
         .max_links = 0, /**< Maximum simultaneous audio/video links */
         .max_seps  = 0  /**< Maximum number of stream end points */
     },

     /* Audio/Video Remote Control configuration */
     .avrc_cfg =
     {
         .roles     = 0, /**< Mask of local roles supported (AVRC_CONN_INITIATOR|AVRC_CONN_ACCEPTOR) */
         .max_links = 0  /**< Maximum simultaneous remote control links */
     },

    /* LE Address Resolution DB size  */
    .addr_resolution_db_size            = 5,                                                               /**< LE Address Resolution DB settings - effective only for pre 4.2 controller*/
};

#include "cybt_platform_config.h"
#include "cybsp_types.h"
#include "cycfg.h"

const cybt_platform_config_t bt_platform_cfg_settings =
{
    .hci_config =
    {
        .hci_transport = CYBT_HCI_UART,

        .hci =
        {
            .hci_uart =
            {
                .uart_tx_pin = CYBSP_BT_UART_TX,
                .uart_rx_pin = CYBSP_BT_UART_RX,
                .uart_rts_pin = CYBSP_BT_UART_RTS,
                .uart_cts_pin = CYBSP_BT_UART_CTS,

                .baud_rate_for_fw_download = 115200,
                .baud_rate_for_feature     = 115200,

                .data_bits = 8,
                .stop_bits = 1,
                .parity = CYHAL_UART_PARITY_NONE,
                .flow_control = true
            }
        }
    },

    .controller_config =
    {
        .bt_power_pin      = CYBSP_BT_POWER,
        .sleep_mode =
        {
        // For ModusToolBox BT LPA configuration
        #if defined(CYCFG_BT_LP_ENABLED)
            .sleep_mode_enabled   = CYCFG_BT_LP_ENABLED,
            .device_wakeup_pin    = CYCFG_BT_DEV_WAKE_GPIO,
            .host_wakeup_pin      = CYCFG_BT_HOST_WAKE_GPIO,
            .device_wake_polarity = CYCFG_BT_DEV_WAKE_POLARITY,
            .host_wake_polarity   = CYCFG_BT_HOST_WAKE_IRQ_EVENT
        #else
            .sleep_mode_enabled   = CYBT_SLEEP_MODE_ENABLED,
            .device_wakeup_pin    = CYBSP_BT_DEVICE_WAKE,
            .host_wakeup_pin      = CYBSP_BT_HOST_WAKE,
            .device_wake_polarity = CYBT_WAKE_ACTIVE_LOW,
            .host_wake_polarity   = CYBT_WAKE_ACTIVE_LOW
        #endif
        }
    },

    .task_mem_pool_size    = 2048
};
