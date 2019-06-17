/** @file mlan_join.c
 *
 *  @brief  This file provides Functions implementing wlan infrastructure
 *  and adhoc join routines IOCTL handlers as well as command preparation
 *  and response routines for sending adhoc start, adhoc join, and
 *  association commands to the firmware.
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
    10/30/2008: initial version
******************************************************/
#include <mlan_wmsdk.h>

/* Additional WMSDK header files */
#include <wmstdio.h>
#include <wmassert.h>
#include <wmerrno.h>
#include <wm_os.h>

/* Always keep this include at the end of all include files */
#include <mlan_remap_mem_operations.h>
/********************************************************
                Local Constants
********************************************************/

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
 *  @brief Append a generic IE as a pass through TLV to a TLV buffer.
 *
 *  This function is called from the network join command prep. routine.
 *    If the IE buffer has been setup by the application, this routine appends
 *    the buffer as a pass through TLV type to the request.
 *
 *  @param priv     A pointer to mlan_private structure
 *  @param ppbuffer pointer to command buffer pointer
 *
 *  @return         bytes added to the buffer
 */
static int
wlan_cmd_append_generic_ie(mlan_private * priv, t_u8 ** ppbuffer)
{
    int ret_len = 0;
    MrvlIEtypesHeader_t ie_header;

    ENTER();

    /* Null Checks */
    if (ppbuffer == MNULL) {
        LEAVE();
        return 0;
    }
    if (*ppbuffer == MNULL) {
        LEAVE();
        return 0;
    }

    /*
     * If there is a generic ie buffer setup, append it to the return
     *   parameter buffer pointer.
     */
    if (priv->gen_ie_buf_len) {
        PRINTM(MINFO, "append generic IE %d to %p\n", priv->gen_ie_buf_len,
               *ppbuffer);

        /* Wrap the generic IE buffer with a pass through TLV type */
        ie_header.type = wlan_cpu_to_le16(TLV_TYPE_PASSTHROUGH);
        ie_header.len = wlan_cpu_to_le16(priv->gen_ie_buf_len);
        memcpy(priv->adapter, *ppbuffer, &ie_header, sizeof(ie_header));

        /* Increment the return size and the return buffer pointer param */
        *ppbuffer += sizeof(ie_header);
        ret_len += sizeof(ie_header);

        /* Copy the generic IE buffer to the output buffer, advance pointer */
        memcpy(priv->adapter, *ppbuffer, priv->gen_ie_buf,
               priv->gen_ie_buf_len);

        /* Increment the return size and the return buffer pointer param */
        *ppbuffer += priv->gen_ie_buf_len;
        ret_len += priv->gen_ie_buf_len;

        /* Reset the generic IE buffer */
        priv->gen_ie_buf_len = 0;
    }

    /* return the length appended to the buffer */
    LEAVE();
    return ret_len;
}

/**
  *  @brief Append TSF tracking info from the scan table for the target AP
  *
  *  This function is called from the network join command prep. routine.
  *     The TSF table TSF sent to the firmware contains two TSF values:
  *        - the TSF of the target AP from its previous beacon/probe response
  *        - the TSF timestamp of our local MAC at the time we observed the
  *          beacon/probe response.
  *
  *     The firmware uses the timestamp values to set an initial TSF value
  *        in the MAC for the new association after a reassociation attempt.
  *
  *    @param pmpriv     A pointer to mlan_private structure
  *    @param ppbuffer   A pointer to command buffer pointer
  *    @param pbss_desc  A pointer to the BSS Descriptor from the scan table of
  *                      the AP we are trying to join
  *
  *    @return         bytes added to the buffer
  */
static int
wlan_cmd_append_tsf_tlv(mlan_private * pmriv, t_u8 ** ppbuffer,
                        BSSDescriptor_t * pbss_desc)
{
    MrvlIEtypes_TsfTimestamp_t tsf_tlv;
    t_u64 tsf_val;

    ENTER();

    /* Null Checks */
    if (ppbuffer == MNULL) {
        LEAVE();
        return 0;
    }
    if (*ppbuffer == MNULL) {
        LEAVE();
        return 0;
    }

    memset(pmriv->adapter, &tsf_tlv, 0x00, sizeof(MrvlIEtypes_TsfTimestamp_t));

    tsf_tlv.header.type = wlan_cpu_to_le16(TLV_TYPE_TSFTIMESTAMP);
    tsf_tlv.header.len = wlan_cpu_to_le16(2 * sizeof(tsf_val));

    memcpy(pmriv->adapter, *ppbuffer, &tsf_tlv, sizeof(tsf_tlv.header));
    *ppbuffer += sizeof(tsf_tlv.header);

    /* TSF timestamp from the firmware TSF when the bcn/prb rsp was received */
    tsf_val = wlan_cpu_to_le64(pbss_desc->network_tsf);
    memcpy(pmriv->adapter, *ppbuffer, &tsf_val, sizeof(tsf_val));
    *ppbuffer += sizeof(tsf_val);

    memcpy(pmriv->adapter, &tsf_val, pbss_desc->time_stamp, sizeof(tsf_val));

    PRINTM(MINFO, "ASSOC: TSF offset calc: %016llx - %016llx\n",
           tsf_val, pbss_desc->network_tsf);

    memcpy(pmriv->adapter, *ppbuffer, &tsf_val, sizeof(tsf_val));
    *ppbuffer += sizeof(tsf_val);

    LEAVE();
    return (sizeof(tsf_tlv.header) + (2 * sizeof(tsf_val)));
}
#endif /* CONFIG_MLAN_WMSDK */

/**
 *  @brief This function finds out the common rates between rate1 and rate2.
 *
 *  It will fill common rates in rate1 as output if found.
 *
 *  NOTE: Setting the MSB of the basic rates needs to be taken
 *   care of, either before or after calling this function
 *
 *  @param pmpriv      A pointer to mlan_private structure
 *  @param rate1       the buffer which keeps input and output
 *  @param rate1_size  the size of rate1 buffer
 *  @param rate2       the buffer which keeps rate2
 *  @param rate2_size  the size of rate2 buffer.
 *
 *  @return            MLAN_STATUS_SUCCESS or MLAN_STATUS_FAILURE
 */
static mlan_status
wlan_get_common_rates(IN mlan_private * pmpriv,
                      IN t_u8 * rate1,
                      IN t_u32 rate1_size, IN t_u8 * rate2, IN t_u32 rate2_size)
{
    mlan_status ret = MLAN_STATUS_SUCCESS;
    mlan_callbacks *pcb = (mlan_callbacks *) & pmpriv->adapter->callbacks;
    t_u8 *ptr = rate1;
    t_u8 *tmp = MNULL;
    t_u32 i, j;

    ENTER();

    ret =
        pcb->moal_malloc(pmpriv->adapter->pmoal_handle, rate1_size,
                         MLAN_MEM_DEF, &tmp);
    if (ret != MLAN_STATUS_SUCCESS || !tmp) {
        PRINTM(MERROR, "Failed to allocate buffer\n");
        ret = MLAN_STATUS_FAILURE;
        goto done;
    }

    memcpy(pmpriv->adapter, tmp, rate1, rate1_size);
    memset(pmpriv->adapter, rate1, 0, rate1_size);

    for (i = 0; rate2[i] && i < rate2_size; i++) {
        for (j = 0; tmp[j] && j < rate1_size; j++) {
            /* Check common rate, excluding the bit for basic rate */
            if ((rate2[i] & 0x7F) == (tmp[j] & 0x7F)) {
                *rate1++ = tmp[j];
                break;
            }
        }
    }

    HEXDUMP("rate1 (AP) Rates", tmp, rate1_size);
    HEXDUMP("rate2 (Card) Rates", rate2, rate2_size);
    HEXDUMP("Common Rates", ptr, rate1 - ptr);
    PRINTM(MINFO, "Tx DataRate is set to 0x%X\n", pmpriv->data_rate);

    if (!pmpriv->is_data_rate_auto) {
        while (*ptr) {
            if ((*ptr & 0x7f) == pmpriv->data_rate) {
                ret = MLAN_STATUS_SUCCESS;
                goto done;
            }
            ptr++;
        }
        PRINTM(MMSG, "Previously set fixed data rate %#x is not "
               "compatible with the network\n", pmpriv->data_rate);

        ret = MLAN_STATUS_FAILURE;
        goto done;
    }

    ret = MLAN_STATUS_SUCCESS;
  done:
    if (tmp)
        pcb->moal_mfree(pmpriv->adapter->pmoal_handle, tmp);

    LEAVE();
    return ret;
}

/**
 *  @brief Create the intersection of the rates supported by a target BSS and
 *         our pmadapter settings for use in an assoc/join command.
 *
 *  @param pmpriv           A pointer to mlan_private structure
 *  @param pbss_desc        BSS Descriptor whose rates are used in the setup
 *  @param pout_rates       Output: Octet array of rates common between the BSS
 *                          and the pmadapter supported rates settings
 *  @param pout_rates_size  Output: Number of rates/octets set in pout_rates
 *
 *  @return                 MLAN_STATUS_SUCCESS or MLAN_STATUS_FAILURE
 */
static mlan_status
wlan_setup_rates_from_bssdesc(IN mlan_private * pmpriv,
                              IN BSSDescriptor_t * pbss_desc,
                              OUT t_u8 * pout_rates,
                              OUT t_u32 * pout_rates_size)
{
    t_u8 card_rates[WLAN_SUPPORTED_RATES];
    t_u32 card_rates_size = 0;
    ENTER();
    /* Copy AP supported rates */
    memcpy(pmpriv->adapter, pout_rates, pbss_desc->supported_rates,
           WLAN_SUPPORTED_RATES);
    /* Get the STA supported rates */
    card_rates_size = wlan_get_active_data_rates(pmpriv, pmpriv->bss_mode,
                                                 pmpriv->config_bands,
                                                 card_rates);
    /* Get the common rates between AP and STA supported rates */
    if (wlan_get_common_rates(pmpriv, pout_rates, WLAN_SUPPORTED_RATES,
                              card_rates, card_rates_size)) {
        *pout_rates_size = 0;
        PRINTM(MERROR, "wlan_get_common_rates failed\n");
        LEAVE();
        return MLAN_STATUS_FAILURE;
    }

    *pout_rates_size =
        MIN(wlan_strlen((char *) pout_rates), WLAN_SUPPORTED_RATES);

    LEAVE();
    return MLAN_STATUS_SUCCESS;
}

#ifndef CONFIG_MLAN_WMSDK
/**
 *  @brief Update the scan entry TSF timestamps to reflect a new association
 *
 *  @param pmpriv        A pointer to mlan_private structure
 *  @param pnew_bss_desc A pointer to the newly associated AP's scan table entry
 *
 *  @return              N/A
 */
static t_void
wlan_update_tsf_timestamps(IN mlan_private * pmpriv,
                           IN BSSDescriptor_t * pnew_bss_desc)
{
    mlan_adapter *pmadapter = pmpriv->adapter;
    t_u32 table_idx;
    t_u64 new_tsf_base;
    t_s64 tsf_delta;

    ENTER();

    memcpy(pmpriv->adapter, &new_tsf_base, pnew_bss_desc->time_stamp,
           sizeof(new_tsf_base));

    tsf_delta = new_tsf_base - pnew_bss_desc->network_tsf;

    PRINTM(MINFO, "TSF: Update TSF timestamps, 0x%016llx -> 0x%016llx\n",
           pnew_bss_desc->network_tsf, new_tsf_base);

    for (table_idx = 0; table_idx < pmadapter->num_in_scan_table; table_idx++) {
        pmadapter->pscan_table[table_idx].network_tsf += tsf_delta;
    }

    LEAVE();
}

