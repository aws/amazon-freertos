/***************************************************************************//**
* \file <hci_control_api.h>
 *
 * HCI Control Protocol Definitions
 *
* \brief
* 	This file provides definitions for the WICED HCI Control Interface between an MCU
* 	and an embedded application running on a Cypress BT SoC. Please refer to the WICED
* 	HCI Control Protocol Software User Manual (002-16618) for additional details.
*
*//*****************************************************************************
* $ Copyright Cypress Semiconductor $
*******************************************************************************/
#ifndef __HCI_CONTROL_API_H
#define __HCI_CONTROL_API_H

/******************************************************************************
* Packets exchanged over the UART between MCU and hci_control applications contain a 5-byte header.
* -------------------------------------------------------------------------------------------------------
* |  Packet Type      | Command Code          |    Group Code       |        Packet Length              |
* -------------------------------------------------------------------------------------------------------
* |HCI_WICED_PKT(0x19)|HCI_CONTROL_COMMAND_...|HCI_CONTROL_GROUP_...|length(low byte)| length(high byte)|
* -------------------------------------------------------------------------------------------------------
******************************************************************************/

/* Packet types */
#define HCI_EVENT_PKT                                       4
#define HCI_ACL_DATA_PKT                                    2
#define HCI_WICED_PKT                                       25

/* Group codes */
#define HCI_CONTROL_GROUP_DEVICE                              0x00
#define HCI_CONTROL_GROUP_LE                                  0x01
#define HCI_CONTROL_GROUP_GATT                                0x02
#define HCI_CONTROL_GROUP_HF                                  0x03
#define HCI_CONTROL_GROUP_SPP                                 0x04
#define HCI_CONTROL_GROUP_AUDIO                               0x05
#define HCI_CONTROL_GROUP_HIDD                                0x06
#define HCI_CONTROL_GROUP_AVRC_TARGET                         0x07
#define HCI_CONTROL_GROUP_TEST                                0x08
#define HCI_CONTROL_GROUP_ANCS                                0x0b
#define HCI_CONTROL_GROUP_ALERT                               0x0c
#define HCI_CONTROL_GROUP_IAP2                                0x0d
#define HCI_CONTROL_GROUP_AG                                  0x0e
#define HCI_CONTROL_GROUP_BSG                                 0x10
#define HCI_CONTROL_GROUP_AVRC_CONTROLLER                     0x11
#define HCI_CONTROL_GROUP_AMS                                 0x12
#define HCI_CONTROL_GROUP_HIDH                                0x13
#define HCI_CONTROL_GROUP_AUDIO_SINK                          0x14
#define HCI_CONTROL_GROUP_PBC                                 0x15
#define HCI_CONTROL_GROUP_MESH                                0x16
#define HCI_CONTROL_GROUP_BATT_CLIENT                         0x17
#define HCI_CONTROL_GROUP_FINDME_LOCATOR                      0x18
#define HCI_CONTROL_GROUP_DEMO                                0x19
#define HCI_CONTROL_GROUP_OPS                                 0x20
#define HCI_CONTROL_GROUP_ANC                                 0x21
#define HCI_CONTROL_GROUP_ANS                                 0x22
#define HCI_CONTROL_GROUP_LE_COC                              0x23
#define HCI_CONTROL_GROUP_LED_DEMO                            0x24
#define HCI_CONTROL_GROUP_SCRIPT                              0x25
#define HCI_CONTROL_GROUP_OTP                                 0x26
#define HCI_CONTROL_GROUP_MISC                                0xFF

#define HCI_CONTROL_GROUP(x) ((((x) >> 8)) & 0xff)

/* General purpose commands */
#define HCI_CONTROL_COMMAND_RESET                           ( ( HCI_CONTROL_GROUP_DEVICE << 8 ) | 0x01 )    /* Restart controller */
#define HCI_CONTROL_COMMAND_TRACE_ENABLE                    ( ( HCI_CONTROL_GROUP_DEVICE << 8 ) | 0x02 )    /* Enable or disable WICED traces */
#define HCI_CONTROL_COMMAND_SET_LOCAL_BDA                   ( ( HCI_CONTROL_GROUP_DEVICE << 8 ) | 0x03 )    /* Set local device addrsss */
#define HCI_CONTROL_COMMAND_PUSH_NVRAM_DATA                 ( ( HCI_CONTROL_GROUP_DEVICE << 8 ) | 0x05 )    /* Download previously saved NVRAM chunk */
#define HCI_CONTROL_COMMAND_DELETE_NVRAM_DATA               ( ( HCI_CONTROL_GROUP_DEVICE << 8 ) | 0x06 )    /* Delete NVRAM chunk currently stored in RAM */
#define HCI_CONTROL_COMMAND_INQUIRY                         ( ( HCI_CONTROL_GROUP_DEVICE << 8 ) | 0x07 )    /* Start/stop inquiry */
#define HCI_CONTROL_COMMAND_SET_VISIBILITY                  ( ( HCI_CONTROL_GROUP_DEVICE << 8 ) | 0x08 )    /* Set BR/EDR connectability and discoverability of the device */
#define HCI_CONTROL_COMMAND_SET_PAIRING_MODE                ( ( HCI_CONTROL_GROUP_DEVICE << 8 ) | 0x09 )    /* Set Pairing Mode for the device 0 = Not pairable 1 = Pairable */
#define HCI_CONTROL_COMMAND_UNBOND                          ( ( HCI_CONTROL_GROUP_DEVICE << 8 ) | 0x0A )    /* Delete bond with specified BDADDR */
#define HCI_CONTROL_COMMAND_USER_CONFIRMATION               ( ( HCI_CONTROL_GROUP_DEVICE << 8 ) | 0x0B )    /* User Confirmation during pairing, TRUE/FALSE passed as parameter */
#define HCI_CONTROL_COMMAND_ENABLE_COEX                     ( ( HCI_CONTROL_GROUP_DEVICE << 8 ) | 0x0C )    /* Enable coex functionality */
#define HCI_CONTROL_COMMAND_DISABLE_COEX                    ( ( HCI_CONTROL_GROUP_DEVICE << 8 ) | 0x0D )    /* Disable coex functionality */
#define HCI_CONTROL_COMMAND_SET_BATTERY_LEVEL               ( ( HCI_CONTROL_GROUP_DEVICE << 8 ) | 0x0E )    /* Sets battery level in the GATT database */
#define HCI_CONTROL_COMMAND_READ_LOCAL_BDA                  ( ( HCI_CONTROL_GROUP_DEVICE << 8 ) | 0x0F )    /* Get local device addrsss */
#define HCI_CONTROL_COMMAND_BOND                            ( ( HCI_CONTROL_GROUP_DEVICE << 8 ) | 0x10 )    /* Initiate Bonding with a peer device */
#define HCI_CONTROL_COMMAND_READ_BUFF_STATS                 ( ( HCI_CONTROL_GROUP_DEVICE << 8 ) | 0x11 )    /* Read Buffer statistics */
#define HCI_CONTROL_COMMAND_SET_LOCAL_NAME                  ( ( HCI_CONTROL_GROUP_DEVICE << 8 ) | 0x12 )    /* Set the local name     */

/* LE Commands */
#define HCI_CONTROL_LE_COMMAND_SCAN                         ( ( HCI_CONTROL_GROUP_LE << 8 ) | 0x01 )    /* Start scan */
#define HCI_CONTROL_LE_COMMAND_ADVERTISE                    ( ( HCI_CONTROL_GROUP_LE << 8 ) | 0x02 )    /* Start advertisements */
#define HCI_CONTROL_LE_COMMAND_CONNECT                      ( ( HCI_CONTROL_GROUP_LE << 8 ) | 0x03 )    /* Connect to peer */
#define HCI_CONTROL_LE_COMMAND_CANCEL_CONNECT               ( ( HCI_CONTROL_GROUP_LE << 8 ) | 0x04 )    /* Cancel connect */
#define HCI_CONTROL_LE_COMMAND_DISCONNECT                   ( ( HCI_CONTROL_GROUP_LE << 8 ) | 0x05 )    /* Disconnect */
#define HCI_CONTROL_LE_RE_PAIR                              ( ( HCI_CONTROL_GROUP_LE << 8 ) | 0x06 )    /* Delete keys and then re-pair */
#define HCI_CONTROL_LE_COMMAND_GET_IDENTITY_ADDRESS         ( ( HCI_CONTROL_GROUP_LE << 8 ) | 0x07 )    /* Get identity address */
#define HCI_CONTROL_LE_COMMAND_SET_CHANNEL_CLASSIFICATION   ( ( HCI_CONTROL_GROUP_LE << 8 ) | 0x08 )    /* Set channel classification for the available 40 channels */
#define HCI_CONTROL_LE_COMMAND_SET_CONN_PARAMS              ( ( HCI_CONTROL_GROUP_LE << 8 ) | 0x09 )    /* Set connection parameters */
#define HCI_CONTROL_LE_COMMAND_SET_RAW_ADVERTISE_DATA       ( ( HCI_CONTROL_GROUP_LE << 8 ) | 0x0a )    /* Set raw advertisement data */

/* GATT Commands */
#define HCI_CONTROL_GATT_COMMAND_DISCOVER_SERVICES          ( ( HCI_CONTROL_GROUP_GATT << 8 ) | 0x01 )    /* Discover services */
#define HCI_CONTROL_GATT_COMMAND_DISCOVER_CHARACTERISTICS   ( ( HCI_CONTROL_GROUP_GATT << 8 ) | 0x02 )    /* Discover characteristics */
#define HCI_CONTROL_GATT_COMMAND_DISCOVER_DESCRIPTORS       ( ( HCI_CONTROL_GROUP_GATT << 8 ) | 0x03 )    /* Discover descriptors */
#define HCI_CONTROL_GATT_COMMAND_READ_REQUEST               ( ( HCI_CONTROL_GROUP_GATT << 8 ) | 0x04 )    /* Send read request */
#define HCI_CONTROL_GATT_COMMAND_READ_RESPONSE              ( ( HCI_CONTROL_GROUP_GATT << 8 ) | 0x05 )    /* Send read response */
#define HCI_CONTROL_GATT_COMMAND_WRITE_COMMAND              ( ( HCI_CONTROL_GROUP_GATT << 8 ) | 0x06 )    /* Send write command */
#define HCI_CONTROL_GATT_COMMAND_WRITE_REQUEST              ( ( HCI_CONTROL_GROUP_GATT << 8 ) | 0x07 )    /* Send write request */
#define HCI_CONTROL_GATT_COMMAND_WRITE_RESPONSE             ( ( HCI_CONTROL_GROUP_GATT << 8 ) | 0x08 )    /* Send write response */
#define HCI_CONTROL_GATT_COMMAND_NOTIFY                     ( ( HCI_CONTROL_GROUP_GATT << 8 ) | 0x09 )    /* Send notification */
#define HCI_CONTROL_GATT_COMMAND_INDICATE                   ( ( HCI_CONTROL_GROUP_GATT << 8 ) | 0x0a )    /* Send indication */
#define HCI_CONTROL_GATT_COMMAND_INDICATE_CONFIRM           ( ( HCI_CONTROL_GROUP_GATT << 8 ) | 0x0b )    /* Send indication confirmation */
#define HCI_CONTROL_GATT_COMMAND_DB_INIT                    ( ( HCI_CONTROL_GROUP_GATT << 8 ) | 0x0d )	  /* Initialize GATT database */
#define HCI_CONTROL_GATT_COMMAND_DB_PRIMARY_SERVICE_ADD     ( ( HCI_CONTROL_GROUP_GATT << 8 ) | 0x0e )	  /* Add primary service into GATT database */
#define HCI_CONTROL_GATT_COMMAND_DB_SECONDARY_SERVICE_ADD   ( ( HCI_CONTROL_GROUP_GATT << 8 ) | 0x0f )	  /* Add secondary service into GATT database */
#define HCI_CONTROL_GATT_COMMAND_DB_INCLUDED_SERVICE_ADD    ( ( HCI_CONTROL_GROUP_GATT << 8 ) | 0x10 )	  /* Add included service into GATT database */
#define HCI_CONTROL_GATT_COMMAND_DB_CHARACTERISTIC_ADD      ( ( HCI_CONTROL_GROUP_GATT << 8 ) | 0x11 )	  /* Add characteristic into GATT database */
#define HCI_CONTROL_GATT_COMMAND_DB_DESCRIPTOR_ADD          ( ( HCI_CONTROL_GROUP_GATT << 8 ) | 0x12 )	  /* Add descriptor into GATT database */

/* Handsfree Commands */
#define HCI_CONTROL_HF_COMMAND_CONNECT                      ( ( HCI_CONTROL_GROUP_HF << 8 ) | 0x01 )    /* Establish connection to HF Audio Gateway */
#define HCI_CONTROL_HF_COMMAND_DISCONNECT                   ( ( HCI_CONTROL_GROUP_HF << 8 ) | 0x02 )    /* Release HF connection */
#define HCI_CONTROL_HF_COMMAND_OPEN_AUDIO                   ( ( HCI_CONTROL_GROUP_HF << 8 ) | 0x03 )    /* Create audio connection on existing service level connection */
#define HCI_CONTROL_HF_COMMAND_CLOSE_AUDIO                  ( ( HCI_CONTROL_GROUP_HF << 8 ) | 0x04 )    /* Disconnect audio */
#define HCI_CONTROL_HF_COMMAND_AUDIO_ACCEPT_CONN            ( ( HCI_CONTROL_GROUP_HF << 8 ) | 0x05 )    /* Accept/Reject Audio connection request */
#define HCI_CONTROL_HF_COMMAND_TURN_OFF_PCM_CLK             ( ( HCI_CONTROL_GROUP_HF << 8 ) | 0x06 )    /* To turn off PCM/I2S clock in Master case for SCO */
#define HCI_CONTROL_HF_COMMAND_BUTTON_PRESS                 ( ( HCI_CONTROL_GROUP_HF << 8 ) | 0x07 )    /* Simulate a button press on a Hands-free headset */
#define HCI_CONTROL_HF_COMMAND_LONG_BUTTON_PRESS            ( ( HCI_CONTROL_GROUP_HF << 8 ) | 0x08 )    /* Simulate a long button press on a Hands-free headset */

/* Sub commands to send various AT Commands */
#define HCI_CONTROL_HF_AT_COMMAND_BASE                      ( ( HCI_CONTROL_GROUP_HF << 8 ) | 0x20 )    /* Send AT command and supporting data */
#define HCI_CONTROL_HF_AT_COMMAND_SPK                       0x00    /* Update speaker volume */
#define HCI_CONTROL_HF_AT_COMMAND_MIC                       0x01    /* Update microphone volume */
#define HCI_CONTROL_HF_AT_COMMAND_A                         0x02    /* Answer incoming call */
#define HCI_CONTROL_HF_AT_COMMAND_BINP                      0x03    /* Retrieve number from voice tag */
#define HCI_CONTROL_HF_AT_COMMAND_BVRA                      0x04    /* Enable/Disable voice recognition */
#define HCI_CONTROL_HF_AT_COMMAND_BLDN                      0x05    /* Last Number redial */
#define HCI_CONTROL_HF_AT_COMMAND_CHLD                      0x06    /* Call hold command */
#define HCI_CONTROL_HF_AT_COMMAND_CHUP                      0x07    /* Call hang up command */
#define HCI_CONTROL_HF_AT_COMMAND_CIND                      0x08    /* Read Indicator Status */
#define HCI_CONTROL_HF_AT_COMMAND_CNUM                      0x09    /* Retrieve Subscriber number */
#define HCI_CONTROL_HF_AT_COMMAND_D                         0x0A    /* Place a call using a number or memory dial */
#define HCI_CONTROL_HF_AT_COMMAND_NREC                      0x0B    /* Disable Noise reduction and echo canceling in AG */
#define HCI_CONTROL_HF_AT_COMMAND_VTS                       0x0C    /* Transmit DTMF tone */
#define HCI_CONTROL_HF_AT_COMMAND_BTRH                      0x0D    /* CCAP incoming call hold */
#define HCI_CONTROL_HF_AT_COMMAND_COPS                      0x0E    /* Query operator selection */
#define HCI_CONTROL_HF_AT_COMMAND_CMEE                      0x0F    /* Enable/disable extended AG result codes */
#define HCI_CONTROL_HF_AT_COMMAND_CLCC                      0x10    /* Query list of current calls in AG */
#define HCI_CONTROL_HF_AT_COMMAND_BIA                       0x11    /* Activate/Deactivate indicators */
#define HCI_CONTROL_HF_AT_COMMAND_BIEV                      0x12    /* Send HF indicator value to peer */
#define HCI_CONTROL_HF_AT_COMMAND_UNAT                      0x13    /* Transmit AT command not in the spec  */
#define HCI_CONTROL_HF_AT_COMMAND_MAX                       0x13    /* For command validation */

/* Serial Port Profile commands */
#define HCI_CONTROL_SPP_COMMAND_CONNECT                     ( ( HCI_CONTROL_GROUP_SPP << 8 ) | 0x01 )    /* Establish connection to SPP server */
#define HCI_CONTROL_SPP_COMMAND_DISCONNECT                  ( ( HCI_CONTROL_GROUP_SPP << 8 ) | 0x02 )    /* Release SPP connection */
#define HCI_CONTROL_SPP_COMMAND_DATA                        ( ( HCI_CONTROL_GROUP_SPP << 8 ) | 0x03 )    /* Send data */

/* Audio Profile commands */
#define HCI_CONTROL_AUDIO_COMMAND_CONNECT                   ( ( HCI_CONTROL_GROUP_AUDIO << 8 ) | 0x01 )    /* Audio connect to sink */
#define HCI_CONTROL_AUDIO_COMMAND_DISCONNECT                ( ( HCI_CONTROL_GROUP_AUDIO << 8 ) | 0x02 )    /* Audio disconnect  */
#define HCI_CONTROL_AUDIO_START                             ( ( HCI_CONTROL_GROUP_AUDIO << 8 ) | 0x03 )    /* Start audio with specific sample rate/mode */
#define HCI_CONTROL_AUDIO_STOP                              ( ( HCI_CONTROL_GROUP_AUDIO << 8 ) | 0x04 )    /* Stop audio */
#define HCI_CONTROL_AUDIO_PACKET_COUNT                      ( ( HCI_CONTROL_GROUP_AUDIO << 8 ) | 0x05 )    /* Debug packet counter sent from host */
#define HCI_CONTROL_AUDIO_DATA                              ( ( HCI_CONTROL_GROUP_AUDIO << 8 ) | 0x06 )    /* Audio data */
#define HCI_CONTROL_AUDIO_READ_STATISTICS                   ( ( HCI_CONTROL_GROUP_AUDIO << 8 ) | 0x07 )    /* Audio Statistics */

/* AVRC Target Profile commands */
#define HCI_CONTROL_AVRC_TARGET_COMMAND_CONNECT                     ( ( HCI_CONTROL_GROUP_AVRC_TARGET << 8 ) | 0x01 )    /* Initiate connection to the peer. */
#define HCI_CONTROL_AVRC_TARGET_COMMAND_DISCONNECT                  ( ( HCI_CONTROL_GROUP_AVRC_TARGET << 8 ) | 0x02 )    /* Disconnect connection to the peer. */
#define HCI_CONTROL_AVRC_TARGET_COMMAND_TRACK_INFO                  ( ( HCI_CONTROL_GROUP_AVRC_TARGET << 8 ) | 0x05 )    /* Track info sent to embedded app */
#define HCI_CONTROL_AVRC_TARGET_COMMAND_PLAYER_STATUS               ( ( HCI_CONTROL_GROUP_AVRC_TARGET << 8 ) | 0x06 )    /* Player status info sent to embedded app */
#define HCI_CONTROL_AVRC_TARGET_COMMAND_REPEAT_MODE_CHANGE          ( ( HCI_CONTROL_GROUP_AVRC_TARGET << 8 ) | 0x07 )    /* Repeat Mode changes sent to embedded app */
#define HCI_CONTROL_AVRC_TARGET_COMMAND_SHUFFLE_MODE_CHANGE         ( ( HCI_CONTROL_GROUP_AVRC_TARGET << 8 ) | 0x08 )    /* Shuffle Mode changes sent to embedded app */
#define HCI_CONTROL_AVRC_TARGET_COMMAND_EQUALIZER_STATUS_CHANGE     ( ( HCI_CONTROL_GROUP_AVRC_TARGET << 8 ) | 0x09 )    /* EQ Status changes sent to embedded app */
#define HCI_CONTROL_AVRC_TARGET_COMMAND_SCAN_STATUS_CHANGE          ( ( HCI_CONTROL_GROUP_AVRC_TARGET << 8 ) | 0x0A )    /* Scan Status changes sent to embedded app */

#define HCI_CONTROL_AVRC_TARGET_COMMAND_REGISTER_NOTIFICATION       ( ( HCI_CONTROL_GROUP_AVRC_TARGET << 8 ) | 0x99 )    /* Register for notifications (PTS only) */

