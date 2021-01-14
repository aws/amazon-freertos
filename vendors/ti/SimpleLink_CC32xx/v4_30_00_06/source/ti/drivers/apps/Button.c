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
 *  ======== Button.c ========
 */

#include <stdint.h>
#include <stdbool.h>

#include <ti/drivers/GPIO.h>
#include <ti/drivers/dpl/ClockP.h>
#include <ti/drivers/apps/Button.h>

/* Default Button_Params parameters structure */
const Button_Params Button_defaultParams =
{
    5,               /* 5 ms is the debounce timer value */
    2000,            /* 2 seconds long press duration */
    200,             /* 200 ms double key press detection timeout */
    0xFF             /* button subscribed for all callbacks */
};

extern Button_Config Button_config[];

/* Local functions */

/*
 *  ======== ticksToMs ========
 * Convert system ticks to milliseconds(ms). If the value cannot be represented
 * with 32 bits, ~0 is returned.
 */
static uint32_t ticksToMs(uint32_t ticks)
{
    uint64_t ms = (ticks * ClockP_getSystemTickPeriod()) / 1000;

    if((uint64_t)ms > (uint32_t)ms)
    {
        return((uint32_t)~0);
    }
    else
    {
        return((uint32_t)ms);
    }
}

/*
 *  ======== timediff ========
 *  Calculates time difference between input system tick and current system
 *  tick value. If more than 32 bits of ticks have passed, this function is
 *  unreliable.
 */
static uint32_t timediff(uint32_t startTick)
{
    uint32_t currentTick = ClockP_getSystemTicks();

    if(currentTick > startTick)
    {
        return(currentTick - startTick);
    }
    else {
        /* System tick value overflowed */
        return(currentTick + ((uint32_t)(~0) - startTick));
    }
}

/*
 *  ======== msToTicks ========
 * Convert milliseconds to system ticks
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

/*
 *  ======== Button_close ========
 *  Closes an instance of a Button
 */
bool Button_close(Button_Handle handle)
{
    Button_Object *obj = (Button_Object *)(handle->object);
    Button_HWAttrs *hw = (Button_HWAttrs *)handle->hwAttrs;
    GPIO_disableInt(hw->gpioIndex);
    ClockP_stop(obj->clockHandle);
    ClockP_delete(obj->clockHandle);
    obj->clockHandle = NULL;
    return(true);
}

/*
 *  ======== Button_gpioCallbackFxn ========
 * Callback function for the button interrupts
 */
void Button_gpioCallbackFxn(uint_least8_t index)
{
    /* Handle GPIO interrupt */
    uint_least8_t i;

    for(i = 0; i < Button_count; i++)
    {
        Button_Object *obj = (Button_Object*) Button_config[i].object;
        Button_HWAttrs *hw = (Button_HWAttrs*) Button_config[i].hwAttrs;
        if(hw->gpioIndex == index)
        {
            ClockP_setTimeout(obj->clockHandle, obj->debounceDuration);
            ClockP_start(obj->clockHandle);

            switch(obj->buttonStateVariables.state)
            {
                case Button_PRESSING:
                    obj->buttonStateVariables.state = Button_RELEASING;
                    break;
                case Button_PRESSED:
                case Button_LONGPRESSING:
                    obj->buttonStateVariables.state = Button_RELEASING;
                    break;
                case Button_LONGPRESSED:
                    obj->buttonStateVariables.state = Button_RELEASING_LONG;
                    break;
                case Button_RELEASED:
                    obj->buttonStateVariables.state = Button_PRESSING;
                    break;
                case Button_DBLPRESS_DETECTION:
                    obj->buttonStateVariables.state = Button_DBLPRESSING;
                    break;
                case Button_DBLPRESSED:
                    obj->buttonStateVariables.state = Button_RELEASING_DBLPRESSED;
                    break;
                /*
                 * Any other case, mark the button as pressed.
                 * Typically we should not come here.
                 */
                default:
                    obj->buttonStateVariables.state = Button_PRESSING;
                    break;
            }
            /* Disable the interrupt until the debounce timer expires */
            GPIO_disableInt(index);
            break;
        }
    }
}

