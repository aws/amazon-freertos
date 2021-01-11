/*
 * $ Copyright Cypress Semiconductor $
 */

/** @file
 *
 * Bluetooth AVRCP Application Programming Interface
 *
 */
#pragma once

#include "wiced_bt_sdp.h"
#include "wiced_bt_avrc_defs.h"



/**
 * @cond DUAL_MODE
 * @defgroup  wicedbt_avrc        Audio/Video Remote Control (AVRC)
 *
 * This section describes the API's to use Audio/Video Remote Control Profile commands
 * which use underlying AVCT protocol.
 *
 * @addtogroup  wicedbt_avrc    Audio/Video Remote Control (AVRC)
 *
 * @ingroup     wicedbt
 *
 * @{
*/

/*****************************************************************************
**  constants
*****************************************************************************/

/**
 * @anchor AVRC_RESULT
 * @name AVRC result codes.
 * @{
 */
#define AVRC_SUCCESS        0                   /**< Function successful */
#define AVRC_NO_RESOURCES   1                   /**< Not enough resources */
#define AVRC_BAD_HANDLE     2                   /**< Bad handle */
#define AVRC_PID_IN_USE     3                   /**< PID already in use */
#define AVRC_NOT_OPEN       4                   /**< Connection not open */
#define AVRC_MSG_TOO_BIG    5                   /**< the message length exceed the MTU of the browsing channel */
#define AVRC_FAIL           0x10                /**< generic failure */
#define AVRC_BAD_PARAM      0x11                /**< bad parameter   */
#define AVRC_BUSY           0x12                /**< busy with another operation */
/** @} AVRC_RESULT */

/**
 * @anchor AVRC_CT_ROLE
 * @name AVRC Control Role
 * @{ */
#define AVRC_CT_TARGET      1                   /**< AVRC target */
#define AVRC_CT_CONTROL     2                   /**< AVRC controller */
#define AVRC_CT_PASSIVE     4                   /**< AVRC role determined by peer device */
/** @} AVRC_CT_ROLE */

/**
 * @anchor AVRC_CONN_ROLE
 * @name AVRC Connection Role
 * @{ */
#define AVRC_CONN_INITIATOR 0                   /**< AVRC initiator */
#define AVRC_CONN_ACCEPTOR  1                   /**< AVRC acceptor  */
/** @} AVRC_CONN_ROLE */

/**
 * @anchor AVRC_EVT
 * @name AVRC CTRL events
 * @{
 */
/** AVRC_OPEN_IND_EVT event is sent when the connection is successfully opened.
 * This eventis sent in response to an wiced_bt_avrc_open(). */
#define AVRC_OPEN_IND_EVT       0

/** AVRC_CLOSE_IND_EVT event is sent when a connection is closed.
 * This event can result from a call to wiced_bt_avrc_close() or when the peer closes
 * the connection.  It is also sent when a connection attempted through
 * wiced_bt_avrc_open() fails. */
#define AVRC_CLOSE_IND_EVT      1

/** AVRC_CONG_IND_EVT event indicates that AVCTP is congested and cannot send
 * any more messages. */
#define AVRC_CONG_IND_EVT       2

/** AVRC_UNCONG_IND_EVT event indicates that AVCTP is uncongested and ready to
 * send messages. */
#define AVRC_UNCONG_IND_EVT     3

/** AVRC_BROWSE_OPEN_IND_EVT event is sent when the browse channel is successfully opened.
 * This eventis sent in response to an wiced_bt_avrc_open() or wiced_bt_avrc_open_browse() . */
#define AVRC_BROWSE_OPEN_IND_EVT       4

/** AVRC_BROWSE_CLOSE_IND_EVT event is sent when a browse channel is closed.
 * This event can result from a call to wiced_bt_avrc_close(), wiced_bt_avrc_close_browse() or when the peer closes
 * the connection.  It is also sent when a connection attempted through
 * wiced_bt_avrc_openBrowse() fails. */
#define AVRC_BROWSE_CLOSE_IND_EVT      5

/** AVRC_BROWSE_CONG_IND_EVT event indicates that AVCTP browse channel is congested and cannot send
 * any more messages. */
#define AVRC_BROWSE_CONG_IND_EVT       6

