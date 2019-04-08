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


#define   democonfigMQTTDemo_ENABLED 
/* #define   democonfigSHADOW_DEMO_ENABLED */             
/* #define   democonfigBLE_MQTT_ECHO_DEMO_ENABLED  */     
/* #define   democonfigMQTT_SUB_PUB_ENABLED    */         
/* #define   democonfigGREENGRASS_DISCOVERY_ENABLED  */
/* #define   democonfigTCP_ECHO_SERVER_ENABLED  */               
/* #define   democonfigTCP_ECHO_TASKS_SEPARATE_ENABLED */
/* #define   democonfigMQTT_ECHO_ENABLED */           
/* #define   democonfigDEFENDER_DEMO_ENABLED */


// TODO: This one needs some work, there are multiple implementations and use of Network Manager
/* #define   democonfigOTA_UPDATE_ENABLED */
// TODO: This one is commented out in the implementation ???
//#define   democonfigTCP_ECHO_ENABLED
// TODO: This demo does not compile due to include paths, especially the portable path
// #define   democonfigPOSIX_DEMO_ENABLED

// The functions are :
    /* vStartPOSIXDemo(); */
    /* vStartOTAUpdateDemoTask(); */
    /* vStartTCPEchoClientTasks_SingleTasks */


void DEMO_RUNNER_RunDemos( void );

#endif

