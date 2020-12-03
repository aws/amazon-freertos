/*
 * $ Copyright Cypress Semiconductor $
 */

/** @file
 *
 *  WICED Generic Attribute (GATT) Application Programming Interface
 */
#pragma once

#include "wiced_result.h"
#include "gattdefs.h"
#include "wiced_bt_dev.h"
#include "l2cdefs.h"
#include "wiced_bt_types.h"


/**
 * @addtogroup wicedbt_gatt
 * @{
 */

/** GATT Status Codes*/
enum wiced_bt_gatt_status_e
{
    WICED_BT_GATT_SUCCESS                    = 0x00,         /**< Success */
    WICED_BT_GATT_INVALID_HANDLE             = 0x01,         /**< Invalid Handle */
    WICED_BT_GATT_READ_NOT_PERMIT            = 0x02,         /**< Read Not Permitted */
    WICED_BT_GATT_WRITE_NOT_PERMIT           = 0x03,         /**< Write Not permitted */
    WICED_BT_GATT_INVALID_PDU                = 0x04,         /**< Invalid PDU */
    WICED_BT_GATT_INSUF_AUTHENTICATION       = 0x05,         /**< Insufficient Authentication */
    WICED_BT_GATT_REQ_NOT_SUPPORTED          = 0x06,         /**< Request Not Supported */
    WICED_BT_GATT_INVALID_OFFSET             = 0x07,         /**< Invalid Offset */
    WICED_BT_GATT_INSUF_AUTHORIZATION        = 0x08,         /**< Insufficient Authorization */
    WICED_BT_GATT_PREPARE_Q_FULL             = 0x09,         /**< Prepare Queue Full */
    WICED_BT_GATT_NOT_FOUND                  = 0x0a,         /**< Not Found */
    WICED_BT_GATT_NOT_LONG                   = 0x0b,         /**< Not Long Size */
    WICED_BT_GATT_INSUF_KEY_SIZE             = 0x0c,         /**< Insufficient Key Size */
    WICED_BT_GATT_INVALID_ATTR_LEN           = 0x0d,         /**< Invalid Attribute Length */
    WICED_BT_GATT_ERR_UNLIKELY               = 0x0e,         /**< Error Unlikely */
    WICED_BT_GATT_INSUF_ENCRYPTION           = 0x0f,         /**< Insufficient Encryption */
    WICED_BT_GATT_UNSUPPORT_GRP_TYPE         = 0x10,         /**< Unsupported Group Type */
    WICED_BT_GATT_INSUF_RESOURCE             = 0x11,         /**< Insufficient Resource */
    WICED_BT_GATT_DATABASE_OUT_OF_SYNC       = 0x12,         /**< GATT Database Out of Sync */
	WICED_BT_GATT_VALUE_NOT_ALLOWED          = 0x13,         /**< Value Not allowed */

    WICED_BT_GATT_ILLEGAL_PARAMETER          = 0x87,         /**< Illegal Parameter */
    WICED_BT_GATT_NO_RESOURCES               = 0x80,         /**< No Resources */
    WICED_BT_GATT_INTERNAL_ERROR             = 0x81,         /**< Internal Error */
    WICED_BT_GATT_WRONG_STATE                = 0x82,         /**< Wrong State */
    WICED_BT_GATT_DB_FULL                    = 0x83,         /**< DB Full */
    WICED_BT_GATT_BUSY                       = 0x84,         /**< Busy */
    WICED_BT_GATT_ERROR                      = 0x85,         /**< Error */
    WICED_BT_GATT_CMD_STARTED                = 0x86,         /**< Command Started */
    WICED_BT_GATT_PENDING                    = 0x88,         /**< Pending */
    WICED_BT_GATT_AUTH_FAIL                  = 0x89,         /**< Authentication Fail */
    WICED_BT_GATT_MORE                       = 0x8a,         /**< More */
    WICED_BT_GATT_INVALID_CFG                = 0x8b,         /**< Invalid Configuration */
    WICED_BT_GATT_SERVICE_STARTED            = 0x8c,         /**< Service Started */
    WICED_BT_GATT_ENCRYPED_MITM              = WICED_BT_GATT_SUCCESS, /**< Encrypted MITM */
    WICED_BT_GATT_ENCRYPED_NO_MITM           = 0x8d,         /**< Encrypted No MITM */
    WICED_BT_GATT_NOT_ENCRYPTED              = 0x8e,         /**< Not Encrypted */
    WICED_BT_GATT_CONGESTED                  = 0x8f,         /**< Congested */
    WICED_BT_GATT_HANDLED                    = 0x90,         /**< Set by application to indicate it has responded to the message */
    WICED_BT_GATT_NO_PENDING_OPERATION       = 0x91,         /**< No pending operation for the response sent by app */

                                                    /* 0xE0 ~ 0xFB reserved for future use */
    WICED_BT_GATT_WRITE_REQ_REJECTED         = 0xFC,         /**< Write operation rejected */
    WICED_BT_GATT_CCC_CFG_ERR                = 0xFD,         /**< Improper Client Char Configuration */
    WICED_BT_GATT_PRC_IN_PROGRESS            = 0xFE,         /**< Procedure Already in Progress */
    WICED_BT_GATT_OUT_OF_RANGE               = 0xFF          /**< Value Out of Range */
};
typedef uint8_t wiced_bt_gatt_status_t;     /**< GATT status (see #wiced_bt_gatt_status_e) */


/** GATT Operation Codes */
#define  GATT_RSP_ERROR                  0x01       /**< Error Response */
#define  GATT_REQ_MTU                    0x02       /**< Exchange MTU Request */
#define  GATT_RSP_MTU                    0x03       /**< Exchange MTU Response */
#define  GATT_REQ_FIND_INFO              0x04       /**< Find Information Request */
#define  GATT_RSP_FIND_INFO              0x05       /**< Find Information Response */
#define  GATT_REQ_FIND_TYPE_VALUE        0x06       /**< Find By Type Value Request */
#define  GATT_RSP_FIND_TYPE_VALUE        0x07       /**< Find By Type Value Response */
#define  GATT_REQ_READ_BY_TYPE           0x08       /**< Read By Type Request */
#define  GATT_RSP_READ_BY_TYPE           0x09       /**< Read By Type Response */
#define  GATT_REQ_READ                   0x0A       /**< Read Request */
#define  GATT_RSP_READ                   0x0B       /**< Read Response */
#define  GATT_REQ_READ_BLOB              0x0C       /**< Read Blob Request */
#define  GATT_RSP_READ_BLOB              0x0D       /**< Read Blob Response */
#define  GATT_REQ_READ_MULTI             0x0E       /**< Read Multiple Request */
#define  GATT_RSP_READ_MULTI             0x0F       /**< Read Multiple Response */
#define  GATT_REQ_READ_BY_GRP_TYPE       0x10       /**< Read By Group Type Request */
#define  GATT_RSP_READ_BY_GRP_TYPE       0x11       /**< Read By Group Type Response */
#define  GATT_REQ_WRITE                  0x12       /**< Write Request */
#define  GATT_RSP_WRITE                  0x13       /**< Write Request */
#define  GATT_REQ_PREPARE_WRITE          0x16       /**< Prepare Write Request */
#define  GATT_RSP_PREPARE_WRITE          0x17       /**< Prepare Write Response */
#define  GATT_REQ_EXEC_WRITE             0x18       /**< Execute Write Request */
#define  GATT_RSP_EXEC_WRITE             0x19       /**< Execute Write Response */
#define  GATT_HANDLE_VALUE_NOTIF         0x1B       /**< Handle Value Notification */
#define  GATT_HANDLE_VALUE_IND           0x1D       /**< Handle Value Indication */
#define  GATT_HANDLE_VALUE_CONF          0x1E       /**< Handle Value Confirmation */
#define  GATT_REQ_READ_MULTI_VAR_LENGTH  0x20       /**< Read Multiple Variable Length Request */
#define  GATT_RSP_READ_MULTI_VAR_LENGTH  0x21       /**< Read Multiple Variable Length Response */
#define  GATT_HANDLE_VALUE_MULTI_NOTIF   0x23       /**< Handle Value Multiple Notifications */

#define  GATT_OP_CODE_MAX                (GATT_HANDLE_VALUE_MULTI_NOTIF + 1) /**<  Maximum opcode value */
#define  GATT_CMD_WRITE                  0x52       /**< Write Command */
#define  GATT_SIGN_CMD_WRITE             0xD2       /**< changed in V4.0 1101-0010 (signed write)  see write cmd above*/

/**  GATT Disconnection reason */
enum wiced_bt_gatt_disconn_reason_e {
    GATT_CONN_UNKNOWN                       = 0,                                    /**< Unknown reason */
    GATT_CONN_L2C_FAILURE                   = 1,                                    /**< General L2cap failure  */
    GATT_CONN_TIMEOUT                       = HCI_ERR_CONNECTION_TOUT,              /**< Connection timeout  */
    GATT_CONN_TERMINATE_PEER_USER           = HCI_ERR_PEER_USER,                    /**< Connection terminated by peer user  */
    GATT_CONN_TERMINATE_LOCAL_HOST          = HCI_ERR_CONN_CAUSE_LOCAL_HOST,        /**< Connection terminated by local host  */
    GATT_CONN_FAIL_ESTABLISH                = HCI_ERR_CONN_FAILED_ESTABLISHMENT,    /**< Connection fail to establish  */
    GATT_CONN_LMP_TIMEOUT                   = HCI_ERR_LMP_RESPONSE_TIMEOUT,         /**< Connection fail due to LMP response tout */
    GATT_CONN_CANCEL                        = L2CAP_CONN_CANCEL                     /**< L2CAP connection cancelled  */
};
typedef uint16_t wiced_bt_gatt_disconn_reason_t;    /**< GATT disconnection reason (see #wiced_bt_gatt_disconn_reason_e) */

/** default GATT MTU size over LE link
 */
