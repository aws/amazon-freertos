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

#ifndef __WIFI_PRIVATE_H__
#define __WIFI_PRIVATE_H__

#include "stdint.h"
#include "type_def.h"
#include "wifi_api.h"
#include "wifi_inband.h"


#ifdef __cplusplus
extern "C" {
#endif

// Pengfei.Qiu  20160504  Only for internal use
// Need do committee and TC/TW review if put them into common SDK
/**
* @brief This function sets the bandwidth configuration that the WIFI driver uses for a specific wireless port.
* This operation takes effect immediately.
*
* @param[in] port indicates the WIFI port that the function will operate on.
*
* Value                         |Definition                                                              |
* ------------------------------|------------------------------------------------------------------------|
* \b #WIFI_PORT_STA             | STA|
* \b #WIFI_PORT_APCLI           | AP Client|
* \b #WIFI_PORT_AP              | AP|
*
* @param[out]  bandwidth is the wireless bandwidth.
*
* Value                                                |Definition                                                                 |
* -----------------------------------------------------|-------------------------------------------------|
* \b #WIFI_IOT_COMMAND_CONFIG_BANDWIDTH_20MHZ          | 20MHz |
* \b #WIFI_IOT_COMMAND_CONFIG_BANDWIDTH_40MHZ          | 40MHz |
* \b #WIFI_IOT_COMMAND_CONFIG_BANDWIDTH_2040MHZ        | 20 or 40MHz |
*
* @param[out]  below_above_ch is the primarychannel of bandwidth.
*
* Value                                     |Definition                                                                 |
* ------------------------------------------|-------------------------------------------------|
* \b 0                                      | Extended channel None  |
* \b 1                                      | Extended Channel Above |
* \b 3                                      | Extended channel Below |
*
*
* @return  >=0 the operation completed successfully, <0 the operation failed.
*
*/
int32_t wifi_config_get_bandwidth_extended(uint8_t port, uint8_t *bandwidth, uint8_t *below_above_ch);

/**
* @brief This function sets the bandwidth configuration that the WIFI driver uses for a specific wireless port.
* This operation takes effect immediately.
*
* @param[in] port indicates the WIFI port that the function will operate on.
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
* @param[in]  below_above_ch is the primarychannel of bandwidth.
*
* Value                                     |Definition                                                                 |
* ------------------------------------------|-------------------------------------------------|
* \b 0                                      | Extended channel None  |
* \b 1                                      | Extended Channel Above |
* \b 3                                      | Extended channel Below |
*
*
* @return  >=0 the operation completed successfully, <0 the operation failed.
*
*/
int32_t wifi_config_set_bandwidth_extended(uint8_t port, uint8_t bandwidth, uint8_t below_above_ch);

int32_t wifi_config_set_frame_filter(uint8_t filter_switch, uint16_t frame_type, extra_vender_ie_t *vender_ie);

int32_t wifi_config_get_rssi_threshold(uint8_t *enable, int8_t *rssi);

int32_t wifi_config_set_rssi_threshold(uint8_t enable, int8_t rssi);

/**
* @brief  The objective of the  API  is to keep CM4 in low power and not be wake up
*         except get a specific IP packet which match the entry of the ip mac port list.
*
*         used to enable or disable the ip mac port filter feature. When the filter is enabled,  cm4 will
*         be wake up if receive the IP packet which match the entry set by #wifi_config_set_ip_mac_port_list_entry
*
* @param [IN] flag
*                1  enable
*                0  disable
*
* @return  >=0 means success, <0 means fail
*
*/
int32_t wifi_config_set_ip_mac_port_filter(uint8_t flag);

/**
* @brief get the status of the ip mac port filter
*
* @param [OUT] flag
*
* @return  >=0 means success, <0 means fail
*
*/
int32_t wifi_config_get_ip_mac_port_filter(uint8_t *flag);


/**
* @brief add or delete an entry of the ip mac port filter, cm4 will be wake up if receive the IP packet which match the entry
*
* @param [IN] action
*               1 create
*               0 delete
* @param [IN]  entry please refer to #ip_match_entry_t
*
* @return  >=0 means success, <0 means fail
*
*/
int32_t wifi_config_set_ip_mac_port_list_entry(uint8_t action,ip_match_entry_t *entry);


/**
* @brief get the number of entry and the entry itself
*
* @param [OUT] entry_num
* @param [OUT] entry
*
* @return  >=0 means success, <0 means fail
*
*/
int32_t wifi_config_get_ip_mac_port_list_entry(uint8_t *entry_num ,ip_match_entry_t *entry);

/**
* @brief This function gets the tx power from hardware configuration.
*The hardware configuration tx power value read back is in a range from 64 to 190.
*
*@param[out]  power is in a range from 64 to 190 which is the hardware configuration.
*                     which means set -315 ~ 315 to the chip
*
* @return  >=0 the operation completed successfully, <0 the operation failed.
*
*/
int32_t wifi_config_get_tx_power(uint8_t *power);

/**
* @brief This function sets the tx power to hardware configuration.
*
* @param[in]  power is in a range from 64 to 190 that means chip configured -315 ~ 315
*
* @return  >=0 the operation completed successfully, <0 the operation failed.
*
*/
int32_t wifi_config_set_tx_power(uint8_t power);

/**
* @brief This function is used to set dwell time for scan, there are two modes scan we support, active and passive.
*
* @param[in] scan_mode 0: active, 1: passive
* @param[in] dwell_time
*
* @return  >=0 the operation completed successfully, <0 the operation failed.
*
*/
int32_t wifi_config_set_dwell_time(uint8_t *scan_option, uint8_t *dwell_time);

int32_t wifi_config_get_dwell_time(uint8_t *scan_option, uint8_t *dwell_time);

typedef int32_t (*N9LOG_HANDLER)(unsigned char *body, int len);
int32_t wifi_config_register_n9log_handler(N9LOG_HANDLER handler);
N9LOG_HANDLER wifi_config_get_n9log_handler(unsigned char *body, int len);
uint32_t wifi_config_get_scan_channel_number(void);

/**
* @brief This function is called to inform the lower layer that the IP address is ready.
*           This is a private API , which used to inform IP is ready to wifi driver
*           In present, WiFi Driver will do some operation when this API is invoked, such as:
*           Do WiFi&BLE Coexstence relative behavior if BLE is enabled and do Power Saving Status change.
*           This API will be improved, user may need to use new API to replace it in future
** @return  >=0 the operation completed successfully, <0 the operation failed.
*/
int32_t wifi_connection_inform_ip_ready(void);

/**
* @brief This function is called when STA got IP address.
*           This is a private API , which used to unlock tickless, then system can goto sleep.
** @return  >=0 the operation completed successfully, <0 the operation failed.
*/
int32_t wifi_connection_unlock_got_ip(void);

uint8_t wifi_config_get_n9_auto_scan_status(void);

void wifi_config_set_n9_auto_scan_status(uint8_t status);


#ifdef __cplusplus
}
#endif

#endif /* WIFI_API */

