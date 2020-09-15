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
 * @file    iot_watchdog.c
 * @brief   This file contains the definitions of Watchdog APIs using TI drivers.
 */
#include <stdint.h>
#include <unistd.h>
#include <stdbool.h>

/* Common IO Header */
#include <iot_watchdog.h>

/* TI Drivers and DPL */
#include <ti/drivers/Watchdog.h>
#include <ti/drivers/dpl/ClockP.h>

/* Driver config */
#include <ti_drivers_config.h>

/**
 * @brief TI Simplelink Watchdog HAL context abstracted in the Peripheral_Descriptor_t.
 *
 * Contains all the parameters needed for working with TI Simplelink Watchdog driver.
 */
typedef struct
{
    Watchdog_Handle tiWatchdogHandle;        /**< TI driver handle used with TI Driver API. */
    ClockP_Struct xBarkClock;                /**< TI clock structure used with TI Driver API. */
    IotWatchdogCallback_t xWatchdogCallback; /**< User registered callback. */
    void * pvUserCallbackContext;            /**< User callback context. */
    IotWatchdogStatus_t xStatus;             /**< Current status. */
    IotWatchdogBiteConfig_t xBiteMode;       /**< Current bitemode configuration. */
    uint32_t ulBarkTime;                     /**< Current bark timeout. */
    uint32_t ulBiteTime;                     /**< Current bite timeout. */
    uint32_t ulStoppedBiteTime;              /**< Bite timeout used when the driver is "stopped". */
    bool xIsOpen;                            /**< Driver open flag. */
} IotWatchdogDescriptor_t;

static void prvWatchdogCallback( Watchdog_Handle xHandle );
static void prvBarkCallback( uintptr_t xWatchdogDesc );

/**
 * @brief Static Watchdog descriptor table.
 */
static IotWatchdogDescriptor_t xWatchdog[ CONFIG_TI_DRIVERS_WATCHDOG_COUNT ];

static bool xIsInitialized = false;

/*-----------------------------------------------------------*/

IotWatchdogHandle_t iot_watchdog_open( int32_t lWatchdogInstance )
{
    Watchdog_Handle xTiHandle = NULL;
    IotWatchdogHandle_t xHandle = NULL;
    Watchdog_Params xWatchdogParams;
    ClockP_Params xClkParams;

    /* Initialize if needed */
    if( false == xIsInitialized )
    {
        Watchdog_init();

        ClockP_Params_init( &xClkParams );
        xClkParams.period = 0;
        xClkParams.startFlag = false;

        uint32_t i;

        for( i = 0; i < CONFIG_TI_DRIVERS_WATCHDOG_COUNT; i++ )
        {
            xWatchdog[ i ].tiWatchdogHandle = NULL;
            xWatchdog[ i ].xIsOpen = false;

            xClkParams.arg = ( uintptr_t ) &xWatchdog[ i ];
            ClockP_construct( &( xWatchdog[ i ].xBarkClock ),
                              ( ClockP_Fxn ) & prvBarkCallback,
                              0, &xClkParams );
        }

        xIsInitialized = true;
    }

    if( ( NULL == xWatchdog[ lWatchdogInstance ].tiWatchdogHandle ) &&
        !xWatchdog[ lWatchdogInstance ].xIsOpen )
    {
        Watchdog_Params_init( &xWatchdogParams );
        xWatchdogParams.resetMode = Watchdog_RESET_ON;
        xWatchdogParams.callbackFxn = ( Watchdog_Callback ) prvWatchdogCallback;

        xTiHandle = Watchdog_open( lWatchdogInstance, &xWatchdogParams );

        if( xTiHandle )
        {
            /* Calculate the highest possible reload value (with 50 minutes
             * ´* being the maximum to avoid having to loop for to long) as the
             * watchdog is already running. This is done so that we don't need
             * to suppress to many interrupts. The API is expected to return 0
             * on an coversion overflow so we need to loop it until we reach an
             * acceptable value. */
            uint32_t ulTimeout = 3000000;
            uint32_t ulBiteTime = Watchdog_convertMsToTicks( xTiHandle, ulTimeout );

            while( 0 == ulBiteTime )
            {
                ulTimeout -= 100000;
                ulBiteTime = Watchdog_convertMsToTicks( xTiHandle, ulTimeout );
            }

            xWatchdog[ lWatchdogInstance ].ulStoppedBiteTime = ulBiteTime;
            Watchdog_setReload( xTiHandle, ulBiteTime );

            xWatchdog[ lWatchdogInstance ].tiWatchdogHandle = xTiHandle;
            xWatchdog[ lWatchdogInstance ].ulBiteTime = 0;
            xWatchdog[ lWatchdogInstance ].ulBarkTime = 0;
            xWatchdog[ lWatchdogInstance ].xStatus = eWatchdogTimerStopped;
            xWatchdog[ lWatchdogInstance ].xIsOpen = true;

            xHandle = ( IotWatchdogHandle_t ) &xWatchdog[ lWatchdogInstance ];
        }
    }
    else if( ( NULL != xWatchdog[ lWatchdogInstance ].tiWatchdogHandle ) &&
             !xWatchdog[ lWatchdogInstance ].xIsOpen )
    {
        xWatchdog[ lWatchdogInstance ].ulBiteTime = 0;
        xWatchdog[ lWatchdogInstance ].ulBarkTime = 0;
        xWatchdog[ lWatchdogInstance ].xStatus = eWatchdogTimerStopped;
        xWatchdog[ lWatchdogInstance ].xIsOpen = true;

        xHandle = ( IotWatchdogHandle_t ) &xWatchdog[ lWatchdogInstance ];
    }

    return xHandle;
}

