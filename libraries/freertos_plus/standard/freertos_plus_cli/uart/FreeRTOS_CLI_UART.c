/*
 * FreeRTOS+CLI V1.0.4
 * Copyright (C) 2017 Amazon.com, Inc. or its affiliates.  All Rights Reserved.
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
 * http://www.FreeRTOS.org
 * http://aws.amazon.com/freertos
 *
 */

/* FreeRTOS includes. */
#include "FreeRTOS.h"
#include "task.h"

#include "FreeRTOS_CLI_UART.h"
#include "iot_uart.h"

extern IotUARTHandle_t xConsoleUart;

/**
 * Number of characters to read at once from the UART receiver. This is
 * set to one since user can type in a variable length commands. The read API either
 * reads one character at a time or blocks for a time until a character is typed. The typed
 * character is displayed to console immediately, so it increases the responsiveness of the CLI.
 */

#define NUM_CHARS_TO_READ_AT_ONCE    ( 1 )

int32_t uart_read( char * const pcInputBuffer,
                   uint32_t xInputBufferLen );
void uart_write( const char * const pcOutputBuffer,
                 uint32_t xOutputBufferLen );

xConsoleIO_t uartConsoleIO =
{
    .read  = uart_read,
    .write = uart_write
};

int32_t uart_read( char * const pcInputBuffer,
                   uint32_t xInputBufferLen )
{
    int32_t status;

    /*
     * Read one character at a time from UART waiting if necessary for any characters
     * to be entered.
     */
    status = iot_uart_read_sync( xConsoleUart, ( uint8_t * ) pcInputBuffer, NUM_CHARS_TO_READ_AT_ONCE );

    if( status != IOT_UART_SUCCESS )
    {
        /* Returns a negative error code as per the CLI interface contract. */
        status = ( 0L - status );
    }
    else
    {
        status = NUM_CHARS_TO_READ_AT_ONCE;
    }

    return status;
}

void uart_write( const char * const pcOutputBuffer,
                 uint32_t xOutputBufferLen )
{
    int32_t status;

    if( xOutputBufferLen > 0 )
    {
        status = iot_uart_write_sync( xConsoleUart, ( uint8_t * ) pcOutputBuffer, xOutputBufferLen );

        if( status != IOT_UART_SUCCESS )
        {
            configPRINTF( ( "Failed to write to uart %d\n", ( int ) status ) );
        }
    }
}
