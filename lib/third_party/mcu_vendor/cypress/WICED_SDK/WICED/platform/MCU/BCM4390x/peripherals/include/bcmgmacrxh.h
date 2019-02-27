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
 * Hardware-specific Receive Data Header for the
 * Broadcom Home Networking Division
 * BCM47XX GbE cores.
 *
 * $Id: bcmgmacrxh.h 376342 2012-12-24 21:02:49Z palter $
 */

#ifndef _bcmgmacrxh_h_
#define	_bcmgmacrxh_h_

/*
 * The Ethernet GMAC core returns an 8-byte Receive Frame Data Header
 * with every frame consisting of
 * 16 bits of frame length, followed by
 * 16 bits of GMAC rx descriptor info, followed by 32bits of undefined.
 */
typedef volatile struct {
	uint16	len;
	uint16	flags;
	uint16	pad[12];
} bcmgmacrxh_t;

#define	RXHDR_LEN	28	/* Header length */

#define	GRXF_DT_MASK	((uint16)0xf)		/* data type */
#define	GRXF_DT_SHIFT	12
#define	GRXF_DC_MASK	((uint16)0xf)		/* (num descr to xfer the frame) - 1 */
#define	GRXF_DC_SHIFT	8
#define	GRXF_OVF	((uint16)1 << 7)	/* overflow error occured */
#define	GRXF_OVERSIZE	((uint16)1 << 4)	/* frame size > rxmaxlength */
#define	GRXF_CRC	((uint16)1 << 3)	/* crc error */
#define	GRXF_VLAN	((uint16)1 << 2)	/* vlan tag detected */
#define	GRXF_PT_MASK	((uint16)3)		/* packet type 0 - Unicast,
						 * 1 - Multicast, 2 - Broadcast
						 */

#endif	/* _bcmgmacrxh_h_ */
