/*
 * Amazon FreeRTOS Device Defender Agent V1.0.1
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
#include "FreeRTOS.h"
#include "list.h"

#include "aws_defender_tcp_conn.h"

extern List_t xBoundTCPSocketsList;
static int xDEFENDER_tcp_conn_count;

int DEFENDER_TcpConnGet( void )
{
    return xDEFENDER_tcp_conn_count;
}

void DEFENDER_TcpConnRefresh( void )
{
    int xTcp_conn_count[ 2 ];
    int xTries = 10;

    do
    {
        xTcp_conn_count[ 0 ] = xBoundTCPSocketsList.uxNumberOfItems;
        xTcp_conn_count[ 1 ] = xBoundTCPSocketsList.uxNumberOfItems;
        xTries--;

        if( 0 == xTries )
        {
            xDEFENDER_tcp_conn_count = -1;

            return;
        }
    } while( xTcp_conn_count[ 0 ] != xTcp_conn_count[ 1 ] );

    xDEFENDER_tcp_conn_count = xTcp_conn_count[ 0 ];
}
