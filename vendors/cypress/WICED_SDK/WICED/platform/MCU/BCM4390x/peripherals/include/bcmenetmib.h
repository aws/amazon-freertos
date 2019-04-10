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
 * Hardware-specific MIB definition for
 * Broadcom Home Networking Division
 * BCM44XX and BCM47XX 10/100 Mbps Ethernet cores.
 *
 * $Id: bcmenetmib.h 376342 2012-12-24 21:02:49Z palter $
 */

#ifndef _bcmenetmib_h_
#define _bcmenetmib_h_

/* cpp contortions to concatenate w/arg prescan */
#ifndef PAD
#define	_PADLINE(line)	pad ## line
#define	_XSTR(line)	_PADLINE(line)
#define	PAD		_XSTR(__LINE__)
#endif	/* PAD */

/*
 * EMAC MIB Registers
 */
typedef volatile struct {
	uint32 tx_good_octets;
	uint32 tx_good_pkts;
	uint32 tx_octets;
	uint32 tx_pkts;
	uint32 tx_broadcast_pkts;
	uint32 tx_multicast_pkts;
	uint32 tx_len_64;
	uint32 tx_len_65_to_127;
	uint32 tx_len_128_to_255;
	uint32 tx_len_256_to_511;
	uint32 tx_len_512_to_1023;
	uint32 tx_len_1024_to_max;
	uint32 tx_jabber_pkts;
	uint32 tx_oversize_pkts;
	uint32 tx_fragment_pkts;
	uint32 tx_underruns;
	uint32 tx_total_cols;
	uint32 tx_single_cols;
	uint32 tx_multiple_cols;
	uint32 tx_excessive_cols;
	uint32 tx_late_cols;
	uint32 tx_defered;
	uint32 tx_carrier_lost;
	uint32 tx_pause_pkts;
	uint32 PAD[8];

	uint32 rx_good_octets;
	uint32 rx_good_pkts;
	uint32 rx_octets;
	uint32 rx_pkts;
	uint32 rx_broadcast_pkts;
	uint32 rx_multicast_pkts;
	uint32 rx_len_64;
	uint32 rx_len_65_to_127;
	uint32 rx_len_128_to_255;
	uint32 rx_len_256_to_511;
	uint32 rx_len_512_to_1023;
	uint32 rx_len_1024_to_max;
	uint32 rx_jabber_pkts;
	uint32 rx_oversize_pkts;
	uint32 rx_fragment_pkts;
	uint32 rx_missed_pkts;
	uint32 rx_crc_align_errs;
	uint32 rx_undersize;
	uint32 rx_crc_errs;
	uint32 rx_align_errs;
	uint32 rx_symbol_errs;
	uint32 rx_pause_pkts;
	uint32 rx_nonpause_pkts;
} bcmenetmib_t;

#endif	/* _bcmenetmib_h_ */
