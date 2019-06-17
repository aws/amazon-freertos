/** @file mlan_11n_aggr.c
 *
 *  @brief  This file provides contains functions for 11n Aggregation
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

void wrapper_deliver_amsdu_subframe(pmlan_buffer amsdu_pmbuf,
				    t_u8 *data, t_u16 pkt_len);

#ifndef CONFIG_MLAN_WMSDK
/**
 *  @brief Aggregate individual packets into one AMSDU packet
 *
 *  @param pmadapter A pointer to mlan_adapter structure
 *  @param amsdu_buf A pointer to packet buffer
 *  @param data      A pointer to aggregated data packet being formed
 *  @param pkt_len   Length of current packet to aggregate
 *  @param pad       Pad
 *
 *  @return         Final packet size
 */
static int
wlan_11n_form_amsdu_pkt(pmlan_adapter pmadapter, t_u8 * amsdu_buf, t_u8 * data,
                        int pkt_len, int *pad)
{
    int dt_offset, amsdu_buf_offset;
    Rfc1042Hdr_t snap = {
        0xaa,                   /* LLC DSAP */
        0xaa,                   /* LLC SSAP */
        0x03,                   /* LLC CTRL */
        {0x00, 0x00, 0x00},     /* SNAP OUI */
        0x0000                  /* SNAP type */
            /*
             * This field will be overwritten
             * later with ethertype
             */
    };

    ENTER();

    memcpy(pmadapter, amsdu_buf, data, (MLAN_MAC_ADDR_LENGTH) * 2);
    dt_offset = amsdu_buf_offset = (MLAN_MAC_ADDR_LENGTH) * 2;

    snap.snap_type = *(t_u16 *) (data + dt_offset);
    dt_offset += sizeof(t_u16);
    *(t_u16 *) (amsdu_buf + amsdu_buf_offset) = mlan_htons(pkt_len +
                                                           LLC_SNAP_LEN -
                                                           ((2 *
                                                             MLAN_MAC_ADDR_LENGTH)
                                                            + sizeof(t_u16)));
    amsdu_buf_offset += sizeof(t_u16);
    memcpy(pmadapter, amsdu_buf + amsdu_buf_offset, &snap, LLC_SNAP_LEN);
    amsdu_buf_offset += LLC_SNAP_LEN;

    memcpy(pmadapter, amsdu_buf + amsdu_buf_offset, data + dt_offset,
           pkt_len - dt_offset);
    *pad =
        (((pkt_len + LLC_SNAP_LEN) & 3)) ? (4 -
                                            (((pkt_len +
                                               LLC_SNAP_LEN)) & 3)) : 0;

    LEAVE();
    return (pkt_len + LLC_SNAP_LEN + *pad);
}

/**
 *  @brief Add TxPD to AMSDU header
 *
 *  @param priv     A pointer to mlan_private structure
 *  @param mbuf		Pointer to buffer where the TxPD will be formed
 *
 *  @return		N/A
 */
static void
wlan_11n_form_amsdu_txpd(mlan_private * priv, mlan_buffer * mbuf)
{
    TxPD *ptx_pd;
    mlan_adapter *pmadapter = priv->adapter;

    ENTER();

    ptx_pd = (TxPD *) mbuf->pbuf;
    memset(pmadapter, ptx_pd, 0, sizeof(TxPD));

    /*
     * Original priority has been overwritten
     */
    ptx_pd->priority = (t_u8) mbuf->priority;
    ptx_pd->pkt_delay_2ms = wlan_wmm_compute_driver_packet_delay(priv, mbuf);
    ptx_pd->bss_num = GET_BSS_NUM(priv);
    ptx_pd->bss_type = priv->bss_type;
    /* Always zero as the data is followed by TxPD */
    ptx_pd->tx_pkt_offset = sizeof(TxPD);
    ptx_pd->tx_pkt_type = PKT_TYPE_AMSDU;

    if (ptx_pd->tx_control == 0)
        /* TxCtrl set by user or default */
        ptx_pd->tx_control = priv->pkt_tx_ctrl;

    endian_convert_TxPD(ptx_pd);

    LEAVE();
}

/**
 *  @brief Update the TxPktLength field in TxPD after the complete AMSDU
 *  packet is formed
 *
 *  @param priv     A pointer to mlan_private structure
 *  @param mbuf     	TxPD buffer
 *
 *  @return		N/A
 */
