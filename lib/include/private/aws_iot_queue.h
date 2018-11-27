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
 * @file aws_iot_queue.h
 * @brief Declares queue and linked list data structures and functions.
 */

#ifndef _AWS_IOT_QUEUE_H_
#define _AWS_IOT_QUEUE_H_

/* Build using a config header, if provided. */
#ifdef AWS_IOT_CONFIG_FILE
    #include AWS_IOT_CONFIG_FILE
#endif

/* Standard includes. */
#include <stdbool.h>
#include <stdint.h>

/* Platform layer includes. */
#include "platform/aws_iot_threads.h"

/**
 * @defgroup queue_datatypes_queuelist Queue and list
 * @brief Structures that represent a queue or list.
 *
 * These structures contain a copy of the [parameter structure]
 * (@ref queue_datatypes_paramstructs) passed to the queue or list creation
 * function. Members of these structures are internal and should only be modified
 * using [queue or list functions](@ref queue_functions). The exception is the
 * `mutex` member, which may be locked as needed to provide thread safety.
 */

/**
 * @paramstructs{queue,queue}
 *
 * Queue and list parameter structures contain function pointers for accessing
 * the next or previous pointers of a queue element. All of these function
 * pointers must be set before calling a queue or list creation function. These
 * function pointers allow the library to set and query next and previous
 * elements while remaining agnostic to the contents of each element.
 */

/**
 * @ingroup queue_datatypes_paramstructs
 * @brief Configuration parameters of an #AwsIotQueue_t.
 *
 * @paramfor @ref queue_function_create
 *
 * Passed to @ref queue_function_create to configure the new queue.
 */
typedef struct AwsIotQueueParams
{
    /**
     * @brief Notification routine to run when data is added to the queue.
     *
     * This notification will run in its own detached thread. May be set to NULL
     * to disable notification routines.
     */
    AwsIotThreadRoutine_t notifyRoutine;

    /**
     * @brief The argument passed to #AwsIotQueueParams_t.notifyRoutine.
     *
     * This member is not used by any queue function and is intended for
     * application use.
     */
    void * pNotifyArgument;

    /* These function pointers provide access to the queue element's next and
     * previous pointers. */
    void * ( *getNext )( void * ); /**< @brief Returns the value of the queue element's next pointer. */
    void * ( *getPrev )( void * ); /**< @brief Returns the value of the queue element's previous pointer. */
    /** @brief Sets the queue element's next pointer. */
    void ( * setNext )( void *,
                        void * );
    /** @brief Sets the queue element's previous pointer. */
    void ( * setPrev )( void *,
                        void * );
} AwsIotQueueParams_t;

/**
 * @ingroup queue_datatypes_queuelist
 * @brief Queue structure.
 *
 * @attention Although the members of #AwsIotQueue_t are visible to applications,
 * they should not be accessed directly.
 */
typedef struct AwsIotQueue
{
    void * pHead;                        /**< @brief Pointer to queue's head. */
    void * pTail;                        /**< @brief Pointer to queue's tail. */

    AwsIotMutex_t mutex;                 /**< @brief Protects this queue from concurrent use. */
    AwsIotSemaphore_t notifyThreadCount; /**< @brief Limits the number of concurrent notification threads. */
    AwsIotQueueParams_t params;          /**< @brief Queue configuration. See #AwsIotQueueParams_t. */
} AwsIotQueue_t;

/**
 * @ingroup queue_datatypes_paramstructs
 * @brief Configuration parameters of an #AwsIotList_t.
 *
 * @paramfor @ref list_function_create
 *
 * Passed to @ref list_function_create to configure a new list.
 */
typedef struct AwsIotListParams
{
    /* These function pointers provide access to the list element's next and
     * previous pointers. */
    void * ( *getNext )( void * ); /**< @brief Returns the value of the list element's next pointer. */
    void * ( *getPrev )( void * ); /**< @brief Returns the value of the list element's previous pointer. */
    /** @brief Sets the list element's next pointer. */
    void ( * setNext )( void *,
                        void * );
    /** @brief Sets the list element's previous pointer. */
    void ( * setPrev )( void *,
                        void * );
} AwsIotListParams_t;

/**
 * @ingroup queue_datatypes_queuelist
 * @brief List structure.
 *
 * @attention Although the members of #AwsIotList_t are visible to applications, only
 * #AwsIotList_t.mutex and should be accessed directly. This mutex should be locked
 * to provide thread safety for all list functions except @ref list_function_removeallmatches.
 */