/* AVRC Controller Profile commands */
#define HCI_CONTROL_AVRC_CONTROLLER_COMMAND_CONNECT                 ( ( HCI_CONTROL_GROUP_AVRC_CONTROLLER << 8 ) | 0x01 )    /* Initiate connection to the peer */
#define HCI_CONTROL_AVRC_CONTROLLER_COMMAND_DISCONNECT              ( ( HCI_CONTROL_GROUP_AVRC_CONTROLLER << 8 ) | 0x02 )    /* Disconnect from the peer */
#define HCI_CONTROL_AVRC_CONTROLLER_COMMAND_PLAY                    ( ( HCI_CONTROL_GROUP_AVRC_CONTROLLER << 8 ) | 0x03 )    /* Send play command to the player */
#define HCI_CONTROL_AVRC_CONTROLLER_COMMAND_STOP                    ( ( HCI_CONTROL_GROUP_AVRC_CONTROLLER << 8 ) | 0x04 )    /* Send stop command to the player */
#define HCI_CONTROL_AVRC_CONTROLLER_COMMAND_PAUSE                   ( ( HCI_CONTROL_GROUP_AVRC_CONTROLLER << 8 ) | 0x05 )    /* Send pause command to the player */
#define HCI_CONTROL_AVRC_CONTROLLER_COMMAND_BEGIN_FAST_FORWARD      ( ( HCI_CONTROL_GROUP_AVRC_CONTROLLER << 8 ) | 0x06 )    /* Start fast forward on the player */
#define HCI_CONTROL_AVRC_CONTROLLER_COMMAND_END_FAST_FORWARD        ( ( HCI_CONTROL_GROUP_AVRC_CONTROLLER << 8 ) | 0x07 )    /* End fast forward on the player */
#define HCI_CONTROL_AVRC_CONTROLLER_COMMAND_BEGIN_REWIND            ( ( HCI_CONTROL_GROUP_AVRC_CONTROLLER << 8 ) | 0x08 )    /* Passthrough Rewind command */
#define HCI_CONTROL_AVRC_CONTROLLER_COMMAND_END_REWIND              ( ( HCI_CONTROL_GROUP_AVRC_CONTROLLER << 8 ) | 0x09 )    /* Passthrough Rewind command */
#define HCI_CONTROL_AVRC_CONTROLLER_COMMAND_NEXT_TRACK              ( ( HCI_CONTROL_GROUP_AVRC_CONTROLLER << 8 ) | 0x0a )    /* Passthrough Next command */
#define HCI_CONTROL_AVRC_CONTROLLER_COMMAND_PREVIOUS_TRACK          ( ( HCI_CONTROL_GROUP_AVRC_CONTROLLER << 8 ) | 0x0b )    /* Passthrough Prev command */
#define HCI_CONTROL_AVRC_CONTROLLER_COMMAND_VOLUME_UP               ( ( HCI_CONTROL_GROUP_AVRC_CONTROLLER << 8 ) | 0x0c )    /* Passthrough Vol Up command */
#define HCI_CONTROL_AVRC_CONTROLLER_COMMAND_VOLUME_DOWN             ( ( HCI_CONTROL_GROUP_AVRC_CONTROLLER << 8 ) | 0x0d )    /* Passthrough Vol Down command */
#define HCI_CONTROL_AVRC_CONTROLLER_COMMAND_GET_TRACK_INFO          ( ( HCI_CONTROL_GROUP_AVRC_CONTROLLER << 8 ) | 0x0e )    /* Get Track Metadata */
#define HCI_CONTROL_AVRC_CONTROLLER_COMMAND_SET_EQUALIZER_STATUS    ( ( HCI_CONTROL_GROUP_AVRC_CONTROLLER << 8 ) | 0x0f )    /* Turn Equalizer On/Off */
#define HCI_CONTROL_AVRC_CONTROLLER_COMMAND_SET_REPEAT_MODE         ( ( HCI_CONTROL_GROUP_AVRC_CONTROLLER << 8 ) | 0x10 )    /* Set Repeat mode */
#define HCI_CONTROL_AVRC_CONTROLLER_COMMAND_SET_SHUFFLE_MODE        ( ( HCI_CONTROL_GROUP_AVRC_CONTROLLER << 8 ) | 0x11 )    /* Set Shuffle mode */
#define HCI_CONTROL_AVRC_CONTROLLER_COMMAND_SET_SCAN_STATUS         ( ( HCI_CONTROL_GROUP_AVRC_CONTROLLER << 8 ) | 0x12 )    /* Set Scan mode to Off, All tracks or Group scan  */
#define HCI_CONTROL_AVRC_CONTROLLER_COMMAND_VOLUME_LEVEL            ( ( HCI_CONTROL_GROUP_AVRC_CONTROLLER << 8 ) | 0x13 )    /* Set Absolute Volume */
#define HCI_CONTROL_AVRC_CONTROLLER_COMMAND_GET_PLAY_STATUS         ( ( HCI_CONTROL_GROUP_AVRC_CONTROLLER << 8 ) | 0x14 )    /* Get play-status command */
#define HCI_CONTROL_AVRC_CONTROLLER_COMMAND_POWER                   ( ( HCI_CONTROL_GROUP_AVRC_CONTROLLER << 8 ) | 0x15 )    /* Passthrough Power command */
#define HCI_CONTROL_AVRC_CONTROLLER_COMMAND_MUTE                    ( ( HCI_CONTROL_GROUP_AVRC_CONTROLLER << 8 ) | 0x16 )    /* Passthrough Mute command */
#define HCI_CONTROL_AVRC_CONTROLLER_COMMAND_BUTTON_PRESS            ( ( HCI_CONTROL_GROUP_AVRC_CONTROLLER << 8 ) | 0x17 )    /* Simulate a button press on a stereo headphone */
#define HCI_CONTROL_AVRC_CONTROLLER_COMMAND_LONG_BUTTON_PRESS       ( ( HCI_CONTROL_GROUP_AVRC_CONTROLLER << 8 ) | 0x18 )    /* Simulate a long button press on a stereo headphone */
#define HCI_CONTROL_AVRC_CONTROLLER_COMMAND_UNIT_INFO               ( ( HCI_CONTROL_GROUP_AVRC_CONTROLLER << 8 ) | 0x19 )    /* Send Unit Info command */
#define HCI_CONTROL_AVRC_CONTROLLER_COMMAND_SUB_UNIT_INFO           ( ( HCI_CONTROL_GROUP_AVRC_CONTROLLER << 8 ) | 0x1A )    /* Send Sub Unit Info command */

/* HID Device commands */
#define HCI_CONTROL_HIDD_COMMAND_ACCEPT_PAIRING             ( ( HCI_CONTROL_GROUP_HIDD << 8 ) | 0x01 )     /* Set device discoverable/connectable to accept pairing */
#define HCI_CONTROL_HIDD_COMMAND_SEND_REPORT                ( ( HCI_CONTROL_GROUP_HIDD << 8 ) | 0x02 )     /* Send HID report */
#define HCI_CONTROL_HIDD_COMMAND_CONNECT                    ( ( HCI_CONTROL_GROUP_HIDD << 8 ) | 0x04 )     /* Connect to previously paired host */
#define HCI_CONTROL_HIDD_COMMAND_HID_HOST_ADDR              ( ( HCI_CONTROL_GROUP_HIDD << 8 ) | 0x05 )     /* Paired host address */
#define HCI_CONTROL_HIDD_COMMAND_DISCONNECT                 ( ( HCI_CONTROL_GROUP_HIDD << 8 ) | 0x06 )     /* Disconnect HID connection */
#define HCI_CONTROL_HIDD_COMMAND_VIRTUAL_UNPLUG             ( ( HCI_CONTROL_GROUP_HIDD << 8 ) | 0x07 )     /* Send Virtual Unplug */

#define HCI_CONTROL_HID_COMMAND_ACCEPT_PAIRING              HCI_CONTROL_HIDD_COMMAND_ACCEPT_PAIRING
#define HCI_CONTROL_HID_COMMAND_SEND_REPORT                 HCI_CONTROL_HIDD_COMMAND_SEND_REPORT
#define HCI_CONTROL_HID_COMMAND_PUSH_PAIRING_HOST_INFO      ( ( HCI_CONTROL_GROUP_HIDD << 8 ) | 0x03 )     /* Paired host address and link keys */
#define HCI_CONTROL_HID_COMMAND_CONNECT                     HCI_CONTROL_HIDD_COMMAND_CONNECT

/* Test commands */
#define HCI_CONTROL_TEST_COMMAND_ENCAPSULATED_HCI_COMMAND   ( ( HCI_CONTROL_GROUP_TEST << 8 ) | 0x10 )     /* Encapsulated HCI command - For manufacturing test purposes */

/* ANCS commands */
#define HCI_CONTROL_ANCS_COMMAND_ACTION                     ( ( HCI_CONTROL_GROUP_ANCS << 8 ) | 0x01 )      /* ANCS notification */
#define HCI_CONTROL_ANCS_COMMAND_CONNECT                    ( ( HCI_CONTROL_GROUP_ANCS << 8 ) | 0x02 )      /* Establish ANCS connection */
#define HCI_CONTROL_ANCS_COMMAND_DISCONNECT                 ( ( HCI_CONTROL_GROUP_ANCS << 8 ) | 0x03 )      /* Disconnect ANCS */

/* ANC commands */
#define HCI_CONTROL_ANC_COMMAND_READ_SERVER_SUPPORTED_NEW_ALERTS        ( ( HCI_CONTROL_GROUP_ANC << 8 ) | 0x01 )   /* Command to read server supported new alerts */
#define HCI_CONTROL_ANC_COMMAND_READ_SERVER_SUPPORTED_UNREAD_ALERTS     ( ( HCI_CONTROL_GROUP_ANC << 8 ) | 0x02 )   /* Command to read server supported unread alerts */
#define HCI_CONTROL_ANC_COMMAND_CONTROL_ALERTS                          ( ( HCI_CONTROL_GROUP_ANC << 8 ) | 0x03 )   /* Command to enable/diable/notify pending or all pending alerts */
#define HCI_CONTROL_ANC_COMMAND_ENABLE_NEW_ALERTS                       ( ( HCI_CONTROL_GROUP_ANC << 8 ) | 0x04 )   /* Command to enable new alerts notifications */
#define HCI_CONTROL_ANC_COMMAND_ENABLE_UNREAD_ALERTS                    ( ( HCI_CONTROL_GROUP_ANC << 8 ) | 0x05 )   /* Command to enable unread alerts notifications */
#define HCI_CONTROL_ANC_COMMAND_DISABLE_NEW_ALERTS                      ( ( HCI_CONTROL_GROUP_ANC << 8 ) | 0x06 )   /* Command to disable new alerts notifications */
#define HCI_CONTROL_ANC_COMMAND_DISABLE_UNREAD_ALERTS                   ( ( HCI_CONTROL_GROUP_ANC << 8 ) | 0x07 )   /* Command to disable unread alerts notifications */

/* ANS commands */
#define HCI_CONTROL_ANS_COMMAND_SET_SUPPORTED_NEW_ALERT_CATEGORIES      ( ( HCI_CONTROL_GROUP_ANS << 8 ) | 0x01 ) /* Command to configure supported new alerts */
#define HCI_CONTROL_ANS_COMMAND_SET_SUPPORTED_UNREAD_ALERT_CATEGORIES   ( ( HCI_CONTROL_GROUP_ANS << 8 ) | 0x02 ) /* Command to configure supported unread alerts  */
#define HCI_CONTROL_ANS_COMMAND_GENERATE_ALERT                          ( ( HCI_CONTROL_GROUP_ANS << 8 ) | 0x03 ) /* Command to generate new alert and unread alert of requested category */
#define HCI_CONTROL_ANS_COMMAND_CLEAR_ALERT                             ( ( HCI_CONTROL_GROUP_ANS << 8 ) | 0x04 ) /* Command to clear new alert and unread alert count of requested category */

/* AMS commands */
#define HCI_CONTROL_AMS_COMMAND_CONNECT                     ( ( HCI_CONTROL_GROUP_AMS << 8 ) | 0x01 )       /* Establish AMS connection */
#define HCI_CONTROL_AMS_COMMAND_DISCONNECT                  ( ( HCI_CONTROL_GROUP_AMS << 8 ) | 0x02 )       /* Disconnect AMS */

/* IAP2 commands */
#define HCI_CONTROL_IAP2_COMMAND_CONNECT                     ( ( HCI_CONTROL_GROUP_IAP2 << 8 ) | 0x01 )    /* Establish connection to SPP server */
#define HCI_CONTROL_IAP2_COMMAND_DISCONNECT                  ( ( HCI_CONTROL_GROUP_IAP2 << 8 ) | 0x02 )    /* Release SPP connection */
#define HCI_CONTROL_IAP2_COMMAND_DATA                        ( ( HCI_CONTROL_GROUP_IAP2 << 8 ) | 0x03 )    /* Send data */
#define HCI_CONTROL_IAP2_COMMAND_GET_AUTH_CHIP_INFO          ( ( HCI_CONTROL_GROUP_IAP2 << 8 ) | 0x04 )    /* Send get auth chip info */
#define HCI_CONTROL_IAP2_COMMAND_GET_AUTH_CHIP_CERTIFICATE   ( ( HCI_CONTROL_GROUP_IAP2 << 8 ) | 0x05 )    /* Send get auth chip info */
#define HCI_CONTROL_IAP2_COMMAND_GET_AUTH_CHIP_SIGNATURE     ( ( HCI_CONTROL_GROUP_IAP2 << 8 ) | 0x06 )    /* Send get auth chip info */

/* Handsfree AG commands */
#define HCI_CONTROL_AG_COMMAND_CONNECT                      ( ( HCI_CONTROL_GROUP_AG << 8 ) | 0x01 )    /* Establish connection to HF Device */
#define HCI_CONTROL_AG_COMMAND_DISCONNECT                   ( ( HCI_CONTROL_GROUP_AG << 8 ) | 0x02 )    /* Release HF connection */
#define HCI_CONTROL_AG_COMMAND_OPEN_AUDIO                   ( ( HCI_CONTROL_GROUP_AG << 8 ) | 0x03 )    /* Create audio connection on existing service level connection */
#define HCI_CONTROL_AG_COMMAND_CLOSE_AUDIO                  ( ( HCI_CONTROL_GROUP_AG << 8 ) | 0x04 )    /* Disconnect audio */

/* BT Serial over GATT service commands */
#define HCI_CONTROL_BSG_COMMAND_DATA                        ( ( HCI_CONTROL_GROUP_BSG << 8 ) | 0x03 )    /* Send data */


/* Miscellaneous commands */
#define HCI_CONTROL_MISC_COMMAND_PING                       ( ( HCI_CONTROL_GROUP_MISC << 8 ) | 0x01 )    /* Ping controller */
#define HCI_CONTROL_MISC_COMMAND_GET_VERSION                ( ( HCI_CONTROL_GROUP_MISC << 8 ) | 0x02 )    /* Get SDK Version */

/* HID Host commands */
#define HCI_CONTROL_HIDH_COMMAND_CONNECT                    ( ( HCI_CONTROL_GROUP_HIDH << 8 ) | 0x01 )      /* HIDH Connect */
#define HCI_CONTROL_HIDH_COMMAND_DISCONNECT                 ( ( HCI_CONTROL_GROUP_HIDH << 8 ) | 0x02 )      /* HIDH Disconnect */
#define HCI_CONTROL_HIDH_COMMAND_ADD                        ( ( HCI_CONTROL_GROUP_HIDH << 8 ) | 0x03 )      /* HIDH Add Device */
#define HCI_CONTROL_HIDH_COMMAND_REMOVE                     ( ( HCI_CONTROL_GROUP_HIDH << 8 ) | 0x04 )      /* HIDH Remove Device */
#define HCI_CONTROL_HIDH_COMMAND_GET_DESCRIPTOR             ( ( HCI_CONTROL_GROUP_HIDH << 8 ) | 0x05 )      /* HIDH Get HID Descriptor */
#define HCI_CONTROL_HIDH_COMMAND_SET_REPORT                 ( ( HCI_CONTROL_GROUP_HIDH << 8 ) | 0x06 )      /* HIDH Set Report */
#define HCI_CONTROL_HIDH_COMMAND_GET_REPORT                 ( ( HCI_CONTROL_GROUP_HIDH << 8 ) | 0x07 )      /* HIDH Get Report */
#define HCI_CONTROL_HIDH_COMMAND_SET_PROTOCOL               ( ( HCI_CONTROL_GROUP_HIDH << 8 ) | 0x08 )      /* HIDH Set HID Protocol */
#define HCI_CONTROL_HIDH_COMMAND_WAKEUP_PATTERN_SET         ( ( HCI_CONTROL_GROUP_HIDH << 8 ) | 0x09 )      /* HIDH WakeUp Pattern */
#define HCI_CONTROL_HIDH_COMMAND_WAKEUP_CONTROL             ( ( HCI_CONTROL_GROUP_HIDH << 8 ) | 0x0A )      /* HIDH WakeUp Control */

/* Audio Sink Profile commands */
#define HCI_CONTROL_AUDIO_SINK_COMMAND_CONNECT              ( ( HCI_CONTROL_GROUP_AUDIO_SINK << 8 ) | 0x01 )    /* Audio connect to source */
#define HCI_CONTROL_AUDIO_SINK_COMMAND_DISCONNECT           ( ( HCI_CONTROL_GROUP_AUDIO_SINK << 8 ) | 0x02 )    /* Audio disconnect  */
#define HCI_CONTROL_AUDIO_SINK_COMMAND_START                ( ( HCI_CONTROL_GROUP_AUDIO_SINK << 8 ) | 0x03 )    /* Start audio with specific Sample rate/mode */
#define HCI_CONTROL_AUDIO_SINK_COMMAND_STOP                 ( ( HCI_CONTROL_GROUP_AUDIO_SINK << 8 ) | 0x04 )    /* Stop audio */
#define HCI_CONTROL_AUDIO_SINK_COMMAND_START_RSP            ( ( HCI_CONTROL_GROUP_AUDIO_SINK << 8 ) | 0x05 )    /* Response to A2DP start request, send start response */
#define HCI_CONTROL_AUDIO_SINK_COMMAND_CHANGE_ROUTE         ( ( HCI_CONTROL_GROUP_AUDIO_SINK << 8 ) | 0x06 )    /* Change the audio route */

/* PBAP Client commands */
#define HCI_CONTROL_PBC_COMMAND_CONNECT                     ( ( HCI_CONTROL_GROUP_PBC << 8 ) | 0x01 )      /* PBC Connect */
#define HCI_CONTROL_PBC_COMMAND_DISCONNECT                  ( ( HCI_CONTROL_GROUP_PBC << 8 ) | 0x02 )      /* PBC Disconnect */
#define HCI_CONTROL_PBC_COMMAND_GET_PHONEBOOK               ( ( HCI_CONTROL_GROUP_PBC << 8 ) | 0x03 )      /* Download phonebook */
#define HCI_CONTROL_PBC_COMMAND_GET_CALL_HISTORY            ( ( HCI_CONTROL_GROUP_PBC << 8 ) | 0x04 )      /* Get call history */
#define HCI_CONTROL_PBC_COMMAND_GET_INCOMMING_CALLS         ( ( HCI_CONTROL_GROUP_PBC << 8 ) | 0x05 )      /* Get list of incoming calls */
#define HCI_CONTROL_PBC_COMMAND_GET_OUTGOING_CALLS          ( ( HCI_CONTROL_GROUP_PBC << 8 ) | 0x06 )      /* Get list of outgoing calls */
#define HCI_CONTROL_PBC_COMMAND_GET_MISSED_CALLS            ( ( HCI_CONTROL_GROUP_PBC << 8 ) | 0x07 )      /* Get list of missed calls */
#define HCI_CONTROL_PBC_COMMAND_ABORT                       ( ( HCI_CONTROL_GROUP_PBC << 8 ) | 0x08 )      /* Abort current operation */

/* LE COC commands */
#define HCI_CONTROL_LE_COC_COMMAND_CONNECT                  ( ( HCI_CONTROL_GROUP_LE_COC << 8 ) | 0x01 )   /* LE COC Connect */
#define HCI_CONTROL_LE_COC_COMMAND_DISCONNECT               ( ( HCI_CONTROL_GROUP_LE_COC << 8 ) | 0x02 )   /* LE COC Disconnect */
#define HCI_CONTROL_LE_COC_COMMAND_SEND_DATA                ( ( HCI_CONTROL_GROUP_LE_COC << 8 ) | 0x03 )   /* Send data */
#define HCI_CONTROL_LE_COC_COMMAND_SET_MTU                  ( ( HCI_CONTROL_GROUP_LE_COC << 8 ) | 0x04 )   /* Set MTU size */
#define HCI_CONTROL_LE_COC_COMMAND_SET_PSM                  ( ( HCI_CONTROL_GROUP_LE_COC << 8 ) | 0x05 )   /* Set PSM number */
#define HCI_CONTROL_LE_COC_COMMAND_ENABLE_LE2M              ( ( HCI_CONTROL_GROUP_LE_COC << 8 ) | 0x06 )   /* Enable LE2M PHY */

