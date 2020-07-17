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
 * @file aws_shadow.c
 * @brief Shadow API. Provide simple function to modify/create/delete Things shadows.
 * This file implements the Shadow v1 API over Shadow v2.
 */

/* The config header is always included first. */
#include "iot_config.h"

/* C library includes. */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

/* FreeRTOS includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"

/* AWS includes. */
#include "aws_shadow_config.h"
#include "aws_shadow_config_defaults.h"
#include "iot_mqtt_agent_config.h"
#include "iot_mqtt_agent_config_defaults.h"
#include "aws_shadow.h"

/* Shadow v2 include. */
#include "aws_iot_shadow.h"

/* Configure logging for Shadow. */
#if shadowconfigENABLE_DEBUG_LOGS == 1
    #define Shadow_debug_printf( X )    configPRINTF( X )
#else
    #define Shadow_debug_printf( X )
#endif

/**
 * @brief Converts FreeRTOS ticks to milliseconds.
 */
#define shadowTICKS_TO_MS( xTicks )    ( xTicks * 1000 / configTICK_RATE_HZ )

/**
 * @brief Holds data on a Shadow client.
 */
typedef struct ShadowClient
{
    MQTTAgentHandle_t xMqttConnection;        /**< @brief Dedicated MQTT connection. */
    const char * pcCallbackThingName;         /**< @brief The Thing Name passed to callbacks. */
    ShadowDeletedCallback_t xDeletedCallback; /**< @brief Shadow v1 deleted callback. */
    ShadowDeltaCallback_t xDeltaCallback;     /**< @brief Shadow v1 delta callback. */
    ShadowUpdatedCallback_t xUpdatedCallback; /**< @brief Shadow v1 updated callback. */
    StaticSemaphore_t xCallbackMutex;         /**< @brief Synchronizes Shadow callbacks and API functions. */
} ShadowClient_t;

/*-----------------------------------------------------------*/

/**
 * @brief Converts a Shadow v2 return code to a Shadow v1 return code.
 *
 * @param[in] xShadowError The Shadow v2 return code.
 *
 * @return An equivalent Shadow v1 return code.
 */
static ShadowReturnCode_t prvConvertReturnCode( AwsIotShadowError_t xShadowError );

/**
 * @brief Wrapper for Shadow v2 delta callbacks. Invokes the Shadow v1 callback.
 *
 * @param[in] pvArgument The associated #ShadowClient_t.
 * @param[in] pxUpdatedDocument Shadow v2 delta document info.
 */
static void prvDeltaCallbackWrapper( void * pvArgument,
                                     AwsIotShadowCallbackParam_t * const pxDeltaDocument );

/**
 * @brief Wrapper for Shadow v2 updated callbacks. Invokes the Shadow v1 callback.
 *
 * @param[in] pvArgument The associated #ShadowClient_t.
 * @param[in] pxUpdatedDocument Shadow v2 updated document info.
 */
static void prvUpdatedCallbackWrapper( void * pvArgument,
                                       AwsIotShadowCallbackParam_t * const pxUpdatedDocument );

/* Retrieves the MQTT v2 connection from the MQTT v1 connection handle. */
extern IotMqttConnection_t MQTT_AGENT_Getv2Connection( MQTTAgentHandle_t xMQTTHandle );

/*-----------------------------------------------------------*/

/**
 * @brief Counts the number of available Shadow clients.
 */
BaseType_t xAvailableShadowClients = shadowconfigMAX_CLIENTS;

/**
 * @brief Tracks whether the Shadow v2 library is initialized.
 */
BaseType_t xLibraryInitialized = pdFALSE;

/*-----------------------------------------------------------*/

