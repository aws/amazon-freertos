/*******************************************************************************
* \file cybt_platform_trace.h
*
* \brief
* Define API to have logging functionality.
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

#ifndef CYBT_PLATFORM_TRACE_H
#define CYBT_PLATFORM_TRACE_H

#include <stdio.h>

/**
 * ENABLE the below definition CYBT_PLATFORM_TRACE_ENABLE to have logging
 * functionality by using trace marcos in this file.
 */
//#define CYBT_PLATFORM_TRACE_ENABLE


/*****************************************************************************
 *                                Constants
 ****************************************************************************/
#define INITIAL_TRACE_LEVEL_MAIN          (CYBT_TRACE_LEVEL_DEBUG)
#define INITIAL_TRACE_LEVEL_SPIF          (CYBT_TRACE_LEVEL_DEBUG)
#define INITIAL_TRACE_LEVEL_BT_TASK       (CYBT_TRACE_LEVEL_DEBUG)
#define INITIAL_TRACE_LEVEL_HCI_TASK      (CYBT_TRACE_LEVEL_DEBUG)
#define INITIAL_TRACE_LEVEL_HCI_DRV       (CYBT_TRACE_LEVEL_DEBUG)
#define INITIAL_TRACE_LEVEL_HCI_LOG       (CYBT_TRACE_LEVEL_DEBUG)
#define INITIAL_TRACE_LEVEL_MEMORY        (CYBT_TRACE_LEVEL_DEBUG)
#define INITIAL_TRACE_LEVEL_PRM           (CYBT_TRACE_LEVEL_DEBUG)
#define INITIAL_TRACE_LEVEL_STACK         (CYBT_TRACE_LEVEL_DEBUG)
#define INITIAL_TRACE_LEVEL_APP           (CYBT_TRACE_LEVEL_DEBUG)


/*****************************************************************************
 *                             Type Definitions
 ****************************************************************************/
/**
 * trace category id
 */
#define CYBT_TRACE_ID_MAIN         (0)
#define CYBT_TRACE_ID_SPIF         (1)
#define CYBT_TRACE_ID_BT_TASK      (2)
#define CYBT_TRACE_ID_HCI_TASK     (3)
#define CYBT_TRACE_ID_HCI_DRV      (4)
#define CYBT_TRACE_ID_HCI_LOG      (5)
#define CYBT_TRACE_ID_MEMORY       (6)
#define CYBT_TRACE_ID_PRM          (7)
#define CYBT_TRACE_ID_STACK        (8)
#define CYBT_TRACE_ID_APP          (9)
#define CYBT_TRACE_ID_MAX          (10)
#define CYBT_TRACE_ID_ALL          (0xFF)
typedef uint8_t cybt_trace_id_t;

/**
 * trace level
 */
#define CYBT_TRACE_LEVEL_NONE      (0)
#define CYBT_TRACE_LEVEL_ERROR     (1)
#define CYBT_TRACE_LEVEL_WARNING   (2)
#define CYBT_TRACE_LEVEL_API       (3)
#define CYBT_TRACE_LEVEL_EVENT     (4)
#define CYBT_TRACE_LEVEL_DEBUG     (5)
#define CYBT_TRACE_LEVEL_MAX       (6)
typedef uint8_t cybt_trace_level_t;

/**
 * trace control block
 */
typedef struct
{
    uint8_t  trace_level[CYBT_TRACE_ID_MAX];
} cybt_platform_trace_cb_t;


