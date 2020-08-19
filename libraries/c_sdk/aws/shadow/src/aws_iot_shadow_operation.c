/*
 * FreeRTOS Shadow V2.2.0
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
 * @file aws_iot_shadow_operation.c
 * @brief Implements functions that process Shadow operations.
 */

/* The config header is always included first. */
#include "iot_config.h"

/* Standard includes. */
#include <string.h>

/* Shadow internal include. */
#include "private/aws_iot_shadow_internal.h"

/* Platform layer includes. */
#include "platform/iot_threads.h"

/* JSON utils include. */
#include "iot_json_utils.h"

/* MQTT include. */
#include "iot_mqtt.h"

/*-----------------------------------------------------------*/

/**
 * @brief First parameter to #_shadowOperation_match.
 */
typedef struct _operationMatchParams
{
    _shadowOperationType_t type; /**< @brief DELETE, GET, or UPDATE. */
    const char * pThingName;     /**< @brief Thing Name of Shadow operation. */
    size_t thingNameLength;      /**< @brief Length of #_operationMatchParams_t.pThingName. */
    const char * pDocument;      /**< @brief Shadow UPDATE response document. */
    size_t documentLength;       /**< @brief Length of #_operationMatchParams_t.pDocument. */
} _operationMatchParams_t;

/*-----------------------------------------------------------*/

/**
 * @brief Match a received Shadow response with a Shadow operation awaiting a
 * response.
 *
 * @param[in] pOperationLink Pointer to the link member of the #_shadowOperation_t
 * to check.
 * @param[in] pMatch Pointer to an #_operationMatchParams_t.
 *
 * @return `true` if `pMatch` matches the received response; `false` otherwise.
 */
static bool _shadowOperation_match( const IotLink_t * pOperationLink,
                                    void * pMatch );

/**
 * @brief Common function for processing received Shadow responses.
 *
 * @param[in] type DELETE, GET, or UPDATE.
 * @param[in] pMessage Received Shadow response (as an MQTT PUBLISH message).
 */
static void _commonOperationCallback( _shadowOperationType_t type,
                                      IotMqttCallbackParam_t * pMessage );

/**
 * @brief Invoked when a Shadow response is received for Shadow DELETE.
 *
 * @param[in] pArgument Ignored.
 * @param[in] pMessage Received Shadow response (as an MQTT PUBLISH message).
 */
static void _deleteCallback( void * pArgument,
                             IotMqttCallbackParam_t * pMessage );

/**
 * @brief Invoked when a Shadow response is received for a Shadow GET.
 *
 * @param[in] pArgument Ignored.
 * @param[in] pMessage Received Shadow response (as an MQTT PUBLISH message).
 */
static void _getCallback( void * pArgument,
                          IotMqttCallbackParam_t * pMessage );

/**
 * @brief Process an incoming Shadow document received when a Shadow GET is
 * accepted.
 *
 * @param[in] pOperation The GET operation associated with the incoming Shadow
 * document.
 * @param[in] pPublishInfo The received Shadow document (as an MQTT PUBLISH
 * message).
 *
 * @return #AWS_IOT_SHADOW_SUCCESS or #AWS_IOT_SHADOW_NO_MEMORY. Memory allocation
 * only happens for a waitable `pOperation`.
 */
static AwsIotShadowError_t _processAcceptedGet( _shadowOperation_t * pOperation,
                                                const IotMqttPublishInfo_t * pPublishInfo );

/**
 * @brief Invoked when a Shadow response is received for a Shadow UPDATE.
 *
 * @param[in] pArgument Ignored.
 * @param[in] pMessage Received Shadow response (as an MQTT PUBLISH message).
 */
static void _updateCallback( void * pArgument,
                             IotMqttCallbackParam_t * pMessage );

/*-----------------------------------------------------------*/

#if LIBRARY_LOG_LEVEL > IOT_LOG_NONE

/**
 * @brief Printable names for each of the Shadow operations.
 */
    const char * const _pAwsIotShadowOperationNames[] =
    {
        "DELETE",
        "GET",
        "UPDATE",
        "SET DELTA",
        "SET UPDATED"
    };