/* LED Demo commands */
#define HCI_CONTROL_LED_COMMAND_TURN_ON                     ( ( HCI_CONTROL_GROUP_LED_DEMO << 8 ) | 0x01 )   /* LED Turn On */
#define HCI_CONTROL_LED_COMMAND_TURN_OFF                    ( ( HCI_CONTROL_GROUP_LED_DEMO << 8 ) | 0x02 )   /* LED Turn Off */
#define HCI_CONTROL_LED_COMMAND_SET_BRIGHTNESS              ( ( HCI_CONTROL_GROUP_LED_DEMO << 8 ) | 0x03 )   /* LED set brightness level */

/* Mesh commands */
#define HCI_CONTROL_MESH_COMMAND_CORE_NETWORK_LAYER_TRNSMIT                 ( ( HCI_CONTROL_GROUP_MESH << 8 ) | 0x01 )  /* Network Layer Transmit Message command */
#define HCI_CONTROL_MESH_COMMAND_CORE_TRANSPORT_LAYER_TRNSMIT               ( ( HCI_CONTROL_GROUP_MESH << 8 ) | 0x02 )  /* Transport Layer Transmit Message command */
#define HCI_CONTROL_MESH_COMMAND_CORE_IVUPDATE_SIGNAL_TRNSIT                ( ( HCI_CONTROL_GROUP_MESH << 8 ) | 0x03 )  /* IV UPDATE Transit Signal command */

#define HCI_CONTROL_MESH_COMMAND_PROVISION_SCAN_CAPABILITIES_GET            ( ( HCI_CONTROL_GROUP_MESH << 8 ) | 0x04 )  /* Sends command to get remote provisioner scan information */
#define HCI_CONTROL_MESH_COMMAND_PROVISION_SCAN_GET                         ( ( HCI_CONTROL_GROUP_MESH << 8 ) | 0x05 )  /* Sends command to get remote provisioner scan */
#define HCI_CONTROL_MESH_COMMAND_PROVISION_SCAN_START                       ( ( HCI_CONTROL_GROUP_MESH << 8 ) | 0x06 )  /* Sends command to start scanning for unprovisioned devices */
#define HCI_CONTROL_MESH_COMMAND_PROVISION_SCAN_STOP                        ( ( HCI_CONTROL_GROUP_MESH << 8 ) | 0x07 )  /* Sends command to stop scanning for unprovisioned devices */
#define HCI_CONTROL_MESH_COMMAND_PROVISION_CONNECT                          ( ( HCI_CONTROL_GROUP_MESH << 8 ) | 0x08 )  /* Sends command to establish provisioning link to remote device */
#define HCI_CONTROL_MESH_COMMAND_PROVISION_DISCONNECT                       ( ( HCI_CONTROL_GROUP_MESH << 8 ) | 0x09 )  /* Sends command to disconnect provisioning link with remote device */
#define HCI_CONTROL_MESH_COMMAND_PROVISION_START                            ( ( HCI_CONTROL_GROUP_MESH << 8 ) | 0x0a )  /* Sends command to start provisioning of the remote device */
#define HCI_CONTROL_MESH_COMMAND_PROVISION_OOB_CONFIGURE                    ( ( HCI_CONTROL_GROUP_MESH << 8 ) | 0x0b )  /* Sends out of band configuration for provisioning device */
#define HCI_CONTROL_MESH_COMMAND_PROVISION_OOB_VALUE                        ( ( HCI_CONTROL_GROUP_MESH << 8 ) | 0x0c )  /* Sends command with out of band value for confirmation calculation */
#define HCI_CONTROL_MESH_COMMAND_SEARCH_PROXY                               ( ( HCI_CONTROL_GROUP_MESH << 8 ) | 0x0d )  /* Sends a command to start/stop scanning for GATT proxy devices */
#define HCI_CONTROL_MESH_COMMAND_PROXY_CONNECT                              ( ( HCI_CONTROL_GROUP_MESH << 8 ) | 0x0e )  /* Sends a command to connect to a GATT proxy devices */
#define HCI_CONTROL_MESH_COMMAND_PROXY_DISCONNECT                           ( ( HCI_CONTROL_GROUP_MESH << 8 ) | 0x0f )  /* Sends a command to disconnect to a GATT proxy devices */
#define HCI_CONTROL_MESH_COMMAND_PROXY_FILTER_TYPE_SET                      ( ( HCI_CONTROL_GROUP_MESH << 8 ) | 0x10 )  /* Set Proxy Filter Type */
#define HCI_CONTROL_MESH_COMMAND_PROXY_FILTER_ADDRESSES_ADD                 ( ( HCI_CONTROL_GROUP_MESH << 8 ) | 0x11 )  /* Add Addresses to Filter */
#define HCI_CONTROL_MESH_COMMAND_PROXY_FILTER_ADDRESSES_DELETE              ( ( HCI_CONTROL_GROUP_MESH << 8 ) | 0x12 )  /* Remove Addresses to Filter  */

#define HCI_CONTROL_MESH_COMMAND_ONOFF_GET                                  ( ( HCI_CONTROL_GROUP_MESH << 8 ) | 0x13 )  /* Generic On/Off Get command */
#define HCI_CONTROL_MESH_COMMAND_ONOFF_SET                                  ( ( HCI_CONTROL_GROUP_MESH << 8 ) | 0x14 )  /* Generic On/Off Get command */

#define HCI_CONTROL_MESH_COMMAND_LEVEL_GET                                  ( ( HCI_CONTROL_GROUP_MESH << 8 ) | 0x15 )  /* Generic Level Client Get command */
#define HCI_CONTROL_MESH_COMMAND_LEVEL_SET                                  ( ( HCI_CONTROL_GROUP_MESH << 8 ) | 0x16 )  /* Generic Level Client Set command */
#define HCI_CONTROL_MESH_COMMAND_LEVEL_DELTA_SET                            ( ( HCI_CONTROL_GROUP_MESH << 8 ) | 0x17 )  /* Generic Level Client Delta command */
#define HCI_CONTROL_MESH_COMMAND_LEVEL_MOVE_SET                             ( ( HCI_CONTROL_GROUP_MESH << 8 ) | 0x18 )  /* Generic Level Client Move command */

#define HCI_CONTROL_MESH_COMMAND_DEF_TRANS_TIME_GET                         ( ( HCI_CONTROL_GROUP_MESH << 8 ) | 0x19 )  /* Generic Default Transition Time Set command */
#define HCI_CONTROL_MESH_COMMAND_DEF_TRANS_TIME_SET                         ( ( HCI_CONTROL_GROUP_MESH << 8 ) | 0x1a )  /* Generic Default Transition Time Get command */

#define HCI_CONTROL_MESH_COMMAND_ONPOWERUP_GET                              ( ( HCI_CONTROL_GROUP_MESH << 8 ) | 0x1b )  /* Power On/Off Set command */
#define HCI_CONTROL_MESH_COMMAND_ONPOWERUP_SET                              ( ( HCI_CONTROL_GROUP_MESH << 8 ) | 0x1c )  /* Power On/Off Get command */

#define HCI_CONTROL_MESH_COMMAND_POWER_LEVEL_GET                            ( ( HCI_CONTROL_GROUP_MESH << 8 ) | 0x1d )  /* Generic Power Level Get command */
#define HCI_CONTROL_MESH_COMMAND_POWER_LEVEL_SET                            ( ( HCI_CONTROL_GROUP_MESH << 8 ) | 0x1e )  /* Generic Power Level Set command */
#define HCI_CONTROL_MESH_COMMAND_POWER_LEVEL_LAST_GET                       ( ( HCI_CONTROL_GROUP_MESH << 8 ) | 0x1f )  /* Generic Power Level Last Get command */
#define HCI_CONTROL_MESH_COMMAND_POWER_LEVEL_DEFAULT_GET                    ( ( HCI_CONTROL_GROUP_MESH << 8 ) | 0x20 )  /* Generic Power Level Default Get command */
#define HCI_CONTROL_MESH_COMMAND_POWER_LEVEL_DEFAULT_SET                    ( ( HCI_CONTROL_GROUP_MESH << 8 ) | 0x21 )  /* Generic Power Level Default Set command */
#define HCI_CONTROL_MESH_COMMAND_POWER_LEVEL_RANGE_GET                      ( ( HCI_CONTROL_GROUP_MESH << 8 ) | 0x22 )  /* Generic Power Level Range Get command */
#define HCI_CONTROL_MESH_COMMAND_POWER_LEVEL_RANGE_SET                      ( ( HCI_CONTROL_GROUP_MESH << 8 ) | 0x23 )  /* Generic Power Level Range Set command */

#define HCI_CONTROL_MESH_COMMAND_LOCATION_GLOBAL_SET                        ( ( HCI_CONTROL_GROUP_MESH << 8 ) | 0x24 )  /* Set Global Location data */
#define HCI_CONTROL_MESH_COMMAND_LOCATION_LOCAL_SET                         ( ( HCI_CONTROL_GROUP_MESH << 8 ) | 0x25 )  /* Set Local Location data */
#define HCI_CONTROL_MESH_COMMAND_LOCATION_GLOBAL_GET                        ( ( HCI_CONTROL_GROUP_MESH << 8 ) | 0x26 )  /* Get Global Location data */
#define HCI_CONTROL_MESH_COMMAND_LOCATION_LOCAL_GET                         ( ( HCI_CONTROL_GROUP_MESH << 8 ) | 0x27 )  /* Get_Local Location data */

#define HCI_CONTROL_MESH_COMMAND_BATTERY_GET                                ( ( HCI_CONTROL_GROUP_MESH << 8 ) | 0x28 )  /* Battery Get state */
#define HCI_CONTROL_MESH_COMMAND_BATTERY_SET                                ( ( HCI_CONTROL_GROUP_MESH << 8 ) | 0x29 )  /* Battery state changed */

#define HCI_CONTROL_MESH_COMMAND_PROPERTIES_GET                             ( ( HCI_CONTROL_GROUP_MESH << 8 ) | 0x2a )  /* Generic Set Value of the Property */
#define HCI_CONTROL_MESH_COMMAND_PROPERTY_GET                               ( ( HCI_CONTROL_GROUP_MESH << 8 ) | 0x2b )  /* Generic Set Value of the Property */
#define HCI_CONTROL_MESH_COMMAND_PROPERTY_SET                               ( ( HCI_CONTROL_GROUP_MESH << 8 ) | 0x2c )  /* Generic Value of the Property Changed Status*/

#define HCI_CONTROL_MESH_COMMAND_LIGHT_LIGHTNESS_GET                        ( ( HCI_CONTROL_GROUP_MESH << 8 ) | 0x2d )  /* Light Lightness Get command */
#define HCI_CONTROL_MESH_COMMAND_LIGHT_LIGHTNESS_SET                        ( ( HCI_CONTROL_GROUP_MESH << 8 ) | 0x2e )  /* Light Lightness Set command */
#define HCI_CONTROL_MESH_COMMAND_LIGHT_LIGHTNESS_LINEAR_GET                 ( ( HCI_CONTROL_GROUP_MESH << 8 ) | 0x2f )  /* Light Lightness Linear Get command */
#define HCI_CONTROL_MESH_COMMAND_LIGHT_LIGHTNESS_LINEAR_SET                 ( ( HCI_CONTROL_GROUP_MESH << 8 ) | 0x30 )  /* Light Lightness Linear Set command */
#define HCI_CONTROL_MESH_COMMAND_LIGHT_LIGHTNESS_LAST_GET                   ( ( HCI_CONTROL_GROUP_MESH << 8 ) | 0x31 )  /* Light Lightness Last Get command */
#define HCI_CONTROL_MESH_COMMAND_LIGHT_LIGHTNESS_DEFAULT_GET                ( ( HCI_CONTROL_GROUP_MESH << 8 ) | 0x32 )  /* Light Lightness Default Get command */
#define HCI_CONTROL_MESH_COMMAND_LIGHT_LIGHTNESS_DEFAULT_SET                ( ( HCI_CONTROL_GROUP_MESH << 8 ) | 0x33 )  /* Light Lightness Default Set command */
#define HCI_CONTROL_MESH_COMMAND_LIGHT_LIGHTNESS_RANGE_GET                  ( ( HCI_CONTROL_GROUP_MESH << 8 ) | 0x34 )  /* Light Lightness Range Get command */
#define HCI_CONTROL_MESH_COMMAND_LIGHT_LIGHTNESS_RANGE_SET                  ( ( HCI_CONTROL_GROUP_MESH << 8 ) | 0x35 )  /* Light Lightness Range Set command */

#define HCI_CONTROL_MESH_COMMAND_LIGHT_CTL_GET                              ( ( HCI_CONTROL_GROUP_MESH << 8 ) | 0x36 )  /* Light CTL Client Get command */
#define HCI_CONTROL_MESH_COMMAND_LIGHT_CTL_SET                              ( ( HCI_CONTROL_GROUP_MESH << 8 ) | 0x37 )  /* Light CTL Client Set command */
#define HCI_CONTROL_MESH_COMMAND_LIGHT_CTL_TEMPERATURE_GET                  ( ( HCI_CONTROL_GROUP_MESH << 8 ) | 0x38 )  /* Light CTL Client Temperature Get command */
#define HCI_CONTROL_MESH_COMMAND_LIGHT_CTL_TEMPERATURE_SET                  ( ( HCI_CONTROL_GROUP_MESH << 8 ) | 0x39 )  /* Light CTL Client Temperature Set command */
#define HCI_CONTROL_MESH_COMMAND_LIGHT_CTL_TEMPERATURE_RANGE_GET            ( ( HCI_CONTROL_GROUP_MESH << 8 ) | 0x3a )  /* Light CTL Client Temperature Range Get command */
#define HCI_CONTROL_MESH_COMMAND_LIGHT_CTL_TEMPERATURE_RANGE_SET            ( ( HCI_CONTROL_GROUP_MESH << 8 ) | 0x3b )  /* Light CTL Client Temperature Range Set command */
#define HCI_CONTROL_MESH_COMMAND_LIGHT_CTL_DEFAULT_GET                      ( ( HCI_CONTROL_GROUP_MESH << 8 ) | 0x3c )  /* Light CTL Client Default Get command */
#define HCI_CONTROL_MESH_COMMAND_LIGHT_CTL_DEFAULT_SET                      ( ( HCI_CONTROL_GROUP_MESH << 8 ) | 0x3d )  /* Light CTL Client Default Set command */

#define HCI_CONTROL_MESH_COMMAND_LIGHT_HSL_GET                              ( ( HCI_CONTROL_GROUP_MESH << 8 ) | 0x3e )  /* Light HSL Client Get command */
#define HCI_CONTROL_MESH_COMMAND_LIGHT_HSL_SET                              ( ( HCI_CONTROL_GROUP_MESH << 8 ) | 0x3f )  /* Light HSL Client Set command */
#define HCI_CONTROL_MESH_COMMAND_LIGHT_HSL_TARGET_GET                       ( ( HCI_CONTROL_GROUP_MESH << 8 ) | 0x40 )  /* Light HSL Client Target Get command */
#define HCI_CONTROL_MESH_COMMAND_LIGHT_HSL_RANGE_GET                        ( ( HCI_CONTROL_GROUP_MESH << 8 ) | 0x41 )  /* Light HSL Client Range Get command */
#define HCI_CONTROL_MESH_COMMAND_LIGHT_HSL_RANGE_SET                        ( ( HCI_CONTROL_GROUP_MESH << 8 ) | 0x42 )  /* Light HSL Client Range Set command */
#define HCI_CONTROL_MESH_COMMAND_LIGHT_HSL_DEFAULT_GET                      ( ( HCI_CONTROL_GROUP_MESH << 8 ) | 0x43 )  /* Light HSL Client Default Get command */
#define HCI_CONTROL_MESH_COMMAND_LIGHT_HSL_DEFAULT_SET                      ( ( HCI_CONTROL_GROUP_MESH << 8 ) | 0x44 )  /* Light HSL Client Default Set command */
#define HCI_CONTROL_MESH_COMMAND_LIGHT_HSL_HUE_GET                          ( ( HCI_CONTROL_GROUP_MESH << 8 ) | 0x45 )  /* Light HSL Client Hue Get command */
#define HCI_CONTROL_MESH_COMMAND_LIGHT_HSL_HUE_SET                          ( ( HCI_CONTROL_GROUP_MESH << 8 ) | 0x46 )  /* Light HSL Client Hue Set command */
#define HCI_CONTROL_MESH_COMMAND_LIGHT_HSL_SATURATION_GET                   ( ( HCI_CONTROL_GROUP_MESH << 8 ) | 0x47 )  /* Light HSL Client Saturation Get command */
#define HCI_CONTROL_MESH_COMMAND_LIGHT_HSL_SATURATION_SET                   ( ( HCI_CONTROL_GROUP_MESH << 8 ) | 0x48 )  /* Light HSL Client Saturation Set command */

#define HCI_CONTROL_MESH_COMMAND_LIGHT_XYL_GET                              ( ( HCI_CONTROL_GROUP_MESH << 8 ) | 0x49 )  /* Light XYL Client Get command */
#define HCI_CONTROL_MESH_COMMAND_LIGHT_XYL_SET                              ( ( HCI_CONTROL_GROUP_MESH << 8 ) | 0x4a )  /* Light XYL Client Set command */
#define HCI_CONTROL_MESH_COMMAND_LIGHT_XYL_RANGE_GET                        ( ( HCI_CONTROL_GROUP_MESH << 8 ) | 0x4b )  /* Light XYL Client Range Get command */
#define HCI_CONTROL_MESH_COMMAND_LIGHT_XYL_RANGE_SET                        ( ( HCI_CONTROL_GROUP_MESH << 8 ) | 0x4c )  /* Light XYL Client Range Set command */
#define HCI_CONTROL_MESH_COMMAND_LIGHT_XYL_TARGET_GET                       ( ( HCI_CONTROL_GROUP_MESH << 8 ) | 0x4d )  /* Light XYL Client Target Get command */
#define HCI_CONTROL_MESH_COMMAND_LIGHT_XYL_DEFAULT_GET                      ( ( HCI_CONTROL_GROUP_MESH << 8 ) | 0x4e )  /* Light XYL Client Default Get command */
#define HCI_CONTROL_MESH_COMMAND_LIGHT_XYL_DEFAULT_SET                      ( ( HCI_CONTROL_GROUP_MESH << 8 ) | 0x4f )  /* Light XYL Client Default Set command */

#define HCI_CONTROL_MESH_COMMAND_LIGHT_LC_MODE_GET                          ( ( HCI_CONTROL_GROUP_MESH << 8 ) | 0x50 )  /* Light LC Client Mode Get command */
#define HCI_CONTROL_MESH_COMMAND_LIGHT_LC_MODE_SET                          ( ( HCI_CONTROL_GROUP_MESH << 8 ) | 0x51 )  /* Light LC Client Mode Set command */
#define HCI_CONTROL_MESH_COMMAND_LIGHT_LC_OCCUPANCY_MODE_GET                ( ( HCI_CONTROL_GROUP_MESH << 8 ) | 0x53 )  /* Light LC Client Occupancy Mode Get command */
#define HCI_CONTROL_MESH_COMMAND_LIGHT_LC_OCCUPANCY_MODE_SET                ( ( HCI_CONTROL_GROUP_MESH << 8 ) | 0x54 )  /* Light LC Client Occupancy Mode Set command */
#define HCI_CONTROL_MESH_COMMAND_LIGHT_LC_ONOFF_GET                         ( ( HCI_CONTROL_GROUP_MESH << 8 ) | 0x56 )  /* Light LC Client OnOff Get command */
#define HCI_CONTROL_MESH_COMMAND_LIGHT_LC_ONOFF_SET                         ( ( HCI_CONTROL_GROUP_MESH << 8 ) | 0x57 )  /* Light LC Client OnOff Set command */
#define HCI_CONTROL_MESH_COMMAND_LIGHT_LC_PROPERTY_GET                      ( ( HCI_CONTROL_GROUP_MESH << 8 ) | 0x59 )  /* Light LC Client Property Get command */
#define HCI_CONTROL_MESH_COMMAND_LIGHT_LC_PROPERTY_SET                      ( ( HCI_CONTROL_GROUP_MESH << 8 ) | 0x5a )  /* Light LC Client Property Set command */
#define HCI_CONTROL_MESH_COMMAND_LIGHT_LC_OCCUPANCY_SET                     ( ( HCI_CONTROL_GROUP_MESH << 8 ) | 0x5c )  /* Light LC Server Occupancy Detected command */

