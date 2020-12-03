/*
 * $ Copyright Cypress Semiconductor $
 */

/** @file
 *
 * WICED Bluetooth Low Energy (BLE) Functions
 *
 */
#pragma once

/**
 * @if DUAL_MODE
 * @addtogroup  btm_ble_api_functions        BLE (Bluetooth Low Energy)
 * @ingroup  wicedbt_DeviceManagement
 * This section describes the API's to use BLE functionality such as advertisement, scanning
 * BLE Connection, Data transfer, BLE Security etc.
 * @else
 * @addtogroup  wicedbt_DeviceManagement
 * @endif
 *
 *
 * @{
 */

#include "wiced_bt_dev.h"
#include "wiced_bt_ble_isoc.h"


#define BTM_AFH_CHNL_MAP_SIZE    HCI_AFH_CHANNEL_MAP_LEN    /**< AFH channel map size */
#define BLE_CHANNEL_MAP_LEN      5                          /**< AFH Channel Map len */
/** BLE Channel Map */
typedef uint8_t wiced_bt_ble_chnl_map_t[BLE_CHANNEL_MAP_LEN];

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
    BTM_BLE_SCANNER_FILTER_ALL_ADV_RSP,              /**< accept adv packet from all, directed adv pkt not directed to local device is ignored */
    BTM_BLE_SCANNER_FILTER_WHITELIST_ADV_RSP,        /**< accept adv packet from device in white list, directed adv packet not directed to local device is ignored */
    BTM_BLE_SCANNER_FILTER_ALL_RPA_DIR_ADV_RSP,      /**< accept adv packet from all, directed adv pkt not directed to local device is ignored except direct adv with RPA */
    BTM_BLE_SCANNER_FILTER_WHITELIST_RPA_DIR_ADV_RSP,/**< accept adv packet from device in white list, directed adv pkt not directed to me is ignored except direct adv with RPA */
    BTM_BLE_SCANNER_FILTER_MAX                       /**< Max Scan filter policy value */
};
/** BLE Scanner filter policy */
typedef uint8_t   wiced_bt_ble_scanner_filter_policy_t;  /**< Scanner filter policy (see #wiced_bt_ble_scanner_filter_policy_e) */

/** advertising channel map */
enum wiced_bt_ble_advert_chnl_map_e
{
    BTM_BLE_ADVERT_CHNL_37  = (0x01 << 0),  /**< ADV channel */
    BTM_BLE_ADVERT_CHNL_38  = (0x01 << 1),  /**< ADV channel */
    BTM_BLE_ADVERT_CHNL_39  = (0x01 << 2)   /**< ADV channel */
};
typedef uint8_t wiced_bt_ble_advert_chnl_map_t;  /**< BLE advertisement channel map (see #wiced_bt_ble_advert_chnl_map_e) */

/** default advertising channel map */
#ifndef BTM_BLE_DEFAULT_ADVERT_CHNL_MAP
#define BTM_BLE_DEFAULT_ADVERT_CHNL_MAP   (BTM_BLE_ADVERT_CHNL_37| BTM_BLE_ADVERT_CHNL_38| BTM_BLE_ADVERT_CHNL_39)
#endif

/** Advertising filter policy */
enum wiced_bt_ble_advert_filter_policy_e {
    BTM_BLE_ADVERT_FILTER_ALL_CONNECTION_REQ_ALL_SCAN_REQ               = 0x00,    /**< Process scan and connection requests from all devices (i.e., the White List is not in use) (default) */
    BTM_BLE_ADVERT_FILTER_ALL_CONNECTION_REQ_WHITELIST_SCAN_REQ         = 0x01,    /**< Process connection requests from all devices and only scan requests from devices that are in the White List. */
    BTM_BLE_ADVERT_FILTER_WHITELIST_CONNECTION_REQ_ALL_SCAN_REQ         = 0x02,    /**< Process scan requests from all devices and only connection requests from devices that are in the White List */
    BTM_BLE_ADVERT_FILTER_WHITELIST_CONNECTION_REQ_WHITELIST_SCAN_REQ   = 0x03,    /**< Process scan and connection requests only from devices in the White List. */
    BTM_BLE_ADVERT_FILTER_MAX                                                      /**< Max Adv filter value */
};
typedef uint8_t   wiced_bt_ble_advert_filter_policy_t;  /**< Advertising filter policy (see #wiced_bt_ble_advert_filter_policy_e) */

/** default advertising filter policy */
#define BTM_BLE_ADVERT_FILTER_DEFAULT   BTM_BLE_ADVERT_FILTER_ALL_CONNECTION_REQ_ALL_SCAN_REQ

#define BTM_BLE_ADVERT_INTERVAL_MIN     0x0020  /**< adv parameter Min value */
#define BTM_BLE_ADVERT_INTERVAL_MAX     0x4000  /**< adv parameter Max value */

