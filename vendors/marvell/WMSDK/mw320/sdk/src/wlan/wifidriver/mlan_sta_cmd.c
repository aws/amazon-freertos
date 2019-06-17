/** @file mlan_sta_cmd.c
 *
 *  @brief This file provides the handling of command. It prepares command and sends it to firmware when it is ready
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
/********************************************************
                Local Variables
********************************************************/

/********************************************************
                Global Variables
********************************************************/

/********************************************************
                Local Functions
********************************************************/

/**
 *  @brief This function prepares command of RSSI info.
 *
 *  @param pmpriv       A pointer to mlan_private structure
 *  @param pcmd         A pointer to HostCmd_DS_COMMAND structure
 *  @param cmd_action   Command action
 *
 *  @return             MLAN_STATUS_SUCCESS
 */
static mlan_status
wlan_cmd_802_11_rssi_info(IN pmlan_private pmpriv,
                          IN HostCmd_DS_COMMAND * pcmd, IN t_u16 cmd_action)
{
    ENTER();

    pcmd->command = wlan_cpu_to_le16(HostCmd_CMD_RSSI_INFO);
    pcmd->size =
        wlan_cpu_to_le16(sizeof(HostCmd_DS_802_11_RSSI_INFO) + S_DS_GEN);
    pcmd->params.rssi_info.action = wlan_cpu_to_le16(cmd_action);
    pcmd->params.rssi_info.ndata = wlan_cpu_to_le16(pmpriv->data_avg_factor);
    pcmd->params.rssi_info.nbcn = wlan_cpu_to_le16(pmpriv->bcn_avg_factor);

    /* Reset SNR/NF/RSSI values in private structure */
    pmpriv->data_rssi_last = 0;
    pmpriv->data_nf_last = 0;
    pmpriv->data_rssi_avg = 0;
    pmpriv->data_nf_avg = 0;
    pmpriv->bcn_rssi_last = 0;
    pmpriv->bcn_nf_last = 0;
    pmpriv->bcn_rssi_avg = 0;
    pmpriv->bcn_nf_avg = 0;

    LEAVE();
    return MLAN_STATUS_SUCCESS;
}

/**
 *  @brief This function prepares command of mac_control.
 *
 *  @param pmpriv       A pointer to mlan_private structure
 *  @param pcmd         A pointer to HostCmd_DS_COMMAND structure
 *  @param cmd_action   Command action
 *  @param pdata_buf    A pointer to command information buffer
 *
 *  @return             MLAN_STATUS_SUCCESS or MLAN_STATUS_FAILURE
 */
static mlan_status
wlan_cmd_mac_control(IN pmlan_private pmpriv,
                     IN HostCmd_DS_COMMAND * pcmd,
                     IN t_u16 cmd_action, IN t_void * pdata_buf)
{
    HostCmd_DS_MAC_CONTROL *pmac = &pcmd->params.mac_ctrl;
    t_u16 action = *((t_u16 *) pdata_buf);

    ENTER();

    if (cmd_action != HostCmd_ACT_GEN_SET) {
        PRINTM(MERROR, "wlan_cmd_mac_control(): support SET only.\n");
        LEAVE();
        return MLAN_STATUS_FAILURE;
    }

    pcmd->command = wlan_cpu_to_le16(HostCmd_CMD_MAC_CONTROL);
    pcmd->size = wlan_cpu_to_le16(sizeof(HostCmd_DS_MAC_CONTROL) + S_DS_GEN);
    pmac->action = wlan_cpu_to_le16(action);

    LEAVE();
    return MLAN_STATUS_SUCCESS;
}

/**
 *  @brief This function prepares command of snmp_mib.
 *
 *  @param pmpriv       A pointer to mlan_private structure
 *  @param cmd          A pointer to HostCmd_DS_COMMAND structure
 *  @param cmd_action   The action: GET or SET
 *  @param cmd_oid      OID: ENABLE or DISABLE
 *  @param pdata_buf    A pointer to command information buffer
 *
 *  @return             MLAN_STATUS_SUCCESS
 */
static mlan_status
wlan_cmd_802_11_snmp_mib(IN pmlan_private pmpriv,
                         IN HostCmd_DS_COMMAND * cmd,
                         IN t_u16 cmd_action,
                         IN t_u32 cmd_oid, IN t_void * pdata_buf)
{
    HostCmd_DS_802_11_SNMP_MIB *psnmp_mib = &cmd->params.smib;
    t_u32 ul_temp;

    ENTER();
    PRINTM(MINFO, "SNMP_CMD: cmd_oid = 0x%x\n", cmd_oid);
    cmd->command = wlan_cpu_to_le16(HostCmd_CMD_802_11_SNMP_MIB);
    cmd->size = sizeof(HostCmd_DS_802_11_SNMP_MIB) - 1 + S_DS_GEN;

    if (cmd_action == HostCmd_ACT_GEN_GET) {
        psnmp_mib->query_type = wlan_cpu_to_le16(HostCmd_ACT_GEN_GET);
        psnmp_mib->buf_size = wlan_cpu_to_le16(MAX_SNMP_BUF_SIZE);
        cmd->size += MAX_SNMP_BUF_SIZE;
    }

    switch (cmd_oid) {
#ifndef CONFIG_MLAN_WMSDK
    case DtimPeriod_i:
        psnmp_mib->oid = wlan_cpu_to_le16((t_u16) DtimPeriod_i);
        if (cmd_action == HostCmd_ACT_GEN_SET) {
            psnmp_mib->query_type = wlan_cpu_to_le16(HostCmd_ACT_GEN_SET);
            psnmp_mib->buf_size = wlan_cpu_to_le16(sizeof(t_u8));
            ul_temp = *((t_u32 *) pdata_buf);
            psnmp_mib->value[0] = (t_u8) ul_temp;
            cmd->size += sizeof(t_u8);
        }
        break;
    case FragThresh_i:
        psnmp_mib->oid = wlan_cpu_to_le16((t_u16) FragThresh_i);
        if (cmd_action == HostCmd_ACT_GEN_SET) {
            psnmp_mib->query_type = wlan_cpu_to_le16(HostCmd_ACT_GEN_SET);
            psnmp_mib->buf_size = wlan_cpu_to_le16(sizeof(t_u16));
            ul_temp = *((t_u32 *) pdata_buf);
            *((t_u16 *) (psnmp_mib->value)) = wlan_cpu_to_le16((t_u16) ul_temp);
            cmd->size += sizeof(t_u16);
        }
        break;
    case RtsThresh_i:
        psnmp_mib->oid = wlan_cpu_to_le16((t_u16) RtsThresh_i);
        if (cmd_action == HostCmd_ACT_GEN_SET) {
            psnmp_mib->query_type = wlan_cpu_to_le16(HostCmd_ACT_GEN_SET);
            psnmp_mib->buf_size = wlan_cpu_to_le16(sizeof(t_u16));
            ul_temp = *((t_u32 *) pdata_buf);
            *(t_u16 *) (psnmp_mib->value) = wlan_cpu_to_le16((t_u16) ul_temp);
            cmd->size += sizeof(t_u16);
        }
        break;

    case ShortRetryLim_i:
        psnmp_mib->oid = wlan_cpu_to_le16((t_u16) ShortRetryLim_i);
        if (cmd_action == HostCmd_ACT_GEN_SET) {
            psnmp_mib->query_type = wlan_cpu_to_le16(HostCmd_ACT_GEN_SET);
            psnmp_mib->buf_size = wlan_cpu_to_le16(sizeof(t_u16));
            ul_temp = (*(t_u32 *) pdata_buf);
            *((t_u16 *) (psnmp_mib->value)) = wlan_cpu_to_le16((t_u16) ul_temp);
            cmd->size += sizeof(t_u16);
        }
        break;
#endif /* CONFIG_MLAN_WMSDK */
    case Dot11D_i:
        psnmp_mib->oid = wlan_cpu_to_le16((t_u16) Dot11D_i);
        if (cmd_action == HostCmd_ACT_GEN_SET) {
            psnmp_mib->query_type = wlan_cpu_to_le16(HostCmd_ACT_GEN_SET);
            psnmp_mib->buf_size = wlan_cpu_to_le16(sizeof(t_u16));
            ul_temp = *(t_u32 *) pdata_buf;
            *((t_u16 *) (psnmp_mib->value)) = wlan_cpu_to_le16((t_u16) ul_temp);
            cmd->size += sizeof(t_u16);
        }
        break;
    case Dot11H_i:
        psnmp_mib->oid = wlan_cpu_to_le16((t_u16) Dot11H_i);
        if (cmd_action == HostCmd_ACT_GEN_SET) {
            psnmp_mib->query_type = wlan_cpu_to_le16(HostCmd_ACT_GEN_SET);
            psnmp_mib->buf_size = wlan_cpu_to_le16(sizeof(t_u16));
            ul_temp = *(t_u32 *) pdata_buf;
            *((t_u16 *) (psnmp_mib->value)) = wlan_cpu_to_le16((t_u16) ul_temp);
            cmd->size += sizeof(t_u16);
        }
        break;
#ifndef CONFIG_MLAN_WMSDK
    case WwsMode_i:
        psnmp_mib->oid = wlan_cpu_to_le16((t_u16) WwsMode_i);
        if (cmd_action == HostCmd_ACT_GEN_SET) {
            psnmp_mib->query_type = wlan_cpu_to_le16(HostCmd_ACT_GEN_SET);
            psnmp_mib->buf_size = wlan_cpu_to_le16(sizeof(t_u16));
            ul_temp = *((t_u32 *) pdata_buf);
            *((t_u16 *) (psnmp_mib->value)) = wlan_cpu_to_le16((t_u16) ul_temp);
            cmd->size += sizeof(t_u16);
        }
        break;
    case Thermal_i:
        psnmp_mib->oid = wlan_cpu_to_le16((t_u16) Thermal_i);
        break;
#endif /* CONFIG_MLAN_WMSDK */
    default:
        break;
    }
    cmd->size = wlan_cpu_to_le16(cmd->size);
    PRINTM(MINFO, "SNMP_CMD: Action=0x%x, OID=0x%x, OIDSize=0x%x, Value=0x%x\n",
           cmd_action, cmd_oid, wlan_le16_to_cpu(psnmp_mib->buf_size),
           wlan_le16_to_cpu(*(t_u16 *) psnmp_mib->value));
    LEAVE();
    return MLAN_STATUS_SUCCESS;
}

/**
 *  @brief This function prepares command of get_log.
 *
 *  @param pmpriv       A pointer to mlan_private structure
 *  @param cmd          A pointer to HostCmd_DS_COMMAND structure
 *
 *  @return             MLAN_STATUS_SUCCESS
 */
static mlan_status
wlan_cmd_802_11_get_log(IN pmlan_private pmpriv, IN HostCmd_DS_COMMAND * cmd)
{
    ENTER();
    cmd->command = wlan_cpu_to_le16(HostCmd_CMD_802_11_GET_LOG);
    cmd->size = wlan_cpu_to_le16(sizeof(HostCmd_DS_802_11_GET_LOG) + S_DS_GEN);
    LEAVE();
    return MLAN_STATUS_SUCCESS;
}

/**
 *  @brief This function prepares command of tx_power_cfg.
 *
 *  @param pmpriv      A pointer to mlan_private structure
 *  @param cmd         A pointer to HostCmd_DS_COMMAND structure
 *  @param cmd_action  The action: GET or SET
 *  @param pdata_buf   A pointer to data buffer
 *
 *  @return            MLAN_STATUS_SUCCESS
 */
static mlan_status
wlan_cmd_tx_power_cfg(IN pmlan_private pmpriv,
                      IN HostCmd_DS_COMMAND * cmd,
                      IN t_u16 cmd_action, IN t_void * pdata_buf)
{
    MrvlTypes_Power_Group_t *ppg_tlv = MNULL;
    HostCmd_DS_TXPWR_CFG *ptxp = MNULL;
    HostCmd_DS_TXPWR_CFG *ptxp_cfg = &cmd->params.txp_cfg;

    ENTER();

    cmd->command = wlan_cpu_to_le16(HostCmd_CMD_TXPWR_CFG);
    cmd->size = wlan_cpu_to_le16(S_DS_GEN + sizeof(HostCmd_DS_TXPWR_CFG));
    switch (cmd_action) {
    case HostCmd_ACT_GEN_SET:
        ptxp = (HostCmd_DS_TXPWR_CFG *) pdata_buf;
        if (ptxp->mode) {
            ppg_tlv =
                (MrvlTypes_Power_Group_t *) ((t_u8 *) pdata_buf +
                                             sizeof(HostCmd_DS_TXPWR_CFG));
            memmove(pmpriv->adapter, ptxp_cfg, pdata_buf,
                    sizeof(HostCmd_DS_TXPWR_CFG) +
                    sizeof(MrvlTypes_Power_Group_t) + ppg_tlv->length);

            ppg_tlv = (MrvlTypes_Power_Group_t *) ((t_u8 *) ptxp_cfg +
                                                   sizeof
                                                   (HostCmd_DS_TXPWR_CFG));
            cmd->size +=
                wlan_cpu_to_le16(sizeof(MrvlTypes_Power_Group_t) +
                                 ppg_tlv->length);
            ppg_tlv->type = wlan_cpu_to_le16(ppg_tlv->type);
            ppg_tlv->length = wlan_cpu_to_le16(ppg_tlv->length);
        } else {
            memmove(pmpriv->adapter, ptxp_cfg, pdata_buf,
                    sizeof(HostCmd_DS_TXPWR_CFG));
        }
        ptxp_cfg->action = wlan_cpu_to_le16(cmd_action);
        ptxp_cfg->cfg_index = wlan_cpu_to_le16(ptxp_cfg->cfg_index);
        ptxp_cfg->mode = wlan_cpu_to_le32(ptxp_cfg->mode);
        break;
    case HostCmd_ACT_GEN_GET:
        ptxp_cfg->action = wlan_cpu_to_le16(cmd_action);
        break;
    }

    LEAVE();
    return MLAN_STATUS_SUCCESS;
}

/**
 *  @brief This function prepares command of rf_tx_power.
 *
 *  @param pmpriv     A pointer to wlan_private structure
 *  @param cmd        A pointer to HostCmd_DS_COMMAND structure
 *  @param cmd_action the action: GET or SET
 *  @param pdata_buf  A pointer to data buffer
 *  @return           MLAN_STATUS_SUCCESS
 */
