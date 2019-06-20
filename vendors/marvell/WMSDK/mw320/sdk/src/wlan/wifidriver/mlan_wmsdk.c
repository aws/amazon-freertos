/** @file mlan_wmsdk.c
 *
 *  @brief This file provides more APIs for mlan.
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

#include <mlan_wmsdk.h>

/* Additional WMSDK header files */
#include <wmstdio.h>
#include <wmassert.h>
#include <wmerrno.h>
#include <wm_os.h>

#include <wifi.h>

#include "wifi-sdio.h"
#include "wifi-internal.h"

/*
 * Bit 0 : Assoc Req
 * Bit 1 : Assoc Resp
 * Bit 2 : ReAssoc Req
 * Bit 3 : ReAssoc Resp
 * Bit 4 : Probe Req
 * Bit 5 : Probe Resp
 * Bit 8 : Beacon
 */
/** Mask for Assoc request frame */
#define MGMT_MASK_ASSOC_REQ  0x01
/** Mask for ReAssoc request frame */
#define MGMT_MASK_REASSOC_REQ  0x04
/** Mask for Assoc response frame */
#define MGMT_MASK_ASSOC_RESP  0x02
/** Mask for ReAssoc response frame */
#define MGMT_MASK_REASSOC_RESP  0x08
/** Mask for probe request frame */
#define MGMT_MASK_PROBE_REQ  0x10
/** Mask for probe response frame */
#define MGMT_MASK_PROBE_RESP 0x20
/** Mask for beacon frame */
#define MGMT_MASK_BEACON 0x100
/** Mask to clear previous settings */
#define MGMT_MASK_CLEAR 0x000

static const char driver_version_format[] = "SD878x-%s-%s-WM";
static const char driver_version[] = "702.1.0";

static unsigned int mgmt_ie_index_bitmap = 0x00;
static int wifi_11d_country = 0x00;

/* This were static functions in mlan file */
mlan_status wlan_cmd_802_11_deauthenticate(IN pmlan_private pmpriv,
                               IN HostCmd_DS_COMMAND * cmd,
                               IN t_void * pdata_buf);
mlan_status wlan_cmd_reg_access(IN HostCmd_DS_COMMAND * cmd,
				IN t_u16 cmd_action, IN t_void * pdata_buf);
mlan_status wlan_cmd_mem_access(IN HostCmd_DS_COMMAND * cmd,
                    IN t_u16 cmd_action, IN t_void * pdata_buf);
mlan_status wlan_cmd_bridge_mode(IN HostCmd_DS_COMMAND * cmd,
		    IN t_u16 cmd_action, IN t_void * pdata_buf);
mlan_status wlan_cmd_recfg_tx_buf(mlan_private * priv,
		    HostCmd_DS_COMMAND * cmd, int cmd_action, void *pdata_buf);
mlan_status wlan_cmd_auto_reconnect(IN HostCmd_DS_COMMAND * cmd,
                    IN t_u16 cmd_action, IN t_void * pdata_buf);
mlan_status wlan_cmd_rx_mgmt_indication(IN pmlan_private pmpriv,
		IN HostCmd_DS_COMMAND * cmd,
		IN t_u16 cmd_action, IN t_void * pdata_buf);
mlan_status wlan_misc_ioctl_region(IN pmlan_adapter pmadapter,
				   IN pmlan_ioctl_req pioctl_req);

int wifi_deauthenticate(uint8_t *bssid)
{
	HostCmd_DS_COMMAND *cmd = wifi_get_command_buffer();

	wifi_get_command_lock();

	 /* fixme: check if this static selection is ok */
#ifdef CONFIG_P2P
	cmd->seq_num = (0x01) << 13;
#else
	cmd->seq_num = 0x0;
#endif
	cmd->result = 0x0;
	
	wlan_cmd_802_11_deauthenticate((mlan_private *) mlan_adap->priv[0],
					cmd, bssid);
	wifi_wait_for_cmdresp(NULL);
	return WM_SUCCESS;
}

int wifi_get_eeprom_data(uint32_t offset, uint32_t byte_count, uint8_t *buf)
{
	mlan_ds_read_eeprom eeprom_rd;
	eeprom_rd.offset = offset;
	eeprom_rd.byte_count = byte_count;

	wifi_get_command_lock();
	HostCmd_DS_COMMAND *cmd = wifi_get_command_buffer();

	cmd->command = HostCmd_CMD_802_11_EEPROM_ACCESS;
	cmd->seq_num = 0x0;
	cmd->result = 0x0;

	wlan_cmd_reg_access(cmd, HostCmd_ACT_GEN_GET, &eeprom_rd);
	wifi_wait_for_cmdresp(buf);
	return wm_wifi.cmd_resp_status;
}

int wifi_reg_access(wifi_reg_t reg_type, uint16_t action,
		    uint32_t offset, uint32_t *value)
{
	mlan_ds_reg_rw reg_rw;
	reg_rw.offset = offset;
	reg_rw.value = *value;
	uint16_t hostcmd;
	switch (reg_type) {
	case REG_MAC:
		hostcmd = HostCmd_CMD_MAC_REG_ACCESS;
		break;
	case REG_BBP:
		hostcmd = HostCmd_CMD_BBP_REG_ACCESS;
		break;
	case REG_RF:
		hostcmd = HostCmd_CMD_RF_REG_ACCESS;
		break;
	default:
		wifi_e("Incorrect register type");
		return -WM_FAIL;
		break;
	}
	
	wifi_get_command_lock();
	HostCmd_DS_COMMAND *cmd = wifi_get_command_buffer();

	cmd->command = hostcmd;
	cmd->seq_num = 0x0;
	cmd->result = 0x0;
	
	wlan_cmd_reg_access(cmd, action, &reg_rw);
	wifi_wait_for_cmdresp(action == HostCmd_ACT_GEN_GET ? value : NULL);
	return wm_wifi.cmd_resp_status;
}

int wifi_mem_access(uint16_t action, uint32_t addr, uint32_t *value)
{
	mlan_ds_mem_rw mem_rw;
	mem_rw.addr = addr;
	mem_rw.value = *value;

	wifi_get_command_lock();
	HostCmd_DS_COMMAND *cmd = wifi_get_command_buffer();

	cmd->command = HostCmd_CMD_MEM_ACCESS;
	cmd->seq_num = 0x0;
	cmd->result = 0x0;

	wlan_cmd_mem_access(cmd, action, &mem_rw);

	wifi_wait_for_cmdresp(action == HostCmd_ACT_GEN_GET ? value : NULL);
	return wm_wifi.cmd_resp_status;
}

static int wifi_auto_reconnect(uint16_t action, wifi_auto_reconnect_config_t
		*auto_reconnect_config)
{
	mlan_ds_auto_reconnect auto_reconnect;

	memset(&auto_reconnect, 0x00, sizeof(mlan_ds_auto_reconnect));

	if (auto_reconnect_config) {
		auto_reconnect.reconnect_counter =
			auto_reconnect_config->reconnect_counter;
		auto_reconnect.reconnect_interval =
			auto_reconnect_config->reconnect_interval;
		auto_reconnect.flags =
			auto_reconnect_config->flags;
	}

	wifi_get_command_lock();
	HostCmd_DS_COMMAND *cmd = wifi_get_command_buffer();

	cmd->command = HostCmd_CMD_AUTO_RECONNECT;
	cmd->seq_num = 0x0;
	cmd->result = 0x0;

	wlan_cmd_auto_reconnect(cmd, action, &auto_reconnect);

	wifi_wait_for_cmdresp(action == HostCmd_ACT_GEN_GET ? auto_reconnect_config : NULL);

	return wm_wifi.cmd_resp_status;
}

int wifi_auto_reconnect_enable(wifi_auto_reconnect_config_t
		auto_reconnect_config)
{
	return wifi_auto_reconnect(HostCmd_ACT_GEN_SET, &auto_reconnect_config);
}

int wifi_auto_reconnect_disable()
{
	return wifi_auto_reconnect(HostCmd_ACT_GEN_SET, NULL);
}

int wifi_get_auto_reconnect_config(wifi_auto_reconnect_config_t
		*auto_reconnect_config)
{
	return wifi_auto_reconnect(HostCmd_ACT_GEN_GET, auto_reconnect_config);
}

int wifi_set_rx_mgmt_indication(unsigned int bss_type,
		unsigned int mgmt_subtype_mask)
{
	mlan_private *pmpriv = (mlan_private *) mlan_adap->priv[0];

	mlan_ds_rx_mgmt_indication rx_mgmt_indication;

	memset(&rx_mgmt_indication, 0x00, sizeof(mlan_ds_rx_mgmt_indication));

	rx_mgmt_indication.mgmt_subtype_mask = mgmt_subtype_mask;

	wifi_get_command_lock();
	HostCmd_DS_COMMAND *cmd = wifi_get_command_buffer();

	cmd->command = HostCmd_CMD_RX_MGMT_IND;
#ifdef CONFIG_P2P
	cmd->seq_num = HostCmd_SET_SEQ_NO_BSS_INFO(
						   0 /* seq_num */,
						   0 /* bss_num */,
						   MLAN_BSS_TYPE_WIFIDIRECT);
#else
	cmd->seq_num = HostCmd_SET_SEQ_NO_BSS_INFO(
						   0 /* seq_num */,
						   0 /* bss_num */,
						   bss_type);
#endif /* CONFIG_P2P */
	cmd->result = 0x0;

	wlan_cmd_rx_mgmt_indication(pmpriv, cmd,
		HostCmd_ACT_GEN_SET, &rx_mgmt_indication);

	wifi_wait_for_cmdresp(NULL);

	return wm_wifi.cmd_resp_status;
}