#endif /* if LIBRARY_LOG_LEVEL > IOT_LOG_NONE */

/**
 * @brief List of active Shadow operations awaiting a response from the Shadow
 * service.
 */
IotListDouble_t _AwsIotShadowPendingOperations = { 0 };

/**
 * @brief Protects #_AwsIotShadowPendingOperations from concurrent access.
 */
IotMutex_t _AwsIotShadowPendingOperationsMutex;

/*-----------------------------------------------------------*/

static bool _shadowOperation_match( const IotLink_t * pOperationLink,
                                    void * pMatch )
{
    /* Because this function is called from a container function, the given link
     * must never be NULL. */
    AwsIotShadow_Assert( pOperationLink != NULL );

    _shadowOperation_t * pOperation = IotLink_Container( _shadowOperation_t,
                                                         pOperationLink,
                                                         link );
    _operationMatchParams_t * pParam = ( _operationMatchParams_t * ) pMatch;
    _shadowSubscription_t * pSubscription = pOperation->pSubscription;
    const char * pClientToken = NULL;
    size_t clientTokenLength = 0;

    /* Check for matching Thing Name and operation type. */
    bool match = ( pOperation->type == pParam->type ) &&
                 ( pParam->thingNameLength == pSubscription->thingNameLength ) &&
                 ( strncmp( pParam->pThingName,
                            pSubscription->pThingName,
                            pParam->thingNameLength ) == 0 );

    /* For a Shadow UPDATE operation, compare the client tokens. */
    if( ( match == true ) && ( pOperation->type == _SHADOW_UPDATE ) )
    {
        /* Check document pointers. */
        AwsIotShadow_Assert( pParam->pDocument != NULL );
        AwsIotShadow_Assert( pParam->documentLength > 0 );
        AwsIotShadow_Assert( pOperation->u.update.pClientToken != NULL );
        AwsIotShadow_Assert( pOperation->u.update.clientTokenLength > 0 );

        IotLogDebug( "Verifying client tokens for Shadow UPDATE." );

        /* Check for the client token in the UPDATE response document. */
        match = IotJsonUtils_FindJsonValue( pParam->pDocument,
                                            pParam->documentLength,
                                            CLIENT_TOKEN_KEY,
                                            CLIENT_TOKEN_KEY_LENGTH,
                                            &pClientToken,
                                            &clientTokenLength );

        /* If the UPDATE response document has a client token, check that it
         * matches. */
        if( match == true )
        {
            match = ( clientTokenLength == pOperation->u.update.clientTokenLength ) &&
                    ( strncmp( pClientToken,
                               pOperation->u.update.pClientToken,
                               clientTokenLength ) == 0 );
        }
        else
        {
            IotLogWarn( "Received a Shadow UPDATE response with no client token. "
                        "This is possibly a response to a bad JSON document:\n%.*s",
                        pParam->documentLength,
                        pParam->pDocument );
        }
    }

    return match;
}

/*-----------------------------------------------------------*/

