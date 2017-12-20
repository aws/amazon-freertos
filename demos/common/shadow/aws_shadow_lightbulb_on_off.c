/*
 * Amazon FreeRTOS Shadow Demo V1.1.0
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
 * @file aws_shadow_lightbulb_on_off.c
 * @brief A simple shadow example.
 *
 * The simple Shadow lightbulb example to illustrate how client application and
 * things communicate with the Shadow service.
 */

/* Standard includes. */
#include <stdio.h>
#include <string.h>

/* FreeRTOS includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"

/* MQTT include. */
#include "aws_mqtt_agent.h"

/* Demo configurations. */
#include "aws_demo_config.h"

/* Required to get the broker address and port. */
#include "aws_clientcredential.h"
#include "queue.h"

/* Required for shadow API's */
#include "aws_shadow.h"
#include "jsmn.h"
/* Required for shadow demo */
#include "aws_shadow_lightbulb_on_off.h"

/** Task name. */
#define shadowDemoCHAR_TASK_NAME    "Shd-IOT-%d"
#define shadowDemoREPORT_JSON \
    "{"                       \
    "\"state\":{"             \
    "\"reported\":"           \
    "%s"                      \
    "}"                       \
    "}"

#define shadowDemoCREATE_JSON \
    "{"                       \
    "\"state\":{"             \
    "\"desired\":{"           \
    "\"%s\":\"red\""          \
    "},"                      \
    "\"reported\":{"          \
    "\"%s\":\"red\""          \
    "}"                       \
    "}"                       \
    "}"

#define shadowDemoTIMEOUT                    pdMS_TO_TICKS( 30000UL )

/** Max size for the name of tasks generated for Shadow.
 */
#define shadowDemoCHAR_TASK_NAME_MAX_SIZE    15

/** update frequency for color of lightbulb
 */
#define shadowDemoSEND_UPDATE_MS             pdMS_TO_TICKS( ( 5L * 1000UL ) )

/** name of the thing
 */
#define shadowDemoTHING_NAME                 clientcredentialIOT_THING_NAME

#define shadowDemoBUFFER_LENGTH              128

/** stack size for task that handles shadow delta and updates
 */
#define shadowDemoUPDATE_TASK_STACK_SIZE     ( ( uint16_t ) configMINIMAL_STACK_SIZE * ( uint16_t ) 5 )

#define shadowDemo_MAX_TOKENS                40
#define shadowDemo_SEND_QUEUE_WAIT_TICKS     3000
#define shadowDemo_RECV_QUEUE_WAIT_TICKS     500
#define shadowDemoUPDATE_QUEUE_LENGTH        democonfigSHADOW_DEMO_NUM_TASKS * 2

/**
 * @brief Could be local. Used in the call back.
 */
typedef struct
{
    uint32_t ulDataLength;
    char pcUpdateBuffer[ shadowDemoBUFFER_LENGTH ];
} xShadowQueueData_t;

static char cCopyString[ shadowDemoBUFFER_LENGTH ];
static jsmntok_t pxJSMNTokens[ shadowDemo_MAX_TOKENS ];

static ShadowClientHandle_t xClientHandle;

typedef struct
{
    char cBulbSate;
    char cTaskName[ shadowDemoCHAR_TASK_NAME_MAX_SIZE ];
    char pcUpdateBuffer[ shadowDemoBUFFER_LENGTH ];
} ShadowTaskParam_t;
static ShadowTaskParam_t xShadowTaskParamBuffer[ democonfigSHADOW_DEMO_NUM_TASKS ];

/**
 * @brief Called when there's a difference between "reported" and "desired" in Shadow document.
 */
static BaseType_t prvDeltaCallback( void * pvUserData,
                                    const char * const pcThingName,
                                    const char * const pcDeltaDocument,
                                    uint32_t ulDocumentLength,
                                    MQTTBufferHandle_t xBuffer );


static void prvShadowUpdateTasks( void * pvParameters );
static void prvShadowMainTask( void * pvParameters );

/**
 * @brief Updates the Thing Shadow when notified.
 */
static void prvUpdateTask( void * pvParameters );

