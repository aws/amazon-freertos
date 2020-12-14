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
 * Bluetooth AVRCP Definition and Data Types
 *
 */
#pragma once

#ifdef __cplusplus
extern "C"
{
#endif

/*****************************************************************************
**  constants
*****************************************************************************/

/* Profile revision numbers */
#define AVRC_REV_1_0        0x0100
#define AVRC_REV_1_3        0x0103
#define AVRC_REV_1_4        0x0104
#define AVRC_REV_1_5        0x0105

#define AVRC_PACKET_LEN             512 /* Per the spec, you must support 512 byte RC packets */

#define AVRC_MIN_CONTROL_MTU        48  /* Per the spec, minimum MTU for the control channel */
#define AVRC_MIN_BROWSE_MTU         335 /* Per the spec, minimum MTU for the browsing channel */

#define AVRC_META_PDU_OFFSET        4
#define AVRC_SUB_TYPE_LEN           4
#define AVRC_UID_SIZE               8
#define AVRC_FEATURE_MASK_SIZE      16

/**
 * @anchor AVRC_CTYPE
 * @name AVRC message types
 * @{ */
#define AVRC_CMD_CTRL       0   /**< Instruct a target to perform an operation */
#define AVRC_CMD_STATUS     1   /**< Check a device’s current status */
#define AVRC_CMD_SPEC_INQ   2   /**< Check whether a target supports a particular
                                   control command; all operands are included */
#define AVRC_CMD_NOTIF      3   /**< Used for receiving notification of a change in a device’s state */
#define AVRC_CMD_GEN_INQ    4   /**< Check whether a target supports a particular
                                   control command; operands are not included */

/* response type codes */
#define AVRC_RSP_NOT_IMPL   8   /**< The target does not implement the command specified
                                   by the opcode and operand,
                                   or doesn’t implement the specified subunit */
#define AVRC_RSP_ACCEPT     9   /**< The target executed or is executing the command */
#define AVRC_RSP_REJ        10  /**< The target implements the command specified by the
                                   opcode but cannot respond because the current state
                                   of the target doesn’t allow it */
#define AVRC_RSP_IN_TRANS   11  /**< The target implements the status command but it is
                                   in a state of transition; the status command may
                                   be retried at a future time */
#define AVRC_RSP_IMPL_STBL  12  /**< For specific inquiry or general inquiy commands,
                                   the target implements the command; for status
                                   commands, the target returns stable and includes
                                   the status results */
#define AVRC_RSP_CHANGED    13  /**< The response frame contains a notification that the
                                   target device’s state has changed */
#define AVRC_RSP_INTERIM    15  /**< For control commands, the target has accepted the
                                   request but cannot return information within 100
                                   milliseconds; for notify commands, the target accepted
                                   the command, and will notify the controller of a change
                                   of target state at a future time */
/** @} AVRC_CTYPE */


/**
 * @anchor AVRC_SUBUNIT_TYPE
 * @name AVRC subunit types
 * @{ */
#define AVRC_SUB_MONITOR    0x00    /**< Monitor */
#define AVRC_SUB_AUDIO      0x01    /**< Audio */
#define AVRC_SUB_PRINTER    0x02    /**< Printer */
#define AVRC_SUB_DISC       0x03    /**< Disc */
#define AVRC_SUB_TAPE       0x04    /**< Tape recorder/player */
#define AVRC_SUB_TUNER      0x05    /**< Tuner */
#define AVRC_SUB_CA         0x06    /**< CA */
#define AVRC_SUB_CAMERA     0x07    /**< Camera */
#define AVRC_SUB_PANEL      0x09    /**< Panel */
#define AVRC_SUB_BB         0x0A    /**< Bulletin Board */
#define AVRC_SUB_CAM_STOR   0x0B    /**< Camera Storage */
#define AVRC_SUB_VENDOR     0x1C    /**< Vendor unique */
#define AVRC_SUB_EXT        0x1E    /**< Subunit type extended to next byte */
#define AVRC_SUB_UNIT       0x1F    /**< Unit */
/** @} AVRC_SUBUNIT_TYPE */

/**
 * @anchor AVRC_OPCODES
 * @name AVRC message opcodes (defined by 1394ta).
 * @{
 */
#define AVRC_OP_UNIT_INFO   0x30    /**< Report unit information */
#define AVRC_OP_SUB_INFO    0x31    /**< Report subunit information */
#define AVRC_OP_VENDOR      0x00    /**< Vendor-dependent commands */
#define AVRC_OP_PASS_THRU   0x7C    /**< panel subunit opcode */
/* opcodes 80-9F and E0-FF are not used by 1394ta.Sneak one for the browsing channel */
#define AVRC_OP_BROWSE      0xFF    /**< Browsing */
#define AVRC_OP_INVALID     0xFE    /**< Invalid  */
/** @} AVRC_OPCODES */


/**
 * @anchor AVRC_COMPANY_ID
 * @name Company IDs
 * @{ */
#define AVRC_CO_BLUETOOTH_SIG   0x00FFFFFF  /**< Bluetooth SIG */
#define AVRC_CO_WIDCOMM         0x00000361  /**< Widcomm Inc. */
#define AVRC_CO_BROADCOM        0x00001018  /**< Broadcom Corporation */
#define AVRC_CO_METADATA        0x00001958  /**< AVRC metadata messages */
/** @} AVRC_COMPANY_ID */

/* State flag for Passthrough commands
*/
#define AVRC_STATE_PRESS    0
#define AVRC_STATE_RELEASE  1

/* Operation ID list for Passthrough commands
*/
#define AVRC_ID_SELECT      0x00    /* select */
#define AVRC_ID_UP          0x01    /* up */
#define AVRC_ID_DOWN        0x02    /* down */
#define AVRC_ID_LEFT        0x03    /* left */
#define AVRC_ID_RIGHT       0x04    /* right */
#define AVRC_ID_RIGHT_UP    0x05    /* right-up */
#define AVRC_ID_RIGHT_DOWN  0x06    /* right-down */
#define AVRC_ID_LEFT_UP     0x07    /* left-up */
#define AVRC_ID_LEFT_DOWN   0x08    /* left-down */
#define AVRC_ID_ROOT_MENU   0x09    /* root menu */
#define AVRC_ID_SETUP_MENU  0x0A    /* setup menu */
#define AVRC_ID_CONT_MENU   0x0B    /* contents menu */
#define AVRC_ID_FAV_MENU    0x0C    /* favorite menu */
#define AVRC_ID_EXIT        0x0D    /* exit */
#define AVRC_ID_0           0x20    /* 0 */
#define AVRC_ID_1           0x21    /* 1 */
#define AVRC_ID_2           0x22    /* 2 */
#define AVRC_ID_3           0x23    /* 3 */
#define AVRC_ID_4           0x24    /* 4 */
#define AVRC_ID_5           0x25    /* 5 */
#define AVRC_ID_6           0x26    /* 6 */
#define AVRC_ID_7           0x27    /* 7 */
#define AVRC_ID_8           0x28    /* 8 */
#define AVRC_ID_9           0x29    /* 9 */
#define AVRC_ID_DOT         0x2A    /* dot */
#define AVRC_ID_ENTER       0x2B    /* enter */
#define AVRC_ID_CLEAR       0x2C    /* clear */
#define AVRC_ID_CHAN_UP     0x30    /* channel up */
#define AVRC_ID_CHAN_DOWN   0x31    /* channel down */
#define AVRC_ID_PREV_CHAN   0x32    /* previous channel */
#define AVRC_ID_SOUND_SEL   0x33    /* sound select */
#define AVRC_ID_INPUT_SEL   0x34    /* input select */
#define AVRC_ID_DISP_INFO   0x35    /* display information */
#define AVRC_ID_HELP        0x36    /* help */
#define AVRC_ID_PAGE_UP     0x37    /* page up */
#define AVRC_ID_PAGE_DOWN   0x38    /* page down */
#define AVRC_ID_POWER       0x40    /* power */
#define AVRC_ID_VOL_UP      0x41    /* volume up */
#define AVRC_ID_VOL_DOWN    0x42    /* volume down */
#define AVRC_ID_MUTE        0x43    /* mute */
#define AVRC_ID_PLAY        0x44    /* play */
#define AVRC_ID_STOP        0x45    /* stop */
#define AVRC_ID_PAUSE       0x46    /* pause */
#define AVRC_ID_RECORD      0x47    /* record */
#define AVRC_ID_REWIND      0x48    /* rewind */
#define AVRC_ID_FAST_FOR    0x49    /* fast forward */
#define AVRC_ID_EJECT       0x4A    /* eject */
#define AVRC_ID_FORWARD     0x4B    /* forward */
#define AVRC_ID_BACKWARD    0x4C    /* backward */
#define AVRC_ID_ANGLE       0x50    /* angle */
#define AVRC_ID_SUBPICT     0x51    /* subpicture */
#define AVRC_ID_F1          0x71    /* F1 */
#define AVRC_ID_F2          0x72    /* F2 */
#define AVRC_ID_F3          0x73    /* F3 */
#define AVRC_ID_F4          0x74    /* F4 */
#define AVRC_ID_F5          0x75    /* F5 */
#define AVRC_ID_VENDOR      0x7E    /* vendor unique */
#define AVRC_KEYPRESSED_RELEASE 0x80

/*****************************************************************************
**  Metadata transfer definitions
*****************************************************************************/

/* Define the Metadata Packet types
*/
#define AVRC_PKT_SINGLE           0
#define AVRC_PKT_START            1
#define AVRC_PKT_CONTINUE         2
#define AVRC_PKT_END              3
#define AVRC_PKT_TYPE_MASK        3

/* Define the PDUs carried in the vendor dependant data
*/
#define AVRC_PDU_GET_CAPABILITIES               0x10
#define AVRC_PDU_LIST_PLAYER_APP_ATTR           0x11
#define AVRC_PDU_LIST_PLAYER_APP_VALUES         0x12
#define AVRC_PDU_GET_CUR_PLAYER_APP_VALUE       0x13
#define AVRC_PDU_SET_PLAYER_APP_VALUE           0x14
#define AVRC_PDU_GET_PLAYER_APP_ATTR_TEXT       0x15
#define AVRC_PDU_GET_PLAYER_APP_VALUE_TEXT      0x16
#define AVRC_PDU_INFORM_DISPLAY_CHARSET         0x17
#define AVRC_PDU_INFORM_BATTERY_STAT_OF_CT      0x18
#define AVRC_PDU_GET_ELEMENT_ATTR               0x20
#define AVRC_PDU_GET_PLAY_STATUS                0x30
#define AVRC_PDU_REGISTER_NOTIFICATION          0x31
#define AVRC_PDU_REQUEST_CONTINUATION_RSP       0x40
#define AVRC_PDU_ABORT_CONTINUATION_RSP         0x41
/* added in 1.4 */
#define AVRC_PDU_SET_ABSOLUTE_VOLUME            0x50
#define AVRC_PDU_SET_ADDRESSED_PLAYER           0x60
#define AVRC_PDU_SET_BROWSED_PLAYER             0x70
#define AVRC_PDU_GET_FOLDER_ITEMS               0x71
#define AVRC_PDU_CHANGE_PATH                    0x72
#define AVRC_PDU_GET_ITEM_ATTRIBUTES            0x73
#define AVRC_PDU_PLAY_ITEM                      0x74
#define AVRC_PDU_GET_TOTAL_NUM_OF_ITEMS         0x75        /* Added in post 1.5 */
#define AVRC_PDU_SEARCH                         0x80
#define AVRC_PDU_ADD_TO_NOW_PLAYING             0x90
#define AVRC_PDU_GENERAL_REJECT                 0xA0

/* Define the vendor unique id carried in the pass through data
*/
#define AVRC_PDU_NEXT_GROUP                     0x00
#define AVRC_PDU_PREV_GROUP                     0x01
/* the only pass through vendor unique commands defined by AVRC is the group navigation commands
 * The len for vendor unique data is 5 */
#define AVRC_PASS_THRU_GROUP_LEN                5

/**
 * @anchor AVRC_STS
 * @name AVRC status codes.
 * @{
 */
 /* 6.15.3 error status code for general reject */
#define AVRC_STS_BAD_CMD        0x00    /**< Invalid command, sent if TG received a PDU that it did not understand. */
#define AVRC_STS_BAD_PARAM      0x01    /**< Invalid parameter, sent if the TG received a PDU with a parameter ID that it did not understand. Sent if there is only one parameter ID in the PDU. */
#define AVRC_STS_NOT_FOUND      0x02    /**< Specified parameter not found., sent if the parameter ID is understood, but content is wrong or corrupted. */
#define AVRC_STS_INTERNAL_ERR   0x03    /**< Internal Error, sent if there are error conditions not covered by a more specific error code. */
#define AVRC_STS_NO_ERROR       0x04    /**< Operation completed without error.  This is the status that should be returned if the operation was successful. */
#define AVRC_STS_UID_CHANGED    0x05    /**< UID Changed - The UIDs on the device have changed */
/* #define AVRC_STS_GEN_ERROR      0x06    Unknown Error - this is changed to "reserved" */
#define AVRC_STS_BAD_DIR        0x07    /**< Invalid Direction - The Direction parameter is invalid - Change Path*/
#define AVRC_STS_NOT_DIR        0x08    /**< Not a Directory - The UID provided does not refer to a folder item  Change Path*/
#define AVRC_STS_NOT_EXIST      0x09    /**< Does Not Exist - The UID provided does not refer to any item    Change Path, PlayItem, AddToNowPlaying, GetItemAttributes*/
#define AVRC_STS_BAD_SCOPE      0x0a    /**< Invalid Scope - The scope parameter is invalid  GetFolderItems, PlayItem, AddToNowPlayer, GetItemAttributes, */
#define AVRC_STS_BAD_RANGE      0x0b    /**< Range Out of Bounds - The start of range provided is not valid  GetFolderItems*/
#define AVRC_STS_UID_IS_DIR     0x0c    /**< UID is a Directory - The UID provided refers to a directory, which cannot be handled by this media player   PlayItem, AddToNowPlaying */
#define AVRC_STS_IN_USE         0x0d    /**< Media in Use - The media is not able to be used for this operation at this time PlayItem, AddToNowPlaying */
#define AVRC_STS_NOW_LIST_FULL  0x0e    /**< Now Playing List Full - No more items can be added to the Now Playing List  AddToNowPlaying*/
#define AVRC_STS_SEARCH_NOT_SUP 0x0f    /**< Search Not Supported - The Browsed Media Player does not support search Search */
#define AVRC_STS_SEARCH_BUSY    0x10    /**< Search in Progress - A search operation is already in progress  Search*/
#define AVRC_STS_BAD_PLAYER_ID  0x11    /**< Invalid Player Id - The specified Player Id does not refer to a valid player    SetAddressedPlayer, SetBrowsedPlayer*/
#define AVRC_STS_PLAYER_N_BR    0x12    /**< Player Not Browsable - The Player Id supplied refers to a Media Player which does not support browsing. SetBrowsedPlayer */
#define AVRC_STS_PLAYER_N_ADDR  0x13    /**< Player Not Addressed.  The Player Id supplied refers to a player which is not currently addressed, and the command is not able to be performed if the player is not set as addressed.   Search, SetBrowsedPlayer*/
#define AVRC_STS_BAD_SEARCH_RES 0x14    /**< No valid Search Results - The Search result list does not contain valid entries, e.g. after being invalidated due to change of browsed player   GetFolderItems */
#define AVRC_STS_NO_AVAL_PLAYER 0x15    /**< No available players ALL */
#define AVRC_STS_ADDR_PLAYER_CHG 0x16   /**< Addressed Player Changed - Register Notification */
typedef uint8_t wiced_bt_avrc_sts_t;
/** @} AVRC_STS */

/* Define the Capability IDs
*/
#define AVRC_CAP_COMPANY_ID                     0x02
#define AVRC_CAP_EVENTS_SUPPORTED               0x03
#define AVRC_COMPANY_ID_LEN                     3
#define AVRC_CAPABILITY_OFFSET                  2

/* Define the Player Application Settings IDs
*/
#define AVRC_PLAYER_SETTING_EQUALIZER           0x01
#define AVRC_PLAYER_SETTING_REPEAT              0x02
#define AVRC_PLAYER_SETTING_SHUFFLE             0x03
#define AVRC_PLAYER_SETTING_SCAN                0x04
#define AVRC_PLAYER_SETTING_LOW_MENU_EXT        0x80
#define AVRC_PLAYER_SETTING_HIGH_MENU_EXT       0xff

/* Define the possible values of the Player Application Settings
*/
#define AVRC_PLAYER_VAL_OFF                     0x01
#define AVRC_PLAYER_VAL_ON                      0x02
#define AVRC_PLAYER_VAL_SINGLE_REPEAT           0x02
#define AVRC_PLAYER_VAL_ALL_REPEAT              0x03
#define AVRC_PLAYER_VAL_GROUP_REPEAT            0x04
#define AVRC_PLAYER_VAL_ALL_SHUFFLE             0x02
#define AVRC_PLAYER_VAL_GROUP_SHUFFLE           0x03
#define AVRC_PLAYER_VAL_ALL_SCAN                0x02
#define AVRC_PLAYER_VAL_GROUP_SCAN              0x03

/* Define the possible values of Battery Status PDU
*/
#define AVRC_BATTERY_STATUS_NORMAL              0x00
#define AVRC_BATTERY_STATUS_WARNING             0x01
#define AVRC_BATTERY_STATUS_CRITICAL            0x02
#define AVRC_BATTERY_STATUS_EXTERNAL            0x03
#define AVRC_BATTERY_STATUS_FULL_CHARGE         0x04
typedef uint8_t wiced_bt_avrc_battery_status_t;

/* Define character set */
#define AVRC_CHAR_SET_SIZE                      2

/* Define the Media Attribute IDs
*/
#define AVRC_MEDIA_ATTR_ID_TITLE                 0x00000001
#define AVRC_MEDIA_ATTR_ID_ARTIST                0x00000002
#define AVRC_MEDIA_ATTR_ID_ALBUM                 0x00000003
#define AVRC_MEDIA_ATTR_ID_TRACK_NUM             0x00000004
#define AVRC_MEDIA_ATTR_ID_NUM_TRACKS            0x00000005
#define AVRC_MEDIA_ATTR_ID_GENRE                 0x00000006
#define AVRC_MEDIA_ATTR_ID_PLAYING_TIME          0x00000007        /* in miliseconds */
#define AVRC_MAX_NUM_MEDIA_ATTR_ID               7

/* Define the possible values of play state
*/
#define AVRC_PLAYSTATE_RESP_MSG_SIZE            9
#define AVRC_PLAYSTATE_STOPPED                  0x00    /* Stopped */
#define AVRC_PLAYSTATE_PLAYING                  0x01    /* Playing */
#define AVRC_PLAYSTATE_PAUSED                   0x02    /* Paused  */
#define AVRC_PLAYSTATE_FWD_SEEK                 0x03    /* Fwd Seek*/
#define AVRC_PLAYSTATE_REV_SEEK                 0x04    /* Rev Seek*/
#define AVRC_PLAYSTATE_ERROR                    0xFF    /* Error   */
typedef uint8_t wiced_bt_avrc_playstate_t;

/* Define the events that can be registered for notifications
*/
#define AVRC_EVT_PLAY_STATUS_CHANGE             0x01
#define AVRC_EVT_TRACK_CHANGE                   0x02
#define AVRC_EVT_TRACK_REACHED_END              0x03
#define AVRC_EVT_TRACK_REACHED_START            0x04
#define AVRC_EVT_PLAY_POS_CHANGED               0x05
#define AVRC_EVT_BATTERY_STATUS_CHANGE          0x06
#define AVRC_EVT_SYSTEM_STATUS_CHANGE           0x07
#define AVRC_EVT_APP_SETTING_CHANGE             0x08
/* added in AVRCP 1.4 */
#define AVRC_EVT_NOW_PLAYING_CHANGE             0x09
#define AVRC_EVT_AVAL_PLAYERS_CHANGE            0x0a
#define AVRC_EVT_ADDR_PLAYER_CHANGE             0x0b
#define AVRC_EVT_UIDS_CHANGE                    0x0c
#define AVRC_EVT_VOLUME_CHANGE                  0x0d

/* the number of events that can be registered for notifications */
#define AVRC_NUM_NOTIF_EVENTS                   0x0d

#define AVRC_EVT_MSG_LEN_1                      0x01
#define AVRC_EVT_MSG_LEN_2                      0x02
#define AVRC_EVT_MSG_LEN_5                      0x05
#define AVRC_EVT_MSG_LEN_9                      0x09

#define AVRC_MAX_VOLUME                         0x7F

/* Define the possible values of system status
*/
#define AVRC_SYSTEMSTATE_PWR_ON                 0x00
#define AVRC_SYSTEMSTATE_PWR_OFF                0x01
#define AVRC_SYSTEMSTATE_PWR_UNPLUGGED          0x02
typedef uint8_t wiced_bt_avrc_systemstate_t;

/* the frequently used character set ids */
#define AVRC_CHARSET_ID_ASCII                  ((uint16_t) 0x0003) /* ASCII */
#define AVRC_CHARSET_ID_UTF8                   ((uint16_t) 0x006a) /* UTF-8 */
#define AVRC_CHARSET_ID_UTF16                  ((uint16_t) 0x03f7) /* 1015 */
#define AVRC_CHARSET_ID_UTF32                  ((uint16_t) 0x03f9) /* 1017 */

/*****************************************************************************
**  Advanced Control
*****************************************************************************/
#define AVRC_ITEM_PLAYER            0x01
#define AVRC_ITEM_FOLDER            0x02
#define AVRC_ITEM_MEDIA             0x03

#define AVRC_SCOPE_PLAYER_LIST      0x00  /* Media Player Item - Contains all available media players */
#define AVRC_SCOPE_FILE_SYSTEM      0x01  /* Folder Item, Media Element Item
                                             - The virtual filesystem containing the media content of the browsed player */
#define AVRC_SCOPE_SEARCH           0x02  /* Media Element Item  The results of a search operation on the browsed player */
#define AVRC_SCOPE_NOW_PLAYING      0x03  /* Media Element Item  The Now Playing list (or queue) of the addressed player */

#define AVRC_FOLDER_ITEM_COUNT_NONE 0xFF

/* folder type */
#define AVRC_FOLDER_TYPE_MIXED      0x00
#define AVRC_FOLDER_TYPE_TITLES     0x01
#define AVRC_FOLDER_TYPE_ALNUMS     0x02
#define AVRC_FOLDER_TYPE_ARTISTS    0x03
#define AVRC_FOLDER_TYPE_GENRES     0x04
#define AVRC_FOLDER_TYPE_PLAYLISTS  0x05
#define AVRC_FOLDER_TYPE_YEARS      0x06

/* major player type */
#define AVRC_MJ_TYPE_AUDIO          0x01  /* Audio */
#define AVRC_MJ_TYPE_VIDEO          0x02  /* Video */
#define AVRC_MJ_TYPE_BC_AUDIO       0x04  /* Broadcasting Audio */
#define AVRC_MJ_TYPE_BC_VIDEO       0x08  /* Broadcasting Video */
#define AVRC_MJ_TYPE_INVALID        0xF0

/* player sub type */
#define AVRC_SUB_TYPE_NONE          0x00
#define AVRC_SUB_TYPE_AUDIO_BOOK    0x01  /* Audio Book */
#define AVRC_SUB_TYPE_PODCAST       0x02  /* Podcast */
#define AVRC_SUB_TYPE_INVALID       0xFC

/* media item - media type */
#define AVRC_MEDIA_TYPE_AUDIO       0x00
#define AVRC_MEDIA_TYPE_VIDEO       0x01

#define AVRC_DIR_UP                 0x00  /* Folder Up */
#define AVRC_DIR_DOWN               0x01  /* Folder Down */

#define AVRC_UID_SIZE               8
typedef uint8_t wiced_bt_avrc_uid_t[AVRC_UID_SIZE];

/*****************************************************************************
**  player attribute - supported features
*****************************************************************************/
#define AVRC_PF_SELECT_BIT_NO           0
#define AVRC_PF_SELECT_MASK             0x01
#define AVRC_PF_SELECT_OFF              0
#define AVRC_PF_SELECT_SUPPORTED(x)     ((x)[AVRC_PF_SELECT_OFF] & AVRC_PF_SELECT_MASK)

#define AVRC_PF_UP_BIT_NO               1
#define AVRC_PF_UP_MASK                 0x02
#define AVRC_PF_UP_OFF                  0
#define AVRC_PF_UP_SUPPORTED(x)         ((x)[AVRC_PF_UP_OFF] & AVRC_PF_UP_MASK)

#define AVRC_PF_DOWN_BIT_NO             2
#define AVRC_PF_DOWN_MASK               0x04
#define AVRC_PF_DOWN_OFF                0
#define AVRC_PF_DOWN_SUPPORTED(x)       ((x)[AVRC_PF_DOWN_OFF] & AVRC_PF_DOWN_MASK)

#define AVRC_PF_LEFT_BIT_NO             3
#define AVRC_PF_LEFT_MASK               0x08
#define AVRC_PF_LEFT_OFF                0
#define AVRC_PF_LEFT_SUPPORTED(x)       ((x)[AVRC_PF_LEFT_OFF] & AVRC_PF_LEFT_MASK)

#define AVRC_PF_RIGHT_BIT_NO            4
#define AVRC_PF_RIGHT_MASK              0x10
#define AVRC_PF_RIGHT_OFF               0
#define AVRC_PF_RIGHT_SUPPORTED(x)      ((x)[AVRC_PF_RIGHT_OFF] & AVRC_PF_RIGHT_MASK)

#define AVRC_PF_RIGHTUP_BIT_NO          5
#define AVRC_PF_RIGHTUP_MASK            0x20
#define AVRC_PF_RIGHTUP_OFF             0
#define AVRC_PF_RIGHTUP_SUPPORTED(x)    ((x)[AVRC_PF_RIGHTUP_OFF] & AVRC_PF_RIGHTUP_MASK)

#define AVRC_PF_RIGHTDOWN_BIT_NO        6
#define AVRC_PF_RIGHTDOWN_MASK          0x40
#define AVRC_PF_RIGHTDOWN_OFF           0
#define AVRC_PF_RIGHTDOWN_SUPPORTED(x)  ((x)[AVRC_PF_RIGHTDOWN_OFF] & AVRC_PF_RIGHTDOWN_MASK)

#define AVRC_PF_LEFTUP_BIT_NO           7
#define AVRC_PF_LEFTUP_MASK             0x80
#define AVRC_PF_LEFTUP_OFF              0
#define AVRC_PF_LEFTUP_SUPPORTED(x)     ((x)[AVRC_PF_LEFTUP_OFF] & AVRC_PF_LEFTUP_MASK)

#define AVRC_PF_LEFTDOWN_BIT_NO         8
#define AVRC_PF_LEFTDOWN_MASK           0x01
#define AVRC_PF_LEFTDOWN_OFF            1
#define AVRC_PF_LEFTDOWN_SUPPORTED(x)   ((x)[AVRC_PF_LEFTDOWN_OFF] & AVRC_PF_LEFTDOWN_MASK)

#define AVRC_PF_ROOT_MENU_BIT_NO        9
#define AVRC_PF_ROOT_MENU_MASK          0x02
#define AVRC_PF_ROOT_MENU_OFF           1
#define AVRC_PF_ROOT_MENU_SUPPORTED(x)  ((x)[AVRC_PF_ROOT_MENU_OFF] & AVRC_PF_ROOT_MENU_MASK)

#define AVRC_PF_SETUP_MENU_BIT_NO       10
#define AVRC_PF_SETUP_MENU_MASK         0x04
#define AVRC_PF_SETUP_MENU_OFF          1
#define AVRC_PF_SETUP_MENU_SUPPORTED(x) ((x)[AVRC_PF_SETUP_MENU_OFF] & AVRC_PF_SETUP_MENU_MASK)

#define AVRC_PF_CONTENTS_MENU_BIT_NO    11
#define AVRC_PF_CONTENTS_MENU_MASK      0x08
#define AVRC_PF_CONTENTS_MENU_OFF       1
#define AVRC_PF_CONTENTS_MENU_SUPPORTED(x)  ((x)[AVRC_PF_CONTENTS_MENU_OFF] & AVRC_PF_CONTENTS_MENU_MASK)

#define AVRC_PF_FAVORITE_MENU_BIT_NO    12
#define AVRC_PF_FAVORITE_MENU_MASK      0x10
#define AVRC_PF_FAVORITE_MENU_OFF       1
#define AVRC_PF_FAVORITE_MENU_SUPPORTED(x)  ((x)[AVRC_PF_FAVORITE_MENU_OFF] & AVRC_PF_FAVORITE_MENU_MASK)

#define AVRC_PF_EXIT_BIT_NO             13
#define AVRC_PF_EXIT_MASK               0x20
#define AVRC_PF_EXIT_OFF                1
#define AVRC_PF_EXIT_SUPPORTED(x)       ((x)[AVRC_PF_EXIT_OFF] & AVRC_PF_EXIT_MASK)

#define AVRC_PF_0_BIT_NO                14
#define AVRC_PF_0_MASK                  0x40
#define AVRC_PF_0_OFF                   1
#define AVRC_PF_0_SUPPORTED(x)          ((x)[AVRC_PF_0_OFF] & AVRC_PF_0_MASK)

#define AVRC_PF_1_BIT_NO                15
#define AVRC_PF_1_MASK                  0x80
#define AVRC_PF_1_OFF                   1
#define AVRC_PF_1_SUPPORTED(x)          ((x)[AVRC_PF_1_OFF] & AVRC_PF_1_MASK)

#define AVRC_PF_2_BIT_NO                16
#define AVRC_PF_2_MASK                  0x01
#define AVRC_PF_2_OFF                   2
#define AVRC_PF_2_SUPPORTED(x)          ((x)[AVRC_PF_2_OFF] & AVRC_PF_2_MASK)

#define AVRC_PF_3_BIT_NO                17
#define AVRC_PF_3_MASK                  0x02
#define AVRC_PF_3_OFF                   2
#define AVRC_PF_3_SUPPORTED(x)          ((x)[AVRC_PF_3_OFF] & AVRC_PF_3_MASK)

#define AVRC_PF_4_BIT_NO                18
#define AVRC_PF_4_MASK                  0x04
#define AVRC_PF_4_OFF                   2
#define AVRC_PF_4_SUPPORTED(x)          ((x)[AVRC_PF_4_OFF] & AVRC_PF_4_MASK)

#define AVRC_PF_5_BIT_NO                19
#define AVRC_PF_5_MASK                  0x08
#define AVRC_PF_5_OFF                   2
#define AVRC_PF_5_SUPPORTED(x)          ((x)[AVRC_PF_5_OFF] & AVRC_PF_5_MASK)

#define AVRC_PF_6_BIT_NO                20
#define AVRC_PF_6_MASK                  0x10
#define AVRC_PF_6_OFF                   2
#define AVRC_PF_6_SUPPORTED(x)          ((x)[AVRC_PF_6_OFF] & AVRC_PF_6_MASK)

#define AVRC_PF_7_BIT_NO                21
#define AVRC_PF_7_MASK                  0x20
#define AVRC_PF_7_OFF                   2
#define AVRC_PF_7_SUPPORTED(x)          ((x)[AVRC_PF_7_OFF] & AVRC_PF_7_MASK)

#define AVRC_PF_8_BIT_NO                22
#define AVRC_PF_8_MASK                  0x40
#define AVRC_PF_8_OFF                   2
#define AVRC_PF_8_SUPPORTED(x)          ((x)[AVRC_PF_8_OFF] & AVRC_PF_8_MASK)

#define AVRC_PF_9_BIT_NO                23
#define AVRC_PF_9_MASK                  0x80
#define AVRC_PF_9_OFF                   2
#define AVRC_PF_9_SUPPORTED(x)          ((x)[AVRC_PF_9_OFF] & AVRC_PF_9_MASK)

#define AVRC_PF_DOT_BIT_NO              24
#define AVRC_PF_DOT_MASK                0x01
#define AVRC_PF_DOT_OFF                 3
#define AVRC_PF_DOT_SUPPORTED(x)        ((x)[AVRC_PF_DOT_OFF] & AVRC_PF_DOT_MASK)

#define AVRC_PF_ENTER_BIT_NO            25
#define AVRC_PF_ENTER_MASK              0x02
#define AVRC_PF_ENTER_OFF               3
#define AVRC_PF_ENTER_SUPPORTED(x)      ((x)[AVRC_PF_ENTER_OFF] & AVRC_PF_ENTER_MASK)

#define AVRC_PF_CLEAR_BIT_NO            26
#define AVRC_PF_CLEAR_MASK              0x04
#define AVRC_PF_CLEAR_OFF               3
#define AVRC_PF_CLEAR_SUPPORTED(x)      ((x)[AVRC_PF_CLEAR_OFF] & AVRC_PF_CLEAR_MASK)

#define AVRC_PF_CHNL_UP_BIT_NO          27
#define AVRC_PF_CHNL_UP_MASK            0x08
#define AVRC_PF_CHNL_UP_OFF             3
#define AVRC_PF_CHNL_UP_SUPPORTED(x)    ((x)[AVRC_PF_CHNL_UP_OFF] & AVRC_PF_CHNL_UP_MASK)

#define AVRC_PF_CHNL_DOWN_BIT_NO        28
#define AVRC_PF_CHNL_DOWN_MASK          0x10
#define AVRC_PF_CHNL_DOWN_OFF           3
#define AVRC_PF_CHNL_DOWN_SUPPORTED(x)  ((x)[AVRC_PF_CHNL_DOWN_OFF] & AVRC_PF_CHNL_DOWN_MASK)

#define AVRC_PF_PREV_CHNL_BIT_NO        29
#define AVRC_PF_PREV_CHNL_MASK          0x20
#define AVRC_PF_PREV_CHNL_OFF           3
#define AVRC_PF_PREV_CHNL_SUPPORTED(x)  ((x)[AVRC_PF_PREV_CHNL_OFF] & AVRC_PF_PREV_CHNL_MASK)

#define AVRC_PF_SOUND_SEL_BIT_NO        30
#define AVRC_PF_SOUND_SEL_MASK          0x40
#define AVRC_PF_SOUND_SEL_OFF           3
#define AVRC_PF_SOUND_SEL_SUPPORTED(x)  ((x)[AVRC_PF_SOUND_SEL_OFF] & AVRC_PF_SOUND_SEL_MASK)

#define AVRC_PF_INPUT_SEL_BIT_NO        31
#define AVRC_PF_INPUT_SEL_MASK          0x80
#define AVRC_PF_INPUT_SEL_OFF           3
#define AVRC_PF_INPUT_SEL_SUPPORTED(x)  ((x)[AVRC_PF_INPUT_SEL_OFF] & AVRC_PF_INPUT_SEL_MASK)

#define AVRC_PF_DISP_INFO_BIT_NO        32
#define AVRC_PF_DISP_INFO_MASK          0x01
#define AVRC_PF_DISP_INFO_OFF           4
#define AVRC_PF_DISP_INFO_SUPPORTED(x)  ((x)[AVRC_PF_DISP_INFO_OFF] & AVRC_PF_DISP_INFO_MASK)

#define AVRC_PF_HELP_BIT_NO             33
#define AVRC_PF_HELP_MASK               0x02
#define AVRC_PF_HELP_OFF                4
#define AVRC_PF_HELP_SUPPORTED(x)       ((x)[AVRC_PF_HELP_OFF] & AVRC_PF_HELP_MASK)

#define AVRC_PF_PAGE_UP_BIT_NO          34
#define AVRC_PF_PAGE_UP_MASK            0x04
#define AVRC_PF_PAGE_UP_OFF             4
#define AVRC_PF_PAGE_UP_SUPPORTED(x)    ((x)[AVRC_PF_PAGE_UP_OFF] & AVRC_PF_PAGE_UP_MASK)

#define AVRC_PF_PAGE_DOWN_BIT_NO        35
#define AVRC_PF_PAGE_DOWN_MASK          0x08
#define AVRC_PF_PAGE_DOWN_OFF           4
#define AVRC_PF_PAGE_DOWN_SUPPORTED(x)  ((x)[AVRC_PF_PAGE_DOWN_OFF] & AVRC_PF_PAGE_DOWN_MASK)

#define AVRC_PF_POWER_BIT_NO            36
#define AVRC_PF_POWER_MASK              0x10
#define AVRC_PF_POWER_OFF               4
#define AVRC_PF_POWER_SUPPORTED(x)      ((x)[AVRC_PF_POWER_OFF] & AVRC_PF_POWER_MASK)

#define AVRC_PF_VOL_UP_BIT_NO           37
#define AVRC_PF_VOL_UP_MASK             0x20
#define AVRC_PF_VOL_UP_OFF              4
#define AVRC_PF_VOL_UP_SUPPORTED(x)     ((x)[AVRC_PF_VOL_UP_OFF] & AVRC_PF_VOL_UP_MASK)

#define AVRC_PF_VOL_DOWN_BIT_NO         38
#define AVRC_PF_VOL_DOWN_MASK           0x40
#define AVRC_PF_VOL_DOWN_OFF            4
#define AVRC_PF_VOL_DOWN_SUPPORTED(x)   ((x)[AVRC_PF_VOL_DOWN_OFF] & AVRC_PF_VOL_DOWN_MASK)

#define AVRC_PF_MUTE_BIT_NO             39
#define AVRC_PF_MUTE_MASK               0x80
#define AVRC_PF_MUTE_OFF                4
#define AVRC_PF_MUTE_SUPPORTED(x)       ((x)[AVRC_PF_MUTE_OFF] & AVRC_PF_MUTE_MASK)

#define AVRC_PF_PLAY_BIT_NO             40
#define AVRC_PF_PLAY_MASK               0x01
#define AVRC_PF_PLAY_OFF                5
#define AVRC_PF_PLAY_SUPPORTED(x)       ((x)[AVRC_PF_PLAY_OFF] & AVRC_PF_PLAY_MASK)

#define AVRC_PF_STOP_BIT_NO             41
#define AVRC_PF_STOP_MASK               0x02
#define AVRC_PF_STOP_OFF                5
#define AVRC_PF_STOP_SUPPORTED(x)       ((x)[AVRC_PF_STOP_OFF] & AVRC_PF_STOP_MASK)

#define AVRC_PF_PAUSE_BIT_NO            42
#define AVRC_PF_PAUSE_MASK              0x04
#define AVRC_PF_PAUSE_OFF               5
#define AVRC_PF_PAUSE_SUPPORTED(x)      ((x)[AVRC_PF_PAUSE_OFF] & AVRC_PF_PAUSE_MASK)

#define AVRC_PF_RECORD_BIT_NO           43
#define AVRC_PF_RECORD_MASK             0x08
#define AVRC_PF_RECORD_OFF              5
#define AVRC_PF_RECORD_SUPPORTED(x)     ((x)[AVRC_PF_RECORD_OFF] & AVRC_PF_RECORD_MASK)

#define AVRC_PF_REWIND_BIT_NO           44
#define AVRC_PF_REWIND_MASK             0x10
#define AVRC_PF_REWIND_OFF              5
#define AVRC_PF_REWIND_SUPPORTED(x)     ((x)[AVRC_PF_REWIND_OFF] & AVRC_PF_REWIND_MASK)

#define AVRC_PF_FAST_FWD_BIT_NO         45
#define AVRC_PF_FAST_FWD_MASK           0x20
#define AVRC_PF_FAST_FWD_OFF            5
#define AVRC_PF_FAST_FWD_SUPPORTED(x)   ((x)[AVRC_PF_FAST_FWD_OFF] & AVRC_PF_FAST_FWD_MASK)

#define AVRC_PF_EJECT_BIT_NO            46
#define AVRC_PF_EJECT_MASK              0x40
#define AVRC_PF_EJECT_OFF               5
#define AVRC_PF_EJECT_SUPPORTED(x)      ((x)[AVRC_PF_EJECT_OFF] & AVRC_PF_EJECT_MASK)

#define AVRC_PF_FORWARD_BIT_NO          47
#define AVRC_PF_FORWARD_MASK            0x80
#define AVRC_PF_FORWARD_OFF             5
#define AVRC_PF_FORWARD_SUPPORTED(x)    ((x)[AVRC_PF_FORWARD_OFF] & AVRC_PF_FORWARD_MASK)

#define AVRC_PF_BACKWARD_BIT_NO         48
#define AVRC_PF_BACKWARD_MASK           0x01
#define AVRC_PF_BACKWARD_OFF            6
#define AVRC_PF_BACKWARD_SUPPORTED(x)   ((x)[AVRC_PF_BACKWARD_OFF] & AVRC_PF_BACKWARD_MASK)

#define AVRC_PF_ANGLE_BIT_NO            49
#define AVRC_PF_ANGLE_MASK              0x02
#define AVRC_PF_ANGLE_OFF               6
#define AVRC_PF_ANGLE_SUPPORTED(x)      ((x)[AVRC_PF_ANGLE_OFF] & AVRC_PF_ANGLE_MASK)

#define AVRC_PF_SUBPICTURE_BIT_NO       50
#define AVRC_PF_SUBPICTURE_MASK         0x04
#define AVRC_PF_SUBPICTURE_OFF          6
#define AVRC_PF_SUBPICTURE_SUPPORTED(x) ((x)[AVRC_PF_SUBPICTURE_OFF] & AVRC_PF_SUBPICTURE_MASK)

#define AVRC_PF_F1_BIT_NO               51
#define AVRC_PF_F1_MASK                 0x08
#define AVRC_PF_F1_OFF                  6
#define AVRC_PF_F1_SUPPORTED(x)         ((x)[AVRC_PF_F1_OFF] & AVRC_PF_F1_MASK)

#define AVRC_PF_F2_BIT_NO               52
#define AVRC_PF_F2_MASK                 0x10
#define AVRC_PF_F2_OFF                  6
#define AVRC_PF_F2_SUPPORTED(x)         ((x)[AVRC_PF_F2_OFF] & AVRC_PF_F2_MASK)

#define AVRC_PF_F3_BIT_NO               53
#define AVRC_PF_F3_MASK                 0x20
#define AVRC_PF_F3_OFF                  6
#define AVRC_PF_F3_SUPPORTED(x)         ((x)[AVRC_PF_F3_OFF] & AVRC_PF_F3_MASK)

#define AVRC_PF_F4_BIT_NO               54
#define AVRC_PF_F4_MASK                 0x40
#define AVRC_PF_F4_OFF                  6
#define AVRC_PF_F4_SUPPORTED(x)         ((x)[AVRC_PF_F4_OFF] & AVRC_PF_F4_MASK)

#define AVRC_PF_F5_BIT_NO               55
#define AVRC_PF_F5_MASK                 0x80
#define AVRC_PF_F5_OFF                  6
#define AVRC_PF_F5_SUPPORTED(x)         ((x)[AVRC_PF_F5_OFF] & AVRC_PF_F5_MASK)

/* Vendor unique. This PASSTHROUGH command is supported. */
#define AVRC_PF_VENDOR_BIT_NO           56
#define AVRC_PF_VENDOR_MASK             0x01
#define AVRC_PF_VENDOR_OFF              7
#define AVRC_PF_VENDOR_SUPPORTED(x)     ((x)[AVRC_PF_VENDOR_OFF] & AVRC_PF_VENDOR_MASK)

/* Basic Group Navigation.  This overrules the SDP entry as it is set per player.7 */
#define AVRC_PF_GROUP_NAVI_BIT_NO       57
#define AVRC_PF_GROUP_NAVI_MASK         0x02
#define AVRC_PF_GROUP_NAVI_OFF          7
#define AVRC_PF_GROUP_NAVI_SUPPORTED(x) ((x)[AVRC_PF_GROUP_NAVI_OFF] & AVRC_PF_GROUP_NAVI_MASK)

/* Advanced Control Player.  This bit is set if the player supports at least AVRCP 1.4. */
#define AVRC_PF_ADV_CTRL_BIT_NO         58
#define AVRC_PF_ADV_CTRL_MASK           0x04
#define AVRC_PF_ADV_CTRL_OFF            7
#define AVRC_PF_ADV_CTRL_SUPPORTED(x)   ((x)[AVRC_PF_ADV_CTRL_OFF] & AVRC_PF_ADV_CTRL_MASK)

/* Browsing.  This bit is set if the player supports browsing. */
#define AVRC_PF_BROWSE_BIT_NO           59
#define AVRC_PF_BROWSE_MASK             0x08
#define AVRC_PF_BROWSE_OFF              7
#define AVRC_PF_BROWSE_SUPPORTED(x)     ((x)[AVRC_PF_BROWSE_OFF] & AVRC_PF_BROWSE_MASK)

/* Searching. This bit is set if the player supports searching. */
#define AVRC_PF_SEARCH_BIT_NO           60
#define AVRC_PF_SEARCH_MASK             0x10
#define AVRC_PF_SEARCH_OFF              7
#define AVRC_PF_SEARCH_SUPPORTED(x)     ((x)[AVRC_PF_SEARCH_OFF] & AVRC_PF_SEARCH_MASK)

/* AddToNowPlaying.  This bit is set if the player supports the AddToNowPlaying command. */
#define AVRC_PF_ADD2NOWPLAY_BIT_NO      61
#define AVRC_PF_ADD2NOWPLAY_MASK        0x20
#define AVRC_PF_ADD2NOWPLAY_OFF         7
#define AVRC_PF_ADD2NOWPLAY_SUPPORTED(x) ((x)[AVRC_PF_ADD2NOWPLAY_OFF] & AVRC_PF_ADD2NOWPLAY_MASK)

/* UIDs unique in player browse tree.  This bit is set if the player is able to maintain unique UIDs across the player browse tree. */
#define AVRC_PF_UID_UNIQUE_BIT_NO       62
#define AVRC_PF_UID_UNIQUE_MASK         0x40
#define AVRC_PF_UID_UNIQUE_OFF          7
#define AVRC_PF_UID_UNIQUE_SUPPORTED(x) ((x)[AVRC_PF_UID_UNIQUE_OFF] & AVRC_PF_UID_UNIQUE_MASK)

/* OnlyBrowsableWhenAddressed.  This bit is set if the player is only able to be browsed when it is set as the Addressed Player. */
#define AVRC_PF_BR_WH_ADDR_BIT_NO       63
#define AVRC_PF_BR_WH_ADDR_MASK         0x80
#define AVRC_PF_BR_WH_ADDR_OFF          7
#define AVRC_PF_BR_WH_ADDR_SUPPORTED(x) ((x)[AVRC_PF_BR_WH_ADDR_OFF] & AVRC_PF_BR_WH_ADDR_MASK)

/* OnlySearchableWhenAddressed.  This bit is set if the player is only able to be searched when it is set as the Addressed player. */
#define AVRC_PF_SEARCH_WH_ADDR_BIT_NO   64
#define AVRC_PF_SEARCH_WH_ADDR_MASK     0x01
#define AVRC_PF_SEARCH_WH_ADDR_OFF      8
#define AVRC_PF_SEARCH_WH_ADDR_SUPPORTED(x) ((x)[AVRC_PF_SEARCH_WH_ADDR_OFF] & AVRC_PF_SEARCH_WH_ADDR_MASK)

/* NowPlaying.  This bit is set if the player supports the NowPlaying folder.  Note that for all players that support browsing this bit shall be set */
#define AVRC_PF_NOW_PLAY_BIT_NO         65
#define AVRC_PF_NOW_PLAY_MASK           0x02
#define AVRC_PF_NOW_PLAY_OFF            8
#define AVRC_PF_NOW_PLAY_SUPPORTED(x)   ((x)[AVRC_PF_NOW_PLAY_OFF] & AVRC_PF_NOW_PLAY_MASK)

/* UIDPersistency.  This bit is set if the Player is able to persist UID values between AVRCP Browse Reconnect */
#define AVRC_PF_UID_PERSIST_BIT_NO      66
#define AVRC_PF_UID_PERSIST_MASK        0x04
#define AVRC_PF_UID_PERSIST_OFF         8
#define AVRC_PF_UID_PERSIST_SUPPORTED(x) ((x)[AVRC_PF_UID_PERSIST_OFF] & AVRC_PF_UID_PERSIST_MASK)

/* NumberOfItems. This bit is set if the player supports the GetTotalNumberOfItems browsing command. */
#define AVRC_PF_GET_NUM_OF_ITEMS_BIT_NO    67
#define AVRC_PF_GET_NUM_OF_ITEMS_MASK      0x08
#define AVRC_PF_GET_NUM_OF_ITEMS_OFF       8
#define AVRC_PF_GET_NUM_OF_ITEMS_SUPPORTED(x) ((x)[AVRC_PF_GET_NUM_OF_ITEMS_OFF] & AVRC_PF_GET_NUM_OF_ITEMS_MASK)

/*****************************************************************************
**  data type definitions
*****************************************************************************/

/** AV/C message header */
typedef struct
{
    uint8_t   ctype;                        /**< Message type (see @ref AVRC_CTYPE "AVRC Message Types") */
    uint8_t   subunit_type;                 /**< Subunit type (see @ref AVRC_SUBUNIT_TYPE "AVRC subunit types") */
    uint8_t   subunit_id;                   /**< Subunit ID (typically ignored for AVRCP; except for VENDOR DEPENDENT messages (Value range: 0-7) */
    uint8_t   opcode;                       /**< Opcode (passthrough, vendor, etc) */
} wiced_bt_avrc_hdr_t;

/** UNIT INFO message. */
typedef struct
{
    wiced_bt_avrc_hdr_t hdr;                /**< Message header */
    uint32_t            company_id;         /**< Company identifier */
    uint8_t             unit_type;          /**< Unit type (see @ref AVRC_SUBUNIT_TYPE "AVRC subunit types") */
    uint8_t             unit;               /**< This value is vendor dependent and typically zero */
} wiced_bt_avrc_msg_unit_t;

/** SUBUNIT INFO message. */
typedef struct
{
    wiced_bt_avrc_hdr_t hdr;                /**< Message header */
    uint8_t             subunit_type[AVRC_SUB_TYPE_LEN];    /**< Array of subunit types (see @ref AVRC_SUBUNIT_TYPE "AVRC subunit types") */
    wiced_bool_t        panel;              /**< TRUE if the panel subunit type is in the subunit_type array, FALSE otherwise */
    uint8_t             page;               /**< Specifies which part of the subunit type table is returned. For AVRCP it is typically zero. Value range is 0-7 */
} wiced_bt_avrc_msg_sub_t;

/** VENDOR DEPENDENT message. */
typedef struct
{
    wiced_bt_avrc_hdr_t hdr;                /**< Message header */
    uint32_t            company_id;         /**< Company identifier */
    uint8_t             *p_vendor_data;     /**< Pointer to vendor dependent data */
    uint16_t            vendor_len;         /**< Length in bytes of vendor dependent data */
} wiced_bt_avrc_msg_vendor_t;

/** PASS THROUGH message */
typedef struct
{
    wiced_bt_avrc_hdr_t hdr;                /**< Message header (ctype, subunit_type, subunit_id unused) */
    uint8_t             op_id;              /**< Operation ID */
    uint8_t             state;              /**< Keypress state */
    uint8_t             *p_pass_data;       /**< Pointer to data (valid only when the op_id is AVRC_ID_VENDOR) */
    uint8_t             pass_len;           /**< Data length (valid only when the op_id is AVRC_ID_VENDOR) */
} wiced_bt_avrc_msg_pass_t;

/* Command/Response indicator. */
#define AVRC_CMD            0               /* Command message */
#define AVRC_RSP            2               /* Response message */

/** Browsing channel message */
typedef struct
{
    wiced_bt_avrc_hdr_t hdr;                /**< Message header (ctype=AVRC_CMD or AVRC_RSP; subunit_type, subunit_id unused) */
    uint8_t             *p_browse_data;     /**< Pointer to data */
    uint16_t            browse_len;         /**< Data length */
    BT_HDR              *p_browse_pkt;      /**< The GKI buffer received. Set to NULL, if the callback function wants to keep the buffer */
} wiced_bt_avrc_msg_browse_t;

/** AVRC message (dependent on message opcode) */
typedef union
{
    wiced_bt_avrc_hdr_t           hdr;      /*  Message header */
    wiced_bt_avrc_msg_unit_t      unit;     /**< UNIT INFO message (opcode AVRC_OP_UNIT_INFO) */
    wiced_bt_avrc_msg_sub_t       sub;      /**< SUBUNIT INFO message (opcode AVRC_OP_SUB_INFO) */
    wiced_bt_avrc_msg_vendor_t    vendor;   /**< VENDOR DEPENDENT message (opcode AVRC_OP_VENDOR) */
    wiced_bt_avrc_msg_pass_t      pass;     /**< PASS THROUGH message (opcode AVRC_OP_PASS_THRU) */
    wiced_bt_avrc_msg_browse_t    browse;   /**< Browsing channel message (opcode AVRC_OP_BROWSE) */
} wiced_bt_avrc_msg_t;

/* macros */
#define AVRC_IS_VALID_CAP_ID(a)           (((a == AVRC_CAP_COMPANY_ID) || (a == AVRC_CAP_EVENTS_SUPPORTED)) ? TRUE : FALSE)

#if (AVRC_ADV_CTRL_INCLUDED == TRUE)
#define AVRC_IS_VALID_EVENT_ID(a)           (((a >= AVRC_EVT_PLAY_STATUS_CHANGE) && \
                                              (a <= AVRC_EVT_VOLUME_CHANGE)) ? TRUE : FALSE)
