
/*
 * $ Copyright Cypress Semiconductor $
 */

/** @file
 * Bluetooth L2CAP Application Programming Interface
 */

#pragma once
#include "l2cdefs.h"
#include "hcidefs.h"
#include "wiced_bt_types.h"
#include "wiced_bt_ble.h"

/**
 *  @addtogroup    l2cap   Logical Link Control and Adaptation Protocol (L2CAP)
 *
 * Logical Link Control and Adaptation Layer Protocol,
 * referred to as L2CAP, provides connection oriented and
 * connectionless data services to upper layer protocols with protocol
 * multiplexing capability and segmentation and reassembly operation.
 *
 * @{
 */

/*****************************************************************************
 *  Constants
 ****************************************************************************/

#define L2CAP_MINIMUM_OFFSET               13   /**< Minimum offset that L2CAP needs in a buffer. This is made up of
                                                     HCI type(1), len(2), handle(2), L2CAP len(2),  CID(2),  control(2) and  SDU length(2) */

#define L2CAP_BLE_CONN_MIN_OFFSET          9    /**< HCI type(1), len(2), handle(2), L2CAP len(2) and CID(2) */
#define L2CAP_DEFAULT_BLE_CB_POOL_ID    0xFF    /**< Use the default HCI ACL buffer pool */
#define L2CAP_BLE_COC_SDU_OFFSET           4    /**< to provide upper layer some minimal offset possibly required
    **  to process incoming packets */

#define L2CAP_BROADCAST_MIN_OFFSET         11  /**< Minimum offset for broadcast needs another two bytes for the PSM */

/**
 * @anchor L2CAP_PING_RESULT
 * @name L2CAP ping result
 * @{
 *
 * Ping result codes.
 */
#define L2CAP_PING_RESULT_OK            0       /**< Ping reply received OK     */
#define L2CAP_PING_RESULT_NO_LINK       1       /**< Link could not be setup    */
#define L2CAP_PING_RESULT_NO_RESPONSE   2       /**< Remote L2CAP did not reply */
/** @} L2CAP_PING_RESULT */

/**
 * @anchor L2CAP_DATAWRITE
 * @name L2CAP data write result
 * @{
 *
 * Result codes for \ref wiced_bt_l2cap_data_write()
 */
#define L2CAP_DATAWRITE_FAILED        FALSE     /**< If data not accepted and error */
#define L2CAP_DATAWRITE_SUCCESS       TRUE      /**< If data accepted */
#define L2CAP_DATAWRITE_CONGESTED     2         /**< if data accepted and the channel is congested */
/** @} L2CAP_DATAWRITE */

/**
 * @anchor L2CAP_DATAWRITE_FLUSH
 * @name L2CAP data write flags
 * @{
 *
 * Flushable/Non Flushable flags parameter used in a call \ref wiced_bt_l2cap_data_write()
 */
#define L2CAP_FLUSHABLE_MASK        0x0001  /**< L2CAP Flushable mask */
#define L2CAP_NON_FLUSHABLE_PACKET  0x0000  /**< Non Flushable flag  */
#define L2CAP_FLUSHABLE_PACKET      0x0001  /**< Flushable flag  */
/** @} L2CAP_DATAWRITE_FLUSH */

/**
 * @anchor  L2CAP_PSM_CHECK
 * @name L2CAP PSM
 * @{
 *
 *  Validity check for PSM.  PSM values must be odd.  Also, all PSM values must
 *  be assigned such that the least significant bit of the most sigificant
 *  octet equals zero.
 *
*/
#define L2C_INVALID_PSM(psm)    (((psm) & 0x0101) != 0x0001)  /**< Returns true on invalid PSM */
#define L2C_IS_VALID_PSM(psm)   (((psm) & 0x0101) == 0x0001)  /**< Returns true on valid PSM */
/** @} L2CAP_PSM_CHECK */


/*****************************************************************************
 *  Type Definitions
 ****************************************************************************/

/** Structure for Enhanced Retransmission Mode Options
 *  Refer to Volume 3, Part A, section 5.4 of BT Core specification for details */
typedef struct
{
    uint8_t  mode;                 /**< Requested mode of link. @cond DUAL_MODE Refer \ref L2CAP_FCR_MODE "L2CAP FCR mode" @endcond */
    uint8_t  tx_window_size;       /**< Maximum transmit window size (1..63) */
    uint8_t  max_transmit;         /**< Maximum number of trasmission attempts */
    uint16_t rtrans_timeout_ms;    /**< Retransmission timeout (msecs) */
    uint16_t monitor_timeout_ms;   /**< Monitor timeout (msecs) */
    uint16_t max_pdu_size;         /**< Maximum PDU payload size */
} wiced_bt_l2cap_fcr_options_t;

/** Define a structure to hold the configuration parameters. Since the
*   parameters are optional, for each parameter there is a boolean to
*   use to signify its presence or absence.
 *  Refer to Volume 3, Part A, section 5.4 of BT Core specification for details
*/
typedef struct
{
    uint16_t        result;                 /**< Only used in confirm messages */
    wiced_bool_t    mtu_present;            /**< TRUE if MTU option present */
    uint16_t        mtu;                    /**< Maximum transmission unit size */
    wiced_bool_t    qos_present;            /**< QoS configuration present */
    wiced_bt_flow_spec_t qos;               /**< QoS configuration */
    wiced_bool_t    flush_timeout_present;  /**< TRUE if flush option present */
    uint16_t        flush_timeout;          /**< Flush timeout value (1 msec increments) */
    wiced_bool_t    fcr_present;            /**< TRUE if Enhanced retransmission & flow control option present */
    wiced_bt_l2cap_fcr_options_t fcr;       /**< Enhanced flow control and retransmission parameters */
    wiced_bool_t    fcs_present;            /**< TRUE if Frame check sequence option present */
    uint8_t         fcs;                    /**< '0' if desire is to bypass FCS, otherwise '1' */
    uint16_t        flags;                  /**< bit 0: 0-no continuation, 1-continuation */
} wiced_bt_l2cap_cfg_information_t;

