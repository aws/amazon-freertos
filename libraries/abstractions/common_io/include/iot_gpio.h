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

/**
 * @file    iot_gpio.h
 * @brief   This file contains all the GPIO HAL API definitions.
 *          This GPIO interface APIs only provides pin-level functions. To modify a GPIO pin, a handle must be obtained
 *          by calling the iot_gpio_open() function. This handle must be passed to other functions in this interface.
 *          Handling multiple GPIOs simultaneously in the same bank or different banks is outside of the scope of this file.
 *          User can modify multiple pins sequentially using this interface by iterating over multiple GPIOs in a loop.
 */

#ifndef _IOT_GPIO_H_
#define _IOT_GPIO_H_

/**
 * @defgroup iot_gpio GPIO HAL APIs
 * @{
 */

/**
 * @brief Error codes
 */
#define IOT_GPIO_SUCCESS                   ( 0 )    /*!< GPIO operation completed succesfully. */
#define IOT_GPIO_INVALID_VALUE             ( 1 )    /*!< At least one parameter is invalid. */
#define IOT_GPIO_READ_FAILED               ( 2 )    /*!< GPIO read operation failed. */
#define IOT_GPIO_WRITE_FAILED              ( 3 )    /*!< GPIO write operation failed. */
#define IOT_GPIO_FUNCTION_NOT_SUPPORTED    ( 4 )    /*!< GPIO operation not supported. */

/**
 * @brief enum for configuring GPIO input/output direction.
 */
typedef enum
{
    eGpioDirectionInput, /*!< Configure GPIO pin direction for input. */
    eGpioDirectionOutput /*!< Configure GPIO pin direction for output. */
} IotGpioDirection_t;

/**
 * @brief enum for configuring GPIO output type.
 */
typedef enum
{
    eGpioOpenDrain, /*!< Configure GPIO pin mode as open drain. */
    eGpioPushPull   /*!< Configure GPIO pin mode as push pull. */
} IotGpioOutputMode_t;

/**
 * @brief   GPIO pin internal pull state. Sets the default state for output pins.
 */
typedef enum
{
    eGpioPullNone, /*!< Configure GPIO pin with no pull direction. */
    eGpioPullUp,   /*!< Configure GPIO pin as pull up. */
    eGpioPullDown  /*!< Configure GPIO pin as pull down. */
} IotGpioPull_t;

/**
 * @brief   GPIO pin interrupt config types.
 */
typedef enum
{
    eGpioInterruptNone,    /*!< Configure GPIO pin to not generate an interrupt. */
    eGpioInterruptRising,  /*!< Configure GPIO pin to generate an interrupt when signal rises. */
    eGpioInterruptFalling, /*!< Configure GPIO pin to generate an interrupt when signal falls. */
    eGpioInterruptEdge,    /*!< Configure GPIO pin to generate an interrupt when either rising or falling. */
    eGpioInterruptLow,     /*!< Configure GPIO pin to generate an interrupt when signal is low. */
    eGpioInterruptHigh,    /*!< Configure GPIO pin to generate an interrupt when signal is high. */
} IotGpioInterrupt_t;

/**
 * @brief Ioctl request types.
 */
typedef enum
{
    eSetGpioFunction,      /*!< Set GPIO function. Takes int32_t as input to set the alternate function.
                            * The value of the alternate function depends on the HW */
    eSetGpioDirection,     /*!< Set GPIO Direction. Takes input type IotGpioDirection_t */
    eSetGpioPull,          /*!< Set GPIO Pull mode. Takes input type IotGpioPull_t */
    eSetGpioOutputMode,    /*!< Set GPIO output type. Takes input type IotGpioOutputType_t */
    eSetGpioInterrupt,     /*!< Set GPIO Interrupt type. This configures the GPIO to generate an interrupt based on the configuration.
                            * Takes input type IotGpioInterrupt_t */
    eSetGpioSpeed,         /*!< Set GPIO Speed. Takes a int32_t value based on the underlying HW support */
    eSetGpioDriveStrength, /*!< Set GPIO Drive Strength. Takes an int32_t value based on the underlying HW support */
    eGetGpioFunction,      /*!< Get GPIO function setting. Returns an int32_t */
    eGetGpioDirection,     /*!< Get GPIO Direction setting. Returns IotGpioDirection_t */
    eGetGpioPull,          /*!< Get GPIO Pull mode setting. Returns IotGpioPull_t */
    eGetGpioOutputType,    /*!< Get GPIO output type. Returns IotGpioOutputType_t */
    eGetGpioInterrupt,     /*!< Get GPIO Interrupt config. Returns IotGpioInterrupt_t type */
    eGetGpioSpeed,         /*!< Get GPIO Speed setting. Returns an int32_t type */
    eGetGpioDriveStrength  /*!< Get GPIO Drive Strength. Returns int32_t type */
} IotGpioIoctlRequest_t;

/**
 * @brief   GPIO descriptor type defined in the source file.
 */
struct                  IotGpioDescriptor;

/**
 * @brief   IotGpioHandle_t type is the GPIO handle returned by calling iot_gpio_open()
 *          this is initialized in open and returned to caller. Caller must pass this pointer
 *          to the rest of the APIs.
 */
