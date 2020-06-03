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
 * WICED Bluetooth Low Energy (BLE) Functions
 *
 */
#pragma once

#include "wiced_bt_dev.h"

#define CHNL_MAP_LEN    5
typedef uint8_t wiced_bt_ble_chnl_map_t[CHNL_MAP_LEN];



/* General callback function for notifying an application that a synchronous
** BTM function is complete. The pointer contains the address of any returned data.
*/
typedef void (wiced_bt_ble_compl_cback) ( void* p_data);


/** Scan modes */
enum wiced_bt_ble_scan_mode_e
{
    BTM_BLE_SCAN_MODE_PASSIVE = 0,  /**< Passive scan mode */
    BTM_BLE_SCAN_MODE_ACTIVE = 1,   /**< Active scan mode */
    BTM_BLE_SCAN_MODE_NONE = 0xff   /**< None */
};
typedef uint8_t wiced_bt_ble_scan_mode_t;   /**< scan mode (see #wiced_bt_ble_scan_mode_e) */


/** Scanner filter policy */
enum wiced_bt_ble_scanner_filter_policy_e {
    /* 0: accept adv packet from all, directed adv pkt not directed to local device is ignored */
    BTM_BLE_SCANNER_FILTER_ALL_ADV_RSP,
    /* 1: accept adv packet from device in white list, directed adv packet not directed to local device is ignored */
    BTM_BLE_SCANNER_FILTER_WHITELIST_ADV_RSP,
    /* 2: accept adv packet from all, directed adv pkt not directed to me is ignored except direct adv with RPA */
    BTM_BLE_SCANNER_FILTER_ALL_RPA_DIR_ADV_RSP,
    /* 3: accept adv packet from device in white list, directed adv pkt not directed to me is ignored except direct adv with RPA */
    BTM_BLE_SCANNER_FILTER_WHITELIST_RPA_DIR_ADV_RSP,
    BTM_BLE_SCANNER_FILTER_MAX
};
typedef uint8_t   wiced_bt_ble_scanner_filter_policy_t;  /**< Scanner filter policy (see #wiced_bt_ble_scanner_filter_policy_e) */


/** advertising channel map */
enum wiced_bt_ble_advert_chnl_map_e
{
    BTM_BLE_ADVERT_CHNL_37  = (0x01 << 0),  /**< ADV channel */
    BTM_BLE_ADVERT_CHNL_38  = (0x01 << 1),  /**< ADV channel */
    BTM_BLE_ADVERT_CHNL_39  = (0x01 << 2)   /**< ADV channel */
};
typedef uint8_t wiced_bt_ble_advert_chnl_map_t;  /**< BLE advertisement channel map (see #wiced_bt_ble_advert_chnl_map_e) */

/* default advertising channel map */
#ifndef BTM_BLE_DEFAULT_ADVERT_CHNL_MAP
#define BTM_BLE_DEFAULT_ADVERT_CHNL_MAP   (BTM_BLE_ADVERT_CHNL_37| BTM_BLE_ADVERT_CHNL_38| BTM_BLE_ADVERT_CHNL_39)
#endif

/** Advertising filter policy */
enum wiced_bt_ble_advert_filter_policy_e {
    BTM_BLE_ADVERT_FILTER_ALL_CONNECTION_REQ_ALL_SCAN_REQ               = 0x00,    /**< Process scan and connection requests from all devices (i.e., the White List is not in use) (default) */
    BTM_BLE_ADVERT_FILTER_ALL_CONNECTION_REQ_WHITELIST_SCAN_REQ         = 0x01,    /**< Process connection requests from all devices and only scan requests from devices that are in the White List. */
    BTM_BLE_ADVERT_FILTER_WHITELIST_CONNECTION_REQ_ALL_SCAN_REQ         = 0x02,    /**< Process scan requests from all devices and only connection requests from devices that are in the White List */
    BTM_BLE_ADVERT_FILTER_WHITELIST_CONNECTION_REQ_WHITELIST_SCAN_REQ   = 0x03,    /**< Process scan and connection requests only from devices in the White List. */
    BTM_BLE_ADVERT_FILTER_MAX
};
typedef uint8_t   wiced_bt_ble_advert_filter_policy_t;  /**< Advertising filter policy (see #wiced_bt_ble_advert_filter_policy_e) */

/* default advertising filter policy */
#define BTM_BLE_ADVERT_FILTER_DEFAULT   BTM_BLE_ADVERT_FILTER_ALL_CONNECTION_REQ_ALL_SCAN_REQ

/* adv parameter boundary values */
#define BTM_BLE_ADVERT_INTERVAL_MIN     0x0020
#define BTM_BLE_ADVERT_INTERVAL_MAX     0x4000

/* connection parameter boundary values */
#define BTM_BLE_SCAN_INTERVAL_MIN       0x0004
#define BTM_BLE_SCAN_INTERVAL_MAX       0x4000
#define BTM_BLE_SCAN_WINDOW_MIN         0x0004
#define BTM_BLE_SCAN_WINDOW_MAX         0x4000
#define BTM_BLE_CONN_INTERVAL_MIN       0x0006
#define BTM_BLE_CONN_INTERVAL_MAX       0x0C80
#define BTM_BLE_CONN_LATENCY_MAX        500
#define BTM_BLE_CONN_SUP_TOUT_MIN       0x000A
#define BTM_BLE_CONN_SUP_TOUT_MAX       0x0C80
#define BTM_BLE_CONN_PARAM_UNDEF        0xffff      /* use this value when a specific value not to be overwritten */
#define BTM_BLE_CONN_SUP_TOUT_DEF       700

