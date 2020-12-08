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
 * Bluetooth Management (BTM) Application Programming Interface
 *
 * The BTM consists of several management entities:
 *      1. Device Control - controls the local device
 *      2. Device Discovery - manages inquiries, discover database
 *      3. ACL Channels - manages ACL connections (BR/EDR and LE)
 *      4. SCO Channels - manages SCO connections
 *      5. Security - manages all security functionality
 *      6. Power Management - manages park, sniff, hold, etc.
 *
 * @defgroup wicedbt      Bluetooth
 *
 * WICED Bluetooth Framework Functions
 */

#pragma once

#include "wiced_bt_types.h"
#include "hcidefs.h"
//#include "rtos.h"
#include "wiced_result.h"
#include "bt_target.h"

/** Result/Status for wiced_bt_dev */
typedef wiced_result_t  wiced_bt_dev_status_t;      /**< Result/Status for wiced_bt_dev */

/** Structure returned with Vendor Specific Command complete callback */
typedef struct
{
    uint16_t    opcode;                     /**< Vendor specific command opcode */
    uint16_t    param_len;                  /**< Return parameter length        */
    uint8_t     *p_param_buf;               /**< Return parameter buffer        */
} wiced_bt_dev_vendor_specific_command_complete_params_t;


/*****************************************************************************
 *  DEVICE DISCOVERY - Inquiry, Remote Name, Discovery, Class of Device
 *****************************************************************************/
/* BR/EDR Discoverable modes */
#ifndef BTM_DISCOVERABILITY_MODE
#define BTM_DISCOVERABILITY_MODE
enum wiced_bt_discoverability_mode_e {
    BTM_NON_DISCOVERABLE            = 0,        /**< Non discoverable */
    BTM_LIMITED_DISCOVERABLE        = 1,        /**< Limited BR/EDR discoverable */
    BTM_GENERAL_DISCOVERABLE        = 2         /**< General BR/EDR discoverable */
};
#define BTM_DISCOVERABLE_MASK       (BTM_LIMITED_DISCOVERABLE|BTM_GENERAL_DISCOVERABLE)
#define BTM_MAX_DISCOVERABLE        BTM_GENERAL_DISCOVERABLE
#endif /* BTM_DISCOVERABILITY_MODE */

/* BR/EDR Connectable modes */
#ifndef BTM_CONNECTABILITY_MODE
#define BTM_CONNECTABILITY_MODE
enum wiced_bt_connectability_mode_e {
    BTM_NON_CONNECTABLE             = 0,        /**< Not connectable */
    BTM_CONNECTABLE                 = 1         /**< BR/EDR connectable */
};
#define BTM_CONNECTABLE_MASK        (BTM_NON_CONNECTABLE | BTM_CONNECTABLE)
#endif /* BTM_CONNECTABILITY_MODE */

/* Inquiry modes
 * Note: These modes are associated with the inquiry active values (BTM_*ACTIVE)
 */
#ifndef BTM_INQUIRY_MODE            /* To avoid redefintions when including wiced_bt_dev.h */
#define BTM_INQUIRY_MODE
enum wiced_bt_inquiry_mode_e {
    BTM_INQUIRY_NONE                = 0,        /**< Stop inquiry */
    BTM_GENERAL_INQUIRY             = 0x01,     /**< General inquiry */
    BTM_LIMITED_INQUIRY             = 0x02,     /**< Limited inquiry */
    BTM_BR_INQUIRY_MASK             = (BTM_GENERAL_INQUIRY | BTM_LIMITED_INQUIRY)
};
#endif /* BTM_INQUIRY_MODE */

/* Define scan types */
#define BTM_SCAN_TYPE_STANDARD      0
#define BTM_SCAN_TYPE_INTERLACED    1

/* Inquiry results mode */
#define BTM_INQ_RESULT              0
#define BTM_INQ_RESULT_WITH_RSSI    1
#define BTM_INQ_RESULT_EXTENDED     2

#define BTM_INQ_RES_IGNORE_RSSI     0x7f    /**< RSSI value not supplied (ignore it) */
#define BTM_SCAN_PARAM_IGNORE       0       /* Passed to BTM_SetScanConfig() to ignore */

/* Inquiry Filter Condition types (see wiced_bt_dev_inq_parms_t) */
#ifndef BTM_INQUIRY_FILTER
#define BTM_INQUIRY_FILTER
enum wiced_bt_dev_filter_cond_e {
    BTM_CLR_INQUIRY_FILTER          = 0,                            /**< No inquiry filter */
    BTM_FILTER_COND_DEVICE_CLASS    = HCI_FILTER_COND_DEVICE_CLASS, /**< Filter on device class */
    BTM_FILTER_COND_BD_ADDR         = HCI_FILTER_COND_BD_ADDR,      /**< Filter on device addr */
};
#endif /* BTM_INQUIRY_FILTER */

/* State of the remote name retrieval during inquiry operations.
 * Used in the wiced_bt_dev_inq_info_t structure, and returned in the
 * BTM_InqDbRead, BTM_InqDbFirst, and BTM_InqDbNext functions.
 * The name field is valid when the state returned is
 * BTM_INQ_RMT_NAME_DONE */
#define BTM_INQ_RMT_NAME_EMPTY      0
#define BTM_INQ_RMT_NAME_PENDING    1
#define BTM_INQ_RMT_NAME_DONE       2
#define BTM_INQ_RMT_NAME_FAILED     3


/** BTM service definitions (used for storing EIR data to bit mask */
#ifndef BTM_EIR_UUID_ENUM
#define BTM_EIR_UUID_ENUM
enum
{
    BTM_EIR_UUID_SERVCLASS_SERVICE_DISCOVERY_SERVER,
    BTM_EIR_UUID_SERVCLASS_SERIAL_PORT,
    BTM_EIR_UUID_SERVCLASS_LAN_ACCESS_USING_PPP,
    BTM_EIR_UUID_SERVCLASS_DIALUP_NETWORKING,
    BTM_EIR_UUID_SERVCLASS_IRMC_SYNC,
    BTM_EIR_UUID_SERVCLASS_OBEX_OBJECT_PUSH,
    BTM_EIR_UUID_SERVCLASS_OBEX_FILE_TRANSFER,
    BTM_EIR_UUID_SERVCLASS_IRMC_SYNC_COMMAND,
    BTM_EIR_UUID_SERVCLASS_HEADSET,
    BTM_EIR_UUID_SERVCLASS_CORDLESS_TELEPHONY,
    BTM_EIR_UUID_SERVCLASS_AUDIO_SOURCE,
    BTM_EIR_UUID_SERVCLASS_AUDIO_SINK,
    BTM_EIR_UUID_SERVCLASS_AV_REM_CTRL_TARGET,
    BTM_EIR_UUID_SERVCLASS_AV_REMOTE_CONTROL,
    BTM_EIR_UUID_SERVCLASS_INTERCOM,
    BTM_EIR_UUID_SERVCLASS_FAX,
    BTM_EIR_UUID_SERVCLASS_HEADSET_AUDIO_GATEWAY,
    BTM_EIR_UUID_SERVCLASS_PANU,
    BTM_EIR_UUID_SERVCLASS_NAP,
    BTM_EIR_UUID_SERVCLASS_GN,
    BTM_EIR_UUID_SERVCLASS_DIRECT_PRINTING,
    BTM_EIR_UUID_SERVCLASS_IMAGING,
    BTM_EIR_UUID_SERVCLASS_IMAGING_RESPONDER,
    BTM_EIR_UUID_SERVCLASS_IMAGING_AUTO_ARCHIVE,
    BTM_EIR_UUID_SERVCLASS_IMAGING_REF_OBJECTS,
    BTM_EIR_UUID_SERVCLASS_HF_HANDSFREE,
    BTM_EIR_UUID_SERVCLASS_AG_HANDSFREE,
    BTM_EIR_UUID_SERVCLASS_DIR_PRT_REF_OBJ_SERVICE,
    BTM_EIR_UUID_SERVCLASS_BASIC_PRINTING,
    BTM_EIR_UUID_SERVCLASS_PRINTING_STATUS,
    BTM_EIR_UUID_SERVCLASS_HUMAN_INTERFACE,
    BTM_EIR_UUID_SERVCLASS_CABLE_REPLACEMENT,
    BTM_EIR_UUID_SERVCLASS_HCRP_PRINT,
    BTM_EIR_UUID_SERVCLASS_HCRP_SCAN,
    BTM_EIR_UUID_SERVCLASS_SAP,
    BTM_EIR_UUID_SERVCLASS_PBAP_PCE,
    BTM_EIR_UUID_SERVCLASS_PBAP_PSE,
    BTM_EIR_UUID_SERVCLASS_PHONE_ACCESS,
    BTM_EIR_UUID_SERVCLASS_HEADSET_HS,
    BTM_EIR_UUID_SERVCLASS_PNP_INFORMATION,
    BTM_EIR_UUID_SERVCLASS_VIDEO_SOURCE,
    BTM_EIR_UUID_SERVCLASS_VIDEO_SINK,
    BTM_EIR_UUID_SERVCLASS_MESSAGE_ACCESS,
    BTM_EIR_UUID_SERVCLASS_MESSAGE_NOTIFICATION,
    BTM_EIR_UUID_SERVCLASS_HDP_SOURCE,
    BTM_EIR_UUID_SERVCLASS_HDP_SINK,
    BTM_EIR_MAX_SERVICES
};
#endif  /* BTM_EIR_UUID_ENUM */


/************************************************************************************************
 * BTM Services MACROS handle array of uint32_t bits for more than 32 services
 ************************************************************************************************/
/* Determine the number of uint32_t's necessary for services */
#define BTM_EIR_ARRAY_BITS          32          /* Number of bits in each array element */
#ifndef BTM_EIR_SERVICE_ARRAY_SIZE
#define BTM_EIR_SERVICE_ARRAY_SIZE  (((uint32_t)BTM_EIR_MAX_SERVICES / BTM_EIR_ARRAY_BITS) + \
                                    (((uint32_t)BTM_EIR_MAX_SERVICES % BTM_EIR_ARRAY_BITS) ? 1 : 0))
#endif

/***************************
 *  Device Discovery Types
 ****************************/
/** Class of Device inquiry filter */
typedef struct
{
    wiced_bt_dev_class_t            dev_class;          /**< class of device */
    wiced_bt_dev_class_t            dev_class_mask;     /**< class of device filter mask */
} wiced_bt_dev_cod_cond_t;

/** Inquiry filter */
typedef union
{
    wiced_bt_device_address_t       bdaddr_cond;        /**< bluetooth address filter */
    wiced_bt_dev_cod_cond_t         cod_cond;           /**< class of device filter */
} wiced_bt_dev_inq_filt_cond_t;

/** Inquiry Parameters */
typedef struct
{
    uint8_t                         mode;               /**< Inquiry mode (see #wiced_bt_inquiry_mode_e) */
    uint8_t                         duration;           /**< Inquiry duration (1.28 sec increments) */
    uint8_t                         filter_cond_type;   /**< Inquiry filter type  (see #wiced_bt_dev_filter_cond_e) */
    wiced_bt_dev_inq_filt_cond_t    filter_cond;        /**< Inquiry filter */
} wiced_bt_dev_inq_parms_t;

/** Inquiry Results */
typedef struct
{
    uint16_t                        clock_offset;                           /**< Clock offset */
    wiced_bt_device_address_t       remote_bd_addr;                         /**< Device address */
    wiced_bt_dev_class_t            dev_class;                              /**< Class of device */
    uint8_t                         page_scan_rep_mode;                     /**< Page scan repetition mode */
    uint8_t                         page_scan_per_mode;                     /**< Page scan per mode */
    uint8_t                         page_scan_mode;                         /**< Page scan mode */
    int8_t                          rssi;                                   /**< Receive signal strength index (#BTM_INQ_RES_IGNORE_RSSI, if not available) */
    uint32_t                        eir_uuid[BTM_EIR_SERVICE_ARRAY_SIZE];   /**< Array or EIR UUIDs */
    wiced_bool_t                    eir_complete_list;                      /**< TRUE if EIR array is complete */
} wiced_bt_dev_inquiry_scan_result_t;

