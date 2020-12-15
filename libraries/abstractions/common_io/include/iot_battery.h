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

/*******************************************************************************
 * @file iot_battery.h
 *
 * @brief HAL APIs for Battery
 *******************************************************************************
 */

/**
 * @file    iot_battery.h
 * @brief   This file contains all the Battery HAL API definitions. The APIs defined in this file
 *          helps with battery information and charging related functions.
 */

#ifndef _IOT_BATTERY_H_
#define _IOT_BATTERY_H_

/**
 * @defgroup iot_battery BATTERY HAL APIs
 * @{
 */

/**
 * @brief Return values used by BATTERY driver
 */
#define IOT_BATTERY_SUCCESS                   ( 0 )  /*!< Battery operation completed successfully. */
#define IOT_BATTERY_INVALID_VALUE             ( 1 )  /*!< At least one parameter is invalid. */
#define IOT_BATTERY_NOT_EXIST                 ( 2 )  /*!< No battery exists in this hardware. */
#define IOT_BATTERY_READ_FAILED               ( 3 )  /*!< Battery read operation failed. */
#define IOT_BATTERY_FUNCTION_NOT_SUPPORTED    ( 4 )  /*!< Battery operation not supported. */

/**
 * @brief Battery Type
 */
typedef enum
{
    eBatteryChargeable,   /*!< Re-chargeable batteries with charging enabled */
    eBatteryNotChargeable /*!< Not chargeable batteries */
} IotBatteryType_t;

/**
 * @brief Charger type
 */
typedef enum
{
    eBatteryChargeNone,        /*!< No Charge source, or non-chargeable battery */
    eBatteryChargeUSB,         /*!< USB battery source */
    eBatteryChargePowerSupply, /*!< AC power supply source */
    erBatteryChargeWireless,   /*!< Wireless charging source */
} IotBatteryChargeSource_t;

/**
 * @brief Battery status
 */
typedef enum
{
    eBatteryCharging,          /*!< Battery is Charging. Applicable only if the battery is chargeable */
    eBatteryDischarging,       /*!< Battery is Discharging. This is the normal status of battery is no charger
                                *  is connected */
    eBatteryNotCharging,       /*!< Battery is not charging. This is set when charger is connected, but battery is not
                                *  charging - usually an error condition */
    eBatteryChargeFull,        /*!< Battery is fully charged. This status will be set only when the charger is connected
                                *  and battery is fully charged */
    eBatteryChargeLow,         /*!< Battery is low on charging and no charger connected. This status is set when battery
                                *  is discharging and reaches a low cahrge value. Low charge value is defined by implementation */
    eBatteryOverVoltage,       /*!< Battery maximum voltage limit reached. */
    eBatteryUnderVoltage,      /*!< Battery low voltage limit reached. */
    eBatteryOverTemp,          /*!< Battery maximum temperature limit reached. */
    eBatteryUnderTemp,         /*!< Battery minimum temperature limit reached. */
    eBatteryOverChargingTemp,  /*!< Battery maximum charging temperature limit reached. */
    eBatteryUnderhargingTemp,  /*!< Battery minimum charging temperature limit reached. */
    eBatteryChargeTimeExpired, /*!< Battery charge timer expired. Charge timer can be set if the underlying hardware supports it using
                                * eSetBatteryChargeTimer IOCTL, and once the chargeTimer expires registered callback is called with this status set */
    eBatteryUnknown            /*!< Unknown status. Default status if no battery is present */
} IotBatteryStatus_t;

/**
 * @brief Battery information.
 */
typedef struct IotBatteryInfo
{
    IotBatteryType_t xBatteryType; /*!< Battery type. chargeable or non-chargeable */
    uint16_t usMinVoltage;         /*!< minimum designed voltage battery can be used at */
    uint16_t usMaxVoltage;         /*!< maximum designed voltage battery can be used at */
    int16_t sMinTemperature;       /*!< lowest temperature the battery can be used at by design */
    int32_t lMaxTemperature;       /*!< max temperature the battery can be used at by design */
    uint16_t usMaxCapacity;        /*!< max capacity of battery by design */
    uint8_t ucAsyncSupported;      /*!< Is asynchronous notifications supported by the driver. This will be
                                    *  set, if the battery/charging has an IC/battery and charging hw module that
                                    *  can atonomously monitor battery characterstics and generate interrupts */
} IotBatteryInfo_t;

