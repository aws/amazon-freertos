/*******************************************************************************
* \file cybt_platform_trace.c
*
* \brief
* Provides platform logging functionality.
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

#include <stdint.h>
#include <stdio.h>

#include "cybt_platform_trace.h"

#ifdef CYBT_PLATFORM_TRACE_ENABLE

/******************************************************************************
 *                           Variables Definitions
 ******************************************************************************/
cybt_platform_trace_cb_t trace_cb =
{
   .trace_level = {INITIAL_TRACE_LEVEL_MAIN,
                   INITIAL_TRACE_LEVEL_SPIF,
                   INITIAL_TRACE_LEVEL_BT_TASK,
                   INITIAL_TRACE_LEVEL_HCI_TASK,
                   INITIAL_TRACE_LEVEL_HCI_DRV,
                   INITIAL_TRACE_LEVEL_HCI_LOG,
                   INITIAL_TRACE_LEVEL_MEMORY,
                   INITIAL_TRACE_LEVEL_PRM,
                   INITIAL_TRACE_LEVEL_STACK,
                   INITIAL_TRACE_LEVEL_APP
                  }
};


/******************************************************************************
 *                           Function Definitions
 ******************************************************************************/

/********************************************************************************
 **
 **    Function Name:   bt_platform_set_trace_level
 **
 **    Purpose:  Set trace level
 **
 **    Input Parameters:  id:    trace id
 **                       level: trace level
 **    Returns:
 **                      Nothing
 **
 *********************************************************************************/
void cybt_platform_set_trace_level(cybt_trace_id_t id,
                                               cybt_trace_level_t level
                                              )
{
    if(CYBT_TRACE_ID_MAX < level)
    {
        return;
    }

    if(CYBT_TRACE_ID_MAX > id)
    {
        trace_cb.trace_level[id] = level;
    }
    else if(CYBT_TRACE_ID_ALL == id)
    {
        uint8_t idx;

        for(idx = 0; idx < CYBT_TRACE_ID_MAX; idx++)
        {
            trace_cb.trace_level[idx] = level;
        }
    }
}
#endif

