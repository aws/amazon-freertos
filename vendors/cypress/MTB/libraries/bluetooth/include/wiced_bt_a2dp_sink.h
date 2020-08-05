/*
 * $ Copyright Cypress Semiconductor $
 */

#pragma once

#ifdef __cplusplus
extern "C"
{
#endif

#include "wiced_bt_types.h"
#include "wiced_result.h"
#include "bt_target.h"
#include "wiced_bt_a2d.h"
#include "wiced_bt_a2d_sbc.h"
#include "wiced_bt_a2d_m12.h"
#include "wiced_bt_a2d_m24.h"
#include "wiced_bt_a2dp_defs.h"
//#include "wiced_bt_audio_codec.h"

/** @file:   wiced_bt_a2dp_sink.h
 *  This file Contains A2DP Sink APIs and definitions.
 */

/**
 * @defgroup    wicedbt_av_profiles Profiles
 * @ingroup     wicedbt_av
 *
 * @addtogroup  wicedbt_a2dp        Advanced Audio Profile (A2DP) Sink
 * @ingroup     wicedbt_av_profiles
 * @ingroup     wicedbt_av
 *
 * The Advanced Audio Distribution Profile (A2DP) defines the protocols and procedures that
 * realize distribution of audio content of high-quality in mono or stereo on ACL channels. The
 * term "advanced audio", therefore, should be distinguished from "Bluetooth audio", which indicates
 * distribution of narrow band voice on SCO channels. A typical usage case is the streaming of music
 * content from a stereo music player to headphones or speakers. The audio data is compressed in a
 * proper format for efficient use of the limited bandwidth. Surround sound distribution is not included in
 * the scope of this profile.
 *
 * @{
 */

/******************************************************
 *                      Macros
 ******************************************************/

/******************************************************
 *                    Constants
 ******************************************************/


/******************************************************
 *                   Enumerations
 ******************************************************/

/** Events in wiced_bt_a2dp_sink_control_cb_t() callback,
 * for payload see wiced_bt_a2dp_sink_event_data_t */
typedef enum
{
    WICED_BT_A2DP_SINK_CONNECT_EVT,     /**< Connected event, received on establishing connection to a peer device */
    WICED_BT_A2DP_SINK_DISCONNECT_EVT,  /**< Disconnected event, received on disconnection from a peer device */
    WICED_BT_A2DP_SINK_START_IND_EVT,   /**< Start stream indication event, received when start req is received */
    WICED_BT_A2DP_SINK_START_CFM_EVT,   /**< Start stream confirm event, received when start req is sent and response is received */
    WICED_BT_A2DP_SINK_SUSPEND_EVT,     /**< Suspend stream event, received when audio streaming is suspended */
    WICED_BT_A2DP_SINK_CODEC_CONFIG_EVT /**< Codec config event, received when codec config for a streaming session is updated */
} wiced_bt_a2dp_sink_event_t;


/** A2DP Sink features masks */
typedef enum
{
    WICED_BT_A2DP_SINK_FEAT_PROTECT   = 0x0001, /**< Streaming media content protection */
    WICED_BT_A2DP_SINK_FEAT_DELAY_RPT = 0x0002, /**< Use delay reporting */
} wiced_bt_a2dp_sink_feature_mask_t;

/******************************************************
 *                 Type Definitions
 ******************************************************/

/******************************************************
 *                    Structures
 ******************************************************/

/** Codec capability information list structure,
 * used to indicate the supported codecs and their capabilities */
typedef struct
{
    uint8_t                     count; /**< Number of codecs present in the list */
    wiced_bt_a2dp_codec_info_t* info;  /**< Codec information list */
} wiced_bt_a2dp_codec_info_list_t;

/** A2DP sink configuration data structure *
 * Please Note that ext_codec field will be ignored for 20706A2 */
typedef struct
{
    wiced_bt_a2dp_sink_feature_mask_t         feature_mask;       /**< Supported features */
    wiced_bt_a2dp_codec_info_list_t           codec_capabilities; /**< List of supported codecs and their capabilities */
    wiced_bt_a2dp_sink_audio_tuning_params_t  p_param;            /**< Audio tuning parameters */
    //wiced_bt_a2dp_ext_codec_info_t            ext_codec;          /**< External Codec information */
} wiced_bt_a2dp_config_data_t;

/** Audio payload header */
typedef struct
{
    BT_HDR  *p_pkt;     /**< Audio data packet */
    uint32_t timestamp; /**< Timestamp */
    uint16_t seq_num;   /**< Sequence number */
    uint8_t  m_pt;      /**< Marker bit */
} wiced_bt_a2dp_sink_audio_data_t;

/** Generic event status info */
typedef struct
{
    wiced_result_t            result;  /**< Whether the event indicates failure or success, WICED_BT_XXX */
    wiced_bt_device_address_t bd_addr; /**< Peer bluetooth device address */
    uint16_t                  handle;  /**< Peer connection handle */
} wiced_bt_a2dp_sink_status_t;

typedef struct
{
    wiced_result_t            result;  /**< Whether the event indicates failure or success, WICED_BT_XXX */
    uint8_t                   label;   /**< Transaction label */
    uint16_t                  handle;  /**< Peer connection handle */
} wiced_bt_a2dp_sink_start_t;

typedef struct
{
    uint16_t                    handle; /**< Peer connection handle */
    wiced_bt_a2dp_codec_info_t  codec;  /**< Configured codec params */
} wiced_bt_a2dp_sink_codec_config_t;

/** Control callback event data */
typedef union
{
    wiced_bt_a2dp_sink_status_t         connect;        /**< WICED_BT_A2DP_SINK_CONNECT_EVT payload */
    wiced_bt_a2dp_sink_status_t         disconnect;     /**< WICED_BT_A2DP_SINK_DISCONNECT_EVT payload */
    wiced_bt_a2dp_sink_start_t          start_ind;      /**< WICED_BT_A2DP_SINK_START_IND_EVT payload */
    wiced_bt_a2dp_sink_status_t         start_cfm;      /**< WICED_BT_A2DP_SINK_START_CFM_EVT payload */
    wiced_bt_a2dp_sink_status_t         suspend;        /**< WICED_BT_A2DP_SINK_SUSPEND_EVT payload */
    wiced_bt_a2dp_sink_codec_config_t   codec_config;   /**< WICED_BT_A2DP_SINK_CODEC_CONFIG_EVT payload */
} wiced_bt_a2dp_sink_event_data_t;


/******************************************************
 *                 Callback Type Definitions
 ******************************************************/

/** A2DP Control path callback type
 *
 *  Application implements callback of this type to receive A2DP control path events.
 *
 *  @param event    Id of event being notified to app.
 *  @param p_data   Pointer to data associated with the event.
 *
 *  @return none
 * */
typedef void (*wiced_bt_a2dp_sink_control_cb_t)( wiced_bt_a2dp_sink_event_t event,
    wiced_bt_a2dp_sink_event_data_t* p_data );

/** A2DP data path callback type
 *
 *  Application implements callback of this type to receive A2DP media packets.
 *
 *  @param codec_type    codec for the associated data.
 *  @param p_audio_data  pointer to audio data and related information.
 *
 *  @return none
 * */
typedef void (*wiced_bt_a2dp_sink_data_cb_t)( wiced_bt_a2dp_codec_t codec_type,
    wiced_bt_a2dp_sink_audio_data_t* p_audio_data );

/******************************************************
 *               Function Declarations
 ******************************************************/

/** API to initialize the A2DP SINK component and register with the stack.
 *
 *  Called by the application before any other API is called.
 *  Application provides the SINK configuration control callbacks
 *  to receive control events.
 *
 *  @param p_config_data    A2DP sink configuration parameters.
 *                          This should remain valid until deinit is called
 *                          as the pointer is stored and used inside the library.
 *  @param control_cb       Callback function for receiving sink events.
 *
 *  @return wiced_result_t (WICED_BT_XXX)
 */
wiced_result_t wiced_bt_a2dp_sink_init( wiced_bt_a2dp_config_data_t* p_config_data,
    wiced_bt_a2dp_sink_control_cb_t control_cb );


/** API to deregister from the stack and to cleanup the memory of A2DP sink component.
 *
 *  Called by the application when A2DP sink component is no longer needed by it.
 *
 *  @param channel  Media type to be handled by the sink.
 *
 *  @return wiced_result_t (WICED_BT_XXX)
 */
wiced_result_t wiced_bt_a2dp_sink_deinit(void);


/** API to connect to a peer device.
 *
 *  Called by the app to establish an A2DP connection with a peer device.
 *
 *  @param bd_address    Bluetooth device address of the device to which connection is requested.
 *
 *  @return wiced_result_t (WICED_BT_XXX)
 */
wiced_result_t wiced_bt_a2dp_sink_connect( wiced_bt_device_address_t bd_address );


/** API to disconnect the connection from a connected peer device.
 *
 *  Called by the application to disconnected from a connected A2DP source.
 *
 *  @param handle    Connection handle corresponding to peer device to disconnect from.
 *
 *  @return wiced_result_t (WICED_BT_XXX)
 */
wiced_result_t wiced_bt_a2dp_sink_disconnect( uint16_t handle );


/** API to start streaming.
 *
 *  Called by the application when it wants to indicate the peer to start streaming.
 *
 *  @param handle        Connection handle corresponding to peer device
 *                       to create a streaming connection.
 *
 *  @return wiced_result_t (WICED_BT_XXX)
 */
wiced_result_t wiced_bt_a2dp_sink_start( uint16_t handle );

/** API to send the start response on receiving the start request from the peer
 *
 *  Called by the application when it wants to indicate the peer that it is ready to start streaming.
 *
 *  @param handle        Connection handle corresponding to peer device
 *  @param label         Transaction label
 *  @param status        Indicates if start request is accepted(AVDT_SUCCESS) or rejected(AVDT Error codes)
 *
 *  @return wiced_result_t (WICED_BT_XXX)
 */
wiced_result_t wiced_bt_a2dp_sink_send_start_response( uint16_t handle, uint8_t label, uint8_t status );


/** API to suspend streaming.
 *
 *  Called by the application when the streaming is to be suspended.
 *
 *  @param handle        Connection handle corresponding to peer device
 *                       for which streaming is suspended.
 *
 *  @return wiced_result_t (WICED_BT_XXX)
 */
wiced_result_t wiced_bt_a2dp_sink_suspend( uint16_t handle );

/** API used to register for data callback.
 *
 *  Application provides the SINK configuration data to receive data packets.
 *
 * @param data_cb          Callback function for receiving audio data.
 *
 */
void wiced_bt_a2dp_sink_register_data_cback( wiced_bt_a2dp_sink_data_cb_t p_cback );


/** API to send sink delay report to the peer.
 *
 *  Called by the app if it supports the sink delay report to report the
 *  latency of the audio rendering path.
 *
 *  @param handle        Connection handle corresponding to peer device
 *                       to which the delay report is to be sent.
 *  @param delay         Delay in ms.
 *
 *  @return wiced_result_t (WICED_BT_XXX)
 */
wiced_result_t wiced_bt_a2dp_sink_send_delay_report( uint16_t handle, uint16_t delay );

/** To mute/unmute the audio while streaming
 *
 *  Called by the application to mute the audio when playing the music. Application shall
 *  this function to unmute, to restart playing the music
 *
 *  @param enable    1 to mute, 0 to unmute
 *
 *  @param ramp_ms    ramp up/down time in milli seconds
 *
 *  @return wiced_result_t (WICED_BT_XXX)
 */
wiced_result_t  wiced_bt_a2dp_sink_mute_audio( wiced_bool_t enable, uint16_t ramp_ms );

/** To configure audio route
 *
 *  Called by the application to configure audio data route path.
 *  API should be called after receiving WICED_BT_A2DP_SINK_CODEC_CONFIG_EVT event and before accepting audio start request.
 *
 *  @param handle        Connection handle corresponding to peer device
 *                       to which route need to be configured
 *
 *  @param route_config  route config parameters
 *
 *  @return WICED_TRUE in success case,otherwise WICED_FALSE
 */
 wiced_bool_t wiced_bt_a2dp_sink_update_route_config( uint16_t handle, wiced_bt_a2dp_sink_route_config *route_config );

/** API To get LRAC Switch data
 *
 *  Called by the application to get the LRAC Switch Data
 *
 *  @param p_opaque     Pointer to a buffer which will be filled with LRAC Switch data (current
 *                      A2DP Sink State)
 *  @param p_opaque     Size of the buffer (IN), size filled (OUT)
 *
 *  @return none
 */
wiced_result_t wiced_bt_a2dp_sink_lrac_switch_get(void *p_opaque, uint16_t *p_sync_data_len);

/** API To set LRAC Switch data
 *
 *  Called by the application to set the LRAC Switch Data
 *
 *  @param p_opaque     Pointer to a buffer which contains LRAC Switch data (new
 *                      A2DP Sink State)
 *  @param p_opaque     Size of the buffer (IN)
 *
 *  @return none
 */
wiced_result_t wiced_bt_a2dp_sink_lrac_switch_set(void *p_opaque, uint16_t sync_data_len);

/** @} */ // end of wicedbt_a2dp

#ifdef __cplusplus
} /*extern "C" */
#endif

