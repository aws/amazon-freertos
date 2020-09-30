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
 * @file    iot_adc.c
 * @brief   This file contains the definitions of ADC APIs using TI drivers.
 */
#include <stdint.h>
#include <unistd.h>
#include <stdbool.h>

/* Common IO Header */
#include <iot_adc.h>

/* TI Drivers and DPL */
#include <ti/drivers/ADCBuf.h>
#include <ti/drivers/adcbuf/ADCBufCC26X2.h>
#include <ti/drivers/dpl/SemaphoreP.h>

/* Driver config */
#include <ti_drivers_config.h>

/**
 * @brief TI Driver ADC callback function.
 *
 * When the TI driver is in ADCBuf_RETURN_MODE_CALLBACK mode, on completion,
 * this function will be called.
 *
 * @param[in] pxAdcHandle The handle passed by the driver
 * @param[in] pxConversion ADC conversion that completed.
 * @param[in] pvBuffer ADC buffer that completed.
 * @param[in] ulChannel ADC channel that completed.
 * @param[in] usStatus ADC status.
 */
static void _adcCallback( ADCBuf_Handle pxAdcHandle, ADCBuf_Conversion * pxConversion,
                          void *pvBuffer, uint32_t ulChannel, int_fast16_t usStatus );

/**
 * @brief TI Simplelink ADC HAL context abstracted in the Peripheral_Descriptor_t.
 *
 * Contains all the parameters needed for working with TI Simplelink Timer driver.
 */
typedef struct
{
    ADCBuf_Handle tiAdcHandle;                                                      /**< TI driver handle used with TI Driver API. */
    ADCBuf_Conversion xSingleConversion;                                             /**< Single conversion structure. */
    IotAdcConfig_t xAdcConfig;                                                      /**< ADC config structure */
    IotAdcCallback_t xAdcCallback[CONFIG_TI_DRIVERS_ADCBUF_CHANNEL_COUNT];      /**< User registered callback */
    void *pvUserCallbackContext[CONFIG_TI_DRIVERS_ADCBUF_CHANNEL_COUNT];        /**< User callback context */
    void * pvChannelBuffer[CONFIG_TI_DRIVERS_ADCBUF_CHANNEL_COUNT];             /**< Channel buffers. */
    uint32_t ulSingleSampleBuffer;                                                  /**< Single sample buffer. */
    bool xIsActive;                                                                 /**< Conversion running flag. */
    bool xIsBlocking;                                                               /**< Blocking conversion running flag. */
    uint8_t ucChannelBufferLength[CONFIG_TI_DRIVERS_ADCBUF_CHANNEL_COUNT];      /**< Channel buffers length. */
    uint8_t ucActiveChannel;                                                        /**< Active channel. */

    /**
     NOTE: Using the TI DPL semaphore instead of FreeRTOS for now. Could we leave
             this as this or should we convert it to FreeRTOS API
           as we get the FreeRTOS SDK going?
     **/
    SemaphoreP_Struct xSampleSem;  /**< Semaphore used for single sample reads */
} IotAdcDescriptor_t;

/**
 * @brief Static ADC descriptor table
 */
static IotAdcDescriptor_t xAdc[CONFIG_TI_DRIVERS_ADCBUF_COUNT];

static bool xIsInitialized = false;

/*-----------------------------------------------------------*/

IotAdcHandle_t iot_adc_open(int32_t lAdc)
{
    ADCBuf_Handle xTiHandle = NULL;
    ADCBuf_Params xParams;
    IotAdcHandle_t xHandle = NULL;

    /* Initialize if needed */
    if (false == xIsInitialized)
    {
        ADCBuf_init();
        xIsInitialized = true;
    }

    ADCBuf_Params_init(&xParams);
    xParams.returnMode = ADCBuf_RETURN_MODE_CALLBACK;
    xParams.recurrenceMode = ADCBuf_RECURRENCE_MODE_ONE_SHOT;
    xParams.callbackFxn = _adcCallback;
    xTiHandle = ADCBuf_open(lAdc, &xParams);

    if (xTiHandle)
    {
        /* Initialize default values */
        xAdc[lAdc].tiAdcHandle = xTiHandle;
        xAdc[lAdc].xIsActive = false;
        /* Set TI Driver defaults */
        xAdc[lAdc].xAdcConfig.ulAdcSampleTime = ADCBufCC26X2_SAMPLING_DURATION_2P7_US;
        xAdc[lAdc].xAdcConfig.ucAdcResolution = ADCBufCC26X2_RESOLUTION;

        /* Construct semaphores */
        SemaphoreP_constructBinary(&(xAdc[lAdc].xSampleSem), 0);

        int i = 0;
        for (i = 0; i < CONFIG_TI_DRIVERS_ADCBUF_CHANNEL_COUNT; i++)
        {
            xAdc[lAdc].xAdcCallback[i] = NULL;
            xAdc[lAdc].pvUserCallbackContext[i] = NULL;
            xAdc[lAdc].pvChannelBuffer[i] = NULL;
            xAdc[lAdc].xSingleConversion.arg = &xAdc[lAdc];
        }

        xHandle = (IotAdcHandle_t) &xAdc[lAdc];
    }

    return xHandle;
}