#else /* AVRCP 1.3 */
#define AVRC_IS_VALID_EVENT_ID(a)           (((a >= AVRC_EVT_PLAY_STATUS_CHANGE) && \
                                              (a <= AVRC_EVT_APP_SETTING_CHANGE)) ? TRUE : FALSE)
#endif

#define AVRC_IS_VALID_ATTRIBUTE(a)          (((a <= AVRC_PLAYER_SETTING_SCAN) || \
                                               (a >= AVRC_PLAYER_SETTING_LOW_MENU_EXT)) ? TRUE : FALSE)


#define AVRC_IS_VALID_MEDIA_ATTRIBUTE(a)    ((a >= AVRC_MEDIA_ATTR_ID_TITLE) && \
                                             (a <= AVRC_MEDIA_ATTR_ID_PLAYING_TIME) ? TRUE : FALSE)

#define AVRC_IS_VALID_BATTERY_STATUS(a)    ((a <= AVRC_BATTERY_STATUS_FULL_CHARGE) ? TRUE : FALSE)

#define AVRC_IS_VALID_SYSTEM_STATUS(a)    ((a <= AVRC_SYSTEMSTATE_PWR_UNPLUGGED) ? TRUE : FALSE)

#define AVRC_IS_VALID_GROUP(a)    ((a <= AVRC_PDU_PREV_GROUP) ? TRUE : FALSE)