static ShadowReturnCode_t prvConvertReturnCode( AwsIotShadowError_t xShadowError )
{
    ShadowReturnCode_t xStatus = eShadowUnknown;

    switch( xShadowError )
    {
        case AWS_IOT_SHADOW_SUCCESS:
            xStatus = eShadowSuccess;
            break;

        case AWS_IOT_SHADOW_TIMEOUT:
            xStatus = eShadowTimeout;
            break;

        case AWS_IOT_SHADOW_BAD_REQUEST:
            xStatus = eShadowRejectedBadRequest;
            break;

        case AWS_IOT_SHADOW_UNAUTHORIZED:
            xStatus = eShadowRejectedUnauthorized;
            break;

        case AWS_IOT_SHADOW_FORBIDDEN:
            xStatus = eShadowRejectedForbidden;
            break;

        case AWS_IOT_SHADOW_NOT_FOUND:
            xStatus = eShadowRejectedNotFound;
            break;

        case AWS_IOT_SHADOW_CONFLICT:
            xStatus = eShadowRejectedConflict;
            break;

        case AWS_IOT_SHADOW_TOO_LARGE:
            xStatus = eShadowRejectedPayloadTooLarge;
            break;

        case AWS_IOT_SHADOW_UNSUPPORTED:
            xStatus = eShadowRejectedUnsupportedMediaType;
            break;

        case AWS_IOT_SHADOW_TOO_MANY_REQUESTS:
            xStatus = eShadowRejectedTooManyRequests;
            break;

        case AWS_IOT_SHADOW_SERVER_ERROR:
            xStatus = eShadowRejectedInternalServerError;
            break;

        default:
            xStatus = eShadowFailure;
            break;
    }

    return xStatus;
}

/*-----------------------------------------------------------*/

static void prvDeltaCallbackWrapper( void * pvArgument,
                                     AwsIotShadowCallbackParam_t * const pxDeltaDocument )
{
    BaseType_t xCallbackReturn = pdFALSE;
    ShadowClient_t * pxShadowClient = ( ShadowClient_t * ) pvArgument;
    ShadowDeltaCallback_t xDeltaCallback = NULL;
    const char * pcCallbackThingName = NULL;
    char * pcDeltaDocument = NULL;

    /* Read the current delta callback. */
    ( void ) xSemaphoreTake( ( QueueHandle_t ) &( pxShadowClient->xCallbackMutex ), portMAX_DELAY );
    pcCallbackThingName = pxShadowClient->pcCallbackThingName;
    xDeltaCallback = pxShadowClient->xDeltaCallback;
    ( void ) xSemaphoreGive( ( QueueHandle_t ) &( pxShadowClient->xCallbackMutex ) );

    if( xDeltaCallback != NULL )
    {
        /* The delta document must be copied in case the user wants to take
         * ownership of it. */
        pcDeltaDocument = pvPortMalloc( pxDeltaDocument->u.callback.documentLength );

        if( pcDeltaDocument != NULL )
        {
            ( void ) memcpy( pcDeltaDocument,
                             pxDeltaDocument->u.callback.pDocument,
                             pxDeltaDocument->u.callback.documentLength );

            xCallbackReturn = xDeltaCallback( pxShadowClient,
                                              pcCallbackThingName,
                                              pcDeltaDocument,
                                              ( uint32_t ) pxDeltaDocument->u.callback.documentLength,
                                              ( MQTTBufferHandle_t ) pcDeltaDocument );

            if( xCallbackReturn == pdFALSE )
            {
                vPortFree( pcDeltaDocument );
            }
        }
    }
}

/*-----------------------------------------------------------*/