static int wifi_bridge_mode(uint16_t action, uint8_t *enable, wifi_bridge_cfg_t *bridgecfg)
{
	mlan_bridge_mode bridge_mode;
	bridge_mode.enable = *enable;

	t_u8 *tlv = NULL;
	MrvlIEtypes_BridgeParamSet_t *bridge_params = NULL;
	MrvlIEtypes_AutoLinkParamSet_t *autolink_params=NULL;
	MrvlIEtypes_SsIdParamSet_t *ssid = NULL;
	MrvlIEtypes_Passphrase_t *pass = NULL;

	wifi_get_command_lock();
	HostCmd_DS_COMMAND *cmd = wifi_get_command_buffer();

	cmd->command = HostCmd_CMD_BRIDGE_MODE;
	cmd->seq_num = 0x0;
	cmd->result = 0x0;

	wlan_cmd_bridge_mode(cmd, action, &bridge_mode);

	if (action == HostCmd_ACT_GEN_GET)
		goto done;

	if (bridgecfg == NULL)
		goto done;

	tlv = (t_u8 *) cmd + sizeof(HostCmd_BRIDGE_MODE)+ S_DS_GEN;

	bridge_params = (MrvlIEtypes_BridgeParamSet_t *) tlv;

	bridge_params->header.type = wlan_cpu_to_le16(TLV_TYPE_BRIDGE_PARAM);
	bridge_params->header.len = 4 * sizeof(MrvlIEtypesHeader_t) +
		bridgecfg->ex_ap_ssid_len + bridgecfg->ex_ap_pass_len +
		bridgecfg->bridge_ssid_len + bridgecfg->bridge_pass_len;
	tlv = (t_u8 *) bridge_params;

	ssid = (MrvlIEtypes_SsIdParamSet_t *) (tlv +
			sizeof(MrvlIEtypesHeader_t));
	ssid->header.type = wlan_cpu_to_le16(TLV_TYPE_SSID);
	ssid->header.len = bridgecfg->ex_ap_ssid_len;
	memcpy(ssid->ssid, bridgecfg->ex_ap_ssid, bridgecfg->ex_ap_ssid_len);
	tlv = (t_u8 *) ssid;

	pass = (MrvlIEtypes_Passphrase_t *) (tlv + sizeof(MrvlIEtypesHeader_t)
			+ ssid->header.len);
	pass->header.type = wlan_cpu_to_le16(TLV_TYPE_PASSPHRASE);
	pass->header.len = bridgecfg->ex_ap_pass_len;
	memcpy(pass->passphrase, bridgecfg->ex_ap_pass,
			bridgecfg->ex_ap_pass_len);

	tlv = (t_u8 *) pass;

	ssid = (MrvlIEtypes_SsIdParamSet_t *) (tlv + sizeof(MrvlIEtypesHeader_t)
			+ pass->header.len);
	ssid->header.type = wlan_cpu_to_le16(TLV_TYPE_SSID);
	ssid->header.len = bridgecfg->bridge_ssid_len;
	memcpy(ssid->ssid, bridgecfg->bridge_ssid, bridgecfg->bridge_ssid_len);

	tlv = (t_u8 *) ssid;

	pass = (MrvlIEtypes_Passphrase_t *) (tlv + sizeof(MrvlIEtypesHeader_t)
			+ ssid->header.len);
	pass->header.type = wlan_cpu_to_le16(TLV_TYPE_PASSPHRASE);
	pass->header.len = bridgecfg->bridge_pass_len;
	memcpy(pass->passphrase, bridgecfg->bridge_pass,
			bridgecfg->bridge_pass_len);

	cmd->size += bridge_params->header.len + sizeof(MrvlIEtypesHeader_t);

	/*prepare autolink info*/
	tlv = (t_u8 *) pass;
	autolink_params=(MrvlIEtypes_AutoLinkParamSet_t *)(tlv+sizeof(MrvlIEtypesHeader_t)
	        +pass->header.len);
	autolink_params->header.type = wlan_cpu_to_le16(TLV_TYPE_AUTOLINK_PARAM);
	autolink_params->header.len = sizeof(MrvlIEtypes_AutoLinkParamSet_t)-
	        sizeof(MrvlIEtypesHeader_t);
	autolink_params->scan_timer_interval=
	        wlan_cpu_to_le32(bridgecfg->autolink.scan_timer_interval);
	autolink_params->scan_timer_condition=bridgecfg->autolink.scan_timer_condition;
	autolink_params->scan_channel_list=bridgecfg->autolink.scan_channel_list;
	cmd->size += autolink_params->header.len + sizeof(MrvlIEtypesHeader_t);

done:

	wifi_wait_for_cmdresp(action == HostCmd_ACT_GEN_GET ? bridgecfg : NULL);

	return wm_wifi.cmd_resp_status;
}


int wifi_enable_bridge_mode(wifi_bridge_cfg_t *cfg)
{
	uint8_t enable = 0x01;

	if (cfg->auto_link) {
		enable |= 1 << ENABLE_AUTOLINK_BIT;
	}
	if (cfg->hidden_ssid) {
		enable |= 1 << HIDDEN_SSID_BIT;
	}

	return wifi_bridge_mode(HostCmd_ACT_GEN_SET, &enable, cfg);
}

int wifi_disable_bridge_mode()
{
	uint8_t disable = 0x00;
	return wifi_bridge_mode(HostCmd_ACT_GEN_SET, &disable, NULL);
}

int wifi_get_bridge_mode_config(wifi_bridge_cfg_t *cfg)
{
	uint8_t config = 0x00;
	return wifi_bridge_mode(HostCmd_ACT_GEN_GET, &config, cfg);
}

int wifi_config_bridge_tx_buf(uint16_t buf_size)
{
	wifi_get_command_lock();
	HostCmd_DS_COMMAND *cmd = wifi_get_command_buffer();

	cmd->command = HostCmd_CMD_RECONFIGURE_TX_BUFF;
	cmd->seq_num = 0x0;
	cmd->result = 0x0;

	wlan_cmd_recfg_tx_buf((mlan_private *) mlan_adap->priv[0],
				cmd, HostCmd_ACT_GEN_SET, &buf_size);

	wifi_wait_for_cmdresp(NULL);
	return wm_wifi.cmd_resp_status;
}

int wifi_send_rssi_info_cmd(wifi_rssi_info_t *rssi_info)
{
	wifi_get_command_lock();
	HostCmd_DS_COMMAND *cmd = wifi_get_command_buffer();
	
	cmd->seq_num = 0x0;
	cmd->result = 0x0;

	mlan_status rv = wlan_ops_sta_prepare_cmd(
						  (mlan_private *) mlan_adap->priv[0],
						  HostCmd_CMD_RSSI_INFO,
						  HostCmd_ACT_GEN_GET,
						  0, NULL, NULL,  cmd);
	if (rv != MLAN_STATUS_SUCCESS)
		return -WM_FAIL;

	wifi_wait_for_cmdresp(rssi_info);
	return wm_wifi.cmd_resp_status;
}

int wifi_send_netmon_cmd(int action, wifi_net_monitor_t *net_mon)
{
	wifi_get_command_lock();
	HostCmd_DS_COMMAND *cmd = wifi_get_command_buffer();

	cmd->seq_num = 0x0;
	cmd->result = 0x0;

	mlan_status rv = wlan_ops_sta_prepare_cmd(
						  (mlan_private *) mlan_adap->priv[0],
						  HostCmd_CMD_802_11_NET_MONITOR,
						  action,
						  0, NULL, net_mon,  cmd);
	if (rv != MLAN_STATUS_SUCCESS)
		return -WM_FAIL;

	wifi_wait_for_cmdresp(net_mon);
	return wm_wifi.cmd_resp_status;
}

int wifi_sniffer_start(const t_u16 filter_flags, const t_u8 radio_type,
			const t_u8 channel)
{
	wifi_net_monitor_t net_mon;

	memset(&net_mon, 0, sizeof(wifi_net_monitor_t));

	wm_wifi.chan_num = channel;

	net_mon.monitor_activity = 0x01;
	net_mon.filter_flags = filter_flags;
	net_mon.radio_type = radio_type;
	net_mon.chan_number = channel;

	return wifi_send_netmon_cmd(HostCmd_ACT_GEN_SET, &net_mon);
}

int wifi_sniffer_status()
{
	wifi_net_monitor_t net_mon;

	memset(&net_mon, 0, sizeof(wifi_net_monitor_t));

	return wifi_send_netmon_cmd(HostCmd_ACT_GEN_GET, &net_mon);
}

int wifi_sniffer_stop()
{
	wifi_net_monitor_t net_mon;

	memset(&net_mon, 0, sizeof(wifi_net_monitor_t));

	net_mon.monitor_activity = 0x00;

	return wifi_send_netmon_cmd(HostCmd_ACT_GEN_SET, &net_mon);
}

int wifi_send_rf_channel_cmd(wifi_rf_channel_t *rf_channel)
{
	wifi_get_command_lock();
	HostCmd_DS_COMMAND *cmd = wifi_get_command_buffer();

	cmd->seq_num = 0x0;
	cmd->result = 0x0;

	/*
	  SET operation is not supported according to spec. So we are
	  sending NULL as one param below.
	*/
	mlan_status rv = wlan_ops_sta_prepare_cmd(
						  (mlan_private *) mlan_adap->priv[0],
						  HostCmd_CMD_802_11_RF_CHANNEL,
						  HostCmd_ACT_GEN_GET,
						  0, NULL, NULL, cmd);
	if (rv != MLAN_STATUS_SUCCESS)
		return -WM_FAIL;

	wifi_wait_for_cmdresp(rf_channel);
	return wm_wifi.cmd_resp_status;
}

