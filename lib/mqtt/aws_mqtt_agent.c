/*
 * Amazon FreeRTOS MQTT Agent V1.1.4
 * Copyright (C) 2017 Amazon.com, Inc. or its affiliates.  All Rights Reserved.
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
 * @file aws_mqtt_agent.c
 * @brief MQTT Agent implementation. Provides backwards compatibility between
 * MQTT v4 and MQTT v1.
 */

/* MQTT v4 config file. */
#ifdef IOT_CONFIG_FILE
    #include IOT_CONFIG_FILE
#endif

/* Standard includes. */
#include <string.h>

/* FreeRTOS includes. */
#include "FreeRTOS.h"
#include "FreeRTOS_POSIX.h"

/* MQTT v1 includes. */
#include "aws_mqtt_agent.h"
#include "aws_mqtt_agent_config.h"
#include "aws_mqtt_agent_config_defaults.h"

/* MQTT v4 include. */
#include "iot_mqtt.h"

/* Platform network include. */
#include "platform/iot_network_afr.h"

/*-----------------------------------------------------------*/

/**
 * @brief Converts FreeRTOS ticks to milliseconds.
 */
#define mqttTICKS_TO_MS( xTicks )    ( xTicks * 1000 / configTICK_RATE_HZ )

/*-----------------------------------------------------------*/

/**
 * @brief Stores data to convert between the MQTT v1 subscription callback
 * and the MQTT v4 subscription callback.
 */