static INLINE void
wlan_11n_update_pktlen_amsdu_txpd(mlan_private * priv, pmlan_buffer mbuf)
{
    TxPD *ptx_pd;
    ENTER();

    ptx_pd = (TxPD *) mbuf->pbuf;
    ptx_pd->tx_pkt_length =
        (t_u16) wlan_cpu_to_le16(mbuf->data_len - sizeof(TxPD));
#ifdef STA_SUPPORT
    if ((GET_BSS_ROLE(priv) == MLAN_BSS_ROLE_STA) &&
        (priv->adapter->pps_uapsd_mode)) {
        if (MTRUE == wlan_check_last_packet_indication(priv)) {
            priv->adapter->tx_lock_flag = MTRUE;
            ptx_pd->flags = MRVDRV_TxPD_POWER_MGMT_LAST_PACKET;
        }
    }
#endif /* STA_SUPPORT */
    LEAVE();
}
#endif /* CONFIG_MLAN_WMSDK */

/**
 *  @brief Get number of aggregated packets
 *
 *  @param data			A pointer to packet data
 *  @param total_pkt_len	Total packet length
 *
 *  @return			Number of packets
 */
static int
wlan_11n_get_num_aggrpkts(t_u8 * data, int total_pkt_len)
{
    int pkt_count = 0, pkt_len, pad;

    ENTER();
    while (total_pkt_len > 0) {
        /* Length will be in network format, change it to host */
        pkt_len = mlan_ntohs((*(t_u16 *) (data + (2 * MLAN_MAC_ADDR_LENGTH))));
        pad = (((pkt_len + sizeof(Eth803Hdr_t)) & 3)) ?
            (4 - ((pkt_len + sizeof(Eth803Hdr_t)) & 3)) : 0;
        data += pkt_len + pad + sizeof(Eth803Hdr_t);
        total_pkt_len -= pkt_len + pad + sizeof(Eth803Hdr_t);
        ++pkt_count;
    }
    LEAVE();
    return pkt_count;
}


/********************************************************
    Global Functions
********************************************************/

/**
 *  @brief Deaggregate the received AMSDU packet
 *
 *  @param priv		A pointer to mlan_private structure
 *  @param pmbuf	A pointer to aggregated data packet
 *
 *  @return		MLAN_STATUS_SUCCESS --success, otherwise fail
 */
