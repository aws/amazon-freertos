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
 * Bluetooth RFCOMM Application Programming Interface
 *
 */
#pragma once

#include "wiced_bt_dev.h"

/******************************************************
 *        Constants and Type definitions
 ******************************************************/

/** RFCOMM Port Event Masks */
typedef enum wiced_bt_rfcomm_port_event_e
{
    PORT_EV_RXCHAR          = 0x00000001,         /**< Any Character received */
    PORT_EV_RXFLAG          = 0x00000002,         /**< Received certain character */
    PORT_EV_TXEMPTY         = 0x00000004,         /**< Transmitt Queue Empty */
    PORT_EV_CTS             = 0x00000008,         /**< CTS changed state */
    PORT_EV_DSR             = 0x00000010,         /**< DSR changed state */
    PORT_EV_RLSD            = 0x00000020,         /**< RLSD changed state */
    PORT_EV_BREAK           = 0x00000040,         /**< BREAK received */
    PORT_EV_ERR             = 0x00000080,         /**< Line status error occurred */
    PORT_EV_RING            = 0x00000100,         /**< Ring signal detected */
    PORT_EV_CTSS            = 0x00000400,         /**< CTS state */
    PORT_EV_DSRS            = 0x00000800,         /**< DSR state */
    PORT_EV_RLSDS           = 0x00001000,         /**< RLSD state */
    PORT_EV_OVERRUN         = 0x00002000,         /**< receiver buffer overrun */
    PORT_EV_TXCHAR          = 0x00004000,         /**< Any character transmitted */
    PORT_EV_CONNECTED       = 0x00000200,         /**< RFCOMM connection established */
    PORT_EV_CONNECT_ERR     = 0x00008000,         /**< Was not able to establish connection or disconnected */
    PORT_EV_FC              = 0x00010000,         /**< data flow enabled flag changed by remote */
    PORT_EV_FCS             = 0x00020000,         /**< data flow enable status true = enabled */
} wiced_bt_rfcomm_port_event_t;

#define PORT_MASK_ALL             (PORT_EV_RXCHAR | PORT_EV_TXEMPTY | PORT_EV_CTS | \
                                   PORT_EV_DSR | PORT_EV_RLSD | PORT_EV_BREAK | \
                                   PORT_EV_ERR | PORT_EV_RING | PORT_EV_CONNECT_ERR | \
                                   PORT_EV_DSRS | PORT_EV_CTSS | PORT_EV_RLSDS | \
                                   PORT_EV_RXFLAG | PORT_EV_TXCHAR | PORT_EV_OVERRUN | \
                                   PORT_EV_FC | PORT_EV_FCS | PORT_EV_CONNECTED)



/** RFCOMM Result Codes */
enum wiced_bt_rfcomm_result_e
{
    WICED_BT_RFCOMM_SUCCESS,                                        /**< Success */
    WICED_BT_RFCOMM_ERROR,                                          /**< Error */
    WICED_BT_RFCOMM_ALREADY_OPENED,                                 /**< Already Opened */
    WICED_BT_RFCOMM_CMD_PENDING,                                    /**< Command Pending */
    WICED_BT_RFCOMM_APP_NOT_REGISTERED,                             /**< App Not Registered */
    WICED_BT_RFCOMM_NO_MEM,                                         /**< No Memory */
    WICED_BT_RFCOMM_NO_RESOURCES,                                   /**< No Resources */
    WICED_BT_RFCOMM_BAD_BD_ADDR,                                    /**< Bad BD Address */
    WICED_BT_RFCOMM_INVALID_MTU,                                    /**< Invalid MTU */
    WICED_BT_RFCOMM_BAD_HANDLE,                                     /**< Bad Handle */
    WICED_BT_RFCOMM_NOT_OPENED,                                     /**< Not Opened */
    WICED_BT_RFCOMM_LINE_ERR,                                       /**< Line Error */
    WICED_BT_RFCOMM_START_FAILED,                                   /**< Start Failed */
    WICED_BT_RFCOMM_PAR_NEG_FAILED,
    WICED_BT_RFCOMM_RFCOMM_NEG_FAILED,
    WICED_BT_RFCOMM_SEC_FAILED,
    WICED_BT_RFCOMM_PEER_CONNECTION_FAILED,                         /**< Peer Connection Failed */
    WICED_BT_RFCOMM_PEER_FAILED,                                    /**< Peer Failed */
    WICED_BT_RFCOMM_PEER_TIMEOUT,                                   /**< Peer Timeout */
    WICED_BT_RFCOMM_CLOSED,                                         /**< Closed */
    WICED_BT_RFCOMM_TX_FULL,
    WICED_BT_RFCOMM_LOCAL_CLOSED,                                   /**< Local Closed */
    WICED_BT_RFCOMM_LOCAL_TIMEOUT,                                  /**< Local Timeout */
    WICED_BT_RFCOMM_TX_QUEUE_DISABLED,
    WICED_BT_RFCOMM_PAGE_TIMEOUT,                                   /**< Page Timeout */
    WICED_BT_RFCOMM_INVALID_SCN                                     /**< Invalid SCN */
};
typedef int wiced_bt_rfcomm_result_t;                           /**< RFCOMM result code (see #wiced_bt_rfcomm_result_e) */