#define HCI_CONTROL_MESH_COMMAND_SENSOR_DESCRIPTOR_GET                      ( ( HCI_CONTROL_GROUP_MESH << 8 ) | 0x60 )  /* Sensor Descriptor Get command */
#define HCI_CONTROL_MESH_COMMAND_SENSOR_CADENCE_GET                         ( ( HCI_CONTROL_GROUP_MESH << 8 ) | 0x61 )  /* Sensor Cadence Get command */
#define HCI_CONTROL_MESH_COMMAND_SENSOR_CADENCE_SET                         ( ( HCI_CONTROL_GROUP_MESH << 8 ) | 0x62 )  /* Sensor Cadence Set command */
#define HCI_CONTROL_MESH_COMMAND_SENSOR_SETTINGS_GET                        ( ( HCI_CONTROL_GROUP_MESH << 8 ) | 0x63 )  /* Sensor Settings Get command */
#define HCI_CONTROL_MESH_COMMAND_SENSOR_SETTING_GET                         ( ( HCI_CONTROL_GROUP_MESH << 8 ) | 0x64 )  /* Sensor Setting Get command */
#define HCI_CONTROL_MESH_COMMAND_SENSOR_GET                                 ( ( HCI_CONTROL_GROUP_MESH << 8 ) | 0x65 )  /* Sensor Get command */
#define HCI_CONTROL_MESH_COMMAND_SENSOR_COLUMN_GET                          ( ( HCI_CONTROL_GROUP_MESH << 8 ) | 0x66 )  /* Sensor Column Get command */
#define HCI_CONTROL_MESH_COMMAND_SENSOR_SERIES_GET                          ( ( HCI_CONTROL_GROUP_MESH << 8 ) | 0x67 )  /* Sensor Series Get command */
#define HCI_CONTROL_MESH_COMMAND_SENSOR_SETTING_SET                         ( ( HCI_CONTROL_GROUP_MESH << 8 ) | 0x68 )  /* Sensor Setting Set command */
#define HCI_CONTROL_MESH_COMMAND_SENSOR_SET                                 ( ( HCI_CONTROL_GROUP_MESH << 8 ) | 0x6b )  /* Sensor Status command */
#define HCI_CONTROL_MESH_COMMAND_SENSOR_COLUMN_SET                          ( ( HCI_CONTROL_GROUP_MESH << 8 ) | 0x6c )  /* Sensor Column Status command */
#define HCI_CONTROL_MESH_COMMAND_SENSOR_SERIES_SET                          ( ( HCI_CONTROL_GROUP_MESH << 8 ) | 0x6d )  /* Sensor Series Status command */

#define HCI_CONTROL_MESH_COMMAND_SCENE_STORE                                ( ( HCI_CONTROL_GROUP_MESH << 8 ) | 0x70 )  /* Scene Store command */
#define HCI_CONTROL_MESH_COMMAND_SCENE_RECALL                               ( ( HCI_CONTROL_GROUP_MESH << 8 ) | 0x71 )  /* Scene Delete command */
#define HCI_CONTROL_MESH_COMMAND_SCENE_GET                                  ( ( HCI_CONTROL_GROUP_MESH << 8 ) | 0x72 )  /* Scene Get command */
#define HCI_CONTROL_MESH_COMMAND_SCENE_REGISTER_GET                         ( ( HCI_CONTROL_GROUP_MESH << 8 ) | 0x73 )  /* Scene Register Get command */
#define HCI_CONTROL_MESH_COMMAND_SCENE_DELETE                               ( ( HCI_CONTROL_GROUP_MESH << 8 ) | 0x74 )  /* Scene Delete command */

#define HCI_CONTROL_MESH_COMMAND_SCHEDULER_GET                              ( ( HCI_CONTROL_GROUP_MESH << 8 ) | 0x75 )  /* Scheduler Register Get command */
#define HCI_CONTROL_MESH_COMMAND_SCHEDULER_ACTION_GET                       ( ( HCI_CONTROL_GROUP_MESH << 8 ) | 0x76 )  /* Scheduler Action Get command */
#define HCI_CONTROL_MESH_COMMAND_SCHEDULER_ACTION_SET                       ( ( HCI_CONTROL_GROUP_MESH << 8 ) | 0x77 )  /* Scheduler Action Set command */

#define HCI_CONTROL_MESH_COMMAND_TIME_GET                                   ( ( HCI_CONTROL_GROUP_MESH << 8 ) | 0x78 )  /* Time Get command */
#define HCI_CONTROL_MESH_COMMAND_TIME_SET                                   ( ( HCI_CONTROL_GROUP_MESH << 8 ) | 0x79 )  /* Time Set command */
#define HCI_CONTROL_MESH_COMMAND_TIME_ZONE_GET                              ( ( HCI_CONTROL_GROUP_MESH << 8 ) | 0x7a )  /* Time Zone Get command */
#define HCI_CONTROL_MESH_COMMAND_TIME_ZONE_SET                              ( ( HCI_CONTROL_GROUP_MESH << 8 ) | 0x7b )  /* Time Zone Set command */
#define HCI_CONTROL_MESH_COMMAND_TIME_TAI_UTC_DELTA_GET                     ( ( HCI_CONTROL_GROUP_MESH << 8 ) | 0x7c )  /* Time TAI_UTC Delta Get command */
#define HCI_CONTROL_MESH_COMMAND_TIME_TAI_UTC_DELTA_SET                     ( ( HCI_CONTROL_GROUP_MESH << 8 ) | 0x7d )  /* Time TAI_UTC Delta Set command */
#define HCI_CONTROL_MESH_COMMAND_TIME_ROLE_GET                              ( ( HCI_CONTROL_GROUP_MESH << 8 ) | 0x7e )  /* Time Role Get command */
#define HCI_CONTROL_MESH_COMMAND_TIME_ROLE_SET                              ( ( HCI_CONTROL_GROUP_MESH << 8 ) | 0x7f )  /* Time Role Set command */

#define HCI_CONTROL_MESH_COMMAND_CONFIG_NODE_RESET                          ( ( HCI_CONTROL_GROUP_MESH << 8 ) | 0x80 )  /* Node Reset */
#define HCI_CONTROL_MESH_COMMAND_CONFIG_BEACON_GET                          ( ( HCI_CONTROL_GROUP_MESH << 8 ) | 0x81 )  /* Beacon State Get */
#define HCI_CONTROL_MESH_COMMAND_CONFIG_BEACON_SET                          ( ( HCI_CONTROL_GROUP_MESH << 8 ) | 0x82 )  /* Beacon State Set */
#define HCI_CONTROL_MESH_COMMAND_CONFIG_COMPOSITION_DATA_GET                ( ( HCI_CONTROL_GROUP_MESH << 8 ) | 0x83 )  /* Composition Data Get */
#define HCI_CONTROL_MESH_COMMAND_CONFIG_DEFAULT_TTL_GET                     ( ( HCI_CONTROL_GROUP_MESH << 8 ) | 0x84 )  /* Default TTL Get */
#define HCI_CONTROL_MESH_COMMAND_CONFIG_DEFAULT_TTL_SET                     ( ( HCI_CONTROL_GROUP_MESH << 8 ) | 0x85 )  /* Default TTL Set */
#define HCI_CONTROL_MESH_COMMAND_CONFIG_GATT_PROXY_GET                      ( ( HCI_CONTROL_GROUP_MESH << 8 ) | 0x86 )  /* GATT Proxy State Get */
#define HCI_CONTROL_MESH_COMMAND_CONFIG_GATT_PROXY_SET                      ( ( HCI_CONTROL_GROUP_MESH << 8 ) | 0x87 )  /* GATT Proxy State Set */
#define HCI_CONTROL_MESH_COMMAND_CONFIG_RELAY_GET                           ( ( HCI_CONTROL_GROUP_MESH << 8 ) | 0x88 )  /* Relay Configuration Get */
#define HCI_CONTROL_MESH_COMMAND_CONFIG_RELAY_SET                           ( ( HCI_CONTROL_GROUP_MESH << 8 ) | 0x89 )  /* Relay Configuration Set */
#define HCI_CONTROL_MESH_COMMAND_CONFIG_FRIEND_GET                          ( ( HCI_CONTROL_GROUP_MESH << 8 ) | 0x8a )  /* Relay Configuration Set */
#define HCI_CONTROL_MESH_COMMAND_CONFIG_FRIEND_SET                          ( ( HCI_CONTROL_GROUP_MESH << 8 ) | 0x8b )  /* Relay Configuration Set */
#define HCI_CONTROL_MESH_COMMAND_CONFIG_HEARBEAT_SUBSCRIPTION_GET           ( ( HCI_CONTROL_GROUP_MESH << 8 ) | 0x8c )  /* Hearbeat Subscription Set */
#define HCI_CONTROL_MESH_COMMAND_CONFIG_HEARBEAT_SUBSCRIPTION_SET           ( ( HCI_CONTROL_GROUP_MESH << 8 ) | 0x8d )  /* Hearbeat Subscription Set */
#define HCI_CONTROL_MESH_COMMAND_CONFIG_HEARBEAT_PUBLICATION_GET            ( ( HCI_CONTROL_GROUP_MESH << 8 ) | 0x8e )  /* Hearbeat Publication Set */
#define HCI_CONTROL_MESH_COMMAND_CONFIG_HEARBEAT_PUBLICATION_SET            ( ( HCI_CONTROL_GROUP_MESH << 8 ) | 0x8f )  /* Hearbeat Publication Set */
#define HCI_CONTROL_MESH_COMMAND_CONFIG_NETWORK_TRANSMIT_GET                ( ( HCI_CONTROL_GROUP_MESH << 8 ) | 0x90 )  /* Network Transmit Get */
#define HCI_CONTROL_MESH_COMMAND_CONFIG_NETWORK_TRANSMIT_SET                ( ( HCI_CONTROL_GROUP_MESH << 8 ) | 0x91 )  /* Network Transmit Set */

#define HCI_CONTROL_MESH_COMMAND_CONFIG_MODEL_PUBLICATION_GET               ( ( HCI_CONTROL_GROUP_MESH << 8 ) | 0x92 )  /* Model Publication Get */
#define HCI_CONTROL_MESH_COMMAND_CONFIG_MODEL_PUBLICATION_SET               ( ( HCI_CONTROL_GROUP_MESH << 8 ) | 0x93 )  /* Model Publication Set */
#define HCI_CONTROL_MESH_COMMAND_CONFIG_MODEL_SUBSCRIPTION_ADD              ( ( HCI_CONTROL_GROUP_MESH << 8 ) | 0x94 )  /* Model Subscription Add */
#define HCI_CONTROL_MESH_COMMAND_CONFIG_MODEL_SUBSCRIPTION_DELETE           ( ( HCI_CONTROL_GROUP_MESH << 8 ) | 0x95 )  /* Model Subscription Delete */
#define HCI_CONTROL_MESH_COMMAND_CONFIG_MODEL_SUBSCRIPTION_OVERWRITE        ( ( HCI_CONTROL_GROUP_MESH << 8 ) | 0x96 )  /* Model Subscription Overwrite */
#define HCI_CONTROL_MESH_COMMAND_CONFIG_MODEL_SUBSCRIPTION_DELETE_ALL       ( ( HCI_CONTROL_GROUP_MESH << 8 ) | 0x97 )  /* Model Subscription Delete All */
#define HCI_CONTROL_MESH_COMMAND_CONFIG_MODEL_SUBSCRIPTION_GET              ( ( HCI_CONTROL_GROUP_MESH << 8 ) | 0x98 )  /* Model Subscription Get */
#define HCI_CONTROL_MESH_COMMAND_CONFIG_NET_KEY_ADD                         ( ( HCI_CONTROL_GROUP_MESH << 8 ) | 0x99 )  /* NetKey Add */
#define HCI_CONTROL_MESH_COMMAND_CONFIG_NET_KEY_DELETE                      ( ( HCI_CONTROL_GROUP_MESH << 8 ) | 0x9a )  /* NetKey Delete */
#define HCI_CONTROL_MESH_COMMAND_CONFIG_NET_KEY_UPDATE                      ( ( HCI_CONTROL_GROUP_MESH << 8 ) | 0x9b )  /* NetKey Update */
#define HCI_CONTROL_MESH_COMMAND_CONFIG_NET_KEY_GET                         ( ( HCI_CONTROL_GROUP_MESH << 8 ) | 0x9c )  /* NetKey Get */
#define HCI_CONTROL_MESH_COMMAND_CONFIG_APP_KEY_ADD                         ( ( HCI_CONTROL_GROUP_MESH << 8 ) | 0x9d )  /* AppKey Add */
#define HCI_CONTROL_MESH_COMMAND_CONFIG_APP_KEY_DELETE                      ( ( HCI_CONTROL_GROUP_MESH << 8 ) | 0x9e )  /* AppKey Delete */
#define HCI_CONTROL_MESH_COMMAND_CONFIG_APP_KEY_UPDATE                      ( ( HCI_CONTROL_GROUP_MESH << 8 ) | 0x9f )  /* AppKey Update */
#define HCI_CONTROL_MESH_COMMAND_CONFIG_APP_KEY_GET                         ( ( HCI_CONTROL_GROUP_MESH << 8 ) | 0xa0 )  /* AppKey Get */
#define HCI_CONTROL_MESH_COMMAND_CONFIG_MODEL_APP_BIND                      ( ( HCI_CONTROL_GROUP_MESH << 8 ) | 0xa1 )  /* Model App Bind */

#define HCI_CONTROL_MESH_COMMAND_CONFIG_MODEL_APP_UNBIND                    ( ( HCI_CONTROL_GROUP_MESH << 8 ) | 0xa2 )  /* Model App Unind */
#define HCI_CONTROL_MESH_COMMAND_CONFIG_MODEL_APP_GET                       ( ( HCI_CONTROL_GROUP_MESH << 8 ) | 0xa3 )  /* Model App Get */
#define HCI_CONTROL_MESH_COMMAND_CONFIG_NODE_IDENTITY_GET                   ( ( HCI_CONTROL_GROUP_MESH << 8 ) | 0xa4 )  /* Node Identity Get */
#define HCI_CONTROL_MESH_COMMAND_CONFIG_NODE_IDENTITY_SET                   ( ( HCI_CONTROL_GROUP_MESH << 8 ) | 0xa5 )  /* Node Identity Get */

#define HCI_CONTROL_MESH_COMMAND_CONFIG_LPN_POLL_TIMEOUT_GET                ( ( HCI_CONTROL_GROUP_MESH << 8 ) | 0xa6 )  /* LPN Poll Timeout Get */
#define HCI_CONTROL_MESH_COMMAND_CONFIG_KEY_REFRESH_PHASE_GET               ( ( HCI_CONTROL_GROUP_MESH << 8 ) | 0xa7 )  /* Key Refresh Phase Get */
#define HCI_CONTROL_MESH_COMMAND_CONFIG_KEY_REFRESH_PHASE_SET               ( ( HCI_CONTROL_GROUP_MESH << 8 ) | 0xa8 )  /* Key Refresh Phase Set */

#define HCI_CONTROL_MESH_COMMAND_HEALTH_FAULT_GET                           ( ( HCI_CONTROL_GROUP_MESH << 8 ) | 0xa9 )  /* Health Fault Get */
#define HCI_CONTROL_MESH_COMMAND_HEALTH_FAULT_CLEAR                         ( ( HCI_CONTROL_GROUP_MESH << 8 ) | 0xaa )  /* Health Fault Clear */
#define HCI_CONTROL_MESH_COMMAND_HEALTH_FAULT_TEST                          ( ( HCI_CONTROL_GROUP_MESH << 8 ) | 0xab )  /* Health Fault Test */
#define HCI_CONTROL_MESH_COMMAND_HEALTH_PERIOD_GET                          ( ( HCI_CONTROL_GROUP_MESH << 8 ) | 0xac )  /* Health Period Get */
#define HCI_CONTROL_MESH_COMMAND_HEALTH_PERIOD_SET                          ( ( HCI_CONTROL_GROUP_MESH << 8 ) | 0xad )  /* Health Period Set */
#define HCI_CONTROL_MESH_COMMAND_HEALTH_ATTENTION_GET                       ( ( HCI_CONTROL_GROUP_MESH << 8 ) | 0xae )  /* Health Attention Get */
#define HCI_CONTROL_MESH_COMMAND_HEALTH_ATTENTION_SET                       ( ( HCI_CONTROL_GROUP_MESH << 8 ) | 0xaf )  /* Health Attention Set */

#define HCI_CONTROL_MESH_COMMAND_CONFIG_MODEL_ADD                           ( ( HCI_CONTROL_GROUP_MESH << 8 ) | 0xb0 )  /* Add Vendor Model */

#define HCI_CONTROL_MESH_COMMAND_SET_LOCAL_DEVICE                           ( ( HCI_CONTROL_GROUP_MESH << 8 ) | 0xe0 )  /* Set Local Device. Application can set it once to make provisioner client. */
#define HCI_CONTROL_MESH_COMMAND_SET_DEVICE_KEY                             ( ( HCI_CONTROL_GROUP_MESH << 8 ) | 0xe1 )  /* Setup device key.  Application can set it once and then send multiple configuration commands. */
#define HCI_CONTROL_MESH_COMMAND_CORE_LOW_POWER_SEND_FRIEND_CLEAR           ( ( HCI_CONTROL_GROUP_MESH << 8 ) | 0xe2 )  /* Terminate friendship with a Friend by sending a Friend Clear */
#define HCI_CONTROL_MESH_COMMAND_CORE_PROVISION                             ( ( HCI_CONTROL_GROUP_MESH << 8 ) | 0xe3 )  /* Sends command to provision remote device */
#define HCI_CONTROL_MESH_COMMAND_CORE_CLEAR_REPLAY_PROT_LIST                ( ( HCI_CONTROL_GROUP_MESH << 8 ) | 0xe4 )  /* Sends command to clear replay protection list */
#define HCI_CONTROL_MESH_COMMAND_CORE_SET_IV_UPDATE_TEST_MODE               ( ( HCI_CONTROL_GROUP_MESH << 8 ) | 0xe5 )  /* Sends command to set or reset IV UPDATE test mode */
#define HCI_CONTROL_MESH_COMMAND_CORE_SET_IV_RECOVERY_STATE                 ( ( HCI_CONTROL_GROUP_MESH << 8 ) | 0xe6 )  /* Sends command to set or reset IV RECOVERY mode */
#define HCI_CONTROL_MESH_COMMAND_CORE_HEALTH_SET_FAULTS                     ( ( HCI_CONTROL_GROUP_MESH << 8 ) | 0xe7 )  /* Sends command to set faults array */
#define HCI_CONTROL_MESH_COMMAND_CORE_CFG_ONE_NETKEY                        ( ( HCI_CONTROL_GROUP_MESH << 8 ) | 0xe8 )  /* Reduces resources to enable one netkey */
#define HCI_CONTROL_MESH_COMMAND_CORE_CFG_ADV_IDENTITY                      ( ( HCI_CONTROL_GROUP_MESH << 8 ) | 0xe9 )  /* Begin advertising with Node Identity, simulating user interaction */
#define HCI_CONTROL_MESH_COMMAND_CORE_ACCESS_PDU                            ( ( HCI_CONTROL_GROUP_MESH << 8 ) | 0xea )  /* Handle Access PDU */
#define HCI_CONTROL_MESH_COMMAND_CORE_SEND_SUBS_UPDT                        ( ( HCI_CONTROL_GROUP_MESH << 8 ) | 0xeb )  /* Send Friend Subscription Add or Remove message */

#define HCI_CONTROL_MESH_COMMAND_CORE_SET_SEQ                               ( ( HCI_CONTROL_GROUP_MESH << 8 ) | 0xec )  /* Set Sequence Number Command */
#define HCI_CONTROL_MESH_COMMAND_CORE_DEL_SEQ                               ( ( HCI_CONTROL_GROUP_MESH << 8 ) | 0xed )  /* Delete Sequence Number from RPL list Command */

#define HCI_CONTROL_MESH_COMMAND_APP_START                                  ( ( HCI_CONTROL_GROUP_MESH << 8 ) | 0xef )  /* Starts Mesh Application - calls mesh_application_init() and deletes all NVRAM chunks */

#define HCI_CONTROL_MESH_COMMAND_PROVISION_SCAN_EXTENDED_START              ( ( HCI_CONTROL_GROUP_MESH << 8 ) | 0xf0 )  /* Sends command to start extended scan */
#define HCI_CONTROL_MESH_COMMAND_GATEWAY_CONN_STATUS                        ( ( HCI_CONTROL_GROUP_MESH << 8 ) | 0xf1 )  /* Application sends connect proxy */
#define HCI_CONTROL_MESH_COMMAND_SEND_PROXY_DATA                            ( ( HCI_CONTROL_GROUP_MESH << 8 ) | 0xf2 )  /* Mesh Application can send proxy data from MCU */
#define HCI_CONTROL_MESH_COMMAND_SET_MODEL_LEVEL_ACCESS                     ( ( HCI_CONTROL_GROUP_MESH << 8 ) | 0xf3 )  /* Mesh Application processes raw data access layer data */
#define HCI_CONTROL_MESH_COMMAND_RAW_MODEL_DATA                             ( ( HCI_CONTROL_GROUP_MESH << 8 ) | 0xf4 )  /* Raw model data to be passed to access layer */
#define HCI_CONTROL_MESH_COMMAND_VENDOR_DATA                                ( ( HCI_CONTROL_GROUP_MESH << 8 ) | 0xff )  /* Vendor model data */