#define GATT_DEF_BLE_MTU_SIZE 23

/** invalid connection ID
 */
#define GATT_INVALID_CONN_ID 0xFFFF

/** characteristic descriptor: client configuration value */
enum wiced_bt_gatt_client_char_config_e
{
    GATT_CLIENT_CONFIG_NONE          = 0x0000,      /**< Does not allow both notifications and indications */
    GATT_CLIENT_CONFIG_NOTIFICATION  = 0x0001,      /**< Allows notifications  */
    GATT_CLIENT_CONFIG_INDICATION    = 0x0002       /**< Allows indications  */
};
typedef uint16_t wiced_bt_gatt_client_char_config_t;     /**< GATT client config (see #wiced_bt_gatt_client_char_config_e) */

/** characteristic descriptor: server configuration value */
enum wiced_bt_gatt_server_char_config_e
{
    GATT_SERVER_CONFIG_NONE = 0x0000,     /**< No broadcast   */
    GATT_SERVER_CONFIG_BROADCAST = 0x0001 /**< Broadcast      */
};
typedef uint16_t wiced_bt_gatt_server_char_config_t; /**< GATT server config (see #wiced_bt_gatt_server_char_config_e) */

/**  GATT Characteristic Properties Mask */
enum wiced_bt_gatt_char_properties_e {
    GATT_CHAR_PROPERTIES_BIT_BROADCAST      = (1 << 0),     /**< bit 0: Broadcast */
    GATT_CHAR_PROPERTIES_BIT_READ           = (1 << 1),     /**< bit 1: Read */
    GATT_CHAR_PROPERTIES_BIT_WRITE_NR       = (1 << 2),     /**< bit 2: Write (No Response) */
    GATT_CHAR_PROPERTIES_BIT_WRITE          = (1 << 3),     /**< bit 3: Write */
    GATT_CHAR_PROPERTIES_BIT_NOTIFY         = (1 << 4),     /**< bit 4: Notify */
    GATT_CHAR_PROPERTIES_BIT_INDICATE       = (1 << 5),     /**< bit 5: Indicate */
    GATT_CHAR_PROPERTIES_BIT_AUTH           = (1 << 6),     /**< bit 6: Authenticate */
    GATT_CHAR_PROPERTIES_BIT_EXT_PROP       = (1 << 7)      /**< bit 7: Extended Properties */
};
typedef uint8_t wiced_bt_gatt_char_properties_t;            /**< GATT characteristic properties mask (see #wiced_bt_gatt_char_properties_e) */

/** Authentication requirement */
enum wiced_bt_gatt_auth_req_e {
    GATT_AUTH_REQ_NONE                  = 0,    /**< No Authentication Required */
    GATT_AUTH_REQ_NO_MITM               = 1,    /**< Unauthenticated encryption (No MITM) */
    GATT_AUTH_REQ_MITM                  = 2,    /**< Authenticated encryption (MITM) */
    GATT_AUTH_REQ_SIGNED_NO_MITM        = 3,    /**< Signed Data (No MITM) */
    GATT_AUTH_REQ_SIGNED_MITM           = 4     /**< Signed Data (MITM) */
};
typedef uint8_t wiced_bt_gatt_auth_req_t;   /**< GATT authentication requirement (see #wiced_bt_gatt_auth_req_e)*/

/** Attribute value, used for GATT write operations, and read response callbacks */
typedef struct
{
    uint16_t                    handle;                     /**< Attribute handle */
    uint16_t                    offset;                     /**< Attribute value offset, ignored if not needed for a command */
    uint16_t                    len;                        /**< Length of attribute value */
    wiced_bt_gatt_auth_req_t    auth_req;                   /**< Authentication requirement (see @link wiced_bt_gatt_auth_req_e wiced_bt_gatt_auth_req_t @endlink) */
    uint8_t                     value[1];                   /**< The attribute value (actual length is specified by 'len') */
} wiced_bt_gatt_value_t;

/** Defines the macro to get the GATT Response Structure Size. Substracting 1 to take care of size of the placeholder array which holds the start of the attribute value */
#define GATT_RESPONSE_SIZE( attr_val_len ) ( sizeof( wiced_bt_gatt_value_t ) - 1 + attr_val_len )

/** GATT Write Execute request flags */
enum wiced_bt_gatt_exec_flag_e {
    GATT_PREPARE_WRITE_CANCEL      = 0x00,         /**< GATT_PREP_WRITE_CANCEL */
    GATT_PREPARE_WRITE_EXEC        = 0x01          /**< GATT_PREP_WRITE_EXEC */
};

#define GATT_PREP_WRITE_CANCEL GATT_PREPARE_WRITE_CANCEL  /**< See #GATT_PREPARE_WRITE_CANCEL */
#define GATT_PREP_WRITE_EXEC   GATT_PREPARE_WRITE_EXEC    /**< See #GATT_PREPARE_WRITE_EXEC */

typedef uint8_t   wiced_bt_gatt_exec_flag_t;    /**< GATT execute flag (see #wiced_bt_gatt_exec_flag_e) */

/** Attribute read request */
typedef struct
{
    uint16_t        handle;     /**< Handle of attribute to read */
    uint16_t        offset;     /**< Offset to read */
    uint16_t        *p_val_len; /**< input and output parameter for value length */
    uint8_t         *p_val;     /**< Value pointer */
} wiced_bt_gatt_read_t;

/** Attribute write request */
typedef struct
{
    uint16_t      handle;     /**< Handle of attribute to write */
    wiced_bool_t  is_prep;    /**< TRUE if this is a prepare write request */
    wiced_bool_t  is_req;    /**< TRUE if this is a write request */
    uint16_t      offset;     /**< Offset to write */
    uint16_t      val_len;    /**< Value length */
    uint8_t       *p_val;     /**< Value pointer */
} wiced_bt_gatt_write_t;

/** Attribute validate read request */
typedef struct
{
    uint16_t      handle;     /**< Handle of attribute to write */
} wiced_bt_gatt_validate_read_t;


/** Attribute information for GATT attribute requests */
typedef union
{
    wiced_bt_gatt_read_t            read_req;            /**< Parameters for GATTS_REQ_TYPE_READ */
    wiced_bt_gatt_write_t           write_req;           /**< Parameters for GATTS_REQ_TYPE_WRITE */
    uint16_t                        handle;              /**< Parameters for GATTS_REQ_TYPE_CONF */
    uint16_t                        mtu;                 /**< Parameters for GATTS_REQ_TYPE_MTU */
    wiced_bt_gatt_exec_flag_t       exec_write;          /**< Parameters for GATTS_REQ_TYPE_WRITE_EXEC */
    wiced_bt_gatt_validate_read_t   validate_read_req;   /**< Parameters for validating read for multi-handle read requests */
} wiced_bt_gatt_request_data_t;

/** GATT Attribute Request Type */
enum wiced_bt_gatt_request_type_e
{
    GATTS_REQ_TYPE_READ = 1,        /**< Attribute read notification (attribute value internally read from GATT database) */
    GATTS_REQ_TYPE_WRITE,           /**< Attribute write notification (attribute value internally written to GATT database) */
    GATTS_REQ_TYPE_WRITE_REQ,       /**< Attribute write req notification (attribute value internally written to GATT database) */
    GATTS_REQ_TYPE_PREP_WRITE,      /**< Attribute Prepare Write Notification (Suspending write request before triggering actual execute write ) */
    GATTS_REQ_TYPE_WRITE_EXEC,      /**< Execute write request */
    GATTS_REQ_TYPE_MTU,             /**< MTU exchange information */
    GATTS_REQ_TYPE_CONF,            /**< Value confirmation */
    GATTS_REQ_TYPE_VALIDATE_READ    /**< Validate attribute read request */
};
typedef uint8_t wiced_bt_gatt_request_type_t;   /**< GATT Attribute Request Type (see #wiced_bt_gatt_request_type_e) */

/** Discovery types */
enum wiced_bt_gatt_discovery_type_e
{
    GATT_DISCOVER_SERVICES_ALL = 1,             /**< discover all services */
    GATT_DISCOVER_SERVICES_BY_UUID,             /**< discover service by UUID */
    GATT_DISCOVER_INCLUDED_SERVICES,            /**< discover an included service within a service */
    GATT_DISCOVER_CHARACTERISTICS,              /**< discover characteristics of a service with/without type requirement */
    GATT_DISCOVER_CHARACTERISTIC_DESCRIPTORS,   /**< discover characteristic descriptors of a character */
    GATT_DISCOVER_MAX                           /**< maximum discovery types */
};
typedef uint8_t wiced_bt_gatt_discovery_type_t;    /**< GATT Discovery type (see #wiced_bt_gatt_discovery_type_e) */

/** Parameters used in a GATT Discovery */
typedef struct
{
    wiced_bt_uuid_t uuid;        /**< Service or Characteristic UUID */
    uint16_t        s_handle;    /**< Start handle for range to search */
    uint16_t        e_handle;    /**< End handle for range to search */
}wiced_bt_gatt_discovery_param_t;

/** GATT Read Types */
enum wiced_bt_gatt_read_type_e
{
    GATT_READ_BY_TYPE = 1,      /**< Read by Type (service or characteristic UUIDs) */
    GATT_READ_BY_HANDLE,        /**< Read by Handle */
    GATT_READ_MULTIPLE,             /**< Read Multiple handle values (client should know value lengths for handles while making read multiple request) */
    GATT_READ_MULTIPLE_VAR_LENGTH,  /**< Read Multiple handle values (client does not know value lengths, hence response is length_value tuple) */
    GATT_READ_CHAR_VALUE,       /**< Read Characteristic Value */
    GATT_READ_PARTIAL,          /**< Read Partial */
    GATT_READ_MAX
};
typedef uint8_t wiced_bt_gatt_read_type_t;      /**< GATT read type (see #wiced_bt_gatt_read_type_e) */

