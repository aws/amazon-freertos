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
 * Fundamental constants relating to DHCP Protocol
 *
 * $Id: bcmdhcp.h 382883 2013-02-04 23:26:09Z xwei $
 */

#ifndef _bcmdhcp_h_
#define _bcmdhcp_h_

/* DHCP params */
#define DHCP_TYPE_OFFSET	0	/* DHCP type (request|reply) offset */
#define DHCP_TID_OFFSET		4	/* DHCP transition id offset */
#define DHCP_FLAGS_OFFSET	10	/* DHCP flags offset */
#define DHCP_CIADDR_OFFSET	12	/* DHCP client IP address offset */
#define DHCP_YIADDR_OFFSET	16	/* DHCP your IP address offset */
#define DHCP_GIADDR_OFFSET	24	/* DHCP relay agent IP address offset */
#define DHCP_CHADDR_OFFSET	28	/* DHCP client h/w address offset */
#define DHCP_OPT_OFFSET		236	/* DHCP options offset */

#define DHCP_OPT_MSGTYPE	53	/* DHCP message type */
#define DHCP_OPT_MSGTYPE_REQ	3
#define DHCP_OPT_MSGTYPE_ACK	5	/* DHCP message type - ACK */

#define DHCP_OPT_CODE_OFFSET	0	/* Option identifier */
#define DHCP_OPT_LEN_OFFSET	1	/* Option data length */
#define DHCP_OPT_DATA_OFFSET	2	/* Option data */

#define DHCP_OPT_CODE_CLIENTID	61	/* Option identifier */

#define DHCP_TYPE_REQUEST	1	/* DHCP request (discover|request) */
#define DHCP_TYPE_REPLY		2	/* DHCP reply (offset|ack) */

#define DHCP_PORT_SERVER	67	/* DHCP server UDP port */
#define DHCP_PORT_CLIENT	68	/* DHCP client UDP port */

#define DHCP_FLAG_BCAST	0x8000	/* DHCP broadcast flag */

#define DHCP_FLAGS_LEN	2	/* DHCP flags field length */

#define DHCP6_TYPE_SOLICIT	1	/* DHCP6 solicit */
#define DHCP6_TYPE_ADVERTISE	2	/* DHCP6 advertise */
#define DHCP6_TYPE_REQUEST	3	/* DHCP6 request */
#define DHCP6_TYPE_CONFIRM	4	/* DHCP6 confirm */
#define DHCP6_TYPE_RENEW	5	/* DHCP6 renew */
#define DHCP6_TYPE_REBIND	6	/* DHCP6 rebind */
#define DHCP6_TYPE_REPLY	7	/* DHCP6 reply */
#define DHCP6_TYPE_RELEASE	8	/* DHCP6 release */
#define DHCP6_TYPE_DECLINE	9	/* DHCP6 decline */
#define DHCP6_TYPE_RECONFIGURE	10	/* DHCP6 reconfigure */
#define DHCP6_TYPE_INFOREQ	11	/* DHCP6 information request */
#define DHCP6_TYPE_RELAYFWD	12	/* DHCP6 relay forward */
#define DHCP6_TYPE_RELAYREPLY	13	/* DHCP6 relay reply */

#define DHCP6_TYPE_OFFSET	0	/* DHCP6 type offset */

#define	DHCP6_MSG_OPT_OFFSET	4	/* Offset of options in client server messages */
#define	DHCP6_RELAY_OPT_OFFSET	34	/* Offset of options in relay messages */

#define	DHCP6_OPT_CODE_OFFSET	0	/* Option identifier */
#define	DHCP6_OPT_LEN_OFFSET	2	/* Option data length */
#define	DHCP6_OPT_DATA_OFFSET	4	/* Option data */

#define	DHCP6_OPT_CODE_CLIENTID	1	/* DHCP6 CLIENTID option */
#define	DHCP6_OPT_CODE_SERVERID	2	/* DHCP6 SERVERID option */

#define DHCP6_PORT_SERVER	547	/* DHCP6 server UDP port */
#define DHCP6_PORT_CLIENT	546	/* DHCP6 client UDP port */

#endif	/* #ifndef _bcmdhcp_h_ */