/**
 *  @brief Append a wapi IE
 *
 *  This function is called from the network join command prep. routine.
 *    If the IE buffer has been setup by the application, this routine appends
 *    the buffer as a wapi TLV type to the request.
 *
 *  @param priv     A pointer to mlan_private structure
 *  @param ppBuffer pointer to command buffer pointer
 *
 *  @return         bytes added to the buffer
 */
static int
wlan_cmd_append_wapi_ie(mlan_private * priv, t_u8 ** ppBuffer)
{
    int retLen = 0;
    MrvlIEtypesHeader_t ie_header;

    ENTER();

    /* Null Checks */
    if (ppBuffer == MNULL) {
        LEAVE();
        return 0;
    }
    if (*ppBuffer == MNULL) {
        LEAVE();
        return 0;
    }

    /*
     * If there is a wapi ie buffer setup, append it to the return
     *   parameter buffer pointer.
     */
    if (priv->wapi_ie_len) {
        PRINTM(MCMND, "append wapi ie %d to %p\n", priv->wapi_ie_len,
               *ppBuffer);

        /* Wrap the generic IE buffer with a pass through TLV type */
        ie_header.type = wlan_cpu_to_le16(TLV_TYPE_WAPI_IE);
        ie_header.len = wlan_cpu_to_le16(priv->wapi_ie_len);
        memcpy(priv->adapter, *ppBuffer, &ie_header, sizeof(ie_header));

        /* Increment the return size and the return buffer pointer param */
        *ppBuffer += sizeof(ie_header);
        retLen += sizeof(ie_header);

        /* Copy the wapi IE buffer to the output buffer, advance pointer */
        memcpy(priv->adapter, *ppBuffer, priv->wapi_ie, priv->wapi_ie_len);

        /* Increment the return size and the return buffer pointer param */
        *ppBuffer += priv->wapi_ie_len;
        retLen += priv->wapi_ie_len;

    }
    /* return the length appended to the buffer */
    LEAVE();
    return retLen;
}
#endif /* CONFIG_MLAN_WMSDK */


/********************************************************
                Global Functions
********************************************************/
/**
 *  @brief This function prepares command of association.
 *
 *  @param pmpriv       A pointer to mlan_private structure
 *  @param cmd          A pointer to HostCmd_DS_COMMAND structure
 *  @param pdata_buf    A pointer cast of BSSDescriptor_t from the
 *                        scan table to assoc
 *
 *  @return             MLAN_STATUS_SUCCESS or MLAN_STATUS_FAILURE
 */
mlan_status
wlan_cmd_802_11_associate(IN mlan_private * pmpriv,
                          IN HostCmd_DS_COMMAND * cmd, IN t_void * pdata_buf)
{
    mlan_status ret = MLAN_STATUS_SUCCESS;
    mlan_adapter *pmadapter = pmpriv->adapter;
    HostCmd_DS_802_11_ASSOCIATE *passo = &cmd->params.associate;
    BSSDescriptor_t *pbss_desc;
    MrvlIEtypes_SsIdParamSet_t *pssid_tlv;
    MrvlIEtypes_PhyParamSet_t *pphy_tlv;
    MrvlIEtypes_SsParamSet_t *pss_tlv;
    MrvlIEtypes_RatesParamSet_t *prates_tlv;
    MrvlIEtypes_AuthType_t *pauth_tlv;
    MrvlIEtypes_RsnParamSet_t *prsn_ie_tlv;
    MrvlIEtypes_ChanListParamSet_t *pchan_tlv;
    WLAN_802_11_RATES rates;
    t_u32 rates_size;
    t_u16 tmp_cap;
    t_u8 *pos;

    ENTER();

    pbss_desc = (BSSDescriptor_t *) pdata_buf;
    pos = (t_u8 *) passo;

    cmd->command = wlan_cpu_to_le16(HostCmd_CMD_802_11_ASSOCIATE);

    /* Save so we know which BSS Desc to use in the response handler */
    pmpriv->pattempted_bss_desc = pbss_desc;

    memcpy(pmadapter, passo->peer_sta_addr,
           pbss_desc->mac_address, sizeof(passo->peer_sta_addr));
    pos += sizeof(passo->peer_sta_addr);

    /* fixme: Look at this value carefully later. The listen interval is given to AP during
     * this assoc. The listen_interval set later during IEEE PS should not (?) exceed this
     */
    /* Set the listen interval */
    passo->listen_interval = wlan_cpu_to_le16(pmpriv->listen_interval);
    /* Set the beacon period */
    passo->beacon_period = wlan_cpu_to_le16(pbss_desc->beacon_period);

    pos += sizeof(passo->cap_info);
    pos += sizeof(passo->listen_interval);
    pos += sizeof(passo->beacon_period);
    pos += sizeof(passo->dtim_period);

    pssid_tlv = (MrvlIEtypes_SsIdParamSet_t *) pos;
    pssid_tlv->header.type = wlan_cpu_to_le16(TLV_TYPE_SSID);
    pssid_tlv->header.len = (t_u16) pbss_desc->ssid.ssid_len;
    memcpy(pmadapter, pssid_tlv->ssid, pbss_desc->ssid.ssid,
           pssid_tlv->header.len);
    pos += sizeof(pssid_tlv->header) + pssid_tlv->header.len;
    pssid_tlv->header.len = wlan_cpu_to_le16(pssid_tlv->header.len);

    pphy_tlv = (MrvlIEtypes_PhyParamSet_t *) pos;
    pphy_tlv->header.type = wlan_cpu_to_le16(TLV_TYPE_PHY_DS);
    pphy_tlv->header.len = sizeof(pphy_tlv->fh_ds.ds_param_set);
    memcpy(pmadapter, &pphy_tlv->fh_ds.ds_param_set,
           &pbss_desc->phy_param_set.ds_param_set.current_chan,
           sizeof(pphy_tlv->fh_ds.ds_param_set));
    pos += sizeof(pphy_tlv->header) + pphy_tlv->header.len;
    pphy_tlv->header.len = wlan_cpu_to_le16(pphy_tlv->header.len);

    pss_tlv = (MrvlIEtypes_SsParamSet_t *) pos;
    pss_tlv->header.type = wlan_cpu_to_le16(TLV_TYPE_CF);
    pss_tlv->header.len = sizeof(pss_tlv->cf_ibss.cf_param_set);
    pos += sizeof(pss_tlv->header) + pss_tlv->header.len;
    pss_tlv->header.len = wlan_cpu_to_le16(pss_tlv->header.len);

    /* Get the common rates supported between the driver and the BSS Desc */
    if (wlan_setup_rates_from_bssdesc(pmpriv, pbss_desc, rates, &rates_size)) {
        ret = MLAN_STATUS_FAILURE;
        goto done;
    }

    /* Save the data rates into Current BSS state structure */
    pmpriv->curr_bss_params.num_of_rates = rates_size;
    memcpy(pmadapter, &pmpriv->curr_bss_params.data_rates, rates, rates_size);

    /* Setup the Rates TLV in the association command */
    prates_tlv = (MrvlIEtypes_RatesParamSet_t *) pos;
    prates_tlv->header.type = wlan_cpu_to_le16(TLV_TYPE_RATES);
    prates_tlv->header.len = wlan_cpu_to_le16((t_u16) rates_size);
    memcpy(pmadapter, prates_tlv->rates, rates, rates_size);
    pos += sizeof(prates_tlv->header) + rates_size;
    PRINTM(MINFO, "ASSOC_CMD: Rates size = %d\n", rates_size);

    /* Add the Authentication type to be used for Auth frames if needed */
    if ((pmpriv->sec_info.authentication_mode != MLAN_AUTH_MODE_AUTO) ||
	(pbss_desc->owe_transition_mode == OWE_TRANS_MODE_OWE)) {
        pauth_tlv = (MrvlIEtypes_AuthType_t *) pos;
        pauth_tlv->header.type = wlan_cpu_to_le16(TLV_TYPE_AUTH_TYPE);
        pauth_tlv->header.len = sizeof(pauth_tlv->auth_type);
	if (pmpriv->sec_info.authentication_mode == MLAN_AUTH_MODE_SAE)
		pauth_tlv->auth_type =
                wlan_cpu_to_le16((t_u16) AssocAgentAuth_Wpa3Sae);
	else if ((pbss_desc->owe_transition_mode == OWE_TRANS_MODE_OWE) ||
		(pmpriv->sec_info.authentication_mode == MLAN_AUTH_MODE_OWE))
		pauth_tlv->auth_type =
                wlan_cpu_to_le16((t_u16) AssocAgentAuth_Owe);
        else if (pmpriv->sec_info.wep_status == Wlan802_11WEPEnabled)
            pauth_tlv->auth_type =
                wlan_cpu_to_le16((t_u16) pmpriv->sec_info.authentication_mode);
        else
            pauth_tlv->auth_type = wlan_cpu_to_le16(MLAN_AUTH_MODE_OPEN);
        pos += sizeof(pauth_tlv->header) + pauth_tlv->header.len;
        pauth_tlv->header.len = wlan_cpu_to_le16(pauth_tlv->header.len);
    }

    if (IS_SUPPORT_MULTI_BANDS(pmadapter)
        && (pbss_desc->bss_band & pmpriv->config_bands)
        && !(ISSUPP_11NENABLED(pmadapter->fw_cap_info)
             && (!pbss_desc->disable_11n)
             && (pmpriv->config_bands & BAND_GN
                 || pmpriv->config_bands & BAND_AN)
             && (pbss_desc->pht_cap)
        )
        ) {
        /* Append a channel TLV for the channel the attempted AP was found on */
        pchan_tlv = (MrvlIEtypes_ChanListParamSet_t *) pos;
        pchan_tlv->header.type = wlan_cpu_to_le16(TLV_TYPE_CHANLIST);
        pchan_tlv->header.len = wlan_cpu_to_le16(sizeof(ChanScanParamSet_t));

        memset(pmadapter, pchan_tlv->chan_scan_param, 0x00,
               sizeof(ChanScanParamSet_t));
        pchan_tlv->chan_scan_param[0].chan_number =
            (pbss_desc->phy_param_set.ds_param_set.current_chan);
        PRINTM(MINFO, "Assoc: TLV Chan = %d\n",
               pchan_tlv->chan_scan_param[0].chan_number);

        pchan_tlv->chan_scan_param[0].radio_type =
            wlan_band_to_radio_type((t_u8) pbss_desc->bss_band);

        PRINTM(MINFO, "Assoc: TLV Band = %d\n",
               pchan_tlv->chan_scan_param[0].radio_type);
        pos += sizeof(pchan_tlv->header) + sizeof(ChanScanParamSet_t);
    }

#ifndef CONFIG_WPA2_ENTP
#ifdef CONFIG_WPS2
    if (!wps_session_attempt) {
#endif /* CONFIG_WPS2 */
#endif
	    /* fixme: The above 'if' is used instead of below 'if' for now since
	       WPS module is mlan integrated yet. Fix after it is done.
	    */
	    /* if (!pmpriv->wps.session_enable) { */
        if ((pmpriv->sec_info.wpa_enabled || pmpriv->sec_info.wpa2_enabled)||
		(pbss_desc->owe_transition_mode == OWE_TRANS_MODE_OWE) ||
		(pmpriv->sec_info.authentication_mode == MLAN_AUTH_MODE_OWE)) {
            prsn_ie_tlv = (MrvlIEtypes_RsnParamSet_t *) pos;
            prsn_ie_tlv->header.type = (t_u16) pmpriv->wpa_ie[0];       /* WPA_IE
                                                                           or
                                                                           RSN_IE
                                                                         */
            prsn_ie_tlv->header.type = prsn_ie_tlv->header.type & 0x00FF;
            prsn_ie_tlv->header.type =
                wlan_cpu_to_le16(prsn_ie_tlv->header.type);
            prsn_ie_tlv->header.len = (t_u16) pmpriv->wpa_ie[1];
            prsn_ie_tlv->header.len = prsn_ie_tlv->header.len & 0x00FF;
            if (prsn_ie_tlv->header.len <= (sizeof(pmpriv->wpa_ie) - 2))
                memcpy(pmadapter, prsn_ie_tlv->rsn_ie, &pmpriv->wpa_ie[2],
                       prsn_ie_tlv->header.len);
            else {
                ret = MLAN_STATUS_FAILURE;
                goto done;
            }
            HEXDUMP("ASSOC_CMD: RSN IE", (t_u8 *) prsn_ie_tlv,
                    sizeof(prsn_ie_tlv->header) + prsn_ie_tlv->header.len);
            pos += sizeof(prsn_ie_tlv->header) + prsn_ie_tlv->header.len;
            prsn_ie_tlv->header.len = wlan_cpu_to_le16(prsn_ie_tlv->header.len);
        } else if (pmpriv->sec_info.ewpa_enabled) {
            prsn_ie_tlv = (MrvlIEtypes_RsnParamSet_t *) pos;
            if (pbss_desc->pwpa_ie) {
                prsn_ie_tlv->header.type =
                    (t_u16) (*(pbss_desc->pwpa_ie)).vend_hdr.element_id;
                prsn_ie_tlv->header.type = prsn_ie_tlv->header.type & 0x00FF;
                prsn_ie_tlv->header.type =
                    wlan_cpu_to_le16(prsn_ie_tlv->header.type);
                prsn_ie_tlv->header.len =
                    (t_u16) (*(pbss_desc->pwpa_ie)).vend_hdr.len;
                prsn_ie_tlv->header.len = prsn_ie_tlv->header.len & 0x00FF;
                if (prsn_ie_tlv->header.len <= (sizeof(pmpriv->wpa_ie))) {
                    memcpy(pmadapter, prsn_ie_tlv->rsn_ie,
                           &((*(pbss_desc->pwpa_ie)).vend_hdr.oui[0]),
                           prsn_ie_tlv->header.len);
                } else {
                    ret = MLAN_STATUS_FAILURE;
                    goto done;
                }

                HEXDUMP("ASSOC_CMD: RSN IE", (t_u8 *) prsn_ie_tlv,
                        sizeof(prsn_ie_tlv->header) + prsn_ie_tlv->header.len);
                pos += sizeof(prsn_ie_tlv->header) + prsn_ie_tlv->header.len;
                prsn_ie_tlv->header.len =
                    wlan_cpu_to_le16(prsn_ie_tlv->header.len);
            }
            if (pbss_desc->prsn_ie) {
                prsn_ie_tlv = (MrvlIEtypes_RsnParamSet_t *) pos;
                prsn_ie_tlv->header.type =
                    (t_u16) (*(pbss_desc->prsn_ie)).ieee_hdr.element_id;
                prsn_ie_tlv->header.type = prsn_ie_tlv->header.type & 0x00FF;
                prsn_ie_tlv->header.type =
                    wlan_cpu_to_le16(prsn_ie_tlv->header.type);
                prsn_ie_tlv->header.len =
                    (t_u16) (*(pbss_desc->prsn_ie)).ieee_hdr.len;
                prsn_ie_tlv->header.len = prsn_ie_tlv->header.len & 0x00FF;
                if (prsn_ie_tlv->header.len <= (sizeof(pmpriv->wpa_ie))) {
                    memcpy(pmadapter, prsn_ie_tlv->rsn_ie,
                           &((*(pbss_desc->prsn_ie)).data[0])
                           , prsn_ie_tlv->header.len);
                } else {
                    ret = MLAN_STATUS_FAILURE;
                    goto done;
                }

                HEXDUMP("ASSOC_CMD: RSN IE", (t_u8 *) prsn_ie_tlv,
                        sizeof(prsn_ie_tlv->header) + prsn_ie_tlv->header.len);
                pos += sizeof(prsn_ie_tlv->header) + prsn_ie_tlv->header.len;
                prsn_ie_tlv->header.len =
                    wlan_cpu_to_le16(prsn_ie_tlv->header.len);
            }
        }
#ifndef CONFIG_WPA2_ENTP
#ifdef CONFIG_WPS2
    }
#endif /* CONFIG_WPS2 */
#endif

    if (ISSUPP_11NENABLED(pmadapter->fw_cap_info)
        && (!pbss_desc->disable_11n)
        && (pmpriv->config_bands & BAND_GN || pmpriv->config_bands & BAND_AN)
	&& wmsdk_is_11N_enabled()
	&& (!pmpriv->sec_info.is_wpa_tkip)) {
        wlan_cmd_append_11n_tlv(pmpriv, pbss_desc, &pos);
    }
    else if (pmpriv->hotspot_cfg & HOTSPOT_ENABLED)
        wlan_add_ext_capa_info_ie(pmpriv, &pos);

#ifdef CONFIG_WMM
    wlan_wmm_process_association_req(pmpriv, &pos, &pbss_desc->wmm_ie,
                                     pbss_desc->pht_cap);
#endif /* CONFIG_WMM */

    /* fixme: Currently not required */
#ifndef CONFIG_MLAN_WMSDK
    if (pmpriv->sec_info.wapi_enabled && pmpriv->wapi_ie_len) {
        wlan_cmd_append_wapi_ie(pmpriv, &pos);
    }

    wlan_cmd_append_generic_ie(pmpriv, &pos);

    wlan_cmd_append_tsf_tlv(pmpriv, &pos, pbss_desc);

    /* wmsdk: The below two steps have been performed before. So we can skip
       them here */
    if (wlan_11d_create_dnld_countryinfo(pmpriv, (t_u8) pbss_desc->bss_band)) {
        PRINTM(MERROR, "Dnld_countryinfo_11d failed\n");
        ret = MLAN_STATUS_FAILURE;
        goto done;
    }
    if (wlan_11d_parse_dnld_countryinfo(pmpriv, pmpriv->pattempted_bss_desc)) {
        ret = MLAN_STATUS_FAILURE;
        goto done;
    }

    /*
     * Call 11h join API after capability bits are set so adhoc/infra 11h
     * behavior can be properly triggered.  pos modified if data is appended
     */
    wlan_11h_process_join(pmpriv, &pos, &passo->cap_info,
                          (t_u8) pbss_desc->bss_band,
                          pbss_desc->phy_param_set.ds_param_set.current_chan,
                          &pbss_desc->wlan_11h_bss_info);
#endif /* CONFIG_MLAN_WMSDK */

    cmd->size = wlan_cpu_to_le16((t_u16) (pos - (t_u8 *) passo) + S_DS_GEN);

    /* Set the Capability info at last */
    memcpy(pmadapter, &tmp_cap, &pbss_desc->cap_info, sizeof(passo->cap_info));

    if (pmpriv->config_bands == BAND_B) {
        SHORT_SLOT_TIME_DISABLED(tmp_cap);
    }

    tmp_cap &= CAPINFO_MASK;
    PRINTM(MINFO, "ASSOC_CMD: tmp_cap=%4X CAPINFO_MASK=%4lX\n",
           tmp_cap, CAPINFO_MASK);
    tmp_cap = wlan_cpu_to_le16(tmp_cap);
    memcpy(pmadapter, &passo->cap_info, &tmp_cap, sizeof(passo->cap_info));

  done:
    LEAVE();
    return ret;
}

