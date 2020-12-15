/*
 * FreeRTOS Common IO V0.1.3
 * Copyright (C) 2020 Amazon.com, Inc. or its affiliates.  All Rights Reserved.
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
#define IOT_TIMER_SUCCESS                   ( 0 )    /*!< Timer operation completed successfully. */
#define IOT_TIMER_INVALID_VALUE             ( 1 )    /*!< At least one parameter is invalid. */
#define IOT_TIMER_FUNCTION_NOT_SUPPORTED    ( 2 )    /*!< Timer operation not supported. */
#define IOT_TIMER_NOT_RUNNING               ( 3 )    /*!< Timer not running. */
#define IOT_TIMER_SET_FAILED                ( 4 )    /*!< Timer set failed. */

/**
 * @brief   timer descriptor type defined in the source file.
 */
struct                        IotTimerDescriptor;

/**
 * @brief   IotTimerHandle_t type is the timer handle returned by calling iot_timer_open()
 *          this is initialized in open and returned to caller. Caller must pass this pointer
 *          to the rest of the APIs.
 */
typedef struct IotTimerDescriptor * IotTimerHandle_t;

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
 * @return
 *   - Handle to IotTimerHandle_t on SUCCESS
 *   - NULL if
 *      - lTimerInstance is invalid
 *      - lTimerInstance is already open.
 */
IotTimerHandle_t iot_timer_open( int32_t lTimerInstance );

/*!
 * @brief   iot_timer_set_callback is used to set the callback to be called when
 *          the timer reaches the count (delay) set by the caller.
 *          Callers can set the delay using the iot_timer_delay API.
 *
 * @note Newly set callback overrides the one previously set
 * @note This callback is per handle. Each instance has its own callback.
 *
 * @warning This function silently does nothing if either pxTimerHandle or
 *          xCallback handle are NULL.
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
 * @return
 *   - IOT_TIMER_SUCCESS on success
 *   - IOT_TIMER_INVALID_VALUE if pxTimerHandle is NULL
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
 * @return
 *   - IOT_TIMER_SUCCESS on success
 *   - IOT_TIMER_INVALID_VALUE if pxTimerHandle is NULL.
 *   - IOT_TIMER_FUNCTION_NOT_SUPPORTED, if the free running timer on SoC cant be stopped.
 *   - IOT_TIMER_NOT_RUNNING if iot_timer_start has not been called.
 */
int32_t iot_timer_stop( IotTimerHandle_t const pxTimerHandle );

/*!
 * @brief   iot_timer_get_value is used to get the current timer value in micro seconds.
 *
 * @param[in]   pxTimerHandle    handle to Timer interface returned in
 *                               iot_timer_open()
 * @param[out]  ullMicroSeconds  current timer count in microseconds.
 *
 * @return
 *   - IOT_TIMER_SUCCESS on success
 *   - IOT_TIMER_INVALID_VALUE if pxTimerHandle or ulMicroSeconds pointers are NULL
 *   - IOT_TIMER_NOT_RUNNING if timer hasn't been started.
 */
int32_t iot_timer_get_value( IotTimerHandle_t const pxTimerHandle,
                             uint64_t * ullMicroSeconds );

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
 * <b>Example timer delay execution</b>
 * The callback function is called to signal when the timer reaches the count (delay)
 * set by the caller.
 * @code{c}
 *
 * IotTimerHandle_t xTimerHandle;
 *
 * xTimerHandle = iot_timer_open( ltestIotTimerInstance);
 * // assert( xTimerHandle == NULL );
 *
 * // Set the callback to call prvTimerCallbackFunction() when delay reached.
 * iot_timer_set_callback(xTimerHandle, prvTimerCallbackFunction, NULL);
 *
 * // Set the timer delay to be TIMER_DEFAULT_DELAY_US
 * lRetVal = iot_timer_delay(xTimerHandle, TIMER_DEFAULT_DELAY_US );
 * // assert( lRetVal != IOT_TIMER_SUCCESS );
 *
 * //Start the timer
 * lRetVal = iot_timer_start(xTimerHandle);
 * // assert ( lRetVal != IOT_TIMER_SUCCESS);
 *
 * // Wait for the Delay callback to be called.  Inside of prvTimerCallbackFunction()
 * // the function will use xSemaphoreGive() to signal completion.
 * lRetVal = xSemaphoreTake(IotTimerSemaphore, portMAX_DELAY);
 * // assert( lRetVal != TRUE );
 *
 * lRetVal = iot_timer_close(xTimerHandle);
 * //assert ( lRetVal != IOT_TIMER_SUCCESS);
 * @endcode
 *
 * @return
 *   - IOT_TIMER_SUCCESS on success
 *   - IOT_TIMER_INVALID_VALUE if pxTimerHandle or ulMicroSeconds pointers are NULL
 *   - IOT_TIMER_NOT_RUNNING if timer hasn't been started.
 *   - IOT_TIMER_SET_FAILED on error.
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
 * @return
 *   - IOT_TIMER_SUCCESS on success
 *   - IOT_TIMER_INVALID_VALUE if pxTimerHandle is NULL
 *   - IOT_TIMER_FUNCTION_NOT_SUPPORTED if timer can't be cancelled.
 *   - IOT_TIMER_NOTHING_TO_CANCEL if there is no timer running to cancel.
 */
int32_t iot_timer_cancel( IotTimerHandle_t const pxTimerHandle );

/**
 * @brief   iot_timer_close is used to de-initializes the timer, stops the timer
 *          if it was started and cancels the delay calls, and resets the timer value.
 *
 * @param[in]   pxTimerHandle   handle to Timer interface returned in
 *                              iot_timer_open()
 *
 * @return
 *   - IOT_TIMER_SUCCESS on success
 *   - IOT_TIMER_INVALID_VALUE if
 *      - pxTimerHandle is NULL.
 *      - pxTimerHandle not open (previously closed).
 */
int32_t iot_timer_close( IotTimerHandle_t const pxTimerHandle );

/**
 * @}
 */

#endif /* ifndef _IOT_TIMER_H_ */
