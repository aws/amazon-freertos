/*
 * FreeRTOS V202012.00
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

/**
 * @file aws_iot_ota_update_demo.c
 * @brief A simple OTA update example.
 *
 * This example initializes the OTA agent to enable OTA updates via the
 * MQTT broker. It simply connects to the MQTT broker with the users
 * credentials and spins in an indefinite loop to allow MQTT messages to be
 * forwarded to the OTA agent for possible processing. The OTA agent does all
 * of the real work; checking to see if the message topic is one destined for
 * the OTA agent. If not, it is simply ignored.
 */

/* The config header is always included first. */
#include "iot_config.h"

/* Standard includes. */
#include <stdio.h>
#include <string.h>

/* MQTT include. */
#include "iot_mqtt.h"

/* Platform includes for demo. */
#include "platform/iot_clock.h"

/* Set up logging for this demo. */
#include "iot_demo_logging.h"

/* Required to get the broker address and port. */
#include "aws_clientcredential.h"

/* FreeRTOS OTA agent includes. */
#include "aws_iot_ota_agent.h"

/* Required for demo task stack and priority */
#include "aws_demo_config.h"
#include "aws_application_version.h"

/**
 * @brief Timeout for MQTT connection, if the MQTT connection is not established within
 * this time, the connect function returns #IOT_MQTT_TIMEOUT
 */
#define OTA_DEMO_CONNECTION_TIMEOUT_MS               ( 2000UL )

/**
 * @brief The maximum time interval that is permitted to elapse between the point at
 * which the MQTT client finishes transmitting one control Packet and the point it starts
 * sending the next.In the absence of control packet a PINGREQ  is sent. The broker must
 * disconnect a client that does not send a message or a PINGREQ packet in one and a
 * half times the keep alive interval.
 */
#define OTA_DEMO_KEEP_ALIVE_SECONDS                  ( 120UL )

/**
 * @brief The delay used in the main OTA Demo task loop to periodically output the OTA
 * statistics like number of packets received, dropped, processed and queued per connection.
 */
#define OTA_DEMO_TASK_DELAY_SECONDS                  ( 1UL )

/**
 * @brief The base interval in seconds for retrying network connection.
 */
#define OTA_DEMO_CONN_RETRY_BASE_INTERVAL_SECONDS    ( 4U )

/**
 * @brief The maximum interval in seconds for retrying network connection.
 */
#define OTA_DEMO_CONN_RETRY_MAX_INTERVAL_SECONDS     ( 360U )

/**
 * @brief The longest client identifier that an MQTT server must accept (as defined
 * by the MQTT 3.1.1 spec) is 23 characters. Add 1 to include the length of the NULL
 * terminator.
 */
#define OTA_DEMO_CLIENT_IDENTIFIER_MAX_LENGTH        ( 24 )

/**
 * @brief Handle of the MQTT connection used in this demo.
 */
static IotMqttConnection_t _mqttConnection = IOT_MQTT_CONNECTION_INITIALIZER;

/**
 * @brief Flag used to unset, during disconnection of currently connected network. This will
 * trigger a reconnection from the OTA demo task.
 */
volatile static bool _networkConnected = false;

/**
 * @brief Connection retry interval in seconds.
 */
static int _retryInterval = OTA_DEMO_CONN_RETRY_BASE_INTERVAL_SECONDS;

static const char * _pStateStr[ eOTA_AgentState_All ] =
{
    "Init",
    "Ready",
    "RequestingJob",
    "WaitingForJob",
    "CreatingFile",
    "RequestingFileBlock",
    "WaitingForFileBlock",
    "ClosingFile",
    "Suspended",
    "ShuttingDown",
    "Stopped"
};

/**
 * @brief Initialize the libraries required for OTA demo.
 *
 * @return `EXIT_SUCCESS` if all libraries were successfully initialized;
 * `EXIT_FAILURE` otherwise.
 */
static int _initializeOtaDemo( void )
{
    int status = EXIT_SUCCESS;
    IotMqttError_t mqttInitStatus = IOT_MQTT_SUCCESS;

    /* Initialize the MQTT library.*/
    if( status == EXIT_SUCCESS )
    {
        mqttInitStatus = IotMqtt_Init();

        if( mqttInitStatus != IOT_MQTT_SUCCESS )
        {
            /* Failed to initialize MQTT library.*/
            status = EXIT_FAILURE;
        }
    }

    return status;
}

/**
 * @brief Clean up libraries initialized for OTA demo.
 */
static void _cleanupOtaDemo( void )
{
    /* Cleanup MQTT library.*/
    IotMqtt_Cleanup();
}

/**
 * @brief Delay before retrying network connection up to a maximum interval.
 */
