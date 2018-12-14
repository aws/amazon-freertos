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
#include "connsys_driver.h"
#include "wifi_scan.h"
#include "inband_queue.h"
#include "net_task.h"
#include "wifi_init.h"
#include "syslog.h"
#include "wifi_profile.h"
#include "get_profile_string.h"
#include "wifi_default_config.h"
#include "wifi_inband.h"
#include "wpa_supplicant_task.h"
#include "hal_efuse.h"
#include "wifi_channel.h"
#include "wifi_os_api.h"

#ifdef MTK_ANT_DIV_ENABLE
#include "wifi_ant_div.h"
#endif

//log_create_module(wifi, PRINT_LEVEL_ERROR);

/*Add the wifi_init_callback,  to avoid the modules dependency,
   some of callback will be invoked in the wifi driver which located in the src_protected path */
wifi_init_callback g_wifi_init_callback;

static void wifi_init_register_callback()
{
#ifdef MTK_ANT_DIV_ENABLE
    g_wifi_init_callback.wifi_record_ant_status = wifi_profile_record_antenna_status;
#endif
#ifdef MTK_SINGLE_SKU_SUPPORT
    g_wifi_init_callback.wifi_get_single_sku = wifi_get_single_sku_from_nvdm;
#endif
    g_wifi_init_callback.wifi_record_pmk_info = wifi_profile_auto_record_pmk_info;

}

static int32_t wifi_is_mac_address_valid(uint8_t *mac_addr)
{
    uint32_t byte_sum = 0;
    for (uint32_t index = 0; index < WIFI_MAC_ADDRESS_LENGTH; index++) {
        byte_sum += mac_addr[index];
    }
    return (byte_sum != 0);
}

wifi_phy_mode_t wifi_change_wireless_mode_5g_to_2g(wifi_phy_mode_t wirelessmode)
{
    if (WIFI_PHY_11A == wirelessmode) {
        return WIFI_PHY_11B;
    } else if (WIFI_PHY_11ABG_MIXED == wirelessmode) {
        return WIFI_PHY_11BG_MIXED;
    } else if (WIFI_PHY_11ABGN_MIXED == wirelessmode) {
        return WIFI_PHY_11BGN_MIXED;
    } else if (WIFI_PHY_11AN_MIXED == wirelessmode) {
        return WIFI_PHY_11N_2_4G;
    } else if (WIFI_PHY_11AGN_MIXED == wirelessmode) {
        return WIFI_PHY_11GN_MIXED;
    } else if (WIFI_PHY_11N_5G == wirelessmode) {
        return WIFI_PHY_11N_2_4G;
    } else {
        return wirelessmode;
    }
}

/**
* @brief get mac address from efuse
*/
static int32_t wifi_get_mac_addr_from_efuse(uint8_t port, uint8_t *mac_addr)
{
    uint8_t buf[16] = {0};//efuse is 16 byte aligned
    uint16_t mac_offset = 0x00;//mac addr offset in efuse
    if (HAL_EFUSE_OK != hal_efuse_read(mac_offset, buf, sizeof(buf))) {
        return -1;
    }
    if (!wifi_is_mac_address_valid(buf+4)) {
        LOG_HEXDUMP_W(wifi, "data in efuse is invalid", buf, sizeof(buf));
        return -1;
    }
    if (WIFI_PORT_STA == port) {
        /* original efuse MAC address for STA */
        os_memcpy(mac_addr, buf+4, WIFI_MAC_ADDRESS_LENGTH);
    } else {
        /* original efuse MAC address with byte[5]+1 for AP */
        os_memcpy(mac_addr, buf+4, WIFI_MAC_ADDRESS_LENGTH);
        mac_addr[WIFI_MAC_ADDRESS_LENGTH-1] += 1;
    }
    return 0;
}


#ifdef MTK_NVDM_ENABLE
/**
* @brief get mac address from nvdm
*/
static int32_t wifi_get_mac_addr_from_nvdm(uint8_t port, uint8_t *mac_addr)
{
    uint8_t buff[PROFILE_BUF_LEN] = {0};
    uint32_t len = sizeof(buff);
    char *group_name = (WIFI_PORT_STA == port) ? "STA" : "AP";

    if (NVDM_STATUS_OK != nvdm_read_data_item(group_name, "MacAddr", buff, &len)) {
        return -1;
    }

    wifi_conf_get_mac_from_str((char *)mac_addr, (char *)buff);
    return 0;
}
#endif

