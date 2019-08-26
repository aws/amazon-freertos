/** @file wlan_smc.c
 *
 *  @brief  This file provides API to set WLAN Parameters
 *
 *  (C) Copyright 2008-2018 Marvell International Ltd. All Rights Reserved.
 *
 *  MARVELL CONFIDENTIAL
 *  The source code contained or described herein and all documents related to
 *  the source code ("Material") are owned by Marvell International Ltd or its
 *  suppliers or licensors. Title to the Material remains with Marvell
 *  International Ltd or its suppliers and licensors. The Material contains
 *  trade secrets and proprietary and confidential information of Marvell or its
 *  suppliers and licensors. The Material is protected by worldwide copyright
 *  and trade secret laws and treaty provisions. No part of the Material may be
 *  used, copied, reproduced, modified, published, uploaded, posted,
 *  transmitted, distributed, or disclosed in any way without Marvell's prior
 *  express written permission.
 *
 *  No license under any patent, copyright, trade secret or other intellectual
 *  property right is granted to or conferred upon you by disclosure or delivery
 *  of the Materials, either expressly, by implication, inducement, estoppel or
 *  otherwise. Any license under such intellectual property rights must be
 *  express and approved by Marvell in writing.
 *
 */


#include <wlan.h>
#include <wlan_smc.h>
#include <wifi.h>

extern void sniffer_register_callback(void (*sniffer_cb)
				(const wlan_frame_t *frame,
				 const uint16_t len));
extern void wlan_set_smart_mode_active();
extern void wlan_set_smart_mode_inactive();
extern bool wlan_get_smart_mode_status();

int wlan_print_smart_mode_cfg()
{
	return wifi_get_smart_mode_cfg();
}

#define FIRST_2_4_CHANNEL 1
#define NUM_2_4_CHANNELS 14
static wifi_chan_list_param_set_t *populate_channel_information(
	struct wlan_network *network_info, smart_mode_cfg_t *cfg)
{
	int chan, first_chan, num_chan, cnt_chan = 0;

	if (cfg->channel) {
		first_chan = cfg->channel - 1;
		num_chan = 3;
	} else {
		first_chan = FIRST_2_4_CHANNEL;
		num_chan = NUM_2_4_CHANNELS;
	}

	for (chan = first_chan; chan < (first_chan + num_chan); chan++) {
		if (wifi_11d_is_channel_allowed(chan))
			cnt_chan++;
	}

	wifi_chan_list_param_set_t *chan_list = os_mem_alloc(
		sizeof(wifi_chan_list_param_set_t) +
		(sizeof(wifi_chan_scan_param_set_t) * (cnt_chan)));
	chan_list->no_of_channels = cnt_chan + 1;

	/* Populate uAP channel information at index 0 of chan_list */
	chan_list->chan_scan_param[0].chan_number = network_info->channel;
	chan_list->chan_scan_param[0].min_scan_time = 30;
	chan_list->chan_scan_param[0].max_scan_time = 30;

	cnt_chan = 1;
	for (chan = first_chan; chan < (first_chan + num_chan); chan++) {
		if (!wifi_11d_is_channel_allowed(chan))
			continue;
		chan_list->chan_scan_param[cnt_chan].chan_number = chan;
		chan_list->chan_scan_param[cnt_chan].min_scan_time =
			cfg->min_scan_time;
		chan_list->chan_scan_param[cnt_chan].max_scan_time =
			cfg->max_scan_time;
		cnt_chan++;
	}
	return chan_list;
}

#define COUNTRY_INFO_TYPE 0x07
static void populate_country_code_information(smart_mode_cfg_t *cfg)
{
	int nr_sb;
	uint8_t *ptr = cfg->custom_ie + cfg->custom_ie_len;
	wifi_sub_band_set_t *sub_band = get_sub_band_from_country(
		cfg->country_code, &nr_sb);
	uint8_t country_info_type = COUNTRY_INFO_TYPE;
	uint8_t sub_band_len = sizeof(wifi_sub_band_set_t) * nr_sb;
	uint8_t country_info_len = sub_band_len + COUNTRY_CODE_LEN;

	*ptr = country_info_type;
	cfg->custom_ie_len += sizeof(country_info_type);
	ptr += sizeof(country_info_type);

	*ptr = country_info_len;
	cfg->custom_ie_len += sizeof(country_info_len);
	ptr += sizeof(country_info_len);

	memcpy(ptr, wlan_11d_country_index_2_string(cfg->country_code),
	       COUNTRY_CODE_LEN);
	cfg->custom_ie_len += COUNTRY_CODE_LEN;
	ptr += COUNTRY_CODE_LEN;

	memcpy(ptr, sub_band, sub_band_len);
	cfg->custom_ie_len += sub_band_len;
	ptr += sub_band_len;
}

