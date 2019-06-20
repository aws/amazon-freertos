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

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "type_def.h"
#include "mt7687.h"
#include "connsys_driver.h"
#include "connsys_util.h"
#include "wifi_scan.h"
#include "wifi_api.h"
#include "wifi_inband.h"
#include "os.h"
//#include "nvdm.h"
#include "get_profile_string.h"
#include "timer.h"
//#include "syslog.h"
#include "wifi_profile.h"


/**
 * @brief judge whether the opmode is valid
 */
bool wifi_is_opmode_valid(uint8_t mode)
{
    return (mode <= WIFI_MODE_P2P_ONLY);
}

/**
 * @brief judge whether the port is valid
 */
bool wifi_is_port_valid(uint8_t port)
{
    return (port <= WIFI_PORT_AP);
}

/**
 * @brief judge whether the band is valid
 */
bool wifi_is_band_valid(uint8_t band)
{
    return (band <= WIFI_BAND_5_G);
}

/**
 * @brief judge whether the bandwidth is valid
 */
bool wifi_is_bandwidth_valid(uint8_t bandwidth)
{
    return (bandwidth <= WIFI_IOT_COMMAND_CONFIG_BANDWIDTH_2040MHZ);
}

/**
 * @brief judge whether the auth mode is valid
 */
bool wifi_is_auth_mode_valid(uint8_t auth_mode)
{
    return (auth_mode <= WIFI_AUTH_MODE_WPA_PSK_WPA2_PSK);
}

/**
 * @brief judge whether the encrypt type is valid
 */
bool wifi_is_encrypt_type_valid(uint8_t encrypt_type)
{
#ifdef WAPI_SUPPORT
    return (encrypt_type <= WIFI_ENCRYPT_TYPE_ENCRYPT_SMS4_ENABLED);
#else
    return (encrypt_type <= WIFI_ENCRYPT_TYPE_GROUP_WEP104_ENABLED);
#endif
}

/**
 * @brief judge whether the power save mode is valid
 */
bool wifi_is_ps_mode_valid(uint8_t ps_mode)
{
    return (ps_mode <= 2);
}

#ifdef MTK_WIFI_PROFILE_ENABLE
int32_t wifi_profile_set_opmode(uint8_t mode)
{
    char buf[WIFI_PROFILE_BUFFER_LENGTH] = {0};

    if (!wifi_is_opmode_valid(mode)) {
        LOG_E(wifi, "mode is invalid: %d", mode);
        return WIFI_ERR_PARA_INVALID;
    }

    sprintf(buf, "%d", mode);

    if (NVDM_STATUS_OK != nvdm_write_data_item(WIFI_PROFILE_BUFFER_COMMON, "OpMode",
            NVDM_DATA_ITEM_TYPE_STRING,
            (uint8_t *)buf, os_strlen(buf))) {
        return WIFI_FAIL;
    }

    return WIFI_SUCC;
}

int32_t wifi_profile_get_opmode(uint8_t *mode)
{
    char buf[WIFI_PROFILE_BUFFER_LENGTH] = {0};
    uint32_t len = sizeof(buf);

    if (NULL == mode) {
        LOG_E(wifi, "mode is null");
        return WIFI_ERR_PARA_INVALID;
    }

    if (NVDM_STATUS_OK != nvdm_read_data_item(WIFI_PROFILE_BUFFER_COMMON, "OpMode", (uint8_t *)buf, &len)) {
        return WIFI_FAIL;
    }

    *mode = atoi(buf);
    return WIFI_SUCC;
}


int32_t wifi_profile_set_channel(uint8_t port, uint8_t channel)
{
    char buf[WIFI_PROFILE_BUFFER_LENGTH] = {0};

    sprintf(buf, "%d", channel);

    if (port == WIFI_PORT_AP) {
        if (NVDM_STATUS_OK != nvdm_write_data_item(WIFI_PROFILE_BUFFER_AP, "Channel",
                NVDM_DATA_ITEM_TYPE_STRING,
                (uint8_t *)buf, os_strlen(buf))) {
            return WIFI_FAIL;
        }
    } else if (port == WIFI_PORT_STA) {
        if (NVDM_STATUS_OK != nvdm_write_data_item(WIFI_PROFILE_BUFFER_STA, "Channel",
                NVDM_DATA_ITEM_TYPE_STRING,
                (uint8_t *)buf, os_strlen(buf))) {
            return WIFI_FAIL;
        }
    } else {
        LOG_E(wifi, "port is invalid: %d", port);
        return WIFI_ERR_PARA_INVALID;
    }
    return WIFI_SUCC;
}

int32_t wifi_profile_get_channel(uint8_t port, uint8_t *channel)
{
    char buf[WIFI_PROFILE_BUFFER_LENGTH] = {0};
    uint32_t len = sizeof(buf);

    if (NULL == channel) {
        return WIFI_ERR_PARA_INVALID;
    }

    if (port == WIFI_PORT_AP) {
        if (NVDM_STATUS_OK != nvdm_read_data_item(WIFI_PROFILE_BUFFER_AP, "Channel", (uint8_t *)buf, &len)) {
            return WIFI_FAIL;
        }
    } else if (port == WIFI_PORT_STA) {
        if (NVDM_STATUS_OK != nvdm_read_data_item(WIFI_PROFILE_BUFFER_STA, "Channel", (uint8_t *)buf, &len)) {
            return WIFI_FAIL;
        }
    } else {
        LOG_E(wifi, "port is invalid: %d", port);
        return WIFI_ERR_PARA_INVALID;
    }

    *channel = atoi(buf);
    return WIFI_SUCC;
}

int32_t wifi_profile_set_bandwidth(uint8_t port, uint8_t bandwidth)
{
    char buf[WIFI_PROFILE_BUFFER_LENGTH] = {0};

    if (!wifi_is_bandwidth_valid(bandwidth)) {
        LOG_E(wifi, "bandwidth is invalid: %d", bandwidth);
        return WIFI_ERR_PARA_INVALID;
    }

    sprintf(buf, "%d", bandwidth);

    if (port == WIFI_PORT_AP) {
        if (NVDM_STATUS_OK != nvdm_write_data_item(WIFI_PROFILE_BUFFER_AP, "BW",
                NVDM_DATA_ITEM_TYPE_STRING,
                (uint8_t *)buf, os_strlen(buf))) {
            return WIFI_FAIL;
        }
    } else if (port == WIFI_PORT_STA) {
        if (NVDM_STATUS_OK != nvdm_write_data_item(WIFI_PROFILE_BUFFER_STA, "BW",
                NVDM_DATA_ITEM_TYPE_STRING,
                (uint8_t *)buf, os_strlen(buf))) {
            return WIFI_FAIL;
        }
    } else {
        LOG_E(wifi, "port is invalid: %d", port);
        return WIFI_ERR_PARA_INVALID;
    }
    return WIFI_SUCC;
}


