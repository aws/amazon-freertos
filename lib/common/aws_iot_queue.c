/*
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
 */

/**
 * @file aws_iot_queue.c
 * @brief Implements the functions in aws_iot_queue.h
 */

/* Build using a config header, if provided. */
#ifdef AWS_IOT_CONFIG_FILE
    #include AWS_IOT_CONFIG_FILE
#endif

/* Standard includes. */
#include <stddef.h>
#include <string.h>

/* Queue include. */
#include "private/aws_iot_queue.h"

/**
 * @def AwsIotQueue_Assert( expression )
 * @brief Assertion macro for the queue library.
 *
 * Set @ref AWS_IOT_QUEUE_ENABLE_ASSERTS to `1` to enable assertions in the queue
 * library.
 *
 * @param[in] expression Expression to be evaluated.
 */
#if AWS_IOT_QUEUE_ENABLE_ASSERTS == 1
    #ifndef AwsIotQueue_Assert
        #include <assert.h>
        #define AwsIotQueue_Assert( expression )    assert( expression )
    #endif
#else
    #define AwsIotQueue_Assert( expression )
#endif

/*-----------------------------------------------------------*/

/**
 * @brief Remove a single element from the queue.
 *
 * This function should be called within a critical section.
 * @param[in] pQueue The queue holding the element to remove.
 * @param[in] pData The element to remove.
 */
static void _queueRemove( AwsIotQueue_t * const pQueue,
                          void * pData );

/*-----------------------------------------------------------*/

static void _queueRemove( AwsIotQueue_t * const pQueue,
                          void * pData )
{
    /* This function should not be called on an empty queue. */
    AwsIotQueue_Assert( pQueue->pHead != NULL && pQueue->pTail != NULL );

    /* Check if the head of the queue is being removed. */
    if( pData == pQueue->pHead )
    {
        /* Queue head should have NULL previous pointer. */
        AwsIotQueue_Assert( pQueue->params.getPrev( pQueue->pHead ) == NULL );

        /* Move the queue head. */
        pQueue->pHead = pQueue->params.getNext( pQueue->pHead );

        /* Check if the queue is now empty. */
        if( pQueue->pHead == NULL )
        {
            /* If the queue is empty, set the tail pointer to NULL also. */
            pQueue->pTail = NULL;
        }
        else
        {
            /* If the queue is not empty, set the new head's previous pointer
             * to NULL. */
            pQueue->params.setPrev( pQueue->pHead, NULL );
        }
    }
    /* Check if the tail of the queue is being removed. */
    else if( pData == pQueue->pTail )
    {
        /* Queue tail should have NULL next pointer. */
        AwsIotQueue_Assert( pQueue->params.getNext( pQueue->pTail ) == NULL );

        /* Move the queue tail. */
        pQueue->pTail = pQueue->params.getPrev( pQueue->pTail );

        /* Check if the queue is now empty. */
        if( pQueue->pTail == NULL )
        {
            /* If the queue is empty, set the head pointer to NULL also. */
            pQueue->pHead = NULL;
        }
        else
        {
            /* If the queue is not empty, set the new tail's next pointer to
             * NULL. */
            pQueue->params.setNext( pQueue->pTail, NULL );
        }
    }
    /* Otherwise, remove from the middle of the queue. */
    else
    {
        /* Check that pointers in the next and previous elements are correctly
         * set. */
        AwsIotQueue_Assert( pQueue->params.getNext( pQueue->params.getPrev( pData ) ) == pData );
        AwsIotQueue_Assert( pQueue->params.getPrev( pQueue->params.getNext( pData ) ) == pData );

        /* Reassign the pointers in the next and previous elements. */
        pQueue->params.setNext( pQueue->params.getPrev( pData ), pQueue->params.getNext( pData ) );
        pQueue->params.setPrev( pQueue->params.getNext( pData ), pQueue->params.getPrev( pData ) );
    }

    /* Clear the next and previous pointers of pData. */
    pQueue->params.setPrev( pData, NULL );
    pQueue->params.setNext( pData, NULL );
}

/*-----------------------------------------------------------*/

