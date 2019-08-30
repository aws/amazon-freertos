/*
 * Amazon FreeRTOS V201908.00
 * Copyright (C) 2019 Amazon.com, Inc. or its affiliates.  All Rights Reserved.
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

#include "aws_demo_config.h"

/*  By design only one demo can be selected at a time. If
 *  you choose more than one only the first choice will be run. If you require
 *  more than one demo to run at the same time you can create a new combined
 *  demo and run both the required demo tasks from your demo entry function as
 *  2 separate tasks
 *
 *  The define to choose the demo to run is in aws_demo_config.h included above.
 */

/* Individual demo task entry definitions */
#if defined( CONFIG_MQTT_DEMO_ENABLED )
    #define DEMO_entryFUNCTION              RunMqttDemo
    #if defined( democonfigMQTT_ECHO_TASK_STACK_SIZE )
        #undef democonfigDEMO_STACKSIZE
        #define democonfigDEMO_STACKSIZE    democonfigMQTT_ECHO_TASK_STACK_SIZE
    #endif
    #if defined( democonfigMQTT_ECHO_TASK_PRIORITY )
        #undef democonfigDEMO_PRIORITY
        #define democonfigDEMO_PRIORITY     democonfigMQTT_ECHO_TASK_PRIORITY
    #endif
#elif defined( CONFIG_SHADOW_DEMO_ENABLED )
    #define DEMO_entryFUNCTION              RunShadowDemo
    #if defined( democonfigSHADOW_DEMO_TASK_STACK_SIZE )
        #undef democonfigDEMO_STACKSIZE
        #define democonfigDEMO_STACKSIZE    democonfigSHADOW_DEMO_TASK_STACK_SIZE
    #endif
    #if defined( democonfigSHADOW_DEMO_TASK_PRIORITY )
        #undef democonfigDEMO_PRIORITY
        #define democonfigDEMO_PRIORITY     democonfigSHADOW_DEMO_TASK_PRIORITY
    #endif
#elif defined( CONFIG_GREENGRASS_DISCOVERY_DEMO_ENABLED )
    #define DEMO_entryFUNCTION              vStartGreenGrassDiscoveryTask
    #if defined( democonfigGREENGRASS_DISCOVERY_TASK_STACK_SIZE )
        #undef democonfigDEMO_STACKSIZE
        #define democonfigDEMO_STACKSIZE    democonfigGREENGRASS_DISCOVERY_TASK_STACK_SIZE
    #endif
    #if defined( democonfigGREENGRASS_DISCOVERY_TASK_PRIORITY )
        #undef democonfigDEMO_PRIORITY
        #define democonfigDEMO_PRIORITY     democonfigGREENGRASS_DISCOVERY_TASK_PRIORITY
    #endif
#elif defined( CONFIG_TCP_ECHO_CLIENT_DEMO_ENABLED )
    #define DEMO_entryFUNCTION              vStartTCPEchoClientTasks_SingleTasks
    #if defined( democonfigTCP_ECHO_TASKS_SINGLE_TASK_STACK_SIZE )
        #undef democonfigDEMO_STACKSIZE
        #define democonfigDEMO_STACKSIZE    democonfigTCP_ECHO_TASKS_SINGLE_TASK_STACK_SIZE
    #endif
    #if defined( democonfigTCP_ECHO_TASKS_SINGLE_TASK_PRIORITY )
        #undef democonfigDEMO_PRIORITY
        #define democonfigDEMO_PRIORITY     democonfigTCP_ECHO_TASKS_SINGLE_TASK_PRIORITY
    #endif
#elif defined( CONFIG_DEFENDER_DEMO_ENABLED )
    #define DEMO_entryFUNCTION              vStartDefenderDemo
#elif defined( CONFIG_POSIX_DEMO_ENABLED )
    #define DEMO_entryFUNCTION              vStartPOSIXDemo
#elif defined( CONFIG_OTA_UPDATE_DEMO_ENABLED )
    #define DEMO_entryFUNCTION              vStartOTAUpdateDemoTask
    #if defined( democonfigOTA_UPDATE_TASK_STACK_SIZE )
        #undef democonfigDEMO_STACKSIZE
        #define democonfigDEMO_STACKSIZE    democonfigOTA_UPDATE_TASK_STACK_SIZE
    #endif
    #if defined( democonfigOTA_UPDATE_TASK_TASK_PRIORITY )
        #undef democonfigDEMO_PRIORITY
        #define democonfigDEMO_PRIORITY    democonfigOTA_UPDATE_TASK_TASK_PRIORITY
    #endif
#elif defined( CONFIG_BLE_GATT_SERVER_DEMO_ENABLED )
    #define DEMO_entryFUNCTION             vGattDemoSvcInit
#elif defined( CONFIG_HTTPS_SYNC_DOWNLOAD_DEMO_ENABLED )
    #define DEMO_entryFUNCTION             RunHttpsSyncDownloadDemo
#elif defined( CONFIG_HTTPS_ASYNC_DOWNLOAD_DEMO_ENABLED )
    #define DEMO_entryFUNCTION             RunHttpsAsyncDownloadDemo
#else /* if defined( CONFIG_MQTT_DEMO_ENABLED ) */
/* if no demo was defined there will be no entry point defined and we will not be able to run the demo */
    #error "No demo to run. One demo should be enabled"
#endif /* if defined( CONFIG_MQTT_DEMO_ENABLED ) */


#endif /* ifndef _IOT_DEMO_RUNNER_H_ */
