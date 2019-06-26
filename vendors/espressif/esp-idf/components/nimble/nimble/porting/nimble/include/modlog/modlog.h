/*
 * Licensed to the Apache Software Foundation (ASF) under one
 * or more contributor license agreements.  See the NOTICE file
 * distributed with this work for additional information
 * regarding copyright ownership.  The ASF licenses this file
 * to you under the Apache License, Version 2.0 (the
 * "License"); you may not use this file except in compliance
 * with the License.  You may obtain a copy of the License at
 *
 *  http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing,
 * software distributed under the License is distributed on an
 * "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
 * KIND, either express or implied.  See the License for the
 * specific language governing permissions and limitations
 * under the License.
 */

#ifndef H_MODLOG_
#define H_MODLOG_

#include <stdio.h>

#include "log/log.h"

#ifdef ESP_PLATFORM
#include "esp_log.h"
#include <stdio.h>
#include <stdarg.h>
#endif

#define MODLOG_MODULE_DFLT 255

#if (MYNEWT_VAL(LOG_LEVEL) > 0)
static inline void
modlog_dummy(const char *msg, ...)
{
    (void)msg;
}
#endif

#ifdef ESP_PLATFORM
#define MODLOG_DEBUG(ml_mod_, ml_msg_, ...) \
    esp_log_write(ESP_LOG_DEBUG, "NimBLE",ml_msg_, ##__VA_ARGS__)

#define MODLOG_INFO(ml_mod_, ml_msg_, ...) \
    esp_log_write(ESP_LOG_INFO, "NimBLE",ml_msg_, ##__VA_ARGS__)

#define MODLOG_WARN(ml_mod_, ml_msg_, ...) \
    esp_log_write(ESP_LOG_WARN, "NimBLE",ml_msg_, ##__VA_ARGS__)

#define MODLOG_ERROR(ml_mod_, ml_msg_, ...) \
    esp_log_write(ESP_LOG_ERROR, "NimBLE",ml_msg_, ##__VA_ARGS__)

#define MODLOG_CRITICAL(ml_mod_, ml_msg_, ...) \
    esp_log_write(ESP_LOG_ERROR, "NimBLE",ml_msg_, ##__VA_ARGS__)

#else

#if (MYNEWT_VAL(LOG_LEVEL) > 1)
#define MODLOG_INFO(ml_mod_, ml_msg_, ...) \
        modlog_dummy((ml_msg_), ##__VA_ARGS__)
#else
#define MODLOG_INFO(ml_mod_, ml_msg_, ...) \
        printf((ml_msg_), ##__VA_ARGS__);
#endif

#if (MYNEWT_VAL(LOG_LEVEL) > 2)
#define MODLOG_WARN(ml_mod_, ml_msg_, ...) \
        modlog_dummy((ml_msg_), ##__VA_ARGS__)
#else
#define MODLOG_WARN(ml_mod_, ml_msg_, ...) \
        printf((ml_msg_), ##__VA_ARGS__);
#endif

#if (MYNEWT_VAL(LOG_LEVEL) > 3)
#define MODLOG_ERROR(ml_mod_, ml_msg_, ...) \
        modlog_dummy((ml_msg_), ##__VA_ARGS__)
#else
#define MODLOG_ERROR(ml_mod_, ml_msg_, ...) \
        printf((ml_msg_), ##__VA_ARGS__);
#endif

#if (MYNEWT_VAL(LOG_LEVEL) > 4)
#define MODLOG_CRITICAL(ml_mod_, ml_msg_, ...) \
        modlog_dummy((ml_msg_), ##__VA_ARGS__)
#else
#define MODLOG_CRITICAL(ml_mod_, ml_msg_, ...) \
        printf((ml_msg_), ##__VA_ARGS__);
#endif

#endif

#define MODLOG(ml_lvl_, ml_mod_, ...) \
    MODLOG_ ## ml_lvl_((ml_mod_), __VA_ARGS__)

#define MODLOG_DFLT(ml_lvl_, ...) \
    MODLOG(ml_lvl_, LOG_MODULE_DEFAULT, __VA_ARGS__)

#endif
