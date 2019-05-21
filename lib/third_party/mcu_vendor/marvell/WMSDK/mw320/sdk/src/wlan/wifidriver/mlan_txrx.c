/** @file mlan_txrx.c
 *
 *  @brief  This file provides the handling of TX/RX in MLAN
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
    05/11/2009: initial version
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

/********************************************************
                Global Variables
********************************************************/

/********************************************************
                Local Functions
********************************************************/

/********************************************************
                Global Functions
********************************************************/
/**
 *   @brief This function processes the received buffer
 *
 *   @param pmadapter A pointer to mlan_adapter
 *   @param pmbuf     A pointer to the received buffer
 *
 *   @return        MLAN_STATUS_SUCCESS or MLAN_STATUS_FAILURE
 */
mlan_status
wlan_handle_rx_packet(pmlan_adapter pmadapter, pmlan_buffer pmbuf)
{
    mlan_status ret = MLAN_STATUS_SUCCESS;
    pmlan_private priv = wlan_get_priv(pmadapter, MLAN_BSS_ROLE_ANY);
    RxPD *prx_pd;
#ifdef DEBUG_LEVEL1
    t_u32 sec, usec;
#endif

    ENTER();

    prx_pd = (RxPD *) (pmbuf->pbuf + pmbuf->data_offset);
    /* Get the BSS number from RxPD, get corresponding priv */
    priv =
        wlan_get_priv_by_id(pmadapter, prx_pd->bss_num & BSS_NUM_MASK,
                            prx_pd->bss_type);
    if (!priv)
        priv = wlan_get_priv(pmadapter, MLAN_BSS_ROLE_ANY);
    pmbuf->bss_index = priv->bss_index;
    PRINTM_GET_SYS_TIME(MDATA, &sec, &usec);
    PRINTM_NETINTF(MDATA, priv);
    /* PRINTM(MDATA, "%lu.%06lu : Data <= FW\n", sec, usec); */
    ret = priv->ops.process_rx_packet(pmadapter, pmbuf);

    LEAVE();
    return ret;
}

/**
 *  @brief This function checks the conditions and sends packet to device
 *
 *  @param priv	   A pointer to mlan_private structure
 *  @param pmbuf   A pointer to the mlan_buffer for process
 *  @param tx_param A pointer to mlan_tx_param structure
 *
 *  @return 	    MLAN_STATUS_SUCCESS/MLAN_STATUS_PENDING --success, otherwise failure
 */
#ifndef CONFIG_MLAN_WMSDK
mlan_status
wlan_process_tx(pmlan_private priv, pmlan_buffer pmbuf,
                mlan_tx_param * tx_param)
{
    mlan_status ret = MLAN_STATUS_SUCCESS;
    pmlan_adapter pmadapter = priv->adapter;
    t_u8 *head_ptr = MNULL;
#ifdef DEBUG_LEVEL1
    t_u32 sec, usec;
#endif
#ifdef STA_SUPPORT
    TxPD *plocal_tx_pd = MNULL;
#endif

    ENTER();

    head_ptr = (t_u8 *) priv->ops.process_txpd(priv, pmbuf);
    if (!head_ptr) {
        pmbuf->status_code = MLAN_ERROR_PKT_INVALID;
        ret = MLAN_STATUS_FAILURE;
        goto done;
    }
#ifdef STA_SUPPORT
    if (GET_BSS_ROLE(priv) == MLAN_BSS_ROLE_STA)
        plocal_tx_pd = (TxPD *) (head_ptr + INTF_HEADER_LEN);
#endif
    ret = wlan_sdio_host_to_card(pmadapter, MLAN_TYPE_DATA, pmbuf, tx_param);
  done:
    switch (ret) {
    case MLAN_STATUS_RESOURCE:
#ifdef STA_SUPPORT
        if ((GET_BSS_ROLE(priv) == MLAN_BSS_ROLE_STA) &&
            pmadapter->pps_uapsd_mode && (pmadapter->tx_lock_flag == MTRUE)) {
            pmadapter->tx_lock_flag = MFALSE;
            plocal_tx_pd->flags = 0;
        }
#endif
        PRINTM(MINFO, "MLAN_STATUS_RESOURCE is returned\n");
        break;
    case MLAN_STATUS_FAILURE:
        pmadapter->data_sent = MFALSE;
        PRINTM(MERROR, "Error: host_to_card failed: 0x%X\n", ret);
        pmadapter->dbg.num_tx_host_to_card_failure++;
        pmbuf->status_code = MLAN_ERROR_DATA_TX_FAIL;
        wlan_write_data_complete(pmadapter, pmbuf, ret);
        break;
    case MLAN_STATUS_PENDING:
        pmadapter->data_sent = MFALSE;
        DBG_HEXDUMP(MDAT_D, "Tx", head_ptr + INTF_HEADER_LEN,
                    MIN(pmbuf->data_len + sizeof(TxPD), MAX_DATA_DUMP_LEN));
        break;
    case MLAN_STATUS_SUCCESS:
        DBG_HEXDUMP(MDAT_D, "Tx", head_ptr + INTF_HEADER_LEN,
                    MIN(pmbuf->data_len + sizeof(TxPD), MAX_DATA_DUMP_LEN));
        wlan_write_data_complete(pmadapter, pmbuf, ret);
        break;
    default:
        break;
    }

    if ((ret == MLAN_STATUS_SUCCESS) || (ret == MLAN_STATUS_PENDING)) {
        PRINTM_GET_SYS_TIME(MDATA, &sec, &usec);
        PRINTM_NETINTF(MDATA, priv);
        PRINTM(MDATA, "%lu.%06lu : Data => FW\n", sec, usec);
    }
    LEAVE();
    return ret;
}