#define BTM_BLE_SCAN_INTERVAL_MIN       0x0004  /**< Scan interval minimum value */
#define BTM_BLE_SCAN_INTERVAL_MAX       0x4000  /**< Scan interval miximum value */
#define BTM_BLE_SCAN_WINDOW_MIN         0x0004  /**< Scan window minimum value */
#define BTM_BLE_SCAN_WINDOW_MAX         0x4000  /**< Scan window maximum value */
#define BTM_BLE_CONN_INTERVAL_MIN       0x0006  /**< Connection interval minimum value */
#define BTM_BLE_CONN_INTERVAL_MAX       0x0C80  /**< Connection interval maximum value */
#define BTM_BLE_CONN_LATENCY_MAX        500     /**< Maximum Connection Latency */
#define BTM_BLE_CONN_SUP_TOUT_MIN       0x000A  /**< Minimum Supervision Timeout */
#define BTM_BLE_CONN_SUP_TOUT_MAX       0x0C80  /**< Maximum Supervision Timeout */
#define BTM_BLE_CONN_PARAM_UNDEF        0xffff  /**< use this value when a specific value not to be overwritten */
#define BTM_BLE_CONN_SUP_TOUT_DEF       700     /**< Default Supervision Timeout */

/* default connection parameters if not configured, use GAP recommend value for auto/selective connection */
/** default scan interval 
 *  30 ~ 60 ms (use 60)  = 96 *0.625
 */
#define BTM_BLE_SCAN_FAST_INTERVAL      96

/** default scan window for background connection, applicable for auto connection or selective conenction 
 * 30 ms = 48 *0.625 
 */
#define BTM_BLE_SCAN_FAST_WINDOW        48

/** default scan interval used in reduced power cycle (background scanning) 
 *  1.28 s   = 2048 *0.625
 */
#define BTM_BLE_SCAN_SLOW_INTERVAL_1    2048

/** default scan window used in reduced power cycle (background scanning) 
 *   11.25 ms = 18 *0.625 
 */
#define BTM_BLE_SCAN_SLOW_WINDOW_1      18

/** default scan interval used in reduced power cycle (background scanning) 
 *  2.56 s   = 4096 *0.625 
 */
#define BTM_BLE_SCAN_SLOW_INTERVAL_2    4096

/** default scan window used in reduced power cycle (background scanning) 
 *  22.5 ms = 36 *0.625
 */
#define BTM_BLE_SCAN_SLOW_WINDOW_2      36

/** default connection interval min 
 *  recommended min: 30ms  = 24 * 1.25 
 */
#define BTM_BLE_CONN_INTERVAL_MIN_DEF   24

/** default connectino interval max
 * recommended max: 50 ms = 56 * 1.25 
 */
#define BTM_BLE_CONN_INTERVAL_MAX_DEF   40

/** default slave latency */
#define BTM_BLE_CONN_SLAVE_LATENCY_DEF  0

/** default supervision timeout */
#define BTM_BLE_CONN_TIMEOUT_DEF                    2000

/** BLE Signature 
 *  BLE data signature length 8 Bytes + 4 bytes counter
 */
#define BTM_BLE_AUTH_SIGNATURE_SIZE                 12
typedef uint8_t wiced_dev_ble_signature_t[BTM_BLE_AUTH_SIGNATURE_SIZE];     /**< Device address (see #BTM_BLE_AUTH_SIGNATURE_SIZE) */

#define BTM_BLE_POLICY_BLACK_ALL                    0x00    /**< relevant to both */
#define BTM_BLE_POLICY_ALLOW_SCAN                   0x01    /**< relevant to advertiser */
#define BTM_BLE_POLICY_ALLOW_CONN                   0x02    /**< relevant to advertiser */
#define BTM_BLE_POLICY_WHITE_ALL                    0x03    /**< relevant to both */

