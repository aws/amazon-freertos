/*
 * Copyright 2019, Cypress Semiconductor Corporation or a subsidiary of
 * Cypress Semiconductor Corporation. All Rights Reserved.
 * 
 * This software, associated documentation and materials ("Software")
 * is owned by Cypress Semiconductor Corporation,
 * or one of its subsidiaries ("Cypress") and is protected by and subject to
 * worldwide patent protection (United States and foreign),
 * United States copyright laws and international treaty provisions.
 * Therefore, you may use this Software only as provided in the license
 * agreement accompanying the software package from which you
 * obtained this Software ("EULA").
 * If no EULA applies, Cypress hereby grants you a personal, non-exclusive,
 * non-transferable license to copy, modify, and compile the Software
 * source code solely for use in connection with Cypress's
 * integrated circuit products. Any reproduction, modification, translation,
 * compilation, or representation of this Software except as specified
 * above is prohibited without the express written permission of Cypress.
 *
 * Disclaimer: THIS SOFTWARE IS PROVIDED AS-IS, WITH NO WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING, BUT NOT LIMITED TO, NONINFRINGEMENT, IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE. Cypress
 * reserves the right to make changes to the Software without notice. Cypress
 * does not assume any liability arising out of the application or use of the
 * Software or any product or circuit described in the Software. Cypress does
 * not authorize its products for use in any products where a malfunction or
 * failure of the Cypress product may reasonably be expected to result in
 * significant property damage, injury or death ("High Risk Product"). By
 * including Cypress's product in a High Risk Product, the manufacturer
 * of such system or application assumes all risk of such use and in doing
 * so agrees to indemnify Cypress against all liability.
 *
 * Fundamental constants relating to IP Protocol
 *
 * $Id: bcmproto.h 329791 2012-04-26 22:36:58Z rnuti $
 */

#ifndef _bcmproto_h_
#define _bcmproto_h_

#ifndef _TYPEDEFS_H_
#include <typedefs.h>
#endif

#include "eapol.h"
#include "802.3.h"
#include "vlan.h"
#include "bcmtcp.h"
/* copy from igsc.h */
#define IGMP_HLEN			8

enum frame_l2_hdr {
FRAME_L2_SNAP_H = 1,
FRAME_L2_SNAPVLAN_H,
FRAME_L2_ETH_H,
FRAME_L2_ETHVLAN_H,
FRAME_L2_ERROR,
};

enum frame_l3_hdr {
FRAME_L3_IP_H = 4,
FRAME_L3_IP6_H = 6,
FRAME_L3_ARP_H,
FRAME_L3_8021X_EAPOLKEY_H,
FRAME_L3_ERROR,
};

enum frame_l4_hdr {
FRAME_L4_ICMP_H = 1,
FRAME_L4_IGMP_H = 2,
FRAME_L4_TCP_H = 6,
FRAME_L4_UDP_H = 17,
FRAME_L4_ICMP6_H = 58,
FRAME_L4_ERROR,
};

typedef struct {
	uint8 *l2;
	uint8 l2_t;
	uint16 l2_len;
	uint8 *l3;
	uint8 l3_t;
	uint16 l3_len;
	uint8 *l4;
	uint8 l4_t;
	uint16 l4_len;
} frame_proto_t;

static const uint8 llc_snap_hdr[SNAP_HDR_LEN] = {0xaa, 0xaa, 0x03, 0x00, 0x00, 0x00};