typedef struct IotGpioDescriptor * IotGpioHandle_t;

/**
 * @brief   GPIO interrupt callback type. This callback is passed
 *          to the driver by using iot_gpio_set_callback API.
 * @param[out]  ucPinState      The variable which has state of the GPIO pin.
 * @param[in]   pvUserContext   User Context passed when setting the callback.
 *                              This is not used by the driver, but just passed back to the user
 *                              in the callback.
 */
typedef void ( * IotGpioCallback_t )( uint8_t ucPinState,
                                      void * pvUserContext );

/**
 * @brief   iot_gpio_open is used to open the GPIO handle.
 *          The application must call this function to open desired GPIO and use other functions.
 *
 * @param[in]   lGpioNumber The logical GPIO number to open. It depends on the
 *                          implementation to map logical GPIO number to physical GPIO port
 *                          and pin.
 *
 * @return
 *   - handle to the GPIO peripheral if everything succeeds
 *   - NULL, if
 *    - invalid instance number
 *    - open same instance more than once before closing it.
 */
IotGpioHandle_t iot_gpio_open( int32_t lGpioNumber );

/**
 * @brief   iot_gpio_set_callback is used to set the callback to be called when an
 *          interrupt is tirggered.
 *
 * @note Single callback is used for both read_async and write_async. Newly set callback overrides the one previously set
 * @note This callback will not be invoked when synchronous operation completes.
 * @note This callback is per handle. Each instance has its own callback.
 *
 * @warning If input handle or if callback function is NULL, this function silently takes no action.
 *
 * @param[in] pxGpio    The GPIO handle returned in the open() call.
 * @param[in] xGpioCallback The callback function to be called on interrupt.
 * @param[in] pvUserContext The user context to be passed back when callback is called.
 */
void iot_gpio_set_callback( IotGpioHandle_t const pxGpio,
                            IotGpioCallback_t xGpioCallback,
                            void * pvUserContext );

/**
 * @brief   iot_gpio_read_sync is used to read data from GPIO pin in blocking mode.
 *
 * @param[in]   pxGpio      The GPIO handle returned in the open() call.
 * @param[out]  pucPinState The variable which reads state of the GPIO pin.
 *
 * @return
 *   - IOT_GPIO_SUCCESS on success
 *   - IOT_GPIO_INVALID_VALUE if pxGpio or pucPinState are NULL
 *   - IOT_GPIO_READ_FAILED on failure.
 *
 */
int32_t iot_gpio_read_sync( IotGpioHandle_t const pxGpio,
                            uint8_t * pucPinState );

/**
 * @brief   iot_gpio_write_sync is used to write data into the GPIO pin in blocking mode.
 *
 * @param[in]   pxGpio      The GPIO handle returned in the open() call.
 * @param[in]   ucState     The value to write into the GPIO pin.
 *
 * @return
 *   - IOT_GPIO_SUCCESS on success
 *   - IOT_GPIO_INVALID_VALUE if pxGpio or pucPinState are NULL
 *   - IOT_GPIO_WRITE_FAILED on failure.
 */
int32_t iot_gpio_write_sync( IotGpioHandle_t const pxGpio,
                             uint8_t ucPinState );

/**
 * @brief   iot_gpio_close is used to deinitializes the GPIO pin to default value and close the handle.
 *          The application should call this function to reset and deinitialize the GPIO pin.
 *
 * @param[in]   pxGpio The GPIO handle returned in the open() call.
 *
 * @return
 *   - IOT_GPIO_SUCCESS on success
 *   - IOT_GPIO_INVALID_VALUE if
 *      - pxGpio handle is NULL
 *      - if is not in open state (already closed).
 */
int32_t iot_gpio_close( IotGpioHandle_t const pxGpio );

/**
 * @brief   iot_gpio_ioctl is used to configure GPIO pin options.
 *          The application should call this function to configure various GPIO
 *          pin options: pin function, I/O direction, pin internal pull mode,
 *          drive strength, slew rate etc
 *
 * @param[in] pxGpio        The GPIO handle returned in the open() call.
 * @param[in] xRequest      One of IotGpioIoctlRequest_t enum
 * @param[in/out] pvBuffer  Buffer holding GPIO set or get values.
 *
 * @return
 *   - IOT_GPIO_SUCCESS on success
 *   - IOT_GPIO_INVALID_VALUE on NULL handle, invalid request, or NULL buffer when required.
 *   - IOT_GPIO_FUNCTION_NOT_SUPPORTED is only valid for
 *      - eSetGpioFunction / eGetGpioFunction
 *      - eSetGpioSpeed / eGetGpioSpeed
 *      - eSetGpioDriveStrength / eGetGpioDriveStrength
 *      - eSetGpioInterrupt / eGetGpioInterrupt
 */
int32_t iot_gpio_ioctl( IotGpioHandle_t const pxGpio,
                        IotGpioIoctlRequest_t xRequest,
                        void * const pvBuffer );

/**
 * @}
 */

#endif /* _IOT_GPIO_H_ */
