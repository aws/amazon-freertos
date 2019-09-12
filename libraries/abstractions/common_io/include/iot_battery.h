/*
 * Amazon FreeRTOS Common IO V1.0.0
 * Copyright (C) 2019 Amazon.com, Inc. or its affiliates.  All Rights Reserved.
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
#define IOT_BATTERY_SUCCESS                 ( 0 )
#define IOT_BATTERY_INVALID_VALUE           ( 1 )
#define IOT_BATTERY_NOT_EXIST               ( 2 )
#define IOT_BATTERY_READ_FAILED             ( 3 )
#define IOT_BATTERY_FUNCTION_NOT_SUPPORTED  ( 4 )

/**
 * @brief Battery Type
 */
typedef enum
{
    eBatteryChargeable,     /*!< Re-chargeable batteries with charging enabled */
    eBatteryNotChargeable   /*!< Not chargeable batteries */
} IotBatteryType_t;

/**
 * @brief Charger type
 */
typedef enum
{
    eBatteryChargeNone,         /*!< No Charge source, or non-chargeable battery */
    eBatteryChargeUSB,          /*!< USB battery source */
    eBatteryChargePowerSupply,  /*!< AC power supply source */
    erBatteryChargeWireless,    /*!< Wireless charging source */
} IotBatteryChargeSource_t;

/**
 * @brief Battery status
 */
typedef enum
{
    eBatteryCharging,           /*!< Battery is Charging. Applicable only if the battery is chargeable */
    eBatteryDischarging,        /*!< Battery is Discharging. This is the normal status of battery is no charger
                                    is connected */
    eBatteryNotCharging,        /*!< Battery is not charging. This is set when charger is connected, but battery is not
                                    charging - usually an error condition */
    eBatteryChargeFull,         /*!< Battery is fully charged. This status will be set only when the charger is connected
                                    and battery is fully charged */
    eBatteryChargeLow,          /*!< Battery is low on charging and no charger connected. This status is set when battery
                                    is discharging and reaches a low cahrge value. Low charge value is defined by implementation */
    eBatteryOverVoltage,        /*!< Battery maximum voltage limit reached. */
    eBatteryUnderVoltage,       /*!< Battery low voltage limit reached. */
    eBatteryOverTemp,           /*!< Battery maximum temperature limit reached. */
    eBatteryUnderTemp,          /*!< Battery minimum temperature limit reached. */
    eBatteryOverChargingTemp,   /*!< Battery maximum charging temperature limit reached. */
    eBatteryUnderhargingTemp,   /*!< Battery minimum charging temperature limit reached. */
    eBatteryChargeTimeExpired,  /*!< Battery charge timer expired. Charge timer can be set if the underlying hardware supports it using
                                   eSetBatteryChargeTimer IOCTL, and once the chargeTimer expires registered callback is called with this status set */
    eBatteryUnknown             /*!< Unknown status. Default status if no battery is present */
} IotBatteryStatus_t;

/**
 * @brief Battery information.
 */
typedef struct IotBatteryInfo
{
    IotBatteryType_t    xBatteryType;       /*!< Battery type. chargeable or non-chargeable */
    uint16_t            usMinVoltage;       /*!< minimum designed voltage battery can be used at */
    uint16_t            usMaxVoltage;       /*!< maximum designed voltage battery can be used at */
    int16_t             sMinTemperature;    /*!< lowest temperature the battery can be used at by design */
    int16_t             sMaxTemperature;    /*!< max temperature the battery can be used at by design */
    uint8_t             usMaxCapacity;      /*!< max capacity of battery by design */
    uint8_t             ucAsyncSupported;   /*!< Is asynchronous notifications supported by the driver. This will be
                                                set, if the battery/charging has an IC/battery and charging hw module that
                                                can atonomously monitor battery characterstics and generate interrupts */
} IotBatteryInfo_t;

/**
 * @brief Ioctl request types.
 */
