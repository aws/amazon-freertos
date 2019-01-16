// Copyright 2018 Espressif Systems (Shanghai) PTE LTD
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at

//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#pragma once

#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"

#ifndef PTHREAD_STACK_MIN
#define PTHREAD_STACK_MIN    (configMINIMAL_STACK_SIZE * sizeof(StackType_t))
#endif

/** pthread mutex FreeRTOS wrapper */
typedef struct esp_pthread_mutex {
    SemaphoreHandle_t   sem;        ///< Handle of the task waiting to join
    StaticSemaphore_t   sembuf;     ///< Buffer for static semaphore
    int                 type;       ///< Mutex type. Currently supported PTHREAD_MUTEX_NORMAL and PTHREAD_MUTEX_RECURSIVE
} esp_pthread_mutex_t;

#define PTHREAD_MUTEX_INITIALIZER                  \
    ((pthread_mutex_t) &((esp_pthread_mutex_t) {   \
        .sem  = NULL,                              \
        .type = PTHREAD_MUTEX_NORMAL               \
    }))
