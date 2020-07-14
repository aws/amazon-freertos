/*
 * FreeRTOS Common IO V0.1.1
 * Copyright (C) 2020 Amazon.com, Inc. or its affiliates.  All Rights Reserved.
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
 *
 * http://aws.amazon.com/freertos
 * http://www.FreeRTOS.org
 */

/**
 * @file iot_gpio.c
 * @brief HAL GPIO implementation on NRF52840 Development Kit. Note, CommonIO GPIO pin-index-space
 *        is separate from board-specific pin index space. User is expected to provide a 1-to-1 mapping
 *        between these spaces.
 *
 */

#include "FreeRTOS.h"

/* Nordic Board includes. */
#include "nrf_gpio.h"
#include "nrf_drv_gpiote.h"

/* Common IO includes */
#include "iot_gpio.h"
#include "iot_gpio_config.h"

/* Logging Includes */
#include "iot_logging_task.h"

/* Note, this depends on logging task being created and running */
#if ( IOT_GPIO_LOGGING_ENABLED == 1 )
    #define IOT_GPIO_MODULE_NAME    "[CommonIO][GPIO]"
    #define IOT_GPIO_LOGF( format, ... )    vLoggingPrintf( IOT_GPIO_MODULE_NAME format, __VA_ARGS__ )
    #define IOT_GPIO_LOG( msg )             vLoggingPrintf( IOT_GPIO_MODULE_NAME msg )
#else
    #define IOT_GPIO_LOGF( format, ... )
    #define IOT_GPIO_LOG( msg )
#endif


typedef enum
{
    IOT_GPIO_CLOSED = 0u,
    IOT_GPIO_OPENED = 1u
} IotGpioState_t;

typedef struct
{
    IotGpioDirection_t xDirection;
    IotGpioOutputMode_t xOutMode;
    int32_t lDriveStrength;
    IotGpioPull_t xPull;
    IotGpioInterrupt_t xInterruptMode;
} IotGpioConfig_t;

/**
 * @brief   GPIO descriptor type defined in the source file.
 */
typedef struct IotGpioDescriptor
{
    int32_t lGpioNumber;
    IotGpioConfig_t xConfig;
    IotGpioCallback_t xUserCallback;
    void * pvUserContext;
    uint8_t ucState;
} IotGpioDescriptor_t;

/*
 * A 1-to-1 mapping set by iot_gpio_config.h. CommonIO Gpio pin index space uses the index i
 * and the board specific identifier is stored at GpioMap[i]. The mapping is user configured.
 */
static const int32_t plGpioMap[] = IOT_COMMON_IO_GPIO_PIN_MAP;

static IotGpioDescriptor_t pxGpioDesc[ IOT_COMMON_IO_GPIO_NUMBER_OF_PINS ];

static const IotGpioDescriptor_t xDefaultGpioDesc =
{
    .lGpioNumber        = -1,
    .xConfig            =
    {
        .xDirection     = eGpioDirectionInput,
        .xOutMode       = eGpioPushPull,
        .lDriveStrength = NRF_GPIO_PIN_S0S1,
        .xPull          = eGpioPullNone,
        .xInterruptMode = eGpioInterruptNone
    },
    .xUserCallback      = NULL,
    .pvUserContext      = NULL,
    .ucState            = IOT_GPIO_CLOSED
};

/*---------------------------------------------------------------------------------*
*                                Private Helpers                                  *
*---------------------------------------------------------------------------------*/

/*
 * @brief   Used to validate whether a CommonIO pin index is a valid argument for iot_gpio_open.
 *          This lGpioNumber will be used to index xGpioDesc and plGpioMap.
 *
 * @param[in] lGpioNumber CommonIO pin index to validate
 *
 * @return    true if pin is okay to use with iot_gpio_open, else false
 */