/* ADV data flag bit definition used for BTM_BLE_ADVERT_TYPE_FLAG */
#define BTM_BLE_LIMITED_DISCOVERABLE_FLAG           (0x01 << 0)     /**< Limited Discoverable */
#define BTM_BLE_GENERAL_DISCOVERABLE_FLAG           (0x01 << 1)     /**< General Discoverable */
#define BTM_BLE_BREDR_NOT_SUPPORTED                 (0x01 << 2)     /**< BR/EDR Not Supported */
/* 4.1 spec adv flag for simultaneous BR/EDR+LE connection support (see) */
#define BTM_BLE_SIMULTANEOUS_DUAL_MODE_TO_SAME_DEVICE_CONTROLLER_SUPPORTED      (0x01 << 3)   /**< Simultaneous LE and BR/EDR to Same Device Capable (Controller). */
#define BTM_BLE_SIMULTANEOUS_DUAL_MODE_TO_SAME_DEVICE_HOST_SUPPORTED            (0x01 << 4)   /**< Simultaneous LE and BR/EDR to Same Device Capable (Host). */
#define BTM_BLE_NON_LIMITED_DISCOVERABLE_FLAG       (0x00 )         /**< Non Discoverable */
#define BTM_BLE_ADVERT_FLAG_MASK                    (BTM_BLE_LIMITED_DISCOVERABLE_FLAG | BTM_BLE_BREDR_NOT_SUPPORTED | BTM_BLE_GENERAL_DISCOVERABLE_FLAG) /**< BLE adverisement mask */
#define BTM_BLE_LIMITED_DISCOVERABLE_MASK           (BTM_BLE_LIMITED_DISCOVERABLE_FLAG )    /**< BLE Limited discovery mask*/


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
    BTM_BLE_ADVERT_TYPE_SIMPLE_PAIRING_HASH_C       = 0x0E,                 /**< Simple Pairing Hash C */
    BTM_BLE_ADVERT_TYPE_SIMPLE_PAIRING_RAND_C       = 0x0F,                 /**< Simple Pairing Randomizer R */
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
    BTM_BLE_ADVERT_TYPE_LE_BD_ADDR                  = 0x1b,                 /**< LE device bluetooth address */
    BTM_BLE_ADVERT_TYPE_LE_ROLE                     = 0x1c,                 /**< LE role */
    BTM_BLE_ADVERT_TYPE_256SIMPLE_PAIRING_HASH      = 0x1d,                 /**< Simple Pairing Hash C-256 */
    BTM_BLE_ADVERT_TYPE_256SIMPLE_PAIRING_RAND      = 0x1e,                 /**< Simple Pairing Randomizer R-256 */
    BTM_BLE_ADVERT_TYPE_32SOLICITATION_SRV_UUID     = 0x1f,                 /**< List of solicitated services - 32 bit UUIDs */
    BTM_BLE_ADVERT_TYPE_32SERVICE_DATA              = 0x20,                 /**< Service data - 32 bit UUID */
    BTM_BLE_ADVERT_TYPE_128SERVICE_DATA             = 0x21,                 /**< Service data - 128 bit UUID */
    BTM_BLE_ADVERT_TYPE_CONN_CONFIRM_VAL            = 0x22,                 /**< LE Secure Connections Confirmation Value */
    BTM_BLE_ADVERT_TYPE_CONN_RAND_VAL               = 0x23,                 /**< LE Secure Connections Random Value */
    BTM_BLE_ADVERT_TYPE_URI                         = 0x24,                 /**< URI */
    BTM_BLE_ADVERT_TYPE_INDOOR_POS                  = 0x25,                 /**< Indoor Positioning */
    BTM_BLE_ADVERT_TYPE_TRANS_DISCOVER_DATA         = 0x26,                 /**< Transport Discovery Data */
    BTM_BLE_ADVERT_TYPE_SUPPORTED_FEATURES          = 0x27,                 /**< LE Supported Features */
    BTM_BLE_ADVERT_TYPE_UPDATE_CH_MAP_IND           = 0x28,                 /**< Channel Map Update Indication */
    BTM_BLE_ADVERT_TYPE_PB_ADV                      = 0x29,                 /**< PB-ADV */
    BTM_BLE_ADVERT_TYPE_MESH_MSG                    = 0x2A,                 /**< Mesh Message */
    BTM_BLE_ADVERT_TYPE_MESH_BEACON                 = 0x2B,                 /**< Mesh Beacon */
    BTM_BLE_ADVERT_TYPE_PSRI                        = 0x2E,                 /**< Generic Audio Provate Set Random Identifier */
    BTM_BLE_ADVERT_TYPE_3D_INFO_DATA                = 0x3D,                 /**< 3D Information Data */
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

/** Advertisement element */
typedef struct
{
    uint8_t                     *p_data;        /**< Advertisement data */
    uint16_t                    len;            /**< Advertisement length */
    wiced_bt_ble_advert_type_t  advert_type;    /**< Advertisement data type */
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
    uint8_t                         flag;                                   /**< Adverisement Flag value */
} wiced_bt_ble_scan_results_t;

/** LE encryption method **/
enum wiced_bt_ble_sec_action_type_e
{
    BTM_BLE_SEC_NONE,               /**< No encryption */
    BTM_BLE_SEC_ENCRYPT,            /**< encrypt the link using current key */
    BTM_BLE_SEC_ENCRYPT_NO_MITM,    /**< encryption without MITM */
    BTM_BLE_SEC_ENCRYPT_MITM        /**< encryption with MITM*/
};
typedef uint8_t wiced_bt_ble_sec_action_type_t;  /**< BLE security type. refer #wiced_bt_ble_sec_action_type_e */

