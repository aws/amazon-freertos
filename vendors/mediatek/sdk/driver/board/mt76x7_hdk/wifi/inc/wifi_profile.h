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

#include <stdint.h>
#include <stdio.h>

int32_t wifi_get_channel_list(uint8_t band, uint8_t region, char *channel_list);


bool wifi_is_opmode_valid(uint8_t mode);

bool wifi_is_port_valid(uint8_t port);

bool wifi_is_band_valid(uint8_t band);

bool wifi_is_bandwidth_valid(uint8_t bandwidth);

bool wifi_is_auth_mode_valid(uint8_t auth_mode);

bool wifi_is_encrypt_type_valid(uint8_t encrypt_type);

bool wifi_is_ps_mode_valid(uint8_t ps_mode);

int32_t wifi_profile_set_opmode(uint8_t mode);

#ifdef MTK_WIFI_PROFILE_ENABLE 
int32_t wifi_profile_get_opmode(uint8_t *mode);

int32_t wifi_profile_set_channel(uint8_t port, uint8_t channel);

int32_t wifi_profile_get_channel(uint8_t port, uint8_t *channel);

int32_t wifi_profile_set_bandwidth(uint8_t port, uint8_t bandwidth);

int32_t wifi_profile_get_bandwidth(uint8_t port, uint8_t *bandwidth);

int32_t wifi_profile_get_mac_address(uint8_t port, uint8_t *address);

int32_t wifi_profile_set_mac_address(uint8_t port, uint8_t *address);

int32_t wifi_profile_set_ssid(uint8_t port, uint8_t *ssid , uint8_t ssid_length);

int32_t wifi_profile_get_ssid(uint8_t port, uint8_t *ssid, uint8_t *ssid_length);

int32_t wifi_profile_commit_setting(char *profile_section);

int32_t wifi_profile_get_profile(uint8_t port, wifi_profile_t *profile);

int32_t wifi_profile_set_wireless_mode(uint8_t port, wifi_phy_mode_t mode);

int32_t wifi_profile_get_wireless_mode(uint8_t port, wifi_phy_mode_t *mode);

int32_t wifi_profile_set_security_mode(uint8_t port, wifi_auth_mode_t auth_mode, wifi_encrypt_type_t encrypt_type);

int32_t wifi_profile_get_security_mode(uint8_t port, wifi_auth_mode_t *auth_mode, wifi_encrypt_type_t *encrypt_type);

int32_t wifi_profile_set_wpa_psk_key(uint8_t port, uint8_t *passphrase, uint8_t passphrase_length);

int32_t wifi_profile_get_wpa_psk_key(uint8_t port, uint8_t *passphrase, uint8_t *passphrase_length);

int32_t wifi_profile_set_pmk(uint8_t port, uint8_t *pmk);

int32_t wifi_profile_get_pmk(uint8_t port, uint8_t *pmk);

int32_t wifi_profile_set_wep_key(uint8_t port, wifi_wep_key_t *wep_keys);

int32_t wifi_profile_get_wep_key(uint8_t port, wifi_wep_key_t *wep_keys);

int32_t wifi_profile_get_country_region(uint8_t band, uint8_t *region);

int32_t wifi_profile_set_country_region(uint8_t band, uint8_t region);

int32_t wifi_profile_get_dtim_interval(uint8_t *interval);

int32_t wifi_profile_set_dtim_interval(uint8_t interval);

int32_t wifi_profile_get_listen_interval(uint8_t *interval);

int32_t wifi_profile_set_listen_interval(uint8_t interval);

int32_t wifi_profile_get_power_save_mode(uint8_t *power_save_mode);

int32_t wifi_profile_set_power_save_mode(uint8_t power_save_mode);

int32_t wifi_profile_get_configfree(uint8_t *config_ready);

int32_t wifi_profile_set_configfree(uint8_t config_ready);
#endif //MTK_WIFI_PROFILE_ENABLE