#if ( mqttconfigENABLE_SUBSCRIPTION_MANAGEMENT == 1 )
    typedef struct MQTTCallback
    {
        BaseType_t xInUse;                                                     /**< Whether this instance is in-use. */
        MQTTPublishCallback_t xFunction;                                       /**< MQTT v1 callback function. */
        void * pvParameter;                                                    /**< Parameter to xFunction. */

#else
    static const char cUserName[] = "";
#endif /* if ( mqttconfigENABLE_METRICS == 1 ) */

/**
 * @brief Stores data on an active MQTT connection.
 */
typedef struct MQTTConnection
{
    BaseType_t xNetworkConnectionCreated;         /**< Tracks if the network connection has been created. */
    IotNetworkConnectionAfr_t xNetworkConnection; /**< Network connection handle. */
    IotMqttConnection_t xMQTTConnection;          /**< MQTT v4 connection handle. */
    MQTTAgentCallback_t pxCallback;               /**< MQTT v1 global callback. */
    void * pvUserData;                            /**< Parameter to pxCallback. */
    StaticSemaphore_t xConnectionMutex;           /**< Protects from concurrent accesses. */
    #if ( mqttconfigENABLE_SUBSCRIPTION_MANAGEMENT == 1 )
        MQTTCallback_t xCallbacks                 /**< Conversion table of MQTT v1 to MQTT v4 subscription callbacks. */
        [ mqttconfigSUBSCRIPTION_MANAGER_MAX_SUBSCRIPTIONS ];
    #endif
} MQTTConnection_t;

/*-----------------------------------------------------------*/

/**
 * @brief Convert an MQTT v4 return code to an MQTT v1 return code.
 *
 * @param[in] xMqttStatus The MQTT v4 return code.
 *
 * @return An equivalent MQTT v1 return code.
 */
static inline MQTTAgentReturnCode_t prvConvertReturnCode( IotMqttError_t xMqttStatus );

/**
 * @brief The callback for incoming PUBLISH messages.
 *
 * @param[in] pvParameter The MQTT connection that received the PUBLISH.
 * @param[in] xPublish Information on the incoming PUBLISH.
 */
static void prvPublishCallbackWrapper( void * pvParameter,
                                       IotMqttCallbackParam_t * const xPublish );

/**
 * @brief The set receive callback function set in the network interface.
 *
 * @see #IotNetworkAfr_SetReceiveCallback
 */
static IotNetworkError_t prvSetReceiveCallbackWrapper( void * pConnection,
                                                       IotNetworkReceiveCallback_t receiveCallback,
                                                       void * pContext );

/**
 * @brief The send function set in the network interface.
 *
 * @see #IotNetworkAfr_Send
 */
static size_t prvSendWrapper( void * pConnection,
                              const uint8_t * pMessage,
                              size_t messageLength );

/**
 * @brief The network close function set in network interface.
 *
 * Wraps the network stack's close function and calls an MQTT v1 disconnect
 * callback.
 * @param[in] pvDisconnectHandle The MQTT connection being disconnected.
 *
 * @return Always returns #IOT_NETWORK_SUCCESS.
 */
static IotNetworkError_t prvCloseWrapper( void * pvDisconnectHandle );

#if ( mqttconfigENABLE_SUBSCRIPTION_MANAGEMENT == 1 )

/**
 * @brief Store an MQTT v1 callback in the conversion table.
 *
 * @param[in] pxConnection Where to store the callback.
 * @param[in] pcTopicFilter Topic filter to store.
 * @param[in] usTopicFilterLength Length of pcTopicFilter.
 * @param[in] xCallback MQTT v1 callback to store.
 * @param[in] pvParameter Parameter to xCallback.
 *
 * @return pdPASS if the callback was successfully stored; pdFAIL otherwise.
 */
    static BaseType_t prvStoreCallback( MQTTConnection_t * const pxConnection,
                                        const char * const pcTopicFilter,
                                        uint16_t usTopicFilterLength,
                                        MQTTPublishCallback_t xCallback,
                                        void * pvParameter );

/**
 * @brief Search the callback conversion table for the given topic filter.
 *
 * @param[in] pxConnection The connection containing the conversion table.
 * @param[in] pcTopicFilter The topic filter to search for.
 * @param[in] usTopicFilterLength The length of pcTopicFilter.
 *
 * @return A pointer to the callback entry if found; NULL otherwise.
 * @note This function should be called with pxConnection->xConnectionMutex
 * locked.
 */
    static MQTTCallback_t * prvFindCallback( MQTTConnection_t * const pxConnection,
                                             const char * const pcTopicFilter,
                                             uint16_t usTopicFilterLength );

/**
 * @brief Remove a topic filter from the callback conversion table.
 *
 * @param[in] pxConnection The connection containing the conversion table.
 * @param[in] pcTopicFilter The topic filter to remove.
 * @param[in] usTopicFilterLength The length of pcTopic.
 */
    static void prvRemoveCallback( MQTTConnection_t * const pxConnection,
                                   const char * const pcTopicFilter,
                                   uint16_t usTopicFilterLength );
#endif /* if ( mqttconfigENABLE_SUBSCRIPTION_MANAGEMENT == 1 ) */

/*-----------------------------------------------------------*/

/**
 * @brief The number of available MQTT brokers, controlled by the constant
 * mqttconfigMAX_BROKERS;
 */
static UBaseType_t uxAvailableBrokers = mqttconfigMAX_BROKERS;

/**
 * @brief A network interface to use.
 *
 * Wraps the network interface's functions. The create and destroy functions
 * are not used.
 */
static const IotNetworkInterface_t xNetworkInterface =
{
    .create             = NULL,
    .setReceiveCallback = prvSetReceiveCallbackWrapper,
    .send               = prvSendWrapper,
    .receive            = IotNetworkAfr_Receive,
    .close              = prvCloseWrapper,
    .destroy            = NULL
};

/*-----------------------------------------------------------*/

static inline MQTTAgentReturnCode_t prvConvertReturnCode( IotMqttError_t xMqttStatus )
{
    MQTTAgentReturnCode_t xStatus = eMQTTAgentSuccess;

    switch( xMqttStatus )
    {
        case IOT_MQTT_SUCCESS:
        case IOT_MQTT_STATUS_PENDING:
            xStatus = eMQTTAgentSuccess;
            break;

        case IOT_MQTT_TIMEOUT:
            xStatus = eMQTTAgentTimeout;
            break;

        default:
            xStatus = eMQTTAgentFailure;
            break;
    }

    return xStatus;
}

/*-----------------------------------------------------------*/

static void prvPublishCallbackWrapper( void * pvParameter,
                                       IotMqttCallbackParam_t * const xPublish )
{
    BaseType_t xStatus = pdPASS;
    size_t xURLLength;
    MQTTBrokerConnection_t * pxConnection = &( xMQTTConnections[ pxEventData->uxBrokerNumber ] );
    char * ppcAlpns[] = { socketsAWS_IOT_ALPN_MQTT };
    TickType_t xMqttTimeout;

    /* Should not get here if the socket used to communicate with the
     * broker is already connected. */
    configASSERT( pxConnection->xSocket == SOCKETS_INVALID_SOCKET );

    /* Calculate the length of the supplied URL. */
    xURLLength = strlen( pxEventData->u.pxConnectParams->pcURL );

    /* Calculate the size of the MQTT buffer that must be allocated. */
    if( xStatus == pdPASS )
    {
        xBufferSize = xPublish->message.info.topicNameLength +
                      xPublish->message.info.payloadLength;

            /* Establish the connection. */
            if( xStatus == pdPASS )
            {
                if( SOCKETS_Connect( pxConnection->xSocket, &xMQTTServerAddress, sizeof( xMQTTServerAddress ) ) != SOCKETS_ERROR_NONE )
                {
                    xStatus = pdFAIL;
                }
            }

            if( xStatus == pdPASS )
            {
                /* Do not block now onwards. */
                ( void ) SOCKETS_SetSockOpt( pxConnection->xSocket,
                                             0 /* Unused. */,
                                             SOCKETS_SO_NONBLOCK,
                                             NULL /* Unused. */,
                                             0 /* Unused. */ );

                /* Set the Send Timeout of Socket to mqttconfigTCP_SEND_TIMEOUT_MS to block on sends. */

                xMqttTimeout = pdMS_TO_TICKS( mqttconfigTCP_SEND_TIMEOUT_MS );

                if( SOCKETS_SetSockOpt( pxConnection->xSocket,
                                        0,
                                        SOCKETS_SO_SNDTIMEO,
                                        &xMqttTimeout,
                                        sizeof( TickType_t ) ) != SOCKETS_ERROR_NONE )
                {
                    xStatus = pdFAIL;
                }
            }
            else
            {
                /* Connection Failed. */
                prvGracefulSocketClose( pxConnection );
            }
        }
        else
        {
            mqttconfigDEBUG_LOG( ( "Incoming PUBLISH message and topic name length too large.\r\n" ) );
            xStatus = pdFAIL;
        }
    }

    /* Allocate an MQTT buffer for the callback. */
    if( xStatus == pdPASS )
    {
        pucMqttBuffer = pvPortMalloc( xBufferSize );

        if( pucMqttBuffer == NULL )
        {
            mqttconfigDEBUG_LOG( ( "Failed to allocate memory for MQTT buffer.\r\n" ) );
            xStatus = pdFAIL;
        }
        else
        {
            /* Copy the topic name and payload. The topic name and payload must be
             * copied in case the user decides to take ownership of the MQTT buffer.
             * The original buffer containing the MQTT topic name and payload may
             * contain further unprocessed packets and must remain property of the
             * MQTT library. Therefore, the topic name and payload are copied into
             * another buffer for the user. */
            ( void ) memcpy( pucMqttBuffer,
                             xPublish->message.info.pTopicName,
                             xPublish->message.info.topicNameLength );
            ( void ) memcpy( pucMqttBuffer + xPublish->message.info.topicNameLength,
                             xPublish->message.info.pPayload,
                             xPublish->message.info.payloadLength );

            /* Set the members of the callback parameter. */
            xPublishData.xMQTTEvent = eMQTTAgentPublish;
            xPublishData.u.xPublishData.pucTopic = pucMqttBuffer;
            xPublishData.u.xPublishData.usTopicLength = xPublish->message.info.topicNameLength;
            xPublishData.u.xPublishData.pvData = pucMqttBuffer + xPublish->message.info.topicNameLength;
            xPublishData.u.xPublishData.ulDataLength = ( uint32_t ) xPublish->message.info.payloadLength;
            xPublishData.u.xPublishData.xQos = ( MQTTQoS_t ) xPublish->message.info.qos;
            xPublishData.u.xPublishData.xBuffer = pucMqttBuffer;
        }
    }

    if( xStatus == pdPASS )
    {
        #if ( mqttconfigENABLE_SUBSCRIPTION_MANAGEMENT == 1 )
            /* When subscription management is enabled, search for a matching subscription. */
            MQTTCallback_t * pxCallbackEntry = prvFindCallback( pxConnection,
                                                                xPublish->message.pTopicFilter,
                                                                xPublish->message.topicFilterLength );

            /* Check if a matching MQTT v1 subscription was found. */
            if( pxCallbackEntry != NULL )
            {
                /* Invoke the topic-specific callback if it exists. */
                if( pxCallbackEntry->xFunction != NULL )
                {
                    xCallbackReturn = pxCallbackEntry->xFunction( pxCallbackEntry->pvParameter,
                                                                  &( xPublishData.u.xPublishData ) );
                }
                else
                {
                    /* Otherwise, invoke the global callback. */
                    if( pxConnection->pxCallback != NULL )
                    {
                        xCallbackReturn = ( BaseType_t ) pxConnection->pxCallback( pxConnection->pvUserData,
                                                                                   &xPublishData );
                    }
                }
            }
        #else /* if ( mqttconfigENABLE_SUBSCRIPTION_MANAGEMENT == 1 ) */

            /* When subscription management is disabled, invoke the global callback
             * if one exists. */

            /* When subscription management is disabled, the topic filter must be "#". */
            mqttconfigASSERT( *( xPublish.message.pTopicFilter ) == '#' );
            mqttconfigASSERT( xPublish.message.topicFilterLength == 1 );

            if( pxConnection->pxCallback != NULL )
            {
                xCallbackReturn = ( BaseType_t ) pxConnection->pxCallback( pxConnection->pvUserData,
                                                                           &xPublishData );
            }
        #endif /* if ( mqttconfigENABLE_SUBSCRIPTION_MANAGEMENT == 1 ) */
    }

    /* Free the MQTT buffer if the user did not take ownership of it. */
    if( ( xCallbackReturn == eMQTTFalse ) && ( pucMqttBuffer != NULL ) )
    {
        vPortFree( pucMqttBuffer );
    }
}

