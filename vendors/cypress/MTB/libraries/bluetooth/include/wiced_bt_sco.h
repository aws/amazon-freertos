/*
 * Copyright 2015, Broadcom Corporation
 * All Rights Reserved.
 *
 * This is UNPUBLISHED PROPRIETARY SOURCE CODE of Broadcom Corporation;
 * the contents of this file may not be disclosed to third parties, copied
 * or duplicated in any form, in whole or in part, without the prior
 * written permission of Broadcom Corporation.
 */

/** @file
 *
 * Bluetooth Synchronous Connection Oriented Channel Application Programming Interface
 *
 */
#pragma once


/******************************************************
 *              Constants
 ******************************************************/

#define WICED_BT_SCO_CONNECTION_ACCEPT              0x00
#define WICED_BT_SCO_CONNECTION_REJECT_RESOURCES    0x0D
#define WICED_BT_SCO_CONNECTION_REJECT_SECURITY     0x0E
#define WICED_BT_SCO_CONNECTION_REJECT_DEVICE       0x0F

/******************************************************
 *              Type Definitions
 ******************************************************/
/*SCO codec IDs*/
enum wiced_bt_sco_esco_codec_setting_id_e
{
    WICED_BT_SCO_ESCO_SETTING_ID_CVSD  =  0,    /**< eSCO setting for CVSD */
    WICED_BT_SCO_ESCO_SETTING_ID_MSBC_T2        /**< eSCO setting for mSBC T2 */
};
typedef uint8_t wiced_bt_sco_esco_codec_setting_id_t;   /**< eSCO Codec Settings ID (See #wiced_bt_sco_esco_codec_setting_id_e) */

/* Subset for the enhanced setup/accept synchronous connection paramters
 * See BT 4.1 or later HCI spec for details */
typedef struct
{
    uint16_t max_latency;                   /**< Maximum latency (0x4-0xFFFE in msecs) */
    uint16_t packet_types;                  /**< Packet Types */
    uint8_t retrans_effort;                 /**< 0x00-0x02, 0xFF don't care */
    wiced_bool_t use_wbs;                   /**< True to use wide band, False to use narrow band */

} wiced_bt_sco_params_t;

enum wiced_bt_sco_data_packet_status_e
{
    WICED_BT_SCO_DATA_CORRECT   =  0,   /**< Correctly received SCO data      */
    WICED_BT_SCO_DATA_PARTIAL_ERROR,    /**< SCO data with possible errors    */
    WICED_BT_SCO_DATA_NONE,             /**< No SCO data received             */
    WICED_BT_SCO_DATA_PARTIAL_LOST      /**< SCO data partially lost          */
};
typedef uint8_t wiced_bt_sco_data_packet_status_t;  /**< SCO Data Packet Status (See #wiced_bt_sco_data_packet_status_e) */

/**
 * Callback wiced_bt_sco_data_cback_t
 *
 * SCO data packet callback (registered with  #wiced_bt_sco_set_data_callback)
 *
 * @param  sco_index         : SCO Index
 * @param  sco_data          : pointer to BT_HDR struct containing SCO data.
 * @param  status            : one of the following values -
 *                             0 = WICED_BT_SCO_DATA_CORRECT,
 *                             1 = WICED_BT_SCO_DATA_PARTIAL_ERROR,
 *                             2 = WICED_BT_SCO_DATA_NONE,
 *                             3 = WICED_BT_SCO_DATA_PARTIAL_LOST
 *
 * @return
 */
typedef void (wiced_bt_sco_data_cback_t) ( uint16_t sco_index, BT_HDR *sco_data, wiced_bt_sco_data_packet_status_t status );

/**
 *  @addtogroup  sco_api_functions       Synchronous Connection Oriented (SCO) Channel
 *  @ingroup     wicedbt
 *
 * The SCO logical transport is a point-to-point transport between the master and a specific slave.
 * The SCO logical transport reserves slots and can therefore be considered as a circuit-switched
 * connection between the master and the slave. SCO packets are never retransmitted.
 * SCO link is mainly used for Hands Free Audio. (HFP)
 *
 *  @{
 */

/******************************************************
 *              Function Declarations
 ******************************************************/
