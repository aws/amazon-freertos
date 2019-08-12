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
 * From FreeBSD 2.2.7: Fundamental constants relating to ethernet.
 *
 * $Id: ethernet.h 473241 2014-04-28 19:15:49Z peterwu $
 */

#ifndef _NET_ETHERNET_H_	/* use native BSD ethernet.h when available */
#define _NET_ETHERNET_H_

#include "wwd_wlioctl.h"

#ifndef _TYPEDEFS_H_
#include "typedefs.h"
#endif

/* This marks the start of a packed structure section. */
#include <packed_section_start.h>


/*
 * The number of bytes in an ethernet (MAC) address.
 */
#define	ETHER_ADDR_LEN		6

/*
 * The number of bytes in the type field.
 */
#define	ETHER_TYPE_LEN		2

/*
 * The number of bytes in the trailing CRC field.
 */
#define	ETHER_CRC_LEN		4

/*
 * The length of the combined header.
 */
#define	ETHER_HDR_LEN		(ETHER_ADDR_LEN * 2 + ETHER_TYPE_LEN)

/*
 * The minimum packet length.
 */
#define	ETHER_MIN_LEN		64

/*
 * The minimum packet user data length.
 */
#define	ETHER_MIN_DATA		46

/*
 * The maximum packet length.
 */
#define	ETHER_MAX_LEN		1518

/*
 * The maximum packet user data length.
 */
#define	ETHER_MAX_DATA		1500

/* ether types */
#define ETHER_TYPE_MIN		0x0600		/* Anything less than MIN is a length */
#define	ETHER_TYPE_IP		0x0800		/* IP */
#define ETHER_TYPE_ARP		0x0806		/* ARP */
#define ETHER_TYPE_8021Q	0x8100		/* 802.1Q */
#define	ETHER_TYPE_IPV6		0x86dd		/* IPv6 */
#define	ETHER_TYPE_BRCM		0x886c		/* Broadcom Corp. */
#define	ETHER_TYPE_802_1X	0x888e		/* 802.1x */
#ifdef PLC
#define	ETHER_TYPE_88E1		0x88e1		/* GIGLE */
#define	ETHER_TYPE_8912		0x8912		/* GIGLE */
#define ETHER_TYPE_GIGLED	0xffff		/* GIGLE */
#endif /* PLC */
#define	ETHER_TYPE_802_1X_PREAUTH 0x88c7	/* 802.1x preauthentication */
#define ETHER_TYPE_WAI		0x88b4		/* WAI */
#define ETHER_TYPE_89_0D	0x890d		/* 89-0d frame for TDLS */

#define ETHER_TYPE_PPP_SES	0x8864		/* PPPoE Session */

#define ETHER_TYPE_IAPP_L2_UPDATE	0x6	/* IAPP L2 update frame */

/* Broadcom subtype follows ethertype;  First 2 bytes are reserved; Next 2 are subtype; */
#define	ETHER_BRCM_SUBTYPE_LEN	4	/* Broadcom 4 byte subtype */

/* ether header */
#define ETHER_DEST_OFFSET	(0 * ETHER_ADDR_LEN)	/* dest address offset */
#define ETHER_SRC_OFFSET	(1 * ETHER_ADDR_LEN)	/* src address offset */
#define ETHER_TYPE_OFFSET	(2 * ETHER_ADDR_LEN)	/* ether type offset */

/*
 * A macro to validate a length with
 */
#define	ETHER_IS_VALID_LEN(foo)	\
	((foo) >= ETHER_MIN_LEN && (foo) <= ETHER_MAX_LEN)

#define ETHER_FILL_MCAST_ADDR_FROM_IP(ea, mgrp_ip) {		\
		((uint8 *)ea)[0] = 0x01;			\
		((uint8 *)ea)[1] = 0x00;			\
		((uint8 *)ea)[2] = 0x5e;			\
		((uint8 *)ea)[3] = ((mgrp_ip) >> 16) & 0x7f;	\
		((uint8 *)ea)[4] = ((mgrp_ip) >>  8) & 0xff;	\
		((uint8 *)ea)[5] = ((mgrp_ip) >>  0) & 0xff;	\
}

#ifndef __INCif_etherh /* Quick and ugly hack for VxWorks */
/*
 * Structure of a 10Mb/s Ethernet header.
 */
BWL_PRE_PACKED_STRUCT struct ether_header {
	uint8	ether_dhost[ETHER_ADDR_LEN];
	uint8	ether_shost[ETHER_ADDR_LEN];
	uint16	ether_type;
} BWL_POST_PACKED_STRUCT;

#if 0 /* WICED - This is in wwd_wlioctl */
/*
 * Structure of a 48-bit Ethernet address.
 */
BWL_PRE_PACKED_STRUCT struct	ether_addr {
	uint8 octet[ETHER_ADDR_LEN];
} BWL_POST_PACKED_STRUCT;
#endif /* if 0  WICED */

#elif defined(VX_BSD4_3) && VX_BSD4_3
/*
 * Structure of a 48-bit Ethernet address.
 */
BWL_PRE_PACKED_STRUCT struct	ether_addr {
	uint8 octet[ETHER_ADDR_LEN];
} BWL_POST_PACKED_STRUCT;
#endif	/* !__INCif_etherh Quick and ugly hack for VxWorks */

