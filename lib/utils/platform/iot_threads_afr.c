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
 * @file iot_threads_afr.c
 * @brief Implementation of the functions in iot_threads.h for POSIX systems.
 */

/* Build using a config header, if provided. */
#ifdef IOT_CONFIG_FILE
    #include IOT_CONFIG_FILE
#endif


/* Platform threads include. */
#include "platform/iot_threads.h"
#include "types/iot_platform_types.h"

/* Configure logs for the functions in this file. */
#ifdef IOT_LOG_LEVEL_PLATFORM
    #define _LIBRARY_LOG_LEVEL        IOT_LOG_LEVEL_PLATFORM
#else
    #ifdef IOT_LOG_LEVEL_GLOBAL
        #define _LIBRARY_LOG_LEVEL    IOT_LOG_LEVEL_GLOBAL
    #else
        #define _LIBRARY_LOG_LEVEL    IOT_LOG_NONE
    #endif
#endif

#define _LIBRARY_LOG_NAME    ( "THREAD" )
#include "iot_logging_setup.h"

/*
 * Provide default values for undefined memory allocation functions based on
 * the usage of dynamic memory allocation.
 */
#ifndef IotThreads_Malloc
    #include <stdlib.h>

    /**
     * @brief Memory allocation. This function should have the same signature
     * as [malloc](http://pubs.opengroup.org/onlinepubs/9699919799/functions/malloc.html).
     */
    #define IotThreads_Malloc    malloc
#endif
#ifndef IotThreads_Free
    #include <stdlib.h>

    /**
     * @brief Free memory. This function should have the same signature as
     * [free](http://pubs.opengroup.org/onlinepubs/9699919799/functions/free.html).
     */
    #define IotThreads_Free    free
#endif

/*-----------------------------------------------------------*/

/**
 * @defgroup Mutex types.
 */
/**@{ */
#ifndef IOT_THREAD_MUTEX_NORMAL
    #define IOT_THREAD_MUTEX_NORMAL        0                        /**< Non-robust, deadlock on relock, does not remember owner. */
#endif
#ifndef IOT_THREAD_MUTEX_ERRORCHECK
    #define IOT_THREAD_MUTEX_ERRORCHECK    1                        /**< Non-robust, error on relock,  remembers owner. */
#endif
#ifndef IOT_THREAD_MUTEX_RECURSIVE
    #define IOT_THREAD_MUTEX_RECURSIVE     2                        /**< Non-robust, recursive relock, remembers owner. */
#endif
#ifndef IOT_THREAD_MUTEX_DEFAULT
    #define IOT_THREAD_MUTEX_DEFAULT       IOT_THREAD_MUTEX_NORMAL  /**< IOT_THREAD_MUTEX_NORMAL (default). */
#endif
/**@} */

/*-----------------------------------------------------------*/

static void _threadRoutineWrapper( void * pArgument )
{
    _threadInfo_t * pThreadInfo = ( _threadInfo_t * ) pArgument;

    /* Run the thread routine. */
    pThreadInfo->threadRoutine( pThreadInfo->pArgument );
    IotThreads_Free( pThreadInfo );

    vTaskDelete( NULL );
}

/*-----------------------------------------------------------*/

