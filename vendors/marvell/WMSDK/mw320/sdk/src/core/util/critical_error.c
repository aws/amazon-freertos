/** @file critical_error.c
 *
 *  @brief This file provides  API for Critical Error Loging and Messaging
 *
 *  (C) Copyright 2008-2018 Marvell International Ltd. All Rights Reserved.
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

#include <wmstdio.h>
#include <wmlog.h>
#include <wm_utils.h>
#include <critical_error.h>

/* Default action for critical error. Can be overriden by appplications
 * by writing their own function with this same signature.
 */
WEAK void critical_error(int crit_errno, void *data)
{
	ll_log("Critical error number: %d (caller addr: %x)\r\n",
			crit_errno, __builtin_return_address(0));
	ll_log("Description: %s\r\n",
			critical_error_msg(crit_errno));
	ll_log("Halting processor\r\n");
	while (1)
		;
}

char *critical_error_msg(int crit_errno)
{
	/* We pass errno to critical_error function as a negative value,
	 * hence we set a switch on -errno. */
	switch (-crit_errno) {
	case CRIT_ERR_WLAN_INIT_FAILED:
		return "WLAN initialization failed!";
	case CRIT_ERR_APP_FRAMEWORK_INIT_FAILED:
		return "App Framework init failed!";
	case CRIT_ERR_APP_FRAMEWORK:
		return "App Framework error!";
	case CRIT_ERR_HARD_FAULT:
		return "Hardfault occurred!";
	default:
		return "Other error!";
	}
}