/* default connection parameters if not configured, use GAP recommend value for auto/selective connection */
/* default scan interval */
#define BTM_BLE_SCAN_FAST_INTERVAL      96    /* 30 ~ 60 ms (use 60)  = 96 *0.625 */

/* default scan window for background connection, applicable for auto connection or selective conenction */
#define BTM_BLE_SCAN_FAST_WINDOW        48      /* 30 ms = 48 *0.625 */

/* default scan paramter used in reduced power cycle (background scanning) */
#define BTM_BLE_SCAN_SLOW_INTERVAL_1    2048    /* 1.28 s   = 2048 *0.625 */

#define BTM_BLE_SCAN_SLOW_WINDOW_1      18      /* 11.25 ms = 18 *0.625 */

/* default scan paramter used in reduced power cycle (background scanning) */
#define BTM_BLE_SCAN_SLOW_INTERVAL_2    4096    /* 2.56 s   = 4096 *0.625 */

#define BTM_BLE_SCAN_SLOW_WINDOW_2      36      /* 22.5 ms = 36 *0.625 */

/* default connection interval min */
#define BTM_BLE_CONN_INTERVAL_MIN_DEF   24      /* recommended min: 30ms  = 24 * 1.25 */

/* default connectino interval max */
#define BTM_BLE_CONN_INTERVAL_MAX_DEF   40      /* recommended max: 50 ms = 56 * 1.25 */

/* default slave latency */
#define BTM_BLE_CONN_SLAVE_LATENCY_DEF  0      /* 0 */

/* default supervision timeout */
#define BTM_BLE_CONN_TIMEOUT_DEF                    2000

#define BTM_BLE_DIR_CONN_FALLBACK_UNDIR             1
#define BTM_BLE_DIR_CONN_FALLBACK_NO_ADV            2

#define BTM_BLE_DIR_CONN_FALLBACK   BTM_BLE_DIR_CONN_FALLBACK_UNDIR

/** BLE Signature */
#define BTM_BLE_AUTH_SIGNATURE_SIZE                 12                      /**< BLE data signature length 8 Bytes + 4 bytes counter*/
typedef uint8_t wiced_dev_ble_signature_t[BTM_BLE_AUTH_SIGNATURE_SIZE];     /**< Device address (see #BTM_BLE_AUTH_SIGNATURE_SIZE) */

#define BTM_BLE_POLICY_BLACK_ALL                    0x00    /* relevant to both */
#define BTM_BLE_POLICY_ALLOW_SCAN                   0x01    /* relevant to advertiser */
#define BTM_BLE_POLICY_ALLOW_CONN                   0x02    /* relevant to advertiser */
#define BTM_BLE_POLICY_WHITE_ALL                    0x03    /* relevant to both */

/* ADV data flag bit definition used for BTM_BLE_ADVERT_TYPE_FLAG */
#define BTM_BLE_LIMITED_DISCOVERABLE_FLAG           (0x01 << 0)
#define BTM_BLE_GENERAL_DISCOVERABLE_FLAG           (0x01 << 1)
#define BTM_BLE_BREDR_NOT_SUPPORTED                 (0x01 << 2)
/* 4.1 spec adv flag for simultaneous BR/EDR+LE connection support (see) */
#define BTM_BLE_SIMULTANEOUS_DUAL_MODE_TO_SAME_DEVICE_CONTROLLER_SUPPORTED      (0x01 << 3)   /**< Simultaneous LE and BR/EDR to Same Device Capable (Controller). */
#define BTM_BLE_SIMULTANEOUS_DUAL_MODE_TO_SAME_DEVICE_HOST_SUPPORTED            (0x01 << 4)   /**< Simultaneous LE and BR/EDR to Same Device Capable (Host). */
#define BTM_BLE_NON_LIMITED_DISCOVERABLE_FLAG       (0x00 )         /* lowest bit unset */
#define BTM_BLE_ADVERT_FLAG_MASK                    (BTM_BLE_LIMITED_DISCOVERABLE_FLAG | BTM_BLE_BREDR_NOT_SUPPORTED | BTM_BLE_GENERAL_DISCOVERABLE_FLAG)
#define BTM_BLE_LIMITED_DISCOVERABLE_MASK           (BTM_BLE_LIMITED_DISCOVERABLE_FLAG )

