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

/*
 *  ======== LED.c ========
 */

/* Drivers */
#include <ti/drivers/GPIO.h>
#include <ti/drivers/PWM.h>
#include <ti/drivers/dpl/ClockP.h>

/* Module Header */
#include <ti/drivers/apps/LED.h>

#define LED_PWMPERIOD_1MS       1000U /* Default PWM period is 1 ms*/

/* Default LED parameters structure */
const LED_Params LED_defaultParams = {
    .pwmPeriod   = LED_PWMPERIOD_1MS,      /* Default PWM period is 1 ms*/
    .brightness  = LED_BRIGHTNESS_MAX,     /* Start at max brightness*/
    .setState    = LED_STATE_OFF,          /* Set LED state to OFF*/
    .blinkPeriod = 0                       /* No blinking*/
};

extern LED_Config LED_config[];

/* Number of user defined LED configs */
extern const uint_least8_t LED_count;

/* Local functions: */

/*
 *  ======== clockTimeoutHandler ========
 * Called when blinker clock times out
 */
static void clockTimeoutHandler(uintptr_t arg)
{
    LED_Object * obj = ((LED_Handle)arg)->object;
    ClockP_setTimeout(obj->clockHandle, obj->togglePeriod);
    ClockP_start(obj->clockHandle);
    LED_toggle((LED_Handle)arg);

    /* Handle blink counting */
    if(obj->blinkCount != LED_BLINK_FOREVER &&
       obj->blinkCount != 0)
    {
        obj->blinkCount--;
        if(obj->blinkCount == 0)
        {
            /* Hit the requested number of blinks */
            LED_stopBlinking((LED_Handle)arg);
        }
    }
}

/*
 *  ======== msToTicks ========
 * Convert milliseconds to system ticks.
 * If passed zero, returns zero. If passed a ms value that converts to less
 * than one system tick, returns one.
 */
static uint32_t msToTicks(uint32_t ms)
{
    uint32_t ticks;

    if(ms == 0)
    {
        return(0);
    }

    ticks = (ms * 1000)/ClockP_getSystemTickPeriod();
    if(ticks == 0)
    {
        ticks = 1;
    }
    return(ticks);
}

/* API functions: */

/*
 *  ======== LED_close ========
 *  Closes an instance of a LED sensor.
 */
void LED_close(LED_Handle ledHandle)
{
    LED_Object *obj = (LED_Object *)(ledHandle->object);

    LED_stopBlinking(ledHandle);
    LED_setOff(ledHandle);

    /* Delete clock instance */
    ClockP_delete(obj->clockHandle);
    obj->clockHandle = NULL;

    /* Close PWM handle if applicable */
    if(obj->pwmHandle != NULL)
    {
       PWM_close(obj->pwmHandle);
       obj->pwmHandle = NULL;
    }

    /* Set type to NONE to indicate the instance is closed */
    obj->ledType = LED_NONE;
}

/*
 *  ======== LED_getState ========
 */
LED_State LED_getState(LED_Handle ledHandle)
{
    LED_Object *obj = (LED_Object *)(ledHandle->object);

    return(obj->state);
}

/*
 *  ======== LED_init ========
 */
void LED_init()
{
    /*
     * Must NOT call GPIO or PWM init here as the weak symbol may be used.
     * Init will be done at open time.
     */
}

/*
 *  ======== LED_open ========
 * Sets up LED and returns LED_Handle
 */
