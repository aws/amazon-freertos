/*
 * Copyright (c) 2016-2019, Texas Instruments Incorporated
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * *  Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * *  Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * *  Neither the name of Texas Instruments Incorporated nor the names of
 *    its contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

 /*!***************************************************************************
 *  @file       Button.h
 *
 *  @brief      Button driver
 *
 *  @anchor ti_drivers_Button_Synopsis
 *  # Synopsis #
 *
 *  @code
 *  #include <ti/drivers/apps/Button.h>
 *
 *  int main(void)
 *  {
 *      Button_Params params;
 *      Button_Handle handle;
 *
 *      Button_Params_init(&params);
 *
 *      handle = Button_open(CONFIG_BUTTON0, buttonCallbackFxn, &params);
 *
 *      ...
 *  }
 *
 *  void buttonCallbackFxn(Button_Handle handle, Button_EventMask events)
 *  {
 *      if (events & Button_EV_CLICK)
 *      {
 *          // Received a click, handle app condition 0 etc
 *          handleAppCond(0);
 *      }
 *      if (events & Button_EV_LONGCLICKED)
 *      {
 *          // Long press, handle app condition 1 etc
 *          handleAppCond(1);
 *      }
 *      ...
 *  }
 *  @endcode
 *
 *  @anchor ti_drivers_Button_Examples
 *  ## Examples #
 *
 *  * @ref ti_drivers_Button_Examples_config "Generic Configuration"
 *
 *  ## Overview #
 *
 *  The Button driver simplifies interfacing push buttons. For example, push
 *  buttons on LaunchPads, BoosterPacks, or custom boards may easily be managed
 *  via the Button API. A given button instance may subscribe to one or several
 *  #Button_Events. When a subscribed event occurs, the user will receive a
 *  callback with the handle of the button and the event(s) that occured.
 *
 *  ## User requirements #
 *  Buttons use the @ref GPIO.h interface for interfacing with hardware, so a
 *  #GPIO_PinConfig array must exist and contain a config for the button pin.
 *  The user must statically allocate a #Button_Config array called
 *  Button_config. Each physical button should map to an index in
 *  Button_config.
 *
 *  ## Defining #Button_Config, #Button_Object and #Button_HWAttrs #
 *  Each structure must be defined by the application. The following
 *  example is for a MSP432 in which two buttons are setup.
 *  The following declarations are placed in "ti_drivers_config.h"
 *  and "ti_drivers_config.c" respectively. How the GPIO configs are defined
 *  are detailed in the next example.
 *
 *  @anchor ti_drivers_Button_Examples_config
 *
 *  "ti_drivers_config.h"
 *  @code
 *  #define CONFIG_BUTTON_0     0  //Button number 1
 *  #define CONFIG_BUTTON_1     1  //Button number 2
 *  @endcode
 *
 *  "ti_drivers_config.c"
 *  @code
 *  #include <Button.h>
 *
 *  Button_Object Button_object[2];
 *
 *  const  Button_HWAttrs Button_hwAttrs[2] = {
 *      {
 *          .gpioIndex = CONFIG_S1,
 *      },
 *      {
 *          .gpioIndex = CONFIG_S2,
 *      }
 *  };
 *
 *  const Button_Config Button_config[2] = {
 *      {
 *          .hwAttrs = &Button_hwAttrs[0],
 *          .object =  &Button_object[0],
 *      },
 *      {
 *          .hwAttrs = &Button_hwAttrs[1],
 *          .object =  &Button_object[1],
 *      },
 *  };
 *  @endcode
 *
 *  ##Setting up GPIO configurations #
 *
 *  The following example is for a MSP432.
 *  We are showing interfacing of two push buttons.  Each need a GPIO pin. The
 *  following definitions are in "ti_drivers_config.h" and
 *  "ti_drivers_config.c" respectively. This example uses GPIO pins 1.1 and
 *  1.4. The other GPIO configuration structures must exist, see @ref GPIO.h.
 *
 *  "ti_drivers_config.h"
 *  @code
 *  #define CONFIG_S1       0
 *  #define CONFIG_S2       1
 *  @endcode
 *
 *  "ti_drivers_config.c"
 *  @code
 *  #include <gpio.h>
 *  GPIO_PinConfig gpioPinConfigs[] = {
 *      GPIOMSP432_P1_1 | GPIO_CFG_IN_PU | GPIO_CFG_IN_INT_FALLING,
 *      GPIOMSP432_P1_4 | GPIO_CFG_IN_PU | GPIO_CFG_IN_INT_FALLING,
 *  }
 *
 *  @endcode
 ******************************************************************************
 */
#ifndef ti_drivers_Button__include
#define ti_drivers_Button__include

#include <stdint.h>
#include <stdbool.h>

/* Driver Header files */
#include <ti/drivers/GPIO.h>
#include <ti/drivers/dpl/ClockP.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Number of user defined Button configurations */
extern const uint_least8_t Button_count;