/** AVRC_BROWSE_UNCONG_IND_EVT event indicates that AVCTP browse channel is uncongested and ready to
 * send messages. */
#define AVRC_BROWSE_UNCONG_IND_EVT     7

/** AVRC_CMD_TIMEOUT_EVT event indicates timeout waiting for AVRC command response from the peer */
#define AVRC_CMD_TIMEOUT_EVT           8

/** @} AVRC_EVT */

/** Supported categories */
#define AVRC_SUPF_CT_CAT1               0x0001      /**< Category 1 */
#define AVRC_SUPF_CT_CAT2               0x0002      /**< Category 2 */
#define AVRC_SUPF_CT_CAT3               0x0004      /**< Category 3 */
#define AVRC_SUPF_CT_CAT4               0x0008      /**< Category 4 */
#define AVRC_SUPF_CT_BROWSE             0x0040      /**< Browsing */

#define AVRC_SUPF_TG_CAT1               0x0001      /**< Category 1 */
#define AVRC_SUPF_TG_CAT2               0x0002      /**< Category 2 */
#define AVRC_SUPF_TG_CAT3               0x0004      /**< Category 3 */
#define AVRC_SUPF_TG_CAT4               0x0008      /**< Category 4 */
#define AVRC_SUPF_TG_APP_SETTINGS       0x0010      /**< Player Application Settings */
#define AVRC_SUPF_TG_GROUP_NAVI         0x0020      /**< Group Navigation */
#define AVRC_SUPF_TG_BROWSE             0x0040      /**< Browsing */
#define AVRC_SUPF_TG_MULTI_PLAYER       0x0080      /**< Muliple Media Player */

#define AVRC_META_SUCCESS               AVRC_SUCCESS    /**< AVRC success */
 #define AVRC_META_FAIL                  AVRC_FAIL      /**< AVRC fail */
#define AVRC_METADATA_CMD               0x0000          /**< AVRC metadata command */
#define AVRC_METADATA_RESP              0x0001          /**< AVRC metadata response */


/*****************************************************************************
**  data type definitions
*****************************************************************************/

/**
 * AVRC control callback function.
 *
 * @param[in]       handle      : Connection handle
 * @param[in]       event       : AVRC event (see @ref AVRC_EVT "AVRC events")
 * @param[in]       result      : Result code (see @ref AVRC_RESULT "AVRC result codes")
 * @param[in]       peer_addr   : Peer device address
 *
 * @return          Nothing
 */
typedef void (wiced_bt_avrc_ctrl_cback_t) (uint8_t handle, uint8_t event, uint16_t result,
             wiced_bt_device_address_t peer_addr);


/**
 * AVRC message callback function.  It is executed when AVCTP has
 * a message packet ready for the application.  The implementation of this
 * callback function must copy the wiced_bt_avrc_msg_t structure passed to it as it
 * is not guaranteed to remain after the callback function exits.
 *
 * @param[in]       handle  : Connection handle
 * @param[in]       label   : Message label
 * @param[in]       opcode  : Message opcode (see @ref AVRC_OPCODES "AVRC opcodes")
 * @param[in]       p_msg   : AVRC message
 *
 * @return          Nothing
 */
typedef void (wiced_bt_avrc_msg_cback_t) (uint8_t handle, uint8_t label, uint8_t opcode,
             wiced_bt_avrc_msg_t *p_msg);

/**
* AVRC message transmitted callback function.  It is executed when AVCTP has
* sent a message packet with application payload.
*
* @param[in]       handle  : Connection handle
* @param[in]       label   : Message label
* @param[in]       opcode  : Message opcode (see @ref AVRC_OPCODES "AVRC opcodes")
* @param[in]       p_msg   : AVRC message
*
* @return          Nothing
*/
typedef void (wiced_bt_avrc_xmitted_cback_t) (uint8_t handle, wiced_bt_avrc_xmit_buf_t *p_buf, wiced_bool_t b_was_sent_ok);