bool AwsIotQueue_Create( AwsIotQueue_t * const pQueue,
                         const AwsIotQueueParams_t * const pQueueParams,
                         uint32_t maxNotifyThreads )
{
    /* If a notify routine is set, at least 1 notify thread must be allowed. */
    if( ( pQueueParams->notifyRoutine != NULL ) && ( maxNotifyThreads == 0 ) )
    {
        return false;
    }

    /* Clear the queue data. This sets all pointers to NULL. */
    ( void ) memset( pQueue, 0x00, sizeof( AwsIotQueue_t ) );

    /* Create the queue mutex. */
    if( AwsIotMutex_Create( &( pQueue->mutex ) ) == false )
    {
        return false;
    }

    /* If a notification routine is set, create the thread count semaphore. */
    if( pQueueParams->notifyRoutine != NULL )
    {
        if( AwsIotSemaphore_Create( &( pQueue->notifyThreadCount ),
                                    maxNotifyThreads,
                                    maxNotifyThreads ) == false )
        {
            AwsIotMutex_Destroy( &( pQueue->mutex ) );

            return false;
        }
    }

    /* Copy the queue params. */
    pQueue->params = *( pQueueParams );

    return true;
}

/*-----------------------------------------------------------*/

void AwsIotQueue_Destroy( AwsIotQueue_t * const pQueue )
{
    /* Clean up notification routine. */
    if( pQueue->params.notifyRoutine != NULL )
    {
        AwsIotMutex_Lock( &( pQueue->mutex ) );

        /* Decrement the count of available notification threads to 0, ensuring
         * no more notification threads may be created. This also waits for
         * termination of any existing notification threads. */
        while( AwsIotSemaphore_GetCount( &( pQueue->notifyThreadCount ) ) > 0 )
        {
            AwsIotMutex_Unlock( &( pQueue->mutex ) );
            AwsIotSemaphore_Wait( &( pQueue->notifyThreadCount ) );
            AwsIotMutex_Lock( &( pQueue->mutex ) );
        }

        AwsIotSemaphore_Destroy( &( pQueue->notifyThreadCount ) );
        AwsIotMutex_Unlock( &( pQueue->mutex ) );
    }

    /* Destroy queue mutex. */
    AwsIotMutex_Destroy( &( pQueue->mutex ) );

    /* Clear all data in the queue. */
    ( void ) memset( pQueue, 0x00, sizeof( AwsIotQueue_t ) );
}

/*-----------------------------------------------------------*/

bool AwsIotQueue_InsertHead( AwsIotQueue_t * const pQueue,
                             void * pData )
{
    bool status = true;

    AwsIotMutex_Lock( &( pQueue->mutex ) );

    /* Clear the next and previous pointers of pData. */
    pQueue->params.setPrev( pData, NULL );
    pQueue->params.setNext( pData, NULL );

    /* Check if the queue is currently empty. */
    if( pQueue->pHead == NULL )
    {
        /* If the head pointer is NULL, the tail pointer should also be NULL. */
        AwsIotQueue_Assert( pQueue->pTail == NULL );

        /* Set the head and tail pointer to the new operation. This places the
         * first item in the queue. */
        pQueue->pHead = pData;
        pQueue->pTail = pData;
    }
    else
    {
        /* The tail shouldn't be NULL when the head isn't NULL. */
        AwsIotQueue_Assert( pQueue->pTail != NULL );

        /* If the queue is not empty, insert the new operation at the head
         * of the queue. */
        pQueue->params.setNext( pData, pQueue->pHead );
        pQueue->params.setPrev( pQueue->pHead, pData );
        pQueue->pHead = pData;
    }

    /* Create a notification thread if this queue has a notify routine set. */
    if( pQueue->params.notifyRoutine != NULL )
    {
        if( ( AwsIotSemaphore_TryWait( &( pQueue->notifyThreadCount ) ) == true ) &&
            ( AwsIot_CreateDetachedThread( pQueue->params.notifyRoutine,
                                           pQueue->params.pNotifyArgument ) == false ) )
        {
            _queueRemove( pQueue, pData );
            AwsIotSemaphore_Post( &( pQueue->notifyThreadCount ) );
            status = false;
        }
    }

    AwsIotMutex_Unlock( &( pQueue->mutex ) );

    return status;
}

/*-----------------------------------------------------------*/

void * AwsIotQueue_RemoveTail( AwsIotQueue_t * const pQueue,
                               bool notifyExitIfEmpty )
{
    void * pResult = NULL;

    AwsIotMutex_Lock( &( pQueue->mutex ) );

    /* Check if a message is available. */
    if( pQueue->pTail != NULL )
    {
        /* If the queue tail isn't NULL, then head shouldn't be NULL. */
        AwsIotQueue_Assert( pQueue->pHead != NULL );

        /* Set the pointer to the tail, then remove it from the queue. */
        pResult = pQueue->pTail;
        _queueRemove( pQueue, pQueue->pTail );
    }
    else
    {
        /* If this is an exiting notification thread, increment the number of
         * available notification threads. */
        if( notifyExitIfEmpty == true )
        {
            AwsIotSemaphore_Post( &( pQueue->notifyThreadCount ) );
        }
    }

    AwsIotMutex_Unlock( &( pQueue->mutex ) );

    return pResult;
}

