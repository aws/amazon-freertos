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

#ifndef __HAL_GPC_H__
#define __HAL_GPC_H__
#include "hal_platform.h"

#ifdef HAL_GPC_MODULE_ENABLED

/**
 * @addtogroup HAL
 * @{
 * @addtogroup GPC
 * @{
 * This section introduces the General Purpose Counter(GPC) APIs including terms and acronyms,
 * supported features, software architecture, details on how to use this driver, GPC function groups, enums, structures and functions.
 *
 *
 * @section HAL_GPC_Terms_Chapter Terms and acronyms
 *
 * |Terms                         |Details                                                                 |
 * |------------------------------|------------------------------------------------------------------------|
 * |\b GPC                        | General Purpose Counter(GPC) captures pulses using raising or falling edge, and counts the number of pulses.|
 * |\b GPIO                       | For an introduction to General Purpose Inputs-Outputs, please refer to the @ref GPIO module in HAL. |
 * |\b NVIC                       | The Nested Vectored Interrupt Controller (NVIC) is the interrupt controller of ARM Cortex-M. For more details, please refer to <a href="http://infocenter.arm.com/help/index.jsp?topic=/com.arm.doc.100166_0001_00_en/ric1417175922867.html"> NVIC introduction in ARM Cortex-M4 Processor Technical Reference Manual </a>.|
 *
 * @section GPC_Features_Chapter Supported features
 *
 * This module is a generic design to capture the number of pulse waveforms. The GPC is to record pulses number.
 * Compared with the EINT to capture pulses, the EINT will generate interrupt, and it occupies the cpu resource. They are two different scenarios.
 *
 * - \b Support \b normal \b mode. \n
 *  In this mode, there is no interrupt, and the register configuration is the default parameter of driver, user cannot modify it. This mode is usually used
 *  for non-urgent pulse-counting. Call #hal_gpc_get_and_clear_count() function to check the counter values, when the GPC module is enabled.\n
 *  \n
 * @}
 * @}
 */
#ifdef HAL_GPC_FEATURE_ADVANCED_CONFIG
/**
 * @addtogroup HAL
 * @{
 * @addtogroup GPC
 * @{
 * - \b Support \b interrupt \b mode. \n
 *   Interrupt mode also means an advanced configuration mode. In this mode, an interrupt will be triggered when counter register value is larger than threshold register value.
 *   This mode is usually used for an immediate pulse-counting. Call #hal_gpc_advanced_config() function to set the register parameter, then register a callback to
 *   record a state or to process a thread when an interrupt is triggered. \n
 *   \n
 * - \b Support \b Callback \b function \b registration.\n
 *   Call #hal_gpc_register_callback() function to register a callback function if the GPC is configured in an interrupt mode.
 *   The callback function is called after the GPC triggers an interrupt in the GPC ISR routine.* \n
 * @}
 * @}
 */
#endif