/*-----------------------------------------------------------*/

int32_t iot_adc_close( IotAdcHandle_t const pxAdc )
{
    IotAdcDescriptor_t * pxAdcDesc = (IotAdcDescriptor_t *) pxAdc;

    if (NULL == pxAdcDesc)
    {
        return IOT_ADC_INVALID_VALUE;
    }
    else if (NULL == pxAdcDesc->tiAdcHandle)
    {
        return IOT_ADC_NOT_OPEN;
    }

    ADCBuf_close(pxAdcDesc->tiAdcHandle);
    pxAdcDesc->tiAdcHandle = NULL;

    /* Don't forget to destroy the semaphores */
    SemaphoreP_destruct(&(pxAdcDesc->xSampleSem));

    return IOT_ADC_SUCCESS;
}

/*-----------------------------------------------------------*/

void iot_adc_set_callback( IotAdcHandle_t const pxAdc,
                           uint8_t ucAdcChannel,
                           IotAdcCallback_t xAdcCallback,
                           void * pvUserContext )
{
    IotAdcDescriptor_t * pxAdcDesc = (IotAdcDescriptor_t *) pxAdc;

    if (pxAdcDesc && xAdcCallback &&
        (CONFIG_TI_DRIVERS_ADCBUF_CHANNEL_COUNT > ucAdcChannel))
    {
        pxAdcDesc->xAdcCallback[ucAdcChannel] = xAdcCallback;
        pxAdcDesc->pvUserCallbackContext[ucAdcChannel] = pvUserContext;
    }
}

/*-----------------------------------------------------------*/

int32_t iot_adc_start( IotAdcHandle_t const pxAdc,
                       uint8_t ucAdcChannel )
{
    IotAdcDescriptor_t * pxAdcDesc = (IotAdcDescriptor_t *) pxAdc;
    int_fast16_t sStatus;

    if ((NULL == pxAdcDesc) ||
        (CONFIG_TI_DRIVERS_ADCBUF_CHANNEL_COUNT <= ucAdcChannel))
    {
        return IOT_ADC_INVALID_VALUE;
    }
    else if (NULL == pxAdcDesc->tiAdcHandle)
    {
        return IOT_ADC_NOT_OPEN;
    }
    else if (NULL == pxAdcDesc->xAdcCallback[ucAdcChannel])
    {
        return IOT_ADC_FAILED;
    }
    else if (pxAdcDesc->xIsActive)
    {
        return IOT_ADC_CH_BUSY;
    }

    /* This is not a blocking operation */
    pxAdcDesc->xIsBlocking = false;

    /* Channel to use */
    pxAdcDesc->xSingleConversion.adcChannel = ucAdcChannel;

    /* Is there a buffer set for this channel? */
    if (NULL != pxAdcDesc->pvChannelBuffer[ucAdcChannel])
    {
        pxAdcDesc->xSingleConversion.sampleBuffer = pxAdcDesc->pvChannelBuffer[ucAdcChannel];
        pxAdcDesc->xSingleConversion.samplesRequestedCount = pxAdcDesc->ucChannelBufferLength[ucAdcChannel];
    }
    else
    {
        pxAdcDesc->xSingleConversion.sampleBuffer = &pxAdcDesc->ulSingleSampleBuffer;
        pxAdcDesc->xSingleConversion.samplesRequestedCount = 1;
    }

    /* Set active before convert call as we could get preempted before potential
       "done" callback. */
    pxAdcDesc->xIsActive = true;

    sStatus = ADCBuf_convert(pxAdcDesc->tiAdcHandle, &pxAdcDesc->xSingleConversion, 1);

    /* Check return status */
    if (ADCBuf_STATUS_SUCCESS != sStatus)
    {
        /* Failed due some reason */
        pxAdcDesc->xIsActive = false;
        return IOT_ADC_FAILED;
    }

    pxAdcDesc->ucActiveChannel = ucAdcChannel;

    return IOT_ADC_SUCCESS;
}

