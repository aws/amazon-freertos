/** @file mlan_cmdevt.c
 *
 *  @brief  This file provides the handling of CMD/EVENT in MLAN
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


/*************************************************************
Change Log:
    05/12/2009: initial version
************************************************************/
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

/*******************************************************
                Global Variables
********************************************************/

/********************************************************
                Local Functions
********************************************************/

#ifndef CONFIG_MLAN_WMSDK
/**
 *  @brief This function convert a given character to hex
 *
 *  @param chr        Character to be converted
 *
 *  @return           The converted hex if chr is a valid hex, else 0
 */
static t_u32
wlan_hexval(t_u8 chr)
{
    if (chr >= '0' && chr <= '9')
        return chr - '0';
    if (chr >= 'A' && chr <= 'F')
        return chr - 'A' + 10;
    if (chr >= 'a' && chr <= 'f')
        return chr - 'a' + 10;

    return 0;
}

/**
 *  @brief This function convert a given string to hex
 *
 *  @param a            A pointer to string to be converted
 *
 *  @return             The converted hex value if param a is a valid hex, else 0
 */
int
wlan_atox(t_u8 * a)
{
    int i = 0;

    ENTER();

    while (wlan_isxdigit(*a))
        i = i * 16 + wlan_hexval(*a++);

    LEAVE();
    return i;
}

/**
 *  @brief This function parse cal data from ASCII to hex
 *
 *  @param src          A pointer to source data
 *  @param len          Source dara length
 *  @param dst          A pointer to a buf to store the parsed data
 *
 *  @return             The parsed hex data length
 */
static t_u32
wlan_parse_cal_cfg(t_u8 * src, t_size len, t_u8 * dst)
{
    t_u8 *ptr;
    t_u8 *dptr;

    ENTER();
    ptr = src;
    dptr = dst;

    while (ptr - src < len) {
        while (*ptr && (wlan_isspace(*ptr) || *ptr == '\t')) {
            ptr++;
        }

        if (wlan_isxdigit(*ptr)) {
            *dptr++ = wlan_atox(ptr);
            ptr += 2;
        } else {
            ptr++;
        }
    }
    LEAVE();
    return (dptr - dst);
}

/**
 *  @brief This function initializes the command node.
 *
 *  @param pmpriv       A pointer to mlan_private structure
 *  @param pcmd_node    A pointer to cmd_ctrl_node structure
 *  @param cmd_oid      Cmd oid: treated as sub command
 *  @param pioctl_buf   A pointer to MLAN IOCTL Request buffer
 *  @param pdata_buf    A pointer to information buffer
 *
 *  @return             N/A
 */
static void
wlan_init_cmd_node(IN pmlan_private pmpriv,
                   IN cmd_ctrl_node * pcmd_node,
                   IN t_u32 cmd_oid,
                   IN t_void * pioctl_buf, IN t_void * pdata_buf)
{
    mlan_adapter *pmadapter = pmpriv->adapter;

    ENTER();

    if (pcmd_node == MNULL) {
        LEAVE();
        return;
    }
    pcmd_node->priv = pmpriv;
    pcmd_node->cmd_oid = cmd_oid;
    pcmd_node->pioctl_buf = pioctl_buf;
    pcmd_node->pdata_buf = pdata_buf;

    pcmd_node->cmdbuf = pcmd_node->pmbuf;

    /* Make sure head_ptr for cmd buf is Align */
    pcmd_node->cmdbuf->data_offset = 0;
    memset(pmadapter, pcmd_node->cmdbuf->pbuf, 0, MRVDRV_SIZE_OF_CMD_BUFFER);

    /* Prepare mlan_buffer for command sending */
    pcmd_node->cmdbuf->buf_type = MLAN_BUF_TYPE_CMD;
    pcmd_node->cmdbuf->data_offset += INTF_HEADER_LEN;

    LEAVE();
}

/**
 *  @brief This function gets a free command node if available in
 *              command free queue.
 *
 *  @param pmadapter        A pointer to mlan_adapter structure
 *
 *  @return cmd_ctrl_node   A pointer to cmd_ctrl_node structure or MNULL
 */
static cmd_ctrl_node *
wlan_get_cmd_node(mlan_adapter * pmadapter)
{
    cmd_ctrl_node *pcmd_node;

    ENTER();

    if (pmadapter == MNULL) {
        LEAVE();
        return MNULL;
    }

    if (util_peek_list(pmadapter->pmoal_handle, &pmadapter->cmd_free_q,
                       pmadapter->callbacks.moal_spin_lock,
                       pmadapter->callbacks.moal_spin_unlock)) {
        pcmd_node = (cmd_ctrl_node *) util_dequeue_list(pmadapter->pmoal_handle,
                                                        &pmadapter->cmd_free_q,
                                                        pmadapter->callbacks.
                                                        moal_spin_lock,
                                                        pmadapter->callbacks.
                                                        moal_spin_unlock);
    } else {
        PRINTM(MERROR, "GET_CMD_NODE: cmd_ctrl_node is not available\n");
        pcmd_node = MNULL;
    }

    LEAVE();
    return pcmd_node;
}

/**
 *  @brief This function cleans command node.
 *
 *  @param pmadapter    A pointer to mlan_adapter structure
 *  @param pcmd_node    A pointer to cmd_ctrl_node structure
 *
 *  @return             N/A
 */
static t_void
wlan_clean_cmd_node(pmlan_adapter pmadapter, cmd_ctrl_node * pcmd_node)
{
    ENTER();

    if (pcmd_node == MNULL) {
        LEAVE();
        return;
    }
    pcmd_node->cmd_oid = 0;
    pcmd_node->cmd_flag = 0;
    pcmd_node->pioctl_buf = MNULL;
    pcmd_node->pdata_buf = MNULL;

    if (pcmd_node->respbuf) {
        wlan_free_mlan_buffer(pmadapter, pcmd_node->respbuf);
        pcmd_node->respbuf = MNULL;
    }

    LEAVE();
    return;
}

/**
 *  @brief This function will return the pointer to the first entry in
 *  		pending cmd which matches the given pioctl_req
 *
 *  @param pmadapter    A pointer to mlan_adapter
 *  @param pioctl_req   A pointer to mlan_ioctl_req buf
 *
 *  @return 	   A pointer to first entry match pioctl_req
 */
static cmd_ctrl_node *
wlan_get_pending_ioctl_cmd(pmlan_adapter pmadapter, pmlan_ioctl_req pioctl_req)
{
    cmd_ctrl_node *pcmd_node = MNULL;

    ENTER();

    if (!
        (pcmd_node =
         (cmd_ctrl_node *) util_peek_list(pmadapter->pmoal_handle,
                                          &pmadapter->cmd_pending_q,
                                          pmadapter->callbacks.moal_spin_lock,
                                          pmadapter->callbacks.
                                          moal_spin_unlock))) {
        LEAVE();
        return MNULL;
    }
    while (pcmd_node != (cmd_ctrl_node *) & pmadapter->cmd_pending_q) {
        if (pcmd_node->pioctl_buf == pioctl_req) {
            LEAVE();
            return pcmd_node;
        }
        pcmd_node = pcmd_node->pnext;
    }
    LEAVE();
    return MNULL;
}

/**
 *  @brief This function handles the command response of host_cmd
 *
 *  @param pmpriv       A pointer to mlan_private structure
 *  @param resp         A pointer to HostCmd_DS_COMMAND
 *  @param pioctl_buf   A pointer to mlan_ioctl_req structure
 *
 *  @return        MLAN_STATUS_SUCCESS
 */
static mlan_status
wlan_ret_host_cmd(IN pmlan_private pmpriv,
                  IN HostCmd_DS_COMMAND * resp, IN mlan_ioctl_req * pioctl_buf)
{
    mlan_ds_misc_cfg *misc;
    t_u16 size = wlan_le16_to_cpu(resp->size);

    ENTER();

    PRINTM(MINFO, "host command response size = %d\n", size);
    size = MIN(size, MRVDRV_SIZE_OF_CMD_BUFFER);
    if (pioctl_buf) {
        misc = (mlan_ds_misc_cfg *) pioctl_buf->pbuf;
        misc->param.hostcmd.len = size;
        memcpy(pmpriv->adapter, misc->param.hostcmd.cmd, (void *) resp, size);
    }

    LEAVE();
    return MLAN_STATUS_SUCCESS;
}

/**
 *  @brief This function sends host command to firmware.
 *
 *  @param pmpriv     	A pointer to mlan_private structure
 *  @param cmd      	A pointer to HostCmd_DS_COMMAND structure
 *  @param pdata_buf	A pointer to data buffer
 *  @return         	MLAN_STATUS_SUCCESS
 */
static mlan_status
wlan_cmd_host_cmd(IN pmlan_private pmpriv,
                  IN HostCmd_DS_COMMAND * cmd, IN t_void * pdata_buf)
{
    mlan_ds_misc_cmd *pcmd_ptr = (mlan_ds_misc_cmd *) pdata_buf;

    ENTER();

    /* Copy the HOST command to command buffer */
    memcpy(pmpriv->adapter, (void *) cmd, pcmd_ptr->cmd,
           MIN(MRVDRV_SIZE_OF_CMD_BUFFER, pcmd_ptr->len));
    PRINTM(MINFO, "Host command size = %d\n", pcmd_ptr->len);
    LEAVE();
    return MLAN_STATUS_SUCCESS;
}

/**
 *  @brief This function downloads a command to firmware.
 *
 *  @param pmpriv       A pointer to mlan_private structure
 *  @param pcmd_node    A pointer to cmd_ctrl_node structure
 *
 *  @return             MLAN_STATUS_SUCCESS or MLAN_STATUS_FAILURE
 */
static mlan_status
wlan_dnld_cmd_to_fw(IN mlan_private * pmpriv, IN cmd_ctrl_node * pcmd_node)
{

    mlan_adapter *pmadapter = pmpriv->adapter;
    mlan_callbacks *pcb = (mlan_callbacks *) & pmadapter->callbacks;
    mlan_status ret = MLAN_STATUS_SUCCESS;
    HostCmd_DS_COMMAND *pcmd;
    mlan_ioctl_req *pioctl_buf = MNULL;
    t_u16 cmd_code;
    t_u16 cmd_size;
#ifdef DEBUG_LEVEL1
    t_u32 sec, usec;
#endif

    ENTER();

    if (pcmd_node)
        if (pcmd_node->pioctl_buf != MNULL)
            pioctl_buf = (mlan_ioctl_req *) pcmd_node->pioctl_buf;
    if (!pmadapter || !pcmd_node) {
        if (pioctl_buf)
            pioctl_buf->status_code = MLAN_ERROR_CMD_DNLD_FAIL;
        ret = MLAN_STATUS_FAILURE;
        goto done;
    }

    pcmd =
        (HostCmd_DS_COMMAND *) (pcmd_node->cmdbuf->pbuf +
                                pcmd_node->cmdbuf->data_offset);

    /* Sanity test */
    if (pcmd == MNULL || pcmd->size == 0) {
        PRINTM(MERROR, "DNLD_CMD: pcmd is null or command size is zero, "
               "Not sending\n");
        if (pioctl_buf)
            pioctl_buf->status_code = MLAN_ERROR_CMD_DNLD_FAIL;
        wlan_insert_cmd_to_free_q(pmadapter, pcmd_node);
        ret = MLAN_STATUS_FAILURE;
        goto done;
    }

    /* Set command sequence number */
    pmadapter->seq_num++;
    pcmd->seq_num =
        wlan_cpu_to_le16(HostCmd_SET_SEQ_NO_BSS_INFO
                         (pmadapter->seq_num, pcmd_node->priv->bss_num,
                          pcmd_node->priv->bss_type));

    wlan_request_cmd_lock(pmadapter);
    pmadapter->curr_cmd = pcmd_node;
    wlan_release_cmd_lock(pmadapter);

    cmd_code = wlan_le16_to_cpu(pcmd->command);
    cmd_size = wlan_le16_to_cpu(pcmd->size);

    pcmd_node->cmdbuf->data_len = cmd_size;

    PRINTM_GET_SYS_TIME(MCMND, &sec, &usec);
    PRINTM_NETINTF(MCMND, pmpriv);
    PRINTM(MCMND, "DNLD_CMD (%lu.%06lu): 0x%x, act 0x%x, len %d, seqno 0x%x\n",
           sec, usec, cmd_code,
           wlan_le16_to_cpu(*(t_u16 *) ((t_u8 *) pcmd + S_DS_GEN)), cmd_size,
           wlan_le16_to_cpu(pcmd->seq_num));
    DBG_HEXDUMP(MCMD_D, "DNLD_CMD", (t_u8 *) pcmd, cmd_size);

    /* Send the command to lower layer */

    pcmd_node->cmdbuf->data_offset -= INTF_HEADER_LEN;
    pcmd_node->cmdbuf->data_len += INTF_HEADER_LEN;
    /* Extra header for SDIO is added here */
    ret =
        wlan_sdio_host_to_card(pmadapter, MLAN_TYPE_CMD, pcmd_node->cmdbuf,
                               MNULL);

    if (ret == MLAN_STATUS_FAILURE) {
        PRINTM(MERROR, "DNLD_CMD: Host to Card Failed\n");
        if (pioctl_buf)
            pioctl_buf->status_code = MLAN_ERROR_CMD_DNLD_FAIL;
        wlan_insert_cmd_to_free_q(pmadapter, pmadapter->curr_cmd);

        wlan_request_cmd_lock(pmadapter);
        pmadapter->curr_cmd = MNULL;
        wlan_release_cmd_lock(pmadapter);

        pmadapter->dbg.num_cmd_host_to_card_failure++;
        ret = MLAN_STATUS_FAILURE;
        goto done;
    }

    /* Save the last command id and action to debug log */
    pmadapter->dbg.last_cmd_index =
        (pmadapter->dbg.last_cmd_index + 1) % DBG_CMD_NUM;
    pmadapter->dbg.last_cmd_id[pmadapter->dbg.last_cmd_index] = cmd_code;
    pmadapter->dbg.last_cmd_act[pmadapter->dbg.last_cmd_index] =
        wlan_le16_to_cpu(*(t_u16 *) ((t_u8 *) pcmd + S_DS_GEN));

    /* Clear BSS_NO_BITS from HostCmd */
    cmd_code &= HostCmd_CMD_ID_MASK;
    /* soft_reset command has no command response, we should return here */
    if (cmd_code == HostCmd_CMD_SOFT_RESET) {
        PRINTM(MCMND, "DNLD_CMD: SoftReset\n");
        if (pioctl_buf) {
            wlan_request_cmd_lock(pmadapter);
            pmadapter->curr_cmd->pioctl_buf = MNULL;
            wlan_release_cmd_lock(pmadapter);
            pcb->moal_ioctl_complete(pmadapter->pmoal_handle, pioctl_buf,
                                     MLAN_STATUS_SUCCESS);
        }
        goto done;
    }

    /* Setup the timer after transmit command */
    pcb->moal_start_timer(pmadapter->pmoal_handle, pmadapter->pmlan_cmd_timer,
                          MFALSE, MRVDRV_TIMER_60S);

    pmadapter->cmd_timer_is_set = MTRUE;

    ret = MLAN_STATUS_SUCCESS;

  done:
    LEAVE();
    return ret;
}

/**
 *  @brief This function sends sleep confirm command to firmware.
 *
 *  @param pmadapter  A pointer to mlan_adapter structure
 *
 *  @return        MLAN_STATUS_SUCCESS or MLAN_STATUS_FAILURE
 */