int wifi_send_remain_on_channel_cmd(unsigned int bss_type,
			wifi_remain_on_channel_t *remain_on_channel)
{
	wifi_get_command_lock();
	HostCmd_DS_COMMAND *cmd = wifi_get_command_buffer();

	cmd->seq_num = HostCmd_SET_SEQ_NO_BSS_INFO(
						   0 /* seq_num */,
						   0 /* bss_num */,
						   bss_type);
	cmd->result = 0x0;

	mlan_status rv = wlan_ops_sta_prepare_cmd(
						  (mlan_private *) mlan_adap->priv[0],
						  HostCmd_CMD_802_11_REMAIN_ON_CHANNEL,
						  HostCmd_ACT_GEN_SET,
						  0, NULL, remain_on_channel, cmd);
	if (rv != MLAN_STATUS_SUCCESS)
		return -WM_FAIL;

	wifi_wait_for_cmdresp(NULL);
	return wm_wifi.cmd_resp_status;
}


/* power_level is not used when cmd_action is GET */
int wifi_get_set_rf_tx_power(t_u16 cmd_action, wifi_tx_power_t *tx_power)
{
	wifi_get_command_lock();
	HostCmd_DS_COMMAND *cmd = wifi_get_command_buffer();

	cmd->seq_num = 0x0;
	cmd->result = 0x0;
	mlan_status rv = wlan_ops_sta_prepare_cmd(
						  (mlan_private *) mlan_adap->priv[0],
						  HostCmd_CMD_802_11_RF_TX_POWER,
						  cmd_action,
						  0, NULL,
						&tx_power->current_level, cmd);
	if (rv != MLAN_STATUS_SUCCESS)
		return -WM_FAIL;

	wifi_wait_for_cmdresp(cmd_action == HostCmd_ACT_GEN_GET ?
					tx_power : NULL);
	return wm_wifi.cmd_resp_status;
}


int wifi_send_get_datarate(wifi_tx_rate_t *tx_rate)
{
	wifi_get_command_lock();
	HostCmd_DS_COMMAND *cmd = wifi_get_command_buffer();

	cmd->seq_num = 0x0;
	cmd->result = 0x0;

	mlan_status rv = wlan_ops_sta_prepare_cmd(
						  (mlan_private *) mlan_adap->priv[0],
						  HostCmd_CMD_802_11_TX_RATE_QUERY,
						  0, 0, NULL, NULL, cmd);
	if (rv != MLAN_STATUS_SUCCESS)
		return -WM_FAIL;

	wifi_wait_for_cmdresp(tx_rate);
	return WM_SUCCESS;
}

/* 
 * fixme: Currently, we support only single SSID based scan. We can extend
 * this to a list of multiple SSIDs. The mlan API supports this.
 */
int wifi_send_scan_cmd(t_u8 bss_mode, const t_u8 *specific_bssid,
			const char *ssid, const char *ssid2,
			const t_u8 num_channels,
			const wifi_scan_channel_list_t *chan_list,
			const t_u8 num_probes,
			const bool keep_previous_scan)
{
	int ssid_len = 0, i;
	int ssid2_len = 0;

	if (ssid) {
		ssid_len = strlen(ssid);
		if (ssid_len > MLAN_MAX_SSID_LENGTH)
			return -WM_E_INVAL;
	}

	if (ssid2) {
		ssid2_len = strlen(ssid2);
		if (ssid2_len > MLAN_MAX_SSID_LENGTH)
			return -WM_E_INVAL;
	}

	wlan_user_scan_cfg *user_scan_cfg =
		os_mem_alloc(sizeof(wlan_user_scan_cfg));
	if (!user_scan_cfg)
		return -WM_E_NOMEM;

	memset(user_scan_cfg, 0x00, sizeof(wlan_user_scan_cfg));

	user_scan_cfg->bss_mode = bss_mode;
	user_scan_cfg->keep_previous_scan = keep_previous_scan;

	if (num_probes > 0 && num_probes <= MAX_PROBES)
		user_scan_cfg->num_probes = num_probes;

	if (specific_bssid) {
		memcpy(user_scan_cfg->specific_bssid, specific_bssid,
		       MLAN_MAC_ADDR_LENGTH);
	}

	if (ssid) {
		memcpy(user_scan_cfg->ssid_list[0].ssid, ssid, ssid_len);
		/* For Wildcard SSID do not set max len */
		/* user_scan_cfg->ssid_list[0].max_len = ssid_len; */
	}

	if (ssid2) {
		memcpy(user_scan_cfg->ssid_list[1].ssid, ssid2, ssid2_len);
	}

	if (num_channels > 0 &&
		num_channels <= WLAN_USER_SCAN_CHAN_MAX && chan_list) {
		for (i = 0; i < num_channels; i++)
		{
			/** Channel Number to scan */
			user_scan_cfg->chan_list[i].chan_number =
				chan_list[i].chan_number;
			/** Radio type: 'B/G' Band = 0, 'A' Band = 1 */
			/* fixme: B/G is hardcoded here. Ask the caller first to
			   send the radio type and then change here */
			if (chan_list[i].chan_number > 14)
				user_scan_cfg->chan_list[i].radio_type = 1;
			/** Scan type: Active = 1, Passive = 2 */
			/* fixme: Active is hardcoded here. Ask the caller first to
			   send the  type and then change here */
			user_scan_cfg->chan_list[i].scan_type = chan_list[i].scan_type;
			/** Scan duration in milliseconds; if 0 default used */
			user_scan_cfg->chan_list[i].scan_time = chan_list[i].scan_time;
		}
	}

	mlan_status rv = wlan_scan_networks(
					    (mlan_private *) mlan_adap->priv[0],
					    NULL, user_scan_cfg);
	if (rv != MLAN_STATUS_SUCCESS) {
		wifi_e("Scan command failed");
		os_mem_free(user_scan_cfg);
		return -WM_FAIL;
	}
	
	/* fixme: Can we free this immediately after wlan_scan_networks
	   call returns */
	os_mem_free(user_scan_cfg);
	return WM_SUCCESS;
}

static int wifi_send_key_material_cmd(int bss_index, mlan_ds_sec_cfg *sec)
{
	/* fixme: check if this needs to go on heap */
	mlan_ioctl_req req;
	mlan_status rv = MLAN_STATUS_SUCCESS;

	memset(&req, 0x00, sizeof(mlan_ioctl_req));
	req.pbuf = (t_u8 *)sec;
	req.buf_len = sizeof(mlan_ds_sec_cfg);
	req.bss_index = bss_index;
	req.req_id = MLAN_IOCTL_SEC_CFG;
	req.action = MLAN_ACT_SET;

	if (bss_index)
		rv = wlan_ops_uap_ioctl(mlan_adap, &req);
	else
		rv = wlan_ops_sta_ioctl(mlan_adap, &req);

	if (rv != MLAN_STATUS_SUCCESS && rv != MLAN_STATUS_PENDING) {
		return -WM_FAIL;
	}

	return WM_SUCCESS;
}

uint8_t broadcast_mac_addr[] ={ 0xff, 0xff, 0xff, 0xff, 0xff, 0xff };

int wifi_set_key(int bss_index, bool is_pairwise, const uint8_t key_index,
		const uint8_t *key, unsigned key_len, const uint8_t *mac_addr)
{
	/* fixme: check if this needs to go on heap */
	mlan_ds_sec_cfg sec;

	memset(&sec, 0x00, sizeof(mlan_ds_sec_cfg));
	sec.sub_command = MLAN_OID_SEC_CFG_ENCRYPT_KEY;

	if (key_len > MAX_WEP_KEY_SIZE) {
		sec.param.encrypt_key.key_flags = KEY_FLAG_TX_SEQ_VALID |
					KEY_FLAG_RX_SEQ_VALID;
		if (is_pairwise) {
#ifndef CONFIG_WiFi_8801
			sec.param.encrypt_key.key_index = MLAN_KEY_INDEX_UNICAST;
#endif
			sec.param.encrypt_key.key_flags |= KEY_FLAG_SET_TX_KEY;
		} else {
#ifndef CONFIG_WiFi_8801
			sec.param.encrypt_key.key_index = 0;
			memcpy(sec.param.encrypt_key.mac_addr, broadcast_mac_addr,
				MLAN_MAC_ADDR_LENGTH);
#endif
			sec.param.encrypt_key.key_flags |= KEY_FLAG_GROUP_KEY;
		}
#ifdef CONFIG_WiFi_8801
		sec.param.encrypt_key.key_index = key_index;
		memcpy(sec.param.encrypt_key.mac_addr, mac_addr,
			MLAN_MAC_ADDR_LENGTH);
#endif
	} else {
		sec.param.encrypt_key.key_index = MLAN_KEY_INDEX_DEFAULT;
		sec.param.encrypt_key.is_current_wep_key = MTRUE;
	}

	sec.param.encrypt_key.key_len = key_len;
	memcpy(sec.param.encrypt_key.key_material, key, key_len);

	return wifi_send_key_material_cmd(bss_index, &sec);
}

