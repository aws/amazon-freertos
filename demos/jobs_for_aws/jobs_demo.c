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
 */

/**
 * @file jobs_demo.c
 *
 * @brief Demo for showing use of the Jobs library API. This demo uses the Jobs library
 * along with the coreMQTT and Secure Sockets libraries to communicate with the AWS IoT Jobs service.
 * Please refer to AWS documentation for more information about AWS IoT Jobs.
 * https://docs.aws.amazon.com/iot/latest/developerguide/iot-jobs.html
 *
 * The Jobs library API provides macros and helper functions for assembling MQTT topics strings,
 * and for determining whether an incoming MQTT message is related to the AWS IoT Jobs service.
 * The Jobs library does not depend on a MQTT library, therefore the code for MQTT connections
 * are placed in another file (mqtt_demo_helpers.c). to make it easy to read the demo code using the
 * Jobs library.
 *
 * @note This demo requires setup of an AWS account, provisioning of a Thing resource on the AWS IoT account,
 * and the creation of Jobs for the Thing resource. Please refer to AWS CLI documentation for more information in creating a
 * Job document.
 * https://docs.aws.amazon.com/cli/latest/reference/iot/create-job.html
 *
 * This demo connects to the AWS IoT broker and calls the MQTT APIs of the AWS IoT Jobs service to receive
 * jobs queued (as JSON documents) for the Thing resource (associated with this demo application) on the cloud,
 * executes the jobs and updates the status of the jobs back to the cloud.
 * The demo expects Job documents to have an "action" JSON key. Actions can
 * be one "print", "publish", or "exit".
 * Print Jobs log a message to the local console, and must contain a "message",
 * e.g. { "action": "print", "message": "Hello World!" }.
 * Publish Jobs publish a message to an MQTT Topic. The Job document must
 * contain a "message" and "topic" to publish to, e.g.
 * { "action": "publish", "topic": "demo/jobs", "message": "Hello World!" }.
 * The exit Job exits the demo. Sending { "action": "exit" } will end the program.
 *
 */

/* Standard includes. */
#include <assert.h>
#include <stdlib.h>
#include <string.h>

/* Demo Specific configs. */
#include "jobs_demo_config.h"

#include "aws_demo.h"

/* Kernel includes. */
#include "FreeRTOS.h"
#include "task.h"

/* Jobs library header. */
#include "jobs.h"

/* JSON library includes. */
#include "core_json.h"

/* Include common MQTT demo helpers. */
#include "mqtt_demo_helpers.h"
#include "core_json.h"

/*------------- Demo configurations -------------------------*/

/** Note: The device client certificate and private key credentials are
 * obtained by the transport layer library (transport_interface_secure_sockets)
 * from the demos/include/aws_clientcredential_keys.h file for connection to broker.
 *
 * The following macros SHOULD be defined for this demo which uses both server
 * and client authentications for TLS session:
 *   - keyCLIENT_CERTIFICATE_PEM for client certificate.
 *   - keyCLIENT_PRIVATE_KEY_PEM for client private key.
 */
#ifndef democonfigTHING_NAME

/**
 * @brief Predefined thing name.
 *
 * This is the example predefine thing name and could be compiled in ROM code.
 */
    #define democonfigTHING_NAME    clientcredentialIOT_THING_NAME
#endif

/**
 * @brief The length of #democonfigTHING_NAME.
 */
#define THING_NAME_LENGTH                           ( ( uint16_t ) ( sizeof( democonfigTHING_NAME ) - 1 ) )

/**
 * @brief The JSON key of the execution object.
 *
 * Job documents received from the AWS IoT Jobs service are in JSON format.
 * All such JSON documents will contain this key, whose value represents the unique
 * identifier of a Job.
 */
#define jobsexampleEXECUTION_KEY                    "execution"

/**
 * @brief The length of #jobsexampleEXECUTION_KEY.
 */
#define jobsexampleEXECUTION_KEY_LENGTH             ( sizeof( jobsexampleEXECUTION_KEY ) - 1 )

/**
 * @brief The query key to use for searching the Job ID key in message payload
 * from AWS IoT Jobs service.
 *
 * Job documents received from the AWS IoT Jobs service are in JSON format.
 * All such JSON documents will contain this key, whose value represents the unique
 * identifier of a Job.
 */
#define jobsexampleQUERY_KEY_FOR_JOB_ID             jobsexampleEXECUTION_KEY  ".jobId"

