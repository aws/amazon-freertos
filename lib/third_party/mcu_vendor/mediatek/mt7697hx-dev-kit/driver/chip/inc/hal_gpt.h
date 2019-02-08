/* Copyright Statement:
 *
 * (C) 2005-2016  MediaTek Inc. All rights reserved.
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
 * the License Agreement ("Permitted User").  If you are not a Permitted User,
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


#ifndef __HAL_GPT_H__
#define __HAL_GPT_H__
#include "hal_platform.h"

#ifdef HAL_GPT_MODULE_ENABLED

/**
 * @addtogroup HAL
 * @{
 * @addtogroup GPT
 * @{
 * This section introduces the General Purpose Timer(GPT) driver APIs including terms and acronyms, supported features, software architecture, details on how to use this driver, GPT function groups, enums, structures and functions.
 *
 * @section HAL_GPT_Terms_Chapter Terms and acronyms
 *
 * |Terms                         |Details                                                                 |
 * |------------------------------|------------------------------------------------------------------------|
 * |\b GPT                        | General Purpose Timer(GPT) is used as an alarm clock for timing.|
 * |\b NVIC                       | The Nested Vectored Interrupt Controller (NVIC) is the interrupt controller of ARM Cortex-M. For more details, please refer to <a href="http://infocenter.arm.com/help/index.jsp?topic=/com.arm.doc.100166_0001_00_en/ric1417175922867.html"> NVIC introduction in ARM Cortex-M4 Processor Technical Reference Manual </a>.|
 *
 * @section HAL_GPT_Features_Chapter Supported features
 *
 * This controller has a generic design to support various combinations of the timer functionality.
 *
 * - \b Support \b oneshot, \b repeat \b and \b free \b run \b modes.\n
 *   A timer can be configured in one of the modes, oneshot mode, repeat mode and free run mode. The timer mode is configured through the mode configuration.
 *  - \b Oneshot \b mode. In this mode, the interrupt is triggered only once when the timer expires.
 *  - \b Repeat \b mode. In this mode, the interrupt is triggered when the timer expires. At the same time,
 *                       the timer is reloaded again with the same value and starts ticking till the next expiration.
 *                       This pattern repeats until the timer is cancelled.
 *                       This mode is useful for handling functions that are executed periodically.\n
 *  - \b Free \b run \b mode. In this mode, the timer simply becomes a counter. There is no interrupt triggered in this mode.
 *                            The counter keeps running once using #hal_gpt_get_free_run_count(), and never stops.
 *                            Call #hal_gpt_get_free_run_count() to get the stamp of counter ticks.
 *                            This mode is useful when a delay or counting operations are performed.
 *  .
 * @}
 * @}
 */

#ifdef HAL_GPT_FEATURE_US_TIMER
/**
 * @addtogroup HAL
 * @{
 * @addtogroup GPT
 * @{
 * - \b Support \b callback \b function \b registration. \n
 *   A callback function must be registered using #hal_gpt_register_callback(),
 *   then call #hal_gpt_start_timer_ms() or #hal_gpt_start_timer_us() to set the oneshot mode or the repeat mode to start the timer.\n
 *   The callback function is called after the GPT triggers an interrupt in the GPT ISR routine.\n
 * @}
 * @}
 */
#else
/**
 * @addtogroup HAL
 * @{
 * @addtogroup GPT
 * @{
 * - \b Support \b delay \b function. \n
 *   The GPT driver supports delays in milliseconds #hal_gpt_delay_ms() and microseconds #hal_gpt_delay_us().
 *   It utilizes the free run mode of the timer and the polling mechanism to determine if the timeout for the timer is reached.\n
 *   The delay function is a hardware delay. It is for some programs to delay for a specified period without the need for the context switch. \n
 * - \b Support \b callback \b function \b registration. \n
 *   A callback function must be registered using #hal_gpt_register_callback() function,
 *   then call #hal_gpt_start_timer_ms() to set the oneshot mode or the repeat mode to start the timer.\n
 *   The callback function is called after the GPT triggers an interrupt in the GPT ISR routine. \n
 * @}
 * @}
 */
#endif

#ifdef HAL_GPT_SW_GPT_FEATURE
/**
 * @addtogroup HAL
 * @{
 * @addtogroup GPT
 * @{
 * - \b Software \b timer \b supporting \b multiple \b users. \n
 *   The software timer supports multiple users.
 *   It is based on GPT driver, and occupies the hardware GPT port. \n
 *   The software timer uses GPT oneshot mode. The maximum number of users is 32.
 *   The unit of the software timer is milliseconds.
 * @}
 * @}
 */