/*-----------------------------------------------------------*/

int32_t iot_adc_stop( IotAdcHandle_t const pxAdc,
                      uint8_t ucAdcChannel )
{
    IotAdcDescriptor_t * pxAdcDesc = (IotAdcDescriptor_t *) pxAdc;

    if ((NULL == pxAdcDesc) || (CONFIG_TI_DRIVERS_ADCBUF_CHANNEL_COUNT <= ucAdcChannel))
    {
        return IOT_ADC_INVALID_VALUE;
    }
    else if (NULL == pxAdcDesc->tiAdcHandle)
    {
        return IOT_ADC_NOT_OPEN;
    }

    /* Set inactive */
    pxAdcDesc->xIsActive = false;

    /* Cancel conversion if running */
    if (ucAdcChannel == pxAdcDesc->ucActiveChannel)
    {
        ADCBuf_convertCancel(pxAdcDesc->tiAdcHandle);
    }

    return IOT_ADC_SUCCESS;
}

/*-----------------------------------------------------------*/

int32_t iot_adc_read_sample( IotAdcHandle_t const pxAdc,
                             uint8_t ucAdcChannel,
                             uint16_t * pusAdcSample )
{
    IotAdcDescriptor_t * pxAdcDesc = (IotAdcDescriptor_t *) pxAdc;
    int_fast16_t sStatus;

    if ((NULL == pxAdcDesc) ||
        (CONFIG_TI_DRIVERS_ADCBUF_CHANNEL_COUNT <= ucAdcChannel) ||
        (NULL == pusAdcSample))
    {
        return IOT_ADC_INVALID_VALUE;
    }
    else if (NULL == pxAdcDesc->tiAdcHandle)
    {
        return IOT_ADC_NOT_OPEN;
    }
    else if (pxAdcDesc->xIsActive)
    {
        return IOT_ADC_CH_BUSY;
    }

    /* Run as blocking single channel sample */
    pxAdcDesc->xIsBlocking = true;
    pxAdcDesc->xIsActive = true;

    /* Setup conversion */
    pxAdcDesc->xSingleConversion.adcChannel = ucAdcChannel;
    pxAdcDesc->xSingleConversion.sampleBuffer = pusAdcSample;
    pxAdcDesc->xSingleConversion.samplesRequestedCount = 1;

    sStatus = ADCBuf_convert(pxAdcDesc->tiAdcHandle, &pxAdcDesc->xSingleConversion, 1);

    if (ADCBuf_STATUS_SUCCESS != sStatus)
    {
        /* Failed due some reason */
        return IOT_ADC_INVALID_VALUE;
    }

    /* Set active channel */
    pxAdcDesc->ucActiveChannel = ucAdcChannel;

    /* Wait for conversion to complete */
    SemaphoreP_pend(&(pxAdcDesc->xSampleSem), SemaphoreP_WAIT_FOREVER);

    /* Set as inactive again */
    pxAdcDesc->xIsActive = false;

    return IOT_ADC_SUCCESS;
}

/*-----------------------------------------------------------*/