typedef struct AwsIotList
{
    void * pHead;              /**< @brief Pointer to the list's head. */
    AwsIotMutex_t mutex;       /**< @brief Protects this list from concurrent use. */
    AwsIotListParams_t params; /**< @brief List configuration. See #AwsIotListParams_t. */
} AwsIotList_t;

/**
 * @functionspage{queue,queue library}
 *
 * Queue functions:
 * - @functionname{queue_function_create}
 * - @functionname{queue_function_destroy}
 * - @functionname{queue_function_inserthead}
 * - @functionname{queue_function_removetail}
 * - @functionname{queue_function_removefirstmatch}
 * - @functionname{queue_function_removeallmatches}
 *
 * List functions:
 * - @functionname{list_function_create}
 * - @functionname{list_function_destroy}
 * - @functionname{list_function_inserthead}
 * - @functionname{list_function_insertsorted}
 * - @functionname{list_function_findfirstmatch}
 * - @functionname{list_function_remove}
 * - @functionname{list_function_removeallmatches}
 */

/**
 * @functionpage{AwsIotQueue_Create,queue,create}
 * @functionpage{AwsIotQueue_Destroy,queue,destroy}
 * @functionpage{AwsIotQueue_InsertHead,queue,inserthead}
 * @functionpage{AwsIotQueue_RemoveTail,queue,removetail}
 * @functionpage{AwsIotQueue_RemoveFirstMatch,queue,removefirstmatch}
 * @functionpage{AwsIotQueue_RemoveAllMatches,queue,removeallmatches}
 * @functionpage{AwsIotList_Create,list,create}
 * @functionpage{AwsIotList_Destroy,list,destroy}
 * @functionpage{AwsIotList_InsertHead,list,inserthead}
 * @functionpage{AwsIotList_InsertSorted,list,insertsorted}
 * @functionpage{AwsIotList_FindFirstMatch,list,findfirstmatch}
 * @functionpage{AwsIotList_Remove,list,remove}
 * @functionpage{AwsIotList_RemoveAllMatches,list,removeallmatches}
 */

/**
 * @brief Create a queue.
 *
 * This function initializes a new queue. It must be called on an uninitialized
 * #AwsIotQueue_t before calling any other queue function. This function must
 * not be called on an already-initialized #AwsIotQueue_t.
 *
 * If [pQueueParams->notifyRoutine](@ref AwsIotQueueParams_t.notifyRoutine) is
 * not `NULL`, then this function creates a notification queue. A notification
 * queue creates a thread to notify the application that data is available. Up to
 * `maxNotifyThreads` simultaneous threads may be created for a single queue.
 *
 * @param[in] pQueue Pointer to the memory that will hold the new queue.
 * @param[in] pQueueParams Configuration parameters of the new queue.
 * @param[in] maxNotifyThreads The maximum number of simultaneous notification
 * threads this queue may have. Ignored if [pQueueParams->notifyRoutine]
 * (@ref AwsIotQueueParams_t.notifyRoutine) is `NULL`. This parameter must be
 * greater than `0` when [pQueueParams->notifyRoutine]
 * (@ref AwsIotQueueParams_t.notifyRoutine) is not `NULL`.
 *
 * @return `true` if a queue was successfully created; `false` otherwise.
 */
/* @[declare_queue_create] */
bool AwsIotQueue_Create( AwsIotQueue_t * const pQueue,
                         const AwsIotQueueParams_t * const pQueueParams,
                         uint32_t maxNotifyThreads );
/* @[declare_queue_create] */

/**
 * @brief Free resources used by a queue.
 *
 * This function frees resources used by a queue. It must be called on an
 * [initialized](@ref queue_function_create) #AwsIotQueue_t. No other queue
 * functions should be called on `pQueue` after calling this function (unless
 * the queue is re-created).
 *
 * If `pQueue` is a notification queue, this function will block to wait for all
 * active notification threads to exit before destroying the queue. This function
 * will also prevent any new notification threads from being created.
 *
 * @param[in] pQueue The queue to destroy.
 */
/* @[declare_queue_destroy] */
void AwsIotQueue_Destroy( AwsIotQueue_t * const pQueue );
/* @[declare_queue_destroy] */

/**
 * @brief Insert an element at the head of the queue.
 *
 * This function places a new element in `pQueue`. New elements are always placed
 * at the head of `pQueue` (FIFO). Data is inserted by setting the [next]
 * (@ref AwsIotQueueParams_t.setNext) and [previous](@ref AwsIotQueueParams_t.setPrev)
 * pointers; no data is copied.
 *
 * If `pQueue` is a notification queue, a new notification thread will be created
 * (assuming the system has the necessary resources).
 *
 * @param[in] pQueue The queue that will hold the new element.
 * @param[in] pData Pointer to the new element.
 *
 * @return `true` if the new element was successfully inserted and any requested
 * notification was successful. If the maximum number of notification threads
 * are active, this function will also return `true`. This function only returns
 * `false` if a notification thread couldn't be created, so its return value can
 * be ignored for queues with no notification routine.
 */