/**
 * @anchor L2CAP_CH_CFG_MASK
 * @name L2CAP channel configuration
 * @{
 *
 * L2CAP channel configured field bitmap. Used for \ref wiced_bt_l2cap_ch_cfg_bits_t
 */
#define L2CAP_CH_CFG_MASK_MTU           0x0001      /**< MTU channel configuration bit mask */
#define L2CAP_CH_CFG_MASK_QOS           0x0002      /**< QOS channel configuration bit mask */
#define L2CAP_CH_CFG_MASK_FLUSH_TO      0x0004      /**< Flush to channel configuration bit mask */
#define L2CAP_CH_CFG_MASK_FCR           0x0008      /**< FCR channel configuration bit mask */
#define L2CAP_CH_CFG_MASK_FCS           0x0010      /**< FCS channel configuration bit mask */
#define L2CAP_CH_CFG_MASK_EXT_FLOW_SPEC 0x0020      /**< Extended flow specification channel configuration bit mask */
/** @} L2CAP_CH_CFG_MASK */

/** \typedef uint16_t wiced_bt_l2cap_ch_cfg_bits_t
 *  Channel configuration fields in bit map. Refer \ref L2CAP_CH_CFG_MASK "L2CAP channel configuration"
 */
typedef uint16_t wiced_bt_l2cap_ch_cfg_bits_t;

/** @cond DUAL_MODE */

/**
 * @anchor  L2CAP_FCR_MODE
 * @name L2CAP FCR Mode
 * @{
 *
 * L2CAP FCR mode configuring options and allowed modes bit mask.
 * From below, first 3 used as a field \ref wiced_bt_l2cap_fcr_options_t.mode
 * and reamining used as a bit mask field \ref wiced_bt_l2cap_ertm_information_t.allowed_modes
 *
 */
#define L2CAP_FCR_BASIC_MODE        0x00                          /**< Basic mode (no FCR) */
#define L2CAP_FCR_ERTM_MODE         0x03                          /**< ERTM mode */
#define L2CAP_FCR_STREAM_MODE       0x04                          /**< Streaming mode */

#define L2CAP_FCR_CHAN_OPT_BASIC    (1 << L2CAP_FCR_BASIC_MODE)   /**< Basic mode (no FCR) bit mask */
#define L2CAP_FCR_CHAN_OPT_ERTM     (1 << L2CAP_FCR_ERTM_MODE)    /**< ERTM mode bit mask */
#define L2CAP_FCR_CHAN_OPT_STREAM   (1 << L2CAP_FCR_STREAM_MODE)  /**< Streaming mode bit mask */
#define L2CAP_FCR_CHAN_OPT_ALL_MASK (L2CAP_FCR_CHAN_OPT_BASIC | L2CAP_FCR_CHAN_OPT_ERTM | L2CAP_FCR_CHAN_OPT_STREAM) /**< Mask for all modes Basic, ERTM and Streaming */

/** @} L2CAP_FCR_MODE */

/**
 * @anchor  L2CAP_FLUSH_CHANNELS
 * @name L2CAP flush channels
 * @{
 *
 * L2CAP flush channel and num_to_flush parameter used in a call \ref wiced_bt_l2cap_flush_channel()
 */
#define L2CAP_FLUSH_CHANNELS_ALL       0xffff   /**< To flush all queued buffers */
#define L2CAP_FLUSH_CHANNELS_GET       0x0000   /**< To get queued buffers to flush */
/** @} L2CAP_FLUSH_CHANNELS */

/**
 * @anchor  L2CAP_ROLE
 * @name L2CAP role
 * @{
 *
 * L2CAP role and new_role parameter used in a call \ref wiced_bt_l2cap_set_desire_role()
 *
 * @note \ref L2CAP_ROLE_SCATTERNET_ALLOWED bit is used to prevent l2CAP to Automatically perform role switch (for both Incoming and
 * outgoing) ACL connections.
 */
#define L2CAP_ROLE_SLAVE                HCI_ROLE_SLAVE    /**< L2CAP Slave role */
#define L2CAP_ROLE_MASTER               HCI_ROLE_MASTER   /**< L2CAP Master role */
#define L2CAP_ROLE_ALLOW_SWITCH         0x80              /**< set this bit to allow switch at create conn */
#define L2CAP_ROLE_DISALLOW_SWITCH      0x40              /**< set this bit to disallow switch at create conn */
#define L2CAP_ROLE_CHECK_SWITCH         0xC0              /**< To check the switch to allow or disallow */
#define L2CAP_ROLE_SCATTERNET_ALLOWED   0x20              /**< set this bit to allow scatternet */
/** @} L2CAP_NEW_ROLE */

/** Structure that applications use to create or accept
*   connections with enhanced retransmission mode.
*/
typedef struct
{
    uint8_t       preferred_mode;     /**< Preferred mode: ERTM, Streaming, or Basic */
    uint8_t       allowed_modes;      /**< Bitmask for allowed modes.
                                           Refer bit mask values in \ref L2CAP_FCR_MODE "L2CAP FCR mode" */
} wiced_bt_l2cap_ertm_information_t;

/**
 * @anchor L2CAP_PRIORITY
 * @name L2CAP ACL Priority Value
 * @{
 *
 * Values for priority parameter to wiced_bt_l2cap_set_acl_priority() and wiced_bt_l2cap_set_acl_priority_ext().
 */
#define L2CAP_PRIORITY_NORMAL       0           /**< Set ACL priority as normal */
#define L2CAP_PRIORITY_HIGH         1           /**< Set ACL priority as high */
/** @} L2CAP_PRIORITY */

/**
 * @anchor L2CAP_DIRECTION
 * @name L2CAP ACL Priority Direction
 * @{
 *
 * Values for direction parameter to wiced_bt_l2cap_set_acl_priority_ext()
 */

#define L2CAP_DIRECTION_IGNORE              0       /**< Set ACL priority direction as ignore */
#define L2CAP_DIRECTION_DATA_SOURCE         1       /**< Set ACL priority direction as source */
#define L2CAP_DIRECTION_DATA_SINK           2       /**< Set ACL priority direction as sink */
/** @} L2CAP_DIRECTION */