/* Battery Client Profile commands */
#define HCI_CONTROL_BATT_CLIENT_COMMAND_CONNECT             ( ( HCI_CONTROL_GROUP_BATT_CLIENT << 8 ) | 0x00 )    /* Battery Client connect */
#define HCI_CONTROL_BATT_CLIENT_COMMAND_DISCONNECT          ( ( HCI_CONTROL_GROUP_BATT_CLIENT << 8 ) | 0x01 )    /* Battery Client disconnect */
#define HCI_CONTROL_BATT_CLIENT_COMMAND_ADD                 ( ( HCI_CONTROL_GROUP_BATT_CLIENT << 8 ) | 0x02 )    /* Battery Client Add */
#define HCI_CONTROL_BATT_CLIENT_COMMAND_REMOVE              ( ( HCI_CONTROL_GROUP_BATT_CLIENT << 8 ) | 0x03 )    /* Battery Client Remove */
#define HCI_CONTROL_BATT_CLIENT_COMMAND_READ                ( ( HCI_CONTROL_GROUP_BATT_CLIENT << 8 ) | 0x04 )    /* Battery Client Read Level */

/* FindMe Locator Client Profile commands */
#define HCI_CONTROL_FINDME_LOCATOR_COMMAND_CONNECT          ( ( HCI_CONTROL_GROUP_FINDME_LOCATOR << 8 ) | 0x00 )    /* FindMe Target connect */
#define HCI_CONTROL_FINDME_LOCATOR_COMMAND_DISCONNECT       ( ( HCI_CONTROL_GROUP_FINDME_LOCATOR << 8 ) | 0x01 )    /* FindMe Target disconnect */
#define HCI_CONTROL_FINDME_LOCATOR_COMMAND_ADD              ( ( HCI_CONTROL_GROUP_FINDME_LOCATOR << 8 ) | 0x02 )    /* FindMe Target Add */
#define HCI_CONTROL_FINDME_LOCATOR_COMMAND_REMOVE           ( ( HCI_CONTROL_GROUP_FINDME_LOCATOR << 8 ) | 0x03 )    /* FindMe Target Remove */
#define HCI_CONTROL_FINDME_LOCATOR_COMMAND_WRITE            ( ( HCI_CONTROL_GROUP_FINDME_LOCATOR << 8 ) | 0x04 )    /* FindMe Target Write Alert */

/* OPP Server commands */
#define HCI_CONTROL_OPS_COMMAND_DISCONNECT                  ( ( HCI_CONTROL_GROUP_OPS << 8 ) | 0x00 )      /* OPS disconnect */
#define HCI_CONTROL_OPS_COMMAND_ACCESS_RSP                  ( ( HCI_CONTROL_GROUP_OPS << 8 ) | 0x01 )      /* OPS access response */

/* Script commands */
#define HCI_CONTROL_SCRIPT_COMMAND_EXECUTE                  ( ( HCI_CONTROL_GROUP_SCRIPT << 8 ) | 0x01 )    /* Execute function */

/* OTP Commands */
#define HCI_CONTROL_OTP_COMMAND_CONNECT                     ( ( HCI_CONTROL_GROUP_OTP << 8 ) | 0x00 )      /* Connect */
#define HCI_CONTROL_OTP_COMMAND_DISCONNECT                  ( ( HCI_CONTROL_GROUP_OTP << 8 ) | 0x01 )      /* Disconnect */
#define HCI_CONTROL_OTP_COMMAND_START_UPGRADE               ( ( HCI_CONTROL_GROUP_OTP << 8 ) | 0x02 )      /* Starting with the firmware upgrade. Prepare for Upgrade */
#define HCI_CONTROL_OTP_COMMAND_SEND_DATA                   ( ( HCI_CONTROL_GROUP_OTP << 8 ) | 0x03 )      /* OTP send firmware upgrade data */
#define HCI_CONTROL_OTP_COMMAND_UPGRADE                     ( ( HCI_CONTROL_GROUP_OTP << 8 ) | 0x04 )      /* Upgrade to the new firmware */
#define HCI_CONTROL_OTP_COMMAND_READ_CHAR                   ( ( HCI_CONTROL_GROUP_OTP << 8 ) | 0x05 )      /* Read Characteristic */
#define HCI_CONTROL_OTP_COMMAND_WRITE_CHAR                  ( ( HCI_CONTROL_GROUP_OTP << 8 ) | 0x06 )      /* Write Characteristic */
#define HCI_CONTROL_OTP_COMMAND_CONFIGURE_IND               ( ( HCI_CONTROL_GROUP_OTP << 8 ) | 0x07 )      /* Configure Indication */
#define HCI_CONTROL_OTP_COMMAND_OACP                        ( ( HCI_CONTROL_GROUP_OTP << 8 ) | 0x08 )      /* Write OACP Command */

/* General events that the controller can send */
#define HCI_CONTROL_EVENT_COMMAND_STATUS                    ( ( HCI_CONTROL_GROUP_DEVICE << 8 ) | 0x01 )    /* Command status event for the requested operation */
#define HCI_CONTROL_EVENT_WICED_TRACE                       ( ( HCI_CONTROL_GROUP_DEVICE << 8 ) | 0x02 )    /* WICED trace packet */
#define HCI_CONTROL_EVENT_HCI_TRACE                         ( ( HCI_CONTROL_GROUP_DEVICE << 8 ) | 0x03 )    /* Bluetooth protocol trace */
#define HCI_CONTROL_EVENT_NVRAM_DATA                        ( ( HCI_CONTROL_GROUP_DEVICE << 8 ) | 0x04 )    /* Request to MCU to save NVRAM chunk */
#define HCI_CONTROL_EVENT_DEVICE_STARTED                    ( ( HCI_CONTROL_GROUP_DEVICE << 8 ) | 0x05 )    /* Device completed power up initialization */
#define HCI_CONTROL_EVENT_INQUIRY_RESULT                    ( ( HCI_CONTROL_GROUP_DEVICE << 8 ) | 0x06 )    /* Inquiry result */
#define HCI_CONTROL_EVENT_INQUIRY_COMPLETE                  ( ( HCI_CONTROL_GROUP_DEVICE << 8 ) | 0x07 )    /* Inquiry completed event */
#define HCI_CONTROL_EVENT_PAIRING_COMPLETE                  ( ( HCI_CONTROL_GROUP_DEVICE << 8 ) | 0x08 )    /* Pairing Completed */
#define HCI_CONTROL_EVENT_ENCRYPTION_CHANGED                ( ( HCI_CONTROL_GROUP_DEVICE << 8 ) | 0x09 )    /* Encryption changed event */
#define HCI_CONTROL_EVENT_CONNECTED_DEVICE_NAME             ( ( HCI_CONTROL_GROUP_DEVICE << 8 ) | 0x0A )    /* Device name event */
#define HCI_CONTROL_EVENT_USER_CONFIRMATION                 ( ( HCI_CONTROL_GROUP_DEVICE << 8 ) | 0x0B )    /* User Confirmation during pairing */
#define HCI_CONTROL_EVENT_DEVICE_ERROR                      ( ( HCI_CONTROL_GROUP_DEVICE << 8 ) | 0x0C )    /* Device Error event */
#define HCI_CONTROL_EVENT_READ_LOCAL_BDA                    ( ( HCI_CONTROL_GROUP_DEVICE << 8 ) | 0x0D )    /* Local BDA Read event */
#define HCI_CONTROL_EVENT_MAX_NUM_OF_PAIRED_DEVICES_REACHED ( ( HCI_CONTROL_GROUP_DEVICE << 8 ) | 0x0E )    /* Key Buffer Pool Full */
#define HCI_CONTROL_EVENT_READ_BUFFER_STATS                 ( ( HCI_CONTROL_GROUP_DEVICE << 8 ) | 0x0F )    /* Read Buffer statistics event */
#define HCI_CONTROL_EVENT_KEYPRESS_NOTIFICATION             ( ( HCI_CONTROL_GROUP_DEVICE << 8 ) | 0x17 )    /* KeyPress notification */
#define HCI_CONTROL_EVENT_CONNECTION_STATUS                 ( ( HCI_CONTROL_GROUP_DEVICE << 8 ) | 0x18 )    /* Connection Status */

/* Events for the HFP profile */
#define HCI_CONTROL_HF_EVENT_OPEN                           ( ( HCI_CONTROL_GROUP_HF << 8 ) | 0x01 )    /* HS connection opened or connection attempt failed  */
#define HCI_CONTROL_HF_EVENT_CLOSE                          ( ( HCI_CONTROL_GROUP_HF << 8 ) | 0x02 )    /* HS connection closed */
#define HCI_CONTROL_HF_EVENT_CONNECTED                      ( ( HCI_CONTROL_GROUP_HF << 8 ) | 0x03 )    /* HS Service Level Connection is UP */
#define HCI_CONTROL_HF_EVENT_AUDIO_OPEN                     ( ( HCI_CONTROL_GROUP_HF << 8 ) | 0x04 )    /* Audio connection open */
#define HCI_CONTROL_HF_EVENT_AUDIO_CLOSE                    ( ( HCI_CONTROL_GROUP_HF << 8 ) | 0x05 )    /* Audio connection closed */
#define HCI_CONTROL_HF_EVENT_AUDIO_CONN_REQ                 ( ( HCI_CONTROL_GROUP_HF << 8 ) | 0x06 )    /* Audio connection request event */
#define HCI_CONTROL_HF_EVENT_PROFILE_TYPE                   ( ( HCI_CONTROL_GROUP_HF << 8 ) | 0x07 )    /* To check the Profile Selected HSP/HFP */

/* Sub-commands AT events defined with AT Commands */
#define HCI_CONTROL_HF_AT_EVENT_BASE                        ( ( HCI_CONTROL_GROUP_HF << 8 ) | 0x20 )
#define HCI_CONTROL_HF_AT_EVENT_OK                          0x00    /* OK response received to previous AT command */
#define HCI_CONTROL_HF_AT_EVENT_ERROR                       0x01    /* ERROR response received */
#define HCI_CONTROL_HF_AT_EVENT_CMEE                        0x02    /* Extended error codes response */
#define HCI_CONTROL_HF_AT_EVENT_RING                        0x03    /* RING indicator */
#define HCI_CONTROL_HF_AT_EVENT_VGS                         0x04
#define HCI_CONTROL_HF_AT_EVENT_VGM                         0x05
#define HCI_CONTROL_HF_AT_EVENT_CCWA                        0x06
#define HCI_CONTROL_HF_AT_EVENT_CHLD                        0x07
#define HCI_CONTROL_HF_AT_EVENT_CIND                        0x08
#define HCI_CONTROL_HF_AT_EVENT_CLIP                        0x09
#define HCI_CONTROL_HF_AT_EVENT_CIEV                        0x0A
#define HCI_CONTROL_HF_AT_EVENT_BINP                        0x0B
#define HCI_CONTROL_HF_AT_EVENT_BVRA                        0x0C
#define HCI_CONTROL_HF_AT_EVENT_BSIR                        0x0D
#define HCI_CONTROL_HF_AT_EVENT_CNUM                        0x0E
#define HCI_CONTROL_HF_AT_EVENT_BTRH                        0x0F
#define HCI_CONTROL_HF_AT_EVENT_COPS                        0x10
#define HCI_CONTROL_HF_AT_EVENT_CLCC                        0x11
#define HCI_CONTROL_HF_AT_EVENT_BIND                        0x12
#define HCI_CONTROL_HF_AT_EVENT_BCS                         0x13
#define HCI_CONTROL_HF_AT_EVENT_UNAT                        0x14
#define HCI_CONTROL_HF_AT_EVENT_MAX                         0x14    /* Maximum AT event value */

/* LE events for the BLE GATT/GAP profile */
#define HCI_CONTROL_LE_EVENT_COMMAND_STATUS                 ( ( HCI_CONTROL_GROUP_LE << 8 ) | 0x01 )    /* Command status event for the requested operation */
#define HCI_CONTROL_LE_EVENT_SCAN_STATUS                    ( ( HCI_CONTROL_GROUP_LE << 8 ) | 0x02 )    /* LE scanning state change notification */
#define HCI_CONTROL_LE_EVENT_ADVERTISEMENT_REPORT           ( ( HCI_CONTROL_GROUP_LE << 8 ) | 0x03 )    /* Advertisement report */
#define HCI_CONTROL_LE_EVENT_ADVERTISEMENT_STATE            ( ( HCI_CONTROL_GROUP_LE << 8 ) | 0x04 )    /* LE Advertisement state change notification */
#define HCI_CONTROL_LE_EVENT_CONNECTED                      ( ( HCI_CONTROL_GROUP_LE << 8 ) | 0x05 )    /* LE Connection established */
#define HCI_CONTROL_LE_EVENT_DISCONNECTED                   ( ( HCI_CONTROL_GROUP_LE << 8 ) | 0x06 )    /* Le Connection Terminated */
#define HCI_CONTROL_LE_EVENT_IDENTITY_ADDRESS               ( ( HCI_CONTROL_GROUP_LE << 8 ) | 0x07 )    /* Identity address */
#define HCI_CONTROL_LE_EVENT_PEER_MTU                       ( ( HCI_CONTROL_GROUP_LE << 8 ) | 0x08 )    /* Client MTU Request */
#define HCI_CONTROL_LE_EVENT_CONN_PARAMS                    ( ( HCI_CONTROL_GROUP_LE << 8 ) | 0x09 )    /* BLE connection parameter update event */

/* GATT events */
#define HCI_CONTROL_GATT_EVENT_COMMAND_STATUS               ( ( HCI_CONTROL_GROUP_GATT << 8 ) | 0x01 )    /* Command status event for the requested operation */
#define HCI_CONTROL_GATT_EVENT_DISCOVERY_COMPLETE           ( ( HCI_CONTROL_GROUP_GATT << 8 ) | 0x02 )    /* Discovery requested by host completed */
#define HCI_CONTROL_GATT_EVENT_SERVICE_DISCOVERED           ( ( HCI_CONTROL_GROUP_GATT << 8 ) | 0x03 )    /* Service discovered */
#define HCI_CONTROL_GATT_EVENT_CHARACTERISTIC_DISCOVERED    ( ( HCI_CONTROL_GROUP_GATT << 8 ) | 0x04 )    /* Characteristic discovered */
#define HCI_CONTROL_GATT_EVENT_DESCRIPTOR_DISCOVERED        ( ( HCI_CONTROL_GROUP_GATT << 8 ) | 0x05 )    /* Characteristic descriptor discovered */
#define HCI_CONTROL_GATT_EVENT_READ_REQUEST                 ( ( HCI_CONTROL_GROUP_GATT << 8 ) | 0x06 )    /* Peer sent Read Request */
#define HCI_CONTROL_GATT_EVENT_READ_RESPONSE                ( ( HCI_CONTROL_GROUP_GATT << 8 ) | 0x07 )    /* Read response */
#define HCI_CONTROL_GATT_EVENT_WRITE_REQUEST                ( ( HCI_CONTROL_GROUP_GATT << 8 ) | 0x08 )    /* Peer sent Write Request */
#define HCI_CONTROL_GATT_EVENT_WRITE_RESPONSE               ( ( HCI_CONTROL_GROUP_GATT << 8 ) | 0x09 )    /* Write operation completed */
#define HCI_CONTROL_GATT_EVENT_INDICATION                   ( ( HCI_CONTROL_GROUP_GATT << 8 ) | 0x0a )    /* Indication from peer */
#define HCI_CONTROL_GATT_EVENT_NOTIFICATION                 ( ( HCI_CONTROL_GROUP_GATT << 8 ) | 0x0b )    /* Notification from peer */
#define HCI_CONTROL_GATT_EVENT_READ_ERROR                   ( ( HCI_CONTROL_GROUP_GATT << 8 ) | 0x0c )    /* GATT Read operation error */
#define HCI_CONTROL_GATT_EVENT_WRITE_ERROR                  ( ( HCI_CONTROL_GROUP_GATT << 8 ) | 0x0d )    /* GATT Write operation error */

/* Events for the SPP profile */
#define HCI_CONTROL_SPP_EVENT_CONNECTED                     ( ( HCI_CONTROL_GROUP_SPP << 8 ) | 0x01 )    /* SPP connection opened */
#define HCI_CONTROL_SPP_EVENT_SERVICE_NOT_FOUND             ( ( HCI_CONTROL_GROUP_SPP << 8 ) | 0x02 )    /* SDP record with SPP service not found */
#define HCI_CONTROL_SPP_EVENT_CONNECTION_FAILED             ( ( HCI_CONTROL_GROUP_SPP << 8 ) | 0x03 )    /* Connection attempt failed  */
#define HCI_CONTROL_SPP_EVENT_DISCONNECTED                  ( ( HCI_CONTROL_GROUP_SPP << 8 ) | 0x04 )    /* SPP connection closed */
#define HCI_CONTROL_SPP_EVENT_TX_COMPLETE                   ( ( HCI_CONTROL_GROUP_SPP << 8 ) | 0x05 )    /* Data packet has been queued for transmission */
#define HCI_CONTROL_SPP_EVENT_RX_DATA                       ( ( HCI_CONTROL_GROUP_SPP << 8 ) | 0x06 )    /* SPP data received */
#define HCI_CONTROL_SPP_EVENT_COMMAND_STATUS                ( ( HCI_CONTROL_GROUP_SPP << 8 ) | 0x07 )    /* Command status event for the requested operation */

/* Events for the Audio profile */
#define HCI_CONTROL_AUDIO_EVENT_COMMAND_COMPLETE            ( ( HCI_CONTROL_GROUP_AUDIO << 8 ) | 0x00 )    /* Command complete event for the requested operation */
#define HCI_CONTROL_AUDIO_EVENT_COMMAND_STATUS              ( ( HCI_CONTROL_GROUP_AUDIO << 8 ) | 0x01 )    /* Command status event for the requested operation */
#define HCI_CONTROL_AUDIO_EVENT_CONNECTED                   ( ( HCI_CONTROL_GROUP_AUDIO << 8 ) | 0x02 )    /* Audio connection opened */
#define HCI_CONTROL_AUDIO_EVENT_SERVICE_NOT_FOUND           ( ( HCI_CONTROL_GROUP_AUDIO << 8 ) | 0x03 )    /* SDP record with audio service not found */
#define HCI_CONTROL_AUDIO_EVENT_CONNECTION_FAILED           ( ( HCI_CONTROL_GROUP_AUDIO << 8 ) | 0x04 )    /* Connection attempt failed  */
#define HCI_CONTROL_AUDIO_EVENT_DISCONNECTED                ( ( HCI_CONTROL_GROUP_AUDIO << 8 ) | 0x05 )    /* Audio connection closed */
#define HCI_CONTROL_AUDIO_EVENT_REQUEST_DATA                ( ( HCI_CONTROL_GROUP_AUDIO << 8 ) | 0x06 )    /* Request for audio pcm sample data */
#define HCI_CONTROL_AUDIO_EVENT_STARTED                     ( ( HCI_CONTROL_GROUP_AUDIO << 8 ) | 0x07 )    /* Command for audio start succeeded */
#define HCI_CONTROL_AUDIO_EVENT_STOPPED                     ( ( HCI_CONTROL_GROUP_AUDIO << 8 ) | 0x08 )    /* Command for audio stop completed */
#define HCI_CONTROL_AUDIO_EVENT_STATISTICS                  ( ( HCI_CONTROL_GROUP_AUDIO << 8 ) | 0x09 )    /* Command for audio statistics */