/* Company ID is 24-bit integer We can not use the macros in bt_types.h */
#define AVRC_CO_ID_TO_BE_STREAM(p, u32) {*(p)++ = (uint8_t)((u32) >> 16); *(p)++ = (uint8_t)((u32) >> 8); *(p)++ = (uint8_t)(u32); }
#define AVRC_BE_STREAM_TO_CO_ID(u32, p) {u32 = (((uint32_t)(*((p) + 2))) + (((uint32_t)(*((p) + 1))) << 8) + (((uint32_t)(*(p))) << 16)); (p) += 3;}

/*****************************************************************************
**  data type definitions
*****************************************************************************/
#define AVRC_MAX_APP_ATTR_SIZE      16
#define AVRC_MAX_CHARSET_SIZE       16
#define AVRC_MAX_ELEM_ATTR_SIZE     8


/*****************************************************************************
**  Metadata transfer Building/Parsing definitions
*****************************************************************************/

typedef struct {
    uint16_t              charset_id;
    uint16_t              str_len;
    uint8_t               *p_str;
} wiced_bt_avrc_full_name_t;

typedef struct {
    uint16_t              str_len;
    uint8_t               *p_str;
} wiced_bt_avrc_name_t;


#ifndef AVRC_CAP_MAX_NUM_COMP_ID
#define AVRC_CAP_MAX_NUM_COMP_ID    4
#endif