/**
* @brief Get WiFi Interface MAC Address.
*
*/
int32_t wifi_config_get_mac_address(uint8_t port, uint8_t *address)
{
    LOG_W(wifi, "wifi_config_get_mac_address for port %s\n", port == WIFI_PORT_STA ? "sta/apcli" : "ap" );

    if (NULL == address) {
        LOG_E(wifi, "address is null.");
        return WIFI_ERR_PARA_INVALID;
    }

    if (!wifi_is_port_valid(port)) {
        LOG_E(wifi, "port is invalid: %d", port);
        return WIFI_ERR_PARA_INVALID;
    }

    if (0 == wifi_get_mac_addr_from_efuse(port, address)) {
        LOG_W(wifi, "address %02x:%02x:%02x:%02x:%02x:%02x\n",
                     address[0] & 0xFF,
                     address[1] & 0xFF,
                     address[2] & 0xFF,
                     address[3] & 0xFF,
                     address[4] & 0xFF,
                     address[5] & 0xFF);
        return 0;
    } else {
        LOG_W(wifi, "wifi_get_mac_addr_from_efuse fail.\n");
    }

#ifdef MTK_NVDM_ENABLE
    if (0 == wifi_get_mac_addr_from_nvdm(port, address)) {
        return 0;
    }
    LOG_E(wifi, "wifi_get_mac_addr_from_nvdm fail.");

    return -1;
#else
    address[0] = 0x00;
    address[1] = 0x0C;
    address[2] = 0xE7;
    address[3] = 0x62;
    address[4] = 0x06;
    address[5] = 0x20;

    LOG_W(wifi, "address %02x:%02x:%02x:%02x:%02x:%02x\n",
                 address[0] & 0xFF,
                 address[1] & 0xFF,
                 address[2] & 0xFF,
                 address[3] & 0xFF,
                 address[4] & 0xFF,
                 address[5] & 0xFF);
    return 0;
#endif
}

uint8_t wifi_get_ps_mode(void)
{
#ifdef MTK_WIFI_PROFILE_ENABLE
    uint8_t buff[PROFILE_BUF_LEN];
    uint32_t len = sizeof(buff);
    nvdm_read_data_item("STA", "PSMode", buff, &len);
    return (uint8_t)atoi((char *)buff);
#else
    return WIFI_DEFAULT_STA_POWER_SAVE_MODE;
#endif
}

static void wifi_save_sta_ext_config(sys_cfg_t *syscfg, wifi_config_ext_t *config_ext)
{
    if (NULL != config_ext) {
        if (config_ext->sta_wep_key_index_present) {
            syscfg->sta_default_key_id = config_ext->sta_wep_key_index;
        }
        if (config_ext->sta_bandwidth_present) {
            syscfg->sta_bw = config_ext->sta_bandwidth;
        }
        if (config_ext->sta_wireless_mode_present) {
            syscfg->sta_wireless_mode = config_ext->sta_wireless_mode;
        }
        if (config_ext->sta_listen_interval_present) {
            syscfg->sta_listen_interval = config_ext->sta_listen_interval;
        }
        if (config_ext->sta_power_save_mode_present) {
            syscfg->sta_ps_mode = config_ext->sta_power_save_mode;
        }
    }
}

static void wifi_save_ap_ext_config(sys_cfg_t *syscfg, wifi_config_ext_t *config_ext)
{
    if (NULL != config_ext) {
        if (config_ext->ap_wep_key_index_present) {
            syscfg->ap_default_key_id = config_ext->ap_wep_key_index;
        }
        if (config_ext->ap_hidden_ssid_enable_present) {
            syscfg->ap_hide_ssid = config_ext->ap_hidden_ssid_enable;
        }
        if (config_ext->ap_wireless_mode_present) {
            syscfg->ap_wireless_mode = config_ext->ap_wireless_mode;
        }
        if (config_ext->ap_dtim_interval_present) {
            syscfg->ap_dtim_period = config_ext->ap_dtim_interval;
        }
    }
}

static void wifi_apply_sta_config(sys_cfg_t *syscfg, wifi_config_t *config, wifi_config_ext_t *config_ext)
{
    os_memcpy(syscfg->sta_ssid, config->sta_config.ssid, WIFI_MAX_LENGTH_OF_SSID);

    syscfg->sta_ssid_len = config->sta_config.ssid_length;

    os_memcpy(syscfg->sta_wpa_psk, config->sta_config.password, WIFI_LENGTH_PASSPHRASE);

    syscfg->sta_wpa_psk_len = config->sta_config.password_length;

    /* save extension config */
    wifi_save_sta_ext_config(syscfg, config_ext);
}

static void wifi_apply_ap_config(sys_cfg_t *syscfg, wifi_config_t *config, wifi_config_ext_t *config_ext)
{
    os_memcpy(syscfg->ap_ssid, config->ap_config.ssid, WIFI_MAX_LENGTH_OF_SSID);

    syscfg->ap_ssid_len = config->ap_config.ssid_length;

    os_memcpy(syscfg->ap_wpa_psk, config->ap_config.password, WIFI_LENGTH_PASSPHRASE);

    syscfg->ap_wpa_psk_len = config->ap_config.password_length;

    syscfg->ap_auth_mode = config->ap_config.auth_mode;

    syscfg->ap_encryp_type = config->ap_config.encrypt_type;

    syscfg->ap_channel = config->ap_config.channel;

    syscfg->ap_bw = config->ap_config.bandwidth;

    syscfg->ap_ht_ext_ch = (WIFI_BANDWIDTH_EXT_40MHZ_UP == config->ap_config.bandwidth_ext) ? 1 : 3;

    /* save extension config */
    wifi_save_ap_ext_config(syscfg, config_ext);
}

static void wifi_apply_repeater_config(sys_cfg_t *syscfg, wifi_config_t *config, wifi_config_ext_t *config_ext)
{
    wifi_apply_sta_config(syscfg, config, config_ext);
    wifi_apply_ap_config(syscfg, config, config_ext);
}

static void wifi_apply_p2p_config(sys_cfg_t *syscfg, wifi_config_t *config, wifi_config_ext_t *config_ext)
{
    /* TBD */
}

