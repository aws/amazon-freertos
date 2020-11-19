/*
 * FreeRTOS V202011.00
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





//#include "core_mqtt_serializer.h"

/* Standard includes. */
#include <stdlib.h>
#include <string.h>

/* Demo configuration includes. */
#include "iot_config.h"

/* Demo logging include. */
#include "iot_demo_logging.h"

/* Demo config. */
#include "demo_config.h"

/* Metrics collector. */
#include "metrics_collector.h"

/* Report builder. */
#include "report_builder.h"

/* MQTT operations. */
#include "mqtt_demo_helpers.h"

/* JSON Library. */
#include "core_json.h"

/* Device Defender Client Library. */
#include "defender.h"


/**
 * THING_NAME is required. Throw compilation error if it is not defined.
 */
#ifndef THING_NAME
    #error "Please define THING_NAME to the thing name registered with AWS IoT Core in demo_config.h."
#endif

/**
 * @brief Number of seconds to wait for the response from AWS IoT Device
 * Defender service.
 */
#define DEFENDER_RESPONSE_WAIT_SECONDS    ( 2 )


/**
 * @brief Status values of the device defender report.
 */
typedef enum
{
    ReportStatusNotReceived,
    ReportStatusAccepted,
    ReportStatusRejected
} ReportStatus_t;
/*-----------------------------------------------------------*/


/**
 * @brief The MQTT context used for MQTT operation.
 */
static MQTTContext_t xMqttContext;

/**
 * @brief The network context used for Openssl operation.
 */
static NetworkContext_t xNetworkContext;

/**
 * @brief Static buffer used to hold MQTT messages being sent and received.
 */
static uint8_t ucSharedBuffer[ NETWORK_BUFFER_SIZE ];

/**
 * @brief Static buffer used to hold MQTT messages being sent and received.
 */
static MQTTFixedBuffer_t xBuffer =
{
    .pBuffer = ucSharedBuffer,
    .size    = NETWORK_BUFFER_SIZE
};

/**
 * @brief Network Stats.
 */
static NetworkStats_t networkStats;

/**
 * @brief Open TCP ports array.
 */
static uint16_t openTcpPorts[ OPEN_TCP_PORTS_ARRAY_SIZE ];

/**
 * @brief Open UDP ports array.
 */
static uint16_t openUdpPorts[ OPEN_UDP_PORTS_ARRAY_SIZE ];

/**
 * @brief Established connections array.
 */
static Connection_t establishedConnections[ ESTABLISHED_CONNECTIONS_ARRAY_SIZE ];

/**
 * @brief All the metrics sent in the device defender report.
 */
static ReportMetrics_t deviceMetrics;

/**
 * @brief Report status.
 */
static ReportStatus_t reportStatus;

/**
 * @brief Buffer for generating the device defender report.
 */
static char deviceMetricsJsonReport[ DEVICE_METRICS_REPORT_BUFFER_SIZE ];

/**
 * @brief Report Id sent in the defender report.
 */
static uint32_t reportId = 0;
/*-----------------------------------------------------------*/



/**
 * @brief Callback to receive the incoming publish messages from the MQTT broker.
 *
 * @param[in] pPublishInfo Pointer to publish info of the incoming publish.
 * @param[in] packetIdentifier Packet identifier of the incoming publish.
 */
static void publishCallback( MQTTContext_t * pxMqttContext,
                             MQTTPacketInfo_t * pxPacketInfo,
                             MQTTDeserializedInfo_t * pxDeserializedInfo );

/**
 * @brief Collect all the metrics to be sent in the device defender report.
 *
 * @return true if all the metrics are successfully collected;
 * false otherwise.
 */
static bool collectDeviceMetrics( void );

/**
 * @brief Generate the device defender report.
 *
 * @param[out] pOutReportLength Length of the device defender report.
 *
 * @return true if the report is generated successfully;
 * false otherwise.
 */
static int generateDeviceMetricsReport( uint32_t * pOutReportLength );