#ifndef AVRC_CAP_MAX_NUM_EVT_ID
#define AVRC_CAP_MAX_NUM_EVT_ID     16
#endif

typedef union
{
    uint32_t  company_id[AVRC_CAP_MAX_NUM_COMP_ID];
    uint8_t   event_id[AVRC_CAP_MAX_NUM_EVT_ID];
} wiced_bt_avrc_caps_param_t;

typedef struct
{
    uint8_t   attr_id;
    uint8_t   attr_val;
} wiced_bt_avrc_app_setting_t;

typedef struct
{
    uint8_t   attr_id;
    uint16_t  charset_id;
    uint8_t   str_len;
    uint8_t   *p_str;
} wiced_bt_avrc_app_setting_text_t;

typedef uint8_t wiced_bt_avrc_feature_mask_t[AVRC_FEATURE_MASK_SIZE];

typedef struct
{
    uint16_t                        player_id;      /* A unique identifier for this media player.*/
    uint8_t                         major_type;     /* Use AVRC_MJ_TYPE_AUDIO, AVRC_MJ_TYPE_VIDEO, AVRC_MJ_TYPE_BC_AUDIO, or AVRC_MJ_TYPE_BC_VIDEO.*/
    uint32_t                        sub_type;       /* Use AVRC_SUB_TYPE_NONE, AVRC_SUB_TYPE_AUDIO_BOOK, or AVRC_SUB_TYPE_PODCAST*/
    uint8_t                         play_status;    /* Use AVRC_PLAYSTATE_STOPPED, AVRC_PLAYSTATE_PLAYING, AVRC_PLAYSTATE_PAUSED, AVRC_PLAYSTATE_FWD_SEEK,
                                                        AVRC_PLAYSTATE_REV_SEEK, or AVRC_PLAYSTATE_ERROR*/
    wiced_bt_avrc_feature_mask_t    features;       /* Supported feature bit mask*/
    wiced_bt_avrc_full_name_t       name;           /* The player name, name length and character set id.*/
} wiced_bt_avrc_item_player_t;