#ifdef MTK_SINGLE_SKU_SUPPORT
/*need improve,  if the NVDM is disable in project,  it will build error here!*/
void wifi_get_single_sku_from_nvdm(singleSKU2G_t * single, uint8_t group_number)
{
    uint8_t buff_ext[1024];
    uint32_t len = sizeof(buff_ext);
    
    (void)len;
    
#ifdef MTK_NVDM_ENABLE

    switch(group_number) {
        case 0:
            nvdm_read_data_item("common", "SingleSKU2G_0", buff_ext, &len);
            break;
        case 1:
            nvdm_read_data_item("common", "SingleSKU2G_1", buff_ext, &len);
            break;
        case 2:
            nvdm_read_data_item("common", "SingleSKU2G_2", buff_ext, &len);
            break;
        case 3:
            nvdm_read_data_item("common", "SingleSKU2G_3", buff_ext, &len);
            break;
        default:
            LOG_W(wifi, "group number is not 0~3, use group 0\n");
            nvdm_read_data_item("common", "SingleSKU2G_0", buff_ext, &len);
            break;
    }
    wifi_conf_get_pwr_from_str(single, (char *)buff_ext);
#else
    LOG_W(wifi, "Not support NVDM, use default value\n");
    wifi_conf_get_pwr_from_str(single, WIFI_DEFAULT_SINGLESKU2G);
#endif
}
#endif/*MTK_SINGLE_SKU_SUPPORT*/

