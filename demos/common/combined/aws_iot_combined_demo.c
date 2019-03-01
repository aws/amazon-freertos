/*
Amazon FreeRTOS Combined Demo V1.0.0
Copyright (C) 2019 Amazon.com, Inc. or its affiliates.  All Rights Reserved.

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
 * @file aws_iot_combined_demo.c
 * @brief A demo that shows OTA, Pub/Sub, 
 *
 * This example initializes the OTA agent to enable OTA updates via the
 * MQTT broker. It simply connects to the MQTT broker with the users
 * credentials and spins in an indefinite loop to allow MQTT messages to be
 * forwarded to the OTA agent for possible processing. The OTA agent does all
 * of the real work; checking to see if the message topic is one destined for
 * the OTA agent. If not, it is simply ignored.
 */
/* MQTT include. */

#include <stdbool.h>
#include <string.h>

/* Build using a config header, if provided. */
#include "iot_demo.h"

/*
 * FreeRTOS header includes
 */
#include "FreeRTOS.h"
#include "semphr.h"

/* MQTT library header includes */
#include "iot_mqtt.h"

/* POSIX and Platform layer includes. */
#include "platform/iot_clock.h"
#include "FreeRTOS_POSIX/time.h"

/* Network connection includes */
#include "iot_network_manager.h"
#include "aws_iot_demo_network.h"

#include "aws_clientcredential.h"
#include "aws_demo_config.h"

#include "aws_iot_network_manager.h"

/* Amazon FreeRTOS OTA agent includes. */
#include "aws_ota_agent.h"
#include "driver/gpio.h"

/**
 * @brief Transport types supported for MQTT Echo demo.
 * BLE is not here, the BLE on/off triggers MQTT reconnect, which we do not want.
 */
#define demoNETWORK_TYPES          ( AWSIOT_NETWORK_TYPE_WIFI )

/**
 * @brief The topic that the MQTT client both subscribes and publishes to.
 */
#define demoMQTT_TOPIC              "telemetry/example"

#define demoPUBLISH_DATA            "{ \"key\": %03d }"

/**
 * @brief Dimension of the character array buffers used to hold data (strings in
 * this case) that is published to and received from the MQTT broker (in the cloud).
 *
 */
#define demoPUBLISH_DATA_LENGTH            ( sizeof( demoPUBLISH_DATA ) )

/**
 * @brief The string appended to messages that are echoed back to the MQTT broker.
 *
 * It is also used to detect if a received message has already been acknowledged.
 */
#define demoACK_STR                           " ACK"

#define demoACK_STR_LENGTH                     ( sizeof( demoACK_STR ) )

#define demoACK_DATA_LENGTH                    ( demoPUBLISH_DATA_LENGTH + demoACK_STR_LENGTH )

#define demoMQTT_QOS                           ( 1 )

#define demoMQTT_KEEPALIVE_SECONDS             ( 120 )

#define demoMAX_PUBLISH_MESSAGES               ( 120 )

#define demoPUBLISH_TIMEOUT_DELAY_MS           ( 5000 )

#define demoPUBLISH_TIMEOUT_RETRIES            ( 2 )

#define demoMQTT_OPERATION_TIMEOUT_MS          ( 5000 )

#define demoCONNECTION_RETRY_INTERVAL_SECONDS  ( 5 )

#define demoCONNECTION_RETRY_LIMIT             ( 100 )

#define demoOTA_INIT_DELAY                     ( 5000 )

#define demoOTA_DELAY                          ( 1000 )

#define demoBUTTON_DELAY                       ( 1000 )

#define demoQUEUE_SEND                         ( 1000 )

#define producerQUEUE_LENGTH                   ( 10 )
/**
 * @brief IO pin for push button.
 */
#define GPIO_INPUT_IO_0                 ( 4 )

/*-----------------------------------------------------------*/
/**
 * @brief Callback invoked when a message is received from the cloud on the demo topic.
 *
 * If not already echoed, the function echoes back the same message by appending an ACK
 * to the original message. If it's an already echoed message, function discards the message.
 *
 * @param pvUserParam[in] User param for the callback
 * @param pxPublishParam[in] Publish param which contains the topic, the payload and other details.
 */
static void prvEchoMessage( void* pvUserParam, IotMqttCallbackParam_t* pxPublishParam );