/* Structure returned with remote name request */
typedef struct
{
    uint16_t                        status;
    wiced_bt_device_address_t       bd_addr;
    uint16_t                        length;
    wiced_bt_remote_name_t          remote_bd_name;
} wiced_bt_dev_remote_name_result_t;

/** RSSI Result (in response to #wiced_bt_dev_read_rssi) */
typedef struct
{
    wiced_result_t                  status;             /**< Status of the operation */
    uint8_t                         hci_status;         /**< Status from controller */
    int8_t                          rssi;               /**< RSSI */
    wiced_bt_device_address_t       rem_bda;            /**< Remote BD address */
} wiced_bt_dev_rssi_result_t;

/** TX Power Result (in response to #wiced_bt_dev_read_tx_power) */
typedef struct
{
    wiced_result_t                  status;             /**< Status of the operation */
    uint8_t                         hci_status;         /**< Status from controller */
    int8_t                          tx_power;           /**< TX power */
    wiced_bt_device_address_t       rem_bda;            /**< Remote BD address */
} wiced_bt_tx_power_result_t;

/*****************************************************************************
 *  SECURITY MANAGEMENT
 *****************************************************************************/

/** Security Service Levels [bit mask]. Encryption should not be used without authentication. */
#ifndef BTM_SEC_LEVEL
#define BTM_SEC_LEVEL
enum wiced_bt_sec_level_e
{
    BTM_SEC_NONE                    = 0x0000,    /**< Nothing required */
    BTM_SEC_IN_AUTHENTICATE         = 0x0002,    /**< Inbound call requires authentication */
    BTM_SEC_OUT_AUTHENTICATE        = 0x0010,    /**< Outbound call requires authentication */
    BTM_SEC_ENCRYPT                 = 0x0024,    /**< Requires encryption (inbound and outbound) */
    BTM_SEC_SECURE_CONNECTION       = 0x0040     /**< Secure Connections Mode (P-256 based Secure Simple Pairing and Authentication) */
};
#endif /* BTM_SEC_LEVEL */

/** security flags for current BR/EDR link */
#ifndef BTM_SEC_LINK_STATE              /* To avoid redefintions when including wiced_bt_dev.h */
#define BTM_SEC_LINK_STATE
enum wiced_bt_sec_flags_e
{
    BTM_SEC_LINK_ENCRYPTED                       = 0x01,                 /**< Link encrypted */
    BTM_SEC_LINK_PAIRED_WITHOUT_MITM             = 0x02,                 /**< Paired without man-in-the-middle protection */
    BTM_SEC_LINK_PAIRED_WITH_MITM                = 0x04                  /**< Link with man-in-the-middle protection */
};
#endif /* BTM_SEC_LINK_STATE */

/** PIN types */
#define BTM_PIN_TYPE_VARIABLE       HCI_PIN_TYPE_VARIABLE
#define BTM_PIN_TYPE_FIXED          HCI_PIN_TYPE_FIXED

/** Size of security keys */
#ifndef BTM_SECURITY_KEY_DATA_LEN
#define BTM_SECURITY_KEY_DATA_LEN       132     /**< Security key data length (used by wiced_bt_device_link_keys_t structure) */
#endif

#ifndef BTM_SECURITY_LOCAL_KEY_DATA_LEN
#define BTM_SECURITY_LOCAL_KEY_DATA_LEN 65      /**< Local security key data length (used by wiced_bt_local_identity_keys_t structure) */
#endif

/** Pairing IO Capabilities */
enum wiced_bt_dev_io_cap_e
{
    BTM_IO_CAPABILITIES_DISPLAY_ONLY,                   /**< Display Only        */
    BTM_IO_CAPABILITIES_DISPLAY_AND_YES_NO_INPUT,       /**< Display Yes/No      */
    BTM_IO_CAPABILITIES_KEYBOARD_ONLY,                  /**< Keyboard Only       */
    BTM_IO_CAPABILITIES_NONE,                           /**< No Input, No Output */
    BTM_IO_CAPABILITIES_BLE_DISPLAY_AND_KEYBOARD_INPUT, /**< Keyboard display (For BLE SMP) */
    BTM_IO_CAPABILITIES_MAX
};
typedef uint8_t wiced_bt_dev_io_cap_t;          /**< IO capabilities (see #wiced_bt_dev_io_cap_e) */

/** BR/EDR Authentication requirement */
enum wiced_bt_dev_auth_req_e {
    BTM_AUTH_SINGLE_PROFILE_NO = 0,                     /**< MITM Protection Not Required - Single Profile/non-bonding. Numeric comparison with automatic accept allowed */
    BTM_AUTH_SINGLE_PROFILE_YES = 1,                    /**< MITM Protection Required - Single Profile/non-bonding. Use IO Capabilities to determine authentication procedure */
    BTM_AUTH_ALL_PROFILES_NO = 2,                       /**< MITM Protection Not Required - All Profiles/dedicated bonding. Numeric comparison with automatic accept allowed */
    BTM_AUTH_ALL_PROFILES_YES = 3,                      /**< MITM Protection Required - All Profiles/dedicated bonding. Use IO Capabilities to determine authentication procedure */
    BTM_AUTH_SINGLE_PROFILE_GENERAL_BONDING_NO = 4,     /**< MITM Protection Not Required - Single Profiles/general bonding. Numeric comparison with automatic accept allowed */
    BTM_AUTH_SINGLE_PROFILE_GENERAL_BONDING_YES = 5,    /**< MITM Protection Required - Single Profiles/general bonding. Use IO Capabilities to determine authentication procedure */
};
typedef uint8_t wiced_bt_dev_auth_req_t;                /**< BR/EDR authentication requirement (see #wiced_bt_dev_auth_req_e) */

/** LE Authentication requirement */
enum wiced_bt_dev_le_auth_req_e {
    BTM_LE_AUTH_REQ_NO_BOND =       0x00,                                               /**< Not required - No Bond */
    BTM_LE_AUTH_REQ_BOND =          0x01,                                               /**< Required - General Bond */
    BTM_LE_AUTH_REQ_MITM =          0x04,                                               /**< MITM required - Auth Y/N*/
    BTM_LE_AUTH_REQ_SC_ONLY =       0x08,                                               /**< LE Secure Connection, no MITM, no Bonding */
    BTM_LE_AUTH_REQ_SC_BOND =       (BTM_LE_AUTH_REQ_SC_ONLY|BTM_LE_AUTH_REQ_BOND),     /**< LE Secure Connection, no MITM, Bonding */
    BTM_LE_AUTH_REQ_SC_MITM =       (BTM_LE_AUTH_REQ_SC_ONLY|BTM_LE_AUTH_REQ_MITM),     /**< LE Secure Connection, MITM, no Bonding */
    BTM_LE_AUTH_REQ_SC_MITM_BOND =  (BTM_LE_AUTH_REQ_SC_ONLY|BTM_LE_AUTH_REQ_MITM|BTM_LE_AUTH_REQ_BOND),    /**< LE Secure Connection, MITM, Bonding */
    BTM_LE_AUTH_REQ_MASK =          0x1D
};
typedef uint8_t wiced_bt_dev_le_auth_req_t;             /**< BLE authentication requirement (see #wiced_bt_dev_le_auth_req_e) */

/** OOB Data status */
#ifndef BTM_OOB_STATE
#define BTM_OOB_STATE
enum wiced_bt_dev_oob_data_e
{
    BTM_OOB_NONE,                                       /**< No OOB data */
    BTM_OOB_PRESENT_192,                                /**< OOB data present (from the P-192 public key) */
    BTM_OOB_PRESENT_256,                                /**< OOB data present (from the P-256 public key) */
    BTM_OOB_PRESENT_192_256,                            /**< OOB data present (from the P-192 and P-256 public keys) */
    BTM_OOB_UNKNOWN                                     /**< OOB data unknown */
};
#endif
typedef uint8_t wiced_bt_dev_oob_data_t;                /**< OOB data (see #wiced_bt_dev_oob_data_e) */

/** Data type for IO capabalities response (BTM_PAIRING_IO_CAPABILITIES_BR_EDR_RESPONSE_EVT) */
typedef struct
{
    wiced_bt_device_address_t   bd_addr;                /**< Peer address */
    wiced_bt_dev_io_cap_t       io_cap;                 /**< Peer IO capabilities */
    wiced_bt_dev_oob_data_t     oob_data;               /**< OOB data present at peer device for the local device */
    wiced_bt_dev_auth_req_t     auth_req;               /**< Authentication required for peer device */
} wiced_bt_dev_bredr_io_caps_rsp_t;

/** Data for pairing confirmation request (BTM_USER_CONFIRMATION_REQUEST_EVT event data type) */
typedef struct
{
    wiced_bt_device_address_t   bd_addr;                            /**< peer address */
    uint32_t                    numeric_value;                      /**< numeric value for comparison (if "just_works", do not show this number to UI) */
    wiced_bool_t                just_works;                         /**< TRUE, if using "just works" association model */
    wiced_bt_dev_auth_req_t     local_authentication_requirements;  /**< Authentication requirement for local device */
    wiced_bt_dev_auth_req_t     remote_authentication_requirements; /**< Authentication requirement for peer device */
} wiced_bt_dev_user_cfm_req_t;

/** Pairing user passkey request  (BTM_USER_PASSKEY_REQUEST_EVT event data type) */
typedef struct
{
    wiced_bt_device_address_t   bd_addr;            /**< peer address       */
} wiced_bt_dev_user_key_req_t;

/** Data for pairing passkey notification (BTM_USER_PASSKEY_NOTIFICATION_EVT event data type) */
typedef struct
{
    wiced_bt_device_address_t   bd_addr;            /**< peer address       */
    uint32_t                    passkey;            /**< passkey            */
} wiced_bt_dev_user_key_notif_t;


/** Pairing keypress types */
enum wiced_bt_dev_passkey_entry_type_e
{
    BTM_PASSKEY_ENTRY_STARTED,          /**< passkey entry started */
    BTM_PASSKEY_DIGIT_ENTERED,          /**< passkey digit entered */
    BTM_PASSKEY_DIGIT_ERASED,           /**< passkey digit erased */
    BTM_PASSKEY_DIGIT_CLEARED,          /**< passkey cleared */
    BTM_PASSKEY_ENTRY_COMPLETED         /**< passkey entry completed */
};
typedef uint8_t wiced_bt_dev_passkey_entry_type_t;  /**< Bluetooth pairing keypress value (see #wiced_bt_dev_passkey_entry_type_e)  */

/** Pairing keypress notification (BTM_USER_KEYPRESS_NOTIFICATION_EVT event data type) */
typedef struct
{
    wiced_bt_device_address_t           bd_addr;        /**< peer address       */
    wiced_bt_dev_passkey_entry_type_t   keypress_type;  /**< type of keypress   */
} wiced_bt_dev_user_keypress_t;

/** BR/EDR pairing complete infomation */
typedef struct
{
    uint8_t         status;                 /**< status of the simple pairing process (see defintions for HCI status codes) */
} wiced_bt_dev_br_edr_pairing_info_t;

