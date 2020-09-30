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

#include <stdint.h>
#include <unistd.h>

#include <iot_pwm.h>

#include <ti/drivers/PWM.h>
#include <ti/drivers/dpl/ClockP.h>

#include <ti_drivers_config.h>

typedef struct
{
    PWM_Handle xTiPwm;      /**< TI driver handle used with TI Driver API. */
    IotPwmConfig_t xConfig; /**< Active PWM config. */
} IotPWMDescriptor_t;

static IotPWMDescriptor_t xPWM[ CONFIG_TI_DRIVERS_PWM_COUNT ];

static bool xIsInitialized = false;

/*-----------------------------------------------------------*/

IotPwmHandle_t iot_pwm_open( int32_t lPwmInstance )
{
    PWM_Handle xTiHandle = NULL;
    IotPwmHandle_t xHandle = NULL;
    PWM_Params xParams;

    if( false == xIsInitialized )
    {
        PWM_init();
        xIsInitialized = true;
    }

    PWM_Params_init( &xParams );

    xTiHandle = PWM_open( lPwmInstance, &xParams );

    if( xTiHandle )
    {
        xHandle = ( IotPwmHandle_t ) &xPWM[ lPwmInstance ];

        /* Initialize default values */
        xPWM[ lPwmInstance ].xTiPwm = xTiHandle;
        xPWM[ lPwmInstance ].xConfig.ulPwmFrequency = xParams.periodValue;
        xPWM[ lPwmInstance ].xConfig.ucPwmDutyCycle = 0;

        /* Set PWM channel to 0xFF to indicate that the driver
         * is yet to be configured */
        xPWM[ lPwmInstance ].xConfig.ucPwmChannel = 0xFF;
    }

    return xHandle;
}

/*-----------------------------------------------------------*/

IotPwmConfig_t * iot_pwm_get_config( IotPwmHandle_t const pxPwmHandle )
{
    IotPwmConfig_t * xRetConfig = NULL;
    IotPWMDescriptor_t * pxPWMDesc = ( IotPWMDescriptor_t * ) pxPwmHandle;

    if( ( NULL != pxPWMDesc ) && ( NULL != pxPWMDesc->xTiPwm ) )
    {
        xRetConfig = &( pxPWMDesc->xConfig );
    }

    return xRetConfig;
}

/*-----------------------------------------------------------*/

int32_t iot_pwm_set_config( IotPwmHandle_t const pxPwmHandle,
                            const IotPwmConfig_t xConfig )
{
    int32_t ret = IOT_PWM_SUCCESS;
    IotPWMDescriptor_t * pxPWMDesc = ( IotPWMDescriptor_t * ) pxPwmHandle;

    if( ( NULL == pxPWMDesc ) || ( NULL == pxPWMDesc->xTiPwm ) )
    {
        ret = IOT_PWM_INVALID_VALUE;
    }
    else
    {
        ClockP_FreqHz freq;
        ClockP_getCpuFreq( &freq );

        if( ( freq.lo < xConfig.ulPwmFrequency ) ||
            ( 100 < xConfig.ucPwmDutyCycle ) ||
            ( 0 < xConfig.ucPwmChannel ) )
        {
            ret = IOT_PWM_INVALID_VALUE;
        }
        else
        {
            pxPWMDesc->xConfig.ulPwmFrequency = xConfig.ulPwmFrequency;
            pxPWMDesc->xConfig.ucPwmDutyCycle = xConfig.ucPwmDutyCycle;
            int16_t status = PWM_setPeriod( pxPWMDesc->xTiPwm, xConfig.ulPwmFrequency );

            if( PWM_STATUS_SUCCESS != status )
            {
                ret = IOT_PWM_INVALID_VALUE;
            }
            else
            {
                uint32_t ulNewDuty = ( uint32_t ) ( ( ( uint64_t ) PWM_DUTY_FRACTION_MAX * xConfig.ucPwmDutyCycle ) / 100 );
                status = PWM_setDuty( pxPWMDesc->xTiPwm, ulNewDuty );

                if( PWM_STATUS_SUCCESS != status )
                {
                    ret = IOT_PWM_INVALID_VALUE;
                }
                else
                {
                    /* Mark as "configured" */
                    pxPWMDesc->xConfig.ucPwmChannel = 0;
                }
            }
        }
    }

    return ret;
}

/*-----------------------------------------------------------*/

int32_t iot_pwm_start( IotPwmHandle_t const pxPwmHandle )
{
    int32_t ret = IOT_PWM_SUCCESS;
    IotPWMDescriptor_t * pxPWMDesc = ( IotPWMDescriptor_t * ) pxPwmHandle;

    if( ( NULL == pxPWMDesc ) || ( NULL == pxPWMDesc->xTiPwm ) )
    {
        ret = IOT_PWM_INVALID_VALUE;
    }
    else if( pxPWMDesc->xConfig.ucPwmChannel != 0 )
    {
        ret = IOT_PWM_NOT_CONFIGURED;
    }
    else
    {
        PWM_start( pxPWMDesc->xTiPwm );
    }

    return ret;
}

/*-----------------------------------------------------------*/

int32_t iot_pwm_stop( IotPwmHandle_t const pxPwmHandle )
{
    int32_t ret = IOT_PWM_SUCCESS;
    IotPWMDescriptor_t * pxPWMDesc = ( IotPWMDescriptor_t * ) pxPwmHandle;

    if( ( NULL == pxPWMDesc ) || ( NULL == pxPWMDesc->xTiPwm ) )
    {
        ret = IOT_PWM_INVALID_VALUE;
    }
    else
    {
        PWM_stop( pxPWMDesc->xTiPwm );
    }

    return ret;
}

/*-----------------------------------------------------------*/

int32_t iot_pwm_close( IotPwmHandle_t const pxPwmHandle )
{
    int32_t ret = IOT_PWM_SUCCESS;
    IotPWMDescriptor_t * pxPWMDesc = ( IotPWMDescriptor_t * ) pxPwmHandle;

    if( ( NULL == pxPWMDesc ) || ( NULL == pxPWMDesc->xTiPwm ) )
    {
        ret = IOT_PWM_INVALID_VALUE;
    }
    else
    {
        PWM_close( pxPWMDesc->xTiPwm );
        pxPWMDesc->xTiPwm = NULL;
    }

    return ret;
}
