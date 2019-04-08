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

#include "iot_demo_runner.h"

/* Choose your demo here. By design only one demo can be selected at a time. If 
    you choose more than one only the first choice will be run. If you require 
    more than one demo to run at the same time you should create a combined 
    demo and run both the required demo tasks from your demo entry function  */


/* Default configuration for all demos. Individual demos can override these below */
#define democonfigDEMO_STACKSIZE               ( configMINIMAL_STACK_SIZE * 8 )
#define democonfigDEMO_PRIORITY                ( tskIDLE_PRIORITY + 5 )
#define democonfigDEMO_onNetworkConnectedFn    NULL
#define democonfigDEMO_onNetworkDisconnectedFn NULL


/* Individual demos task config and overrides for the above defaults*/
#if defined( demoMQTTDemo_ENABLED )
    #define DEMO_entryFUNCTION                              RunMqttDemo
#elif defined( democonfigSHADOW_DEMO_ENABLED )
    #define DEMO_entryFUNCTION                              RunShadowDemo
#elif defined(democonfigBLE_MQTT_ECHO_DEMO_ENABLED) 
    #define DEMO_entryFUNCTION                              RunBleMqttEchoDemo
#elif defined(democonfigMQTT_SUB_PUB_ENABLED)
    #define DEMO_entryFUNCTION                              vStartSubpubDemoTasksProxy
#elif defined(democonfigGREENGRASS_DISCOVERY_ENABLED)
    #define DEMO_entryFUNCTION                              vStartGreenGrassDiscoveryTask
#else
/* if no demo was defined there will be no entry point defined and we will not be able to run the demo */
    #error "At least one demo should be enabled in the file iot_demo_runner.h"
#endif 

     
/* 
#elif defined(democonfigBLE_MQTT_EchoDemoOnNetworkConnected_DEMO_ENABLED)
    #define DEMO_entryFUNCTION                                   RBleMqttEchoDemoOnNetworkConnected
#elif defined( democonfigTCP_ECHO_ENABLED )
    #define DEMO_entryFUNCTION                                   RunMqttDemo
#elif defined( democonfigOTA_UPDATE_ENABLED )
    #define DEMO_entryFUNCTION                                   RunMqttDemo
    #define democonfigDEMO_STACKSIZE                             ( configMINIMAL_STACK_SIZE * 6 )
#elif defined( democonfigTCP_ECHO_SERVER_ENABLED )
    #define DEMO_entryFUNCTION                                   RunMqttDemo
#elif defined( democonfigTCP_ECHO_TASKS_SEPARATE_ENABLED )
    #define DEMO_entryFUNCTION                                   RunMqttDemo
#elif defined( democonfigMQTT_ECHO_ENABLED )
    #define DEMO_entryFUNCTION                                   RunMqttDemo
#elif defined( democonfigWIFI_CONNECT_ENABLED )
    #define DEMO_entryFUNCTION                                   RunMqttDemo
    #define democonfigDEMO_STACKSIZE                             ( configMINIMAL_STACK_SIZE * 4 )
#elif defined( democonfigWIFI_CONNECT_ENABLED )
    #define DEMO_entryFUNCTION                                   RunMqttDemo
#elif defined( democonfigDEFENDER_ENABLED )
    #define DEMO_entryFUNCTION                                   RunMqttDemo
*/



#define democonfigSHADOW_DEMO_NUM_TASKS             ( 1 )
#define democonfigSHADOW_DEMO_TASK_STACK_SIZE       ( configMINIMAL_STACK_SIZE * 4 )
#define democonfigSHADOW_DEMO_TASK_PRIORITY         ( tskIDLE_PRIORITY + 5 )
#define shadowDemoUPDATE_TASK_STACK_SIZE            ( configMINIMAL_STACK_SIZE * 5 )

#define democonfigMQTT_ECHO_TLS_NEGOTIATION_TIMEOUT pdMS_TO_TICKS( 12000 )
#define democonfigMQTT_ECHO_TASK_STACK_SIZE         ( configMINIMAL_STACK_SIZE * 4 )
#define democonfigMQTT_ECHO_TASK_PRIORITY           ( tskIDLE_PRIORITY )

/* Number of sub pub tasks that connect to a broker that is not using TLS. */
#define democonfigMQTT_SUB_PUB_NUM_UNSECURE_TASKS            ( 0 )
/* Number of sub pub tasks that connect to a broker that is using TLS. */
#define democonfigMQTT_SUB_PUB_NUM_SECURE_TASKS              ( 1 )

#define democonfigMQTT_SUB_PUB_TASK_STACK_SIZE      ( configMINIMAL_STACK_SIZE * 4 )
#define democonfigMQTT_SUB_PUB_TASK_PRIORITY        ( tskIDLE_PRIORITY + 5 )

#define democonfigTCP_ECHO_TASKS_SEPARATE_TASK_STACK_SIZE   ( configMINIMAL_STACK_SIZE * 4 )
#define democonfigTCP_ECHO_TASKS_SEPARATE_TASK_PRIORITY     ( tskIDLE_PRIORITY + 5 )

/* Timeout used when performing MQTT operations that do not need extra time
 * to perform a TLS negotiation. */
#define democonfigMQTT_TIMEOUT                         pdMS_TO_TICKS( 3000 )

/* Send AWS IoT MQTT traffic encrypted to destination port 443. */
#define democonfigMQTT_AGENT_CONNECT_FLAGS             ( mqttagentREQUIRE_TLS | mqttagentUSE_AWS_IOT_ALPN_443 )

#endif /* _AWS_DEMO_CONFIG_H_ */