/** Parameters for GATT_READ_BY_TYPE and GATT_READ_CHAR_VALUE */
typedef struct
{
    wiced_bt_gatt_auth_req_t    auth_req;       /**< Authentication requirement (see @link wiced_bt_gatt_auth_req_e wiced_bt_gatt_auth_req_t @endlink) */
    uint16_t                    s_handle;       /**< Starting handle */
    uint16_t                    e_handle;       /**< Ending handle */
    wiced_bt_uuid_t             uuid;           /**< uuid */
} wiced_bt_gatt_read_by_type_t;

#define GATT_MAX_READ_MULTI_HANDLES      10     /**< Max attributes allowed in one GATT_READ_MULTIPLE request */
/** Parameters for GATT_READ_MULTIPLE */
typedef struct
{
    wiced_bt_gatt_auth_req_t    auth_req;                               /**< authentication requirement (see @link wiced_bt_gatt_auth_req_e wiced_bt_gatt_auth_req_t @endlink) */
    uint16_t                    num_handles;                            /**< number of handles to read */
    uint16_t                    handles[GATT_MAX_READ_MULTI_HANDLES];   /**< handles list to be read */
} wiced_bt_gatt_read_multi_t;

/** Parameters for GATT_READ_BY_HANDLE */
typedef struct
{
    wiced_bt_gatt_auth_req_t    auth_req;    /**< authentication requirement (see @link wiced_bt_gatt_auth_req_e wiced_bt_gatt_auth_req_t @endlink) */
    uint16_t                    handle;      /**< handle */
} wiced_bt_gatt_read_by_handle_t;

/** Parameters for GATT_READ_PARTIAL */
typedef struct
{
    wiced_bt_gatt_auth_req_t    auth_req;    /**< authentication requirement (see @link wiced_bt_gatt_auth_req_e wiced_bt_gatt_auth_req_t @endlink) */
    uint16_t                    handle;      /**< handle */
    uint16_t                    offset;      /**< offset */
} wiced_bt_gatt_read_partial_t;

/** Read request parameters - used when calling #wiced_bt_gatt_send_read */
typedef union
{
    wiced_bt_gatt_read_by_type_t   service;           /**< Parameters for GATT_READ_BY_TYPE */
    wiced_bt_gatt_read_by_type_t   char_type;         /**< Parameters for GATT_READ_CHAR_VALUE */
    wiced_bt_gatt_read_multi_t     read_multiple;     /**< Parameters for GATT_READ_MULTIPLE */
    wiced_bt_gatt_read_by_handle_t by_handle;         /**< Parameters for GATT_READ_BY_HANDLE */
    wiced_bt_gatt_read_partial_t   partial;           /**< Parameters for GATT_READ_PARTIAL  */
} wiced_bt_gatt_read_param_t;

/**  Write request types - used when calling #wiced_bt_gatt_send_write */
enum wiced_bt_gatt_write_type_e
{
    GATT_WRITE_NO_RSP = 1,  /**< Write without response */
    GATT_WRITE,             /**< Write with response */
    GATT_WRITE_PREPARE      /**< Prepare to write (call #wiced_bt_gatt_send_execute_write to execute the write) */
};
typedef uint8_t wiced_bt_gatt_write_type_t;     /**< GATT write type (see #wiced_bt_gatt_write_type_e) */

/**  Response data for read operations */
typedef struct
{
    uint16_t                    handle;     /**< handle */
    uint16_t                    len;        /**< length of response data or notification or indication data.
                                                 In case of multiple notifications, it is total length of
                                                 notification tuples pointed by p_data */
    uint16_t                    offset;     /**< offset */
    uint8_t                     *p_data;    /**< attribute data. In case of multiple notifications,
                                                 it is array of handle length and notification data tuples */
} wiced_bt_gatt_data_t;

/** Client Operation Complete response data (dependent on operation completed) */
typedef union
{
    wiced_bt_gatt_data_t    att_value;      /**< Response data for read operations (initiated using #wiced_bt_gatt_send_read)
                                                 (or) Notification or Inidcation data
                                                 (or) Handle length value tuple in case of multiple notifications.*/
    uint16_t                mtu;            /**< Response data for configuration operations */
    uint16_t                handle;         /**< Response data for write operations (initiated using #wiced_bt_gatt_send_write) */
} wiced_bt_gatt_operation_complete_rsp_t;   /**< GATT operation complete response type */

/** Parameters for sending multiple variable length notifications */
typedef struct
{
    uint16_t    handle;                     /**< handle */
    uint16_t    length;                     /**< length of data */
    uint8_t     *p_val;                     /**< pointer to data */
}wiced_bt_gatt_handle_length_value_t;

/** GATT client operation type, used in client callback function
*/
enum wiced_bt_gatt_optype_e
{
    GATTC_OPTYPE_NONE             = 0,    /**< None      */
    GATTC_OPTYPE_DISCOVERY        = 1,    /**< Discovery */
    GATTC_OPTYPE_READ             = 2,    /**< Read      */
    GATTC_OPTYPE_WRITE            = 3,    /**< Write     */
    GATTC_OPTYPE_EXE_WRITE        = 4,    /**< Execute Write */
    GATTC_OPTYPE_CONFIG           = 5,    /**< Configure */
    GATTC_OPTYPE_NOTIFICATION     = 6,    /**< Notification */
    GATTC_OPTYPE_INDICATION             = 7,    /**< Indication */
    GATTC_OPTYPE_MULTIPLE_NOTIFICATION_COMPLETE = 8,    /**< Multiple variable Length Notifications receive complete */
    GATTC_OPTYPE_READ_COMPLETE    = 9,    /**< Read operation complete,
                                              application can now send next read request if desired
                                          */
};

/**GATT_PREP_WRITE_CANCEL GATT Client Operation Codes */
typedef uint8_t wiced_bt_gatt_optype_t; /**< GATT operation type (see #wiced_bt_gatt_optype_e) */

/** GATT caching status of the peer(client) */
enum wiced_bt_gatt_caching_status_e
{
    GATT_PEER_CLIENT_CACHE_CHANGE_AWARE   = 0,     /**< Peer client is cache aware   */
    GATT_PEER_CLIENT_CACHE_CHANGE_UNAWARE = 1,     /**< Peer client is cache unaware */
    GATT_PEER_CLIENT_CACHE_READY_TO_BE_AWARE = 2   /**< Peer client is reading the database hash */
};

typedef uint8_t wiced_bt_gatt_caching_status_t; /**< GATT peer caching status (see #wiced_bt_gatt_caching_status_e) */

/** characteristic declaration */
typedef struct
{
    wiced_bt_gatt_char_properties_t characteristic_properties;  /**< characteristic properties (see @link wiced_bt_gatt_char_properties_e wiced_bt_gatt_char_properties_t @endlink) */
    uint16_t                        val_handle;                 /**< characteristic value attribute handle */
    uint16_t                        handle;                     /**< characteristic declaration handle */
    wiced_bt_uuid_t                 char_uuid;                  /**< characteristic UUID type */
} wiced_bt_gatt_char_declaration_t;

/** GATT group value */
typedef struct
{
    wiced_bt_uuid_t service_type;   /**< group type */
    uint16_t        s_handle;       /**< starting handle of the group */
    uint16_t        e_handle;       /**< ending handle of the group */
} wiced_bt_gatt_group_value_t;


/** included service attribute value */
typedef struct
{
    wiced_bt_uuid_t service_type;   /**< included service UUID */
    uint16_t        handle;         /**< included service handle */
    uint16_t        s_handle;       /**< starting handle */
    uint16_t        e_handle;       /**< ending handle */
} wiced_bt_gatt_included_service_t;

/** characteristic descriptor information */
typedef struct
{
    wiced_bt_uuid_t         type;      /**< descriptor UUID type */
    uint16_t                handle;    /**< descriptor attribute handle */
}wiced_bt_gatt_char_descr_info_t;


/**
 * Discovery result data
 * Use  GATT_DISCOVERY_RESULT_SERVICE_* or GATT_DISCOVERY_RESULT_CHARACTERISTIC_* macros to parse discovery data)
 */
typedef union
{
    wiced_bt_gatt_included_service_t    included_service;           /**< Result for GATT_DISCOVER_INCLUDED_SERVICES */
    wiced_bt_gatt_group_value_t         group_value;                /**< Result for GATT_DISCOVER_SERVICES_ALL or GATT_DISCOVER_SERVICES_BY_UUID  */
    wiced_bt_gatt_char_declaration_t    characteristic_declaration; /**< Result for GATT_DISCOVER_CHARACTERISTICS */
    wiced_bt_gatt_char_descr_info_t     char_descr_info;            /**< Result for GATT_DISCOVER_CHARACTERISTIC_DESCRIPTORS */
} wiced_bt_gatt_discovery_data_t;

typedef uint16_t wiced_bt_gatt_appearance_t;     /**< GATT appearance (see gatt_appearance_e in gattdefs.h) */


/*****************************************************************************
 *  GATT Database Defintions
 *****************************************************************************/
#define GATTDB_PERM_NONE                             (0x00)      /**<  The permission bits (see Vol 3, Part F, 3.2.5) */
#define GATTDB_PERM_VARIABLE_LENGTH                  (0x1 << 0)  /**<  The permission bits (see Vol 3, Part F, 3.2.5) */
#define GATTDB_PERM_READABLE                         (0x1 << 1)  /**<  The permission bits (see Vol 3, Part F, 3.2.5) */
#define GATTDB_PERM_WRITE_CMD                        (0x1 << 2)  /**<  The permission bits (see Vol 3, Part F, 3.2.5) */
#define GATTDB_PERM_WRITE_REQ                        (0x1 << 3)  /**<  The permission bits (see Vol 3, Part F, 3.2.5) */
#define GATTDB_PERM_AUTH_READABLE                    (0x1 << 4)  /**<  The permission bits (see Vol 3, Part F, 3.2.5) */
#define GATTDB_PERM_RELIABLE_WRITE                   (0x1 << 5)  /**<  The permission bits (see Vol 3, Part F, 3.2.5) */
#define GATTDB_PERM_AUTH_WRITABLE                    (0x1 << 6)  /**<  The permission bits (see Vol 3, Part F, 3.2.5) */