/**
 * @brief The length of #jobsexampleQUERY_KEY_FOR_JOB_ID.
 */
#define jobsexampleQUERY_KEY_FOR_JOB_ID_LENGTH      ( sizeof( jobsexampleQUERY_KEY_FOR_JOB_ID ) - 1 )

/**
 * @brief The query key to use for searching the Jobs document ID key in message payload
 * from AWS IoT Jobs service.
 *
 * Job documents received from the AWS IoT Jobs service are in JSON format.
 * All such JSON documents will contain this key, whose value represents the unique
 * identifier of a Job.
 */
#define jobsexampleQUERY_KEY_FOR_JOBS_DOC           jobsexampleEXECUTION_KEY  ".jobDocument"

/**
 * @brief The length of #jobsexampleQUERY_KEY_FOR_JOBS_DOC.
 */
#define jobsexampleQUERY_KEY_FOR_JOBS_DOC_LENGTH    ( sizeof( jobsexampleQUERY_KEY_FOR_JOBS_DOC ) - 1 )

/**
 * @brief The query key to use for searching the Action key in Jobs document
 * from AWS IoT Jobs service.
 *
 * This demo program expects this key to be in the Job document. It is a key
 * specific to this demo.
 */
#define jobsexampleQUERY_KEY_FOR_ACTION             jobsexampleQUERY_KEY_FOR_JOBS_DOC".action"

/**
 * @brief The length of #jobsexampleQUERY_KEY_FOR_ACTION.
 */
#define jobsexampleQUERY_KEY_FOR_ACTION_LENGTH      ( sizeof( jobsexampleQUERY_KEY_FOR_ACTION ) - 1 )

/**
 * @brief The query key to use for searching the Message key in Jobs document
 * from AWS IoT Jobs service.
 *
 * This demo program expects this key to be in the Job document if the "action"
 * is either "publish" or "print". It represents the message that should be
 * published or printed, respectively.
 */
#define jobsexampleQUERY_KEY_FOR_MESSAGE            jobsexampleQUERY_KEY_FOR_JOBS_DOC ".message"

/**
 * @brief The length of #jobsexampleQUERY_KEY_FOR_MESSAGE.
 */
#define jobsexampleQUERY_KEY_FOR_MESSAGE_LENGTH     ( sizeof( jobsexampleQUERY_KEY_FOR_MESSAGE ) - 1 )

/**
 * @brief The query key to use for searching the topic key in Jobs document
 * from AWS IoT Jobs service.
 *
 * This demo program expects this key to be in the Job document if the "action"
 * is "publish". It represents the MQTT topic on which the message should be
 * published.
 */
#define jobsexampleQUERY_KEY_FOR_TOPIC              jobsexampleQUERY_KEY_FOR_JOBS_DOC".topic"

/**
 * @brief The length of #jobsexampleQUERY_KEY_FOR_TOPIC.
 */
#define jobsexampleQUERY_KEY_FOR_TOPIC_LENGTH       ( sizeof( jobsexampleQUERY_KEY_FOR_TOPIC ) - 1 )

/**
 * @brief The minimum length of a string in a JSON Job document.
 *
 * At the very least the Job ID must have the quotes that identify it as a JSON
 * string and 1 character for the string itself (the string must not be empty).
 */
#define jobsexampleJSON_STRING_MIN_LENGTH           ( ( size_t ) 3 )

/**
 * @brief Time to wait before exiting demo.
 *
 * The milliseconds to wait before exiting. This is because the MQTT Broker
 * will disconnect us if we are idle too long, and we have disabled keep alive.
 */
#define jobsexampleMS_BEFORE_EXIT                   ( 10 * 60 * 1000 )

/**
 * @brief Utility macro to generate the PUBLISH topic string to the
 * DescribePendingJobExecution API of AWS IoT Jobs service for requesting
 * the next pending job information.
 *
 * @param[in] thingName The name of the Thing resource to query for the
 * next pending job.
 */
#define START_NEXT_JOB_TOPIC( thingName ) \
    ( JOBS_API_PREFIX ""                  \
      thingName "" JOBS_API_BRIDGE \
    ""JOBS_API_STARTNEXT )

/**
 * @brief Utility macro to generate the subscription topic string for the
 * NextJobExecutionChanged API of AWS IoT Jobs service that is required
 * for getting notification about changes in the next pending job in the queue.
 *
 * @param[in] thingName The name of the Thing resource to query for the
 * next pending job.
 */
