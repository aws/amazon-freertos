/** @file mlan_11n_rxreorder.c
 *
 *  @brief  This file provides handling of RxReordering in wlan
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
    11/10/2008: initial version
********************************************************/
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
 *  @brief This function will dispatch amsdu packet and
 *  		forward it to kernel/upper layer
 *
 *  @param priv    	A pointer to mlan_private
 *  @param pmbuf    A pointer to the received buffer
 *
 *  @return 	   	MLAN_STATUS_SUCCESS or MLAN_STATUS_FAILURE
 */
static mlan_status
wlan_11n_dispatch_amsdu_pkt(mlan_private * priv, pmlan_buffer pmbuf)
{
    RxPD *prx_pd;
    prx_pd = (RxPD *) (pmbuf->pbuf + pmbuf->data_offset);

    ENTER();
    if (prx_pd->rx_pkt_type == PKT_TYPE_AMSDU) {
        pmbuf->data_len = prx_pd->rx_pkt_length;
        pmbuf->data_offset += prx_pd->rx_pkt_offset;
        wlan_11n_deaggregate_pkt(priv, pmbuf);
        LEAVE();
        return MLAN_STATUS_SUCCESS;
    }
    LEAVE();
    return MLAN_STATUS_FAILURE;
}

/**
 *  @brief This function will process the rx packet and
 *  		forward it to kernel/upper layer
 *
 *  @param priv    	A pointer to mlan_private
 *  @param payload  A pointer to rx packet payload
 *
 *  @return 	   	MLAN_STATUS_SUCCESS or MLAN_STATUS_FAILURE
 */
static mlan_status
wlan_11n_dispatch_pkt(t_void * priv, t_void * payload)
{
    mlan_status ret = MLAN_STATUS_SUCCESS;
#ifdef STA_SUPPORT
    pmlan_adapter pmadapter = ((pmlan_private) priv)->adapter;
#endif
    ENTER();
    if (payload == (t_void *) RX_PKT_DROPPED_IN_FW) {
        LEAVE();
        return ret;
    }
#ifndef CONFIG_MLAN_WMSDK
#ifdef UAP_SUPPORT
    if (GET_BSS_ROLE((mlan_private *) priv) == MLAN_BSS_ROLE_UAP) {
        if (MLAN_STATUS_SUCCESS ==
            wlan_11n_dispatch_amsdu_pkt((mlan_private *) priv,
                                        (pmlan_buffer) payload)) {
            LEAVE();
            return ret;
        }
        ret = wlan_process_uap_rx_packet(priv, (pmlan_buffer) payload);
        LEAVE();
        return ret;
    }
#endif /* UAP_SUPPORT */
#endif /* CONFIG_MLAN_WMSDK */

#ifdef STA_SUPPORT
    if (MLAN_STATUS_SUCCESS ==
        wlan_11n_dispatch_amsdu_pkt((mlan_private *) priv,
                                    (pmlan_buffer) payload)) {
        LEAVE();
        return ret;
    }
    ret = wlan_process_rx_packet(pmadapter, (pmlan_buffer) payload);
#endif /* STA_SUPPORT */
    LEAVE();
    return ret;
}

/**
 *  @brief This function restarts the reordering timeout timer
 *
 *  @param pmadapter    	A pointer to mlan_adapter
 *  @param rx_reor_tbl_ptr  A pointer to structure RxReorderTbl
 *
 *  @return                 N/A
 */
static void
mlan_11n_rxreorder_timer_restart(pmlan_adapter pmadapter,
                                 RxReorderTbl * rx_reor_tbl_ptr)
{
    ENTER();
    if (rx_reor_tbl_ptr->timer_context.timer_is_set)
        pmadapter->callbacks.moal_stop_timer(pmadapter->pmoal_handle,
                                             rx_reor_tbl_ptr->timer_context.
                                             timer);

    pmadapter->callbacks.moal_start_timer(pmadapter->pmoal_handle,
                                          rx_reor_tbl_ptr->timer_context.timer,
                                          MFALSE,
                                          (rx_reor_tbl_ptr->win_size
                                           * MIN_FLUSH_TIMER_MS));

    rx_reor_tbl_ptr->timer_context.timer_is_set = MTRUE;
    LEAVE();
}

/**
 *  @brief This function dispatches all the packets in the buffer.
 *  		There could be holes in the buffer.
 *
 *  @param priv    	        A pointer to mlan_private
 *  @param rx_reor_tbl_ptr  A pointer to structure RxReorderTbl
 *  @param start_win        Start window
 *
 *  @return 	   	        MLAN_STATUS_SUCCESS
 */
static mlan_status
wlan_11n_dispatch_pkt_until_start_win(t_void * priv,
                                      RxReorderTbl * rx_reor_tbl_ptr,
                                      int start_win)
{
    int no_pkt_to_send, i, xchg;
    mlan_status ret = MLAN_STATUS_SUCCESS;
    void *rx_tmp_ptr = MNULL;
    mlan_private *pmpriv = (mlan_private *) priv;

    ENTER();

    no_pkt_to_send = (start_win > rx_reor_tbl_ptr->start_win) ?
        MIN((start_win - rx_reor_tbl_ptr->start_win),
            rx_reor_tbl_ptr->win_size) : rx_reor_tbl_ptr->win_size;

    for (i = 0; i < no_pkt_to_send; ++i) {
        pmpriv->adapter->callbacks.moal_spin_lock(pmpriv->adapter->pmoal_handle,
                                                  pmpriv->rx_pkt_lock);
        rx_tmp_ptr = MNULL;
        if (rx_reor_tbl_ptr->rx_reorder_ptr[i]) {
            rx_tmp_ptr = rx_reor_tbl_ptr->rx_reorder_ptr[i];
            rx_reor_tbl_ptr->rx_reorder_ptr[i] = MNULL;
        }
        pmpriv->adapter->callbacks.moal_spin_unlock(pmpriv->adapter->
                                                    pmoal_handle,
                                                    pmpriv->rx_pkt_lock);
        if (rx_tmp_ptr)
            wlan_11n_dispatch_pkt(priv, rx_tmp_ptr);
    }

    pmpriv->adapter->callbacks.moal_spin_lock(pmpriv->adapter->pmoal_handle,
                                              pmpriv->rx_pkt_lock);
    /*
     * We don't have a circular buffer, hence use rotation to simulate
     * circular buffer
     */
    xchg = rx_reor_tbl_ptr->win_size - no_pkt_to_send;
    for (i = 0; i < xchg; ++i) {
        rx_reor_tbl_ptr->rx_reorder_ptr[i] =
            rx_reor_tbl_ptr->rx_reorder_ptr[no_pkt_to_send + i];
        rx_reor_tbl_ptr->rx_reorder_ptr[no_pkt_to_send + i] = MNULL;
    }

    rx_reor_tbl_ptr->start_win = start_win;
    pmpriv->adapter->callbacks.moal_spin_unlock(pmpriv->adapter->pmoal_handle,
                                                pmpriv->rx_pkt_lock);

    LEAVE();
    return ret;
}

