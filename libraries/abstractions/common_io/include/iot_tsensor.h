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
 * @file    iot_tsensor.h
 * @brief   File for the APIs of TempSensor called by application layer.
 */

#ifndef _IOT_TSENSOR_H_
#define _IOT_TSENSOR_H_

/**
 * @defgroup iot_tsensor TempSensor Abstraction APIs.
 * @{
 */

#include <stdint.h>

/**
 * @brief Return values used by tsensor driver
 */
#define IOT_TSENSOR_SUCCESS            ( 0 ) /*!< TempSensor operation completed successfully.*/
#define IOT_TSENSOR_INVALID_VALUE      ( 1 ) /*!< At least one parameter is invalid.*/
#define IOT_TSENSOR_DISABLED           ( 2 ) /*!< TempSensor is disabled.*/
#define IOT_TSENSOR_CLOSED             ( 3 ) /*!< TempSensor instance is not open.*/
#define IOT_TSENSOR_GET_TEMP_FAILED    ( 4 ) /*!< TempSensor failed to get the temperature.*/
#define IOT_TSENSOR_SET_FAILED         ( 5 ) /*!< TempSensor set threshold operation failed.*/
#define IOT_TSENSOR_NOT_SUPPORTED      ( 6 ) /*!< TempSensor operation not supported.*/

/**
 * @brief tsensor threshold reached status
 */
typedef enum
{
    eTsensorMinThresholdReached, /*!< min temperature threshold reached status */
    eTsensorMaxThresholdReached  /*!< max temperature threshold reached status */
} IotTsensorStatus_t;

/**
 * @brief Ioctl request types.
 */
typedef enum
{
    eTsensorSetMinThreshold,   /*!< Set min temperature threshold using IotI2CIoctlConfig_t.  Takes int32_t value in degrees celcius.*/
    eTsensorSetMaxThreshold,   /*!< Set max temperature threshold using IotI2CIoctlConfig_t . Takes int32_t value in degrees celcius.*/
    eTsensorGetMinThreshold,   /*!< Get min temperature threshold using IotI2CIoctlConfig_t.  Returns int32_t value in degrees celcius.*/
    eTsensorGetMaxThreshold,   /*!< Get max temperature threshold using IotI2CIoctlConfig_t.  Returns int32_t value in degrees celcius.*/
    eTsensorPerformCalibration /*!< Perform calibration of the sensor */
} IotTsensorIoctlRequest_t;

/**
 * @brief   tsensor descriptor type defined in the source file.
 */
struct IotTsensorDescriptor;

/**
 * @brief   IotTsensorHandle_t type is the tsensor handle returned by calling iot_tsensor_open()
 *          this is initialized in open and returned to caller. Caller must pass this pointer
 *          to the rest of the APIs.
 */
typedef struct IotTsensorDescriptor * IotTsensorHandle_t;

/**
 * @brief The callback function for completion of Tsensor operation.
 *
 * @param[in]   xStatus         tsensor threshold reached status
 * @param[in]   pvUserContext   user provid context
 */
typedef void ( * IotTsensorCallback_t)( IotTsensorStatus_t xStatus,
                                        void * pvUserContext );

/**
 * @brief   iot_tsensor_open is used to initialize the temperature sensor.
 *          It sets up the clocks  and power etc, if the sensor is internal
 *          and sets up the communication channel if the sensor is external.
 *
 * @param[in]   lTsensorInstance   The instance of the tsensor to initialize. The instance
 *                                 number is platform specific. i,e if you have more than one temperature
 *                                 ensors, then 2 instances point to 2 different sensors.
 *
 * @return
 *   - Handle to tsensor interface on success.
 *   - NULL if
 *      - lTesnsorInstance is invalid.
 *      - instance is already open.
 */
IotTsensorHandle_t iot_tsensor_open( int32_t lTsensorInstance );

/**
 * @brief   Set the callback to be called when a threshold is reached on the sensor.
 *          The caller must set the threshold level using IOCTL before the callback can be called.
 *
 * @note Single callback is used per instance for both min and max threshold points being reached.
 * @note Newly set callback overrides the one previously set
 *
 * @warning If input handle or if callback function is NULL, this function silently takes no action.
 *
 * @warning If threshold detection is not supported by the hardware, then the callback function will not work.
 *
 * @param[in]   xTsensorHandle  Handle to tsensor driver returned in open() call
 * @param[in]   xCallback       The callback function to be called on completion of transaction.
 * @param[in]   pvUserContext   user provid context
 */