/*-----------------------------------------------------------*/

void * AwsIotQueue_RemoveFirstMatch( AwsIotQueue_t * const pQueue,
                                     void * pArgument,
                                     bool ( *shouldRemove )( void *, void * ) )
{
    void * pResult = NULL;

    AwsIotMutex_Lock( &( pQueue->mutex ) );

    /* Iterate through the queue to find the first matching element. */
    for( pResult = pQueue->pTail; pResult != NULL; pResult = pQueue->params.getPrev( pResult ) )
    {
        if( ( ( shouldRemove == NULL ) && ( pArgument == pResult ) ) ||
            ( ( shouldRemove != NULL ) && ( shouldRemove( pArgument, pResult ) == true ) ) )
        {
            /* If a matching element is found, remove it from the queue. */
            _queueRemove( pQueue, pResult );
            break;
        }
    }

    AwsIotMutex_Unlock( &( pQueue->mutex ) );

    return pResult;
}

/*-----------------------------------------------------------*/

void AwsIotQueue_RemoveAllMatches( AwsIotQueue_t * const pQueue,
                                   void * pArgument,
                                   bool ( *shouldRemove )( void *, void * ),
                                   void ( *freeElement )( void * ) )
{
    void * i = NULL, * pCurrent = NULL;

    AwsIotMutex_Lock( &( pQueue->mutex ) );
    i = pQueue->pHead;

    /* Iterate through the entire queue. Remove and free all matching elements. */
    while( i != NULL )
    {
        /* Save a pointer to the current element and move the iterating pointer. */
        pCurrent = i;
        i = pQueue->params.getNext( i );

        /* Check if the current queue element should be removed. */
        if( ( shouldRemove == NULL ) ||
            ( shouldRemove( pArgument, pCurrent ) == true ) )
        {
            /* Remove the matched element from the queue and free it. */
            _queueRemove( pQueue, pCurrent );

            if( freeElement != NULL )
            {
                freeElement( pCurrent );
            }
        }
    }

    AwsIotMutex_Unlock( &( pQueue->mutex ) );
}

/*-----------------------------------------------------------*/

bool AwsIotList_Create( AwsIotList_t * const pList,
                        const AwsIotListParams_t * const pListParams )
{
    /* Clear the list data. This the head pointer to NULL. */
    ( void ) memset( pList, 0x00, sizeof( AwsIotList_t ) );

    /* Create the list mutex. */
    if( AwsIotMutex_Create( &( pList->mutex ) ) == false )
    {
        return false;
    }

    /* Copy the list parameters. */
    pList->params = *( pListParams );

    return true;
}

/*-----------------------------------------------------------*/

void AwsIotList_Destroy( AwsIotList_t * const pList )
{
    /* Destroy list mutex. */
    AwsIotMutex_Destroy( &( pList->mutex ) );

    /* Clear all data in the list. */
    ( void ) memset( pList, 0x00, sizeof( AwsIotList_t ) );
}

/*-----------------------------------------------------------*/

void AwsIotList_InsertHead( AwsIotList_t * const pList,
                            void * pData )
{
    /* Clear the next and previous pointers of pData. */
    pList->params.setNext( pData, NULL );
    pList->params.setPrev( pData, NULL );

    /* pData must not already be in the list. */
    AwsIotQueue_Assert( AwsIotList_FindFirstMatch( pList,
                                                   pList->pHead,
                                                   pData,
                                                   NULL ) == NULL );

    /* Insert the new node at the list head. */
    if( pList->pHead == NULL )
    {
        /* Set new head if list is empty. */
        pList->pHead = pData;
    }
    else
    {
        pList->params.setNext( pData, pList->pHead );
        pList->params.setPrev( pList->pHead, pData );
        pList->pHead = pData;
    }
}

/*-----------------------------------------------------------*/