#define NEXT_JOB_EXECUTION_CHANGED_TOPIC( thingName ) \
    ( JOBS_API_PREFIX ""                              \
      thingName "" JOBS_API_BRIDGE                    \
      ""JOBS_API_NEXTJOBCHANGED )

/**
 * @brief Format a JSON status message.
 *
 * @param[in] x one of "IN_PROGRESS", "SUCCEEDED", or "FAILED"
 */
#define MAKE_STATUS_REPORT( x )    "{\"status\":\"" x "\"}"

/**
 * @brief Empty JSON message to send as PUBLISH message payload to
 * AWS IoT Jobs service APIs.
 */
#define JSON_EMPTY_REQUEST    "{}"


/*-----------------------------------------------------------*/

/**
 * @brief Currently supported actions that a Job document can specify.
 */
typedef enum JobActionType
{
    JOB_ACTION_PRINT,   /**< Print a message. */
    JOB_ACTION_PUBLISH, /**< Publish a message to an MQTT topic. */
    JOB_ACTION_EXIT,    /**< Exit the demo. */
    JOB_ACTION_UNKNOWN  /**< Unknown action. */
} JobActionType;


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
 * @brief The flag to indicate the mqtt session changed.
 */
static BaseType_t mqttSessionEstablished = pdTRUE;

/**
 * @brief Static buffer used to hold MQTT messages being sent and received.
 */
static uint8_t ucSharedBuffer[ democonfigNETWORK_BUFFER_SIZE ];

/**
 * @brief Static buffer used to hold MQTT messages being sent and received.
 */
static MQTTFixedBuffer_t xBuffer =
{
    .pBuffer = ucSharedBuffer,
    .size    = democonfigNETWORK_BUFFER_SIZE
};

/*-----------------------------------------------------------*/

/**
 * @brief This example uses the MQTT library of the AWS IoT Device SDK for
 * Embedded C. This is the prototype of the callback function defined by
 * that library. It will be invoked whenever the MQTT library receives an
 * incoming message.
 *
 * @param[in] pxMqttContext MQTT context pointer.
 * @param[in] pxPacketInfo Packet Info pointer for the incoming packet.
 * @param[in] pxDeserializedInfo Deserialized information from the incoming packet.
 */
static void prvEventCallback( MQTTContext_t * pxMqttContext,
                              MQTTPacketInfo_t * pxPacketInfo,
                              MQTTDeserializedInfo_t * pxDeserializedInfo );

/**
 * @brief Process payload from $aws/things<thingName>/jobs/<jobId>/get/accepted topic.
 *
 * This handler parses the payload received about the next pending job to identify
 * the action requested in the job document, and perform the appropriate
 * action to execute the job.
 *
 * @param[in] pPublishInfo Deserialized publish info pointer for the incoming
 * packet.
 */
static void prvStartNextJobHandler( MQTTPublishInfo_t * pxPublishInfo );

/*exitActionHandler */
/*publishActionHandler */
/*printActionHandler */

/**
 * @brief A global flag which represents whether a job for the "Exit" action
 * has been received from AWS IoT Jobs service.
 */
static BaseType_t xExitActionJobReceived = pdFALSE;

/**
 * @brief Processes success response from the UpdateJobExecution API for a job update
 * sent to the AWS IoT Jobs service.
 * The success response is received on the $aws/things/<thingName>/jobs/update/accepted
 * topic.
 *
 * This handler examines that the accepted message that carries the same clientToken
 * as sent before.
 *
 * @param[in] pxPublishInfo Deserialized publish info pointer for the incoming
 * packet.
 */
static void prvUpdateJobAcceptedResponeHandler( MQTTPublishInfo_t * pxPublishInfo );

/*-----------------------------------------------------------*/

static JobActionType prvGetAction( const char * pcAction,
                                   size_t xActionLength )
{
    JobActionType xAction = JOB_ACTION_UNKNOWN;

    configASSERT( pcAction != NULL );

    if( strncmp( pcAction, "print", xActionLength ) == 0 )
    {
        xAction = JOB_ACTION_PRINT;
    }
    else if( strncmp( pcAction, "publish", xActionLength ) == 0 )
    {
        xAction = JOB_ACTION_PUBLISH;
    }
    else if( strncmp( pcAction, "exit", xActionLength ) == 0 )
    {
        xAction = JOB_ACTION_EXIT;
    }

    return xAction;
}


