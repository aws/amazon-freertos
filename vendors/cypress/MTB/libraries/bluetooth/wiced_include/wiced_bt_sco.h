/*
 * $ Copyright Cypress Semiconductor $
 */

/** @file
 *
 * Bluetooth Synchronous Connection Oriented Channel Application Programming Interface
 *
 */
#pragma once

#include "wiced.h"

/**
 * @cond DUAL_MODE
 * @addtogroup  sco      Synchronous Connection Oriented (SCO) Channel
 * @ingroup     wicedbt_sco
 *
 * @{
 */


/******************************************************
 *              Constants
 ******************************************************/

#ifndef WICED_SCO_PKT_TYPES_MASK
#define WICED_INVALID_SCO_INDEX           0xFFFF    /**< Default SCO index */
#define WICED_SCO_LINK_ALL_PKT_MASK       0x003F    /**< SCO packet type all */
#define WICED_SCO_PKT_TYPES_MASK_HV3      0x0004    /**< SCO packet type HV3 */
#define WICED_SCO_PKT_TYPES_MASK_EV3      0x0008    /**< SCO packet type EV3 */
#define WICED_SCO_PKT_TYPES_MASK_EV4      0x0010    /**< SCO packet type EV4 */
#define WICED_SCO_PKT_TYPES_MASK_EV5      0x0020    /**< SCO packet type EV5 */
#define WICED_SCO_PKT_TYPES_MASK_NO_2_EV3 0x0040    /**< SCO packet type 2-EV3 */
#define WICED_SCO_PKT_TYPES_MASK_NO_3_EV3 0x0080    /**< SCO packet type 3-EV3 */
#define WICED_SCO_PKT_TYPES_MASK_NO_2_EV5 0x0100    /**< SCO packet type 2-EV5 */
#define WICED_SCO_PKT_TYPES_MASK_NO_3_EV5 0x0200    /**< SCO packet type 3-EV5 */
#endif

/** SCO route path */
typedef enum
{
    WICED_BT_SCO_OVER_PCM = 0,    /**< [DEFAULT] PCM data config for routing over I2S/PCM interface */
    WICED_BT_SCO_OVER_APP_CB,     /**< PCM data config for routing over APP */
    WICED_BT_SCO_OVER_HCI         /**< SCO over HCI to Wiced stack in the host */
}wiced_bt_sco_route_path_t;

/******************************************************
 *              Type Definitions
 ******************************************************/
#define  WICED_BT_SCO_DATA_CB_GET_LENGTH(ltch_len)   ((ltch_len>>8)&0xff)	/**< SCO data callback length */


/** Call back function for pcm data transfer, ltch_len = (length)<<8|(sco_channel) */
typedef void (wiced_bt_sco_data_cb_t) (uint32_t ltch_len, uint8_t *p_data);

/** Subset for the enhanced setup/accept synchronous connection paramters
 * See BT 4.1 or later HCI spec for details */
typedef struct
{
    uint16_t max_latency;                   /**< Maximum latency (0x4-0xFFFE in msecs) */
    uint16_t packet_types;                  /**< Packet Types */
    uint8_t retrans_effort;                 /**< 0x00-0x02, 0xFF don't care */
    wiced_bool_t use_wbs;                   /**< True to use wide band, False to use narrow band */
} wiced_bt_sco_params_t;

/** SCO path config */
typedef struct
{
    wiced_bt_sco_route_path_t    path;           /**< sco routing path  0:pcm/i2s; 1: app*/
    wiced_bt_sco_data_cb_t       *p_sco_data_cb; /**< If not NULL and route is APP_CB, callback function called for incoming pcm data */
}wiced_bt_voice_path_setup_t;


/******************************************************
 *              Function Declarations
 ******************************************************/
#ifdef __cplusplus
extern "C"
{
#endif

/**
 *
 *The wiced_sco_lib.a was required to included before we link this function.
 *Creates a synchronous connection oriented connection as initiator.
 *
 *  @param[in]  bd_addr                 : Peer bd_addr
 *  @param[in]  p_params                : Pointer to the SCO parameter structure
 *  @param[out] p_sco_index             : SCO index returned
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
                                                        wiced_bt_sco_params_t *p_params);

/**
 *
 *Creates a synchronous connection oriented connection as acceptor.
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
 *
 *Removes a specific synchronous connection oriented connection.
 *
 *  @param[in]  sco_index                           : SCO index to remove
 *
 *  @return     <b> WICED_BT_UNKNOWN_ADDR </b>      : Remove connection failed, invalid SCO index
 *              <b> WICED_BT_NO_RESOURCES </b>      : Remove connection failed, no resource
 *              <b> WICED_BT_SUCCESS </b>           : Remove connection successfully, device is still
 *                                                    listening for incoming connection
 *              <b> WICED_BT_PENDING </b>           : Remove connection successfully
 */
wiced_bt_dev_status_t wiced_bt_sco_remove (uint16_t sco_index);

/**
 *
 *The wiced_sco_lib.a was required to be included before we link this function.
 *Called to handle (e)SCO connection request event (wiced_bt_sco_connect_request_event).
 *
 *
 *  @param[in]  sco_index           : SCO index
 *
 *  @param[in]  hci_status          : WICED_BT_SCO_CONNECTION_ACCEPT              0x00
 *                                    WICED_BT_SCO_CONNECTION_REJECT_RESOURCES    0x0D
 *                                    WICED_BT_SCO_CONNECTION_REJECT_SECURITY     0x0E
 *                                    WICED_BT_SCO_CONNECTION_REJECT_DEVICE       0x0F
 *  @param[in]  p_params            : Pointer to the SCO parameter structure
 *
 */
void wiced_bt_sco_accept_connection (uint16_t sco_index, uint8_t hci_status,
                                     wiced_bt_sco_params_t *p_params);


/**
 *
 *The wiced_sco_lib.a was required to include before we link this function.
 *Configure the SCO routing path.
 *
 *  @param[in]  pData                 : To setup SCO routing path
 *
 * @return
 *              WICED_BT_SUCCESS    : Config success.
 *              WICED_BT_PENDING    : Command sent. Waiting for command complete event.
 *              WICED_BT_BUSY       : Command not sent. Waiting for command complete event for prior command.
 */
wiced_bt_dev_status_t wiced_bt_sco_setup_voice_path(wiced_bt_voice_path_setup_t *pData);

/**
 *
 *The wiced_sco_lib.a was required to include before we link this function.
 *Send pcm data to internal audio pipe.
 *
 *
 *  @param[in]  sco_index     : SCO index to send the stream
 *  @param[in]  p_pcmsrc        : Audio stream source.
 *  @param[in]  len             : Length of stream.
 *
 *  @return                     : Return the legth of non transmited stream.
 */
uint16_t wiced_bt_sco_output_stream( uint16_t sco_index, uint8_t* p_pcmsrc,uint16_t len );

/**
 *
 *The wiced_voice_path.a was required to include before we link this function.
 *To turn off the PCM/I2S hardware clock
 *This function needs to be called after the application turns off (or mutes audio data in/out of) the codec
 *
 *  @return                     : None
 */

void  wiced_bt_sco_turn_off_pcm_clock( void );


#ifdef __cplusplus
}
#endif

/** @} sco */
/* @endcond*/