int32_t iot_adc_ioctl( IotAdcHandle_t const pxAdc,
                       IotAdcIoctlRequest_t xRequest,
                       void * const pvBuffer )
{
    IotAdcDescriptor_t * pxAdcDesc = (IotAdcDescriptor_t *) pxAdc;
    int32_t ret = IOT_ADC_SUCCESS;

    if ((NULL == pxAdcDesc) || (NULL == pvBuffer))
    {
        ret = IOT_ADC_INVALID_VALUE;
        return ret;
    }
    else if (NULL == pxAdcDesc->tiAdcHandle)
    {
        ret = IOT_ADC_CH_BUSY;
        return ret;
    }

    switch (xRequest)
    {
        case eSetAdcConfig:
        {
            IotAdcConfig_t * pxConfig = (IotAdcConfig_t *) pvBuffer;

            /* Currently we don't support anything else than what the HW can handle */
            if (ADCBufCC26X2_RESOLUTION != pxConfig->ucAdcResolution)
            {
                ret = IOT_ADC_INVALID_VALUE;
                break;
            }
            else if (pxAdcDesc->xIsActive)
            {
                ret = IOT_ADC_CH_BUSY;
                break;
            }

            /* Map ulAdcSampleTime "clock cycles" to sample times.
             * The ADC clock is 6 MHz and lowest number of cycles is 16.
             * Use the GCC "Count leading zeroes" to get an idea of the size
             * but first make sure we strap to the max value.
             */
            uint32_t ulSampleTime = ADCBufCC26X2_SAMPLING_DURATION_2P7_US;
            uint32_t ulMsbBitPosition = 32 - (uint32_t) __builtin_clz(pxConfig->ulAdcSampleTime);

            if (65536 < pxConfig->ulAdcSampleTime)
            {
                ulSampleTime = ADCBufCC26X2_SAMPLING_DURATION_10P9_MS;

                /* Set the actual ADC value, not the one passed by the user */
                ulMsbBitPosition = ADCBufCC26X2_SAMPLING_DURATION_10P9_MS;
            }
            else
            {
                /* We know have a rough idea of which sampling time to set.
                 * Pin-point the closest valid value, if the MSB bit is not higher than 2 then assume
                 * the fastest possible sampling time */
                uint32_t ulRoundUpOrDown = 0;
                if (2 < ulMsbBitPosition)
                {
                    /* Calculate middle point between MSB and MSB - 1 */
                    ulRoundUpOrDown = (1 << ulMsbBitPosition) + (1 << (ulMsbBitPosition - 1));

                    if (0 > (ulRoundUpOrDown - pxConfig->ulAdcSampleTime))
                    {
                        /* Round down */
                        ulMsbBitPosition--;
                    }

                    /* Need to still be larger than 2 for it to make a difference */
                    if (2 < ulMsbBitPosition)
                    {
                        /* MSB bit position should map to sample times:
                            3h = 2P7_US : 16x 6 MHz clock periods = 2.7us
                            4h = 5P3_US : 32x 6 MHz clock periods = 5.3us
                            5h = 10P6_US : 64x 6 MHz clock periods = 10.6us
                            6h = 21P3_US : 128x 6 MHz clock periods = 21.3us
                            7h = 42P6_US : 256x 6 MHz clock periods = 42.6us
                            8h = 85P3_US : 512x 6 MHz clock periods = 85.3us
                            9h = 170_US : 1024x 6 MHz clock periods = 170us
                            Ah = 341_US : 2048x 6 MHz clock periods = 341us
                            Bh = 682_US : 4096x 6 MHz clock periods = 682us
                            Ch = 1P37_MS : 8192x 6 MHz clock periods = 1.37ms
                            Dh = 2P73_MS : 16384x 6 MHz clock periods = 2.73ms
                            Eh = 5P46_MS : 32768x 6 MHz clock periods = 5.46ms
                            Fh = 10P9_MS : 65536x 6 MHz clock periods = 10.9ms */
                        ulSampleTime = ulMsbBitPosition;
                    }
                }

                /* If still less than 3, set to 3 as this reflects the
                 * smallest possible value */
                if (3 > ulMsbBitPosition)
                {
                    ulMsbBitPosition = 3;
                }
            }

            /* Set actual sample value, not the one passed by the user */
            pxAdcDesc->xAdcConfig.ulAdcSampleTime = 1 << (ulMsbBitPosition + 1);
            pxAdcDesc->xAdcConfig.ucAdcResolution = pxConfig->ucAdcResolution;

            /* Re-open the ADCbuf driver with the custom settings. These are CC13x2/CC26x2 specific. */
            uint32_t i;
            for (i = 0; i < CONFIG_TI_DRIVERS_ADCBUF_COUNT; i++)
            {
                if (xAdc[i].tiAdcHandle == pxAdcDesc->tiAdcHandle)
                {
                    ADCBuf_close(pxAdcDesc->tiAdcHandle);

                    ADCBuf_Params xParams;
                    ADCBufCC26X2_ParamsExtension xCustomParams;
                    ADCBuf_Params_init(&xParams);
                    xParams.returnMode = ADCBuf_RETURN_MODE_CALLBACK;
                    xParams.recurrenceMode = ADCBuf_RECURRENCE_MODE_ONE_SHOT;
                    xParams.callbackFxn = _adcCallback;
                    xParams.custom = &xCustomParams;
                    xCustomParams.samplingDuration = ulSampleTime;
                    xCustomParams.samplingMode = ADCBufCC26X2_SAMPING_MODE_SYNCHRONOUS,
                    xCustomParams.refSource = ADCBufCC26X2_FIXED_REFERENCE,
                    xCustomParams.inputScalingEnabled = true,

                    pxAdcDesc->tiAdcHandle = ADCBuf_open(i, &xParams);

                    if (NULL == pxAdcDesc->tiAdcHandle)
                    {
                        /* maybe not what is expected but accurate. */
                        ret = IOT_ADC_NOT_OPEN;
                    }

                    break;
                }
            }
            break;
        }

        case eGetAdcConfig:
        {
            /* Return the local copy */
            *((IotAdcConfig_t *) pvBuffer) = pxAdcDesc->xAdcConfig;
            break;
        }

        case eGetChStatus:
        {
            IotAdcChStatus_t * pxConfig = (IotAdcChStatus_t *) pvBuffer;

            /* Sanity checks */
            if (CONFIG_TI_DRIVERS_ADCBUF_CHANNEL_COUNT <= pxConfig->ucAdcChannel)
            {
                ret = IOT_ADC_INVALID_VALUE;
                break;
            }

            /* Assume idle */
            pxConfig->xAdcChState = eChStateIdle;

            if ((pxAdcDesc->ucActiveChannel == pxConfig->ucAdcChannel) &&
                (pxAdcDesc->xIsActive))
            {
                pxConfig->xAdcChState = eChStateBusy;
            }

            break;
        }

        case eSetChBuffer:
        {
            IotAdcChBuffer_t * pxConfig = (IotAdcChBuffer_t *) pvBuffer;

            /* Sanity checks */
            if ((NULL == pxConfig->pvBuffer) || (0 == pxConfig->ucBufLen) ||
                (CONFIG_TI_DRIVERS_ADCBUF_CHANNEL_COUNT <= pxConfig->ucAdcChannel))
            {
                ret = IOT_ADC_INVALID_VALUE;
                break;
            }
            else if (pxAdcDesc->xIsActive)
            {
                ret = IOT_ADC_CH_BUSY;
                break;
            }

            /* Store buffer */
            pxAdcDesc->pvChannelBuffer[pxConfig->ucAdcChannel] = pxConfig->pvBuffer;
            pxAdcDesc->ucChannelBufferLength[pxConfig->ucAdcChannel] = pxConfig->ucBufLen;
            break;
        }

        case eSetAdcChain:
        {
            /* While we could potentially support this, the amount of work to support
             * the current API description of it is significant (and cofnusing). */
            ret = IOT_ADC_FUNCTION_NOT_SUPPORTED;
            break;
        }

        default:
        {
            ret = IOT_ADC_INVALID_VALUE;
        }
    }

    return ret;
}