/** BLE pairing complete infomation */
typedef struct
{
    wiced_result_t                    status;                 /**< status of the simple pairing process   */
    uint8_t                           reason;                 /**< failure reason (see #wiced_bt_smp_status_t) */
    uint8_t                           sec_level;              /**< 0 - None, 1- Unauthenticated Key, 4-Authenticated Key  */
    wiced_bool_t                      is_pair_cancel;         /**< True if cancelled, else False   */
    wiced_bt_device_address_t         resolved_bd_addr;       /**< Resolved address (if remote device using private address) */
    wiced_bt_ble_address_type_t       resolved_bd_addr_type;  /**< Resolved addr type of bonded device */
} wiced_bt_dev_ble_pairing_info_t;

/** Transport dependent pairing complete infomation */
typedef union
{
    wiced_bt_dev_br_edr_pairing_info_t  br_edr;         /**< BR/EDR pairing complete infomation */
    wiced_bt_dev_ble_pairing_info_t     ble;            /**< BLE pairing complete infomation */
} wiced_bt_dev_pairing_info_t;

/** Pairing complete notification (BTM_PAIRING_COMPLETE_EVT event data type) */
typedef struct
{
    uint8_t                     *bd_addr;               /**< peer address           */
    wiced_bt_transport_t        transport;              /**< BT_TRANSPORT_BR_EDR or BT_TRANSPORT_LE */
    wiced_bt_dev_pairing_info_t pairing_complete_info;  /**< Transport dependent pairing complete infomation */
    wiced_result_t                 bonding_status;         /**< current status of bonding process to notify app of completion status of storing keys */
} wiced_bt_dev_pairing_cplt_t;

/** Security/authentication failure status  (used by BTM_SECURITY_FAILED_EVT notication) */
typedef struct
{
    wiced_bt_device_address_t   bd_addr;                /**< [in]  Peer address */
    wiced_result_t              status;                 /**< Status of the operation */
    uint8_t                     hci_status;             /**< Status from controller */
} wiced_bt_dev_security_failed_t;

/** Security request (BTM_SECURITY_REQUEST_EVT event data type) */
typedef struct
{
    wiced_bt_device_address_t      bd_addr;             /**< peer address           */
} wiced_bt_dev_security_request_t;

/** LE Key type */
#ifndef BTM_LE_KEY_TYPES
#define BTM_LE_KEY_TYPES
enum wiced_bt_dev_le_key_type_e
{
    BTM_LE_KEY_PENC =   (1 << 0),                       /**< encryption information of peer device */
    BTM_LE_KEY_PID =    (1 << 1),                       /**< identity key of the peer device */
    BTM_LE_KEY_PCSRK =  (1 << 2),                       /**< peer SRK */
#if SMP_LE_SC_INCLUDED == TRUE
    BTM_LE_KEY_PLK =    (1 << 3),
    BTM_LE_KEY_LENC =   (1 << 4),                       /**< master role security information:div */
    BTM_LE_KEY_LID =    (1 << 5),                       /**< master device ID key */
    BTM_LE_KEY_LCSRK =  (1 << 6),                       /**< local CSRK has been deliver to peer */
    BTM_LE_KEY_LLK =    (1 << 7),
#else
    BTM_LE_KEY_LENC =   (1 << 3),                       /**< master role security information:div */
    BTM_LE_KEY_LID =    (1 << 4),                       /**< master device ID key */
    BTM_LE_KEY_LCSRK =  (1 << 5)                        /**< local CSRK has been deliver to peer */
#endif
};
#endif  /* BTM_LE_KEY_TYPES */
typedef uint8_t wiced_bt_dev_le_key_type_t;             /**< LE key type (see #wiced_bt_dev_le_key_type_e) */

/** Scan duty cycle (used for BTM_BLE_SCAN_STATE_CHANGED_EVT and wiced_bt_dev_create_connection) */
#ifndef BTM_BLE_SCAN_TYPE
#define BTM_BLE_SCAN_TYPE
enum wiced_bt_ble_scan_type_e
{
    BTM_BLE_SCAN_TYPE_NONE,         /**< Stop scanning */
    BTM_BLE_SCAN_TYPE_HIGH_DUTY,    /**< High duty cycle scan */
    BTM_BLE_SCAN_TYPE_LOW_DUTY      /**< Low duty cycle scan */
};
#endif
typedef uint8_t wiced_bt_ble_scan_type_t;   /**< scan type (see #wiced_bt_ble_scan_type_e) */


/** bonding device information from wiced_bt_dev_get_bonded_devices */
typedef struct
{
    wiced_bt_device_address_t     bd_addr;                 /**< peer address           */
    wiced_bt_ble_address_type_t   addr_type;               /**< peer address type : BLE_ADDR_PUBLIC/BLE_ADDR_RANDOM */
    wiced_bt_device_type_t        device_type;             /**< peer device type : BT_DEVICE_TYPE_BREDR/BT_DEVICE_TYPE_BLE/BT_DEVICE_TYPE_BREDR_BLE  */
}wiced_bt_dev_bonded_device_info_t;

/* LE Secure connection event data */
/** Type of OOB data required  */
#ifndef BTM_OOB_REQ_TYPE
#define BTM_OOB_REQ_TYPE
enum wiced_bt_dev_oob_data_req_type_e
{
    BTM_OOB_INVALID_TYPE,
    BTM_OOB_PEER,                                       /**< Peer OOB data requested */
    BTM_OOB_LOCAL,                                      /**< Local OOB data requested */
    BTM_OOB_BOTH                                        /**< Both local and peer OOB data requested */
};
#endif
typedef UINT8 wiced_bt_dev_oob_data_req_type_t;         /**< OOB data type requested (see #wiced_bt_dev_oob_data_req_type_t) */

/** SMP Pairing status codes */
enum wiced_bt_smp_status_e
{
    SMP_SUCCESS                 = 0,                    /**< Success */
    SMP_PASSKEY_ENTRY_FAIL      = 0x01,                 /**< Passkey entry failed */
    SMP_OOB_FAIL                = 0x02,                 /**< OOB failed */
    SMP_PAIR_AUTH_FAIL          = 0x03,                 /**< Authentication failed */
    SMP_CONFIRM_VALUE_ERR       = 0x04,                 /**< Value confirmation failed */
    SMP_PAIR_NOT_SUPPORT        = 0x05,                 /**< Not supported */
    SMP_ENC_KEY_SIZE            = 0x06,                 /**< Encryption key size failure */
    SMP_INVALID_CMD             = 0x07,                 /**< Invalid command */
    SMP_PAIR_FAIL_UNKNOWN       = 0x08,                 /**< Unknown failure */
    SMP_REPEATED_ATTEMPTS       = 0x09,                 /**< Repeated attempts */
    SMP_INVALID_PARAMETERS      = 0x0A,                 /**< Invalid parameters  */
    SMP_DHKEY_CHK_FAIL          = 0x0B,                 /**< DH Key check failed */
    SMP_NUMERIC_COMPAR_FAIL     = 0x0C,                 /**< Numeric comparison failed */
    SMP_BR_PAIRING_IN_PROGR     = 0x0D,                 /**< BR paIring in progress */
    SMP_XTRANS_DERIVE_NOT_ALLOW = 0x0E,                 /**< Cross transport key derivation not allowed */
    SMP_MAX_FAIL_RSN_PER_SPEC   = SMP_XTRANS_DERIVE_NOT_ALLOW,

    /* bte smp status codes */
    SMP_PAIR_INTERNAL_ERR       = (SMP_MAX_FAIL_RSN_PER_SPEC + 0x01), /**< Internal error */
    SMP_UNKNOWN_IO_CAP          = (SMP_MAX_FAIL_RSN_PER_SPEC + 0x02), /**< unknown IO capability, unable to decide associatino model */
    SMP_INIT_FAIL               = (SMP_MAX_FAIL_RSN_PER_SPEC + 0x03), /**< Initialization failed */
    SMP_CONFIRM_FAIL            = (SMP_MAX_FAIL_RSN_PER_SPEC + 0x04), /**< Confirmation failed */
    SMP_BUSY                    = (SMP_MAX_FAIL_RSN_PER_SPEC + 0x05), /**< Busy */
    SMP_ENC_FAIL                = (SMP_MAX_FAIL_RSN_PER_SPEC + 0x06), /**< Encryption failed */
    SMP_STARTED                 = (SMP_MAX_FAIL_RSN_PER_SPEC + 0x07), /**< Started */
    SMP_RSP_TIMEOUT             = (SMP_MAX_FAIL_RSN_PER_SPEC + 0x08), /**< Response timeout */
    SMP_FAIL                    = (SMP_MAX_FAIL_RSN_PER_SPEC + 0x09), /**< Generic failure */
    SMP_CONN_TOUT               = (SMP_MAX_FAIL_RSN_PER_SPEC + 0x0A), /**< Connection timeout */
};
typedef uint8_t wiced_bt_smp_status_t;      /**< SMP Pairing status (see #wiced_bt_smp_status_e) */

/** data type for BTM_SMP_REMOTE_OOB_DATA_REQUEST_EVT */
typedef struct
{
    wiced_bt_device_address_t           bd_addr;        /* peer address */
} wiced_bt_smp_remote_oob_req_t;

/** data type for BTM_SMP_SC_REMOTE_OOB_DATA_REQUEST_EVT */
typedef struct
{
    wiced_bt_device_address_t           bd_addr;        /* peer address */
    wiced_bt_dev_oob_data_req_type_t    oob_type;       /* requested oob data types (BTM_OOB_PEER, BTM_OOB_LOCAL, or BTM_OOB_BOTH) */
} wiced_bt_smp_sc_remote_oob_req_t;

/** Public key */
typedef struct
{
    BT_OCTET32  x;
    BT_OCTET32  y;
} wiced_bt_public_key_t;

/**< Data for BTM_SMP_SC_LOCAL_OOB_DATA_NOTIFICATION_EVT */
typedef struct
{
    wiced_bool_t                present;                /**< TRUE if local oob is present */
    BT_OCTET16                  randomizer;             /**< randomizer */
    BT_OCTET16                  commitment;             /**< commitment */

    wiced_bt_ble_address_t      addr_sent_to;           /**< peer address sent to */
    BT_OCTET32                  private_key_used;       /**< private key */
    wiced_bt_public_key_t       public_key_used;        /**< public key */
} wiced_bt_smp_sc_local_oob_t;

/* Data associated with the information received from the peer via OOB interface */
typedef struct
{
    wiced_bool_t    present;
    BT_OCTET16      randomizer;
    BT_OCTET16      commitment;

    wiced_bt_ble_address_t    addr_rcvd_from;
} wiced_bt_smp_sc_peer_oob_data_t;

/**< Data for wiced_bt_smp_sc_oob_reply */
typedef struct
{
    wiced_bt_smp_sc_local_oob_t     local_oob_data;
    wiced_bt_smp_sc_peer_oob_data_t peer_oob_data;
}wiced_bt_smp_sc_oob_data_t;

/** SCO link type */
#define BTM_LINK_TYPE_SCO           HCI_LINK_TYPE_SCO       /**< Link type SCO */
#define BTM_LINK_TYPE_ESCO          HCI_LINK_TYPE_ESCO      /**< Link type eSCO */
typedef uint8_t wiced_bt_sco_type_t;


/** LE identity key for local device (used by BTM_LE_LOCAL_IDENTITY_KEYS_UPDATE_EVT and BTM_LE_LOCAL_KEYS_REQUEST_EVT notification) */
typedef struct {
    uint8_t     local_key_data[BTM_SECURITY_LOCAL_KEY_DATA_LEN];    /**< [in/out] Local security key */
} wiced_bt_local_identity_keys_t;

/** SCO connected event related data */
typedef struct {
    uint16_t    sco_index;                  /**< SCO index */
} wiced_bt_sco_connected_t;

/** SCO disconnected event related data */
typedef struct {
    uint16_t    sco_index;                  /**< SCO index */
} wiced_bt_sco_disconnected_t;