int32_t wifi_profile_get_bandwidth(uint8_t port, uint8_t *bandwidth)
{
    char buf[WIFI_PROFILE_BUFFER_LENGTH] = {0};
    uint32_t len = sizeof(buf);

    if (NULL == bandwidth) {
        LOG_E(wifi, "bandwidth is null");
        return WIFI_ERR_PARA_INVALID;
    }

    if (port == WIFI_PORT_AP) {
        if (NVDM_STATUS_OK != nvdm_read_data_item(WIFI_PROFILE_BUFFER_AP, "BW", (uint8_t *)buf, &len)) {
            return WIFI_FAIL;
        }
    } else if (port == WIFI_PORT_STA) {
        if (NVDM_STATUS_OK != nvdm_read_data_item(WIFI_PROFILE_BUFFER_STA, "BW", (uint8_t *)buf, &len)) {
            return WIFI_FAIL;
        }
    } else {
        LOG_E(wifi, "port is invalid: %d", port);
        return WIFI_ERR_PARA_INVALID;
    }

    *bandwidth = atoi(buf);
    return WIFI_SUCC;
}

int32_t wifi_profile_get_mac_address(uint8_t port, uint8_t *address)
{
    char buf[WIFI_PROFILE_BUFFER_LENGTH] = {0};
    uint32_t len = sizeof(buf);

    if (!wifi_is_port_valid(port)) {
        LOG_E(wifi, "port is invalid: %d", port);
        return WIFI_ERR_PARA_INVALID;
    }
    if (NULL == address) {
        LOG_E(wifi, "address is null");
        return WIFI_ERR_PARA_INVALID;
    }

#ifdef MTK_WIFI_REPEATER_ENABLE
    uint8_t mode;
    if (wifi_profile_get_opmode(&mode) < 0) {
        return WIFI_FAIL;
    }
    if ((mode == WIFI_MODE_REPEATER) && (port == WIFI_PORT_AP)) {
        if (NVDM_STATUS_OK != nvdm_read_data_item(WIFI_PROFILE_BUFFER_AP, "MacAddr", (uint8_t *)buf, &len)) {
            return WIFI_FAIL;
        }
    } else {
        if (NVDM_STATUS_OK != nvdm_read_data_item(WIFI_PROFILE_BUFFER_STA, "MacAddr", (uint8_t *)buf, &len)) {
            return WIFI_FAIL;
        }
    }
#else
    /* Use STA MAC/IP as AP MAC/IP for the time being, due to N9 dual interface not ready yet */
    if (NVDM_STATUS_OK != nvdm_read_data_item(WIFI_PROFILE_BUFFER_STA, "MacAddr", (uint8_t *)buf, &len)) {
        return WIFI_FAIL;
    }
#endif

    wifi_conf_get_mac_from_str((char *)address, buf);
    return WIFI_SUCC;
}


int32_t wifi_profile_set_mac_address(uint8_t port, uint8_t *address)
{
    if (!wifi_is_port_valid(port)) {
        LOG_E(wifi, "port is invalid: %d", port);
        return WIFI_ERR_PARA_INVALID;
    }
    if (address == NULL) {
        LOG_E(wifi, "mac address is null.");
        return WIFI_ERR_PARA_INVALID;
    }

    char buf[WIFI_PROFILE_BUFFER_LENGTH] = {0};

    sprintf(buf, "%02x:%02x:%02x:%02x:%02x:%02x",
            address[0], address[1], address[2], address[3], address[4], address[5]);

    if (port == WIFI_PORT_AP) {
        if (NVDM_STATUS_OK != nvdm_write_data_item(WIFI_PROFILE_BUFFER_AP, "MacAddr",
                NVDM_DATA_ITEM_TYPE_STRING,
                (uint8_t *)buf, os_strlen(buf))) {
            return WIFI_FAIL;
        }
    } else {
        if (NVDM_STATUS_OK != nvdm_write_data_item(WIFI_PROFILE_BUFFER_STA, "MacAddr",
                NVDM_DATA_ITEM_TYPE_STRING,
                (uint8_t *)buf, os_strlen(buf))) {
            return WIFI_FAIL;
        }
    }

    return WIFI_SUCC;
}


int32_t wifi_profile_set_ssid(uint8_t port, uint8_t *ssid , uint8_t ssid_length)
{
    if (!wifi_is_port_valid(port)) {
        LOG_E(wifi, "port is invalid: %d", port);
        return WIFI_ERR_PARA_INVALID;
    }
    if (ssid_length > WIFI_MAX_LENGTH_OF_SSID) {
        LOG_I(wifi, "incorrect length(=%d)", ssid_length);
        return WIFI_ERR_PARA_INVALID;
    }
    if (NULL == ssid) {
        LOG_E(wifi, "ssid is null.");
        return WIFI_ERR_PARA_INVALID;
    }

    char ssid_buf[WIFI_PROFILE_BUFFER_LENGTH] = {0};
    char ssid_len_buf[WIFI_PROFILE_BUFFER_LENGTH] = {0};

    os_memcpy(ssid_buf, ssid, ssid_length);
    ssid_buf[ssid_length] = '\0';


    /***add by Pengfei, optimize PMK calculate at boot up***/
    if (WIFI_PORT_STA == port) {
        uint8_t pmk_info[WIFI_MAX_LENGTH_OF_SSID+WIFI_LENGTH_PASSPHRASE+WIFI_LENGTH_PMK] = {0};
        uint8_t buf_passphrase[WIFI_LENGTH_PASSPHRASE] = {0};
        uint8_t passphrase_length = 0;
        uint8_t psk[WIFI_LENGTH_PMK];
        uint8_t *passphrase = NULL;
        if (wifi_profile_get_wpa_psk_key(port, buf_passphrase, &passphrase_length) < 0) {
            return WIFI_FAIL;
        }

        passphrase = os_malloc(WIFI_LENGTH_PASSPHRASE);
        if(passphrase == NULL) {
            LOG_E(wifi, "cal_pmk malloc fail");
            return WIFI_FAIL;
        }
        os_memset(passphrase, 0, WIFI_LENGTH_PASSPHRASE);
        os_memcpy(passphrase, buf_passphrase, passphrase_length);
#ifdef MTK_MINISUPP_ENABLE
        if (__g_wpa_supplicant_api.cal_pmk) {
            if (__g_wpa_supplicant_api.cal_pmk(buf_passphrase, ssid, ssid_length, psk) < 0) {
                LOG_E(wifi, "callback cal_pmk is not ready");
                os_free(passphrase);
                return WIFI_FAIL;
            }
        }
#endif /* MTK_MINISUPP_ENABLE */

        os_free(passphrase);
        os_memcpy(pmk_info, ssid, ssid_length);
        os_memcpy(pmk_info+WIFI_MAX_LENGTH_OF_SSID, buf_passphrase, passphrase_length);
        os_memcpy(pmk_info+WIFI_MAX_LENGTH_OF_SSID+WIFI_LENGTH_PASSPHRASE, psk, WIFI_LENGTH_PMK);
        if (NVDM_STATUS_OK != nvdm_write_data_item(WIFI_PROFILE_BUFFER_STA, "PMK_INFO", NVDM_DATA_ITEM_TYPE_STRING,
                            (uint8_t *)pmk_info, WIFI_MAX_LENGTH_OF_SSID+WIFI_LENGTH_PASSPHRASE+WIFI_LENGTH_PMK)) {
            return WIFI_FAIL;
        }
    }

    sprintf(ssid_len_buf, "%d", ssid_length);
    if (port == WIFI_PORT_AP) {
        if (NVDM_STATUS_OK != nvdm_write_data_item(WIFI_PROFILE_BUFFER_AP, "Ssid",
                NVDM_DATA_ITEM_TYPE_STRING,
                (uint8_t *)ssid_buf, os_strlen(ssid_buf))) {
            return WIFI_FAIL;
        }
        if (NVDM_STATUS_OK != nvdm_write_data_item(WIFI_PROFILE_BUFFER_AP, "SsidLen",
                NVDM_DATA_ITEM_TYPE_STRING,
                (uint8_t *)ssid_len_buf, os_strlen(ssid_len_buf))) {
            return WIFI_FAIL;
        }
    } else {
        if (NVDM_STATUS_OK != nvdm_write_data_item(WIFI_PROFILE_BUFFER_STA, "Ssid",
                NVDM_DATA_ITEM_TYPE_STRING,
                (uint8_t *)ssid_buf, os_strlen(ssid_buf))) {
            return WIFI_FAIL;
        }
        if (NVDM_STATUS_OK != nvdm_write_data_item(WIFI_PROFILE_BUFFER_STA, "SsidLen",
                NVDM_DATA_ITEM_TYPE_STRING,
                (uint8_t *)ssid_len_buf, os_strlen(ssid_len_buf))) {
            return WIFI_FAIL;
        }
    }
    return WIFI_SUCC;
}