static mlan_status
wlan_dnld_sleep_confirm_cmd(mlan_adapter * pmadapter)
{
    mlan_status ret = MLAN_STATUS_SUCCESS;
    static t_u32 i = 0;
    t_u16 cmd_len = 0;
    opt_sleep_confirm_buffer *sleep_cfm_buf =
        (opt_sleep_confirm_buffer *) (pmadapter->psleep_cfm->pbuf +
                                      pmadapter->psleep_cfm->data_offset);

    ENTER();

    cmd_len = sizeof(OPT_Confirm_Sleep);
    pmadapter->seq_num++;
    sleep_cfm_buf->ps_cfm_sleep.seq_num =
        wlan_cpu_to_le16(HostCmd_SET_SEQ_NO_BSS_INFO
                         (pmadapter->seq_num,
                          (wlan_get_priv(pmadapter, MLAN_BSS_ROLE_ANY))->
                          bss_num,
                          (wlan_get_priv(pmadapter, MLAN_BSS_ROLE_ANY))->
                          bss_type));
    DBG_HEXDUMP(MCMD_D, "SLEEP_CFM", &sleep_cfm_buf->ps_cfm_sleep,
                sizeof(OPT_Confirm_Sleep));

    /* Send sleep confirm command to firmware */

    pmadapter->psleep_cfm->data_len = cmd_len + INTF_HEADER_LEN;
    ret = wlan_sdio_host_to_card(pmadapter, MLAN_TYPE_CMD,
                                 pmadapter->psleep_cfm, MNULL);

    if (ret == MLAN_STATUS_FAILURE) {
        PRINTM(MERROR, "SLEEP_CFM: failed\n");
        pmadapter->dbg.num_cmd_sleep_cfm_host_to_card_failure++;
        goto done;
    } else {
        if (GET_BSS_ROLE(wlan_get_priv(pmadapter, MLAN_BSS_ROLE_ANY)) ==
            MLAN_BSS_ROLE_UAP)
            pmadapter->ps_state = PS_STATE_SLEEP_CFM;
#ifdef STA_SUPPORT
        if (GET_BSS_ROLE(wlan_get_priv(pmadapter, MLAN_BSS_ROLE_ANY)) ==
            MLAN_BSS_ROLE_STA) {
            if (!sleep_cfm_buf->ps_cfm_sleep.sleep_cfm.resp_ctrl) {
                /* Response is not needed for sleep confirm command */
                pmadapter->ps_state = PS_STATE_SLEEP;
            } else {
                pmadapter->ps_state = PS_STATE_SLEEP_CFM;
            }

            if (!sleep_cfm_buf->ps_cfm_sleep.sleep_cfm.resp_ctrl
                && (pmadapter->is_hs_configured &&
                    !pmadapter->sleep_period.period)) {
                pmadapter->pm_wakeup_card_req = MTRUE;
                wlan_host_sleep_activated_event(wlan_get_priv
                                                (pmadapter, MLAN_BSS_ROLE_STA),
                                                MTRUE);
            }
        }
#endif /* STA_SUPPORT */

#define NUM_SC_PER_LINE         16
        if (++i % NUM_SC_PER_LINE == 0)
            PRINTM(MEVENT, "+\n");
        else
            PRINTM(MEVENT, "+");
    }

  done:
    LEAVE();
    return ret;
}

/********************************************************
                Global Functions
********************************************************/

/**
 *  @brief Event handler
 *
 *  @param priv		A pointer to mlan_private structure
 *  @param event_id	Event ID
 *  @param pmevent	Event buffer
 *
 *  @return		MLAN_STATUS_SUCCESS
 */
mlan_status
wlan_recv_event(pmlan_private priv, mlan_event_id event_id, t_void * pmevent)
{
    pmlan_callbacks pcb = &priv->adapter->callbacks;

    ENTER();

    if (pmevent)
        /* The caller has provided the event. */
        pcb->moal_recv_event(priv->adapter->pmoal_handle,
                             (pmlan_event) pmevent);
    else {
        mlan_event mevent;

        memset(priv->adapter, &mevent, 0, sizeof(mlan_event));
        mevent.bss_index = priv->bss_index;
        mevent.event_id = event_id;
        mevent.event_len = 0;

        pcb->moal_recv_event(priv->adapter->pmoal_handle, &mevent);
    }

    LEAVE();
    return MLAN_STATUS_SUCCESS;
}

/**
 *  @brief This function allocates the command buffer and links
 *              it to command free queue.
 *
 *  @param pmadapter    A pointer to mlan_adapter structure
 *
 *  @return             MLAN_STATUS_SUCCESS or MLAN_STATUS_FAILURE
 */
mlan_status
wlan_alloc_cmd_buffer(IN mlan_adapter * pmadapter)
{
    mlan_status ret = MLAN_STATUS_SUCCESS;
    mlan_callbacks *pcb = (mlan_callbacks *) & pmadapter->callbacks;
    cmd_ctrl_node *pcmd_array = MNULL;
    t_u32 buf_size;
    t_u32 i;

    ENTER();

    /* Allocate and initialize cmd_ctrl_node */
    buf_size = sizeof(cmd_ctrl_node) * MRVDRV_NUM_OF_CMD_BUFFER;
    ret =
        pcb->moal_malloc(pmadapter->pmoal_handle, buf_size,
                         MLAN_MEM_DEF | MLAN_MEM_DMA, (t_u8 **) & pcmd_array);
    if (ret != MLAN_STATUS_SUCCESS || !pcmd_array) {
        PRINTM(MERROR, "ALLOC_CMD_BUF: Failed to allocate pcmd_array\n");
        ret = MLAN_STATUS_FAILURE;
        goto done;
    }

    pmadapter->cmd_pool = pcmd_array;
    memset(pmadapter, pmadapter->cmd_pool, 0, buf_size);

    /* Allocate and initialize command buffers */
    for (i = 0; i < MRVDRV_NUM_OF_CMD_BUFFER; i++) {
        if (!(pcmd_array[i].pmbuf = wlan_alloc_mlan_buffer(pmadapter,
                                                           MRVDRV_SIZE_OF_CMD_BUFFER,
                                                           0, MTRUE))) {
            PRINTM(MERROR,
                   "ALLOC_CMD_BUF: Failed to allocate command buffer\n");
            ret = MLAN_STATUS_FAILURE;
            goto done;
        }
    }

    for (i = 0; i < MRVDRV_NUM_OF_CMD_BUFFER; i++) {
        wlan_insert_cmd_to_free_q(pmadapter, &pcmd_array[i]);
    }
    ret = MLAN_STATUS_SUCCESS;
  done:
    LEAVE();
    return ret;
}

/**
 *  @brief This function frees the command buffer.
 *
 *  @param pmadapter    A pointer to mlan_adapter structure
 *
 *  @return             MLAN_STATUS_SUCCESS
 */
mlan_status
wlan_free_cmd_buffer(IN mlan_adapter * pmadapter)
{
    mlan_callbacks *pcb = (mlan_callbacks *) & pmadapter->callbacks;
    cmd_ctrl_node *pcmd_array;
    t_u32 i;

    ENTER();

    /* Need to check if cmd pool is allocated or not */
    if (pmadapter->cmd_pool == MNULL) {
        PRINTM(MINFO, "FREE_CMD_BUF: cmd_pool is Null\n");
        goto done;
    }

    pcmd_array = pmadapter->cmd_pool;

    /* Release shared memory buffers */
    for (i = 0; i < MRVDRV_NUM_OF_CMD_BUFFER; i++) {
        if (pcmd_array[i].pmbuf) {
            PRINTM(MINFO, "Free all the command buffer.\n");
            wlan_free_mlan_buffer(pmadapter, pcmd_array[i].pmbuf);
            pcmd_array[i].pmbuf = MNULL;
        }
        if (pcmd_array[i].respbuf) {
            wlan_free_mlan_buffer(pmadapter, pcmd_array[i].respbuf);
            pcmd_array[i].respbuf = MNULL;
        }
    }
    /* Release cmd_ctrl_node */
    if (pmadapter->cmd_pool) {
        PRINTM(MINFO, "Free command pool.\n");
        pcb->moal_mfree(pmadapter->pmoal_handle, (t_u8 *) pmadapter->cmd_pool);
        pmadapter->cmd_pool = MNULL;
    }

  done:
    LEAVE();
    return MLAN_STATUS_SUCCESS;
}

/**
 *  @brief This function handles events generated by firmware
 *
 *  @param pmadapter		A pointer to mlan_adapter structure
 *
 *  @return		MLAN_STATUS_SUCCESS or MLAN_STATUS_FAILURE
 */
mlan_status
wlan_process_event(pmlan_adapter pmadapter)
{
    mlan_status ret = MLAN_STATUS_SUCCESS;
    pmlan_private priv = wlan_get_priv(pmadapter, MLAN_BSS_ROLE_ANY);
    pmlan_buffer pmbuf = pmadapter->pmlan_buffer_event;
    t_u32 eventcause = pmadapter->event_cause;
#ifdef DEBUG_LEVEL1
    t_u32 in_ts_sec, in_ts_usec;
#endif
    ENTER();

    /* Save the last event to debug log */
    pmadapter->dbg.last_event_index =
        (pmadapter->dbg.last_event_index + 1) % DBG_CMD_NUM;
    pmadapter->dbg.last_event[pmadapter->dbg.last_event_index] =
        (t_u16) eventcause;

    if ((eventcause & EVENT_ID_MASK) == EVENT_RADAR_DETECTED) {
        if (wlan_11h_dfs_event_preprocessing(pmadapter) == MLAN_STATUS_SUCCESS) {
            memcpy(pmadapter, (t_u8 *) & eventcause,
                   pmbuf->pbuf + pmbuf->data_offset, sizeof(eventcause));
        }
    }
    /* Get BSS number and corresponding priv */
    priv =
        wlan_get_priv_by_id(pmadapter, EVENT_GET_BSS_NUM(eventcause),
                            EVENT_GET_BSS_TYPE(eventcause));
    if (!priv)
        priv = wlan_get_priv(pmadapter, MLAN_BSS_ROLE_ANY);

    /* Clear BSS_NO_BITS from event */
    eventcause &= EVENT_ID_MASK;
    pmadapter->event_cause = eventcause;

    if (pmbuf) {
        pmbuf->bss_index = priv->bss_index;
        memcpy(pmadapter,
               pmbuf->pbuf + pmbuf->data_offset,
               (t_u8 *) & eventcause, sizeof(eventcause));
    }

    if (MTRUE && (eventcause != EVENT_PS_SLEEP && eventcause != EVENT_PS_AWAKE)
        ) {
        PRINTM_GET_SYS_TIME(MEVENT, &in_ts_sec, &in_ts_usec);
        PRINTM_NETINTF(MEVENT, priv);
        PRINTM(MEVENT, "%lu.%06lu : Event: 0x%x\n", in_ts_sec, in_ts_usec,
               eventcause);
    }

    ret = priv->ops.process_event(priv);

    pmadapter->event_cause = 0;
    pmadapter->pmlan_buffer_event = MNULL;
    if (pmbuf) {
        wlan_free_mlan_buffer(pmadapter, pmbuf);
    }

    LEAVE();
    return ret;
}

/**
 *  @brief This function requests a lock on command queue.
 *
 *  @param pmadapter    A pointer to mlan_adapter structure
 *
 *  @return             N/A
 */
t_void
wlan_request_cmd_lock(IN mlan_adapter * pmadapter)
{
    mlan_callbacks *pcb = (mlan_callbacks *) & pmadapter->callbacks;

    ENTER();

    /* Call MOAL spin lock callback function */
    pcb->moal_spin_lock(pmadapter->pmoal_handle, pmadapter->pmlan_cmd_lock);

    LEAVE();
    return;
}

/**
 *  @brief This function releases a lock on command queue.
 *
 *  @param pmadapter    A pointer to mlan_adapter structure
 *
 *  @return             N/A
 */
t_void
wlan_release_cmd_lock(IN mlan_adapter * pmadapter)
{
    mlan_callbacks *pcb = (mlan_callbacks *) & pmadapter->callbacks;

    ENTER();

    /* Call MOAL spin unlock callback function */
    pcb->moal_spin_unlock(pmadapter->pmoal_handle, pmadapter->pmlan_cmd_lock);

    LEAVE();
    return;
}
#endif /* CONFIG_MLAN_WMSDK */

/**
 *  @brief This function prepare the command before sending to firmware.
 *
 *  @param pmpriv       A pointer to mlan_private structure
 *  @param cmd_no       Command number
 *  @param cmd_action   Command action: GET or SET
 *  @param cmd_oid      Cmd oid: treated as sub command
 *  @param pioctl_buf   A pointer to MLAN IOCTL Request buffer
 *  @param pdata_buf    A pointer to information buffer
 *
 *  @return             MLAN_STATUS_SUCCESS or MLAN_STATUS_FAILURE
 */
mlan_status
wlan_prepare_cmd(IN mlan_private * pmpriv,
                 IN t_u16 cmd_no,
                 IN t_u16 cmd_action,
                 IN t_u32 cmd_oid,
                 IN t_void * pioctl_buf, IN t_void * pdata_buf)
{
#ifndef CONFIG_MLAN_WMSDK
    mlan_status ret = MLAN_STATUS_SUCCESS;
    mlan_adapter *pmadapter = pmpriv->adapter;
    cmd_ctrl_node *pcmd_node = MNULL;
    HostCmd_DS_COMMAND *cmd_ptr = MNULL;
    pmlan_ioctl_req pioctl_req = (mlan_ioctl_req *) pioctl_buf;

    ENTER();

    /* Sanity test */
    if (!pmadapter || pmadapter->surprise_removed) {
        PRINTM(MERROR, "PREP_CMD: Card is Removed\n");
        if (pioctl_req)
            pioctl_req->status_code = MLAN_ERROR_FW_NOT_READY;
        ret = MLAN_STATUS_FAILURE;
        goto done;
    }

    if (pmadapter->hw_status == WlanHardwareStatusReset) {
        if ((cmd_no != HostCmd_CMD_FUNC_INIT)
            ) {
            PRINTM(MERROR, "PREP_CMD: FW is in reset state\n");
            if (pioctl_req)
                pioctl_req->status_code = MLAN_ERROR_FW_NOT_READY;
            ret = MLAN_STATUS_FAILURE;
            goto done;
        }
    }

    /* Get a new command node */
    pcmd_node = wlan_get_cmd_node(pmadapter);

    if (pcmd_node == MNULL) {
        PRINTM(MERROR, "PREP_CMD: No free cmd node\n");
        if (pioctl_req)
            pioctl_req->status_code = MLAN_ERROR_NO_MEM;
        ret = MLAN_STATUS_FAILURE;
        goto done;
    }

    /* Initialize the command node */
    wlan_init_cmd_node(pmpriv, pcmd_node, cmd_oid, pioctl_buf, pdata_buf);

    if (pcmd_node->cmdbuf == MNULL) {
        PRINTM(MERROR, "PREP_CMD: No free cmd buf\n");
        if (pioctl_req)
            pioctl_req->status_code = MLAN_ERROR_NO_MEM;
        ret = MLAN_STATUS_FAILURE;
        goto done;
    }

    cmd_ptr =
        (HostCmd_DS_COMMAND *) (pcmd_node->cmdbuf->pbuf +
                                pcmd_node->cmdbuf->data_offset);
    cmd_ptr->command = cmd_no;
    cmd_ptr->result = 0;

    /* Prepare command */
    if (cmd_no)
        ret =
            pmpriv->ops.prepare_cmd(pmpriv, cmd_no, cmd_action, cmd_oid,
                                    pioctl_buf, pdata_buf, cmd_ptr);
    else {
        ret = wlan_cmd_host_cmd(pmpriv, cmd_ptr, pdata_buf);
        pcmd_node->cmd_flag |= CMD_F_HOSTCMD;
    }

    /* Return error, since the command preparation failed */
    if (ret != MLAN_STATUS_SUCCESS) {
        PRINTM(MERROR, "PREP_CMD: Command 0x%x preparation failed\n", cmd_no);
        pcmd_node->pioctl_buf = MNULL;
        wlan_insert_cmd_to_free_q(pmadapter, pcmd_node);
        if (pioctl_req)
            pioctl_req->status_code = MLAN_ERROR_CMD_DNLD_FAIL;
        ret = MLAN_STATUS_FAILURE;
        goto done;
    }

    /* Send command */
#ifdef STA_SUPPORT
    if (cmd_no == HostCmd_CMD_802_11_SCAN
#ifdef EXT_SCAN_SUPPORT
        || cmd_no == HostCmd_CMD_802_11_SCAN_EXT
#endif
       )
        wlan_queue_scan_cmd(pmpriv, pcmd_node);
    else {
#endif
        if ((cmd_no == HostCmd_CMD_802_11_HS_CFG_ENH) &&
            (cmd_action == HostCmd_ACT_GEN_SET) &&
            (pmadapter->hs_cfg.conditions == HOST_SLEEP_CFG_CANCEL))
            wlan_insert_cmd_to_pending_q(pmadapter, pcmd_node, MFALSE);
        else
            wlan_insert_cmd_to_pending_q(pmadapter, pcmd_node, MTRUE);
#ifdef STA_SUPPORT
    }
#endif
  done:
    LEAVE();
#endif /* CONFIG_MLAN_WMSDK */
    /* Note: We send only one command at a time and do not need the linked
       list based implementation used here. So we will call our own
       implementation here.
    */

    return wifi_prepare_and_send_cmd(pmpriv, cmd_no,
				     cmd_action, cmd_oid,
				     pioctl_buf, pdata_buf,
				     pmpriv->bss_type, NULL);

}

