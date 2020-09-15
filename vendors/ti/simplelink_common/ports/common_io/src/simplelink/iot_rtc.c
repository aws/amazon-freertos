/*
 * Copyright (c) 2020, Texas Instruments Incorporated
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * *  Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * *  Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * *  Neither the name of Texas Instruments Incorporated nor the names of
 *    its contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

/**
 * @file    iot_rtc.c
 * @brief   This file contains the definitions of RTC APIs using TI DPL.
 *          The implementation uses the TI Clock DPL layer to allow for low-power
 *          operation. This limits resolution to that of the RTC clock (~30.5 us).
 */
#include <stdint.h>
#include <unistd.h>
#include <stdbool.h>
#include <time.h>

/* Common IO Header */
#include <iot_rtc.h>

/* TI DPL */
#include <ti/drivers/dpl/ClockP.h>

/* Maximum number of RTC intances supported (soft limit) */
#define NUMBER_OF_SUPPORTED_INSTANCES    ( 1 )

/* The maximum delay is not the theoretical maximum to guard against a ClockP wrap-around bug */
#define MAX_CLOCK_TIMEOUT_IN_TICKS       ( ( ( uint32_t ) ~( 0 ) - 10000 ) )

/**
 * @brief TI Simplelink RTC HAL context abstracted in the Peripheral_Descriptor_t.
 *
 * Contains all the parameters needed for working with TI Simplelink ClockP DPL.
 */
typedef struct
{
    ClockP_Handle xAlarmHandle;       /**< TI ClockP handle used with TI API. */
    ClockP_Handle xWakeupHandle;      /**< TI ClockP handle used with TI API. */
    ClockP_Struct xAlarmClockObject;  /**< TI ClockP object. */
    ClockP_Struct xWakeupClockObject; /**< TI ClockP object. */
    IotRtcStatus_t xStatus;           /**< Current status. */
    IotRtcCallback_t xRtcCallback;    /**< User registered callback. */
    void * pvUserCallbackContext;     /**< User callback context */
    struct tm xDateTime;              /**< Internal date/time structure. */
    struct tm xAlarmTime;             /**< Internal alarm date/time structure. */
    uint32_t ulClockTicksToSec;       /**< ClockP ticks to seconds conversion. */
    uint32_t ulMaxTimeoutInSeconds;   /**< ClockP maximum timeout value in seconds. */
    uint32_t ulSecondsToAlarm;        /**< Seconds left until alarm. */
    uint32_t ulLastTimestamp;         /**< Last RTC timestamp in ticks */
    uint32_t ulSubSecAdjustment;      /**< RTC sub-seconds adjustment value in ticks */
    bool xAlarmSet;                   /**< Alarm configured flag */
} IotRtcDescriptor_t;

static void prvAlarmCallback( uintptr_t index );
static void prvWakeupCallback( uintptr_t index );
static time_t prvUpdateDateTime( IotRtcDescriptor_t * pxRtcDesc );
static void prvStartClockAdjusted( IotRtcDescriptor_t * pxRtcDesc,
                                   uint32_t ulTimeoutInTicks );
static bool prvCheckDateTimeValid( IotRtcDatetime_t * xDateTime );

/**
 * @brief Static RTC descriptor table
 */
static IotRtcDescriptor_t xRtc[ NUMBER_OF_SUPPORTED_INSTANCES ];

static bool xIsInitialized = false;

/*-----------------------------------------------------------*/