/**
 * @brief Recreates the network connection.
 *
 * Tears down the existing network connection, waits for a physical transport type to available and then creates
 * a new network connection.
 *
 * @param pxConnection Pointer to hold the connection.
 * @return New network type connected.
 */
 static BaseType_t prxReCreateConnection( void );

/**
 * @brief Opens a new MQTT connection with the given physical transport type.
 *
 * Function first creates an MQTT connection with an IOT broker endpoint over a physical transport type which
 * can be either WIFI or Bluetooth Low energy.
 *
 * @return true if all the steps succeeded.
 */
 static IotMqttError_t prxOpenMqttConnection( void );

/**
 * @brief Closes an MQTT connection with the broker endpoint.
 *
 * Function first closes the MQTT connection with the broker by optionally (set in the parameter) sending a
 * DISCONNECT message to the broker. It also tears down the physical connectivity betweend the device and the
 * broker.
 *
 * @param bSendDisconnect[in] Should send a DISCONNECT message to the broker.
 */
void prvCloseMqttConnection( BaseType_t xCleanupOnly );

/**
 * @brief Subscribes or unsubscribes to the demo topic.
 * @param bSubscribe[in] Set to true if its a subscribe or false for unsubscribe.
 *
 * @return true if operation was successful.
 */
static IotMqttError_t prxSubscribeorUnsubscribeToTopic( BaseType_t xSubscribe );
/**
 * @brief Publishes a message using QoS0 or QoS1 to the specified topic.
 *
 * @param pcMesg Pointer to the message
 * @param xLength Length of the message
 * @return IOT_MQTT_SUCCESS if the publish was successful.
 */
static IotMqttError_t prxPublishMQTTMessage( const char* pcMesg, size_t xLength );

/**
 * @brief Task used to publish all the messages to the topic.
 * @param pvParameters [in] Task parameter
 */
static void vMqttPubTask( void * pvParameters  );

/**
 * @brief Task used to produce messages into a queue.
 * @param pvParameters [in] Task parameter
 */
static void vProducerTask(void * pvParameters ); 

/**
 * @brief Task used to initiate OTA.
 * @param pvParameters [in] Task parameter
 */
void vOtaTask( void * pvParameters );

/**
 * @brief Callback invoked when a DISCONNECT event was received for the connected network.
 *
 * @param pvContext The handle for the connection to the network.
 */
static IotNetworkError_t prvMQTTDisconnectCallback( void* pvContext );

/* Declaration of snprintf. The header stdio.h is not included because it
 * includes conflicting symbols on some platforms. */
extern int snprintf( char * pcS,
                     size_t xN,
                     const char * pcFormat,
                     ... );

/**
 * @brief Network manager subscription
 */
static SubscriptionHandle_t xSubscriptionHandle = AWSIOT_NETWORK_SUBSCRIPTION_HANDLE_INITIALIZER;

/**
 * @brief Structure which holds the context for an MQTT connection within Demo.
 */
static MqttConnectionContext_t xConnection =
{
     .pvNetworkConnection = NULL,
     .ulNetworkType       = AWSIOT_NETWORK_TYPE_NONE,
     .xNetworkInterface   = IOT_MQTT_NETIF_INITIALIZER,
     .xMqttConnection     = IOT_MQTT_CONNECTION_INITIALIZER,
     .xDisconnectCallback = prvMQTTDisconnectCallback
};

/**
 * @brief Semaphore to indicate a new network is available.
 */
static SemaphoreHandle_t xNetworkAvailableLock = NULL;

/**
 * @brief Flag used to unset, during disconnection of currently connected network. This will
 * trigger a reconnection from the main MQTT task.
 */
static BaseType_t xNetworkConnected = pdFALSE;

/**
 * @brief Global variable used to indiacte if the networking socket is connected.
 * Set by primary applicatino protocol (e.g. MQTT). Used by secondary application
 * protocols (e.g. OTA) to initiate or shutdown application protocol.
 */
static bool xSocketConnected = false;

/**
 * @brief Global variable used to indiacte that an appliction protcol is
 * using the socket. If secondary application is actively using the socket
 * this flag is true. 
 */
static SemaphoreHandle_t xOtaProtocolUsingSocket;

/**
 * @brief Queue of message sent to MQTT Pub task. Meant to simulate UART input.
 */