static bool prvIsValidPinIndex( int32_t lGpioNumber )
{
    int32_t ulNumberOfMappings = sizeof( plGpioMap ) == 0 ? 0 : sizeof( plGpioMap ) / sizeof( plGpioMap[ 0 ] );

    return ( ulNumberOfMappings > 0 ) &&
           ( ulNumberOfMappings == IOT_COMMON_IO_GPIO_NUMBER_OF_PINS ) &&
           ( 0 <= lGpioNumber && lGpioNumber < ulNumberOfMappings );
}


/*
 * @brief   Used as a first-order check by all API functions that take a IotGpioHandle_t
 *
 * @param[in] pxGpio  IotGpioHandle_t to validate
 *
 * @return    true if handle is available for API interface
 *
 */
static bool prvIsValidHandle( IotGpioHandle_t const pxGpio )
{
    return ( pxGpio != NULL ) && ( pxGpio->ucState == IOT_GPIO_OPENED );
}

/*
 * @brief   Maps CommonIO Drive mode setting to corresponding HAL setting.
 *
 * @param[in] xOutMode      CommonIO setting to map to HAL setting
 * @param{in] xOutMode_NRF  Address to store board HAL specific value hat corresponds with xOutMode
 *
 * @return    true if the xOutMode is within range of valid HAL settings and could map
 *
 */
static bool prvGetOutModeForHAL( IotGpioOutputMode_t xOutMode,
                                 nrf_gpio_pin_drive_t * xOutMode_NRF )
{
    bool bReturnCode = true;

    switch( xOutMode )
    {
        case eGpioOpenDrain:
            *xOutMode_NRF = NRF_GPIO_PIN_S0D1;
            break;

        case eGpioPushPull:
            *xOutMode_NRF = NRF_GPIO_PIN_S0S1;
            break;

        default:
            bReturnCode = false;
    }

    return bReturnCode;
}

static bool prvGetDriveStrengthForHAL( int32_t lDriveStrength,
                                       nrf_gpio_pin_drive_t * lDriveStrength_NRF )
{
    bool bReturnCode = true;

    switch( lDriveStrength )
    {
        case NRF_GPIO_PIN_S0S1:
        case NRF_GPIO_PIN_H0S1:
        case NRF_GPIO_PIN_S0H1:
        case NRF_GPIO_PIN_H0H1:
        case NRF_GPIO_PIN_D0S1:
        case NRF_GPIO_PIN_D0H1:
        case NRF_GPIO_PIN_S0D1:
        case NRF_GPIO_PIN_H0D1:
            *lDriveStrength_NRF = lDriveStrength;
            break;

        default:
            bReturnCode = false;
    }

    return bReturnCode;
}

static bool prvGetPullForHAL( IotGpioPull_t xPull,
                              nrf_gpio_pin_pull_t * xPull_NRF )
{
    bool bReturnCode = true;

    switch( xPull )
    {
        case eGpioPullNone:
            *xPull_NRF = NRF_GPIO_PIN_NOPULL;
            break;

        case eGpioPullUp:
            *xPull_NRF = NRF_GPIO_PIN_PULLUP;
            break;

        case eGpioPullDown:
            *xPull_NRF = NRF_GPIO_PIN_PULLDOWN;
            break;

        default:
            bReturnCode = false;
    }

    return bReturnCode;
}

