/*
 * Amazon FreeRTOS V1.2.3
 * Copyright (C) 2019 Amazon.com, Inc. or its affiliates.  All Rights Reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 *
 * http://aws.amazon.com/freertos
 * http://www.FreeRTOS.org
 */

/*******************************************************************************
 * @file iot_rtc.h
 *
 * @brief HAL APIs for RTC
 *******************************************************************************
 */

/**
 * @file    iot_rtc.h
 * @brief   This file contains all the RTC HAL API definitions
 */

#ifndef _IOT_RTC_H_
#define _IOT_RTC_H_

/**
 * @defgroup iot_rtc RTC HAL APIs
 * @{
 */

/**
 * @brief Return values used by RTC driver
 */
#define IOT_RTC_SUCCESS                   ( 0 )
#define IOT_RTC_INVALID_VALUE             ( 1 )
#define IOT_RTC_NOT_STARTED               ( 2 )
#define IOT_RTC_GET_FAILED                ( 3 )
#define IOT_RTC_SET_FAILED                ( 4 )
#define IOT_RTC_FUNCTION_NOT_SUPPORTED    ( 5 )

/**
 * @brief RTC driver status values
 */
typedef enum
{
    eRtcTimerStopped,
    eRtcTimerRunning,
    eRtcTimerAlaramTriggered,
    eRtcTimerWakeupTriggered,
} IotRtcStatus_t;

/**
 * @brief RTC date and time format info.
 */
typedef struct IotRtcDatetime
{
    uint8_t ucSecond; /*!< Seconds - range from 0 to 59.*/
    uint8_t ucMinute; /*!< minutes - range from 0 to 59.*/
    uint8_t ucHour;   /*!< hours - range from 0 to 23.*/
    uint8_t ucDay;    /*!< date of month - range from 1 to 31 (depending on month).*/
    uint8_t ucMonth;  /*!< months since January - range from 0 to 11.*/
    uint16_t usYear;  /*!< years since 1900 */
    uint8_t ucWday;   /*!< Week day from Sunday - range from 0 to 6 */
} IotRtcDatetime_t;

/**
 * @brief Ioctl request types.
 */
typedef enum IotRtcIoctlRequest
{
    eSetRtcAlarm,       /*!< Set Alarm, date&time when Alarm need to occur. Takes input type IotRtcDatetime_t */
    eGetRtcAlarm,       /*!< Get Alarm, gives the date&time when Alarm will occur. Returns IotRtcDatetime_t */
    eCancelRtcAlarm,    /*!< Cancel any scheduled Alarm */
    eSetRtcWakeupTime,  /*!< Set Wakeup time  in miliseconds. Maximum number of miliseconds depend on the platform.  Value is uint32_t */
    eGetRtcWakeupTime,  /*!< Get Wakeup time in milli-seconds */
    eCancelRtcWakeup,   /*!< Cancel any scheduled wake-up */
    eGetRtcStatus       /*!< Get the RTC timer status value. Returns IotRtcStatus_t type*/
} IotRtcIoctlRequest_t;

/**
 * @brief   RTC descriptor type defined in the source file.
 */
struct                      IotRtcDescriptor_t;

/**
 * @brief   IotRtcHandle_t type is the RTC handle returned by calling iot_rtc_open()
 *          this is initialized in open and returned to caller. Caller must pass this pointer
 *          to the rest of the APIs.
 */
typedef struct IotRtcDescriptor_t   * IotRtcHandle_t;

/**
 * @brief   RTC notification callback type. This callback is passed
 *          to the driver by using iot_rtc_set_callback API. The callback is
 *          used to get the notifications for Alarm and Wakeup timers.
 *
 * @param[out] xStatus      RTC timer status.
 * @param[in] pvUserContext User Context passed when setting the callback.
 *                          This is not used by the driver, but just passed back to the user
 *                          in the callback.
 */
typedef void ( * IotRtcCallback_t)( IotRtcStatus_t xStatus, void * pvUserContext );

