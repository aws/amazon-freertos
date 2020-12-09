/*
 * FreeRTOS Common IO V0.1.3
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
 * @file iot_adc.h
 * @brief File for the HAL APIs of ADC called by application layer.
 */
#ifndef _IOT_ADC_H_
#define _IOT_ADC_H_

/**
 * @brief Error code returned by ADC driver
 */
#define IOT_ADC_SUCCESS                   ( 0 )     /*!< ADC operation completed successfully. */
#define IOT_ADC_FAILED                    ( 1 )     /*!< ADC operation failed. */
#define IOT_ADC_INVALID_VALUE             ( 2 )     /*!< At least one parameter is invalid. */
#define IOT_ADC_NOT_OPEN                  ( 3 )     /*!< ADC operation not possible unless ADC instance is opened. */
#define IOT_ADC_FUNCTION_NOT_SUPPORTED    ( 4 )     /*!< ADC operation not supported. */
#define IOT_ADC_CH_BUSY                   ( 5 )     /*!< ADC channel is busy at current time. */

/**
 * @brief The handle for a ADC peripheral, defined in the source file.
 * This is an anonymous struct that is vendor/driver specific.
 */
struct IotAdcDescriptor;

/**
 * @brief IotAdcHandle_t is the handle type returned by calling iot_adc_open().
 *        This is initialized in open and returned to caller.  The caller must pass
 *        this pointer to the rest of the ADC APIs.
 */
typedef struct IotAdcDescriptor * IotAdcHandle_t;

/**
 * @brief ADC notification callback type
 *
 * @param[in] pvUserContext User Context passed when setting the callback.
 *            This is not used or modified by the driver.  The context is
 *            provided by the caller when setting the callback, and is
 *            passed back to the caller in the callback.
 * @param[out] pusConvertedData pointer to converted ADC sample data.
 */
typedef void ( * IotAdcCallback_t )( uint16_t * pusConvertedData,
                                     void * pvUserContext );

/**
 * @brief Initializes ADC controller with default configuration.
 * init ADC controller, enable ADC clock, reset HW FIFO, set default
 * configuration parameters, etc. Also allocate all required resources
 * for ADC operation such as software data buffer etc.
 *
 * @warning Must be called prior to any other ADC api's so that a valid handle is obtained.
 * @warning Once opened, the same ADC instance must be closed before calling open again.
 *
 * @param[in] lAdc The instance of ADC controller to initialize.
 *
 * @return
 * - handle to the ADC controller on success
 * - NULL, if
 *    - invalid instance number
 *    - open same instance more than once before closing it.
 */
IotAdcHandle_t iot_adc_open( int32_t lAdc );

/**
 * @brief Close ADC controller.
 * All pending operation will be cancelled, put ADC module in reset state or low
 * power state if possible. Release all resources claimed during open call
 *
 * @param[in] pxAdc handle to ADC controller returned from iot_adc_open() call.
 *
 * @return
 * - IOT_ADC_SUCCESS, on success;
 * - IOT_I2C_INVALID_VALUE, if pxAdc is NULL
 * - IOT_ADC_NOT_OPEN, if is not in open state (already closed).
 */
int32_t iot_adc_close( IotAdcHandle_t const pxAdc );

/**
 * @brief Sets channel callback on availability of channel scan data.
 * On availability of ADC scan data, the application is notified with a
 * function callback. The callback function and user context for callback
 * are set using iot_adc_set_callback.
 *
 * @note This callback is per AdcChannel for each handle.
 * @note If input handle or AdcChannel is invalid, or if callback function is NULL,
 *       this function silently takes no action.
 *
 * @param[in] pxAdc The ADC handle returned in the open() call.
 * @param[in] ucAdcChannel The ADC channel for which the callback is set
 * @param[in] xAdcCallback The callback function to be called on availability of ADC channel data.
 * @param[in] pvUserContext The user context to be passed when callback is called.
 *
 * <b>Example Callback Function</b>
 * For asychronous ADC calls, a callback function is used to signal when the async
 * task is complete.  This example uses a Semaphore to signal the completion.
 * @code{c}
 * static void prvAdcChCallback( uint16_t * pusConvertedData,
 *                               void * pvUserContext )
 * {
 *     BaseType_t xHigherPriorityTaskWoken;
 *     xSemaphoreGiveFromISR( xIotAdcSemaphore, &xHigherPriorityTaskWoken );
 * }
 * @endcode
 */
void iot_adc_set_callback( IotAdcHandle_t const pxAdc,
                           uint8_t ucAdcChannel,
                           IotAdcCallback_t xAdcCallback,
                           void * pvUserContext );