void iot_tsensor_set_callback( IotTsensorHandle_t const xTsensorHandle,
                               IotTsensorCallback_t xCallback,
                               void * pvUserContext );

/**
 * @brief   iot_tsensor_enable is used to enable the temperature sensor
 *          to start reading the temperature and trigger thresholds (if any were set and supported)
 *
 * @param[in]   xTsensorHandle  Handle to tsensor driver returned in
 *                              iot_tsensor_open
 *
 * @return
 *   - IOT_TSENSOR_SUCCESS on success
 *   - IOT_TSENSOR_INVALID_VALUE if xTsensorHandle is NULL.
 */
int32_t iot_tsensor_enable( IotTsensorHandle_t const xTsensorHandle );

/**
 * @brief   iot_tsensor_disable is used to disable the temperature sensor
 *          which stops monitoring the temperature.
 *
 * @param[in]   xTsensorHandle  Handle to tsensor driver returned in
 *                              iot_tsensor_open
 *
 * @return
 *   - IOT_TSENSOR_SUCCESS on success
 *   - IOT_TSENSOR_INVALID_VALUE if xTsensorHandle is NULL.
 */
int32_t iot_tsensor_disable( IotTsensorHandle_t const xTsensorHandle );

/**
 * @brief   iot_tsensor_get_temp is used to get the current temperature
 *          read from the sensor.
 *
 * @param[in]   xTsensorHandle  handle to tsensor driver returned in
 *                              iot_tsensor_open
 * @param[out]  plTemp           temperature read from the sensor.
 *
 * @return
 *   - IOT_TSENSOR_SUCCESS on success
 *   - IOT_TSENSOR_INVALID_VALUE if
 *      - xTsensorHandle is NULL
 *      - lTemp is NULL
 *   - IOT_TSENSOR_DISABLED if tsensor instance has been disabled with call to iot_tsensor_disable().
 *   - IOT_TSENSOR_GET_TEMP_FAILED if error occured reading the temperature.
 */
int32_t iot_tsensor_get_temp( IotTsensorHandle_t const xTsensorHandle,
                              int32_t * plTemp );

/**
 * @brief   iot_tsensor_ioctl is used to set tsensor configuration
 *          and tsensor properties like minimum threshold, maximum threshold value, etc.
 *          Supported IOCTL requests are defined in aws_hal_Tsensor_Ioctl_Request_t
 *
 * @param[in]       xTsensorHandle  handle to tsensor driver returned in
 *                              iot_tsensor_open
 * @param[in]       xRequest    configuration request.
 * @param[in,out]   pvBuffer    buffer holding tsensor set and get values.
 *
 * @return
 *   - IOT_TSENSOR_SUCCESS on success
 *   - IOT_TSENSOR_INVALID_VALUE
 *      - xTsensorHandle is NULL
 *      - xRequest is invalid
 *      - pvBuffer is NULL (excluding eTsensorPerformCalibration)
 *   - IOT_TSENSOR_CLOSED if instance not in open state.
 *   - IOT_TSENSOR_DISABLED if tsensor instance has been disabled with call to iot_tsensor_disable().
 *   - IOT_TSENSOR_GET_TEMP_FAILED if error occured reading the temperature.
 *   - IOT_TSENSOR_SET_FAILED if set threshold operation failed.
 *   - IOT_TSENSOR_NOT_SUPPORTED valid if xRequest feature not supported.
 * @note:  If eTsensorSetMinThreshold or eTsensorSetMaxThreshold ioctl is supported, then the
 *         corresponding eTsensorGetMinThreshold and eTsensorGetMaxThreshold must also be supported.
 */
int32_t iot_tsensor_ioctl( IotTsensorHandle_t const xTsensorHandle,
                           IotTsensorIoctlRequest_t xRequest,
                           void * const pvBuffer );

/**
 * @brief iot_tsensor_close is used to de-initialize Tsensor.
 *
 * @param[in]   xTsensorHandle  handle to tsensor driver returned in
 *                              iot_tsensor_open
 *
 * @return
 *   - IOT_TSENSOR_SUCCESS on success
 *   - IOT_TSENSOR_INVALID_VALUE if
 *      - xTensorHandle is NULL
 *      - not in open state (already closed)
 */
int32_t iot_tsensor_close( IotTsensorHandle_t const xTsensorHandle );


/**
 * @}
 */
/* end of group iot_tsensor */

#endif /* ifndef _IOT_TSENSOR_H_ */
