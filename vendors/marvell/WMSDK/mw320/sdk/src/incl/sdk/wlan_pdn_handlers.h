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

/*! \file wlan_pdn_handlers.h
 *  \brief WLAN Power Down handler API
 */

#ifndef _WLAN_PDN_H_
#define _WLAN_PDN_H_

/** WLAN Power down event enumerations */
typedef enum {
	/** On entering PDN  mode */
	ACTION_ENTER_PDN = 1,
	/** On exiting PDN mode */
	ACTION_EXIT_PDN = 2,
} wlan_pdn_event_t;

/** Function pointer for callback function called on WLAN Power-Down entry and
 * exit. Callback will be called by wlcmgr thread. */
typedef void (*wlan_pdn_cb_fn) (wlan_pdn_event_t event, void *data);

/** Register WLAN Power-Down callback
 *
 *  This function registers a callback handler with WLAN power-down manager.
 *  Based on the 'action' specified, this callback will be invoked on the
 *  corresponding WLAN power-down enter or exit events.
 *
 *  @param[in] action Logical OR of \ref ACTION_ENTER_PDN and
 *  \ref ACTION_EXIT_PDN
 *  @param[in] cb_fn pointer to the callback function of type
 *  \ref wlan_pdn_cb_fn
 *  @param[in] data Pointer which can be a pointer to any object or can be NULL
 *
 *  @return handle to be used in other APIs on success,
 *  @return -WM_E_INVAL if incorrect parameter
 *  @return -WM_E_NOMEM if no space left
 */
int wlan_pdn_register_cb(unsigned char action, wlan_pdn_cb_fn cb_fn,
		void *data);

/** De-register Power Down Callback
 *
 * This function de-registers the power down callback handlers from the WLAN
 * power manager
 *
 * @param[in] handle handle returned in wlan_pdn_register_cb()
 * @return WM_SUCCESS on success
 * @return -WM_FAIL otherwise
 */
int wlan_pdn_deregister_cb(int handle);

/** WLAN invoke power-down callback
 *
 * This function is called when either of WLAN power down events occur.
 * Callback will be called from wlcmgr's thread context.
 *
 * @param[in] action WLAN Power-Down event for which callback will be invoked.
 */
void wlan_pdn_invoke_callback(wlan_pdn_event_t action);
#endif /* _WLAN_PDN_H_ */
