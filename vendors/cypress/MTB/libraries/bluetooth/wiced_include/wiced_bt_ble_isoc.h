/*
 * $ Copyright Cypress Semiconductor $
 */

/** @file
 *
 * WICED Bluetooth Low Energy (BLE) Ischoronous function
 *
 *  Some Acronyms and Abbreviations :
 *      CIS   -  Connected Isochronous Stream
 *      CIG   -  Connected Isochronous Group
 *      BIS   -  Broadcast Isochronous Stream
 *      BIG   -  Broadcast Isochronous Group
 *      ISO   -  Isochronous
 *      ISOAL -  Isochronous Adaption Layer
 */
#pragma once
#include "wiced_bt_types.h"

/**
 * @defgroup  wicedbt_isoc        Ischoronous (ISOC)
 *
 * This section describes the API's to use ISOC functionality.
 *
*/

/**
 * @addtogroup  wicedbt_isoc_defs         ISOC data types and macros
 * @ingroup     wicedbt_isoc
 *
 * @{
*/
/*****************************************************************************
**  constants
*****************************************************************************/

#define WICED_BLE_ISOC_MIN_TRANSPORT_LATENCY      0x0005     /**< ISOC Minimum Latency */
#define WICED_BLE_ISOC_MAX_TRANSPORT_LATENCY      0x0FA0     /**< ISOC Maximum Latency */

/** ISOC packing methods */
enum wiced_bt_ble_isoc_packing_e
{
    WICED_BLE_ISOC_SEQUENTIAL_PACKING = 0,
    WICED_BLE_ISOC_INTERLEAVED_PACKING = 1
};
typedef uint8_t wiced_bt_ble_isoc_packing_t; /**< ISOC packing methods (see #wiced_bt_ble_isoc_packing_e) */

/** ISOC Framing types */
enum wiced_bt_ble_isoc_framing_e
{
    WICED_BLE_ISOC_UNFRAMED = 0,
    WICED_BLE_ISOC_FRAMED   = 1
};
typedef uint8_t wiced_bt_ble_isoc_framing_t; /**< ISOC Framing types (see #wiced_bt_ble_isoc_framing_e) */

/** ISOC LE PHY */
enum wiced_bt_ble_isoc_phy_e
{
    WICED_BLE_ISOC_LE_1M_PHY = 0,
    WICED_BLE_ISOC_LE_2M_PHY = 1,
    WICED_BLE_ISOC_LE_CODED  = 2,
};
typedef uint8_t wiced_bt_ble_isoc_phy_t; /**< ISOC LE PHY (see #wiced_bt_ble_isoc_phy_e) */

/** ISOC Events */
enum wiced_bt_ble_isoc_event_e
{
    WICED_BLE_ISOC_CIG_CMD_RSP,             /**< CIG Command Response */
    WICED_BLE_ISOC_CIS_REQUEST,             /**< CIS connection Requested */
    WICED_BLE_ISOC_CIS_ESTABLISHED,         /**< CIS connection established */
    WICED_BLE_ISOC_SLAVE_CLOCK_ACCURACY,    /**< Slave Clock Accuracy */
    WICED_BLE_ISOC_CIS_DISCONNECTED,        /**< CIS disconnected */
    WICED_BLE_ISOC_BIG_CREATED,             /**< BIG Connected */
    WICED_BLE_ISOC_BIG_SYNC_ESTABLISHED,    /**< BIG Sync Established */
    WICED_BLE_ISOC_BIG_TERMINATED,          /**< BIG Terminated */
    WICED_BLE_ISOC_BIG_SYNC_LOST            /**< BIG Sync Lost */
};
typedef uint8_t wiced_bt_ble_isoc_event_t; /**< ISOC Events (see #wiced_bt_ble_isoc_event_e) */

/** ISOC CIS Request Event Data */
typedef struct
{
    uint16_t    cis_conn_handle;        /**< CIS Connection Handle */
    uint8_t     cig_id;                 /**< CIG ID */
    uint8_t     cis_id;                 /**< CIS ID */
}wiced_ble_isoc_cis_request_data_t;

/** ISOC CIS Disconnect Event Data */
typedef struct
{
    uint16_t    cis_conn_handle;        /**< CIS Connection Handle */
    uint8_t     cis_id;                 /**< CIS ID */
}wiced_ble_isoc_cis_disconnect_data_t;