static void prvUpdatedCallbackWrapper( void * pvArgument,
                                       AwsIotShadowCallbackParam_t * const pxUpdatedDocument )
{
    BaseType_t xCallbackReturn = pdFALSE;
    ShadowClient_t * pxShadowClient = ( ShadowClient_t * ) pvArgument;
    ShadowUpdatedCallback_t xUpdatedCallback = NULL;
    const char * pcCallbackThingName = NULL;
    char * pcUpdatedDocument = NULL;

    /* Read the current updated callback. */
    ( void ) xSemaphoreTake( ( QueueHandle_t ) &( pxShadowClient->xCallbackMutex ), portMAX_DELAY );
    pcCallbackThingName = pxShadowClient->pcCallbackThingName;
    xUpdatedCallback = pxShadowClient->xUpdatedCallback;
    ( void ) xSemaphoreGive( ( QueueHandle_t ) &( pxShadowClient->xCallbackMutex ) );

    if( xUpdatedCallback != NULL )
    {
        /* The updated document must be copied in case the user wants to take
         * ownership of it. */
        pcUpdatedDocument = pvPortMalloc( pxUpdatedDocument->u.callback.documentLength );

        if( pcUpdatedDocument != NULL )
        {
            ( void ) memcpy( pcUpdatedDocument,
                             pxUpdatedDocument->u.callback.pDocument,
                             pxUpdatedDocument->u.callback.documentLength );

            xCallbackReturn = xUpdatedCallback( pxShadowClient,
                                                pcCallbackThingName,
                                                pcUpdatedDocument,
                                                ( uint32_t ) pxUpdatedDocument->u.callback.documentLength,
                                                ( MQTTBufferHandle_t ) pcUpdatedDocument );

            if( xCallbackReturn == pdFALSE )
            {
                vPortFree( pcUpdatedDocument );
            }
        }
    }
}

/*-----------------------------------------------------------*/

ShadowReturnCode_t SHADOW_ClientCreate( ShadowClientHandle_t * pxShadowClientHandle,
                                        const ShadowCreateParams_t * const pxShadowCreateParams )
{
    ShadowReturnCode_t xStatus = eShadowSuccess;
    ShadowClient_t * pxNewShadowClient = NULL;
    BaseType_t xInitializeLibrary = pdFALSE;

    /* Shared client mode is not supported. */
    if( pxShadowCreateParams->xMQTTClientType == eSharedMQTTClient )
    {
        Shadow_debug_printf( ( "Shared client is not supported in version 1. Please use version 4 API.\r\n" ) );
        xStatus = eShadowFailure;
    }

    /* Check for available Shadow clients. */
    if( xStatus == eShadowSuccess )
    {
        taskENTER_CRITICAL();

        if( xAvailableShadowClients == 0 )
        {
            xStatus = eShadowFailure;
        }
        else
        {
            xAvailableShadowClients--;
        }

        taskEXIT_CRITICAL();
    }

    if( xStatus == eShadowSuccess )
    {
        /* Allocate memory for new Shadow client. */
        pxNewShadowClient = pvPortMalloc( sizeof( ShadowClient_t ) );

        if( pxNewShadowClient == NULL )
        {
            Shadow_debug_printf( ( "Failed to allocate memory for new Shadow client.\r\n" ) );
            xStatus = eShadowFailure;

            taskENTER_CRITICAL();
            xAvailableShadowClients++;
            taskEXIT_CRITICAL();
        }
        else
        {
            /* Clear the Shadow client. */
            ( void ) memset( pxNewShadowClient, 0x00, sizeof( ShadowClient_t ) );

            /* Create the Shadow callback mutex. This function will not fail
             * when its parameter is not NULL. */
            ( void ) xSemaphoreCreateMutexStatic( &( pxNewShadowClient->xCallbackMutex ) );

            *pxShadowClientHandle = pxNewShadowClient;
        }
    }
    else
    {
        Shadow_debug_printf( ( "No available Shadow clients.\r\n" ) );
    }

    /* Create MQTT agent. */
    if( xStatus == eShadowSuccess )
    {
        if( MQTT_AGENT_Create( &( pxNewShadowClient->xMqttConnection ) ) != eMQTTAgentSuccess )
        {
            xStatus = eShadowFailure;

            taskENTER_CRITICAL();
            xAvailableShadowClients++;
            taskEXIT_CRITICAL();
        }
    }

    /* Initialize Shadow v2 library if needed. */
    if( xLibraryInitialized == pdFALSE )
    {
        taskENTER_CRITICAL();

        if( xLibraryInitialized == pdFALSE )
        {
            xInitializeLibrary = pdTRUE;
            xLibraryInitialized = pdTRUE;
        }

        taskEXIT_CRITICAL();

        if( ( xInitializeLibrary == pdTRUE ) &&
            ( ( AwsIotShadow_Init( shadowconfigCLEANUP_TIME_MS ) ) != AWS_IOT_SHADOW_SUCCESS ) )
        {
            xStatus = eShadowFailure;

            taskENTER_CRITICAL();
            xAvailableShadowClients++;
            taskEXIT_CRITICAL();
        }
    }

    /* Free allocated clients on failure. */
    if( ( xStatus == eShadowFailure ) && ( pxNewShadowClient != NULL ) )
    {
        if( pxNewShadowClient->xMqttConnection != NULL )
        {
            MQTT_AGENT_Delete( pxNewShadowClient->xMqttConnection );
        }

        vPortFree( pxNewShadowClient );
    }

    return xStatus;
}