/*-----------------------------------------------------------*/

int32_t iot_watchdog_start( IotWatchdogHandle_t const pxWatchdogHandle )
{
    int32_t ret = IOT_WATCHDOG_SUCCESS;
    IotWatchdogDescriptor_t * pxWatchdogDesc = ( IotWatchdogDescriptor_t * ) pxWatchdogHandle;

    if( ( NULL == pxWatchdogDesc ) || ( !pxWatchdogDesc->xIsOpen ) )
    {
        ret = IOT_WATCHDOG_INVALID_VALUE;
    }
    else if( 0 == pxWatchdogDesc->ulBiteTime )
    {
        ret = IOT_WATCHDOG_TIME_NOT_SET;
    }
    else
    {
        /* "Start" the Watchdog by changing status from stopped to running.
         * This will disable interrupt suppression. */
        pxWatchdogDesc->xStatus = eWatchdogTimerRunning;

        if( 0 < pxWatchdogDesc->ulBarkTime )
        {
            ClockP_start( ClockP_handle( &pxWatchdogDesc->xBarkClock ) );
        }
    }

    return ret;
}

/*-----------------------------------------------------------*/

int32_t iot_watchdog_stop( IotWatchdogHandle_t const pxWatchdogHandle )
{
    int32_t ret = IOT_WATCHDOG_SUCCESS;
    IotWatchdogDescriptor_t * pxWatchdogDesc = ( IotWatchdogDescriptor_t * ) pxWatchdogHandle;

    if( ( NULL == pxWatchdogDesc ) || ( !pxWatchdogDesc->xIsOpen ) )
    {
        ret = IOT_WATCHDOG_INVALID_VALUE;
    }
    else if( 0 == pxWatchdogDesc->ulBiteTime )
    {
        ret = IOT_WATCHDOG_TIME_NOT_SET;
    }
    else
    {
        /* "Stop" the Watchdog timer by changing status from running to stopped.
         * This will enable interrupt suppression again. */
        pxWatchdogDesc->xStatus = eWatchdogTimerStopped;

        Watchdog_setReload( pxWatchdogDesc->tiWatchdogHandle, pxWatchdogDesc->ulStoppedBiteTime );
        ClockP_stop( ClockP_handle( &pxWatchdogDesc->xBarkClock ) );
    }

    return ret;
}

/*-----------------------------------------------------------*/

