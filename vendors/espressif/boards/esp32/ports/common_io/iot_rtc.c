// Copyright 2020 Espressif Systems (Shanghai) PTE LTD
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

#include "stdint.h"
#include <FreeRTOS.h>
#include <freertos/timers.h>
#include "iot_rtc.h"
#include "esp_err.h"
#include "esp_log.h"
#include "sys/time.h"
#include "time.h"

#define USEC_TO_SEC ( 1000000 )
#define INVALID_YEAR ( 0xFFFF )

static const char *TAG = "esp-hal-rtc";
static bool is_rtc_open;

typedef struct {
    IotRtcStatus_t status;
    int32_t timer_status;
    time_t timer_time;
    struct timeval seconds;
    void (*func)(IotRtcStatus_t arg1, void *arg2);
    void *arg2;
} time_ctx_t;

IotRtcHandle_t iot_rtc_open( int32_t lRtcInstance )
{
    if (lRtcInstance != 0) {
        return NULL;
    }
    if (is_rtc_open == true) {
        return NULL;
    }

    time_ctx_t *time_instance = (time_ctx_t *) calloc(1, sizeof(time_ctx_t));
    if (time_instance == NULL) {
        ESP_LOGD(TAG, "Could not allocate memory for the RTC");
        return NULL;
    }
    is_rtc_open = true;
    time_instance->status = eRtcTimerStopped;
    time_instance->timer_status = IOT_RTC_NOT_STARTED;
    return (IotRtcHandle_t)time_instance;
}

int32_t iot_rtc_set_datetime( IotRtcHandle_t const pxRtcHandle,
                              const IotRtcDatetime_t *pxDatetime )
{
    if (pxRtcHandle == NULL || pxDatetime == NULL) {
        return IOT_RTC_INVALID_VALUE;
    }

    if (pxDatetime->usYear == INVALID_YEAR) {
        return IOT_RTC_SET_FAILED;
    }
    if (pxDatetime->ucMonth > 11 || pxDatetime->ucDay > 31|| pxDatetime->ucHour > 23
        || pxDatetime->ucMinute > 59 || pxDatetime->ucSecond > 59) {
        return IOT_RTC_SET_FAILED;
    }
    struct tm tmp;
    tmp.tm_sec = pxDatetime->ucSecond;
    tmp.tm_min = pxDatetime->ucMinute;
    tmp.tm_hour = pxDatetime->ucHour;
    tmp.tm_mday = pxDatetime->ucDay;
    tmp.tm_mon = pxDatetime->ucMonth;
    tmp.tm_year = pxDatetime->usYear;
    tmp.tm_wday = pxDatetime->ucWday;

    time_ctx_t *time_instance = (time_ctx_t *) pxRtcHandle;
    time_instance->seconds.tv_sec = mktime(&tmp);

    if (settimeofday(&time_instance->seconds, NULL) != ESP_OK) {
        return IOT_RTC_SET_FAILED;
    }

    time_instance->status = eRtcTimerRunning;
    time_instance->timer_status = eRtcTimerRunning;
    return IOT_RTC_SUCCESS;
}

int32_t iot_rtc_get_datetime( IotRtcHandle_t const pxRtcHandle,
                              IotRtcDatetime_t *pxDatetime )
{
    if (pxRtcHandle == NULL || pxDatetime == NULL) {
        return IOT_RTC_INVALID_VALUE;
    }

    time_ctx_t *time_instance = (time_ctx_t *) pxRtcHandle;
    if (time_instance->timer_status == IOT_RTC_NOT_STARTED) {
        return IOT_RTC_NOT_STARTED;
    }

    struct timeval get_time;
    if (gettimeofday(&get_time, NULL) != IOT_RTC_SUCCESS) {
        return IOT_RTC_GET_FAILED;
    }

    struct tm *tmp;
    tmp = gmtime(&get_time.tv_sec);
    pxDatetime->usYear = tmp->tm_year;
    pxDatetime->ucMonth = tmp->tm_mon;
    pxDatetime->ucDay = tmp->tm_mday;
    pxDatetime->ucHour = tmp->tm_hour;
    pxDatetime->ucMinute = tmp->tm_min;
    pxDatetime->ucSecond = tmp->tm_sec;
    pxDatetime->ucWday = tmp->tm_wday;

    return IOT_RTC_SUCCESS;
}

static void rtc_cb(void *arg, unsigned int ulparam)
{
    time_ctx_t *time_instance = (time_ctx_t *) arg;
    if (time_instance->func) {
        time_instance->func(time_instance->status, time_instance->arg2);
        time_instance->func = NULL;
        ESP_LOGD(TAG, "Callback invoked %s", __func__);
    }
}

