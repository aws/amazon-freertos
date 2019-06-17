/** @file mlan_uap_ioctl.c
 *
 *  @brief  This file provides handling of AP mode ioctls
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

#ifndef CONFIG_MLAN_WMSDK
/********************************************************
                Global Variables
********************************************************/
extern t_u8 tos_to_tid_inv[];

/********************************************************
    Local Functions
********************************************************/
/**
 *  @brief Stop BSS
 *
 *  @param pmadapter	A pointer to mlan_adapter structure
 *  @param pioctl_req	A pointer to ioctl request buffer
 *
 *  @return		MLAN_STATUS_PENDING --success, otherwise fail
 */

static mlan_status
wlan_uap_bss_ioctl_stop(IN pmlan_adapter pmadapter,
                        IN pmlan_ioctl_req pioctl_req)
{
    mlan_status ret = MLAN_STATUS_SUCCESS;
    pmlan_private pmpriv = pmadapter->priv[pioctl_req->bss_index];

    ENTER();

    ret = wlan_prepare_cmd(pmpriv,
                           HOST_CMD_APCMD_BSS_STOP,
                           HostCmd_ACT_GEN_SET,
                           0, (t_void *) pioctl_req, MNULL);
    if (ret == MLAN_STATUS_SUCCESS)
        ret = MLAN_STATUS_PENDING;

    LEAVE();
    return ret;
}

/**
 *  @brief Callback to finish BSS IOCTL START
 *  Not to be called directly to initiate bss_start
 *
 *  @param priv A pointer to mlan_private structure (cast from t_void*)
 *
 *  @return     MLAN_STATUS_PENDING --success, otherwise fail
 *  @sa         wlan_uap_bss_ioctl_start
 */
static mlan_status
wlan_uap_callback_bss_ioctl_start(IN t_void * priv)
{
    mlan_status ret = MLAN_STATUS_SUCCESS;
    mlan_private *pmpriv = (mlan_private *) priv;
    mlan_callbacks *pcb = (mlan_callbacks *) & pmpriv->adapter->callbacks;
    wlan_uap_get_info_cb_t *puap_state_chan_cb = &pmpriv->uap_state_chan_cb;
    t_u8 old_channel;

    ENTER();
    /* clear callback now that we're here */
    puap_state_chan_cb->get_chan_callback = MNULL;

    /*
     * Check if the region and channel requires we check for radar.
     */
    if ((puap_state_chan_cb->band_config & BAND_CONFIG_5GHZ) &&
        wlan_11h_radar_detect_required(pmpriv, puap_state_chan_cb->channel)) {
        /* first check if channel is under NOP */
        if (wlan_11h_is_channel_under_nop(pmpriv->adapter,
                                          puap_state_chan_cb->channel)) {
            /* recently we've seen radar on this channel */
            ret = MLAN_STATUS_FAILURE;
        }

        /* Check cached radar check on the channel */
        if (ret == MLAN_STATUS_SUCCESS)
            ret =
                wlan_11h_check_chan_report(pmpriv, puap_state_chan_cb->channel);

        /* Found radar: try to switch to a non-dfs channel */
        if (ret != MLAN_STATUS_SUCCESS) {
            old_channel = puap_state_chan_cb->channel;
            ret =
                wlan_11h_switch_non_dfs_chan(pmpriv,
                                             &puap_state_chan_cb->channel);

            if (ret == MLAN_STATUS_SUCCESS) {
                ret = wlan_uap_set_channel(pmpriv,
                                           pmpriv->uap_state_chan_cb.
                                           band_config,
                                           puap_state_chan_cb->channel);
                if (ret == MLAN_STATUS_SUCCESS) {
                    PRINTM(MMSG, "Radar found on channel %d,"
                           "switch to new channel %d.\n",
                           old_channel, puap_state_chan_cb->channel);
                } else {
                    PRINTM(MMSG, "Radar found on channel %d,"
                           " switch to new channel %d failed.\n",
                           old_channel, puap_state_chan_cb->channel);
                    pcb->moal_ioctl_complete(pmpriv->adapter->pmoal_handle,
                                             puap_state_chan_cb->
                                             pioctl_req_curr,
                                             MLAN_STATUS_FAILURE);
                    goto done;
                }
            } else {
                PRINTM(MMSG,
                       "Radar found on channel %d, no switch channel available.\n",
                       old_channel);
                /* No command sent with the ioctl, need manually signal
                   completion */
                pcb->moal_ioctl_complete(pmpriv->adapter->pmoal_handle,
                                         puap_state_chan_cb->pioctl_req_curr,
                                         MLAN_STATUS_FAILURE);
                goto done;
            }
        } else {
            PRINTM(MINFO, "No Radar found on channel %d\n",
                   puap_state_chan_cb->channel);
        }
    }

    /* else okay to send command: not DFS channel or no radar */
    ret = wlan_prepare_cmd(pmpriv,
                           HOST_CMD_APCMD_BSS_START,
                           HostCmd_ACT_GEN_SET,
                           0,
                           (t_void *) puap_state_chan_cb->pioctl_req_curr,
                           MNULL);
    if (ret == MLAN_STATUS_SUCCESS)
        ret = MLAN_STATUS_PENDING;

  done:
    puap_state_chan_cb->pioctl_req_curr = MNULL;        // prevent re-use
    LEAVE();
    return ret;
}

/**
 *  @brief Start BSS
 *
 *  @param pmadapter	A pointer to mlan_adapter structure
 *  @param pioctl_req	A pointer to ioctl request buffer
 *
 *  @return		MLAN_STATUS_PENDING --success, otherwise fail
 */
/**
 *  @sa         wlan_uap_callback_bss_ioctl_start
 */
static mlan_status
wlan_uap_bss_ioctl_start(IN pmlan_adapter pmadapter,
                         IN pmlan_ioctl_req pioctl_req)
{
    mlan_status ret = MLAN_STATUS_SUCCESS;
    pmlan_private pmpriv = pmadapter->priv[pioctl_req->bss_index];

    ENTER();

    /* First check channel report, defer BSS_START CMD to callback. */
    /* store params, issue command to get UAP channel, whose CMD_RESP will
       callback remainder of bss_start handling */
    pmpriv->uap_state_chan_cb.pioctl_req_curr = pioctl_req;
    pmpriv->uap_state_chan_cb.get_chan_callback =
        wlan_uap_callback_bss_ioctl_start;

    ret = wlan_uap_get_channel(pmpriv);
    if (ret == MLAN_STATUS_SUCCESS)
        ret = MLAN_STATUS_PENDING;

    LEAVE();
    return ret;
}

/**
 *  @brief reset BSS
 *
 *  @param pmadapter	A pointer to mlan_adapter structure
 *  @param pioctl_req	A pointer to ioctl request buffer
 *
 *  @return		MLAN_STATUS_PENDING --success, otherwise fail
 */
static mlan_status
wlan_uap_bss_ioctl_reset(IN pmlan_adapter pmadapter,
                         IN pmlan_ioctl_req pioctl_req)
{
    mlan_status ret = MLAN_STATUS_SUCCESS;
    pmlan_private pmpriv = pmadapter->priv[pioctl_req->bss_index];
    t_u8 i = 0;

    ENTER();

    /*
     * Reset any uap private parameters here
     */
    for (i = 0; i < pmadapter->max_mgmt_ie_index; i++) {
        memset(pmadapter, &pmpriv->mgmt_ie[i], 0, sizeof(custom_ie));
    }
    pmpriv->add_ba_param.timeout = MLAN_DEFAULT_BLOCK_ACK_TIMEOUT;
    pmpriv->add_ba_param.tx_win_size = MLAN_UAP_AMPDU_DEF_TXWINSIZE;
    pmpriv->add_ba_param.rx_win_size = MLAN_UAP_AMPDU_DEF_RXWINSIZE;
    for (i = 0; i < MAX_NUM_TID; i++) {
        pmpriv->aggr_prio_tbl[i].ampdu_user = tos_to_tid_inv[i];
        pmpriv->aggr_prio_tbl[i].amsdu = BA_STREAM_NOT_ALLOWED;
        pmpriv->addba_reject[i] = ADDBA_RSP_STATUS_ACCEPT;
    }

    /* hs_configured, hs_activated are reset by main loop */
    pmadapter->hs_cfg.conditions = HOST_SLEEP_DEF_COND;
    pmadapter->hs_cfg.gpio = HOST_SLEEP_DEF_GPIO;
    pmadapter->hs_cfg.gap = HOST_SLEEP_DEF_GAP;

    ret = wlan_prepare_cmd(pmpriv,
                           HOST_CMD_APCMD_SYS_RESET,
                           HostCmd_ACT_GEN_SET,
                           0, (t_void *) pioctl_req, MNULL);
    if (ret == MLAN_STATUS_SUCCESS)
        ret = MLAN_STATUS_PENDING;

    LEAVE();
    return ret;
}

