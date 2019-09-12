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
#define IOT_TSENSOR_SUCCESS         ( 0 )
#define IOT_TSENSOR_INVALID_VALUE   ( 1 )
#define IOT_TSENSOR_DISABLED        ( 2 )
#define IOT_TSENSOR_CLOSED          ( 3 )
#define IOT_TSENSOR_GET_TEMP_FAILED ( 4 )
#define IOT_TSENSOR_SET_FAILED      ( 5 )
#define IOT_TSENSOR_NOT_SUPPORTED   ( 6 )

/**
 * @brief tsensor threshold reached status
 */
typedef enum
{
    eTsensorMinThresholdReached,    /*!< min temperature threshold reached status */
    eTsensorMaxThresholdReached     /*!< max temperature threshold reached status */
} IotTsensorStatus_t;

/**
 * @brief Ioctl request types.
 */
typedef enum
{
    eTsensorSetMinThreshold,    /*!< Set min temperature threshold using IotI2CIoctlConfig_t */
    eTsensorSetMaxThreshold,    /*!< Set max temperature threshold using IotI2CIoctlConfig_t */
    eTsensorGetMinThreshold,    /*!< Get min temperature threshold using IotI2CIoctlConfig_t */
    eTsensorGetMaxThreshold,    /*!< Get max temperature threshold using IotI2CIoctlConfig_t */
    eTsensorPerformCalibration  /*!< Perform calibration of the sensor */
} IotTsensorIoctlRequest_t;

/**
 * @brief   tsensor descriptor type defined in the source file.
 */
struct IotTsensorDescriptor_t;

/**
 * @brief   IotTsensorHandle_t type is the tsensor handle returned by calling iot_tsensor_open()
 *          this is initialized in open and returned to caller. Caller must pass this pointer
 *          to the rest of the APIs.
 */
typedef struct IotTsensorDescriptor_t * IotTsensorHandle_t;

/**
 * @brief The callback function for completion of Tsensor operation.
 *
 * @param[in]   xStatus         tsensor threshold reached status
 * @param[in]   pvUserContext   user provid context
 */
typedef void( * IotTsensorCallback_t)(IotTsensorStatus_t xStatus, void * pvUserContext);

/**
 * @brief   iot_tsensor_open is used to initialize the temperature sensor.
 *          It sets up the clocks  and power etc, if the sensor is internal
 *          and sets up the communication channel if the sensor is external.
 *
 * @param[in]   lTsensorInstance   The instance of the tsensor to initialize. The instance
 *                                 number is platform specific. i,e if you have more than one temperature
 *                                 ensors, then 2 instances point to 2 different sensors.
 *
 * @return Handle to tsensor interface on success, or NULL on failure.
 */
IotTsensorHandle_t iot_tsensor_open(int32_t lTsensorInstance);

/**
 * @brief   Set the callback to be called when a threshold is reached on the sensor.
 *          The caller must set the threshold level using IOCTL first.
 *
 * @param[in]   xTsensorHandle  Handle to tsensor driver returned in open() call
 * @param[in]   xCallback       The callback function to be called on completion of transaction.
 * @param[in]   pvUserContext   user provid context
 */
void iot_tsensor_set_callback(IotTsensorHandle_t const xTsensorHandle,
                              IotTsensorCallback_t xCallback,
                              void * pvUserContext);

/**
 * @brief   iot_tsensor_enable is used to enable the temperature sensor
 *          to start reading the temperature and trigger thresholds (if any were set and supported)
 *
 * @param[in]   xTsensorHandle  Handle to tsensor driver returned in
 *                              iot_tsensor_open
 *
 * @return  SUCCESS=IOT_TSENSOR_SUCCESS,
 *          FAILED=IOT_TSENSOR_INVALID_VALUE,
 */
int32_t iot_tsensor_enable(IotTsensorHandle_t const xTsensorHandle);

/**
 * @brief   iot_tsensor_disable is used to disable the temperature sensor
 *          which stops monitoring the temperature.
 *
 * @param[in]   xTsensorHandle  Handle to tsensor driver returned in
 *                              iot_tsensor_open
 *
 * @return  SUCCESS=IOT_TSENSOR_SUCCESS,
 *          FAILED=IOT_TSENSOR_INVALID_VALUE,
 */
int32_t iot_tsensor_disable(IotTsensorHandle_t const xTsensorHandle);

/**
 * @brief   iot_tsensor_get_temp is used to get the current temperature
 *          read from the sensor.
 *
 * @param[in]   xTsensorHandle  handle to tsensor driver returned in
 *                              iot_tsensor_open
 * @param[out]  lTemp           temperature read from the sensor.
 *
 * @return  SUCCESS=IOT_TSENSOR_SUCCESS,
 *          Input_Error=IOT_TSENSOR_INVALID_VALUE,
 *          Sensor_Disabled=IOT_TSENSOR_DISABLED,
 *          Get_Temp_Fail=IOT_TSENSOR_GET_TEMP_FAILED
 */
int32_t iot_tsensor_get_temp(IotTsensorHandle_t const xTsensorHandle,
                             int32_t * lTemp);

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
 * @return  SUCCESS=IOT_TSENSOR_SUCCESS,
 *          FAILED=IOT_TSENSOR_INVALID_VALUE,
 *          Sensor_Disabled=IOT_TSENSOR_DISABLED,
 *          Ioctl_Fail=IOT_TSENSOR_SET_FAILED
 *          NOT_SUPPORT=IOT_TSENSOR_NOT_SUPPORTED
 *
 */
int32_t iot_tsensor_ioctl(IotTsensorHandle_t const xTsensorHandle,
                          IotTsensorIoctlRequest_t xRequest,
                          void * const pvBuffer);

/**
 * @brief iot_tsensor_close is used to de-initialize Tsensor.
 *
 * @param[in]   xTsensorHandle  handle to tsensor driver returned in
 *                              iot_tsensor_open
 *
 * @return  SUCCESS=IOT_TSENSOR_SUCCESS,
 *          FAILED=IOT_TSENSOR_INVALID_VALUE,
 */
int32_t iot_tsensor_close( IotTsensorHandle_t const xTsensorHandle);


/**
@}
*/
// end of group iot_tsensor

#endif /* ifndef _IOT_TSENSOR_H_ */
