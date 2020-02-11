/*
 * FreeRTOS OTA V1.1.1
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

/* The config header is always included first. */
#include "iot_config.h"

/* Standard includes. */
#include <string.h>

/* Common includes. */
#include "iot_init.h"
#include "iot_error.h"

/* MQTT internal include. */
#include "iot_mqtt.h"
/* Platform layer includes. */
#include "platform/iot_clock.h"
#include "platform/iot_threads.h"
/* FreeRTOS OTA agent includes. */
#include "aws_iot_ota_agent.h"

/* Test network header include. */
#include IOT_TEST_NETWORK_HEADER

#define KEEPALIVE_SECONDS            ( 1200 )
#define CONN_TIMEOUT_MS              2000UL
#define ONE_SECOND_DELAY_IN_TICKS    pdMS_TO_TICKS( 1000UL )

/**
 * @brief An MQTT network setup parameter to share among the tests.
 */
static IotMqttNetworkInfo_t networkInfo = IOT_MQTT_NETWORK_INFO_INITIALIZER;
static IotMqttConnectInfo_t connectInfo = IOT_MQTT_CONNECT_INFO_INITIALIZER;

/**
 * @brief Network server info to share among the tests.
 */
static const IotTestNetworkServerInfo_t serverInfo = IOT_TEST_NETWORK_SERVER_INFO_INITIALIZER;

/**
 * @brief Network credential info to share among the tests.
 */
#if IOT_TEST_SECURED_CONNECTION == 1
    static const IotTestNetworkCredentials_t credentials = IOT_TEST_NETWORK_CREDENTIALS_INITIALIZER;
#endif

/**
 * @brief An MQTT connection to share among the tests.
 */
static IotMqttConnection_t mqttConnection = IOT_MQTT_CONNECTION_INITIALIZER;

/* OTA Update task parameters. */
#define OTA_UPDATE_TASK_STACK_SIZE       ( 6 * configMINIMAL_STACK_SIZE )
#define OTA_UPDATE_TASK_TASK_PRIORITY    ( tskIDLE_PRIORITY + 5 )


/* The OTA agent has completed the update job or determined that we're in
 * self test mode. If it was accepted, we want to activate the new image.
 * This typically means we should reset the device to run the new firmware.
 * If now is not a good time to reset the device, it may be activated later
 * by your user code. If the update was rejected, just return without doing
 * anything and we'll wait for another job. If it reported that we should
 * start test mode, normally we would perform some kind of system checks to
 * make sure our new firmware does the basic things we think it should do
 * but we'll just go ahead and set the image as accepted for demo purposes.
 * The accept function varies depending on your platform. Refer to the OTA
 * PAL implementation for your platform in aws_ota_pal.c to see what it
 * does for you.
 */

static void App_OTACompleteCallback( OTA_JobEvent_t eEvent )
{
    OTA_Err_t xErr = kOTA_Err_Uninitialized;

    if( eEvent == eOTA_JobEvent_Activate )
    {
        configPRINTF( ( "Received eOTA_JobEvent_Activate callback from OTA Agent.\r\n" ) );
        OTA_ActivateNewImage();
    }
    else if( eEvent == eOTA_JobEvent_Fail )
    {
        configPRINTF( ( "Received eOTA_JobEvent_Fail callback from OTA Agent.\r\n" ) );
        /* Nothing special to do. The OTA agent handles it. */
    }
    else if( eEvent == eOTA_JobEvent_StartTest )
    {
        /* This demo just accepts the image since it was a good OTA update and networking
         * and services are all working (or we wouldn't have made it this far). If this
         * were some custom device that wants to test other things before calling it OK,
         * this would be the place to kick off those tests before calling OTA_SetImageState()
         * with the final result of either accepted or rejected. */
        configPRINTF( ( "Received eOTA_JobEvent_StartTest callback from OTA Agent.\r\n" ) );
        xErr = OTA_SetImageState( eOTA_ImageState_Accepted );

        if( xErr != kOTA_Err_None )
        {
            OTA_LOG_L1( " Error! Failed to set image state as accepted.\r\n" );
        }
    }
}