static int32_t prvGetInterruptModeForHAL( IotGpioInterrupt_t xInterrupt,
                                          nrf_drv_gpiote_in_config_t * xInterruptConfig_NRF )
{
    int32_t lReturnCode = IOT_GPIO_SUCCESS;

    switch( xInterrupt )
    {
        case eGpioInterruptNone:
            break;

        case eGpioInterruptRising:
            *xInterruptConfig_NRF = ( nrf_drv_gpiote_in_config_t ) GPIOTE_CONFIG_IN_SENSE_LOTOHI( true );
            break;

        case eGpioInterruptFalling:
            *xInterruptConfig_NRF = ( nrf_drv_gpiote_in_config_t ) GPIOTE_CONFIG_IN_SENSE_HITOLO( true );
            break;

        case eGpioInterruptEdge:
            *xInterruptConfig_NRF = ( nrf_drv_gpiote_in_config_t ) GPIOTE_CONFIG_IN_SENSE_TOGGLE( true );
            break;

        /* GPIOTE lib doesn't provide a way to do this it's edge based. We, or the user, could implement
         * similar behaviour by using eGpioInterruptEdge, entering a loop once desired hi/low value achieved,
         * and reamining in loop until another toggle.
         */
        case eGpioInterruptLow:
        case eGpioInterruptHigh:
            lReturnCode = IOT_GPIO_FUNCTION_NOT_SUPPORTED;
            IOT_GPIO_LOG( " Warning: This board does not support level-based interrupts.\r\n"
                          "        Similar behaviour can be implemented only using edge-based interrupts\r\n" );
            break;

        default:
            lReturnCode = IOT_GPIO_INVALID_VALUE;
            break;
    }

    return lReturnCode;
}

/*
 * @brief   This event handler gets installed for all gpiote
 *          then calls appropriate user callback. Pin mapping is one-directional O(1).
 *          Order of pins is typically small, so we just
 *          to a linear search to match nrf-pin --> common-io pin
 *
 * @param[in] xPin  nrf pin index that had a event triggered.
 * @param[in] xPolarity  edge type that triggered the event
 *
 */
static void prvPinEventHandler( nrfx_gpiote_pin_t xPin,
                                nrf_gpiote_polarity_t xPolarity )
{
    IotGpioHandle_t pxGpio = NULL;

    for( int i = 0; i < IOT_COMMON_IO_GPIO_NUMBER_OF_PINS; i++ )
    {
        if( plGpioMap[ pxGpioDesc[ i ].lGpioNumber ] == xPin )
        {
            pxGpio = &pxGpioDesc[ i ];
            break;
        }
    }

    if( ( pxGpio != NULL ) && ( pxGpio->xUserCallback != NULL ) && ( pxGpio->xConfig.xInterruptMode != eGpioInterruptNone ) )
    {
        pxGpio->xUserCallback( nrf_gpio_pin_read( xPin ), pxGpio->pvUserContext );
    }
}

/*
 * @brief   Configures a board pin as input, mapping CommonIO settings to board HAL settings.
 *          Assumes higher-level API verify parameters before making this call. This should only be called by
 *          prvConfigurePin, which takes care of updating pxGpio with new settings after success
 *
 * @param[in] pxGpio  Handle with settings to apply to input pin
 * @param[in] pxNewConfig  Tentative new settings for pxGpio
 *
 */
