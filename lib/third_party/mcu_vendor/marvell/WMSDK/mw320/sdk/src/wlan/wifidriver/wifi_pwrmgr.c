/** @file wifi_pwrmgr.c
 *
 *  @brief This file provides all power management code for WIFI.
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

#include <wmstdio.h>
#include <wmassert.h>
#include <wmerrno.h>
#include <wm_os.h>
#include <board.h>
#include <wifi.h>
#include <wifi_events.h>

#include <wmlog.h>
#include "wifi-sdio.h"
#include "wifi-internal.h"

#define pwr_e(...)				\
	wmlog_e("pwr", ##__VA_ARGS__)
#define pwr_w(...)				\
	wmlog_w("pwr", ##__VA_ARGS__)

#ifdef CONFIG_PWR_DEBUG
#define pwr_d(...)				\
	wmlog("pwr", ##__VA_ARGS__)
#else
#define pwr_d(...)
#endif /* ! CONFIG_PWR_DEBUG */



#define MAX_LISTEN_INTERVAL_IN_BCON 49
#define MIN_LISTEN_INTERVAL_IN_TU 50
#define MAX_LISTEN_INTERVAL_IN_TU 65000
#define CLOSEST_DTIM_TO_LISTEN_INTERVAL  65534

static bool ieeeps_enabled;
static bool deepsleepps_enabled;
static bool uapps_enabled;

void wifi_set_ps_cfg(t_u16 multiple_dtims,
		     t_u16 bcn_miss_timeout,
		     t_u16 local_listen_interval,
		     t_u16 adhoc_wake_period,
		     t_u16 mode,
		     t_u16 delay_to_ps)
{
	pmlan_adapter pmadapter = ((mlan_private *)
				   mlan_adap->priv[0])->adapter;
	pmadapter->bcn_miss_time_out = bcn_miss_timeout;
	pmadapter->multiple_dtim = multiple_dtims;
	pmadapter->local_listen_interval = local_listen_interval;
	pmadapter->delay_to_ps = delay_to_ps;
	pmadapter->enhanced_ps_mode = mode;
	pmadapter->adhoc_awake_period = adhoc_wake_period;

}

void wifi_configure_null_pkt_interval(unsigned int null_pkt_interval)
{
	pmlan_adapter pmadapter = ((mlan_private *)
				   mlan_adap->priv[0])->adapter;
	pmadapter->null_pkt_interval = null_pkt_interval;
}

void wifi_configure_listen_interval(int listen_interval)
{
	mlan_private *pmpriv = (mlan_private *) mlan_adap->priv[0];

	pmpriv->listen_interval = listen_interval;

	pmlan_adapter pmadapter = ((mlan_private *)
			mlan_adap->priv[0])->adapter;
	wifi_set_ps_cfg(CLOSEST_DTIM_TO_LISTEN_INTERVAL,
			DEFAULT_BCN_MISS_TIMEOUT,
			0,
			0,
			PS_MODE_AUTO,
			DELAY_TO_PS_DEFAULT);
	pmadapter->local_listen_interval = listen_interval;
}