int wifi_set_igtk_key(int bss_index, const uint8_t *pn, const uint16_t key_index,
		const uint8_t *key, unsigned key_len)
{
	/* fixme: check if this needs to go on heap */
	mlan_ds_sec_cfg sec;

	memset(&sec, 0x00, sizeof(mlan_ds_sec_cfg));
	sec.sub_command = MLAN_OID_SEC_CFG_ENCRYPT_KEY;

	sec.param.encrypt_key.key_flags = KEY_FLAG_TX_SEQ_VALID |
					KEY_FLAG_RX_SEQ_VALID;

	sec.param.encrypt_key.key_flags = KEY_FLAG_AES_MCAST_IGTK;
	sec.param.encrypt_key.key_index = key_index;

	memcpy(sec.param.encrypt_key.pn, pn, SEQ_MAX_SIZE);
	sec.param.encrypt_key.key_len = key_len;
	memcpy(sec.param.encrypt_key.key_material, key, key_len);

	return wifi_send_key_material_cmd(bss_index, &sec);
}

int wifi_remove_key(int bss_index, bool is_pairwise,
		const uint8_t key_index, const uint8_t *mac_addr)
{
	/* fixme: check if this needs to go on heap */
	mlan_ds_sec_cfg sec;

	memset(&sec, 0x00, sizeof(mlan_ds_sec_cfg));
	sec.sub_command = MLAN_OID_SEC_CFG_ENCRYPT_KEY;

	sec.param.encrypt_key.key_flags = KEY_FLAG_REMOVE_KEY;
	sec.param.encrypt_key.key_remove = MTRUE;

	sec.param.encrypt_key.key_index = key_index;
	memcpy(sec.param.encrypt_key.mac_addr, mac_addr,
			MLAN_MAC_ADDR_LENGTH);

	sec.param.encrypt_key.key_len = WPA_AES_KEY_LEN;

	return wifi_send_key_material_cmd(bss_index, &sec);
}

static int wifi_send_rf_antenna_cmd(t_u16 action, uint16_t *ant_mode)
{
	mlan_private *pmpriv = (mlan_private *) mlan_adap->priv[0];

	if (action != HostCmd_ACT_GEN_GET && action != HostCmd_ACT_GEN_SET)
		return -WM_FAIL;

	wifi_get_command_lock();
	HostCmd_DS_COMMAND *cmd = wifi_get_command_buffer();

	cmd->seq_num = 0x0;
	cmd->result = 0x0;
	

	mlan_status rv =
		wlan_ops_sta_prepare_cmd(pmpriv,
					 HostCmd_CMD_802_11_RF_ANTENNA,
					 action, 0, NULL, ant_mode, cmd);
	if (rv != MLAN_STATUS_SUCCESS)
		return -WM_FAIL;

	return wifi_wait_for_cmdresp(action == HostCmd_ACT_GEN_GET ?
				     ant_mode : NULL);
}

int wifi_get_antenna(uint16_t *ant_mode)
{
	if (!ant_mode)
		return -WM_E_INVAL;

	int rv = wifi_send_rf_antenna_cmd(HostCmd_ACT_GEN_GET, ant_mode);
	if (rv != WM_SUCCESS || wm_wifi.cmd_resp_status != WM_SUCCESS)
		return -WM_FAIL;

	return WM_SUCCESS;
}

int wifi_set_antenna(uint16_t ant_mode)
{
	return wifi_send_rf_antenna_cmd(HostCmd_ACT_GEN_SET, &ant_mode);
}

static int wifi_send_get_log_cmd(wlan_pkt_stats_t *stats)
{
	mlan_private *pmpriv = (mlan_private *) mlan_adap->priv[0];

	wifi_get_command_lock();
	HostCmd_DS_COMMAND *cmd = wifi_get_command_buffer();

	cmd->seq_num = 0x0;
	cmd->result = 0x0;

	mlan_status rv = wlan_ops_sta_prepare_cmd(pmpriv,
				HostCmd_CMD_802_11_GET_LOG,
				HostCmd_ACT_GEN_GET, 0, NULL, NULL, cmd);
	if (rv != MLAN_STATUS_SUCCESS)
		return -WM_FAIL;

	return wifi_wait_for_cmdresp(stats);
}

int wifi_get_log(wlan_pkt_stats_t *stats)
{
	int rv = wifi_send_get_log_cmd(stats);
	if (rv != WM_SUCCESS || wm_wifi.cmd_resp_status != WM_SUCCESS)
			return -WM_FAIL;

	return WM_SUCCESS;
}

static int wifi_send_cmd_802_11_supplicant_pmk(int mode,
					       mlan_ds_sec_cfg *sec,
					       t_u32 action)
{
	/* fixme: check if this needs to go on heap */
	mlan_ioctl_req req;

	memset(&req, 0x00, sizeof(mlan_ioctl_req));
	req.pbuf = (t_u8 *)sec;
	req.buf_len = sizeof(mlan_ds_sec_cfg);
	req.bss_index = 0;
	req.req_id = MLAN_IOCTL_SEC_CFG;
	req.action = action;

	mlan_status rv = wlan_ops_sta_ioctl(mlan_adap, &req);
	if (rv != MLAN_STATUS_SUCCESS && rv != MLAN_STATUS_PENDING) {
		return -WM_FAIL;
	}

	return WM_SUCCESS;
}

int wifi_send_add_wpa_pmk(int mode, char *ssid, char *bssid, char *pmk, unsigned int len)
{
	if (!ssid || (len != MLAN_MAX_KEY_LENGTH))
		return -WM_E_INVAL;

	mlan_ds_sec_cfg sec;

	memset(&sec, 0x00, sizeof(mlan_ds_sec_cfg));
	sec.sub_command = MLAN_OID_SEC_CFG_PASSPHRASE;

	/* SSID */
	int ssid_len = strlen(ssid);
	if (ssid_len > MLAN_MAX_SSID_LENGTH)
		return -WM_E_INVAL;

	mlan_ds_passphrase *pp = &sec.param.passphrase;
	pp->ssid.ssid_len = ssid_len;
	memcpy(pp->ssid.ssid, ssid, ssid_len);

	/* MAC */
	if (bssid)
		memcpy(pp->bssid, bssid, MLAN_MAC_ADDR_LENGTH);

	/* PMK */
	pp->psk_type = MLAN_PSK_PMK;
	memcpy(pp->psk.pmk.pmk, pmk, len);

	return wifi_send_cmd_802_11_supplicant_pmk(mode, &sec, MLAN_ACT_SET);
}

/* fixme: This function has not been tested because of known issue in
   calling function. The calling function has been disabled for that */
int wifi_send_get_wpa_pmk(int mode, char *ssid)
{
	if (!ssid)
		return -WM_E_INVAL;

	mlan_ds_sec_cfg sec;

	memset(&sec, 0x00, sizeof(mlan_ds_sec_cfg));
	sec.sub_command = MLAN_OID_SEC_CFG_PASSPHRASE;

	/* SSID */
	int ssid_len = strlen(ssid);
	if (ssid_len > MLAN_MAX_SSID_LENGTH)
		return -WM_E_INVAL;

	mlan_ds_passphrase *pp = &sec.param.passphrase;
	pp->ssid.ssid_len = ssid_len;
	memcpy(pp->ssid.ssid, ssid, ssid_len);

	/* Zero MAC */

	pp->psk_type = MLAN_PSK_QUERY;

	return wifi_send_cmd_802_11_supplicant_pmk(mode, &sec, MLAN_ACT_GET);
}

/*
Note:
Passphrase can be between 8 to 63 if it is ASCII and 64 if its PSK
hexstring
*/
int wifi_send_add_wpa_psk(int mode, char *ssid,
			  char *passphrase, unsigned int len)
{
	if (!ssid || (len > 64))
		return -WM_E_INVAL;

	mlan_ds_sec_cfg sec;

	memset(&sec, 0x00, sizeof(mlan_ds_sec_cfg));
	sec.sub_command = MLAN_OID_SEC_CFG_PASSPHRASE;

	/* SSID */
	int ssid_len = strlen(ssid);
	if (ssid_len > MLAN_MAX_SSID_LENGTH)
		return -WM_E_INVAL;

	mlan_ds_passphrase *pp = &sec.param.passphrase;
	pp->ssid.ssid_len = ssid_len;
	memcpy(pp->ssid.ssid, ssid, ssid_len);

	/* Zero MAC */

	/* Passphrase */
	pp->psk_type = MLAN_PSK_PASSPHRASE;
	pp->psk.passphrase.passphrase_len = len;
	memcpy(pp->psk.passphrase.passphrase, passphrase, len);

	return wifi_send_cmd_802_11_supplicant_pmk(mode, &sec, MLAN_ACT_SET);
}

int wifi_send_clear_wpa_psk(int mode, const char *ssid)
{
	if (!ssid)
		return -WM_E_INVAL;

	mlan_ds_sec_cfg sec;

	memset(&sec, 0x00, sizeof(mlan_ds_sec_cfg));
	sec.sub_command = MLAN_OID_SEC_CFG_PASSPHRASE;

	/* SSID */
	int ssid_len = strlen(ssid);
	if (ssid_len > MLAN_MAX_SSID_LENGTH)
		return -WM_E_INVAL;

	sec.param.passphrase.ssid.ssid_len = ssid_len;
	strcpy((char *)sec.param.passphrase.ssid.ssid, ssid);

	/* Zero MAC */

	sec.param.passphrase.psk_type = MLAN_PSK_CLEAR;
	return wifi_send_cmd_802_11_supplicant_pmk(mode, &sec, MLAN_ACT_SET);
}

