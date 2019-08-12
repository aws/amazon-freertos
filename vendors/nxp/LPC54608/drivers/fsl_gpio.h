/*
 * Copyright (c) 2016, Freescale Semiconductor, Inc.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * o Redistributions of source code must retain the above copyright notice, this list
 *   of conditions and the following disclaimer.
 *
 * o Redistributions in binary form must reproduce the above copyright notice, this
 *   list of conditions and the following disclaimer in the documentation and/or
 *   other materials provided with the distribution.
 *
 * o Neither the name of Freescale Semiconductor, Inc. nor the names of its
 *   contributors may be used to endorse or promote products derived from this
 *   software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SDRVL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
 * ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef _LPC_GPIO_H_
#define _LPC_GPIO_H_

#include "fsl_common.h"

/*!
 * @addtogroup lpc_gpio
 * @{
 */

/*! @file */

/*******************************************************************************
 * Definitions
 ******************************************************************************/

/*! @name Driver version */
/*@{*/
/*! @brief LPC GPIO driver version 2.0.0. */
#define FSL_GPIO_DRIVER_VERSION (MAKE_VERSION(2, 0, 0))
/*@}*/

/*! @brief LPC GPIO direction definition */
typedef enum _gpio_pin_direction
{
    kGPIO_DigitalInput = 0U,  /*!< Set current pin as digital input*/
    kGPIO_DigitalOutput = 1U, /*!< Set current pin as digital output*/
} gpio_pin_direction_t;

/*!
 * @brief The GPIO pin configuration structure.
 *
 * Every pin can only be configured as either output pin or input pin at a time.
 * If configured as a input pin, then leave the outputConfig unused.
 */
typedef struct _gpio_pin_config
{
    gpio_pin_direction_t pinDirection; /*!< GPIO direction, input or output */
    /* Output configurations, please ignore if configured as a input one */
    uint8_t outputLogic; /*!< Set default output logic, no use in input */
} gpio_pin_config_t;

/*******************************************************************************
 * API
 ******************************************************************************/
#if defined(__cplusplus)
extern "C" {
#endif

/*! @name GPIO Configuration */
/*@{*/

/*!
 * @brief Initializes a GPIO pin used by the board.
 *
 * To initialize the GPIO, define a pin configuration, either input or output, in the user file.
 * Then, call the GPIO_PinInit() function.
 *
 * This is an example to define an input pin or output pin configuration:
 * @code
 * // Define a digital input pin configuration,
 * gpio_pin_config_t config =
 * {
 *   kGPIO_DigitalInput,
 *   0,
 * }
 * //Define a digital output pin configuration,
 * gpio_pin_config_t config =
 * {
 *   kGPIO_DigitalOutput,
 *   0,
 * }
 * @endcode
 *
 * @param base   GPIO peripheral base pointer(Typically GPIO)
 * @param port   GPIO port number
 * @param pin    GPIO pin number
 * @param config GPIO pin configuration pointer
 */
void GPIO_PinInit(GPIO_Type *base, uint32_t port, uint32_t pin, const gpio_pin_config_t *config);

/*@}*/

/*! @name GPIO Output Operations */
/*@{*/

/*!
 * @brief Sets the output level of the one GPIO pin to the logic 1 or 0.
 *
 * @param base    GPIO peripheral base pointer(Typically GPIO)
 * @param port   GPIO port number
 * @param pin    GPIO pin number
 * @param output  GPIO pin output logic level.
 *        - 0: corresponding pin output low-logic level.
 *        - 1: corresponding pin output high-logic level.
 */
static inline void GPIO_WritePinOutput(GPIO_Type *base, uint32_t port, uint32_t pin, uint8_t output)
{
    base->B[port][pin] = output;
}
/*@}*/
/*! @name GPIO Input Operations */
/*@{*/

/*!
 * @brief Reads the current input value of the GPIO PIN.
 *
 * @param base GPIO peripheral base pointer(Typically GPIO)
 * @param port   GPIO port number
 * @param pin    GPIO pin number
 * @retval GPIO port input value
 *        - 0: corresponding pin input low-logic level.
 *        - 1: corresponding pin input high-logic level.
 */
static inline uint32_t GPIO_ReadPinInput(GPIO_Type *base, uint32_t port, uint32_t pin)
{
    return (uint32_t)base->B[port][pin];
}
/*@}*/

/*!
 * @brief Sets the output level of the multiple GPIO pins to the logic 1.
 *
 * @param base GPIO peripheral base pointer(Typically GPIO)
 * @param port GPIO port number
 * @param mask GPIO pin number macro
 */
static inline void GPIO_SetPinsOutput(GPIO_Type *base, uint32_t port, uint32_t mask)
{
    base->SET[port] = mask;
}

/*!
 * @brief Sets the output level of the multiple GPIO pins to the logic 0.
 *
 * @param base GPIO peripheral base pointer(Typically GPIO)
 * @param port GPIO port number
 * @param mask GPIO pin number macro
 */
static inline void GPIO_ClearPinsOutput(GPIO_Type *base, uint32_t port, uint32_t mask)
{
    base->CLR[port] = mask;
}

/*!
 * @brief Reverses current output logic of the multiple GPIO pins.
 *
 * @param base GPIO peripheral base pointer(Typically GPIO)
 * @param port GPIO port number
 * @param mask GPIO pin number macro
 */
static inline void GPIO_TogglePinsOutput(GPIO_Type *base, uint32_t port, uint32_t mask)
{
    base->NOT[port] = mask;
}
/*@}*/

/*!
 * @brief Reads the current input value of the whole GPIO port.
 *
 * @param base GPIO peripheral base pointer(Typically GPIO)
 * @param port GPIO port number
 */
static inline uint32_t GPIO_ReadPinsInput(GPIO_Type *base, uint32_t port)
{
    return (uint32_t)base->PIN[port];
}

/*@}*/
/*! @name GPIO Mask Operations */
/*@{*/

/*!
 * @brief Sets port mask, 0 - enable pin, 1 - disable pin.
 *
 * @param base GPIO peripheral base pointer(Typically GPIO)
 * @param port GPIO port number
 * @param mask GPIO pin number macro
 */
static inline void GPIO_SetPortMask(GPIO_Type *base, uint32_t port, uint32_t mask)
{
    base->MASK[port] = mask;
}

/*!
 * @brief Sets the output level of the masked GPIO port. Only pins enabled by GPIO_SetPortMask() will be affected.
 *
 * @param base    GPIO peripheral base pointer(Typically GPIO)
 * @param port   GPIO port number
 * @param output  GPIO port output value.
 */
static inline void GPIO_WriteMPort(GPIO_Type *base, uint32_t port, uint32_t output)
{
    base->MPIN[port] = output;
}

/*!
 * @brief Reads the current input value of the masked GPIO port. Only pins enabled by GPIO_SetPortMask() will be
 * affected.
 *
 * @param base   GPIO peripheral base pointer(Typically GPIO)
 * @param port   GPIO port number
 * @retval       masked GPIO port value
 */
static inline uint32_t GPIO_ReadMPort(GPIO_Type *base, uint32_t port)
{
    return (uint32_t)base->MPIN[port];
}

/*@}*/

#if defined(__cplusplus)
}
#endif

/*!
 * @}
 */

#endif /* _LPC_GPIO_H_*/