LED_Handle LED_open(uint_least8_t ledIndex, LED_Params *params)
{
    LED_Handle ledHandle;
    LED_Object *obj;
    LED_HWAttrs *hw;
    PWM_Params pwmParams;
    ClockP_Params clockParams;

    ledHandle = (LED_Handle)(&LED_config[ledIndex]);
    obj = (LED_Object *)(LED_config[ledIndex].object);
    hw = (LED_HWAttrs *)(LED_config[ledIndex].hwAttrs);

    /* ledIndex cannot be more than number of available LEDs */
    if((ledIndex >= LED_count))
    {
        return(NULL);
    }

    /* If params are NULL use defaults. */
    if(params == NULL)
    {
        params = (LED_Params *)&LED_defaultParams;
    }

    /* If we already have a ledType then the instance is already open */
    if(obj->ledType != LED_NONE)
    {
        return(NULL);
    }

    if(hw->type == LED_PWM_CONTROLLED)
    {
        obj->ledType = LED_PWM_CONTROLLED;

        /* PWM init must only be called if a PWM_config is strongly declared */
        PWM_init();

        PWM_Params_init(&pwmParams);
        pwmParams.periodValue = params->pwmPeriod;
        pwmParams.dutyUnits = PWM_DUTY_US;
        pwmParams.periodUnits = PWM_PERIOD_US;
        pwmParams.dutyValue = 0;

        /* Open the PWM instance */
        obj->pwmHandle = PWM_open(hw->index, &pwmParams);
        if(NULL == obj->pwmHandle)
        {
            return(NULL);
        }

        /* Handle params and start conditions */
        PWM_start(obj->pwmHandle);
        if(params->setState != LED_STATE_OFF)
        {
            PWM_setDuty(obj->pwmHandle,
                        params->pwmPeriod * params->brightness / 100);
        }
    }
    else
    {
        /* GPIO init must only be called if a GPIO_config is strongly declared */
        GPIO_init();
        /* Store gpio index so we don't have to potentially read from flash */
        obj->gpioIndex = hw->index;
        obj->ledType = LED_GPIO_CONTROLLED;
    }

    /* Create the clock instance */
    ClockP_Params_init(&clockParams);
    clockParams.startFlag = false;
    clockParams.arg = (uintptr_t)ledHandle;
    clockParams.period = 0; // One shot clock
    obj->clockHandle = ClockP_create(clockTimeoutHandler, 0, &clockParams);
    if(NULL == obj->clockHandle)
    {
        /* Also close PWM if we opened one */
        if(obj->pwmHandle != NULL)
        {
            PWM_close(obj->pwmHandle);
        }
        return(NULL);
    }

    /* Update Object fields*/
    obj->pwmPeriod  = params->pwmPeriod;
    obj->brightness = params->brightness;

    /* Set LED state, default is Off if setState is not modified by user*/
    switch (params->setState)
    {
        case LED_STATE_OFF:
            LED_setOff(ledHandle);
            break;

        case LED_STATE_ON:
            LED_setOn(ledHandle, obj->brightness);
            break;

        case LED_STATE_BLINKING:
            LED_startBlinking(ledHandle,
                              (params->blinkPeriod)/2,
                              LED_BLINK_FOREVER);
            break;

        default:
            /* Invalid setState value*/
            break;
    }

    return(ledHandle);
}

/*
 *  ======== LED_Params_init ========
 * Initialize a LED_Params struct to default settings.
 */
void LED_Params_init(LED_Params *params)
{
    *params = LED_defaultParams;
}

/*
 *  ======== LED_setBrightnessLevel ========
 * Sets brightness level as requested.
 */
bool LED_setBrightnessLevel(LED_Handle ledHandle, uint8_t level)
{
    bool ret;
    uint32_t duty = 0;
    LED_Object *obj = (LED_Object *)(ledHandle->object);

    /* If in GPIO mode or a pwm handle was not provided, fail */
    if(NULL == obj->pwmHandle)
    {
        return(false);
    }

    /* Report false if brightness request is more than maximum(100%) level */
    if(level > LED_BRIGHTNESS_MAX)
    {
        return(false);
    }

    /* Calculate duty based on requested level and set that */
    duty = (obj->pwmPeriod * level)/100;
    if(PWM_setDuty(obj->pwmHandle, duty) == PWM_STATUS_SUCCESS)
    {
        ret = true;
        obj->brightness = level;
    }
    else
    {
        ret = false;
    }

    if(duty > 0)
    {
        obj->rawState = LED_STATE_ON;
    }
    else
    {
        obj->rawState = LED_STATE_OFF;
    }

    return(ret);
}

/*
 *  ======== LED_setOff ========
 *  Turns Off a specified a LED sensor.
 */
bool LED_setOff(LED_Handle ledHandle)
{
    LED_Object *obj = (LED_Object *)(ledHandle->object);
    uint16_t level;
    bool ret = true;

    obj->rawState = LED_STATE_OFF;

    if(obj->ledType == LED_GPIO_CONTROLLED)
    {
        GPIO_write(obj->gpioIndex, LED_OFF);
    }

    /* For PWM LED, set brightness to zero
     * Also, restoring brightness level which was there before turning it Off
     * so that Toggle APIs can set same brightness while turning it On */
    else
    {
        level = obj->brightness;
        ret = LED_setBrightnessLevel(ledHandle, LED_BRIGHTNESS_MIN);
        obj->brightness = level;
    }

    /* Set LED state
     * If LED is blinking, which is a separate state(mix of ON + OFF), no need
     * to change state; rawState contains the actual ON or OFF state*/
    if(obj->state != LED_STATE_BLINKING)
    {
        obj->state = LED_STATE_OFF;
    }

    return(ret);
}

