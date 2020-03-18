/* mbed Microcontroller Library
 * Copyright (c) 2019, Arm Limited and affiliates.
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
 */

#ifndef MKTIME_H
#define MKTIME_H

#include <time.h>
#include <stdbool.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    RTC_FULL_LEAP_YEAR_SUPPORT,
    RTC_4_YEAR_LEAP_YEAR_SUPPORT
} rtc_leap_year_support_t;

bool _rtc_is_leap_year(int year, rtc_leap_year_support_t leap_year_support);

bool _rtc_maketime(const struct tm *time, time_t *seconds, rtc_leap_year_support_t leap_year_support);

bool _rtc_localtime(time_t timestamp, struct tm *time_info, rtc_leap_year_support_t leap_year_support);


#ifdef __cplusplus
}
#endif

#endif /* MKTIME_H */

