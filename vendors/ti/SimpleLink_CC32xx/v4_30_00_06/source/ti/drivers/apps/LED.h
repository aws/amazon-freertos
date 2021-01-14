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

/*!*****************************************************************************
 *  @file       LED.h
 *
 *  @brief      LED driver
 *
 *  The LED driver is provided for easy access to common LED functionality.
 *  All functionality can be replicated using the GPIO.h and PWM.h APIs.
 *
 *  @anchor ti_drivers_LED_Synopsis
 *  # Synopsis #
 *
 *  @code
 *  #include <ti/drivers/apps/LED.h>
 *
 *  LED_Handle handle;
 *  LED_Params ledParams;
 *
 *  // Assume our LED is configured to be a PWM LED
 *  LED_Params_init(&ledParams);
 *  ledHandle = LED_open(CONFIG_LED0, &ledParams);
 *
 *  // Turn on, set brightness, and blink
 *  LED_setOn(handle, 80);
 *  LED_startBlinking(handle, 500, LED_BLINK_FOREVER);
 *
 *  LED_setOff(handle);
 *  LED_close(handle);
 *
 *  @endcode
 *
 *  @anchor ti_drivers_LED_Examples
 *  ## Examples #
 *
 *  * @ref ti_drivers_LED_Examples_config_array "Generic Configuration"
 *  * @ref ti_drivers_LED_Examples_gpio_config "GPIO Configuration"
 *  * @ref ti_drivers_LED_Examples_pwm_led "PWM Mode"
 *
 *  # Operation #
 *  LED driver simplifies using an LED (may be GPIO or PWM controlled)
 *  available on board and supports following operations -
 *
 *      1. To Turn ON/OFF
 *      2. Blink with requested delay, stop when requested
 *      3. Vary brightness (can only be done to a PWM controlled LED)
 *      4. Toggle
 *
 *  There are also APIs to open and close an LED handle and also one to get
 *  current state of a LED. User can request to set a LED into particular state
 *  while opening itself i.e. to start blink as part of LED_open() call.
 *
 *  LED_init() must be called before using LED_open().
 *
 *  ## Defining #LED_Config, #LED_Object and #LED_HWAttrs #
 *  To use the LED driver, an application has to indicate how many LEDs it
 *  wants to operate, of what type (PWM or GPIO controlled), and which GPIO or
 *  PWM to index for each LED.
 *
 *  Each structure must be defined by the application. The following
 *  example is for an MSP432P401R platform in which four LEDs are available
 *  on board.
 *  The following declarations are placed in "ti_drivers_config.c".
 *  How the gpio indices are defined is detailed in the next section.
 *
 *  ### LED_config structures #
 *  @anchor ti_drivers_LED_Examples_config_array
 *  "ti_drivers_config.c"
 *  @code
 *  #include <ti/drivers/apps/LED.h>
 *
 *  LED_Object LED_object[4];
 *
 *  const LED_HWAttrs LED_hwAttrs[4] = {
 *          {
 *              .index = CONFIG_LED1,
 *              .type  = LED_GPIO_CONTROLLED
 *          },
 *          {
 *              .index = CONFIG_LED_RED,
 *              .type  = LED_GPIO_CONTROLLED
 *          },
 *          {
 *              .index = CONFIG_NA_GPIO_PWMLED,
 *              .type  = LED_PWM_CONTROLLED
 *          },
 *          {
 *              .index = CONFIG_NA_GPIO_PWMLED,
 *              .type  = LED_PWM_CONTROLLED
 *          }
 *  };
 *
 *  const LED_Config LED_config[] = {
 *      {
 *          .object =  &LED_object[0],
 *          .hwAttrs = &LED_hwAttrs[0],
 *      },
 *      {
 *          .object =  &LED_object[1],
 *          .hwAttrs = &LED_hwAttrs[1],
 *      },
 *      {
 *          .object =  &LED_object[2],
 *          .hwAttrs = &LED_hwAttrs[2],
 *      },
 *      {
 *          .object =  &LED_object[3],
 *          .hwAttrs = &LED_hwAttrs[3],
 *      }
 *  };
 *
 *  uint32_t LED_count = 4;
 *
 *  @endcode
 *
 *  ##Setting up a GPIO controlled LED #
 *  The following code snippet shows how a GPIO pin controlling an LED is
 *  configured. The index the user provides to LED_open() corresponds to an
 *  entry in the #GPIO_PinConfig array which will source the LED. It is the
 *  user's responsibility to ensure that the pin is configured properly in the
 *  pin array. Typically this means configuring the pin as an output.
 *
 *  ### GPIO controlled LED #
 *  @anchor ti_drivers_LED_Examples_gpio_config
 *
 *  The following definitions are in
 *  "ti_drivers_config.h" and "ti_drivers_config.c" respectively. This
 *  example uses GPIO pins 1.0 and 2.0 which control LED1 and RED LED on
 *  LaunchPad respectively. In addition to the structures shown below, the
 *  other GPIO configuration data must exist. See @ref GPIO.h.
 *
 *  "ti_drivers_config.h"
 *  @code
 *  #define CONFIG_LED1         0
 *  #define CONFIG_LED_RED      1
 *  @endcode
 *
 *  "ti_drivers_config.c"
 *  @code
 *  #include <ti/drivers/GPIO.h>
 *  GPIO_PinConfig gpioPinConfigs[] = {
 *      GPIOMSP432_P1_0 | GPIO_CFG_OUT_STD | GPIO_CFG_OUT_STR_HIGH | GPIO_CFG_OUT_LOW,
 *      GPIOMSP432_P2_0 | GPIO_CFG_OUT_STD | GPIO_CFG_OUT_STR_HIGH | GPIO_CFG_OUT_LOW,
 *  }
 *
 *  @endcode
 *
 *  ## Configuring a PWM controlled LED #
 *  The LED driver allows for an LED to be driven by the PWM driver. This allows
 *  the user to set a brightness level in addition to the other LED features.
 *  The user must specify in the #LED_HWAttrs of each #LED_Config entry which
 *  #PWM_Config the LED instance is allowed to use. LED instances cannot share
 *  a PWM instance.
 *
 *  The user may specify the period of the PWM signal in the #LED_Params passed
 *  to LED_open(). This is not to be confused with #LED_Params.blinkPeriod
 *  which specifies the default blink period.
 *
 *  ### Opening a PWM LED #
 *  @anchor ti_drivers_LED_Examples_pwm_led
 *
 *  We will borrow the 3rd LED_config entry from the
 *  @ref ti_drivers_LED_Examples_config_array
 *
 *  In "ti_drivers_config.h"
 *  @code
 *  #define CONFIG_LED0     0
 *  @endcode
 *
 *  In application code:
 *  @code
 *  #include <ti/drivers/apps/LED.h>
 *
 *  LED_Handle LEDHandle;
 *  LED_Params ledParams;
 *
 *  LED_Params_init(&ledParams);
 *  ledParams.pwmPeriod = 100; // 0.1 ms period
 *  ledParams.blinkPeriod = 500; // LED will toggle twice a second
 *  ledParams.setState = LED_STATE_BLINKING; // Start LED blink on open
 *  ledHandle = LED_open(CONFIG_LED0, &ledParams); // Open the first LED_Config
 *
 *  // Turn on at half brightness level
 *  LED_setOn(ledHandle, 50);
 *  @endcode
 *
 *******************************************************************************
 */


