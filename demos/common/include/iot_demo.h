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

/**
 * @file iot_demo.h
 * @brief Declares the platform-independent demo functions.
 */

#ifndef _IOT_DEMO_H_
#define _IOT_DEMO_H_

/* Build using a config header, if provided. */
#ifdef IOT_CONFIG_FILE
    #include IOT_CONFIG_FILE
#endif

/* Standard includes. */
#include <stdbool.h>
#include <stdint.h>

/* MQTT include. */
#include "iot_mqtt.h"

/* Configure logs for the demos. */
#ifdef IOT_LOG_LEVEL_DEMO
    #define _LIBRARY_LOG_LEVEL        IOT_LOG_LEVEL_DEMO
#else
    #ifdef IOT_LOG_LEVEL_GLOBAL
        #define _LIBRARY_LOG_LEVEL    IOT_LOG_LEVEL_GLOBAL
    #else
        #define _LIBRARY_LOG_LEVEL    IOT_LOG_NONE
    #endif
#endif

#define _LIBRARY_LOG_NAME    ( "DEMO" )
#include "iot_logging_setup.h"

/*----------------------------- Demo functions ------------------------------*/

/* See iot_demo_mqtt.c for documentation of this function. */
int IotDemo_RunMqttDemo( bool awsIotMqttMode,
                         const char * const pClientIdentifier,
                         IotMqttConnection_t * const pMqttConnection,
                         const IotMqttNetIf_t * const pNetworkInterface );

/* See aws_iot_demo_shadow.c for documentation of this function. */
int AwsIotDemo_RunShadowDemo( const char * const pThingName,
                              IotMqttConnection_t * const pMqttConnection,
                              const IotMqttNetIf_t * const pNetworkInterface );

#endif /* ifndef _IOT_DEMO_H_ */