static QueueHandle_t xTelemetryQueue = NULL;

static const char *pcStateStr[eOTA_NumAgentStates] =
{
     "Not Ready",
     "Ready",
     "Active",
     "Shutting down"
};

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
        IotLogInfo ( "App_OTACompleteCallback: Received eOTA_JobEvent_Activate callback from OTA Agent.\r\n" ) ;
        OTA_ActivateNewImage();
    }
    else if (eEvent == eOTA_JobEvent_Fail)
    {
        IotLogInfo(  "App_OTACompleteCallback: Received eOTA_JobEvent_Fail callback from OTA Agent.\r\n" ) ;
        /* Nothing special to do. The OTA agent handles it. */
    }
    else if (eEvent == eOTA_JobEvent_StartTest)
    {
        /* This demo just accepts the image since it was a good OTA update and networking
         * and services are all working (or we wouldn't have made it this far). If this
         * were some custom device that wants to test other things before calling it OK,
         * this would be the place to kick off those tests before calling OTA_SetImageState()
         * with the final result of either accepted or rejected. */
        IotLogInfo(  "App_OTACompleteCallback: Received eOTA_JobEvent_StartTest callback from OTA Agent.\r\n"  );
	xErr = OTA_SetImageState (eOTA_ImageState_Accepted);
        if( xErr != kOTA_Err_None )
        {
            OTA_LOG_L1( " App_OTACompleteCallback: Error! Failed to set image state as accepted.\r\n" );    
        }
    }
}

void vOtaTask( void * pvParameters )
{
    OTA_State_t otaState = eOTA_AgentState_Unknown;
    OTA_State_t otaLastState = eOTA_AgentState_Unknown;
    uint32_t noSocketCounter = 0;
    bool otaInited = false;
    TickType_t otaInitDelay  = pdMS_TO_TICKS( demoOTA_INIT_DELAY);
    TickType_t otaDelay  = pdMS_TO_TICKS( demoOTA_DELAY );

    /* Remove compiler warnings about unused parameters. */
    ( void ) pvParameters;

    IotLogInfo( "vOtaTask: start task\n" );

    while(1)
    {
        otaState = OTA_GetAgentState();

        if (xSocketConnected)
        {
            if (!otaInited)
            {
                IotLogInfo( "vOtaTask: Socket just connected, initing OTA\n" );
                xSemaphoreTake(xOtaProtocolUsingSocket, portMAX_DELAY);
                IotLogInfo( "vOtaTask: initing OTA, after take semaphore\n" );
                if (OTA_AgentInit(xConnection.xMqttConnection, ( const uint8_t * ) ( clientcredentialIOT_THING_NAME ), App_OTACompleteCallback, ( TickType_t ) ~0 ) == eOTA_AgentState_Ready)
                {
                    IotLogInfo( "vOtaTask: initing OTA, after init\n" );
                    otaInited = true;
                }
                else
                {
                    xSemaphoreGive(xOtaProtocolUsingSocket);
                }
            }
            else 
            {
                if (otaState ==  eOTA_AgentState_Active)
                {
                    IotLogInfo(  "vOtaTask: State: %s  Received: %u   Queued: %u   Processed: %u   Dropped: %u\r\n",
                        pcStateStr[otaState], OTA_GetPacketsReceived(), OTA_GetPacketsQueued(), 
                        OTA_GetPacketsProcessed(), OTA_GetPacketsDropped() ) ;
                }
                else if (otaState ==  eOTA_AgentState_NotReady || otaState ==  eOTA_AgentState_Ready || 
                         otaState ==  eOTA_AgentState_ShuttingDown)
                {
                    IotLogInfo("vOtaTask: State: %s \n", pcStateStr[otaState]);
                }
                else if (otaState ==  eOTA_AgentState_NotReady || otaState ==  eOTA_AgentState_Ready || 
                         otaState ==  eOTA_AgentState_ShuttingDown)
                {
                    IotLogInfo("vOtaTask: State: %s \n", pcStateStr[otaState]);
                }
                else if (otaState ==  eOTA_AgentState_Unknown)
                {
                    /* Note: value of eOTA_AgentState_Unknown is -1. Don't use as index intopcStateStr[] */
                    IotLogInfo("vOtaTask: State: Unknown\n");
                }

                if ((otaLastState == eOTA_AgentState_Ready || 
                     otaLastState == eOTA_AgentState_Active)
                        &&
                    (otaState == eOTA_AgentState_ShuttingDown || 
                     otaState == eOTA_AgentState_NotReady  || 
                     otaState == eOTA_AgentState_Unknown))
                {
                    IotLogInfo("vOtaTask: Socket is connected. Unexpected state change\n");
                }
            }
            noSocketCounter = 0;
        }
        else 
        {
            if (otaInited)
            {
                /* take Semaphore happened when OTA was initiated*/
                IotLogInfo( "vOtaTask: Socket disconnected, shutdown OTA\n" );
                OTA_AgentShutdown(  (TickType_t ) ~0 );
                IotLogInfo( "vOtaTask: OTA is shutdown\n" );
                otaInited = false;
                xSemaphoreGive(xOtaProtocolUsingSocket);
                IotLogInfo( "vOtaTask: Gave up semaphore\n" );
            }
            else
            {
                if (!(noSocketCounter % 30))
                {
                    IotLogInfo( "vOtaTask: waiting for socket to connect\n" );
                 }   
                vTaskDelay( otaInitDelay  );
            }
            noSocketCounter++;
        } 
        vTaskDelay( otaDelay  );
        otaLastState = otaState;
    }
    IotLogInfo( "vOtaTask: stop task\n" );
}


