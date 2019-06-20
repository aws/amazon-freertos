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

#ifndef __HAL_WDT_H__
#define __HAL_WDT_H__
#include "hal_platform.h"

#ifdef HAL_WDT_MODULE_ENABLED

/**
 * @addtogroup HAL
 * @{
 * @addtogroup WDT
 * @{
 *
 * @section HAL_WDT_Terms_Chapter Terms and acronyms
 *
 * |Terms                         |Details                                                                 |
 * |------------------------------|------------------------------------------------------------------------|
 * |\b NVIC                       | Nested Vectored Interrupt Controller. NVIC is the interrupt controller of ARM Cortex-M4. For more details, please refer to <a href="http://infocenter.arm.com/help/index.jsp?topic=/com.arm.doc.100166_0001_00_en/ric1417175922867.html"> NVIC introduction in ARM Cortex-M4 Processor Technical Reference Manual</a>.|
 * |\b WDT                        | Watchdog Timer. A watchdog timer is an electronic timer that is used to detect and recover from computer malfunctions. For an introduction to Watchdog timer, please refer to <a href="https://en.wikipedia.org/wiki/Watchdog_timer"> Watchdog timer in Wikipedia</a>. |
 *
 * @section HAL_WDT_Features_Chapter Supported features
 *
 * - \b Support \b reset \b and \b interrupt \b modes. \n
 *    There are two hardware behaviors based on the supported mode configuration after the timer times out or soft reset register is set:
 *  - \b Reset \b mode: In this mode, a hardware reset signal will be triggered by WDT hardware, and the MCU subsystem, MCU peripheral and DSP subsystem
 *                    will be reset. The system will reboot.
 *  - \b Interrupt \b mode: In this mode, an interrupt will be triggered by WDT hardware. This mode is usually
 *                    used for debugging. The MCU subsystem, MCU peripheral and DSP subsystem will not be reset and system will not reboot. You can register a callback to
 *                    record important information for debugging while the interrupt is triggered.
 *  .
 *   You can use #hal_wdt_init() to set the mode and use #hal_wdt_get_mode() to get current mode.\n
 *   \n
 * - \b Support \b software \b reset. \n
 *   For special purposes, you can manually trigger a watchdog reset or watchdog interrupt by calling #hal_wdt_software_reset(). \n
 *   It will trigger WDT reset/interrupt immediately without waiting the timer to timeout. \n
 *   \n
 * - \b Support \b callback \b function \b registration.
 *   When the WDT is configured as interrupt mode, you can register a callback function \n
 *   via #hal_wdt_register_callback(). Your callback function will be called after the WDT triggers an interrupt in the WDT ISR routine.
 *
 * @section HAL_WDT_Architechture_Chapter Software architecture of WDT
 *
 * The software architecture of WDT driver is shown in the below diagrams.
 * -# WDT reset mode architecture : Set WDT to reset mode by calling #hal_wdt_init() function, then call #hal_wdt_enable() function to enable the timer. After the timer is enabled, call #hal_wdt_feed()
 *    function to feed the watchdog continuously to avoid the timer expiration. If the software fails to feed the watchdog on time or  #hal_wdt_software_reset() function is called
* to manually trigger a reset, the WDT hardware will reset the MCU subsystem, MCU peripheral and DSP subsystem and reboot the system.
 * @image html hal_wdt_reset_mode.png
 * -# WDT interrupt mode architecture: Unlike WDT reset mode, in WDT interrupt mode, an interrupt will be triggered, the MCU subsystem, MCU peripheral and DSP subsystem will not be reset and the system will not reboot.
 * @image html hal_wdt_interrupt_mode.png
 *
 *
 * @section HAL_WDT_Driver_Usage_Chapter How to use this driver
 *
 * - Use WDT in a reset mode. \n
 *  - step1: call #hal_wdt_init() to configure the WDT as reset mode.
 *  - step2: call #hal_wdt_enable() to start the watchdog timer.
 *  - step3: call #hal_wdt_feed() to feed the watchdog continuously to avoid the timer expiration.
 *  - step4: call #hal_wdt_disable() if user wants to disable the WDT.
 *  - step5: call #hal_wdt_deinit() if user wants to deinit the WDT.
 *  - sample code:
 *    @code
 *       uint32_t ret_reset_status;
 *       hal_wdt_config_t wdt_config;
 *       wdt_config.mode = HAL_WDT_MODE_RESET;
 *       wdt_config.seconds = 4;
 *
 *       ret_reset_status = hal_wdt_get_reset_status(); //get reset status first, because hal_wdt_init, hal_wdt_enable,
 *       //hal_wdt_disable will clear the reset status. User can get the reason of last reset by this status. ret_reset_status may be used at any time after #hal_wdt_init() is called.
 *       //set WDT as t0 mode.
 *       if(HAL_WDT_STATUS_OK != hal_wdt_init(&wdt_config)) {
 *             //error handle
 *       }
 *       hal_wdt_enable(HAL_WDT_ENABLE_MAGIC); //enable WDT to start the timer.
 *       hal_wdt_feed(HAL_WDT_FEED_MAGIC);  //feed the WDT regularly.
 *       // ...
 *       //hal_wdt_disable(HAL_WDT_DISABLE_MAGIC); //if you want to disable the WDT
 *       //hal_wdt_deinit();// if you want to deinit the WDT.
 *
 *    @endcode
 * - Use WDT in an interrupt mode. \n
 *  - step1: call #hal_wdt_init() to configure the WDT as reset mode.
 *  - step2: call #hal_wdt_register_callback() to register a callback function.
 *  - step3: call #hal_wdt_enable() to start the watchdog timer.
 *  - step4: call #hal_wdt_feed() to feed the watchdog regularly to avoid the timer expiration.
 *  - step5: call #hal_wdt_disable() if user wants to disable the WDT.
 *  - step6: call #hal_wdt_deinit() if user wants to deinit the WDT.
 *  - sample code:
 *    @code
 *       uint32_t ret_reset_status;
 *       hal_wdt_config_t wdt_config;
 *       wdt_config.mode = HAL_WDT_MODE_INTERRUPT;
 *       wdt_config.seconds = 4;
 *
 *       ret_reset_status = hal_wdt_get_reset_status(); //get reset status first, because hal_wdt_init, hal_wdt_enable,
 *       //hal_wdt_disable will clear the reset status. User can get the reason of last reset by this status. ret_reset_status may be used at any time after #hal_wdt_init() is called.
 *       //set WDT as interrupt mode.
 *       if(HAL_WDT_STATUS_OK != hal_wdt_init(&wdt_config)) {
 *             //error handle
 *       }
 *       hal_wdt_register_callback(user_callback); // register a user callback.
 *       hal_wdt_enable(HAL_WDT_ENABLE_MAGIC); //enable WDT to start the timer.
 *       hal_wdt_feed(HAL_WDT_FEED_MAGIC);  //feed the WDT regularly.
 *       //...
 *       //hal_wdt_disable(HAL_WDT_DISABLE_MAGIC); //if you want to disable the WDT
 *       //hal_wdt_deinit();// if you want to deinit the WDT.
 *
 *    @endcode
 *    @code
 *       // Callback function. This function should be registered with #hal_wdt_register_callback().
 *       void wdt_callback(hal_wdt_reset_status_t wdt_reset_status)
 *       {
 *           //Just normal irq, what we do in this function is depends on user's usage, such as for debug.
 *           //ret_callback(wdt_reset_status);
 *       }
 *
 *
 * @endcode
 * - To trigger a WDT reset or interrupt manually, it only needs to call the #hal_wdt_software_reset(). The reset or interrupt will be triggered immediately after the call.
 */