/*-----------------------------------------------------------*/

static IotNetworkError_t prvSetReceiveCallbackWrapper( void * pConnection,
                                                       IotNetworkReceiveCallback_t receiveCallback,
                                                       void * pContext )
{
    MQTTConnection_t * pxConnection = ( MQTTConnection_t * ) pConnection;

    return IotNetworkAfr_SetReceiveCallback( &( pxConnection->xNetworkConnection ),
                                             receiveCallback,
                                             pContext );
}

/*-----------------------------------------------------------*/

static size_t prvSendWrapper( void * pConnection,
                              const uint8_t * pMessage,
                              size_t messageLength )
{
    MQTTConnection_t * pxConnection = ( MQTTConnection_t * ) pConnection;

    return IotNetworkAfr_Send( &( pxConnection->xNetworkConnection ),
                               pMessage,
                               messageLength );
}

/*-----------------------------------------------------------*/

static IotNetworkError_t prvCloseWrapper( void * pvDisconnectHandle )
{
    MQTTConnection_t * pxConnection = ( MQTTConnection_t * ) pvDisconnectHandle;
    IotNetworkError_t xNetworkStatus = IOT_NETWORK_SUCCESS;
    MQTTAgentCallbackParams_t xCallbackParams = { 0 };

    /* Call the network stack's disconnect function. */
    xNetworkStatus = IotNetworkAfr_Close( &( pxConnection->xNetworkConnection ) );

    /* Call the MQTT v1 global callback with a disconnect event. */
    if( pxConnection->pxCallback != NULL )
    {
        xCallbackParams.xMQTTEvent = eMQTTAgentDisconnect;
        pxConnection->pxCallback( pxConnection->pvUserData, &xCallbackParams );
    }

    return xNetworkStatus;
}