/**
 * @brief Subscribe to the device defender topics.
 *
 * @return true if the subscribe is successful;
 * false otherwise.
 */
static bool subscribeToDefenderTopics( MQTTContext_t * pxMqttContext );

/**
 * @brief Unsubscribe from the device defender topics.
 *
 * @return true if the unsubscribe is successful;
 * false otherwise.
 */
static int unsubscribeFromDefenderTopics( MQTTContext_t * pxMqttContext );

/**
 * @brief Validate the response received from the AWS IoT Device Defender Service.
 *
 * This functions checks that a valid JSON is received and the value of reportId
 * is same as was sent in the published report.
 *
 * @param[in] defenderResponse The defender response to validate.
 * @param[in] defenderResponseLength Length of the defender response.
 *
 * @return true if the response is valid;
 * false otherwise.
 */
static bool validateDefenderResponse( const char * defenderResponse,
                                      uint32_t defenderResponseLength );
/*-----------------------------------------------------------*/


static bool validateDefenderResponse( const char * defenderResponse,
                                      uint32_t defenderResponseLength )
{
    bool status = false;
    JSONStatus_t jsonResult = JSONSuccess;
    char * reportIdString;
    size_t reportIdStringLength;
    uint32_t reportIdInResponse;

    /* Is the response a valid JSON? */
    jsonResult = JSON_Validate( defenderResponse, defenderResponseLength );

    if( jsonResult != JSONSuccess )
    {
        LogError( ( "Invalid response from AWS IoT Device Defender Service: %.*s.",
                    ( int ) defenderResponseLength,
                    defenderResponse ) );
    }

    if( jsonResult == JSONSuccess )
    {
        /* Search the reportId key in the response. */
        jsonResult = JSON_Search( ( char * ) defenderResponse,
                                  defenderResponseLength,
                                  "reportId",
                                  sizeof( "reportId" ) - 1,
                                  &( reportIdString ),
                                  &( reportIdStringLength ) );

        if( jsonResult != JSONSuccess )
        {
            LogError( ( "reportId key not found in the response from the"
                        "AWS IoT Device Defender Service: %.*s.",
                        ( int ) defenderResponseLength,
                        defenderResponse ) );
        }
    }

    if( jsonResult == JSONSuccess )
    {
        reportIdInResponse = ( uint32_t ) strtoul( reportIdString, NULL, 10 );

        /* Is the reportId present in the response same as was sent in the
         * published report? */
        if( reportIdInResponse == reportId )
        {
            LogInfo( ( "A valid reponse with reportId %u received from the "
                       "AWS IoT Device Defender Service.", reportId ) );
            status = true;
        }
        else
        {
            LogError( ( "Unexpected reportId found in the response from the AWS"
                        "IoT Device Defender Service. Expected: %u, Found: %u, "
                        "Complete Response: %.*s.",
                        reportIdInResponse,
                        reportId,
                        ( int ) defenderResponseLength,
                        defenderResponse ) );
        }
    }

    return status;
}
/*-----------------------------------------------------------*/