/**
 *  @brief This function will display the rxReorder table
 *
 *  @param pmadapter          A pointer to mlan_adapter structure
 *  @param rx_reor_tbl_ptr    A pointer to structure RxReorderTbl
 *
 *  @return 	   	N/A
 */
static t_void
wlan_11n_display_tbl_ptr(pmlan_adapter pmadapter,
                         RxReorderTbl * rx_reor_tbl_ptr)
{
    ENTER();

    DBG_HEXDUMP(MDAT_D, "Reorder ptr", rx_reor_tbl_ptr->rx_reorder_ptr,
                sizeof(t_void *) * rx_reor_tbl_ptr->win_size);

    LEAVE();
}

/**
 *  @brief This function will dispatch all packets sequentially
 *  		from start_win until a hole is found and adjust the
 *  		start_win appropriately
 *
 *  @param priv    	        A pointer to mlan_private
 *  @param rx_reor_tbl_ptr  A pointer to structure RxReorderTbl
 *
 *  @return 	   	        MLAN_STATUS_SUCCESS
 */
static mlan_status
wlan_11n_scan_and_dispatch(t_void * priv, RxReorderTbl * rx_reor_tbl_ptr)
{
    int i, j, xchg;
    mlan_status ret = MLAN_STATUS_SUCCESS;
    void *rx_tmp_ptr = MNULL;
    mlan_private *pmpriv = (mlan_private *) priv;

    ENTER();

    for (i = 0; i < rx_reor_tbl_ptr->win_size; ++i) {
        pmpriv->adapter->callbacks.moal_spin_lock(pmpriv->adapter->pmoal_handle,
                                                  pmpriv->rx_pkt_lock);
        if (!rx_reor_tbl_ptr->rx_reorder_ptr[i]) {
            pmpriv->adapter->callbacks.moal_spin_unlock(pmpriv->adapter->
                                                        pmoal_handle,
                                                        pmpriv->rx_pkt_lock);
            break;
        }
        rx_tmp_ptr = rx_reor_tbl_ptr->rx_reorder_ptr[i];
        rx_reor_tbl_ptr->rx_reorder_ptr[i] = MNULL;
        pmpriv->adapter->callbacks.moal_spin_unlock(pmpriv->adapter->
                                                    pmoal_handle,
                                                    pmpriv->rx_pkt_lock);
        wlan_11n_dispatch_pkt(priv, rx_tmp_ptr);
    }

    pmpriv->adapter->callbacks.moal_spin_lock(pmpriv->adapter->pmoal_handle,
                                              pmpriv->rx_pkt_lock);
    /*
     * We don't have a circular buffer, hence use rotation to simulate
     * circular buffer
     */
    if (i > 0) {
        xchg = rx_reor_tbl_ptr->win_size - i;
        for (j = 0; j < xchg; ++j) {
            rx_reor_tbl_ptr->rx_reorder_ptr[j] =
                rx_reor_tbl_ptr->rx_reorder_ptr[i + j];
            rx_reor_tbl_ptr->rx_reorder_ptr[i + j] = MNULL;
        }
    }

    rx_reor_tbl_ptr->start_win = (rx_reor_tbl_ptr->start_win + i)
        & (MAX_TID_VALUE - 1);

    pmpriv->adapter->callbacks.moal_spin_unlock(pmpriv->adapter->pmoal_handle,
                                                pmpriv->rx_pkt_lock);
    LEAVE();
    return ret;
}

/**
 *  @brief This function delete rxreorder table's entry
 *         	and free the memory
 *
 *  @param priv    	        A pointer to mlan_private
 *  @param rx_reor_tbl_ptr  A pointer to structure RxReorderTbl
 *
 *  @return 	   	        N/A
 */
static t_void
wlan_11n_delete_rxreorder_tbl_entry(mlan_private * priv,
                                    RxReorderTbl * rx_reor_tbl_ptr)
{
    pmlan_adapter pmadapter = priv->adapter;

    ENTER();

    if (!rx_reor_tbl_ptr) {
        LEAVE();
        return;
    }

    wlan_11n_dispatch_pkt_until_start_win(priv, rx_reor_tbl_ptr,
                                          (rx_reor_tbl_ptr->start_win +
                                           rx_reor_tbl_ptr->win_size)
                                          & (MAX_TID_VALUE - 1));

    if (rx_reor_tbl_ptr->timer_context.timer) {
        if (rx_reor_tbl_ptr->timer_context.timer_is_set)
            priv->adapter->callbacks.moal_stop_timer(pmadapter->pmoal_handle,
                                                     rx_reor_tbl_ptr->timer_context.
                                                     timer);
        priv->adapter->callbacks.moal_free_timer(pmadapter->pmoal_handle,
                                                 &rx_reor_tbl_ptr->timer_context.
                                                 timer);
    }

    PRINTM(MDAT_D, "Delete rx_reor_tbl_ptr: %p\n", rx_reor_tbl_ptr);
    util_unlink_list(pmadapter->pmoal_handle,
                     &priv->rx_reorder_tbl_ptr,
                     (pmlan_linked_list) rx_reor_tbl_ptr,
                     pmadapter->callbacks.moal_spin_lock,
                     pmadapter->callbacks.moal_spin_unlock);

    pmadapter->callbacks.moal_mfree(pmadapter->pmoal_handle,
                                    (t_u8 *) rx_reor_tbl_ptr->rx_reorder_ptr);
    pmadapter->callbacks.moal_mfree(pmadapter->pmoal_handle,
                                    (t_u8 *) rx_reor_tbl_ptr);

    LEAVE();
}

/**
 *  @brief This function returns the last used sequence number
 *
 *  @param rx_reorder_tbl_ptr   A pointer to structure RxReorderTbl
 *
 *  @return 	   	            Last used sequence number
 */
static int
wlan_11n_find_last_seqnum(RxReorderTbl * rx_reorder_tbl_ptr)
{
    int i;

    ENTER();
    for (i = (rx_reorder_tbl_ptr->win_size - 1); i >= 0; --i) {
        if (rx_reorder_tbl_ptr->rx_reorder_ptr[i]) {
            LEAVE();
            return i;
        }
    }
    LEAVE();
    return -1;
}

/**
 *  @brief This function flushes all data
 *
 *  @param context      Reorder context pointer
 *
 *  @return 	   	    N/A
 */
static t_void
wlan_flush_data(t_void * tmr_handle)
{
	/* Note: Giving tmr_handle as a parameter in callback is a feature
	   of FreeRTOS. Hence, we have to change the default mlan code here
	   to get the actual context expected by it */
	reorder_tmr_cnxt_t *reorder_cnxt = (reorder_tmr_cnxt_t *)
		os_timer_get_context((os_timer_t *)&tmr_handle);
	int startWin;

    ENTER();
    reorder_cnxt->timer_is_set = MFALSE;
    wlan_11n_display_tbl_ptr(reorder_cnxt->priv->adapter, reorder_cnxt->ptr);

    if ((startWin = wlan_11n_find_last_seqnum(reorder_cnxt->ptr)) >= 0) {
        PRINTM(MINFO, "Flush data %d\n", startWin);
        wlan_11n_dispatch_pkt_until_start_win(reorder_cnxt->priv,
                                              reorder_cnxt->ptr,
                                              ((reorder_cnxt->ptr->start_win +
                                                startWin + 1) & (MAX_TID_VALUE -
                                                                 1)));
    }

    wlan_11n_display_tbl_ptr(reorder_cnxt->priv->adapter, reorder_cnxt->ptr);
    LEAVE();
}

