/*
 * FreeRTOS V202012.00
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
 * @file freertos_command_pool.c
 * @brief Implements functions to obtain and release commands.
 */

/* Standard includes. */
#include <string.h>
#include <stdio.h>

/* Kernel includes. */
#include "FreeRTOS.h"
#include "semphr.h"

/* Header include. */
#include "freertos_command_pool.h"
#include "freertos_agent_message.h"

/*-----------------------------------------------------------*/

#define QUEUE_NOT_INITIALIZED    ( 0U )
#define QUEUE_INITIALIZED        ( 1U )

/**
 * @brief The pool of command structures used to hold information on commands (such
 * as PUBLISH or SUBSCRIBE) between the command being created by an API call and
 * completion of the command by the execution of the command's callback.
 */
static Command_t xCommandStructurePool[ MQTT_COMMAND_CONTEXTS_POOL_SIZE ];

/**
 * @brief The message context used to guard the pool of Command_t structures.
 * For FreeRTOS, this is implemented with a queue. Structures may be
 * obtained by receiving a pointer from the queue, and returned by
 * sending the pointer back into it.
 */
static AgentMessageContext_t xCommandStructMessageCtx;

/**
 * @brief Initialization status of the queue.
 */
static volatile uint8_t ucInitStatus = QUEUE_NOT_INITIALIZED;

/*-----------------------------------------------------------*/

void Agent_InitializePool( void )
{
    size_t xCommandPoolSize;
    Command_t * pxCommand;
    static uint8_t ucStaticQueueStorageArea[ MQTT_COMMAND_CONTEXTS_POOL_SIZE * sizeof( Command_t * ) ];
    static StaticQueue_t xStaticQueueStructure;
    bool bCommandAdded = false;

    if( ucInitStatus == QUEUE_NOT_INITIALIZED )
    {
        memset( ( void * ) xCommandStructurePool, 0x00, sizeof( xCommandStructurePool ) );
        xCommandStructMessageCtx.queue = xQueueCreateStatic( MQTT_COMMAND_CONTEXTS_POOL_SIZE,
                                                             sizeof( Command_t * ),
                                                             ucStaticQueueStorageArea,
                                                             &xStaticQueueStructure );
        configASSERT( xCommandStructMessageCtx.queue );

        /* Populate the queue. */
        for( xCommandPoolSize = 0; xCommandPoolSize < MQTT_COMMAND_CONTEXTS_POOL_SIZE; xCommandPoolSize++ )
        {
            /* Store the address as a variable. */
            pxCommand = &xCommandStructurePool[ xCommandPoolSize ];
            /* Send the pointer to the queue. */
            bCommandAdded = Agent_MessageSend( &xCommandStructMessageCtx, &pxCommand, 0U );
            configASSERT( bCommandAdded );
        }

        ucInitStatus = QUEUE_INITIALIZED;
    }
}

/*-----------------------------------------------------------*/

Command_t * Agent_GetCommand( uint32_t ulBlockTimeMs )
{
    Command_t * pxCommand = NULL;
    bool bStructRetrieved = false;

    /* Check queue has been created. */
    configASSERT( ucInitStatus == QUEUE_INITIALIZED );

    /* Retrieve a struct from the queue. */
    bStructRetrieved = Agent_MessageReceive( &xCommandStructMessageCtx, &( pxCommand ), ulBlockTimeMs );

    if( !bStructRetrieved )
    {
        LogError( ( "No command structure available." ) );
    }

    return pxCommand;
}

/*-----------------------------------------------------------*/

bool Agent_ReleaseCommand( Command_t * pxCommandToRelease )
{
    bool bStructReturned = false;

    configASSERT( ucInitStatus == QUEUE_INITIALIZED );

    /* See if the structure being returned is actually from the pool. */
    if( ( pxCommandToRelease >= xCommandStructurePool ) &&
        ( pxCommandToRelease < ( xCommandStructurePool + MQTT_COMMAND_CONTEXTS_POOL_SIZE ) ) )
    {
        bStructReturned = Agent_MessageSend( &xCommandStructMessageCtx, &pxCommandToRelease, 0U );

        /* The send should not fail as the queue was created to hold every command
         * in the pool. */
        configASSERT( bStructReturned );
        LogDebug( ( "Returned Command Context %d to pool",
                    ( int ) ( pxCommandToRelease - xCommandStructurePool ) ) );
    }

    return bStructReturned;
}