#ifdef __cplusplus
extern "C" {
#endif


/** @defgroup hal_wdt_define Define
  * @{
  */

/** @brief  This macro defines a magic number used in #hal_wdt_enable() function.
  * Please always use this macro as a parameter when you call #hal_wdt_enable().
  */
#define HAL_WDT_ENABLE_MAGIC    (3405691582)


/** @brief  This Macro defines a magic number used in #hal_wdt_disable() function.
  * Please always use this macro as a parameter when you call #hal_wdt_disable().
  */
#define HAL_WDT_DISABLE_MAGIC   (3405643877)


/** @brief  This Macro defines a magic number used in #hal_wdt_feed() function.
  * Please always use this macro as a parameter when you call #hal_wdt_feed().
  */
#define HAL_WDT_FEED_MAGIC      (3735931646)

/**
  * @}
  */


/** @defgroup hal_wdt_enum Enum
  * @{
  */

/** @brief  This enum define the API return type.  */
typedef enum {
    HAL_WDT_STATUS_INVALID_PARAMETER = -2,       /**<  A wrong parameter is given. */
    HAL_WDT_STATUS_INVALID_MAGIC = -1,           /**<  A wrong magic number is given. */
    HAL_WDT_STATUS_OK = 0                        /**<  No error occurred during the function call. */
} hal_wdt_status_t;


/** @brief This enum define the WDT reset status.    */
typedef enum {
    HAL_WDT_TIMEOUT_RESET    = 0, /**<  The WDT reset/interrupt occurred because the timer counts down to 0 . */
    HAL_WDT_SOFTWARE_RESET   = 1, /**<  The WDT reset/interrupt occurred because the software reset register is set . */
    HAL_WDT_NONE_RESET       = 2  /**<  The no WDT reset/interrupt occurred */
} hal_wdt_reset_status_t;


/** @brief This enum defines the WDT mode. see #hal_wdt_init() for more details. See @ref HAL_WDT_Features_Chapter for more about reset mode and interrupt mode. */
typedef enum {
    HAL_WDT_MODE_RESET = 0,        /**<  WDT reset mode. A hardware reset will trigger in this mode if the timer counts down to 0 or software reset register is set. */
    HAL_WDT_MODE_INTERRUPT = 1     /**<  WDT interrupt mode. An interrupt will trigger instead of hardware reset in this mode. */
} hal_wdt_mode_t;


/**
  * @}
  */

/** @defgroup hal_wdt_struct Struct
  * @{
  */

/** @brief This structure defines the init config structure. For more infomation, please refer to #hal_wdt_init() */
typedef struct {
    hal_wdt_mode_t  mode;       /**< Specify watchdog to reset mode or interrupt mode. For more details about the mode, please refer to @ref HAL_WDT_Features_Chapter in overview. */
    uint32_t        seconds;    /**< Specify the watchdog timer timeout value in seconds. The minimum is 1 second and the maximum is 30 seconds.*/
} hal_wdt_config_t;

/**
  * @}
  */

/** @defgroup hal_wdt_typedef Typedef
  * @{
  */


/** @brief  This defines the callback function prototype.
 *          User should register a callback function while WDT is in an
 *          interrupt mode. This function will be called after the watchdog times out in the WDT ISR routine.
 *          For more details about the callback, please refer to #hal_wdt_register_callback().
 *  @param [in] wdt_reset_status: The value is defined in #hal_wdt_reset_status_t. This paramter is given by the driver
 *              to tell the user the reason for current watchdog reset.
 */
typedef void (*hal_wdt_callback_t)(hal_wdt_reset_status_t wdt_reset_status);


/**
  * @}
  */



/**
 * @brief This function is mainly used to initialize the watchdog hardware. It is used to set the WDT to either reset or interrupt modes.
 *  For more details on reset and interrupt modes, please refer to overview in @ref HAL_WDT_Features_Chapter. It’s also used to set the timeout value
 *  of the watchdog timer. This function will not enable the WDT. To enable WDT, the #hal_wdt_enable() must be called.
 * @param[in] wdt_config is the init configuration parameter. For more details about this parameter, please refer to #hal_wdt_config_t.
 * @return    #HAL_WDT_STATUS_OK means success; \n
 *            #HAL_WDT_STATUS_INVALID_PARAMETER means a wrong parameter is given, the parameter must be verified.
 * @sa  #hal_wdt_deinit()
 * @par       Example
 * Sample code please refer to @ref HAL_WDT_Driver_Usage_Chapter
 */
hal_wdt_status_t hal_wdt_init(hal_wdt_config_t *wdt_config);


/**
 * @brief De-initialize the watchdog.
 *        After this function is called, the callback will be cleared and the WDT will be disabled.
 * @return    This function will return #HAL_WDT_STATUS_OK.
 * @sa  #hal_wdt_init()
 * @par       Example
 * Sample code please refer to @ref HAL_WDT_Driver_Usage_Chapter
 */
hal_wdt_status_t hal_wdt_deinit(void);


/**
 * @brief Enable the watchdog timer. After this function call, the watchdog timer will start counting down.
 * @param[in] magic is the magic number of this function. Please always use #HAL_WDT_ENABLE_MAGIC.
 * @return    #HAL_WDT_STATUS_OK means enable success. \n
 *            #HAL_WDT_STATUS_INVALID_MAGIC means an invalid magic number is given.
 * @note      The reset status (hardware status register) will be cleared by this function.
 * @sa  #hal_wdt_disable(), #hal_wdt_get_reset_status(), #hal_wdt_get_enable_status().
 * @par       Example
 * Sample code please refer to @ref HAL_WDT_Driver_Usage_Chapter
 */
hal_wdt_status_t hal_wdt_enable(uint32_t magic);


/**
 * @brief Disable the watchdog timer. After this function, the watchdog timer will stop counting.
 * @param[in] magic is the magic number of this function. Please always use #HAL_WDT_DISABLE_MAGIC.
 * @return    #HAL_WDT_STATUS_OK means enable success. \n
 *            #HAL_WDT_STATUS_INVALID_MAGIC means an invalid magic number is given.
 * @note      The reset status (hardware status register) will be cleared by this function.
 * @sa  #hal_wdt_disable(), #hal_wdt_get_reset_status(), #hal_wdt_get_enable_status().
 * @par       Example
 * Sample code please refer to @ref HAL_WDT_Driver_Usage_Chapter
 */
hal_wdt_status_t hal_wdt_disable(uint32_t magic);


/**
 * @brief This function is used to trigger a watchdog reset manually. After this function is called, the software reset
 *        bit of the hardware status register will be set, you can get this status by using #hal_wdt_get_reset_status().
 * @return    This function will return #HAL_WDT_STATUS_OK.
 * @note      Please be noted that the WDT will immediately trigger hardware reset or interrupt even if the timeout has not been reached.
 * @par       Example
 * Sample code please refer to @ref HAL_WDT_Driver_Usage_Chapter
 */
hal_wdt_status_t hal_wdt_software_reset(void);


/**
 * @brief This function is used to feed the watchdog(restart the watchdog timer).
 *        To avoid the WDT from expiring, this function should be called regularly.
 * @param[in] magic is the magic number of this function. Please always use #HAL_WDT_FEED_MAGIC.
 * @return    #HAL_WDT_STATUS_OK means enable success. \n
 *            #HAL_WDT_STATUS_INVALID_MAGIC means an invalid magic number is given.
 * @par       Example
 * Sample code please refer to @ref HAL_WDT_Driver_Usage_Chapter
 */
hal_wdt_status_t hal_wdt_feed(uint32_t magic);


/**
 * @brief Register a callback function while using interrupt mode.
 * @param[in] wdt_callback is the function pointer to the callback.
 *            This callback is called once the WDT triggers an interrupt in the ISR routine.
 *            WDT driver will only keep the last registered \e wdt_callback. It means if user1 registers callback func1,
 *            later, user2 registers callback func2, the driver will only keep callback func2 and return func1 to user2.
 * @return    This function will return the current callback function. This means that if user1 is the first one to call
 *            this function to register a callback func1, it will receive NULL as a return value. Then, user2 calls this function
 *            to register its own callback func2; it will receive a function pointer func1. User can make use of this character to
 *            implement a \b  callback \b chain.  \n
 *                \n
 *            For example, user1 registers func1 and gets return value NULL, user2 registers
 *            func2 and gets return value func1, and user3 register func3 and get func2. In this case, the WDT driver will keep
 *            the last callback func3. Once the WDT times out and triggers an interrupt, the WDT ISR routine will call func3. As
 *            func3 have recorded func2, it can call func2 in func3. Using similar analogy, func2 can call func1. But as func1 is the first
 *            one registered and the return value must be NULL, func1 should be the last one to be called.
 * @sa  #hal_wdt_init()
 * @par       Example
 * @code
 *       // in user code
 *       ret_callback = hal_wdt_register_callback(wdt_callback);
 * @endcode
 * @code
 *       void wdt_callback(hal_wdt_reset_status_t wdt_reset_status)
 *       {
 *           // other code.
 *           ret_callback(wdt_reset_status);
 *       }
 *
 *
 * @endcode
 */
hal_wdt_callback_t hal_wdt_register_callback(hal_wdt_callback_t wdt_callback);


/**
 * @brief Get the status of last watchdog reset/interrupt. If the WDT failed to be fed in time and has expired,
 *        the status will be #HAL_WDT_TIMEOUT_RESET. If the software register is set by #hal_wdt_software_reset() function, the status
 *        will be #HAL_WDT_SOFTWARE_RESET. Note that the reset status can be cleared by #hal_wdt_init(), #hal_wdt_enable() and #hal_wdt_disable()
 *        functions. It is advised to call this function to get the reset status before #hal_wdt_init(), #hal_wdt_enable() and #hal_wdt_disable()
 *        function calls. If you want to preserve the reset status of the last reset, you should call this function to get the reset status
 *        before calling #hal_wdt_init() and #hal_wdt_enable() and #hal_wdt_disable().
 * @return    #HAL_WDT_TIMEOUT_RESET means the WDT reset is due to the timer timing out; \n
 *            #HAL_WDT_SOFTWARE_RESET means the WDT reset is due to software trigger; \n
 *            #HAL_WDT_NONE_RESET means no WDT reset occurred.
 * @sa   #hal_wdt_init(), #hal_wdt_enable(), #hal_wdt_disable(), #hal_wdt_software_reset().
 * @par       Example
 * @code
 *       ret = hal_wdt_get_reset_status();
 *       if (HAL_WDT_TIMEOUT_RESET == ret) {
             // handle timeout reset;
         } else if (HAL_WDT_SOFTWARE_RESET == ret) {
             // handle software reset;
         } else (HAL_WDT_NONE_RESET == ret) {
             // handle none reset case;
         }
 * @endcode
 */
hal_wdt_reset_status_t hal_wdt_get_reset_status(void);


/**
 * @brief Get the enable status of the WDT.
 * @return    true means the watchdog is enabled and the timer is counting. \n
 *            false means the watchdog is disabled and the timer is stopped.
 * @sa   #hal_wdt_enable(), #hal_wdt_disable().
 * @par       Example
 * @code
 *       ret = hal_wdt_get_enable_status();
 *       if (true == ret) {
 *          // WDT enable
 *       } else {
 *          // WDT disable
 *       }
 * @endcode
 */
bool hal_wdt_get_enable_status(void);


/**
 * @brief Get the current mode of the WDT. For more information on reset and interrupt modes, please refer to @ref HAL_WDT_Features_Chapter
 *        in overview.
 * @return
 *          #HAL_WDT_MODE_RESET means the watchdog is in reset mode. \n
 *          #HAL_WDT_MODE_INTERRUPT means the watchdog is in interrupt mode.
 * @sa   #hal_wdt_init().
 * @par       Example
 * @code
 *       ret = hal_wdt_get_mode();
 *       if (HAL_WDT_MODE_RESET == ret) {
 *          // WDT reset mode
 *       } else if (HAL_WDT_MODE_INTERRUPT == ret) {
 *          // WDT interrupt mode
 *       }
 * @endcode
 */
hal_wdt_mode_t hal_wdt_get_mode(void);


#ifdef __cplusplus
}
#endif


/**
* @}
* @}
*/
#endif /*HAL_WDT_MODULE_ENABLED*/
#endif /* __HAL_WDT_H__ */