int32_t iot_watchdog_restart( IotWatchdogHandle_t const pxWatchdogHandle )
{
    int32_t ret = IOT_WATCHDOG_SUCCESS;
    IotWatchdogDescriptor_t * pxWatchdogDesc = ( IotWatchdogDescriptor_t * ) pxWatchdogHandle;

    if( ( NULL == pxWatchdogDesc ) || ( !pxWatchdogDesc->xIsOpen ) )
    {
        ret = IOT_WATCHDOG_INVALID_VALUE;
    }
    else if( 0 == pxWatchdogDesc->ulBiteTime )
    {
        ret = IOT_WATCHDOG_TIME_NOT_SET;
    }
    else
    {
        Watchdog_clear( pxWatchdogDesc->tiWatchdogHandle );
        ClockP_stop( ClockP_handle( &pxWatchdogDesc->xBarkClock ) );

        if( eWatchdogTimerStopped != pxWatchdogDesc->xStatus )
        {
            if( 0 < pxWatchdogDesc->ulBarkTime )
            {
                ClockP_start( ClockP_handle( &pxWatchdogDesc->xBarkClock ) );
            }

            pxWatchdogDesc->xStatus = eWatchdogTimerRunning;
        }
    }

    return ret;
}

/*-----------------------------------------------------------*/

void iot_watchdog_set_callback( IotWatchdogHandle_t const pxWatchdogHandle,
                                IotWatchdogCallback_t xCallback,
                                void * pvUserContext )
{
    IotWatchdogDescriptor_t * pxWatchdogDesc = ( IotWatchdogDescriptor_t * ) pxWatchdogHandle;

    if( pxWatchdogDesc && xCallback )
    {
        pxWatchdogDesc->xWatchdogCallback = xCallback;
        pxWatchdogDesc->pvUserCallbackContext = pvUserContext;
    }
}

/*-----------------------------------------------------------*/

int32_t iot_watchdog_ioctl( IotWatchdogHandle_t const pxWatchdogHandle,
                            IotWatchdogIoctlRequest_t xRequest,
                            void * const pvBuffer )
{
    int32_t ret = IOT_WATCHDOG_SUCCESS;
    IotWatchdogDescriptor_t * pxWatchdogDesc = ( IotWatchdogDescriptor_t * ) pxWatchdogHandle;

    if( ( NULL == pxWatchdogDesc ) || ( NULL == pvBuffer ) ||
        ( !pxWatchdogDesc->xIsOpen ) )
    {
        ret = IOT_WATCHDOG_INVALID_VALUE;
    }
    else
    {
        switch( xRequest )
        {
            case eSetWatchdogBarkTime:
               {
                   uint32_t ulNewBarkTime = *( ( uint32_t * ) pvBuffer );
                   pxWatchdogDesc->ulBarkTime = ulNewBarkTime;

                   if( ( pxWatchdogDesc->ulBiteTime > 0 ) && ( pxWatchdogDesc->ulBiteTime < ulNewBarkTime ) )
                   {
                       ret = IOT_WATCHDOG_INVALID_VALUE;
                   }
                   else
                   {
                       if( 0 == ulNewBarkTime )
                       {
                           ClockP_stop( ClockP_handle( &pxWatchdogDesc->xBarkClock ) );
                       }

                       /* Convert ms to ticks */
                       ulNewBarkTime *= 1000 / ClockP_getSystemTickPeriod();
                       ClockP_setTimeout( ClockP_handle( &pxWatchdogDesc->xBarkClock ), ulNewBarkTime );
                   }

                   break;
               }

            case eGetWatchdogBarkTime:
                *( ( uint32_t * ) pvBuffer ) = pxWatchdogDesc->ulBarkTime;
                break;

            case eSetWatchdogBiteTime:
               {
                   uint32_t ulNewBiteTime = *( ( uint32_t * ) pvBuffer );

                   if( 0 == ulNewBiteTime )
                   {
                       ret = IOT_WATCHDOG_TIME_NOT_SET;
                   }
                   else if( ulNewBiteTime < pxWatchdogDesc->ulBarkTime )
                   {
                       ret = IOT_WATCHDOG_INVALID_VALUE;
                   }
                   else
                   {
                       /* Convert ms to ticks */
                       uint32_t ulNewBiteTimeInTicks = Watchdog_convertMsToTicks( pxWatchdogDesc->tiWatchdogHandle, ulNewBiteTime );

                       if( 0 == ulNewBiteTimeInTicks )
                       {
                           ret = IOT_WATCHDOG_INVALID_VALUE;
                       }
                       else
                       {
                           pxWatchdogDesc->ulBiteTime = ulNewBiteTime;

                           /* Actual "bite" in terms of reset is device specific.
                            * For CC13XX/CC26XX, CC32XX and MSP4320, this would be
                            * 2 times the bite time as the first "unserved" interrupt
                            * would cause a reset. */
                           Watchdog_setReload( pxWatchdogDesc->tiWatchdogHandle, ulNewBiteTimeInTicks );
                       }
                   }

                   break;
               }

            case eGetWatchdogBiteTime:
                *( ( uint32_t * ) pvBuffer ) = pxWatchdogDesc->ulBiteTime;
                break;

            case eGetWatchdogStatus:
                *( ( IotWatchdogStatus_t * ) pvBuffer ) = pxWatchdogDesc->xStatus;
                break;

            case eSetWatchdogBiteBehaviour:
               {
                   IotWatchdogBiteConfig_t xBiteConfig = *( ( IotWatchdogBiteConfig_t * ) pvBuffer );

                   pxWatchdogDesc->xBiteMode = xBiteConfig;
                   break;
               }

            default:
                ret = IOT_WATCHDOG_INVALID_VALUE;
                break;
        }
    }

    return ret;
}

