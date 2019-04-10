/* Copyright Statement:
 *
 * (C) 2005-2017 MediaTek Inc. All rights reserved.
 *
 * This software/firmware and related documentation ("MediaTek Software") are
 * protected under relevant copyright laws. The information contained herein
 * is confidential and proprietary to MediaTek Inc. ("MediaTek") and/or its licensors.
 * Without the prior written permission of MediaTek and/or its licensors,
 * any reproduction, modification, use or disclosure of MediaTek Software,
 * and information contained herein, in whole or in part, shall be strictly prohibited.
 * You may only use, reproduce, modify, or distribute (as applicable) MediaTek Software
 * if you have agreed to and been bound by the applicable license agreement with
 * MediaTek ("License Agreement") and been granted explicit permission to do so within
 * the License Agreement ("Permitted User"). If you are not a Permitted User,
 * please cease any access or use of MediaTek Software immediately.
 * BY OPENING THIS FILE, RECEIVER HEREBY UNEQUIVOCALLY ACKNOWLEDGES AND AGREES
 * THAT MEDIATEK SOFTWARE RECEIVED FROM MEDIATEK AND/OR ITS REPRESENTATIVES
 * ARE PROVIDED TO RECEIVER ON AN "AS-IS" BASIS ONLY. MEDIATEK EXPRESSLY DISCLAIMS ANY AND ALL
 * WARRANTIES, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE OR NONINFRINGEMENT.
 * NEITHER DOES MEDIATEK PROVIDE ANY WARRANTY WHATSOEVER WITH RESPECT TO THE
 * SOFTWARE OF ANY THIRD PARTY WHICH MAY BE USED BY, INCORPORATED IN, OR
 * SUPPLIED WITH MEDIATEK SOFTWARE, AND RECEIVER AGREES TO LOOK ONLY TO SUCH
 * THIRD PARTY FOR ANY WARRANTY CLAIM RELATING THERETO. RECEIVER EXPRESSLY ACKNOWLEDGES
 * THAT IT IS RECEIVER'S SOLE RESPONSIBILITY TO OBTAIN FROM ANY THIRD PARTY ALL PROPER LICENSES
 * CONTAINED IN MEDIATEK SOFTWARE. MEDIATEK SHALL ALSO NOT BE RESPONSIBLE FOR ANY MEDIATEK
 * SOFTWARE RELEASES MADE TO RECEIVER'S SPECIFICATION OR TO CONFORM TO A PARTICULAR
 * STANDARD OR OPEN FORUM. RECEIVER'S SOLE AND EXCLUSIVE REMEDY AND MEDIATEK'S ENTIRE AND
 * CUMULATIVE LIABILITY WITH RESPECT TO MEDIATEK SOFTWARE RELEASED HEREUNDER WILL BE,
 * AT MEDIATEK'S OPTION, TO REVISE OR REPLACE MEDIATEK SOFTWARE AT ISSUE,
 * OR REFUND ANY SOFTWARE LICENSE FEES OR SERVICE CHARGE PAID BY RECEIVER TO
 * MEDIATEK FOR SUCH MEDIATEK SOFTWARE AT ISSUE.
 */

/**
 * @file wifi_api.h
 *
 * Wi-Fi processor configuration / settings for the STA operations.
 *
 */

/**@addtogroup WIFI
* @{
* This section introduces the Wi-Fi driver APIs including terms and acronyms, supported features,
* software architecture, details on how to use this driver, Wi-Fi function groups, enumerations,
* structures and functions. The Wi-Fi APIs are divided into three categories:
* Profile, Configuration and Connection. And there are four types of operation modes supported in Wi-Fi module:
* #WIFI_MODE_STA_ONLY, #WIFI_MODE_AP_ONLY, #WIFI_MODE_REPEATER and #WIFI_MODE_MONITOR.
*
* Profile APIs get or set the profile in the Flash memory. The profile settings can be read by
* Wi-Fi profile get APIs. The profile settings can be used to initialize the Wi-Fi
* driver configuration at system boot up. There are many profile sections in the Flash memory, such as:
* STA, AP, common. Use the same profile section for #WIFI_PORT_STA/#WIFI_PORT_APCLI.
*
* Configuration APIs get and set the Wi-Fi driver configuration. In most cases they take effect
* immediately, but there are some configuration APIs that only take
* effect by calling the #wifi_config_reload_setting() API, such as: \n
* #wifi_config_set_pmk() \n
* #wifi_config_set_security_mode() \n
* #wifi_config_set_ssid() \n
* #wifi_config_set_wep_key() \n
* #wifi_config_set_wpa_psk_key() \n
* The Configuration APIs include wifi_config_xxx APIs and wifi_wps_config_xxx APIs. They use in-band mechanism (in-band is a mechanism used for Wi-Fi driver and Wi-Fi firmware communication),
* thus these APIs must be called after OS task scheduler is started to make sure an in-band task is running.
* The wifi_wps_config_xxx APIs can be used to set or get default device configuration before triggering the Push Button Connection (PBC) or Personal Identification Number (PIN) process.
* The function #wifi_config_register_rx_handler() registers wifi_rx_handler to manage the raw packet. There is a limitation calling this handler as it's using an in-band mechanism,
* it's restricted to call any in-band function similar to Wi-Fi Configuration or Wi-Fi Connection APIs in wifi_rx_handler.
*
* The Connection APIs include wifi_connection_xxx APIs and wifi_wps_connection_xxx APIs. They manage the link status, such as disconnect AP, disconnect the station, get the link status, get the station list, start or stop the scan.
* Register an event handler for scan, connect or disconnect event. The Connection APIs use an in-band mechanism, thus these APIs must be called after the OS Task
* scheduler is started, to make sure an in-band task is running.
* The wifi_wps_connection_xxx APIs can be used to trigger the PBC or PIN process.
*
*
* Terms                         |Details                                                                 |
* ------------------------------|------------------------------------------------------------------------|
* \b STA                        | In station (STA) mode the device operates as a client that connects to a Wi-Fi access point.|
* \b AP                         | In Access Point (AP) mode other devices can connect to the Wi-Fi access point.|
* \b APCLI                      | In AP Client (APCLI) mode the Access Point becomes a wireless client for another AP.|
* \b DTIM                       | Delivery Traffic Indication Message (DTIM) is how the AP (wireless router) warns its clients that it is about to transmit the multicast (and broadcast*) frames it queued up since the previous DTIM. For more information, please refer to <a href="https://en.wikipedia.org/wiki/Delivery_traffic_indication_message">introduction to DTIM in Wikipedia</a>. |
* \b MCS                        | Modulation and Coding Set (MCS) implies the packet data rate. For more information, please refer to <a href="https://en.wikipedia.org/wiki/Link_adaptation">introduction to MCS in Wikipedia</a>.|
* \b WPS                        | Wi-Fi Protected Setup (WPS) is a network security standard to create a secure wireless home network. For more information, please refer to <a href="https://en.wikipedia.org/wiki/Wi-Fi_Protected_Setup">introduction to WPS in Wikipedia</a>.|
* \b PIN                        | Personal identification number (PIN) is a method of WPS, has to be read from either a sticker or display on the new wireless device. This PIN must then be entered at the "representant" of the network, usually the network's AP. Alternately, a PIN provided by the AP may be entered into the new device. This method is the mandatory baseline mode and everything must support it. The Wi-Fi direct specification supersedes this requirement by stating that all devices with a keypad or display must support the PIN method.|
* \b PBC                        | Push button (PBC) is a method of WPS. For more information, please refer to <a herf="https://en.wikipedia.org/wiki/Wi-Fi_Protected_Setup">introduction to PBC in Wikipedia</a>.|
* \b STBC                       | Space-time Block Code (STBC) used in wireless telecommunications. Space-time block coding is a technique used in wireless communications to transmit multiple copies of a data stream across a number of antennas and to exploit the various received versions of the data to improve the reliability of data-transfer. For more information, please refer to <a href="https://en.wikipedia.org/wiki/Space%E2%80%93time_block_code">introduction to STBC in Wikipedia</a>.|
* \b FCS                        | Frame Check Sequence (FCS) refers to the extra error-detecting code added to a frame in a communications protocol. For more information, please refer to <a href="https://en.wikipedia.org/wiki/Frame_check_sequence">introduction to FCS in Wikipedia</a>.|
* \b Broadcast                  | A communication where a piece of information is sent from one point to all other points.|
* \b Multicast                  | A communication where a piece of information is sent from one or more points to a set of other points.|
* \b RTS                        | Request to Send (RTS) is IEEE 802.11 control signal is an optional mechanism used by the 802.11 wireless networking protocol to reduce frame collisions. For more information, please refer to <a href="https://en.wikipedia.org/wiki/RTS/CTS">introduction to RTS in Wikipedia</a>.|
* \b CTS                        | Clear to Send (CTS) is IEEE 802.11 control signal is an optional mechanism used by the 802.11 wireless networking protocols to reduce frame collisions. For more information, please refer to <a href="https://en.wikipedia.org/wiki/RTS/CTS">introduction to CTS in Wikipedia</a>.|
* \b RSSI                       | Received Signal Strength Indication (RSSI). For more information, please refer to <a href="https://en.wikipedia.org/wiki/Received_signal_strength_indication">introduction to RSSI in Wikipedia</a>. |
* \b Beacon                     | Beacon frame is one of the management frames in IEEE 802.11 based WLANs.|
* \b WPA                        | Wi-Fi Protected Access (WPA) is a wireless encryption standard. For more information, please refer to <a href="https://en.wikipedia.org/wiki/Wi-Fi_Protected_Access">introduction to WPA in Wikipedia</a>.|
* \b WPS                        | Wi-Fi Protected Setup (WPS) is a network security standard. For more information, please refer to <a href="https://en.wikipedia.org/wiki/Wi-Fi_Protected_Setup">introduction to WPS in Wikipedia</a>.|
* \b OPMODE                     | Operate Mode (OPMODE) is STA/AP/APCLI.|
* \b PSK                        | Pre-shared key (PSK) is a method to set encryption keys. For more information, please refer to <a href="https://en.wikipedia.org/wiki/Pre-shared_key">introduction to PSK in Wikipedia</a>.|
* \b PMK                        | Pair-wise Master Key (PMK) is calculated from PSK.|
* \b WEP                        | Wired Equivalent Privacy (WEP) is a wireless network security standard. For more information, please refer to <a href="https://en.wikipedia.org/wiki/Wired_Equivalent_Privacy">introduction to WEP in Wikipedia</a>.|
* \b WOW                        | Wake on Wireless (WOW) is a technology that allows remote wake-up of workstations from a standby power state to facilitate device management. |
* \b TKIP                       | Temporal Key Integrity Protocol (TKIP) is an algorithm used to secure wireless computer networks. For more information, please refer to <a href="https://en.wikipedia.org/wiki/Temporal_Key_Integrity_Protocol">introduction to TKIP in Wikipedia</a>.|
* \b AES                        | Advanced Encryption Standard process (AES) is a 'symmetric block cipher' for encrypting texts which can be decrypted with the original encryption key. For more information, please refer to <a href="https://en.wikipedia.org/wiki/Advanced_Encryption_Standard">introduction to AES in Wikipedia</a>.|
* \b PHY                        | PHY of the Open system interconnection (OSI) model and refers to the circuitry required to implement physical layer functions. For more information, please refer to <a href="https://en.wikipedia.org/wiki/PHY_(chip)">introduction to PHY in Wikipedia</a>.|
* \b BSS                        | The basic service set (BSS) provides the basic building-block of an 802.11 wireless LAN. For more information, please refer to <a href="https://en.wikipedia.org/wiki/Service_set_%28802.11_network%29">introduction to 802.11 network in Wikipedia</a>.|
* \b IBSS                       | With 802.11, one can set up an ad hoc network of client devices without a controlling access point; the result is called an IBSS (independent BSS). For more information, please refer to <a href="https://en.wikipedia.org/wiki/Service_set_%28802.11_network%29">introduction to 802.11 network in Wikipedia</a>.|
* \b MCS                        | Modulation and Coding Scheme (MCS) is related to the maximum data rate and robustness of data transmission. For more information, please refer to <a href="https://en.wikipedia.org/wiki/Link_adaptation">introduction to Link adaptation in Wikipedia</a>.|
* \b LDPC                       | In information theory, a low-density parity-check (LDPC) code is a linear error correcting code, a method of transmitting a message over a noisy transmission channel. For more information, please refer to <a href="https://en.wikipedia.org/wiki/Low-density_parity-check_code">introduction to Low-density parity-check code in Wikipedia</a>.|
* \b GI                         | In telecommunications, guard intervals (GI) are used to ensure that distinct transmissions do not interfere with one another. For more information, please refer to <a href="https://en.wikipedia.org/wiki/Guard_interval">introduction to Guard interval in Wikipedia</a>.|
* \b MPDU                       | MPDU stands for MAC protocol data unit.|
* \b TSC                        | TKIP uses a per-MPDU TKIP sequence counter (TSC) to sequence the MPDUs it sends. The receiver drops MPDUs received out of order, i.e., not received with increasing sequence numbers.|
*
*/

#ifndef __WIFI_API_H__
#define __WIFI_API_H__

#include "stdint.h"
#include "stdbool.h"


