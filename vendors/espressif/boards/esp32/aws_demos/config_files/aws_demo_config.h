/*
 * FreeRTOS V1.4.7
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

#ifndef _AWS_DEMO_CONFIG_H_
#define _AWS_DEMO_CONFIG_H_

/* To run a particular demo you need to define one of these.
 * Only one demo can be configured at a time
 *
 *          CONFIG_CORE_HTTP_MUTUAL_AUTH_DEMO_ENABLED
 *          CONFIG_CORE_HTTP_S3_DOWNLOAD_DEMO_ENABLED
 *          CONFIG_CORE_HTTP_S3_DOWNLOAD_MULTITHREADED_DEMO_ENABLED
 *          CONFIG_CORE_HTTP_S3_UPLOAD_DEMO_ENABLED
 *          CONFIG_CORE_MQTT_MUTUAL_AUTH_DEMO_ENABLED
 *          CONFIG_CORE_MQTT_AGENT_DEMO_ENABLED
 *          CONFIG_DEVICE_SHADOW_DEMO_ENABLED
 *          CONFIG_DEVICE_DEFENDER_DEMO_ENABLED
 *          CONFIG_OTA_MQTT_UPDATE_DEMO_ENABLED
 *          CONFIG_OTA_HTTP_UPDATE_DEMO_ENABLED
 *          CONFIG_JOBS_DEMO_ENABLED
 *          CONFIG_GREENGRASS_DISCOVERY_DEMO_ENABLED
 *          CONFIG_TCP_ECHO_CLIENT_DEMO_ENABLED
 *          CONFIG_BLE_GATT_SERVER_DEMO_ENABLED
 *          CONFIG_MQTT_BLE_TRANSPORT_DEMO_ENABLED
 *          CONFIG_SHADOW_BLE_TRANSPORT_DEMO_ENABLED
 *          CONFIG_OTA_MQTT_BLE_TRANSPORT_DEMO_ENABLED
 *          CONFIG_CLI_UART_DEMO_ENABLED
 *
 *  These defines are used in iot_demo_runner.h for demo selection */

#define CONFIG_CORE_MQTT_MUTUAL_AUTH_DEMO_ENABLED

/* Default configuration for all demos. Individual demos can override these below */
#define democonfigDEMO_STACKSIZE    ( configMINIMAL_STACK_SIZE * 8 )
#define democonfigDEMO_PRIORITY     ( tskIDLE_PRIORITY + 5 )
#define democonfigNETWORK_TYPES     ( AWSIOT_NETWORK_TYPE_WIFI )

#if defined( CONFIG_CORE_MQTT_MUTUAL_AUTH_DEMO_ENABLED )
    #undef democonfigNETWORK_TYPES
    #define democonfigNETWORK_TYPES    ( AWSIOT_NETWORK_TYPE_WIFI )
#endif

#if defined( CONFIG_CORE_MQTT_AGENT_DEMO_ENABLED )
    #undef democonfigNETWORK_TYPES
    #define democonfigNETWORK_TYPES                                  ( AWSIOT_NETWORK_TYPE_WIFI )
#endif

#define democonfigGREENGRASS_DISCOVERY_TASK_STACK_SIZE               ( configMINIMAL_STACK_SIZE * 12 )

/* MQTT Connection sharing demo task priority. */
#define democonfigCORE_MQTT_AGENT_DEMO_TASK_PRIORITY                 ( tskIDLE_PRIORITY + 1 )

#define democonfigMEMORY_ANALYSIS

#ifdef democonfigMEMORY_ANALYSIS
    #define democonfigMEMORY_ANALYSIS_STACK_DEPTH_TYPE    UBaseType_t
    #define democonfigMEMORY_ANALYSIS_MIN_EVER_HEAP_SIZE()        xPortGetMinimumEverFreeHeapSize()
    #if ( INCLUDE_uxTaskGetStackHighWaterMark == 1 )
        /* Shift by left twice to convert from stack words to bytes */
        #define democonfigMEMORY_ANALYSIS_STACK_WATERMARK( x )    uxTaskGetStackHighWaterMark( x )
    #else
        #define democonfigMEMORY_ANALYSIS_STACK_WATERMARK( x )    NULL
    #endif /* if( INCLUDE_uxTaskGetStackHighWaterMark == 1 ) */
#endif /* democonfigMEMORY_ANALYSIS */


#define IOT_WIFI_ENABLE_SOFTAP_PROVISIONING    0


#endif /* _AWS_DEMO_CONFIG_H_ */
