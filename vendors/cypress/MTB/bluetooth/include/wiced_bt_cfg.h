/*
 * Copyright 2014, Broadcom Corporation
 * All Rights Reserved.
 *
 * This is UNPUBLISHED PROPRIETARY SOURCE CODE of Broadcom Corporation;
 * the contents of this file may not be disclosed to third parties, copied
 * or duplicated in any form, in whole or in part, without the prior
 * written permission of Broadcom Corporation.
 */

/** @file
 *
 * Runtime Bluetooth configuration parameters
 *
 */
#pragma once
#include "data_types.h"
#include "wiced_bt_types.h"
#include "wiced_bt_dev.h"
#include "wiced_bt_ble.h"
#include "wiced_bt_gatt.h"

#ifdef __cplusplus
extern "C" {
#endif

/*****************************************************************************
 * Default configuration values
 ****************************************************************************/
#define WICED_BT_CFG_DEFAULT_INQUIRY_SCAN_INTERVAL                  0x0800      /**< Inquiry scan interval */
#define WICED_BT_CFG_DEFAULT_INQUIRY_SCAN_WINDOW                    0x0012      /**< Inquiry scan window */
#define WICED_BT_CFG_DEFAULT_PAGE_SCAN_INTERVAL                     0x0800      /**< Page scan interval  */
#define WICED_BT_CFG_DEFAULT_PAGE_SCAN_WINDOW                       0x0012      /**< Page scan window */
#define WICED_BT_CFG_DEFAULT_HIGH_DUTY_SCAN_INTERVAL                96          /**< High duty scan interval  */
#define WICED_BT_CFG_DEFAULT_HIGH_DUTY_SCAN_WINDOW                  48          /**< High duty scan window  */
#define WICED_BT_CFG_DEFAULT_LOW_DUTY_SCAN_INTERVAL                 2048        /**< Low duty scan interval  */
#define WICED_BT_CFG_DEFAULT_LOW_DUTY_SCAN_WINDOW                   18          /**< Low duty scan window */
#define WICED_BT_CFG_DEFAULT_HIGH_DUTY_CONN_SCAN_INTERVAL           96          /**< High duty cycle connection scan interval BTM_BLE_SCAN_FAST_INT*/
#define WICED_BT_CFG_DEFAULT_HIGH_DUTY_CONN_SCAN_WINDOW             48          /**< High duty cycle connection scan window BTM_BLE_SCAN_FAST_WIN */
#define WICED_BT_CFG_DEFAULT_LOW_DUTY_CONN_SCAN_INTERVAL            2048        /**< Low duty cycle connection scan interval BTM_BLE_SCAN_SLOW_INT_1 */
#define WICED_BT_CFG_DEFAULT_LOW_DUTY_CONN_SCAN_WINDOW              18          /**< Low duty cycle connection scan window BTM_BLE_SCAN_SLOW_WIN_1 */
#define WICED_BT_CFG_DEFAULT_CONN_MIN_INTERVAL                      24          /**< Minimum connection event interval */
#define WICED_BT_CFG_DEFAULT_CONN_MAX_INTERVAL                      40          /**< Maximum connection event interval */
#define WICED_BT_CFG_DEFAULT_CONN_LATENCY                           0           /**< Connection latency */
#define WICED_BT_CFG_DEFAULT_CONN_SUPERVISION_TIMEOUT               700         /**< Connection link supervsion timeout */

/* undirected connectable advertisement high/low duty cycle interval default */
#define WICED_BT_CFG_DEFAULT_HIGH_DUTY_ADV_MIN_INTERVAL             48          /**< Tgap(adv_fast_interval1) = 30(used) ~ 60 ms  = 48 *0.625 */
#define WICED_BT_CFG_DEFAULT_HIGH_DUTY_ADV_MAX_INTERVAL             48          /**< Tgap(adv_fast_interval1) = 30(used) ~ 60 ms  = 48 *0.625 */
#define WICED_BT_CFG_DEFAULT_LOW_DUTY_ADV_MIN_INTERVAL              2048        /**< Tgap(adv_slow_interval) = 1.28 s= 512 * 0.625 ms */
#define WICED_BT_CFG_DEFAULT_LOW_DUTY_ADV_MAX_INTERVAL              2048        /**< Tgap(adv_slow_interval) = 1.28 s= 512 * 0.625 ms */

/* non-connectable advertisement high/low duty cycle advertisement interval default */
#define WICED_BT_CFG_DEFAULT_HIGH_DUTY_NONCONN_ADV_MIN_INTERVAL     160         /**< Tgap(adv_fast_interval2) = 100(used) ~ 150 ms = 160 * 0.625 ms */
#define WICED_BT_CFG_DEFAULT_HIGH_DUTY_NONCONN_ADV_MAX_INTERVAL     160         /**< Tgap(adv_fast_interval2) = 100(used) ~ 150 ms = 160 * 0.625 ms */
#define WICED_BT_CFG_DEFAULT_LOW_DUTY_NONCONN_ADV_MIN_INTERVAL      2048        /**< Tgap(adv_slow_interval) = 1.28 s= 512 * 0.625 ms */
#define WICED_BT_CFG_DEFAULT_LOW_DUTY_NONCONN_ADV_MAX_INTERVAL      2048        /**< Tgap(adv_slow_interval) = 1.28 s= 512 * 0.625 ms */

/* directed connectable advertisement high/low duty cycle interval default */
#define WICED_BT_CFG_DEFAULT_HIGH_DUTY_DIRECTED_ADV_MIN_INTERVAL    400        /**< Tgap(dir_conn_adv_int_max) = 250 ms = 400 * 0.625 ms */
#define WICED_BT_CFG_DEFAULT_HIGH_DUTY_DIRECTED_ADV_MAX_INTERVAL    800        /**< Tgap(dir_conn_adv_int_min) = 500 ms = 800 * 0.625 ms */
#define WICED_BT_CFG_DEFAULT_LOW_DUTY_DIRECTED_ADV_MIN_INTERVAL     48         /**< Tgap(adv_fast_interval1) = 30(used) ~ 60 ms  = 48 *0.625 */
#define WICED_BT_CFG_DEFAULT_LOW_DUTY_DIRECTED_ADV_MAX_INTERVAL     48         /**< Tgap(adv_fast_interval1) = 30(used) ~ 60 ms  = 48 *0.625 */

/* refreshment timing interval of random private address */
#define WICED_BT_CFG_DEFAULT_RANDOM_ADDRESS_CHANGE_TIMEOUT          900        /*< default refreshment timing interval 900secs */
#define WICED_BT_CFG_DEFAULT_RANDOM_ADDRESS_NEVER_CHANGE            0          /*< value for disabling random address refresh */


/*****************************************************************************
 * Wiced_bt core stack configuration
 ****************************************************************************/

/** BR/EDR scan settings */
typedef struct
{
    uint16_t                            inquiry_scan_type;              /**< Inquiry scan type (BTM_SCAN_TYPE_STANDARD or BTM_SCAN_TYPE_INTERLACED) */
    uint16_t                            inquiry_scan_interval;          /**< Inquiry scan interval (default: WICED_BT_CFG_DEFAULT_INQUIRY_SCAN_INTERVAL) */
    uint16_t                            inquiry_scan_window;            /**< Inquiry scan window (default: WICED_BT_CFG_DEFAULT_INQUIRY_SCAN_WINDOW) */

    uint16_t                            page_scan_type;                 /**< Page scan type (BTM_SCAN_TYPE_STANDARD or BTM_SCAN_TYPE_INTERLACED) */
    uint16_t                            page_scan_interval;             /**< Page scan interval (default: WICED_BT_CFG_DEFAULT_PAGE_SCAN_INTERVAL) */
    uint16_t                            page_scan_window;               /**< Page scan window (default: WICED_BT_CFG_DEFAULT_PAGE_SCAN_WINDOW) */
} wiced_bt_cfg_br_edr_scan_settings_t;
extern const wiced_bt_cfg_br_edr_scan_settings_t *wiced_bt_cfg_br_edr_scan_settings;  /**< BR/EDR Scan settings (NULL to use defaults) */

/** LE Scan settings */
typedef struct
{
    wiced_bt_ble_scan_mode_t            scan_mode;                          /**< BLE scan mode (BTM_BLE_SCAN_MODE_PASSIVE, BTM_BLE_SCAN_MODE_ACTIVE) */

    /* Advertisement scan configuration */
    uint16_t                            high_duty_scan_interval;            /**< High duty scan interval (default: WICED_BT_CFG_DEFAULT_HIGH_DUTY_SCAN_INTERVAL) */
    uint16_t                            high_duty_scan_window;              /**< High duty scan window (default: WICED_BT_CFG_DEFAULT_HIGH_DUTY_SCAN_WINDOW) */
    uint16_t                            high_duty_scan_duration;            /**< High duty scan duration in seconds (0 for infinite) */

    uint16_t                            low_duty_scan_interval;             /**< Low duty scan interval (default: WICED_BT_CFG_DEFAULT_LOW_DUTY_SCAN_INTERVAL) */
    uint16_t                            low_duty_scan_window;               /**< Low duty scan window (default: WICED_BT_CFG_DEFAULT_LOW_DUTY_SCAN_WINDOW) */
    uint16_t                            low_duty_scan_duration;             /**< Low duty scan duration in seconds (0 for infinite) */

    /* Connection scan configuration */
    uint16_t                            high_duty_conn_scan_interval;       /**< High duty cycle connection scan interval (default: WICED_BT_CFG_DEFAULT_HIGH_DUTY_CONN_SCAN_INTERVAL) */
    uint16_t                            high_duty_conn_scan_window;         /**< High duty cycle connection scan window (default: WICED_BT_CFG_DEFAULT_HIGH_DUTY_CONN_SCAN_WINDOW) */
    uint16_t                            high_duty_conn_duration;            /**< High duty cycle connection duration in seconds (0 for infinite) */

    uint16_t                            low_duty_conn_scan_interval;        /**< Low duty cycle connection scan interval (default: WICED_BT_CFG_DEFAULT_LOW_DUTY_CONN_SCAN_INTERVAL) */
    uint16_t                            low_duty_conn_scan_window;          /**< Low duty cycle connection scan window (default: WICED_BT_CFG_DEFAULT_LOW_DUTY_CONN_SCAN_WINDOW) */
    uint16_t                            low_duty_conn_duration;             /**< Low duty cycle connection duration in seconds (0 for infinite) */

    /* Connection configuration */
    uint16_t                            conn_min_interval;                  /**< Minimum connection interval (default: WICED_BT_CFG_DEFAULT_CONN_MIN_INTERVAL) */
    uint16_t                            conn_max_interval;                  /**< Maximum connection interval (default: WICED_BT_CFG_DEFAULT_CONN_MAX_INTERVAL) */
    uint16_t                            conn_latency;                       /**< Connection latency (default: WICED_BT_CFG_DEFAULT_CONN_LATENCY) */
    uint16_t                            conn_supervision_timeout;           /**< Connection link supervision timeout (default: WICED_BT_CFG_DEFAULT_ CONN_SUPERVISION_TIMEOUT) */
} wiced_bt_cfg_ble_scan_settings_t;

/** Advertising settings */
typedef struct
{
    wiced_bt_ble_advert_chnl_map_t      channel_map;                        /**< Advertising channel map (mask of BTM_BLE_ADVERT_CHNL_37, BTM_BLE_ADVERT_CHNL_38, BTM_BLE_ADVERT_CHNL_39) */

    uint16_t                            high_duty_min_interval;             /**< High duty undirected connectable advert minimum advertising interval (default: WICED_BT_CFG_DEFAULT_HIGH_DUTY_ADV_MIN_INTERVAL) */
    uint16_t                            high_duty_max_interval;             /**< High duty undirected connectable advert maximum advertising interval (default: WICED_BT_CFG_DEFAULT_HIGH_DUTY_ADV_MAX_INTERVAL) */
    uint16_t                            high_duty_duration;                 /**< High duty advertising duration in seconds (0 for infinite) */

    uint16_t                            low_duty_min_interval;              /**< Low duty undirected connectable advert minimum advertising interval (default: WICED_BT_CFG_DEFAULT_LOW_DUTY_ADV_MIN_INTERVAL) */
    uint16_t                            low_duty_max_interval;              /**< Low duty undirected connectable advert maximum advertising interval (default: WICED_BT_CFG_DEFAULT_LOW_DUTY_ADV_MAX_INTERVAL) */
    uint16_t                            low_duty_duration;                  /**< Low duty advertising duration in seconds (0 for infinite) */

    uint16_t                            high_duty_directed_min_interval;    /**< high duty directed adv minimum advertising interval (default: WICED_BT_CFG_DEFAULT_HIGH_DUTY_DIRECTED_ADV_MIN_INTERVAL) */
    uint16_t                            high_duty_directed_max_interval;    /**< high duty directed adv maximum advertising interval (default: WICED_BT_CFG_DEFAULT_HIGH_DUTY_DIRECTED_ADV_MAX_INTERVAL) */

    uint16_t                            low_duty_directed_min_interval;     /**< Low duty directed adv minimum advertising interval (default: WICED_BT_CFG_DEFAULT_LOW_DUTY_DIRECTED_ADV_MIN_INTERVAL) */
    uint16_t                            low_duty_directed_max_interval;     /**< Low duty directed adv maximum advertising interval (default: WICED_BT_CFG_DEFAULT_LOW_DUTY_DIRECTED_ADV_MAX_INTERVAL) */
    uint16_t                            low_duty_directed_duration;         /**< Low duty directed advertising duration in seconds (0 for infinite) */

    uint16_t                            high_duty_nonconn_min_interval;     /**< High duty non-connectable adv minimum advertising interval (default: WICED_BT_CFG_DEFAULT_HIGH_DUTY_NONCONN_ADV_MIN_INTERVAL) */
    uint16_t                            high_duty_nonconn_max_interval;     /**< High duty non-connectable adv maximum advertising interval (default: WICED_BT_CFG_DEFAULT_HIGH_DUTY_NONCONN_ADV_MAX_INTERVAL) */
    uint16_t                            high_duty_nonconn_duration;         /**< High duty non-connectable advertising duration in seconds (0 for infinite) */

    uint16_t                            low_duty_nonconn_min_interval;      /**< Low duty non-connectable adv minimum advertising interval (default: WICED_BT_CFG_DEFAULT_LOW_DUTY_NONCONN_ADV_MIN_INTERVAL) */
    uint16_t                            low_duty_nonconn_max_interval;      /**< Low duty non-connectable adv maximum advertising interval (default: WICED_BT_CFG_DEFAULT_LOW_DUTY_NONCONN_ADV_MAX_INTERVAL) */
    uint16_t                            low_duty_nonconn_duration;          /**< Low duty non-connectable advertising duration in seconds (0 for infinite) */

} wiced_bt_cfg_ble_advert_settings_t;

/** GATT settings */
typedef struct
{
    wiced_bt_gatt_appearance_t          appearance;                     /**< GATT appearance (see #gatt_appearance_e) */
    uint8_t                             client_max_links;               /**< Client config: maximum number of servers that local client can connect to  */
    uint8_t                             server_max_links;               /**< Server config: maximum number of remote clients connections allowed by the local */
    uint16_t                            max_attr_len;                   /**< Maximum attribute length; gki_cfg must have a corresponding buffer pool that can hold this length */
    uint16_t                            max_mtu_size;                   /**< Maximum MTU size for GATT connections, should be between 23 and (max_attr_len + 5) */
} wiced_bt_cfg_gatt_settings_t;

/** Settings for application managed L2CAP protocols (optional) */
typedef struct
{
    uint8_t                             max_links;                      /**< Maximum number of application-managed l2cap links (BR/EDR and LE) */

    /* BR EDR l2cap configuration */
    uint8_t                             max_psm;                        /**< Maximum number of application-managed BR/EDR PSMs */
    uint8_t                             max_channels;                   /**< Maximum number of application-managed BR/EDR channels  */

    /* LE L2cap connection-oriented channels configuration */
    uint8_t                             max_le_psm;                     /**< Maximum number of application-managed LE PSMs */
    uint8_t                             max_le_channels;                /**< Maximum number of application-managed LE channels */

    /* LE L2cap fixed channel configuration */
    uint8_t                             max_le_l2cap_fixed_channels;    /**< Maximum number of application managed fixed channels supported (in addition to mandatory channels 4, 5 and 6). > */

} wiced_bt_cfg_l2cap_application_t;

/** Audio/Video Distribution configuration */
typedef struct
{
    uint8_t                             max_links;                      /**< Maximum simultaneous audio/video links */
    uint8_t                             max_seps;                       /**< Maximum number of stream end points */
} wiced_bt_cfg_avdt_t;

/** Audio/Video Remote Control configuration */
typedef struct
{
    uint8_t                             roles;                         /**< 1 if AVRC_CONN_ACCEPTOR is supported */
    uint8_t                             max_links;                     /**< Maximum simultaneous remote control links */
} wiced_bt_cfg_avrc_t;


/* RFCOMM configuration */
typedef struct
{
    uint8_t                             max_links;                      /**< Maximum number of simultaneous connected remote devices*/
    uint8_t                             max_ports;                      /**< Maximum number of simultaneous RFCOMM ports */
} wiced_bt_cfg_rfcomm_t;


/** Bluetooth stack configuration */
typedef struct
{
    uint8_t                             *device_name;                   /**< Local device name (NULL terminated) */
    wiced_bt_dev_class_t                device_class;                   /**< Local device class */
    uint8_t                             security_requirement_mask;      /**< Security requirements mask (BTM_SEC_NONE, or combination of BTM_SEC_IN_AUTHENTICATE, BTM_SEC_OUT_AUTHENTICATE, BTM_SEC_ENCRYPT (see #wiced_bt_sec_level_e)) */
    uint8_t                             max_simultaneous_links;         /**< Maximum number simultaneous links to different devices */

    /* Scan and advertisement configuration */
    wiced_bt_cfg_br_edr_scan_settings_t br_edr_scan_cfg;                /**< BR/EDR scan settings */
    wiced_bt_cfg_ble_scan_settings_t    ble_scan_cfg;                   /**< BLE scan settings */
    wiced_bt_cfg_ble_advert_settings_t  ble_advert_cfg;                 /**< BLE advertisement settings */

    /* GATT configuration */
    wiced_bt_cfg_gatt_settings_t        gatt_cfg;                       /**< GATT settings */

    /* RFCOMM configuration */
    wiced_bt_cfg_rfcomm_t               rfcomm_cfg;                     /**< RFCOMM settings */

    /* Application managed l2cap protocol configuration */
    wiced_bt_cfg_l2cap_application_t    l2cap_application;              /**< Application managed l2cap protocol configuration */

    /* Audio/Video Distribution configuration */
    wiced_bt_cfg_avdt_t                 avdt_cfg;                       /**< Audio/Video Distribution configuration */

    /* Audio/Video Remote Control configuration */
    wiced_bt_cfg_avrc_t                 avrc_cfg;                       /**< Audio/Video Remote Control configuration */

    /* LE Address Resolution DB size  */
    uint8_t                             addr_resolution_db_size;        /**< LE Address Resolution DB settings - effective only for pre 4.2 controller*/

    /* Maximum number of buffer pools */
    uint8_t                             max_number_of_buffer_pools;     /**< Maximum number of buffer pools in p_btm_cfg_buf_pools and by wiced_create_pool */

    /* Interval of  random address refreshing */
    uint16_t                            rpa_refresh_timeout;            /**< Interval of  random address refreshing - secs */

    /* BLE white list size */
    uint8_t                             ble_white_list_size;            /**< Maximum number of white list devices allowed. Cannot be more than 128 */
} wiced_bt_cfg_settings_t;

/*****************************************************************************
 * Buffer pool configuration
 *
 * Pools must be ordered in increasing buf_size.
 * If a pool runs out of buffers, the next  pool will be used
 *
 * Pool usage (4 pools):
 *      Pool 0: Small Buffer Pool   - miscellaneous use (small HCI messages)
 *      Pool 1: Medium Buffer Pool  - HCI & RFCOMM control messages, min recommended buf_size is 360
 *      Pool 2: Large Buffer Pool   - HCI ACL data messages
 *      Pool 3: Extra Large Buffer Pool - Used for avdt media packets and miscellaneous (if not needed, set buf_count to 0)
 *
 ****************************************************************************/
#define WICED_BT_CFG_NUM_BUF_POOLS          (4)                         /**< Wiced_bt stack uses 4 pools */
typedef struct {
    uint16_t                            buf_size;                       /**< size of buffers in the pool */
    uint16_t                            buf_count;                      /**< number of buffers in the pool */
} wiced_bt_cfg_buf_pool_t;

/**
 * Function         wiced_bt_print_cfg_buf_pool_stats
 *
 *                  Displays Bluetooth buffer pool usage - prints a detailed summary of application-specified
 *                  buffer pools. It also displays the statistics of internal buffer pools used by the stack.
 *
 * @param[in]
 *
 * @return
 */
void wiced_bt_print_cfg_buf_pool_stats(void);

#ifdef __cplusplus
} /* extern "C" */
#endif



/**@} wiced_bt_cfg */