/** RFCOMM Signals */
enum wiced_bt_rfcomm_signal_e
{
    PORT_SET_DTRDSR=0x01,   /** DTRDSR set */
    PORT_CLR_DTRDSR,        /** DTRDSR clear */
    PORT_SET_CTSRTS,        /** CTSRTS set */
    PORT_CLR_CTSRTS,        /** CTSRTS clear */
    PORT_SET_RI,            /** RI set (DCE only) */
    PORT_CLR_RI,            /** RI clear (DCE only) */
    PORT_SET_DCD,           /** DCD set (DCE only) */
    PORT_CLR_DCD,           /** DCD clear (DCE only) */
    PORT_BREAK,             /** BRK */
};
typedef uint8_t wiced_bt_rfcomm_signal_t;   /**< RFCOMM Signals (see #wiced_bt_rfcomm_signal_e) */

/**
 *  Define the callback function prototypes for wiced_bt_rfcomm_data_cback_t
 *
 *  @param  port_handle         : A 16-bit unsigned integer returned by @link wiced_bt_rfcomm_create_connection wiced_bt_rfcomm_create_connection @endlink.
 *  @param  *p_data             : A pointer to the array of bytes received from the peer device.
 *  @param  len                 : The length of the data received.
 */
typedef int  (wiced_bt_rfcomm_data_cback_t) (uint16_t port_handle, void *p_data, uint16_t len);

/**
 *  Port management callback
 *
 *  @param  code                : Result code
 *  @param  port_handle         : Port handle from @link wiced_bt_rfcomm_create_connection wiced_bt_rfcomm_create_connection @endlink.
 */
typedef void (wiced_bt_port_mgmt_cback_t) (wiced_bt_rfcomm_result_t code, uint16_t port_handle);

/**
 *  Port event callback
 *
 *  @param  event               : A 32-bit event code that contains a bit-mask of one  or more events the caller would like to register.
 *  @param  port_handle         : Port handle from @link wiced_bt_rfcomm_create_connection wiced_bt_rfcomm_create_connection @endlink.
 */
typedef void (wiced_bt_port_event_cback_t) (wiced_bt_rfcomm_port_event_t event, uint16_t port_handle);

/**
 *  @addtogroup  rfcomm_api_functions       RFCOMM
 *  @ingroup     wicedbt
 *
 * The RFCOMM protocol provides emulation of serial ports over the L2CAP protocol. The protocol is
 * based on the ETSI standard GSM 7.1.0. RFCOMM is a simple transport protocol, with additional
 * provisions for emulating the nine circuits of RS-232 (ITU-T V.24) serial ports. The RFCOMM protocol
 * supports up to 60 simultaneous connections between two Bluetooth devices. The number of connections
 * that may be used simultaneously in a Bluetooth device is implementation-specific.
 *
 *  @{
 */
/******************************************************
 *               Function Declarations
 ******************************************************/
