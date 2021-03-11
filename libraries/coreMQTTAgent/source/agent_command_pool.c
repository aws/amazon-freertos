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
 * https://www.FreeRTOS.org
 * https://aws.amazon.com/freertos
 *
 */

/**
 * @file agent_command_pool.c
 * @brief Implements functions to obtain and release commands.
 */

/* Standard includes. */
#include <string.h>
#include <stdio.h>

/* Kernel includes. */
#include "FreeRTOS.h"
#include "semphr.h"

/* Header include. */
#include "agent_command_pool.h"

/*-----------------------------------------------------------*/

#define SEMAPHORE_NOT_INITIALIZED    ( 0U )
#define SEMAPHORE_INIT_PENDING       ( 1U )
#define SEMAPHORE_INITIALIZED        ( 2U )

/**
 * @brief The pool of command structures used to hold information on commands (such
 * as PUBLISH or SUBSCRIBE) between the command being created by an API call and
 * completion of the command by the execution of the command's callback.
 */
static Command_t commandStructurePool[ MQTT_COMMAND_CONTEXTS_POOL_SIZE ];

/**
 * @brief A counting semaphore used to guard the pool of Command_t structures.  To
 * obtain a structure first decrement the semaphore count.  To return a structure
 * increment the semaphore count after the structure is back in the pool.
 */
static SemaphoreHandle_t freeCommandStructMutex = NULL;

static volatile uint8_t initStatus = SEMAPHORE_NOT_INITIALIZED;

/*-----------------------------------------------------------*/

static void initializePool()
{
    bool owner = false;

    taskENTER_CRITICAL();
    {
        if( initStatus == SEMAPHORE_NOT_INITIALIZED )
        {
            owner = true;
            initStatus = SEMAPHORE_INIT_PENDING;
        }
    }
    taskEXIT_CRITICAL();

    if( owner )
    {
        memset( ( void * ) commandStructurePool, 0x00, sizeof( commandStructurePool ) );
        freeCommandStructMutex = xSemaphoreCreateCounting( MQTT_COMMAND_CONTEXTS_POOL_SIZE, MQTT_COMMAND_CONTEXTS_POOL_SIZE );
        initStatus = SEMAPHORE_INITIALIZED;
    }
}

/*-----------------------------------------------------------*/

Command_t * Agent_GetCommand( uint32_t blockTimeMs )
{
    Command_t * structToUse = NULL;
    size_t i;

    /* Check here so we do not enter a critical section every time. */
    if( initStatus == SEMAPHORE_NOT_INITIALIZED )
    {
        initializePool();
        configASSERT( freeCommandStructMutex ); /*_RB_ Create all objects here statically. */
    }

    /* Check counting semaphore has been created. */
    if( freeCommandStructMutex != NULL )
    {
        /* If the semaphore count is not zero then a command context is available. */
        if( xSemaphoreTake( freeCommandStructMutex, pdMS_TO_TICKS( blockTimeMs ) ) == pdPASS )
        {
            for( i = 0; i < MQTT_COMMAND_CONTEXTS_POOL_SIZE; i++ )
            {
                taskENTER_CRITICAL();
                {
                    /* If the commandType is NONE then the structure is not in use. */
                    if( commandStructurePool[ i ].commandType == NONE )
                    {
                        structToUse = &( commandStructurePool[ i ] );

                        /* To show the struct is no longer available to be returned
                         * by calls to Agent_ReleaseCommand(). */
                        structToUse->commandType = !NONE;
                        taskEXIT_CRITICAL();
                        break;
                    }
                }
                taskEXIT_CRITICAL();
            }
        }
    }

    return structToUse;
}

/*-----------------------------------------------------------*/

bool Agent_ReleaseCommand( Command_t * pCommandToRelease )
{
    size_t i;
    bool structReturned = false;

    /* See if the structure being returned is actually from the pool. */
    for( i = 0; i < MQTT_COMMAND_CONTEXTS_POOL_SIZE; i++ )
    {
        if( pCommandToRelease == &( commandStructurePool[ i ] ) )
        {
            taskENTER_CRITICAL();

            /* Yes its from the pool.  Clearing it to zero not only removes the old
             * data it also sets the structure's commandType parameter to NONE to
             * mark the structure as free again. */
            memset( ( void * ) pCommandToRelease, 0x00, sizeof( Command_t ) );
            taskEXIT_CRITICAL();

            /* Give back the counting semaphore after returning the structure so the
             * semaphore count equals the number of available structures. */
            xSemaphoreGive( freeCommandStructMutex );
            structReturned = true;

            LogDebug( ( "Returned Command Context %d to pool", ( int ) i ) );

            break;
        }
    }

    return structReturned;
}