/* Generic header parser function */
static INLINE int
hnd_frame_proto(uint8 *p, int plen, frame_proto_t *fp)
{
	struct dot3_mac_llc_snap_header *sh = (struct dot3_mac_llc_snap_header *)p;
	struct dot3_mac_llc_snapvlan_header *svh = (struct dot3_mac_llc_snapvlan_header *)p;
	struct ether_header *eh = (struct ether_header *)p;
	struct ethervlan_header *evh = (struct ethervlan_header *)p;
	uint16 type;
	uint16 len;

	if (p == NULL || plen <= 0)
		return BCME_ERROR;

	type  = ntoh16(eh->ether_type);

	bzero(fp, sizeof(frame_proto_t));

	/* L2 header/pointer check */
	fp->l2 = p;
	fp->l2_len = (uint16)plen;
	if (type < ETHER_TYPE_MIN) {
		if (bcmp(&sh->dsap, llc_snap_hdr, SNAP_HDR_LEN) == 0) {
			type = ntoh16(sh->type);
			if (type == ETHER_TYPE_8021Q) {
				fp->l2_t = FRAME_L2_SNAPVLAN_H;
				p += sizeof(struct dot3_mac_llc_snap_header);
				if ((plen -= sizeof(struct dot3_mac_llc_snap_header)) <= 0)
					return BCME_ERROR;
			}
			else {
				fp->l2_t = FRAME_L2_SNAP_H;
				type = ntoh16(svh->ether_type);
				p += sizeof(struct dot3_mac_llc_snapvlan_header);
				if ((plen -= sizeof(struct dot3_mac_llc_snapvlan_header)) <= 0)
					return BCME_ERROR;
			}
		}
		else
			return BCME_ERROR;
	}
	else {
		if (type == ETHER_TYPE_8021Q) {
			fp->l2_t = FRAME_L2_ETHVLAN_H;
			type = ntoh16(evh->ether_type);
			p += ETHERVLAN_HDR_LEN;
			if ((plen -= ETHERVLAN_HDR_LEN) <= 0)
				return BCME_ERROR;
		}
		else {
			fp->l2_t = FRAME_L2_ETH_H;
			p += ETHER_HDR_LEN;
			if ((plen -= ETHER_HDR_LEN) <= 0)
				return BCME_ERROR;
		}
	}
	/* L3 header/pointer check */
	fp->l3 = p;
	fp->l3_len = (uint16)plen;
	switch (type) {
	case ETHER_TYPE_ARP: {
		if ((plen -= ARP_DATA_LEN) < 0)
			return BCME_ERROR;

		fp->l3_t = FRAME_L3_ARP_H;
		/* no layer 4 protocol, return */
		return BCME_OK;
		break;
	}
	case ETHER_TYPE_IP: {
		struct ipv4_hdr *iph = (struct ipv4_hdr *)p;
		len = IPV4_HLEN(iph);

		if ((plen -= len) <= 0)
			return BCME_ERROR;

		if (IP_VER(iph) == IP_VER_4 && len >= IPV4_MIN_HEADER_LEN) {
			fp->l3_t = FRAME_L3_IP_H;
			type = IPV4_PROT(iph);
			p += len;
		}
		else /* not a valid ipv4 packet */
			return BCME_ERROR;
		break;
	}
	case ETHER_TYPE_IPV6: {
		struct ipv6_hdr *ip6h = (struct ipv6_hdr *)p;

		if ((plen -= IPV6_MIN_HLEN) <= 0)
			return BCME_ERROR;

		if (IP_VER(ip6h) == IP_VER_6) {
			fp->l3_t = FRAME_L3_IP6_H;
			type = IPV6_PROT(ip6h);
			p += IPV6_MIN_HLEN;
			if (IPV6_EXTHDR(type)) {
				uint8 proto = 0;
				int32 exth_len = ipv6_exthdr_len(p, &proto);
				if (exth_len < 0 || ((plen -= exth_len) <= 0))
					return BCME_ERROR;
				type = proto;
				p += exth_len;
			}
		}
		else /* not a valid ipv6 packet */
			return BCME_ERROR;
		break;
	}
	case ETHER_TYPE_802_1X: {
		eapol_hdr_t *eapolh = (eapol_hdr_t *)p;

		if ((plen -= EAPOL_HDR_LEN) <= 0)
			return BCME_ERROR;

		if (eapolh->type == EAPOL_KEY) {
			fp->l3_t = FRAME_L3_8021X_EAPOLKEY_H;
			return BCME_OK;
		}
		else /* not a valid ipv6 packet */
			return BCME_ERROR;

		break;
	}
	default:
		/* not interesting case */
		return BCME_ERROR;
		break;
	}

	/* L4 header/pointer check */
	fp->l4 = p;
	fp->l4_len = (uint16)plen;
	switch (type) {
	case IP_PROT_ICMP:
		fp->l4_t = FRAME_L4_ICMP_H;
		if ((plen -= sizeof(struct bcmicmp_hdr)) < 0)
			return BCME_ERROR;
		break;
	case IP_PROT_IGMP:
		fp->l4_t = FRAME_L4_IGMP_H;
		if ((plen -= IGMP_HLEN) < 0)
			return BCME_ERROR;
		break;
	case IP_PROT_TCP:
		fp->l4_t = FRAME_L4_TCP_H;
		if ((plen -= sizeof(struct bcmtcp_hdr)) < 0)
			return BCME_ERROR;
		break;
	case IP_PROT_UDP:
		fp->l4_t = FRAME_L4_UDP_H;
		if ((plen -= sizeof(struct bcmudp_hdr)) < 0)
			return BCME_ERROR;
		break;
	case IP_PROT_ICMP6:
		fp->l4_t = FRAME_L4_ICMP6_H;
		if ((plen -= sizeof(struct icmp6_hdr)) < 0)
			return BCME_ERROR;
		break;
	default:
		break;
	}

	return BCME_OK;
}

#define SNAP_HDR_LEN	6	/* 802.3 LLC/SNAP header length */

#define FRAME_DROP 0
#define FRAME_NOP 1
#define FRAME_TAKEN 2

#endif	/* _bcmproto_h_ */