/* @[declare_queue_inserthead] */
bool AwsIotQueue_InsertHead( AwsIotQueue_t * const pQueue,
                             void * pData );
/* @[declare_queue_inserthead] */

/**
 * @brief Removes the element at the tail of the queue.
 *
 * Removes the oldest element from `pQueue`.
 *
 * If this function is being called from a notification thread that will exit
 * if no data is available in the queue, the parameter `notifyExitIfEmpty` should
 * be `true`. Otherwise, `notifyExitIfEmpty` should be `false`.
 *
 * @param[in] pQueue The queue that holds the element to remove.
 * @param[in] notifyExitIfEmpty If this parameter is `true`, then this function
 * will increment the number of available notification threads and expect the
 * calling thread to exit. This parameter should only be `true` if the calling
 * thread is a notification thread.
 *
 * @return Pointer to the removed queue tail; `NULL` if the queue is empty.
 */
/* @[declare_queue_removetail] */
void * AwsIotQueue_RemoveTail( AwsIotQueue_t * const pQueue,
                               bool notifyExitIfEmpty );
/* @[declare_queue_removetail] */

/**
 * @brief Remove a single matching element from a queue.
 *
 * Removes the oldest matching queue element. This function searches starting
 * from the queue tail. It will return the first matching element, so it must
 * be called multiple times for multiple matching elements.
 *
 * @param[in] pQueue The queue to search.
 * @param[in] pArgument If `shouldRemove` is not `NULL`, this value will be passed as the
 * first argument to `shouldRemove`. If `shouldRemove` is `NULL`, every element in the queue
 * will be compared to this value to find a match.
 * @param[in] shouldRemove Function to determine if an element matches. Pass `NULL` to
 * search using the address `pArgument`, i.e. `element == pArgument`.
 *
 * @return Pointer to the removed element; `NULL` if no match was found.
 */
/* @[declare_queue_removefirstmatch] */
void * AwsIotQueue_RemoveFirstMatch( AwsIotQueue_t * const pQueue,
                                     void * pArgument,
                                     bool ( * shouldRemove )( void *, void * ) );
/* @[declare_queue_removefirstmatch] */

/**
 * @brief Remove all matching elements from a queue.
 *
 * This function searches the entire queue and removes all matching elements.
 * If it's not `NULL`, the `freeElement` function is called on all matching
 * elements.
 *
 * @param[in] pQueue The queue to search.
 * @param[in] pArgument The first argument passed to `shouldRemove`.
 * @param[in] shouldRemove Function used to determine if an element matches.
 * Pass `NULL` to remove all elements from the queue.
 * @param[in] freeElement Function called on each matching element. Pass
 * `NULL` to ignore.
 */
/* @[declare_queue_removeallmatches] */
void AwsIotQueue_RemoveAllMatches( AwsIotQueue_t * const pQueue,
                                   void * pArgument,
                                   bool ( * shouldRemove )( void *, void * ),
                                   void ( * freeElement )( void * ) );
/* @[declare_queue_removeallmatches] */

/**
 * @brief Create a list.
 *
 * This function initializes a new list. It must be called on an uninitialized
 * #AwsIotList_t before calling any other list function. This function must not
 * be called on an already-initialized #AwsIotList_t.
 *
 * @param[in] pList Pointer to the memory that will hold the new list.
 * @param[in] pListParams Configuration parameters of the new list.
 *
 * @return `true` if a list was successfully created; `false` otherwise.
 */
/* @[declare_list_create] */
bool AwsIotList_Create( AwsIotList_t * const pList,
                      const AwsIotListParams_t * const pListParams );
/* @[declare_list_create] */

/**
 * @brief Free resources used by a list.
 *
 * This function frees resources used by a list. It must be called on an [initialized]
 * (@ref list_function_create) #AwsIotList_t. No other list functions should be
 * called on `pList` after calling this function (unless the list is re-created).
 *
 * @param[in] pList The list to destroy.
 */
/* @[declare_list_destroy] */
void AwsIotList_Destroy( AwsIotList_t * const pList );
/* @[declare_list_destroy] */

