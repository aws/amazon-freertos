/*
 * Amazon FreeRTOS Demo Bootloader V1.4.4
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
 * @file aws_boot_log_uart.c
 * @brief Basic tiny uart write.
 */

/* Bootloader includes.*/
#include "aws_boot_log.h"

/* Microchip includes.*/
#include <system_definitions.h>

/*-----------------------------------------------------------*/

size_t BOOT_LOG_UART_Write( const char * pcBuf,
                            size_t xNumBytes )
{
    char cCharTx;
    const char * pcStr = pcBuf;

    while( cCharTx = *pcStr++ )
    {
        while( PLIB_USART_TransmitterBufferIsFull( DRV_USART_PERIPHERAL_ID_IDX0 ) )
        {
            /*Wait for transmit.*/
        }

        PLIB_USART_TransmitterByteSend( DRV_USART_PERIPHERAL_ID_IDX0, cCharTx );
    }

    /* Return only after transmitting bytes. */
    return xNumBytes;
}
