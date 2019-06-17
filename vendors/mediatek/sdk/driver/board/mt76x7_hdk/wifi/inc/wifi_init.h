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

#ifndef __WIFI_INIT_H__
#define __WIFI_INIT_H__

#include "wifi_api.h"
#include "connsys_profile.h"
#include "wifi_inband.h"//wifi_antenna_status_t in this head currently

typedef struct {
    int32_t (* single_sku_2g)(wifi_config_advance_type_t profile_type, wifi_config_advance_t * profile_data);
    int32_t (* sta_fast_link_enable)(wifi_config_advance_type_t profile_type, wifi_config_advance_t * profile_data);
    int32_t (* bg_channel_table)(wifi_config_advance_type_t profile_type, wifi_config_advance_t * profile_data);
#ifdef MTK_ANT_DIV_ENABLE
    int32_t (* wifi_rf_diversity)(wifi_config_advance_type_t profile_type, wifi_config_advance_t * profile_data);
#endif
} wifi_init_config_handler_struct_t;

/*use the wifi_init_callback,  to make the modules independence */
typedef struct __wifi_init_callback {
#if 1//def MTK_ANT_DIV_ENABLE
    int32_t (*wifi_record_ant_status)(wifi_antenna_status_t * ant_status);
#endif
#if 1//def MTK_SINGLE_SKU_SUPPORT
    void (*wifi_get_single_sku)(singleSKU2G_t * single, uint8_t group_number);
#endif
    int32_t (*wifi_record_pmk_info)(uint8_t *ssid, uint8_t ssid_length, uint8_t *passphrase, uint8_t passphrase_len, uint8_t *psk);
} wifi_init_callback;

void wifi_scan_init(wifi_config_t *config);
uint8_t wifi_get_ps_mode(void);
bool wifi_get_security_valid(void);
void wifi_set_security_valid(bool value);

uint8_t wifi_set_sleep_handle(const char *handle_name);
int32_t wifi_lock_sleep(uint8_t handle_index);
int32_t wifi_unlock_sleep(uint8_t handle_index);
int32_t wifi_unlock_sleep_all(void);
int32_t wifi_release_sleep_handle(uint8_t handle);
wifi_phy_mode_t wifi_change_wireless_mode_5g_to_2g(wifi_phy_mode_t wirelessmode);
int32_t wifi_sleep_manager_get_lock_status(void);
int32_t record_2G_5G_channel_list(uint8_t BGband_region, uint8_t Aband_region);
int32_t wifi_country_code_region_mapping(uint8_t *country_code, uint8_t *bg_band_region, uint8_t *a_band_region);

#ifdef MTK_SINGLE_SKU_SUPPORT
void wifi_get_single_sku_from_nvdm(singleSKU2G_t * single, uint8_t group_number);
#endif
int32_t wifi_profile_auto_record_pmk_info(uint8_t *ssid, uint8_t ssid_length, uint8_t *passphrase, uint8_t passphrase_len, uint8_t *psk);

void wifi_connect(void *param);

#endif /* __WIFI_INIT_H__ */