int wifi_send_enable_supplicant(int mode, const char *ssid)
{
	if (!ssid)
		return -WM_E_INVAL;

	mlan_ds_sec_cfg sec;

	memset(&sec, 0x00, sizeof(mlan_ds_sec_cfg));
	sec.sub_command = MLAN_OID_SEC_CFG_PASSPHRASE;

	/* SSID */
	int ssid_len = strlen(ssid);
	if (ssid_len > MLAN_MAX_SSID_LENGTH)
		return -WM_E_INVAL;

	sec.param.passphrase.ssid.ssid_len = ssid_len;
	strcpy((char *)sec.param.passphrase.ssid.ssid, ssid);

	/* Zero MAC */

	sec.param.passphrase.psk_type = MLAN_PSK_PASSPHRASE;
	return wifi_send_cmd_802_11_supplicant_pmk(mode, &sec, MLAN_ACT_SET);
}

int wifi_send_disable_supplicant(int mode)
{
	mlan_ds_sec_cfg sec;

	memset(&sec, 0x00, sizeof(mlan_ds_sec_cfg));
	sec.sub_command = MLAN_OID_SEC_CFG_PASSPHRASE;

	sec.param.passphrase.psk_type = MLAN_PSK_CLEAR;

	return wifi_send_cmd_802_11_supplicant_pmk(mode, &sec, MLAN_ACT_SET);
}

int wifi_set_mac_multicast_addr(const char *mlist, t_u32 num_of_addr)
{
	if (!mlist)
		return -WM_E_INVAL;
	if (num_of_addr > MLAN_MAX_MULTICAST_LIST_SIZE)
		return -WM_E_INVAL;

	mlan_multicast_list *pmcast_list;
	pmcast_list = os_mem_alloc(sizeof(mlan_multicast_list));
	if (!pmcast_list)
		return -WM_E_NOMEM;

	memcpy(pmcast_list->mac_list, mlist,
	       num_of_addr * MLAN_MAC_ADDR_LENGTH);
	pmcast_list->num_multicast_addr = num_of_addr;
	wifi_get_command_lock();
	HostCmd_DS_COMMAND *cmd = wifi_get_command_buffer();

	mlan_status rv = wlan_ops_sta_prepare_cmd(
						  (mlan_private *)
						  mlan_adap->priv[0],
						  HostCmd_CMD_MAC_MULTICAST_ADR,
						  HostCmd_ACT_GEN_SET,
						  0, NULL, pmcast_list,  cmd);

	if (rv != MLAN_STATUS_SUCCESS) {
		os_mem_free(pmcast_list);
		return -WM_FAIL;
	}
	wifi_wait_for_cmdresp(NULL);
	os_mem_free(pmcast_list);
	return WM_SUCCESS;
}

int wifi_get_otp_user_data(uint8_t *buf, uint16_t len)
{
	wifi_get_command_lock();
	HostCmd_DS_COMMAND *cmd = wifi_get_command_buffer();
	mlan_ds_misc_otp_user_data pdata;

	if (!buf)
		return -WM_E_INVAL;

	cmd->seq_num = 0x0;
	cmd->result = 0x0;
	pdata.user_data_length = len > MAX_OTP_USER_DATA_LEN
					? MAX_OTP_USER_DATA_LEN : len;

	mlan_status rv = wlan_ops_sta_prepare_cmd(
					(mlan_private *) mlan_adap->priv[0],
					HostCmd_CMD_OTP_READ_USER_DATA,
					HostCmd_ACT_GEN_GET, 0, NULL, &pdata,
					cmd);
	if (rv != MLAN_STATUS_SUCCESS)
		return -WM_FAIL;

	wifi_wait_for_cmdresp(buf);
	return wm_wifi.cmd_resp_status;
}

int wifi_get_cal_data(wifi_cal_data_t *cal_data)
{
	uint32_t size = S_DS_GEN + sizeof(HostCmd_DS_802_11_CFG_DATA) - 1;

	wifi_get_command_lock();
	HostCmd_DS_COMMAND *cmd = wifi_get_command_buffer();

	cmd->command = wlan_cpu_to_le16(HostCmd_CMD_CFG_DATA);

	HostCmd_DS_802_11_CFG_DATA *cfg_data_cmd =
	    (HostCmd_DS_802_11_CFG_DATA *) ((uint32_t) cmd + S_DS_GEN);

	cfg_data_cmd->action = HostCmd_ACT_GEN_GET;
	cfg_data_cmd->type = 0x02;
	cfg_data_cmd->data_len =0x00;

	cmd->size = size;
	cmd->seq_num = 0x00;
	cmd->result = 0x00;

	wifi_wait_for_cmdresp(cal_data);

	return wm_wifi.cmd_resp_status;
}

int wifi_get_firmware_version_ext(wifi_fw_version_ext_t *version_ext)
{
	if (!version_ext)
		return -WM_E_INVAL;

	mlan_private *pmpriv = (mlan_private *) mlan_adap->priv[0];
	mlan_status rv =
		wifi_prepare_and_send_cmd(pmpriv,
					  HostCmd_CMD_VERSION_EXT,
					  HostCmd_ACT_GEN_GET, 0, NULL,
					  &version_ext->version_str_sel,
					  MLAN_BSS_TYPE_STA,
					  version_ext);
	return (rv == MLAN_STATUS_SUCCESS ? WM_SUCCESS : -WM_FAIL);
}

int wifi_get_firmware_version(wifi_fw_version_t *ver)
{
	if (!ver)
		return -WM_E_INVAL;

	union {
		uint32_t l;
		uint8_t c[4];
	} u_ver;
	char fw_ver[32];

	u_ver.l = mlan_adap->fw_release_number;
	sprintf(fw_ver, "%u.%u.%u.p%u", u_ver.c[2], u_ver.c[1],
		u_ver.c[0], u_ver.c[3]);

	snprintf(ver->version_str, MLAN_MAX_VER_STR_LEN,
		 driver_version_format, fw_ver, driver_version);

	return WM_SUCCESS;
}

/* Region: US(US) or Canada(CA) or Singapore(SG) 2.4 GHz */
wifi_sub_band_set_t subband_US_CA_SG_2_4_GHz[] = {
	{1, 11, 20}
};

/* Region: Europe(EU), Australia(AU), Republic of Korea(KR),
China(CN) 2.4 GHz */
wifi_sub_band_set_t subband_EU_AU_KR_CN_2_4GHz[] = {
	{1, 13, 20}
};

/* Region: France(FR) 2.4 GHz */
wifi_sub_band_set_t subband_FR_2_4GHz[] = {
	{1, 9, 20},
	{10, 4, 10}
};

/* Region: Japan(JP) 2.4 GHz */
wifi_sub_band_set_t subband_JP_2_4GHz[] = {
	{1, 14, 20},
};

/* Region: Constrained 2.4 Ghz */
wifi_sub_band_set_t subband_CS_2_4GHz[] = {
	{1, 9, 20},
	{10, 2, 10}
};

#ifdef CONFIG_5GHz_SUPPORT

/* Region: US(US) or France(FR) or Singapore(SG) 5 GHz */
wifi_sub_band_set_t subband_US_SG_FR_5_GHz[] = {
	{36, 8, 20},
	{100, 11, 20},
	{149, 5, 20}
};

/* Region: Canada(CA) 5 GHz */
wifi_sub_band_set_t subband_CA_5_GHz[] = {
	{36, 8, 20},
	{100, 5, 20},
	{132, 3, 20},
	{149, 5, 20}
};

/* Region: Region: Europe(EU), Australia(AU), Republic of Korea(KR)
 * 5 GHz */
wifi_sub_band_set_t subband_EU_AU_KR_5_GHz[] = {
	{36, 8, 20},
        {100, 11, 20},
};

/* Region: Japan(JP) 5 GHz */
wifi_sub_band_set_t subband_JP_5_GHz[] = {
	{8, 3, 23},
	{36, 8, 23},
	{100, 11, 23},
};

/* Region: China(CN) 5 Ghz */
wifi_sub_band_set_t subband_CN_5_GHz[] = {
	{149, 5, 33},
};

#endif /* CONFIG_5GHz_SUPPORT */

int wifi_get_region_code(t_u32 *region_code)
{
	mlan_ds_misc_cfg misc;

	mlan_ioctl_req req = {
		.bss_index = 0,
		.pbuf = (t_u8 *)&misc,
		.action = MLAN_ACT_GET,
	};

	mlan_status mrv = wlan_misc_ioctl_region(mlan_adap, &req);
	if (mrv != MLAN_STATUS_SUCCESS) {
		wifi_w("Unable to get region code");
		return -WM_FAIL;
	}

	*region_code = misc.param.region_code;
	return WM_SUCCESS;
}

int wifi_set_region_code(t_u32 region_code)
{
	mlan_ds_misc_cfg misc = {
		.param.region_code = region_code,
	};

	mlan_ioctl_req req = {
		.bss_index = 0,
		.pbuf = (t_u8 *)&misc,
		.action = MLAN_ACT_SET,
	};

	mlan_status mrv = wlan_misc_ioctl_region(mlan_adap, &req);
	if (mrv != MLAN_STATUS_SUCCESS) {
		wifi_w("Unable to set region code");
		return -WM_FAIL;
	}

	return WM_SUCCESS;
}

