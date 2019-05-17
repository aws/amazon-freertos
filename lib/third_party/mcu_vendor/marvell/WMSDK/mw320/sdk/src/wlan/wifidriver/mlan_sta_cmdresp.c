/** @file mlan_sta_cmdresp.c
 *
 *  @brief  This file provides the handling of command
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


/******************************************************
Change log:
    10/21/2008: initial version
******************************************************/

#include <mlan_wmsdk.h>

/* Additional WMSDK header files */
#include <wmstdio.h>
#include <wmassert.h>
#include <wmerrno.h>
#include <wm_os.h>

/* Always keep this include at the end of all include files */
#include <mlan_remap_mem_operations.h>

void dump_rf_channel_info(HostCmd_DS_802_11_RF_CHANNEL *prf_channel);

/********************************************************
                Local Variables
********************************************************/

/********************************************************
                Global Variables
********************************************************/

/********************************************************
                Local Functions
********************************************************/
#ifndef CONFIG_MLAN_WMSDK
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
wlan_process_cmdresp_error(mlan_private * pmpriv, HostCmd_DS_COMMAND * resp,
                           mlan_ioctl_req * pioctl_buf)
{
    mlan_adapter *pmadapter = pmpriv->adapter;

    ENTER();

    PRINTM(MERROR, "CMD_RESP: cmd %#x error, result=%#x\n",
           resp->command, resp->result);
    if (pioctl_buf)
        pioctl_buf->status_code = MLAN_ERROR_FW_CMDRESP;

    switch (resp->command) {
    case HostCmd_CMD_802_11_PS_MODE_ENH:
        {
            HostCmd_DS_802_11_PS_MODE_ENH *pm = &resp->params.psmode_enh;
            PRINTM(MERROR,
                   "PS_MODE_ENH command failed: result=0x%x action=0x%X\n",
                   resp->result, wlan_le16_to_cpu(pm->action));
            /*
             * We do not re-try enter-ps command in ad-hoc mode.
             */
            if (wlan_le16_to_cpu(pm->action) == EN_AUTO_PS &&
                (wlan_le16_to_cpu(pm->params.auto_ps.ps_bitmap) & BITMAP_STA_PS)
                && pmpriv->bss_mode == MLAN_BSS_MODE_IBSS)
                pmadapter->ps_mode = Wlan802_11PowerModeCAM;
        }
        break;
#ifdef EXT_SCAN_SUPPORT
    case HostCmd_CMD_802_11_SCAN_EXT:
#endif
    case HostCmd_CMD_802_11_SCAN:
        /* Cancel all pending scan command */
        wlan_flush_scan_queue(pmadapter);
        wlan_recv_event(pmpriv, MLAN_EVENT_ID_DRV_SCAN_REPORT, MNULL);
        break;

    case HostCmd_CMD_MAC_CONTROL:
        break;

    default:
        break;
    }
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
 *  @brief This function handles the command response of RSSI info
 *
 *  @param pmpriv       A pointer to mlan_private structure
 *  @param resp         A pointer to HostCmd_DS_COMMAND
 *  @param pioctl_buf   A pointer to mlan_ioctl_req structure
 *
 *  @return             MLAN_STATUS_SUCCESS
 */
static mlan_status
wlan_ret_802_11_rssi_info(IN pmlan_private pmpriv,
                          IN HostCmd_DS_COMMAND * resp,
                          IN mlan_ioctl_req * pioctl_buf)
{
    HostCmd_DS_802_11_RSSI_INFO_RSP *prssi_info_rsp =
        &resp->params.rssi_info_rsp;
    mlan_ds_get_info *pget_info = MNULL;

    ENTER();

    pmpriv->data_rssi_last = wlan_le16_to_cpu(prssi_info_rsp->data_rssi_last);
    pmpriv->data_nf_last = wlan_le16_to_cpu(prssi_info_rsp->data_nf_last);

    pmpriv->data_rssi_avg = wlan_le16_to_cpu(prssi_info_rsp->data_rssi_avg);
    pmpriv->data_nf_avg = wlan_le16_to_cpu(prssi_info_rsp->data_nf_avg);

    pmpriv->bcn_rssi_last = wlan_le16_to_cpu(prssi_info_rsp->bcn_rssi_last);
    pmpriv->bcn_nf_last = wlan_le16_to_cpu(prssi_info_rsp->bcn_nf_last);

    pmpriv->bcn_rssi_avg = wlan_le16_to_cpu(prssi_info_rsp->bcn_rssi_avg);
    pmpriv->bcn_nf_avg = wlan_le16_to_cpu(prssi_info_rsp->bcn_nf_avg);

    /* Need to indicate IOCTL complete */
    if (pioctl_buf != MNULL) {
        pget_info = (mlan_ds_get_info *) pioctl_buf->pbuf;

        memset(pmpriv->adapter, &pget_info->param.signal, 0,
               sizeof(mlan_ds_get_signal));

        pget_info->param.signal.selector = ALL_RSSI_INFO_MASK;

        /* RSSI */
        pget_info->param.signal.bcn_rssi_last = pmpriv->bcn_rssi_last;
        pget_info->param.signal.bcn_rssi_avg = pmpriv->bcn_rssi_avg;
        pget_info->param.signal.data_rssi_last = pmpriv->data_rssi_last;
        pget_info->param.signal.data_rssi_avg = pmpriv->data_rssi_avg;

        /* SNR */
        pget_info->param.signal.bcn_snr_last =
            CAL_SNR(pmpriv->bcn_rssi_last, pmpriv->bcn_nf_last);
        pget_info->param.signal.bcn_snr_avg =
            CAL_SNR(pmpriv->bcn_rssi_avg, pmpriv->bcn_nf_avg);
        pget_info->param.signal.data_snr_last =
            CAL_SNR(pmpriv->data_rssi_last, pmpriv->data_nf_last);
        pget_info->param.signal.data_snr_avg =
            CAL_SNR(pmpriv->data_rssi_avg, pmpriv->data_nf_avg);

        /* NF */
        pget_info->param.signal.bcn_nf_last = pmpriv->bcn_nf_last;
        pget_info->param.signal.bcn_nf_avg = pmpriv->bcn_nf_avg;
        pget_info->param.signal.data_nf_last = pmpriv->data_nf_last;
        pget_info->param.signal.data_nf_avg = pmpriv->data_nf_avg;

        pioctl_buf->data_read_written = sizeof(mlan_ds_get_info);
    }

    LEAVE();
    return MLAN_STATUS_SUCCESS;
}

/**
 *  @brief This function handles the command response of mac_control
 *
 *  @param pmpriv       A pointer to mlan_private structure
 *  @param resp         A pointer to HostCmd_DS_COMMAND
 *  @param pioctl_buf   A pointer to mlan_ioctl_req structure
 *
 *  @return             MLAN_STATUS_SUCCESS
 */
static mlan_status
wlan_ret_mac_control(IN pmlan_private pmpriv,
                     IN HostCmd_DS_COMMAND * resp,
                     IN mlan_ioctl_req * pioctl_buf)
{
    ENTER();
    LEAVE();
    return MLAN_STATUS_SUCCESS;
}
#endif /* CONFIG_MLAN_WMSDK  */

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
wlan_ret_802_11_snmp_mib(IN pmlan_private pmpriv,
                         IN HostCmd_DS_COMMAND * resp,
                         IN mlan_ioctl_req * pioctl_buf)
{
    HostCmd_DS_802_11_SNMP_MIB *psmib = &resp->params.smib;
    t_u16 oid = wlan_le16_to_cpu(psmib->oid);
    t_u16 query_type = wlan_le16_to_cpu(psmib->query_type);
    t_u32 ul_temp;

#ifndef CONFIG_MLAN_WMSDK
    mlan_ds_snmp_mib *mib = MNULL;

    ENTER();

    if (pioctl_buf)
        mib = (mlan_ds_snmp_mib *) pioctl_buf->pbuf;

    /* wmsdk */
    PRINTM(MINFO, "SNMP_RESP: value of the oid = 0x%x, query_type=0x%x\n", oid,
           query_type);
    PRINTM(MINFO, "SNMP_RESP: Buf size  = 0x%x\n",
           wlan_le16_to_cpu(psmib->buf_size));
#endif /* CONFIG_MLAN_WMSDK */
    if (query_type == HostCmd_ACT_GEN_GET) {
	    /* wmsdk: GET is not used. Disable */
#ifndef CONFIG_MLAN_WMSDK
        switch (oid) {
        case DtimPeriod_i:
            ul_temp = psmib->value[0];
            PRINTM(MINFO, "SNMP_RESP: DTIM Period =%u\n", ul_temp);
            if (mib)
                mib->param.dtim_period = ul_temp;
            break;
        case FragThresh_i:
            ul_temp = wlan_le16_to_cpu(*((t_u16 *) (psmib->value)));
            PRINTM(MINFO, "SNMP_RESP: FragThsd =%u\n", ul_temp);
            if (mib)
                mib->param.frag_threshold = ul_temp;
            break;

        case RtsThresh_i:
            ul_temp = wlan_le16_to_cpu(*((t_u16 *) (psmib->value)));
            PRINTM(MINFO, "SNMP_RESP: RTSThsd =%u\n", ul_temp);
            if (mib)
                mib->param.rts_threshold = ul_temp;
            break;

        case ShortRetryLim_i:
            ul_temp = wlan_le16_to_cpu(*((t_u16 *) (psmib->value)));
            PRINTM(MINFO, "SNMP_RESP: TxRetryCount=%u\n", ul_temp);
            if (mib)
                mib->param.retry_count = ul_temp;
            break;
        case WwsMode_i:
            ul_temp = wlan_le16_to_cpu(*((t_u16 *) (psmib->value)));
            PRINTM(MINFO, "SNMP_RESP: WWSCfg =%u\n", ul_temp);
            if (pioctl_buf)
                ((mlan_ds_misc_cfg *) pioctl_buf->pbuf)->param.wws_cfg =
                    ul_temp;
            break;
        case Thermal_i:
            ul_temp = wlan_le32_to_cpu(*((t_u32 *) (psmib->value)));
            PRINTM(MINFO, "SNMP_RESP: Thermal =%u\n", ul_temp);
            if (pioctl_buf)
                ((mlan_ds_misc_cfg *) pioctl_buf->pbuf)->param.thermal =
                    ul_temp;
            break;

        default:
            break;
        }
#endif /* CONFIG_MLAN_WMSDK */
    } else {                    /* (query_type == HostCmd_ACT_GEN_SET) */
        /* Update state for 11d */
        if (oid == Dot11D_i) {
            ul_temp = wlan_le16_to_cpu(*((t_u16 *) (psmib->value)));
            /* Set 11d state to private */
            pmpriv->state_11d.enable_11d = ul_temp;
            /* Set user enable flag if called from ioctl */
            if (pioctl_buf)
                pmpriv->state_11d.user_enable_11d = ul_temp;
        }
        /* Update state for 11h */
        if (oid == Dot11H_i) {
            ul_temp = wlan_le16_to_cpu(*((t_u16 *) (psmib->value)));
            /* Set 11h state to priv */
            pmpriv->intf_state_11h.is_11h_active = (ul_temp & ENABLE_11H_MASK);
            /* Set radar_det state to adapter */
            pmpriv->adapter->state_11h.is_master_radar_det_active
                = (ul_temp & MASTER_RADAR_DET_MASK) ? MTRUE : MFALSE;
            pmpriv->adapter->state_11h.is_slave_radar_det_active
                = (ul_temp & SLAVE_RADAR_DET_MASK) ? MTRUE : MFALSE;
        }
    }

    if (pioctl_buf) {
        /* Indicate ioctl complete */
        pioctl_buf->data_read_written = sizeof(mlan_ds_snmp_mib);
    }

    LEAVE();
    return MLAN_STATUS_SUCCESS;
}

/**
 *  @brief This function handles the command response of get_log
 *
 *  @param pmpriv       A pointer to mlan_private structure
 *  @param resp         A pointer to HostCmd_DS_COMMAND
 *  @param pioctl_buf   A pointer to mlan_ioctl_req structure
 *
 *  @return             MLAN_STATUS_SUCCESS
 */
static mlan_status
wlan_ret_get_log(IN pmlan_private pmpriv,
                 IN HostCmd_DS_COMMAND * resp, IN mlan_ioctl_req * pioctl_buf)
{
    HostCmd_DS_802_11_GET_LOG *pget_log =
        (HostCmd_DS_802_11_GET_LOG *) & resp->params.get_log;
    mlan_ds_get_info *pget_info = MNULL;

    ENTER();
    if (pioctl_buf) {
        pget_info = (mlan_ds_get_info *) pioctl_buf->pbuf;
        pget_info->param.stats.mcast_tx_frame =
            wlan_le32_to_cpu(pget_log->mcast_tx_frame);
        pget_info->param.stats.failed = wlan_le32_to_cpu(pget_log->failed);
        pget_info->param.stats.retry = wlan_le32_to_cpu(pget_log->retry);
        pget_info->param.stats.multi_retry =
            wlan_le32_to_cpu(pget_log->multiretry);
        pget_info->param.stats.frame_dup =
            wlan_le32_to_cpu(pget_log->frame_dup);
        pget_info->param.stats.rts_success =
            wlan_le32_to_cpu(pget_log->rts_success);
        pget_info->param.stats.rts_failure =
            wlan_le32_to_cpu(pget_log->rts_failure);
        pget_info->param.stats.ack_failure =
            wlan_le32_to_cpu(pget_log->ack_failure);
        pget_info->param.stats.rx_frag = wlan_le32_to_cpu(pget_log->rx_frag);
        pget_info->param.stats.mcast_rx_frame =
            wlan_le32_to_cpu(pget_log->mcast_rx_frame);
        pget_info->param.stats.fcs_error =
            wlan_le32_to_cpu(pget_log->fcs_error);
        pget_info->param.stats.tx_frame = wlan_le32_to_cpu(pget_log->tx_frame);
        pget_info->param.stats.wep_icv_error[0] =
            wlan_le32_to_cpu(pget_log->wep_icv_err_cnt[0]);
        pget_info->param.stats.wep_icv_error[1] =
            wlan_le32_to_cpu(pget_log->wep_icv_err_cnt[1]);
        pget_info->param.stats.wep_icv_error[2] =
            wlan_le32_to_cpu(pget_log->wep_icv_err_cnt[2]);
        pget_info->param.stats.wep_icv_error[3] =
            wlan_le32_to_cpu(pget_log->wep_icv_err_cnt[3]);
        /* Indicate ioctl complete */
        pioctl_buf->data_read_written = sizeof(mlan_ds_get_info);
    }
    LEAVE();
    return MLAN_STATUS_SUCCESS;
}

/**
 *  @brief Get power level and rate index
 *
 *  @param pmpriv       A pointer to mlan_private structure
 *  @param pdata_buf    Pointer to the data buffer
 *
 *  @return             MLAN_STATUS_SUCCESS or MLAN_STATUS_FAILURE
 */
static mlan_status
wlan_get_power_level(pmlan_private pmpriv, void *pdata_buf)
{
    int length = -1, max_power = -1, min_power = -1;
    MrvlTypes_Power_Group_t *ppg_tlv = MNULL;
    Power_Group_t *pg = MNULL;

    ENTER();

    if (pdata_buf) {
        ppg_tlv =
            (MrvlTypes_Power_Group_t *) ((t_u8 *) pdata_buf +
                                         sizeof(HostCmd_DS_TXPWR_CFG));
        pg = (Power_Group_t *) ((t_u8 *) ppg_tlv +
                                sizeof(MrvlTypes_Power_Group_t));
        length = ppg_tlv->length;
        if (length > 0) {
            max_power = pg->power_max;
            min_power = pg->power_min;
            length -= sizeof(Power_Group_t);
        }
        while (length) {
            pg++;
            if (max_power < pg->power_max) {
                max_power = pg->power_max;
            }
            if (min_power > pg->power_min) {
                min_power = pg->power_min;
            }
            length -= sizeof(Power_Group_t);
        }
        if (ppg_tlv->length > 0) {
            pmpriv->min_tx_power_level = (t_u8) min_power;
            pmpriv->max_tx_power_level = (t_u8) max_power;
        }
    } else {
        LEAVE();
        return MLAN_STATUS_FAILURE;
    }

    LEAVE();
    return MLAN_STATUS_SUCCESS;
}
/**
 *  @brief This function handles the command response of tx_power_cfg
 *
 *  @param pmpriv       A pointer to mlan_private structure
 *  @param resp         A pointer to HostCmd_DS_COMMAND
 *  @param pioctl_buf   A pointer to mlan_ioctl_req structure
 *
 *  @return             MLAN_STATUS_SUCCESS
 */
static mlan_status
wlan_ret_tx_power_cfg(IN pmlan_private pmpriv,
                      IN HostCmd_DS_COMMAND * resp,
                      IN mlan_ioctl_req * pioctl_buf)
{
    HostCmd_DS_TXPWR_CFG *ptxp_cfg = &resp->params.txp_cfg;
    MrvlTypes_Power_Group_t *ppg_tlv = MNULL;
    Power_Group_t *pg = MNULL;
    t_u16 action = wlan_le16_to_cpu(ptxp_cfg->action);
    mlan_ds_power_cfg *power = MNULL;
    t_u32 data[5];

    ENTER();

    ppg_tlv = (MrvlTypes_Power_Group_t *) ((t_u8 *) ptxp_cfg
                                           + sizeof(HostCmd_DS_TXPWR_CFG));
    pg = (Power_Group_t *) ((t_u8 *) ppg_tlv + sizeof(MrvlTypes_Power_Group_t));

    switch (action) {
    case HostCmd_ACT_GEN_GET:
        ppg_tlv->length = wlan_le16_to_cpu(ppg_tlv->length);
        if (pmpriv->adapter->hw_status == WlanHardwareStatusInitializing)
            wlan_get_power_level(pmpriv, ptxp_cfg);
        pmpriv->tx_power_level = (t_u16) pg->power_min;
	PRINTM(MMSG, "The Sta tx power level: %d\r\n",
		 pmpriv->tx_power_level);
        break;

    case HostCmd_ACT_GEN_SET:
	    if (wlan_le32_to_cpu(ptxp_cfg->mode)) {
		    if (pg->power_max == pg->power_min)
			    pmpriv->tx_power_level = (t_u16) pg->power_min;
	    }
	    break;
    default:
        PRINTM(MERROR, "CMD_RESP: unknown command action %d\n", action);
        LEAVE();
        return MLAN_STATUS_SUCCESS;
    }

    PRINTM(MINFO, "Current TxPower Level = %d,Max Power=%d, Min Power=%d\n",
           pmpriv->tx_power_level,
           pmpriv->max_tx_power_level, pmpriv->min_tx_power_level);

    if (pioctl_buf) {
        power = (mlan_ds_power_cfg *) pioctl_buf->pbuf;
        if (action == HostCmd_ACT_GEN_GET) {
            if (power->sub_command == MLAN_OID_POWER_CFG) {
                pioctl_buf->data_read_written
                    = sizeof(mlan_power_cfg_t) + MLAN_SUB_COMMAND_SIZE;
                power->param.power_cfg.power_level = pmpriv->tx_power_level;
                if (wlan_le32_to_cpu(ptxp_cfg->mode))
                    power->param.power_cfg.is_power_auto = 0;
                else
                    power->param.power_cfg.is_power_auto = 1;
            } else {
                power->param.power_ext.len = 0;
                while (ppg_tlv->length) {
                    data[0] = pg->first_rate_code;
                    data[1] = pg->last_rate_code;
                    if (pg->modulation_class == MOD_CLASS_OFDM) {
                        data[0] += MLAN_RATE_INDEX_OFDM0;
                        data[1] += MLAN_RATE_INDEX_OFDM0;
                    } else if (pg->modulation_class == MOD_CLASS_HT) {
                        data[0] += MLAN_RATE_INDEX_MCS0;
                        data[1] += MLAN_RATE_INDEX_MCS0;
                        if (pg->ht_bandwidth == HT_BW_40) {
                            data[0] |= TX_RATE_HT_BW40_BIT;
                            data[1] |= TX_RATE_HT_BW40_BIT;
                        }
                    }
                    data[2] = pg->power_min;
                    data[3] = pg->power_max;
                    data[4] = pg->power_step;
                    memcpy(pmpriv->adapter,
                           (t_u8 *) (&power->param.power_ext.
                                     power_data[power->param.power_ext.len]),
                           (t_u8 *) data, sizeof(data));
                    power->param.power_ext.len += 5;
                    pg++;
                    ppg_tlv->length -= sizeof(Power_Group_t);
                }
                pioctl_buf->data_read_written
                    = sizeof(mlan_power_cfg_ext) + MLAN_SUB_COMMAND_SIZE;
            }
        }
    }

    LEAVE();
    return MLAN_STATUS_SUCCESS;
}
#ifndef CONFIG_MLAN_WMSDK

/**
 *  @brief This function handles the command response of rf_tx_power
 *
 *  @param pmpriv       A pointer to mlan_private structure
 *  @param resp         A pointer to HostCmd_DS_COMMAND
 *  @param pioctl_buf   A pointer to mlan_ioctl_req structure
 *  @return             MLAN_STATUS_SUCCESS
 */
static mlan_status
wlan_ret_802_11_rf_tx_power(IN pmlan_private pmpriv,
                            IN HostCmd_DS_COMMAND * resp,
                            IN mlan_ioctl_req * pioctl_buf)
{
    HostCmd_DS_802_11_RF_TX_POWER *rtp = &resp->params.txp;
    t_u16 action = wlan_le16_to_cpu(rtp->action);
    mlan_ds_power_cfg *power = MNULL;

    ENTER();

    pmpriv->tx_power_level = wlan_le16_to_cpu(rtp->current_level);

    if (action == HostCmd_ACT_GEN_GET) {
        pmpriv->max_tx_power_level = rtp->max_power;
        pmpriv->min_tx_power_level = rtp->min_power;
        if (pioctl_buf) {
            power = (mlan_ds_power_cfg *) pioctl_buf->pbuf;
            if (power->sub_command == MLAN_OID_POWER_CFG) {
                pioctl_buf->data_read_written
                    = sizeof(mlan_power_cfg_t) + MLAN_SUB_COMMAND_SIZE;
                power->param.power_cfg.power_level = pmpriv->tx_power_level;
            }
        }
    }

    PRINTM(MINFO, "Current TxPower Level = %d,Max Power=%d, Min Power=%d\n",
           pmpriv->tx_power_level,
           pmpriv->max_tx_power_level, pmpriv->min_tx_power_level);

    LEAVE();
    return MLAN_STATUS_SUCCESS;
}

/**
 *  @brief This function handles the command response of sleep_period
 *
 *  @param pmpriv       A pointer to mlan_private structure
 *  @param resp         A pointer to HostCmd_DS_COMMAND
 *  @param pioctl_buf   A pointer to mlan_ioctl_req structure
 *  @return             MLAN_STATUS_SUCCESS
 */
static mlan_status
wlan_ret_802_11_sleep_period(IN pmlan_private pmpriv,
                             IN HostCmd_DS_COMMAND * resp,
                             IN mlan_ioctl_req * pioctl_buf)
{
    HostCmd_DS_802_11_SLEEP_PERIOD *pcmd_sleep_pd = &resp->params.sleep_pd;
    mlan_ds_pm_cfg *pm_cfg = MNULL;
    t_u16 sleep_pd = 0;

    ENTER();

    sleep_pd = wlan_le16_to_cpu(pcmd_sleep_pd->sleep_pd);
    if (pioctl_buf) {
        pm_cfg = (mlan_ds_pm_cfg *) pioctl_buf->pbuf;
        pm_cfg->param.sleep_period = (t_u32) sleep_pd;
        pioctl_buf->data_read_written = sizeof(pm_cfg->param.sleep_period)
            + MLAN_SUB_COMMAND_SIZE;
    }
    pmpriv->adapter->sleep_period.period = sleep_pd;

    pmpriv->adapter->pps_uapsd_mode = MFALSE;
    if ((pmpriv->adapter->sleep_period.period != 0) &&
        (pmpriv->adapter->sleep_period.period != SLEEP_PERIOD_RESERVED_FF)) {
        pmpriv->adapter->gen_null_pkt = MTRUE;
    } else {
        pmpriv->adapter->delay_null_pkt = MFALSE;
        pmpriv->adapter->gen_null_pkt = MFALSE;
    }

    LEAVE();
    return MLAN_STATUS_SUCCESS;
}

/**
 *  @brief This function handles the command response of sleep_params
 *
 *  @param pmpriv       A pointer to mlan_private structure
 *  @param resp         A pointer to HostCmd_DS_COMMAND
 *  @param pioctl_buf   A pointer to mlan_ioctl_req structure
 *  @return             MLAN_STATUS_SUCCESS
 */
static mlan_status
wlan_ret_802_11_sleep_params(IN pmlan_private pmpriv,
                             IN HostCmd_DS_COMMAND * resp,
                             IN mlan_ioctl_req * pioctl_buf)
{
    HostCmd_DS_802_11_SLEEP_PARAMS *presp_sp = &resp->params.sleep_param;
    mlan_ds_pm_cfg *pm_cfg = MNULL;
    mlan_ds_sleep_params *psp = MNULL;
    sleep_params_t *psleep_params = &pmpriv->adapter->sleep_params;

    ENTER();

    psleep_params->sp_reserved = wlan_le16_to_cpu(presp_sp->reserved);
    psleep_params->sp_error = wlan_le16_to_cpu(presp_sp->error);
    psleep_params->sp_offset = wlan_le16_to_cpu(presp_sp->offset);
    psleep_params->sp_stable_time = wlan_le16_to_cpu(presp_sp->stable_time);
    psleep_params->sp_cal_control = presp_sp->cal_control;
    psleep_params->sp_ext_sleep_clk = presp_sp->external_sleep_clk;

    if (pioctl_buf) {
        pm_cfg = (mlan_ds_pm_cfg *) pioctl_buf->pbuf;
        psp = (mlan_ds_sleep_params *) & pm_cfg->param.sleep_params;

        psp->error = (t_u32) psleep_params->sp_error;
        psp->offset = (t_u32) psleep_params->sp_offset;
        psp->stable_time = (t_u32) psleep_params->sp_stable_time;
        psp->cal_control = (t_u32) psleep_params->sp_cal_control;
        psp->ext_sleep_clk = (t_u32) psleep_params->sp_ext_sleep_clk;
        psp->reserved = (t_u32) psleep_params->sp_reserved;

        pioctl_buf->data_read_written = sizeof(pm_cfg->param.sleep_params)
            + MLAN_SUB_COMMAND_SIZE;
    }

    LEAVE();
    return MLAN_STATUS_SUCCESS;
}

/**
 *  @brief This function handles the command response of mac_address
 *
 *  @param pmpriv       A pointer to mlan_private structure
 *  @param resp         A pointer to HostCmd_DS_COMMAND
 *  @param pioctl_buf   A pointer to mlan_ioctl_req structure
 *
 *  @return             MLAN_STATUS_SUCCESS
 */
static mlan_status
wlan_ret_802_11_mac_address(IN pmlan_private pmpriv,
                            IN HostCmd_DS_COMMAND * resp,
                            IN mlan_ioctl_req * pioctl_buf)
{
    HostCmd_DS_802_11_MAC_ADDRESS *pmac_addr = &resp->params.mac_addr;
    mlan_ds_bss *bss = MNULL;

    ENTER();

    memcpy(pmpriv->adapter, pmpriv->curr_addr, pmac_addr->mac_addr,
           MLAN_MAC_ADDR_LENGTH);

    PRINTM(MINFO, "MAC address: %02x:%02x:%02x:%02x:%02x:%02x\n",
           pmpriv->curr_addr[0], pmpriv->curr_addr[1], pmpriv->curr_addr[2],
           pmpriv->curr_addr[3], pmpriv->curr_addr[4], pmpriv->curr_addr[5]);
    if (pioctl_buf) {
        bss = (mlan_ds_bss *) pioctl_buf->pbuf;
        memcpy(pmpriv->adapter, &bss->param.mac_addr, pmpriv->curr_addr,
               MLAN_MAC_ADDR_LENGTH);
        pioctl_buf->data_read_written =
            MLAN_MAC_ADDR_LENGTH + MLAN_SUB_COMMAND_SIZE;
    }
    LEAVE();
    return MLAN_STATUS_SUCCESS;
}

/**
 *  @brief This function handles the command response of multicast_address
 *
 *  @param pmpriv       A pointer to mlan_private structure
 *  @param resp         A pointer to HostCmd_DS_COMMAND
 *  @param pioctl_buf   A pointer to mlan_ioctl_req structure
 *
 *  @return             MLAN_STATUS_SUCCESS
 */
static mlan_status
wlan_ret_mac_multicast_adr(IN pmlan_private pmpriv,
                           IN HostCmd_DS_COMMAND * resp,
                           IN mlan_ioctl_req * pioctl_buf)
{
    ENTER();
    if (pioctl_buf) {
        pioctl_buf->data_read_written =
            sizeof(mlan_multicast_list) + MLAN_SUB_COMMAND_SIZE;
    }
    LEAVE();
    return MLAN_STATUS_SUCCESS;
}

#endif /* 0 */

/**
 *  @brief This function handles the command response of deauthenticate
 *
 *  @param pmpriv       A pointer to mlan_private structure
 *  @param resp         A pointer to HostCmd_DS_COMMAND
 *  @param pioctl_buf   A pointer to mlan_ioctl_req structure
 *
 *  @return             MLAN_STATUS_SUCCESS
 */
mlan_status
wlan_ret_802_11_deauthenticate(IN pmlan_private pmpriv,
                               IN HostCmd_DS_COMMAND * resp,
                               IN mlan_ioctl_req * pioctl_buf)
{
#ifndef CONFIG_MLAN_WMSDK
    mlan_adapter *pmadapter = pmpriv->adapter;
#endif /* CONFIG_MLAN_WMSDK */
    ENTER();
#ifndef CONFIG_MLAN_WMSDK
    pmadapter->dbg.num_cmd_deauth++;

    if (!memcmp(pmadapter, resp->params.deauth.mac_addr,
                &pmpriv->curr_bss_params.bss_descriptor.mac_address,
                sizeof(resp->params.deauth.mac_addr)))
#endif /* CONFIG_MLAN_WMSDK */
        wlan_reset_connect_state(pmpriv, MTRUE);

#ifndef CONFIG_MLAN_WMSDK
    if (pmpriv->adapter->state_rdh.stage == RDH_STOP_INTFS)
        wlan_11h_radar_detected_callback((t_void *) pmpriv);
#endif /* CONFIG_MLAN_WMSDK */

    LEAVE();
    return MLAN_STATUS_SUCCESS;
}

#ifndef CONFIG_MLAN_WMSDK
/**
 *  @brief This function handles the command response of ad_hoc_stop
 *
 *  @param pmpriv       A pointer to mlan_private structure
 *  @param resp         A pointer to HostCmd_DS_COMMAND
 *  @param pioctl_buf   A pointer to mlan_ioctl_req structure
 *
 *  @return        MLAN_STATUS_SUCCESS
 */
static mlan_status
wlan_ret_802_11_ad_hoc_stop(IN pmlan_private pmpriv,
                            IN HostCmd_DS_COMMAND * resp,
                            IN mlan_ioctl_req * pioctl_buf)
{
    ENTER();

    wlan_reset_connect_state(pmpriv, MTRUE);
    if (pmpriv->adapter->state_rdh.stage == RDH_STOP_INTFS)
        wlan_11h_radar_detected_callback((t_void *) pmpriv);
    LEAVE();
    return MLAN_STATUS_SUCCESS;
}

/**
 *  @brief This function handles the command response of key_material
 *
 *  @param pmpriv       A pointer to mlan_private structure
 *  @param resp         A pointer to HostCmd_DS_COMMAND
 *  @param pioctl_buf   A pointer to mlan_ioctl_req structure
 *
 *  @return        MLAN_STATUS_SUCCESS
 */
static mlan_status
wlan_ret_802_11_key_material(IN pmlan_private pmpriv,
                             IN HostCmd_DS_COMMAND * resp,
                             IN mlan_ioctl_req * pioctl_buf)
{
    HostCmd_DS_802_11_KEY_MATERIAL *pkey = &resp->params.key_material;
    mlan_ds_sec_cfg *sec = MNULL;

    ENTER();

    if (wlan_le16_to_cpu(pkey->action) == HostCmd_ACT_GEN_SET) {
        if ((wlan_le16_to_cpu(pkey->key_param_set.key_info) &
             KEY_INFO_TKIP_MCAST)) {
            PRINTM(MINFO, "key: GTK is set\n");
            pmpriv->wpa_is_gtk_set = MTRUE;
            if (pmpriv->port_ctrl_mode == MTRUE) {
                /* GTK is set, open the port */
                PRINTM(MINFO, "GTK_SET: Open port for WPA/WPA2 h-supp mode\n");
                pmpriv->port_open = MTRUE;
            }
            pmpriv->scan_block = MFALSE;
        }
    } else {
        if (pioctl_buf &&
            (wlan_le16_to_cpu(pkey->key_param_set.type) ==
             TLV_TYPE_KEY_MATERIAL)) {
            PRINTM(MIOCTL, "key_type_id=%d, key_len=%d, key_info=0x%x\n",
                   wlan_le16_to_cpu(pkey->key_param_set.key_type_id),
                   wlan_le16_to_cpu(pkey->key_param_set.key_len),
                   wlan_le16_to_cpu(pkey->key_param_set.key_info));
            sec = (mlan_ds_sec_cfg *) pioctl_buf->pbuf;
#define WAPI_KEY_SIZE 32
            switch (wlan_le16_to_cpu(pkey->key_param_set.key_type_id)) {
            case KEY_TYPE_ID_WEP:
                sec->param.encrypt_key.key_index = pkey->key_param_set.key[0];
                sec->param.encrypt_key.key_len =
                    wlan_le16_to_cpu(pkey->key_param_set.key_len);
                memcpy(pmpriv->adapter, sec->param.encrypt_key.key_material,
                       &pkey->key_param_set.key[2],
                       sec->param.encrypt_key.key_len);
                break;
            case KEY_TYPE_ID_TKIP:
                sec->param.encrypt_key.key_len =
                    wlan_le16_to_cpu(pkey->key_param_set.key_len);
                memcpy(pmpriv->adapter, sec->param.encrypt_key.key_material,
                       pkey->key_param_set.key, sec->param.encrypt_key.key_len);
                break;
            case KEY_TYPE_ID_AES:
                sec->param.encrypt_key.key_len =
                    wlan_le16_to_cpu(pkey->key_param_set.key_len);
                memcpy(pmpriv->adapter, sec->param.encrypt_key.key_material,
                       pkey->key_param_set.key, sec->param.encrypt_key.key_len);
                break;
            case KEY_TYPE_ID_WAPI:
                sec->param.encrypt_key.is_wapi_key = MTRUE;
                sec->param.encrypt_key.key_index = pkey->key_param_set.key[0];
                sec->param.encrypt_key.key_len = WAPI_KEY_SIZE;
                memcpy(pmpriv->adapter, sec->param.encrypt_key.key_material,
                       &pkey->key_param_set.key[2],
                       sec->param.encrypt_key.key_len);
                memcpy(pmpriv->adapter, sec->param.encrypt_key.pn,
                       &pkey->key_param_set.key[2 + WAPI_KEY_SIZE], PN_SIZE);
                break;
            }
        }
    }
    LEAVE();
    return MLAN_STATUS_SUCCESS;
}

/**
 *  @brief Handle the supplicant pmk response
 *
 *  @param pmpriv       A pointer to mlan_private structure
 *  @param resp         A pointer to HostCmd_DS_COMMAND
 *  @param pioctl_buf   A pointer to mlan_ioctl_req structure
 *
 *  @return        MLAN_STATUS_SUCCESS MLAN_STATUS_FAILURE
 */
static mlan_status
wlan_ret_802_11_supplicant_pmk(IN pmlan_private pmpriv,
                               IN HostCmd_DS_COMMAND * resp,
                               IN mlan_ioctl_req * pioctl_buf)
{
    HostCmd_DS_802_11_SUPPLICANT_PMK *supplicant_pmk_resp =
        &resp->params.esupplicant_psk;
    mlan_ds_sec_cfg sec_buf;
    mlan_ds_sec_cfg *sec = MNULL;
    MrvlIEtypes_PMK_t *ppmk_tlv = MNULL;
    MrvlIEtypes_Passphrase_t *passphrase_tlv = MNULL;
    MrvlIEtypes_SsIdParamSet_t *pssid_tlv = MNULL;
    MrvlIEtypes_Bssid_t *pbssid_tlv = MNULL;
    t_u8 *tlv_buf = (t_u8 *) supplicant_pmk_resp->tlv_buffer;
    t_u16 action = wlan_le16_to_cpu(supplicant_pmk_resp->action);
    int tlv_buf_len = 0;
    t_u16 tlv;
    mlan_status ret = MLAN_STATUS_SUCCESS;

    ENTER();
    tlv_buf_len = resp->size - (sizeof(HostCmd_DS_802_11_SUPPLICANT_PMK) +
                                S_DS_GEN - 1);
    if (pioctl_buf) {
        if (((mlan_ds_bss *) pioctl_buf->pbuf)->sub_command ==
            MLAN_OID_BSS_FIND_BSS)
            sec = &sec_buf;
        else
            sec = (mlan_ds_sec_cfg *) pioctl_buf->pbuf;
        if (action == HostCmd_ACT_GEN_GET) {
            while (tlv_buf_len > 0) {
                tlv = (*tlv_buf) | (*(tlv_buf + 1) << 8);
                if ((tlv != TLV_TYPE_SSID) && (tlv != TLV_TYPE_BSSID) &&
                    (tlv != TLV_TYPE_PASSPHRASE)
                    && (tlv != TLV_TYPE_PMK))
                    break;
                switch (tlv) {
                case TLV_TYPE_SSID:
                    pssid_tlv = (MrvlIEtypes_SsIdParamSet_t *) tlv_buf;
                    pssid_tlv->header.len =
                        wlan_le16_to_cpu(pssid_tlv->header.len);
                    memcpy(pmpriv->adapter, sec->param.passphrase.ssid.ssid,
                           pssid_tlv->ssid, MIN(MLAN_MAX_SSID_LENGTH,
                                                pssid_tlv->header.len));
                    sec->param.passphrase.ssid.ssid_len =
                        MIN(MLAN_MAX_SSID_LENGTH, pssid_tlv->header.len);
                    tlv_buf +=
                        pssid_tlv->header.len + sizeof(MrvlIEtypesHeader_t);
                    tlv_buf_len -=
                        (pssid_tlv->header.len + sizeof(MrvlIEtypesHeader_t));
                    break;
                case TLV_TYPE_BSSID:
                    pbssid_tlv = (MrvlIEtypes_Bssid_t *) tlv_buf;
                    pbssid_tlv->header.len =
                        wlan_le16_to_cpu(pbssid_tlv->header.len);
                    memcpy(pmpriv->adapter, &sec->param.passphrase.bssid,
                           pbssid_tlv->bssid, MLAN_MAC_ADDR_LENGTH);
                    tlv_buf +=
                        pbssid_tlv->header.len + sizeof(MrvlIEtypesHeader_t);
                    tlv_buf_len -=
                        (pbssid_tlv->header.len + sizeof(MrvlIEtypesHeader_t));
                    break;
                case TLV_TYPE_PASSPHRASE:
                    passphrase_tlv = (MrvlIEtypes_Passphrase_t *) tlv_buf;
                    passphrase_tlv->header.len =
                        wlan_le16_to_cpu(passphrase_tlv->header.len);
                    sec->param.passphrase.psk_type = MLAN_PSK_PASSPHRASE;
                    sec->param.passphrase.psk.passphrase.passphrase_len =
                        passphrase_tlv->header.len;
                    memcpy(pmpriv->adapter,
                           sec->param.passphrase.psk.passphrase.passphrase,
                           passphrase_tlv->passphrase,
                           MIN(MLAN_MAX_PASSPHRASE_LENGTH,
                               passphrase_tlv->header.len));
                    tlv_buf +=
                        passphrase_tlv->header.len +
                        sizeof(MrvlIEtypesHeader_t);
                    tlv_buf_len -=
                        (passphrase_tlv->header.len +
                         sizeof(MrvlIEtypesHeader_t));
                    break;
                case TLV_TYPE_PMK:
                    ppmk_tlv = (MrvlIEtypes_PMK_t *) tlv_buf;
                    ppmk_tlv->header.len =
                        wlan_le16_to_cpu(ppmk_tlv->header.len);
                    sec->param.passphrase.psk_type = MLAN_PSK_PMK;
                    memcpy(pmpriv->adapter, sec->param.passphrase.psk.pmk.pmk,
                           ppmk_tlv->pmk, MIN(MLAN_MAX_KEY_LENGTH,
                                              ppmk_tlv->header.len));
                    tlv_buf +=
                        ppmk_tlv->header.len + sizeof(MrvlIEtypesHeader_t);
                    tlv_buf_len -=
                        (ppmk_tlv->header.len + sizeof(MrvlIEtypesHeader_t));
                    break;

                }
            }
            if (((mlan_ds_bss *) pioctl_buf->pbuf)->sub_command ==
                MLAN_OID_BSS_FIND_BSS) {
                wlan_set_ewpa_mode(pmpriv, &sec->param.passphrase);
                ret = wlan_find_bss(pmpriv, pioctl_buf);
            }
        } else if (action == HostCmd_ACT_GEN_SET) {
            PRINTM(MINFO, "Esupp PMK set: enable ewpa query\n");
            pmpriv->ewpa_query = MTRUE;
        }
    }

    LEAVE();
    return ret;
}

/**
 *  @brief Handle the supplicant profile response
 *
 *  @param pmpriv       A pointer to mlan_private structure
 *  @param resp         A pointer to HostCmd_DS_COMMAND
 *  @param pioctl_buf   A pointer to mlan_ioctl_req structure
 *
 *  @return        MLAN_STATUS_SUCCESS
 */
static mlan_status
wlan_ret_802_11_supplicant_profile(IN pmlan_private pmpriv,
                                   IN HostCmd_DS_COMMAND * resp,
                                   IN mlan_ioctl_req * pioctl_buf)
{
    HostCmd_DS_802_11_SUPPLICANT_PROFILE *psup_profile =
        &resp->params.esupplicant_profile;
    MrvlIEtypesHeader_t *head;
    MrvlIEtypes_EncrProto_t *encr_proto_tlv = MNULL;
    MrvlIEtypes_Cipher_t *pcipher_tlv = MNULL;
    mlan_ds_sec_cfg *sec = MNULL;
    t_u8 *tlv;
    int len;

    ENTER();

    len = resp->size - S_DS_GEN - sizeof(t_u16);
    tlv = psup_profile->tlv_buf;
    if (pioctl_buf) {
        sec = (mlan_ds_sec_cfg *) pioctl_buf->pbuf;
        while (len > 0) {
            head = (MrvlIEtypesHeader_t *) tlv;
            head->type = wlan_le16_to_cpu(head->type);
            head->len = wlan_le16_to_cpu(head->len);
            switch (head->type) {
            case TLV_TYPE_ENCRYPTION_PROTO:
                encr_proto_tlv = (MrvlIEtypes_EncrProto_t *) head;
                sec->param.esupp_mode.rsn_mode =
                    wlan_le16_to_cpu(encr_proto_tlv->rsn_mode);
                PRINTM(MINFO, "rsn_mode=0x%x\n",
                       sec->param.esupp_mode.rsn_mode);
                break;
            case TLV_TYPE_CIPHER:
                pcipher_tlv = (MrvlIEtypes_Cipher_t *) head;
                sec->param.esupp_mode.act_paircipher = pcipher_tlv->pair_cipher;
                sec->param.esupp_mode.act_groupcipher =
                    pcipher_tlv->group_cipher;
                PRINTM(MINFO, "paircipher=0x%x, groupcipher=0x%x\n",
                       sec->param.esupp_mode.act_paircipher,
                       sec->param.esupp_mode.act_groupcipher);
                break;
            }
            len -= (head->len - sizeof(MrvlIEtypesHeader_t));
            tlv = tlv + (head->len + sizeof(MrvlIEtypesHeader_t));
        }
    }

    LEAVE();
    return MLAN_STATUS_SUCCESS;
}
#endif /* CONFIG_MLAN_WMSDK */

/**
 *  @brief This function handles the command response of rf_channel
 *
 *  @param pmpriv       A pointer to mlan_private structure
 *  @param resp         A pointer to HostCmd_DS_COMMAND
 *  @param pioctl_buf   A pointer to mlan_ioctl_req structure
 *
 *  @return             MLAN_STATUS_SUCCESS
 */
static mlan_status
wlan_ret_802_11_rf_channel(IN pmlan_private pmpriv,
                           IN HostCmd_DS_COMMAND * resp,
                           IN mlan_ioctl_req * pioctl_buf)
{

    ENTER();
    /* fixme: enable this part when needed */
#ifndef CONFIG_MLAN_WMSDK
    HostCmd_DS_802_11_RF_CHANNEL *prf_channel = &resp->params.rf_channel;
    t_u16 new_channel = wlan_le16_to_cpu(prf_channel->current_channel);
    /* mlan_ds_bss *bss = MNULL; */
    if (pmpriv->curr_bss_params.bss_descriptor.channel != new_channel) {
        PRINTM(MINFO, "Channel Switch: %d to %d\n",
               pmpriv->curr_bss_params.bss_descriptor.channel, new_channel);
        /* Update the channel again */
        pmpriv->curr_bss_params.bss_descriptor.channel = new_channel;
    }
    if (pioctl_buf) {
        bss = (mlan_ds_bss *) pioctl_buf->pbuf;
        bss->param.bss_chan.channel = new_channel;
    }
#endif /* CONFIG_MLAN_WMSDK */
    LEAVE();
    return MLAN_STATUS_SUCCESS;
}

#ifndef CONFIG_MLAN_WMSDK
/**
 *  @brief Handle the ibss_coalescing_status resp
 *
 *  @param pmpriv       A pointer to mlan_private structure
 *  @param resp         A pointer to HostCmd_DS_COMMAND
 *
 *  @return             MLAN_STATUS_SUCCESS
 */
static mlan_status
wlan_ret_ibss_coalescing_status(IN pmlan_private pmpriv,
                                IN HostCmd_DS_COMMAND * resp)
{
    HostCmd_DS_802_11_IBSS_STATUS *pibss_coal_resp =
        &(resp->params.ibss_coalescing);
    t_u8 zero_mac[MLAN_MAC_ADDR_LENGTH] = { 0, 0, 0, 0, 0, 0 };

    ENTER();

    if (wlan_le16_to_cpu(pibss_coal_resp->action) == HostCmd_ACT_GEN_SET) {
        LEAVE();
        return MLAN_STATUS_SUCCESS;
    }

    PRINTM(MINFO, "New BSSID %02x:%02x:%02x:%02x:%02x:%02x\n",
           pibss_coal_resp->bssid[0], pibss_coal_resp->bssid[1],
           pibss_coal_resp->bssid[2], pibss_coal_resp->bssid[3],
           pibss_coal_resp->bssid[4], pibss_coal_resp->bssid[5]);

    /* If rsp has MNULL BSSID, Just return..... No Action */
    if (!memcmp
        (pmpriv->adapter, pibss_coal_resp->bssid, zero_mac,
         MLAN_MAC_ADDR_LENGTH)) {
        PRINTM(MMSG, "New BSSID is MNULL\n");
        LEAVE();
        return MLAN_STATUS_SUCCESS;
    }

    /* If BSSID is diff, modify current BSS parameters */
    if (memcmp
        (pmpriv->adapter, pmpriv->curr_bss_params.bss_descriptor.mac_address,
         pibss_coal_resp->bssid, MLAN_MAC_ADDR_LENGTH)) {
        /* BSSID */
        memcpy(pmpriv->adapter,
               pmpriv->curr_bss_params.bss_descriptor.mac_address,
               pibss_coal_resp->bssid, MLAN_MAC_ADDR_LENGTH);

        /* Beacon Interval and ATIM window */
        pmpriv->curr_bss_params.bss_descriptor.beacon_period
            = wlan_le16_to_cpu(pibss_coal_resp->beacon_interval);
        pmpriv->curr_bss_params.bss_descriptor.atim_window
            = wlan_le16_to_cpu(pibss_coal_resp->atim_window);

        /* ERP Information */
        pmpriv->curr_bss_params.bss_descriptor.erp_flags
            = (t_u8) wlan_le16_to_cpu(pibss_coal_resp->use_g_rate_protect);

        pmpriv->adhoc_state = ADHOC_COALESCED;
    }

    LEAVE();
    return MLAN_STATUS_SUCCESS;
}

/**
 *  @brief This function handles the command response of MGMT_IE_LIST
 *
 *  @param pmpriv       A pointer to mlan_private structure
 *  @param resp         A pointer to HostCmd_DS_COMMAND
 *  @param pioctl_buf   A pointer to mlan_ioctl_req structure
 *
 *  @return             MLAN_STATUS_SUCCESS
 */
static mlan_status
wlan_ret_mgmt_ie_list(IN pmlan_private pmpriv,
                      IN HostCmd_DS_COMMAND * resp,
                      OUT mlan_ioctl_req * pioctl_buf)
{
    t_u16 resp_len = 0, travel_len = 0;
    int i = 0;
    mlan_ds_misc_cfg *misc = MNULL;
    mlan_ds_misc_custom_ie *cust_ie = MNULL;
    custom_ie *cptr;
    tlvbuf_max_mgmt_ie *max_mgmt_ie = MNULL;
    HostCmd_DS_MGMT_IE_LIST_CFG *pmgmt_ie_list = &(resp->params.mgmt_ie_list);

    ENTER();

    if (wlan_le16_to_cpu(pmgmt_ie_list->action) == HostCmd_ACT_GEN_SET) {
        if ((pmpriv->adapter->state_rdh.stage == RDH_SET_CUSTOM_IE) ||
            (pmpriv->adapter->state_rdh.stage == RDH_REM_CUSTOM_IE))
            wlan_11h_radar_detected_callback((t_void *) pmpriv);
        LEAVE();
        return MLAN_STATUS_SUCCESS;
    }

    misc = (mlan_ds_misc_cfg *) pioctl_buf->pbuf;
    cust_ie = (mlan_ds_misc_custom_ie *) & pmgmt_ie_list->ds_mgmt_ie;
    max_mgmt_ie =
        (tlvbuf_max_mgmt_ie *) ((t_u8 *) cust_ie + cust_ie->len +
                                sizeof(MrvlIEtypesHeader_t));
    if (cust_ie) {
        cust_ie->type = wlan_le16_to_cpu(cust_ie->type);
        resp_len = cust_ie->len = wlan_le16_to_cpu(cust_ie->len);
        travel_len = 0;
        /* conversion for index, mask, len */
        if (resp_len == sizeof(t_u16))
            cust_ie->ie_data_list[0].ie_index =
                wlan_cpu_to_le16(cust_ie->ie_data_list[0].ie_index);

        while (resp_len > sizeof(t_u16)) {
            cptr =
                (custom_ie *) (((t_u8 *) cust_ie->ie_data_list) + travel_len);
            cptr->ie_index = wlan_le16_to_cpu(cptr->ie_index);
            cptr->mgmt_subtype_mask = wlan_le16_to_cpu(cptr->mgmt_subtype_mask);
            cptr->ie_length = wlan_le16_to_cpu(cptr->ie_length);
            travel_len += cptr->ie_length + sizeof(custom_ie) - MAX_IE_SIZE;
            resp_len -= cptr->ie_length + sizeof(custom_ie) - MAX_IE_SIZE;
        }
        memcpy(pmpriv->adapter, &misc->param.cust_ie, cust_ie,
               (cust_ie->len + sizeof(MrvlIEtypesHeader_t)));
        if (max_mgmt_ie) {
            max_mgmt_ie->type = wlan_le16_to_cpu(max_mgmt_ie->type);
            if (max_mgmt_ie->type == TLV_TYPE_MAX_MGMT_IE) {
                max_mgmt_ie->len = wlan_le16_to_cpu(max_mgmt_ie->len);
                max_mgmt_ie->count = wlan_le16_to_cpu(max_mgmt_ie->count);
                for (i = 0; i < max_mgmt_ie->count; i++) {
                    max_mgmt_ie->info[i].buf_size =
                        wlan_le16_to_cpu(max_mgmt_ie->info[i].buf_size);
                    max_mgmt_ie->info[i].buf_count =
                        wlan_le16_to_cpu(max_mgmt_ie->info[i].buf_count);
                }
                /* Append max_mgmt_ie TLV after custom_ie */
                memcpy(pmpriv->adapter,
                       (t_u8 *) & misc->param.cust_ie + (cust_ie->len +
                                                         sizeof
                                                         (MrvlIEtypesHeader_t)),
                       max_mgmt_ie,
                       max_mgmt_ie->len + sizeof(MrvlIEtypesHeader_t));
            }
        }
    }

    LEAVE();
    return MLAN_STATUS_SUCCESS;
}

/**
 *  @brief This function handles the command response of sysclock
 *
 *  @param pmpriv       A pointer to mlan_private structure
 *  @param resp         A pointer to HostCmd_DS_COMMAND
 *  @param pioctl_buf   A pointer to mlan_ioctl_req structure
 *
 *  @return        MLAN_STATUS_SUCCESS
 */
static mlan_status
wlan_ret_sysclock_cfg(IN pmlan_private pmpriv,
                      IN HostCmd_DS_COMMAND * resp,
                      IN mlan_ioctl_req * pioctl_buf)
{
    mlan_ds_misc_cfg *mis_ccfg = MNULL;
    HostCmd_DS_ECL_SYSTEM_CLOCK_CONFIG *clk_cfg = &resp->params.sys_clock_cfg;
    int i = 0;

    ENTER();

    if (pioctl_buf) {
        mis_ccfg = (mlan_ds_misc_cfg *) pioctl_buf->pbuf;
        mis_ccfg->param.sys_clock.cur_sys_clk =
            wlan_le16_to_cpu(clk_cfg->cur_sys_clk);
        mis_ccfg->param.sys_clock.sys_clk_type =
            wlan_le16_to_cpu(clk_cfg->sys_clk_type);
        mis_ccfg->param.sys_clock.sys_clk_num =
            wlan_le16_to_cpu(clk_cfg->sys_clk_len) / sizeof(t_u16);
        for (i = 0; i < mis_ccfg->param.sys_clock.sys_clk_num; i++)
            mis_ccfg->param.sys_clock.sys_clk[i] =
                wlan_le16_to_cpu(clk_cfg->sys_clk[i]);
    }

    LEAVE();
    return MLAN_STATUS_SUCCESS;
}

/**
 *  @brief This function handles the command response of reg_access
 *
 *  @param type         The type of reg access (MAC, BBP or RF)
 *  @param resp         A pointer to HostCmd_DS_COMMAND
 *  @param pioctl_buf   A pointer to command buffer
 *
 *  @return             MLAN_STATUS_SUCCESS or MLAN_STATUS_FAILURE
 */
static mlan_status
wlan_ret_reg_access(mlan_adapter * pmadapter,
                    t_u16 type,
                    IN HostCmd_DS_COMMAND * resp,
                    IN mlan_ioctl_req * pioctl_buf)
{
    mlan_ds_reg_mem *reg_mem = MNULL;
    mlan_ds_reg_rw *reg_rw = MNULL;

    ENTER();

    if (pioctl_buf) {
        reg_mem = (mlan_ds_reg_mem *) pioctl_buf->pbuf;
        reg_rw = &reg_mem->param.reg_rw;
        switch (type) {
        case HostCmd_CMD_MAC_REG_ACCESS:
            {
                HostCmd_DS_MAC_REG_ACCESS *reg;
                reg = (HostCmd_DS_MAC_REG_ACCESS *) & resp->params.mac_reg;
                reg_rw->offset = (t_u32) wlan_le16_to_cpu(reg->offset);
                reg_rw->value = wlan_le32_to_cpu(reg->value);
                break;
            }
        case HostCmd_CMD_BBP_REG_ACCESS:
            {
                HostCmd_DS_BBP_REG_ACCESS *reg;
                reg = (HostCmd_DS_BBP_REG_ACCESS *) & resp->params.bbp_reg;
                reg_rw->offset = (t_u32) wlan_le16_to_cpu(reg->offset);
                reg_rw->value = (t_u32) reg->value;
                break;
            }

        case HostCmd_CMD_RF_REG_ACCESS:
            {
                HostCmd_DS_RF_REG_ACCESS *reg;
                reg = (HostCmd_DS_RF_REG_ACCESS *) & resp->params.rf_reg;
                reg_rw->offset = (t_u32) wlan_le16_to_cpu(reg->offset);
                reg_rw->value = (t_u32) reg->value;
                break;
            }
        case HostCmd_CMD_CAU_REG_ACCESS:
            {
                HostCmd_DS_RF_REG_ACCESS *reg;
                reg = (HostCmd_DS_RF_REG_ACCESS *) & resp->params.rf_reg;
                reg_rw->offset = (t_u32) wlan_le16_to_cpu(reg->offset);
                reg_rw->value = (t_u32) reg->value;
                break;
            }
        case HostCmd_CMD_802_11_EEPROM_ACCESS:
            {
                mlan_ds_read_eeprom *eeprom = &reg_mem->param.rd_eeprom;
                HostCmd_DS_802_11_EEPROM_ACCESS *cmd_eeprom =
                    (HostCmd_DS_802_11_EEPROM_ACCESS *) & resp->params.eeprom;
                cmd_eeprom->byte_count =
                    wlan_le16_to_cpu(cmd_eeprom->byte_count);
                PRINTM(MINFO, "EEPROM read len=%x\n", cmd_eeprom->byte_count);
                if (eeprom->byte_count < cmd_eeprom->byte_count) {
                    eeprom->byte_count = 0;
                    PRINTM(MINFO, "EEPROM read return length is too big\n");
                    pioctl_buf->status_code = MLAN_ERROR_CMD_RESP_FAIL;
                    LEAVE();
                    return MLAN_STATUS_FAILURE;
                }
                eeprom->offset = wlan_le16_to_cpu(cmd_eeprom->offset);
                eeprom->byte_count = cmd_eeprom->byte_count;
                if (eeprom->byte_count > 0) {
                    memcpy(pmadapter, &eeprom->value, &cmd_eeprom->value,
                           MIN(MAX_EEPROM_DATA, eeprom->byte_count));
                    HEXDUMP("EEPROM", (char *) &eeprom->value,
                            MIN(MAX_EEPROM_DATA, eeprom->byte_count));
                }
                break;
            }
        default:
            pioctl_buf->status_code = MLAN_ERROR_CMD_RESP_FAIL;
            LEAVE();
            return MLAN_STATUS_FAILURE;
        }
    }

    LEAVE();
    return MLAN_STATUS_SUCCESS;
}

/**
 *  @brief This function handles the command response of mem_access
 *
 *  @param pmpriv       A pointer to mlan_private structure
 *  @param resp         A pointer to HostCmd_DS_COMMAND
 *  @param pioctl_buf   A pointer to command buffer
 *
 *  @return             MLAN_STATUS_SUCCESS
 */
static mlan_status
wlan_ret_mem_access(IN pmlan_private pmpriv,
                    IN HostCmd_DS_COMMAND * resp,
                    IN mlan_ioctl_req * pioctl_buf)
{
    mlan_ds_reg_mem *reg_mem = MNULL;
    mlan_ds_mem_rw *mem_rw = MNULL;
    HostCmd_DS_MEM_ACCESS *mem = (HostCmd_DS_MEM_ACCESS *) & resp->params.mem;

    ENTER();

    if (pioctl_buf) {
        reg_mem = (mlan_ds_reg_mem *) pioctl_buf->pbuf;
        mem_rw = &reg_mem->param.mem_rw;

        mem_rw->addr = wlan_le32_to_cpu(mem->addr);
        mem_rw->value = wlan_le32_to_cpu(mem->value);
    }

    LEAVE();
    return MLAN_STATUS_SUCCESS;
}

/**
 *  @brief This function handles the command response of inactivity timeout
 *
 *  @param pmpriv       A pointer to mlan_private structure
 *  @param resp	        A pointer to HostCmd_DS_COMMAND
 *  @param pioctl_buf   A pointer to command buffer
 *
 *  @return             MLAN_STATUS_SUCCESS
 */
static mlan_status
wlan_ret_inactivity_timeout(IN pmlan_private pmpriv,
                            IN HostCmd_DS_COMMAND * resp,
                            IN mlan_ioctl_req * pioctl_buf)
{
    mlan_ds_pm_cfg *pmcfg = MNULL;
    mlan_ds_inactivity_to *inac_to = MNULL;
    HostCmd_DS_INACTIVITY_TIMEOUT_EXT *cmd_inac_to =
        (HostCmd_DS_INACTIVITY_TIMEOUT_EXT *) & resp->params.inactivity_to;

    ENTER();

    if (pioctl_buf) {
        pmcfg = (mlan_ds_pm_cfg *) pioctl_buf->pbuf;
        inac_to = &pmcfg->param.inactivity_to;
        inac_to->timeout_unit = wlan_le16_to_cpu(cmd_inac_to->timeout_unit);
        inac_to->unicast_timeout =
            wlan_le16_to_cpu(cmd_inac_to->unicast_timeout);
        inac_to->mcast_timeout = wlan_le16_to_cpu(cmd_inac_to->mcast_timeout);
        inac_to->ps_entry_timeout =
            wlan_le16_to_cpu(cmd_inac_to->ps_entry_timeout);
    }

    LEAVE();
    return MLAN_STATUS_SUCCESS;
}

/**
 *  @brief This function handles the command response of
 *  subscribe event
 *
 *  @param pmpriv       A pointer to mlan_private structure
 *  @param resp         A pointer to HostCmd_DS_COMMAND
 *  @param pioctl_buf   A pointer to command buffer
 *
 *  @return             MLAN_STATUS_SUCCESS
 */
static mlan_status
wlan_ret_subscribe_event(IN pmlan_private pmpriv,
                         IN HostCmd_DS_COMMAND * resp,
                         IN mlan_ioctl_req * pioctl_buf)
{

    HostCmd_DS_SUBSCRIBE_EVENT *evt =
        (HostCmd_DS_SUBSCRIBE_EVENT *) & resp->params.subscribe_event;
    mlan_ds_subscribe_evt *sub_evt = MNULL;
    mlan_ds_misc_cfg *misc = MNULL;

    ENTER();
    if (pioctl_buf && (pioctl_buf->action == MLAN_ACT_GET)) {
        misc = (mlan_ds_misc_cfg *) pioctl_buf->pbuf;
        sub_evt = &misc->param.subscribe_event;
        sub_evt->evt_bitmap = wlan_le16_to_cpu(evt->event_bitmap);
        pioctl_buf->data_read_written = sizeof(mlan_ds_misc_cfg);
    }
    LEAVE();
    return MLAN_STATUS_SUCCESS;
}

/**
 *  @brief This function handles the command response of
 *  OTP user data
 *
 *  @param pmpriv       A pointer to mlan_private structure
 *  @param resp         A pointer to HostCmd_DS_COMMAND
 *  @param pioctl_buf   A pointer to command buffer
 *
 *  @return             MLAN_STATUS_SUCCESS
 */
static mlan_status
wlan_ret_otp_user_data(IN pmlan_private pmpriv,
                       IN HostCmd_DS_COMMAND * resp,
                       IN mlan_ioctl_req * pioctl_buf)
{

    HostCmd_DS_OTP_USER_DATA *cmd_user_data =
        (HostCmd_DS_OTP_USER_DATA *) & resp->params.otp_user_data;
    mlan_ds_misc_otp_user_data *user_data = MNULL;

    ENTER();
    if (pioctl_buf && (pioctl_buf->action == MLAN_ACT_GET)) {
        user_data = (mlan_ds_misc_otp_user_data *) pioctl_buf->pbuf;
        user_data->user_data_length = MIN(MAX_OTP_USER_DATA_LEN,
                                          wlan_le16_to_cpu(cmd_user_data->
                                                           user_data_length));
        memcpy(pmpriv->adapter, user_data->user_data, cmd_user_data->user_data,
               user_data->user_data_length);
        pioctl_buf->data_read_written =
            sizeof(mlan_ds_misc_otp_user_data) + user_data->user_data_length;
    }
    LEAVE();
    return MLAN_STATUS_SUCCESS;
}
#endif /* CONFIG_MLAN_WMSDK */

/********************************************************
                Global Functions
********************************************************/
/**
 *  @brief This function handles the station command response
 *
 *  @param priv             A pointer to mlan_private structure
 *  @param cmdresp_no       cmd no
 *  @param pcmd_buf         cmdresp buf
 *  @param pioctl           A pointer to ioctl buf
 *
 *  @return             MLAN_STATUS_SUCCESS or MLAN_STATUS_FAILURE
 */
mlan_status
wlan_ops_sta_process_cmdresp(IN t_void * priv,
                             IN t_u16 cmdresp_no,
                             IN t_void * pcmd_buf, IN t_void * pioctl)
{
    mlan_status ret = MLAN_STATUS_SUCCESS;
    mlan_private *pmpriv = (mlan_private *) priv;
    HostCmd_DS_COMMAND *resp = (HostCmd_DS_COMMAND *) pcmd_buf;
    mlan_ioctl_req *pioctl_buf = (mlan_ioctl_req *) pioctl;
    /* mlan_adapter *pmadapter = pmpriv->adapter; */
    /* int ctr; */

    ENTER();

 /* fixme: enable below code when required */
#ifndef CONFIG_MLAN_WMSDK
    /* If the command is not successful, cleanup and return failure */
    if ((resp->result != HostCmd_RESULT_OK)
        ) {
        wlan_process_cmdresp_error(pmpriv, resp, pioctl_buf);
        LEAVE();
        return MLAN_STATUS_FAILURE;
    }
#endif /* CONFIG_MLAN_WMSDK */
    /* Command successful, handle response */
    switch (cmdresp_no) {
    case HostCmd_CMD_GET_HW_SPEC:
        ret = wlan_ret_get_hw_spec(pmpriv, resp, pioctl_buf);
        break;
    case HostCmd_CMD_TXPWR_CFG:
	    ret = wlan_ret_tx_power_cfg(pmpriv, resp, pioctl_buf);
        break;
#ifndef CONFIG_MLAN_WMSDK
    case HostCmd_CMD_CFG_DATA:
        ret = wlan_ret_cfg_data(pmpriv, resp, pioctl_buf);
        break;
    case HostCmd_CMD_MAC_CONTROL:
        ret = wlan_ret_mac_control(pmpriv, resp, pioctl_buf);
        break;
    case HostCmd_CMD_802_11_MAC_ADDRESS:
        ret = wlan_ret_802_11_mac_address(pmpriv, resp, pioctl_buf);
        break;
    case HostCmd_CMD_MAC_MULTICAST_ADR:
        ret = wlan_ret_mac_multicast_adr(pmpriv, resp, pioctl_buf);
        break;
#endif /* CONFIG_MLAN_WMSDK */
    case HostCmd_CMD_TX_RATE_CFG:
        ret = wlan_ret_tx_rate_cfg(pmpriv, resp, pioctl_buf);
        break;
#ifndef EXT_SCAN_SUPPORT
    case HostCmd_CMD_802_11_SCAN:
        ret = wlan_ret_802_11_scan(pmpriv, resp, pioctl_buf);
        pioctl_buf = MNULL;
        /* pmadapter->curr_cmd->pioctl_buf = MNULL; */
        break;
#else
    case HostCmd_CMD_802_11_SCAN_EXT:
        ret = wlan_ret_802_11_scan_ext(pmpriv, resp, pioctl_buf);
        pioctl_buf = MNULL;
        break;
#endif /* EXT_SCAN_SUPPORT */
#ifndef CONFIG_MLAN_WMSDK
    case HostCmd_CMD_802_11_BG_SCAN_CONFIG:
        ret = wlan_ret_bgscan_config(pmpriv, resp, pioctl_buf);
        break;
    case HostCmd_CMD_802_11_BG_SCAN_QUERY:
        ret = wlan_ret_802_11_scan(pmpriv, resp, pioctl_buf);
        wlan_recv_event(pmpriv, MLAN_EVENT_ID_FW_BG_SCAN, MNULL);
        PRINTM(MINFO, "CMD_RESP: BG_SCAN result is ready!\n");
        break;
    case HostCmd_CMD_802_11_RF_TX_POWER:
        ret = wlan_ret_802_11_rf_tx_power(pmpriv, resp, pioctl_buf);
        break;

    case HostCmd_CMD_802_11_PS_MODE_ENH:
        ret = wlan_ret_enh_power_mode(pmpriv, resp, pioctl_buf);
        break;
    case HostCmd_CMD_802_11_HS_CFG_ENH:
        ret = wlan_ret_802_11_hs_cfg(pmpriv, resp, pioctl_buf);
        break;
    case HostCmd_CMD_802_11_SLEEP_PERIOD:
        ret = wlan_ret_802_11_sleep_period(pmpriv, resp, pioctl_buf);
        break;
    case HostCmd_CMD_802_11_SLEEP_PARAMS:
        ret = wlan_ret_802_11_sleep_params(pmpriv, resp, pioctl_buf);
        break;
#endif /* CONFIG_MLAN_WMSDK */
    case HostCmd_CMD_802_11_ASSOCIATE:
        ret = wlan_ret_802_11_associate(pmpriv, resp, pioctl_buf);
        break;
#ifndef CONFIG_MLAN_WMSDK
    case HostCmd_CMD_802_11_DEAUTHENTICATE:
        ret = wlan_ret_802_11_deauthenticate(pmpriv, resp, pioctl_buf);
        break;
    case HostCmd_CMD_802_11_AD_HOC_START:
    case HostCmd_CMD_802_11_AD_HOC_JOIN:
        ret = wlan_ret_802_11_ad_hoc(pmpriv, resp, pioctl_buf);
        break;
    case HostCmd_CMD_802_11_AD_HOC_STOP:
        ret = wlan_ret_802_11_ad_hoc_stop(pmpriv, resp, pioctl_buf);
        break;
#endif /* CONFIG_MLAN_WMSDK */
    case HostCmd_CMD_802_11_GET_LOG:
        ret = wlan_ret_get_log(pmpriv, resp, pioctl_buf);
        break;
#ifndef CONFIG_MLAN_WMSDK
    case HostCmd_CMD_RSSI_INFO:
        ret = wlan_ret_802_11_rssi_info(pmpriv, resp, pioctl_buf);
        break;
#endif /* CONFIG_MLAN_WMSDK */
    case HostCmd_CMD_802_11_SNMP_MIB:
        ret = wlan_ret_802_11_snmp_mib(pmpriv, resp, pioctl_buf);
        break;
#ifndef CONFIG_MLAN_WMSDK
    case HostCmd_CMD_802_11_RADIO_CONTROL:
        ret = wlan_ret_802_11_radio_control(pmpriv, resp, pioctl_buf);
        break;
#endif /* CONFIG_MLAN_WMSDK */
    case HostCmd_CMD_802_11_TX_RATE_QUERY:
        ret = wlan_ret_802_11_tx_rate_query(pmpriv, resp, pioctl_buf);
        break;
    case HostCmd_CMD_802_11_RF_CHANNEL:
        ret = wlan_ret_802_11_rf_channel(pmpriv, resp, pioctl_buf);
        break;
#ifndef CONFIG_MLAN_WMSDK
    case HostCmd_CMD_802_11_RF_ANTENNA:
        ret = wlan_ret_802_11_rf_antenna(pmpriv, resp, pioctl_buf);
        break;
    case HostCmd_CMD_VERSION_EXT:
        ret = wlan_ret_ver_ext(pmpriv, resp, pioctl_buf);
        break;
    case HostCmd_CMD_RX_MGMT_IND:
        ret = wlan_ret_rx_mgmt_ind(pmpriv, resp, pioctl_buf);
        break;
    case HostCmd_CMD_FUNC_INIT:
    case HostCmd_CMD_FUNC_SHUTDOWN:
        break;
    case HostCmd_CMD_802_11_KEY_MATERIAL:
        ret = wlan_ret_802_11_key_material(pmpriv, resp, pioctl_buf);
        break;
    case HostCmd_CMD_SUPPLICANT_PMK:
        ret = wlan_ret_802_11_supplicant_pmk(pmpriv, resp, pioctl_buf);
        break;
    case HostCmd_CMD_SUPPLICANT_PROFILE:
        ret = wlan_ret_802_11_supplicant_profile(pmpriv, resp, pioctl_buf);
        break;
    case HostCmd_CMD_802_11D_DOMAIN_INFO:
	    /* fixme: This command is sent. but the response handler does
	       not seem to be doing anything lasting. Skipping handling
	       response for now */
        ret = wlan_ret_802_11d_domain_info(pmpriv, resp);
        break;
    case HostCmd_CMD_802_11_TPC_ADAPT_REQ:
    case HostCmd_CMD_802_11_TPC_INFO:
    case HostCmd_CMD_802_11_CHAN_SW_ANN:
    case HostCmd_CMD_CHAN_REPORT_REQUEST:
        ret = wlan_11h_cmdresp_process(pmpriv, resp);
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
    case HostCmd_CMD_WMM_GET_STATUS:
        ret = wlan_ret_wmm_get_status(pmpriv,
                                      resp->params.get_wmm_status.
                                      queue_status_tlv, resp->size - S_DS_GEN);
        break;
    case HostCmd_CMD_WMM_ADDTS_REQ:
        ret = wlan_ret_wmm_addts_req(pmpriv, resp, pioctl_buf);
        break;
    case HostCmd_CMD_WMM_DELTS_REQ:
        ret = wlan_ret_wmm_delts_req(pmpriv, resp, pioctl_buf);
        break;
    case HostCmd_CMD_WMM_QUEUE_CONFIG:
        ret = wlan_ret_wmm_queue_config(pmpriv, resp, pioctl_buf);
        break;
    case HostCmd_CMD_WMM_QUEUE_STATS:
        ret = wlan_ret_wmm_queue_stats(pmpriv, resp, pioctl_buf);
        break;
    case HostCmd_CMD_WMM_TS_STATUS:
        ret = wlan_ret_wmm_ts_status(pmpriv, resp, pioctl_buf);
        break;
    case HostCmd_CMD_802_11_IBSS_COALESCING_STATUS:
        ret = wlan_ret_ibss_coalescing_status(pmpriv, resp);
        break;
    case HostCmd_CMD_MGMT_IE_LIST:
        ret = wlan_ret_mgmt_ie_list(pmpriv, resp, pioctl_buf);
        break;
    case HostCmd_CMD_11N_CFG:
        ret = wlan_ret_11n_cfg(pmpriv, resp, pioctl_buf);
        break;
    case HostCmd_CMD_ECL_SYSTEM_CLOCK_CONFIG:
        ret = wlan_ret_sysclock_cfg(pmpriv, resp, pioctl_buf);
        break;
    case HostCmd_CMD_MAC_REG_ACCESS:
    case HostCmd_CMD_BBP_REG_ACCESS:
    case HostCmd_CMD_RF_REG_ACCESS:
    case HostCmd_CMD_CAU_REG_ACCESS:
    case HostCmd_CMD_802_11_EEPROM_ACCESS:
        ret =
            wlan_ret_reg_access(pmpriv->adapter, cmdresp_no, resp, pioctl_buf);
        break;
    case HostCmd_CMD_MEM_ACCESS:
        ret = wlan_ret_mem_access(pmpriv, resp, pioctl_buf);
        break;
    case HostCmd_CMD_INACTIVITY_TIMEOUT_EXT:
        ret = wlan_ret_inactivity_timeout(pmpriv, resp, pioctl_buf);
        break;
    case HostCmd_CMD_SDIO_GPIO_INT_CONFIG:
        break;
    case HostCmd_CMD_SET_BSS_MODE:
        break;
    case HostCmd_CMD_MEASUREMENT_REQUEST:
    case HostCmd_CMD_MEASUREMENT_REPORT:
        ret = wlan_meas_cmdresp_process(pmpriv, resp);
        break;
#ifdef WIFI_DIRECT_SUPPORT
    case HostCmd_CMD_802_11_REMAIN_ON_CHANNEL:
        ret = wlan_ret_remain_on_channel(pmpriv, resp, pioctl_buf);
        break;
    case HOST_CMD_WIFI_DIRECT_MODE_CONFIG:
        ret = wlan_ret_wifi_direct_mode(pmpriv, resp, pioctl_buf);
        break;
#endif
    case HostCmd_CMD_802_11_SUBSCRIBE_EVENT:
        ret = wlan_ret_subscribe_event(pmpriv, resp, pioctl_buf);
        break;
    case HostCmd_CMD_OTP_READ_USER_DATA:
        ret = wlan_ret_otp_user_data(pmpriv, resp, pioctl_buf);
        break;
    case HostCMD_CONFIG_LOW_POWER_MODE:
	break;
#endif /* CONFIG_MLAN_WMSDK */
    default:
        PRINTM(MERROR, "CMD_RESP: Unknown command response %#x\n",
               resp->command);
        break;
    }

    LEAVE();
    return ret;
}