#ifdef MTK_WIFI_PROFILE_ENABLE
static void wifi_get_config_from_nvdm(sys_cfg_t *config)
{
    // init wifi profile
    uint8_t buff[PROFILE_BUF_LEN];
    uint32_t len = sizeof(buff);

    // common
    len = sizeof(buff);
    nvdm_read_data_item("common", "OpMode", buff, &len);
    config->opmode = (uint8_t)atoi((char *)buff);
    len = sizeof(buff);
    nvdm_read_data_item("common", "CountryRegion", buff, &len);
    config->country_region = (uint8_t)atoi((char *)buff);
    len = sizeof(buff);
    nvdm_read_data_item("common", "CountryRegionABand", buff, &len);
    config->country_region_a_band = (uint8_t)atoi((char *)buff);
    len = sizeof(buff);
    nvdm_read_data_item("common", "CountryCode", buff, &len);
    os_memcpy(config->country_code, buff, 4);
    wifi_country_code_region_mapping(config->country_code, &(config->country_region), &(config->country_region_a_band));
    len = sizeof(buff);
    nvdm_read_data_item("common", "RadioOff", buff, &len);
    config->radio_off = (uint8_t)atoi((char *)buff);
    len = sizeof(buff);
    nvdm_read_data_item("common", "RTSThreshold", buff, &len);
    config->rts_threshold = (uint16_t)atoi((char *)buff);
    len = sizeof(buff);
    nvdm_read_data_item("common", "FragThreshold", buff, &len);
    config->frag_threshold = (uint16_t)atoi((char *)buff);
    len = sizeof(buff);
    nvdm_read_data_item("common", "DbgLevel", buff, &len);
    config->dbg_level = (uint8_t)atoi((char *)buff);
    len = sizeof(buff);
    nvdm_read_data_item("common", "IpAddr", buff, &len);
    wifi_conf_get_ip_from_str(config->ap_ip_addr, (char *)buff);
    wifi_conf_get_ip_from_str(config->sta_ip_addr, (char *)buff);

#ifdef MTK_ANT_DIV_ENABLE
    len = sizeof(buff);
    nvdm_read_data_item("common", "AntennaNumber", buff, &len);
    config->antenna_number = (uint8_t)atoi((char *)buff);
    len = sizeof(buff);
    nvdm_read_data_item("common", "AntDiversityEnable", buff, &len);
    config->antenna_diversity_enable= (uint8_t)atoi((char *)buff);
    len = sizeof(buff);
    nvdm_read_data_item("common", "AntProbeReqCount", buff, &len);
    config->antenna_probe_req_count= (uint8_t)atoi((char *)buff);
    len = sizeof(buff);
    nvdm_read_data_item("common", "AntThresholdRssi", buff, &len);
    config->antenna_threshold_level= (int8_t)atoi((char *)buff);
    len = sizeof(buff);
    nvdm_read_data_item("common", "AntDeltaRssi", buff, &len);
    config->antenna_delta_rssi= (uint8_t)atoi((char *)buff);
    len = sizeof(buff);
    nvdm_read_data_item("common", "AntRssiSwitch", buff, &len);
    config->antenna_rssi_switch= (uint8_t)atoi((char *)buff);
    len = sizeof(buff);
    nvdm_read_data_item("common", "AntPreSelectedRssi", buff, &len);
    config->antenna_pre_selected_rssi= (int8_t)atoi((char *)buff);
#endif

    // STA
    len = sizeof(buff);
    nvdm_read_data_item("STA", "LocalAdminMAC", buff, &len);
    config->sta_local_admin_mac = (uint8_t)atoi((char *)buff);
    len = sizeof(buff);

    //nvdm_read_data_item("STA", "MacAddr", buff, &len);
    //wifi_conf_get_mac_from_str((char *)config->sta_mac_addr, (char *)buff);
    if(wifi_config_get_mac_address(WIFI_PORT_STA, config->sta_mac_addr) < 0) {
        LOG_E(wifi, "STA wifi init get MAC Fail.");
    }

    len = sizeof(buff);
    nvdm_read_data_item("STA", "SsidLen", buff, &len);
    config->sta_ssid_len = (uint8_t)atoi((char *)buff);
    len = sizeof(buff);
    nvdm_read_data_item("STA", "Ssid", buff, &len);
    os_memcpy(config->sta_ssid, buff, config->sta_ssid_len);
    len = sizeof(buff);
    nvdm_read_data_item("STA", "BssType", buff, &len);
    config->sta_bss_type = (uint8_t)atoi((char *)buff);
    len = sizeof(buff);
    nvdm_read_data_item("STA", "Channel", buff, &len);
    config->sta_channel = (uint8_t)atoi((char *)buff);
    len = sizeof(buff);
    nvdm_read_data_item("STA", "BW", buff, &len);
    config->sta_bw = (uint8_t)atoi((char *)buff);
    len = sizeof(buff);
    nvdm_read_data_item("STA", "WirelessMode", buff, &len);
    if (wifi_5g_support() < 0) {
        config->sta_wireless_mode = (uint8_t)wifi_change_wireless_mode_5g_to_2g((wifi_phy_mode_t)atoi((char *)buff));
    }else {
        config->sta_wireless_mode = (uint8_t)atoi((char *)buff);
    }
    len = sizeof(buff);
    nvdm_read_data_item("STA", "BADecline", buff, &len);
    config->sta_ba_decline = (uint8_t)atoi((char *)buff);
    len = sizeof(buff);
    nvdm_read_data_item("STA", "AutoBA", buff, &len);
    config->sta_auto_ba = (uint8_t)atoi((char *)buff);
    len = sizeof(buff);
    nvdm_read_data_item("STA", "HT_MCS", buff, &len);
    config->sta_ht_mcs = (uint8_t)atoi((char *)buff);
    len = sizeof(buff);
    nvdm_read_data_item("STA", "HT_BAWinSize", buff, &len);
    config->sta_ht_ba_win_size = (uint8_t)atoi((char *)buff);
    len = sizeof(buff);
    nvdm_read_data_item("STA", "HT_GI", buff, &len);
    config->sta_ht_gi = (uint8_t)atoi((char *)buff);
    len = sizeof(buff);
    nvdm_read_data_item("STA", "HT_PROTECT", buff, &len);
    config->sta_ht_protect = (uint8_t)atoi((char *)buff);
    len = sizeof(buff);
    nvdm_read_data_item("STA", "HT_EXTCHA", buff, &len);
    config->sta_ht_ext_ch = (uint8_t)atoi((char *)buff);
    len = sizeof(buff);
    nvdm_read_data_item("STA", "WmmCapable", buff, &len);
    config->sta_wmm_capable = (uint8_t)atoi((char *)buff);
    len = sizeof(buff);
    nvdm_read_data_item("STA", "ListenInterval", buff, &len);
    config->sta_listen_interval = (uint8_t)atoi((char *)buff);
    len = sizeof(buff);
    nvdm_read_data_item("STA", "AuthMode", buff, &len);
    config->sta_auth_mode = (uint8_t)atoi((char *)buff);
    len = sizeof(buff);
    nvdm_read_data_item("STA", "EncrypType", buff, &len);
    config->sta_encryp_type = (uint8_t)atoi((char *)buff);
    len = sizeof(buff);
    nvdm_read_data_item("STA", "WpaPskLen", buff, &len);
    config->sta_wpa_psk_len = (uint8_t)atoi((char *)buff);
    len = sizeof(buff);
    nvdm_read_data_item("STA", "WpaPsk", buff, &len);
    os_memcpy(config->sta_wpa_psk, buff, config->sta_wpa_psk_len);
    len = sizeof(buff);
    nvdm_read_data_item("STA", "PMK", buff, &len);
    os_memcpy(config->sta_pmk, buff, 32); // TODO: How to save binary PMK value not ending by ' ; ' ?
    len = sizeof(buff);
    nvdm_read_data_item("STA", "PairCipher", buff, &len);
    config->sta_pair_cipher = (uint8_t)atoi((char *)buff);
    len = sizeof(buff);
    nvdm_read_data_item("STA", "GroupCipher", buff, &len);
    config->sta_group_cipher = (uint8_t)atoi((char *)buff);
    len = sizeof(buff);
    nvdm_read_data_item("STA", "DefaultKeyId", buff, &len);
    config->sta_default_key_id = (uint8_t)atoi((char *)buff);
    len = sizeof(buff);
    nvdm_read_data_item("STA", "PSMode", buff, &len);
    config->sta_ps_mode = (uint8_t)atoi((char *)buff);
    len = sizeof(buff);
    nvdm_read_data_item("STA", "KeepAlivePeriod", buff, &len);
    config->sta_keep_alive_period = (uint8_t)atoi((char *)buff);
    len = sizeof(buff);
    nvdm_read_data_item("STA", "BeaconLostTime", buff, &len);
    config->beacon_lost_time = (uint8_t)atoi((char *)buff);
    len = sizeof(buff);
    nvdm_read_data_item("STA", "ApcliBWAutoUpBelow", buff, &len);
    config->apcli_40mhz_auto_upbelow = (uint8_t)atoi((char *)buff);

    // AP
#ifdef MTK_WIFI_REPEATER_ENABLE
    if (config->opmode == WIFI_MODE_REPEATER) {
        len = sizeof(buff);
        nvdm_read_data_item("STA", "Channel", buff, &len);
        config->ap_channel = (uint8_t)atoi((char *)buff);
        len = sizeof(buff);
        nvdm_read_data_item("STA", "BW", buff, &len);
        config->ap_bw = (uint8_t)atoi((char *)buff);
        len = sizeof(buff);
        nvdm_read_data_item("STA", "WirelessMode", buff, &len);
        config->ap_wireless_mode = (uint8_t)atoi((char *)buff);
    } else
#endif
    {
        /* Use STA MAC/IP as AP MAC/IP for the time being, due to N9 dual interface not ready yet */
        len = sizeof(buff);
        nvdm_read_data_item("AP", "Channel", buff, &len);
        config->ap_channel = (uint8_t)atoi((char *)buff);
        len = sizeof(buff);
        nvdm_read_data_item("AP", "BW", buff, &len);
        config->ap_bw = (uint8_t)atoi((char *)buff);
        len = sizeof(buff);
        nvdm_read_data_item("AP", "WirelessMode", buff, &len);
        if (wifi_5g_support() < 0) {
            config->ap_wireless_mode = (uint8_t)wifi_change_wireless_mode_5g_to_2g((wifi_phy_mode_t)atoi((char *)buff));
        }else {
            config->ap_wireless_mode = (uint8_t)atoi((char *)buff);
        }
    }
    len = sizeof(buff);
    nvdm_read_data_item("AP", "LocalAdminMAC", buff, &len);
    config->ap_local_admin_mac = (uint8_t)atoi((char *)buff);
    len = sizeof(buff);

    //nvdm_read_data_item("AP", "MacAddr", buff, &len);
    //wifi_conf_get_mac_from_str((char *)config->ap_mac_addr, (char *)buff);
    if (wifi_config_get_mac_address(WIFI_PORT_AP, config->ap_mac_addr) < 0) {
        LOG_E(wifi, "AP wifi init get MAC Fail.");
    }

    len = sizeof(buff);
    nvdm_read_data_item("AP", "SsidLen", buff, &len);
    config->ap_ssid_len = (uint8_t)atoi((char *)buff);
    len = sizeof(buff);
    nvdm_read_data_item("AP", "Ssid", buff, &len);
    os_memcpy(config->ap_ssid, buff, config->ap_ssid_len);
    len = sizeof(buff);
    nvdm_read_data_item("AP", "AutoBA", buff, &len);
    config->ap_auto_ba = (uint8_t)atoi((char *)buff);
    len = sizeof(buff);
    nvdm_read_data_item("AP", "HT_MCS", buff, &len);
    config->ap_ht_mcs = (uint8_t)atoi((char *)buff);
    len = sizeof(buff);
    nvdm_read_data_item("AP", "HT_BAWinSize", buff, &len);
    config->ap_ht_ba_win_size = (uint8_t)atoi((char *)buff);
    len = sizeof(buff);
    nvdm_read_data_item("AP", "HT_GI", buff, &len);
    config->ap_ht_gi = (uint8_t)atoi((char *)buff);
    len = sizeof(buff);
    nvdm_read_data_item("AP", "HT_PROTECT", buff, &len);
    config->ap_ht_protect = (uint8_t)atoi((char *)buff);
    len = sizeof(buff);
    nvdm_read_data_item("AP", "HT_EXTCHA", buff, &len);
    config->ap_ht_ext_ch = (uint8_t)atoi((char *)buff);
    len = sizeof(buff);
    nvdm_read_data_item("AP", "WmmCapable", buff, &len);
    config->ap_wmm_capable = (uint8_t)atoi((char *)buff);
    len = sizeof(buff);
    nvdm_read_data_item("AP", "DtimPeriod", buff, &len);
    config->ap_dtim_period = (uint8_t)atoi((char *)buff);
    len = sizeof(buff);
    nvdm_read_data_item("AP", "HideSSID", buff, &len);
    config->ap_hide_ssid = (uint8_t)atoi((char *)buff);
    len = sizeof(buff);
    nvdm_read_data_item("AP", "AutoChannelSelect", buff, &len);
    config->ap_auto_channel_select = (uint8_t)atoi((char *)buff);
    len = sizeof(buff);
    nvdm_read_data_item("AP", "AuthMode", buff, &len);
    config->ap_auth_mode = (uint8_t)atoi((char *)buff);
    len = sizeof(buff);
    nvdm_read_data_item("AP", "EncrypType", buff, &len);
    config->ap_encryp_type = (uint8_t)atoi((char *)buff);
    len = sizeof(buff);
    nvdm_read_data_item("AP", "WpaPskLen", buff, &len);
    config->ap_wpa_psk_len = (uint8_t)atoi((char *)buff);
    len = sizeof(buff);
    nvdm_read_data_item("AP", "WpaPsk", buff, &len);
    os_memcpy(config->ap_wpa_psk, buff, config->ap_wpa_psk_len);
    len = sizeof(buff);
    nvdm_read_data_item("AP", "PMK", buff, &len);
    os_memcpy(config->ap_pmk, buff, 32); // TODO: How to save binary PMK value not ending by ' ; ' ?
    len = sizeof(buff);
    nvdm_read_data_item("AP", "PairCipher", buff, &len);
    config->ap_pair_cipher = (uint8_t)atoi((char *)buff);
    len = sizeof(buff);
    nvdm_read_data_item("AP", "GroupCipher", buff, &len);
    config->ap_group_cipher = (uint8_t)atoi((char *)buff);
    len = sizeof(buff);
    nvdm_read_data_item("AP", "DefaultKeyId", buff, &len);
    config->ap_default_key_id = (uint8_t)atoi((char *)buff);
    len = sizeof(buff);
    nvdm_read_data_item("AP", "BcnDisEn", buff, &len);
    config->ap_beacon_disable = (uint8_t)atoi((char *)buff);

    // scan channel table and regulatory table
    len = sizeof(buff);
    nvdm_read_data_item("common", "BGChannelTable", buff, &len);
    config->bg_band_entry_num = wifi_conf_get_ch_table_from_str(config->bg_band_triple, 10, (char *)buff, os_strlen((char *)buff));

    len = sizeof(buff);
    nvdm_read_data_item("common", "AChannelTable", buff, &len);
    config->a_band_entry_num = wifi_conf_get_ch_table_from_str(config->a_band_triple, 10, (char *)buff, os_strlen((char *)buff));

    config->forwarding_zero_copy = 1;

#ifdef MTK_WIFI_CONFIGURE_FREE_ENABLE
    /* These are for MBSS support, but not exist trunk (it's customer feature), however,
            we have to add them here due to N9 FW has them (only one version of N9 FW)
         */
    // TODO: How to solve it in the future...Michael
    config->mbss_enable = 0;
    os_memset(config->mbss_ssid1, 0x0, sizeof(config->mbss_ssid1));;
    config->mbss_ssid_len1 = 0;
    os_memset(config->mbss_ssid2, 0x0, sizeof(config->mbss_ssid2));;
    config->mbss_ssid_len2 = 0;

    len = sizeof(buff);
    nvdm_read_data_item("common", "ConfigFree_Ready", buff, &len);
    config->config_free_ready = (uint8_t)atoi((char *)buff);
    len = sizeof(buff);
    nvdm_read_data_item("common", "ConfigFree_Enable", buff, &len);
    config->config_free_enable = (uint8_t)atoi((char *)buff);
#endif /* MTK_WIFI_CONFIGURE_FREE_ENABLE */
    len = sizeof(buff);
    nvdm_read_data_item("common", "StaFastLink", buff, &len);
    config->sta_fast_link = (uint8_t)atoi((char *)buff);
#ifdef MTK_WIFI_PRIVILEGE_ENABLE
    len = sizeof(buff);
    nvdm_read_data_item("common", "WiFiPrivilegeEnable", buff, &len);
    config->wifi_privilege_enable = (uint8_t)atoi((char *)buff);
#else
    config->wifi_privilege_enable = 0;
#endif
#ifdef MTK_SINGLE_SKU_SUPPORT
    len = sizeof(buff);
    nvdm_read_data_item("common", "SupportSingleSKU", buff, &len);
    config->support_single_sku= (uint8_t)atoi((char *)buff);

    len = sizeof(buff);
    nvdm_read_data_item("common", "SingleSKU_index", buff, &len);
    if((uint8_t)atoi((char *)buff) == 0) {
        wifi_get_single_sku_from_nvdm(&config->single_sku_2g, 0);
    }  else if(((uint8_t)atoi((char *)buff) == 1)) {
        wifi_get_single_sku_from_nvdm(&config->single_sku_2g, 1);
    } else if(((uint8_t)atoi((char *)buff) == 2)) {
        wifi_get_single_sku_from_nvdm(&config->single_sku_2g, 2);
    } else if(((uint8_t)atoi((char *)buff) == 3)) {
        wifi_get_single_sku_from_nvdm(&config->single_sku_2g, 3);
    }
#endif
}
#endif
static void wifi_apply_user_config(sys_cfg_t *syscfg, wifi_config_t *config, wifi_config_ext_t *config_ext)
{
    syscfg->opmode = config->opmode;
    if (config_ext->country_code_present) {
        os_memcpy(syscfg->country_code, config_ext->country_code, 4);
        wifi_country_code_region_mapping(syscfg->country_code, &(syscfg->country_region), &(syscfg->country_region_a_band));
    }
    if (WIFI_MODE_STA_ONLY == syscfg->opmode) {
        wifi_apply_sta_config(syscfg, config, config_ext);
    } else if (WIFI_MODE_AP_ONLY == syscfg->opmode) {
        wifi_apply_ap_config(syscfg, config, config_ext);
    } else if (WIFI_MODE_REPEATER == syscfg->opmode) {
        wifi_apply_repeater_config(syscfg, config, config_ext);
    } else if (WIFI_MODE_P2P_ONLY == syscfg->opmode) {
        wifi_apply_p2p_config(syscfg, config, config_ext);
    } else {
        /* no configuration is required for Monitor Mode */
    }
}

