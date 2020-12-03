/*******************************************************************************
* \file cybt_platform_task.c
*
* \brief
* Provides API for OS task communication setup.
*
********************************************************************************
* \copyright
* Copyright 2018-2019 Cypress Semiconductor Corporation
* SPDX-License-Identifier: Apache-2.0
*
* Licensed under the Apache License, Version 2.0 (the "License");
* you may not use this file except in compliance with the License.
* You may obtain a copy of the License at
*
*     http://www.apache.org/licenses/LICENSE-2.0
*
* Unless required by applicable law or agreed to in writing, software
* distributed under the License is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
* See the License for the specific language governing permissions and
* limitations under the License.
*******************************************************************************/


#ifndef CYBT_PLATFORM_TASK_H
#define CYBT_PLATFORM_TASK_H

#include <stdint.h>
#include <stdbool.h>

#include "cyabs_rtos.h"

#include "cybt_result.h"

/*****************************************************************************
 *                                Constants
 *****************************************************************************/
#define BT_TASK_ID_BTU                     (0)
#define BT_TASK_ID_HCI                     (1)
#define BT_TASK_NUM                        (2)

#define BT_TASK_NAME_BTU                    "CYBT_BT_Task"
#define BT_TASK_NAME_HCI                    "CYBT_HCI_Task"

#define BT_EVT_TO_BTU_EVENT                 (0x0201)
#define BT_EVT_TO_BTU_ACL                   (0x0202)
#define BT_EVT_TO_BTU_SCO                   (0x0203)
#define BT_EVT_TO_BTU_TIMER                 (0x0204)

#define BT_EVT_TO_HCI_COMMAND               (0x0401)
#define BT_EVT_TO_HCI_ACL                   (0x0402)
#define BT_EVT_TO_HCI_SCO                   (0x0403)
#define BT_EVT_TO_HCI_TIMER                 (0x0404)
#define BT_EVT_TO_HCI_DATA_READY            (0x0405)

#define BT_EVT_TASK_SHUTDOWN                (0xDAFFFFFF)

#define BT_EVT_HCI_DATA_READY_BASE          (0xDAAEADFC)

#define BTU_TASK_PRIORITY                   (CY_RTOS_PRIORITY_ABOVENORMAL)
#define HCI_TASK_PRIORITY                   (CY_RTOS_PRIORITY_HIGH)

#define BTU_TASK_STACK_SIZE                 (0x1800)
#define HCI_TASK_STACK_SIZE                 (0x1000)

#define  CYBT_TASK_DEFAULT_POOL_SIZE        (2048)

#define  CYBT_INVALID_QUEUE_UTILIZATION     (0xFF)

/*****************************************************************************
 *                           Type Definitions
 *****************************************************************************/
/**
 * BT HCI incoming data ready event
 */
#define BT_EVT_TO_HCI_DATA_READY_UNKNOWN   (BT_EVT_HCI_DATA_READY_BASE + 0)
#define BT_EVT_TO_HCI_DATA_READY_ACL       (BT_EVT_HCI_DATA_READY_BASE + 1)
#define BT_EVT_TO_HCI_DATA_READY_SCO       (BT_EVT_HCI_DATA_READY_BASE + 2)
#define BT_EVT_TO_HCI_DATA_READY_EVT       (BT_EVT_HCI_DATA_READY_BASE + 3)
typedef uint16_t bt_task_event_t;

/**
 * Message structure is used to communicate between tasks
 */
typedef struct
{
    bt_task_event_t  event;    /**< event id */
    uint16_t         length;   /**< payload length */
} BT_MSG_HDR;

/**
 * Message header size
 */
#define BT_MSG_HDR_SIZE             (sizeof(BT_MSG_HDR))

extern cy_queue_t  cybt_task_queue[];

/**
 * Task queue related declaration
 */
#define BTU_TASK_QUEUE              (cybt_task_queue[BT_TASK_ID_BTU])
#define HCI_TASK_QUEUE              (cybt_task_queue[BT_TASK_ID_HCI])

#define BTU_TASK_QUEUE_COUNT        (32)
#define HCI_TASK_QUEUE_COUNT        (32)
#define BTU_TASK_QUEUE_ITEM_SIZE    (sizeof(BT_MSG_HDR *))
#define HCI_TASK_QUEUE_ITEM_SIZE    (sizeof(BT_MSG_HDR *))

#ifdef __cplusplus
extern "C"
{
#endif

/*****************************************************************************
 *                           Function Declarations
 ****************************************************************************/

/**
 * Initialize Bluetooth related OS tasks.
 *
 * @returns  CYBT_SUCCESS
 *           CYBT_ERR_INIT_MEMPOOL_FAILED
 *           CYBT_ERR_INIT_QUEUE_FAILED
 *           CYBT_ERR_CREATE_TASK_FAILED
 */
cybt_result_t cybt_platform_task_init(void);


/**
 * Delete Bluetooth related OS tasks.
 *
 * @returns  CYBT_SUCCESS
 *           CYBT_ERR_OUT_OF_MEMORY
 *           CYBT_ERR_SEND_QUEUE_FAILED
 */
cybt_result_t cybt_platform_task_deinit(void);


/**
 * Initialize task memory pool.
 *
 * @param[in] total_size: the request size of memory pool
 *
 * @returns  CYBT_SUCCESS
 *           CYBT_ERR_OUT_OF_MEMORY
 *
 */
cybt_result_t cybt_platform_task_mempool_init(uint32_t total_size);


/**
 * Allocate the memory block from task memory pool.
 *
 * @param[in] req_size: the request size of memory block
 *
 * @returns  the pointer of memory block
 *
 */
void *cybt_platform_task_mempool_alloc(uint32_t req_size);


/**
 * Free and return the memory block to task memory pool.
 *
 * @param[in]   p_mem_block: the pointer of memory block
 *
 * @returns     void
 *
 */
void cybt_platform_task_mempool_free(void *p_mem_block);


/**
 * Release task memory pool.
 *
 * @returns     void
 */
void cybt_platform_task_mempool_deinit(void);


/**
 * Send message to BT task.
 *
 * @param[in] p_bt_msg    : the pointer of the message
 * @param[in] is_from_isr : true if this function is called from isr context
 *                         otherwise false
 *
 * @returns  CYBT_SUCCESS
 *           CYBT_ERR_BADARG
 *           CYBT_ERR_QUEUE_ALMOST_FULL
 *           CYBT_ERR_SEND_QUEUE_FAILED
 */
cybt_result_t cybt_send_msg_to_bt_task(BT_MSG_HDR *p_bt_msg,
                                                   bool is_from_isr
                                                  );


/**
 * Send message to HCI task.
 *
 * @param[in] p_bt_msg    : the pointer of the message
 * @param[in] is_from_isr : true if this function is called from isr context
 *                         otherwise false
 *
 * @returns  CYBT_SUCCESS
 *           CYBT_ERR_BADARG
 *           CYBT_ERR_SEND_QUEUE_FAILED
 */
cybt_result_t cybt_send_msg_to_hci_task(BT_MSG_HDR *p_bt_msg,
                                                     bool is_from_isr
                                                    );


/**
 * Get the usage rate of task message queue.
 *
 * @param[in]  task_id: the task id to query
 *
 * @returns    the utilization in percentage
 */
uint8_t cybt_platform_task_queue_utilization(uint8_t task_id);


#ifdef __cplusplus
} /* extern "C" */
#endif

#endif

