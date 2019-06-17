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

/*! \file system-work-queue.h
 * \brief System Work Queue
 *
 * Work queues are software entities provided as a service to the
 * application layer to simplify the task of programming new
 * features. For more information on this, please refer \ref work-queue.h.
 * If various modules need to use this feature, there will be too many work
 * queues in the system, which is not desirable. Hence, a common system queue
 * has been defined which can be used by all the modules. It runs at a priority
 * \ref DEFAULT_WORKER_PRIO and offers stack as per CONFIG_SYS_WQ_STACK config
 * option
 */

#ifndef __SYSTEM_WORK_QUEUE_H__
#define __SYSTEM_WORK_QUEUE_H__

#include <work-queue.h>

/** Initialize the system work queue
 *
 * This functions initializes the system work queue.
 * This call is mandatory before a work queue can be used.
 *
 * \return WM_SUCCESS System Work queue initialization successfully.
 * \return -WM_E_INVAL if invalid arguments given.
 * \return -WM_E_NOMEM if heap allocation failed.
 * \return -WM_FAIL for any other failure.
 */
int sys_work_queue_init();
/** Gives the System Work Queue Handle
 *
 * This API returns a system work queue handle if its initialized using
 * sys_work_queue_init(). Calling this API multiple times does not cause any
 * inadvertent behavior. Jobs can then be added to or removed from this queue
 * using work_enqueue() and work_dequeue(). This API is safe to call from
 * an interrupt handler as well.
 *
 * \return wq_handle_t System Work queue handle if work queue is initialized
 * successfully.
 * \return 0 If work queue is not initialized.
 */
wq_handle_t sys_work_queue_get_handle(void);

#endif /* __SYSTEM_WORK_QUEUE_H__ */