IotRtcHandle_t iot_rtc_open( int32_t lRtcInstance )
{
    ClockP_Handle xAlarmHandle = NULL;
    ClockP_Handle xWakeupHandle = NULL;
    IotRtcHandle_t xHandle = NULL;
    ClockP_Params xParams;

    if( !xIsInitialized )
    {
        ClockP_Params_init( &xParams );
        xParams.startFlag = false;

        int i;

        for( i = 0; i < NUMBER_OF_SUPPORTED_INSTANCES; i++ )
        {
            xRtc[ i ].xAlarmHandle = NULL;
            xRtc[ i ].xWakeupHandle = &( xRtc[ i ].xWakeupClockObject );
            xRtc[ i ].ulClockTicksToSec = 1000 * 1000 / ClockP_getSystemTickPeriod();
            xRtc[ i ].ulMaxTimeoutInSeconds = MAX_CLOCK_TIMEOUT_IN_TICKS / xRtc[ i ].ulClockTicksToSec;

            xParams.arg = ( uintptr_t ) i;
            xParams.period = MAX_CLOCK_TIMEOUT_IN_TICKS;
            xAlarmHandle = ClockP_construct( &( xRtc[ i ].xAlarmClockObject ),
                                             ( ClockP_Fxn ) & prvAlarmCallback,
                                             MAX_CLOCK_TIMEOUT_IN_TICKS, &xParams );

            xParams.period = 0;
            xWakeupHandle = ClockP_construct( &( xRtc[ i ].xWakeupClockObject ),
                                              ( ClockP_Fxn ) & prvWakeupCallback,
                                              MAX_CLOCK_TIMEOUT_IN_TICKS, &xParams );

            /* There is no reason for this to ever fail as the objects
             * are static but in case this happens, return right away. */
            if( ( NULL == xAlarmHandle ) || ( NULL == xWakeupHandle ) )
            {
                return xHandle;
            }
        }

        xIsInitialized = true;
    }

    if( ( lRtcInstance >= 0 ) && ( lRtcInstance < NUMBER_OF_SUPPORTED_INSTANCES ) &&
        ( NULL == xRtc[ lRtcInstance ].xAlarmHandle ) && xIsInitialized )
    {
        /* Initialize with default values */
        xRtc[ lRtcInstance ].xAlarmHandle = ( ClockP_Handle ) ( &( xRtc[ lRtcInstance ].xAlarmClockObject ) );
        xRtc[ lRtcInstance ].xRtcCallback = NULL;
        xRtc[ lRtcInstance ].pvUserCallbackContext = NULL;
        xRtc[ lRtcInstance ].xAlarmSet = false;
        xRtc[ lRtcInstance ].ulSubSecAdjustment = 0;
        xRtc[ lRtcInstance ].xStatus = eRtcTimerStopped;

        /* Start the clock with maximum timeout, this to capture wrap-arounds */
        ClockP_setTimeout( xRtc[ lRtcInstance ].xAlarmHandle, MAX_CLOCK_TIMEOUT_IN_TICKS );
        ClockP_start( xRtc[ lRtcInstance ].xAlarmHandle );

        xHandle = ( IotRtcHandle_t ) &xRtc[ lRtcInstance ];
    }

    return xHandle;
}

/*-----------------------------------------------------------*/

void iot_rtc_set_callback( IotRtcHandle_t const pxRtcHandle,
                           IotRtcCallback_t xCallback,
                           void * pvUserContext )
{
    IotRtcDescriptor_t * pxRtcDesc = ( IotRtcDescriptor_t * ) pxRtcHandle;

    if( pxRtcDesc && xCallback )
    {
        pxRtcDesc->xRtcCallback = xCallback;
        pxRtcDesc->pvUserCallbackContext = pvUserContext;
    }
}

/*-----------------------------------------------------------*/