/**  SCO connect request event related data */
typedef struct {
    uint16_t                    sco_index;      /**< SCO index */
    wiced_bt_device_address_t   bd_addr;        /**< Peer bd address */
    wiced_bt_dev_class_t        dev_class;      /**< Peer device class */
    wiced_bt_sco_type_t         link_type;      /**< SCO link type */
} wiced_bt_sco_connection_request_t;

/** SCO connection change event related data */
typedef struct {
    uint16_t                    sco_index;          /**< SCO index */
    uint16_t                    rx_pkt_len;         /**< RX packet length */
    uint16_t                    tx_pkt_len;         /**< TX packet length */
    wiced_bt_device_address_t   bd_addr;            /**< Peer bd address */
    uint8_t                     hci_status;         /**< HCI status */
    uint8_t                     tx_interval;        /**< TX interval */
    uint8_t                     retrans_windows;    /**< Retransmission windows */
} wiced_bt_sco_connection_change_t;

/* data type for BTM_BLE_CONNECTION_PARAM_UPDATE */
typedef struct
{
    uint8_t                    status;
    wiced_bt_device_address_t  bd_addr;
    uint16_t                   conn_interval;
    uint16_t                   conn_latency;
    uint16_t                   supervision_timeout;
} wiced_bt_ble_conn_param_update_t;

/** BLE Physical link update event related data */
typedef struct
{
    uint8_t                     status;                 /**< LE Phy update status */
    wiced_bt_device_address_t   bd_address;             /**< peer BD address*/
    uint8_t                     tx_phy;                 /**< Transmitter PHY, values: 1=1M, 2=2M, 3=LE coded */
    uint8_t                     rx_phy;                 /**< Receiver PHY, values: 1=1M, 2=2M, 3=LE coded */
} wiced_bt_ble_phy_update_t;

/** Power Management status codes */
#ifndef BTM_PM_STATUS_CODES
#define BTM_PM_STATUS_CODES
enum wiced_bt_dev_power_mgmt_status_e
{
    BTM_PM_STS_ACTIVE = HCI_MODE_ACTIVE,    /**< Active */
    BTM_PM_STS_HOLD   = HCI_MODE_HOLD,      /**< Hold */
    BTM_PM_STS_SNIFF  = HCI_MODE_SNIFF,     /**< Sniff */
    BTM_PM_STS_PARK   = HCI_MODE_PARK,      /**< Park */
    BTM_PM_STS_SSR,                         /**< Sniff subrating notification */
    BTM_PM_STS_PENDING,                     /**< Pending (waiting for status from controller) */
    BTM_PM_STS_ERROR                        /**< Error (controller returned error) */
};
#endif
typedef uint8_t wiced_bt_dev_power_mgmt_status_t;   /**< Power management status (see #wiced_bt_dev_power_mgmt_status_e) */

#ifndef BTM_MANAGEMENT_EVT
#define BTM_MANAGEMENT_EVT
/** Bluetooth Management events */
enum wiced_bt_management_evt_e {
    /* Bluetooth status events */
    BTM_ENABLED_EVT,                                /**< Bluetooth controller and host stack enabled. Event data: wiced_bt_dev_enabled_t */
    BTM_DISABLED_EVT,                               /**< Bluetooth controller and host stack disabled. Event data: NULL */
    BTM_POWER_MANAGEMENT_STATUS_EVT,                /**< Power management status change. Event data: wiced_bt_power_mgmt_notification_t */

    /* Security events */
    BTM_PIN_REQUEST_EVT,                            /**< PIN request (used only with legacy devices). Event data: #wiced_bt_dev_name_and_class_t */
    BTM_USER_CONFIRMATION_REQUEST_EVT,              /**< received USER_CONFIRMATION_REQUEST event (respond using #wiced_bt_dev_confirm_req_reply). Event data: #wiced_bt_dev_user_cfm_req_t */
    BTM_PASSKEY_NOTIFICATION_EVT,                   /**< received USER_PASSKEY_NOTIFY event. Event data: #wiced_bt_dev_user_key_notif_t */
    BTM_PASSKEY_REQUEST_EVT,                        /**< received USER_PASSKEY_REQUEST event (respond using #wiced_bt_dev_pass_key_req_reply). Event data: #wiced_bt_dev_user_key_req_t */
    BTM_KEYPRESS_NOTIFICATION_EVT,                  /**< received KEYPRESS_NOTIFY event. Event data: #wiced_bt_dev_user_keypress_t */
    BTM_PAIRING_IO_CAPABILITIES_BR_EDR_REQUEST_EVT, /**< Requesting IO capabilities for BR/EDR pairing. Event data: #wiced_bt_dev_bredr_io_caps_req_t */
    BTM_PAIRING_IO_CAPABILITIES_BR_EDR_RESPONSE_EVT,/**< Received IO capabilities response for BR/EDR pairing. Event data: #wiced_bt_dev_bredr_io_caps_rsp_t */
    BTM_PAIRING_IO_CAPABILITIES_BLE_REQUEST_EVT,    /**< Requesting IO capabilities for BLE pairing. Event data: #wiced_bt_dev_ble_io_caps_req_t */
    BTM_PAIRING_COMPLETE_EVT,                       /**< received SIMPLE_PAIRING_COMPLETE event. Event data: #wiced_bt_dev_pairing_cplt_t */
    BTM_ENCRYPTION_STATUS_EVT,                      /**< Encryption status change. Event data: #wiced_bt_dev_encryption_status_t */
    BTM_SECURITY_REQUEST_EVT,                       /**< Security request (respond using #wiced_bt_ble_security_grant). Event data: #wiced_bt_dev_security_request_t */
    BTM_SECURITY_FAILED_EVT,                        /**< Security procedure/authentication failed. Event data: #wiced_bt_dev_security_failed_t */
    BTM_SECURITY_ABORTED_EVT,                       /**< Security procedure aborted locally, or unexpected link drop. Event data: #wiced_bt_dev_name_and_class_t */

    BTM_READ_LOCAL_OOB_DATA_COMPLETE_EVT,           /**< Result of reading local OOB data (wiced_bt_dev_read_local_oob_data). Event data: #wiced_bt_dev_local_oob_t */
    BTM_REMOTE_OOB_DATA_REQUEST_EVT,                /**< OOB data from remote device (respond using #wiced_bt_dev_remote_oob_data_reply). Event data: #wiced_bt_dev_remote_oob_t */

    BTM_PAIRED_DEVICE_LINK_KEYS_UPDATE_EVT,         /**< Updated remote device link keys (store device_link_keys to  NV memory). Event data: #wiced_bt_device_link_keys_t */
    BTM_PAIRED_DEVICE_LINK_KEYS_REQUEST_EVT,        /**< Request for stored remote device link keys (restore device_link_keys from NV memory). If successful, return WICED_BT_SUCCESS. Event data: #wiced_bt_device_link_keys_t */

    BTM_LOCAL_IDENTITY_KEYS_UPDATE_EVT,             /**< Update local identity key (stored local_identity_keys NV memory). Event data: #wiced_bt_local_identity_keys_t */
    BTM_LOCAL_IDENTITY_KEYS_REQUEST_EVT,            /**< Request local identity key (get local_identity_keys from NV memory). If successful, return WICED_BT_SUCCESS. Event data: #wiced_bt_local_identity_keys_t */

    BTM_BLE_SCAN_STATE_CHANGED_EVT,                 /**< BLE scan state change. Event data: #wiced_bt_ble_scan_type_t */
    BTM_BLE_ADVERT_STATE_CHANGED_EVT,               /**< BLE advertisement state change. Event data: #wiced_bt_ble_advert_mode_t */

    /* BLE Secure Connection events */
    BTM_SMP_REMOTE_OOB_DATA_REQUEST_EVT,            /**< SMP remote oob data request. Reply using wiced_bt_smp_oob_data_reply. Event data: wiced_bt_smp_remote_oob_req_t  */
    BTM_SMP_SC_REMOTE_OOB_DATA_REQUEST_EVT,         /**< LE secure connection remote oob data request. Reply using wiced_bt_smp_sc_oob_reply. Event data: #wiced_bt_smp_sc_remote_oob_req_t */
    BTM_SMP_SC_LOCAL_OOB_DATA_NOTIFICATION_EVT,     /**< LE secure connection local OOB data (wiced_bt_smp_create_local_sc_oob_data). Event data: #wiced_bt_smp_sc_local_oob_t*/

    BTM_SCO_CONNECTED_EVT,                          /**< SCO connected event. Event data: #wiced_bt_sco_connected_t */
    BTM_SCO_DISCONNECTED_EVT,                       /**< SCO disconnected event. Event data: #wiced_bt_sco_disconnected_t */
    BTM_SCO_CONNECTION_REQUEST_EVT,                 /**< SCO connection request event. Event data: #wiced_bt_sco_connection_request_t */
    BTM_SCO_CONNECTION_CHANGE_EVT,                  /**< SCO connection change event. Event data: #wiced_bt_sco_connection_change_t */
    BTM_BLE_CONNECTION_PARAM_UPDATE,                /**< BLE connection parameter update. Event data: wiced_bt_ble_conn_param_update_t */
    BTM_BLE_PHY_UPDATE_EVT,                         /**< BLE Physical link update. Event data: wiced_bt_ble_phy_update_t */
    BTM_LPM_STATE_LOW_POWER,                        /**< BT device wake has been deasserted */
};
#endif
typedef uint8_t wiced_bt_management_evt_t;          /**< Bluetooth management events (see #wiced_bt_management_evt_e) */

/** Device enabled (used by BTM_ENABLED_EVT) */
typedef struct {
    wiced_result_t          status;                     /**< Status */
} wiced_bt_dev_enabled_t;

/** Device disabled (used by BTM_DISABLED_EVT) */
typedef struct {
    uint8_t          reason;                     /** < Reason for BTM Disable */
}wiced_bt_dev_disabled_t;

/** Remote device information (used by BTM_PIN_REQUEST_EVT, BTM_SECURITY_ABORTED_EVT) */
typedef struct {
    wiced_bt_device_address_t  *bd_addr;                /**< BD Address of remote */
    wiced_bt_dev_class_t    *dev_class;                 /**< peer class of device   */
    uint8_t                 *bd_name;                   /**< BD Name of remote */
} wiced_bt_dev_name_and_class_t;

/** Change in power management status  (used by BTM_POWER_MANAGEMENT_STATUS_EVT notication) */
typedef struct {
    wiced_bt_device_address_t           bd_addr;        /**< BD Address of remote */
    wiced_bt_dev_power_mgmt_status_t    status;         /**< PM status */
    uint16_t                            value;          /**< Additional mode data */
    uint8_t                             hci_status;     /**< HCI status */
} wiced_bt_power_mgmt_notification_t;

/** Encryption status change (used by BTM_ENCRYPTION_STATUS_EVT) */
typedef struct {
    uint8_t                 *bd_addr;                   /**< BD Address of remote */
    wiced_bt_transport_t    transport;                  /**< BT_TRANSPORT_BR_EDR or BT_TRANSPORT_LE */
    void                    *p_ref_data;                /**< Optional data passed in by wiced_bt_dev_set_encryption */
    wiced_result_t          result;                     /**< Result of the operation */
} wiced_bt_dev_encryption_status_t;

/** Local OOB data BTM_READ_LOCAL_OOB_DATA_COMPLETE_EVT */
typedef struct {
    wiced_result_t          status;                     /**< Status */
    wiced_bool_t            is_extended_oob_data;       /**< TRUE if extended OOB data */

    BT_OCTET16              c_192;                      /**< Simple Pairing Hash C derived from the P-192 public key */
    BT_OCTET16              r_192;                      /**< Simple Pairing Randomnizer R associated with the P-192 public key */
    BT_OCTET16              c_256;                      /**< Simple Pairing Hash C derived from the P-256 public key (valid only if is_extended_oob_data=TRUE) */
    BT_OCTET16              r_256;                      /**< Simple Pairing Randomnizer R associated with the P-256 public key (valid only if is_extended_oob_data=TRUE) */
} wiced_bt_dev_local_oob_t;