static void _commonOperationCallback( _shadowOperationType_t type,
                                      IotMqttCallbackParam_t * pMessage )
{
    _shadowOperation_t * pOperation = NULL;
    IotLink_t * pOperationLink = NULL;
    _shadowOperationStatus_t status = _UNKNOWN_STATUS;
    _operationMatchParams_t param = { .type = ( _shadowOperationType_t ) 0 };
    uint32_t flags = 0;

    /* Set operation type to search. */
    param.type = type;

    /* Set the response document for a Shadow UPDATE. */
    if( type == _SHADOW_UPDATE )
    {
        param.pDocument = pMessage->u.message.info.pPayload;
        param.documentLength = pMessage->u.message.info.payloadLength;
    }

    /* Parse the Thing Name from the MQTT topic name. */
    if( _AwsIotShadow_ParseThingName( pMessage->u.message.info.pTopicName,
                                      pMessage->u.message.info.topicNameLength,
                                      &( param.pThingName ),
                                      &( param.thingNameLength ) ) != AWS_IOT_SHADOW_SUCCESS )
    {
        return;
    }

    /* Lock the pending operations list for exclusive access. */
    IotMutex_Lock( &( _AwsIotShadowPendingOperationsMutex ) );

    /* Search for a matching pending operation. */
    pOperationLink = IotListDouble_FindFirstMatch( &( _AwsIotShadowPendingOperations ),
                                                   NULL,
                                                   _shadowOperation_match,
                                                   &param );

    /* Find and remove the first Shadow operation of the given type. */
    if( pOperationLink == NULL )
    {
        /* Operation is not pending. It may have already been processed. Return
         * without doing anything */
        IotMutex_Unlock( &( _AwsIotShadowPendingOperationsMutex ) );

        IotLogWarn( "Shadow %s callback received an unknown operation.",
                    _pAwsIotShadowOperationNames[ type ] );

        return;
    }
    else
    {
        pOperation = IotLink_Container( _shadowOperation_t, pOperationLink, link );

        /* Remove a non-waitable operation from the pending operation list. */
        if( ( pOperation->flags & AWS_IOT_SHADOW_FLAG_WAITABLE ) == 0 )
        {
            IotListDouble_Remove( &( pOperation->link ) );
            IotMutex_Unlock( &( _AwsIotShadowPendingOperationsMutex ) );
        }
    }

    /* Check that the Shadow operation type and status. */
    AwsIotShadow_Assert( pOperation->type == type );
    AwsIotShadow_Assert( pOperation->status == AWS_IOT_SHADOW_STATUS_PENDING );

    IotLogDebug( "Received Shadow response on topic %.*s",
                 pMessage->u.message.info.topicNameLength,
                 pMessage->u.message.info.pTopicName );

    /* Parse the status from the topic name. */
    status = _AwsIotShadow_ParseShadowStatus( pMessage->u.message.info.pTopicName,
                                              pMessage->u.message.info.topicNameLength );

    switch( status )
    {
        case _SHADOW_ACCEPTED:
            IotLogInfo( "Shadow %s of %.*s was ACCEPTED.",
                        _pAwsIotShadowOperationNames[ type ],
                        pOperation->pSubscription->thingNameLength,
                        pOperation->pSubscription->pThingName );

            /* Process the retrieved document for a Shadow GET. Otherwise, set
             * status to success. */
            if( type == _SHADOW_GET )
            {
                pOperation->status = _processAcceptedGet( pOperation,
                                                          &( pMessage->u.message.info ) );
            }
            else
            {
                pOperation->status = AWS_IOT_SHADOW_SUCCESS;
            }

            break;

        case _SHADOW_REJECTED:
            IotLogWarn( "Shadow %s of %.*s was REJECTED.",
                        _pAwsIotShadowOperationNames[ type ],
                        pOperation->pSubscription->thingNameLength,
                        pOperation->pSubscription->pThingName );

            pOperation->status = _AwsIotShadow_ParseErrorDocument( pMessage->u.message.info.pPayload,
                                                                   pMessage->u.message.info.payloadLength );
            break;

        default:
            IotLogWarn( "Unknown status for %s of %.*s Shadow. Ignoring message.",
                        _pAwsIotShadowOperationNames[ type ],
                        pOperation->pSubscription->thingNameLength,
                        pOperation->pSubscription->pThingName );

            pOperation->status = AWS_IOT_SHADOW_BAD_RESPONSE;
            break;
    }

    /* Copy the flags from the Shadow operation. The notify function may delete the operation. */
    flags = pOperation->flags;

    /* Notify of operation completion. */
    _AwsIotShadow_Notify( pOperation );

    /* For waitable operations, unlock the pending operation list mutex to signal
     * this function's completion. */
    if( ( flags & AWS_IOT_SHADOW_FLAG_WAITABLE ) == AWS_IOT_SHADOW_FLAG_WAITABLE )
    {
        IotMutex_Unlock( &( _AwsIotShadowPendingOperationsMutex ) );
    }
}

/*-----------------------------------------------------------*/

static void _deleteCallback( void * pArgument,
                             IotMqttCallbackParam_t * pMessage )
{
    /* Silence warnings about unused parameter. */
    ( void ) pArgument;

    _commonOperationCallback( _SHADOW_DELETE, pMessage );
}

/*-----------------------------------------------------------*/

