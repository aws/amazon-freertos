/*
 * $ Copyright Cypress Semiconductor $
 */

/** @file
 *
 * Bluetooth RFCOMM Application Programming Interface
 *
 */
 /**
  * @cond DUAL_MODE
  * @addtogroup  rfcomm_api_functions       RFCOMM
  *
  * The RFCOMM protocol provides emulation of serial ports over the L2CAP protocol. The protocol is
  * based on the ETSI standard GSM 7.1.0. RFCOMM is a simple transport protocol, with additional
  * provisions for emulating the nine circuits of RS-232 (ITU-T V.24) serial ports. The RFCOMM protocol
  * supports up to 60 simultaneous connections between two Bluetooth devices. The number of connections
  * that may be used simultaneously in a Bluetooth device is implementation-specific.
  *
  *  @{
  */
#pragma once

#include "wiced_bt_dev.h"

/******************************************************
 *        Constants and Type definitions
 ******************************************************/

/**RFCOMM Port Event Masks*/
typedef enum wiced_bt_rfcomm_port_event_e
{
    PORT_EV_NONE            = 0x00000000,         /**< No event */
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
    PORT_EV_CONNECTED       = 0x00000200,         /**< RFCOMM connection established */
    PORT_EV_CONNECT_ERR     = 0x00008000,         /**< Was not able to establish connection or disconnected */
    PORT_EV_FC              = 0x00010000,         /**< data flow enabled flag changed by remote */
    PORT_EV_FCS             = 0x00020000,         /**< data flow enable status true = enabled */
} wiced_bt_rfcomm_port_event_t;

/** RFCOMM Port Mask to enable all */
#define PORT_MASK_ALL             (PORT_EV_RXCHAR | PORT_EV_TXEMPTY | PORT_EV_CTS | \
                                   PORT_EV_DSR | PORT_EV_RLSD | PORT_EV_BREAK | \
                                   PORT_EV_ERR | PORT_EV_RING | PORT_EV_CONNECT_ERR | \
                                   PORT_EV_DSRS | PORT_EV_CTSS | PORT_EV_RLSDS | \
                                   PORT_EV_RXFLAG | PORT_EV_OVERRUN | \
                                   PORT_EV_FC | PORT_EV_FCS | PORT_EV_CONNECTED)

/** RFCOMM Result Codes*/
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
    WICED_BT_RFCOMM_PAR_NEG_FAILED,                                 /**< DLC parameter negotiation failed */
    WICED_BT_RFCOMM_RFCOMM_NEG_FAILED,                              /**< Remote port negotiation failed*/
    WICED_BT_RFCOMM_PEER_CONNECTION_FAILED,                         /**< Peer Connection Failed */
    WICED_BT_RFCOMM_PEER_FAILED,                                    /**< Peer Failed */
    WICED_BT_RFCOMM_PEER_TIMEOUT,                                   /**< Peer Timeout */
    WICED_BT_RFCOMM_CLOSED,                                         /**< Closed */
    WICED_BT_RFCOMM_TX_FULL,                                        /**< No space to transmit the packet*/
    WICED_BT_RFCOMM_LOCAL_CLOSED,                                   /**< Local Closed */
    WICED_BT_RFCOMM_LOCAL_TIMEOUT,                                  /**< Local Timeout */
    WICED_BT_RFCOMM_TX_QUEUE_DISABLED,                              /**< Transmit queue disabled */
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
 *  Port management callback
 *
 *  @param  code                : Result code
 *  @param  port_handle         : Port handle from @link wiced_bt_rfcomm_create_connection @endlink.
 */
typedef void (wiced_bt_port_mgmt_cback_t) (wiced_bt_rfcomm_result_t code, uint16_t port_handle);

/**
 *  Port event callback
 *
 *  @param  event               : A 32-bit event code that contains a bit-mask of one or more events the caller would like to register.(see #wiced_bt_rfcomm_port_event_t) 
 *  @param  port_handle         : Port handle from @link wiced_bt_rfcomm_create_connection wiced_bt_rfcomm_create_connection @endlink.
 */
typedef void (wiced_bt_port_event_cback_t) (wiced_bt_rfcomm_port_event_t event, uint16_t port_handle);