#ifndef CONFIG_MLAN_WMSDK
/**
 *  @brief This function inserts command node to cmd_free_q
 *              after cleaning it.
 *
 *  @param pmadapter    A pointer to mlan_adapter structure
 *  @param pcmd_node    A pointer to cmd_ctrl_node structure
 *
 *  @return             N/A
 */
t_void
wlan_insert_cmd_to_free_q(IN mlan_adapter * pmadapter,
                          IN cmd_ctrl_node * pcmd_node)
{
    mlan_callbacks *pcb = (mlan_callbacks *) & pmadapter->callbacks;
    mlan_ioctl_req *pioctl_req = MNULL;
    ENTER();

    if (pcmd_node == MNULL)
        goto done;
    if (pcmd_node->pioctl_buf) {
        pioctl_req = (mlan_ioctl_req *) pcmd_node->pioctl_buf;
        if (pioctl_req->status_code != MLAN_ERROR_NO_ERROR)
            pcb->moal_ioctl_complete(pmadapter->pmoal_handle,
                                     pioctl_req, MLAN_STATUS_FAILURE);
        else
            pcb->moal_ioctl_complete(pmadapter->pmoal_handle,
                                     pioctl_req, MLAN_STATUS_SUCCESS);
    }
    /* Clean the node */
    wlan_clean_cmd_node(pmadapter, pcmd_node);

    /* Insert node into cmd_free_q */
    util_enqueue_list_tail(pmadapter->pmoal_handle, &pmadapter->cmd_free_q,
                           (pmlan_linked_list) pcmd_node,
                           pmadapter->callbacks.moal_spin_lock,
                           pmadapter->callbacks.moal_spin_unlock);
  done:
    LEAVE();
}

/**
 *  @brief This function queues the command to cmd list.
 *
 *  @param pmadapter    A pointer to mlan_adapter structure
 *  @param pcmd_node    A pointer to cmd_ctrl_node structure
 *  @param add_tail      Specify if the cmd needs to be queued in the header or tail
 *
 *  @return             N/A
 */
t_void
wlan_insert_cmd_to_pending_q(IN mlan_adapter * pmadapter,
                             IN cmd_ctrl_node * pcmd_node, IN t_u32 add_tail)
{
    HostCmd_DS_COMMAND *pcmd = MNULL;
    t_u16 command;

    ENTER();

    if (pcmd_node == MNULL) {
        PRINTM(MERROR, "QUEUE_CMD: pcmd_node is MNULL\n");
        goto done;
    }

    pcmd =
        (HostCmd_DS_COMMAND *) (pcmd_node->cmdbuf->pbuf +
                                pcmd_node->cmdbuf->data_offset);
    if (pcmd == MNULL) {
        PRINTM(MERROR, "QUEUE_CMD: pcmd is MNULL\n");
        goto done;
    }

    command = wlan_le16_to_cpu(pcmd->command);

    /* Exit_PS command needs to be queued in the header always. */
    if (command == HostCmd_CMD_802_11_PS_MODE_ENH) {
        HostCmd_DS_802_11_PS_MODE_ENH *pm = &pcmd->params.psmode_enh;
        if (wlan_le16_to_cpu(pm->action) == DIS_AUTO_PS) {
            if (pmadapter->ps_state != PS_STATE_AWAKE)
                add_tail = MFALSE;
        }
    }

    if (add_tail) {
        util_enqueue_list_tail(pmadapter->pmoal_handle,
                               &pmadapter->cmd_pending_q,
                               (pmlan_linked_list) pcmd_node,
                               pmadapter->callbacks.moal_spin_lock,
                               pmadapter->callbacks.moal_spin_unlock);
    } else {
        util_enqueue_list_head(pmadapter->pmoal_handle,
                               &pmadapter->cmd_pending_q,
                               (pmlan_linked_list) pcmd_node,
                               pmadapter->callbacks.moal_spin_lock,
                               pmadapter->callbacks.moal_spin_unlock);
    }

    PRINTM_NETINTF(MCMND, pcmd_node->priv);
    PRINTM(MCMND, "QUEUE_CMD: cmd=0x%x is queued\n", command);

  done:
    LEAVE();
    return;
}

/**
 *  @brief This function executes next command in command
 *      pending queue. It will put firmware back to PS mode
 *      if applicable.
 *
 *  @param pmadapter     A pointer to mlan_adapter structure
 *
 *  @return             MLAN_STATUS_SUCCESS or MLAN_STATUS_FAILURE
 */
mlan_status
wlan_exec_next_cmd(mlan_adapter * pmadapter)
{
    mlan_private *priv = MNULL;
    cmd_ctrl_node *pcmd_node = MNULL;
    mlan_status ret = MLAN_STATUS_SUCCESS;
    HostCmd_DS_COMMAND *pcmd;

    ENTER();

    /* Sanity test */
    if (pmadapter == MNULL) {
        PRINTM(MERROR, "EXEC_NEXT_CMD: pmadapter is MNULL\n");
        ret = MLAN_STATUS_FAILURE;
        goto done;
    }
    /* Check if already in processing */
    if (pmadapter->curr_cmd) {
        PRINTM(MERROR, "EXEC_NEXT_CMD: there is command in processing!\n");
        ret = MLAN_STATUS_FAILURE;
        goto done;
    }

    wlan_request_cmd_lock(pmadapter);
    /* Check if any command is pending */
    pcmd_node =
        (cmd_ctrl_node *) util_peek_list(pmadapter->pmoal_handle,
                                         &pmadapter->cmd_pending_q,
                                         pmadapter->callbacks.moal_spin_lock,
                                         pmadapter->callbacks.moal_spin_unlock);

    if (pcmd_node) {
        pcmd =
            (HostCmd_DS_COMMAND *) (pcmd_node->cmdbuf->pbuf +
                                    pcmd_node->cmdbuf->data_offset);
        priv = pcmd_node->priv;

        if (pmadapter->ps_state != PS_STATE_AWAKE) {
            PRINTM(MERROR,
                   "Cannot send command in sleep state, this should not happen\n");
            wlan_release_cmd_lock(pmadapter);
            goto done;
        }

        util_unlink_list(pmadapter->pmoal_handle,
                         &pmadapter->cmd_pending_q,
                         (pmlan_linked_list) pcmd_node,
                         pmadapter->callbacks.moal_spin_lock,
                         pmadapter->callbacks.moal_spin_unlock);
        wlan_release_cmd_lock(pmadapter);
        ret = wlan_dnld_cmd_to_fw(priv, pcmd_node);
        priv = wlan_get_priv(pmadapter, MLAN_BSS_ROLE_ANY);
        /* Any command sent to the firmware when host is in sleep mode, should
           de-configure host sleep */
        /* We should skip the host sleep configuration command itself though */
        if (priv &&
            (pcmd->command !=
             wlan_cpu_to_le16(HostCmd_CMD_802_11_HS_CFG_ENH))) {
            if (pmadapter->hs_activated == MTRUE) {
                pmadapter->is_hs_configured = MFALSE;
                wlan_host_sleep_activated_event(priv, MFALSE);
            }
        }
        goto done;
    } else {
        wlan_release_cmd_lock(pmadapter);
    }
    ret = MLAN_STATUS_SUCCESS;
  done:
    LEAVE();
    return ret;
}

/**
 *  @brief This function handles the command response
 *
 *  @param pmadapter    A pointer to mlan_adapter structure
 *
 *  @return             MLAN_STATUS_SUCCESS or MLAN_STATUS_FAILURE
 */
mlan_status
wlan_process_cmdresp(mlan_adapter * pmadapter)
{
    HostCmd_DS_COMMAND *resp = MNULL;
    mlan_private *pmpriv = wlan_get_priv(pmadapter, MLAN_BSS_ROLE_ANY);
    mlan_private *pmpriv_next = MNULL;
    mlan_status ret = MLAN_STATUS_SUCCESS;
    t_u16 orig_cmdresp_no;
    t_u16 cmdresp_no;
    t_u16 cmdresp_result;
    mlan_ioctl_req *pioctl_buf = MNULL;
    mlan_callbacks *pcb = (mlan_callbacks *) & pmadapter->callbacks;
#ifdef DEBUG_LEVEL1
    t_u32 sec, usec;
#endif
    t_u32 i;

    ENTER();

    /* Now we got response from FW, cancel the command timer */
    if (pmadapter->cmd_timer_is_set) {
        /* Cancel command timeout timer */
        pcb->moal_stop_timer(pmadapter->pmoal_handle,
                             pmadapter->pmlan_cmd_timer);
        /* Cancel command timeout timer */
        pmadapter->cmd_timer_is_set = MFALSE;
    }

    if (pmadapter->curr_cmd)
        if (pmadapter->curr_cmd->pioctl_buf != MNULL) {
            pioctl_buf = (mlan_ioctl_req *) pmadapter->curr_cmd->pioctl_buf;
        }

    if (!pmadapter->curr_cmd || !pmadapter->curr_cmd->respbuf) {
        resp = (HostCmd_DS_COMMAND *) pmadapter->upld_buf;
        resp->command = wlan_le16_to_cpu(resp->command);
        PRINTM(MERROR, "CMD_RESP: No curr_cmd, 0x%x\n", resp->command);
        if (pioctl_buf)
            pioctl_buf->status_code = MLAN_ERROR_CMD_RESP_FAIL;
        ret = MLAN_STATUS_FAILURE;
        goto done;
    }

    pmadapter->num_cmd_timeout = 0;

    DBG_HEXDUMP(MCMD_D, "CMD_RESP",
                pmadapter->curr_cmd->respbuf->pbuf +
                pmadapter->curr_cmd->respbuf->data_offset,
                pmadapter->curr_cmd->respbuf->data_len);

    resp =
        (HostCmd_DS_COMMAND *) (pmadapter->curr_cmd->respbuf->pbuf +
                                pmadapter->curr_cmd->respbuf->data_offset);
    wlan_request_cmd_lock(pmadapter);
    if (pmadapter->curr_cmd->cmd_flag & CMD_F_CANCELED) {
        cmd_ctrl_node *free_cmd = pmadapter->curr_cmd;
        pmadapter->curr_cmd = MNULL;
        wlan_release_cmd_lock(pmadapter);
        PRINTM(MERROR, "CMD_RESP: 0x%x been canceled!\n",
               wlan_le16_to_cpu(resp->command));
        wlan_insert_cmd_to_free_q(pmadapter, free_cmd);
        if (pioctl_buf)
            pioctl_buf->status_code = MLAN_ERROR_CMD_CANCEL;
        ret = MLAN_STATUS_FAILURE;
        goto done;
    } else {
        wlan_release_cmd_lock(pmadapter);
    }
    if (pmadapter->curr_cmd->cmd_flag & CMD_F_HOSTCMD) {
        /* Copy original response back to response buffer */
        wlan_ret_host_cmd(pmpriv, resp, pioctl_buf);
    }
    orig_cmdresp_no = wlan_le16_to_cpu(resp->command);
    resp->size = wlan_le16_to_cpu(resp->size);
    resp->seq_num = wlan_le16_to_cpu(resp->seq_num);
    resp->result = wlan_le16_to_cpu(resp->result);

    /* Get BSS number and corresponding priv */
    pmpriv =
        wlan_get_priv_by_id(pmadapter, HostCmd_GET_BSS_NO(resp->seq_num),
                            HostCmd_GET_BSS_TYPE(resp->seq_num));
    if (!pmpriv)
        pmpriv = wlan_get_priv(pmadapter, MLAN_BSS_ROLE_ANY);
    /* Clear RET_BIT from HostCmd */
    resp->command = (orig_cmdresp_no & HostCmd_CMD_ID_MASK);
    cmdresp_no = resp->command;

    cmdresp_result = resp->result;

    /* Save the last command response to debug log */
    pmadapter->dbg.last_cmd_resp_index =
        (pmadapter->dbg.last_cmd_resp_index + 1) % DBG_CMD_NUM;
    pmadapter->dbg.last_cmd_resp_id[pmadapter->dbg.last_cmd_resp_index] =
        orig_cmdresp_no;

    PRINTM_GET_SYS_TIME(MCMND, &sec, &usec);
    PRINTM_NETINTF(MCMND, pmadapter->curr_cmd->priv);
    PRINTM(MCMND, "CMD_RESP (%lu.%06lu): 0x%x, result %d, len %d, seqno 0x%x\n",
           sec, usec, orig_cmdresp_no, cmdresp_result, resp->size,
           resp->seq_num);

    if (!(orig_cmdresp_no & HostCmd_RET_BIT)) {
        PRINTM(MERROR, "CMD_RESP: Invalid response to command!\n");
        if (pioctl_buf) {
            pioctl_buf->status_code = MLAN_ERROR_FW_CMDRESP;
        }
        wlan_insert_cmd_to_free_q(pmadapter, pmadapter->curr_cmd);
        wlan_request_cmd_lock(pmadapter);
        pmadapter->curr_cmd = MNULL;
        wlan_release_cmd_lock(pmadapter);
        ret = MLAN_STATUS_FAILURE;
        goto done;
    }

    if (pmadapter->curr_cmd->cmd_flag & CMD_F_HOSTCMD) {
        pmadapter->curr_cmd->cmd_flag &= ~CMD_F_HOSTCMD;
        if ((cmdresp_result == HostCmd_RESULT_OK)
            && (cmdresp_no == HostCmd_CMD_802_11_HS_CFG_ENH))
            ret = wlan_ret_802_11_hs_cfg(pmpriv, resp, pioctl_buf);
    } else {
        /* handle response */
        ret = pmpriv->ops.process_cmdresp(pmpriv, cmdresp_no, resp, pioctl_buf);
    }

    /* Check init command response */
    if (pmadapter->hw_status == WlanHardwareStatusInitializing) {
        if (ret == MLAN_STATUS_FAILURE) {
#if defined(STA_SUPPORT)
            if (pmadapter->pwarm_reset_ioctl_req) {
                /* warm reset failure */
                pmadapter->pwarm_reset_ioctl_req->status_code =
                    MLAN_ERROR_CMD_RESP_FAIL;
                pcb->moal_ioctl_complete(pmadapter->pmoal_handle,
                                         pmadapter->pwarm_reset_ioctl_req,
                                         MLAN_STATUS_FAILURE);
                pmadapter->pwarm_reset_ioctl_req = MNULL;
                goto done;
            }
#endif
            PRINTM(MERROR, "cmd 0x%02x failed during initialization\n",
                   cmdresp_no);
            wlan_init_fw_complete(pmadapter);
            goto done;
        }
    }

    wlan_request_cmd_lock(pmadapter);
    if (pmadapter->curr_cmd) {
        cmd_ctrl_node *free_cmd = pmadapter->curr_cmd;
        pioctl_buf = (mlan_ioctl_req *) pmadapter->curr_cmd->pioctl_buf;
        pmadapter->curr_cmd = MNULL;
        wlan_release_cmd_lock(pmadapter);

        if (pioctl_buf && (ret == MLAN_STATUS_SUCCESS))
            pioctl_buf->status_code = MLAN_ERROR_NO_ERROR;
        else if (pioctl_buf && (ret == MLAN_STATUS_FAILURE))
            pioctl_buf->status_code = MLAN_ERROR_CMD_RESP_FAIL;

        /* Clean up and put current command back to cmd_free_q */
        wlan_insert_cmd_to_free_q(pmadapter, free_cmd);
    } else {
        wlan_release_cmd_lock(pmadapter);
    }

    if ((pmadapter->hw_status == WlanHardwareStatusInitializing) &&
        (pmadapter->last_init_cmd == cmdresp_no)) {
        i = pmpriv->bss_index + 1;
        while (!(pmpriv_next = pmadapter->priv[i]) && i < pmadapter->priv_num)
            i++;
        if (!pmpriv_next || i >= pmadapter->priv_num) {
#if defined(STA_SUPPORT)
            if (pmadapter->pwarm_reset_ioctl_req) {
                /* warm reset complete */
                pmadapter->hw_status = WlanHardwareStatusReady;
                pcb->moal_ioctl_complete(pmadapter->pmoal_handle,
                                         pmadapter->pwarm_reset_ioctl_req,
                                         MLAN_STATUS_SUCCESS);
                pmadapter->pwarm_reset_ioctl_req = MNULL;
                goto done;
            }
#endif
            pmadapter->hw_status = WlanHardwareStatusInitdone;
        } else {
            /* Issue init commands for the next interface */
            ret = pmpriv_next->ops.init_cmd(pmpriv_next, MFALSE);
        }
    }

  done:
    LEAVE();
    return ret;
}