static void publishCallback( MQTTContext_t * pxMqttContext,
                              MQTTPacketInfo_t * pxPacketInfo,
                              MQTTDeserializedInfo_t * pxDeserializedInfo )
{
    DefenderStatus_t status;
    DefenderTopic_t api;
    bool validationResult;
    MQTTPublishInfo_t * pPublishInfo = pxDeserializedInfo->pPublishInfo;
    
    /* Silence compiler warnings about unused variables. */
    ( void ) pxMqttContext;
    ( void ) pxPacketInfo;

    status = Defender_MatchTopic( pPublishInfo->pTopicName,
                                  pPublishInfo->topicNameLength,
                                  &( api ),
                                  NULL,
                                  NULL );

    if( status == DefenderSuccess )
    {
        if( api == DefenderJsonReportAccepted )
        {
            /* Check if the response is valid and is for the report we published. */
            validationResult = validateDefenderResponse( pPublishInfo->pPayload,
                                                         pPublishInfo->payloadLength );

            if( validationResult == true )
            {
                LogInfo( ( "The defender report was accepted by the service. Response: %.*s.",
                           ( int ) pPublishInfo->payloadLength,
                           ( const char * ) pPublishInfo->pPayload ) );
                reportStatus = ReportStatusAccepted;
            }
        }
        else if( api == DefenderJsonReportRejected )
        {
            /* Check if the response is valid and is for the report we published. */
            validationResult = validateDefenderResponse( pPublishInfo->pPayload,
                                                         pPublishInfo->payloadLength );

            if( validationResult == true )
            {
                LogError( ( "The defender report was rejected by the service. Response: %.*s.",
                            ( int ) pPublishInfo->payloadLength,
                            ( const char * ) pPublishInfo->pPayload ) );
                reportStatus = ReportStatusRejected;
            }
        }
        else
        {
            LogError( ( "Unexpected defender API : %d.", api ) );
        }
    }
    else
    {
        LogError( ( "Unexpected publish message received. Topic: %.*s, Payload: %.*s.",
                    ( int ) pPublishInfo->topicNameLength,
                    ( const char * ) pPublishInfo->pTopicName,
                    ( int ) pPublishInfo->payloadLength,
                    ( const char * ) ( pPublishInfo->pPayload ) ) );
    }
}
/*-----------------------------------------------------------*/

static bool collectDeviceMetrics( void )
{
    int status = EXIT_FAILURE;
    MetricsCollectorStatus_t metricsCollectorStatus;
    uint32_t numOpenTcpPorts, numOpenUdpPorts, numEstablishedConnections;

    /* Collect bytes and packets sent and received. */
    metricsCollectorStatus = GetNetworkStats( &( networkStats ) );

    if( metricsCollectorStatus != MetricsCollectorSuccess )
    {
        IotLogError( "GetNetworkStats failed. Status: %d.",
                    metricsCollectorStatus );
    }

    /* Collect a list of open TCP ports. */
    if( metricsCollectorStatus == MetricsCollectorSuccess )
    {
        metricsCollectorStatus = GetOpenTcpPorts( &( openTcpPorts[ 0 ] ),
                                                  OPEN_TCP_PORTS_ARRAY_SIZE,
                                                  &( numOpenTcpPorts ) );

        if( metricsCollectorStatus != MetricsCollectorSuccess )
        {
            IotLogError( "GetOpenTcpPorts failed. Status: %d.",
                        metricsCollectorStatus );
        }
    }

    /* Collect a list of open UDP ports. */
    if( metricsCollectorStatus == MetricsCollectorSuccess )
    {
        metricsCollectorStatus = GetOpenUdpPorts( &( openUdpPorts[ 0 ] ),
                                                  OPEN_UDP_PORTS_ARRAY_SIZE,
                                                  &( numOpenUdpPorts ) );

        if( metricsCollectorStatus != MetricsCollectorSuccess )
        {
            LogError( ( "GetOpenUdpPorts failed. Status: %d.",
                        metricsCollectorStatus ) );
        }
    }
		
    /* Collect a list of established connections. */
    if( metricsCollectorStatus == MetricsCollectorSuccess )
    {
        metricsCollectorStatus = GetEstablishedConnections( &( establishedConnections[ 0 ] ),
                                                            ESTABLISHED_CONNECTIONS_ARRAY_SIZE,
                                                            &( numEstablishedConnections ) );

        if( metricsCollectorStatus != MetricsCollectorSuccess )
        {
            IotLogError( "GetEstablishedConnections failed. Status: %d.",
                        metricsCollectorStatus );
        }
    }

    /* Populate device metrics. */
    if( metricsCollectorStatus == MetricsCollectorSuccess )
    {
        status = EXIT_SUCCESS;
        deviceMetrics.pNetworkStats = &( networkStats );
        deviceMetrics.pOpenTcpPortsArray = &( openTcpPorts[ 0 ] );
        deviceMetrics.openTcpPortsArrayLength = numOpenTcpPorts;
        deviceMetrics.pOpenUdpPortsArray = &( openUdpPorts[ 0 ] );
        deviceMetrics.openUdpPortsArrayLength = numOpenUdpPorts;
        deviceMetrics.pEstablishedConnectionsArray = &( establishedConnections[ 0 ] );
        deviceMetrics.establishedConnectionsArrayLength = numEstablishedConnections;
    }

    

    return status;
}
/*-----------------------------------------------------------*/