/*-----------------------------------------------------------*/

int32_t iot_watchdog_close( IotWatchdogHandle_t const pxWatchdogHandle )
{
    int32_t ret = IOT_WATCHDOG_SUCCESS;
    IotWatchdogDescriptor_t * pxWatchdogDesc = ( IotWatchdogDescriptor_t * ) pxWatchdogHandle;

    if( ( NULL == pxWatchdogDesc ) || ( !pxWatchdogDesc->xIsOpen ) )
    {
        ret = IOT_WATCHDOG_INVALID_VALUE;
    }
    else
    {
        iot_watchdog_stop( pxWatchdogHandle );
        pxWatchdogDesc->xIsOpen = false;
    }

    return ret;
}

/*-----------------------------------------------------------*/

static void prvWatchdogCallback( Watchdog_Handle xHandle )
{

    IotWatchdogDescriptor_t * pxWatchdogDesc = NULL;
    uint32_t i;

    for( i = 0; i < CONFIG_TI_DRIVERS_WATCHDOG_COUNT; i++ )
    {
        if( xWatchdog[ i ].tiWatchdogHandle == xHandle )
        {
            pxWatchdogDesc = ( IotWatchdogDescriptor_t * ) &xWatchdog[ i ];
            break;
        }
    }

    if( ( eWatchdogTimerStopped == pxWatchdogDesc->xStatus ) ||
        ( !pxWatchdogDesc->xIsOpen ) )
    {
        /* Suppress the interrupt. */
        Watchdog_clear( pxWatchdogDesc->tiWatchdogHandle );
    }
    else
    {
        pxWatchdogDesc->xStatus = eWatchdogTimerBiteExpired;

        if( ( eWatchdogBiteTimerInterrupt == pxWatchdogDesc->xBiteMode ) &&
            pxWatchdogDesc->xWatchdogCallback )
        {
            pxWatchdogDesc->xWatchdogCallback( pxWatchdogDesc->pvUserCallbackContext );
        }
        else if( eWatchdogBiteTimerReset == pxWatchdogDesc->xBiteMode )
        {
            /* If set to eWatchdogBiteTimerReset, set a 0 reload value so that
             * a second interrupt is triggered right away. This to account for
             * the fact that we need a second, unserved, interrupt to reset
             * the CC13XX/CC26XX, CC32XX and MSP4320 devices. */
            Watchdog_setReload( pxWatchdogDesc->tiWatchdogHandle, 0 );
        }
    }
}

/*-----------------------------------------------------------*/

static void prvBarkCallback( uintptr_t xWatchdogDesc )
{
    IotWatchdogDescriptor_t * pxWatchdogDesc = ( IotWatchdogDescriptor_t * ) xWatchdogDesc;

    pxWatchdogDesc->xStatus = eWatchdogTimerBarkExpired;

    if( pxWatchdogDesc->xWatchdogCallback )
    {
        pxWatchdogDesc->xWatchdogCallback( pxWatchdogDesc->pvUserCallbackContext );
    }

    if( 0 != pxWatchdogDesc->ulBarkTime )
    {
        ClockP_start( ClockP_handle( &pxWatchdogDesc->xBarkClock ) );
    }
}
