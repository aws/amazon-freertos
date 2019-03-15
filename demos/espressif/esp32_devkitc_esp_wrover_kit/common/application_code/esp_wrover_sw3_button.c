/*
 * Amazon FreeRTOS Combined Demo V1.0.0
 * Copyright (C) 2019 Amazon.com, Inc. or its affiliates.  All Rights Reserved.
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
 * @file esp_wrover_sw3_button.c
 * @brief Espressif ESP32 Rover KIT hardware specific routines to detect button push
 *
 * These routines should be called from an application task wanting to monitor
 * the SW3 or boot button on the ESP32 Rover Kit HW being pushed. This only 
 * applies to this specific version of hw. In order to duplicate this functionaliy
 * on other HW from Espressif or another vendor one must create similar routines specific
 * to the HW on which Amazon FreeRTOS runs.
 *
 * Expected use/operation of this code:
 *      buttonInit() is called once at the beginning of the task. 
 *      buttonWasPushed() is called repeatedly within a while loop.
 *
 */

/* POSIX and Platform layer includes. */
#include "platform/iot_clock.h"
#include "FreeRTOS_POSIX/time.h"

#include "driver/gpio.h"

/**
 * @brief Time button must be pushed down before release
 */
#define demoBUTTON_DELAY ( 1000 )

/**
 * @brief IO pin for push button.
 */
#define GPIO_INPUT_IO_0                          ( 4 )

void buttonInit( void )
{
    /* Enable GPIO 0 so that we can read the state */
    gpio_config_t io_conf;

    io_conf.intr_type = GPIO_PIN_INTR_DISABLE;
    /*bit mask of the pins, use GPIO0 here */
    io_conf.pin_bit_mask = ( 1 << GPIO_INPUT_IO_0 );
    /*set as input mode */
    io_conf.mode = GPIO_MODE_INPUT;
    /*enable pull-up mode */
    io_conf.pull_up_en = 1;
    gpio_config( &io_conf );
}


/*-----------------------------------------------------------*/

bool buttonWasPushed( void )
{
    int numSecsToWait = 5;
    bool pushed = false;
    bool released = false;
    TickType_t buttonDelay = pdMS_TO_TICKS( demoBUTTON_DELAY );

    while( numSecsToWait-- )
    {
        if( gpio_get_level( GPIO_NUM_0 ) == 0 )
        {
            /*   IotLogInfo("buttonWasPushed: button pushed\n"); */
            pushed = true;
            break;
        }

        vTaskDelay( buttonDelay );
    }

    if( pushed )
    {
        numSecsToWait = 20;

        while( numSecsToWait-- )
        {
            if( gpio_get_level( GPIO_NUM_0 ) == 1 )
            {
                /* IotLogInfo("buttonWasPushed: button released\n"); */
                released = true;
                break;
            }

            vTaskDelay( buttonDelay );
        }
    }

    return released;
}

