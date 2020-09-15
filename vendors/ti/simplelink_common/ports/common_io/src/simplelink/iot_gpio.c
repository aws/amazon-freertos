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
 * @file    iot_gpio.c
 * @brief   This file contains the definitions of GPIO APIs using TI drivers.
 */
#include <stdint.h>
#include <unistd.h>
#include <stdbool.h>

#include <iot_gpio.h>

/* TI Drivers */
#include <ti/drivers/GPIO.h>

/* Driver config */
#include <ti_drivers_config.h>

/**
 * @brief TI GPIO driver context.
 *
 * Contains all the parameters needed for working with the TI Simplelink GPIO Driver.
 */
typedef struct
{
    uint32_t ulGpioIndex;        /**< TI driver GPIO index, used with TI Driver API. */
    bool xGpioOwned;             /**< GPIO is being used. */
    IotGpioCallback_t xCallback; /**< User registered callback. */
    void * pvUserContext;        /**< User callback context */
} IotGPIODescriptor_t;

static void prvGpioCallback( uint_least8_t index );

/**
 * @brief Reset config for all "open" GPIO pins. Set to input, no pull.
 */
static const GPIO_PinConfig xGpioResetConfig = GPIO_CFG_IN_NOPULL;

/**
 * @brief TI GPIO Driver configurations to GPIO HAL mapping tables.
 */
static const GPIO_PinConfig xGpioDirection[] =
{
    GPIO_CFG_INPUT | GPIO_CFG_IN_INT_NONE, /*!< GPIO input direction */
    GPIO_CFG_OUT_STD,                      /*!< GPIO output direction */
};

static const GPIO_PinConfig xGpioInputPull[] =
{
    GPIO_CFG_IN_NOPULL, /*!< GPIO input no-pull */
    GPIO_CFG_IN_PU,     /*!< GPIO input pull-up */
    GPIO_CFG_IN_PD      /*!< GPIO input pull-down */
};

static const GPIO_PinConfig xGpioOutputPull[] =
{
    GPIO_CFG_OUT_OD_NOPULL, /*!< GPIO input no-pull */
    GPIO_CFG_OUT_OD_PU,     /*!< GPIO input pull-up */
    GPIO_CFG_OUT_OD_PD      /*!< GPIO input pull-down */
};

static const GPIO_PinConfig xGpioOutputMode[] =
{
    GPIO_CFG_OUT_OD_NOPULL, /*!< GPIO mode open-drain */
    GPIO_CFG_OUT_STD,       /*!< GPIO input push-pull */
};

static const GPIO_PinConfig xGpioInterrupt[] =
{
    GPIO_CFG_IN_INT_ONLY | GPIO_CFG_IN_INT_NONE,       /*!< No GPIO interrupt */
    GPIO_CFG_IN_INT_ONLY | GPIO_CFG_IN_INT_RISING,     /*!< Rising-edge interrupt */
    GPIO_CFG_IN_INT_ONLY | GPIO_CFG_IN_INT_FALLING,    /*!< Falling-edge interrupt */
    GPIO_CFG_IN_INT_ONLY | GPIO_CFG_IN_INT_BOTH_EDGES, /*!< Any edge interrupt */
    GPIO_CFG_IN_INT_ONLY | GPIO_CFG_IN_INT_LOW,        /*!< Low level interrupt */
    GPIO_CFG_IN_INT_ONLY | GPIO_CFG_IN_INT_HIGH        /*!< High level interrupt */
};

/**
 * @brief Static GPIO descriptor table
 */
static IotGPIODescriptor_t xGpio[ CONFIG_TI_DRIVERS_GPIO_COUNT ];

static bool xIsInitialized = false;

/*-----------------------------------------------------------*/