/*-----------------------------------------------------------*/

ShadowReturnCode_t SHADOW_ClientConnect( ShadowClientHandle_t xShadowClientHandle,
                                         MQTTAgentConnectParams_t * const pxConnectParams,
                                         TickType_t xTimeoutTicks )
{
    ShadowReturnCode_t xStatus = eShadowSuccess;
    ShadowClient_t * pxShadowClient = ( ShadowClient_t * ) xShadowClientHandle;

    /* Connect MQTT agent. */
    if( MQTT_AGENT_Connect( pxShadowClient->xMqttConnection,
                            pxConnectParams,
                            xTimeoutTicks ) != eMQTTAgentSuccess )
    {
        Shadow_debug_printf( ( "Failed to establish MQTT connection for Shadow." ) );
        xStatus = eShadowFailure;
    }

    return xStatus;
}

/*-----------------------------------------------------------*/

ShadowReturnCode_t SHADOW_ClientDisconnect( ShadowClientHandle_t xShadowClientHandle )
{
    ShadowClient_t * pxShadowClient = ( ShadowClient_t * ) xShadowClientHandle;

    /* Timeout is not used. */
    MQTT_AGENT_Disconnect( pxShadowClient->xMqttConnection,
                           0 );

    return eShadowSuccess;
}

/*-----------------------------------------------------------*/

ShadowReturnCode_t SHADOW_ClientDelete( ShadowClientHandle_t xShadowClientHandle )
{
    ShadowClient_t * pxShadowClient = ( ShadowClient_t * ) xShadowClientHandle;
    BaseType_t xCleanupLibrary = pdFALSE;

    /* Delete MQTT agent handle. */
    MQTT_AGENT_Delete( pxShadowClient->xMqttConnection );

    /* Free Shadow client. */
    vPortFree( xShadowClientHandle );

    /* Increment number of available clients. */
    taskENTER_CRITICAL();
    xAvailableShadowClients++;

    /* Clean up Shadow library when all clients are deleted. */
    if( ( xAvailableShadowClients == shadowconfigMAX_CLIENTS ) &&
        ( xLibraryInitialized == pdTRUE ) )
    {
        xCleanupLibrary = pdTRUE;
        xLibraryInitialized = pdFALSE;
    }

    taskEXIT_CRITICAL();

    if( xCleanupLibrary == pdTRUE )
    {
        AwsIotShadow_Cleanup();
    }

    return eShadowSuccess;
}

/*-----------------------------------------------------------*/

ShadowReturnCode_t SHADOW_Update( ShadowClientHandle_t xShadowClientHandle,
                                  ShadowOperationParams_t * const pxUpdateParams,
                                  TickType_t xTimeoutTicks )
{
    AwsIotShadowError_t xShadowError = AWS_IOT_SHADOW_STATUS_PENDING;
    ShadowClient_t * pxShadowClient = ( ShadowClient_t * ) xShadowClientHandle;
    uint32_t xFlags = 0;
    AwsIotShadowDocumentInfo_t xUpdateDocument = AWS_IOT_SHADOW_DOCUMENT_INFO_INITIALIZER;

    /* Convert parameter structures. */
    xUpdateDocument.qos = ( IotMqttQos_t ) pxUpdateParams->xQoS;
    xUpdateDocument.pThingName = pxUpdateParams->pcThingName;
    xUpdateDocument.thingNameLength = strlen( pxUpdateParams->pcThingName );
    xUpdateDocument.u.update.pUpdateDocument = pxUpdateParams->pcData;
    xUpdateDocument.u.update.updateDocumentLength = ( size_t ) pxUpdateParams->ulDataLength;

    if( pxUpdateParams->ucKeepSubscriptions == 1 )
    {
        xFlags = AWS_IOT_SHADOW_FLAG_KEEP_SUBSCRIPTIONS;
    }

    /* Call the MQTT v2 blocking Shadow update function. */
    xShadowError = AwsIotShadow_TimedUpdate( MQTT_AGENT_Getv2Connection( pxShadowClient->xMqttConnection ),
                                             &xUpdateDocument,
                                             xFlags,
                                             shadowTICKS_TO_MS( xTimeoutTicks ) );

    return prvConvertReturnCode( xShadowError );
}

