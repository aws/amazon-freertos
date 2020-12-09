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
 * @file iot_watchdog.h
 *
 * @brief HAL APIs for WatchDog driver
 *******************************************************************************
 */

/**
 * @file    iot_watchdog.h
 * @brief   This file contains all the WatchDog HAL API definitions
 */

#ifndef _IOT_WATCHDOG_H_
#define _IOT_WATCHDOG_H_

/**
 * @defgroup iot_watchdog WatchDog HAL APIs
 * @{
 */

/**
 * @brief Return values used by WatchDog driver
 */
#define IOT_WATCHDOG_SUCCESS                   ( 0 )     /*!< Watchdog operation completed successfully.*/
#define IOT_WATCHDOG_INVALID_VALUE             ( 1 )     /*!< At least one parameter is invalid.*/
#define IOT_WATCHDOG_TIME_NOT_SET              ( 2 )     /*!< Watchdog timeout value not set.*/
#define IOT_WATCHDOG_FUNCTION_NOT_SUPPORTED    ( 3 )     /*!< Watchdog operation not supported.*/

/**
 * @brief WatchDog timer status values
 */
typedef enum
{
    eWatchdogTimerStopped,     /*!< WatchDog is stopped */
    eWatchdogTimerRunning,     /*!< WatchDog is running */
    eWatchdogTimerBarkExpired, /*!< WatchDog bark timer expired */
    eWatchdogTimerBiteExpired  /*!< WatchDog bite timer expired */
} IotWatchdogStatus_t;

/**
 * @brief WatchDog timer bite behavior setting
 */
typedef enum
{
    eWatchdogBiteTimerReset,    /*!< Reset the device when WatchDog bite timer expires */
    eWatchdogBiteTimerInterrupt /*!< Generate Interrupt when WatchDog bite timer expires */
} IotWatchdogBiteConfig_t;

/**
 * @brief   WatchDog descriptor type defined in the source file.
 */
struct                           IotWatchdogDescriptor;

/**
 * @brief   IotWatchdogHandle_t type is the WatchDog handle returned by calling iot_watchdog_open()
 *          this is initialized in open and returned to caller. Caller must pass this pointer
 *          to the rest of the APIs.
 */
typedef struct IotWatchdogDescriptor * IotWatchdogHandle_t;

/**
 * @brief Ioctl request types.
 *
 * @note BarkTime is the number of msec before a warning signaled in the form of an interrupt call to the
 * set callback function.
 * @note BiteTime is the number of msec before a critical condition is signaled in the form of an interrupt
 * (If supported and configured), and usually ends with the system being reset.
 *
 * @warning the BarkTime must be less than or equal to the BiteTime
 */
typedef enum
{
    eSetWatchdogBarkTime,     /*!< Set the WatchDog warning time (bark value) as uint32_t in msec.
                               * @warning The maximum value is limited to by the number of bits used for
                               * WatchDog counter in the HW.  ex:  at 24-bit Watchdog counter would have
                               * a maximum of 16,777,215 msec.*/
    eGetWatchdogBarkTime,     /*!< Get the WatchDog warning time (bark value) as uint32_t in msec */
    eSetWatchdogBiteTime,     /*!< Set the WatchDog expire time (bite value) as uint32_t in msec.
                               * @warning The maximum value is limited to by the number of bits used for
                               * WatchDog counter in the HW.  ex:  at 24-bit Watchdog counter would have
                               * a maximum of 16,777,215 msec.*/
    eGetWatchdogBiteTime,     /*!< Get the WatchDog expire time (bite value) as uint32_t in msec */
    eGetWatchdogStatus,       /*!< Returns the WatchDog timer status of type IotWatchdogStatus_t */
    eSetWatchdogBiteBehaviour /*!< Set the WatchDog bite behavior. Takes IotWatchdogBiteConfig_t type */
    /*!< @warning  Not all platforms may support interrupt generation. */
} IotWatchdogIoctlRequest_t;

/**
 * @brief   WatchDog notification callback type. This callback is passed
 *          to the driver by using iot_watchdog_set_callback API. This callback is used for
 *          warning notification when the bark timer or bite timer expires based on the configuration.
 *          Caller can check the status of the WatchDog timer by using eGetStatus IOCTL
 *
 * @warning The callback will only be called if WatchdogBark is supported, or if WatchdogBite behavior
 *          is set to interrupt and is supported.
 *
 * @param[in] pvUserContext User Context passed when setting the callback.
 *                          This is not used by the driver, but just passed back to the user
 *                          in the callback.
 */
typedef void (* IotWatchdogCallback_t)( void * pvUserContext );

/**
 * @brief   iot_watchdog_open is used to initialize the WatchDog,
 *          This function will stop the timer if it was started and resets the timer
 *          if any was configured earlier.
 *
 * @param[in]   lWatchdogInstance   The instance of the WatchDog to initialize.
 *
 * @return
 *   - Handle to IotWatchdogHandle_t on success
 *   - NULL if
 *      - invalid lWatchdogInstance
 *      - instance is already open
 */
IotWatchdogHandle_t iot_watchdog_open( int32_t lWatchdogInstance );