static void _connectionRetryDelay( void )
{
    unsigned int retryIntervalwithJitter = 0;

    if( ( _retryInterval * 2 ) >= OTA_DEMO_CONN_RETRY_MAX_INTERVAL_SECONDS )
    {
        /* Retry interval is already max.*/
        _retryInterval = OTA_DEMO_CONN_RETRY_MAX_INTERVAL_SECONDS;
    }
    else
    {
        /* Double the retry interval time.*/
        _retryInterval *= 2;
    }

    /* Add random jitter upto current retry interval .*/
    retryIntervalwithJitter = _retryInterval + ( rand() % _retryInterval );

    IotLogInfo( "Retrying network connection in %d Secs ", retryIntervalwithJitter );

    /* Delay for the calculated time interval .*/
    IotClock_SleepMs( retryIntervalwithJitter * 1000 );
}

/**
 * @brief Initialize the libraries required for OTA demo.
 *
 * @return `EXIT_SUCCESS` if all libraries were successfully initialized;
 * `EXIT_FAILURE` otherwise.
 */

static void prvNetworkDisconnectCallback( void * param,
                                          IotMqttCallbackParam_t * mqttCallbackParams )
{
    ( void ) param;

    /* Log the reason for MQTT disconnect.*/
    switch( mqttCallbackParams->u.disconnectReason )
    {
        case IOT_MQTT_DISCONNECT_CALLED:
            IotLogInfo( "Mqtt disconnected due to invoking diconnect function.\r\n" );
            break;

        case IOT_MQTT_BAD_PACKET_RECEIVED:
            IotLogInfo( "Mqtt disconnected due to invalid packet received from the network.\r\n" );
            break;

        case IOT_MQTT_KEEP_ALIVE_TIMEOUT:
            IotLogInfo( "Mqtt disconnected due to Keep-alive response not received.\r\n" );
            break;

        default:
            IotLogInfo( "Mqtt disconnected due to unknown reason." );
            break;
    }

    /* Clear the flag for network connection status.*/
    _networkConnected = false;
}

/**
 * @brief Establish a new connection to the MQTT server.
 *
 * @param[in] awsIotMqttMode Specify if this demo is running with the AWS IoT
 * MQTT server. Set this to `false` if using another MQTT server.
 * @param[in] pIdentifier NULL-terminated MQTT client identifier.
 * @param[in] pNetworkServerInfo Passed to the MQTT connect function when
 * establishing the MQTT connection.
 * @param[in] pNetworkCredentialInfo Passed to the MQTT connect function when
 * establishing the MQTT connection.
 * @param[in] pNetworkInterface The network interface to use for this demo.
 * @param[out] pMqttConnection Set to the handle to the new MQTT connection.
 *
 * @return `EXIT_SUCCESS` if the connection is successfully established; `EXIT_FAILURE`
 * otherwise.
 */
static int _establishMqttConnection( bool awsIotMqttMode,
                                     const char * pIdentifier,
                                     void * pNetworkServerInfo,
                                     void * pNetworkCredentialInfo,
                                     const IotNetworkInterface_t * pNetworkInterface,
                                     IotMqttConnection_t * pMqttConnection )
{
    int status = EXIT_SUCCESS;
    IotMqttError_t connectStatus = IOT_MQTT_STATUS_PENDING;
    IotMqttNetworkInfo_t networkInfo = IOT_MQTT_NETWORK_INFO_INITIALIZER;
    IotMqttConnectInfo_t connectInfo = IOT_MQTT_CONNECT_INFO_INITIALIZER;
    IotMqttPublishInfo_t willInfo = IOT_MQTT_PUBLISH_INFO_INITIALIZER;
    char pClientIdentifierBuffer[ OTA_DEMO_CLIENT_IDENTIFIER_MAX_LENGTH ] = { 0 };

    /* Set the members of the network info not set by the initializer. This
     * struct provided information on the transport layer to the MQTT connection. */
    networkInfo.createNetworkConnection = true;
    networkInfo.u.setup.pNetworkServerInfo = pNetworkServerInfo;
    networkInfo.u.setup.pNetworkCredentialInfo = pNetworkCredentialInfo;
    networkInfo.pNetworkInterface = pNetworkInterface;
    networkInfo.disconnectCallback.function = prvNetworkDisconnectCallback;

    /* Set the members of the connection info not set by the initializer. */
    connectInfo.awsIotMqttMode = awsIotMqttMode;
    connectInfo.cleanSession = true;
    connectInfo.awsIotMqttMode = true;
    connectInfo.keepAliveSeconds = OTA_DEMO_KEEP_ALIVE_SECONDS;
    connectInfo.clientIdentifierLength = ( uint16_t ) strlen( clientcredentialIOT_THING_NAME );
    connectInfo.pClientIdentifier = clientcredentialIOT_THING_NAME;

    /* Establish the MQTT connection. */
    if( status == EXIT_SUCCESS )
    {
        IotLogInfo( "MQTT demo client identifier is %.*s (length %hu).",
                    connectInfo.clientIdentifierLength,
                    connectInfo.pClientIdentifier,
                    connectInfo.clientIdentifierLength );

        connectStatus = IotMqtt_Connect( &networkInfo,
                                         &connectInfo,
                                         OTA_DEMO_CONNECTION_TIMEOUT_MS,
                                         pMqttConnection );

        if( connectStatus != IOT_MQTT_SUCCESS )
        {
            IotLogError( "MQTT CONNECT returned error %s.",
                         IotMqtt_strerror( connectStatus ) );

            status = EXIT_FAILURE;
        }
    }

    return status;
}