/*-----------------------------------------------------------*/

ShadowReturnCode_t SHADOW_Get( ShadowClientHandle_t xShadowClientHandle,
                               ShadowOperationParams_t * const pxGetParams,
                               TickType_t xTimeoutTicks )
{
    AwsIotShadowError_t xShadowError = AWS_IOT_SHADOW_STATUS_PENDING;
    ShadowClient_t * pxShadowClient = ( ShadowClient_t * ) xShadowClientHandle;
    uint32_t xFlags = 0;
    AwsIotShadowDocumentInfo_t xGetDocument = AWS_IOT_SHADOW_DOCUMENT_INFO_INITIALIZER;
    const char * pcRetrievedDocument = NULL;
    size_t xRetrievedDocumentLength = 0;

    /* Convert parameter structures. */
    xGetDocument.qos = ( IotMqttQos_t ) pxGetParams->xQoS;
    xGetDocument.pThingName = pxGetParams->pcThingName;
    xGetDocument.thingNameLength = strlen( pxGetParams->pcThingName );
    xGetDocument.u.get.mallocDocument = pvPortMalloc;

    if( pxGetParams->ucKeepSubscriptions == 1 )
    {
        xFlags = AWS_IOT_SHADOW_FLAG_KEEP_SUBSCRIPTIONS;
    }

    /* Call the MQTT v2 blocking Shadow get function. */
    xShadowError = AwsIotShadow_TimedGet( MQTT_AGENT_Getv2Connection( pxShadowClient->xMqttConnection ),
                                          &xGetDocument,
                                          xFlags,
                                          shadowTICKS_TO_MS( xTimeoutTicks ),
                                          &pcRetrievedDocument,
                                          &xRetrievedDocumentLength );

    /* Set output parameters. */
    if( xShadowError == AWS_IOT_SHADOW_SUCCESS )
    {
        pxGetParams->pcData = pcRetrievedDocument;
        pxGetParams->ulDataLength = ( uint32_t ) xRetrievedDocumentLength;
        pxGetParams->xBuffer = ( MQTTBufferHandle_t ) pcRetrievedDocument;
    }
    else
    {
        pxGetParams->pcData = NULL;
        pxGetParams->ulDataLength = 0;
        pxGetParams->xBuffer = NULL;
    }

    return prvConvertReturnCode( xShadowError );
}

/*-----------------------------------------------------------*/

ShadowReturnCode_t SHADOW_Delete( ShadowClientHandle_t xShadowClientHandle,
                                  ShadowOperationParams_t * const pxDeleteParams,
                                  TickType_t xTimeoutTicks )
{
    AwsIotShadowError_t xShadowError = AWS_IOT_SHADOW_STATUS_PENDING;
    ShadowClient_t * pxShadowClient = ( ShadowClient_t * ) xShadowClientHandle;
    uint32_t xFlags = 0;

    if( pxDeleteParams->ucKeepSubscriptions == 1 )
    {
        xFlags = AWS_IOT_SHADOW_FLAG_KEEP_SUBSCRIPTIONS;
    }

    /* Call the MQTT v2 blocking Shadow delete function. */
    xShadowError = AwsIotShadow_TimedDelete( MQTT_AGENT_Getv2Connection( pxShadowClient->xMqttConnection ),
                                             pxDeleteParams->pcThingName,
                                             strlen( pxDeleteParams->pcThingName ),
                                             xFlags,
                                             shadowTICKS_TO_MS( xTimeoutTicks ) );

    return prvConvertReturnCode( xShadowError );
}