/**
 *  @brief This function handles the timeout of command sending.
 *  It will re-send the same command again.
 *
 *  @param function_context   A pointer to function_context
 *  @return 	   N/A
 */
t_void
wlan_cmd_timeout_func(t_void * function_context)
{
    mlan_adapter *pmadapter = (mlan_adapter *) function_context;
    cmd_ctrl_node *pcmd_node = MNULL;
    mlan_ioctl_req *pioctl_buf = MNULL;
#ifdef DEBUG_LEVEL1
    t_u32 sec, usec;
#endif
    t_u8 i;
    mlan_private *pmpriv = MNULL;

    ENTER();

    pmadapter->cmd_timer_is_set = MFALSE;
    pmadapter->num_cmd_timeout++;
    pmadapter->dbg.num_cmd_timeout++;
    if (!pmadapter->curr_cmd) {
        PRINTM(MWARN, "CurCmd Empty\n");
        goto exit;
    }
    pcmd_node = pmadapter->curr_cmd;
    if (pcmd_node->pioctl_buf != MNULL) {
        pioctl_buf = (mlan_ioctl_req *) pcmd_node->pioctl_buf;
        pioctl_buf->status_code = MLAN_ERROR_CMD_TIMEOUT;
    }

    if (pcmd_node) {
        pmadapter->dbg.timeout_cmd_id =
            pmadapter->dbg.last_cmd_id[pmadapter->dbg.last_cmd_index];
        pmadapter->dbg.timeout_cmd_act =
            pmadapter->dbg.last_cmd_act[pmadapter->dbg.last_cmd_index];
        PRINTM_GET_SYS_TIME(MERROR, &sec, &usec);
        PRINTM(MERROR, "Timeout cmd id (%lu.%06lu) = 0x%x, act = 0x%x \n", sec,
               usec, pmadapter->dbg.timeout_cmd_id,
               pmadapter->dbg.timeout_cmd_act);
        if (pcmd_node->cmdbuf) {
            t_u8 *pcmd_buf;
            pcmd_buf =
                pcmd_node->cmdbuf->pbuf + pcmd_node->cmdbuf->data_offset +
                INTF_HEADER_LEN;
            for (i = 0; i < 16; i++) {
                PRINTM(MERROR, "%02x ", *pcmd_buf++);
            }
            PRINTM(MERROR, "\n");
        }

        pmpriv = pcmd_node->priv;
        if (pmpriv) {
            PRINTM(MERROR, "BSS type = %d BSS role= %d\n", pmpriv->bss_type,
                   pmpriv->bss_role);
        }

        PRINTM(MERROR, "num_cmd_timeout = %d\n",
               pmadapter->dbg.num_cmd_timeout);
        PRINTM(MERROR, "last_cmd_index = %d\n", pmadapter->dbg.last_cmd_index);
        PRINTM(MERROR, "last_cmd_id = ");
        for (i = 0; i < DBG_CMD_NUM; i++) {
            PRINTM(MERROR, "0x%x ", pmadapter->dbg.last_cmd_id[i]);
        }
        PRINTM(MERROR, "\n");
        PRINTM(MERROR, "last_cmd_act = ");
        for (i = 0; i < DBG_CMD_NUM; i++) {
            PRINTM(MERROR, "0x%x ", pmadapter->dbg.last_cmd_act[i]);
        }
        PRINTM(MERROR, "\n");
        PRINTM(MERROR, "last_cmd_resp_index = %d\n",
               pmadapter->dbg.last_cmd_resp_index);
        PRINTM(MERROR, "last_cmd_resp_id = ");
        for (i = 0; i < DBG_CMD_NUM; i++) {
            PRINTM(MERROR, "0x%x ", pmadapter->dbg.last_cmd_resp_id[i]);
        }
        PRINTM(MERROR, "\n");
        PRINTM(MERROR, "last_event_index = %d\n",
               pmadapter->dbg.last_event_index);
        PRINTM(MERROR, "last_event = ");
        for (i = 0; i < DBG_CMD_NUM; i++) {
            PRINTM(MERROR, "0x%x ", pmadapter->dbg.last_event[i]);
        }
        PRINTM(MERROR, "\n");

        PRINTM(MERROR, "num_data_h2c_failure = %d\n",
               pmadapter->dbg.num_tx_host_to_card_failure);
        PRINTM(MERROR, "num_cmd_h2c_failure = %d\n",
               pmadapter->dbg.num_cmd_host_to_card_failure);
        PRINTM(MERROR, "num_data_c2h_failure = %d\n",
               pmadapter->dbg.num_rx_card_to_host_failure);
        PRINTM(MERROR, "num_cmdevt_c2h_failure = %d\n",
               pmadapter->dbg.num_cmdevt_card_to_host_failure);
        PRINTM(MERROR, "num_int_read_failure = %d\n",
               pmadapter->dbg.num_int_read_failure);
        PRINTM(MERROR, "last_int_status = %d\n",
               pmadapter->dbg.last_int_status);

        PRINTM(MERROR, "num_event_deauth = %d\n",
               pmadapter->dbg.num_event_deauth);
        PRINTM(MERROR, "num_event_disassoc = %d\n",
               pmadapter->dbg.num_event_disassoc);
        PRINTM(MERROR, "num_event_link_lost = %d\n",
               pmadapter->dbg.num_event_link_lost);
        PRINTM(MERROR, "num_cmd_deauth = %d\n", pmadapter->dbg.num_cmd_deauth);
        PRINTM(MERROR, "num_cmd_assoc_success = %d\n",
               pmadapter->dbg.num_cmd_assoc_success);
        PRINTM(MERROR, "num_cmd_assoc_failure = %d\n",
               pmadapter->dbg.num_cmd_assoc_failure);
        PRINTM(MERROR, "cmd_resp_received=%d\n", pmadapter->cmd_resp_received);
        PRINTM(MERROR, "event_received=%d\n", pmadapter->event_received);

        PRINTM(MERROR, "max_tx_buf_size=%d\n", pmadapter->max_tx_buf_size);
        PRINTM(MERROR, "tx_buf_size=%d\n", pmadapter->tx_buf_size);
        PRINTM(MERROR, "curr_tx_buf_size=%d\n", pmadapter->curr_tx_buf_size);

        PRINTM(MERROR, "data_sent=%d cmd_sent=%d\n", pmadapter->data_sent,
               pmadapter->cmd_sent);

        PRINTM(MERROR, "ps_mode=%d ps_state=%d\n", pmadapter->ps_mode,
               pmadapter->ps_state);
        PRINTM(MERROR, "wakeup_dev_req=%d wakeup_tries=%d\n",
               pmadapter->pm_wakeup_card_req, pmadapter->pm_wakeup_fw_try);
        PRINTM(MERROR, "hs_configured=%d hs_activated=%d\n",
               pmadapter->is_hs_configured, pmadapter->hs_activated);
        PRINTM(MERROR, "pps_uapsd_mode=%d sleep_pd=%d\n",
               pmadapter->pps_uapsd_mode, pmadapter->sleep_period.period);
        PRINTM(MERROR, "tx_lock_flag = %d\n", pmadapter->tx_lock_flag);
        PRINTM(MERROR, "scan_processing = %d\n", pmadapter->scan_processing);

        PRINTM(MERROR, "mp_rd_bitmap=0x%x curr_rd_port=0x%x\n",
               pmadapter->mp_rd_bitmap, pmadapter->curr_rd_port);
        PRINTM(MERROR, "mp_wr_bitmap=0x%x curr_wr_port=0x%x\n",
               pmadapter->mp_wr_bitmap, pmadapter->curr_wr_port);
    }
    if (pmadapter->hw_status == WlanHardwareStatusInitializing)
        wlan_init_fw_complete(pmadapter);
    else
        /* Signal MOAL to perform extra handling for debugging */
    if (pmpriv) {
        wlan_recv_event(pmpriv, MLAN_EVENT_ID_DRV_DBG_DUMP, MNULL);
    } else {
        wlan_recv_event(wlan_get_priv(pmadapter, MLAN_BSS_ROLE_ANY),
                        MLAN_EVENT_ID_DRV_DBG_DUMP, MNULL);
    }

  exit:
    LEAVE();
    return;
}

#ifdef STA_SUPPORT
/**
 *  @brief Internal function used to flush the scan pending queue
 *
 *  @param pmadapter    A pointer to mlan_adapter structure
 *
 *  @return             N/A
 */
t_void
wlan_flush_scan_queue(IN pmlan_adapter pmadapter)
{
    mlan_callbacks *pcb = (pmlan_callbacks) & pmadapter->callbacks;
    cmd_ctrl_node *pcmd_node = MNULL;

    ENTER();

    while ((pcmd_node =
            (cmd_ctrl_node *) util_peek_list(pmadapter->pmoal_handle,
                                             &pmadapter->scan_pending_q,
                                             pcb->moal_spin_lock,
                                             pcb->moal_spin_unlock))) {
        util_unlink_list(pmadapter->pmoal_handle, &pmadapter->scan_pending_q,
                         (pmlan_linked_list) pcmd_node, pcb->moal_spin_lock,
                         pcb->moal_spin_unlock);
        pcmd_node->pioctl_buf = MNULL;
        wlan_insert_cmd_to_free_q(pmadapter, pcmd_node);
    }
    wlan_request_cmd_lock(pmadapter);
    pmadapter->scan_processing = MFALSE;
    wlan_release_cmd_lock(pmadapter);

    LEAVE();
}
#endif

/**
 *  @brief Cancel all pending cmd.
 *
 *  @param pmadapter	A pointer to mlan_adapter
 *
 *  @return		N/A
 */
t_void
wlan_cancel_all_pending_cmd(pmlan_adapter pmadapter)
{
    cmd_ctrl_node *pcmd_node = MNULL;
    pmlan_callbacks pcb = &pmadapter->callbacks;
    mlan_ioctl_req *pioctl_buf = MNULL;

    ENTER();
    /* Cancel current cmd */
    wlan_request_cmd_lock(pmadapter);
    if ((pmadapter->curr_cmd) && (pmadapter->curr_cmd->pioctl_buf)) {
        pioctl_buf = (mlan_ioctl_req *) pmadapter->curr_cmd->pioctl_buf;
        pmadapter->curr_cmd->pioctl_buf = MNULL;
        wlan_release_cmd_lock(pmadapter);
        pioctl_buf->status_code = MLAN_ERROR_CMD_CANCEL;
        pcb->moal_ioctl_complete(pmadapter->pmoal_handle, pioctl_buf,
                                 MLAN_STATUS_FAILURE);
    } else {
        wlan_release_cmd_lock(pmadapter);
    }
    /* Cancel all pending command */
    while ((pcmd_node =
            (cmd_ctrl_node *) util_peek_list(pmadapter->pmoal_handle,
                                             &pmadapter->cmd_pending_q,
                                             pcb->moal_spin_lock,
                                             pcb->moal_spin_unlock))) {
        util_unlink_list(pmadapter->pmoal_handle, &pmadapter->cmd_pending_q,
                         (pmlan_linked_list) pcmd_node, pcb->moal_spin_lock,
                         pcb->moal_spin_unlock);
        if (pcmd_node->pioctl_buf) {
            pioctl_buf = (mlan_ioctl_req *) pcmd_node->pioctl_buf;
            pioctl_buf->status_code = MLAN_ERROR_CMD_CANCEL;
            pcb->moal_ioctl_complete(pmadapter->pmoal_handle, pioctl_buf,
                                     MLAN_STATUS_FAILURE);
            pcmd_node->pioctl_buf = MNULL;
        }
        wlan_insert_cmd_to_free_q(pmadapter, pcmd_node);
    }
#ifdef STA_SUPPORT
    /* Cancel all pending scan command */
    wlan_flush_scan_queue(pmadapter);
#endif
    LEAVE();
}

/**
 *  @brief Cancel pending ioctl cmd.
 *
 *  @param pmadapter	A pointer to mlan_adapter
 *  @param pioctl_req	A pointer to mlan_ioctl_req buf
 *
 *  @return		N/A
 */
t_void
wlan_cancel_pending_ioctl(pmlan_adapter pmadapter, pmlan_ioctl_req pioctl_req)
{
    pmlan_callbacks pcb = &pmadapter->callbacks;
    cmd_ctrl_node *pcmd_node = MNULL;

    ENTER();

    PRINTM(MIOCTL, "MOAL Cancel IOCTL: 0x%x sub_id=0x%x action=%d\n",
           pioctl_req->req_id, *((t_u32 *) pioctl_req->pbuf),
           (int) pioctl_req->action);

    wlan_request_cmd_lock(pmadapter);
    if ((pmadapter->curr_cmd) &&
        (pmadapter->curr_cmd->pioctl_buf == pioctl_req)) {
        pcmd_node = pmadapter->curr_cmd;
        pcmd_node->pioctl_buf = MNULL;
        pcmd_node->cmd_flag |= CMD_F_CANCELED;
    }

    while ((pcmd_node =
            wlan_get_pending_ioctl_cmd(pmadapter, pioctl_req)) != MNULL) {
        util_unlink_list(pmadapter->pmoal_handle, &pmadapter->cmd_pending_q,
                         (pmlan_linked_list) pcmd_node,
                         pmadapter->callbacks.moal_spin_lock,
                         pmadapter->callbacks.moal_spin_unlock);
        pcmd_node->pioctl_buf = MNULL;
        wlan_release_cmd_lock(pmadapter);
        wlan_insert_cmd_to_free_q(pmadapter, pcmd_node);
        wlan_request_cmd_lock(pmadapter);
    }
    wlan_release_cmd_lock(pmadapter);
#ifdef STA_SUPPORT
    if (pioctl_req->req_id == MLAN_IOCTL_SCAN) {
        /* IOCTL will be completed, avoid calling IOCTL complete again from
           EVENT */
        if (pmadapter->pext_scan_ioctl_req == pioctl_req)
            pmadapter->pext_scan_ioctl_req = MNULL;
        /* Cancel all pending scan command */
        wlan_flush_scan_queue(pmadapter);
    }
#endif
    pioctl_req->status_code = MLAN_ERROR_CMD_CANCEL;
    pcb->moal_ioctl_complete(pmadapter->pmoal_handle, pioctl_req,
                             MLAN_STATUS_FAILURE);

    LEAVE();
    return;
}

/**
 *  @brief Handle the version_ext resp
 *
 *  @param pmpriv       A pointer to mlan_private structure
 *  @param resp         A pointer to HostCmd_DS_COMMAND
 *  @param pioctl_buf   A pointer to mlan_ioctl_req structure
 *
 *  @return             MLAN_STATUS_SUCCESS
 */
mlan_status
wlan_ret_ver_ext(IN pmlan_private pmpriv,
                 IN HostCmd_DS_COMMAND * resp, IN mlan_ioctl_req * pioctl_buf)
{
    HostCmd_DS_VERSION_EXT *ver_ext = &resp->params.verext;
    mlan_ds_get_info *info;
    ENTER();
    if (pioctl_buf) {
        info = (mlan_ds_get_info *) pioctl_buf->pbuf;
        info->param.ver_ext.version_str_sel = ver_ext->version_str_sel;
        memcpy(pmpriv->adapter, info->param.ver_ext.version_str,
               ver_ext->version_str, sizeof(char) * 128);
    }
    LEAVE();
    return MLAN_STATUS_SUCCESS;
}