/**
 *  @brief Set/Get MAC address
 *
 *  @param pmadapter	A pointer to mlan_adapter structure
 *  @param pioctl_req	A pointer to ioctl request buffer
 *
 *  @return		MLAN_STATUS_PENDING --success, otherwise fail
 */
static mlan_status
wlan_uap_bss_ioctl_mac_address(IN pmlan_adapter pmadapter,
                               IN pmlan_ioctl_req pioctl_req)
{
    mlan_private *pmpriv = pmadapter->priv[pioctl_req->bss_index];
    mlan_ds_bss *bss = MNULL;
    mlan_status ret = MLAN_STATUS_SUCCESS;
    t_u16 cmd_action = 0;

    ENTER();

    bss = (mlan_ds_bss *) pioctl_req->pbuf;
    if (pioctl_req->action == MLAN_ACT_SET) {
        memcpy(pmadapter, pmpriv->curr_addr, &bss->param.mac_addr,
               MLAN_MAC_ADDR_LENGTH);
        cmd_action = HostCmd_ACT_GEN_SET;
    } else
        cmd_action = HostCmd_ACT_GEN_GET;
    /* Send request to firmware */
    ret = wlan_prepare_cmd(pmpriv, HOST_CMD_APCMD_SYS_CONFIGURE,
                           cmd_action, 0, (t_void *) pioctl_req, MNULL);

    if (ret == MLAN_STATUS_SUCCESS)
        ret = MLAN_STATUS_PENDING;

    LEAVE();
    return ret;
}

/**
 *  @brief Get Uap statistics
 *
 *  @param pmadapter	A pointer to mlan_adapter structure
 *  @param pioctl_req	A pointer to ioctl request buffer
 *
 *  @return		MLAN_STATUS_PENDING --success, otherwise fail
 */
static mlan_status
wlan_uap_get_stats(IN pmlan_adapter pmadapter, IN pmlan_ioctl_req pioctl_req)
{
    mlan_status ret = MLAN_STATUS_SUCCESS;
    pmlan_private pmpriv = pmadapter->priv[pioctl_req->bss_index];

    ENTER();

    ret = wlan_prepare_cmd(pmpriv,
                           HostCmd_CMD_802_11_SNMP_MIB,
                           HostCmd_ACT_GEN_GET,
                           0, (t_void *) pioctl_req, MNULL);
    if (ret == MLAN_STATUS_SUCCESS)
        ret = MLAN_STATUS_PENDING;

    LEAVE();
    return ret;
}

/**
 *  @brief Set/Get AP config
 *
 *  @param pmadapter	A pointer to mlan_adapter structure
 *  @param pioctl_req	A pointer to ioctl request buffer
 *
 *  @return		MLAN_STATUS_PENDING --success, otherwise fail
 */
static mlan_status
wlan_uap_bss_ioctl_config(IN pmlan_adapter pmadapter,
                          IN pmlan_ioctl_req pioctl_req)
{
    mlan_private *pmpriv = pmadapter->priv[pioctl_req->bss_index];
    mlan_status ret = MLAN_STATUS_SUCCESS;
    t_u16 cmd_action = 0;

    ENTER();

    if (pioctl_req->action == MLAN_ACT_SET)
        cmd_action = HostCmd_ACT_GEN_SET;
    else
        cmd_action = HostCmd_ACT_GEN_GET;

    /* Send request to firmware */
    ret = wlan_prepare_cmd(pmpriv, HOST_CMD_APCMD_SYS_CONFIGURE,
                           cmd_action, 0, (t_void *) pioctl_req, MNULL);

    if (ret == MLAN_STATUS_SUCCESS)
        ret = MLAN_STATUS_PENDING;

    LEAVE();
    return ret;
}

/**
 *  @brief deauth sta
 *
 *  @param pmadapter	A pointer to mlan_adapter structure
 *  @param pioctl_req	A pointer to ioctl request buffer
 *
 *  @return		MLAN_STATUS_PENDING --success, otherwise fail
 */
static mlan_status
wlan_uap_bss_ioctl_deauth_sta(IN pmlan_adapter pmadapter,
                              IN pmlan_ioctl_req pioctl_req)
{
    mlan_status ret = MLAN_STATUS_SUCCESS;
    pmlan_private pmpriv = pmadapter->priv[pioctl_req->bss_index];
    mlan_ds_bss *bss = MNULL;

    ENTER();

    bss = (mlan_ds_bss *) pioctl_req->pbuf;
    ret = wlan_prepare_cmd(pmpriv,
                           HOST_CMD_APCMD_STA_DEAUTH,
                           HostCmd_ACT_GEN_SET,
                           0,
                           (t_void *) pioctl_req,
                           (t_void *) & bss->param.deauth_param);
    if (ret == MLAN_STATUS_SUCCESS)
        ret = MLAN_STATUS_PENDING;

    LEAVE();
    return ret;
}

/**
 *  @brief Get station list
 *
 *  @param pmadapter	A pointer to mlan_adapter structure
 *  @param pioctl_req	A pointer to ioctl request buffer
 *
 *  @return		MLAN_STATUS_PENDING --success, otherwise fail
 */
static mlan_status
wlan_uap_get_sta_list(IN pmlan_adapter pmadapter, IN pmlan_ioctl_req pioctl_req)
{
    pmlan_private pmpriv = pmadapter->priv[pioctl_req->bss_index];
    mlan_status ret = MLAN_STATUS_SUCCESS;

    ENTER();

    /* Send request to firmware */
    ret = wlan_prepare_cmd(pmpriv,
                           HOST_CMD_APCMD_STA_LIST,
                           HostCmd_ACT_GEN_GET,
                           0, (t_void *) pioctl_req, MNULL);

    if (ret == MLAN_STATUS_SUCCESS)
        ret = MLAN_STATUS_PENDING;

    LEAVE();
    return ret;
}

/**
 *  @brief soft_reset
 *
 *  @param pmadapter	A pointer to mlan_adapter structure
 *  @param pioctl_req	A pointer to ioctl request buffer
 *
 *  @return		MLAN_STATUS_PENDING --success, otherwise fail
 */
static mlan_status
wlan_uap_misc_ioctl_soft_reset(IN pmlan_adapter pmadapter,
                               IN pmlan_ioctl_req pioctl_req)
{
    mlan_private *pmpriv = pmadapter->priv[pioctl_req->bss_index];
    mlan_status ret = MLAN_STATUS_SUCCESS;

    ENTER();

    ret = wlan_prepare_cmd(pmpriv,
                           HostCmd_CMD_SOFT_RESET,
                           HostCmd_ACT_GEN_SET,
                           0, (t_void *) pioctl_req, MNULL);
    if (ret == MLAN_STATUS_SUCCESS)
        ret = MLAN_STATUS_PENDING;

    LEAVE();
    return ret;
}

/**
 *  @brief Tx data pause
 *
 *  @param pmadapter	A pointer to mlan_adapter structure
 *  @param pioctl_req	A pointer to ioctl request buffer
 *
 *  @return		MLAN_STATUS_PENDING --success, otherwise fail
 */