bool Iot_CreateDetachedThread(  IotThreadRoutine_t threadRoutine,
                                void * pArgument,
                                int32_t priority,
                                size_t stackSize )
{
    IotLogDebug( "Creating new thread." );
    int status = 0;
    _threadInfo_t * pThreadInfo = IotThreads_Malloc(sizeof(_threadInfo_t));

    if ( pThreadInfo == NULL)
    {
        IotLogDebug( "Unable to allocate memory for threadRoutine %p.", threadRoutine );
        status = 1;
    }

    /* Create the FreeRTOS task that will run the thread. */
    if ( status == 0 )
    {
        pThreadInfo->threadRoutine = threadRoutine;
        pThreadInfo->pArgument = pArgument;
        
        /* update supplied if we require the default stack size */
        if ( stackSize == IOT_THREAD_DEFAULT_STACK_SIZE)
        {
            stackSize = 3072;
        }

        /* update supplied if we require the default thread priority */
        if ( priority == IOT_THREAD_DEFAULT_PRIORITY )
        {
            priority = 5;
        }

        if( xTaskCreate( _threadRoutineWrapper,
                        "iot_thread",
                        stackSize,
                        pThreadInfo,
                        priority,
                        NULL ) != pdPASS )
        {
            /* Task creation failed. */
            IotLogWarn( "Failed to create thread.\r\n" );
            IotLogWarn( "stackSize = %d\r\npriority = %d" , stackSize, priority);
            IotThreads_Free( pThreadInfo );
            status = 1;
        }
    }
    
    return ( status == 0 );
}

/*-----------------------------------------------------------*/

bool IotMutex_Create( IotMutex_t * pNewMutex , bool recursive )
{
    IotLogDebug( "Creating new mutex %p.", pNewMutex );
    _iot_mutex_internal_t * pMutex = ( _iot_mutex_internal_t * ) pNewMutex;
    
    if ( recursive  )
    {
        ( void ) xSemaphoreCreateRecursiveMutexStatic( & pMutex->xMutex );
    }
    else 
    {
        ( void ) xSemaphoreCreateMutexStatic( & pMutex->xMutex );
    }
    
    pMutex->recursive = recursive; /* remember the type of mutex */

    return true;
}

/*-----------------------------------------------------------*/

void IotMutex_Destroy( IotMutex_t * const pMutex )
{
    _iot_mutex_internal_t * internalMutex = ( _iot_mutex_internal_t * ) pMutex;

    vSemaphoreDelete( ( SemaphoreHandle_t ) & internalMutex->xMutex );
}

/*-----------------------------------------------------------*/

bool prIotMutexTimedLock(IotMutex_t * const pMutex, TickType_t timeout)
{
    _iot_mutex_internal_t * internalMutex = ( _iot_mutex_internal_t * ) pMutex;
    BaseType_t  lockResult;

    IotLogDebug( "Locking mutex %p.", internalMutex );

    /* Call the correct FreeRTOS mutex take function based on mutex type. */
    if( internalMutex->recursive == true )
    {
        lockResult = xSemaphoreTakeRecursive( ( SemaphoreHandle_t ) & internalMutex->xMutex, timeout );
    }
    else
    {
        lockResult = xSemaphoreTake( ( SemaphoreHandle_t ) & internalMutex->xMutex, timeout);
    }
  
    return ( lockResult == pdTRUE );
}

/*-----------------------------------------------------------*/

void IotMutex_Lock( IotMutex_t * const pMutex )
{
    prIotMutexTimedLock( pMutex, portMAX_DELAY );
}

/*-----------------------------------------------------------*/

bool IotMutex_TryLock( IotMutex_t * const pMutex )
{
    return prIotMutexTimedLock( pMutex, 0 );
}

/*-----------------------------------------------------------*/

void IotMutex_Unlock( IotMutex_t * const pMutex )
{
    _iot_mutex_internal_t * internalMutex = ( _iot_mutex_internal_t * ) pMutex;

    IotLogDebug( "Unlocking mutex %p.", internalMutex );
    
    /* Call the correct FreeRTOS mutex unlock function based on mutex type. */
    if( internalMutex->recursive == true )
    {
        ( void ) xSemaphoreGiveRecursive( ( SemaphoreHandle_t ) & internalMutex->xMutex );
    }
    else
    {
         ( void ) xSemaphoreGive( ( SemaphoreHandle_t ) & internalMutex->xMutex );
    }
}

/*-----------------------------------------------------------*/

