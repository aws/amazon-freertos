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
#include "core_mqtt_helpers.h"
#include "core_json.h"

#ifndef THING_NAME

/**
 * @brief Predefined thing name.
 *
 * This is the example predefine thing name and could be compiled in ROM code.
 */
    #define THING_NAME    democonfigCLIENT_IDENTIFIER
#endif

/**
 * @brief The length of #THING_NAME.
 */
#define THING_NAME_LENGTH                    ( ( uint16_t ) ( sizeof( THING_NAME ) - 1 ) )

/**
 * @brief The JSON key of the Job ID.
 *
 * Job documents are in JSON documents received from the AWS IoT Jobs service.
 * All such JSON documents will contain this key, whose value represents the unique
 * identifier of a Job.
 */
#define jobsexampleID_KEY                    "jobId"

/**
 * @brief The length of #jobsexampleID_KEY.
 */
#define jobsexampleID_KEY_LENGTH             ( sizeof( jobsexampleID_KEY ) - 1 )

/**
 * @brief The JSON key of the Job document.
 *
 * Job documents are in JSON documents received from the AWS IoT Jobs service.
 * All such JSON documents will contain this key, whose value is an application-specific
 * Job document.
 */
#define jobsexampleDOC_KEY                   "jobDocument"

/**
 * @brief The length of #jobsexampleDOC_KEY.
 */
#define jobsexampleDOC_KEY_LENGTH            ( sizeof( jobsexampleDOC_KEY ) - 1 )

/**
 * @brief The JSON key whose value represents the action this demo should take.
 *
 * This demo program expects this key to be in the Job document. It is a key
 * specific to this demo.
 */
#define jobsexampleACTION_KEY                "action"

/**
 * @brief The length of #jobsexampleACTION_KEY.
 */
#define jobsexampleACTION_KEY_LENGTH         ( sizeof( jobsexampleACTION_KEY ) - 1 )

/**
 * @brief A message associated with the Job action.
 *
 * This demo program expects this key to be in the Job document if the "action"
 * is either "publish" or "print". It represents the message that should be
 * published or printed, respectively.
 */
#define jobsexampleMESSAGE_KEY               "message"

/**
 * @brief The length of #jobsexampleMESSAGE_KEY.
 */
#define jobsexampleMESSAGE_KEY_LENGTH        ( sizeof( jobsexampleMESSAGE_KEY ) - 1 )

/**
 * @brief An MQTT topic associated with the Job "publish" action.
 *
 * This demo program expects this key to be in the Job document if the "action"
 * is "publish". It represents the MQTT topic on which the message should be
 * published.
 */
#define jobsexampleTOPIC_KEY                 "topic"

/**
 * @brief The length of #jobsexampleTOPIC_KEY.
 */
#define jobsexampleTOPIC_KEY_LENGTH          ( sizeof( jobsexampleTOPIC_KEY ) - 1 )

/**
 * @brief The minimum length of a string in a JSON Job document.
 *
 * At the very least the Job ID must have the quotes that identify it as a JSON
 * string and 1 character for the string itself (the string must not be empty).
 */
#define jobsexampleJSON_STRING_MIN_LENGTH    ( ( size_t ) 3 )

/**
 * @brief Time to wait before exiting demo.
 *
 * The milliseconds to wait before exiting. This is because the MQTT Broker
 * will disconnect us if we are idle too long, and we have disabled keep alive.
 */
#define jobsexampleMS_BEFORE_EXIT            ( 10 * 60 * 1000 )

/**
 * @brief Utility macro to generate the PUBLISH topic string to the
 * DescribePendingJobExecution API of AWS IoT Jobs service for requesting
 * the next pending job information.
 */
#define DESCRIBE_NEXT_JOB_TOPIC( thingName ) \
    ( JOBS_API_PREFIX ""                     \
      THING_NAME "" JOBS_API_BRIDGE          \
      "$next/"JOBS_API_DESCRIBE )

/**
 * @brief Utility macro to generate the subscription topic string for the
 * NextJobExecutionChanged API of AWS IoT Jobs service that is required
 * for getting notification about changes in the next pending job in the queue.
 */
#define NEXT_JOB_EXECUTION_CHANGED_TOPIC( thingName ) \
    ( JOBS_API_PREFIX ""                              \
      THING_NAME "" JOBS_API_BRIDGE                   \
      ""JOBS_API_NEXTJOBCHANGED )

/**
 * @brief Format a JSON status message.
 *
 * @param[in] x one of "IN_PROGRESS", "SUCCEEDED", or "FAILED"
 */
#define makeReport_( x )    "{\"status\":\"" x "\"}"


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
static void prvDescribeNextJobHandler( MQTTPublishInfo_t * pxPublishInfo );

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


