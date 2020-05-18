/***************************************************************************//**
* \file cy_mutex_pool.h
*
* \brief
* Mutex pool implementation for FreeRTOS
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

#pragma once

#include "FreeRTOS.h"
#include <semphr.h>

#ifdef __cplusplus
extern "C" {
#endif

//! Internal use only. Initializes the mutex pool.
void cy_mutex_pool_setup(void);

//! Internal use only. Allocates a mutex.
//! \return SemaphoreHandle_t
SemaphoreHandle_t cy_mutex_pool_create(void);

//! Internal use only. Acquires a mutex.
//! \param m SemaphoreHandle_t
void cy_mutex_pool_acquire(SemaphoreHandle_t m);

//! Internal use only. Releases a mutex.
//! \param m SemaphoreHandle_t
void cy_mutex_pool_release(SemaphoreHandle_t m);

//! Internal use only. Destroys a mutex.
//! \param m SemaphoreHandle_t
void cy_mutex_pool_destroy(SemaphoreHandle_t m);

#ifdef __cplusplus
}
#endif