/*-----------------------------------------------------------*/

ShadowReturnCode_t SHADOW_RegisterCallbacks( ShadowClientHandle_t xShadowClientHandle,
                                             ShadowCallbackParams_t * const pxCallbackParams,
                                             TickType_t xTimeoutTicks )
{
    AwsIotShadowError_t xShadowError = AWS_IOT_SHADOW_SUCCESS;
    ShadowClient_t * pxShadowClient = ( ShadowClient_t * ) xShadowClientHandle;
    size_t thingNameLength = strlen( pxCallbackParams->pcThingName );
    AwsIotShadowCallbackInfo_t callbackInfo = AWS_IOT_SHADOW_CALLBACK_INFO_INITIALIZER,
                               * pCallbackInfo = NULL;

    /* Shadow v2 does not use a timeout for setting callbacks. */
    ( void ) xTimeoutTicks;

    /* Set the callback functions in the Shadow client. */
    ( void ) xSemaphoreTake( ( QueueHandle_t ) &( pxShadowClient->xCallbackMutex ), portMAX_DELAY );
    pxShadowClient->pcCallbackThingName = pxCallbackParams->pcThingName;
    pxShadowClient->xDeletedCallback = pxCallbackParams->xShadowDeletedCallback;
    pxShadowClient->xDeltaCallback = pxCallbackParams->xShadowDeltaCallback;
    pxShadowClient->xUpdatedCallback = pxCallbackParams->xShadowUpdatedCallback;
    ( void ) xSemaphoreGive( ( QueueHandle_t ) &( pxShadowClient->xCallbackMutex ) );

    /* Set the callback parameter. */
    callbackInfo.pCallbackContext = pxShadowClient;

    /* Set the delta callback. */
    if( xShadowError == AWS_IOT_SHADOW_SUCCESS )
    {
        if( pxCallbackParams->xShadowDeltaCallback != NULL )
        {
            callbackInfo.function = prvDeltaCallbackWrapper;
            pCallbackInfo = &callbackInfo;
        }
        else
        {
            pCallbackInfo = NULL;
        }

        xShadowError = AwsIotShadow_SetDeltaCallback( MQTT_AGENT_Getv2Connection( pxShadowClient->xMqttConnection ),
                                                      pxCallbackParams->pcThingName,
                                                      thingNameLength,
                                                      0,
                                                      pCallbackInfo );
    }

    if( xShadowError == AWS_IOT_SHADOW_SUCCESS )
    {
        /* Set the updated callback. */
        if( pxCallbackParams->xShadowUpdatedCallback != NULL )
        {
            callbackInfo.function = prvUpdatedCallbackWrapper;
            pCallbackInfo = &callbackInfo;
        }
        else
        {
            pCallbackInfo = NULL;
        }

        xShadowError = AwsIotShadow_SetUpdatedCallback( MQTT_AGENT_Getv2Connection( pxShadowClient->xMqttConnection ),
                                                        pxCallbackParams->pcThingName,
                                                        thingNameLength,
                                                        0,
                                                        pCallbackInfo );
    }

    /* Deleted callback is not supported. */
    if( pxCallbackParams->xShadowDeletedCallback != NULL )
    {
        Shadow_debug_printf( ( "Deleted callback is not supported." ) );
    }

    return prvConvertReturnCode( xShadowError );
}

/*-----------------------------------------------------------*/

ShadowReturnCode_t SHADOW_ReturnMQTTBuffer( ShadowClientHandle_t xShadowClientHandle,
                                            MQTTBufferHandle_t xBufferHandle )
{
    /* Silence warnings about unused parameters. */
    ( void ) xShadowClientHandle;

    /* Free the Shadow buffer. */
    vPortFree( xBufferHandle );

    return eShadowSuccess;
}

/*-----------------------------------------------------------*/