static int wifi_send_11d_cfg_ioctl(mlan_ds_11d_cfg *d_cfg)
{
	/* fixme: check if this needs to go on heap */
	mlan_ioctl_req req;

	memset(&req, 0x00, sizeof(mlan_ioctl_req));
	req.pbuf = (t_u8 *)d_cfg;
	req.buf_len = sizeof(mlan_ds_11d_cfg);
	req.bss_index = 0;
	req.req_id = MLAN_IOCTL_11D_CFG;
	req.action = HostCmd_ACT_GEN_SET;

	mlan_status rv = wlan_ops_sta_ioctl(mlan_adap, &req);
	if (rv != MLAN_STATUS_SUCCESS && rv != MLAN_STATUS_PENDING) {
		return -WM_FAIL;
	}

	return WM_SUCCESS;
}
int wifi_set_domain_params(wifi_domain_param_t *dp)
{
	if (!dp)
		return -WM_E_INVAL;

	mlan_ds_11d_cfg d_cfg;

	memset(&d_cfg, 0x00, sizeof(mlan_ds_11d_cfg));

	d_cfg.sub_command = MLAN_OID_11D_DOMAIN_INFO;

	memcpy(&d_cfg.param.domain_info.country_code, dp->country_code,
			COUNTRY_CODE_LEN);

	d_cfg.param.domain_info.band = BAND_B | BAND_G;

#ifdef CONFIG_11N
	d_cfg.param.domain_info.band |= BAND_GN;
#ifdef CONFIG_5GHz_SUPPORT
	d_cfg.param.domain_info.band |= BAND_AN;
#endif
#else
#ifdef CONFIG_5GHz_SUPPORT
	d_cfg.param.domain_info.band |= BAND_A;
#endif
#endif
	d_cfg.param.domain_info.no_of_sub_band = dp->no_of_sub_band;

	wifi_sub_band_set_t *is = dp->sub_band;
	mlan_ds_subband_set_t *s = d_cfg.param.domain_info.sub_band;
	int i;
	for (i = 0; i < dp->no_of_sub_band; i++) {
		s[i].first_chan = is[i].first_chan;
		s[i].no_of_chan = is[i].no_of_chan;
		s[i].max_tx_pwr = is[i].max_tx_pwr;
	}

	wifi_enable_11d_support_APIs();

	return wifi_send_11d_cfg_ioctl(&d_cfg);
}

int wifi_enable_11d_support()
{
	mlan_private *pmpriv = (mlan_private *) mlan_adap->priv[0];

	wrapper_wlan_11d_enable();

	return wlan_enable_11d_support(pmpriv);
}

int wifi_enable_11d_support_APIs()
{
	mlan_private *pmpriv = (mlan_private *) mlan_adap->priv[0];

	return wlan_11d_support_APIs(pmpriv);
}

wifi_sub_band_set_t *get_sub_band_from_country(int country, int *nr_sb)
{
	*nr_sb = 1;

	switch(country){
		case 1:
		case 2:
		case 3:
			return subband_US_CA_SG_2_4_GHz;
		case 4:
		case 5:
		case 6:
		case 9:
			return subband_EU_AU_KR_CN_2_4GHz;
		case 7:
			*nr_sb = 2;
			return subband_FR_2_4GHz;
		case 8:
			return subband_JP_2_4GHz;
		default:
			*nr_sb = 2;
			return subband_CS_2_4GHz;
	}

	*nr_sb = 2;
	return subband_CS_2_4GHz;
}

static wifi_sub_band_set_t *get_sub_band_from_region_code(int region_code, int *nr_sb)
{
	*nr_sb = 1;

	switch(region_code){
		case 0x10:
			return subband_US_CA_SG_2_4_GHz;
		case 0x30:
			return subband_EU_AU_KR_CN_2_4GHz;
		case 0x32:
			*nr_sb = 2;
			return subband_FR_2_4GHz;
		case 0xFF:
			return subband_JP_2_4GHz;
		default:
			*nr_sb = 2;
			return subband_CS_2_4GHz;
	}

	*nr_sb = 2;
	return subband_CS_2_4GHz;
}

#ifdef CONFIG_5GHz_SUPPORT
static wifi_sub_band_set_t * get_sub_band_from_country_5ghz(int country, int *nr_sb)
{
	*nr_sb = 1;

	switch(country){
		case 1:
		case 3:
		case 7:
			*nr_sb = 3;
			return subband_US_SG_FR_5_GHz;
		case 2:
			*nr_sb = 4;
			return subband_CA_5_GHz;
		case 4:
		case 5:
		case 6:
			*nr_sb = 2;
			return subband_EU_AU_KR_5_GHz;
		case 8:
			*nr_sb = 3;
                        return subband_JP_5_GHz;
		case 9:
			return subband_CN_5_GHz;
		default:
			*nr_sb = 3;
			return subband_US_SG_FR_5_GHz;
	}
}

static wifi_sub_band_set_t * get_sub_band_from_region_code_5ghz(int region_code, int *nr_sb)
{
	*nr_sb = 1;

	switch(region_code){
		case 0x10:
		case 0x32:
			*nr_sb = 3;
			return subband_US_SG_FR_5_GHz;
		case 0x20:
			*nr_sb = 4;
			return subband_CA_5_GHz;
		case 0x30:
			*nr_sb = 2;
			return subband_EU_AU_KR_5_GHz;
		case 0x40:
			*nr_sb = 3;
			return subband_JP_5_GHz;
		case 0x50:
			return subband_CN_5_GHz;
		default:
			*nr_sb = 3;
			return subband_US_SG_FR_5_GHz;
	}
}
#endif /* CONFIG_5GHz_SUPPORT */

bool wifi_11d_is_channel_allowed(int channel)
{
	int i, j, k, nr_sb = 0;

	mlan_private *pmpriv = (mlan_private *) mlan_adap->priv[0];

	wifi_sub_band_set_t *sub_band = NULL;

	if (channel > 14) {

#ifdef CONFIG_5GHz_SUPPORT
		if (wifi_11d_country == 0x00)
			sub_band = get_sub_band_from_region_code_5ghz(pmpriv->adapter->region_code,
						&nr_sb);
		else
			sub_band = get_sub_band_from_country_5ghz(wifi_11d_country,
						&nr_sb);
#else
		wifi_w("5 GHz support is not enabled");

#endif /* CONFIG_5GHz_SUPPORT */
	} else {
		if (wifi_11d_country == 0x00)
			sub_band = get_sub_band_from_region_code(pmpriv->adapter->region_code,
						&nr_sb);
		else
			sub_band = get_sub_band_from_country(wifi_11d_country,
						&nr_sb);
	}

	for (i = 0; i < nr_sb; i++)
	{
		j = sub_band[i].first_chan;

		for (k = 0; k < sub_band[i].no_of_chan; k++)
		{
			if (j == channel)
				return true;

			if (channel > 14)
				j += 4;
			else
				j++;
		}
	}

	return false;
}

char * wifi_get_country_str(int country)
{
	switch (country) {
		case 1:
			return "US ";
		case 2:
			return "CA ";
		case 3:
			return "SG ";
		case 4:
			return "EU ";
		case 5:
			return "AU ";
		case 6:
			return "KR ";
		case 7:
			return "FR ";
		case 8:
			return "JP ";
		case 9:
			return "CN ";
		default:
			return "UN ";
	}
	return "UN ";
}

static wifi_domain_param_t *get_11d_domain_params(int country, wifi_sub_band_set_t *sub_band, int nr_sb)
{

	wifi_domain_param_t *dp =
		os_mem_alloc(sizeof(wifi_domain_param_t) +
				(sizeof(wifi_sub_band_set_t) * (nr_sb - 1)));

	memcpy(dp->country_code, wifi_get_country_str(country), COUNTRY_CODE_LEN);

	dp->no_of_sub_band = nr_sb;
	memcpy(&dp->sub_band[0], sub_band,
			nr_sb * sizeof(wifi_sub_band_set_t));

	return dp;
}


int wifi_get_country()
{
	return wifi_11d_country;
}

int wifi_set_country(int country)
{
	int ret, nr_sb;

	wifi_11d_country = country;

	wifi_sub_band_set_t *sub_band = get_sub_band_from_country(country, &nr_sb);

	wifi_domain_param_t *dp = get_11d_domain_params(country, sub_band, nr_sb);

	ret = wifi_uap_set_domain_params(dp);

	if (ret != WM_SUCCESS) {
		wifi_11d_country = 0x00;
		os_mem_free(dp);
		return ret;
	}

	ret = wifi_set_domain_params(dp);

	if (ret != WM_SUCCESS) {
		wifi_11d_country = 0x00;
		os_mem_free(dp);
		return ret;
	}

	os_mem_free(dp);
	return WM_SUCCESS;
}

static int get_free_mgmt_ie_index()
{
	if (!(mgmt_ie_index_bitmap & MBIT(0)))
		return 0;
	else if (!(mgmt_ie_index_bitmap & MBIT(1)))
		return 1;
	else if (!(mgmt_ie_index_bitmap & MBIT(2)))
		return 2;
	else if (!(mgmt_ie_index_bitmap & MBIT(3)))
		return 3;
	return -1;
}

static void set_ie_index(int index)
{
	mgmt_ie_index_bitmap |= (MBIT(index));
}

static void clear_ie_index(int index)
{
	mgmt_ie_index_bitmap &= ~(MBIT(index));
}

