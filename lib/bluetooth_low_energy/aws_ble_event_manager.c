/*
 * Amazon FreeRTOS
 * Copyright (C) 2018 Amazon.com, Inc. or its affiliates.  All Rights Reserved.
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
 * @file aws_ble_event_manager.c
 * @brief Event manager for BLE, handles GATT and GAP events.
 */

#include <stdbool.h>
#include "FreeRTOS.h"
#include "semphr.h"
#include "queue.h"
#include "event_groups.h"
#include "aws_ble_event_manager.h"
#include "aws_ble_event_manager_config.h"

#define EVTMNG_NB_EVENT_GROUPS    ( ( ( EVTMNG_MAX_EVENTS - 1 ) / EVTMNG_MAX_EVENT_PER_GROUP ) + 1 )

typedef struct
{
    Link_t xEventList;
    void * pvEventParams;
    uint32_t ulEvent;
} event_t;

typedef struct
{
    Link_t xEventListHead;
    size_t xNbEvents;
    size_t xOffset;
} eventManager_t;

static int16_t xErrorFromEvent[ EVTMNG_MAX_EVENTS ];
static StaticEventGroup_t xSyncOnEvent[ EVTMNG_NB_EVENT_GROUPS ];
static size_t xEventListCounter = 0;
static eventManager_t xEventManager[ EVTMNG_MAX_EVT_MANAGERS ];
static StaticSemaphore_t xThreadSafetyMutex;

/*-----------------------------------------------------------*/

void prvGetEventGroupOffsetAndIndex( uint32_t ulHandle,
                                     uint16_t event,
                                     uint8_t * pucEventGroup,
                                     uint8_t * pucOffset,
                                     uint8_t * pucEventIndex )
{
    *pucEventIndex = xEventManager[ ulHandle ].xOffset + event;
    *pucEventGroup = ( *pucEventIndex ) / EVTMNG_MAX_EVENT_PER_GROUP;
    *pucOffset = ( *pucEventIndex ) - ( ( *pucEventGroup ) * EVTMNG_MAX_EVENT_PER_GROUP );
}

/*-----------------------------------------------------------*/

BaseType_t vEVTMNGRinitialize( void )
{
    BaseType_t xStatus = pdPASS;
    size_t xIndex;

    xSemaphoreCreateMutexStatic( &xThreadSafetyMutex );

    for( xIndex = 0; xIndex < EVTMNG_NB_EVENT_GROUPS; xIndex++ )
    {
        if( xEventGroupCreateStatic( &xSyncOnEvent[ xIndex ] ) == NULL )
        {
            xStatus = pdFAIL;
            break;
        }
    }

    return xStatus;
}

/*-----------------------------------------------------------*/

BaseType_t vEVTMNGRgetNewInstance( uint32_t * pulHandle,
                                   uint16_t ulNbEvents )
{
    BaseType_t xStatus = pdPASS;


    if( xSemaphoreTake( ( SemaphoreHandle_t ) &xThreadSafetyMutex, portMAX_DELAY ) == pdPASS )
    {
        if( xEventListCounter < EVTMNG_MAX_EVT_MANAGERS )
        {
            listINIT_HEAD( &xEventManager[ xEventListCounter ].xEventListHead );

            if( xEventListCounter == 0 )
            {
                xEventManager[ xEventListCounter ].xOffset = 0;
            }
            else
            {
                xEventManager[ xEventListCounter ].xOffset = xEventManager[ xEventListCounter - 1 ].xOffset + xEventManager[ xEventListCounter - 1 ].xNbEvents;
            }



            if( xEventManager[ xEventListCounter ].xOffset + ulNbEvents > EVTMNG_MAX_EVENTS )
            {
            	xEventManager[ xEventListCounter ].xNbEvents = 0;
            	*pulHandle = xEventListCounter;
                xStatus = pdFAIL;
            }else
            {
                xEventManager[ xEventListCounter ].xNbEvents = ulNbEvents;
                *pulHandle = xEventListCounter;
                xEventListCounter++;
            }


            xSemaphoreGive( ( SemaphoreHandle_t ) &xThreadSafetyMutex );
        }
        else
        {
            xStatus = pdFAIL;
        }
    }

    return xStatus;
}