int wifi_send_hs_cfg_cmd(mlan_bss_type interface, t_u32 ipv4_addr,
				t_u16 action, t_u32 conditions)
{
	arpfilter_header *arpfilter = NULL;
	filter_entry *entry = NULL;
	void *pdata_buf = NULL;
	hs_config_param hs_cfg_obj;

	wifi_get_command_lock();

	HostCmd_DS_COMMAND *cmd = wifi_get_command_buffer();
	memset(cmd, 0x00, sizeof(HostCmd_DS_COMMAND));

	cmd->seq_num = HostCmd_SET_SEQ_NO_BSS_INFO(
						   0 /* seq_num */,
						   0 /* bss_num */,
						   interface);
	if (action == HS_CONFIGURE) {
		hs_cfg_obj.conditions = conditions;
		hs_cfg_obj.gap = 0x2;
#ifdef CONFIG_CPU_MC200
		hs_cfg_obj.gpio =  board_wifi_host_wakeup();
#elif CONFIG_CPU_MW300
		hs_cfg_obj.gpio =  16;
#endif
		pdata_buf = &hs_cfg_obj;
	}
	/* wake conditions for broadcast is
	 * enabled when bit 0 is set.
	 * The code below sets the correct bit which
	 * firmware will use to give host wakeup
	 */
	if (conditions & WIFI_WAKE_ON_ARP_BROADCAST) {
		hs_cfg_obj.conditions |= WIFI_WAKE_ON_ALL_BROADCAST;
		hs_cfg_obj.conditions &= ~WIFI_WAKE_ON_ARP_BROADCAST;
	}
	mlan_status status =
		wlan_ops_sta_prepare_cmd(
					 (mlan_private *) mlan_adap->priv[0],
					 HostCmd_CMD_802_11_HS_CFG_ENH,
					 HostCmd_ACT_GEN_SET,
					 0, NULL, pdata_buf,  cmd);
	/* Construct the ARP filter TLV */
	arpfilter = (arpfilter_header *) ((uint32_t)cmd + cmd->size);
	arpfilter->type = TLV_TYPE_ARP_FILTER;

	if (ipv4_addr) {
		entry = (filter_entry *) ((uint32_t)arpfilter +
					  sizeof(arpfilter_header));
		entry->addr_type = ADDR_TYPE_MULTICAST;
		entry->eth_type = ETHER_TYPE_ANY;
		entry->ipv4_addr =  IPV4_ADDR_ANY;
		entry++;


		entry->addr_type = ADDR_TYPE_BROADCAST;
		if (conditions & WIFI_WAKE_ON_ALL_BROADCAST) {
			entry->eth_type = ETHER_TYPE_ANY;
			entry->ipv4_addr = IPV4_ADDR_ANY ;
		} else {
			entry->eth_type = ETHER_TYPE_ARP;
			entry->ipv4_addr =  ipv4_addr;
		}
		entry++;

		entry->addr_type = ADDR_TYPE_UNICAST;
		entry->eth_type = ETHER_TYPE_ANY;
		entry->ipv4_addr =  IPV4_ADDR_ANY ;
		arpfilter->len = 3*sizeof(filter_entry);
	} else {
		arpfilter->len = 0;
	}
	cmd->size = cmd->size +
		sizeof(arpfilter_header) + arpfilter->len;

	wifi_wait_for_cmdresp(NULL);
	return status;
}

int wifi_send_power_save_command(int action, int ps_bitmap,
				 mlan_bss_type interface, void *pdata_buf)
{
	mlan_status status;
	wifi_get_command_lock();
	HostCmd_DS_COMMAND *cmd = wifi_get_command_buffer();
	memset(cmd, 0x00, sizeof(HostCmd_DS_COMMAND));

	cmd->seq_num = HostCmd_SET_SEQ_NO_BSS_INFO(
						   0 /* seq_num */,
						   0 /* bss_num */,
						   interface);
	if (ps_bitmap == BITMAP_AUTO_DS) {
		mlan_ds_auto_ds ds_param;
		ds_param.idletime = (*(t_u16 *)pdata_buf);
		pdata_buf = &ds_param;
	}

	cmd->result = 0x0;
	status = wlan_cmd_enh_power_mode((mlan_private *)
					 mlan_adap->priv[0], cmd,
					 action, ps_bitmap, pdata_buf);

	wifi_wait_for_cmdresp(NULL);
	return status;
}

int wifi_enter_ieee_power_save(void)
{
	return wifi_send_power_save_command(EN_AUTO_PS, BITMAP_STA_PS,
					    MLAN_BSS_TYPE_STA, NULL);
}

int wifi_exit_ieee_power_save(void)
{
	return wifi_send_power_save_command(DIS_AUTO_PS, BITMAP_STA_PS,
					    MLAN_BSS_TYPE_STA, NULL);
}