/** BTM_REMOTE_OOB_DATA_REQUEST_EVT */
typedef struct {
    wiced_bt_device_address_t   bd_addr;                /**< BD Address of remote */
    wiced_bool_t                extended_oob_data;      /**< TRUE if requesting extended OOB (P-256) */
} wiced_bt_dev_remote_oob_t;

/** BR/EDR Pairing IO Capabilities (to be filled by application callback on BTM_PAIRING_IO_CAPABILITIES_BR_EDR_REQUEST_EVT) */
typedef struct
{
    wiced_bt_device_address_t   bd_addr;                /**< [in] BD Address of remote   */
    wiced_bt_dev_io_cap_t       local_io_cap;           /**< local IO capabilities (to be filled by application callback) */
    wiced_bt_dev_oob_data_t     oob_data;               /**< OOB data present at peer device for the local device   */
    wiced_bt_dev_auth_req_t     auth_req;               /**< Authentication required for peer device                */
    wiced_bool_t                is_orig;                /**< TRUE, if local device initiated the pairing process    */
} wiced_bt_dev_bredr_io_caps_req_t;

/** BLE Pairing IO Capabilities (to be filled by application callback on BTM_PAIRING_IO_CAPABILITIES_BLE_REQUEST_EVT) */
typedef struct
{
    wiced_bt_device_address_t   bd_addr;                /**< [in] BD Address of remote   */
    wiced_bt_dev_io_cap_t       local_io_cap;           /**< local IO capabilities (to be filled by application callback) */
    uint8_t                     oob_data;               /**< OOB data present (locally) for the peer device                             */
    wiced_bt_dev_le_auth_req_t  auth_req;               /**< Authentication request (for local device) contain bonding and MITM info    */
    uint8_t                     max_key_size;           /**< Max encryption key size                                                    */
    wiced_bt_dev_le_key_type_t  init_keys;              /**< Keys to be distributed, bit mask                                           */
    wiced_bt_dev_le_key_type_t  resp_keys;              /**< keys to be distributed, bit mask                                           */
} wiced_bt_dev_ble_io_caps_req_t;



typedef struct
{
    BT_OCTET16          irk;            /**< peer diverified identity root */
#if SMP_INCLUDED == TRUE && SMP_LE_SC_INCLUDED == TRUE
    BT_OCTET16          pltk;           /**< peer long term key */
    BT_OCTET16          pcsrk;          /**< peer SRK peer device used to secured sign local data  */

    BT_OCTET16          lltk;           /**< local long term key */
    BT_OCTET16          lcsrk;          /**< local SRK peer device used to secured sign local data  */
#else
    BT_OCTET16          ltk;            /**< peer long term key */
    BT_OCTET16          csrk;           /**< peer SRK peer device used to secured sign local data  */
#endif

    BT_OCTET8           rand;           /**< random vector for LTK generation */
    UINT16              ediv;           /**< LTK diversifier of this slave device */
    UINT16              div;            /**< local DIV  to generate local LTK=d1(ER,DIV,0) and CSRK=d1(ER,DIV,1)  */
    uint8_t             sec_level;      /**< local pairing security level */
    uint8_t             key_size;       /**< key size of the LTK delivered to peer device */
    uint8_t             srk_sec_level;  /**< security property of peer SRK for this device */
    uint8_t             local_csrk_sec_level;  /**< security property of local CSRK for this device */

    UINT32              counter;        /**< peer sign counter for verifying rcv signed cmd */
    UINT32              local_counter;  /**< local sign counter for sending signed write cmd*/
}wiced_bt_ble_keys_t;


typedef struct
{
    /* BR/EDR key */
    uint8_t                           br_edr_key_type;        /**<  BR/EDR Link Key type */
    wiced_bt_link_key_t               br_edr_key;             /**<  BR/EDR Link Key */

    /* LE Keys */
    wiced_bt_dev_le_key_type_t        le_keys_available_mask; /**<  Mask of available BLE keys */
    wiced_bt_ble_address_type_t       ble_addr_type;          /**<  LE device type: public or random address */
    wiced_bt_ble_address_type_t       static_addr_type;       /**<  static address type */
    wiced_bt_device_address_t         static_addr;            /**<  static address */
    wiced_bt_ble_keys_t               le_keys;                /**<  LE keys */
} wiced_bt_device_sec_keys_t;

/** Paired device link key notification (used by BTM_PAIRED_DEVICE_LINK_KEYS_UPDATE_EVT notication) */
typedef struct
{
    wiced_bt_device_address_t   bd_addr;                                    /**< [in] BD Address of remote */
    wiced_bt_device_sec_keys_t  key_data;        /**< [in/out] Key data */
} wiced_bt_device_link_keys_t;


/** advertisement type (used when calling wiced_bt_start_advertisements) */
#ifndef BTM_BLE_ADVERT_MODE
#define BTM_BLE_ADVERT_MODE
enum wiced_bt_ble_advert_mode_e
{
    BTM_BLE_ADVERT_OFF,                 /**< Stop advertising */
    BTM_BLE_ADVERT_DIRECTED_HIGH,       /**< Directed advertisement (high duty cycle) */
    BTM_BLE_ADVERT_DIRECTED_LOW,        /**< Directed advertisement (low duty cycle) */
    BTM_BLE_ADVERT_UNDIRECTED_HIGH,     /**< Undirected advertisement (high duty cycle) */
    BTM_BLE_ADVERT_UNDIRECTED_LOW,      /**< Undirected advertisement (low duty cycle) */
    BTM_BLE_ADVERT_NONCONN_HIGH,        /**< Non-connectable advertisement (high duty cycle) */
    BTM_BLE_ADVERT_NONCONN_LOW,         /**< Non-connectable advertisement (low duty cycle) */
    BTM_BLE_ADVERT_DISCOVERABLE_HIGH,   /**< discoverable advertisement (high duty cycle) */
    BTM_BLE_ADVERT_DISCOVERABLE_LOW     /**< discoverable advertisement (low duty cycle) */
};
#endif
typedef uint8_t wiced_bt_ble_advert_mode_t;   /**< Advertisement type (see #wiced_bt_ble_advert_mode_e) */

/** scan mode used in initiating */
#ifndef BTM_BLE_CONN_MODE
#define BTM_BLE_CONN_MODE
enum wiced_bt_ble_conn_mode_e
{
    BLE_CONN_MODE_OFF,                  /**< Stop initiating */
    BLE_CONN_MODE_LOW_DUTY,             /**< slow connection scan parameter */
    BLE_CONN_MODE_HIGH_DUTY             /**< fast connection scan parameter */
};
#endif
typedef uint8_t wiced_bt_ble_conn_mode_t;       /**< Conn mode (see #wiced_bt_ble_conn_mode_e) */

/** Structure definitions for Bluetooth Management (wiced_bt_management_cback_t) event notifications */
typedef union
{
    /* Bluetooth status event data types*/
    wiced_bt_dev_enabled_t              enabled;                            /**< Data for BTM_ENABLED_EVT */
    wiced_bt_dev_disabled_t             disabled;                           /**< Data for BTM_DISABLED_EVT */
    wiced_bt_power_mgmt_notification_t  power_mgmt_notification;            /**< Data for BTM_POWER_MANAGEMENT_STATUS_EVT */

    /* Security event data types */
    wiced_bt_dev_name_and_class_t       pin_request;                        /**< Data for BTM_PIN_REQUEST_EVT */
    wiced_bt_dev_user_cfm_req_t         user_confirmation_request;          /**< Data for BTM_USER_CONFIRMATION_REQUEST_EVT */
    wiced_bt_dev_user_key_notif_t       user_passkey_notification;          /**< Data for BTM_USER_PASSKEY_NOTIFICATION_EVT */
    wiced_bt_dev_user_key_req_t         user_passkey_request;               /**< Data for BTM_USER_PASSKEY_REQUEST_EVT */
    wiced_bt_dev_user_keypress_t        user_keypress_notification;         /**< Data for BTM_USER_KEYPRESS_NOTIFICATION_EVT - See #wiced_bt_dev_user_keypress_t */
    wiced_bt_dev_bredr_io_caps_req_t    pairing_io_capabilities_br_edr_request; /**< Data for BTM_PAIRING_IO_CAPABILITIES_BR_EDR_REQUEST_EVT */
    wiced_bt_dev_bredr_io_caps_rsp_t    pairing_io_capabilities_br_edr_response;/**< Data for BTM_PAIRING_IO_CAPABILITIES_BR_EDR_RESPONSE_EVT */
    wiced_bt_dev_ble_io_caps_req_t      pairing_io_capabilities_ble_request;    /**< Data for BTM_PAIRING_IO_CAPABILITIES_BLE_REQUEST_EVT */
    wiced_bt_dev_pairing_cplt_t         pairing_complete;                   /**< Data for BTM_PAIRING_COMPLETE_EVT */
    wiced_bt_dev_encryption_status_t    encryption_status;                  /**< Data for BTM_ENCRYPTION_STATUS_EVT */
    wiced_bt_dev_security_request_t     security_request;                   /**< Data for BTM_SECURITY_REQUEST_EVT */
    wiced_bt_dev_security_failed_t      security_failed;                    /**< Data for BTM_SECURITY_FAILED_EVT See #wiced_bt_dev_security_failed_t */
    wiced_bt_dev_name_and_class_t       security_aborted;                   /**< Data for BTM_SECURITY_ABORTED_EVT */

    wiced_bt_dev_local_oob_t            read_local_oob_data_complete;       /**< Data for BTM_READ_LOCAL_OOB_DATA_COMPLETE_EVT */
    wiced_bt_dev_remote_oob_t           remote_oob_data_request;            /**< Data for BTM_REMOTE_OOB_DATA_REQUEST_EVT */

    wiced_bt_device_link_keys_t         paired_device_link_keys_update;     /**< Data for BTM_PAIRED_DEVICE_LINK_KEYS_UPDATE_EVT */
    wiced_bt_device_link_keys_t         paired_device_link_keys_request;    /**< Data for BTM_PAIRED_DEVICE_LINK_KEYS_REQUEST_EVT */
    wiced_bt_local_identity_keys_t      local_identity_keys_update;         /**< Data for BTM_LOCAL_IDENTITY_KEYS_UPDATE_EVT */
    wiced_bt_local_identity_keys_t      local_identity_keys_request;        /**< Data for BTM_LOCAL_IDENTITY_KEYS_REQUEST_EVT */

    wiced_bt_ble_scan_type_t            ble_scan_state_changed;             /**< Data for BTM_BLE_SCAN_STATE_CHANGED_EVT */
    wiced_bt_ble_advert_mode_t          ble_advert_state_changed;           /**< Data for BTM_BLE_ADVERT_STATE_CHANGED_EVT */

    wiced_bt_smp_remote_oob_req_t       smp_remote_oob_data_request;        /**< Data for BTM_SMP_REMOTE_OOB_DATA_REQUEST_EVT */
    wiced_bt_smp_sc_remote_oob_req_t    smp_sc_remote_oob_data_request;     /**< Data for BTM_SMP_SC_REMOTE_OOB_DATA_REQUEST_EVT */
    wiced_bt_smp_sc_local_oob_t         *p_smp_sc_local_oob_data;           /**< Data for BTM_SMP_SC_LOCAL_OOB_DATA_NOTIFICATION_EVT */

    wiced_bt_sco_connected_t            sco_connected;                      /**< Data for BTM_SCO_CONNECTED_EVT */
    wiced_bt_sco_disconnected_t         sco_disconnected;                   /**< Data for BTM_SCO_DISCONNECTED_EVT */
    wiced_bt_sco_connection_request_t   sco_connection_request;             /**< Data for BTM_SCO_CONNECTION_REQUEST_EVT */
    wiced_bt_sco_connection_change_t    sco_connection_change;              /**< Data for BTM_SCO_CONNECTION_CHANGE_EVT */
    wiced_bt_ble_conn_param_update_t    ble_connection_param_update;        /**< Data for BTM_BLE_CONNECTION_PARAM_UPDATE */
    wiced_bt_ble_phy_update_t           ble_phy_update_event;               /**< Data for BTM_BLE_PHY_UPDATE_EVT */

} wiced_bt_management_evt_data_t;

