/* Copyright Statement:
 *
 * (C) 2005-2016  MediaTek Inc. All rights reserved.
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
 * the License Agreement ("Permitted User").  If you are not a Permitted User,
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
 * WiFi processor configure / setting for STA operations
 *
 * @author  Michael Rong
 * @version 2015/04/24 Initial
 * @version 2015/06/03 API alternation and Doxygen warning fix.
 */

#ifndef __WIFI_INBAND_H__
#define __WIFI_INBAND_H__

#include "stdint.h"
#include "type_def.h"
#include "wifi_api.h"

#if (defined(__CC_ARM) || defined(__ICCARM__))
#include <stdio.h>
#endif



#ifdef __cplusplus
extern "C" {
#endif

#define WIFI_APPLE_IE_MAX_LENGTH                (256)

#define WIFI_INBAND_CMD_IS_QUERY_SET            (0)
#define WIFI_INBAND_CMD_IS_QUERY_QUERY          (1)
#define WIFI_INBAND_CMD_IS_QUERY_NEED_STATUS    (2)

#ifdef MTK_WIFI_DIRECT_ENABLE
/**
* @brief The p2p state and p2p role information
*/
#define P2P_IDLE    (0)
#define P2P_NEGO    (1)
#define P2P_CONNECT (2)

#define P2P_DEVICE  (0)
#define P2P_CLIENT  (1)
#define P2P_GO      (2)
#endif //MTK_WIFI_DIRECT_ENABLE

#define WIFI_INBAND_CMD_IS_QUERY_SET            (0)
#define WIFI_INBAND_CMD_IS_QUERY_QUERY          (1)
#define WIFI_INBAND_CMD_IS_QUERY_NEED_STATUS    (2)

#define MGMT_PKT_FILTER_ASSOC_REQ    (0x1<<0)
#define MGMT_PKT_FILTER_ASSOC_RESP   (0x1<<1)
#define MGMT_PKT_FILTER_REASSOC_REQ  (0x1<<2)
#define MGMT_PKT_FILTER_REASSOC_RESP (0x1<<3)
#define MGMT_PKT_FILTER_PROBE_REQ    (0x1<<4)
#define MGMT_PKT_FILTER_PROBE_RESP   (0x1<<5)
#define MGMT_PKT_FILTER_BEACON       (0x1<<8)
#define MGMT_PKT_FILTER_ATIM         (0x1<<9)
#define MGMT_PKT_FILTER_REASSOC      (0x1<<10)
#define MGMT_PKT_FILTER_AUTH         (0x1<<11)
#define MGMT_PKT_FILTER_DEAUTH       (0x1<<12)

#define WIFI_DRIVER_STATE_PORT_SECURED   (0x1 << 0)
#define WIFI_DRIVER_STATE_IP_DONE        (0x1 << 1)


/** @brief This enumeration defines the command ID from Host to Wi-Fi firmware.
*/
typedef enum {
    WIFI_COMMAND_ID_IOT_OWN_MAC_ADDRESS = 0xd0,   /**< 0xd0 (Set / Query) */
    WIFI_COMMAND_ID_IOT_SSID,                     /**< 0xd1 (Set / Query) */
    WIFI_COMMAND_ID_IOT_BSSID,                    /**< 0xd2 (Set / Query) */
    WIFI_COMMAND_ID_IOT_BANDWIDTH,                /**< 0xd3 (Set / Query) */
    WIFI_COMMAND_ID_IOT_BANDWIDTH_EXT = WIFI_COMMAND_ID_IOT_BANDWIDTH, /**< 0xd3 (Set / Query) */
    WIFI_COMMAND_ID_IOT_SET_DISCONNECT,           /**< 0xd4 (Set) */
    WIFI_COMMAND_ID_IOT_GET_LINK_STATUS,          /**< 0xd5 (Query) */
    WIFI_COMMAND_ID_IOT_GET_PROFILE,              /**< 0xd6 (Query) */
    WIFI_COMMAND_ID_IOT_START_SCAN,               /**< 0xd7 (Set) */
    WIFI_COMMAND_ID_IOT_STOP_SCAN,                /**< 0xd8 (Set) */
    WIFI_COMMAND_ID_IOT_GET_SCAN_LIST,            /**< 0xd9 (Query) */
    WIFI_COMMAND_ID_IOT_RX_FILTER,                /**< 0xda (Set) */
    WIFI_COMMAND_ID_IOT_CHANNEL,                  /**< 0xdb (Set / Query) */
    WIFI_COMMAND_ID_IOT_80211_TO_HOST,            /**< 0xdc (Set) */
    WIFI_COMMAND_ID_IOT_SWITCH_MODE,              /**< 0xdd (Set) */
    WIFI_COMMAND_ID_IOT_SET_SECURITY,			  /**< 0xde (Set), used in E1 EVB */
    WIFI_COMMAND_ID_IOT_SET_KEY = WIFI_COMMAND_ID_IOT_SET_SECURITY,	    /**< 0xde (Set), used in E2 EVB */
    WIFI_COMMAND_ID_IOT_EXTENSION = WIFI_COMMAND_ID_IOT_SET_SECURITY,	/**< 0xde (Set), used in E2 EVB */
} wifi_command_id_t;

/** @brief This enumeration defines the extension command ID from Host to Wi-Fi firmware.
*/
typedef enum {
    WIFI_COMMAND_ID_EXTENSION_DEBUG_LEVEL = 0x1,           /**< 0x1 (Set / Query) */
    WIFI_COMMAND_ID_EXTENSION_SET_AUTH_MODE ,              /**< 0x2 (Set)         */
    WIFI_COMMAND_ID_EXTENSION_SET_ENCRYPT_TYPE,            /**< 0x3 (Set)         */
    WIFI_COMMAND_ID_EXTENSION_SET_WEP_KEY,                 /**< 0x4 (Set)         */
    WIFI_COMMAND_ID_EXTENSION_WIRELESS_MODE,               /**< 0x5 (Set / Query) */
    WIFI_COMMAND_ID_EXTENSION_COUNTRY_REGION,              /**< 0x6 (Set / Query) */
    WIFI_COMMAND_ID_EXTENSION_RADIO_ON_OFF,                /**< 0x7 (Set / Query) */
    WIFI_COMMAND_ID_EXTENSION_DTIM_INTERVAL,               /**< 0x8 (Set / Query) */
    WIFI_COMMAND_ID_EXTENSION_LISTEN_INTERVAL,             /**< 0x9 (Set / Query) */
    WIFI_COMMAND_ID_EXTENSION_GET_STA_LIST,                /**< 0xA (Query)       */
    WIFI_COMMAND_ID_EXTENSION_SMART_CONNECTION_FILTER,     /**< 0xB (Set / Query) */
    WIFI_COMMAND_ID_EXTENSION_WOW_ENABLE,                  /**< 0x0C (Set/Query)  */
    WIFI_COMMAND_ID_EXTENSION_WOW_MAGIC_PACKET,            /**< 0x0D (Set/Query)  */
    WIFI_COMMAND_ID_EXTENSION_WOW_BITMAP_PATTERN,          /**< 0x0E (Set/Query)  */
    WIFI_COMMAND_ID_EXTENSION_WOW_ARP_OFFLOAD,             /**< 0x0F (Set/Query)  */
    WIFI_COMMAND_ID_EXTENSION_WOW_REKEY_OFFLOAD,           /**< 0x10 (Set/Query)  */
    WIFI_COMMAND_ID_EXTENSION_AP_STOP,                     /**< 0x11 (Set)   */
    WIFI_COMMAND_ID_EXTENSION_AP_START,                    /**< 0x12 (Set)   */
    WIFI_COMMAND_ID_EXTENSION_POWER_MANAGE_STATE,          /**< 0x13 (set)   */
    WIFI_COMMAND_ID_EXTENSION_GET_MAX_STA_NUMBER,          /**< 0x14 (Query) */
    WIFI_COMMAND_ID_EXTENSION_SET_EXTRA_IE,                /**< 0x15 (Set)   */
    WIFI_COMMAND_ID_EXTENSION_UPDATE_HOMEKIT_IE,           /**< 0x16 (Set)   */
    WIFI_COMMAND_ID_EXTENSION_ENABLE_AIRPLAY,              /**< 0x17 (Set)   */
    WIFI_COMMAND_ID_EXTENSION_GET_CONNECTED_AP_RSSI,       /**< 0x18 (Query) */
    WIFI_COMMAND_ID_EXTENSION_SET_PSMODE,                  /**< 0x19 (Set)   */
    WIFI_COMMAND_ID_EXTENSION_EFUSE,                       /**< 0x1A (Set/Query)  */
    WIFI_COMMAND_ID_EXTENSION_TX_RAW,                      /**< 0x1B (Send raw packet) */
    WIFI_COMMAND_ID_EXTENSION_P2P_LISTEN,                  /**< 0x1C (Set)  */
    WIFI_COMMAND_ID_EXTENSION_MBSSID,                      /**< 0x1D (mbssid placeholder) */
    WIFI_COMMAND_ID_EXTENSION_SET_TX_POWER,                /**< 0x1E (settxpower placeholder) */
    WIFI_COMMAND_ID_EXTENSION_SET_IE,                      /**< 0x1F (DO NOT USE, under development)   */
    WIFI_COMMAND_ID_EXTENSION_P2P_STATE_ROLE,              /**< 0x20 (Set)   */
    WIFI_COMMAND_ID_EXTENSION_N9_CONSOLE_LOG,              /**< 0x21 (Set)  */
    WIFI_COMMAND_ID_EXTENSION_RSSI_THRESHOLD = 0x22,       /**< 0x22 (Set/ Query) */
    WIFI_COMMAND_ID_EXTENSION_RX_VECTOR_GRP3 = 0x23,       /**< 0x23 (Set/ Query) */
    WIFI_COMMAND_ID_EXTENSION_PACKET_MANAGEMENT_RX_FILTER = 0x24,
    WIFI_COMMAND_ID_EXTENSION_WIFI_PRIVILEGE = 0x25,       /**< 0x25 (start/stop) */
    WIFI_COMMAND_ID_EXTENSION_IP_MAC_PORT_MATCH_SET = 0x26,/**< 0x26 (Set/ Query) */
    WIFI_COMMAND_ID_EXTENSION_IP_MAC_PORT_LIST_ENTRY = 0x27,/**< 0x27 (Set/ Query) */

    WIFI_COMMAND_ID_EXTENSION_IOT_SSID_CHANNEL = 0x28,     /**< 0x28 (Set/ Query) */
    WIFI_COMMAND_ID_EXTENSION_COUNTRY_CODE =0x29,          /**< 0x29 (Set / Query) */

    WIFI_COMMAND_ID_EXTENSION_DWELL_TIME = 0x2A,           /**< 0x2A (Set/Query) */
    WIFI_COMMAND_ID_EXTENSION_SIGNLE_SKU_2G = 0x2C,       /**< 0x2C (Set/Quary) */
    WIFI_COMMAND_ID_EXTENSION_ANTENNA_MONITOR = 0x2D,        /**< 0x2D (Set) */
    WIFI_COMMAND_ID_EXTENSION_ANTENNA_STATUS  = 0x2E,       /**< 0x2E (Query) */
    WIFI_COMMAND_ID_EXTENSION_STATISTICS  = 0x30,         /**< 0x30 (Query) */
    WIFI_COMMAND_ID_EXTENSION_SIGNLE_SKU_5G = 0x31,         /**< 0x31 (Set/Quary) */    
    WIFI_COMMAND_ID_EXTENSION_IS_AP_CONNECTED = 0x33,         /**< 0x32 (Set/Quary) */    
    WIFI_COMMAND_ID_EXTENSION_IS_AP_IP_READY = 0x34,		  /**< 0x32 (Set/Quary) */	  
    WIFI_COMMAND_ID_EXTENSION_IS_IP_READY = 0x35		  /**< 0x32 (Set/Quary) */	  
} wifi_extension_command_id_t;

#define WIFI_MAX_NUMBER_OF_IP_MAC_PORT_ENTRY  			(10)

typedef struct
{
     uint8_t src_ipAddr[4];
     uint8_t src_macAddr[6];
     uint16_t src_port;
     uint8_t dst_ipAddr[4];
     uint8_t dst_macAddr[6];
     uint16_t dst_port;

     /*
      * valid_bitmap = 0x0001 means src_ipAddr is valid
      * valid_bitmap = 0x0002 means src_macAddr is valid
      * valid_bitmap = 0x0004 src_port is valid
      * valid_bitmap = 0x0008 dst_ipAddr is valid
      * valid_bitmap = 0x0010 means dst_macAddr is valid
      * valid_bitmap = 0x0020 means dst_port is valid
      * bit[6] reserved
      * bit[7] reserved
      */
     uint8_t valid_bitmap;
} ip_match_entry_t;


/** @brief Enable wake on wireless or not.
*/
typedef struct {
    bool enable; /**< 0: disable, 1:enable */
} wifi_wow_enable_t;

/** @brief Wake up host by magic packet.
*/
typedef struct {
    uint8_t bss_index; /**< BSS Index (BIT(0) enable for BSSID0, BIT(1) enable for BSSID1, BIT(2) enable for BSSID2, BIT(3) enable for BSSID3) */
    bool enable; /**< 0: disable, 1:enable the magic packet filter */
} wifi_wow_magic_packet_t;

/** @brief Wake up host on receiving packet with specific pattern.
*/
typedef struct {
    uint8_t bitmap_mask[16]; /**< 16 bytes of Bitmap mask, each bit of bitmap_mask[] use to mask corresponding byte of bitmap_pattern[] */
    uint8_t bitmap_pattern[128]; /**< 128 bytes of Bitmap pattern */
} wifi_wow_bitmap_pattern_t;

/** @brief ARP offloading.
*/
typedef struct {
    uint8_t ip_address[4]; /**< e.g.192.168.0.11-->ip_address[]={192, 168, 0, 11} */
} wifi_wow_arp_offload_t;

/** @brief Re-key offloading.
*/
typedef struct {
    uint8_t ptk[64]; /**< 64 bytes Pairwise Transient Key */
} wifi_wow_rekey_offload_t;


#define MAX_WEP_KEY_SIZE_CM4 32
typedef struct _wep_key_cm4 {
    uint8_t ucdefault_key_id;
    uint8_t auckey_0[MAX_WEP_KEY_SIZE_CM4];
    uint8_t uckey_0_len;
    uint8_t auckey_1[MAX_WEP_KEY_SIZE_CM4];
    uint8_t uckey_1_len;
    uint8_t auckey_2[MAX_WEP_KEY_SIZE_CM4];
    uint8_t uckey_2_len;
    uint8_t auckey_3[MAX_WEP_KEY_SIZE_CM4];
    uint8_t uckey_3_len;
} wep_key_cm4_t;

typedef enum _extra_ie_type {
    WIFI_EXTRA_WPS_BEACON_IE = 0,
    WIFI_EXTRA_WPS_PROBE_REQ_IE = 1,
    WIFI_EXTRA_WPS_PROBE_RSP_IE = 2,
    WIFI_EXTRA_WPS_ASSOC_REQ_IE = 3,
    WIFI_EXTRA_WPS_ASSOC_RSP_IE = 4,
#ifdef MTK_WIFI_DIRECT_ENABLE
    WIFI_EXTRA_P2P_BEACON_IE = 5,
    WIFI_EXTRA_P2P_PROBE_REQ_IE = 6,
    WIFI_EXTRA_P2P_PROBE_RSP_IE = 7,
    WIFI_EXTRA_P2P_ASSOC_REQ_IE = 8,
    WIFI_EXTRA_P2P_ASSOC_RSP_IE = 9,
#endif /*MTK_WIFI_DIRECT_ENABLE*/
} extra_ie_type_t;

typedef struct _extra_ie {
    uint8_t port;
    uint8_t type; /* extra_ie_type */
    uint8_t rsv[2];
    uint32_t extra_ie_len;
#ifndef __CC_ARM
    uint8_t extra_ie[0];
#else
    uint8_t extra_ie[];
#endif
} extra_ie_t;

typedef enum _wifi_tx_mode_t {
    WIFI_MODE_CCK = 0,
    WIFI_MODE_OFDM,
    WIFI_MODE_HTMIX,
    WIFI_MODE_HTGREENFIELD,
    WIFI_MODE_VHT,
    WIFI_MODE_MAX
} wifi_tx_mode_t;


#ifdef MTK_WIFI_DIRECT_ENABLE
typedef struct _P2P_LISTEN_PARAM {
    uint8_t  channel;
    uint8_t  ucReserved;
    uint16_t ListenDuration;                              //in milliseconds
} P2P_LISTEN_PARAM;
#endif /*MTK_WIFI_DIRECT_ENABLE*/

typedef struct _txd_param {
    uint8_t frame_protected; /* 0/1: Disable/Enable protection */
    uint8_t ack_required;           /* 0/1: Disable/Enable ACK */
    uint8_t rate_fixed;       /* 0/1: Disable/Enable Fixed rate */
    uint8_t tx_mode;    /*  0: MODE_CCK, 1: MODE_OFDM, 2: MODE_HTMIX, 3: MODE_HTGREENFIELD, 4: MODE_VHT  */
    uint8_t mcs;   /* Note 1 */
    uint8_t retry_count;    /* 0: No Retry, 1-30: Retry count, 31: Unlimited Retry count  */
    uint8_t reserved[2];
} txd_param_t;

/** @brief This enumeration defines the extension command ID from Host to Wi-Fi firmware.
*/
     /**< 0x24 (Set)  */

typedef struct {
    uint8_t element_id;
    uint8_t length;
    uint8_t OUI[3];
}extra_vender_ie_t;


typedef struct {
    uint8_t antenna_number;   //current antenna 0:ant0   1: ant1
    int8_t  ant0_rssi;
    int8_t  ant1_rssi;
    int8_t  pre_selected_rssi;
} wifi_antenna_status_t;

/** @brief Indicates the wlan inband statistic information.
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
    int32_t SNR;                   /**< Average SNR from Data packets.*/
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
} ATTR_PACKED wifi_inband_statistic_t;