/*
 *  ======== Button_init ========
 */
void Button_init()
{
    /* Initialize GPIO driver if it wasn't already */
    GPIO_init();
}

/*
 *  ======== Button_clockTimeoutHandler ========
 * Timeout handler for the clock timeouts
 */
static void Button_clockTimeoutHandler(uintptr_t arg)
{
    Button_Object *obj;
    Button_HWAttrs *hw;
    Button_Handle buttonHandle;
    GPIO_PinConfig pinConfig;
    Button_EventMask buttonEvents = 0;

    buttonHandle = (Button_Handle)arg;
    obj = (Button_Object *)buttonHandle->object;
    hw  = (Button_HWAttrs*)buttonHandle->hwAttrs;
    GPIO_getConfig(hw->gpioIndex,&pinConfig);

    if(GPIO_read(hw->gpioIndex) == obj->buttonPull)
    {
        /*
         * Getting a debounce duration timeout callback. The button is
         * currently in unpressed (pull) state.
         */
        switch(obj->buttonStateVariables.state)
        {
            case Button_RELEASING:
                if(obj->buttonEventMask & Button_EV_DOUBLECLICKED)
                {
                    /* Set clock to detect a double press */
                    obj->buttonStateVariables.state = Button_DBLPRESS_DETECTION;
                    ClockP_setTimeout(obj->clockHandle,
                                      obj->doublePressDetectiontimeout -
                                      obj->debounceDuration);
                    ClockP_start(obj->clockHandle);
                }
                else
                {
                    obj->buttonStateVariables.lastPressedDuration =
                        timediff(obj->buttonStateVariables.pressedStartTime);
                    obj->buttonStateVariables.state = Button_RELEASED;
                    if(obj->buttonEventMask & Button_EV_RELEASED)
                    {
                        buttonEvents |= Button_EV_RELEASED;
                    }
                    if(obj->buttonEventMask & Button_EV_CLICKED)
                    {
                        buttonEvents |= Button_EV_CLICKED;
                    }
                }
                break;

            case Button_DBLPRESS_DETECTION:
                obj->buttonStateVariables.lastPressedDuration =
                    timediff(obj->buttonStateVariables.pressedStartTime);
                if(obj->buttonEventMask & Button_EV_RELEASED)
                {
                    buttonEvents |= Button_EV_RELEASED;
                }
                if(obj->buttonEventMask & Button_EV_CLICKED)
                {
                    buttonEvents |= Button_EV_CLICKED;
                }
                obj->buttonStateVariables.state = Button_RELEASED;
                break;

            case Button_RELEASING_LONG:
                obj->buttonStateVariables.lastPressedDuration =
                     timediff(obj->buttonStateVariables.pressedStartTime);
                if(obj->buttonEventMask & Button_EV_LONGCLICKED)
                {
                    buttonEvents |= Button_EV_LONGCLICKED;
                    buttonEvents |= Button_EV_RELEASED;
                }
                obj->buttonStateVariables.state = Button_RELEASED;
                break;

            case Button_RELEASING_DBLPRESSED:
                obj->buttonStateVariables.state = Button_RELEASED;
                if(obj->buttonEventMask & Button_EV_RELEASED)
                {
                    buttonEvents |= Button_EV_RELEASED;
                }
                break;

            case Button_PRESSING:
            case Button_DBLPRESSING:
            case Button_LONGPRESSING:
                /*
                 * Button was pressed and released within debounce time
                 * Does not count.
                 */
                obj->buttonStateVariables.state = Button_RELEASED;
                break;

            /*
             * Any other case, mark the button as pressed.
             * Typically we should not come here.
             */
            default:
                obj->buttonStateVariables.state = Button_RELEASED;
                break;
        }
        if(obj->buttonPull == Button_PULL_DOWN)
        {
            GPIO_setConfig(hw->gpioIndex,
                          ((pinConfig & (~GPIO_CFG_INT_MASK))|
                            GPIO_CFG_IN_INT_RISING));
        }
        else if(obj->buttonPull == Button_PULL_UP)
        {
            GPIO_setConfig(hw->gpioIndex,
                          ((pinConfig & (~GPIO_CFG_INT_MASK))|
                          GPIO_CFG_IN_INT_FALLING));
        }
    }
    /*
     * Getting a debounce duration timeout callback. The button is currently
     * pressed.
     */
    else
    {
        switch(obj->buttonStateVariables.state)
        {
            case Button_PRESSING:
                /* This is a debounced press */
                obj->buttonStateVariables.pressedStartTime = ClockP_getSystemTicks();
                if(obj->buttonEventMask & Button_EV_PRESSED)
                {
                    buttonEvents |= Button_EV_PRESSED;
                }
                /* Start countdown if interest in long-press */
                if(obj->buttonEventMask &
                   (Button_EV_LONGPRESSED | Button_EV_LONGCLICKED))
                {
                    obj->buttonStateVariables.state = Button_LONGPRESSING;
                    ClockP_setTimeout(obj->clockHandle, obj->longPressDuration - obj->debounceDuration);
                    ClockP_start(obj->clockHandle);
                }
                else
                {
                    obj->buttonStateVariables.state = Button_PRESSED;
                }
                break;

            case Button_DBLPRESSING:
                /* This is a debounced press (this is considered as double click) */
                if(obj->buttonEventMask & Button_EV_DOUBLECLICKED)
                {
                    buttonEvents |= Button_EV_DOUBLECLICKED;
                }
                obj->buttonStateVariables.state = Button_DBLPRESSED;
                break;

            case Button_LONGPRESSING:
                obj->buttonStateVariables.state = Button_LONGPRESSED;
                if(obj->buttonEventMask & Button_EV_LONGPRESSED)
                {
                    buttonEvents |= Button_EV_LONGPRESSED;
                }
                break;

            case Button_RELEASING:
            case Button_RELEASING_LONG:
            case Button_RELEASING_DBLPRESSED:
            /*
            * We're releasing, but isn't released after debounce.
            * Start count down again if interest in long-press
            */
            if(obj->buttonEventMask &
               (Button_EV_LONGPRESSED | Button_EV_LONGCLICKED))
            {
                obj->buttonStateVariables.state = Button_LONGPRESSING;
                ClockP_setTimeout(obj->clockHandle, obj->longPressDuration - obj->debounceDuration);
                ClockP_start(obj->clockHandle);
                obj->buttonStateVariables.state = Button_LONGPRESSING;
            }
            else
            {
                obj->buttonStateVariables.state = Button_PRESSED;
            }

            /*
             * Any other case, mark the button as pressed.
             * Typically we should not come here
             */
            default:
                 obj->buttonStateVariables.state = Button_PRESSED;
                 break;
        }
        if(obj->buttonPull == Button_PULL_DOWN)
        {
            GPIO_setConfig(hw->gpioIndex,
                          ((pinConfig & (~GPIO_CFG_INT_MASK))
                          |GPIO_CFG_IN_INT_FALLING));
        }
        else if(obj->buttonPull == Button_PULL_UP)
        {
            GPIO_setConfig(hw->gpioIndex,
                          ((pinConfig & (~GPIO_CFG_INT_MASK))|
                          GPIO_CFG_IN_INT_RISING));
        }
    }
    if((buttonEvents != 0) && (obj->buttonCallback != NULL))
    {
        obj->buttonCallback(buttonHandle,buttonEvents);
    }
    GPIO_enableInt(hw->gpioIndex);
}

