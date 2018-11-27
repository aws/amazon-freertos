/*
 * Amazon FreeRTOS
 * Copyright (C) 2018 Amazon.com, Inc. or its affiliates.  All Rights Reserved.
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
 * @file aws_ble_event_manager.h
 * @brief Event manager for BLE, handles GATT and GAP events.
 */


#ifndef _AWS_BLE_EVENT_MANAGER_
#define _AWS_BLE_EVENT_MANAGER_

#include "aws_doubly_linked_list.h"
#include "stdbool.h"

BaseType_t vEVTMNGRinitialize( void );
BaseType_t vEVTMNGRgetNewInstance( uint32_t * pulHandle, uint16_t ulNbEvents );
BaseType_t vEVTMNGRsetEventParameters( uint32_t ulHandle, uint32_t ulEvent, void * pvEventParams );
void vEVTMNGRgetEventParameters( uint32_t ulHandle, uint32_t ulEvent, void ** ppvEventParams, bool * pbFoundEvent );
int16_t vEVTMNGRwaitEvent( uint32_t ulHandle, uint16_t event);
void vEVTMNGRsetEvent( uint32_t ulHandle, uint16_t event, int16_t status);
void vEVTMNGRremoveLastEvent( uint32_t ulHandle, uint32_t ulEvent );

#endif
