/*
 * FreeRTOS V202012.00
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

#ifndef __NETIF_PORT_H__
#define __NETIF_PORT_H__

/* The LWIP network metric bytes-in and bytes-out need the support from LWIP
 * network interface port. The bytes counts are implemented in the board
 * LWIP network interface port. The metric collector reads the bytes-in and
 * bytes-out data from a LWIP netif structure defined in the board LWIP network
 */

/* Boards netif port configurations. */
#ifdef MT7697HX_DEV_KIT
    /* Extern netif variables from board's port. */
    extern struct netif sta_if, ap_if;

/* This define is used by metrics_collector.c to read the number of
 * bytes-in/out. */
    #define LWIP_NET_IF                      sta_if
#else
    #define LWIP_BYTES_IN_OUT_UNSUPPORTED    1
#endif

#endif /* __NETIF_PORT_H__ */