int wifi_enter_deepsleep_power_save(void)
{
	t_u16 idletime = 0;
	return wifi_send_power_save_command(EN_AUTO_PS, BITMAP_AUTO_DS,
					    MLAN_BSS_TYPE_STA, &idletime);
}

int wifi_exit_deepsleep_power_save(void)
{
	return wifi_send_power_save_command(DIS_AUTO_PS, BITMAP_AUTO_DS,
					    MLAN_BSS_TYPE_STA, NULL);
}

int wifi_uap_ps_inactivity_sleep_enter(int type,
					unsigned int ctrl_bitmap,
					unsigned int min_sleep,
					unsigned int max_sleep,
					unsigned int inactivity_to,
					unsigned int min_awake,
					unsigned int max_awake)
{
	mlan_ds_ps_mgmt  data_buff;

	memset(&data_buff, 0x00, sizeof(mlan_ds_ps_mgmt));

	data_buff.sleep_param.ctrl_bitmap = ctrl_bitmap;
	data_buff.sleep_param.min_sleep = min_sleep;
	data_buff.sleep_param.max_sleep = max_sleep;
	data_buff.inact_param.inactivity_to = inactivity_to;
	data_buff.inact_param.min_awake = min_awake;
	data_buff.inact_param.max_awake = max_awake;
	data_buff.ps_mode = PS_MODE_INACTIVITY;
	data_buff.flags = PS_FLAG_INACT_SLEEP_PARAM
		| PS_FLAG_SLEEP_PARAM;

	return wifi_send_power_save_command(EN_AUTO_PS, BITMAP_UAP_INACT_PS,
					    type, &data_buff);
}

int wifi_uap_ps_inactivity_sleep_exit(mlan_bss_type type)
{
	return wifi_send_power_save_command(DIS_AUTO_PS, BITMAP_UAP_INACT_PS,
					    type, NULL);
}

void send_sleep_confirm_command(mlan_bss_type interface)
{
	OPT_Confirm_Sleep *ps_cfm_sleep;

	HostCmd_DS_COMMAND *command = wifi_get_command_buffer();

	wifi_get_command_lock();
	ps_cfm_sleep = (OPT_Confirm_Sleep *) (command);

	memset(ps_cfm_sleep, 0, sizeof(OPT_Confirm_Sleep));
	ps_cfm_sleep->command = HostCmd_CMD_802_11_PS_MODE_ENH;
	ps_cfm_sleep->seq_num = HostCmd_SET_SEQ_NO_BSS_INFO(
						0 /* seq_num */,
						0 /* bss_num */,
						interface);

	ps_cfm_sleep->size = sizeof(OPT_Confirm_Sleep);
	ps_cfm_sleep->result = 0;
	ps_cfm_sleep->action = SLEEP_CONFIRM;
	ps_cfm_sleep->sleep_cfm.resp_ctrl = RESP_NEEDED;

	wifi_wait_for_cmdresp(NULL);
}

/* fixme: accept HostCmd_DS_COMMAND directly */
void wifi_process_hs_cfg_resp(t_u8 *cmd_res_buffer)
{
	pmlan_adapter pmadapter = ((mlan_private *)
				   mlan_adap->priv[0])->adapter;
	HostCmd_DS_802_11_HS_CFG_ENH *hs_cfg =
		(HostCmd_DS_802_11_HS_CFG_ENH *)
		(cmd_res_buffer + S_DS_GEN);
	if (hs_cfg->action == HS_ACTIVATE) {
		pwr_d("Host sleep activated");
		wlan_update_rxreorder_tbl(pmadapter, MTRUE);
	}
	else
		pwr_d("Host sleep configuration done");
}

