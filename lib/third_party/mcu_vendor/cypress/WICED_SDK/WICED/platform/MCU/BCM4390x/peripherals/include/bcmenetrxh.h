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
 * BCM44XX and BCM47XX 10/100 Mbps Ethernet cores.
 *
 * $Id: bcmenetrxh.h 376342 2012-12-24 21:02:49Z palter $
 */

#ifndef _bcmenetrxh_h_
#define	_bcmenetrxh_h_

/*
 * The Ethernet MAC core returns an 8-byte Receive Frame Data Header
 * with every frame consisting of
 * 16bits of frame length, followed by
 * 16bits of EMAC rx descriptor info, followed by 32bits of undefined.
 */
typedef volatile struct {
	uint16	len;
	uint16	flags;
	uint16	pad[12];
} bcmenetrxh_t;

#define	RXHDR_LEN	28	/* Header length */

#define	RXF_L		((uint16)1 << 11)	/* last buffer in a frame */
#define	RXF_MISS	((uint16)1 << 7)	/* received due to promisc mode */
#define	RXF_BRDCAST	((uint16)1 << 6)	/* dest is broadcast address */
#define	RXF_MULT	((uint16)1 << 5)	/* dest is multicast address */
#define	RXF_LG		((uint16)1 << 4)	/* frame length > rxmaxlength */
#define	RXF_NO		((uint16)1 << 3)	/* odd number of nibbles */
#define	RXF_RXER	((uint16)1 << 2)	/* receive symbol error */
#define	RXF_CRC		((uint16)1 << 1)	/* crc error */
#define	RXF_OV		((uint16)1 << 0)	/* fifo overflow */

#endif	/* _bcmenetrxh_h_ */