/** ISOC CIS Established Event Data */
typedef struct
{
    uint8_t                     status;                 /**< CIG Establishment Status */
    uint16_t                    cis_conn_handle;        /**< CIS Connection Handle */
    uint32_t                    cig_sync_delay;         /**< CIG Sync Delay in microseconds */
    uint32_t                    cis_sync_delay;         /**< CIS Sync Delay in microseconds */
    uint32_t                    latency_m_to_s;         /**< Maximum time, in microseconds, for an SDU to be transported from the master Controller to slave Controller */
    uint32_t                    latency_s_to_m;         /**< Maximum time, in microseconds, for an SDU to be transported from the slave Controller to master Controller */
    wiced_bt_ble_isoc_phy_t     phy_m_to_s;             /**< The transmitter PHY of packets from the master */
    wiced_bt_ble_isoc_phy_t     phy_s_to_m;             /**< The transmitter PHY of packets from the slave */
    uint8_t                     nse;                    /**< Maximum Number of Subevent in each isochronous event */
    uint8_t                     bn_m_to_s;              /**< Burst number for master to slave transmission */
    uint8_t                     bn_s_to_m;              /**< Burst number for slave to master transmission */
    uint8_t                     ft_m_to_s;              /**< Flush timeout, in multiples of the ISO_Interval for master to slave transmission */
    uint8_t                     ft_s_to_m;              /**< Flush timeout, in multiples of the ISO_Interval for slave to master transmission */
    uint16_t                    max_pdu_m_to_s;         /**< Maximum size, in bytes, of an SDU from the master’s Host */
    uint16_t                    max_pdu_s_to_m;         /**< Maximum size, in octets, of an SDU from the slave’s Host */
    uint16_t                    iso_interval;           /**< Time between two consecutive CIS anchor points */
}wiced_ble_isoc_cis_established_data_t;

/** ISOC CIG Command Status data */
typedef struct
{
    uint8_t        status;                       /**< CIG Establishment Status */
    uint8_t        cig_id;                       /**< CIG ID */
    uint8_t        cis_count;                    /**< CIS Count */
    uint8_t        *cis_id_list;                 /**< CIS ID List */ 
    uint16_t       *cis_connection_handle_list;  /**< CIS Connection Handle List */ 
}wiced_ble_isoc_cig_status_data_t;

/** ISOC Peer Slave Clock Accuracy data */
typedef struct
{
    uint8_t        status;                       /**< SCA Status */
    wiced_bt_device_address_t peer_bda;          /**< Peer Bluetooth Address */
    uint8_t        sca;                          /**< Slave Clock Accuracy value */
}wiced_ble_isoc_sca_t;

/** ISOC Event Data */
typedef union
{
    wiced_ble_isoc_cig_status_data_t        cig_status_data;        /**< CIG Command Status */
    wiced_ble_isoc_cis_established_data_t   cis_established_data;   /**< CIS Established */
    wiced_ble_isoc_cis_request_data_t       cis_request;            /**< CIS Request     */
    wiced_ble_isoc_sca_t                    sca_data;               /**< Slave Clock Accuracy */
    wiced_ble_isoc_cis_disconnect_data_t    cis_disconnect;         /**< CIS Disconnect  */
}wiced_bt_ble_isoc_event_data_t;

/** ISOC CIS Configuration */
typedef struct 
{
    uint8_t                     cis_id;           /**< CIS Id : ZERO if not created*/
    uint16_t                    max_sdu_m_to_s;   /**< Maximum size, in bytes, of an SDU from the master’s Host 
                                                       Valid Range 0x000 to 0xFFF*/
    uint16_t                    max_sdu_s_to_m;   /**< Maximum size, in octets, of an SDU from the slave’s Host 
                                                       Valid Range 0x000 to 0xFFF*/
    wiced_bt_ble_isoc_phy_t     phy_m_to_s;       /**< The transmitter PHY of packets from the master */
    wiced_bt_ble_isoc_phy_t     phy_s_to_m;       /**< The transmitter PHY of packets from the slave */
    uint8_t                     rtn_m_to_s;       /**< Maximum number of times every CIS Data PDU should be retransmitted from the master to slave */
    uint8_t                     rtn_s_to_m;       /**< Maximum number of times every CIS Data PDU should be retransmitted from the slave to master */
}wiced_bt_ble_cis_config_t;

/** ISOC CIG Configuration */
typedef struct
{
    uint8_t                     cig_id;                     /**< CIG ID if known */
    uint32_t                    sdu_interval_m_to_s;        /**< Time interval in microseconds between the start of consecutive SDUs from the master’s Host for all the CISes in the CIG */
    uint32_t                    sdu_interval_s_to_m;        /**< Time interval in microseconds between the start of consecutive SDUs from the slave’s Host for all the CISes in the CIG. */
    uint8_t                     slave_clock_accuracy;       /**< Slave Clock Accuracy */
    uint32_t                    max_trans_latency_m_to_s;   /**< Maximum time, in microseconds, for an SDU to be transported from the master Controller to slave Controller */
    uint32_t                    max_trans_latency_s_to_m;   /**< Maximum time, in microseconds, for an SDU to be transported from the slave Controller to master Controller */
    wiced_bt_ble_isoc_packing_t packing;                    /**< Packing method  */
    wiced_bt_ble_isoc_framing_t framing;                    /**< Framing parameter */
    uint8_t                     cis_count;                  /**< Total number of CISes in the CIG being added or modified 
                                                                  Valid Range 0x00 to 0x10 */
    wiced_bt_ble_cis_config_t   *cis_config_list;           /**< CIS configurations */
}wiced_bt_ble_cig_param_t;

/******************************************************
 *               Function Declarations
 *
 ******************************************************/