/**
 *  @brief Association firmware command response handler
 *
 *   The response buffer for the association command has the following
 *      memory layout.
 *
 *   For cases where an association response was not received (indicated
 *      by the CapInfo and AId field):
 *
 *     .------------------------------------------------------------.
 *     |  Header(4 * sizeof(t_u16)):  Standard command response hdr |
 *     .------------------------------------------------------------.
 *     |  cap_info/Error Return(t_u16):                             |
 *     |           0xFFFF(-1): Internal error for association       |
 *     |           0xFFFE(-2): Authentication unhandled message     |
 *     |           0xFFFD(-3): Authentication refused               |
 *     |           0xFFFC(-4): Timeout waiting for AP response      |
 *     |           0xFFFB(-5): Internal error for authentication    |
 *     .------------------------------------------------------------.
 *     |  status_code(t_u16):                                       |
 *     |        If cap_info is -1:                                  |
 *     |           An internal firmware failure prevented the       |
 *     |           command from being processed. The status code    |
 *     |           is 6 if associate response parameter invlaid,    |
 *     |           1 otherwise.                                     |
 *     |                                                            |
 *     |        If cap_info is -2:                                  |
 *     |           An authentication frame was received but was     |
 *     |           not handled by the firmware. IEEE Status code    |
 *     |           for the failure is returned.                     |
 *     |                                                            |
 *     |        If cap_info is -3:                                  |
 *     |           An authentication frame was received and the     |
 *     |           status_code is the IEEE Status reported in the   |
 *     |           response.                                        |
 *     |                                                            |
 *     |        If cap_info is -4:                                  |
 *     |           (1) Association response timeout                 |
 *     |           (2) Authentication response timeout              |
 *     |                                                            |
 *     |        If cap_info is -5:                                  |
 *     |           An internal firmware failure prevented the       |
 *     |           command from being processed. The status code    |
 *     |           is 6 if authentication parameter invlaid,        |
 *     |           1 otherwise.                                     |
 *     .------------------------------------------------------------.
 *     |  a_id(t_u16): 0xFFFF                                       |
 *     .------------------------------------------------------------.
 *
 *
 *   For cases where an association response was received, the IEEE
 *     standard association response frame is returned:
 *
 *     .------------------------------------------------------------.
 *     |  Header(4 * sizeof(t_u16)):  Standard command response hdr |
 *     .------------------------------------------------------------.
 *     |  cap_info(t_u16): IEEE Capability                          |
 *     .------------------------------------------------------------.
 *     |  status_code(t_u16): IEEE Status Code                      |
 *     .------------------------------------------------------------.
 *     |  a_id(t_u16): IEEE Association ID                          |
 *     .------------------------------------------------------------.
 *     |  IEEE IEs(variable): Any received IEs comprising the       |
 *     |                      remaining portion of a received       |
 *     |                      association response frame.           |
 *     .------------------------------------------------------------.
 *
 *  For simplistic handling, the status_code field can be used to determine
 *    an association success (0) or failure (non-zero).
 *
 *  @param pmpriv       A pointer to mlan_private structure
 *  @param resp         A pointer to HostCmd_DS_COMMAND
 *  @param pioctl_buf   A pointer to mlan_ioctl_req structure
 *
 *  @return             MLAN_STATUS_SUCCESS or MLAN_STATUS_FAILURE
 */