int32_t wifi_profile_get_ssid(uint8_t port, uint8_t *ssid, uint8_t *ssid_length)
{
    char buf[WIFI_PROFILE_BUFFER_LENGTH];
    uint32_t len;

    if (!wifi_is_port_valid(port)) {
        LOG_E(wifi, "port is invalid: %d", port);
        return WIFI_ERR_PARA_INVALID;
    }
    if (NULL == ssid_length) {
        LOG_E(wifi, "ssid_length is null.");
        return WIFI_ERR_PARA_INVALID;
    }
    if (NULL == ssid) {
        LOG_E(wifi, "ssid is null.");
        return WIFI_ERR_PARA_INVALID;
    }

    if (port == WIFI_PORT_AP) {
        os_memset(buf, 0, WIFI_PROFILE_BUFFER_LENGTH);
        len = sizeof(buf);
        if (NVDM_STATUS_OK != nvdm_read_data_item(WIFI_PROFILE_BUFFER_AP, "Ssid", (uint8_t *)buf, &len)) {
            return WIFI_FAIL;
        }
        strcpy((char *)ssid, buf);

        os_memset(buf, 0, WIFI_PROFILE_BUFFER_LENGTH);
        len = sizeof(buf);
        if (NVDM_STATUS_OK != nvdm_read_data_item(WIFI_PROFILE_BUFFER_AP, "SsidLen", (uint8_t *)buf, &len)) {
            return WIFI_FAIL;
        }
        *ssid_length = atoi(buf);
        return WIFI_SUCC;
    } else {
        os_memset(buf, 0, WIFI_PROFILE_BUFFER_LENGTH);
        len = sizeof(buf);
        if (NVDM_STATUS_OK != nvdm_read_data_item(WIFI_PROFILE_BUFFER_STA, "Ssid", (uint8_t *)buf, &len)) {
            return WIFI_FAIL;
        }
        strcpy((char *)ssid, buf);

        os_memset(buf, 0, WIFI_PROFILE_BUFFER_LENGTH);
        len = sizeof(buf);
        if (NVDM_STATUS_OK != nvdm_read_data_item(WIFI_PROFILE_BUFFER_STA, "SsidLen", (uint8_t *)buf, &len)) {
            return WIFI_FAIL;
        }
        *ssid_length = atoi(buf);
        return WIFI_SUCC;
    }
}

int32_t wifi_profile_commit_setting(char *profile_section)
{
    return WIFI_SUCC;
}

int32_t wifi_profile_get_profile(uint8_t port, wifi_profile_t *profile)
{
    return WIFI_SUCC;
}

int32_t wifi_profile_set_wireless_mode(uint8_t port, wifi_phy_mode_t mode)
{
    char buf[WIFI_PROFILE_BUFFER_LENGTH] = {0};

    if (!wifi_is_port_valid(port)) {
        LOG_E(wifi, "port is invalid: %d", port);
        return WIFI_ERR_PARA_INVALID;
    }

    int32_t support_5g = 0;
    support_5g = wifi_5g_support();
    if (((WIFI_PHY_11A == mode) || (WIFI_PHY_11ABG_MIXED == mode) ||(WIFI_PHY_11ABGN_MIXED == mode) || (WIFI_PHY_11AN_MIXED == mode))
         && (support_5g < 0)) {
         LOG_E(wifi, "Chip doesn't support 5G.");
         return WIFI_ERR_NOT_SUPPORT;
    }

    sprintf(buf, "%d", mode);

    if (port == WIFI_PORT_AP) {
        if (NVDM_STATUS_OK != nvdm_write_data_item(WIFI_PROFILE_BUFFER_AP, "WirelessMode",
                NVDM_DATA_ITEM_TYPE_STRING,
                (uint8_t *)buf, os_strlen(buf))) {
            return WIFI_FAIL;
        }
    } else {
        if (NVDM_STATUS_OK != nvdm_write_data_item(WIFI_PROFILE_BUFFER_STA, "WirelessMode",
                NVDM_DATA_ITEM_TYPE_STRING,
                (uint8_t *)buf, os_strlen(buf))) {
            return WIFI_FAIL;
        }
    }
    return WIFI_SUCC;
}