static mlan_status
wlan_uap_misc_ioctl_txdatapause(IN pmlan_adapter pmadapter,
                                IN pmlan_ioctl_req pioctl_req)
{
    mlan_private *pmpriv = pmadapter->priv[pioctl_req->bss_index];
    mlan_ds_misc_cfg *pmisc = (mlan_ds_misc_cfg *) pioctl_req->pbuf;
    mlan_status ret = MLAN_STATUS_SUCCESS;
    t_u16 cmd_action = 0;

    ENTER();

    if (pioctl_req->action == MLAN_ACT_SET)
        cmd_action = HostCmd_ACT_GEN_SET;
    else
        cmd_action = HostCmd_ACT_GEN_GET;
    ret = wlan_prepare_cmd(pmpriv,
                           HostCmd_CMD_CFG_TX_DATA_PAUSE,
                           cmd_action,
                           0,
                           (t_void *) pioctl_req, &(pmisc->param.tx_datapause));
    if (ret == MLAN_STATUS_SUCCESS)
        ret = MLAN_STATUS_PENDING;

    LEAVE();
    return ret;
}

/**
 *  @brief Set/Get Power mode
 *
 *  @param pmadapter	A pointer to mlan_adapter structure
 *  @param pioctl_req	A pointer to ioctl request buffer
 *
 *  @return		MLAN_STATUS_PENDING --success, otherwise fail
 */
static mlan_status
wlan_uap_pm_ioctl_mode(IN pmlan_adapter pmadapter,
                       IN pmlan_ioctl_req pioctl_req)
{
    mlan_private *pmpriv = pmadapter->priv[pioctl_req->bss_index];
    mlan_ds_pm_cfg *pm = MNULL;
    mlan_status ret = MLAN_STATUS_SUCCESS;
    t_u16 cmd_action = 0;
    t_u32 cmd_oid = 0;

    ENTER();

    pm = (mlan_ds_pm_cfg *) pioctl_req->pbuf;
    if (pioctl_req->action == MLAN_ACT_SET) {
        if (pm->param.ps_mgmt.ps_mode == PS_MODE_INACTIVITY) {
            cmd_action = EN_AUTO_PS;
            cmd_oid = BITMAP_UAP_INACT_PS;
        } else if (pm->param.ps_mgmt.ps_mode == PS_MODE_PERIODIC_DTIM) {
            cmd_action = EN_AUTO_PS;
            cmd_oid = BITMAP_UAP_DTIM_PS;
        } else {
            cmd_action = DIS_AUTO_PS;
            cmd_oid = BITMAP_UAP_INACT_PS | BITMAP_UAP_DTIM_PS;
        }
    } else {
        cmd_action = GET_PS;
        cmd_oid = BITMAP_UAP_INACT_PS | BITMAP_UAP_DTIM_PS;
    }
    /* Send request to firmware */
    ret = wlan_prepare_cmd(pmpriv,
                           HostCmd_CMD_802_11_PS_MODE_ENH,
                           cmd_action, cmd_oid, (t_void *) pioctl_req,
                           (t_void *) & pm->param.ps_mgmt);
    if ((ret == MLAN_STATUS_SUCCESS) &&
        (pioctl_req->action == MLAN_ACT_SET) && (cmd_action == DIS_AUTO_PS)) {
        ret = wlan_prepare_cmd(pmpriv,
                               HostCmd_CMD_802_11_PS_MODE_ENH, GET_PS,
                               0, MNULL, MNULL);
    }
    if (ret == MLAN_STATUS_SUCCESS)
        ret = MLAN_STATUS_PENDING;

    LEAVE();
    return ret;
}

/**
 *  @brief Set WAPI IE
 *
 *  @param priv         A pointer to mlan_private structure
 *  @param pioctl_req	A pointer to ioctl request buffer
 *
 *  @return             MLAN_STATUS_PENDING --success, otherwise fail
 */
static mlan_status
wlan_uap_set_wapi_ie(mlan_private * priv, pmlan_ioctl_req pioctl_req)
{
    mlan_ds_misc_cfg *misc = MNULL;
    mlan_status ret = MLAN_STATUS_SUCCESS;

    ENTER();
    misc = (mlan_ds_misc_cfg *) pioctl_req->pbuf;
    if (misc->param.gen_ie.len) {
        if (misc->param.gen_ie.len > sizeof(priv->wapi_ie)) {
            PRINTM(MWARN, "failed to copy WAPI IE, too big \n");
            pioctl_req->status_code = MLAN_ERROR_INVALID_PARAMETER;
            LEAVE();
            return MLAN_STATUS_FAILURE;
        }
        memcpy(priv->adapter, priv->wapi_ie, misc->param.gen_ie.ie_data,
               misc->param.gen_ie.len);
        priv->wapi_ie_len = misc->param.gen_ie.len;
        PRINTM(MIOCTL, "Set wapi_ie_len=%d IE=%#x\n", priv->wapi_ie_len,
               priv->wapi_ie[0]);
        DBG_HEXDUMP(MCMD_D, "wapi_ie", priv->wapi_ie, priv->wapi_ie_len);
        if (priv->wapi_ie[0] == WAPI_IE)
            priv->sec_info.wapi_enabled = MTRUE;
    } else {
        memset(priv->adapter, priv->wapi_ie, 0, sizeof(priv->wapi_ie));
        priv->wapi_ie_len = misc->param.gen_ie.len;
        PRINTM(MINFO, "Reset wapi_ie_len=%d IE=%#x\n", priv->wapi_ie_len,
               priv->wapi_ie[0]);
        priv->sec_info.wapi_enabled = MFALSE;
    }

    /* Send request to firmware */
    ret = wlan_prepare_cmd(priv, HOST_CMD_APCMD_SYS_CONFIGURE,
                           HostCmd_ACT_GEN_SET, 0, (t_void *) pioctl_req,
                           MNULL);

    if (ret == MLAN_STATUS_SUCCESS)
        ret = MLAN_STATUS_PENDING;

    LEAVE();
    return ret;
}

/**
 *  @brief Set generic IE
 *
 *  @param pmadapter	A pointer to mlan_adapter structure
 *  @param pioctl_req	A pointer to ioctl request buffer
 *
 *  @return		MLAN_STATUS_SUCCESS/MLAN_STATUS_PENDING --success, otherwise fail
 */
static mlan_status
wlan_uap_misc_ioctl_gen_ie(IN pmlan_adapter pmadapter,
                           IN pmlan_ioctl_req pioctl_req)
{
    mlan_status ret = MLAN_STATUS_SUCCESS;
    mlan_private *pmpriv = pmadapter->priv[pioctl_req->bss_index];
    mlan_ds_misc_cfg *misc = MNULL;
    IEEEtypes_VendorHeader_t *pvendor_ie = MNULL;

    ENTER();

    misc = (mlan_ds_misc_cfg *) pioctl_req->pbuf;

    if ((misc->param.gen_ie.type == MLAN_IE_TYPE_GEN_IE) &&
        (pioctl_req->action == MLAN_ACT_SET)) {
        if (misc->param.gen_ie.len) {
            pvendor_ie =
                (IEEEtypes_VendorHeader_t *) misc->param.gen_ie.ie_data;
            if (pvendor_ie->element_id == WAPI_IE) {
                /* IE is a WAPI IE so call set_wapi function */
                ret = wlan_uap_set_wapi_ie(pmpriv, pioctl_req);
            }
        } else {
            /* clear WAPI IE */
            ret = wlan_uap_set_wapi_ie(pmpriv, pioctl_req);
        }
    }
    LEAVE();
    return ret;
}

/**
 *  @brief Set/Get WAPI status
 *
 *  @param pmadapter	A pointer to mlan_adapter structure
 *  @param pioctl_req	A pointer to ioctl request buffer
 *
 *  @return		MLAN_STATUS_SUCCESS --success
 */