#ifndef ti_drivers_LED__include
#define ti_drivers_LED__include

#include <stdint.h>
#include <stdbool.h>

/* Driver Header files */
#include <ti/drivers/GPIO.h>
#include <ti/drivers/PWM.h>
#include <ti/drivers/dpl/ClockP.h>

#ifdef __cplusplus
extern "C" {
#endif

#define LED_BRIGHTNESS_MAX      100U  /* Max brightness in % is 100%*/
#define LED_BRIGHTNESS_MIN      0U    /* Max brightness in % is 0%*/

#define LED_ON                  1U
#define LED_OFF                 0U

#define LED_BLINK_FOREVER       0xFFFF

/* Number of user defined LED configurations */
extern const uint_least8_t LED_count;

/*!
*  @brief    LED types based on control source
*
*  A LED can be controlled by GPIO or PWM. Only a PWM controlled LED can
*  be operated to show brightness variation. An unopened instance will be of
*  type #LED_NONE.
*/
typedef enum {
    LED_NONE = 0,
    LED_GPIO_CONTROLLED,
    LED_PWM_CONTROLLED
} LED_Type;

/*!
*  @brief    LED State
*
*  A LED can be in OFF, ON or BLINKING state
*
*  State of particular LED may be tied with a warning/alert in system
*  which a thread/task may want to know.
*/
typedef enum {
    LED_STATE_OFF =  0,
    LED_STATE_ON,
    LED_STATE_BLINKING
} LED_State;

/*!
 *  @brief    LED configuration
 *
 *  The LED_Config structure contains a set of pointers used to characterize
 *  the LED driver implementation.
 *
 *  This structure needs to be defined and provided by the application and will
 *  be NULL terminated.
 */
typedef struct {
    /*! Pointer to drivers internal data state object */
    void        *object;
    /*! Pointer to a driver specific hardware attributes structure */
    void const  *hwAttrs;
} LED_Config;

/*!
 *  @brief    A handle that is returned from a LED_open() call.
 */
typedef LED_Config* LED_Handle;

/*!
 *  @brief    Hardware specific settings for a LED module.
 *
 *  This structure should be defined and provided by the application. The
 *  index should be correspond to the desired pin in either the PWM or GPIO
 *  config tables depending on the requested #LED_Type.
 */
typedef struct {
    uint_least8_t index; /*!< Index into GPIO or PWM config array */
    LED_Type      type; /*<! GPIO (binary) or PWM (dimmable) control */
} LED_HWAttrs;

/*!
 *  @brief    LED Object structure
 *
*  The application must not access any member variables of this structure!
 */
typedef struct {
    uint32_t        pwmPeriod;    /*!< pwmPeriod(us) of controlling PWM */
    PWM_Handle      pwmHandle;    /*!< Used for PWM calls if PWM type LED */
    ClockP_Handle   clockHandle;  /*!< Handle to clock used for blinking */
    ClockP_Struct   clock;        /*!< Clock internal data */
    LED_State       state;        /*!< Current State of LED */
    LED_State       rawState;     /*!< rawState maintains actual state On or Off
                                       while blinking which is super state */
    LED_Type        ledType;      /*!< may be either GPIO or PWM controlled */
    uint8_t         brightness;   /*!< Varying min-max(0-100%) for PWM LED) */
    uint_least8_t   gpioIndex;    /*!< Index into #GPIO_PinConfig array */
    uint16_t        togglePeriod; /*!< Toggleperiod(ms), 0 for non-blinking LED
                                       This is half of blinkPeriod: two toggles
                                       is one blink */
    uint16_t        blinkCount;   /*!< Blinkcount, 0 for non-blinking LED */
} LED_Object;

/*!
 *  @brief  LED Parameters
 *
 *  LED parameters are used with the LED_open() call. Default values for
 *  these parameters are set using LED_Params_init(). It contains brightness
 *  field which will be used to control brightness of a LED and also blink
 *  period if user wants to set LED in blinking mode.
 *
 *  @sa     LED_Params_init()
 */
typedef struct {
    uint32_t    pwmPeriod;      /*!< pwmPeriod(us) of controlling PWM */
    uint16_t    blinkPeriod;    /*!< param to set blink period (in ms) */
    uint8_t     brightness;     /*!< may vary from 0-100% for PWM LED */
    LED_State   setState;       /*!< request to set a LED state(eg blinking) */
} LED_Params;

/*!
 *  @brief  Function to close a LED specified by the LED handle
 *
 *  This call will destruct associated clock, turn off LED, and close the PWM
 *  instance if applicable.
 *
 *  @pre    LED_open() had to be called first.
 *
 *  @param  ledHandle    A #LED_Handle returned from LED_open()
 *
 */
extern void LED_close(LED_Handle ledHandle);

/*!
 *  @brief  Function to get LED state.
 *
 *  This function may be useful in scenarios if a LED state(ON/OFF/BLINKING) is
 *  tied with some system warning/alerts
 *
 *  @param  ledHandle    A #LED_Handle returned from LED_open()
 *
 *  @return  The LED State
 */
extern LED_State LED_getState(LED_Handle ledHandle);

/*!
 *  @brief  Function to initialize LED driver.
 *
 *  This function will initialize the LED driver.
 */
extern void LED_init();

/*!
 *  @brief  Function to open an instance of LED
 *
 *  Function to open an LED instance in the LED_config array. The GPIO or PWM
 *  configuartions must already exist before this function is called. The
 *  #LED_Params input can be used to specify the run time options of the LED
 *  instance.
 *
 *  @pre    LED_init() has to be called first
 *
 *  @param  index          Index into the LED_config array specifying the
 *                         #LED_Config that is to be used to open the LED.
 *
 *  @param  *params        A pointer to #LED_Params structure. If NULL, it
 *                         will use default values.
 *
 *  @return  A LED_Handle on success, or a NULL on failure.
 *
 *  @sa      LED_init()
 *  @sa      LED_Params_init()
 *  @sa      LED_close()
 */
LED_Handle LED_open(uint_least8_t index, LED_Params *params);

/*!
 *  @brief  Function to initialize a #LED_Params struct to its defaults
 *
 *  @param  params      A pointer to #LED_Params structure for
 *                      initialization.
 *
 *  The default parameters are:
 *  - LED initially off
 *  - Blink period of zero
 *  - Max brightness (for PWM LED only)
 *  - PWM period of 1 ms (for PWM LED only)
 */
extern void LED_Params_init(LED_Params *params);

/*!
 *  @brief  Function to set brightness level of a LED
 *
 *  Ignores brightness settings if LED is not PWM controlled. Fails if
 *  requested brightness is above 100%.
 *
 *  @param  ledHandle    A #LED_Handle
 *
 *  @param  level        Brightness level in terms of percentage (0-100)
 *
 *  @return true on success or false upon failure.
 */
extern bool LED_setBrightnessLevel(LED_Handle ledHandle, uint8_t level);

/*!
 *  @brief  Function to turn off an LED
 *
 *  @param  ledHandle    An #LED_Handle
 *
 *  @return true on success or false upon failure.
 */
extern bool LED_setOff(LED_Handle ledHandle);

/*!
 *  @brief  Function to turn on an LED
 *
 *  @param  ledHandle    An #LED_Handle
 *
 *  @param  brightness   Brightness level in terms of percentage 0-100%.
 *                       Is ignored for non PWM LEDs.
 *
 *  @return true on success or false upon failure.
 */
extern bool LED_setOn(LED_Handle ledHandle, uint8_t brightness);

/*!
 *  @brief  Function to start an LED blinking
 *
 *  @param  ledHandle    An #LED_Handle
 *
 *  @param  blinkPeriod  Value in ms which determines how often the LED
 *                       blinks. A value of 1000 will cause the LED to
 *                       blink once a second. The maximum value is ~65 seconds
 *                       or 0xFFFF ms.
 *
 *  @param  blinkCount   If not set to #LED_BLINK_FOREVER, the LED will blink
 *                       the specified number of times and then will turn off.
 *                       A value of zero will stop the LED blinking. Maximum
 *                       number of blinks is 0x7FFF or 32,767 blinks. An input
 *                       exceeding this value will be truncated to 0x7FFF.
 */
extern void LED_startBlinking(LED_Handle ledHandle,
                              uint16_t blinkPeriod,
                              uint16_t blinkCount);

/*!
 *  @brief  Function to stop an LED blinking
 *
 *  @param  ledHandle    An #LED_Handle
 *
 */
extern void LED_stopBlinking(LED_Handle ledHandle);

/*!
 *  @brief  Function to toggle an LED
 *
 *  @param  ledHandle    An #LED_Handle
 *
 */
extern void LED_toggle(LED_Handle ledHandle);

/*!
 *  @brief  Specify binary state of an LED
 *
 *  @param  ledHandle    An #LED_Handle
 *
 *  @param  value        TRUE for on, FALSE for off
 *
 */
extern void LED_write(LED_Handle ledHandle, bool value);

#ifdef __cplusplus
}
#endif

#endif /* ti_drivers_LED__include */