int32_t wifi_profile_get_wireless_mode(uint8_t port, wifi_phy_mode_t *mode)
{
    char buf[WIFI_PROFILE_BUFFER_LENGTH] = {0};
    uint32_t len = sizeof(buf);

    if (!wifi_is_port_valid(port)) {
        LOG_E(wifi, "port is invalid: %d", port);
        return WIFI_ERR_PARA_INVALID;
    }
    if (NULL == mode) {
        LOG_E(wifi, "mode is null");
        return WIFI_ERR_PARA_INVALID;
    }

    if (port == WIFI_PORT_AP) {
        if (NVDM_STATUS_OK != nvdm_read_data_item(WIFI_PROFILE_BUFFER_AP, "WirelessMode", (uint8_t *)buf, &len)) {
            return WIFI_FAIL;
        }
    } else {
        if (NVDM_STATUS_OK != nvdm_read_data_item(WIFI_PROFILE_BUFFER_STA, "WirelessMode", (uint8_t *)buf, &len)) {
            return WIFI_FAIL;
        }
    }

    *mode = (wifi_phy_mode_t)atoi(buf);
    return WIFI_SUCC;
}

int32_t wifi_profile_set_security_mode(uint8_t port, wifi_auth_mode_t auth_mode, wifi_encrypt_type_t encrypt_type)
{
    char auth_buf[WIFI_PROFILE_BUFFER_LENGTH] = {0};
    char encrypt_buf[WIFI_PROFILE_BUFFER_LENGTH] = {0};

    if (!wifi_is_port_valid(port)) {
        LOG_E(wifi, "port is invalid: %d", port);
        return WIFI_ERR_PARA_INVALID;
    }
    if (!wifi_is_auth_mode_valid(auth_mode)) {
        LOG_E(wifi, "auth_mode is invalid: %d", auth_mode);
        return WIFI_ERR_PARA_INVALID;
    }
    if (!wifi_is_encrypt_type_valid(encrypt_type)) {
        LOG_E(wifi, "encrypt_type is invalid: %d", encrypt_type);
        return WIFI_ERR_PARA_INVALID;
    }

    sprintf(auth_buf, "%d", auth_mode);
    sprintf(encrypt_buf, "%d", encrypt_type);

    if (port == WIFI_PORT_AP) {
        if (NVDM_STATUS_OK != nvdm_write_data_item(WIFI_PROFILE_BUFFER_AP, "AuthMode",
                NVDM_DATA_ITEM_TYPE_STRING,
                (uint8_t *)auth_buf, os_strlen(auth_buf))) {
            return WIFI_FAIL;
        }
        if (NVDM_STATUS_OK != nvdm_write_data_item(WIFI_PROFILE_BUFFER_AP, "EncrypType",
                NVDM_DATA_ITEM_TYPE_STRING,
                (uint8_t *)encrypt_buf, os_strlen(encrypt_buf))) {
            return WIFI_FAIL;
        }
    } else {
        if (NVDM_STATUS_OK != nvdm_write_data_item(WIFI_PROFILE_BUFFER_STA, "AuthMode",
                NVDM_DATA_ITEM_TYPE_STRING,
                (uint8_t *)auth_buf, os_strlen(auth_buf))) {
            return WIFI_FAIL;
        }
        if (NVDM_STATUS_OK != nvdm_write_data_item(WIFI_PROFILE_BUFFER_STA, "EncrypType",
                NVDM_DATA_ITEM_TYPE_STRING,
                (uint8_t *)encrypt_buf, os_strlen(encrypt_buf))) {
            return WIFI_FAIL;
        }
    }
    return WIFI_SUCC;
}

int32_t wifi_profile_get_security_mode(uint8_t port, wifi_auth_mode_t *auth_mode, wifi_encrypt_type_t *encrypt_type)
{
    char auth_buf[WIFI_PROFILE_BUFFER_LENGTH] = {0};
    char encypt_buf[WIFI_PROFILE_BUFFER_LENGTH] = {0};
    uint32_t auth_buf_len = sizeof(auth_buf);
    uint32_t encypt_buf_len = sizeof(encypt_buf);

    if (!wifi_is_port_valid(port)) {
        LOG_E(wifi, "port is invalid: %d", port);
        return WIFI_ERR_PARA_INVALID;
    }
    if (NULL == auth_mode || NULL == encrypt_type) {
        LOG_E(wifi, "null input pointer");
        return WIFI_ERR_PARA_INVALID;
    }

    if (port == WIFI_PORT_AP) {
        if (NVDM_STATUS_OK != nvdm_read_data_item(WIFI_PROFILE_BUFFER_AP, "AuthMode", (uint8_t *)auth_buf, &auth_buf_len)) {
            return WIFI_FAIL;
        }
        if (NVDM_STATUS_OK != nvdm_read_data_item(WIFI_PROFILE_BUFFER_AP, "EncrypType", (uint8_t *)encypt_buf, &encypt_buf_len)) {
            return WIFI_FAIL;
        }
    } else {
        if (NVDM_STATUS_OK != nvdm_read_data_item(WIFI_PROFILE_BUFFER_STA, "AuthMode", (uint8_t *)auth_buf, &auth_buf_len)) {
            return WIFI_FAIL;
        }
        if (NVDM_STATUS_OK != nvdm_read_data_item(WIFI_PROFILE_BUFFER_STA, "EncrypType", (uint8_t *)encypt_buf, &encypt_buf_len)) {
            return WIFI_FAIL;
        }
    }

    *auth_mode = (wifi_auth_mode_t)atoi(auth_buf);
    *encrypt_type = (wifi_encrypt_type_t)atoi(encypt_buf);
    return WIFI_SUCC;
}


