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
 * @file    iot_tsensor.c
 * @brief   This file contains the definitions of temperature sensor APIs using the TI temperature drivers.
 */
#include <stdint.h>
#include <stdbool.h>
#include <unistd.h>

/* Common IO Header */
#include <iot_tsensor.h>

/* TI Drivers */
#include <ti/drivers/Temperature.h>

/* Driver config */
#include <ti_drivers_config.h>

/* Maximum number of tsensor supported (soft limit) */
#define NUMBER_OF_SUPPORTED_TSENSOR_INSTANCES    ( 3 )

/**
 * @brief TI Simplelink temperature sensor HAL context abstracted in the Peripheral_Descriptor_t.
 *
 * Contains all the parameters needed for working with TI Simplelink Temperature driver.
 */
typedef struct
{
    Temperature_NotifyObj xNotifyObj;      /**< TI Driver object */
    IotTsensorCallback_t xTSensorCallback; /**< User registered callback. */
    void * pvUserCallbackContext;          /**< User callback context */
    bool xMaxThresholdIsSet;               /**< Max threshold set flag */
    bool xMinThresholdIsSet;               /**< Min threshold set flag */
    bool xIsOpen;                          /**< Instance is open */
    bool xIsDisabled;                      /**< Instance is disabled */
    int32_t ulMaxThreshold;                /**< Max threshold */
    int32_t ulMinThreshold;                /**< Min threshold */
} IotTSensorDescriptor_t;


void prvRegisterNotifications( IotTSensorDescriptor_t * pxTSensorDesc );
void prvNotificationCallback( int16_t sCurrentTemperature,
                              int16_t sThresholdTemperature,
                              uintptr_t upClientArg,
                              struct Temperature_NotifyObj * pxNotifyObject );

/**
 * @brief Static temperature sensor descriptor table
 */
static IotTSensorDescriptor_t xTSensor[ NUMBER_OF_SUPPORTED_TSENSOR_INSTANCES ];

static bool xIsInitialized = false;

/*-----------------------------------------------------------*/

IotTsensorHandle_t iot_tsensor_open( int32_t lTsensorInstance )
{
    IotTsensorHandle_t xHandle = NULL;

    if( false == xIsInitialized )
    {
        Temperature_init();

        uint32_t i;

        for( i = 0; i < NUMBER_OF_SUPPORTED_TSENSOR_INSTANCES; i++ )
        {
            xTSensor[ i ].xIsOpen = false;
        }

        xIsInitialized = true;
    }

    if( ( NUMBER_OF_SUPPORTED_TSENSOR_INSTANCES > lTsensorInstance ) &&
        ( lTsensorInstance >= 0 ) )
    {
        if( !xTSensor[ lTsensorInstance ].xIsOpen )
        {
            xHandle = ( IotTsensorHandle_t ) &xTSensor;

            /* Initialize default values */
            xTSensor[ lTsensorInstance ].xMaxThresholdIsSet = false;
            xTSensor[ lTsensorInstance ].xMinThresholdIsSet = false;
            xTSensor[ lTsensorInstance ].xIsDisabled = true;
            xTSensor[ lTsensorInstance ].xIsOpen = true;
        }
    }

    return xHandle;
}

/*-----------------------------------------------------------*/

void iot_tsensor_set_callback( IotTsensorHandle_t const xTsensorHandle,
                               IotTsensorCallback_t xCallback,
                               void * pvUserContext )
{
    IotTSensorDescriptor_t * pxTSensorDesc = ( IotTSensorDescriptor_t * ) xTsensorHandle;

    if( pxTSensorDesc && xCallback )
    {
        pxTSensorDesc->xTSensorCallback = xCallback;
        pxTSensorDesc->pvUserCallbackContext = pvUserContext;
    }
}

/*-----------------------------------------------------------*/

int32_t iot_tsensor_enable( IotTsensorHandle_t const xTsensorHandle )
{
    int32_t ret = IOT_TSENSOR_SUCCESS;
    IotTSensorDescriptor_t * pxTSensorDesc = ( IotTSensorDescriptor_t * ) xTsensorHandle;

    if( ( NULL == pxTSensorDesc ) || ( !pxTSensorDesc->xIsOpen ) )
    {
        ret = IOT_TSENSOR_INVALID_VALUE;
    }
    else
    {
        /* Regsiter notifications if any is set */
        prvRegisterNotifications( pxTSensorDesc );

        /* We are now "enabled" */
        pxTSensorDesc->xIsDisabled = false;
    }

    return ret;
}

/*-----------------------------------------------------------*/

int32_t iot_tsensor_disable( IotTsensorHandle_t const xTsensorHandle )
{
    int32_t ret = IOT_TSENSOR_SUCCESS;
    IotTSensorDescriptor_t * pxTSensorDesc = ( IotTSensorDescriptor_t * ) xTsensorHandle;

    if( ( NULL == pxTSensorDesc ) || ( !pxTSensorDesc->xIsOpen ) )
    {
        ret = IOT_TSENSOR_INVALID_VALUE;
    }
    else
    {
        /* Unsubscribe to notifications if any */
        Temperature_unregisterNotify( &pxTSensorDesc->xNotifyObj );

        /* Set to "disabled" */
        pxTSensorDesc->xIsDisabled = true;
    }

    return ret;
}

/*-----------------------------------------------------------*/