static void _getCallback( void * pArgument,
                          IotMqttCallbackParam_t * pMessage )
{
    /* Silence warnings about unused parameter. */
    ( void ) pArgument;

    _commonOperationCallback( _SHADOW_GET, pMessage );
}

/*-----------------------------------------------------------*/

static AwsIotShadowError_t _processAcceptedGet( _shadowOperation_t * pOperation,
                                                const IotMqttPublishInfo_t * pPublishInfo )
{
    AwsIotShadowError_t status = AWS_IOT_SHADOW_SUCCESS;

    /* A non-waitable operation can re-use the pointers from the publish info,
     * since those are guaranteed to be in-scope throughout the user callback.
     * But a waitable operation must copy the data from the publish info because
     * AwsIotShadow_Wait may be called after the MQTT library frees the publish
     * info. */
    if( ( pOperation->flags & AWS_IOT_SHADOW_FLAG_WAITABLE ) == 0 )
    {
        pOperation->u.get.pDocument = pPublishInfo->pPayload;
        pOperation->u.get.documentLength = pPublishInfo->payloadLength;
    }
    else
    {
        IotLogDebug( "Allocating new buffer for waitable Shadow GET." );

        /* Parameter validation should not have allowed a NULL malloc function. */
        AwsIotShadow_Assert( pOperation->u.get.mallocDocument != NULL );

        /* Allocate a buffer for the retrieved document. */
        pOperation->u.get.pDocument = pOperation->u.get.mallocDocument( pPublishInfo->payloadLength );

        if( pOperation->u.get.pDocument == NULL )
        {
            IotLogError( "Failed to allocate buffer for retrieved Shadow document." );

            status = AWS_IOT_SHADOW_NO_MEMORY;
        }
        else
        {
            /* Copy the retrieved document. */
            ( void ) memcpy( ( void * ) pOperation->u.get.pDocument,
                             pPublishInfo->pPayload,
                             pPublishInfo->payloadLength );
            pOperation->u.get.documentLength = pPublishInfo->payloadLength;
        }
    }

    return status;
}

/*-----------------------------------------------------------*/

static void _updateCallback( void * pArgument,
                             IotMqttCallbackParam_t * pMessage )
{
    /* Silence warnings about unused parameter. */
    ( void ) pArgument;

    _commonOperationCallback( _SHADOW_UPDATE, pMessage );
}

/*-----------------------------------------------------------*/

AwsIotShadowError_t _AwsIotShadow_CreateOperation( _shadowOperation_t ** pNewOperation,
                                                   _shadowOperationType_t type,
                                                   uint32_t flags,
                                                   const AwsIotShadowCallbackInfo_t * pCallbackInfo )
{
    _shadowOperation_t * pOperation = NULL;

    IotLogDebug( "Creating operation record for Shadow %s.",
                 _pAwsIotShadowOperationNames[ type ] );

    /* Allocate memory for a new Shadow operation. */
    pOperation = AwsIotShadow_MallocOperation( sizeof( _shadowOperation_t ) );

    if( pOperation == NULL )
    {
        IotLogError( "Failed to allocate memory for Shadow %s.",
                     _pAwsIotShadowOperationNames[ type ] );

        return AWS_IOT_SHADOW_NO_MEMORY;
    }

    /* Clear the operation data. */
    ( void ) memset( pOperation, 0x00, sizeof( _shadowOperation_t ) );

    /* Check if the waitable flag is set. If it is, create a semaphore to
     * wait on. */
    if( ( flags & AWS_IOT_SHADOW_FLAG_WAITABLE ) == AWS_IOT_SHADOW_FLAG_WAITABLE )
    {
        if( IotSemaphore_Create( &( pOperation->notify.waitSemaphore ), 0, 1 ) == false )
        {
            IotLogError( "Failed to create semaphore for waitable Shadow %s.",
                         _pAwsIotShadowOperationNames[ type ] );

            AwsIotShadow_FreeOperation( pOperation );

            return AWS_IOT_SHADOW_NO_MEMORY;
        }
    }
    else
    {
        /* If the waitable flag isn't set but a callback is, copy the callback
         * information. */
        if( pCallbackInfo != NULL )
        {
            pOperation->notify.callback = *pCallbackInfo;
        }
    }

    /* Set the remaining common members of the Shadow operation. */
    pOperation->type = type;
    pOperation->flags = flags;
    pOperation->status = AWS_IOT_SHADOW_STATUS_PENDING;

    /* Set the output parameter. */
    *pNewOperation = pOperation;

    return AWS_IOT_SHADOW_SUCCESS;
}

