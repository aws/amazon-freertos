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
 * @file aws_iot_threads_posix.c
 * @brief Implementation of the functions in aws_iot_threads.h for POSIX systems.
 */

/* Build using a config header, if provided. */
#ifdef AWS_IOT_CONFIG_FILE
    #include AWS_IOT_CONFIG_FILE
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
#include "platform/aws_iot_threads.h"

/* Configure logs for the functions in this file. */
#ifdef AWS_IOT_LOG_LEVEL_PLATFORM
    #define _LIBRARY_LOG_LEVEL        AWS_IOT_LOG_LEVEL_PLATFORM
#else
    #ifdef AWS_IOT_LOG_LEVEL_GLOBAL
        #define _LIBRARY_LOG_LEVEL    AWS_IOT_LOG_LEVEL_GLOBAL
    #else
        #define _LIBRARY_LOG_LEVEL    AWS_IOT_LOG_NONE
    #endif
#endif

#define _LIBRARY_LOG_NAME    ( "THREAD" )
#include "aws_iot_logging_setup.h"

/*
 * Provide default values for undefined memory allocation functions based on
 * the usage of dynamic memory allocation.
 */
#ifndef AwsIotThreads_Malloc
    #include <stdlib.h>

    /**
     * @brief Memory allocation. This function should have the same signature
     * as [malloc](http://pubs.opengroup.org/onlinepubs/9699919799/functions/malloc.html).
     */
    #define AwsIotThreads_Malloc    malloc
#endif
#ifndef AwsIotThreads_Free
    #include <stdlib.h>

    /**
     * @brief Free memory. This function should have the same signature as
     * [free](http://pubs.opengroup.org/onlinepubs/9699919799/functions/free.html).
     */
    #define AwsIotThreads_Free    free
#endif

/*-----------------------------------------------------------*/

/**
 * @brief Wraps an AwsIotThreadRoutine_t with a POSIX-compliant one.
 *
 * @param[in] pArgument The value passed as `arg` to `pthread_create`.
 *
 * @return Always returns `NULL`.
 */
static void * _threadRoutineWrapper( void * pArgument );

/* Platform-specific function implemented in aws_iot_clock_posix.c */
extern bool AwsIotClock_TimeoutToTimespec( uint64_t timeoutMs,
                                           struct timespec * const pOutput );

/*-----------------------------------------------------------*/

/**
 * @brief Holds information about an active detached thread.
 */
typedef struct _threadInfo
{
    void * pArgument;                    /**< @brief First argument to `threadRoutine`. */
    AwsIotThreadRoutine_t threadRoutine; /**< @brief Thread function to run. */
} _threadInfo_t;

/*-----------------------------------------------------------*/

static void * _threadRoutineWrapper( void * pArgument )
{
    _threadInfo_t * pThreadInfo = ( _threadInfo_t * ) pArgument;

    /* Run the thread routine. */
    pThreadInfo->threadRoutine( pThreadInfo->pArgument );

    AwsIotThreads_Free( pThreadInfo );

    return NULL;
}

/*-----------------------------------------------------------*/

bool AwsIot_CreateDetachedThread( AwsIotThreadRoutine_t threadRoutine,
                                  void * pArgument )
{
    int posixErrno = 0;
    _threadInfo_t * pThreadInfo = NULL;
    pthread_t newThread;
    pthread_attr_t threadAttributes;

    AwsIotLogDebug( "Creating new thread." );

    /* Allocate memory for the new thread. */
    pThreadInfo = AwsIotThreads_Malloc( sizeof( _threadInfo_t ) );

    if( pThreadInfo == NULL )
    {
        AwsIotLogError( "Failed to allocate memory for new thread." );

        return false;
    }

    /* Create a new thread attributes object. */
    posixErrno = pthread_attr_init( &threadAttributes );

    if( posixErrno != 0 )
    {
        AwsIotLogError( "Failed to initialize thread attributes. errno=%d.",
                        posixErrno );
        AwsIotThreads_Free( pThreadInfo );

        return false;
    }

    /* Set the new thread to detached. */
    posixErrno = pthread_attr_setdetachstate( &threadAttributes,
                                              PTHREAD_CREATE_DETACHED );

    if( posixErrno != 0 )
    {
        AwsIotLogError( "Failed to set detached thread attribute. errno=%d.",
                        posixErrno );
        ( void ) pthread_attr_destroy( &threadAttributes );
        AwsIotThreads_Free( pThreadInfo );

        return false;
    }

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
        AwsIotLogError( "Failed to create new thread. errno=%d.", posixErrno );
        ( void ) pthread_attr_destroy( &threadAttributes );
        AwsIotThreads_Free( pThreadInfo );

        return false;
    }

    /* Destroy thread attributes object. */
    posixErrno = pthread_attr_destroy( &threadAttributes );

    if( posixErrno != 0 )
    {
        AwsIotLogWarn( "Failed to destroy thread attributes. errno=%d.",
                       posixErrno );
    }

    return true;
}

/*-----------------------------------------------------------*/

bool AwsIotMutex_Create( AwsIotMutex_t * pNewMutex )
{
    AwsIotLogDebug( "Creating new mutex %p.", pNewMutex );

    int mutexError = pthread_mutex_init( pNewMutex, NULL );

    if( mutexError != 0 )
    {
        AwsIotLogError( "Failed to create new mutex %p. errno=%d.",
                        pNewMutex,
                        mutexError );

        return false;
    }

    return true;
}

/*-----------------------------------------------------------*/

