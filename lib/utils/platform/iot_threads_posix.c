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
 * @file iot_threads_posix.c
 * @brief Implementation of the functions in iot_threads.h for POSIX systems.
 */

/* Build using a config header, if provided. */
#ifdef IOT_CONFIG_FILE
    #include IOT_CONFIG_FILE
#endif

/* POSIX includes. Allow the default POSIX headers to be overridden. */
#ifdef POSIX_ERRNO_HEADER
    #include POSIX_ERRNO_HEADER
#else
    #include <errno.h>
#endif
#ifdef POSIX_LIMITS_HEADER
    #include POSIX_LIMITS_HEADER
#else
    #include <limits.h>
#endif
#ifdef POSIX_PTHREAD_HEADER
    #include POSIX_PTHREAD_HEADER
#else
    #include <pthread.h>
#endif
#ifdef POSIX_SEMAPHORE_HEADER
    #include POSIX_SEMAPHORE_HEADER
#else
    #include <semaphore.h>
#endif

/* Platform threads include. */
#include "platform/iot_threads.h"

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
 * Provide default values for undefined memory allocation functions.
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
 * @brief Wraps an #IotThreadRoutine_t with a POSIX-compliant one.
 *
 * @param[in] pArgument The value passed as `arg` to `pthread_create`.
 *
 * @return Always returns `NULL`.
 */
static void * _threadRoutineWrapper( void * pArgument );

/* Platform-specific function implemented in iot_clock_posix.c */
extern bool IotClock_TimeoutToTimespec( uint64_t timeoutMs,
                                        struct timespec * const pOutput );

/*-----------------------------------------------------------*/

/**
 * @brief Holds information about an active detached thread.
 */
typedef struct _threadInfo
{
    void * pArgument;                 /**< @brief First argument to `threadRoutine`. */
    IotThreadRoutine_t threadRoutine; /**< @brief Thread function to run. */
} _threadInfo_t;

/*-----------------------------------------------------------*/

static void * _threadRoutineWrapper( void * pArgument )
{
    _threadInfo_t * pThreadInfo = ( _threadInfo_t * ) pArgument;

    /* Read thread routine and argument, then free thread info. */
    IotThreadRoutine_t threadRoutine = pThreadInfo->threadRoutine;
    void * pThreadRoutineArgument = pThreadInfo->pArgument;

    IotThreads_Free( pThreadInfo );

    /* Run the thread routine. */
    threadRoutine( pThreadRoutineArgument );

    return NULL;
}

/*-----------------------------------------------------------*/

