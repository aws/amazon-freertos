/*
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
 */

#ifndef _AWS_IOT_DEFENDER_CONFIG_DEFAULT_H_
#define _AWS_IOT_DEFENDER_CONFIG_DEFAULT_H_

#include "aws_clientcredential.h"
#include "aws_clientcredential_keys.h"

/* constants */
#define AWS_IOT_DEFENDER_METRICS_FORMAT_CBOR    1

#define AWS_IOT_DEFENDER_METRICS_FORMAT_JSON    2

/* default configuration */
#ifndef AWS_IOT_DEFENDER_MQTT_ENDPOINT
    #define AWS_IOT_DEFENDER_MQTT_ENDPOINT    clientcredentialMQTT_BROKER_ENDPOINT
#endif

#ifndef AWS_IOT_DEFENDER_THING_NAME
    #define AWS_IOT_DEFENDER_THING_NAME    clientcredentialIOT_THING_NAME
#endif

#ifndef AWS_IOT_DEFENDER_CLIENT_CERTIFICATE_PEM
    #define AWS_IOT_DEFENDER_CLIENT_CERTIFICATE_PEM    keyCLIENT_CERTIFICATE_PEM
#endif

#ifndef AWS_IOT_DEFENDER_MQTT_BROKER_PORT
    #define AWS_IOT_DEFENDER_MQTT_BROKER_PORT    clientcredentialMQTT_BROKER_PORT
#endif

#ifndef AWS_IOT_DEFENDER_DEFAULT_PERIOD_SECONDS
    #define AWS_IOT_DEFENDER_DEFAULT_PERIOD_SECONDS    ( 300 )
#endif

#ifndef AWS_IOT_DEFENDER_DEFAULT_MQTT_TIMEOUT_SECONDS
    #define AWS_IOT_DEFENDER_DEFAULT_MQTT_TIMEOUT_SECONDS    ( 10U )
#endif

#ifndef AWS_IOT_DEFENDER_METRICS_FORMAT_TYPE
    #define AWS_IOT_DEFENDER_METRICS_FORMAT_TYPE    AWS_IOT_DEFENDER_METRICS_FORMAT_CBOR
#endif

#ifndef AWS_IOT_DEFENDER_USE_LONG_TAG
    #define AWS_IOT_DEFENDER_USE_LONG_TAG    ( 0 )
#endif

#endif /* ifndef _AWS_IOT_DEFENDER_CONFIG_DEFAULT_H_ */
