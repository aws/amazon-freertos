/*
Amazon FreeRTOS OTA Update Demo V1.4.4
Copyright (C) 2017 Amazon.com, Inc. or its affiliates.  All Rights Reserved.

Permission is hereby granted, free of charge, to any person obtaining a copy of
this software and associated documentation files (the "Software"), to deal in
the Software without restriction, including without limitation the rights to
use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
the Software, and to permit persons to whom the Software is furnished to do so,
subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

 http://aws.amazon.com/freertos
 http://www.FreeRTOS.org
*/


/**
 * @file aws_ota_update_demo.c
 * @brief A simple OTA update example.
 *
 * This example initializes the OTA agent to enable OTA updates via the
 * MQTT broker. It simply connects to the MQTT broker with the users
 * credentials and spins in an indefinite loop to allow MQTT messages to be
 * forwarded to the OTA agent for possible processing. The OTA agent does all
 * of the real work; checking to see if the message topic is one destined for
 * the OTA agent. If not, it is simply ignored.
 */
#ifdef IOT_CONFIG_FILE
    #include IOT_CONFIG_FILE
#endif

#include "iot_common.h"

/* MQTT include. */
#include "iot_mqtt.h"
/* Standard includes. */
#include <stdio.h>
#include <string.h>

/* FreeRTOS includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"



/* Demo network handling */
#include "aws_iot_demo_network.h"

/* Required to get the broker address and port. */
#include "aws_clientcredential.h"

/* Amazon FreeRTOS OTA agent includes. */
#include "aws_ota_agent.h"

#include "iot_network_manager_private.h"
/* Required for demo task stack and priority */
#include "aws_ota_update_demo.h"
#include "aws_demo_config.h"
#include "aws_application_version.h"

static void App_OTACompleteCallback(OTA_JobEvent_t eEvent );

static BaseType_t prxCreateNetworkConnection( void );
static IotNetworkError_t prvNetworkDisconnectCallback( void* pvContext );
static void prvNetworkStateChangeCallback( uint32_t ulNetworkType, AwsIotNetworkState_t xNetworkState, void* pvContext );

/*-----------------------------------------------------------*/

#define otademoCONN_TIMEOUT_MS 2000UL

#define echoCONN_RETRY_INTERVAL_SECONDS    ( 5 )

#define echoCONN_RETRY_LIMIT               ( 100 )

#define echoKEEPALIVE_SECONDS              ( 120 )

#define myappONE_SECOND_DELAY_IN_TICKS  pdMS_TO_TICKS( 1000UL )

#define otaDemoNETWORK_TYPES               ( AWSIOT_NETWORK_TYPE_ALL ) 

/**
 * @brief Structure which holds the context for an MQTT connection within Demo.
 */
static MqttConnectionContext_t xConnection =
{
     .pvNetworkConnection = NULL,
     .ulNetworkType       = AWSIOT_NETWORK_TYPE_NONE,
     .xNetworkInfo   = IOT_MQTT_NETWORK_INFO_INITIALIZER,
     .xMqttConnection     = IOT_MQTT_CONNECTION_INITIALIZER,
     .xDisconnectCallback = prvNetworkDisconnectCallback
};

/**
 * @brief Network manager subscription callback.
 */

static SubscriptionHandle_t xSubscriptionHandle = AWSIOT_NETWORK_SUBSCRIPTION_HANDLE_INITIALIZER;

/**
 * @brief Semaphore to indicate a new network is available.
 */
static SemaphoreHandle_t xNetworkAvailableLock = NULL;

/**
 * @brief Flag used to unset, during disconnection of currently connected network. This will
 * trigger a reconnection from the main MQTT task.
 */
static BaseType_t xNetworkConnected = pdFALSE;

static void prvNetworkStateChangeCallback( uint32_t ulNetworkType, AwsIotNetworkState_t xNetworkState, void* pvContext )
{
    if( xNetworkState == eNetworkStateEnabled )
    {
        /* Release the semaphore, to indicate other tasks that a network is available */
        xSemaphoreGive( xNetworkAvailableLock );
    }
    else if ( xNetworkState == eNetworkStateDisabled )
    {
        if( ( AwsIotNetworkManager_GetConnectedNetworks()
                & otaDemoNETWORK_TYPES ) == AWSIOT_NETWORK_TYPE_NONE )
        {
            /* Take the semaphore if not taken already */
            xSemaphoreTake( xNetworkAvailableLock, 0 );
        }

        /* If the publish task is already connected to this network, set connected network flag to none,
         * to trigger a reconnect.
         */
        if( xConnection.ulNetworkType == ulNetworkType )
        {
            xNetworkConnected = pdFALSE;
        }
    }
}