/**
 * @brief Ioctl request types.
 */
typedef enum IotBatteryIoctlRequest
{
    eSetBatteryMinVoltageThreshold,     /*!< Set the battery minimum voltage threshold. When the voltage goes below this threshold
                                         *   value, the registered callback will be called with the status set to "eBatteryUnderVoltage".
                                         *   If battery management HW does not have a way to monitor voltage, this IOCTL will not be supported.
                                         *   Parameter is uint16_t value in milliVolt units. */
    eSetBatteryMaxVoltageThreshold,     /*!< Set the battery mimimum voltage threshold. When the voltage goes below this threshold
                                         *   value, the registered callback will be called with the status set to "eBatteryOverVoltage".
                                         *   If battery management HW does not have a way to monitor voltage, this IOCTL will not be supported.
                                         *   Parameter is uint16_t value in milliVolt units. */
    eSetBatteryMinBatteryTempThreshold, /*!< Set the minimum battery temperature threshold. When the battery temperature goes below this value,
                                         *   the registered callback will be called with the status value "eBatteryUnderTemp".
                                         *   This IOCTL takes int16_t value as input in milliCelcius units. */
    eSetBatteryMaxBatteryTempThreshold, /*!< Set the maximum battery temperature threshold. When the battery temperature goes above this value,
                                         *   the registered callback will be called with the status value "eBatteryUnderTemp".
                                         *   This IOCTL takes int32_t value as input in milliCelcius units. */
    eSetBatteryMinChargeTempThreshold,  /*!< Set the minimum charge temperature threshold. When the charging temperature goes below this value,
                                         *   the registered callback will be called with the status value "eBatteryUnderChargingTemp".
                                         *   This IOCTL takes int16_t value as input in milliCelcius units.*/
    eSetBatteryMaxChargeTempThreshold,  /*!< Set the maximum charge temperature threshold. When the charging temperature goes above this value,
                                         *   the registered callback will be called with the status value "eBatteryOverChargingTemp".
                                         *   This IOCTL takes int32_t value as input in milliCelcius units. */
    eSetBatteryMaxOutputCurrent,        /*!< Set the maximum output current limit from the battery.
                                         *   This IOCTL takes uint16_t value as input in milliAmp units. */
    eSetBatteryMaxInputChargeCurrent,   /*!< Set the maximum input charging current limit to the battery.
                                         *   This IOCTL takes uint16_t value as input in milliAmp units. */
    eSetBatteryChargeTimer,             /*!< Set the battery charging timer limit. AFter the timer expires charging will stop.
                                         *   This IOCTL takes uint32_t value as input in milliSec units. */
    eGetBatteryMinVoltageThreshold,     /*!< Get the battery minimum thresold.
                                         *   This IOCTL returns uint16_t value in milliVolt units. */
    eGetBatteryMaxVoltageThreshold,     /*!< Get the battery maximum thresold.
                                         *   This IOCTL returns uint16_t value in milliVolt units. */
    eGetBatteryOCV,                     /*!< Get the battery OCV (open circuit voltage)
                                         *   This IOCTL returns uint16_t value in milliVolt units. */
    eGetBatteryBootVoltage,             /*!< Get the battery boot-up voltage
                                         *   This IOCTL returns uint16_t value in milliVolt units. */
    eGetBatteryMinBatteryTempThreshold, /*!< Get the minimum battery temperature thresholde
                                         *   This IOCTL returns int16_t value in milliCelcius units. */
    eGetBatteryMaxBatteryTempThreshold, /*!< Get the maximum battery temperature threshold.
                                         *   This IOCTL returns int32_t value in milliCelcius units. */
    eGetBatteryMinChargeTempThreshold,  /*!< Get the minimum charging temperature threshold.
                                         *   This IOCTL returns int16_t value in milliCelcius units. */
    eGetBatteryMaxChargeTempThreshold,  /*!< Get the maximum charging temperature threshold.
                                         *   This IOCTL returns int32_t value in milliCelcius units. */
    eGetBatteryMaxOutputCurrent,        /*!< Get the battery output current limit.
                                         *   This IOCTL returns uint16_t value in milliAmp units. */
    eGetBatteryMaxInputChargeCurrent,   /*!< Get the battery input charge current limit.
                                         *   This IOCTL returns uint16_t value in milliAmp units. */
    eGetBatteryChargeTimer,             /*!< Get the current battery charge time limit.
                                         *   This IOCTL returns uint32_t value in milliSec units. */
    eGetBatteryChargeSource,            /*!< Get the current battery charge source.
                                         *   This IOCTL returns a IotBatteryChargeSource_t as it's parameter. */
    eGetBatteryStatus                   /*!< Get the battery status. Returns one of the IotBatteryStatus_t values as it's parameter. */
} IotBatteryIoctlRequest_t;

