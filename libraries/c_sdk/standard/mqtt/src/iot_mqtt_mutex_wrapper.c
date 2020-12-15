/*
 * FreeRTOS MQTT V2.3.1
 * Copyright(C) 2020 Amazon.com, Inc. or its affiliates.All Rights Reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of
 * this softwareand associated documentation files(the "Software"), to deal in
 * the Software without restriction, including without limitation the rights to
 * use, copy, modify, merge, publish, distribute, sublicense, and /or sell copies of
 * the Software, and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions :
 *
 * The above copyright noticeand this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE AUTHORS OR
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 * https://github.com/freertos
 * https://www.FreeRTOS.org
 */

/**
 * @file iot_mqtt_mutex_wrapper.c
 * @brief Implements wrapper for mutexes in the shim.
 */
/* The config header is always included first. */
#include "iot_config.h"

/* Standard includes. */
#include <string.h>

/* Error handling include. */
#include "private/iot_error.h"

/* MQTT internal includes. */
#include "private/iot_mqtt_internal.h"

#include "semphr.h"

/*-----------------------------------------------------------*/

bool IotMutex_CreateNonRecursiveMutex( SemaphoreHandle_t * pMutex,
                                       StaticSemaphore_t * pxMutexBuffer )
{
    bool status = false;

    IotMqtt_Assert( pMutex != NULL );
    IotMqtt_Assert( pxMutexBuffer != NULL );

    /* Create a static mutex. */
    *pMutex = xSemaphoreCreateMutexStatic( pxMutexBuffer );

    if( *pMutex != NULL )
    {
        /* Mutex created successfully. */
        status = true;
    }
    else
    {
        IotLogError( "Failed to create mutex for new connection, no memory is available." );
    }

    return status;
}

bool IotMutex_CreateRecursiveMutex( SemaphoreHandle_t * pMutex,
                                    StaticSemaphore_t * pxMutexBuffer )
{
    bool status = false;

    IotMqtt_Assert( pMutex != NULL );
    IotMqtt_Assert( pxMutexBuffer != NULL );

    /* Create a recursive static mutex. */
    *pMutex = xSemaphoreCreateRecursiveMutexStatic( pxMutexBuffer );

    if( *pMutex != NULL )
    {
        /* Mutex created successfully. */
        status = true;
    }
    else
    {
        IotLogError( "Failed to create mutex for new connection, no memory is available." );
    }

    return status;
}

bool IotMutex_Take( SemaphoreHandle_t * pMutex )
{
    bool status = false;

    IotMqtt_Assert( pMutex != NULL );

    if( ( xSemaphoreTake( *pMutex, portMAX_DELAY ) ) == pdTRUE )
    {
        /* Mutex granted successfully. */
        status = true;
    }
    else
    {
        /* Time expired without the mutex becoming available. */
        IotLogError( "Failed to take lock on mutex within specified time." );
    }

    return status;
}

bool IotMutex_Give( SemaphoreHandle_t * pMutex )
{
    bool status = false;

    IotMqtt_Assert( pMutex != NULL );

    if( ( xSemaphoreGive( *pMutex ) ) == pdTRUE )
    {
        /* Mutex given back successfully. */
        status = true;
    }
    else
    {
        /* Mutex cannot be given back as no space is avaliable on the queue to post the message. */
        IotLogError( "Failed to unlock mutex. Semaphores are implemented using queues."
                     "An error occurred due to no space on the queue to post a message." );
    }

    return status;
}

bool IotMutex_TakeRecursive( SemaphoreHandle_t * pMutex )
{
    bool status = false;

    IotMqtt_Assert( pMutex != NULL );

    if( ( xSemaphoreTakeRecursive( *pMutex, portMAX_DELAY ) ) == pdTRUE )
    {
        /* Mutex granted successfully. */
        status = true;
    }
    else
    {
        /* Time expired without the mutex becoming available. */
        IotLogError( "Failed to take lock on mutex within specified time." );
    }

    return status;
}

bool IotMutex_GiveRecursive( SemaphoreHandle_t * pMutex )
{
    bool status = false;

    IotMqtt_Assert( pMutex != NULL );

    if( ( xSemaphoreGiveRecursive( *pMutex ) ) == pdTRUE )
    {
        /* Mutex given back successfully. */
        status = true;
    }
    else
    {
        /* Mutex cannot be given back as no space is avaliable on the queue to post the message. */
        IotLogError( "Failed to unlock mutex. Semaphores are implemented using queues."
                     "An error occurred due to no space on the queue to post a message." );
    }

    return status;
}

void IotMutex_Delete( SemaphoreHandle_t * pMutex )
{
    IotMqtt_Assert( pMutex != NULL );
    /* Deleting the mutex. */
    vSemaphoreDelete( *pMutex );
}
