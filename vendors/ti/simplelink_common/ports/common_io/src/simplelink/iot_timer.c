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
 * @file    iot_timer.c
 * @brief   This file contains the definitions of Timer APIs using TI drivers.
 *          The implementation uses the TI Clock DPL layer to allow for low-power
 *          operation. This limits resolution to that of the RTC clock (~30.5 us).
 */
#include <stdint.h>
#include <unistd.h>
#include <stdbool.h>

/* Common IO Header */
#include <iot_timer.h>

/* TI DPL */
#include <ti/drivers/dpl/ClockP.h>

/* Maximum number of timers supported (soft limit) */
#define NUMBER_OF_SUPPORTED_TIMERS    ( 10 )

/**
 * @brief TI Simplelink Timer HAL context abstracted in the Peripheral_Descriptor_t.
 *
 * Contains all the parameters needed for working with TI Simplelink Timer driver.
 */
typedef struct
{
    ClockP_Handle tiTimerHandle;       /**< TI driver handle used with TI Driver API. */
    ClockP_Struct xClockObject;        /**< TI clock structure used with TI Driver API. */
    IotTimerCallback_t xTimerCallback; /**< User registered callback. */
    void * pvUserCallbackContext;      /**< User callback context */
    uint32_t ulStartTimestamp;         /**< Timestamp from when "start" is called */
    bool xIsRunning;                   /**< Timer running or not */
    bool xDelayIsSet;                  /**< Timer delay is set */
} IotTimerDescriptor_t;

static void prvTimerCallback( uintptr_t puTimerDesc );

/**
 * @brief Static Timer descriptor table
 */
static IotTimerDescriptor_t xTimer[ NUMBER_OF_SUPPORTED_TIMERS ];

static bool xIsInitialized = false;

/*-----------------------------------------------------------*/

IotTimerHandle_t iot_timer_open( int32_t lTimerInstance )
{
    ClockP_Handle xTiHandle = NULL;
    IotTimerHandle_t xHandle = NULL;
    ClockP_Params xParams;

    if( false == xIsInitialized )
    {
        ClockP_Params_init( &xParams );
        xParams.period = 0;
        xParams.startFlag = false;

        int i;

        for( i = 0; i < NUMBER_OF_SUPPORTED_TIMERS; i++ )
        {
            xTimer[ i ].tiTimerHandle = NULL;

            xParams.arg = ( uintptr_t ) &xTimer[ i ];
            xTiHandle = ClockP_construct( &( xTimer[ i ].xClockObject ),
                                          ( ClockP_Fxn ) & prvTimerCallback,
                                          ~( 0 ), &xParams );

            /* If we for some reason fail, return NULL.
             * There is no reason for this to ever happen as the objects
             * are static. */
            if( NULL == xTiHandle )
            {
                return xHandle;
            }
        }

        xIsInitialized = true;
    }

    if( ( lTimerInstance >= 0 ) && ( lTimerInstance < NUMBER_OF_SUPPORTED_TIMERS ) &&
        ( NULL == xTimer[ lTimerInstance ].tiTimerHandle ) && xIsInitialized )
    {
        /* Initialize default values */
        xTimer[ lTimerInstance ].tiTimerHandle = ( ClockP_Handle ) ( &( xTimer[ lTimerInstance ].xClockObject ) );
        xTimer[ lTimerInstance ].xTimerCallback = NULL;
        xTimer[ lTimerInstance ].pvUserCallbackContext = NULL;
        xTimer[ lTimerInstance ].xIsRunning = false;
        xTimer[ lTimerInstance ].xDelayIsSet = false;

        xHandle = ( IotTimerHandle_t ) &xTimer[ lTimerInstance ];
    }

    return xHandle;
}

/*-----------------------------------------------------------*/

void iot_timer_set_callback( IotTimerHandle_t const pxTimerHandle,
                             IotTimerCallback_t xCallback,
                             void * pvUserContext )
{
    IotTimerDescriptor_t * pxTimerDesc = ( IotTimerDescriptor_t * ) pxTimerHandle;

    if( pxTimerDesc && xCallback )
    {
        pxTimerDesc->xTimerCallback = xCallback;
        pxTimerDesc->pvUserCallbackContext = pvUserContext;
    }
}

/*-----------------------------------------------------------*/

int32_t iot_timer_start( IotTimerHandle_t const pxTimerHandle )
{
    int32_t ret = IOT_TIMER_SUCCESS;
    IotTimerDescriptor_t * pxTimerDesc = ( IotTimerDescriptor_t * ) pxTimerHandle;

    if( ( NULL == pxTimerDesc ) || ( NULL == pxTimerDesc->tiTimerHandle ) )
    {
        ret = IOT_TIMER_INVALID_VALUE;
    }
    else
    {
        pxTimerDesc->ulStartTimestamp = ( uint64_t ) ClockP_getSystemTicks();
        pxTimerDesc->xIsRunning = true;

        if( pxTimerDesc->xDelayIsSet )
        {
            ClockP_start( pxTimerDesc->tiTimerHandle );
        }
    }

    return ret;
}

/*-----------------------------------------------------------*/