/**
 * @brief   Battery descriptor type defined in the source file.
 */
struct  IotBatteryDescriptor;

/**
 * @brief   IotBatteryHandle_t type is the Battery handle returned by calling iot_battery_open()
 *          this is initialized in open and returned to caller. Caller must pass this pointer
 *          to the rest of the APIs.
 */
typedef struct IotBatteryDescriptor * IotBatteryHandle_t;

/**
 * @brief   Battery notification callback type. This callback is passed
 *          to the driver by using iot_battery_set_callback API. The callback is
 *          used to get the notifications about battery status changes for ex: when charging starts, or
 *          when critical threshold is reached or when battery is full etc...
 *          The callback is only used if the driver supports the asynchronous notifications. The caller
 *          must get the batteryInfo to find if the asynchronous notifications are supported.
 *          On simiple underlying batteries, the caller/application must manage the battery conditions by
 *          periodically probing the battery using read APIs, and for sophisticated battery management service
 *          modules, these can be managed in the HW hence asyncrhonous notifications can be sent (using registered callback)
 *
 * @param[in] xStatus       battery status from IotBatteryStatus_t
 * @param[in] pvUserContext User Context passed when setting the callback.
 *                          This is not used by the driver, but just passed back to the user
 *                          in the callback.
 */
typedef void ( * IotBatteryCallback_t)( IotBatteryStatus_t xStatus,
                                        void * pvUserContext );

/**
 * @brief   iot_battery_open is used to initialize the Battery and Charging driver.
 *
 * @param[in]   lBatteryInstance   The instance of the battery interface to initialize.
 *                                  Usually, there is only one battery interface.
 *
 * @return
 * - handle to the Battery controller on success
 * - NULL, if
 *    - invalid instance
 *    - open same instance more than once before closing it.
 *    - on failure.
 */
IotBatteryHandle_t iot_battery_open( int32_t lBatteryInstance );

/*!
 * @brief   iot_battery_set_callback is used to set the callback to be called when a notification
 *          needs to be sent to the caller. THe callback is only called if the battery supports async
 *          notifications. Caller must check isAsyncNotificationSupported value before registering
 *          for callbacks, if async is not supported, registering for callback has no affect, and the callback
 *          will never be called.
 *
 * @note Single callback is used for all features that support callback
 *   - eBatteryChargeTimeExpired,
 *   - eSetBatteryMinVoltageThreshold, eSetBatteryMaxVoltageThreshold,
 *   - eSetBatteryMinBatteryTempThreshold, eSetBatteryMaxBatteryTempThreshold,
 *   - eSetBatteryMinChargeTempThreshold, eSetBatteryMaxChargeTempThreshold,
 * @note Newly set callback overrides the one previously set
 * @note This callback is per handle. Each instance has its own callback.
 *
 * @warning If input handle or if callback function is NULL, this function silently takes no action.
 *
 * @param[in]   pxBatteryHandle  handle to Battery driver returned in
 *                          iot_battery_open()
 * @param[in]   xCallback   callback function to be called.
 * @param[in]   pvUserContext   user context to be passed when callback is called.
 *
 */
void iot_battery_set_callback( IotBatteryHandle_t const pxBatteryHandle,
                               IotBatteryCallback_t xCallback,
                               void * pvUserContext );