/* Events for the AVRCP profile target events */
#define HCI_CONTROL_AVRC_TARGET_EVENT_CONNECTED             ( ( HCI_CONTROL_GROUP_AVRC_TARGET << 8 ) | 0x01 )    /* AVRCP Target connected */
#define HCI_CONTROL_AVRC_TARGET_EVENT_DISCONNECTED          ( ( HCI_CONTROL_GROUP_AVRC_TARGET << 8 ) | 0x02 )    /* AVRCP Target disconnected */
#define HCI_CONTROL_AVRC_TARGET_EVENT_PLAY                  ( ( HCI_CONTROL_GROUP_AVRC_TARGET << 8 ) | 0x03 )    /* Play command received */
#define HCI_CONTROL_AVRC_TARGET_EVENT_STOP                  ( ( HCI_CONTROL_GROUP_AVRC_TARGET << 8 ) | 0x04 )    /* Stop command received */
#define HCI_CONTROL_AVRC_TARGET_EVENT_PAUSE                 ( ( HCI_CONTROL_GROUP_AVRC_TARGET << 8 ) | 0x05 )    /* Pause command received */
#define HCI_CONTROL_AVRC_TARGET_EVENT_NEXT_TRACK            ( ( HCI_CONTROL_GROUP_AVRC_TARGET << 8 ) | 0x06 )    /* Next Track command received */
#define HCI_CONTROL_AVRC_TARGET_EVENT_PREVIOUS_TRACK        ( ( HCI_CONTROL_GROUP_AVRC_TARGET << 8 ) | 0x07 )    /* Previous track command received */
#define HCI_CONTROL_AVRC_TARGET_EVENT_BEGIN_FAST_FORWARD    ( ( HCI_CONTROL_GROUP_AVRC_TARGET << 8 ) | 0x08 )    /* Next Track command received */
#define HCI_CONTROL_AVRC_TARGET_EVENT_END_FAST_FORWARD      ( ( HCI_CONTROL_GROUP_AVRC_TARGET << 8 ) | 0x09 )    /* Previous track command received */
#define HCI_CONTROL_AVRC_TARGET_EVENT_BEGIN_REWIND          ( ( HCI_CONTROL_GROUP_AVRC_TARGET << 8 ) | 0x0A )    /* Next Track command received */
#define HCI_CONTROL_AVRC_TARGET_EVENT_END_REWIND            ( ( HCI_CONTROL_GROUP_AVRC_TARGET << 8 ) | 0x0B )    /* Previous track command received */
#define HCI_CONTROL_AVRC_TARGET_EVENT_VOLUME_LEVEL          ( ( HCI_CONTROL_GROUP_AVRC_TARGET << 8 ) | 0x0C )    /* Volume Level changed received */
#define HCI_CONTROL_AVRC_TARGET_EVENT_REPEAT_SETTINGS       ( ( HCI_CONTROL_GROUP_AVRC_TARGET << 8 ) | 0x0D )    /* Repeat settings changed by peer, sent to MCU app */
#define HCI_CONTROL_AVRC_TARGET_EVENT_SHUFFLE_SETTINGS      ( ( HCI_CONTROL_GROUP_AVRC_TARGET << 8 ) | 0x0E )    /* Shuffle settings changed by peer, sent to MCU app */
#define HCI_CONTROL_AVRC_TARGET_EVENT_GET_PLAYER_STATUS     ( ( HCI_CONTROL_GROUP_AVRC_TARGET << 8 ) | 0x0F )    /* Player status info requested by peer, get info from MCU app */
#define HCI_CONTROL_AVRC_TARGET_EVENT_PASSTHROUGH_STATUS    ( ( HCI_CONTROL_GROUP_AVRC_TARGET << 8 ) | 0x10 )    /* Passthrough command status received from peer */
#define HCI_CONTROL_AVRC_TARGET_EVENT_COMMAND_STATUS        ( ( HCI_CONTROL_GROUP_AVRC_TARGET << 8 ) | 0xFF )    /* Result status for AVRCP commands */

/* Events for the AVRCP CT profile */
#define HCI_CONTROL_AVRC_CONTROLLER_EVENT_CONNECTED          ( ( HCI_CONTROL_GROUP_AVRC_CONTROLLER << 8 ) | 0x01 )    /* AVRCP Controller connected */
#define HCI_CONTROL_AVRC_CONTROLLER_EVENT_DISCONNECTED       ( ( HCI_CONTROL_GROUP_AVRC_CONTROLLER << 8 ) | 0x02 )    /* AVRCP Controller disconnected */
#define HCI_CONTROL_AVRC_CONTROLLER_EVENT_CURRENT_TRACK_INFO ( ( HCI_CONTROL_GROUP_AVRC_CONTROLLER << 8 ) | 0x03 )    /* AVRCP Controller disconnected */
#define HCI_CONTROL_AVRC_CONTROLLER_EVENT_PLAY_STATUS        ( ( HCI_CONTROL_GROUP_AVRC_CONTROLLER << 8 ) | 0x04 )    /* AVRCP Controller Play Status Change */
#define HCI_CONTROL_AVRC_CONTROLLER_EVENT_PLAY_POSITION      ( ( HCI_CONTROL_GROUP_AVRC_CONTROLLER << 8 ) | 0x05 )    /* AVRCP Controller Play Position Change */
#define HCI_CONTROL_AVRC_CONTROLLER_EVENT_TRACK_CHANGE       ( ( HCI_CONTROL_GROUP_AVRC_CONTROLLER << 8 ) | 0x06 )    /* AVRCP Controller Track Changed */
#define HCI_CONTROL_AVRC_CONTROLLER_EVENT_TRACK_END          ( ( HCI_CONTROL_GROUP_AVRC_CONTROLLER << 8 ) | 0x07 )    /* AVRCP Controller Track reached End */
#define HCI_CONTROL_AVRC_CONTROLLER_EVENT_TRACK_START        ( ( HCI_CONTROL_GROUP_AVRC_CONTROLLER << 8 ) | 0x08 )    /* AVRCP Controller Track reached Start */
#define HCI_CONTROL_AVRC_CONTROLLER_EVENT_SETTING_AVAILABLE  ( ( HCI_CONTROL_GROUP_AVRC_CONTROLLER << 8 ) | 0x09 )    /* AVRCP Controller Player setting available */
#define HCI_CONTROL_AVRC_CONTROLLER_EVENT_SETTING_CHANGE     ( ( HCI_CONTROL_GROUP_AVRC_CONTROLLER << 8 ) | 0x0a )    /* AVRCP Controller Player setting changed */
#define HCI_CONTROL_AVRC_CONTROLLER_EVENT_PLAYER_CHANGE      ( ( HCI_CONTROL_GROUP_AVRC_CONTROLLER << 8 ) | 0x0b )    /* AVRCP Controller Player changed */
#define HCI_CONTROL_AVRC_CONTROLLER_EVENT_PLAY_STATUS_INFO   ( ( HCI_CONTROL_GROUP_AVRC_CONTROLLER << 8 ) | 0x0c )    /* AVRCP Controller Play status response */
#define HCI_CONTROL_AVRC_CONTROLLER_EVENT_COMMAND_STATUS     ( ( HCI_CONTROL_GROUP_AVRC_CONTROLLER << 8 ) | 0xFF )    /* Result status for AVRCP commands */

/* Events for the HID Device profile */
#define HCI_CONTROL_HIDD_EVENT_OPENED                       ( ( HCI_CONTROL_GROUP_HIDD << 8 ) | 0x01 )    /* Both HID channels are opened */
#define HCI_CONTROL_HIDD_EVENT_VIRTUAL_CABLE_UNPLUGGED      ( ( HCI_CONTROL_GROUP_HIDD << 8 ) | 0x02 )    /* Host requested Virtual Cable Unplug */
#define HCI_CONTROL_HIDD_EVENT_DATA                         ( ( HCI_CONTROL_GROUP_HIDD << 8 ) | 0x03 )    /* Host sent report */
#define HCI_CONTROL_HIDD_EVENT_CLOSED                       ( ( HCI_CONTROL_GROUP_HIDD << 8 ) | 0x04 )    /* Host attempt to establish connection failed */
#define HCI_CONTROL_HIDD_EVENT_COMMAND_STATUS               ( ( HCI_CONTROL_GROUP_HIDD << 8 ) | 0xFF )    /* Result status for HID commands */

#define HCI_CONTROL_HID_EVENT_OPENED                        HCI_CONTROL_HIDD_EVENT_OPENED
#define HCI_CONTROL_HID_EVENT_VIRTUAL_CABLE_UNPLUGGED       HCI_CONTROL_HIDD_EVENT_VIRTUAL_CABLE_UNPLUGGED
#define HCI_CONTROL_HID_EVENT_DATA                          HCI_CONTROL_HIDD_EVENT_DATA
#define HCI_CONTROL_HID_EVENT_CLOSED                        HCI_CONTROL_HIDD_EVENT_CLOSED
#define HCI_CONTROL_HID_EVENT_COMMAND_STATUS                HCI_CONTROL_HIDD_EVENT_COMMAND_STATUS


/* Events for the Test events group */
#define HCI_CONTROL_TEST_EVENT_ENCAPSULATED_HCI_EVENT       ( ( HCI_CONTROL_GROUP_TEST << 8 ) | 0x01 )     /* Encapsulated HCI Event message */

/* Events for ANCS */
#define HCI_CONTROL_ANCS_EVENT_NOTIFICATION                 ( ( HCI_CONTROL_GROUP_ANCS << 8 ) | 0x01 )      /* ANCS notification */
#define HCI_CONTROL_ANCS_EVENT_COMMAND_STATUS               ( ( HCI_CONTROL_GROUP_ANCS << 8 ) | 0x02 )      /* Command status event for the requested operation */
#define HCI_CONTROL_ANCS_EVENT_SERVICE_FOUND                ( ( HCI_CONTROL_GROUP_ANCS << 8 ) | 0x03 )      /* ANCS event for service found */
#define HCI_CONTROL_ANCS_EVENT_CONNECTED                    ( ( HCI_CONTROL_GROUP_ANCS << 8 ) | 0x04 )      /* ANCS event for service connected */
#define HCI_CONTROL_ANCS_EVENT_DISCONNECTED                 ( ( HCI_CONTROL_GROUP_ANCS << 8 ) | 0x05 )      /* ANCS event for service disconnected */

/* Events for ANC */
#define HCI_CONTROL_ANC_EVENT_ANC_ENABLED                           ( ( HCI_CONTROL_GROUP_ANC << 8 ) | 0x01 )   /* ANC event when connected to ANS */
#define HCI_CONTROL_ANC_EVENT_SERVER_SUPPORTED_NEW_ALERTS           ( ( HCI_CONTROL_GROUP_ANC << 8 ) | 0x02 )   /* ANC event on complete of read server supported new alerts */
#define HCI_CONTROL_ANC_EVENT_SERVER_SUPPORTED_UNREAD_ALERTS        ( ( HCI_CONTROL_GROUP_ANC << 8 ) | 0x03 )   /* ANC event on complete of read server supported unread alerts */
#define HCI_CONTROL_ANC_EVENT_CONTROL_ALERTS                        ( ( HCI_CONTROL_GROUP_ANC << 8 ) | 0x04 )   /* ANC event on complete of control alerts configuration */
#define HCI_CONTROL_ANC_EVENT_ENABLE_NEW_ALERTS                     ( ( HCI_CONTROL_GROUP_ANC << 8 ) | 0x05 )   /* ANC event on complete of enable new alerts */
#define HCI_CONTROL_ANC_EVENT_DISABLE_NEW_ALERTS                    ( ( HCI_CONTROL_GROUP_ANC << 8 ) | 0x06 )   /* ANC event on complete of disable new alerts */
#define HCI_CONTROL_ANC_EVENT_ENABLE_UNREAD_ALERTS                  ( ( HCI_CONTROL_GROUP_ANC << 8 ) | 0x07 )   /* ANC event on complete of enable unread alerts */
#define HCI_CONTROL_ANC_EVENT_DISABLE_UNREAD_ALERTS                 ( ( HCI_CONTROL_GROUP_ANC << 8 ) | 0x08 )   /* ANC event on complete of disable unread alerts */
#define HCI_CONTROL_ANC_EVENT_ANC_DISABLED                          ( ( HCI_CONTROL_GROUP_ANC << 8 ) | 0x09 )   /* ANC event when disconnected from ANS */
#define HCI_CONTROL_ANC_EVENT_COMMAND_STATUS                        ( ( HCI_CONTROL_GROUP_ANC << 8 ) | 0x0A )   /* Command status event for the requested operation */

/* Events for ANS */
#define HCI_CONTROL_ANS_EVENT_COMMAND_STATUS                ( ( HCI_CONTROL_GROUP_ANS << 8 ) | 0x01 )   /* Command status event for the requested operation */
#define HCI_CONTROL_ANS_EVENT_ANS_ENABLED                   ( ( HCI_CONTROL_GROUP_ANS << 8 ) | 0x02 )   /* ANS event for Alert server is up and running */
#define HCI_CONTROL_ANS_EVENT_CONNECTION_UP                 ( ( HCI_CONTROL_GROUP_ANS << 8 ) | 0x03 )   /* ANS event whenconnected to ANS client */
#define HCI_CONTROL_ANS_EVENT_CONNECTION_DOWN               ( ( HCI_CONTROL_GROUP_ANS << 8 ) | 0x04 )   /* ANS event when disconnected from ANS client */

/* Events for AMS */
#define HCI_CONTROL_AMS_EVENT_COMMAND_STATUS                ( ( HCI_CONTROL_GROUP_AMS << 8 ) | 0x01 )       /* Command status event for the requested operation */
#define HCI_CONTROL_AMS_EVENT_SERVICE_FOUND                 ( ( HCI_CONTROL_GROUP_AMS << 8 ) | 0x02 )       /* AMS event for service found */
#define HCI_CONTROL_AMS_EVENT_CONNECTED                     ( ( HCI_CONTROL_GROUP_AMS << 8 ) | 0x03 )       /* AMS event for service connected */
#define HCI_CONTROL_AMS_EVENT_DISCONNECTED                  ( ( HCI_CONTROL_GROUP_AMS << 8 ) | 0x04 )       /* AMS event for service disconnected */

/* Events for the FindMe application */
#define HCI_CONTROL_ALERT_EVENT_NOTIFICATION                ( ( HCI_CONTROL_GROUP_ALERT << 8 ) | 0x01 )     /* Alert Level Notification */

/* Events for IAP2 */
#define HCI_CONTROL_IAP2_EVENT_CONNECTED                    ( ( HCI_CONTROL_GROUP_IAP2 << 8 ) | 0x01 )    /* IAP2 connection opened */
#define HCI_CONTROL_IAP2_EVENT_SERVICE_NOT_FOUND            ( ( HCI_CONTROL_GROUP_IAP2 << 8 ) | 0x02 )    /* SDP record with IAP2 service not found */
#define HCI_CONTROL_IAP2_EVENT_CONNECTION_FAILED            ( ( HCI_CONTROL_GROUP_IAP2 << 8 ) | 0x03 )    /* Connection attempt failed  */
#define HCI_CONTROL_IAP2_EVENT_DISCONNECTED                 ( ( HCI_CONTROL_GROUP_IAP2 << 8 ) | 0x04 )    /* IAP2 connection closed */
#define HCI_CONTROL_IAP2_EVENT_TX_COMPLETE                  ( ( HCI_CONTROL_GROUP_IAP2 << 8 ) | 0x05 )    /* Data packet has been queued for transmission */
#define HCI_CONTROL_IAP2_EVENT_RX_DATA                      ( ( HCI_CONTROL_GROUP_IAP2 << 8 ) | 0x06 )    /* IAP2 data received */
#define HCI_CONTROL_IAP2_EVENT_AUTH_CHIP_INFO               ( ( HCI_CONTROL_GROUP_IAP2 << 8 ) | 0x07 )    /* IAP2 auth chip info */
#define HCI_CONTROL_IAP2_EVENT_AUTH_CHIP_CERTIFICATE        ( ( HCI_CONTROL_GROUP_IAP2 << 8 ) | 0x08 )    /* IAP2 auth chip certificate */
#define HCI_CONTROL_IAP2_EVENT_AUTH_CHIP_SIGNATURE          ( ( HCI_CONTROL_GROUP_IAP2 << 8 ) | 0x09 )    /* IAP2 auth chip signature */

/* Events for Handsfree AG */
#define HCI_CONTROL_AG_EVENT_OPEN                           ( ( HCI_CONTROL_GROUP_AG << 8 ) | 0x01 )
#define HCI_CONTROL_AG_EVENT_CLOSE                          ( ( HCI_CONTROL_GROUP_AG << 8 ) | 0x02 )
#define HCI_CONTROL_AG_EVENT_CONNECTED                      ( ( HCI_CONTROL_GROUP_AG << 8 ) | 0x03 )
#define HCI_CONTROL_AG_EVENT_AUDIO_OPEN                     ( ( HCI_CONTROL_GROUP_AG << 8 ) | 0x04 )
#define HCI_CONTROL_AG_EVENT_AUDIO_CLOSE                    ( ( HCI_CONTROL_GROUP_AG << 8 ) | 0x05 )

/* Events for the Broadcom Serial over GATT profile */
#define HCI_CONTROL_BSG_EVENT_CONNECTED                     ( ( HCI_CONTROL_GROUP_BSG << 8 ) | 0x01 )    /* BSG Connected */
#define HCI_CONTROL_BSG_EVENT_DISCONNECTED                  ( ( HCI_CONTROL_GROUP_BSG << 8 ) | 0x02 )    /* BSG Disconnected */
#define HCI_CONTROL_BSG_EVENT_TX_COMPLETE                   ( ( HCI_CONTROL_GROUP_BSG << 8 ) | 0x03 )    /* Data packet has been queued for transmission */
#define HCI_CONTROL_BSG_EVENT_RX_DATA                       ( ( HCI_CONTROL_GROUP_BSG << 8 ) | 0x04 )    /* BSG data received */

/* HID Host events */
#define HCI_CONTROL_HIDH_EVENT_CONNECTED                    ( ( HCI_CONTROL_GROUP_HIDH << 8 ) | 0x01 )      /* HIDH Connected */
#define HCI_CONTROL_HIDH_EVENT_DISCONNECTED                 ( ( HCI_CONTROL_GROUP_HIDH << 8 ) | 0x02 )      /* HIDH Disconnected */
#define HCI_CONTROL_HIDH_EVENT_DESCRIPTOR                   ( ( HCI_CONTROL_GROUP_HIDH << 8 ) | 0x03 )      /* HIDH Descriptor */
#define HCI_CONTROL_HIDH_EVENT_SET_REPORT                   ( ( HCI_CONTROL_GROUP_HIDH << 8 ) | 0x04 )      /* HIDH Set report */
#define HCI_CONTROL_HIDH_EVENT_GET_REPORT                   ( ( HCI_CONTROL_GROUP_HIDH << 8 ) | 0x05 )      /* HIDH Get report */
#define HCI_CONTROL_HIDH_EVENT_REPORT                       ( ( HCI_CONTROL_GROUP_HIDH << 8 ) | 0x06 )      /* HIDH Get report */
#define HCI_CONTROL_HIDH_EVENT_VIRTUAL_UNPLUG               ( ( HCI_CONTROL_GROUP_HIDH << 8 ) | 0x07 )      /* HIDH Virtual Unplug */
#define HCI_CONTROL_HIDH_EVENT_SET_PROTOCOL                 ( ( HCI_CONTROL_GROUP_HIDH << 8 ) | 0x08 )      /* HIDH Set Protocol */

#define HCI_CONTROL_HIDH_EVENT_AUDIO_START                  ( ( HCI_CONTROL_GROUP_HIDH << 8 ) | 0x09 )      /* HIDH Audio Start */
#define HCI_CONTROL_HIDH_EVENT_AUDIO_STOP                   ( ( HCI_CONTROL_GROUP_HIDH << 8 ) | 0x0A )      /* HIDH Audio Stop */
#define HCI_CONTROL_HIDH_EVENT_AUDIO_DATA                   ( ( HCI_CONTROL_GROUP_HIDH << 8 ) | 0x0B )      /* HIDH Audio Data */

#define HCI_CONTROL_HIDH_EVENT_STATUS                       ( ( HCI_CONTROL_GROUP_HIDH << 8 ) | 0xFF )      /* HIDH Command status */

/* Events for the Audio sink profile */
#define HCI_CONTROL_AUDIO_SINK_EVENT_COMMAND_STATUS         ( ( HCI_CONTROL_GROUP_AUDIO_SINK << 8 ) | 0x01 )    /* Command status event for the requested operation */
#define HCI_CONTROL_AUDIO_SINK_EVENT_CONNECTED              ( ( HCI_CONTROL_GROUP_AUDIO_SINK << 8 ) | 0x02 )    /* Audio connection opened */
#define HCI_CONTROL_AUDIO_SINK_EVENT_SERVICE_NOT_FOUND      ( ( HCI_CONTROL_GROUP_AUDIO_SINK << 8 ) | 0x03 )    /* SDP record with audio service not found */
#define HCI_CONTROL_AUDIO_SINK_EVENT_CONNECTION_FAILED      ( ( HCI_CONTROL_GROUP_AUDIO_SINK << 8 ) | 0x04 )    /* Connection attempt failed  */
#define HCI_CONTROL_AUDIO_SINK_EVENT_DISCONNECTED           ( ( HCI_CONTROL_GROUP_AUDIO_SINK << 8 ) | 0x05 )    /* Audio connection closed */
#define HCI_CONTROL_AUDIO_SINK_EVENT_STARTED                ( ( HCI_CONTROL_GROUP_AUDIO_SINK << 8 ) | 0x06 )    /* Command for audio start succeeded */
#define HCI_CONTROL_AUDIO_SINK_EVENT_STOPPED                ( ( HCI_CONTROL_GROUP_AUDIO_SINK << 8 ) | 0x07 )    /* Command for audio stop completed */
#define HCI_CONTROL_AUDIO_SINK_EVENT_CODEC_CONFIGURED       ( ( HCI_CONTROL_GROUP_AUDIO_SINK << 8 ) | 0x08 )    /* Peer codec configured event*/
#define HCI_CONTROL_AUDIO_SINK_EVENT_START_IND              ( ( HCI_CONTROL_GROUP_AUDIO_SINK << 8 ) | 0x09 )    /* A2DP Start indication event, received A2DP Start request */
#define HCI_CONTROL_AUDIO_SINK_EVENT_AUDIO_DATA             ( ( HCI_CONTROL_GROUP_AUDIO_SINK << 8 ) | 0x0a )    /* Received audio data. Encoded for AAC, decoded for SBC. */

