/** @file mlan_uap.h
 *
 *  @brief This file contains related macros, enum, and struct
 *  of uap functionalities
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

/********************************************************
Change log:
    02/05/2009: initial version
********************************************************/

#ifndef _MLAN_UAP_H_
#define _MLAN_UAP_H_

#ifdef BIG_ENDIAN_SUPPORT
/** Convert TxPD to little endian format from CPU format */
#define uap_endian_convert_TxPD(x)                                          \
    {                                                                   \
        (x)->tx_pkt_length = wlan_cpu_to_le16((x)->tx_pkt_length);      \
        (x)->tx_pkt_offset = wlan_cpu_to_le16((x)->tx_pkt_offset);      \
        (x)->tx_pkt_type = wlan_cpu_to_le16((x)->tx_pkt_type);      \
        (x)->tx_control = wlan_cpu_to_le32((x)->tx_control);      \
    }

/** Convert RxPD from little endian format to CPU format */
#define uap_endian_convert_RxPD(x)                                          \
    {                                                                   \
        (x)->rx_pkt_length = wlan_le16_to_cpu((x)->rx_pkt_length);      \
        (x)->rx_pkt_offset = wlan_le16_to_cpu((x)->rx_pkt_offset);      \
        (x)->rx_pkt_type = wlan_le16_to_cpu((x)->rx_pkt_type);      \
        (x)->seq_num = wlan_le16_to_cpu((x)->seq_num);              \
    }
#else
/** Convert TxPD to little endian format from CPU format */
#define uap_endian_convert_TxPD(x)  do {} while (0)
/** Convert RxPD from little endian format to CPU format */
#define uap_endian_convert_RxPD(x)  do {} while (0)
#endif /* BIG_ENDIAN_SUPPORT */

/** Band config 5GHz */
#define UAP_BAND_CONFIG_5GHZ  0x01

/** Packet forwarding to be done by FW or host */
#define PKT_FWD_FW_BIT  0x01
/** Intra-BSS broadcast packet forwarding allow bit */
#define PKT_FWD_INTRA_BCAST 0x02
/** Intra-BSS unicast packet forwarding allow bit */
#define PKT_FWD_INTRA_UCAST 0x04
/** Inter-BSS unicast packet forwarding allow bit */
#define PKT_FWD_INTER_UCAST 0x08
/** Intra-BSS unicast packet */
#define PKT_INTRA_UCAST     0x01
/** Inter-BSS unicast packet */
#define PKT_INTER_UCAST     0x02
/** Enable Host PKT forwarding */
#define PKT_FWD_ENABLE_BIT  0x01

mlan_status wlan_uap_get_channel(IN pmlan_private pmpriv);

mlan_status wlan_uap_set_channel(IN pmlan_private pmpriv,
                                 IN t_u8 uap_band_cfg, IN t_u8 channel);

mlan_status wlan_uap_get_beacon_dtim(IN pmlan_private pmpriv);

mlan_status wlan_ops_uap_ioctl(t_void * adapter, pmlan_ioctl_req pioctl_req);

mlan_status wlan_ops_uap_prepare_cmd(IN t_void * priv,
                                     IN t_u16 cmd_no,
                                     IN t_u16 cmd_action,
                                     IN t_u32 cmd_oid,
                                     IN t_void * pioctl_buf,
                                     IN t_void * pdata_buf,
                                     IN t_void * pcmd_buf);

mlan_status wlan_ops_uap_process_cmdresp(IN t_void * priv,
                                         IN t_u16 cmdresp_no,
                                         IN t_void * pcmd_buf,
                                         IN t_void * pioctl);

mlan_status wlan_ops_uap_process_rx_packet(IN t_void * adapter,
                                           IN pmlan_buffer pmbuf);

mlan_status wlan_ops_uap_process_event(IN t_void * priv);

t_void *wlan_ops_uap_process_txpd(IN t_void * priv, IN pmlan_buffer pmbuf);

mlan_status wlan_ops_uap_init_cmd(IN t_void * priv, IN t_u8 first_bss);

#endif /* _MLAN_UAP_H_ */