mlan_status
wlan_11n_deaggregate_pkt(mlan_private * priv, pmlan_buffer pmbuf)
{
    t_u16 pkt_len;
    int total_pkt_len;
    t_u8 *data;
    int pad;
    mlan_status ret = MLAN_STATUS_FAILURE;
    RxPacketHdr_t *prx_pkt;
    mlan_buffer *daggr_mbuf = MNULL;
    /* mlan_adapter *pmadapter = priv->adapter; */
    t_u8 rfc1042_eth_hdr[MLAN_MAC_ADDR_LENGTH] = { 0xaa, 0xaa, 0x03,
        0x00, 0x00, 0x00
    };

    ENTER();

    data = (t_u8 *) (pmbuf->pbuf + pmbuf->data_offset);
    total_pkt_len = pmbuf->data_len;

    /* Sanity test */
    if (total_pkt_len > MLAN_RX_DATA_BUF_SIZE) {
        PRINTM(MERROR, "Total packet length greater than tx buffer"
               " size %d\n", total_pkt_len);
        goto done;
    }

    pmbuf->use_count = wlan_11n_get_num_aggrpkts(data, total_pkt_len);

    while (total_pkt_len > 0) {
        prx_pkt = (RxPacketHdr_t *) data;
        /* Length will be in network format, change it to host */
        pkt_len = mlan_ntohs((*(t_u16 *) (data + (2 * MLAN_MAC_ADDR_LENGTH))));
        if (pkt_len > total_pkt_len) {
            PRINTM(MERROR,
                   "Error in packet length: total_pkt_len = %d, pkt_len = %d\n",
                   total_pkt_len, pkt_len);
            break;
        }

        pad = (((pkt_len + sizeof(Eth803Hdr_t)) & 3)) ?
            (4 - ((pkt_len + sizeof(Eth803Hdr_t)) & 3)) : 0;

        total_pkt_len -= pkt_len + pad + sizeof(Eth803Hdr_t);

        if (memcmp(pmadapter, &prx_pkt->rfc1042_hdr,
                   rfc1042_eth_hdr, sizeof(rfc1042_eth_hdr)) == 0) {
            memmove(pmadapter, data + LLC_SNAP_LEN, data, (2 *
                                                           MLAN_MAC_ADDR_LENGTH));
            data += LLC_SNAP_LEN;
            pkt_len += sizeof(Eth803Hdr_t) - LLC_SNAP_LEN;
        } else {
            *(t_u16 *) (data + (2 * MLAN_MAC_ADDR_LENGTH))
                = (t_u16) 0;
            pkt_len += sizeof(Eth803Hdr_t);
        }

#ifndef CONFIG_MLAN_WMSDK
        daggr_mbuf = wlan_alloc_mlan_buffer(pmadapter, pkt_len, 0, MFALSE);
        if (daggr_mbuf == MNULL) {
            PRINTM(MERROR, "Error allocating daggr mlan_buffer\n");
            LEAVE();
            return MLAN_STATUS_FAILURE;
        }
        daggr_mbuf->bss_index = pmbuf->bss_index;
        daggr_mbuf->buf_type = pmbuf->buf_type;
        daggr_mbuf->data_len = pkt_len;
        daggr_mbuf->in_ts_sec = pmbuf->in_ts_sec;
        daggr_mbuf->in_ts_usec = pmbuf->in_ts_usec;
        daggr_mbuf->pparent = pmbuf;
        daggr_mbuf->priority = pmbuf->priority;
        memcpy(pmadapter, daggr_mbuf->pbuf + daggr_mbuf->data_offset, data,
               pkt_len);
#else
	/* This part is customized for WMSDK. We do not need and will not
	   allocate the mlan buffer. */
	wrapper_deliver_amsdu_subframe(pmbuf,  data, pkt_len);
	ret = MLAN_STATUS_SUCCESS;
#endif /* CONFIG_MLAN_WMSDK */

#ifndef CONFIG_MLAN_WMSDK
#ifdef UAP_SUPPORT
        if (GET_BSS_ROLE(priv) == MLAN_BSS_ROLE_UAP)
            ret = wlan_uap_recv_packet(priv, daggr_mbuf);
        else
#endif /* UAP_SUPPORT */
            ret =
                pmadapter->callbacks.moal_recv_packet(pmadapter->pmoal_handle,
                                                      daggr_mbuf);
#endif /* CONFIG_MLAN_WMSDK */

	switch (ret) {
        case MLAN_STATUS_PENDING:
            break;
        case MLAN_STATUS_FAILURE:
            PRINTM(MERROR, "Deaggr, send to moal failed\n");
            daggr_mbuf->status_code = MLAN_ERROR_PKT_INVALID;
        case MLAN_STATUS_SUCCESS:
#ifndef CONFIG_MLAN_WMSDK
		wlan_recv_packet_complete(pmadapter, daggr_mbuf, ret);
#endif /* CONFIG_MLAN_WMSDK */
            break;
        default:
            break;
        }

        data += pkt_len + pad;
    }

  done:
    LEAVE();
    return ret;
}

#ifndef CONFIG_MLAN_WMSDK
/**
 *  @brief Aggregate multiple packets into one single AMSDU packet
 *
 *  @param priv 	A pointer to mlan_private structure
 *  @param pra_list	Pointer to the RA List table containing the pointers
 *  			    to packets.
 *  @param headroom	Any interface specific headroom that may be need. TxPD
 *  				will be formed leaving this headroom.
 *  @param ptrindex	Pointer index
 *
 *  @return		Final packet size or MLAN_STATUS_FAILURE
 */