typedef enum IotBatteryIoctlRequest
{
    eSetBatteryMinVoltageThreshold,     /*!< Set the battery minimum voltage threshold. When the voltage goes below this threshold
                                            value, the registered callback will be called with the status set to "eBatteryUnderVoltage".
                                            If battery management HW does not have a way to monitor voltage, this IOCTL will not be supported. */
    eSetBatteryMaxVoltageThreshold,     /*!< Set the battery mimimum voltage threshold. When the voltage goes below this threshold
                                            value, the registered callback will be called with the status set to "eBatteryOverVoltage".
                                            If battery management HW does not have a way to monitor voltage, this IOCTL will not be supported. */

    eSetBatteryMinBatteryTempThreshold, /*!< Set the minimum battery temperature threshold. When the battery temperature goes below this value,
                                            the registered callback will be called with the status value "eBatteryUnderTemp".
                                            This IOCTL takes integer value as input */
    eSetBatteryMaxBatteryTempThreshold, /*!< Set the maximum battery temperature threshold. When the battery temperature goes above this value,
                                            the registered callback will be called with the status value "eBatteryUnderTemp".
                                            This IOCTL takes integer value as input */
    eSetBatteryMinChargeTempThreshold,  /*!< Set the minimum charge temperature threshold. When the charging temperature goes below this value,
                                            the registered callback will be called with the status value "eBatteryUnderChargingTemp".
                                            This IOCTL takes integer value as input */
    eSetBatteryMaxChargeTempThreshold,  /*!< Set the maximum charge temperature threshold. When the charging temperature goes above this value,
                                            the registered callback will be called with the status value "eBatteryOverChargingTemp".
                                            This IOCTL takes integer value as input */
    eSetBatteryMaxOutputCurrent,        /*!< Set the maximum output current limit from the battery.
                                            This IOCTL takes integer value as input */
    eSetBatteryMaxInputChargeCurrent,   /*!< Set the maximum input charging current limit to the battery.
                                            This IOCTL takes integer value as input */
    eSetBatteryChargeTimer,             /*!< Set the battery charging timer limit. AFter the timer expires charging will stop.
                                            This IOCTL takes integer value as input*/

    eGetBatteryMinVoltageThreshold,     /*!< Get the battery minimum thresold. */
    eGetBatteryMaxVoltageThreshold,     /*!< Get the battery maximum thresold. */
    eGetBatteryOCV,                     /*!< Get the battery OCV (open circuit voltage) */
    eGetBatteryBootVoltage,             /*!< Get the battery boot-up voltage */
    eGetBatteryMinBatteryTempThreshold, /*!< Get the minimum battery temperature threshold. */
    eGetBatteryMaxBatteryTempThreshold, /*!< Get the maximum battery temperature threshold. */
    eGetBatteryMinChargeTempThreshold,  /*!< Get the minimum charging temperature threshold. */
    eGetBatteryMaxChargeTempThreshold,  /*!< Get the maximum charging temperature threshold. */
    eGetBatteryMaxOutputCurrent,        /*!< Get the battery output current limit. */
    eGetBatteryMaxInputChargeCurrent,   /*!< Get the battery input charge current limit. */
    eGetBatteryChargeTimer,             /*!< Get the current battery charge time limit */
    eGetBatteryChargeSource,            /*!< Get the current battery charge source */
    eGetBatteryStatus                   /*!< Get the battery status. Returns one of the IotBatteryStatus_t values */

} IotBatteryIoctlRequest_t;

/**
 * @brief   Battery descriptor type defined in the source file.
 */
struct  IotBatteryDescriptor_t;

/**
 * @brief   IotBatteryHandle_t type is the Battery handle returned by calling iot_battery_open()
 *          this is initialized in open and returned to caller. Caller must pass this pointer
 *          to the rest of the APIs.
 */
typedef struct IotBatteryDescriptor_t * IotBatteryHandle_t;

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
 * @param[in] pvUserContext User Context passed when setting the callback.
 *                          This is not used by the driver, but just passed back to the user
 *                          in the callback.
 */