static void prvDescribeNextJobHandler( MQTTPublishInfo_t * pxPublishInfo )
{
    LogInfo( ( "Received messages from the DescribeJobExecution API" ) );
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
        const char * pcThingName = NULL;
        uint16_t usThingNameLength = 0U;
        char pcJobId[ JOBS_JOBID_MAX_LENGTH ] = { 0 };
        int16_t usJobIdLength = 0;
        JobsStatus_t xStatus = JobsError;

        LogDebug( ( "Received an incoming publish message: TopicName=%.*s",
                    pxDeserializedInfo->pPublishInfo->topicNameLength,
                    ( const char * ) pxDeserializedInfo->pPublishInfo->pTopicName ) ) );

        /* Let the Device Shadow library tell us whether this is a device shadow message. */
        xStatus == Jobs_MatchTopic( pxDeserializedInfo->pPublishInfo->pTopicName,
                                    pxDeserializedInfo->pPublishInfo->topicNameLength,
                                    &messageType,
                                    &pcThingName,
                                    &usThingNameLength );

        if( xStatus == JobsSuccess )
        {
            /* Make sure that we have received messages from the AWS IoT Jobs services for the same
             * thing name that we requested. */
            configASSERT( 0 == strncmp( pcThingName, usThingNameLength ) )

            /* Upon successful return, the messageType has been filled in. */
            if( topicType == JobsDescribeSuccess )
            {
                /* Handler function to process payload. */
                prvDescribeNextJobHandler( pxDeserializedInfo->pPublishInfo );
            }
            else if( topicType == JobsUpdateSuccess )
            {
                /* Handler function to process payload. */
                prvUpdateJobAcceptedResponeHandler( pxDeserializedInfo->pPublishInfo );
            }
            else if( topicType == JobsDescribeFailed )
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
            LogError( ( "Failed to parse incoming publish job. Topic=%s!",
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

    /* A buffer containing the update document. It has static duration to prevent
     * it from being placed on the call stack. */
    static char pcUpdateDocument[ SHADOW_REPORTED_JSON_LENGTH + 1 ] = { 0 };

    /* Remove compiler warnings about unused parameters. */
    ( void ) awsIotMqttMode;
    ( void ) pIdentifier;
    ( void ) pNetworkServerInfo;
    ( void ) pNetworkCredentialInfo;
    ( void ) pNetworkInterface;

    /* Establish an MQTT connection with AWS IoT over a mutually authenticated TLS session */
    demoStatus = EstablishMqttSession( &xMqttContext,
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
                                         NEXT_JOB_EXECUTION_CHANGED_TOPIC( THING_NAME ),
                                         sizeof( NEXT_JOB_EXECUTION_CHANGED_TOPIC( THING_NAME ) - 1 ) );
    }

    if( returnStatus == pdPASS )
    {
        /* Publish to AWS IoT Jobs on the DescribeJobExecution API to request the next pending job. */
        returnStatus = PublishToTopic( &xMqttContext,
                                       DESCRIBE_NEXT_JOB_TOPIC( THING_NAME ),
                                       sizeof( DESCRIBE_NEXT_JOB_TOPIC( THING_NAME ) ) - 1,
                                       pcUpdateDocument,
                                       0U );

        /* This demo uses a constant #THING_NAME known at compile time therefore we can use macros to
         * assemble shadow topic strings.
         * If the thing name is known at run time, then we could use the API #Shadow_GetTopicString to
         * assemble shadow topic strings, here is the example for /update/delta:
         *
         * For /update/delta:
         *
         * #define SHADOW_TOPIC_MAX_LENGTH  (256U)
         *
         * ShadowStatus_t shadowStatus = SHADOW_STATUS_SUCCESS;
         * char cTopicBuffer[ SHADOW_TOPIC_MAX_LENGTH ] = { 0 };
         * uint16_t usBufferSize = SHADOW_TOPIC_MAX_LENGTH;
         * uint16_t usOutLength = 0;
         * const char * pcThingName = "TestThingName";
         * uint16_t usThingNameLength  = ( sizeof( pcThingName ) - 1U );
         *
         * shadowStatus = Shadow_GetTopicString( SHADOW_TOPIC_STRING_TYPE_UPDATE_DELTA,
         *                                       pcThingName,
         *                                       usThingNameLength,
         *                                       & ( cTopicBuffer[ 0 ] ),
         *                                       usBufferSize,
         *                                       & usOutLength );
         */

        /* Then we publish a desired state to the /update topic. Since we've deleted
         * the device shadow at the beginning of the demo, this will cause a delta message
         * to be published, which we have subscribed to.
         * In many real applications, the desired state is not published by
         * the device itself. But for the purpose of making this demo self-contained,
         * we publish one here so that we can receive a delta message later.
         */
        if( returnStatus == EXIT_SUCCESS )
        {
            /* Desired power on state . */
            LogInfo( ( "Send desired power state with 1." ) );

            ( void ) memset( pcUpdateDocument,
                             0x00,
                             sizeof( pcUpdateDocument ) );

            snprintf( pcUpdateDocument,
                      SHADOW_DESIRED_JSON_LENGTH + 1,
                      SHADOW_DESIRED_JSON,
                      ( int ) 1,
                      ( long unsigned ) ( xTaskGetTickCount() % 1000000 ) );

            returnStatus = PublishToTopic( SHADOW_TOPIC_STRING_UPDATE( THING_NAME ),
                                           SHADOW_TOPIC_LENGTH_UPDATE( THING_NAME_LENGTH ),
                                           pcUpdateDocument,
                                           ( SHADOW_DESIRED_JSON_LENGTH + 1 ) );
        }

        if( returnStatus == EXIT_SUCCESS )
        {
            /* Note that PublishToTopic already called MQTT_ProcessLoop,
             * therefore responses may have been received and the prvEventCallback
             * may have been called, which may have changed the stateChanged flag.
             * Check if the state change flag has been modified or not. If it's modified,
             * then we publish reported state to update topic.
             */
            if( stateChanged == true )
            {
                /* Report the latest power state back to device shadow. */
                LogInfo( ( "Report to the state change: %d", ulCurrentPowerOnState ) );
                ( void ) memset( pcUpdateDocument,
                                 0x00,
                                 sizeof( pcUpdateDocument ) );

                /* Keep the client token in global variable used to compare if
                 * the same token in /update/accepted. */
                ulClientToken = ( xTaskGetTickCount() % 1000000 );

                snprintf( pcUpdateDocument,
                          SHADOW_REPORTED_JSON_LENGTH + 1,
                          SHADOW_REPORTED_JSON,
                          ( int ) ulCurrentPowerOnState,
                          ( long unsigned ) ulClientToken );

                returnStatus = PublishToTopic( SHADOW_TOPIC_STRING_UPDATE( THING_NAME ),
                                               SHADOW_TOPIC_LENGTH_UPDATE( THING_NAME_LENGTH ),
                                               pcUpdateDocument,
                                               ( SHADOW_DESIRED_JSON_LENGTH + 1 ) );
            }
            else
            {
                LogInfo( ( "No change from /update/delta, unsubscribe all shadow topics and disconnect from MQTT.\r\n" ) );
            }
        }

        if( returnStatus == EXIT_SUCCESS )
        {
            LogInfo( ( "Start to unsubscribe shadow topics and disconnect from MQTT. \r\n" ) );

            returnStatus = UnsubscribeFromTopic( SHADOW_TOPIC_STRING_UPDATE_DELTA( THING_NAME ),
                                                 SHADOW_TOPIC_LENGTH_UPDATE_DELTA( THING_NAME_LENGTH ) );

            if( returnStatus != EXIT_SUCCESS )
            {
                LogError( ( "Failed to unsubscribe the topic %s",
                            SHADOW_TOPIC_STRING_UPDATE_DELTA( THING_NAME ) ) );
            }
        }

        if( returnStatus == EXIT_SUCCESS )
        {
            returnStatus = UnsubscribeFromTopic( SHADOW_TOPIC_STRING_UPDATE_ACCEPTED( THING_NAME ),
                                                 SHADOW_TOPIC_LENGTH_UPDATE_ACCEPTED( THING_NAME_LENGTH ) );

            if( returnStatus != EXIT_SUCCESS )
            {
                LogError( ( "Failed to unsubscribe the topic %s",
                            SHADOW_TOPIC_STRING_UPDATE_ACCEPTED( THING_NAME ) ) );
            }
        }

        if( returnStatus == EXIT_SUCCESS )
        {
            returnStatus = UnsubscribeFromTopic( SHADOW_TOPIC_STRING_UPDATE_REJECTED( THING_NAME ),
                                                 SHADOW_TOPIC_LENGTH_UPDATE_REJECTED( THING_NAME_LENGTH ) );

            if( returnStatus != EXIT_SUCCESS )
            {
                LogError( ( "Failed to unsubscribe the topic %s",
                            SHADOW_TOPIC_STRING_UPDATE_REJECTED( THING_NAME ) ) );
            }
        }

        /* The MQTT session is always disconnected, even there were prior failures. */
        returnStatus = DisconnectMqttSession();

        /* This demo performs only Device Shadow operations. If matching the Shadow
         * MQTT topic fails or there are failure in parsing the received JSON document,
         * then this demo was not successful. */
        if( ( lUpdateAcceptedReturn != EXIT_SUCCESS ) || ( lUpdateDeltaReturn != EXIT_SUCCESS ) )
        {
            LogError( ( "Callback function failed." ) );
            returnStatus = pdFAIL;
        }
    }

    return( ( demoStatus == pdPASS ) ? EXIT_SUCCESS : EXIT_FAILURE );
}

/*-----------------------------------------------------------*/