static mlan_status
wlan_uap_sec_ioctl_wapi_enable(IN pmlan_adapter pmadapter,
                               IN pmlan_ioctl_req pioctl_req)
{
    mlan_status ret = MLAN_STATUS_SUCCESS;
    mlan_private *pmpriv = pmadapter->priv[pioctl_req->bss_index];
    mlan_ds_sec_cfg *sec = MNULL;
    ENTER();
    sec = (mlan_ds_sec_cfg *) pioctl_req->pbuf;
    if (pioctl_req->action == MLAN_ACT_GET) {
        if (pmpriv->wapi_ie_len)
            sec->param.wapi_enabled = MTRUE;
        else
            sec->param.wapi_enabled = MFALSE;
    } else {
        if (sec->param.wapi_enabled == MFALSE) {
            memset(pmpriv->adapter, pmpriv->wapi_ie, 0,
                   sizeof(pmpriv->wapi_ie));
            pmpriv->wapi_ie_len = 0;
            PRINTM(MINFO, "Reset wapi_ie_len=%d IE=%#x\n", pmpriv->wapi_ie_len,
                   pmpriv->wapi_ie[0]);
            pmpriv->sec_info.wapi_enabled = MFALSE;
        }
    }
    pioctl_req->data_read_written = sizeof(t_u32) + MLAN_SUB_COMMAND_SIZE;
    LEAVE();
    return ret;
}

#ifdef HOST_AUTHENTICATOR
/**
 *  @brief report mic error
 *
 *  @param pmadapter	A pointer to mlan_adapter structure
 *  @param pioctl_req	A pointer to ioctl request buffer
 *
 *  @return		MLAN_STATUS_PENDING --success, otherwise fail
 */
static mlan_status wlan_uap_sec_ioctl_report_mic_error(
		IN pmlan_adapter    pmadapter,
		IN pmlan_ioctl_req  pioctl_req)
{
	mlan_status  ret = MLAN_STATUS_SUCCESS;
	pmlan_private pmpriv = pmadapter->priv[pioctl_req->bss_index];
	mlan_ds_sec_cfg *sec = MNULL;

	ENTER();

	sec = (mlan_ds_sec_cfg *)pioctl_req->pbuf;
	ret = wlan_prepare_cmd(pmpriv,
			HOST_CMD_APCMD_REPORT_MIC,
			HostCmd_ACT_GEN_SET,
			0,
			(t_void *)pioctl_req,
			(t_void *)sec->param.sta_mac);
	if (ret == MLAN_STATUS_SUCCESS)
		ret = MLAN_STATUS_PENDING;

	LEAVE();
	return ret;
}
#endif
#endif /* CONFIG_MLAN_WMSDK */


#if defined(WAPI_AP) || defined(HOST_AUTHENTICATOR)
/**
 *  @brief Set encrypt key
 *
 *  @param pmadapter	A pointer to mlan_adapter structure
 *  @param pioctl_req	A pointer to ioctl request buffer
 *
 *  @return		MLAN_STATUS_SUCCESS/MLAN_STATUS_PENDING --success, otherwise fail
 */