wifi_init_config_handler_struct_t g_wifi_init_config_handler;

int32_t wifi_init_register_config_handler(wifi_config_advance_type_t profile_type, wifi_init_config_handler_t handler)
{
    int32_t ret = 0;

    switch(profile_type) {
#ifdef MTK_SINGLE_SKU_SUPPORT
        case WIFI_CONFIG_ADVANCE_SINGLESKU2G:
            g_wifi_init_config_handler.single_sku_2g = handler;
            ret = 0;
            break;
#endif

        case WIFI_CONFIG_ADVANCE_STA_FAST_LINK_ENABLE:
            g_wifi_init_config_handler.sta_fast_link_enable = handler;
            ret = 0;
            break;

        case WIFI_CONFIG_ADVANCE_BG_CHANNEL_TABLE:
            g_wifi_init_config_handler.bg_channel_table = handler;
            ret = 0;
            break;

#ifdef MTK_ANT_DIV_ENABLE
        case WIFI_CONFIG_ADVANCE_RF_DIVERSITY:
            g_wifi_init_config_handler.wifi_rf_diversity = handler;
            ret = 0;
            break;
#endif

        default:
            LOG_E(wifi,"Wrong profile type");
            ret = -1;
            break;
    }

    return ret;
}

static void wifi_apply_user_init_config(sys_cfg_t *syscfg)
{
    wifi_config_advance_t config_advanced;

#ifdef MTK_SINGLE_SKU_SUPPORT
    if(g_wifi_init_config_handler.single_sku_2g) {
        config_advanced.single_sku_2g = &syscfg->single_sku_2g;
        g_wifi_init_config_handler.single_sku_2g(WIFI_CONFIG_ADVANCE_SINGLESKU2G, &config_advanced);
    }
#endif

    if(g_wifi_init_config_handler.sta_fast_link_enable) {
        config_advanced.sta_fast_link = &syscfg->sta_fast_link;
        g_wifi_init_config_handler.sta_fast_link_enable(WIFI_CONFIG_ADVANCE_STA_FAST_LINK_ENABLE, &config_advanced);
        LOG_I(wifi,"sta_fast_link:%d", syscfg->sta_fast_link);
    }

    if(g_wifi_init_config_handler.bg_channel_table) {
        uint8_t bg_channel_table_number = 0;
        char * bg_channel_table_buffer = wifi_os_zalloc(10 * (sizeof(uint8_t) * 9 )); //char * table = "xx,xx,xx|";
        if(bg_channel_table_buffer != NULL) {
            config_advanced.bg_channel_table = bg_channel_table_buffer;
            g_wifi_init_config_handler.bg_channel_table(WIFI_CONFIG_ADVANCE_BG_CHANNEL_TABLE, &config_advanced);
            bg_channel_table_number = wifi_conf_get_ch_table_from_str(syscfg->bg_band_triple, 10, bg_channel_table_buffer, os_strlen(bg_channel_table_buffer));
            if(bg_channel_table_number > 0 && bg_channel_table_number <= 10) {
                syscfg->bg_band_entry_num = bg_channel_table_number;
            } else {
                LOG_E(wifi,"BG channel table write error, num=%d", bg_channel_table_number);
            }
            LOG_HEXDUMP_I(wifi,"bg_channel_table",syscfg->bg_band_triple,sizeof(ch_desc_t)*bg_channel_table_number);
            wifi_os_free(bg_channel_table_buffer);
        } else {
            LOG_E(wifi,"BG channel table memalloc fail");
        }
    }

#ifdef MTK_ANT_DIV_ENABLE
    if(g_wifi_init_config_handler.wifi_rf_diversity) {
        wifi_rf_diversity user_rf_diversity = {0};
        config_advanced.rf_diversity = &user_rf_diversity;
        g_wifi_init_config_handler.wifi_rf_diversity(WIFI_CONFIG_ADVANCE_RF_DIVERSITY, &config_advanced);
        syscfg->antenna_number = user_rf_diversity.antenna_number;
        syscfg->antenna_diversity_enable = user_rf_diversity.antenna_diversity_enable;
        syscfg->antenna_probe_req_count = user_rf_diversity.antenna_probe_req_count;
        syscfg->antenna_threshold_level = user_rf_diversity.antenna_threshold_level;
        syscfg->antenna_delta_rssi = user_rf_diversity.antenna_delta_rssi;
        syscfg->antenna_rssi_switch = user_rf_diversity.antenna_rssi_switch;
        syscfg->antenna_pre_selected_rssi = user_rf_diversity.antenna_pre_selected_rssi;
    }
#endif

    return;
}