int32_t iot_rtc_ioctl( IotRtcHandle_t const pxRtcHandle,
                       IotRtcIoctlRequest_t xRequest,
                       void * const pvBuffer )
{
    int32_t ret = IOT_RTC_INVALID_VALUE;
    IotRtcDescriptor_t * pxRtcDesc = ( IotRtcDescriptor_t * ) pxRtcHandle;

    if( ( NULL != pxRtcDesc ) && ( NULL != pxRtcDesc->xAlarmHandle ) )
    {
        switch( xRequest )
        {
            case eSetRtcAlarm:

                if( NULL != pvBuffer )
                {
                    if( eRtcTimerStopped == pxRtcDesc->xStatus )
                    {
                        ret = IOT_RTC_NOT_STARTED;
                    }
                    else
                    {
                        IotRtcDatetime_t * xNewAlarmTime = ( IotRtcDatetime_t * ) pvBuffer;

                        if( prvCheckDateTimeValid( xNewAlarmTime ) )
                        {
                            time_t xCurrentTimeinSec = prvUpdateDateTime( pxRtcDesc );

                            struct tm xCheckAlarmTime;
                            xCheckAlarmTime.tm_sec = xNewAlarmTime->ucSecond;
                            xCheckAlarmTime.tm_min = xNewAlarmTime->ucMinute;
                            xCheckAlarmTime.tm_hour = xNewAlarmTime->ucHour;
                            xCheckAlarmTime.tm_mday = xNewAlarmTime->ucDay;
                            xCheckAlarmTime.tm_mon = xNewAlarmTime->ucMonth;
                            xCheckAlarmTime.tm_year = xNewAlarmTime->usYear;
                            xCheckAlarmTime.tm_wday = xNewAlarmTime->ucWday;

                            /* Make sure the alarm is in the future */
                            double xSecondsToAlarm = difftime( mktime( &xCheckAlarmTime ), xCurrentTimeinSec );

                            if( 0 < xSecondsToAlarm )
                            {
                                ClockP_stop( pxRtcDesc->xAlarmHandle );

                                pxRtcDesc->xAlarmTime = xCheckAlarmTime;
                                pxRtcDesc->ulSecondsToAlarm = ( uint32_t ) xSecondsToAlarm;

                                uint32_t ulSecondsToTick = MAX_CLOCK_TIMEOUT_IN_TICKS;

                                if( pxRtcDesc->ulSecondsToAlarm < pxRtcDesc->ulMaxTimeoutInSeconds )
                                {
                                    ulSecondsToTick = pxRtcDesc->ulSecondsToAlarm * pxRtcDesc->ulClockTicksToSec;
                                }

                                pxRtcDesc->xAlarmSet = true;
                                prvStartClockAdjusted( pxRtcDesc, ulSecondsToTick );

                                ret = IOT_RTC_SUCCESS;
                            }
                        }
                        else
                        {
                            ret = IOT_RTC_SET_FAILED;
                        }
                    }
                }

                break;

            case eGetRtcAlarm:

                if( NULL != pvBuffer )
                {
                    if( !pxRtcDesc->xAlarmSet )
                    {
                        ret = IOT_RTC_NOT_STARTED;
                    }
                    else
                    {
                        IotRtcDatetime_t * xReturnDateTime = ( IotRtcDatetime_t * ) pvBuffer;

                        xReturnDateTime->ucSecond = pxRtcDesc->xAlarmTime.tm_sec;
                        xReturnDateTime->ucMinute = pxRtcDesc->xAlarmTime.tm_min;
                        xReturnDateTime->ucHour = pxRtcDesc->xAlarmTime.tm_hour;
                        xReturnDateTime->ucDay = pxRtcDesc->xAlarmTime.tm_mday;
                        xReturnDateTime->ucMonth = pxRtcDesc->xAlarmTime.tm_mon;
                        xReturnDateTime->usYear = pxRtcDesc->xAlarmTime.tm_year;
                        xReturnDateTime->ucWday = pxRtcDesc->xAlarmTime.tm_wday;

                        ret = IOT_RTC_SUCCESS;
                    }
                }

                break;

            case eCancelRtcAlarm:

                if( pxRtcDesc->xAlarmSet )
                {
                    pxRtcDesc->xAlarmSet = false;
                    ret = IOT_RTC_SUCCESS;
                }
                else
                {
                    ret = IOT_RTC_NOT_STARTED;
                }

                break;

            case eSetRtcWakeupTime:

                if( NULL != pvBuffer )
                {
                    uint32_t ulWakeupTime = ( uint32_t ) ( *( ( uint32_t * ) pvBuffer ) );

                    if( eRtcTimerStopped == pxRtcDesc->xStatus )
                    {
                        ret = IOT_RTC_NOT_STARTED;
                    }
                    else if( ulWakeupTime > 0 )
                    {
                        ulWakeupTime *= pxRtcDesc->ulClockTicksToSec / 1000;

                        ClockP_setTimeout( pxRtcDesc->xWakeupHandle, ulWakeupTime );
                        ClockP_start( pxRtcDesc->xWakeupHandle );

                        ret = IOT_RTC_SUCCESS;
                    }
                }

                break;

            case eGetRtcWakeupTime:

                if( NULL != pvBuffer )
                {
                    uint32_t ulRemainingTimeoutInMs = 0;

                    if( ClockP_isActive( pxRtcDesc->xWakeupHandle ) )
                    {
                        /* Return the current timeout value rounded up to closest ms */
                        uint32_t ulTicksToMs = pxRtcDesc->ulClockTicksToSec / 1000;
                        ulRemainingTimeoutInMs = ClockP_getTimeout( pxRtcDesc->xWakeupHandle );
                        ulRemainingTimeoutInMs += ( ulTicksToMs / 2 );
                        ulRemainingTimeoutInMs /= ulTicksToMs;
                    }

                    *( ( uint32_t * ) pvBuffer ) = ulRemainingTimeoutInMs;

                    ret = IOT_RTC_SUCCESS;
                }

                break;

            case eCancelRtcWakeup:
                ClockP_stop( pxRtcDesc->xWakeupHandle );
                ret = IOT_RTC_SUCCESS;

                break;

            case eGetRtcStatus:
                *( ( uint32_t * ) pvBuffer ) = pxRtcDesc->xStatus;
                ret = IOT_RTC_SUCCESS;

                break;

            default:
                break;
        }
    }

    return ret;
}