int32_t wifi_profile_set_wpa_psk_key(uint8_t port, uint8_t *passphrase, uint8_t passphrase_length)
{
    char pass_buf[WIFI_PROFILE_BUFFER_LENGTH] = {0};
    char pass_len_buf[WIFI_PROFILE_BUFFER_LENGTH] = {0};

    if (!wifi_is_port_valid(port)) {
        LOG_E(wifi, "port is invalid: %d", port);
        return WIFI_ERR_PARA_INVALID;
    }
    if (passphrase == NULL) {
        LOG_E(wifi, "passphrase is null.");
        return WIFI_ERR_PARA_INVALID;
    }
    if ((passphrase_length < 8) || (passphrase_length > WIFI_LENGTH_PASSPHRASE)) {
        LOG_E(wifi, "incorrect length(=%d)", passphrase_length);
        return WIFI_ERR_PARA_INVALID;
    }
    if (passphrase_length == WIFI_LENGTH_PASSPHRASE) {
        for (uint8_t index = 0; index < WIFI_LENGTH_PASSPHRASE; index++) {
            if (!hex_isdigit(passphrase[index])) {
                LOG_E(wifi, "length(=%d) but the strings are not hex strings!", passphrase_length);
                return WIFI_ERR_PARA_INVALID;
            }
        }
    }

    sprintf(pass_len_buf, "%d", passphrase_length);
    os_memcpy(pass_buf, passphrase, passphrase_length);
    pass_buf[passphrase_length] = '\0';

    /***add by Pengfei, optimize PMK calculate at boot up***/
    if (WIFI_PORT_STA == port) {
        uint8_t pmk_info[WIFI_MAX_LENGTH_OF_SSID+WIFI_LENGTH_PASSPHRASE+WIFI_LENGTH_PMK] = {0};
        uint8_t ssid[WIFI_PROFILE_BUFFER_LENGTH] = {0};
        uint8_t ssid_length = 0;
        uint8_t psk[WIFI_LENGTH_PMK];
        if (wifi_profile_get_ssid(port, ssid, &ssid_length) < 0) {
            return WIFI_FAIL;
        }

#ifdef MTK_MINISUPP_ENABLE
        if (__g_wpa_supplicant_api.cal_pmk) {
            if (__g_wpa_supplicant_api.cal_pmk(passphrase, ssid, ssid_length, psk) < 0) {
                LOG_E(wifi, "passphrase length is 64.");
            }
        }
#endif /* MTK_MINISUPP_ENABLE */

        os_memcpy(pmk_info, ssid, ssid_length);
        os_memcpy(pmk_info+WIFI_MAX_LENGTH_OF_SSID, passphrase, passphrase_length);
        os_memcpy(pmk_info+WIFI_MAX_LENGTH_OF_SSID+WIFI_LENGTH_PASSPHRASE, psk, WIFI_LENGTH_PMK);
        if (NVDM_STATUS_OK != nvdm_write_data_item(WIFI_PROFILE_BUFFER_STA, "PMK_INFO", NVDM_DATA_ITEM_TYPE_STRING,
                            (uint8_t *)pmk_info, WIFI_MAX_LENGTH_OF_SSID+WIFI_LENGTH_PASSPHRASE+WIFI_LENGTH_PMK)) {
            return WIFI_FAIL;
        }

    }

    if (port == WIFI_PORT_AP) {
        if (NVDM_STATUS_OK != nvdm_write_data_item(WIFI_PROFILE_BUFFER_AP, "WpaPskLen",
                NVDM_DATA_ITEM_TYPE_STRING,
                (uint8_t *)pass_len_buf, os_strlen(pass_len_buf))) {
            return WIFI_FAIL;
        }
        if (NVDM_STATUS_OK != nvdm_write_data_item(WIFI_PROFILE_BUFFER_AP, "WpaPsk",
                NVDM_DATA_ITEM_TYPE_STRING,
                (uint8_t *)pass_buf, os_strlen(pass_buf))) {
            return WIFI_FAIL;
        }
    } else {
        if (NVDM_STATUS_OK != nvdm_write_data_item(WIFI_PROFILE_BUFFER_STA, "WpaPskLen",
                NVDM_DATA_ITEM_TYPE_STRING,
                (uint8_t *)pass_len_buf, os_strlen(pass_len_buf))) {
            return WIFI_FAIL;
        }
        if (NVDM_STATUS_OK != nvdm_write_data_item(WIFI_PROFILE_BUFFER_STA, "WpaPsk",
                NVDM_DATA_ITEM_TYPE_STRING,
                (uint8_t *)pass_buf, os_strlen(pass_buf))) {
            return WIFI_FAIL;
        }
    }
    return WIFI_SUCC;
}


int32_t wifi_profile_get_wpa_psk_key(uint8_t port, uint8_t *passphrase, uint8_t *passphrase_length)
{
    char pass_len_buf[WIFI_PROFILE_BUFFER_LENGTH] = {0};
    char pass_buf[WIFI_PROFILE_BUFFER_LENGTH] = {0};
    uint32_t pass_len_buf_size = sizeof(pass_len_buf);
    uint32_t pass_buf_size = sizeof(pass_buf);

    if (!wifi_is_port_valid(port)) {
        LOG_E(wifi, "port is invalid: %d", port);
        return WIFI_ERR_PARA_INVALID;
    }
    if (NULL == passphrase || NULL == passphrase_length) {
        LOG_E(wifi, "null input pointer");
        return WIFI_ERR_PARA_INVALID;
    }

    if (port == WIFI_PORT_AP) {
        if (NVDM_STATUS_OK != nvdm_read_data_item(WIFI_PROFILE_BUFFER_AP, "WpaPskLen", (uint8_t *)pass_len_buf, &pass_len_buf_size)) {
            return WIFI_FAIL;
        }
        if (NVDM_STATUS_OK != nvdm_read_data_item(WIFI_PROFILE_BUFFER_AP, "WpaPsk", (uint8_t *)pass_buf, &pass_buf_size)) {
            return WIFI_FAIL;
        }
    } else {
        if (NVDM_STATUS_OK != nvdm_read_data_item(WIFI_PROFILE_BUFFER_STA, "WpaPskLen", (uint8_t *)pass_len_buf, &pass_len_buf_size)) {
            return WIFI_FAIL;
        }
        if (NVDM_STATUS_OK != nvdm_read_data_item(WIFI_PROFILE_BUFFER_STA, "WpaPsk", (uint8_t *)pass_buf, &pass_buf_size)) {
            return WIFI_FAIL;
        }
    }

    if ((strlen(pass_len_buf) == 0) || (strlen(pass_buf) == 0)) {
        *passphrase_length = 0;
        passphrase[0] = '\0';
        return WIFI_SUCC;
    }

    *passphrase_length = atoi(pass_len_buf);
    if (*passphrase_length > WIFI_LENGTH_PASSPHRASE) {
        LOG_E(wifi, "passphrase_length is too big: %d", *passphrase_length);
        return WIFI_FAIL;
    }

    os_memcpy(passphrase, pass_buf, *passphrase_length);
    return WIFI_SUCC;
}


int32_t wifi_profile_set_pmk(uint8_t port, uint8_t *pmk)
{
    char buf[WIFI_PROFILE_BUFFER_LENGTH] = {0};

    if (!wifi_is_port_valid(port)) {
        LOG_E(wifi, "port is invalid: %d", port);
        return WIFI_ERR_PARA_INVALID;
    }
    if (NULL == pmk) {
        LOG_E(wifi, "pmk is null");
        return WIFI_ERR_PARA_INVALID;
    }

    char *buf_ptr = buf;
    for (int i = 0; i < WIFI_LENGTH_PMK; i++) {
        sprintf(buf_ptr, "%02x", pmk[i]);
        buf_ptr += 2;
    }
    *buf_ptr = '\0';

    if (port == WIFI_PORT_AP) {
        if (NVDM_STATUS_OK != nvdm_write_data_item(WIFI_PROFILE_BUFFER_AP, "PMK",
                NVDM_DATA_ITEM_TYPE_STRING,
                (uint8_t *)buf, os_strlen(buf))) {
            return WIFI_FAIL;
        }
    } else {
        if (NVDM_STATUS_OK != nvdm_write_data_item(WIFI_PROFILE_BUFFER_STA, "PMK",
                NVDM_DATA_ITEM_TYPE_STRING,
                (uint8_t *)buf, os_strlen(buf))) {
            return WIFI_FAIL;
        }
    }
    return WIFI_SUCC;
}


