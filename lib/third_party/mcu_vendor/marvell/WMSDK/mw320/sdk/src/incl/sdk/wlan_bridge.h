/*
*  (C) Copyright 2008-2018 Marvell International Ltd. All Rights Reserved
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

/*! \file wlan_bridge.h
 * \brief WLAN bridge header
 */

#ifndef __WLAN_BRIDGE_H__
#define __WLAN_BRIDGE_H__

#include <wifi.h>

/** Auto Link Periodical scan interval in seconds*/
#define AUTOLINK_SCANINTERVAL_DEFAULT		20

/** The condition triggers Auto Link periodical scan
0:trigger scan when current link is not good
1:trigger scan by host setting(always periodical scan)
*/
#define AUTOLINK_SCANTIMER_COND_DEFAULT		0x01

/** Auto Link periodical scan channel list:
0:only scan with previous In-STA associated channel
1: 2.4G all channels
*/
#define AUTOLINK_SCANCHANNEL_DEFAULT		0x01

/** Bridge configuration structure */
typedef wifi_bridge_cfg_t wlan_bridge_cfg_t;


/** Enable Bridge mode in WLAN firmware.
 * \
 * \param[in] cfg, A pointer to Bridge configuration structure holding enable,
 *	      auto_link, hidden_ssid, EX-AP SSID, Passphrase, Bridge SSID and
 *	      Passphrase and autolink configuration.
 *
 * \return WM_SUCCESS if operation is successful.
 * \return -WM_FAIL if operation is failed.
 */
static inline int wlan_enable_bridge_mode(wlan_bridge_cfg_t *cfg)
{
	return wifi_enable_bridge_mode(cfg);
}

/** Disable Bridge mode in WLAN firmware.
 *
 * \return WM_SUCCESS if operation is successful.
 * \return -WM_FAIL if operation is failed.
 */
static inline int wlan_disable_bridge_mode()
{
	return wifi_disable_bridge_mode();
}

/** Get Bridge configuration from WLAN firmware.
 *
 * \param[out] cfg Bridge configuration structure where EX-AP SSID,
 *             Passphrase, Bridge SSID and Passphrase will get copied.
 *
 * \return WM_SUCCESS if operation is successful.
 * \return -WM_FAIL if operation is failed.
 */
static inline int wlan_get_bridge_mode_config(wlan_bridge_cfg_t *cfg)
{
	return wifi_get_bridge_mode_config(cfg);
}

/**
 * Reconfigure TX buffer size during bridge mode operation.
 *
 * \param[in] buf_size Buffer size to configure.
 *
 * \return WM_SUCCESS if operation is successful.
 * \return -WM_FAIL is operation is failed.
 */
static inline int wlan_config_bridge_tx_buf(uint16_t buf_size)
{
	return wifi_config_bridge_tx_buf(buf_size);
}
#endif /* WLAN_BRIDGE_H */