/**
 * Bluetooth Management callback
 *
 * Callback for Bluetooth Management event notifications.
 * Registered using wiced_bt_stack_init()
 *
 * @param event             : Event ID
 * @param p_event_data      : Event data
 *
 * @return Status of event handling
 */
typedef wiced_result_t (wiced_bt_management_cback_t) (wiced_bt_management_evt_t event, wiced_bt_management_evt_data_t *p_event_data);

/**
 * Connection status change callback
 *
 * Callback for Bluetooth Management event notifications.
 * Registered using wiced_bt_register_connection_status_change()
 *
 * @param[in] bd_addr       : BD Address of remote
 * @param[in] p_features
 * @param[in] is_connected  : TRUE if connected
 * @param[in] handle        : Connection handle
 * @param[in] transport     : BT_TRANSPORT_BR_EDR or BT_TRANSPORT_LE
 * @param[in] reason        : status for acl connection change
 *                                    HCI_SUCCESS
 *                                    HCI_ERR_PAGE_TIMEOUT
 *                                    HCI_ERR_MEMORY_FULL
 *                                    HCI_ERR_CONNECTION_TOUT
 *                                    HCI_ERR_PEER_USER
 *                                    HCI_ERR_CONN_CAUSE_LOCAL_HOST
 *                                    HCI_ERR_LMP_RESPONSE_TIMEOUT
 *                                    HCI_ERR_CONN_FAILED_ESTABLISHMENT
 *
 * @return void
 */
typedef void (wiced_bt_connection_status_change_cback_t) (wiced_bt_device_address_t bd_addr, uint8_t *p_features, wiced_bool_t is_connected, uint16_t handle, wiced_bt_transport_t transport, uint8_t reason);  /**<   connection status change callback */


/**
 * Inquiry result callback.
 *
 * @param p_inquiry_result          : Inquiry result data (NULL if inquiry is complete)
 * @param p_eir_data                : Extended inquiry response data
 *
 * @return Nothing
 */
typedef void (wiced_bt_inquiry_result_cback_t) (wiced_bt_dev_inquiry_scan_result_t *p_inquiry_result, uint8_t *p_eir_data); /**<   inquiry result callback */

/**
 * Remote name result callback.
 *
 * @param p_remote_name_result          : Remote name result data
 *
 * @return Nothing
 */
typedef void (wiced_bt_remote_name_cback_t) (wiced_bt_dev_remote_name_result_t *p_remote_name_result); /**<   remote name result callback */

/**
 * Synchronous BTM operation is complete.
 *
 * @param p_data            : Operation dependent data
 *
 * @return Nothing
 */
typedef void (wiced_bt_dev_cmpl_cback_t) (void *p_data);

/**
 * Vendor specific command complete
 *
 * @param p_command_complete_params : Command complete parameters
 *
 * @return Nothing
 */
typedef void (wiced_bt_dev_vendor_specific_command_complete_cback_t) (wiced_bt_dev_vendor_specific_command_complete_params_t *p_command_complete_params);
typedef void (wiced_bt_dev_vendor_specific_event_callback_t) (uint8_t len, uint8_t *p);

/******************************************************
 *               Function Declarations
 ******************************************************/
