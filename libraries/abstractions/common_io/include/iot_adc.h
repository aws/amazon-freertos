/*
 * Amazon FreeRTOS V1.2.3
 * Copyright (C) 2019 Amazon.com, Inc. or its affiliates.  All Rights Reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 *
 * http://aws.amazon.com/freertos
 * http://www.FreeRTOS.org
 */

/**
 * @file iot_adc.h
 * @brief File for the HAL APIs of ADC called by application layer.
 */
#ifndef _IOT_ADC_H_
#define _IOT_ADC_H_

/**
 * @brief Error code returned by ADC driver
 */
#define IOT_ADC_SUCCESS                   ( 0 )
#define IOT_ADC_FAILED                    ( 1 )
#define IOT_ADC_INVALID_VALUE             ( 2 )
#define IOT_ADC_NOT_OPEN                  ( 3 )
#define IOT_ADC_FUNCTION_NOT_SUPPORTED    ( 4 )
#define IOT_ADC_CH_BUSY                   ( 5 )

/**
 * @brief The handle for a ADC peripheral.
 */
struct IotAdcDescriptor;
typedef struct IotAdcDescriptor * IotAdcHandle_t;

/**
 * @brief adc notification callback type
 *
 * @param[in] pvUserContext User Context passed when setting the callback.
 * @param[out] pusConvertedData pointer to converted ADC sample data.
 */
typedef void ( * IotAdcCallback_t )( uint16_t * pusConvertedData,
                                     void * pvUserContext );

/**
 * @brief Initializes ADC controller with default configuration.
 *
 * init ADC controller, enable ADC clock, reset HW FIFO, set default
 * configuration parameters, etc. Also allocate all required resources
 * for ADC operation such as software data buffer etc.
 *
 * @param[in] lAdc The instance of ADC controller to initialize.
 *
 * @return handle to the ADC controller if everything succeeds, otherwise NULL.
 */

IotAdcHandle_t iot_adc_open( int32_t lAdc );

/**
 * @brief Close ADC controller.
 *
 * All pending operation will be cancelled, put ADC module in reset state or low
 * power state if possible. Release all resources claimed during open call
 *
 * @param[in] pxAdc handle to ADC controller
 *
 * @return 0 on success; otherwise negative error code
 */
int32_t iot_adc_close( IotAdcHandle_t const pxAdc );

/**
 * @brief Sets channel callback on availability of channel scan data.
 *
 * On availability of ADC scan data, the application is notified with a
 * function callback. The callback function and user context for callback
 * are set using iot_adc_set_callback.
 *
 * @param[in] pxAdc The Adc handle returned in the open() call.
 * @param[in] ucAdcChannel The Adc channel for which the callback is set
 * @param[in] xAdcCallback The callback function to be called on availability of ADC channel data.
 * @param[in] pvUserContext The user context to be passed when callback is called.
 */
void iot_adc_set_callback( IotAdcHandle_t const pxAdc,
                           uint8_t ucAdcChannel,
                           IotAdcCallback_t xAdcCallback,
                           void * pvUserContext );

/**
 * @brief Start data acquisition for ADC channel until iot_adc_stop API is called.
 *        data will be passed back to client using callback function.
 *        by default each callback will pass back one data sample, however if client has
 *        used ioctl to pass in data buffer, only when buffer is full will callback
 *        be triggered
 *
 * @param[in] pxAdc. The ADC handle returned in the open() call
 * @param[in] ucAdcChannel. The ADC channel to start data acquisition
 *
 * @return 0 on success; otherwise negative error code
 */
int32_t iot_adc_start( IotAdcHandle_t const pxAdc,
                       uint8_t ucAdcChannel );

/**
 * @brief Stop data acquisition for ADC channel
 *
 * @param[in] pxAdc. The ADC handle returned in the open() call
 * @param[in] ucAdcChannel. The ADC channel to stop data acquisition
 *
 * @return 0 on success; otherwise negative error code
 */
int32_t iot_adc_stop( IotAdcHandle_t const pxAdc,
                      uint8_t ucAdcChannel );