/*-----------------------------------------------------------*/

int32_t iot_rtc_set_datetime( IotRtcHandle_t const pxRtcHandle,
                              const IotRtcDatetime_t * pxDatetime )
{
    int32_t ret = IOT_RTC_INVALID_VALUE;
    IotRtcDescriptor_t * pxRtcDesc = ( IotRtcDescriptor_t * ) pxRtcHandle;
    uint32_t ulSecondsToTick = MAX_CLOCK_TIMEOUT_IN_TICKS;

    if( ( NULL != pxRtcDesc ) && ( NULL != pxRtcDesc->xAlarmHandle ) &&
        ( NULL != pxDatetime ) )
    {
        if( prvCheckDateTimeValid( ( IotRtcDatetime_t * ) pxDatetime ) )
        {
            /* Get timestamp right away to get it as close to the invocation
             * of the iot_rtc_set_datetime API as possible. */
            pxRtcDesc->ulLastTimestamp = ( uint32_t ) ClockP_getSystemTicks();
            pxRtcDesc->ulSubSecAdjustment = 0;

            ClockP_stop( pxRtcDesc->xAlarmHandle );

            pxRtcDesc->xDateTime.tm_sec = pxDatetime->ucSecond;
            pxRtcDesc->xDateTime.tm_min = pxDatetime->ucMinute;
            pxRtcDesc->xDateTime.tm_hour = pxDatetime->ucHour;
            pxRtcDesc->xDateTime.tm_mday = pxDatetime->ucDay;
            pxRtcDesc->xDateTime.tm_mon = pxDatetime->ucMonth;
            pxRtcDesc->xDateTime.tm_year = pxDatetime->usYear;
            pxRtcDesc->xDateTime.tm_wday = pxDatetime->ucWday;

            if( pxRtcDesc->xAlarmSet )
            {
                double xSecondsToAlarm = difftime( mktime( &( pxRtcDesc->xAlarmTime ) ), mktime( &( pxRtcDesc->xDateTime ) ) );

                if( xSecondsToAlarm > 0 )
                {
                    pxRtcDesc->ulSecondsToAlarm = ( uint32_t ) xSecondsToAlarm;

                    if( pxRtcDesc->ulSecondsToAlarm < pxRtcDesc->ulMaxTimeoutInSeconds )
                    {
                        ulSecondsToTick = pxRtcDesc->ulSecondsToAlarm * pxRtcDesc->ulClockTicksToSec;
                    }
                }
                else
                {
                    /* Alarm was set to expire before the new datetime */
                    pxRtcDesc->xStatus = eRtcTimerAlarmTriggered;

                    if( pxRtcDesc->xRtcCallback )
                    {
                        pxRtcDesc->xRtcCallback( eRtcTimerAlarmTriggered, pxRtcDesc->pvUserCallbackContext );
                    }

                    pxRtcDesc->xAlarmSet = false;
                }
            }

            prvStartClockAdjusted( pxRtcDesc, ulSecondsToTick );
            pxRtcDesc->xStatus = eRtcTimerRunning;

            ret = IOT_RTC_SUCCESS;
        }
        else
        {
            ret = IOT_RTC_SET_FAILED;
        }
    }

    return ret;
}