/*!
 *  @brief    Button configuration
 *
 *  Each #Button_Config represents a single physical button. It contains
 *  pointers to the button's #Button_HWAttrs and #Button_Object. The user must
 *  statically allocate all of these structures.
 */
typedef struct Button_Config
{
    /*! Pointer to a #Button_Object struct */
    void *object;

    /*! Pointer to a #Button_HWAttrs structure */
    void const *hwAttrs;
}Button_Config;

/*!
 *  @brief    A handle that is returned from a Button_open() call.
 *
 *  User will use this handle to interact with a given button instance.
 */
typedef struct Button_Config* Button_Handle;

/*!
 *  @brief    Button State
 *  @private
 *
 * This enumeration describes whether the button is pressed or released etc.
 * This is for internal state machine handling.
 */
typedef enum Button_State
{
    /*! Edge detected, debouncing */
    Button_PRESSING                     = 1,
    /*! Press verified, not detecting longpress */
    Button_PRESSED                      = 2,
    /*! Press verified, waiting for longpress timeout. */
    Button_LONGPRESSING                 = 3,
    /*! Longpress verified, waiting for neg-edge */
    Button_LONGPRESSED                  = 4,
    /*! Neg-edge received, debouncing */
    Button_RELEASING                    = 5,
    /*! Neg-edge received after long-press, debouncing. */
    Button_RELEASING_LONG               = 6,
    /*! Button release verified. */
    Button_RELEASED                     = 7,
    /*! EDGE detected doublepress */
    Button_DBLPRESS_DETECTION           = 8,
    /*! EDGE detected doublepress */
    Button_DBLPRESSING                  = 9,
    /*! DOUBLE PRESS verified, waiting for neg edge */
    Button_DBLPRESSED                   = 10,
    /*! DOUBLE PRESS verified, waiting for neg edge k*/
    Button_RELEASING_DBLPRESSED         = 11
} Button_State;

/*!
 *  @brief Button event flags
 *
 *  The event flags can be used by the user to subscribe to specific kinds of
 *  button actions and by the driver to signal which event caused a callback.
 */
typedef enum Button_Events
{
    /*! Button pressed down, may or may not subsequently have been released */
    Button_EV_PRESSED        = 0x01,
    /*! Button held down for more than tLongpress (ms) */
    Button_EV_LONGPRESSED    = 0x02,
    /*! Button released after press or longpress */
    Button_EV_RELEASED       = 0x04,
    /*! Button was pressed and released, but was not a long press */
    Button_EV_CLICKED        = 0x08,
    /*!
     * Button was pressed and released, and held for longer than
     * longPressDuration (ms)
     */
    Button_EV_LONGCLICKED    = 0x10,
    /*! Button was pressed when double click detection was active */
    Button_EV_DOUBLECLICKED  = 0x20,
} Button_Events;

/*! @brief Event subscription and notification mask type */
typedef uint8_t Button_EventMask;

/*!
 *  @brief    A handler to receive button callbacks.
 */
typedef void (*Button_Callback)(Button_Handle buttonHandle,
                                Button_EventMask buttonEvents);

/*!
 *  @brief    Button Pull settings
 *
 * This enumeration defines whether the GPIO connected to the button
 * is PULL UP or PULL DOWN
 */
typedef enum Button_Pull
{
    /* NOTE: DO NOT change the values of DOWN/UP from (0,1) */
    Button_PULL_DOWN   = 0,     /*!< Button is PULLED DOWN. */
    Button_PULL_UP     = 1,     /*!< Button is PULLED UP. */
    Button_PULL_NOTSET = 2      /*!< Button pull not set */
} Button_Pull;

/*!
 *  @brief    Hardware specific settings for a button
 *
 *  This structure should be defined and provided by the application.
 *  The index provided should correspond to a gpio pin in a #GPIO_PinConfig
 *  array. This gpio pin should be the pin connected to the button and must
 *  be configured as #GPIO_CFG_INPUT and #GPIO_CFG_IN_INT_FALLING.
 */
typedef struct Button_HWAttrs
{
    uint_least8_t gpioIndex;    /*!< GPIO configuration index. */
} Button_HWAttrs;

/*!
 *  @brief  Button State Variables
 *  @private
 *
 *  Each button instance needs set of variables to monitor its state.
 *  We group these variables under the structure Button_State.
 *
 *  @sa     Button_Params_init()
 */
typedef struct Button_StateVariables
{
    /*! Button state */
    Button_State   state;
    /*! Button pressed start time in milliseconds(ms) */
    uint32_t       pressedStartTime;
    /*! Button pressed duration (ms) */
    uint32_t       lastPressedDuration;
}Button_StateVariables;

/*!
 *  @brief    Internal to Button module. Members should not be accessed
 *            by the application.
 */