static int32_t prvConfigureInputPin( IotGpioHandle_t const pxGpio,
                                     const IotGpioConfig_t * const pxNewConfig )
{
    int32_t lReturnCode = IOT_GPIO_SUCCESS;

    /* Validate and map all CommonIO settings to NRF HAL settings */
    nrf_gpio_pin_pull_t xPull_NRF;

    if( !prvGetPullForHAL( pxNewConfig->xPull, &xPull_NRF ) )
    {
        lReturnCode = IOT_GPIO_INVALID_VALUE;
    }

    nrf_drv_gpiote_in_config_t xGpioteConfig_NRF;

    if( lReturnCode == IOT_GPIO_SUCCESS )
    {
        lReturnCode = prvGetInterruptModeForHAL( pxNewConfig->xInterruptMode, &xGpioteConfig_NRF );
    }

    if( lReturnCode == IOT_GPIO_SUCCESS )
    {
        /* Apply pin settings first. GPIOTE is set to skip alteration of drive and pull settings */
        nrf_gpio_cfg( plGpioMap[ pxGpio->lGpioNumber ],
                      NRF_GPIO_PIN_DIR_INPUT,
                      pxGpio->ucState == IOT_GPIO_CLOSED ? NRF_GPIO_PIN_INPUT_DISCONNECT : NRF_GPIO_PIN_INPUT_CONNECT,
                      xPull_NRF,
                      NRF_GPIO_PIN_S0S1,
                      NRF_GPIO_PIN_NOSENSE );

        /* Apply new gpiote settings, or leave it unset if new setting has interrupts disabled */
        if( pxGpio->xConfig.xInterruptMode != eGpioInterruptNone )
        {
            nrf_drv_gpiote_in_uninit( plGpioMap[ pxGpio->lGpioNumber ] );
        }

        if( pxNewConfig->xInterruptMode != eGpioInterruptNone )
        {
            if( !nrf_drv_gpiote_is_init() )
            {
                nrf_drv_gpiote_init();
            }

            ret_code_t xReturnCode_NRF = nrf_drv_gpiote_in_init( plGpioMap[ pxGpio->lGpioNumber ],
                                                                 &xGpioteConfig_NRF,
                                                                 prvPinEventHandler );

            /* Additional checks for logging */
            switch( xReturnCode_NRF )
            {
                case NRF_SUCCESS:
                    nrf_drv_gpiote_in_event_enable( plGpioMap[ pxGpio->lGpioNumber ], true );
                    break;

                case NRFX_ERROR_INVALID_STATE:
                    lReturnCode = IOT_GPIO_INVALID_VALUE;
                    IOT_GPIO_LOG( " Board Error: GPIOTE modules is either already initialized or in a corrupt state\r\n" );
                    break;

                case NRFX_ERROR_NO_MEM:
                    lReturnCode = IOT_GPIO_INVALID_VALUE;
                    IOT_GPIO_LOGF( " Board Error: No memory left to setup interrupt channel for nrf pin[%d]\r\n",
                                   plGpioMap[ pxGpio->lGpioNumber ] );
                    break;

                default:
                    lReturnCode = IOT_GPIO_INVALID_VALUE;
                    IOT_GPIO_LOGF( " Board Error: Unknown internal failure. nrf_drv_gpiote_in_init(...) returned %d\r\n",
                                   xReturnCode_NRF );
                    break;
            }
        }
    }

    return lReturnCode;
}

/*
 * @brief   Configures a board pin as output, mapping CommonIO settings to board HAL settings.
 *          Assumes higher-level API verify parameters before making this call.
 *
 * @note    Output pins have no regard of interrupt mode and will not trigger a callback
 *
 * @param[in] pxGpio  Handle with settings to apply to output pin
 * @param[in] pxNewConfig  Tentative new settings for pxGpio
 *
 */
static int32_t prvConfigureOutputPin( IotGpioHandle_t const pxGpio,
                                      const IotGpioConfig_t * const pxNewConfig )
{
    int32_t lReturnCode = IOT_GPIO_INVALID_VALUE;

    /* Validate CommonIO settings and map to corresponding HAL settings */
    nrf_gpio_pin_pull_t xPull_NRF;
    bool bValidInputs = prvGetPullForHAL( pxNewConfig->xPull, &xPull_NRF );

    nrf_gpio_pin_drive_t xDrive_NRF;

    if( pxNewConfig->xOutMode == eGpioOpenDrain )
    {
        xDrive_NRF = NRF_GPIO_PIN_S0D1;
    }
    else
    {
        bValidInputs &= prvGetDriveStrengthForHAL( pxNewConfig->lDriveStrength, &xDrive_NRF );
    }

    /* Only if inputs are valid and were mapped do we then configure the pin */
    if( bValidInputs )
    {
        nrf_gpio_cfg( plGpioMap[ pxGpio->lGpioNumber ],
                      NRF_GPIO_PIN_DIR_OUTPUT,
                      NRF_GPIO_PIN_INPUT_DISCONNECT,
                      xPull_NRF,
                      xDrive_NRF,
                      NRF_GPIO_PIN_NOSENSE );

        lReturnCode = IOT_GPIO_SUCCESS;
    }

    return lReturnCode;
}

