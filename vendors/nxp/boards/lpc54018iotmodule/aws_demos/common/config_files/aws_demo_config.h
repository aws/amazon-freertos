/*
 * Amazon FreeRTOS V1.4.7
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

#ifndef _AWS_DEMO_CONFIG_H_
#define _AWS_DEMO_CONFIG_H_

#include "iot_demo_runner.h"

/* Default configuration for all demos. Individual demos can override these below */
#define democonfigDEMO_STACKSIZE               ( configMINIMAL_STACK_SIZE * 15 )
#define democonfigDEMO_PRIORITY                ( tskIDLE_PRIORITY + 1 )
#define democonfigNETWORK_TYPES                ( AWSIOT_NETWORK_TYPE_WIFI )

/* Some individual demos want to override these defaults, that is done in this section */
#if defined(democonfigTCP_ECHO_TASKS_SEPARATE_ENABLED)
    #undef democonfigDEMO_STACKSIZE
    #define democonfigDEMO_STACKSIZE                        ( configMINIMAL_STACK_SIZE * 4 )
    #undef democonfigDEMO_PRIORITY
    #define democonfigDEMO_PRIORITY                         ( tskIDLE_PRIORITY + 5 ) 
#endif

#if defined(democonfigTCP_ECHO_TASKS_SINGLE_ENABLED)
    #undef democonfigDEMO_STACKSIZE
    #define democonfigDEMO_STACKSIZE                        ( configMINIMAL_STACK_SIZE * 4 )
    #undef democonfigDEMO_PRIORITY
    #define democonfigDEMO_PRIORITY                         ( tskIDLE_PRIORITY + 1 ) 
#endif

#define democonfigSHADOW_DEMO_NUM_TASKS             ( 2 )
#define democonfigSHADOW_DEMO_TASK_STACK_SIZE       ( configMINIMAL_STACK_SIZE * 4 )
#define democonfigSHADOW_DEMO_TASK_PRIORITY         ( tskIDLE_PRIORITY + 1 )
#define shadowDemoUPDATE_TASK_STACK_SIZE            ( configMINIMAL_STACK_SIZE * 5 )

#define democonfigMQTT_ECHO_TLS_NEGOTIATION_TIMEOUT pdMS_TO_TICKS( 12000 )
#define democonfigMQTT_ECHO_TASK_STACK_SIZE         ( configMINIMAL_STACK_SIZE * 4 )
#define democonfigMQTT_ECHO_TASK_PRIORITY           ( tskIDLE_PRIORITY )

/* Number of sub pub tasks that connect to a broker that is not using TLS. */
#define democonfigMQTT_SUB_PUB_NUM_UNSECURE_TASKS            ( 0 )
/* Number of sub pub tasks that connect to a broker that is using TLS. */
#define democonfigMQTT_SUB_PUB_NUM_SECURE_TASKS              ( 1 )

#define democonfigMQTT_SUB_PUB_TASK_STACK_SIZE      ( configMINIMAL_STACK_SIZE * 5 )
#define democonfigMQTT_SUB_PUB_TASK_PRIORITY        ( tskIDLE_PRIORITY + 1 )


/* Greengrass discovery example task parameters. */
#define democonfigGREENGRASS_DISCOVERY_TASK_STACK_SIZE     ( configMINIMAL_STACK_SIZE * 22 )
#define democonfigGREENGRASS_DISCOVERY_TASK_PRIORITY       ( tskIDLE_PRIORITY + 1 )

/* Timeout used when performing MQTT operations that do not need extra time
to perform a TLS negotiation. */
#define democonfigMQTT_TIMEOUT			           pdMS_TO_TICKS( 2500 )

/* Send AWS IoT MQTT traffic encrypted. */
#define democonfigMQTT_AGENT_CONNECT_FLAGS          	   ( mqttagentREQUIRE_TLS )

#endif /* _AWS_DEMO_CONFIG_H_ */
