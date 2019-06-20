/*  (C) Copyright 2008-2018 Marvell International Ltd. All Rights Reserved
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

#ifndef __WLAN_SMC_H__
#define __WLAN_SMC_H__

/**
 * WLAN Smart Mode Configuration
 *
 * SMC mode is a sniffer mode in which device sniffs packets on
 * pre-configured channels. Frame filters can be applied for data, control or
 * management frames using configuration parameters defining filters on address
 * type, start address and end address of multicast packets, subtype of
 * management packets etc.
 *
 * In SMC mode, co-existence of sniffer mode and micro-AP mode is achieved as
 * device sniffs frames as well as sends beacons and probe responses on
 * pre-defined channel.
 *
 */


#include <wlan_11d.h>
#include <wlan.h>

typedef struct {
	/** Beacon Period in milliseconds */
	int beacon_period;
	/** Minimum scan duration on each channel */
	uint16_t min_scan_time;
	/** Maximum scan duration on each channel */
	uint16_t max_scan_time;
	/** Starting address of valid multicast address
	 *  that would be forwarded to host
	 */
	uint8_t smc_start_addr[MLAN_MAC_ADDR_LENGTH];
	/** End address of valid multicast address
	 *  that would be forwarded to host
	 */
	uint8_t smc_end_addr[MLAN_MAC_ADDR_LENGTH];
	/** Filters for exit from STA or exit from AP frames
	 *
	 *  Bits     |  Description
	 *  15 - 2   |  Reserved
	 *  1        |  Rx frames from STAs
	 *  0        |  Rx frames from APs
	 *
	 */
	uint16_t filter_type;
	/** Length indicates number of DestType and
	 *  FrameType filters listed in smc_frame_filter
	 */
	int smc_frame_filter_len;
	/** Configure frame filters to forward matching filter frames to host
	 *
	 *  SMC Frame Filter information
	 *
	 *  Dest Type:
	 *  0x01 unicast
	 *  0x02 broadcast
	 *  0x04 multicast
	 *
	 *  Frame Type:
	 *  Bits [1:0] Type
	 *        10   Data
	 *        01   Control
	 *        00   Management
	 *  Bits [5:2] Subtype
	 *       0000  Association request
	 *       0001  Association response
	 *       1000  Beacon
	 *       0101  Probe response
	 *       0100  Probe request
	 *       1011  Authentication
	 *
	 *  smc_frame_filter list is in the form of
	 *  { [dest type 0], [frame type 0], [dest type 1], [frame type 1], ..}
	 *
	 *  For eg. To apply frame filter for,
	 *  probe request which is broadcast packet and management frame,
	 *  Dest Type: 0x02 and Frame Type: 0b00010000 which is 0x10
	 *  multicast packet which is a data packet,
	 *  Dest Type: 0x04 and Frame Type: 0b00000010
	 *  smc_frame_filter will be {0x02, 0x10, 0x04, 0x02}
	 *
	 */
	uint8_t *smc_frame_filter;
	/** 802.11d country codes */
	country_code_t country_code;
	/** Channel information
	 *  Value       Meaning
	 *  0 (auto)    Sniff packets on country code specific frequency bands
	 *  x           Sniff packets on channel x and country code specific
	 *              channels adjacent to x with increased dwelling time
	 */
	uint8_t channel;
	/** Custom IE length */
	int custom_ie_len;
	/** Custom IE to set in Beacon and Probe Response */
	uint8_t custom_ie[255];
} smart_mode_cfg_t;

/**
 * Print WLAN Smart Configuration.
 *
 * This function displays hexdump of command sent for smart mode configuration
 *
 * \return WM_SUCCESS if successful.
 * \return -WM_FAIL if unsuccessful.
 *
 */
int wlan_print_smart_mode_cfg();

/**
 * Set WLAN Smart Configuration.
 *
 * \param[in] uap_network uAP network information
 *            uap_network->ssid: the uAP SSID
 *            uap_network->channel: the uAP channel
 *            [Note] In smc mode, beacons and probe responses are sent on this
 *                   channel
 *            uap_network->security.type: security type
 *            uap_network->security.psk: passphrase
 *            uap_network->security.psk_len: passphrase length
 *            [Note] uAP security is not supported yet
 * \param[in] cfg Populate \ref smart_mode_cfg_t
 * \param[in] sniffer_callback callback function to receive sniffed packets.
 *
 * \return WM_SUCCESS if successful.
 * \return -WM_FAIL if unsuccessful.
 *
 */
int wlan_set_smart_mode_cfg(struct wlan_network *uap_network,
			    smart_mode_cfg_t *cfg, void (*sniffer_callback)
			    (const wlan_frame_t *frame, const uint16_t len));

/**
 * Start WLAN Smart Configuration mode.
 *
 * \return WM_SUCCESS if successful.
 * \return -WM_FAIL if unsuccessful.
 *
 */
int wlan_start_smart_mode();

/**
 * Stop WLAN Smart Configuration mode.
 *
 * \return WM_SUCCESS if successful.
 * \return -WM_FAIL if unsuccessful.
 *
 */
int wlan_stop_smart_mode();

/**
 * Stop WLAN Smart Configuration mode and start uAP.
 *
 * \return WM_SUCCESS if successful.
 * \return -WM_FAIL if unsuccessful.
 *
 */
int wlan_stop_smart_mode_start_uap(struct wlan_network *uap_network);
#endif /* __WLAN_SMC_H__ */
