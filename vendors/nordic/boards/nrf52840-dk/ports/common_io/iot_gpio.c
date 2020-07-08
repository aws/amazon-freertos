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


#define IOT_GPIO_CLOSED              ( ( uint8_t ) 0 )
#define IOT_GPIO_OPENED              ( ( uint8_t ) 1 )

#define DEFAULT_GPIO_DESCRIPTOR \
{                               \
      -1,                       \
      eGpioDirectionInput,      \
      eGpioOpenDrain,           \
      NRF_GPIO_PIN_S0S1,        \
      eGpioPullNone,            \
      eGpioInterruptNone,       \
      NULL,                     \
      NULL,                     \
      IOT_GPIO_CLOSED           \
}

/**
 * @brief   GPIO descriptor type defined in the source file.
 */
typedef struct IotGpioDescriptor
{
    int32_t             lGpioNumber;
    IotGpioDirection_t  xDirection;
    IotGpioOutputMode_t xOutMode;
    int32_t             lDriveStrength;
    IotGpioPull_t       xPull;
    IotGpioInterrupt_t  xInterruptMode;
    IotGpioCallback_t   xUserCallback;
    void *              pvUserContext;
    uint8_t             ucState;
} IotGpioDescriptor_t;

/*
 * A 1-to-1 mapping set by iot_gpio_config.h. CommonIO Gpio pin index space uses the index i
 * and the board specific identifier is stored at GpioMap[i]. The mapping is user configured.
 */
static const int32_t plGpioMap[] = IOT_COMMON_IO_GPIO_PIN_MAP;

static IotGpioDescriptor_t pxGpioDesc[ IOT_COMMON_IO_GPIO_NUMBER_OF_PINS ];


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
bool prvIsValidPinIndex( int32_t lGpioNumber )
{
    int32_t ulNumberOfMappings = sizeof(plGpioMap) == 0 ? 0 : sizeof(plGpioMap) / sizeof(plGpioMap[0]);
           
    return  ( ulNumberOfMappings  > 0 ) &&
            ( ulNumberOfMappings == IOT_COMMON_IO_GPIO_NUMBER_OF_PINS ) &&
            ( 0 <= lGpioNumber && lGpioNumber < ulNumberOfMappings);
}



 /*
 * @brief   Used as a first-order check by all API functions that take a IotGpioHandle_t
 * 
 * @param[in] pxGpio  IotGpioHandle_t to validate
 *
 * @return    true if handle is available for API interface
 * 
 */
bool prvIsValidHandle( IotGpioHandle_t const pxGpio )
{
    return pxGpio != NULL && pxGpio->ucState == IOT_GPIO_OPENED;
}

 /*
 * @brief   Maps CommonIO Drive mode settin to correspnding HAL setting.
 * 
 * @param[in] xOutMode      CommonIO setting to map to HAL setting
 * @param{in] xOutMode_NRF  Address to store board HAL specific value hat corresponds with xOutMode
 *
 * @return    true if the xOutMode is within range of valid HAL settings and could map
 * 
 */
bool prvGetOutModeForHAL( IotGpioOutputMode_t xOutMode, nrf_gpio_pin_drive_t *xOutMode_NRF)
{
    switch(xOutMode)
    {
        case eGpioOpenDrain:
            *xOutMode_NRF = NRF_GPIO_PIN_S0D1;
            break;

        case eGpioPushPull:
            *xOutMode_NRF = NRF_GPIO_PIN_S0S1;
            break;

        default:
            return false;
    }

    return true;
}
 
bool prvGetDriveStrengthForHAL( int32_t lDriveStrength, nrf_gpio_pin_drive_t *lDriveStrength_NRF)
{
    switch(lDriveStrength)
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
            return false;
    }

    return true;
}

bool prvGetPullForHAL( IotGpioPull_t xPull, nrf_gpio_pin_pull_t *xPull_NRF)
{
    switch(xPull)
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
            return false;
    }

    return true;
}