/*-----------------------------------------------------------*/

static void _adcCallback( ADCBuf_Handle pxAdcHandle, ADCBuf_Conversion * pxConversion,
                          void *pvBuffer, uint32_t ulChannel, int_fast16_t usStatus )
{
    IotAdcDescriptor_t * pxAdcDesc = (IotAdcDescriptor_t *) pxConversion->arg;

    /* Conversion completed, issue callback if any */
    if (pxAdcDesc->xAdcCallback[ulChannel] && pxAdcDesc->xIsActive)
    {
        pxAdcDesc->xAdcCallback[ulChannel](pvBuffer, pxAdcDesc->pvUserCallbackContext[ulChannel]);
    }

    /* Release semaphore if needed */
    if (pxAdcDesc->xIsBlocking)
    {
        SemaphoreP_post(&(pxAdcDesc->xSampleSem));
    }
    else if (pxAdcDesc->xIsActive)
    {
        /* "Continous" reading only when a buffer is not used, restart */
        if ( pvBuffer != pxAdcDesc->pvChannelBuffer[ulChannel] )
        {
            ADCBuf_convert(pxAdcDesc->tiAdcHandle, pxConversion, 1);
        }
        else
        {
            /* Was a "user buffer", mark as inactive again */
            pxAdcDesc->xIsActive = false;
        }
    }
}