/**
 * @addtogroup HAL
 * @{
 * @addtogroup GPC
 * @{
 *
 *
 * @section HAL_GPC_Driver_Usage_Chapter How to use this driver
 * - \b Using \b GPC \b normal \b mode.  \n
 *  To use the GPC driver in a normal mode, look up @ref GPIO datasheet to decide which GPIOs should be selected to pinmux to GPC pin,
 *  then call #hal_pinmux_set_function to select the pinmux function. After the pinmux setting, #hal_gpc_get_running_status() must be called to check the
 *  GPC status. If the status is #HAL_GPC_STATUS_BUSY, user should wait till the status becomes #HAL_GPC_STATUS_IDLE. Once the pulses are captured,
 *  always call #hal_gpc_deinit() to release the GPC resource, to enable other users to use the GPC module.
 * - Use GPC with normal mode. \n
 *  - Step1: Call #hal_gpio_init() to initialize the pin. For mode details about #hal_gpio_init please refer to @ref GPIO module in HAL.
 *  - Step2: Call #hal_pinmux_set_function to set GPC pinmux. For mode details about #hal_pinmux_set_function please refer to @ref GPIO module in HAL.
 *  - Step3: Call #hal_gpc_init() to configue the GPC hardware.
 *  - Step4: Call #hal_gpc_enable() to start to capture pulses.
 *  - Step5: Call #hal_gpc_get_and_clear_count() to get GPC counter value.
 *  - Step6: Call #hal_gpc_disable() to stop capturing pulses.
 *  - Step6: Call #hal_gpc_deinit() to to deinitialize the GPC module if it's no longer in use.
 *  - Sample code:
 *    @code
 *       hal_gpc_running_status_t running_status;
 *       hal_gpc_status_t         ret_status;
 *       uint32_t                 count_value;
 *       hal_gpc_config_t         gpc_config;
 *       gpc_config.edge = HAL_GPC_EDGE_RAISING;
 *
 *       hal_gpio_init(HAL_GPIO_33);
 *       //Call hal_pinmux_set_function() to set GPIO pinmux, for more information, please reference hal_pinmux_define.h
 *       //function_index = HAL_GPIO_33_PULSE_CNT, need not to configure pinmux if EPT tool is used.
 *       hal_pinmux_set_function(HAL_GPIO_33, function_index);                 //set pinmux
 *       //initialize GPC hardware with driver default configuration
 *       if(HAL_GPC_STATUS_OK != hal_gpc_init(HAL_GPC_0, &gpc_config) {
 *          //error handle
 *       }
 *       hal_gpc_enable(HAL_GPC_0);                                          //start to capture pulses
 *       //....                                                              //capture pulse
 *       //....
 *       hal_gpc_get_and_clear_count(HAL_GPC_0,&count_value);                //get counter value and clear it.
 *       hal_gpc_disable(HAL_GPC_0);                                         //stop capturing pulses
 *       hal_gpc_deinit(HAL_GPC_0);                                          //if no longer in use.
 *    @endcode
 * @}
 * @}
 */

#ifdef HAL_GPC_FEATURE_ADVANCED_CONFIG
/**
* @addtogroup HAL
* @{
* @addtogroup GPC
* @{
* - Using GPC with interrupt mode. \n

*  To use GPC driver in an interrupt mode, look up @ref GPIO datasheet to decide which GPIOs should be selected to pinmux to GPC pin,
*  then call #hal_pinmux_set_function to select the pinmux function. Call #hal_gpc_get_running_status() function to check the
*  GPC status. If the status is #HAL_GPC_STATUS_BUSY, wait till the status becomes #HAL_GPC_STATUS_IDLE.
*  Call #hal_gpc_init() to setup the environment. Manually configure the parameter settings for debounce clock source, debouce time and
*  interrupt threshold value by calling #hal_gpc_advanced_config() function. Use #hal_gpc_register_callback()
*  to register a callback function in the ISR routine, then use #hal_gpc_interrupt_enable() function to enable the interrupt mode.
*  If the counter value is larger than the threshold value, an interrupt is triggered. In your callback, use #hal_gpc_get_and_clear_count() to obtain counter register
*  value, and clear it. Call #hal_gpc_set_threshold() function to change the interrupt threshold during GPC is running or not.
*  Once the pulses are captured, always call #hal_gpc_deinit() function to release the GPC resource, to enable other users to use the GPC module. Steps are shown below:
*  - Step1: Call #hal_gpio_init() to initialize the pin. For mode details about #hal_gpio_init please refer to @ref GPIO module in HAL.
*  - Step2: Call #hal_pinmux_set_function to set the GPC pinmux. For more details about #hal_pinmux_set_function, please refer to @ref GPIO API document.
*  - Step3: Call #hal_gpc_get_running_status() to get the current running status.
*  - Step4: Call #hal_gpc_init() to configrue the GPC hardware.
*  - Step5: Call #hal_gpc_advanced_config() to set parameter.
*  - Step6: Call #hal_gpc_register_callback() to register user's callback function.
*  - Step7: Call #hal_gpc_interrupt_enable() to enable the GPC interrupt.
*  - Step8: Call #hal_gpc_enable() to start to caputres pulses.
*  - Step9: Call #hal_gpc_get_and_clear_count() to get GPC counter value.
*  - Step10: Call #hal_gpc_disable() to stop capturing pulses.
*  - Step11: Call #hal_gpc_interrupt_disable() to disable the GPC interrupt.
*  - Step12: Call #hal_gpc_deinit() to de-initilaize the GPC module if GPC no longer in use.
*  - Sample code:
*    @code
*       hal_gpc_running_status_t running_status;
*       hal_gpc_advanced_config_t gpc_advanced_config;
*
*       hal_gpio_init(HAL_GPIO_33);
*       //Call hal_pinmux_set_function() to set GPIO pinmux, for more information, please reference hal_pinmux_define.h
*       //function_index = HAL_GPIO_33_PULSE_CNT, need not to configure pinmux if EPT tool is used.
*       hal_pinmux_set_function(HAL_GPIO_33, function_index);              //set pinmux
*
*       
*       if( ret_status != hal_gpc_init(HAL_GPC_0)) {                       //set GPC hardware.
*          //error handle
*       }
*       gpc_advanced_config.clock_source =  HAL_GPC_CLOCK_26MHZ;           //set debounce clock source as 26Mhz
*       gpc_advanced_config.debounce     =  50;                            //set debounce tick is 50, so debounce time = 50*(1/26Mhz)
*       gpc_advanced_config.threshold    =  100;                           //set threshold value as 100 means when counter reaches 100, an interrupt is triggered.
*       hal_gpc_advanced_config(HAL_GPC_0, &gpc_advanced_config);
*       hal_gpc_register_callback(HAL_GPC_0, user_gpc_callback, &user_data); //register a user callback.
*
*       hal_gpc_interrupt_enable(HAL_GPC_0);                                //enable interrupt
*       hal_gpc_enable(HAL_GPC_0)                                           //start to capture pulses 
*       //....                                                              //capture pulse
*       //If user want to change the threshold value, executing the following code.
*       //hal_gpc_interrupt_disble(HAL_GPC_0);                              //disable interrupt
*       //gpc_advanced_config.threshold    =  200;
*       //hal_gpc_set_threshold(HAL_GPC_0, &gpc_advanced_config.threshold); //change threshold value as 200 means when counter reaches 200, an interrupt is triggered.
*       //hal_gpc_enable(HAL_GPC_0)                                         //enable GPC module
*       //....                                                              //capture pulse
*
*    @endcode
*    @code
*       // Callback function sample code. This function should be passed to driver while call hal_gpc_register_callback().
*       void user_gpc_callback (void *user_data)
*       {  //user code.
*          //...
*          hal_gpc_get_and_clear_count(HAL_GPC_0);                          //get counter value and clear, this step is usually implemented done in user's callback.
*          //hal_gpc_disable(HAL_GPC_0);                                      //disable GPC module if it's no longer in use.
*          //hal_gpc_deinit(HAL_GPC_0);                                       //deinit GPC module if it's no longer in use.
*       }
*    @endcode
* @}
* @}
*/
#endif