int wifi_process_ps_enh_response(t_u8 *cmd_res_buffer, t_u16 *ps_event,
				 t_u16 *action)
{
	int result = WIFI_EVENT_REASON_FAILURE;
	MrvlIEtypesHeader_t *mrvl_tlv = NULL;
#ifdef CONFIG_PWR_DEBUG
	MrvlIEtypes_ps_param_t *ps_tlv = NULL;
#endif  /*  CONFIG_PWR_DEBUG*/
	HostCmd_DS_802_11_PS_MODE_ENH *ps_mode =
		(HostCmd_DS_802_11_PS_MODE_ENH *)
		(cmd_res_buffer + S_DS_GEN);

	*ps_event = WIFI_EVENT_PS_INVALID;
	*action = (ps_mode->action);
	if (ps_mode->action == EN_AUTO_PS) {
		if (ps_mode->params.auto_ps.ps_bitmap &
		    BITMAP_AUTO_DS) {
			pwr_d("Enabled deep sleep mode");
			mrvl_tlv =
				(MrvlIEtypesHeader_t *)
				((uint8_t *) ps_mode + AUTO_PS_FIX_SIZE);
			while (mrvl_tlv->type !=
			       TLV_TYPE_AUTO_DS_PARAM) {
				mrvl_tlv =
					(MrvlIEtypesHeader_t *)
					((uint8_t *) mrvl_tlv +
					 mrvl_tlv->len
					 + sizeof(MrvlIEtypesHeader_t));
			}
			*ps_event = WIFI_EVENT_DEEP_SLEEP;
			pwr_d("Enabled Deep Sleep mode");
			deepsleepps_enabled = true;
		}
		if (ps_mode->params.auto_ps.ps_bitmap
		    & BITMAP_STA_PS) {
			pwr_d("Enabled IEEE power "
				 "save mode");
			mrvl_tlv =
				(MrvlIEtypesHeader_t *)
				((uint8_t *) ps_mode + AUTO_PS_FIX_SIZE);
			while (mrvl_tlv->type !=
			       TLV_TYPE_PS_PARAM) {
				mrvl_tlv =
					(MrvlIEtypesHeader_t *)
					((uint8_t *) mrvl_tlv +
					 mrvl_tlv->len
					 + sizeof(MrvlIEtypesHeader_t));
			}
#ifdef CONFIG_PWR_DEBUG
			ps_tlv =
				(MrvlIEtypes_ps_param_t *)mrvl_tlv;
#endif  /*  CONFIG_PWR_DEBUG*/
			pwr_d("pscfg: %u %u %u %u "
			       "%u %u",
			       ps_tlv->param.null_pkt_interval,
			       ps_tlv->param.multiple_dtims,
			       ps_tlv->param.local_listen_interval,
			       ps_tlv->param.bcn_miss_timeout,
			       ps_tlv->param.delay_to_ps,
			       ps_tlv->param.mode);
			*ps_event = WIFI_EVENT_IEEE_PS;
			ieeeps_enabled = true;
		}
		if (ps_mode->params.auto_ps.ps_bitmap
		    &   BITMAP_UAP_INACT_PS) {
			*ps_event = WIFI_EVENT_UAP_INACT_PS;
			pwr_d("Enabled UAP Inactivity "
				 "Timeout Power Save mode");
			uapps_enabled = true;
		}
		result = WIFI_EVENT_REASON_SUCCESS;
	} else if (ps_mode->action == DIS_AUTO_PS) {
		if (ps_mode->params.ps_bitmap
		    & BITMAP_AUTO_DS) {
			pwr_d
				("Disabled DeepSleep mode");
			*ps_event = WIFI_EVENT_DEEP_SLEEP;
			deepsleepps_enabled = false;
		}
		if (ps_mode->params.ps_bitmap
		    & BITMAP_STA_PS) {
			pwr_d("Disabled IEEE power "
				 "save mode");
			*ps_event = WIFI_EVENT_IEEE_PS;
			ieeeps_enabled = false;
		}
		if (ps_mode->params.auto_ps.ps_bitmap
		    &   BITMAP_UAP_INACT_PS) {
			*ps_event = WIFI_EVENT_UAP_INACT_PS;
			pwr_d("Disabled UAP Inactivity "
				 "Timeout Power Save mode");
			uapps_enabled = false;
		}
		result = WIFI_EVENT_REASON_SUCCESS;
	} else if (ps_mode->action == GET_PS) {
		if (ps_mode->params.ps_bitmap
		    & BITMAP_AUTO_DS) {
			pwr_d("Deep sleep mode is on");
			mrvl_tlv =
				(MrvlIEtypesHeader_t *)
				((uint8_t *) ps_mode + AUTO_PS_FIX_SIZE);
			while (mrvl_tlv->type !=
			       TLV_TYPE_AUTO_DS_PARAM) {
				mrvl_tlv =
					(MrvlIEtypesHeader_t *)
					((uint8_t *) mrvl_tlv +
					 mrvl_tlv->len
					 + sizeof(MrvlIEtypesHeader_t));
			}
		}
		if (ps_mode->params.ps_bitmap & BITMAP_STA_PS) {
			pwr_d("IEEE power save mode is on");
			mrvl_tlv =
				(MrvlIEtypesHeader_t *)
				((uint8_t *) ps_mode + AUTO_PS_FIX_SIZE);
			while (mrvl_tlv->type !=
			       TLV_TYPE_PS_PARAM) {
				mrvl_tlv =
					(MrvlIEtypesHeader_t *)
					((uint8_t *) mrvl_tlv +
					 mrvl_tlv->len
					 + sizeof(MrvlIEtypesHeader_t));
			}
#ifdef CONFIG_PWR_DEBUG
			ps_tlv =
				(MrvlIEtypes_ps_param_t *)
				mrvl_tlv;
#endif  /*  CONFIG_PWR_DEBUG*/
			pwr_d("pscfg: %u %u %u %u "
				 "%u %u\r\n",
				 ps_tlv->param.null_pkt_interval,
				 ps_tlv->param.multiple_dtims,
				 ps_tlv->param.local_listen_interval,
				 ps_tlv->param.bcn_miss_timeout,
				 ps_tlv->param.delay_to_ps,
				 ps_tlv->param.mode);
		}
	} else if (ps_mode->action == SLEEP_CONFIRM) {
		if (ieeeps_enabled)
			*ps_event = WIFI_EVENT_IEEE_PS;
		else if (deepsleepps_enabled)
			*ps_event = WIFI_EVENT_DEEP_SLEEP;
		else if (uapps_enabled)
			*ps_event = WIFI_EVENT_UAP_INACT_PS;
		else
			return WIFI_EVENT_REASON_FAILURE;

		result = WIFI_EVENT_REASON_SUCCESS;
	}
	return result;
}