/*-----------------------------------------------------------*/

#if ( mqttconfigENABLE_SUBSCRIPTION_MANAGEMENT == 1 )
    static BaseType_t prvStoreCallback( MQTTConnection_t * const pxConnection,
                                        const char * const pcTopicFilter,
                                        uint16_t usTopicFilterLength,
                                        MQTTPublishCallback_t xCallback,
                                        void * pvParameter )
    {
        MQTTCallback_t * pxCallback = NULL;
        BaseType_t xStatus = pdFAIL, i = 0;

        /* Prevent other tasks from modifying stored callbacks while this function
         * runs. */
        if( xSemaphoreTake( ( QueueHandle_t ) &( pxConnection->xConnectionMutex ),
                            portMAX_DELAY ) == pdTRUE )
        {
            /* Check if the topic filter already has an entry. */
            pxCallback = prvFindCallback( pxConnection, pcTopicFilter, usTopicFilterLength );

            if( pxCallback == NULL )
            {
                /* If no entry was found, find a free entry. */
                for( i = 0; i < mqttconfigSUBSCRIPTION_MANAGER_MAX_SUBSCRIPTIONS; i++ )
                {
                    if( pxConnection->xCallbacks[ i ].xInUse == pdFALSE )
                    {
                        pxConnection->xCallbacks[ i ].xInUse = pdTRUE;
                        pxCallback = &( pxConnection->xCallbacks[ i ] );
                        break;
                    }
                }
            }

            /* Set the members of the callback entry. */
            if( i < mqttconfigSUBSCRIPTION_MANAGER_MAX_SUBSCRIPTIONS )
            {
                pxCallback->pvParameter = pvParameter;
                pxCallback->usTopicFilterLength = usTopicFilterLength;
                pxCallback->xFunction = xCallback;
                ( void ) strncpy( pxCallback->pcTopicFilter, pcTopicFilter, usTopicFilterLength );
                xStatus = pdPASS;
            }

            ( void ) xSemaphoreGive( ( QueueHandle_t ) &( pxConnection->xConnectionMutex ) );
        }

        return xStatus;
    }

/*-----------------------------------------------------------*/

    static MQTTCallback_t * prvFindCallback( MQTTConnection_t * const pxConnection,
                                             const char * const pcTopicFilter,
                                             uint16_t usTopicFilterLength )
    {
        BaseType_t i = 0;
        MQTTCallback_t * pxResult = NULL;

        /* Search the callback conversion table for the topic filter. */
        for( i = 0; i < mqttconfigSUBSCRIPTION_MANAGER_MAX_SUBSCRIPTIONS; i++ )
        {
            #if ( mqttconfigENABLE_METRICS == 1 )
                mqttconfigDEBUG_LOG( ( "Anonymous metrics will be collected. Recompile with"
                                       "mqttconfigENABLE_METRICS set to 0 to disable.\r\n" ) );
            #endif

            /* Setup connect parameters and call the Core library connect function. */
            xConnectParams.pucClientId = pxEventData->u.pxConnectParams->pucClientId;
            xConnectParams.usClientIdLength = pxEventData->u.pxConnectParams->usClientIdLength;
            xConnectParams.pucUserName = ( const uint8_t * ) cUserName;
            xConnectParams.usUserNameLength = usUserNameLength;
            xConnectParams.usKeepAliveIntervalSeconds = mqttconfigKEEP_ALIVE_INTERVAL_SECONDS;
            xConnectParams.ulKeepAliveActualIntervalTicks = mqttconfigKEEP_ALIVE_ACTUAL_INTERVAL_TICKS;
            xConnectParams.ulPingRequestTimeoutTicks = mqttconfigKEEP_ALIVE_TIMEOUT_TICKS;
            xConnectParams.usPacketIdentifier = ( uint16_t ) ( mqttMESSAGE_IDENTIFIER_EXTRACT( pxEventData->xNotificationData.ulMessageIdentifier ) );
            xConnectParams.ulTimeoutTicks = pxEventData->xTicksToWait;

            if( MQTT_Connect( &( pxConnection->xMQTTContext ), &( xConnectParams ) ) != eMQTTSuccess )
            {
                pxResult = &( pxConnection->xCallbacks[ i ] );
                break;
            }
        }

        return pxResult;
    }