/**
 * Called when shadow service received UPDATE request and updated thing shadow
 */
static BaseType_t prvUpdatedCallback( void * pvUserData,
                                      const char * const pcThingName,
                                      const char * const pcDeltaDocument,
                                      uint32_t ulDocumentLength,
                                      MQTTBufferHandle_t xBuffer );

/**
 * @brief Generate initial shadow document
 */
static uint32_t prvGenerateShadowJSON( const ShadowTaskParam_t * pxShadowTaskParam );

/**
 * @brief Handle of the update queue used to pass updates to shadow
 */
static QueueHandle_t xUpdateQueue = NULL;

/**
 * @brief Called when thing shadow is deleted
 */
static void prvDeletedCallback( void * pvUserData,
                                const char * const pcThingName );

/**
 * @brief client(client app, aws console, here ShadowMainTask )
 * updates the desired state of lightbulb
 */
static uint32_t prvUpdateShadowJSON( ShadowTaskParam_t * pxShadowTaskParam );

/**
 * @brief Initialize Shadow Client.
 */
static ShadowReturnCode_t prvShadowClientInit( void );
static BaseType_t prvGGDJsoneq( const char * pcJson,     /*lint !e971 can use char without signed/unsigned. */
                                const jsmntok_t * const pxTok,
                                const char * pcString ); /*lint !e971 can use char without signed/unsigned. */
static void prvShadowMainTask( void * pvParameters );
/*-----------------------------------------------------------*/

static BaseType_t prvGGDJsoneq( const char * pcJson,    /*lint !e971 can use char without signed/unsigned. */
                                const jsmntok_t * const pxTok,
                                const char * pcString ) /*lint !e971 can use char without signed/unsigned. */
{
    uint32_t ulStringSize = ( uint32_t ) pxTok->end - ( uint32_t ) pxTok->start;
    BaseType_t xStatus = pdFALSE;

    if( pxTok->type == JSMN_STRING )
    {
        if( ( uint32_t ) strlen( pcString ) == ulStringSize )
        {
            if( ( int16_t ) strncmp( &pcJson[ pxTok->start ],
                                     pcString,
                                     ulStringSize ) == 0 )
            {
                xStatus = pdTRUE;
            }
        }
    }

    return xStatus;
}


/*-----------------------------------------------------------*/

static BaseType_t prvDeltaCallback( void * pvUserData,
                                    const char * const pcThingName,
                                    const char * const pcDeltaDocument,
                                    uint32_t ulDocumentLength,
                                    MQTTBufferHandle_t xBuffer )
{
    uint16_t usTokenIndex;
    int32_t lNbTokens;
    int32_t lStringSize;
    BaseType_t xMatchState;
    static jsmn_parser xJSMNParser;
    xShadowQueueData_t xShadowQueueData;

    ( void ) pvUserData;
    ( void ) xBuffer;
    ( void ) ulDocumentLength;
    ( void ) pcThingName;
    ( void ) pcDeltaDocument;

    jsmn_init( &xJSMNParser );

    lNbTokens = ( int32_t ) jsmn_parse( &xJSMNParser,
                                        pcDeltaDocument,
                                        ( size_t ) ulDocumentLength,
                                        pxJSMNTokens,
                                        ( unsigned int ) shadowDemo_MAX_TOKENS );

    if( lNbTokens > 0 )
    {
        for( usTokenIndex = 0; usTokenIndex < ( uint16_t ) lNbTokens; usTokenIndex++ )
        {
            xMatchState = prvGGDJsoneq( pcDeltaDocument, &pxJSMNTokens[ usTokenIndex ], "state" );

            if( xMatchState == pdTRUE )
            {
                lStringSize = pxJSMNTokens[ usTokenIndex + ( uint16_t ) 1 ].end
                              - pxJSMNTokens[ usTokenIndex + ( uint16_t ) 1 ].start;
                configASSERT( lStringSize < shadowDemoBUFFER_LENGTH );
                memcpy( cCopyString,
                        &pcDeltaDocument[ pxJSMNTokens[ usTokenIndex + ( uint16_t ) 1 ].start ],
                        ( size_t ) lStringSize );
                cCopyString[ lStringSize ] = ( char ) '\0';
                xShadowQueueData.ulDataLength = ( uint32_t ) snprintf( xShadowQueueData.pcUpdateBuffer, shadowDemoBUFFER_LENGTH, shadowDemoREPORT_JSON, cCopyString );

                configASSERT( xQueueSendToBack( xUpdateQueue, &xShadowQueueData, shadowDemo_SEND_QUEUE_WAIT_TICKS ) == pdPASS );
                /*thing updates the device state in shadow */

                break;
            }
        }
    }

    return pdFALSE;
}

