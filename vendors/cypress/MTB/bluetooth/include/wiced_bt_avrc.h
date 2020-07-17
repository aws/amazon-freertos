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
 * Bluetooth AVRCP Application Programming Interface
 *
 */
#pragma once

#include "wiced_bt_sdp.h"
#include "wiced_bt_avrc_defs.h"

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

/* Supported categories */
#define AVRC_SUPF_CT_CAT1               0x0001      /* Category 1 */
#define AVRC_SUPF_CT_CAT2               0x0002      /* Category 2 */
#define AVRC_SUPF_CT_CAT3               0x0004      /* Category 3 */
#define AVRC_SUPF_CT_CAT4               0x0008      /* Category 4 */
#define AVRC_SUPF_CT_BROWSE             0x0040      /* Browsing */

#define AVRC_SUPF_TG_CAT1               0x0001      /* Category 1 */
#define AVRC_SUPF_TG_CAT2               0x0002      /* Category 2 */
#define AVRC_SUPF_TG_CAT3               0x0004      /* Category 3 */
#define AVRC_SUPF_TG_CAT4               0x0008      /* Category 4 */
#define AVRC_SUPF_TG_APP_SETTINGS       0x0010      /* Player Application Settings */
#define AVRC_SUPF_TG_GROUP_NAVI         0x0020      /* Group Navigation */
#define AVRC_SUPF_TG_BROWSE             0x0040      /* Browsing */
#define AVRC_SUPF_TG_MULTI_PLAYER       0x0080      /* Muliple Media Player */

#define AVRC_META_SUCCESS               AVRC_SUCCESS
#define AVRC_META_FAIL                  AVRC_FAIL
#define AVRC_METADATA_CMD               0x0000
#define AVRC_METADATA_RESP              0x0001



/*****************************************************************************
**  data type definitions
*****************************************************************************/

/**
 *
 * Function         wiced_bt_avrc_ctrl_cback_t
 *
 *                  AVRC control callback function.
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
 *
 * Function         wiced_bt_avrc_msg_cback_t
 *
 *                  AVRC message callback function.  It is executed when AVCTP has
 *                  a message packet ready for the application.  The implementation of this
 *                  callback function must copy the wiced_bt_avrc_msg_t structure passed to it as it
 *                  is not guaranteed to remain after the callback function exits.
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

/** AVRC connection control block; used when calling wiced_bt_avrc_open() to configure the AVRC connection and register for callbacks. */
typedef struct
{
    wiced_bt_avrc_ctrl_cback_t      *p_ctrl_cback;  /**< AVRC connection control callback */
    wiced_bt_avrc_msg_cback_t       *p_msg_cback;   /**< AVRC message callback */
    uint32_t                        company_id;     /**< Company ID  (see @ref AVRC_COMPANY_ID "Company IDs") */
    uint8_t                         conn;           /**< Connection role: AVRC_CONN_INT (initiator) or AVRC_CONN_ACP (acceptor) (see @ref AVRC_CONN_ROLE "AVRC connection roles") */
    uint8_t                         control;        /**< Control role: AVRC_CT_TARGET (target) or AVRC_CT_CONTROL (controller) (see @ref AVRC_CT_ROLE "AVRC control roles")*/
} wiced_bt_avrc_conn_cb_t;


/*****************************************************************************
**  external function declarations
*****************************************************************************/

/**
 *
 * @defgroup wicedbt_av  Audio/Video
 * @ingroup  wicedbt
 *
 * @defgroup wicedbt_av_helper Audio/Video Helper Functions
 * @ingroup  wicedbt_av
 *
 * @defgroup wicedbt_avrc_helper AVRCP Helper Functions
 * @ingroup  wicedbt_av_helper
 *
 * @addtogroup  wicedbt_avrc_helper        AVRCP Profile Helper Functions
 * @ingroup     wicedbt_av_helper
 * @ingroup     wicedbt_av
 *
 * AVRCP Helper Functions
 *
 * @{
 * @{
*/