typedef struct
{
    wiced_bt_avrc_uid_t             uid;            /* The uid of this folder */
    uint8_t                         type;           /* Use AVRC_FOLDER_TYPE_MIXED, AVRC_FOLDER_TYPE_TITLES,
                                                        AVRC_FOLDER_TYPE_ALNUMS, AVRC_FOLDER_TYPE_ARTISTS, AVRC_FOLDER_TYPE_GENRES,
                                                        AVRC_FOLDER_TYPE_PLAYLISTS, or AVRC_FOLDER_TYPE_YEARS.*/
    wiced_bool_t                    playable;       /* TRUE, if the folder can be played. */
    wiced_bt_avrc_full_name_t       name;           /* The folder name, name length and character set id. */
} wiced_bt_avrc_item_folder_t;

typedef struct
{
    uint32_t                        attr_id;        /* Use AVRC_MEDIA_ATTR_ID_TITLE, AVRC_MEDIA_ATTR_ID_ARTIST, AVRC_MEDIA_ATTR_ID_ALBUM,
                                                        AVRC_MEDIA_ATTR_ID_TRACK_NUM, AVRC_MEDIA_ATTR_ID_NUM_TRACKS,
                                                        AVRC_MEDIA_ATTR_ID_GENRE, AVRC_MEDIA_ATTR_ID_PLAYING_TIME */
    wiced_bt_avrc_full_name_t       name;           /* The attribute value, value length and character set id. */
} wiced_bt_avrc_attr_entry_t;

