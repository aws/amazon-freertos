/*
 * FreeRTOS V1.4.4
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
 *          CONFIG_MQTT_DEMO_ENABLED
 *          CONFIG_SHADOW_DEMO_ENABLED
 *          CONFIG_OTA_UPDATE_DEMO_ENABLED
 *          CONFIG_POSIX_DEMO_ENABLED
 *          CONFIG_BLE_GATT_SERVER_DEMO_ENABLED
 *
 *  These defines are used in iot_demo_runner.h for demo selection */

#define CONFIG_MQTT_DEMO_ENABLED

/* Default configuration for all demos. Individual demos can override these below */
#define democonfigDEMO_STACKSIZE                             ( configMINIMAL_STACK_SIZE * 8 )
#define democonfigDEMO_PRIORITY                              ( tskIDLE_PRIORITY + 5 )
#define democonfigNETWORK_TYPES                              ( AWSIOT_NETWORK_TYPE_BLE )

#define democonfigSHADOW_DEMO_NUM_TASKS                      ( 1 )

/* IoT simple subscribe/publish example task parameters. */
#define democonfigMQTT_SUB_PUB_TASK_STACK_SIZE               ( configMINIMAL_STACK_SIZE * 8 )
#define democonfigMQTT_SUB_PUB_TASK_PRIORITY                 ( tskIDLE_PRIORITY + 5 )

/* Greengrass discovery example task parameters. */
#define democonfigGREENGRASS_DISCOVERY_TASK_STACK_SIZE       ( configMINIMAL_STACK_SIZE * 8 )
#define democonfigGREENGRASS_DISCOVERY_TASK_PRIORITY         ( tskIDLE_PRIORITY + 5 )

/* Shadow demo task parameters. */
#define democonfigSHADOW_DEMO_TASK_STACK_SIZE                ( configMINIMAL_STACK_SIZE * 4 )
#define democonfigSHADOW_DEMO_TASK_PRIORITY                  ( tskIDLE_PRIORITY )

/* TCP Echo Client tasks single example parameters. */
#define democonfigTCP_ECHO_TASKS_SINGLE_TASK_STACK_SIZE      ( configMINIMAL_STACK_SIZE * 8 )
#define democonfigTCP_ECHO_TASKS_SINGLE_TASK_PRIORITY        ( tskIDLE_PRIORITY + 5 )

/* OTA Update task example parameters. */
#define democonfigOTA_UPDATE_TASK_STACK_SIZE                 ( 4 * configMINIMAL_STACK_SIZE )
#define democonfigOTA_UPDATE_TASK_TASK_PRIORITY              ( tskIDLE_PRIORITY + 5 )

/* Simple TCP Echo Server task example parameters */
#define democonfigTCP_ECHO_SERVER_TASK_STACK_SIZE            ( configMINIMAL_STACK_SIZE * 8 )
#define democonfigTCP_ECHO_SERVER_TASK_PRIORITY              ( tskIDLE_PRIORITY + 5 )

/* TCP Echo Client tasks multi task example parameters. */
#define democonfigTCP_ECHO_TASKS_SEPARATE_TASK_STACK_SIZE    ( configMINIMAL_STACK_SIZE * 8 )
#define democonfigTCP_ECHO_TASKS_SEPARATE_TASK_PRIORITY      ( tskIDLE_PRIORITY + 5 )

/* MQTT echo task example parameters. */
#define democonfigMQTT_ECHO_TASK_STACK_SIZE                  ( configMINIMAL_STACK_SIZE * 8 )
#define democonfigMQTT_ECHO_TASK_PRIORITY                    ( tskIDLE_PRIORITY + 5 )

/* Timeout used when establishing a connection, which required TLS
 * negotiation. */
#define democonfigMQTT_ECHO_TLS_NEGOTIATION_TIMEOUT          pdMS_TO_TICKS( 12000 )

/* Timeout used when performing MQTT operations that do not need extra time
 * to perform a TLS negotiation. */
#define democonfigMQTT_TIMEOUT                               pdMS_TO_TICKS( 3000 )

/* Send AWS IoT MQTT traffic encrypted to destination port 443. */
#define democonfigMQTT_AGENT_CONNECT_FLAGS                   ( mqttagentREQUIRE_TLS | mqttagentUSE_AWS_IOT_ALPN_443 )

#endif /* _AWS_DEMO_CONFIG_H_ */
