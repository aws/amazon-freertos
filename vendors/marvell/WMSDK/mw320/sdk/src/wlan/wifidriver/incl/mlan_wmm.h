/** @file mlan_wmm.h
 *
 *  @brief This file contains related macros, enum, and struct
 *  of wmm functionalities
 *
 *  (C) Copyright 2008-2018 Marvell International Ltd. All Rights Reserved
 *
 *  MARVELL CONFIDENTIAL
 *  The source code contained or described herein and all documents related to
 *  the source code ("Material") are owned by Marvell International Ltd or its
 *  suppliers or licensors. Title to the Material remains with Marvell International Ltd
 *  or its suppliers and licensors. The Material contains trade secrets and
 *  proprietary and confidential information of Marvell or its suppliers and
 *  licensors. The Material is protected by worldwide copyright and trade secret
 *  laws and treaty provisions. No part of the Material may be used, copied,
 *  reproduced, modified, published, uploaded, posted, transmitted, distributed,
 *  or disclosed in any way without Marvell's prior express written permission.
 * 
 *  No license under any patent, copyright, trade secret or other intellectual
 *  property right is granted to or conferred upon you by disclosure or delivery
 *  of the Materials, either expressly, by implication, inducement, estoppel or
 *  otherwise. Any license under such intellectual property rights must be
 *  express and approved by Marvell in writing.
 *
 */

/****************************************************
Change log:
    10/24/2008: initial version
****************************************************/

#ifndef _MLAN_WMM_H_
#define _MLAN_WMM_H_

/**
 *  @brief This function gets the TID
 *  
 *  @param pmadapter    A pointer to mlan_adapter structure
 *  @param ptr          A pointer to RA list table
 *
 *  @return 	    TID
 */
static INLINE int
wlan_get_tid(pmlan_adapter pmadapter, raListTbl * ptr)
{
    pmlan_buffer mbuf;

    ENTER();
    mbuf =
        (pmlan_buffer) util_peek_list(pmadapter->pmoal_handle, &ptr->buf_head,
                                      MNULL, MNULL);
    LEAVE();

    return mbuf->priority;
}

/**
 *  @brief This function gets the length of a list
 *  
 *  @param pmadapter    A pointer to mlan_adapter structure
 *  @param head         A pointer to mlan_list_head
 *
 *  @return 	    Length of list
 */
static INLINE int
wlan_wmm_list_len(pmlan_adapter pmadapter, pmlan_list_head head)
{
    pmlan_linked_list pos;
    int count = 0;

    ENTER();

    pos = head->pnext;

    while (pos != (pmlan_linked_list) head) {
        ++count;
        pos = pos->pnext;
    }

    LEAVE();
    return count;
}

/** Add buffer to WMM Tx queue */
void wlan_wmm_add_buf_txqueue(pmlan_adapter pmadapter, pmlan_buffer pmbuf);
/** Add to RA list */
void wlan_ralist_add(mlan_private * priv, t_u8 * ra);
/** Update the RA list */
int wlan_ralist_update(mlan_private * priv, t_u8 * old_ra, t_u8 * new_ra);

/** WMM status change command handler */
mlan_status wlan_cmd_wmm_status_change(pmlan_private priv);
/** Check if WMM lists are empty */
int wlan_wmm_lists_empty(pmlan_adapter pmadapter);
/** Process WMM transmission */
t_void wlan_wmm_process_tx(pmlan_adapter pmadapter);
/** Test to see if the ralist ptr is valid */
int wlan_is_ralist_valid(mlan_private * priv, raListTbl * ra_list, int tid);

raListTbl *wlan_wmm_get_ralist_node(pmlan_private priv, t_u8 tid,
                                    t_u8 * ra_addr);
t_u8 wlan_get_random_ba_threshold(pmlan_adapter pmadapter);

/** Compute driver packet delay */
t_u8 wlan_wmm_compute_driver_packet_delay(pmlan_private priv,
                                          const pmlan_buffer pmbuf);
/** Initialize WMM */
t_void wlan_wmm_init(pmlan_adapter pmadapter);
/** Setup WMM queues */
extern void wlan_wmm_setup_queues(pmlan_private priv);
/* Setup default queues */
void wlan_wmm_default_queue_priorities(pmlan_private priv);