void iot_rtc_set_callback( IotRtcHandle_t const pxRtcHandle,
                           IotRtcCallback_t xCallback,
                           void *pvUserContext )
{
    if (pxRtcHandle == NULL) {
        ESP_LOGD(TAG, "Invalid RTC Handler");
        return;
    }

    time_ctx_t *time_instance = (time_ctx_t *) pxRtcHandle;
    time_instance->func = xCallback;
    time_instance->arg2 = pvUserContext;
}

int32_t iot_rtc_ioctl( IotRtcHandle_t const pxRtcHandle,
                       IotRtcIoctlRequest_t xRequest,
                       void *const pvBuffer )
{
    //pvBuffer can be NULL in few xRequest cases
    if (pxRtcHandle == NULL || xRequest == -1) {
        return IOT_RTC_INVALID_VALUE;
    }

    time_ctx_t *time_instance = (time_ctx_t *) pxRtcHandle;
    struct timeval get_time;
    struct tm tmp;
    switch (xRequest) {
    case eSetRtcAlarm: {

        gettimeofday(&get_time, NULL);

        IotRtcDatetime_t *user_time = (IotRtcDatetime_t *) pvBuffer;

        if (user_time->ucMonth > 11 || user_time->ucDay > 31|| user_time->ucHour > 23
        || user_time->ucMinute > 59 || user_time->ucSecond > 59) {
            return IOT_RTC_SET_FAILED;
        }

        tmp.tm_sec = user_time->ucSecond;
        tmp.tm_min = user_time->ucMinute;
        tmp.tm_hour = user_time->ucHour;
        tmp.tm_mday = user_time->ucDay;
        tmp.tm_mon = user_time->ucMonth;
        tmp.tm_year = user_time->usYear;
        tmp.tm_wday = user_time->ucWday;

        if (time_instance->timer_status == IOT_RTC_NOT_STARTED) {
            return IOT_RTC_NOT_STARTED;
        }
        time_instance->timer_time = mktime(&tmp) - get_time.tv_sec;
        if (time_instance->timer_time <= 0) {
            ESP_LOGD(TAG, "Alarm already set in past");
            return IOT_RTC_SET_FAILED;
        }

        if (xTimerPendFunctionCall(rtc_cb, (void *)pxRtcHandle, 0,
            ((uint64_t)time_instance->timer_time * USEC_TO_SEC)/ portTICK_RATE_MS) != pdTRUE) {
            ESP_LOGE(TAG, "%s: failed to start timer for RTC", __func__);
            return IOT_RTC_NOT_STARTED;
        }
        time_instance->timer_time = mktime(&tmp);
        time_instance->status = eRtcTimerAlarmTriggered;
        return IOT_RTC_SUCCESS;
        break;
    }
    case eGetRtcAlarm: {
        if (time_instance->timer_status == IOT_RTC_NOT_STARTED) {
            return IOT_RTC_NOT_STARTED;
        }
        gettimeofday(&get_time, NULL);

        struct tm *get_alarm ;
        get_alarm = gmtime(&time_instance->timer_time);

        IotRtcDatetime_t *user_time = (IotRtcDatetime_t *) pvBuffer;

        user_time->usYear = get_alarm->tm_year;
        user_time->ucMonth = get_alarm->tm_mon;
        user_time->ucDay = get_alarm->tm_mday;
        user_time->ucHour = get_alarm->tm_hour;
        user_time->ucMinute = get_alarm->tm_min;
        user_time->ucSecond = get_alarm->tm_sec;
        user_time->ucWday = get_alarm->tm_wday;

        return IOT_RTC_SUCCESS;
        break;
    }
    case eSetRtcWakeupTime: {
        if (time_instance->timer_status == IOT_RTC_NOT_STARTED) {
            return IOT_RTC_NOT_STARTED;
        }
        //left blank
        return IOT_RTC_FUNCTION_NOT_SUPPORTED;
        break;
    }
    case eGetRtcWakeupTime: {
        if (time_instance->timer_status == IOT_RTC_NOT_STARTED) {
            return IOT_RTC_NOT_STARTED;
        }
        //left blank
        return IOT_RTC_FUNCTION_NOT_SUPPORTED;
        break;
    }
    case eGetRtcStatus: {
        IotRtcStatus_t *alarm_status = (IotRtcStatus_t *)pvBuffer;
        *alarm_status = time_instance->status;
        return IOT_RTC_SUCCESS;
        break;
    }
    default: {
        return IOT_RTC_FUNCTION_NOT_SUPPORTED;
        break;
    }
    }
}

int32_t iot_rtc_close( IotRtcHandle_t const pxRtcHandle )
{
    if (pxRtcHandle == NULL) {
        ESP_LOGD(TAG, "Invalid RTC Handler");
        return IOT_RTC_INVALID_VALUE;
    }

    if (is_rtc_open == false) {
        return IOT_RTC_INVALID_VALUE;
    }
    is_rtc_open = false;
    free(pxRtcHandle);
    return IOT_RTC_SUCCESS;
}
