/***************************************************************************//**
* \file reent.h
*
* \brief
* IAR C library port for FreeRTOS
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

#include <stdint.h>
#include <string.h>

#ifdef __cplusplus
extern "C" {
#endif

// Adapt FreeRTOS's configUSE_NEWLIB_REENTRANT to work with the IAR C library

//! Structure to store thread-local state
struct _reent {
    void *ptr;  //!< Pointer to thread-local storage area
};

extern struct _reent *_impure_ptr;          //!< Pointer to struct _reent for current thread

//! Initializes a struct _reent
//! \param r struct _reent
void cy_iar_init_reent(struct _reent *r);

//! Initializes the struct _reent by pointer x
#define _REENT_INIT_PTR(x) cy_iar_init_reent(x)

//! Frees resources associated with the struct _reent
//! \param r struct _reent
void _reclaim_reent(struct _reent *r);

#ifdef __cplusplus
}
#endif
