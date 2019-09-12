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
 * @file    iot_power.h
 * @brief   This file contains all the Power HAL API definitions
 */

#ifndef _IOT_HAL_POWER_H_
#define _IOT_HAL_POWER_H_

#include <stdbool.h>
#include <stdint.h>

/**
 * @defgroup iot_power Power HAL APIs
 * @{
 */

/**
 * @brief Return values used by the driver.
 */
#define IOT_POWER_SUCCESS                ( 0 )
#define IOT_POWER_INVALID_VALUE          ( 1 )
#define IOT_POWER_NOT_INITIALIZED        ( 2 )
#define IOT_POWER_GET_FAILED             ( 3 )
#define IOT_POWER_SET_FAILED             ( 4 )
#define IOT_POWER_FUNCTION_NOT_SUPPORTED ( 5 )

/**
 * @brief   power descriptor type defined in the source file.
 */
struct                        IotPowerDescriptor;

/**
 * @brief   IotPowerHandle_t type is the power handle returned by calling iot_power_open()
 *          this is initialized in open and returned to caller. Caller must pass this pointer
 *          to the rest of the APIs.
 */
typedef struct IotPowerDescriptor   * IotPowerHandle_t;

/**
 * @brief   Power modes supported by the driver.
 *          These power modes may perform different actions on
 *          different platforms based on the underlying HW support.
 */
typedef enum
{
    eHighPowerPerfMode,        /*!< Set the high performance mode at the SoC level */
    eNormalPowerPerfMode,      /*!< Set the Normal mode. power_open sets the system to this mode by default*/
    eLowPowerPerfMode,         /*!< Set the lower performance mode at the Soc level */
    eUnknownPowerMode = 0xFFFF /*!< This mode cannot be set, and when a reset_mode is called currentMode will be set to eUnknown */
} IotPowerMode_t;

/**
 * @brief   Different Idle mode mappings to the threshold values set using the IOCTLs.
 */
typedef enum
{
    eIdleNoneMode,         /*!< No Idle mode entered. */
    eIdlePCMode,            /*!< Normal Idle mode with core collapsed or clock gated. Actual
                                mode implementation may differ based on the underlying SoC support. */
    eIdleClkSrcOffModeMode, /*!< Source clock off in addition to core collapse. Actual
                                mode implementation may differ based on the underlying SoC support. */
    eIdleVddOffMode         /*!< Vddcore off in addition to clock source and core collapse. Actual
                                mode implementation may differ based on the underlying SoC support. */
} IotPowerIdleMode_t;

/**
 * @brief   Wakeup source information.
 */
typedef struct IotPowerWakeupSources
{
    uint8_t * pucWakeupSources; /*!< Wakeup sources byte stream, where each bit in the bytes
                                may indicate a different wakeup source and is platform dependent */
    uint16_t usLength;          /*!< Length of the Wakeup sources in number of bytes.*/

} IotPowerWakeupSources_t;

/**
 * @brief Ioctl request types.
 */
