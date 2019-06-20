/** @file wmlog.h
*
*  @brief This file contains macros to print logs
*
*  (C) Copyright 2015-2018 Marvell International Ltd. All Rights Reserved
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

#ifndef __WMLOG_H__
#define __WMLOG_H__

#ifdef CONFIG_ENABLE_ERROR_LOGS
#define wmlog_e(_mod_name_, _fmt_, ...)                                        \
	wmprintf("[%s]%s" _fmt_ "\n\r", _mod_name_, " Error: ", ##__VA_ARGS__)
#else
#define wmlog_e(...)
#endif /* CONFIG_ENABLE_ERROR_LOGS */

#ifdef CONFIG_ENABLE_WARNING_LOGS
#define wmlog_w(_mod_name_, _fmt_, ...)                                        \
	wmprintf("[%s]%s" _fmt_ "\n\r", _mod_name_, " Warn: ", ##__VA_ARGS__)
#else
#define wmlog_w(...)
#endif /* CONFIG_ENABLE_WARNING_LOGS */

/* General debug function. User can map his own debug functions to this
ne */
#ifdef CONFIG_DEBUG_BUILD
#define wmlog(_mod_name_, _fmt_, ...)                          \
	wmprintf("[%s] " _fmt_ "\n\r", _mod_name_, ##__VA_ARGS__)
#else
#define wmlog(...)
#endif /* CONFIG_DEBUG_BUILD */

#ifdef CONFIG_DEBUG_BUILD
/* Function entry */
#define wmlog_entry(_fmt_, ...)                                        \
	wmprintf("> %s (" _fmt_ ")\n\r", __func__, ##__VA_ARGS__)

/* function exit */
#define wmlog_exit(_fmt_, ...)                                        \
	wmprintf("< %s" _fmt_ "\n\r", __func__, ##__VA_ARGS__)
#else
#define wmlog_entry(...)
#define wmlog_exit(...)
#endif /* CONFIG_DEBUG_BUILD */

#ifdef CONFIG_LL_DEBUG
#define ll_log(_fmt_, ...)						\
	ll_printf(_fmt_, ##__VA_ARGS__)
#else
#define ll_log(...)
#endif /* CONFIG_LL_DEBUG */
#endif /* __WMLOG_H__ */