mlan_status
wlan_ret_802_11_associate(IN mlan_private * pmpriv,
                          IN HostCmd_DS_COMMAND * resp, IN t_void * pioctl_buf)
{
    mlan_status ret = MLAN_STATUS_SUCCESS;
    mlan_ioctl_req *pioctl_req = (mlan_ioctl_req *) pioctl_buf;
    IEEEtypes_AssocRsp_t *passoc_rsp;
    BSSDescriptor_t *pbss_desc;
    /* t_u8 enable_data = MTRUE; */
    /* t_u8 event_buf[100]; */
    /* mlan_event *pevent = (mlan_event *) event_buf; */
    t_u8 cur_mac[MLAN_MAC_ADDR_LENGTH];
    t_u8 media_connected = pmpriv->media_connected;
    /* mlan_adapter *pmadapter = pmpriv->adapter; */

    ENTER();

    passoc_rsp = (IEEEtypes_AssocRsp_t *) & resp->params;
    passoc_rsp->status_code = wlan_le16_to_cpu(passoc_rsp->status_code);
    if (pmpriv->media_connected == MTRUE)
        memcpy(pmpriv->adapter, cur_mac,
               pmpriv->curr_bss_params.bss_descriptor.mac_address,
               MLAN_MAC_ADDR_LENGTH);

    /* fixme: Enable this when req. We may not need to save the
       resp. buffer at all */
#ifndef CONFIG_MLAN_WMSDK
    HEXDUMP("ASSOC_RESP:", (t_u8 *) & resp->params, (resp->size - S_DS_GEN));

    pmpriv->assoc_rsp_size = MIN(resp->size - S_DS_GEN,
                                 sizeof(pmpriv->assoc_rsp_buf));

    memcpy(pmpriv->adapter, pmpriv->assoc_rsp_buf, &resp->params,
           pmpriv->assoc_rsp_size);
#endif /* CONFIG_MLAN_WMSDK */

    /* fixme: This is not needed as of now. */
#ifndef CONFIG_MLAN_WMSDK
    if (passoc_rsp->status_code) {
        if (pmpriv->media_connected == MTRUE) {
            if (pmpriv->port_ctrl_mode == MTRUE)
                pmpriv->port_open = pmpriv->prior_port_status;
            if (!memcmp
                (pmpriv->adapter, cur_mac,
                 pmpriv->pattempted_bss_desc->mac_address,
                 MLAN_MAC_ADDR_LENGTH))
                wlan_reset_connect_state(pmpriv, MTRUE);
            else
                wlan_recv_event(pmpriv, MLAN_EVENT_ID_DRV_ASSOC_FAILURE_REPORT,
                                MNULL);
        }
        pmpriv->adapter->dbg.num_cmd_assoc_failure++;
        PRINTM(MERROR, "ASSOC_RESP: Association Failed, "
               "status code = %d, error = 0x%x, a_id = 0x%x\n",
               wlan_le16_to_cpu(passoc_rsp->status_code),
               wlan_le16_to_cpu(*(t_u16 *) & passoc_rsp->capability),
               wlan_le16_to_cpu(passoc_rsp->a_id));

        ret = MLAN_STATUS_FAILURE;
        goto done;
    }
#endif /* CONFIG_MLAN_WMSDK */

    /* Send a Media Connected event, according to the Spec */
    pmpriv->media_connected = MTRUE;

#ifndef CONFIG_MLAN_WMSDK
    pmpriv->adapter->pps_uapsd_mode = MFALSE;
    pmpriv->adapter->tx_lock_flag = MFALSE;
    pmpriv->adapter->delay_null_pkt = MFALSE;
#endif /* CONFIG_MLAN_WMSDK */

    /* Set the attempted BSSID Index to current */
    pbss_desc = pmpriv->pattempted_bss_desc;

    PRINTM(MINFO, "ASSOC_RESP: %s\n", pbss_desc->ssid.ssid);

    /* Make a copy of current BSSID descriptor */
    memcpy(pmpriv->adapter, &pmpriv->curr_bss_params.bss_descriptor,
           pbss_desc, sizeof(BSSDescriptor_t));

    /* Update curr_bss_params */
    pmpriv->curr_bss_params.bss_descriptor.channel
        = pbss_desc->phy_param_set.ds_param_set.current_chan;

    pmpriv->curr_bss_params.band = (t_u8) pbss_desc->bss_band;

#ifndef CONFIG_MLAN_WMSDK
    /*
     * Adjust the timestamps in the scan table to be relative to the newly
     * associated AP's TSF
     */
    wlan_update_tsf_timestamps(pmpriv, pbss_desc);
#endif /* CONFIG_MLAN_WMSDK */

    if (pbss_desc->wmm_ie.vend_hdr.element_id == WMM_IE)
        pmpriv->curr_bss_params.wmm_enabled = MTRUE;
    else
        pmpriv->curr_bss_params.wmm_enabled = MFALSE;

    if ((pmpriv->wmm_required
         || (pbss_desc->pht_cap &&
             (pbss_desc->pht_cap->ieee_hdr.element_id == HT_CAPABILITY))
        ) && pmpriv->curr_bss_params.wmm_enabled)
        pmpriv->wmm_enabled = MTRUE;
    else
        pmpriv->wmm_enabled = MFALSE;

    /* fixme: Enable if req */
#ifndef CONFIG_MLAN_WMSDK
    pmpriv->curr_bss_params.wmm_uapsd_enabled = MFALSE;

    if (pmpriv->wmm_enabled == MTRUE)
        pmpriv->curr_bss_params.wmm_uapsd_enabled
            = pbss_desc->wmm_ie.qos_info.qos_uapsd;

    PRINTM(MINFO, "ASSOC_RESP: curr_pkt_filter is 0x%x\n",
           pmpriv->curr_pkt_filter);
#endif /* CONFIG_MLAN_WMSDK */
    if (pmpriv->sec_info.wpa_enabled || pmpriv->sec_info.wpa2_enabled)
        pmpriv->wpa_is_gtk_set = MFALSE;

#ifndef CONFIG_MLAN_WMSDK
    if (pmpriv->wmm_enabled)
        /* Don't re-enable carrier until we get the WMM_GET_STATUS event */
        enable_data = MFALSE;
    else
        /* Since WMM is not enabled, setup the queues with the defaults */
        wlan_wmm_setup_queues(pmpriv);

    if (enable_data) {
        PRINTM(MINFO, "Post association, re-enabling data flow\n");
    }
#endif /* CONFIG_MLAN_WMSDK */

    /* Reset SNR/NF/RSSI values */
    pmpriv->data_rssi_last = 0;
    pmpriv->data_nf_last = 0;
    pmpriv->data_rssi_avg = 0;
    pmpriv->data_nf_avg = 0;
    pmpriv->bcn_rssi_last = 0;
    pmpriv->bcn_nf_last = 0;
    pmpriv->bcn_rssi_avg = 0;
    pmpriv->bcn_nf_avg = 0;
    pmpriv->rxpd_rate = 0;
    pmpriv->rxpd_htinfo = 0;
    if (pbss_desc->pht_cap) {
        if (GETHT_MAXAMSDU(pbss_desc->pht_cap->ht_cap.ht_cap_info))
            pmpriv->max_amsdu = MLAN_TX_DATA_BUF_SIZE_8K;
        else
            pmpriv->max_amsdu = MLAN_TX_DATA_BUF_SIZE_4K;
    }


#ifndef CONFIG_MLAN_WMSDK
    /* fixme: We do not need to save the beacon buffer */
    wlan_save_curr_bcn(pmpriv);

    pmpriv->adapter->dbg.num_cmd_assoc_success++;

    PRINTM(MINFO, "ASSOC_RESP: Associated\n");
    pevent->bss_index = pmpriv->bss_index;
    pevent->event_id = MLAN_EVENT_ID_DRV_CONNECTED;
    pevent->event_len = MLAN_MAC_ADDR_LENGTH;
    memcpy(pmpriv->adapter, (t_u8 *) pevent->event_buf,
           (t_u8 *) pmpriv->curr_bss_params.bss_descriptor.mac_address,
           MLAN_MAC_ADDR_LENGTH);
#endif /* CONFIG_MLAN_WMSDK */
    /* Add the ra_list here for infra mode as there will be only 1 ra always */
    if (media_connected) {
#ifndef CONFIG_MLAN_WMSDK
        /** replace ralist's mac address with new mac address */
        if (0 ==
            wlan_ralist_update(pmpriv, cur_mac,
                               pmpriv->curr_bss_params.bss_descriptor.
                               mac_address))
            wlan_ralist_add(pmpriv,
                            pmpriv->curr_bss_params.bss_descriptor.mac_address);
#endif /* CONFIG_MLAN_WMSDK */
        wlan_11n_cleanup_reorder_tbl(pmpriv);
#ifndef CONFIG_MLAN_WMSDK
        pmadapter->callbacks.moal_spin_lock(pmadapter->pmoal_handle,
                                            pmpriv->wmm.ra_list_spinlock);
        wlan_11n_deleteall_txbastream_tbl(pmpriv);
        pmadapter->callbacks.moal_spin_unlock(pmadapter->pmoal_handle,
                                              pmpriv->wmm.ra_list_spinlock);
#endif /* CONFIG_MLAN_WMSDK */
    }
#ifndef CONFIG_MLAN_WMSDK
else
        wlan_ralist_add(pmpriv,
                        pmpriv->curr_bss_params.bss_descriptor.mac_address);

    wlan_recv_event(pmpriv, MLAN_EVENT_ID_DRV_CONNECTED, pevent);

    /* Send OBSS scan param to the application if available */
    wlan_2040_coex_event(pmpriv);
#endif /* CONFIG_MLAN_WMSDK */
    if (!pmpriv->sec_info.wpa_enabled
        && !pmpriv->sec_info.wpa2_enabled
        && !pmpriv->sec_info.ewpa_enabled && !pmpriv->sec_info.wapi_enabled) {
        /* We are in Open/WEP mode, open port immediately */
        if (pmpriv->port_ctrl_mode == MTRUE) {
            pmpriv->port_open = MTRUE;
            PRINTM(MINFO, "ASSOC_RESP: port_status = OPEN\n");
        }
    }

    if (pmpriv->sec_info.wpa_enabled
        || pmpriv->sec_info.wpa2_enabled
        || pmpriv->sec_info.ewpa_enabled || pmpriv->sec_info.wapi_enabled)
        pmpriv->scan_block = MTRUE;

  /* done: */
    /* Need to indicate IOCTL complete */
    if (pioctl_req != MNULL) {
        if (ret != MLAN_STATUS_SUCCESS) {
            if (passoc_rsp->status_code)
                pioctl_req->status_code =
                    wlan_le16_to_cpu(passoc_rsp->status_code);
            else
                pioctl_req->status_code = MLAN_ERROR_CMD_ASSOC_FAIL;
        } else {
            pioctl_req->status_code = MLAN_ERROR_NO_ERROR;
        }
    }

    LEAVE();
    return ret;
}

#ifndef CONFIG_MLAN_WMSDK
/**
 *  @brief This function prepares command of ad_hoc_start.
 *
 *  @param pmpriv       A pointer to mlan_private structure
 *  @param cmd          A pointer to HostCmd_DS_COMMAND structure
 *  @param pdata_buf    A pointer cast of mlan_802_11_ssid structure
 *
 *  @return             MLAN_STATUS_SUCCESS or MLAN_STATUS_FAILURE
 */