int32_t iot_tsensor_get_temp( IotTsensorHandle_t const xTsensorHandle,
                              int32_t * plTemp )
{
    int32_t ret = IOT_TSENSOR_SUCCESS;
    IotTSensorDescriptor_t * pxTSensorDesc = ( IotTSensorDescriptor_t * ) xTsensorHandle;

    if( ( NULL == pxTSensorDesc ) || ( !pxTSensorDesc->xIsOpen ) ||
        ( NULL == plTemp ) )
    {
        ret = IOT_TSENSOR_INVALID_VALUE;
    }
    else if( pxTSensorDesc->xIsDisabled )
    {
        ret = IOT_TSENSOR_DISABLED;
    }
    else
    {
        /* Read out current temperature in C and convert it to fixed point */
        *( plTemp ) = ( ( uint32_t ) ( 0x0000FFFF & Temperature_getTemperature() ) ) * 1000;
    }

    return ret;
}

/*-----------------------------------------------------------*/

int32_t iot_tsensor_ioctl( IotTsensorHandle_t const xTsensorHandle,
                           IotTsensorIoctlRequest_t xRequest,
                           void * const pvBuffer )
{
    int32_t ret = IOT_TSENSOR_SUCCESS;
    IotTSensorDescriptor_t * pxTSensorDesc = ( IotTSensorDescriptor_t * ) xTsensorHandle;

    if( ( NULL == pxTSensorDesc ) ||
        ( ( NULL == pvBuffer ) && ( eTsensorPerformCalibration != xRequest ) ) )
    {
        ret = IOT_TSENSOR_INVALID_VALUE;
    }
    else if( !pxTSensorDesc->xIsOpen )
    {
        ret = IOT_TSENSOR_CLOSED;
    }
    else
    {
        switch( xRequest )
        {
            case eTsensorSetMinThreshold:
                pxTSensorDesc->ulMinThreshold = ( *( ( int32_t * ) pvBuffer ) / 1000 );
                pxTSensorDesc->xMinThresholdIsSet = true;
                prvRegisterNotifications( pxTSensorDesc );
                break;

            case eTsensorSetMaxThreshold:
                pxTSensorDesc->ulMaxThreshold = ( *( ( int32_t * ) pvBuffer ) / 1000 );
                pxTSensorDesc->xMaxThresholdIsSet = true;
                prvRegisterNotifications( pxTSensorDesc );
                break;

            case eTsensorGetMinThreshold:
                *( ( int32_t * ) pvBuffer ) = pxTSensorDesc->ulMinThreshold * 1000;
                break;

            case eTsensorGetMaxThreshold:
                *( ( int32_t * ) pvBuffer ) = pxTSensorDesc->ulMaxThreshold * 1000;
                break;

            case eTsensorPerformCalibration:
                ret = IOT_TSENSOR_NOT_SUPPORTED;
                break;

            default:
                ret = IOT_TSENSOR_INVALID_VALUE;
        }
    }

    return ret;
}

/*-----------------------------------------------------------*/

int32_t iot_tsensor_close( IotTsensorHandle_t const xTsensorHandle )
{
    int32_t ret = IOT_TSENSOR_SUCCESS;
    IotTSensorDescriptor_t * pxTSensorDesc = ( IotTSensorDescriptor_t * ) xTsensorHandle;

    if( ( NULL == pxTSensorDesc ) || ( !pxTSensorDesc->xIsOpen ) )
    {
        ret = IOT_TSENSOR_INVALID_VALUE;
    }
    else
    {
        Temperature_unregisterNotify( &pxTSensorDesc->xNotifyObj );
        pxTSensorDesc->xIsOpen = false;
    }

    return ret;
}

/*-----------------------------------------------------------*/

void prvRegisterNotifications( IotTSensorDescriptor_t * pxTSensorDesc )
{
    Temperature_unregisterNotify( &pxTSensorDesc->xNotifyObj );

    /* Check which kind of event we should subscribe to */
    if( ( pxTSensorDesc->xMaxThresholdIsSet && pxTSensorDesc->xMinThresholdIsSet ) )
    {
        Temperature_registerNotifyRange( &pxTSensorDesc->xNotifyObj,
                                         pxTSensorDesc->ulMaxThreshold,
                                         pxTSensorDesc->ulMinThreshold,
                                         prvNotificationCallback,
                                         ( uintptr_t ) pxTSensorDesc );
    }
    else if( pxTSensorDesc->xMaxThresholdIsSet )
    {
        Temperature_registerNotifyHigh( &pxTSensorDesc->xNotifyObj,
                                        pxTSensorDesc->ulMaxThreshold,
                                        prvNotificationCallback,
                                        ( uintptr_t ) pxTSensorDesc );
    }
    else if( pxTSensorDesc->xMinThresholdIsSet )
    {
        Temperature_registerNotifyLow( &pxTSensorDesc->xNotifyObj,
                                       pxTSensorDesc->ulMinThreshold,
                                       prvNotificationCallback,
                                       ( uintptr_t ) pxTSensorDesc );
    }
}

/*-----------------------------------------------------------*/

void prvNotificationCallback( int16_t sCurrentTemperature,
                              int16_t sThresholdTemperature,
                              uintptr_t upClientArg,
                              struct Temperature_NotifyObj * pxNotifyObject )
{
    IotTSensorDescriptor_t * pxTSensorDesc = ( IotTSensorDescriptor_t * ) upClientArg;

    if( pxTSensorDesc->xTSensorCallback &&
        pxTSensorDesc->xIsOpen &&
        !pxTSensorDesc->xIsDisabled )
    {
        IotTsensorStatus_t xStatus = eTsensorMinThresholdReached;

        if( sThresholdTemperature == pxTSensorDesc->ulMaxThreshold )
        {
            xStatus = eTsensorMaxThresholdReached;
        }

        pxTSensorDesc->xTSensorCallback( xStatus, pxTSensorDesc->pvUserCallbackContext );
    }
}