/**
* @brief build the whole configurations
*/
static int32_t wifi_build_whole_config(sys_cfg_t *syscfg, wifi_config_t *config, wifi_config_ext_t *config_ext)
{
#ifdef MTK_WIFI_PROFILE_ENABLE
    wifi_get_config_from_nvdm(syscfg);
#else
    if (0 != wifi_get_default_config(syscfg)) {
        return -1;
    }
#endif
    wifi_apply_user_config(syscfg, config, config_ext);

    //used for overwrite the syscfg advanced profile by user registered handler
    wifi_apply_user_init_config(syscfg);

    LOG_W(wifi, "profile mac %02x:%02x:%02x after build whole config\n",
                 syscfg->sta_mac_addr[0] & 0Xff,
                 syscfg->sta_mac_addr[1] & 0Xff,
                 syscfg->sta_mac_addr[2] & 0Xff);

    return 0;
}

void wifi_init(wifi_config_t *config, wifi_config_ext_t *config_ext)
{
    sys_cfg_t *syscfg;

    syscfg = os_zalloc(sizeof(*syscfg));

    configASSERT(syscfg);

    if (NULL == config) {
        LOG_E(wifi, "config is null.");
        os_free(syscfg);
        return;
    }

    if (0 != wifi_build_whole_config(syscfg, config, config_ext)) {
        LOG_E(wifi, "wifi_build_whole_config fail. initial aborted!");
        os_free(syscfg);
        return;
    }

    wifi_channel_list_init(syscfg);

    connsys_init(syscfg);

	wifi_init_register_callback();

#ifdef MTK_ANT_DIV_ENABLE
    /* After HW/patch download, to overwrite the patch configuration */
    // tx_switch_for_antenna,rx_switch_for_antenna is controled in NVDM and
    // include in MTK_WIFI_PROFILE_ENABLE.
    if (syscfg->antenna_diversity_enable != 0) {

        ant_diversity_init(syscfg->antenna_number);

        //printf("tx_switch_for_antenna=%d,rx_switch_for_antenna=%d",
        //         syscfg->tx_switch_for_antenna, syscfg->rx_switch_for_antenna);
    }
    //printf_ant_diversity(10);
#endif

    wifi_scan_init(config);
    inband_queue_init();
    NetTaskInit();

    /* Fix if SSID length = 0, switch to repeater mode from station mode, N9 will assert*/
    if (config->ap_config.ssid_length == 0) {
        config->ap_config.ssid_length = syscfg->ap_ssid_len;
        os_memcpy(config->ap_config.ssid, syscfg->ap_ssid, WIFI_MAX_LENGTH_OF_SSID);
    } else if(config->sta_config.ssid_length == 0) {
        config->sta_config.ssid_length = syscfg->sta_ssid_len;
        os_memcpy(config->sta_config.ssid, syscfg->sta_ssid, WIFI_MAX_LENGTH_OF_SSID);
    }
#ifdef MTK_MINISUPP_ENABLE
    wpa_supplicant_task_init(config, config_ext);
#endif

    // Mark the status whether N9 is doing auto scan when bootup.
    wifi_config_set_n9_auto_scan_status(syscfg->sta_fast_link);

    os_free(syscfg);
}


