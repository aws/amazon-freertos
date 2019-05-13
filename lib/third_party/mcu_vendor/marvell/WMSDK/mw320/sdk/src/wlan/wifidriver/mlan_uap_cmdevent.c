/** @file mlan_uap_cmdevent.c
 *
 *  @brief  This file provides the handling of AP mode command and event
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


/********************************************************
Change log:
    02/05/2009: initial version
********************************************************/

#include <mlan_wmsdk.h>

/* Additional WMSDK header files */
#include <wmstdio.h>
#include <wmassert.h>
#include <wmerrno.h>
#include <wm_os.h>

/* Always keep this include at the end of all include files */
#include <mlan_remap_mem_operations.h>

/*
 * Disable uAP 11N support if both CONFIG_UAP_AMPDU_TX and
 * CONFIG_UAP_AMPDU_RX are not set.
 *
 */
#if !defined(CONFIG_UAP_AMPDU_TX) && !defined(CONFIG_UAP_AMPDU_RX)
#define UAP_DISABLE_11N
#endif

#ifndef CONFIG_MLAN_WMSDK


/********************************************************
    Local Functions
********************************************************/
/**
 *  @brief This function handles the command response error
 *
 *  @param pmpriv       A pointer to mlan_private structure
 *  @param resp         A pointer to HostCmd_DS_COMMAND
 *  @param pioctl_buf   A pointer to command buffer
 *
 *  @return             N/A
 */
static void
uap_process_cmdresp_error(mlan_private * pmpriv, HostCmd_DS_COMMAND * resp,
                          mlan_ioctl_req * pioctl_buf)
{
    mlan_adapter *pmadapter = pmpriv->adapter;

    ENTER();

    PRINTM(MERROR, "CMD_RESP: cmd %#x error, result=%#x\n", resp->command,
           resp->result);
    if (pioctl_buf)
        pioctl_buf->status_code = resp->result;
    /*
     * Handling errors here
     */
    wlan_insert_cmd_to_free_q(pmadapter, pmadapter->curr_cmd);

    wlan_request_cmd_lock(pmadapter);
    pmadapter->curr_cmd = MNULL;
    wlan_release_cmd_lock(pmadapter);

    LEAVE();
    return;
}

/**
 * @brief This function prepares command of hs_cfg.
 *
 * @param pmpriv       A pointer to mlan_private structure
 * @param cmd          A pointer to HostCmd_DS_COMMAND structure
 * @param cmd_action   The action: GET or SET
 * @param pdata_buf    A pointer to data buffer
 *
 * @return             MLAN_STATUS_SUCCESS
 */
static mlan_status
wlan_uap_cmd_802_11_hs_cfg(IN pmlan_private pmpriv,
                           IN HostCmd_DS_COMMAND * cmd,
                           IN t_u16 cmd_action, IN hs_config_param * pdata_buf)
{
    HostCmd_DS_802_11_HS_CFG_ENH *phs_cfg = &cmd->params.opt_hs_cfg;

    ENTER();
    cmd->command = wlan_cpu_to_le16(HostCmd_CMD_802_11_HS_CFG_ENH);
    cmd->size =
        wlan_cpu_to_le16(S_DS_GEN + sizeof(HostCmd_DS_802_11_HS_CFG_ENH));

    if (pdata_buf == MNULL) {
        phs_cfg->action = wlan_cpu_to_le16(HS_ACTIVATE);
        phs_cfg->params.hs_activate.resp_ctrl = wlan_cpu_to_le16(RESP_NEEDED);
    } else {
        phs_cfg->action = wlan_cpu_to_le16(HS_CONFIGURE);
        phs_cfg->params.hs_config.conditions =
            wlan_cpu_to_le32(pdata_buf->conditions);
        phs_cfg->params.hs_config.gpio = pdata_buf->gpio;
        phs_cfg->params.hs_config.gap = pdata_buf->gap;
        PRINTM(MCMND, "HS_CFG_CMD: condition:0x%x gpio:0x%x gap:0x%x\n",
               phs_cfg->params.hs_config.conditions,
               phs_cfg->params.hs_config.gpio, phs_cfg->params.hs_config.gap);
    }
    LEAVE();
    return MLAN_STATUS_SUCCESS;
}

/**
 *  @brief This function prepares command of Tx data pause
 *
 *  @param pmpriv		A pointer to mlan_private structure
 *  @param cmd	   		A pointer to HostCmd_DS_COMMAND structure
 *  @param cmd_action   the action: GET or SET
 *  @param pdata_buf    A pointer to data buffer
 *  @return         MLAN_STATUS_SUCCESS
 */
static mlan_status
wlan_uap_cmd_txdatapause(pmlan_private pmpriv,
                         IN HostCmd_DS_COMMAND * cmd,
                         IN t_u16 cmd_action, IN t_void * pdata_buf)
{
    HostCmd_DS_CMD_TX_DATA_PAUSE *pause_cmd =
        (HostCmd_DS_CMD_TX_DATA_PAUSE *) & cmd->params.tx_data_pause;
    mlan_ds_misc_tx_datapause *data_pause =
        (mlan_ds_misc_tx_datapause *) pdata_buf;

    ENTER();

    cmd->command = wlan_cpu_to_le16(HostCmd_CMD_CFG_TX_DATA_PAUSE);
    cmd->size =
        wlan_cpu_to_le16(sizeof(HostCmd_DS_CMD_TX_DATA_PAUSE) + S_DS_GEN);
    pause_cmd->action = wlan_cpu_to_le16(cmd_action);

    if (cmd_action == HostCmd_ACT_GEN_SET) {
        pause_cmd->enable_tx_pause = (t_u8) data_pause->tx_pause;
        pause_cmd->pause_tx_count = (t_u8) data_pause->tx_buf_cnt;
    }

    LEAVE();
    return MLAN_STATUS_SUCCESS;
}

/**
 *  @brief This function handles the command response of Tx data pause
 *
 *  @param pmpriv       A pointer to mlan_private structure
 *  @param resp         A pointer to HostCmd_DS_COMMAND
 *  @param pioctl_buf   A pointer to mlan_ioctl_req structure
 *
 *  @return             MLAN_STATUS_SUCCESS
 */
static mlan_status
wlan_uap_ret_txdatapause(IN pmlan_private pmpriv,
                         IN HostCmd_DS_COMMAND * resp,
                         IN mlan_ioctl_req * pioctl_buf)
{
    HostCmd_DS_CMD_TX_DATA_PAUSE *pause_cmd =
        (HostCmd_DS_CMD_TX_DATA_PAUSE *) & resp->params.tx_data_pause;
    mlan_ds_misc_cfg *misc_cfg = MNULL;

    ENTER();

    if (pioctl_buf) {
        misc_cfg = (mlan_ds_misc_cfg *) pioctl_buf->pbuf;
        misc_cfg->param.tx_datapause.tx_pause = pause_cmd->enable_tx_pause;
        misc_cfg->param.tx_datapause.tx_buf_cnt = pause_cmd->pause_tx_count;
    }
    LEAVE();
    return MLAN_STATUS_SUCCESS;
}

/**
 *  @brief This function will process tx pause event
 *
 *
 *  @param priv    A pointer to mlan_private
 *  @param pevent  A pointer to event buf
 *
 *  @return	       N/A
 */
static void
wlan_process_tx_pause_event(pmlan_private priv, pmlan_buffer pevent)
{
    t_u16 tlv_type, tlv_len;
    int tlv_buf_left = pevent->data_len - sizeof(t_u32);
    MrvlIEtypesHeader_t *tlv =
        (MrvlIEtypesHeader_t *) (pevent->pbuf + pevent->data_offset +
                                 sizeof(t_u32));
    MrvlIEtypes_tx_pause_t *tx_pause_tlv;
    sta_node *sta_ptr = MNULL;

    ENTER();

    while (tlv_buf_left >= (int) sizeof(MrvlIEtypesHeader_t)) {
        tlv_type = wlan_le16_to_cpu(tlv->type);
        tlv_len = wlan_le16_to_cpu(tlv->len);
        if ((sizeof(MrvlIEtypesHeader_t) + tlv_len) >
            (unsigned int) tlv_buf_left) {
            PRINTM(MERROR, "wrong tlv: tlvLen=%d, tlvBufLeft=%d\n", tlv_len,
                   tlv_buf_left);
            break;
        }
        if (tlv_type == TLV_TYPE_TX_PAUSE) {
            tx_pause_tlv = (MrvlIEtypes_tx_pause_t *) tlv;
            PRINTM(MCMND,
                   "TxPause: %02x:%02x:%02x:%02x:%02x:%02x pause=%d, pkts=%d\n",
                   tx_pause_tlv->peermac[0], tx_pause_tlv->peermac[1],
                   tx_pause_tlv->peermac[2], tx_pause_tlv->peermac[3],
                   tx_pause_tlv->peermac[4], tx_pause_tlv->peermac[5],
                   tx_pause_tlv->tx_pause, tx_pause_tlv->pkt_cnt);
            if ((sta_ptr = wlan_get_station_entry(priv, tx_pause_tlv->peermac))) {
                if (sta_ptr->tx_pause != tx_pause_tlv->tx_pause) {
                    sta_ptr->tx_pause = tx_pause_tlv->tx_pause;
                    wlan_updata_ralist_tx_pause(priv, tx_pause_tlv->peermac,
                                                tx_pause_tlv->tx_pause);
                }
            }
        }
        tlv_buf_left -= (sizeof(MrvlIEtypesHeader_t) + tlv_len);
        tlv =
            (MrvlIEtypesHeader_t *) ((t_u8 *) tlv + tlv_len +
                                     sizeof(MrvlIEtypesHeader_t));
    }

    LEAVE();
    return;
}
#endif /* CONFIG_MLAN_WMSDK */

/**
 *  @brief This function prepares command for config uap settings
 *
 *  @param pmpriv		A pointer to mlan_private structure
 *  @param cmd	   		A pointer to HostCmd_DS_COMMAND structure
 *  @param cmd_action   the action: GET or SET
 *  @param pioctl_buf   A pointer to mlan_ioctl_req structure
 *  @return         MLAN_STATUS_SUCCESS or MLAN_STATUS_FAILURE
 */