int
wlan_11n_aggregate_pkt(mlan_private * priv, raListTbl * pra_list,
                       int headroom, int ptrindex)
{
    int pkt_size = 0;
    pmlan_adapter pmadapter = priv->adapter;
    mlan_buffer *pmbuf_aggr, *pmbuf_src;
    t_u8 *data;
    int pad = 0;
    mlan_status ret = MLAN_STATUS_SUCCESS;
#ifdef DEBUG_LEVEL1
    t_u32 sec, usec;
#endif
    mlan_tx_param tx_param;
#ifdef STA_SUPPORT
    TxPD *ptx_pd = MNULL;
#endif
    t_u32 max_amsdu_size = MIN(pra_list->max_amsdu, pmadapter->tx_buf_size);
    ENTER();

    PRINTM(MDAT_D, "Handling Aggr packet\n");

    if ((pmbuf_src =
         (pmlan_buffer) util_peek_list(pmadapter->pmoal_handle,
                                       &pra_list->buf_head, MNULL, MNULL))) {

        if (!(pmbuf_aggr = wlan_alloc_mlan_buffer(pmadapter,
                                                  pmadapter->tx_buf_size, 0,
                                                  MTRUE))) {
            PRINTM(MERROR, "Error allocating mlan_buffer\n");
            pmadapter->callbacks.moal_spin_unlock(pmadapter->pmoal_handle,
                                                  priv->wmm.ra_list_spinlock);
            LEAVE();
            return MLAN_STATUS_FAILURE;
        }

        data = pmbuf_aggr->pbuf + headroom;
        pmbuf_aggr->bss_index = pmbuf_src->bss_index;
        pmbuf_aggr->buf_type = pmbuf_src->buf_type;
        pmbuf_aggr->priority = pmbuf_src->priority;
        pmbuf_aggr->pbuf = data;
        pmbuf_aggr->data_offset = 0;

        /* Form AMSDU */
        wlan_11n_form_amsdu_txpd(priv, pmbuf_aggr);
        pkt_size = sizeof(TxPD);
#ifdef STA_SUPPORT
        if (GET_BSS_ROLE(priv) == MLAN_BSS_ROLE_STA)
            ptx_pd = (TxPD *) pmbuf_aggr->pbuf;
#endif
    } else {
        pmadapter->callbacks.moal_spin_unlock(pmadapter->pmoal_handle,
                                              priv->wmm.ra_list_spinlock);
        goto exit;
    }

    while (pmbuf_src && ((pkt_size + (pmbuf_src->data_len + LLC_SNAP_LEN)
                          + headroom) <= max_amsdu_size)) {

        pmbuf_src = (pmlan_buffer)
            util_dequeue_list(pmadapter->pmoal_handle, &pra_list->buf_head,
                              MNULL, MNULL);

        pra_list->total_pkts--;

        /* decrement for every PDU taken from the list */
        priv->wmm.pkts_queued[ptrindex]--;
        util_scalar_decrement(pmadapter->pmoal_handle,
                              &priv->wmm.tx_pkts_queued, MNULL, MNULL);

        pmadapter->callbacks.moal_spin_unlock(pmadapter->pmoal_handle,
                                              priv->wmm.ra_list_spinlock);

        pkt_size += wlan_11n_form_amsdu_pkt(pmadapter,
                                            (data + pkt_size),
                                            pmbuf_src->pbuf +
                                            pmbuf_src->data_offset,
                                            pmbuf_src->data_len, &pad);

        DBG_HEXDUMP(MDAT_D, "pmbuf_src", pmbuf_src, sizeof(mlan_buffer));
        wlan_write_data_complete(pmadapter, pmbuf_src, MLAN_STATUS_SUCCESS);

        pmadapter->callbacks.moal_spin_lock(pmadapter->pmoal_handle,
                                            priv->wmm.ra_list_spinlock);

        if (!wlan_is_ralist_valid(priv, pra_list, ptrindex)) {
            pmadapter->callbacks.moal_spin_unlock(pmadapter->pmoal_handle,
                                                  priv->wmm.ra_list_spinlock);
            LEAVE();
            return MLAN_STATUS_FAILURE;
        }

        pmbuf_src =
            (pmlan_buffer) util_peek_list(pmadapter->pmoal_handle,
                                          &pra_list->buf_head, MNULL, MNULL);
    }

    pmadapter->callbacks.moal_spin_unlock(pmadapter->pmoal_handle,
                                          priv->wmm.ra_list_spinlock);

    /* Last AMSDU packet does not need padding */
    pkt_size -= pad;
    pmbuf_aggr->data_len = pkt_size;
    wlan_11n_update_pktlen_amsdu_txpd(priv, pmbuf_aggr);
    pmbuf_aggr->data_len += headroom;
    pmbuf_aggr->pbuf = data - headroom;
    tx_param.next_pkt_len = ((pmbuf_src) ?
                             pmbuf_src->data_len + sizeof(TxPD) : 0);

    ret = wlan_sdio_host_to_card(pmadapter, MLAN_TYPE_DATA,
                                 pmbuf_aggr, &tx_param);
    switch (ret) {
    case MLAN_STATUS_RESOURCE:
        pmadapter->callbacks.moal_spin_lock(pmadapter->pmoal_handle,
                                            priv->wmm.ra_list_spinlock);

        if (!wlan_is_ralist_valid(priv, pra_list, ptrindex)) {
            pmadapter->callbacks.moal_spin_unlock(pmadapter->pmoal_handle,
                                                  priv->wmm.ra_list_spinlock);
            pmbuf_aggr->status_code = MLAN_ERROR_PKT_INVALID;
            wlan_write_data_complete(pmadapter, pmbuf_aggr,
                                     MLAN_STATUS_FAILURE);
            LEAVE();
            return MLAN_STATUS_FAILURE;
        }
#ifdef STA_SUPPORT
        /* reset tx_lock_flag */
        if ((GET_BSS_ROLE(priv) == MLAN_BSS_ROLE_STA) &&
            pmadapter->pps_uapsd_mode && (pmadapter->tx_lock_flag == MTRUE)) {
            pmadapter->tx_lock_flag = MFALSE;
            ptx_pd->flags = 0;
        }
#endif
        util_enqueue_list_head(pmadapter->pmoal_handle, &pra_list->buf_head,
                               (pmlan_linked_list) pmbuf_aggr, MNULL, MNULL);

        pra_list->total_pkts++;

        /* add back only one: aggregated packet is requeued as one */
        priv->wmm.pkts_queued[ptrindex]++;
        util_scalar_increment(pmadapter->pmoal_handle,
                              &priv->wmm.tx_pkts_queued, MNULL, MNULL);
        pmbuf_aggr->flags |= MLAN_BUF_FLAG_REQUEUED_PKT;
        pmadapter->callbacks.moal_spin_unlock(pmadapter->pmoal_handle,
                                              priv->wmm.ra_list_spinlock);
        PRINTM(MINFO, "MLAN_STATUS_RESOURCE is returned\n");
        pmbuf_aggr->status_code = MLAN_ERROR_PKT_INVALID;
        break;
    case MLAN_STATUS_FAILURE:
        pmadapter->data_sent = MFALSE;
        PRINTM(MERROR, "Error: host_to_card failed: 0x%X\n", ret);
        pmbuf_aggr->status_code = MLAN_ERROR_DATA_TX_FAIL;
        pmadapter->dbg.num_tx_host_to_card_failure++;
        wlan_write_data_complete(pmadapter, pmbuf_aggr, ret);
        goto exit;
    case MLAN_STATUS_PENDING:
        pmadapter->data_sent = MFALSE;
        break;
    case MLAN_STATUS_SUCCESS:
        wlan_write_data_complete(pmadapter, pmbuf_aggr, ret);
        break;
    default:
        break;
    }
    if (ret != MLAN_STATUS_RESOURCE) {
        pmadapter->callbacks.moal_spin_lock(pmadapter->pmoal_handle,
                                            priv->wmm.ra_list_spinlock);
        if (wlan_is_ralist_valid(priv, pra_list, ptrindex)) {
            priv->wmm.packets_out[ptrindex]++;
            priv->wmm.tid_tbl_ptr[ptrindex].ra_list_curr = pra_list;
        }
        pmadapter->bssprio_tbl[priv->bss_priority].bssprio_cur =
            pmadapter->bssprio_tbl[priv->bss_priority].bssprio_cur->pnext;
        pmadapter->callbacks.moal_spin_unlock(pmadapter->pmoal_handle,
                                              priv->wmm.ra_list_spinlock);
    }
    PRINTM_GET_SYS_TIME(MDATA, &sec, &usec);
    PRINTM_NETINTF(MDATA, priv);
    PRINTM(MDATA, "%lu.%06lu : Data => FW\n", sec, usec);

  exit:
    LEAVE();
    return (pkt_size + headroom);
}
#endif /* CONFIG_MLAN_WMSDK */