/*-----------------------------------------------------------*/

void _AwsIotShadow_DestroyOperation( void * pData )
{
    _shadowOperation_t * pOperation = ( _shadowOperation_t * ) pData;

    /* The Shadow operation pointer must not be NULL. */
    AwsIotShadow_Assert( pOperation != NULL );

    IotLogDebug( "Destroying Shadow operation %s.",
                 _pAwsIotShadowOperationNames[ pOperation->type ] );

    /* Check if a wait semaphore was created for this operation. */
    if( ( pOperation->flags & AWS_IOT_SHADOW_FLAG_WAITABLE ) == AWS_IOT_SHADOW_FLAG_WAITABLE )
    {
        /* Destroy the wait semaphore */
        IotSemaphore_Destroy( &( pOperation->notify.waitSemaphore ) );
    }

    /* If this is a Shadow update, free any allocated client token. */
    if( ( pOperation->type == _SHADOW_UPDATE ) &&
        ( pOperation->u.update.pClientToken != NULL ) )
    {
        AwsIotShadow_Assert( pOperation->u.update.clientTokenLength > 0 );

        AwsIotShadow_FreeString( ( void * ) ( pOperation->u.update.pClientToken ) );
    }

    /* Free the memory used to hold operation data. */
    AwsIotShadow_FreeOperation( pOperation );
}

/*-----------------------------------------------------------*/

AwsIotShadowError_t _AwsIotShadow_GenerateShadowTopic( _shadowOperationType_t type,
                                                       const char * pThingName,
                                                       size_t thingNameLength,
                                                       char ** pTopicBuffer,
                                                       uint16_t * pOperationTopicLength )
{
    uint16_t bufferLength = 0;
    uint16_t operationTopicLength = 0;
    char * pBuffer = NULL;

    /* Lookup table for Shadow operation strings. */
    const char * const pOperationString[ SHADOW_OPERATION_COUNT ] =
    {
        SHADOW_DELETE_OPERATION_STRING, /* Shadow delete operation. */
        SHADOW_GET_OPERATION_STRING,    /* Shadow get operation. */
        SHADOW_UPDATE_OPERATION_STRING  /* Shadow update operation. */
    };

    /* Lookup table for Shadow operation string lengths. */
    const uint16_t pOperationStringLength[ SHADOW_OPERATION_COUNT ] =
    {
        SHADOW_DELETE_OPERATION_STRING_LENGTH, /* Shadow delete operation. */
        SHADOW_GET_OPERATION_STRING_LENGTH,    /* Shadow get operation. */
        SHADOW_UPDATE_OPERATION_STRING_LENGTH  /* Shadow update operation. */
    };

    /* Only Shadow delete, get, and update operation types should be passed to this
     * function. */
    AwsIotShadow_Assert( ( type == _SHADOW_DELETE ) ||
                         ( type == _SHADOW_GET ) ||
                         ( type == _SHADOW_UPDATE ) );

    /* Calculate the required topic buffer length. */
    bufferLength = ( uint16_t ) ( SHADOW_TOPIC_PREFIX_LENGTH +
                                  thingNameLength +
                                  pOperationStringLength[ type ] +
                                  SHADOW_LONGEST_SUFFIX_LENGTH );

    /* Allocate memory for the topic buffer if no topic buffer is given. */
    if( *pTopicBuffer == NULL )
    {
        pBuffer = AwsIotShadow_MallocString( ( size_t ) bufferLength );

        if( pBuffer == NULL )
        {
            return AWS_IOT_SHADOW_NO_MEMORY;
        }
    }
    /* Otherwise, use the given topic buffer. */
    else
    {
        pBuffer = *pTopicBuffer;
    }

    /* Copy the Shadow topic prefix into the topic buffer. */
    ( void ) memcpy( pBuffer, SHADOW_TOPIC_PREFIX, SHADOW_TOPIC_PREFIX_LENGTH );
    operationTopicLength = ( uint16_t ) ( operationTopicLength + SHADOW_TOPIC_PREFIX_LENGTH );

    /* Copy the Thing Name into the topic buffer. */
    ( void ) memcpy( pBuffer + operationTopicLength, pThingName, thingNameLength );
    operationTopicLength = ( uint16_t ) ( operationTopicLength + thingNameLength );

    /* Copy the Shadow operation string into the topic buffer. */
    ( void ) memcpy( pBuffer + operationTopicLength,
                     pOperationString[ type ],
                     pOperationStringLength[ type ] );
    operationTopicLength = ( uint16_t ) ( operationTopicLength + pOperationStringLength[ type ] );

    /* Ensure that the topic length is in the topic buffer. */
    AwsIotShadow_Assert( operationTopicLength < bufferLength );

    /* Set the output parameters. */
    if( *pTopicBuffer == NULL )
    {
        *pTopicBuffer = pBuffer;
    }

    *pOperationTopicLength = operationTopicLength;

    return AWS_IOT_SHADOW_SUCCESS;
}