bool buttonWasPushed(void)
{
    int numSecsToWait = 5;
    bool pushed = false;
    bool released  = false;
    TickType_t buttonDelay = pdMS_TO_TICKS( demoBUTTON_DELAY );

    while (numSecsToWait--)
    {
        if (gpio_get_level(GPIO_NUM_0) == 0)
        {
            /*   IotLogInfo("buttonWasPushed: button pushed\n"); */
            pushed = true;
            break;
        }
        vTaskDelay( buttonDelay );
    }
    if (pushed)
    {
        numSecsToWait = 20;
        while (numSecsToWait--)
        {
            if (gpio_get_level(GPIO_NUM_0) == 1)
            {
                /* IotLogInfo("buttonWasPushed: button released\n"); */
                released = true;
                break;
            }
            vTaskDelay( buttonDelay );
        }
    }
    return released;
}

void vBLEButtonTask(void * pvParameters )
{
    // Enable GPIO 0 so that we can read the state
    gpio_config_t io_conf;
    io_conf.intr_type = GPIO_PIN_INTR_DISABLE;
    //bit mask of the pins, use GPIO0 here
    io_conf.pin_bit_mask = (1<<GPIO_INPUT_IO_0);
    //set as input mode
    io_conf.mode = GPIO_MODE_INPUT;
    //enable pull-up mode
    io_conf.pull_up_en = 1;
    gpio_config(&io_conf);

    IotLogInfo( "vBLEButtonTask: start task\n" );

    while( true )
    {
        if (buttonWasPushed())
        {
            if (AwsIotNetworkManager_GetEnabledNetworks() & AWSIOT_NETWORK_TYPE_BLE) 
            {
                IotLogInfo( "vBLEButtonTask: BLE Button OFF\n");
                AwsIotNetworkManager_DisableNetwork(AWSIOT_NETWORK_TYPE_BLE);
            }
            else 
            {
                /* Once BLE starts, WiFi can be reprovisioned which triggers a WiFi disconnect call back. */
                IotLogInfo( "vBLEButtonTask: BLE Button ON\n");
                AwsIotNetworkManager_EnableNetwork(AWSIOT_NETWORK_TYPE_BLE);
            }
        }
    }
    IotLogInfo( "vBLEButtonTask: stop task\n" );
}

void vProducerTask(void * pvParameters )
{
    uint16_t counter = 0;
    uint16_t failCounter = 0;
    TickType_t qsendDelay = pdMS_TO_TICKS( demoQUEUE_SEND );
    /* Create the queue used to pass data to publish task. */
    IotLogInfo( "vProducerTask: start task\n" );
    while( true )
    {
        if( xQueueSend( xTelemetryQueue, &counter, pdMS_TO_TICKS(1000) ) != pdPASS )
        {
            if (!failCounter % 20)
            {
                IotLogError(( "vProducerTask: Failed to send data to TelemetryQueue\n" ) );
            }
            failCounter++;
        }
        else 
        {
            failCounter = 0;
            ++counter;
        }   
        /*IotLogInfo( "vProducerTask: produced %d\n", (int) counter);*/
        vTaskDelay( qsendDelay );
    }
    IotLogInfo( "vProducerTask: stop task\n" );
}