static mlan_status
wlan_uap_cmd_ap_config(pmlan_private pmpriv,
                       IN HostCmd_DS_COMMAND * cmd,
                       IN t_u16 cmd_action, IN pmlan_ioctl_req pioctl_buf)
{
    mlan_ds_bss *bss = MNULL;
    HostCmd_DS_SYS_CONFIG *sys_config =
        (HostCmd_DS_SYS_CONFIG *) & cmd->params.sys_config;
    t_u8 *tlv = MNULL;
    MrvlIEtypes_MacAddr_t *tlv_mac = MNULL;
    MrvlIEtypes_SsIdParamSet_t *tlv_ssid = MNULL;
    MrvlIEtypes_beacon_period_t *tlv_beacon_period = MNULL;
    MrvlIEtypes_ecsa_config_t *tlv_ecsa_config = MNULL;
    MrvlIEtypes_dtim_period_t *tlv_dtim_period = MNULL;
    MrvlIEtypes_RatesParamSet_t *tlv_rates = MNULL;
#ifdef UAP_DISABLE_11N
    MrvlIEtypes_Cipher_t *tlv_cipher = MNULL;
#endif
#ifndef CONFIG_MLAN_WMSDK
    MrvlIEtypes_tx_rate_t *tlv_txrate = MNULL;
    MrvlIEtypes_mcbc_rate_t *tlv_mcbc_rate = MNULL;
    MrvlIEtypes_tx_power_t *tlv_tx_power = MNULL;
#endif /* CONFIG_MLAN_WMSDK */
    MrvlIEtypes_bcast_ssid_t *tlv_bcast_ssid = MNULL;
#ifndef CONFIG_MLAN_WMSDK
    MrvlIEtypes_antenna_mode_t *tlv_antenna = MNULL;
    MrvlIEtypes_pkt_forward_t *tlv_pkt_forward = MNULL;
    MrvlIEtypes_max_sta_count_t *tlv_sta_count = MNULL;
    MrvlIEtypes_sta_ageout_t *tlv_sta_ageout = MNULL;
    MrvlIEtypes_ps_sta_ageout_t *tlv_ps_sta_ageout = MNULL;
    MrvlIEtypes_rts_threshold_t *tlv_rts_threshold = MNULL;
    MrvlIEtypes_frag_threshold_t *tlv_frag_threshold = MNULL;
    MrvlIEtypes_retry_limit_t *tlv_retry_limit = MNULL;
    MrvlIEtypes_eapol_pwk_hsk_timeout_t *tlv_pairwise_timeout = MNULL;
    MrvlIEtypes_eapol_pwk_hsk_retries_t *tlv_pairwise_retries = MNULL;
    MrvlIEtypes_eapol_gwk_hsk_timeout_t *tlv_groupwise_timeout = MNULL;
    MrvlIEtypes_eapol_gwk_hsk_retries_t *tlv_groupwise_retries = MNULL;
    MrvlIEtypes_mgmt_ie_passthru_t *tlv_mgmt_ie_passthru = MNULL;
    MrvlIEtypes_2040_coex_enable_t *tlv_2040_coex_enable = MNULL;
    MrvlIEtypes_mac_filter_t *tlv_mac_filter = MNULL;
    MrvlIEtypes_auth_type_t *tlv_auth_type = MNULL;
#endif /* CONFIG_MLAN_WMSDK */
    MrvlIEtypes_channel_band_t *tlv_chan_band = MNULL;
    MrvlIEtypes_ChanListParamSet_t *tlv_chan_list = MNULL;
    ChanScanParamSet_t *pscan_chan = MNULL;
    MrvlIEtypes_encrypt_protocol_t *tlv_encrypt_protocol = MNULL;
    MrvlIEtypes_akmp_t *tlv_akmp = MNULL;
    MrvlIEtypes_pwk_cipher_t *tlv_pwk_cipher = MNULL;
    MrvlIEtypes_gwk_cipher_t *tlv_gwk_cipher = MNULL;
#ifndef CONFIG_MLAN_WMSDK
    MrvlIEtypes_rsn_replay_prot_t *tlv_rsn_prot = MNULL;
#endif /* CONFIG_MLAN_WMSDK */
    MrvlIEtypes_passphrase_t *tlv_passphrase = MNULL;
#ifndef CONFIG_MLAN_WMSDK
    MrvlIEtypes_group_rekey_time_t *tlv_rekey_time = MNULL;
    MrvlIEtypes_wep_key_t *tlv_wep_key = MNULL;
    MrvlIEtypes_wmm_parameter_t *tlv_wmm_parameter = MNULL;
#endif /* CONFIG_MLAN_WMSDK */
#ifndef UAP_DISABLE_11N
    MrvlIETypes_HTCap_t *tlv_htcap = MNULL;
#endif /* UAP_DISABLE_11N */
    t_u32 cmd_size = 0;
    t_u8 zero_mac[] = { 0, 0, 0, 0, 0, 0 };
    t_u16 i;
    /* t_u16 ac; */

    ENTER();
    if (pioctl_buf == MNULL) {
        LEAVE();
        return MLAN_STATUS_FAILURE;
    }

    bss = (mlan_ds_bss *) pioctl_buf->pbuf;

    cmd->command = wlan_cpu_to_le16(HOST_CMD_APCMD_SYS_CONFIGURE);
    sys_config->action = wlan_cpu_to_le16(cmd_action);
    cmd_size = sizeof(HostCmd_DS_SYS_CONFIG) - 1 + S_DS_GEN;

    tlv = (t_u8 *) sys_config->tlv_buffer;
    if (memcmp
        (pmpriv->adapter, zero_mac, &bss->param.bss_config.mac_addr,
         MLAN_MAC_ADDR_LENGTH)) {
        tlv_mac = (MrvlIEtypes_MacAddr_t *) tlv;
        tlv_mac->header.type = wlan_cpu_to_le16(TLV_TYPE_UAP_MAC_ADDRESS);
        tlv_mac->header.len = wlan_cpu_to_le16(MLAN_MAC_ADDR_LENGTH);
        memcpy(pmpriv->adapter, tlv_mac->mac, &bss->param.bss_config.mac_addr,
               MLAN_MAC_ADDR_LENGTH);
        cmd_size += sizeof(MrvlIEtypes_MacAddr_t);
        tlv += sizeof(MrvlIEtypes_MacAddr_t);
    }

    if (bss->param.bss_config.ssid.ssid_len) {
        tlv_ssid = (MrvlIEtypes_SsIdParamSet_t *) tlv;
        tlv_ssid->header.type = wlan_cpu_to_le16(TLV_TYPE_SSID);
        tlv_ssid->header.len =
            wlan_cpu_to_le16((t_u16) bss->param.bss_config.ssid.ssid_len);
        memcpy(pmpriv->adapter, tlv_ssid->ssid, bss->param.bss_config.ssid.ssid,
               bss->param.bss_config.ssid.ssid_len);
        cmd_size +=
            sizeof(MrvlIEtypesHeader_t) + bss->param.bss_config.ssid.ssid_len;
        tlv +=
            sizeof(MrvlIEtypesHeader_t) + bss->param.bss_config.ssid.ssid_len;
    }

    if ((bss->param.bss_config.beacon_period >= MIN_BEACON_PERIOD) &&
        (bss->param.bss_config.beacon_period <= MAX_BEACON_PERIOD)) {
        tlv_beacon_period = (MrvlIEtypes_beacon_period_t *) tlv;
        tlv_beacon_period->header.type =
            wlan_cpu_to_le16(TLV_TYPE_UAP_BEACON_PERIOD);
        tlv_beacon_period->header.len = wlan_cpu_to_le16(sizeof(t_u16));
        tlv_beacon_period->beacon_period =
            wlan_cpu_to_le16(bss->param.bss_config.beacon_period);
        cmd_size += sizeof(MrvlIEtypes_beacon_period_t);
        tlv += sizeof(MrvlIEtypes_beacon_period_t);
    }

    if ((bss->param.bss_config.chan_sw_count >= MIN_CHSW_COUNT) &&
        (bss->param.bss_config.chan_sw_count <= MAX_CHSW_COUNT)) {
        tlv_ecsa_config = (MrvlIEtypes_ecsa_config_t *) tlv;
        tlv_ecsa_config->header.type =
            wlan_cpu_to_le16(TLV_TYPE_UAP_ECSA_CONFIG);
        tlv_ecsa_config->header.len = wlan_cpu_to_le16(sizeof(t_u16) +
	    sizeof(t_u8) + sizeof(t_u8));
        tlv_ecsa_config->enable = 1;
	tlv_ecsa_config->ChannelSwitchMode = 0;
	tlv_ecsa_config->ChannelSwitchCount =
            bss->param.bss_config.chan_sw_count;
        cmd_size += sizeof(MrvlIEtypes_ecsa_config_t);
        tlv += sizeof(MrvlIEtypes_ecsa_config_t);
    }

    if ((bss->param.bss_config.dtim_period >= MIN_DTIM_PERIOD) &&
        (bss->param.bss_config.dtim_period <= MAX_DTIM_PERIOD)) {
        tlv_dtim_period = (MrvlIEtypes_dtim_period_t *) tlv;
        tlv_dtim_period->header.type =
            wlan_cpu_to_le16(TLV_TYPE_UAP_DTIM_PERIOD);
        tlv_dtim_period->header.len = wlan_cpu_to_le16(sizeof(t_u8));
        tlv_dtim_period->dtim_period = bss->param.bss_config.dtim_period;
        cmd_size += sizeof(MrvlIEtypes_dtim_period_t);
        tlv += sizeof(MrvlIEtypes_dtim_period_t);
    }

    if (bss->param.bss_config.rates[0]) {
        tlv_rates = (MrvlIEtypes_RatesParamSet_t *) tlv;
        tlv_rates->header.type = wlan_cpu_to_le16(TLV_TYPE_RATES);
        for (i = 0; i < MAX_DATA_RATES && bss->param.bss_config.rates[i]; i++) {
            tlv_rates->rates[i] = bss->param.bss_config.rates[i];
        }
        tlv_rates->header.len = wlan_cpu_to_le16(i);
        cmd_size += sizeof(MrvlIEtypesHeader_t) + i;
        tlv += sizeof(MrvlIEtypesHeader_t) + i;
    }

#ifndef CONFIG_MLAN_WMSDK
    if (bss->param.bss_config.tx_data_rate <= DATA_RATE_54M) {
        tlv_txrate = (MrvlIEtypes_tx_rate_t *) tlv;
        tlv_txrate->header.type = wlan_cpu_to_le16(TLV_TYPE_UAP_TX_DATA_RATE);
        tlv_txrate->header.len = wlan_cpu_to_le16(sizeof(t_u16));
        tlv_txrate->tx_data_rate =
            wlan_cpu_to_le16(bss->param.bss_config.tx_data_rate);
        cmd_size += sizeof(MrvlIEtypes_tx_rate_t);
        tlv += sizeof(MrvlIEtypes_tx_rate_t);
    }

    if (bss->param.bss_config.mcbc_data_rate <= DATA_RATE_54M) {
        tlv_mcbc_rate = (MrvlIEtypes_mcbc_rate_t *) tlv;
        tlv_mcbc_rate->header.type =
            wlan_cpu_to_le16(TLV_TYPE_UAP_MCBC_DATA_RATE);
        tlv_mcbc_rate->header.len = wlan_cpu_to_le16(sizeof(t_u16));
        tlv_mcbc_rate->mcbc_data_rate =
            wlan_cpu_to_le16(bss->param.bss_config.mcbc_data_rate);
        cmd_size += sizeof(MrvlIEtypes_mcbc_rate_t);
        tlv += sizeof(MrvlIEtypes_mcbc_rate_t);
    }

    if (bss->param.bss_config.tx_power_level <= MAX_TX_POWER) {
        tlv_tx_power = (MrvlIEtypes_tx_power_t *) tlv;
        tlv_tx_power->header.type = wlan_cpu_to_le16(TLV_TYPE_UAP_TX_POWER);
        tlv_tx_power->header.len = wlan_cpu_to_le16(sizeof(t_u8));
        tlv_tx_power->tx_power = bss->param.bss_config.tx_power_level;
        cmd_size += sizeof(MrvlIEtypes_tx_power_t);
        tlv += sizeof(MrvlIEtypes_tx_power_t);
    }
#endif /* CONFIG_MLAN_WMSDK */

    if (bss->param.bss_config.bcast_ssid_ctl <= MTRUE) {
        tlv_bcast_ssid = (MrvlIEtypes_bcast_ssid_t *) tlv;
        tlv_bcast_ssid->header.type =
            wlan_cpu_to_le16(TLV_TYPE_UAP_BCAST_SSID_CTL);
        tlv_bcast_ssid->header.len = wlan_cpu_to_le16(sizeof(t_u8));
        tlv_bcast_ssid->bcast_ssid_ctl = bss->param.bss_config.bcast_ssid_ctl;
        cmd_size += sizeof(MrvlIEtypes_bcast_ssid_t);
        tlv += sizeof(MrvlIEtypes_bcast_ssid_t);
    }

#ifndef CONFIG_MLAN_WMSDK
    if ((bss->param.bss_config.tx_antenna == ANTENNA_MODE_A) ||
        (bss->param.bss_config.tx_antenna == ANTENNA_MODE_B)) {
        tlv_antenna = (MrvlIEtypes_antenna_mode_t *) tlv;
        tlv_antenna->header.type = wlan_cpu_to_le16(TLV_TYPE_UAP_ANTENNA_CTL);
        tlv_antenna->header.len = wlan_cpu_to_le16(sizeof(t_u8) + sizeof(t_u8));
        tlv_antenna->which_antenna = TX_ANTENNA;
        tlv_antenna->antenna_mode = bss->param.bss_config.tx_antenna;
        cmd_size += sizeof(MrvlIEtypes_antenna_mode_t);
        tlv += sizeof(MrvlIEtypes_antenna_mode_t);
    }

    if ((bss->param.bss_config.rx_antenna == ANTENNA_MODE_A) ||
        (bss->param.bss_config.rx_antenna == ANTENNA_MODE_B)) {
        tlv_antenna = (MrvlIEtypes_antenna_mode_t *) tlv;
        tlv_antenna->header.type = wlan_cpu_to_le16(TLV_TYPE_UAP_ANTENNA_CTL);
        tlv_antenna->header.len = wlan_cpu_to_le16(sizeof(t_u8) + sizeof(t_u8));
        tlv_antenna->which_antenna = RX_ANTENNA;
        tlv_antenna->antenna_mode = bss->param.bss_config.rx_antenna;
        cmd_size += sizeof(MrvlIEtypes_antenna_mode_t);
        tlv += sizeof(MrvlIEtypes_antenna_mode_t);
    }

    if (bss->param.bss_config.pkt_forward_ctl <= MAX_PKT_FWD_CTRL) {
        tlv_pkt_forward = (MrvlIEtypes_pkt_forward_t *) tlv;
        tlv_pkt_forward->header.type =
            wlan_cpu_to_le16(TLV_TYPE_UAP_PKT_FWD_CTL);
        tlv_pkt_forward->header.len = wlan_cpu_to_le16(sizeof(t_u8));
        tlv_pkt_forward->pkt_forward_ctl =
            bss->param.bss_config.pkt_forward_ctl;
        cmd_size += sizeof(MrvlIEtypes_pkt_forward_t);
        tlv += sizeof(MrvlIEtypes_pkt_forward_t);
    }

    if (bss->param.bss_config.max_sta_count <= MAX_STA_COUNT) {
        tlv_sta_count = (MrvlIEtypes_max_sta_count_t *) tlv;
        tlv_sta_count->header.type = wlan_cpu_to_le16(TLV_TYPE_UAP_MAX_STA_CNT);
        tlv_sta_count->header.len = wlan_cpu_to_le16(sizeof(t_u16));
        tlv_sta_count->max_sta_count =
            wlan_cpu_to_le16(bss->param.bss_config.max_sta_count);
        cmd_size += sizeof(MrvlIEtypes_max_sta_count_t);
        tlv += sizeof(MrvlIEtypes_max_sta_count_t);
    }

    if (((bss->param.bss_config.sta_ageout_timer >= MIN_STAGE_OUT_TIME) &&
         (bss->param.bss_config.sta_ageout_timer <= MAX_STAGE_OUT_TIME)) ||
        (bss->param.bss_config.sta_ageout_timer == 0)) {
        tlv_sta_ageout = (MrvlIEtypes_sta_ageout_t *) tlv;
        tlv_sta_ageout->header.type =
            wlan_cpu_to_le16(TLV_TYPE_UAP_STA_AGEOUT_TIMER);
        tlv_sta_ageout->header.len = wlan_cpu_to_le16(sizeof(t_u32));
        tlv_sta_ageout->sta_ageout_timer =
            wlan_cpu_to_le32(bss->param.bss_config.sta_ageout_timer);
        cmd_size += sizeof(MrvlIEtypes_sta_ageout_t);
        tlv += sizeof(MrvlIEtypes_sta_ageout_t);
    }

    if (((bss->param.bss_config.ps_sta_ageout_timer >= MIN_STAGE_OUT_TIME) &&
         (bss->param.bss_config.ps_sta_ageout_timer <= MAX_STAGE_OUT_TIME)) ||
        (bss->param.bss_config.ps_sta_ageout_timer == 0)) {
        tlv_ps_sta_ageout = (MrvlIEtypes_ps_sta_ageout_t *) tlv;
        tlv_ps_sta_ageout->header.type =
            wlan_cpu_to_le16(TLV_TYPE_UAP_PS_STA_AGEOUT_TIMER);
        tlv_ps_sta_ageout->header.len = wlan_cpu_to_le16(sizeof(t_u32));
        tlv_ps_sta_ageout->ps_sta_ageout_timer =
            wlan_cpu_to_le32(bss->param.bss_config.ps_sta_ageout_timer);
        cmd_size += sizeof(MrvlIEtypes_ps_sta_ageout_t);
        tlv += sizeof(MrvlIEtypes_ps_sta_ageout_t);
    }
    if (bss->param.bss_config.rts_threshold <= MAX_RTS_THRESHOLD) {
        tlv_rts_threshold = (MrvlIEtypes_rts_threshold_t *) tlv;
        tlv_rts_threshold->header.type =
            wlan_cpu_to_le16(TLV_TYPE_UAP_RTS_THRESHOLD);
        tlv_rts_threshold->header.len = wlan_cpu_to_le16(sizeof(t_u16));
        tlv_rts_threshold->rts_threshold =
            wlan_cpu_to_le16(bss->param.bss_config.rts_threshold);
        cmd_size += sizeof(MrvlIEtypes_rts_threshold_t);
        tlv += sizeof(MrvlIEtypes_rts_threshold_t);
    }

    if ((bss->param.bss_config.frag_threshold >= MIN_FRAG_THRESHOLD) &&
        (bss->param.bss_config.frag_threshold <= MAX_FRAG_THRESHOLD)) {
        tlv_frag_threshold = (MrvlIEtypes_frag_threshold_t *) tlv;
        tlv_frag_threshold->header.type =
            wlan_cpu_to_le16(TLV_TYPE_UAP_FRAG_THRESHOLD);
        tlv_frag_threshold->header.len = wlan_cpu_to_le16(sizeof(t_u16));
        tlv_frag_threshold->frag_threshold =
            wlan_cpu_to_le16(bss->param.bss_config.frag_threshold);
        cmd_size += sizeof(MrvlIEtypes_frag_threshold_t);
        tlv += sizeof(MrvlIEtypes_frag_threshold_t);
    }

    if (bss->param.bss_config.retry_limit <= MAX_RETRY_LIMIT) {
        tlv_retry_limit = (MrvlIEtypes_retry_limit_t *) tlv;
        tlv_retry_limit->header.type =
            wlan_cpu_to_le16(TLV_TYPE_UAP_RETRY_LIMIT);
        tlv_retry_limit->header.len = wlan_cpu_to_le16(sizeof(t_u8));
        tlv_retry_limit->retry_limit = (t_u8) bss->param.bss_config.retry_limit;
        cmd_size += sizeof(MrvlIEtypes_retry_limit_t);
        tlv += sizeof(MrvlIEtypes_retry_limit_t);
    }

    if (bss->param.bss_config.pairwise_update_timeout < (MAX_VALID_DWORD)) {
        tlv_pairwise_timeout = (MrvlIEtypes_eapol_pwk_hsk_timeout_t *) tlv;
        tlv_pairwise_timeout->header.type =
            wlan_cpu_to_le16(TLV_TYPE_UAP_EAPOL_PWK_HSK_TIMEOUT);
        tlv_pairwise_timeout->header.len = wlan_cpu_to_le16(sizeof(t_u32));
        tlv_pairwise_timeout->pairwise_update_timeout =
            wlan_cpu_to_le32(bss->param.bss_config.pairwise_update_timeout);
        cmd_size += sizeof(MrvlIEtypes_eapol_pwk_hsk_timeout_t);
        tlv += sizeof(MrvlIEtypes_eapol_pwk_hsk_timeout_t);
    }

    if (bss->param.bss_config.pwk_retries < (MAX_VALID_DWORD)) {
        tlv_pairwise_retries = (MrvlIEtypes_eapol_pwk_hsk_retries_t *) tlv;
        tlv_pairwise_retries->header.type =
            wlan_cpu_to_le16(TLV_TYPE_UAP_EAPOL_PWK_HSK_RETRIES);
        tlv_pairwise_retries->header.len = wlan_cpu_to_le16(sizeof(t_u32));
        tlv_pairwise_retries->pwk_retries =
            wlan_cpu_to_le32(bss->param.bss_config.pwk_retries);
        cmd_size += sizeof(MrvlIEtypes_eapol_pwk_hsk_retries_t);
        tlv += sizeof(MrvlIEtypes_eapol_pwk_hsk_retries_t);
    }

    if (bss->param.bss_config.groupwise_update_timeout < (MAX_VALID_DWORD)) {
        tlv_groupwise_timeout = (MrvlIEtypes_eapol_gwk_hsk_timeout_t *) tlv;
        tlv_groupwise_timeout->header.type =
            wlan_cpu_to_le16(TLV_TYPE_UAP_EAPOL_GWK_HSK_TIMEOUT);
        tlv_groupwise_timeout->header.len = wlan_cpu_to_le16(sizeof(t_u32));
        tlv_groupwise_timeout->groupwise_update_timeout =
            wlan_cpu_to_le32(bss->param.bss_config.groupwise_update_timeout);
        cmd_size += sizeof(MrvlIEtypes_eapol_gwk_hsk_timeout_t);
        tlv += sizeof(MrvlIEtypes_eapol_gwk_hsk_timeout_t);
    }

    if (bss->param.bss_config.gwk_retries < (MAX_VALID_DWORD)) {
        tlv_groupwise_retries = (MrvlIEtypes_eapol_gwk_hsk_retries_t *) tlv;
        tlv_groupwise_retries->header.type =
            wlan_cpu_to_le16(TLV_TYPE_UAP_EAPOL_GWK_HSK_RETRIES);
        tlv_groupwise_retries->header.len = wlan_cpu_to_le16(sizeof(t_u32));
        tlv_groupwise_retries->gwk_retries =
            wlan_cpu_to_le32(bss->param.bss_config.gwk_retries);
        cmd_size += sizeof(MrvlIEtypes_eapol_gwk_hsk_retries_t);
        tlv += sizeof(MrvlIEtypes_eapol_gwk_hsk_retries_t);
    }

    if ((bss->param.bss_config.filter.filter_mode <= MAC_FILTER_MODE_BLOCK_MAC)
        && (bss->param.bss_config.filter.mac_count <= MAX_MAC_FILTER_NUM)) {
        tlv_mac_filter = (MrvlIEtypes_mac_filter_t *) tlv;
        tlv_mac_filter->header.type =
            wlan_cpu_to_le16(TLV_TYPE_UAP_STA_MAC_ADDR_FILTER);
        tlv_mac_filter->header.len =
            wlan_cpu_to_le16(2 +
                             MLAN_MAC_ADDR_LENGTH *
                             bss->param.bss_config.filter.mac_count);
        tlv_mac_filter->count = (t_u8) bss->param.bss_config.filter.mac_count;
        tlv_mac_filter->filter_mode =
            (t_u8) bss->param.bss_config.filter.filter_mode;
        memcpy(pmpriv->adapter, tlv_mac_filter->mac_address,
               (t_u8 *) bss->param.bss_config.filter.mac_list,
               MLAN_MAC_ADDR_LENGTH * bss->param.bss_config.filter.mac_count);
        cmd_size +=
            sizeof(MrvlIEtypesHeader_t) + 2 +
            MLAN_MAC_ADDR_LENGTH * bss->param.bss_config.filter.mac_count;
        tlv +=
            sizeof(MrvlIEtypesHeader_t) + 2 +
            MLAN_MAC_ADDR_LENGTH * bss->param.bss_config.filter.mac_count;
    }
#endif /* CONFIG_MLAN_WMSDK */

    if ((((bss->param.bss_config.band_cfg & BAND_CONFIG_ACS_MODE) ==
          BAND_CONFIG_MANUAL) && (bss->param.bss_config.channel > 0) &&
         (bss->param.bss_config.channel <= MLAN_MAX_CHANNEL)) ||
        (bss->param.bss_config.band_cfg & BAND_CONFIG_ACS_MODE)) {
        tlv_chan_band = (MrvlIEtypes_channel_band_t *) tlv;
        tlv_chan_band->header.type =
            wlan_cpu_to_le16(TLV_TYPE_UAP_CHAN_BAND_CONFIG);
        tlv_chan_band->header.len =
            wlan_cpu_to_le16(sizeof(t_u8) + sizeof(t_u8));
        tlv_chan_band->band_config = bss->param.bss_config.band_cfg;
        tlv_chan_band->channel = bss->param.bss_config.channel;
        cmd_size += sizeof(MrvlIEtypes_channel_band_t);
        tlv += sizeof(MrvlIEtypes_channel_band_t);
    }

    if ((bss->param.bss_config.num_of_chan) &&
        (bss->param.bss_config.num_of_chan <= MLAN_MAX_CHANNEL)) {
        tlv_chan_list = (MrvlIEtypes_ChanListParamSet_t *) tlv;
        tlv_chan_list->header.type = wlan_cpu_to_le16(TLV_TYPE_CHANLIST);
        tlv_chan_list->header.len =
            wlan_cpu_to_le16((t_u16)
                             (sizeof(ChanScanParamSet_t) *
                              bss->param.bss_config.num_of_chan));
        pscan_chan = tlv_chan_list->chan_scan_param;
        for (i = 0; i < bss->param.bss_config.num_of_chan; i++) {
            pscan_chan->chan_number =
                bss->param.bss_config.chan_list[i].chan_number;
            pscan_chan->radio_type =
                bss->param.bss_config.chan_list[i].band_config_type;
            pscan_chan++;
        }
        cmd_size += sizeof(tlv_chan_list->header) +
            (sizeof(ChanScanParamSet_t) * bss->param.bss_config.num_of_chan);
        tlv += sizeof(tlv_chan_list->header) +
            (sizeof(ChanScanParamSet_t) * bss->param.bss_config.num_of_chan);
    }

#ifndef CONFIG_MLAN_WMSDK
    if ((bss->param.bss_config.auth_mode <= MLAN_AUTH_MODE_SHARED) ||
        (bss->param.bss_config.auth_mode == MLAN_AUTH_MODE_AUTO)) {
        tlv_auth_type = (MrvlIEtypes_auth_type_t *) tlv;
        tlv_auth_type->header.type = wlan_cpu_to_le16(TLV_TYPE_AUTH_TYPE);
        tlv_auth_type->header.len = wlan_cpu_to_le16(sizeof(t_u8));
        tlv_auth_type->auth_type = (t_u8) bss->param.bss_config.auth_mode;
        cmd_size += sizeof(MrvlIEtypes_auth_type_t);
        tlv += sizeof(MrvlIEtypes_auth_type_t);
    }
#endif /* CONFIG_MLAN_WMSDK */

    if (bss->param.bss_config.protocol) {
        tlv_encrypt_protocol = (MrvlIEtypes_encrypt_protocol_t *) tlv;
        tlv_encrypt_protocol->header.type =
            wlan_cpu_to_le16(TLV_TYPE_UAP_ENCRYPT_PROTOCOL);
        tlv_encrypt_protocol->header.len = wlan_cpu_to_le16(sizeof(t_u16));
        tlv_encrypt_protocol->protocol =
            wlan_cpu_to_le16(bss->param.bss_config.protocol);
        cmd_size += sizeof(MrvlIEtypes_encrypt_protocol_t);
        tlv += sizeof(MrvlIEtypes_encrypt_protocol_t);
    }

    if ((bss->param.bss_config.protocol & PROTOCOL_WPA) ||
        (bss->param.bss_config.protocol & PROTOCOL_WPA2) ||
        (bss->param.bss_config.protocol & PROTOCOL_WPA3_SAE) ||
        (bss->param.bss_config.protocol & PROTOCOL_OWE) ||
        (bss->param.bss_config.protocol & PROTOCOL_EAP)) {
        tlv_akmp = (MrvlIEtypes_akmp_t *) tlv;
        tlv_akmp->header.type = wlan_cpu_to_le16(TLV_TYPE_UAP_AKMP);
        tlv_akmp->key_mgmt = wlan_cpu_to_le16(bss->param.bss_config.key_mgmt);
        tlv_akmp->header.len = sizeof(t_u16);
        tlv_akmp->key_mgmt_operation =
            wlan_cpu_to_le16(bss->param.bss_config.key_mgmt_operation);
        tlv_akmp->header.len += sizeof(t_u16);
        tlv_akmp->header.len = wlan_cpu_to_le16(tlv_akmp->header.len);
        cmd_size += sizeof(MrvlIEtypes_akmp_t);
        tlv += sizeof(MrvlIEtypes_akmp_t);

        if (bss->param.bss_config.wpa_cfg.
            pairwise_cipher_wpa & VALID_CIPHER_BITMAP) {
            tlv_pwk_cipher = (MrvlIEtypes_pwk_cipher_t *) tlv;
            tlv_pwk_cipher->header.type = wlan_cpu_to_le16(TLV_TYPE_PWK_CIPHER);
            tlv_pwk_cipher->header.len =
                wlan_cpu_to_le16(sizeof(t_u16) + sizeof(t_u8) + sizeof(t_u8));
            tlv_pwk_cipher->protocol = wlan_cpu_to_le16(PROTOCOL_WPA);
            tlv_pwk_cipher->pairwise_cipher =
                bss->param.bss_config.wpa_cfg.pairwise_cipher_wpa;
            cmd_size += sizeof(MrvlIEtypes_pwk_cipher_t);
            tlv += sizeof(MrvlIEtypes_pwk_cipher_t);
        }

        if (bss->param.bss_config.wpa_cfg.
            pairwise_cipher_wpa2 & VALID_CIPHER_BITMAP) {
            tlv_pwk_cipher = (MrvlIEtypes_pwk_cipher_t *) tlv;
            tlv_pwk_cipher->header.type = wlan_cpu_to_le16(TLV_TYPE_PWK_CIPHER);
            tlv_pwk_cipher->header.len =
                wlan_cpu_to_le16(sizeof(t_u16) + sizeof(t_u8) + sizeof(t_u8));
            tlv_pwk_cipher->protocol = wlan_cpu_to_le16(PROTOCOL_WPA2);
            tlv_pwk_cipher->pairwise_cipher =
                bss->param.bss_config.wpa_cfg.pairwise_cipher_wpa2;
            cmd_size += sizeof(MrvlIEtypes_pwk_cipher_t);
            tlv += sizeof(MrvlIEtypes_pwk_cipher_t);
        }

        if (bss->param.bss_config.wpa_cfg.group_cipher & VALID_CIPHER_BITMAP) {
            tlv_gwk_cipher = (MrvlIEtypes_gwk_cipher_t *) tlv;
            tlv_gwk_cipher->header.type = wlan_cpu_to_le16(TLV_TYPE_GWK_CIPHER);
            tlv_gwk_cipher->header.len =
                wlan_cpu_to_le16(sizeof(t_u8) + sizeof(t_u8));
            tlv_gwk_cipher->group_cipher =
                bss->param.bss_config.wpa_cfg.group_cipher;
            cmd_size += sizeof(MrvlIEtypes_gwk_cipher_t);
            tlv += sizeof(MrvlIEtypes_gwk_cipher_t);
        }

#ifndef CONFIG_MLAN_WMSDK
        if (bss->param.bss_config.wpa_cfg.rsn_protection <= MTRUE) {
            tlv_rsn_prot = (MrvlIEtypes_rsn_replay_prot_t *) tlv;
            tlv_rsn_prot->header.type =
                wlan_cpu_to_le16(TLV_TYPE_UAP_RSN_REPLAY_PROTECT);
            tlv_rsn_prot->header.len = wlan_cpu_to_le16(sizeof(t_u8));
            tlv_rsn_prot->rsn_replay_prot =
                bss->param.bss_config.wpa_cfg.rsn_protection;
            cmd_size += sizeof(MrvlIEtypes_rsn_replay_prot_t);
            tlv += sizeof(MrvlIEtypes_rsn_replay_prot_t);
        }
#endif /* CONFIG_MLAN_WMSDK */

        if (bss->param.bss_config.wpa_cfg.length) {
            tlv_passphrase = (MrvlIEtypes_passphrase_t *) tlv;
            tlv_passphrase->header.type =
                wlan_cpu_to_le16(TLV_TYPE_UAP_WPA_PASSPHRASE);
            tlv_passphrase->header.len =
                (t_u16) wlan_cpu_to_le16(bss->param.bss_config.wpa_cfg.length);
            memcpy(pmpriv->adapter, tlv_passphrase->passphrase,
                   bss->param.bss_config.wpa_cfg.passphrase,
                   bss->param.bss_config.wpa_cfg.length);
            cmd_size +=
                sizeof(MrvlIEtypesHeader_t) +
                bss->param.bss_config.wpa_cfg.length;
            tlv +=
                sizeof(MrvlIEtypesHeader_t) +
                bss->param.bss_config.wpa_cfg.length;
        }

#ifndef CONFIG_MLAN_WMSDK
        if (bss->param.bss_config.wpa_cfg.gk_rekey_time < MAX_GRP_TIMER) {
            tlv_rekey_time = (MrvlIEtypes_group_rekey_time_t *) tlv;
            tlv_rekey_time->header.type =
                wlan_cpu_to_le16(TLV_TYPE_UAP_GRP_REKEY_TIME);
            tlv_rekey_time->header.len = wlan_cpu_to_le16(sizeof(t_u32));
            tlv_rekey_time->gk_rekey_time =
                wlan_cpu_to_le32(bss->param.bss_config.wpa_cfg.gk_rekey_time);
            cmd_size += sizeof(MrvlIEtypes_group_rekey_time_t);
            tlv += sizeof(MrvlIEtypes_group_rekey_time_t);
        }
#endif /* CONFIG_MLAN_WMSDK */
    } else {
#ifndef CONFIG_MLAN_WMSDK
        if ((bss->param.bss_config.wep_cfg.key0.length) &&
            ((bss->param.bss_config.wep_cfg.key0.length == 5) ||
             (bss->param.bss_config.wep_cfg.key0.length == 10) ||
             (bss->param.bss_config.wep_cfg.key0.length == 13) ||
             (bss->param.bss_config.wep_cfg.key0.length == 26))) {
            tlv_wep_key = (MrvlIEtypes_wep_key_t *) tlv;
            tlv_wep_key->header.type = wlan_cpu_to_le16(TLV_TYPE_UAP_WEP_KEY);
            tlv_wep_key->header.len =
                wlan_cpu_to_le16(2 + bss->param.bss_config.wep_cfg.key0.length);
            tlv_wep_key->key_index =
                bss->param.bss_config.wep_cfg.key0.key_index;
            tlv_wep_key->is_default =
                bss->param.bss_config.wep_cfg.key0.is_default;
            memcpy(pmpriv->adapter, tlv_wep_key->key,
                   bss->param.bss_config.wep_cfg.key0.key,
                   bss->param.bss_config.wep_cfg.key0.length);
            cmd_size +=
                sizeof(MrvlIEtypesHeader_t) + 2 +
                bss->param.bss_config.wep_cfg.key0.length;
            tlv +=
                sizeof(MrvlIEtypesHeader_t) + 2 +
                bss->param.bss_config.wep_cfg.key0.length;
        }

        if ((bss->param.bss_config.wep_cfg.key1.length) &&
            ((bss->param.bss_config.wep_cfg.key1.length == 5) ||
             (bss->param.bss_config.wep_cfg.key1.length == 10) ||
             (bss->param.bss_config.wep_cfg.key1.length == 13) ||
             (bss->param.bss_config.wep_cfg.key1.length == 26))) {
            tlv_wep_key = (MrvlIEtypes_wep_key_t *) tlv;
            tlv_wep_key->header.type = wlan_cpu_to_le16(TLV_TYPE_UAP_WEP_KEY);
            tlv_wep_key->header.len =
                wlan_cpu_to_le16(2 + bss->param.bss_config.wep_cfg.key1.length);
            tlv_wep_key->key_index =
                bss->param.bss_config.wep_cfg.key1.key_index;
            tlv_wep_key->is_default =
                bss->param.bss_config.wep_cfg.key1.is_default;
            memcpy(pmpriv->adapter, tlv_wep_key->key,
                   bss->param.bss_config.wep_cfg.key1.key,
                   bss->param.bss_config.wep_cfg.key1.length);
            cmd_size +=
                sizeof(MrvlIEtypesHeader_t) + 2 +
                bss->param.bss_config.wep_cfg.key1.length;
            tlv +=
                sizeof(MrvlIEtypesHeader_t) + 2 +
                bss->param.bss_config.wep_cfg.key1.length;
        }

        if ((bss->param.bss_config.wep_cfg.key2.length) &&
            ((bss->param.bss_config.wep_cfg.key2.length == 5) ||
             (bss->param.bss_config.wep_cfg.key2.length == 10) ||
             (bss->param.bss_config.wep_cfg.key2.length == 13) ||
             (bss->param.bss_config.wep_cfg.key2.length == 26))) {
            tlv_wep_key = (MrvlIEtypes_wep_key_t *) tlv;
            tlv_wep_key->header.type = wlan_cpu_to_le16(TLV_TYPE_UAP_WEP_KEY);
            tlv_wep_key->header.len =
                wlan_cpu_to_le16(2 + bss->param.bss_config.wep_cfg.key2.length);
            tlv_wep_key->key_index =
                bss->param.bss_config.wep_cfg.key2.key_index;
            tlv_wep_key->is_default =
                bss->param.bss_config.wep_cfg.key2.is_default;
            memcpy(pmpriv->adapter, tlv_wep_key->key,
                   bss->param.bss_config.wep_cfg.key2.key,
                   bss->param.bss_config.wep_cfg.key2.length);
            cmd_size +=
                sizeof(MrvlIEtypesHeader_t) + 2 +
                bss->param.bss_config.wep_cfg.key2.length;
            tlv +=
                sizeof(MrvlIEtypesHeader_t) + 2 +
                bss->param.bss_config.wep_cfg.key2.length;
        }

        if ((bss->param.bss_config.wep_cfg.key3.length) &&
            ((bss->param.bss_config.wep_cfg.key3.length == 5) ||
             (bss->param.bss_config.wep_cfg.key3.length == 10) ||
             (bss->param.bss_config.wep_cfg.key3.length == 13) ||
             (bss->param.bss_config.wep_cfg.key3.length == 26))) {
            tlv_wep_key = (MrvlIEtypes_wep_key_t *) tlv;
            tlv_wep_key->header.type = wlan_cpu_to_le16(TLV_TYPE_UAP_WEP_KEY);
            tlv_wep_key->header.len =
                wlan_cpu_to_le16(2 + bss->param.bss_config.wep_cfg.key3.length);
            tlv_wep_key->key_index =
                bss->param.bss_config.wep_cfg.key3.key_index;
            tlv_wep_key->is_default =
                bss->param.bss_config.wep_cfg.key3.is_default;
            memcpy(pmpriv->adapter, tlv_wep_key->key,
                   bss->param.bss_config.wep_cfg.key3.key,
                   bss->param.bss_config.wep_cfg.key3.length);
            cmd_size +=
                sizeof(MrvlIEtypesHeader_t) + 2 +
                bss->param.bss_config.wep_cfg.key3.length;
            tlv +=
                sizeof(MrvlIEtypesHeader_t) + 2 +
                bss->param.bss_config.wep_cfg.key3.length;
        }
#endif /* CONFIG_MLAN_WMSDK */
    }

#ifndef UAP_DISABLE_11N
    if ((bss->param.bss_config.ht_cap_info)
        ) {
	/* wmsdk: All the values received will be zero by default. */
        tlv_htcap = (MrvlIETypes_HTCap_t *) tlv;
        tlv_htcap->header.type = wlan_cpu_to_le16(HT_CAPABILITY);
        tlv_htcap->header.len = wlan_cpu_to_le16(sizeof(HTCap_t));
        tlv_htcap->ht_cap.ht_cap_info =
            wlan_cpu_to_le16(bss->param.bss_config.ht_cap_info);
        tlv_htcap->ht_cap.ampdu_param = bss->param.bss_config.ampdu_param;
        memcpy(pmpriv->adapter, tlv_htcap->ht_cap.supported_mcs_set,
               bss->param.bss_config.supported_mcs_set, 16);
        tlv_htcap->ht_cap.ht_ext_cap =
            wlan_cpu_to_le16(bss->param.bss_config.ht_ext_cap);
        tlv_htcap->ht_cap.tx_bf_cap =
            wlan_cpu_to_le32(bss->param.bss_config.tx_bf_cap);
        tlv_htcap->ht_cap.asel = bss->param.bss_config.asel;
        cmd_size += sizeof(MrvlIETypes_HTCap_t);
        tlv += sizeof(MrvlIETypes_HTCap_t);
    }
#else /* UAP_DISABLE_11N */
    /* Disable 11n*/
    /* Some of the 11n clients fail to associate with uAP since
       we dont have 11n support in driver hence disabling 11n in
       firmware */
    tlv_cipher = (MrvlIEtypes_Cipher_t *) tlv;
    tlv_cipher->header.type = 0x2d;
    tlv_cipher->header.len = 0x1a;
    memset(pmadapter, &tlv_cipher->pair_cipher, 0, 0x1a);
    cmd_size += 0x1a + 0x4;
    tlv += 0x1a + 0x4;
#endif /* UAP_DISABLE_11N */

#ifndef CONFIG_MLAN_WMSDK
    if (bss->param.bss_config.mgmt_ie_passthru_mask < (MAX_VALID_DWORD)) {
        tlv_mgmt_ie_passthru = (MrvlIEtypes_mgmt_ie_passthru_t *) tlv;
        tlv_mgmt_ie_passthru->header.type =
            wlan_cpu_to_le16(TLV_TYPE_UAP_MGMT_IE_PASSTHRU_MASK);
        tlv_mgmt_ie_passthru->header.len = wlan_cpu_to_le16(sizeof(t_u32));
        /* keep copy in private data */
        pmpriv->mgmt_frame_passthru_mask =
            bss->param.bss_config.mgmt_ie_passthru_mask;
        tlv_mgmt_ie_passthru->mgmt_ie_mask =
            wlan_cpu_to_le32(bss->param.bss_config.mgmt_ie_passthru_mask);
        cmd_size += sizeof(MrvlIEtypes_mgmt_ie_passthru_t);
        tlv += sizeof(MrvlIEtypes_mgmt_ie_passthru_t);
    }
    if (((bss->param.bss_config.enable_2040coex == 0) ||
         (bss->param.bss_config.enable_2040coex == 1))
        ) {
        tlv_2040_coex_enable = (MrvlIEtypes_2040_coex_enable_t *) tlv;
        tlv_2040_coex_enable->header.type =
            wlan_cpu_to_le16(TLV_TYPE_2040_BSS_COEX_CONTROL);
        tlv_2040_coex_enable->header.len = wlan_cpu_to_le16(sizeof(t_u8));
        tlv_2040_coex_enable->enable_2040coex =
            bss->param.bss_config.enable_2040coex;
        cmd_size += sizeof(MrvlIEtypes_2040_coex_enable_t);
        tlv += sizeof(MrvlIEtypes_2040_coex_enable_t);
    }
    if (bss->param.bss_config.wmm_para.qos_info == 0x80 ||
        bss->param.bss_config.wmm_para.qos_info == 0x00) {
        tlv_wmm_parameter = (MrvlIEtypes_wmm_parameter_t *) tlv;
        tlv_wmm_parameter->header.type =
            wlan_cpu_to_le16(TLV_TYPE_VENDOR_SPECIFIC_IE);
        tlv_wmm_parameter->header.len =
            wlan_cpu_to_le16(sizeof(bss->param.bss_config.wmm_para));
        memcpy(pmpriv->adapter, tlv_wmm_parameter->wmm_para.ouitype,
               bss->param.bss_config.wmm_para.ouitype,
               sizeof(tlv_wmm_parameter->wmm_para.ouitype));
        tlv_wmm_parameter->wmm_para.ouisubtype =
            bss->param.bss_config.wmm_para.ouisubtype;
        tlv_wmm_parameter->wmm_para.version =
            bss->param.bss_config.wmm_para.version;
        tlv_wmm_parameter->wmm_para.qos_info =
            bss->param.bss_config.wmm_para.qos_info;
        for (ac = 0; ac < 4; ac++) {
            tlv_wmm_parameter->wmm_para.ac_params[ac].aci_aifsn.aifsn =
                bss->param.bss_config.wmm_para.ac_params[ac].aci_aifsn.aifsn;
            tlv_wmm_parameter->wmm_para.ac_params[ac].ecw.ecw_max =
                bss->param.bss_config.wmm_para.ac_params[ac].ecw.ecw_max;
            tlv_wmm_parameter->wmm_para.ac_params[ac].ecw.ecw_min =
                bss->param.bss_config.wmm_para.ac_params[ac].ecw.ecw_min;
            tlv_wmm_parameter->wmm_para.ac_params[ac].tx_op_limit =
                wlan_cpu_to_le16(bss->param.bss_config.wmm_para.ac_params[ac].
                                 tx_op_limit);
        }
        cmd_size += sizeof(MrvlIEtypes_wmm_parameter_t);
        tlv += sizeof(MrvlIEtypes_wmm_parameter_t);
    }
#endif /* CONFIG_MLAN_WMSDK */

    cmd->size = (t_u16) wlan_cpu_to_le16(cmd_size);
    PRINTM(MCMND, "AP config: cmd_size=%d\n", cmd_size);
    LEAVE();
    return MLAN_STATUS_SUCCESS;
}

