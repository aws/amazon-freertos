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
 * @file iot_hw.h
 * @brief Define common APIs to get hardware ID and hardware revision.
 */
#ifndef _IOT_HW_H_
#define _IOT_HW_H_

/**
 * @brief Error code returned by platform
 *
 * We assume that neither hardware ID nor hardware revision number will ever be greater than 255
 * it means that API is not implemented or supported if API returns 0xFF
 */
#define IOT_HW_UNSUPPORTED    ( 0xFFFF )

/**
 * @brief API to read hardware ID.
 *
 * @note:  the format and contents of this field are specific
 *         to each vendor.  With the expectation that they will
 *         be unique for each board.
 *
 * @return hardware ID number as unsigned byte
 */
uint16_t iot_hw_get_id( void );

/**
 * @brief API to read hardware revision.
 *
 * @note:  the format and contents of this field are specific
 *         to each vendor.  With the expectation that they will
 *         be unique for each revision.
 *
 * @return hardware revision number as unsigned byte
 */
uint16_t iot_hw_get_rev( void );

#endif /* _IOT_HW_H_ */