int32_t wifi_profile_get_pmk(uint8_t port, uint8_t *pmk)
{
    char buf[WIFI_PROFILE_BUFFER_LENGTH] = {0};
    uint32_t len = sizeof(buf);

    if (!wifi_is_port_valid(port)) {
        LOG_E(wifi, "port is invalid: %d", port);
        return WIFI_ERR_PARA_INVALID;
    }
    if (NULL == pmk) {
        LOG_E(wifi, "pmk is null");
        return WIFI_ERR_PARA_INVALID;
    }

    if (port == WIFI_PORT_AP) {
        if (NVDM_STATUS_OK != nvdm_read_data_item(WIFI_PROFILE_BUFFER_AP, "PMK", (uint8_t *)buf, &len)) {
            return WIFI_FAIL;
        }
    } else {
        if (NVDM_STATUS_OK != nvdm_read_data_item(WIFI_PROFILE_BUFFER_STA, "PMK", (uint8_t *)buf, &len)) {
            return WIFI_FAIL;
        }
    }

    if (os_strlen(buf) == 0) {
        os_memset(pmk, 0, WIFI_LENGTH_PMK);
        return WIFI_SUCC;
    } else if (os_strlen(buf) != (WIFI_LENGTH_PMK * 2)) {
        LOG_E(wifi, "pmk integrity check fail");
        return WIFI_FAIL;
    }

    char segment[3];
    for (int i = 0; i < WIFI_LENGTH_PMK; i++) {
        os_memset(segment, 0, 3);
        os_memcpy(segment, (buf + (i * 2)), 2);
        segment[2] = '\0';
        pmk[i] = (int)strtol(segment, NULL, 16);
    }
    return WIFI_SUCC;
}

int32_t wifi_profile_set_wep_key(uint8_t port, wifi_wep_key_t *wep_keys)
{
    char key_buf[WIFI_PROFILE_BUFFER_LENGTH] = {0};
    char key_len_buf[WIFI_PROFILE_BUFFER_LENGTH] = {0};
    char def_key_id_buf[WIFI_PROFILE_BUFFER_LENGTH] = {0};

    char temp_str[4];
    char temp_str1[WIFI_MAX_WEP_KEY_LENGTH + 2];
    char *key_len_ptr = key_len_buf;
    char *key_ptr = key_buf;

    if (!wifi_is_port_valid(port)) {
        LOG_E(wifi, "port is invalid: %d", port);
        return WIFI_ERR_PARA_INVALID;
    }
    if (NULL == wep_keys) {
        LOG_E(wifi, "wep_keys is null");
        return WIFI_ERR_PARA_INVALID;
    }

    for (uint8_t index = 0; index < WIFI_NUMBER_WEP_KEYS; index++) {
        if (index < WIFI_NUMBER_WEP_KEYS - 1) {
            sprintf(temp_str, "%d,", wep_keys->wep_key_length[index]);
            os_memcpy(temp_str1, wep_keys->wep_key[index], wep_keys->wep_key_length[index]);
            temp_str1[wep_keys->wep_key_length[index]] = ',';
            temp_str1[wep_keys->wep_key_length[index] + 1] = '\0';
        } else {
            sprintf(temp_str, "%d", wep_keys->wep_key_length[index]);
            os_memcpy(temp_str1, wep_keys->wep_key[index], wep_keys->wep_key_length[index]);
            temp_str1[wep_keys->wep_key_length[index]] = '\0';
        }
        strcpy(key_len_ptr, temp_str);
        key_len_ptr += strlen(temp_str);
        strcpy(key_ptr, temp_str1);
        key_ptr += strlen(temp_str1);
        os_memset(temp_str, 0, 4);
        os_memset(temp_str1, 0, WIFI_MAX_WEP_KEY_LENGTH + 2);
    }

    sprintf(def_key_id_buf, "%d", wep_keys->wep_tx_key_index);
    LOG_I(wifi, "wifi_profile_set_wep_key: SharedKey =%s, SharedKeyLen=%s, DefaultKeyId=%s", key_buf, key_len_buf, temp_str);

    if (port == WIFI_PORT_AP) {
        if (NVDM_STATUS_OK != nvdm_write_data_item(WIFI_PROFILE_BUFFER_AP, "SharedKey",
                NVDM_DATA_ITEM_TYPE_STRING,
                (uint8_t *)key_buf, os_strlen(key_buf))) {
            return WIFI_FAIL;
        }
        if (NVDM_STATUS_OK != nvdm_write_data_item(WIFI_PROFILE_BUFFER_AP, "SharedKeyLen",
                NVDM_DATA_ITEM_TYPE_STRING,
                (uint8_t *)key_len_buf, os_strlen(key_len_buf))) {
            return WIFI_FAIL;
        }
        if (NVDM_STATUS_OK != nvdm_write_data_item(WIFI_PROFILE_BUFFER_AP, "DefaultKeyId",
                NVDM_DATA_ITEM_TYPE_STRING,
                (uint8_t *)def_key_id_buf, os_strlen(def_key_id_buf))) {
            return WIFI_FAIL;
        }
    } else {
        if (NVDM_STATUS_OK != nvdm_write_data_item(WIFI_PROFILE_BUFFER_STA, "SharedKey",
                NVDM_DATA_ITEM_TYPE_STRING,
                (uint8_t *)key_buf, os_strlen(key_buf))) {
            return WIFI_FAIL;
        }
        if (NVDM_STATUS_OK != nvdm_write_data_item(WIFI_PROFILE_BUFFER_STA, "SharedKeyLen",
                NVDM_DATA_ITEM_TYPE_STRING,
                (uint8_t *)key_len_buf, os_strlen(key_len_buf))) {
            return WIFI_FAIL;
        }
        if (NVDM_STATUS_OK != nvdm_write_data_item(WIFI_PROFILE_BUFFER_STA, "DefaultKeyId",
                NVDM_DATA_ITEM_TYPE_STRING,
                (uint8_t *)def_key_id_buf, os_strlen(def_key_id_buf))) {
            return WIFI_FAIL;
        }
    }
    return WIFI_SUCC;
}