bool Iot_CreateDetachedThread( IotThreadRoutine_t threadRoutine,
                               void * pArgument,
                               int32_t priority,
                               size_t stackSize )
{
    bool status = true;
    int posixErrno = 0;
    _threadInfo_t * pThreadInfo = NULL;
    pthread_t newThread;
    pthread_attr_t threadAttributes;

    IotLogDebug( "Creating new thread." );

    /* Check priority if a non-default scheduling priority is given. */
    if( priority != IOT_THREAD_DEFAULT_PRIORITY )
    {
        if( priority < 0 )
        {
            IotLogError( "Priority %ld is not valid for a new thread.",
                         ( long int ) priority );
            status = false;
        }
        else if( ( int ) priority < sched_get_priority_min( SCHED_RR ) )
        {
            IotLogError( "Priority %ld is less than minimum allowed %d.",
                         sched_get_priority_min( SCHED_RR ) );
            status = false;
        }
        else if( ( int ) priority > sched_get_priority_max( SCHED_RR ) )
        {
            IotLogError( "Priority %ld is greater than maximum allowed %d.",
                         sched_get_priority_max( SCHED_RR ) );
            status = false;
        }
        else
        {
            IotLogInfo( "New thread will have priority %ld with SCHED_RR policy.",
                        ( long int ) priority );
        }
    }

    /* Check stack size if a non-default value is given. */
    if( status == true )
    {
        /* Adjust stack size based on system minimum. */
        if( stackSize != IOT_THREAD_DEFAULT_STACK_SIZE )
        {
            if( stackSize < PTHREAD_STACK_MIN )
            {
                IotLogWarn( "Stack size of %lu is smaller than system minimum %d."
                            " System minimum will be used instead.",
                            ( unsigned long ) stackSize,
                            PTHREAD_STACK_MIN );

                stackSize = PTHREAD_STACK_MIN;
            }
        }
    }

    if( status == true )
    {
        /* Allocate memory for the new thread. */
        pThreadInfo = IotThreads_Malloc( sizeof( _threadInfo_t ) );

        if( pThreadInfo == NULL )
        {
            IotLogError( "Failed to allocate memory for new thread." );

            status = false;
        }
    }

    /* Set up thread attributes object. */
    if( status == true )
    {
        /* Create a new thread attributes object. */
        posixErrno = pthread_attr_init( &threadAttributes );

        if( posixErrno != 0 )
        {
            IotLogError( "Failed to initialize thread attributes. errno=%d.",
                         posixErrno );

            status = false;
        }
        else
        {
            /* Set the new thread to detached. */
            posixErrno = pthread_attr_setdetachstate( &threadAttributes,
                                                      PTHREAD_CREATE_DETACHED );

            if( posixErrno != 0 )
            {
                IotLogError( "Failed to set detached thread attribute. errno=%d.",
                             posixErrno );

                status = false;
            }

            /* Set the stack size if given. */
            if( status == true )
            {
                if( stackSize != IOT_THREAD_DEFAULT_STACK_SIZE )
                {
                    posixErrno = pthread_attr_setstacksize( &threadAttributes,
                                                            stackSize );

                    if( posixErrno != 0 )
                    {
                        IotLogError( "Failed to set stack size %lu.", ( unsigned long ) stackSize );

                        status = false;
                    }
                }
            }

            /* Set scheduler policy and priority if given. */
            if( status == true )
            {
                if( priority != IOT_THREAD_DEFAULT_PRIORITY )
                {
                    struct sched_param schedulerParam =
                    {
                        .sched_priority = ( int ) priority
                    };

                    /* Set scheduler policy SCHED_RR. */
                    posixErrno = pthread_attr_setschedpolicy( &threadAttributes,
                                                              SCHED_RR );

                    if( posixErrno != 0 )
                    {
                        IotLogError( "Failed to set scheduling policy SCHED_RR." );

                        status = false;
                    }

                    /* Set scheduler priority. */
                    if( status == true )
                    {
                        posixErrno = pthread_attr_setschedparam( &threadAttributes,
                                                                 &schedulerParam );

                        if( posixErrno != 0 )
                        {
                            IotLogError( "Failed to set scheduling priority %d.",
                                         schedulerParam.sched_priority );

                            status = false;
                        }
                    }
                }
            }

            if( status == false )
            {
                ( void ) pthread_attr_destroy( &threadAttributes );
            }
        }
    }

    if( status == true )
    {
        /* Set the thread routine and argument. */
        pThreadInfo->threadRoutine = threadRoutine;
        pThreadInfo->pArgument = pArgument;

        /* Create the underlying POSIX thread. */
        posixErrno = pthread_create( &newThread,
                                     &threadAttributes,
                                     _threadRoutineWrapper,
                                     pThreadInfo );

        if( posixErrno != 0 )
        {
            IotLogError( "Failed to create new thread. errno=%d.", posixErrno );

            status = false;
        }

        /* Destroy thread attributes object. */
        posixErrno = pthread_attr_destroy( &threadAttributes );

        if( posixErrno != 0 )
        {
            IotLogWarn( "Failed to destroy thread attributes. errno=%d.",
                        posixErrno );
        }
    }

    if( status == false )
    {
        if( pThreadInfo != NULL )
        {
            IotThreads_Free( pThreadInfo );
        }
    }

    return status;
}

/*-----------------------------------------------------------*/

bool IotMutex_Create( IotMutex_t * const pNewMutex, bool recursive )
{
    bool status = true;
    int mutexError = 0;
    pthread_mutexattr_t mutexAttributes, *pMutexAttributes = NULL;

    if( recursive == true )
    {
        IotLogDebug( "Creating new recursive mutex %p.", pNewMutex );

        /* Create new mutex attributes object. */
        mutexError = pthread_mutexattr_init( &mutexAttributes );

        if( mutexError != 0 )
        {
            IotLogError( "Failed to initialize mutex attributes. errno=%d.",
                         mutexError );

            status = false;
        }

        if( status == true )
        {
            pMutexAttributes = &mutexAttributes;

            /* Set recursive mutex type. */
            mutexError = pthread_mutexattr_settype( &mutexAttributes,
                                                    PTHREAD_MUTEX_RECURSIVE );

            if( mutexError != 0 )
            {
                IotLogError( "Failed to set recursive mutex type. errno=%d.",
                             mutexError );

                status = false;
            }
        }
    }
    else
    {
        IotLogDebug( "Creating new mutex %p.", pNewMutex );
    }

    mutexError = pthread_mutex_init( pNewMutex, pMutexAttributes );

    if( mutexError != 0 )
    {
        IotLogError( "Failed to create new mutex %p. errno=%d.",
                     pNewMutex,
                     mutexError );

        status = false;
    }

    /* Destroy any created mutex attributes. */
    if( pMutexAttributes != NULL )
    {
        ( void ) pthread_mutexattr_destroy( &mutexAttributes );
    }

    return status;
}

/*-----------------------------------------------------------*/

void IotMutex_Destroy( IotMutex_t * const pMutex )
{
    IotLogDebug( "Destroying mutex %p.", pMutex );

    int mutexError = pthread_mutex_destroy( pMutex );

    if( mutexError != 0 )
    {
        IotLogWarn( "Failed to destroy mutex %p. errno=%d.",
                    pMutex,
                    mutexError );
    }
}

/*-----------------------------------------------------------*/