static IotNetworkError_t prvNetworkDisconnectCallback( void* pvContext )
{
    ( void ) pvContext;
    xNetworkConnected = pdFALSE;

    return IOT_NETWORK_SUCCESS;
}

static BaseType_t prxCreateNetworkConnection( void )
{
    BaseType_t xRet = pdFALSE;

    /* If no networks are available, block for a physical network connection */
    if( ( AwsIotNetworkManager_GetConnectedNetworks()
            & otaDemoNETWORK_TYPES ) == AWSIOT_NETWORK_TYPE_NONE )
    {
        /* Block for a Network Connection. */
        configPRINTF(( "Waiting for a network connection.\r\n" ));
        xSemaphoreTake( xNetworkAvailableLock, portMAX_DELAY );
    }

    /* At least one network type is available. Connect to the network type. */
    xRet = xMqttDemoCreateNetworkConnection(
            &xConnection,
            otaDemoNETWORK_TYPES );

    return xRet;
}

static const char *pcStateStr[eOTA_NumAgentStates] =
{
     "Not Ready",
     "Ready",
     "Active",
     "Shutting down"
};

void vOTAUpdateDemoTask( void * pvParameters )
{
    IotMqttConnectInfo_t xConnectInfo = IOT_MQTT_CONNECT_INFO_INITIALIZER;
    OTA_State_t eState;

/* Remove compiler warnings about unused parameters. */
    ( void ) pvParameters;

	configPRINTF ( ("OTA demo version %u.%u.%u\r\n",
		xAppFirmwareVersion.u.x.ucMajor,
		xAppFirmwareVersion.u.x.ucMinor,
		xAppFirmwareVersion.u.x.usBuild ) );
    configPRINTF( ( "Creating MQTT Client...\r\n" ) );

    /* Create the MQTT Client. */

    xNetworkConnected = prxCreateNetworkConnection();


    if( xNetworkConnected  )
    {
        for ( ; ; )
        {
            configPRINTF( ( "Connecting to broker...\r\n" ) );
            memset( &xConnectInfo, 0, sizeof( xConnectInfo ) );
            if( xConnection.ulNetworkType == AWSIOT_NETWORK_TYPE_BLE )
            {
                xConnectInfo.awsIotMqttMode = false;
                xConnectInfo.keepAliveSeconds = 0;
            }
            else
            {
                xConnectInfo.awsIotMqttMode = true;
                xConnectInfo.keepAliveSeconds = echoKEEPALIVE_SECONDS;
            }

            xConnectInfo.cleanSession = true;
            xConnectInfo.clientIdentifierLength = strlen( clientcredentialIOT_THING_NAME );
            xConnectInfo.pClientIdentifier = clientcredentialIOT_THING_NAME;
            /* Connect to the broker. */
            if( IotMqtt_Connect( &( xConnection.xNetworkInfo ),
                &xConnectInfo,
                otademoCONN_TIMEOUT_MS,&( xConnection.xMqttConnection ) ) == IOT_MQTT_SUCCESS )
            {
                configPRINTF( ( "Connected to broker.\r\n" ) );
                OTA_AgentInit( xConnection.xMqttConnection, ( const uint8_t * ) ( clientcredentialIOT_THING_NAME ), App_OTACompleteCallback, ( TickType_t ) ~0 );

                while( ( eState = OTA_GetAgentState() ) != eOTA_AgentState_NotReady )
                {
                    /* Wait forever for OTA traffic but allow other tasks to run and output statistics only once per second. */
                    vTaskDelay( myappONE_SECOND_DELAY_IN_TICKS );
                    configPRINTF( ( "State: %s  Received: %u   Queued: %u   Processed: %u   Dropped: %u\r\n", pcStateStr[eState],
                            OTA_GetPacketsReceived(), OTA_GetPacketsQueued(), OTA_GetPacketsProcessed(), OTA_GetPacketsDropped() ) );
                }
                IotMqtt_Disconnect( xConnection.xMqttConnection, false);
            }
            else
            {
                configPRINTF( ( "ERROR:  MQTT_AGENT_Connect() Failed.\r\n" ) );
            }
            /* After failure to connect or a disconnect, wait an arbitrary one second before retry. */
            vTaskDelay( myappONE_SECOND_DELAY_IN_TICKS );
        }
    }
    else
    {
        configPRINTF( ( "Failed to create MQTT client.\r\n" ) );
    }

    /* All done.  FreeRTOS does not allow a task to run off the end of its
     * implementing function, so the task must be deleted. */
    vTaskDelete( NULL );
}


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

    if ( eEvent == eOTA_JobEvent_Activate )
    {
        configPRINTF( ( "Received eOTA_JobEvent_Activate callback from OTA Agent.\r\n" ) );
        OTA_ActivateNewImage();
    }
    else if (eEvent == eOTA_JobEvent_Fail)
    {
        configPRINTF( ( "Received eOTA_JobEvent_Fail callback from OTA Agent.\r\n" ) );
        /* Nothing special to do. The OTA agent handles it. */
    }
	else if (eEvent == eOTA_JobEvent_StartTest)
	{
		/* This demo just accepts the image since it was a good OTA update and networking
		 * and services are all working (or we wouldn't have made it this far). If this
		 * were some custom device that wants to test other things before calling it OK,
		 * this would be the place to kick off those tests before calling OTA_SetImageState()
		 * with the final result of either accepted or rejected. */
        configPRINTF( ( "Received eOTA_JobEvent_StartTest callback from OTA Agent.\r\n" ) );
	xErr = OTA_SetImageState (eOTA_ImageState_Accepted);
        if( xErr != kOTA_Err_None )
        {
            OTA_LOG_L1( " Error! Failed to set image state as accepted.\r\n" );
        }
	}
}


