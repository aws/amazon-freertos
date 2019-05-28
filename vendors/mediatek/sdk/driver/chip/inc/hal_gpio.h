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

#ifndef __HAL_GPIO_H__
#define __HAL_GPIO_H__
#include "hal_platform.h"
#include "hal_pinmux_define.h"

#ifdef HAL_GPIO_MODULE_ENABLED

/**
 * @addtogroup HAL
 * @{
 * @addtogroup GPIO
 * @{
 * This section provides introduction to the General Purpose Input Output(GPIO) APIs, including terms and acronyms, features,
 * architecture, how to use APIs, the GPIO function groups, enums, structures and functions.
 *
 * @section HAL_GPIO_Terms_Chapter Terms and acronyms
 *
 * |Terms                         |Details                                                                 |
 * |------------------------------|------------------------------------------------------------------------|
 * |\b GPIO                       | General Purpose Input Output is a generic pin on an integrated circuit defined as an input or output pin, and controlled by the user at run time. For more information, please check <a href="https://en.wikipedia.org/wiki/General-purpose_input/output"> General Purpose Input Output in wiki </a>.|
 *
 * @section HAL_GPIO_Features_Chapter Supported features
 * - \b Support \b GPIO \b and \b peripheral \b operating \b modes. \n
 *       GPIO operates in various modes depending to user configuration:
 *  - \b GPIO \b mode: This mode specifies the pin to operate as a GPIO, which means the pin is programmed as a software controlled input or output.
 *       That is to say, the GPIO can be configured as output or input, the output data can be configured through writing to output data register if it is configured as output.
 *       The input data can be got from input data register if it is configured as input.
 *       Both input data and output data can be digital low or digital high.  \n
 *  - \b Peripheral \b mode:  This mode specifies the pin a function of an embedded peripheral.
 *       The microcontroller pin connects the peripheral through multiplexer with which the pin can be specified to only one alternate function at a time,
 *       that is to say the peripheral module must configure the correct pinmux to the corresponding pin before it is used. Two ways can be used to achieve the pinmux configuration,
 *       one is to call hal_pinmux_set_function() and the other way is to configure the EPT(Easy pinmux tool) which is a software tool providing a graphical user interface to configure the pinmux of all the pins.
 *       Detailed information about alternate functions of every pin can be got from hal_pinmux_define.h.
 *   \n
 * - Call hal_pinmux_set_function() to set the mode of the pin and call hal_gpio_set_direction() to set the direction of GPIO. If the pin is in GPIO mode, call the hal_gpio_get_input()
 * to get input data of the pin, and hal_gpio_set_output() to set data to output.
 *   \n
 * - \b Support \b toggle \b function. \n
 *      The toggle function inverses output data of pins once only and doesn't operate on the same pin the next time.\n
 *      Call hal_gpio_toggle_pin() to toggle the output data of target pin one time.
 *   \n
 * - \b Support \b pull-up \b and \b pull-down \b functions. \n
 *      The pull-up and pull-down functions define the input state of a pin if no signal source is connected.
 *      Both the pull-up and pull-down functions are implemented using resistors. Their function is the same as to create a default value for the pin, but one pulls the line high, the other pulls it low.
 *      Set the pull state of target pin by configuring GPIO registers.
 *      \n
 *      Call hal_gpio_pull_down() to set the pin to pull down state and hal_gpio_pull_up() to set the pin to pull up state.
 *
 * @}
 * @}
 */

#ifdef HAL_GPIO_FEATURE_PUPD
/**
 * @addtogroup HAL
 * @{
 * @addtogroup GPIO
 * @{
 * - \b Support \b pull-up \b and \b pull-down \b with \b different \b resistance \b functions. \n
 *      Pins can be configured to pull up or pull down state with different resistance if needed.\n
 *      Call hal_gpio_set_pupd_register() to set pull state with corresponding resistance according to hardware design.
 * @}
 * @}
 */
#endif