IotGpioHandle_t iot_gpio_open( int32_t lGpioNumber )
{
    IotGpioHandle_t xHandle = NULL;

    if( false == xIsInitialized )
    {
        GPIO_init();

        uint32_t i;

        for( i = 0; i < CONFIG_TI_DRIVERS_GPIO_COUNT; i++ )
        {
            xGpio[ i ].ulGpioIndex = i;
            xGpio[ i ].xGpioOwned = false;
        }

        xIsInitialized = true;
    }

    if( ( 0 <= lGpioNumber ) && ( CONFIG_TI_DRIVERS_GPIO_COUNT > lGpioNumber ) )
    {
        if( false == xGpio[ lGpioNumber ].xGpioOwned )
        {
            xGpio[ lGpioNumber ].xGpioOwned = true;
            xGpio[ lGpioNumber ].xCallback = NULL;
            xGpio[ lGpioNumber ].pvUserContext = NULL;

            GPIO_setConfig( xGpio[ lGpioNumber ].ulGpioIndex, xGpioResetConfig );

            xHandle = ( IotGpioHandle_t ) &xGpio[ lGpioNumber ];
        }
    }

    return xHandle;
}

/*-----------------------------------------------------------*/

void iot_gpio_set_callback( IotGpioHandle_t const pxGpio,
                            IotGpioCallback_t xGpioCallback,
                            void * pvUserContext )
{
    IotGPIODescriptor_t * pxGpioDesc = ( IotGPIODescriptor_t * ) pxGpio;

    if( pxGpioDesc && ( pxGpioDesc->xGpioOwned ) )
    {
        if( xGpioCallback )
        {
            GPIO_setCallback( pxGpioDesc->ulGpioIndex, prvGpioCallback );
            pxGpioDesc->xCallback = xGpioCallback;
        }
        else
        {
            GPIO_setCallback( pxGpioDesc->ulGpioIndex, NULL );
            pxGpioDesc->xCallback = NULL;
        }

        pxGpioDesc->pvUserContext = pvUserContext;
    }
}

/*-----------------------------------------------------------*/

int32_t iot_gpio_read_sync( IotGpioHandle_t const pxGpio,
                            uint8_t * pucPinState )
{
    int32_t ret = IOT_GPIO_SUCCESS;
    IotGPIODescriptor_t * pxGpioDesc = ( IotGPIODescriptor_t * ) pxGpio;

    if( ( NULL == pxGpioDesc ) || ( NULL == pucPinState ) ||
        ( false == pxGpioDesc->xGpioOwned ) )
    {
        ret = IOT_GPIO_INVALID_VALUE;
    }
    else
    {
        *pucPinState = GPIO_read( pxGpioDesc->ulGpioIndex );
    }

    return ret;
}

/*-----------------------------------------------------------*/

int32_t iot_gpio_write_sync( IotGpioHandle_t const pxGpio,
                             uint8_t ucPinState )
{
    int32_t ret = IOT_GPIO_SUCCESS;
    IotGPIODescriptor_t * pxGpioDesc = ( IotGPIODescriptor_t * ) pxGpio;

    if( ( NULL == pxGpioDesc ) || ( false == pxGpioDesc->xGpioOwned ) )
    {
        ret = IOT_GPIO_INVALID_VALUE;
    }
    else
    {
        GPIO_write( pxGpioDesc->ulGpioIndex, ucPinState );
    }

    return ret;
}

/*-----------------------------------------------------------*/

int32_t iot_gpio_close( IotGpioHandle_t const pxGpio )
{
    int32_t ret = IOT_GPIO_SUCCESS;
    IotGPIODescriptor_t * pxGpioDesc = ( IotGPIODescriptor_t * ) pxGpio;

    if( ( NULL == pxGpioDesc ) || ( false == pxGpioDesc->xGpioOwned ) )
    {
        ret = IOT_GPIO_INVALID_VALUE;
    }
    else
    {
        GPIO_disableInt( pxGpioDesc->ulGpioIndex );
        GPIO_setCallback( pxGpioDesc->ulGpioIndex, NULL );
        GPIO_setConfig( pxGpioDesc->ulGpioIndex, xGpioResetConfig );
        pxGpioDesc->xGpioOwned = false;
    }

    return ret;
}

/*-----------------------------------------------------------*/