/*
 * Takes a pointer, set, test, clear, toggle locally admininistered
 * address bit in the 48-bit Ethernet address.
 */
#define ETHER_SET_LOCALADDR(ea)	(((uint8 *)(ea))[0] = (((uint8 *)(ea))[0] | 2))
#define ETHER_IS_LOCALADDR(ea) 	(((uint8 *)(ea))[0] & 2)
#define ETHER_CLR_LOCALADDR(ea)	(((uint8 *)(ea))[0] = (((uint8 *)(ea))[0] & 0xfd))
#define ETHER_TOGGLE_LOCALADDR(ea)	(((uint8 *)(ea))[0] = (((uint8 *)(ea))[0] ^ 2))

/* Takes a pointer, marks unicast address bit in the MAC address */
#define ETHER_SET_UNICAST(ea)	(((uint8 *)(ea))[0] = (((uint8 *)(ea))[0] & ~1))

/*
 * Takes a pointer, returns true if a 48-bit multicast address
 * (including broadcast, since it is all ones)
 */
#define ETHER_ISMULTI(ea) (((const uint8 *)(ea))[0] & 1)


/* compare two ethernet addresses - assumes the pointers can be referenced as shorts */
#define eacmp(a, b)	((((const uint16 *)(a))[0] ^ ((const uint16 *)(b))[0]) | \
	                 (((const uint16 *)(a))[1] ^ ((const uint16 *)(b))[1]) | \
	                 (((const uint16 *)(a))[2] ^ ((const uint16 *)(b))[2]))

#define	ether_cmp(a, b)	eacmp(a, b)

/* copy an ethernet address - assumes the pointers can be referenced as shorts */
#define eacopy(s, d) \
do { \
	((uint16 *)(d))[0] = ((const uint16 *)(s))[0]; \
	((uint16 *)(d))[1] = ((const uint16 *)(s))[1]; \
	((uint16 *)(d))[2] = ((const uint16 *)(s))[2]; \
} while (0)

#define	ether_copy(s, d) eacopy(s, d)

/* Copy an ethernet address in reverse order */
#define	ether_rcopy(s, d) \
do { \
	((uint16 *)(d))[2] = ((uint16 *)(s))[2]; \
	((uint16 *)(d))[1] = ((uint16 *)(s))[1]; \
	((uint16 *)(d))[0] = ((uint16 *)(s))[0]; \
} while (0)

/* Copy 14B ethernet header: 32bit aligned source and destination. */
#define ehcopy32(s, d) \
do { \
	((uint32 *)(d))[0] = ((const uint32 *)(s))[0]; \
	((uint32 *)(d))[1] = ((const uint32 *)(s))[1]; \
	((uint32 *)(d))[2] = ((const uint32 *)(s))[2]; \
	((uint16 *)(d))[6] = ((const uint16 *)(s))[6]; \
} while (0)

#ifdef DONGLEBUILD

/* Dongles use bcmutils functions instead of macros.
 * Possibly slower but saves over 800 bytes off THUMB dongle image.
 */

extern const struct ether_addr ether_bcast;
extern const struct ether_addr ether_null;
extern const struct ether_addr ether_ipv6_mcast;

extern int ether_isbcast(const void *ea);
extern int ether_isnulladdr(const void *ea);

#define ETHER_ISBCAST(ea)	ether_isbcast(ea)
#define ETHER_ISNULLADDR(ea)	ether_isnulladdr(ea)

#else /* !DONGLEBUILD */

static const struct ether_addr ether_bcast = {{255, 255, 255, 255, 255, 255}};
static const struct ether_addr ether_null = {{0, 0, 0, 0, 0, 0}};
static const struct ether_addr ether_ipv6_mcast = {{0x33, 0x33, 0x00, 0x00, 0x00, 0x01}};

#define ETHER_ISBCAST(ea)	((((const uint8 *)(ea))[0] &		\
	                          ((const uint8 *)(ea))[1] &		\
				  ((const uint8 *)(ea))[2] &		\
				  ((const uint8 *)(ea))[3] &		\
				  ((const uint8 *)(ea))[4] &		\
				  ((const uint8 *)(ea))[5]) == 0xff)
#define ETHER_ISNULLADDR(ea)	((((const uint8 *)(ea))[0] |		\
				  ((const uint8 *)(ea))[1] |		\
				  ((const uint8 *)(ea))[2] |		\
				  ((const uint8 *)(ea))[3] |		\
				  ((const uint8 *)(ea))[4] |		\
				  ((const uint8 *)(ea))[5]) == 0)

#endif /* !DONGLEBUILD */

#define ETHER_ISNULLDEST(da)	((((const uint16 *)(da))[0] |           \
				  ((const uint16 *)(da))[1] |           \
				  ((const uint16 *)(da))[2]) == 0)
#define ETHER_ISNULLSRC(sa)	ETHER_ISNULLDEST(sa)

#define ETHER_MOVE_HDR(d, s) \
do { \
	struct ether_header t; \
	t = *(struct ether_header *)(s); \
	*(struct ether_header *)(d) = t; \
} while (0)

#define  ETHER_ISUCAST(ea) ((((uint8 *)(ea))[0] & 0x01) == 0)

/* This marks the end of a packed structure section. */
#include <packed_section_end.h>

#endif /* _NET_ETHERNET_H_ */
