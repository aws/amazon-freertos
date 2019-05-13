/*
 * Amazon FreeRTOS V1.4.7
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

#ifndef _AWS_DEMO_CONFIG_H_
#define _AWS_DEMO_CONFIG_H_

/* Number of sub pub tasks that connect to a broker that is not using TLS. */
#define democonfigMQTT_SUB_PUB_NUM_UNSECURE_TASKS         ( 2 )

/* Number of sub pub tasks that connect to a broker that is using TLS. */
#define democonfigMQTT_SUB_PUB_NUM_SECURE_TASKS           ( 2 )

/* IoT simple subscribe/publish example task parameters. */
#define democonfigMQTT_SUB_PUB_TASK_STACK_SIZE            ( configMINIMAL_STACK_SIZE * 5 )
#define democonfigMQTT_SUB_PUB_TASK_PRIORITY              ( tskIDLE_PRIORITY )

/* Greengrass discovery example task parameters. */
#define democonfigGREENGRASS_DISCOVERY_TASK_STACK_SIZE    ( configMINIMAL_STACK_SIZE * 16 )
#define democonfigGREENGRASS_DISCOVERY_TASK_PRIORITY      ( tskIDLE_PRIORITY )

/* Shadow demo task parameters. */
#define democonfigSHADOW_DEMO_TASK_STACK_SIZE             ( configMINIMAL_STACK_SIZE * 4 )
#define democonfigSHADOW_DEMO_TASK_PRIORITY               ( tskIDLE_PRIORITY + 1 )

/* Number of shadow light switch tasks running. */
#define democonfigSHADOW_DEMO_NUM_TASKS                   ( 2 )

/* TCP Echo Client tasks single example parameters. */
#define democonfigTCP_ECHO_TASKS_SINGLE_TASK_STACK_SIZE    ( configMINIMAL_STACK_SIZE * 4 )
#define democonfigTCP_ECHO_TASKS_SINGLE_TASK_PRIORITY      ( tskIDLE_PRIORITY + 1 )

/* TCP Echo Client tasks separate example parameters */
#define democonfigTCP_ECHO_TASKS_SEPARATE_TASK_STACK_SIZE  ( configMINIMAL_STACK_SIZE * 3 )
#define democonfigTCP_ECHO_TASKS_SEPARATE_TASK_PRIORITY    ( tskIDLE_PRIORITY + 1 )

/* Simple TCP Echo Server task example parameters */
#define democonfigTCP_ECHO_SERVER_TASK_STACK_SIZE          ( configMINIMAL_STACK_SIZE * 6 )
#define democonfigTCP_ECHO_SERVER_TASK_PRIORITY            ( tskIDLE_PRIORITY )

/* MQTT echo task example parameters. */
#define democonfigMQTT_ECHO_TASK_STACK_SIZE                ( configMINIMAL_STACK_SIZE * 3 )
#define democonfigMQTT_ECHO_TASK_PRIORITY                  ( tskIDLE_PRIORITY + 1 )

/* MQTT echo task example parameters. */
#define democonfigLOGGING_TASK_STACK_SIZE                  ( configMINIMAL_STACK_SIZE * 3 )
#define democonfigLOGGING_TASK_PRIORITY                    ( tskIDLE_PRIORITY + 2 )
#define democonfigLOGGING_QUEUE_LENGTH                     ( 512 )

/* Timeout used when establishing a connection, which required TLS
 * negotiation. */
#define democonfigMQTT_ECHO_TLS_NEGOTIATION_TIMEOUT        pdMS_TO_TICKS( 12000 )

/* Timeout used when performing MQTT operations that do not need extra time
 * to perform a TLS negotiation. */
#define democonfigMQTT_TIMEOUT                             pdMS_TO_TICKS( 2500 )

/* Send AWS IoT MQTT traffic encrypted. */
#define democonfigMQTT_AGENT_CONNECT_FLAGS          	   ( mqttagentREQUIRE_TLS )

#endif /* _AWS_DEMO_CONFIG_H_ */