/*-----------------------------------------------------------*/

int32_t iot_rtc_get_datetime( IotRtcHandle_t const pxRtcHandle,
                              IotRtcDatetime_t * pxDatetime )
{
    int32_t ret = IOT_RTC_INVALID_VALUE;
    IotRtcDescriptor_t * pxRtcDesc = ( IotRtcDescriptor_t * ) pxRtcHandle;

    if( ( NULL != pxRtcDesc ) && ( NULL != pxRtcDesc->xAlarmHandle ) &&
        ( NULL != pxDatetime ) )
    {
        if( eRtcTimerStopped != pxRtcDesc->xStatus )
        {
            prvUpdateDateTime( pxRtcDesc );

            pxDatetime->ucSecond = pxRtcDesc->xDateTime.tm_sec;
            pxDatetime->ucMinute = pxRtcDesc->xDateTime.tm_min;
            pxDatetime->ucHour = pxRtcDesc->xDateTime.tm_hour;
            pxDatetime->ucDay = pxRtcDesc->xDateTime.tm_mday;
            pxDatetime->ucMonth = pxRtcDesc->xDateTime.tm_mon;
            pxDatetime->usYear = pxRtcDesc->xDateTime.tm_year;
            pxDatetime->ucWday = pxRtcDesc->xDateTime.tm_wday;

            ret = IOT_RTC_SUCCESS;
        }
        else
        {
            ret = IOT_RTC_NOT_STARTED;
        }
    }

    return ret;
}

/*-----------------------------------------------------------*/

int32_t iot_rtc_close( IotRtcHandle_t const pxRtcHandle )
{
    int32_t ret = IOT_RTC_INVALID_VALUE;
    IotRtcDescriptor_t * pxRtcDesc = ( IotRtcDescriptor_t * ) pxRtcHandle;

    if( ( NULL != pxRtcDesc ) && ( NULL != pxRtcDesc->xAlarmHandle ) )
    {
        ClockP_stop( pxRtcDesc->xAlarmHandle );
        ClockP_stop( pxRtcDesc->xWakeupHandle );
        pxRtcDesc->xAlarmHandle = NULL;

        ret = IOT_RTC_SUCCESS;
    }

    return ret;
}

/*-----------------------------------------------------------*/

static void prvStartClockAdjusted( IotRtcDescriptor_t * pxRtcDesc,
                                   uint32_t ulTimeoutInTicks )
{
    uint32_t ulAdjustValue = ClockP_getSystemTicks();

    if( ulAdjustValue <= pxRtcDesc->ulLastTimestamp )
    {
        ulAdjustValue = ( ~0 ) - ( pxRtcDesc->ulLastTimestamp - ulAdjustValue );
    }
    else
    {
        ulAdjustValue = ulAdjustValue - pxRtcDesc->ulLastTimestamp;
    }

    ulTimeoutInTicks -= ulAdjustValue;

    ClockP_setTimeout( pxRtcDesc->xAlarmHandle, ulTimeoutInTicks );
    ClockP_start( pxRtcDesc->xAlarmHandle );
}

/*-----------------------------------------------------------*/