#ifdef __cplusplus
extern "C"
{
#endif

#ifdef CYBT_PLATFORM_TRACE_ENABLE

extern cybt_platform_trace_cb_t trace_cb;

extern void cybt_platform_log_print(const char *fmt_str, ...);

#define MAIN_TRACE_ERROR(...)    {if (trace_cb.trace_level[CYBT_TRACE_ID_MAIN] >= CYBT_TRACE_LEVEL_ERROR) cybt_platform_log_print(__VA_ARGS__);}
#define MAIN_TRACE_WARNING(...)  {if (trace_cb.trace_level[CYBT_TRACE_ID_MAIN] >= CYBT_TRACE_LEVEL_WARNING) cybt_platform_log_print(__VA_ARGS__);}
#define MAIN_TRACE_API(...)      {if (trace_cb.trace_level[CYBT_TRACE_ID_MAIN] >= CYBT_TRACE_LEVEL_API) cybt_platform_log_print(__VA_ARGS__);}
#define MAIN_TRACE_EVENT(...)    {if (trace_cb.trace_level[CYBT_TRACE_ID_MAIN] >= CYBT_TRACE_LEVEL_EVENT) cybt_platform_log_print(__VA_ARGS__);}
#define MAIN_TRACE_DEBUG(...)    {if (trace_cb.trace_level[CYBT_TRACE_ID_MAIN] >= CYBT_TRACE_LEVEL_DEBUG) cybt_platform_log_print(__VA_ARGS__);}

#define SPIF_TRACE_ERROR(...)    {if (trace_cb.trace_level[CYBT_TRACE_ID_SPIF] >= CYBT_TRACE_LEVEL_ERROR) cybt_platform_log_print(__VA_ARGS__);}
#define SPIF_TRACE_WARNING(...)  {if (trace_cb.trace_level[CYBT_TRACE_ID_SPIF] >= CYBT_TRACE_LEVEL_WARNING) cybt_platform_log_print(__VA_ARGS__);}
#define SPIF_TRACE_API(...)      {if (trace_cb.trace_level[CYBT_TRACE_ID_SPIF] >= CYBT_TRACE_LEVEL_API) cybt_platform_log_print(__VA_ARGS__);}
#define SPIF_TRACE_EVENT(...)    {if (trace_cb.trace_level[CYBT_TRACE_ID_SPIF] >= CYBT_TRACE_LEVEL_EVENT) cybt_platform_log_print(__VA_ARGS__);}
#define SPIF_TRACE_DEBUG(...)    {if (trace_cb.trace_level[CYBT_TRACE_ID_SPIF] >= CYBT_TRACE_LEVEL_DEBUG) cybt_platform_log_print(__VA_ARGS__);}

#define BTTASK_TRACE_ERROR(...)    {if (trace_cb.trace_level[CYBT_TRACE_ID_BT_TASK] >= CYBT_TRACE_LEVEL_ERROR) cybt_platform_log_print(__VA_ARGS__);}
#define BTTASK_TRACE_WARNING(...)  {if (trace_cb.trace_level[CYBT_TRACE_ID_BT_TASK] >= CYBT_TRACE_LEVEL_WARNING) cybt_platform_log_print(__VA_ARGS__);}
#define BTTASK_TRACE_API(...)      {if (trace_cb.trace_level[CYBT_TRACE_ID_BT_TASK] >= CYBT_TRACE_LEVEL_API) cybt_platform_log_print(__VA_ARGS__);}
#define BTTASK_TRACE_EVENT(...)    {if (trace_cb.trace_level[CYBT_TRACE_ID_BT_TASK] >= CYBT_TRACE_LEVEL_EVENT) cybt_platform_log_print(__VA_ARGS__);}
#define BTTASK_TRACE_DEBUG(...)    {if (trace_cb.trace_level[CYBT_TRACE_ID_BT_TASK] >= CYBT_TRACE_LEVEL_DEBUG) cybt_platform_log_print(__VA_ARGS__);}

#define HCITASK_TRACE_ERROR(...)    {if (trace_cb.trace_level[CYBT_TRACE_ID_HCI_TASK] >= CYBT_TRACE_LEVEL_ERROR) cybt_platform_log_print(__VA_ARGS__);}
#define HCITASK_TRACE_WARNING(...)  {if (trace_cb.trace_level[CYBT_TRACE_ID_HCI_TASK] >= CYBT_TRACE_LEVEL_WARNING) cybt_platform_log_print(__VA_ARGS__);}
#define HCITASK_TRACE_API(...)      {if (trace_cb.trace_level[CYBT_TRACE_ID_HCI_TASK] >= CYBT_TRACE_LEVEL_API) cybt_platform_log_print(__VA_ARGS__);}
#define HCITASK_TRACE_EVENT(...)    {if (trace_cb.trace_level[CYBT_TRACE_ID_HCI_TASK] >= CYBT_TRACE_LEVEL_EVENT) cybt_platform_log_print(__VA_ARGS__);}
#define HCITASK_TRACE_DEBUG(...)    {if (trace_cb.trace_level[CYBT_TRACE_ID_HCI_TASK] >= CYBT_TRACE_LEVEL_DEBUG) cybt_platform_log_print(__VA_ARGS__);}

#define HCIDRV_TRACE_ERROR(...)    {if (trace_cb.trace_level[CYBT_TRACE_ID_HCI_DRV] >= CYBT_TRACE_LEVEL_ERROR) cybt_platform_log_print(__VA_ARGS__);}
#define HCIDRV_TRACE_WARNING(...)  {if (trace_cb.trace_level[CYBT_TRACE_ID_HCI_DRV] >= CYBT_TRACE_LEVEL_WARNING) cybt_platform_log_print(__VA_ARGS__);}
#define HCIDRV_TRACE_API(...)      {if (trace_cb.trace_level[CYBT_TRACE_ID_HCI_DRV] >= CYBT_TRACE_LEVEL_API) cybt_platform_log_print(__VA_ARGS__);}
#define HCIDRV_TRACE_EVENT(...)    {if (trace_cb.trace_level[CYBT_TRACE_ID_HCI_DRV] >= CYBT_TRACE_LEVEL_EVENT) cybt_platform_log_print(__VA_ARGS__);}
#define HCIDRV_TRACE_DEBUG(...)    {if (trace_cb.trace_level[CYBT_TRACE_ID_HCI_DRV] >= CYBT_TRACE_LEVEL_DEBUG) cybt_platform_log_print(__VA_ARGS__);}

#define HCILOG_TRACE_ERROR(...)    {if (trace_cb.trace_level[CYBT_TRACE_ID_HCI_LOG] >= CYBT_TRACE_LEVEL_ERROR) cybt_platform_log_print(__VA_ARGS__);}
#define HCILOG_TRACE_WARNING(...)  {if (trace_cb.trace_level[CYBT_TRACE_ID_HCI_LOG] >= CYBT_TRACE_LEVEL_WARNING) cybt_platform_log_print(__VA_ARGS__);}
#define HCILOG_TRACE_API(...)      {if (trace_cb.trace_level[CYBT_TRACE_ID_HCI_LOG] >= CYBT_TRACE_LEVEL_API) cybt_platform_log_print(__VA_ARGS__);}
#define HCILOG_TRACE_EVENT(...)    {if (trace_cb.trace_level[CYBT_TRACE_ID_HCI_LOG] >= CYBT_TRACE_LEVEL_EVENT) cybt_platform_log_print(__VA_ARGS__);}
#define HCILOG_TRACE_DEBUG(...)    {if (trace_cb.trace_level[CYBT_TRACE_ID_HCI_LOG] >= CYBT_TRACE_LEVEL_DEBUG) cybt_platform_log_print(__VA_ARGS__);}

#define MEM_TRACE_ERROR(...)    {if (trace_cb.trace_level[CYBT_TRACE_ID_MEMORY] >= CYBT_TRACE_LEVEL_ERROR) cybt_platform_log_print(__VA_ARGS__);}
#define MEM_TRACE_WARNING(...)  {if (trace_cb.trace_level[CYBT_TRACE_ID_MEMORY] >= CYBT_TRACE_LEVEL_WARNING) cybt_platform_log_print(__VA_ARGS__);}
#define MEM_TRACE_API(...)      {if (trace_cb.trace_level[CYBT_TRACE_ID_MEMORY] >= CYBT_TRACE_LEVEL_API) cybt_platform_log_print(__VA_ARGS__);}
#define MEM_TRACE_EVENT(...)    {if (trace_cb.trace_level[CYBT_TRACE_ID_MEMORY] >= CYBT_TRACE_LEVEL_EVENT) cybt_platform_log_print(__VA_ARGS__);}
#define MEM_TRACE_DEBUG(...)    {if (trace_cb.trace_level[CYBT_TRACE_ID_MEMORY] >= CYBT_TRACE_LEVEL_DEBUG) cybt_platform_log_print(__VA_ARGS__);}

#define PRM_TRACE_ERROR(...)    {if (trace_cb.trace_level[CYBT_TRACE_ID_PRM] >= CYBT_TRACE_LEVEL_ERROR) cybt_platform_log_print(__VA_ARGS__);}
#define PRM_TRACE_WARNING(...)  {if (trace_cb.trace_level[CYBT_TRACE_ID_PRM] >= CYBT_TRACE_LEVEL_WARNING) cybt_platform_log_print(__VA_ARGS__);}
#define PRM_TRACE_API(...)      {if (trace_cb.trace_level[CYBT_TRACE_ID_PRM] >= CYBT_TRACE_LEVEL_API) cybt_platform_log_print(__VA_ARGS__);}
#define PRM_TRACE_EVENT(...)    {if (trace_cb.trace_level[CYBT_TRACE_ID_PRM] >= CYBT_TRACE_LEVEL_EVENT) cybt_platform_log_print(__VA_ARGS__);}
#define PRM_TRACE_DEBUG(...)    {if (trace_cb.trace_level[CYBT_TRACE_ID_PRM] >= CYBT_TRACE_LEVEL_DEBUG) cybt_platform_log_print(__VA_ARGS__);}

#define STACK_TRACE_ERROR(...)    {if (trace_cb.trace_level[CYBT_TRACE_ID_STACK] >= CYBT_TRACE_LEVEL_ERROR) cybt_platform_log_print(__VA_ARGS__);}
#define STACK_TRACE_WARNING(...)  {if (trace_cb.trace_level[CYBT_TRACE_ID_STACK] >= CYBT_TRACE_LEVEL_WARNING) cybt_platform_log_print(__VA_ARGS__);}
#define STACK_TRACE_API(...)      {if (trace_cb.trace_level[CYBT_TRACE_ID_STACK] >= CYBT_TRACE_LEVEL_API) cybt_platform_log_print(__VA_ARGS__);}
#define STACK_TRACE_EVENT(...)    {if (trace_cb.trace_level[CYBT_TRACE_ID_STACK] >= CYBT_TRACE_LEVEL_EVENT) cybt_platform_log_print(__VA_ARGS__);}
#define STACK_TRACE_DEBUG(...)    {if (trace_cb.trace_level[CYBT_TRACE_ID_STACK] >= CYBT_TRACE_LEVEL_DEBUG) cybt_platform_log_print(__VA_ARGS__);}

#define APP_TRACE_ERROR(...)    {if (trace_cb.trace_level[CYBT_TRACE_ID_APP] >= CYBT_TRACE_LEVEL_ERROR) cybt_platform_log_print(__VA_ARGS__);}
#define APP_TRACE_WARNING(...)  {if (trace_cb.trace_level[CYBT_TRACE_ID_APP] >= CYBT_TRACE_LEVEL_WARNING) cybt_platform_log_print(__VA_ARGS__);}
#define APP_TRACE_API(...)      {if (trace_cb.trace_level[CYBT_TRACE_ID_APP] >= CYBT_TRACE_LEVEL_API) cybt_platform_log_print(__VA_ARGS__);}
#define APP_TRACE_EVENT(...)    {if (trace_cb.trace_level[CYBT_TRACE_ID_APP] >= CYBT_TRACE_LEVEL_EVENT) cybt_platform_log_print(__VA_ARGS__);}
#define APP_TRACE_DEBUG(...)    {if (trace_cb.trace_level[CYBT_TRACE_ID_APP] >= CYBT_TRACE_LEVEL_DEBUG) cybt_platform_log_print(__VA_ARGS__);}


/*****************************************************************************
 *                           Function Declarations
 ****************************************************************************/
/**
 * Set the trace level for trace categories id.
 *
 * @param[in]       id    : trace id, CYBT_TRACE_ID_ALL for all categories
 * @param[in]       level : trace level
 *
 * @returns         void
 */
void cybt_platform_set_trace_level(cybt_trace_id_t                id,
                                               cybt_trace_level_t level
                                             );

#else

#define MAIN_TRACE_ERROR(...)
#define MAIN_TRACE_WARNING(...)
#define MAIN_TRACE_API(...)
#define MAIN_TRACE_EVENT(...)
#define MAIN_TRACE_DEBUG(...)

#define SPIF_TRACE_ERROR(...)
#define SPIF_TRACE_WARNING(...)
#define SPIF_TRACE_API(...)
#define SPIF_TRACE_EVENT(...)
#define SPIF_TRACE_DEBUG(...)

#define BTTASK_TRACE_ERROR(...)
#define BTTASK_TRACE_WARNING(...)
#define BTTASK_TRACE_API(...)
#define BTTASK_TRACE_EVENT(...)
#define BTTASK_TRACE_DEBUG(...)

#define HCITASK_TRACE_ERROR(...)
#define HCITASK_TRACE_WARNING(...)
#define HCITASK_TRACE_API(...)
#define HCITASK_TRACE_EVENT(...)
#define HCITASK_TRACE_DEBUG(...)

#define HCIDRV_TRACE_ERROR(...)
#define HCIDRV_TRACE_WARNING(...)
#define HCIDRV_TRACE_API(...)
#define HCIDRV_TRACE_EVENT(...)
#define HCIDRV_TRACE_DEBUG(...)

#define HCILOG_TRACE_ERROR(...)
#define HCILOG_TRACE_WARNING(...)
#define HCILOG_TRACE_API(...)
#define HCILOG_TRACE_EVENT(...)
#define HCILOG_TRACE_DEBUG(...)

#define MEM_TRACE_ERROR(...)
#define MEM_TRACE_WARNING(...)
#define MEM_TRACE_API(...)
#define MEM_TRACE_EVENT(...)
#define MEM_TRACE_DEBUG(...)

#define PRM_TRACE_ERROR(...)
#define PRM_TRACE_WARNING(...)
#define PRM_TRACE_API(...)
#define PRM_TRACE_EVENT(...)
#define PRM_TRACE_DEBUG(...)

#define STACK_TRACE_ERROR(...)
#define STACK_TRACE_WARNING(...)
#define STACK_TRACE_API(...)
#define STACK_TRACE_EVENT(...)
#define STACK_TRACE_DEBUG(...)

#define APP_TRACE_ERROR(...)
#define APP_TRACE_WARNING(...)
#define APP_TRACE_API(...)
#define APP_TRACE_EVENT(...)
#define APP_TRACE_DEBUG(...)
#endif

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif

