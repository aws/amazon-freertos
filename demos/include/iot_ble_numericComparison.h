/*
 * FreeRTOS V202203.00
 * Copyright (C) 2021 Amazon.com, Inc. or its affiliates.  All Rights Reserved.
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
 * @file iot_ble_numericComparison.h
 * @brief Header file for supporting user confirmation for BLE passkey.
 */
#ifndef _IOT_BLE_NUMERIC_COMPARISON_H_
#define _IOT_BLE_NUMERIC_COMPARISON_H_

#include "FreeRTOS.h"
#include "iot_ble.h"


void vDemoBLENumericComparisonCb( BTBdaddr_t * pxRemoteBdAddr,
                                  uint32_t ulPassKey );

void vDemoBLEGAPPairingStateChangedCb( BTStatus_t xStatus,
                                       BTBdaddr_t * pxRemoteBdAddr,
                                       BTBondState_t bondState,
                                       BTSecurityLevel_t xSecurityLevel,
                                       BTAuthFailureReason_t xReason );

/**
 * @brief Initializer for BLE numeric comparison demo.
 */
void vDemoBLENumericComparisonInit( void );

/**
 * @brief Port specific funtion to receive the reply for a numeric comparison
 * request from the user.
 * Vendor needs to implement this for each board using their I/O APIs such
 * as UART.
 * The function should wait for timeout ticks or upto messageLength bytes are received
 * from terminal. If it received only partial bytes then it can send the partial bytes and
 * return the number of bytes read. In case of an error it can return a negative integer
 * representing the error.
 *
 * @param[in] pMessage The buffer used to hold the message received from user.
 * @param[in] messageLength The length of the buffer used to receive the buffer.
 * @param[in] timeoutTicks Ticks to wait for the entire message.
 * @return number of bytes read, or < 0 if there is an error.
 */
int32_t xPortGetUserInput( uint8_t * pMessage,
                           uint32_t messageLength,
                           TickType_t timeoutTicks );


#endif /* _IOT_BLE_NUMERIC_COMPARISON_H_ */