/*!
 * @brief   iot_battery_getInfo is used to get the battery info
 *
 * @param[in]   pxBatteryHandle handle to Battery driver returned in
 *                              iot_battery_open()
 *
 * @return
 * - returns a pointer to the IotBatteryInfo structure on success
 * - NULL on error
 */
IotBatteryInfo_t * iot_battery_getInfo( IotBatteryHandle_t const pxBatteryHandle );

/*!
 * @brief   iot_battery_current is used to get the battery current in mA.
 *
 * @param[in]   pxBatteryHandle handle to Battery driver returned in
 *                              iot_battery_open()
 * @param[out]  pusCurrent   battery current value.
 *
 * @return
 * - IOT_BATTERY_SUCCESS on success
 * - IOT_BATTERY_INVALID_VALUE if pxBatteryHandle == NULL or pusCurrent == NULL
 * - IOT_BATTERY_NOT_EXIST if no battery present.
 * - IOT_BATTERY_READ_FAILED on error obtaining valid reading
 * - IOT_BATTERY_FUNCTION_NOT_SUPPORTED if its not supported by the hardware.
 */
int32_t iot_battery_current( IotBatteryHandle_t const pxBatteryHandle,
                             uint16_t * pusCurrent );

/*!
 * @brief   iot_battery_voltage is used to get the battery voltage in milli-volts.
 *
 * @param[in]   pxBatteryHandle handle to Battery driver returned in
 *                              iot_battery_open()
 * @param[out]  pusVoltage  battery voltage value.
 *
 * @return
 * - IOT_BATTERY_SUCCESS on success
 * - IOT_BATTERY_INVALID_VALUE if pxBatteryHandle == NULL  or pusVoltage == NULL
 * - IOT_BATTERY_NOT_EXIST if no battery in this hardware
 * - IOT_BATTERY_READ_FAILED on error
 * - IOT_BATTERY_FUNCTION_NOT_SUPPORTED if its not supported by the hardware.
 */
int32_t iot_battery_voltage( IotBatteryHandle_t const pxBatteryHandle,
                             uint16_t * pusVoltage );

/*!
 * @brief   iot_battery_chargeLevel is used to get the battery charging level in percentage (from 1 to 100).
 *
 * @param[in]   pxBatteryHandle handle to Battery driver returned in
 *                              iot_battery_open()
 * @param[out]  pucChargeLevel battery charging value.
 *
 * @return
 * - IOT_BATTERY_SUCCESS on success
 * - IOT_BATTERY_INVALID_VALUE if pxBatteryHandle == NULL or pusChargeLevel == NULL
 * - IOT_BATTERY_NOT_EXIST if no battery in this hardware
 * - IOT_BATTERY_READ_FAILED on error
 * - IOT_BATTERY_FUNCTION_NOT_SUPPORTED if its not supported by the hardware.
 */
int32_t iot_battery_chargeLevel( IotBatteryHandle_t const pxBatteryHandle,
                                 uint8_t * pucChargeLevel );


/*!
 * @brief   iot_battery_capacity is used to get the current battery capacity value in mAh
 *
 * @param[in]   pxBatteryHandle handle to Battery driver returned in
 *                              iot_battery_open()
 * @param[out]  pusCapacity  battery capacity value.
 *
 * @return
 * - IOT_BATTERY_SUCCESS on success
 * - IOT_BATTERY_INVALID_VALUE if pxBatteryHandle == NULL or pusCapacity == NULL
 * - IOT_BATTERY_NOT_EXIST if no battery in this hardware
 * - IOT_BATTERY_READ_FAILED on error
 * - IOT_BATTERY_FUNCTION_NOT_SUPPORTED if its not supported by the hardware.
 */
int32_t iot_battery_capacity( IotBatteryHandle_t const pxBatteryHandle,
                              uint16_t * pusCapacity );