/*-----------------------------------------------------------*/

    static void prvRemoveCallback( MQTTConnection_t * const pxConnection,
                                   const char * const pcTopicFilter,
                                   uint16_t usTopicFilterLength )
    {
        MQTTCallback_t * pxCallback = NULL;

        /* Prevent other tasks from modifying stored callbacks while this function
         * runs. */
        if( xSemaphoreTake( ( QueueHandle_t ) &( pxConnection->xConnectionMutex ),
                            portMAX_DELAY ) == pdTRUE )
        {
            /* Find the given topic filter. */
            pxCallback = prvFindCallback( pxConnection, pcTopicFilter, usTopicFilterLength );

            if( pxCallback != NULL )
            {
                /* Clear the callback entry. */
                mqttconfigASSERT( pxCallback->xInUse == pdTRUE );
                ( void ) memset( pxCallback, 0x00, sizeof( MQTTCallback_t ) );
            }

            ( void ) xSemaphoreGive( ( QueueHandle_t ) &( pxConnection->xConnectionMutex ) );
        }
    }
#endif /* if ( mqttconfigENABLE_SUBSCRIPTION_MANAGEMENT == 1 ) */

/*-----------------------------------------------------------*/

IotMqttConnection_t MQTT_AGENT_Getv4Connection( MQTTAgentHandle_t xMQTTHandle )
{
    MQTTConnection_t * pxConnection = ( MQTTConnection_t * ) xMQTTHandle;

    return pxConnection->xMQTTConnection;
}

/*-----------------------------------------------------------*/