/**
 *  @brief This function prepares command of sys_config
 *
 *  @param pmpriv		A pointer to mlan_private structure
 *  @param cmd	   		A pointer to HostCmd_DS_COMMAND structure
 *  @param cmd_action   the action: GET or SET
 *  @param pioctl_buf   A pointer to mlan_ioctl_req structure
 *  @return         MLAN_STATUS_SUCCESS or MLAN_STATUS_FAILURE
 */
static mlan_status
wlan_uap_cmd_sys_configure(pmlan_private pmpriv,
                           IN HostCmd_DS_COMMAND * cmd,
                           IN t_u16 cmd_action,
                           IN pmlan_ioctl_req pioctl_buf, IN t_void * pdata_buf)
{
    mlan_ds_bss *bss = MNULL;
    HostCmd_DS_SYS_CONFIG *sys_config =
        (HostCmd_DS_SYS_CONFIG *) & cmd->params.sys_config;
    MrvlIEtypes_channel_band_t *chan_band_tlv = MNULL, *pdat_tlv_cb = MNULL;
    MrvlIEtypes_max_sta_count_t *max_sta_cnt_tlv = MNULL, *pdat_tlv_ccb = MNULL;
#ifndef CONFIG_MLAN_WMSDK
    MrvlIEtypes_MacAddr_t *mac_tlv = MNULL;
    MrvlIEtypes_beacon_period_t *bcn_pd_tlv = MNULL, *pdat_tlv_bcnpd = MNULL;
    MrvlIEtypes_dtim_period_t *dtim_pd_tlv = MNULL, *pdat_tlv_dtimpd = MNULL;
    mlan_ds_misc_custom_ie *cust_ie = MNULL;
    mlan_ds_misc_cfg *misc = MNULL;
    MrvlIEtypesHeader_t *ie_header =
        (MrvlIEtypesHeader_t *) sys_config->tlv_buffer;
    t_u8 *ie = (t_u8 *) sys_config->tlv_buffer + sizeof(MrvlIEtypesHeader_t);
    t_u16 req_len = 0, travel_len = 0;
    custom_ie *cptr = MNULL;
#endif /* CONFIG_MLAN_WMSDK */
    mlan_status ret = MLAN_STATUS_SUCCESS;

    ENTER();

    cmd->command = wlan_cpu_to_le16(HOST_CMD_APCMD_SYS_CONFIGURE);
    sys_config->action = wlan_cpu_to_le16(cmd_action);
    cmd->size = wlan_cpu_to_le16(sizeof(HostCmd_DS_SYS_CONFIG) - 1 + S_DS_GEN);
    if (pioctl_buf == MNULL) {
        if (pdata_buf) {
            switch (*(t_u16 *) pdata_buf) {
            case TLV_TYPE_UAP_CHAN_BAND_CONFIG:
                pdat_tlv_cb = (MrvlIEtypes_channel_band_t *) pdata_buf;
                chan_band_tlv =
                    (MrvlIEtypes_channel_band_t *) sys_config->tlv_buffer;
                cmd->size =
                    wlan_cpu_to_le16(sizeof(HostCmd_DS_SYS_CONFIG) - 1 +
                                     S_DS_GEN +
                                     sizeof(MrvlIEtypes_channel_band_t));
                chan_band_tlv->header.type =
                    wlan_cpu_to_le16(TLV_TYPE_UAP_CHAN_BAND_CONFIG);
                chan_band_tlv->header.len =
                    wlan_cpu_to_le16(sizeof(MrvlIEtypes_channel_band_t)
                                     - sizeof(MrvlIEtypesHeader_t));
                if (cmd_action) {
                    chan_band_tlv->band_config = pdat_tlv_cb->band_config;
                    chan_band_tlv->channel = pdat_tlv_cb->channel;
                }
                ret = MLAN_STATUS_SUCCESS;
                break;
	    case TLV_TYPE_UAP_MAX_STA_CNT:
                pdat_tlv_ccb = (MrvlIEtypes_max_sta_count_t *) pdata_buf;
                max_sta_cnt_tlv =
                    (MrvlIEtypes_max_sta_count_t *) sys_config->tlv_buffer;
                cmd->size =
                    wlan_cpu_to_le16(sizeof(HostCmd_DS_SYS_CONFIG) - 1 +
                                     S_DS_GEN +
                                     sizeof(MrvlIEtypes_max_sta_count_t));
                max_sta_cnt_tlv->header.type =
                    wlan_cpu_to_le16(TLV_TYPE_UAP_MAX_STA_CNT);

                if (cmd_action) {
		    max_sta_cnt_tlv->header.len =
                    wlan_cpu_to_le16(sizeof(MrvlIEtypes_max_sta_count_t)
                                     - sizeof(MrvlIEtypesHeader_t));
		    max_sta_cnt_tlv->max_sta_count = pdat_tlv_ccb->max_sta_count;
                } else {
			max_sta_cnt_tlv->header.len = 0;
			max_sta_cnt_tlv->max_sta_count = 0;
		}
                ret = MLAN_STATUS_SUCCESS;
                break;
#ifndef CONFIG_MLAN_WMSDK /* wmsdk: functionality not req. for now */
            case TLV_TYPE_UAP_BEACON_PERIOD:
                pdat_tlv_bcnpd = (MrvlIEtypes_beacon_period_t *) pdata_buf;
                bcn_pd_tlv =
                    (MrvlIEtypes_beacon_period_t *) sys_config->tlv_buffer;
                cmd->size =
                    sizeof(HostCmd_DS_SYS_CONFIG) - 1 + S_DS_GEN +
                    sizeof(MrvlIEtypes_beacon_period_t);
                bcn_pd_tlv->header.type =
                    wlan_cpu_to_le16(TLV_TYPE_UAP_BEACON_PERIOD);
                bcn_pd_tlv->header.len =
                    wlan_cpu_to_le16(sizeof(MrvlIEtypes_beacon_period_t)
                                     - sizeof(MrvlIEtypesHeader_t));
                if (cmd_action) {
                    bcn_pd_tlv->beacon_period =
                        wlan_cpu_to_le16(pdat_tlv_bcnpd->beacon_period);
                }
                /* Add TLV_UAP_DTIM_PERIOD if it follws in pdata_buf */
                pdat_tlv_dtimpd =
                    (MrvlIEtypes_dtim_period_t *) (((t_u8 *) pdata_buf)
                                                   +
                                                   sizeof
                                                   (MrvlIEtypes_beacon_period_t));
                if (TLV_TYPE_UAP_DTIM_PERIOD == pdat_tlv_dtimpd->header.type) {
                    dtim_pd_tlv =
                        (MrvlIEtypes_dtim_period_t *) (sys_config->tlv_buffer +
                                                       sizeof
                                                       (MrvlIEtypes_beacon_period_t));
                    cmd->size += sizeof(MrvlIEtypes_dtim_period_t);
                    dtim_pd_tlv->header.type =
                        wlan_cpu_to_le16(TLV_TYPE_UAP_DTIM_PERIOD);
                    dtim_pd_tlv->header.len =
                        wlan_cpu_to_le16(sizeof(MrvlIEtypes_dtim_period_t)
                                         - sizeof(MrvlIEtypesHeader_t));
                    if (cmd_action) {
                        dtim_pd_tlv->dtim_period = pdat_tlv_dtimpd->dtim_period;
                    }
                }
                /* Finalize cmd size */
                cmd->size = wlan_cpu_to_le16(cmd->size);
                ret = MLAN_STATUS_SUCCESS;
                break;
            case TLV_TYPE_MGMT_IE:
                cust_ie = (mlan_ds_misc_custom_ie *) pdata_buf;
                cmd->size =
                    wlan_cpu_to_le16(sizeof(HostCmd_DS_SYS_CONFIG) - 1 +
                                     S_DS_GEN + sizeof(MrvlIEtypesHeader_t) +
                                     cust_ie->len);
                ie_header->type = wlan_cpu_to_le16(TLV_TYPE_MGMT_IE);
                ie_header->len = wlan_cpu_to_le16(cust_ie->len);

                if (ie && cust_ie->ie_data_list) {
                    req_len = cust_ie->len;
                    travel_len = 0;
                    /* conversion for index, mask, len */
                    if (req_len == sizeof(t_u16))
                        cust_ie->ie_data_list[0].ie_index =
                            wlan_cpu_to_le16(cust_ie->ie_data_list[0].ie_index);
                    while (req_len > sizeof(t_u16)) {
                        cptr =
                            (custom_ie *) (((t_u8 *) & cust_ie->ie_data_list) +
                                           travel_len);
                        travel_len +=
                            cptr->ie_length + sizeof(custom_ie) - MAX_IE_SIZE;
                        req_len -=
                            cptr->ie_length + sizeof(custom_ie) - MAX_IE_SIZE;
                        cptr->ie_index = wlan_cpu_to_le16(cptr->ie_index);
                        cptr->mgmt_subtype_mask =
                            wlan_cpu_to_le16(cptr->mgmt_subtype_mask);
                        cptr->ie_length = wlan_cpu_to_le16(cptr->ie_length);
                    }
                    memcpy(pmpriv->adapter, ie, cust_ie->ie_data_list,
                           cust_ie->len);
                }
                break;
#endif /* CONFIG_MLAN_WMSDK */
            default:
                PRINTM(MERROR,
                       "Wrong data, or missing TLV_TYPE 0x%04x handler.\n",
                       *(t_u16 *) pdata_buf);
                break;
            }
            goto done;
        } else {
#ifndef CONFIG_MLAN_WMSDK
            mac_tlv = (MrvlIEtypes_MacAddr_t *) sys_config->tlv_buffer;
            cmd->size =
                wlan_cpu_to_le16(sizeof(HostCmd_DS_SYS_CONFIG) - 1 + S_DS_GEN +
                                 sizeof(MrvlIEtypes_MacAddr_t));
            mac_tlv->header.type = wlan_cpu_to_le16(TLV_TYPE_UAP_MAC_ADDRESS);
            mac_tlv->header.len = wlan_cpu_to_le16(MLAN_MAC_ADDR_LENGTH);
            ret = MLAN_STATUS_SUCCESS;
            goto done;
#endif /* CONFIG_MLAN_WMSDK */
        }
    }

    if (pioctl_buf->req_id == MLAN_IOCTL_BSS) {
        bss = (mlan_ds_bss *) pioctl_buf->pbuf;
        if (bss->sub_command == MLAN_OID_BSS_MAC_ADDR) {
#ifndef CONFIG_MLAN_WMSDK
            mac_tlv = (MrvlIEtypes_MacAddr_t *) sys_config->tlv_buffer;
            cmd->size =
                wlan_cpu_to_le16(sizeof(HostCmd_DS_SYS_CONFIG) - 1 + S_DS_GEN +
                                 sizeof(MrvlIEtypes_MacAddr_t));
            mac_tlv->header.type = wlan_cpu_to_le16(TLV_TYPE_UAP_MAC_ADDRESS);
            mac_tlv->header.len = wlan_cpu_to_le16(MLAN_MAC_ADDR_LENGTH);
            if (cmd_action == HostCmd_ACT_GEN_SET)
                memcpy(pmpriv->adapter, mac_tlv->mac, &bss->param.mac_addr,
                       MLAN_MAC_ADDR_LENGTH);
#endif /* CONFIG_MLAN_WMSDK */
        } else if ((bss->sub_command == MLAN_OID_UAP_BSS_CONFIG) &&
                   (cmd_action == HostCmd_ACT_GEN_SET)) {
            ret = wlan_uap_cmd_ap_config(pmpriv, cmd, cmd_action, pioctl_buf);
            goto done;
        }
    } else if (pioctl_buf->req_id == MLAN_IOCTL_MISC_CFG) {
#ifndef CONFIG_MLAN_WMSDK
        misc = (mlan_ds_misc_cfg *) pioctl_buf->pbuf;
        if ((misc->sub_command == MLAN_OID_MISC_GEN_IE) &&
            (misc->param.gen_ie.type == MLAN_IE_TYPE_GEN_IE)
            ) {
            cmd->size =
                wlan_cpu_to_le16(sizeof(HostCmd_DS_SYS_CONFIG) - 1 + S_DS_GEN +
                                 sizeof(MrvlIEtypesHeader_t) +
                                 misc->param.gen_ie.len);
            ie_header->type = wlan_cpu_to_le16(TLV_TYPE_WAPI_IE);
            ie_header->len = wlan_cpu_to_le16(misc->param.gen_ie.len);
            if (cmd_action == HostCmd_ACT_GEN_SET)
                memcpy(pmpriv->adapter, ie, misc->param.gen_ie.ie_data,
                       misc->param.gen_ie.len);
        }
        if ((misc->sub_command == MLAN_OID_MISC_CUSTOM_IE) &&
            (misc->param.cust_ie.type == TLV_TYPE_MGMT_IE)) {
            cmd->size =
                wlan_cpu_to_le16(sizeof(HostCmd_DS_SYS_CONFIG) - 1 + S_DS_GEN +
                                 sizeof(MrvlIEtypesHeader_t) +
                                 misc->param.cust_ie.len);
            ie_header->type = wlan_cpu_to_le16(TLV_TYPE_MGMT_IE);
            ie_header->len = wlan_cpu_to_le16(misc->param.cust_ie.len);

            if (ie && misc->param.cust_ie.ie_data_list) {
                req_len = misc->param.cust_ie.len;
                travel_len = 0;
                /* conversion for index, mask, len */
                if (req_len == sizeof(t_u16))
                    misc->param.cust_ie.ie_data_list[0].ie_index =
                        wlan_cpu_to_le16(misc->param.cust_ie.ie_data_list[0].
                                         ie_index);
                while (req_len > sizeof(t_u16)) {
                    cptr =
                        (custom_ie
                         *) (((t_u8 *) & misc->param.cust_ie.ie_data_list) +
                             travel_len);
                    travel_len +=
                        cptr->ie_length + sizeof(custom_ie) - MAX_IE_SIZE;
                    req_len -=
                        cptr->ie_length + sizeof(custom_ie) - MAX_IE_SIZE;
                    cptr->ie_index = wlan_cpu_to_le16(cptr->ie_index);
                    cptr->mgmt_subtype_mask =
                        wlan_cpu_to_le16(cptr->mgmt_subtype_mask);
                    cptr->ie_length = wlan_cpu_to_le16(cptr->ie_length);
                }
                if (misc->param.cust_ie.len)
                    memcpy(pmpriv->adapter, ie,
                           misc->param.cust_ie.ie_data_list,
                           misc->param.cust_ie.len);
            }
        }
#endif /* CONFIG_MLAN_WMSDK */
    }
  done:
    LEAVE();
    return ret;
}