/**
 *  @brief This function will create a entry in rx reordering table for the
 *  		given ta/tid and will initialize it with seq_num, win_size
 *
 *  @param priv     A pointer to mlan_private
 *  @param ta       ta to find in reordering table
 *  @param tid	    tid to find in reordering table
 *  @param win_size win_size for the give ta/tid pair.
 *  @param seq_num  Starting sequence number for current entry.
 *
 *  @return 	    N/A
 */
static t_void
wlan_11n_create_rxreorder_tbl(mlan_private * priv, t_u8 * ta, int tid,
                              int win_size, int seq_num)
{
    int i;
    pmlan_adapter pmadapter = priv->adapter;
    RxReorderTbl *rx_reor_tbl_ptr, *new_node;
    /* sta_node *sta_ptr = MNULL; */
    t_u16 last_seq = 0;

    ENTER();

#ifdef DEBUG_11N_REORDERING
    wmprintf("### Creating reorder table for TID: %d\n\r", tid);
#endif /* DEBUG_11N_REORDERING */

    /*
     * If we get a TID, ta pair which is already present dispatch all the
     * the packets and move the window size until the ssn
     */
    if ((rx_reor_tbl_ptr = wlan_11n_get_rxreorder_tbl(priv, tid, ta))) {
        wlan_11n_dispatch_pkt_until_start_win(priv, rx_reor_tbl_ptr, seq_num);
    } else {
        PRINTM(MDAT_D, "%s: seq_num %d, tid %d, ta %02x:%02x:%02x:%02x:"
               "%02x:%02x, win_size %d\n", __FUNCTION__,
               seq_num, tid, ta[0], ta[1], ta[2], ta[3],
               ta[4], ta[5], win_size);
        if (pmadapter->callbacks.
            moal_malloc(pmadapter->pmoal_handle, sizeof(RxReorderTbl),
                        MLAN_MEM_DEF, (t_u8 **) & new_node)) {
            PRINTM(MERROR, "Rx reorder memory allocation failed\n");
            LEAVE();
            return;
        }

        util_init_list((pmlan_linked_list) new_node);
        new_node->tid = tid;
        memcpy(pmadapter, new_node->ta, ta, MLAN_MAC_ADDR_LENGTH);
        new_node->start_win = seq_num;
	new_node->pkt_count = 0;
        if (queuing_ra_based(priv)) {
#ifndef CONFIG_MLAN_WMSDK /* fixme: This part seems something related to UAP. Disable for now. */
            // TODO for adhoc
            if (GET_BSS_ROLE(priv) == MLAN_BSS_ROLE_UAP) {
                if ((sta_ptr = wlan_get_station_entry(priv, ta)))
                    last_seq = sta_ptr->rx_seq[tid];
            }
            PRINTM(MINFO, "UAP/ADHOC:last_seq=%d start_win=%d\n", last_seq,
                   new_node->start_win);
#endif /* CONFIG_MLAN_WMSDK */
        } else {
            last_seq = priv->rx_seq[tid];
        }
	new_node->last_seq = last_seq;
        new_node->win_size = win_size;
	new_node->force_no_drop = MFALSE;
	new_node->check_start_win = MTRUE;

        if (pmadapter->callbacks.
            moal_malloc(pmadapter->pmoal_handle, sizeof(t_void *) * win_size,
                        MLAN_MEM_DEF, (t_u8 **) & new_node->rx_reorder_ptr)) {
            PRINTM(MERROR, "Rx reorder table memory allocation" "failed\n");
            pmadapter->callbacks.moal_mfree(pmadapter->pmoal_handle,
                                            (t_u8 *) new_node);
            LEAVE();
            return;
        }

	PRINTM(MDAT_D, "Create ReorderPtr: %p start_win=%d last_seq=%d\n",
		new_node, new_node->start_win, last_seq);
        new_node->timer_context.ptr = new_node;
        new_node->timer_context.priv = priv;
        new_node->timer_context.timer_is_set = MFALSE;
        new_node->ba_status = BA_STREAM_SETUP_INPROGRESS;

        pmadapter->callbacks.moal_init_timer(pmadapter->pmoal_handle,
                                             &new_node->timer_context.timer,
                                             wlan_flush_data,
                                             &new_node->timer_context);

        for (i = 0; i < win_size; ++i)
            new_node->rx_reorder_ptr[i] = MNULL;

        util_enqueue_list_tail(pmadapter->pmoal_handle,
                               &priv->rx_reorder_tbl_ptr,
                               (pmlan_linked_list) new_node,
                               pmadapter->callbacks.moal_spin_lock,
                               pmadapter->callbacks.moal_spin_unlock);
    }

    LEAVE();
}

/********************************************************
    Global Functions
********************************************************/

/**
 *  @brief This function will return the pointer to a entry in rx reordering
 *  		table which matches the give TA/TID pair
 *
 *  @param priv    A pointer to mlan_private
 *  @param ta      ta to find in reordering table
 *  @param tid	   tid to find in reordering table
 *
 *  @return	   A pointer to structure RxReorderTbl
 */
RxReorderTbl *
wlan_11n_get_rxreorder_tbl(mlan_private * priv, int tid, t_u8 * ta)
{
    RxReorderTbl *rx_reor_tbl_ptr;

    ENTER();

    if (!
        (rx_reor_tbl_ptr =
         (RxReorderTbl *) util_peek_list(priv->adapter->pmoal_handle,
                                         &priv->rx_reorder_tbl_ptr,
                                         priv->adapter->callbacks.
                                         moal_spin_lock,
                                         priv->adapter->callbacks.
                                         moal_spin_unlock))) {
#ifdef DEBUG_11N_REORDERING
	    wmprintf("### Not found even one entry in  RX reorder table\n\r");
#endif /* DEBUG_11N_REORDERING */
        LEAVE();
        return MNULL;
    }

    while (rx_reor_tbl_ptr != (RxReorderTbl *) & priv->rx_reorder_tbl_ptr) {
        if ((!memcmp
             (priv->adapter, rx_reor_tbl_ptr->ta, ta, MLAN_MAC_ADDR_LENGTH)) &&
            (rx_reor_tbl_ptr->tid == tid)) {
            LEAVE();
            return rx_reor_tbl_ptr;
        }

        rx_reor_tbl_ptr = rx_reor_tbl_ptr->pnext;
    }

#ifdef DEBUG_11N_REORDERING
    wmprintf("### Failed to find RX reorder table for TID: %d R: %p\n\r",
	     tid, __builtin_return_address(0));
#endif /* DEBUG_11N_REORDERING */
    LEAVE();
    return MNULL;
}