IotMqttError_t prxPublishMQTTMessage( const char* pcMesg, size_t xLength )
{
    IotMqttPublishInfo_t xPublishInfo = IOT_MQTT_PUBLISH_INFO_INITIALIZER;
    IotMqttReference_t xOperationLock = IOT_MQTT_REFERENCE_INITIALIZER;
    IotMqttError_t xStatus;

    xPublishInfo.qos = 1;
    xPublishInfo.pTopicName = demoMQTT_TOPIC;
    xPublishInfo.topicNameLength = strlen( demoMQTT_TOPIC );

    xPublishInfo.pPayload = ( void * ) pcMesg;
    xPublishInfo.payloadLength = xLength;
    xPublishInfo.retryLimit = 0;
            /* WiFI is started before vStartCombinedDemo(); therefore, this Give is required. */
    xPublishInfo.retryMs = 0;


    if( demoMQTT_QOS == 0 )
    {
        xStatus = IotMqtt_Publish(
                xConnection.xMqttConnection,
                &xPublishInfo,
                0,
                NULL,
                NULL );
    }
    else if( demoMQTT_QOS == 1 )
    {
        /* PUBLISH a message with timeout */
        xStatus = IotMqtt_TimedPublish(
            xConnection.xMqttConnection,
            &xPublishInfo,
            0,
            demoPUBLISH_TIMEOUT_DELAY_MS );

        IotLogInfo("prxPublishMQTTMessage: pub string = %s", xPublishInfo.pPayload);
    }
    else if( demoMQTT_QOS == 2 )
    {
        xStatus = IotMqtt_Publish(
                xConnection.xMqttConnection,
                &xPublishInfo,
                IOT_MQTT_FLAG_WAITABLE,
                NULL,
                &xOperationLock );
        if( xStatus == IOT_MQTT_STATUS_PENDING )
        {
            xStatus = IotMqtt_Wait( xOperationLock, demoMQTT_OPERATION_TIMEOUT_MS );
        }
    }

    return xStatus;
}

void prvEchoMessage( void* pvUserParam, IotMqttCallbackParam_t* pxPublishParam )
{

    size_t xAckPos, xPayloadLen = pxPublishParam->message.info.payloadLength;
    const char *pcPayload = ( const char *) pxPublishParam->message.info.pPayload;
    char cAck[ demoACK_DATA_LENGTH ] = { 0 };
    IotMqttError_t xStatus;

    /* User parameters are not used */
    ( void ) pvUserParam;

    xAckPos = xPayloadLen - demoACK_STR_LENGTH;
    if( strncmp( ( pcPayload + xAckPos ), demoACK_STR, demoACK_STR_LENGTH ) != 0 )
    {
        IotLogInfo( "Received Message: %.*s.", xPayloadLen, pcPayload);

        if( xPayloadLen < demoPUBLISH_DATA_LENGTH )
        {
            memcpy(cAck, pcPayload,  xPayloadLen );
            strcat( cAck, demoACK_STR );
            xStatus = prxPublishMQTTMessage( cAck, strlen( cAck ) );
            if( xStatus != IOT_MQTT_SUCCESS )
            {
                IotLogInfo(" Failed to send ACK Message, reason: %s.", IotMqtt_strerror( xStatus ));
            }
            else
            {
                IotLogInfo( "Sent ACK message: %s.\n", cAck );
            }
        }
    }
}

static BaseType_t prxCreateNetworkConnection( void )
{

    BaseType_t xRet = pdFALSE;

    /* If no networks are available, block for a physical network connection */
    if( ( AwsIotNetworkManager_GetConnectedNetworks() & demoNETWORK_TYPES ) == 0 )
    {
        /* Block for a Network Connection. */
        IotLogInfo( "Waiting for a network connection.");
        xSemaphoreTake( xNetworkAvailableLock, portMAX_DELAY );
    }

    /* At least one network type is available. Connect to the network type. */
    xRet = xMqttDemoCreateNetworkConnection(
            &xConnection,
            demoNETWORK_TYPES,
            demoCONNECTION_RETRY_INTERVAL_SECONDS,
            demoCONNECTION_RETRY_LIMIT );

    return xRet;
}