#ifndef CONFIG_MLAN_WMSDK
/**
 *  @brief This function handles command resp for get uap settings
 *
 *  @param pmpriv       A pointer to mlan_private structure
 *  @param resp         A pointer to HostCmd_DS_COMMAND
 *  @param pioctl_buf   A pointer to mlan_ioctl_req structure
 *
 *  @return             MLAN_STATUS_SUCCESS
 */
static mlan_status
wlan_uap_ret_cmd_ap_config(IN pmlan_private pmpriv,
                           IN HostCmd_DS_COMMAND * resp,
                           IN mlan_ioctl_req * pioctl_buf)
{
    HostCmd_DS_SYS_CONFIG *sys_config =
        (HostCmd_DS_SYS_CONFIG *) & resp->params.sys_config;
    mlan_ds_bss *bss = MNULL;
    MrvlIEtypesHeader_t *tlv = MNULL;
    t_u16 tlv_buf_left = 0;
    t_u16 tlv_type = 0;
    t_u16 tlv_len = 0;
    MrvlIEtypes_MacAddr_t *tlv_mac = MNULL;
    MrvlIEtypes_SsIdParamSet_t *tlv_ssid = MNULL;
    MrvlIEtypes_beacon_period_t *tlv_beacon_period = MNULL;
    MrvlIEtypes_dtim_period_t *tlv_dtim_period = MNULL;
    MrvlIEtypes_RatesParamSet_t *tlv_rates = MNULL;
    MrvlIEtypes_tx_rate_t *tlv_txrate = MNULL;
    MrvlIEtypes_mcbc_rate_t *tlv_mcbc_rate = MNULL;
    MrvlIEtypes_tx_power_t *tlv_tx_power = MNULL;
    MrvlIEtypes_bcast_ssid_t *tlv_bcast_ssid = MNULL;
    MrvlIEtypes_antenna_mode_t *tlv_antenna = MNULL;
    MrvlIEtypes_pkt_forward_t *tlv_pkt_forward = MNULL;
    MrvlIEtypes_max_sta_count_t *tlv_sta_count = MNULL;
    MrvlIEtypes_sta_ageout_t *tlv_sta_ageout = MNULL;
    MrvlIEtypes_ps_sta_ageout_t *tlv_ps_sta_ageout = MNULL;
    MrvlIEtypes_rts_threshold_t *tlv_rts_threshold = MNULL;
    MrvlIEtypes_frag_threshold_t *tlv_frag_threshold = MNULL;
    MrvlIEtypes_retry_limit_t *tlv_retry_limit = MNULL;
    MrvlIEtypes_eapol_pwk_hsk_timeout_t *tlv_pairwise_timeout = MNULL;
    MrvlIEtypes_eapol_pwk_hsk_retries_t *tlv_pairwise_retries = MNULL;
    MrvlIEtypes_eapol_gwk_hsk_timeout_t *tlv_groupwise_timeout = MNULL;
    MrvlIEtypes_eapol_gwk_hsk_retries_t *tlv_groupwise_retries = MNULL;
    MrvlIEtypes_mgmt_ie_passthru_t *tlv_mgmt_ie_passthru = MNULL;
    MrvlIEtypes_2040_coex_enable_t *tlv_2040_coex_enable = MNULL;
    MrvlIEtypes_mac_filter_t *tlv_mac_filter = MNULL;
    MrvlIEtypes_channel_band_t *tlv_chan_band = MNULL;
    MrvlIEtypes_ChanListParamSet_t *tlv_chan_list = MNULL;
    ChanScanParamSet_t *pscan_chan = MNULL;
    MrvlIEtypes_auth_type_t *tlv_auth_type = MNULL;
    MrvlIEtypes_encrypt_protocol_t *tlv_encrypt_protocol = MNULL;
    MrvlIEtypes_akmp_t *tlv_akmp = MNULL;
    MrvlIEtypes_pwk_cipher_t *tlv_pwk_cipher = MNULL;
    MrvlIEtypes_gwk_cipher_t *tlv_gwk_cipher = MNULL;
    MrvlIEtypes_rsn_replay_prot_t *tlv_rsn_prot = MNULL;
    MrvlIEtypes_passphrase_t *tlv_passphrase = MNULL;
#ifdef WIFI_DIRECT_SUPPORT
    MrvlIEtypes_psk_t *tlv_psk = MNULL;
#endif /* WIFI_DIRECT_SUPPORT */
    MrvlIEtypes_group_rekey_time_t *tlv_rekey_time = MNULL;
    MrvlIEtypes_wep_key_t *tlv_wep_key = MNULL;
    MrvlIEtypes_preamble_t *tlv_preamble = MNULL;
    MrvlIEtypes_bss_status_t *tlv_bss_status = MNULL;
    MrvlIETypes_HTCap_t *tlv_htcap = MNULL;
    MrvlIEtypes_wmm_parameter_t *tlv_wmm_parameter = MNULL;

    wep_key *pkey = MNULL;
    t_u16 i;
    t_u16 ac;

    ENTER();

    bss = (mlan_ds_bss *) pioctl_buf->pbuf;
    tlv = (MrvlIEtypesHeader_t *) sys_config->tlv_buffer;
    tlv_buf_left = resp->size - (sizeof(HostCmd_DS_SYS_CONFIG) - 1 + S_DS_GEN);

    while (tlv_buf_left >= sizeof(MrvlIEtypesHeader_t)) {

        tlv_type = wlan_le16_to_cpu(tlv->type);
        tlv_len = wlan_le16_to_cpu(tlv->len);

        if (tlv_buf_left < (tlv_len + sizeof(MrvlIEtypesHeader_t))) {
            PRINTM(MERROR,
                   "Error processing uAP sys config TLVs, bytes left < TLV length\n");
            break;
        }

        switch (tlv_type) {
        case TLV_TYPE_UAP_MAC_ADDRESS:
            tlv_mac = (MrvlIEtypes_MacAddr_t *) tlv;
            memcpy(pmpriv->adapter, &bss->param.bss_config.mac_addr,
                   tlv_mac->mac, MLAN_MAC_ADDR_LENGTH);
            break;
        case TLV_TYPE_SSID:
            tlv_ssid = (MrvlIEtypes_SsIdParamSet_t *) tlv;
            bss->param.bss_config.ssid.ssid_len =
                MIN(MLAN_MAX_SSID_LENGTH, tlv_len);
            memcpy(pmpriv->adapter, bss->param.bss_config.ssid.ssid,
                   tlv_ssid->ssid, MIN(MLAN_MAX_SSID_LENGTH, tlv_len));
            break;
        case TLV_TYPE_UAP_BEACON_PERIOD:
            tlv_beacon_period = (MrvlIEtypes_beacon_period_t *) tlv;
            bss->param.bss_config.beacon_period =
                wlan_le16_to_cpu(tlv_beacon_period->beacon_period);
            pmpriv->uap_state_chan_cb.beacon_period =
                wlan_le16_to_cpu(tlv_beacon_period->beacon_period);
            break;
        case TLV_TYPE_UAP_DTIM_PERIOD:
            tlv_dtim_period = (MrvlIEtypes_dtim_period_t *) tlv;
            bss->param.bss_config.dtim_period = tlv_dtim_period->dtim_period;
            pmpriv->uap_state_chan_cb.dtim_period =
                tlv_dtim_period->dtim_period;
            break;
        case TLV_TYPE_RATES:
            tlv_rates = (MrvlIEtypes_RatesParamSet_t *) tlv;
            memcpy(pmpriv->adapter, bss->param.bss_config.rates,
                   tlv_rates->rates, MIN(MAX_DATA_RATES, tlv_len));
            break;
        case TLV_TYPE_UAP_TX_DATA_RATE:
            tlv_txrate = (MrvlIEtypes_tx_rate_t *) tlv;
            bss->param.bss_config.tx_data_rate =
                wlan_le16_to_cpu(tlv_txrate->tx_data_rate);
            break;
        case TLV_TYPE_UAP_MCBC_DATA_RATE:
            tlv_mcbc_rate = (MrvlIEtypes_mcbc_rate_t *) tlv;
            bss->param.bss_config.mcbc_data_rate =
                wlan_le16_to_cpu(tlv_mcbc_rate->mcbc_data_rate);
            break;
        case TLV_TYPE_UAP_TX_POWER:
            tlv_tx_power = (MrvlIEtypes_tx_power_t *) tlv;
            bss->param.bss_config.tx_power_level = tlv_tx_power->tx_power;
            break;
        case TLV_TYPE_UAP_BCAST_SSID_CTL:
            tlv_bcast_ssid = (MrvlIEtypes_bcast_ssid_t *) tlv;
            bss->param.bss_config.bcast_ssid_ctl =
                tlv_bcast_ssid->bcast_ssid_ctl;
            break;
        case TLV_TYPE_UAP_ANTENNA_CTL:
            tlv_antenna = (MrvlIEtypes_antenna_mode_t *) tlv;
            if (tlv_antenna->which_antenna == TX_ANTENNA)
                bss->param.bss_config.tx_antenna = tlv_antenna->antenna_mode;
            else if (tlv_antenna->which_antenna == RX_ANTENNA)
                bss->param.bss_config.rx_antenna = tlv_antenna->antenna_mode;
            break;
        case TLV_TYPE_UAP_PKT_FWD_CTL:
            tlv_pkt_forward = (MrvlIEtypes_pkt_forward_t *) tlv;
            bss->param.bss_config.pkt_forward_ctl =
                tlv_pkt_forward->pkt_forward_ctl;
            break;
        case TLV_TYPE_UAP_MAX_STA_CNT:
            tlv_sta_count = (MrvlIEtypes_max_sta_count_t *) tlv;
            bss->param.bss_config.max_sta_count =
                wlan_le16_to_cpu(tlv_sta_count->max_sta_count);
            break;
        case TLV_TYPE_UAP_STA_AGEOUT_TIMER:
            tlv_sta_ageout = (MrvlIEtypes_sta_ageout_t *) tlv;
            bss->param.bss_config.sta_ageout_timer =
                wlan_le32_to_cpu(tlv_sta_ageout->sta_ageout_timer);
            break;
        case TLV_TYPE_UAP_PS_STA_AGEOUT_TIMER:
            tlv_ps_sta_ageout = (MrvlIEtypes_ps_sta_ageout_t *) tlv;
            bss->param.bss_config.ps_sta_ageout_timer =
                wlan_le32_to_cpu(tlv_ps_sta_ageout->ps_sta_ageout_timer);
            break;
        case TLV_TYPE_UAP_RTS_THRESHOLD:
            tlv_rts_threshold = (MrvlIEtypes_rts_threshold_t *) tlv;
            bss->param.bss_config.rts_threshold =
                wlan_le16_to_cpu(tlv_rts_threshold->rts_threshold);
            break;
        case TLV_TYPE_UAP_FRAG_THRESHOLD:
            tlv_frag_threshold = (MrvlIEtypes_frag_threshold_t *) tlv;
            bss->param.bss_config.frag_threshold =
                wlan_le16_to_cpu(tlv_frag_threshold->frag_threshold);
            break;
        case TLV_TYPE_UAP_RETRY_LIMIT:
            tlv_retry_limit = (MrvlIEtypes_retry_limit_t *) tlv;
            bss->param.bss_config.retry_limit = tlv_retry_limit->retry_limit;
            break;
        case TLV_TYPE_UAP_EAPOL_PWK_HSK_TIMEOUT:
            tlv_pairwise_timeout = (MrvlIEtypes_eapol_pwk_hsk_timeout_t *) tlv;
            bss->param.bss_config.pairwise_update_timeout =
                wlan_le32_to_cpu(tlv_pairwise_timeout->pairwise_update_timeout);
            break;
        case TLV_TYPE_UAP_EAPOL_PWK_HSK_RETRIES:
            tlv_pairwise_retries = (MrvlIEtypes_eapol_pwk_hsk_retries_t *) tlv;
            bss->param.bss_config.pwk_retries =
                wlan_le32_to_cpu(tlv_pairwise_retries->pwk_retries);
            break;
        case TLV_TYPE_UAP_EAPOL_GWK_HSK_TIMEOUT:
            tlv_groupwise_timeout = (MrvlIEtypes_eapol_gwk_hsk_timeout_t *) tlv;
            bss->param.bss_config.groupwise_update_timeout =
                wlan_le32_to_cpu(tlv_groupwise_timeout->
                                 groupwise_update_timeout);
            break;
        case TLV_TYPE_UAP_EAPOL_GWK_HSK_RETRIES:
            tlv_groupwise_retries = (MrvlIEtypes_eapol_gwk_hsk_retries_t *) tlv;
            bss->param.bss_config.gwk_retries =
                wlan_le32_to_cpu(tlv_groupwise_retries->gwk_retries);
            break;
        case TLV_TYPE_UAP_MGMT_IE_PASSTHRU_MASK:
            tlv_mgmt_ie_passthru = (MrvlIEtypes_mgmt_ie_passthru_t *) tlv;
            bss->param.bss_config.mgmt_ie_passthru_mask =
                wlan_le32_to_cpu(tlv_mgmt_ie_passthru->mgmt_ie_mask);
            break;
        case TLV_TYPE_2040_BSS_COEX_CONTROL:
            tlv_2040_coex_enable = (MrvlIEtypes_2040_coex_enable_t *) tlv;
            bss->param.bss_config.enable_2040coex =
                tlv_2040_coex_enable->enable_2040coex;
            break;
        case TLV_TYPE_UAP_STA_MAC_ADDR_FILTER:
            tlv_mac_filter = (MrvlIEtypes_mac_filter_t *) tlv;
            bss->param.bss_config.filter.mac_count =
                MIN(MAX_MAC_FILTER_NUM, tlv_mac_filter->count);
            bss->param.bss_config.filter.filter_mode =
                tlv_mac_filter->filter_mode;
            memcpy(pmpriv->adapter,
                   (t_u8 *) bss->param.bss_config.filter.mac_list,
                   tlv_mac_filter->mac_address,
                   MLAN_MAC_ADDR_LENGTH *
                   bss->param.bss_config.filter.mac_count);
            break;
        case TLV_TYPE_UAP_CHAN_BAND_CONFIG:
            tlv_chan_band = (MrvlIEtypes_channel_band_t *) tlv;
            bss->param.bss_config.band_cfg = tlv_chan_band->band_config;
            bss->param.bss_config.channel = tlv_chan_band->channel;
            pmpriv->uap_state_chan_cb.band_config = tlv_chan_band->band_config;
            pmpriv->uap_state_chan_cb.channel = tlv_chan_band->channel;
            break;
        case TLV_TYPE_CHANLIST:
            tlv_chan_list = (MrvlIEtypes_ChanListParamSet_t *) tlv;
            bss->param.bss_config.num_of_chan =
                tlv_len / sizeof(ChanScanParamSet_t);
            pscan_chan = tlv_chan_list->chan_scan_param;
            for (i = 0; i < bss->param.bss_config.num_of_chan; i++) {
                bss->param.bss_config.chan_list[i].chan_number =
                    pscan_chan->chan_number;
                bss->param.bss_config.chan_list[i].band_config_type =
                    pscan_chan->radio_type;
                pscan_chan++;
            }
            break;
        case TLV_TYPE_AUTH_TYPE:
            tlv_auth_type = (MrvlIEtypes_auth_type_t *) tlv;
            bss->param.bss_config.auth_mode = tlv_auth_type->auth_type;
            break;
        case TLV_TYPE_UAP_ENCRYPT_PROTOCOL:
            tlv_encrypt_protocol = (MrvlIEtypes_encrypt_protocol_t *) tlv;
            bss->param.bss_config.protocol =
                wlan_le16_to_cpu(tlv_encrypt_protocol->protocol);
            break;
        case TLV_TYPE_UAP_AKMP:
            tlv_akmp = (MrvlIEtypes_akmp_t *) tlv;
            bss->param.bss_config.key_mgmt =
                wlan_le16_to_cpu(tlv_akmp->key_mgmt);
            if (tlv_len > sizeof(t_u16))
                bss->param.bss_config.key_mgmt_operation =
                    wlan_le16_to_cpu(tlv_akmp->key_mgmt_operation);
            break;
        case TLV_TYPE_PWK_CIPHER:
            tlv_pwk_cipher = (MrvlIEtypes_pwk_cipher_t *) tlv;
            if (wlan_le16_to_cpu(tlv_pwk_cipher->protocol) & PROTOCOL_WPA)
                bss->param.bss_config.wpa_cfg.pairwise_cipher_wpa =
                    tlv_pwk_cipher->pairwise_cipher;
            if (wlan_le16_to_cpu(tlv_pwk_cipher->protocol) & PROTOCOL_WPA2)
                bss->param.bss_config.wpa_cfg.pairwise_cipher_wpa2 =
                    tlv_pwk_cipher->pairwise_cipher;
            break;
        case TLV_TYPE_GWK_CIPHER:
            tlv_gwk_cipher = (MrvlIEtypes_gwk_cipher_t *) tlv;
            bss->param.bss_config.wpa_cfg.group_cipher =
                tlv_gwk_cipher->group_cipher;
            break;
        case TLV_TYPE_UAP_RSN_REPLAY_PROTECT:
            tlv_rsn_prot = (MrvlIEtypes_rsn_replay_prot_t *) tlv;
            bss->param.bss_config.wpa_cfg.rsn_protection =
                tlv_rsn_prot->rsn_replay_prot;
            break;
        case TLV_TYPE_UAP_WPA_PASSPHRASE:
            tlv_passphrase = (MrvlIEtypes_passphrase_t *) tlv;
            bss->param.bss_config.wpa_cfg.length =
                MIN(MLAN_PMK_HEXSTR_LENGTH, tlv_len);
            memcpy(pmpriv->adapter, bss->param.bss_config.wpa_cfg.passphrase,
                   tlv_passphrase->passphrase,
                   bss->param.bss_config.wpa_cfg.length);
            break;
#ifdef WIFI_DIRECT_SUPPORT
        case TLV_TYPE_UAP_PSK:
            tlv_psk = (MrvlIEtypes_psk_t *) tlv;
            memcpy(pmpriv->adapter, bss->param.bss_config.psk, tlv_psk->psk,
                   MIN(MLAN_MAX_KEY_LENGTH, tlv_len));
            break;
#endif /* WIFI_DIRECT_SUPPORT */
        case TLV_TYPE_UAP_GRP_REKEY_TIME:
            tlv_rekey_time = (MrvlIEtypes_group_rekey_time_t *) tlv;
            bss->param.bss_config.wpa_cfg.gk_rekey_time =
                wlan_le32_to_cpu(tlv_rekey_time->gk_rekey_time);
            break;
        case TLV_TYPE_UAP_WEP_KEY:
            tlv_wep_key = (MrvlIEtypes_wep_key_t *) tlv;
            pkey = MNULL;
            if (tlv_wep_key->key_index == 0)
                pkey = &bss->param.bss_config.wep_cfg.key0;
            else if (tlv_wep_key->key_index == 1)
                pkey = &bss->param.bss_config.wep_cfg.key1;
            else if (tlv_wep_key->key_index == 2)
                pkey = &bss->param.bss_config.wep_cfg.key2;
            else if (tlv_wep_key->key_index == 3)
                pkey = &bss->param.bss_config.wep_cfg.key3;
            if (pkey) {
                pkey->key_index = tlv_wep_key->key_index;
                pkey->is_default = tlv_wep_key->is_default;
                pkey->length = MIN(MAX_WEP_KEY_SIZE, (tlv_len - 2));
                memcpy(pmpriv->adapter, pkey->key, tlv_wep_key->key,
                       pkey->length);
            }
            break;
        case TLV_TYPE_UAP_PREAMBLE_CTL:
            tlv_preamble = (MrvlIEtypes_preamble_t *) tlv;
            bss->param.bss_config.preamble_type = tlv_preamble->preamble_type;
            break;
        case TLV_TYPE_BSS_STATUS:
            tlv_bss_status = (MrvlIEtypes_bss_status_t *) tlv;
            bss->param.bss_config.bss_status =
                wlan_le16_to_cpu(tlv_bss_status->bss_status);
            pmpriv->uap_bss_started =
                (bss->param.bss_config.bss_status) ? MTRUE : MFALSE;
            break;
        case HT_CAPABILITY:
            tlv_htcap = (MrvlIETypes_HTCap_t *) tlv;
            bss->param.bss_config.ht_cap_info =
                wlan_le16_to_cpu(tlv_htcap->ht_cap.ht_cap_info);
            bss->param.bss_config.ampdu_param = tlv_htcap->ht_cap.ampdu_param;
            memcpy(pmpriv->adapter, bss->param.bss_config.supported_mcs_set,
                   tlv_htcap->ht_cap.supported_mcs_set, 16);
            bss->param.bss_config.ht_ext_cap =
                wlan_le16_to_cpu(tlv_htcap->ht_cap.ht_ext_cap);
            bss->param.bss_config.tx_bf_cap =
                wlan_le32_to_cpu(tlv_htcap->ht_cap.tx_bf_cap);
            bss->param.bss_config.asel = tlv_htcap->ht_cap.asel;
            break;
        case TLV_TYPE_VENDOR_SPECIFIC_IE:
            tlv_wmm_parameter = (MrvlIEtypes_wmm_parameter_t *) tlv;
            bss->param.bss_config.wmm_para.qos_info =
                tlv_wmm_parameter->wmm_para.qos_info;
            for (ac = 0; ac < 4; ac++) {
                bss->param.bss_config.wmm_para.ac_params[ac].aci_aifsn.aifsn =
                    tlv_wmm_parameter->wmm_para.ac_params[ac].aci_aifsn.aifsn;
                bss->param.bss_config.wmm_para.ac_params[ac].ecw.ecw_max =
                    tlv_wmm_parameter->wmm_para.ac_params[ac].ecw.ecw_max;
                bss->param.bss_config.wmm_para.ac_params[ac].ecw.ecw_min =
                    tlv_wmm_parameter->wmm_para.ac_params[ac].ecw.ecw_min;
                bss->param.bss_config.wmm_para.ac_params[ac].tx_op_limit =
                    wlan_le16_to_cpu(tlv_wmm_parameter->wmm_para.ac_params[ac].
                                     tx_op_limit);
            }
            break;
        }

        tlv_buf_left -= tlv_len + sizeof(MrvlIEtypesHeader_t);
        tlv =
            (MrvlIEtypesHeader_t *) ((t_u8 *) tlv + tlv_len +
                                     sizeof(MrvlIEtypesHeader_t));
    }

    LEAVE();
    return MLAN_STATUS_SUCCESS;
}

