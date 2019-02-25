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
 * Fundamental constants relating to TCP Protocol
 *
 * $Id: bcmtcp.h 458523 2014-02-27 02:26:47Z shinuk $
 */

#ifndef _bcmtcp_h_
#define _bcmtcp_h_

#ifndef _TYPEDEFS_H_
#include <typedefs.h>
#endif

/* This marks the start of a packed structure section. */
#include <packed_section_start.h>


#define TCP_SRC_PORT_OFFSET	0	/* TCP source port offset */
#define TCP_DEST_PORT_OFFSET	2	/* TCP dest port offset */
#define TCP_SEQ_NUM_OFFSET	4	/* TCP sequence number offset */
#define TCP_ACK_NUM_OFFSET	8	/* TCP acknowledgement number offset */
#define TCP_HLEN_OFFSET		12	/* HLEN and reserved bits offset */
#define TCP_FLAGS_OFFSET	13	/* FLAGS and reserved bits offset */
#define TCP_CHKSUM_OFFSET	16	/* TCP body checksum offset */

#define TCP_PORT_LEN		2	/* TCP port field length */

/* 8bit TCP flag field */
#define TCP_FLAG_URG            0x20
#define TCP_FLAG_ACK            0x10
#define TCP_FLAG_PSH            0x08
#define TCP_FLAG_RST            0x04
#define TCP_FLAG_SYN            0x02
#define TCP_FLAG_FIN            0x01

#define TCP_HLEN_MASK           0xf000
#define TCP_HLEN_SHIFT          12

/* These fields are stored in network order */
BWL_PRE_PACKED_STRUCT struct bcmtcp_hdr
{
	uint16	src_port;	/* Source Port Address */
	uint16	dst_port;	/* Destination Port Address */
	uint32	seq_num;	/* TCP Sequence Number */
	uint32	ack_num;	/* TCP Sequence Number */
	uint16	hdrlen_rsvd_flags;	/* Header length, reserved bits and flags */
	uint16	tcpwin;		/* TCP window */
	uint16	chksum;		/* Segment checksum with pseudoheader */
	uint16	urg_ptr;	/* Points to seq-num of byte following urg data */
} BWL_POST_PACKED_STRUCT;

#define TCP_MIN_HEADER_LEN 20

#define TCP_HDRLEN_MASK 0xf0
#define TCP_HDRLEN_SHIFT 4
#define TCP_HDRLEN(hdrlen) (((hdrlen) & TCP_HDRLEN_MASK) >> TCP_HDRLEN_SHIFT)

#define TCP_FLAGS_MASK  0x1f
#define TCP_FLAGS(hdrlen) ((hdrlen) & TCP_FLAGS_MASK)

/* This marks the end of a packed structure section. */
#include <packed_section_end.h>

/* To address round up by 32bit. */
#define IS_TCPSEQ_GE(a, b) ((a - b) < NBITVAL(31))		/* a >= b */
#define IS_TCPSEQ_LE(a, b) ((b - a) < NBITVAL(31))		/* a =< b */
#define IS_TCPSEQ_GT(a, b) !IS_TCPSEQ_LE(a, b)		/* a > b */
#define IS_TCPSEQ_LT(a, b) !IS_TCPSEQ_GE(a, b)		/* a < b */

#endif	/* #ifndef _bcmtcp_h_ */
