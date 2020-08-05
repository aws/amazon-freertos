/*
 * FreeRTOS MQTT SHIM V1.0.0
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
 * http://aws.amazon.com/freertos
 * http ://www.FreeRTOS.org
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

bool IotMutex_CreateNonRecursive( StaticSemaphore_t * pMutex )
{
    bool status = false;

    IotMqtt_Assert( pMutex != NULL );

    if( ( xSemaphoreCreateMutexStatic( ( SemaphoreHandle_t ) pMutex ) != NULL ) )
    {
        status = true;
    }
    else
    {
        status = false;
    }

    return status;
}

bool IotMutex_CreateRecursive( StaticSemaphore_t * pMutex )
{
    bool status = false;

    IotMqtt_Assert( pMutex != NULL );

    if( ( xSemaphoreCreateRecursiveMutexStatic( ( SemaphoreHandle_t ) pMutex ) != NULL ) )
    {
        status = true;
    }
    else
    {
        status = false;
    }

    return status;
}

bool IotMutex_Take( StaticSemaphore_t * pMutex )
{
    bool status = false;

    IotMqtt_Assert( pMutex != NULL );

    if( ( xSemaphoreTake( ( SemaphoreHandle_t ) pMutex, portMAX_DELAY ) ) == pdTRUE )
    {
        status = true;
    }
    else
    {
        IotLogError( "Failed to take lock on mutex within specified time." );
        status = false;
    }

    return status;
}

bool IotMutex_Give( StaticSemaphore_t * pMutex )
{
    bool status = false;

    IotMqtt_Assert( pMutex != NULL );

    if( ( xSemaphoreGive( ( SemaphoreHandle_t ) pMutex ) ) == pdTRUE )
    {
        status = true;
    }
    else
    {
        IotLogError( "Failed to unlock mutex. Semaphores are implemented using queues."
                     "An error occured due to no space on the queue to post a message." );
        status = false;
    }

    return status;
}

bool IotMutex_TakeRecursive( StaticSemaphore_t * pMutex )
{
    bool status = false;

    IotMqtt_Assert( pMutex != NULL );

    if( ( xSemaphoreTakeRecursive( ( SemaphoreHandle_t ) pMutex, portMAX_DELAY ) ) == pdTRUE )
    {
        status = true;
    }
    else
    {
        IotLogError( "Failed to take lock on mutex within specified time." );
        status = false;
    }

    return status;
}

bool IotMutex_GiveRecursive( StaticSemaphore_t * pMutex )
{
    bool status = false;

    IotMqtt_Assert( pMutex != NULL );

    if( ( xSemaphoreGiveRecursive( ( SemaphoreHandle_t ) pMutex ) ) == pdTRUE )
    {
        status = true;
    }
    else
    {
        IotLogError( "Failed to unlock mutex. Semaphores are implemented using queues."
                     "An error occured due to no space on the queue to post a message." );
        status = false;
    }

    return status;
}

void IotMutex_Delete( StaticSemaphore_t * pMutex )
{
    IotMqtt_Assert( pMutex != NULL );
    vSemaphoreDelete( ( SemaphoreHandle_t ) pMutex );
}