/** Advertisement data types */
enum wiced_bt_ble_advert_type_e {
    BTM_BLE_ADVERT_TYPE_FLAG                        = 0x01,                 /**< Advertisement flags */
    BTM_BLE_ADVERT_TYPE_16SRV_PARTIAL               = 0x02,                 /**< List of supported services - 16 bit UUIDs (partial) */
    BTM_BLE_ADVERT_TYPE_16SRV_COMPLETE              = 0x03,                 /**< List of supported services - 16 bit UUIDs (complete) */
    BTM_BLE_ADVERT_TYPE_32SRV_PARTIAL               = 0x04,                 /**< List of supported services - 32 bit UUIDs (partial) */
    BTM_BLE_ADVERT_TYPE_32SRV_COMPLETE              = 0x05,                 /**< List of supported services - 32 bit UUIDs (complete) */
    BTM_BLE_ADVERT_TYPE_128SRV_PARTIAL              = 0x06,                 /**< List of supported services - 128 bit UUIDs (partial) */
    BTM_BLE_ADVERT_TYPE_128SRV_COMPLETE             = 0x07,                 /**< List of supported services - 128 bit UUIDs (complete) */
    BTM_BLE_ADVERT_TYPE_NAME_SHORT                  = 0x08,                 /**< Short name */
    BTM_BLE_ADVERT_TYPE_NAME_COMPLETE               = 0x09,                 /**< Complete name */
    BTM_BLE_ADVERT_TYPE_TX_POWER                    = 0x0A,                 /**< TX Power level  */
    BTM_BLE_ADVERT_TYPE_DEV_CLASS                   = 0x0D,                 /**< Device Class */
    BTM_BLE_ADVERT_TYPE_SM_TK                       = 0x10,                 /**< Security manager TK value */
    BTM_BLE_ADVERT_TYPE_SM_OOB_FLAG                 = 0x11,                 /**< Security manager Out-of-Band data */
    BTM_BLE_ADVERT_TYPE_INTERVAL_RANGE              = 0x12,                 /**< Slave connection interval range */
    BTM_BLE_ADVERT_TYPE_SOLICITATION_SRV_UUID       = 0x14,                 /**< List of solicitated services - 16 bit UUIDs */
    BTM_BLE_ADVERT_TYPE_128SOLICITATION_SRV_UUID    = 0x15,                 /**< List of solicitated services - 128 bit UUIDs */
    BTM_BLE_ADVERT_TYPE_SERVICE_DATA                = 0x16,                 /**< Service data - 16 bit UUID */
    BTM_BLE_ADVERT_TYPE_PUBLIC_TARGET               = 0x17,                 /**< Public target address */
    BTM_BLE_ADVERT_TYPE_RANDOM_TARGET               = 0x18,                 /**< Random target address */
    BTM_BLE_ADVERT_TYPE_APPEARANCE                  = 0x19,                 /**< Appearance */
    BTM_BLE_ADVERT_TYPE_ADVERT_INTERVAL             = 0x1a,                 /**< Advertising interval */
    BTM_BLE_ADVERT_TYPE_32SOLICITATION_SRV_UUID     = 0x1b,                 /**< List of solicitated services - 32 bit UUIDs */
    BTM_BLE_ADVERT_TYPE_32SERVICE_DATA              = 0x1c,                 /**< Service data - 32 bit UUID */
    BTM_BLE_ADVERT_TYPE_128SERVICE_DATA             = 0x1d,                 /**< Service data - 128 bit UUID */
    BTM_BLE_ADVERT_TYPE_MANUFACTURER                = 0xFF                  /**< Manufacturer data */
};
typedef uint8_t   wiced_bt_ble_advert_type_t;    /**< BLE advertisement data type (see #wiced_bt_ble_advert_type_e) */

/** security settings used with L2CAP LE COC */
enum wiced_bt_ble_sec_flags_e
{
    BTM_SEC_LE_LINK_ENCRYPTED                       = 0x01,                 /**< Link encrypted */
    BTM_SEC_LE_LINK_PAIRED_WITHOUT_MITM             = 0x02,                 /**< Paired without man-in-the-middle protection */
    BTM_SEC_LE_LINK_PAIRED_WITH_MITM                = 0x04                  /**< Link with man-in-the-middle protection */
};

typedef struct
{
    uint8_t                         *p_data;                        /**< ADV data */
    uint16_t                        len;                               /**< Advertisement length */
    wiced_bt_ble_advert_type_t      advert_type;                          /**< Advertisement data type */
}wiced_bt_ble_advert_elem_t;

/** Scan result event type */
enum wiced_bt_dev_ble_evt_type_e {
    BTM_BLE_EVT_CONNECTABLE_ADVERTISEMENT           = 0x00,                 /**< Connectable advertisement */
    BTM_BLE_EVT_CONNECTABLE_DIRECTED_ADVERTISEMENT  = 0x01,                 /**< Connectable Directed advertisement */
    BTM_BLE_EVT_SCANNABLE_ADVERTISEMENT             = 0x02,                 /**< Scannable advertisement */
    BTM_BLE_EVT_NON_CONNECTABLE_ADVERTISEMENT       = 0x03,                 /**< Non connectable advertisement */
    BTM_BLE_EVT_SCAN_RSP                            = 0x04                  /**< Scan response */
};
typedef uint8_t wiced_bt_dev_ble_evt_type_t;    /**< Scan result event value (see #wiced_bt_dev_ble_evt_type_e) */

/** Background connection type */
enum wiced_bt_ble_conn_type_e
{
    BTM_BLE_CONN_NONE,                          /**< No background connection */
    BTM_BLE_CONN_AUTO,                          /**< Auto connection */
    BTM_BLE_CONN_SELECTIVE                      /**< Selective connection */
};
typedef uint8_t wiced_bt_ble_conn_type_t;       /**< Connection type (see #wiced_bt_ble_conn_type_e) */

/** LE inquiry result type */
typedef struct
{
    wiced_bt_device_address_t       remote_bd_addr;                         /**< Device address */
    uint8_t                         ble_addr_type;                          /**< LE Address type */
    wiced_bt_dev_ble_evt_type_t     ble_evt_type;                           /**< Scan result event type */
    int8_t                          rssi;                                   /**< Set to #BTM_INQ_RES_IGNORE_RSSI, if not valid */
    uint8_t                         flag;
} wiced_bt_ble_scan_results_t;


/*  Host preferences on PHY.    */

/*  bit field that indicates the transmitter PHYs that
    the Host prefers the Controller to use.Bit number 3 -7 reserved for future.*/
#define BTM_BLE_PREFER_1M_PHY              0x01
#define BTM_BLE_PREFER_2M_PHY              0x02
#define BTM_BLE_PREFER_LELR_PHY            0x04
typedef UINT8   wiced_bt_ble_host_phy_preferences_t;