mlan_status
wlan_cmd_802_11_ad_hoc_start(IN mlan_private * pmpriv,
                             IN HostCmd_DS_COMMAND * cmd, IN t_void * pdata_buf)
{
    mlan_status ret = MLAN_STATUS_SUCCESS;
    mlan_adapter *pmadapter = pmpriv->adapter;
    HostCmd_DS_802_11_AD_HOC_START *padhoc_start = &cmd->params.adhoc_start;
    BSSDescriptor_t *pbss_desc;
    t_u32 cmd_append_size = 0;
    t_u32 i;
    t_u16 tmp_cap;
    MrvlIEtypes_ChanListParamSet_t *pchan_tlv;

    MrvlIEtypes_RsnParamSet_t *prsn_ie_tlv;
    MrvlIETypes_HTCap_t *pht_cap;
    MrvlIETypes_HTInfo_t *pht_info;
    /* wpa ie for WPA_NONE AES */
    const t_u8 wpa_ie[24] = { 0xdd, 0x16, 0x00, 0x50, 0xf2, 0x01, 0x01, 0x00,
        0x00, 0x50, 0xf2, 0x04, 0x01, 0x00, 0x00, 0x50,
        0xf2, 0x00, 0x01, 0x00, 0x00, 0x50, 0xf2, 0x00
    };
    t_s32 append_size_11h = 0;
    t_u8 *pos = (t_u8 *) padhoc_start + sizeof(HostCmd_DS_802_11_AD_HOC_START);

    ENTER();

    if (!pmadapter) {
        ret = MLAN_STATUS_FAILURE;
        goto done;
    }

    cmd->command = wlan_cpu_to_le16(HostCmd_CMD_802_11_AD_HOC_START);

    pbss_desc = &pmpriv->curr_bss_params.bss_descriptor;
    pmpriv->pattempted_bss_desc = pbss_desc;

    /*
     * Fill in the parameters for 2 data structures:
     *   1. HostCmd_DS_802_11_AD_HOC_START command
     *   2. pbss_desc
     * Driver will fill up SSID, bss_mode,IBSS param, Physical Param,
     * probe delay, and Cap info.
     * Firmware will fill up beacon period, Basic rates
     * and operational rates.
     */

    memset(pmadapter, padhoc_start->ssid, 0, MLAN_MAX_SSID_LENGTH);

    memcpy(pmadapter, padhoc_start->ssid,
           ((mlan_802_11_ssid *) pdata_buf)->ssid,
           MIN(MLAN_MAX_SSID_LENGTH,
               ((mlan_802_11_ssid *) pdata_buf)->ssid_len));

    PRINTM(MINFO, "ADHOC_S_CMD: SSID = %s\n", padhoc_start->ssid);

    memset(pmadapter, pbss_desc->ssid.ssid, 0, MLAN_MAX_SSID_LENGTH);
    memcpy(pmadapter, pbss_desc->ssid.ssid,
           ((mlan_802_11_ssid *) pdata_buf)->ssid,
           MIN(MLAN_MAX_SSID_LENGTH,
               ((mlan_802_11_ssid *) pdata_buf)->ssid_len));

    pbss_desc->ssid.ssid_len =
        MIN(MLAN_MAX_SSID_LENGTH, ((mlan_802_11_ssid *) pdata_buf)->ssid_len);

    /* Set the BSS mode */
    padhoc_start->bss_mode = HostCmd_BSS_MODE_IBSS;
    pbss_desc->bss_mode = MLAN_BSS_MODE_IBSS;
    padhoc_start->beacon_period = wlan_cpu_to_le16(pmpriv->beacon_period);
    pbss_desc->beacon_period = pmpriv->beacon_period;

    /* Set Physical param set */
/** Parameter IE Id */
#define DS_PARA_IE_ID   3
/** Parameter IE length */
#define DS_PARA_IE_LEN  1

    padhoc_start->phy_param_set.ds_param_set.element_id = DS_PARA_IE_ID;
    padhoc_start->phy_param_set.ds_param_set.len = DS_PARA_IE_LEN;

    if (!wlan_get_cfp_by_band_and_channel
        (pmadapter, pmadapter->adhoc_start_band, (t_u16) pmpriv->adhoc_channel,
         pmadapter->region_channel)) {

        chan_freq_power_t *cfp;
        cfp =
            wlan_get_cfp_by_band_and_channel(pmadapter,
                                             pmadapter->adhoc_start_band,
                                             FIRST_VALID_CHANNEL,
                                             pmadapter->region_channel);
        if (cfp)
            pmpriv->adhoc_channel = (t_u8) cfp->channel;
    }

    MASSERT(pmpriv->adhoc_channel);

    PRINTM(MINFO, "ADHOC_S_CMD: Creating ADHOC on Channel %d\n",
           pmpriv->adhoc_channel);

    pmpriv->curr_bss_params.bss_descriptor.channel = pmpriv->adhoc_channel;
    pmpriv->curr_bss_params.band = pmadapter->adhoc_start_band;

    pbss_desc->channel = pmpriv->adhoc_channel;
    padhoc_start->phy_param_set.ds_param_set.current_chan =
        pmpriv->adhoc_channel;

    memcpy(pmadapter, &pbss_desc->phy_param_set,
           &padhoc_start->phy_param_set, sizeof(IEEEtypes_PhyParamSet_t));

    pbss_desc->network_type_use = Wlan802_11DS;

    /* Set IBSS param set */
/** IBSS parameter IE Id */
#define IBSS_PARA_IE_ID   6
/** IBSS parameter IE length */
#define IBSS_PARA_IE_LEN  2

    padhoc_start->ss_param_set.ibss_param_set.element_id = IBSS_PARA_IE_ID;
    padhoc_start->ss_param_set.ibss_param_set.len = IBSS_PARA_IE_LEN;
    padhoc_start->ss_param_set.ibss_param_set.atim_window
        = wlan_cpu_to_le16(pmpriv->atim_window);
    pbss_desc->atim_window = pmpriv->atim_window;
    memcpy(pmadapter, &pbss_desc->ss_param_set,
           &padhoc_start->ss_param_set, sizeof(IEEEtypes_SsParamSet_t));

    /* Set Capability info */
    padhoc_start->cap.ess = 0;
    padhoc_start->cap.ibss = 1;
    pbss_desc->cap_info.ibss = 1;

    /* Set up privacy in pbss_desc */
    if (pmpriv->sec_info.wep_status == Wlan802_11WEPEnabled
        || pmpriv->adhoc_aes_enabled
        || pmpriv->sec_info.wpa_enabled || pmpriv->sec_info.ewpa_enabled) {
/** Ad-Hoc capability privacy on */
#define AD_HOC_CAP_PRIVACY_ON   1
        PRINTM(MINFO, "ADHOC_S_CMD: wep_status set, Privacy to WEP\n");
        pbss_desc->privacy = Wlan802_11PrivFilter8021xWEP;
        padhoc_start->cap.privacy = AD_HOC_CAP_PRIVACY_ON;
    } else {
        PRINTM(MWARN, "ADHOC_S_CMD: wep_status NOT set, Setting "
               "Privacy to ACCEPT ALL\n");
        pbss_desc->privacy = Wlan802_11PrivFilterAcceptAll;
    }

    memset(pmadapter, padhoc_start->DataRate, 0,
           sizeof(padhoc_start->DataRate));
    wlan_get_active_data_rates(pmpriv, pmpriv->bss_mode,
                               pmadapter->adhoc_start_band,
                               padhoc_start->DataRate);
    if ((pmadapter->adhoc_start_band & BAND_G) &&
        (pmpriv->curr_pkt_filter & HostCmd_ACT_MAC_ADHOC_G_PROTECTION_ON)) {
        ret =
            wlan_prepare_cmd(pmpriv, HostCmd_CMD_MAC_CONTROL,
                             HostCmd_ACT_GEN_SET, 0, MNULL,
                             &pmpriv->curr_pkt_filter);

        if (ret) {
            PRINTM(MERROR, "ADHOC_S_CMD: G Protection config failed\n");
            ret = MLAN_STATUS_FAILURE;
            goto done;
        }
    }
    /* Find the last non zero */
    for (i = 0; i < sizeof(padhoc_start->DataRate) && padhoc_start->DataRate[i];
         i++);

    pmpriv->curr_bss_params.num_of_rates = i;

    /* Copy the ad-hoc creating rates into Current BSS rate structure */
    memcpy(pmadapter, &pmpriv->curr_bss_params.data_rates,
           &padhoc_start->DataRate, pmpriv->curr_bss_params.num_of_rates);

    PRINTM(MINFO, "ADHOC_S_CMD: Rates=%02x %02x %02x %02x \n",
           padhoc_start->DataRate[0], padhoc_start->DataRate[1],
           padhoc_start->DataRate[2], padhoc_start->DataRate[3]);

    PRINTM(MINFO, "ADHOC_S_CMD: AD HOC Start command is ready\n");

    if (IS_SUPPORT_MULTI_BANDS(pmadapter)) {
        /* Append a channel TLV */
        pchan_tlv = (MrvlIEtypes_ChanListParamSet_t *) pos;
        pchan_tlv->header.type = wlan_cpu_to_le16(TLV_TYPE_CHANLIST);
        pchan_tlv->header.len = wlan_cpu_to_le16(sizeof(ChanScanParamSet_t));

        memset(pmadapter, pchan_tlv->chan_scan_param, 0x00,
               sizeof(ChanScanParamSet_t));
        pchan_tlv->chan_scan_param[0].chan_number =
            (t_u8) pmpriv->curr_bss_params.bss_descriptor.channel;

        PRINTM(MINFO, "ADHOC_S_CMD: TLV Chan = %d\n",
               pchan_tlv->chan_scan_param[0].chan_number);

        pchan_tlv->chan_scan_param[0].radio_type
            = wlan_band_to_radio_type(pmpriv->curr_bss_params.band);
        if (pmadapter->adhoc_start_band & BAND_GN
            || pmadapter->adhoc_start_band & BAND_AN) {
            if (pmadapter->chan_bandwidth == CHANNEL_BW_40MHZ_ABOVE) {
                pchan_tlv->chan_scan_param[0].radio_type |=
                    SECOND_CHANNEL_ABOVE;
                pchan_tlv->chan_scan_param[0].radio_type |= CHAN_BW_40MHZ << 2;
            } else if (pmadapter->chan_bandwidth == CHANNEL_BW_40MHZ_BELOW) {
                pchan_tlv->chan_scan_param[0].radio_type |=
                    SECOND_CHANNEL_BELOW;
                pchan_tlv->chan_scan_param[0].radio_type |= CHAN_BW_40MHZ << 2;
            }
        }
        PRINTM(MINFO, "ADHOC_S_CMD: TLV Band = %d\n",
               pchan_tlv->chan_scan_param[0].radio_type);
        pos += sizeof(pchan_tlv->header) + sizeof(ChanScanParamSet_t);
        cmd_append_size +=
            sizeof(pchan_tlv->header) + sizeof(ChanScanParamSet_t);
    }

    if (wlan_11d_create_dnld_countryinfo(pmpriv, pmpriv->curr_bss_params.band)) {
        PRINTM(MERROR, "ADHOC_S_CMD: dnld_countryinfo_11d failed\n");
        ret = MLAN_STATUS_FAILURE;
        goto done;
    }

    /*
     * Call 11h start API to add any 11h flags/elements as TLV parameters
     */
    append_size_11h = wlan_11h_process_start(pmpriv, &pos, &padhoc_start->cap,
                                             pmpriv->adhoc_channel,
                                             &pbss_desc->wlan_11h_bss_info);
    if (append_size_11h >= 0)
        cmd_append_size += append_size_11h;
    else {
        ret = MLAN_STATUS_FAILURE;
        goto done;
    }

    if (pmpriv->sec_info.ewpa_enabled) {
        memcpy(pmadapter, pmpriv->wpa_ie, wpa_ie, sizeof(wpa_ie));
        pmpriv->wpa_ie_len = sizeof(wpa_ie);
    }

    if (pmpriv->sec_info.wpa_enabled || pmpriv->sec_info.ewpa_enabled) {
        prsn_ie_tlv = (MrvlIEtypes_RsnParamSet_t *) pos;
        prsn_ie_tlv->header.type = (t_u16) pmpriv->wpa_ie[0];   /* WPA_IE or
                                                                   RSN_IE */
        prsn_ie_tlv->header.type = prsn_ie_tlv->header.type & 0x00FF;
        prsn_ie_tlv->header.type = wlan_cpu_to_le16(prsn_ie_tlv->header.type);
        prsn_ie_tlv->header.len = (t_u16) pmpriv->wpa_ie[1];
        prsn_ie_tlv->header.len = prsn_ie_tlv->header.len & 0x00FF;
        if (prsn_ie_tlv->header.len <= (sizeof(pmpriv->wpa_ie) - 2))
            memcpy(pmadapter, prsn_ie_tlv->rsn_ie, &pmpriv->wpa_ie[2],
                   prsn_ie_tlv->header.len);
        else {
            ret = MLAN_STATUS_FAILURE;
            goto done;
        }

        DBG_HEXDUMP(MCMD_D, "ADHOC_S_CMD: RSN IE", (t_u8 *) prsn_ie_tlv,
                    sizeof(prsn_ie_tlv->header) + prsn_ie_tlv->header.len);
        pos += sizeof(prsn_ie_tlv->header) + prsn_ie_tlv->header.len;
        cmd_append_size +=
            sizeof(prsn_ie_tlv->header) + prsn_ie_tlv->header.len;
        prsn_ie_tlv->header.len = wlan_cpu_to_le16(prsn_ie_tlv->header.len);
    }

    if (pmadapter->adhoc_11n_enabled == MTRUE) {
        {
            pht_cap = (MrvlIETypes_HTCap_t *) pos;
            memset(pmadapter, pht_cap, 0, sizeof(MrvlIETypes_HTCap_t));
            pht_cap->header.type = wlan_cpu_to_le16(HT_CAPABILITY);
            pht_cap->header.len = sizeof(HTCap_t);
            wlan_fill_ht_cap_tlv(pmpriv, pht_cap, pmpriv->curr_bss_params.band);
            HEXDUMP("ADHOC_START: HT_CAPABILITIES IE", (t_u8 *) pht_cap,
                    sizeof(MrvlIETypes_HTCap_t));
            pos += sizeof(MrvlIETypes_HTCap_t);
            cmd_append_size += sizeof(MrvlIETypes_HTCap_t);
            pht_cap->header.len = wlan_cpu_to_le16(pht_cap->header.len);
        }
        {
            pht_info = (MrvlIETypes_HTInfo_t *) pos;
            memset(pmadapter, pht_info, 0, sizeof(MrvlIETypes_HTInfo_t));
            pht_info->header.type = wlan_cpu_to_le16(HT_OPERATION);
            pht_info->header.len = sizeof(HTInfo_t);
            pht_info->ht_info.pri_chan =
                (t_u8) pmpriv->curr_bss_params.bss_descriptor.channel;
            if (pmadapter->chan_bandwidth) {
                pht_info->ht_info.field2 = pmadapter->chan_bandwidth;
                SET_CHANWIDTH40(pht_info->ht_info.field2);
            }
            pht_info->ht_info.field3 = NON_GREENFIELD_STAS;
            pht_info->ht_info.basic_mcs_set[0] = 0xff;
            HEXDUMP("ADHOC_START: HT_INFORMATION IE", (t_u8 *) pht_info,
                    sizeof(MrvlIETypes_HTInfo_t));
            pos += sizeof(MrvlIETypes_HTInfo_t);
            cmd_append_size += sizeof(MrvlIETypes_HTInfo_t);
            pht_info->header.len = wlan_cpu_to_le16(pht_info->header.len);
        }
    }

    cmd->size =
        (t_u16) wlan_cpu_to_le16((t_u16) (sizeof(HostCmd_DS_802_11_AD_HOC_START)
                                          + S_DS_GEN + cmd_append_size));

    memcpy(pmadapter, &tmp_cap, &padhoc_start->cap, sizeof(t_u16));

    if (pmadapter->adhoc_start_band == BAND_B) {
        SHORT_SLOT_TIME_DISABLED(tmp_cap);
    } else {
        SHORT_SLOT_TIME_ENABLED(tmp_cap);
    }

    tmp_cap = wlan_cpu_to_le16(tmp_cap);
    memcpy(pmadapter, &padhoc_start->cap, &tmp_cap, sizeof(t_u16));

    ret = MLAN_STATUS_SUCCESS;
  done:
    LEAVE();
    return ret;
}