#ifdef CONFIG_P2P
int wifi_wfd_ps_inactivity_sleep_enter(unsigned int ctrl_bitmap,
				       unsigned int inactivity_to,
				       unsigned int min_sleep,
				       unsigned int max_sleep,
				       unsigned int min_awake,
				       unsigned int max_awake)
{
	mlan_ds_ps_mgmt  data_buff;
	data_buff.sleep_param.ctrl_bitmap = ctrl_bitmap;
	data_buff.sleep_param.min_sleep = min_sleep;
	data_buff.sleep_param.max_sleep = max_sleep;
	data_buff.inact_param.min_awake = min_awake;
	data_buff.inact_param.max_awake = max_awake;
	data_buff.inact_param.inactivity_to = inactivity_to;
	data_buff.flags = PS_FLAG_INACT_SLEEP_PARAM
		| PS_FLAG_SLEEP_PARAM;

	return wifi_send_power_save_command(EN_AUTO_PS, BITMAP_UAP_INACT_PS,
					    MLAN_BSS_TYPE_WIFIDIRECT,
					    &data_buff);
}

int  wifi_wfd_ps_inactivity_sleep_exit(void)
{
	return wifi_send_power_save_command(DIS_AUTO_PS, BITMAP_UAP_INACT_PS,
					    MLAN_BSS_TYPE_WIFIDIRECT,
					    NULL);
}
#endif /*  CONFIG_P2P*/