#ifdef __cplusplus
extern "C" {
#endif


/**
 * ISOC event callback
 *
 * Callback for ISOC event notifications
 * Registered using #wiced_ble_isoc_register_cb
 *
 * @param event             : Event ID
 * @param p_event_data      : Event data
 *
 * @return Status of event handling
*/
typedef void wiced_ble_isoc_cback_t(wiced_bt_ble_isoc_event_t event, wiced_bt_ble_isoc_event_data_t *p_event_data);

/** @} wicedbt_isoc_defs         */

/**
 * @addtogroup  wicedbt_isoc_functions   Ischoronous (ISOC) functions
 * @ingroup     wicedbt_isoc
 *
 * Ischoronous(ISOC) Functions.
 *
 * @{
 */

/**
 *
 * Function         wiced_ble_isoc_register_cb
 *
 *                  ISOC Register event callback handler
 *
 * @param[in]       isoc_cb  : ISOC event callback
 *
 * @return      None
 *
 */
void wiced_ble_isoc_register_cb(wiced_ble_isoc_cback_t isoc_cb);

/**
 *
 * Function         wiced_ble_isoc_set_cig_param
 *
 *                  Set CIG(Connected Isochronous Group) parameter
 *
 * @param[in]       cig_params  : CIG parameter
 *
 * @return      status
 *
 */
wiced_result_t wiced_ble_isoc_set_cig_param(wiced_bt_ble_cig_param_t *cig_params);

/**
 *
 * Function         wiced_ble_isoc_create_cis
 *
 *                  This API is invoked to create one or more CIS channels
 *
 * @param[in]       cis_count        : Number of elements in CIS handle list and ACL handle list. 
 *                                     These lists must be of the same length
 * @param[in]       cis_handle_list  : List of connection handles of the cis to be established
 * @param[in]       acl_handle_list  : ACL connection handle associated with the cis handles.
 *                                     The ith acl handle corresponds to the ith cis handle, etc.
 *
 * @return      status
 *
 *  Note : Once CIS is establish WICED_BLE_ISOC_CIS_ESTABLISHED event will be received in registered application callback.
 */
wiced_result_t wiced_ble_isoc_create_cis(uint8_t cis_count, uint16_t *cis_handle_list, uint16_t *acl_handle_list);

/**
 *
 * Function         wiced_ble_isoc_create_cis_by_bda
 *
 *                  Create CIS (Connected Isochronous Stream) connection
 *
 * @param[in]       peer_bda        : Peer Bluetooth Address
 *
 * @return      status
 *
 *  Note : Once CIS is establish WICED_BLE_ISOC_CIS_ESTABLISHED event will be received in registered application callback.
 */
wiced_result_t wiced_ble_isoc_create_cis_by_bda(wiced_bt_device_address_t peer_bda);

/**
 *
 * Function         wiced_ble_isoc_accept_cis
 *
 *                  Accept CIS (Connected Isochronous Stream) connection request
 *                  Slave should call this API on receiving WICED_BLE_ISOC_CIS_REQUEST event in registered application callback
 *
 * @param[in]       cis_handle  : CIS handle
 *
 * @return      status
 *
 *  Note : Once CIS is establish WICED_BLE_ISOC_CIS_ESTABLISHED event will be received in registered application callback.
 */
wiced_result_t wiced_ble_isoc_accept_cis(uint16_t cis_handle);

/**
 *
 * Function         wiced_ble_isoc_reject_cis
 *
 *                  Reject CIS (Connected Isochronous Stream) connection request
 *                  Slave should call this API on receiving WICED_BLE_ISOC_CIS_REQUEST event in registered application callback
 *
 * @param[in]       cis_handle  : CIS handle
 * @param[in]       reason      : Reject Reason
 *
 * @return      status
 *
 */
wiced_result_t wiced_ble_isoc_reject_cis(uint16_t cis_handle, uint8_t reason);

/**
 *
 * Function         wiced_ble_isoc_disconnect_cis
 *
 *                  Disconnect CIS (Connected Isochronous Stream) connection
 *
 * @param[in]       cis_handle  : CIS handle
 *
 * @return      status
 *
 *  Note : Once CIS is disconnected WICED_BLE_ISOC_CIS_DISCONNECTED event will be received in registered application callback.
 */
wiced_result_t wiced_ble_isoc_disconnect_cis(uint16_t cis_handle);

/**
 *
 * Function         wiced_ble_isoc_request_peer_sca
 *
 *                  Request for Slave Clock Accuracy
 *
 * @param[in]       peer_bda  : Peer Bluetooth Address
 *
 * @return      status
 *
 *  Note : WICED_BLE_ISOC_SLAVE_CLOCK_ACCURACY event will be received in registered application callback.
 */
wiced_result_t wiced_ble_isoc_request_peer_sca(wiced_bt_device_address_t peer_bda);

/**
 *
 * Function         wiced_ble_isoc_remove_cig
 *
 *                  Remove given CIG
 *
 * @param[in]       cig_id  : CIG ID
 *
 * @return      status
 *
 */
wiced_result_t wiced_ble_isoc_remove_cig(uint8_t cig_id);
/**@} wicedbt_isoc_functions */

#ifdef __cplusplus
}
#endif