#define BTM_BLE_PREFER_1M_PHY              0x01    /**< LE 1M PHY preference */
#define BTM_BLE_PREFER_2M_PHY              0x02    /**< LE 2M PHY preference */
#define BTM_BLE_PREFER_LELR_PHY            0x04    /**< LE LELR PHY preference */

/**  Host preferences on PHY.    
 *  bit field that indicates the transmitter PHYs that
 *  the Host prefers the Controller to use.Bit number 3 -7 reserved for future.
 */
typedef uint8_t   wiced_bt_ble_host_phy_preferences_t;

/**  The PHY_options parameter is a bit field that allows the Host to specify options
 *    for LE long range PHY. Default connection is with no LE coded PHY.The Controller may override any
 *    preferred coding (S2 coded phy for 512k speed and s8 coded phy for 128K) for
 *    transmitting on the LE Coded PHY.
 *    The Host may specify a preferred coding even if it prefers not to use the LE
 *    Coded transmitter PHY since the Controller may override the PHY preference.
 *    Bit 2-15 reserved for future use.
 *  @note  These preferences applicable only when BTM_BLE_PREFER_LELR_PHY flag gest set 
 */
enum wiced_bt_ble_lelr_phy_e {
    BTM_BLE_PREFER_NO_LELR      = 0x0000,
    BTM_BLE_PREFER_LELR_125K    = 0x0001,
    BTM_BLE_PREFER_LELR_512K    = 0x0002
};
/** BLE LELR phy preference. refer #wiced_bt_ble_lelr_phy_e */
typedef uint16_t  wiced_bt_ble_lelr_phy_preferences_t;

/** Host PHY preferences */
typedef struct
{
    wiced_bt_device_address_t               remote_bd_addr;     /**< Peer Device address */
    wiced_bt_ble_host_phy_preferences_t     tx_phys;            /**< Host preference among the TX PHYs */
    wiced_bt_ble_host_phy_preferences_t     rx_phys;            /**< Host preference among the RX PHYs */
    wiced_bt_ble_lelr_phy_preferences_t     phy_opts;           /**< Host preference on LE coded PHY */
}wiced_bt_ble_phy_preferences_t;

/** BLE connection parameteres */
typedef struct
{
    uint8_t     role;                   /**< Connection role 0:MASTER 1: Slave */
    uint16_t    conn_interval;          /**< Connection interval in slots */
    uint16_t    conn_latency;           /**< Connection latency */
    uint16_t    supervision_timeout;    /**< Supervision timeout */
}wiced_bt_ble_conn_params_t;

/* The power table for multi ADV Tx Power levels
    Min   : -12 dBm     #define BTM_BLE_ADV_TX_POWER_MIN        0
    Low   :  -8 dBm     #define BTM_BLE_ADV_TX_POWER_LOW        1
    Mid   :  -4 dBm     #define BTM_BLE_ADV_TX_POWER_MID        2
    Upper :   0 dBm     #define BTM_BLE_ADV_TX_POWER_UPPER      3
    Max   :   4 dBm     #define BTM_BLE_ADV_TX_POWER_MAX        4
*/
#define MULTI_ADV_TX_POWER_MIN_INDEX                0   /**< Multi adv tx min power index */
#define MULTI_ADV_TX_POWER_MAX_INDEX                4   /**< Multi adv tx max power index */

/** Transmit Power in dBm ( #MULTI_ADV_TX_POWER_MIN_INDEX to #MULTI_ADV_TX_POWER_MAX_INDEX ) */
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
    MULTI_ADVERT_CONNECTABLE_UNDIRECT_EVENT         = 0x00,     /**< Multi adv Connectable undirected event */
    MULTI_ADVERT_CONNECTABLE_DIRECT_EVENT           = 0x01,     /**< Multi adv Connectable directed event */
    MULTI_ADVERT_DISCOVERABLE_EVENT                 = 0x02,     /**< Multi adv Discoverable event */
    MULTI_ADVERT_NONCONNECTABLE_EVENT               = 0x03,     /**< Multi adv NonConnectable event */
    MULTI_ADVERT_LOW_DUTY_CYCLE_DIRECT_EVENT        = 0x04      /**< Multi adv Low Cycle directed event */
};
typedef uint8_t wiced_bt_ble_multi_advert_type_t;    /**< BLE advertisement type (see #wiced_bt_ble_multi_advert_type_e) */


/** LE Multi advertising parameter */
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

/** Privacy mode 
 * refer Spec version 5.0 Vol 3 Part C Section 10.7 privacy feature
 */
enum wiced_bt_ble_privacy_e
{
    BTM_BLE_PRIVACY_MODE_NETWORK,                           /**< network privacy mode*/
    BTM_BLE_PRIVACY_MODE_DEVICE                             /**< device privacy mode*/
};
/** BLE Privacy mode. Refer #wiced_bt_ble_privacy_e */
typedef uint8_t wiced_bt_ble_privacy_mode_t;