/**
 * @brief The OTA agent has completed the update job or it is in
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
 *
 * @param[in] eEvent Specify if this demo is running with the AWS IoT
 * MQTT server. Set this to `false` if using another MQTT server.
 * @return None.
 */
static void App_OTACompleteCallback( OTA_JobEvent_t eEvent )
{
    OTA_Err_t xErr = kOTA_Err_Uninitialized;

    DEFINE_OTA_METHOD_NAME( "App_OTACompleteCallback" );

    /* OTA job is completed. so delete the MQTT and network connection. */
    if( eEvent == eOTA_JobEvent_Activate )
    {
        IotLogInfo( "Received eOTA_JobEvent_Activate callback from OTA Agent.\r\n" );

        /* OTA job is completed. so delete the network connection. */
        if( _mqttConnection != NULL )
        {
            IotMqtt_Disconnect( _mqttConnection, false );
        }

        /* Activate the new firmware image. */
        OTA_ActivateNewImage();

        /* We should never get here as new image activation must reset the device.*/
        IotLogError( "New image activation failed.\r\n" );

        for( ; ; )
        {
        }
    }
    else if( eEvent == eOTA_JobEvent_Fail )
    {
        IotLogInfo( "Received eOTA_JobEvent_Fail callback from OTA Agent.\r\n" );

        /* Nothing special to do. The OTA agent handles it. */
    }
    else if( eEvent == eOTA_JobEvent_StartTest )
    {
        /* This demo just accepts the image since it was a good OTA update and networking
         * and services are all working (or we wouldn't have made it this far). If this
         * were some custom device that wants to test other things before calling it OK,
         * this would be the place to kick off those tests before calling OTA_SetImageState()
         * with the final result of either accepted or rejected. */

        IotLogInfo( "Received eOTA_JobEvent_StartTest callback from OTA Agent.\r\n" );
        xErr = OTA_SetImageState( eOTA_ImageState_Accepted );

        if( xErr != kOTA_Err_None )
        {
            IotLogError( " Error! Failed to set image state as accepted.\r\n" );
        }
    }
}

/**
 * @brief The function that implements the main OTA demo task loop. It first
 * establishes the connection , initializes the OTA Agent, keeps logging
 * OTA statistics and restarts the process if OTA Agent stops.
 *
 * @param[in] awsIotMqttMode Specify if this demo is running with the AWS IoT
 * MQTT server. Set this to `false` if using another MQTT server.
 * @param[in] pIdentifier NULL-terminated MQTT client identifier.
 * @param[in] pNetworkServerInfo Passed to the MQTT connect function when
 * establishing the MQTT connection.
 * @param[in] pNetworkCredentialInfo Passed to the MQTT connect function when
 * establishing the MQTT connection.
 * @param[in] pNetworkInterface The network interface to use for this demo.
 *
 * @return `EXIT_SUCCESS` if the demo completes successfully; `EXIT_FAILURE` otherwise.
 */