/**
 *  @brief Packet send completion handling
 *
 *  @param pmadapter		A pointer to mlan_adapter structure
 *  @param pmbuf		A pointer to mlan_buffer structure
 *  @param status		Callback status
 *
 *  @return			MLAN_STATUS_SUCCESS
 */
mlan_status
wlan_write_data_complete(IN pmlan_adapter pmadapter,
                         IN pmlan_buffer pmbuf, IN mlan_status status)
{
    mlan_status ret = MLAN_STATUS_SUCCESS;
    pmlan_callbacks pcb;

    ENTER();

    MASSERT(pmadapter && pmbuf);

    pcb = &pmadapter->callbacks;
    if ((pmbuf->buf_type == MLAN_BUF_TYPE_DATA) ||
        (pmbuf->buf_type == MLAN_BUF_TYPE_RAW_DATA)) {
        PRINTM(MINFO, "wlan_write_data_complete: DATA %p\n", pmbuf);
        if (pmbuf->flags & MLAN_BUF_FLAG_MOAL_TX_BUF) {
            /* pmbuf was allocated by MOAL */
            pcb->moal_send_packet_complete(pmadapter->pmoal_handle,
                                           pmbuf, status);
        } else {
            if (pmbuf->flags & MLAN_BUF_FLAG_BRIDGE_BUF) {
                pmadapter->pending_bridge_pkts--;
            }
            /* pmbuf was allocated by MLAN */
            wlan_free_mlan_buffer(pmadapter, pmbuf);
        }
    }

    LEAVE();
    return ret;
}

/**
 *  @brief Packet receive completion callback handler
 *
 *  @param pmadapter		A pointer to mlan_adapter structure
 *  @param pmbuf		A pointer to mlan_buffer structure
 *  @param status		Callback status
 *
 *  @return			MLAN_STATUS_SUCCESS
 */
mlan_status
wlan_recv_packet_complete(IN pmlan_adapter pmadapter,
                          IN pmlan_buffer pmbuf, IN mlan_status status)
{
    mlan_status ret = MLAN_STATUS_SUCCESS;
    pmlan_private pmp;
    pmlan_callbacks pcb;
    pmlan_buffer pmbuf_parent;

    ENTER();

    MASSERT(pmadapter && pmbuf);
    pcb = &pmadapter->callbacks;
    MASSERT(pmbuf->bss_index < pmadapter->priv_num);

    pmp = pmadapter->priv[pmbuf->bss_index];

    if (pmbuf->pparent) {
        pmbuf_parent = pmbuf->pparent;

        pmadapter->callbacks.moal_spin_lock(pmadapter->pmoal_handle,
                                            pmp->rx_pkt_lock);
        --pmbuf_parent->use_count;
        if (!pmbuf_parent->use_count) {
            pmadapter->callbacks.moal_spin_unlock(pmadapter->pmoal_handle,
                                                  pmp->rx_pkt_lock);
            wlan_free_mlan_buffer(pmadapter, pmbuf_parent);
        } else {
            pmadapter->callbacks.moal_spin_unlock(pmadapter->pmoal_handle,
                                                  pmp->rx_pkt_lock);
        }
        wlan_free_mlan_buffer(pmadapter, pmbuf);
    } else {
        wlan_free_mlan_buffer(pmadapter, pmbuf);
    }

    LEAVE();
    return ret;
}