/**
* @brief Get/Set WiFi Operation Mode.
*
* @param [IN]flag_set
* @param [OUT]mode
* @param 0x01 WIFI_MODE_STA_ONLY
* @param 0x02 WIFI_MODE_AP_ONLY
* @param 0x03 WIFI_MODE_REPEATER
*
* @return  >=0 means success, <0 means fail
*/
int32_t wifi_inband_opmode(uint8_t flag_set, uint8_t *mode);

/**
* @brief Configure bandwidth.
*
* @param [IN]flag_set
* @param [IN]port
* @param [IN/OUT] bandwidth  IOT_CMD_CBW_20MHZ, IOT_CMD_CBW_40MHZ , IOT_CMD_CBW_2040MHZ are supported
*
* @return >=0 means success, <0 means fail
*
* @note Default value will be HT_20
*/
int32_t wifi_inband_bandwidth(uint8_t flag_set, uint8_t port, uint8_t *bandwidth);

/**
* @brief Get/Set the current channel for STA wireless port.
*
* @param [IN]flag_set
* @param [IN]port
* @param [IN/OUT] channel I1~14 are supported for 2.4G only product
*
* @return >=0 means success, <0 means fail
*/
int32_t wifi_inband_channel(uint8_t flag_set, uint8_t port, uint8_t *channel);