#endif

/**
 * @addtogroup HAL
 * @{
 * @addtogroup GPT
 * @{
 * @section HAL_GPT_Driver_Usage_Chapter How to use this driver
 *
 * - \b Using \b GPT \b oneshot \b or \b repeat \b mode.\n
 *    Call #hal_gpt_get_running_status() function to manually check the GPT status before using the GPT driver in oneshot or repeat mode.
 *    If the status is #HAL_GPT_RUNNING, user should wait till the status is #HAL_GPT_STOPPED.\n
 *    Call #hal_gpt_init() then #hal_gpt_start_timer_ms() functions to set the GPT mode and expiration time.\n
 *    The timer then starts ticking. Once the pre-set time expires in a oneshot mode, the GPT triggers an interrupt, stops the timer, and calls user's callback function.
 *    In a repeat mode, when the set time expires, the timer is reloaded with the set time and the user callback function is invoked.
 *    To ensure the precise timing, do not overload the callback function. Let the callback return as fast as possible.
 *  - Step1: Call #hal_gpt_get_running_status() to get the current running status.
 *  - Step2: Call #hal_gpt_init() to configure the base environment.
 *  - Step3: Call #hal_gpt_register_callback() to register a callback function.
 *  - Step4: Call #hal_gpt_start_timer_ms() to set the timer mode and the expiration time and start the timer.
 *  - Step5: Call #hal_gpt_stop_timer() to stop the timer.
 *  - Step6: Call #hal_gpt_deinit() to de-initilize the GPT module if it's no longer in use.
 *  - Sample code:
 *    @code
 *       hal_gpt_running_status_t running_status;
 *       hal_gpt_status_t         ret_status;
 *
 *       //Get the running status to check if this port is in use or not.
 *       if (HAL_GPT_STATUS_OK != hal_gpt_get_running_status(HAL_GPT_1,&running_status)) {//Handle the error, if the timer is running.
 *          //error handle
 *       }
 *       //Set the GPT base environment.
 *       if(HAL_GPT_STATUS_OK != hal_gpt_init(HAL_GPT_1)) {
 *          //error handle
 *       }
 *       hal_gpt_register_callback(HAL_GPT_1, user_gpt_callback, &user_data); //Register a user callback.
 *       hal_gpt_start_timer_ms(HAL_GPT_1, 10, HAL_GPT_TIMER_TYPE_ONE_SHOT); //Set 10ms timeout, set oneshot mode and start timer.
 *       //if in repeat mode, please call hal_gpt_start_timer_ms(gpt_port, 10, HAL_GPT_TIMER_TYPE_REPEAT);
 *       //....
 *    @endcode
 *    @code
 *       // Callback function. This function should be registered with #hal_gpt_register_callback().
 *       void user_gpt_callback (void *user_data)
 *       {
 *           // user's handle
 *           hal_gpt_stop_timer(HAL_GPT_1);                                    //Stop the timer.
 *           hal_gpt_deinit(HAL_GPT_1);                                        //If it's no longer in use.
 *       }
 *
 *    @endcode
 * @}
 * @}
 */
#ifdef HAL_GPT_FEATURE_US_TIMER
/**

 * @addtogroup HAL
 * @{
 * @addtogroup GPT
 * @{
 *  - For the timer whose unit of the tick count is microseconds, the #hal_gpt_start_timer_us() works the same way as with #hal_gpt_start_timer_ms().
 * @}
 * @}
 */
#endif