/**
 *  @brief This function prepares command of ad_hoc_join.
 *
 *  @param pmpriv       A pointer to mlan_private structure
 *  @param cmd          A pointer to HostCmd_DS_COMMAND structure
 *  @param pdata_buf    Void cast of BSSDescriptor_t from the
 *                        scan table to join
 *
 *  @return             MLAN_STATUS_SUCCESS or MLAN_STATUS_FAILURE
 */
mlan_status
wlan_cmd_802_11_ad_hoc_join(IN mlan_private * pmpriv,
                            IN HostCmd_DS_COMMAND * cmd, IN t_void * pdata_buf)
{
    mlan_status ret = MLAN_STATUS_SUCCESS;
    mlan_adapter *pmadapter = pmpriv->adapter;
    HostCmd_DS_802_11_AD_HOC_JOIN *padhoc_join = &cmd->params.adhoc_join;
    BSSDescriptor_t *pbss_desc = (BSSDescriptor_t *) pdata_buf;
    MrvlIEtypes_ChanListParamSet_t *pchan_tlv;
    MrvlIEtypes_RsnParamSet_t *prsn_ie_tlv;
    t_u32 cmd_append_size = 0;
    t_u16 tmp_cap;
    t_u32 i, rates_size = 0;
    t_u16 curr_pkt_filter;
    t_u8 *pos = (t_u8 *) padhoc_join + sizeof(HostCmd_DS_802_11_AD_HOC_JOIN);

    ENTER();

/** Use G protection */
#define USE_G_PROTECTION        0x02
    if (pbss_desc->erp_flags & USE_G_PROTECTION) {
        curr_pkt_filter =
            pmpriv->curr_pkt_filter | HostCmd_ACT_MAC_ADHOC_G_PROTECTION_ON;

        ret = wlan_prepare_cmd(pmpriv,
                               HostCmd_CMD_MAC_CONTROL,
                               HostCmd_ACT_GEN_SET, 0, MNULL, &curr_pkt_filter);
        if (ret) {
            PRINTM(MERROR, "ADHOC_J_CMD: G Protection config failed\n");
            ret = MLAN_STATUS_FAILURE;
            goto done;
        }
    }

    pmpriv->pattempted_bss_desc = pbss_desc;

    cmd->command = wlan_cpu_to_le16(HostCmd_CMD_802_11_AD_HOC_JOIN);

    padhoc_join->bss_descriptor.bss_mode = HostCmd_BSS_MODE_IBSS;

    padhoc_join->bss_descriptor.beacon_period
        = wlan_cpu_to_le16(pbss_desc->beacon_period);

    memcpy(pmadapter, &padhoc_join->bss_descriptor.bssid,
           &pbss_desc->mac_address, MLAN_MAC_ADDR_LENGTH);

    memcpy(pmadapter, &padhoc_join->bss_descriptor.ssid,
           &pbss_desc->ssid.ssid,
           MIN(MLAN_MAX_SSID_LENGTH, pbss_desc->ssid.ssid_len));

    memcpy(pmadapter, &padhoc_join->bss_descriptor.phy_param_set,
           &pbss_desc->phy_param_set, sizeof(IEEEtypes_PhyParamSet_t));

    memcpy(pmadapter, &padhoc_join->bss_descriptor.ss_param_set,
           &pbss_desc->ss_param_set, sizeof(IEEEtypes_SsParamSet_t));
    padhoc_join->bss_descriptor.ss_param_set.ibss_param_set.atim_window = 0;

    memcpy(pmadapter, &tmp_cap, &pbss_desc->cap_info,
           sizeof(IEEEtypes_CapInfo_t));

    tmp_cap &= CAPINFO_MASK;

    PRINTM(MINFO, "ADHOC_J_CMD: tmp_cap=%4X CAPINFO_MASK=%4lX\n",
           tmp_cap, CAPINFO_MASK);
    memcpy(pmadapter, &padhoc_join->bss_descriptor.cap, &tmp_cap,
           sizeof(IEEEtypes_CapInfo_t));

    /* Information on BSSID descriptor passed to FW */
    PRINTM(MINFO,
           "ADHOC_J_CMD: BSSID = %02x-%02x-%02x-%02x-%02x-%02x, SSID = %s\n",
           padhoc_join->bss_descriptor.bssid[0],
           padhoc_join->bss_descriptor.bssid[1],
           padhoc_join->bss_descriptor.bssid[2],
           padhoc_join->bss_descriptor.bssid[3],
           padhoc_join->bss_descriptor.bssid[4],
           padhoc_join->bss_descriptor.bssid[5],
           padhoc_join->bss_descriptor.ssid);

    for (i = 0; i < WLAN_SUPPORTED_RATES && pbss_desc->supported_rates[i]; i++);
    rates_size = i;

    /* Copy Data Rates from the Rates recorded in scan response */
    memset(pmadapter, padhoc_join->bss_descriptor.data_rates, 0,
           sizeof(padhoc_join->bss_descriptor.data_rates));
    memcpy(pmadapter, padhoc_join->bss_descriptor.data_rates,
           pbss_desc->supported_rates, rates_size);

    HEXDUMP("Adapted Rates:", padhoc_join->bss_descriptor.data_rates,
            rates_size);

    /* Copy the adhoc join rates into Current BSS state structure */
    pmpriv->curr_bss_params.num_of_rates = rates_size;
    memcpy(pmadapter, &pmpriv->curr_bss_params.data_rates,
           pbss_desc->supported_rates, rates_size);

    /* Copy the channel information */
    pmpriv->curr_bss_params.bss_descriptor.channel = pbss_desc->channel;
    pmpriv->curr_bss_params.band = (t_u8) pbss_desc->bss_band;

    if (pmpriv->sec_info.wep_status == Wlan802_11WEPEnabled
        || pmpriv->adhoc_aes_enabled
        || pmpriv->sec_info.wpa_enabled || pmpriv->sec_info.ewpa_enabled)
        padhoc_join->bss_descriptor.cap.privacy = AD_HOC_CAP_PRIVACY_ON;

    if (IS_SUPPORT_MULTI_BANDS(pmadapter)) {
        /* Append a channel TLV */
        pchan_tlv = (MrvlIEtypes_ChanListParamSet_t *) pos;
        pchan_tlv->header.type = wlan_cpu_to_le16(TLV_TYPE_CHANLIST);
        pchan_tlv->header.len = wlan_cpu_to_le16(sizeof(ChanScanParamSet_t));

        memset(pmadapter, pchan_tlv->chan_scan_param, 0x00,
               sizeof(ChanScanParamSet_t));
        pchan_tlv->chan_scan_param[0].chan_number =
            (pbss_desc->phy_param_set.ds_param_set.current_chan);
        PRINTM(MINFO, "ADHOC_J_CMD: TLV Chan = %d\n",
               pchan_tlv->chan_scan_param[0].chan_number);

        pchan_tlv->chan_scan_param[0].radio_type
            = wlan_band_to_radio_type((t_u8) pbss_desc->bss_band);

        PRINTM(MINFO, "ADHOC_J_CMD: TLV Band = %d\n",
               pchan_tlv->chan_scan_param[0].radio_type);
        pos += sizeof(pchan_tlv->header) + sizeof(ChanScanParamSet_t);
        cmd_append_size +=
            sizeof(pchan_tlv->header) + sizeof(ChanScanParamSet_t);
    }

    if (wlan_11d_create_dnld_countryinfo(pmpriv, (t_u8) pbss_desc->bss_band)) {
        PRINTM(MERROR, "Dnld_countryinfo_11d failed\n");
        ret = MLAN_STATUS_FAILURE;
        goto done;
    }
    if (wlan_11d_parse_dnld_countryinfo(pmpriv, pmpriv->pattempted_bss_desc)) {
        ret = MLAN_STATUS_FAILURE;
        goto done;
    }

    /*
     * Call 11h join API after capability bits are set so
     *   adhoc/infra 11h behavior can be properly triggered.
     *   pos modified if data is appended
     */
    cmd_append_size += wlan_11h_process_join(pmpriv, &pos,
                                             &padhoc_join->bss_descriptor.cap,
                                             (t_u8) pbss_desc->bss_band,
                                             pbss_desc->channel,
                                             &pbss_desc->wlan_11h_bss_info);

    if (pmpriv->sec_info.wpa_enabled) {
        prsn_ie_tlv = (MrvlIEtypes_RsnParamSet_t *) pos;
        prsn_ie_tlv->header.type = (t_u16) pmpriv->wpa_ie[0];   /* WPA_IE or
                                                                   RSN_IE */
        prsn_ie_tlv->header.type = prsn_ie_tlv->header.type & 0x00FF;
        prsn_ie_tlv->header.type = wlan_cpu_to_le16(prsn_ie_tlv->header.type);
        prsn_ie_tlv->header.len = (t_u16) pmpriv->wpa_ie[1];
        prsn_ie_tlv->header.len = prsn_ie_tlv->header.len & 0x00FF;
        if (prsn_ie_tlv->header.len <= (sizeof(pmpriv->wpa_ie) - 2))
            memcpy(pmadapter, prsn_ie_tlv->rsn_ie, &pmpriv->wpa_ie[2],
                   prsn_ie_tlv->header.len);
        else {
            ret = MLAN_STATUS_FAILURE;
            goto done;
        }

        HEXDUMP("ADHOC_JOIN: RSN IE", (t_u8 *) prsn_ie_tlv,
                sizeof(prsn_ie_tlv->header) + prsn_ie_tlv->header.len);
        pos += sizeof(prsn_ie_tlv->header) + prsn_ie_tlv->header.len;
        cmd_append_size +=
            sizeof(prsn_ie_tlv->header) + prsn_ie_tlv->header.len;
        prsn_ie_tlv->header.len = wlan_cpu_to_le16(prsn_ie_tlv->header.len);
    } else if (pmpriv->sec_info.ewpa_enabled) {
        prsn_ie_tlv = (MrvlIEtypes_RsnParamSet_t *) pos;
        if (pbss_desc->pwpa_ie) {
            prsn_ie_tlv->header.type =
                (t_u16) (*(pbss_desc->pwpa_ie)).vend_hdr.element_id;
            prsn_ie_tlv->header.type = prsn_ie_tlv->header.type & 0x00FF;
            prsn_ie_tlv->header.type =
                wlan_cpu_to_le16(prsn_ie_tlv->header.type);
            prsn_ie_tlv->header.len =
                (t_u16) (*(pbss_desc->pwpa_ie)).vend_hdr.len;
            prsn_ie_tlv->header.len = prsn_ie_tlv->header.len & 0x00FF;
            if (prsn_ie_tlv->header.len <= (sizeof(pmpriv->wpa_ie))) {
                memcpy(pmadapter, prsn_ie_tlv->rsn_ie,
                       &((*(pbss_desc->pwpa_ie)).vend_hdr.oui[0]),
                       prsn_ie_tlv->header.len);
            } else {
                ret = MLAN_STATUS_FAILURE;
                goto done;
            }

            HEXDUMP("ADHOC_JOIN: RSN IE", (t_u8 *) prsn_ie_tlv,
                    sizeof(prsn_ie_tlv->header) + prsn_ie_tlv->header.len);
            pos += sizeof(prsn_ie_tlv->header) + prsn_ie_tlv->header.len;
            cmd_append_size +=
                sizeof(prsn_ie_tlv->header) + prsn_ie_tlv->header.len;
            prsn_ie_tlv->header.len = wlan_cpu_to_le16(prsn_ie_tlv->header.len);
        }
        if (pbss_desc->prsn_ie) {
            prsn_ie_tlv = (MrvlIEtypes_RsnParamSet_t *) pos;
            prsn_ie_tlv->header.type =
                (t_u16) (*(pbss_desc->prsn_ie)).ieee_hdr.element_id;
            prsn_ie_tlv->header.type = prsn_ie_tlv->header.type & 0x00FF;
            prsn_ie_tlv->header.type =
                wlan_cpu_to_le16(prsn_ie_tlv->header.type);
            prsn_ie_tlv->header.len =
                (t_u16) (*(pbss_desc->prsn_ie)).ieee_hdr.len;
            prsn_ie_tlv->header.len = prsn_ie_tlv->header.len & 0x00FF;
            if (prsn_ie_tlv->header.len <= (sizeof(pmpriv->wpa_ie))) {
                memcpy(pmadapter, prsn_ie_tlv->rsn_ie,
                       &((*(pbss_desc->prsn_ie)).data[0])
                       , prsn_ie_tlv->header.len);
            } else {
                ret = MLAN_STATUS_FAILURE;
                goto done;
            }

            HEXDUMP("ADHOC_JOIN: RSN IE", (t_u8 *) prsn_ie_tlv,
                    sizeof(prsn_ie_tlv->header) + prsn_ie_tlv->header.len);
            pos += sizeof(prsn_ie_tlv->header) + prsn_ie_tlv->header.len;
            cmd_append_size +=
                sizeof(prsn_ie_tlv->header) + prsn_ie_tlv->header.len;
            prsn_ie_tlv->header.len = wlan_cpu_to_le16(prsn_ie_tlv->header.len);
        }
    }

    if (ISSUPP_11NENABLED(pmadapter->fw_cap_info)
        )
        cmd_append_size += wlan_cmd_append_11n_tlv(pmpriv, pbss_desc, &pos);

    cmd->size =
        (t_u16) wlan_cpu_to_le16((t_u16) (sizeof(HostCmd_DS_802_11_AD_HOC_JOIN)
                                          + S_DS_GEN + cmd_append_size));

    memcpy(pmadapter, &tmp_cap, &padhoc_join->bss_descriptor.cap,
           sizeof(IEEEtypes_CapInfo_t));
    tmp_cap = wlan_cpu_to_le16(tmp_cap);

    memcpy(pmadapter, &padhoc_join->bss_descriptor.cap,
           &tmp_cap, sizeof(IEEEtypes_CapInfo_t));

  done:
    LEAVE();
    return ret;
}