/** Multi-advertisement Filtering policy  */
enum wiced_bt_ble_multi_advert_filtering_policy_e
{
    MULTI_ADVERT_FILTER_POLICY_WHITE_LIST_NOT_USED                    = 0x00,   /**< Multi adv filter white list not used */
    MULTI_ADVERT_WHITE_LIST_POLICY_ADV_ALLOW_UNKNOWN_CONNECTION       = 0x01,   /**< Multi adv filter white list for scan request */
    MULTI_ADVERT_WHITE_LIST_POLICY_ADV_ALLOW_UNKNOWN_SCANNING         = 0x02,   /**< Multi adv filter white list for connection request */
    MULTI_ADVERT_FILTER_POLICY_WHITE_LIST_USED_FOR_ALL                = 0x03    /**< Multi adv filter white list for all */
};
typedef uint8_t wiced_bt_ble_multi_advert_filtering_policy_t;    /**< BLE advertisement filtering policy (see #wiced_bt_ble_multi_advert_filtering_policy_e) */

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
 * @addtogroup  btm_ble_adv_scan_functions        Advertisement & Scan
 * @ingroup     btm_ble_api_functions
 *
 * This section provides functions for BLE advertisement and BLE scan operations.
 *
 * @{
 */

/**
 * Start advertising.
 *
 * Use #wiced_bt_ble_set_raw_advertisement_data to configure advertising data
 * prior to starting avertisements. The advertisements are stopped upon successful LE connection establishment.
 *
 * The <b>advert_mode</b> parameter determines what advertising parameters and durations
 * to use (as specified by the application configuration).
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
 * Get current advertising mode
 *
 * @return          Current advertising mode (refer #wiced_bt_ble_advert_mode_e)
 *
 */
wiced_bt_ble_advert_mode_t wiced_bt_ble_get_current_advert_mode(void);


/**
 *
 * Set advertisement raw data.
 *
 * @param[in] num_elem :   number of ADV data element
 * @param[in] p_data :      advertisement raw data
 *
 * @return          void
 *
 */
wiced_result_t wiced_bt_ble_set_raw_advertisement_data(uint8_t num_elem,
                                                       wiced_bt_ble_advert_elem_t *p_data);

/**
 * Parse advertising data (returned from scan results callback #wiced_bt_ble_scan_result_cback_t).
 * Look for specified advertisement data type.
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
 * Update the filter policy of advertiser.
 *
 *  @param[in]      advertising_policy: advertising filter policy
 *
 *  @return         TRUE if successful
 */
wiced_bool_t wiced_btm_ble_update_advertisement_filter_policy(wiced_bt_ble_advert_filter_policy_t advertising_policy);

/**
*  Command to set LE Advertisement tx power
*
* @param[in]       power          :  power value in db
* @param[in]       p_cb           :  Result callback (wiced_bt_set_adv_tx_power_result_t will be passed to the callback)
*
* @return          wiced_result_t
*
**/
wiced_result_t wiced_bt_ble_set_adv_tx_power(int8_t power, wiced_bt_dev_vendor_specific_command_complete_cback_t *p_cb);

/**
* Read LE Advertisement transmit power
*
* @param[in]       p_cback         : Result callback (wiced_bt_tx_power_result_t will be passed to the callback)
*
* @return
*
*                  WICED_BT_PENDING if command issued to controller.
*                  WICED_BT_NO_RESOURCES if couldn't allocate memory to issue command
*                  WICED_BT_BUSY if command is already in progress
*
*/
wiced_result_t wiced_bt_ble_read_adv_tx_power(wiced_bt_dev_cmpl_cback_t *p_cback);

/**
 *
 * Set scan response raw data
 *
 * @param[in] num_elem :   number of scan response data element
 * @param[in] p_data :      scan response raw data
 *
 * @return          status of the operation
 *
 */
wiced_bt_dev_status_t wiced_bt_ble_set_raw_scan_response_data(uint8_t num_elem,
                                                        wiced_bt_ble_advert_elem_t *p_data);

/**
 *
 * This function makes the device start or stop operating in the observer role.
 * The observer role device receives advertising events from a broadcast device.
 *
 * @param[in] start :               TRUE to start the observer role
 * @param[in] duration :            the duration for the observer role
 * @param[in] p_scan_result_cback : scan result callback
 *
 * @return          status of the operation
 *
 * Note : It will use Low Duty Scan configuration
 *
 */
wiced_bt_dev_status_t wiced_bt_ble_observe (wiced_bool_t start, uint8_t duration, wiced_bt_ble_scan_result_cback_t *p_scan_result_cback);

/**
 * Start LE scanning
 *
 * The <b>scan_type</b> parameter determines what scanning parameters and durations
 *                  to use (as specified by the application configuration).
 *
 *                  Scan results are notified using <b>p_scan_result_cback</b>
 *
 * @param[in]       scan_type : BTM_BLE_SCAN_TYPE_NONE, BTM_BLE_SCAN_TYPE_HIGH_DUTY,  BTM_BLE_SCAN_TYPE_LOW_DUTY
 * @param[in]       duplicate_filter_enable : TRUE or FALSE to enable or disable  duplicate filtering
 *
 * @param[in]       p_scan_result_cback : scan result callback
 *
 * @return          wiced_result_t \n
 *
 * <b> WICED_BT_PENDING </b>        : if successfully initiated \n
 * <b> WICED_BT_BUSY </b>           : if already in progress \n
 * <b> WICED_BT_ILLEGAL_VALUE </b>  : if parameter(s) are out of range \n
 * <b> WICED_BT_NO_RESOURCES </b>   : if could not allocate resources to start the command \n
 * <b> WICED_BT_WRONG_MODE </b>     : if the device is not up.
 */
wiced_result_t  wiced_bt_ble_scan (wiced_bt_ble_scan_type_t scan_type, wiced_bool_t duplicate_filter_enable, wiced_bt_ble_scan_result_cback_t *p_scan_result_cback);

/**
 *
 * Get current scan state
 *
 * @return          wiced_bt_ble_scan_type_t \n
 *
 * <b> BTM_BLE_SCAN_TYPE_NONE </b>         Not scanning \n
 * <b> BTM_BLE_SCAN_TYPE_HIGH_DUTY </b>    High duty cycle scan \n
 * <b> BTM_BLE_SCAN_TYPE_LOW_DUTY </b>     Low duty cycle scan
 */
wiced_bt_ble_scan_type_t wiced_bt_ble_get_current_scan_state(void);

/**
 *
 * Update the filter policy of scanning.
 *
 *  @param[in]      scanner_policy: scanning filter policy
 *
 *  @return         void
 */
void wiced_bt_ble_update_scanner_filter_policy(wiced_bt_ble_scanner_filter_policy_t scanner_policy);

/**@} btm_ble_adv_scan_functions */

/**
 * @addtogroup  btm_ble_conn_whitelist_functions        Connection and Whitelist
 * @ingroup     btm_ble_api_functions
 *
 * This section provides functions for BLE connection related and whitelist operations.
 * @note For whitelist operation, the controller should support this.
 *
 * @{
 */

/**
 *
 * Set BLE background connection procedure type.
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
 * This function is called to add or remove a device into/from
 * background connection procedure. The background connection
 * procedure is decided by the background connection type, it can be
 * auto connection, or selective connection.
 *
 * @param[in]       add_remove: TRUE to add; FALSE to remove.
 * @param[in]       remote_bda: device address to add/remove.
 *
 * @return          TRUE if successful
 *
 */
wiced_bool_t wiced_bt_ble_update_background_connection_device(wiced_bool_t add_remove, wiced_bt_device_address_t remote_bda);

/**
 * To read LE connection parameters based on connection address received in gatt connection up indication.
 *
 * @param[in]       remote_bda          : remote device address.
 * @param[in]       p_conn_parameters   : Connection Parameters
 *
 * @return          wiced_result_t \n
 *
 * <b> WICED_BT_ILLEGAL_VALUE </b> : if p_conn_parameters is NULL. \n
 * <b> WICED_BT_UNKNOWN_ADDR </b>  : if device address is bad. \n
 * <b> WICED_BT_SUCCESS </b> otherwise.
 *
 */
wiced_result_t wiced_bt_ble_get_connection_parameters(wiced_bt_device_address_t remote_bda, wiced_bt_ble_conn_params_t *p_conn_parameters);

/**
 *
 * Add or remove device from advertising white list
 *
 * @param[in]       add: TRUE to add; FALSE to remove
 * @param[in]       remote_bda: remote device address.
 *
 * @return          wiced_bool_t (<b> WICED_TRUE </b> if successful else <b> WICED_FALSE </b>)
 *
 */
wiced_bool_t wiced_bt_ble_update_advertising_white_list(wiced_bool_t add, wiced_bt_device_address_t remote_bda);

/**
 *
 * Add or remove device from scanner white list
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
 * Request clearing white list in controller side
 *
 *
 * @return          TRUE if request of clear is sent to controller side
 *
 */
wiced_bool_t wiced_bt_ble_clear_white_list(void);

/**
 *
 * Returns size of white list size in controller side
 *
 *
 * @return          size of whitelist in current controller
 *
 */
uint8_t wiced_bt_ble_get_white_list_size(void);

/**@} btm_ble_conn_whitelist_functions */

/**
 * @addtogroup  btm_ble_phy_functions        Phy
 * @ingroup     btm_ble_api_functions
 *
 * This section provides functionality to read and update PHY.
 *
 * @{
 */

/**
 * Host to read the current transmitter PHY and receiver PHY on the connection identified by the remote bdaddr.
 * phy results notified using #wiced_bt_ble_read_phy_complete_callback_t callback
 *
 * @param[in]       remote_bd_addr                   - remote device address
 * @param[in]       p_read_phy_complete_callback     - read phy complete callback
 *
 * @return          wiced_result_t \n
 *
 * <b> WICED_BT_SUCCESS </b>        : if the request was successfully sent to HCI. \n
 * <b> WICED_BT_UNKNOWN_ADDR </b>   : if device address does not correspond to a connected remote device \n
 * <b> WICED_BT_ILLEGAL_VALUE </b>  : if p_read_phy_complete_callback is NULL \n
 * <b> WICED_BT_NO_RESOURCES </b>   : if could not allocate resources to start the command
 *
 */
wiced_bt_dev_status_t wiced_bt_ble_read_phy (wiced_bt_device_address_t remote_bd_addr,
                        wiced_bt_ble_read_phy_complete_callback_t *p_read_phy_complete_callback);

/**
 * Host to configure default transmitter phy and receiver phy to
 * be used for all subsequent connections over the LE transport.
 *
 *
 * @param[in]       phy_preferences      - Phy preferences
 *
 * Note : remote_bd_addr field of the phy_preferences is ignored.
 *
 * @return          wiced_result_t
 *
 * <b> WICED_BT_SUCCESS </b>        : if the request was successfully sent to HCI. \n
 * <b> WICED_BT_ILLEGAL_VALUE </b>  : if phy_preferences is NULL \n
 * <b> WICED_BT_NO_RESOURCES </b>   : if could not allocate resources to start the command
 *
 */
wiced_bt_dev_status_t wiced_bt_ble_set_default_phy (wiced_bt_ble_phy_preferences_t *phy_preferences);

/**
 * Host to configure the LE link to 1M or 2M and LE coding to be used
 *
 * @param[in]       phy_preferences      - Phy preferences
 *
 * @return          wiced_result_t \n
 *
 * <b> WICED_BT_SUCCESS </b>        : if the request was successfully sent to HCI. \n
 * <b> WICED_BT_ILLEGAL_VALUE </b>  : if phy_preferences is NULL \n
 * <b> WICED_BT_UNKNOWN_ADDR </b>   : if device address does not correspond to a connected remote device \n
 * <b> WICED_BT_NO_RESOURCES </b>   : if could not allocate resources to start the command
 *
 */
wiced_result_t wiced_bt_ble_set_phy (wiced_bt_ble_phy_preferences_t *phy_preferences);

/**
 * Set channel classification for the available 40 channels.
 *
 * Channel n is bad = 0.
 * Channel n is unknown = 1.
 *
 * At least one channel shall be marked as unknown.
 *
 * @param[in]       ble_channel_map
 *
 * @return          wiced_result_t \n
 *
 * <b> WICED_BT_SUCCESS </b>        if successfully initiated \n
 * <b> WICED_BT_NO_RESOURCES </b>   if could not allocate resources to start the command
 */
wiced_result_t wiced_bt_ble_set_channel_classification(const wiced_bt_ble_chnl_map_t ble_channel_map);
/**@} btm_ble_phy_functions */

/**
 * @addtogroup  btm_ble_multi_adv_functions        MultiAdv
 * @ingroup     btm_ble_api_functions
 *
 * This section describes Multiple Advertisement API, using this interface application can enable more than one advertisement train.
 * @note Controller should have support for this feature.
 *
 * @{
 */

/**
 * Start/Stop Mulit advertisements.
 * wiced_start_multi_advertisements gives option to start multiple adverstisment instances
 * Each of the instances can set different #wiced_set_multi_advertisement_params and #wiced_set_multi_advertisement_data.
 * Hence this feature allows the device to advertise to multiple masters at the same time like a multiple peripheral device,
 * with different advertising data, Random private addresses, tx_power etc.
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
 * Set multi advertisement data for each adv_instance
 *
 *
 * @param[in]       p_data        : Advertising Data ( Max length 31 bytess)
 * @param[in]       data_len      : Advertising Data len ( Max 31 bytes )
 * @param[in]       adv_instance  : 1 to MULTI_ADV_MAX_NUM_INSTANCES
 *
 * @return          wiced_bt_dev_status_t \n
 *                  WICED_BT_SUCCESS if command succeeded
 */
wiced_bt_dev_status_t wiced_set_multi_advertisement_data( uint8_t * p_data, uint8_t data_len, uint8_t adv_instance );

/**
 * Set multi advertisement params for each adv_instance
 *
 *
 * @param[in]       adv_instance  : 1 to MULTI_ADV_MAX_NUM_INSTANCES
 * @param[in]       params        : Advertising params refer #wiced_bt_ble_multi_adv_params_t
 *
 * @return          wiced_bt_dev_status_t \n
 *                  WICED_BT_SUCCESS if command succeeded
 */

wiced_bt_dev_status_t wiced_set_multi_advertisement_params(uint8_t adv_instance, wiced_bt_ble_multi_adv_params_t *params);


/**
 * Set multi advertisement data for scan response
 *
 * @param[in]       p_data        : Advertising Data ( Max length 31 bytess)
 * @param[in]       data_len      : Advertising Data len ( Max 31 bytes )
 * @param[in]       adv_instance  : 1 to MULTI_ADV_MAX_NUM_INSTANCES
 *
 * @return          wiced_bt_dev_status_t \n
 *                  WICED_BT_SUCCESS if command succeeded
 */
wiced_bt_dev_status_t wiced_set_multi_advertisement_scan_response_data( uint8_t * p_data, uint8_t data_len, uint8_t adv_instance );

/**
 * Set multi advertisement random address for an instance
 *
 *
 * @param[in]       randomAddr    : own random address
 * @param[in]       adv_instance  : 1 to MULTI_ADV_MAX_NUM_INSTANCES
 *
 * @return          wiced_bt_dev_status_t \n
 *                  WICED_BT_SUCCESS if command succeeded
 */
wiced_bt_dev_status_t wiced_set_multi_advertisements_random_address( wiced_bt_device_address_t randomAddr, uint8_t adv_instance );

/**
 * Allows the application to register a callback that will be invoked
 * just before an ADV packet is about to be sent out and immediately after.
 *
 * @param[in]       adv_instance                : 1 to MULTI_ADV_MAX_NUM_INSTANCES
 * @param[in]       clientCallback              : Pointer to a function that will be invoked in application thread context
 *                  with WICED_BT_ADV_NOTIFICATION_READY for before ADV and WICED_BT_ADV_NOTIFICATION_DONE after ADV packet is complete.
 * @param[in]       advanceNoticeInMicroSeconds : Number of microseconds before the ADV the notification is to be sent. Will be rounded down to
 *                  the nearest 1.25mS. Has to be an even multiple of 625uS.
 *
 * @return          wiced_bool_t \n
 *                  WICED_TRUE if command succeeded
 */
wiced_bool_t wiced_bt_notify_multi_advertisement_packet_transmissions( uint8_t adv_instance, void (*clientCallback)( uint32_t ),
                                                                       uint32_t advanceNoticeInMicroSeconds );

/**@} btm_ble_multi_adv_functions */

/**
 * @} btm_ble_api_functions 
 */


/**
 * @ingroup     btm_ble_sec_api_functions
 *
 * @{
 */

/**
 *
 * Grant or deny access.  Used in response to an BTM_SECURITY_REQUEST_EVT event.
 *
 * @param[in]       bd_addr     : peer device bd address.
 * @param[in]       res         : BTM_SUCCESS to grant access;
                                  BTM_MODE_UNSUPPORTED, if local device does not allow pairing;
                                  BTM_REPEATED_ATTEMPTS otherwise
 *
 * @return          <b> None </b>
 *
 */
void wiced_bt_ble_security_grant(wiced_bt_device_address_t bd_addr, wiced_bt_dev_status_t res);

/**
 * Sign the data using AES128 CMAC algorith.
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
 * Verify the data signature
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
 * Get security mode 1 flags and encryption key size for LE peer.
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
 * Updates privacy mode if device is already available in controller resolving list
 *
 * @param[in]       remote_bda      -remote device address received during connection up
 * @param[in]       rem_bda_type    -remote device address type received during connection up
 * @param[in]       privacy_mode    - privacy mode (see #wiced_bt_ble_privacy_mode_t)
 * @return          wiced_bt_dev_status_t \n
 * <b> WICED_BT_ILLEGAL_VALUE </b>  : if paramer is wrong \n
 * <b> WICED_BT_UNSUPPORTED </b>    : if command not supported \n
 * <b> WICED_BT_UNKNOWN_ADDR </b>   : if bd_addr is wrong \n
 * <b> WICED_BT_ILLEGAL_ACTION </b> : if device not added to resolving list or peer irk is not valid \n
 * <b> WICED_BT_ERROR </b>      : error while processing the command \n
 * <b> WICED_BT_SUCCESS</b>     : if command started
 *
 */
wiced_bt_dev_status_t wiced_bt_ble_set_privacy_mode(wiced_bt_device_address_t remote_bda, wiced_bt_ble_address_type_t rem_bda_type, wiced_bt_ble_privacy_mode_t privacy_mode);

/**@} btm_ble_api_functions */

#ifdef __cplusplus
}
#endif