typedef struct Button_Object
{
    /*! Handle to clock used for timing */
    ClockP_Handle          clockHandle;

    /*! State variables for handling the debounce state machine */
    Button_StateVariables  buttonStateVariables;

    /*! Event subscription mask for the button */
    Button_EventMask       buttonEventMask;

    /*! Callback function for the button */
    Button_Callback        buttonCallback;

    /*! Debounce duration for the button in milliseconds(ms) */
    uint32_t               debounceDuration;

    /*! Long press duration is milliseconds(ms) */
    uint32_t               longPressDuration;

    /*! Double press detection timeout is milliseconds(ms) */
    uint32_t               doublePressDetectiontimeout;

    /*! Button pull(stored after reading from GPIO module) */
    Button_Pull            buttonPull;
} Button_Object;

/*!
 *  @brief  Button Parameters
 *
 *  Button parameters are used with the Button_open() call. Default values for
 *  these parameters are set using Button_Params_init().
 *
 *  @sa     Button_Params_init()
 */
typedef struct Button_Params
{
    /*! Debounce duration for the button in milliseconds(ms) */
    uint32_t        debounceDuration;

    /*! Long press duration is milliseconds(ms) */
    uint32_t        longPressDuration;

    /*! Double press detection timeout is milliseconds(ms) */
    uint32_t        doublePressDetectiontimeout;

    /*! Event subscription mask for the button */
    Button_EventMask    buttonEventMask;
} Button_Params;

/*!
 *  @brief  Function to close a Button specified by the #Button_Handle
 *
 *  @pre        Button_open() had to be called first.
 *
 *  @param[in]  handle    A #Button_Handle returned from Button_open() call
 *
 *  @return     True on success or false upon failure.
 */
extern bool Button_close(Button_Handle handle);

/*!
 *  @brief  Function to initialize Button driver.
 */
extern void Button_init();

/*!
 *  @brief  Function to open a given Button
 *
 *  Function to open a button instance corresponding to a #Button_Config in the
 *  Button_config array. The GPIO configurations must exist prior to calling
 *  this function. The #Button_Params may be used to specify runtime parameters.
 *
 *  @pre       Button_init() has to be called first
 *
 *  @param[in] buttonIndex    Logical button number indexed into
 *                            the Button_config table
 *
 *  @param[in] buttonCallback A #Button_Callback that is called when a desired
 *                            event occurs.
 *
 *  @param[in] *params        A pointer to #Button_Params structure. If NULL,
 *                            it will use default values.
 *
 *  @return  A #Button_Handle on success, or a NULL on failure.
 *
 *  @sa      Button_init()
 *  @sa      Button_Params_init()
 *  @sa      Button_close()
 */
extern Button_Handle Button_open(uint_least8_t buttonIndex,
                                 Button_Callback buttonCallback,
                                 Button_Params *params);

/*!
 *  @brief  Function to initialize a #Button_Params struct to its defaults
 *
 *  @param[in] params   A pointer to a #Button_Params structure that will be
 *                      initialized.
 *
 *  Default values
 *  ------------------------------------------------------------------
 *  parameter        | value        | description              | unit
 *  -----------------|--------------|--------------------------|------------
 *  debounceDuration | 10           | debounce duration        | ms
 *  longPressDuration| 2000         | long press duration      | ms
 *  buttonEventMask  | 0xFF         | subscribed to all events | NA
 */
extern void Button_Params_init(Button_Params *params);

/*!
 *  @brief  Function to return the lastPressedDuration (valid only for short
 *          press, long press)
 *
 *  The API returns last pressed duration and it is valid only for shortpress,
 *  longpress. If this API is called after receiving an event click or long
 *  click then the API returns the press duration which is time delta between
 *  the press and release of the button.
 *  @note This API call is only valid after a click or long click and not after
 *  a double click.
 *
 *  @param[in] handle   Pointer to the #Button_Handle of the desired button.
 *
 *  @return    time duration in milliseconds.
 *
 */
extern uint32_t Button_getLastPressedDuration(Button_Handle handle);

/*!
 *  @brief     Function to set callback function for the button instance
 *
 *  @param[in] handle           A #Button_Handle returned from Button_open()
 *
 *  @param[in] buttonCallback   button callback function
 *
 */
extern void Button_setCallback(Button_Handle handle,
                               Button_Callback buttonCallback);

/*!
 *  @brief  This is the GPIO interrupt callback function which is called on a
 *          button press or release. This is internally used by button module.
 *
 *  This function is internally used by button module for receiving the GPIO
 *  interrupt callbacks. This is exposed to the application for wake up cases.
 *  In some of the MCUs, when in LPDS(Low power deep sleep) the GPIO interrupt
 *  is consumed for wake up, and in order to make the button module work the
 *  the application has to call this API with the index of the GPIO pin which
 *  actually was the reason for the wake up.
 *
 *  @param[in]  index   Index of the GPIO for which the button press has to be
 *                      detected. This is an index in #GPIO_PinConfig array.
 */
void Button_gpioCallbackFxn(uint_least8_t index);

#ifdef __cplusplus
}
#endif

#endif /* ti_drivers_Button__include */