#ifdef HAL_GPIO_FEATURE_INVERSE
/**
 * @addtogroup HAL
 * @{
 * @addtogroup GPIO
 * @{
 * - \b Support \b inversion \b function. \n
 *      Inversion function inverses input data of GPIO until the function is disabled, it is usefull when chip is input high active while the connected device is output and low active.  \n
 *      Call hal_gpio_enable_inversion() to inverse the input data of the pin until the function is disabled by calling hal_gpio_disable_inversion(). \n
 * @}
 * @}
 */
#endif


#ifdef HAL_GPIO_FEATURE_CLOCKOUT
/**
 * @addtogroup HAL
 * @{
 * @addtogroup GPIO
 * @{
 * - \b Support \b output \b clock \b function.  \n
 *      To facilitate application use, there are 6 clock-out ports embedded in all GPIO pins and each of them can be configured with appropriate clock frequency to send outside the chip.
 *   \n
 *       Call hal_gpio_set_clockout() to set the output clock source of target pin after configuring the pin to operate in output clock mode. \n
 * @}
 * @}
 */
#endif


/**
 * @addtogroup HAL
 * @{
 * @addtogroup GPIO
 * @{
 * @section HAL_GPIO_Architechture_Chapter Software architecture of GPIO
 *
 * The GPIO driver supports two modes of operation as described below.
 * -# \b GPIO \b mode \b architecture: If the pin is specified to operate in GPIO mode, it can be freely programmable to input and output directions.
 *    If the direction is configured as output through programming direction register, the data written to the output register will be
 *    output on the pin, the output data register is also read accessible while a read access to the output data register only gets the last written value.
 *    The output pin is push-pull type by default, push-pull output here means a type of electronic circuit that uses a pair of active devices that alternately supply current to,
 *    or absorb current from, a connected load. Push-pull outputs are present in TTL and CMOS digital logic circuits and in some types of amplifiers, and are usually implemented as a complementary pair of transistors,
 *    one dissipating or sinking current from the load to ground or a negative power supply, and the other supplying or sourcing current to the load from a positive power supply.
 *    If the direction is configured as input, the data present on the pin can be got from input data register. Unlike the output circuit,
 *    there are schmitt trigger and pull-up and pull-down resistors on the input circuit. Among them, schmitt trigger is an active circuit which converts an analog input signal to a digital output signal.
 *    The pull-up and pull-down resistors can help the input data of target pin be equivalent to a default value if the pin is left unconnected.
 *    \n
 * -# \b Peripheral \b mode \b architecture : Pins can operate in one of several specified peripheral modes as the pin is connected to onboard peripherals/modules through a
 *    multiplexer, in other words, it can only operate in one mode at a time to avoid conflict between peripherals sharing the same pin.
 *    \n
 * @section HAL_GPIO_Driver_Usage_Chapter How to use this driver
 *
 *  - \b operate \b in \b GPIO \b mode.
 *   - Step1 : Call hal_gpio_init() to initialize the pin.
 *   - Step2 : Call hal_pinmux_set_function() to configure the pin to operate in GPIO mode.
 *   - Step3 : Call hal_gpio_set_direction() to configure the direction of GPIO.
 *   - Step4 : Call hal_gpio_set_output() to set the data to be output if direction is ouput.
 *   - Step5 : Call hal_gpio_deinit() to deinitialize the pin.
 *   - sample code:
 *    @code
 *     void gpio_application(void)
 *     {
 *
 *       hal_gpio_init(gpio_pin);
 *       hal_pinmux_set_function(gpio_pin, function_index); //set the pin to work in GPIO mode
 *       hal_gpio_set_direction(gpio_pin, HAL_GPIO_DIRECTION_OUTPUT);
 *       hal_gpio_set_output(gpio_pin, HAL_GPIO_DATA_HIGH);
 *       hal_gpio_deinit(gpio_pin);
 *     }
 *    @endcode
 * @}
 * @}
 */