/*-----------------------------------------------------------*/

AwsIotShadowError_t _AwsIotShadow_ProcessOperation( IotMqttConnection_t mqttConnection,
                                                    const char * pThingName,
                                                    size_t thingNameLength,
                                                    _shadowOperation_t * pOperation,
                                                    const AwsIotShadowDocumentInfo_t * pDocumentInfo )
{
    _shadowSubscription_t * pSubscription = NULL;
    AwsIotShadowError_t status = AWS_IOT_SHADOW_STATUS_PENDING;
    IotMqttError_t publishStatus = IOT_MQTT_STATUS_PENDING;
    char * pTopicBuffer = NULL;
    uint16_t operationTopicLength = 0;
    bool freeTopicBuffer = true;
    IotMqttPublishInfo_t publishInfo = IOT_MQTT_PUBLISH_INFO_INITIALIZER;

    /* Lookup table for Shadow operation callbacks. */
    const _mqttCallbackFunction_t shadowCallbacks[ SHADOW_OPERATION_COUNT ] =
    {
        _deleteCallback,
        _getCallback,
        _updateCallback
    };

    IotLogDebug( "Processing Shadow operation %s for Thing %.*s.",
                 _pAwsIotShadowOperationNames[ pOperation->type ],
                 thingNameLength,
                 pThingName );

    /* Set the operation's MQTT connection. */
    pOperation->mqttConnection = mqttConnection;

    /* Generate the operation topic buffer. */
    if( _AwsIotShadow_GenerateShadowTopic( pOperation->type,
                                           pThingName,
                                           thingNameLength,
                                           &pTopicBuffer,
                                           &operationTopicLength ) != AWS_IOT_SHADOW_SUCCESS )
    {
        IotLogError( "No memory for Shadow operation topic buffer." );

        _AwsIotShadow_DestroyOperation( pOperation );

        return AWS_IOT_SHADOW_NO_MEMORY;
    }

    /* Lock the subscription list mutex for exclusive access. */
    IotMutex_Lock( &_AwsIotShadowSubscriptionsMutex );

    /* Check for an existing subscription. This function will attempt to allocate
     * a new subscription if not found. */
    pSubscription = _AwsIotShadow_FindSubscription( pThingName,
                                                    thingNameLength );

    if( pSubscription == NULL )
    {
        /* No existing subscription was found, and no new subscription could be
         * allocated. */
        status = AWS_IOT_SHADOW_NO_MEMORY;
    }

    if( status == AWS_IOT_SHADOW_STATUS_PENDING )
    {
        /* Ensure that the subscription Thing Name matches. */
        AwsIotShadow_Assert( pSubscription != NULL );
        AwsIotShadow_Assert( pSubscription->thingNameLength == thingNameLength );
        AwsIotShadow_Assert( strncmp( pSubscription->pThingName,
                                      pThingName,
                                      thingNameLength ) == 0 );

        /* Set the subscription object for the Shadow operation. */
        pOperation->pSubscription = pSubscription;

        /* Assign the topic buffer to the subscription to use for unsubscribing if
         * the subscription has no topic buffer. */
        if( pSubscription->pTopicBuffer == NULL )
        {
            pSubscription->pTopicBuffer = pTopicBuffer;

            /* This function should not free the topic buffer. */
            freeTopicBuffer = false;
        }

        /* Increment the reference count for this Shadow operation's
         * subscriptions. */
        status = _AwsIotShadow_IncrementReferences( pOperation,
                                                    pTopicBuffer,
                                                    operationTopicLength,
                                                    shadowCallbacks[ pOperation->type ] );

        if( status != AWS_IOT_SHADOW_STATUS_PENDING )
        {
            /* Failed to add subscriptions for a Shadow operation. The reference
             * count was not incremented. Check if this subscription should be
             * deleted. */
            _AwsIotShadow_RemoveSubscription( pSubscription, NULL );
        }
    }

    /* Unlock the Shadow subscription list mutex. */
    IotMutex_Unlock( &_AwsIotShadowSubscriptionsMutex );

    /* Check that all memory allocation and subscriptions succeeded. */
    if( status == AWS_IOT_SHADOW_STATUS_PENDING )
    {
        /* Set the operation topic name. */
        publishInfo.pTopicName = pTopicBuffer;
        publishInfo.topicNameLength = operationTopicLength;

        IotLogDebug( "Shadow %s message will be published to topic %.*s",
                     _pAwsIotShadowOperationNames[ pOperation->type ],
                     publishInfo.topicNameLength,
                     publishInfo.pTopicName );

        /* Set the document info if this operation is not a Shadow DELETE. */
        if( pOperation->type != _SHADOW_DELETE )
        {
            publishInfo.qos = pDocumentInfo->qos;
            publishInfo.retryLimit = pDocumentInfo->retryLimit;
            publishInfo.retryMs = pDocumentInfo->retryMs;

            IotLogDebug( "Shadow %s message will be published at QoS %d with "
                         "retryLimit %d and retryMs %u.",
                         _pAwsIotShadowOperationNames[ pOperation->type ],
                         publishInfo.qos,
                         publishInfo.retryLimit,
                         publishInfo.retryMs );
        }

        /* Set the PUBLISH payload to the update document for Shadow UPDATE. */
        if( pOperation->type == _SHADOW_UPDATE )
        {
            publishInfo.pPayload = pDocumentInfo->u.update.pUpdateDocument;
            publishInfo.payloadLength = pDocumentInfo->u.update.updateDocumentLength;
        }

        /* Set the PUBLISH payload to an empty string for Shadow DELETE and GET,
         * per the Shadow spec. */
        else
        {
            publishInfo.pPayload = "";
            publishInfo.payloadLength = 0;
        }

        /* Add Shadow operation to the pending operations list. */
        IotMutex_Lock( &( _AwsIotShadowPendingOperationsMutex ) );
        IotListDouble_InsertHead( &( _AwsIotShadowPendingOperations ),
                                  &( pOperation->link ) );
        IotMutex_Unlock( &( _AwsIotShadowPendingOperationsMutex ) );

        /* Publish to the Shadow topic name. */
        publishStatus = IotMqtt_TimedPublish( pOperation->mqttConnection,
                                              &publishInfo,
                                              0,
                                              _AwsIotShadowMqttTimeoutMs );

        /* Check for errors from the MQTT publish. */
        if( publishStatus != IOT_MQTT_SUCCESS )
        {
            IotLogError( "Failed to publish MQTT message to %s %.*s Shadow, error %s.",
                         _pAwsIotShadowOperationNames[ pOperation->type ],
                         thingNameLength,
                         pThingName,
                         IotMqtt_strerror( publishStatus ) );

            /* Convert the MQTT "NO MEMORY" error to a Shadow "NO MEMORY" error. */
            if( publishStatus == IOT_MQTT_NO_MEMORY )
            {
                status = AWS_IOT_SHADOW_NO_MEMORY;
            }
            else
            {
                status = AWS_IOT_SHADOW_MQTT_ERROR;
            }

            /* If the "keep subscriptions" flag is not set, decrement the reference
             * count. */
            if( ( pOperation->flags & AWS_IOT_SHADOW_FLAG_KEEP_SUBSCRIPTIONS ) == 0 )
            {
                IotMutex_Lock( &_AwsIotShadowSubscriptionsMutex );
                _AwsIotShadow_DecrementReferences( pOperation,
                                                   pTopicBuffer,
                                                   NULL );
                IotMutex_Unlock( &_AwsIotShadowSubscriptionsMutex );
            }

            /* Remove Shadow operation from the pending operations list. */
            IotMutex_Lock( &( _AwsIotShadowPendingOperationsMutex ) );
            IotListDouble_Remove( &( pOperation->link ) );
            IotMutex_Unlock( &( _AwsIotShadowPendingOperationsMutex ) );
        }
        else
        {
            IotLogDebug( "Shadow %s PUBLISH message successfully sent.",
                         _pAwsIotShadowOperationNames[ pOperation->type ] );
        }
    }

    /* Free the topic buffer used by this function if it was not assigned to a
     * subscription. */
    if( ( freeTopicBuffer == true ) && ( pTopicBuffer != NULL ) )
    {
        AwsIotShadow_FreeString( pTopicBuffer );
    }

    /* Destroy the Shadow operation on failure. */
    if( status != AWS_IOT_SHADOW_STATUS_PENDING )
    {
        _AwsIotShadow_DestroyOperation( pOperation );
    }

    return status;
}

