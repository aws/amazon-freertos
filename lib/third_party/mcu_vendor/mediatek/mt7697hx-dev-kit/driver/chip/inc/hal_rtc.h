/* Copyright Statement:
 *
 * (C) 2005-2016 MediaTek Inc. All rights reserved.
 *
 * This software/firmware and related documentation ("MediaTek Software") are
 * protected under relevant copyright laws. The information contained herein
 * is confidential and proprietary to MediaTek Inc. ("MediaTek") and/or its licensors.
 * Without the prior written permission of MediaTek and/or its licensors,
 * any reproduction, modification, use or disclosure of MediaTek Software,
 * and information contained herein, in whole or in part, shall be strictly prohibited.
 * You may only use, reproduce, modify, or distribute (as applicable) MediaTek Software
 * if you have agreed to and been bound by the applicable license agreement with
 * MediaTek ("License Agreement") and been granted explicit permission to do so within
 * the License Agreement ("Permitted User"). If you are not a Permitted User,
 * please cease any access or use of MediaTek Software immediately.
 * BY OPENING THIS FILE, RECEIVER HEREBY UNEQUIVOCALLY ACKNOWLEDGES AND AGREES
 * THAT MEDIATEK SOFTWARE RECEIVED FROM MEDIATEK AND/OR ITS REPRESENTATIVES
 * ARE PROVIDED TO RECEIVER ON AN "AS-IS" BASIS ONLY. MEDIATEK EXPRESSLY DISCLAIMS ANY AND ALL
 * WARRANTIES, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE OR NONINFRINGEMENT.
 * NEITHER DOES MEDIATEK PROVIDE ANY WARRANTY WHATSOEVER WITH RESPECT TO THE
 * SOFTWARE OF ANY THIRD PARTY WHICH MAY BE USED BY, INCORPORATED IN, OR
 * SUPPLIED WITH MEDIATEK SOFTWARE, AND RECEIVER AGREES TO LOOK ONLY TO SUCH
 * THIRD PARTY FOR ANY WARRANTY CLAIM RELATING THERETO. RECEIVER EXPRESSLY ACKNOWLEDGES
 * THAT IT IS RECEIVER'S SOLE RESPONSIBILITY TO OBTAIN FROM ANY THIRD PARTY ALL PROPER LICENSES
 * CONTAINED IN MEDIATEK SOFTWARE. MEDIATEK SHALL ALSO NOT BE RESPONSIBLE FOR ANY MEDIATEK
 * SOFTWARE RELEASES MADE TO RECEIVER'S SPECIFICATION OR TO CONFORM TO A PARTICULAR
 * STANDARD OR OPEN FORUM. RECEIVER'S SOLE AND EXCLUSIVE REMEDY AND MEDIATEK'S ENTIRE AND
 * CUMULATIVE LIABILITY WITH RESPECT TO MEDIATEK SOFTWARE RELEASED HEREUNDER WILL BE,
 * AT MEDIATEK'S OPTION, TO REVISE OR REPLACE MEDIATEK SOFTWARE AT ISSUE,
 * OR REFUND ANY SOFTWARE LICENSE FEES OR SERVICE CHARGE PAID BY RECEIVER TO
 * MEDIATEK FOR SUCH MEDIATEK SOFTWARE AT ISSUE.
 */

#ifndef __HAL_RTC_H__
#define __HAL_RTC_H__
#include "hal_platform.h"

#ifdef HAL_RTC_MODULE_ENABLED
/**
 * @addtogroup HAL
 * @{
 * @addtogroup RTC
 * @{
 * This section introduces the Real-Time Clock (RTC) APIs including terms and acronyms,
 * supported features, software architecture, details on how to use this driver, enums, structures and functions.
 *
 * @section HAL_RTC_Terms_Chapter Terms and acronyms
 *
 * |Terms                   |Details                                                                 |
 * |------------------------------|------------------------------------------------------------------------|
 * |\b RTC                        | Real-Time Clock. A real-time clock (RTC) is a computer clock, mostly in the form of an integrated circuit, that keeps track of the current time. For more information, please refer to <a href="https://en.wikipedia.org/wiki/Real-time_clock"> an introduction to the RTC in Wikipedia </a>.|
 *
 * @section HAL_RTC_Features_Chapter Supported features
 *
 * @}
 * @}
 */