static const char * pcStateStr[ eOTA_AgentState_All ] =
{
    "Init",
    "Ready",
    "RequestingJob",
    "WaitingForJob",
    "CreatingFile",
    "RequestingFileBlock",
    "WaitingForFileBlock",
    "ClosingFile",
    "ShuttingDown",
    "Stopped"
};

void vOTAUpdateTestTask( void * pvParameters )
{
    OTA_State_t eState;
    IotMqttError_t mqttStatus;

    /* Initialize common components. */
    if( IotSdk_Init() == true )
    {
        /* Initialize the MQTT library. */
        if( IotMqtt_Init() == IOT_MQTT_SUCCESS )
        {
            /* Set the MQTT network setup parameters. */
            ( void ) memset( &networkInfo, 0x00, sizeof( IotMqttNetworkInfo_t ) );
            networkInfo.createNetworkConnection = true;
            networkInfo.u.setup.pNetworkServerInfo = ( void * ) &serverInfo;
            networkInfo.pNetworkInterface = IOT_TEST_NETWORK_INTERFACE;
            networkInfo.pMqttSerializer = IOT_TEST_MQTT_SERIALIZER;

            /* Set the members of the MQTT connection info. */
            connectInfo.cleanSession = true;
            connectInfo.awsIotMqttMode = true;
            connectInfo.keepAliveSeconds = KEEPALIVE_SECONDS;
            connectInfo.pClientIdentifier = clientcredentialIOT_THING_NAME;
            connectInfo.clientIdentifierLength = ( uint16_t ) strlen( clientcredentialIOT_THING_NAME );

            #if IOT_TEST_SECURED_CONNECTION == 1
                networkInfo.u.setup.pNetworkCredentialInfo = ( void * ) &credentials;
            #endif
        }
        else
        {
            configPRINTF( ( "Failed to initialize MQTT\n" ) );
        }
    }
    else
    {
        configPRINTF( ( "Failed to initialize common components\n" ) );
    }

    for( ; ; )
    {
        mqttStatus = IotMqtt_Connect( &networkInfo,
                                      &connectInfo,
                                      CONN_TIMEOUT_MS,
                                      &mqttConnection );

        if( mqttStatus == IOT_MQTT_SUCCESS )
        {
            eState = OTA_AgentInit( mqttConnection, ( const uint8_t * ) ( clientcredentialIOT_THING_NAME ), App_OTACompleteCallback, ( TickType_t ) ~0 );

            if( eState == eOTA_AgentState_Stopped )
            {
                configPRINTF( ( "Failed to start the OTA Agent\n" ) );
            }

            while( ( eState = OTA_GetAgentState() ) != eOTA_AgentState_Stopped )
            {
                /* Wait forever for OTA traffic but allow other tasks to run and output statistics only once per second. */
                vTaskDelay( ONE_SECOND_DELAY_IN_TICKS );
                configPRINTF( ( "State: %s  Received: %u   Queued: %u   Processed: %u   Dropped: %u\r\n", pcStateStr[ eState ],
                                OTA_GetPacketsReceived(), OTA_GetPacketsQueued(), OTA_GetPacketsProcessed(), OTA_GetPacketsDropped() ) );
            }

            IotMqtt_Disconnect( mqttConnection, false );
        }
        else
        {
            configPRINTF( ( "Failed to connect MQTT with status: %d\n", mqttStatus ) );
        }

        vTaskDelay( ONE_SECOND_DELAY_IN_TICKS );
    }

    /* All done.  FreeRTOS does not allow a task to run off the end of its
     * implementing function, so the task must be deleted. */
    vTaskDelete( NULL );
}

void vStartOTAUpdateDemoTask()
{
    if( xTaskCreate( vOTAUpdateTestTask,
                     "OTAStarter",
                     OTA_UPDATE_TASK_STACK_SIZE,
                     NULL,
                     OTA_UPDATE_TASK_TASK_PRIORITY,
                     NULL ) != pdPASS )
    {
        configPRINTF( ( "Failed to create OTA update test task.\n" ) );
    }

    while( 1 )
    {
        vTaskDelay( ONE_SECOND_DELAY_IN_TICKS );
    }
}
