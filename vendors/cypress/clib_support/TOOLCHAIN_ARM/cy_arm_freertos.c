/***************************************************************************//**
* \file cy_arm_freertos.h
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

#include <stdatomic.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include "reent.h"
#include <FreeRTOS.h>
#include <semphr.h>
#include <task.h>
#include <cmsis_compiler.h>
#include "cy_mutex_pool.h"

__asm(".global __use_no_semihosting\n\t");

SemaphoreHandle_t cy_ctor_mutex;

void _platform_post_stackheap_init(void)
{
    cy_ctor_mutex = cy_mutex_pool_create();
}

// ARM thread-local state

static struct _reent cy_armlib_global_impure;
struct _reent *_impure_ptr = &cy_armlib_global_impure;

__attribute__((used))
struct _reent *__user_perthread_libspace(void)
{
    return _impure_ptr;
}

// ARM library locking

// The ARM library allocates 4 bytes for each mutex.

__attribute__((used))
int _mutex_initialize(SemaphoreHandle_t *m)
{
    *m = cy_mutex_pool_create();
    return 1;
}

__attribute__((used))
void _mutex_acquire(SemaphoreHandle_t *m)
{
    cy_mutex_pool_acquire(*m);
}

__attribute__((used))
void _mutex_release(SemaphoreHandle_t *m)
{
    cy_mutex_pool_release(*m);
}

__attribute__((used))
void _mutex_free(SemaphoreHandle_t *m)
{
    cy_mutex_pool_destroy(*m);
    *m = NULL;
}

// The __cxa_guard_acquire, __cxa_guard_release, and __cxa_guard_abort
// functions ensure that constructors for static local variables are
// executed exactly once. For more information, see:
// https://itanium-cxx-abi.github.io/cxx-abi/abi.html#obj-ctor

typedef struct {
    atomic_uchar initialized;
    uint8_t acquired;
} cy_cxa_guard_object_t;

int __cxa_guard_acquire(cy_cxa_guard_object_t *guard_object)
{
    int acquired = 0;
    if (0 == atomic_load(&guard_object->initialized)) {
        cy_mutex_pool_acquire(cy_ctor_mutex);
        if (0 == atomic_load(&guard_object->initialized)) {
            acquired = 1;
#ifndef NDEBUG
            if (guard_object->acquired) {
                __BKPT(0);  // acquire called again without release/abort
            }
#endif
            guard_object->acquired = 1;
        }
        else {
            cy_mutex_pool_release(cy_ctor_mutex);
        }
    }
    return acquired;
}

void __cxa_guard_abort(cy_cxa_guard_object_t *guard_object)
{
    if (guard_object->acquired) {
        guard_object->acquired = 0;
        cy_mutex_pool_release(cy_ctor_mutex);
    }
#ifndef NDEBUG
    else {
        __BKPT(0);  // __cxa_guard_abort called when not acquired
    }
#endif
}

void __cxa_guard_release(cy_cxa_guard_object_t *guard_object)
{
    atomic_store(&guard_object->initialized, 1);
    __cxa_guard_abort(guard_object);    // Release mutex
}

// Replace functions that depend on semihosting

__attribute__((used))
void _sys_exit(int unused __attribute__((unused)))
{
    __disable_irq();
    while (1) {
        __WFI();
    }
}

__attribute__((used))
void _ttywrch(int ch)
{
    fputc(ch, stdout);
}

extern int $Super$$_sys_open(const char *, int);

__attribute__((used))
int $Sub$$_sys_open(const char *name, int openmode __attribute__((unused)))
{
    extern const char __stdin_name, __stdout_name, __stderr_name;
    int fd = -1;
    if (0 == strcmp(name, &__stdin_name)) {
        fd = 0; // STDIN_FILENO
    }
    else if (0 == strcmp(name, &__stdout_name)) {
        fd = 1; // STDOUT_FILENO
    }
    else if (0 == strcmp(name, &__stderr_name)) {
        fd = 2; // STDERR_FILENO
    }
    return fd;
}

__attribute__((used))
char *_sys_command_string(char *unused1 __attribute__((unused)), int unused2 __attribute__((unused)))
{
    return NULL;
}
