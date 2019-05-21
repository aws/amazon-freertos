/** @file semdbg.h
*
*  @brief This file contains declarations for Semaphore Debug functions
*
*  (C) Copyright 2013-2018 Marvell International Ltd. All Rights Reserved
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

#ifndef _SEM_DBG_H_
#define _SEM_DBG_H_

#include "semphr.h"

#ifdef CONFIG_SEMAPHORE_DEBUG
typedef struct {
	xSemaphoreHandle x_queue;
	char *q_name;
	bool is_semaphore;
} sem_dbg_info_t;

#define MAX_SEM_INFO_BUF 48
extern void sem_debug_add(const xSemaphoreHandle handle, const char *name,
			  bool is_semaphore);
extern void sem_debug_delete(const xSemaphoreHandle handle);
extern int seminfo_init(void);

#else

static inline void sem_debug_add(const xSemaphoreHandle handle,
				 const char *name, bool is_semaphore) { }
static inline void sem_debug_delete(const xSemaphoreHandle handle) { }
static inline int seminfo_init(void)
{
	return 0;
}

#endif /* CONFIG_SEMAPHORE_DEBUG */

#endif /* _SEM_DBG_H_ */