/**
 *  @brief Handle the rx mgmt forward registration resp
 *
 *  @param pmpriv       A pointer to mlan_private structure
 *  @param resp         A pointer to HostCmd_DS_COMMAND
 *  @param pioctl_buf   A pointer to mlan_ioctl_req structure
 *
 *  @return             MLAN_STATUS_SUCCESS
 */
mlan_status
wlan_ret_rx_mgmt_ind(IN pmlan_private pmpriv,
                     IN HostCmd_DS_COMMAND * resp,
                     IN mlan_ioctl_req * pioctl_buf)
{
    mlan_ds_misc_cfg *misc = MNULL;
    ENTER();

    if (pioctl_buf) {
        misc = (mlan_ds_misc_cfg *) pioctl_buf->pbuf;
        misc->param.mgmt_subtype_mask =
            wlan_le32_to_cpu(resp->params.rx_mgmt_ind.mgmt_subtype_mask);
    }

    LEAVE();
    return MLAN_STATUS_SUCCESS;
}

/**
 *  @brief This function checks conditions and prepares to
 *              send sleep confirm command to firmware if OK.
 *
 *  @param pmadapter    A pointer to mlan_adapter structure
 *
 *  @return             N/A
 */
t_void
wlan_check_ps_cond(mlan_adapter * pmadapter)
{
    ENTER();

    if (!pmadapter->cmd_sent &&
        !pmadapter->curr_cmd && !IS_CARD_RX_RCVD(pmadapter)) {
        wlan_dnld_sleep_confirm_cmd(pmadapter);
    } else {
        PRINTM(MCMND, "Delay Sleep Confirm (%s%s%s)\n",
               (pmadapter->cmd_sent) ? "D" : "",
               (pmadapter->curr_cmd) ? "C" : "",
               (IS_CARD_RX_RCVD(pmadapter)) ? "R" : "");
    }

    LEAVE();
}

/**
 *  @brief This function sends the HS_ACTIVATED event to the application
 *
 *  @param priv         A pointer to mlan_private structure
 *  @param activated    MTRUE if activated, MFALSE if de-activated
 *
 *  @return             N/A
 */
t_void
wlan_host_sleep_activated_event(pmlan_private priv, t_u8 activated)
{
    ENTER();

    if (activated) {
        if (priv->adapter->is_hs_configured) {
            priv->adapter->hs_activated = MTRUE;
            PRINTM(MEVENT, "hs_activated\n");
            wlan_recv_event(priv, MLAN_EVENT_ID_DRV_HS_ACTIVATED, MNULL);
        } else
            PRINTM(MWARN, "hs_activated: HS not configured !!!\n");
    } else {
        PRINTM(MEVENT, "hs_deactived\n");
        priv->adapter->hs_activated = MFALSE;
        wlan_recv_event(priv, MLAN_EVENT_ID_DRV_HS_DEACTIVATED, MNULL);
    }

    LEAVE();
}

/**
 *  @brief This function sends the HS_WAKEUP event to the application
 *
 *  @param priv         A pointer to mlan_private structure
 *
 *  @return             N/A
 */
t_void
wlan_host_sleep_wakeup_event(pmlan_private priv)
{
    ENTER();

    if (priv->adapter->is_hs_configured) {
        wlan_recv_event(priv, MLAN_EVENT_ID_FW_HS_WAKEUP, MNULL);
    } else {
        PRINTM(MWARN, "hs_wakeup: Host Sleep not configured !!!\n");
    }

    LEAVE();
}

/**
 *  @brief This function handles the command response of hs_cfg
 *
 *  @param pmpriv       A pointer to mlan_private structure
 *  @param resp         A pointer to HostCmd_DS_COMMAND
 *  @param pioctl_buf   A pointer to mlan_ioctl_req structure
 *
 *  @return             MLAN_STATUS_SUCCESS
 */
mlan_status
wlan_ret_802_11_hs_cfg(IN pmlan_private pmpriv,
                       IN HostCmd_DS_COMMAND * resp,
                       IN mlan_ioctl_req * pioctl_buf)
{
    pmlan_adapter pmadapter = pmpriv->adapter;
    HostCmd_DS_802_11_HS_CFG_ENH *phs_cfg = &resp->params.opt_hs_cfg;

    ENTER();

    if (wlan_le16_to_cpu(phs_cfg->action) == HS_ACTIVATE) {
        /* clean up curr_cmd to allow suspend */
        if (pioctl_buf)
            pioctl_buf->status_code = MLAN_ERROR_NO_ERROR;
        /* Clean up and put current command back to cmd_free_q */
        wlan_insert_cmd_to_free_q(pmadapter, pmadapter->curr_cmd);
        wlan_request_cmd_lock(pmadapter);
        pmadapter->curr_cmd = MNULL;
        wlan_release_cmd_lock(pmadapter);
        wlan_host_sleep_activated_event(pmpriv, MTRUE);
        goto done;
    } else {
        phs_cfg->params.hs_config.conditions =
            wlan_le32_to_cpu(phs_cfg->params.hs_config.conditions);
        PRINTM(MCMND,
               "CMD_RESP: HS_CFG cmd reply result=%#x,"
               " conditions=0x%x gpio=0x%x gap=0x%x\n", resp->result,
               phs_cfg->params.hs_config.conditions,
               phs_cfg->params.hs_config.gpio, phs_cfg->params.hs_config.gap);
    }
    if (phs_cfg->params.hs_config.conditions != HOST_SLEEP_CFG_CANCEL) {
        pmadapter->is_hs_configured = MTRUE;
    } else {
        pmadapter->is_hs_configured = MFALSE;
        if (pmadapter->hs_activated)
            wlan_host_sleep_activated_event(pmpriv, MFALSE);
    }

  done:
    LEAVE();
    return MLAN_STATUS_SUCCESS;
}

/**
 *  @brief Perform hs related activities on receiving the power up interrupt
 *
 *  @param pmadapter  A pointer to the adapter structure
 *  @return           N/A
 */
t_void
wlan_process_hs_config(pmlan_adapter pmadapter)
{
    ENTER();
    PRINTM(MINFO, "Recevie interrupt/data in HS mode\n");
    if (pmadapter->hs_cfg.gap == HOST_SLEEP_CFG_GAP_FF)
        wlan_pm_wakeup_card(pmadapter);
    LEAVE();
    return;
}

/**
 *  @brief Check sleep confirm command response and set the state to ASLEEP
 *
 *  @param pmadapter  A pointer to the adapter structure
 *  @param pbuf       A pointer to the command response buffer
 *  @param upld_len   Command response buffer length
 *  @return           N/A
 */
void
wlan_process_sleep_confirm_resp(pmlan_adapter pmadapter, t_u8 * pbuf,
                                t_u32 upld_len)
{
    HostCmd_DS_COMMAND *cmd;

    ENTER();

    if (!upld_len) {
        PRINTM(MERROR, "Command size is 0\n");
        LEAVE();
        return;
    }
    cmd = (HostCmd_DS_COMMAND *) pbuf;
    cmd->result = wlan_le16_to_cpu(cmd->result);
    cmd->command = wlan_le16_to_cpu(cmd->command);
    cmd->seq_num = wlan_le16_to_cpu(cmd->seq_num);

    /* Update sequence number */
    cmd->seq_num = HostCmd_GET_SEQ_NO(cmd->seq_num);
    /* Clear RET_BIT from HostCmd */
    cmd->command &= HostCmd_CMD_ID_MASK;

    if (cmd->command != HostCmd_CMD_802_11_PS_MODE_ENH) {
        PRINTM(MERROR,
               "Received unexpected response for command %x, result = %x\n",
               cmd->command, cmd->result);
        LEAVE();
        return;
    }
    PRINTM(MEVENT, "#\n");
    if (cmd->result != MLAN_STATUS_SUCCESS) {
        PRINTM(MERROR, "Sleep confirm command failed\n");
        pmadapter->pm_wakeup_card_req = MFALSE;
        pmadapter->ps_state = PS_STATE_AWAKE;
        LEAVE();
        return;
    }
    pmadapter->pm_wakeup_card_req = MTRUE;

    if (pmadapter->is_hs_configured) {
        wlan_host_sleep_activated_event(wlan_get_priv
                                        (pmadapter, MLAN_BSS_ROLE_ANY), MTRUE);
    }
    pmadapter->ps_state = PS_STATE_SLEEP;
    LEAVE();
}
#endif  /* CONFIG_MLAN_WMSDK */
/**
 *  @brief This function prepares command of power mode
 *
 *  @param pmpriv		A pointer to mlan_private structure
 *  @param cmd	   		A pointer to HostCmd_DS_COMMAND structure
 *  @param cmd_action   the action: GET or SET
 *  @param ps_bitmap    PS bitmap
 *  @param pdata_buf    A pointer to data buffer
 *  @return         MLAN_STATUS_SUCCESS
 */
mlan_status
wlan_cmd_enh_power_mode(pmlan_private pmpriv,
                        IN HostCmd_DS_COMMAND * cmd,
                        IN t_u16 cmd_action,
                        IN t_u16 ps_bitmap, IN t_void * pdata_buf)
{
    HostCmd_DS_802_11_PS_MODE_ENH *psmode_enh = &cmd->params.psmode_enh;
    t_u8 *tlv = MNULL;
    t_u16 cmd_size = 0;

    ENTER();

    PRINTM(MCMND, "PS Command: action = 0x%x, bitmap = 0x%x\n", cmd_action,
           ps_bitmap);

    cmd->command = wlan_cpu_to_le16(HostCmd_CMD_802_11_PS_MODE_ENH);
    if (cmd_action == DIS_AUTO_PS) {
        psmode_enh->action = wlan_cpu_to_le16(DIS_AUTO_PS);
        psmode_enh->params.ps_bitmap = wlan_cpu_to_le16(ps_bitmap);
        cmd->size = wlan_cpu_to_le16(S_DS_GEN + AUTO_PS_FIX_SIZE);
    } else if (cmd_action == GET_PS) {
        psmode_enh->action = wlan_cpu_to_le16(GET_PS);
        psmode_enh->params.ps_bitmap = wlan_cpu_to_le16(ps_bitmap);
        cmd->size = wlan_cpu_to_le16(S_DS_GEN + AUTO_PS_FIX_SIZE);
    } else if (cmd_action == EN_AUTO_PS) {
        psmode_enh->action = wlan_cpu_to_le16(EN_AUTO_PS);
        psmode_enh->params.auto_ps.ps_bitmap = wlan_cpu_to_le16(ps_bitmap);
        cmd_size = S_DS_GEN + AUTO_PS_FIX_SIZE;
        tlv = (t_u8 *) cmd + cmd_size;
        if (ps_bitmap & BITMAP_STA_PS) {
            pmlan_adapter pmadapter = pmpriv->adapter;
            MrvlIEtypes_ps_param_t *ps_tlv = (MrvlIEtypes_ps_param_t *) tlv;
		ps_param *ps_mode = (ps_param *) &ps_tlv->param;
            ps_tlv->header.type = wlan_cpu_to_le16(TLV_TYPE_PS_PARAM);
            ps_tlv->header.len =
                wlan_cpu_to_le16(sizeof(MrvlIEtypes_ps_param_t) -
                                 sizeof(MrvlIEtypesHeader_t));
            cmd_size += sizeof(MrvlIEtypes_ps_param_t);
            tlv += sizeof(MrvlIEtypes_ps_param_t);
            ps_mode->null_pkt_interval =
                wlan_cpu_to_le16(pmadapter->null_pkt_interval);
            ps_mode->multiple_dtims =
                wlan_cpu_to_le16(pmadapter->multiple_dtim);
            ps_mode->bcn_miss_timeout =
                wlan_cpu_to_le16(pmadapter->bcn_miss_time_out);
            ps_mode->local_listen_interval =
                wlan_cpu_to_le16(pmadapter->local_listen_interval);
            ps_mode->adhoc_wake_period =
                wlan_cpu_to_le16(pmadapter->adhoc_awake_period);
            ps_mode->delay_to_ps = wlan_cpu_to_le16(pmadapter->delay_to_ps);
            ps_mode->mode = wlan_cpu_to_le16(pmadapter->enhanced_ps_mode);
        }

	if (ps_bitmap & BITMAP_AUTO_DS) {
            MrvlIEtypes_auto_ds_param_t *auto_ps_tlv =
                (MrvlIEtypes_auto_ds_param_t *) tlv;
		auto_ds_param *auto_ds = (auto_ds_param *) &auto_ps_tlv->param;
            t_u16 idletime = 0;
            auto_ps_tlv->header.type = wlan_cpu_to_le16(TLV_TYPE_AUTO_DS_PARAM);
            auto_ps_tlv->header.len =
                wlan_cpu_to_le16(sizeof(MrvlIEtypes_auto_ds_param_t) -
                                 sizeof(MrvlIEtypesHeader_t));
            cmd_size += sizeof(MrvlIEtypes_auto_ds_param_t);
            tlv += sizeof(MrvlIEtypes_auto_ds_param_t);
            if (pdata_buf)
                idletime = ((mlan_ds_auto_ds *) pdata_buf)->idletime;
            auto_ds->deep_sleep_timeout = wlan_cpu_to_le16(idletime);
        }
	/* fixme :
	 * This macro is not defined as if now
	 * once full fledged support is added in the SDK
	 * for UAP this macro will be defined and
	 * line below will be uncommented*/
	/*#if defined(UAP_SUPPORT)*/
        if (pdata_buf &&
            (ps_bitmap & (BITMAP_UAP_INACT_PS | BITMAP_UAP_DTIM_PS))) {
            mlan_ds_ps_mgmt *ps_mgmt = (mlan_ds_ps_mgmt *) pdata_buf;
            MrvlIEtypes_sleep_param_t *sleep_tlv = MNULL;
            MrvlIEtypes_inact_sleep_param_t *inact_tlv = MNULL;
            if (ps_mgmt->flags & PS_FLAG_SLEEP_PARAM) {
                sleep_tlv = (MrvlIEtypes_sleep_param_t *) tlv;
                sleep_tlv->header.type =
                    wlan_cpu_to_le16(TLV_TYPE_AP_SLEEP_PARAM);
                sleep_tlv->header.len =
                    wlan_cpu_to_le16(sizeof(MrvlIEtypes_sleep_param_t) -
                                     sizeof(MrvlIEtypesHeader_t));
                sleep_tlv->ctrl_bitmap =
                    wlan_cpu_to_le32(ps_mgmt->sleep_param.ctrl_bitmap);
                sleep_tlv->min_sleep =
                    wlan_cpu_to_le32(ps_mgmt->sleep_param.min_sleep);
                sleep_tlv->max_sleep =
                    wlan_cpu_to_le32(ps_mgmt->sleep_param.max_sleep);
                cmd_size += sizeof(MrvlIEtypes_sleep_param_t);
                tlv += sizeof(MrvlIEtypes_sleep_param_t);
            }
            if (ps_mgmt->flags & PS_FLAG_INACT_SLEEP_PARAM) {
                inact_tlv = (MrvlIEtypes_inact_sleep_param_t *) tlv;
                inact_tlv->header.type =
                    wlan_cpu_to_le16(TLV_TYPE_AP_INACT_SLEEP_PARAM);
                inact_tlv->header.len =
                    wlan_cpu_to_le16(sizeof(MrvlIEtypes_inact_sleep_param_t)
                                     - sizeof(MrvlIEtypesHeader_t));
                inact_tlv->inactivity_to =
                    wlan_cpu_to_le32(ps_mgmt->inact_param.inactivity_to);
                inact_tlv->min_awake =
                    wlan_cpu_to_le32(ps_mgmt->inact_param.min_awake);
                inact_tlv->max_awake =
                    wlan_cpu_to_le32(ps_mgmt->inact_param.max_awake);
                cmd_size += sizeof(MrvlIEtypes_inact_sleep_param_t);
                tlv += sizeof(MrvlIEtypes_inact_sleep_param_t);
            }
        }
	/*#endif*/
        cmd->size = wlan_cpu_to_le16(cmd_size);
    }

    LEAVE();
    return MLAN_STATUS_SUCCESS;
}