/**
 * @addtogroup HAL
 * @{
 * @addtogroup GPIO
 * @{
 *  - \b operate \b in \b peripheral \b mode.
 *   - Step1 : Call hal_gpio_init() to initialize the pin.
 *   - Step2 : Call hal_pinmux_set_function() to configure the pin to operate in peripheral mode.
 *   - Step3 : Call hal_gpio_deinit() to deinitialize the pin.
 *   - sample code:
 *    @code
 *     void gpio_application(void)
 *     {
 *       hal_gpio_init(gpio_pin);
 *       //more information about pinmux, please refernence hal_pinmux_define.h
 *       hal_pinmux_set_function(gpio_pin, function_index);   // set the pin to work in peripheral mode determined by parameter of function_index
 *       hal_gpio_deinit(gpio_pin);
 *      }
 *   @endcode
 *  \n
 */



#ifdef __cplusplus
extern "C" {
#endif

/** @defgroup hal_gpio_enum Enum
  * @{
  */

/** @brief This enum defines gpio direction */
typedef enum {
    HAL_GPIO_DIRECTION_INPUT  = 0,              /**< define GPIO input direction */
    HAL_GPIO_DIRECTION_OUTPUT = 1               /**< define GPIO output direction */
} hal_gpio_direction_t;


/** @brief This enum defines input or output data of GPIO */
typedef enum {
    HAL_GPIO_DATA_LOW  = 0,               /**< define GPIO data of low */
    HAL_GPIO_DATA_HIGH = 1                /**< define GPIO data of high */
} hal_gpio_data_t;


#ifdef HAL_GPIO_FEATURE_CLOCKOUT
/** @brief  This enum defines output clock number of GPIO */
typedef enum {
    HAL_GPIO_CLOCK_0   = 0,              /**< define GPIO output clock 0 */
    HAL_GPIO_CLOCK_1   = 1,              /**< define GPIO output clock 1 */
    HAL_GPIO_CLOCK_2   = 2,              /**< define GPIO output clock 2 */
    HAL_GPIO_CLOCK_3   = 3,              /**< define GPIO output clock 3 */
    HAL_GPIO_CLOCK_4   = 4,              /**< define GPIO output clock 4 */
    HAL_GPIO_CLOCK_5   = 5,              /**< define GPIO output clock 5 */
    HAL_GPIO_CLOCK_MAX                   /**< define GPIO output clock max number(invalid) */
} hal_gpio_clock_t;


/** @brief This enum defines output clock mode of GPIO */
typedef enum {
    HAL_GPIO_CLOCK_MODE_26M = 1,        /**< define GPIO output clock mode as 26MHz */
    HAL_GPIO_CLOCK_MODE_32K = 4,        /**< define GPIO output clock mode as 32KHz */
    HAL_GPIO_CLOCK_MODE_MAX             /**< define GPIO output clock mode of max number(invalid) */
} hal_gpio_clock_mode_t;
#endif


/** @brief This enum defines GPIO API return type */
typedef enum {
    HAL_GPIO_STATUS_ERROR             = -3,         /**< Indicate GPIO function execution failed */
    HAL_GPIO_STATUS_ERROR_PIN         = -2,         /**< Indicate a wrong pin number is given */
    HAL_GPIO_STATUS_INVALID_PARAMETER = -1,         /**< Indicate a wrong parameter is given */
    HAL_GPIO_STATUS_OK                = 0           /**< Indicate GPIO function execute successfully */
} hal_gpio_status_t;


/** @brief This enum defines pinmux API return type */
typedef enum {
    HAL_PINMUX_STATUS_ERROR             = -3,         /**< Indicate pinmux function execution failed */
    HAL_PINMUX_STATUS_ERROR_PORT        = -2,         /**< Indicate a wrong pin number is given */
    HAL_PINMUX_STATUS_INVALID_FUNCTION  = -1,         /**< Indicate a wrong function is given */
    HAL_PINMUX_STATUS_OK                = 0           /**< Indicate the pinmux function executes successfully */
} hal_pinmux_status_t;


/**
  * @}
  */



/**
 * @brief     This function initializes the GPIO hardware with basic functionality. The target pin must be initialized before used.
 * @param[in] gpio_pin specifies pin number to init.
 * @return    To indicate whether this function call is successful or not, for example:
 *            If the return value is #HAL_GPIO_STATUS_OK, it means success;
 *            If the return value is #HAL_GPIO_STATUS_ERROR_PIN, it means a wrong pin number is given, the parameter must be verified;
 *            If the return value is #HAL_GPIO_STATUS_ERROR, it means failure.
 * @note
 * @warning
 */
hal_gpio_status_t hal_gpio_init(hal_gpio_pin_t gpio_pin);


/**
 * @brief     This function deinitializes the GPIO hardware to its default status. The target pin must be deinitialized if not used.
 * @param[in] gpio_pin specifies pin number to deinit.
 * @return    To indicate whether this function call is successful or not, for example:
 *            If the return value is #HAL_GPIO_STATUS_OK, it means success;
 *            If the return value is #HAL_GPIO_STATUS_ERROR_PIN, it means a wrong pin number is given, the parameter must be verified;
 *            If the return value is #HAL_GPIO_STATUS_ERROR, it means failure.
 * @note
 * @warning
 */
hal_gpio_status_t hal_gpio_deinit(hal_gpio_pin_t gpio_pin);


/**
 * @brief     This function is used to configure pinmux of target GPIO.
 *            Pinmux represents Pin Multiplexor which connects the pin and the onboard peripherals,
 *            so the pin will operate in a specific mode once the pin is programmed to a peripheral's function.
 *            All the alternate functions of every pin can be seen from hal_pinmux_define.h.
 * @param[in] gpio_pin specifies pin number to configure.
 * @param[in] function_index specifies the function for the pin.
 * @return    To indicate whether this function call is successful or not, for example:
 *            If the return value is #HAL_PINMUX_STATUS_OK, it means success;
 *            If the return value is #HAL_PINMUX_STATUS_INVALID_FUNCTION, it means a wrong alternate function is given, the parameter must be verified;
 *            If the return value is #HAL_PINMUX_STATUS_ERROR_PORT, it means a wrong pin number is given, the parameter must be verified;
 *            If the return value is #HAL_PINMUX_STATUS_ERROR, it means failure.
 * @note
 * @warning
 */
hal_pinmux_status_t hal_pinmux_set_function(hal_gpio_pin_t gpio_pin, uint8_t function_index);


/**
 * @brief     This function is used to get input data of target GPIO when the direction of the GPIO is input.
 * @param[in] gpio_pin specifies pin number to operate.
 * @param[in] gpio_data represents input data received from target GPIO.
 * @return    To indicate whether this function call is successful or not, for example:
 *            If the return value is #HAL_GPIO_STATUS_OK, it means success;
 *            If the return value is #HAL_GPIO_STATUS_INVALID_PARAMETER, it means a wrong parameter(except for pin number) is given, the parameter must be verified;
 *            If the return value is #HAL_GPIO_STATUS_ERROR_PIN, it means a wrong pin number is given, the parameter must be verified;
 *            If the return value is #HAL_GPIO_STATUS_ERROR, it means failure.
 * @note
 * @warning
 */
hal_gpio_status_t hal_gpio_get_input(hal_gpio_pin_t gpio_pin, hal_gpio_data_t *gpio_data);


/**
 * @brief     This function is used to set output data of target GPIO.
 * @param[in] gpio_pin specifies pin number to operate.
 * @param[in] gpio_data represents output data of target GPIO.
 * @return    To indicate whether this function call is successful or not, for example:
 *            If the return value is #HAL_GPIO_STATUS_OK, it means success;
 *            If the return value is #HAL_GPIO_STATUS_INVALID_PARAMETER, it means a wrong parameter(except for pin number) is given, the parameter must be verified;
 *            If the return value is #HAL_GPIO_STATUS_ERROR_PIN, it means a wrong pin number is given, the parameter must be verified;
 *            If the return value is #HAL_GPIO_STATUS_ERROR, it means failure.
 * @note
 * @warning
 */
hal_gpio_status_t hal_gpio_set_output(hal_gpio_pin_t gpio_pin, hal_gpio_data_t gpio_data);


/**
 * @brief     This function is used to get output data of target GPIO which is last set when the direction of the GPIO is output.
 * @param[in] gpio_pin specifies pin number to operate.
 * @param[in] gpio_data represents output data of target GPIO.
 * @return    To indicate whether this function call is successful or not, for example:
 *            If the return value is #HAL_GPIO_STATUS_OK, it means success;
 *            If the return value is #HAL_GPIO_STATUS_INVALID_PARAMETER, it means a wrong parameter(except for pin number) is given, the parameter must be verified;
 *            If the return value is #HAL_GPIO_STATUS_ERROR_PIN, it means a wrong pin number is given, the parameter must be verified;
 *            If the return value is #HAL_GPIO_STATUS_ERROR, it means failure.
 * @note
 * @warning
 */
hal_gpio_status_t hal_gpio_get_output(hal_gpio_pin_t gpio_pin, hal_gpio_data_t *gpio_data);


/**
 * @brief     This function is used to set direction of target GPIO.
 * @param[in] gpio_pin specifies pin number to set.
 * @param[in] gpio_direction specified the direction of target GPIO, the direction can be input and output.
 * @return    To indicate whether this function call is successful or not, for example:
 *            If the return value is #HAL_GPIO_STATUS_OK, it means success;
 *            If the return value is #HAL_GPIO_STATUS_INVALID_PARAMETER, it means a wrong parameter(except for pin number) is given, the parameter must be verified;
 *            If the return value is #HAL_GPIO_STATUS_ERROR_PIN, it means a wrong pin number is given, the parameter must be verified;
 *            If the return value is #HAL_GPIO_STATUS_ERROR, it means failure.
 * @note
 * @warning
 */
hal_gpio_status_t hal_gpio_set_direction(hal_gpio_pin_t gpio_pin, hal_gpio_direction_t gpio_direction);


/**
 * @brief     This function is used to get direction of target GPIO.
 * @param[in] gpio_pin specifies pin number to operate.
 * @param[in] gpio_direction represents direction of target GPIO, the direction can be input and output.
 * @return    To indicate whether this function call is successful or not, for example:
 *            If the return value is #HAL_GPIO_STATUS_OK, it means success;
 *            If the return value is #HAL_GPIO_STATUS_INVALID_PARAMETER, it means a wrong parameter(except for pin number) is given, the parameter must be verified;
 *            If the return value is #HAL_GPIO_STATUS_ERROR_PIN, it means a wrong pin number is given, the parameter must be verified;
 *            If the return value is #HAL_GPIO_STATUS_ERROR, it means failure.
 * @note
 * @warning
 */
hal_gpio_status_t hal_gpio_get_direction(hal_gpio_pin_t gpio_pin, hal_gpio_direction_t *gpio_direction);


#ifdef HAL_GPIO_FEATURE_HIGH_Z
/**
 * @brief     This function is used to set target GPIO to high impedance state.
 *            High impedance can prevent target GPIO from electric leakage.
 *            The pin in high impedance state can be seen as an open circuit because connecting it to a low impedance circuit will not affect that circuit.
 *            It is adviced to put the pin into high impedance state if the pin is unused to optimize the power consumption.
 * @param[in] gpio_pin specifies pin number to set.
 * @return    To indicate whether this function call is successful or not, for example:
 *            If the return value is #HAL_GPIO_STATUS_OK, it means success;
 *            If the return value is #HAL_GPIO_STATUS_ERROR_PIN, it means a wrong pin number is given, the parameter must be verified;
 *            If the return value is #HAL_GPIO_STATUS_ERROR, it means failure.
 * @note
 * @warning
 */
hal_gpio_status_t hal_gpio_set_high_impedance(hal_gpio_pin_t gpio_pin);

/**
 * @brief     This function is used to make target GPIO out of high impedance state.
 *            High impedance can prevent target GPIO from electric leakage.
 *            It is necessary to call this function before further configuration if the pin is in high impedance state.
 * @param[in] gpio_pin specifies pin number to set.
 * @return    To indicate whether this function call is successful or not, for example:
 *            If the return value is #HAL_GPIO_STATUS_OK, it means success;
 *            If the return value is #HAL_GPIO_STATUS_ERROR_PIN, it means a wrong pin number is given, the parameter must be verified;
 *            If the return value is #HAL_GPIO_STATUS_ERROR, it means failure.
 * @note
 * @warning
 * @par       Example
 * @code
 *
 *            hal_gpio_status_t ret;
 *
 *            ret = hal_gpio_init(gpio_pin);
 *            ret = hal_gpio_set_high_impedance(gpio_pin);
 *            // do something else
 *            ret = hal_gpio_clear_high_impedance(gpio_pin); //put target GPIO out of high impedance state before other configuration
 *            ret = hal_gpio_deinit(gpio_pin);
 *
 * @endcode
 */
hal_gpio_status_t hal_gpio_clear_high_impedance(hal_gpio_pin_t gpio_pin);
#endif


/**
 * @brief     This function is used to toggle output data of target GPIO when the direction of the pin is output. After this function,
 *            the output data of target GPIO will be inversed only one time.
 * @param[in] gpio_pin specifies pin number to toggle.
 * @return    To indicate whether this function call is successful or not, for example:
 *            If the return value is #HAL_GPIO_STATUS_OK, it means success;
 *            If the return value is #HAL_GPIO_STATUS_ERROR_PIN, it means a wrong pin number is given, the parameter must be verified;
 *            If the return value is #HAL_GPIO_STATUS_ERROR, it means failure.
 * @note
 * @warning
 * @par       Example
 * @code
 *            hal_gpio_status_t ret;
 *            hal_pinmux_status_t ret_pinmux_status;
 *
 *            ret = hal_gpio_init(gpio_pin);
 *            ret_pinmux_status = hal_pinmux_set_function(gpio_pin, function_index); //set the pin to work in GPIO mode
 *            ret = hal_gpio_set_direction(gpio_pin, HAL_GPIO_DIRECTION_OUTPUT);
 *            ret = hal_gpio_set_output(gpio_pin, HAL_GPIO_DATA_HIGH);
 *            ret = hal_gpio_toggle_pin(gpio_pin);  // output data of gpio_pin will be toggled to low from high
 *            ret = hal_gpio_deinit(gpio_pin);
 * @endcode
 */
hal_gpio_status_t hal_gpio_toggle_pin(hal_gpio_pin_t gpio_pin);


#ifdef HAL_GPIO_FEATURE_INVERSE
/**
 * @brief     This function is used to enable input data inverse of target GPIO, after this function,
 *            the input data of target GPIO will always be inversed until the inverse function is disabled.
 * @param[in] gpio_pin specifies pin number to inverse.
 * @return    To indicate whether this function call is successful or not, for example:
 *            If the return value is #HAL_GPIO_STATUS_OK, it means success;
 *            If the return value is #HAL_GPIO_STATUS_ERROR_PIN, it means a wrong pin number is given, the parameter must be verified;
 *            If the return value is #HAL_GPIO_STATUS_ERROR, it means failure.
 * @note
 * @warning
 * @par       Example
 * @code
 *            hal_gpio_status_t ret;
 *
 *            ret = hal_gpio_init(gpio_pin);
 *            ret = hal_gpio_enable_inversion(gpio_pin);
 *            // do something else
 *            ret = hal_gpio_disable_inversion(gpio_pin);
 *            ret = hal_gpio_deinit(gpio_pin);
 * @endcode
 */
hal_gpio_status_t hal_gpio_enable_inversion(hal_gpio_pin_t gpio_pin);


/**
 * @brief     This function is used to disable input data inverse of target GPIO.
 * @param[in] gpio_pin specifies pin number to config.
 * @return    To indicate whether this function call is successful or not, for example:
 *            If the return value is #HAL_GPIO_STATUS_OK, it means success;
 *            If the return value is #HAL_GPIO_STATUS_ERROR_PIN, it means a wrong pin number is given, the parameter must be verified;
 *            If the return value is #HAL_GPIO_STATUS_ERROR, it means failure.
 * @note
 * @warning
 * @par       Example
 * @code
 *            hal_gpio_status_t ret;
 *
 *            ret = hal_gpio_init(gpio_pin);
 *            ret = hal_gpio_enable_inversion(gpio_pin);
 *            // do something else
 *            ret = hal_gpio_disable_inversion(gpio_pin);
 *            ret = hal_gpio_deinit(gpio_pin);
 * @endcode
 */
hal_gpio_status_t hal_gpio_disable_inversion(hal_gpio_pin_t gpio_pin);
#endif


/**
 * @brief     This function is used to set target GPIO to pull-up state, after this function,
 *            the input data of target pin will be equivalent to high if the pin is left unconnected.
 *            This function only works on the pin which has only one pull-up resister.
 * @param[in] gpio_pin specifies pin number to set.
 * @return    To indicate whether this function call is successful or not, for example:
 *            If the return value is #HAL_GPIO_STATUS_OK, it means success;
 *            If the return value is #HAL_GPIO_STATUS_ERROR_PIN, it means a wrong pin number is given, the parameter must be verified;
 *            If the return value is #HAL_GPIO_STATUS_ERROR, it means failure.
 * @note
 * @warning
 * @par       Example
 * @code
 *            hal_gpio_status_t ret;
 *            hal_pinmux_status_t ret_pinmux_status;
 *
 *            ret = hal_gpio_init(gpio_pin);
 *            ret_pinmux_status = hal_pinmux_set_function(gpio_pin, function_index); //set the pin to work in GPIO mode
 *            ret = hal_gpio_set_direction(gpio_pin, HAL_GPIO_DIRECTION_INPUT);
 *            ret = hal_gpio_pull_up(gpio_pin);   //pull state of the target GPIO is set to pull-up
 *            ret = hal_gpio_deinit(gpio_pin);
 * @endcode
 */
hal_gpio_status_t hal_gpio_pull_up(hal_gpio_pin_t gpio_pin);


/**
 * @brief     This function is used to set target GPIO to pull-down state, after this function,
 *            the input data of target pin will be equivalent to low if the pin is left unconnected.
 *            This function only works on the pin which has only one pull-down resister.
 * @param[in] gpio_pin specifies pin number to set.
 * @return    To indicate whether this function call is successful or not, for example:
 *            If the return value is #HAL_GPIO_STATUS_OK, it means success;
 *            If the return value is #HAL_GPIO_STATUS_ERROR_PIN, it means a wrong pin number is given, the parameter must be verified;
 *            If the return value is #HAL_GPIO_STATUS_ERROR, it means failure.
 * @note
 * @warning
 * @par       Example
 * @code
 *            hal_gpio_status_t ret;
 *            hal_pinmux_status_t ret_pinmux_status;
 *
 *            ret = hal_gpio_init(gpio_pin);
 *            ret_pinmux_status = hal_pinmux_set_function(gpio_pin, function_index); //set the pin to work in GPIO mode
 *            ret = hal_gpio_set_direction(gpio_pin, HAL_GPIO_DIRECTION_INPUT);
 *            ret = hal_gpio_pull_down(gpio_pin);   //pull state of the target GPIO is set to pull-down
 *            ret = hal_gpio_deinit(gpio_pin);
 * @endcode
 */
hal_gpio_status_t hal_gpio_pull_down(hal_gpio_pin_t gpio_pin);


/**
 * @brief     This function is used to disable pull-up or pull-down of target GPIO.
 *            This function only works on the pin which has only one pull-up resister and one pull-down resister.
 * @param[in] gpio_pin specifies pin number to set.
 * @return    To indicate whether this function call is successful or not, for example:
 *            If the return value is #HAL_GPIO_STATUS_OK, it means success;
 *            If the return value is #HAL_GPIO_STATUS_ERROR_PIN, it means a wrong pin number is given, the parameter must be verified;
 *            If the return value is #HAL_GPIO_STATUS_ERROR, it means failure.
 * @note
 * @warning
 * @par       Example
 * @code
 *            hal_gpio_status_t ret;
 *            hal_pinmux_status_t ret_pinmux_status;
 *
 *            ret = hal_gpio_init(gpio_pin);
 *            ret_pinmux_status = hal_pinmux_set_function(gpio_pin, function_index); //set the pin to work in GPIO mode
 *            ret = hal_gpio_set_direction(gpio_pin, HAL_GPIO_DIRECTION_INPUT);
 *            ret = hal_gpio_pull_down(gpio_pin);
 *            ret = hal_gpio_disable_pull(gpio_pin);   //pull state of the target GPIO is set to disable
 *            ret = hal_gpio_deinit(gpio_pin);
 * @endcode
 */
hal_gpio_status_t hal_gpio_disable_pull(hal_gpio_pin_t gpio_pin);


#ifdef HAL_GPIO_FEATURE_PUPD
/**
 * @brief     This function is used to set pull up/down state of GPIO who has more than one pull-up or pull-down resister.
 * @param[in] gpio_pin specifies pin number to configure.
 * @param[in] gpio_pupd specifies pull-up or pull-down of target GPIO.
 * @param[in] gpio_r0 works with gpio_r1 to specify pull resister of target GPIO.
 * @param[in] gpio_r1 works with gpio_r0 to specify pull resister of target GPIO.
 * @return    To indicate whether this function call is successful or not, for example:
 *            If the return value is #HAL_GPIO_STATUS_OK, it means success;
 *            If the return value is #HAL_GPIO_STATUS_ERROR_PIN, it means a wrong pin number is given, the parameter must be verified;
 *            If the return value is #HAL_GPIO_STATUS_ERROR, it means failure.
 * @note
 * @warning
 * @par       Example
 * @code
 *            hal_gpio_status_t ret;
 *            hal_pinmux_status_t ret_pinmux_status;
 *
 *            ret = hal_gpio_init(gpio_pin);
 *            ret_pinmux_status = hal_pinmux_set_function(gpio_pin, function_index); //set the pin to work in GPIO mode
 *            ret = hal_gpio_set_direction(gpio_pin, HAL_GPIO_DIRECTION_INPUT);
 *            ret = hal_gpio_set_pupd_register(gpio_pin,gpio_pupd,gpio_r0,gpio_r1); //pull state of the target GPIO is set to a state decided by combination of gpio_pupd,gpio_r0 and gpio_r1
 *            ret = hal_gpio_deinit(gpio_pin);
 * @endcode
 */
hal_gpio_status_t hal_gpio_set_pupd_register(hal_gpio_pin_t gpio_pin, uint8_t gpio_pupd, uint8_t gpio_r0, uint8_t gpio_r1);
#endif


#ifdef HAL_GPIO_FEATURE_CLOCKOUT
/**
 * @brief     This function is used to set clock out source of target GPIO.
 *            To facilitate application use, the software can configure which clock to send outside the chip.
 *            There are 6 clock-out ports embedded in all GPIO pins, and each clock-out can be programmed to output appropriate clock source.
 *            This function can only be used after configuring the pin to operate in output clock mode.
 * @param[in] gpio_clock_num specifies pin clock number to set.
 * @param[in] clock_mode specifies clock mode to set to target GPIO.
 * @return    To indicate whether this function call is successful or not, for example
 *            If the return value is #HAL_GPIO_STATUS_OK, it means success;
 *            If the return value is #HAL_GPIO_STATUS_INVALID_PARAMETER, it means a wrong parameter(except for pin number) is given, the parameter must be verified;
 *            If the return value is #HAL_GPIO_STATUS_ERROR, it means failure.
 * @note
 * @warning
 * @par       Example
 * @code
 *            hal_gpio_status_t ret;
 *            hal_pinmux_status_t ret_pinmux_status;
 *
 *            ret = hal_gpio_init(gpio_pin);
 *            ret_pinmux_status = hal_pinmux_set_function(gpio_pin, function_index_of_clockout);  // set the pin to work in clock output mode
 *            ret = hal_gpio_set_clockout(gpio_clock_num, clock_mode); // the pin will output clock on the frequency decided by clock_mode, gpio_clock_num is determined by the previous step according to the pin number
 *            ret = hal_gpio_deinit(gpio_pin);
 * @endcode
 */
hal_gpio_status_t hal_gpio_set_clockout(hal_gpio_clock_t gpio_clock_num, hal_gpio_clock_mode_t clock_mode);
#endif


#ifdef __cplusplus
}
#endif


/**
* @}
* @}
*/
#endif /*HAL_GPIO_MODULE_ENABLED*/
#endif /* __HAL_GPIO_H__ */