/**
  *  @brief This function prepares command for adding a block ack
  *  		request.
  *
  *  @param priv        A pointer to mlan_private structure
  *  @param cmd         A pointer to HostCmd_DS_COMMAND structure
  *  @param pdata_buf   A pointer to data buffer
  *
  *  @return            MLAN_STATUS_SUCCESS
  */
mlan_status
wlan_cmd_11n_addba_req(mlan_private * priv,
                       HostCmd_DS_COMMAND * cmd, t_void * pdata_buf)
{
    HostCmd_DS_11N_ADDBA_REQ *padd_ba_req = (HostCmd_DS_11N_ADDBA_REQ *)
        & cmd->params.add_ba_req;
    ENTER();

    cmd->command = wlan_cpu_to_le16(HostCmd_CMD_11N_ADDBA_REQ);
    cmd->size = wlan_cpu_to_le16(sizeof(HostCmd_DS_11N_ADDBA_REQ) + S_DS_GEN);

    memcpy(priv->adapter, padd_ba_req, pdata_buf,
           sizeof(HostCmd_DS_11N_ADDBA_REQ));
    padd_ba_req->block_ack_param_set =
        wlan_cpu_to_le16(padd_ba_req->block_ack_param_set);
    padd_ba_req->block_ack_tmo = wlan_cpu_to_le16(padd_ba_req->block_ack_tmo);
    padd_ba_req->ssn = wlan_cpu_to_le16(padd_ba_req->ssn);
    padd_ba_req->add_req_result = 0;

    LEAVE();
    return MLAN_STATUS_SUCCESS;
}

/**
  *  @brief This function prepares command for adding a block ack
  *  		response.
  *
  *  @param priv        A pointer to mlan_private structure
  *  @param cmd         A pointer to HostCmd_DS_COMMAND structure
  *  @param pdata_buf   A pointer to data buffer
  *
  *  @return            MLAN_STATUS_SUCCESS
  */
mlan_status
wlan_cmd_11n_addba_rspgen(mlan_private * priv,
                          HostCmd_DS_COMMAND * cmd, void *pdata_buf)
{
    HostCmd_DS_11N_ADDBA_RSP *padd_ba_rsp = (HostCmd_DS_11N_ADDBA_RSP *)
        & cmd->params.add_ba_rsp;
    HostCmd_DS_11N_ADDBA_REQ *pevt_addba_req =
        (HostCmd_DS_11N_ADDBA_REQ *) pdata_buf;
    t_u8 tid = 0;
    int win_size = 0;

    ENTER();

    pevt_addba_req->block_ack_param_set =
        wlan_le16_to_cpu(pevt_addba_req->block_ack_param_set);
    pevt_addba_req->block_ack_tmo =
        wlan_le16_to_cpu(pevt_addba_req->block_ack_tmo);
    pevt_addba_req->ssn = wlan_le16_to_cpu(pevt_addba_req->ssn);

    cmd->command = wlan_cpu_to_le16(HostCmd_CMD_11N_ADDBA_RSP);
    cmd->size = wlan_cpu_to_le16(sizeof(HostCmd_DS_11N_ADDBA_RSP) + S_DS_GEN);

    memcpy(NULL, padd_ba_rsp->peer_mac_addr, pevt_addba_req->peer_mac_addr,
	   MLAN_MAC_ADDR_LENGTH);
    padd_ba_rsp->dialog_token = pevt_addba_req->dialog_token;
    padd_ba_rsp->block_ack_tmo =
        wlan_cpu_to_le16(pevt_addba_req->block_ack_tmo);
    padd_ba_rsp->ssn = wlan_cpu_to_le16(pevt_addba_req->ssn);
    padd_ba_rsp->add_rsp_result = 0;

    padd_ba_rsp->block_ack_param_set = pevt_addba_req->block_ack_param_set;
    tid = (padd_ba_rsp->block_ack_param_set & BLOCKACKPARAM_TID_MASK)
        >> BLOCKACKPARAM_TID_POS;
    if (priv->addba_reject[tid]
#ifdef STA_SUPPORT
#ifndef CONFIG_MLAN_WMSDK  /* fixme: enable this if wps is merged into mlan */
	|| ((GET_BSS_ROLE(priv) == MLAN_BSS_ROLE_STA)
	&& priv->wps.session_enable)
#endif /* CONFIG_MLAN_WMSDK */
#endif
	/* wmsdk: we are not using UAP with mlan right now */
#ifndef CONFIG_MLAN_WMSDK
#ifdef UAP_SUPPORT
        || ((GET_BSS_ROLE(priv) == MLAN_BSS_ROLE_UAP)
            && (priv->adapter->pending_bridge_pkts > RX_LOW_THRESHOLD))
#endif
#endif /* CONFIG_MLAN_WMSDK */
        )
        padd_ba_rsp->status_code = wlan_cpu_to_le16(ADDBA_RSP_STATUS_DECLINED);
    else
        padd_ba_rsp->status_code = wlan_cpu_to_le16(ADDBA_RSP_STATUS_ACCEPT);
    padd_ba_rsp->block_ack_param_set &= ~BLOCKACKPARAM_WINSIZE_MASK;
    if (!priv->add_ba_param.rx_amsdu ||
        (priv->aggr_prio_tbl[tid].amsdu == BA_STREAM_NOT_ALLOWED))
        /* We do not support AMSDU inside AMPDU, hence reset the bit */
        padd_ba_rsp->block_ack_param_set &= ~BLOCKACKPARAM_AMSDU_SUPP_MASK;

    padd_ba_rsp->block_ack_param_set |= (priv->add_ba_param.rx_win_size <<
                                         BLOCKACKPARAM_WINSIZE_POS);
    win_size = (padd_ba_rsp->block_ack_param_set & BLOCKACKPARAM_WINSIZE_MASK)
        >> BLOCKACKPARAM_WINSIZE_POS;
    if (win_size == 0)
        padd_ba_rsp->status_code = wlan_cpu_to_le16(ADDBA_RSP_STATUS_DECLINED);

    padd_ba_rsp->block_ack_param_set =
        wlan_cpu_to_le16(padd_ba_rsp->block_ack_param_set);

#ifdef CONFIG_STA_AMPDU_RX
    if (!sta_ampdu_rx_enable) {
	padd_ba_rsp->status_code = wlan_cpu_to_le16(ADDBA_RSP_STATUS_DECLINED);
	padd_ba_rsp->add_rsp_result = BA_RESULT_FAILURE;
    }
#else
    padd_ba_rsp->status_code = wlan_cpu_to_le16(ADDBA_RSP_STATUS_DECLINED);
    padd_ba_rsp->add_rsp_result = BA_RESULT_FAILURE;
#endif

    if (padd_ba_rsp->status_code == wlan_cpu_to_le16(ADDBA_RSP_STATUS_ACCEPT))
        wlan_11n_create_rxreorder_tbl(priv, pevt_addba_req->peer_mac_addr, tid,
                                      win_size, pevt_addba_req->ssn);
    LEAVE();
    return MLAN_STATUS_SUCCESS;
}