/*-----------------------------------------------------------*/

void _AwsIotShadow_Notify( _shadowOperation_t * pOperation )
{
    AwsIotShadowCallbackParam_t callbackParam = { .callbackType = ( AwsIotShadowCallbackType_t ) 0 };
    _shadowSubscription_t * pSubscription = pOperation->pSubscription,
                          * pRemovedSubscription = NULL;

    /* If the operation is waiting, post to its wait semaphore and return. */
    if( ( pOperation->flags & AWS_IOT_SHADOW_FLAG_WAITABLE ) == AWS_IOT_SHADOW_FLAG_WAITABLE )
    {
        IotSemaphore_Post( &( pOperation->notify.waitSemaphore ) );

        return;
    }

    /* Decrement the reference count. This also removes subscriptions if the
     * count reaches 0. */
    IotMutex_Lock( &_AwsIotShadowSubscriptionsMutex );
    _AwsIotShadow_DecrementReferences( pOperation,
                                       pSubscription->pTopicBuffer,
                                       &pRemovedSubscription );
    IotMutex_Unlock( &_AwsIotShadowSubscriptionsMutex );

    /* Set the subscription pointer used for the user callback based on whether
     * a subscription was removed from the list. */
    if( pRemovedSubscription != NULL )
    {
        pSubscription = pRemovedSubscription;
    }

    AwsIotShadow_Assert( pSubscription != NULL );

    /* Invoke the user callback if provided. */
    if( pOperation->notify.callback.function != NULL )
    {
        /* Set the common members of the callback parameter. */
        callbackParam.callbackType = ( AwsIotShadowCallbackType_t ) pOperation->type;
        callbackParam.mqttConnection = pOperation->mqttConnection;
        callbackParam.u.operation.result = pOperation->status;
        callbackParam.u.operation.reference = pOperation;
        callbackParam.pThingName = pSubscription->pThingName;
        callbackParam.thingNameLength = pSubscription->thingNameLength;

        /* Set the members of the callback parameter for a received document. */
        if( pOperation->type == _SHADOW_GET )
        {
            callbackParam.u.operation.get.pDocument = pOperation->u.get.pDocument;
            callbackParam.u.operation.get.documentLength = pOperation->u.get.documentLength;
        }

        pOperation->notify.callback.function( pOperation->notify.callback.pCallbackContext,
                                              &callbackParam );
    }

    /* Destroy a removed subscription. */
    if( pRemovedSubscription != NULL )
    {
        _AwsIotShadow_DestroySubscription( pRemovedSubscription );
    }

    _AwsIotShadow_DestroyOperation( pOperation );
}

/*-----------------------------------------------------------*/