void IotMutex_Lock( IotMutex_t * const pMutex )
{
    IotLogDebug( "Locking mutex %p.", pMutex );

    int mutexError = pthread_mutex_lock( pMutex );

    if( mutexError != 0 )
    {
        IotLogWarn( "Failed to lock mutex %p. errno=%d.",
                    pMutex,
                    mutexError );
    }
}

/*-----------------------------------------------------------*/

bool IotMutex_TryLock( IotMutex_t * const pMutex )
{
    bool status = true;

    IotLogDebug( "Attempting to lock mutex %p.", pMutex );

    int mutexError = pthread_mutex_trylock( pMutex );

    if( mutexError != 0 )
    {
        IotLogDebug( "Mutex mutex %p is not available. errno=%d.",
                     pMutex,
                     mutexError );

        status = false;
    }

    return status;
}

/*-----------------------------------------------------------*/

void IotMutex_Unlock( IotMutex_t * const pMutex )
{
    IotLogDebug( "Unlocking mutex %p.", pMutex );

    int mutexError = pthread_mutex_unlock( pMutex );

    if( mutexError != 0 )
    {
        IotLogWarn( "Failed to unlock mutex %p. errno=%d.",
                    pMutex,
                    mutexError );
    }
}

/*-----------------------------------------------------------*/

bool IotSemaphore_Create( IotSemaphore_t * const pNewSemaphore,
                          uint32_t initialValue,
                          uint32_t maxValue )
{
    bool status = true;

    IotLogDebug( "Creating new semaphore %p.", pNewSemaphore );

    if( maxValue > ( uint32_t ) SEM_VALUE_MAX )
    {
        IotLogError( "%lu is larger than the maximum value a semaphore may"
                     " have on this system.", maxValue );

        status = false;
    }
    else
    {
        if( sem_init( pNewSemaphore, 0, ( unsigned int ) initialValue ) != 0 )
        {
            IotLogError( "Failed to create new semaphore %p. errno=%d.",
                         pNewSemaphore,
                         errno );

            status = false;
        }
    }

    return status;
}

/*-----------------------------------------------------------*/

uint32_t IotSemaphore_GetCount( IotSemaphore_t * const pSemaphore )
{
    int count = 0;

    if( sem_getvalue( pSemaphore, &count ) != 0 )
    {
        IotLogWarn( "Failed to query semaphore count of %p. errno=%d.",
                    pSemaphore,
                    errno );
    }

    IotLogDebug( "Semaphore %p has count %d.", pSemaphore, count );

    return ( uint32_t ) count;
}

/*-----------------------------------------------------------*/

void IotSemaphore_Destroy( IotSemaphore_t * const pSemaphore )
{
    IotLogDebug( "Destroying semaphore %p.", pSemaphore );

    if( sem_destroy( pSemaphore ) != 0 )
    {
        IotLogWarn( "Failed to destroy semaphore %p. errno=%d.",
                    pSemaphore,
                    errno );
    }
}

/*-----------------------------------------------------------*/

void IotSemaphore_Wait( IotSemaphore_t * pSemaphore )
{
    IotLogDebug( "Waiting on semaphore %p.", pSemaphore );

    if( sem_wait( pSemaphore ) != 0 )
    {
        IotLogWarn( "Failed to wait on semaphore %p. errno=%d.",
                    pSemaphore,
                    errno );
    }
}

/*-----------------------------------------------------------*/

bool IotSemaphore_TryWait( IotSemaphore_t * const pSemaphore )
{
    bool status = true;

    IotLogDebug( "Attempting to wait on semaphore %p.", pSemaphore );

    if( sem_trywait( pSemaphore ) != 0 )
    {
        IotLogDebug( "Semaphore %p is not available. errno=%d.",
                     pSemaphore,
                     errno );

        status = false;
    }

    return status;
}

/*-----------------------------------------------------------*/

bool IotSemaphore_TimedWait( IotSemaphore_t * const pSemaphore,
                             uint64_t timeoutMs )
{
    bool status = true;
    struct timespec timeout = { 0 };

    IotLogDebug( "Attempting to wait on semaphore %p with timeout %llu.",
                 pSemaphore,
                 timeoutMs );

    if( IotClock_TimeoutToTimespec( timeoutMs, &timeout ) == false )
    {
        IotLogError( "Invalid timeout." );

        status = false;
    }
    else
    {
        if( sem_timedwait( pSemaphore, &timeout ) != 0 )
        {
            IotLogDebug( "Semaphore %p is not available. errno=%d.",
                         pSemaphore,
                         errno );

            status = false;
        }
    }

    return status;
}

/*-----------------------------------------------------------*/

void IotSemaphore_Post( IotSemaphore_t * const pSemaphore )
{
    IotLogDebug( "Posting to semaphore %p.", pSemaphore );

    if( sem_post( pSemaphore ) != 0 )
    {
        IotLogWarn( "Failed to post to semaphore %p. errno=%d.",
                    pSemaphore,
                    errno );
    }
}

/*-----------------------------------------------------------*/