/** \typedef uint8_t wiced_bt_l2cap_chnl_priority_t
 *  Values for priority parameter to wiced_bt_l2cap_set_tx_priority().
 *  Refer \ref L2CAP_CHNL_PRIORITY "L2CAP channel transmission priority"
 */
typedef uint8_t wiced_bt_l2cap_chnl_priority_t;

/**
 * @anchor L2CAP_CHNL_PRIORITY
 * @name L2CAP transmission channel priority
 * @{
 *
 * L2CAP channel transmission priority. Used for \ref wiced_bt_l2cap_chnl_priority_t
 */

#define L2CAP_CHNL_PRIORITY_HIGH    0       /**< Transmission priority as high for a channel (FCR Mode) */
#define L2CAP_CHNL_PRIORITY_MEDIUM  1       /**< Transmission priority as medium for a channel (FCR Mode) */
#define L2CAP_CHNL_PRIORITY_LOW     2       /**< Transmission priority as low for a channel (FCR Mode) */
/** @} L2CAP_CHNL_PRIORITY */
/* @endcond */

/**
 * @anchor  L2CAP_LE_PSM_CHECK
 * @name L2CAP LE PSM
 * @{
 *
 *  Validity check for LE_PSM.
 *  Fixed LE_PSMs are in the range 0x0001 - 0x007F.
 *  Dynamic LE_PSM are in the range 0x0080 - 0x00FF.
 *  The values 0x0000 and 0x0100 - 0xFFFF are reserved.
*/
#define MINIMIUM_DYNAMIC_LE_PSM       0x0080                                                   /**< First application dynamic PSM  allowed */
#define MAXIMUM_LE_PSM                0x00FF                                                   /**< LE PSM range limit */
#define L2C_BLE_INVALID_PSM(le_psm)   (!(le_psm) || (le_psm) > MAXIMUM_LE_PSM)                 /**< Returns true on invalid LE PSM */
#define L2C_BLE_IS_VALID_PSM(le_psm)  (((le_psm) != 0) && ((le_psm) <= MAXIMUM_LE_PSM))        /**< Returns true on valid LE PSM */
/** @} L2CAP_LE_PSM_CHECK */

/*********************************
 *  Callback Functions Prototypes
 *********************************/
/**
 *  @brief  Connection established callback prototype.
 *
 *  @param bd_addr          : BD Address of remote
 *  @param local_cid        : Local CID assigned to the connection
 *  @param peer_mtu         : Peer MTU
 *
 *  @ingroup l2cap_br_edr_callbacks
 *
 *  @return void
*/
typedef void (wiced_bt_l2cap_connected_cback_t) (wiced_bt_device_address_t bd_addr, uint16_t local_cid, uint16_t peer_mtu);

/**
 *  @brief  Disconnect indication callback prototype.
 *
 *  @param local_cid        : Local CID
 *  @param ack              : Boolean whether upper layer should ack this
 *
 *
 *  @return void
*/
typedef void (wiced_bt_l2cap_disconnect_indication_cback_t) (uint16_t local_cid, wiced_bool_t ack);

/**
 *  @brief  Disconnect confirm callback prototype.
 *
 *  @param local_cid        : Local CID
 *  @param result           : Result
 *
 *
 *  @return void
*/
typedef void (wiced_bt_l2cap_disconnect_confirm_cback_t) (uint16_t local_cid, uint16_t result);

/**
 *  @brief  Data received indication callback prototype.
 *
 *  @param local_cid        : Local CID
 *  @param p_buff           : Pointer to the data
 *  @param buf_len          : Length of the data
 *
 *
 *  @return void
*/
typedef void (wiced_bt_l2cap_data_indication_cback_t) (uint16_t local_cid, tDRB *p_drb);

/**
 *  @brief  Transmit complete callback protype. This callback is optional. If
 *  set, L2CAP will call it when packets are sent or flushed. If the
 *  count is 0xFFFF, it means all packets are sent for that CID (eRTM
 *  mode only).
 *
 *  @param local_cid        : Local CID
 *  @param p_buff           : Pointer to the data that was sent
 *
 *
 *  @return void
*/
typedef void (wiced_bt_l2cap_tx_complete_cback_t) (uint16_t local_cid, void *p_data);

/**
 *  Define the structure that applications use to register with
 *  L2CAP. This structure includes callback functions. All functions
 *  MUST be provided.
 *  Additionally, if registering client for dynamic PSM, connect_ind_cb() must
 *  be NULL since dynamic PSMs use this as a flag for "virtual PSM".
 *
 *
*/
typedef struct
{
    wiced_bt_l2cap_connected_cback_t             *connected_cback;              /**< BR/EDR connected event */
    wiced_bt_l2cap_disconnect_indication_cback_t *disconnect_indication_cback;  /**< BR/EDR disconnect indication event */
    wiced_bt_l2cap_disconnect_confirm_cback_t    *disconnect_confirm_cback;     /**< BR/EDR disconnect confirmation event */
    wiced_bt_l2cap_data_indication_cback_t       *data_indication_cback;        /**< BR/EDR data received indication */
    wiced_bt_l2cap_tx_complete_cback_t           *tx_complete_cback;            /**< BR/EDR transmit complete event */

    uint16_t                        mtu;                    /**< Maximum transmission unit size */
    wiced_bool_t                    qos_present;            /**< QoS configuration present */
    wiced_bt_flow_spec_t            qos;                    /**< QoS configuration */
    wiced_bool_t                    flush_timeout_present;  /**< TRUE if flush option present */
    uint16_t                        flush_timeout;          /**< Flush timeout value (1 msec increments) */
    uint8_t                         fcr_allowed_modes;      /**< Set to 0 or L2CAP_FCR_CHAN_OPT_BASIC for no FCR */
    wiced_bt_l2cap_fcr_options_t    fcr;                    /**< Enhanced flow control and retransmission parameters */
    wiced_bool_t                    fcs_present;            /**< TRUE if Frame check sequence option present */
    uint8_t                         fcs;                    /**< '0' if desire is to bypass FCS, otherwise '1' */
    wiced_bool_t                    is_ob_only;             /**< Set to TRUE if registration is for outbound only to a dynamic PSM */
} wiced_bt_l2cap_appl_information_t;