static BaseType_t prxReCreateConnection( void )
{
    vMqttDemoDeleteNetworkConnection( &xConnection );
    return prxCreateNetworkConnection();
}

static IotMqttError_t prxOpenMqttConnection(void)
{

    IotMqttConnectInfo_t xConnectInfo = IOT_MQTT_CONNECT_INFO_INITIALIZER;
    IotMqttError_t xMqttStatus;

    if( xConnection.ulNetworkType == AWSIOT_NETWORK_TYPE_BLE )
    {
        xConnectInfo.awsIotMqttMode = false;
        xConnectInfo.keepAliveSeconds = 0;
    }
    else
    {
        xConnectInfo.awsIotMqttMode = true;
        xConnectInfo.keepAliveSeconds = demoMQTT_KEEPALIVE_SECONDS;
    }

    xConnectInfo.cleanSession = true;
    xConnectInfo.clientIdentifierLength = strlen( clientcredentialIOT_THING_NAME );
    xConnectInfo.pClientIdentifier = clientcredentialIOT_THING_NAME;

    /* Connect to the IoT broker endpoint */
    xMqttStatus = IotMqtt_Connect(
            &( xConnection.xMqttConnection ),
            &( xConnection.xNetworkInterface ),
            &xConnectInfo,
            demoMQTT_OPERATION_TIMEOUT_MS );


    if( xMqttStatus == IOT_MQTT_SUCCESS )
    {
        /* MQTT Connection succeeded, subscribe to the topic */
        xMqttStatus = prxSubscribeorUnsubscribeToTopic( pdTRUE );
    }

    if( xMqttStatus != IOT_MQTT_SUCCESS )
    {
        /* Close the MQTT connection to perform any cleanup */
        prvCloseMqttConnection( pdFALSE );
    }

    return xMqttStatus;
}

static IotMqttError_t prxSubscribeorUnsubscribeToTopic( BaseType_t xSubscribe )
{
    IotMqttReference_t xOperationLock = IOT_MQTT_REFERENCE_INITIALIZER;
    IotMqttSubscription_t xSubscription = IOT_MQTT_SUBSCRIPTION_INITIALIZER;
    IotMqttError_t xMqttStatus;

    xSubscription.qos = demoMQTT_QOS;
    xSubscription.callback.function = prvEchoMessage;
    xSubscription.callback.param1 = NULL;
    xSubscription.pTopicFilter = demoMQTT_TOPIC;
    xSubscription.topicFilterLength = strlen( demoMQTT_TOPIC );

    if( xSubscribe )
    {
        xMqttStatus = IotMqtt_Subscribe(
                xConnection.xMqttConnection,
                &xSubscription,
                1,
                IOT_MQTT_FLAG_WAITABLE,
                NULL,
                &xOperationLock );
    }
    else
    {
        xMqttStatus = IotMqtt_Unsubscribe(
                xConnection.xMqttConnection,
                &xSubscription,
                1,
                IOT_MQTT_FLAG_WAITABLE,
                NULL,
                &xOperationLock );

    }

    if( xMqttStatus == IOT_MQTT_STATUS_PENDING )
    {
        xMqttStatus = IotMqtt_Wait( xOperationLock, demoMQTT_OPERATION_TIMEOUT_MS );
    }

    return xMqttStatus;
}
void prvCloseMqttConnection( BaseType_t xCleanupOnly )
{
    /* Close the MQTT connection either by sending a DISCONNECT operation or not */
    if( xConnection.xMqttConnection != IOT_MQTT_CONNECTION_INITIALIZER )
    {
        IotMqtt_Disconnect( xConnection.xMqttConnection, xCleanupOnly );
        xConnection.xMqttConnection = IOT_MQTT_CONNECTION_INITIALIZER;
    }
}

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
               & demoNETWORK_TYPES ) == AWSIOT_NETWORK_TYPE_NONE )
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

static IotNetworkError_t prvMQTTDisconnectCallback( void* pvContext )
{
    ( void ) pvContext;
    xNetworkConnected = pdFALSE;
    return IOT_NETWORK_SUCCESS;
}