#ifndef CONFIG_MLAN_WMSDK
/**
 *  @brief This function handles the command response of ps_mode_enh
 *
 *  @param pmpriv       A pointer to mlan_private structure
 *  @param resp         A pointer to HostCmd_DS_COMMAND
 *  @param pioctl_buf   A pointer to mlan_ioctl_req structure
 *
 *  @return             MLAN_STATUS_SUCCESS
 */
mlan_status
wlan_ret_enh_power_mode(IN pmlan_private pmpriv,
                        IN HostCmd_DS_COMMAND * resp,
                        IN mlan_ioctl_req * pioctl_buf)
{
    pmlan_adapter pmadapter = pmpriv->adapter;
    MrvlIEtypesHeader_t *mrvl_tlv = MNULL;
    MrvlIEtypes_auto_ds_param_t *auto_ds_tlv = MNULL;
    HostCmd_DS_802_11_PS_MODE_ENH *ps_mode = &resp->params.psmode_enh;

    ENTER();

    ps_mode->action = wlan_le16_to_cpu(ps_mode->action);
    PRINTM(MINFO, "CMD_RESP: PS_MODE cmd reply result=%#x action=0x%X\n",
           resp->result, ps_mode->action);
    if (ps_mode->action == EN_AUTO_PS) {
        ps_mode->params.auto_ps.ps_bitmap =
            wlan_le16_to_cpu(ps_mode->params.auto_ps.ps_bitmap);
        if (ps_mode->params.auto_ps.ps_bitmap & BITMAP_AUTO_DS) {
            PRINTM(MCMND, "Enabled auto deep sleep\n");
            pmpriv->adapter->is_deep_sleep = MTRUE;
            mrvl_tlv =
                (MrvlIEtypesHeader_t *) ((t_u8 *) ps_mode + AUTO_PS_FIX_SIZE);
            while (wlan_le16_to_cpu(mrvl_tlv->type) != TLV_TYPE_AUTO_DS_PARAM) {
                mrvl_tlv =
                    (MrvlIEtypesHeader_t *) ((t_u8 *) mrvl_tlv +
                                             wlan_le16_to_cpu(mrvl_tlv->len)
                                             + sizeof(MrvlIEtypesHeader_t));
            }
            auto_ds_tlv = (MrvlIEtypes_auto_ds_param_t *) mrvl_tlv;
            pmpriv->adapter->idle_time =
                wlan_le16_to_cpu(auto_ds_tlv->param.deep_sleep_timeout);
        }
        if (ps_mode->params.auto_ps.ps_bitmap & BITMAP_STA_PS) {
            PRINTM(MCMND, "Enabled STA power save\n");
            if (pmadapter->sleep_period.period) {
                PRINTM(MCMND, "Setting uapsd/pps mode to TRUE\n");
            }
        }
#if defined(UAP_SUPPORT)
        if (ps_mode->params.auto_ps.
            ps_bitmap & (BITMAP_UAP_INACT_PS | BITMAP_UAP_DTIM_PS)) {
            pmadapter->ps_mode = Wlan802_11PowerModePSP;
            PRINTM(MCMND, "Enabled uAP power save\n");
        }
#endif
    } else if (ps_mode->action == DIS_AUTO_PS) {
        ps_mode->params.ps_bitmap = wlan_cpu_to_le16(ps_mode->params.ps_bitmap);
        if (ps_mode->params.ps_bitmap & BITMAP_AUTO_DS) {
            pmpriv->adapter->is_deep_sleep = MFALSE;
            PRINTM(MCMND, "Disabled auto deep sleep\n");
        }
        if (ps_mode->params.ps_bitmap & BITMAP_STA_PS) {
            PRINTM(MCMND, "Disabled STA power save\n");
            if (pmadapter->sleep_period.period) {
                pmadapter->delay_null_pkt = MFALSE;
                pmadapter->tx_lock_flag = MFALSE;
                pmadapter->pps_uapsd_mode = MFALSE;
            }
        }
#if defined(UAP_SUPPORT)
        if (ps_mode->params.
            ps_bitmap & (BITMAP_UAP_INACT_PS | BITMAP_UAP_DTIM_PS)) {
            pmadapter->ps_mode = Wlan802_11PowerModeCAM;
            PRINTM(MCMND, "Disabled uAP power save\n");
        }
#endif
    } else if (ps_mode->action == GET_PS) {
        ps_mode->params.ps_bitmap = wlan_le16_to_cpu(ps_mode->params.ps_bitmap);
        if (ps_mode->params.auto_ps.
            ps_bitmap & (BITMAP_STA_PS | BITMAP_UAP_INACT_PS |
                         BITMAP_UAP_DTIM_PS))
            pmadapter->ps_mode = Wlan802_11PowerModePSP;
        else
            pmadapter->ps_mode = Wlan802_11PowerModeCAM;
        PRINTM(MCMND, "ps_bitmap=0x%x\n", ps_mode->params.ps_bitmap);
        if (pioctl_buf) {
            mlan_ds_pm_cfg *pm_cfg = (mlan_ds_pm_cfg *) pioctl_buf->pbuf;
            if (pm_cfg->sub_command == MLAN_OID_PM_CFG_IEEE_PS) {
                if (ps_mode->params.auto_ps.ps_bitmap & BITMAP_STA_PS)
                    pm_cfg->param.ps_mode = 1;
                else
                    pm_cfg->param.ps_mode = 0;
            }
#if defined(UAP_SUPPORT)
            if (pm_cfg->sub_command == MLAN_OID_PM_CFG_PS_MODE) {
                MrvlIEtypes_sleep_param_t *sleep_tlv = MNULL;
                MrvlIEtypes_inact_sleep_param_t *inact_tlv = MNULL;
                MrvlIEtypesHeader_t *tlv = MNULL;
                t_u16 tlv_type = 0;
                t_u16 tlv_len = 0;
                t_u16 tlv_buf_left = 0;
                pm_cfg->param.ps_mgmt.flags = PS_FLAG_PS_MODE;
                if (ps_mode->params.ps_bitmap & BITMAP_UAP_INACT_PS)
                    pm_cfg->param.ps_mgmt.ps_mode = PS_MODE_INACTIVITY;
                else if (ps_mode->params.ps_bitmap & BITMAP_UAP_DTIM_PS)
                    pm_cfg->param.ps_mgmt.ps_mode = PS_MODE_PERIODIC_DTIM;
                else
                    pm_cfg->param.ps_mgmt.ps_mode = PS_MODE_DISABLE;
                tlv_buf_left = resp->size - (S_DS_GEN + AUTO_PS_FIX_SIZE);
                tlv =
                    (MrvlIEtypesHeader_t *) ((t_u8 *) ps_mode +
                                             AUTO_PS_FIX_SIZE);
                while (tlv_buf_left >= sizeof(MrvlIEtypesHeader_t)) {
                    tlv_type = wlan_le16_to_cpu(tlv->type);
                    tlv_len = wlan_le16_to_cpu(tlv->len);
                    switch (tlv_type) {
                    case TLV_TYPE_AP_SLEEP_PARAM:
                        sleep_tlv = (MrvlIEtypes_sleep_param_t *) tlv;
                        pm_cfg->param.ps_mgmt.flags |= PS_FLAG_SLEEP_PARAM;
                        pm_cfg->param.ps_mgmt.sleep_param.ctrl_bitmap =
                            wlan_le32_to_cpu(sleep_tlv->ctrl_bitmap);
                        pm_cfg->param.ps_mgmt.sleep_param.min_sleep =
                            wlan_le32_to_cpu(sleep_tlv->min_sleep);
                        pm_cfg->param.ps_mgmt.sleep_param.max_sleep =
                            wlan_le32_to_cpu(sleep_tlv->max_sleep);
                        break;
                    case TLV_TYPE_AP_INACT_SLEEP_PARAM:
                        inact_tlv = (MrvlIEtypes_inact_sleep_param_t *) tlv;
                        pm_cfg->param.ps_mgmt.flags |=
                            PS_FLAG_INACT_SLEEP_PARAM;
                        pm_cfg->param.ps_mgmt.inact_param.inactivity_to =
                            wlan_le32_to_cpu(inact_tlv->inactivity_to);
                        pm_cfg->param.ps_mgmt.inact_param.min_awake =
                            wlan_le32_to_cpu(inact_tlv->min_awake);
                        pm_cfg->param.ps_mgmt.inact_param.max_awake =
                            wlan_le32_to_cpu(inact_tlv->max_awake);
                        break;
                    }
                    tlv_buf_left -= tlv_len + sizeof(MrvlIEtypesHeader_t);
                    tlv =
                        (MrvlIEtypesHeader_t *) ((t_u8 *) tlv + tlv_len +
                                                 sizeof(MrvlIEtypesHeader_t));
                }
            }
#endif
        }
    }

    LEAVE();
    return MLAN_STATUS_SUCCESS;
}
#endif /* CONFIG_MLAN_WMSDK */

/**
 *  @brief This function handles the command response of tx rate query
 *
 *  @param pmpriv       A pointer to mlan_private structure
 *  @param resp         A pointer to HostCmd_DS_COMMAND
 *  @param pioctl_buf   A pointer to mlan_ioctl_req structure
 *
 *  @return             MLAN_STATUS_SUCCESS
 */
mlan_status
wlan_ret_802_11_tx_rate_query(IN pmlan_private pmpriv,
                              IN HostCmd_DS_COMMAND * resp,
                              IN mlan_ioctl_req * pioctl_buf)
{
    mlan_adapter *pmadapter = pmpriv->adapter;
#ifndef CONFIG_MLAN_WMSDK
    mlan_ds_rate *rate = MNULL;
#endif /* CONFIG_MLAN_WMSDK */
    ENTER();

    pmpriv->tx_rate = resp->params.tx_rate.tx_rate;
    pmpriv->tx_htinfo = resp->params.tx_rate.ht_info;
    if (!pmpriv->is_data_rate_auto) {
        pmpriv->data_rate = wlan_index_to_data_rate(pmadapter,
                                                    pmpriv->tx_rate,
                                                    pmpriv->tx_htinfo);
    }

    if (pioctl_buf) {
#ifndef CONFIG_MLAN_WMSDK
        rate = (mlan_ds_rate *) pioctl_buf->pbuf;
        if (rate->sub_command == MLAN_OID_RATE_CFG) {
            if (rate->param.rate_cfg.rate_type == MLAN_RATE_INDEX) {
                if (pmpriv->tx_htinfo & MBIT(0))
                    rate->param.rate_cfg.rate =
                        pmpriv->tx_rate + MLAN_RATE_INDEX_MCS0;
                else
                    /* For HostCmd_CMD_802_11_TX_RATE_QUERY, there is a hole in
                       rate table between HR/DSSS and OFDM rates, so minus 1
                       for OFDM rate index */
                    rate->param.rate_cfg.rate =
                        (pmpriv->tx_rate >
                         MLAN_RATE_INDEX_OFDM0) ? pmpriv->tx_rate -
                        1 : pmpriv->tx_rate;
            } else {
                rate->param.rate_cfg.rate =
                    wlan_index_to_data_rate(pmadapter, pmpriv->tx_rate,
                                            pmpriv->tx_htinfo);
            }
        } else if (rate->sub_command == MLAN_OID_GET_DATA_RATE) {
            if (pmpriv->tx_htinfo & MBIT(0)) {
                rate->param.data_rate.tx_data_rate =
                    pmpriv->tx_rate + MLAN_RATE_INDEX_MCS0;
                if (pmpriv->tx_htinfo & MBIT(1))
                    rate->param.data_rate.tx_ht_bw = MLAN_HT_BW40;
                else
                    rate->param.data_rate.tx_ht_bw = MLAN_HT_BW20;
                if (pmpriv->tx_htinfo & MBIT(2))
                    rate->param.data_rate.tx_ht_gi = MLAN_HT_SGI;
                else
                    rate->param.data_rate.tx_ht_gi = MLAN_HT_LGI;
            } else
                /* For HostCmd_CMD_802_11_TX_RATE_QUERY, there is a hole in
                   rate table between HR/DSSS and OFDM rates, so minus 1 for
                   OFDM rate index */
                rate->param.data_rate.tx_data_rate =
                    (pmpriv->tx_rate >
                     MLAN_RATE_INDEX_OFDM0) ? pmpriv->tx_rate -
                    1 : pmpriv->tx_rate;
            if (pmpriv->rxpd_htinfo & MBIT(0)) {
                rate->param.data_rate.rx_data_rate =
                    pmpriv->rxpd_rate + MLAN_RATE_INDEX_MCS0;
                if (pmpriv->rxpd_htinfo & MBIT(1))
                    rate->param.data_rate.rx_ht_bw = MLAN_HT_BW40;
                else
                    rate->param.data_rate.rx_ht_bw = MLAN_HT_BW20;
                if (pmpriv->tx_htinfo & MBIT(2))
                    rate->param.data_rate.rx_ht_gi = MLAN_HT_SGI;
                else
                    rate->param.data_rate.rx_ht_gi = MLAN_HT_LGI;
            } else
                /* For rate index in RxPD, there is a hole in rate table
                   between HR/DSSS and OFDM rates, so minus 1 for OFDM rate
                   index */
                rate->param.data_rate.rx_data_rate =
                    (pmpriv->rxpd_rate >
                     MLAN_RATE_INDEX_OFDM0) ? pmpriv->rxpd_rate -
                    1 : pmpriv->rxpd_rate;
        }
        pioctl_buf->data_read_written =
            sizeof(mlan_multicast_list) + MLAN_SUB_COMMAND_SIZE;
#endif /* CONFIG_MLAN_WMSDK */
    }
    LEAVE();
    return MLAN_STATUS_SUCCESS;
}

/**
 *  @brief This function prepares command of tx_rate_cfg.
 *
 *  @param pmpriv       A pointer to mlan_private structure
 *  @param cmd          A pointer to HostCmd_DS_COMMAND structure
 *  @param cmd_action   The action: GET or SET
 *  @param pdata_buf    A pointer to data buffer
 *
 *  @return             MLAN_STATUS_SUCCESS
 */
mlan_status
wlan_cmd_tx_rate_cfg(IN pmlan_private pmpriv,
                     IN HostCmd_DS_COMMAND * cmd,
                     IN t_u16 cmd_action, IN t_void * pdata_buf)
{
    HostCmd_DS_TX_RATE_CFG *rate_cfg = &cmd->params.tx_rate_cfg;
    MrvlRateScope_t *rate_scope;
    MrvlRateDropPattern_t *rate_drop;
    t_u16 *pbitmap_rates = (t_u16 *) pdata_buf;

    t_u32 i;

    ENTER();

    cmd->command = wlan_cpu_to_le16(HostCmd_CMD_TX_RATE_CFG);

    rate_cfg->action = wlan_cpu_to_le16(cmd_action);
    rate_cfg->cfg_index = 0;

    rate_scope = (MrvlRateScope_t *) ((t_u8 *) rate_cfg +
                                      sizeof(HostCmd_DS_TX_RATE_CFG));
    rate_scope->type = wlan_cpu_to_le16(TLV_TYPE_RATE_SCOPE);
    rate_scope->length = wlan_cpu_to_le16(sizeof(MrvlRateScope_t) -
                                          sizeof(MrvlIEtypesHeader_t));
    if (pbitmap_rates != MNULL) {
        rate_scope->hr_dsss_rate_bitmap = wlan_cpu_to_le16(pbitmap_rates[0]);
        rate_scope->ofdm_rate_bitmap = wlan_cpu_to_le16(pbitmap_rates[1]);
        for (i = 0; i < sizeof(rate_scope->ht_mcs_rate_bitmap) / sizeof(t_u16);
             i++)
            rate_scope->ht_mcs_rate_bitmap[i] =
                wlan_cpu_to_le16(pbitmap_rates[2 + i]);
    } else {
        rate_scope->hr_dsss_rate_bitmap =
            wlan_cpu_to_le16(pmpriv->bitmap_rates[0]);
        rate_scope->ofdm_rate_bitmap =
            wlan_cpu_to_le16(pmpriv->bitmap_rates[1]);
        for (i = 0; i < sizeof(rate_scope->ht_mcs_rate_bitmap) / sizeof(t_u16);
             i++)
            rate_scope->ht_mcs_rate_bitmap[i] =
                wlan_cpu_to_le16(pmpriv->bitmap_rates[2 + i]);
    }

    rate_drop = (MrvlRateDropPattern_t *) ((t_u8 *) rate_scope +
                                           sizeof(MrvlRateScope_t));
    rate_drop->type = wlan_cpu_to_le16(TLV_TYPE_RATE_DROP_PATTERN);
    rate_drop->length = wlan_cpu_to_le16(sizeof(rate_drop->rate_drop_mode));
    rate_drop->rate_drop_mode = 0;

    cmd->size = wlan_cpu_to_le16(S_DS_GEN + sizeof(HostCmd_DS_TX_RATE_CFG) +
                                 sizeof(MrvlRateScope_t) +
                                 sizeof(MrvlRateDropPattern_t));

    LEAVE();
    return MLAN_STATUS_SUCCESS;
}

