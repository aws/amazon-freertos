/*
 * Copyright (c) 2020, Texas Instruments Incorporated
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * *  Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * *  Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * *  Neither the name of Texas Instruments Incorporated nor the names of
 *    its contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

/**
 * @file bt_hal_mq_task.h
 * @brief Task used to process asychronous ICall messages
 */

#ifndef _BT_HAL_MQ_TASK_
#define _BT_HAL_MQ_TASK_

#include <icall.h>
#include <bcomdef.h>
#include <bt_hal_manager_types.h>

#ifdef __cplusplus
extern "C"
{
#endif


/**
 * @brief      Create ICall task that runs in the background reading
 *             asynchronous messages from the TI BLE-Stack. This task will
 *             distribute these messages to the necessary layers within the
 *             AFR BLE HAL Layer
 *
 * @return     eBTStatusDone - Task has already been created
 *             eBTStatusFail - POSIX returned an error when creating the
 *                             task or creating/posting semaphore
 *             eBTStatusSuccess - Task created successfully
 */
extern BTStatus_t _IotBleHalAsyncMq_CreateTask( void );

/**
 * @brief      Destroy ICall MQ task
 *
 * @return     The bt status.
 */
extern void _IotBleHalAsyncMq_DestroyTask( void );

/**
 * @brief      Get ICall entity ID associated with the message queue task
 *
 *             Used by other modules within the BLE HAL to route asynchronous
 *             messages from the stack to the message queue task. This prevents
 *             the user task from having to service the ICall message queue
 *
 * @return     ICall_EntityID - Entity ID of the ICall message queue task
 */
extern ICall_EntityID _IotBleHalAsyncMq_GetEntity( void );

#ifdef __cplusplus
}
#endif

#endif /* ifndef _BT_HAL_MQ_TASK_ */