static void prvStartNextJobHandler( MQTTPublishInfo_t * pxPublishInfo )
{
    JSONStatus_t xJsonStatus = JSONSuccess;

    configASSERT( pxPublishInfo != NULL );
    configASSERT( ( pxPublishInfo->pPayload != NULL ) && ( pxPublishInfo->payloadLength > 0 ) );

    /* Check validaty of JSON message response from server.*/
    xJsonStatus = JSON_Validate( pxPublishInfo->pPayload, pxPublishInfo->payloadLength );

    if( xJsonStatus != JSONSuccess )
    {
        LogError( ( "Received invalid JSON payload from AWS IoT Jobs service" ) );
    }
    else
    {
        char * pcJobId = NULL;
        size_t ulJobIdLength = 0U;

        xJsonStatus = JSON_Search( ( char * ) pxPublishInfo->pPayload,
                                   pxPublishInfo->payloadLength,
                                   jobsexampleQUERY_KEY_FOR_JOB_ID,
                                   jobsexampleQUERY_KEY_FOR_JOB_ID_LENGTH,
                                   &pcJobId,
                                   &ulJobIdLength );

        if (xJsonStatus == JSONSuccess)
        {
            char* pcAction = NULL;
            size_t uActionLength = 0U;
            JobActionType xActionType = JOB_ACTION_UNKNOWN;

            LogInfo(("Received a Job from AWS IoT Jobs service: JobId=%.*s",
                    ulJobIdLength, pcJobId));

            xJsonStatus = JSON_Search( ( char * ) pxPublishInfo->pPayload,
                                       pxPublishInfo->payloadLength,
                                       jobsexampleQUERY_KEY_FOR_ACTION,
                                       jobsexampleQUERY_KEY_FOR_ACTION_LENGTH,
                                       &pcAction,
                                       &uActionLength );

            xActionType = prvGetAction( pcAction, uActionLength );

            switch( xActionType )
            {
                case JOB_ACTION_EXIT:
                    LogInfo( ( "Received job contains \"Exit\" action. Updating state of demo." ) );
                    xExitActionJobReceived = pdTRUE;
                    break;

                case JOB_ACTION_PRINT:
                    LogInfo( ( "Received job contains \"Print\" action." ) );
                    char * pcMessage = NULL;
                    size_t ulMessageLength = 0U;

                    xJsonStatus = JSON_Search( ( char * ) pxPublishInfo->pPayload,
                                               pxPublishInfo->payloadLength,
                                               jobsexampleQUERY_KEY_FOR_MESSAGE,
                                               jobsexampleQUERY_KEY_FOR_MESSAGE_LENGTH,
                                               &pcMessage,
                                               &ulMessageLength );

                    if( xJsonStatus == JSONSuccess )
                    {
                        /* Print the given message if the action is "print". */
                        LogInfo( (
                                     "\r\n"
                                     "/*-----------------------------------------------------------*/\r\n"
                                     "\r\n"
                                     "%.*s\r\n"
                                     "\r\n"
                                     "/*-----------------------------------------------------------*/\r\n"
                                     "\r\n", ulMessageLength, pcMessage ) );
                    }

                    break;

                case JOB_ACTION_PUBLISH:
                    LogInfo( ( "Received job contains \"Publish\" action." ) );
                    char * pcTopic = NULL;
                    size_t ulTopicLength = 0U;

                    xJsonStatus = JSON_Search( ( char * ) pxPublishInfo->pPayload,
                                               pxPublishInfo->payloadLength,
                                               jobsexampleQUERY_KEY_FOR_TOPIC,
                                               jobsexampleQUERY_KEY_FOR_TOPIC_LENGTH,
                                               &pcTopic,
                                               &ulTopicLength );

                    /* Search for "topic" key in the Jobs document.*/
                    if( xJsonStatus == JSONSuccess )
                    {
                        const char * pcMessage = NULL;
                        size_t ulMessageLength = 0U;

                        xJsonStatus = JSON_Search( ( char * ) pxPublishInfo->pPayload,
                                                   pxPublishInfo->payloadLength,
                                                   jobsexampleQUERY_KEY_FOR_MESSAGE,
                                                   jobsexampleQUERY_KEY_FOR_MESSAGE_LENGTH,
                                                   &pcMessage,
                                                   &ulMessageLength );

                        /* Search for "message" key in Jobs document.*/
                        if( xJsonStatus == JSONSuccess )
                        {
                            /* Publish to the parsed MQTT topic with the message obtained from
                             * the Jobs document.*/
                            PublishToTopic( &xMqttContext,
                                            pcTopic,
                                            ulTopicLength,
                                            pcMessage,
                                            ulMessageLength );
                        }
                    }

                    break;

                default:
                    configPRINTF( ( "Received Job document with unknown action %.*s.",
                                    uActionLength,
                                    pcAction ) );
                    break;
            }
        }
    }
}