/**
 *  @brief This function handles the command response of ad_hoc_start and
 *  		ad_hoc_join
 *
 *  @param pmpriv       A pointer to mlan_private structure
 *  @param resp         A pointer to HostCmd_DS_COMMAND
 *  @param pioctl_buf   A pointer to mlan_ioctl_req structure
 *
 *  @return             MLAN_STATUS_SUCCESS or MLAN_STATUS_FAILURE
 */
mlan_status
wlan_ret_802_11_ad_hoc(IN mlan_private * pmpriv,
                       IN HostCmd_DS_COMMAND * resp, IN t_void * pioctl_buf)
{
    mlan_status ret = MLAN_STATUS_SUCCESS;
    mlan_ioctl_req *pioctl_req = (mlan_ioctl_req *) pioctl_buf;
    HostCmd_DS_802_11_AD_HOC_START_RESULT *padhoc_start_result =
        &resp->params.adhoc_start_result;
    HostCmd_DS_802_11_AD_HOC_JOIN_RESULT *padhoc_join_result =
        &resp->params.adhoc_join_result;
    BSSDescriptor_t *pbss_desc;
    t_u16 command = resp->command;
    t_u8 result = 0;
    t_u8 event_buf[100];
    mlan_event *pevent = (mlan_event *) event_buf;

    ENTER();

    if (command == HostCmd_CMD_802_11_AD_HOC_START)
        result = padhoc_start_result->result;
    else
        result = padhoc_join_result->result;

    pbss_desc = pmpriv->pattempted_bss_desc;

    /*
     * Join result code 0 --> SUCCESS
     */
    if (result) {
        PRINTM(MERROR, "ADHOC_RESP Failed 0x%x\n", result);
        if (pmpriv->media_connected == MTRUE)
            wlan_reset_connect_state(pmpriv, MTRUE);
        if (pmpriv->adhoc_state == ADHOC_STARTING)
            pmpriv->adhoc_state = ADHOC_IDLE;

        memset(pmpriv->adapter, &pmpriv->curr_bss_params.bss_descriptor,
               0x00, sizeof(BSSDescriptor_t));

        ret = MLAN_STATUS_FAILURE;
        goto done;
    }

    /* Send a Media Connected event, according to the Spec */
    pmpriv->media_connected = MTRUE;

    if (command == HostCmd_CMD_802_11_AD_HOC_START) {
        PRINTM(MINFO, "ADHOC_S_RESP  %s\n", pbss_desc->ssid.ssid);

        /* Update the created network descriptor with the new BSSID */
        memcpy(pmpriv->adapter, pbss_desc->mac_address,
               padhoc_start_result->bssid, MLAN_MAC_ADDR_LENGTH);

        pmpriv->adhoc_state = ADHOC_STARTED;
        if (pmpriv->adapter->state_rdh.stage == RDH_RESTART_INTFS)
            wlan_11h_radar_detected_callback((t_void *) pmpriv);
    } else {
        /*
         * Now the join cmd should be successful.
         * If BSSID has changed use SSID to compare instead of BSSID
         */
        PRINTM(MINFO, "ADHOC_J_RESP  %s\n", pbss_desc->ssid.ssid);

        /*
         * Make a copy of current BSSID descriptor, only needed for join since
         * the current descriptor is already being used for adhoc start
         */
        memcpy(pmpriv->adapter, &pmpriv->curr_bss_params.bss_descriptor,
               pbss_desc, sizeof(BSSDescriptor_t));

        pmpriv->adhoc_state = ADHOC_JOINED;
    }

    PRINTM(MINFO, "ADHOC_RESP: Channel = %d\n", pmpriv->adhoc_channel);
    PRINTM(MINFO, "ADHOC_RESP: BSSID = %02x:%02x:%02x:%02x:%02x:%02x\n",
           pmpriv->curr_bss_params.bss_descriptor.mac_address[0],
           pmpriv->curr_bss_params.bss_descriptor.mac_address[1],
           pmpriv->curr_bss_params.bss_descriptor.mac_address[2],
           pmpriv->curr_bss_params.bss_descriptor.mac_address[3],
           pmpriv->curr_bss_params.bss_descriptor.mac_address[4],
           pmpriv->curr_bss_params.bss_descriptor.mac_address[5]);

    pevent->bss_index = pmpriv->bss_index;
    pevent->event_id = MLAN_EVENT_ID_DRV_CONNECTED;
    pevent->event_len = MLAN_MAC_ADDR_LENGTH;
    memcpy(pmpriv->adapter, (t_u8 *) pevent->event_buf,
           (t_u8 *) pmpriv->curr_bss_params.bss_descriptor.mac_address,
           MLAN_MAC_ADDR_LENGTH);
    wlan_recv_event(pmpriv, MLAN_EVENT_ID_DRV_CONNECTED, pevent);
    wlan_save_curr_bcn(pmpriv);

  done:
    /* Need to indicate IOCTL complete */
    if (pioctl_req != MNULL) {
        if (ret != MLAN_STATUS_SUCCESS) {
            pioctl_req->status_code = MLAN_ERROR_CMD_ASSOC_FAIL;
        } else {
            pioctl_req->status_code = MLAN_ERROR_NO_ERROR;
        }
    }

    LEAVE();
    return ret;
}
#endif /* CONFIG_MLAN_WMSDK */