/**
 * @addtogroup HAL
 * @{
 * @addtogroup GPT
 * @{
 * - \b Using \b GPT \b in \b free \b run \b mode. \n
 *    Call #hal_gpt_get_free_run_count() to get the first stamp of timer ticks.\n
 *    Call #hal_gpt_get_free_run_count() again to get the second stamp of timer ticks.\n
 *    If HAL_GPT_CLOCK_SOURCE_32K parameter is in use, the unit of tick is 1/32768 seconds.
 *    The difference between the first and the second stamps is the counting ticks.
 *    In a free run mode, the GPT runs continuously and never stops. It does not provide interrupt feature.
 *    The GPT driver also uses this function implementation similar to the delay function #hal_gpt_delay_ms().\n
 *  - Note: when the GPT works at freerun mode, we need not call #hal_gpt_init() and #hal_gpt_deinit() to init and deinit the GPT.
 *  - Step1: Call #hal_gpt_get_free_run_count() to get the first tick count.
 *  - Step2: Call #hal_gpt_get_free_run_count() to get the second tick count.
 *  - Step3: Call #hal_gpt_get_duration_count() to get the duration time.
 *  - Sample code:
 *    @code
 *       hal_gpt_status_t         ret_status;
 *       uint32_t                 count1, count2, duration_count;
 *       uint32_t                 time;
 *
 *       //Get the first value.
 *       if(HAL_GPT_STATUS_OK != hal_gpt_get_free_run_count(HAL_GPT_CLOCK_SOURCE_32K, &count1)) {
 *          //error handle
 *       }
 *
 *       hal_gpt_delay_ms(10);                               //Delay for 10 ms.
 *
 *       //Get the second value.
 *       if(HAL_GPT_STATUS_OK != hal_gpt_get_free_run_count(HAL_GPT_CLOCK_SOURCE_32K, &count2)) {
 *          //error handle
 *       }
 *
 *       //Calculate count1 and count2 durations, the duration unit is 1/32768 seconds.
 *       if(HAL_GPT_STATUS_OK != hal_gpt_get_duration_count(count1, count2, &duration_count)) {
 *          //error handle
 *       }
 *
 *       time = (duration_count*1000)/32768;                //The time duration in milliseconds.
 *    @endcode
 * @}
 * @}
 */

/**
 * @addtogroup HAL
 * @{
 * @addtogroup GPT
 * @{
 *  -
 *    If the parameter HAL_GPT_CLOCK_SOURCE_1M is used in #hal_gpt_get_free_run_count(),
 *    the unit of the tick count is in microseconds.
 *    For more details about HAL_GPT_CLOCK_SOURCE_1M, please refer to @ref hal_gpt_enum about hal_gpt_clock_source_t.
 *  - Sample code:
 *    @code
 *       hal_gpt_status_t         ret_status;
 *       uint32_t                 count1, count2, duration_count;
 *       uint32_t                 time;
 *
 *       //Get the first value.
 *       if(HAL_GPT_STATUS_OK != hal_gpt_get_free_run_count(HAL_GPT_CLOCK_SOURCE_1M, &count1)) {
 *          //error handle
 *       }
 *
 *       hal_gpt_delay_us(10);                       //Delay for 10 us.
 *
 *       //Get the second value.
 *       if(HAL_GPT_STATUS_OK != hal_gpt_get_free_run_count(HAL_GPT_CLOCK_SOURCE_1M, &count2)) {
 *          //error handle
 *       }
 *
 *       //Calculate count1 and count2 durations, the duration unit is 1 microseconds.
 *       if(HAL_GPT_STATUS_OK != hal_gpt_get_duration_count(count1, count2, &duration_count)) {
 *          //error handle
 *       }
 *
 *       time = duration_count;                      //The time duration in microseconds.
 *    @endcode
 * @}
 * @}
 */

/**
* @addtogroup HAL
* @{
* @addtogroup GPT
* @{
* - \b Using \b the \b GPT \b delay \b function. \n
*   Call #hal_gpt_delay_ms() function to set the delay time.
*   When in a delay, the GPT driver uses free run mode and polls register value until it expires.
*  - Sample code:
*    @code
*       hal_gpt_delay_ms(10);  //Delay for 10 ms.
*    @endcode
* @}
* @}
*/

#ifdef HAL_GPT_FEATURE_US_TIMER
/**
 * @addtogroup HAL
 * @{
 * @addtogroup GPT
 * @{
 *  - Sample code:
 *    @code
 *       hal_gpt_delay_us(10);  //Delay for 10 us.
 *    @endcode
 * @}
 * @}
 */
#endif

