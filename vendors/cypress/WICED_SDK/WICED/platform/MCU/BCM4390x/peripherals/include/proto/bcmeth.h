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
 * Broadcom Ethernettype  protocol definitions
 *
 * $Id: bcmeth.h 445748 2013-12-30 12:57:34Z karenp $
 */

/*
 * Broadcom Ethernet protocol defines
 */

#ifndef _BCMETH_H_
#define _BCMETH_H_

#ifndef _TYPEDEFS_H_
#include <typedefs.h>
#endif

/* This marks the start of a packed structure section. */
#include <packed_section_start.h>

/* ETHER_TYPE_BRCM is defined in ethernet.h */

/*
 * Following the 2byte BRCM ether_type is a 16bit BRCM subtype field
 * in one of two formats: (only subtypes 32768-65535 are in use now)
 *
 * subtypes 0-32767:
 *     8 bit subtype (0-127)
 *     8 bit length in bytes (0-255)
 *
 * subtypes 32768-65535:
 *     16 bit big-endian subtype
 *     16 bit big-endian length in bytes (0-65535)
 *
 * length is the number of additional bytes beyond the 4 or 6 byte header
 *
 * Reserved values:
 * 0 reserved
 * 5-15 reserved for iLine protocol assignments
 * 17-126 reserved, assignable
 * 127 reserved
 * 32768 reserved
 * 32769-65534 reserved, assignable
 * 65535 reserved
 */

/*
 * While adding the subtypes and their specific processing code make sure
 * bcmeth_bcm_hdr_t is the first data structure in the user specific data structure definition
 */

#define	BCMILCP_SUBTYPE_RATE		1
#define	BCMILCP_SUBTYPE_LINK		2
#define	BCMILCP_SUBTYPE_CSA		3
#define	BCMILCP_SUBTYPE_LARQ		4
#define BCMILCP_SUBTYPE_VENDOR		5
#define	BCMILCP_SUBTYPE_FLH		17

#define BCMILCP_SUBTYPE_VENDOR_LONG	32769
#define BCMILCP_SUBTYPE_CERT		32770
#define BCMILCP_SUBTYPE_SES		32771


#define BCMILCP_BCM_SUBTYPE_RESERVED		0
#define BCMILCP_BCM_SUBTYPE_EVENT		1
#define BCMILCP_BCM_SUBTYPE_SES			2
/*
 * The EAPOL type is not used anymore. Instead EAPOL messages are now embedded
 * within BCMILCP_BCM_SUBTYPE_EVENT type messages
 */
/* #define BCMILCP_BCM_SUBTYPE_EAPOL		3 */
#define BCMILCP_BCM_SUBTYPE_DPT                 4

#define BCMILCP_BCM_SUBTYPEHDR_MINLENGTH	8
#define BCMILCP_BCM_SUBTYPEHDR_VERSION		0

/* These fields are stored in network order */
typedef BWL_PRE_PACKED_STRUCT struct bcmeth_hdr
{
	uint16	subtype;	/* Vendor specific..32769 */
	uint16	length;
	uint8	version;	/* Version is 0 */
	uint8	oui[3];		/* Broadcom OUI */
	/* user specific Data */
	uint16	usr_subtype;
} BWL_POST_PACKED_STRUCT bcmeth_hdr_t;


/* This marks the end of a packed structure section. */
#include <packed_section_end.h>

#endif	/*  _BCMETH_H_ */