/* API to populate rsn information required in beacon to indicate that the
 * micro-AP is WPA2 secured
 */
static void populate_rsn_information(smart_mode_cfg_t *cfg)
{
	uint8_t *ptr = cfg->custom_ie + cfg->custom_ie_len;
	uint8_t buffer[] = {
		/* Tag Number: RSN information */
		0x30,
		/* Length */
		0x14,
		/* RSN version */
		0x01, 0x00,
		/* Group Cipher Suite: (IEEE8021) AES (CCM)
		 *
		 * Further expansion:
		 * Group Cipher Suite OUI: 00 0f ac (IEEE8021)
		 * Group Cipher Suite type: 04 (AES CCM)
		 */
		0x00, 0x0f, 0xac, 0x04,
		/* Pairwise Cipher Suite count */
		0x01, 0x00,
		/* Pairwise Cipher Suite: (IEEE8021) AES (CCM)
		 *
		 * Further expansion:
		 * Pairwise Cipher Suite OUI: 00 0f ac (IEEE8021)
		 * Pairwise Cipher Suite type: 04 (AES CCM)
		 */
		0x00, 0x0f, 0xac, 0x04,
		/* Auth Key Management Suite Count: 1 */
		0x01, 0x00,
		/* Auth Key Management List (IEEE8021) PSK
		 *
		 * Further expansion:
		 * Auth Key Management OUI: 00 0f ac (IEEE8021)
		 * Auth Key Management type: 02 (PSK)
		 */
		0x00, 0x0f, 0xac, 0x02,
		/* RSN Capabilities */
		0x00, 0x00};
	memcpy(ptr, buffer, sizeof(buffer));
	cfg->custom_ie_len += sizeof(buffer);
}


int wlan_set_smart_mode_cfg(struct wlan_network *uap_network,
			    smart_mode_cfg_t *cfg, void (*sniffer_callback)
			    (const wlan_frame_t *frame, const uint16_t len))
{
	int ret;
	uint8_t *smc_start_addr = NULL, *smc_end_addr = NULL;
	uint8_t mac_zero[] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
	wifi_chan_list_param_set_t *channel_list;
	if (!cfg->country_code)
		wlan_set_country(COUNTRY_US);
	channel_list = populate_channel_information(uap_network, cfg);
	populate_country_code_information(cfg);
	if (uap_network->security.type == WLAN_SECURITY_WPA2)
		populate_rsn_information(cfg);

	sniffer_register_callback(sniffer_callback);
	if (memcmp(cfg->smc_start_addr, mac_zero, sizeof(MLAN_MAC_ADDR_LENGTH))
	   &&
	   memcmp(cfg->smc_end_addr, mac_zero, sizeof(MLAN_MAC_ADDR_LENGTH))) {
		smc_start_addr = cfg->smc_start_addr;
		smc_end_addr = cfg->smc_end_addr;
	}
	ret = wifi_set_smart_mode_cfg(uap_network->ssid, cfg->beacon_period,
				       channel_list, smc_start_addr,
				       smc_end_addr, cfg->filter_type,
				       cfg->smc_frame_filter_len,
				       cfg->smc_frame_filter,
				       cfg->custom_ie_len, cfg->custom_ie);
	os_mem_free(channel_list);
	return ret;
}

int wlan_start_smart_mode()
{
	if (wlan_get_smart_mode_status() == true)
		return WM_SUCCESS;
	wlan_set_smart_mode_active();
	return wifi_start_smart_mode();
}

int wlan_stop_smart_mode()
{
	if (wlan_get_smart_mode_status() == false)
		return WM_SUCCESS;
	wlan_set_smart_mode_inactive();
	return wifi_stop_smart_mode();
}

int wlan_stop_smart_mode_start_uap(struct wlan_network *uap_network)
{
	int ret;
	uint8_t mac_addr[6];

	ret = wlan_stop_smart_mode();
	if (ret != WM_SUCCESS) {
		wlcm_e("Failed to stop smart mode");
		return ret;
	}

	uap_network->type = WLAN_BSS_TYPE_UAP;
	wlan_get_mac_address(mac_addr);

	wlan_remove_network(uap_network->name);

	ret = wlan_add_network(uap_network);
	if (ret != WM_SUCCESS) {
		return -WM_FAIL;
	}
	ret = wlan_start_network(uap_network->name);
	if (ret != WM_SUCCESS) {
		return -WM_FAIL;
	}
	if (ret) {
		wlcm_e("uAP start failed, giving up");
		return -WM_FAIL;
	}
	return WM_SUCCESS;
}