typedef void ( * IotBatteryCallback_t)( void * pvUserContext );

/**
 * @brief   iot_battery_open is used to initialize the Battery and Charging driver.
 *          this API can return error if battery is not present or some other error.
 *
 * @param[in]   lBatteryInstance   The instance of the battery interface to initialize.
 *                                  Usually, there is only one battery interface.
 *
 * @return  returns the handle IotBatteryHandle_t on success, or NULL on failure.
 */
IotBatteryHandle_t iot_battery_open( int32_t lBatteryInstance );

/*!
 * @brief   iot_battery_set_callback is used to set the callback to be called when a notification
 *          needs to be sent to the caller. THe callback is only called if the battery supports async
 *          notifications. Caller must check isAsyncNotificationSupported value before registering
 *          for callbacks, if async is not supported, registering for callback has no affect, and the callback
 *          will never be called.
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
 * @return  returns a pointer to the IotBatteryInfo structure on success
 *          or returns NULL
 */
IotBatteryInfo_t * iot_battery_getInfo( IotBatteryHandle_t const pxBatteryHandle);

/*!
 * @brief   iot_battery_current is used to get the battery current in mA.
 *
 * @param[in]   pxBatteryHandle handle to Battery driver returned in
 *                              iot_battery_open()
 * @param[out]  usCurrent   battery current value.
 *
 * @return  returns IOT_BATTERY_SUCCESS on success or returns
 *          one of IOT_BATTERY_INVALID_VALUE or IOT_BATTERY_NOT_EXIST or IOT_BATTERY_READ_FAILED on error or
 *          IOT_BATTERY_FUNCTION_NOT_SUPPORTED if its not supported by the hardware.
 */
int32_t iot_battery_current( IotBatteryHandle_t const pxBatteryHandle,
                            uint16_t * pusCurrent);

/*!
 * @brief   iot_battery_voltage is used to get the battery voltage in milli-volts.
 *
 * @param[in]   pxBatteryHandle handle to Battery driver returned in
 *                              iot_battery_open()
 * @param[out]  usVoltage  battery voltage value.
 *
 * @return  returns IOT_BATTERY_SUCCESS on success or returns
 *          one of IOT_BATTERY_INVALID_VALUE or IOT_BATTERY_NOT_EXIST or IOT_BATTERY_READ_FAILED on error or
 *          IOT_BATTERY_FUNCTION_NOT_SUPPORTED if its not supported by the hardware.
 */
int32_t iot_battery_voltage( IotBatteryHandle_t const pxBatteryHandle,
                            uint16_t * pusVoltage);

/*!
 * @brief   iot_battery_chargeLevel is used to get the battery charging level in percentage (from 1 to 100).
 *
 * @param[in]   pxBatteryHandle handle to Battery driver returned in
 *                              iot_battery_open()
 * @param[out]  ucChargeLevel battery charging value.
 *
 * @return  returns IOT_BATTERY_SUCCESS on success or returns
 *          one of IOT_BATTERY_INVALID_VALUE or IOT_BATTERY_NOT_EXIST or IOT_BATTERY_READ_FAILED on error or
 *          IOT_BATTERY_FUNCTION_NOT_SUPPORTED if its not supported by the hardware.
 */
int32_t iot_battery_chargeLevel( IotBatteryHandle_t const pxBatteryHandle,
                                    uint8_t * pucChargeLevel);


/*!
 * @brief   iot_battery_capacity is used to get the current battery capacity value in mAh
 *
 * @param[in]   pxBatteryHandle handle to Battery driver returned in
 *                              iot_battery_open()
 * @param[out]  usCapacity  battery capacity value.
 *
 * @return  returns IOT_BATTERY_SUCCESS on success or returns
 *          one of IOT_BATTERY_INVALID_VALUE or IOT_BATTERY_NOT_EXIST or IOT_BATTERY_READ_FAILED on error or
 *          IOT_BATTERY_FUNCTION_NOT_SUPPORTED if its not supported by the hardware.
 */