static bool wifi_security_valid = false;
bool wifi_get_security_valid(void)
{
    return wifi_security_valid;
}

void wifi_set_security_valid(bool value)
{
    wifi_security_valid = value;
    return;
}

/***************** Just for internal use **********************/
#include "hal_sleep_manager.h"
#include "hal_sleep_driver.h"
#ifdef HAL_SLEEP_MANAGER_ENABLED
uint8_t locks[MAX_SLEEP_HANDLE];
#endif
uint8_t wifi_set_sleep_handle(const char *handle_name)
{
#ifdef HAL_SLEEP_MANAGER_ENABLED
    return hal_sleep_manager_set_sleep_handle(handle_name);
#else
    return 0xff;
#endif
}

int32_t wifi_lock_sleep(uint8_t handle_index)
{
#ifdef HAL_SLEEP_MANAGER_ENABLED
    locks[handle_index] = 1;
    return hal_sleep_manager_lock_sleep(handle_index);
#else
    return -1;
#endif
}

int32_t wifi_unlock_sleep(uint8_t handle_index)
{
#ifdef HAL_SLEEP_MANAGER_ENABLED
    locks[handle_index] = 0;
    return hal_sleep_manager_unlock_sleep(handle_index);
#else
    return -1;
#endif
}

int32_t wifi_unlock_sleep_all(void)
{
#ifdef HAL_SLEEP_MANAGER_ENABLED
    int i = 0;
    for (i = 0; i < MAX_SLEEP_HANDLE; i++) {
        if (locks[i] == 1) {
            hal_sleep_manager_unlock_sleep(i);
        }
    }
    return 0;
#else
    return -1;
#endif
}