mlan_status
wlan_cmd_11n_uap_addba_rspgen(mlan_private * priv,
                          HostCmd_DS_COMMAND * cmd, void *pdata_buf)
{
    HostCmd_DS_11N_ADDBA_RSP *padd_ba_rsp = (HostCmd_DS_11N_ADDBA_RSP *)
        & cmd->params.add_ba_rsp;
    HostCmd_DS_11N_ADDBA_REQ *pevt_addba_req =
        (HostCmd_DS_11N_ADDBA_REQ *) pdata_buf;

    ENTER();

    pevt_addba_req->block_ack_param_set =
        wlan_le16_to_cpu(pevt_addba_req->block_ack_param_set);
    pevt_addba_req->block_ack_tmo =
        wlan_le16_to_cpu(pevt_addba_req->block_ack_tmo);
    pevt_addba_req->ssn = wlan_le16_to_cpu(pevt_addba_req->ssn);

    cmd->command = wlan_cpu_to_le16(HostCmd_CMD_11N_ADDBA_RSP);
    cmd->size = wlan_cpu_to_le16(sizeof(HostCmd_DS_11N_ADDBA_RSP) + S_DS_GEN);

    memcpy(NULL, padd_ba_rsp->peer_mac_addr, pevt_addba_req->peer_mac_addr,
	   MLAN_MAC_ADDR_LENGTH);
    padd_ba_rsp->dialog_token = pevt_addba_req->dialog_token;
    padd_ba_rsp->block_ack_tmo =
        wlan_cpu_to_le16(pevt_addba_req->block_ack_tmo);
    padd_ba_rsp->ssn = wlan_cpu_to_le16(pevt_addba_req->ssn);

    padd_ba_rsp->block_ack_param_set = pevt_addba_req->block_ack_param_set;

    /* We do not support AMSDU inside AMPDU, hence reset the bit */
    padd_ba_rsp->block_ack_param_set &= ~BLOCKACKPARAM_AMSDU_SUPP_MASK;

    padd_ba_rsp->status_code = wlan_cpu_to_le16(ADDBA_RSP_STATUS_ACCEPT);

#ifndef CONFIG_UAP_AMPDU_RX
    padd_ba_rsp->status_code = wlan_cpu_to_le16(ADDBA_RSP_STATUS_DECLINED);
    padd_ba_rsp->add_rsp_result = BA_RESULT_FAILURE;
#endif

    padd_ba_rsp->block_ack_param_set =
        wlan_cpu_to_le16(padd_ba_rsp->block_ack_param_set);

    LEAVE();
    return MLAN_STATUS_SUCCESS;
}

/**
  *  @brief This function prepares command for deleting a block ack
  *  		request.
  *
  *  @param priv       A pointer to mlan_private structure
  *  @param cmd        A pointer to HostCmd_DS_COMMAND structure
  *  @param pdata_buf  A pointer to data buffer
  *
  *  @return           MLAN_STATUS_SUCCESS
  */
mlan_status
wlan_cmd_11n_delba(mlan_private * priv,
                   HostCmd_DS_COMMAND * cmd, void *pdata_buf)
{
    HostCmd_DS_11N_DELBA *pdel_ba = (HostCmd_DS_11N_DELBA *)
        & cmd->params.del_ba;

    ENTER();

    cmd->command = wlan_cpu_to_le16(HostCmd_CMD_11N_DELBA);
    cmd->size = wlan_cpu_to_le16(sizeof(HostCmd_DS_11N_DELBA) + S_DS_GEN);

    memcpy(priv->adapter, pdel_ba, pdata_buf, sizeof(HostCmd_DS_11N_DELBA));
    pdel_ba->del_ba_param_set = wlan_cpu_to_le16(pdel_ba->del_ba_param_set);
    pdel_ba->reason_code = wlan_cpu_to_le16(pdel_ba->reason_code);

    LEAVE();
    return MLAN_STATUS_SUCCESS;
}

/**
 *  @brief This function will identify if RxReodering is needed for the packet
 *  		and will do the reordering if required before sending it to kernel
 *
 *  @param priv     A pointer to mlan_private
 *  @param seq_num  Seqence number of the current packet
 *  @param tid	    Tid of the current packet
 *  @param ta	    Transmiter address of the current packet
 *  @param pkt_type Packetype for the current packet (to identify if its a BAR)
 *  @param payload  Pointer to the payload
 *
 *  @return 	    MLAN_STATUS_SUCCESS or MLAN_STATUS_FAILURE
 */