/** AVRC connection control block; used when calling wiced_bt_avrc_open() to configure the AVRC connection and register for callbacks. */
typedef struct
{
    wiced_bt_avrc_ctrl_cback_t      *p_ctrl_cback;      /**< AVRC connection control callback */
    wiced_bt_avrc_msg_cback_t       *p_msg_cback;       /**< AVRC message callback */
    wiced_bt_avrc_xmitted_cback_t   *p_xmitted_cback;   /**< pointer to application transmit complete callback */
    uint32_t                        company_id;         /**< Company ID  (see @ref AVRC_COMPANY_ID "Company IDs") */
    uint8_t                         connection_role;    /**< Connection role: AVRC_CONN_INT (initiator) or AVRC_CONN_ACP (acceptor) (see @ref AVRC_CONN_ROLE "AVRC connection roles") */
    uint8_t                         control;            /**< Control role: AVRC_CT_TARGET (target) or AVRC_CT_CONTROL (controller) (see @ref AVRC_CT_ROLE "AVRC control roles")*/
    uint16_t                        avrc_seg_buf_len;   /**< Maximum length of assembled AVRC data */
    uint8_t                         *p_avrc_buff;       /**< Pointer to AVRC assembly buffer */
    uint16_t                        avct_seg_buf_len;   /**< Maximum length of AVCT assembled data */
    uint8_t                         *p_avct_buff;       /**< Pointer to AVCT assembly buffer */
} wiced_bt_avrc_conn_cb_t;