typedef enum IotPowerIoctlRequest
{
    eSetPCWakeThreshold,            /*!< Set Wakeup threshold time to hint the power driver to power collapse the
                                      core if the scheduled wakeup time is greater than threshold time specified.
                                      Threshold values are uint32_t.  */
    eGetPCWakeThreshold,            /*!< Get Wakeup threshold time to hint the power driver to power collapse the
                                      core if the scheduled wakeup time is greater than threshold time specified.
                                      Threshold values are uint32_t.  */
    eSetClkSrcOffWakeThreshold,     /*!< Set Clock off threshold time to hint the power driver to turn off the clock
                                      source if the scheduled wakeup time is greater than threshold time specified.
                                      Threshold values are uint32_t.  */
    eGetClkSrcOffWakeThreshold,     /*!< Get Clock off threshold time to hint the power driver to turn off the clock
                                      source if the scheduled wakeup time is greater than threshold time specified.
                                      Threshold values are uint32_t.  */
    eSetVddOfffWakeThreshold,       /*!< Set power off (vdd rails off - or lower input voltage etc..) threshold time
                                      to hint the power driver to turn off the clock source if the scheduled wakeup
                                      time is greater than threshold time specified. Threshold values are uint32_t.  */
    eGetVddOfffWakeThreshold,       /*!< Get power off (vdd rails off - or lower input voltage etc..) threshold time
                                      to hint the power driver to turn off the clock source if the scheduled wakeup
                                      time is greater than threshold time specified. Threshold values are uint32_t.  */
    eSetWakeupSources,              /* Set wakeup sources that can wake-up the target from idle modes. Wakeup sources
                                        may only be needed for specific idle modes based on the thresholds set above
                                        and may are platform/SoC dependent. Takes input of type IotPowerWakeupSources_t */
    eGetWakeupSources,              /* Get the current wakeup sources set to wakeup the target from idle mode. Returns the
                                        wake-up sources as IotPowerWakeupSources_t type */
    eGetLastIdleMode                /* Get the last Idle mode entered when the target was idle. Returns one of the modes
                                        defined in IotPowerIdleMode_t */
} IotPowerIoctlRequest_t;

/**
 * @brief   This callback type is used to set a callback for idle enter and exit notification callback type.
 *          This callback is used for notifying the registered caller(s) before
 *          entering and after coming out of target low power state (i,e:idle/sleep).
 *          bIdleState is used to let the caller know if the callback is called when entering or exiting the low
 *          power mode. "true" indicates that target is entering idle and "false" indicates that its exiting.
 *          The callback is not used for any other mode transitions which are synchronous and blocking.
 *          For example when the caller calls a set_mode to eHighPerfMode, the mode is set in the same context and the call is blocking.
 *
 * @param[in] bIdleState    IdleState set to "true" when entering idle and "false" when exiting.
 * @param[in] pvUserContext User Context passed when setting the callback.
 */
typedef void ( * IotPowerCallback_t )( bool bIdleState,
                                        void * pvUserContext );

/**
 * @brief   iot_power_open is used to Initialize the CPU power management driver.
 *          power_open sets up the default mode to eNomralMode, so when the power_open is called,
 *          the caller must set the mode using set_mode to a specific mode of interest.
 *
 * @return  handle to power interface on success, or NULL on failure.
 */
IotPowerHandle_t iot_power_open( void );

/**
 * @brief   iot_power_set_mode is used to set or change the power mode to a specific mode. The actual changes made as part
 *          of switching to a specific power mode are platform specific. When set_mode is called to switch
 *          from high mode to low mode, the target may go through several steps to enter low performance mode.
 *          The aggregation of switching to a particular mode must be made prior to calling this, as a call
 *          to this API may change the CPU frequency and voltage which may affect the software execution.
 *          This API is blocking and the mode switched in the same context as the caller and is a blocking call.
 *
 * @param[in]   pxPowerHandle   handle to power driver returned in
 *                              iot_power_open
 * @param[in]   xMode           power mode to be switched to.
 *
 * @return  returns IOT_POWER_SUCCESS on success or returns
 *          one of IOT_POWER_INVALID_VALUE, IOT_POWER_SET_FAILED on error
 *          or IOT_POWER_FUNCTION_NOT_SUPPORTED.
 */
int32_t iot_power_set_mode( IotPowerHandle_t const pxPowerHandle,
                            IotPowerMode_t xMode );