void vMqttPubTask( void * pvParam  )
{
    uint16_t receiveValue = 0;
    uint32_t publishRetries = demoPUBLISH_TIMEOUT_RETRIES;
    char cMessage[ demoPUBLISH_DATA_LENGTH ] = { 0 };
    size_t xMessageLength;

    TickType_t xPublishRetryDelay = pdMS_TO_TICKS( demoPUBLISH_TIMEOUT_DELAY_MS );

    IotMqttError_t xMqttStatus = IOT_MQTT_SUCCESS ;

    /* Avoid compiler warnings about the parameters */
    (void ) pvParam;

    IotLogInfo( "vMqttPubTask: start task\n" );

    /* Creates a physical networks connection */
    xNetworkConnected = prxCreateNetworkConnection();
    if( xNetworkConnected != pdFALSE )
    {
        /* Open an MQTT connection */
        xMqttStatus = prxOpenMqttConnection();
        if( xMqttStatus == IOT_MQTT_SUCCESS )
        {
            /* Start publishing MQTT Messages in a loop */
            while(1)
            {
                if( xNetworkConnected != pdFALSE && publishRetries > 0)
                {
                    xSocketConnected = true;

                    /* Block to wait for data from publisher. */
                    if (xQueueReceive(xTelemetryQueue, &receiveValue, portMAX_DELAY) == pdPASS)
                    {
                        /* Set length of receiveValue in cMessage to always be 3 characters. 
                         * Otherwise the prvEchoMessage() does not find the "ACK" string in 
                         * the response, then we get ACKs on ACKs.
                         */
                        receiveValue %= 1000;

                        xMessageLength = snprintf( 
                            cMessage,
                            demoPUBLISH_DATA_LENGTH,
                            demoPUBLISH_DATA,
                            (int) ( receiveValue ) ) ;

                        xMqttStatus = prxPublishMQTTMessage( cMessage, xMessageLength );

                        if( xMqttStatus == IOT_MQTT_SUCCESS)
                        {
                            IotLogInfo( "vMqttPubTask: Pub Thread Sent SUCCESS. Msg=%s, Count = %d", 
                                    cMessage, xMessageLength);
                            publishRetries = demoPUBLISH_TIMEOUT_RETRIES;
                        }
                        else if( xMqttStatus == IOT_MQTT_NETWORK_ERROR)
                        {
                            IotLogInfo( "vMqttPubTask: publish SEND ERROR\n");
                            publishRetries--;
                            vTaskDelay( xPublishRetryDelay );
                        }
                        else if( xMqttStatus == IOT_MQTT_TIMEOUT)
                        {
                            IotLogInfo( "vMqttPubTask: publish TIMEOUT error\n");
                            vTaskDelay( xPublishRetryDelay );
                        }
                        else if( ( xMqttStatus == IOT_MQTT_NO_MEMORY ) || ( xMqttStatus == IOT_MQTT_BAD_PARAMETER ) )
                        {
                            IotLogInfo( "vMqttPubTask:Pub Task Failed to publish Message, error = %s", IotMqtt_strerror( xMqttStatus ) );
                            break;
                        }
                    }
                }
                else
                {
                    publishRetries = demoPUBLISH_TIMEOUT_RETRIES;

                    xSocketConnected = false;
                    IotLogInfo( "vMqttPubTask; Socket connection dropped, reconnect\n" );
                    xSemaphoreTake(xOtaProtocolUsingSocket, portMAX_DELAY);
                    IotLogInfo( "vMqttPubTask; Socket connection dropped, have semaphore\n" );

                    /* Recreate the Network Connection */
                    IotLogError("vMqttPubTask: reconnecting:: before close .");
                    prvCloseMqttConnection( pdTRUE );
                    IotLogError("vMqttPubTask: reconnecting: before reconnect.");
                    xNetworkConnected = prxReCreateConnection();
                    if( xNetworkConnected == pdFALSE )
                    {
                        IotLogError(("vMqttPubTask: reconnecting: Failed to reconnect to Network."));
                        break;
                    }

                    /* Create an MQTT connection over the network connection */
                    IotLogError(("vMqttPubTask: reconnecting: before Open."));
                    xMqttStatus = prxOpenMqttConnection();
                    IotLogError(("vMqttPubTask: reconnecting: after Open."));
                    if( xMqttStatus != IOT_MQTT_SUCCESS )
                    {
                        IotLogError(("vMqttPubTask: reconnecting: Failed to create an MQTT connection."));
                        break;
                    }

                    xSocketConnected = true;
                    IotLogInfo( "vMqttPubTask; Socket connection reconnected\n" );
                    xSemaphoreGive(xOtaProtocolUsingSocket);
                }
            }
        }
    }
    else
    {
        IotLogError(("vMqttPubTask: failed to create a network connection."));
    }

    if( xNetworkConnected != pdFALSE )
    {
        vMqttDemoDeleteNetworkConnection( &xConnection );
    }

    IotLogInfo( "vMqttPubTask: stop task\n" );
    AwsIotNetworkManager_RemoveSubscription( xSubscriptionHandle );
    vSemaphoreDelete( xNetworkAvailableLock );
    vTaskDelete( NULL );
}