int32_t iot_timer_stop( IotTimerHandle_t const pxTimerHandle )
{
    int32_t ret = IOT_TIMER_SUCCESS;
    IotTimerDescriptor_t * pxTimerDesc = ( IotTimerDescriptor_t * ) pxTimerHandle;

    if( ( NULL == pxTimerDesc ) || ( NULL == pxTimerDesc->tiTimerHandle ) )
    {
        ret = IOT_TIMER_INVALID_VALUE;
    }
    else if( !pxTimerDesc->xIsRunning )
    {
        ret = IOT_TIMER_NOT_RUNNING;
    }
    else
    {
        ClockP_stop( pxTimerDesc->tiTimerHandle );
        pxTimerDesc->xIsRunning = false;
    }

    return ret;
}

/*-----------------------------------------------------------*/

int32_t iot_timer_get_value( IotTimerHandle_t const pxTimerHandle,
                             uint64_t * ullMicroSeconds )
{
    int32_t ret = IOT_TIMER_SUCCESS;
    IotTimerDescriptor_t * pxTimerDesc = ( IotTimerDescriptor_t * ) pxTimerHandle;

    if( ( NULL == pxTimerDesc ) || ( NULL == ullMicroSeconds ) ||
        ( NULL == pxTimerDesc->tiTimerHandle ) )
    {
        ret = IOT_TIMER_INVALID_VALUE;
    }
    else if( !pxTimerDesc->xIsRunning )
    {
        ret = IOT_TIMER_NOT_RUNNING;
    }
    else
    {
        uint32_t ulTimerCount = ClockP_getSystemTicks();

        if( ulTimerCount < pxTimerDesc->ulStartTimestamp )
        {
            ulTimerCount += ( ~( 0 ) ) - pxTimerDesc->ulStartTimestamp;
        }
        else if( ulTimerCount >= pxTimerDesc->ulStartTimestamp )
        {
            ulTimerCount -= pxTimerDesc->ulStartTimestamp;
        }

        *ullMicroSeconds = ( ( uint64_t ) ulTimerCount ) * ( ( uint64_t ) ClockP_getSystemTickPeriod() );
    }

    return ret;
}

/*-----------------------------------------------------------*/

int32_t iot_timer_delay( IotTimerHandle_t const pxTimerHandle,
                         uint32_t ulDelayMicroSeconds )
{
    int32_t ret = IOT_TIMER_SUCCESS;
    IotTimerDescriptor_t * pxTimerDesc = ( IotTimerDescriptor_t * ) pxTimerHandle;

    if( ( NULL == pxTimerDesc ) || ( 0 == ulDelayMicroSeconds ) ||
        ( NULL == pxTimerDesc->tiTimerHandle ) )
    {
        ret = IOT_TIMER_INVALID_VALUE;
    }
    else
    {
        uint32_t ulDelayinTicks = ulDelayMicroSeconds / ClockP_getSystemTickPeriod();
        ClockP_setTimeout( pxTimerDesc->tiTimerHandle, ulDelayinTicks );

        pxTimerDesc->xDelayIsSet = true;

        if( !pxTimerDesc->xIsRunning )
        {
            ret = IOT_TIMER_NOT_RUNNING;
        }
    }

    return ret;
}

/*-----------------------------------------------------------*/

int32_t iot_timer_cancel( IotTimerHandle_t const pxTimerHandle )
{
    int32_t ret = IOT_TIMER_SUCCESS;
    IotTimerDescriptor_t * pxTimerDesc = ( IotTimerDescriptor_t * ) pxTimerHandle;

    if( ( NULL == pxTimerDesc ) || ( NULL == pxTimerDesc->tiTimerHandle ) )
    {
        ret = IOT_TIMER_INVALID_VALUE;
    }
    else if( !pxTimerDesc->xIsRunning )
    {
        ret = IOT_TIMER_NOT_RUNNING;
    }
    else
    {
        ClockP_stop( pxTimerDesc->tiTimerHandle );
    }

    return ret;
}

/*-----------------------------------------------------------*/

int32_t iot_timer_close( IotTimerHandle_t const pxTimerHandle )
{
    int32_t ret = IOT_TIMER_SUCCESS;
    IotTimerDescriptor_t * pxTimerDesc = ( IotTimerDescriptor_t * ) pxTimerHandle;

    if( ( NULL == pxTimerDesc ) || ( NULL == pxTimerDesc->tiTimerHandle ) )
    {
        ret = IOT_TIMER_INVALID_VALUE;
    }
    else
    {
        ClockP_stop( pxTimerDesc->tiTimerHandle );
        pxTimerDesc->tiTimerHandle = NULL;
    }

    return ret;
}

/*-----------------------------------------------------------*/

static void prvTimerCallback( uintptr_t puTimerDesc )
{
    IotTimerDescriptor_t * pxTimerDesc = ( IotTimerDescriptor_t * ) puTimerDesc;

    if( pxTimerDesc->xIsRunning && pxTimerDesc->xTimerCallback )
    {
        pxTimerDesc->xTimerCallback( pxTimerDesc->pvUserCallbackContext );
    }
}