/*!
 * @brief   iot_battery_temp is used to get the battery temperature in milliCelcius
 *
 * @param[in]   pxBatteryHandle handle to Battery driver returned in
 *                              iot_battery_open()
 * @param[out]  plTemp   battery temperature in milliCelcius
 *
 * @return
 * - IOT_BATTERY_SUCCESS on success
 * - IOT_BATTERY_INVALID_VALUE if pxBatteryHandle == NULL or psTemp == NULL
 * - IOT_BATTERY_NOT_EXIST if no battery in this hardware
 * - IOT_BATTERY_READ_FAILED on error
 * - IOT_BATTERY_FUNCTION_NOT_SUPPORTED if its not supported by the hardware.
 */
int32_t iot_battery_temp( IotBatteryHandle_t const pxBatteryHandle,
                          int32_t * plTemp );

/*!
 * @brief   iot_battery_enable_charging is used to enable battery charging if charging is supported.
 *          if battery is already at full charge, enable will not charge battery until the charging level
 *          drops below 100, and even if battery is at critical level, and charging is not enabled, battery will
 *          not be charging. Charging is always enabled by default unless disabled by using this API.
 *
 * @param[in]  pxBatteryHandle handle to Battery driver returned in
 *                              iot_battery_open()
 * @param[in]  pucEnable   Enable or Disable the charging of battery. set to 1 to enable, or 0 to disable.
 *
 * @return
 * - IOT_BATTERY_SUCCESS on success
 * - IOT_BATTERY_INVALID_VALUE if NULL handle or NULL voltage pointer
 * - IOT_BATTERY_NOT_EXIST if no battery in this hardware
 * - IOT_BATTERY_FUNCTION_NOT_SUPPORTED if its not supported by the hardware.
 */
int32_t iot_battery_enable_charging( IotBatteryHandle_t const pxBatteryHandle,
                                     uint8_t * pucEnable );

/*!
 * @brief   iot_battery_is_charging is used to query if the battery is currently charging.
 *
 * @param[in]   pxBatteryHandle handle to Battery driver returned in
 *                              iot_battery_open()
 * @param[out]  pucCharging   If charging this will contain value of 1 otherwise 0.
 *
 * @return
 * - IOT_BATTERY_SUCCESS on success
 * - IOT_BATTERY_INVALID_VALUE if pxBatteryHandle == NULL or pusCharging == NULL
 * - IOT_BATTERY_NOT_EXIST if no battery in this hardware
 * - IOT_BATTERY_FUNCTION_NOT_SUPPORTED if its not supported by the hardware.
 */
int32_t iot_battery_is_charging( IotBatteryHandle_t const pxBatteryHandle,
                                 uint8_t * pucCharging );

/**
 * @brief   iot_battery_ioctl is used to set Battery configuration and
 *          Battery properties like battery threshold,  temperature threshold, charging max, chargin min etc..
 *
 * @param[in]   pxBatteryHandle  handle to Battery driver returned in
 *                          iot_battery_open()
 * @param[in]   xRequest    configuration request of type IotBatteryIoctlRequest_t
 * @param[in,out] pvBuffer  buffer holding Battery set and get values.
 *
 * @return
 * - IOT_BATTERY_SUCCESS on success
 * - IOT_BATTERY_INVALID_VALUE if
 *    - pxBatteryHandle == NULL
 *    - invalid xRequest
 *    - pvBuffer == NULL when required
 * - IOT_BATTERY_NOT_EXIST if no battery in this hardware
 * - IOT_BATTERY_FUNCTION_NOT_SUPPORTED valid for any ioctl if its not supported by the hardware.
 */
int32_t iot_battery_ioctl( IotBatteryHandle_t const pxBatteryHandle,
                           IotBatteryIoctlRequest_t xRequest,
                           void * const pvBuffer );

/**
 * @brief   iot_battery_close is used to de-Initialize Battery driver.
 *
 * @param[in]   pxBatteryHandle  handle to Battery interface.
 *
 * @return
 * - IOT_BATTERY_SUCCESS on success
 * - IOT_BATTERY_INVALID_VALUE if
 *    - pxBatteryHandle == NULL
 *    - if is not in open state (already closed).
 */
int32_t iot_battery_close( IotBatteryHandle_t const pxBatteryHandle );

/**
 * @}
 */

#endif /* ifndef _IOT_BATTERY_H_ */