#define GATTDB_PERM_WRITABLE  (GATTDB_PERM_WRITE_CMD | GATTDB_PERM_WRITE_REQ| GATTDB_PERM_AUTH_WRITABLE)  /**<  The permission bits (see Vol 3, Part F, 3.2.5) */
#define GATTDB_PERM_MASK                             (0x7f)  /**< All the permission bits. */
#define GATTDB_PERM_SERVICE_UUID_128                 (0x1 << 7)  /**<  The permission bits (see Vol 3, Part F, 3.2.5) */

#define GATTDB_CHAR_PROP_BROADCAST                  (0x1 << 0)  /**< GATT Characteristic Properties (see Vol 3, Part G, 3.3.1.1) */
#define GATTDB_CHAR_PROP_READ                       (0x1 << 1)  /**< GATT Characteristic Properties (see Vol 3, Part G, 3.3.1.1) */
#define GATTDB_CHAR_PROP_WRITE_NO_RESPONSE          (0x1 << 2)  /**< GATT Characteristic Properties (see Vol 3, Part G, 3.3.1.1) */
#define GATTDB_CHAR_PROP_WRITE                      (0x1 << 3)  /**< GATT Characteristic Properties (see Vol 3, Part G, 3.3.1.1) */
#define GATTDB_CHAR_PROP_NOTIFY                     (0x1 << 4)  /**< GATT Characteristic Properties (see Vol 3, Part G, 3.3.1.1) */
#define GATTDB_CHAR_PROP_INDICATE                   (0x1 << 5)  /**< GATT Characteristic Properties (see Vol 3, Part G, 3.3.1.1) */
#define GATTDB_CHAR_PROP_AUTHD_WRITES               (0x1 << 6)  /**< GATT Characteristic Properties (see Vol 3, Part G, 3.3.1.1) */
#define GATTDB_CHAR_PROP_EXTENDED                   (0x1 << 7)  /**< GATT Characteristic Properties (see Vol 3, Part G, 3.3.1.1) */

#define GATTDB_CHAR_EXTENDED_PROP_RELIABLE             (0x1 << 0)  /**< GATT Characteristic Extended Properties (see Vol 3, Part G, 3.3.3.1) */
#define GATTDB_CHAR_EXTENDED_PROP_WRITABLE_AUXILIARIES (0x1 << 1)  /**< GATT Characteristic Extended Properties (see Vol 3, Part G, 3.3.3.1) */

/** Conversion macros */
#define BIT16_TO_8( val ) \
    (uint8_t)(  (val)        & 0xff),/* LSB */ \
    (uint8_t)(( (val) >> 8 ) & 0xff) /* MSB */

#define GATTDB_UUID16_SIZE                 2   /**< UUID lengths */
#define GATTDB_UUID128_SIZE                16  /**< UUID lengths */

/** Since the GATT DB macro names were changed from LEGATTDB_xxx from GATTDB_xxx,
 * these backwards compatibility macros are temporarily defined to minimize impact
 * on applications. It is expected that they wll be deprecated over time.
 */
#define LEGATTDB_PERM_NONE                    GATTDB_PERM_NONE                    /**< To be deprecated, defined for backward compatiablility */
#define LEGATTDB_PERM_VARIABLE_LENGTH         GATTDB_PERM_VARIABLE_LENGTH         /**< To be deprecated, defined for backward compatiablility */
#define LEGATTDB_PERM_READABLE                GATTDB_PERM_READABLE                /**< To be deprecated, defined for backward compatiablility */
#define LEGATTDB_PERM_WRITE_CMD               GATTDB_PERM_WRITE_CMD               /**< To be deprecated, defined for backward compatiablility */
#define LEGATTDB_PERM_WRITE_REQ               GATTDB_PERM_WRITE_REQ               /**< To be deprecated, defined for backward compatiablility */
#define LEGATTDB_PERM_AUTH_READABLE           GATTDB_PERM_AUTH_READABLE           /**< To be deprecated, defined for backward compatiablility */
#define LEGATTDB_PERM_RELIABLE_WRITE          GATTDB_PERM_RELIABLE_WRITE          /**< To be deprecated, defined for backward compatiablility */
#define LEGATTDB_PERM_AUTH_WRITABLE           GATTDB_PERM_AUTH_WRITABLE           /**< To be deprecated, defined for backward compatiablility */
#define LEGATTDB_PERM_WRITABLE                GATTDB_PERM_WRITABLE                /**< To be deprecated, defined for backward compatiablility */
#define LEGATTDB_PERM_MASK                    GATTDB_PERM_MASK                    /**< To be deprecated, defined for backward compatiablility */
#define LEGATTDB_PERM_SERVICE_UUID_128        GATTDB_PERM_SERVICE_UUID_128        /**< To be deprecated, defined for backward compatiablility */
#define LEGATTDB_CHAR_PROP_BROADCAST          GATTDB_CHAR_PROP_BROADCAST          /**< To be deprecated, defined for backward compatiablility */
#define LEGATTDB_CHAR_PROP_READ               GATTDB_CHAR_PROP_READ               /**< To be deprecated, defined for backward compatiablility */
#define LEGATTDB_CHAR_PROP_WRITE_NO_RESPONSE  GATTDB_CHAR_PROP_WRITE_NO_RESPONSE  /**< To be deprecated, defined for backward compatiablility */
#define LEGATTDB_CHAR_PROP_WRITE              GATTDB_CHAR_PROP_WRITE              /**< To be deprecated, defined for backward compatiablility */
#define LEGATTDB_CHAR_PROP_NOTIFY             GATTDB_CHAR_PROP_NOTIFY             /**< To be deprecated, defined for backward compatiablility */
#define LEGATTDB_CHAR_PROP_INDICATE           GATTDB_CHAR_PROP_INDICATE           /**< To be deprecated, defined for backward compatiablility */
#define LEGATTDB_CHAR_PROP_AUTHD_WRITES       GATTDB_CHAR_PROP_AUTHD_WRITES       /**< To be deprecated, defined for backward compatiablility */
#define LEGATTDB_CHAR_PROP_EXTENDED           GATTDB_CHAR_PROP_EXTENDED           /**< To be deprecated, defined for backward compatiablility */
#define LEGATTDB_UUID16_SIZE                  GATTDB_UUID16_SIZE                  /**< To be deprecated, defined for backward compatiablility */
#define LEGATTDB_UUID128_SIZE                 GATTDB_UUID128_SIZE                 /**< To be deprecated, defined for backward compatiablility */

/**
 * @anchor GATT_DB_MACROS
 * @name Service and Characteristic macros
 * @{
 */

/** Macro to assist 16 bit primary service declaration */
#define PRIMARY_SERVICE_UUID16(handle, service)  \
    BIT16_TO_8((uint16_t)(handle)), \
    GATTDB_PERM_READABLE, \
    4, \
    BIT16_TO_8((GATT_UUID_PRI_SERVICE)), \
    BIT16_TO_8((service))

/** Macro to assist 128 bit primary service declaration */
#define PRIMARY_SERVICE_UUID128(handle, service)  \
    BIT16_TO_8((uint16_t)(handle)), \
    GATTDB_PERM_READABLE, \
    18, \
    BIT16_TO_8(GATT_UUID_PRI_SERVICE), \
    service

/** Macro to assist 16 bit secondary service declaration */
#define SECONDARY_SERVICE_UUID16(handle, service)  \
    BIT16_TO_8((uint16_t)(handle)), \
    GATTDB_PERM_READABLE, \
    4, \
    BIT16_TO_8((GATT_UUID_SEC_SERVICE)), \
    BIT16_TO_8((service))

/** Macro to assist 128 bit secondary service declaration */
#define SECONDARY_SERVICE_UUID128(handle, service)  \
    BIT16_TO_8((uint16_t)(handle)), \
    GATTDB_PERM_READABLE, \
    18, \
    BIT16_TO_8(GATT_UUID_SEC_SERVICE), \
    service

/** Macro to assist included service declaration */
#define INCLUDE_SERVICE_UUID16(handle, service_handle, end_group_handle, service)  \
    BIT16_TO_8((uint16_t)(handle)), \
    GATTDB_PERM_READABLE, \
    8, \
    BIT16_TO_8(GATT_UUID_INCLUDE_SERVICE), \
    BIT16_TO_8(service_handle), \
    BIT16_TO_8(end_group_handle), \
    BIT16_TO_8(service)

/** Macro to assist 128 bit included service declaration */
#define INCLUDE_SERVICE_UUID128(handle, service_handle, end_group_handle)\
    BIT16_TO_8((uint16_t)(handle)), \
    GATTDB_PERM_READABLE, \
    6, \
    BIT16_TO_8(GATT_UUID_INCLUDE_SERVICE), \
    BIT16_TO_8(service_handle), \
    BIT16_TO_8(end_group_handle)

/** Macro to assist readable 16 bit characteristic declaration */
#define CHARACTERISTIC_UUID16(handle, handle_value, uuid, properties, permission) \
    BIT16_TO_8((uint16_t)(handle)), \
    GATTDB_PERM_READABLE, \
    0x07, \
    BIT16_TO_8(GATT_UUID_CHAR_DECLARE), \
    (uint8_t)(properties), \
    BIT16_TO_8((uint16_t)(handle_value)), \
    BIT16_TO_8(uuid), \
    BIT16_TO_8((uint16_t)(handle_value)), \
    (uint8_t)(permission), \
    (uint8_t)(GATTDB_UUID16_SIZE), \
    BIT16_TO_8(uuid)