/*-----------------------------------------------------------*/

void vStartOTAUpdateDemoTask( void )
{
    BaseType_t xRet = pdTRUE;

    /* Initialize common libraries and MQTT. */
    if( IotCommon_Init() == false )
    {
        configPRINTF(( "Failed to initialize common libraries.\r\n" ));
        xRet = pdFALSE;
    }

    if( xRet == pdTRUE )
    {
        if( IotMqtt_Init() != IOT_MQTT_SUCCESS )
        {
            configPRINTF(( "Failed to initialize MQTT library.\r\n" ));
            xRet = pdFALSE;
        }
    }

    if( xRet == pdTRUE )
    {
        if( otaDemoNETWORK_TYPES == AWSIOT_NETWORK_TYPE_NONE )
        {
            configPRINTF(( "There are no networks configured for the demo.\r\n" ));
            xRet = pdFALSE;
        }
    }

    /* Create semaphore to notify network available */
    if( xRet == pdTRUE )
    {
        xNetworkAvailableLock = xSemaphoreCreateBinary();
        if( xNetworkAvailableLock == NULL )
        {
            configPRINTF(( "Failed to create semaphore.\r\n" ));
            xRet = pdFALSE;
        }
    }

    /**
     * Create a Network Manager Subscription for all the network types supported.
     */
    if( xRet == pdTRUE )
    {
        xRet = AwsIotNetworkManager_SubscribeForStateChange( otaDemoNETWORK_TYPES, prvNetworkStateChangeCallback, NULL, &xSubscriptionHandle );
        if( xRet == pdFALSE )
        {
            configPRINTF(( "Failed to create Network Manager subscription.\r\n" ));
        }
    }

    if( xRet == pdTRUE )
    {
        xRet = xTaskCreate( vOTAUpdateDemoTask,
                     "OTA",
                     democonfigOTA_UPDATE_TASK_STACK_SIZE,
                     NULL,
                     democonfigOTA_UPDATE_TASK_TASK_PRIORITY,
                     NULL );

        if( xRet == pdFALSE )
        {
            configPRINTF(( "Failed to create OTA demo tasks.\r\n" ));
        }

    }

    if(  xRet == pdFALSE )
    {
        if( xSubscriptionHandle != AWSIOT_NETWORK_SUBSCRIPTION_HANDLE_INITIALIZER )
        {
            AwsIotNetworkManager_RemoveSubscription( xSubscriptionHandle );
        }

        if( xNetworkAvailableLock != NULL )
        {
            vSemaphoreDelete( xNetworkAvailableLock );
        }
    }
}
