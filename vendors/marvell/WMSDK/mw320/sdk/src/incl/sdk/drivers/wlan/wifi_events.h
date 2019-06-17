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

/*! \file wifi_events.h
 * \brief Wi-Fi events
 */

#ifndef __WIFI_EVENTS_H__
#define __WIFI_EVENTS_H__

/** Wifi events */
enum wifi_event {
	WIFI_EVENT_UAP_STARTED = 0,
	WIFI_EVENT_UAP_CLIENT_ASSOC,
	WIFI_EVENT_UAP_CLIENT_DEAUTH,
	WIFI_EVENT_UAP_NET_ADDR_CONFIG,
	WIFI_EVENT_UAP_DTIM_PS,
	WIFI_EVENT_UAP_INACT_PS,
	WIFI_EVENT_UAP_STOPPED,
	WIFI_EVENT_UAP_LAST,
	/* All the uAP related events need to be above and STA related events
	   below */
	WIFI_EVENT_SCAN_RESULT,
	WIFI_EVENT_GET_HW_SPEC,
	WIFI_EVENT_ASSOCIATION,
	WIFI_EVENT_PMK,
	WIFI_EVENT_AUTHENTICATION,
	WIFI_EVENT_DISASSOCIATION,
	WIFI_EVENT_DEAUTHENTICATION,
	WIFI_EVENT_LINK_LOSS,
	WIFI_EVENT_NET_STA_ADDR_CONFIG,
	WIFI_EVENT_NET_INTERFACE_CONFIG,
	WIFI_EVENT_WEP_CONFIG,
	WIFI_EVENT_MAC_ADDR_CONFIG,
	WIFI_EVENT_NET_DHCP_CONFIG,
	WIFI_EVENT_SUPPLICANT_PMK,
	WIFI_EVENT_SLEEP,
	WIFI_EVENT_AWAKE,
	WIFI_EVENT_IEEE_PS,
	WIFI_EVENT_DEEP_SLEEP,
	WIFI_EVENT_PS_INVALID,
	WIFI_EVENT_HS_CONFIG,
	WIFI_EVENT_ERR_MULTICAST,
	WIFI_EVENT_ERR_UNICAST,
	/* Add Block Ack */
	WIFI_EVENT_11N_ADDBA,
	WIFI_EVENT_11N_BA_STREAM_TIMEOUT,
	WIFI_EVENT_11N_DELBA,
	WIFI_EVENT_11N_AGGR_CTRL,
	/* Channel switch */
	WIFI_EVENT_CHAN_SWITCH,
	/* IPv6 address state change */
	WIFI_EVENT_NET_IPV6_CONFIG,
	/* Auto link switch network */
	WIFI_EVENT_AUTOLINK_NETWORK_SWITCHED,
	WIFI_EVENT_LAST,
/* other events can be added after this, however this must
   be the last event in the wifi module */
};

enum wifi_event_reason {
	WIFI_EVENT_REASON_SUCCESS,
	WIFI_EVENT_REASON_TIMEOUT,
	WIFI_EVENT_REASON_FAILURE,
};

/** Network wireless BSS Type */
enum wlan_bss_type {
	WLAN_BSS_TYPE_STA = 0,
	WLAN_BSS_TYPE_UAP = 1,
	WLAN_BSS_TYPE_WIFIDIRECT = 2,
	WLAN_BSS_TYPE_ANY = 0xff,
};

/** Network wireless BSS Role */
enum wlan_bss_role {
	/** Infrastructure network. The system will act as a station connected
	 *  to an Access Point. */
	WLAN_BSS_ROLE_STA = 0,
	/** uAP (micro-AP) network.  The system will act as an uAP node to
	 * which other Wireless clients can connect. */
	WLAN_BSS_ROLE_UAP = 1,
	/** Either Infrastructure network or micro-AP network */
	WLAN_BSS_ROLE_ANY = 0xff,
};

/* fixme: Move this to wifi.h*/
/** This enum defines various wakeup events
 * for which wakeup will occur */
enum wifi_wakeup_event_t {
	/** Wakeup on broadcast  */
	WIFI_WAKE_ON_ALL_BROADCAST = 1,
	/** Wakeup on unicast  */
	WIFI_WAKE_ON_UNICAST = 1<<1,
	/** Wakeup on MAC event  */
	WIFI_WAKE_ON_MAC_EVENT = 1<<2,
	/** Wakeup on multicast  */
	WIFI_WAKE_ON_MULTICAST = 1<<3,
	/** Wakeup on ARP broadcast  */
	WIFI_WAKE_ON_ARP_BROADCAST = 1 << 4,
	/** Wakeup on receiving a management frame  */
	WIFI_WAKE_ON_MGMT_FRAME = 1<<6,
};


/* fixme : This enum should be mapped to enum of WLAN state.
 * This enum is used to check  power state on exit from PM4.
 * If WIFI is in PDN and reboot is caused due to PM4 exit
 * do firmware download on WIFI chip.
 * If WIFI is in DS and reboot is caused due to PM4 exit
 * skip firmware download on WIFI chip.
 * This enum is a replica of wlan_ps_mode in wlna.h
 * Replica will be removed later.
 *
 */

/** Enum for wifi card state */
enum wifi_state {
	/** Un-initialized state */
	WIFI_UNINIT = 0,
	/** Idle state */
	WIFI_INACTIVE,
	/** Active state*/
	WIFI_ACTIVE ,
	/** Deep sleep power save state */
	WIFI_DEEP_SLEEP,
	/** Power down power state */
	WIFI_PDN,
};

#endif  /*__WIFI_EVENTS_H__*/