int32_t iot_gpio_ioctl( IotGpioHandle_t const pxGpio,
                        IotGpioIoctlRequest_t xRequest,
                        void * const pvBuffer )
{
    int32_t ret = IOT_GPIO_SUCCESS;
    IotGPIODescriptor_t * pxGpioDesc = ( IotGPIODescriptor_t * ) pxGpio;

    if( ( NULL == pxGpioDesc ) ||
        ( false == pxGpioDesc->xGpioOwned ) ||
        ( NULL == pvBuffer ) )
    {
        ret = IOT_GPIO_INVALID_VALUE;
    }
    else
    {
        GPIO_PinConfig xCurrentConfig;
        GPIO_getConfig( pxGpioDesc->ulGpioIndex, &xCurrentConfig );

        /* Mask out IO configuration */
        GPIO_PinConfig xCurrentIoConfig = ( ( 0x7 << GPIO_CFG_IO_LSB ) & xCurrentConfig );

        bool xIsInput = ( bool ) ( ( xCurrentIoConfig >> GPIO_CFG_IO_LSB ) % 2 );

        switch( xRequest )
        {
            case eSetGpioDirection:
               {
                   IotGpioDirection_t xDir = *( ( IotGpioDirection_t * ) pvBuffer );
                   GPIO_setConfig( pxGpioDesc->ulGpioIndex, xGpioDirection[ xDir ] );
                   break;
               }

            case eSetGpioPull:
               {
                   IotGpioPull_t xPull = *( ( IotGpioPull_t * ) pvBuffer );

                   if( xIsInput )
                   {
                       GPIO_setConfig( pxGpioDesc->ulGpioIndex, xGpioInputPull[ xPull ] );
                   }
                   else
                   {
                       /* Make sure it is in open-drain mode before setting PU/PD */
                       if( 0 < xCurrentIoConfig )
                       {
                           GPIO_setConfig( pxGpioDesc->ulGpioIndex, xGpioOutputPull[ xPull ] );
                       }
                   }

                   break;
               }

            case eSetGpioOutputMode:
               {
                   IotGpioOutputMode_t xMode = *( ( IotGpioOutputMode_t * ) pvBuffer );

                   if( !xIsInput )
                   {
                       GPIO_setConfig( pxGpioDesc->ulGpioIndex, xGpioOutputMode[ xMode ] );
                   }

                   break;
               }

            case eSetGpioInterrupt:
               {
                   IotGpioInterrupt_t xInt = *( ( IotGpioInterrupt_t * ) pvBuffer );

                   /* "Low" and "high" interrupts are not supported by all devices */
                   #if ( defined DeviceFamily_CC13X2 ) || ( defined DeviceFamily_CC26X2 ) || ( defined DeviceFamily_CC13X2_CC26X2 )
                       if( ( eGpioInterruptLow == xInt ) ||
                           ( eGpioInterruptHigh == xInt ) )
                       {
                           ret = IOT_GPIO_FUNCTION_NOT_SUPPORTED;
                           break;
                       }
                   #endif
                   GPIO_setConfig( pxGpioDesc->ulGpioIndex, xGpioInterrupt[ xInt ] );

                   if( eGpioInterruptNone != xInt )
                   {
                       GPIO_enableInt( pxGpioDesc->ulGpioIndex );
                   }
                   else
                   {
                       GPIO_disableInt( pxGpioDesc->ulGpioIndex );
                   }

                   break;
               }

            case eSetGpioDriveStrength:
               {
                   /* Input value is defined by HW. Re-using the same defines as in
                    * the TI Driver GPIO.h */
                   GPIO_PinConfig xNewConfig = *( ( GPIO_PinConfig * ) pvBuffer );

                   if( ( GPIO_CFG_OUT_STR_LOW != xNewConfig ) &&
                       ( GPIO_CFG_OUT_STR_MED != xNewConfig ) &&
                       ( GPIO_CFG_OUT_STR_HIGH != xNewConfig ) )
                   {
                       ret = IOT_GPIO_INVALID_VALUE;
                   }
                   else
                   {
                       GPIO_setConfig( pxGpioDesc->ulGpioIndex, xNewConfig );
                   }

                   break;
               }

            case eGetGpioDirection:
                *( ( IotGpioDirection_t * ) pvBuffer ) = eGpioDirectionOutput;

                if( xIsInput )
                {
                    *( ( IotGpioDirection_t * ) pvBuffer ) = eGpioDirectionInput;
                }

                break;

            case eGetGpioPull:
                *( ( IotGpioPull_t * ) pvBuffer ) = eGpioPullNone;

                if( ( GPIO_CFG_IN_NOPULL == xCurrentIoConfig ) ||
                    ( GPIO_CFG_OUT_OD_NOPULL == xCurrentIoConfig ) )
                {
                    *( ( IotGpioPull_t * ) pvBuffer ) = eGpioPullNone;
                }
                else if( ( GPIO_CFG_IN_PU == xCurrentIoConfig ) ||
                         ( GPIO_CFG_OUT_OD_PU == xCurrentIoConfig ) )
                {
                    *( ( IotGpioPull_t * ) pvBuffer ) = eGpioPullUp;
                }
                else if( ( GPIO_CFG_IN_PD == xCurrentIoConfig ) ||
                         ( GPIO_CFG_OUT_OD_PD == xCurrentIoConfig ) )
                {
                    *( ( IotGpioPull_t * ) pvBuffer ) = eGpioPullDown;
                }

                break;

            case eGetGpioOutputType:
                *( ( IotGpioOutputMode_t * ) pvBuffer ) = eGpioOpenDrain;

                if( xCurrentIoConfig == GPIO_CFG_OUT_STD )
                {
                    *( ( IotGpioOutputMode_t * ) pvBuffer ) = eGpioPushPull;
                }

                break;

            case eGetGpioInterrupt:
                xCurrentConfig &= GPIO_CFG_INT_MASK;

                *( ( IotGpioInterrupt_t * ) pvBuffer ) = eGpioInterruptNone;

                if( GPIO_CFG_IN_INT_FALLING == xCurrentConfig )
                {
                    *( ( IotGpioInterrupt_t * ) pvBuffer ) = eGpioInterruptFalling;
                }
                else if( GPIO_CFG_IN_INT_RISING == xCurrentConfig )
                {
                    *( ( IotGpioInterrupt_t * ) pvBuffer ) = eGpioInterruptRising;
                }
                else if( GPIO_CFG_IN_INT_BOTH_EDGES == xCurrentConfig )
                {
                    *( ( IotGpioInterrupt_t * ) pvBuffer ) = eGpioInterruptEdge;
                }
                else if( GPIO_CFG_IN_INT_LOW == xCurrentConfig )
                {
                    *( ( IotGpioInterrupt_t * ) pvBuffer ) = eGpioInterruptLow;
                }
                else if( GPIO_CFG_IN_INT_HIGH == xCurrentConfig )
                {
                    *( ( IotGpioInterrupt_t * ) pvBuffer ) = eGpioInterruptHigh;
                }

                break;

            case eGetGpioDriveStrength:
                xCurrentConfig &= GPIO_CFG_OUT_STRENGTH_MASK;
                *( ( int32_t * ) pvBuffer ) = xCurrentConfig;

                break;

            case eSetGpioFunction:
            case eGetGpioFunction:
            case eSetGpioSpeed:
            case eGetGpioSpeed:
                ret = IOT_GPIO_FUNCTION_NOT_SUPPORTED;
                break;

            default:
                ret = IOT_GPIO_INVALID_VALUE;
                break;
        }
    }

    return ret;
}

/*-----------------------------------------------------------*/

static void prvGpioCallback( uint_least8_t index )
{
    if( ( CONFIG_TI_DRIVERS_GPIO_COUNT > index ) && xGpio[ index ].xGpioOwned )
    {
        if( xGpio[ index ].xCallback )
        {
            uint8_t ucState = GPIO_read( index );
            xGpio[ index ].xCallback( ucState, xGpio[ index ].pvUserContext );
        }
    }
}