void vStartCombinedDemo( void )
{
    BaseType_t xRet = pdTRUE;
    IotLogInfo( "vStartCombinedDemo: ****** starting ****** \n");

    xTelemetryQueue = xQueueCreate( producerQUEUE_LENGTH, sizeof( uint16_t ) );
    if( xTelemetryQueue == NULL )
    {
        IotLogError(( "vStartCombinedDemo: creating telemetry queue failed." ));
        xRet = pdFALSE;
    }

    if (xRet == pdTRUE )
    {
        xNetworkAvailableLock = xSemaphoreCreateBinary();
        if( xNetworkAvailableLock == NULL )
        {
            IotLogError(( "vStartCombinedDemo: Creating xNetworkAvailableLock failed."));
            xRet = pdFALSE;
        }
    }

    if (xRet == pdTRUE )
    {
        xOtaProtocolUsingSocket= xSemaphoreCreateBinary();
        if( xOtaProtocolUsingSocket == NULL )
        {
            IotLogError(( "vStartCombinedDemo: Creating xNetworkAvailableLock failed."));
            xRet = pdFALSE;
        }
        else 
        {
            xSemaphoreGive(xOtaProtocolUsingSocket);
        }
    }
    if( xRet == pdTRUE )
    {
        xRet = AwsIotNetworkManager_SubscribeForStateChange( demoNETWORK_TYPES, prvNetworkStateChangeCallback, NULL, &xSubscriptionHandle );
        if( xRet == pdFALSE )
        {
            IotLogError(( "vStartCombinedDemo: Failed to create Network Manager subscription." ));
        }
    }

    if( xRet == pdTRUE )
    {
        xRet = xTaskCreate( vMqttPubTask,
            "MqttPub",
            democonfigMQTT_PUB_TASK_STACK_SIZE,
            NULL,
            democonfigMQTT_PUB_TASK_PRIORITY,
            NULL);
        if( xRet == pdFALSE )
        {
            IotLogError(( "vStartCombinedDemo: Failed to start vMqttPubTask." ));
        }
    }

    if( xRet == pdTRUE )
    {
        xRet = xTaskCreate(vProducerTask,
            "Producer",
            democonfigPRODUCER_TASK_STACK_SIZE,
            NULL,
            democonfigPRODUCER_TASK_PRIORITY,
            NULL);
        if( xRet == pdFALSE )
        {
            IotLogError(( "vStartCombinedDemo: Failed to start vProducerTask." ));
        }
    }

    if( xRet == pdTRUE )
    {
        xRet = xTaskCreate(vBLEButtonTask,
            "BLE_Button",
            democonfigBLE_BUTTON_TASK_STACK_SIZE,
            NULL,
            democonfigBLE_BUTTON_TASK_PRIORITY,
            NULL);
        if( xRet == pdFALSE )
        {
            IotLogError(( "vStartCombinedDemo: Failed to start vBLEButtonTask." ));
        }
    }

    if( xRet == pdTRUE )
    {
        xRet = xTaskCreate( vOtaTask,
            "OTA",
            democonfigCOMBINED_OTA_TASK_STACK_SIZE,
            NULL,
            democonfigCOMBINED_OTA_TASK_PRIORITY,
            NULL);
        if( xRet == pdFALSE )
        {
            IotLogError(( "vStartCombinedDemo: Failed to start vOtaTask." ));
        }
    }

    IotLogInfo( "vStartCombinedDemo; ****** exiting ****** ");
}