/**
 *  @brief This function handles the command response of tx_rate_cfg
 *
 *  @param pmpriv       A pointer to mlan_private structure
 *  @param resp         A pointer to HostCmd_DS_COMMAND
 *  @param pioctl_buf   A pointer to mlan_ioctl_req structure
 *
 *  @return             MLAN_STATUS_SUCCESS or MLAN_STATUS_FAILURE
 */
mlan_status
wlan_ret_tx_rate_cfg(IN pmlan_private pmpriv,
                     IN HostCmd_DS_COMMAND * resp,
                     IN mlan_ioctl_req * pioctl_buf)
{
    mlan_adapter *pmadapter = pmpriv->adapter;
    /*mlan_ds_rate *ds_rate = MNULL;*/
    HostCmd_DS_TX_RATE_CFG *prate_cfg = MNULL;
    MrvlRateScope_t *prate_scope;
    MrvlIEtypesHeader_t *head = MNULL;
    t_u16 tlv, tlv_buf_len;
    t_u8 *tlv_buf;
    t_u32 i;
    t_s32 index;
    t_u8 rate_index = -1;
    mlan_status ret = MLAN_STATUS_SUCCESS;

    ENTER();

    if (resp == MNULL) {
        LEAVE();
        return MLAN_STATUS_FAILURE;
    }
    prate_cfg = &resp->params.tx_rate_cfg;

    tlv_buf = (t_u8 *) ((t_u8 *) prate_cfg) + sizeof(HostCmd_DS_TX_RATE_CFG);
    tlv_buf_len = *(t_u16 *) (tlv_buf + sizeof(t_u16));
    tlv_buf_len = wlan_le16_to_cpu(tlv_buf_len);

    while (tlv_buf && tlv_buf_len > 0) {
        tlv = (*tlv_buf);
        tlv = tlv | (*(tlv_buf + 1) << 8);

        switch (tlv) {
        case TLV_TYPE_RATE_SCOPE:
            prate_scope = (MrvlRateScope_t *) tlv_buf;
            pmpriv->bitmap_rates[0] =
                wlan_le16_to_cpu(prate_scope->hr_dsss_rate_bitmap);
            pmpriv->bitmap_rates[1] =
                wlan_le16_to_cpu(prate_scope->ofdm_rate_bitmap);
            for (i = 0;
                 i < sizeof(prate_scope->ht_mcs_rate_bitmap) / sizeof(t_u16);
                 i++)
                pmpriv->bitmap_rates[2 + i] =
                    wlan_le16_to_cpu(prate_scope->ht_mcs_rate_bitmap[i]);
            break;
            /* Add RATE_DROP tlv here */
        }

        head = (MrvlIEtypesHeader_t *) tlv_buf;
        head->len = wlan_le16_to_cpu(head->len);
        tlv_buf += head->len + sizeof(MrvlIEtypesHeader_t);
        tlv_buf_len -= head->len;
    }
    pmpriv->is_data_rate_auto = wlan_is_rate_auto(pmpriv);

    if (pmpriv->is_data_rate_auto) {
        pmpriv->data_rate = 0;
	PRINTM(MINFO, "Rate is Auto\r\n");
    } else {
	    /* check the LG rate */
	    index = wlan_get_rate_index(pmadapter, &pmpriv->bitmap_rates[0], 4);
	    if (index != -1) {
		    if ((index >= MLAN_RATE_BITMAP_OFDM0) &&
				    (index <= MLAN_RATE_BITMAP_OFDM7))
			    index -= (MLAN_RATE_BITMAP_OFDM0 - MLAN_RATE_INDEX_OFDM0);
		    rate_index = index;
	    }
	    /* check the HT rate */
	    index = wlan_get_rate_index(pmadapter,
			    &pmpriv->bitmap_rates[2], 16);
	    if (index != -1) {
		    rate_index = index + MLAN_RATE_INDEX_MCS0;
	    }

	    if (rate_index != -1)
		    PRINTM(MINFO, "Rate index is :: %d\r\n", rate_index);
	    else
		    PRINTM(MINFO, "Rate index is not changed\r\n");
#ifndef CONFIG_MLAN_WMSDK
        ret = wlan_prepare_cmd(pmpriv,
                               HostCmd_CMD_802_11_TX_RATE_QUERY,
                               HostCmd_ACT_GEN_GET, 0, MNULL, MNULL);
#endif /* CONFIG_MLAN_WMSDK */
    }

#ifndef CONFIG_MLAN_WMSDK
    if (pioctl_buf) {
        ds_rate = (mlan_ds_rate *) pioctl_buf->pbuf;
        if (ds_rate == MNULL) {
            PRINTM(MERROR, "Request buffer not found!\n");
            LEAVE();
            return MLAN_STATUS_FAILURE;
        }
        if (pmpriv->is_data_rate_auto) {
            ds_rate->param.rate_cfg.is_rate_auto = MTRUE;
        } else {
            ds_rate->param.rate_cfg.is_rate_auto = MFALSE;
            /* check the LG rate */
            index = wlan_get_rate_index(pmadapter, &pmpriv->bitmap_rates[0], 4);
            if (index != -1) {
                if ((index >= MLAN_RATE_BITMAP_OFDM0) &&
                    (index <= MLAN_RATE_BITMAP_OFDM7))
                    index -= (MLAN_RATE_BITMAP_OFDM0 - MLAN_RATE_INDEX_OFDM0);
                ds_rate->param.rate_cfg.rate = index;
            }
            /* check the HT rate */
            index = wlan_get_rate_index(pmadapter,
                                        &pmpriv->bitmap_rates[2], 16);
            if (index != -1) {
                ds_rate->param.rate_cfg.rate = index + MLAN_RATE_INDEX_MCS0;
            }
            PRINTM(MINFO, "Rate index is %d\n", ds_rate->param.rate_cfg.rate);
        }
        for (i = 0; i < MAX_BITMAP_RATES_SIZE; i++) {
            ds_rate->param.rate_cfg.bitmap_rates[i] = pmpriv->bitmap_rates[i];
        }

    }
#endif /* CONFIG_MLAN_WMSDK */

    LEAVE();
    return ret;
}

#ifndef CONFIG_MLAN_WMSDK
/**
 *  @brief  This function issues adapter specific commands
 *  		to initialize firmware
 *
 *  @param pmadapter    A pointer to mlan_adapter structure
 *
 *  @return             MLAN_STATUS_PENDING or MLAN_STATUS_FAILURE
 */
mlan_status
wlan_adapter_init_cmd(IN pmlan_adapter pmadapter)
{
    mlan_status ret = MLAN_STATUS_SUCCESS;
    pmlan_private pmpriv = MNULL;
#ifdef STA_SUPPORT
    pmlan_private pmpriv_sta = MNULL;
#endif

    ENTER();

    pmpriv = wlan_get_priv(pmadapter, MLAN_BSS_ROLE_ANY);
#ifdef STA_SUPPORT
    pmpriv_sta = wlan_get_priv(pmadapter, MLAN_BSS_ROLE_STA);
#endif

    /*
     * This should be issued in the very first to config
     *   SDIO_GPIO interrupt mode.
     */
    if (wlan_set_sdio_gpio_int(pmpriv) != MLAN_STATUS_SUCCESS) {
        ret = MLAN_STATUS_FAILURE;
        goto done;
    }

    ret = wlan_prepare_cmd(pmpriv, HostCmd_CMD_FUNC_INIT,
                           HostCmd_ACT_GEN_SET, 0, MNULL, MNULL);
    if (ret) {
        ret = MLAN_STATUS_FAILURE;
        goto done;
    }

    /** Cal data dnld cmd prepare */
    if ((pmadapter->pcal_data) && (pmadapter->cal_data_len > 0)) {
        ret =
            wlan_prepare_cmd(pmpriv, HostCmd_CMD_CFG_DATA, HostCmd_ACT_GEN_SET,
                             0, MNULL, MNULL);
        if (ret) {
            ret = MLAN_STATUS_FAILURE;
            goto done;
        }
        pmadapter->pcal_data = MNULL;
        pmadapter->cal_data_len = 0;
    }

    /*
     * Get HW spec
     */
    ret = wlan_prepare_cmd(pmpriv, HostCmd_CMD_GET_HW_SPEC,
                           HostCmd_ACT_GEN_GET, 0, MNULL, MNULL);
    if (ret) {
        ret = MLAN_STATUS_FAILURE;
        goto done;
    }

    /* Reconfigure tx buf size */
    ret = wlan_prepare_cmd(pmpriv, HostCmd_CMD_RECONFIGURE_TX_BUFF,
                           HostCmd_ACT_GEN_SET, 0, MNULL,
                           &pmadapter->max_tx_buf_size);
    if (ret) {
        ret = MLAN_STATUS_FAILURE;
        goto done;
    }
#if defined(STA_SUPPORT)
    if (pmpriv_sta && (pmpriv_sta->state_11d.user_enable_11d == ENABLE_11D)) {
        /* Send command to FW to enable 11d */
        ret = wlan_prepare_cmd(pmpriv_sta,
                               HostCmd_CMD_802_11_SNMP_MIB,
                               HostCmd_ACT_GEN_SET,
                               Dot11D_i,
                               MNULL, &pmpriv_sta->state_11d.user_enable_11d);
        if (ret) {
            ret = MLAN_STATUS_FAILURE;
            goto done;
        }
    }
#endif

#if defined(STA_SUPPORT)
    if (pmpriv_sta && (pmadapter->ps_mode == Wlan802_11PowerModePSP)) {
        ret = wlan_prepare_cmd(pmpriv_sta, HostCmd_CMD_802_11_PS_MODE_ENH,
                               EN_AUTO_PS, BITMAP_STA_PS, MNULL, MNULL);
        if (ret) {
            ret = MLAN_STATUS_FAILURE;
            goto done;
        }
    }
#endif

    if (pmadapter->init_auto_ds) {
        mlan_ds_auto_ds auto_ds;
        /* Enable auto deep sleep */
        auto_ds.idletime = pmadapter->idle_time;
        ret = wlan_prepare_cmd(pmpriv, HostCmd_CMD_802_11_PS_MODE_ENH,
                               EN_AUTO_PS, BITMAP_AUTO_DS, MNULL, &auto_ds);
        if (ret) {
            ret = MLAN_STATUS_FAILURE;
            goto done;
        }
    }

    ret = MLAN_STATUS_PENDING;
  done:
    LEAVE();
    return ret;
}
#endif /* CONFIG_MLAN_WMSDK */
/**
 *  @brief This function prepares command of get_hw_spec.
 *
 *  @param pmpriv       A pointer to mlan_private structure
 *  @param pcmd         A pointer to HostCmd_DS_COMMAND structure
 *
 *  @return             MLAN_STATUS_SUCCESS
 */
mlan_status
wlan_cmd_get_hw_spec(IN pmlan_private pmpriv, IN HostCmd_DS_COMMAND * pcmd)
{
    HostCmd_DS_GET_HW_SPEC *hw_spec = &pcmd->params.hw_spec;

    ENTER();

    pcmd->command = wlan_cpu_to_le16(HostCmd_CMD_GET_HW_SPEC);
    pcmd->size = wlan_cpu_to_le16(sizeof(HostCmd_DS_GET_HW_SPEC) + S_DS_GEN);
    memcpy(pmpriv->adapter, hw_spec->permanent_addr, pmpriv->curr_addr,
           MLAN_MAC_ADDR_LENGTH);

    LEAVE();
    return MLAN_STATUS_SUCCESS;
}

#ifndef CONFIG_MLAN_WMSDK
/**
 *  @brief This function prepares command of set_cfg_data.
 *
 *  @param pmpriv       A pointer to mlan_private strcture
 *  @param pcmd         A pointer to HostCmd_DS_COMMAND structure
 *  @param cmd_action   Command action: GET or SET
 *  @param pdata_buf    A pointer to cal_data buf
 *
 *  @return             MLAN_STATUS_SUCCESS
 */
mlan_status
wlan_cmd_cfg_data(IN pmlan_private pmpriv,
                  IN HostCmd_DS_COMMAND * pcmd,
                  IN t_u16 cmd_action, IN t_void * pdata_buf)
{
    mlan_status ret = MLAN_STATUS_SUCCESS;
    HostCmd_DS_802_11_CFG_DATA *pcfg_data = &(pcmd->params.cfg_data);
    pmlan_adapter pmadapter = pmpriv->adapter;
    t_u32 len;
    t_u32 cal_data_offset;
    t_u8 *temp_pcmd = (t_u8 *) pcmd;

    ENTER();

    cal_data_offset = S_DS_GEN + sizeof(HostCmd_DS_802_11_CFG_DATA);
    if ((pmadapter->pcal_data) && (pmadapter->cal_data_len > 0)) {
        len = wlan_parse_cal_cfg((t_u8 *) pmadapter->pcal_data,
                                 pmadapter->cal_data_len,
                                 (t_u8 *) (temp_pcmd + cal_data_offset));
    } else {
        ret = MLAN_STATUS_FAILURE;
        goto done;
    }

    pcfg_data->action = cmd_action;
    pcfg_data->type = 2;        /* cal data type */
    pcfg_data->data_len = len;

    pcmd->command = HostCmd_CMD_CFG_DATA;
    pcmd->size = pcfg_data->data_len + cal_data_offset;

    pcmd->command = wlan_cpu_to_le16(pcmd->command);
    pcmd->size = wlan_cpu_to_le16(pcmd->size);

    pcfg_data->action = wlan_cpu_to_le16(pcfg_data->action);
    pcfg_data->type = wlan_cpu_to_le16(pcfg_data->type);
    pcfg_data->data_len = wlan_cpu_to_le16(pcfg_data->data_len);

  done:
    LEAVE();
    return ret;
}

/**
 *  @brief This function handles the command response of set_cfg_data
 *
 *  @param pmpriv       A pointer to mlan_private structure
 *  @param resp         A pointer to HostCmd_DS_COMMAND
 *  @param pioctl_buf   A pointer to A pointer to mlan_ioctl_req
 *
 *  @return             MLAN_STATUS_SUCCESS or MLAN_STATUS_FAILURE
 */
mlan_status
wlan_ret_cfg_data(IN pmlan_private pmpriv,
                  IN HostCmd_DS_COMMAND * resp, IN t_void * pioctl_buf)
{
    mlan_status ret = MLAN_STATUS_SUCCESS;

    ENTER();

    if (resp->result != HostCmd_RESULT_OK) {
        PRINTM(MERROR, "Cal data cmd resp failed\n");
        ret = MLAN_STATUS_FAILURE;
    }
    LEAVE();
    return ret;
}
#endif /* CONFIG_MLAN_WMSDK */

/**
 *  @brief This function handles the command response of get_hw_spec
 *
 *  @param pmpriv       A pointer to mlan_private structure
 *  @param resp         A pointer to HostCmd_DS_COMMAND
 *  @param pioctl_buf   A pointer to command buffer
 *
 *  @return             MLAN_STATUS_SUCCESS or MLAN_STATUS_FAILURE
 */