#ifdef __cplusplus
extern "C"
{
#endif

/**
 *
 * Function         wiced_bt_avrc_open
 *
 *                  Open AVRC connection (as intiator or acceptor); register notification callbacks.
 *
 *                  The connection role may be AVRC controller or target.
 *
 *                  The connection remains available to the application until
 *                  wiced_bt_avrc_close() is called.
 *
 *                  On receiving AVRC_CLOSE_IND_EVT, acceptor connections remain in
 *                  acceptor mode (no need to re-open the connection)
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
 *
 * Function         wiced_bt_avrc_close
 *
 *                  Close AVRCP connection
 *
 * @param[in]       handle      : Handle of connection to close
 *
 * @return          Result code (see @ref AVRC_RESULT "AVRC result codes")
 *
 */
uint16_t wiced_bt_avrc_close(uint8_t handle);

/**
 *
 * Function         wiced_bt_avrc_open_browse
 *
 *                  Open AVRCP browsing connection, either as initiator or acceptor.
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
 *
 * Function         wiced_bt_avrc_close_browse
 *
 *                  Close AVRCP browsing connection
 *
 * @param[in]       handle      : Connection handle
 *
 * @return          Result code (see @ref AVRC_RESULT "AVRC result codes")
 *
 */
uint16_t wiced_bt_avrc_close_browse(uint8_t handle);

/**
 *
 * Function         wiced_bt_avrc_set_buffer_pool
 *
 *                  If buffer_size > 0 and there's no private pool for re-assembly,
 *                  this function allocates a private buffer pool for
 *                  re-assembling incoming messages
 *                  If buffer_size = 0 and there's a private buffer pool for re-assembly,
 *                  this function deallocates the private buffer pool for
 *                  re-assembling incoming messages
 *
 * @param[in]       buffer_size      : buffer size for the private pool
 * @param[in]       buffer_count     : the number of buffers in this new pool
 *
 * @return          Result code (see @ref AVRC_RESULT "AVRC result codes")
 *
 */
uint16_t wiced_bt_avrc_set_buffer_pool(uint16_t buffer_size, uint16_t buffer_count);

/**
 *
 * Function         wiced_bt_avrc_msg_req
 *
 *                  Send an AVRC message
 *
 * @note            It is expected that p_pkt->offset is at least AVCT_MSG_OFFSET
 *                  p_pkt->layer_specific is AVCT_DATA_CTRL or AVCT_DATA_BROWSE
 *                  p_pkt->event is AVRC_OP_VENDOR, AVRC_OP_PASS_THRU or AVRC_OP_BROWSING
 *                  The above BT_HDR settings are set by the AVRC_Bld* functions.
 *
 * @param[in]       handle      : Connection handle
 * @param[in]       label       : Transaction label
 * @param[in]       ctype       : Message type (see @ref AVRC_CTYPE "AVRC message types")
 * @param[in]       p_pkt       : Pointer to the buffer holding the AVRC message
 *
 * @return          Result code (see @ref AVRC_RESULT "AVRC result codes")
 *
 */
uint16_t wiced_bt_avrc_msg_req (uint8_t handle, uint8_t label, uint8_t ctype, BT_HDR *p_pkt);

/**
 *
 * Function         wiced_bt_avrc_unit_cmd
 *
 *                  Send a UNIT INFO command to the peer device. This
 *                  function can only be called for controller role connections.
 *                  Any response message from the peer is passed back through
 *                  the wiced_bt_avrc_msg_cback_t callback function.
 *
 * @param[in]       handle      : Connection handle
 * @param[in]       label       : Transaction label
 *
 * @return          Result code (see @ref AVRC_RESULT "AVRC result codes")
 *
 */
uint16_t wiced_bt_avrc_unit_cmd(uint8_t handle, uint8_t label);

/**
 *
 * Function         wiced_bt_avrc_sub_cmd
 *
 *                  Send a SUBUNIT INFO command to the peer device.  This
 *                  function can only be called for controller role connections.
 *                  Any response message from the peer is passed back through
 *                  the wiced_bt_avrc_msg_cback_t callback function.
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
 *
 * Function         wiced_bt_avrc_pass_cmd
 *
 *                  Send a PASS THROUGH command to the peer device.  This
 *                  function can only be called for controller role connections.
 *                  Any response message from the peer is passed back through
 *                  the wiced_bt_avrc_msg_cback_t callback function.
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
 *
 * Function         wiced_bt_avrc_pass_rsp
 *
 *                  Send a PASS THROUGH response to the peer device.  This
 *                  function can only be called for target role connections.
 *                  This function must be called when a PASS THROUGH command
 *                  message is received from the peer through the
 *                  wiced_bt_avrc_msg_cback_t callback function.
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
 *
 * Function         wiced_bt_avrc_vendor_cmd
 *
 *                  Send a VENDOR DEPENDENT command to the peer device.  This
 *                  function can only be called for controller role connections.
 *                  Any response message from the peer is passed back through
 *                  the wiced_bt_avrc_msg_cback_t callback function.
 *
 * @param[in]       handle      : Connection handle
 * @param[in]       label       : Transaction label
 * @param[in]       p_msg       : Pointer to the vendor dependent command
 *
 * @return          Result code (see @ref AVRC_RESULT "AVRC result codes")
 *
 */
uint16_t wiced_bt_avrc_vendor_cmd(uint8_t  handle, uint8_t  label, wiced_bt_avrc_msg_vendor_t *p_msg);


/**
 *
 * Function         wiced_bt_avrc_vendor_rsp
 *
 *                  Send a VENDOR DEPENDENT response to the peer device.  This
 *                  function can only be called for target role connections.
 *                  This function must be called when a VENDOR DEPENDENT
 *                  command message is received from the peer through the
 *                  wiced_bt_avrc_msg_cback_t callback function.
 *
 * @param[in]       handle      : Connection handle
 * @param[in]       label       : Transaction label
 * @param[in]       p_msg       : Pointer to the vendor dependent response
 *
 * @return          Result code (see @ref AVRC_RESULT "AVRC result codes")
 *
 */
uint16_t wiced_bt_avrc_vendor_rsp(uint8_t  handle, uint8_t  label, wiced_bt_avrc_msg_vendor_t *p_msg);


/*
 *
 * Function         wiced_bt_avrc_set_trace_level
 *
 *                  Sets the trace level for AVRC. If 0xff is passed, the
 *                  current trace level is returned.
 *
 * @param[in]       new_level   : New trace level
 *
 * @return          The new trace level or current trace level if
 *                  the input parameter is 0xff.
 *
 */
uint8_t wiced_bt_avrc_set_trace_level (uint8_t new_level);

/**
 *
 * Function         wiced_bt_avrc_parse_command
 *
 *                  Parse incoming AVRCP command message.
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
 *
 * Function         wiced_bt_avrc_parse_response
 *
 *                  Parse incoming AVRCP response message.
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
 *
 * Function         wiced_bt_avrc_bld_command
 *
 *                  Build AVRCP command
 *
 * @param[out]      pp_pkt      : Pointer to pointer to the built command
 * @param[in]       p_cmd       : Pointer to the structure to build the command from
 *
 * @return          Status code (see @ref AVRC_STS "AVRC status codes")
 *                  AVRC_STS_NO_ERROR, if the message in p_data is parsed successfully.
 *                  Otherwise, the error code defined by AVRCP 1.4
 *
 */
wiced_bt_avrc_sts_t wiced_bt_avrc_bld_command( wiced_bt_avrc_command_t *p_cmd, BT_HDR **pp_pkt);

/**
 * Function         wiced_bt_avrc_bld_response
 *
 *                  Build AVRCP response
 *
 * @param[out]      pp_pkt      : Pointer to pointer to the built response
 * @param[in]       handle      : Connection handle
 * @param[in]       p_rsp       : Pointer to the structure to build the response from
 *
 *
 * @return          Status code (see @ref AVRC_STS "AVRC status codes")
 *                  AVRC_STS_NO_ERROR, if the message in p_data is parsed successfully.
 *                  Otherwise, the error code defined by AVRCP 1.4
 *
 */
wiced_bt_avrc_sts_t wiced_bt_avrc_bld_response( uint8_t handle, wiced_bt_avrc_response_t *p_rsp, BT_HDR **pp_pkt);

/*
 *
 * Function         wiced_bt_avrc_is_valid_avc_type
 *
 *                  Check if correct AVRC message type is specified
 *
 * @param[in]       pdu_id      : PDU ID
 * @param[in]       ctype       : avrc message type (see @ref AVRC_CTYPE "AVRC message types")
 *
 * @return          true if it is valid, false otherwise
 *
 *
 */
wiced_bool_t wiced_bt_avrc_is_valid_avc_type(uint8_t pdu_id, uint8_t ctype);

/*
 *
 * Function         wiced_bt_avrc_is_valid_player_attr
 *
 *                  Check if the given attrib value is a valid one
 *
 * @param[in]       attr      : Player attribute ID
 *
 * Returns          returns true if it is valid
 *
 */
wiced_bool_t wiced_bt_avrc_is_valid_player_attr(uint8_t attr);

/*
 *
 * Function         wiced_bt_avrc_get_ctrl_mtu
 *
 * Returns          returns AVRC Control MTU
 *
 */
uint16_t wiced_bt_avrc_get_ctrl_mtu(void);

/*
 *
 * Function         wiced_bt_avrc_get_ctrl_mtu
 *
 * Returns          returns AVRC DATA MTU
 *
 */
uint16_t wiced_bt_avrc_get_data_mtu(void);

#ifdef __cplusplus
}
#endif

/** @} wicedbt_avrc_helper */
/** @} wicedbt_av */


