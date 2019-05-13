/*
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

/*! \file wmstats.h
 *
 * \brief Wireless Microcontroller statistics
 */

#ifndef _WMSTATS_H_
#define _WMSTATS_H_

typedef enum
{
	CONN_STATS,
	DHCP_STATS,
	HTTP_CLIENT_STATS,
	CLOUD_CUMUL_STATS,
	CLOUD_STATS,
	HTTPD_STATS,
	NET_STATS,
	IP_ADDR,
	TIME,
	PROV_TYPE,
	WD_STATUS,
} wm_stat_type_t;

/*! Define return values */
typedef enum
{
	/** */
	REASON_ADV_SETTINGS_DONE,
	/** */
	REASON_NW_ADD_FAIL,
	/** */
	REASON_NW_FAILURE,
	/** */
	REASON_SWITCH_TO_PROV,
	/** */
	REASON_AUTH_FAILURES,
	/** */
	REASON_USER_REBOOT,
	/** */
	REASON_CLOUD_WATCHDOG,
	/** */
	REASON_CLOUD_FW_UPDATE,
	/** */
	REASON_WLAN_FW_UNRESPONSIVE,
} wm_reboot_reason_t;

struct wm_stats {
	unsigned short     wm_lloss;
	/** The number of connection attempts. 
	 * wm_conn_att == wm_conn_succ + wm_conn_fail
	 */
	unsigned short     wm_conn_att;
	/** The connection success indicates successful association+authentication
	    attempt. This does NOT include the address configuration step */
	unsigned short     wm_conn_succ;
	/** This should be split up into multiple types of failures 
	 * wm_conn_fail = wm_auth_fail + wm_nwnt_found + others
	 */
	unsigned short     wm_conn_fail;
	unsigned short     wm_auth_fail;
	unsigned short     wm_nwnt_found;
	/** DHCP Address assignment */
	unsigned short     wm_dhcp_succ;
	unsigned short     wm_dhcp_fail;
	/** DHCP Lease renewal */
	unsigned short     wm_leas_succ;
	unsigned short     wm_leas_fail;
	/** 1 if DHCP */
	char               wm_addr_type;
	unsigned int       wm_dhcp_req_cnt;
	/** HTTP Client */
	unsigned short     wm_ht_dns_fail;
	unsigned short     wm_ht_sock_fail;
	unsigned short     wm_ht_conn_no_route;
	unsigned short     wm_ht_conn_timeout;
	unsigned short     wm_ht_conn_reset;
	unsigned short     wm_ht_conn_other;
	/** Cloud Stats */
	unsigned long      wm_cl_post_succ;
	unsigned long      wm_cl_post_fail;
	unsigned long	   wm_cl_total;
	unsigned int	   wm_cl_cum_total;
#ifdef CONFIG_ENABLE_HTTPD_STATS
	/** HTTP Stats */
	/* Note: wm_hd_file is always included in hd_wsgi_call count */
	unsigned short     wm_hd_wsgi_call;
	unsigned short     wm_hd_file;
	httpd_useragent_t  wm_hd_useragent;
	unsigned int	   wm_hd_time;
#endif /* CONFIG_ENABLE_HTTPD_STATS */
	/** Provisioning type */
	/* Tells whether the module was in NORMAL mode, PROVISIONING mode or
	 * TIMED PROVISIONING mode immediately after rebooting*/
	unsigned short	   wm_prov_type;

	/** Reboot reason of the device */
	wm_reboot_reason_t reboot_reason;
};

extern struct wm_stats g_wm_stats;

/** Note: This should be in sync with definition in firmware file
 * tcpip_custom.h.
 * */

#endif /* ! _WMSTATS_H_ */
