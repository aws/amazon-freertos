/*
 * Amazon FreeRTOS
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
 * http://aws.amazon.com/freertos
 * http://www.FreeRTOS.org
 */

/**
 * @brief Defined interface for portable socket metrics data.
 *
 * TODO: it is an open question whether such interface should be integrated with Secure Sockets layer.
 *
 */

#ifndef _AWS_IOT_DEFENDER_METRICS_SOCKET_H_
#define _AWS_IOT_DEFENDER_METRICS_SOCKET_H_

#include <stdint.h>

typedef struct _defenderMetricsConnection
{
    uint16_t localPort;
    uint16_t remotePort;
    uint32_t remoteIP;
} _defenderMetricsConnection_t;

typedef struct _defenderMetricsPort
{
    uint16_t port;
} _defenderMetricsPort_t;

/**
 * @brief Return the count of established TCP connections.
 *
 * The returned value should be used to allocate buffer of _defenderMetricsConnection_t and passed into
 * AwsIotDefenderInternal_GetMetricsEstablishedConnections function.
 */
uint8_t AwsIotDefenderInternal_GetEstablishedConnectionsCount();

/**
 * @brief Set an array of established connections information.
 */
uint8_t AwsIotDefenderInternal_GetEstablishedConnections( _defenderMetricsConnection_t * pConnections,
                                                              uint8_t connectionsLength );

/**
 * @brief Return the count of listening TCP ports.
 *
 * The returned value should be used to allocate buffer of _defenderMetricsPort_t and passed into
 * AwsIotDefenderInternal_GetMetricsListeningTcpPorts function.
 */
uint8_t AwsIotDefenderInternal_GetListeningTcpPortsCount();

/**
 * @brief Set an array of listening ports information.
 */
uint8_t AwsIotDefenderInternal_GetListeningTcpPorts( _defenderMetricsPort_t * pPorts,
                                                         uint8_t portsLength );

#endif /* ifndef _AWS_IOT_DEFENDER_METRICS_SOCKET_H_ */
