/** @file psm-internal.h
*
*  @brief PSM Internal Header
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

#ifndef __PSM_INTERNAL_H__
#define __PSM_INTERNAL_H__

/* #define CONFIG_PSM_INIT_SCAN_DEBUG */
/* #define CONFIG_PSM_EXTRA_DEBUG */
/* #define CONFIG_PSM_FUNC_DEBUG */
/* #define CONFIG_PSM_INDEXING_DEBUG */
/* #define CONFIG_PSM_LOWLEVEL_FLASH_OPR_DEBUG */
/* #define CONFIG_PSM_LOCKS_DEBUG */

#include <wmlog.h>

#ifdef CONFIG_PSM_INIT_SCAN_DEBUG
#define psm_isc(...)				\
	wmlog("psm-isc", ##__VA_ARGS__)
#else
#define psm_isc(...)
#endif

#define psm_e(...)				\
	wmlog_e("psm", ##__VA_ARGS__)
#define psm_w(...)				\
	wmlog_w("psm", ##__VA_ARGS__)
#ifdef CONFIG_PSM_DEBUG
#define psm_d(...)				\
	wmlog("psm", ##__VA_ARGS__)
#else
#define psm_d(...)
#endif /* ! CONFIG_PSM_DEBUG */
#ifdef CONFIG_PSM_EXTRA_DEBUG
#define psm_ed(...)				\
	wmlog("psm-ed", ##__VA_ARGS__)
#else
#define psm_ed(...)
#endif /* ! CONFIG_PSM_EXTRA_DEBUG */
#ifdef CONFIG_PSM_INDEXING_DEBUG
#define psm_i(...)				\
	wmlog("psm-i", ##__VA_ARGS__)
#else
#define psm_i(...)
#endif /* ! CONFIG_PSM_INDEXING_DEBUG */
#ifdef CONFIG_PSM_LOWLEVEL_FLASH_OPR_DEBUG
#define psm_ll(...)				\
	wmlog("psm-ll", ##__VA_ARGS__)
#else
#define psm_ll(...)
#endif /* ! CONFIG_PSM_LOWLEVEL_FLASH_OPR_DEBUG */

#ifdef CONFIG_PSM_FUNC_DEBUG
#define psm_entry(_fmt_, ...)						\
	wmprintf("######## > %s ("_fmt_") ####\n\r", __func__, ##__VA_ARGS__)
#define psm_entry_i(...)			\
	wmlog_entry(__VA_ARGS__)
#else
#define psm_entry(...)
#define psm_entry_i(...)
#endif /* ! CONFIG_PSM_DEBUG */


typedef enum {
	PSM_EVENT_READ,
	PSM_EVENT_WRITE,
	PSM_EVENT_ERASE,
	PSM_EVENT_COMPACTION,
} psm_event_t;

typedef enum {
	PSM_COMPACTION_1,
	PSM_COMPACTION_2,
	PSM_COMPACTION_3,
	PSM_COMPACTION_4,
	PSM_COMPACTION_5,
	PSM_COMPACTION_6,
	PSM_COMPACTION_7,
	PSM_COMPACTION_8,
	PSM_COMPACTION_9,
	PSM_COMPACTION_10,
	PSM_COMPACTION_MAX,
} psm_compaction_evt_t;

typedef void (*psm_event_callback)(psm_event_t event, void *data, void *data1);

int psm_register_event_callback(psm_event_callback cb);

#endif /* __PSM_INTERNAL_H__ */