/*
 *  ======== Button_open ========
 *  Open a Button instance
 */
Button_Handle Button_open(uint_least8_t buttonIndex,
                          Button_Callback buttonCallback,
                          Button_Params *params)
{
    Button_Params localParams;
    Button_Handle handle;
    Button_Object *obj;
    Button_HWAttrs *hw;
    GPIO_PinConfig pinConfig;
    ClockP_Params clockParams;

    /*
     * This sets the init state of the button
     * buttonIndex cannot be greater than total ButtonCount
     */
    if(buttonIndex >= Button_count)
    {
        return(NULL);
    }

    /* If params is null then use the default params */
    if(params == NULL)
    {
        /*
         * Make a local copy of default params to pass, to avoid casting away
         * const on Button_defaultParams
         */
        localParams = Button_defaultParams;
        params = &localParams;
    }

    /* Call init in case user forgot */
    Button_init();

    /* Get instance state structure */
    handle = (Button_Handle)&Button_config[buttonIndex];
    obj = (Button_Object*)(Button_config[buttonIndex].object);
    hw  = (Button_HWAttrs*)(Button_config[buttonIndex].hwAttrs);

    /* If instance already has a clock then it is already open */
    if(obj->clockHandle != NULL)
    {
        return(NULL);
    }

    /* Set internal variables */
    obj->debounceDuration            = msToTicks(params->debounceDuration);
    obj->longPressDuration           = msToTicks(params->longPressDuration);
    obj->doublePressDetectiontimeout = msToTicks(params->
                                                 doublePressDetectiontimeout);
    obj->buttonCallback              = buttonCallback;
    obj->buttonEventMask             = params->buttonEventMask;

    /* Get button configuration from GPIO pin config */
    obj->buttonPull = Button_PULL_NOTSET;
    GPIO_getConfig(hw->gpioIndex, &pinConfig);
    if(pinConfig & GPIO_CFG_IN_NOPULL)
    {
        /*
         * Must infer pull from trigger edge. There is likely an external pull
         * up/down attached to the button
         */
        if(pinConfig & GPIO_CFG_IN_INT_FALLING)
        {
            obj->buttonPull = Button_PULL_UP;
        }
        else if(pinConfig & GPIO_CFG_IN_INT_RISING)
        {
            obj->buttonPull = Button_PULL_DOWN;
        }
    }
    else
    {
        /* Using an internal pull up/down */
        if(pinConfig & GPIO_CFG_IN_PU)
        {
            obj->buttonPull = Button_PULL_UP;
        }
        else if(pinConfig & GPIO_CFG_IN_PD)
        {
            obj->buttonPull = Button_PULL_DOWN;
        }
    }

    /* If read incorrect gpio settings, fail to open */
    if(obj->buttonPull == Button_PULL_NOTSET)
    {
        return(NULL);
    }

    /* Create one shot clock for handling debounce */
    ClockP_Params_init(&clockParams);
    clockParams.period = 0; /* Indicates a one shot clock */
    clockParams.startFlag = false;
    clockParams.arg = (uintptr_t) handle;
    obj->clockHandle = ClockP_create(Button_clockTimeoutHandler, 0, &clockParams);
    if(NULL == obj->clockHandle)
    {
        return(NULL);
    }

    /* Enable gpio interrupt */
    GPIO_setCallback(hw->gpioIndex, &Button_gpioCallbackFxn);
    GPIO_enableInt(hw->gpioIndex);
    return(handle);
}

/*
 *  ======== Button_Params_init ========
 * Initialize a Button_Params struct to default settings.
 */
void Button_Params_init(Button_Params *params)
{
    *params = Button_defaultParams;
}

/*
 *  ======== Button_setCallback ========
 * Set the callback for the buttons.
 */
void Button_setCallback(Button_Handle handle,Button_Callback buttonCallback)
{
    Button_Object *obj = (Button_Object *)handle->object;

    obj->buttonCallback = buttonCallback;
}

/*
 *  ======== Button_getLastPressedDuration ========
 * Return the get last pressed duration
 */
extern uint32_t Button_getLastPressedDuration(Button_Handle handle)
{
    uint32_t ticks = ((Button_Object *)(handle->object))->
        buttonStateVariables.lastPressedDuration;
    return(ticksToMs(ticks));
}