#ifdef __cplusplus
extern "C"
{
#endif

/**
 * Function         wiced_bt_sco_create_as_initiator
 *
 *                  Creates a synchronous connection oriented connection as initiator.
 *
 *  @param[in]  bd_addr                             : Peer bd_addr
 *  @param[out] p_sco_index                         : SCO index
 *  @param[in]  esco_set_id                         : see wiced_bt_sco_esco_codec_setting_id_t
 *
 *  @return     <b> WICED_BT_UNKNOWN_ADDR </b>      : Create connection failed, ACL connection is not up
 *              <b> WICED_BT_BUSY </b>              : Create connection failed, another SCO is being
 *                                                    conncted to the same BD address
 *              <b> WICED_BT_WRONG_MODE </b>        : Create connection failed, wrong mode
 *              <b> WICED_BT_NO_RESOURCES </b>      : Create connection failed, max SCO limit has been
 *                                                    reached
 *              <b> WICED_BT_PENDING </b>            : Create connection successfully, "p_sco_index" is returned
 */
wiced_bt_dev_status_t wiced_bt_sco_create_as_initiator (wiced_bt_device_address_t bd_addr,
                                                                        uint16_t *p_sco_index,
                                                                        wiced_bt_sco_esco_codec_setting_id_t esco_set_id);

/**
 * Function         wiced_bt_sco_create_as_acceptor
 *
 *                  Creates a synchronous connection oriented connection as acceptor.
 *
 *  @param[out] p_sco_index             : SCO index returned
 *
 *  @return     <b> WICED_BT_UNKNOWN_ADDR </b>      : Create connection failed, ACL connection is not up or
 *                                                    address is invalid
 *              <b> WICED_BT_BUSY </b>              : Create connection failed, a SCO connection is already
 *                                                    conncted to the same BD address
 *              <b> WICED_BT_WRONG_MODE </b>        : Create connection failed, link in park mode or
 *                                                    automatic un-park is not supported
 *              <b> WICED_BT_NO_RESOURCES </b>      : Create connection failed, max SCO limit has been
 *                                                    reached
 *              <b> WICED_BT_PENDING </b>            : Create connection successfully, "p_sco_index" is returned
 */
wiced_bt_dev_status_t wiced_bt_sco_create_as_acceptor (uint16_t *p_sco_index);

/**
 * Function         wiced_bt_sco_remove
 *
 *                  Removes a specific synchronous connection oriented connection.
 *
 *  @param[in]  sco_index           : SCO index to remove
 *
 *  @return     <b> WICED_BT_UNKNOWN_ADDR </b>      : Remove connection failed, invalid SCO index
 *              <b> WICED_BT_NO_RESOURCES </b>      : Remove connection failed, no resource
 *              <b> WICED_BT_SUCCESS </b>           : Remove connection successfully, device is still
 *                                                    listening for incoming connection
 *              <b> WICED_BT_PENDING </b>            : Remove connection successfully
 */
wiced_bt_dev_status_t wiced_bt_sco_remove (uint16_t sco_index);

/**
 * Function         wiced_bt_sco_accept_connection
 *
 *                  Called to handle (e)SCO connection request event (wiced_bt_sco_connect_request_event).
 *
 *
 *  @param[in]  sco_index           : SCO index to remove
 *
 *  @param[in]  HCI status code     : WICED_BT_SCO_CONNECTION_ACCEPT              0x00
 *                                    WICED_BT_SCO_CONNECTION_REJECT_RESOURCES    0x0D
 *                                    WICED_BT_SCO_CONNECTION_REJECT_SECURITY     0x0E
 *                                    WICED_BT_SCO_CONNECTION_REJECT_DEVICE       0x0F
 *  @param[in]  esco_set_id         : see wiced_bt_sco_esco_codec_setting_id_t
 *
 *  @return     WICED_BT_SUCCESS           if successful .
 *              WICED_BT_ILLEGAL_VALUE     invalid esco set ID.
 */
wiced_bt_dev_status_t wiced_bt_sco_accept_connection (uint16_t sco_index, uint8_t hci_status,
                                     wiced_bt_sco_esco_codec_setting_id_t esco_set_id);

/**
 * Function         wiced_bt_sco_set_data_callback
 *
 *                  App must call this API to register a data callback function.
 *                  The stack calls this callback function when there are incoming SCO packets.
 *
 *
 *  @param[in]  p_cback             : function pointer to the callabck function
 *                                    Stack calls this function to deliver incoming SCO
 *                                    packets to app.
 *
 *    @return   WICED_BT_SUCCESS           if the successful (PCM or Test mode).
 *              WICED_BT_NO_RESOURCES      no resources to start the command.
 *              WICED_BT_ILLEGAL_VALUE     invalid callback function pointer.
 *              WICED_BT_PENDING           Command sent. Waiting for command cmpl event.
 *
 * @note            if the call back is NULL or if the application doesn't call this function
 *                  to register a callback, and if the SCO route is SCO_OVER_HCI,
 *                  the received data will be discarded.
 */
wiced_bt_dev_status_t wiced_bt_sco_set_data_callback ( wiced_bt_sco_data_cback_t *p_cback );


/**
 * Function         wiced_bt_sco_write_data
 *
 *                  Called to send data over (e)SCO.
 *
 *
 *  @param[in]  sco_index           : SCO index.
 *  @param[in]  sco_data            : pointer to the data to be sent over SCO.
 *  @param[in]  data_length         : length of the data
 *
 *  @return     WICED_BT_SUCCESS           data write is successful
 *              WICED_BT_SCO_BAD_LENGTH    SCO data length exceeds the max SCO packet
 *                                         size.
 *              WICED_BT_NO_RESOURCES      no resources.
 *              WICED_BT_UNKNOWN_ADDR      unknown SCO connection handle, or SCO is not
 *                                         routed via HCI.
 */

wiced_bt_dev_status_t wiced_bt_sco_write_data (uint16_t sco_index, uint8_t* sco_data,
                                                                    uint16_t data_length);

/**
 * Function         wiced_bt_sco_set_buffer_pool
 *
 *                  If buffer_size > 0 and wiced_bt_sco does not have a private pool yet,
 *                  this function allocates a private GKI buffer pool.
 *                  If buffer_size = 0 and wiced_bt_sco has a private pool,
 *                  this function deallocates the private GKI buffer pool.
 *
 *  @param[in]  buffer_size         : buffer size for the private pool.
 *  @param[in]  buffer_count        : number of buffers in this pool.
 *
 *  @return     WICED_BT_SUCCESS           if the action was carried out successfully as desired
 *              WICED_BT_NO_RESOURCES      no resources.
 */
wiced_bt_dev_status_t wiced_bt_sco_set_buffer_pool(uint16_t buffer_size,
                            uint16_t buffer_count);

/**
 * Function         wiced_bt_sco_get_buffer_pool
 *
 *                  Userial calls this function to find the private GKI buffer pool for SCO over HCI.
 *
 *
 *  @return     the private pool.
 */
void * wiced_bt_sco_get_buffer_pool(void);

#ifdef __cplusplus
}
#endif
/** @} wicedbt_sco */