/** Macro to assist readable 128 bit characteristic declaration */
#define CHARACTERISTIC_UUID128(handle, handle_value, uuid, properties, permission) \
    BIT16_TO_8((uint16_t)(handle)), \
    GATTDB_PERM_READABLE, \
    21, \
    BIT16_TO_8(GATT_UUID_CHAR_DECLARE), \
    (uint8_t)(properties), \
    BIT16_TO_8((uint16_t)(handle_value)), \
    uuid, \
    BIT16_TO_8((uint16_t)(handle_value)), \
    (uint8_t)(permission | GATTDB_PERM_SERVICE_UUID_128), \
    (uint8_t)(GATTDB_UUID128_SIZE), \
    uuid

/** Macro to assist writable 16 bit characteristic declaration */
#define CHARACTERISTIC_UUID16_WRITABLE(handle, handle_value, uuid, properties, permission) \
    BIT16_TO_8((uint16_t)(handle)), \
    GATTDB_PERM_READABLE, \
    0x07, \
    BIT16_TO_8(GATT_UUID_CHAR_DECLARE), \
    (uint8_t)(properties), \
    BIT16_TO_8((uint16_t)(handle_value)), \
    BIT16_TO_8(uuid), \
    BIT16_TO_8((uint16_t)(handle_value)), \
    (uint8_t)(permission), \
    (uint8_t)(GATTDB_UUID16_SIZE), \
    (uint8_t)(0), \
    BIT16_TO_8(uuid)

/** Macro to assist writable 128 bit characteristic declaration */
#define CHARACTERISTIC_UUID128_WRITABLE(handle, handle_value, uuid, properties, permission) \
    BIT16_TO_8((uint16_t)(handle)), \
    GATTDB_PERM_READABLE, \
    21, \
    BIT16_TO_8(GATT_UUID_CHAR_DECLARE), \
    (uint8_t)(properties), \
    BIT16_TO_8((uint16_t)(handle_value)), \
    uuid, \
    BIT16_TO_8((uint16_t)(handle_value)), \
    (uint8_t)(permission | GATTDB_PERM_SERVICE_UUID_128), \
    (uint8_t)(GATTDB_UUID128_SIZE), \
    (uint8_t)(0), \
    uuid

/** Macro to assist writable 16 bit descriptor declaration */
#define CHAR_DESCRIPTOR_UUID16_WRITABLE(handle, uuid, permission) \
    BIT16_TO_8((uint16_t)(handle)), \
    (uint8_t)(permission), \
    (uint8_t)(GATTDB_UUID16_SIZE), \
    (uint8_t)(0), \
    BIT16_TO_8(uuid)

/** Macro to assist readable 16 bit descriptor declaration */
#define CHAR_DESCRIPTOR_UUID16(handle, uuid, permission) \
    BIT16_TO_8((uint16_t)(handle)), \
    (uint8_t)(permission), \
    (uint8_t)(GATTDB_UUID16_SIZE), \
    BIT16_TO_8(uuid)

/** Macro to assist writable 128 bit descriptor declaration */
#define CHAR_DESCRIPTOR_UUID128_WRITABLE(handle, uuid, permission) \
    BIT16_TO_8((uint16_t)(handle)), \
    (uint8_t)(permission | GATTDB_PERM_SERVICE_UUID_128), \
    (uint8_t)(GATTDB_UUID128_SIZE), \
    (uint8_t)(0), \
    uuid

/** Macro to assist readable 128 bit descriptor declaration */
#define CHAR_DESCRIPTOR_UUID128(handle, uuid, permission) \
    BIT16_TO_8((uint16_t)(handle)), \
    (uint8_t)(permission | GATTDB_PERM_SERVICE_UUID_128), \
    (uint8_t)(GATTDB_UUID128_SIZE), \
    uuid

/** Macro to assist extended properties declaration */
#define CHAR_DESCRIPTOR_EXTENDED_PROPERTIES(handle, ext_properties)\
    BIT16_TO_8((uint16_t)(handle)), \
    (uint8_t)(GATTDB_PERM_READABLE), \
    (uint8_t)(GATTDB_UUID16_SIZE + 1), \
    BIT16_TO_8(GATT_UUID_CHAR_EXT_PROP),\
    (uint8_t)(ext_properties)

/** @} GATT_DB_MACROS */

/**
 * list of EATT lcid's
 *
 * LCID: L2CAP Channel ID
 * L2CAP channel ids are assigned by L2CAP during the channel creation process.
 * The list is passed to the application using #wiced_bt_gatt_eatt_connection_indication_event_t
 */
typedef uint16_t wiced_bt_eatt_lcid_list[EATT_CHANNELS_PER_TRANSACTION];
typedef uint16_t wiced_bt_gatt_eatt_conn_id_list[EATT_CHANNELS_PER_TRANSACTION]; /**< list of EATT connection id's */

/**< Data Receive Buffer
 *  Data Receive Buffer or DRB is a memory area of type #tDRB and size(channel MTU size + #DRB_OVERHEAD_SIZE).
 * The DRB buffer is allocated by the application during channel creation.
 * The allocated DRBs are passed to the GATT layer using during EATT connection initiation using #wiced_bt_gatt_eatt_connect or
 * when responding to a connection request using #wiced_bt_gatt_eatt_connection_response_t
 * Application has to allocate one DRB per GATT bearer.
*/
typedef tDRB * wiced_bt_eatt_drbs[EATT_CHANNELS_PER_TRANSACTION];                /**< list of Data Receive Blocks */


/** GATT events */
typedef enum
{
    GATT_CONNECTION_STATUS_EVT,        /**< GATT connection status change. Event data: #wiced_bt_gatt_connection_status_t */
    GATT_OPERATION_CPLT_EVT,           /**< GATT operation complete. Event data: #wiced_bt_gatt_event_data_t */
    GATT_DISCOVERY_RESULT_EVT,         /**< GATT attribute discovery result. Event data: #wiced_bt_gatt_discovery_result_t */
    GATT_DISCOVERY_CPLT_EVT,           /**< GATT attribute discovery complete. Event data: #wiced_bt_gatt_event_data_t */
    GATT_ATTRIBUTE_REQUEST_EVT,        /**< GATT attribute request (from remote client). Event data: #wiced_bt_gatt_attribute_request_t */
    GATT_CONGESTION_EVT,               /**< GATT congestion (running low in tx buffers). Event data: #wiced_bt_gatt_congestion_event_t TODO: add more details regarding congestion */
} wiced_bt_gatt_evt_t;

/**< A note on handling the #GATT_CONGESTION_EVT
* Applications may receive a GATT_CONGESTION_EVT to indicate a congestion at the GATT layer.
* On reception of the #GATT_CONGESTION_EVT with the #wiced_bt_gatt_congestion_event_t.congested variable set to #WICED_TRUE the application should not attempt
*    to send any further requests or commands to the peer indicated in #wiced_bt_gatt_congestion_event_t.congested as these will return with error code of #WICED_BT_GATT_CONGESTED
* On reception of the #GATT_CONGESTION_EVT with the #wiced_bt_gatt_congestion_event_t.congested variable set to #WICED_FALSE the application may resume  sending any further APIs
*/


/** Discovery result (used by GATT_DISCOVERY_RESULT_EVT notification) */
typedef struct
{
    uint16_t                                conn_id;            /**< ID of the connection */
    wiced_bt_gatt_discovery_type_t          discovery_type;     /**< Discovery type (see @link wiced_bt_gatt_discovery_type_e wiced_bt_gatt_discovery_type_t @endlink) */
    wiced_bt_gatt_discovery_data_t          discovery_data;     /**< Discovery data  */
} wiced_bt_gatt_discovery_result_t;

/** Discovery Complete (used by GATT_DISCOVERY_CPLT_EVT notification) */
typedef struct
{
    uint16_t                                conn_id;            /**< ID of the connection */
    wiced_bt_gatt_discovery_type_t          disc_type;          /**< Discovery type (see @link wiced_bt_gatt_discovery_type_e wiced_bt_gatt_discovery_type_t @endlink) */
    wiced_bt_gatt_status_t                  status;             /**< Status of operation */
} wiced_bt_gatt_discovery_complete_t;

/** Response to read/write/disc/config operations (used by GATT_OPERATION_CPLT_EVT notification) */
typedef struct
{
    uint16_t                                conn_id;            /**< ID of the connection */
    wiced_bt_gatt_optype_t                  op;                 /**< Type of operation completed (see @link wiced_bt_gatt_optype_e wiced_bt_gatt_optype_t @endlink) */
    wiced_bt_gatt_status_t                  status;             /**< Status of operation */
    uint8_t                                 pending_events;     /**< Number of pending events, used to initiate next read */
    wiced_bt_gatt_operation_complete_rsp_t  response_data;      /**< Response data (dependent on optype) */
} wiced_bt_gatt_operation_complete_t;

/** GATT connection status (used by GATT_CONNECTION_STATUS_EVT notification) */
typedef struct
{
    uint8_t                         *bd_addr;               /**< Remote device address */
    wiced_bt_ble_address_type_t     addr_type;              /**< Remmote device address type */
    uint16_t                        conn_id;                /**< ID of the connection */
    wiced_bool_t                    connected;              /**< TRUE if connected, FALSE if disconnected */
    wiced_bt_gatt_disconn_reason_t  reason;                 /**< Reason code (see @link wiced_bt_gatt_disconn_reason_e wiced_bt_gatt_disconn_reason_t @endlink) */
    wiced_bt_transport_t            transport;              /**< Transport type of the connection */
    uint8_t                         link_role;              /**< Link role on this connection */
} wiced_bt_gatt_connection_status_t;

/** GATT attribute request (used by GATT_ATTRIBUTE_REQUEST_EVT notification) */
typedef struct
{
    uint16_t                                conn_id;            /**< ID of the connection */
    wiced_bt_gatt_request_type_t            request_type;       /**< Request type (see @link wiced_bt_gatt_request_type_e wiced_bt_gatt_request_type_t @endlink)  */
    wiced_bt_gatt_request_data_t            data;               /**< Information about attribute being request (dependent on request type) */
} wiced_bt_gatt_attribute_request_t;

