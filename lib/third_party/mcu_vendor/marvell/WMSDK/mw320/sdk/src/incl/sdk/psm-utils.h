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

/** @file psm-utils.h
*
*  @brief PSM Utililty APIs.
*/

#ifndef __PSM_UTILS_H__
#define __PSM_UTILS_H__

#include <psm-v2.h>

/**
 * Get Sys PSM handle
 *
 * sys_psm_init() initializes the System PSM. sys_psm_get_handle() allows
 * you to retrieve the PSM handle init'ed in sys_psm_init(). This function
 * can be called as many times as necessary and just returns the handle
 * stored internally by Sys PSM.
 *
 * @return Non zero PSM handle. If PSM is not initialized by calling
 * sys_psm_init() then NULL will be returned.
 */
psm_hnd_t sys_psm_get_handle(void);

/**
 * Initialized System PSM
 *
 * This API is given as a helper function to initialize the PSM generally
 * used by app framework and applications. Though not mandatory the System
 * PSM is generally present in any decent application based on WMSDK. It
 * stores key-value pairs viz. AP SSiD, Passphrase required for wlan
 * functionality. The application may also use this PSM to store its own
 * key-value data.
 *
 * The System PSM partition is just like any other psm-v2 partition. So,
 * once initialized using this API, the sys_psm_get_handle() should be used
 * to retrieve the System PSM handle. This handle should then be used along
 * with PSMv2 APIs present in psm-v2.h
 *
 * @return WM_SUCCESS if System PSM is init'ed successfully.
 * @return Negative error code otherwise.
 */
int sys_psm_init(void);

/**
 * Initialize System PSM with specified partition name or flash space.
 *
 * Compared to its peer \ref sys_psm_init(), this API initializes the PSM
 * module with the enhancement that partition details can be specified either
 * as a partition name from partition table or as flash descriptor itself.
 * Refer to \ref psm_module_init_ex for more details.
 *
 * \note This API is useful for devices that do not have a standard system
 * PSM partition (with component type 'FC_COMP_PSM') in their partition table.
 *
 * @param[in] sys_init_part Pointer to \ref psm_init_part_t structure having
 * partition details. PSMv2 won't need the user copy of any field in
 * sys_init_part after call returns.
 *
 * @return WM_SUCCESS if System PSM is init'ed successfully.
 * @return -WM_E_INVAL Invalid arguments.
 * @return -WM_E_NOMEM Memory allocation failure.
 * @return -WM_FAIL otherwise
 */
int sys_psm_init_ex(const psm_init_part_t *sys_init_part);

/**
 * Initialize PSM cli for the given PSM partition
 *
 * This API helps you read/write to any PSM partition initialized by
 * you on the developer console. The existing psm cli commands can be used
 * for this purpose. psm cli commands take optional part-name
 * parameter. The part-name passed through this API is to be used at the
 * time to access the partition.
 *
 * @param[in] hnd This is the PSM handle. This can be obtained by called
 * psm_module_init() from the psm-v2 API set. Alternatively for System PSM
 * a helper function sys_psm_init() is also present.
 * @param[in] part_name This is the user friendly name of the PSM partition
 * to be used along with psm cli commands. This is the only purpose and
 * scope of the part_name.
 *
 * @return WM_SUCCESS if cli registration was successful.
 * @return Negative error code otherwise.
 */
int psm_cli_init(psm_hnd_t hnd, const char *part_name);

/*
 * Dump all the contents of the PSM partition
 *
 * @param[in] hnd This is the PSM handle. This can be obtained by called
 * psm_module_init() from the psm-v2 API set.
 */
void psm_util_dump(psm_hnd_t hnd);

#endif /* __PSM_UTILS_H__ */