int wifi_config_mgmt_ie(unsigned int bss_type, int action, unsigned int index,
				void *buffer, unsigned int *ie_len)
{
	uint8_t *buf, *pos;
	IEEEtypes_Header_t *ptlv_header = NULL;
	uint16_t buf_len = 0;
	tlvbuf_custom_ie *tlv = NULL;
	custom_ie *ie_ptr = NULL;
	int mgmt_ie_index = -1;
	int total_len = sizeof(tlvbuf_custom_ie) +
			2 * (sizeof(custom_ie) - MAX_IE_SIZE)
			+ sizeof(IEEEtypes_Header_t) + *ie_len;

	buf = (uint8_t *) os_mem_alloc(total_len);
	if (!buf) {
		wifi_e("Cannot allocate memory");
		return -WM_FAIL;
	}

	memset(buf, 0, total_len);

	tlv = (tlvbuf_custom_ie *) buf;
	tlv->type = MRVL_MGMT_IE_LIST_TLV_ID;

	/* Locate headers */
	ie_ptr = (custom_ie *) (tlv->ie_data);
	/* Set TLV fields */
	buf_len = sizeof(tlvbuf_custom_ie);

	if (action == HostCmd_ACT_GEN_SET) {
		if (*ie_len == 0) {
			if (index < 0)
				return -WM_FAIL;

			/* Clear WPS IE */
			ie_ptr->mgmt_subtype_mask = MGMT_MASK_CLEAR;
			ie_ptr->ie_length = 0;
			ie_ptr->ie_index = index;

			ie_ptr =
			    (custom_ie *) (((uint8_t *) ie_ptr)
					+ sizeof(custom_ie) - MAX_IE_SIZE);
			ie_ptr->mgmt_subtype_mask = MGMT_MASK_CLEAR;
			ie_ptr->ie_length = 0;
			ie_ptr->ie_index = index + 1;
			tlv->length = 2 * (sizeof(custom_ie) - MAX_IE_SIZE);
			buf_len += tlv->length;
			clear_ie_index(index);
		} else {
			/* Set WPS IE */
			mgmt_ie_index = get_free_mgmt_ie_index();

			if (mgmt_ie_index < 0)
				return -WM_FAIL;

			pos = ie_ptr->ie_buffer;
			ptlv_header = (IEEEtypes_Header_t *) pos;
			pos += sizeof(IEEEtypes_Header_t);

			ptlv_header->element_id = index;
			ptlv_header->len = *ie_len;
			if (bss_type == MLAN_BSS_TYPE_UAP)
				ie_ptr->mgmt_subtype_mask = MGMT_MASK_BEACON |
							MGMT_MASK_PROBE_RESP |
							MGMT_MASK_ASSOC_RESP |
							MGMT_MASK_REASSOC_RESP;
			else if (bss_type == MLAN_BSS_TYPE_STA)
				ie_ptr->mgmt_subtype_mask =
							MGMT_MASK_PROBE_REQ |
							MGMT_MASK_ASSOC_REQ |
							MGMT_MASK_REASSOC_REQ;

			tlv->length =
				sizeof(custom_ie) + sizeof(IEEEtypes_Header_t)
					+ *ie_len - MAX_IE_SIZE;
			ie_ptr->ie_length = sizeof(IEEEtypes_Header_t) + *ie_len;
			ie_ptr->ie_index = mgmt_ie_index;

			buf_len += tlv->length;

			memcpy(pos, buffer, *ie_len);
		}
	}
	else if (action == HostCmd_ACT_GEN_GET) {
		/* Get WPS IE */
		tlv->length = 0;
	}

	mlan_status rv = wrapper_wlan_cmd_mgmt_ie(bss_type, buf,
						buf_len, action);

        if (rv != MLAN_STATUS_SUCCESS && rv != MLAN_STATUS_PENDING) {
		os_mem_free(buf);
                return -WM_FAIL;
	}

	if (action == HostCmd_ACT_GEN_GET) {
		if (wm_wifi.cmd_resp_status) {
			wifi_w("Unable to get mgmt ie buffer");
			os_mem_free(buf);
			return wm_wifi.cmd_resp_status;
		}
		ie_ptr = (custom_ie *) (buf);
		memcpy(buffer, ie_ptr->ie_buffer, ie_ptr->ie_length);
		*ie_len = ie_ptr->ie_length;
	}

	if (buf)
		os_mem_free(buf);

	if ((action == HostCmd_ACT_GEN_SET) && *ie_len)
	{
		set_ie_index(mgmt_ie_index);
		return mgmt_ie_index;
	}
	else
		return WM_SUCCESS;
}

int wifi_get_mgmt_ie(unsigned int bss_type, unsigned int index,
			void *buf, unsigned int *buf_len)
{
	return wifi_config_mgmt_ie(bss_type, HostCmd_ACT_GEN_GET,
					index, buf, buf_len);
}

int wifi_set_mgmt_ie(unsigned int bss_type, unsigned int id,
			void *buf, unsigned int buf_len)
{
	unsigned int data_len = buf_len;

	return wifi_config_mgmt_ie(bss_type, HostCmd_ACT_GEN_SET,
					id, buf, &data_len);
}

int wifi_clear_mgmt_ie(unsigned int bss_type, unsigned int index)
{
	unsigned int data_len = 0;
	return wifi_config_mgmt_ie(bss_type, HostCmd_ACT_GEN_SET,
					index, NULL, &data_len);
}

int wifi_country_trpc_cfg_data(struct pwr_table *tp, uint8_t country, chan_trpc_t *chan_trpc)
{
	int i , j;

	if (tp) {

		chan_trpc->country = country;

		chan_trpc->num_chans = tp->num_of_channels;

		for (i = 0; i < chan_trpc->num_chans; i++) {
			chan_trpc->chan_trpc_config[i].num_mod_grps = MOD_GROUPS;
			chan_trpc->chan_trpc_config[i].chan_desc.start_freq = 2407;
			chan_trpc->chan_trpc_config[i].chan_desc.chan_width = 20;
			chan_trpc->chan_trpc_config[i].chan_desc.chan_num =
				*((uint8_t *)tp->table + i * (MOD_GROUPS + 1) + 0);

			for (j = 0; j < MOD_GROUPS; j++) {
				chan_trpc->chan_trpc_config[i].chan_trpc_entry[j].mod_group = j;
				chan_trpc->chan_trpc_config[i].chan_trpc_entry[j].tx_power =
				*((uint8_t *)tp->table + i * (MOD_GROUPS + 1) + j + 1);
			}
		}

		return WM_SUCCESS;
	}
	return -WM_FAIL;
}

int wifi_set_trpc_params(chan_trpc_t *chan_trpc)
{
	int i, ret;

	HostCmd_DS_COMMAND *cmd = wifi_get_command_buffer();

	wifi_get_command_lock();

	cmd->command = HostCmd_CMD_CHANNEL_TRPC_CONFIG;
	cmd->seq_num = 0x0;
	cmd->result = 0x0;
	cmd->size = S_DS_GEN + 2 * sizeof(t_u16) +
		  chan_trpc->num_chans * (sizeof(MrvlIEtypesHeader_t)
		  + sizeof(MrvlChannelDesc_t))
		  + (chan_trpc->num_chans *
		  chan_trpc->chan_trpc_config->num_mod_grps *
		  sizeof(MrvlChanTrpcEntry_t));

	HostCmd_DS_CHAN_TRPC_CONFIG *chan_trpc_config =
		(HostCmd_DS_CHAN_TRPC_CONFIG *) ((uint8_t *)cmd + S_DS_GEN);

	trpc_country = chan_trpc->country;
	chan_trpc_config->action = HostCmd_ACT_GEN_SET;
	chan_trpc_config->reserved = 0x00;

	for (i = 0; i < chan_trpc->num_chans ; i++) {
		chan_trpc_config->ChanTrpcG.ChanTrpc[i].header.type =
				TLV_TYPE_CHANNEL_TRPC_CONFIG;
		chan_trpc_config->ChanTrpcG.ChanTrpc[i].header.len =
			sizeof(MrvlChannelDesc_t) +
			chan_trpc->chan_trpc_config->num_mod_grps *
			sizeof(MrvlChanTrpcEntry_t);
		chan_trpc_config->ChanTrpcG.ChanTrpc[i].chanDesc.startFreq =
			chan_trpc->chan_trpc_config[i].chan_desc.start_freq;
		chan_trpc_config->ChanTrpcG.ChanTrpc[i].chanDesc.chanWidth =
			chan_trpc->chan_trpc_config[i].chan_desc.chan_width;
		chan_trpc_config->ChanTrpcG.ChanTrpc[i].chanDesc.chanNum =
			chan_trpc->chan_trpc_config[i].chan_desc.chan_num;
		memcpy(chan_trpc_config->ChanTrpcG.ChanTrpc[i].chanTrpcEntry,
			chan_trpc->chan_trpc_config[i].chan_trpc_entry,
			chan_trpc->chan_trpc_config->num_mod_grps *
			sizeof(MrvlChanTrpcEntry_t));

	}
	ret = wifi_wait_for_cmdresp(NULL);
	return ret;
}

int wifi_get_trpc_params(chan_trpc_t *chan_trpc)
{
	int ret;

	HostCmd_DS_COMMAND *cmd = wifi_get_command_buffer();

	wifi_get_command_lock();

	cmd->command = HostCmd_CMD_CHANNEL_TRPC_CONFIG;
	cmd->seq_num = 0x0;
	cmd->result = 0x0;
	cmd->size = S_DS_GEN + 2 * sizeof(t_u16);

	HostCmd_DS_CHAN_TRPC_CONFIG *chan_trpc_config =
		(HostCmd_DS_CHAN_TRPC_CONFIG *) ((uint8_t *)cmd + S_DS_GEN);

	chan_trpc_config->action = HostCmd_ACT_GEN_GET;
	chan_trpc_config->reserved = 0x00;

	ret = wifi_wait_for_cmdresp(chan_trpc);
	chan_trpc->country = trpc_country;
	return ret;
}