/** GATT channel congestion/uncongestion (used by GATT_CONGESTION_EVT notification) */
typedef struct
{
    uint16_t                                conn_id;            /**< ID of the connection */
    wiced_bool_t                            congested;          /**< congestion state */
} wiced_bt_gatt_congestion_event_t;

/** GATT caching status (used by GATT_CACHING_STATUS_UPDATE_EVT) */
typedef struct
{
    wiced_bt_device_address_t      bdaddr; /**< Remote device address */
    wiced_bt_gatt_caching_status_t status; /**< GATT caching status #wiced_bt_gatt_caching_status_e */
} wiced_bt_gatt_caching_status_event_t;

/** Stuctures for GATT event notifications */
typedef union
{
    wiced_bt_gatt_discovery_result_t        discovery_result;   /**< Data for GATT_DISCOVERY_RESULT_EVT */
    wiced_bt_gatt_discovery_complete_t      discovery_complete; /**< Data for GATT_DISCOVERY_CPLT_EVT */
    wiced_bt_gatt_operation_complete_t      operation_complete; /**< Data for GATT_OPERATION_CPLT_EVT */
    wiced_bt_gatt_connection_status_t       connection_status;  /**< Data for GATT_CONNECTION_STATUS_EVT */
    wiced_bt_gatt_attribute_request_t       attribute_request;  /**< Data for GATT_ATTRIBUTE_REQUEST_EVT */
    wiced_bt_gatt_congestion_event_t        congestion;         /**< Data for GATT_CONGESTION_EVT */
    wiced_bt_gatt_caching_status_event_t    caching_status;     /**< Data for GATT_CACHING_STATUS_UPDATE_EVT */
} wiced_bt_gatt_event_data_t;

/**
 * GATT event notification callback
 *
 * Callback for GATT event notifications
 * Registered using wiced_bt_gatt_register()
 *
 * @param event             : Event ID
 * @param p_event_data      : Event data
 *
 * @return Status of event handling
*/
typedef wiced_bt_gatt_status_t wiced_bt_gatt_cback_t(wiced_bt_gatt_evt_t event, wiced_bt_gatt_event_data_t *p_event_data);


/** GATT attribute structure for preferred connection parameters*/
typedef struct
{
    uint16_t  int_min;  /**< minimum connection interval */
    uint16_t  int_max;  /**< maximum connection interval */
    uint16_t  latency;  /**< connection latency */
    uint16_t  sp_tout;  /**< connection supervision timeout */
}wiced_bt_gatt_gap_ble_pref_param_t;

/** GATT attribute value included in central role DB*/
typedef union
{
    uint8_t                                addr_resolution;  /**< binary value to indicate if addr. resolution is supported */
}wiced_bt_gatt_gap_ble_attr_value_t;

/**
 * Structure used by wiced_bt_gattdb APIS, to parse GATTDB
 */
typedef struct wiced_gattdb_entry_s
{
    uint16_t handle;        /**< attribute Handle  */
    uint8_t  perm;          /**< attribute permission.*/
    uint8_t  len;           /**< attribute length . It excludes the header.*/
} wiced_gattdb_entry_t;

/** GATT connection request (used by GATT_EATT_CONNECTION_INDICATION_EVT notification)
To be returned by application for accepting/rejecting the connection with the wiced_bt_eatt_connection_response
*/
typedef struct
{
    uint8_t                *bdaddr;                 /**< Bluetooth address of remote device */
    wiced_bt_transport_t    transport;              /**< Transport type of the connection */
    wiced_bt_eatt_lcid_list lcids;                  /**< List of the l2cap cids (channel ids) */
    uint16_t                mtu;                    /**< Peer MTU */
    uint8_t                 trans_id;               /**< Transaction id for the connection */
} wiced_bt_gatt_eatt_connection_indication_event_t;

/**
 * @brief structure to be used to respond to received EATT connection request
 *
 */
typedef struct
{
    wiced_bt_gatt_status_t response;                /**< Application response to be sent for the connection request */
    uint16_t               our_rx_mtu;              /**< Application MTU to be used for the connection */
    uint16_t               our_rx_mps;              /**< Application MPS to be used for the connection */
    wiced_bt_eatt_drbs     ppDRBs;                  /**< Data Receive Buffers of our_rx_mtu size to be used for receiving incoming data */
}wiced_bt_gatt_eatt_connection_response_t;

typedef void (*wiced_bt_gatt_eatt_on_connect_ind_t)(wiced_bt_gatt_eatt_connection_indication_event_t *indication); /**< callback upon GATT EATT connection */
typedef void (*wiced_bt_gatt_eatt_on_connect_complete_t)(uint16_t conn_id, uint16_t result, uint16_t mtu);         /**< callback upon GATT EATT connection complete */
typedef void (*wiced_bt_gatt_eatt_on_reconfigure_ind_t)(uint16_t conn_id, uint16_t mtu, uint16_t mps);             /**< callback upon GATT EATT reconnection complete */
typedef void (*wiced_bt_gatt_eatt_release_drb_t)(tDRB *p_drb);                                                     /**< callback upon releasing the DRB */

/** callbacks for GATT EATT event notifications */
typedef struct
{
    wiced_bt_gatt_eatt_on_connect_ind_t           eatt_connect_ind_cb;      /**< callback upon GATT EATT connection */
    wiced_bt_gatt_eatt_on_connect_complete_t      eatt_connect_cmpl_cb;     /**< callback upon GATT EATT connection complete */
    wiced_bt_gatt_eatt_on_reconfigure_ind_t       eatt_reconfigure_ind_cb;  /**< callback upon GATT EATT reconnection complete */
    wiced_bt_gatt_eatt_release_drb_t              eatt_release_drb;         /**< callback upon releasing the DRB */
} wiced_bt_gatt_eatt_callbacks_t;

/**
 * @}
 */

/*****************************************************************************
 *  External Function Declarations
 ****************************************************************************/
