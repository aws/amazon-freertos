/*
 * $ Copyright Broadcom Corporation $
 */

/** @file
 *
 * Bluetooth AVRC Remote Control Application Programming WICED Interface
 *
 * @defgroup wicedbt      Bluetooth
 *
 * WICED Bluetooth AVRC Remote Control Functions
 */

#pragma once

#include "wiced_bt_types.h"
#include "wiced_result.h"
#include "bt_target.h"
#include "wiced_bt_avrc_defs.h"

#include "wiced_bt_avrc.h"


/*****************************************************************************
**  Constants and data types
*****************************************************************************/

/** AVRC remote control feature mask */
typedef enum
{
    REMOTE_CONTROL_FEATURE_TARGET   = 0x0001,
    REMOTE_CONTROL_FEATURE_CONTROLLER = 0x0002,
    REMOTE_CONTROL_FEATURE_PROTECT = 0x0004,
    REMOTE_CONTROL_FEATURE_VENDOR = 0x0008,
    REMOTE_CONTROL_FEATURE_BROWSE = 0x0010,
    REMOTE_CONTROL_FEATURE_REPORT = 0x0020,
    REMOTE_CONTROL_FEATURE_DELAY_RPT = 0x0040,
    REMOTE_CONTROL_FEATURE_METADATA = 0x0080,
} wiced_bt_remote_control_features_t;

/** AVRC remote control connection state */
typedef enum
{
    REMOTE_CONTROL_DISCONNECTED = 0,
    REMOTE_CONTROL_CONNECTED    = 1,
} wiced_bt_remote_control_connection_state_t;

/** Callback for connection state */
/* @peer_features      : ORed values of enum wiced_bt_remote_control_features_t */
typedef void (*wiced_bt_remote_control_connection_state_cback_t)(
                wiced_bt_device_address_t remote_addr, wiced_result_t status,
                wiced_bt_remote_control_connection_state_t connection_state,
                uint32_t peer_features);

/** Response callback from peer device for AVRCP commands */
typedef void (*wiced_bt_remote_control_rsp_cback_t)(wiced_bt_device_address_t remote_addr,
                wiced_bt_avrc_response_t *avrc_rsp);

/** Callback when peer device sends AVRCP commands */
typedef void (*wiced_bt_remote_control_cmd_cback_t)(wiced_bt_device_address_t remote_addr,
                wiced_bt_avrc_command_t *avrc_cmd);


/******************************************************
 *               Function Declarations
 ******************************************************/