/**
*  Port TX Complete callback
*
*  @param  port_handle         : Port handle from @link wiced_bt_rfcomm_create_connection wiced_bt_rfcomm_create_connection @endlink.
*  @param  p_data              : Pointer to application data
*/
typedef void (wiced_bt_port_tx_cback_t)(uint16_t port_handle, void *p_data);

/******************************************************
 *               Function Declarations
 ******************************************************/
#ifdef __cplusplus
extern "C"
{
#endif

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
 *                                If 0, a default size of L2CAP_MAX_RX_MTU minus
 *                                5 bytes is used
 *  @param[in]  bd_addr         : BD_ADDR of the peer (if client), NULL if server (see #wiced_bt_device_address_t) 
 *  @param[in]  p_mgmt_cb       : Pointer to callback function to receive connection
 *                                up/down events (see #wiced_bt_port_mgmt_cback_t)
 *  @param[out]  p_handle       : A pointer to the handle set by RFCOMM to be used in
 *                                consecutive calls for this connection
 *
 *  @return     <b> WICED_BT_RFCOMM_SUCCESS </b>        : If successful\n
 *              <b> WICED_BT_RFCOMM_ALREADY_OPENED </b> : If the client tries to establish a connection to the same BD_ADDR\n
 *              <b> WICED_BT_RFCOMM_NO_RESOURCES </b>   : If there is not enough memory to allocate a control block structure\n
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
 *                                    @link wiced_bt_rfcomm_create_connection @endlink.
 *  @param[in]  remove_server       : (for server only) If TRUE, then also remove server; otherwise server remains enabled
 *                                    after connection is closed.
 *
 *  @return     <b> WICED_BT_RFCOMM_SUCCESS </b>        : If successful \n
 *              <b> WICED_BT_RFCOMM_BAD_HANDLE </b>     : If the handle is out of range \n
 *              <b> WICED_BT_RFCOMM_NOT_OPENED </b>     : If the connection is not opened \n
 */
wiced_bt_rfcomm_result_t wiced_bt_rfcomm_remove_connection (uint16_t handle, wiced_bool_t remove_server);

/**
 *  Set event callback the specified connection.
 *
 *  @param[in]  port_handle         : A 16-bit unsigned integer returned by
 *                                    @link wiced_bt_rfcomm_create_connection @endlink
 *  @param[in]  p_port_cb           : Address of the callback function which should
 *                                    be called from the RFCOMM when an event
 *                                    specified in the mask occurs. (see #wiced_bt_port_event_cback_t)
 *  @param[in]  p_tx_cmpl_cb        : Address of the callback function which should
 *                                    be called from the RFCOMM when TX complete (see #wiced_bt_port_tx_cback_t)
 *
 *  @return     <b> WICED_BT_RFCOMM_SUCCESS </b>        : If successful\n
 *              <b> WICED_BT_RFCOMM_BAD_HANDLE </b>     : If the handle is out of range\n
 *              <b> WICED_BT_RFCOMM_NOT_OPENED </b>     : If the connection is not opened\n
 */
wiced_bt_rfcomm_result_t wiced_bt_rfcomm_set_event_callback (uint16_t port_handle, wiced_bt_port_event_cback_t *p_port_cb,
                                                             wiced_bt_port_tx_cback_t *p_tx_cmpl_cb);

/**
 *  Set events for which to be notified
 *
 *  @param[in]  port_handle         : A 16-bit unsigned integer returned by
 *                                    @link wiced_bt_rfcomm_create_connection wiced_bt_rfcomm_create_connection @endlink
 *  @param[in]  mask                : Event mask (see #wiced_bt_rfcomm_port_event_t)
 *
 *  @return     <b> WICED_BT_RFCOMM_SUCCESS </b>        : If successful\n
 *              <b> WICED_BT_RFCOMM_BAD_HANDLE </b>     : If the handle is out of range\n
 *              <b> WICED_BT_RFCOMM_NOT_OPENED </b>     : If the connection is not opened\n
 */
wiced_bt_rfcomm_result_t wiced_bt_rfcomm_set_event_mask (uint16_t port_handle, wiced_bt_rfcomm_port_event_t mask);

/**
 *  Send control signal to the peer device.
 *
 *  @param[in]  handle              : The connection handle returned by
 *                                    @link wiced_bt_rfcomm_create_connection wiced_bt_rfcomm_create_connection @endlink
 *  @param[in]  signal              : Signal to send (see #wiced_bt_rfcomm_signal_e)
 *
 *  @return     <b> WICED_BT_RFCOMM_SUCCESS </b>        : If successful\n
 *              <b> WICED_BT_RFCOMM_BAD_HANDLE </b>     : If the handle is out of range\n
 *              <b> WICED_BT_RFCOMM_NOT_OPENED </b>     : If the connection is not opened\n
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
 *  @return     <b> WICED_BT_RFCOMM_SUCCESS </b>        : If successful\n
 *              <b> WICED_BT_RFCOMM_BAD_HANDLE </b>     : If the handle is out of range\n
 *              <b> WICED_BT_RFCOMM_NOT_OPENED </b>     : If the connection is not opened\n
 */
wiced_bt_rfcomm_result_t wiced_bt_rfcomm_flow_control (uint16_t handle, wiced_bool_t enable);

/**
*  This function provides RFCOMM with a data area to receive peer's data.
*
*  @param[in]  handle              : The connection handle returned by
*                                    @link wiced_bt_rfcomm_create_connection wiced_bt_rfcomm_create_connection @endlink
*  @param[in]  p_mem               : Pointer to the fifo area
*  @param[in]  size                : Size of the fifo area
*
*  @return     <b> WICED_BT_RFCOMM_SUCCESS </b>        : If successful\n
*              <b> WICED_BT_RFCOMM_BAD_HANDLE </b>     : If the handle is out of range\n
*              <b> WICED_BT_RFCOMM_NOT_OPENED </b>     : If the connection is not opened\n
*/
wiced_bt_rfcomm_result_t wiced_bt_rfcomm_set_rx_fifo (uint16_t handle, char *p_mem, uint16_t size);

/**
*  This function reads the received application data from the peer device.
*
*  @param[in]  handle              : The connection handle returned by
*                                    @link wiced_bt_rfcomm_create_connection wiced_bt_rfcomm_create_connection @endlink
*  @param[in]  p_data              : Where to read the data into
*  @param[in]  max_len             : Maximum Byte count to read
*  @param[out] p_len               : Bytes actually read
*
*  @return     <b> WICED_BT_RFCOMM_SUCCESS </b>        : If successful\n
*              <b> WICED_BT_RFCOMM_BAD_HANDLE </b>     : If the handle is out of range\n
*              <b> WICED_BT_RFCOMM_NOT_OPENED </b>     : If the connection is not opened\n
*/
wiced_bt_rfcomm_result_t wiced_bt_rfcomm_read_data(uint16_t handle, char *p_data, uint16_t max_len, uint16_t *p_len);

/**
 *  This function sends the given application data to the peer device.
 *
 *  @param[in]  handle              : The connection handle returned by
 *                                    @link wiced_bt_rfcomm_create_connection wiced_bt_rfcomm_create_connection @endlink
 *  @param[in]  p_data              : Data to write
 *  @param[in]  max_len             : Byte count to write
 *
 *  @return     <b> WICED_BT_RFCOMM_SUCCESS </b>        : If successful\n
 *              <b> WICED_BT_RFCOMM_BAD_HANDLE </b>     : If the handle is out of range\n
 *              <b> WICED_BT_RFCOMM_NOT_OPENED </b>     : If the connection is not opened\n
 */
wiced_bt_rfcomm_result_t wiced_bt_rfcomm_write_data (uint16_t handle, char *p_data, uint16_t max_len );

/**
 *  This function checks connection referenced by handle is up and running.
 *
 *  @param[in]  handle              : The connection handle returned by
 *                                    @link wiced_bt_rfcomm_create_connection wiced_bt_rfcomm_create_connection @endlink
 *  @param[out]  bd_addr            : Peer BD Address (see #wiced_bt_device_address_t)
 *  @param[out]  p_lcid             : L2CAP's LCID
 *
 *  @return     <b> WICED_BT_RFCOMM_SUCCESS </b>        : If successful\n
 *              <b> WICED_BT_RFCOMM_LINE_ERR </b>       : If connection is not up and running\n
 */
wiced_bt_rfcomm_result_t wiced_bt_rfcomm_check_connection (uint16_t handle, wiced_bt_device_address_t bd_addr, uint16_t *p_lcid);


#ifdef __cplusplus
}
#endif

/**@}rfcomm_api_functions */
/**  @endcond */