/**
* @brief Register the handler to receive 802.11 raw packet from network processor .
* @      and the network processor begin to indicate 802.11 raw packet with RXWI
* @param [IN] enable/disable
*
* @return >=0 means success, <0 means fail
*/
int32_t wifi_inband_set_raw_pkt(uint8_t enable);

/**
* @brief Get/Set WiFi SSID.
*
* @param [IN]flag_set
* @param [IN]port
* @param 0 STA
* @param 1 AP
* @param [IN/OUT]ssid SSID
* @param [IN/OUT]ssid_len Length of SSID
*
* @return  >=0 means success, <0 means fail
*/
int32_t wifi_inband_ssid(uint8_t flag_set, uint8_t port, uint8_t *ssid, uint8_t *ssid_length);

/**
* @brief Get/Set WiFi SSID.
*
* @param [IN]flag_set
* @param [IN]port
* @param 0 STA/AP Client
* @param 1 AP
* @param [IN/OUT]ssid SSID
* @param [IN/OUT]ssid_len Length of SSID
* @param [IN]channel The channel of target SSID
*
* @return  >=0 means success, <0 means fail
*/
int32_t wifi_inband_ssid_channel(uint8_t flag_set, uint8_t port, uint8_t *ssid, uint8_t *ssid_length, uint8_t channel);

