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
 * @file iot_clock_posix.c
 * @brief Implementation of the functions in iot_clock.h for POSIX systems.
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
#ifdef POSIX_PTHREAD_HEADER
    #include POSIX_PTHREAD_HEADER
#else
    #include <pthread.h>
#endif
#ifdef POSIX_SIGNAL_HEADER
    #include POSIX_SIGNAL_HEADER
#else
    #include <signal.h>
#endif
#ifdef POSIX_TIME_HEADER
    #include POSIX_TIME_HEADER
#else
    #include <time.h>
#endif

/* Platform clock include. */
#include "platform/iot_clock.h"

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

#define _LIBRARY_LOG_NAME    ( "CLOCK" )
#include "iot_logging_setup.h"

/*-----------------------------------------------------------*/

/**
 * @brief The format of timestrings printed in logs.
 *
 * For more information on timestring formats, see [this link.]
 * (http://pubs.opengroup.org/onlinepubs/9699919799/functions/strftime.html)
 */
#define _TIMESTRING_FORMAT              ( "%F %R:%S" )

/*
 * Time conversion constants.
 */
#define _NANOSECONDS_PER_SECOND         ( 1000000000 ) /**< @brief Nanoseconds per second. */
#define _NANOSECONDS_PER_MILLISECOND    ( 1000000 )    /**< @brief Nanoseconds per millisecond. */
#define _MILLISECONDS_PER_SECOND        ( 1000 )       /**< @brief Milliseconds per second. */

/*-----------------------------------------------------------*/

/**
 * @brief Wraps an #IotThreadRoutine_t with a POSIX-compliant one.
 *
 * @param[in] argument The value passed as `sigevent.sigev_value`.
 */
static void _timerExpirationWrapper( union sigval argument );

/**
 * @brief Convert a relative timeout in milliseconds to an absolute timeout
 * represented as a struct timespec.
 *
 * This function is not included in iot_clock.h because it's platform-specific.
 * But it may be called by other POSIX platform files.
 * @param[in] timeoutMs The relative timeout.
 * @param[out] pOutput Where to store the resulting `timespec`.
 *
 * @return `true` if `timeoutMs` was successfully converted; `false` otherwise.
 */
bool IotClock_TimeoutToTimespec( uint64_t timeoutMs,
                                 struct timespec * const pOutput );

/*-----------------------------------------------------------*/

static void _timerExpirationWrapper( union sigval argument )
{
    IotTimer_t * pTimer = ( IotTimer_t * ) argument.sival_ptr;

    /* Call the wrapped thread routine. */
    pTimer->threadRoutine( pTimer->pArgument );
}

/*-----------------------------------------------------------*/

bool IotClock_TimeoutToTimespec( uint64_t timeoutMs,
                                 struct timespec * const pOutput )
{
    bool status = true;
    struct timespec systemTime = { 0 };

    if( clock_gettime( CLOCK_REALTIME, &systemTime ) == 0 )
    {
        /* Add the nanoseconds value to the time. */
        systemTime.tv_nsec += ( long ) ( ( timeoutMs % _MILLISECONDS_PER_SECOND ) * _NANOSECONDS_PER_MILLISECOND );

        /* Check for overflow of nanoseconds value. */
        if( systemTime.tv_nsec >= _NANOSECONDS_PER_SECOND )
        {
            systemTime.tv_nsec -= _NANOSECONDS_PER_SECOND;
            systemTime.tv_sec++;
        }

        /* Add the seconds value to the timeout. */
        systemTime.tv_sec += ( time_t ) ( timeoutMs / _MILLISECONDS_PER_SECOND );

        /* Set the output parameter. */
        *pOutput = systemTime;
    }
    else
    {
        IotLogError( "Failed to read system time. errno=%d", errno );

        status = false;
    }

    return status;
}

/*-----------------------------------------------------------*/