/**
 * @brief   iot_rtc_open is used to initialize the RTC timer.
 *          It usually resets the RTC timer, sets up the clock for RTC etc...
 *
 * @param[in]   lRtcInstance   The instance of the RTC timer to initialize.
 *
 * @return  returns the handle IotRtcHandle_t on success, or NULL on failure.
 */
IotRtcHandle_t iot_rtc_open( int32_t lRtcInstance );

/*!
 * @brief   iot_rtc_set_callback is used to set the callback to be called when alarmTime triggers.
 *          The caller must set the Alarm time using IOCTL to get the callback.
 *
 * @param[in]   pxRtcHandle  handle to RTC driver returned in
 *                          iot_rtc_open()
 * @param[in]   xCallback   callback function to be called.
 * @param[in]   pvUserContext   user context to be passed when callback is called.
 *
 * @return  returns IOT_RTC_SUCCESS on success or returns
 *          one of IOT_RTC_INVALID_VALUE, IOT_RTC_NOT_STARTED on error.
 */
void iot_rtc_set_callback( IotRtcHandle_t const pxRtcHandle,
                           IotRtcCallback_t xCallback,
                           void * pvUserContext );

/**
 * @brief   iot_rtc_ioctl is used to set RTC configuration and
 *          RTC properties like Wakeup time, alarms etc.
 *          Supported IOCTL requests are defined in iot_RtcIoctlRequest_t
 *
 * @param[in]   pxRtcHandle  handle to RTC driver returned in
 *                          iot_rtc_open()
 * @param[in]   xRequest    configuration request of type IotRtcIoctlRequest_t
 * @param[in,out] pvBuffer  buffer holding RTC set and get values.
 *
 * @return  returns IOT_RTC_SUCCESS on success or returns
 *          one of IOT_RTC_INVALID_VALUE, IOT_RTC_NOT_STARTED on error
 *          or IOT_RTC_FUNCTION_NOT_SUPPORTED.
 */
int32_t iot_rtc_ioctl( IotRtcHandle_t const pxRtcHandle,
                       IotRtcIoctlRequest_t xRequest,
                       void * const pvBuffer );

/**
 * @brief   iot_rtc_set_date_time is used to set the current time as a reference in RTC timer counter.
 *
 * @param[in]   pxRtcHandle  handle to RTC driver returned in
 *                          iot_rtc_open()
 * @param[in]   pxDatetime  pointer to IotRtcDatetime_t structure to set the date&time
 *                          to be set in RTC counter.
 *
 * @return  returns IOT_RTC_SUCCESS on success or returns
 *          one of IOT_RTC_INVALID_VALUE, IOT_RTC_SET_FAILED on error.
 */
int32_t iot_rtc_set_datetime( IotRtcHandle_t const pxRtcHandle,
                              const IotRtcDatetime_t * pxDatetime );

/**
 * @brief   iot_rtc_get_datetime is used to get the current time from the RTC counter.
 *          The time must be set first as a reference to get the time.
 *
 * @param[in]   pxRtcHandle  handle to RTC driver returned in
 *                          iot_rtc_open()
 * @param[in]   pxDatetime  pointer to IotRtcDatetime_t structure to get the date&time
 *                          from RTC counter.
 *
 * @return  returns IOT_RTC_SUCCESS on success or returns
 *          one of IOT_RTC_INVALID_VALUE, IOT_RTC_NOT_STARTED on error.
 */
int32_t iot_rtc_get_datetime( IotRtcHandle_t const pxRtcHandle,
                              IotRtcDatetime_t * pxDatetime );

/**
 * @brief   iot_rtc_close is used to de-Initialize RTC Timer.
 *          it resets the RTC timer and may stop the timer.
 *
 * @param[in]   pxRtcHandle  handle to RTC interface.
 *
 * @return  returns IOT_RTC_SUCCESS on success or returns
 *          one of IOT_RTC_INVALID_VALUE on error.
 */
int32_t iot_rtc_close( IotRtcHandle_t const pxRtcHandle );

/**
 * @}
 */

#endif /* ifndef _IOT_RTC_H_ */
