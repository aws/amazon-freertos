/*
 * Extensible Authentication Protocol (EAP) definitions
 *
 * See
 * RFC 2284: PPP Extensible Authentication Protocol (EAP)
 *
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
 * $Id: eap.h 405837 2013-06-05 00:13:20Z harveysm $
 */

#ifndef _eap_h_
#define _eap_h_

/* This marks the start of a packed structure section. */
#include <packed_section_start.h>

/* EAP packet format */
typedef BWL_PRE_PACKED_STRUCT struct {
	unsigned char code;	/* EAP code */
	unsigned char id;	/* Current request ID */
	unsigned short length;	/* Length including header */
	unsigned char type;	/* EAP type (optional) */
	unsigned char data[1];	/* Type data (optional) */
} BWL_POST_PACKED_STRUCT eap_header_t;

#define EAP_HEADER_LEN 4

/* EAP codes */
#define EAP_REQUEST	1
#define EAP_RESPONSE	2
#define EAP_SUCCESS	3
#define EAP_FAILURE	4

/* EAP types */
#define EAP_IDENTITY		1
#define EAP_NOTIFICATION	2
#define EAP_NAK			3
#define EAP_MD5			4
#define EAP_OTP			5
#define EAP_GTC			6
#define EAP_TLS			13
#define EAP_EXPANDED		254
#define BCM_EAP_SES		10
#define BCM_EAP_EXP_LEN		12  /* EAP_LEN 5 + 3 bytes for SMI ID + 4 bytes for ven type */
#define BCM_SMI_ID		0x113d
#define WFA_VENDOR_SMI	0x009F68

#ifdef  BCMCCX
#define EAP_LEAP		17

#define LEAP_VERSION		1
#define LEAP_CHALLENGE_LEN	8
#define LEAP_RESPONSE_LEN	24

/* LEAP challenge */
typedef struct {
	unsigned char version;		/* should be value of LEAP_VERSION */
	unsigned char reserved;		/* not used */
	unsigned char chall_len;	/* always value of LEAP_CHALLENGE_LEN */
	unsigned char challenge[LEAP_CHALLENGE_LEN]; /* random */
	unsigned char username[1];
} leap_challenge_t;

#define LEAP_CHALLENGE_HDR_LEN	12

/* LEAP challenge reponse */
typedef struct {
	unsigned char version;	/* should be value of LEAP_VERSION */
	unsigned char reserved;	/* not used */
	unsigned char resp_len;	/* always value of LEAP_RESPONSE_LEN */
	/* MS-CHAP hash of challenge and user's password */
	unsigned char response[LEAP_RESPONSE_LEN];
	unsigned char username[1];
} leap_response_t;

#define LEAP_RESPONSE_HDR_LEN	28

#endif /* BCMCCX */

/* This marks the end of a packed structure section. */
#include <packed_section_end.h>

#endif /* _eap_h_ */