static void prvUpdateJobAcceptedResponeHandler( MQTTPublishInfo_t * pxPublishInfo )
{
    LogInfo( ( "Received message from the UpdateJobExecution API" ) );
}

/*-----------------------------------------------------------*/

/* This is the callback function invoked by the MQTT stack when it receives
 * incoming messages. This function demonstrates how to use the Shadow_MatchTopic
 * function to determine whether the incoming message is a device shadow message
 * or not. If it is, it handles the message depending on the message type.
 */
static void prvEventCallback( MQTTContext_t * pxMqttContext,
                              MQTTPacketInfo_t * pxPacketInfo,
                              MQTTDeserializedInfo_t * pxDeserializedInfo )
{
    uint16_t usPacketIdentifier;

    ( void ) pxMqttContext;

    configASSERT( pxDeserializedInfo != NULL );
    configASSERT( pxMqttContext != NULL );
    configASSERT( pxPacketInfo != NULL );

    usPacketIdentifier = pxDeserializedInfo->packetIdentifier;

    /* Handle incoming publish. The lower 4 bits of the publish packet
     * type is used for the dup, QoS, and retain flags. Hence masking
     * out the lower bits to check if the packet is publish. */
    if( ( pxPacketInfo->type & 0xF0U ) == MQTT_PACKET_TYPE_PUBLISH )
    {
        configASSERT( pxDeserializedInfo->pPublishInfo != NULL );
        JobsTopic_t topicType = JobsMaxTopic;
        char * pcJobId = NULL;
        int16_t usJobIdLength = 0;
        JobsStatus_t xStatus = JobsError;

        LogDebug( ( "Received an incoming publish message: TopicName=%.*s",
                    pxDeserializedInfo->pPublishInfo->topicNameLength,
                    ( const char * ) pxDeserializedInfo->pPublishInfo->pTopicName ) );

        /* Let the Device Shadow library tell us whether this is a device shadow message. */
        xStatus = Jobs_MatchTopic((char*)pxDeserializedInfo->pPublishInfo->pTopicName,
            pxDeserializedInfo->pPublishInfo->topicNameLength,
            democonfigTHING_NAME,
            THING_NAME_LENGTH,
            &topicType,
            &pcJobId,
            &usJobIdLength);

        if( xStatus == JobsSuccess )
        {
            /* Upon successful return, the messageType has been filled in. */
            if( topicType == JobsStartNextSuccess )
            {
                /* Handler function to process payload. */
                prvStartNextJobHandler( pxDeserializedInfo->pPublishInfo );
            }
            else if( topicType == JobsUpdateSuccess )
            {
                /* Handler function to process payload. */
                prvUpdateJobAcceptedResponeHandler( pxDeserializedInfo->pPublishInfo );
            }
            else if( topicType == JobsStartNextFailed )
            {
                LogWarn( ( "Request for next job description rejected: RejectedResponse=%.*s.",
                           pxDeserializedInfo->pPublishInfo->payloadLength,
                           ( const char * ) pxDeserializedInfo->pPublishInfo->pPayload ) );
            }
            else if( topicType == JobsUpdateFailed )
            {
                LogWarn( ( "Request for job update rejected: RejectedResponse=%.*s.",
                           pxDeserializedInfo->pPublishInfo->payloadLength,
                           ( const char * ) pxDeserializedInfo->pPublishInfo->pPayload ) );
            }
            else
            {
                LogWarn( ( "Received an unexpected messages from AWS IoT Jobs service: "
                           "JobsTopicType=%u", topicType ) );
            }
        }
        else if( xStatus == JobsNoMatch )
        {
            LogWarn( ( "Incoming message topic does not belong to IoT Jobs!: topic=%.*s",
                       pxDeserializedInfo->pPublishInfo->topicNameLength,
                       ( const char * ) pxDeserializedInfo->pPublishInfo->pTopicName ) );
        }
        else
        {
            LogError( ( "Failed to parse incoming publish job. Topic=%.*s!",
                        pxDeserializedInfo->pPublishInfo->topicNameLength,
                        ( const char * ) pxDeserializedInfo->pPublishInfo->pTopicName ) );
        }
    }
    else
    {
        vHandleOtherIncomingPacket( pxPacketInfo, usPacketIdentifier );
    }
}