int32_t wifi_profile_get_wep_key(uint8_t port, wifi_wep_key_t *wep_keys)
{
    char key_len_buf[WIFI_PROFILE_BUFFER_LENGTH] = {0};
    uint32_t key_len_buf_size = sizeof(key_len_buf);

    uint8_t index = 0;
    char *ptr = NULL;

    if (!wifi_is_port_valid(port)) {
        LOG_E(wifi, "port is invalid: %d", port);
        return WIFI_ERR_PARA_INVALID;
    }
    if (NULL == wep_keys) {
        LOG_E(wifi, "wep_keys is null");
        return WIFI_ERR_PARA_INVALID;
    }

    /* WEP KEY LEN */
    if (port == WIFI_PORT_AP) {
        if (NVDM_STATUS_OK != nvdm_read_data_item(WIFI_PROFILE_BUFFER_AP, "SharedKeyLen", (uint8_t *)key_len_buf, &key_len_buf_size)) {
            return WIFI_FAIL;
        }
    } else {
        if (NVDM_STATUS_OK != nvdm_read_data_item(WIFI_PROFILE_BUFFER_STA, "SharedKeyLen", (uint8_t *)key_len_buf, &key_len_buf_size)) {
            return WIFI_FAIL;
        }
    }

    for (index = 0, ptr = rstrtok((char *)key_len_buf, ","); (ptr); ptr = rstrtok(NULL, ",")) {
        wep_keys->wep_key_length[index] = atoi(ptr);
        index++;
        if (index >= WIFI_NUMBER_WEP_KEYS) {
            break;
        }
    }

    /* WEP KEY */
    char key_buf[WIFI_PROFILE_BUFFER_LENGTH] = {0};
    uint32_t key_buf_len = sizeof(key_buf);
    if (port == WIFI_PORT_AP) {
        if (NVDM_STATUS_OK != nvdm_read_data_item(WIFI_PROFILE_BUFFER_AP, "SharedKey", (uint8_t *)key_buf, &key_buf_len)) {
            return WIFI_FAIL;
        }
    } else {
        if (NVDM_STATUS_OK != nvdm_read_data_item(WIFI_PROFILE_BUFFER_STA, "SharedKey", (uint8_t *)key_buf, &key_buf_len)) {
            return WIFI_FAIL;
        }
    }

    for (index = 0, ptr = rstrtok(key_buf, ","); (ptr); ptr = rstrtok(NULL, ",")) {
        if (wep_keys->wep_key_length[index] == 5 ||
                wep_keys->wep_key_length[index] == 13) {
            os_memcpy(&wep_keys->wep_key[index], ptr, wep_keys->wep_key_length[index]);
        } else if (wep_keys->wep_key_length[index] == 10 ||
                   wep_keys->wep_key_length[index] == 26) {
            wep_keys->wep_key_length[index] /= 2;
            AtoH(ptr, (char *)&wep_keys->wep_key[index], (int)wep_keys->wep_key_length[index]);
        } else {
            //printf("WEP Key Length(=%d) is incorrect.\n", wep_keys->wep_key_length[index]);
        }
        index++;
        if (index >= WIFI_NUMBER_WEP_KEYS) {
            break;
        }
    }

    /* Default key ID */
    char def_key_id_buf[WIFI_PROFILE_BUFFER_LENGTH] = {0};
    uint32_t def_key_id_buf_size = sizeof(def_key_id_buf);
    if (port == WIFI_PORT_AP) {
        if (NVDM_STATUS_OK != nvdm_read_data_item(WIFI_PROFILE_BUFFER_AP, "DefaultKeyId", (uint8_t *)def_key_id_buf, &def_key_id_buf_size)) {
            return WIFI_FAIL;
        }
    } else {
        if (NVDM_STATUS_OK != nvdm_read_data_item(WIFI_PROFILE_BUFFER_STA, "DefaultKeyId", (uint8_t *)def_key_id_buf, &def_key_id_buf_size)) {
            return WIFI_FAIL;
        }
    }

    wep_keys->wep_tx_key_index = (uint8_t)atoi(def_key_id_buf);
    return WIFI_SUCC;
}


int32_t wifi_profile_get_country_region(uint8_t band, uint8_t *region)
{
    if (!wifi_is_band_valid(band)) {
        LOG_E(wifi, "band is invalid: %d", band);
        return WIFI_ERR_PARA_INVALID;
    }

    int32_t support_5g = 0;

    support_5g = wifi_5g_support();
    if ((band == WIFI_BAND_5_G) && (support_5g < 0)) {
        LOG_E(wifi, "Chip doesn't support 5G.");
        return WIFI_ERR_NOT_SUPPORT;
    }

    if (NULL == region) {
        LOG_E(wifi, "region is null");
        return WIFI_ERR_PARA_INVALID;
    }

    char buf[WIFI_PROFILE_BUFFER_LENGTH] = {0};
    uint32_t len = sizeof(buf);

    if (band == WIFI_BAND_2_4_G) {
        if (NVDM_STATUS_OK != nvdm_read_data_item(WIFI_PROFILE_BUFFER_COMMON, "CountryRegion", (uint8_t *)buf, &len)) {
            return WIFI_FAIL;
        }
    } else {
        if (NVDM_STATUS_OK != nvdm_read_data_item(WIFI_PROFILE_BUFFER_COMMON, "CountryRegionABand", (uint8_t *)buf, &len)) {
            return WIFI_FAIL;
        }
    }

    *region = atoi(buf);
    return WIFI_SUCC;
}

int32_t wifi_profile_set_country_region(uint8_t band, uint8_t region)
{
    char buf[WIFI_PROFILE_BUFFER_LENGTH] = {0};
    int32_t support_5g = 0;

    if (!wifi_is_band_valid(band)) {
        LOG_E(wifi, "band is invalid: %d", band);
        return WIFI_ERR_PARA_INVALID;
    }

    sprintf(buf, "%d", region);

    support_5g = wifi_5g_support();

    if (band == WIFI_BAND_2_4_G) {
        if (NVDM_STATUS_OK != nvdm_write_data_item(WIFI_PROFILE_BUFFER_COMMON, "CountryRegion",
                NVDM_DATA_ITEM_TYPE_STRING,
                (uint8_t *)buf, os_strlen(buf))) {
            return WIFI_FAIL;
        }
        if (wifi_get_channel_list(WIFI_BAND_2_4_G, region, buf) >= 0) {
            if (NVDM_STATUS_OK != nvdm_write_data_item("common", "BGChannelTable",
                    NVDM_DATA_ITEM_TYPE_STRING,
                    (uint8_t *)buf, os_strlen(buf))) {
                return WIFI_FAIL;
            }
        }
    } else {
        if (support_5g < 0) {
            LOG_E(wifi, "Chip doesn't support 5G.");
            return WIFI_ERR_NOT_SUPPORT;
		}
        if (NVDM_STATUS_OK != nvdm_write_data_item(WIFI_PROFILE_BUFFER_COMMON, "CountryRegionABand",
                NVDM_DATA_ITEM_TYPE_STRING,
                (uint8_t *)buf, os_strlen(buf))) {
            return WIFI_FAIL;
        }
        if (wifi_get_channel_list(WIFI_BAND_5_G, region, buf) >= 0) {
            if (NVDM_STATUS_OK != nvdm_write_data_item("common", "AChannelTable",
                    NVDM_DATA_ITEM_TYPE_STRING,
                    (uint8_t *)buf, os_strlen(buf))) {
                return WIFI_FAIL;
            }
        }
    }
    return WIFI_SUCC;
}