#ifdef STA_SUPPORT
/** Process WMM association request */
extern t_u32 wlan_wmm_process_association_req(pmlan_private priv,
                                              t_u8 ** ppAssocBuf,
                                              IEEEtypes_WmmParameter_t * pWmmIE,
                                              IEEEtypes_HTCap_t * pHTCap);
#endif /* STA_SUPPORT */

/** setup wmm queue priorities */
void wlan_wmm_setup_queue_priorities(pmlan_private priv,
                                     IEEEtypes_WmmParameter_t * wmm_ie);

/** Downgrade WMM priority queue */
void wlan_wmm_setup_ac_downgrade(pmlan_private priv);
/** select WMM queue */
t_u8 wlan_wmm_select_queue(mlan_private * pmpriv, t_u8 tid);
#ifdef UAP_SUPPORT
t_void wlan_wmm_delete_peer_ralist(pmlan_private priv, t_u8 * mac);
#endif

#ifdef STA_SUPPORT
/* 
 *  Functions used in the cmd handling routine
 */
/** WMM ADDTS request command handler */
extern mlan_status wlan_cmd_wmm_addts_req(IN pmlan_private pmpriv,
                                          OUT HostCmd_DS_COMMAND * cmd,
                                          IN t_void * pdata_buf);
/** WMM DELTS request command handler */
extern mlan_status wlan_cmd_wmm_delts_req(IN pmlan_private pmpriv,
                                          OUT HostCmd_DS_COMMAND * cmd,
                                          IN t_void * pdata_buf);
/** WMM QUEUE_CONFIG command handler */
extern mlan_status wlan_cmd_wmm_queue_config(IN pmlan_private pmpriv,
                                             OUT HostCmd_DS_COMMAND * cmd,
                                             IN t_void * pdata_buf);
/** WMM QUEUE_STATS command handler */
extern mlan_status wlan_cmd_wmm_queue_stats(IN pmlan_private pmpriv,
                                            OUT HostCmd_DS_COMMAND * cmd,
                                            IN t_void * pdata_buf);
/** WMM TS_STATUS command handler */
extern mlan_status wlan_cmd_wmm_ts_status(IN pmlan_private pmpriv,
                                          OUT HostCmd_DS_COMMAND * cmd,
                                          IN t_void * pdata_buf);

/* 
 *  Functions used in the cmdresp handling routine
 */
/** WMM get status command response handler */
extern mlan_status wlan_ret_wmm_get_status(IN pmlan_private priv,
                                           IN t_u8 * ptlv, IN int resp_len);
/** WMM ADDTS request command response handler */
extern mlan_status wlan_ret_wmm_addts_req(IN pmlan_private pmpriv,
                                          const IN HostCmd_DS_COMMAND * resp,
                                          OUT mlan_ioctl_req * pioctl_buf);
/** WMM DELTS request command response handler */
extern mlan_status wlan_ret_wmm_delts_req(IN pmlan_private pmpriv,
                                          const IN HostCmd_DS_COMMAND * resp,
                                          OUT mlan_ioctl_req * pioctl_buf);
/** WMM QUEUE_CONFIG command response handler */
extern mlan_status wlan_ret_wmm_queue_config(IN pmlan_private pmpriv,
                                             const IN HostCmd_DS_COMMAND * resp,
                                             OUT mlan_ioctl_req * pioctl_buf);
/** WMM QUEUE_STATS command response handler */
extern mlan_status wlan_ret_wmm_queue_stats(IN pmlan_private pmpriv,
                                            const IN HostCmd_DS_COMMAND * resp,
                                            OUT mlan_ioctl_req * pioctl_buf);
/** WMM TS_STATUS command response handler */
extern mlan_status wlan_ret_wmm_ts_status(IN pmlan_private pmpriv,
                                          IN HostCmd_DS_COMMAND * resp,
                                          OUT mlan_ioctl_req * pioctl_buf);
#endif /* STA_SUPPORT */

#endif /* !_MLAN_WMM_H_ */