int32_t prvGetInterruptModeForHAL( IotGpioInterrupt_t xInterrupt, nrf_drv_gpiote_in_config_t *xInterruptConfig_NRF )
{
    int32_t lReturnCode = IOT_GPIO_SUCCESS;
    switch(xInterrupt)
    {
        case eGpioInterruptRising:
            *xInterruptConfig_NRF = (nrf_drv_gpiote_in_config_t)GPIOTE_CONFIG_IN_SENSE_LOTOHI(true);
            break;

        case eGpioInterruptFalling:
            *xInterruptConfig_NRF = (nrf_drv_gpiote_in_config_t)GPIOTE_CONFIG_IN_SENSE_HITOLO(true);
            break;

        case eGpioInterruptEdge:
            *xInterruptConfig_NRF = (nrf_drv_gpiote_in_config_t)GPIOTE_CONFIG_IN_SENSE_TOGGLE(true);
            break;
      
        /* GPIOTE lib doesn't provide a way to do this it's edge based. We, or the user, could implement 
         * similar behaviour by using eGpioInterruptEdge, entering a loop once desired hi/low value achieved,
         * and reamining in loop until another toggle.
         */
        case eGpioInterruptLow:
        case eGpioInterruptHigh:
            lReturnCode = IOT_GPIO_FUNCTION_NOT_SUPPORTED;
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
void prvPinEventHandler( nrfx_gpiote_pin_t xPin, nrf_gpiote_polarity_t xPolarity )
{
    IotGpioHandle_t pxGpio = NULL;
    for (int i=0; i<IOT_COMMON_IO_GPIO_NUMBER_OF_PINS; i++)
    {
        if (plGpioMap[pxGpioDesc[i].lGpioNumber] == xPin)
        {
            pxGpio = &pxGpioDesc[i];
            break;
        }
    }

    /* If we are to ever add warnings, it would make sense to log a warnign if we reach this code while
     * while pxGpio->xInterruptMode == eGpioInterruptNone
     */
    if (pxGpio == NULL || pxGpio->xUserCallback == NULL || pxGpio->xInterruptMode == eGpioInterruptNone)
    {
        return;
    }
    else
    {
        pxGpio->xUserCallback(nrf_gpio_pin_read(xPin), pxGpio->pvUserContext);
    }

}

 /*
 * @brief   Configures a board pin as input, mapping CommonIO settings to board HAL settings.
 *          Assumes higher-level API verify parameters before making this call.
 * 
 * @param[in] pxGpio  Handle with settings to apply to input pin
 * 
 */
int32_t prvConfigureInputPin( IotGpioHandle_t const pxGpio )
{
    /* Validate and map all CommonIO settings to NRF HAL settings */
    nrf_gpio_pin_pull_t  xPull_NRF;
    nrf_gpio_pin_drive_t xDrive_NRF;
    if (!prvGetPullForHAL(pxGpio->xPull, &xPull_NRF) ||
        !prvGetDriveStrengthForHAL(pxGpio->lDriveStrength, &xDrive_NRF))
    {
        return IOT_GPIO_INVALID_VALUE;
    }

    /* Apply pin settings first. GPIOTE is set to skip alteration of drive and pull settings */
    nrf_gpio_cfg_input(plGpioMap[pxGpio->lGpioNumber], xPull_NRF);

    /* Gpiote uses separate lib and requires additional settings */ 
    if (pxGpio->xInterruptMode != eGpioInterruptNone )
    {
        nrf_drv_gpiote_in_config_t xGpioteConfig_NRF;
        
        int32_t lInterruptConfigStatus = prvGetInterruptModeForHAL(pxGpio->xInterruptMode, &xGpioteConfig_NRF);
        if (lInterruptConfigStatus == IOT_GPIO_SUCCESS)
        {
            /* Initialize gpiote modules if necessary */
            if (!nrf_drv_gpiote_is_init())
            {
                nrf_drv_gpiote_init();
            }

            if( NRF_SUCCESS == nrf_drv_gpiote_in_init(plGpioMap[pxGpio->lGpioNumber],
                                                      &xGpioteConfig_NRF,
                                                      prvPinEventHandler))
            {
                return IOT_GPIO_SUCCESS;
            } 
        } 
        else
        {
            /* There are some interrupt settings that we don't currently support */
            return lInterruptConfigStatus;
        }
    }

    return IOT_GPIO_INVALID_VALUE;
}

 /*
 * @brief   Configures a board pin as output, mapping CommonIO settings to board HAL settings.
 *          Assumes higher-level API verify parameters before making this call.
 * 
 * @note    Output pins have no regard of interrupt mode and will not trigger a callback 
 * 
 * @param[in] pxGpio  Handle with settings to apply to output pin
 * 
 */
int32_t prvConfigureOutputPin( IotGpioHandle_t const pxGpio )
{
    int32_t lReturnCode = IOT_GPIO_INVALID_VALUE;

    /* Validate CommonIO settings and map to corresponding HAL settings */
    nrf_gpio_pin_pull_t  xPull_NRF;
    bool bValid = prvGetPullForHAL(pxGpio->xPull, &xPull_NRF);
    
    nrf_gpio_pin_drive_t xDrive_NRF;
    if (pxGpio->xOutMode == eGpioOpenDrain)
    {
        xDrive_NRF = NRF_GPIO_PIN_S0D1;
    }
    else
    {
        bValid &= prvGetDriveStrengthForHAL( pxGpio->lDriveStrength, &xDrive_NRF);
    }

    /* Only if inputs are valid and were mapped do we then configure the pin */
    if (bValid)
    {
        nrf_gpio_cfg(plGpioMap[pxGpio->lGpioNumber],
                     NRF_GPIO_PIN_DIR_OUTPUT,
                     NRF_GPIO_PIN_INPUT_DISCONNECT,
                     xPull_NRF,
                     xDrive_NRF,
                     NRF_GPIO_PIN_NOSENSE);

        lReturnCode = IOT_GPIO_SUCCESS;
    }

    return lReturnCode;
}

 /*
 * @brief   Configures board HAL parameters according to settings of pxGpio, then configures
 *          via board HAL.
 * 
 * @param[in] pxGpio  Handle which will have settings applied to board pin
 * 
 */
int32_t prvConfigurePin( IotGpioHandle_t const pxGpio )
{
    int32_t lReturnCode = IOT_GPIO_INVALID_VALUE;

    switch(pxGpio->xDirection)
    {
        case eGpioDirectionInput:
            lReturnCode = prvConfigureInputPin(pxGpio);
            break;

        case eGpioDirectionOutput:
            lReturnCode = prvConfigureOutputPin(pxGpio);
            break;

        default:
            lReturnCode = IOT_GPIO_INVALID_VALUE;
            break;
    }
    
    configASSERT(lReturnCode == IOT_GPIO_SUCCESS);
    return lReturnCode;
}

/*---------------------------------------------------------------------------------*
 *                               API Implementation                                *
 *---------------------------------------------------------------------------------*/

IotGpioHandle_t iot_gpio_open( int32_t lGpioNumber )
{
    if ( !prvIsValidPinIndex(lGpioNumber) ) 
    {
        return NULL;
    }

    /* Configure pin to default settings. */
    IotGpioHandle_t pxGpio = &pxGpioDesc[lGpioNumber];
    *pxGpio = (IotGpioDescriptor_t)DEFAULT_GPIO_DESCRIPTOR;

    if (IOT_GPIO_SUCCESS == prvConfigurePin(pxGpio))
    {
        /* Claim descriptor */
        pxGpio->ucState = IOT_GPIO_OPENED;

        return pxGpio;
    } 

    return NULL;

}

void iot_gpio_set_callback( IotGpioHandle_t const pxGpio,
                            IotGpioCallback_t xGpioCallback,
                            void * pvUserContext )
{
    if ( !prvIsValidHandle(pxGpio) || xGpioCallback == NULL )
    {
        return;
    }

    pxGpio->xUserCallback = xGpioCallback;
    pxGpio->pvUserContext = pvUserContext;
}


int32_t iot_gpio_read_sync( IotGpioHandle_t const pxGpio,
                            uint8_t * pucPinState )
{
    if ( !prvIsValidHandle(pxGpio) || pxGpio->xDirection != eGpioDirectionInput )
    {
        return IOT_GPIO_INVALID_VALUE;
    }

    *pucPinState = nrf_gpio_pin_read(plGpioMap[pxGpio->lGpioNumber]);
   
    return IOT_GPIO_SUCCESS;
}


int32_t iot_gpio_write_sync( IotGpioHandle_t const pxGpio,
                             uint8_t ucPinState )
{
    if ( !prvIsValidHandle(pxGpio) || pxGpio->xDirection != eGpioDirectionOutput )
    {
        return IOT_GPIO_INVALID_VALUE;
    }

    nrf_gpio_pin_write( plGpioMap[pxGpio->lGpioNumber], ucPinState );

    return IOT_GPIO_SUCCESS;
}

int32_t iot_gpio_close( IotGpioHandle_t const pxGpio )
{
    if (!prvIsValidHandle(pxGpio))
    {
        return IOT_GPIO_INVALID_VALUE;
    }

    /* De-init, setting pin to board's default settings for unused pins, then unclaim*/
    uint32_t ulPin_NRF = plGpioMap[pxGpio->lGpioNumber];
    if (pxGpio->xDirection == eGpioDirectionInput && pxGpio->xInterruptMode != eGpioInterruptNone)
    {
        /* GPIOTE require separate deinit */
        nrf_drv_gpiote_in_uninit(ulPin_NRF);
    }
   
    nrf_gpio_cfg_default(ulPin_NRF);
    pxGpio->ucState = IOT_GPIO_OPENED;

    return IOT_GPIO_SUCCESS;
}



int32_t iot_gpio_ioctl( IotGpioHandle_t const pxGpio,
                        IotGpioIoctlRequest_t xRequest,
                        void * const pvBuffer )
{
    if (!prvIsValidHandle(pxGpio))
    {
        return IOT_GPIO_INVALID_VALUE;
    }

    /* Snapshot the existing settings, as they'll be restored in the event of a failure */
    const IotGpioDescriptor_t xCurrentSettings = *pxGpio;

    /* Adjust all the necessary CommonIO domain settings */
    int32_t lReturnCode = IOT_GPIO_SUCCESS;
    switch(xRequest)
    {
        case eSetGpioDirection:
            memcpy(&pxGpio->xDirection, pvBuffer, sizeof(pxGpio->xDirection));
            break;

        case eGetGpioDirection:
            memcpy(pvBuffer, &pxGpio->xDirection, sizeof(pxGpio->xDirection));
            break;

        case eSetGpioPull:
            memcpy(&pxGpio->xPull, pvBuffer, sizeof(pxGpio->xPull));
            break;

        case eGetGpioPull:
            memcpy(pvBuffer, &pxGpio->xPull, sizeof(pxGpio->xPull));
            break;

        case eSetGpioOutputMode:
            memcpy(&pxGpio->xOutMode, pvBuffer, sizeof(pxGpio->xOutMode));
            break;

        case eGetGpioOutputType:
            memcpy(pvBuffer, &pxGpio->xOutMode, sizeof(pxGpio->xOutMode));
            break;
          
        case eSetGpioInterrupt:
            memcpy(&pxGpio->xInterruptMode, pvBuffer, sizeof(pxGpio->xInterruptMode));
            break;

        case eGetGpioInterrupt:
            memcpy(pvBuffer, &pxGpio->xInterruptMode, sizeof(pxGpio->xInterruptMode));
            break;

        case eSetGpioDriveStrength:
            memcpy(&pxGpio->lDriveStrength, pvBuffer, sizeof(pxGpio->lDriveStrength));
            break;

        case eGetGpioDriveStrength:
            memcpy(pvBuffer, &pxGpio->lDriveStrength, sizeof(pxGpio->lDriveStrength));
            break;
        
        /* Unsupported functions */
        case eSetGpioFunction:
        case eGetGpioFunction:
        case eSetGpioSpeed:
        case eGetGpioSpeed:
        default:
            lReturnCode = IOT_GPIO_FUNCTION_NOT_SUPPORTED;
            break;
    }

    if (lReturnCode == IOT_GPIO_SUCCESS)
    {
        lReturnCode = prvConfigurePin(pxGpio);
    }

    return lReturnCode;

}
