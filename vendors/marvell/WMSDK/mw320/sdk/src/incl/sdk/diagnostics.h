/** @file diagnostics.h
*
*  @brief System Diagnostics Header
*
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

#ifndef __DIAGNOSTICS_H__
#define __DIAGNOSTICS_H__

#include <wmstats.h>
#include <wmerrno.h>
#include <psm-v2.h>
#include <json_parser.h>
#include <json_generator.h>

int diagnostics_read_stats(struct json_str *jptr, psm_hnd_t psm_hnd);
int diagnostics_write_stats(psm_hnd_t psm_hnd);
int diagnostics_read_stats_psm(struct json_str *jptr, psm_hnd_t psm_hnd);
void diagnostics_set_reboot_reason(wm_reboot_reason_t reason);
typedef void (*diagnostics_write_cb)(psm_hnd_t psm_hnd,
	wm_reboot_reason_t reason);

/** Initialize the diagnostics module
 *
 * \return WM_SUCCESS on success
 * \return error code otherwise
 *
 */
int diagnostics_init();
#endif