/**
 * @brief   iot_power_reset_mode is used to reset the mode (i,e remove the vote to set mode). When reset mode is called,
 *          its implicitly notifying the OS to enter deepest low power mode possible in idle state based on
 *          existing timers. If a set_mode is called with any of the performance modes with out calling reset_mode,
 *          then when target enters idle state, it will not be able to enter deep power states since there is an
 *          existing vote for a performance mode from the caller. The target will still do clock gating of the core
 *          by doing a WFI and WFE.
 *          Additionally, if a reset_mode is called, followed by a get_mode, eUnKnownMode will be returned to the caller.
 *          But if reset_mode is called, and then target enters idle state, when coming out of the idle_state it will resume
 *          to a known previous state.
 *          For example, if we take the following flow:
 *          1. set_mode(eHighPerformanceMode)
 *          2. reset_modqe() -> At this point the current mode is set to eUnKnownMode
 *          3. Idle checks to see if it can enter a deep power state based on existing timers.
 *          4. If a callback is registered, the callback is called with bIdle set to "true"
 *          5. Target enters Idle state and goes to 'some' deep power state
 *          6. Target wakes up due to external interrupt or scheduled timer interrupt.
 *          7. If a callback is registered, the callback is called with bIdle set to "false"
 *          8. At this point the caller can set the desired performance state.
 *          9. Target is set to eHighPerformanceMode as this was the known mode before entering idle. (optional if mode was not set at #6)
 *
 * @param[in]   pxPowerHandle   handle to power driver returned in
 *                              iot_power_open
 *
 * @return  returns IOT_POWER_SUCCESS on success or returns
 *          one of IOT_POWER_INVALID_VALUE on error
 *          or IOT_POWER_FUNCTION_NOT_SUPPORTED.
 */
int32_t iot_power_reset_mode( IotPowerHandle_t const pxPowerHandle );

/**
 * @brief   iot_power_set_callback is used to set the callback to be called when the target is entering low power mode,
 *          and when target is coming out of low power mode. Callback must not use any HW resources
 *          when this callback is called with out first requesting for a performance mode.
 *
 * @param[in]   pxPowerHandle   handle to power driver returned in
 *                              iot_power_open
 * @param[in]   xCallback       The callback function to be called.
 * @param[in]   pvUserContext   The user context to be passed when callback is called.
 */
void iot_power_set_callback( IotPowerHandle_t const pxPowerHandle,
                                IotPowerCallback_t xCallback,
                                void * pvUserContext );

/**
 * @brief   iot_power_ioctl is used to set power configuration and
 *          power properties like Wakeup time thresholds.
 *          Supported IOCTL requests are defined in iot_PowerIoctlRequest_t
 *
 * @param[in]   pxPowerHandle  handle to power driver returned in
 *                          iot_power_open()
 * @param[in]   xRequest    configuration request of type IotPowerIoctlRequest_t
 * @param[in,out] pvBuffer  buffer holding Power set and get values.
 *
 * @return  returns IOT_POWER_SUCCESS on success or returns
 *          one of IOT_POWER_INVALID_VALUE on error
 *          or IOT_POWER_FUNCTION_NOT_SUPPORTED.
 */
int32_t iot_power_ioctl( IotPowerHandle_t const pxPowerHandle,
                       IotPowerIoctlRequest_t xRequest,
                       void * const pvBuffer );
/**
 * @brief   iot_power_get_mode is used to get the current power mode the target is in.
 *
 * @param[in]   pxPowerHandle   handle to power driver returned in
 *                              iot_power_open
 * @param[out]  xMode      current power mode.
 *
 * @return  returns IOT_POWER_SUCCESS on success or returns
 *          one of IOT_POWER_INVALID_VALUE, IOT_POWER_GET_FAILED on error.
 */
int32_t iot_power_get_mode( IotPowerHandle_t const pxPowerHandle,
                            IotPowerMode_t * xMode );

/**
 * @brief   iot_power_close is used to de-initialize power driver.
 *
 * @param[in]   pxPowerHandle   handle to power driver returned in
 *                              iot_power_open
 *
 * @return  returns IOT_POWER_SUCCESS on success or returns
 *          one of IOT_POWER_INVALID_VALUE on error.
 */
int32_t iot_power_close( IotPowerHandle_t const pxPowerHandle );

/**
 * @}
 */

#endif /* ifndef _IOT_HAL_POWER_H_ */