/*
 * @brief   Configures board HAL parameters according to settings of pxGpio, then configures
 *          via board HAL.
 *
 * @param[in] pxGpio  Handle which will have settings applied to board pin
 * @param[in] pxNewConfig  Tentative new settings for pxGpio
 * @param[in] pxNewConfig  Disconnect NRF port input buffer.
 *
 */
static int32_t prvConfigurePin( IotGpioHandle_t const pxGpio,
                                const IotGpioConfig_t * const pxNewConfig )
{
    int32_t lReturnCode = IOT_GPIO_INVALID_VALUE;

    switch( pxNewConfig->xDirection )
    {
        case eGpioDirectionInput:
            lReturnCode = prvConfigureInputPin( pxGpio, pxNewConfig );
            break;

        case eGpioDirectionOutput:
            lReturnCode = prvConfigureOutputPin( pxGpio, pxNewConfig );
            break;

        default:
            break;
    }

    if( lReturnCode == IOT_GPIO_SUCCESS )
    {
        /* Update was successful, update descriptor to reflect new settings */
        pxGpio->xConfig = *pxNewConfig;
    }

    return lReturnCode;
}

/*---------------------------------------------------------------------------------*
*                               API Implementation                                *
*---------------------------------------------------------------------------------*/
IotGpioHandle_t iot_gpio_open( int32_t lGpioNumber )
{
    IotGpioHandle_t xReturnHandle = NULL;

    if( prvIsValidPinIndex( lGpioNumber ) )
    {
        IotGpioHandle_t pxGpio = &pxGpioDesc[ lGpioNumber ];

        if( pxGpio->ucState == IOT_GPIO_CLOSED )
        {
            /* Initialize since descriptor VLA starts as unitialized */
            *pxGpio = xDefaultGpioDesc;
            pxGpio->lGpioNumber = lGpioNumber;

            if( IOT_GPIO_SUCCESS == prvConfigurePin( pxGpio, &xDefaultGpioDesc.xConfig ) )
            {
                /* Claim descriptor */
                pxGpio->ucState = IOT_GPIO_OPENED;
                xReturnHandle = pxGpio;
            }
        }
        else
        {
            IOT_GPIO_LOGF( " Cannot open. GPIO[%d] is already opened\r\n", lGpioNumber );
        }
    }
    else
    {
        IOT_GPIO_LOGF( " Incorrect pin index[%d]. Please verify IOT_COMMON_IO_GPIO_PIN_MAP\r\n", lGpioNumber );
    }

    return xReturnHandle;
}

void iot_gpio_set_callback( IotGpioHandle_t const pxGpio,
                            IotGpioCallback_t xGpioCallback,
                            void * pvUserContext )
{
    if( prvIsValidHandle( pxGpio ) && ( xGpioCallback != NULL ) )
    {
        pxGpio->xUserCallback = xGpioCallback;
        pxGpio->pvUserContext = pvUserContext;
    }
}


int32_t iot_gpio_read_sync( IotGpioHandle_t const pxGpio,
                            uint8_t * pucPinState )
{
    int32_t lReturnCode = IOT_GPIO_SUCCESS;

    if( prvIsValidHandle( pxGpio ) && ( pxGpio->xConfig.xDirection == eGpioDirectionInput ) )
    {
        *pucPinState = nrf_gpio_pin_read( plGpioMap[ pxGpio->lGpioNumber ] );
    }
    else
    {
        lReturnCode = IOT_GPIO_INVALID_VALUE;
    }

    return lReturnCode;
}


int32_t iot_gpio_write_sync( IotGpioHandle_t const pxGpio,
                             uint8_t ucPinState )
{
    int lReturnCode = IOT_GPIO_SUCCESS;

    if( prvIsValidHandle( pxGpio ) && ( pxGpio->xConfig.xDirection == eGpioDirectionOutput ) )
    {
        nrf_gpio_pin_write( plGpioMap[ pxGpio->lGpioNumber ], ucPinState );
    }
    else
    {
        lReturnCode = IOT_GPIO_INVALID_VALUE;
    }

    return lReturnCode;
}