static bool subscribeToDefenderTopics( MQTTContext_t * pxMqttContext )
{
    bool status = false;

    status = SubscribeToTopic( pxMqttContext,
                               DEFENDER_API_JSON_ACCEPTED( THING_NAME ),
                               DEFENDER_API_LENGTH_JSON_ACCEPTED( THING_NAME_LENGTH ) );

    if( status == true )
    {
        status = SubscribeToTopic( pxMqttContext,
                                   DEFENDER_API_JSON_REJECTED( THING_NAME ),
                                   DEFENDER_API_LENGTH_JSON_REJECTED( THING_NAME_LENGTH ) );
    }

    return status;
}
/*-----------------------------------------------------------*/

static int unsubscribeFromDefenderTopics( MQTTContext_t * pxMqttContext )
{
    int status = false;

    status = UnsubscribeFromTopic( pxMqttContext,
                                   DEFENDER_API_JSON_ACCEPTED( THING_NAME ),
                                   DEFENDER_API_LENGTH_JSON_ACCEPTED( THING_NAME_LENGTH ) );

    if( status == true )
    {
        status = UnsubscribeFromTopic( pxMqttContext,
                                       DEFENDER_API_JSON_REJECTED( THING_NAME ),
                                       DEFENDER_API_LENGTH_JSON_REJECTED( THING_NAME_LENGTH ) );
    }

    return status;
}
/*-----------------------------------------------------------*/

static int generateDeviceMetricsReport( uint32_t * pOutReportLength )
{
    int status = EXIT_FAILURE;
    ReportBuilderStatus_t reportBuilderStatus;

    /* Generate the metrics report in the format expected by the AWS IoT Device
     * Defender Service. */
    reportBuilderStatus = GenerateJsonReport( &( deviceMetricsJsonReport[ 0 ] ),
                                              DEVICE_METRICS_REPORT_BUFFER_SIZE,
                                              &( deviceMetrics ),
                                              DEVICE_METRICS_REPORT_MAJOR_VERSION,
                                              DEVICE_METRICS_REPORT_MINOR_VERSION,
                                              reportId,
                                              pOutReportLength );

    if( reportBuilderStatus != ReportBuilderSuccess )
    {
        IotLogError( "GenerateJsonReport failed. Status: %d.",
                    reportBuilderStatus );
    }
    else
    {
        IotLogInfo( "Generated Report: %.*s.",                    //MingL: Used to be Debug
                    *pOutReportLength,
                    &( deviceMetricsJsonReport[ 0 ] ) );
        status = EXIT_SUCCESS;
    }

    return status;
}
/*-----------------------------------------------------------*/

/**
 * @brief The function that runs the Defender demo, called by the demo runner.
 *
 * @param[in] awsIotMqttMode Ignored for the Defender demo.
 * @param[in] pIdentifier NULL-terminated Defender Thing Name.
 * @param[in] pNetworkServerInfo Passed to the MQTT connect function when
 * establishing the MQTT connection for Defender.
 * @param[in] pNetworkCredentialInfo Passed to the MQTT connect function when
 * establishing the MQTT connection for Defender.
 * @param[in] pNetworkInterface The network interface to use for this demo.
 *
 * @return `EXIT_SUCCESS` if the demo completes successfully; `EXIT_FAILURE` otherwise.
 */