/**
* @brief Get/Set WiFi BSSID.
*
* @param [IN]flag_set
*
* @param [IN/OUT]bssid BSSID
*
* @param [IN]channel The channel of target BSSID
*
* @param [IN]wps_enable WPS flag
*
* @return  >=0 means success, <0 means fail
* @note Only used for WIFI_PORT_STA
*/
int32_t wifi_inband_bssid(uint8_t flag_set, uint8_t *bssid, uint8_t channel, uint8_t wps_enable, uint8_t ssid_length, uint8_t *ssid);

/**
* @brief Get/Set WiFi Wireless Mode
*
* @param [IN]flag_set
* @param [IN]port
* @param [IN/OUT]mode
* @param  1 legacy 11B only
* @param 2 legacy 11A only
* @param 3 legacy 11A/B/G mixed
* @param 4 legacy 11G only
* @param 5 11ABGN mixed
* @param 6 11N only in 2.4G
* @param 7 11GN mixed
* @param 8 11AN mixed
* @param 9 11BGN mixed
* @param 10 11AGN mixed
*
*/
int32_t wifi_inband_wireless_mode(uint8_t flag_set, uint8_t port, uint8_t *mode);

/**
* @brief Get/Set WiFi Country Region
*
* @param [IN]flag_set
* @param [IN]band
* @param 0 2.4G
* @param 1 5G
*
* @param [IN/OUT]region
* @region: (2.4G)
* @param  0   CH1-11
* @param  1   CH1-13
* @param  2   CH10-11
* @param  3   CH10-13
* @param  4   CH14
* @param  5   CH1-14 all active scan
* @param  6   CH3-9
* @param  7   CH5-13
* @param
* @param  region: (5G)
* @param  0   CH36, 40, 44, 48, 52, 56, 60, 64, 149, 153, 157, 161, 165
* @param  1   CH36, 40, 44, 48, 52, 56, 60, 64, 100, 104, 108, 112, 116, 120, 124, 128, 132, 136, 140
* @param  2   CH36, 40, 44, 48, 52, 56, 60, 64
* @param  3   CH52, 56, 60, 64, 149, 153, 157, 161
* @param  4   CH149, 153, 157, 161, 165
* @param  5   CH149, 153, 157, 161
* @param  6   CH36, 40, 44, 48
* @param  7   CH36, 40, 44, 48, 52, 56, 60, 64, 100, 104, 108, 112, 116, 120, 124, 128, 132, 136, 140, 149, 153, 157, 161, 165
* @param  8   CH52, 56, 60, 64
* @param  9   CH36, 40, 44, 48, 52, 56, 60, 64, 100, 104, 108, 112, 116, 132, 136, 140, 149, 153, 157, 161, 165
* @param  10   CH36, 40, 44, 48, 149, 153, 157, 161, 165
* @param  11   CH36, 40, 44, 48, 52, 56, 60, 64, 100, 104, 108, 112, 116, 120, 149, 153, 157, 161
*/
int32_t wifi_inband_country_region(uint8_t flag_set, uint8_t band, uint8_t *region);