BaseType_t MQTT_AGENT_Init( void )
{
    BaseType_t xStatus = pdFAIL;
    MQTTNotificationData_t * pxNotificationData;
    MQTTSubscribeParams_t xSubscribeParams;
    MQTTBrokerConnection_t * pxConnection = &( xMQTTConnections[ pxEventData->uxBrokerNumber ] );

    /* Store notification data. */
    pxNotificationData = prvStoreNotificationData( pxConnection, pxEventData );

    /* If a free buffer was not available to store the  notification data
     * (i.e. mqttconfigMAX_PARALLEL_OPS tasks are already in progress), fail
     * immediately. */
    if( pxNotificationData != NULL )
    {
        /* Setup subscribe parameters and call the Core library subscribe function. */
        xSubscribeParams.pucTopic = pxEventData->u.pxSubscribeParams->pucTopic;
        xSubscribeParams.usTopicLength = pxEventData->u.pxSubscribeParams->usTopicLength;
        xSubscribeParams.xQos = pxEventData->u.pxSubscribeParams->xQoS;
        xSubscribeParams.usPacketIdentifier = ( uint16_t ) ( mqttMESSAGE_IDENTIFIER_EXTRACT( pxEventData->xNotificationData.ulMessageIdentifier ) );
        xSubscribeParams.ulTimeoutTicks = pxEventData->xTicksToWait;
        #if ( mqttconfigENABLE_SUBSCRIPTION_MANAGEMENT == 1 )
            xSubscribeParams.pvPublishCallbackContext = pxEventData->u.pxSubscribeParams->pvPublishCallbackContext;
            xSubscribeParams.pxPublishCallback = pxEventData->u.pxSubscribeParams->pxPublishCallback;
        #endif /* mqttconfigENABLE_SUBSCRIPTION_MANAGEMENT */

    /* Call the initialization function of MQTT v4. */
    if( IotMqtt_Init() == IOT_MQTT_SUCCESS )
    {
        xStatus = pdTRUE;
    }

    return xStatus;
}

/*-----------------------------------------------------------*/

MQTTAgentReturnCode_t MQTT_AGENT_Create( MQTTAgentHandle_t * const pxMQTTHandle )
{
    BaseType_t xStatus = pdFAIL;
    MQTTNotificationData_t * pxNotificationData;
    MQTTUnsubscribeParams_t xUnsubscribeParams;
    MQTTBrokerConnection_t * pxConnection = &( xMQTTConnections[ pxEventData->uxBrokerNumber ] );

    /* Store notification data. */
    pxNotificationData = prvStoreNotificationData( pxConnection, pxEventData );

    /* If a free buffer was not available to store the  notification data
     * (i.e. mqttconfigMAX_PARALLEL_OPS tasks are already in progress), fail
     * immediately. */
    if( pxNotificationData != NULL )
    {
        if( uxAvailableBrokers == 0 )
        {
            xStatus = eMQTTAgentFailure;
        }
        else
        {
            uxAvailableBrokers--;
            mqttconfigASSERT( uxAvailableBrokers <= mqttconfigMAX_BROKERS );
        }
    }
    taskEXIT_CRITICAL();

    /* Allocate memory for an MQTT connection. */
    if( xStatus == eMQTTAgentSuccess )
    {
        pxNewConnection = pvPortMalloc( sizeof( MQTTConnection_t ) );

        if( pxNewConnection == NULL )
        {
            xStatus = eMQTTAgentFailure;

            taskENTER_CRITICAL();
            {
                uxAvailableBrokers++;
                mqttconfigASSERT( uxAvailableBrokers <= mqttconfigMAX_BROKERS );
            }
            taskEXIT_CRITICAL();
        }
        else
        {
            ( void ) memset( pxNewConnection, 0x00, sizeof( MQTTConnection_t ) );
            pxNewConnection->xMQTTConnection = IOT_MQTT_CONNECTION_INITIALIZER;
        }
    }

    /* Create the connection mutex and set the output parameter. */
    if( xStatus == eMQTTAgentSuccess )
    {
        ( void ) xSemaphoreCreateMutexStatic( &( pxNewConnection->xConnectionMutex ) );
        *pxMQTTHandle = ( MQTTAgentHandle_t ) pxNewConnection;
    }

    return xStatus;
}

/*-----------------------------------------------------------*/

MQTTAgentReturnCode_t MQTT_AGENT_Delete( MQTTAgentHandle_t xMQTTHandle )
{
    MQTTConnection_t * pxConnection = ( MQTTConnection_t * ) xMQTTHandle;

    /* Clean up any allocated MQTT or network resources. */
    if( pxConnection->xMQTTConnection != IOT_MQTT_CONNECTION_INITIALIZER )
    {
        IotMqtt_Disconnect( pxConnection->xMQTTConnection, true );
        pxConnection->xMQTTConnection = IOT_MQTT_CONNECTION_INITIALIZER;
    }

    if( pxConnection->xNetworkConnectionCreated == pdTRUE )
    {
        IotNetworkAfr_Destroy( &( pxConnection->xNetworkConnection ) );

        memset( &( pxConnection->xNetworkConnection ), 0x00, sizeof( IotNetworkConnectionAfr_t ) );
        pxConnection->xNetworkConnectionCreated = pdFALSE;
    }

    /* Free memory used by the MQTT connection. */
    vPortFree( pxConnection );

    /* Increment the number of available brokers. */
    taskENTER_CRITICAL();
    {
        uxAvailableBrokers++;
        mqttconfigASSERT( uxAvailableBrokers <= mqttconfigMAX_BROKERS );
    }
    taskEXIT_CRITICAL();

    return eMQTTAgentSuccess;
}

/*-----------------------------------------------------------*/

MQTTAgentReturnCode_t MQTT_AGENT_Connect( MQTTAgentHandle_t xMQTTHandle,
                                          const MQTTAgentConnectParams_t * const pxConnectParams,
                                          TickType_t xTimeoutTicks )
{
    MQTTAgentReturnCode_t xStatus = eMQTTAgentSuccess;
    IotMqttError_t xMqttStatus = IOT_MQTT_STATUS_PENDING;
    MQTTConnection_t * pxConnection = ( MQTTConnection_t * ) xMQTTHandle;
    IotNetworkServerInfoAfr_t xServerInfo = AWS_IOT_NETWORK_SERVER_INFO_AFR_INITIALIZER;
    IotNetworkCredentialsAfr_t xCredentials = AWS_IOT_NETWORK_CREDENTIALS_AFR_INITIALIZER, * pxCredentials = NULL;
    IotMqttNetworkInfo_t xNetworkInfo = IOT_MQTT_NETWORK_INFO_INITIALIZER;
    IotMqttConnectInfo_t xMqttConnectInfo = IOT_MQTT_CONNECT_INFO_INITIALIZER;

    /* Copy the global callback and parameter. */
    pxConnection->pxCallback = pxConnectParams->pxCallback;
    pxConnection->pvUserData = pxConnectParams->pvUserData;

    /* Set the TLS info for a secured connection. */
    if( ( pxConnectParams->xSecuredConnection == pdTRUE ) ||
        ( ( pxConnectParams->xFlags & mqttagentREQUIRE_TLS ) == mqttagentREQUIRE_TLS ) )
    {
        pxCredentials = &xCredentials;

        /* Set the server certificate. Other credentials are set by the initializer. */
        xCredentials.pRootCa = pxConnectParams->pcCertificate;
        xCredentials.rootCaSize = ( size_t ) pxConnectParams->ulCertificateSize;

        /* Disable ALPN if requested. */
        if( ( pxConnectParams->xFlags & mqttagentUSE_AWS_IOT_ALPN_443 ) == 0 )
        {
            xCredentials.pAlpnProtos = NULL;
        }

        /* Disable SNI if requested. */
        if( ( pxConnectParams->xURLIsIPAddress == pdTRUE ) ||
            ( ( pxConnectParams->xFlags & mqttagentURL_IS_IP_ADDRESS ) == mqttagentURL_IS_IP_ADDRESS ) )
        {
            xCredentials.disableSni = true;
        }
    }

    /* Establish the network connection. */
    if( IotNetworkAfr_Create( &xServerInfo,
                              pxCredentials,
                              &( pxConnection->xNetworkConnection ) ) != IOT_NETWORK_SUCCESS )
    {
        xStatus = eMQTTAgentFailure;
    }
    else
    {
        pxConnection->xNetworkConnectionCreated = pdTRUE;
    }

    /* Establish the MQTT connection. */
    if( xStatus == eMQTTAgentSuccess )
    {
        /* Set the members of the network info. */
        xNetworkInfo.createNetworkConnection = false;
        xNetworkInfo.pNetworkConnection = pxConnection;
        xNetworkInfo.pNetworkInterface = &xNetworkInterface;

        /* Set the members of the MQTT connect info. */
        xMqttConnectInfo.cleanSession = true;
        xMqttConnectInfo.pClientIdentifier = ( const char * ) ( pxConnectParams->pucClientId );
        xMqttConnectInfo.clientIdentifierLength = pxConnectParams->usClientIdLength;
        xMqttConnectInfo.keepAliveSeconds = mqttconfigKEEP_ALIVE_INTERVAL_SECONDS;

        /* Call MQTT v4's CONNECT function. */
        xMqttStatus = IotMqtt_Connect( &xNetworkInfo,
                                       &xMqttConnectInfo,
                                       mqttTICKS_TO_MS( xTimeoutTicks ),
                                       &( pxConnection->xMQTTConnection ) );
        xStatus = prvConvertReturnCode( xMqttStatus );
    }

    /* Add a subscription to "#" to support the global callback when subscription
     * manager is disabled. */
    #if ( mqttconfigENABLE_SUBSCRIPTION_MANAGEMENT == 0 )
        IotMqttSubscription_t xGlobalSubscription = IOT_MQTT_SUBSCRIPTION_INITIALIZER;
        IotMqttReference_t xGlobalSubscriptionRef = IOT_MQTT_REFERENCE_INITIALIZER;

        if( xStatus == eMQTTAgentSuccess )
        {
            xGlobalSubscription.pTopicFilter = "#";
            xGlobalSubscription.topicFilterLength = 1;
            xGlobalSubscription.qos = 0;
            xGlobalSubscription.callback.param1 = pxConnection;
            xGlobalSubscription.callback.function = prvPublishCallbackWrapper;

            xMqttStatus = IotMqtt_Subscribe( pxConnection->xMQTTConnection,
                                             &xGlobalSubscription,
                                             1,
                                             IOT_MQTT_FLAG_WAITABLE,
                                             NULL,
                                             &xGlobalSubscriptionRef );
            xStatus = prvConvertReturnCode( xMqttStatus );
        }

        /* Wait for the subscription to "#" to complete. */
        if( xStatus == eMQTTAgentSuccess )
        {
            xMqttStatus = IotMqtt_Wait( xGlobalSubscriptionRef,
                                        mqttTICKS_TO_MS( xTimeoutTicks ) );
            xStatus = prvConvertReturnCode( xMqttStatus );
        }
    #endif /* if ( mqttconfigENABLE_SUBSCRIPTION_MANAGEMENT == 1 ) */

    return xStatus;
}

/*-----------------------------------------------------------*/

MQTTAgentReturnCode_t MQTT_AGENT_Disconnect( MQTTAgentHandle_t xMQTTHandle,
                                             TickType_t xTimeoutTicks )
{
    MQTTConnection_t * pxConnection = ( MQTTConnection_t * ) xMQTTHandle;

    /* MQTT v4's DISCONNECT function does not have a timeout argument. */
    ( void ) xTimeoutTicks;

    /* Check that the connection is established. */
    if( pxConnection->xMQTTConnection != IOT_MQTT_CONNECTION_INITIALIZER )
    {
        mqttconfigASSERT( pxConnection->xNetworkConnectionCreated == pdTRUE );

        /* Call MQTT v4's DISCONNECT function. */
        IotMqtt_Disconnect( pxConnection->xMQTTConnection,
                            false );
        pxConnection->xMQTTConnection = IOT_MQTT_CONNECTION_INITIALIZER;
    }

    return eMQTTAgentSuccess;
}

/*-----------------------------------------------------------*/

MQTTAgentReturnCode_t MQTT_AGENT_Subscribe( MQTTAgentHandle_t xMQTTHandle,
                                            const MQTTAgentSubscribeParams_t * const pxSubscribeParams,
                                            TickType_t xTimeoutTicks )
{
    MQTTAgentReturnCode_t xStatus = eMQTTAgentSuccess;
    IotMqttError_t xMqttStatus = IOT_MQTT_STATUS_PENDING;
    MQTTConnection_t * pxConnection = ( MQTTConnection_t * ) xMQTTHandle;
    IotMqttSubscription_t xSubscription = IOT_MQTT_SUBSCRIPTION_INITIALIZER;

    /* Store the topic filter if subscription management is enabled. */
    #if ( mqttconfigENABLE_SUBSCRIPTION_MANAGEMENT == 1 )
        /* Check topic filter length. */
        if( pxSubscribeParams->usTopicLength > mqttconfigSUBSCRIPTION_MANAGER_MAX_TOPIC_LENGTH )
        {
            xStatus = eMQTTAgentFailure;
        }

        /* Store the subscription. */
        if( prvStoreCallback( pxConnection,
                              ( const char * ) pxSubscribeParams->pucTopic,
                              pxSubscribeParams->usTopicLength,
                              pxSubscribeParams->pxPublishCallback,
                              pxSubscribeParams->pvPublishCallbackContext ) == pdFAIL )
        {
            xStatus = eMQTTAgentFailure;
        }
    #endif /* if ( mqttconfigENABLE_SUBSCRIPTION_MANAGEMENT == 1 ) */

    /* Call MQTT v4 blocking SUBSCRIBE function. */
    if( xStatus == eMQTTAgentSuccess )
    {
        /* Set the members of the MQTT subscription. */
        xSubscription.pTopicFilter = ( const char * ) ( pxSubscribeParams->pucTopic );
        xSubscription.topicFilterLength = pxSubscribeParams->usTopicLength;
        xSubscription.qos = ( IotMqttQos_t ) pxSubscribeParams->xQoS;
        xSubscription.callback.param1 = pxConnection;
        xSubscription.callback.function = prvPublishCallbackWrapper;

        xMqttStatus = IotMqtt_TimedSubscribe( pxConnection->xMQTTConnection,
                                              &xSubscription,
                                              1,
                                              0,
                                              mqttTICKS_TO_MS( xTimeoutTicks ) );
        xStatus = prvConvertReturnCode( xMqttStatus );
    }

    return xStatus;
}

/*-----------------------------------------------------------*/

MQTTAgentReturnCode_t MQTT_AGENT_Unsubscribe( MQTTAgentHandle_t xMQTTHandle,
                                              const MQTTAgentUnsubscribeParams_t * const pxUnsubscribeParams,
                                              TickType_t xTimeoutTicks )
{
    IotMqttError_t xMqttStatus = IOT_MQTT_STATUS_PENDING;
    MQTTConnection_t * pxConnection = ( MQTTConnection_t * ) xMQTTHandle;
    IotMqttSubscription_t xSubscription = IOT_MQTT_SUBSCRIPTION_INITIALIZER;

    /* Remove any subscription callback that may be registered. */
    #if ( mqttconfigENABLE_SUBSCRIPTION_MANAGEMENT == 1 )
        prvRemoveCallback( pxConnection,
                           ( const char * ) ( pxUnsubscribeParams->pucTopic ),
                           pxUnsubscribeParams->usTopicLength );
    #endif

    /* Set the members of the subscription to remove. */
    xSubscription.pTopicFilter = ( const char * ) ( pxUnsubscribeParams->pucTopic );
    xSubscription.topicFilterLength = pxUnsubscribeParams->usTopicLength;
    xSubscription.callback.param1 = pxConnection;
    xSubscription.callback.function = prvPublishCallbackWrapper;

    /* Call MQTT v4 blocking UNSUBSCRIBE function. */
    xMqttStatus = IotMqtt_TimedUnsubscribe( pxConnection->xMQTTConnection,
                                            &xSubscription,
                                            1,
                                            0,
                                            mqttTICKS_TO_MS( xTimeoutTicks ) );

    return prvConvertReturnCode( xMqttStatus );
}

/*-----------------------------------------------------------*/

MQTTAgentReturnCode_t MQTT_AGENT_Publish( MQTTAgentHandle_t xMQTTHandle,
                                          const MQTTAgentPublishParams_t * const pxPublishParams,
                                          TickType_t xTimeoutTicks )
{
    IotMqttError_t xMqttStatus = IOT_MQTT_STATUS_PENDING;
    MQTTConnection_t * pxConnection = ( MQTTConnection_t * ) xMQTTHandle;
    IotMqttPublishInfo_t xPublishInfo = IOT_MQTT_PUBLISH_INFO_INITIALIZER;

    /* Set the members of the publish info. */
    xPublishInfo.pTopicName = ( const char * ) pxPublishParams->pucTopic;
    xPublishInfo.topicNameLength = pxPublishParams->usTopicLength;
    xPublishInfo.qos = ( IotMqttQos_t ) pxPublishParams->xQoS;
    xPublishInfo.pPayload = ( const void * ) pxPublishParams->pvData;
    xPublishInfo.payloadLength = pxPublishParams->ulDataLength;

    /* Call the MQTT v4 blocking PUBLISH function. */
    xMqttStatus = IotMqtt_TimedPublish( pxConnection->xMQTTConnection,
                                        &xPublishInfo,
                                        0,
                                        mqttTICKS_TO_MS( xTimeoutTicks ) );

    return prvConvertReturnCode( xMqttStatus );
}

/*-----------------------------------------------------------*/

MQTTAgentReturnCode_t MQTT_AGENT_ReturnBuffer( MQTTAgentHandle_t xMQTTHandle,
                                               MQTTBufferHandle_t xBufferHandle )
{
    ( void ) xMQTTHandle;

    /* Free the MQTT buffer. */
    vPortFree( xBufferHandle );

    return eMQTTAgentSuccess;
}

/*-----------------------------------------------------------*/