/* PBAP Client events */
#define HCI_CONTROL_PBC_EVENT_CONNECTED                     ( ( HCI_CONTROL_GROUP_PBC << 8 ) | 0x01 )      /* PBC Connected */
#define HCI_CONTROL_PBC_EVENT_DISCONNECTED                  ( ( HCI_CONTROL_GROUP_PBC << 8 ) | 0x02 )      /* PBC Disconnected */
#define HCI_CONTROL_PBC_EVENT_PHONEBOOK                     ( ( HCI_CONTROL_GROUP_PBC << 8 ) | 0x03 )      /* Download phonebook event */
#define HCI_CONTROL_PBC_EVENT_CALL_HISTORY                  ( ( HCI_CONTROL_GROUP_PBC << 8 ) | 0x04 )      /* Call history event*/
#define HCI_CONTROL_PBC_EVENT_INCOMMING_CALLS               ( ( HCI_CONTROL_GROUP_PBC << 8 ) | 0x05 )      /* List of incoming calls event*/
#define HCI_CONTROL_PBC_EVENT_OUTGOING_CALLS                ( ( HCI_CONTROL_GROUP_PBC << 8 ) | 0x06 )      /* List of outgoing calls event*/
#define HCI_CONTROL_PBC_EVENT_MISSED_CALLS                  ( ( HCI_CONTROL_GROUP_PBC << 8 ) | 0x07 )      /* List of missed calls event*/
#define HCI_CONTROL_PBC_EVENT_ABORTED                       ( ( HCI_CONTROL_GROUP_PBC << 8 ) | 0x08 )      /* Aborted operation event */

/* LE COC Events */
#define HCI_CONTROL_LE_COC_EVENT_CONNECTED                  ( ( HCI_CONTROL_GROUP_LE_COC << 8 ) | 0x01)    /* LE COC Connected */
#define HCI_CONTROL_LE_COC_EVENT_DISCONNECTED               ( ( HCI_CONTROL_GROUP_LE_COC << 8 ) | 0x02)    /* LE COC Disconnected */
#define HCI_CONTROL_LE_COC_EVENT_RX_DATA                    ( ( HCI_CONTROL_GROUP_LE_COC << 8 ) | 0x03)    /* Received data from peer */
#define HCI_CONTROL_LE_COC_EVENT_TX_COMPLETE                ( ( HCI_CONTROL_GROUP_LE_COC << 8 ) | 0x04)    /* Data Transmission successfull event */
#define HCI_CONTROL_LE_COC_EVENT_ADV_STS                    ( ( HCI_CONTROL_GROUP_LE_COC << 8 ) | 0x05)    /* Advertising status event */

/* Mesh events */
#define HCI_CONTROL_MESH_EVENT_COMMAND_STATUS                               ( ( HCI_CONTROL_GROUP_MESH << 8 ) | 0x00 )      /* Command status event */
#define HCI_CONTROL_MESH_EVENT_TX_COMPLETE                                  ( ( HCI_CONTROL_GROUP_MESH << 8 ) | 0x01 )      /* Tx Complete event */

#define HCI_CONTROL_MESH_EVENT_ONOFF_SET                                    ( ( HCI_CONTROL_GROUP_MESH << 8 ) | 0x08 )      /* ON/OFF Server Set */
#define HCI_CONTROL_MESH_EVENT_ONOFF_STATUS                                 ( ( HCI_CONTROL_GROUP_MESH << 8 ) | 0x09 )      /* ON/OFF Client status */

#define HCI_CONTROL_MESH_EVENT_LEVEL_SET                                    ( ( HCI_CONTROL_GROUP_MESH << 8 ) | 0x0c )      /* Level Server Set */
#define HCI_CONTROL_MESH_EVENT_LEVEL_STATUS                                 ( ( HCI_CONTROL_GROUP_MESH << 8 ) | 0x0d )      /* Level Client status */

#define HCI_CONTROL_MESH_EVENT_LOCATION_GLOBAL_SET                          ( ( HCI_CONTROL_GROUP_MESH << 8 ) | 0x10 )      /* Set Global Location data */
#define HCI_CONTROL_MESH_EVENT_LOCATION_LOCAL_SET                           ( ( HCI_CONTROL_GROUP_MESH << 8 ) | 0x11 )      /* Set Local Location data */
#define HCI_CONTROL_MESH_EVENT_LOCATION_GLOBAL_STATUS                       ( ( HCI_CONTROL_GROUP_MESH << 8 ) | 0x12 )      /* Global Location data changed */
#define HCI_CONTROL_MESH_EVENT_LOCATION_LOCAL_STATUS                        ( ( HCI_CONTROL_GROUP_MESH << 8 ) | 0x13 )      /* Local Location data changed */

#define HCI_CONTROL_MESH_EVENT_BATTERY_STATUS                               ( ( HCI_CONTROL_GROUP_MESH << 8 ) | 0x15 )      /* Battery status data */

#define HCI_CONTROL_MESH_EVENT_DEF_TRANS_TIME_STATUS                        ( ( HCI_CONTROL_GROUP_MESH << 8 ) | 0x1b )      /* Default Transition Time Client Status */

#define HCI_CONTROL_MESH_EVENT_POWER_ONOFF_STATUS                           ( ( HCI_CONTROL_GROUP_MESH << 8 ) | 0x20 )      /* Power ON/OFF Client status */

#define HCI_CONTROL_MESH_EVENT_POWER_LEVEL_SET                              ( ( HCI_CONTROL_GROUP_MESH << 8 ) | 0x25 )      /* Power Level Server Set */
#define HCI_CONTROL_MESH_EVENT_POWER_LEVEL_DEFAULT_SET                      ( ( HCI_CONTROL_GROUP_MESH << 8 ) | 0x26 )      /* Power Level Server Set Default Power Level */
#define HCI_CONTROL_MESH_EVENT_POWER_LEVEL_RANGE_SET                        ( ( HCI_CONTROL_GROUP_MESH << 8 ) | 0x27 )      /* Power Level Server Set Min/Max Power Level range */
#define HCI_CONTROL_MESH_EVENT_POWER_LEVEL_STATUS                           ( ( HCI_CONTROL_GROUP_MESH << 8 ) | 0x28 )      /* Power Level Client Status */
#define HCI_CONTROL_MESH_EVENT_POWER_LEVEL_LAST_STATUS                      ( ( HCI_CONTROL_GROUP_MESH << 8 ) | 0x29 )      /* Last Power Level Client status */
#define HCI_CONTROL_MESH_EVENT_POWER_LEVEL_DEFAULT_STATUS                   ( ( HCI_CONTROL_GROUP_MESH << 8 ) | 0x2a )      /* Default Power Level Client status */
#define HCI_CONTROL_MESH_EVENT_POWER_LEVEL_RANGE_STATUS                     ( ( HCI_CONTROL_GROUP_MESH << 8 ) | 0x2b )      /* Default Power Level Client status */

#define HCI_CONTROL_MESH_EVENT_PROPERTY_SET                                 ( ( HCI_CONTROL_GROUP_MESH << 8 ) | 0x30 )      /* Set Value of the Property */
#define HCI_CONTROL_MESH_EVENT_PROPERTIES_STATUS                            ( ( HCI_CONTROL_GROUP_MESH << 8 ) | 0x31 )      /* List of Properties reported by the Server */
#define HCI_CONTROL_MESH_EVENT_PROPERTY_STATUS                              ( ( HCI_CONTROL_GROUP_MESH << 8 ) | 0x32 )      /* Value of the Property Changed status*/

#define HCI_CONTROL_MESH_EVENT_LIGHT_LIGHTNESS_SET                          ( ( HCI_CONTROL_GROUP_MESH << 8 ) | 0x38 )      /* Light Lightness Server Set */
#define HCI_CONTROL_MESH_EVENT_LIGHT_LIGHTNESS_STATUS                       ( ( HCI_CONTROL_GROUP_MESH << 8 ) | 0x39 )      /* Light Lightness Client status */
#define HCI_CONTROL_MESH_EVENT_LIGHT_LIGHTNESS_LINEAR_STATUS                ( ( HCI_CONTROL_GROUP_MESH << 8 ) | 0x3a )      /* Light Lightness Client status */
#define HCI_CONTROL_MESH_EVENT_LIGHT_LIGHTNESS_LAST_STATUS                  ( ( HCI_CONTROL_GROUP_MESH << 8 ) | 0x3b )      /* Last Light Lightness Client status */
#define HCI_CONTROL_MESH_EVENT_LIGHT_LIGHTNESS_DEFAULT_STATUS               ( ( HCI_CONTROL_GROUP_MESH << 8 ) | 0x3c )      /* Default Light Lightness Client status */
#define HCI_CONTROL_MESH_EVENT_LIGHT_LIGHTNESS_RANGE_STATUS                 ( ( HCI_CONTROL_GROUP_MESH << 8 ) | 0x3d )      /* Range Light Lightness Client status */
#define HCI_CONTROL_MESH_EVENT_LIGHT_LIGHTNESS_RANGE_SET                    ( ( HCI_CONTROL_GROUP_MESH << 8 ) | 0x3e )      /* Light Lightness Server Range Set event */

#define HCI_CONTROL_MESH_EVENT_LIGHT_CTL_STATUS                             ( ( HCI_CONTROL_GROUP_MESH << 8 ) | 0x40 )  /* Client Light CTL status event */
#define HCI_CONTROL_MESH_EVENT_LIGHT_CTL_TEMPERATURE_STATUS                 ( ( HCI_CONTROL_GROUP_MESH << 8 ) | 0x41 )  /* Client Light CTL Temperature status event */
#define HCI_CONTROL_MESH_EVENT_LIGHT_CTL_TEMPERATURE_RANGE_STATUS           ( ( HCI_CONTROL_GROUP_MESH << 8 ) | 0x42 )  /* Client Light CTL Temperature Range status event */
#define HCI_CONTROL_MESH_EVENT_LIGHT_CTL_DEFAULT_STATUS                     ( ( HCI_CONTROL_GROUP_MESH << 8 ) | 0x43 )  /* Client Light CTL Default status event */
#define HCI_CONTROL_MESH_EVENT_LIGHT_CTL_SET                                ( ( HCI_CONTROL_GROUP_MESH << 8 ) | 0x44 )  /* Server Light CTL Set event */
#define HCI_CONTROL_MESH_EVENT_LIGHT_CTL_TEMPERATURE_SET                    ( ( HCI_CONTROL_GROUP_MESH << 8 ) | 0x45 )  /* Server Light CTL Temperature Set event */
#define HCI_CONTROL_MESH_EVENT_LIGHT_CTL_TEMPERATURE_RANGE_SET              ( ( HCI_CONTROL_GROUP_MESH << 8 ) | 0x46 )  /* Server Light CTL Temperature Range Set event */
#define HCI_CONTROL_MESH_EVENT_LIGHT_CTL_DEFAULT_SET                        ( ( HCI_CONTROL_GROUP_MESH << 8 ) | 0x47 )  /* Server Light CTL Default Set event */

#define HCI_CONTROL_MESH_EVENT_LIGHT_HSL_SET                                ( ( HCI_CONTROL_GROUP_MESH << 8 ) | 0x50 )  /* Server Light HSL Set event */
#define HCI_CONTROL_MESH_EVENT_LIGHT_HSL_STATUS                             ( ( HCI_CONTROL_GROUP_MESH << 8 ) | 0x51 )  /* Client Light HSL status event */
#define HCI_CONTROL_MESH_EVENT_LIGHT_HSL_TARGET_STATUS                      ( ( HCI_CONTROL_GROUP_MESH << 8 ) | 0x52 )  /* Client Light HSL status event */
#define HCI_CONTROL_MESH_EVENT_LIGHT_HSL_RANGE_SET                          ( ( HCI_CONTROL_GROUP_MESH << 8 ) | 0x53 )  /* Server Light HSL Temperature Range Set event */
#define HCI_CONTROL_MESH_EVENT_LIGHT_HSL_RANGE_STATUS                       ( ( HCI_CONTROL_GROUP_MESH << 8 ) | 0x54 )  /* Client Light HSL Default status event */
#define HCI_CONTROL_MESH_EVENT_LIGHT_HSL_DEFAULT_SET                        ( ( HCI_CONTROL_GROUP_MESH << 8 ) | 0x55 )  /* Server Light HSL Default Set event */
#define HCI_CONTROL_MESH_EVENT_LIGHT_HSL_DEFAULT_STATUS                     ( ( HCI_CONTROL_GROUP_MESH << 8 ) | 0x56 )  /* Client Light HSL Default status event */
#define HCI_CONTROL_MESH_EVENT_LIGHT_HSL_HUE_SET                            ( ( HCI_CONTROL_GROUP_MESH << 8 ) | 0x57 )  /* Server Light HSL Set event */
#define HCI_CONTROL_MESH_EVENT_LIGHT_HSL_HUE_STATUS                         ( ( HCI_CONTROL_GROUP_MESH << 8 ) | 0x58 )  /* Client Light HSL status event */
#define HCI_CONTROL_MESH_EVENT_LIGHT_HSL_SATURATION_SET                     ( ( HCI_CONTROL_GROUP_MESH << 8 ) | 0x59 )  /* Server Light HSL Set event */
#define HCI_CONTROL_MESH_EVENT_LIGHT_HSL_SATURATION_STATUS                  ( ( HCI_CONTROL_GROUP_MESH << 8 ) | 0x5a )  /* Client Light HSL status event */

#define HCI_CONTROL_MESH_EVENT_LIGHT_XYL_SET                                ( ( HCI_CONTROL_GROUP_MESH << 8 ) | 0x60 )  /* Server Light XYL Set event */
#define HCI_CONTROL_MESH_EVENT_LIGHT_XYL_STATUS                             ( ( HCI_CONTROL_GROUP_MESH << 8 ) | 0x61 )  /* Client Light XYL status event */
#define HCI_CONTROL_MESH_EVENT_LIGHT_XYL_TARGET_STATUS                      ( ( HCI_CONTROL_GROUP_MESH << 8 ) | 0x62 )  /* Client Light XYL status event */
#define HCI_CONTROL_MESH_EVENT_LIGHT_XYL_RANGE_SET                          ( ( HCI_CONTROL_GROUP_MESH << 8 ) | 0x63 )  /* Server Light XYL Temperature Range Set event */
#define HCI_CONTROL_MESH_EVENT_LIGHT_XYL_RANGE_STATUS                       ( ( HCI_CONTROL_GROUP_MESH << 8 ) | 0x64 )  /* Client Light XYL Default status event */
#define HCI_CONTROL_MESH_EVENT_LIGHT_XYL_DEFAULT_SET                        ( ( HCI_CONTROL_GROUP_MESH << 8 ) | 0x65 )  /* Server Light XYL Default Set event */
#define HCI_CONTROL_MESH_EVENT_LIGHT_XYL_DEFAULT_STATUS                     ( ( HCI_CONTROL_GROUP_MESH << 8 ) | 0x66 )  /* Client Light XYL Default status event */

#define HCI_CONTROL_MESH_EVENT_LIGHT_LC_MODE_SERVER_SET                     ( ( HCI_CONTROL_GROUP_MESH << 8 ) | 0x70 )  /* Light LC Server mode Set event */
#define HCI_CONTROL_MESH_EVENT_LIGHT_LC_MODE_CLIENT_STATUS                  ( ( HCI_CONTROL_GROUP_MESH << 8 ) | 0x71 )  /* Light LC Client mode status event */
#define HCI_CONTROL_MESH_EVENT_LIGHT_LC_OCCUPANCY_MODE_SERVER_SET           ( ( HCI_CONTROL_GROUP_MESH << 8 ) | 0x72 )  /* Light LC Server Occupancy mode Set event */
#define HCI_CONTROL_MESH_EVENT_LIGHT_LC_OCCUPANCY_MODE_CLIENT_STATUS        ( ( HCI_CONTROL_GROUP_MESH << 8 ) | 0x73 )  /* Light LC Client Occupancy mode status Event */
#define HCI_CONTROL_MESH_EVENT_LIGHT_LC_ONOFF_SERVER_SET                    ( ( HCI_CONTROL_GROUP_MESH << 8 ) | 0x74 )  /* Light LC Server OnOff Set event */
#define HCI_CONTROL_MESH_EVENT_LIGHT_LC_ONOFF_CLIENT_STATUS                 ( ( HCI_CONTROL_GROUP_MESH << 8 ) | 0x75 )  /* Light LC Client OnOff status event */
#define HCI_CONTROL_MESH_EVENT_LIGHT_LC_PROPERTY_SERVER_SET                 ( ( HCI_CONTROL_GROUP_MESH << 8 ) | 0x76 )  /* Light LC Server Property Set event */
#define HCI_CONTROL_MESH_EVENT_LIGHT_LC_PROPERTY_CLIENT_STATUS              ( ( HCI_CONTROL_GROUP_MESH << 8 ) | 0x77 )  /* Light LC Client Property status event */

#define HCI_CONTROL_MESH_EVENT_SENSOR_DESCRIPTOR_STATUS                     ( ( HCI_CONTROL_GROUP_MESH << 8 ) | 0x80 )  /* Value of the Sensor Descriptor status*/
#define HCI_CONTROL_MESH_EVENT_SENSOR_STATUS                                ( ( HCI_CONTROL_GROUP_MESH << 8 ) | 0x81 )  /* Value of the Sensor status*/
#define HCI_CONTROL_MESH_EVENT_SENSOR_COLUMN_STATUS                         ( ( HCI_CONTROL_GROUP_MESH << 8 ) | 0x82 )  /* Value of the Sensor Column status*/
#define HCI_CONTROL_MESH_EVENT_SENSOR_SERIES_STATUS                         ( ( HCI_CONTROL_GROUP_MESH << 8 ) | 0x83 )  /* Value of the Sensor Series status*/
#define HCI_CONTROL_MESH_EVENT_SENSOR_CADENCE_STATUS                        ( ( HCI_CONTROL_GROUP_MESH << 8 ) | 0x84 )  /* Value of the Sensor Cadence status*/
#define HCI_CONTROL_MESH_EVENT_SENSOR_SETTING_STATUS                        ( ( HCI_CONTROL_GROUP_MESH << 8 ) | 0x85 )  /* Value of the Sensor Setting status*/
#define HCI_CONTROL_MESH_EVENT_SENSOR_SETTINGS_STATUS                       ( ( HCI_CONTROL_GROUP_MESH << 8 ) | 0x86 )  /* Value of the Sensor Settings status*/
#define HCI_CONTROL_MESH_EVENT_SENSOR_CADENCE_GET                           ( ( HCI_CONTROL_GROUP_MESH << 8 ) | 0x87 )  /* Sensor Cadence Get */
#define HCI_CONTROL_MESH_EVENT_SENSOR_CADENCE_SET                           ( ( HCI_CONTROL_GROUP_MESH << 8 ) | 0x88 )  /* Sensor Cadence Set */
#define HCI_CONTROL_MESH_EVENT_SENSOR_SETTING_GET                           ( ( HCI_CONTROL_GROUP_MESH << 8 ) | 0x89 )  /* Sensor Setting Get */
#define HCI_CONTROL_MESH_EVENT_SENSOR_GET                                   ( ( HCI_CONTROL_GROUP_MESH << 8 ) | 0x8a )  /* Sensor Get */
#define HCI_CONTROL_MESH_EVENT_SENSOR_COLUMN_GET                            ( ( HCI_CONTROL_GROUP_MESH << 8 ) | 0x8b )  /* Sensor Column Get */
#define HCI_CONTROL_MESH_EVENT_SENSOR_SERIES_GET                            ( ( HCI_CONTROL_GROUP_MESH << 8 ) | 0x8c )  /* Sensor Series Get */
#define HCI_CONTROL_MESH_EVENT_SENSOR_SETTING_SET                           ( ( HCI_CONTROL_GROUP_MESH << 8 ) | 0x8d )  /* Sensor Setting Set */

#define HCI_CONTROL_MESH_EVENT_SCENE_STATUS                                 ( ( HCI_CONTROL_GROUP_MESH << 8 ) | 0x90 )  /* Scene status event */
#define HCI_CONTROL_MESH_EVENT_SCENE_REGISTER_STATUS                        ( ( HCI_CONTROL_GROUP_MESH << 8 ) | 0x91 )  /* Scene register status event */

#define HCI_CONTROL_MESH_EVENT_SCHEDULER_STATUS                             ( ( HCI_CONTROL_GROUP_MESH << 8 ) | 0x98 )  /* Scheduler register status event */
#define HCI_CONTROL_MESH_EVENT_SCHEDULER_ACTION_STATUS                      ( ( HCI_CONTROL_GROUP_MESH << 8 ) | 0x99 )  /* Scheduler action status event */