int RunDeviceDefenderDemo( bool awsIotMqttMode,
                     const char * pIdentifier,
                     void * pNetworkServerInfo,
                     void * pNetworkCredentialInfo,
                     const void * pNetworkInterface )
{
    int exitStatus = EXIT_FAILURE;
    uint32_t reportLength = 0, i, mqttSessionEstablished = 0;


    /* Start with report not received. */
    reportStatus = ReportStatusNotReceived;

    /* Set a report Id to be used. */
    reportId = 1;

    IotLogInfo( "Establishing MQTT session..." );
    exitStatus = EstablishMqttSession( &xMqttContext,
                                   &xNetworkContext,
                                   &xBuffer,
                                   publishCallback );

    if( exitStatus != EXIT_SUCCESS )
    {
        IotLogError( "Failed to establish MQTT session." );
    }
    else
    {
        mqttSessionEstablished = 1;
    }

    if( exitStatus == EXIT_SUCCESS )
    {
        IotLogInfo( "Subscribing to defender topics..." );
        exitStatus = subscribeToDefenderTopics( &xMqttContext );

        if( exitStatus != EXIT_SUCCESS )
        {
            IotLogError( "Failed to subscribe to defender topics." );
        }
    }

    if( exitStatus == EXIT_SUCCESS )
    {
        IotLogInfo( "Collecting device metrics..." );
        exitStatus = collectDeviceMetrics();

        if( exitStatus != EXIT_SUCCESS )
        {
            IotLogError( "Failed to collect device metrics." );
        }
    }

    if( exitStatus == EXIT_SUCCESS )
    {
        IotLogInfo( "Generating device defender report..." );
        exitStatus = generateDeviceMetricsReport( &( reportLength ) );

        if( exitStatus != EXIT_SUCCESS )
        {
            IotLogError( "Failed to generate device defender report." );
        }
    }

    if( exitStatus == EXIT_SUCCESS )
    {
        IotLogInfo( "Publishing device defender report..." );
        exitStatus = PublishToTopic( &xMqttContext,
                                     DEFENDER_API_JSON_PUBLISH( THING_NAME ),
                                     DEFENDER_API_LENGTH_JSON_PUBLISH( THING_NAME_LENGTH ),
                                     &( deviceMetricsJsonReport[ 0 ] ),
                                     reportLength );

        if( exitStatus != EXIT_SUCCESS )
        {
            IotLogError( "Failed to publish device defender report." );
        }
    }

    if( exitStatus == EXIT_SUCCESS )
    {
        for( i = 0; i < DEFENDER_RESPONSE_WAIT_SECONDS; i++ )
        {

            /* reportStatus is updated in the publishCallback. */
            if( reportStatus != ReportStatusNotReceived )
            {
                break;
            }

            /* Wait for sometime between consecutive executions of ProcessLoop. */
            vTaskDelay( pdMS_TO_TICKS( 1000 ) );
        }
    }

    if( reportStatus == ReportStatusNotReceived )
    {
        IotLogError( "Failed to receive response from AWS IoT Device Defender Service." );
        exitStatus = EXIT_FAILURE;
    }

    /* Unsubscribe and disconnect if MQTT session was established. Per the MQTT
     * protocol spec, it is okay to send UNSUBSCRIBE even if no corresponding
     * subscription exists on the broker. Therefore, it is okay to attempt
     * unsubscribe even if one more subscribe failed earlier. */
    if( mqttSessionEstablished == 1 )
    {
        IotLogInfo( "Unsubscribing from defender topics..." );
        exitStatus = unsubscribeFromDefenderTopics( &xMqttContext );

        if( exitStatus != EXIT_SUCCESS )
        {
            IotLogError( "Failed to unsubscribe from defender topics." );
        }

        IotLogInfo( "Closing MQTT session..." );
        ( void ) DisconnectMqttSession( &xMqttContext, &xNetworkContext );
    }

    if( ( exitStatus == EXIT_SUCCESS ) && ( reportStatus == ReportStatusAccepted ) )
    {
        IotLogInfo( "Demo completed successfully." );
    }
    else
    {
        exitStatus = EXIT_FAILURE;
        IotLogError( "Demo failed." );
    }

    return exitStatus;
}
											 

/*-----------------------------------------------------------*/
