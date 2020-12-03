/*******************************************************************************
* \file cybt_result.h
*
* \brief
* Defines BT basic function result. Based on this information it's easy to know
* whether the result is succeed or error with detail description.
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

#ifndef CYBT_RESULT_H
#define CYBT_RESULT_H

/*****************************************************************************
 *                           Type Definitions
 ****************************************************************************/
/** BT result code */
typedef enum
{
    CYBT_SUCCESS = 0,
    CYBT_ERR_BADARG = 0xB1,
    CYBT_ERR_OUT_OF_MEMORY,
    CYBT_ERR_TIMEOUT,
    CYBT_ERR_HCI_INIT_FAILED,
    CYBT_ERR_HCI_UNSUPPORTED_IF,
    CYBT_ERR_HCI_UNSUPPORTED_BAUDRATE,
    CYBT_ERR_HCI_NOT_INITIALIZE,
    CYBT_ERR_HCI_WRITE_FAILED,
    CYBT_ERR_HCI_READ_FAILED,
    CYBT_ERR_HCI_GET_TX_MUTEX_FAILED,
    CYBT_ERR_HCI_GET_RX_MUTEX_FAILED,
    CYBT_ERR_HCI_SET_BAUDRATE_FAILED,
    CYBT_ERR_HCI_SET_FLOW_CTRL_FAILED,
    CYBT_ERR_INIT_MEMPOOL_FAILED,
    CYBT_ERR_INIT_QUEUE_FAILED,
    CYBT_ERR_CREATE_TASK_FAILED,
    CYBT_ERR_SEND_QUEUE_FAILED,
    CYBT_ERR_MEMPOOL_NOT_INITIALIZE,
    CYBT_ERR_QUEUE_ALMOST_FULL,
    CYBT_ERR_QUEUE_FULL,
    CYBT_ERR_GENERIC
} cybt_result_t;

#endif