/**
* @addtogroup HAL
* @{
* @addtogroup GPC
* @{
*/



#ifdef __cplusplus
extern "C" {
#endif

/** @defgroup hal_gpc_typedef Typedef
  * @{
  */
#ifdef HAL_GPC_FEATURE_ADVANCED_CONFIG
/** @brief  This defines the callback function prototype.
 *          User should register a callback function while use GPC interrupt mode.
 *          This function is called in the GPC ISR routine.
 *          More details about the callback, please refer to #hal_gpc_register_callback().
 *  @param [in] user_data is the paremeter pointer for this callback, it is used or not, is decided by user.
 */
typedef void (*hal_gpc_callback_t)(void *user_data);
#endif

/**
  * @}
  */

/** @defgroup hal_gpc_enum Enum
  * @{
  */
#ifdef HAL_GPC_FEATURE_ADVANCED_CONFIG
/** @brief This enum define GPC clock source*/
typedef enum {
    HAL_GPC_CLOCK_26MHZ = 0,                        /**< Set the GPC clock source 26Mhz */
    HAL_GPC_CLOCK_32KHZ = 1,                        /**< Set the GPC clock source 32Khz */
} hal_gpc_clock_t;
#endif


/** @brief This enum define GPC caputure edge */
typedef enum {
    HAL_GPC_EDGE_RAISING = 0,                       /**< Set the  GPC raising edge */
    HAL_GPC_EDGE_FALLING = 1,                       /**< Set the  falling edge */
} hal_gpc_edge_t;

/** @brief This enum define GPC function status */
typedef enum {
    HAL_GPC_STATUS_ERROR             = -3,          /**< A GPC function ERROR occured*/
    HAL_GPC_STATUS_ERROR_PORT        = -2,          /**< A wrong GPC port  */
    HAL_GPC_STATUS_INVALID_PARAMETER = -1,          /**< An invalid parameter */
    HAL_GPC_STATUS_OK   = 0,                        /**< No error occurred during the function call*/
} hal_gpc_status_t;

/** @brief define GPC busy status */
typedef enum {
    HAL_GPC_STATUS_IDLE = 0,                        /**< The GPC is idle*/
    HAL_GPC_STATUS_BUSY = 1,                        /**< The GPC is busy*/
} hal_gpc_running_status_t;
/**
  * @}
  */

/** @defgroup hal_gpc_struct Struct
  * @{
  */

/** @brief This structure defines the initial configuration structure. For more infomation, please refer to #hal_gpc_init() */
typedef struct {
    hal_gpc_edge_t     edge;             /**< Specify GPC capture edge,#HAL_GPC_EDGE_RAISING or #HAL_GPC_EDGE_FALLING */
} hal_gpc_config_t;

#ifdef HAL_GPC_FEATURE_ADVANCED_CONFIG
/** @brief This structure defines the avanced configuration structure. For more infomation,  please refer to #hal_gpc_advanced_config() */
typedef struct {
    hal_gpc_clock_t    clock_source;     /**< Specify GPC clock source */
    uint32_t           debounce;         /**< Specify GPC debounce,debouce_time = debouce*clock_source period*/
    uint32_t           threshold;        /**< Specify GPC set threshold, if count value > threshold trigger an interrupt*/
} hal_gpc_advanced_config_t;
#endif

/**
  * @}
  */

/**
 * @brief     This function is used to initialize the GPC base environment.
 * @param[in] gpc_port is the port number.
 * @param[in] gpc_config is the pointer of config. For mode details please, refer to #hal_gpc_config_t.
 * @return    #HAL_GPC_STATUS_OK, if OK. \n
 *            #HAL_GPC_STATUS_ERROR_PORT, the gpc_port value is wrong.
 * @sa        #hal_gpc_deinit()
 */
hal_gpc_status_t hal_gpc_init(hal_gpc_port_t gpc_port, const hal_gpc_config_t *gpc_config);

/**
 * @brief     Deinitilize the GPC.
 *            After this function, the callback is cleared, clock power is closed, the GPC module disabled.
 * @param[in] gpc_port is the port number.
 * @return    #HAL_GPC_STATUS_OK, if OK.\n
 *            #HAL_GPC_STATUS_ERROR_PORT, the gpc_port value is wrong.
 * @sa        #hal_gpc_init()
 */
hal_gpc_status_t hal_gpc_deinit(hal_gpc_port_t gpc_port);


/**
 * @brief     Enable the GPC module, after that the GPC can start to capture the pulses.
 * @param[in] gpc_port is the port number.
 * @return    #HAL_GPC_STATUS_OK, if OK. \n
 *            #HAL_GPC_STATUS_ERROR_PORT, the gpc_port value is wrong.
 * @sa        hal_gpc_disable().
 */
hal_gpc_status_t hal_gpc_enable(hal_gpc_port_t gpc_port);


/**
 * @brief     Disable the GPC module, after that the GPC stop capturing the pulses. 
 *            If user do not call #hal_gpc_get_and_clear_count(), the value in count register will be kept until clear it.
 * @param[in] gpc_port is the port number.
 * @return    #HAL_GPC_STATUS_OK, if OK. \n
 *            #HAL_GPC_STATUS_ERROR_PORT, the gpc_port value is wrong.
 * @sa        hal_gpc_enable().
 */
hal_gpc_status_t hal_gpc_disable(hal_gpc_port_t gpc_port);


/**
 * @brief     To get the port that you specified is idle or busy.
 * @param[in] gpc_port is the port number.
 * @param[out] running_state is a pointer to get status value. For mode details, please refer to #hal_gpc_running_status_t.
 * @return    #HAL_GPC_STATUS_OK, if OK.\n
 *            #HAL_GPC_STATUS_ERROR_PORT, the gpc_port value is wrong.
 */
hal_gpc_status_t hal_gpc_get_running_status(hal_gpc_port_t gpc_port, hal_gpc_running_status_t *running_state);


/**
 * @brief     Get the current count value, then clear count register to 0.
 * @param[in] gpc_port is the port number.
 * @param[out] count is a pointer that to get count value.
 * @return    #HAL_GPC_STATUS_OK, if OK.\n
 *            #HAL_GPC_STATUS_ERROR_PORT, the gpc_port value is wrong.
 */
hal_gpc_status_t hal_gpc_get_and_clear_count(hal_gpc_port_t gpc_port, uint32_t *count);


/************************************ HAL_GPC_FEATURE_ADVANCED_CONFIG *********************************/
#ifdef  HAL_GPC_FEATURE_ADVANCED_CONFIG
/**
 * @brief     Enable GPC interrupt, after this, if the count value is larger than the threshold, an interrupt is triggered.
 * @param[in] gpc_port is the port number.
 * @return    #HAL_GPC_STATUS_OK, if OK.\n
 *            #HAL_GPC_STATUS_ERROR_PORT, the gpc_port value is wrong.
 * @sa        hal_gpc_interrupt_disable().
 */
hal_gpc_status_t hal_gpc_interrupt_enable(hal_gpc_port_t gpc_port);

/**
 * @brief     Disable GPC interrupt, after this, no interrupt is triggered.
 * @param[in] gpc_port is the port number.
 * @return    #HAL_GPC_STATUS_OK, if OK.\n
 *            #HAL_GPC_STATUS_ERROR_PORT, the gpc_port value is wrong.
 * @sa        hal_gpc_interrupt_enable().e
 */
hal_gpc_status_t hal_gpc_interrupt_disable(hal_gpc_port_t gpc_port);

/**
 * @brief     Register a callback function while in an interrupt mode. It only can be used when GPC is #HAL_GPC_STATUS_IDLE.
 *            If it is #HAL_GPC_STATUS_BUSY, it will not register successfully, and return #HAL_GPC_STATUS_ERROR.
 * @param[in] gpc_port is the port number.
 * @param[in] callback is the function pointer of the callback.
 *            This callback will be called when count value larger than threshold value.
 * @param[in] user_data is the paremeter pointer for this callback, it is used or not, decided by user.
 * @return    #HAL_GPC_STATUS_OK, if OK.\n
 *            #HAL_GPC_STATUS_ERROR_PORT, the gpc_port value is wrong.\n
 *            #HAL_GPC_STATUS_ERROR, register callback failed.
 */
hal_gpc_status_t hal_gpc_register_callback(hal_gpc_port_t gpc_port, hal_gpc_callback_t callback, void *user_data);


/**
 * @brief     Set threshold value.
 * @param[in] gpc_port is the port number.
 * @param[in] threshold is the parameter pointer of user given, if count value is larger than threshold , an interrupt is triggered.
 * @return    #HAL_GPC_STATUS_OK, if OK.\n
 *            #HAL_GPC_STATUS_ERROR_PORT, the gpc_port value is wrong.
 * @sa        #hal_gpc_get_threshold.
 */
hal_gpc_status_t hal_gpc_set_threshold(hal_gpc_port_t gpc_port, uint32_t *threshold);

/**
 * @brief     Get current threshold value.
 * @param[in] gpc_port is the port number.
 * @param[out] threshold is the user's parameter pointer to get the current threshold value.
 * @return    #HAL_GPC_STATUS_OK, if OK.\n
 * @sa        #hal_gpc_set_threshold().
 */
hal_gpc_status_t hal_gpc_get_threshold(hal_gpc_port_t gpc_port, uint32_t *threshold);

/**
 * @brief     This function is used to enhance the coniguration. User can set clock_source, debounce time and threshold value using this funtion.
 *            For more details, please refer to @ref GPC_Features_Chapter.
 * @param[in] gpc_port is the port number.
 * @param[in] gpc_advanced_config is the structure pointer of advanced configuration. For more details, please refer to #hal_gpc_advanced_config()_t.
 * @return    #HAL_GPC_STATUS_OK, if OK.\n
 *            #HAL_GPC_STATUS_ERROR_PORT, the gpc_port value is wrong.
 */
hal_gpc_status_t hal_gpc_advanced_config(hal_gpc_port_t gpc_port, const hal_gpc_advanced_config_t *gpc_advanced_config);

#endif

#ifdef __cplusplus
}
#endif

/**
* @}
* @}
*/
#endif /*HAL_GPC_MODULE_ENABLED*/
#endif /* __HAL_GPC_H__ */