int32_t iot_battery_capacity( IotBatteryHandle_t const pxBatteryHandle,
                                    uint16_t * pusCapacity);

/*!
 * @brief   iot_battery_temp is used to get the battery temperature in farenheit
 *
 * @param[in]   pxBatteryHandle handle to Battery driver returned in
 *                              iot_battery_open()
 * @param[out]  usTemp   battery temperature in Farenheit
 *
 * @return  returns IOT_BATTERY_SUCCESS on success or returns
 *          one of IOT_BATTERY_INVALID_VALUE or IOT_BATTERY_NOT_EXIST or IOT_BATTERY_READ_FAILED on error or
 *          IOT_BATTERY_FUNCTION_NOT_SUPPORTED if its not supported by the hardware.
 */
int32_t iot_battery_temp( IotBatteryHandle_t const pxBatteryHandle,
                        uint16_t * pusTemp);

/*!
 * @brief   iot_battery_enable_charging is used to enable battery charging if charging is supported.
 *          if battery is already at full charge, enable will not charge battery until the charging level
 *          drops below 100, and even if battery is at critical level, and charging is not enabled, battery will
 *          not be charging. Charging is always enabled by default unless disabled by using this API.
 *
 * @param[in]   pxBatteryHandle handle to Battery driver returned in
 *                              iot_battery_open()
 * @param[in]  ucEnable   Enable or Disable the charging of battery. set to 1 to enable, or 0 to disable.
 *
 * @return  returns IOT_BATTERY_SUCCESS on success or returns
 *          one of IOT_BATTERY_INVALID_VALUE or IOT_BATTERY_NOT_EXIST on error or
 *          IOT_BATTERY_FUNCTION_NOT_SUPPORTED if its not supported by the hardware.
 */
int32_t iot_battery_enable_charging( IotBatteryHandle_t const pxBatteryHandle,
                                    uint8_t * pucEnable);

/*!
 * @brief   iot_battery_is_charging is used to query if the battery is currently charging.
 *
 * @param[in]   pxBatteryHandle handle to Battery driver returned in
 *                              iot_battery_open()
 * @param[out]  ucCharging   If charging this will contain value of 1 otherwise 0.
 *
 * @return  returns IOT_BATTERY_SUCCESS on success or returns
 *          one of IOT_BATTERY_INVALID_VALUE or IOT_BATTERY_NOT_EXIST on error or
 *          IOT_BATTERY_FUNCTION_NOT_SUPPORTED if its not supported by the hardware.
 */
int32_t iot_battery_is_charging( IotBatteryHandle_t const pxBatteryHandle,
                                uint8_t * pucCharging);

/**
 * @brief   iot_battery_ioctl is used to set Battery configuration and
 *          Battery properties like battery threshold,  temperature threshold, charging max, chargin min etc..
 *
 * @param[in]   pxBatteryHandle  handle to Battery driver returned in
 *                          iot_battery_open()
 * @param[in]   xRequest    configuration request of type IotBatteryIoctlRequest_t
 * @param[in,out] pvBuffer  buffer holding Battery set and get values.
 *
 * @return  returns IOT_BATTERY_SUCCESS on success or returns
 *          one of IOT_BATTERY_INVALID_VALUE, IOT_BATTERY_NOT_EXIST, IOT_BATTERY_FUNCTION_NOT_SUPPORTED on error.
 */
int32_t iot_battery_ioctl( IotBatteryHandle_t const pxBatteryHandle,
                            IotBatteryIoctlRequest_t xRequest,
                            void * const pvBuffer );

/**
 * @brief   iot_battery_close is used to de-Initialize Battery driver.
 *
 * @param[in]   pxBatteryHandle  handle to Battery interface.
 *
 * @return  returns IOT_BATTERY_SUCCESS on success or returns
 *          one of IOT_BATTERY_INVALID_VALUE on error.
 */
int32_t iot_battery_close( IotBatteryHandle_t const pxBatteryHandle );

/**
 * @}
 */

#endif /* ifndef _IOT_BATTERY_H_ */

