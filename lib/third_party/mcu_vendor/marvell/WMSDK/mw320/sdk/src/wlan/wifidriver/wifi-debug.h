/** @file wifi-debug.h
*
*  @brief WLAN Debug Header
*
*  (C) Copyright 2008-2018 Marvell International Ltd. All Rights Reserved
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


#ifndef __WIFI_DEBUG_H__
#define __WIFI_DEBUG_H__

#include <mlan_wmsdk.h>
#include <wmlog.h>

#define wscan_e(...)                            \
	wmlog_e("wscan", ##__VA_ARGS__)
#define wscan_w(...)                            \
	wmlog_w("wscan", ##__VA_ARGS__)
#ifdef CONFIG_WIFI_SCAN_DEBUG
#define wscan_d(...)                            \
	wmlog("wscan", ##__VA_ARGS__)
#else
#define wscan_d(...)
#endif /* ! CONFIG_WIFI_SCAN_DEBUG */

#define wifi_e(...)				\
	wmlog_e("wifi", ##__VA_ARGS__)
#define wifi_w(...)				\
	wmlog_w("wifi", ##__VA_ARGS__)

#ifdef CONFIG_WIFI_DEBUG
#define wifi_d(...)				\
	wmlog("wifi", ##__VA_ARGS__)
#else
#define wifi_d(...)
#endif /* ! CONFIG_WIFI_DEBUG */

#define ampdu_e(...)				\
	wmlog_e("ampdu", ##__VA_ARGS__)
#define ampdu_w(...)				\
	wmlog_w("ampdu", ##__VA_ARGS__)

#ifdef CONFIG_WIFI_AMPDU_DEBUG
#define ampdu_d(...)				\
	wmlog("ampdu", ##__VA_ARGS__)
#else
#define ampdu_d(...)
#endif /* ! CONFIG_WIFI_AMPDU_DEBUG */

#define w_tmr_e(...)				\
	wmlog_e("w_tmr", ##__VA_ARGS__)
#define w_tmr_w(...)				\
	wmlog_w("w_tmr", ##__VA_ARGS__)

#ifdef CONFIG_WIFI_TIMER_DEBUG
#define w_tmr_d(...)				\
	wmlog("w_tmr", ##__VA_ARGS__)
#else
#define w_tmr_d(...)
#endif /* CONFIG_WIFI_TIMER_DEBUG */

#define w_mem_e(...)				\
	wmlog_e("w_mem", ##__VA_ARGS__)
#define w_mem_w(...)				\
	wmlog_w("w_mem", ##__VA_ARGS__)

#ifdef CONFIG_WIFI_MEM_DEBUG
#define w_mem_d(...)				\
	wmlog("w_mem", ##__VA_ARGS__)
#else
#define w_mem_d(...)
#endif /* ! CONFIG_WIFI_MEM_DEBUG */

#define w_pkt_e(...)				\
	wmlog_e("w_pkt", ##__VA_ARGS__)
#define w_pkt_w(...)				\
	wmlog_w("w_pkt", ##__VA_ARGS__)

#ifdef CONFIG_WIFI_PKT_DEBUG
#define w_pkt_d(...)				\
	wmlog("w_pkt", ##__VA_ARGS__)
#else
#define w_pkt_d(...)
#endif /* ! CONFIG_WIFI_PKT_DEBUG */

#define wevt_e(...)				\
	wmlog_e("wevt", ##__VA_ARGS__)
#define wevt_w(...)				\
	wmlog_w("wevt", ##__VA_ARGS__)

#ifdef CONFIG_WIFI_EVENTS_DEBUG
#define wevt_d(...)				\
	wmlog("wevt", ##__VA_ARGS__)
#else
#define wevt_d(...)
#endif /* ! CONFIG_WIFI_EVENTS_DEBUG */

#define wcmdr_e(...)				\
	wmlog_e("wcmdr", ##__VA_ARGS__)
#define wcmdr_w(...)				\
	wmlog_w("wcmdr", ##__VA_ARGS__)

#ifdef CONFIG_WIFI_CMD_RESP_DEBUG
#define wcmdr_d(...)				\
	wmlog("wcmdr", ##__VA_ARGS__)
#else
#define wcmdr_d(...)
#endif /* ! CONFIG_WIFI_CMD_RESP_DEBUG */

#define wuap_e(...)				\
	wmlog_e("uap", ##__VA_ARGS__)
#define wuap_w(...)				\
	wmlog_w("uap", ##__VA_ARGS__)

#ifdef CONFIG_WIFI_UAP_DEBUG
#define wuap_d(...)				\
	wmlog("uap", ##__VA_ARGS__)
#else
#define wuap_d(...)
#endif /* ! CONFIG_WIFI_UAP_DEBUG */


void wifi_show_assoc_fail_reason(int status);

void dump_mac_addr(const char *msg, unsigned char *addr);
void dump_packet_header(const HostCmd_DS_COMMAND *cmd);
void dump_addba_req_rsp_packet(const HostCmd_DS_COMMAND *cmd);
void dump_htcap_info(const MrvlIETypes_HTCap_t *htcap);
void dump_ht_info(const MrvlIETypes_HTInfo_t *htinfo);

#endif /* __WIFI_DEBUG_H__ */