/**
* @brief Get/Set Country code
*
* @param [IN]flag_set
* @param [IN/OUT]country_code
*
* @return  >=0 means success, <0 means fail
*
*/
int32_t wifi_inband_country_code(uint8_t flag_set, uint8_t *country_code);

/**
* @brief Get/Set WiFi DTIM Interval
*
* @param [IN]flag_set
* @param [IN/OUT]interval 1 ~ 255
*
* @return  >=0 means success, <0 means fail
*
*/
int32_t wifi_inband_dtim_interval(uint8_t flag_set, uint8_t *interval);

/**
* @brief Get/Set WiFi Listen Interval
*
* @param [IN]flag_set
* @param [IN/OUT]interval 1 ~ 255
*
*/
int32_t wifi_inband_listen_interval(uint8_t flag_set, uint8_t *interval);


/**
* @brief Get/Set WiFi Radio ON/OFF
*
* @param [IN]flag_set
* @param [IN/OUT]onoff
* @param 0 OFF
* @param 1 ON
*
* @return  >=0 means success, <0 means fail
*
* @note in MODE_Dual, both WiFi interface radio will be turn on/off at the same time
*/
int32_t wifi_inband_radio_on(uint8_t flag_set, uint8_t *onoff);


/**
 * @brief Get the debug level from coprocessor.
 *
 * @param debug_level the debug level to be passed back to caller.
 *
 * @return >=0 means success, <0 means fail
 */
