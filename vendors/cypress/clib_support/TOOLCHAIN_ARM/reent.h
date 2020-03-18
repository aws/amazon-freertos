/***************************************************************************//**
* \file reent.h
*
* \brief
* ARM C library port for FreeRTOS
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

// Adapt FreeRTOS's configUSE_NEWLIB_REENTRANT to work with the ARM C library

//! Structure to store thread-local state
struct _reent {
    //! The ARM C library for AArch32 requires 96 bytes for thread-local state
    uint64_t placeholder[12];
};

extern struct _reent *_impure_ptr;          //!< Pointer to struct _reent for current thread

//! Initializes the struct _reent by pointer x
#define _REENT_INIT_PTR(x) memset((x), 0, sizeof(struct _reent))

//! Frees resources associated with the struct _reent
#define _reclaim_reent(x) do { /* Nothing to do */ } while (0)

#ifdef __cplusplus
}
#endif