static mlan_status
wlan_cmd_802_11_rf_tx_power(IN pmlan_private pmpriv,
                            IN HostCmd_DS_COMMAND * cmd,
                            IN t_u16 cmd_action, IN t_void * pdata_buf)
{

    HostCmd_DS_802_11_RF_TX_POWER *prtp = &cmd->params.txp;

    ENTER();

    cmd->size = wlan_cpu_to_le16((sizeof(HostCmd_DS_802_11_RF_TX_POWER))
                                 + S_DS_GEN);
    cmd->command = wlan_cpu_to_le16(HostCmd_CMD_802_11_RF_TX_POWER);
    prtp->action = cmd_action;

    PRINTM(MINFO, "RF_TX_POWER_CMD: Size:%d Cmd:0x%x Act:%d\n",
           cmd->size, cmd->command, prtp->action);

    switch (cmd_action) {
    case HostCmd_ACT_GEN_GET:
        prtp->action = wlan_cpu_to_le16(HostCmd_ACT_GEN_GET);
        prtp->current_level = 0;
        break;

    case HostCmd_ACT_GEN_SET:
        prtp->action = wlan_cpu_to_le16(HostCmd_ACT_GEN_SET);
        prtp->current_level = wlan_cpu_to_le16(*((t_u16 *) pdata_buf));
        break;
    }
    LEAVE();
    return MLAN_STATUS_SUCCESS;
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
wlan_cmd_802_11_hs_cfg(IN pmlan_private pmpriv,
                       IN HostCmd_DS_COMMAND * cmd,
                       IN t_u16 cmd_action, IN hs_config_param * pdata_buf)
{
#ifndef CONFIG_MLAN_WMSDK
    pmlan_adapter pmadapter = pmpriv->adapter;
#endif /* CONFIG_MLAN_WMSDK*/
    HostCmd_DS_802_11_HS_CFG_ENH *phs_cfg = &cmd->params.opt_hs_cfg;
    t_u16 hs_activate = MFALSE;

    ENTER();
    if (pdata_buf == MNULL) {
        /* New Activate command */
        hs_activate = MTRUE;
    }
    cmd->command = wlan_cpu_to_le16(HostCmd_CMD_802_11_HS_CFG_ENH);
#ifndef CONFIG_MLAN_WMSDK
    if (!hs_activate && (pdata_buf->conditions != HOST_SLEEP_CFG_CANCEL)
        && ((pmadapter->arp_filter_size > 0)
            && (pmadapter->arp_filter_size <= ARP_FILTER_MAX_BUF_SIZE))) {
        PRINTM(MINFO, "Attach %d bytes ArpFilter to HSCfg cmd\n",
               pmadapter->arp_filter_size);
        memcpy(pmpriv->adapter,
               ((t_u8 *) phs_cfg) + sizeof(HostCmd_DS_802_11_HS_CFG_ENH),
               pmadapter->arp_filter, pmadapter->arp_filter_size);
        cmd->size =
            (t_u16) wlan_cpu_to_le16(pmadapter->arp_filter_size +
                                     sizeof(HostCmd_DS_802_11_HS_CFG_ENH) +
                                     S_DS_GEN);
    } else
#endif /* CONFIG_MLAN_WMSDK*/

        cmd->size =
            wlan_cpu_to_le16(S_DS_GEN + sizeof(HostCmd_DS_802_11_HS_CFG_ENH));

    if (hs_activate) {
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
 *  @brief This function prepares command of mac_address.
 *
 *  @param pmpriv       A pointer to mlan_private structure
 *  @param cmd          A pointer to HostCmd_DS_COMMAND structure
 *  @param cmd_action   The action: GET or SET
 *
 *  @return             MLAN_STATUS_SUCCESS
 */
static mlan_status
wlan_cmd_802_11_mac_address(IN pmlan_private pmpriv,
                            IN HostCmd_DS_COMMAND * cmd, IN t_u16 cmd_action,
                            t_void * pdata_buf)
{
    ENTER();
    cmd->command = wlan_cpu_to_le16(HostCmd_CMD_802_11_MAC_ADDRESS);
    cmd->size = wlan_cpu_to_le16(sizeof(HostCmd_DS_802_11_MAC_ADDRESS) +
                                 S_DS_GEN);
    cmd->result = 0;

    cmd->params.mac_addr.action = wlan_cpu_to_le16(cmd_action);

    if (cmd_action == HostCmd_ACT_GEN_SET) {
        memcpy(pmpriv->adapter, cmd->params.mac_addr.mac_addr,
               pdata_buf, MLAN_MAC_ADDR_LENGTH);
        // HEXDUMP("SET_CMD: MAC ADDRESS-", priv->CurrentAddr, 6);
    }
    LEAVE();
    return MLAN_STATUS_SUCCESS;
}

#ifndef CONFIG_MLAN_WMSDK
/**
 * @brief This function prepares command of sleep_period.
 *
 * @param pmpriv       A pointer to mlan_private structure
 * @param cmd          A pointer to HostCmd_DS_COMMAND structure
 * @param cmd_action   The action: GET or SET
 * @param pdata_buf    A pointer to data buffer
 *
 * @return             MLAN_STATUS_SUCCESS
 */
static mlan_status
wlan_cmd_802_11_sleep_period(IN pmlan_private pmpriv,
                             IN HostCmd_DS_COMMAND * cmd,
                             IN t_u16 cmd_action, IN t_u16 * pdata_buf)
{
    HostCmd_DS_802_11_SLEEP_PERIOD *pcmd_sleep_pd = &cmd->params.sleep_pd;

    ENTER();

    cmd->command = wlan_cpu_to_le16(HostCmd_CMD_802_11_SLEEP_PERIOD);
    cmd->size =
        wlan_cpu_to_le16(sizeof(HostCmd_DS_802_11_SLEEP_PERIOD) + S_DS_GEN);
    if (cmd_action == HostCmd_ACT_GEN_SET) {
        pcmd_sleep_pd->sleep_pd = wlan_cpu_to_le16(*(t_u16 *) pdata_buf);
    }
    pcmd_sleep_pd->action = wlan_cpu_to_le16(cmd_action);

    LEAVE();
    return MLAN_STATUS_SUCCESS;
}

/**
 * @brief This function prepares command of sleep_params.
 *
 * @param pmpriv       A pointer to mlan_private structure
 * @param cmd          A pointer to HostCmd_DS_COMMAND structure
 * @param cmd_action   The action: GET or SET
 * @param pdata_buf    A pointer to data buffer
 *
 * @return             MLAN_STATUS_SUCCESS
 */
static mlan_status
wlan_cmd_802_11_sleep_params(IN pmlan_private pmpriv,
                             IN HostCmd_DS_COMMAND * cmd,
                             IN t_u16 cmd_action, IN t_u16 * pdata_buf)
{
    HostCmd_DS_802_11_SLEEP_PARAMS *pcmd_sp = &cmd->params.sleep_param;
    mlan_ds_sleep_params *psp = (mlan_ds_sleep_params *) pdata_buf;

    ENTER();

    cmd->command = wlan_cpu_to_le16(HostCmd_CMD_802_11_SLEEP_PARAMS);
    cmd->size =
        wlan_cpu_to_le16(sizeof(HostCmd_DS_802_11_SLEEP_PARAMS) + S_DS_GEN);
    if (cmd_action == HostCmd_ACT_GEN_SET) {
        pcmd_sp->reserved = (t_u16) psp->reserved;
        pcmd_sp->error = (t_u16) psp->error;
        pcmd_sp->offset = (t_u16) psp->offset;
        pcmd_sp->stable_time = (t_u16) psp->stable_time;
        pcmd_sp->cal_control = (t_u8) psp->cal_control;
        pcmd_sp->external_sleep_clk = (t_u8) psp->ext_sleep_clk;

        pcmd_sp->reserved = wlan_cpu_to_le16(pcmd_sp->reserved);
        pcmd_sp->error = wlan_cpu_to_le16(pcmd_sp->error);
        pcmd_sp->offset = wlan_cpu_to_le16(pcmd_sp->offset);
        pcmd_sp->stable_time = wlan_cpu_to_le16(pcmd_sp->stable_time);
    }
    pcmd_sp->action = wlan_cpu_to_le16(cmd_action);

    LEAVE();
    return MLAN_STATUS_SUCCESS;
}
#endif /* CONFIG_MLAN_WMSDK*/
/**
 *  @brief This function prepares command of mac_multicast_adr.
 *
 *  @param pmpriv       A pointer to mlan_private structure
 *  @param cmd          A pointer to HostCmd_DS_COMMAND structure
 *  @param cmd_action   The action: GET or SET
 *  @param pdata_buf    A pointer to data buffer
 *
 *  @return             MLAN_STATUS_SUCCESS
 */
static mlan_status
wlan_cmd_mac_multicast_adr(IN pmlan_private pmpriv,
                           IN HostCmd_DS_COMMAND * cmd,
                           IN t_u16 cmd_action, IN t_void * pdata_buf)
{
    mlan_multicast_list *pmcast_list = (mlan_multicast_list *) pdata_buf;
    HostCmd_DS_MAC_MULTICAST_ADR *pmc_addr = &cmd->params.mc_addr;

    ENTER();
    cmd->size =
        wlan_cpu_to_le16(sizeof(HostCmd_DS_MAC_MULTICAST_ADR) + S_DS_GEN);
    cmd->command = wlan_cpu_to_le16(HostCmd_CMD_MAC_MULTICAST_ADR);

    pmc_addr->action = wlan_cpu_to_le16(cmd_action);
    pmc_addr->num_of_adrs =
        wlan_cpu_to_le16((t_u16) pmcast_list->num_multicast_addr);
    memcpy(pmpriv->adapter, pmc_addr->mac_list, pmcast_list->mac_list,
           pmcast_list->num_multicast_addr * MLAN_MAC_ADDR_LENGTH);

    LEAVE();
    return MLAN_STATUS_SUCCESS;
}


/**
 *  @brief This function prepares command of deauthenticate.
 *
 * @param pmpriv       A pointer to mlan_private structure
 * @param cmd          A pointer to HostCmd_DS_COMMAND structure
 * @param pdata_buf    A pointer to data buffer
 *
 * @return             MLAN_STATUS_SUCCESS
 */
/* static */ mlan_status
wlan_cmd_802_11_deauthenticate(IN pmlan_private pmpriv,
                               IN HostCmd_DS_COMMAND * cmd,
                               IN t_void * pdata_buf)
{
    HostCmd_DS_802_11_DEAUTHENTICATE *pdeauth = &cmd->params.deauth;

    ENTER();

    cmd->command = wlan_cpu_to_le16(HostCmd_CMD_802_11_DEAUTHENTICATE);
    cmd->size =
        wlan_cpu_to_le16(sizeof(HostCmd_DS_802_11_DEAUTHENTICATE) + S_DS_GEN);

    /* Set AP MAC address */
    memcpy(pmpriv->adapter, pdeauth->mac_addr, (t_u8 *) pdata_buf,
           MLAN_MAC_ADDR_LENGTH);

    PRINTM(MCMND, "Deauth: %02x:%02x:%02x:%02x:%02x:%02x\n",
           pdeauth->mac_addr[0], pdeauth->mac_addr[1], pdeauth->mac_addr[2],
           pdeauth->mac_addr[3], pdeauth->mac_addr[4], pdeauth->mac_addr[5]);

    if (pmpriv->adapter->state_11h.recvd_chanswann_event) {
/** Reason code 36 = Requested from peer station as it is leaving the BSS */
#define REASON_CODE_PEER_STA_LEAVING  36
        pdeauth->reason_code = wlan_cpu_to_le16(REASON_CODE_PEER_STA_LEAVING);
    } else {
/** Reason code 3 = Station is leaving */
#define REASON_CODE_STA_LEAVING 3
        pdeauth->reason_code = wlan_cpu_to_le16(REASON_CODE_STA_LEAVING);
    }

    LEAVE();
    return MLAN_STATUS_SUCCESS;
}

#ifndef CONFIG_MLAN_WMSDK
/**
 *  @brief This function prepares command of ad_hoc_stop.
 *
 *  @param pmpriv       A pointer to mlan_private structure
 *  @param cmd          A pointer to HostCmd_DS_COMMAND structure
 *
 *  @return             MLAN_STATUS_SUCCESS
 */
static mlan_status
wlan_cmd_802_11_ad_hoc_stop(IN pmlan_private pmpriv,
                            IN HostCmd_DS_COMMAND * cmd)
{
    ENTER();

    cmd->command = wlan_cpu_to_le16(HostCmd_CMD_802_11_AD_HOC_STOP);
    cmd->size = wlan_cpu_to_le16(S_DS_GEN);

    if (wlan_11h_is_active(pmpriv))
        wlan_11h_activate(pmpriv, MNULL, MFALSE);

    LEAVE();
    return MLAN_STATUS_SUCCESS;
}
#endif /* CONFIG_MLAN_WMSDK */

#ifdef WPA
#ifdef KEY_MATERIAL_WEP
#ifndef KEY_PARAM_SET_V2
/**
 *  @brief This function sets WEP key(s) to key_param_set TLV(s).
 *
 *  @param priv           	A pointer to mlan_private structure
 *  @param key_param_set    A pointer to MrvlIEtype_KeyParamSet_t structure
 *  @param key_param_len    A pointer to the length variable
 *
 *  @return                 MLAN_STATUS_SUCCESS or MLAN_STATUS_FAILURE
 */
	static mlan_status
wlan_set_keyparamset_wep(mlan_private * priv,
		MrvlIEtype_KeyParamSet_t * key_param_set,
		t_u16 * key_param_len)
{
	int cur_key_param_len = 0;
	t_u8 i;
	mlan_status ret = MLAN_STATUS_SUCCESS;

	ENTER();

	/* Multi-key_param_set TLV is supported */
	for (i = 0; i < MRVL_NUM_WEP_KEY; i++) {
		if ((priv->wep_key[i].key_length == WEP_40_BIT_LEN) ||
			(priv->wep_key[i].key_length == WEP_104_BIT_LEN)) {
			key_param_set->type =
				wlan_cpu_to_le16(TLV_TYPE_KEY_MATERIAL);
			/** Key_param_set WEP fixed length */
#define KEYPARAMSET_WEP_FIXED_LEN 8
			key_param_set->length =
				wlan_cpu_to_le16((t_u16)
						(priv->wep_key[i].key_length +
						 KEYPARAMSET_WEP_FIXED_LEN));
			key_param_set->key_type_id =
				wlan_cpu_to_le16(KEY_TYPE_ID_WEP);
			key_param_set->key_info = wlan_cpu_to_le16
				(KEY_INFO_WEP_ENABLED | KEY_INFO_WEP_UNICAST |
				 KEY_INFO_WEP_MCAST);
			key_param_set->key_len =
				(t_u16) wlan_cpu_to_le16(
				priv->wep_key[i].key_length);
			/* Set WEP key index */
			key_param_set->key[0] = i;
			/* Set default Tx key flag */
			if (i == (priv->wep_key_curr_index &
					HostCmd_WEP_KEY_INDEX_MASK))
				key_param_set->key[1] = 1;
			else
				key_param_set->key[1] = 0;
			memmove(priv->adapter, &key_param_set->key[2],
					priv->wep_key[i].key_material,
					priv->wep_key[i].key_length);

			cur_key_param_len = priv->wep_key[i].key_length +
				KEYPARAMSET_WEP_FIXED_LEN +
				sizeof(MrvlIEtypesHeader_t);
			*key_param_len += (t_u16) cur_key_param_len;
			key_param_set =
				(MrvlIEtype_KeyParamSet_t *) (
						(t_u8 *) key_param_set +
						cur_key_param_len);
		} else if (!priv->wep_key[i].key_length) {
			continue;
		} else {
			PRINTM(MERROR,
				"key%d Length = %d is incorrect\n", (i + 1),
					priv->wep_key[i].key_length);
			ret = MLAN_STATUS_FAILURE;
			goto done;
		}
	}

done:
	LEAVE();
	return ret;
}
#endif /* KEY_PARAM_SET_V2 */
#else
/**
 *  @brief This function prepares command of set_wep.
 *
 *  @param priv     A pointer to mlan_private structure
 *  @param cmd      A pointer to HostCmd_DS_COMMAND structure
 *  @param cmd_action   HostCmd_ACT_ADD or HostCmd_ACT_REMOVE
 *  @return         MLAN_STATUS_SUCCESS or MLAN_STATUS_FAILURE
 */
static mlan_status wlan_cmd_802_11_set_wep(mlan_private *priv,
		HostCmd_DS_COMMAND *cmd,
		t_u16 cmd_action)
{
	HostCmd_DS_802_11_SET_WEP   *wep = &cmd->params.wep;
	mlan_status             ret = MLAN_STATUS_SUCCESS;

	ENTER();

	if (cmd_action == HostCmd_ACT_ADD) {
		cmd->command = wlan_cpu_to_le16(HostCmd_CMD_802_11_SET_WEP);
		cmd->size = wlan_cpu_to_le16((sizeof(HostCmd_DS_802_11_SET_WEP))
						+ S_DS_GEN);
		wep->action = wlan_cpu_to_le16(HostCmd_ACT_ADD);

		/* default tx key index */
		wep->key_index = wlan_cpu_to_le16(priv->wep_key_curr_index &
				HostCmd_WEP_KEY_INDEX_MASK);

		PRINTM(MINFO, "Tx Key Index: %u\n", wep->key_index);
		switch (priv->wep_key[0].key_length) {
			case WEP_40_BIT_LEN:
				wep->wep_type_for_key1 =
					HostCmd_TYPE_WEP_40_BIT;
				memmove(priv->adapter, wep->wep1,
					priv->wep_key[0].key_material,
					priv->wep_key[0].key_length);
				break;
			case WEP_104_BIT_LEN:
				wep->wep_type_for_key1 =
					HostCmd_TYPE_WEP_104_BIT;
				memmove(priv->adapter, wep->wep1,
					priv->wep_key[0].key_material,
					priv->wep_key[0].key_length);
				break;
			case 0:
				break;
			default:
				PRINTM(MWARN, "Key1 Length = %d is incorrect\n",
					priv->wep_key[0].key_length);
				ret = MLAN_STATUS_FAILURE;
				goto done;
		}

		switch (priv->wep_key[1].key_length) {
			case WEP_40_BIT_LEN:
				wep->wep_type_for_key2 =
					HostCmd_TYPE_WEP_40_BIT;
				memmove(priv->adapter, wep->wep2,
					priv->wep_key[1].key_material,
					priv->wep_key[1].key_length);
				break;
			case WEP_104_BIT_LEN:
				wep->wep_type_for_key2 =
					HostCmd_TYPE_WEP_104_BIT;
				memmove(priv->adapter, wep->wep2,
					priv->wep_key[1].key_material,
					priv->wep_key[1].key_length);
				break;
			case 0:
				break;
			default:
				PRINTM(MWARN, "Key2 Length = %d is incorrect\n",
					priv->wep_key[1].key_length);
				ret = MLAN_STATUS_FAILURE;
				goto done;
		}
		switch (priv->wep_key[2].key_length) {
			case WEP_40_BIT_LEN:
				wep->wep_type_for_key3 =
					HostCmd_TYPE_WEP_40_BIT;
				memmove(priv->adapter, wep->wep3,
					priv->wep_key[2].key_material,
					priv->wep_key[2].key_length);
				break;
			case WEP_104_BIT_LEN:
				wep->wep_type_for_key3 =
					HostCmd_TYPE_WEP_104_BIT;
				memmove(priv->adapter, wep->wep3,
					priv->wep_key[2].key_material,
					priv->wep_key[2].key_length);
				break;
			case 0:
				break;
			default:
				PRINTM(MWARN, "Key3 Length = %d is incorrect\n",
					priv->wep_key[2].key_length);
				ret = MLAN_STATUS_FAILURE;
				goto done;
		}

		switch (priv->wep_key[3].key_length) {
			case WEP_40_BIT_LEN:
				wep->wep_type_for_key4 =
					HostCmd_TYPE_WEP_40_BIT;
				memmove(priv->adapter, wep->wep4,
					priv->wep_key[3].key_material,
					priv->wep_key[3].key_length);
				break;
			case WEP_104_BIT_LEN:
				wep->wep_type_for_key4 =
					HostCmd_TYPE_WEP_104_BIT;
				memmove(priv->adapter, wep->wep4,
					priv->wep_key[3].key_material,
					priv->wep_key[3].key_length);
				break;
			case 0:
				break;
			default:
				PRINTM(MWARN, "Key4 Length = %d is incorrect\n",
					priv->wep_key[3].key_length);
				ret = MLAN_STATUS_FAILURE;
				goto done;
		}
	}
	else if (cmd_action == HostCmd_ACT_REMOVE) {
		cmd->command = wlan_cpu_to_le16(HostCmd_CMD_802_11_SET_WEP);
		cmd->size = wlan_cpu_to_le16((sizeof(HostCmd_DS_802_11_SET_WEP))
						+ S_DS_GEN);
		wep->action = wlan_cpu_to_le16(HostCmd_ACT_REMOVE);

		/* default tx key index */
		wep->key_index = wlan_cpu_to_le16((t_u16)
					(priv->wep_key_curr_index &
					(t_u32) HostCmd_WEP_KEY_INDEX_MASK));
	}

	ret = MLAN_STATUS_SUCCESS;
done:
	LEAVE();
	return ret;
}
#endif /* KEY_MATERIAL_WEP */

#ifdef KEY_PARAM_SET_V2
/**
 *  @brief This function prepares command of key_material.
 *
 *  @param pmpriv       A pointer to mlan_private structure
 *  @param cmd          A pointer to HostCmd_DS_COMMAND structure
 *  @param cmd_action   The action: GET or SET
 *  @param cmd_oid      OID: ENABLE or DISABLE
 *  @param pdata_buf    A pointer to data buffer
 *
 *  @return             MLAN_STATUS_SUCCESS or MLAN_STATUS_FAILURE
 */
static mlan_status wlan_cmd_802_11_key_material(
		IN pmlan_private        pmpriv,
		IN HostCmd_DS_COMMAND   *cmd,
		IN t_u16                cmd_action,
		IN t_u32                cmd_oid,
		IN t_void               *pdata_buf)
{
	HostCmd_DS_802_11_KEY_MATERIAL  *pkey_material =
					&cmd->params.key_material;
	mlan_ds_encrypt_key *pkey = (mlan_ds_encrypt_key*)pdata_buf;
	mlan_status ret = MLAN_STATUS_SUCCESS;

	ENTER ();
	if(!pkey) {
		ret = MLAN_STATUS_FAILURE;
		goto done;
	}
	cmd->command = wlan_cpu_to_le16(HostCmd_CMD_802_11_KEY_MATERIAL);
	pkey_material->action = wlan_cpu_to_le16(cmd_action);
	if( cmd_action == HostCmd_ACT_GEN_GET ) {
		PRINTM(MCMND, "GET Key\n");
		pkey_material->key_param_set.key_idx =
				pkey->key_index & KEY_INDEX_MASK;
		pkey_material->key_param_set.type =
				wlan_cpu_to_le16(TLV_TYPE_KEY_PARAM_V2);
		pkey_material->key_param_set.length =
				wlan_cpu_to_le16(KEY_PARAMS_FIXED_LEN);
		memcpy(pmpriv->adapter, pkey_material->key_param_set.mac_addr,
				pkey->mac_addr, MLAN_MAC_ADDR_LENGTH);
		if(pkey->key_flags & KEY_FLAG_GROUP_KEY)
			pkey_material->key_param_set.key_info |=
					KEY_INFO_MCAST_KEY;
		else
			pkey_material->key_param_set.key_info |=
					KEY_INFO_UCAST_KEY;
#ifdef ENABLE_802_11W
		if(pkey->key_flags & KEY_FLAG_AES_MCAST_IGTK)
			pkey_material->key_param_set.key_info =
					KEY_INFO_CMAC_AES_KEY;
#endif
		pkey_material->key_param_set.key_info =
			wlan_cpu_to_le16(pkey_material->key_param_set.key_info);
		cmd->size = wlan_cpu_to_le16(sizeof(MrvlIEtypesHeader_t)
				+ S_DS_GEN + KEY_PARAMS_FIXED_LEN
				+ sizeof(pkey_material->action));
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
	pkey_material->key_param_set.key_idx =
				pkey->key_index & KEY_INDEX_MASK;
	pkey_material->key_param_set.type =
				wlan_cpu_to_le16(TLV_TYPE_KEY_PARAM_V2);
	pkey_material->key_param_set.key_info = KEY_INFO_ENABLE_KEY;
	memcpy(pmpriv->adapter, pkey_material->key_param_set.mac_addr,
				pkey->mac_addr, MLAN_MAC_ADDR_LENGTH);
	if(pkey->key_len <= MAX_WEP_KEY_SIZE) {
		pkey_material->key_param_set.length =
				wlan_cpu_to_le16(KEY_PARAMS_FIXED_LEN
				+ sizeof(wep_param_t));
		pkey_material->key_param_set.key_type = KEY_TYPE_ID_WEP;
		if(pkey->is_current_wep_key){
			pkey_material->key_param_set.key_info |=
					KEY_INFO_MCAST_KEY | KEY_INFO_UCAST_KEY;
			if(pkey_material->key_param_set.key_idx ==
				(pmpriv->wep_key_curr_index & KEY_INDEX_MASK))
				pkey_material->key_param_set.key_info |=
					KEY_INFO_DEFAULT_KEY;
		}
		else{
			if(pkey->key_flags & KEY_FLAG_GROUP_KEY)
				pkey_material->key_param_set.key_info |=
					 KEY_INFO_MCAST_KEY;
			else
				pkey_material->key_param_set.key_info |=
					KEY_INFO_UCAST_KEY;
			if(pkey->key_flags & KEY_FLAG_SET_TX_KEY)
				pkey_material->key_param_set.key_info |=
					KEY_INFO_DEFAULT_KEY;
		}
		pkey_material->key_param_set.key_info =
				wlan_cpu_to_le16(pkey_material->key_param_set.key_info);
		pkey_material->key_param_set.key_params.wep.key_len =
				wlan_cpu_to_le16(pkey->key_len);
		memcpy(pmpriv->adapter, pkey_material->key_param_set.key_params.wep.key,
				pkey->key_material,pkey->key_len);
		cmd->size = wlan_cpu_to_le16(sizeof(MrvlIEtypesHeader_t)
				+ S_DS_GEN + KEY_PARAMS_FIXED_LEN
				+ sizeof(wep_param_t) + sizeof(pkey_material->action));
		PRINTM(MCMND, "Set WEP Key\n");
		goto done;
	}
	if(pkey->key_flags & KEY_FLAG_GROUP_KEY)
		pkey_material->key_param_set.key_info |= KEY_INFO_MCAST_KEY;
	else
		pkey_material->key_param_set.key_info |= KEY_INFO_UCAST_KEY;
	if(pkey->key_flags & KEY_FLAG_SET_TX_KEY)
		pkey_material->key_param_set.key_info |=
				KEY_INFO_TX_KEY | KEY_INFO_RX_KEY;
	else
		pkey_material->key_param_set.key_info |= KEY_INFO_RX_KEY;
#if defined(WAPI)
	if(pkey->is_wapi_key) {
		pkey_material->key_param_set.key_type = KEY_TYPE_ID_WAPI;
		memcpy(pmpriv->adapter,
			pkey_material->key_param_set.key_params.wapi.pn,
			pkey->pn, PN_SIZE);
		pkey_material->key_param_set.key_params.wapi.key_len =
				wlan_cpu_to_le16(pkey->key_len);
		memcpy(pmpriv->adapter,
			pkey_material->key_param_set.key_params.wapi.key,
			pkey->key_material,pkey->key_len);
		if(!pmpriv->sec_info.wapi_key_on)
			pkey_material->key_param_set.key_info |=
				KEY_INFO_DEFAULT_KEY;
		if(pkey->key_flags & KEY_FLAG_GROUP_KEY)
			pmpriv->sec_info.wapi_key_on = MTRUE;
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
	if(pmpriv->bss_mode == MLAN_BSS_MODE_INFRA) {
		/* Enable default key for WPA/WPA2 */
		if(!pmpriv->wpa_is_gtk_set)
			pkey_material->key_param_set.key_info |=
					KEY_INFO_DEFAULT_KEY;
	}
	else {
		pkey_material->key_param_set.key_info |= KEY_INFO_DEFAULT_KEY;
		/* Enable unicast bit for WPA-NONE/ADHOC_AES */
		if((!pmpriv->sec_info.wpa2_enabled) &&
			(pkey->key_flags & KEY_FLAG_SET_TX_KEY))
			pkey_material->key_param_set.key_info |=
					KEY_INFO_UCAST_KEY;
	}
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
				pkey_material->key_param_set.key_params.aes.pn,
				pkey->pn, SEQ_MAX_SIZE);
			pkey_material->key_param_set.key_type = KEY_TYPE_ID_AES;
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
	if (pkey->key_len == WPA_IGTK_KEY_LEN &&
			(pkey->key_flags & KEY_FLAG_AES_MCAST_IGTK)) {
		if(pkey->key_flags &
			(KEY_FLAG_RX_SEQ_VALID | KEY_FLAG_TX_SEQ_VALID))
			memcpy(pmpriv->adapter,
				pkey_material->key_param_set.key_params.
				cmac_aes.ipn,
				pkey->pn, SEQ_MAX_SIZE);
			pkey_material->key_param_set.key_info &=
					~KEY_INFO_MCAST_KEY;
			pkey_material->key_param_set.key_info |=
					wlan_cpu_to_le16(KEY_INFO_AES_MCAST_IGTK);
			pkey_material->key_param_set.key_type =
					KEY_TYPE_ID_AES_CMAC;
			pkey_material->key_param_set.key_params.
					cmac_aes.key_len =
					wlan_cpu_to_le16(pkey->key_len);
			memcpy(pmpriv->adapter,
				pkey_material->key_param_set.key_params.
				cmac_aes.key,
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
	if (pkey->key_len == WPA_TKIP_KEY_LEN) {
		if(pkey->key_flags &
			(KEY_FLAG_RX_SEQ_VALID | KEY_FLAG_TX_SEQ_VALID))
			memcpy(pmpriv->adapter,
					pkey_material->key_param_set.key_params.
					tkip.pn, pkey->pn, SEQ_MAX_SIZE);
			pkey_material->key_param_set.key_type =
					KEY_TYPE_ID_TKIP;
			pkey_material->key_param_set.key_params.tkip.key_len =
					wlan_cpu_to_le16(pkey->key_len);
			memcpy(pmpriv->adapter, pkey_material->key_param_set.key_params.tkip.key,
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
done:
#if 0
	dump_hex(cmd, cmd->size);
#endif
	LEAVE();
	return ret;
}
#else
/**
*  @brief This function prepares command of key_material.
*
*  @param pmpriv       A pointer to mlan_private structure
*  @param cmd          A pointer to HostCmd_DS_COMMAND structure
*  @param cmd_action   The action: GET or SET
*  @param cmd_oid      OID: ENABLE or DISABLE
*  @param pdata_buf    A pointer to data buffer
*
*  @return             MLAN_STATUS_SUCCESS or MLAN_STATUS_FAILURE
*/
static mlan_status wlan_cmd_802_11_key_material(
		IN pmlan_private        pmpriv,
		IN HostCmd_DS_COMMAND   *cmd,
		IN t_u16                cmd_action,
		IN t_u32                cmd_oid,
		IN t_void               *pdata_buf)
{
	HostCmd_DS_802_11_KEY_MATERIAL  *pkey_material =
					&cmd->params.key_material;
	mlan_ds_encrypt_key             *pkey = (mlan_ds_encrypt_key*)pdata_buf;
	t_u16                           key_param_len = 0;
	mlan_status                     ret = MLAN_STATUS_SUCCESS;
#ifdef WPSE_SUPPORT
	MrvlIEtypes_MacAddr_t           *tlv = MNULL;
#endif
#if defined(WAPI)
	const t_u8      bc_mac[] = {0xff, 0xff, 0xff, 0xff, 0xff, 0xff};
#endif

	ENTER();

	cmd->command = wlan_cpu_to_le16(HostCmd_CMD_802_11_KEY_MATERIAL);
	pkey_material->action = wlan_cpu_to_le16(cmd_action);

	if( cmd_action == HostCmd_ACT_GEN_GET ) {
		cmd->size = wlan_cpu_to_le16(sizeof(pkey_material->action)
				+ S_DS_GEN + KEYPARAMSET_FIXED_LEN
				+ sizeof(MrvlIEtypesHeader_t));
		memset(pmpriv->adapter, &pkey_material->key_param_set, 0,
				sizeof(MrvlIEtype_KeyParamSet_t));
		pkey_material->key_param_set.type =
				wlan_cpu_to_le16(TLV_TYPE_KEY_MATERIAL);
		pkey_material->key_param_set.length =
				wlan_cpu_to_le16(KEYPARAMSET_FIXED_LEN);
		if(pkey->key_flags & KEY_FLAG_GROUP_KEY)
			pkey_material->key_param_set.key_info |=
					KEY_INFO_MCAST_KEY;
		else
			pkey_material->key_param_set.key_info |=
					KEY_INFO_UCAST_KEY;
#ifdef ENABLE_802_11W
		if (pkey->key_flags & KEY_FLAG_AES_MCAST_IGTK)
			pkey_material->key_param_set.key_info =
					KEY_INFO_AES_MCAST_IGTK;
#endif
		pkey_material->key_param_set.key_info =
			wlan_cpu_to_le16(pkey_material->key_param_set.key_info);
		goto done;
	}

#ifdef KEY_MATERIAL_WEP
	if (!pkey) {
		memset(pmpriv->adapter, &pkey_material->key_param_set, 0,
			(MRVL_NUM_WEP_KEY * sizeof(MrvlIEtype_KeyParamSet_t)));
		ret = wlan_set_keyparamset_wep(pmpriv,
				&pkey_material->key_param_set,
				&key_param_len);
		cmd->size = wlan_cpu_to_le16(key_param_len
					+ sizeof(pkey_material->action)
					+ S_DS_GEN);
		goto done;
	} else
#endif /* KEY_MATERIAL_WEP */
		memset(pmpriv->adapter, &pkey_material->key_param_set,
				0, sizeof(MrvlIEtype_KeyParamSet_t));
#if defined(WAPI)
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

		pkey_material->key_param_set.key[0] = (t_u8)pkey->key_index;
		if(!pmpriv->sec_info.wapi_key_on)
			pkey_material->key_param_set.key[1] = 1;
		else
			pkey_material->key_param_set.key[1] = 0;
			/* set 0 when re-key */

		/* WAPI pairwise key: unicast */
		if (0 != memcmp(pmpriv->adapter, pkey->mac_addr,
				bc_mac, sizeof(bc_mac)))
			pkey_material->key_param_set.key_info |=
					wlan_cpu_to_le16(KEY_INFO_WAPI_UNICAST);
		else {  /* WAPI group key: multicast */
			pkey_material->key_param_set.key_info |=
					wlan_cpu_to_le16(KEY_INFO_WAPI_MCAST);
			pmpriv->sec_info.wapi_key_on = MTRUE;
		}

		pkey_material->key_param_set.type =
				wlan_cpu_to_le16(TLV_TYPE_KEY_MATERIAL);
		pkey_material->key_param_set.key_len =
				wlan_cpu_to_le16(WAPI_KEY_LEN);
		memcpy(pmpriv->adapter, &pkey_material->key_param_set.key[2],
				pkey->key_material, pkey->key_len);
		memcpy(pmpriv->adapter, &pkey_material->key_param_set.key[2+pkey->key_len],
				pkey->pn, PN_SIZE);
		pkey_material->key_param_set.length =
			wlan_cpu_to_le16(WAPI_KEY_LEN + KEYPARAMSET_FIXED_LEN);

		key_param_len = (WAPI_KEY_LEN + KEYPARAMSET_FIXED_LEN)
				+ sizeof(MrvlIEtypesHeader_t);
		cmd->size = wlan_cpu_to_le16(key_param_len
				+ sizeof(pkey_material->action) + S_DS_GEN);
		goto done;
	}
#endif /* WAPI */
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
				KEY_INFO_AES_ENABLED;
		else
			pkey_material->key_param_set.key_info = 0;

		/* AES pairwise key: unicast */
		if (pkey->key_index & MLAN_KEY_INDEX_UNICAST)
			pkey_material->key_param_set.key_info |=
					KEY_INFO_AES_UNICAST;
		else /* AES group key: multicast */
			pkey_material->key_param_set.key_info |=
					KEY_INFO_AES_MCAST;
		/** pass key index to BIT(6) and BIT(7)	*/
		pkey_material->key_param_set.key_info |=
			(pkey->key_index & 0x03) << 6;
		pkey_material->key_param_set.key_info =
			wlan_cpu_to_le16(pkey_material->key_param_set.key_info);
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
	else if (pkey->key_len == WPA_TKIP_KEY_LEN) {
		PRINTM(MCMND, "WPA_TKIP\n");
		pkey_material->key_param_set.key_type_id =
				wlan_cpu_to_le16(KEY_TYPE_ID_TKIP);
		pkey_material->key_param_set.key_info = KEY_INFO_TKIP_ENABLED;
		 /* TKIP pairwise key: unicast */
		if (pkey->key_index & MLAN_KEY_INDEX_UNICAST)
			pkey_material->key_param_set.key_info |=
					KEY_INFO_TKIP_UNICAST;
		else  /* TKIP group key: multicast */
			pkey_material->key_param_set.key_info |=
					KEY_INFO_TKIP_MCAST;
		/** pass key index to BIT(6) and BIT(7)	*/
		pkey_material->key_param_set.key_info |=
			(pkey->key_index & 0x03) << 6;
		pkey_material->key_param_set.key_info =
			wlan_cpu_to_le16(pkey_material->key_param_set.key_info);
	}

	if (pkey_material->key_param_set.key_type_id) {
		pkey_material->key_param_set.type =
				wlan_cpu_to_le16(TLV_TYPE_KEY_MATERIAL);
		pkey_material->key_param_set.key_len =
				wlan_cpu_to_le16((t_u16)pkey->key_len);
		memcpy(pmpriv->adapter, pkey_material->key_param_set.key,
				pkey->key_material, pkey->key_len);
		pkey_material->key_param_set.length =
				wlan_cpu_to_le16((t_u16)pkey->key_len
						+ KEYPARAMSET_FIXED_LEN);
		key_param_len = (t_u16)(pkey->key_len
				+ KEYPARAMSET_FIXED_LEN)
				+ sizeof(MrvlIEtypesHeader_t);

#ifdef ENABLE_802_11W
		/* key format with pn field is defined in Key Material V1 */
		if (pkey_material->key_param_set.key_type_id ==
				wlan_cpu_to_le16(KEY_TYPE_ID_AES_CMAC)) {
			cmac_param *param;
			param = (cmac_param *)pkey_material->key_param_set.key;
			memcpy(pmpriv->adapter, param->ipn, pkey->pn,
					SEQ_MAX_SIZE);
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

#ifdef ENABLE_IBSS_WPA2
		if ((pmpriv->bss_mode == MLAN_BSS_MODE_IBSS) &&
				(pmpriv->sec_info.wpa2_enabled)) {
			tlv = (MrvlIEtypes_MacAddr_t *)
				((t_u8 *)&pkey_material->key_param_set
					+ key_param_len);
			tlv->header.type =
				wlan_cpu_to_le16(TLV_TYPE_STA_MAC_ADDRESS);
			tlv->header.len =
				wlan_cpu_to_le16(MLAN_MAC_ADDR_LENGTH);
			memcpy(pmpriv->adapter,
				tlv->mac,pkey->mac_addr, MLAN_MAC_ADDR_LENGTH);
			cmd->size = wlan_cpu_to_le16(key_param_len
					+ sizeof(pkey_material->action)
					+ S_DS_GEN
					+ sizeof(MrvlIEtypes_MacAddr_t));
		}
		else
#endif
			cmd->size = wlan_cpu_to_le16(key_param_len
				+ sizeof(pkey_material->action) + S_DS_GEN);
	}
done:
	LEAVE();
	return ret;
}
#endif /* KEY_PARAM_SET_V2 */
#endif /* WPA */

/**
 *  @brief This function prepares command of supplicant pmk
 *
 *  @param pmpriv       A pointer to mlan_private structure
 *  @param cmd          A pointer to HostCmd_DS_COMMAND structure
 *  @param cmd_action   The action: GET or SET
 *  @param pdata_buf    A pointer to data buffer
 *
 *  @return             MLAN_STATUS_SUCCESS or MLAN_STATUS_FAILURE
 */
static mlan_status
wlan_cmd_802_11_supplicant_pmk(IN pmlan_private pmpriv,
                               IN HostCmd_DS_COMMAND * cmd,
                               IN t_u16 cmd_action, IN t_void * pdata_buf)
{
    MrvlIEtypes_PMK_t *ppmk_tlv = MNULL;
    MrvlIEtypes_Passphrase_t *ppassphrase_tlv = MNULL;
    MrvlIEtypes_SsIdParamSet_t *pssid_tlv = MNULL;
    MrvlIEtypes_Bssid_t *pbssid_tlv = MNULL;
    HostCmd_DS_802_11_SUPPLICANT_PMK *pesupplicant_psk =
        &cmd->params.esupplicant_psk;
    t_u8 *ptlv_buffer = (t_u8 *) pesupplicant_psk->tlv_buffer;
    mlan_ds_passphrase *psk = (mlan_ds_passphrase *) pdata_buf;
    t_u8 zero_mac[] = { 0, 0, 0, 0, 0, 0 };

    ENTER();
    /*
     * Parse the rest of the buf here
     *      1)  <ssid="valid ssid"> - This will get the passphrase, AKMP
     *          for specified ssid, if none specified then it will get all.
     *          Eg: iwpriv <mlanX> passphrase 0:ssid=marvell
     *      2)  <psk="psk">:<passphrase="passphare">:<bssid="00:50:43:ef:23:f3">
     *          <ssid="valid ssid"> - passphrase and psk cannot be provided to
     *          the same SSID, Takes one SSID at a time, If ssid= is present
     *          the it should contain a passphrase or psk. If no arguments are
     *          provided then AKMP=802.1x, and passphrase should be provided
     *          after association.
     *          End of each parameter should be followed by a ':'(except for the
     *          last parameter) as the delimiter. If ':' has to be used in
     *          an SSID then a '/' should be preceded to ':' as a escape.
     *          Eg:iwpriv <mlanX> passphrase
     *                    "1:ssid=mrvl AP:psk=abcdefgh:bssid=00:50:43:ef:23:f3"
     *          iwpriv <mlanX> passphrase
     *                 "1:ssid=mrvl/: AP:psk=abcdefgd:bssid=00:50:43:ef:23:f3"
     *          iwpriv <mlanX> passphrase "1:ssid=mrvlAP:psk=abcdefgd"
     *      3)  <ssid="valid ssid"> - This will clear the passphrase
     *          for specified ssid, if none specified then it will clear all.
     *          Eg: iwpriv <mlanX> passphrase 2:ssid=marvell
     */

    /* -1 is for t_u8 TlvBuffer[1] as this should not be included */
    cmd->size = sizeof(HostCmd_DS_802_11_SUPPLICANT_PMK) + S_DS_GEN - 1;
    if (psk->ssid.ssid_len) {
        pssid_tlv = (MrvlIEtypes_SsIdParamSet_t *) ptlv_buffer;
        pssid_tlv->header.type = wlan_cpu_to_le16(TLV_TYPE_SSID);
        pssid_tlv->header.len =
            (t_u16) MIN(MLAN_MAX_SSID_LENGTH, psk->ssid.ssid_len);
        memcpy(pmpriv->adapter, (char *) pssid_tlv->ssid, psk->ssid.ssid,
               MIN(MLAN_MAX_SSID_LENGTH, psk->ssid.ssid_len));
        ptlv_buffer += (pssid_tlv->header.len + sizeof(MrvlIEtypesHeader_t));
        cmd->size += (pssid_tlv->header.len + sizeof(MrvlIEtypesHeader_t));
        pssid_tlv->header.len = wlan_cpu_to_le16(pssid_tlv->header.len);
    }
    if (memcmp
        (pmpriv->adapter, (t_u8 *) & psk->bssid, zero_mac, sizeof(zero_mac))) {
        pbssid_tlv = (MrvlIEtypes_Bssid_t *) ptlv_buffer;
        pbssid_tlv->header.type = wlan_cpu_to_le16(TLV_TYPE_BSSID);
        pbssid_tlv->header.len = MLAN_MAC_ADDR_LENGTH;
        memcpy(pmpriv->adapter, pbssid_tlv->bssid, (t_u8 *) & psk->bssid,
               MLAN_MAC_ADDR_LENGTH);
        ptlv_buffer += (pbssid_tlv->header.len + sizeof(MrvlIEtypesHeader_t));
        cmd->size += (pbssid_tlv->header.len + sizeof(MrvlIEtypesHeader_t));
        pbssid_tlv->header.len = wlan_cpu_to_le16(pbssid_tlv->header.len);
    }
    if (psk->psk_type == MLAN_PSK_PASSPHRASE) {
        ppassphrase_tlv = (MrvlIEtypes_Passphrase_t *) ptlv_buffer;
        ppassphrase_tlv->header.type = wlan_cpu_to_le16(TLV_TYPE_PASSPHRASE);
        ppassphrase_tlv->header.len =
            (t_u16) MIN(MLAN_MAX_PASSPHRASE_LENGTH,
                        psk->psk.passphrase.passphrase_len);
        memcpy(pmpriv->adapter, ppassphrase_tlv->passphrase,
               psk->psk.passphrase.passphrase, MIN(MLAN_MAX_PASSPHRASE_LENGTH,
                                                   psk->psk.passphrase.
                                                   passphrase_len));
        ptlv_buffer +=
            (ppassphrase_tlv->header.len + sizeof(MrvlIEtypesHeader_t));
        cmd->size +=
            (ppassphrase_tlv->header.len + sizeof(MrvlIEtypesHeader_t));
        ppassphrase_tlv->header.len =
            wlan_cpu_to_le16(ppassphrase_tlv->header.len);
    }
    if (psk->psk_type == MLAN_PSK_PMK) {
        ppmk_tlv = (MrvlIEtypes_PMK_t *) ptlv_buffer;
        ppmk_tlv->header.type = wlan_cpu_to_le16(TLV_TYPE_PMK);
        ppmk_tlv->header.len = MLAN_MAX_KEY_LENGTH;
        memcpy(pmpriv->adapter, ppmk_tlv->pmk, psk->psk.pmk.pmk,
               MLAN_MAX_KEY_LENGTH);
        ptlv_buffer += (ppmk_tlv->header.len + sizeof(MrvlIEtypesHeader_t));
        cmd->size += (ppmk_tlv->header.len + sizeof(MrvlIEtypesHeader_t));
        ppmk_tlv->header.len = wlan_cpu_to_le16(ppmk_tlv->header.len);
    }
    if ((cmd_action == HostCmd_ACT_GEN_SET) &&
        ((pssid_tlv || pbssid_tlv) && (!ppmk_tlv && !ppassphrase_tlv))) {
        PRINTM(MERROR,
               "Invalid case,ssid/bssid present without pmk or passphrase\n");
        LEAVE();
        return MLAN_STATUS_FAILURE;
    }
    cmd->command = wlan_cpu_to_le16(HostCmd_CMD_SUPPLICANT_PMK);
    pesupplicant_psk->action = wlan_cpu_to_le16(cmd_action);
    pesupplicant_psk->cache_result = 0;
    cmd->size = wlan_cpu_to_le16(cmd->size);
    LEAVE();
    return MLAN_STATUS_SUCCESS;
}

#ifndef CONFIG_MLAN_WMSDK
/**
 *  @brief Handle the supplicant profile command
 *
 *  @param pmpriv       A pointer to mlan_private structure
 *  @param cmd          A pointer to HostCmd_DS_COMMAND structure
 *  @param cmd_action   The action: GET or SET
 *
 *  @return             MLAN_STATUS_SUCCESS
 */
static mlan_status
wlan_cmd_802_11_supplicant_profile(IN pmlan_private pmpriv,
                                   IN HostCmd_DS_COMMAND * cmd,
                                   IN t_u16 cmd_action, IN t_void * pdata_buf)
{
    HostCmd_DS_802_11_SUPPLICANT_PROFILE *sup_profile =
        &cmd->params.esupplicant_profile;
    MrvlIEtypes_EncrProto_t *encr_proto_tlv = MNULL;
    MrvlIEtypes_Cipher_t *pcipher_tlv = MNULL;
    t_u8 *ptlv_buffer = (t_u8 *) sup_profile->tlv_buf;
    mlan_ds_esupp_mode *esupp = MNULL;

    ENTER();

    cmd->size =
        wlan_cpu_to_le16(sizeof(HostCmd_DS_802_11_SUPPLICANT_PROFILE) +
                         S_DS_GEN - 1);
    cmd->command = wlan_cpu_to_le16(HostCmd_CMD_SUPPLICANT_PROFILE);
    sup_profile->action = wlan_cpu_to_le16(cmd_action);
    if ((cmd_action == HostCmd_ACT_GEN_SET) && pdata_buf) {
        esupp = (mlan_ds_esupp_mode *) pdata_buf;
        if (esupp->rsn_mode) {
            encr_proto_tlv = (MrvlIEtypes_EncrProto_t *) ptlv_buffer;
            encr_proto_tlv->header.type =
                wlan_cpu_to_le16(TLV_TYPE_ENCRYPTION_PROTO);
            encr_proto_tlv->header.len =
                (t_u16) sizeof(encr_proto_tlv->rsn_mode);
            encr_proto_tlv->rsn_mode = wlan_cpu_to_le16(esupp->rsn_mode);
            ptlv_buffer +=
                (encr_proto_tlv->header.len + sizeof(MrvlIEtypesHeader_t));
            cmd->size +=
                (encr_proto_tlv->header.len + sizeof(MrvlIEtypesHeader_t));
            encr_proto_tlv->header.len =
                wlan_cpu_to_le16(encr_proto_tlv->header.len);
        }
        if (esupp->act_paircipher || esupp->act_groupcipher) {
            pcipher_tlv = (MrvlIEtypes_Cipher_t *) ptlv_buffer;
            pcipher_tlv->header.type = wlan_cpu_to_le16(TLV_TYPE_CIPHER);
            pcipher_tlv->header.len =
                (t_u16) (sizeof(pcipher_tlv->pair_cipher) +
                         sizeof(pcipher_tlv->group_cipher));
            if (esupp->act_paircipher) {
                pcipher_tlv->pair_cipher =
                    wlan_cpu_to_le16(esupp->act_paircipher);
            }
            if (esupp->act_groupcipher) {
                pcipher_tlv->group_cipher =
                    wlan_cpu_to_le16(esupp->act_groupcipher);
            }
            ptlv_buffer +=
                (pcipher_tlv->header.len + sizeof(MrvlIEtypesHeader_t));
            cmd->size +=
                (pcipher_tlv->header.len + sizeof(MrvlIEtypesHeader_t));
            pcipher_tlv->header.len = wlan_cpu_to_le16(pcipher_tlv->header.len);
        }
    }
    cmd->size = wlan_cpu_to_le16(cmd->size);
    LEAVE();
    return MLAN_STATUS_SUCCESS;
}
#endif /* 0 */

/**
 *  @brief This function prepares command of rf_channel.
 *
 *  @param pmpriv       A pointer to mlan_private structure
 *  @param cmd          A pointer to HostCmd_DS_COMMAND structure
 *  @param cmd_action   The action: GET or SET
 *  @param pdata_buf    A pointer to data buffer
 *
 *  @return             MLAN_STATUS_SUCCESS
 */
static mlan_status
wlan_cmd_802_11_rf_channel(IN pmlan_private pmpriv,
                           IN HostCmd_DS_COMMAND * cmd,
                           IN t_u16 cmd_action, IN t_void * pdata_buf)
{
    HostCmd_DS_802_11_RF_CHANNEL *prf_chan = &cmd->params.rf_channel;

    ENTER();

    cmd->command = wlan_cpu_to_le16(HostCmd_CMD_802_11_RF_CHANNEL);
    cmd->size = wlan_cpu_to_le16(sizeof(HostCmd_DS_802_11_RF_CHANNEL)
                                 + S_DS_GEN);

    if (cmd_action == HostCmd_ACT_GEN_SET) {
        if ((pmpriv->adapter->adhoc_start_band & BAND_A)
            || (pmpriv->adapter->adhoc_start_band & BAND_AN)
            )
            prf_chan->rf_type = HostCmd_SCAN_RADIO_TYPE_A;
        SET_SECONDARYCHAN(prf_chan->rf_type, pmpriv->adapter->chan_bandwidth);
        prf_chan->rf_type = wlan_cpu_to_le16(prf_chan->rf_type);
        prf_chan->current_channel = wlan_cpu_to_le16(*((t_u16 *) pdata_buf));
    }
    prf_chan->action = wlan_cpu_to_le16(cmd_action);
    LEAVE();
    return MLAN_STATUS_SUCCESS;
}

#ifndef CONFIG_MLAN_WMSDK
/**
 *  @brief This function prepares command of ibss_coalescing_status.
 *
 *  @param pmpriv       A pointer to mlan_private structure
 *  @param cmd          A pointer to HostCmd_DS_COMMAND structure
 *  @param cmd_action   The action: GET or SET
 *  @param pdata_buf    A pointer to data buffer or MNULL
 *
 *  @return             MLAN_STATUS_SUCCESS
 */
static mlan_status
wlan_cmd_ibss_coalescing_status(IN pmlan_private pmpriv,
                                IN HostCmd_DS_COMMAND * cmd,
                                IN t_u16 cmd_action, IN t_void * pdata_buf)
{
    HostCmd_DS_802_11_IBSS_STATUS *pibss_coal = &(cmd->params.ibss_coalescing);
    t_u16 enable = 0;

    ENTER();

    cmd->command = wlan_cpu_to_le16(HostCmd_CMD_802_11_IBSS_COALESCING_STATUS);
    cmd->size =
        wlan_cpu_to_le16(sizeof(HostCmd_DS_802_11_IBSS_STATUS) + S_DS_GEN);
    cmd->result = 0;
    pibss_coal->action = wlan_cpu_to_le16(cmd_action);

    switch (cmd_action) {
    case HostCmd_ACT_GEN_SET:
        if (pdata_buf != MNULL)
            enable = *(t_u16 *) pdata_buf;
        pibss_coal->enable = wlan_cpu_to_le16(enable);
        break;

        /* In other case.. Nothing to do */
    case HostCmd_ACT_GEN_GET:
    default:
        break;
    }

    LEAVE();
    return MLAN_STATUS_SUCCESS;
}
#endif /* CONFIG_MLAN_WMSDK */

/**
 *  @brief This function prepares command of mgmt IE list.
 *
 *  @param pmpriv       A pointer to mlan_private structure
 *  @param cmd          A pointer to HostCmd_DS_COMMAND structure
 *  @param cmd_action   The action: GET or SET
 *  @param pdata_buf	A pointer to data buffer
 *
 *  @return             MLAN_STATUS_SUCCESS
 */
static mlan_status
wlan_cmd_mgmt_ie_list(IN pmlan_private pmpriv,
                      IN HostCmd_DS_COMMAND * cmd,
                      IN t_u16 cmd_action, IN t_void * pdata_buf)
{
    t_u16 req_len = 0, travel_len = 0;
    custom_ie *cptr = MNULL;
    mlan_ds_misc_custom_ie *cust_ie = MNULL;
    HostCmd_DS_MGMT_IE_LIST_CFG *pmgmt_ie_list = &(cmd->params.mgmt_ie_list);

    ENTER();

    cmd->command = wlan_cpu_to_le16(HostCmd_CMD_MGMT_IE_LIST);
    cmd->size = sizeof(HostCmd_DS_MGMT_IE_LIST_CFG) + S_DS_GEN;
    cmd->result = 0;
    pmgmt_ie_list->action = wlan_cpu_to_le16(cmd_action);

    cust_ie = (mlan_ds_misc_custom_ie *) pdata_buf;
    pmgmt_ie_list->ds_mgmt_ie.type = wlan_cpu_to_le16(cust_ie->type);
    pmgmt_ie_list->ds_mgmt_ie.len = wlan_cpu_to_le16(cust_ie->len);

    if (pmgmt_ie_list->ds_mgmt_ie.ie_data_list && cust_ie->ie_data_list) {
        req_len = cust_ie->len;
        travel_len = 0;
        /* conversion for index, mask, len */
        if (req_len == sizeof(t_u16))
            cust_ie->ie_data_list[0].ie_index =
                wlan_cpu_to_le16(cust_ie->ie_data_list[0].ie_index);

        while (req_len > sizeof(t_u16)) {
            cptr =
                (custom_ie *) (((t_u8 *) cust_ie->ie_data_list) + travel_len);
            travel_len += cptr->ie_length + sizeof(custom_ie) - MAX_IE_SIZE;
            req_len -= cptr->ie_length + sizeof(custom_ie) - MAX_IE_SIZE;
            cptr->ie_index = wlan_cpu_to_le16(cptr->ie_index);
            cptr->mgmt_subtype_mask = wlan_cpu_to_le16(cptr->mgmt_subtype_mask);
            cptr->ie_length = wlan_cpu_to_le16(cptr->ie_length);
        }
        if (cust_ie->len)
            memcpy(pmpriv->adapter, pmgmt_ie_list->ds_mgmt_ie.ie_data_list,
                   cust_ie->ie_data_list, cust_ie->len);
    }

    cmd->size -=
        (MAX_MGMT_IE_INDEX_TO_FW * sizeof(custom_ie)) +
        sizeof(tlvbuf_max_mgmt_ie);
    cmd->size += cust_ie->len;
    cmd->size = wlan_cpu_to_le16(cmd->size);

    LEAVE();
    return MLAN_STATUS_SUCCESS;
}

#ifndef CONFIG_MLAN_WMSDK
/**
 *  @brief This function prepares system clock cfg command
 *
 *  @param pmpriv    	A pointer to mlan_private structure
 *  @param cmd	   	A pointer to HostCmd_DS_COMMAND structure
 *  @param cmd_action 	The action: GET or SET
 *  @param pdata_buf 	A pointer to data buffer
 *  @return 	   	MLAN_STATUS_SUCCESS
 */
static mlan_status
wlan_cmd_sysclock_cfg(IN pmlan_private pmpriv,
                      IN HostCmd_DS_COMMAND * cmd,
                      IN t_u16 cmd_action, IN t_void * pdata_buf)
{
    HostCmd_DS_ECL_SYSTEM_CLOCK_CONFIG *cfg = &cmd->params.sys_clock_cfg;
    mlan_ds_misc_sys_clock *clk_cfg = (mlan_ds_misc_sys_clock *) pdata_buf;
    int i = 0;

    ENTER();

    cmd->command = wlan_cpu_to_le16(HostCmd_CMD_ECL_SYSTEM_CLOCK_CONFIG);
    cmd->size =
        wlan_cpu_to_le16(sizeof(HostCmd_DS_ECL_SYSTEM_CLOCK_CONFIG) + S_DS_GEN);

    cfg->action = wlan_cpu_to_le16(cmd_action);
    cfg->cur_sys_clk = wlan_cpu_to_le16(clk_cfg->cur_sys_clk);
    cfg->sys_clk_type = wlan_cpu_to_le16(clk_cfg->sys_clk_type);
    cfg->sys_clk_len = wlan_cpu_to_le16(clk_cfg->sys_clk_num) * sizeof(t_u16);
    for (i = 0; i < clk_cfg->sys_clk_num; i++)
        cfg->sys_clk[i] = wlan_cpu_to_le16(clk_cfg->sys_clk[i]);

    LEAVE();
    return MLAN_STATUS_SUCCESS;
}
#endif /* CONFIG_MLAN_WMSDK */

/**
 *  @brief This function prepares command of reg_access.
 *
 *  @param cmd          A pointer to HostCmd_DS_COMMAND structure
 *  @param cmd_action   the action: GET or SET
 *  @param pdata_buf    A pointer to data buffer
 *  @return             MLAN_STATUS_SUCCESS or MLAN_STATUS_FAILURE
 */
/* static */ mlan_status
wlan_cmd_reg_access(IN HostCmd_DS_COMMAND * cmd,
                    IN t_u16 cmd_action, IN t_void * pdata_buf)
{
    mlan_ds_reg_rw *reg_rw;

    ENTER();

    reg_rw = (mlan_ds_reg_rw *) pdata_buf;
    switch (cmd->command) {
    case HostCmd_CMD_MAC_REG_ACCESS:
        {
            HostCmd_DS_MAC_REG_ACCESS *mac_reg;
            cmd->size =
                wlan_cpu_to_le16(sizeof(HostCmd_DS_MAC_REG_ACCESS) + S_DS_GEN);
            mac_reg = (HostCmd_DS_MAC_REG_ACCESS *) & cmd->params.mac_reg;
            mac_reg->action = wlan_cpu_to_le16(cmd_action);
            mac_reg->offset = wlan_cpu_to_le16((t_u16) reg_rw->offset);
            mac_reg->value = wlan_cpu_to_le32(reg_rw->value);
            break;
        }
    case HostCmd_CMD_BBP_REG_ACCESS:
        {
            HostCmd_DS_BBP_REG_ACCESS *bbp_reg;
            cmd->size =
                wlan_cpu_to_le16(sizeof(HostCmd_DS_BBP_REG_ACCESS) + S_DS_GEN);
            bbp_reg = (HostCmd_DS_BBP_REG_ACCESS *) & cmd->params.bbp_reg;
            bbp_reg->action = wlan_cpu_to_le16(cmd_action);
            bbp_reg->offset = wlan_cpu_to_le16((t_u16) reg_rw->offset);
            bbp_reg->value = (t_u8) reg_rw->value;
            break;
        }
    case HostCmd_CMD_RF_REG_ACCESS:
        {
            HostCmd_DS_RF_REG_ACCESS *rf_reg;
            cmd->size =
                wlan_cpu_to_le16(sizeof(HostCmd_DS_RF_REG_ACCESS) + S_DS_GEN);
            rf_reg = (HostCmd_DS_RF_REG_ACCESS *) & cmd->params.rf_reg;
            rf_reg->action = wlan_cpu_to_le16(cmd_action);
            rf_reg->offset = wlan_cpu_to_le16((t_u16) reg_rw->offset);
            rf_reg->value = (t_u8) reg_rw->value;
            break;
        }
    case HostCmd_CMD_CAU_REG_ACCESS:
        {
            HostCmd_DS_RF_REG_ACCESS *cau_reg;
            cmd->size =
                wlan_cpu_to_le16(sizeof(HostCmd_DS_RF_REG_ACCESS) + S_DS_GEN);
            cau_reg = (HostCmd_DS_RF_REG_ACCESS *) & cmd->params.rf_reg;
            cau_reg->action = wlan_cpu_to_le16(cmd_action);
            cau_reg->offset = wlan_cpu_to_le16((t_u16) reg_rw->offset);
            cau_reg->value = (t_u8) reg_rw->value;
            break;
        }
    case HostCmd_CMD_802_11_EEPROM_ACCESS:
        {
            mlan_ds_read_eeprom *rd_eeprom = (mlan_ds_read_eeprom *) pdata_buf;
            HostCmd_DS_802_11_EEPROM_ACCESS *cmd_eeprom =
                (HostCmd_DS_802_11_EEPROM_ACCESS *) & cmd->params.eeprom;
            cmd->size =
                wlan_cpu_to_le16(sizeof(HostCmd_DS_802_11_EEPROM_ACCESS) +
                                 S_DS_GEN);
            cmd_eeprom->action = wlan_cpu_to_le16(cmd_action);
            cmd_eeprom->offset = wlan_cpu_to_le16(rd_eeprom->offset);
            cmd_eeprom->byte_count = wlan_cpu_to_le16(rd_eeprom->byte_count);
            cmd_eeprom->value = 0;
            break;
        }
    default:
        LEAVE();
        return MLAN_STATUS_FAILURE;
    }
    cmd->command = wlan_cpu_to_le16(cmd->command);

    LEAVE();
    return MLAN_STATUS_SUCCESS;
}

/**
 *  @brief This function prepares command of mem_access.
 *
 *  @param cmd          A pointer to HostCmd_DS_COMMAND structure
 *  @param cmd_action   the action: GET or SET
 *  @param pdata_buf    A pointer to data buffer
 *  @return             MLAN_STATUS_SUCCESS
 */
mlan_status
wlan_cmd_mem_access(IN HostCmd_DS_COMMAND * cmd,
                    IN t_u16 cmd_action, IN t_void * pdata_buf)
{
    mlan_ds_mem_rw *mem_rw = (mlan_ds_mem_rw *) pdata_buf;
    HostCmd_DS_MEM_ACCESS *mem_access =
        (HostCmd_DS_MEM_ACCESS *) & cmd->params.mem;

    ENTER();

    cmd->command = wlan_cpu_to_le16(HostCmd_CMD_MEM_ACCESS);
    cmd->size = wlan_cpu_to_le16(sizeof(HostCmd_DS_MEM_ACCESS) + S_DS_GEN);

    mem_access->action = wlan_cpu_to_le16(cmd_action);
    mem_access->addr = wlan_cpu_to_le32(mem_rw->addr);
    mem_access->value = wlan_cpu_to_le32(mem_rw->value);

    LEAVE();
    return MLAN_STATUS_SUCCESS;
}

/**
 *  @brief This function prepares command of auto_reconnect
 *
 *  @param cmd          A pointer to HostCmd_DS_COMMAND structure
 *  @param cmd_action   the action: GET or SET
 *  @param pdata_buf    A pointer to data buffer
 *  @return             MLAN_STATUS_SUCCESS
 */
mlan_status
wlan_cmd_auto_reconnect(IN HostCmd_DS_COMMAND * cmd,
                    IN t_u16 cmd_action, IN t_void * pdata_buf)
{
    mlan_ds_auto_reconnect *auto_reconnect = (mlan_ds_auto_reconnect *) pdata_buf;
    HostCmd_DS_AUTO_RECONNECT *auto_reconn =
        (HostCmd_DS_AUTO_RECONNECT *) & cmd->params.auto_reconnect;

    ENTER();

    cmd->command = wlan_cpu_to_le16(HostCmd_CMD_AUTO_RECONNECT);
    cmd->size = wlan_cpu_to_le16(sizeof(HostCmd_DS_AUTO_RECONNECT) + S_DS_GEN);


    auto_reconn->action = wlan_cpu_to_le16(cmd_action);
    auto_reconn->reconnect_counter = auto_reconnect->reconnect_counter;
    auto_reconn->reconnect_interval = auto_reconnect->reconnect_interval;
    auto_reconn->flags = wlan_cpu_to_le16(auto_reconnect->flags);

    LEAVE();
    return MLAN_STATUS_SUCCESS;
}

/**
 *  @brief This function prepares command of rx management indication
 *
 *  @param cmd          A pointer to HostCmd_DS_COMMAND structure
 *  @param cmd_action   the action: GET or SET
 *  @param pdata_buf    A pointer to data buffer
 *  @return             MLAN_STATUS_SUCCESS
 */
mlan_status
wlan_cmd_rx_mgmt_indication(IN pmlan_private pmpriv,
		IN HostCmd_DS_COMMAND * cmd,
		IN t_u16 cmd_action, IN t_void * pdata_buf)
{
    mlan_ds_rx_mgmt_indication *rx_mgmt_indication =
	(mlan_ds_rx_mgmt_indication *) pdata_buf;
    HostCmd_DS_RX_MGMT_IND *rx_mgmt =
        (HostCmd_DS_RX_MGMT_IND *) & cmd->params.rx_mgmt_ind;

    ENTER();

    /* Set passthru mask for mgmt frame */
    pmpriv->mgmt_frame_passthru_mask = rx_mgmt_indication->mgmt_subtype_mask;

    cmd->command = wlan_cpu_to_le16(HostCmd_CMD_RX_MGMT_IND);
    cmd->size = wlan_cpu_to_le16(sizeof(HostCmd_DS_RX_MGMT_IND) + S_DS_GEN);

    rx_mgmt->action = wlan_cpu_to_le16(cmd_action);
    rx_mgmt->mgmt_subtype_mask =
		wlan_cpu_to_le32(rx_mgmt_indication->mgmt_subtype_mask);

    LEAVE();
    return MLAN_STATUS_SUCCESS;
}

/**
 *  @brief This function prepares command of bridge mode
 *
 *  @param cmd          A pointer to HostCmd_DS_COMMAND structure
 *  @param cmd_action   the action: GET or SET
 *  @param pdata_buf    A pointer to data buffer
 *  @return             MLAN_STATUS_SUCCESS
 */
mlan_status
wlan_cmd_bridge_mode(IN HostCmd_DS_COMMAND * cmd,
		IN t_u16 cmd_action, IN t_void * pdata_buf)
{
	mlan_bridge_mode *bridge_mode = (mlan_bridge_mode *) pdata_buf;
	HostCmd_BRIDGE_MODE *bg_mode =
		(HostCmd_BRIDGE_MODE *) & cmd->params.bridge_mode;

	ENTER();

	cmd->command = wlan_cpu_to_le16(HostCmd_CMD_BRIDGE_MODE);
	cmd->size = wlan_cpu_to_le16(sizeof(HostCmd_BRIDGE_MODE) + S_DS_GEN);

	bg_mode->action = wlan_cpu_to_le16(cmd_action);
	bg_mode->enable = wlan_cpu_to_le32(bridge_mode->enable);
	LEAVE();
	return MLAN_STATUS_SUCCESS;
}

#ifndef CONFIG_MLAN_WMSDK
/**
 *  @brief This function prepares command of subscribe event.
 *
 *  @param pmpriv    	A pointer to mlan_private structure
 *  @param cmd          A pointer to HostCmd_DS_COMMAND structure
 *  @param cmd_action   the action: GET or SET
 *  @param pdata_buf    A pointer to data buffer
 *  @return             MLAN_STATUS_SUCCESS
 */
static mlan_status
wlan_cmd_subscribe_event(IN pmlan_private pmpriv,
                         IN HostCmd_DS_COMMAND * cmd,
                         IN t_u16 cmd_action, IN t_void * pdata_buf)
{
    mlan_ds_subscribe_evt *sub_evt = (mlan_ds_subscribe_evt *) pdata_buf;
    HostCmd_DS_SUBSCRIBE_EVENT *evt =
        (HostCmd_DS_SUBSCRIBE_EVENT *) & cmd->params.subscribe_event;
    t_u16 cmd_size = 0;
    t_u8 *tlv = MNULL;
    MrvlIEtypes_BeaconLowRssiThreshold_t *rssi_low = MNULL;
    MrvlIEtypes_BeaconLowSnrThreshold_t *snr_low = MNULL;
    MrvlIEtypes_FailureCount_t *fail_count = MNULL;
    MrvlIEtypes_BeaconsMissed_t *beacon_missed = MNULL;
    MrvlIEtypes_BeaconHighRssiThreshold_t *rssi_high = MNULL;
    MrvlIEtypes_BeaconHighSnrThreshold_t *snr_high = MNULL;
    MrvlIEtypes_DataLowRssiThreshold_t *data_rssi_low = MNULL;
    MrvlIEtypes_DataLowSnrThreshold_t *data_snr_low = MNULL;
    MrvlIEtypes_DataHighRssiThreshold_t *data_rssi_high = MNULL;
    MrvlIEtypes_DataHighSnrThreshold_t *data_snr_high = MNULL;
    MrvlIEtypes_LinkQualityThreshold_t *link_quality = MNULL;
    MrvlIETypes_PreBeaconMissed_t *pre_bcn_missed = MNULL;

    ENTER();

    cmd->command = wlan_cpu_to_le16(HostCmd_CMD_802_11_SUBSCRIBE_EVENT);
    evt->action = wlan_cpu_to_le16(cmd_action);
    cmd_size = sizeof(HostCmd_DS_SUBSCRIBE_EVENT) + S_DS_GEN;
    if (cmd_action == HostCmd_ACT_GEN_GET)
        goto done;
#define HostCmd_ACT_BITWISE_SET	0x02
    evt->action = wlan_cpu_to_le16(HostCmd_ACT_BITWISE_SET);
    evt->event_bitmap = wlan_cpu_to_le16(sub_evt->evt_bitmap);
    tlv = (t_u8 *) cmd + cmd_size;
    if (sub_evt->evt_bitmap & SUBSCRIBE_EVT_RSSI_LOW) {
        rssi_low = (MrvlIEtypes_BeaconLowRssiThreshold_t *) tlv;
        rssi_low->header.type = wlan_cpu_to_le16(TLV_TYPE_RSSI_LOW);
        rssi_low->header.len =
            wlan_cpu_to_le16(sizeof(MrvlIEtypes_BeaconLowRssiThreshold_t) -
                             sizeof(MrvlIEtypesHeader_t));
        rssi_low->value = sub_evt->low_rssi;
        rssi_low->frequency = sub_evt->low_rssi_freq;
        tlv += sizeof(MrvlIEtypes_BeaconLowRssiThreshold_t);
        cmd_size += sizeof(MrvlIEtypes_BeaconLowRssiThreshold_t);
    }
    if (sub_evt->evt_bitmap & SUBSCRIBE_EVT_SNR_LOW) {
        snr_low = (MrvlIEtypes_BeaconLowSnrThreshold_t *) tlv;
        snr_low->header.type = wlan_cpu_to_le16(TLV_TYPE_SNR_LOW);
        snr_low->header.len =
            wlan_cpu_to_le16(sizeof(MrvlIEtypes_BeaconLowSnrThreshold_t) -
                             sizeof(MrvlIEtypesHeader_t));
        snr_low->value = sub_evt->low_snr;
        snr_low->frequency = sub_evt->low_snr_freq;
        tlv += sizeof(MrvlIEtypes_BeaconLowSnrThreshold_t);
        cmd_size += sizeof(MrvlIEtypes_BeaconLowSnrThreshold_t);
    }
    if (sub_evt->evt_bitmap & SUBSCRIBE_EVT_MAX_FAIL) {
        fail_count = (MrvlIEtypes_FailureCount_t *) tlv;
        fail_count->header.type = wlan_cpu_to_le16(TLV_TYPE_FAILCOUNT);
        fail_count->header.len =
            wlan_cpu_to_le16(sizeof(MrvlIEtypes_FailureCount_t) -
                             sizeof(MrvlIEtypesHeader_t));
        fail_count->value = sub_evt->failure_count;
        fail_count->frequency = sub_evt->failure_count_freq;
        tlv += sizeof(MrvlIEtypes_FailureCount_t);
        cmd_size += sizeof(MrvlIEtypes_FailureCount_t);
    }
    if (sub_evt->evt_bitmap & SUBSCRIBE_EVT_BEACON_MISSED) {
        beacon_missed = (MrvlIEtypes_BeaconsMissed_t *) tlv;
        beacon_missed->header.type = wlan_cpu_to_le16(TLV_TYPE_BCNMISS);
        beacon_missed->header.len =
            wlan_cpu_to_le16(sizeof(MrvlIEtypes_BeaconsMissed_t) -
                             sizeof(MrvlIEtypesHeader_t));
        beacon_missed->value = sub_evt->beacon_miss;
        beacon_missed->frequency = sub_evt->beacon_miss_freq;
        tlv += sizeof(MrvlIEtypes_BeaconsMissed_t);
        cmd_size += sizeof(MrvlIEtypes_BeaconsMissed_t);
    }
    if (sub_evt->evt_bitmap & SUBSCRIBE_EVT_RSSI_HIGH) {
        rssi_high = (MrvlIEtypes_BeaconHighRssiThreshold_t *) tlv;
        rssi_high->header.type = wlan_cpu_to_le16(TLV_TYPE_RSSI_HIGH);
        rssi_high->header.len =
            wlan_cpu_to_le16(sizeof(MrvlIEtypes_BeaconHighRssiThreshold_t) -
                             sizeof(MrvlIEtypesHeader_t));
        rssi_high->value = sub_evt->high_rssi;
        rssi_high->frequency = sub_evt->high_rssi_freq;
        tlv += sizeof(MrvlIEtypes_BeaconHighRssiThreshold_t);
        cmd_size += sizeof(MrvlIEtypes_BeaconHighRssiThreshold_t);
    }
    if (sub_evt->evt_bitmap & SUBSCRIBE_EVT_SNR_HIGH) {
        snr_high = (MrvlIEtypes_BeaconHighSnrThreshold_t *) tlv;
        snr_high->header.type = wlan_cpu_to_le16(TLV_TYPE_SNR_HIGH);
        snr_high->header.len =
            wlan_cpu_to_le16(sizeof(MrvlIEtypes_BeaconHighSnrThreshold_t) -
                             sizeof(MrvlIEtypesHeader_t));
        snr_high->value = sub_evt->high_snr;
        snr_high->frequency = sub_evt->high_snr_freq;
        tlv += sizeof(MrvlIEtypes_BeaconHighSnrThreshold_t);
        cmd_size += sizeof(MrvlIEtypes_BeaconHighSnrThreshold_t);
    }
    if (sub_evt->evt_bitmap & SUBSCRIBE_EVT_DATA_RSSI_LOW) {
        data_rssi_low = (MrvlIEtypes_DataLowRssiThreshold_t *) tlv;
        data_rssi_low->header.type = wlan_cpu_to_le16(TLV_TYPE_RSSI_LOW_DATA);
        data_rssi_low->header.len =
            wlan_cpu_to_le16(sizeof(MrvlIEtypes_DataLowRssiThreshold_t) -
                             sizeof(MrvlIEtypesHeader_t));
        data_rssi_low->value = sub_evt->data_low_rssi;
        data_rssi_low->frequency = sub_evt->data_low_rssi_freq;
        tlv += sizeof(MrvlIEtypes_DataLowRssiThreshold_t);
        cmd_size += sizeof(MrvlIEtypes_DataLowRssiThreshold_t);
    }
    if (sub_evt->evt_bitmap & SUBSCRIBE_EVT_DATA_SNR_LOW) {
        data_snr_low = (MrvlIEtypes_DataLowSnrThreshold_t *) tlv;
        data_snr_low->header.type = wlan_cpu_to_le16(TLV_TYPE_SNR_LOW_DATA);
        data_snr_low->header.len =
            wlan_cpu_to_le16(sizeof(MrvlIEtypes_DataLowSnrThreshold_t) -
                             sizeof(MrvlIEtypesHeader_t));
        data_snr_low->value = sub_evt->data_low_snr;
        data_snr_low->frequency = sub_evt->data_low_snr_freq;
        tlv += sizeof(MrvlIEtypes_DataLowSnrThreshold_t);
        cmd_size += sizeof(MrvlIEtypes_DataLowSnrThreshold_t);
    }
    if (sub_evt->evt_bitmap & SUBSCRIBE_EVT_DATA_RSSI_HIGH) {
        data_rssi_high = (MrvlIEtypes_DataHighRssiThreshold_t *) tlv;
        data_rssi_high->header.type = wlan_cpu_to_le16(TLV_TYPE_RSSI_HIGH_DATA);
        data_rssi_high->header.len =
            wlan_cpu_to_le16(sizeof(MrvlIEtypes_DataHighRssiThreshold_t) -
                             sizeof(MrvlIEtypesHeader_t));
        data_rssi_high->value = sub_evt->data_high_rssi;
        data_rssi_high->frequency = sub_evt->data_high_rssi_freq;
        tlv += sizeof(MrvlIEtypes_DataHighRssiThreshold_t);
        cmd_size += sizeof(MrvlIEtypes_DataHighRssiThreshold_t);
    }
    if (sub_evt->evt_bitmap & SUBSCRIBE_EVT_DATA_SNR_HIGH) {
        data_snr_high = (MrvlIEtypes_DataHighSnrThreshold_t *) tlv;
        data_snr_high->header.type = wlan_cpu_to_le16(TLV_TYPE_SNR_HIGH_DATA);
        data_snr_high->header.len =
            wlan_cpu_to_le16(sizeof(MrvlIEtypes_DataHighSnrThreshold_t) -
                             sizeof(MrvlIEtypesHeader_t));
        data_snr_high->value = sub_evt->data_high_snr;
        data_snr_high->frequency = sub_evt->data_high_snr_freq;
        tlv += sizeof(MrvlIEtypes_DataHighSnrThreshold_t);
        cmd_size += sizeof(MrvlIEtypes_DataHighSnrThreshold_t);
    }
    if (sub_evt->evt_bitmap & SUBSCRIBE_EVT_LINK_QUALITY) {
        link_quality = (MrvlIEtypes_LinkQualityThreshold_t *) tlv;
        link_quality->header.type = wlan_cpu_to_le16(TLV_TYPE_LINK_QUALITY);
        link_quality->header.len =
            wlan_cpu_to_le16(sizeof(MrvlIEtypes_LinkQualityThreshold_t) -
                             sizeof(MrvlIEtypesHeader_t));
        link_quality->link_snr = wlan_cpu_to_le16(sub_evt->link_snr);
        link_quality->link_snr_freq = wlan_cpu_to_le16(sub_evt->link_snr_freq);
        link_quality->link_rate = wlan_cpu_to_le16(sub_evt->link_rate);
        link_quality->link_rate_freq =
            wlan_cpu_to_le16(sub_evt->link_rate_freq);
        link_quality->link_tx_latency =
            wlan_cpu_to_le16(sub_evt->link_tx_latency);
        link_quality->link_tx_lantency_freq =
            wlan_cpu_to_le16(sub_evt->link_tx_lantency_freq);
        tlv += sizeof(MrvlIEtypes_LinkQualityThreshold_t);
        cmd_size += sizeof(MrvlIEtypes_LinkQualityThreshold_t);
    }
    if (sub_evt->evt_bitmap & SUBSCRIBE_EVT_PRE_BEACON_LOST) {
        pre_bcn_missed = (MrvlIETypes_PreBeaconMissed_t *) tlv;
        pre_bcn_missed->header.type = wlan_cpu_to_le16(TLV_TYPE_PRE_BCNMISS);
        pre_bcn_missed->header.len =
            wlan_cpu_to_le16(sizeof(MrvlIETypes_PreBeaconMissed_t) -
                             sizeof(MrvlIEtypesHeader_t));
        pre_bcn_missed->value = sub_evt->pre_beacon_miss;
        pre_bcn_missed->frequency = 0;
        tlv += sizeof(MrvlIETypes_PreBeaconMissed_t);
        cmd_size += sizeof(MrvlIETypes_PreBeaconMissed_t);
    }
  done:
    cmd->size = wlan_cpu_to_le16(cmd_size);
    LEAVE();
    return MLAN_STATUS_SUCCESS;
}
#endif /* CONFIG_MLAN_WMSDK */

/**
 *  @brief This function prepares command of OTP user data.
 *
 *  @param pmpriv    	A pointer to mlan_private structure
 *  @param cmd          A pointer to HostCmd_DS_COMMAND structure
 *  @param cmd_action   the action: GET or SET
 *  @param pdata_buf    A pointer to data buffer
 *  @return             MLAN_STATUS_SUCCESS
 */
static mlan_status
wlan_cmd_otp_user_data(IN pmlan_private pmpriv,
                       IN HostCmd_DS_COMMAND * cmd,
                       IN t_u16 cmd_action, IN t_void * pdata_buf)
{
    mlan_ds_misc_otp_user_data *user_data =
        (mlan_ds_misc_otp_user_data *) pdata_buf;
    HostCmd_DS_OTP_USER_DATA *cmd_user_data =
        (HostCmd_DS_OTP_USER_DATA *) & cmd->params.otp_user_data;
    t_u16 cmd_size = 0;

    ENTER();

    cmd->command = wlan_cpu_to_le16(HostCmd_CMD_OTP_READ_USER_DATA);
    cmd_size = sizeof(HostCmd_DS_OTP_USER_DATA) + S_DS_GEN - 1;

    cmd_user_data->action = wlan_cpu_to_le16(cmd_action);
    cmd_user_data->reserved = 0;
    cmd_user_data->user_data_length =
        wlan_cpu_to_le16(user_data->user_data_length);
    /* wmsdk: below change is added in order to count user_data_length size
     * for SET/Write operation only, in case GET/Read it is not required
     * (SDIO cmd size remains same).
     */
    if (cmd_action == HostCmd_ACT_GEN_SET)
	   cmd_size += user_data->user_data_length;
    cmd->size = wlan_cpu_to_le16(cmd_size);

    LEAVE();
    return MLAN_STATUS_SUCCESS;
}

#ifndef CONFIG_MLAN_WMSDK
/**
 *  @brief This function prepares inactivity timeout command
 *
 *  @param cmd          A pointer to HostCmd_DS_COMMAND structure
 *  @param cmd_action   the action: GET or SET
 *  @param pdata_buf    A pointer to data buffer
 *  @return             MLAN_STATUS_SUCCESS
 */
static mlan_status
wlan_cmd_inactivity_timeout(IN HostCmd_DS_COMMAND * cmd,
                            IN t_u16 cmd_action, IN t_void * pdata_buf)
{
    pmlan_ds_inactivity_to inac_to;
    HostCmd_DS_INACTIVITY_TIMEOUT_EXT *cmd_inac_to = &cmd->params.inactivity_to;

    ENTER();

    inac_to = (mlan_ds_inactivity_to *) pdata_buf;

    cmd->size =
        wlan_cpu_to_le16(sizeof(HostCmd_DS_INACTIVITY_TIMEOUT_EXT) + S_DS_GEN);
    cmd->command = wlan_cpu_to_le16(cmd->command);
    cmd_inac_to->action = wlan_cpu_to_le16(cmd_action);
    if (cmd_action == HostCmd_ACT_GEN_SET) {
        cmd_inac_to->timeout_unit =
            wlan_cpu_to_le16((t_u16) inac_to->timeout_unit);
        cmd_inac_to->unicast_timeout =
            wlan_cpu_to_le16((t_u16) inac_to->unicast_timeout);
        cmd_inac_to->mcast_timeout =
            wlan_cpu_to_le16((t_u16) inac_to->mcast_timeout);
        cmd_inac_to->ps_entry_timeout =
            wlan_cpu_to_le16((t_u16) inac_to->ps_entry_timeout);
    }

    LEAVE();
    return MLAN_STATUS_SUCCESS;
}

/**
 *  @brief This function prepares Low Power Mode
 *
 *  @param pmpriv       A pointer to mlan_private structure
 *  @param cmd          A pointer to HostCmd_DS_COMMAND structure
 *  @param cmd_action   the action: GET or SET
 *  @param pdata_buf    A pointer to data buffer
 *
 *  @return             MLAN_STATUS_SUCCESS
 */
static mlan_status wlan_cmd_low_pwr_mode(
    IN pmlan_private            pmpriv,
    IN HostCmd_DS_COMMAND       *cmd,
    IN t_void                   *pdata_buf)
{
    HostCmd_CONFIG_LOW_PWR_MODE *cmd_lpm_cfg = &cmd->params.low_pwr_mode_cfg;
    t_u8 *enable;

    ENTER();

    cmd->command = wlan_cpu_to_le16(HostCMD_CONFIG_LOW_POWER_MODE);
    cmd->size = S_DS_GEN + sizeof(HostCmd_CONFIG_LOW_PWR_MODE);

    enable = (t_u8 *) pdata_buf;
    cmd->size = wlan_cpu_to_le16(cmd->size);
    cmd->command = wlan_cpu_to_le16(cmd->command);
    cmd_lpm_cfg->enable = *enable;

    LEAVE();
    return MLAN_STATUS_SUCCESS;
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
wlan_ops_sta_prepare_cmd(IN t_void * priv,
                         IN t_u16 cmd_no,
                         IN t_u16 cmd_action,
                         IN t_u32 cmd_oid,
                         IN t_void * pioctl_buf,
                         IN t_void * pdata_buf, IN t_void * pcmd_buf)
{
    HostCmd_DS_COMMAND *cmd_ptr = (HostCmd_DS_COMMAND *) pcmd_buf;
    mlan_private *pmpriv = (mlan_private *) priv;
    mlan_status ret = MLAN_STATUS_SUCCESS;

    ENTER();

    /* Prepare command */
    switch (cmd_no) {
#ifndef CONFIG_MLAN_WMSDK
    case HostCmd_CMD_GET_HW_SPEC:
        ret = wlan_cmd_get_hw_spec(pmpriv, cmd_ptr);
        break;
    case HostCmd_CMD_CFG_DATA:
        ret = wlan_cmd_cfg_data(pmpriv, cmd_ptr, cmd_action, pdata_buf);
        break;
#endif /* CONFIG_MLAN_WMSDK */
    case HostCmd_CMD_MAC_CONTROL:
        ret = wlan_cmd_mac_control(pmpriv, cmd_ptr, cmd_action, pdata_buf);
        break;
    case HostCmd_CMD_802_11_MAC_ADDRESS:
        ret = wlan_cmd_802_11_mac_address(pmpriv, cmd_ptr, cmd_action,
					  pdata_buf);
        break;
    case HostCmd_CMD_MAC_MULTICAST_ADR:
        ret =
            wlan_cmd_mac_multicast_adr(pmpriv, cmd_ptr, cmd_action, pdata_buf);
        break;
    case HostCmd_CMD_TX_RATE_CFG:
        ret = wlan_cmd_tx_rate_cfg(pmpriv, cmd_ptr, cmd_action, pdata_buf);
        break;
    case HostCmd_CMD_802_11_RF_ANTENNA:
        ret =
            wlan_cmd_802_11_rf_antenna(pmpriv, cmd_ptr, cmd_action, pdata_buf);
        break;
    case HostCmd_CMD_802_11_NET_MONITOR:
        ret =
            wlan_cmd_802_11_net_monitor(pmpriv, cmd_ptr, cmd_action, pdata_buf);
        break;
    case HostCmd_CMD_TXPWR_CFG:
        ret = wlan_cmd_tx_power_cfg(pmpriv, cmd_ptr, cmd_action, pdata_buf);
        break;
    case HostCmd_CMD_802_11_RF_TX_POWER:
        ret = wlan_cmd_802_11_rf_tx_power(pmpriv, cmd_ptr,
                                          cmd_action, pdata_buf);
        break;
#ifndef CONFIG_MLAN_WMSDK
    case HostCmd_CMD_802_11_PS_MODE_ENH:
        ret =
            wlan_cmd_enh_power_mode(pmpriv, cmd_ptr, cmd_action,
                                    (t_u16) cmd_oid, pdata_buf);
        break;
#endif /* CONFIG_MLAN_WMSDK */
    case HostCmd_CMD_802_11_HS_CFG_ENH:
        ret =
            wlan_cmd_802_11_hs_cfg(pmpriv, cmd_ptr, cmd_action,
                                   (hs_config_param *) pdata_buf);
        break;
#ifndef CONFIG_MLAN_WMSDK
    case HostCmd_CMD_802_11_SLEEP_PERIOD:
        ret = wlan_cmd_802_11_sleep_period(pmpriv, cmd_ptr,
                                           cmd_action, (t_u16 *) pdata_buf);
        break;
    case HostCmd_CMD_802_11_SLEEP_PARAMS:
        ret = wlan_cmd_802_11_sleep_params(pmpriv, cmd_ptr,
                                           cmd_action, (t_u16 *) pdata_buf);
        break;
#endif /* CONFIG_MLAN_WMSDK */
#ifndef EXT_SCAN_SUPPORT
    case HostCmd_CMD_802_11_SCAN:
        ret = wlan_cmd_802_11_scan(pmpriv, cmd_ptr, pdata_buf);
        break;
#endif
#ifndef CONFIG_MLAN_WMSDK
    case HostCmd_CMD_802_11_BG_SCAN_CONFIG:
        ret = wlan_cmd_bgscan_config(pmpriv, cmd_ptr, pdata_buf);
        break;
    case HostCmd_CMD_802_11_BG_SCAN_QUERY:
        ret = wlan_cmd_802_11_bg_scan_query(pmpriv, cmd_ptr, pdata_buf);
        break;
#endif /* CONFIG_MLAN_WMSDK */
    case HostCmd_CMD_802_11_ASSOCIATE:
        ret = wlan_cmd_802_11_associate(pmpriv, cmd_ptr, pdata_buf);
        break;
#ifndef CONFIG_MLAN_WMSDK
    case HostCmd_CMD_802_11_DEAUTHENTICATE:
        ret = wlan_cmd_802_11_deauthenticate(pmpriv, cmd_ptr, pdata_buf);
        break;
    case HostCmd_CMD_802_11_AD_HOC_START:
        ret = wlan_cmd_802_11_ad_hoc_start(pmpriv, cmd_ptr, pdata_buf);
        break;
    case HostCmd_CMD_802_11_AD_HOC_JOIN:
        ret = wlan_cmd_802_11_ad_hoc_join(pmpriv, cmd_ptr, pdata_buf);
        break;
    case HostCmd_CMD_802_11_AD_HOC_STOP:
        ret = wlan_cmd_802_11_ad_hoc_stop(pmpriv, cmd_ptr);
        break;
#endif /* CONFIG_MLAN_WMSDK */
    case HostCmd_CMD_802_11_GET_LOG:
        ret = wlan_cmd_802_11_get_log(pmpriv, cmd_ptr);
        break;
    case HostCmd_CMD_RSSI_INFO:
        ret = wlan_cmd_802_11_rssi_info(pmpriv, cmd_ptr, cmd_action);
        break;
    case HostCmd_CMD_802_11_SNMP_MIB:
        ret =
            wlan_cmd_802_11_snmp_mib(pmpriv, cmd_ptr, cmd_action, cmd_oid,
                                     pdata_buf);
        break;
#ifndef CONFIG_MLAN_WMSDK
    case HostCmd_CMD_802_11_RADIO_CONTROL:
        ret =
            wlan_cmd_802_11_radio_control(pmpriv, cmd_ptr, cmd_action,
                                          pdata_buf);
        break;
#endif /* CONFIG_MLAN_WMSDK */
    case HostCmd_CMD_802_11_TX_RATE_QUERY:
        cmd_ptr->command = wlan_cpu_to_le16(HostCmd_CMD_802_11_TX_RATE_QUERY);
        cmd_ptr->size =
            wlan_cpu_to_le16(sizeof(HostCmd_TX_RATE_QUERY) + S_DS_GEN);
        pmpriv->tx_rate = 0;
        ret = MLAN_STATUS_SUCCESS;
        break;
    case HostCmd_CMD_VERSION_EXT:
	memset(pmpriv->adapter, cmd_ptr, 0x00, sizeof(HostCmd_DS_COMMAND));
        cmd_ptr->command = wlan_cpu_to_le16(cmd_no);
        cmd_ptr->params.verext.version_str_sel =
            * (t_u8 *) pdata_buf;
        cmd_ptr->size =
            wlan_cpu_to_le16(sizeof(HostCmd_DS_VERSION_EXT) + S_DS_GEN);
        ret = MLAN_STATUS_SUCCESS;
        break;
#ifndef CONFIG_MLAN_WMSDK
    case HostCmd_CMD_RX_MGMT_IND:
        cmd_ptr->command = wlan_cpu_to_le16(cmd_no);
        cmd_ptr->params.rx_mgmt_ind.action = wlan_cpu_to_le16(cmd_action);
        cmd_ptr->params.rx_mgmt_ind.mgmt_subtype_mask =
            wlan_cpu_to_le32((t_u32) (*((t_u32 *) pdata_buf)));
        cmd_ptr->size = wlan_cpu_to_le16(sizeof(t_u32) + S_DS_GEN);
        break;
#endif /* CONFIG_MLAN_WMSDK */
    case HostCmd_CMD_802_11_RF_CHANNEL:
        ret =
            wlan_cmd_802_11_rf_channel(pmpriv, cmd_ptr, cmd_action, pdata_buf);
        break;
#ifndef CONFIG_MLAN_WMSDK
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
    case HostCmd_CMD_SOFT_RESET:
        cmd_ptr->command = wlan_cpu_to_le16(cmd_no);
        cmd_ptr->size = wlan_cpu_to_le16(S_DS_GEN);
        break;
#endif /* CONFIG_MLAN_WMSDK */
    case HostCmd_CMD_11N_ADDBA_REQ:
        ret = wlan_cmd_11n_addba_req(pmpriv, cmd_ptr, pdata_buf);
        break;
    case HostCmd_CMD_11N_DELBA:
        ret = wlan_cmd_11n_delba(pmpriv, cmd_ptr, pdata_buf);
        break;
    case HostCmd_CMD_11N_ADDBA_RSP:
        ret = wlan_cmd_11n_addba_rspgen(pmpriv, cmd_ptr, pdata_buf);
        break;
#ifdef WPA
    case HostCmd_CMD_802_11_KEY_MATERIAL:
        ret = wlan_cmd_802_11_key_material(pmpriv, cmd_ptr, cmd_action,
					cmd_oid, pdata_buf);
        break;
#endif /* End of WPA */
    case HostCmd_CMD_SUPPLICANT_PMK:
        ret = wlan_cmd_802_11_supplicant_pmk(pmpriv, cmd_ptr, cmd_action,
                                             pdata_buf);
        break;
#ifndef CONFIG_MLAN_WMSDK
    case HostCmd_CMD_SUPPLICANT_PROFILE:
        ret = wlan_cmd_802_11_supplicant_profile(pmpriv, cmd_ptr, cmd_action,
                                                 pdata_buf);
        break;
#endif /* CONFIG_MLAN_WMSDK */
    case HostCmd_CMD_802_11D_DOMAIN_INFO:
	if (pmpriv->support_11d_APIs)
		ret = pmpriv->support_11d_APIs->
		wlan_cmd_802_11d_domain_info_p(pmpriv, cmd_ptr, cmd_action);
        break;
#ifndef CONFIG_MLAN_WMSDK
    case HostCmd_CMD_802_11_TPC_ADAPT_REQ:
    case HostCmd_CMD_802_11_TPC_INFO:
    case HostCmd_CMD_802_11_CHAN_SW_ANN:
    case HostCmd_CMD_CHAN_REPORT_REQUEST:
        ret = wlan_11h_cmd_process(pmpriv, cmd_ptr, pdata_buf);
        break;
    case HostCmd_CMD_RECONFIGURE_TX_BUFF:
        ret = wlan_cmd_recfg_tx_buf(pmpriv, cmd_ptr, cmd_action, pdata_buf);
        break;
    case HostCmd_CMD_AMSDU_AGGR_CTRL:
        ret = wlan_cmd_amsdu_aggr_ctrl(pmpriv, cmd_ptr, cmd_action, pdata_buf);
        break;
    case HostCmd_CMD_11N_CFG:
        ret = wlan_cmd_11n_cfg(pmpriv, cmd_ptr, cmd_action, pdata_buf);
        break;
    case HostCmd_CMD_WMM_GET_STATUS:
        PRINTM(MINFO, "WMM: WMM_GET_STATUS cmd sent\n");
        cmd_ptr->command = wlan_cpu_to_le16(HostCmd_CMD_WMM_GET_STATUS);
        cmd_ptr->size =
            wlan_cpu_to_le16(sizeof(HostCmd_DS_WMM_GET_STATUS) + S_DS_GEN);
        ret = MLAN_STATUS_SUCCESS;
        break;
    case HostCmd_CMD_WMM_ADDTS_REQ:
        ret = wlan_cmd_wmm_addts_req(pmpriv, cmd_ptr, pdata_buf);
        break;
    case HostCmd_CMD_WMM_DELTS_REQ:
        ret = wlan_cmd_wmm_delts_req(pmpriv, cmd_ptr, pdata_buf);
        break;
    case HostCmd_CMD_WMM_QUEUE_CONFIG:
        ret = wlan_cmd_wmm_queue_config(pmpriv, cmd_ptr, pdata_buf);
        break;
    case HostCmd_CMD_WMM_QUEUE_STATS:
        ret = wlan_cmd_wmm_queue_stats(pmpriv, cmd_ptr, pdata_buf);
        break;
    case HostCmd_CMD_WMM_TS_STATUS:
        ret = wlan_cmd_wmm_ts_status(pmpriv, cmd_ptr, pdata_buf);
        break;
    case HostCmd_CMD_802_11_IBSS_COALESCING_STATUS:
        ret =
            wlan_cmd_ibss_coalescing_status(pmpriv, cmd_ptr, cmd_action,
                                            pdata_buf);
        break;
#endif /* CONFIG_MLAN_WMSDK */
    case HostCmd_CMD_MGMT_IE_LIST:
        ret = wlan_cmd_mgmt_ie_list(pmpriv, cmd_ptr, cmd_action, pdata_buf);
        break;
#ifdef EXT_SCAN_SUPPORT
    case HostCmd_CMD_802_11_SCAN_EXT:
        ret = wlan_cmd_802_11_scan_ext(pmpriv, cmd_ptr, pdata_buf);
        break;
#endif
#ifndef CONFIG_MLAN_WMSDK
    case HostCmd_CMD_ECL_SYSTEM_CLOCK_CONFIG:
        ret = wlan_cmd_sysclock_cfg(pmpriv, cmd_ptr, cmd_action, pdata_buf);
        break;
    case HostCmd_CMD_MAC_REG_ACCESS:
    case HostCmd_CMD_BBP_REG_ACCESS:
    case HostCmd_CMD_RF_REG_ACCESS:
    case HostCmd_CMD_CAU_REG_ACCESS:
    case HostCmd_CMD_802_11_EEPROM_ACCESS:
        ret = wlan_cmd_reg_access(cmd_ptr, cmd_action, pdata_buf);
        break;
    case HostCmd_CMD_MEM_ACCESS:
        ret = wlan_cmd_mem_access(cmd_ptr, cmd_action, pdata_buf);
        break;
    case HostCmd_CMD_INACTIVITY_TIMEOUT_EXT:
        ret = wlan_cmd_inactivity_timeout(cmd_ptr, cmd_action, pdata_buf);
        break;
    case HostCmd_CMD_SDIO_GPIO_INT_CONFIG:
        ret = wlan_cmd_sdio_gpio_int(pmpriv, cmd_ptr, cmd_action, pdata_buf);
        break;
    case HostCmd_CMD_SET_BSS_MODE:
        cmd_ptr->command = wlan_cpu_to_le16(cmd_no);
#ifdef WIFI_DIRECT_SUPPORT
        if (pdata_buf) {
            cmd_ptr->params.bss_mode.con_type = *(t_u8 *) pdata_buf;
        } else
#endif
        if (pmpriv->bss_mode == MLAN_BSS_MODE_IBSS)
            cmd_ptr->params.bss_mode.con_type = CONNECTION_TYPE_ADHOC;
        else if (pmpriv->bss_mode == MLAN_BSS_MODE_INFRA)
            cmd_ptr->params.bss_mode.con_type = CONNECTION_TYPE_INFRA;
        cmd_ptr->size =
            wlan_cpu_to_le16(sizeof(HostCmd_DS_SET_BSS_MODE) + S_DS_GEN);
        ret = MLAN_STATUS_SUCCESS;
        break;
    case HostCmd_CMD_MEASUREMENT_REQUEST:
    case HostCmd_CMD_MEASUREMENT_REPORT:
        ret = wlan_meas_cmd_process(pmpriv, cmd_ptr, pdata_buf);
        break;
#endif /* CONFIG_MLAN_WMSDK */
    case HostCmd_CMD_802_11_REMAIN_ON_CHANNEL:
        ret =
            wlan_cmd_remain_on_channel(pmpriv, cmd_ptr, cmd_action, pdata_buf);
        break;
#ifndef CONFIG_MLAN_WMSDK
#ifdef WIFI_DIRECT_SUPPORT
    case HOST_CMD_WIFI_DIRECT_MODE_CONFIG:
        ret = wlan_cmd_wifi_direct_mode(pmpriv, cmd_ptr, cmd_action, pdata_buf);
        break;
#endif
    case HostCmd_CMD_802_11_SUBSCRIBE_EVENT:
        ret = wlan_cmd_subscribe_event(pmpriv, cmd_ptr, cmd_action, pdata_buf);
        break;
#endif /* CONFIG_MLAN_WMSDK */
    case HostCmd_CMD_OTP_READ_USER_DATA:
        ret = wlan_cmd_otp_user_data(pmpriv, cmd_ptr, cmd_action, pdata_buf);
        break;
#ifndef CONFIG_MLAN_WMSDK
    case HostCMD_CONFIG_LOW_POWER_MODE:
	ret = wlan_cmd_low_pwr_mode(pmpriv, cmd_ptr, pdata_buf);
	break;
#endif /* CONFIG_MLAN_WMSDK */
    default:
        PRINTM(MERROR, "PREP_CMD: unknown command- %#x\n", cmd_no);
        ret = MLAN_STATUS_FAILURE;
        break;
    }

    LEAVE();
    return ret;
}

#ifndef CONFIG_MLAN_WMSDK
/**
 *  @brief  This function issues commands to initialize firmware
 *
 *  @param priv     	A pointer to mlan_private structure
 *  @param first_bss	flag for first BSS
 *
 *  @return		MLAN_STATUS_PENDING or MLAN_STATUS_FAILURE
 */
mlan_status
wlan_ops_sta_init_cmd(IN t_void * priv, IN t_u8 first_bss)
{
    pmlan_private pmpriv = (pmlan_private) priv;
    mlan_status ret = MLAN_STATUS_SUCCESS;
    t_u16 enable = MTRUE;
    mlan_ds_11n_amsdu_aggr_ctrl amsdu_aggr_ctrl;

    ENTER();

    if (first_bss == MTRUE) {
        ret = wlan_adapter_init_cmd(pmpriv->adapter);
        if (ret == MLAN_STATUS_FAILURE)
            goto done;
    }

    /* get tx rate */
    ret = wlan_prepare_cmd(pmpriv,
                           HostCmd_CMD_TX_RATE_CFG,
                           HostCmd_ACT_GEN_GET, 0, MNULL, MNULL);
    if (ret) {
        ret = MLAN_STATUS_FAILURE;
        goto done;
    }
    pmpriv->data_rate = 0;

    /* get tx power */
    ret = wlan_prepare_cmd(pmpriv,
                           HostCmd_CMD_802_11_RF_TX_POWER,
                           HostCmd_ACT_GEN_GET, 0, MNULL, MNULL);
    if (ret) {
        ret = MLAN_STATUS_FAILURE;
        goto done;
    }

    /* set ibss coalescing_status */
    ret = wlan_prepare_cmd(pmpriv, HostCmd_CMD_802_11_IBSS_COALESCING_STATUS,
                           HostCmd_ACT_GEN_SET, 0, MNULL, &enable);
    if (ret) {
        ret = MLAN_STATUS_FAILURE;
        goto done;
    }

    memset(pmpriv->adapter, &amsdu_aggr_ctrl, 0, sizeof(amsdu_aggr_ctrl));
    amsdu_aggr_ctrl.enable = MLAN_ACT_ENABLE;
    /* Send request to firmware */
    ret = wlan_prepare_cmd(pmpriv, HostCmd_CMD_AMSDU_AGGR_CTRL,
                           HostCmd_ACT_GEN_SET, 0, MNULL,
                           (t_void *) & amsdu_aggr_ctrl);
    if (ret) {
        ret = MLAN_STATUS_FAILURE;
        goto done;
    }
    /* MAC Control must be the last command in init_fw */
    /* set MAC Control */
    ret = wlan_prepare_cmd(pmpriv, HostCmd_CMD_MAC_CONTROL,
                           HostCmd_ACT_GEN_SET, 0, MNULL,
                           &pmpriv->curr_pkt_filter);
    if (ret) {
        ret = MLAN_STATUS_FAILURE;
        goto done;
    }
    /** set last_init_cmd */
    pmpriv->adapter->last_init_cmd = HostCmd_CMD_MAC_CONTROL;

    if (first_bss == MFALSE) {
        /* Get MAC address */
        ret = wlan_prepare_cmd(pmpriv,
                               HostCmd_CMD_802_11_MAC_ADDRESS,
                               HostCmd_ACT_GEN_GET, 0, MNULL, MNULL);
        if (ret) {
            ret = MLAN_STATUS_FAILURE;
            goto done;
        }
        pmpriv->adapter->last_init_cmd = HostCmd_CMD_802_11_MAC_ADDRESS;
    }

    ret = MLAN_STATUS_PENDING;
  done:
    LEAVE();
    return ret;
}
#endif /* CONFIG_MLAN_WMSDK */