int32_t iot_gpio_close( IotGpioHandle_t const pxGpio )
{
    int32_t lReturnCode = IOT_GPIO_SUCCESS;

    if( prvIsValidHandle( pxGpio ) )
    {
        /* Restore back to default settings. Configuring a CLOSED pin disconnects it's NRF pin input buffer. */
        pxGpio->ucState = IOT_GPIO_CLOSED;
        lReturnCode = prvConfigurePin( pxGpio, &xDefaultGpioDesc.xConfig );

        pxGpio->lGpioNumber = xDefaultGpioDesc.lGpioNumber;
        pxGpio->xUserCallback = xDefaultGpioDesc.xUserCallback;
        pxGpio->pvUserContext = xDefaultGpioDesc.pvUserContext;
    }
    else
    {
        lReturnCode = IOT_GPIO_INVALID_VALUE;
    }

    return lReturnCode;
}

int32_t iot_gpio_ioctl( IotGpioHandle_t const pxGpio,
                        IotGpioIoctlRequest_t xRequest,
                        void * const pvBuffer )
{
    int32_t lReturnCode = IOT_GPIO_INVALID_VALUE;

    if( prvIsValidHandle( pxGpio ) )
    {
        IotGpioConfig_t xNewConfig = pxGpio->xConfig;
        lReturnCode = IOT_GPIO_SUCCESS;

        switch( xRequest )
        {
            case eSetGpioDirection:
                memcpy( &xNewConfig.xDirection, pvBuffer, sizeof( xNewConfig.xDirection ) );
                lReturnCode = prvConfigurePin( pxGpio, &xNewConfig );
                break;

            case eGetGpioDirection:
                memcpy( pvBuffer, &pxGpio->xConfig.xDirection, sizeof( pxGpio->xConfig.xDirection ) );
                break;

            case eSetGpioPull:
                memcpy( &xNewConfig.xPull, pvBuffer, sizeof( xNewConfig.xPull ) );
                lReturnCode = prvConfigurePin( pxGpio, &xNewConfig );
                break;

            case eGetGpioPull:
                memcpy( pvBuffer, &pxGpio->xConfig.xPull, sizeof( pxGpio->xConfig.xPull ) );
                break;

            case eSetGpioOutputMode:
                memcpy( &xNewConfig.xOutMode, pvBuffer, sizeof( xNewConfig.xOutMode ) );
                lReturnCode = prvConfigurePin( pxGpio, &xNewConfig );
                break;

            case eGetGpioOutputType:
                memcpy( pvBuffer, &pxGpio->xConfig.xOutMode, sizeof( pxGpio->xConfig.xOutMode ) );
                break;

            case eSetGpioInterrupt:
                memcpy( &xNewConfig.xInterruptMode, pvBuffer, sizeof( xNewConfig.xInterruptMode ) );
                lReturnCode = prvConfigurePin( pxGpio, &xNewConfig );
                break;

            case eGetGpioInterrupt:
                memcpy( pvBuffer, &pxGpio->xConfig.xInterruptMode, sizeof( pxGpio->xConfig.xInterruptMode ) );
                break;

            case eSetGpioDriveStrength:
                memcpy( &xNewConfig.lDriveStrength, pvBuffer, sizeof( xNewConfig.lDriveStrength ) );
                lReturnCode = prvConfigurePin( pxGpio, &xNewConfig );
                break;

            case eGetGpioDriveStrength:
                memcpy( pvBuffer, &pxGpio->xConfig.lDriveStrength, sizeof( pxGpio->xConfig.lDriveStrength ) );
                break;

            /* Unsupported functions */
            case eSetGpioFunction:
            case eGetGpioFunction:
            case eSetGpioSpeed:
            case eGetGpioSpeed:
            default:
                lReturnCode = IOT_GPIO_FUNCTION_NOT_SUPPORTED;
                IOT_GPIO_LOGF( " Warning: ioctl[%d] is unsupported and was ignored\r\n", xRequest );
                break;
        }
    }

    return lReturnCode;
}