mlan_status
wlan_ret_get_hw_spec(IN pmlan_private pmpriv,
                     IN HostCmd_DS_COMMAND * resp, IN t_void * pioctl_buf)
{
    HostCmd_DS_GET_HW_SPEC *hw_spec = &resp->params.hw_spec;
    mlan_adapter *pmadapter = pmpriv->adapter;
    mlan_status ret = MLAN_STATUS_SUCCESS;
    t_u32 i;
    pmlan_ioctl_req pioctl_req = (mlan_ioctl_req *) pioctl_buf;

    ENTER();

    pmadapter->fw_cap_info = wlan_le32_to_cpu(hw_spec->fw_cap_info);
#ifdef STA_SUPPORT
    if (IS_SUPPORT_MULTI_BANDS(pmadapter)) {
        pmadapter->fw_bands = (t_u8) GET_FW_DEFAULT_BANDS(pmadapter);
#ifndef CONFIG_5GHz_SUPPORT
    /* fixme: Re-check if this is the correct way to disable 5 GHz. */
	pmadapter->fw_bands &= ~BAND_A;
#endif /* CONFIG_5GHz_SUPPORT */
    } else {
        pmadapter->fw_bands = BAND_B;
    }

    pmadapter->config_bands = pmadapter->fw_bands;
    for (i = 0; i < pmadapter->priv_num; i++) {
        if (pmadapter->priv[i])
            pmadapter->priv[i]->config_bands = pmadapter->fw_bands;
    }

    if (pmadapter->fw_bands & BAND_A) {
        if (pmadapter->fw_bands & BAND_GN) {
            pmadapter->config_bands |= BAND_AN;
            for (i = 0; i < pmadapter->priv_num; i++) {
                if (pmadapter->priv[i])
                    pmadapter->priv[i]->config_bands |= BAND_AN;
            }

            pmadapter->fw_bands |= BAND_AN;
        }
        if ((pmadapter->fw_bands & BAND_AN)
            ) {
            pmadapter->adhoc_start_band = BAND_A | BAND_AN;
            pmadapter->adhoc_11n_enabled = MTRUE;
        } else
            pmadapter->adhoc_start_band = BAND_A;
        pmpriv->adhoc_channel = DEFAULT_AD_HOC_CHANNEL_A;
    } else if ((pmadapter->fw_bands & BAND_GN)
        ) {
        pmadapter->adhoc_start_band = BAND_G | BAND_B | BAND_GN;
        pmpriv->adhoc_channel = DEFAULT_AD_HOC_CHANNEL;
        pmadapter->adhoc_11n_enabled = MTRUE;
    } else if (pmadapter->fw_bands & BAND_G) {
        pmadapter->adhoc_start_band = BAND_G | BAND_B;
        pmpriv->adhoc_channel = DEFAULT_AD_HOC_CHANNEL;
    } else if (pmadapter->fw_bands & BAND_B) {
        pmadapter->adhoc_start_band = BAND_B;
        pmpriv->adhoc_channel = DEFAULT_AD_HOC_CHANNEL;
    }
#endif /* STA_SUPPORT */

    pmadapter->fw_release_number = hw_spec->fw_release_number;
    pmadapter->number_of_antenna = wlan_le16_to_cpu(hw_spec->number_of_antenna);

    PRINTM(MINFO, "GET_HW_SPEC: fw_release_number- 0x%X\n",
           wlan_le32_to_cpu(pmadapter->fw_release_number));
    PRINTM(MINFO, "GET_HW_SPEC: Permanent addr- %2x:%2x:%2x:%2x:%2x:%2x\n",
           hw_spec->permanent_addr[0], hw_spec->permanent_addr[1],
           hw_spec->permanent_addr[2], hw_spec->permanent_addr[3],
           hw_spec->permanent_addr[4], hw_spec->permanent_addr[5]);
    PRINTM(MINFO, "GET_HW_SPEC: hw_if_version=0x%X  version=0x%X\n",
           wlan_le16_to_cpu(hw_spec->hw_if_version),
           wlan_le16_to_cpu(hw_spec->version));

    if (pmpriv->curr_addr[0] == 0xff) {
        memmove(pmadapter, pmpriv->curr_addr, hw_spec->permanent_addr,
                MLAN_MAC_ADDR_LENGTH);
#ifdef CONFIG_P2P
	memmove(pmadapter, pmpriv->curr_p2p_addr, hw_spec->permanent_addr,
                MLAN_MAC_ADDR_LENGTH);
	pmpriv->curr_p2p_addr[0] |= (0x01 << 1);
#endif
    }

    pmadapter->hw_dot_11n_dev_cap = wlan_le32_to_cpu(hw_spec->dot_11n_dev_cap);
    pmadapter->usr_dot_11n_dev_cap_bg = pmadapter->hw_dot_11n_dev_cap &
        DEFAULT_11N_CAP_MASK_BG;
    pmadapter->usr_dot_11n_dev_cap_a = pmadapter->hw_dot_11n_dev_cap &
        DEFAULT_11N_CAP_MASK_A;
    pmadapter->usr_dev_mcs_support = pmadapter->hw_dev_mcs_support =
        hw_spec->dev_mcs_support;
    wlan_show_dot11ndevcap(pmadapter, pmadapter->hw_dot_11n_dev_cap);
    wlan_show_devmcssupport(pmadapter, pmadapter->hw_dev_mcs_support);
    pmadapter->mp_end_port = wlan_le16_to_cpu(hw_spec->mp_end_port);

    for (i = 1; i <= (unsigned) (MAX_PORT - pmadapter->mp_end_port); i++) {
        pmadapter->mp_data_port_mask &= ~(1 << (MAX_PORT - i));
    }

#ifndef CONFIG_MLAN_WMSDK
    pmadapter->max_mgmt_ie_index = wlan_le16_to_cpu(hw_spec->mgmt_buf_count);
    PRINTM(MINFO, "GET_HW_SPEC: mgmt IE count=%d\n",
           pmadapter->max_mgmt_ie_index);
    if (!pmadapter->max_mgmt_ie_index)
        pmadapter->max_mgmt_ie_index = MAX_MGMT_IE_INDEX;
#endif /* CONFIG_MLAN_WMSDK */

    pmadapter->region_code = wlan_le16_to_cpu(hw_spec->region_code);
    for (i = 0; i < MRVDRV_MAX_REGION_CODE; i++) {
        /* Use the region code to search for the index */
        if (pmadapter->region_code == region_code_index[i])
            break;
    }
    /* If it's unidentified region code, use the default */
    if (i >= MRVDRV_MAX_REGION_CODE) {
        pmadapter->region_code = MRVDRV_DEFAULT_REGION_CODE;
        PRINTM(MWARN, "unidentified region code, use the default (0x%02x)\n",
               MRVDRV_DEFAULT_REGION_CODE);
    }
    /* Synchronize CFP code with region code */
    pmadapter->cfp_code_bg = pmadapter->region_code;
    pmadapter->cfp_code_a = pmadapter->region_code;

    if (wlan_set_regiontable(pmpriv, (t_u8) pmadapter->region_code,
                             pmadapter->fw_bands)) {
        if (pioctl_req)
            pioctl_req->status_code = MLAN_ERROR_CMD_SCAN_FAIL;
        ret = MLAN_STATUS_FAILURE;
        goto done;
    }
#ifdef STA_SUPPORT
    if (wlan_11d_set_universaltable(pmpriv, pmadapter->fw_bands)) {
        if (pioctl_req)
            pioctl_req->status_code = MLAN_ERROR_CMD_SCAN_FAIL;
        ret = MLAN_STATUS_FAILURE;
        goto done;
    }
#endif /* STA_SUPPORT */

 done:
    LEAVE();
    return ret;
}

#ifndef CONFIG_MLAN_WMSDK
/**
 *  @brief This function prepares command of radio_control.
 *
 *  @param pmpriv       A pointer to mlan_private structure
 *  @param cmd          A pointer to HostCmd_DS_COMMAND structure
 *  @param cmd_action   The action: GET or SET
 *  @param pdata_buf    A pointer to data buffer
 *
 *  @return             MLAN_STATUS_SUCCESS
 */
mlan_status
wlan_cmd_802_11_radio_control(IN pmlan_private pmpriv,
                              IN HostCmd_DS_COMMAND * cmd,
                              IN t_u16 cmd_action, IN t_void * pdata_buf)
{
    HostCmd_DS_802_11_RADIO_CONTROL *pradio_control = &cmd->params.radio;
    t_u32 radio_ctl;
    ENTER();
    cmd->size = wlan_cpu_to_le16((sizeof(HostCmd_DS_802_11_RADIO_CONTROL))
                                 + S_DS_GEN);
    cmd->command = wlan_cpu_to_le16(HostCmd_CMD_802_11_RADIO_CONTROL);
    pradio_control->action = wlan_cpu_to_le16(cmd_action);
    memcpy(pmpriv->adapter, &radio_ctl, pdata_buf, sizeof(t_u32));
    pradio_control->control = wlan_cpu_to_le16((t_u16) radio_ctl);
    LEAVE();
    return MLAN_STATUS_SUCCESS;
}

/**
 *  @brief This function handles the command response of radio_control
 *
 *  @param pmpriv       A pointer to mlan_private structure
 *  @param resp         A pointer to HostCmd_DS_COMMAND
 *  @param pioctl_buf   A pointer to mlan_ioctl_req structure
 *
 *  @return             MLAN_STATUS_SUCCESS
 */
mlan_status
wlan_ret_802_11_radio_control(IN pmlan_private pmpriv,
                              IN HostCmd_DS_COMMAND * resp,
                              IN mlan_ioctl_req * pioctl_buf)
{
    HostCmd_DS_802_11_RADIO_CONTROL *pradio_ctrl =
        (HostCmd_DS_802_11_RADIO_CONTROL *) & resp->params.radio;
    mlan_ds_radio_cfg *radio_cfg = MNULL;
    mlan_adapter *pmadapter = pmpriv->adapter;

    ENTER();
    pmadapter->radio_on = wlan_le16_to_cpu(pradio_ctrl->control);
    if (pioctl_buf) {
        radio_cfg = (mlan_ds_radio_cfg *) pioctl_buf->pbuf;
        radio_cfg->param.radio_on_off = (t_u32) pmadapter->radio_on;
        pioctl_buf->data_read_written = sizeof(mlan_ds_radio_cfg);
    }
    LEAVE();
    return MLAN_STATUS_SUCCESS;
}
#endif /* CONFIG_MLAN_WMSDK */

/**
 *  @brief This function prepares command of remain_on_channel.
 *
 *  @param pmpriv       A pointer to mlan_private structure
 *  @param cmd          A pointer to HostCmd_DS_COMMAND structure
 *  @param cmd_action   The action: GET or SET
 *  @param pdata_buf    A pointer to data buffer
 *
 *  @return             MLAN_STATUS_SUCCESS
 */
mlan_status
wlan_cmd_remain_on_channel(IN pmlan_private pmpriv,
                           IN HostCmd_DS_COMMAND * cmd,
                           IN t_u16 cmd_action, IN t_void * pdata_buf)
{
    HostCmd_DS_REMAIN_ON_CHANNEL *remain_channel = &cmd->params.remain_on_chan;
    mlan_ds_remain_chan *cfg = (mlan_ds_remain_chan *) pdata_buf;
    ENTER();
    cmd->size = wlan_cpu_to_le16((sizeof(HostCmd_DS_REMAIN_ON_CHANNEL))
                                 + S_DS_GEN);
    cmd->command = wlan_cpu_to_le16(HostCmd_CMD_802_11_REMAIN_ON_CHANNEL);
    remain_channel->action = cmd_action;
    if (cmd_action == HostCmd_ACT_GEN_SET) {
        if (cfg->remove) {
            remain_channel->action = HostCmd_ACT_GEN_REMOVE;
        } else {
	    remain_channel->status = 0;
	    remain_channel->reserved = 0;
            remain_channel->bandcfg = cfg->bandcfg;
            remain_channel->channel = cfg->channel;
            remain_channel->remain_period =
                wlan_cpu_to_le32(cfg->remain_period);
        }
    }
    remain_channel->action = wlan_cpu_to_le16(remain_channel->action);

    LEAVE();
    return MLAN_STATUS_SUCCESS;
}

#ifndef CONFIG_MLAN_WMSDK
#ifdef WIFI_DIRECT_SUPPORT
/**
 *  @brief This function handles the command response of remain_on_channel
 *
 *  @param pmpriv       A pointer to mlan_private structure
 *  @param resp         A pointer to HostCmd_DS_COMMAND
 *  @param pioctl_buf   A pointer to mlan_ioctl_req structure
 *
 *  @return             MLAN_STATUS_SUCCESS
 */
mlan_status
wlan_ret_remain_on_channel(IN pmlan_private pmpriv,
                           IN HostCmd_DS_COMMAND * resp,
                           IN mlan_ioctl_req * pioctl_buf)
{
    HostCmd_DS_REMAIN_ON_CHANNEL *remain_channel = &resp->params.remain_on_chan;
    mlan_ds_radio_cfg *radio_cfg = MNULL;

    ENTER();
    if (pioctl_buf) {
        radio_cfg = (mlan_ds_radio_cfg *) pioctl_buf->pbuf;
        radio_cfg->param.remain_chan.status = remain_channel->status;
        radio_cfg->param.remain_chan.bandcfg = remain_channel->bandcfg;
        radio_cfg->param.remain_chan.channel = remain_channel->channel;
        radio_cfg->param.remain_chan.remain_period =
            wlan_le32_to_cpu(remain_channel->remain_period);
        pioctl_buf->data_read_written = sizeof(mlan_ds_radio_cfg);
    }
    LEAVE();
    return MLAN_STATUS_SUCCESS;
}

/**
 *  @brief This function prepares command of wifi direct mode.
 *
 *  @param pmpriv       A pointer to mlan_private structure
 *  @param cmd          A pointer to HostCmd_DS_COMMAND structure
 *  @param cmd_action   The action: GET or SET
 *  @param pdata_buf    A pointer to data buffer
 *
 *  @return             MLAN_STATUS_SUCCESS
 */
mlan_status
wlan_cmd_wifi_direct_mode(IN pmlan_private pmpriv,
                          IN HostCmd_DS_COMMAND * cmd,
                          IN t_u16 cmd_action, IN t_void * pdata_buf)
{
    HostCmd_DS_WIFI_DIRECT_MODE *wfd_mode = &cmd->params.wifi_direct_mode;
    t_u16 mode = *((t_u16 *) pdata_buf);
    ENTER();
    cmd->size = wlan_cpu_to_le16((sizeof(HostCmd_DS_WIFI_DIRECT_MODE))
                                 + S_DS_GEN);
    cmd->command = wlan_cpu_to_le16(HOST_CMD_WIFI_DIRECT_MODE_CONFIG);
    wfd_mode->action = wlan_cpu_to_le16(cmd_action);
    if (cmd_action == HostCmd_ACT_GEN_SET)
        wfd_mode->mode = wlan_cpu_to_le16(mode);

    LEAVE();
    return MLAN_STATUS_SUCCESS;
}

/**
 *  @brief This function handles the command response of wifi direct mode
 *
 *  @param pmpriv       A pointer to mlan_private structure
 *  @param resp         A pointer to HostCmd_DS_COMMAND
 *  @param pioctl_buf   A pointer to mlan_ioctl_req structure
 *
 *  @return             MLAN_STATUS_SUCCESS
 */
mlan_status
wlan_ret_wifi_direct_mode(IN pmlan_private pmpriv,
                          IN HostCmd_DS_COMMAND * resp,
                          IN mlan_ioctl_req * pioctl_buf)
{
    HostCmd_DS_WIFI_DIRECT_MODE *wfd_mode = &resp->params.wifi_direct_mode;
    mlan_ds_bss *bss = MNULL;

    ENTER();
    if (pioctl_buf) {
        bss = (mlan_ds_bss *) pioctl_buf->pbuf;
        bss->param.wfd_mode = wlan_le16_to_cpu(wfd_mode->mode);
        pioctl_buf->data_read_written = sizeof(mlan_ds_bss);
    }
    LEAVE();
    return MLAN_STATUS_SUCCESS;
}
#endif
#endif /* CONFIG_MLAN_WMSDK */