typedef struct
{
    wiced_bt_avrc_uid_t             uid;            /* The uid of this media element item */
    uint8_t                         type;           /* Use AVRC_MEDIA_TYPE_AUDIO or AVRC_MEDIA_TYPE_VIDEO. */
    wiced_bt_avrc_full_name_t       name;           /* The media name, name length and character set id. */
    uint8_t                         attr_count;     /* The number of attributes in p_attr_list */
    wiced_bt_avrc_attr_entry_t*     p_attr_list;    /* Attribute entry list. */
} wiced_bt_avrc_item_media_t;

typedef struct
{
    uint8_t                         item_type;      /* AVRC_ITEM_PLAYER, AVRC_ITEM_FOLDER, or AVRC_ITEM_MEDIA */
    union
    {
        wiced_bt_avrc_item_player_t player;         /* The properties of a media player item.*/
        wiced_bt_avrc_item_folder_t folder;         /* The properties of a folder item.*/
        wiced_bt_avrc_item_media_t  media;          /* The properties of a media item.*/
    } u;
} wiced_bt_avrc_item_t;

/** GetCapability */
typedef struct
{
    uint8_t                 pdu;
    wiced_bt_avrc_sts_t     status;
    uint8_t                 opcode;         /* Op Code (assigned by AVRC_BldCommand according to pdu) */
    uint8_t                 capability_id;
} wiced_bt_avrc_get_caps_cmd_t;

/** ListPlayerAppValues */
typedef struct
{
    uint8_t                 pdu;
    wiced_bt_avrc_sts_t     status;
    uint8_t                 opcode;         /* Op Code (assigned by AVRC_BldCommand according to pdu) */
    uint8_t                 attr_id;
} wiced_bt_avrc_list_app_values_cmd_t;

/** GetCurAppValue */
typedef struct
{
    uint8_t                 pdu;
    wiced_bt_avrc_sts_t     status;
    uint8_t                 opcode;         /* Op Code (assigned by AVRC_BldCommand according to pdu) */
    uint8_t                 num_attr;
    uint8_t                 attrs[AVRC_MAX_APP_ATTR_SIZE];
} wiced_bt_avrc_get_cur_app_value_cmd_t;