/**
 *  @brief This function handles the command response of sys_reset
 *         Clear various private state variables used by DFS.
 *
 *  @param pmpriv       A pointer to mlan_private structure
 *  @param resp         A pointer to HostCmd_DS_COMMAND
 *  @param pioctl_buf   A pointer to mlan_ioctl_req structure
 *
 *  @return             MLAN_STATUS_SUCCESS
 */
static mlan_status
wlan_uap_ret_sys_reset(IN pmlan_private pmpriv,
                       IN HostCmd_DS_COMMAND * resp,
                       IN mlan_ioctl_req * pioctl_buf)
{
    ENTER();

    pmpriv->uap_state_chan_cb.band_config = 0;
    pmpriv->uap_state_chan_cb.channel = 0;
    pmpriv->uap_state_chan_cb.beacon_period = 0;
    pmpriv->uap_state_chan_cb.dtim_period = 0;

    /* assume default 11d/11h states are off, should check with FW */
    /* currently don't clear domain_info... global, could be from STA */
    wlan_11d_priv_init(pmpriv);
    wlan_11h_priv_init(pmpriv);

    LEAVE();
    return MLAN_STATUS_SUCCESS;
}

/**
 *  @brief This function handles the command response of sys_config
 *
 *  @param pmpriv       A pointer to mlan_private structure
 *  @param resp         A pointer to HostCmd_DS_COMMAND
 *  @param pioctl_buf   A pointer to mlan_ioctl_req structure
 *
 *  @return             MLAN_STATUS_SUCCESS
 */
static mlan_status
wlan_uap_ret_sys_config(IN pmlan_private pmpriv,
                        IN HostCmd_DS_COMMAND * resp,
                        IN mlan_ioctl_req * pioctl_buf)
{
    int resp_len = 0, travel_len = 0;
    int i = 0;
    custom_ie *cptr;
    HostCmd_DS_SYS_CONFIG *sys_config =
        (HostCmd_DS_SYS_CONFIG *) & resp->params.sys_config;
    mlan_ds_bss *bss = MNULL;
    mlan_ds_misc_cfg *misc = MNULL;
    MrvlIEtypes_MacAddr_t *tlv =
        (MrvlIEtypes_MacAddr_t *) sys_config->tlv_buffer;
    mlan_ds_misc_custom_ie *cust_ie = MNULL;
    tlvbuf_max_mgmt_ie *max_mgmt_ie = MNULL;
    MrvlIEtypes_channel_band_t *tlv_cb = MNULL;
    MrvlIEtypes_beacon_period_t *tlv_bcnpd = MNULL;
    MrvlIEtypes_dtim_period_t *tlv_dtimpd = MNULL;

    ENTER();
    if (pioctl_buf) {
        if (pioctl_buf->req_id == MLAN_IOCTL_BSS) {
            bss = (mlan_ds_bss *) pioctl_buf->pbuf;
            if (bss->sub_command == MLAN_OID_BSS_MAC_ADDR) {
                if (TLV_TYPE_UAP_MAC_ADDRESS ==
                    wlan_le16_to_cpu(tlv->header.type)) {
                    memcpy(pmpriv->adapter, &bss->param.mac_addr, tlv->mac,
                           MLAN_MAC_ADDR_LENGTH);
                }
            } else if ((bss->sub_command == MLAN_OID_UAP_BSS_CONFIG) &&
                       (pioctl_buf->action == MLAN_ACT_GET)) {
                wlan_uap_ret_cmd_ap_config(pmpriv, resp, pioctl_buf);
            }
        }
        if (pioctl_buf->req_id == MLAN_IOCTL_MISC_CFG) {
            misc = (mlan_ds_misc_cfg *) pioctl_buf->pbuf;
            cust_ie = (mlan_ds_misc_custom_ie *) sys_config->tlv_buffer;
            max_mgmt_ie =
                (tlvbuf_max_mgmt_ie *) (sys_config->tlv_buffer + cust_ie->len +
                                        sizeof(MrvlIEtypesHeader_t));
            if ((pioctl_buf->action == MLAN_ACT_GET) &&
                (misc->sub_command == MLAN_OID_MISC_CUSTOM_IE)) {

                cust_ie->type = wlan_le16_to_cpu(cust_ie->type);
                resp_len = cust_ie->len = wlan_le16_to_cpu(cust_ie->len);
                travel_len = 0;
                /* conversion for index, mask, len */
                if (resp_len == sizeof(t_u16))
                    cust_ie->ie_data_list[0].ie_index =
                        wlan_cpu_to_le16(cust_ie->ie_data_list[0].ie_index);

                while (resp_len > sizeof(t_u16)) {
                    cptr =
                        (custom_ie *) (((t_u8 *) cust_ie->ie_data_list) +
                                       travel_len);
                    cptr->ie_index = wlan_le16_to_cpu(cptr->ie_index);
                    cptr->mgmt_subtype_mask =
                        wlan_le16_to_cpu(cptr->mgmt_subtype_mask);
                    cptr->ie_length = wlan_le16_to_cpu(cptr->ie_length);
                    travel_len +=
                        cptr->ie_length + sizeof(custom_ie) - MAX_IE_SIZE;
                    resp_len -=
                        cptr->ie_length + sizeof(custom_ie) - MAX_IE_SIZE;
                }
                memcpy(pmpriv->adapter, &misc->param.cust_ie, cust_ie,
                       MIN(sizeof(mlan_ds_misc_custom_ie) -
                           sizeof(tlvbuf_max_mgmt_ie),
                           (cust_ie->len + sizeof(MrvlIEtypesHeader_t))));
                if (max_mgmt_ie) {
                    max_mgmt_ie->type = wlan_le16_to_cpu(max_mgmt_ie->type);
                    if (max_mgmt_ie->type == TLV_TYPE_MAX_MGMT_IE) {
                        max_mgmt_ie->len = wlan_le16_to_cpu(max_mgmt_ie->len);
                        max_mgmt_ie->count =
                            wlan_le16_to_cpu(max_mgmt_ie->count);
                        for (i = 0; i < max_mgmt_ie->count; i++) {
                            max_mgmt_ie->info[i].buf_size =
                                wlan_le16_to_cpu(max_mgmt_ie->info[i].buf_size);
                            max_mgmt_ie->info[i].buf_count =
                                wlan_le16_to_cpu(max_mgmt_ie->info[i].
                                                 buf_count);
                        }
                        /* Append max_mgmt_ie TLV after custom_ie */
                        memcpy(pmpriv->adapter,
                               (t_u8 *) & misc->param.cust_ie + (cust_ie->len +
                                                                 sizeof
                                                                 (MrvlIEtypesHeader_t)),
                               max_mgmt_ie, MIN(sizeof(tlvbuf_max_mgmt_ie),
                                                max_mgmt_ie->len +
                                                sizeof(MrvlIEtypesHeader_t)));
                    }
                }
            }
        }
    } else {                    /* no ioctl: driver generated get/set */
        switch (wlan_le16_to_cpu(tlv->header.type)) {
        case TLV_TYPE_UAP_MAC_ADDRESS:
            memcpy(pmpriv->adapter, pmpriv->curr_addr, tlv->mac,
                   MLAN_MAC_ADDR_LENGTH);
            break;
        case TLV_TYPE_UAP_CHAN_BAND_CONFIG:
            tlv_cb = (MrvlIEtypes_channel_band_t *) tlv;
            pmpriv->uap_state_chan_cb.band_config = tlv_cb->band_config;
            pmpriv->uap_state_chan_cb.channel = tlv_cb->channel;
            /* call callback waiting for channel info */
            if (pmpriv->uap_state_chan_cb.get_chan_callback)
                pmpriv->uap_state_chan_cb.get_chan_callback(pmpriv);
            break;
        case TLV_TYPE_UAP_BEACON_PERIOD:
            tlv_bcnpd = (MrvlIEtypes_beacon_period_t *) tlv;
            pmpriv->uap_state_chan_cb.beacon_period =
                wlan_le16_to_cpu(tlv_bcnpd->beacon_period);
            /* copy dtim_period as well if it follows */
            tlv_dtimpd =
                (MrvlIEtypes_dtim_period_t *) (((t_u8 *) tlv) +
                                               sizeof
                                               (MrvlIEtypes_beacon_period_t));
            if (TLV_TYPE_UAP_DTIM_PERIOD ==
                wlan_le16_to_cpu(tlv_dtimpd->header.type))
                pmpriv->uap_state_chan_cb.dtim_period = tlv_dtimpd->dtim_period;
            /* call callback waiting for beacon/dtim info */
            if (pmpriv->uap_state_chan_cb.get_chan_callback)
                pmpriv->uap_state_chan_cb.get_chan_callback(pmpriv);
            break;
        case TLV_TYPE_MGMT_IE:
            if ((pmpriv->adapter->state_rdh.stage == RDH_SET_CUSTOM_IE) ||
                (pmpriv->adapter->state_rdh.stage == RDH_REM_CUSTOM_IE))
                wlan_11h_radar_detected_callback((t_void *) pmpriv);
            break;
        }
    }
    LEAVE();
    return MLAN_STATUS_SUCCESS;
}
#endif /* CONFIG_MLAN_WMSDK */

/**
 *  @brief This function prepares command of snmp_mib
 *
 *  @param pmpriv		A pointer to mlan_private structure
 *  @param cmd	   		A pointer to HostCmd_DS_COMMAND structure
 *  @param cmd_action   the action: GET or SET
 *  @param cmd_oid      Cmd oid: treated as sub command
 *  @param pioctl_buf   A pointer to mlan_ioctl_req structure
 *  @param pdata_buf    A pointer to information buffer
 *  @return         MLAN_STATUS_SUCCESS or MLAN_STATUS_FAILURE
 */
static mlan_status
wlan_uap_cmd_snmp_mib(pmlan_private pmpriv,
                      IN HostCmd_DS_COMMAND * cmd,
                      IN t_u16 cmd_action,
                      IN t_u32 cmd_oid,
                      IN pmlan_ioctl_req pioctl_buf, IN t_void * pdata_buf)
{
    HostCmd_DS_802_11_SNMP_MIB *psnmp_mib = &cmd->params.smib;
    mlan_status ret = MLAN_STATUS_SUCCESS;
    t_u8 *psnmp_oid = MNULL;
    t_u32 ul_temp;
    t_u8 i;

    t_u8 snmp_oids[] = {
        tkip_mic_failures,
        ccmp_decrypt_errors,
        wep_undecryptable_count,
        wep_icv_error_count,
        decrypt_failure_count,
        dot11_mcast_tx_count,
        dot11_failed_count,
        dot11_retry_count,
        dot11_multi_retry_count,
        dot11_frame_dup_count,
        dot11_rts_success_count,
        dot11_rts_failure_count,
        dot11_ack_failure_count,
        dot11_rx_fragment_count,
        dot11_mcast_rx_frame_count,
        dot11_fcs_error_count,
        dot11_tx_frame_count,
        dot11_rsna_tkip_cm_invoked,
        dot11_rsna_4way_hshk_failures,
    };

    ENTER();

    if (cmd_action == HostCmd_ACT_GEN_GET) {
        cmd->command = wlan_cpu_to_le16(HostCmd_CMD_802_11_SNMP_MIB);
        psnmp_mib->query_type = wlan_cpu_to_le16(HostCmd_ACT_GEN_GET);
	if (cmd_oid == StopDeauth_i) {
	    psnmp_mib->oid = wlan_cpu_to_le16((t_u16)StopDeauth_i);
	    psnmp_mib->buf_size = wlan_cpu_to_le16(sizeof(t_u8));
	    cmd->size =
	       wlan_cpu_to_le16(sizeof(HostCmd_DS_802_11_SNMP_MIB) + S_DS_GEN);
	} else {
        cmd->size =
            wlan_cpu_to_le16(sizeof(t_u16) + S_DS_GEN +
                             sizeof(snmp_oids) *
                             sizeof(MrvlIEtypes_snmp_oid_t));
        psnmp_oid = (t_u8 *) & psnmp_mib->oid;
        for (i = 0; i < sizeof(snmp_oids); i++) {
            /* SNMP OID header type */
            *(t_u16 *) psnmp_oid = wlan_cpu_to_le16(snmp_oids[i]);
            psnmp_oid += sizeof(t_u16);
            /* SNMP OID header length */
            *(t_u16 *) psnmp_oid = wlan_cpu_to_le16(sizeof(t_u32));
            psnmp_oid += sizeof(t_u16) + sizeof(t_u32);
        }
	}
    } else {                    /* cmd_action == ACT_SET */
        cmd->command = wlan_cpu_to_le16(HostCmd_CMD_802_11_SNMP_MIB);
        cmd->size = sizeof(HostCmd_DS_802_11_SNMP_MIB) - 1 + S_DS_GEN;
        psnmp_mib->query_type = wlan_cpu_to_le16(HostCmd_ACT_GEN_SET);

        switch (cmd_oid) {
        case Dot11D_i:
        case Dot11H_i:
            psnmp_mib->oid = wlan_cpu_to_le16((t_u16) cmd_oid);
            psnmp_mib->buf_size = wlan_cpu_to_le16(sizeof(t_u16));
            ul_temp = *(t_u32 *) pdata_buf;
            *((t_u16 *) (psnmp_mib->value)) = wlan_cpu_to_le16((t_u16) ul_temp);
            cmd->size += sizeof(t_u16);
            break;
	case StopDeauth_i:
	    psnmp_mib->oid = wlan_cpu_to_le16((t_u16) cmd_oid);
	    psnmp_mib->buf_size = wlan_cpu_to_le16(sizeof(t_u8));
	    psnmp_mib->value[0] = *((t_u8 *)pdata_buf);
	    cmd->size += sizeof(t_u8);
	    break;
        default:
            PRINTM(MERROR, "Unsupported OID.\n");
            ret = MLAN_STATUS_FAILURE;
            break;
        }
        cmd->size = wlan_cpu_to_le16(cmd->size);
    }

    LEAVE();
    return ret;
}

#ifndef CONFIG_MLAN_WMSDK
/**
 *  @brief This function handles the command response of snmp_mib
 *
 *  @param pmpriv       A pointer to mlan_private structure
 *  @param resp         A pointer to HostCmd_DS_COMMAND
 *  @param pioctl_buf   A pointer to mlan_ioctl_req structure
 *
 *  @return             MLAN_STATUS_SUCCESS
 */
static mlan_status
wlan_uap_ret_snmp_mib(IN pmlan_private pmpriv,
                      IN HostCmd_DS_COMMAND * resp,
                      IN mlan_ioctl_req * pioctl_buf)
{
    pmlan_adapter pmadapter = pmpriv->adapter;
    HostCmd_DS_802_11_SNMP_MIB *psnmp_mib =
        (HostCmd_DS_802_11_SNMP_MIB *) & resp->params.smib;
    mlan_ds_get_info *info;
    mlan_ds_snmp_mib *mib = MNULL;
    t_u16 oid = wlan_le16_to_cpu(psnmp_mib->oid);
    t_u8 *psnmp_oid = MNULL;
    t_u32 data;
    t_u16 tlv_buf_left = 0;
    t_u16 tlv_type = 0;

    ENTER();
    if (psnmp_mib->query_type == HostCmd_ACT_GEN_GET) {
        if (!pioctl_buf) {
            LEAVE();
            return MLAN_STATUS_SUCCESS;
        }
	if (oid == StopDeauth_i) {
	    mib = (mlan_ds_snmp_mib *) pioctl_buf->pbuf;
	    if (mib)
		mib->param.deauthctrl = psnmp_mib->value[0];
	    LEAVE();
	    return MLAN_STATUS_SUCCESS;
	}
        info = (mlan_ds_get_info *) pioctl_buf->pbuf;
        tlv_buf_left = resp->size - (sizeof(t_u16) + S_DS_GEN);
        psnmp_oid = (t_u8 *) & psnmp_mib->oid;
        while (tlv_buf_left >= sizeof(MrvlIEtypes_snmp_oid_t)) {
            tlv_type = wlan_le16_to_cpu(*(t_u16 *) psnmp_oid);
            psnmp_oid += sizeof(t_u16) + sizeof(t_u16);
            memcpy(pmadapter, &data, psnmp_oid, sizeof(t_u32));
            switch (tlv_type) {
            case tkip_mic_failures:
                info->param.ustats.tkip_mic_failures = wlan_le32_to_cpu(data);
                break;
            case ccmp_decrypt_errors:
                info->param.ustats.ccmp_decrypt_errors = wlan_le32_to_cpu(data);
                break;
            case wep_undecryptable_count:
                info->param.ustats.wep_undecryptable_count =
                    wlan_le32_to_cpu(data);
                break;
            case wep_icv_error_count:
                info->param.ustats.wep_icv_error_count = wlan_le32_to_cpu(data);
                break;
            case decrypt_failure_count:
                info->param.ustats.decrypt_failure_count =
                    wlan_le32_to_cpu(data);
                break;
            case dot11_mcast_tx_count:
                info->param.ustats.mcast_tx_count = wlan_le32_to_cpu(data);
                break;
            case dot11_failed_count:
                info->param.ustats.failed_count = wlan_le32_to_cpu(data);
                break;
            case dot11_retry_count:
                info->param.ustats.retry_count = wlan_le32_to_cpu(data);
                break;
            case dot11_multi_retry_count:
                info->param.ustats.multi_retry_count = wlan_le32_to_cpu(data);
                break;
            case dot11_frame_dup_count:
                info->param.ustats.frame_dup_count = wlan_le32_to_cpu(data);
                break;
            case dot11_rts_success_count:
                info->param.ustats.rts_success_count = wlan_le32_to_cpu(data);
                break;
            case dot11_rts_failure_count:
                info->param.ustats.rts_failure_count = wlan_le32_to_cpu(data);
                break;
            case dot11_ack_failure_count:
                info->param.ustats.ack_failure_count = wlan_le32_to_cpu(data);
                break;
            case dot11_rx_fragment_count:
                info->param.ustats.rx_fragment_count = wlan_le32_to_cpu(data);
                break;
            case dot11_mcast_rx_frame_count:
                info->param.ustats.mcast_rx_frame_count =
                    wlan_le32_to_cpu(data);
                break;
            case dot11_fcs_error_count:
                info->param.ustats.fcs_error_count = wlan_le32_to_cpu(data);
                break;
            case dot11_tx_frame_count:
                info->param.ustats.tx_frame_count = wlan_le32_to_cpu(data);
                break;
            case dot11_rsna_tkip_cm_invoked:
                info->param.ustats.rsna_tkip_cm_invoked =
                    wlan_le32_to_cpu(data);
                break;
            case dot11_rsna_4way_hshk_failures:
                info->param.ustats.rsna_4way_hshk_failures =
                    wlan_le32_to_cpu(data);
                break;
            }
            tlv_buf_left -= sizeof(MrvlIEtypes_snmp_oid_t);
            psnmp_oid += sizeof(t_u32);
        }
    } else {                    /* ACT_SET */
        switch (psnmp_mib->oid) {
        case Dot11D_i:
            data = wlan_le16_to_cpu(*((t_u16 *) (psnmp_mib->value)));
            /* Set 11d state to private */
            pmpriv->state_11d.enable_11d = data;
            /* Set user enable flag if called from ioctl */
            if (pioctl_buf)
                pmpriv->state_11d.user_enable_11d = data;
            break;
        case Dot11H_i:
            data = wlan_le16_to_cpu(*((t_u16 *) (psnmp_mib->value)));
            /* Set 11h state to priv */
            pmpriv->intf_state_11h.is_11h_active = (data & ENABLE_11H_MASK);
            /* Set radar_det state to adapter */
            pmpriv->adapter->state_11h.is_master_radar_det_active
                = (data & MASTER_RADAR_DET_MASK) ? MTRUE : MFALSE;
            pmpriv->adapter->state_11h.is_slave_radar_det_active
                = (data & SLAVE_RADAR_DET_MASK) ? MTRUE : MFALSE;
            break;
        }
    }

    LEAVE();
    return MLAN_STATUS_SUCCESS;
}
#endif /* CONFIG_MLAN_WMSDK */

/**
 *  @brief This function prepares command of deauth station
 *
 *  @param pmpriv		A pointer to mlan_private structure
 *  @param cmd	   		A pointer to HostCmd_DS_COMMAND structure
 *  @param pdata_buf    A pointer to data buffer
 *  @return         MLAN_STATUS_SUCCESS
 */
static mlan_status
wlan_uap_cmd_sta_deauth(pmlan_private pmpriv,
                        IN HostCmd_DS_COMMAND * cmd, IN t_void * pdata_buf)
{
    HostCmd_DS_STA_DEAUTH *pcmd_sta_deauth =
        (HostCmd_DS_STA_DEAUTH *) & cmd->params.sta_deauth;
    mlan_deauth_param *deauth = (mlan_deauth_param *) pdata_buf;

    ENTER();
    cmd->command = wlan_cpu_to_le16(HOST_CMD_APCMD_STA_DEAUTH);
    cmd->size = wlan_cpu_to_le16(S_DS_GEN + sizeof(HostCmd_DS_STA_DEAUTH));
    memcpy(pmpriv->adapter, pcmd_sta_deauth->mac, deauth->mac_addr,
           MLAN_MAC_ADDR_LENGTH);
    pcmd_sta_deauth->reason = wlan_cpu_to_le16(deauth->reason_code);
    LEAVE();
    return MLAN_STATUS_SUCCESS;
}

#ifndef CONFIG_MLAN_WMSDK
#ifdef HOST_AUTHENTICATOR
/**
 *  @brief This function prepares command of report mic_err
 *
 *  @param pmpriv       A pointer to mlan_private structure
 *  @param cmd          A pointer to HostCmd_DS_COMMAND structure
 *  @param pdata_buf    A pointer to data buffer
 *  @return         MLAN_STATUS_SUCCESS
 */
static mlan_status wlan_uap_cmd_report_mic(
		pmlan_private           pmpriv,
		IN HostCmd_DS_COMMAND   *cmd,
		IN t_void               *pdata_buf)
{
	HostCmd_DS_REPORT_MIC *pcmd_report_mic =
		(HostCmd_DS_REPORT_MIC*) &cmd->params.report_mic;

	ENTER();
	cmd->command = wlan_cpu_to_le16(HOST_CMD_APCMD_REPORT_MIC);
	cmd->size = wlan_cpu_to_le16(S_DS_GEN + sizeof(HostCmd_DS_REPORT_MIC));
	memcpy(pmpriv->adapter, pcmd_report_mic->mac, pdata_buf,
				MLAN_MAC_ADDR_LENGTH);
	LEAVE();
	return MLAN_STATUS_SUCCESS;
}
#endif /* HOST_AUTHENTICATOR */
#endif /* CONFIG_MLAN_WMSDK */

#if defined(WAPI_AP) || defined(HOST_AUTHENTICATOR)
#ifdef KEY_PARAM_SET_V2
/**
 *  @brief This function prepares command of key material
 *
 *  @param pmpriv       A pointer to mlan_private structure
 *  @param cmd          A pointer to HostCmd_DS_COMMAND structure
 *  @param cmd_action   The action: GET or SET
 *  @param cmd_oid      OID: ENABLE or DISABLE
 *  @param pdata_buf    A pointer to data buffer
 *  @return             MLAN_STATUS_SUCCESS
 */