int32_t wifi_sleep_manager_get_lock_status(void)
{
#ifdef HAL_SLEEP_MANAGER_ENABLED
    return hal_sleep_manager_get_lock_status();
#else
    return -1;
#endif
}

int32_t wifi_release_sleep_handle(uint8_t handle)
{
#ifdef HAL_SLEEP_MANAGER_ENABLED
    return hal_sleep_manager_release_sleep_handle(handle);
#else
    return -1;
#endif
}

/**
*@brief Auto record the PMK info to NVDM, when ssid/ssid_len/passphrase changed during wifi connect.
*/
int32_t wifi_profile_auto_record_pmk_info(uint8_t *ssid, uint8_t ssid_length, uint8_t *passphrase, uint8_t passphrase_len, uint8_t *psk)
{
    uint8_t pmk_info[WIFI_MAX_LENGTH_OF_SSID+WIFI_LENGTH_PASSPHRASE+WIFI_LENGTH_PMK] = {0};

    os_memcpy(pmk_info, ssid, ssid_length);
    os_memcpy(pmk_info+WIFI_MAX_LENGTH_OF_SSID, passphrase, passphrase_len);
    os_memcpy(pmk_info+WIFI_MAX_LENGTH_OF_SSID+WIFI_LENGTH_PASSPHRASE, psk, WIFI_LENGTH_PMK);
#ifdef MTK_NVDM_ENABLE
    if (NVDM_STATUS_OK != nvdm_write_data_item(WIFI_PROFILE_BUFFER_STA, "PMK_INFO", NVDM_DATA_ITEM_TYPE_STRING,
                    (uint8_t *)pmk_info, WIFI_MAX_LENGTH_OF_SSID+WIFI_LENGTH_PASSPHRASE+WIFI_LENGTH_PMK)) {
        return -1;
    }
    return 0;
#else
    LOG_W(wifi,"NVDM not enable to record PMK INFO\n");
    return -1;
#endif
}
/***************** Just for internal use **********************/
