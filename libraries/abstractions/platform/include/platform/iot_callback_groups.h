/*
 * Amazon FreeRTOS
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

#ifndef IOT_CALLBACK_GROUPS_H
#define IOT_CALLBACK_GROUPS_H

#include "iot_os_common.h"
#include "FreeRTOS.h"
#include "event_groups.h"

typedef StaticEventGroup_t		iot_StaticCallbackGroup_t;
typedef EventGroupHandle_t 		iot_CallbackGroupHandle_t;
typedef EventBits_t				iot_CallbackBits_t;

// ======================================================= //

#define iot_CallbackGroupCreateStatic( pxEventGroupBuffer ) \
			xEventGroupCreateStatic( pxEventGroupBuffer )

#define iot_CallbackGroupWaitBits( xEventGroup, uxBitsToWaitFor, xClearOnExit, xWaitForAllBits, xTicksToWait ) \
			xEventGroupWaitBits( xEventGroup, uxBitsToWaitFor, xClearOnExit, xWaitForAllBits, xTicksToWait )

#define iot_CallbackGroupSetBits( xEventGroup, uxBitsToSet ) \
			xEventGroupSetBits( xEventGroup, uxBitsToSet )

// ======================================================= //

#endif /* IOT_CALLBACK_GROUPS_H */