#define HCI_CONTROL_MESH_EVENT_TIME_STATUS                                  ( ( HCI_CONTROL_GROUP_MESH << 8 ) | 0xa0 )  /* Time Status event */
#define HCI_CONTROL_MESH_EVENT_TIME_ZONE_STATUS                             ( ( HCI_CONTROL_GROUP_MESH << 8 ) | 0xa1 )  /* Time Zone status event */
#define HCI_CONTROL_MESH_EVENT_TIME_TAI_UTC_DELTA_STATUS                    ( ( HCI_CONTROL_GROUP_MESH << 8 ) | 0xa2 )  /* Time TAI_UTC Delta status event */
#define HCI_CONTROL_MESH_EVENT_TIME_ROLE_STATUS                             ( ( HCI_CONTROL_GROUP_MESH << 8 ) | 0xa3 )  /* Time Role status event */
#define HCI_CONTROL_MESH_EVENT_TIME_SET                                     ( ( HCI_CONTROL_GROUP_MESH << 8 ) | 0xa4 )  /* Time Set event */

#define HCI_CONTROL_MESH_EVENT_PROVISION_SCAN_CAPABILITIES_STATUS           ( ( HCI_CONTROL_GROUP_MESH << 8 ) | 0xb0 )  /* Remote Provisioning Scanning Server Information */
#define HCI_CONTROL_MESH_EVENT_PROVISION_SCAN_STATUS                        ( ( HCI_CONTROL_GROUP_MESH << 8 ) | 0xb1 )  /* Remote Provisioning Scan Parameters */
#define HCI_CONTROL_MESH_EVENT_PROVISION_SCAN_REPORT                        ( ( HCI_CONTROL_GROUP_MESH << 8 ) | 0xb2 )  /* Remote Provisioning Report the scanned Device UUID */
#define HCI_CONTROL_MESH_EVENT_PROVISION_SCAN_EXTENDED_REPORT               ( ( HCI_CONTROL_GROUP_MESH << 8 ) | 0xb3 )  /* Remote Provisioning report the Advertising data containing the defined AD Type */
#define HCI_CONTROL_MESH_EVENT_PROVISION_LINK_REPORT                        ( ( HCI_CONTROL_GROUP_MESH << 8 ) | 0xb4 )  /* Provision link established or dropped */
#define HCI_CONTROL_MESH_EVENT_PROVISION_END                                ( ( HCI_CONTROL_GROUP_MESH << 8 ) | 0xb5 )  /* Provision end event */
#define HCI_CONTROL_MESH_EVENT_PROVISION_DEVICE_CAPABITIES                  ( ( HCI_CONTROL_GROUP_MESH << 8 ) | 0xb6 )  /* Provisioning device capabilities */
#define HCI_CONTROL_MESH_EVENT_PROVISION_OOB_DATA                           ( ( HCI_CONTROL_GROUP_MESH << 8 ) | 0xb7 )  /* Provisioning OOB data request */
#define HCI_CONTROL_MESH_EVENT_PROXY_DEVICE_NETWORK_DATA                    ( ( HCI_CONTROL_GROUP_MESH << 8 ) | 0xb8 )  /* Proxy device network data event */
#define HCI_CONTROL_MESH_EVENT_PROXY_DATA                                   ( ( HCI_CONTROL_GROUP_MESH << 8 ) | 0xb9 )  /* Proxy device network data event */

#define HCI_CONTROL_MESH_EVENT_RAW_MODEL_DATA                               ( ( HCI_CONTROL_GROUP_MESH << 8 ) | 0xbf )  /* Raw model data from the access layer */

#define HCI_CONTROL_MESH_EVENT_PROXY_CONNECTION_STATUS                      ( ( HCI_CONTROL_GROUP_MESH << 8 ) | 0xe0 )  /* Proxy connection status */

#define HCI_CONTROL_MESH_EVENT_NODE_RESET_STATUS                            ( ( HCI_CONTROL_GROUP_MESH << 8 ) | 0xe1 )  /* Config Node Reset status */
#define HCI_CONTROL_MESH_EVENT_COMPOSITION_DATA_STATUS                      ( ( HCI_CONTROL_GROUP_MESH << 8 ) | 0xe2 )  /* Config Composition Data status */
#define HCI_CONTROL_MESH_EVENT_FRIEND_STATUS                                ( ( HCI_CONTROL_GROUP_MESH << 8 ) | 0xe3 )  /* Config Friend status */
#define HCI_CONTROL_MESH_EVENT_GATT_PROXY_STATUS                            ( ( HCI_CONTROL_GROUP_MESH << 8 ) | 0xe4 )  /* Config GATT Proxy status */
#define HCI_CONTROL_MESH_EVENT_RELAY_STATUS                                 ( ( HCI_CONTROL_GROUP_MESH << 8 ) | 0xe5 )  /* Config Relay status */
#define HCI_CONTROL_MESH_EVENT_DEFAULT_TTL_STATUS                           ( ( HCI_CONTROL_GROUP_MESH << 8 ) | 0xe6 )  /* Config Default TTL status */
#define HCI_CONTROL_MESH_EVENT_BEACON_STATUS                                ( ( HCI_CONTROL_GROUP_MESH << 8 ) | 0xe7 )  /* Config Beacon status */
#define HCI_CONTROL_MESH_EVENT_NODE_IDENTITY_STATUS                         ( ( HCI_CONTROL_GROUP_MESH << 8 ) | 0xe8 )  /* Config Node Identity status */
#define HCI_CONTROL_MESH_EVENT_MODEL_PUBLICATION_STATUS                     ( ( HCI_CONTROL_GROUP_MESH << 8 ) | 0xe9 )  /* Config Model Publication status */
#define HCI_CONTROL_MESH_EVENT_MODEL_SUBSCRIPTION_STATUS                    ( ( HCI_CONTROL_GROUP_MESH << 8 ) | 0xea )  /* Config Model Subscription status */
#define HCI_CONTROL_MESH_EVENT_MODEL_SUBSCRIPTION_LIST                      ( ( HCI_CONTROL_GROUP_MESH << 8 ) | 0xeb )  /* Config Model Subscription List */
#define HCI_CONTROL_MESH_EVENT_NETKEY_STATUS                                ( ( HCI_CONTROL_GROUP_MESH << 8 ) | 0xec )  /* Config NetKey status */
#define HCI_CONTROL_MESH_EVENT_NETKEY_LIST                                  ( ( HCI_CONTROL_GROUP_MESH << 8 ) | 0xed )  /* Config Netkey List */
#define HCI_CONTROL_MESH_EVENT_APPKEY_STATUS                                ( ( HCI_CONTROL_GROUP_MESH << 8 ) | 0xee )  /* Config AppKey status */
#define HCI_CONTROL_MESH_EVENT_APPKEY_LIST                                  ( ( HCI_CONTROL_GROUP_MESH << 8 ) | 0xef )  /* Config Appkey List */
#define HCI_CONTROL_MESH_EVENT_MODEL_APP_BIND_STATUS                        ( ( HCI_CONTROL_GROUP_MESH << 8 ) | 0xf0 )  /* Config Model App status */
#define HCI_CONTROL_MESH_EVENT_MODEL_APP_LIST                               ( ( HCI_CONTROL_GROUP_MESH << 8 ) | 0xf1 )  /* Config Model App List */
#define HCI_CONTROL_MESH_EVENT_HEARTBEAT_SUBSCRIPTION_STATUS                ( ( HCI_CONTROL_GROUP_MESH << 8 ) | 0xf2 )  /* Config Heartbeat Subscription status */
#define HCI_CONTROL_MESH_EVENT_HEARTBEAT_PUBLICATION_STATUS                 ( ( HCI_CONTROL_GROUP_MESH << 8 ) | 0xf3 )  /* Config Heartbeat Publication status */
#define HCI_CONTROL_MESH_EVENT_NETWORK_TRANSMIT_PARAMS_STATUS               ( ( HCI_CONTROL_GROUP_MESH << 8 ) | 0xf4 )  /* Config Network Transmit status */
#define HCI_CONTROL_MESH_EVENT_HEALTH_CURRENT_STATUS                        ( ( HCI_CONTROL_GROUP_MESH << 8 ) | 0xf5 )  /* Health Current status */
#define HCI_CONTROL_MESH_EVENT_HEALTH_FAULT_STATUS                          ( ( HCI_CONTROL_GROUP_MESH << 8 ) | 0xf6 )  /* Health Fault status */
#define HCI_CONTROL_MESH_EVENT_HEALTH_PERIOD_STATUS                         ( ( HCI_CONTROL_GROUP_MESH << 8 ) | 0xf7 )  /* Health Period status */
#define HCI_CONTROL_MESH_EVENT_HEALTH_ATTENTION_STATUS                      ( ( HCI_CONTROL_GROUP_MESH << 8 ) | 0xf8 )  /* Health Attention status */
#define HCI_CONTROL_MESH_EVENT_LPN_POLL_TIMEOUT_STATUS                      ( ( HCI_CONTROL_GROUP_MESH << 8 ) | 0xf9 )  /* Low Power node Poll Timeout status */
#define HCI_CONTROL_MESH_EVENT_KEY_REFRESH_PHASE_STATUS                     ( ( HCI_CONTROL_GROUP_MESH << 8 ) | 0xfa )  /* Key Refresh Phase status */
#define HCI_CONTROL_MESH_EVENT_PROXY_FILTER_STATUS                          ( ( HCI_CONTROL_GROUP_MESH << 8 ) | 0xfb )  /* Proxy Filter status */
#define HCI_CONTROL_MESH_EVENT_CORE_SEQ_CHANGED                             ( ( HCI_CONTROL_GROUP_MESH << 8 ) | 0xfc )  /* New Sequence Number */
#define HCI_CONTROL_MESH_EVENT_NVRAM_DATA                                   ( ( HCI_CONTROL_GROUP_MESH << 8 ) | 0xfe )  /*  */
#define HCI_CONTROL_MESH_EVENT_VENDOR_DATA                                  ( ( HCI_CONTROL_GROUP_MESH << 8 ) | 0xff )  /* Vendor model data */

/* Events for the Battery Client profile */
#define HCI_CONTROL_BATT_CLIENT_EVENT_CONNECTED             ( ( HCI_CONTROL_GROUP_BATT_CLIENT << 8 ) | 0x00 )    /* Battery Client connected */
#define HCI_CONTROL_BATT_CLIENT_EVENT_DISCONNECTED          ( ( HCI_CONTROL_GROUP_BATT_CLIENT << 8 ) | 0x01 )    /* Battery Client disconnected */
#define HCI_CONTROL_BATT_CLIENT_EVENT_LEVEL                 ( ( HCI_CONTROL_GROUP_BATT_CLIENT << 8 ) | 0x02 )    /* Battery Client Level */
#define HCI_CONTROL_BATT_CLIENT_EVENT_STATUS                ( ( HCI_CONTROL_GROUP_BATT_CLIENT << 8 ) | 0xFF )    /* Battery Client status */

/* Events for the FindMe Locator Client profile */
#define HCI_CONTROL_FINDME_LOCATOR_EVENT_CONNECTED          ( ( HCI_CONTROL_GROUP_FINDME_LOCATOR << 8 ) | 0x00 )    /* FindMe Target connected */
#define HCI_CONTROL_FINDME_LOCATOR_EVENT_DISCONNECTED       ( ( HCI_CONTROL_GROUP_FINDME_LOCATOR << 8 ) | 0x01 )    /* FindMe Target disconnected */
#define HCI_CONTROL_FINDME_LOCATOR_EVENT_STATUS             ( ( HCI_CONTROL_GROUP_FINDME_LOCATOR << 8 ) | 0xFF )    /* FindMe Locator Command status */

/* OPP Server events */
#define HCI_CONTROL_OPS_EVENT_CONNECTED                     ( ( HCI_CONTROL_GROUP_OPS << 8 ) | 0x00 )      /* Connection to peer is open. */
#define HCI_CONTROL_OPS_EVENT_PROGRESS                      ( ( HCI_CONTROL_GROUP_OPS << 8 ) | 0x01 )      /* Object being sent or received. */
#define HCI_CONTROL_OPS_EVENT_OBJECT                        ( ( HCI_CONTROL_GROUP_OPS << 8 ) | 0x02 )      /* Object has been received */
#define HCI_CONTROL_OPS_EVENT_CLOSE                         ( ( HCI_CONTROL_GROUP_OPS << 8 ) | 0x03 )      /* Connection to peer closed */
#define HCI_CONTROL_OPS_EVENT_ACCESS                        ( ( HCI_CONTROL_GROUP_OPS << 8 ) | 0x04 )      /* Request for access to push or pull object */
#define HCI_CONTROL_OPS_EVENT_PUSH_DATA                     ( ( HCI_CONTROL_GROUP_OPS << 8 ) | 0x05 )      /* Push data received */

/* OTP events */
#define HCI_CONTROL_OTP_EVENT_CONNECTED                     ( ( HCI_CONTROL_GROUP_OTP << 8 ) | 0x00 )      /* Connected to Object Server */
#define HCI_CONTROL_OTP_EVENT_DISCONNECTED                  ( ( HCI_CONTROL_GROUP_OTP << 8 ) | 0x01 )      /* Disconnected from Object Server */
#define HCI_CONTROL_OTP_EVENT_DISCOVERY_COMPLETE            ( ( HCI_CONTROL_GROUP_OTP << 8 ) | 0x02 )      /* Object Server Service and Characteristics discovery complete */
#define HCI_CONTROL_OTP_EVENT_UPGRADE_INIT_DONE             ( ( HCI_CONTROL_GROUP_OTP << 8 ) | 0x03 )      /* OTP Procedures completed. Start with the firmware image transfer */
#define HCI_CONTROL_OTP_EVENT_TX_COMPLETE                   ( ( HCI_CONTROL_GROUP_OTP << 8 ) | 0x04 )      /* TX complete */

/* Miscellaneous events */
#define HCI_CONTROL_MISC_EVENT_PING_REPLY                   ( ( HCI_CONTROL_GROUP_MISC << 8 ) | 0x01 )    /* Ping reply */
#define HCI_CONTROL_MISC_EVENT_VERSION                      ( ( HCI_CONTROL_GROUP_MISC << 8 ) | 0x02 )    /* SDK Version */

/* Script events */
#define HCI_CONTROL_SCRIPT_EVENT_RET_CODE                   ( ( HCI_CONTROL_GROUP_SCRIPT << 8 ) | 0x01 )   /* Script command return code */
#define HCI_CONTROL_SCRIPT_EVENT_UNKNOWN_CMD                ( ( HCI_CONTROL_GROUP_SCRIPT << 8 ) | 0x02 )   /* Unknown Script command */
#define HCI_CONTROL_SCRIPT_EVENT_CALLBACK                   ( ( HCI_CONTROL_GROUP_SCRIPT << 8 ) | 0x03 )   /* Async script callback */
#define HCI_CONTROL_SCRIPT_TRACE                            ( ( HCI_CONTROL_GROUP_SCRIPT << 8 ) | 0xF0 )   /* Trace message (low nibble has type) */

/* Demo events */
#define HCI_CONTROL_DEMO_EVENT_SSID_PASSWD                  ( ( HCI_CONTROL_GROUP_DEMO << 8 ) | 0x01 )    /* SSID and Password */

/* Scan state events that are reported with the HCI_CONTROL_LE_EVENT_SCAN_STATUS */
#define HCI_CONTROL_SCAN_EVENT_NO_SCAN                      0
#define HCI_CONTROL_SCAN_EVENT_HIGH_SCAN                    1
#define HCI_CONTROL_SCAN_EVENT_LOW_SCAN                     2
#define HCI_CONTROL_SCAN_EVENT_HIGH_CONN                    3
#define HCI_CONTROL_SCAN_EVENT_LOW_CONN                     4

/* Status codes returned in HCI_CONTROL_EVENT_COMMAND_STATUS the event */
#define HCI_CONTROL_STATUS_SUCCESS                          0
#define HCI_CONTROL_STATUS_IN_PROGRESS                      1
#define HCI_CONTROL_STATUS_ALREADY_CONNECTED                2
#define HCI_CONTROL_STATUS_NOT_CONNECTED                    3
#define HCI_CONTROL_STATUS_BAD_HANDLE                       4
#define HCI_CONTROL_STATUS_WRONG_STATE                      5
#define HCI_CONTROL_STATUS_INVALID_ARGS                     6
#define HCI_CONTROL_STATUS_FAILED                           7
#define HCI_CONTROL_STATUS_UNKNOWN_GROUP                    8
#define HCI_CONTROL_STATUS_UNKNOWN_COMMAND                  9
#define HCI_CONTROL_STATUS_CLIENT_NOT_REGISTERED            10
#define HCI_CONTROL_STATUS_OUT_OF_MEMORY                    11
#define HCI_CONTROL_STATUS_DISALLOWED                       12

/* HS open status codes */
#define HCI_CONTROL_HF_STATUS_SUCCESS                       0   /* Connection successfully opened */
#define HCI_CONTROL_HF_STATUS_FAIL_SDP                      1   /* Open failed due to SDP */
#define HCI_CONTROL_HF_STATUS_FAIL_RFCOMM                   2   /* Open failed due to RFCOMM */
#define HCI_CONTROL_HF_STATUS_FAIL_CONN_TOUT                3   /* Link loss occured due to connection timeout */

/* BSG status codes */
#define HCI_CONTROL_BSG_STATUS_SUCCESS                      0
#define HCI_CONTROL_BSG_STATUS_NOT_CONNECTED                1
#define HCI_CONTROL_BSG_STATUS_OVERRUN                      2


#ifndef BD_ADDR_LEN
#define BD_ADDR_LEN 6
#endif

/* LE advertisement states */
#define LE_ADV_STATE_NO_DISCOVERABLE                        0
#define LE_ADV_STATE_HIGH_DISCOVERABLE                      1
#define LE_ADV_STATE_LOW_DISCOVERABLE                       2

/* HID Report Channel */
#define HCI_CONTROL_HID_REPORT_CHANNEL_CONTROL              0
#define HCI_CONTROL_HID_REPORT_CHANNEL_INTERRUPT            1

/* HID Report Type (matches BT HID Spec definitions) */
#define HCI_CONTROL_HID_REPORT_TYPE_OTHER                   0
#define HCI_CONTROL_HID_REPORT_TYPE_INPUT                   1
#define HCI_CONTROL_HID_REPORT_TYPE_OUTPUT                  2
#define HCI_CONTROL_HID_REPORT_TYPE_FEATURE                 3

#define HCI_CONTROL_HID_REPORT_ID                           1

/* Max TX packet to be sent over SPP */
#define HCI_CONTROL_SPP_MAX_TX_BUFFER                       700

/* Max GATT command packet size to be sent over UART */
#define HCI_CONTROL_GATT_COMMAND_MAX_TX_BUFFER              100

/* Gatt Operation Status */
#define HCI_CONTROL_GATT_STATUS_INSUF_AUTHENTICATION        5

/* Define Player Setting capabilities */
#define HCI_CONTROL_PLAYER_EQUALIZER_ENABLED

#define HCI_CONTROL_PLAYER_REPEAT_ENABLED
#define HCI_CONTROL_PLAYER_REPEAT_SINGLE_ENABLED
#define HCI_CONTROL_PLAYER_REPEAT_ALL_ENABLED
#define HCI_CONTROL_PLAYER_REPEAT_GROUP_ENABLED

#define HCI_CONTROL_PLAYER_SHUFFLE_ENABLED
#define HCI_CONTROL_PLAYER_SHUFFLE_ALL_ENABLED
#define HCI_CONTROL_PLAYER_SHUFFLE_GROUP_ENABLED

#define HCI_CONTROL_MESH_STATUS_SUCCESS                     0   /* Command executed successfully */
#define HCI_CONTROL_MESH_STATUS_ERROR                       1   /* Command start failed */
#define HCI_CONTROL_MESH_STATUS_CONNECT_FAILED              2

// HCI definitions for script application. 
//
#define HCI_CONTROL_GROUP_SCRIPT                              0x25
#define HCI_CONTROL_SCRIPT_COMMAND_EXECUTE                  ( ( HCI_CONTROL_GROUP_SCRIPT << 8 ) | 0x01 )    /* Execute a function */
#define HCI_CONTROL_SCRIPT_EVENT_RET_CODE                   ( ( HCI_CONTROL_GROUP_SCRIPT << 8 ) | 0x01 )   /* Script command return code */
#define HCI_CONTROL_SCRIPT_EVENT_UNKNOWN_CMD                ( ( HCI_CONTROL_GROUP_SCRIPT << 8 ) | 0x02 )   /* Unknown Script command */
#define HCI_CONTROL_SCRIPT_EVENT_CALLBACK                   ( ( HCI_CONTROL_GROUP_SCRIPT << 8 ) | 0x03 )   /* Async script callback */
#define HCI_CONTROL_SCRIPT_TRACE                            ( ( HCI_CONTROL_GROUP_SCRIPT << 8 ) | 0xF0 )   /* Trace message (low nibble has type) */

#endif /* HCI_CONTROL_API.H_ */