bool IotSemaphore_Create( IotSemaphore_t * const pNewSemaphore,
                             uint32_t initialValue,
                             uint32_t maxValue )
{
    _iot_sem_internal_t * internalSemaphore = ( _iot_sem_internal_t * ) pNewSemaphore;
    IotLogDebug( "Creating new semaphore %p.", pNewSemaphore );

    ( void ) xSemaphoreCreateCountingStatic( maxValue, initialValue, &internalSemaphore->xSemaphore );

    return true;
}

/*-----------------------------------------------------------*/

uint32_t IotSemaphore_GetCount( IotSemaphore_t * const pSemaphore )
{
    _iot_sem_internal_t * internalSemaphore = ( _iot_sem_internal_t * ) pSemaphore;
    UBaseType_t count = 0;

    count = ( UBaseType_t ) uxSemaphoreGetCount( ( SemaphoreHandle_t ) &internalSemaphore->xSemaphore );

    IotLogDebug( "Semaphore %p has count %d.", pSemaphore, count );

    return ( uint32_t ) count;
}

/*-----------------------------------------------------------*/

void IotSemaphore_Destroy( IotSemaphore_t * const pSemaphore )
{
    _iot_sem_internal_t * internalSemaphore = ( _iot_sem_internal_t * ) pSemaphore;

    IotLogDebug( "Destroying semaphore %p.", internalSemaphore );

    vSemaphoreDelete( ( SemaphoreHandle_t ) &internalSemaphore->xSemaphore );
}

/*-----------------------------------------------------------*/

void IotSemaphore_Wait( IotSemaphore_t * pSemaphore )
{
    _iot_sem_internal_t * internalSemaphore = ( _iot_sem_internal_t * ) pSemaphore;

    IotLogDebug( "Waiting on semaphore %p.", internalSemaphore );

    /* Take the semaphore using the FreeRTOS API. */
    if( xSemaphoreTake( ( SemaphoreHandle_t ) &internalSemaphore->xSemaphore,
                          portMAX_DELAY ) != pdTRUE )
    {
        IotLogWarn( "Failed to wait on semaphore %p.",
                       pSemaphore);
    }
}

/*-----------------------------------------------------------*/

bool IotSemaphore_TryWait( IotSemaphore_t * const pSemaphore )
{
    _iot_sem_internal_t * internalSemaphore = ( _iot_sem_internal_t * ) pSemaphore;

    IotLogDebug( "Attempting to wait on semaphore %p.", internalSemaphore );

    return IotSemaphore_TimedWait( pSemaphore, 0 );
}

/*-----------------------------------------------------------*/

bool IotSemaphore_TimedWait( IotSemaphore_t * const pSemaphore,
                                uint64_t timeoutMs )
{
    _iot_sem_internal_t * internalSemaphore = ( _iot_sem_internal_t * ) pSemaphore;

    /* Take the semaphore using the FreeRTOS API. */
    if( xSemaphoreTake( ( SemaphoreHandle_t ) &internalSemaphore->xSemaphore,
                          timeoutMs * configTICK_RATE_HZ / 1000 ) != pdTRUE )
    {
        /* Only warn if timeout > 0 */
        if (timeoutMs > 0)
        {
            IotLogWarn( "Timeout waiting on semaphore %p.",
                        internalSemaphore );
        }
        return false;
    }

    return true;
}

/*-----------------------------------------------------------*/

void IotSemaphore_Post( IotSemaphore_t * const pSemaphore )
{
    _iot_sem_internal_t * internalSemaphore = ( _iot_sem_internal_t * ) pSemaphore;

    IotLogDebug( "Posting to semaphore %p.", internalSemaphore );
    /* Give the semaphore using the FreeRTOS API. */
    int result =  xSemaphoreGive( ( SemaphoreHandle_t ) &internalSemaphore->xSemaphore );
    if (result == pdFALSE)
    {
        IotLogDebug( "Unable to give semaphore over maximum", internalSemaphore );
    }
}

/*-----------------------------------------------------------*/