/**
 *  @brief Add packet to Bypass TX queue
 *
 *  @param pmadapter  Pointer to the mlan_adapter driver data struct
 *  @param pmbuf      Pointer to the mlan_buffer data struct
 *
 *  @return         N/A
 */
t_void
wlan_add_buf_bypass_txqueue(mlan_adapter * pmadapter, pmlan_buffer pmbuf)
{
    ENTER();

    if (pmbuf->buf_type != MLAN_BUF_TYPE_RAW_DATA) {
        pmbuf->buf_type = MLAN_BUF_TYPE_DATA;
    }

    util_enqueue_list_tail(pmadapter->pmoal_handle, &pmadapter->bypass_txq,
                           (pmlan_linked_list) pmbuf,
                           pmadapter->callbacks.moal_spin_lock,
                           pmadapter->callbacks.moal_spin_unlock);
    LEAVE();
}

/**
 *  @brief Check if packets are available in Bypass TX queue
 *
 *  @param pmadapter  Pointer to the mlan_adapter driver data struct
 *
 *  @return         MFALSE if not empty; MTRUE if empty
 */
INLINE t_u8
wlan_bypass_tx_list_empty(mlan_adapter * pmadapter)
{
    t_u8 q_empty = MTRUE;
    pmlan_callbacks pcb = &pmadapter->callbacks;
    ENTER();

    q_empty = (util_peek_list(pmadapter->pmoal_handle, &pmadapter->bypass_txq,
                              pcb->moal_spin_lock,
                              pcb->moal_spin_unlock)) ? MFALSE : MTRUE;

    LEAVE();
    return q_empty;
}

/**
 *  @brief Clean up the By-pass TX queue
 *
 *  @param pmadapter Pointer to the mlan_adapter driver data struct
 *
 *  @return      N/A
 */
t_void
wlan_cleanup_bypass_txq(mlan_adapter * pmadapter)
{
    pmlan_buffer pmbuf;
    ENTER();

    pmadapter->callbacks.moal_spin_lock(pmadapter->pmoal_handle,
                                        pmadapter->bypass_txq.plock);

    while ((pmbuf =
            (pmlan_buffer) util_peek_list(pmadapter->pmoal_handle,
                                          &pmadapter->bypass_txq, MNULL,
                                          MNULL))) {
        util_unlink_list(pmadapter->pmoal_handle, &pmadapter->bypass_txq,
                         (pmlan_linked_list) pmbuf, MNULL, MNULL);
        wlan_write_data_complete(pmadapter, pmbuf, MLAN_STATUS_FAILURE);
    }

    pmadapter->callbacks.moal_spin_unlock(pmadapter->pmoal_handle,
                                          pmadapter->bypass_txq.plock);

    LEAVE();
}

/**
 *  @brief Transmit the By-passed packet awaiting in by-pass queue
 *
 *  @param pmadapter Pointer to the mlan_adapter driver data struct
 *
 *  @return        N/A
 */
t_void
wlan_process_bypass_tx(pmlan_adapter pmadapter)
{
    pmlan_buffer pmbuf;
    mlan_tx_param tx_param;
    mlan_status status = MLAN_STATUS_SUCCESS;

    ENTER();

    if ((pmbuf = (pmlan_buffer) util_dequeue_list(pmadapter->pmoal_handle,
                                                  &pmadapter->bypass_txq,
                                                  pmadapter->callbacks.
                                                  moal_spin_lock,
                                                  pmadapter->callbacks.
                                                  moal_spin_unlock))) {
        PRINTM(MINFO, "Dequeuing bypassed packet %p\n", pmbuf);
        /* XXX: nex_pkt_len ??? */
        tx_param.next_pkt_len = 0;
        status =
            wlan_process_tx(pmadapter->priv[pmbuf->bss_index], pmbuf,
                            &tx_param);

        if (status == MLAN_STATUS_RESOURCE) {
            /* Queue the packet again so that it will be TX'ed later */
            util_enqueue_list_head(pmadapter->pmoal_handle,
                                   &pmadapter->bypass_txq,
                                   (pmlan_linked_list) pmbuf,
                                   pmadapter->callbacks.moal_spin_lock,
                                   pmadapter->callbacks.moal_spin_unlock);
        }
    } else {
        PRINTM(MINFO, "Nothing to send\n");
    }

    LEAVE();
}
#endif /* CONFIG_MLAN_WMSDK */
