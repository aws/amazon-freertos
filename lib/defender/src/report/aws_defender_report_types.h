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
#ifndef AWS_REPORT_TYPES_H
#define AWS_REPORT_TYPES_H

#include "aws_cbor.h"

typedef void (* update_metric_t)( void );
typedef cbor_handle_t (* report_metric_t)( void );

struct defender_metric_s
{
    update_metric_t UpdateMetric;
    report_metric_t ReportMetric;
};

/**
 * Enumeration of agent states
 */
typedef enum
{
    eDEFENDER_STATE_INIT,
    eDEFENDER_STATE_STARTED,
    eDEFENDER_STATE_NEW_MQTT_FAILED,
    eDEFENDER_STATE_NEW_MQTT_SUCCESS,
    eDEFENDER_STATE_CONNECT_MQTT_FAILED,
    eDEFENDER_STATE_CONNECT_MQTT_SUCCESS,
    eDEFENDER_STATE_SUBSCRIBE_MQTT_FAILED,
    eDEFENDER_STATE_SUBSCRIBE_MQTT_SUCCESS,
    eDEFENDER_STATE_SUBMIT_REPORT_FAILED,
    eDEFENDER_STATE_SUBMIT_REPORT_SUCCESS,
    eDEFENDER_STATE_DISCONNECT_FAILED,
    eDEFENDER_STATE_DISCONNECTED,
    eDEFENDER_STATE_DELETE_FAILED,
    eDEFENDER_STATE_SLEEP,

    eDEFENDER_STATE_COUNT,
} defender_state_t;

#endif /* end of include guard: AWS_REPORT_TYPES_H */