#ifdef __cplusplus
extern "C" {
#endif

/****************************************************************************/
/**
 * The Audio/Video Remote Control Profile (AVRCP) defines the features and procedures required to ensure
 * interoperability between Bluetooth devices with audio/video control functions in the Audio/Video distribution
 * scenarios. This profile specifies the scope of the AV/C Digital Interface Command Set (AV/C command set,
 * defined by the 1394 Trade Association) to be applied, and it realizes simple implementation and easy operability.
 * This profile adopts the AV/C device model and command format for control messages, and those messages are
 * transported by the Audio/Video Control Transport Protocol (AVCTP). Browsing functionality is provided over a
 * second AVCTP channel, which does not use AV/C.
 *
 * @defgroup    wicedbt_avrcp   Audio/Video Remote Control Protocol (AVRCP)
 * @ingroup     wicedbt_av_profiles
 *
 * @addtogroup  wicedbt_avrcp       Remote control
 * @ingroup     wicedbt_av_profiles
 *
 * @{
 */
/****************************************************************************/

/**
 * Function         wiced_bt_remote_control_init
 *
 *                  Initialize the AVRC controller and start listening for incoming connections
 *
 * @param[in]       local_features      : Local supported features mask
 *                                        Combination of wiced_bt_remote_control_features_t
 * @param[in]       p_connection_cback  : Callback for connection state
 * @param[in]       p_rsp_cb            : Callback from peer device in response to AVRCP commands
 * @param[in]       p_cmd_cb            : Callback when peer device sends AVRCP commands
 *
 * @return          wiced_result_t
 *
 */
wiced_result_t wiced_bt_remote_control_init(uint32_t local_features,
                    wiced_bt_remote_control_connection_state_cback_t p_connection_cb,
                    wiced_bt_remote_control_cmd_cback_t p_cmd_cb,
                    wiced_bt_remote_control_rsp_cback_t p_rsp_cb);

/**
 * Function         wiced_bt_remote_control_deinit
 *
 *                  Deinit the AVRC controller and stop listening for incoming connections
 *
 * @return          wiced_result_t
 *
 */
wiced_result_t wiced_bt_remote_control_deinit(void);

/**
 * Function         wiced_bt_remote_control_connect
 *
 *                  Initiate connection to the peer AVRC target device.
 *                  After connection establishment, stop listening for incoming connections
 *
 * @param[in]       remote_addr : Bluetooth address of peer device
 *
 * @return          wiced_result_t
 *
 */
wiced_result_t wiced_bt_remote_control_connect(wiced_bt_device_address_t remote_addr);

/**
 * Function         wiced_bt_remote_control_disconnect
 *
 *                  Disconnect from the peer AVRC target device
 *                  After disconnection , start listening for incoming connections
 *
 * @param[in]       remote_addr : Bluetooth address of connected peer device
 *
 * @return          wiced_result_t
 *
 */
wiced_result_t wiced_bt_remote_control_disconnect(wiced_bt_device_address_t remote_addr);

/**
 * Function         wiced_bt_remote_control_send_pass_through_cmd
 *
 *                  Send PASS THROUGH command
 *
 * @param[in]       remote_addr     : Bluetooth address of connected peer device
 * @param[in]       cmd             : Pass through command id (see #AVRC_ID_XX)
 * @param[in]       state           : State of the pass through command (see #AVRC_STATE_XX)
 * @param[in]       data_field_len  : Data field length
 * @param[in]       data_field      : Data field
 *
 * @return          wiced_result_t
 *
 */
wiced_result_t wiced_bt_remote_control_send_pass_through_cmd(
                    wiced_bt_device_address_t remote_addr, uint8_t cmd, uint8_t state,
                    uint8_t data_len, uint8_t *data);

/**
 * Function         wiced_bt_remote_control_get_element_attr_cmd
 *
 *                  Requests the target device to provide the attributes
 *                  of the element specified in the parameter
 *
 * @param[in]       remote_addr : Bluetooth address of connected peer device
 * @param[in]       element_id  : Element id
 * @param[in]       num_attr    : Number of attributes
 * @param[in]       p_attrs     : Media attribute ids (see #AVRC_MEDIA_ATTR_ID)
 *
 * @return          wiced_result_t
 *
 */
wiced_result_t wiced_bt_remote_control_get_element_attr_cmd(
                    wiced_bt_device_address_t remote_addr, wiced_bt_avrc_uid_t element_id,
                    uint8_t num_attr, uint32_t *p_attrs);

/**
 * Function         wiced_bt_remote_control_get_play_status_cmd
 *
 *                  Get the status of the currently playing media at the TG
 *
 * @param[in]       remote_addr : Bluetooth address of connected peer device
 *
 * @return          wiced_result_t
 *
 */
wiced_result_t wiced_bt_remote_control_get_play_status_cmd(wiced_bt_device_address_t remote_addr);

/*****************************************************************************
 *  APPLICATION SETTINGS FUNCTIONS
 ****************************************************************************/

/**
 * Function         wiced_bt_remote_control_list_player_attrs_cmd
 *
 *                  Request the device to provide its supported
 *                  player application attributes
 *
 * @param[in]       remote_addr : Bluetooth address of connected peer device
 *
 * @return          wiced_result_t
 *
 */
wiced_result_t wiced_bt_remote_control_list_player_attrs_cmd(
                wiced_bt_device_address_t remote_addr);

/**
 * Function         wiced_bt_remote_control_list_player_values_cmd
 *
 *                  Requests the device to return the
 *                  set of possible values for the requested player application setting attribute
 *
 * @param[in]       remote_addr : Bluetooth address of connected peer device
 * @param[in]       attr        : Player application setting attribute. Refer Player Application Settings IDs in wiced_bt_avrc_defs.h
 *
 * @return          wiced_result_t
 *
 */
wiced_result_t wiced_bt_remote_control_list_player_values_cmd(
                    wiced_bt_device_address_t remote_addr, uint8_t attr);

/**
 * Function         wiced_bt_remote_control_get_player_value_cmd
 *
 *                  Requests the target device to provide the current set values
 *                  on the target for the provided player application setting attributes list
 *
 * @param[in]       remote_addr : Bluetooth address of connected peer device
 * @param[in]       num_attr    : Number of attributes
 * @param[in]       p_attrs     : Player attribute ids
 *
 * @return          wiced_result_t
 *
 */
wiced_result_t wiced_bt_remote_control_get_player_value_cmd(wiced_bt_device_address_t remote_addr,
                    uint8_t num_attr, uint8_t *p_attrs);

/**
 * Function         wiced_bt_remote_control_set_player_value_cmd
 *
 *                  Requests to set the player application setting values on the target device
 *
 * @param[in]       remote_addr : Bluetooth address of connected peer device
 * @param[in]       p_vals      : Player application setting values
 *
 * @return          wiced_result_t
 *
 */
wiced_result_t wiced_bt_remote_control_set_player_value_cmd(wiced_bt_device_address_t remote_addr,
                    wiced_bt_avrc_player_app_param_t *p_vals);

/**
 * Function         wiced_bt_remote_control_get_player_attrs_text_cmd
 *
 *                  Requests the target device to provide the current set values
 *                  on the target for the provided player application setting attributes list
 *
 * @param[in]       remote_addr : Bluetooth address of connected peer device
 * @param[in]       num_attr    : Number of attributes
 * @param[in]       p_attrs     : Player attribute ids (see #AVRC_PLAYER_SETTING_XX)
 *
 * @return          wiced_result_t
 *
 */
wiced_result_t wiced_bt_remote_control_get_player_attrs_text_cmd(
                    wiced_bt_device_address_t remote_addr,
                    uint8_t num_attr, uint8_t *p_attrs);

/**
 * Function         wiced_bt_remote_control_get_player_values_text_cmd
 *
 *                  Request the target device to provide target supported player
 *                  application setting value displayable text
 *
 * @param[in]       remote_addr : Bluetooth address of connected peer device
 * @param[in]       attr        : player application setting attribute
 * @param[in]       num_attr    : Number of values
 * @param[in]       p_attrs     : Player value scan value ids (see #AVRC_PLAYER_VAL_XX)
 *
 * @return          wiced_result_t
 *
 */
wiced_result_t wiced_bt_remote_control_get_player_values_text_cmd(
                    wiced_bt_device_address_t remote_addr, uint8_t attr,
                    uint8_t num_val, uint8_t *p_values);

/*****************************************************************************
 *  AVRCP 1.5 BROWSING FUNCTIONS
 ****************************************************************************/

/**
 * Function         wiced_bt_remote_control_set_addressed_player_cmd
 *
 *                  Set the player id to the player to be addressed on the target device
 *
 * @param[in]       remote_addr : Bluetooth address of connected peer device
 * @param[in]       player_id   : Player id
 *
 * @return          wiced_result_t
 *
 */
wiced_result_t wiced_bt_remote_control_set_addressed_player_cmd(
                    wiced_bt_device_address_t remote_addr, uint16_t player_id);

/**
 * Function         wiced_bt_remote_control_set_browsed_player_cmd
 *
 *                  Set the player id to the browsed player to be addressed on the target device
 *
 * @param[in]       remote_addr : Bluetooth address of connected peer device
 * @param[in]       player_id   : Player id
 *
 * @return          wiced_result_t
 *
 */
wiced_result_t wiced_bt_remote_control_set_browsed_player_cmd(
                    wiced_bt_device_address_t remote_addr, uint16_t player_id);

/**
 * Function         wiced_bt_remote_control_change_path_cmd
 *
 *                  Change the path in the Virtual file system being browsed
 *
 * @param[in]       remote_addr : Bluetooth address of connected peer device
 * @param[in]       direction   : Direction of path change
 * @param[in]       path_uid    : Path uid
 *
 * @return          wiced_result_t
 *
 */
wiced_result_t wiced_bt_remote_control_change_path_cmd(wiced_bt_device_address_t remote_addr,
                    uint8_t direction, wiced_bt_avrc_uid_t path_uid);

/**
 * Function         wiced_bt_remote_control_get_folder_items_cmd
 *
 *                  Retrieves a listing of the contents of a folder
 *
 * @param[in]       remote_addr : Bluetooth address of connected peer device
 * @param[in]       scope       : Scope of the folder
 * @param[in]       start_item  : Start item index
 * @param[in]       end_item    : End item index
 * @param[in]       num_attr    : Number of attributes
 * @param[in]       p_attrs     : Media attribute ids (see #AVRC_MEDIA_ATTR_ID)
 *
 * @return          wiced_result_t
 *
 */
wiced_result_t wiced_bt_remote_control_get_folder_items_cmd(wiced_bt_device_address_t remote_addr,
                    uint8_t scope, uint32_t start_item, uint32_t end_item,
                    uint8_t num_attr, uint32_t *p_attrs);

/**
 * Function         wiced_bt_remote_control_get_item_attributes_cmd
 *
 *                  Retrieves the metadata attributes for a
 *                  particular media element item or folder item
 *
 * @param[in]       remote_addr : Bluetooth address of connected peer device
 * @param[in]       scope       : Scope of the item
 * @param[in]       path_uid    : Path of the item
 * @param[in]       num_attr    : Number of attributes
 * @param[in]       p_attrs     : Media attribute ids (see #AVRC_MEDIA_ATTR_ID)
 *
 * @return          wiced_result_t
 *
 */
wiced_result_t wiced_bt_remote_control_get_item_attributes_cmd(
                    wiced_bt_device_address_t remote_addr,
                    uint8_t scope, wiced_bt_avrc_uid_t path_uid,
                    uint8_t num_attr, uint32_t *p_attrs);

/**
 * Function         wiced_bt_remote_control_search_cmd
 *
 *                  Performs search from the current folder
 *                  in the Browsed Player's virtual file system
 *
 * @param[in]       remote_addr     : Bluetooth address of connected peer device
 * @param[in]       search_string   : Search string
 *
 * @return          wiced_result_t
 *
 */
wiced_result_t wiced_bt_remote_control_search_cmd(wiced_bt_device_address_t remote_addr,
                    wiced_bt_avrc_full_name_t search_string);

/**
 * Function         wiced_bt_remote_control_play_item_cmd
 *
 *                  Starts playing an item indicated by the UID
 *
 * @param[in]       remote_addr : Bluetooth address of connected peer device
 * @param[in]       scope       : Scope of the item (see #AVRC_SCOPE_XX)
 * @param[in]       item_uid    : UID of the item
 *
 * @return          wiced_result_t
 *
 */
wiced_result_t wiced_bt_remote_control_play_item_cmd(wiced_bt_device_address_t remote_addr,
                    uint8_t scope, wiced_bt_avrc_uid_t item_uid);


/**
 * Function         wiced_bt_remote_control_add_to_now_playing_cmd
 *
 *                  Adds an item indicated by the UID to the Now Playing queue
 *
 * @param[in]       remote_addr : Bluetooth address of connected peer device
 * @param[in]       scope       : Scope of the item (see #AVRC_SCOPE_XX)
 * @param[in]       item_uid    : UID of the item
 *
 * @return          wiced_result_t
 *
 */
wiced_result_t wiced_bt_remote_control_add_to_now_playing_cmd(
                    wiced_bt_device_address_t remote_addr, uint8_t scope,
                    wiced_bt_avrc_uid_t item_uid);

/*****************************************************************************
 *  VOLUME FUNCTIONS
 ****************************************************************************/
/**
 * Function         wiced_bt_remote_control_set_volume_cmd
 *
 *                  Set volume for peer device
 *
 * @param[in]       remote_addr : Bluetooth address of connected peer device
 * @param[in]       volume      : Volume (offset between 0 - 127)
 *
 * @return          wiced_result_t
 *
 */
wiced_result_t wiced_bt_remote_control_set_volume_cmd(wiced_bt_device_address_t remote_addr,
                    uint8_t volume);

/** @}*/

#ifdef __cplusplus
}
#endif