int32_t wifi_inband_get_debug_level(uint8_t *debug_level);


/**
 * @brief Set the specified debug level to coprocessor.
 *
 * @param debug_level the debug level to be supplied to coprocessor.
 *
 * @return >=0 means success, <0 means fail
 */
int32_t wifi_inband_set_debug_level(uint8_t debug_level);

int32_t wifi_inband_set_is_ap_connected(uint8_t mode);

int32_t wifi_inband_set_is_ap_ip_ready(uint8_t mode);

int32_t wifi_inband_set_is_ip_ready(uint8_t mode);



/**
* @brief Set the authentication mode for the target AP
*
* @param [IN]authmode
* @param 0 OPEN
* @param 1 SHARED
* @param 2 WEPAUTO
* @param 3 WPA
* @param 4 WPAPSK
* @param 5 WPANONE
* @param 6 WPA2
* @param 7 WPA2PSK
* @param 8 WPA1WPA2
* @param 9 WPA1PSKWPA2PSK
*
* @return >=0 means success, <0 means fail
* @note Default to OPEN
*/
int32_t wifi_inband_set_auth_mode(uint8_t port, uint8_t authmode);

/**
* @brief Set the encryption method for the target AP
*
* @parameter
*   [IN] encryption method index:
*         1 NONE
*         0 WEP
*         4 TKIP
*         6 AES
*         TKIPAES
* @return >=0 means success, <0 means fail
* @note Default to NONE
*/
int32_t wifi_inband_set_encryption(uint8_t port, uint8_t encryption);

/**
* @brief Set WiFi WEP Keys
*
* @param [IN]wep_key_cm4_t
*
* @return >=0 means success, <0 means fail
*/
int32_t wifi_inband_set_wep_key(uint8_t port, wep_key_cm4_t *wep_keys);

/**
* @brief Set WiFi PTK Keys
*
* @param [IN]keys
*
* @return >=0 means success, <0 means fail
*/
int32_t wifi_inband_set_ptk_key(uint8_t port, uint8_t *keys);


/**
* @brief Disconnect the current connection
*
* @parameter address
* @return >=0 means success, <0 means fail
* @note if WIFI_PORT_STA or PORT_APCLI, it  will back to scan state once disconnect from AP
* @note if WIFI_PORT_AP, send the disconnected STA's mac address as parameter.
*/
int32_t wifi_inband_disconnect(uint8_t port, uint8_t *address);

/**
* @brief Get the current STA port link up / link down status of the connection
*
* @parameter None
* @return >=0 means success, the status will be:
*               WIFI_STATUS_LINK_DISCONNECTED(0) and
*               WIFI_STATUS_LINK_CONNECTED(1),
*               <0 means fail
* @note WIFI_STATUS_LINK_DISCONNECTED indicates STA may in IDLE/ SCAN/ CONNECTING state
*/
int32_t wifi_inband_get_link_status(uint8_t *link_status);

/**
* @brief Get WiFi Associated Station List
*
* @param [OUT]station_list
*
* @return Number of associated stations will be returned
*/
int32_t wifi_inband_get_station_list(uint8_t *number, wifi_sta_list_t *station_list);

/**
* @brief Get the max number of the supported stations in ap mode or Repeater Mode.
*
* @param [OUT]sta_number The max number of the supported stations will be returned
*
* @return  >=0 means success, <0 means fail
*
*/
int32_t wifi_inband_get_max_station_number(uint8_t *sta_number);


/**
* @brief Star WiFi Scanning
*
* @param [IN]ssid SSID
* @param [IN]ssid_length Length of SSID
* @param [IN]bssid BSSID
* @param [IN]scan_mode
* @param 0 full
* @param 1 partial
* @param [IN]scan_option
* @param 0: active (passive in regulatory channel)
* @param 1: passive in all channel
* @param 2: force_active (active in all channel)
*
* @return  >=0 means success, <0 means fail
*
* @note When SSID/BSSID specified (not NULL)
* @note 1. ProbeReq carries SSID (for Hidden AP)
* @note 2. Scan Table always keeps the APs of this specified SSID/BSSID
*/
int32_t wifi_inband_start_scan(uint8_t *ssid, uint8_t ssid_length, uint8_t *bssid, uint8_t scan_mode, uint8_t scan_option);