mlan_status
mlan_11n_rxreorder_pkt(void *priv, t_u16 seq_num, t_u16 tid,
                       t_u8 * ta, t_u8 pkt_type, void *payload)
{
    RxReorderTbl *rx_reor_tbl_ptr;
    int prev_start_win, start_win, end_win, win_size;
    mlan_status ret = MLAN_STATUS_SUCCESS;
    pmlan_adapter pmadapter = ((mlan_private *) priv)->adapter;

    ENTER();

    if (!
        (rx_reor_tbl_ptr =
         wlan_11n_get_rxreorder_tbl((mlan_private *) priv, tid, ta))) {
        if (pkt_type != PKT_TYPE_BAR)
            wlan_11n_dispatch_pkt(priv, payload);

        LEAVE();
        return ret;

    } else {
        if ((pkt_type == PKT_TYPE_AMSDU) && !rx_reor_tbl_ptr->amsdu) {
            wlan_11n_dispatch_pkt(priv, payload);
            LEAVE();
            return ret;
        }
        if (pkt_type == PKT_TYPE_BAR)
            PRINTM(MDAT_D, "BAR ");
        if (pkt_type == PKT_TYPE_AMSDU)
            PRINTM(MDAT_D, "AMSDU ");

	if (rx_reor_tbl_ptr->check_start_win) {
		if (seq_num == rx_reor_tbl_ptr->start_win)
			rx_reor_tbl_ptr->check_start_win = MFALSE;
		else {
			rx_reor_tbl_ptr->pkt_count++;
			if (rx_reor_tbl_ptr->pkt_count < (rx_reor_tbl_ptr->win_size / 2)) {
				if (rx_reor_tbl_ptr->last_seq == seq_num) {
					/** drop duplicate packet */
					ret = MLAN_STATUS_FAILURE;
				} else {
					/** forward the packet to kernel */
					rx_reor_tbl_ptr->last_seq = seq_num;
					if (pkt_type != PKT_TYPE_BAR)
						wlan_11n_dispatch_pkt(priv, payload);
				}
				LEAVE();
				return ret;
			}

			rx_reor_tbl_ptr->check_start_win = MFALSE;
			if ((seq_num != rx_reor_tbl_ptr->start_win) &&
					(rx_reor_tbl_ptr->last_seq != DEFAULT_SEQ_NUM)) {
				end_win =
					(rx_reor_tbl_ptr->start_win + rx_reor_tbl_ptr->win_size -
					 1) & (MAX_TID_VALUE - 1);
				if (((end_win > rx_reor_tbl_ptr->start_win)
							&& (rx_reor_tbl_ptr->last_seq >=
								rx_reor_tbl_ptr->start_win)
							&& (rx_reor_tbl_ptr->last_seq < end_win)) ||
						((end_win < rx_reor_tbl_ptr->start_win) &&
						 ((rx_reor_tbl_ptr->last_seq >= rx_reor_tbl_ptr->start_win)
						  || (rx_reor_tbl_ptr->last_seq < end_win)))) {
					PRINTM(MDAT_D,
							"Update start_win: last_seq=%d, start_win=%d seq_num=%d\n",
							rx_reor_tbl_ptr->last_seq,
							rx_reor_tbl_ptr->start_win, seq_num);
					rx_reor_tbl_ptr->start_win = rx_reor_tbl_ptr->last_seq + 1;
				} else if ((seq_num < rx_reor_tbl_ptr->start_win) &&
						(seq_num > rx_reor_tbl_ptr->last_seq)) {
					PRINTM(MDAT_D,
							"Update start_win: last_seq=%d, start_win=%d seq_num=%d\n",
							rx_reor_tbl_ptr->last_seq,
							rx_reor_tbl_ptr->start_win, seq_num);
					rx_reor_tbl_ptr->start_win = rx_reor_tbl_ptr->last_seq + 1;
				}
			}
		}
	}

        prev_start_win = start_win = rx_reor_tbl_ptr->start_win;
        win_size = rx_reor_tbl_ptr->win_size;
        end_win = ((start_win + win_size) - 1) & (MAX_TID_VALUE - 1);

        PRINTM(MDAT_D,
               "TID %d, TA %02x:%02x:%02x:%02x:%02x:%02x\n",
               tid, ta[0], ta[1], ta[2], ta[3], ta[4], ta[5]);
        PRINTM(MDAT_D,
               "1:seq_num %d start_win %d win_size %d end_win %d\n",
               seq_num, start_win, win_size, end_win);
        /*
         * If seq_num is less then starting win then ignore and drop
         * the packet
         */
	if (rx_reor_tbl_ptr->force_no_drop) {
            PRINTM(MDAT_D, "Force no drop packet after HS_ACTIVED\n");
            rx_reor_tbl_ptr->force_no_drop = MFALSE;
        } else {
            if ((start_win + TWOPOW11) > (MAX_TID_VALUE - 1)) { /* Wrap */
                if (seq_num >= ((start_win + (TWOPOW11)) &
                                (MAX_TID_VALUE - 1)) && (seq_num < start_win)) {
                    ret = MLAN_STATUS_FAILURE;
                    goto done;
                }
            } else if ((seq_num < start_win) ||
                       (seq_num > (start_win + (TWOPOW11)))) {
                ret = MLAN_STATUS_FAILURE;
                goto done;
            }
        }

        /*
         * If this packet is a BAR we adjust seq_num as
         * WinStart = seq_num
         */
        if (pkt_type == PKT_TYPE_BAR)
            seq_num = ((seq_num + win_size) - 1) & (MAX_TID_VALUE - 1);

        PRINTM(MDAT_D,
               "2:seq_num %d start_win %d win_size %d end_win %d\n",
               seq_num, start_win, win_size, end_win);

        if (((end_win < start_win) &&
	     (seq_num < start_win) && (seq_num > end_win))
            || ((end_win > start_win) &&
                ((seq_num > end_win) || (seq_num < start_win)))) {

            end_win = seq_num;
            if (((seq_num - win_size) + 1) >= 0)
                start_win = (end_win - win_size) + 1;
            else
                start_win = (MAX_TID_VALUE - (win_size - seq_num)) + 1;

            if ((ret = wlan_11n_dispatch_pkt_until_start_win(priv,
                                                             rx_reor_tbl_ptr,
                                                             start_win))) {
                goto done;
            }
        }

        PRINTM(MDAT_D, "3:seq_num %d start_win %d win_size %d"
               " end_win %d\n", seq_num, start_win, win_size, end_win);
        if (pkt_type != PKT_TYPE_BAR) {
            if (seq_num >= start_win) {
                if (rx_reor_tbl_ptr->rx_reorder_ptr[seq_num - start_win]) {
                    PRINTM(MDAT_D, "Drop Duplicate Pkt\n");
                    ret = MLAN_STATUS_FAILURE;
                    goto done;
                }
                rx_reor_tbl_ptr->rx_reorder_ptr[seq_num - start_win] = payload;
            } else {            /* Wrap condition */
                if (rx_reor_tbl_ptr->rx_reorder_ptr[(seq_num
                                                     + (MAX_TID_VALUE)) -
                                                    start_win]) {
                    PRINTM(MDAT_D, "Drop Duplicate Pkt\n");
                    ret = MLAN_STATUS_FAILURE;
                    goto done;
                }
                rx_reor_tbl_ptr->rx_reorder_ptr[(seq_num
                                                 + (MAX_TID_VALUE)) -
                                                start_win] = payload;
            }
        }

        wlan_11n_display_tbl_ptr(pmadapter, rx_reor_tbl_ptr);

        /*
         * Dispatch all packets sequentially from start_win until a
         * hole is found and adjust the start_win appropriately
         */
        ret = wlan_11n_scan_and_dispatch(priv, rx_reor_tbl_ptr);

        wlan_11n_display_tbl_ptr(pmadapter, rx_reor_tbl_ptr);
    }

  done:
    if (!rx_reor_tbl_ptr->timer_context.timer_is_set ||
        (prev_start_win != rx_reor_tbl_ptr->start_win)) {

        mlan_11n_rxreorder_timer_restart(pmadapter, rx_reor_tbl_ptr);
    }

    LEAVE();
    return ret;
}

/**
 *  @brief This function will delete an entry for a given tid/ta pair. tid/ta
 *  		are taken from delba_event body
 *
 *  @param priv    	    A pointer to mlan_private
 *  @param tid		    tid to send delba
 *  @param peer_mac	    MAC address to send delba
 *  @param type 	    TYPE_DELBA_SENT	or TYPE_DELBA_RECEIVE
 *  @param initiator    MTRUE if we are initiator of ADDBA, MFALSE otherwise
 *
 *  @return 	   	    N/A
 */
void
mlan_11n_delete_bastream_tbl(mlan_private * priv, int tid,
                             t_u8 * peer_mac, t_u8 type, int initiator)
{
    RxReorderTbl *rx_reor_tbl_ptr;
    TxBAStreamTbl *ptxtbl;
    t_u8 cleanup_rx_reorder_tbl;

    ENTER();

    if (type == TYPE_DELBA_RECEIVE)
        cleanup_rx_reorder_tbl = (initiator) ? MTRUE : MFALSE;
    else
        cleanup_rx_reorder_tbl = (initiator) ? MFALSE : MTRUE;

    PRINTM(MEVENT, "DELBA: %02x:%02x:%02x:%02x:%02x:%02x tid=%d,"
           "initiator=%d\n", peer_mac[0],
           peer_mac[1], peer_mac[2],
           peer_mac[3], peer_mac[4], peer_mac[5], tid, initiator);

    if (cleanup_rx_reorder_tbl) {
        if (!(rx_reor_tbl_ptr = wlan_11n_get_rxreorder_tbl(priv, tid,
                                                           peer_mac))) {
            PRINTM(MWARN, "TID, TA not found in table!\n");
            LEAVE();
            return;
        }
        wlan_11n_delete_rxreorder_tbl_entry(priv, rx_reor_tbl_ptr);
    } else {
        if (!(ptxtbl = wlan_11n_get_txbastream_tbl(priv, tid, peer_mac))) {
            PRINTM(MWARN, "TID, RA not found in table!\n");
            LEAVE();
            return;
        }

        wlan_11n_delete_txbastream_tbl_entry(priv, ptxtbl);
    }

    LEAVE();
}

