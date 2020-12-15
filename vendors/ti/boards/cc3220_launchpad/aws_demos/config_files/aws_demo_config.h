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
 *          CONFIG_CORE_MQTT_CONNECTION_SHARING_DEMO_ENABLED
 *          CONFIG_DEVICE_SHADOW_DEMO_ENABLED
 *          CONFIG_DEVICE_DEFENDER_DEMO_ENABLED
 *          CONFIG_JOBS_DEMO_ENABLED
 *          CONFIG_GREENGRASS_DISCOVERY_DEMO_ENABLED
 *          CONFIG_TCP_ECHO_CLIENT_DEMO_ENABLED
 *          CONFIG_POSIX_DEMO_ENABLED
 *          CONFIG_OTA_UPDATE_DEMO_ENABLED
 *
 *  These defines are used in iot_demo_runner.h for demo selection */

#define CONFIG_CORE_MQTT_MUTUAL_AUTH_DEMO_ENABLED

/* Default configuration for all demos. Individual demos can override these below */
#define democonfigDEMO_STACKSIZE    ( configMINIMAL_STACK_SIZE * 8 )
#define democonfigDEMO_PRIORITY     ( tskIDLE_PRIORITY + 5 )
#define democonfigNETWORK_TYPES     ( AWSIOT_NETWORK_TYPE_WIFI )

#if defined( democonfigBLE_MQTT_ECHO_DEMO_ENABLED )
    #undef democonfigNETWORK_TYPES
    #define democonfigNETWORK_TYPES                                  ( AWSIOT_NETWORK_TYPE_WIFI | AWSIOT_NETWORK_TYPE_BLE )
#endif

#define democonfigSHADOW_DEMO_NUM_TASKS                              ( 1 )
#define democonfigSHADOW_DEMO_TASK_STACK_SIZE                        ( 1024 )
#define democonfigSHADOW_DEMO_TASK_PRIORITY                          ( tskIDLE_PRIORITY + 5 )
#define shadowDemoUPDATE_TASK_STACK_SIZE                             ( configMINIMAL_STACK_SIZE * 5 )

#define democonfigMQTT_ECHO_TLS_NEGOTIATION_TIMEOUT                  pdMS_TO_TICKS( 12000 )
#define democonfigMQTT_ECHO_TASK_PRIORITY                            ( tskIDLE_PRIORITY )

/* MQTT Connection sharing demo task priority. */
#define democonfigCORE_MQTT_CONNECTION_SHARING_DEMO_TASK_PRIORITY    ( tskIDLE_PRIORITY + 1 )

/* Timeout used when performing MQTT operations that do not need extra time
 * to perform a TLS negotiation. */
#define democonfigMQTT_TIMEOUT                                       pdMS_TO_TICKS( 3000 )

/* Send AWS IoT MQTT traffic encrypted. */
#define democonfigMQTT_AGENT_CONNECT_FLAGS                           ( mqttagentREQUIRE_TLS )

#define democonfigGREENGRASS_DISCOVERY_TASK_STACK_SIZE               ( configMINIMAL_STACK_SIZE * 14 )


#define democonfigMEMORY_ANALYSIS

#ifdef democonfigMEMORY_ANALYSIS
    #define democonfigMEMORY_ANALYSIS_STACK_DEPTH_TYPE    UBaseType_t
    #define democonfigMEMORY_ANALYSIS_MIN_EVER_HEAP_SIZE()        xPortGetMinimumEverFreeHeapSize()
    #if ( INCLUDE_uxTaskGetStackHighWaterMark == 1 )
        /* Convert from stack words to bytes */
        #define democonfigMEMORY_ANALYSIS_STACK_WATERMARK( x )    uxTaskGetStackHighWaterMark( x ) * ( uint32_t ) sizeof( StackType_t ); /*lint !e961 Casting is not redundant on smaller architectures. */
    #else
        #define democonfigMEMORY_ANALYSIS_STACK_WATERMARK( x )    NULL
    #endif /* if( INCLUDE_uxTaskGetStackHighWaterMark == 1 ) */
#endif /* democonfigMEMORY_ANALYSIS */


#endif /* _AWS_DEMO_CONFIG_H_ */