/*-----------------------------------------------------------*/

/**
 * @brief Entry point of the Jobs demo.
 *
 * This main function demonstrates how to use the Jobs library API Device Shadow library.
 *
 * The helper functions this demo uses for MQTT operations have internal
 * loops to process incoming messages. Those are not the focus of this demo
 * and therefor, are placed in a separate file mqtt_demo_utils.c.
 */
int RunJobsDemo( bool awsIotMqttMode,
                 const char * pIdentifier,
                 void * pNetworkServerInfo,
                 void * pNetworkCredentialInfo,
                 const void * pNetworkInterface )
{
    BaseType_t returnStatus = pdPASS;

    /* Remove compiler warnings about unused parameters. */
    ( void ) awsIotMqttMode;
    ( void ) pIdentifier;
    ( void ) pNetworkServerInfo;
    ( void ) pNetworkCredentialInfo;
    ( void ) pNetworkInterface;

    /* Establish an MQTT connection with AWS IoT over a mutually authenticated TLS session */
    returnStatus = EstablishMqttSession( &xMqttContext,
                                         &xNetworkContext,
                                         &xBuffer,
                                         prvEventCallback );

    if( returnStatus == pdFAIL )
    {
        /* Log error to indicate connection failure. */
        LogError( ( "Failed to connect to AWS IoT broker." ) );
    }
    else
    {
        /* Print out a short user guide to the console. The default logging
         * limit of 255 characters can be changed in demo_logging.c, but breaking
         * up the only instance of a 1000+ character string is more practical. */
        LogInfo( ( "\r\n"
                   "/*-----------------------------------------------------------*/\r\n"
                   "\r\n"
                   "The Jobs demo is now ready to accept Jobs.\r\n"
                   "Jobs may be created using the AWS IoT console or AWS CLI.\r\n"
                   "See the following link for more information.\r\n"
                   "\r\n" ) );
        LogInfo( ( "\r"
                   "https://docs.aws.amazon.com/cli/latest/reference/iot/create-job.html\r\n"
                   "\r\n"
                   "This demo expects Job documents to have an \"action\" JSON key.\r\n"
                   "The following actions are currently supported:\r\n" ) );
        LogInfo( ( "\r"
                   " - print          \r\n"
                   "   Logs a message to the local console. The Job document must also contain a \"message\".\r\n"
                   "   For example: { \"action\": \"print\", \"message\": \"Hello world!\"} will cause\r\n"
                   "   \"Hello world!\" to be printed on the console.\r\n" ) );
        LogInfo( ( "\r"
                   " - publish        \r\n"
                   "   Publishes a message to an MQTT topic. The Job document must also contain a \"message\" and \"topic\".\r\n" ) );
        LogInfo( ( "\r"
                   "   For example: { \"action\": \"publish\", \"topic\": \"demo/jobs\", \"message\": \"Hello world!\"} will cause\r\n"
                   "   \"Hello world!\" to be published to the topic \"demo/jobs\".\r\n" ) );
        LogInfo( ( "\r"
                   " - exit           \r\n"
                   "   Exits the demo program. This program will run until { \"action\": \"exit\" } is received.\r\n"
                   "\r\n"
                   "/*-----------------------------------------------------------*/\r\n" ) );

        /* Subscribe to the NextJobExecutionChanged API topic to receive notifications about the next pending job in the queue. */
        returnStatus = SubscribeToTopic( &xMqttContext,
                                         NEXT_JOB_EXECUTION_CHANGED_TOPIC( democonfigTHING_NAME ),
                                         sizeof( NEXT_JOB_EXECUTION_CHANGED_TOPIC( democonfigTHING_NAME ) - 1 ) );
    }

    if( returnStatus == pdPASS )
    {
        /* Publish to AWS IoT Jobs on the DescribeJobExecution API to request the next pending job. */
        returnStatus = PublishToTopic( &xMqttContext,
                                       START_NEXT_JOB_TOPIC( democonfigTHING_NAME ),
                                       sizeof( START_NEXT_JOB_TOPIC( democonfigTHING_NAME ) ) - 1,
                                       JSON_EMPTY_REQUEST,
                                       sizeof( JSON_EMPTY_REQUEST ) - 1 );
    }

    return( ( returnStatus == pdPASS ) ? EXIT_SUCCESS : EXIT_FAILURE );
}

/*-----------------------------------------------------------*/