void AwsIotList_InsertSorted( AwsIotList_t * const pList,
                              void * pData,
                              int ( *compare )( void *, void * ) )
{
    void * i = NULL;

    /* Clear the next and previous pointers of pData. */
    pList->params.setNext( pData, NULL );
    pList->params.setPrev( pData, NULL );

    /* pData must not already be in the list. */
    AwsIotQueue_Assert( AwsIotList_FindFirstMatch( pList,
                                                   pList->pHead,
                                                   pData,
                                                   NULL ) == NULL );

    /* Insert at head if list is empty. */
    if( pList->pHead == NULL )
    {
        pList->pHead = pData;
    }
    /* Check if the head should be replaced. */
    else if( compare( pData, pList->pHead ) <= 0 )
    {
        pList->params.setNext( pData, pList->pHead );
        pList->params.setPrev( pList->pHead, pData );
        pList->pHead = pData;
    }
    else
    {
        for( i = pList->pHead; i != NULL; i = pList->params.getNext( i ) )
        {
            /* Insert at queue tail if there's no next element. */
            if( pList->params.getNext( i ) == NULL )
            {
                pList->params.setPrev( pData, i );
                pList->params.setNext( i, pData );
                break;
            }
            /* Check the order of the current and next element. */
            else if( ( compare( pData, i ) >= 0 ) &&
                     ( compare( pData, pList->params.getNext( i ) ) < 0 ) )
            {
                pList->params.setNext( pData, pList->params.getNext( i ) );
                pList->params.setPrev( pData, i );

                pList->params.setNext( i, pData );
                pList->params.setPrev( pList->params.getNext( pData ), pData );
                break;
            }
        }

        /* Ensure that pData was placed in the list. */
        AwsIotQueue_Assert( ( pList->params.getNext( pData ) != NULL ) ||
                            ( pList->params.getPrev( pData ) != NULL ) );
    }
}

/*-----------------------------------------------------------*/

void * AwsIotList_FindFirstMatch( AwsIotList_t * const pList,
                                  void * pStartPoint,
                                  void * pArgument,
                                  bool ( *compare )( void *, void * ) )
{
    void * i = NULL;

    /* Iterate through the list to find the first matching element. */
    for( i = pStartPoint; i != NULL; i = pList->params.getNext( i ) )
    {
        if( ( ( compare == NULL ) && ( pArgument == i ) ) ||
            ( ( compare != NULL ) && ( compare( pArgument, i ) == true ) ) )
        {
            break;
        }
    }

    return i;
}

/*-----------------------------------------------------------*/

void AwsIotList_Remove( AwsIotList_t * const pList,
                        void * pData )
{
    /* This function should not be called with an NULL pData or an empty list. */
    if( ( pList->pHead == NULL ) || ( pData == NULL ) )
    {
        return;
    }

    /* pData must be in pList. */
    AwsIotQueue_Assert( AwsIotList_FindFirstMatch( pList,
                                                   pList->pHead,
                                                   pData,
                                                   NULL ) != NULL );

    /* Check if the list head is being removed. */
    if( pData == pList->pHead )
    {
        /* Set the next node's previous pointer to NULL. */
        if( pList->params.getNext( pData ) != NULL )
        {
            pList->params.setPrev( pList->params.getNext( pData ), NULL );
        }

        /* Update the head pointer. */
        pList->pHead = pList->params.getNext( pData );
    }
    else
    {
        /* Update the next node's previous pointer. */
        if( pList->params.getNext( pData ) != NULL )
        {
            pList->params.setPrev( pList->params.getNext( pData ),
                                   pList->params.getPrev( pData ) );
        }

        /* Update the previous node's next pointer. */
        pList->params.setNext( pList->params.getPrev( pData ),
                               pList->params.getNext( pData ) );
    }

    /* Clear the next and previous pointers of pData. */
    pList->params.setNext( pData, NULL );
    pList->params.setPrev( pData, NULL );
}

/*-----------------------------------------------------------*/

void AwsIotList_RemoveAllMatches( AwsIotList_t * const pList,
                                  void * pArgument,
                                  bool ( *shouldRemove )( void *, void * ),
                                  void ( *freeElement )( void * ) )
{
    void * i = NULL, * pCurrent = NULL;

    AwsIotMutex_Lock( &( pList->mutex ) );
    i = pList->pHead;

    /* Iterate through the entire list. Remove and free all matching elements. */
    while( i != NULL )
    {
        /* Save a pointer to the current element and move the iterating pointer. */
        pCurrent = i;
        i = pList->params.getNext( i );

        /* Check if the current list element should be removed. */
        if( ( shouldRemove == NULL ) ||
            ( shouldRemove( pArgument, pCurrent ) == true ) )
        {
            /* Remove the matched element from the list and free it. */
            AwsIotList_Remove( pList, pCurrent );

            if( freeElement != NULL )
            {
                freeElement( pCurrent );
            }
        }
    }

    AwsIotMutex_Unlock( &( pList->mutex ) );
}

/*-----------------------------------------------------------*/