#ifdef __cplusplus
extern "C"
{
#endif

/**
 * Function         wiced_bt_rfcomm_set_buffer_pool
 *
 *                  If buffer_size > 0 and wiced_bt_rfcomm does not have a private pool yet,
 *                  this function allocates a private buffer pool.
 *                  If buffer_size = 0 and wiced_bt_rfcomm has a private buffer pool,
 *                  this function deallocates the private buffer pool.
 *
 *  @param[in]  buffer_size         : data size for the private pool. The actual buffer size includes additional overhead.
 *  @param[in]  buffer_count        : number of buffers in this new pool.
 *
 *  @return     WICED_BT_SUCCESS           if the action was carried out successfully as desired
 *              WICED_BT_NO_RESOURCES      no resources.
 */
wiced_bt_dev_status_t wiced_bt_rfcomm_set_buffer_pool(uint16_t buffer_size,
                            uint16_t buffer_count);

/**
 *  Establish serial port connection to the peer device, or allow
 *  RFCOMM to accept a connection from peer devices.
 *
 *  @note
 *  Server can call this function with the same scn parameter multiple times if
 *  it is ready to accept multiple simulteneous connections.
 *
 *  DLCI for the connection is (scn * 2 + 1) if client originates connection on
 *  existing none initiator multiplexer channel.  Otherwise it is (scn * 2).
 *  For the server DLCI can be changed later if client will be calling it using
 *  (scn * 2 + 1) dlci.
 *
 *  @param[in]  uuid            : The Universal Unique Identifier (UUID) of the
 *                                Class ID of the service being opened
 *  @param[in]  scn             : The Service Channel Number(SCN) as registered
 *                                with the SDP (server) or obtained using SDP from
 *                                the peer device (client)
 *  @param[in]  is_server       : TRUE if requesting application is a server
 *  @param[in]  mtu             : The maximum number of bytes transferred per frame
 *                                If 0, a default size of L2CAP_MTU_SIZE minus
 *                                5 bytes is used
 *  @param[in]  bd_addr         : BD_ADDR of the peer (if client), NULL if server
 *  @param[in]  p_mgmt_cb       : Pointer to callback function to receive connection
 *                                up/down events
 *  @param[out]  p_handle       : A pointer to the handle set by RFCOMM to be used in
 *                                consecutive calls for this connection
 *
 *  @return     <b> WICED_BT_RFCOMM_SUCCESS </b>        : If successful
 *              <b> WICED_BT_RFCOMM_ALREADY_OPENED </b> : If the client tries to establish a connection to the same BD_ADDR
 *              <b> WICED_BT_RFCOMM_NO_RESOURCES </b>   : If there is not enough memory to allocate a control block structure
 */
wiced_bt_rfcomm_result_t wiced_bt_rfcomm_create_connection (uint16_t uuid, uint8_t scn,
                                            wiced_bool_t is_server, uint16_t mtu,
                                            wiced_bt_device_address_t bd_addr,
                                            uint16_t *p_handle,
                                            wiced_bt_port_mgmt_cback_t *p_mgmt_cb);

/**
 *  Close the specified connection.
 *
 *  @param[in]  handle              : The connection handle returned by
 *                                    @link wiced_bt_rfcomm_create_connection wiced_bt_rfcomm_create_connection @endlink.
 *  @param[in]  remove_server       : (for server only) If TRUE, then also remove server; otherwise server remains enabled
 *                                    after connection is closed.
 *
 *  @return     <b> WICED_BT_RFCOMM_SUCCESS </b>        : If successful
 *              <b> WICED_BT_RFCOMM_BAD_HANDLE </b>     : If the handle is out of range
 *              <b> WICED_BT_RFCOMM_NOT_OPENED </b>     : If the connection is not opened
 */
wiced_bt_rfcomm_result_t wiced_bt_rfcomm_remove_connection (uint16_t handle, wiced_bool_t remove_server);


/**
 *  Set event callback the specified connection.
 *
 *  @param[in]  port_handle         : A 16-bit unsigned integer returned by
 *                                    @link wiced_bt_rfcomm_create_connection wiced_bt_rfcomm_create_connection @endlink
 *  @param[in]  p_port_cb           : Address of the callback function which should
 *                                    be called from the RFCOMM when an event
 *                                    specified in the mask occurs
 *
 *  @return     <b> WICED_BT_RFCOMM_SUCCESS </b>        : If successful
 *              <b> WICED_BT_RFCOMM_BAD_HANDLE </b>     : If the handle is out of range
 *              <b> WICED_BT_RFCOMM_NOT_OPENED </b>     : If the connection is not opened
 */
wiced_bt_rfcomm_result_t wiced_bt_rfcomm_set_event_callback (uint16_t port_handle, wiced_bt_port_event_cback_t *p_port_cb);

/**
 *  Set event data callback the specified connection.
 *
 *  @param[in]  port_handle         : A 16-bit unsigned integer returned by
 *                                    @link wiced_bt_rfcomm_create_connection wiced_bt_rfcomm_create_connection @endlink
 *  @param[in]  p_cb                : Address of the callback function which should
 *                                    be called from the RFCOMM when a data
 *                                    packet is received
 *
 *  @return     <b> WICED_BT_RFCOMM_SUCCESS </b>        : If successful
 *              <b> WICED_BT_RFCOMM_BAD_HANDLE </b>     : If the handle is out of range
 *              <b> WICED_BT_RFCOMM_NOT_OPENED </b>     : If the connection is not opened
 */
wiced_bt_rfcomm_result_t wiced_bt_rfcomm_set_data_callback (uint16_t port_handle, wiced_bt_rfcomm_data_cback_t *p_cb);


/**
 *  Set events for which to be notified
 *
 *  @param[in]  port_handle         : A 16-bit unsigned integer returned by
 *                                    @link wiced_bt_rfcomm_create_connection wiced_bt_rfcomm_create_connection @endlink
 *  @param[in]  mask                : Event mask
 *
 *  @return     <b> WICED_BT_RFCOMM_SUCCESS </b>        : If successful
 *              <b> WICED_BT_RFCOMM_BAD_HANDLE </b>     : If the handle is out of range
 *              <b> WICED_BT_RFCOMM_NOT_OPENED </b>     : If the connection is not opened
 */
wiced_bt_rfcomm_result_t wiced_bt_rfcomm_set_event_mask (uint16_t port_handle, wiced_bt_rfcomm_port_event_t mask);

/**
 *  Send control signal to the peer device.
 *
 *  @param[in]  handle              : The connection handle returned by
 *                                    @link wiced_bt_rfcomm_create_connection wiced_bt_rfcomm_create_connection @endlink
 *  @param[in]  signal              : Signal to send (see #wiced_bt_rfcomm_signal_e)
 *
 *  @return     <b> WICED_BT_RFCOMM_SUCCESS </b>        : If successful
 *              <b> WICED_BT_RFCOMM_BAD_HANDLE </b>     : If the handle is out of range
 *              <b> WICED_BT_RFCOMM_NOT_OPENED </b>     : If the connection is not opened
 */
wiced_bt_rfcomm_result_t wiced_bt_rfcomm_control (uint16_t handle, uint8_t signal);

/**
 *  This function directs a specified connection to pass flow control message to the peer device.
 *  Enable flag passed shows if port can accept more data.
 *
 *  @param[in]  handle              : The connection handle returned by
 *                                    @link wiced_bt_rfcomm_create_connection wiced_bt_rfcomm_create_connection @endlink
 *  @param[in]  enable              : Flow control setting
 *                                    TRUE  Enable data flow
 *                                    FALSE Disable data flow
 *
 *  @return     <b> WICED_BT_RFCOMM_SUCCESS </b>        : If successful
 *              <b> WICED_BT_RFCOMM_BAD_HANDLE </b>     : If the handle is out of range
 *              <b> WICED_BT_RFCOMM_NOT_OPENED </b>     : If the connection is not opened
 */
wiced_bt_rfcomm_result_t wiced_bt_rfcomm_flow_control (uint16_t handle, wiced_bool_t enable);

/**
 *  This function sends the given application data to the peer device.
 *  If wiced_bt_rfcomm_set_buffer_pool() was called to create a private buffer pool, the buffer from the private buffer pool is used to
 *  hold the data in the RFCOMM TX queue. Otherwise, RFCOMM uses the RFCOMM_DATA_POOL_ID pool to hold the data.
 *
 *  @param[in]  handle              : The connection handle returned by
 *                                    @link wiced_bt_rfcomm_create_connection wiced_bt_rfcomm_create_connection @endlink
 *  @param[in]  p_data              : Data to write
 *  @param[in]  max_len             : Byte count to write
 *  @param[out] p_len               : Bytes written
 *
 *  @return     <b> WICED_BT_RFCOMM_SUCCESS </b>        : If successful
 *              <b> WICED_BT_RFCOMM_BAD_HANDLE </b>     : If the handle is out of range
 *              <b> WICED_BT_RFCOMM_NOT_OPENED </b>     : If the connection is not opened
 */
wiced_bt_rfcomm_result_t wiced_bt_rfcomm_write_data (uint16_t handle, char *p_data, uint16_t max_len, uint16_t *p_len);

/**
 *  This function checks connection referenced by handle is up and running.
 *
 *  @param[in]  handle              : The connection handle returned by
 *                                    @link wiced_bt_rfcomm_create_connection wiced_bt_rfcomm_create_connection @endlink
 *  @param[out]  bd_addr            : Peer BD Address
 *  @param[out]  p_lcid             : L2CAP's LCID
 *
 *  @return     <b> WICED_BT_RFCOMM_SUCCESS </b>        : If successful
 *              <b> WICED_BT_RFCOMM_LINE_ERR </b>       : If connection is not up and running
 */
wiced_bt_rfcomm_result_t wiced_bt_rfcomm_check_connection (UINT16 handle, BD_ADDR bd_addr, UINT16 *p_lcid);

#ifdef __cplusplus
}
#endif

/**@} */