#ifndef IEEEtypes_SSID_SIZE
#define IEEEtypes_SSID_SIZE            32
#endif /* IEEEtypes_SSID_SIZE */
#define MRVL_SSID_TLV_ID                0x0000
#define PROPRIETARY_TLV_BASE_ID         0x0100
#define MRVL_BEACON_PERIOD_TLV_ID       (PROPRIETARY_TLV_BASE_ID + 0x2c)
#define TLV_TYPE_CHANLIST           	(PROPRIETARY_TLV_BASE_ID + 0x01)
#define TLV_TYPE_PASSTHROUGH        	(PROPRIETARY_TLV_BASE_ID + 0x0a)
#define TLV_TYPE_SMCADDRRANGE        	(PROPRIETARY_TLV_BASE_ID + 0xcc)
#define TLV_TYPE_SMCFRAMEFILTER        	(PROPRIETARY_TLV_BASE_ID + 0xd1)

int wifi_set_smart_mode_cfg(char *ssid, int beacon_period,
			    wifi_chan_list_param_set_t *chan_list,
			    uint8_t *smc_start_addr, uint8_t *smc_end_addr,
			    uint16_t filter_type, int smc_frame_filter_len,
			    uint8_t *smc_frame_filter, int custom_ie_len,
			    uint8_t *custom_ie)
{
	unsigned int ssid_len = 0, i;
	uint32_t size = S_DS_GEN + sizeof(HostCmd_DS_SYS_CONFIG) - 1;
	MrvlIEtypes_SsIdParamSet_t *tlv_ssid = NULL;
	MrvlIEtypes_beacon_period_t *tlv_beacon_period = NULL;
	MrvlIEtypes_ChanListParamSet_t *tlv_chan_list = NULL;
	MrvlIEtypes_Data_t *tlv_custom_ie = NULL;
	MrvlIETypes_SmcAddrRange_t *tlv_smc_addr_range = NULL;
	MrvlIETypes_SmcFrameFilter_t *tlv_smc_frame_filter = NULL;

	wifi_get_command_lock();
	HostCmd_DS_COMMAND *cmd = wifi_get_command_buffer();

	cmd->command = wlan_cpu_to_le16(HOST_CMD_SMART_MODE_CFG);
	HostCmd_DS_SYS_CONFIG *sys_config_cmd =
	    (HostCmd_DS_SYS_CONFIG *) ((uint32_t) cmd + S_DS_GEN);
	sys_config_cmd->action = HostCmd_ACT_GEN_SET;
	uint8_t *tlv = (uint8_t *) sys_config_cmd->tlv_buffer;

	ssid_len = strlen(ssid);
	if (ssid_len > IEEEtypes_SSID_SIZE || custom_ie_len > 255)
		return -WM_E_INVAL;;

	tlv_ssid = (MrvlIEtypes_SsIdParamSet_t *) sys_config_cmd->tlv_buffer;
	tlv_ssid->header.type = MRVL_SSID_TLV_ID;
	tlv_ssid->header.len = strlen(ssid);
	memcpy(tlv_ssid->ssid, ssid, strlen(ssid));
	size += sizeof(tlv_ssid->header) + tlv_ssid->header.len;
	tlv += sizeof(tlv_ssid->header) + tlv_ssid->header.len;
	tlv_beacon_period = (MrvlIEtypes_beacon_period_t *) tlv;
	tlv_beacon_period->header.type = MRVL_BEACON_PERIOD_TLV_ID;
	tlv_beacon_period->header.len = sizeof(uint16_t);
	tlv_beacon_period->beacon_period = beacon_period;

	size += sizeof(tlv_beacon_period->header) +
		tlv_beacon_period->header.len;
	tlv += sizeof(tlv_beacon_period->header) +
		tlv_beacon_period->header.len;

	tlv_chan_list = (MrvlIEtypes_ChanListParamSet_t *) tlv;
	tlv_chan_list->header.type = TLV_TYPE_CHANLIST;
	tlv_chan_list->header.len = chan_list->no_of_channels *
		sizeof(ChanScanParamSet_t);

	for (i = 0; i < chan_list->no_of_channels; i++)
	{
		tlv_chan_list->chan_scan_param[i].chan_number =
			chan_list->chan_scan_param[i].chan_number;
		tlv_chan_list->chan_scan_param[i].min_scan_time =
			chan_list->chan_scan_param[i].min_scan_time;
		tlv_chan_list->chan_scan_param[i].max_scan_time =
			chan_list->chan_scan_param[i].max_scan_time;
	}

	size += sizeof(tlv_chan_list->header) + tlv_chan_list->header.len;
	tlv += sizeof(tlv_chan_list->header) + tlv_chan_list->header.len;

	if (custom_ie && custom_ie_len > 0) {
		tlv_custom_ie = (MrvlIEtypes_Data_t *) tlv;
		tlv_custom_ie->header.type = TLV_TYPE_PASSTHROUGH;
		tlv_custom_ie->header.len = custom_ie_len;
		memcpy(tlv_custom_ie->data, custom_ie, custom_ie_len);

		size += sizeof(tlv_custom_ie->header) +
			tlv_custom_ie->header.len;
		tlv += sizeof(tlv_custom_ie->header) +
			tlv_custom_ie->header.len;
	}

	if (smc_start_addr && smc_end_addr) {
		tlv_smc_addr_range = (MrvlIETypes_SmcAddrRange_t *) tlv;
		tlv_smc_addr_range->header.type = TLV_TYPE_SMCADDRRANGE;
		tlv_smc_addr_range->header.len = 2 * MLAN_MAC_ADDR_LENGTH +
			sizeof(uint16_t);

		memcpy(tlv_smc_addr_range->smcstartAddr, smc_start_addr,
			MLAN_MAC_ADDR_LENGTH);
		memcpy(tlv_smc_addr_range->smcendAddr, smc_end_addr,
			MLAN_MAC_ADDR_LENGTH);

		tlv_smc_addr_range->filter_type = filter_type;

		size += sizeof(tlv_smc_addr_range->header) +
			tlv_smc_addr_range->header.len;
		tlv += sizeof(tlv_smc_addr_range->header) +
			tlv_smc_addr_range->header.len;
	}

	tlv_smc_frame_filter = (MrvlIETypes_SmcFrameFilter_t *) tlv;
	tlv_smc_frame_filter->header.type = TLV_TYPE_SMCFRAMEFILTER;
	tlv_smc_frame_filter->header.len = smc_frame_filter_len;
	memcpy(tlv_smc_frame_filter->frame_filter, smc_frame_filter,
	       smc_frame_filter_len);

	size += sizeof(tlv_smc_frame_filter->header) +
		tlv_smc_frame_filter->header.len;
	tlv += sizeof(tlv_smc_frame_filter->header) +
		tlv_smc_frame_filter->header.len;

	cmd->size = size;
	cmd->seq_num = 0x00;
	cmd->result = 0x00;

	wifi_wait_for_cmdresp(NULL);

	return WM_SUCCESS;
}

int wifi_get_smart_mode_cfg()
{
	uint32_t size = S_DS_GEN + sizeof(HostCmd_DS_SYS_CONFIG) - 1;

	wifi_get_command_lock();
	HostCmd_DS_COMMAND *cmd = wifi_get_command_buffer();

	cmd->command = wlan_cpu_to_le16(HOST_CMD_SMART_MODE_CFG);
	HostCmd_DS_SYS_CONFIG *sys_config_cmd =
	    (HostCmd_DS_SYS_CONFIG *) ((uint32_t) cmd + S_DS_GEN);
	sys_config_cmd->action = HostCmd_ACT_GEN_GET;

	cmd->size = size;
	cmd->seq_num = 0x00;
	cmd->result = 0x00;

	wifi_wait_for_cmdresp(NULL);
	return WM_SUCCESS;
}

int wifi_start_smart_mode()
{
	uint32_t size = S_DS_GEN + sizeof(HostCmd_DS_SYS_CONFIG) - 1;

	wifi_get_command_lock();
	HostCmd_DS_COMMAND *cmd = wifi_get_command_buffer();

	cmd->command = wlan_cpu_to_le16(HOST_CMD_SMART_MODE_CFG);
	HostCmd_DS_SYS_CONFIG *sys_config_cmd =
	    (HostCmd_DS_SYS_CONFIG *) ((uint32_t) cmd + S_DS_GEN);
	sys_config_cmd->action = HostCmd_ACT_GEN_START;

	cmd->size = size;
	cmd->seq_num = 0x00;
	cmd->result = 0x00;

	wifi_wait_for_cmdresp(NULL);
	return WM_SUCCESS;
}

int wifi_stop_smart_mode()
{
	uint32_t size = S_DS_GEN + sizeof(HostCmd_DS_SYS_CONFIG) - 1;

	wifi_get_command_lock();
	HostCmd_DS_COMMAND *cmd = wifi_get_command_buffer();

	cmd->command = wlan_cpu_to_le16(HOST_CMD_SMART_MODE_CFG);
	HostCmd_DS_SYS_CONFIG *sys_config_cmd =
	    (HostCmd_DS_SYS_CONFIG *) ((uint32_t) cmd + S_DS_GEN);
	sys_config_cmd->action = HostCmd_ACT_GEN_STOP;

	cmd->size = size;
	cmd->seq_num = 0x00;
	cmd->result = 0x00;

	wifi_wait_for_cmdresp(NULL);

	return WM_SUCCESS;
}