#ifdef HAL_RTC_FEATURE_TIME_CALLBACK
/**
* @addtogroup HAL
* @{
* @addtogroup RTC
* @{
* - \b Support \b date \b and \b time \b information. \n
*   Call #hal_rtc_set_time() to set the RTC current time and #hal_rtc_get_time() to retrieve the RTC current time.
*   Call #hal_rtc_set_time_callback() to register a callback to receive RTC current time change notification. This
*        notification will be triggered in precise RTC time unit configured(like every second, every minute, or every hour etc.).
*   Call #hal_rtc_set_time_notification_period() to set the desired notification period.
*   Then your callback function will be periodically executed according to the period you set.
* - \b Support \b fine \b tuning \b of \b the \b RTC \b timer. \n
*   Call #hal_rtc_set_repeat_calibration() to adjust the RTC time counting speed, if you have a highly accurate
*   reference clock, or if you find out the RTC time is faster or slower. #hal_rtc_set_one_shot_calibration() can be used
*   to fine tune the current RTC time.
* @}
* @}
*/
#else
/**
* @addtogroup HAL
* @{
* @addtogroup RTC
* @{
* - \b Support \b date \b and \b time \b information. \n
*   Call #hal_rtc_set_time() to set the RTC current time and #hal_rtc_get_time() to retrieve the RTC current time.
* @}
* @}
*/
#endif
/**
* @addtogroup HAL
* @{
* @addtogroup RTC
* @{
* - \b Support \b alarm \b notification. \n
*   Call #hal_rtc_set_alarm() to set the RTC alarm time and #hal_rtc_get_alarm() to get the RTC alarm time.
*   To receive alarm notification, please call #hal_rtc_set_alarm_callback() to register a callback function and call
*   #hal_rtc_enable_alarm() to enable the RTC alarm notification. When the RTC current time reaches the RTC alarm
*   time, the callback function you registered will be executed. The alarm notification can be disabled by using #hal_rtc_disable_alarm()
*   function, if it's no longer required.
* - \b Support \b store \b data \b during \b power \b off. \n
*   There are several spare registers in the RTC to store data that are not cleared during a power off, except when the battery is removed.
*
* @section HAL_RTC_Architechture_Chapter Software architecture of RTC
*
* @}
* @}
*/
#ifdef HAL_RTC_FEATURE_TIME_CALLBACK
/**
* @addtogroup HAL
* @{
* @addtogroup RTC
* @{
* The software architecture of the RTC driver is shown in the diagram below.
* - The user needs to call #hal_rtc_init() to initialize the RTC hardware before using the RTC service. The user can set and get the current
*   time or RTC alarm time at any time. RTC supports two notifications, the RTC current time change notification and the
*   alarm notification. The user can use #hal_rtc_set_time_callback() and #hal_rtc_set_alarm_callback() respectively, to register
*   callbacks for these two RTC notifications.
* @image html hal_rtc_archi.png
* @}
* @}
*/
#else
/**
* @addtogroup HAL
* @{
* @addtogroup RTC
* @{
* The software architecture of the RTC driver is shown in the diagram below.
* - The user needs to call #hal_rtc_init() to initialize the RTC hardware before using the RTC service. The user can set and get the current
*   time or RTC alarm time at any time. RTC just supports the alarm notification. The user can use #hal_rtc_set_alarm_callback(), to register
*   callbacks for the RTC notification.
* @image html hal_rtc_archi.png
* @}
* @}
*/
#endif
/**
* @addtogroup HAL
* @{
* @addtogroup RTC
* @{
*
* @section HAL_RTC_Driver_Usage_Chapter How to use this driver
*
* - Set the RTC current time. \n
*  - Step 1: Call #hal_rtc_init() to initialize the RTC hardware, if it hasn't been called after the power went on.
*  - Step 2: Call #hal_rtc_set_time() to set the RTC current time.
*  - sample code:
* Note: the base year of RTC should be 2000, there will be leap year problem if a value other than 2000 is used.
*    @code
*       #define BASE_YEAR 2000
*       set_current_time(uint16_t year, uint8_t mon, uint8_t day, uint8_t hour, uint8_t min, uint8_t sec) {
*           hal_rtc_time_t time;
*
*           if(HAL_RTC_STATUS_OK != hal_rtc_init()) {
*               // Error handler
*           }
*
*           // The user has to define the base year and the RTC year is defined
*           // as an offset. For example, define the base year as 2000 and assign 15 to the RTC year to represent the year of 2015.
*           time.rtc_year = year - BASE_YEAR;
*           time.rtc_mon = mon;
*           time.rtc_day = day;
*           time.rtc_hour = hour;
*           time.rtc_min = min;
*           time.rtc_sec = sec;
*
*           // Set the RTC current time.
*           if(HAL_RTC_STATUS_OK != hal_rtc_set_time(&time)) {
*               // error handle
*           }
*       }
*
*    @endcode
*
* - Get the RTC current time. \n
*  - Step 1: Call #hal_rtc_init() to initialize the RTC hardware if it has not been called after the power went on.
*  - Step 2: Call #hal_rtc_get_time() to get RTC current time.
*  - sample code:
*    @code
*       #define BASE_YEAR 2000
*       get_current_time(uint8_t *year, uint8_t *mon, uint8_t *day, uint8_t *hour, uint8_t *min, uint8_t *sec) {
*           hal_rtc_time_t time;
*
*           if(HAL_RTC_STATUS_OK != hal_rtc_init()) {
*               // Error handler
*           }
*
*           if(HAL_RTC_STATUS_OK != hal_rtc_get_time(&time)) {
*               // Error handler
*           }
*
*           // The user has to define the base year and the RTC year is defined
*           // as an offset. For example, define the base year as 2000 and assign 15 to the RTC year to represent the year of 2015.
*           *year = time.rtc_year + BASE_YEAR;
*           *mon = time.rtc_mon;
*           *day = time.rtc_day;
*           *hour = time.rtc_hour;
*           *min = time.rtc_min;
*           *sec = time.rtc_sec;
*       }
*
*    @endcode
*
* @}
* @}
*/
#ifdef HAL_RTC_FEATURE_TIME_CALLBACK
/**
* @addtogroup HAL
* @{
* @addtogroup RTC
* @{
* - Register a user callback function to handle the RTC current time change. \n
*  - Step 1: Call #hal_rtc_init() to initialize the RTC hardware if it has not been called after the power went on.
*  - Step 2: Call #hal_rtc_set_time_callback() to register a user callback function to handle the RTC current time change, then when a
*           specific RTC current time change occurs, the callback function is executed in an interrupt service routine.
*  - Step 3: Call #hal_rtc_set_time_notification_period() to set a specific RTC current time change notification period.
*  - sample code:
*    @code
*       uint32_t g_user_data;
*
*       set_time_notification_period(hal_rtc_time_notification_period_t period) {
*
*           if(HAL_RTC_STATUS_OK != hal_rtc_init()) {
*               // Error handler
*           }
*           // Register a user callback function to handle the RTC current time change.
*           if(HAL_RTC_STATUS_OK != hal_rtc_set_time_callback(time_change_cb, &g_user_data)) {
*               // Error handler
*           }
*           // Set a specific RTC current time change notification period.
*           if(HAL_RTC_STATUS_OK != hal_rtc_set_time_notification_period(period)) {
*               // Error handler
*           }
*       }
*    @endcode
*    @code
*       void time_change_cb(void *user_data)
*       {
*           // In this case, *user_data is g_user_data.
*           // Apply the RTC functionality, for example, get the RTC current time and update it on the user interface.
*           // Please note, that this callback runs in an interrupt service routine.
*       }
*
*    @endcode
*
* @}
* @}
*/
#endif
/**
* @addtogroup HAL
* @{
* @addtogroup RTC
* @{
* - Set the RTC alarm time. \n
*  - Step 1: Call #hal_rtc_init() to initialize the RTC hardware if it has not been called after the power went on.
*  - Step 2: Call #hal_rtc_set_time() to set the RTC alarm time.
*  - sample code:
*    @code
*       #define BASE_YEAR 2000
*       set_alarm_time(uint16_t year, uint8_t mon, uint8_t day, uint8_t hour, uint8_t min, uint8_t sec) {
*           hal_rtc_time_t alarm;
*
*           if(HAL_RTC_STATUS_OK != hal_rtc_init()) {
*               // Error handler
*           }
*
*           // The user has to define the base year and the RTC year is defined
*           // as an offset. For example, define the base year as 2000 and assign 15 to the RTC year to represent the year of 2015.
*           alarm.rtc_year = year - BASE_YEAR;
*           alarm.rtc_mon = mon;
*           alarm.rtc_day = day;
*           alarm.rtc_hour = hour;
*           alarm.rtc_min = min;
*           alarm.rtc_sec = sec;
*
*           // Set the RTC alarm time.
*           if(HAL_RTC_STATUS_OK != hal_rtc_set_alarm(&alarm)) {
*               // Error handler
*           }
*       }
*
*    @endcode
*
* - Get the RTC alarm time. \n
*  - Step 1: Call #hal_rtc_init() to initialize the RTC hardware if it has not been called after the power went on.
*  - Step 2: Call #hal_rtc_get_alarm() to get the RTC alarm time.
*  - sample code:
*    @code
*       get_alarm_time(uint8_t *year, uint8_t *mon, uint8_t *day, uint8_t *hour, uint8_t *min, uint8_t *sec) {
*           hal_rtc_time_t alarm;
*
*           if(HAL_RTC_STATUS_OK != hal_rtc_init()) {
*               // Error handler
*           }
*
*           if(HAL_RTC_STATUS_OK != hal_rtc_get_time(&alarm)) {
*               // Error handler
*           }
*
*           // The user has to define the base year and the RTC year is defined
*           // as an offset. For example, define the base year as 2000 and assign 15 to the RTC year to represent the year of 2015.
*           *year = time.rtc_year + BASE_YEAR;
*           *mon = alarm.rtc_mon;
*           *day = alarm.rtc_day;
*           *hour = alarm.rtc_hour;
*           *min = alarm.rtc_min;
*           *sec = alarm.rtc_sec;
*       }
*
*    @endcode
*
* - Register a user callback function to handle the RTC alarm. \n
*  - Step 1: Call #hal_rtc_init() to initialize the RTC hardware if it has not been called after the power went on.
*  - Step 2: Call #hal_rtc_set_alarm_callback() to register a user callback function to handle the RTC alarm.
*  - Step 3: Call #hal_rtc_enable_alarm() to enable an alarm notification. A callback function is triggered in an
*           interrupt service routine if an alarm notification is received.
*  - sample code:
*    @code
*       uint32_t g_user_data;
*
*       set_alarm_handle_cb(hal_rtc_time_notification_period_t period) {
*
*           if(HAL_RTC_STATUS_OK != hal_rtc_init()) {
*               // Error handler
*           }
*
*           // Register a callback function to handle the RTC alarm.
*           if(HAL_RTC_STATUS_OK != hal_rtc_set_alarm_callback(alarm_handle_cb, &g_user_data)) {
*               // Error handler
*           }
*
*           // Enable an alarm notification.
*           if(HAL_RTC_STATUS_OK != hal_rtc_enable_alarm()) {
*               // Error handler
*           }
*       }
*    @endcode
*    @code
*       void alarm_handle_cb(void *user_data)
*       {
*           // In this case, *user_data is g_user_data.
*           // Apply the RTC functionality, for example, get the RTC current time and update it on the user interface.
*           // Please note, that this callback runs in an interrupt service routine.
*       }
*
*    @endcode
*
*/
#ifdef HAL_RTC_FEATURE_SLEEP
/**
* @addtogroup HAL
* @{
* @addtogroup RTC
* @{
* - Put the system into sleep if the magic number input is as expected. \n
*  - Call #hal_rtc_sleep() to set the system to sleep mode.
*  - sample code:
*    @code
*       uint32_t magic = 0xBABEBABE; // 0xBABEBABE is the expected magic number.
*       hal_rtc_sleep(magic); // Set the system to sleep mode.
*    @endcode
*
* @}
* @}
*/
#endif