/*  The PHY_options parameter is a bit field that allows the Host to specify options
    for LE long range PHY. Default connection is with no LELR.The Controller may override any
    preferred coding (S2 coded phy for 512k speed LELR and s8 coded phy for 128K LELR) for
    transmitting on the LE Coded PHY.
    The Host may specify a preferred coding even if it prefers not to use the LE
    Coded transmitter PHY since the Controller may override the PHY preference.
    Bit 2-15 reserved for future use.
Note:-  These preferences applicable only when BTM_BLE_PREFER_LELR_PHY flag gest set */
#define BTM_BLE_PREFER_NO_LELR                         0x0000
#define BTM_BLE_PREFER_LELR_125K                       0x0001
#define BTM_BLE_PREFER_LELR_512K                       0x0002
typedef UINT16  wiced_bt_ble_lelr_phy_preferences_t;

/** Host PHY preferences */
typedef struct
{
    wiced_bt_device_address_t               remote_bd_addr;     /**< Peer Device address */
    wiced_bt_ble_host_phy_preferences_t     tx_phys;            /**< Host preference among the TX PHYs */
    wiced_bt_ble_host_phy_preferences_t     rx_phys;            /**< Host preference among the RX PHYs */
    wiced_bt_ble_lelr_phy_preferences_t     phy_opts;           /**< Host preference on LE coded PHY */
    uint8_t                                 reserved;           /**< Reserved for future use */
}wiced_bt_ble_phy_preferences_t;

/** BLE connection parameteres */
typedef struct
{
    uint8_t     role;
    uint16_t    conn_interval;
    uint16_t    conn_latency;
    uint16_t    supervision_timeout;
}wiced_bt_ble_conn_params_t;
 
/* The power table for multi ADV Tx Power levels
    Min   : -12 dBm     #define BTM_BLE_ADV_TX_POWER_MIN        0
    Low   :  -8 dBm     #define BTM_BLE_ADV_TX_POWER_LOW        1
    Mid   :  -4 dBm     #define BTM_BLE_ADV_TX_POWER_MID        2
    Upper :   0 dBm     #define BTM_BLE_ADV_TX_POWER_UPPER      3
    Max   :   4 dBm     #define BTM_BLE_ADV_TX_POWER_MAX        4
*/
#define MULTI_ADV_TX_POWER_MIN_INDEX                0
#define MULTI_ADV_TX_POWER_MAX_INDEX                4

/* Transmit Power in dBm ( MULTI_ADV_TX_POWER_MIN to MULTI_ADV_TX_POWER_MAX ) */
typedef int8_t wiced_bt_ble_adv_tx_power_t;

/** Multi-advertisement start/stop */
enum wiced_bt_ble_multi_advert_start_e
{
    MULTI_ADVERT_STOP                               = 0x00,                 /**< Stop Multi-adverstisment */
    MULTI_ADVERT_START                              = 0x01                  /**< Start Multi-adverstisment */
};

/** Multi-advertisement type */
enum wiced_bt_ble_multi_advert_type_e
{
    MULTI_ADVERT_CONNECTABLE_UNDIRECT_EVENT         = 0x00,
    MULTI_ADVERT_CONNECTABLE_DIRECT_EVENT           = 0x01,
    MULTI_ADVERT_DISCOVERABLE_EVENT                 = 0x02,
    MULTI_ADVERT_NONCONNECTABLE_EVENT               = 0x03,
    MULTI_ADVERT_LOW_DUTY_CYCLE_DIRECT_EVENT        = 0x04
};
typedef uint8_t wiced_bt_ble_multi_advert_type_t;    /**< BLE advertisement type (see #wiced_bt_ble_multi_advert_type_e) */


/* LE Multi advertising parameter */
typedef struct
{
    /**< BTM_BLE_ADVERT_INTERVAL_MIN to BTM_BLE_ADVERT_INTERVAL_MAX ( As per spec ) */
    uint16_t                             adv_int_min;            /**< Minimum adv interval */
    /**< BTM_BLE_ADVERT_INTERVAL_MIN to BTM_BLE_ADVERT_INTERVAL_MAX ( As per spec ) */
    uint16_t                             adv_int_max;            /**< Maximum adv interval */
    wiced_bt_ble_multi_advert_type_t     adv_type;               /**< Adv event type */
    wiced_bt_ble_advert_chnl_map_t       channel_map;            /**< Adv channel map */
    wiced_bt_ble_advert_filter_policy_t  adv_filter_policy;      /**< Advertising filter policy */
    wiced_bt_ble_adv_tx_power_t          adv_tx_power;           /**< Adv tx power */
    wiced_bt_device_address_t            peer_bd_addr;           /**< Peer Device address */
    wiced_bt_ble_address_type_t          peer_addr_type;         /**< Peer LE Address type */
    wiced_bt_device_address_t            own_bd_addr;            /**< Own LE address */
    wiced_bt_ble_address_type_t          own_addr_type;          /**< Own LE Address type */
}wiced_bt_ble_multi_adv_params_t;

/* Privacy mode */
enum
{
    BTM_BLE_PRIVACY_MODE_NETWORK,                           /**< network privacy mode*/
    BTM_BLE_PRIVACY_MODE_DEVICE                             /**< device privacy mode*/
};
typedef uint8_t wiced_bt_ble_privacy_mode_t;

/**
 * Callback wiced_bt_ble_selective_conn_cback_t
 *
 * Selective connection callback (registered with  #wiced_bt_ble_set_background_connection_type)
 *
 * @param remote_bda    : remote device
 * @param p_remote_name : remote device name
 *
 * @return
 */
typedef wiced_bool_t (wiced_bt_ble_selective_conn_cback_t)(wiced_bt_device_address_t remote_bda, uint8_t *p_remote_name);