/**
 * @brief Start data acquisition for ADC channel until iot_adc_stop API is called.
 *        Data will be passed back to client using callback function.
 *        By default each callback will pass back one data sample, however if client has
 *        used ioctl to pass in data buffer, only when buffer is full will callback
 *        be triggered.
 *
 * @warning iot_adc_set_callback() must be called prior to this in order to get notification
 *          when ADC scan is complete and data is available.
 *
 * @note iot_adc_set_callback() must be called prior to iot_adc_start().
 *
 * @param[in] pxAdc. The ADC handle returned in the open() call
 * @param[in] ucAdcChannel. The ADC channel to start data acquisition
 *
 * @return
 * - IOT_ADC_SUCCESS on success
 * - IOT_ADC_INVALID_VALUE, on NULL handle or invalid AdcChannel
 * - IOT_ADC_NOT_OPEN if ADC has not been opened yet.
 * - IOT_ADC_CH_BUSY if ADC operation has started but is not complete
 * - IOT_ADC_FAILED if not callback function has been set.
 * <b>Example Asynchronous read</b>
 * @code{c}
 * // ADC Instance to open
 * int32_t lAdcInstance = 0;
 *
 * // Return value of ADC functions
 * int32_t lRetVal;
 *
 * // ADC Channel to read
 * int32_t lAdcChannel = 0;
 *
 * // User/Driver context; if necessary
 * void xUserContext = NULL;
 *
 * // Open the ADC instance and get a handle.
 * xAdcHandle = iot_adc_open( lAdcInstance );
 * // assert(lRetVal == IOT_ADC_SUCCESS);
 *
 * // set the callback function
 * iot_adc_set_callback( xAdcHandle, lAdcChannel, prvAdcChCallback, &xUserCntx );
 *
 * // start channel data scan on channel
 * lRetVal = iot_adc_start( xAdcHandle, lAdcChannel );
 *  // assert( IOT_ADC_SUCCESS == lRetVal );
 *
 *  // wait for the ADC operation to complete
 *  lRetVal = xSemaphoreTake( xIotAdcSemaphore, lIotAdcChWaitTime );
 *  // assert ( pdTRUE == lRetVal );
 *
 *  // stop channel data scan
 *  lRetVal = iot_adc_stop( xAdcHandle, lAdcChannel );
 *  assert( IOT_ADC_SUCCESS == lRetVal );
 *
 *  // close ADC module
 *  lRetVal = iot_adc_close( xAdcHandle );
 *  // assert( IOT_ADC_SUCCESS == lRetVal );
 * @endcode
 */
int32_t iot_adc_start( IotAdcHandle_t const pxAdc,
                       uint8_t ucAdcChannel );

/**
 * @brief Stop data acquisition for ADC channel
 *
 * @param[in] pxAdc. The ADC handle returned in the open() call
 * @param[in] ucAdcChannel. The ADC channel to stop data acquisition
 *
 * @return
 * - IOT_ADC_SCUCCESS on success
 * - IOT_ADC_INVALID_VALUE, on NULL handle or invalid AdcChannel
 * - IOT_ADC_NOT_OPEN if ADC has been closed without re-opening.
 */
int32_t iot_adc_stop( IotAdcHandle_t const pxAdc,
                      uint8_t ucAdcChannel );

/**
 * @brief read one ADC data sample. This API will return one ADC sample.
 *
 * @param[in] pxAdc. The ADC handle returned in the open() call.
 * @param[in] ucAdcChannel. The ADC channel to read data from.
 * @param[out] pusAdcSample. ADC channel read sample value.
 *
 * @return
 * - IOT_ADC_SCUCCESS on success.
 * - IOT_ADC_INVALID_VALUE, on NULL handle or invalid AdcChannel.
 * - IOT_ADC_CH_BUSY if ADC operation not complete.
 * - IOT_ADC_NOT_OPEN if ADC has been closed without re-opening.
 * <b>Example Synchronous read</b>
 * @code{c}
 * // ADC Instance to open
 * int32_t lAdcInstance = 0;
 *
 * // ADC Channel to read
 * int32_t lAdcChannel = 0;
 *
 * // Declare and ADC handle
 * IotAdcHandle_t xAdcHandle;
 *
 * // Return value of ADC functions
 * int32_t lRetVal;
 *
 * // ADC value read
 * uint16_t usSample;
 *
 * // Open the ADC instance and get a handle.
 * xAdcHandle = iot_adc_open( lAdcInstance );
 * // assert(lRetVal == IOT_ADC_SUCCESS);
 *
 * // Read sample from ADC channel
 * lRetVal = iot_adc_read_sample( xAdcHandle, lAdcChannel, &usSample);
 * // assert(lRetVal == IOT_ADC_SUCCESS);
 *
 * // Close the ADC instance and get a handle.
 * lRetVal = iot_adc_close( xAdcHandle );
 * // assert(lRetVal == IOT_ADC_SUCCESS);
 * @endcode
 *
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
    uint32_t ulAdcSampleTime; /*!< Sample time in ADC clock cycles, apply to all channels.
                               *!< Shortest sample time maximize conversion speed for lower impedance input
                               *!< Extending sample time improve sample accuracy for higher impedance input.
                               *!< Sample time is reported as the closest possible value, rounded up, the hardware can support. */
    uint8_t ucAdcResolution;  /*!< ADC channel resolution, reported as the closest possible value, rounded up.
                               *!< value = resolution bits; 12 = 12bit, 13 = 13bit, etc */
} IotAdcConfig_t;