#ifdef HAL_GPT_SW_GPT_FEATURE
/**
 * @addtogroup HAL
 * @{
 * @addtogroup GPT
 * @{
 * - \b Using \b software \b timer. \n
 *    Call #hal_gpt_sw_get_timer() to allocate a software timer handle.\n
 *    Call #hal_gpt_sw_start_timer_ms() to register the callback function and start the timer.\n
 *    The unit of the software timer is milliseconds.\n
 *    The software timer is a oneshot timer. To run the software timer continuously, call it again in its callback function.
 *    Call #hal_gpt_sw_stop_timer_ms() to stop the running timer.
 *    If the timer is not running, the #HAL_GPT_STATUS_ERROR is returned.
 *    Call #hal_gpt_sw_get_remaining_time_ms() to get the remaining timeout value at any time.
 *    Call #hal_gpt_sw_free_timer() to free the allocated timer resource.
 *  - Step1: Call #hal_gpt_sw_get_timer() to allocate a software timer handle.
 *  - Step2: Call #hal_gpt_sw_start_timer_ms() to register the callback function and start the timer.
 *  - Step3: Call #hal_gpt_sw_free_timer() to free the allocated timer if it's no longer in use.
 *  - Sample code:
 *    @code
 *       uint32_t                 handle;
 *
 *       //Get the allocated timer handle
 *       if (HAL_GPT_STATUS_OK != hal_gpt_sw_get_timer(&handle) ) {
 *          //error handler
 *       }
 *
 *       //Set the timeout to 10 ms, and register the callback at the same time
 *       if(HAL_GPT_STATUS_OK != hal_gpt_sw_start_timer_ms(handle, 10, user_gpt_callback, NULL)) {
 *          //error handler
 *       }
 *
 *       //....
 *       //Waiting for the timer to timeout.
 *    @endcode
 *    @code
 *       // Callback function.
 *       void user_gpt_callback (void *user_data)
 *       {
 *           // user's handler
 *           hal_gpt_sw_free_timer(handle);                          //If it's no longer in use.
 *       }
 *
 *    @endcode
 * @}
 * @}
 */
#endif

/**
 * @addtogroup HAL
 * @{
 * @addtogroup GPT
 * @{
 */