/**
 * @brief read one ADC data sample. This API will return one ADC sample.
 *
 * @param[in] pxAdc. The ADC handle returned in the open() call
 * @param[in] ucAdcChannel. The ADC channel to read data from
 * @param[out] pusAdcSample. ADC channel read sample value
 *
 * @return 0 on succeess; otherwise negative error code
 */
int32_t iot_adc_read_sample( IotAdcHandle_t const pxAdc,
                             uint8_t ucAdcChannel,
                             uint16_t * pusAdcSample );

/**
 * @brief data structures for ioctl request
 */

/**
 * @brief data structure for ioctl SetAdcConfig and GetAdcConfig
 */
typedef struct IotAdcConfig_s
{
    uint32_t ulAdcSampleTime; /* Sample time in ADC clock cycles, apply to all channels.
                               * Shortest sample time maximize conversion speed for lower impedance input
                               * Extending sample time improve sample accuracy for higher impedance input */
    uint8_t ucAdcResolution;  /* ADC channel resolution, 12 = 12bit, 13 = 13bit, etc */
} IotAdcConfig_t;

typedef enum
{
    eChStateIdle,
    eChStateBusy,
} IotAdcChState_t;

/**
 * @brief data structure for ioctl GetChStatus
 */
typedef struct IotAdcChStatus_s
{
    uint8_t ucAdcChannel;
    IotAdcChState_t xAdcChState;
} IotAdcChStatus_t;

/**
 * @brief data structure for ioctl SetChBuffer
 *        setting channel data buffer is optional using this ioctl
 *        if client doesn't pass in data buffer for driver to use, callback is triggered
 *        for every ADC sample to pass data back to client as driver doesn't have buffer
 *        to accumulate data. As soon as callback returns, xConverted_Data becomes invalid.
 *        On the other hand however if client does pass a buffer for driver to use,
 *        callback is triggered only after driver has filled buffer with xBufLen samples,
 *        client buffer is passed back in callback as XConverted_Data whose life span is
 *        controlled by the client even after callback returns.
 */
typedef struct IotAdcChBuffer_s
{
    uint8_t ucAdcChannel;
    void * pvBuffer;
    uint8_t ucBufLen;
} IotAdcChBuffer_t;

/**
 * @brief Some ADC host controller supports grouping multiple ADC channels into a chain.
 *        When the chain is triggered to sample ADC data, all ADC channels in the group
 *        are sampled in sequence so that client doesn't need to trigger each channel
 *        individually. Coverted ADC samples from such chain group can be passed back
 *        to the client with a single callback.
 *        This data structure is used for ioctl to define ADC chain setting.
 */
typedef struct IoTAdcChain_s
{
    uint8_t ucAdcChannel; /* logical ADC channel number as input to ioctl. It is recommended
                           * that client uses the first ADC channel number in the group as its
                           * logical number which client can use later to trigger group sampling */
    void * pvBuffer;      /* data buffer used by driver to save converted sample data.
                           * the buffer is allocated by client and passed to driver to use */
    uint8_t ucBufLen;     /* data buffer length, shalll be large enough to hold group sample data */
    uint16_t usChainMask; /* define which ADC channels are in the chain group.
                           * e.g. 'x' bit set to 1 means ADC channel x is inclued in the group.
                           * Client shall manage potential ADC channel use conflict. */
} IotAdcChain_t;

/**
 * @brief adc ioctl request types.
 */
typedef enum IotAdcIoctlRequest_s
{
    eSetAdcConfig,
    eGetAdcConfig,
    eGetChStatus,
    eSetChBuffer,
    eSetAdcChain,
} IotAdcIoctlRequest_t;

/**
 * @brief Used for various ADC control functions.
 *
 * @param[in] pxAdc The Adc handle returned in the open() call.
 * @param[in] xRequest ioctl request defined by IotAdcIoctlRequest_s enums
 * @param[in/out] pvBuffer data buffer for ioctl request
 * @param[in] pvBuffer size
 *
 * @return 0 on success; otherwise negative error code
 */
int32_t iot_adc_ioctl( IotAdcHandle_t const pxAdc,
                       IotAdcIoctlRequest_t xRequest,
                       void * const pvBuffer );

#endif /* _IOT_ADC_H_ */