#ifdef __cplusplus
extern "C"
{
#endif

/**
 * Generic Attribute (GATT) Functions.
 * The Generic Attribute Profile (GATT) defines a service framework which enables Bluetooth low
 * energy applications to configure themselves as a client or server device.
 *
 * The profile also provides the capability to perform discovery of services, read, write,
 * notification and indication of characteristics defined on a server.
 *
 * @addtogroup wicedbt_gatt Generic Attribute (GATT)
 * @ingroup wicedbt
 */

/**
 * BLE (Bluetooth Low Energy) Specific functions.
 *
 * @if DUAL_MODE
 * @addtogroup gatt_le BLE (Bluetooth Low Energy)
 * @ingroup gatt_common_api
 * @endif
 */

/**
 *  BR/EDR (Bluetooth Basic Rate / Enhanced Data Rate) Specific functions.
 *
 *  @if DUAL_MODE
 *  @addtogroup gatt_br BR/EDR (Bluetooth Basic Rate / Enhanced Data Rate)
 *  @ingroup gatt_common_api
 *  @endif
 */

/**
 * GATT Profile Server Functions
 *
 *  @addtogroup  gatt_server_api_functions       Server API
 *  @ingroup wicedbt_gatt
 *
 *  <b> Server API Functions </b> sub module for @b GATT.
 */

/**
 *  GATT Server Data API
 *
 *  @addtogroup  gattsr_api_functions GATT Server Data API
 *  @ingroup gatt_server_api_functions
 *
 */

/**
 *
 * This API will send an indication to the client for the specified handle
 *
 *  @param[in]  conn_id     : connection identifier.
 *  @param[in]  attr_handle : Attribute handle of this handle value indication.
 *  @param[in]  val_len     : Length of notification value passed.
 *  @param[in]  p_val       : Notification Value.
 *
 *  @return @link wiced_bt_gatt_status_e wiced_bt_gatt_status_t @endlink
 *  @ingroup gattsr_api_functions
 */
wiced_bt_gatt_status_t wiced_bt_gatt_send_indication (uint16_t conn_id, uint16_t attr_handle, uint16_t val_len, uint8_t *p_val );

/**
 * This API will send a notification to the client for the specified handle
 *
 *  @param[in]  conn_id     : connection identifier.
 *  @param[in]  attr_handle : Attribute handle of this handle value indication.
 *  @param[in]  val_len     : Length of notification value passed.
 *  @param[in]  p_val       : Notification Value.
 *
 *  @return @link wiced_bt_gatt_status_e wiced_bt_gatt_status_t @endlink
 *  @ingroup gattsr_api_functions
 */
wiced_bt_gatt_status_t wiced_bt_gatt_send_notification (uint16_t conn_id, uint16_t attr_handle, uint16_t val_len, uint8_t *p_val );

/**
 * This API will send a multiple variable length notifications to a client.
 *
 *  @param[in]  conn_id             : connection identifier.
 *  @param[in]  num_notifications   : number of notifications
 *  @param[in]  p_notifications     : pointer to array of handle length notification values.
 *
 *  @return @link wiced_bt_gatt_status_e wiced_bt_gatt_status_t @endlink
 *  @ingroup gattsr_api_functions
 */
wiced_bt_gatt_status_t wiced_bt_gatt_send_multiple_notifications (uint16_t conn_id, uint32_t num_notifications, wiced_bt_gatt_handle_length_value_t *p_notifications );

/**
 *  GATT Database Access Functions
 *
 *  @addtogroup  gattdb_api_functions GATT Database
 *  @ingroup gatt_server_api_functions
 *
 */

/**
 * Initialize the GATT database (Please refer @ref GATT_DB_MACROS "Service and Characteristic macros" for MACRO's to create or add entries to GATT database)
 *
 * @param[in]   p_gatt_db       : First element in GATT database array
 * @param[in]   gatt_db_size    : Size (in bytes) of GATT database
 * @param[out]  hash            : The calculated database hash value. The hash pointer passed to this function can be NULL incase the
 * application does not support dynamic databases and does not support database caching
 *
 * @return @link wiced_bt_gatt_status_e wiced_bt_gatt_status_t @endlink
 *  @ingroup gattdb_api_functions
 */
wiced_bt_gatt_status_t wiced_bt_gatt_db_init (const uint8_t *p_gatt_db, uint16_t gatt_db_size, wiced_bt_db_hash_t hash);

/**
 * Add a service module to the database.
 * Service modules define the complete service handles
 * The handle range in the service modules have to be distinct, i.e, cannot overlap with any of the existing service modules
 * in the database
 *
 * @param[in]   p_gatt_db       : First element in GATT database array
 * @param[in]   gatt_db_size    : Size (in bytes) of GATT database
 * @param[out]  hash            : The calculated database hash value. The hash pointer passed to this function can be NULL incase the
 * application does not support dynamic databases and does not support database caching
 *
 * @return @link wiced_bt_gatt_status_e wiced_bt_gatt_status_t @endlink
 *  @ingroup gattdb_api_functions
 */
wiced_bt_gatt_status_t wiced_bt_gatt_add_services_to_db(const uint8_t* p_gatt_db, uint16_t gatt_db_size, wiced_bt_db_hash_t hash);

/**
 * Remove the service module from the database.
 *
 * @param[in]   p_gatt_db       : First element in GATT database array
 * @param[out]  hash            : The calculated database hash value. The hash pointer passed to this function can be NULL incase the
 * application does not support dynamic databases and does not support database caching
 *
 * @return @link wiced_bt_gatt_status_e wiced_bt_gatt_status_t @endlink
 *  @ingroup gattdb_api_functions
 */
wiced_bt_gatt_status_t wiced_bt_gatt_db_remove_services_from_db(const uint8_t* p_gatt_db, wiced_bt_db_hash_t hash);

/**
 * Find the next GATT attribute entry in the local GATT database
 * To find the first attribute entry pass the address of the local GATT database as a parameter
 *
 *  @param[in]   p_db_entry     : GATT DB attribute entry
 *
 *  @return pointer to next attribute entry
 *  @ingroup gattdb_api_functions
 */
wiced_gattdb_entry_t * wiced_bt_gattdb_next_entry (wiced_gattdb_entry_t *p_db_entry);

/**
 * Utility to get attribute handle from GATT DB entry
 * To get next attribute handle, application needs traverse by wiced_bt_gattdb_next_entry
 *
 *  @param[in]   p_db_entry     : GATT DB attribute entry
 *
 *  @return attribute handle
 *  @ingroup gattdb_api_functions
 */
uint16_t wiced_bt_gattdb_get_handle (wiced_gattdb_entry_t *p_db_entry);

/**
 * Utility to get the attribute UUID for the given database entry.
 *
 * @param[in]   p_db_entry     : GATT DB attribute entry
 * @param[out]  p_uuid         : pointer to UUID holder. Application should pass the required size of UUID.
 * It is recommended to pass uint8_t[16], so that we can avoid overflow in case of 128 bit uuid
 *
 *  @returns size of the UUID
 *  @ingroup gattdb_api_functions
 */
uint32_t wiced_bt_gattdb_get_attribute_uuid (wiced_gattdb_entry_t *p_db_entry, uint8_t *p_uuid);

/**
 * Utility to get service UUID from GATTDB Primary/Secondary services
 *
 *  @param[in]     p_db_entry     : GATT DB attribute entry
 *
 *  @return 16 bit uuid
 *  @ingroup gattdb_api_functions
 */
uint16_t wiced_bt_gattdb_get_attribute_value_uuid16 (wiced_gattdb_entry_t *p_db_entry);

/**
 * Utility to get characteristic descriptor handle value from GATTDB.
 * If characteristic does not have a characteristic descriptor with specified UUID, function will return zero
 *
 *  @param[in]     char_handle         : GATT DB characteristic handle
 *  @param[in]     descriptor_uuid     : Characteristic descriptor UUID
 *
 *  @returns characteristic descriptor handle
 *  @ingroup gattdb_api_functions
 */
uint16_t wiced_bt_gattdb_get_charcteristic_descriptor_handle (uint16_t char_handle, uint16_t descriptor_uuid);

/**
 * @brief  Utility to get characteristic handle based on descriptor handle from GATTDB.
 *         If characteristic does not have a characteristic descriptor with specified UUID, function will return zero
 *
 * @param desc_handle descriptor handle
 * @return characteristic handle
 */
uint16_t wiced_bt_gattdb_get_charcteristic_handle (uint16_t desc_handle);

/**
 * @brief Utility to get UUID based on handle from GATTDB.
 *
 * @param handle handle
 * @param uuid pointer to buffer that can hold 16 bytes max (to support both 2 & 16 byte UUID).
 * @return uint8_t UUID Size
 */
uint8_t wiced_bt_gattdb_get_UUID (uint16_t handle, uint8_t *uuid);

/**
* When application receives a Read Request, Write Request or Indication from the
* peer it can reply synchronously or return a WICED_BT_GATT_PENDING result code
* indicating to the stack that the message is not processed yet.  In that case
* application should call this function to send data or just a confirmation to
* the peer.
*
*  @param[in]  status      : Status of the operation to be send to the peer
*  @param[in]  conn_id     : Connection handle
*  @param[in]  attr_handle : Attribute handle
*  @param[in]  attr_len    : Length of the attribute to send
*  @param[in]  offset      : Attribute value offset
*  @param[in]  p_attr      : Attribute Value
*
*  @return @link wiced_bt_gatt_status_e wiced_bt_gatt_status_t @endlink
*/
wiced_bt_gatt_status_t wiced_bt_gatt_send_response(wiced_bt_gatt_status_t status, uint16_t conn_id,
    uint16_t attr_handle, uint16_t attr_len, uint16_t offset, uint8_t* p_attr);

/**
 *  GATT Robust Caching  API
 *
 *  @addtogroup  gatt_robust_caching_api_functions GATT Robust Caching
 *  @ingroup gatt_server_api_functions
 *
 */

/**
 *
 * Indicate to stack if peer supports robust caching
 * Set by the application by querying the remote peers Client Supported Features in case the server supports gatt caching.
 *
 * @param[in]   conn_id : GATT Connection ID
 * @param[in]   robust_caching_supported : TRUE if peer supports robust caching
 *
 * @return None
 * @ingroup gatt_robust_caching_api_functions
 */
void wiced_bt_gatt_update_peer_robust_caching_support_status(uint16_t conn_id, uint32_t robust_caching_supported);

/**
 * Function     wiced_bt_gatt_get_peer_caching_status
 * Can be used by the server application to read the robust caching status of the peer.
 *
 * @param[in]       conn_id: connection identifier.
 * @param[out]      caching_status: GATT_PEER_CLIENT_CACHE_CHANGE_AWARE/GATT_PEER_CLIENT_CACHE_CHANGE_UNAWARE
 *
 * @return          None
 * @ingroup gatt_robust_caching_api_functions
 */
void wiced_bt_gatt_get_peer_caching_status(uint16_t conn_id, wiced_bt_gatt_caching_status_t* caching_status);

/**
 *
 * This function is used by the application to set peer's caching state.
 * The application is expected to store the last #wiced_bt_db_hash_t shared with the bonded peer in NVRAM
 * On connection, the application compares the stored #wiced_bt_db_hash_t with the current db_hash.
 * If the hashes match, the status is set to #GATT_PEER_CLIENT_CACHE_CHANGE_AWARE else #GATT_PEER_CLIENT_CACHE_CHANGE_UNAWARE
 *
 * @param[in]       conn_id: connection identifier.
 * @param[in]       status: CHANGE_AWARE/CHANGE_UNAWARE.
 *
 * @return          None
 * @ingroup gatt_robust_caching_api_functions
 */
void wiced_bt_gatt_set_peer_caching_status(uint16_t conn_id, wiced_bt_gatt_caching_status_t status);

/**
 *  GATT Profile Client Functions
 *
 *  @addtogroup  gatt_client_api_functions   Client API
 *  @ingroup wicedbt_gatt
 */

/**
 * Configure the ATT MTU size for a connection on an LE
 * transport. Allowed mtu range is 23 to max_mtu_size configured in #wiced_bt_cfg_settings_t
 *
 *  @param[in]  conn_id             : GATT connection handle
 *  @param[in]  mtu                 : New MTU size
 *
 *  @return @link wiced_bt_gatt_status_e wiced_bt_gatt_status_t @endlink
 *  @ingroup gatt_client_api_functions
*/
wiced_bt_gatt_status_t wiced_bt_gatt_configure_mtu (uint16_t conn_id, uint16_t mtu);

/**
 * Start an attribute discovery on an ATT server.
 * Discovery results are notified using <b> GATT_DISCOVERY_RESULT_EVT </b>;
 * completion is notified using <b> GATT_DISCOVERY_CPLT_EVT </b> of #wiced_bt_gatt_cback_t.
 *
 *  @param[in]  conn_id             : GATT connection handle
 *  @param[in]  discovery_type      : Discover type
 *  @param[in]  p_discovery_param   : Discover parameter
 *
 *  @return @link wiced_bt_gatt_status_e wiced_bt_gatt_status_t @endlink
 *  @ingroup gatt_client_api_functions
 */
wiced_bt_gatt_status_t wiced_bt_gatt_send_discover (uint16_t conn_id,
                                             wiced_bt_gatt_discovery_type_t discovery_type,
                                             wiced_bt_gatt_discovery_param_t *p_discovery_param );

/**
 * Read from remote ATT server.
 * Result is notified using <b> GATT_OPERATION_CPLT_EVT </b> of #wiced_bt_gatt_cback_t.
 *
 *  @param[in]  conn_id     : Connection handle
 *  @param[in]  type        : Type of the read
 *  @param[in]  p_read      : Pointer to the read request parameters
 *
 *  @return @link wiced_bt_gatt_status_e wiced_bt_gatt_status_t @endlink
 *  @ingroup gatt_client_api_functions
 *
 */
wiced_bt_gatt_status_t wiced_bt_gatt_send_read (uint16_t conn_id, wiced_bt_gatt_read_type_t type,
                                             wiced_bt_gatt_read_param_t *p_read);

/**
 * Write to remote ATT server.
 * Result is notified using <b> GATT_OPERATION_CPLT_EVT </b> of #wiced_bt_gatt_cback_t.
 *
 *  @param[in]  conn_id     : Connection handle
 *  @param[in]  type        : Type of write
 *  @param[in]  p_write     : Pointer to the write parameters
 *
 *  @return @link wiced_bt_gatt_status_e wiced_bt_gatt_status_t @endlink
 *  @ingroup gatt_client_api_functions
 */
wiced_bt_gatt_status_t wiced_bt_gatt_send_write (uint16_t conn_id, wiced_bt_gatt_write_type_t type,
                                              wiced_bt_gatt_value_t *p_write);

/**
 * Send Execute Write request to remote ATT server.
 *
 *  @param[in]  conn_id             : Connection handle
 *  @param[in]  is_execute          : <b>WICED_BT_TRUE </b> to execute, <b> WICED_BT_FALSE </b> to cancel
 *
 *  @return @link wiced_bt_gatt_status_e wiced_bt_gatt_status_t @endlink
 *  @ingroup gatt_client_api_functions
 *
 */
wiced_bt_gatt_status_t wiced_bt_gatt_send_execute_write (uint16_t conn_id, wiced_bool_t is_execute);

/**
 * Send a handle value confirmation to remote ATT server.
 * in response to <b>GATTC_OPTYPE_INDICATION </b>  of #wiced_bt_gatt_cback_t
 *
 *  @param[in]  conn_id             : Connection handle
 *  @param[in]  handle              : Attribute handle
 *
 *  @return @link wiced_bt_gatt_status_e wiced_bt_gatt_status_t @endlink
 *  @ingroup gatt_client_api_functions
*/
wiced_bt_gatt_status_t wiced_bt_gatt_send_indication_confirm (uint16_t conn_id, uint16_t handle);

/**
 *  GATT Profile Connection Functions
 *
 *  @addtogroup gatt_common_api Connection API
 *  @ingroup wicedbt_gatt
 *
 */

/**
 * Open GATT over LE connection to a remote device
 * Result is notified using <b> GATT_CONNECTION_STATUS_EVT </b> of #wiced_bt_gatt_cback_t.
 *
 *  @param[in]  bd_addr     : Remote device address
 *  @param[in]  bd_addr_type: Public or random address
 *  @param[in]  conn_mode   : connection scan mode
 *  @param[in]  is_direct   : Is direct connection or not
 *
 *  @return <b> TRUE </b>            : If connection started
 *          <b> FALSE </b>           : If connection start failure
 *
 * NOTE :  If is_direct = WICED_FALSE, it will create background connection.
 *         Default Background connection type is BTM_BLE_CONN_NONE.
 *         Before calling wiced_bt_gatt_le_connect please set background connection type (AUTO / SELECTIVE)
 *         using wiced_bt_ble_set_background_connection_type API
 *
 * @if DUAL_MODE
 * @ingroup gatt_le
 * @else
 * @ingroup gatt_common_api
 * @endif
 */
wiced_bool_t wiced_bt_gatt_le_connect (wiced_bt_device_address_t bd_addr,
                                    wiced_bt_ble_address_type_t bd_addr_type,
                                    wiced_bt_ble_conn_mode_t conn_mode,
                                    wiced_bool_t is_direct);
/**
 * Open GATT over BR/EDR connection to a remote device
 * Result is notified using <b> GATT_CONNECTION_STATUS_EVT </b> of #wiced_bt_gatt_cback_t.
 *
 *  @param[in]  bd_addr     : Remote device address
 *
 *  @return <b> TRUE </b>            : If connection started
 *          <b> FALSE </b>           : If connection start failure
 *
 *  @ingroup gatt_br
 */
wiced_bool_t wiced_bt_gatt_bredr_connect (wiced_bt_device_address_t bd_addr);

/**
 * Register an application callback for GATT.
 *
 *  @param[in]    p_gatt_cback      : The GATT notification callback
 *
 *  @return @link wiced_bt_gatt_status_e wiced_bt_gatt_status_t @endlink
 *
 * @ingroup gatt_common_api
 */
wiced_bt_gatt_status_t wiced_bt_gatt_register (wiced_bt_gatt_cback_t *p_gatt_cback);

/**
 * Cancel initiating GATT connection
 *
 *  @param[in]  bd_addr     : Remote device address
 *  @param[in]  is_direct   : Is direct connection or not
 *
 *  @return <b> TRUE </b>            : If connection started
 *          <b> FALSE </b>           : If connection start failure
 *
 * @ingroup gatt_common_api
 */
wiced_bool_t wiced_bt_gatt_cancel_connect (wiced_bt_device_address_t bd_addr, wiced_bool_t is_direct);

/**
 * Close the specified GATT connection.
 * Result is notified using <b> GATT_CONNECTION_STATUS_EVT </b> of #wiced_bt_gatt_cback_t.
 *
 *  @param[in]   conn_id     : GATT connection ID
 *
 *  @return @link wiced_bt_gatt_status_e wiced_bt_gatt_status_t @endlink
 *
 * @ingroup gatt_common_api
 */
wiced_bt_gatt_status_t wiced_bt_gatt_disconnect (uint16_t conn_id);


/**
 * Start or stop LE advertisement and listen for connection.
 *
 *  @param[in]   start      : TRUE to add device to whitelist / FALSE to remove
 *  @param[in]   bd_addr    : Device to add/remove from whitelist
 *
 *  @return <b> TRUE </b>            : Success
 *          <b> FALSE </b>           : Failure
 *
 * @if DUAL_MODE
 * @ingroup gatt_le
 * @else
 * @ingroup gatt_common_api
 * @endif
 */
wiced_bool_t wiced_bt_gatt_listen (wiced_bool_t start, wiced_bt_device_address_t bd_addr);

/**
 * EATT API
 *
 *  @addtogroup  gatt_eatt_functions EATT
 *  @ingroup gatt_common_api
 */

/**
 * Register an application callback for GATT.
 *
 *  @param[in]    p_gatt_cback      : The GATT notification callback
 *  @param[in]    max_gatt_bearers  : Total number of gatt bearers allowed across all devices (legacy + eatt)
 *
 *  @return Number of (legacy + eatt) bearers allocated
 *
 *  @ingroup gatt_eatt_functions
 */
uint32_t wiced_bt_gatt_eatt_register(wiced_bt_gatt_eatt_callbacks_t *p_gatt_cback, uint32_t max_gatt_bearers);

/**
 * API to create the enhanced gatt channels, using the legacy/unenhanced connection id.
 *
 *  @param[in]     legacy_conn_id : GATT DB characteristic handle
 *  @param[in]     mtu            : EATT bearer Maximum Transmission Unit
 *  @param[in]     mps            : EATT bearer Maximum PDU Payload Size
 *  @param[in]     num_bearers    : Number of EATT bearers to be established in this call
 *  @param[in]     ppDRBs         : Data Receive Buffers, each of MTU size for each of the bearers to be established
 *  @param[out]    conn_id_list_out : Connection Ids created for the EATT bearers
 *
 *  @returns #wiced_result_t
 *
 *  @ingroup gatt_eatt_functions
 */
wiced_result_t wiced_bt_gatt_eatt_connect(uint16_t legacy_conn_id, uint32_t mtu, uint32_t mps, uint32_t num_bearers,
    wiced_bt_eatt_drbs ppDRBs, wiced_bt_gatt_eatt_conn_id_list conn_id_list_out);

/**
 * API to create the respond to the enhanced gatt channels connection indication
 *
 *  @param[in]     p_indication   : #wiced_bt_gatt_eatt_connection_indication_event_t received in #wiced_bt_gatt_eatt_callbacks_t
 *  @param[in]     p_response     : #wiced_bt_gatt_eatt_connection_response_t response data sent by the receiver of #wiced_bt_gatt_eatt_callbacks_t
 *  @param[out]    conn_id_list_out : Connection Ids created for the EATT bearers
 *
 *  @returns characteristic descriptor handle
 *
 *  @ingroup gatt_eatt_functions
 */
wiced_result_t wiced_bt_gatt_eatt_connect_response(wiced_bt_gatt_eatt_connection_indication_event_t * p_indication,
    wiced_bt_gatt_eatt_connection_response_t * p_response,
    wiced_bt_gatt_eatt_conn_id_list conn_id_list_out);

/**
 * API to create the reconfigurethe enhanced gatt channels
 *
 *  @param[in]     conn_id_list : #wiced_bt_gatt_eatt_conn_id_list to be reconfigured
 *  @param[in]     num_bearers  : Number of bearers in the conn_ids list
 *  @param[in]     mtu          : New MTU value
 *  @param[in]     mps          : New MPS value
 *  @param[in]     ppDRBs       : New pointers to the DRBs, each of size MTU for each of bearer in the conn_id_list
 *
 *  @returns #wiced_result_t
 *
 *  @ingroup gatt_eatt_functions
 */
wiced_result_t wiced_bt_gatt_eatt_reconfigure(wiced_bt_gatt_eatt_conn_id_list conn_id_list, uint32_t num_bearers, uint32_t mtu, uint32_t mps, wiced_bt_eatt_drbs ppDRBs);

/**
 * API to get the bluetooth device address of the conn_id
 *
 *  @param[in]     conn_id : Connection handle of the gatt bearer
 *  @param[in]     p_bdaddr : #wiced_bt_device_address_t
 *
 *  @returns #wiced_bt_gatt_status_t
 *
 *  @ingroup gatt_common_api
 */
wiced_bt_gatt_status_t wiced_bt_gatt_get_device_address(uint16_t conn_id, wiced_bt_device_address_t* p_bdaddr);

#ifdef __cplusplus
}
#endif