#ifdef __cplusplus
extern "C" {
#endif

/****************************************************************************/
/**
 * Device Management Functions
 *
 * @addtogroup  wicedbt_DeviceManagement    Device Management
 * @ingroup     wicedbt
 *
 * @{
 */
/****************************************************************************/


/****************************************************************************/
/**
 * Bluetooth Basic Rate / Enhanced Data Rate Functions
 *
 * @addtogroup  wicedbt_bredr    BR/EDR (Bluetooth Basic Rate / Enhanced Data Rate)
 * @ingroup     wicedbt_DeviceManagement
 *
 * @{
 */
/****************************************************************************/

/**
 * Function         wiced_bt_start_inquiry
 *
 *                  Begin BR/EDR inquiry for peer devices.
 *
 * @param[in]       p_inqparms              : inquiry parameters
 * @param[in]       p_inquiry_result_cback  : inquiry results callback
 *
 * @return          wiced_result_t
 *
 *                  WICED_BT_PENDING if successfully initiated
 *                  WICED_BT_BUSY if already in progress
 *                  WICED_BT_ILLEGAL_VALUE if parameter(s) are out of range
 *                  WICED_BT_NO_RESOURCES if could not allocate resources to start the command
 *                  WICED_BT_WRONG_MODE if the device is not up.
 */
wiced_result_t  wiced_bt_start_inquiry (wiced_bt_dev_inq_parms_t *p_inqparms, wiced_bt_inquiry_result_cback_t *p_inquiry_result_cback);

/**
 * Function         wiced_bt_cancel_inquiry
 *
 *                  Cancel inquiry
 *
 * @return
 *
 *                  WICED_BT_SUCCESS         if successful
 *                  WICED_BT_NO_RESOURCES    if could not allocate a message buffer
 *                  WICED_BT_WRONG_MODE      if the device is not up.
 */
wiced_result_t wiced_bt_cancel_inquiry(void);

/**
 * Function         wiced_bt_dev_read_local_addr
 *
 * Read the local device address
 *
 * @param[out]      bd_addr        : Local bd address
 *
 * @return          void
 *
 */
void wiced_bt_dev_read_local_addr (wiced_bt_device_address_t bd_addr);


/**
 * Function         wiced_bt_dev_set_advanced_connection_params
 *
 *                  Set advanced connection parameters for subsequent BR/EDR connections
 *                  (remote clock offset, page scan mode,  and other information obtained during inquiry)
 *
 *                  If not called, then default connection parameters will be used.
 *
 * @param[in]       p_inquiry_scan_result : Inquiry scan result (from #wiced_bt_inquiry_result_cback_t)
 *
 * @return          wiced_result_t
 *
 *                  WICED_BT_SUCCESS : on success;
 *                  WICED_BT_FAILED  : if an error occurred
 */
wiced_result_t wiced_bt_dev_set_advanced_connection_params (wiced_bt_dev_inquiry_scan_result_t *p_inquiry_scan_result);

/**
 * Function         wiced_bt_dev_vendor_specific_command
 *
 *                  Send a vendor specific HCI command to the controller.
 *
 * @param[in]       opcode              : Opcode of vendor specific command
 * @param[in]       param_len           : Length of parameter buffer
 * @param[in]       p_param_buf         : Parameters
 * @param[in]       p_cback             : Callback for command complete
 *
 * @return
 *
 *                  WICED_BT_SUCCESS    : Command sent. Does not expect command complete event. (command complete callback param is NULL)
 *                  WICED_BT_PENDING    : Command sent. Waiting for command complete event.
 *                  WICED_BT_BUSY       : Command not sent. Waiting for command complete event for prior command.
 *
 */
wiced_result_t wiced_bt_dev_vendor_specific_command (uint16_t opcode, uint8_t param_len, uint8_t *p_param_buf,
                                wiced_bt_dev_vendor_specific_command_complete_cback_t *p_cback);
/**
 * Function         wiced_bt_dev_register_vendor_specific_event
 *
 *                  Register/deregister a callback for vendor specific HCI events
 *
 * @param[in]       p_event_callback    : Callback to be registered
 * @param[in]       is_register         : If is_register=TRUE, then the function will be registered;
**                                        if is_register=FALSE, then the function will be deregistered.
 *
 * @return
 *
 *                  WICED_BT_SUCCESS    : if Registration/deregistration successful
 *                  WICED_BT_BUSY       : if maximum number of callbacks have already been registered.
 *
 */
wiced_bt_dev_status_t wiced_bt_dev_register_vendor_specific_event( wiced_bt_dev_vendor_specific_event_callback_t* p_event_callback, wiced_bool_t is_register );

/**
 * Function         wiced_bt_dev_set_discoverability
 *
 *                  Set discoverability
 *
 * @note            The duration must be less than or equal to the interval.
 *
 * @param[in]       inq_mode        : Discoverability mode (see #wiced_bt_discoverability_mode_e)
 * @param[in]       duration        : Duration (in 0.625 msec intervals). <b>BTM_DEFAULT_DISC_WINDOW</b>, or range: <b>0x0012 ~ 0x1000 </b> (11.25 ~ 2560 msecs)
 * @param[in]       interval        : Interval (in 0.625 msec intervals). <b>BTM_DEFAULT_DISC_INTERVAL</b>, or range: <b>0x0012 ~ 0x1000 </b> (11.25 ~ 2560 msecs)
 *
 * @return
 *
 *                  WICED_BT_SUCCESS:        If successful
 *                  WICED_BT_BUSY:           If a setting of the filter is already in progress
 *                  WICED_BT_NO_RESOURCES:   If couldn't get a memory pool buffer
 *                  WICED_BT_ILLEGAL_VALUE:  If a bad parameter was detected
 *                  WICED_BT_WRONG_MODE:     If the device is not up
 */
wiced_result_t  wiced_bt_dev_set_discoverability (uint8_t inq_mode, uint16_t duration,
                                                    uint16_t interval);

/**
 * Function         wiced_bt_dev_set_connectability
 *
 *                  Set connectablilty
 *
 * @note            The duration (window parameter) must be less than or equal to the interval.
 *
 * @param[in]       page_mode       : Connectability mode (see #wiced_bt_connectability_mode_e)
 * @param[in]       window          : Duration (in 0.625 msec intervals). <b>BTM_DEFAULT_CONN_WINDOW</b>, or range: <b>0x0012 ~ 0x1000 </b> (11.25 ~ 2560 msecs)
 * @param[in]       interval        : Interval (in 0.625 msec intervals). <b>BTM_DEFAULT_CONN_INTERVAL</b>, or range: <b>0x0012 ~ 0x1000 </b> (11.25 ~ 2560 msecs)
 *
 * @return
 *
 *                  WICED_BT_SUCCESS:        If successful
 *                  WICED_BT_ILLEGAL_VALUE:  If a bad parameter is detected
 *                  WICED_BT_NO_RESOURCES:   If could not allocate a message buffer
 *                  WICED_BT_WRONG_MODE:     If the device is not up
 */
wiced_result_t wiced_bt_dev_set_connectability (uint8_t page_mode, uint16_t window,
                                                      uint16_t interval);
/**
 * Function         wiced_bt_dev_register_connection_status_change
 *
 *                  Register callback for connection status change
 *
 *
 * @param[in]       p_wiced_bt_connection_status_change_cback - Callback for connection status change
 *
 * @return          wiced_result_t
 *
 *                  WICED_BT_SUCCESS : on success;
 *                  WICED_BT_FAILED : if an error occurred
 */
wiced_result_t wiced_bt_dev_register_connection_status_change(wiced_bt_connection_status_change_cback_t *p_wiced_bt_connection_status_change_cback);

/**
 * Function         wiced_bt_dev_set_sniff_mode
 *
 *                  Set a connection into sniff mode.
 *
 * @param[in]       remote_bda      : Link for which to put into sniff mode
 * @param[in]       min_period      : Minimum sniff period
 * @param[in]       max_period      : Maximum sniff period
 * @param[in]       attempt         : Number of attempts for switching to sniff mode
 * @param[in]       timeout         : Timeout for attempting to switch to sniff mode
 *
 * @return          WICED_BT_PENDING if successfully initiated, otherwise error
 */
wiced_result_t wiced_bt_dev_set_sniff_mode (wiced_bt_device_address_t remote_bda, uint16_t min_period,
                                             uint16_t max_period, uint16_t attempt,
                                             uint16_t timeout);


/**
 * Function         wiced_bt_dev_cancel_sniff_mode
 *
 *                  Take a connection out of sniff mode.
 *                  A check is made if the connection is already in sniff mode,
 *                  and if not, the cancel sniff mode is ignored.
 *
 * @return          WICED_BT_PENDING if successfully initiated, otherwise error
 *
 */
wiced_result_t wiced_bt_dev_cancel_sniff_mode (wiced_bt_device_address_t remote_bda);


/**
 *
 * Function         wiced_bt_dev_set_sniff_subrating
 *
 *                  Set sniff subrating parameters for an active connection
 *
 * @param[in]       remote_bda          : device address of desired ACL connection
 * @param[in]       max_latency         : maximum latency (in 0.625ms units) (range: 0x0002-0xFFFE)
 * @param[in]       min_remote_timeout  : minimum remote timeout
 * @param[in]       min_local_timeout   : minimum local timeout
 *
 * @return
 *
 *                  WICED_BT_SUCCESS        : on success;
 *                  WICED_BT_ILLEGAL_ACTION : if an error occurred
 */
wiced_result_t wiced_bt_dev_set_sniff_subrating (wiced_bt_device_address_t remote_bda, uint16_t max_latency,
                              uint16_t min_remote_timeout, uint16_t min_local_timeout);

/**
 * Function         wiced_bt_dev_read_rssi
 *
 *                  Get Receive Signal Strenth Index (RSSI) for the requested link
 *
 * @param[in]       remote_bda      : BD address of connection to read rssi
 * @param[in]       transport       : Transport type
 * @param[in]       p_cback         : Result callback (wiced_bt_dev_rssi_result_t will be passed to the callback)
 *
 * @return
 *
 *                  WICED_BT_PENDING if command issued to controller.
 *                  WICED_BT_NO_RESOURCES if couldn't allocate memory to issue command
 *                  WICED_BT_UNKNOWN_ADDR if no active link with bd addr specified
 *                  WICED_BT_BUSY if command is already in progress
 *
 */
wiced_result_t wiced_bt_dev_read_rssi (wiced_bt_device_address_t remote_bda, wiced_bt_transport_t transport, wiced_bt_dev_cmpl_cback_t *p_cback);

/**
 * Function         wiced_bt_dev_read_tx_power
 *
 *                  Read the transmit power for the requested link
 *
 * @param[in]       remote_bda      : BD address of connection to read tx power
 * @param[in]       transport       : Transport type
 * @param[in]       p_cback         : Result callback (wiced_bt_tx_power_result_t will be passed to the callback)
 *
 * @return
 *
 *                  WICED_BT_PENDING if command issued to controller.
 *                  WICED_BT_NO_RESOURCES if couldn't allocate memory to issue command
 *                  WICED_BT_UNKNOWN_ADDR if no active link with bd addr specified
 *                  WICED_BT_BUSY if command is already in progress
 *
 */
wiced_result_t wiced_bt_dev_read_tx_power (wiced_bt_device_address_t remote_bda, wiced_bt_transport_t transport,
                                            wiced_bt_dev_cmpl_cback_t *p_cback);


/**
 *
 * Function         wiced_bt_dev_write_eir
 *
 *                  Write EIR data to controller.
 *
 * @param[in]       p_buff   : EIR data
 * @param[in]       len      : Length of EIR data
 *
 * @return          WICED_BT_SUCCESS if successful
 *                  WICED_BT_NO_RESOURCES if couldn't allocate memory to issue command
 *                  WICED_BT_UNSUPPORTED if local device cannot support request
 *
 */
wiced_result_t wiced_bt_dev_write_eir (uint8_t *p_buff, uint16_t len);

/**@} wicedbt_bredr */

/*****************************************************************************
 *  SECURITY MANAGEMENT FUNCTIONS
 ****************************************************************************/
/****************************************************************************/
/**
 * This module defines the generic procedures related to discovery of Bluetooth devices and link
 * management aspects of connecting to Bluetooth devices. It also defines procedures related to use of
 * different security levels.
 *
 * @addtogroup  btm_sec_api_functions        Security
 * @ingroup     wicedbt_DeviceManagement
 *
 * @{
 */

/**
 * Function         wiced_bt_dev_pin_code_reply
 *
 *                  PIN code reply (use in response to <b>BTM_PIN_REQUEST_EVT </b> in #wiced_bt_management_cback_t)
 *
 *  @param[in]      bd_addr     : Address of the device for which PIN was requested
 *  @param[in]      res         : result of the operation WICED_BT_SUCCESS if success
 *  @param[in]      pin_len     : length in bytes of the PIN Code
 *  @param[in]      p_pin       : pointer to array with the PIN Code
 *
 * @return          void
 */
void wiced_bt_dev_pin_code_reply (wiced_bt_device_address_t bd_addr, wiced_result_t res, uint8_t pin_len, uint8_t *p_pin);

/**
 * Function         wiced_bt_dev_sec_bond
 *
 *                  Bond with peer device. If the connection is already up, but not secure, pairing is attempted.
 *
 *  @note           PIN parameters are only needed when bonding with legacy devices (pre-2.1 Core Spec)
 *
 *  @param[in]      bd_addr         : Peer device bd address to pair with.
 *  @param[in]      bd_addr_type    : BLE_ADDR_PUBLIC or BLE_ADDR_RANDOM (applies to LE devices only)
 *  @param[in]      transport       : BT_TRANSPORT_BR_EDR or BT_TRANSPORT_LE
 *  @param[in]      pin_len         : Length of input parameter p_pin (0 if not used).
 *  @param[in]      p_pin           : Pointer to Pin Code to use (NULL if not used).
 *
 *  @return
 *
 *                  WICED_BT_PENDING if successfully initiated,
 *                  WICED_BT_SUCCESS if already paired to the device, else
 *                  error code
 */
wiced_result_t wiced_bt_dev_sec_bond (wiced_bt_device_address_t bd_addr, wiced_bt_ble_address_type_t bd_addr_type, wiced_bt_transport_t transport, uint8_t pin_len, uint8_t *p_pin);


/**
 * Function         wiced_bt_dev_sec_bond_cancel
 *
 * Cancel an ongoing bonding process with peer device.
 *
 *  @param[in]      bd_addr         : Peer device bd address to pair with.
 *
 *  @return
 *
 *                  WICED_BT_PENDING if cancel initiated,
 *                  WICED_BT_SUCCESS if cancel has completed already, else error code.
 */
wiced_result_t wiced_bt_dev_sec_bond_cancel (wiced_bt_device_address_t bd_addr);


/**
 * Function         wiced_bt_dev_set_encryption
 *
 *                  Encrypt the specified connection.
 *                  Status is notified using <b>BTM_ENCRYPTION_STATUS_EVT </b> of #wiced_bt_management_cback_t.
 *
 *  @param[in]      bd_addr         : Address of peer device
 *  @param[in]      transport       : BT_TRANSPORT_BR_EDR or BT_TRANSPORT_LE
 *  @param[in]      p_ref_data      : pointer to reference data to be passed upon completion (NULL if no data)
 *
 * @return
 *
 *                  WICED_BT_SUCCESS            : already encrypted
 *                  WICED_BT_PENDING            : command will be returned in the callback
 *                  WICED_BT_WRONG_MODE         : connection not up.
 *                  WICED_BT_BUSY               : security procedures are currently active
 */
wiced_result_t wiced_bt_dev_set_encryption (wiced_bt_device_address_t bd_addr, wiced_bt_transport_t transport, void *p_ref_data);


/**
 * Function         wiced_bt_dev_confirm_req_reply
 *
 *                  Confirm the numeric value for pairing (in response to <b>BTM_USER_CONFIRMATION_REQUEST_EVT </b> of #wiced_bt_management_cback_t)
 *
 * @param[in]       res           : result of the operation WICED_BT_SUCCESS if success
 * @param[in]       bd_addr       : Address of the peer device
 *
 * @return          void
 */
void wiced_bt_dev_confirm_req_reply(wiced_result_t res, wiced_bt_device_address_t bd_addr);

/**
 * Function         wiced_bt_dev_pass_key_req_reply
 *
 *                  Provide the pairing passkey (in response to <b>BTM_PASSKEY_REQUEST_EVT </b> of #wiced_bt_management_cback_t)
 *
 * @param[in]       res           : result of the operation WICED_BT_SUCCESS if success
 * @param[in]       bd_addr       : Address of the peer device
 * @param[in]       passkey       : numeric value in the range of 0 - 999999(0xF423F).
 *
 * @return          void
 */
void wiced_bt_dev_pass_key_req_reply(wiced_result_t res, wiced_bt_device_address_t bd_addr, uint32_t passkey);

/**
 * Function         wiced_bt_dev_send_key_press_notif
 *
 *                  Inform remote device of keypress during pairing.
 *
 *                  Used during the passkey entry by a device with KeyboardOnly IO capabilities
 *                  (typically a HID keyboard device).
 *
 * @param[in]       bd_addr : Address of the peer device
 * @param[in]       type    : notification type
 *
 */
void wiced_bt_dev_send_key_press_notif(wiced_bt_device_address_t bd_addr, wiced_bt_dev_passkey_entry_type_t type);

/**
 *
 * Function         wiced_bt_dev_read_local_oob_data
 *
 *                  Read the local OOB data from controller (for sending
 *                  to peer device over oob message). When
 *                  operation is completed, local OOB data will be
 *                  provided via BTM_READ_LOCAL_OOB_DATA_COMPLETE_EVT.
 *
 */
wiced_result_t wiced_bt_dev_read_local_oob_data(void);

/**
 * Function         wiced_bt_dev_remote_oob_data_reply
 *
 *                  Provide the remote OOB extended data for Simple Pairing
 *                  in response to BTM_REMOTE_OOB_DATA_REQUEST_EVT
 *
 * @param[in]       bd_addr                 : Address of the peer device
 * @param[in]       is_extended_oob_data    : TRUE if extended OOB data (set according to BTM_REMOTE_OOB_DATA_REQUEST_EVT request)
 * @param[in]       c_192                   : simple pairing Hash C derived from the P-192 public key.
 * @param[in]       r_192                   : simple pairing Randomizer R associated with the P-192 public key.
 * @param[in]       c_256                   : simple pairing Hash C derived from the P-256 public key (if is_extended_oob_data=TRUE)
 * @param[in]       r_256                   : simple pairing Randomizer R associated with the P-256 public key (if is_extended_oob_data=TRUE)
 *
 */
void wiced_bt_dev_remote_oob_data_reply (wiced_result_t res, wiced_bt_device_address_t bd_addr,
                                              wiced_bool_t is_extended_oob_data,
                                              BT_OCTET16 c_192, BT_OCTET16 r_192,
                                              BT_OCTET16 c_256, BT_OCTET16 r_256);

/*
 *
 * Function         wiced_bt_dev_build_oob_data
 *
 *                  Build the OOB data block to be used to send OOB extended
 *                  data over OOB (non-Bluetooth) link.
 *
 * @param[out]      p_data                  : OOB data block location
 * @param[in]       max_len                 : OOB data block size
 * @param[in]       is_extended_oob_data    : TRUE if extended OOB data (for Secure Connections)
 * @param[in]       c_192                   : simple pairing Hash C derived from the P-192 public key.
 * @param[in]       r_192                   : simple pairing Randomizer R associated with the P-192 public key.
 * @param[in]       c_256                   : simple pairing Hash C derived from the P-256 public key (if is_extended_oob_data=TRUE)
 * @param[in]       r_256                   : simple pairing Randomizer R associated with the P-256 public key (if is_extended_oob_data=TRUE)
 *
 * @return          Number of bytes put into OOB data block.
 *
 */
uint16_t wiced_bt_dev_build_oob_data(uint8_t *p_data, uint16_t max_len,
                                          wiced_bool_t is_extended_oob_data,
                                          BT_OCTET16 c_192, BT_OCTET16 r_192,
                                          BT_OCTET16 c_256, BT_OCTET16 r_256);

/*
 *
 * Function         wiced_bt_smp_oob_data_reply
 *
 *                  This function is called to provide the OOB data for
 *                  SMP in response to BTM_SMP_REMOTE_OOB_DATA_REQUEST_EVT
 *
 *  @param[in]      bd_addr     - Address of the peer device
 *  @param[in]      res         - result of the operation WICED_BT_SUCCESS if success
 *  @param[in]      len         - oob data length
 *  @param[in]      p_data      - oob data
 *
 */
void wiced_bt_smp_oob_data_reply(wiced_bt_device_address_t bd_addr, wiced_result_t res, uint8_t len, uint8_t *p_data);

/**
 * Function         wiced_bt_smp_create_local_sc_oob_data
 *
 *                  Create local BLE SC (secure connection) OOB data. When
 *                  operation is completed, local OOB data will be
 *                  provided via BTM_SMP_SC_LOCAL_OOB_DATA_NOTIFICATION_EVT.
 *
 * @param[in]       bd_addr         : intended remote address for the OOB data
 * @param[in]       bd_addr_type    : BLE_ADDR_PUBLIC or BLE_ADDR_PUBLIC
 *
 * @return          TRUE: creation of local SC OOB data set started.
 *
 */
wiced_bool_t wiced_bt_smp_create_local_sc_oob_data (wiced_bt_device_address_t bd_addr, wiced_bt_ble_address_type_t bd_addr_type);

/**
 *
 * Function         wiced_bt_smp_sc_oob_reply
 *
 * Description      Provide the SC OOB data for SMP in response to
 *                  BTM_SMP_SC_REMOTE_OOB_DATA_REQUEST_EVT
 *
 * @param[in]       p_oob_data  : oob data
 *
 */
void wiced_bt_smp_sc_oob_reply (wiced_bt_smp_sc_oob_data_t *p_oob_data);

/**@} btm_sec_api_functions */

/**
 *
 * Function         wiced_bt_dev_read_oob_data
 *
 * Description      This function is called to parse the OOB data payload
 *                  received over OOB (non-Bluetooth) link
 *
 * @param[in]       p_data  : oob data
 * @param[in]       eir_tag : EIR Data type( version5.0, Volume 3, Part C Section 5.2.2.7 )
 * @param[out]      p_len   : the length of the data with the given EIR Data type
 *
 * @return          The beginning of the data with the given EIR Data type.
 *                  NULL, if the tag is not found.
 */
uint8_t* wiced_bt_dev_read_oob_data(uint8_t *p_data, uint8_t eir_tag, uint8_t *p_len);

/**@} wicedbt_DeviceManagement */


/** HCI trace types  */
typedef enum
{
    HCI_TRACE_EVENT, /**< HCI event data from controller to the host */
    HCI_TRACE_COMMAND, /**< HCI command data from host to controller */
    HCI_TRACE_INCOMING_ACL_DATA,/**< HCI incoming acl data */
    HCI_TRACE_OUTGOING_ACL_DATA/**< HCI outgoing acl data */
}wiced_bt_hci_trace_type_t;

/**
 * HCI trace callback
 *
 * Callback for HCI traces
 * Registered using wiced_bt_dev_register_hci_trace()
 *
 * @param[in] type       : Trace type
 * @param[in] length : Length of the trace data
 * @param[in] p_data  : Pointer to the data
 *
 * @return void
 */
typedef void ( wiced_bt_hci_trace_cback_t )( wiced_bt_hci_trace_type_t type, uint16_t length, uint8_t* p_data );

/**
 * Function         wiced_bt_dev_register_hci_trace
 *
 * Register to get the hci traces
 *
 * @param[in]      p_cback        : Callback for hci traces
 *
 * @return          void
 *
 */
void wiced_bt_dev_register_hci_trace( wiced_bt_hci_trace_cback_t* p_cback );


/**
 * Function         wiced_bt_dev_get_bonded_devices
 *
 *                  get bonded device list
 *
 * @param[out]      p_paired_device_list    : array for getting bd address of bonded devices
 * @param[in/out]   p_num_devices           : list size of p_pared_device_list total number of bonded devices stored
 *
 * @return          wiced_result_t
 *
 */
wiced_result_t wiced_bt_dev_get_bonded_devices(wiced_bt_dev_bonded_device_info_t *p_paired_device_list,uint16_t *p_num_devices);



/**
 * Function         wiced_bt_dev_delete_bonded_device
 *
 *   remove bonding with remote device with assigned bd_addr
 *
 * @param[in]      bd_addr :   bd_addr of remote device to be removed from bonding list
 *
 * @return          wiced_result_t
 *
 */
wiced_result_t wiced_bt_dev_delete_bonded_device(wiced_bt_device_address_t bd_addr);


/**
 * Function         wiced_bt_dev_add_device_to_address_resolution_db
 *
 *                  add link key information to internal address resolution db
 *
 * @param[in]      p_link_keys    : link keys information stored in application side
 *
 * @return          wiced_result_t
 *
 */
wiced_result_t wiced_bt_dev_add_device_to_address_resolution_db(wiced_bt_device_link_keys_t *p_link_keys);


/**
 * Function         wiced_bt_dev_remove_device_from_address_resolution_db
 *
 *                  remove link key information from internal address resolution db
 *
 * @param[in]      p_link_keys    : link keys information stored in application side
 *
 * @return          wiced_result_t
 *
 */
wiced_result_t wiced_bt_dev_remove_device_from_address_resolution_db(wiced_bt_device_link_keys_t *p_link_keys);


/**
 * Function         wiced_bt_dev_get_ble_keys
 *
 *                  get le key mask from stored key information of nv ram
 *
 * @param[in]      bd_addr    : remote bd address
 * @param[out]      p_key_mask    : ble key mask stored
 *
 * @return          wiced_result_t
 *
 */
wiced_result_t wiced_bt_dev_get_ble_keys(wiced_bt_device_address_t bd_addr, wiced_bt_dev_le_key_type_t *p_key_mask);

/**
 * Function         wiced_bt_set_local_bdaddr
 *
 *                  Set Local Bluetooth Device Address
 *
 * @param[in]      bd_addr    : device address to use
 *
 * @return          void
 *
 */
void wiced_bt_set_local_bdaddr( wiced_bt_device_address_t  bd_addr );

/**
 * Function         wiced_bt_dev_get_role
 *
 *                  This function is called to get the role of the local device
 *                  for the ACL connection with the specified remote device
 *
 * @param[in]       remote_bd_addr      : BD address of remote device
 * @param[in]       transport               : BT_TRANSPORT_BR_EDR or BT_TRANSPORT_LE
 *
 * @param[out]     p_role       : Role of the local device
 *
 * @return      WICED_BT_UNKNOWN_ADDR if no active link with bd addr specified
 *
 */
wiced_result_t wiced_bt_dev_get_role( wiced_bt_device_address_t remote_bd_addr, UINT8 *p_role, wiced_bt_transport_t transport );

/**
 * Function         wiced_bt_dev_get_security_state
 *
 *                  Get security flags for the device
 *
 * @param[in]       bd_addr         : peer address
 * @param[out]      p_sec_flags  : security flags (see #wiced_bt_sec_flags_e)
 *
 * @return          TRUE if successful
 *
 */
wiced_bool_t wiced_bt_dev_get_security_state(wiced_bt_device_address_t bd_addr, uint8_t *p_sec_flags);

/**
 * Function         wiced_bt_dev_get_low_power_mode
 *
 *                  Get Current low-power mode of device
 *
 * @return          TRUE if Low-power mode is enabled
 *
 */
wiced_bool_t wiced_bt_dev_get_low_power_mode(void);

/**
 * Function         wiced_bt_dev_set_low_power_mode
 *
 *                  Enable or Disable low-power mode of device
 *
 * @param[in]       enable_low_power  : 0 to disable, 1 to enable
 *
 * @return          WICED_BT_SUCCESS if successful
 *                  WICED_BT_UNSUPPORTED if Low-power Mode not supported by bluetooth stack
 *                  WICED_BT_ERROR if setting low-power mode failed
 *
 */
wiced_result_t wiced_bt_dev_set_low_power_mode(wiced_bool_t enable_low_power);

/**
 * Function         wiced_bt_dev_allow_host_sleep
 *
 *                  Read Host-Wake line to see if it is ok for Host to sleep
 *
 * @return          WICED_TRUE if it is ok for Host to sleep
 *                  WICED_FALSE otherwise
 *
 */
wiced_bool_t wiced_bt_dev_allow_host_sleep( void );

#ifdef USE_WICED_HCI
/**
 * Function         wiced_bt_dev_push_nvram_data
 * .....MCU host push all the saved NVRAM informatoin mainly paired device Info
 *
 * @param[in]       paired_device_info : Remote device address, Link key
 *
 * @return          WICED_BT_SUCCESS if successful
 *
 */
wiced_result_t wiced_bt_dev_push_nvram_data(wiced_bt_device_link_keys_t *paired_device_info);
#endif

/**
 *
 * Function         wiced_bt_set_tx_power
 *                  This command will adjust the transmit power attenuation on a per connection basis.
 * @param[in]       bd_addr : Remote device address
 * @param[in]       power    : input power to be set , range should be between -28 to 4 dbm  Min Val is 4dbm and Max val is -28
 *
 * @return          WICED_BT_SUCCESS if successful
 *                      WICED_BT_NO_RESOURCES if could not allocate resources to start the command
 *                      WICED_BT_UNSUPPORTED if set tx power is not supported by bluetooth stack
 *
 */
wiced_result_t wiced_bt_set_tx_power( wiced_bt_device_address_t bd_addr , int power );

/**
 * Function         wiced_bt_dev_get_remote_name
 *
 *                  Get BT Friendly name from remote device.
 *
 * @param[in]       bd_addr  : Peer bd address
 * @param[in]       p_remote_name_result_cback  : remote name result callback
 *
 * @return          wiced_result_t
 *
 *                  WICED_BT_PENDING if successfully initiated
 *                  WICED_BT_BUSY if already in progress
 *                  WICED_BT_ILLEGAL_VALUE if parameter(s) are out of range
 *                  WICED_BT_NO_RESOURCES if could not allocate resources to start the command
 *                  WICED_BT_WRONG_MODE if the device is not up.
 */
wiced_result_t  wiced_bt_dev_get_remote_name (wiced_bt_device_address_t bd_addr, wiced_bt_remote_name_cback_t *p_remote_name_result_cback);

/**
 * Function         wiced_bt_dev_get_remote_type
 *
 *                  Get type of the remote device.
 *
 * @param[in]       bd_addr  : Peer bd address
 *
 * @return          uint8_t
 *
 *                  #define BT_DEVICE_TYPE_BREDR   0x01
 *                  #define BT_DEVICE_TYPE_BLE     0x02
 *                  #define BT_DEVICE_TYPE_DUMO    0x03
 */
uint8_t wiced_bt_dev_get_remote_type (wiced_bt_device_address_t remote_bda);

#ifdef __cplusplus
}
#endif
