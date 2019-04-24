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

#ifndef _IOT_DEMO_RUNNER_H_
#define _IOT_DEMO_RUNNER_H_

/* Choose your demo here. By design only one demo can be selected at a time. If
    you choose more than one only the first choice will be run. If you require
    more than one demo to run at the same time you should create a combined
    demo and run both the required demo tasks from your demo entry function  */

#define   democonfigMQTTDemo_ENABLED
//#define   democonfigSHADOW_DEMO_ENABLED
//#define   democonfigBLE_MQTT_ECHO_DEMO_ENABLED
//#define   democonfigMQTT_SUB_PUB_ENABLED
//#define   democonfigGREENGRASS_DISCOVERY_ENABLED
//#define   democonfigTCP_ECHO_SERVER_ENABLED
//#define   democonfigTCP_ECHO_TASKS_SEPARATE_ENABLED
//#define   democonfigTCP_ECHO_TASKS_SINGLE_ENABLED
//#define   democonfigMQTT_ECHO_ENABLED
//#define   democonfigDEFENDER_DEMO_ENABLED
//#define   democonfigOTA_UPDATE_DEMO_ENABLED

void DEMO_RUNNER_RunDemos( void );

/* Individual demo task entry definitions */
#if defined( democonfigMQTTDemo_ENABLED )
    #define DEMO_entryFUNCTION                              RunMqttDemo
#elif defined( democonfigSHADOW_DEMO_ENABLED )
    #define DEMO_entryFUNCTION                              RunShadowDemo
#elif defined(democonfigBLE_MQTT_ECHO_DEMO_ENABLED)
    #define DEMO_entryFUNCTION                              RunBLEMqttEchoDemo
    #define DEMO_networkConnectedCallback                   BLEMqttEchoDemoNetworkConnectedCallback
    #define DEMO_networkDisconnectedCallback                BLEMqttEchoDemoNetworkDisconnectedCallback
#elif defined(democonfigMQTT_SUB_PUB_ENABLED)
    #define DEMO_entryFUNCTION                              vStartSubpubDemoTasksProxy
#elif defined(democonfigGREENGRASS_DISCOVERY_ENABLED)
    #define DEMO_entryFUNCTION                              vStartGreenGrassDiscoveryTask
#elif defined(democonfigTCP_ECHO_SERVER_ENABLED)
    #define DEMO_entryFUNCTION                              vStartSimpleTCPServerTasksProxy
#elif defined(democonfigTCP_ECHO_TASKS_SEPARATE_ENABLED)
    #define DEMO_entryFUNCTION                              vStartTCPEchoClientTasks_SeparateTasks
#elif defined(democonfigTCP_ECHO_TASKS_SINGLE_ENABLED)
    #define DEMO_entryFUNCTION                              vStartTCPEchoClientTasks_SingleTasks
#elif defined(democonfigMQTT_ECHO_ENABLED)
    #define DEMO_entryFUNCTION                              vStartMQTTEchoDemo
#elif defined(democonfigDEFENDER_DEMO_ENABLED)
    #define DEMO_entryFUNCTION                              vStartDefenderDemo
#elif defined(democonfigPOSIX_DEMO_ENABLED)
    #define DEMO_entryFUNCTION                              vStartPOSIXDemo
#elif defined (democonfigOTA_UPDATE_DEMO_ENABLED)
    #define DEMO_entryFUNCTION                              vStartOTAUpdateDemoTask
#else
/* if no demo was defined there will be no entry point defined and we will not be able to run the demo */
    #error "At least one demo should be enabled in the file iot_demo_runner.h"
#endif


#endif