int32_t wifi_profile_get_dtim_interval(uint8_t *interval)
{
    char buf[WIFI_PROFILE_BUFFER_LENGTH] = {0};
    uint32_t len = sizeof(buf);

    if (NULL == interval) {
        LOG_E(wifi, "interval is null");
        return WIFI_ERR_PARA_INVALID;
    }

    if (NVDM_STATUS_OK != nvdm_read_data_item(WIFI_PROFILE_BUFFER_AP, "DtimPeriod", (uint8_t *)buf, &len)) {
        return WIFI_FAIL;
    }

    *interval = atoi(buf);
    return WIFI_SUCC;
}


int32_t wifi_profile_set_dtim_interval(uint8_t interval)
{
    char buf[WIFI_PROFILE_BUFFER_LENGTH] = {0};

    if (0 == interval) {
        LOG_E(wifi, "interval is invalid: %d", interval);
        return WIFI_ERR_PARA_INVALID;
    }

    sprintf(buf, "%d", interval);
    if (NVDM_STATUS_OK != nvdm_write_data_item(WIFI_PROFILE_BUFFER_AP, "DtimPeriod",
            NVDM_DATA_ITEM_TYPE_STRING,
            (const uint8_t *)buf, os_strlen(buf))) {
        return WIFI_FAIL;
    }

    return WIFI_SUCC;
}


int32_t wifi_profile_get_listen_interval(uint8_t *interval)
{
    char buf[WIFI_PROFILE_BUFFER_LENGTH] = {0};
    uint32_t len = sizeof(buf);

    if (NULL == interval) {
        LOG_E(wifi, "interval is null");
        return WIFI_ERR_PARA_INVALID;
    }

    if (NVDM_STATUS_OK != nvdm_read_data_item(WIFI_PROFILE_BUFFER_STA, "ListenInterval", (uint8_t *)buf, &len)) {
        return WIFI_FAIL;
    }

    *interval = atoi(buf);
    return WIFI_SUCC;
}


int32_t wifi_profile_set_listen_interval(uint8_t interval)
{
    char buf[WIFI_PROFILE_BUFFER_LENGTH] = {0};

    if (0 == interval) {
        LOG_E(wifi, "interval is invalid: %d", interval);
        return WIFI_ERR_PARA_INVALID;
    }

    sprintf(buf, "%d", interval);

    if (NVDM_STATUS_OK != nvdm_write_data_item(WIFI_PROFILE_BUFFER_STA, "ListenInterval",
            NVDM_DATA_ITEM_TYPE_STRING,
            (const uint8_t *)buf, os_strlen(buf))) {
        return WIFI_FAIL;
    }

    return WIFI_SUCC;
}

int32_t wifi_profile_get_power_save_mode(uint8_t *power_save_mode)
{
    char buf[WIFI_PROFILE_BUFFER_LENGTH] = {0};
    uint32_t len = sizeof(buf);

    if (NULL == power_save_mode) {
        LOG_E(wifi, "power_save_mode is null");
        return WIFI_ERR_PARA_INVALID;
    }

    if (NVDM_STATUS_OK != nvdm_read_data_item(WIFI_PROFILE_BUFFER_STA, "PSMode", (uint8_t *)buf, &len)) {
        return WIFI_FAIL;
    }

    *power_save_mode = atoi(buf);
    return WIFI_SUCC;
}

int32_t wifi_profile_set_power_save_mode(uint8_t power_save_mode)
{
    char buf[WIFI_PROFILE_BUFFER_LENGTH] = {0};

    if (!wifi_is_ps_mode_valid(power_save_mode)) {
        LOG_E(wifi, "power_save_mode is invalid: %d", power_save_mode);
        return WIFI_ERR_PARA_INVALID;
    }

    sprintf(buf, "%d", power_save_mode);
    if (nvdm_write_data_item(WIFI_PROFILE_BUFFER_STA, "PSMode",
                             NVDM_DATA_ITEM_TYPE_STRING,
                             (const uint8_t *)buf, os_strlen(buf))) {
        return WIFI_FAIL;
    }

    return WIFI_SUCC;
}

int32_t wifi_profile_get_configfree(uint8_t *config_ready)
{
    char buf[WIFI_PROFILE_BUFFER_LENGTH] = {0};
    uint32_t len = sizeof(buf);

    if (NULL == config_ready) {
        LOG_E(wifi, "config_ready is null");
        return WIFI_ERR_PARA_INVALID;
    }

    if (NVDM_STATUS_OK != nvdm_read_data_item(WIFI_PROFILE_BUFFER_COMMON, "ConfigFree_Ready", (uint8_t *)buf, &len)) {
        return WIFI_FAIL;
    }

    *config_ready = atoi(buf);
    return WIFI_SUCC;
}

int32_t wifi_profile_set_configfree(uint8_t config_ready)
{
    char buf[WIFI_PROFILE_BUFFER_LENGTH] = {0};

    if (0 != config_ready && 1 != config_ready) {
        LOG_E(wifi, "config_ready is invalid: %d", config_ready);
        return WIFI_ERR_PARA_INVALID;
    }

    sprintf(buf, "%d", WIFI_MODE_REPEATER);
    LOG_I(wifi, "wifi_profile_set_opmode: opmode=%s", buf);

    if (NVDM_STATUS_OK != nvdm_write_data_item(WIFI_PROFILE_BUFFER_COMMON, "OpMode",
            NVDM_DATA_ITEM_TYPE_STRING,
            (uint8_t *)buf, os_strlen(buf))) {
        return WIFI_FAIL;
    }

    os_memset(buf, 0x0, WIFI_PROFILE_BUFFER_LENGTH);

    sprintf(buf, "%d", config_ready);
    LOG_I(wifi, "ConfigFree ready: %s", buf);

    if (NVDM_STATUS_OK != nvdm_write_data_item(WIFI_PROFILE_BUFFER_COMMON, "ConfigFree_Ready",
            NVDM_DATA_ITEM_TYPE_STRING,
            (uint8_t *)buf, os_strlen(buf))) {
        return WIFI_FAIL;
    }

    return WIFI_SUCC;
}
#endif //MTK_WIFI_PROFILE_ENABLE