/**
* @brief Stop WiFi Scanning
*
* @return  >=0 means success, <0 means fail
*/
int32_t wifi_inband_stop_scan(void);

#ifdef MTK_WIFI_DIRECT_ENABLE
/**
* @brief Start WiFi p2p listen procedure (remain_on_channel)
*
* @return  >=0 means success, <0 means fail
*/
int32_t wifi_inband_p2p_listen(P2P_LISTEN_PARAM *remain_on_channel_param);

/**
* @brief Inform N9 the p2p_state and p2p role to do opmode switch, apstart...
*
* @return  >=0 means success, <0 means fail
*/
int32_t wifi_inband_p2p_info(uint8_t p2p_state, uint8_t p2p_role);
#endif /*MTK_WIFI_DIRECT_ENABLE*/

/**
* @brief Configure packet format wanted to be received
*
* @param [IN/OUT]filter defined in  wifi_rx_filter_t
*
* @return >=0 means success, <0 means fail
*/
int32_t wifi_inband_rx_filter(uint8_t flag_set, uint32_t *filter);

/**
* @brief Set/Get MTK Smart Connection Filter
*
* @param [IN]flag_set
* @param [IN/OUT]enable
*
* @return  >=0 means success, <0 means fail
*/
int32_t wifi_inband_smart_connection(uint8_t flag_set, uint8_t  *enable);

/**
* @brief Inform N9 to do ApStop
*
* @return >=0 means success, <0 means fail
*/
int32_t wifi_inband_set_ap_stop(void);

/**
* @brief Inform N9 to do ApStart
*
* @return >=0 means success, <0 means fail
*/
int32_t wifi_inband_set_ap_start(void);

/**
* @brief Set WiFi Extra IE
*
* @param [IN]port
* @param 0 AP
* @param 1 AP Client
* @param 2 STA
* @param [IN]ie_type
* @param [IN]extra_ie
* @param [IN]extra_ie_len
*
* @return  >=0 means success, <0 means fail
*/
int32_t wifi_inband_set_extra_ie(uint8_t port, uint8_t ie_type, uint8_t *extra_ie, uint32_t extra_le_len);

/**
* @brief Get the rssi of the connected AP
*
* @parameter None
*
* @return  >=0 means success, <0 means fail
*/
int32_t wifi_inband_get_rssi(int8_t *rssi);


/**
* @brief Set the rssi threshold which is used to filter packets
*
* @param [IN]enable
* @param 0 enable rssi threshold
* @param 1 disable rssi threshold
* @param [IN]rssi value only useful when enable=1
*
* @return  >=0 means success, <0 means fail
*/
int32_t wifi_inband_set_rssi_threshold(uint8_t enable, int8_t rssi);


/**
 * @brief Get the rssi threshold which is used to filter packets
 *
 * @param [IN]enable to specify that rssi threshold is on/off
 * @param [IN]rssi value if enable=1, otherwise nosense
 *
 *@return  >=0 means success, <0 means fail
 */
int32_t wifi_inband_get_rssi_threshold(uint8_t *enable, int8_t *rssi);


/**
* @brief Inform N9 to configure PSMode
* @param [IN] ps mode
*
* @return >=0 means success, <0 means fail
*/
int32_t wifi_inband_set_ps_mode(uint8_t ps_mode);

/**
* @brief Set/Get ip mac port filter
*
* @param [IN]flag_set
* @param [IN/OUT]enable
*
* @return  >=0 means success, <0 means fail
*/
int32_t wifi_inband_ip_mac_port_filter(uint8_t flag_set, uint8_t  *enable);

/**
* @brief Get ip mac port entry List
*
* @param [OUT] number
* @param [OUT] entry_list
*
* @return 0 means success, > 0 means fail
*/
int32_t wifi_inband_get_ip_mac_port_list_entry(uint8_t *number, ip_match_entry_t *entry_list);

/**
* @brief set ip mac port entry List
*
* @param [IN] action
* @param [OUT] entry_list
*
* @return 0 means success, > 0 means fail
*/
int32_t wifi_inband_set_ip_mac_port_list_entry(uint8_t action,uint8_t *pui1_index,ip_match_entry_t *entry);

/****************************************************************************/

/**
 * Defines the macro for EFUSE address space.
 */
typedef enum {
    EFUSE_ADDR_SPACE_LOGICAL  = 0,
    EFUSE_ADDR_SPACE_PHYSICAL = 1
} efuse_address_space_t;


#define EFUSE_WRITE_MAGIC   (0xFEEDBABE)


