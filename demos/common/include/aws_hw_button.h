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
 * @file aws_hw_button.h
 * @brief AWS abstration of a physical push button on a particular version of HW
 *
 * Modify this file to include files to HW that is specific to a demo, then create the files 
 * specific for a that version of hw. 
 *
 * This example is for ESP32 Wrover Kit
 *     ~/demos/espressif/esp32_devkitc_esp_wrover_kit/common/application_code/esp_wrover_sw3_button.c
 */
#ifndef _HW_BUTTON_H_
#define _HW_BUTTON_H_

/**
 * @brief Init HW for push button detection.
 * Only call this function once.
 */
extern void buttonInit( void );

/**
 * @brief Determine if button was pushed.
 */
extern bool buttonWasPushed( void );

#define HW_BUTTON      

#endif /* _HW_BUTTON_H_ */