/*
 *  ======== LED_setOn ========
 *  Turns On a specified LED sensor.
 */
bool LED_setOn(LED_Handle ledHandle, uint8_t brightness)
{
    bool ret = true;
    LED_Object *obj = (LED_Object *)(ledHandle->object);

    if(obj->ledType == LED_GPIO_CONTROLLED)
    {
        GPIO_write(obj->gpioIndex, LED_ON);
    }

    /* For PWM LED, turn it On with requested level*/
    else
    {
        ret = LED_setBrightnessLevel(ledHandle, brightness);
    }

    /* Set LED state(conditional) and rawState(always)*/
    if(ret == true)
    {
        if(obj->state != LED_STATE_BLINKING)
        {
            obj->state = LED_STATE_ON;
        }

        obj->rawState  = LED_STATE_ON;
    }

    return(ret);
}

/*
 *  ======== LED_startBlinking ========
 *  Starts blinking an LED with specified period and specified number of times
 */
void LED_startBlinking(LED_Handle ledHandle,
                       uint16_t blinkPeriod,
                       uint16_t blinkCount)
{
    LED_Object *obj = (LED_Object *)(ledHandle->object);

    if(blinkPeriod == 0 || blinkCount == 0)
    {
        LED_stopBlinking(ledHandle);
    }
    else
    {
        /*
         * Start the periodic clock with period = blinkperiod in ms units. If
         * not blinking forever, we set the number of timeouts to be twice the
         * number of requested blinks since one blink is two toggles.
         */
        LED_setOff(ledHandle);
        ClockP_stop(obj->clockHandle);

        if (blinkCount == LED_BLINK_FOREVER)
        {
            /*
             * clockTimeoutHandler knows not to decrement blinkCount when it
             * is set to LED_BLINK_FOREVER
             */
            obj->blinkCount = LED_BLINK_FOREVER;
        }
        else
        {
            /* Don't overflow or set blinkCount to LED_BLINK_FOREVER */
            if (blinkCount > 0x7FFF)
            {
                blinkCount = 0x7FFF;
            }
            obj->blinkCount = 2 * blinkCount;
        }

        obj->togglePeriod = msToTicks(blinkPeriod/2);
        ClockP_setTimeout(obj->clockHandle, obj->togglePeriod);
        ClockP_start(obj->clockHandle);
        obj->state = LED_STATE_BLINKING;
    }
}

/*
 *  ======== LED_stopBlinking ========
 *  Stops blinking a led.
 */
void LED_stopBlinking(LED_Handle ledHandle)
{
    LED_Object *obj = (LED_Object *)(ledHandle->object);

    if(obj->state == LED_STATE_BLINKING)
    {
        /* Stop the clock*/
        ClockP_stop(obj->clockHandle);

        /* After stopping blinking, sets LED Off, a default LED state*/
        LED_setOff(ledHandle);
        obj->state = LED_STATE_OFF;
    }
    else
    {
        /* If LED is not blinking, no need to stop it, so ignore the request*/
    }
}

/*
 *  ======== LED_toggle ========
 *  Toggle a led.
 */
void LED_toggle(LED_Handle ledHandle)
{
    LED_Object *obj = (LED_Object *)(ledHandle->object);

    if(obj->rawState == LED_STATE_ON)
    {
        LED_setOff(ledHandle);
    }
    else if(obj->rawState == LED_STATE_OFF)
    {
        LED_setOn(ledHandle, obj->brightness);
    }
}

/*
 *  ======== LED_write ========
 *  Set the state of an LED instance
 */
void LED_write(LED_Handle ledHandle, bool value)
{
    LED_Object *obj = (LED_Object *)(ledHandle->object);
    if(value)
    {
        LED_setOn(ledHandle, obj->brightness);
    }
    else
    {
        LED_setOff(ledHandle);
    }
}