/*-----------------------------------------------------------*/

BaseType_t vEVTMNGRsetEventParameters( uint32_t ulHandle,
                                       uint32_t ulEvent,
                                       void * pvEventParams )
{
    event_t * pxNewEvent;
    BaseType_t xStatus = pdPASS;

    pxNewEvent = pvPortMalloc( sizeof( event_t ) );

    if( pxNewEvent != NULL )
    {
        if( xSemaphoreTake( ( SemaphoreHandle_t ) &xThreadSafetyMutex, portMAX_DELAY ) == pdPASS )
        {
            pxNewEvent->ulEvent = ulEvent;
            pxNewEvent->pvEventParams = pvEventParams;
            listADD( &xEventManager[ ulHandle ].xEventListHead, &pxNewEvent->xEventList );

            xSemaphoreGive( ( SemaphoreHandle_t ) &xThreadSafetyMutex );
        }
    }
    else
    {
        xStatus = pdFAIL;
    }

    return xStatus;
}

/*-----------------------------------------------------------*/

void vEVTMNGRgetEventParameters( uint32_t ulHandle,
                                 uint32_t ulEvent,
                                 void ** ppvEventParams,
                                 bool * pbFoundEvent )
{
    Link_t * pxEventLink;
    event_t * pxEvent = NULL;

    *pbFoundEvent = false;

    if( ppvEventParams != NULL )
    {
        if( xSemaphoreTake( ( SemaphoreHandle_t ) &xThreadSafetyMutex, portMAX_DELAY ) == pdPASS )
        {
            listFOR_EACH( pxEventLink, &xEventManager[ ulHandle ].xEventListHead )
            {
                pxEvent = listCONTAINER( pxEventLink, event_t, xEventList );

                if( pxEvent->ulEvent == ulEvent )
                {
                    *pbFoundEvent = true;
                    break;
                }
            }

            if( *pbFoundEvent == true )
            {
                *ppvEventParams = pxEvent->pvEventParams;
                listREMOVE( &pxEvent->xEventList );
                ( void ) vPortFree( pxEvent );
            }

            xSemaphoreGive( ( SemaphoreHandle_t ) &xThreadSafetyMutex );
        }
    }
}

/*-----------------------------------------------------------*/

void vEVTMNGRremoveLastEvent( uint32_t ulHandle,
                              uint32_t ulEvent )
{
    void * pvEventParams = NULL;
    bool bFoundEvent;

    vEVTMNGRgetEventParameters( ulHandle, ulEvent, &pvEventParams, &bFoundEvent );

    if( pvEventParams == NULL )
    {
        ( void ) vPortFree( pvEventParams );
    }
}

/*-----------------------------------------------------------*/

int16_t vEVTMNGRwaitEvent( uint32_t ulHandle,
                           uint16_t event )
{
    uint16_t usBits;
    uint8_t ucEventGroup, ucOffset;
    uint8_t ucEventIndex;

    prvGetEventGroupOffsetAndIndex( ulHandle, event, &ucEventGroup, &ucOffset, &ucEventIndex );

    usBits = 1 << ucOffset;
    ( void ) xEventGroupWaitBits( &xSyncOnEvent[ ucEventGroup ], usBits, pdTRUE, pdTRUE, portMAX_DELAY );

    return xErrorFromEvent[ ucEventIndex ];
}

/*-----------------------------------------------------------*/

void vEVTMNGRsetEvent( uint32_t ulHandle,
                       uint16_t event,
                       int16_t status )
{
    uint8_t ucEventGroup, ucOffset;
    uint8_t ucEventIndex;

    prvGetEventGroupOffsetAndIndex( ulHandle, event, &ucEventGroup, &ucOffset, &ucEventIndex );

    xErrorFromEvent[ ucEventIndex ] = status;

    ( void ) xEventGroupSetBits( &xSyncOnEvent[ ucEventGroup ], 1 << ucOffset );
}