/*-----------------------------------------------------------*/

static void prvUpdateTask( void * pvParameters )
{
    ShadowOperationParams_t xUpdateParams;
    xShadowQueueData_t xShadowQueueData;

    ( void ) pvParameters;

    xUpdateParams.pcThingName = shadowDemoTHING_NAME;
    xUpdateParams.xQoS = eMQTTQoS0;
    xUpdateParams.pcData = xShadowQueueData.pcUpdateBuffer;
    /* subscribe to the accepted and rejected topics */
    xUpdateParams.ucKeepSubscriptions = pdTRUE;

    for( ;; )
    {
        if( xQueueReceive( xUpdateQueue, &xShadowQueueData, shadowDemo_RECV_QUEUE_WAIT_TICKS ) != pdFALSE )
        {
            /* We were able to obtain the semaphore and can now access the
             * shared resource. */

            configPRINTF( ( "Performing thing update.\r\n" ) );
            xUpdateParams.ulDataLength = xShadowQueueData.ulDataLength;

            ( void ) SHADOW_Update( xClientHandle, &xUpdateParams, shadowDemoTIMEOUT );

            configPRINTF( ( "Performing thing update complete.\r\n" ) );
        }
    }
}
/*-----------------------------------------------------------*/

static BaseType_t prvUpdatedCallback( void * pvUserData,
                                      const char * const pcThingName,
                                      const char * const pcDeltaDocument,
                                      uint32_t ulDocumentLength,
                                      MQTTBufferHandle_t xBuffer )
{
    ( void ) pvUserData;
    ( void ) pcThingName;
    ( void ) pcDeltaDocument;
    ( void ) ulDocumentLength;
    ( void ) xBuffer;

    return pdFALSE;
}
/*-----------------------------------------------------------*/

static void prvDeletedCallback( void * pvUserData,
                                const char * const pcThingName )
{
    ( void ) pvUserData;
    ( void ) pcThingName;
}

/*-----------------------------------------------------------*/

static uint32_t prvGenerateShadowJSON( const ShadowTaskParam_t * pxShadowTaskParam )
{
    /*init shadow document with settings desired and reported color of light bulb. */

    return ( uint32_t ) snprintf( ( char * ) pxShadowTaskParam->pcUpdateBuffer,
                                  shadowDemoBUFFER_LENGTH,
                                  shadowDemoCREATE_JSON,
                                  pxShadowTaskParam->cTaskName,
                                  pxShadowTaskParam->cTaskName ); /*lint !e9005 safe cast, lint will complain depending on snprintf implementation. */
}
/*-----------------------------------------------------------*/

static uint32_t prvUpdateShadowJSON( ShadowTaskParam_t * pxShadowTaskParam )
{
    /*toggle color of lightbulb on every update  */
    if( pxShadowTaskParam->cBulbSate == ( char ) 0 )
    {
        pxShadowTaskParam->cBulbSate = ( char ) 1;
    }
    else
    {
        pxShadowTaskParam->cBulbSate = ( char ) 0;
    }

    if( pxShadowTaskParam->cBulbSate != ( char ) 0 )
    {
        return ( uint32_t ) snprintf( ( char * ) pxShadowTaskParam->pcUpdateBuffer, shadowDemoBUFFER_LENGTH,
                                      "{"
                                      "\"state\":{"
                                      "\"desired\":{"
                                      "\"%s\":\"green\""
                                      "}"
                                      "}"
                                      "}",
                                      pxShadowTaskParam->cTaskName );
    }
    else
    {
        return ( uint32_t ) snprintf( ( char * ) pxShadowTaskParam->pcUpdateBuffer, shadowDemoBUFFER_LENGTH,
                                      "{"
                                      "\"state\":{"
                                      "\"desired\":{"
                                      "\"%s\":\"red\""
                                      "}"
                                      "}"
                                      "}",
                                      pxShadowTaskParam->cTaskName );
    }
}
/*-----------------------------------------------------------*/