static mlan_status wlan_uap_cmd_key_material(
		IN pmlan_private        pmpriv,
		IN HostCmd_DS_COMMAND   *cmd,
		IN t_u16                cmd_action,
		IN t_u16                cmd_oid,
		IN t_void               *pdata_buf)
{
	HostCmd_DS_802_11_KEY_MATERIAL  *pkey_material =
					&cmd->params.key_material;
	mlan_ds_encrypt_key             *pkey = (mlan_ds_encrypt_key*)pdata_buf;
	mlan_status                     ret = MLAN_STATUS_SUCCESS;
#if defined(WAPI_AP) && defined(UAP_STA_LIST)
	sta_node *sta_ptr = MNULL;
#endif

	ENTER ();
	if(!pkey) {
		ret = MLAN_STATUS_FAILURE;
		goto done;
	}
	cmd->command = wlan_cpu_to_le16(HostCmd_CMD_802_11_KEY_MATERIAL);
	pkey_material->action = wlan_cpu_to_le16(cmd_action);
	if( cmd_action == HostCmd_ACT_GEN_GET ) {
		cmd->size = wlan_cpu_to_le16(sizeof(pkey_material->action)
						+ S_DS_GEN);
		goto done;
	}
	memset(pmpriv->adapter, &pkey_material->key_param_set, 0,
			sizeof(MrvlIEtype_KeyParamSetV2_t));
	if(pkey->key_flags & KEY_FLAG_REMOVE_KEY) {
		pkey_material->action =
				wlan_cpu_to_le16(HostCmd_ACT_GEN_REMOVE);
		pkey_material->key_param_set.type =
				wlan_cpu_to_le16(TLV_TYPE_KEY_PARAM_V2);
		pkey_material->key_param_set.length =
				wlan_cpu_to_le16(KEY_PARAMS_FIXED_LEN);
		pkey_material->key_param_set.key_idx =
				pkey->key_index & KEY_INDEX_MASK;
		pkey_material->key_param_set.key_info =
				KEY_INFO_MCAST_KEY | KEY_INFO_UCAST_KEY;
		memcpy(pmpriv->adapter, pkey_material->key_param_set.mac_addr,
				pkey->mac_addr, MLAN_MAC_ADDR_LENGTH);
		cmd->size = wlan_cpu_to_le16(sizeof(MrvlIEtypesHeader_t)
				+ S_DS_GEN + KEY_PARAMS_FIXED_LEN
				+ sizeof(pkey_material->action));
		PRINTM(MCMND, "Remove Key\n");
		goto done;
	}
	pkey_material->action = wlan_cpu_to_le16(HostCmd_ACT_GEN_SET);
	pkey_material->key_param_set.key_idx = pkey->key_index & KEY_INDEX_MASK;
	pkey_material->key_param_set.type =
				wlan_cpu_to_le16(TLV_TYPE_KEY_PARAM_V2);
	pkey_material->key_param_set.key_info = KEY_INFO_ENABLE_KEY;
	memcpy(pmpriv->adapter, pkey_material->key_param_set.mac_addr,
				pkey->mac_addr, MLAN_MAC_ADDR_LENGTH);
#ifndef CONFIG_MLAN_WMSDK
	if(pkey->key_len <= MAX_WEP_KEY_SIZE) {
		pkey_material->key_param_set.length =
				wlan_cpu_to_le16(KEY_PARAMS_FIXED_LEN
						+ sizeof(wep_param_t));
		pkey_material->key_param_set.key_type =
				wlan_cpu_to_le16(KEY_TYPE_ID_WEP);
		pkey_material->key_param_set.key_info |=
				KEY_INFO_MCAST_KEY | KEY_INFO_UCAST_KEY ;
		if(pkey_material->key_param_set.key_idx ==
				(pmpriv->wep_key_curr_index & KEY_INDEX_MASK))
			pkey_material->key_param_set.key_info |=
				KEY_INFO_DEFAULT_KEY;
		pkey_material->key_param_set.key_info =
			wlan_cpu_to_le16(pkey_material->key_param_set.key_info);
		pkey_material->key_param_set.key_params.wep.key_len =
				wlan_cpu_to_le16(pkey->key_len);
		memcpy(pmpriv->adapter,
			pkey_material->key_param_set.key_params.wep.key,
			pkey->key_material,pkey->key_len);
		cmd->size = wlan_cpu_to_le16(sizeof(MrvlIEtypesHeader_t)
				+ S_DS_GEN + KEY_PARAMS_FIXED_LEN
				+ sizeof(wep_param_t)
				+ sizeof(pkey_material->action));
		PRINTM(MCMND, "Set WEP Key\n");
		goto done;
	}
#endif /* CONFIG_MLAN_WMSDK */
	if(pkey->key_flags & KEY_FLAG_GROUP_KEY)
		pkey_material->key_param_set.key_info |= KEY_INFO_MCAST_KEY;
	else
		pkey_material->key_param_set.key_info |= KEY_INFO_UCAST_KEY;
#ifdef ENABLE_802_11W
	if(pkey->key_flags & KEY_FLAG_AES_MCAST_IGTK)
		pkey_material->key_param_set.key_info = KEY_INFO_CMAC_AES_KEY;
#endif
	if(pkey->key_flags & KEY_FLAG_SET_TX_KEY)
		pkey_material->key_param_set.key_info |=
				KEY_INFO_TX_KEY | KEY_INFO_RX_KEY;
	else
		pkey_material->key_param_set.key_info |= KEY_INFO_TX_KEY;
#ifndef CONFIG_MLAN_WMSDK
#ifdef WAPI
	if(pkey->is_wapi_key) {
		pkey_material->key_param_set.key_type =
				wlan_cpu_to_le16(KEY_TYPE_ID_WAPI);
		memcpy(pmpriv->adapter,
			pkey_material->key_param_set.key_params.wapi.pn,
			pkey->pn, PN_SIZE);
		pkey_material->key_param_set.key_params.wapi.key_len =
			MIN(WAPI_KEY_SIZE,wlan_cpu_to_le16(pkey->key_len));
		memcpy(pmpriv->adapter,
			pkey_material->key_param_set.key_params.wapi.key,
			pkey->key_material,MIN(WAPI_KEY_SIZE,pkey->key_len));
		if(!pmpriv->sec_info.wapi_key_on)
			pkey_material->key_param_set.key_info |=
				KEY_INFO_DEFAULT_KEY;
		if(pkey->key_flags & KEY_FLAG_GROUP_KEY) {
			pmpriv->sec_info.wapi_key_on = MTRUE;
		}
#ifdef UAP_STA_LIST
		else {
			/* WAPI pairwise key: unicast */
			sta_ptr = wlan_add_station_entry(pmpriv,
						pkey->mac_addr);
			if (sta_ptr) {
				PRINTM(MCMND,"station: wapi_key_on\n");
				sta_ptr->wapi_key_on = MTRUE;
			}
		}
#endif /* UAP_STA_LIST */
		pkey_material->key_param_set.key_info =
			wlan_cpu_to_le16(pkey_material->key_param_set.key_info);
		pkey_material->key_param_set.length =
			wlan_cpu_to_le16(KEY_PARAMS_FIXED_LEN
					+ sizeof(wapi_param));
		cmd->size = wlan_cpu_to_le16(sizeof(MrvlIEtypesHeader_t)
				+ S_DS_GEN + KEY_PARAMS_FIXED_LEN
				+ sizeof(wapi_param)
				+ sizeof(pkey_material->action));
		PRINTM(MCMND, "Set WAPI Key\n");
		goto done;
	}
#endif
#endif /* CONFIG_MLAN_WMSDK */

	pkey_material->key_param_set.key_info |= KEY_INFO_DEFAULT_KEY;
	pkey_material->key_param_set.key_info =
			wlan_cpu_to_le16(pkey_material->key_param_set.key_info);
#ifdef ENABLE_802_11W
	if (pkey->key_len == WPA_AES_KEY_LEN &&
			!(pkey->key_flags & KEY_FLAG_AES_MCAST_IGTK)) {
#else
	if (pkey->key_len == WPA_AES_KEY_LEN) {
#endif
		if(pkey->key_flags &
			(KEY_FLAG_RX_SEQ_VALID | KEY_FLAG_TX_SEQ_VALID))
			memcpy(pmpriv->adapter,
				pkey_material->key_param_set.
				key_params.aes.pn,
				pkey->pn, SEQ_MAX_SIZE);
			pkey_material->key_param_set.key_type =
					wlan_cpu_to_le16(KEY_TYPE_ID_AES);
			pkey_material->key_param_set.key_params.aes.key_len =
					wlan_cpu_to_le16(pkey->key_len);
			memcpy(pmpriv->adapter,
				pkey_material->key_param_set.key_params.aes.key,
				pkey->key_material,pkey->key_len);
			pkey_material->key_param_set.length =
				wlan_cpu_to_le16(KEY_PARAMS_FIXED_LEN
						+ sizeof(aes_param));
			cmd->size = wlan_cpu_to_le16(sizeof(MrvlIEtypesHeader_t)
					+ S_DS_GEN + KEY_PARAMS_FIXED_LEN
					+ sizeof(aes_param)
					+ sizeof(pkey_material->action));
			PRINTM(MCMND, "Set AES Key\n");
			goto done;
		}
#ifdef ENABLE_802_11W
	if (pkey->key_len == WPA_IGTK_KEY_LEN
		&& (pkey->key_flags & KEY_FLAG_AES_MCAST_IGTK)) {
		if(pkey->key_flags &
			(KEY_FLAG_RX_SEQ_VALID | KEY_FLAG_TX_SEQ_VALID))
				memcpy(pmpriv->adapter,
					pkey_material->key_param_set.
					key_params.cmac_aes.ipn,
					pkey->pn, SEQ_MAX_SIZE);
			pkey_material->key_param_set.key_info &=
					~KEY_INFO_MCAST_KEY;
			pkey_material->key_param_set.key_info |=
				wlan_cpu_to_le16(KEY_INFO_AES_MCAST_IGTK);
			pkey_material->key_param_set.key_type =
					KEY_TYPE_ID_AES_CMAC;
			pkey_material->key_param_set.key_params.cmac_aes.key_len
					 = wlan_cpu_to_le16(pkey->key_len);
			memcpy(pmpriv->adapter,
				pkey_material->key_param_set.
				key_params.cmac_aes.key,
				pkey->key_material,pkey->key_len);
			pkey_material->key_param_set.length =
				wlan_cpu_to_le16(KEY_PARAMS_FIXED_LEN
						+ sizeof(cmac_aes_param));
			cmd->size = wlan_cpu_to_le16(sizeof(MrvlIEtypesHeader_t)
					+ S_DS_GEN + KEY_PARAMS_FIXED_LEN
					+ sizeof(cmac_aes_param)
					+ sizeof(pkey_material->action));
			PRINTM(MCMND, "Set CMAC AES Key\n");
			goto done;
	}
#endif
#ifndef CONFIG_MLAN_WMSDK
	if (pkey->key_len == WPA_TKIP_KEY_LEN) {
		if(pkey->key_flags &
			(KEY_FLAG_RX_SEQ_VALID | KEY_FLAG_TX_SEQ_VALID))
			memcpy(pmpriv->adapter,
					pkey_material->key_param_set.
					key_params.tkip.pn,
					pkey->pn, SEQ_MAX_SIZE);
			pkey_material->key_param_set.key_type =
					wlan_cpu_to_le16(KEY_TYPE_ID_TKIP);
			pkey_material->key_param_set.key_params.tkip.key_len =
					wlan_cpu_to_le16(pkey->key_len);
			memcpy(pmpriv->adapter,
				pkey_material->key_param_set.
				key_params.tkip.key,
				pkey->key_material,pkey->key_len);
			pkey_material->key_param_set.length =
				wlan_cpu_to_le16(KEY_PARAMS_FIXED_LEN
						+ sizeof(tkip_param));
			cmd->size = wlan_cpu_to_le16(sizeof(MrvlIEtypesHeader_t)
					+ S_DS_GEN + KEY_PARAMS_FIXED_LEN
					+ sizeof(tkip_param)
					+ sizeof(pkey_material->action));
			PRINTM(MCMND, "Set TKIP Key\n");
	}
#endif /* CONFIG_MLAN_WMSDK */
done:
#if 0
	dump_hex(cmd, cmd->size);
#endif
	LEAVE();
	return ret;
}
#else
/**
 *  @brief This function prepares command of key material
 *
 *  @param pmpriv       A pointer to mlan_private structure
 *  @param cmd          A pointer to HostCmd_DS_COMMAND structure
 *  @param cmd_action   The action: GET or SET
 *  @param cmd_oid      OID: ENABLE or DISABLE
 *  @param pdata_buf    A pointer to data buffer
 *  @return             MLAN_STATUS_SUCCESS
 */
static mlan_status wlan_uap_cmd_key_material(
		IN pmlan_private        pmpriv,
		IN HostCmd_DS_COMMAND   *cmd,
		IN t_u16                cmd_action,
		IN t_u16                cmd_oid,
		IN t_void               *pdata_buf)
{
	HostCmd_DS_802_11_KEY_MATERIAL  *pkey_material =
						&cmd->params.key_material;
	mlan_ds_encrypt_key             *pkey = (mlan_ds_encrypt_key*)pdata_buf;
	MrvlIEtypes_MacAddr_t           *tlv = MNULL;
	const t_u8      bc_mac[] = {0xff, 0xff, 0xff, 0xff, 0xff, 0xff};
	t_u16                           key_param_len = 0;
	mlan_status                     ret = MLAN_STATUS_SUCCESS;
#if defined(WAPI_AP) && defined(UAP_STA_LIST)
	sta_node *sta_ptr = MNULL;
#endif

	ENTER();

	cmd->command = wlan_cpu_to_le16(HostCmd_CMD_802_11_KEY_MATERIAL);
	pkey_material->action = wlan_cpu_to_le16(cmd_action);
	if( cmd_action == HostCmd_ACT_GEN_GET ) {
		cmd->size = wlan_cpu_to_le16(sizeof(pkey_material->action)
						+ S_DS_GEN);
		goto done;
	}

	memset(pmpriv->adapter, &pkey_material->key_param_set, 0,
			sizeof(MrvlIEtype_KeyParamSet_t));
#ifndef CONFIG_MLAN_WMSDK
#ifdef WAPI
	if (pkey->is_wapi_key) {
		PRINTM(MINFO, "Set WAPI Key\n");
		pkey_material->key_param_set.key_type_id =
				wlan_cpu_to_le16(KEY_TYPE_ID_WAPI);
		if (cmd_oid == KEY_INFO_ENABLED)
			pkey_material->key_param_set.key_info =
				wlan_cpu_to_le16(KEY_INFO_WAPI_ENABLED);
		else
			pkey_material->key_param_set.key_info =
				!(wlan_cpu_to_le16(KEY_INFO_WAPI_ENABLED));
			pkey_material->key_param_set.key[0] = pkey->key_index;
		if(!pmpriv->sec_info.wapi_key_on)
			pkey_material->key_param_set.key[1] = 1;
		else
			pkey_material->key_param_set.key[1] = 0;

		if (0 != memcmp(pmpriv->adapter,
				pkey->mac_addr, bc_mac,
				sizeof(bc_mac))) {/
			pkey_material->key_param_set.key_info |=
			wlan_cpu_to_le16(KEY_INFO_WAPI_UNICAST);
#ifdef UAP_STA_LIST
			sta_ptr = wlan_add_station_entry(pmpriv,
						pkey->mac_addr);
			if (sta_ptr) {
				PRINTM(MCMND,"station: wapi_key_on\n");
				sta_ptr->wapi_key_on = MTRUE;
			}
#endif /* UAP_STA_LIST */
		}
		else {	               /* WAPI group key: multicast */
			pkey_material->key_param_set.key_info |=
				wlan_cpu_to_le16(KEY_INFO_WAPI_MCAST);
			pmpriv->sec_info.wapi_key_on = MTRUE;
		}
		pkey_material->key_param_set.type =
				wlan_cpu_to_le16(TLV_TYPE_KEY_MATERIAL);
		pkey_material->key_param_set.key_len =
				wlan_cpu_to_le16(WAPI_KEY_LEN);
		memcpy(pmpriv->adapter,
				&pkey_material->key_param_set.key[2],
				pkey->key_material, pkey->key_len);
		pkey_material->key_param_set.length =
				wlan_cpu_to_le16(WAPI_KEY_LEN
				+ KEYPARAMSET_FIXED_LEN);

		key_param_len = (WAPI_KEY_LEN + KEYPARAMSET_FIXED_LEN)
					+ sizeof(MrvlIEtypesHeader_t);
		tlv =(MrvlIEtypes_MacAddr_t *)
				((t_u8 *)&pkey_material->key_param_set
					+ key_param_len);
		tlv->header.type =
			wlan_cpu_to_le16(TLV_TYPE_STA_MAC_ADDRESS);
		tlv->header.len= wlan_cpu_to_le16(MLAN_MAC_ADDR_LENGTH);
		memcpy(pmpriv->adapter,
			tlv->mac,pkey->mac_addr, MLAN_MAC_ADDR_LENGTH);
		cmd->size = wlan_cpu_to_le16(key_param_len
					+ sizeof(pkey_material->action)
					+ S_DS_GEN
					+ sizeof(MrvlIEtypes_MacAddr_t));
		goto done;
	}
#endif /* WAPI */
#endif /* CONFIG_MLAN_WMSDK */
#ifdef ENABLE_802_11W
		/* IGTK key length is the same as AES key length */
	if (pkey->key_len == WPA_AES_KEY_LEN &&
		!(pkey->key_flags & KEY_FLAG_AES_MCAST_IGTK)) {
#else
	if (pkey->key_len == WPA_AES_KEY_LEN) {
#endif
		PRINTM(MCMND, "WPA_AES\n");
		pkey_material->key_param_set.key_type_id =
			wlan_cpu_to_le16(KEY_TYPE_ID_AES);
		if ( cmd_oid == KEY_INFO_ENABLED)
			pkey_material->key_param_set.key_info =
				wlan_cpu_to_le16(KEY_INFO_AES_ENABLED);
		else
			pkey_material->key_param_set.key_info =
				!(wlan_cpu_to_le16(KEY_INFO_AES_ENABLED));

		if (memcmp(pmpriv->adapter, pkey->mac_addr, bc_mac,
			sizeof(bc_mac))) /* AES pairwise key: unicast */
			pkey_material->key_param_set.key_info |=
					wlan_cpu_to_le16(KEY_INFO_AES_UNICAST);
		else        /* AES group key: multicast */
			pkey_material->key_param_set.key_info |=
					wlan_cpu_to_le16(KEY_INFO_AES_MCAST);
	}
#ifdef ENABLE_802_11W
	else if ((pkey->key_flags & KEY_FLAG_AES_MCAST_IGTK) &&
			pkey->key_len == WPA_IGTK_KEY_LEN) {
		PRINTM(MCMND, "WPA_AES_CMAC\n");
		pkey_material->key_param_set.key_type_id =
					wlan_cpu_to_le16(KEY_TYPE_ID_AES_CMAC);
		if (cmd_oid == KEY_INFO_ENABLED)
			pkey_material->key_param_set.key_info =
					wlan_cpu_to_le16(KEY_INFO_AES_ENABLED);
		else
			pkey_material->key_param_set.key_info =
				!(wlan_cpu_to_le16(KEY_INFO_AES_ENABLED));
			pkey_material->key_param_set.key_info |=
				wlan_cpu_to_le16(KEY_INFO_AES_MCAST_IGTK);
	}
#endif
#ifndef CONFIG_MLAN_WMSDK
	else if (pkey->key_len == WPA_TKIP_KEY_LEN) {
		PRINTM(MCMND, "WPA_TKIP\n");
		pkey_material->key_param_set.key_type_id =
				wlan_cpu_to_le16(KEY_TYPE_ID_TKIP);
		pkey_material->key_param_set.key_info =
				wlan_cpu_to_le16(KEY_INFO_TKIP_ENABLED);

		if (memcmp(pmpriv->adapter, pkey->mac_addr, bc_mac,
			sizeof(bc_mac)))        /* TKIP pairwise key: unicast */
			pkey_material->key_param_set.key_info |=
					wlan_cpu_to_le16(KEY_INFO_TKIP_UNICAST);
		else                       /* TKIP group key: multicast */
			pkey_material->key_param_set.key_info |=
					wlan_cpu_to_le16(KEY_INFO_TKIP_MCAST);
	}
#endif /* CONFIG_MLAN_WMSDK */
	if (pkey_material->key_param_set.key_type_id) {
		pkey_material->key_param_set.type =
					wlan_cpu_to_le16(TLV_TYPE_KEY_MATERIAL);
		pkey_material->key_param_set.key_len =
					wlan_cpu_to_le16(pkey->key_len);
		memcpy(pmpriv->adapter, pkey_material->key_param_set.key,
				pkey->key_material, pkey->key_len);
		pkey_material->key_param_set.length =
				wlan_cpu_to_le16((t_u16)pkey->key_len
				+ KEYPARAMSET_FIXED_LEN);
		key_param_len = (pkey->key_len + KEYPARAMSET_FIXED_LEN)
					+ sizeof(MrvlIEtypesHeader_t);
#ifdef ENABLE_802_11W
		/* key format with pn field is defined in Key Material V1 */
		if (pkey_material->key_param_set.key_type_id ==
				wlan_cpu_to_le16(KEY_TYPE_ID_AES_CMAC)) {
			cmac_param *param;
			param = (cmac_param *)pkey_material->key_param_set.key;
			memcpy(pmpriv->adapter, param->ipn,
				pkey->pn, SEQ_MAX_SIZE);
			memcpy(pmpriv->adapter, param->key,
				pkey->key_material, pkey->key_len);
			pkey_material->key_param_set.key_len =
				wlan_cpu_to_le16((t_u16)sizeof(cmac_param));
			pkey_material->key_param_set.length =
				wlan_cpu_to_le16((t_u16)sizeof(cmac_param)
						+ KEYPARAMSET_FIXED_LEN);
			key_param_len = (t_u16)(sizeof(cmac_param)
						+ KEYPARAMSET_FIXED_LEN)
						+ sizeof(MrvlIEtypesHeader_t);
		}
#endif

		tlv =(MrvlIEtypes_MacAddr_t *)
			((t_u8 *) &pkey_material->key_param_set
					+ key_param_len);
		tlv->header.type = wlan_cpu_to_le16(TLV_TYPE_STA_MAC_ADDRESS);
		tlv->header.len= wlan_cpu_to_le16(MLAN_MAC_ADDR_LENGTH);
		memcpy(pmpriv->adapter, tlv->mac,pkey->mac_addr,
				MLAN_MAC_ADDR_LENGTH);
		cmd->size = wlan_cpu_to_le16(key_param_len
					+ sizeof(pkey_material->action)
					+ S_DS_GEN
					+ sizeof(MrvlIEtypes_MacAddr_t));
	}
done:
	LEAVE();
	return ret;
}
#endif /* KEY_PARAM_SET_V2 */
#endif /* WAPI_AP || HOST_AUTHENTICATOR */

#ifndef CONFIG_MLAN_WMSDK
/**
 *  @brief This function handles the command response of sta_list
 *
 *  @param pmpriv       A pointer to mlan_private structure
 *  @param resp         A pointer to HostCmd_DS_COMMAND
 *  @param pioctl_buf   A pointer to mlan_ioctl_req structure
 *
 *  @return             MLAN_STATUS_SUCCESS
 */
static mlan_status
wlan_uap_ret_sta_list(IN pmlan_private pmpriv,
                      IN HostCmd_DS_COMMAND * resp,
                      IN mlan_ioctl_req * pioctl_buf)
{
    HostCmd_DS_STA_LIST *sta_list =
        (HostCmd_DS_STA_LIST *) & resp->params.sta_list;
    mlan_ds_get_info *info;
    MrvlIEtypes_sta_info_t *tlv = MNULL;
    t_u8 i = 0;

    ENTER();
    if (pioctl_buf) {
        info = (mlan_ds_get_info *) pioctl_buf->pbuf;
        info->param.sta_list.sta_count = wlan_le16_to_cpu(sta_list->sta_count);
        tlv =
            (MrvlIEtypes_sta_info_t *) ((t_u8 *) sta_list +
                                        sizeof(HostCmd_DS_STA_LIST));
        info->param.sta_list.sta_count =
            MIN(info->param.sta_list.sta_count, MAX_NUM_CLIENTS);
        for (i = 0; i < info->param.sta_list.sta_count; i++) {
            memcpy(pmpriv->adapter, info->param.sta_list.info[i].mac_address,
                   tlv->mac_address, MLAN_MAC_ADDR_LENGTH);
            info->param.sta_list.info[i].power_mfg_status =
                tlv->power_mfg_status;
            info->param.sta_list.info[i].rssi = tlv->rssi;
            tlv++;
        }
    }

    LEAVE();
    return MLAN_STATUS_SUCCESS;
}

/**
 *  @brief This function will search for the specific ie
 *
 *
 *  @param priv    A pointer to mlan_private
 *  @param pevent  A pointer to event buf
 *  @param sta_ptr A pointer to sta_node
 *
 *  @return	       N/A
 */
static void
wlan_check_sta_capability(pmlan_private priv, pmlan_buffer pevent,
                          sta_node * sta_ptr)
{
    t_u16 tlv_type, tlv_len;
    t_u16 frame_control, frame_sub_type = 0;
    t_u8 *assoc_req_ie = MNULL;
    t_u8 ie_len = 0, assoc_ie_len = 0;
    IEEEtypes_HTCap_t *pht_cap = MNULL;
    int tlv_buf_left = pevent->data_len - ASSOC_EVENT_FIX_SIZE;
    MrvlIEtypesHeader_t *tlv = (MrvlIEtypesHeader_t *)
        (pevent->pbuf + pevent->data_offset + ASSOC_EVENT_FIX_SIZE);
    MrvlIETypes_MgmtFrameSet_t *mgmt_tlv = MNULL;

    ENTER();
    while (tlv_buf_left >= (int) sizeof(MrvlIEtypesHeader_t)) {
        tlv_type = wlan_le16_to_cpu(tlv->type);
        tlv_len = wlan_le16_to_cpu(tlv->len);
        if ((sizeof(MrvlIEtypesHeader_t) + tlv_len) >
            (unsigned int) tlv_buf_left) {
            PRINTM(MERROR, "wrong tlv: tlvLen=%d, tlvBufLeft=%d\n", tlv_len,
                   tlv_buf_left);
            break;
        }
        if (tlv_type == TLV_TYPE_UAP_MGMT_FRAME) {
            mgmt_tlv = (MrvlIETypes_MgmtFrameSet_t *) tlv;
            memcpy(priv->adapter, &frame_control,
                   (t_u8 *) & (mgmt_tlv->frame_control), sizeof(frame_control));
            frame_sub_type = IEEE80211_GET_FC_MGMT_FRAME_SUBTYPE(frame_control);
            if ((mgmt_tlv->frame_control.type == 0) &&
                ((frame_sub_type == SUBTYPE_ASSOC_REQUEST) ||
                 (frame_sub_type == SUBTYPE_REASSOC_REQUEST))) {

                if (frame_sub_type == SUBTYPE_ASSOC_REQUEST)
                    assoc_ie_len = sizeof(IEEEtypes_AssocRqst_t);
                else if (frame_sub_type == SUBTYPE_REASSOC_REQUEST)
                    assoc_ie_len = sizeof(IEEEtypes_ReAssocRqst_t);

                ie_len = tlv_len - sizeof(IEEEtypes_FrameCtl_t) - assoc_ie_len;
                assoc_req_ie =
                    (t_u8 *) tlv + sizeof(MrvlIETypes_MgmtFrameSet_t) +
                    assoc_ie_len;
                pht_cap =
                    (IEEEtypes_HTCap_t *) wlan_get_specific_ie(priv,
                                                               assoc_req_ie,
                                                               ie_len,
                                                               HT_CAPABILITY);
                if (pht_cap) {
                    PRINTM(MCMND, "STA supports 11n\n");
                    sta_ptr->is_11n_enabled = MTRUE;
                    if (GETHT_MAXAMSDU(pht_cap->ht_cap.ht_cap_info))
                        sta_ptr->max_amsdu = MLAN_TX_DATA_BUF_SIZE_8K;
                    else
                        sta_ptr->max_amsdu = MLAN_TX_DATA_BUF_SIZE_4K;
                } else {
                    PRINTM(MCMND, "STA doesn't support 11n\n");
                }
                break;
            }
        }
        tlv_buf_left -= (sizeof(MrvlIEtypesHeader_t) + tlv_len);
        tlv =
            (MrvlIEtypesHeader_t *) ((t_u8 *) tlv + tlv_len +
                                     sizeof(MrvlIEtypesHeader_t));
    }
    LEAVE();

    return;
}

/** Fixed size of bss start event */
#define BSS_START_EVENT_FIX_SIZE    12

/**
 *  @brief This function will search for the specific ie
 *
 *
 *  @param priv    A pointer to mlan_private
 *  @param pevent  A pointer to event buf
 *
 *  @return	       N/A
 */
static void
wlan_check_uap_capability(pmlan_private priv, pmlan_buffer pevent)
{
    t_u16 tlv_type, tlv_len;
    int tlv_buf_left = pevent->data_len - BSS_START_EVENT_FIX_SIZE;
    MrvlIEtypesHeader_t *tlv =
        (MrvlIEtypesHeader_t *) (pevent->pbuf + pevent->data_offset +
                                 BSS_START_EVENT_FIX_SIZE);
    const t_u8 wmm_oui[4] = { 0x00, 0x50, 0xf2, 0x02 };
    IEEEtypes_WmmParameter_t *pWmmParamIe = MNULL;
    priv->wmm_enabled = MFALSE;
    priv->pkt_fwd = MFALSE;
    priv->is_11n_enabled = MFALSE;

    ENTER();

    while (tlv_buf_left >= (int) sizeof(MrvlIEtypesHeader_t)) {
        tlv_type = wlan_le16_to_cpu(tlv->type);
        tlv_len = wlan_le16_to_cpu(tlv->len);
        if ((sizeof(MrvlIEtypesHeader_t) + tlv_len) >
            (unsigned int) tlv_buf_left) {
            PRINTM(MERROR, "wrong tlv: tlvLen=%d, tlvBufLeft=%d\n", tlv_len,
                   tlv_buf_left);
            break;
        }
        if (tlv_type == HT_CAPABILITY) {
            DBG_HEXDUMP(MCMD_D, "HT_CAP tlv", tlv,
                        tlv_len + sizeof(MrvlIEtypesHeader_t));
            priv->is_11n_enabled = MTRUE;
        }
        if (tlv_type == VENDOR_SPECIFIC_221) {
            if (!memcmp
                (priv->adapter, (t_u8 *) tlv + sizeof(MrvlIEtypesHeader_t),
                 wmm_oui, sizeof(wmm_oui))) {
                DBG_HEXDUMP(MCMD_D, "wmm ie tlv", tlv,
                            tlv_len + sizeof(MrvlIEtypesHeader_t));
                priv->wmm_enabled = MFALSE;
                wlan_wmm_setup_ac_downgrade(priv);
                priv->wmm_enabled = MTRUE;
                pWmmParamIe = (IEEEtypes_WmmParameter_t *) ((t_u8 *) tlv + 2);
                pWmmParamIe->vend_hdr.len = (t_u8) tlv_len;
                pWmmParamIe->vend_hdr.element_id = WMM_IE;
                wlan_wmm_setup_queue_priorities(priv, pWmmParamIe);
            }
        }
        if (tlv_type == TLV_TYPE_UAP_PKT_FWD_CTL) {
            DBG_HEXDUMP(MCMD_D, "pkt_fwd tlv", tlv,
                        tlv_len + sizeof(MrvlIEtypesHeader_t));
            priv->pkt_fwd = *((t_u8 *) tlv + sizeof(MrvlIEtypesHeader_t));
            PRINTM(MCMND, "pkt_fwd FW: 0x%x\n", priv->pkt_fwd);
            if (priv->pkt_fwd & PKT_FWD_FW_BIT)
                priv->pkt_fwd = MFALSE;
            else
                priv->pkt_fwd |= PKT_FWD_ENABLE_BIT;
            PRINTM(MCMND, "pkt_fwd DRV: 0x%x\n", priv->pkt_fwd);
        }
        tlv_buf_left -= (sizeof(MrvlIEtypesHeader_t) + tlv_len);
        tlv =
            (MrvlIEtypesHeader_t *) ((t_u8 *) tlv + tlv_len +
                                     sizeof(MrvlIEtypesHeader_t));
    }
    if (priv->wmm_enabled == MFALSE) {
        /* Since WMM is not enabled, setup the queues with the defaults */
        wlan_wmm_setup_queues(priv);
    }

    LEAVE();
}

/**
 *  @brief This function will update WAPI PN in statation assoc event
 *
 *
 *  @param priv    A pointer to mlan_private
 *  @param pevent  A pointer to event buf
 *
 *  @return	       MFALSE
 */
static t_u32
wlan_update_wapi_info_tlv(pmlan_private priv, pmlan_buffer pevent)
{
    t_u32 ret = MFALSE;
    t_u16 tlv_type, tlv_len;
    t_u32 tx_pn[4];
    t_u32 i = 0;
    int tlv_buf_left = pevent->data_len - ASSOC_EVENT_FIX_SIZE;
    MrvlIEtypesHeader_t *tlv = (MrvlIEtypesHeader_t *)
        (pevent->pbuf + pevent->data_offset + ASSOC_EVENT_FIX_SIZE);
    MrvlIEtypes_wapi_info_t *wapi_tlv = MNULL;

    ENTER();
    while (tlv_buf_left >= (int) sizeof(MrvlIEtypesHeader_t)) {
        tlv_type = wlan_le16_to_cpu(tlv->type);
        tlv_len = wlan_le16_to_cpu(tlv->len);
        if ((sizeof(MrvlIEtypesHeader_t) + tlv_len) >
            (unsigned int) tlv_buf_left) {
            PRINTM(MERROR, "wrong tlv: tlvLen=%d, tlvBufLeft=%d\n", tlv_len,
                   tlv_buf_left);
            break;
        }
        if (tlv_type == TLV_TYPE_AP_WAPI_INFO) {
            wapi_tlv = (MrvlIEtypes_wapi_info_t *) tlv;
            DBG_HEXDUMP(MCMD_D, "Fw:multicast_PN", wapi_tlv->multicast_PN,
                        PN_SIZE);
            memcpy(priv->adapter, (t_u8 *) tx_pn, wapi_tlv->multicast_PN,
                   PN_SIZE);
            for (i = 0; i < 4; i++)
                tx_pn[i] = mlan_ntohl(tx_pn[i]);
            memcpy(priv->adapter, wapi_tlv->multicast_PN, (t_u8 *) tx_pn,
                   PN_SIZE);
            DBG_HEXDUMP(MCMD_D, "Host:multicast_PN", wapi_tlv->multicast_PN,
                        PN_SIZE);
            break;
        }
        tlv_buf_left -= (sizeof(MrvlIEtypesHeader_t) + tlv_len);
        tlv =
            (MrvlIEtypesHeader_t *) ((t_u8 *) tlv + tlv_len +
                                     sizeof(MrvlIEtypesHeader_t));
    }
    LEAVE();

    return ret;
}

/**
 *  @brief This function send sta_assoc_event to moal
 *  	   payload with sta mac address and assoc ie.
 *
 *  @param priv    A pointer to mlan_private
 *  @param pevent  A pointer to mlan_event buffer
 *  @param pbuf    A pointer to mlan_buffer which has event content.
 *
 *  @return	       MFALSE
 */
static t_u32
wlan_process_sta_assoc_event(pmlan_private priv, mlan_event * pevent,
                             pmlan_buffer pmbuf)
{
    t_u32 ret = MFALSE;
    t_u16 tlv_type, tlv_len;
    t_u16 frame_control, frame_sub_type = 0;
    t_u8 *assoc_req_ie = MNULL;
    t_u8 ie_len = 0, assoc_ie_len = 0;
    int tlv_buf_left = pmbuf->data_len - ASSOC_EVENT_FIX_SIZE;
    MrvlIEtypesHeader_t *tlv = (MrvlIEtypesHeader_t *)
        (pmbuf->pbuf + pmbuf->data_offset + ASSOC_EVENT_FIX_SIZE);
    MrvlIETypes_MgmtFrameSet_t *mgmt_tlv = MNULL;

    ENTER();
    pevent->event_id = MLAN_EVENT_ID_UAP_FW_STA_CONNECT;
    pevent->bss_index = priv->bss_index;
    pevent->event_len = MLAN_MAC_ADDR_LENGTH;
    memcpy(priv->adapter, pevent->event_buf,
           pmbuf->pbuf + pmbuf->data_offset + 6, pevent->event_len);
    while (tlv_buf_left >= (int) sizeof(MrvlIEtypesHeader_t)) {
        tlv_type = wlan_le16_to_cpu(tlv->type);
        tlv_len = wlan_le16_to_cpu(tlv->len);
        if ((sizeof(MrvlIEtypesHeader_t) + tlv_len) >
            (unsigned int) tlv_buf_left) {
            PRINTM(MERROR, "wrong tlv: tlvLen=%d, tlvBufLeft=%d\n", tlv_len,
                   tlv_buf_left);
            break;
        }
        if (tlv_type == TLV_TYPE_UAP_MGMT_FRAME) {
            mgmt_tlv = (MrvlIETypes_MgmtFrameSet_t *) tlv;
            memcpy(priv->adapter, &frame_control,
                   (t_u8 *) & (mgmt_tlv->frame_control), sizeof(frame_control));
            frame_sub_type = IEEE80211_GET_FC_MGMT_FRAME_SUBTYPE(frame_control);
            if ((mgmt_tlv->frame_control.type == 0) &&
                ((frame_sub_type == SUBTYPE_ASSOC_REQUEST) ||
                 (frame_sub_type == SUBTYPE_REASSOC_REQUEST))) {

                if (frame_sub_type == SUBTYPE_ASSOC_REQUEST)
                    assoc_ie_len = sizeof(IEEEtypes_AssocRqst_t);
                else if (frame_sub_type == SUBTYPE_REASSOC_REQUEST)
                    assoc_ie_len = sizeof(IEEEtypes_ReAssocRqst_t);

                ie_len = tlv_len - sizeof(IEEEtypes_FrameCtl_t) - assoc_ie_len;
                assoc_req_ie =
                    (t_u8 *) tlv + sizeof(MrvlIETypes_MgmtFrameSet_t) +
                    assoc_ie_len;
                memcpy(priv->adapter, pevent->event_buf + pevent->event_len,
                       assoc_req_ie, ie_len);
                pevent->event_len += ie_len;
                break;
            }
        }
        tlv_buf_left -= (sizeof(MrvlIEtypesHeader_t) + tlv_len);
        tlv =
            (MrvlIEtypesHeader_t *) ((t_u8 *) tlv + tlv_len +
                                     sizeof(MrvlIEtypesHeader_t));
    }
    PRINTM(MEVENT, "STA assoc event len=%d\n", pevent->event_len);
    DBG_HEXDUMP(MCMD_D, "STA assoc event", pevent->event_buf,
                pevent->event_len);
    wlan_recv_event(priv, pevent->event_id, pevent);
    LEAVE();
    return ret;
}
#endif /* CONFIG_MLAN_WMSDK */

/********************************************************
    Global Functions
********************************************************/
/**
 *  @brief This function prepare the command before sending to firmware.
 *
 *  @param priv       A pointer to mlan_private structure
 *  @param cmd_no       Command number
 *  @param cmd_action   Command action: GET or SET
 *  @param cmd_oid      Cmd oid: treated as sub command
 *  @param pioctl_buf   A pointer to MLAN IOCTL Request buffer
 *  @param pdata_buf    A pointer to information buffer
 *  @param pcmd_buf      A pointer to cmd buf
 *
 *  @return             MLAN_STATUS_SUCCESS or MLAN_STATUS_FAILURE
 */
mlan_status
wlan_ops_uap_prepare_cmd(IN t_void * priv,
                         IN t_u16 cmd_no,
                         IN t_u16 cmd_action,
                         IN t_u32 cmd_oid,
                         IN t_void * pioctl_buf,
                         IN t_void * pdata_buf, IN t_void * pcmd_buf)
{
    HostCmd_DS_COMMAND *cmd_ptr = (HostCmd_DS_COMMAND *) pcmd_buf;
    mlan_private *pmpriv = (mlan_private *) priv;
    mlan_status ret = MLAN_STATUS_SUCCESS;
    pmlan_ioctl_req pioctl_req = (mlan_ioctl_req *) pioctl_buf;

    ENTER();

    /* Prepare command */
    switch (cmd_no) {
    case HostCmd_CMD_SOFT_RESET:
    case HOST_CMD_APCMD_BSS_STOP:
    case HOST_CMD_APCMD_BSS_START:
    case HOST_CMD_APCMD_SYS_INFO:
    case HOST_CMD_APCMD_SYS_RESET:
    case HOST_CMD_APCMD_STA_LIST:
        cmd_ptr->command = wlan_cpu_to_le16(cmd_no);
        cmd_ptr->size = wlan_cpu_to_le16(S_DS_GEN);
        break;
    case HOST_CMD_APCMD_SYS_CONFIGURE:
        ret = wlan_uap_cmd_sys_configure(pmpriv, cmd_ptr, cmd_action,
                                         (pmlan_ioctl_req) pioctl_buf,
                                         pdata_buf);
        break;
#ifndef CONFIG_MLAN_WMSDK
    case HostCmd_CMD_802_11_PS_MODE_ENH:
        ret =
            wlan_cmd_enh_power_mode(pmpriv, cmd_ptr, cmd_action,
                                    (t_u16) cmd_oid, pdata_buf);
        break;
    case HostCmd_CMD_SDIO_GPIO_INT_CONFIG:
        ret = wlan_cmd_sdio_gpio_int(pmpriv, cmd_ptr, cmd_action, pdata_buf);
        break;
    case HostCmd_CMD_FUNC_INIT:
        if (pmpriv->adapter->hw_status == WlanHardwareStatusReset)
            pmpriv->adapter->hw_status = WlanHardwareStatusInitializing;
        cmd_ptr->command = wlan_cpu_to_le16(cmd_no);
        cmd_ptr->size = wlan_cpu_to_le16(S_DS_GEN);
        break;
    case HostCmd_CMD_FUNC_SHUTDOWN:
        pmpriv->adapter->hw_status = WlanHardwareStatusReset;
        cmd_ptr->command = wlan_cpu_to_le16(cmd_no);
        cmd_ptr->size = wlan_cpu_to_le16(S_DS_GEN);
        break;
    case HostCmd_CMD_CFG_DATA:
        ret = wlan_cmd_cfg_data(pmpriv, cmd_ptr, cmd_action, pdata_buf);
        break;
#endif /* CONFIG_MLAN_WMSDK */
    case HostCmd_CMD_802_11_SNMP_MIB:
        ret = wlan_uap_cmd_snmp_mib(pmpriv, cmd_ptr, cmd_action, cmd_oid,
                                    (pmlan_ioctl_req) pioctl_buf, pdata_buf);
        break;
    case HostCmd_CMD_802_11D_DOMAIN_INFO:
	if (pmpriv->support_11d_APIs)
		ret = pmpriv->support_11d_APIs->
		wlan_cmd_802_11d_domain_info_p(pmpriv, cmd_ptr, cmd_action);
        break;
#ifndef CONFIG_MLAN_WMSDK
    case HostCmd_CMD_CHAN_REPORT_REQUEST:
        ret = wlan_11h_cmd_process(pmpriv, cmd_ptr, pdata_buf);
        break;
#endif /* CONFIG_MLAN_WMSDK */
    case HOST_CMD_APCMD_STA_DEAUTH:
        ret = wlan_uap_cmd_sta_deauth(pmpriv, cmd_ptr, pdata_buf);
        break;
#ifndef CONFIG_MLAN_WMSDK
#ifdef HOST_AUTHENTICATOR
    case HOST_CMD_APCMD_REPORT_MIC:
        ret = wlan_uap_cmd_report_mic(pmpriv, cmd_ptr, pdata_buf);
        break;
#endif
#endif /* CONFIG_MLAN_WMSDK */
#if defined(WAPI_AP) || defined(HOST_AUTHENTICATOR)
    case HostCmd_CMD_802_11_KEY_MATERIAL:
        ret = wlan_uap_cmd_key_material(pmpriv, cmd_ptr, cmd_action,
					cmd_oid, pdata_buf);
        break;
#endif
#ifndef CONFIG_MLAN_WMSDK
    case HostCmd_CMD_GET_HW_SPEC:
        ret = wlan_cmd_get_hw_spec(pmpriv, cmd_ptr);
        break;
    case HostCmd_CMD_802_11_HS_CFG_ENH:
        ret =
            wlan_uap_cmd_802_11_hs_cfg(pmpriv, cmd_ptr, cmd_action,
                                       (hs_config_param *) pdata_buf);
        break;
    case HostCmd_CMD_RECONFIGURE_TX_BUFF:
        ret = wlan_cmd_recfg_tx_buf(pmpriv, cmd_ptr, cmd_action, pdata_buf);
        break;
    case HostCmd_CMD_AMSDU_AGGR_CTRL:
        ret = wlan_cmd_amsdu_aggr_ctrl(pmpriv, cmd_ptr, cmd_action, pdata_buf);
        break;
#endif
    case HostCmd_CMD_11N_ADDBA_REQ:
        ret = wlan_cmd_11n_addba_req(pmpriv, cmd_ptr, pdata_buf);
        break;
    case HostCmd_CMD_11N_DELBA:
        ret = wlan_cmd_11n_delba(pmpriv, cmd_ptr, pdata_buf);
        break;
#ifndef CONFIG_MLAN_WMSDK
    case HostCmd_CMD_11N_ADDBA_RSP:
        ret = wlan_cmd_11n_addba_rspgen(pmpriv, cmd_ptr, pdata_buf);
        break;
#if defined(WIFI_DIRECT_SUPPORT)
    case HostCmd_CMD_SET_BSS_MODE:
        cmd_ptr->command = wlan_cpu_to_le16(cmd_no);
        if (pdata_buf)
            cmd_ptr->params.bss_mode.con_type = *(t_u8 *) pdata_buf;
        else
            cmd_ptr->params.bss_mode.con_type = BSS_MODE_WIFIDIRECT_GO;
        cmd_ptr->size =
            wlan_cpu_to_le16(sizeof(HostCmd_DS_SET_BSS_MODE) + S_DS_GEN);
        ret = MLAN_STATUS_SUCCESS;
        break;
#endif
    case HostCmd_CMD_VERSION_EXT:
        cmd_ptr->command = wlan_cpu_to_le16(cmd_no);
        cmd_ptr->params.verext.version_str_sel =
            (t_u8) (*((t_u32 *) pdata_buf));
        cmd_ptr->size =
            wlan_cpu_to_le16(sizeof(HostCmd_DS_VERSION_EXT) + S_DS_GEN);
        ret = MLAN_STATUS_SUCCESS;
        break;
    case HostCmd_CMD_RX_MGMT_IND:
        cmd_ptr->command = wlan_cpu_to_le16(cmd_no);
        cmd_ptr->params.rx_mgmt_ind.action = wlan_cpu_to_le16(cmd_action);
        cmd_ptr->params.rx_mgmt_ind.mgmt_subtype_mask =
            (t_u32) (*((t_u32 *) pdata_buf));
        cmd_ptr->size = wlan_cpu_to_le16(sizeof(t_u32) + S_DS_GEN);
        break;
    case HostCmd_CMD_CFG_TX_DATA_PAUSE:
        ret = wlan_uap_cmd_txdatapause(pmpriv, cmd_ptr, cmd_action, pdata_buf);
        break;
    case HostCmd_CMD_802_11_RADIO_CONTROL:
        ret =
            wlan_cmd_802_11_radio_control(pmpriv, cmd_ptr, cmd_action,
                                          pdata_buf);
        break;
    case HostCmd_CMD_TX_RATE_CFG:
        ret = wlan_cmd_tx_rate_cfg(pmpriv, cmd_ptr, cmd_action, pdata_buf);
        break;
    case HostCmd_CMD_802_11_TX_RATE_QUERY:
        cmd_ptr->command = wlan_cpu_to_le16(HostCmd_CMD_802_11_TX_RATE_QUERY);
        cmd_ptr->size =
            wlan_cpu_to_le16(sizeof(HostCmd_TX_RATE_QUERY) + S_DS_GEN);
        pmpriv->tx_rate = 0;
        ret = MLAN_STATUS_SUCCESS;
        break;
#ifdef WIFI_DIRECT_SUPPORT
    case HostCmd_CMD_802_11_REMAIN_ON_CHANNEL:
        ret =
            wlan_cmd_remain_on_channel(pmpriv, cmd_ptr, cmd_action, pdata_buf);
        break;
    case HOST_CMD_WIFI_DIRECT_MODE_CONFIG:
        ret = wlan_cmd_wifi_direct_mode(pmpriv, cmd_ptr, cmd_action, pdata_buf);
        break;
#endif
#endif /* CONFIG_MLAN_WMSDK */
    default:
        PRINTM(MERROR, "PREP_CMD: unknown command- %#x\n", cmd_no);
        if (pioctl_req)
            pioctl_req->status_code = MLAN_ERROR_CMD_INVALID;
        ret = MLAN_STATUS_FAILURE;
        break;
    }
    LEAVE();
    return ret;
}

#ifndef CONFIG_MLAN_WMSDK
/**
 *  @brief This function handles the AP mode command response
 *
 *  @param priv             A pointer to mlan_private structure
 *  @param cmdresp_no       cmd no
 *  @param pcmd_buf         cmdresp buf
 *  @param pioctl           A pointer to ioctl buf
 *
 *  @return             MLAN_STATUS_SUCCESS or MLAN_STATUS_FAILURE
 */
mlan_status
wlan_ops_uap_process_cmdresp(IN t_void * priv,
                             IN t_u16 cmdresp_no,
                             IN t_void * pcmd_buf, IN t_void * pioctl)
{
    mlan_status ret = MLAN_STATUS_SUCCESS;
    mlan_private *pmpriv = (mlan_private *) priv;
    HostCmd_DS_COMMAND *resp = (HostCmd_DS_COMMAND *) pcmd_buf;
    mlan_ioctl_req *pioctl_buf = (mlan_ioctl_req *) pioctl;
    mlan_adapter *pmadapter = pmpriv->adapter;
    int ctr;

    ENTER();

    /* If the command is not successful, cleanup and return failure */
    if (resp->result != HostCmd_RESULT_OK) {
        uap_process_cmdresp_error(pmpriv, resp, pioctl_buf);
        LEAVE();
        return MLAN_STATUS_FAILURE;
    }

    /* Command successful, handle response */
    switch (cmdresp_no) {
    case HOST_CMD_APCMD_BSS_STOP:
        pmpriv->uap_bss_started = MFALSE;
        wlan_11h_check_update_radar_det_state(pmpriv);

        if (pmpriv->adapter->state_rdh.stage == RDH_STOP_INTFS)
            wlan_11h_radar_detected_callback((t_void *) pmpriv);
        break;
    case HOST_CMD_APCMD_BSS_START:
        if (pmpriv->adapter->state_rdh.stage == RDH_RESTART_INTFS)
            wlan_11h_radar_detected_callback((t_void *) pmpriv);
        break;
    case HOST_CMD_APCMD_SYS_RESET:
        ret = wlan_uap_ret_sys_reset(pmpriv, resp, pioctl_buf);

        pmpriv->uap_bss_started = MFALSE;
        wlan_11h_check_update_radar_det_state(pmpriv);
        break;
    case HOST_CMD_APCMD_SYS_INFO:
        break;
    case HOST_CMD_APCMD_SYS_CONFIGURE:
        ret = wlan_uap_ret_sys_config(pmpriv, resp, pioctl_buf);
        break;
    case HostCmd_CMD_802_11_PS_MODE_ENH:
        ret = wlan_ret_enh_power_mode(pmpriv, resp, pioctl_buf);
        break;
    case HostCmd_CMD_SDIO_GPIO_INT_CONFIG:
        break;
    case HostCmd_CMD_FUNC_INIT:
    case HostCmd_CMD_FUNC_SHUTDOWN:
        break;
    case HostCmd_CMD_802_11_SNMP_MIB:
        ret = wlan_uap_ret_snmp_mib(pmpriv, resp, pioctl_buf);
        break;
    case HostCmd_CMD_802_11D_DOMAIN_INFO:
        ret = wlan_ret_802_11d_domain_info(pmpriv, resp);
        break;
    case HostCmd_CMD_CHAN_REPORT_REQUEST:
        ret = wlan_11h_cmdresp_process(pmpriv, resp);
        break;
    case HOST_CMD_APCMD_STA_DEAUTH:
        break;
    case HostCmd_CMD_802_11_KEY_MATERIAL:
        break;
    case HOST_CMD_APCMD_STA_LIST:
        ret = wlan_uap_ret_sta_list(pmpriv, resp, pioctl_buf);
        break;
    case HostCmd_CMD_GET_HW_SPEC:
        ret = wlan_ret_get_hw_spec(pmpriv, resp, pioctl_buf);
        break;
    case HostCmd_CMD_CFG_DATA:
        ret = wlan_ret_cfg_data(pmpriv, resp, pioctl_buf);
        break;
    case HostCmd_CMD_802_11_HS_CFG_ENH:
        ret = wlan_ret_802_11_hs_cfg(pmpriv, resp, pioctl_buf);
        break;
    case HostCmd_CMD_11N_ADDBA_REQ:
        ret = wlan_ret_11n_addba_req(pmpriv, resp);
        break;
    case HostCmd_CMD_11N_DELBA:
        ret = wlan_ret_11n_delba(pmpriv, resp);
        break;
    case HostCmd_CMD_11N_ADDBA_RSP:
        ret = wlan_ret_11n_addba_resp(pmpriv, resp);
        break;
    case HostCmd_CMD_SET_BSS_MODE:
        break;
    case HostCmd_CMD_RECONFIGURE_TX_BUFF:
        pmadapter->tx_buf_size =
            (t_u16) wlan_le16_to_cpu(resp->params.tx_buf.buff_size);
        pmadapter->tx_buf_size =
            (pmadapter->tx_buf_size / MLAN_SDIO_BLOCK_SIZE) *
            MLAN_SDIO_BLOCK_SIZE;
        pmadapter->curr_tx_buf_size = pmadapter->tx_buf_size;
        pmadapter->mp_end_port =
            wlan_le16_to_cpu(resp->params.tx_buf.mp_end_port);
        pmadapter->mp_data_port_mask = DATA_PORT_MASK;

        for (ctr = 1; ctr <= MAX_PORT - pmadapter->mp_end_port; ctr++) {
            pmadapter->mp_data_port_mask &= ~(1 << (MAX_PORT - ctr));
        }
        pmadapter->curr_wr_port = 1;
        PRINTM(MCMND, "end port %d, data port mask %x\n",
               wlan_le16_to_cpu(resp->params.tx_buf.mp_end_port),
               pmadapter->mp_data_port_mask);
        PRINTM(MCMND, "max_tx_buf_size=%d, tx_buf_size=%d\n",
               pmadapter->max_tx_buf_size, pmadapter->tx_buf_size);
        break;
    case HostCmd_CMD_AMSDU_AGGR_CTRL:
        ret = wlan_ret_amsdu_aggr_ctrl(pmpriv, resp, pioctl_buf);
        break;
    case HostCmd_CMD_VERSION_EXT:
        ret = wlan_ret_ver_ext(pmpriv, resp, pioctl_buf);
        break;
    case HostCmd_CMD_RX_MGMT_IND:
        ret = wlan_ret_rx_mgmt_ind(pmpriv, resp, pioctl_buf);
        break;
    case HostCmd_CMD_CFG_TX_DATA_PAUSE:
        ret = wlan_uap_ret_txdatapause(pmpriv, resp, pioctl_buf);
        break;
    case HostCmd_CMD_802_11_RADIO_CONTROL:
        ret = wlan_ret_802_11_radio_control(pmpriv, resp, pioctl_buf);
        break;
    case HostCmd_CMD_TX_RATE_CFG:
        ret = wlan_ret_tx_rate_cfg(pmpriv, resp, pioctl_buf);
        break;
    case HostCmd_CMD_802_11_TX_RATE_QUERY:
        ret = wlan_ret_802_11_tx_rate_query(pmpriv, resp, pioctl_buf);
        break;
#ifdef WIFI_DIRECT_SUPPORT
    case HostCmd_CMD_802_11_REMAIN_ON_CHANNEL:
        ret = wlan_ret_remain_on_channel(pmpriv, resp, pioctl_buf);
        break;
    case HOST_CMD_WIFI_DIRECT_MODE_CONFIG:
        ret = wlan_ret_wifi_direct_mode(pmpriv, resp, pioctl_buf);
        break;
#endif
    default:
        PRINTM(MERROR, "CMD_RESP: Unknown command response %#x\n",
               resp->command);
        if (pioctl_buf)
            pioctl_buf->status_code = MLAN_ERROR_CMD_RESP_FAIL;
        break;
    }
    LEAVE();
    return ret;
}

/**
 *  @brief This function handles events generated by firmware
 *
 *  @param priv		A pointer to mlan_private structure
 *
 *  @return		MLAN_STATUS_SUCCESS or MLAN_STATUS_FAILURE
 */
mlan_status
wlan_ops_uap_process_event(IN t_void * priv)
{
    pmlan_private pmpriv = (pmlan_private) priv;
    pmlan_adapter pmadapter = pmpriv->adapter;
    pmlan_callbacks pcb = &pmadapter->callbacks;
    mlan_status ret = MLAN_STATUS_SUCCESS;
    t_u32 eventcause = pmadapter->event_cause;
    pmlan_buffer pmbuf = pmadapter->pmlan_buffer_event;
    t_u8 *event_buf = MNULL;
    mlan_event *pevent = MNULL;
    t_u8 sta_addr[MLAN_MAC_ADDR_LENGTH];
    sta_node *sta_ptr = MNULL;
    t_u8 i = 0;

    ENTER();

    /* Allocate memory for event buffer */
    ret =
        pcb->moal_malloc(pmadapter->pmoal_handle, MAX_EVENT_SIZE, MLAN_MEM_DEF,
                         &event_buf);
    if ((ret != MLAN_STATUS_SUCCESS) || !event_buf) {
        PRINTM(MERROR, "Could not allocate buffer for event buf\n");
        if (pmbuf)
            pmbuf->status_code = MLAN_ERROR_NO_MEM;
        goto done;
    }
    pevent = (pmlan_event) event_buf;
    memset(pmadapter, &pevent->event_id, 0, sizeof(pevent->event_id));

    if (eventcause != EVENT_PS_SLEEP && eventcause != EVENT_PS_AWAKE &&
        pmbuf->data_len > sizeof(eventcause))
        DBG_HEXDUMP(MEVT_D, "EVENT", pmbuf->pbuf + pmbuf->data_offset,
                    pmbuf->data_len);

    switch (eventcause) {
    case EVENT_MICRO_AP_BSS_START:
        PRINTM(MEVENT, "EVENT: MICRO_AP_BSS_START\n");
        pmpriv->uap_bss_started = MTRUE;
        memcpy(pmadapter, pmpriv->curr_addr, pmadapter->event_body + 2,
               MLAN_MAC_ADDR_LENGTH);
        pevent->event_id = MLAN_EVENT_ID_UAP_FW_BSS_START;
        wlan_check_uap_capability(pmpriv, pmbuf);
        break;
    case EVENT_MICRO_AP_BSS_ACTIVE:
        PRINTM(MEVENT, "EVENT: MICRO_AP_BSS_ACTIVE\n");
        pmpriv->media_connected = MTRUE;
        pevent->event_id = MLAN_EVENT_ID_UAP_FW_BSS_ACTIVE;
        break;
    case EVENT_MICRO_AP_BSS_IDLE:
        PRINTM(MEVENT, "EVENT: MICRO_AP_BSS_IDLE\n");
        pevent->event_id = MLAN_EVENT_ID_UAP_FW_BSS_IDLE;
        pmpriv->media_connected = MFALSE;
        wlan_clean_txrx(pmpriv);
        wlan_delete_station_list(pmpriv);
        break;
    case EVENT_PS_AWAKE:
        PRINTM(MINFO, "EVENT: AWAKE \n");
        PRINTM(MEVENT, "||");
        /* Handle unexpected PS AWAKE event */
        if (pmadapter->ps_state == PS_STATE_SLEEP_CFM)
            break;
        pmadapter->pm_wakeup_card_req = MFALSE;
        pmadapter->pm_wakeup_fw_try = MFALSE;
        pmadapter->ps_state = PS_STATE_AWAKE;
        break;
    case EVENT_PS_SLEEP:
        PRINTM(MINFO, "EVENT: SLEEP\n");
        PRINTM(MEVENT, "__");
        /* Handle unexpected PS SLEEP event */
        if (pmadapter->ps_state == PS_STATE_SLEEP_CFM)
            break;
        pmadapter->ps_state = PS_STATE_PRE_SLEEP;
        wlan_check_ps_cond(pmadapter);
        break;
    case EVENT_MICRO_AP_STA_ASSOC:
        PRINTM(MEVENT, "EVENT: MICRO_AP_STA_ASSOC\n");
        wlan_process_sta_assoc_event(pmpriv, pevent, pmbuf);
        memcpy(pmadapter, sta_addr, pmadapter->event_body + 2,
               MLAN_MAC_ADDR_LENGTH);
        sta_ptr = wlan_add_station_entry(pmpriv, sta_addr);
        if (pmpriv->is_11n_enabled) {
            wlan_check_sta_capability(pmpriv, pmbuf, sta_ptr);
            for (i = 0; i < MAX_NUM_TID; i++) {
                if (sta_ptr->is_11n_enabled)
                    sta_ptr->ampdu_sta[i] = pmpriv->aggr_prio_tbl[i].ampdu_user;
                else
                    sta_ptr->ampdu_sta[i] = BA_STREAM_NOT_ALLOWED;
            }
            memset(pmadapter, sta_ptr->rx_seq, 0xff, sizeof(sta_ptr->rx_seq));
        }
        if (pmpriv->sec_info.wapi_enabled)
            wlan_update_wapi_info_tlv(pmpriv, pmbuf);
        pevent->event_id = MLAN_EVENT_ID_DRV_PASSTHRU;
        break;
    case EVENT_MICRO_AP_STA_DEAUTH:
        PRINTM(MEVENT, "EVENT: MICRO_AP_STA_DEAUTH\n");
        pevent->event_id = MLAN_EVENT_ID_UAP_FW_STA_DISCONNECT;
        pevent->bss_index = pmpriv->bss_index;
        pevent->event_len = pmbuf->data_len - 4;
        /* skip event length field */
        memcpy(pmadapter, (t_u8 *) pevent->event_buf,
               pmbuf->pbuf + pmbuf->data_offset + 4, pevent->event_len);
        wlan_recv_event(pmpriv, pevent->event_id, pevent);
        memcpy(pmadapter, sta_addr, pmadapter->event_body + 2,
               MLAN_MAC_ADDR_LENGTH);
        if (pmpriv->is_11n_enabled) {
            wlan_cleanup_reorder_tbl(pmpriv, sta_addr);
            pmadapter->callbacks.moal_spin_lock(pmadapter->pmoal_handle,
                                                pmpriv->wmm.ra_list_spinlock);
            wlan_11n_cleanup_txbastream_tbl(pmpriv, sta_addr);
            pmadapter->callbacks.moal_spin_unlock(pmadapter->pmoal_handle,
                                                  pmpriv->wmm.ra_list_spinlock);
        }
        wlan_wmm_delete_peer_ralist(pmpriv, sta_addr);
        wlan_delete_station_entry(pmpriv, sta_addr);
        pevent->event_id = MLAN_EVENT_ID_DRV_PASSTHRU;
        break;
    case EVENT_HS_ACT_REQ:
        PRINTM(MEVENT, "EVENT: HS_ACT_REQ\n");
        ret =
            wlan_prepare_cmd(pmpriv, HostCmd_CMD_802_11_HS_CFG_ENH, 0, 0, MNULL,
                             MNULL);
        break;
    case EVENT_ADDBA:
        PRINTM(MEVENT, "EVENT: ADDBA Request\n");
        wlan_prepare_cmd(pmpriv, HostCmd_CMD_11N_ADDBA_RSP,
                         HostCmd_ACT_GEN_SET, 0, MNULL, pmadapter->event_body);
        break;
    case EVENT_DELBA:
        PRINTM(MEVENT, "EVENT: DELBA Request\n");
        wlan_11n_delete_bastream(pmpriv, pmadapter->event_body);
        break;
    case EVENT_BA_STREAM_TIMEOUT:
        PRINTM(MEVENT, "EVENT:  BA Stream timeout\n");
        wlan_11n_ba_stream_timeout(pmpriv,
                                   (HostCmd_DS_11N_BATIMEOUT *) pmadapter->
                                   event_body);
        break;
    case EVENT_RXBA_SYNC:
        PRINTM(MEVENT, "EVENT:  RXBA_SYNC\n");
        wlan_11n_rxba_sync_event(pmpriv, pmadapter->event_body,
                                 pmbuf->data_len - sizeof(eventcause));
        break;
    case EVENT_AMSDU_AGGR_CTRL:
        PRINTM(MEVENT, "EVENT:  AMSDU_AGGR_CTRL %d\n",
               *(t_u16 *) pmadapter->event_body);
        pmadapter->tx_buf_size =
            MIN(pmadapter->curr_tx_buf_size,
                wlan_le16_to_cpu(*(t_u16 *) pmadapter->event_body));
        PRINTM(MEVENT, "tx_buf_size %d\n", pmadapter->tx_buf_size);
        break;
    case EVENT_TX_DATA_PAUSE:
        PRINTM(MEVENT, "EVENT: TX_DATA_PAUSE\n");
        wlan_process_tx_pause_event(priv, pmbuf);
        break;
    case EVENT_RADAR_DETECTED:
        PRINTM(MEVENT, "EVENT: Radar Detected\n");

        /* Send as passthru first, this event can cause other events */
        memset(pmadapter, event_buf, 0x00, MAX_EVENT_SIZE);
        pevent->bss_index = pmpriv->bss_index;
        pevent->event_id = MLAN_EVENT_ID_DRV_PASSTHRU;
        pevent->event_len = pmbuf->data_len;
        memcpy(pmadapter, (t_u8 *) pevent->event_buf,
               pmbuf->pbuf + pmbuf->data_offset, pevent->event_len);
        wlan_recv_event(pmpriv, pevent->event_id, pevent);
        pevent->event_id = 0;   // clear to avoid resending at end of fcn

        if (pmadapter->state_rdh.stage == RDH_OFF) {
            pmadapter->state_rdh.stage = RDH_CHK_INTFS;
            wlan_11h_radar_detected_handling(pmadapter);
        } else {
            PRINTM(MEVENT, "Ignore Event Radar Detected - handling"
                   " already in progress.\n");
        }
        break;
    case EVENT_CHANNEL_REPORT_RDY:
        PRINTM(MEVENT, "EVENT: Channel Report Ready\n");
        memset(pmadapter, event_buf, 0x00, MAX_EVENT_SIZE);
        /* Setup event buffer */
        pevent->bss_index = pmpriv->bss_index;
        pevent->event_id = MLAN_EVENT_ID_FW_CHANNEL_REPORT_RDY;
        pevent->event_len = pmbuf->data_len - sizeof(eventcause);
        /* Copy event data */
        memcpy(pmadapter, (t_u8 *) pevent->event_buf,
               pmbuf->pbuf + pmbuf->data_offset + sizeof(eventcause),
               pevent->event_len);
        /* Handle / pass event data, and free buffer */
        ret = wlan_11h_handle_event_chanrpt_ready(pmpriv, pevent);

        /* Send up this Event to unblock MOAL waitqueue */
        wlan_recv_event(pmpriv, MLAN_EVENT_ID_DRV_MEAS_REPORT, MNULL);
        pevent->event_id = MLAN_EVENT_ID_DRV_PASSTHRU;
        break;
#ifdef WIFI_DIRECT_SUPPORT
    case EVENT_REMAIN_ON_CHANNEL_EXPIRED:
        PRINTM(MEVENT, "EVENT: REMAIN_ON_CHANNEL_EXPIRED reason=%d\n",
               *(t_u16 *) pmadapter->event_body);
        pevent->event_id = MLAN_EVENT_ID_FW_REMAIN_ON_CHAN_EXPIRED;
        break;
#endif
    default:
        pevent->event_id = MLAN_EVENT_ID_DRV_PASSTHRU;
        break;
    }

    if (pevent->event_id) {
        pevent->bss_index = pmpriv->bss_index;
        pevent->event_len = pmbuf->data_len;
        memcpy(pmadapter, (t_u8 *) pevent->event_buf,
               pmbuf->pbuf + pmbuf->data_offset, pevent->event_len);
        wlan_recv_event(pmpriv, pevent->event_id, pevent);
    }
  done:
    if (event_buf)
        pcb->moal_mfree(pmadapter->pmoal_handle, event_buf);
    LEAVE();
    return ret;
}

/**
 *  @brief  This function issues commands to initialize firmware
 *
 *  @param priv     	A pointer to mlan_private structure
 *  @param first_bss	flag for first BSS
 *
 *  @return		MLAN_STATUS_SUCCESS or MLAN_STATUS_PENDING or MLAN_STATUS_FAILURE
 */
mlan_status
wlan_ops_uap_init_cmd(IN t_void * priv, IN t_u8 first_bss)
{
    mlan_status ret = MLAN_STATUS_SUCCESS;
    pmlan_private pmpriv = (pmlan_private) priv;
    t_u16 last_cmd = 0;

    ENTER();

    if (first_bss) {
        if (wlan_adapter_init_cmd(pmpriv->adapter) == MLAN_STATUS_FAILURE) {
            ret = MLAN_STATUS_FAILURE;
            goto done;
        }
    }

    ret = wlan_prepare_cmd(pmpriv, HOST_CMD_APCMD_SYS_CONFIGURE,
                           HostCmd_ACT_GEN_GET, 0, MNULL, MNULL);
    if (ret) {
        ret = MLAN_STATUS_FAILURE;
        goto done;
    }
    last_cmd = HOST_CMD_APCMD_SYS_CONFIGURE;
    /** set last_init_cmd */
    if (last_cmd) {
        pmpriv->adapter->last_init_cmd = last_cmd;
        ret = MLAN_STATUS_PENDING;
    }
  done:
    LEAVE();
    return ret;
}

#endif /* CONFIG_MLAN_WMSDK */