#ifdef __cplusplus
extern "C"
{
#endif

/**
* This function initializes AVRCP and prepares the protocol stack for its use.
* This function must be called once by the system or platform using AVRCP
* before the other functions of the API an be used
*
*  @param[in]      mtu       : Control Channel MTU (Min Value = 48, default value = L2CAP MTU)
*  @param[in]      mtu_br    : Browsing Channel MTU (Min Value = 335, default value = L2CAP MTU)
*  @param[in]      sec_mask  : Security requirement mask
*
*  @return          None
*/
void wiced_bt_avrc_register(uint16_t mtu, uint16_t mtu_br, uint8_t sec_mask);

/**
 * Open AVRC connection (as intiator or acceptor); register notification callbacks.
 * The connection role may be AVRC controller or target.
 * The connection remains available to the application until
 * wiced_bt_avrc_close() is called.
 * On receiving AVRC_CLOSE_IND_EVT, acceptor connections remain in
 * acceptor mode (no need to re-open the connection)
 *
 * @param[out]      p_handle    : Connection handle (valid if AVRC_SUCCESS is returned)
 * @param[in]       p_ccb       : AVRC connection control block (callbacks and role configuration)
 * @param[in]       peer_addr   : Peer device address (if initiator)
 *
 * @return          Result code (see @ref AVRC_RESULT "AVRC result codes")
 */
uint16_t wiced_bt_avrc_open(uint8_t *p_handle, wiced_bt_avrc_conn_cb_t *p_ccb,
                            wiced_bt_device_address_t peer_addr);

/**
 * Close AVRCP connection
 *
 * @param[in]       handle      : Handle of connection to close
 *
 * @return          Result code (see @ref AVRC_RESULT "AVRC result codes")
 *
 */
uint16_t wiced_bt_avrc_close(uint8_t handle);

/**
 * Open AVRCP browsing connection, either as initiator or acceptor.
 *
 * @param[in]       handle      : Connection handle (obtained from wiced_bt_avrc_open)
 * @param[in]       conn_role   : Initiator or acceptor of the connection
 *                                (see @ref AVRC_CONN_ROLE "AVRC connection roles")
 *
 * @return          Result code (see @ref AVRC_RESULT "AVRC result codes")
 *
 */
uint16_t wiced_bt_avrc_open_browse(uint8_t handle, uint8_t conn_role);

/**
 * Close AVRCP browsing connection
 *
 * @param[in]       handle      : Connection handle
 *
 * @return          Result code (see @ref AVRC_RESULT "AVRC result codes")
 *
 */
uint16_t wiced_bt_avrc_close_browse(uint8_t handle);

/**
 * Send an AVRC message
 *
 * @param[in]       handle      : Connection handle
 * @param[in]       label       : Transaction label
 * @param[in]       ctype       : Message type (see @ref AVRC_CTYPE "AVRC message types")
 * @param[in]       p_cmdbuf       : Pointer to the buffer holding the AVRC message
 *
 * @return          Result code (see @ref AVRC_RESULT "AVRC result codes")
 *
 */
uint16_t wiced_bt_avrc_msg_req (uint8_t handle, uint8_t label, uint8_t ctype, wiced_bt_avrc_xmit_buf_t *p_cmdbuf);

/**
 * Send a UNIT INFO command to the peer device. This
 * function can only be called for controller role connections.
 * Any response message from the peer is passed back through
 * the wiced_bt_avrc_msg_cback_t callback function.
 *
 * @param[in]       handle      : Connection handle
 * @param[in]       label       : Transaction label
 *
 * @return          Result code (see @ref AVRC_RESULT "AVRC result codes")
 *
 */
uint16_t wiced_bt_avrc_unit_cmd(uint8_t handle, uint8_t label);

/**
 * Send a SUBUNIT INFO command to the peer device.  This
 * function can only be called for controller role connections.
 * Any response message from the peer is passed back through
 * the wiced_bt_avrc_msg_cback_t callback function.
 *
 *
 * @param[in]       handle      : Connection handle
 * @param[in]       label       : Transaction label
 * @param[in]       page        : Specifies which subunit table is requested.
 *                                For AVRCP it is typically zero. Value range is 0-7.
 *
 * @return          Result code (see @ref AVRC_RESULT "AVRC result codes")
 *
 */
uint16_t wiced_bt_avrc_sub_cmd(uint8_t handle, uint8_t label, uint8_t page);


/**
 * Send a PASS THROUGH command to the peer device.  This
 * function can only be called for controller role connections.
 * Any response message from the peer is passed back through
 * the wiced_bt_avrc_msg_cback_t callback function.
 *
 * @param[in]       handle      : Connection handle
 * @param[in]       label       : Transaction label
 * @param[in]       p_msg       : Pointer to the pass through command
 *
 * @return          Result code (see @ref AVRC_RESULT "AVRC result codes")
 *
 */
uint16_t wiced_bt_avrc_pass_cmd(uint8_t handle, uint8_t label, wiced_bt_avrc_msg_pass_t *p_msg);

/**
 * Send a PASS THROUGH response to the peer device.  This
 * function can only be called for target role connections.
 * This function must be called when a PASS THROUGH command
 *                  message is received from the peer through the
 * wiced_bt_avrc_msg_cback_t callback function.
 *
 * @param[in]       handle      : Connection handle
 * @param[in]       label       : Transaction label
 * @param[in]       p_msg       : Pointer to the pass through response
 *
 * @return          Result code (see @ref AVRC_RESULT "AVRC result codes")
 *
 */
uint16_t wiced_bt_avrc_pass_rsp(uint8_t handle, uint8_t label, wiced_bt_avrc_msg_pass_t *p_msg);


/**
 * Send a VENDOR DEPENDENT command to the peer device.  This
 * function can only be called for controller role connections.
 * Any response message from the peer is passed back through
 * the wiced_bt_avrc_msg_cback_t callback function.
 *
 * @param[in]       handle      : Connection handle
 * @param[in]       label       : Transaction label
 * @param[in]       p_msg       : Pointer to the vendor dependent command
 *
 * @return          Result code (see @ref AVRC_RESULT "AVRC result codes")
 *
 */
uint16_t wiced_bt_avrc_vendor_cmd(uint8_t handle, uint8_t  label, wiced_bt_avrc_msg_vendor_t *p_msg);


/**
 * Send a VENDOR DEPENDENT response to the peer device.  This
 * function can only be called for target role connections.
 * This function must be called when a VENDOR DEPENDENT
 * command message is received from the peer through the
 * wiced_bt_avrc_msg_cback_t callback function.
 *
 * @param[in]       handle      : Connection handle
 * @param[in]       label       : Transaction label
 * @param[in]       p_msg       : Pointer to the vendor dependent response
 *
 * @return          Result code (see @ref AVRC_RESULT "AVRC result codes")
 *
 */
uint16_t wiced_bt_avrc_vendor_rsp(uint8_t handle, uint8_t  label, wiced_bt_avrc_msg_vendor_t *p_msg);


/**
 * Sets the trace level for AVRC. If 0xff is passed, the
 * current trace level is returned.
 *
 * @param[in]       new_level   : New trace level
 *
 * @return          The new trace level or current trace level if
 *                  the input parameter is 0xff.
 *
 */
uint8_t wiced_bt_avrc_set_trace_level (uint8_t new_level);

/**
 * Parse incoming AVRCP command message.
 *
 * @param[out]      p_result    : Pointer to the parsed command
 * @param[in]       p_msg       : Pointer to the message to parse
 * @param[in]       p_buf       : Pointer to the buffer for parsing avrc messages
 * @param[in]       buf_len     : Size of the buffer
 *
 * @return          Status code (see @ref AVRC_STS "AVRC status codes")
 *                  AVRC_STS_NO_ERROR, if the message in p_data is parsed successfully.
 *                  Otherwise, the error code defined by AVRCP 1.4
 *
 */
wiced_bt_avrc_sts_t wiced_bt_avrc_parse_command (wiced_bt_avrc_msg_t *p_msg, wiced_bt_avrc_command_t *p_result, uint8_t *p_buf, uint16_t buf_len);

/**
 * Parse incoming AVRCP response message.
 *
 * @param[out]      p_result    : Pointer to the parsed response
 * @param[in]       p_msg       : Pointer to the message to parse
 * @param[in]       p_buf       : Pointer to the buffer for parsing avrc messages
 * @param[in]       buf_len     : Size of the buffer
 *
 * @return          Status code (see @ref AVRC_STS "AVRC status codes")
 *                  AVRC_STS_NO_ERROR, if the message in p_data is parsed successfully.
 *                  Otherwise, the error code defined by AVRCP 1.4
 *
 */
wiced_bt_avrc_sts_t wiced_bt_avrc_parse_response (wiced_bt_avrc_msg_t *p_msg, wiced_bt_avrc_response_t *p_result, uint8_t *p_buf, uint16_t buf_len);

/**
 * Build AVRCP command
 *
 * @param[in]       p_cmd       : Pointer to the structure to build the command from
 * @param[in]       p_xmit_buf  : Pointer to the buffer to build the command into
 *
 * @return          Status code (see @ref AVRC_STS "AVRC status codes")
 *                  AVRC_STS_NO_ERROR, if the message in p_data is parsed successfully.
 *                  Otherwise, the error code defined by AVRCP 1.4
 *
 */
wiced_bt_avrc_sts_t wiced_bt_avrc_bld_command( wiced_bt_avrc_command_t *p_cmd, wiced_bt_avrc_xmit_buf_t *p_xmit_buf);

/**
 * Build AVRCP response
 *
 * @param[in]       handle      : Connection handle
 * @param[in]       p_rsp       : Pointer to the structure to build the response from
 * @param[in]       p_rspbuf    : Pointer to the buffer to build the response into
 *
 *
 * @return          Status code (see @ref AVRC_STS "AVRC status codes")
 *                  AVRC_STS_NO_ERROR, if the message in p_data is parsed successfully.
 *                  Otherwise, the error code defined by AVRCP 1.4
 *
 */
wiced_bt_avrc_sts_t wiced_bt_avrc_bld_response( uint8_t handle, wiced_bt_avrc_response_t *p_rsp, wiced_bt_avrc_xmit_buf_t *p_rspbuf);

/**
 * Check if correct AVRC message type is specified
 *
 * @param[in]       pdu_id      : PDU ID
 * @param[in]       ctype       : avrc message type (see @ref AVRC_CTYPE "AVRC message types")
 *
 * @return          true if it is valid, false otherwise
 *
 *
 */
wiced_bool_t wiced_bt_avrc_is_valid_avc_type(uint8_t pdu_id, uint8_t ctype);

/**
 * Check if the given attrib value is a valid one
 *
 * @param[in]       attr      : Player attribute ID
 *
 * Returns          returns true if it is valid
 *
 */
wiced_bool_t wiced_bt_avrc_is_valid_player_attr(uint8_t attr);

/**
 *
 * This function gets the control MTU
 *
 * Returns          returns AVRC Control MTU
 *
 */
uint16_t wiced_bt_avrc_get_ctrl_mtu(void);

/**
 *
 * This function gets the data MTU
 *
 * Returns          returns AVRC DATA MTU
 *
 */
uint16_t wiced_bt_avrc_get_data_mtu(void);

/**@} wicedbt */
/* @endcond*/

#ifdef __cplusplus
}
#endif




