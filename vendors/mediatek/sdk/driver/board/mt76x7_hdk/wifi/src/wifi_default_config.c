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

#include "os.h"
#include "wifi_api.h"
#include "wifi_default_config.h"
#include "syslog.h"
#include "get_profile_string.h"
#include "wifi_init.h"

int32_t wifi_get_default_config(sys_cfg_t *syscfg)
{
    if (NULL == syscfg) {
        LOG_E(wifi, "syscfg is null");
        return -1;
    }

    syscfg->opmode = WIFI_MODE_MONITOR;
    syscfg->country_region = WIFI_DEFAULT_COUNTRY_REGION;
    syscfg->country_region_a_band = WIFI_DEFAULT_COUNTRY_REGION_A_BAND;
    os_strncpy((char *)syscfg->country_code, WIFI_DEFAULT_COUNTRY_CODE, sizeof(syscfg->country_code));
    wifi_country_code_region_mapping(syscfg->country_code, &(syscfg->country_region), &(syscfg->country_region_a_band));
    syscfg->radio_off = 0;
    syscfg->dbg_level = WIFI_DEFAULT_N9_DEBUG_LEVEL;
    syscfg->rts_threshold = WIFI_DEFAULT_RTS_THRESHOLD;
    syscfg->frag_threshold = WIFI_DEFAULT_FRAGMENT_THRESHOLD;

    syscfg->sta_local_admin_mac = 1;
    wifi_conf_get_ip_from_str(syscfg->sta_ip_addr, "192.168.1.1");
    if (0 > wifi_config_get_mac_address(WIFI_PORT_STA, syscfg->sta_mac_addr)) {
        LOG_W(wifi, "wifi_config_get_mac_address fail.");
        return -1;
    }
    os_strncpy((char *)syscfg->sta_ssid, "MTK_SOFT_AP", sizeof(syscfg->sta_ssid));
    syscfg->sta_ssid_len = os_strlen("MTK_SOFT_AP");
    syscfg->sta_bss_type = WIFI_DEFAULT_STA_BSS_TYPE;
    syscfg->sta_channel = WIFI_DEFAULT_STA_CHANNEL;
    syscfg->sta_bw = WIFI_DEFAULT_STA_BANDWIDTH;
    syscfg->sta_wireless_mode = WIFI_DEFAULT_STA_WIRELESS_MODE;
    syscfg->sta_ba_decline = WIFI_DEFAULT_STA_BA_DECLINE;
    syscfg->sta_auto_ba = WIFI_DEFAULT_STA_AUTO_BA;
    syscfg->sta_ht_mcs = WIFI_DEFAULT_STA_HT_MCS;
    syscfg->sta_ht_ba_win_size = WIFI_DEFAULT_STA_HT_BA_WINDOW_SIZE;
    syscfg->sta_ht_gi = WIFI_DEFAULT_STA_HT_GI;
    syscfg->sta_ht_protect = WIFI_DEFAULT_STA_HT_PROTECT;
    syscfg->sta_ht_ext_ch = WIFI_DEFAULT_STA_HT_EXT_CHANNEL;
    syscfg->sta_wmm_capable = WIFI_DEFAULT_STA_WMM_CAPABLE;
    syscfg->sta_listen_interval = WIFI_DEFAULT_STA_LISTEN_INTERVAL;
    syscfg->sta_auth_mode = WIFI_AUTH_MODE_WPA_PSK_WPA2_PSK;
    syscfg->sta_encryp_type = WIFI_ENCRYPT_TYPE_TKIP_AES_MIX;
    os_strncpy((char *)syscfg->sta_wpa_psk, "12345678", sizeof(syscfg->sta_wpa_psk));
    syscfg->sta_wpa_psk_len = os_strlen("12345678");
    os_strncpy((char *)syscfg->sta_pmk, "", sizeof(syscfg->sta_pmk));
    syscfg->sta_pair_cipher = 0;
    syscfg->sta_group_cipher = 0;
    syscfg->sta_default_key_id = 0;
    syscfg->sta_ps_mode = wifi_get_ps_mode();
    syscfg->sta_keep_alive_period = WIFI_DEFAULT_STA_KEEP_ALIVE_PERIOD;

    syscfg->ap_local_admin_mac = 1;
    wifi_conf_get_ip_from_str(syscfg->ap_ip_addr, "192.168.1.1");
    if (0 > wifi_config_get_mac_address(WIFI_PORT_AP, syscfg->ap_mac_addr)) {
        LOG_W(wifi, "wifi_config_get_mac_address fail.");
        return -1;
    }
    os_strncpy((char *)syscfg->ap_ssid, "MTK_SOFT_AP", sizeof(syscfg->ap_ssid));
    syscfg->ap_ssid_len = os_strlen("MTK_SOFT_AP");
    syscfg->ap_channel = WIFI_DEFAULT_AP_CHANNEL;
    syscfg->ap_bw = WIFI_DEFAULT_AP_BANDWIDTH;
    syscfg->ap_wireless_mode = WIFI_DEFAULT_AP_WIRELESS_MODE;
    syscfg->ap_auto_ba = WIFI_DEFAULT_AP_AUTO_BA;
    syscfg->ap_ht_mcs = WIFI_DEFAULT_AP_HT_MCS;
    syscfg->ap_ht_ba_win_size = WIFI_DEFAULT_AP_HT_BA_WINDOW_SIZE;
    syscfg->ap_ht_gi = WIFI_DEFAULT_AP_HT_GI;
    syscfg->ap_ht_protect = WIFI_DEFAULT_AP_HT_PROTECT;
    syscfg->ap_ht_ext_ch = WIFI_DEFAULT_AP_HT_EXT_CHANNEL;
    syscfg->ap_wmm_capable = WIFI_DEFAULT_AP_WMM_CAPABLE;
    syscfg->ap_dtim_period = WIFI_DEFAULT_AP_DTIM_PERIOD;
    syscfg->ap_hide_ssid = WIFI_DEFAULT_AP_HIDDEN_SSID;
    syscfg->ap_auto_channel_select = WIFI_DEFAULT_AP_AUTO_CHANNEL_SELECT;
    syscfg->ap_auth_mode = WIFI_AUTH_MODE_WPA_PSK_WPA2_PSK;
    syscfg->ap_encryp_type = WIFI_ENCRYPT_TYPE_TKIP_AES_MIX;
    os_strncpy((char *)syscfg->ap_wpa_psk, "12345678", sizeof(syscfg->ap_wpa_psk));
    syscfg->ap_wpa_psk_len = os_strlen("12345678");
    os_strncpy((char *)syscfg->ap_pmk, "", sizeof(syscfg->ap_pmk));
    syscfg->ap_pair_cipher = 0;
    syscfg->ap_group_cipher = 0;
    syscfg->ap_default_key_id = 0;
    syscfg->ap_beacon_disable = WIFI_DEFAULT_AP_BEACON_DISABLE;
    syscfg->forwarding_zero_copy = 1;

    syscfg->bg_band_entry_num = wifi_conf_get_ch_table_from_str(syscfg->bg_band_triple,
                                                                10, WIFI_DEFAULT_BG_CHANNEL_TABLE,
                                                                os_strlen(WIFI_DEFAULT_BG_CHANNEL_TABLE));

    syscfg->a_band_entry_num = wifi_conf_get_ch_table_from_str(syscfg->a_band_triple,
                                                               10, WIFI_DEFAULT_A_CHANNEL_TABLE,
                                                               os_strlen(WIFI_DEFAULT_A_CHANNEL_TABLE));
    syscfg->mbss_enable = WIFI_DEFAULT_MBSS_ENABLE;
    os_strncpy((char *)syscfg->mbss_ssid1, WIFI_DEFAULT_MBSS_SSID1, sizeof(syscfg->mbss_ssid1));
    syscfg->mbss_ssid_len1 = os_strlen(WIFI_DEFAULT_MBSS_SSID1);
    os_strncpy((char *)syscfg->mbss_ssid2, WIFI_DEFAULT_MBSS_SSID2, sizeof(syscfg->mbss_ssid2));
    syscfg->mbss_ssid_len2 = os_strlen(WIFI_DEFAULT_MBSS_SSID2);

    syscfg->config_free_ready = 0;
    syscfg->config_free_enable = WIFI_DEFAULT_CONFIG_FREE_ENABLE;
    syscfg->sta_fast_link = WIFI_DEFAULT_STA_FAST_LINK;
    syscfg->beacon_lost_time = WIFI_DEFAULT_BEACON_LOST_TIME;
    syscfg->apcli_40mhz_auto_upbelow = WIFI_DEFAULT_APCLI_BW_AUTO_UP_BELOW;
    syscfg->wifi_privilege_enable = WIFI_DEFAULT_WIFI_PRIVILEGE_ENABLE;

#ifdef MTK_SINGLE_SKU_SUPPORT
    syscfg->support_single_sku = WIFI_DEFAULT_SUPPORT_SINGLE_SKU;
    wifi_conf_get_pwr_from_str(&syscfg->single_sku_2g, WIFI_DEFAULT_SINGLESKU2G);
#endif

#ifdef MTK_ANT_DIV_ENABLE
    syscfg->antenna_number = WIFI_DEFAULT_ANT_NUMBER;
    syscfg->antenna_diversity_enable= WIFI_DEFAULT_ANT_DIVERSITY_ENABLE;
    syscfg->antenna_probe_req_count= WIFI_DEFAULT_ANT_PROB_REQ_COUNT;
    syscfg->antenna_threshold_level= WIFI_DEFAULT_ANT_THRESHOLD_LEVEL;
    syscfg->antenna_delta_rssi= WIFI_DEFAULT_ANT_DELTA_RSSI;
    syscfg->antenna_rssi_switch= WIFI_DEFAULT_ANT_RSSI_SWITCH;
    syscfg->antenna_pre_selected_rssi= WIFI_DEFAULT_ANT_PRE_SELECTED_RSSI;
#endif

    return 0;
}