/**
 * @brief   iot_watchdog_start is used to start the WatchDog timer counter.
 *          WatchDog expiry (bite) time must be set before starting the WatchDog counter.
 *          Set the bite time using eSetBiteTime IOCTL.
 *
 * @param[in]   pxWatchdogHandle handle to WatchDog interface returned in
 *                              iot_watchdog_open.
 *
 * @return
 *   - IOT_WATCHDOG_SUCCESS on success
 *   - IOT_WATCHDOG_INVALID_VALUE if pxWatchdogHandle is NULL
 *   - IOT_WATCHDOG_TIME_NOT_SET if bite time has not been set.
 */
int32_t iot_watchdog_start( IotWatchdogHandle_t const pxWatchdogHandle );

/**
 * @brief   iot_watchdog_stop is used to stop and resets the WatchDog timer counter.
 *          After stopping the timer and before starting the timer again,
 *          expireTime must be set.
 *
 * @param[in]   pxWatchdogHandle handle to WatchDog interface returned in
 *                              iot_watchdog_open.
 *
 * @return
 *   - IOT_WATCHDOG_SUCCESS on success
 *   - IOT_WATCHDOG_INVALID_VALUE if pxWatchdogHandle is NULL
 *   - IOT_WATCHDOG_NOT_SUPPORTED if stop operation not supported.
 */
int32_t iot_watchdog_stop( IotWatchdogHandle_t const pxWatchdogHandle );

/**
 * @brief   iot_watchdog_restart is used to restart the WatchDog timer to the
 *          originally programmed values. This function is usually used
 *          once the WatchDog timer (either bark or bite) expired and generated
 *          a callback, so caller can restart the timer to original values to restart
 *          the WatchDog timer. The main difference b/w iot_watchdog_start and iot_watchdog_restart
 *          APIs are, the former requires the time values are set using the IOCTLs and the latter
 *          re-uses the already programmed values and re-programs them. If restart_timer is used
 *          without first setting the timers, it will return an error.
 *
 * @param[in]   pxWatchdogHandle handle to WatchDog interface returned in
 *                              iot_watchdog_open.
 *
 * @return
 *   - IOT_WATCHDOG_SUCCESS on success
 *   - IOT_WATCHDOG_INVALID_VALUE if pxWatchdogHandle is NULL
 *   - IOT_WATCHDOG_TIME_NOT_SET if watchdog bark or bite time have not been set.
 */
int32_t iot_watchdog_restart( IotWatchdogHandle_t const pxWatchdogHandle );

/*!
 * @brief   iot_wathcdog_set_callback is used to set the callback to be called when
 *          bark time reaches the WatchDog counter or if the bite time is configured to
 *          generate interrupt (if supported by HW). The caller must set the timers using
 *          IOCTL and start the timer for the callback to be called back.
 *          Caller must restart the timer when bark timer expires and bite time is configured
 *          to reset the device to avoid the target reset.
 *
 * @note Single callback is used, per instance, if eWatchdogBiteTimerInterrupt has been configured or
 *       if the bark time reaches the WatchDog counter.
 * @note Newly set callback overrides the one previously set
 *
 * @warning If input handle or if callback function is NULL, this function silently takes no action.
 *
 * @param[in]   pxWatchdogHandle handle to WatchDog interface returned in
 *                              iot_watchdog_open.
 * @param[in]   xCallback       The callback function to be called.
 * @param[in]   pvUserContext   The user context to be passed when callback is called.
 */
void iot_watchdog_set_callback( IotWatchdogHandle_t const pxWatchdogHandle,
                                IotWatchdogCallback_t xCallback,
                                void * pvUserContext );

/**
 * @brief   iot_watchdog_ioctl is used to configure the WatchDog timer properties
 *          like the WatchDog timeout value, WatchDog clock, handler for
 *          WatchDog interrupt etc.
 *
 * @param[in]   pxWatchdogHandle handle to WatchDog interface returned in
 *                              iot_watchdog_open.
 * @param[in]   xRequest    configuration request of type IotFlashIoctlRequest_t
 * @param[in,out] pvBuffer   the configuration buffer to hold the request or response of IOCTL.
 *
 * @return
 *   - returns IOT_WATCHDOG_SUCCESS on success
 *   - IOT_WATCHDOG_INVALID_VALUE if
 *      - pxWatchdogHandle is NULL
 *      - xRequest is invalid
 *      - pvBuffer is NULL
 *   - IOT_WATCHDOG_TIME_NOT_SET on error
 *   - IOT_WATCHDOG_FUNCTION_NOT_SUPPORTED
 */
int32_t iot_watchdog_ioctl( IotWatchdogHandle_t const pxWatchdogHandle,
                            IotWatchdogIoctlRequest_t xRequest,
                            void * const pvBuffer );

/**
 * @brief   iot_watchdog_close is used to de-initializes the WatchDog, stops the timer
 *          if it was started and resets the timer value.
 *
 * @param[in]   pxWatchdogHandle handle to WatchDog interface returned in
 *                              iot_watchdog_open.
 * @return
 *   - IOT_WATCHDOG_SUCCESS on success
 *   - IOT_WATCHDOG_INVALID_VALUE if
 *      - pxWatchdogHandle == NULL
 *      - pxWatchdogHandle is not open (previously closed).
 */
int32_t iot_watchdog_close( IotWatchdogHandle_t const pxWatchdogHandle );

/**
 * @}
 */

#endif /* ifndef _IOT_WATCHDOG_H_ */