#ifdef __cplusplus
extern "C" {
#endif

/**@defgroup WIFI_DEFINE Define
* @{
*/

/**@brief The maximum length of SSID.
*/
#define WIFI_MAX_LENGTH_OF_SSID             (32)

/**@brief MAC address length.
*/
#define WIFI_MAC_ADDRESS_LENGTH             (6)

/**
* @brief Length of 802.11 MAC header.
*/
#define WIFI_LENGTH_802_11                  (24)

/**@brief Timestamp length in a frame header.
*/
#define WIFI_TIMESTAMP_LENGTH               (8)

/**@brief Beacon interval length in a frame header.
*/
#define WIFI_BEACON_INTERVAL_LENGTH         (2)

/**@brief Length of capability information in a frame header.
*/
#define WIFI_CAPABILITY_INFO_LENGTH         (2)

/**@brief The Maximum number of stations that can
* connect to LinkIt 7687 HDK in SoftAP mode.
*/
#define WIFI_MAX_NUMBER_OF_STA              (16)

/**@brief The maximum length of passphrase used in WPA-PSK and WPA2-PSK encryption types.
*/
#define WIFI_LENGTH_PASSPHRASE              (64)

/**@brief The maximum length of PMK used in WPA-PSK and WPA2-PSK encryption types.
*/
#define WIFI_LENGTH_PMK                     (32)

/**@brief The AP buffer in a profile.
*/
#define WIFI_PROFILE_BUFFER_AP              "AP"

/**@brief The STA buffer in a profile.
*/
#define WIFI_PROFILE_BUFFER_STA             "STA"

/**@brief The common buffer in a profile.
*/
#define WIFI_PROFILE_BUFFER_COMMON          "common"

/**@brief The maximum length of each entity stored
* in the Flash memory. Allocate a buffer of this length so that the Flash memory
* entity won't overflow.
*/
#define WIFI_PROFILE_BUFFER_LENGTH          (128)

/**@brief Specifies 2.4GHz band.
*/
#define WIFI_BAND_2_4_G                     (0)

/**@brief Specifies 5GHz band.
*/
#define WIFI_BAND_5_G                       (1)


/**@brief The event ID to indicate the link is disconnected.
*/
#define WIFI_STATUS_LINK_DISCONNECTED       (0)

/**@brief The event ID to indicate the link is connected.
*/
#define WIFI_STATUS_LINK_CONNECTED          (1)

/**@brief Specifies 20MHz bandwidth in the 2.4GHz band.
*/
#define WIFI_IOT_COMMAND_CONFIG_BANDWIDTH_20MHZ       (0x00)

/**@brief Specifies 40MHz bandwidth in the 2.4GHz band.
*/
#define WIFI_IOT_COMMAND_CONFIG_BANDWIDTH_40MHZ       (0x01)

/**@brief Specifies 20MHz | 40MHz bandwidth in the 2.4GHz band.
*/
#define WIFI_IOT_COMMAND_CONFIG_BANDWIDTH_2040MHZ     (WIFI_IOT_COMMAND_CONFIG_BANDWIDTH_20MHZ|WIFI_IOT_COMMAND_CONFIG_BANDWIDTH_40MHZ)


/**
* @brief Station operation mode. In this mode the device works as a Wi-Fi client.
*/
#define WIFI_MODE_STA_ONLY      (1)

/**
* @brief SoftAP operation mode. In AP mode, other client devices can connect to the Wi-Fi AP.
*/
#define WIFI_MODE_AP_ONLY       (2)

/**
* @brief Repeater mode. There are two virtual ports in repeater mode, one is #WIFI_PORT_AP, and the other is #WIFI_PORT_APCLI.
* Both ports should be configured to operate on the same channel with the same bandwidth, while their other settings can be different. For example, both ports can have different MAC addresses and security modes.
*/
#define WIFI_MODE_REPEATER          (3)

/**
* @brief This macro defines the monitoring mode. In this mode it can sniffer
* the Wi-Fi packet in the air given the specific channel and bandwidth.
* It is used to enter a hybrid mode and handle a raw packet.
* Call #wifi_config_register_rx_handler() to register a raw packet handler
* once this mode is set.
*/
#define WIFI_MODE_MONITOR       (4)

/**
* @brief Supports Wi-Fi Direct feature.
*/
#define WIFI_MODE_P2P_ONLY          (5)
/**
* @brief Specifies the STA operation.
*/
#define WIFI_PORT_STA           (0)

/**
* @brief Specifies the AP client operation.
*/
#define WIFI_PORT_APCLI         WIFI_PORT_STA

/**
* @brief Specifies the AP operation.
*/
#define WIFI_PORT_AP            (1)

#define WIFI_NUMBER_WEP_KEYS     (4)    /**<  The group number of WEP keys. */
#define WIFI_MAX_WEP_KEY_LENGTH  (26)   /**<  The maximum length of each WEP key. */

#define WIFI_INIT_TIME_OUT      1000    /**<  Initial Wi-Fi timeout is 1000ms. */

/**
* @brief The length of device information used for WPS.
*/
#define WIFI_WPS_MAX_LENGTH_DEVICE_NAME     (32)    /**< The maximum length of device name. */
#define WIFI_WPS_MAX_LENGTH_MANUFACTURER    (64)    /**< The maximum length of manufacturer's name. */
#define WIFI_WPS_MAX_LENGTH_MODEL_NAME      (32)    /**< The maximum length of module name. */
#define WIFI_WPS_MAX_LENGTH_MODEL_NUMBER    (32)    /**< The maximum length of module number. */
#define WIFI_WPS_MAX_LENGTH_SERIAL_NUMBER   (32)    /**< The maximum length of serial number. */

/**
 * @brief The definition of the error code for the wifi public functions
 */
#define WIFI_SUCC  (0)
#define WIFI_FAIL  (-1)
#define WIFI_ERR_PARA_INVALID (-2)
#define WIFI_ERR_NOT_SUPPORT  (-3)

/**
* @brief definitation of 2G max channel number.
*/
#define WIFI_2G_CHANNEL_NUMBER            (14)    /**< The 2G total channel number. */

/**
* @brief definitation of single SKU number of each channel for 2G channel.
*/
#define WIFI_2G_SKU_POWER_PER_CHANNEL     (19)    /**< The single  SKU number for each channel */


/**
* @}
*/


/**@defgroup WIFI_ENUM Enumeration
* @{
*/
/**@brief This enumeration defines the supported events generated by the Wi-Fi driver. The event will be sent to the upper layer handler registered in #wifi_connection_register_event_handler().
*/
typedef enum {
    WIFI_EVENT_IOT_CONNECTED = 0,               /**< Connected Event.When authentication and association complete, then will receive this event. 
                                                                        Payload: mac address[6 bytes] + port[1 byte], the payload length is 7. 
                                                                        \n MAC address:in station mode the MAC address of remote AP which the device conencted, in AP mode, the MAC address is the client which connect to AP. 
                                                                        \n Port: indicate which port the event comes from.0: STA port; 1: AP port.*/
    WIFI_EVENT_IOT_SCAN_COMPLETE,               /**< Scan completed event.Triggered when the scan process is complete. 
                                                                        Payload: NULL.*/
    WIFI_EVENT_IOT_DISCONNECTED,                /**< Disconnected event.It means the device turn to disconnect status from connect status when receive this event.
                                                                        Payload: mac address[6 bytes] + port[1 byte], the payload length is 7. 
                                                                        \n MAC address: in station mode, the MAC address of remote AP which the device disconencted.In the AP mode, the address is the client which left. 
                                                                        \n Port: indicate which port the event comes from.0: STA port; 1: AP port.*/
    WIFI_EVENT_IOT_PORT_SECURE,                 /**< Port secure event, mainly processed in supplicant.In open mode, this event is triggered when authentication and association complete; In WPA-PSK/WPA2-PSK mode, this event is triggered when 4 way handshake(group handshake) complete. 
                                                                        Payload: mac address[6 bytes] + port[1 byte], the payload length is 7.
                                                                        \n MAC address: the MAC address of remote AP which the device conencted.
                                                                        \n Port: indicate which port the event comes from.0: STA port; 1: AP port*/
    WIFI_EVENT_IOT_REPORT_BEACON_PROBE_RESPONSE,/**< Sends notification, if beacon or probe request is received in scan progress.It will receive this event with the payload of beacon/probe response. 
                                                                        Payload: the beacon/probe response RAW data, the length is based on the beacon/probe response frame. 
                                                                        \n The payload can be parsed by the function #wifi_connection_parse_beacon(), please refer to the API reference for details.*/
    WIFI_EVENT_IOT_WPS_COMPLETE,                /**< Credential event, It is triggered in Enrollee when it successfully parsed WPS credentials in M8 message. 
                                                                        Payload: Represents the credentials used for WPS, the payload format follows wifi_wps_credential_info_t structure. 
                                                                        \n Detail information please refer to the structure of #wifi_wps_credential_info_t in the Wi-Fi API Reference Manual.*/
    WIFI_EVENT_IOT_INIT_COMPLETE,               /**< Initialization complete event for Wi-Fi module.It is triggered when supplicant intial complete. 
                                                                        Payload: zero data[6 bytes] + port[1 byte]
                                                                        \n Port: indicate which port the event comes from.0: STA port; 1: AP port.*/
    WIFI_EVENT_IOT_REPORT_FILTERED_FRAME,       /**< It is triggered after invoke #wifi_config_set_rx_filter().Then report the expected management packet frames, beside, your can choose several type of frame in the same time.Please refer to #wifi_config_set_rx_filter() in  the Wi-Fi API Reference Manual. 
                                                                        Payload: expected management packet frame.*/
    WIFI_EVENT_IOT_CONNECTION_FAILED,          /**< Connection has failed.For the reason code, please refer to #wifi_reason_code_t. 
                                                                        Payload: port[1 byte] + reason code[2 bytes]. 
                                                                        \n Port:  indicate which port the event comes from.0: STA port; 1: AP port.*/
    WIFI_EVENT_MAX_NUMBER
} wifi_event_t;

/** @brief This enumeration defines the reason code of the WIFI_EVENT_IOT_CONNECTION_FAILED event in #wifi_event_t.
* When receives WIFI_EVENT_IOT_CONNECTION_FAILED handler, the payload format for the WIFI_EVENT_IOT_CONNECTION_FAILED handler is port(1 byte) and reason code(2 bytes). 
* Find the details for the reason code below.
*/
typedef enum {
    WIFI_REASON_CODE_SUCCESS,                             /**<  0   Reserved. */
    WIFI_REASON_CODE_FIND_AP_FAIL,                        /**<  1   (Internal) No AP found. */
    WIFI_REASON_CODE_PREV_AUTH_INVALID,                   /**<  2   Previous authentication is no longer valid. */
    WIFI_REASON_CODE_DEAUTH_LEAVING_BSS,                  /**<  3   Deauthenticated because sending STA is leaving (or has left) IBSS or ES. */
    WIFI_REASON_CODE_DISASSOC_INACTIVITY,                 /**<  4   Disassociated due to inactivity. */
    WIFI_REASON_CODE_DISASSOC_AP_OVERLOAD,                /**<  5   Disassociated because AP is unable to handle all currently associated STAs. */
    WIFI_REASON_CODE_CLASS_2_ERR,                         /**<  6   Class 2 frame received from nonauthenticated STA. */
    WIFI_REASON_CODE_CLASS_3_ERR,                         /**<  7   Class 3 frame received from nonauthenticated STA. */
    WIFI_REASON_CODE_DISASSOC_LEAVING_BSS,                /**<  8   Disassociated because sending STA is leaving (or has left) BSS. */
    WIFI_REASON_CODE_ASSOC_BEFORE_AUTH,                   /**<  9   STA requesting (re)association is not authenticated with responding STA. */
    WIFI_REASON_CODE_DISASSOC_PWR_CAP_UNACCEPTABLE,       /**<  10  Disassociated because the information in the Power Capability element is unacceptable. */
    WIFI_REASON_CODE_DISASSOC_SUP_CHS_UNACCEPTABLE,       /**<  11  Disassociated because the information in the Supported Channels element is unacceptable. */
    WIFI_REASON_CODE_INVALID_INFO_ELEM = 13,              /**<  13  Invalid information element. */
    WIFI_REASON_CODE_MIC_FAILURE,                         /**<  14  Message integrity code (MIC) failure. */
    WIFI_REASON_CODE_4_WAY_HANDSHAKE_TIMEOUT,             /**<  15  4-Way Handshake time out. */
    WIFI_REASON_CODE_GROUP_KEY_UPDATE_TIMEOUT,            /**<  16  Group Key Handshake time out. */
    WIFI_REASON_CODE_DIFFERENT_INFO_ELEM,                 /**<  17  Information element in 4-Way Handshake different from (Re)Association Request/Probe Response/Beacon frame. */
    WIFI_REASON_CODE_MULTICAST_CIPHER_NOT_VALID,          /**<  18  Invalid group cipher. */
    WIFI_REASON_CODE_UNICAST_CIPHER_NOT_VALID,            /**<  19  Invalid pairwise cipher. */
    WIFI_REASON_CODE_AKMP_NOT_VALID,                      /**<  20  Invalid AKMP. */
    WIFI_REASON_CODE_UNSUPPORTED_RSNE_VERSION,            /**<  21  Unsupported RSN information element version. */
    WIFI_REASON_CODE_INVALID_RSNE_CAPABILITIES,           /**<  22  Invalid RSN information element capabilities. */
    WIFI_REASON_CODE_IEEE_802_1X_AUTH_FAILED,             /**<  23  IEEE 802.1X authentication failed. */
    WIFI_REASON_CODE_CIPHER_REJECT_SEC_POLICY,            /**<  24  Cipher suite rejected because of the security policy. */
    WIFI_REASON_CODE_DONT_SUPPORT_HT_OPERATION,           /**<  25  (Internal) Association denied due to requesting STA not supporting the Short Slot Time option. */
    WIFI_REASON_CODE_DONT_SUPPORT_DSS_OFDM,               /**<  26  (Internal) Association denied due to requesting STA not supporting the DSSS-OFDM option. */
    WIFI_REASON_CODE_DONT_SUPPORT_HT,                     /**<  27  (Internal) HT not supported. */
    WIFI_REASON_CODE_DISASSOC_UNSPECIFIED_QOS = 32,       /**<  32  Disassociated for unspecified, QoS-related reason. */
    WIFI_REASON_CODE_DISASSOC_LACK_OF_BANDWIDTH,          /**<  33  Disassociated because QoS AP lacks sufficient bandwidth for this QoS STA. */
    WIFI_REASON_CODE_DISASSOC_ACK_LOST_POOR_CHANNEL,      /**<  34  Disassociated because excessive number of frames need to be acknowledged, but are not acknowledged due to AP transmissions and/or poor channel conditions. */
    WIFI_REASON_CODE_DISASSOC_TX_OUTSIDE_TXOP_LIMIT,      /**<  35  Disassociated because STA is transmitting outside the limits of its TXOPs. */
    WIFI_REASON_CODE_PEER_WHILE_LEAVING,                  /**<  36  Requested from peer STA as the STA is leaving the BSS (or resetting). */
    WIFI_REASON_CODE_PEER_REFUSE_DLP,                     /**<  37  Requested from peer STA as it does not want to use the mechanism. */
    WIFI_REASON_CODE_PEER_SETUP_REQUIRED,                 /**<  38  Requested from peer STA as the STA received frames using the mechanism for which a setup is required. */
    WIFI_REASON_CODE_PEER_TIME_OUT,                       /**<  39  Requested from peer STA due to time out. */
    WIFI_REASON_CODE_INVALID_INFORMATION_ELEMENT,         /**<  40  (Internal) Invalid information element. */
    WIFI_REASON_CODE_INVALID_GROUP_CIPHER,                /**<  41  (Internal) Invalid group cipher. */
    WIFI_REASON_CODE_INVALID_PAIRWISE_CIPHER,             /**<  42  (Internal) Invalid pairwise cipher. */
    WIFI_REASON_CODE_INVALID_AKMP,                        /**<  43  (Internal) Invalid AKMP. */
    WIFI_REASON_CODE_DONT_SUPPORT_RSN_VERSION,            /**<  44  (Internal) Unsupported RSN information element version.*/
    WIFI_REASON_CODE_PEER_CIPHER_UNSUPPORTED,             /**<  45  Peer STA does not support the requested cipher suite. */
    WIFI_REASON_CODE_PASSWORD_ERROR = 100                 /**<  100 (Internal) Connection has failed, wrong password. */
} wifi_reason_code_t;

/** @brief This enumeration defines the RX filter register's bitmap. Each bit indicates a specific drop frame.
*/
typedef enum {
    WIFI_RX_FILTER_DROP_STBC_BCN_BC_MC,       /**< bit 0   Drops the STBC beacon/BC/MC frames. */
    WIFI_RX_FILTER_DROP_FCS_ERR,              /**< bit 1   Drops the FCS error frames. */
    WIFI_RX_FILTER_RESERVED,                  /**< bit 2   A reserved bit, not used. */
    WIFI_RX_FILTER_DROP_VER_NOT_0,            /**< bit 3   Drops the version field of Frame Control field. It cannot be 0. */
    WIFI_RX_FILTER_DROP_PROBE_REQ,            /**< bit 4   Drops the probe request frame. */
    WIFI_RX_FILTER_DROP_MC_FRAME,             /**< bit 5   Drops multicast frame. */
    WIFI_RX_FILTER_DROP_BC_FRAME,             /**< bit 6   Drops broadcast frame. */
    WIFI_RX_FILTER_RM_FRAME_REPORT_EN = 12,   /**< bit 12  Enables report frames. */
    WIFI_RX_FILTER_DROP_CTRL_RSV,             /**< bit 13  Drops reserved definition control frames. */
    WIFI_RX_FILTER_DROP_CTS,                  /**< bit 14  Drops CTS frames. */
    WIFI_RX_FILTER_DROP_RTS,                  /**< bit 15  Drops RTS frames. */
    WIFI_RX_FILTER_DROP_DUPLICATE,            /**< bit 16  Drops duplicate frames. */
    WIFI_RX_FILTER_DROP_NOT_MY_BSSID,         /**< bit 17  Drops not my BSSID frames. */
    WIFI_RX_FILTER_DROP_NOT_UC2ME,            /**< bit 18  Drops not unicast to me frames. */
    WIFI_RX_FILTER_DROP_DIFF_BSSID_BTIM,      /**< bit 19  Drops different BSSID TIM (Traffic Indication Map) Broadcast frame. */
    WIFI_RX_FILTER_DROP_NDPA                  /**< bit 20  Drops the NDPA or not. */
} wifi_rx_filter_t;

/** @brief This enumeration defines the wireless authentication mode to indicate the Wi-Fi device’s authentication attribute.
*/
typedef enum {
    WIFI_AUTH_MODE_OPEN = 0,                        /**< Open mode.     */
    WIFI_AUTH_MODE_SHARED,                          /**< Not supported. */
    WIFI_AUTH_MODE_AUTO_WEP,                        /**< Not supported. */
    WIFI_AUTH_MODE_WPA,                             /**< Not supported. */
    WIFI_AUTH_MODE_WPA_PSK,                         /**< WPA_PSK.       */
    WIFI_AUTH_MODE_WPA_None,                        /**< Not supported. */
    WIFI_AUTH_MODE_WPA2,                            /**< Not supported. */
    WIFI_AUTH_MODE_WPA2_PSK,                        /**< WPA2_PSK.      */
    WIFI_AUTH_MODE_WPA_WPA2,                        /**< Not supported. */
    WIFI_AUTH_MODE_WPA_PSK_WPA2_PSK,                /**< Mixture mode.  */
} wifi_auth_mode_t;

/** @brief This enumeration defines the wireless encryption type to indicate the Wi-Fi device’s encryption attribute.
*/
typedef enum {
    WIFI_ENCRYPT_TYPE_WEP_ENABLED = 0,                                              /**< WEP encryption type.  */
    WIFI_ENCRYPT_TYPE_ENCRYPT1_ENABLED = WIFI_ENCRYPT_TYPE_WEP_ENABLED,             /**< WEP encryption type.  */
    WIFI_ENCRYPT_TYPE_WEP_DISABLED = 1,                                             /**< No encryption.     */
    WIFI_ENCRYPT_TYPE_ENCRYPT_DISABLED = WIFI_ENCRYPT_TYPE_WEP_DISABLED,            /**< No encryption.     */
    WIFI_ENCRYPT_TYPE_WEP_KEY_ABSENT = 2,                                           /**< Not supported.     */
    WIFI_ENCRYPT_TYPE_ENCRYPT_KEY_ABSENT = WIFI_ENCRYPT_TYPE_WEP_KEY_ABSENT,        /**< Not supported.     */
    WIFI_ENCRYPT_TYPE_WEP_NOT_SUPPORTED = 3,                                        /**< Not supported.     */
    WIFI_ENCRYPT_TYPE_ENCRYPT_NOT_SUPPORTED = WIFI_ENCRYPT_TYPE_WEP_NOT_SUPPORTED,  /**< Not supported.     */
    WIFI_ENCRYPT_TYPE_TKIP_ENABLED = 4,                                             /**< TKIP encryption.   */
    WIFI_ENCRYPT_TYPE_ENCRYPT2_ENABLED = WIFI_ENCRYPT_TYPE_TKIP_ENABLED,            /**< TKIP encryption.   */
    WIFI_ENCRYPT_TYPE_AES_ENABLED = 6,                                              /**< AES encryption.    */
    WIFI_ENCRYPT_TYPE_ENCRYPT3_ENABLED = WIFI_ENCRYPT_TYPE_AES_ENABLED,             /**< AES encryption.     */
    WIFI_ENCRYPT_TYPE_AES_KEY_ABSENT = 7,                                           /**< Not supported.     */
    WIFI_ENCRYPT_TYPE_TKIP_AES_MIX = 8,                                             /**< TKIP or AES mix.   */
    WIFI_ENCRYPT_TYPE_ENCRYPT4_ENABLED = WIFI_ENCRYPT_TYPE_TKIP_AES_MIX,            /**< TKIP or AES mix.   */
    WIFI_ENCRYPT_TYPE_TKIP_AES_KEY_ABSENT = 9,                                      /**< Not supported.     */
    WIFI_ENCRYPT_TYPE_GROUP_WEP40_ENABLED = 10,                                     /**< Not supported.     */
    WIFI_ENCRYPT_TYPE_GROUP_WEP104_ENABLED = 11,                                    /**< Not supported.     */
#ifdef WAPI_SUPPORT
    WIFI_ENCRYPT_TYPE_ENCRYPT_SMS4_ENABLED,                                         /**< WPI SMS4 support.  */
#endif /* WAPI_SUPPORT */
} wifi_encrypt_type_t;


/** @brief This enumeration defines the wireless physical mode.
*/
typedef enum {
    WIFI_PHY_11BG_MIXED = 0,     /**<  0, 2.4GHz band. */
    WIFI_PHY_11B,                /**<  1, 2.4GHz band. */
    WIFI_PHY_11A,                /**<  2, 5GHz band. */
    WIFI_PHY_11ABG_MIXED,        /**<  3, both 2.4G and 5G band. */
    WIFI_PHY_11G,                /**<  4, 2.4GHz band. */
    WIFI_PHY_11ABGN_MIXED,       /**<  5, both 2.4G and 5G band. */
    WIFI_PHY_11N_2_4G,           /**<  6, 11n-only with 2.4GHz band. */
    WIFI_PHY_11GN_MIXED,         /**<  7, 2.4GHz band. */
    WIFI_PHY_11AN_MIXED,         /**<  8, 5GHz band. */
    WIFI_PHY_11BGN_MIXED,        /**<  9, 2.4GHz band. */
    WIFI_PHY_11AGN_MIXED,        /**< 10, both 2.4G and 5G band. */
    WIFI_PHY_11N_5G,             /**< 11, 11n-only with 5GHz band. */
} wifi_phy_mode_t;

/** @brief This enumeration defines wireless security cipher suits.
*/
typedef enum {
        WIFI_CIPHER_NONE = 0,     /**<  0, No cipher. */
        WIFI_CIPHER_WEP64,        /**<  1, WEP64 cipher.  */
        WIFI_CIPHER_WEP128,       /**<  2, WEP128 cipher. */
        WIFI_CIPHER_TKIP,         /**<  3, TKIP cipher. */
        WIFI_CIPHER_AES,          /**<  4, AES cipher. */
        WIFI_CIPHER_CKIP64,       /**<  5, CKIP64 cipher. */
        WIFI_CIPHER_CKIP128,      /**<  6, CKIP128 cipher. */
        WIFI_CIPHER_CKIP152,      /**<  7, CKIP152 cipher. */
        WIFI_CIPHER_SMS4,         /**<  8, SMS4 cipher. */
        WIFI_CIPHER_WEP152,       /**<  9, WEP152 cipher. */
        WIFI_CIPHER_BIP,          /**< 10, BIP cipher. */
        WIFI_CIPHER_WAPI,         /**< 11, WAPI cipher. */
        WIFI_CIPHER_TKIP_NO_MIC   /**< 12, TKIP_NO_MIC cipher. */
} wifi_security_cipher_suits_t;


/** @brief This enumeration defines 40MHz bandwidth extension.
*/
typedef enum {
    WIFI_BANDWIDTH_EXT_40MHZ_UP,    /**< 40MHz up. */
    WIFI_BANDWIDTH_EXT_40MHZ_BELOW  /**< 40MHz below. */
} wifi_bandwidth_ext_t;

/**
* @brief This enumeration defines three power saving modes.
*/
typedef enum {
    WIFI_POWER_SAVING_MODE_OFF = 0,          /**<  WIFI_POWER_SAVING_MODE_OFF is a power saving mode that keeps the radio powered up continuously to ensure there is a minimal lag in response time. This power saving setting consumes the most power but offers the highest throughput. */
    WIFI_POWER_SAVING_MODE_LEGACY,           /**<  WIFI_POWER_SAVING_MODE_LEGACY: The access point buffers incoming messages for the radio. The radio occasionally 'wakes up' to determine if any buffered messages are waiting and then returns to sleep mode after it has responded to each message. This setting conserves the most power but also provides the lowest throughput. It is recommended for radios in which power consumption is the most important (such as small battery-operated devices). */
    WIFI_POWER_SAVING_MODE_FAST              /**<  WIFI_POWER_SAVING_MODE_FAST is a power saving mode that switches between power saving and WIFI_POWER_SAVING_MODE_OFF modes, depending on the network traffic. For example, it switches to WIFI_POWER_SAVING_MODE_OFF mode after receiving a large number of packets and switches back to power saving mode after the packets have been retrieved. Fast is recommended when power consumption and throughput are a concern.  */
} wifi_power_saving_mode_t;

/** @brief This enumeration of device password ID is used for WPS.
* This enumeration is only applicable when the boolean variable selected_registrar inside #wifi_wps_element_t is true.
* For more information, please refer to Table 37, "Device Password ID" in chapter 12, "Data Element Definitions"
* of the Wi-Fi Simple Configuration Technical Specification V2.0.5.
*/
typedef enum {
    WIFI_WPS_DPID_DEFAULT             = 0x0000,     /**<  Default value or use PIN if selected_registrar is true. */
    WIFI_WPS_DPID_USER_SPECIFIED      = 0x0001,     /**<  User specified. */
    WIFI_WPS_DPID_MACHINE_SPECIFIED   = 0x0002,     /**<  Machine specified. */
    WIFI_WPS_DPID_REKEY               = 0x0003,     /**<  Rekey. */
    WIFI_WPS_DPID_PUSHBUTTON          = 0x0004,     /**<  Pushbutton. */
    WIFI_WPS_DPID_REGISTRAR_SPECIFIED = 0x0005,     /**<  Registrar specified. */
    WIFI_WPS_DPID_MAX_NUMBER          = 0xFFFF
} wps_device_password_id_t;


/**
* @brief This enumeration lists the configuration methods supported by the Enrollee or Registrar.
* For more information, please refer to Table 33, "Configuration Methods" in chapter 12, "Data Element Definitions"
* of the Wi-Fi Simple Configuration Technical Specification V2.0.5.
*/
typedef enum {
    WIFI_WPS_CONFIG_USBA                    = 0x0001,  /**<  @deprecated Ignored in WPS version 2.0. */
    WIFI_WPS_CONFIG_ETHERNET                = 0x0002,  /**<  @deprecated Ignored in WPS version 2.0. */
    WIFI_WPS_CONFIG_LABEL                   = 0x0004,  /**<  8-digit static PIN.  */
    WIFI_WPS_CONFIG_DISPLAY                 = 0x0008,  /**<  A dynamic PIN from a display. */
    WIFI_WPS_CONFIG_EXTERNAL_NFC_TOKEN      = 0x0010,  /**<  The NFC tag is used for WPS.  */
    WIFI_WPS_CONFIG_INTEGRATED_NFC_TOKEN    = 0x0020,  /**<  The NFC tag is integrated in the device.  */
    WIFI_WPS_CONFIG_NFC_INTERFACE           = 0x0040,  /**<  The device contains an NFC interface. */
    WIFI_WPS_CONFIG_PUSHBUTTON              = 0x0080,  /**<  WPS PBC supports either a physical or a virtual pushbutton. */
    WIFI_WPS_CONFIG_KEYPAD                  = 0x0100,  /**<  Device is capable of entering a PIN. */
    WIFI_WPS_CONFIG_VIRTUAL_PUSHBUTTON      = 0x0280,  /**<  WPS PBC supports software user interface. */
    WIFI_WPS_CONFIG_PHY_PUSHBUTTON          = 0x0480,  /**<  WPS PBC available on a physical push button of the device. */
    WIFI_WPS_CONFIG_VIRTUAL_DISPLAY         = 0x2008,  /**<  The dynamic PIN is displayed through a remote user interface, such as an HTML page. */
    WIFI_WPS_CONFIG_PHY_DISPLAY             = 0x4008   /**<  The dynamic PIN is shown on a display that is part of the device. */
} wps_config_method_t;

/** @brief This enumeration defines WiFi advanced config type that can be overwrite by user, 
* The type is defined corresponding with #wifi_config_advance_t.
*/
typedef enum {
    WIFI_CONFIG_ADVANCE_SINGLESKU2G           = 0x0000,  /**<  WiFi single_SKU 2G. */
    WIFI_CONFIG_ADVANCE_STA_FAST_LINK_ENABLE  = 0x0001,  /**<  WiFi STA fast link enable. */
    WIFI_CONFIG_ADVANCE_BG_CHANNEL_TABLE      = 0x0002,  /**<  WiFi BG channel table. */
    WIFI_CONFIG_ADVANCE_RF_DIVERSITY          = 0x0003,  /**<  WiFi RF diversity. */
    WIFI_CONFIG_ADVANCE_MAX_NUMBER            = 0xFFFF
} wifi_config_advance_type_t;

/**
* @}
*/

/**@defgroup WIFI_TYPEDEF Typedef
* @{
*/
/**
* @brief This defines the Wi-Fi event handler. Call #wifi_connection_register_event_handler() to register a handler, then the Wi-Fi driver generates an event and sends it to the handler.
* @param[in] event  is an optional event to register. For more details, please refer to #wifi_event_t.
*
* @param[in] payload  is the payload for the event.
*                     When the event is WIFI_EVENT_IOT_CONNECTED in AP mode, payload is the connected STA's MAC address.
*                     When the event is WIFI_EVENT_IOT_CONNECTED in STA mode, payload is the connected AP's BSSID.
* @param[in] length is the length of a packet.
*
* @return The return value is reserved and it is ignored.
*/
typedef int32_t (* wifi_event_handler_t)(wifi_event_t event, uint8_t *payload, uint32_t length);


/**
* @brief This is only for compatibility purpose, so that applications implemented with an earlier version of the API can work with the updated API.
*/
typedef wifi_event_handler_t wifi_event_handler;


/**
* @brief This defines the 802.11 RX raw packet handler. Call #wifi_config_register_rx_handler() to register a handler, then the network processor will deliver the packets to the handler function instead of sending them to the IP stack.
* @param[in] payload  is the packet payload.
* @param[in] length is the packet length.
*
* @return  1: handled, and the packet won't be processed by other tasks 0: not handled, it will be processed by other tasks.
*/
typedef int32_t (* wifi_rx_handler_t)(uint8_t *payload, uint32_t length);


/**
* @}
*/


/**@defgroup WIFI_STRUCT Structure
* @{
*/

/** @brief This structure is the Wi-Fi configuration for initialization in STA mode.
*/
typedef struct {
    uint8_t ssid[WIFI_MAX_LENGTH_OF_SSID];    /**< The SSID of the target AP. */
    uint8_t ssid_length;                      /**< The length of the SSID. */
    uint8_t bssid_present;                    /**< The BSSID is present if it is set to 1. Otherwise, it is set to 0. */
    uint8_t bssid[WIFI_MAC_ADDRESS_LENGTH];   /**< The MAC address of the target AP. */
    uint8_t password[WIFI_LENGTH_PASSPHRASE]; /**< The password of the target AP. */
    uint8_t password_length;                  /**< The length of the password. */
} wifi_sta_config_t;


/** @brief This structure is the Wi-Fi configuration for initialization in AP mode.
*/
typedef struct {
    uint8_t ssid[WIFI_MAX_LENGTH_OF_SSID];    /**< The SSID of the AP. */
    uint8_t ssid_length;                      /**< The length of the SSID. */
    uint8_t password[WIFI_LENGTH_PASSPHRASE]; /**< The password of the AP. */
    uint8_t password_length;                  /**< The length of the password. */
    wifi_auth_mode_t auth_mode;               /**< The authentication mode. */
    wifi_encrypt_type_t encrypt_type;         /**< The encryption mode. */
    uint8_t channel;                          /**< The channel. */
    uint8_t bandwidth;                        /**< The bandwidth that is either #WIFI_IOT_COMMAND_CONFIG_BANDWIDTH_20MHZ or #WIFI_IOT_COMMAND_CONFIG_BANDWIDTH_40MHZ. */
    wifi_bandwidth_ext_t bandwidth_ext;       /**< The bandwidth extension. It is only applicable when the bandwidth is set to #WIFI_IOT_COMMAND_CONFIG_BANDWIDTH_40MHZ. */
} wifi_ap_config_t;


/** @brief Wi-Fi configuration for initialization.
*/
typedef struct {
    uint8_t opmode;                          /**< The operation mode. The value should be #WIFI_MODE_STA_ONLY, #WIFI_MODE_AP_ONLY, #WIFI_MODE_REPEATER or #WIFI_MODE_MONITOR*/
    wifi_sta_config_t sta_config;            /**< The configurations for the STA. It should be set when the OPMODE is #WIFI_MODE_STA_ONLY or #WIFI_MODE_REPEATER. */
    wifi_ap_config_t ap_config;              /**< The configurations for the AP. It should be set when the OPMODE is #WIFI_MODE_AP_ONLY or #WIFI_MODE_REPEATER. */
} wifi_config_t;


/** @brief Wi-Fi extention configuration for initialization.
*/
typedef struct {
    uint32_t sta_wep_key_index_present: 1;        /**< Set to 1 to mark the presence of the sta_wep_key_index, set to 0, otherwise. */
    uint32_t sta_auto_connect_present: 1;         /**< Set to 1 to mark the presence of the sta_auto_connect, set to 0, otherwise. */
    uint32_t ap_wep_key_index_present: 1;         /**< Set to 1 to mark the presence of the ap_wep_key_index, set to 0, otherwise. */
    uint32_t ap_hidden_ssid_enable_present: 1;    /**< Set to 1 to mark the presence of the ap_hidden_ssid_enable, set to 0, otherwise. */
    uint32_t country_code_present: 1;             /**< Set to 1 to mark the presence of the country_code[4], set to 0, otherwise. */
    uint32_t sta_bandwidth_present: 1;            /**< Set to 1 to mark the presence of the sta_bandwidth, set to 0, otherwise. */
    uint32_t sta_wireless_mode_present: 1;        /**< Set to 1 to mark the presence of the sta_wireless_mode, set to 0, otherwise. */
    uint32_t sta_listen_interval_present: 1;      /**< Set to 1 to mark the presence of the sta_listen_interval, set to 0, otherwise. */
    uint32_t sta_power_save_mode_present: 1;      /**< Set to 1 to mark the presence of the sta_power_save_mode, set to 0, otherwise. */
    uint32_t ap_wireless_mode_present: 1;         /**< Set to 1 to mark the presence of the ap_wireless_mode, set to 0, otherwise. */
    uint32_t ap_dtim_interval_present: 1;         /**< Set to 1 to mark the presence of the ap_dtim_interval, set to 0, otherwise. */
    uint32_t reserved_bit: 21;                    /**< Reserved. */
    uint32_t reserved_word[3];                    /**< Reserved. */

    uint8_t sta_wep_key_index;                    /**< The WEP key index for STA. It should be set when the STA uses the WEP encryption. */
    uint8_t sta_auto_connect;                     /**< Set to 1 to enable the STA to automatically connect to the target AP after the initialization. Set to 0 to force the STA to stay idle after the initialization and to call #wifi_config_reload_setting() to trigger connection. The default is set to 1. */
    uint8_t ap_wep_key_index;                     /**< The WEP key index for AP. It should be set when the AP uses WEP encryption. */
    uint8_t ap_hidden_ssid_enable;                /**< Set to 1 to enable the hidden SSID in the beacon and probe response packets. The default is set to 0. */
    uint8_t country_code[4];                      /**< The country code setting. */
    uint8_t sta_bandwidth;                        /**< The bandwidth setting for STA. The value is either #WIFI_IOT_COMMAND_CONFIG_BANDWIDTH_20MHZ or #WIFI_IOT_COMMAND_CONFIG_BANDWIDTH_40MHZ, or WIFI_IOT_COMMAND_CONFIG_BANDWIDTH_2040MHZ.*/
    wifi_phy_mode_t sta_wireless_mode;            /**< The wireless mode setting for STA. Please refer to the definition of #wifi_phy_mode_t.*/
    uint8_t sta_listen_interval;                  /**< The listening interval setting for STA. The interval range is from 1 to 255 beacon intervals.*/
    wifi_power_saving_mode_t sta_power_save_mode; /**< The power saving mode setting for STA. Please refer to the definition of #wifi_power_saving_mode_t.*/
    wifi_phy_mode_t ap_wireless_mode;             /**< The wireless mode setting for AP. Please refer to the definition of #wifi_phy_mode_t.*/
    uint8_t ap_dtim_interval;                     /**< The DTIM interval setting for AP. The interval range is from 1 to 255 beacon intervals. */
} wifi_config_ext_t;


/** @brief Wi-Fi WEP keys.
*/
typedef struct {
    uint8_t wep_key[WIFI_NUMBER_WEP_KEYS][WIFI_MAX_WEP_KEY_LENGTH];     /**< There are four WEP keys. */
    uint8_t wep_key_length[WIFI_NUMBER_WEP_KEYS];                       /**< WEP key length. */
    uint8_t wep_tx_key_index;                                           /**< Default key index for TX frames using WEP. */
} wifi_wep_key_t;


/** @brief Wi-Fi profile entity.
*/
typedef struct {
    uint8_t ssid[WIFI_MAX_LENGTH_OF_SSID];          /**< Stores the predefined SSID.  */
    uint8_t ssid_length;                            /**< The length of SSID.         */
    uint8_t mac_address[WIFI_MAC_ADDRESS_LENGTH];   /**< MAC address of the STA.     */
    uint8_t wireless_mode;                          /**< Please refer to the definition of #wifi_phy_mode_t. */
    uint8_t channel;                                /**< The channel used.                    */
    uint8_t bandwidth;                              /**< The bandwidth is 20MHz or 40MHz.     */
    uint8_t mcs;                                    /**< Defines the packet data rate.        */
    uint8_t auth_mode;                              /**< Wi-Fi authentication mode, please refer to the definition of #wifi_auth_mode_t.   */
    uint8_t encrypt_mode;                           /**< Wi-Fi encryption mode, please refer to the definition of #wifi_encrypt_type_t.*/
} wifi_profile_t;


/** @brief This defines the WPS element.
*/
typedef struct {
    bool selected_registrar;                        /**< Indicates whether AP WPS is triggered and in progress. 1 - triggered, 0 - not triggered. */
    wps_device_password_id_t device_password_id;    /**< Indicates AP WPS status, only when selected_registrar is 1, this parameter will take effect. Please refer to the definition of #wps_device_password_id_t. */
    wps_config_method_t configuration_methods;      /**< Indicates AP's capability to support WPS methods. Please refer to the definition of #wps_config_method_t.  */
} wifi_wps_element_t;


/** @brief This structure defines the list of scanned APs with their corresponding information.
*/
typedef struct {
    uint8_t  is_valid;                         /**< Indicates whether the scanned item is valid. */
    int8_t   rssi;                             /**< Records the RSSI value when probe response is received. */
    uint8_t  ssid[WIFI_MAX_LENGTH_OF_SSID];    /**< Stores the predefined SSID. */
    uint8_t  ssid_length;                      /**< Length of the SSID. */
    uint8_t  bssid[WIFI_MAC_ADDRESS_LENGTH];   /**< AP's MAC address. */
    uint8_t  channel;                          /**< The channel used. */
    uint8_t  central_channel;                  /**< The center channel in 40MHz bandwidth. */
    bool     is_wps_supported;                 /**< Indicates whether WPS is supported or not. 1: WPS is supported, 0: WPS is not supported. */
    wifi_auth_mode_t     auth_mode;            /**< Please refer to the definition of #wifi_auth_mode_t. */
    wifi_encrypt_type_t  encrypt_type;         /**< Please refer to the definition of #wifi_encrypt_type_t. */
    bool     is_hidden;                        /**< Indicates the SSID is hidden or not. 1: hidden SSID, 0: not hidden SSID. */
    uint8_t  bss_type;                         /**< Indicates the type of BSS. 0: IBSS 1: Infrastructure. */
    uint16_t beacon_interval;                  /**< Indicates the beacon interval. */
    uint16_t capability_info;                  /**< The Capability Information field contains a number of subfields that are used to indicate requested or advertised optional capabilities. */
    wifi_wps_element_t wps_element;            /**< Please refer to the definition of #wifi_wps_element_t. */
    uint8_t  dtim_period;                      /**< The dtim period of Beacon. */
} wifi_scan_list_item_t;

/** @brief This structure defines a link to the scanned AP list.
*/
typedef struct {
    wifi_scan_list_item_t ap_data;  /**< The information about an AP obtained through the scan result is stored in this data member. The information for all scanned AP(s) is stored in the linked list. Please refer to the definition of #wifi_scan_list_item_t.*/
    struct _wifi_scan_list_t *next; /**< Next item in the list.*/
} wifi_scan_list_t;

/** @brief The RSSI sample is a measurement of the power in a received radio signal.
*/
typedef struct {
    int8_t          last_rssi;             /**< The latest received RSSI.     */
    uint8_t         reserved;              /**< Reserved.               */
    int16_t         average_rssi;          /**< 8 times of the average RSSI.*/
} wifi_rssi_sample_t;


/** @brief Specifies the Wi-Fi transmitter settings, including MCS, LDPC, bandwidth, short-GI algorithms, STBC and others.
*/
typedef union {
    struct {
        uint16_t mcs: 6;                      /**< Implies the data rate. */
        uint16_t ldpc: 1;                     /**< Checks if a low-density parity-check code is adopted. */
        uint16_t bandwidth: 2;                /**< Decides the bandwidth. */
        uint16_t short_gi: 1;                 /**< Checks if short-GI algorithms are applied. */
        uint16_t stbc: 1;                     /**< Checks if a space-time code is adopted.    */
        uint16_t external_tx_beam_forming: 1; /**< Checks if an external buffer is adopted.   */
        uint16_t internal_tx_beam_forming: 1; /**< Checks if an internal buffer is adopted.   */
        uint16_t mode: 3;
    } field;                                 /**< Structure name for the Wi-Fi transmitter settings. */
    uint16_t word;                           /**< Defines the length of the union data. */
} wifi_transmit_setting_t;

/** @brief Specifies the Wi-Fi station items, including the Wi-Fi transmitter settings, the RSSI sample, MAC address, power saving mode, bandwidth and keep alive status.
*/
typedef struct {
    wifi_transmit_setting_t last_tx_rate;            /**< The latest TX rate.  */
    uint16_t reserved_one;                           /**< For padding.   */
    wifi_transmit_setting_t last_rx_rate;            /**< The latest RX rate.  */
    uint16_t reserved_two;                           /**< For padding.   */
    wifi_rssi_sample_t rssi_sample;                  /**< The RSSI sample.   */
    uint8_t mac_address[WIFI_MAC_ADDRESS_LENGTH];    /**< Station MAC address.     */
    uint8_t power_save_mode;                         /**< Station power save mode. */
    uint8_t bandwidth;                               /**< Negotiation bandwidth.   */
    uint8_t keep_alive;                              /**< Keep alive status.       */
    uint8_t reserved[3];                             /**< For padding.             */
} wifi_sta_list_t;

/** @brief Indicates the detailed cipher information.
*/
typedef struct {
    uint8_t   key[16];            /**< Four keys are implemented, with a maximum length of 128bits.*/
    uint8_t   rx_mic[8];          /**< 8 bytes of RX MIC key.  */
    uint8_t   tx_mic[8];          /**< 8 bytes of TX MIC key. */
    uint8_t   tx_tsc[6];          /**< 48bit TSC value. */
    uint8_t   rx_tsc[6];          /**< 48bit TSC value. */
    uint8_t   cipher_type;        /**< 0:none, 1:WEP64, 2:WEP128, 3:TKIP, 4:AES, 5:CKIP64, 6:CKIP128.*/
    uint8_t   key_length;         /**< Key length for each key, 0: entry is invalid.*/
    uint8_t   bssid[6];           /**< AP's MAC address. */
    uint8_t   type;               /**< Indicates the type (pairwise or group) when reporting a MIC error.*/
} wifi_cipher_key_t;

/** @brief This structures provide the country code information.
*/
typedef struct {
    uint8_t first_channel;        /**< The first channel in the single channel table.*/
    uint8_t num_of_channel;       /**< The number of channels in the single channel table.*/
    uint8_t max_tx_power;         /**< The maximum transmit power in the single channel table, the unit is dBm.*/
    uint8_t geography;            /**< Not supported.*/
    bool    dfs_req;              /**< 0: No DFS request, 1: DFS request.*/
} wifi_channel_table_t;

typedef struct {
    uint8_t country_code[4];                /**< Country code string.*/
    uint8_t dfs_type;                       /**< Not supported.*/
    uint8_t num_of_channel_table;           /**< Number of channel table.*/
    wifi_channel_table_t channel_table[];   /**< The details of each channel table, please refer to #wifi_channel_table_t.*/
} wifi_country_code_t;

/** @brief Indicates the wlan statistic information.
*/
typedef struct {
    uint32_t Current_Temperature;  /**< Temperature of the device.*/
    uint32_t Tx_Success_Count;     /**< No of TX success, value is reset upon read.*/
    uint32_t Tx_Retry_Count;       /**< No of packet retries, value is reset upon read.*/
    uint32_t Tx_Fail_Count;        /**< No of TX Fails, value is reset upon read.*/
    uint32_t Rx_Success_Count;     /**< No of RX success, value is reset upon read.*/
    uint32_t Rx_with_CRC;          /**< No of FCS Errors, value is reset upon read.*/
    uint32_t Rx_Drop_due_to_out_of_resources;/**< No of RX drops due to FIFO Full. Value is reset upon read.*/
    uint32_t MIC_Error_Count;      /**< No of MIC Errors, value is reset upon read.*/
    uint32_t Cipher_Error_Count;   /**< No of Cipher errors, value is reset upon read.*/
    int8_t   Rssi;                 /**< Average RSSI of Data Packets.*/
    uint32_t Tx_AGG_Range_1;       /**< A-MPDU count with Aggregate Size in the range 1. Value is reset upon read.*/
    uint32_t Tx_AGG_Range_2;       /**< A-MPDU count with Aggregate Size in the range 2. Value is reset upon read.*/
    uint32_t Tx_AGG_Range_3;       /**< A-MPDU count with Aggregate Size in the range 3. Value is reset upon read.*/
    uint32_t Tx_AGG_Range_4;       /**< A-MPDU count with Aggregate Size in the range 4. Value is reset upon read.*/
    uint32_t AMPDU_Tx_Success;     /**< No of AMPDU TX Success, value is reset upon read.*/
    uint32_t AMPDU_Tx_Fail;        /**< No of AMPDU TX Failure, value is reset upon read.*/
    int32_t  SNR;                  /**< Average SNR from Data packets.*/
    uint16_t MAX_PHY_Rate;         /**< Max_phy_rate, value is reset upon read*/
    uint16_t REAL_TX_PHY_Rate;     /**< Real_Tx_phy_rate, value is reset upon read*/
    uint16_t REAL_RX_PHY_Rate;     /**< Real_Rx_phy_rate */
    uint8_t  BBPCurrentBW;         /**< BW_20=  0  , BW_40=  1*/
    uint8_t  REAL_TX_PHY_Mode;     /**< TX Mode , indicate the transmission mode 0: Legacy CCK, 1: Legacy OFDM, 2: HT Mixed mode, 3: HT Green field mode */
    uint8_t  REAL_RX_PHY_Mode;     /**< TX Mode , indicate the transmission mode 0: Legacy CCK, 1: Legacy OFDM, 2: HT Mixed mode, 3: HT Green field mode */
    uint8_t  REAL_TX_ShortGI;      /**< 0:Normal GI (800) , 1:Short GI (400) */
    uint8_t  REAL_RX_ShortGI;      /**< 0:Normal GI (800) , 1:Short GI (400) */
    uint8_t  REAL_TX_MCS;          /**< MCS 0 to 7 */
    uint8_t  REAL_RX_MCS;          /**< MCS 0 to 7 */
} wifi_statistic_t;

#ifdef MTK_WIFI_WPS_ENABLE
/**
* @brief Represents the device information used for WPS.
*/
typedef struct {
    char device_name[WIFI_WPS_MAX_LENGTH_DEVICE_NAME];          /**< Device name, up to 32 octets encoded in UTF-8. */
    char manufacturer[WIFI_WPS_MAX_LENGTH_MANUFACTURER];        /**< Device manufacturer, up to 64 ASCII characters . */
    char model_name[WIFI_WPS_MAX_LENGTH_MODEL_NAME];            /**< Model name, up to 32 ASCII characters. */
    char model_number[WIFI_WPS_MAX_LENGTH_MODEL_NUMBER];        /**< Model number, up to 32 ASCII characters. */
    char serial_number[WIFI_WPS_MAX_LENGTH_SERIAL_NUMBER];      /**< Serial number of the device, up to 32 characters. */
} wifi_wps_device_info_t;

/**
* @brief Represents the credentials used for WPS.
*/
typedef struct {
    uint8_t  ssid[WIFI_MAX_LENGTH_OF_SSID];   /**< Stores Registrar network SSID. */
    uint32_t ssid_len;                        /**< The length of Registrar network SSID. */
    wifi_auth_mode_t     auth_mode;           /**< Please refer to the definition of #wifi_auth_mode_t. */
    wifi_encrypt_type_t  encrypt_type;        /**< Please refer to the definition of #wifi_encrypt_type_t. */
    uint8_t  key_index;                       /**< The WEP key inndex.*/
    uint8_t  key[WIFI_LENGTH_PASSPHRASE];     /**< The key. */
    uint32_t key_length;                      /**< Key length.*/
} wifi_wps_credential_info_t;

#endif //end of MTK_WIFI_WPS_ENABLE

/** @brief Indicates the detailed single SKU information.
* For each channel power item format is
* | -------|----------|----------|----------- |------------|-------- |---------|---------|---------- |---------- |-------- |-------- |-------- | --------|---------- |---------- |-------- |-------- |-------- |
* |CCK1/2M|CCK5.5/11M|OFDM_6/9M|OFDM_12/18M|OFDM_24/36M|OFDM_48M|OFDM_54M| HT20MCS0|HT20MCS1/2|HT20MCS3/4|HT20MCS5|HT20MCS6|HT20MCS7|HT40MCS0|HT40MCS1/2|HT40MCS3/4|HT40MCS5|HT40MCS6|HT40MCS7|
*/
typedef struct {
    uint8_t singleSKU2G[WIFI_2G_CHANNEL_NUMBER][WIFI_2G_SKU_POWER_PER_CHANNEL];
} singleSKU2G_t;

#ifdef MTK_ANT_DIV_ENABLE
typedef struct {
    uint8_t antenna_number;             /**< The antenna number 0/1, defualt is 0. */
    uint8_t antenna_diversity_enable;   /**< The RF diversity feature enable. 1 is enable, 0 is disable, defualt is 0. */
    uint8_t antenna_probe_req_count;    /**< The probe request count that will send for rssi collection from probe response from target AP. */
    int8_t  antenna_threshold_level;    /**< The rssi threshold to trigger RF diversity algorithm when main antenna rssi less than threshold, it used together with  #antenna_delta_rssi. */
    uint8_t antenna_delta_rssi;         /**< The delta rssi threshold to trigger RF diversity algorithm, when the diff between main antenna and #antenna_pre_selected_rssi is bigger than it, it used together with  #antenna_threshold_level. */
    uint8_t antenna_rssi_switch;        /**< The rssi switch threshold, when the diff between aux antenna rssi and main antenna rssi is bigger than it, the antenna will switch. */
    int8_t  antenna_pre_selected_rssi;  /**< The rssi of the last time antenna was switched. */
} wifi_rf_diversity;
#endif

/** @brief Represents WiFi advanced config item, will overwrite by user defined handler #wifi_init_config_handler_t
*/
typedef struct {
    singleSKU2G_t * single_sku_2g;            /**< 2G single SKU table. */
    uint8_t * sta_fast_link;                  /**< The sta_fast_link set to 1 to enable the STA fast link feature, it is only for STA mode and will trigger WiFi FirmWare auto scan the target AP once after FirmWare download finish to reduce the wifi connected time. 
                                                                       The target AP is specified by ssid in #wifi_sta_config_t and bg_channel_table or  a_channel_table in #wifi_config_ext_t when WiFi init. The default is set to 0. */
    char *    bg_channel_table;               /**< The bg_channel_table setting for AP/STA. It is used to build channel list for WiFi scanning and connect, it must set as string "first_channel,number_of_channel,scan_option|" for one group and support at most 10 group. 
                                                                        In this string, the "first_channel" and "number_of_channel" is described as #wifi_channel_table_t, the "scan_option" is described in #wifi_connection_start_scan()  and STA only supports full scan mode, AP only support partical scan mode, the '|' is terminator of channel table or separator between multiple channel tables*/
#ifdef MTK_ANT_DIV_ENABLE
    wifi_rf_diversity * rf_diversity;         /**< The WiFi RF diversity setting. */
#endif
}wifi_config_advance_t;

/**
* @brief This defines the WiFi advanced config item #wifi_config_advance_t overwrite handler. Call #wifi_init_register_config_handler() to register a handler, then the WiFi driver will deliver the WiFi advanced config struct point to handler for overwriting.
* @param[in] profile_type is the WiFi advanced config item type.
* @param[in] profile_data is the WiFi advanced structure point.
*
* @return The return value is reserved and it is ignored.
*/
typedef int32_t (* wifi_init_config_handler_t)(wifi_config_advance_type_t profile_type, wifi_config_advance_t * profile_data);

/**
*@}
*/


/**
* @brief This function initializes the Wi-Fi module.
*
* @param[in] config is the Wi-Fi configuration to be set, it should not be null.
*
* @param[in] config_ext is the extended Wi-Fi configuration to be set, it can be null if no extended features are expected.
*
* @note Call this function only once at the initialization stage.
*/
void wifi_init(wifi_config_t *config, wifi_config_ext_t *config_ext);

/**
* @brief This function receives the wireless operation mode of the Wi-Fi driver.
*
* @param[out]  mode indicates the operation mode.
*
* Value                         |Definition                                                              |
* ------------------------------|------------------------------------------------------------------------|
* \b 0x01                       | #WIFI_MODE_STA_ONLY|
* \b 0x02                       | #WIFI_MODE_AP_ONLY|
* \b 0x03                       | #WIFI_MODE_REPEATER|
* \b 0x04                       | #WIFI_MODE_MONITOR|
*
* @return  >=0 the operation completed successfully, <0 the operation failed.
*/
int32_t wifi_config_get_opmode(uint8_t *mode);

/*set ip status*/
int32_t wifi_set_is_ap_connected(uint8_t mode);
int32_t wifi_set_is_ap_ip_ready(uint8_t mode);
int32_t wifi_set_is_ip_ready(uint8_t mode);


/**
* @brief This function sets the Wi-Fi operation mode and it takes effect immediately.
*
* @param[in] mode the operation mode to set.
*
* Value                         |Definition                                                              |
* ------------------------------|------------------------------------------------------------------------|
* \b 0x01                       | #WIFI_MODE_STA_ONLY|
* \b 0x02                       | #WIFI_MODE_AP_ONLY|
* \b 0x03                       | #WIFI_MODE_REPEATER|
* \b 0x04                       | #WIFI_MODE_MONITOR|
*
* @return  >=0 the operation completed successfully, <0 the operation failed.
*
* @note This function resets the Wi-Fi driver configuration to default values of the target's operation mode (including supplicant and firmware) previously set by Wi-Fi Configuration APIs.
*/
int32_t wifi_config_set_opmode(uint8_t mode);

/**
* @brief This function gets the MAC address of a specific wireless port used by the Wi-Fi driver.
*
* @param[in] port indicates the Wi-Fi port that the function will operate on.
*
* Value                         |Definition                                                              |
* ------------------------------|------------------------------------------------------------------------|
* \b #WIFI_PORT_STA             | STA|
* \b #WIFI_PORT_APCLI           | AP Client|
* \b #WIFI_PORT_AP              | AP|
*
* @param[out]  address is the MAC address. The length of the MAC address is #WIFI_MAC_ADDRESS_LENGTH in bytes.
*
* @return  >=0 the operation completed successfully, <0 the operation failed.
*
* @note To get the MAC address, follow these steps:
*       1. Read eFuse from address 0x00, take bytes 4 to 9 as the STA MAC address and generate AP address from it by increasing the last byte by 1.
*       2. Read NVDM from group "STA" and key "MacAddr" for STA MAC address and from group "AP" and key "MacAddr" for AP MAC address.
*          If the MAC address is retrieved successfully in Step 1, skip Step 2.
*          If no information is set in eFuse and NVDM, this function will return an error and consequently the Wi-Fi driver initialization will possibly fail, too.
*
*/
int32_t wifi_config_get_mac_address(uint8_t port, uint8_t *address);

/**
* @brief This function gets the SSID and SSID length of a specific wireless port used by the Wi-Fi driver.
*
* @param[in] port indicates the Wi-Fi port that the function will operate on.
*
* Value                         |Definition                                                              |
* ------------------------------|------------------------------------------------------------------------|
* \b #WIFI_PORT_STA             | STA|
* \b #WIFI_PORT_APCLI           | AP Client|
* \b #WIFI_PORT_AP              | AP|
*
* @param[out]  ssid is the SSID content. In STA mode, the SSID is the target AP's SSID;\n
*                       In SoftAP mode, the SSID indicates the device's own SSID.
* @param[out]  ssid_length is the length of the SSID. The maximum length is #WIFI_MAX_LENGTH_OF_SSID in bytes.
*
* @return  >=0 the operation completed successfully, <0 the operation failed.
*/
int32_t wifi_config_get_ssid(uint8_t port, uint8_t *ssid, uint8_t *ssid_length);

/**
* @brief This function sets the SSID and SSID length that the Wi-Fi driver uses for a specific wireless port.
* This operation only takes effect after #wifi_config_reload_setting() is called.
*
* @param[in] port indicates the Wi-Fi port that the function will operate on.
*
* Value                         |Definition                                                              |
* ------------------------------|------------------------------------------------------------------------|
* \b #WIFI_PORT_STA             | STA|
* \b #WIFI_PORT_APCLI           | AP Client|
* \b #WIFI_PORT_AP              | AP|
*
* @param[in] ssid is the SSID content. If #WIFI_PORT_STA or #WIFI_PORT_APCLI, the SSID indicates the target AP's SSID.
*                       If WIFI_PORT_AP, the SSID indicates the device's own SSID.
* @param[in] ssid_length is the length of the SSID, the maximum length is #WIFI_MAX_LENGTH_OF_SSID in bytes.
*
* @return  >=0 the operation completed successfully, <0 the operation failed.
*/
int32_t wifi_config_set_ssid(uint8_t port, uint8_t *ssid, uint8_t ssid_length);

/**
* @brief This function should be used only in station mode or in APCLI port of repeater mode.
*
* @param[out] bssid is the BSSID of the target AP that this station device or the APCLI port is connected to.
*                   If bssid is NULL, it indicates this device is not connected yet.
*
* @return  >=0 the operation completed successfully, <0 the operation failed.
*/
int32_t wifi_config_get_bssid(uint8_t *bssid);

/**
* @brief This function should be used only in station mode or in APCLI port of repeater mode. It must be called along with #wifi_config_set_ssid().
* When both #wifi_config_set_ssid() and #wifi_config_set_bssid() are called, the device will connect to the AP with matching SSID and BSSID.
* If only #wifi_config_set_bssid() is called, the connection might fail.
* This operation only takes effect after #wifi_config_reload_setting() is called.
* BSSID configuration setting (#wifi_config_set_bssid()) is optional.
* To cancel the BSSID setting, simply do not call #wifi_config_set_bssid() before #wifi_config_reload_setting().
*
* @param[in] bssid is the target AP's BSSID to connect.
*
* @return  >=0 the operation completed successfully, <0 the operation failed.
*/
int32_t wifi_config_set_bssid(uint8_t *bssid);

/**
* @brief This function sets the bandwidth configuration that the Wi-Fi driver uses for a specific wireless port.
* This operation takes effect immediately.
*
* @param[in] port indicates the Wi-Fi port that the function will operate on.
*
* Value                         |Definition                                                              |
* ------------------------------|------------------------------------------------------------------------|
* \b #WIFI_PORT_STA             | STA|
* \b #WIFI_PORT_APCLI           | AP Client|
* \b #WIFI_PORT_AP              | AP|
*
* @param[in]  bandwidth is the wireless bandwidth.
*
* Value                                                |Definition                                                                 |
* -----------------------------------------------------|-------------------------------------------------|
* \b #WIFI_IOT_COMMAND_CONFIG_BANDWIDTH_20MHZ          | 20MHz |
* \b #WIFI_IOT_COMMAND_CONFIG_BANDWIDTH_40MHZ          | 40MHz |
* \b #WIFI_IOT_COMMAND_CONFIG_BANDWIDTH_2040MHZ        | 20 or 40MHz |
*
*
* @return  >=0 the operation completed successfully, <0 the operation failed.
*
* @note    The default value is #WIFI_IOT_COMMAND_CONFIG_BANDWIDTH_20MHZ.
*/
int32_t wifi_config_set_bandwidth(uint8_t port, uint8_t bandwidth);

/**
* @brief   This function gets the bandwidth configuration that the Wi-Fi driver uses for a specific wireless port.
*
* @param[in] port indicates the Wi-Fi port that the function will operate on.
*
* Value                         |Definition                                                              |
* ------------------------------|------------------------------------------------------------------------|
* \b #WIFI_PORT_STA             | STA|
* \b #WIFI_PORT_APCLI           | AP Client|
* \b #WIFI_PORT_AP              | AP|
*
* @param[out]   bandwidth is the wireless bandwidth.
*
* Value                                                  |Definition                                     |
* -------------------------------------------------------|-----------------------------------------------|
* \b #WIFI_IOT_COMMAND_CONFIG_BANDWIDTH_20MHZ            | 20MHz |
* \b #WIFI_IOT_COMMAND_CONFIG_BANDWIDTH_40MHZ            | 40MHz |
* \b #WIFI_IOT_COMMAND_CONFIG_BANDWIDTH_2040MHZ          | 20 or 40MHz |
*
*
* @return  >=0 the operation completed successfully, <0 the operation failed.
*
* @note    The default value is #WIFI_IOT_COMMAND_CONFIG_BANDWIDTH_20MHZ.
*/
int32_t wifi_config_get_bandwidth(uint8_t port, uint8_t *bandwidth);

/**
* @brief This function sets the channel number that the Wi-Fi driver uses for a specific wireless port.
* The STA remains idle on a specified channel. Call #wifi_config_set_channel()
* in station mode while the device is disconnected from the AP, then the device will scan and change the channel in a loop.
* This operation takes effect immediately in the station mode, in the AP mode, it'll take effect after calling #wifi_config_reload_setting().
* @param[in] port indicates the Wi-Fi port that the function will operate on.
*
* Value                         |Definition                                                              |
* ------------------------------|------------------------------------------------------------------------|
* \b #WIFI_PORT_STA             | STA|
* \b #WIFI_PORT_APCLI           | AP Client|
* \b #WIFI_PORT_AP              | AP|
*
* @param[in] channel    is the channel number that the Wi-Fi driver uses for a specific wireless port.
*                       The channel number range is from 1 to 14 for 2.4GHz bandwidth. The specific channel number is determined
*                       by country region settings in the profile.
*
* @return  >=0 the operation completed successfully, <0 the operation failed.
*/
int32_t wifi_config_set_channel(uint8_t port, uint8_t channel);

/**
* @brief This function gets the channel number that the Wi-Fi driver uses for a specific wireless port.
*
* @param[in] port is the Wi-Fi port that the function will operate on.
*
* Value                         |Definition                                                              |
* ------------------------------|------------------------------------------------------------------------|
* \b #WIFI_PORT_STA             | STA|
* \b #WIFI_PORT_APCLI           | AP Client|
* \b #WIFI_PORT_AP              | AP|
*
* @param[out]  channel    is the channel number that the Wi-Fi driver uses for a specific wireless port.
*                         The channel number range is from 1 to 14 for 2.4GHz bandwidth. The specific channel number is determined
*                         by country region settings in the profile.
*
* @return  >=0 the operation completed successfully, <0 the operation failed.
*/
int32_t wifi_config_get_channel(uint8_t port, uint8_t *channel);


/**
* @brief This function gets the wireless mode that the Wi-Fi driver uses for a specific wireless port.
*
* @param[in] port is the Wi-Fi port that the function will operate on.
*
* Value                         |Definition                                                              |
* ------------------------------|------------------------------------------------------------------------|
* \b #WIFI_PORT_STA             | STA|
* \b #WIFI_PORT_APCLI           | AP Client|
* \b #WIFI_PORT_AP              | AP|
*
* @param[out]  mode is the wireless mode. For more information, please refer to #wifi_phy_mode_t.
*
* @return  >=0 the operation completed successfully, <0 the operation failed.
*/
int32_t wifi_config_get_wireless_mode(uint8_t port, wifi_phy_mode_t *mode);


/**
* @brief This function gets the wireless mode that the Wi-Fi driver uses for a specific wireless port.
* This operation takes effect immediately.
*
* @param[in] port is the Wi-Fi port that the function will operate on.
*
* Value                         |Definition                                                              |
* ------------------------------|------------------------------------------------------------------------|
* \b #WIFI_PORT_STA             | STA|
* \b #WIFI_PORT_APCLI           | AP Client|
* \b #WIFI_PORT_AP              | AP|
*
* @param[in] mode is the wireless mode. For more information, please refer to #wifi_phy_mode_t.
*
* @return  >=0 the operation completed successfully, <0 the operation failed.
*/
int32_t wifi_config_set_wireless_mode(uint8_t port, wifi_phy_mode_t mode);


/**
* @brief Set the country code.
*
* @param[in] wifi_country_code is  the country code that the driver uses. For more information, please refer to #wifi_country_code_t.
* @note The standard country code settings are provided below, initialize wifi_country_code based on the requirements.
* @note The country code can also be custom-defined.
* @note This API will re-build the channel list used for WiFi scanning and connect.
*
* @code
*  static wifi_country_code_t wifi_country_code[] =
*  {
*        {    // Antigua and Berbuda
*            "AG",
*             CE,
*             4,
*        {
*                { 1,   13, 20, BOTH, FALSE},    // 2.4GHz, ch 1~13
*                { 36,  4,  23, BOTH, FALSE},    // 5GHz, ch 36~48
*                { 52,  4,  23, BOTH, FALSE},    // 5GHz, ch 52~64
*                { 100, 11, 30, BOTH, FALSE},    // 5GHz, ch 100~140
*            }
*        },
*        {    // Argentina
*            "AR",
*             CE,
*             3,
*            {
*                { 1,   13, 20, BOTH, FALSE},    // 2.4GHz, ch 1~13
*                { 52,  4,  24, BOTH, FALSE},    // 5GHz, ch 52~64
*                { 149, 4,  30, BOTH, FALSE},    // 5GHz, ch 149~161
*            }
*        },
*
*        {    // Aruba
*            "AW",
*             CE,
*             4,
*            {
*                { 1,   13, 20, BOTH, FALSE},    // 2.4GHz, ch 1~13
*                { 36,  4,  23, BOTH, FALSE},    // 5GHz, ch 36~48
*                { 52,  4,  23, BOTH, FALSE},    // 5GHz, ch 52~64
*                { 100, 11, 30, BOTH, FALSE},    // 5GHz, ch 100~140
*            }
*        },
*
*        {    // Australia
*            "AU",
*             CE,
*             4,
*            {
*                { 1,   13, 20, BOTH, FALSE},    // 2.4GHz, ch 1~13
*                { 36,  4,  23, BOTH, FALSE},    // 5GHz, ch 36~48
*                { 52,  4,  24, BOTH, FALSE},    // 5GHz, ch 52~64
*                { 149, 5,  30, BOTH, FALSE},    // 5GHz, ch 149~165
*            }
*        },
*
*        {    // Austria
*            "AT",
*             CE,
*             4,
*            {
*                { 1,   13, 20, BOTH, FALSE},    // 2.4GHz, ch 1~13
*                { 36,  4,  23, IDOR, TRUE},     // 5GHz, ch 36~48
*                { 52,  4,  23, IDOR, TRUE},     // 5GHz, ch 52~64
*                { 100, 11, 30, BOTH, TRUE},     // 5GHz, ch 100~140
*            }
*        },
*
*        {    // Bahamas
*            "BS",
*             CE,
*             4,
*            {
*                { 1,   13, 20, BOTH, FALSE},    // 2.4GHz, ch 1~13
*                { 36,  4,  23, BOTH, FALSE},    // 5GHz, ch 36~48
*                { 52,  4,  24, BOTH, FALSE},    // 5GHz, ch 52~64
*                { 149, 5,  30, BOTH, FALSE},    // 5GHz, ch 149~165
*            }
*        },
*
*        {    // Barbados
*            "BB",
*             CE,
*             4,
*            {
*                { 1,   13, 20, BOTH, FALSE},    // 2.4GHz, ch 1~13
*                { 36,  4,  23, BOTH, FALSE},    // 5GHz, ch 36~48
*                { 52,  4,  24, BOTH, FALSE},    // 5GHz, ch 52~64
*                { 100, 11, 30, BOTH, FALSE},    // 5GHz, ch 100~140
*            }
*        },
*
*        {    // Bermuda
*            "BM",
*             CE,
*             4,
*            {
*                { 1,   13, 20, BOTH, FALSE},    // 2.4GHz, ch 1~13
*                { 36,  4,  23, BOTH, FALSE},    // 5GHz, ch 36~48
*                { 52,  4,  24, BOTH, FALSE},    // 5GHz, ch 52~64
*                { 100, 11, 30, BOTH, FALSE},    // 5GHz, ch 100~140
*            }
*        },
*
*        {    // Brazil
*            "BR",
*             CE,
*             5,
*            {
*                { 1,   13, 20, BOTH, FALSE},    // 2.4GHz, ch 1~13
*                { 36,  4,  23, BOTH, FALSE},    // 5GHz, ch 36~48
*                { 52,  4,  24, BOTH, FALSE},    // 5GHz, ch 52~64
*                { 100, 11, 24, BOTH, FALSE},    // 5GHz, ch 100~140
*                { 149, 5,  30, BOTH, FALSE},    // 5GHz, ch 100~140
*            }
*        },
*
*        {    // Belgium
*            "BE",
*             CE,
*             3,
*            {
*                { 1,   13, 20, BOTH, FALSE},    // 2.4GHz, ch 1~13
*                { 36,  4,  18, IDOR, FALSE},    // 5GHz, ch 36~48
*                { 52,  4,  18, IDOR, FALSE},    // 5GHz, ch 52~64
*            }
*        },
*
*        {    // Bulgaria
*            "BG",
*             CE,
*             4,
*            {
*                { 1,   13, 20, BOTH, FALSE},    // 2.4GHz, ch 1~13
*                { 36,  4,  23, IDOR, FALSE},    // 5GHz, ch 36~48
*                { 52,  4,  23, IDOR, TRUE},     // 5GHz, ch 52~64
*                { 100, 11, 30, ODOR, TRUE},     // 5GHz, ch 100~140
*            }
*        },
*
*        {    // Canada
*            "CA",
*             CE,
*             4,
*            {
*                { 1,   13, 20, BOTH, FALSE},    // 2.4GHz, ch 1~13
*                { 36,  4,  23, BOTH, FALSE},    // 5GHz, ch 36~48
*                { 52,  4,  23, BOTH, FALSE},    // 5GHz, ch 52~64
*                { 149, 5,  30, BOTH, FALSE},    // 5GHz, ch 149~165
*            }
*        },
*
*        {    // Cayman IsLands
*            "KY",
*             CE,
*             4,
*            {
*                { 1,   13, 20, BOTH, FALSE},    // 2.4GHz, ch 1~13
*                { 36,  4,  23, BOTH, FALSE},    // 5GHz, ch 36~48
*                { 52,  4,  24, BOTH, FALSE},    // 5GHz, ch 52~64
*                { 100, 11, 30, BOTH, FALSE},    // 5GHz, ch 100~140
*            }
*        },
*
*        {    // Chile
*            "CL",
*             CE,
*             4,
*            {
*                { 1,   13, 20, BOTH, FALSE},    // 2.4GHz, ch 1~13
*                { 36,  4,  20, BOTH, FALSE},    // 5GHz, ch 36~48
*                { 52,  4,  20, BOTH, FALSE},    // 5GHz, ch 52~64
*                { 149, 5,  20, BOTH, FALSE},    // 5GHz, ch 149~165
*            }
*        },
*
*        {    // China
*            "CN",
*             CE,
*             2,
*            {
*                { 1,   13, 20, BOTH, FALSE},    // 2.4GHz, ch 1~13
*                { 149, 4,  27, BOTH, FALSE},    // 5GHz, ch 149~161
*            }
*        },
*
*        {    // Colombia
*            "CO",
*             CE,
*             5,
*            {
*                { 1,   13, 20, BOTH, FALSE},    // 2.4GHz, ch 1~13
*                { 36,  4,  17, BOTH, FALSE},    // 5GHz, ch 36~48
*                { 52,  4,  24, BOTH, FALSE},    // 5GHz, ch 52~64
*                { 100, 11, 30, BOTH, FALSE},    // 5GHz, ch 100~140
*                { 149, 5,  30, BOTH, FALSE},    // 5GHz, ch 149~165
*            }
*        },
*
*        {    // Costa Rica
*            "CR",
*             CE,
*             4,
*            {
*                { 1,   13, 20, BOTH, FALSE},    // 2.4GHz, ch 1~13
*                { 36,  4,  17, BOTH, FALSE},    // 5GHz, ch 36~48
*                { 52,  4,  24, BOTH, FALSE},    // 5GHz, ch 52~64
*                { 149, 4,  30, BOTH, FALSE},    // 5GHz, ch 149~161
*            }
*        },
*
*        {    // Cyprus
*            "CY",
*             CE,
*             4,
*            {
*                { 1,   13, 20, BOTH, FALSE},    // 2.4GHz, ch 1~13
*                { 36,  4,  23, IDOR, FALSE},    // 5GHz, ch 36~48
*                { 52,  4,  24, IDOR, TRUE},     // 5GHz, ch 52~64
*                { 100, 11, 30, BOTH, TRUE},     // 5GHz, ch 100~140
*            }
*        },
*
*        {    // Czech Republic
*            "CZ",
*             CE,
*             3,
*            {
*                { 1,   13, 20, BOTH, FALSE},    // 2.4GHz, ch 1~13
*                { 36,  4,  23, IDOR, FALSE},    // 5GHz, ch 36~48
*                { 52,  4,  23, IDOR, TRUE},     // 5GHz, ch 52~64
*            }
*        },
*
*        {    // Denmark
*            "DK",
*             CE,
*             4,
*            {
*                { 1,   13, 20, BOTH, FALSE},    // 2.4GHz, ch 1~13
*                { 36,  4,  23, IDOR, FALSE},    // 5GHz, ch 36~48
*                { 52,  4,  23, IDOR, TRUE},     // 5GHz, ch 52~64
*                { 100, 11, 30, BOTH, TRUE},     // 5GHz, ch 100~140
*            }
*        },
*
*        {    // Dominican Republic
*            "DO",
*             CE,
*             2,
*            {
*                { 1,   0,  20, BOTH, FALSE},    // 2.4GHz, ch 0
*                { 149, 4,  20, BOTH, FALSE},    // 5GHz, ch 149~161
*            }
*        },
*
*        {    // Equador
*            "EC",
*             CE,
*             2,
*            {
*                { 1,   13,  20, BOTH, FALSE},    // 2.4GHz, ch 1~13
*                { 100, 11,  27, BOTH, FALSE},    // 5GHz, ch 100~140
*            }
*        },
*
*        {    // El Salvador
*            "SV",
*             CE,
*             4,
*            {
*                { 1,   13,  20, BOTH, FALSE},    // 2.4GHz, ch 1~13
*                { 36,  4,   23, IDOR, FALSE},    // 5GHz, ch 36~48
*                { 52,  4,   30, BOTH, TRUE},     // 5GHz, ch 52~64
*                { 149, 4,   36, BOTH, TRUE},     // 5GHz, ch 149~165
*            }
*        },
*
*        {    // Finland
*            "FI",
*             CE,
*             4,
*            {
*                { 1,   13,  20, BOTH, FALSE},    // 2.4GHz, ch 1~13
*                { 36,  4,   23, IDOR, FALSE},    // 5GHz, ch 36~48
*                { 52,  4,   23, IDOR, TRUE},     // 5GHz, ch 52~64
*                { 100, 11,  30, BOTH, TRUE},     // 5GHz, ch 100~140
*            }
*        },
*
*        {    // France
*            "FR",
*             CE,
*             3,
*            {
*                { 1,   13,  20, BOTH, FALSE},    // 2.4GHz, ch 1~13
*                { 36,  4,   23, IDOR, FALSE},    // 5GHz, ch 36~48
*                { 52,  4,   23, IDOR, TRUE},     // 5GHz, ch 52~64
*            }
*        },
*
*        {    // Germany
*            "DE",
*             CE,
*             4,
*            {
*                { 1,   13,  20, BOTH, FALSE},    // 2.4GHz, ch 1~13
*                { 36,  4,   23, IDOR, FALSE},    // 5GHz, ch 36~48
*                { 52,  4,   23, IDOR, TRUE},     // 5GHz, ch 52~64
*                { 100, 11,  30, BOTH, TRUE},     // 5GHz, ch 100~140
*            }
*        },
*
*        {    // Greece
*            "GR",
*             CE,
*             4,
*            {
*                { 1,   13,  20, BOTH, FALSE},    // 2.4GHz, ch 1~13
*                { 36,  4,   23, IDOR, FALSE},    // 5GHz, ch 36~48
*                { 52,  4,   23, IDOR, TRUE},     // 5GHz, ch 52~64
*                { 100, 11,  30, ODOR, TRUE},     // 5GHz, ch 100~140
*            }
*        },
*
*        {    // Guam
*            "GU",
*             CE,
*             5,
*            {
*                { 1,   11,  20, BOTH, FALSE},    // 2.4GHz, ch 1~11
*                { 36,  4,   17, BOTH, FALSE},    // 5GHz, ch 36~48
*                { 52,  4,   24, BOTH, FALSE},    // 5GHz, ch 52~64
*                { 100, 11,  30, BOTH, FALSE},    // 5GHz, ch 100~140
*                { 149,  5,  30, BOTH, FALSE},    // 5GHz, ch 149~165
*            }
*        },
*
*        {    // Guatemala
*            "GT",
*             CE,
*             4,
*            {
*                { 1,   13,  20, BOTH, FALSE},    // 2.4GHz, ch 1~13
*                { 36,  4,   17, BOTH, FALSE},    // 5GHz, ch 36~48
*                { 52,  4,   24, BOTH, FALSE},    // 5GHz, ch 52~64
*                { 149,  4,  30, BOTH, FALSE},    // 5GHz, ch 149~161
*            }
*        },
*
*        {    // Haiti
*            "HT",
*             CE,
*             4,
*            {
*                { 1,   13,  20, BOTH, FALSE},    // 2.4GHz, ch 1~13
*                { 36,  4,   17, BOTH, FALSE},    // 5GHz, ch 36~48
*                { 52,  4,   24, BOTH, FALSE},    // 5GHz, ch 52~64
*                { 149,  4,  30, BOTH, FALSE},    // 5GHz, ch 149~161
*            }
*        },
*
*        {    // Honduras
*            "HN",
*             CE,
*             2,
*            {
*                { 1,   13,  20, BOTH, FALSE},    // 2.4GHz, ch 1~13
*                { 149,  4,  27, BOTH, FALSE},    // 5GHz, ch 149~161
*            }
*        },
*
*        {    // Hong Kong
*            "HK",
*             CE,
*             4,
*            {
*                { 1,   13,  20, BOTH, FALSE},    // 2.4GHz, ch 1~13
*                { 36,   4,  23, IDOR, FALSE},    // 5GHz, ch 36~48
*                { 52,   4,  23, IDOR, FALSE},    // 5GHz, ch 52~64
*                { 149,  4,  30, BOTH, FALSE},    // 5GHz, ch 149~161
*            }
*        },
*
*        {    // Hungary
*            "HU",
*             CE,
*             3,
*            {
*                { 1,   13,  20, BOTH, FALSE},    // 2.4GHz, ch 1~13
*                { 36,   4,  23, IDOR, FALSE},    // 5GHz, ch 36~48
*                { 52,   4,  23, IDOR, TRUE},     // 5GHz, ch 52~64
*            }
*        },
*
*        {    // Iceland
*            "IS",
*             CE,
*             4,
*            {
*                { 1,   13,  20, BOTH, FALSE},    // 2.4GHz, ch 1~13
*                { 36,   4,  23, IDOR, FALSE},    // 5GHz, ch 36~48
*                { 52,   4,  23, IDOR, TRUE},     // 5GHz, ch 52~64
*                { 100, 11,  30, BOTH, TRUE},     // 5GHz, ch 100~140
*            }
*        },
*
*        {    // India
*            "IN",
*             CE,
*             2,
*            {
*                { 1,   13,  20, BOTH, FALSE},     // 2.4GHz, ch 1~13
*                { 149,     4,  24, IDOR, FALSE},  // 5GHz, ch 149~161
*            }
*        },
*
*        {    // Indonesia
*            "ID",
*             CE,
*             2,
*            {
*                { 1,   13,  20, BOTH, FALSE},     // 2.4GHz, ch 1~13
*                { 149,     4,  27, BOTH, FALSE},  // 5GHz, ch 149~161
*            }
*        },
*
*        {    // Indonesia
*            "ID",
*             CE,
*             2,
*            {
*                { 1,   13,  20, BOTH, FALSE},       // 2.4GHz, ch 1~13
*                { 149,     4,  27, BOTH, FALSE},    // 5GHz, ch 149~161
*            }
*        },
*
*        {    // Ireland
*            "IE",
*             CE,
*             4,
*            {
*                { 1,   13,  20, BOTH, FALSE},      // 2.4GHz, ch 1~13
*                { 36,     4,  23, IDOR, FALSE},    // 5GHz, ch 36~48
*                { 52,     4,  23, IDOR, TRUE},     // 5GHz, ch 52~64
*                { 100, 11,  30, ODOR, TRUE},       // 5GHz, ch 100~140
*            }
*        },
*
*        {    // Israel
*            "IL",
*             CE,
*             3,
*            {
*                { 1,    3,  20, IDOR, FALSE},     // 2.4GHz, ch 1~3
*                { 4,     6,  20, BOTH, FALSE},    // 2.4GHz, ch 4~9
*                { 10,     4,  20, IDOR, FALSE},   // 2.4GHz, ch 10~13
*            }
*        },
*
*        {    // Italy
*            "IT",
*             CE,
*             4,
*            {
*                { 1,   13,  20, BOTH, FALSE},     // 2.4GHz, ch 1~13
*                { 36,     4,  23, IDOR, FALSE},   // 5GHz, ch 36~48
*                { 52,     4,  23, IDOR, TRUE},    // 5GHz, ch 52~64
*                { 100, 11,  30, ODOR, TRUE},      // 5GHz, ch 100~140
*            }
*        },
*
*        {    // Japan
*            "JP",
*             JAP,
*             2,
*            {
*                { 1,   14,  20, BOTH, FALSE},     // 2.4GHz, ch 1~14
*                { 34,     4,  23, IDOR, FALSE},   // 5GHz, ch 34~46
*            }
*        },
*
*        {    // Jordan
*            "JO",
*             CE,
*             3,
*            {
*                { 1,   13,  20, IDOR, FALSE},      // 2.4GHz, ch 1~13
*                { 36,     4,  23, IDOR, FALSE},    // 5GHz, ch 36~48
*                { 149,     4,  23, IDOR, FALSE},   // 5GHz, ch 149~161
*            }
*        },
*
*        {    // Latvia
*            "LV",
*             CE,
*             4,
*            {
*                { 1,   13,  20, BOTH, FALSE},      // 2.4GHz, ch 1~13
*                { 36,     4,  23, IDOR, FALSE},    // 5GHz, ch 36~48
*                { 52,     4,  23, IDOR, TRUE},     // 5GHz, ch 52~64
*                { 100, 11,  30, BOTH, TRUE},       // 5GHz, ch 100~140
*            }
*        },
*
*        {    // Liechtenstein
*            "LI",
*             CE,
*             3,
*            {
*                { 1,   13,  20, BOTH, FALSE},     // 2.4GHz, ch 1~13
*                { 52,     4,  23, IDOR, TRUE},    // 5GHz, ch 52~64
*                { 100, 11,  30, BOTH, TRUE},      // 5GHz, ch 100~140
*            }
*        },
*
*        {    // Lithuania
*            "LT",
*             CE,
*             4,
*            {
*                { 1,   13,  20, BOTH, FALSE},      // 2.4GHz, ch 1~13
*                { 36,     4,  23, IDOR, FALSE},    // 5GHz, ch 36~48
*                { 52,     4,  23, IDOR, TRUE},     // 5GHz, ch 52~64
*                { 100, 11,  30, BOTH, TRUE},       // 5GHz, ch 100~140
*            }
*        },
*
*        {    // Luxemburg
*            "LU",
*             CE,
*             4,
*            {
*                { 1,   13,  20, BOTH, FALSE},       // 2.4GHz, ch 1~13
*                { 36,     4,  23, IDOR, FALSE},     // 5GHz, ch 36~48
*                { 52,     4,  23, IDOR, TRUE},      // 5GHz, ch 52~64
*                { 100, 11,  30, BOTH, TRUE},        // 5GHz, ch 100~140
*            }
*        },
*
*        {    // Malaysia
*            "MY",
*             CE,
*             3,
*            {
*                { 36,     4,  23, BOTH, FALSE},    // 5GHz, ch 36~48
*                { 52,     4,  23, BOTH, FALSE},    // 5GHz, ch 52~64
*                { 149,  5,  20, BOTH, FALSE},      // 5GHz, ch 149~165
*            }
*        },
*
*        {    // Malta
*            "MT",
*             CE,
*             4,
*            {
*                { 1,   13,  20, BOTH, FALSE},      // 2.4GHz, ch 1~13
*                { 36,     4,  23, IDOR, FALSE},    // 5GHz, ch 36~48
*                { 52,     4,  23, IDOR, TRUE},     // 5GHz, ch 52~64
*                { 100, 11,  30, BOTH, TRUE},       // 5GHz, ch 100~140
*            }
*        },
*
*        {    // Marocco
*            "MA",
*             CE,
*             2,
*            {
*                { 1,   13,  20, BOTH, FALSE},      // 2.4GHz, ch 1~13
*                { 36,     4,  24, IDOR, FALSE},    // 5GHz, ch 36~48
*            }
*        },
*
*        {    // Mexico
*            "MX",
*             CE,
*             4,
*            {
*                { 1,   13,  20, BOTH, FALSE},      // 2.4GHz, ch 1~13
*                { 36,     4,  23, BOTH, FALSE},    // 5GHz, ch 36~48
*                { 52,     4,  24, BOTH, FALSE},    // 5GHz, ch 52~64
*                { 149,  5,  30, IDOR, FALSE},      // 5GHz, ch 149~165
*            }
*        },
*
*        {    // Netherlands
*            "NL",
*             CE,
*             4,
*            {
*                { 1,   13,  20, BOTH, FALSE},      // 2.4GHz, ch 1~13
*                { 36,     4,  23, IDOR, FALSE},    // 5GHz, ch 36~48
*                { 52,     4,  24, IDOR, TRUE},     // 5GHz, ch 52~64
*                { 100, 11,  30, BOTH, TRUE},       // 5GHz, ch 100~140
*            }
*        },
*
*        {    // New Zealand
*            "NZ",
*             CE,
*             4,
*            {
*                { 1,   13,  20, BOTH, FALSE},      // 2.4GHz, ch 1~13
*                { 36,     4,  24, BOTH, FALSE},    // 5GHz, ch 36~48
*                { 52,     4,  24, BOTH, FALSE},    // 5GHz, ch 52~64
*                { 149,  4,  30, BOTH, FALSE},      // 5GHz, ch 149~161
*            }
*        },
*
*        {    // Norway
*            "NO",
*             CE,
*             4,
*            {
*                { 1,   13,  20, BOTH, FALSE},      // 2.4GHz, ch 1~13
*                { 36,     4,  24, IDOR, FALSE},    // 5GHz, ch 36~48
*                { 52,     4,  24, IDOR, TRUE},     // 5GHz, ch 52~64
*                { 100, 11,  30, BOTH, TRUE},       // 5GHz, ch 149~161
*            }
*        },
*
*        {    // Peru
*            "PE",
*             CE,
*             2,
*            {
*                { 1,   13,  20, BOTH, FALSE},    // 2.4GHz, ch 1~13
*                { 149,  4,  27, BOTH, FALSE},    // 5GHz, ch 149~161
*            }
*        },
*
*        {    // Portugal
*            "PT",
*             CE,
*             4,
*            {
*                { 1,   13,  20, BOTH, FALSE},    // 2.4GHz, ch 1~13
*                { 36,   4,  23, IDOR, FALSE},    // 5GHz, ch 36~48
*                { 52,   4,  23, IDOR, TRUE},     // 5GHz, ch 52~64
*                { 100, 11,  30, BOTH, TRUE},     // 5GHz, ch 100~140
*            }
*        },
*
*        {    // Poland
*            "PL",
*             CE,
*             4,
*            {
*                { 1,   13,  20, BOTH, FALSE},    // 2.4GHz, ch 1~13
*                { 36,   4,  23, IDOR, FALSE},    // 5GHz, ch 36~48
*                { 52,   4,  23, IDOR, TRUE},     // 5GHz, ch 52~64
*                { 100, 11,  30, BOTH, TRUE},     // 5GHz, ch 100~140
*            }
*        },
*
*        {    // Romania
*            "RO",
*             CE,
*             4,
*            {
*                { 1,   13,  20, BOTH, FALSE},    // 2.4GHz, ch 1~13
*                { 36,   4,  23, IDOR, FALSE},    // 5GHz, ch 36~48
*                { 52,   4,  23, IDOR, TRUE},     // 5GHz, ch 52~64
*                { 100, 11,  30, BOTH, TRUE},     // 5GHz, ch 100~140
*            }
*        },
*
*        {    // Russia
*            "RU",
*             CE,
*             2,
*            {
*                { 1,   13,  20, BOTH, FALSE},    // 2.4GHz, ch 1~13
*                { 149,  4,  20, IDOR, FALSE},    // 5GHz, ch 149~161
*            }
*        },
*
*        {    // Saudi Arabia
*            "SA",
*             CE,
*             4,
*            {
*                { 1,   13,  20, BOTH, FALSE},    // 2.4GHz, ch 1~13
*                { 36,   4,  23, BOTH, FALSE},    // 5GHz, ch 36~48
*                { 52,   4,  23, BOTH, FALSE},    // 5GHz, ch 52~64
*                { 149,  4,  23, BOTH, FALSE},    // 5GHz, ch 149~161
*            }
*        },
*
*        {    // Serbia and Montenegro
*            "CS",
*             CE,
*             1,
*            {
*                { 1,   13,  20, BOTH, FALSE},    // 2.4GHz, ch 1~13
*            }
*        },
*
*        {    // Singapore
*            "SG",
*             CE,
*             4,
*            {
*                { 1,   13,  20, BOTH, FALSE},    // 2.4GHz, ch 1~13
*                { 36,   4,  23, BOTH, FALSE},    // 5GHz, ch 36~48
*                { 52,   4,  23, BOTH, FALSE},    // 5GHz, ch 52~64
*                { 149,  4,  20, BOTH, FALSE},    // 5GHz, ch 149~161
*            }
*        },
*
*        {    // Slovakia
*            "SK",
*             CE,
*             4,
*            {
*                { 1,   13,  20, BOTH, FALSE},    // 2.4GHz, ch 1~13
*                { 36,   4,  23, IDOR, FALSE},    // 5GHz, ch 36~48
*                { 52,   4,  23, IDOR, TRUE},     // 5GHz, ch 52~64
*                { 100, 11,  30, BOTH, TRUE},     // 5GHz, ch 100~140
*            }
*        },
*
*        {    // Slovenia
*            "SI",
*             CE,
*             3,
*            {
*                { 1,   13,  20, BOTH, FALSE},    // 2.4GHz, ch 1~13
*                { 36,   4,  23, IDOR, FALSE},    // 5GHz, ch 36~48
*                { 52,   4,  23, IDOR, TRUE},     // 5GHz, ch 52~64
*            }
*        },
*
*        {    // South Africa
*            "ZA",
*             CE,
*             5,
*            {
*                { 1,   13,  20, BOTH, FALSE},    // 2.4GHz, ch 1~13
*                { 36,   4,  23, BOTH, FALSE},    // 5GHz, ch 36~48
*                { 52,   4,  23, IDOR, FALSE},    // 5GHz, ch 52~64
*                { 100, 11,  30, BOTH, TRUE},     // 5GHz, ch 100~140
*                { 149,  4,  30, BOTH, FALSE},    // 5GHz, ch 149~161
*            }
*        },
*
*        {    // South Korea
*            "KR",
*             CE,
*             5,
*            {
*                { 1,   13,  20, BOTH, FALSE},    // 2.4GHz, ch 1~13
*                { 36,   4,  20, BOTH, FALSE},    // 5GHz, ch 36~48
*                { 52,   4,  20, BOTH, FALSE},    // 5GHz, ch 52~64
*                { 100,  8,  20, BOTH, FALSE},    // 5GHz, ch 100~128
*                { 149,  4,  20, BOTH, FALSE},    // 5GHz, ch 149~161
*            }
*        },
*
*        {    // Spain
*            "ES",
*             CE,
*             4,
*            {
*                { 1,   13,  20, BOTH, FALSE},    // 2.4GHz, ch 1~13
*                { 36,   4,  17, IDOR, FALSE},    // 5GHz, ch 36~48
*                { 52,   4,  23, IDOR, TRUE},     // 5GHz, ch 52~64
*                { 100, 11,  30, BOTH, TRUE},     // 5GHz, ch 100~140
*            }
*        },
*
*        {    // Sweden
*            "SE",
*             CE,
*             4,
*            {
*                { 1,   13,  20, BOTH, FALSE},    // 2.4GHz, ch 1~13
*                { 36,   4,  23, IDOR, FALSE},    // 5GHz, ch 36~48
*                { 52,   4,  23, IDOR, TRUE},     // 5GHz, ch 52~64
*                { 100, 11,  30, BOTH, TRUE},     // 5GHz, ch 100~140
*            }
*        },
*
*        {    // Switzerland
*            "CH",
*             CE,
*             3,
*            {
*                { 1,   13,  20, BOTH, FALSE},    // 2.4GHz, ch 1~13
*                { 36,   4,  23, IDOR, TRUE},     // 5GHz, ch 36~48
*                { 52,   4,  23, IDOR, TRUE},     // 5GHz, ch 52~64
*            }
*        },
*
*        {    // Taiwan
*            "TW",
*             CE,
*             2,
*            {
*                { 1,   11,  30, BOTH, FALSE},    // 2.4GHz, ch 1~11
*                { 52,   4,  23, IDOR, FALSE},    // 5GHz, ch 52~64
*            }
*        },
*
*        {    // Turkey
*            "TR",
*             CE,
*             3,
*            {
*                { 1,   13,  20, BOTH, FALSE},    // 2.4GHz, ch 1~11
*                { 36,   4,  23, BOTH, FALSE},    // 5GHz, ch 36~48
*                { 52,   4,  23, BOTH, FALSE},    // 5GHz, ch 52~64
*            }
*        },
*
*        {    // UK
*            "GB",
*             CE,
*             4,
*            {
*                { 1,   13,  20, BOTH, FALSE},    // 2.4GHz, ch 1~11
*                { 36,   4,  23, IDOR, FALSE},    // 5GHz, ch 52~64
*                { 52,   4,  23, IDOR, TRUE},     // 5GHz, ch 52~64
*                { 100, 11,  30, BOTH, TRUE},     // 5GHz, ch 100~140
*            }
*        },
*
*        {    // Ukraine
*            "UA",
*             CE,
*             1,
*            {
*                { 1,   13,  20, BOTH, FALSE},    // 2.4GHz, ch 1~11
*            }
*        },
*
*        {    // United Arab Emirates
*            "AE",
*             CE,
*             1,
*            {
*                { 1,   13,  20, BOTH, FALSE},    // 2.4GHz, ch 1~11
*            }
*        },
*
*        {    // United States
*            "US",
*             CE,
*             5,
*            {
*                { 1,   11,  30, BOTH, FALSE},    // 2.4GHz, ch 1~11
*                { 36,   4,  17, IDOR, FALSE},    // 5GHz, ch 52~64
*                { 52,   4,  24, BOTH, TRUE},     // 5GHz, ch 52~64
*                { 100, 11,  30, BOTH, TRUE},     // 5GHz, ch 100~140
*                { 149,  5,  30, BOTH, FALSE},    // 5GHz, ch 149~165
*            }
*        },
*
*        {    // Venezuela
*            "VE",
*             CE,
*             2,
*            {
*                { 1,   13,  20, BOTH, FALSE},    // 2.4GHz, ch 1~11
*                { 149,  4,  27, BOTH, FALSE},    // 5GHz, ch 149~161
*            }
*        },
*
*        {    // Default
*            "",
*             CE,
*             5,
*            {
*                { 1,   11,  20, BOTH, FALSE},    // 2.4GHz, ch 1~11
*                { 36,   4,  20, BOTH, FALSE},    // 5GHz, ch 52~64
*                { 52,   4,  20, BOTH, FALSE},    // 5GHz, ch 52~64
*                { 100, 11,  20, BOTH, FALSE},    // 5GHz, ch 100~140
*                { 149,  5,  20, BOTH, FALSE},    // 5GHz, ch 149~165
*            }
*        },
*  };
*  @endcode
*
* @return  >=0 means success, <0 means fail
*
*/
int32_t wifi_config_set_country_code(wifi_country_code_t *wifi_country_code);

/**
* @brief This function gets the country code string that the Wi-Fi driver uses.
*
* @param[out] country_code contains the country code string, which is already set by #wifi_config_set_country_code(), upon the return of this function call.
*
* @return  >=0 the operation completed successfully, <0 the operation failed.
*
* @note The Wi-Fi driver provides the country code already defined in #wifi_country_code_t.
*/
int32_t wifi_config_get_country_code(wifi_country_code_t *wifi_country_code);

/**
* @deprecated This function is deprecated! This function gets the country region that the Wi-Fi driver uses for a specific wireless port.
* Various world regions have allocated different amounts of frequency space
* around 2.4GHz for Wi-Fi communication and some of them have fewer or more channels
* available than others. The region setting ensures that your router enables
* using the Wi-Fi channels valid only in user-specific location.
*
* @param[in] band indicates the Wi-Fi bandwidth that the function will operate on.
*
* Value                         |Definition                                                              |
* ------------------------------|------------------------------------------------------------------------|
* \b #WIFI_BAND_2_4_G           | 2.4GHz|
* \b #WIFI_BAND_5_G             | 5GHz|
*
* @param[out]  region indicates different channels in 2.4GHz and 5GHz.
* @param  region: (2.4GHz) the channel list for different regions in 2.4GHz.
*
* Value                         |Definition                                                              |
* ------------------------------|------------------------------------------------------------------------|
* \b 0                          | CH1-11 active scan|
* \b 1                          | CH1-13 active scan|
* \b 2                          | CH10-11 active scan|
* \b 3                          | CH10-13 active scan|
* \b 4                          | CH14 active scan|
* \b 5                          | CH1-14 all active scan|
* \b 6                          | CH3-9 active scan|
* \b 7                          | CH5-13 active scan|
* \b 31                         | CH1-11 active scan, CH12-14 passive scan|
* \b 32                         | CH1-11 active scan, CH12-13 passive scan|
* \b 33                         | CH1-14 all active scan|
* @param  region: (5GHz)  the channel list for different regions in 5GHz.
*
* Value                         |Definition                                                              |
* ------------------------------|------------------------------------------------------------------------|
* \b 0                          | CH36-64 active scan, CH149-165 active scan|
* \b 1                          | CH36-64 active scan, CH100-140 active scan|
* \b 2                          | CH36-64 active scan|
* \b 3                          | CH52-64 active scan, CH149-161 active scan|
* \b 4                          | CH149-165 active scan|
* \b 5                          | CH149-161 active scan|
* \b 6                          | CH36-48 active scan|
* \b 7                          | CH36-64 active scan, CH100-140 active scan, CH149-165 active scan|
* \b 8                          | CH52-64 active scan|
* \b 9                          | CH36-64 active scan, CH100-116 active scan, CH132-140 active scan, CH149-165 active scan|
* \b 10                         | CH36-48 active scan, CH149-165 active scan|
* \b 11                         | CH36-64 active scan, CH100-120 active scan, CH149-161 active scan|
* \b 12                         | CH36-64 active scan, CH100-140 active scan|
* \b 13                         | CH52-64 active scan, CH100-140 active scan, CH149-161 active scan|
* \b 14                         | CH36-64 active scan, CH100-116 active scan, CH136-140 active scan, CH149-165 active scan|
* \b 15                         | CH149-173 active scan|
* \b 16                         | CH52-64 active scan, CH149-165 active scan|
* \b 17                         | CH36-48 active scan, CH149-161 active scan|
* \b 18                         | CH36-64 active scan, CH100-116 active scan, CH132-140 active scan|
* \b 19                         | CH56-64 active scan, CH100-140 active scan, CH149-161 active scan|
* \b 20                         | CH36-64 active scan, CH100-124 active scan, CH149-161 active scan|
* \b 21                         | CH36-64 active scan, CH100-140 active scan, CH149-161 active scan|
* \b 22                         | CH36-64 active scan|
* \b 30                         | CH36-48 active scan, CH52-64 passive scan, CH100-140 passive scan, CH149-165 active scan|
* \b 31                         | CH52-64 passive scan, CH100-140 passive scan, CH149-165 active scan|
* \b 32                         | CH36-48 active scan, CH52-64 passive scan, CH100-140 passive scan, CH149-161 active scan|
* \b 33                         | CH36-48 active scan, CH52-64 passive scan, CH100-140 passive scan|
* \b 34                         | CH36-48 active scan, CH52-64 passive scan, CH149-165 active scan|
* \b 35                         | CH36-48 active scan, CH52-64 passive scan|
* \b 36                         | CH36-48 active scan, CH100-140 passive scan, CH149-165 active scan|
* \b 37                         | CH36-48 active scan, CH52-64 passive scan, CH149-165 active scan, CH173 active scan|
*
* @return  >=0 the operation completed successfully, <0 the operation failed.
*
* @note Migration method: please use #wifi_config_get_country_code instead, it returns the current effected channel list.
*/
int32_t wifi_config_get_country_region(uint8_t band, uint8_t  *region);

/**
* @deprecated This function is deprecated! This function sets the country region that the Wi-Fi driver uses for a specific wireless port.
* Various world regions have allocated different amounts of frequency space
* around 2.4GHz for Wi-Fi and some of them have fewer or more channels
* available than others. The region setting ensures that the router enables
* using the Wi-Fi channels valid only in user-specific location.
*
* @param[in] band indicates the Wi-Fi bandwidth to operate on.
*
* Value                         |Definition                                                              |
* ------------------------------|------------------------------------------------------------------------|
* \b #WIFI_BAND_2_4_G           | 2.4GHz|
* \b #WIFI_BAND_5_G             | 5GHz|
*
* @param[in] region indicates different channels in 2.4GHz and 5GHz.
* @param  region: (2.4GHz)  the channel list for different regions in 2.4GHz.
*
* Value                         |Definition                                                              |
* ------------------------------|------------------------------------------------------------------------|
* \b 0                          | CH1-11 active scan|
* \b 1                          | CH1-13 active scan|
* \b 2                          | CH10-11 active scan|
* \b 3                          | CH10-13 active scan|
* \b 4                          | CH14 active scan|
* \b 5                          | CH1-14 all active scan|
* \b 6                          | CH3-9 active scan|
* \b 7                          | CH5-13 active scan|
* \b 31                         | CH1-11 active scan, CH12-14 passive scan|
* \b 32                         | CH1-11 active scan, CH12-13 passive scan|
* \b 33                         | CH1-14 all active scan|
* @param  region: (5GHz)  the channel list for different regions in 5GHz.
*
* Value                         |Definition                                                              |
* ------------------------------|------------------------------------------------------------------------|
* \b 0                          | CH36-64 active scan, CH149-165 active scan|
* \b 1                          | CH36-64 active scan, CH100-140 active scan|
* \b 2                          | CH36-64 active scan|
* \b 3                          | CH52-64 active scan, CH149-161 active scan|
* \b 4                          | CH149-165 active scan|
* \b 5                          | CH149-161 active scan|
* \b 6                          | CH36-48 active scan|
* \b 7                          | CH36-64 active scan, CH100-140 active scan, CH149-165 active scan|
* \b 8                          | CH52-64 active scan|
* \b 9                          | CH36-64 active scan, CH100-116 active scan, CH132-140 active scan, CH149-165 active scan|
* \b 10                         | CH36-48 active scan, CH149-165 active scan|
* \b 11                         | CH36-64 active scan, CH100-120 active scan, CH149-161 active scan|
* \b 12                         | CH36-64 active scan, CH100-140 active scan|
* \b 13                         | CH52-64 active scan, CH100-140 active scan, CH149-161 active scan|
* \b 14                         | CH36-64 active scan, CH100-116 active scan, CH136-140 active scan, CH149-165 active scan|
* \b 15                         | CH149-173 active scan|
* \b 16                         | CH52-64 active scan, CH149-165 active scan|
* \b 17                         | CH36-48 active scan, CH149-161 active scan|
* \b 18                         | CH36-64 active scan, CH100-116 active scan, CH132-140 active scan|
* \b 19                         | CH56-64 active scan, CH100-140 active scan, CH149-161 active scan|
* \b 20                         | CH36-64 active scan, CH100-124 active scan, CH149-161 active scan|
* \b 21                         | CH36-64 active scan, CH100-140 active scan, CH149-161 active scan|
* \b 22                         | CH36-64 active scan|
* \b 30                         | CH36-48 active scan, CH52-64 passive scan, CH100-140 passive scan, CH149-165 active scan|
* \b 31                         | CH52-64 passive scan, CH100-140 passive scan, CH149-165 active scan|
* \b 32                         | CH36-48 active scan, CH52-64 passive scan, CH100-140 passive scan, CH149-161 active scan|
* \b 33                         | CH36-48 active scan, CH52-64 passive scan, CH100-140 passive scan|
* \b 34                         | CH36-48 active scan, CH52-64 passive scan, CH149-165 active scan|
* \b 35                         | CH36-48 active scan, CH52-64 passive scan|
* \b 36                         | CH36-48 active scan, CH100-140 passive scan, CH149-165 active scan|
* \b 37                         | CH36-48 active scan, CH52-64 passive scan, CH149-165 active scan, CH173 active scan|
*
* @return  >=0 the operation completed successfully, <0 the operation failed.
*
* @note Migration method: please use #wifi_config_set_country_code instead to set the channel list.
* For example, to set the country region 0(2.4G) and 0(5G),
* initialize the struture #wifi_country_code_t as num_of_channel_table = 3, channel_table[0] = {1,11,0,0,0}, channel_table[1] = {36,8,0,0,0}, channel_table[2] = {149,5,0,0,0}.
* Then call #wifi_config_set_country_code to set the channel list. dfs_req = 0 in channel_table indicates an active scan and dfs_req = 1 is for passive scan.
*
*/
int32_t wifi_config_set_country_region(uint8_t band, uint8_t region);

/**
* @brief This function gets the DTIM interval used in the Wi-Fi driver in the SoftAP mode.
* Depending on the value set in the SoftAP mode, the AP will buffer
* broadcast and multicast data and let clients know when to wake up to
* receive those data.
*
* @param[out]  interval beacon intervals between 1 to 255.
* For more information, please refer to <a herf="https://en.wikipedia.org/wiki/Delivery_traffic_indication_message">introduction to DTIM in Wikipedia</a>.|
*
* @return  >=0 the operation completed successfully, <0 the operation failed.
*
*/
int32_t wifi_config_get_dtim_interval(uint8_t *interval);

/**
* @brief This function sets the DTIM interval used in the Wi-Fi driver in the SoftAP mode.
* Depending on the value set in the SoftAP mode, the AP will buffer
* broadcast and multicast data and let clients know when to wake up to
* receive those data.
* This operation takes effect immediately.
*
* @param[out]  interval is the number in a range of 1 ~ 255 beacon intervals.
* For more information, please refer to <a herf="https://en.wikipedia.org/wiki/Delivery_traffic_indication_message">introduction to DTIM in Wikipedia</a>.|
*
* @return  >=0 the operation completed successfully, <0 the operation failed.
*/
int32_t wifi_config_set_dtim_interval(uint8_t interval);

/**
* @brief This function gets the beacon listen interval used in the Wi-Fi driver in a station mode.
* Depending on the timing set in the station, the station in power saving mode
* resumes to listen to the beacon and receives
* buffered packets from the AP, if the TIM element in beacon frame indicates the AP has stored packets for specific station.
*
* @param[out]  interval is the beacon's interval in a range from 1 to 255. The beacon's interval is usually 100 times the unit (100*1.024 ms), which means, the listen interval range is around (1~255) * 102.4ms.
*
* @return  >=0 the operation completed successfully, <0 the operation failed.
*
*/
int32_t wifi_config_get_listen_interval(uint8_t *interval);

/**
* @brief This function sets the beacon listen interval used in the Wi-Fi driver in a station mode.
* Depending on the timing set in the Station, the station in power saving mode
* resumes to listen to the beacon and receives
* buffered packets from the AP, if the TIM element in beacon frame indicates the AP has stored packets for specific station.
* This operation takes effect immediately.
*
* @param[in] interval is the beacon's interval in a range from 1 to 255. The beacon's interval is usually 100 times the unit (100*1.024 ms), which means, the listen interval range is around (1~255) * 102.4ms.
*
* @return  >=0 the operation completed successfully, <0 the operation failed.
*/
int32_t wifi_config_set_listen_interval(uint8_t interval);

/**
* @brief This function gets the authentication and encryption modes used in the Wi-Fi driver for a specific wireless port.
*
* @param[in] port indicates the Wi-Fi port that the function will operate on.
*
* Value                         |Definition                                                              |
* ------------------------------|------------------------------------------------------------------------|
* \b #WIFI_PORT_STA             | STA|
* \b #WIFI_PORT_APCLI           | AP Client|
* \b #WIFI_PORT_AP              | AP|
*
* @param[out]  auth_mode is the authentication mode. For details about this parameter, please refer to
* #wifi_auth_mode_t.
* @param[out]  encrypt_type is the encryption mode. For details about this parameter, please refer to
* #wifi_encrypt_type_t.
*
* @return  >=0 the operation completed successfully, <0 the operation failed.
*/
int32_t wifi_config_get_security_mode(uint8_t port, wifi_auth_mode_t *auth_mode, wifi_encrypt_type_t *encrypt_type);

/**
* @brief This function sets the authentication and encryption modes used in the Wi-Fi driver for a specific wireless port.
* This operation only takes effect after #wifi_config_reload_setting() is called.
*
* @param[in] port is the Wi-Fi port that the function will operate on.
*
* Value                         |Definition                                                              |
* ------------------------------|------------------------------------------------------------------------|
* \b #WIFI_PORT_STA             | STA|
* \b #WIFI_PORT_APCLI           | AP Client|
* \b #WIFI_PORT_AP              | AP|
*
* @param[in] auth_mode is the authentication mode. For details about this parameter, please refer to
* #wifi_auth_mode_t.
* @param[in] encrypt_type is the encryption mode. For details about this parameter, please refer to
* #wifi_encrypt_type_t.
*
* @return  >=0 the operation completed successfully, <0 the operation failed.
*
* @note In station mode, the device can connect to AP without setting any security mode.\n
*       If #wifi_config_set_security_mode() is not called, the device will change security mode dynamically according to the target AP.\n
*       If #wifi_config_set_security_mode() is called, the device will set a security mode to connect to the target AP.\n
*       Calling #wifi_config_set_security_mode() is optional in station mode.\n
* To skip the security mode setting, simply do not call #wifi_config_set_security_mode() before #wifi_config_reload_setting().
*/
int32_t wifi_config_set_security_mode(uint8_t port, wifi_auth_mode_t auth_mode, wifi_encrypt_type_t encrypt_type);

/**
* @brief This function gets the password of the WPA-PSK or WPA2-PSK encryption type used in the Wi-Fi driver for a specific wireless port.
*
* @param[in] port is the Wi-Fi port that the function will operate on.
*
* Value                         |Definition                                                              |
* ------------------------------|------------------------------------------------------------------------|
* \b #WIFI_PORT_STA             | STA|
* \b #WIFI_PORT_APCLI           | AP Client|
* \b #WIFI_PORT_AP              | AP|
*
* @param[out]  passphrase is 8 to 63 bytes ASCII or 64 bytes hexadecimal.
* @param[out]  passphrase_length is the length range from 8 to 64.
*
* @return  >=0 the operation completed successfully, <0 the operation failed.
*/
int32_t wifi_config_get_wpa_psk_key(uint8_t port, uint8_t *passphrase, uint8_t *passphrase_length);

/**
* @brief This function sets the password of the WPA-PSK or WPA2-PSK encryption type used in the Wi-Fi driver for a specific wireless port.
* This operation only takes effect after #wifi_config_reload_setting() is called.  If the user called the
* following two APIs in a sequence, #wifi_config_set_pmk(), #wifi_config_set_wpa_psk_key(), only the last
* one will take effect. Once #wifi_config_reload_setting() is called after #wifi_config_set_wpa_psk_key(),
* the user can get the related PMK value by calling #wifi_config_get_pmk(). The PMK value is calculated
* using the PSK data.
*
* @param[in] port indicates the Wi-Fi port that the function will operate on.
*
* Value                         |Definition                                                              |
* ------------------------------|------------------------------------------------------------------------|
* \b #WIFI_PORT_STA             | STA|
* \b #WIFI_PORT_APCLI           | AP Client|
* \b #WIFI_PORT_AP              | AP|
*
* @param[in] passphrase is 8 to 63 bytes ASCII or 64 bytes hexadecimal.
* @param[in] passphrase_length is the length range from 8 to 64.
*
* @return  >=0 the operation completed successfully, <0 the operation failed.
*/
int32_t wifi_config_set_wpa_psk_key(uint8_t port, uint8_t *passphrase, uint8_t passphrase_length);

/**
* @brief This function gets the PMK used in the Wi-Fi driver for a specific wireless port.
* @param[in] port is the Wi-Fi port that the function will operate on.
*
* Value                         |Definition                                                              |
* ------------------------------|------------------------------------------------------------------------|
* \b #WIFI_PORT_STA             | STA|
* \b #WIFI_PORT_APCLI           | AP Client|
* \b #WIFI_PORT_AP              | AP|
*
* @param[out]  pmk is pairwise master key, the shared secret key used in the
* IEEE 802.11 protocol.
*
* @return >=0 the operation completed successfully, <0 the operation failed.
*/
int32_t wifi_config_get_pmk(uint8_t port, uint8_t *pmk);

/**
* @brief This function sets the PMK used in the Wi-Fi driver for a specific wireless port. This operation
* only takes effect after #wifi_config_reload_setting() is called. If the user have called the following
* two APIs,  #wifi_config_set_pmk() or #wifi_config_set_wpa_psk_key(), only the last one will take effect.
* Once #wifi_config_reload_setting() is called after  #wifi_config_set_wpa_psk_key(), the user can get
* the related PMK value by calling #wifi_config_get_pmk(). The PMK value is calculated using the PSK data.
* The PSK should be ignored, if both PMK and PSK are available.
*
* @param[in] port is the Wi-Fi port that the function will operate on.
*
* Value                         |Definition                                                              |
* ------------------------------|------------------------------------------------------------------------|
* \b #WIFI_PORT_STA             | STA|
* \b #WIFI_PORT_APCLI           | AP Client|
* \b #WIFI_PORT_AP              | AP|
*
* @param[out]  pmk is a pointer to the PMK buffer with a length of #WIFI_LENGTH_PMK in bytes.
* PMK is used in WPA-PSK or WPA2-PSK encryption type and the PMK length is #WIFI_LENGTH_PMK in bytes.
*
* @return >=0 the operation completed successfully, <0 the operation failed.
*/
int32_t wifi_config_set_pmk(uint8_t port, uint8_t *pmk);

/**
* @brief This function gets the WEP key group used in the Wi-Fi driver for a specific wireless port.
* @param[in] port indicates the Wi-Fi port that the function will operate on.
*
* Value                         |Definition                                                              |
* ------------------------------|------------------------------------------------------------------------|
* \b #WIFI_PORT_STA             | STA|
* \b #WIFI_PORT_APCLI           | AP Client|
* \b #WIFI_PORT_AP              | AP|
*
* @param[out]  wep_keys is a pointer to the WEP keys. For more details, please refer to #wifi_wep_key_t.
*
* @return >=0 the operation completed successfully, <0 the operation failed.
*/
int32_t wifi_config_get_wep_key(uint8_t port, wifi_wep_key_t *wep_keys);

/**
* @brief This function sets the WEP key group used in the Wi-Fi driver for a specific wireless port.
* This operation only takes effect after #wifi_config_reload_setting() is called.
*
* @param[in] port indicates the Wi-Fi port that the function will operate on.
*
* Value                         |Definition                                                              |
* ------------------------------|------------------------------------------------------------------------|
* \b #WIFI_PORT_STA             | STA|
* \b #WIFI_PORT_APCLI           | AP Client|
* \b #WIFI_PORT_AP              | AP|
*
* @param[in] wep_keys is a pointer to the WEP keys. For more details, please refer to #wifi_wep_key_t.
*
* @return  >=0 the operation completed successfully, <0 the operation failed.
*/
int32_t wifi_config_set_wep_key(uint8_t port, wifi_wep_key_t *wep_keys);

/**
* @brief This function informs the wpa_supplicant to reload the configuration and applies the
* configuration settings of the Wi-Fi Configuration APIs. This function is applied to
* the following APIs: #wifi_config_set_ssid(), #wifi_config_set_security_mode(),
* #wifi_config_set_wpa_psk_key(), #wifi_config_set_wep_key() and  #wifi_config_set_pmk().
* In wireless station mode, the device will use the new configuration and start to scan
* and connect to the target AP router. In wireless AP mode, device will load the new
* SSID and encryption information.
*
* @return  >=0 the operation completed successfully, <0 the operation failed.
*/
int32_t wifi_config_reload_setting(void);

/**
* @brief This function registers a handler to receive 802.11 raw packets from a network processor.
* @brief Once the handler is registered, the network processor does not send the packets
* to the IP stack but delivers to the handler function.
* The raw packet includes received packet information header, 802.11 header and data. Usually, this API
* cooperates with the RX filter set APIs; either #wifi_config_set_rx_filter() or #wifi_config_set_smart_connection_filter()
* , or both of them, to deal with specifically formatted RX packets.
*
* @param[in] wifi_rx_handler is the handler routine. For more information, please refer to #wifi_rx_handler_t.
* There is a limitation to use the handler that is based on the in-band mechanism.
* Calls to any in-band functions, such as Wi-Fi Configuration or Wi-Fi connection API in wifi_rx_handler, are prohibited.
*
* @return >=0 the operation completed successfully, <0 the operation failed.
*
*/
int32_t wifi_config_register_rx_handler(wifi_rx_handler_t wifi_rx_handler);

/**
 * @brief This function unregisters the handler to receive 802.11 raw packets from the network processor.
 * @brief Once the handler is unregistered, the network processor will send the packets to the IP stack.
 *
 * @return >=0 the operation completed successfully, <0 the operation failed.
 *
 */
int32_t wifi_config_unregister_rx_handler(void);

/**
* @brief This function gets the Wi-Fi RX filter used in the Wi-Fi driver. The RX filter setting provides
* the packet format to be received, such as broadcast, multicast or unicast frames.
*
* @param[out] flag is defined in #wifi_rx_filter_t.
* @return  >=0 the operation completed successfully, <0 the operation failed.
*/
int32_t wifi_config_get_rx_filter(uint32_t  *flag);

/**
* @brief This function sets the Wi-Fi RX filter used in the Wi-Fi driver. The RX filter setting provides
* the packet format to be received, such as broadcast, multicast,
* unicast frames. This operation takes effect immediately.
*
* @param[in] flag is defined in #wifi_rx_filter_t.
*
* @return >=0 the operation completed successfully, <0 the operation failed.
*
* @note The flag can be generated the way as follows:
*
*    @code
*      #define BIT(x) ((uint32_t)1 << (x))
*      uint32_t filter = 0;
*      wifi_config_get_rx_filter(&filter);
*      filter = filter | BIT(WIFI_RX_FILTER_DROP_FCS_ERR) | BIT(WIFI_RX_FILTER_DROP_VER_NOT_0);
*      wifi_config_set_rx_filter(filter);
*    @endcode
*
*/
int32_t wifi_config_set_rx_filter(uint32_t flag);

/**
* @brief This function gets MediaTek Smart Connection RX Filter used in the Wi-Fi driver.
* When MediaTek Smart Connection filter is enabled, the data frames with
* ToDS=1, A3=multicast address will be received and delivered to the
* RX filter handler,  other frames will be dropped.
* Call #wifi_config_set_rx_filter() to receive other frames.
*
* @param[out]  flag indicates if the smart connection function is enabled or disabled.
*
* Value                         |Definition                                                              |
* ------------------------------|------------------------------------------------------------------------|
* \b 0                          | DISABLE|
* \b 1                          | ENABLE|
*
* @return  >=0 the operation completed successfully, <0 the operation failed.
*/
int32_t wifi_config_get_smart_connection_filter(uint8_t *flag);

/**
* @brief This function sets MediaTek Smart Connection RX Filter used in the Wi-Fi driver.
* This operation takes effect immediately.
* When MediaTek smart connection filter is enabled, MediaTek Smart Connection packets will be delivered to the RX queue directly,
* the other packets will be delivered to the RX filter. For more information, please refer to #wifi_config_set_rx_filter().
*
* @param[in] flag is the parameter set to enable or disable smart connection function.
*
* Value                         |Definition                                                              |
* ------------------------------|------------------------------------------------------------------------|
* \b 0                          | DISABLE|
* \b 1                          | ENABLE|
*
*
* @return  >=0 the operation completed successfully, <0 the operation failed.
*/
int32_t wifi_config_set_smart_connection_filter(uint8_t flag);

/**
* @brief This function gets the radio status of the Wi-Fi driver.
*
* @param[out]  on_off indicates the Wi-Fi radio is on or off.
*
* Value                         |Definition                                                              |
* ------------------------------|------------------------------------------------------------------------|
* \b 0                          | OFF, the Wi-Fi radio is turned off, and Wi-Fi TX/RX is disabled.|
* \b 1                          | ON, the Wi-Fi radio is turned on, and Wi-Fi TX/RX is enabled.|
*
*
* @return  >=0 the operation completed successfully, <0 the operation failed.
*
* @note This API is only supported in the station mode.
*/
int32_t wifi_config_get_radio(uint8_t *on_off);

/**
* @brief This function sets the radio status of the Wi-Fi driver. This operation takes effect immediately.
*
* @param[in] on_off indicates the Wi-Fi radio is on or off.
*
* Value                         |Definition                                                              |
* ------------------------------|------------------------------------------------------------------------|
* \b 0                          | OFF, the Wi-Fi radio is turned off, and Wi-Fi TX/RX is disabled|
* \b 1                          | ON, the Wi-Fi radio is turned on, and Wi-Fi TX/RX is enabled|
*
* @return  >=0 the operation completed successfully, <0 the operation failed.
*
* @note This API is only supported in the station mode.
*/
int32_t wifi_config_set_radio(uint8_t on_off);

/**
* @brief Get wlan statistic info, include Temperature, Tx_Success_Count/Retry_Count/Fail_Count, Rx_Success_Count/Rx_with_CRC/Rx_Drop,
* also more detail please refer to #wifi_statistic_t
*
* @param [IN]wifi_statistic
*
* @return  >=0 means success, <0 means fail
*
* @note only used in STA mode. If it connected to an AP router, PHY rate also will be supported.
*/
int32_t wifi_config_get_statistic(wifi_statistic_t *wifi_statistic);

/**
* @brief This function disconnects the current connection from AP. It takes effect immediately and the device
* switches to idle state. After calling #wifi_config_reload_setting(), the device switches to scan state and tries to connect to an AP router with an existing Wi-Fi
* configuration. This API should be used only in the station mode.
*
* @return  >=0 the operation completed successfully, <0 the operation failed.
*
*/
int32_t wifi_connection_disconnect_ap(void);

/**
* @brief This function disconnects specific station's connection, and takes effect immediately. This
* API is available only in the AP mode.
*
* @param[in] address is station's MAC address.
*
* @return  >=0 the operation completed successfully, <0 the operation failed.
*/
int32_t wifi_connection_disconnect_sta(uint8_t *address);

/**
* @brief This function gets the current STA port's link up or link down connection status.
*
* @param[out]  link_status indicates the current STA port's link up or link down connection status.
*
* Value                         |Definition                                                              |
* ------------------------------|------------------------------------------------------------------------|
* \b 0                          | #WIFI_STATUS_LINK_DISCONNECTED|
* \b 1                          | #WIFI_STATUS_LINK_CONNECTED|
*
* @return  >=0 the operation completed successfully, <0 the operation failed.
*
* @note #WIFI_STATUS_LINK_DISCONNECTED indicates station may be in
*       IDLE/ SCAN/ CONNECTING state.
*/
int32_t wifi_connection_get_link_status(uint8_t *link_status);

/**
* @brief This function initializes the scan table to the driver.
*
* @param[in] ap_list is a pointer of the user buffer where the further scanned AP list will be stored.
* @param[in] max_count is the maximum number of ap_list can be stored.
*
* @return   >=0 the operation completed successfully, <0 the operation failed.
*
* @note #wifi_connection_scan_init() should be called before calling #wifi_connection_start_scan(), and it should be called only once to initialize one scan.\n
*       When the scan is done, the scanned AP list is already stored in parameter ap_list with descending order of the RSSI values.
*/
int32_t wifi_connection_scan_init(wifi_scan_list_item_t *ap_list, uint32_t max_count);

/**
* @brief This function deinitializes the scan table.
*
* @return   >=0 the operation completed successfully, <0 the operation failed.
*
* @note When the scan is finished, #wifi_connection_scan_deinit() should be called to unload the buffer from the driver. After that, the data in the parameter ap_list can be safely processed by user applications, and then another scan can be initialized by calling #wifi_connection_scan_init().
*/
int32_t wifi_connection_scan_deinit(void);

/**
* @brief This function starts Wi-Fi Scanning and recieving the beacon/probe response frames.
*
* @param[in] ssid is the specific SSID that be carried in probe request frame for scan hidden AP. If the SSID is NULL, the SSID field in probe request will be NULL.
* @param[in] ssid_length is the length of the SSID.
* @param[in] bssid is specified to scan to send unicast probe request. If the BSSID is NULL will send broadcast probe request.
* @param[in] scan_mode is the scan mode that can be either a full scan or a partial scan.
*
* Value                         |Definition                                                              |
* ------------------------------|------------------------------------------------------------------------|
* \b 0                          | Full|
* \b 1                          | Partial, it's expected to be used in the SoftAP mode and keep stations online.|
*
* In the partial scan mode, the scanning is performed as follows.
* If the AP is currently on channel 5, it jumps to channels (1, 2),
* and then back to channel 5. After that it scans the channels (3, 4).
* The number of channels it scans each time is specified in the implementation.
* @param[in] scan_option selects scan options based on one of the following: active scan, passive scan or force active scan.
*
* Value                         |Definition                                                              |
* ------------------------------|------------------------------------------------------------------------|
* \b 0                          | Active scan, (Passive in regulatory channels). The channels that will be scanned are decided by the product's country and region. A probe request will be sent to each of these channels.|
* \b 1                          | Passive in all channels. Receives and processes the beacon. No probe request is sent.|
* \b 2                          | Force active (active in all channels). Forced to active scan in all channels. A probe request will be sent to each of these channels.|
*
*
* @return  >=0 the operation completed successfully, <0 the operation failed.
*
* @note If starts different Wi-Fi Scanning at the same time, the previous scanning will be stoped, and the newest scan will take effect.
* @note If the SSID/BSSID is specified (not NULL).
* @note 1. Probe Request carries SSID/BSSID (for Hidden AP).
* @note 2. The station mode supports only the full scan mode.
* @note 3. The AP/repeater/p2p GO mode supports only the partial scan mode.
*/
int32_t wifi_connection_start_scan(uint8_t *ssid, uint8_t ssid_length, uint8_t *bssid, uint8_t scan_mode, uint8_t scan_option);

/**
* @brief This function stops the Wi-Fi Scanning triggered by #wifi_connection_start_scan(). If the device in station mode cannot connect to an AP, it keeps scanning till it connects to the target AP.
*
* @return  >=0 the operation completed successfully, <0 the operation failed.
*
*@note If stops the Wi-Fi Scanning triggered by #wifi_connection_start_scan(), will not got WIFI_EVENT_IOT_SCAN_COMPLETE event of it.
*/
int32_t wifi_connection_stop_scan(void);

/**
* @brief This function parses the beacon or probe request packets.
*
* @param[in] pbuf  is a pointer to indicate the payload of beacon or probe request packets.
* @param[in] len is the length of packet's payload.
* @param[out]  ap_data is the buffer to store AP information received from beacon or probe request packet parse result.
* @return  <0 means parse failed, =0 means success.
*/
int32_t wifi_connection_parse_beacon(uint8_t *pbuf, uint32_t len, wifi_scan_list_item_t *ap_data);

/**
* @brief This function gets the station list associated with the Wi-Fi, only for SoftAP mode.
*
* @param[in,out] number is the user want to stored sta_list number when used as an input and is the number of actual associated stations when used as an output.
* @param[out]  sta_list is defined in #wifi_sta_list_t that includes station information such as RSSI, bandwidth, channel, MAC and TX rate.
*              sta_list buffer should allocate as inupt number * sizeof(wifi_sta_list_t), suggest input number use WIFI_MAX_NUMBER_OF_STA to avoid get stations list incomplete.
*
* @return  >=0 the operation completed successfully, <0 the operation failed.
*
*/
int32_t wifi_connection_get_sta_list(uint8_t *number, wifi_sta_list_t *sta_list);

/**
* @brief This function gets the maximum number of the supported stations in the AP mode or repeater mode.
*
* @param[out]  sta_number returns the maximum number of the supported stations.
*
* @return  >=0 the operation completed successfully, <0 the operation failed.
*
*/
int32_t wifi_connection_get_max_sta_number(uint8_t *sta_number);

/**
* @brief This function gets the RSSI of the connected AP. It's only used for the STA mode and while the station is connected to the AP.
*
* @param[out]  rssi returns the RSSI of the connected AP.
*
* @return  >=0 the operation completed successfully, <0 the operation failed.
*
*
*/
int32_t wifi_connection_get_rssi(int8_t *rssi);

/**
* @brief This function registers the Wi-Fi event handler. Each event can register multiple event handlers with maximum number of 16.
* In the AP/STA mode, the scan stops automatically when the scan cycle is finished
* and SCAN COMPLETE event is triggered;\n
* In the AP mode, once the device is disconnected from the station,  a DISCONNECT event with station's
* MAC address is triggered;\n
* In the AP mode, once the station connects to the device, CONNECT event with station's
* MAC address is triggered;\n
* In the STA mode, once the device disconnects from the AP, DISCONNECT event with BSSID is triggered;\n
* In the STA mode, once the device connect to the AP, CONNECT event with BSSID is triggered.\n
*
* @param[in] event is the event ID. For more details, please refer to #wifi_event_t.
*
* @param[in] handler is the event handler. For more details, please refer to #wifi_event_handler_t.
*
* @return  >=0 the operation completed successfully, <0 the operation failed.
*
* @note  It is not recommended to call any other Wi-Fi APIs or sleep in any form in the event handler.
*/
int32_t wifi_connection_register_event_handler(wifi_event_t event, wifi_event_handler_t handler);

/**
* @deprecated This function is deprecated. This is only for compatibility purposes, so that applications implemented with an earlier version of the API can work with the updated API. Please use #wifi_connection_register_event_handler() to replace this function.
*
* @param[in] event is the event ID. For more details, please refer to #wifi_event_t.
*
* @param[in] notifier is the event handler. For more details, please refer to #wifi_event_handler_t.
*
* @return  >=0 the operation completed successfully, <0 the operation failed.
*/
int32_t wifi_connection_register_event_notifier(uint8_t event, wifi_event_handler_t notifier);

/**
* @brief This function unregisters Wi-Fi event handler. The function #wifi_connection_register_event_handler() registers an event and matches it with the corresponding event handler. For the event behavior, please refer to #wifi_connection_register_event_handler().
*
* @param[in] event is the event ID. For more details, please refer to #wifi_event_t for more details.
*
* @param[in] handler is the event handler. For more details, please refer to #wifi_event_handler_t.
*
* @return  >=0 the operation completed successfully, <0 the operation failed.
*/
int32_t wifi_connection_unregister_event_handler(wifi_event_t event, wifi_event_handler_t handler);

/**
* @deprecated This function is deprecated. This is only for compatibility purposes, so that applications implemented with an earlier version of the API can work with the updated API. Please use #wifi_connection_unregister_event_handler() to replace this function.
*
* @param[in] event is the event ID. For more details, please refer to #wifi_event_t.
*
* @param[in] notifier is the event handler. For more details, please refer to #wifi_event_handler_t.
*
* @return  >=0 the operation completed successfully, <0 the operation failed.
*/
int32_t wifi_connection_unregister_event_notifier(uint8_t event, wifi_event_handler_t notifier);

/**
* @brief This function registers Wi-Fi advanced config overwrite handler. It need config WiFi advanced, this API should be call before #wifi_init()
* @param[in] profile_type is advanced config type. For more details, please refer to #wifi_config_advance_type_t for more details.
*
* @param[in] handler is the overwrite handler. For more details, please refer to #wifi_init_config_handler_t.
*
* @return  >=0 the operation completed successfully, <0 the operation failed.
*/
int32_t wifi_init_register_config_handler(wifi_config_advance_type_t profile_type, wifi_init_config_handler_t handler);


#ifdef MTK_WIFI_PROFILE_ENABLE
/**
* @deprecated This function is deprecated! For more information, please refer to LinkIt for RTOS Wi-Fi Migration Developer's Guide under the doc folder in the SDK package. This function stores the operation mode in a profile in the Flash memory. \n
* The profile is read to initialize the system at boot up.
*
* @param[in] mode indicates the operation mode to be set.
*
* Value                         |Definition                                                              |
* ------------------------------|------------------------------------------------------------------------|
* \b 0x01                       | #WIFI_MODE_STA_ONLY|
* \b 0x02                       | #WIFI_MODE_AP_ONLY|
* \b 0x03                       | #WIFI_MODE_REPEATER|
* \b 0x04                       | #WIFI_MODE_MONITOR|
*
* @return  >=0 the operation completed successfully, <0 the operation failed.
*
* @note This function resets the entire configuration set by previous Wi-Fi Configuration APIs.\n
*/
int32_t wifi_profile_set_opmode(uint8_t mode);

/**
* @deprecated This function is deprecated! For more information, please refer to LinkIt for RTOS Wi-Fi Migration Developer's Guide under the doc folder in the SDK package. This function gets the operation mode from a profile in the Flash memory.
*
* @param[out]  mode indicates operation mode in the Flash memory.
*
* Value                         |Definition                                                              |
* ------------------------------|------------------------------------------------------------------------|
* \b 0x01                       | #WIFI_MODE_STA_ONLY|
* \b 0x02                       | #WIFI_MODE_AP_ONLY|
* \b 0x03                       | #WIFI_MODE_REPEATER|
* \b 0x04                       | #WIFI_MODE_MONITOR|
*
* @return  >=0 the operation completed successfully, <0 the operation failed.
*/
int32_t wifi_profile_get_opmode(uint8_t *mode);

/**
* @deprecated This function is deprecated! For more information, please refer to LinkIt for RTOS Wi-Fi Migration Developer's Guide under the doc folder in the SDK package. This function stores the channel in a profile in the Flash memory. It uses the same profile section for a specific wireless port, such as #WIFI_PORT_STA / #WIFI_PORT_APCLI.
* The profile is read to initialize the system at boot up.
*
* @param[in] port indicates the Wi-Fi port that the function will operate on.
*
* Value                         |Definition                                                              |
* ------------------------------|------------------------------------------------------------------------|
* \b #WIFI_PORT_STA             | STA|
* \b #WIFI_PORT_APCLI           | AP Client|
* \b #WIFI_PORT_AP              | AP|
*
* @param[in] channel    is the channel number used in the Wi-Fi driver for a specific wireless port.
*                       The channel number range is from 1 to 14 for 2.4GHz. The specific channel number is determined
*                       by country region.
*
* @return  >=0 the operation completed successfully, <0 the operation failed.
*/
int32_t wifi_profile_set_channel(uint8_t port, uint8_t channel);

/**
* @deprecated This function is deprecated! For more information, please refer to LinkIt for RTOS Wi-Fi Migration Developer's Guide under the doc folder in the SDK package. This function gets the channel from a profile in the Flash memory. It uses the same profile section for a specific wireless port, such as #WIFI_PORT_STA / #WIFI_PORT_APCLI.
*
* @param[in] port indicates the Wi-Fi port that the function will operate on.
*
* Value                         |Definition                                                              |
* ------------------------------|------------------------------------------------------------------------|
* \b #WIFI_PORT_STA             | STA|
* \b #WIFI_PORT_APCLI           | AP Client|
* \b #WIFI_PORT_AP              | AP|
*
* @param[out]  channel    is the channel number used in the Wi-Fi driver for a specific wireless port.
*                         The channel number range is about from 1 to 14 for 2.4GHz. The specific channel number is determined
*                         by country region.
*
* @return  >=0 the operation completed successfully, <0 the operation failed.
*/
int32_t wifi_profile_get_channel(uint8_t port, uint8_t *channel);

/**
* @deprecated This function is deprecated! For more information, please refer to LinkIt for RTOS Wi-Fi Migration Developer's Guide under the doc folder in the SDK package. This function stores the bandwidth in a profile in the Flash memory. \n
* It uses the same profile section for a specific wireless port, such as #WIFI_PORT_STA / #WIFI_PORT_APCLI.
* The profile is read to initialize the system at boot up.
*
* @param[in] port indicates the Wi-Fi port that the function will operate on.
*
* Value                         |Definition                                                              |
* ------------------------------|------------------------------------------------------------------------|
* \b #WIFI_PORT_STA             | STA|
* \b #WIFI_PORT_APCLI           | AP Client|
* \b #WIFI_PORT_AP              | AP|
*
* @param[in] bandwidth is the wireless bandwidth.
*
* Value                                                       |Definition                                |
* ------------------------------------------------------------|------------------------------------------|
* \b #WIFI_IOT_COMMAND_CONFIG_BANDWIDTH_20MHZ                 | 20MHz |
* \b #WIFI_IOT_COMMAND_CONFIG_BANDWIDTH_40MHZ                 | 40MHz |
* \b #WIFI_IOT_COMMAND_CONFIG_BANDWIDTH_2040MHZ               | 20 or 40MHz |
*
* @return  >=0 the operation completed successfully, <0 the operation failed.
*
* @note    The default value is #WIFI_IOT_COMMAND_CONFIG_BANDWIDTH_20MHZ.\n
*/
int32_t wifi_profile_set_bandwidth(uint8_t port, uint8_t bandwidth);

/**
* @deprecated This function is deprecated! For more information, please refer to LinkIt for RTOS Wi-Fi Migration Developer's Guide under the doc folder in the SDK package. This function gets bandwidth from a profile in the Flash memory. It uses the same profile section for a specific wireless port, such as #WIFI_PORT_STA / #WIFI_PORT_APCLI.
*
* @param[in] port indicates the Wi-Fi port that the function will operate on.
*
* Value                         |Definition                                                              |
* ------------------------------|------------------------------------------------------------------------|
* \b #WIFI_PORT_STA             | STA|
* \b #WIFI_PORT_APCLI           | AP Client|
* \b #WIFI_PORT_AP              | AP|
*
* @param[out]  bandwidth is the wireless bandwidth.
*
* Value                                                        |Definition                               |
* -------------------------------------------------------------|-----------------------------------------|
* \b #WIFI_IOT_COMMAND_CONFIG_BANDWIDTH_20MHZ                  | 20MHz |
* \b #WIFI_IOT_COMMAND_CONFIG_BANDWIDTH_40MHZ                  | 40MHz |
* \b #WIFI_IOT_COMMAND_CONFIG_BANDWIDTH_2040MHZ                | 20 or 40MHz |
*
* @return  >=0 the operation completed successfully, <0 the operation failed.
*
* @note    The default value is #WIFI_IOT_COMMAND_CONFIG_BANDWIDTH_20MHZ.\n
*/
int32_t wifi_profile_get_bandwidth(uint8_t port, uint8_t *bandwidth);

/**
* @deprecated This function is deprecated! For more information, please refer to LinkIt for RTOS Wi-Fi Migration Developer's Guide under the doc folder in the SDK package. This function gets the MAC address of the Wi-Fi interface from a profile in the Flash memory.
*
* @param[in] port indicates the Wi-Fi port that the function will operate on.
*
* Value                         |Definition                                                              |
* ------------------------------|------------------------------------------------------------------------|
* \b #WIFI_PORT_STA             | STA|
* \b #WIFI_PORT_APCLI           | AP Client|
* \b #WIFI_PORT_AP              | AP|
*
* @param[out]  address is the MAC address #WIFI_MAC_ADDRESS_LENGTH bytes.
*
* @return  >=0 the operation completed successfully, <0 the operation failed.
*/
int32_t  wifi_profile_get_mac_address(uint8_t port, uint8_t *address);

/**
* @deprecated This function is deprecated! For more information, please refer to LinkIt for RTOS Wi-Fi Migration Developer's Guide under the doc folder in the SDK package. This function sets the MAC address of the Wi-Fi interface in a profile in the Flash memory.
* It uses the same profile section for a specific wireless port, such as #WIFI_PORT_STA / #WIFI_PORT_APCLI.
* Currently, in the AP mode, the Wi-Fi driver
* also uses the MAC address in STA profile section.
* The profile is read to initialize the system at boot up.
* Note that there is no corresponding wifi_config_set_mac_address(), because the MAC
* address cannot be modified dynamically.
*
* @param[in] port indicates the Wi-Fi port that the function will operate on.
*
* Value                         |Definition                                                              |
* ------------------------------|------------------------------------------------------------------------|
* \b #WIFI_PORT_STA             | STA|
* \b #WIFI_PORT_APCLI           | AP Client|
* \b #WIFI_PORT_AP              | AP|
*
* @param[in] address is the buffer assigned for the MAC Address, with the length of #WIFI_MAC_ADDRESS_LENGTH in bytes.
*
* @return  >=0 the operation completed successfully, <0 the operation failed.
*/
int32_t wifi_profile_set_mac_address(uint8_t port, uint8_t *address);

/**
* @deprecated This function is deprecated! For more information, please refer to LinkIt for RTOS Wi-Fi Migration Developer's Guide under the doc folder in the SDK package. This function stores the SSID in a profile in the Flash memory. \n
* It uses the same profile section for a specific wireless port, such as #WIFI_PORT_STA / #WIFI_PORT_APCLI.
* The profile is read to initialize the system at boot up.
*
* @param[in] port indicates the Wi-Fi port that the function will operate on.
*
* Value                         |Definition                                                              |
* ------------------------------|------------------------------------------------------------------------|
* \b #WIFI_PORT_STA             | STA|
* \b #WIFI_PORT_APCLI           | AP Client|
* \b #WIFI_PORT_AP              | AP|
*
* @param[in] ssid is the SSID content. If #WIFI_PORT_STA or #WIFI_PORT_APCLI, the SSID indicates the target AP's SSID.
* If #WIFI_PORT_AP, the SSID indicates the device's own SSID.
* @param[in] ssid_length is the length of the SSID, the maximum is #WIFI_MAX_LENGTH_OF_SSID in bytes.
*
* @return  >=0 the operation completed successfully, <0 the operation failed.
*/
int32_t wifi_profile_set_ssid(uint8_t port, uint8_t *ssid , uint8_t ssid_length);

/**
* @deprecated This function is deprecated! For more information, please refer to LinkIt for RTOS Wi-Fi Migration Developer's Guide under the doc folder in the SDK package. This function gets the SSID from a profile in the Flash memory. It uses the same profile section for a specific wireless port, such as #WIFI_PORT_STA / #WIFI_PORT_APCLI.
*
* @param[in] port indicates the Wi-Fi port that the function will operate on.
*
* Value                         |Definition                                                              |
* ------------------------------|------------------------------------------------------------------------|
* \b #WIFI_PORT_STA             | STA|
* \b #WIFI_PORT_APCLI           | AP Client|
* \b #WIFI_PORT_AP              | AP|
*
* @param[out]  ssid is the SSID content. If #WIFI_PORT_STA or #WIFI_PORT_APCLI, the SSID indicates the target AP's SSID.
* If #WIFI_PORT_AP, the SSID indicates the device's own SSID.
* @param[out]  ssid_length is the length of the SSID, the maximum is #WIFI_MAX_LENGTH_OF_SSID in bytes.
*
* @return  >=0 the operation completed successfully, <0 the operation failed.
*/
int32_t wifi_profile_get_ssid(uint8_t port, uint8_t *ssid, uint8_t *ssid_length);

/**
* @deprecated This function is deprecated! For more information, please refer to LinkIt for RTOS Wi-Fi Migration Developer's Guide under the doc folder in the SDK package. This function stores the Wi-Fi Wireless Mode in a profile in the Flash memory. \n
* It uses the same profile section for a specific wireless port, such as #WIFI_PORT_STA / #WIFI_PORT_APCLI.
* The profile is read to initialize the system at boot up.
*
* @param[in] port indicates the Wi-Fi port that the function will operate on.
*
* Value                         |Definition                                                              |
* ------------------------------|------------------------------------------------------------------------|
* \b #WIFI_PORT_STA             | STA|
* \b #WIFI_PORT_APCLI           | AP Client|
* \b #WIFI_PORT_AP              | AP|
*
* @param[in] mode  is wireless mode. For more details, please refer to #wifi_phy_mode_t.
*
* @return  >=0 the operation completed successfully, <0 the operation failed.
*/
int32_t wifi_profile_set_wireless_mode(uint8_t port, wifi_phy_mode_t mode);

/**
* @deprecated This function is deprecated! For more information, please refer to LinkIt for RTOS Wi-Fi Migration Developer's Guide under the doc folder in the SDK package. This function gets the Wi-Fi Wireless Mode from a profile in the Flash memory. It uses the same profile section for a specific wireless port, such as #WIFI_PORT_STA / #WIFI_PORT_APCLI.
*
* @param[in] port indicates the Wi-Fi port that the function will operate on.
*
* Value                         |Definition                                                              |
* ------------------------------|------------------------------------------------------------------------|
* \b #WIFI_PORT_STA             | STA|
* \b #WIFI_PORT_APCLI           | AP Client|
* \b #WIFI_PORT_AP              | AP|
*
* @param[out]  mode is wireless mode. For more details, please refer to #wifi_phy_mode_t.
*
* @return  >=0 the operation completed successfully, <0 the operation failed.
*/
int32_t wifi_profile_get_wireless_mode(uint8_t port, wifi_phy_mode_t *mode);

/**
* @deprecated This function is deprecated! For more information, please refer to LinkIt for RTOS Wi-Fi Migration Developer's Guide under the doc folder in the SDK package. This function stores the Wi-Fi authentication mode and encryption type in a profile in the Flash memory.
* It uses the same profile section for a specific wireless port, such as #WIFI_PORT_STA / #WIFI_PORT_APCLI.
* The profile is read to initialize the system at boot up.
*
* @param[in] port indicates the Wi-Fi port which the function will operate to.
*
* Value                         |Definition                                                              |
* ------------------------------|------------------------------------------------------------------------|
* \b #WIFI_PORT_STA             | STA|
* \b #WIFI_PORT_APCLI           | AP Client|
* \b #WIFI_PORT_AP              | AP|
*
* @param[out]  auth_mode is the authentication mode. For more details, please refer to #wifi_auth_mode_t.
* @param[out]  encrypt_type is the encryption type. For more details, please refer to #wifi_encrypt_type_t.
*
* @return  >=0 the operation completed successfully, <0 the operation failed.
*/
int32_t wifi_profile_set_security_mode(uint8_t port, wifi_auth_mode_t auth_mode, wifi_encrypt_type_t encrypt_type);

/**
* @deprecated This function is deprecated! For more information, please refer to LinkIt for RTOS Wi-Fi Migration Developer's Guide under the doc folder in the SDK package. This function gets the Wi-Fi authentication mode and encryption type from a profile in the Flash memory. It uses the same profile section for a specific wireless port, such as #WIFI_PORT_STA / #WIFI_PORT_APCLI.
*
* @param[in] port indicates the Wi-Fi port that the function will operate on.
*
* Value                         |Definition                                                              |
* ------------------------------|------------------------------------------------------------------------|
* \b #WIFI_PORT_STA             | STA|
* \b #WIFI_PORT_APCLI           | AP Client|
* \b #WIFI_PORT_AP              | AP|
*
* @param[out]  auth_mode is the authentication mode. For more details, please refer to #wifi_auth_mode_t.
* @param[out]  encrypt_type is the encryption type. For more details, please refer to #wifi_encrypt_type_t.
*
* @return  >=0 the operation completed successfully, <0 the operation failed.
*/
int32_t wifi_profile_get_security_mode(uint8_t port, wifi_auth_mode_t *auth_mode, wifi_encrypt_type_t *encrypt_type);

/**
* @deprecated This function is deprecated! For more information, please refer to LinkIt for RTOS Wi-Fi Migration Developer's Guide under the doc folder in the SDK package. This function stores the Wi-Fi WPAPSK/WPA2PSK passphrase in a profile in the Flash memory. \n
* It uses the same profile section for a specific wireless port, such as #WIFI_PORT_STA / #WIFI_PORT_APCLI.
* The profile is read to initialize the system at boot up.
* PMK is preferred if the PSK and PMK exist simultaneously.
*
*
* @param[in] port indicates the Wi-Fi port that the function will operate on.
*
* Value                         |Definition                                                              |
* ------------------------------|------------------------------------------------------------------------|
* \b #WIFI_PORT_STA             | STA|
* \b #WIFI_PORT_APCLI           | AP Client|
* \b #WIFI_PORT_AP              | AP|
*
* @param[in] passphrase is from 8 to 63 bytes ASCII or 64bytes hexadecimal.
* @param[in] passphrase_length is the length, ranging from 8 to 64.
*
* @return  >=0 the operation completed successfully, <0 the operation failed.
*/
int32_t wifi_profile_set_wpa_psk_key(uint8_t port, uint8_t *passphrase, uint8_t passphrase_length);

/**
* @deprecated This function is deprecated! For more information, please refer to LinkIt for RTOS Wi-Fi Migration Developer's Guide under the doc folder in the SDK package. This function gets the Wi-Fi WPAPSK/WPA2PSK passphrase from a profile in the Flash memory. It uses the same profile section for a specific wireless port, such as #WIFI_PORT_STA / #WIFI_PORT_APCLI.
*
* @param[in] port indicates the Wi-Fi port that the function will operate on.
*
* Value                         |Definition                                                              |
* ------------------------------|------------------------------------------------------------------------|
* \b #WIFI_PORT_STA             | STA|
* \b #WIFI_PORT_APCLI           | AP Client|
* \b #WIFI_PORT_AP              | AP|
*
* @param[out]  passphrase is from 8 to 63 bytes ASCII or 64bytes hexadecimal.
* @param[out]  passphrase_length is the length, ranging from 8 to 64.
*
* @return  >=0 the operation completed successfully, <0 the operation failed.
*/
int32_t wifi_profile_get_wpa_psk_key(uint8_t port, uint8_t *passphrase, uint8_t *passphrase_length);

/**
* @deprecated This function is deprecated! For more information, please refer to LinkIt for RTOS Wi-Fi Migration Developer's Guide under the doc folder in the SDK package. This function stores the PMK in a profile in the Flash memory. It uses the same profile section for a specific wireless port, such as #WIFI_PORT_STA / #WIFI_PORT_APCLI.
* The profile is read to initialize the system at boot up.
* PMK is preferred, if the PSK and PMK exist simultaneously.
*
* @param[in] port indicates the Wi-Fi port that the function will operate on.
*
* Value                         |Definition                                                              |
* ------------------------------|------------------------------------------------------------------------|
* \b #WIFI_PORT_STA             | STA|
* \b #WIFI_PORT_APCLI           | AP Client|
* \b #WIFI_PORT_AP              | AP|
*
* @param[in] pmk is the Pairwise Master Key, the shared secret key used in the IEEE 802.11 protocol.
*
* @return >=0 the operation completed successfully, <0 the operation failed.
*/
int32_t wifi_profile_set_pmk(uint8_t port, uint8_t *pmk);

/**
* @deprecated This function is deprecated! For more information, please refer to LinkIt for RTOS Wi-Fi Migration Developer's Guide under the doc folder in the SDK package. This function gets the PMK from a profile in the Flash memory.
* @param[in] port indicates the Wi-Fi port that the function will operate on.
*
* Value                         |Definition                                                              |
* ------------------------------|------------------------------------------------------------------------|
* \b #WIFI_PORT_STA             | STA|
* \b #WIFI_PORT_APCLI           | AP Client|
* \b #WIFI_PORT_AP              | AP|
*
* @param[out] pmk is the Pairwise Master Key, the shared secret key used in the IEEE 802.11 protocol.
*
* @return >=0 the operation completed successfully, <0 the operation failed.
*/
int32_t wifi_profile_get_pmk(uint8_t port, uint8_t *pmk);

/**
* @deprecated This function is deprecated! For more information, please refer to LinkIt for RTOS Wi-Fi Migration Developer's Guide under the doc folder in the SDK package. This function stores Wi-Fi WEP Keys in a profile in the Flash memory.
* It uses the same profile section for a specific wireless port, such as #WIFI_PORT_STA / #WIFI_PORT_APCLI.
* The profile is read to initialize the system at boot up.
*
* @param[in] port indicates the Wi-Fi port that the function will operate on.
*
* Value                         |Definition                                                              |
* ------------------------------|------------------------------------------------------------------------|
* \b #WIFI_PORT_STA             | STA|
* \b #WIFI_PORT_APCLI           | AP Client|
* \b #WIFI_PORT_AP              | AP|
*
* @param[in] wep_keys are the WEP keys. For more details, please refer to #wifi_wep_key_t.
*
* @return  >=0 the operation completed successfully, <0 the operation failed.
*/
int32_t wifi_profile_set_wep_key(uint8_t port, wifi_wep_key_t *wep_keys);

/**
* @deprecated This function is deprecated! For more information, please refer to LinkIt for RTOS Wi-Fi Migration Developer's Guide under the doc folder in the SDK package. This function gets the Wi-Fi WEP Keys from a profile in the Flash memory. It uses the same profile section for a specific wireless port, such as #WIFI_PORT_STA / #WIFI_PORT_APCLI.
* @param[in] port indicates the Wi-Fi port that the function will operate on.
*
* Value                         |Definition                                                              |
* ------------------------------|------------------------------------------------------------------------|
* \b #WIFI_PORT_STA             | STA|
* \b #WIFI_PORT_APCLI           | AP Client|
* \b #WIFI_PORT_AP              | AP|
*
* @param[out]  wep_keys are the WEP keys. For more details, please refer to #wifi_wep_key_t.
*
* @return >=0 the operation completed successfully, <0 the operation failed.
*/
int32_t wifi_profile_get_wep_key(uint8_t port, wifi_wep_key_t *wep_keys);

/**
* @deprecated This function is deprecated! For more information, please refer to LinkIt for RTOS Wi-Fi Migration Developer's Guide under the doc folder in the SDK package. This function gets the Wi-Fi Country Region used in the Wi-Fi driver.
* Various world regions have allocated different amounts of frequency space
* around 2.4GHz for Wi-Fi and some of them have fewer or more channels
* available than others. The region setting ensures that your router will only
* let you use the Wi-Fi channels valid in your location.
*
* @param[in] band  indicates the Wi-Fi bandwidth that the function will operate on.
*
* Value                         |Definition                                                              |
* ------------------------------|------------------------------------------------------------------------|
* \b #WIFI_BAND_2_4_G           | 2.4GHz|
* \b #WIFI_BAND_5_G             | 5GHz|
*
* @param[out]  region  is the region code that relates to the channel list. For more details, please refer to #wifi_config_set_country_region().
*
* @return  >=0 the operation completed successfully, <0 the operation failed.
*/
int32_t wifi_profile_get_country_region(uint8_t band, uint8_t  *region);

/**
* @deprecated This function is deprecated! For more information, please refer to LinkIt for RTOS Wi-Fi Migration Developer's Guide under the doc folder in the SDK package. This function sets the Wi-Fi Country Region in a profile in the Flash memory.
* Various world regions have allocated different amounts of frequency space
* around 2.4GHz for Wi-Fi and some of them have fewer or more channels
* available than others. The region setting ensures that the router only operates on the Wi-Fi channels valid for the specific location.
* The profile is read to initialize the system at boot up.
*
* @param[in] band indicates the Wi-Fi bandwidth that the function will operate on.
*
* Value                         |Definition                                                              |
* ------------------------------|------------------------------------------------------------------------|
* \b #WIFI_BAND_2_4_G           | 2.4GHz|
* \b #WIFI_BAND_5_G             | 5GHz|
*
* @param[in] region is the region code that related to the channel list. For more details, please refer to #wifi_config_set_country_region().
 *
* @return  >=0 the operation completed successfully, <0 the operation failed.
*/
int32_t wifi_profile_set_country_region(uint8_t band, uint8_t region);

/**
* @deprecated This function is deprecated! For more information, please refer to LinkIt for RTOS Wi-Fi Migration Developer's Guide under the doc folder in the SDK package. This function gets the Wi-Fi DTIM interval from a profile in the Flash memory.
*
* @param[out] interval is the interval of a beacon in a range from 1 to 255. For more details, please refer to #wifi_config_set_dtim_interval().
*
* @return  >=0 the operation completed successfully, <0 the operation failed.
*/
int32_t wifi_profile_get_dtim_interval(uint8_t *interval);

/**
* @deprecated This function is deprecated! For more information, please refer to LinkIt for RTOS Wi-Fi Migration Developer's Guide under the doc folder in the SDK package. This function sets the Wi-Fi DTIM interval in a profile in the Flash memory. This API can be used only for STA mode.
* The profile is read to initialize the system at boot up.
*
* @param[in] interval is the interval of a beacon in a range from 1 to 255. For more details, please refer to #wifi_config_set_dtim_interval().
*
* @return  >=0 the operation completed successfully, <0 the operation failed.
*/
int32_t wifi_profile_set_dtim_interval(uint8_t interval);

/**
* @deprecated This function is deprecated! For more information, please refer to LinkIt for RTOS Wi-Fi Migration Developer's Guide under the doc folder in the SDK package. This function gets the Wi-Fi Listen Interval from a profile in the Flash memory.
*
* @param[out] interval is the interval of a beacon in a range from 1 to 255. The beacon's interval is usually
* 100 times the unit (100*1.024 ms), which means, the DTIM interval range is (1~255) * 102.4ms.
*
* @return  >=0 the operation completed successfully, <0 the operation failed.
*/
int32_t wifi_profile_get_listen_interval(uint8_t *interval);

/**
* @deprecated This function is deprecated! For more information, please refer to LinkIt for RTOS Wi-Fi Migration Developer's Guide under the doc folder in the SDK package. This function sets the Wi-Fi Listen Interval in a profile in the Flash memory. This API can be used only for STA mode.
* The profile is read to initialize the system at boot up.
*
* @param[in] interval is the interval of a beacon in a range from 1 to 255.
*
* @return  >=0 the operation completed successfully, <0 the operation failed.
*/
int32_t wifi_profile_set_listen_interval(uint8_t interval);

/**
* @deprecated This function is deprecated! For more information, please refer to LinkIt for RTOS Wi-Fi Migration Developer's Guide under the doc folder in the SDK package. This function stores the Wi-Fi Power Save mode in a profile in the Flash memory. \n
* The profile is read to initialize the system at boot up.
*
* @param[in] power_save_mode indicates three power save mode below.
*
* Value                       |Definition                                                                |
* ----------------------------|--------------------------------------------------------------------------|
* \b 0                        | CAM: CAM (Constantly Awake Mode) is a power save mode that keeps the radio powered up continuously to ensure there is a minimal lag in response time. This power save setting consumes the most power but offers the highest throughput.|
* \b 1                        | LEGACY_POWERSAVE: the access point buffers incoming messages for the radio. The radio occasionally 'wakes up' to determine if any buffered messages are waiting and then returns to sleep mode after it requests each message. This setting conserves the most power but also provides the lowest throughput. It is recommended for radios in which power consumption is the most important (such as small battery-operating devices).|
* \b 2                        | FAST_POWERSAVE: Fast is a power save mode that switches between power saving and CAM modes, depending on the network traffic. For example, it switches to CAM when receiving a large number of packets and switches back to PS mode after the packets have been retrieved. Fast is recommended when power consumption and throughput are a concern.|
*
* @return  >=0 the operation completed successfully, <0 the operation failed.
*/
int32_t wifi_profile_set_power_save_mode(uint8_t power_save_mode);

/**
* @deprecated This function is deprecated! For more information, please refer to LinkIt for RTOS Wi-Fi Migration Developer's Guide under the doc folder in the SDK package. This function gets the Wi-Fi Power Save mode from a profile in the Flash memory. This profile setting is only loaded during the system boot up.
*
* @param[out]  power_save_mode is the three power save mode can be set.
*
* Value                       |Definition                                                                |
* ----------------------------|--------------------------------------------------------------------------|
* \b 0                        | CAM: CAM (Constantly Awake Mode) is a power save mode that keeps the radio powered up continuously to ensure there is a minimal lag in response time. This power save setting consumes the most power but offers the highest throughput.|
* \b 1                        | LEGACY_POWERSAVE: the access point buffers incoming messages for the radio. The radio occasionally 'wakes up' to determine if any buffered messages are waiting and then returns to sleep mode after it requests each message. This setting conserves the most power but also provides the lowest throughput. It is recommended for radios in which power consumption is the most important (such as small battery-operating devices).|
* \b 2                        | FAST_POWERSAVE: Fast is a power save mode that switches between power saving and CAM modes, depending on the network traffic. For example, it switches to CAM when receiving a large number of packets and switches back to PS mode after the packets have been retrieved. Fast is recommended when power consumption and throughput are a concern.|
*
* @return  >=0 the operation completed successfully, <0 the operation failed.
*/
int32_t wifi_profile_get_power_save_mode(uint8_t *power_save_mode);

/**
* @deprecated This function is deprecated! For more information, please refer to LinkIt for RTOS Wi-Fi Migration Developer's Guide under the doc folder in the SDK package. This function is not implemented.
*/
int32_t wifi_profile_commit_setting(char *profile_section);

/**
* @deprecated This function is deprecated! For more information, please refer to LinkIt for RTOS Wi-Fi Migration Developer's Guide under the doc folder in the SDK package. This function is not implemented.
*/
int32_t wifi_profile_get_profile(uint8_t port, wifi_profile_t *profile);

#endif //MTK_WIFI_PROFILE_ENABLE

#ifdef MTK_WIFI_WPS_ENABLE
/**
* @brief This function gets the pin code of a specific wireless port, the pin code may be different for each function call.
* This function is only available when device is running in station mode or in AP mode.
*
* @param[in] port is the Wi-Fi port that the function will operate on.
*
* Value                         |Definition                                                              |
* ------------------------------|------------------------------------------------------------------------|
* \b #WIFI_PORT_STA             | STA|
* \b #WIFI_PORT_AP              | AP|
*
* @param[out]  pin_code is the pin code with 8 ASCII characters.
*
* @return  >=0 the operation completed successfully, <0 the operation failed.
*/
int32_t wifi_wps_config_get_pin_code(uint8_t port, uint8_t *pin_code);

/**
* @brief This function triggers a WPS PBC process.
* This function is only available when the device is in station mode or AP mode.
* If port is #WIFI_PORT_AP, the device acts as a registrar and the BSSID should be NULL. The registrar will send a credential to enrollee after WPS successfully completes.
* If port is #WIFI_PORT_STA, the device acts as an enrollee. The enrollee will give the credentials from registrar and connect to the AP, then store the credential into the Flash memory using Wi-Fi profile APIs. The credentials include the SSID, #wifi_wep_key_t, authentication mode, encryption type and the PSK key.
* The BSSID is optional when port is set to #WIFI_PORT_STA.
* When the BSSID is set, enrollee will only complete the WPS process with the specific AP having the same BSSID.
* When the BSSID is NULL, enrollee will do the WPS process with the AP on which the PBC starts at the same time.
* If there are two or more APs working on WPS PBC, the process will fail according to the specifications.
* This function is only available when device is running in station mode or AP mode.
*
* @param[in] port indicates the Wi-Fi port that the function will operate on.
*
* Value                         |Definition                                                              |
* ------------------------------|------------------------------------------------------------------------|
* \b #WIFI_PORT_STA             | STA|
* \b #WIFI_PORT_AP              | AP|

* @param[in]  bssid is the target AP's MAC address, it should be NULL the port is set to #WIFI_PORT_AP.
*
* @return  >=0 the operation completed successfully, <0 the operation failed.
*/
int32_t wifi_wps_connection_by_pbc(uint8_t port, uint8_t *bssid);

/**
* @brief This function triggers the WPS PIN process.
* If port is #WIFI_PORT_AP, the device is a registrar. In the WPS this condition, the BSSID should be NULL, and the pin is the target enrollee's pin code. The registrar will send the credentials to enrollee after WPS process completes successfully.
* If port is #WIFI_PORT_STA, the device is an enrollee. The enrollee credentials from registrar and connects the AP, then store the credential and the PSK into the Flash memory provides by Wi-Fi profile APIs.The provide credentials include the SSID, #wifi_wep_key_t, authentication mode, encryption type and the PSK key.
* The BSSID is mandatory when port is set to #WIFI_PORT_STA.
* Get the pin_code by calling the #wifi_wps_config_get_pin_code().
* This function is only available when device is running in station mode or AP mode.
*
* @param[in] port indicates the Wi-Fi port that the function will operate on.
*
* Value                         |Definition                                                              |
* ------------------------------|------------------------------------------------------------------------|
* \b #WIFI_PORT_STA             | STA|
* \b #WIFI_PORT_AP              | AP|

* @param[in]  bssid is the target AP's MAC address, it should be NULL while the port is #WIFI_PORT_AP.
*
* @param[in]  pin_code is the enrollee's pin code with 8 ASCII characters,
*             when the device is an enrollee, the pin_code should be generated by #wifi_wps_config_get_pin_code(), the registrar should input the same pin code.
*             when the device is a registrar, the pin_code should be the enrollee's pin code that's transmitted to the registrar using keypad, UI or other interfaces.
* @return  >=0 the operation completed successfully, <0 the operation failed.
*/
int32_t wifi_wps_connection_by_pin(uint8_t port, uint8_t *bssid, uint8_t *pin_code);

/**
* @brief This function sets the WPS device information.
*
* @param[in] device_info indicates the device informaton. For more details, please refer to #wifi_wps_device_info_t.
*            device_information needs to be set each time before starting the WPS process.
*
* @return  >=0 the operation completed successfully, <0 the operation failed.
*/
int32_t wifi_wps_config_set_device_info(wifi_wps_device_info_t *device_info);

/**
* @brief This function gets the WPS device information.
*
* @param[out] device_info indicates the device informaton. For more details, please refer to #wifi_wps_device_info_t.
*             device_information can read back by this API after set.
*
* @return  >=0 the operation completed successfully, <0 the operation failed.
*/
int32_t wifi_wps_config_get_device_info(wifi_wps_device_info_t *device_info);

/**
* @brief This function controls the auto connection behavior after the WPS credential information is received by the Enrollee.
*
* @param[in] auto_connection set the auto_connection to true to enable auto connection, otherwise set it to false.
*                   When auto connection is enabled, the Enrollee will automatically connect to the target AP obtained from the WPS credential information after WPS process is complete.
*                   When auto connection is disabled, the Enrollee will not automatically connect to the target AP after the WPS process is complete.
*
* Value           |Definition                                                |
* ------------------------------|------------------------------------------------------------------------|
* \b  true        |Enable WI-FI auto connection.|
* \b  false       |Disable WI-FI auto connection.|
*
* @return  >=0 the operation completed successfully, <0 the operation failed.
*
* @note    Auto connection is enabled by default.
*/
int32_t wifi_wps_config_set_auto_connection(bool auto_connection);

/**
* @brief This function gets auto connection configuration.
*
* @param[out] auto_connection indicates whether auto connection is enabled or disabled.
*
* Value           |Definition                                                |
* ----------------|------------------------------------------------------------------------|
* \b  true        |Auto connection is enabled and the Enrollee will automatically connect to the target AP got from the WPS credential information after WPS process is done.|
* \b  false       |Auto connection is disabled and the Enrollee will not automatically connect to the target AP.|
*
* @return  >=0 the operation completed successfully, <0 the operation failed.
*/
int32_t wifi_wps_config_get_auto_connection(bool *auto_connection);

#endif //end of MTK_WIFI_WPS_ENABLE

/**
* @brief This function sends a raw Wi-Fi packet over the air.
*
* @param[in] raw_packet is a pointer to the raw packet which is a complete 802.11 packet including the 802.11 header and the payload. The FCS will be automatically computed and attached to the end of the raw packet by hardware.
*            Note that some fields are controlled by hardware, such as power management, sequence number and duration. So the value of these fields set by software will be ignored.
* @param[in] length is the length of the raw packet.
*
* @return  >=0 the operation completed successfully, <0 the operation failed.
*/
int32_t wifi_connection_send_raw_packet(uint8_t *raw_packet, uint32_t length);

#ifdef MTK_SINGLE_SKU_SUPPORT

/**
* @brief This function sets the single SKU power group.
*
* @param[in]  0~3 instead of 4 power group stored in NVDM.
*
* @return  >=0 the operation completed successfully, <0 the operation failed.
*
*/
int32_t wifi_config_set_single_sku_group(uint8_t group);

/**
* @brief This function gets the single SKU power group for WiFi driver.
*
* @param[out]  0~3 instead of 4 power group stored in NVDM.
*
* @return  >=0 the operation completed successfully, <0 the operation failed.
*
*/
int32_t wifi_config_get_single_sku_group(uint8_t * group);

/**
* @brief This function sets the 2G single SKU power.
*
* @param[in]  sku_table_2g is power table of 14 channel in 2G.
*
*For each channel power item format
*| -------|----------|----------|----------- |------------|-------- |---------|---------|---------- |---------- |-------- |-------- |-------- | --------|---------- |---------- |-------- |-------- |-------- |
*|CCK1/2M|CCK5.5/11M|OFDM_6/9M|OFDM_12/18M|OFDM_24/36M|OFDM_48M|OFDM_54M| HT20MCS0|HT20MCS1/2|HT20MCS3/4|HT20MCS5|HT20MCS6|HT20MCS7|HT40MCS0|HT40MCS1/2|HT40MCS3/4|HT40MCS5|HT40MCS6|HT40MCS7|
*
* @return  >=0 the operation completed successfully, <0 the operation failed.
*/
int32_t wifi_config_set_single_sku_2G(singleSKU2G_t* sku_table_2g);

/**
* @brief This function gets the 2G single SKU power.
*
* @param[out]  sku_table_2g is power table of 14 channel in 2G.
*
*For each channel power item format
*| -------|----------|----------|----------- |------------|-------- |---------|---------|---------- |---------- |-------- |-------- |-------- | --------|---------- |---------- |-------- |-------- |-------- |
*|CCK1/2M|CCK5.5/11M|OFDM_6/9M|OFDM_12/18M|OFDM_24/36M|OFDM_48M|OFDM_54M| HT20MCS0|HT20MCS1/2|HT20MCS3/4|HT20MCS5|HT20MCS6|HT20MCS7|HT40MCS0|HT40MCS1/2|HT40MCS3/4|HT40MCS5|HT40MCS6|HT40MCS7|
*
* @return  >=0 the operation completed successfully, <0 the operation failed.
*/
int32_t wifi_config_get_single_sku_2G(singleSKU2G_t* sku_table_2g);

#endif


#ifdef __cplusplus
}
#endif

/**
*@}
*/
#endif /* __WIFI_API_H__ */

