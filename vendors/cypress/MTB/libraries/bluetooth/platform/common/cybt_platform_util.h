/*******************************************************************************
* \file cybt_platform_util.h
*
* \brief
* Define utility API(s), which are to be used by all bt platform files.
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

#ifndef CYBT_PLATFORM_UTIL_H
#define CYBT_PLATFORM_UTIL_H

#include <stdbool.h>

#ifdef __cplusplus
extern "C"
{
#endif

/*****************************************************************************
 *                           Function Declarations
 ****************************************************************************/

/**
 * Get platform configuration structure.
 *
 * @returns  the pointer of platform config structure.
 *
 */
const cybt_platform_config_t* cybt_platform_get_config(void);


/**
 * Get current status of sleep mode.
 *
 * @returns  true : bt sleep mode is already enabled
 *           false: bt sleep mode is disabled
 *
 */
bool cybt_platform_get_sleep_mode_status(void);


#ifdef __cplusplus
} /* extern "C" */
#endif

#endif

