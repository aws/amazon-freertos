/*
 * $ Copyright Broadcom Corporation $
 */

#pragma once

#ifdef __cplusplus
extern "C"
{
#endif

#include "wiced_bt_types.h"
#include "wiced_result.h"
#include "bt_target.h"

/**
 * @addtogroup  wicedbt_hfp          Hands Free Profile (HFP)
 * @ingroup wicedbt_av_profiles
 *
 * The profile defines how two devices supporting the Hands-Free Profile shall interact with each other
 * on a point-to-point basis. An implementation of the Hands-Free Profile typically enables a headset, or
 * an embedded hands-free unit to connect, wirelessly, to a cellular phone for the purposes of acting as
 * the cellular phone's audio input and output mechanism and allowing typical telephony functions to be
 * performed without access to the actual phone.
 *
 * The most common examples of such devices are in-car Hands-Free units used together with cellular phones,
 * or wearable wireless headsets.
 *
 * @{
*/


/** @file:   wiced_bt_hfp_hf.h
 *  This file Contains Hand Free Profile - Hands Free Device APIs and definitions.
 */


/******************************************************
 *                      Macros
 ******************************************************/
#define WICED_BT_HFP_HF_CALLER_NUMBER_MAX_LENGTH        32
#define WICED_BT_HFP_HF_AT_CMD_RESULT_CODE_MAX_LENGTH   256

/* Maximum number of HFP HF connections supported */
#define WICED_BT_HFP_HF_MAX_CONN 2

/******************************************************
 *                    Constants
 ******************************************************/

/******************************************************
 *                   Typedefs
 ******************************************************/
typedef char wiced_bt_hfp_hf_caller_num_t[WICED_BT_HFP_HF_CALLER_NUMBER_MAX_LENGTH];
typedef char wiced_bt_hfp_hf_at_result_code_t[WICED_BT_HFP_HF_AT_CMD_RESULT_CODE_MAX_LENGTH];

/******************************************************
 *                   Enumerations
 ******************************************************/

/** HF device supported feature flags. */
typedef enum
{
    WICED_BT_HFP_HF_FEATURE_ECNR                         = 0x00000001,
    WICED_BT_HFP_HF_FEATURE_3WAY_CALLING                 = 0x00000002,
    WICED_BT_HFP_HF_FEATURE_CLIP_CAPABILITY              = 0x00000004,
    WICED_BT_HFP_HF_FEATURE_VOICE_RECOGNITION_ACTIVATION = 0x00000008,
    WICED_BT_HFP_HF_FEATURE_REMOTE_VOLUME_CONTROL        = 0x00000010,
    WICED_BT_HFP_HF_FEATURE_ENHANCED_CALL_STATUS         = 0x00000020,
    WICED_BT_HFP_HF_FEATURE_ENHANCED_CALL_CONTROL        = 0x00000040,
    WICED_BT_HFP_HF_FEATURE_CODEC_NEGOTIATION            = 0x00000080,
    WICED_BT_HFP_HF_FEATURE_HF_INDICATORS                = 0x00000100,
    WICED_BT_HFP_HF_FEATURE_ESCO_S4_T2_SETTINGS_SUPPORT  = 0x00000200,
    WICED_BT_HFP_HF_FEATURE_ENHANCED_VOICE_RECOGNITION   = 0x00004000
} wiced_bt_hfp_hf_supported_features_t;

/** AG supported feature flags. */
typedef enum
{
    WICED_BT_HFP_AG_FEATURE_3WAY_CALLING                 = 0x00000001,
    WICED_BT_HFP_AG_FEATURE_ECNR                         = 0x00000002,
    WICED_BT_HFP_AG_FEATURE_VOICE_RECOGNITION_ACTIVATION = 0x00000004,
    WICED_BT_HFP_AG_FEATURE_INBAND_RING_TONE_CAPABILITY  = 0x00000008,
    WICED_BT_HFP_AG_FEATURE_ATTACH_NUMBER_TO_VOICE_TAG   = 0x00000010,
    WICED_BT_HFP_AG_FEATURE_ABILITY_TO_REJECT_CALL       = 0x00000020,
    WICED_BT_HFP_AG_FEATURE_ENHANCED_CALL_STATUS         = 0x00000040,
    WICED_BT_HFP_AG_FEATURE_ENHANCED_CALL_CONTROL        = 0x00000080,
    WICED_BT_HFP_AG_FEATURE_EXTENDED_ERROR_RESULT_CODES  = 0x00000100,
    WICED_BT_HFP_AG_FEATURE_CODEC_NEGOTIATION            = 0x00000200,
    WICED_BT_HFP_AG_FEATURE_HF_INDICATORS                = 0x00000400,
    WICED_BT_HFP_AG_FEATURE_ESCO_S4_T2_SETTINGS_SUPPORT  = 0x00000800,
    WICED_BT_HFP_AG_FEATURE_ENHANCED_VOICE_RECOGNITION   = 0x00001000
} wiced_bt_hfp_ag_supported_features_t;

/** HF Events. These are received via wiced_bt_hfp_hf_event_cb_t() callback function.
 *  See wiced_bt_hfp_hf_event_data_t for payload. */
typedef enum
{
    WICED_BT_HFP_HF_CONNECTION_STATE_EVT,   /**< Received on control path connection state change */
    WICED_BT_HFP_HF_AG_FEATURE_SUPPORT_EVT, /**< Indicates HFP features supported in AG */
    WICED_BT_HFP_HF_SERVICE_STATE_EVT,      /**< Indicates AG's cellular network connection state */
    WICED_BT_HFP_HF_SERVICE_TYPE_EVT,       /**< Indicates whether AG is connected to home or romaing network */
    WICED_BT_HFP_HF_CALL_SETUP_EVT,         /**< Received when there is a change in call state, e.g., incoming call, call termination */
    WICED_BT_HFP_HF_RING_EVT,               /**< Ring indication (during incoming call) received */
    WICED_BT_HFP_HF_INBAND_RING_STATE_EVT,  /**< Indicates if the AG supports sending the ring-tone to HF over audio connection */
    WICED_BT_HFP_HF_RSSI_IND_EVT,           /**< Indicates AG's cellular signal strength */
    WICED_BT_HFP_HF_BATTERY_STATUS_IND_EVT, /**< Indicates AG's battery status */
    WICED_BT_HFP_HF_VOLUME_CHANGE_EVT,      /**< Received when there is a change in the microphone or speaker volume is changed by AG */
    WICED_BT_HFP_HF_CLIP_IND_EVT,           /**< Indicates calling line identification name/number */
    WICED_BT_HFP_HF_AT_RESULT_CODE_IND_EVT  /**< Indicates the result code from AG  */
} wiced_bt_hfp_hf_event_t;

/** HF Control Connection States */
typedef enum
{
    WICED_BT_HFP_HF_STATE_DISCONNECTED, /**< HF control connection is closed */
    WICED_BT_HFP_HF_STATE_CONNECTED,    /**< HF control connection established */
    WICED_BT_HFP_HF_STATE_SLC_CONNECTED /**< HF synchronized with AG's state, ready to send/recive commands/notifications */
} wiced_bt_hfp_hf_connection_state_t;

/** AG's serivce states */
typedef enum
{
    WICED_BT_HFP_HF_SERVICE_STATE_NOT_AVAILABLE, /**< AG's cellular services not available */
    WICED_BT_HFP_HF_SERVICE_STATE_AVAILABLE      /**< AG is connected to cellular services */
} wiced_bt_hfp_hf_service_state_t;

/** AG's serivce type */
typedef enum
{
    WICED_BT_HFP_HF_SERVICE_TYPE_HOME,   /**< AG is connected to home network */
    WICED_BT_HFP_HF_SERVICE_TYPE_ROAMING /**< AG is connected to a romaing network */
} wiced_bt_hfp_hf_service_type_t;

/** States of a call during setup procedure*/
typedef enum
{
    WICED_BT_HFP_HF_CALLSETUP_STATE_IDLE,     /**< No call set up in progress */
    WICED_BT_HFP_HF_CALLSETUP_STATE_INCOMING, /**< There is an incoming call */
    WICED_BT_HFP_HF_CALLSETUP_STATE_DIALING,  /**< Outgoing call is being setup up */
    WICED_BT_HFP_HF_CALLSETUP_STATE_ALERTING, /**< Remote party is being alterted of the call */
    WICED_BT_HFP_HF_CALLSETUP_STATE_WAITING   /**< Incoming call is waiting (received when a call is already active) */
} wiced_bt_hfp_hf_callsetup_state_t;

/** In-band ring tone setting in AG*/
typedef enum
{
    WICED_BT_HFP_HF_INBAND_RING_DISABLED, /**< AG will not send ring-tone thru an audio connection, HF should use some means to alert the user of an incoming call */
    WICED_BT_HFP_HF_INBAND_RING_ENABLED   /**< AG will send the ring-tone thru an audio connection */
} wiced_bt_hfp_hf_inband_ring_state_t;

/** Audio input/output device on the HF Device */
typedef enum
{
    WICED_BT_HFP_HF_SPEAKER, /**< Refers to speaker on the HF Device */
    WICED_BT_HFP_HF_MIC      /**< Refers to microphone on the HF Device */
} wiced_bt_hfp_hf_volume_type_t;

/** Call action command */
typedef enum
{
    WICED_BT_HFP_HF_CALL_ACTION_DIAL,   /**< Place an outgoing call request */
    WICED_BT_HFP_HF_CALL_ACTION_ANSWER, /**< Answer an incoming call */
    WICED_BT_HFP_HF_CALL_ACTION_HANGUP, /**< Hangup an active call, reject an incoming call, end an outgoing call (which is being setup) */
    WICED_BT_HFP_HF_CALL_ACTION_HOLD_0, /**< Release a held call, or reject a waiting call (UDUB) */
    WICED_BT_HFP_HF_CALL_ACTION_HOLD_1, /**< Release active call and activate a held or waiting call */
    WICED_BT_HFP_HF_CALL_ACTION_HOLD_2, /**< Place active call on hold, and accept a waiting call or retrieve a held call */
} wiced_bt_hfp_hf_call_action_t;


/******************************************************
 *                 Type Definitions
 ******************************************************/

/******************************************************
 *                    Structures
 ******************************************************/

/** Call State event data */
typedef struct
{
    wiced_bt_hfp_hf_callsetup_state_t setup_state;         /**< Call setup progress indicator */
    wiced_bool_t                      held_call_present;   /**< TRUE if a held call is present, else FALSE */
    wiced_bool_t                      active_call_present; /**< TRUE if an active call is present, else FALSE */
} wiced_bt_hfp_hf_call_data_t;

/** Volume Change event data */
typedef struct
{
    wiced_bt_hfp_hf_volume_type_t type;  /**< Whether HF volume change is being requested for mic or spkr */
    uint8_t                       level; /**< Volume level from 0 to 15 */
} wiced_bt_hfp_hf_volume_data_t;

typedef struct
{
    uint8_t  mic_volume;                   /**< Default/initial mic volume level from 0 to 15 */
    uint8_t  speaker_volume;               /**< Default/initial speaker volume level from 0 to 15 */
    uint32_t feature_mask;                 /**< HFP HF features supported bitmask - A combination of wiced_bt_hfp_hf_supported_features_t values */
    uint8_t  num_server;                   /**< Number of HFP HF server to start during init */
    uint8_t  scn[WICED_BT_HFP_HF_MAX_CONN];/**< Array of num_server HFP HF server channel number. This should be the same as configured in the SDP server */
} wiced_bt_hfp_hf_config_data_t;

typedef struct
{
    wiced_bt_hfp_hf_caller_num_t caller_num;
    uint8_t                      type;
} wiced_bt_hfp_hf_clip_data_t;

/** HF Event Data */
typedef struct
{
    wiced_bt_device_address_t                remote_address;    /**< BD address of the remote device */
    union
    {
        wiced_bt_hfp_hf_connection_state_t   conn_state;       /**< Payload for WICED_BT_HFP_HF_CONNECTION_STATE_EVT */
        uint32_t                             ag_feature_flags; /**< Payload for WICED_BT_HFP_HF_AG_FEATURE_SUPPORT_EVT */
        wiced_bt_hfp_hf_service_state_t      service_state;    /**< Payload for WICED_BT_HFP_HF_SERVICE_STATE_EVT */
        wiced_bt_hfp_hf_service_type_t       service_type;     /**< Payload for WICED_BT_HFP_HF_SERVICE_TYPE_EVT */
        wiced_bt_hfp_hf_call_data_t          call_data;        /**< Payload for WICED_BT_HFP_HF_CALL_STATE_EVT */
        wiced_bt_hfp_hf_inband_ring_state_t  inband_ring;      /**< Payload for WICED_BT_HFP_HF_INBAND_RING_STATE_EVT */
        uint8_t                              rssi;             /**< Payload for WICED_BT_HFP_HF_RSSI_IND_EVT */
        uint8_t                              battery_level;    /**< Payload for WICED_BT_HFP_HF_BATTERY_STATUS_IND_EVT */
        wiced_bt_hfp_hf_volume_data_t        volume;           /**< Payload for WICED_BT_HFP_HF_VOLUME_CHANGE_EVT */
        wiced_bt_hfp_hf_clip_data_t          clip;             /**< Paylaod for WICED_BT_HFP_HF_CLIP_IND_EVT */
        wiced_bt_hfp_hf_at_result_code_t     at_result_code;   /**< Payload for WICED_BT_HFP_HF_AT_RESP_NOTIFICATION_EVT */
    };
}wiced_bt_hfp_hf_event_data_t;

/******************************************************
 *                 Callback Type Definitions
 ******************************************************/

/** HF control path callback type
 *
 *  Application implements callback of this type to receive HF events and commands.
 *
 *  @param event    HF event.
 *  @param p_data   pointer to event data.
 *
 *  @return none
 * */
typedef void (*wiced_bt_hfp_hf_event_cb_t)( wiced_bt_hfp_hf_event_t event,
    wiced_bt_hfp_hf_event_data_t* p_data);

/******************************************************
 *               Function Declarations
 ******************************************************/

/** API to initialize the HFP-HF component and register with the stack.
 *
 *  Called by the application before any other API is called.
 *  Application provides the SINK configuration data and callback to receive control events.
 *
 *  @param num_server    Number of Handsfree server to be created
 *  @param a_config_data    HF configuration parameters array for each server. See wiced_bt_hfp_hf_config_data_t.
 *  @param event_cb         Callback function for receiving HF events.
 *
 *  @return wiced_result_t (WICED_BT_XXX)
 */
wiced_result_t wiced_bt_hfp_hf_init( wiced_bt_hfp_hf_config_data_t *p_config_data,
    wiced_bt_hfp_hf_event_cb_t event_cb );

/** API to deregister the HFP-HF component from the stack and to cleanup the internal data structures.
 *
 *  Called by the application when the HFP-HF component is no longer needed by it.
 *
 *  @return wiced_result_t (WICED_BT_XXX)
 */
wiced_result_t wiced_bt_hfp_hf_deinit( void );

/** API to initiate a HFP connection to an AG
 *
 *  Called by the application to connect to an AG with the given address.
 *
 *  @param bd_address   BD address of the AG.
 *
 *  @return wiced_result_t (WICED_BT_XXX)
 */
wiced_result_t wiced_bt_hfp_hf_connect( wiced_bt_device_address_t bd_address );

/** API to disconnect from an AG
 *
 *  Called by the application to disconnect from an AG with the given address.
 *
 *  @param bd_address   BD address of the AG.
 *
 *  @return wiced_result_t (WICED_BT_XXX)
 */
wiced_result_t wiced_bt_hfp_hf_disconnect( wiced_bt_device_address_t bd_address );

/** API to manipulate a call (i.e., to answer, hold, hangup, reject, etc)
 *
 *  Allows the application to take actions indicated in wiced_bt_hfp_hf_call_action_t.
 *
 *  @param bd_address   BD address of the AG
 *  @param action       Action to be initiated, see wiced_bt_hfp_hf_call_action_t.
 *  @param number       Contains a NUL terminated number to be called,
 *                      if NULL, the last number redial (LNR) is initiated.
 *                      valid when action is WICED_BT_HFP_HF_CALL_ACTION_DIAL,
 *                      for all other actions this is ignored.
 *
 *  @return wiced_result_t (WICED_BT_XXX)
 */
wiced_result_t wiced_bt_hfp_hf_perform_call_action( wiced_bt_device_address_t bd_address,
    wiced_bt_hfp_hf_call_action_t action, char* number );

/** API to send the current speaker/mic volume level to AG
 *
 *  Called by the application to notify the AG of the change in volume of mic or speaker.
 *
 *  @param bd_address   BD address of the AG.
 *  @param volume_type  Mic or speaker for which the volume was changed.
 *  @param volume_level Volume level from 0 to 15.
 *
 *  @return wiced_result_t (WICED_BT_XXX)
 */
wiced_result_t wiced_bt_hfp_hf_notify_volume( wiced_bt_device_address_t bd_address,
    wiced_bt_hfp_hf_volume_type_t volume_type, uint8_t volume_level );

/** API to send the at command to the AG
 *
 *  Called by the application to send an at command to AG. The command sent is pass thru
 *  for library. The response is received thru WICED_BT_HFP_HF_AT_RESULT_CODE_IND_EVT.
 *
 *  @param bd_address   BD address of the AG.
 *  @param at_cmd       Null terminated at command string to be sent to AG.
 *
 *  @return wiced_result_t (WICED_BT_XXX)
 */
wiced_result_t wiced_bt_hfp_hf_send_at_cmd( wiced_bt_device_address_t bd_address, char* at_cmd );

/** @} */ // end of hfp_hf

#ifdef __cplusplus
} /*extern "C" */
#endif