#ifdef __cplusplus
extern "C" {
#endif

/** @defgroup hal_rtc_define Define
  * @{
  */

#ifdef HAL_RTC_FEATURE_SLEEP
/** @brief  This macro defines a magic number used in #hal_rtc_sleep().
  * Please always use this macro as a parameter when you call #hal_rtc_sleep().
  */
#define HAL_RTC_SLEEP_MAGIC      (0xBABEBABE)
/** @brief  This macro defines the maximum size of backup data used in #hal_rtc_set_data(),
  * #hal_rtc_get_data(), #hal_rtc_clear_data functions.
  */
#define HAL_RTC_BACKUP_BYTE_NUM_MAX     (144)
#else
/** @brief  This macro defines the maximum size of backup data used in #hal_rtc_set_data(),
  * #hal_rtc_get_data(), #hal_rtc_clear_data functions.
  */
#define HAL_RTC_BACKUP_BYTE_NUM_MAX     (13)
#endif

/**
  * @}
  */

/** @defgroup hal_rtc_enum Enum
  * @{
  */

/** @brief RTC status */
typedef enum {
    HAL_RTC_STATUS_ERROR = -2,                              /**< An error occurred. */
    HAL_RTC_STATUS_INVALID_PARAM = -1,                      /**< Invalid parameter is given. */
    HAL_RTC_STATUS_OK = 0                                   /**< The operation completed successfully. */
} hal_rtc_status_t;

#ifdef HAL_RTC_FEATURE_TIME_CALLBACK
/** @brief RTC current time change notification period selections. */
typedef enum {
    HAL_RTC_TIME_NOTIFICATION_NONE = 0,                     /**< No need for a time notification. */
    HAL_RTC_TIME_NOTIFICATION_EVERY_SECOND = 1,             /**< Execute a callback function set by #hal_rtc_set_time_notification_period() for every second. */
    HAL_RTC_TIME_NOTIFICATION_EVERY_MINUTE = 2,             /**< Execute a callback function set by #hal_rtc_set_time_notification_period() for every minute. */
    HAL_RTC_TIME_NOTIFICATION_EVERY_HOUR = 3,               /**< Execute a callback function set by #hal_rtc_set_time_notification_period() for every hour. */
    HAL_RTC_TIME_NOTIFICATION_EVERY_DAY = 4,                /**< Execute a callback function set by #hal_rtc_set_time_notification_period() for every day. */
    HAL_RTC_TIME_NOTIFICATION_EVERY_MONTH = 5,              /**< Execute a callback function set by #hal_rtc_set_time_notification_period() for every month. */
    HAL_RTC_TIME_NOTIFICATION_EVERY_YEAR = 6                /**< Execute a callback function set by #hal_rtc_set_time_notification_period() for every year. */
} hal_rtc_time_notification_period_t;
#endif

/**
  * @}
  */

/** @defgroup hal_rtc_struct Struct
  * @{
  */
#ifdef HAL_RTC_FEATURE_TIME_CALLBACK
/** @brief RTC time structure definition. */
typedef struct {
    uint8_t rtc_sec;                                  /**< Seconds after minutes   - [0,59]  */
    uint8_t rtc_min;                                  /**< Minutes after the hour  - [0,59]  */
    uint8_t rtc_hour;                                 /**< Hours after midnight    - [0,23]  */
    uint8_t rtc_day;                                  /**< Day of the month        - [1,31]  */
    uint8_t rtc_mon;                                  /**< Months                  - [1,12]  */
    uint8_t rtc_week;                                 /**< Days in a week          - [0,6]   */
    uint8_t rtc_year;                                 /**< Years                   - [0,127] */
} hal_rtc_time_t;
#else
/** @brief RTC time structure definition. */
typedef struct {
    uint8_t rtc_sec;                                  /**< Seconds after minutes   - [0,59]  */
    uint8_t rtc_min;                                  /**< Minutes after the hour  - [0,59]  */
    uint8_t rtc_hour;                                 /**< Hours after midnight    - [0,23]  */
    uint8_t rtc_day;                                  /**< Day of the month        - [1,31]  */
    uint8_t rtc_mon;                                  /**< Months                  - [1,12]  */
    uint8_t rtc_week;                                 /**< Days in a week          - [0,6]   */
    uint8_t rtc_year;                                 /**< Years                   - [0,99] */
} hal_rtc_time_t;
#endif
/**
  * @}
  */

/** @defgroup hal_rtc_typedef Typedef
  * @{
  */

#ifdef HAL_RTC_FEATURE_TIME_CALLBACK
/** @brief Callback function definition to handle the RTC current time change. \n
 *  @param[in] user_data is a pointer to the data assigned by #hal_rtc_set_time_callback(). \n
 */
typedef void (*hal_rtc_time_callback_t)(void *user_data);
#endif

/** @brief Callback function definition to handle the RTC alarm. \n
 *  @param[in] user_data is a pointer to the data assigned by #hal_rtc_set_alarm_callback(). \n
 */
typedef void (*hal_rtc_alarm_callback_t)(void *user_data);

/**
  * @}
  */

/**
 * @brief Initialize the RTC module. This function must be called once the power is on and before using the RTC service.
 * @return #HAL_RTC_STATUS_OK, the operation completed successfully.
 */
hal_rtc_status_t hal_rtc_init(void);

/**
 * @brief Deinitialize the RTC module.
 * @return #HAL_RTC_STATUS_OK, the operation completed successfully.
 */
hal_rtc_status_t hal_rtc_deinit(void);

/**
 * @brief Set the RTC current time.
 * @param[in] time is a pointer to the #hal_rtc_time_t structure that contains the date and time settings that will be set to the RTC current time.
 * @return #HAL_RTC_STATUS_INVALID_PARAM, an invalid time parameter is given.
 *         #HAL_RTC_STATUS_OK, the operation completed successfully.
 * @par Example
 * Please refer to "Set RTC current time" in @ref HAL_RTC_Driver_Usage_Chapter.
 */
hal_rtc_status_t hal_rtc_set_time(const hal_rtc_time_t *time);

/**
 * @brief Get the RTC current time.
 * @param[out] time is the pointer to a #hal_rtc_time_t structure to store the date and time settings received from the RTC current time.
 * @return #HAL_RTC_STATUS_OK, the operation completed successfully.
 * @par Example
 * Please refer to "Get RTC current time" in @ref HAL_RTC_Driver_Usage_Chapter.
 */
hal_rtc_status_t hal_rtc_get_time(hal_rtc_time_t *time);

#ifdef HAL_RTC_FEATURE_TIME_CALLBACK
/**
 * @brief Set the RTC current time change notification period. The time notification callback function
 *        is different from a common timer or alarm callback function. The common timer or alarm callback function
 *        is only called once at a specific time, but the time notification callback function
 *        is called anytime a specific time notification condition meet. For example, the callback function
 *        runs for each second, if #HAL_RTC_TIME_NOTIFICATION_EVERY_SECOND as the parameter.
 * @param[in] period is the expected time for the RTC current time change notification period.
 * @return #HAL_RTC_STATUS_INVALID_PARAM, an invalid period parameter is given.
 *         #HAL_RTC_STATUS_OK, the operation completed successfully.
 * @sa #hal_rtc_set_time_callback()
 * @par Example
 * Please refer to "Register a user callback function for handling RTC current time change" in @ref HAL_RTC_Driver_Usage_Chapter.
 */
hal_rtc_status_t hal_rtc_set_time_notification_period(hal_rtc_time_notification_period_t period);
#endif

/**
 * @brief Set the RTC alarm time.
 * @param[in] time is a pointer to the hal_rtc_time_t structure that contains the date and time settings to configure the RTC alarm time.
 * @return #HAL_RTC_STATUS_INVALID_PARAM, an invalid alarm time parameter is given.
 *         #HAL_RTC_STATUS_OK, the operation completed successfully.
 * @par Example
 * Please refer to "Set RTC current time" in @ref HAL_RTC_Driver_Usage_Chapter.
 */
hal_rtc_status_t hal_rtc_set_alarm(const hal_rtc_time_t *time);

/**
 * @brief Get the RTC alarm time.
 * @param[out] time is a pointer to the hal_rtc_time_t structure to store the date and time settings received from the RTC alarm time.
 * @return #HAL_RTC_STATUS_OK, the operation completed successfully.
 * @par Example
 * Please refer to "Get RTC alarm time" in @ref HAL_RTC_Driver_Usage_Chapter.
 */
hal_rtc_status_t hal_rtc_get_alarm(hal_rtc_time_t *time);

/**
 * @brief Enable an alarm.
 * @return #HAL_RTC_STATUS_OK, the operation completed successfully.
 * @par Example
 * Please refer to "Register a user callback function for handling alarm" in @ref HAL_RTC_Driver_Usage_Chapter.
 */
hal_rtc_status_t hal_rtc_enable_alarm(void);

/**
 * @brief Disable an alarm. Call this function if the alarm notification is no longer required.
 * @return #HAL_RTC_STATUS_OK, the operation completed successfully.
 */
hal_rtc_status_t hal_rtc_disable_alarm(void);

#ifdef HAL_RTC_FEATURE_TIME_CALLBACK
/**
 * @brief Set the RTC time notification callback, and the callback execution period is set by hal_rtc_set_time_notification_period().
 * @param[in] callback_function is a user-defined callback function.
 * @param[in] user_data is a pointer to the data assigned to callback function.
 * @return #HAL_RTC_STATUS_OK, the operation completed successfully.
 * @sa #hal_rtc_set_time_notification_period()
 * @par Example
 * Please refer to "Set RTC current time" in @ref HAL_RTC_Driver_Usage_Chapter.
 */
hal_rtc_status_t hal_rtc_set_time_callback(hal_rtc_time_callback_t callback_function, void *user_data);
#endif

#ifdef HAL_RTC_FEATURE_SLEEP
/**
 * @brief Set the system into sleep mode. This function should not be called from ISR and the caller is
 *        responsible for making sure the ISR masks won't block the expecting ISRs, which will then
 *        set the system into sleep mode with no option to wake up.
 * @param[in] magic is the magic number of this function. Please always use #HAL_RTC_SLEEP_MAGIC.
 * @return #HAL_RTC_STATUS_ERROR, the RTC hardware is not ready for use.
 *         #HAL_RTC_STATUS_INVALID_PARAM, an invalid magic parameter is given.
 *         #HAL_RTC_STATUS_OK, the operation completed successfully.
 */
hal_rtc_status_t hal_rtc_sleep(uint32_t magic);
#endif

/**
 * @brief Set the alarm callback. This callback is executed if the RTC current and alarm times are the same.
 * @param[in] callback_function is the user-defined callback function.
 * @param[in] user_data is a pointer to the data assigned to callback function.
 * @return #HAL_RTC_STATUS_OK, the operation completed successfully.
 * @sa #hal_rtc_set_alarm(), #hal_rtc_get_alarm(), #hal_rtc_enable_alarm() and #hal_rtc_disable_alarm()
 * @par Example
 * Please refer to "Register a user callback function for handling alarm" in @ref HAL_RTC_Driver_Usage_Chapter.
 */
hal_rtc_status_t hal_rtc_set_alarm_callback(const hal_rtc_alarm_callback_t callback_function, void *user_data);

#ifdef HAL_RTC_FEATURE_CALIBRATION
/**
 * @brief Set the tick value that will be adjusted to the RTC current time. Note, this function should be executed only once per second.
 * @param[in] ticks is the ticks that are adjusted to the RTC current time. The valid value is in a range from -2048(-62.5ms) to 2045(62.4ms). The unit is about 30.52us (1s/32768).
 * @return #HAL_RTC_STATUS_OK, the operation completed successfully.
 * @sa #hal_rtc_get_one_shot_calibration()
 */
hal_rtc_status_t hal_rtc_set_one_shot_calibration(int16_t ticks);

/**
 * @brief Get the tick value that will be adjusted to the RTC current time. Note, ticks will equal to zero after executing this function if one shot calibration is finished.
 * @param[in] ticks is the ticks that are adjusted to the RTC current time. The valid value is in a range from -2048(-62.5ms) to 2045(62.4ms). The unit is about 30.52us (1s/32768).
 * @return #HAL_RTC_STATUS_OK, the operation completed successfully.
 * @sa #hal_rtc_set_one_shot_calibration()
 */
hal_rtc_status_t hal_rtc_get_one_shot_calibration(int16_t *ticks);

/**
 * @brief Set the ticks that are periodically adjusted to the RTC current time per 8 seconds.
 * @param[in] ticks_per_8_seconds is the units that are adjusted to the RTC current time per 8 seconds. The valid value is from -64(-0.244ms) ~ 63(0.24ms). One unit is about 3.81us (1s/32768/8).
 * @return #HAL_RTC_STATUS_OK, the operation completed successfully.
 * @sa #hal_rtc_get_repeat_calibration()
 */
hal_rtc_status_t hal_rtc_set_repeat_calibration(int16_t ticks_per_8_seconds);

/**
 * @brief Get the ticks that are periodically adjusted to the RTC current time per 8 seconds.
 * @param[in] ticks_per_8_seconds is the units that are adjusted to the RTC current time per 8 seconds. The valid value is from -64(-0.244ms) ~ 63(0.24ms). One unit is about 3.81us (1s/32768/8).
 * @return #HAL_RTC_STATUS_OK, the operation completed successfully.
 * @sa #hal_rtc_set_repeat_calibration().
 */
hal_rtc_status_t hal_rtc_get_repeat_calibration(int16_t *ticks_per_8_seconds);
#endif

/**
 * @brief Store data to the RTC registers that won't be cleared even if the system power is off, except when the battery is removed. Note, the size of the backup date is #HAL_RTC_BACKUP_BYTE_NUM_MAX bytes.
 * @param[in] offset is the position of RTC spare registers to store data. The unit is in bytes.
 * @param[in] buf is the address of buffer to store the data write to the RTC spare registers.
 * @param[in] len is the number of data store to the RTC spare registers. The unit is in bytes.
 * @return #HAL_RTC_STATUS_ERROR means the RTC hardware isn't ready for use.
 *         #HAL_RTC_STATUS_OK, the operation completed successfully.
 * @sa #hal_rtc_get_data(), #hal_rtc_clear_data().
 * @par       Example
 * @code
 *
 *
 *       // Write 3 bytes from buf to RTC spare registers bytes 2 to 4.
 *       if(HAL_RTC_STATUS_OK != hal_rtc_set_data(2, buf, 3)) {
 *           // Error handler
 *       }
 * @endcode
 */
hal_rtc_status_t hal_rtc_set_data(uint16_t offset, const char *buf, uint16_t len);

/**
 * @brief Read data from the RTC registers that won't be cleared even if the system power is off, except when the battery is removed. Note, the size of the backup date is #HAL_RTC_BACKUP_BYTE_NUM_MAX bytes.
 * @param[in] offset is the position of RTC spare registers to store data. The unit is in bytes.
 * @param[in] buf is the address of buffer to store the data received from the RTC spare registers.
 * @param[in] len is the number of data read from the RTC spare registers. The unit is in bytes.
 * @return #HAL_RTC_STATUS_ERROR means the RTC hardware isn't ready for use.
 *         #HAL_RTC_STATUS_OK, the operation completed successfully.
 * @sa #hal_rtc_set_data(), #hal_rtc_clear_data().
 * @par       Example
 * @code
 *
 *
 *       // Read 3 bytes from the RTC spare registers byte 2~4 and store the 3 bytes data to buf
 *       if(HAL_RTC_STATUS_OK != hal_rtc_get_data(2, buf, 3)) {
 *           // Error handler
 *       }
 * @endcode
 */
hal_rtc_status_t hal_rtc_get_data(uint16_t offset, char *buf, uint16_t len);

/**
 * @brief Store zero to the RTC registers that won't be cleared even if the system power is off, except when the battery is removed. Note, the size of the backup date is #HAL_RTC_BACKUP_BYTE_NUM_MAX bytes.
 * @param[in] offset is the position of RTC spare registers to store data. The unit is in bytes.
 * @param[in] len is the number of data read from the RTC spare registers. The unit is in bytes.
 * @return #HAL_RTC_STATUS_ERROR means the RTC hardware isn't ready for use.
 *         #HAL_RTC_STATUS_OK, the operation completed successfully.
 * @sa #hal_rtc_set_data(), #hal_rtc_get_data().
 * @par       Example
 * @code
 *
 *
 *       // Set the RTC spare registers (bytes 2 to 4) with zeros.
 *       if(HAL_RTC_STATUS_OK != hal_rtc_clear_data(2, 3)) {
 *           // Error handler
 *       }
 * @endcode
 */
hal_rtc_status_t hal_rtc_clear_data(uint16_t offset, uint16_t len);

/**
 * @brief This function gets the frequency value of 32.768kHz clock source, unit is in Hz.
 * @param[in] frequency is a pointer to store the frequency.
 * @return #HAL_RTC_STATUS_OK, the operation completed successfully.
 */
hal_rtc_status_t hal_rtc_get_f32k_frequency(uint32_t *frequency);

#ifdef __cplusplus
}
#endif

/**
* @}
* @}
*/

#endif /*HAL_RTC_MODULE_ENABLED*/
#endif /*__HAL_RTC_H__*/