#ifdef __cplusplus
extern "C" {
#endif

/** @defgroup hal_gpt_typedef Typedef
  * @{
  */

/** @brief  Register a callback function when using GPT oneshot or repeat modes.
  *         The callback is called after the timer expires and triggers an interrupt in the GPT ISR routine.
  *         For more details about the callback function, please refer to #hal_gpt_register_callback().
  * @param[in] user_data is the pointer to the user defined data to pass to the callback function.
  */
typedef void(* hal_gpt_callback_t)(void *user_data);

/**
  * @}
  */
/** @defgroup hal_gpt_enum Enum
  * @{
  */
/** @brief This enum defines the GPT timer type. */
typedef enum {
    HAL_GPT_TIMER_TYPE_ONE_SHOT = 0,                /**< Set the GPT oneshot mode.  */
    HAL_GPT_TIMER_TYPE_REPEAT   = 1                 /**< Set the GPT repeat  mode.  */
} hal_gpt_timer_type_t;


/** @brief This enum defines the GPT status. */
typedef enum {
    HAL_GPT_STATUS_ERROR_PORT_USED   = -4,         /**< The timer has beed used. */
    HAL_GPT_STATUS_ERROR             = -3,         /**< GPT function error occurred. */
    HAL_GPT_STATUS_ERROR_PORT        = -2,         /**< A wrong GPT port is set. */
    HAL_GPT_STATUS_INVALID_PARAMETER = -1,         /**< An invalid parameter. */
    HAL_GPT_STATUS_OK   = 0                        /**< No error occurred during the function call.*/
} hal_gpt_status_t;


/** @brief This enum defines the GPT running status. */
typedef enum {
    HAL_GPT_STOPPED = 0,                            /**< The GPT has stopped. */
    HAL_GPT_RUNNING = 1                             /**< The GPT is running. */
} hal_gpt_running_status_t;


/**
  * @}
  */

/**
 * @brief     This function initializes the GPT base enironment. Call this function if a timer is required.
 * @param[in] gpt_port is the port number.
 * @return    #HAL_GPT_STATUS_OK, if the operation is successful.\n
 *            #HAL_GPT_STATUS_ERROR_PORT, if the gpt_port value is wrong.
 * @sa        #hal_gpt_deinit()
 */
hal_gpt_status_t hal_gpt_init(hal_gpt_port_t gpt_port);


/**
 * @brief     This function de-initializes the GPT timer.
 *            After calling this function, the callback is cleared, the clock power is turned off, interrupts and GPT module are disabled.
 * @param[in] gpt_port is the port number.
 * @return    #HAL_GPT_STATUS_OK, if the operation is successful.\n
 *            #HAL_GPT_STATUS_ERROR_PORT, if the gpt_port value is wrong.
 * @sa        #hal_gpt_init()
 */
hal_gpt_status_t hal_gpt_deinit(hal_gpt_port_t gpt_port);


/**
 * @brief     This function gets the running status of the port as specified.
 * @param[in] gpt_port is the port number.
 * @param[out] running_status is the pointer to the running status after the function returns. For more details about this parameter, please refer to #hal_gpt_running_status_t.
 * @return    #HAL_GPT_STATUS_OK, if the operation is successful.\n
 *            #HAL_GPT_STATUS_ERROR_PORT, if the gpt_port value is wrong.
 */
hal_gpt_status_t hal_gpt_get_running_status(hal_gpt_port_t gpt_port, hal_gpt_running_status_t *running_status);


/**
 * @brief     This function registers a callback function with the timer specified at the port.
 *            The callback can only be registered when the timer, as specified by the port, is in #HAL_GPT_STOPPED state.
 *            If the timer is in #HAL_GPT_RUNNING state, the callback cannot be registered and this function returns #HAL_GPT_STATUS_ERROR.
 * @param[in] gpt_port is the port number.
 * @param[in] callback is the function pointer of the callback.
 *            This callback will be called when the timer expires.
 * @param[in] user_data is the pointer to the user defined data to pass to the callback function.
 * @return    #HAL_GPT_STATUS_OK, if the operation is successful.\n
 *            #HAL_GPT_STATUS_ERROR_PORT, if the gpt_port value is wrong.\n
 *            #HAL_GPT_STATUS_ERROR, if the callback registration failed.
 */
hal_gpt_status_t hal_gpt_register_callback(hal_gpt_port_t gpt_port, hal_gpt_callback_t callback, void *user_data);

/**
 * @brief      This function gets the current count of timer in the free run mode.
 * @param[in]  clock_source is the clock source of the timer in the free run mode.
 *             For more details, please refer to @ref hal_gpt_enum about hal_gpt_clock_source_t.
 * @param[out] count is the user's pointer to a parameter to get the count value.
 * @return     #HAL_GPT_STATUS_OK, if the operation is successful.\n
 */
hal_gpt_status_t hal_gpt_get_free_run_count(hal_gpt_clock_source_t clock_source, uint32_t *count);


/**
 * @brief     This function sets the expiration time in milliseconds and the timer mode, then starts the timer.
 *            The function should only be called while the timer is stopped. An error would be returned if this function is called when the timer is running.
 * @param[in] gpt_port is the port number.
 * @param[in] timeout_time_ms is the expiration time in milliseconds.
 * @param[in] timer_type is the timer mode, such as oneshot or repeat timer mode defined in #hal_gpt_timer_type_t.
 * @return    #HAL_GPT_STATUS_OK, if the operation is successful.\n
 *            #HAL_GPT_STATUS_ERROR_PORT, if the gpt_port value is wrong.
 */
hal_gpt_status_t hal_gpt_start_timer_ms(hal_gpt_port_t gpt_port, uint32_t timeout_time_ms, hal_gpt_timer_type_t timer_type);


/**
 * @brief     This function sets the delay time in milliseconds.
 *            The maximum delay time = 1/GPT_clock * 0xffffffff.
 * @param[in] ms is the delay time in milliseconds.
 * @return    #HAL_GPT_STATUS_OK, if the operation is successful.\n
 *            #HAL_GPT_STATUS_ERROR_PORT, if the gpt_port value is wrong.
 */
hal_gpt_status_t hal_gpt_delay_ms(uint32_t ms);


#ifdef HAL_GPT_FEATURE_US_TIMER
/**
 * @brief     This function sets the expiration time in microseconds and the timer mode, then starts the timer.
 *            The function should only be called while the timer is stopped. An error would be returned if this function is called when the timer is running.
 * @param[in] gpt_port is the port number.
 * @param[in] timeout_time_us is the expiration time in microseconds.
 * @param[in] timer_type is the timer mode, i.e. oneshot or repeat timer mode defined in #hal_gpt_timer_type_t.
 * @return    #HAL_GPT_STATUS_OK, if the operation is successful.\n
 *            #HAL_GPT_STATUS_ERROR_PORT, if the gpt_port value is wrong.
 */
hal_gpt_status_t hal_gpt_start_timer_us(hal_gpt_port_t gpt_port, uint32_t timeout_time_us, hal_gpt_timer_type_t timer_type);
#endif


/**
 * @brief     This function sets delay time in microseconds.
 *            The maximum delay time = 1/GPT_clock * 0xffffffff.
 * @param[in] us is the delay time in microseconds.
 * @return    #HAL_GPT_STATUS_OK, if the operation is successful.\n
 *            #HAL_GPT_STATUS_ERROR_PORT, if the gpt_port value is wrong.
 */
hal_gpt_status_t hal_gpt_delay_us(uint32_t us);

/**
 * @brief     This function stops the timer only for oneshot mode and repeat mode.
 * @param[in] gpt_port is the port number.
 * @return    #HAL_GPT_STATUS_OK, if the timer is stopped successfully.\n
 *            #HAL_GPT_STATUS_ERROR_PORT, if the gpt_port value is wrong.
 */
hal_gpt_status_t hal_gpt_stop_timer(hal_gpt_port_t gpt_port);

/**
 * @brief      This function calculates the count duration. This API is to help user cover count value rollback problem.
 *             For example, if start_count < end_count, it means the count value has grown up from 0xffffffff to 0.
 * @param[in]  start_count is the value of start count.
 * @param[in]  end_count is the value of end count.
 * @param[out] duration_count is the user's parameter pointer to get count duration.
 * @return     #HAL_GPT_STATUS_OK, if the operation is successful.\n
 *             #HAL_GPT_STATUS_INVALID_PARAMETER, if the duration_count is null.
 */
hal_gpt_status_t hal_gpt_get_duration_count(uint32_t start_count, uint32_t end_count, uint32_t *duration_count);

#ifdef HAL_GPT_SW_GPT_FEATURE
/**
 * @brief      This function allocates timer handle.
 * @param[out] handle is an unsigned integer for accessing this timer. It's equivalent to an ID number of the timer.
 * @return     #HAL_GPT_STATUS_OK, if the operation is successful.\n
 *             #HAL_GPT_STATUS_ERROR, if a timer cannot be allocated.
 */
hal_gpt_status_t hal_gpt_sw_get_timer(uint32_t *handle);

/**
 * @brief      This function frees timer.
 * @param[in]  handle is the handle of the timer to be freed.
 * @return     #HAL_GPT_STATUS_OK, if the operation is successful.\n
 *             #HAL_GPT_STATUS_INVALID_PARAMETER, if the handle is invalid. \n
 *             #HAL_GPT_STATUS_ERROR, for all other errors.
 */
hal_gpt_status_t hal_gpt_sw_free_timer(uint32_t handle);

/**
 * @brief      This function starts the software timer.
 * @param[in]  handle is the handle of the timer to be started.
 * @param[in]  timeout_time_ms is the timeout value in milliseconds.
 * @param[in]  callback is the callback to be registered with this timer.
 *             This callback will be called when the timer times out.
 * @param[in]  user_data is the pointer to the user defined data to pass to the callback function.
 * @return     #HAL_GPT_STATUS_OK, if the operation is successful.\n
 *             #HAL_GPT_STATUS_INVALID_PARAMETER, if the parameter is invalid. \n
 *             #HAL_GPT_STATUS_ERROR, for all other errors.
 */
hal_gpt_status_t hal_gpt_sw_start_timer_ms(uint32_t handle, uint32_t timeout_time_ms, hal_gpt_callback_t callback, void *user_data);

/**
 * @brief      This function stops the specified software timer.
 * @param[in]  handle is the handle of the timer to be stopped.
 * @return     #HAL_GPT_STATUS_OK, if the operation is successful.\n
 *             #HAL_GPT_STATUS_INVALID_PARAMETER, if the handle is invalid. \n
 *             #HAL_GPT_STATUS_ERROR, for all other errors.
 */
hal_gpt_status_t hal_gpt_sw_stop_timer_ms(uint32_t handle);

/**
 * @brief      This function gets the remaining timeout value of the specified software timer.
 * @param[in]  handle is the handle of the timer to get the remaining time from.
 * @param[out] remaing_time is the pointer to the value of the remaining timeout, after the return of this function.
 * @return     #HAL_GPT_STATUS_OK, if the operation is successful.\n
 *             #HAL_GPT_STATUS_INVALID_PARAMETER, if the handle is invalid. \n
 *             #HAL_GPT_STATUS_ERROR, if the handle is not allocated.
 */
hal_gpt_status_t hal_gpt_sw_get_remaining_time_ms(uint32_t handle, uint32_t *remaing_time);

#endif

#ifdef __cplusplus
}
#endif

/**
* @}
* @}
*/
#endif /*HAL_GPT_MODULE_ENABLED*/
#endif /* __HAL_GPT_H__ */