/*
 *
 *                      Fixed Channel callback prototypes
 *
 */

/**
 *  @brief   Fixed channel connected and disconnected.
 *
 *  @param bd_addr          : BD Address of remote
 *  @param connected        : TRUE if channel is connected, FALSE if disconnected
 *  @param reason           : Reason for connection failure
 *  @param transport        : Bluetooth Transport (BR/EDR or LE)
 *
 *
 *  @return void
*/
typedef void (wiced_bt_l2cap_fixed_chnl_cback_t) (wiced_bt_device_address_t bd_addr, wiced_bool_t connected,
                                                  uint16_t reason, wiced_bt_transport_t transport);

/**
 *  @brief  Signalling data received.
 *
 *  @param bd_addr          : BD Address of remote
 *  @param p_buff           : Pointer to data
 *  @param data_len         : Data length
 *
 *
 *  @return void
*/
typedef void (wiced_bt_l2cap_fixed_data_cback_t) (wiced_bt_device_address_t bd_addr, uint8_t *p_data, uint16_t data_len);

/**
 *  @brief    LE Connection indication callback prototype.
 *
 *  @param bd_addr          : BD Address of remote
 *  @param local_cid        : Local CID assigned to the connection
 *  @param psm              : PSM that the remote wants to connect to
 *  @param id               : Identifier that the remote sent
 *  @param mtu_peer         : MTU of the peer
 *
 *
 *  @return void
*/
typedef void (wiced_bt_l2cap_le_connect_indication_cback_t) (wiced_bt_device_address_t bd_addr, uint16_t local_cid,
                                                             uint16_t psm, uint8_t id, uint16_t mtu_peer);

/**
 *  @brief          User DRB may be released callback prototype. This function address is passed
 *                  in when the user provides a DRB for an eRTM or LE-COC channel. It is called
 *                  when the DRB can be released, i.e. at the end of the connection.
 *
 *  @param          p_drb          : Address of the DRB that can be released
 *
 *
 *  @return         void
*/
typedef void (wiced_bt_l2cap_drb_release_cb) (tDRB *p_drb);

/**
 *  @brief LE Connection confirmation callback prototype.
 *
 *  @param local_cid        : Local CID
 *  @param result           : Result - 0 = connected, non-zero means failure reason
 *  @param mtu_peer         : MTU of the peer
 *
 *
 *  @return     void
*/
typedef void (wiced_bt_l2cap_le_connect_confirm_cback_t) (uint16_t local_cid, uint16_t result, uint16_t mtu_peer);

/**
 *  Define the structure that applications use to register with
 *  LE L2CAP. This structure includes callback functions. All functions
 *  MUST be provided, with the exception of the "connect pending" callback.
 *  Additionally, if registering client for dynamic PSM, connect_ind_cb() must
 *  be NULL since dynamic PSMs use this as a flag for "virtual PSM".
 *
 *
*/
typedef struct
{
    wiced_bt_l2cap_le_connect_indication_cback_t  *le_connect_indication_cback; /**< LE connect indication event */
    wiced_bt_l2cap_le_connect_confirm_cback_t     *le_connect_confirm_cback;    /**< LE connect confirm event */
    wiced_bt_l2cap_disconnect_indication_cback_t  *disconnect_indication_cback; /**< LE disconnect indication event */
    wiced_bt_l2cap_disconnect_confirm_cback_t     *disconnect_confirm_cback;    /**< LE disconnect confirm event */
    wiced_bt_l2cap_data_indication_cback_t        *data_indication_cback;       /**< LE data received indication */
    wiced_bt_l2cap_tx_complete_cback_t            *le_tx_complete_cback;        /**< LE tx complete  */
    wiced_bt_l2cap_drb_release_cb                 *le_release_drb_cb;           /**< LE DRB can be released */
} wiced_bt_l2cap_le_appl_information_t;

/**
 *  Fixed channel registration info (the callback addresses and channel config)
*/
typedef struct
{
    uint16_t                                        channel_id;               /**< Fixed channel ID */
    uint8_t                                         in_use;                   /**< TRUE if channel is in use */
    wiced_bt_l2cap_fixed_chnl_cback_t               *fixed_conn_cback;        /**< Connected callback */
    wiced_bt_l2cap_fixed_data_cback_t               *fixed_data_cback;        /**< Data received callback */

    uint16_t                                        default_idle_timeout;     /**< default idle timeout */
    wiced_bt_l2cap_tx_complete_cback_t              *fixed_tx_complete_cback; /**< TX complete callback */
    void                                            *context;                 /**< Upper layer context */
} wiced_bt_l2cap_fixed_chnl_reg_t;