typedef enum
{
    eChStateIdle, /*!< ADC channel is idle. */
    eChStateBusy, /*!< ADC channel is busy. */
} IotAdcChState_t;

/**
 * @brief data structure for ioctl GetChStatus
 */
typedef struct IotAdcChStatus_s
{
    uint8_t ucAdcChannel;        /*!< ADC Channel number */
    IotAdcChState_t xAdcChState; /*!< ADC Channel State */
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
    uint8_t ucAdcChannel; /*!< Adc Channel number */
    void * pvBuffer;      /*!< Buffer to store adc results in */
    uint8_t ucBufLen;     /*!< Max buffer length to write into pvBuffer */
} IotAdcChBuffer_t;

/**
 * @brief Some ADC host controller supports grouping multiple ADC channels into a chain.
 *        When the chain is triggered to sample ADC data, all ADC channels in the group
 *        are sampled in sequence so that client doesn't need to trigger each channel
 *        individually. Converted ADC samples from such chain group can be passed back
 *        to the client with a single callback.
 *        This data structure is used for ioctl to define ADC chain setting.
 */
typedef struct IoTAdcChain_s
{
    uint8_t ucAdcChannel; /*!< logical ADC channel number as input to ioctl. It is recommended
                           *!< that client uses the first ADC channel number in the group as its
                           *!< logical number which client can use later to trigger group sampling */
    void * pvBuffer;      /*!< data buffer used by driver to save converted sample data.
                           *!< The buffer is allocated by client and passed to driver to use. */
    uint8_t ucBufLen;     /*!< Data buffer length, shall be large enough to hold group sample data. */
    uint16_t usChainMask; /*!< Define which ADC channels are in the chain group.
                           *!< e.g. 'x' bit set to 1 means ADC channel x is included in the group.
                           *!< Client shall manage potential ADC channel use conflict. */
} IotAdcChain_t;

/**
 * @brief adc ioctl request types.
 */
typedef enum IotAdcIoctlRequest_s
{
    eSetAdcConfig, /*!< Set the ADC Configuration.  Takes IotAdcConfig_t parameter. */
    eGetAdcConfig, /*!< Get the ADC Configuration.  Returns results in IotAdcConfig_t parameter. */
    eGetChStatus,  /*!< Get the Channel Status.  Returns results in IotAdcChStatus_t parameter. */
    eSetChBuffer,  /*!< Set the buffer for ADC values to be stored in.  Takes IotAdcChBuffer_t parameter. */
    eSetAdcChain,  /*!< ADC Chain for multiple ADC channels.  Takes IotAdcChain_t parameter. */
} IotAdcIoctlRequest_t;

/**
 * @brief Used for various ADC control functions.
 *
 * @param[in] pxAdc The ADC handle returned in the open() call.
 * @param[in] xRequest ioctl request defined by IotAdcIoctlRequest_s enums.
 * @param[in/out] pvBuffer data buffer for ioctl request.
 * @param[in] pvBuffer size.
 *
 * @return
 * - IOT_ADC_SCUCCESS on success
 * - IOT_ADC_INVALID_VALUE, on NULL handle or invalid request or NULL or invalid buffer pointer
 * - IOT_ADC_CH_BUSY if ADC operation not complete.
 * - IOT_ADC_NOT_OPEN if ADC has been closed without re-opening.
 * - IOT_ADC_FAILED if invalid ADC chain is requested.
 * - IOT_ADC_FUNCTION_NOT_SUPPORTED only valid for eSetAdcChain, if feature is not supported.
 */
int32_t iot_adc_ioctl( IotAdcHandle_t const pxAdc,
                       IotAdcIoctlRequest_t xRequest,
                       void * const pvBuffer );

#endif /* _IOT_ADC_H_ */