void AwsIotMutex_Destroy( AwsIotMutex_t * const pMutex )
{
    AwsIotLogDebug( "Destroying mutex %p.", pMutex );

    int mutexError = pthread_mutex_destroy( pMutex );

    if( mutexError != 0 )
    {
        AwsIotLogWarn( "Failed to destroy mutex %p. errno=%d.",
                       pMutex,
                       mutexError );
    }
}

/*-----------------------------------------------------------*/

void AwsIotMutex_Lock( AwsIotMutex_t * const pMutex )
{
    AwsIotLogDebug( "Locking mutex %p.", pMutex );

    int mutexError = pthread_mutex_lock( pMutex );

    if( mutexError != 0 )
    {
        AwsIotLogWarn( "Failed to lock mutex %p. errno=%d.",
                       pMutex,
                       mutexError );
    }
}

/*-----------------------------------------------------------*/

bool AwsIotMutex_TryLock( AwsIotMutex_t * const pMutex )
{
    AwsIotLogDebug( "Attempting to lock mutex %p.", pMutex );

    int mutexError = pthread_mutex_trylock( pMutex );

    if( mutexError != 0 )
    {
        AwsIotLogDebug( "Mutex mutex %p is not available. errno=%d.",
                        pMutex,
                        mutexError );

        return false;
    }

    return true;
}

/*-----------------------------------------------------------*/

void AwsIotMutex_Unlock( AwsIotMutex_t * const pMutex )
{
    AwsIotLogDebug( "Unlocking mutex %p.", pMutex );

    int mutexError = pthread_mutex_unlock( pMutex );

    if( mutexError != 0 )
    {
        AwsIotLogWarn( "Failed to unlock mutex %p. errno=%d.",
                       pMutex,
                       mutexError );
    }
}

/*-----------------------------------------------------------*/

bool AwsIotSemaphore_Create( AwsIotSemaphore_t * const pNewSemaphore,
                             uint32_t initialValue,
                             uint32_t maxValue )
{
    AwsIotLogDebug( "Creating new semaphore %p.", pNewSemaphore );

    if( maxValue > ( uint32_t ) SEM_VALUE_MAX )
    {
        AwsIotLogError( "%lu is larger than the maximum value a semaphore may"
                        " have on this system.", maxValue );

        return false;
    }

    if( sem_init( pNewSemaphore, 0, ( unsigned int ) initialValue ) != 0 )
    {
        AwsIotLogError( "Failed to create new semaphore %p. errno=%d.",
                        pNewSemaphore,
                        errno );

        return false;
    }

    return true;
}

/*-----------------------------------------------------------*/

uint32_t AwsIotSemaphore_GetCount( AwsIotSemaphore_t * const pSemaphore )
{
    int count = 0;

    if( sem_getvalue( pSemaphore, &count ) != 0 )
    {
        AwsIotLogWarn( "Failed to query semaphore count of %p. errno=%d.",
                       pSemaphore,
                       errno );
    }

    AwsIotLogDebug( "Semaphore %p has count %d.", pSemaphore, count );

    return ( uint32_t ) count;
}

/*-----------------------------------------------------------*/

void AwsIotSemaphore_Destroy( AwsIotSemaphore_t * const pSemaphore )
{
    AwsIotLogDebug( "Destroying semaphore %p.", pSemaphore );

    if( sem_destroy( pSemaphore ) != 0 )
    {
        AwsIotLogWarn( "Failed to destroy semaphore %p. errno=%d.",
                       pSemaphore,
                       errno );
    }
}

/*-----------------------------------------------------------*/

void AwsIotSemaphore_Wait( AwsIotSemaphore_t * pSemaphore )
{
    AwsIotLogDebug( "Waiting on semaphore %p.", pSemaphore );

    if( sem_wait( pSemaphore ) != 0 )
    {
        AwsIotLogWarn( "Failed to wait on semaphore %p. errno=%d.",
                       pSemaphore,
                       errno );
    }
}

/*-----------------------------------------------------------*/

bool AwsIotSemaphore_TryWait( AwsIotSemaphore_t * const pSemaphore )
{
    AwsIotLogDebug( "Attempting to wait on semaphore %p.", pSemaphore );

    if( sem_trywait( pSemaphore ) != 0 )
    {
        AwsIotLogDebug( "Semaphore %p is not available. errno=%d.",
                        pSemaphore,
                        errno );

        return false;
    }

    return true;
}

/*-----------------------------------------------------------*/

bool AwsIotSemaphore_TimedWait( AwsIotSemaphore_t * const pSemaphore,
                                uint64_t timeoutMs )
{
    struct timespec timeout = { 0 };

    AwsIotLogDebug( "Attempting to wait on semaphore %p with timeout %llu.",
                    pSemaphore, timeoutMs );

    if( AwsIotClock_TimeoutToTimespec( timeoutMs, &timeout ) == false )
    {
        AwsIotLogError( "Invalid timeout." );

        return false;
    }

    if( sem_timedwait( pSemaphore, &timeout ) != 0 )
    {
        AwsIotLogDebug( "Semaphore %p is not available. errno=%d.",
                        pSemaphore,
                        errno );

        return false;
    }

    return true;
}

/*-----------------------------------------------------------*/

void AwsIotSemaphore_Post( AwsIotSemaphore_t * const pSemaphore )
{
    AwsIotLogDebug( "Posting to semaphore %p.", pSemaphore );

    if( sem_post( pSemaphore ) != 0 )
    {
        AwsIotLogWarn( "Failed to post to semaphore %p. errno=%d.",
                       pSemaphore,
                       errno );
    }
}

/*-----------------------------------------------------------*/