#ifdef __cplusplus
extern "C"
{
#endif

/*****************************************************************************
 *  External Function Declarations
 ****************************************************************************/

/**
 * @if DUAL_MODE
 *  @addtogroup  l2cap_common_api_functions       Common
 *  @ingroup     l2cap
 *   Commonly used API's for both BE/EDR and LE \ref l2cap "L2CAP"
 * @else
 *  @ingroup     l2cap
 * @endif
 *
 * @{
 */

/**
 *  @brief          Register a fixed channel.
 *
 *  @param[in]      fixed_cid   : Fixed Channel #
 *  @param[in]      p_freg      : Channel Callbacks and config
 *
 *  @return         TRUE if registered OK
 */
wiced_bool_t  wiced_bt_l2cap_register_fixed_channel (uint16_t fixed_cid, wiced_bt_l2cap_fixed_chnl_reg_t *p_freg);


/**
 *  @brief          Connect an fixed signalling channel to a remote device.
 *
 *  @param[in]      fixed_cid   : Fixed CID
 *  @param[in]      bd_addr     : BD Address of remote
 *
 *  @return         TRUE if connection started
 */
wiced_bool_t wiced_bt_l2cap_connect_fixed_chnl (uint16_t fixed_cid, wiced_bt_device_address_t bd_addr);


/**
 *  @brief          Write data on a fixed signalling channel.
 *
 *  @param[in]      fixed_cid   : Fixed CID
 *  @param[in]      rem_bda     : BD Address of remote
 *  @param[in]      p_data      : Pointer to data to send
 *  @param[in]      data_len    : Length of data to send
 *
 *  @return         L2CAP_DATAWRITE_SUCCESS, if data accepted
 *                  L2CAP_DATAWRITE_FAILED,  if error
 */
uint16_t wiced_bt_l2cap_send_fixed_chnl_data (uint16_t fixed_cid, wiced_bt_device_address_t rem_bda,
                                              uint8_t *p_data, uint16_t data_len);



/**
 *  @brief          Remove a fixed channel to a remote device.
 *
 *  @param[in]      fixed_cid   : Fixed CID
 *  @param[in]      rem_bda     : BD Address of remote
 *                                Idle timeout to use (or 0xFFFF if don't care)
 *
 *  @return         TRUE if channel removed
 */
wiced_bool_t wiced_bt_l2cap_remove_fixed_chnl (uint16_t fixed_cid, wiced_bt_device_address_t rem_bda);


/**
 *  @brief          Higher layers call this function to set the idle timeout for
 *                  a fixed channel. The "idle timeout" is the amount of time that
 *                  a connection can remain up with no L2CAP channels on it.
 *                  A timeout of zero means that the connection will be torn
 *                  down immediately when the last channel is removed.
 *                  A timeout of 0xFFFF means no timeout. Values are in seconds.
 *                  A bd_addr is the remote BD address. If bd_addr = BT_BD_ANY,
 *                  then the idle timeouts for all active l2cap links will be
 *                  changed.
 *
 *  @param[in]      rem_bda         : Remote BD address
 *  @param[in]      fixed_cid       : Fixed CID
 *  @param[in]      idle_timeout    : Idle timeout
 * 
 *  @return         TRUE if command succeeded, FALSE if failed
 */
wiced_bool_t wiced_bt_l2cap_set_fixed_channel_timeout (wiced_bt_device_address_t rem_bda, uint16_t fixed_cid, uint16_t idle_timeout);


/**
 *  @brief          This function returns configurations of L2CAP channel
 *
 *  @param[in]      lcid              : Local CID
 *  @param[in]      pp_our_cfg        : pointer of our saved configuration options
 *  @param[in]      p_our_cfg_bits    : valid config in bitmap
 *  @param[in]      pp_peer_cfg       : pointer of peer's saved configuration options
 *  @param[in]      p_peer_cfg_bits   : valid config in bitmap
 *
 *  @return         TRUE if successful
 */
wiced_bool_t wiced_bt_l2cap_get_current_config (uint16_t lcid,
                                                wiced_bt_l2cap_cfg_information_t **pp_our_cfg,  wiced_bt_l2cap_ch_cfg_bits_t *p_our_cfg_bits,
                                                wiced_bt_l2cap_cfg_information_t **pp_peer_cfg, wiced_bt_l2cap_ch_cfg_bits_t *p_peer_cfg_bits);


/**
 *  @brief          Other layers call this function to register for L2CAP
 *                  services.
 *
 *  @param[in]      psm                 : PSM value
 *  @param[in]      p_cb_information    : L2CAP cb info
 *
 *  @return         PSM to use or zero if error. Typically, the PSM returned
 *                  is the same as was passed in, but for an outgoing-only
 *                  connection to a dynamic PSM, a "virtual" PSM is returned
 *                  and should be used in the calls to wiced_bt_l2cap_connect_req().
 */
uint16_t wiced_bt_l2cap_register (uint16_t psm, wiced_bt_l2cap_appl_information_t *p_cb_information);


/**
 *  @brief          Other layers call this function to deregister for L2CAP
 *                  services.
 *
 *  @param[in]      psm : PSM value
 *
 *  @return         void
 */
void wiced_bt_l2cap_deregister (uint16_t psm);


/**
 *  @brief          Other layers call this function to find an unused PSM for
 *                  L2CAP services.
 *
 *  @return         PSM to use.
 */
uint16_t wiced_bt_l2cap_allocate_psm (void);

/**
 *  @brief          Higher layers call this function to disconnect a channel.
 *
 *  @param[in]      cid : CID value
 *
 *  @return         TRUE if disconnect sent, else FALSE
 */
wiced_bool_t wiced_bt_l2cap_disconnect_req (uint16_t cid);


/**
 *  @brief           Higher layers call this function to acknowledge the
 *                  disconnection of a channel.
 *
 *  @param[in]      cid : CID value
 *
 *  @return         void
 */
wiced_bool_t wiced_bt_l2cap_disconnect_rsp (uint16_t cid);


/**
 *  @brief          Higher layers call this function to write data with extended
 *
 *  @param[in]      cid     : CID value
 *  @param[in]      p_buf   : Input buffer
 *  @param[in]      buf_len : p_buf buffer size
 *  @param[in]      flags   : refer \ref L2CAP_DATAWRITE_FLUSH "L2CAP data write flags"
 *
 *  @return         refer \ref L2CAP_DATAWRITE "L2CAP data write result"
 */
uint8_t wiced_bt_l2cap_data_write (uint16_t cid, uint8_t *p_buf, uint16_t buf_len, uint16_t flags);

/**
 * @brief           Higher layers call this function to set the idle timeout for
 *                  a connection, or for all future connections. The "idle timeout"
 *                  is the amount of time that a connection can remain up with
 *                  no L2CAP channels on it. A timeout of zero means that the
 *                  connection will be torn down immediately when the last channel
 *                  is removed. A timeout of 0xFFFF means no timeout. Values are
 *                  in seconds.
 *
 *  @param[in]      cid         : CID value
 *  @param[in]      timeout     : Timeout value
 *  @param[in]      is_global   : TRUE, if global
 *
 *  @return         TRUE if command succeeded, FALSE if failed
 */
wiced_bool_t wiced_bt_l2cap_set_idle_timeout (uint16_t cid, uint16_t timeout,
                                              wiced_bool_t is_global);


/**
 *  @brief          Higher layers call this function to set the idle timeout for
 *                  a connection. The "idle timeout" is the amount of time that
 *                  a connection can remain up with no L2CAP channels on it.
 *                  A timeout of zero means that the connection will be torn
 *                  down immediately when the last channel is removed.
 *                  A timeout of 0xFFFF means no timeout. Values are in seconds.
 *                  A bd_addr is the remote BD address. If bd_addr = BT_BD_ANY,
 *                  then the idle timeouts for all active l2cap links will be
 *                  changed.
 *
 *  @param[in]      bd_addr     : BD Address
 *  @param[in]      timeout     : Timeout value
 *  @param[in]      transport   : Transport (BR-EDR or LE)
 *
 *  @return         TRUE if command succeeded, FALSE if failed
 *
 *  @note           This timeout applies to all logical channels active on the
 *                  ACL link.
 */
wiced_bool_t wiced_bt_l2cap_set_idle_timeout_by_bd_addr (wiced_bt_device_address_t bd_addr, uint16_t timeout,
                                                         wiced_bt_transport_t transport);

/**
 *  @brief          This function sets the trace level for L2CAP. If called with
 *                  a value of 0xFF, it simply reads the current trace level.
 *
 *  @param[in]      trace_level: Trace level
 *
 *  @return         the new (current) trace level
 */
uint8_t wiced_bt_l2cap_set_trace_level (uint8_t trace_level);

/**
 *  @brief          Get BD address for the given HCI handle
 *
 *  @param[in]      handle  : HCI handle
 *  @param[in]      bd_addr : Peer Bd Address
 *
 *  @return:        TRUE if found lcb for the given handle, FALSE otherwise
 *
 */
wiced_bool_t wiced_bt_l2cap_get_bdaddrby_handle (uint16_t handle, wiced_bt_device_address_t bd_addr);

/**
 *  @brief          This function returns the disconnect reason code.
 *
 *  @param[in]      remote_bda : Remote BD Address
 *  @param[in]      transport  : Transport (BR-EDR or LE)
 *
 *  @return         disconnect reason
 */
uint16_t wiced_bt_l2cap_get_disconnect_reason (wiced_bt_device_address_t remote_bda, wiced_bt_transport_t transport);

/** @} */

/**
 * @cond DUAL_MODE
 *  @addtogroup  l2cap_br_edr_api_functions       BR/EDR
 *  @ingroup     l2cap
 *
 *  API's used for BR/EDR \ref l2cap "L2CAP".
 *
 * @{
 */

/**
 *  @brief          Higher layers call this function to create an L2CAP connection.
 *                  Note that the connection is not established at this time, but
 *                  connection establishment gets started. The callback function
 *                  will be invoked when connection establishes or fails.
 *
 *  @param[in]      psm                 : PSM value
 *  @param[in]      p_bd_addr           : BD Address
 *
 *  @return         the CID of the connection, or 0 if it failed to start
 */
uint16_t wiced_bt_l2cap_connect_req (uint16_t psm, wiced_bt_device_address_t p_bd_addr);


/**
 * @brief           Enable ERTM.
 *
 *                  Calling this function will cause the linker to include
 *                  ERTM related functions.
 *
 * @return          void
 *
 */
void wiced_bt_l2cap_ertm_enable (void);


/**
 *  @brief          Higher layers call this function to create an L2CAP connection
 *                  that needs to use Enhanced Retransmission Mode.
 *                  Note that the connection is not established at this time, but
 *                  connection establishment gets started. The callback function
 *                  will be invoked when connection establishes or fails.
 *
 *  @param[in]      psm                 : PSM value
 *  @param[in]      p_bd_addr           : BD Address
 *  @param[in]      p_ertm_information  : ERTM info
 *
 *  @return         the CID of the connection, or 0 if it failed to start
 */
uint16_t wiced_bt_l2cap_ertm_connect_req (uint16_t psm, wiced_bt_device_address_t p_bd_addr,
                                          wiced_bt_l2cap_ertm_information_t *p_ertm_information);


/**
 *  @brief         Higher layers call this function to register a DRM for an ERTM connection.
 *
 *  @param[in]     lcid                 : Local CID value
 *  @param[in]     p_drb                : Address of the DRB
 *  @param[in]     drb_max_payload_len  : DRB Size. It should greater than or equal to to MTU.
 *  @param[in]     p_unreg_cb           : \ref wiced_bt_l2cap_drb_release_cb.
 *
 *  @return        TRUE if disconnect sent, else FALSE
 */
wiced_bool_t wiced_bt_l2cap_register_ertm_drb (uint16_t lcid, tDRB *p_drb, uint16_t drb_max_payload_len, wiced_bt_l2cap_drb_release_cb *p_unreg_cb);


/**
 * @brief       This function sets the desire role for L2CAP.
 *              If the new role is L2CAP_ROLE_ALLOW_SWITCH, allow switch on
 *              HciCreateConnection.
 *              If the new role is L2CAP_ROLE_DISALLOW_SWITCH, do not allow switch on
 *              HciCreateConnection.
 *
 *              If the new role is a valid role (HCI_ROLE_MASTER or HCI_ROLE_SLAVE),
 *              the desire role is set to the new value. Otherwise, it is not changed.
 *
 *  @param[in]  new_role    : New role value. Refer \ref L2CAP_ROLE "L2CAP role"
 *
 *  @return     the new (current) role. Refer \ref L2CAP_ROLE "L2CAP role"
 */
uint8_t wiced_bt_l2cap_set_desire_role (uint8_t new_role);

/**
 * @brief       This function flushes none, some or all buffers queued up
 *              for xmission for a particular CID. If called with
 *              L2CAP_FLUSH_CHANNELS_GET (0), it simply returns the number
 *              of buffers queued for that CID L2CAP_FLUSH_CHANNELS_ALL (0xffff)
 *              flushes all buffers.  All other values specifies the maximum
 *              buffers to flush.
 *
 *  @param[in]  lcid            : Local CID value
 *  @param[in]  num_to_flush    : Number of items for flushing, Refer \ref L2CAP_FLUSH_CHANNELS "L2CAP flush channels"
 *
 *  @return     Number of buffers left queued for that CID
 */
uint16_t   wiced_bt_l2cap_flush_channel (uint16_t lcid, uint16_t num_to_flush);


/**
 *  @brief          Sets the priority for an ACL channel
 *
 *  @param[in]      bd_addr     : BD Address
 *  @param[in]      priority    : Refer \ref L2CAP_PRIORITY "L2CAP ACL Priority Value"
 *
 *  @return         TRUE if a valid channel, else FALSE
 */
wiced_bool_t wiced_bt_l2cap_set_acl_priority (wiced_bt_device_address_t bd_addr, uint8_t priority);

/**
 *  @brief          Sets the priority for an ACL channel
 *                  with extended parameters.
 *
 *  @param[in]      bd_addr     : BD Address
 *  @param[in]      priority    : Refer \ref L2CAP_PRIORITY "L2CAP ACL Priority Value"
 *  @param[in]      direction   : Refer \ref L2CAP_DIRECTION "L2CAP ACL Priority Direction"
 *
 *  @return         TRUE if a valid channel, else FALSE
 */
wiced_bool_t wiced_bt_l2cap_set_acl_priority_ext (wiced_bt_device_address_t bd_addr, uint8_t priority, uint8_t direction);


/**
 *  @brief          Higher layers call this function to flow control a channel.
 *
 *                  data_enabled - TRUE data flows, FALSE data is stopped
 *
 *  @param[in]      cid             : CID value
 *  @param[in]      data_enabled    : data enabled
 *
 *  @return         TRUE if valid channel, else FALSE
 */
wiced_bool_t wiced_bt_l2cap_flow_control (uint16_t cid, wiced_bool_t data_enabled);


/**
 *  @brief          Sets the transmission priority for a channel. (FCR Mode)
 *
 *  @param[in]      cid       : CID value
 *  @param[in]      priority  : refer \ref wiced_bt_l2cap_chnl_priority_t

 *
 *  @return         TRUE if a valid channel, else FALSE
 */
wiced_bool_t wiced_bt_l2cap_set_tx_priority (uint16_t cid, wiced_bt_l2cap_chnl_priority_t priority);


/**
 *  @brief          This function set the automatic flush time out in Baseband
 *                  for ACL-U packets.
 *
 *  @param[in]      bd_addr         : The remote BD address of ACL link. If it is BT_DB_ANY
 *                                    then the flush time out will be applied to all ACL link.
 *  @param[in]      flush_timeout   : flush time out in ms
 *                      0x0000                   : No automatic flush
 *                      L2CAP_NO_RETRANSMISSION  : No retransmission
 *                      0x0002 - 0xFFFE          : flush time out, if (flush_timeout*8)+3/5)
 *                                                 <= HCI_MAX_AUTO_FLUSH_TOUT (in 625us slot).
 *                                                 Otherwise, return FALSE.
 *                      L2CAP_NO_AUTOMATIC_FLUSH : No automatic flush
 *
 *  @return         TRUE if command succeeded, FALSE if failed
 *
 *  @note           This flush timeout applies to all logical channels active on the
 *                  ACL link.
 */
wiced_bool_t wiced_bt_l2cap_set_flush_timeout (wiced_bt_device_address_t bd_addr, uint16_t flush_timeout);


/**
 *  @brief         Get a peers features and fixed channel map
 *
 *  @param[in]      bd_addr     : Peer Bd Address
 *  @param[in]      p_ext_feat  : features
 *  @param[in]      p_chnl_mask : mask storage area
 *
 *  @return:        TRUE if peer is connected
 *
 */
wiced_bool_t wiced_bt_l2cap_get_peer_features (wiced_bt_device_address_t bd_addr, uint32_t *p_ext_feat, uint8_t *p_chnl_mask);

/**
 *  @brief          Get the channel FCR mode
 *
 *  @param[in]      lcid: Local CID
 *
 *  @return         Channel mode Refer \ref L2CAP_FCR_MODE
 */
uint8_t wiced_bt_l2cap_get_chnl_fcr_mode (uint16_t lcid);

/**@} l2cap_br_edr_api_functions */
/* @endcond */

/**
 * @if DUAL_MODE
 *  @addtogroup  l2cap_le_api_functions       BLE
 *  @ingroup     l2cap
 *  API's used for LE \ref l2cap "L2CAP".
 * @else
 *  @ingroup     l2cap
 * @endif
 *
 * @{
 */


/**
 *  @brief          Cancel a pending connection attempt to a BLE device.
 *
 *  @param[in]      rem_bda : BD Address of remote
 *
 *  @return:        TRUE if connection was cancelled
 */
wiced_bool_t wiced_bt_l2cap_cancel_ble_connect_req (wiced_bt_device_address_t rem_bda);


/**
 *  @brief          Update BLE connection parameters.
 *
 *  @param[in]      rem_bdRa    : Remote BD Address
 *  @param[in]      min_int     : Min interval
 *  @param[in]      max_int     : Max interval
 *  @param[in]      latency     : Latency value
 *  @param[in]      timeout     : Timeout value
 *
 *  @return:        TRUE if update started
 */
wiced_bool_t wiced_bt_l2cap_update_ble_conn_params (wiced_bt_device_address_t rem_bdRa, uint16_t min_int, uint16_t max_int, uint16_t latency, uint16_t timeout);


/**
 *  @brief          Update BLE connection parameters.
 *
 *  @param[in]      rem_bda: Remote Bd Address
 *  @param[in]      enable: Enable Flag
 *
 *  @return:        TRUE if update started
 *
 */
wiced_bool_t wiced_bt_l2cap_enable_update_ble_conn_params (wiced_bt_device_address_t rem_bda, wiced_bool_t enable);


/**
 *  @brief          This function returns the connection role.
 *
 *  @param[in]      bd_addr: BD Address
 *
 *  @return         link role.( 0 => HCI_ROLE_MASTER and 1 => HCI_ROLE_SLAVE)
 */
uint8_t wiced_bt_l2cap_get_ble_conn_role (wiced_bt_device_address_t bd_addr);

/**
 *  @brief          Other layers call this function to register L2CAP services
 *                  for LE_PSM.
 *
 *  @param[in]      le_psm              : LE PSM value
 *  @param[in]      p_cb_information    : L2CAP cb info
 *
 *  @return         LE_PSM to use or zero if error. Typically the LE_PSM returned
 *                  is the same as was passed in, but for an outgoing-only
 *                  connection a "virtual" LE_PSM is returned  and should be used
 *                  in the calls to wiced_bt_l2cap_le_connect_req() and wiced_bt_l2cap_le_deregister().
 */
uint16_t wiced_bt_l2cap_le_register (uint16_t le_psm, wiced_bt_l2cap_le_appl_information_t *p_cb_information);


/**
 *  @brief          Other layers call this function to deregister L2CAP services
 *                  for LE_PSM.
 *
 *  @param[in]      le_psm: LE PSM value
 *
 *  @return         TRUE for success, FALSE for failure
 */
wiced_bool_t wiced_bt_l2cap_le_deregister (uint16_t le_psm);


/**
 *  @brief          Higher layers call this function to create an L2CAP connection
 *                  for LE_PSM.
 *                  Note that the connection is not established at this time, but
 *                  connection establishment gets started. The callback function
 *                  will be invoked when connection establishes or fails.
 *
 *  @param[in]      le_psm              : LE PSM value
 *  @param[in]      p_bd_addr           : BD Address
 *  @param[in]      bd_addr_type        : BLE_ADDR_PUBLIC or BLE_ADDR_RANDOM
 *  @param[in]      conn_mode           : BLE_CONN_MODE_HIGH_DUTY or BLE_CONN_MODE_LOW_DUTY
 *  @param[in]      rx_mtu              : Rx MTU value (must be <= ACL_POOL_SIZE)
 *  @param[in]      req_security        : Security required
 *  @param[in]      req_encr_key_size   : key size
 *  @param[in]      p_rx_drb            : a DRB to receive peer's data. MUST be large enough to hold RX MTU data
 *
 *  @return         the CID of the connection, or 0 if it failed to start
 */
uint16_t wiced_bt_l2cap_le_connect_req (uint16_t le_psm, wiced_bt_device_address_t p_bd_addr,
                                        wiced_bt_ble_address_type_t bd_addr_type,
                                        wiced_bt_ble_conn_mode_t conn_mode,
                                        uint16_t rx_mtu,  uint8_t req_security, uint8_t req_encr_key_size, tDRB *p_rx_drb);


/**
 *  @brief          Higher layers call this function to accept an incoming
 *                  LE L2CAP connection, for which they had gotten an connect
 *                  indication callback.
 *
 *  @param[in]      p_bd_addr   : BD Address
 *  @param[in]      id          : ID received from wiced_bt_l2cap_le_connect_indication_cback_t callback.
 *  @param[in]      lcid        : Local CID
 *  @param[in]      result      : L2CAP result codes (\ref L2CAP_CONN_RESULT)
 *  @param[in]      rx_mtu      : Rx MTU value (must be <= ACL_POOL_SIZE)
 *  @param[in]      p_rx_drb    : a DRB to receive peer's data. MUST be large enough to hold RX MTU data
 *
 *  @return         TRUE for success, FALSE for failure
 */
wiced_bool_t  wiced_bt_l2cap_le_connect_rsp (wiced_bt_device_address_t p_bd_addr, uint8_t id, uint16_t lcid,
                                             uint16_t result, uint16_t rx_mtu, tDRB *p_rx_drb);


/**
 *  @brief          Higher layers call this function to disconnect a LE COC
 *                  channel.
 *
 *  @param[in]      lcid: Local CID value
 *
 *  @return         TRUE if disconnect sent, else FALSE
 *
 */
wiced_bool_t wiced_bt_l2cap_le_disconnect_req (uint16_t lcid);


/**
 *  @brief          Higher layers call this function to acknowledge the
 *                  disconnection of a LE COC channel.
 *
 *  @param[in]      lcid: Local CID value
 *
 *  @return         void
 */
wiced_bool_t wiced_bt_l2cap_le_disconnect_rsp (uint16_t lcid);


/**
 *  @brief          Send data over LE connection-oriented channel.
 *
 *  @param[in]      cid     : CID value
 *  @param[in]      p_data  : Input buffer
 *  @param[in]      buf_len : p_data buffer size
 *
 *  @return         L2CAP_DATAWRITE_SUCCESS, if data accepted, else FALSE
 *                  L2CAP_DATAWRITE_CONGESTED, if data accepted and the channel is congested
 *                  L2CAP_DATAWRITE_FAILED, if error
 */
uint8_t wiced_bt_l2cap_le_data_write (uint16_t cid, uint8_t *p_data, uint16_t buf_len);


/**
 *  @brief          Higher layers call this function to tell if the connection
 *                  is congested or not
 *
 *  @param[in]      lcid            : Local CID value
 *  @param[in]      is_congested    : TRUE, if congested
 *
 *  @return         TRUE if command processed OK
 */
wiced_bool_t  wiced_bt_l2cap_le_set_user_congestion (uint16_t lcid, wiced_bool_t is_congested);


/**
 *  @brief          Higher layers call this function to get peer MTU.
 *
 *  @param[in]      lcid    : Local CID value
 *
 *  @return         Peer MTU or 0.
 */
uint16_t wiced_bt_l2cap_le_get_peer_mtu (uint16_t lcid);


/**
 *  @brief          Higher layers call this function to check if the current
 *                  device security settings are sufficient to continue with
 *                  call establishment.
 *                  It is called by call acceptor on reception of LE Credit
 *                  Based Connection Request.
 *
 *  @param[in]      bd_addr           : BD Address
 *  @param[in]      req_secur         : Security required
 *  @param[in]      req_encr_key_size : Key size
 *
 *  @return         L2CAP_CONN_OK/L2CAP_BLE_CONN_BAD_AUTHENT/
 *                  L2CAP_BLE_CONN_BAD_KEY_SIZE/L2CAP_BLE_CONN_BAD_ENCRYPT/
 *                  L2CAP_CONN_NO_RESOURCES.
 */
uint16_t wiced_bt_l2cap_le_determ_secur_rsp (wiced_bt_device_address_t bd_addr, uint8_t req_secur, uint8_t req_encr_key_size);
/**@} l2cap_le_api_functions */
/**@} l2cap*/


#ifdef __cplusplus
}
#endif