bool IotClock_GetTimestring( char * const pBuffer,
                             size_t bufferSize,
                             size_t * const pTimestringLength )
{
    bool status = true;
    const time_t unixTime = time( NULL );
    struct tm localTime = { 0 };
    size_t timestringLength = 0;

    /* localtime_r is the thread-safe variant of localtime. Its return value
     * should be the pointer to the localTime struct. */
    if( localtime_r( &unixTime, &localTime ) != &localTime )
    {
        status = false;
    }

    if( status == true )
    {
        /* Convert the localTime struct to a string. */
        timestringLength = strftime( pBuffer, bufferSize, _TIMESTRING_FORMAT, &localTime );

        /* Check for error from strftime. */
        if( timestringLength == 0 )
        {
            status = false;
        }
        else
        {
            /* Set the output parameter. */
            *pTimestringLength = timestringLength;
        }
    }

    return status;
}

/*-----------------------------------------------------------*/

uint64_t IotClock_GetTimeMs( void )
{
    struct timespec currentTime = { 0 };

    if( clock_gettime( CLOCK_MONOTONIC, &currentTime ) != 0 )
    {
        IotLogWarn( "Failed to read time from CLOCK_MONOTONIC. errno=%d",
                    errno );
    }

    return ( ( uint64_t ) currentTime.tv_sec ) * 1000ULL +
           ( ( uint64_t ) currentTime.tv_nsec ) / 1000000ULL;
}

/*-----------------------------------------------------------*/

bool IotClock_TimerCreate( IotTimer_t * const pNewTimer,
                           IotThreadRoutine_t expirationRoutine,
                           void * pArgument )
{
    bool status = true;
    struct sigevent expirationNotification =
    {
        .sigev_notify            = SIGEV_THREAD,
        .sigev_signo             =                       0,
        .sigev_value.sival_ptr   = pNewTimer,
        .sigev_notify_function   = _timerExpirationWrapper,
        .sigev_notify_attributes = NULL
    };

    IotLogDebug( "Creating new timer %p.", pNewTimer );

    /* Set the timer expiration routine and argument. */
    pNewTimer->threadRoutine = expirationRoutine;
    pNewTimer->pArgument = pArgument;

    /* Create the underlying POSIX timer. */
    if( timer_create( CLOCK_REALTIME,
                      &expirationNotification,
                      &( pNewTimer->timer ) ) != 0 )
    {
        IotLogError( "Failed to create new timer %p. errno=%d.", pNewTimer, errno );
        status = false;
    }

    return status;
}

/*-----------------------------------------------------------*/

void IotClock_TimerDestroy( IotTimer_t * const pTimer )
{
    IotLogDebug( "Destroying timer %p.", pTimer );

    /* Destroy the underlying POSIX timer. */
    if( timer_delete( pTimer->timer ) != 0 )
    {
        IotLogWarn( "Failed to destroy timer %p. errno=%d.", pTimer, errno );
    }
}

/*-----------------------------------------------------------*/

bool IotClock_TimerArm( IotTimer_t * const pTimer,
                        uint64_t relativeTimeoutMs,
                        uint64_t periodMs )
{
    bool status = true;
    struct itimerspec timerExpiration =
    {
        .it_value    = { 0 },
        .it_interval = { 0 }
    };

    IotLogDebug( "Arming timer %p with timeout %llu and period %llu.",
                 pTimer,
                 relativeTimeoutMs,
                 periodMs );

    /* Calculate the initial timer expiration. */
    if( IotClock_TimeoutToTimespec( relativeTimeoutMs,
                                    &( timerExpiration.it_value ) ) == false )
    {
        IotLogError( "Invalid relative timeout." );

        status = false;
    }

    if( status == true )
    {
        /* Calculate the timer expiration period. */
        if( periodMs > 0 )
        {
            timerExpiration.it_interval.tv_sec = ( time_t ) ( periodMs / _MILLISECONDS_PER_SECOND );
            timerExpiration.it_interval.tv_nsec = ( long ) ( ( periodMs % _MILLISECONDS_PER_SECOND ) * _NANOSECONDS_PER_MILLISECOND );
        }

        /* Arm the underlying POSIX timer. */
        if( timer_settime( pTimer->timer, TIMER_ABSTIME, &timerExpiration, NULL ) != 0 )
        {
            IotLogError( "Failed to arm timer %p. errno=%d.", pTimer, errno );

            status = false;
        }
    }

    return status;
}

/*-----------------------------------------------------------*/