static ShadowReturnCode_t prvShadowClientInit( void )
{
    ShadowCreateParams_t sCreateParams;
    MQTTAgentConnectParams_t sConnectParams;
    ShadowCallbackParams_t sCallbackParams;
    ShadowReturnCode_t sReturn;

    sCreateParams.xMQTTClientType = eDedicatedMQTTClient; /*_RB_ Why does this have to be dedicated?  What is the effect on other demos that might get built at the same time? */
                                                          /*ToDo: sub manager? */
    sReturn = SHADOW_ClientCreate( &xClientHandle, &sCreateParams );

    if( sReturn == eShadowSuccess )
    {
        memset( &sConnectParams, 0x00, sizeof( sConnectParams ) );
        sConnectParams.pcURL = clientcredentialMQTT_BROKER_ENDPOINT;
        sConnectParams.xURLIsIPAddress = pdFALSE;
        sConnectParams.usPort = clientcredentialMQTT_BROKER_PORT;

        sConnectParams.xSecuredConnection = pdTRUE;
        sConnectParams.pcCertificate = NULL;
        sConnectParams.ulCertificateSize = 0;
        sConnectParams.pxCallback = NULL;
        sConnectParams.pvUserData = &xClientHandle;

        sConnectParams.pucClientId = ( const uint8_t * ) ( clientcredentialIOT_THING_NAME );
        sConnectParams.usClientIdLength = ( uint16_t ) strlen( clientcredentialIOT_THING_NAME );
        sReturn = SHADOW_ClientConnect( xClientHandle,
                                        &sConnectParams,
                                        shadowDemoTIMEOUT );

        if( sReturn == eShadowSuccess )
        {
            sCallbackParams.pcThingName = shadowDemoTHING_NAME;
            sCallbackParams.xShadowUpdatedCallback = prvUpdatedCallback;
            sCallbackParams.xShadowDeletedCallback = prvDeletedCallback;
            sCallbackParams.xShadowDeltaCallback = prvDeltaCallback;
            sReturn = SHADOW_RegisterCallbacks( xClientHandle,
                                                &sCallbackParams,
                                                shadowDemoTIMEOUT );

            if( sReturn != eShadowSuccess )
            {
                configPRINTF( ( "Shadow_RegisterCallbacks unsuccessful, returned %d.\r\n", sReturn ) );
            }
        }
        else
        {
            configPRINTF( ( "Shadow_ClientConnect unsuccessful, returned %d.\r\n", sReturn ) );
        }
    }
    else
    {
        configPRINTF( ( "Shadow_ClientCreate unsuccessful, returned %d.\r\n", sReturn ) );
    }

    return sReturn;
}
/*-----------------------------------------------------------*/