/**
 *  @brief This function handles the command response of
 *  		a block ack response
 *
 *  @param priv    A pointer to mlan_private structure
 *  @param resp    A pointer to HostCmd_DS_COMMAND
 *
 *  @return        MLAN_STATUS_SUCCESS
 */
mlan_status
wlan_ret_11n_addba_resp(mlan_private * priv, HostCmd_DS_COMMAND * resp)
{
    HostCmd_DS_11N_ADDBA_RSP *padd_ba_rsp = (HostCmd_DS_11N_ADDBA_RSP *)
        & resp->params.add_ba_rsp;
    int tid;
    RxReorderTbl *rx_reor_tbl_ptr = MNULL;

    ENTER();

    padd_ba_rsp->status_code = wlan_le16_to_cpu(padd_ba_rsp->status_code);
    padd_ba_rsp->block_ack_param_set =
        wlan_le16_to_cpu(padd_ba_rsp->block_ack_param_set);
    padd_ba_rsp->block_ack_tmo = wlan_le16_to_cpu(padd_ba_rsp->block_ack_tmo);
    padd_ba_rsp->ssn = wlan_le16_to_cpu(padd_ba_rsp->ssn);

    tid = (padd_ba_rsp->block_ack_param_set & BLOCKACKPARAM_TID_MASK)
        >> BLOCKACKPARAM_TID_POS;
    /* Check if we had rejected the ADDBA, if yes then do not create the stream */
    if (padd_ba_rsp->status_code == BA_RESULT_SUCCESS) {
        PRINTM(MCMND,
               "ADDBA RSP: %02x:%02x:%02x:%02x:%02x:%02x tid=%d ssn=%d win_size=%d,amsdu=%d\n",
               padd_ba_rsp->peer_mac_addr[0], padd_ba_rsp->peer_mac_addr[1],
               padd_ba_rsp->peer_mac_addr[2], padd_ba_rsp->peer_mac_addr[3],
               padd_ba_rsp->peer_mac_addr[4], padd_ba_rsp->peer_mac_addr[5],
               tid, padd_ba_rsp->ssn,
               ((padd_ba_rsp->block_ack_param_set & BLOCKACKPARAM_WINSIZE_MASK)
                >> BLOCKACKPARAM_WINSIZE_POS),
               padd_ba_rsp->
               block_ack_param_set & BLOCKACKPARAM_AMSDU_SUPP_MASK);

        if ((rx_reor_tbl_ptr =
             wlan_11n_get_rxreorder_tbl(priv, tid,
                                        padd_ba_rsp->peer_mac_addr))) {
            rx_reor_tbl_ptr->ba_status = BA_STREAM_SETUP_COMPLETE;
            if ((padd_ba_rsp->
                 block_ack_param_set & BLOCKACKPARAM_AMSDU_SUPP_MASK) &&
                priv->add_ba_param.rx_amsdu &&
                (priv->aggr_prio_tbl[tid].amsdu != BA_STREAM_NOT_ALLOWED))
                rx_reor_tbl_ptr->amsdu = MTRUE;
            else
                rx_reor_tbl_ptr->amsdu = MFALSE;
        }
    } else {
        PRINTM(MERROR,
               "ADDBA RSP: Failed(%02x:%02x:%02x:%02x:%02x:%02x tid=%d)\n",
               padd_ba_rsp->peer_mac_addr[0], padd_ba_rsp->peer_mac_addr[1],
               padd_ba_rsp->peer_mac_addr[2], padd_ba_rsp->peer_mac_addr[3],
               padd_ba_rsp->peer_mac_addr[4], padd_ba_rsp->peer_mac_addr[5],
               tid);
        if ((rx_reor_tbl_ptr =
             wlan_11n_get_rxreorder_tbl(priv, tid,
                                        padd_ba_rsp->peer_mac_addr))) {
            wlan_11n_delete_rxreorder_tbl_entry(priv, rx_reor_tbl_ptr);
        }
    }

    LEAVE();
    return MLAN_STATUS_SUCCESS;
}

/**
 *  @brief This function handles ba_stream_timeout event
 *
 *  @param priv    	A pointer to mlan_private
 *  @param event    A pointer to structure HostCmd_DS_11N_BATIMEOUT
 *
 *  @return 	   	N/A
 */
void
wlan_11n_ba_stream_timeout(mlan_private * priv,
                           HostCmd_DS_11N_BATIMEOUT * event)
{
    HostCmd_DS_11N_DELBA delba;

    ENTER();

    DBG_HEXDUMP(MCMD_D, "Event:", (t_u8 *) event, 20);

    memset(priv->adapter, &delba, 0, sizeof(HostCmd_DS_11N_DELBA));
    memcpy(priv->adapter, delba.peer_mac_addr, event->peer_mac_addr,
           MLAN_MAC_ADDR_LENGTH);

    delba.del_ba_param_set |= (t_u16) event->tid << DELBA_TID_POS;
    delba.del_ba_param_set |= (t_u16) event->origninator << DELBA_INITIATOR_POS;
    delba.reason_code = REASON_CODE_STA_TIMEOUT;
    wlan_prepare_cmd(priv, HostCmd_CMD_11N_DELBA, 0, 0, MNULL, &delba);

    LEAVE();
    return;
}

/**
 *  @brief This function cleans up reorder tbl
 *
 *  @param priv    	A pointer to mlan_private
 *
 *  @return 	   	N/A
 */
void
wlan_11n_cleanup_reorder_tbl(mlan_private * priv)
{
    RxReorderTbl *del_tbl_ptr;

    ENTER();

    while ((del_tbl_ptr = (RxReorderTbl *)
            util_peek_list(priv->adapter->pmoal_handle,
                           &priv->rx_reorder_tbl_ptr,
                           priv->adapter->callbacks.moal_spin_lock,
                           priv->adapter->callbacks.moal_spin_unlock))) {
        wlan_11n_delete_rxreorder_tbl_entry(priv, del_tbl_ptr);
    }

    util_init_list((pmlan_linked_list) & priv->rx_reorder_tbl_ptr);

    memset(priv->adapter, priv->rx_seq, 0xff, sizeof(priv->rx_seq));
    LEAVE();
}

#ifndef CONFIG_MLAN_WMSDK
/**
 *  @brief This function handle the rxba_sync event
 *
 *  @param priv    	  A pointer to mlan_private
 *  @param event_buf  A pointer to event buf
 *  @param len        event_buf length
 *  @return 	   	N/A
 */