/**
 * Callback wiced_bt_ble_scan_result_cback_t
 *
 * Scan result callback (from calling #wiced_bt_ble_scan)
 *
 * @param p_scan_result             : scan result data (NULL indicates end of scanning)
 * @param p_adv_data                : Advertisement data (parse using #wiced_bt_ble_check_advertising_data)
 *
 * @return Nothing
 */
typedef void (wiced_bt_ble_scan_result_cback_t) (wiced_bt_ble_scan_results_t *p_scan_result, uint8_t *p_adv_data);

/**
 * Callback wiced_bt_ble_read_phy_complete_callback_t
 *
 * read phy complete callback (from calling #wiced_bt_ble_read_phy)
 *
 * @param p_phy_result             : read phys result
 *
 * @return Nothing
 */
typedef void (wiced_bt_ble_read_phy_complete_callback_t) (wiced_bt_ble_phy_update_t *p_phy_result);

/******************************************************
 *               Function Declarations
 *
 ******************************************************/
#ifdef __cplusplus
extern "C" {
#endif

/**
 * @addtogroup  btm_ble_api_functions        BLE (Bluetooth Low Energy)
 * @ingroup     wicedbt_DeviceManagement
 *
 * BLE (Bluetooth Low Energy) Functions.
 *
 * @{
 */

/**
 *
 * Function         wiced_bt_start_advertisements
 *
 *                  Start advertising.
 *
 *                  Use #wiced_bt_ble_set_raw_advertisement_data to configure advertising data
 *                  prior to starting avertisements.
 *
 *                  The <b>advert_mode</b> parameter determines what advertising parameters and durations
 *                  to use (as specified by the application configuration).
 *
 * @param[in]       advert_mode                         : advertisement mode
 * @param[in]       directed_advertisement_bdaddr_type  : BLE_ADDR_PUBLIC or BLE_ADDR_RANDOM (if using directed advertisement mode)
 * @param[in]       directed_advertisement_bdaddr_ptr   : Directed advertisement address (NULL if not using directed advertisement)
 *
 * @return      status
 *
 */
wiced_result_t wiced_bt_start_advertisements(wiced_bt_ble_advert_mode_t advert_mode, wiced_bt_ble_address_type_t directed_advertisement_bdaddr_type, wiced_bt_device_address_ptr_t directed_advertisement_bdaddr_ptr);

/**
 *
 * Function         wiced_bt_ble_get_current_advert_mode
 *
 *                  Get current advertising mode
 *
 * @return          Current advertising mode
 *
 */
wiced_bt_ble_advert_mode_t wiced_bt_ble_get_current_advert_mode(void);


/**
 *
 * Function         wiced_bt_ble_set_raw_advertisement_data
 *
 *                  Set advertisement raw data.
 *
 * @param[in] data_mask :   number of ADV data element
 * @param[in] p_data :      advertisement raw data
 *
 * @return          void
 *
 */
wiced_result_t wiced_bt_ble_set_raw_advertisement_data(UINT8 num_elem,
                                                       wiced_bt_ble_advert_elem_t *p_data);

/**
 *
 * Function         wiced_bt_ble_set_raw_scan_response_data
 *
 *                  Set scan response raw data
 *
 * @param[in] data_mask :   number of scan response data element
 * @param[in] p_data :      scan response raw data
 *
 * @return          status of the operation
 *
 */
wiced_bt_dev_status_t wiced_bt_ble_set_raw_scan_response_data(uint8_t num_elem,
                                                        wiced_bt_ble_advert_elem_t *p_data);

/**
 *
 * Function         wiced_bt_ble_observe
 *
 *                  This function makes the device start or stop operating in the observer role.
 *                  The observer role device receives advertising events from a broadcast device.
 *
 * @param[in] start :               TRUE to start the observer role
 * @param[in] duration :            the duration for the observer role
 * @param[in] p_scan_result_cback : scan result callback
 *
 * @return          status of the operation
 *
 */
wiced_bt_dev_status_t wiced_bt_ble_observe (wiced_bool_t start, uint8_t duration, wiced_bt_ble_scan_result_cback_t *p_scan_result_cback);

/**
 * Function         wiced_bt_ble_scan
 *
 *                  Start LE scanning
 *
 *                  The <b>scan_type</b> parameter determines what scanning parameters and durations
 *                  to use (as specified by the application configuration).
 *
 *                  Scan results are notified using <b>p_scan_result_cback</b>
 *
 * @param[in]       scan_type : BTM_BLE_SCAN_TYPE_NONE, BTM_BLE_SCAN_TYPE_HIGH_DUTY,  BTM_BLE_SCAN_TYPE_LOW_DUTY
 * @param[in]       duplicate_filter_enable : TRUE or FALSE to enable or disable  duplicate filtering
 *
 * @param[in]       p_scan_result_cback : scan result callback
 *
 * @return          wiced_result_t
 *
 *                  WICED_BT_PENDING if successfully initiated
 *                  WICED_BT_BUSY if already in progress
 *                  WICED_BT_ILLEGAL_VALUE if parameter(s) are out of range
 *                  WICED_BT_NO_RESOURCES if could not allocate resources to start the command
 *                  WICED_BT_WRONG_MODE if the device is not up.
 */
wiced_result_t  wiced_bt_ble_scan (wiced_bt_ble_scan_type_t scan_type, wiced_bool_t duplicate_filter_enable, wiced_bt_ble_scan_result_cback_t *p_scan_result_cback);

/**
 *
 * Function         wiced_bt_ble_get_current_scan_state
 *
 *                  Get current scan state
 *
 * @return          wiced_bt_ble_scan_type_t
 *
 *                      BTM_BLE_SCAN_TYPE_NONE          Not scanning
 *                      BTM_BLE_SCAN_TYPE_HIGH_DUTY     High duty cycle scan
 *                      BTM_BLE_SCAN_TYPE_LOW_DUTY      Low duty cycle scan
 */
wiced_bt_ble_scan_type_t wiced_bt_ble_get_current_scan_state(void);


/**
 *
 * Function         wiced_bt_ble_security_grant
 *
 *                  Grant or deny access.  Used in response to an BTM_SECURITY_REQUEST_EVT event.
 *
 * @param[in]       bd_addr     : peer device bd address.
 * @param[in]       res         : BTM_SUCCESS to grant access; BTM_REPEATED_ATTEMPTS otherwise
 *
 * @return          <b> None </b>
 *
 */
void wiced_bt_ble_security_grant(wiced_bt_device_address_t bd_addr, uint8_t res);

/**
 *
 * Function         wiced_bt_ble_data_signature
 *
 *                  Sign the data using AES128 CMAC algorith.
 *
 * @param[in]       bd_addr: target device the data to be signed for.
 * @param[in]       p_text: signing data
 * @param[in]       len: length of the signing data
 * @param[in]       signature: output parameter where data signature is going to be stored
 *
 * @return          TRUE if signing successful, otherwise FALSE.
 *
 */
wiced_bool_t wiced_bt_ble_data_signature (wiced_bt_device_address_t bd_addr, uint8_t *p_text, uint16_t len,
                                             wiced_dev_ble_signature_t signature);

/**
 *
 * Function         wiced_bt_ble_verify_signature
 *
 *                  Verify the data signature
 *
 * @param[in]       bd_addr: target device the data to be signed for.
 * @param[in]       p_orig:  original data before signature.
 * @param[in]       len: length of the signing data
 * @param[in]       counter: counter used when doing data signing
 * @param[in]       p_comp: signature to be compared against.
 *
 * @return          TRUE if signature verified correctly; otherwise FALSE.
 *
 */
wiced_bool_t wiced_bt_ble_verify_signature (wiced_bt_device_address_t bd_addr, uint8_t *p_orig,
                                            uint16_t len, uint32_t counter,
                                            uint8_t *p_comp);

/**
 *
 * Function         wiced_bt_ble_set_background_connection_type
 *
 *                  Set BLE background connection procedure type.
 *
 * @param[in]       conn_type: BTM_BLE_CONN_NONE, BTM_BLE_CONN_AUTO, or BTM_BLE_CONN_SELECTIVE
 * @param[in]       p_select_cback: callback for BTM_BLE_CONN_SELECTIVE
 *
 * @return          TRUE if background connection set
 *
 */
wiced_bool_t wiced_bt_ble_set_background_connection_type (wiced_bt_ble_conn_type_t conn_type, wiced_bt_ble_selective_conn_cback_t *p_select_cback);

/**
 *
 * Function         wiced_bt_ble_update_background_connection_device
 *
 *                  This function is called to add or remove a device into/from
 *                  background connection procedure. The background connection
*                   procedure is decided by the background connection type, it can be
*                   auto connection, or selective connection.
 *
 * @param[in]       add_remove: TRUE to add; FALSE to remove.
 * @param[in]       remote_bda: device address to add/remove.
 *
 * @return          TRUE if successful
 *
 */
wiced_bool_t wiced_bt_ble_update_background_connection_device(wiced_bool_t add_remove, wiced_bt_device_address_t remote_bda);

/**
 *
 * Function         wiced_bt_ble_check_advertising_data
 *
 *                  Parse advertising data (returned from scan results callback #wiced_bt_ble_scan_result_cback_t).
 *                  Look for specified advertisement data type.
 *
 * @param[in]       p_adv       : pointer to advertisement data
 * @param[in]       type        : advertisement data type to look for
 * @param[out]      p_length    : length of advertisement data (if found)
 *
 * @return          pointer to start of requested advertisement data (if found). NULL if requested data type not found.
 *
 */
uint8_t *wiced_bt_ble_check_advertising_data( uint8_t *p_adv, wiced_bt_ble_advert_type_t type, uint8_t *p_length);

/**
 *
 * Function         wiced_bt_ble_get_security_state
 *
 *                  Get security mode 1 flags and encryption key size for LE peer.
 *
 * @param[in]       bd_addr         : peer address
 * @param[out]      p_le_sec_flags  : security flags (see #wiced_bt_ble_sec_flags_e)
 * @param[out]      p_le_key_size   : encryption key size
 *
 * @return          TRUE if successful
 *
 */
wiced_bool_t wiced_bt_ble_get_security_state (wiced_bt_device_address_t bd_addr, uint8_t *p_le_sec_flags, uint8_t *p_le_key_size);

/**
 *
 * Function         wiced_bt_ble_update_advertising_white_list
 *
 *                  Add or remove device from advertising white list
 *
 * @param[in]       add: TRUE to add; FALSE to remove
 * @param[in]       remote_bda: remote device address.
 *
 * @return          void
 *
 */
wiced_bool_t wiced_bt_ble_update_advertising_white_list(wiced_bool_t add, wiced_bt_device_address_t remote_bda);

/**
 *
 * Function         wiced_btm_ble_update_advertisement_filter_policy
 *
 *                  Update the filter policy of advertiser.
 *
 *  @param[in]      advertising_policy: advertising filter policy
 *
 *  @return         TRUE if successful
 */
wiced_bool_t wiced_btm_ble_update_advertisement_filter_policy(wiced_bt_ble_advert_filter_policy_t advertising_policy);


/**
 *
 * Function         wiced_bt_ble_update_scanner_white_list
 *
 *                  Add or remove device from scanner white list
 *
 * @param[in]       add: TRUE to add; FALSE to remove
 * @param[in]       remote_bda: remote device address.
 * @param[in]       addr_type   : remote device address type .
 *
 * @return          WICED_TRUE if successful else WICED_FALSE
 *
 */
wiced_bool_t wiced_bt_ble_update_scanner_white_list(wiced_bool_t add, wiced_bt_device_address_t remote_bda,  wiced_bt_ble_address_type_t addr_type);

/**
 *
 * Function         wiced_bt_ble_update_scanner_filter_policy
 *
 *                  Update the filter policy of scanning.
 *
 *  @param[in]      scanner_policy: scanning filter policy
 *
 *  @return         void
 */
void wiced_bt_ble_update_scanner_filter_policy(wiced_bt_ble_scanner_filter_policy_t scanner_policy);



/**
 *
 * Function         wiced_bt_ble_clear_white_list
 *
 *                     Request clearing white list in controller side
 *
 * @return         TRUE if request of clear is sent to controller side
 *
 */
wiced_bool_t wiced_bt_ble_clear_white_list(void);

/**
 *
 * Function         wiced_bt_ble_get_white_list_size
 *
 *                     Returns size of white list size in controller side
 *
 *
 * @return          size of whitelist in current controller
 *
 */
uint8_t wiced_bt_ble_get_white_list_size(void);

/**
 *
 * Function         wiced_bt_ble_set_adv_tx_power
 *
 *                  This command will adjust the transmit power attenuation of the advertisements.
 *
 * @param[in]   power  : Input power to be set , range should be between -28 to 4 dbm
 *                     Min Val is 4dbm and Max val is -28
*
 * @return      wiced_result_t
 *                  WICED_BT_SUCCESS if successful
 *                  WICED_BT_NO_RESOURCES if could not allocate resources to start the command
 *                  WICED_BT_UNSUPPORTED if command is not supported by bluetooth stack
 *
 */
wiced_result_t wiced_bt_ble_set_adv_tx_power( int power );

/**
 *
 * Function         wiced_bt_ble_read_adv_tx_power
 *
 *                  This command retrieves the transmit power attenuation of the advertisements.
 *
 * @param[in]   wiced_bt_ble_compl_cback  :  callback function returns the set  power
 *
 * @return       wiced_result_t
 *                   WICED_BT_SUCCESS if successfully initiated
 *                   WICED_BT_NO_RESOURCES if could not allocate resources to start the command
 *                   WICED_BT_UNSUPPORTED if command is not supported by bluetooth stack
 *
 */
wiced_result_t wiced_bt_ble_read_adv_tx_power( wiced_bt_ble_compl_cback *p_cb );

/**
 * Function         wiced_bt_ble_read_phy
 *
 *                  Host to read the current transmitter PHY and receiver PHY on 
 *                  the connection identified by the remote bdaddr.
 *
 *                  phy results notified using <b>p_read_phy_complete_callback</b>
 *
 * @param[in]       wiced_bt_device_address_t                   - remote device address
 * @param[in]       wiced_bt_ble_read_phy_complete_callback_t   - read phy complete callback

 * @return          wiced_result_t
 *
 *                  WICED_BT_SUCCESS is returned if the request was successfully sent to HCI.
 *                  WICED_BT_UNKNOWN_ADDR  if device address does not correspond to a connected remote device
 *                  WICED_BT_ILLEGAL_VALUE if p_read_phy_complete_callback is NULL
 *                  WICED_BT_NO_RESOURCES if could not allocate resources to start the command
 *
 */
wiced_bt_dev_status_t wiced_bt_ble_read_phy (wiced_bt_device_address_t remote_bd_addr,
                        wiced_bt_ble_read_phy_complete_callback_t *p_read_phy_complete_callback);

/**
 * Function         wiced_bt_ble_set_default_phy
 *
 *                  Host to configure default transmitter phy and receiver phy to
 *                  be used for all subsequent connections over the LE transport.
 *
 *
 * @param[in]       wiced_bt_ble_phy_preferences_t      - Phy preferences
 *
 * Note : remote_bd_addr field of the phy_preferences is ignored.
 *
 * @return          wiced_result_t
 *
 *                  WICED_BT_SUCCESS is returned if the request was successfully sent to HCI.
 *                  WICED_BT_ILLEGAL_VALUE if phy_preferences is NULL
 *                  WICED_BT_NO_RESOURCES if could not allocate resources to start the command
 *
 */
wiced_bt_dev_status_t wiced_bt_ble_set_default_phy (wiced_bt_ble_phy_preferences_t *phy_preferences);

/**
 * Function         wiced_bt_ble_set_phy
 *
 *                  Host to configure the LE link to 1M or 2M and LE coding to be used
 *
 * @param[in]       wiced_bt_ble_phy_preferences_t      - Phy preferences
 *
 * @return          wiced_result_t
 *
 *                  WICED_BT_SUCCESS is returned if the request was successfully sent to HCI.
 *                  WICED_BT_ILLEGAL_VALUE if phy_preferences is NULL
 *                  WICED_BT_UNKNOWN_ADDR if device address does not correspond to a connected remote device
 *                  WICED_BT_NO_RESOURCES if could not allocate resources to start the command
 *
 */
wiced_bt_dev_status_t wiced_bt_ble_set_phy (wiced_bt_ble_phy_preferences_t *phy_preferences);


/*
 * Function         wiced_start_multi_advertisements
 *
 *                  Start/Stop Mulit advertisements.
 *                  wiced_start_multi_advertisements gives option to start multiple adverstisment instances
 *                  Each of the instances can set different #wiced_set_multi_advertisement_params and #wiced_set_multi_advertisement_data.
 *                  Hence this feature allows the device to advertise to multiple masters at the same time like a multiple peripheral device,
 *                  with different advertising data, Random private addresses, tx_power etc.
 *
 * @param[in]       advertising_enable : MULTI_ADVERT_START  - Advertising is enabled
 *                                       MULTI_ADVERT_STOP   - Advertising is disabled
 *
 * @param[in]       adv_instance       : 1 to MULTI_ADV_MAX_NUM_INSTANCES
 *
 * @return          wiced_bt_dev_status_t
 *
 *                  TRUE if command succeeded
 */
 wiced_bt_dev_status_t wiced_start_multi_advertisements( uint8_t advertising_enable, uint8_t adv_instance );

/**
 * Function         wiced_set_multi_advertisement_params
 *
 *                  Set multi advertisement parameters for each adv_instance
 *
 *                  All the parameters below refered to as "spec" is BT 4.2 core specification, page 1277 (LE Set Advertising Parameter Command)
 *
 * @param[in]       adv_instance                : 1 to MULTI_ADV_MAX_NUM_INSTANCES
 * @param[in]       p_param                : pointer to the adv parameter structure.
 *
 * @return          wiced_bt_dev_status_t
 *
 *                  TRUE if command succeeded
 */
wiced_bt_dev_status_t wiced_set_multi_advertisement_params( uint8_t adv_instance, wiced_bt_ble_multi_adv_params_t *p_param );

/**
 * Function         wiced_set_multi_advertisement_data
 *
 *                  Set multi advertisement data for each adv_instance
 *
 *
 * @param[in]       p_data        : Advertising Data ( Max length 31 bytess)
 * @param[in]       data_len      : Advertising Data len ( Max 31 bytes )
 * @param[in]       adv_instance  : 1 to MULTI_ADV_MAX_NUM_INSTANCES
 *
 * @return          wiced_bt_dev_status_t
 *
 *                  TRUE if command succeeded
 */
wiced_bt_dev_status_t wiced_set_multi_advertisement_data( uint8_t * p_data, uint8_t data_len, uint8_t adv_instance );

/**
 * Function         wiced_set_multi_advertisements_random_address
 *
 *                  Set multi advertisement random address for an instance
 *
 *
 * @param[in]       randomAddr    : own random address
 * @param[in]       adv_instance  : 1 to MULTI_ADV_MAX_NUM_INSTANCES
 *
 * @return          wiced_bt_dev_status_t
 *
 *                  TRUE if command succeeded
 */
wiced_bt_dev_status_t wiced_set_multi_advertisements_random_address( wiced_bt_device_address_t randomAddr, uint8_t adv_instance );

/**
 * Function         wiced_set_multi_advertisement_scan_response_data
 *
 *                  Set multi advertisement data for each adv_instance
 *
 *
 * @param[in]       p_data        : Advertising Data ( Max length 31 bytess)
 * @param[in]       data_len      : Advertising Data len ( Max 31 bytes )
 * @param[in]       adv_instance  : 1 to MULTI_ADV_MAX_NUM_INSTANCES
 *
 * @return          wiced_bt_dev_status_t
 *
 *                  TRUE if command succeeded
 */
wiced_bt_dev_status_t wiced_set_multi_advertisement_scan_response_data( uint8_t * p_data, uint8_t data_len, uint8_t adv_instance );

/**
 * Function         wiced_bt_notify_multi_advertisement_packet_transmissions
 *
 *                  Allows the application to register a callback that will be invoked
 *                  just before an ADV packet is about to be sent out and immediately after.
 *
 * @param[in]       adv_instance                : 1 to MULTI_ADV_MAX_NUM_INSTANCES
 * @param[in]       clientCallback              : Pointer to a function that will be invoked in application thread context 
 *                  with WICED_BT_ADV_NOTIFICATION_READY for before ADV and WICED_BT_ADV_NOTIFICATION_DONE after ADV packet is complete.
 * @param[in]       advanceNoticeInMicroSeconds : Number of microseconds before the ADV the notification is to be sent. Will be rounded down to
 *                  the nearest 1.25mS. Has to be an even multiple of 625uS.
 *
 * @return          wiced_bool_t
 *
 *                  TRUE if command succeeded
 */
wiced_bool_t wiced_bt_notify_multi_advertisement_packet_transmissions( uint8_t adv_instance, void (*clientCallback)( uint32_t ),
                                                                       uint32_t advanceNoticeInMicroSeconds );

/*******************************************************************************
**
* Function         BTM_BleSetPrivacyMode
*
* Description      updates privacy mode if device is already available in controller resolving list

* @param[in]       remote_bda      -remote device address received during connection up
* @param[in]       privacy_mode      - privacy mode (see #wiced_bt_ble_privacy_mode_t)

* @return          wiced_bt_dev_status_t

*               WICED_BT_ILLEGAL_VALUE if paramer is wrong
*               WICED_BT_UNSUPPORTED if command not supported
*               WICED_BT_UNKNOWN_ADDR if bd_addr is wrong
*               WICED_BT_ILLEGAL_ACTION if device not added to resolving list or peer irk is not valid
*               WICED_BT_ERROR, error while processing the command
*               WICED_BT_SUCCESS if command started
*
*******************************************************************************/
wiced_bt_dev_status_t wiced_bt_ble_set_privacy_mode(wiced_bt_device_address_t remote_bda, wiced_bt_ble_privacy_mode_t privacy_mode);
/**@} btm_ble_api_functions */

#ifdef __cplusplus
}
#endif