static void prvShadowUpdateTasks( void * pvParameters )
{
    TickType_t xLastWakeTime;
    ShadowTaskParam_t * pxShadowTaskParam;
    ShadowOperationParams_t xOperationParams;

    pxShadowTaskParam = ( ShadowTaskParam_t * ) pvParameters; /*lint !e9087 Safe cast from context. */
    pxShadowTaskParam->cBulbSate = ( char ) 0;

    xOperationParams.pcThingName = shadowDemoTHING_NAME;
    xOperationParams.xQoS = eMQTTQoS0;
    xOperationParams.pcData = pxShadowTaskParam->pcUpdateBuffer;
    /* subscribe to the accepted and rejected topics */
    xOperationParams.ucKeepSubscriptions = pdTRUE;
    xOperationParams.ulDataLength = prvGenerateShadowJSON( pxShadowTaskParam );

    ( void ) SHADOW_Update( xClientHandle,
                            &xOperationParams,
                            shadowDemoTIMEOUT );

    xLastWakeTime = xTaskGetTickCount();

    configPRINTF( ( "Shadow Demo initialized.\r\n" ) );

    /* keep changing state of light bulb periodically as per duration set in shadowSendUpdateMs*/
    for( ;; )
    {
        configPRINTF( ( "Client request to change color of light bulb.\r\n" ) );
        xOperationParams.ulDataLength = prvUpdateShadowJSON( pxShadowTaskParam );

        ( void ) SHADOW_Update( xClientHandle,
                                &xOperationParams,
                                shadowDemoTIMEOUT );

        configPRINTF( ( "Client change done in thing shadow.\r\n" ) );
        vTaskDelayUntil( &xLastWakeTime, shadowDemoSEND_UPDATE_MS );
    }
}
/*-----------------------------------------------------------*/

static void prvShadowMainTask( void * pvParameters )
{
    uint8_t x;
    /* The variable used to hold the queue's data structure. */
    static StaticQueue_t xStaticQueue;
    ShadowReturnCode_t xStatus;
    ShadowOperationParams_t xOperationParams;

    /* The array to use as the queue's storage area.  This must be at least
     * uxQueueLength * uxItemSize bytes.  Again, must be static. */
    static uint8_t ucQueueStorageArea[ shadowDemoUPDATE_QUEUE_LENGTH * sizeof( xShadowQueueData_t ) ];

    ( void ) pvParameters;
    xUpdateQueue = xQueueCreateStatic( shadowDemoUPDATE_QUEUE_LENGTH, sizeof( xShadowQueueData_t ), ucQueueStorageArea, &xStaticQueue );

    xStatus = prvShadowClientInit();

    if( xStatus == eShadowSuccess )
    {
        xOperationParams.pcThingName = shadowDemoTHING_NAME;
        xOperationParams.xQoS = eMQTTQoS0;
        xOperationParams.pcData = NULL;
        /* subscribe to the accepted and rejected topics */
        xOperationParams.ucKeepSubscriptions = pdTRUE;

        /*create initial shadow document for the thing with default color for the light bulb as red.*/
        xStatus = SHADOW_Delete( xClientHandle,
                                 &xOperationParams,
                                 shadowDemoTIMEOUT );
    }

    if( xStatus == eShadowSuccess )
    {
        /* Create the update task which will handle the delta in shadow */
        ( void ) xTaskCreate( prvUpdateTask,
                              "ShDemoUpdt",
                              shadowDemoUPDATE_TASK_STACK_SIZE,
                              NULL,
                              tskIDLE_PRIORITY,
                              NULL );

        /* Create the Shadow demo task which connects the modify Shadow. */
        for( x = 0; x < ( uint8_t ) democonfigSHADOW_DEMO_NUM_TASKS; x++ )
        {
            ( void ) snprintf( ( char * ) ( &( xShadowTaskParamBuffer[ x ] ) )->cTaskName,
                               shadowDemoCHAR_TASK_NAME_MAX_SIZE,
                               shadowDemoCHAR_TASK_NAME,
                               x );
            ( void ) xTaskCreate( prvShadowUpdateTasks,
                                  ( const char * ) ( &( xShadowTaskParamBuffer[ x ] ) )->cTaskName,
                                  democonfigSHADOW_LIGHTBULB_TASK_STACK_SIZE,
                                  &( xShadowTaskParamBuffer[ x ] ),
                                  democonfigSHADOW_LIGHTBULB_TASK_PRIORIRY,
                                  NULL );
        }
    }

    vTaskDelete( NULL );
}

/* Create the shadow demo main task which will act as a client application to
 * request periodic change in state(color) of light bulb  */
void vStartShadowDemoTasks( void )
{
    ( void ) xTaskCreate( prvShadowMainTask,
                          "MainDemoTask",
                          shadowDemoUPDATE_TASK_STACK_SIZE,
                          NULL,
                          tskIDLE_PRIORITY,
                          NULL );
}
/*-----------------------------------------------------------*/
