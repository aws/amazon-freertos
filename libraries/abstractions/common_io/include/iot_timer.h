/*
 * Amazon FreeRTOS Common IO V1.0.0
 * Copyright (C) 2019 Amazon.com, Inc. or its affiliates.  All Rights Reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of
 * this software and associated documentation files (the "Software"), to deal in
 * the Software without restriction, including without limitation the rights to
 * use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
 * the Software, and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 * http://aws.amazon.com/freertos
 * http://www.FreeRTOS.org
 */

/*******************************************************************************
 * @file iot_timer.h
 *
 * @brief HAL APIs for a generic timer driver
 *******************************************************************************
 */

/**
 * @file    iot_timer.h
 * @brief   This file contains all the Timer HAL API definitions
 */

#ifndef _IOT_TIMER_H_
#define _IOT_TIMER_H_

/**
 * @defgroup iot_timer Timer HAL APIs
 * @{
 */

/**
 * @brief Return values used by timer driver
 */
#define IOT_TIMER_SUCCESS                   ( 0 )
#define IOT_TIMER_INVALID_VALUE             ( 1 )
#define IOT_TIMER_FUNCTION_NOT_SUPPORTED    ( 2 )
#define IOT_TIMER_NOT_RUNNING               ( 3 )
#define IOT_TIMER_SET_FAILED                ( 4 )

/**
 * @brief   timer descriptor type defined in the source file.
 */
struct                        IotTimerDescriptor;

/**
 * @brief   IotTimerHandle_t type is the timer handle returned by calling iot_timer_open()
 *          this is initialized in open and returned to caller. Caller must pass this pointer
 *          to the rest of the APIs.
 */
typedef struct IotTimerDescriptor   * IotTimerHandle_t;

/**
 * @brief   timer callback notification type. This callback is used for
 *          notifying the caller when the setup timer expires.
 *
 * @param[in] pvUserContext User Context passed when setting the callback.
 */
typedef void ( * IotTimerCallback_t)( void * pvUserContext );

/**
 * @brief   iot_timer_open is used to initialize the timer.
 *          This function will start the timer.
 *
 * @param[in]   lTimerInstance  instance of the timer to initialize.
 *
 * @return  Handle to IotTimerHandle_t on SUCCESS or NULL on failure.
 */
IotTimerHandle_t iot_timer_open( int32_t lTimerInstance );

/*!
 * @brief   iot_timer_set_callback is used to set the callback to be called when
 *          the timer reaches the count (delay) set by the caller.
 *          Callers can set the delay using the iot_timer_delay API.
 *
 * @param[in]   pxTimerHandle   handle to Timer interface returned in
 *                              iot_timer_open()
 * @param[in]   xCallback       The callback function to be called.
 * @param[in]   pvUserContext   The user context to be passed when callback is called.
 *
 */
void iot_timer_set_callback( IotTimerHandle_t const pxTimerHandle,
                             IotTimerCallback_t xCallback,
                             void * pvUserContext );

/*!
 * @brief   iot_timer_start is used to start the timer counter. This call only makes the
 *          timer counter running, and does not setup any match values etc..
 *
 * @param[in]   pxTimerHandle   handle to Timer interface returned in
 *                              iot_timer_open()
 *
 * @return  returns IOT_TIMER_SUCCESS on success or returns
 *          one of IOT_TIMER_INVALID_VALUE on error.
 */
int32_t iot_timer_start( IotTimerHandle_t const pxTimerHandle );

/*!
 * @brief   iot_timer_stop is used to stop the timer counter if the timer is running.
 *
 * @param[in]   pxTimerHandle    handle to Timer interface returned in
 *                              iot_timer_open()
 *
 * @return  returns IOT_TIMER_SUCCESS on success or returns
 *          one of IOT_TIMER_INVALID_VALUE, IOT_TIMER_FUNCTION_NOT_SUPPORTED on error.
 */
int32_t iot_timer_stop( IotTimerHandle_t const pxTimerHandle );

/*!
 * @brief   iot_timer_get_value is used to get the current timer value in micro seconds.
 *
 * @param[in]   pxTimerHandle   handle to Timer interface returned in
 *                              iot_timer_open()
 * @param[out]  ulMicroSeconds  current timer count in microseconds.
 *
 * @return  returns IOT_TIMER_SUCCESS on success or returns
 *          one of IOT_TIMER_INVALID_VALUE, IOT_TIMER_NOT_RUNNING on error.
 */
int32_t iot_timer_get_value( IotTimerHandle_t const pxTimerHandle,
                             uint64_t * ulMicroSeconds );

/*!
 * @brief   iot_timer_delay is used to set up a delay/wake-up time in microseconds.
 *          The caller can use this API to delay current execution until the specified microSeconds.
 *          A callback is called once the delay is expired (i,e the amount of microseconds is passed
 *          from the time the API is called).
 *          If no callback is registered by the caller, then once the delay is expired, caller cannot be
 *          notified, but this mechanism can be useful to wake up the target from sleep.
 *
 * @param[in]   pxTimerHandle       handle to Timer interface returned in
 *                                  iot_timer_open()
 * @param[in]   ulDelayMicroSeconds delay time in micro seconds
 *
 * @return  returns IOT_TIMER_SUCCESS on success or returns
 *          one of IOT_TIMER_INVALID_VALUE, IOT_TIMER_NOT_RUNNING, IOT_TIMER_SET_FAILED on error.
 */
int32_t iot_timer_delay( IotTimerHandle_t const pxTimerHandle,
                         uint32_t ulDelayMicroSeconds );

/**
 * @brief   iot_timer_cancel is used to cancel any existing delay call.
 *          If a call to iot_timer_delay() was made earlier, calling iot_timer_cancel
 *          will cancel that delay call, so no call-back will be called.
 *
 * @param[in]   pxTimerHandle   handle to Timer interface returned in
 *                              iot_timer_open()
 *
 * @return  returns IOT_TIMER_SUCCESS on success or returns
 *          one of IOT_TIMER_INVALID_VALUE on error
 *          or IOT_TIMER_FUNCTION_NOT_SUPPORTED, IOT_TIMER_NOTHING_TO_CANCEL
 */
int32_t iot_timer_cancel( IotTimerHandle_t const pxTimerHandle );

/**
 * @brief   iot_timer_close is used to de-initializes the timer, stops the timer
 *          if it was started and cancels the delay calls, and resets the timer value.
 *
 * @param[in]   pxTimerHandle   handle to Timer interface returned in
 *                              iot_timer_open()
 *
 * @return  returns IOT_TIMER_SUCCESS on success or returns
 *          one of IOT_TIMER_INVALID_VALUE on error.
 */
int32_t iot_timer_close( IotTimerHandle_t const pxTimerHandle );

/**
 * @}
 */

#endif /* ifndef _IOT_TIMER_H_ */