/**
 * @brief Insert an element at the head of the list.
 *
 * This function places a new element at the head of `pList`. Data is inserted
 * by setting the [next](@ref AwsIotListParams_t.setNext) and [previous]
 * (@ref AwsIotListParams_t.setPrev) pointers; no data is copied.
 *
 * @param[in] pList The list that will hold the new element.
 * @param[in] pData Pointer to the new element.
 *
 * @note This function should be called with [pList->mutex](@ref AwsIotList_t.mutex)
 * locked if thread safety is needed.
 */
/* @[declare_list_inserthead] */
void AwsIotList_InsertHead( AwsIotList_t * const pList,
                            void * pData );
/* @[declare_list_inserthead] */

/**
 * @brief Insert an element in a sorted list.
 *
 * Places an element into a list by sorting it into order. The function
 * `compare` is used to determine where to place the new element. Data is
 * inserted by setting the [next](@ref AwsIotListParams_t.setNext) and
 * [previous](@ref AwsIotListParams_t.setPrev) pointers; no data is copied.
 *
 * @param[in] pList The list that will hold the new element.
 * @param[in] pData Pointer to the new element.
 * @param[in] compare Determines the order of the list. Returns a negative
 * value if its first argument is less than its second argument; returns
 * zero if its first argument is equal to its second argument; returns a
 * positive value if its first argument is greater than its second argument.
 *
 * @note This function should be called with [pList->mutex](@ref AwsIotList_t.mutex)
 * locked if thread safety is needed.
 */
/* @[declare_list_insertsorted] */
void AwsIotList_InsertSorted( AwsIotList_t * const pList,
                              void * pData,
                              int ( * compare )( void *, void * ) );
/* @[declare_list_insertsorted] */

/**
 * @brief Search a list for the first matching element after the start point.
 *
 * This function searches `pList` from `pStartPoint` to the list tail for the
 * first matching element. If a match is found, the matching element is <b>not</b>
 * removed from the list.
 *
 * @param[in] pList The list to search.
 * @param[in] pStartPoint Start point of the search. Pass [pList->pHead]
 * (@ref AwsIotList_t.pHead) to search the entire list.
 * @param[in] pArgument If `compare` is not `NULL`, this value will be passed
 * as the first argument to `compare`. If `compare` is `NULL`, every
 * element in the list will be compared to this value to find a match.
 * @param[in] compare Function to determine if an element matches. Pass `NULL` to
 * search using the address `pArgument`, i.e. `element == pArgument`.
 *
 * @return Pointer to a matching element; `NULL` if no match was found.
 *
 * @note This function should be called with [pList->mutex](@ref AwsIotList_t.mutex)
 * locked if thread safety is needed.
 */
/* @[declare_list_findfirstmatch] */
void * AwsIotList_FindFirstMatch( AwsIotList_t * const pList,
                                  void * pStartPoint,
                                  void * pArgument,
                                  bool ( * compare )( void *, void * ) );
/* @[declare_list_findfirstmatch] */

/**
 * @brief Remove a single element from a list.
 *
 * Remove an element from a list. The given element must be in `pList`; use
 * @ref list_function_findfirstmatch to determine if an element is in `pList`.
 *
 * @param[in] pList The list that holds `pData`.
 * @param[in] pData The element to remove.
 *
 * @warning Do not pass a `pData` that is not in `pList`.
 * @note This function should be called with [pList->mutex](@ref AwsIotList_t.mutex)
 * locked if thread safety is needed.
 */
/* @[declare_list_remove] */
void AwsIotList_Remove( AwsIotList_t * const pList,
                        void * pData );
/* @[declare_list_remove] */

/**
 * @brief Remove all matching elements from a list.
 *
 * This function searches an entire list and removes all matching elements.
 * If it's not `NULL`, the `freeElement` function is called on all the matching
 * elements.
 *
 * @param[in] pList The list to search.
 * @param[in] pArgument The first argument passed to `shouldRemove`.
 * @param[in] shouldRemove Function used to determine if an element matches.
 * Pass `NULL` to remove all elements from the list.
 * @param[in] freeElement Function called on each matching element. Pass
 * `NULL` to ignore.
 *
 * @warning Unlike the other #AwsIotList_t functions, this function provides
 * thread safety by locking [pList->mutex](@ref AwsIotList_t.mutex). Therefore,
 * locking [pList->mutex](@ref AwsIotList_t.mutex) before calling this function
 * will cause a deadlock.
 */
/* @[declare_list_removeallmatches] */
void AwsIotList_RemoveAllMatches( AwsIotList_t * const pList,
                                  void * pArgument,
                                  bool ( * shouldRemove )( void *, void * ),
                                  void ( * freeElement )( void * ) );
/* @[declare_list_removeallmatches] */

#endif /* ifndef _AWS_IOT_QUEUE_H_ */