/**
 * get the number of available blocks in eFuse.
 */
int32_t wifi_inband_get_efuse_free(uint32_t *free_blocks);

/**
 * Read eFuse.
 */
int32_t wifi_inband_get_efuse(efuse_address_space_t space,
                              uint16_t              address,
                              uint8_t               *buffer);

/**
 * magic number has to be EFUSE_WRITE_MAGIC.
 */
int32_t wifi_inband_set_efuse(efuse_address_space_t space,
                              uint16_t              address,
                              const uint8_t         *buffer,
                              uint8_t               len,
                              uint32_t              magic);


/**
 * query the needed blocks for writing eFuse content.
 */
int32_t wifi_inband_query_efuse(efuse_address_space_t space,
                                uint16_t              address,
                                const uint8_t         *buffer,
                                uint8_t               len,
                                uint8_t               *ok,
                                uint8_t               *used,
                                uint8_t               *noop,
                                uint8_t               *reuse);

/****************************************************************************/

int32_t wifi_inband_tx_raw(uint8_t *payload, uint32_t len, txd_param_t *txd_param, uint8_t need_tx_status);

typedef enum wifi_inband_packet_type_e {
    WIFI_BEACON_PACKET_TYPE_ID,                 /**< beacon                     */
    WIFI_PROBE_REQUEST_PACKET_TYPE_ID,          /**< probe request              */
    WIFI_PROBE_RESPONSE_PACKET_TYPE_ID,         /**< probe response             */
    WIFI_ASSOCIATION_REQUEST_PACKET_TYPE_ID,    /**< assoication request        */
    WIFI_ASSOCIATION_RESPONSE_PACKET_TYPE_ID,   /**< clear assoication response */
    WIFI_PACKET_TYPE_CLEAR_ALL_ID               /**< clear all packets          */
} wifi_inband_packet_type_t;

typedef enum wifi_inband_port_id_e {
    WIFI_INBAND_APCLI_STA_PORT = 0,
    WIFI_INBAND_AP_PORT
} wifi_inband_port_id_t;

int32_t wifi_inband_set_ie(wifi_inband_packet_type_t   packet_type,
                           wifi_inband_port_id_t       port,
                           uint16_t                    ie_length,
                           uint8_t                     *ie_body);

int32_t wifi_inband_wow_enable(const wifi_wow_enable_t *param);

int32_t wifi_inband_wow_set_magic_packet(const wifi_wow_magic_packet_t *param);

int32_t wifi_inband_wow_set_bitmap_pattern(const wifi_wow_bitmap_pattern_t *param);

int32_t wifi_inband_wow_set_arp_offload(const wifi_wow_arp_offload_t *param);

int32_t wifi_inband_wow_set_rekey_offload(const wifi_wow_rekey_offload_t *param);


/**
 * Customized SCAN behavior. Normally, wifi_inband_start_scan should be used.
 *
 * @param param     the scan parameter.
 * @param param_len the length of data in param.
 * @note param is typed as void buffer to decouple exported inband API from
 *       the details of scan. To use this API, one must include wifi_scan.h
 *       and use type:
 */
int32_t wifi_inband_scan_custom(void *param, size_t param_len);


int32_t wifi_inband_set_n9_consol_log_state(uint8_t n9_consol_log_state);


int32_t wifi_inband_set_rx_vector_stat(uint8_t enable);


int32_t wifi_inband_get_rx_vector_stat(uint8_t *enable);


int32_t wifi_5g_support(void);

/**
* @brief Get/Set WiFi Tx Power
*
* @param [IN]flag_set 1:set;  0:get.
* @param [IN/OUT]interval 64 ~ 190, instead of -315~315 set into chip
*
*/
int32_t wifi_inband_tx_power(uint8_t flag_set, uint8_t *power);

int32_t wifi_inband_bandwidth_extended(uint8_t flag_set, uint8_t port, uint8_t *bandwidth, uint8_t *below_above_ch);

int32_t wifi_inband_set_wifi_privilege(uint8_t privilege_switch, uint8_t driver_state);

int32_t wifi_inband_frame_type_filiter(uint8_t filter_switch, uint16_t frame_type, extra_vender_ie_t *vender_ie);

int32_t wifi_inband_dwell_time(uint8_t set_flag, uint8_t *scan_option, uint8_t *dwell_time);

#ifdef MTK_SINGLE_SKU_SUPPORT
int32_t wifi_inband_single_sku(uint8_t flag_set, uint8_t band, void * power_table);
#endif

#ifdef MTK_ANT_DIV_ENABLE
int32_t wifi_inband_antenna_status(uint8_t flag_set, wifi_antenna_status_t * ant_status);
int32_t wifi_inband_antenna_monitor(uint8_t flag_set);
#endif

int32_t wifi_inband_get_wlan_statistics(wifi_inband_statistic_t *wifi_inband_statistics);

#ifdef __cplusplus
}
#endif

#endif /* WIFI_API */
