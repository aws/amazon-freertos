/*
 * Amazon FreeRTOS Wi-Fi for Curiosity PIC32MZEF V1.0.3
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

/* Wi-Fi driver includes. */
#include "wdrv_wilc1000_api.h"

/* Amazon FreeRTOS include. */
#include "FreeRTOSConfig.h"

/**
 * @brief Overriding function for WDRV_STUB_Assert() defined in wdrv_wilc1000_stub.h.
 * We want to call configASSERT() in Amazon FreeRTOS instead so that functions abort
 * tests for assertions instead of falling into a while loop.
 */
void WDRV_STUB_Assert(int condition, const char *msg, const char *file, int line)
{
    if (!condition)
    {
        if ( *msg )
            configPRINTF( ( "Wi-Fi Assert: %s in %s, line %u\r\n", msg, file, line ) );
        else
            configPRINTF( ( "Wi-Fi Assert: %s, line %u\r\n", file, line ) );

        configASSERT( condition );
    }
    return;
}