/** SetAppValue */
typedef struct
{
    uint8_t                     pdu;
    wiced_bt_avrc_sts_t         status;
    uint8_t                     opcode;         /* Op Code (assigned by AVRC_BldCommand according to pdu) */
    uint8_t                     num_val;
    wiced_bt_avrc_app_setting_t *p_vals;
} wiced_bt_avrc_set_app_value_cmd_t;

/** GetAppAttrTxt */
typedef struct
{
    uint8_t                 pdu;
    wiced_bt_avrc_sts_t     status;
    uint8_t                 opcode;         /* Op Code (assigned by AVRC_BldCommand according to pdu) */
    uint8_t                 num_attr;
    uint8_t                 attrs[AVRC_MAX_APP_ATTR_SIZE];
} wiced_bt_avrc_get_app_attr_txt_cmd_t;

/** GetAppValueTxt */
typedef struct
{
    uint8_t                 pdu;
    wiced_bt_avrc_sts_t     status;
    uint8_t                 opcode;         /* Op Code (assigned by AVRC_BldCommand according to pdu) */
    uint8_t                 attr_id;
    uint8_t                 num_val;
    uint8_t                 vals[AVRC_MAX_APP_ATTR_SIZE];
} wiced_bt_avrc_get_app_val_txt_cmd_t;

/** InformCharset */
typedef struct
{
    uint8_t                 pdu;
    wiced_bt_avrc_sts_t     status;
    uint8_t                 opcode;         /* Op Code (assigned by AVRC_BldCommand according to pdu) */
    uint8_t                 num_id;
    uint16_t                charsets[AVRC_MAX_CHARSET_SIZE];
} wiced_bt_avrc_inform_charset_cmd_t;

/** InformBatteryStatus */
typedef struct
{
    uint8_t                 pdu;
    wiced_bt_avrc_sts_t     status;
    uint8_t                 opcode;         /* Op Code (assigned by AVRC_BldCommand according to pdu) */
    uint8_t                 battery_status;
} wiced_bt_avrc_battery_status_cmd_t;

/** GetElemAttrs */
typedef struct
{
    uint8_t                 pdu;
    wiced_bt_avrc_sts_t     status;
    uint8_t                 opcode;         /* Op Code (assigned by AVRC_BldCommand according to pdu) */
    uint8_t                 num_attr;
    uint32_t                attrs[AVRC_MAX_ELEM_ATTR_SIZE];
} wiced_bt_avrc_get_elem_attrs_cmd_t;

/** RegNotify */
typedef struct
{
    uint8_t                 pdu;
    wiced_bt_avrc_sts_t     status;
    uint8_t                 opcode;         /* Op Code (assigned by AVRC_BldCommand according to pdu) */
    uint8_t                 event_id;
    uint32_t                param;
} wiced_bt_avrc_reg_notif_cmd_t;

/** SetAddrPlayer */
typedef struct
{
    uint8_t                 pdu;
    wiced_bt_avrc_sts_t     status;
    uint8_t                 opcode;         /* Op Code (assigned by AVRC_BldCommand according to pdu) */
    uint16_t                player_id;
} wiced_bt_avrc_set_addr_player_cmd_t;

/** SetBrowsedPlayer */
typedef struct
{
    uint8_t                 pdu;
    wiced_bt_avrc_sts_t     status;
    uint8_t                 opcode;         /* Op Code (assigned by AVRC_BldCommand according to pdu) */
    uint16_t                player_id;
} wiced_bt_avrc_set_br_player_cmd_t;

/** SetAbsVolume */
typedef struct
{
    uint8_t                 pdu;
    wiced_bt_avrc_sts_t     status;
    uint8_t                 opcode;         /* Op Code (assigned by AVRC_BldCommand according to pdu) */
    uint8_t                 volume;
} wiced_bt_avrc_set_volume_cmd_t;

/** GetFolderItems */
typedef struct
{
    uint8_t                 pdu;
    wiced_bt_avrc_sts_t     status;
    uint8_t                 opcode;         /* Op Code (assigned by AVRC_BldCommand according to pdu) */
    uint8_t                 scope;
    uint32_t                start_item;
    uint32_t                end_item;
    uint8_t                 attr_count;
    uint32_t                *p_attr_list;
} wiced_bt_avrc_get_items_cmd_t;

/** ChangePath */
typedef struct
{
    uint8_t                 pdu;
    wiced_bt_avrc_sts_t     status;
    uint8_t                 opcode;         /* Op Code (assigned by AVRC_BldCommand according to pdu) */
    uint16_t                uid_counter;
    uint8_t                 direction;
    wiced_bt_avrc_uid_t     folder_uid;
} wiced_bt_avrc_chg_path_cmd_t;

/** GetItemAttrs */
typedef struct
{
    uint8_t                 pdu;
    wiced_bt_avrc_sts_t     status;
    uint8_t                 opcode;         /* Op Code (assigned by AVRC_BldCommand according to pdu) */
    uint8_t                 scope;
    wiced_bt_avrc_uid_t     uid;
    uint16_t                uid_counter;
    uint8_t                 attr_count;
    uint32_t                *p_attr_list;
} wiced_bt_avrc_get_attrs_cmd_t;

/** Search */
typedef struct
{
    uint8_t                     pdu;
    wiced_bt_avrc_sts_t         status;
    uint8_t                     opcode;         /* Op Code (assigned by AVRC_BldCommand according to pdu) */
    wiced_bt_avrc_full_name_t   string;
} wiced_bt_avrc_search_cmd_t;

/** PlayItem */
typedef struct
{
    uint8_t                 pdu;
    wiced_bt_avrc_sts_t     status;
    uint8_t                 opcode;         /* Op Code (assigned by AVRC_BldCommand according to pdu) */
    uint8_t                 scope;
    wiced_bt_avrc_uid_t     uid;
    uint16_t                uid_counter;
} wiced_bt_avrc_play_item_cmd_t;

/** AddToNowPlaying */
typedef struct
{
    uint8_t                 pdu;
    wiced_bt_avrc_sts_t     status;
    uint8_t                 opcode;         /* Op Code (assigned by AVRC_BldCommand according to pdu) */
    uint8_t                 scope;
    wiced_bt_avrc_uid_t     uid;
    uint16_t                uid_counter;
} wiced_bt_avrc_add_to_play_cmd_t;

/** GetTotalNumOfItems */
typedef struct
{
    uint8_t                 pdu;
    wiced_bt_avrc_sts_t     status;
    uint8_t                 opcode;         /* Op Code (assigned by AVRC_BldCommand according to pdu) */
    uint8_t                 scope;
} wiced_bt_avrc_get_num_of_items_cmd_t;

/** Generic AVRC command */
typedef struct
{
    uint8_t                 pdu;
    wiced_bt_avrc_sts_t     status;
    uint8_t                 opcode;         /* Op Code (assigned by AVRC_BldCommand according to pdu) */
} wiced_bt_avrc_cmd_t;

/** Continue and Abort */
typedef struct
{
    uint8_t                 pdu;
    wiced_bt_avrc_sts_t     status;
    uint8_t                 opcode;         /* Op Code (assigned by AVRC_BldCommand according to pdu) */
    uint8_t                 target_pdu;
} wiced_bt_avrc_next_cmd_t;

/** AVRC commands */
typedef union
{
    uint8_t                               pdu;
    wiced_bt_avrc_cmd_t                   cmd;
    wiced_bt_avrc_get_caps_cmd_t          get_caps;               /**< GetCapability */
    wiced_bt_avrc_cmd_t                   list_app_attr;          /**< ListPlayerAppAttr */
    wiced_bt_avrc_list_app_values_cmd_t   list_app_values;        /**< ListPlayerAppValues */
    wiced_bt_avrc_get_cur_app_value_cmd_t get_cur_app_val;        /**< GetCurAppValue */
    wiced_bt_avrc_set_app_value_cmd_t     set_app_val;            /**< SetAppValue */
    wiced_bt_avrc_get_app_attr_txt_cmd_t  get_app_attr_txt;       /**< GetAppAttrTxt */
    wiced_bt_avrc_get_app_val_txt_cmd_t   get_app_val_txt;        /**< GetAppValueTxt */
    wiced_bt_avrc_inform_charset_cmd_t    inform_charset;         /**< InformCharset */
    wiced_bt_avrc_battery_status_cmd_t    inform_battery_status;  /**< InformBatteryStatus */
    wiced_bt_avrc_get_elem_attrs_cmd_t    get_elem_attrs;         /**< GetElemAttrs */
    wiced_bt_avrc_cmd_t                   get_play_status;        /**< GetPlayStatus */
    wiced_bt_avrc_reg_notif_cmd_t         reg_notif;              /**< RegNotify */
    wiced_bt_avrc_next_cmd_t              continu;                /**< Continue */
    wiced_bt_avrc_next_cmd_t              abort;                  /**< Abort */

    wiced_bt_avrc_set_addr_player_cmd_t   addr_player;            /**< SetAddrPlayer */
    wiced_bt_avrc_set_volume_cmd_t        volume;                 /**< SetAbsVolume */
    wiced_bt_avrc_set_br_player_cmd_t     br_player;              /**< SetBrowsedPlayer */
    wiced_bt_avrc_get_items_cmd_t         get_items;              /**< GetFolderItems */
    wiced_bt_avrc_chg_path_cmd_t          chg_path;               /**< ChangePath */
    wiced_bt_avrc_get_attrs_cmd_t         get_attrs;              /**< GetItemAttrs */
    wiced_bt_avrc_search_cmd_t            search;                 /**< Search */
    wiced_bt_avrc_play_item_cmd_t         play_item;              /**< PlayItem */
    wiced_bt_avrc_add_to_play_cmd_t       add_to_play;            /**< AddToNowPlaying */
    wiced_bt_avrc_get_num_of_items_cmd_t  get_num_of_items;       /**< GetTotalNumOfItems */
} wiced_bt_avrc_command_t;

/** GetCapability */
typedef struct
{
    uint8_t                     pdu;
    wiced_bt_avrc_sts_t         status;
    uint8_t                     opcode;         /* Op Code (copied from avrc_cmd.opcode by AVRC_BldResponse user. invalid one to generate according to pdu) */
    uint8_t                     capability_id;
    uint8_t                     count;
    wiced_bt_avrc_caps_param_t  param;
} wiced_bt_avrc_get_caps_rsp_t;