void vRunOTAUpdateDemo( bool awsIotMqttMode,
                        const char * pIdentifier,
                        void * pNetworkServerInfo,
                        void * pNetworkCredentialInfo,
                        const IotNetworkInterface_t * pNetworkInterface )
{
    OTA_State_t eState;
    static OTA_ConnectionContext_t xOTAConnectionCtx;

    IotLogInfo( "OTA demo version %u.%u.%u\r\n",
                xAppFirmwareVersion.u.x.ucMajor,
                xAppFirmwareVersion.u.x.ucMinor,
                xAppFirmwareVersion.u.x.usBuild );

    for( ; ; )
    {
        IotLogInfo( "Connecting to broker...\r\n" );

        /* Establish a new MQTT connection. */
        if( _establishMqttConnection( awsIotMqttMode,
                                      pIdentifier,
                                      pNetworkServerInfo,
                                      pNetworkCredentialInfo,
                                      pNetworkInterface,
                                      &_mqttConnection ) == EXIT_SUCCESS )
        {
            /* Update the connection context shared with OTA Agent.*/
            xOTAConnectionCtx.pxNetworkInterface = ( void * ) pNetworkInterface;
            xOTAConnectionCtx.pvNetworkCredentials = pNetworkCredentialInfo;
            xOTAConnectionCtx.pvControlClient = _mqttConnection;

            /* Set the base interval for connection retry.*/
            _retryInterval = OTA_DEMO_CONN_RETRY_BASE_INTERVAL_SECONDS;

            /* Update the connection available flag.*/
            _networkConnected = true;

            /* Check if OTA Agent is suspended and resume.*/
            if( ( eState = OTA_GetAgentState() ) == eOTA_AgentState_Suspended )
            {
                OTA_Resume( &xOTAConnectionCtx );
            }

            /* Initialize the OTA Agent , if it is resuming the OTA statistics will be cleared for new connection.*/
            OTA_AgentInit( ( void * ) ( &xOTAConnectionCtx ),
                           ( const uint8_t * ) ( clientcredentialIOT_THING_NAME ),
                           App_OTACompleteCallback,
                           ( TickType_t ) ~0 );

            while( ( ( eState = OTA_GetAgentState() ) != eOTA_AgentState_Stopped ) && _networkConnected )
            {
                /* Wait forever for OTA traffic but allow other tasks to run and output statistics only once per second. */
                IotClock_SleepMs( OTA_DEMO_TASK_DELAY_SECONDS * 1000 );

                IotLogInfo( "State: %s  Received: %u   Queued: %u   Processed: %u   Dropped: %u\r\n", _pStateStr[ eState ],
                            OTA_GetPacketsReceived(), OTA_GetPacketsQueued(), OTA_GetPacketsProcessed(), OTA_GetPacketsDropped() );
            }

            /* Check if we got network disconnect callback and suspend OTA Agent.*/
            if( _networkConnected == false )
            {
                /* Suspend OTA agent.*/
                if( OTA_Suspend() == kOTA_Err_None )
                {
                    while( ( eState = OTA_GetAgentState() ) != eOTA_AgentState_Suspended )
                    {
                        /* Wait for OTA Agent to process the suspend event. */
                        IotClock_SleepMs( OTA_DEMO_TASK_DELAY_SECONDS * 1000 );
                    }
                }
            }
            else
            {
                /* Try to close the MQTT connection. */
                if( _mqttConnection != NULL )
                {
                    IotMqtt_Disconnect( _mqttConnection, 0 );
                }
            }
        }
        else
        {
            IotLogError( "ERROR:  MQTT_AGENT_Connect() Failed.\r\n" );
        }

        /* After failure to connect or a disconnect, delay for retrying connection. */
        _connectionRetryDelay();
    }
}

/**
 * @brief The function that runs the OTA demo, called by the demo runner.
 *
 * @param[in] awsIotMqttMode Specify if this demo is running with the AWS IoT
 * MQTT server. Set this to `false` if using another MQTT server.
 * @param[in] pIdentifier NULL-terminated MQTT client identifier.
 * @param[in] pNetworkServerInfo Passed to the MQTT connect function when
 * establishing the MQTT connection.
 * @param[in] pNetworkCredentialInfo Passed to the MQTT connect function when
 * establishing the MQTT connection.
 * @param[in] pNetworkInterface The network interface to use for this demo.
 *
 * @return `EXIT_SUCCESS` if the demo completes successfully; `EXIT_FAILURE` otherwise.
 */
int vStartOTAUpdateDemoTask( bool awsIotMqttMode,
                             const char * pIdentifier,
                             void * pNetworkServerInfo,
                             void * pNetworkCredentialInfo,
                             const IotNetworkInterface_t * pNetworkInterface )
{
    /* Return value of this function and the exit status of this program. */
    int status = EXIT_SUCCESS;

    /* Flags for tracking which cleanup functions must be called. */
    bool otademoInitialized = false;

    /* Initialize the libraries required for this demo. */
    status = _initializeOtaDemo();

    if( status == EXIT_SUCCESS )
    {
        otademoInitialized = true;

        /* Start OTA Agent.*/
        vRunOTAUpdateDemo( awsIotMqttMode,
                           pIdentifier,
                           pNetworkServerInfo,
                           pNetworkCredentialInfo,
                           pNetworkInterface );
    }

    /* Clean up libraries if they were initialized. */
    if( otademoInitialized == true )
    {
        _cleanupOtaDemo();
    }

    return status;
}