void
wlan_11n_rxba_sync_event(mlan_private * priv, t_u8 * event_buf, t_u16 len)
{
    MrvlIEtypes_RxBaSync_t *tlv_rxba = (MrvlIEtypes_RxBaSync_t *) event_buf;
    t_u16 tlv_type, tlv_len;
    RxReorderTbl *rx_reor_tbl_ptr = MNULL;
    t_u8 i, j;
    t_u16 seq_num = 0;
    int tlv_buf_left = len;
    ENTER();
    DBG_HEXDUMP(MEVT_D, "RXBA_SYNC_EVT", event_buf, len);
    while (tlv_buf_left >= sizeof(MrvlIEtypes_RxBaSync_t)) {
        tlv_type = wlan_le16_to_cpu(tlv_rxba->header.type);
        tlv_len = wlan_le16_to_cpu(tlv_rxba->header.len);
        if (tlv_type != TLV_TYPE_RXBA_SYNC) {
            PRINTM(MERROR, "Wrong TLV id=0x%x\n", tlv_type);
            goto done;
        }
        tlv_rxba->seq_num = wlan_le16_to_cpu(tlv_rxba->seq_num);
        tlv_rxba->bitmap_len = wlan_le16_to_cpu(tlv_rxba->bitmap_len);
        PRINTM(MEVENT,
               "%02x:%02x:%02x:%02x:%02x:%02x tid=%d seq_num=%d bitmap_len=%d\n",
               tlv_rxba->mac[0], tlv_rxba->mac[1], tlv_rxba->mac[2],
               tlv_rxba->mac[3], tlv_rxba->mac[4], tlv_rxba->mac[5],
               tlv_rxba->tid, tlv_rxba->seq_num, tlv_rxba->bitmap_len);
        rx_reor_tbl_ptr =
            wlan_11n_get_rxreorder_tbl(priv, tlv_rxba->tid, tlv_rxba->mac);
        if (!rx_reor_tbl_ptr) {
            PRINTM(MEVENT, "Can not find rx_reorder_tbl\n");
            goto done;
        }
        for (i = 0; i < tlv_rxba->bitmap_len; i++) {
            for (j = 0; j < 8; j++) {
                if (tlv_rxba->bitmap[i] & (1 << j)) {
                    seq_num =
                        (tlv_rxba->seq_num + i * 8 + j) & (MAX_TID_VALUE - 1);
                    PRINTM(MEVENT,
                           "Fw dropped packet, seq=%d start_win=%d, win_size=%d\n",
                           seq_num, rx_reor_tbl_ptr->start_win,
                           rx_reor_tbl_ptr->win_size);
                    if (MLAN_STATUS_SUCCESS !=
                        mlan_11n_rxreorder_pkt(priv, seq_num, tlv_rxba->tid,
                                               tlv_rxba->mac, 0,
                                               (t_void *) RX_PKT_DROPPED_IN_FW))
                    {
                        PRINTM(MERROR,
                               "Fail to handle dropped packet, seq=%d\n",
                               seq_num);
                    }
                }
            }
        }
        tlv_buf_left -= (sizeof(MrvlIEtypesHeader_t) + tlv_len);
        tlv_rxba =
            (MrvlIEtypes_RxBaSync_t *) ((t_u8 *) tlv_rxba + tlv_len +
                                        sizeof(MrvlIEtypesHeader_t));
    }
  done:
    LEAVE();
    return;
}

/**
 *  @brief This function will send a DELBA for each entry in the priv's
 *          rx reordering table
 *
 *  @param priv    A pointer to mlan_private
 */
t_void
wlan_send_delba_to_all_in_reorder_tbl(pmlan_private priv)
{
    RxReorderTbl *rx_reor_tbl_ptr;

    ENTER();

    if (!
        (rx_reor_tbl_ptr =
         (RxReorderTbl *) util_peek_list(priv->adapter->pmoal_handle,
                                         &priv->rx_reorder_tbl_ptr,
                                         priv->adapter->callbacks.
                                         moal_spin_lock,
                                         priv->adapter->callbacks.
                                         moal_spin_unlock))) {
        LEAVE();
        return;
    }

    while (rx_reor_tbl_ptr != (RxReorderTbl *) & priv->rx_reorder_tbl_ptr) {
        if (rx_reor_tbl_ptr->ba_status == BA_STREAM_SETUP_COMPLETE) {
            rx_reor_tbl_ptr->ba_status = BA_STREAM_SETUP_INPROGRESS;
            wlan_send_delba(priv, rx_reor_tbl_ptr->tid, rx_reor_tbl_ptr->ta, 0);
        }
        rx_reor_tbl_ptr = rx_reor_tbl_ptr->pnext;
    }
    LEAVE();
}
#endif /* CONFIG_MLAN_WMSDK */
/**
 *  @brief This function cleans up reorder tbl for specific station
 *
 *  @param priv    	A pointer to mlan_private
 *  @param ta      ta to find in reordering table
 *  @return 	   	N/A
 */
void
wlan_cleanup_reorder_tbl(mlan_private * priv, t_u8 * ta)
{
    RxReorderTbl *rx_reor_tbl_ptr = MNULL;
    t_u8 i;
    ENTER();
    for (i = 0; i < MAX_NUM_TID; ++i) {
        if ((rx_reor_tbl_ptr = wlan_11n_get_rxreorder_tbl(priv, i, ta))) {
            wlan_11n_delete_rxreorder_tbl_entry(priv, rx_reor_tbl_ptr);
        }
    }
    LEAVE();
    return;
}

/**
 *  @brief This function will set force_no_drop flag in rxreorder_tbl.
 *
 *  @param priv    A pointer to mlan_private
 *  @param flag    MTRUE/MFALSE
 *
 *  @return	 N/A
 */
void
wlan_set_rxreorder_tbl_no_drop_flag(mlan_private * priv, t_u8 flag)
{
    RxReorderTbl *rx_reor_tbl_ptr;

    ENTER();

    if (!
        (rx_reor_tbl_ptr =
         (RxReorderTbl *) util_peek_list(priv->adapter->pmoal_handle,
                                         &priv->rx_reorder_tbl_ptr,
                                         priv->adapter->callbacks.
                                         moal_spin_lock,
                                         priv->adapter->callbacks.
                                         moal_spin_unlock))) {
        LEAVE();
        return;
    }

    while (rx_reor_tbl_ptr != (RxReorderTbl *) & priv->rx_reorder_tbl_ptr) {
        rx_reor_tbl_ptr->force_no_drop = flag;
        rx_reor_tbl_ptr = rx_reor_tbl_ptr->pnext;
    }

    LEAVE();
    return;
}

/**
 *  @brief This function update all the rx_reorder_tbl's force_no_drop flag
 *
 *  @param pmadapter   	A pointer to mlan_adapter
 *  @param flag		    MTRUE/MFALSE
 *  @return 	        N/A
 */
void
wlan_update_rxreorder_tbl(pmlan_adapter pmadapter, t_u8 flag)
{
    t_u8 i;
    pmlan_private priv = MNULL;
    for (i = 0; i < pmadapter->priv_num; i++) {
        if (pmadapter->priv[i]) {
            priv = pmadapter->priv[i];
            wlan_set_rxreorder_tbl_no_drop_flag(priv, flag);
        }
    }
    return;
}