/** ListPlayerAppAttr */
typedef struct
{
    uint8_t                 pdu;
    wiced_bt_avrc_sts_t     status;
    uint8_t                 opcode;         /* Op Code (copied from avrc_cmd.opcode by AVRC_BldResponse user. invalid one to generate according to pdu) */
    uint8_t                 num_attr;
    uint8_t                 attrs[AVRC_MAX_APP_ATTR_SIZE];
} wiced_bt_avrc_list_app_attr_rsp_t;

/** ListPlayerAppValues */
typedef struct
{
    uint8_t                 pdu;
    wiced_bt_avrc_sts_t     status;
    uint8_t                 opcode;         /* Op Code (copied from avrc_cmd.opcode by AVRC_BldResponse user. invalid one to generate according to pdu) */
    uint8_t                 num_val;
    uint8_t                 vals[AVRC_MAX_APP_ATTR_SIZE];
} wiced_bt_avrc_list_app_values_rsp_t;

/** GetCurAppValue */
typedef struct
{
    uint8_t                     pdu;
    wiced_bt_avrc_sts_t         status;
    uint8_t                     opcode;         /* Op Code (copied from avrc_cmd.opcode by AVRC_BldResponse user. invalid one to generate according to pdu) */
    uint8_t                     num_val;
    wiced_bt_avrc_app_setting_t *p_vals;
} wiced_bt_avrc_get_cur_app_value_rsp_t;

/** GetAppAttrTxt */
typedef struct
{
    uint8_t                             pdu;
    wiced_bt_avrc_sts_t                 status;
    uint8_t                             opcode;         /* Op Code (copied from avrc_cmd.opcode by AVRC_BldResponse user. invalid one to generate according to pdu) */
    uint8_t                             num_attr;
    wiced_bt_avrc_app_setting_text_t    *p_attrs;
} wiced_bt_avrc_get_app_attr_txt_rsp_t;

/** GetElemAttrs */
typedef struct
{
    uint8_t                     pdu;
    wiced_bt_avrc_sts_t         status;
    uint8_t                     opcode;         /* Op Code (copied from avrc_cmd.opcode by AVRC_BldResponse user. invalid one to generate according to pdu) */
    uint8_t                     num_attr;
    wiced_bt_avrc_attr_entry_t  *p_attrs;
} wiced_bt_avrc_get_elem_attrs_rsp_t;

/** GetPlayStatus */
typedef struct
{
    uint8_t                 pdu;
    wiced_bt_avrc_sts_t     status;
    uint8_t                 opcode;         /* Op Code (copied from avrc_cmd.opcode by AVRC_BldResponse user. invalid one to generate according to pdu) */
    uint32_t                song_len;
    uint32_t                song_pos;
    uint8_t                 play_status;
} wiced_bt_avrc_get_play_status_rsp_t;

/** notification event parameter for AddressedPlayer change */
typedef struct
{
    uint16_t                player_id;
    uint16_t                uid_counter;
} wiced_bt_avrc_addr_player_param_t;

#ifndef AVRC_MAX_APP_SETTINGS
#define AVRC_MAX_APP_SETTINGS    8
#endif

/** notification event parameter for Player Application setting change */
typedef struct
{
    uint8_t                 num_attr;
    uint8_t                 attr_id[AVRC_MAX_APP_SETTINGS];
    uint8_t                 attr_value[AVRC_MAX_APP_SETTINGS];
} wiced_bt_avrc_player_app_param_t;

typedef union
{
    wiced_bt_avrc_playstate_t           play_status;
    wiced_bt_avrc_uid_t                 track;
    uint32_t                            play_pos;
    wiced_bt_avrc_battery_status_t      battery_status;
    wiced_bt_avrc_systemstate_t         system_status;
    wiced_bt_avrc_player_app_param_t    player_setting;
    wiced_bt_avrc_addr_player_param_t   addr_player;
    uint16_t                            uid_counter;
    uint8_t                             volume;
} wiced_bt_avrc_notif_rsp_param_t;

/** RegNotify */
typedef struct
{
    uint8_t                pdu;
    wiced_bt_avrc_sts_t    status;
    uint8_t                opcode;         /* Op Code (copied from avrc_cmd.opcode by AVRC_BldResponse user. invalid one to generate according to pdu) */
    uint8_t                event_id;
    wiced_bt_avrc_notif_rsp_param_t   param;
} wiced_bt_avrc_reg_notif_rsp_t;

/** SetAbsVolume */
typedef struct
{
    uint8_t                 pdu;
    wiced_bt_avrc_sts_t     status;
    uint8_t                 opcode;         /* Op Code (copied from avrc_cmd.opcode by AVRC_BldResponse user. invalid one to generate according to pdu) */
    uint8_t                 volume;
} wiced_bt_avrc_set_volume_rsp_t;

/** SetBrowsedPlayer */
typedef struct
{
    uint8_t                 pdu;
    wiced_bt_avrc_sts_t     status;
    uint8_t                 opcode;         /* Op Code (copied from avrc_cmd.opcode by AVRC_BldResponse user. invalid one to generate according to pdu) */
    uint16_t                uid_counter;
    uint32_t                num_items;
    uint16_t                charset_id;
    uint8_t                 folder_depth;
    wiced_bt_avrc_name_t    *p_folders;
} wiced_bt_avrc_set_br_player_rsp_t;

/** GetFolderItems */
typedef struct
{
    uint8_t                 pdu;
    wiced_bt_avrc_sts_t     status;
    uint8_t                 opcode;         /* Op Code (copied from avrc_cmd.opcode by AVRC_BldResponse user. invalid one to generate according to pdu) */
    uint16_t                uid_counter;
    uint16_t                item_count;
    wiced_bt_avrc_item_t    *p_item_list;
} wiced_bt_avrc_get_items_rsp_t;

/** ChangePath */
typedef struct
{
    uint8_t                 pdu;
    wiced_bt_avrc_sts_t     status;
    uint8_t                 opcode;         /* Op Code (copied from avrc_cmd.opcode by AVRC_BldResponse user. invalid one to generate according to pdu) */
    uint32_t                num_items;
} wiced_bt_avrc_chg_path_rsp_t;

/** GetItemAttrs */
typedef struct
{
    uint8_t                     pdu;
    wiced_bt_avrc_sts_t         status;
    uint8_t                     opcode;         /* Op Code (copied from avrc_cmd.opcode by AVRC_BldResponse user. invalid one to generate according to pdu) */
    uint8_t                     attr_count;
    wiced_bt_avrc_attr_entry_t  *p_attr_list;
} wiced_bt_avrc_get_attrs_rsp_t;

/** Get Total Number of Items */
typedef struct
{
    uint8_t                 pdu;
    wiced_bt_avrc_sts_t     status;
    uint8_t                 opcode;         /* Op Code (copied from avrc_cmd.opcode by AVRC_BldResponse user. invalid one to generate according to pdu) */
    uint16_t                uid_counter;
    uint32_t                num_items;
} wiced_bt_avrc_get_num_of_items_rsp_t;

/** Search */
typedef struct
{
    uint8_t                 pdu;
    wiced_bt_avrc_sts_t     status;
    uint8_t                 opcode;         /* Op Code (copied from avrc_cmd.opcode by AVRC_BldResponse user. invalid one to generate according to pdu) */
    uint16_t                uid_counter;
    uint32_t                num_items;
} wiced_bt_avrc_search_rsp_t;

/** Generic AVRC response */
typedef struct
{
    uint8_t                 pdu;
    wiced_bt_avrc_sts_t     status;
    uint8_t                 opcode;         /* Op Code (copied from avrc_cmd.opcode by AVRC_BldResponse user. invalid one to generate according to pdu) */
} wiced_bt_avrc_rsp_t;

/** AVRC response messages */
typedef union
{
    uint8_t                                   pdu;                    /**< PDU */
    wiced_bt_avrc_rsp_t                       rsp;                    /**< general response */
    wiced_bt_avrc_get_caps_rsp_t              get_caps;               /**< GetCapability */
    wiced_bt_avrc_list_app_attr_rsp_t         list_app_attr;          /**< ListPlayerAppAttr */
    wiced_bt_avrc_list_app_values_rsp_t       list_app_values;        /**< ListPlayerAppValues */
    wiced_bt_avrc_get_cur_app_value_rsp_t     get_cur_app_val;        /**< GetCurAppValue */
    wiced_bt_avrc_rsp_t                       set_app_val;            /**< SetAppValue */
    wiced_bt_avrc_get_app_attr_txt_rsp_t      get_app_attr_txt;       /**< GetAppAttrTxt */
    wiced_bt_avrc_get_app_attr_txt_rsp_t      get_app_val_txt;        /**< GetAppValueTxt */
    wiced_bt_avrc_rsp_t                       inform_charset;         /**< InformCharset */
    wiced_bt_avrc_rsp_t                       inform_battery_status;  /**< InformBatteryStatus */
    wiced_bt_avrc_get_elem_attrs_rsp_t        get_elem_attrs;         /**< GetElemAttrs */
    wiced_bt_avrc_get_play_status_rsp_t       get_play_status;        /**< GetPlayStatus */
    wiced_bt_avrc_reg_notif_rsp_t             reg_notif;              /**< RegNotify */
    wiced_bt_avrc_rsp_t                       continu;                /**< Continue */
    wiced_bt_avrc_rsp_t                       abort;                  /**< Abort */

    wiced_bt_avrc_rsp_t                       addr_player;            /**< SetAddrPlayer */
    wiced_bt_avrc_set_volume_rsp_t            volume;                 /**< SetAbsVolume */
    wiced_bt_avrc_set_br_player_rsp_t         br_player;              /**< SetBrowsedPlayer */
    wiced_bt_avrc_get_items_rsp_t             get_items;              /**< GetFolderItems */
    wiced_bt_avrc_chg_path_rsp_t              chg_path;               /**< ChangePath */
    wiced_bt_avrc_get_attrs_rsp_t             get_attrs;              /**< GetItemAttrs */
    wiced_bt_avrc_get_num_of_items_rsp_t      get_num_of_items;       /**< GetTotalNumberOfItems */
    wiced_bt_avrc_search_rsp_t                search;                 /**< Search */
    wiced_bt_avrc_rsp_t                       play_item;              /**< PlayItem */
    wiced_bt_avrc_rsp_t                       add_to_play;            /**< AddToNowPlaying */
} wiced_bt_avrc_response_t;


#ifdef __cplusplus
}
#endif