/**
 *  @brief Associated to a specific BSS discovered in a scan
 *
 *  @param pmpriv       A pointer to mlan_private structure
 *  @param pioctl_buf   A pointer to MLAN IOCTL Request buffer
 *  @param pbss_desc     A pointer to the BSS descriptor to associate with.
 *
 *  @return             MLAN_STATUS_SUCCESS or MLAN_STATUS_FAILURE
 */
mlan_status
wlan_associate(IN mlan_private * pmpriv,
               IN t_void * pioctl_buf, IN BSSDescriptor_t * pbss_desc)
{
    mlan_status ret = MLAN_STATUS_SUCCESS;
    t_u8 current_bssid[MLAN_MAC_ADDR_LENGTH];
    pmlan_ioctl_req pioctl_req = (mlan_ioctl_req *) pioctl_buf;

    ENTER();

    /* Return error if the pmadapter or table entry is not marked as infra */
    if ((pmpriv->bss_mode != MLAN_BSS_MODE_INFRA) ||
        (pbss_desc->bss_mode != MLAN_BSS_MODE_INFRA)) {
        if (pioctl_req)
            pioctl_req->status_code = MLAN_ERROR_IOCTL_INVALID;
        LEAVE();
        return MLAN_STATUS_FAILURE;
    }

    memcpy(pmpriv->adapter, &current_bssid,
           &pmpriv->curr_bss_params.bss_descriptor.mac_address,
           sizeof(current_bssid));

    /* Clear any past association response stored for application retrieval */
    /* fixme: enable this later if assoc rsp buffer is needed */
    /* pmpriv->assoc_rsp_size = 0; */

    ret = wlan_prepare_cmd(pmpriv,
                           HostCmd_CMD_802_11_ASSOCIATE,
                           HostCmd_ACT_GEN_SET, 0, pioctl_buf, pbss_desc);

    LEAVE();
    return ret;
}

#ifndef CONFIG_MLAN_WMSDK
/**
 *  @brief Start an Adhoc Network
 *
 *  @param pmpriv       A pointer to mlan_private structure
 *  @param pioctl_buf   A pointer to MLAN IOCTL Request buffer
 *  @param padhoc_ssid  The ssid of the Adhoc Network
 *
 *  @return             MLAN_STATUS_SUCCESS--success, MLAN_STATUS_FAILURE--fail
 */
mlan_status
wlan_adhoc_start(IN mlan_private * pmpriv,
                 IN t_void * pioctl_buf, IN mlan_802_11_ssid * padhoc_ssid)
{
    mlan_status ret = MLAN_STATUS_SUCCESS;
    wlan_meas_state_t *pmeas_state = &pmpriv->adapter->state_meas;
    t_u8 radar = MFALSE;
    pmlan_ioctl_req pioctl_req = (mlan_ioctl_req *) pioctl_buf;

    ENTER();

    /*
     * If the report indicates no measurement was done, leave the default
     * return value alone.
     */
    if (!pmeas_state->meas_rpt_returned.rpt.basic.map.unmeasured) {
        radar =
            pmeas_state->meas_rpt_returned.rpt.basic.map.radar ? MTRUE : MFALSE;
    }

    if (radar) {
        if (pioctl_req)
            pioctl_req->status_code = MLAN_ERROR_IOCTL_INVALID;
        ret = MLAN_STATUS_FAILURE;
        LEAVE();
        return ret;
    }

    PRINTM(MINFO, "Adhoc Channel = %d\n", pmpriv->adhoc_channel);
    PRINTM(MINFO, "curr_bss_params.channel = %d\n",
           pmpriv->curr_bss_params.bss_descriptor.channel);
    PRINTM(MINFO, "curr_bss_params.band = %d\n", pmpriv->curr_bss_params.band);

    ret = wlan_prepare_cmd(pmpriv,
                           HostCmd_CMD_802_11_AD_HOC_START,
                           HostCmd_ACT_GEN_SET, 0, pioctl_buf, padhoc_ssid);
#if defined(STA_SUPPORT)
    if (ret == MLAN_STATUS_SUCCESS)
        memcpy(pmpriv->adapter, &pmpriv->adhoc_last_start_ssid,
               padhoc_ssid, sizeof(mlan_802_11_ssid));
#endif

    LEAVE();
    return ret;
}

/**
 *  @brief Join an adhoc network found in a previous scan
 *
 *  @param pmpriv       A pointer to mlan_private structure
 *  @param pioctl_buf   A pointer to MLAN IOCTL Request buffer
 *  @param pbss_desc     A pointer to the BSS descriptor found in a previous scan
 *                      to attempt to join
 *
 *  @return             MLAN_STATUS_SUCCESS--success, MLAN_STATUS_FAILURE--fail
 */
mlan_status
wlan_adhoc_join(IN mlan_private * pmpriv,
                IN t_void * pioctl_buf, IN BSSDescriptor_t * pbss_desc)
{
    pmlan_adapter pmadapter = pmpriv->adapter;
    mlan_status ret = MLAN_STATUS_SUCCESS;
    pmlan_ioctl_req pioctl_req = (mlan_ioctl_req *) pioctl_buf;

    ENTER();

    PRINTM(MINFO, "wlan_adhoc_join: CurBss.ssid =%s\n",
           pmpriv->curr_bss_params.bss_descriptor.ssid.ssid);
    PRINTM(MINFO, "wlan_adhoc_join: CurBss.ssid_len =%u\n",
           pmpriv->curr_bss_params.bss_descriptor.ssid.ssid_len);
    PRINTM(MINFO, "wlan_adhoc_join: ssid =%s\n", pbss_desc->ssid.ssid);
    PRINTM(MINFO, "wlan_adhoc_join: ssid len =%u\n", pbss_desc->ssid.ssid_len);

    /* Check if the requested SSID is already joined */
    if (pmpriv->curr_bss_params.bss_descriptor.ssid.ssid_len &&
        !wlan_ssid_cmp(pmadapter, &pbss_desc->ssid,
                       &pmpriv->curr_bss_params.bss_descriptor.ssid) &&
        (pmpriv->curr_bss_params.bss_descriptor.bss_mode ==
         MLAN_BSS_MODE_IBSS)) {

        PRINTM(MINFO,
               "ADHOC_J_CMD: New ad-hoc SSID is the same as current, "
               "not attempting to re-join\n");

        if (pioctl_req)
            pioctl_req->status_code = MLAN_ERROR_IOCTL_INVALID;
        LEAVE();
        return MLAN_STATUS_FAILURE;
    }

    PRINTM(MINFO, "curr_bss_params.channel = %d\n",
           pmpriv->curr_bss_params.bss_descriptor.channel);
    PRINTM(MINFO, "curr_bss_params.band = %d\n", pmpriv->curr_bss_params.band);

    ret = wlan_prepare_cmd(pmpriv,
                           HostCmd_CMD_802_11_AD_HOC_JOIN,
                           HostCmd_ACT_GEN_SET, 0, pioctl_buf, pbss_desc);

    LEAVE();
    return ret;
}

/**
 *  @brief Send Deauthentication Request or Stop the AdHoc network depending on mode
 *
 *  @param pmpriv    A pointer to mlan_private structure
 *  @param pioctl_req A pointer to mlan_ioctl_req structure
 *  @param mac       A pointer to mlan_802_11_mac_addr structure
 *
 *  @return          MLAN_STATUS_SUCCESS--success, MLAN_STATUS_FAILURE--fail, MLAN_STATUS_PENDING--pending
 */
mlan_status
wlan_disconnect(IN mlan_private * pmpriv,
                IN mlan_ioctl_req * pioctl_req, IN mlan_802_11_mac_addr * mac)
{
    mlan_802_11_mac_addr mac_address;
    mlan_status ret = MLAN_STATUS_SUCCESS;
    t_u8 zero_mac[] = { 0, 0, 0, 0, 0, 0 };

    ENTER();

    if (pmpriv->media_connected == MTRUE) {
        if (pmpriv->bss_mode == MLAN_BSS_MODE_INFRA) {
            if (mac) {
                if (!memcmp(pmpriv->adapter, mac, zero_mac, sizeof(zero_mac)))
                    memcpy(pmpriv->adapter, (t_u8 *) & mac_address,
                           (t_u8 *) & pmpriv->curr_bss_params.bss_descriptor.
                           mac_address, MLAN_MAC_ADDR_LENGTH);
                else {
                    memcpy(pmpriv->adapter, (t_u8 *) & mac_address,
                           (t_u8 *) mac, MLAN_MAC_ADDR_LENGTH);
                }
            } else {
                memcpy(pmpriv->adapter, (t_u8 *) & mac_address,
                       (t_u8 *) & pmpriv->curr_bss_params.bss_descriptor.
                       mac_address, MLAN_MAC_ADDR_LENGTH);
            }

            ret = wlan_prepare_cmd(pmpriv,
                                   HostCmd_CMD_802_11_DEAUTHENTICATE,
                                   HostCmd_ACT_GEN_SET,
                                   0, (t_void *) pioctl_req, &mac_address);

            if (ret == MLAN_STATUS_SUCCESS && pioctl_req)
                ret = MLAN_STATUS_PENDING;

        } else if (pmpriv->bss_mode == MLAN_BSS_MODE_IBSS) {
            ret = wlan_prepare_cmd(pmpriv,
                                   HostCmd_CMD_802_11_AD_HOC_STOP,
                                   HostCmd_ACT_GEN_SET,
                                   0, (t_void *) pioctl_req, MNULL);

            if (ret == MLAN_STATUS_SUCCESS && pioctl_req)
                ret = MLAN_STATUS_PENDING;
        }
    }

    LEAVE();
    return ret;
}
#endif /* CONFIG_MLAN_WMSDK */

/**
 *  @brief Convert band to radio type used in channel TLV
 *
 *  @param band		Band enumeration to convert to a channel TLV radio type
 *
 *  @return		Radio type designator for use in a channel TLV
 */
t_u8
wlan_band_to_radio_type(IN t_u8 band)
{
    t_u8 ret_radio_type;

    ENTER();

    switch (band) {
    case BAND_A:
    case BAND_AN:
    case BAND_A | BAND_AN:
        ret_radio_type = HostCmd_SCAN_RADIO_TYPE_A;
        break;
    case BAND_B:
    case BAND_G:
    case BAND_B | BAND_G:
    default:
        ret_radio_type = HostCmd_SCAN_RADIO_TYPE_BG;
        break;
    }

    LEAVE();
    return ret_radio_type;
}