static time_t prvUpdateDateTime( IotRtcDescriptor_t * pxRtcDesc )
{
    uint32_t ulOldTimestamp = pxRtcDesc->ulLastTimestamp;

    pxRtcDesc->ulLastTimestamp = ( uint32_t ) ClockP_getSystemTicks();
    uint32_t ulDeltaTimestamp = 0;
    uint32_t ulSeconds = 0;

    if( pxRtcDesc->ulLastTimestamp <= ulOldTimestamp )
    {
        ulDeltaTimestamp = ( ~0 ) - ( ulOldTimestamp - pxRtcDesc->ulLastTimestamp );
    }
    else
    {
        ulDeltaTimestamp = pxRtcDesc->ulLastTimestamp - ulOldTimestamp;
    }

    /* Add in subsec adjustment and calculate the new value */
    uint32_t ulNewSubSecAdjustment = ulDeltaTimestamp + pxRtcDesc->ulSubSecAdjustment;

    while( ulNewSubSecAdjustment >= pxRtcDesc->ulClockTicksToSec )
    {
        ulNewSubSecAdjustment -= pxRtcDesc->ulClockTicksToSec;
        ulSeconds++;
    }

    pxRtcDesc->ulSubSecAdjustment = ulNewSubSecAdjustment;

    pxRtcDesc->xDateTime.tm_sec += ulSeconds;
    time_t xCurrentTimeInSec = mktime( &( pxRtcDesc->xDateTime ) );

    return xCurrentTimeInSec;
}

/*-----------------------------------------------------------*/

static bool prvCheckDateTimeValid( IotRtcDatetime_t * xDateTime )
{
    bool ret = true;

    if( ( xDateTime->ucSecond > 59 ) ||
        ( xDateTime->ucMinute > 59 ) ||
        ( xDateTime->ucHour > 23 ) ||
        ( xDateTime->ucDay > 31 ) ||
        ( xDateTime->ucDay < 1 ) ||
        ( xDateTime->ucMonth > 11 ) ||
        ( xDateTime->ucWday > 6 ) )
    {
        ret = false;
    }

    return ret;
}

/*-----------------------------------------------------------*/

static void prvAlarmCallback( uintptr_t index )
{
    prvUpdateDateTime( ( IotRtcDescriptor_t * ) &xRtc[ index ] );

    if( xRtc[ index ].xAlarmSet )
    {
        if( xRtc[ index ].ulSecondsToAlarm < xRtc[ index ].ulMaxTimeoutInSeconds )
        {
            xRtc[ index ].xStatus = eRtcTimerAlarmTriggered;
            xRtc[ index ].xAlarmSet = false;

            if( xRtc[ index ].xRtcCallback )
            {
                xRtc[ index ].xRtcCallback( eRtcTimerAlarmTriggered, xRtc[ index ].pvUserCallbackContext );
            }
        }
        else
        {
            uint32_t ulNextTimeout = MAX_CLOCK_TIMEOUT_IN_TICKS;

            /* Calling ClockP_stop should guarantee us getting the set timeout value
             * and not the current remaining timeout value as the clock is periodic. */
            ClockP_stop( xRtc[ index ].xAlarmHandle );

            uint32_t ulLastTimeout = ClockP_getTimeout( xRtc[ index ].xAlarmHandle );
            ulLastTimeout /= xRtc[ index ].ulClockTicksToSec;

            xRtc[ index ].ulSecondsToAlarm -= ulLastTimeout;

            if( xRtc[ index ].ulSecondsToAlarm < xRtc[ index ].ulMaxTimeoutInSeconds )
            {
                ulNextTimeout = xRtc[ index ].ulSecondsToAlarm * xRtc[ index ].ulClockTicksToSec;
            }

            prvStartClockAdjusted( ( IotRtcDescriptor_t * ) &xRtc[ index ], ulNextTimeout );
        }
    }
}

/*-----------------------------------------------------------*/

static void prvWakeupCallback( uintptr_t index )
{
    if( NULL != xRtc[ index ].xAlarmHandle )
    {
        if( xRtc[ index ].xRtcCallback )
        {
            xRtc[ index ].xStatus = eRtcTimerWakeupTriggered;
            xRtc[ index ].xRtcCallback( eRtcTimerWakeupTriggered, xRtc[ index ].pvUserCallbackContext );
        }
    }
}