static mlan_status wlan_uap_sec_ioctl_set_encrypt_key(
		IN pmlan_adapter    pmadapter,
		IN pmlan_ioctl_req  pioctl_req)
{
	mlan_status ret = MLAN_STATUS_SUCCESS;
	mlan_private    *pmpriv = pmadapter->priv[pioctl_req->bss_index];
	mlan_ds_sec_cfg *sec = MNULL;

	ENTER();
	sec = (mlan_ds_sec_cfg *)pioctl_req->pbuf;
	if(pioctl_req->action != MLAN_ACT_SET) {
		pioctl_req->status_code = MLAN_ERROR_IOCTL_INVALID;
		LEAVE();
		return MLAN_STATUS_FAILURE;
	}
#ifdef KEY_PARAM_SET_V2
	if (!sec->param.encrypt_key.key_remove &&
		!sec->param.encrypt_key.key_len) {
#else
		if(!sec->param.encrypt_key.key_len){
#endif /* KEY_PARAM_SET_V2 */
			PRINTM(MCMND,"Skip set key with key_len = 0\n");
			LEAVE();
			return ret;
		}
		ret = wlan_prepare_cmd(pmpriv,
				HostCmd_CMD_802_11_KEY_MATERIAL,
				HostCmd_ACT_GEN_SET,
				KEY_INFO_ENABLED,
				(t_void *)pioctl_req,
				&sec->param.encrypt_key);

		if (ret == MLAN_STATUS_SUCCESS)
			ret = MLAN_STATUS_PENDING;
		LEAVE();
		return ret;
	}
#endif

#ifndef CONFIG_MLAN_WMSDK
/**
 *  @brief Get BSS information
 *
 *  @param pmadapter	A pointer to mlan_adapter structure
 *  @param pioctl_req	A pointer to ioctl request buffer
 *
 *  @return		MLAN_STATUS_SUCCESS --success
 */
static mlan_status
wlan_uap_get_bss_info(IN pmlan_adapter pmadapter, IN pmlan_ioctl_req pioctl_req)
{
    pmlan_private pmpriv = pmadapter->priv[pioctl_req->bss_index];
    mlan_status ret = MLAN_STATUS_SUCCESS;
    mlan_ds_get_info *info;

    ENTER();

    info = (mlan_ds_get_info *) pioctl_req->pbuf;
    /* Connection status */
    info->param.bss_info.media_connected = pmpriv->media_connected;

    /* Radio status */
    info->param.bss_info.radio_on = pmadapter->radio_on;

    /* BSSID */
    memcpy(pmadapter, &info->param.bss_info.bssid, pmpriv->curr_addr,
           MLAN_MAC_ADDR_LENGTH);
    info->param.bss_info.is_hs_configured = pmadapter->is_hs_configured;
    pioctl_req->data_read_written =
        sizeof(mlan_bss_info) + MLAN_SUB_COMMAND_SIZE;

    LEAVE();
    return ret;
}

/**
 *  @brief Set Host Sleep configurations
 *
 *  @param pmadapter	A pointer to mlan_adapter structure
 *  @param pioctl_req	A pointer to ioctl request buffer
 *
 *  @return		MLAN_STATUS_SUCCES/MLAN_STATUS_PENDING --success, otherwise fail
 */
static mlan_status
wlan_uap_pm_ioctl_deepsleep(IN pmlan_adapter pmadapter,
                            IN pmlan_ioctl_req pioctl_req)
{
    mlan_status ret = MLAN_STATUS_SUCCESS;
    mlan_private *pmpriv = pmadapter->priv[pioctl_req->bss_index];
    mlan_ds_pm_cfg *pm = MNULL;
    mlan_ds_auto_ds auto_ds;
    t_u32 mode;

    ENTER();

    if (pioctl_req->buf_len < sizeof(mlan_ds_pm_cfg)) {
        PRINTM(MWARN, "MLAN bss IOCTL length is too short.\n");
        pioctl_req->data_read_written = 0;
        pioctl_req->buf_len_needed = sizeof(mlan_ds_pm_cfg);
        pioctl_req->status_code = MLAN_ERROR_INVALID_PARAMETER;
        LEAVE();
        return MLAN_STATUS_RESOURCE;
    }
    pm = (mlan_ds_pm_cfg *) pioctl_req->pbuf;

    if (pioctl_req->action == MLAN_ACT_GET) {
        if (pmadapter->is_deep_sleep) {
            pm->param.auto_deep_sleep.auto_ds = DEEP_SLEEP_ON;
            pm->param.auto_deep_sleep.idletime = pmadapter->idle_time;
        } else
            pm->param.auto_deep_sleep.auto_ds = DEEP_SLEEP_OFF;
    } else {
        if (pmadapter->is_deep_sleep &&
            pm->param.auto_deep_sleep.auto_ds == DEEP_SLEEP_ON) {
            PRINTM(MMSG, "uAP already in deep sleep mode\n");
            LEAVE();
            return MLAN_STATUS_FAILURE;
        }
        if (((mlan_ds_pm_cfg *) pioctl_req->pbuf)->param.auto_deep_sleep.
            auto_ds == DEEP_SLEEP_ON) {
            auto_ds.auto_ds = DEEP_SLEEP_ON;
            mode = EN_AUTO_PS;
            PRINTM(MINFO, "Auto Deep Sleep: on\n");
        } else {
            mode = DIS_AUTO_PS;
            auto_ds.auto_ds = DEEP_SLEEP_OFF;
            PRINTM(MINFO, "Auto Deep Sleep: off\n");
        }
        if (((mlan_ds_pm_cfg *) pioctl_req->pbuf)->param.auto_deep_sleep.
            idletime)
            auto_ds.idletime =
                ((mlan_ds_pm_cfg *) pioctl_req->pbuf)->param.auto_deep_sleep.
                idletime;
        else
            auto_ds.idletime = pmadapter->idle_time;
        ret = wlan_prepare_cmd(pmpriv,
                               HostCmd_CMD_802_11_PS_MODE_ENH,
                               (t_u16) mode,
                               BITMAP_AUTO_DS, (t_void *) pioctl_req, &auto_ds);
        if (ret == MLAN_STATUS_SUCCESS)
            ret = MLAN_STATUS_PENDING;
    }
    LEAVE();
    return ret;
}

/**
 *  @brief Set SNMP MIB for 11D
 *
 *  @param pmadapter    A pointer to mlan_adapter structure
 *  @param pioctl_req   A pointer to ioctl request buffer
 *
 *  @return     MLAN_STATUS_PENDING --success, otherwise fail
 */
static mlan_status
wlan_uap_snmp_mib_11d(IN pmlan_adapter pmadapter, IN pmlan_ioctl_req pioctl_req)
{
    mlan_status ret = MLAN_STATUS_SUCCESS;
    mlan_private *pmpriv = pmadapter->priv[pioctl_req->bss_index];
    mlan_ds_snmp_mib *snmp = MNULL;
    state_11d_t flag;

    ENTER();

    if (pioctl_req->buf_len < sizeof(mlan_ds_snmp_mib)) {
        PRINTM(MWARN, "MLAN snmp_mib IOCTL length is too short.\n");
        pioctl_req->data_read_written = 0;
        pioctl_req->buf_len_needed = sizeof(mlan_ds_snmp_mib);
        LEAVE();
        return MLAN_STATUS_RESOURCE;
    }

    if ((pioctl_req->action == MLAN_ACT_SET) && pmpriv->uap_bss_started) {
        PRINTM(MIOCTL,
               "11D setting cannot be changed while UAP bss is started.\n");
        pioctl_req->data_read_written = 0;
        LEAVE();
        return MLAN_STATUS_FAILURE;
    }

    snmp = (mlan_ds_snmp_mib *) pioctl_req->pbuf;
    flag = (snmp->param.oid_value) ? ENABLE_11D : DISABLE_11D;

    ret = wlan_11d_enable(pmpriv, (t_void *) pioctl_req, flag);
    if (ret == MLAN_STATUS_SUCCESS)
        ret = MLAN_STATUS_PENDING;

    LEAVE();
    return ret;
}

/**
 *  @brief Callback to finish domain_info handling
 *  Not to be called directly to initiate domain_info setting.
 *
 *  @param pmpriv   A pointer to mlan_private structure (cast from t_void*)
 *
 *  @return     MLAN_STATUS_PENDING --success, otherwise fail
 *  @sa         wlan_uap_domain_info
 */
static mlan_status
wlan_uap_callback_domain_info(IN t_void * priv)
{
    mlan_status ret = MLAN_STATUS_SUCCESS;
    mlan_private *pmpriv = (mlan_private *) priv;
    wlan_uap_get_info_cb_t *puap_state_chan_cb = &pmpriv->uap_state_chan_cb;
    mlan_ds_11d_cfg *cfg11d;
    t_u8 band;

    ENTER();
    /* clear callback now that we're here */
    puap_state_chan_cb->get_chan_callback = MNULL;

    cfg11d = (mlan_ds_11d_cfg *) puap_state_chan_cb->pioctl_req_curr->pbuf;
    band =
        (puap_state_chan_cb->band_config & BAND_CONFIG_5GHZ) ? BAND_A : BAND_B;

    ret =
        wlan_11d_handle_uap_domain_info(pmpriv, band, cfg11d->param.domain_tlv,
                                        puap_state_chan_cb->pioctl_req_curr);
    if (ret == MLAN_STATUS_SUCCESS)
        ret = MLAN_STATUS_PENDING;

    puap_state_chan_cb->pioctl_req_curr = MNULL;        // prevent re-use
    LEAVE();
    return ret;
}

/**
 *  @brief Set Domain Info for 11D
 *
 *  @param pmadapter    A pointer to mlan_adapter structure
 *  @param pioctl_req   A pointer to ioctl request buffer
 *
 *  @return     MLAN_STATUS_PENDING --success, otherwise fail
 *  @sa         wlan_uap_callback_domain_info
 */
static mlan_status
wlan_uap_domain_info(IN pmlan_adapter pmadapter, IN pmlan_ioctl_req pioctl_req)
{
    mlan_status ret = MLAN_STATUS_SUCCESS;
    mlan_private *pmpriv = pmadapter->priv[pioctl_req->bss_index];

    ENTER();

    if (pioctl_req->buf_len < sizeof(mlan_ds_11d_cfg)) {
        PRINTM(MWARN, "MLAN 11d_cfg IOCTL length is too short.\n");
        pioctl_req->data_read_written = 0;
        pioctl_req->buf_len_needed = sizeof(mlan_ds_11d_cfg);
        LEAVE();
        return MLAN_STATUS_RESOURCE;
    }

    if ((pioctl_req->action == MLAN_ACT_SET) && pmpriv->uap_bss_started) {
        PRINTM(MIOCTL,
               "Domain_info cannot be changed while UAP bss is started.\n");
        pioctl_req->data_read_written = 0;
        LEAVE();
        return MLAN_STATUS_FAILURE;
    }

    /* store params, issue command to get UAP channel, whose CMD_RESP will
       callback remainder of domain_info handling */
    pmpriv->uap_state_chan_cb.pioctl_req_curr = pioctl_req;
    pmpriv->uap_state_chan_cb.get_chan_callback = wlan_uap_callback_domain_info;

    ret = wlan_uap_get_channel(pmpriv);
    if (ret == MLAN_STATUS_SUCCESS)
        ret = MLAN_STATUS_PENDING;

    LEAVE();
    return ret;
}

/**
 *  @brief Callback to finish 11H channel check handling.
 *  Not to be called directly to initiate channel check.
 *
 *  @param priv A pointer to mlan_private structure (cast from t_void*)
 *
 *  @return     MLAN_STATUS_SUCCESS/PENDING --success, otherwise fail
 *  @sa         wlan_uap_11h_channel_check_req
 */
static mlan_status
wlan_uap_callback_11h_channel_check_req(IN t_void * priv)
{
    mlan_status ret = MLAN_STATUS_FAILURE;
    mlan_private *pmpriv = (mlan_private *) priv;
    mlan_callbacks *pcb = (mlan_callbacks *) & pmpriv->adapter->callbacks;
    wlan_uap_get_info_cb_t *puap_state_chan_cb = &pmpriv->uap_state_chan_cb;
    /* keep copy as local variable */
    pmlan_ioctl_req pioctl = puap_state_chan_cb->pioctl_req_curr;

    ENTER();
    /* clear callback now that we're here */
    puap_state_chan_cb->get_chan_callback = MNULL;
    /* clear early to avoid race condition */
    puap_state_chan_cb->pioctl_req_curr = MNULL;

    /*
     * Check if the region and channel requires a channel availability
     * check.
     */
    if ((puap_state_chan_cb->band_config & BAND_CONFIG_5GHZ) &&
        wlan_11h_radar_detect_required(pmpriv, puap_state_chan_cb->channel) &&
        !wlan_11h_is_channel_under_nop(pmpriv->adapter,
                                       puap_state_chan_cb->channel)) {

        /*
         * Radar detection is required for this channel, make sure
         * 11h is activated in the firmware
         */
        ret = wlan_11h_activate(pmpriv, MNULL, MTRUE);
        ret = wlan_11h_config_master_radar_det(pmpriv, MTRUE);
        ret = wlan_11h_check_update_radar_det_state(pmpriv);

        /* Check for radar on the channel */
        ret = wlan_11h_issue_radar_detect(pmpriv,
                                          pioctl, puap_state_chan_cb->channel);
        if (ret == MLAN_STATUS_SUCCESS)
            ret = MLAN_STATUS_PENDING;
    } else {
        /* No command sent with the ioctl, need manually signal completion */
        pcb->moal_ioctl_complete(pmpriv->adapter->pmoal_handle,
                                 pioctl, MLAN_STATUS_FAILURE);
    }

    LEAVE();
    return ret;
}

/**
 *  @brief 802.11h uap start channel check
 *
 *  @param pmadapter    A pointer to mlan_adapter structure
 *  @param pioctl_req   A pointer to ioctl request buffer
 *
 *  @return     MLAN_STATUS_PENDING --success, otherwise fail
 *  @sa         wlan_uap_callback_11h_channel_check_req
 */
static mlan_status
wlan_uap_11h_channel_check_req(IN pmlan_adapter pmadapter,
                               IN pmlan_ioctl_req pioctl_req)
{
    mlan_status ret = MLAN_STATUS_SUCCESS;
    mlan_private *pmpriv = pmadapter->priv[pioctl_req->bss_index];

    ENTER();

    if (pioctl_req->buf_len < sizeof(mlan_ds_11h_cfg)) {
        PRINTM(MWARN, "MLAN 11h_cfg IOCTL length is too short.\n");
        pioctl_req->data_read_written = 0;
        pioctl_req->buf_len_needed = sizeof(mlan_ds_11h_cfg);
        LEAVE();
        return MLAN_STATUS_RESOURCE;
    }

    /* store params, issue command to get UAP channel, whose CMD_RESP will
       callback remainder of 11H channel check handling */
    pmpriv->uap_state_chan_cb.pioctl_req_curr = pioctl_req;
    pmpriv->uap_state_chan_cb.get_chan_callback =
        wlan_uap_callback_11h_channel_check_req;

    ret = wlan_uap_get_channel(pmpriv);
    if (ret == MLAN_STATUS_SUCCESS)
        ret = MLAN_STATUS_PENDING;

    LEAVE();
    return ret;
}

/**
 *  @brief Callback to finish 11H handling
 *  Not to be called directly to initiate 11H setting.
 *
 *  @param pmpriv   A pointer to mlan_private structure (cast from t_void*)
 *
 *  @return     MLAN_STATUS_PENDING --success, otherwise fail
 *  @sa         wlan_uap_snmp_mib_11h
 */
static mlan_status
wlan_uap_callback_snmp_mib_11h(IN t_void * priv)
{
    mlan_status ret = MLAN_STATUS_SUCCESS;
    mlan_private *pmpriv = (mlan_private *) priv;
    wlan_uap_get_info_cb_t *puap_state_chan_cb = &pmpriv->uap_state_chan_cb;
    mlan_ds_snmp_mib *snmp;
    t_bool enable_11h;

    ENTER();
    /* clear callback now that we're here */
    puap_state_chan_cb->get_chan_callback = MNULL;

    snmp = (mlan_ds_snmp_mib *) puap_state_chan_cb->pioctl_req_curr->pbuf;
    enable_11h = (snmp->param.oid_value) ? MTRUE : MFALSE;

    if (enable_11h) {
        if ((puap_state_chan_cb->band_config & BAND_CONFIG_5GHZ) &&
            wlan_11h_radar_detect_required(pmpriv,
                                           puap_state_chan_cb->channel)) {
            if (!wlan_11h_is_master_radar_det_active(pmpriv))
                wlan_11h_config_master_radar_det(pmpriv, MTRUE);
        }
    }

    ret =
        wlan_11h_activate(pmpriv,
                          (t_void *) puap_state_chan_cb->pioctl_req_curr,
                          enable_11h);
    wlan_11h_check_update_radar_det_state(pmpriv);
    if (ret == MLAN_STATUS_SUCCESS)
        ret = MLAN_STATUS_PENDING;

    puap_state_chan_cb->pioctl_req_curr = MNULL;        // prevent re-use
    LEAVE();
    return ret;
}

/**
 *  @brief Set SNMP MIB for 11H
 *
 *  @param pmadapter    A pointer to mlan_adapter structure
 *  @param pioctl_req   A pointer to ioctl request buffer
 *
 *  @return     MLAN_STATUS_PENDING --success, otherwise fail
 *  @sa         wlan_uap_callback_snmp_mib_11h
 */
static mlan_status
wlan_uap_snmp_mib_11h(IN pmlan_adapter pmadapter, IN pmlan_ioctl_req pioctl_req)
{
    mlan_status ret = MLAN_STATUS_SUCCESS;
    mlan_private *pmpriv = pmadapter->priv[pioctl_req->bss_index];
    mlan_ds_snmp_mib *snmp = MNULL;
    t_bool enable;

    ENTER();

    if (pioctl_req->buf_len < sizeof(mlan_ds_snmp_mib)) {
        PRINTM(MWARN, "MLAN snmp_mib IOCTL length is too short.\n");
        pioctl_req->data_read_written = 0;
        pioctl_req->buf_len_needed = sizeof(mlan_ds_snmp_mib);
        LEAVE();
        return MLAN_STATUS_RESOURCE;
    }
    snmp = (mlan_ds_snmp_mib *) pioctl_req->pbuf;
    enable = (snmp->param.oid_value) ? MTRUE : MFALSE;

    if (enable) {
        /* first enable 11D if it is not enabled */
        if (!wlan_11d_is_enabled(pmpriv)) {
            ret = wlan_11d_enable(pmpriv, MNULL, ENABLE_11D);
            if (ret != MLAN_STATUS_SUCCESS) {
                PRINTM(MERROR,
                       "Failed to first enable 11D before enabling 11H.\n");
                LEAVE();
                return ret;
            }
        }
    }

    /* store params, issue command to get UAP channel, whose CMD_RESP will
       callback remainder of 11H handling (and radar detect if DFS chan) */
    pmpriv->uap_state_chan_cb.pioctl_req_curr = pioctl_req;
    pmpriv->uap_state_chan_cb.get_chan_callback =
        wlan_uap_callback_snmp_mib_11h;

    ret = wlan_uap_get_channel(pmpriv);
    if (ret == MLAN_STATUS_SUCCESS)
        ret = MLAN_STATUS_PENDING;

    LEAVE();
    return ret;
}

/********************************************************
    Global Functions
********************************************************/

/**
 *  @brief Issue CMD to UAP firmware to get current channel
 *
 *  @param pmpriv   A pointer to mlan_private structure
 *
 *  @return         MLAN_STATUS_SUCCESS --success, otherwise fail
 */
mlan_status
wlan_uap_get_channel(IN pmlan_private pmpriv)
{
    MrvlIEtypes_channel_band_t tlv_chan_band;
    mlan_status ret = MLAN_STATUS_SUCCESS;

    ENTER();
    memset(pmpriv->adapter, &tlv_chan_band, 0, sizeof(tlv_chan_band));
    tlv_chan_band.header.type = TLV_TYPE_UAP_CHAN_BAND_CONFIG;
    tlv_chan_band.header.len = sizeof(MrvlIEtypes_channel_band_t)
        - sizeof(MrvlIEtypesHeader_t);

    ret = wlan_prepare_cmd(pmpriv, HOST_CMD_APCMD_SYS_CONFIGURE,
                           HostCmd_ACT_GEN_GET, 0, MNULL, &tlv_chan_band);
    LEAVE();
    return ret;
}

/**
 *  @brief Issue CMD to UAP firmware to set current channel
 *
 *  @param pmpriv       A pointer to mlan_private structure
 *  @param uap_band_cfg UAP band configuration
 *  @param channel      New channel
 *
 *  @return         MLAN_STATUS_SUCCESS --success, otherwise fail
 */
mlan_status
wlan_uap_set_channel(IN pmlan_private pmpriv,
                     IN t_u8 uap_band_cfg, IN t_u8 channel)
{
    MrvlIEtypes_channel_band_t tlv_chan_band;
    mlan_status ret = MLAN_STATUS_SUCCESS;

    ENTER();
    memset(pmpriv->adapter, &tlv_chan_band, 0, sizeof(tlv_chan_band));
    tlv_chan_band.header.type = TLV_TYPE_UAP_CHAN_BAND_CONFIG;
    tlv_chan_band.header.len = sizeof(MrvlIEtypes_channel_band_t)
        - sizeof(MrvlIEtypesHeader_t);
    tlv_chan_band.band_config = uap_band_cfg;
    tlv_chan_band.channel = channel;

    ret = wlan_prepare_cmd(pmpriv, HOST_CMD_APCMD_SYS_CONFIGURE,
                           HostCmd_ACT_GEN_SET, 0, MNULL, &tlv_chan_band);
    LEAVE();
    return ret;
}

/**
 *  @brief Issue CMD to UAP firmware to get current beacon and dtim periods
 *
 *  @param pmpriv   A pointer to mlan_private structure
 *
 *  @return         MLAN_STATUS_SUCCESS --success, otherwise fail
 */
mlan_status
wlan_uap_get_beacon_dtim(IN pmlan_private pmpriv)
{
    t_u8 tlv_buffer[sizeof(MrvlIEtypes_beacon_period_t) +
                    sizeof(MrvlIEtypes_dtim_period_t)];
    MrvlIEtypes_beacon_period_t *ptlv_beacon_pd;
    MrvlIEtypes_dtim_period_t *ptlv_dtim_pd;
    mlan_status ret = MLAN_STATUS_SUCCESS;

    ENTER();

    memset(pmpriv->adapter, &tlv_buffer, 0, sizeof(tlv_buffer));
    ptlv_beacon_pd = (MrvlIEtypes_beacon_period_t *) tlv_buffer;
    ptlv_beacon_pd->header.type = TLV_TYPE_UAP_BEACON_PERIOD;
    ptlv_beacon_pd->header.len = sizeof(MrvlIEtypes_beacon_period_t)
        - sizeof(MrvlIEtypesHeader_t);

    ptlv_dtim_pd = (MrvlIEtypes_dtim_period_t *) (tlv_buffer
                                                  +
                                                  sizeof
                                                  (MrvlIEtypes_beacon_period_t));
    ptlv_dtim_pd->header.type = TLV_TYPE_UAP_DTIM_PERIOD;
    ptlv_dtim_pd->header.len = sizeof(MrvlIEtypes_dtim_period_t)
        - sizeof(MrvlIEtypesHeader_t);

    ret = wlan_prepare_cmd(pmpriv, HOST_CMD_APCMD_SYS_CONFIGURE,
                           HostCmd_ACT_GEN_GET, 0, MNULL, tlv_buffer);
    LEAVE();
    return ret;
}
#endif /* CONFIG_MLAN_WMSDK */

/**
 *  @brief Set/Get deauth control.
 *
 *  @param pmadapter	A pointer to mlan_adapter structure
 *  @param pioctl_req	A pointer to ioctl request buffer
 *
 *  @return		MLAN_STATUS_PENDING --success, otherwise fail
 */
mlan_status
wlan_uap_snmp_mib_ctrl_deauth(
    IN pmlan_adapter pmadapter,
    IN pmlan_ioctl_req pioctl_req)
{
    mlan_private *pmpriv = pmadapter->priv[pioctl_req->bss_index];
    mlan_status ret = MLAN_STATUS_SUCCESS;
    mlan_ds_snmp_mib *mib = (mlan_ds_snmp_mib *) pioctl_req->pbuf;
    t_u16 cmd_action = 0;

    ENTER();

    mib = (mlan_ds_snmp_mib *)pioctl_req->pbuf;
    if (pioctl_req->action == MLAN_ACT_SET) {
        cmd_action = HostCmd_ACT_GEN_SET;
    } else {
        cmd_action = HostCmd_ACT_GEN_GET;
    }

    /* Send command to firmware */
    ret = wlan_prepare_cmd(pmpriv,
                           HostCmd_CMD_802_11_SNMP_MIB,
                           cmd_action,
                           StopDeauth_i,
                           (t_void *) pioctl_req,
                           &mib->param.deauthctrl);

    if (ret == MLAN_STATUS_SUCCESS)
       ret = MLAN_STATUS_PENDING;

    LEAVE();
    return ret;
}


/**
 *  @brief MLAN uap ioctl handler
 *
 *  @param adapter	A pointer to mlan_adapter structure
 *  @param pioctl_req	A pointer to ioctl request buffer
 *
 *  @return		MLAN_STATUS_SUCCESS --success, otherwise fail
 */
mlan_status
wlan_ops_uap_ioctl(t_void * adapter, pmlan_ioctl_req pioctl_req)
{
    pmlan_adapter pmadapter = (pmlan_adapter) adapter;
    mlan_status status = MLAN_STATUS_SUCCESS;
#ifndef CONFIG_MLAN_WMSDK
    mlan_ds_bss *bss = MNULL;
    mlan_ds_get_info *pget_info = MNULL;
    mlan_ds_misc_cfg *misc = MNULL;
#endif /* CONFIG_MLAN_WMSDK */
    mlan_ds_sec_cfg *sec = MNULL;
#ifndef CONFIG_MLAN_WMSDK
    mlan_ds_pm_cfg *pm = MNULL;
    mlan_ds_snmp_mib *snmp = MNULL;
    mlan_ds_11d_cfg *cfg11d = MNULL;
    mlan_ds_11h_cfg *cfg11h = MNULL;
    mlan_ds_radio_cfg *radiocfg = MNULL;
    mlan_ds_rate *rate = MNULL;
    mlan_private *pmpriv = pmadapter->priv[pioctl_req->bss_index];
#endif /* CONFIG_MLAN_WMSDK */

    ENTER();

    switch (pioctl_req->req_id) {
#ifndef CONFIG_MLAN_WMSDK
    case MLAN_IOCTL_BSS:
        bss = (mlan_ds_bss *) pioctl_req->pbuf;
        if (bss->sub_command == MLAN_OID_BSS_MAC_ADDR)
            status = wlan_uap_bss_ioctl_mac_address(pmadapter, pioctl_req);
        else if (bss->sub_command == MLAN_OID_BSS_STOP)
            status = wlan_uap_bss_ioctl_stop(pmadapter, pioctl_req);
        else if (bss->sub_command == MLAN_OID_BSS_START)
            status = wlan_uap_bss_ioctl_start(pmadapter, pioctl_req);
        else if (bss->sub_command == MLAN_OID_UAP_BSS_CONFIG)
            status = wlan_uap_bss_ioctl_config(pmadapter, pioctl_req);
        else if (bss->sub_command == MLAN_OID_UAP_DEAUTH_STA)
            status = wlan_uap_bss_ioctl_deauth_sta(pmadapter, pioctl_req);
        else if (bss->sub_command == MLAN_OID_UAP_BSS_RESET)
            status = wlan_uap_bss_ioctl_reset(pmadapter, pioctl_req);
#if defined(STA_SUPPORT) && defined(UAP_SUPPORT)
        else if (bss->sub_command == MLAN_OID_BSS_ROLE)
            status = wlan_bss_ioctl_bss_role(pmadapter, pioctl_req);
#endif
#ifdef WIFI_DIRECT_SUPPORT
        else if (bss->sub_command == MLAN_OID_WIFI_DIRECT_MODE)
            status = wlan_bss_ioctl_wifi_direct_mode(pmadapter, pioctl_req);
#endif
        break;
    case MLAN_IOCTL_GET_INFO:
        pget_info = (mlan_ds_get_info *) pioctl_req->pbuf;
        if (pget_info->sub_command == MLAN_OID_GET_VER_EXT)
            status = wlan_get_info_ver_ext(pmadapter, pioctl_req);
        else if (pget_info->sub_command == MLAN_OID_GET_DEBUG_INFO)
            status = wlan_get_info_debug_info(pmadapter, pioctl_req);
        else if (pget_info->sub_command == MLAN_OID_GET_STATS)
            status = wlan_uap_get_stats(pmadapter, pioctl_req);
        else if (pget_info->sub_command == MLAN_OID_UAP_STA_LIST)
            status = wlan_uap_get_sta_list(pmadapter, pioctl_req);
        else if (pget_info->sub_command == MLAN_OID_GET_BSS_INFO)
            status = wlan_uap_get_bss_info(pmadapter, pioctl_req);
        else if (pget_info->sub_command == MLAN_OID_GET_FW_INFO) {
            pioctl_req->data_read_written =
                sizeof(mlan_fw_info) + MLAN_SUB_COMMAND_SIZE;
            memcpy(pmadapter, &pget_info->param.fw_info.mac_addr,
                   pmpriv->curr_addr, MLAN_MAC_ADDR_LENGTH);
            pget_info->param.fw_info.fw_ver = pmadapter->fw_release_number;
            pget_info->param.fw_info.fw_bands = pmadapter->fw_bands;
            pget_info->param.fw_info.hw_dev_mcs_support =
                pmadapter->hw_dev_mcs_support;
        }
        break;
    case MLAN_IOCTL_MISC_CFG:
        misc = (mlan_ds_misc_cfg *) pioctl_req->pbuf;
        if (misc->sub_command == MLAN_OID_MISC_INIT_SHUTDOWN)
            status = wlan_misc_ioctl_init_shutdown(pmadapter, pioctl_req);
        if (misc->sub_command == MLAN_OID_MISC_SOFT_RESET)
            status = wlan_uap_misc_ioctl_soft_reset(pmadapter, pioctl_req);
        if (misc->sub_command == MLAN_OID_MISC_HOST_CMD)
            status = wlan_misc_ioctl_host_cmd(pmadapter, pioctl_req);
        if (misc->sub_command == MLAN_OID_MISC_GEN_IE)
            status = wlan_uap_misc_ioctl_gen_ie(pmadapter, pioctl_req);
        if (misc->sub_command == MLAN_OID_MISC_CUSTOM_IE)
            status =
                wlan_misc_ioctl_custom_ie_list(pmadapter, pioctl_req, MTRUE);
        if (misc->sub_command == MLAN_OID_MISC_TX_DATAPAUSE)
            status = wlan_uap_misc_ioctl_txdatapause(pmadapter, pioctl_req);
        if (misc->sub_command == MLAN_OID_MISC_RX_MGMT_IND)
            status = wlan_reg_rx_mgmt_ind(pmadapter, pioctl_req);
#ifdef DEBUG_LEVEL1
        if (misc->sub_command == MLAN_OID_MISC_DRVDBG)
            status = wlan_set_drvdbg(pmadapter, pioctl_req);
#endif
        break;
    case MLAN_IOCTL_PM_CFG:
        pm = (mlan_ds_pm_cfg *) pioctl_req->pbuf;
        if (pm->sub_command == MLAN_OID_PM_CFG_PS_MODE)
            status = wlan_uap_pm_ioctl_mode(pmadapter, pioctl_req);
        if (pm->sub_command == MLAN_OID_PM_CFG_DEEP_SLEEP)
            status = wlan_uap_pm_ioctl_deepsleep(pmadapter, pioctl_req);
        if (pm->sub_command == MLAN_OID_PM_CFG_HS_CFG) {
            status = wlan_pm_ioctl_hscfg(pmadapter, pioctl_req);
        }
        if (pm->sub_command == MLAN_OID_PM_INFO) {
            status = wlan_get_pm_info(pmadapter, pioctl_req);
        }
        break;
    case MLAN_IOCTL_SNMP_MIB:
        snmp = (mlan_ds_snmp_mib *) pioctl_req->pbuf;
	if (snmp->sub_command == MLAN_OID_SNMP_MIB_CTRL_DEAUTH)
	    status = wlan_uap_snmp_mib_ctrl_deauth(pmadapter, pioctl_req);
        if (snmp->sub_command == MLAN_OID_SNMP_MIB_DOT11D)
            status = wlan_uap_snmp_mib_11d(pmadapter, pioctl_req);
        if (snmp->sub_command == MLAN_OID_SNMP_MIB_DOT11H)
            status = wlan_uap_snmp_mib_11h(pmadapter, pioctl_req);
        break;
#endif /* CONFIG_MLAN_WMSDK */
#if defined(WAPI_AP) || defined(HOST_AUTHENTICATOR)
    case MLAN_IOCTL_SEC_CFG:
        sec = (mlan_ds_sec_cfg *)pioctl_req->pbuf;
        if(sec->sub_command == MLAN_OID_SEC_CFG_ENCRYPT_KEY)
            status = wlan_uap_sec_ioctl_set_encrypt_key(pmadapter, pioctl_req);
#ifdef WAPI_AP
        if(sec->sub_command == MLAN_OID_SEC_CFG_WAPI_ENABLED)
            status = wlan_uap_sec_ioctl_wapi_enable(pmadapter, pioctl_req);
#endif
#ifndef CONFIG_MLAN_WMSDK
#ifdef HOST_AUTHENTICATOR
        if(sec->sub_command == MLAN_OID_SEC_CFG_REPORT_MIC_ERR)
            status = wlan_uap_sec_ioctl_report_mic_error(pmadapter, pioctl_req);
#endif
#endif /* CONFIG_MLAN_WMSDK */
        break;
#endif /* WAPI_AP || HOST_AUTHENTICATOR */
#ifndef CONFIG_MLAN_WMSDK
    case MLAN_IOCTL_11N_CFG:
        status = wlan_11n_cfg_ioctl(pmadapter, pioctl_req);
        break;
    case MLAN_IOCTL_11D_CFG:
        cfg11d = (mlan_ds_11d_cfg *) pioctl_req->pbuf;
        if (cfg11d->sub_command == MLAN_OID_11D_DOMAIN_INFO)
            status = wlan_uap_domain_info(pmadapter, pioctl_req);
        break;
    case MLAN_IOCTL_11H_CFG:
        cfg11h = (mlan_ds_11h_cfg *) pioctl_req->pbuf;
        if (cfg11h->sub_command == MLAN_OID_11H_CHANNEL_CHECK)
            status = wlan_uap_11h_channel_check_req(pmadapter, pioctl_req);
#if defined(DFS_TESTING_SUPPORT)
        if (cfg11h->sub_command == MLAN_OID_11H_DFS_TESTING)
            status = wlan_11h_ioctl_dfs_testing(pmadapter, pioctl_req);
#endif
        break;
    case MLAN_IOCTL_RADIO_CFG:
        radiocfg = (mlan_ds_radio_cfg *) pioctl_req->pbuf;
        if (radiocfg->sub_command == MLAN_OID_RADIO_CTRL)
            status = wlan_radio_ioctl_radio_ctl(pmadapter, pioctl_req);
#ifdef WIFI_DIRECT_SUPPORT
        if (radiocfg->sub_command == MLAN_OID_REMAIN_CHAN_CFG)
            status = wlan_radio_ioctl_remain_chan_cfg(pmadapter, pioctl_req);
#endif
        break;
    case MLAN_IOCTL_RATE:
        rate = (mlan_ds_rate *) pioctl_req->pbuf;
        if (rate->sub_command == MLAN_OID_RATE_CFG)
            status = wlan_rate_ioctl_cfg(pmadapter, pioctl_req);
        break;
#endif /* CONFIG_MLAN_WMSDK */
    default:
        pioctl_req->status_code = MLAN_ERROR_IOCTL_INVALID;
        break;
    }
    LEAVE();
    return status;
}
