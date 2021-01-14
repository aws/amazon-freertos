/*
 * Copyright (c) 2016-2019, Texas Instruments Incorporated
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
/*!*****************************************************************************
 *  @file       Timer.h
 *  @brief      Timer driver
 *
 *  @anchor ti_drivers_Timer_Overview
 *  # Overview
 *  The timer driver allows you to measure elapsed time with simple and
 *  portable APIs.This driver does not have PWM or capture functionalities.
 *  These functionalities are addressed in both the capture and PWM driver.
 *
 *  The timer driver also handles the general purpose timer resource allocation.
 *  For each driver that requires use of a general purpose timer, it calls
 *  Timer_open() to occupy the specified timer, and calls Timer_close() to
 *  release the occupied timer resource.
 *
 *  @anchor ti_drivers_Timer_Usage
 *  # Usage
 *
 *  This documentation provides a basic @ref ti_drivers_Timer_Synopsis
 *  "usage summary" and a set of @ref ti_drivers_Timer_Examples "examples"
 *  in the form of commented code fragments. Detailed descriptions of the
 *  APIs are provided in subsequent sections.
 *
 *  @anchor ti_drivers_Timer_Synopsis
 *  ## Synopsis
 *  @anchor ti_drivers_Timer_Synopsis_Code
 *  @code
 *  // Import Timer Driver definitions
 *  #include <ti/drivers/Timer.h>
 *
 *  Timer_Handle    handle;
 *  Timer_Params    params;
 *
 *  Timer_Params_init(&params);
 *  params.periodUnits = Timer_PERIOD_HZ;
 *  params.period = 1000;
 *  params.timerMode  = Timer_CONTINUOUS_CALLBACK;
 *  params.timerCallback = UserCallbackFunction;
 *
 *  handle = Timer_open(CONFIG_TIMER0, &params);
 *
 *  @code
 *  // Import Timer Driver definitions
 *  #include <ti/drivers/Timer.h>
 *
 *  Timer_Handle    handle;
 *  Timer_Params    params;
 *
 *  // Initialize Timer parameters
 *  Timer_Params_init(&params);
 *  params.periodUnits = Timer_PERIOD_HZ;
 *  params.period = 1000;
 *  params.timerMode  = Timer_CONTINUOUS_CALLBACK;
 *  params.timerCallback = UserCallbackFunction;
 *
 *  // Open Timer instance
 *  handle = Timer_open(CONFIG_TIMER0, &params);
 *
 *  sleep(10000);
 *
 *  Timer_stop(handle);
 *  @endcode
 *
 *  <hr>
 *  @anchor ti_drivers_Timer_Examples
 *  # Examples
 *
 *  @li @ref ti_drivers_Timer_Examples_open "Opening a Timer Instance"
 *  @li @ref ti_drivers_Timer_Examples_mode "Configuring Timer mode and period"
 *
 *  @anchor ti_drivers_Timer_Examples_open
 *  ## Opening a Timer instance
 *
 *  @code
 *  Timer_Handle    handle;
 *  Timer_Params    params;
 *
 *  Timer_Params_init(&params);
 *  handle = Timer_open(CONFIG_TIMER0, &params);
 *
 *  if (handle == NULL) {
 *      // Timer_open() failed
 *      while (1);
 *  }
 @endcode
 *
 *  @anchor ti_drivers_Timer_Examples_mode
 *  ##Configuring Timer mode and period
 *
 *  The following example code opens a timer in continuous callback mode. The
 *  period is set to 1000 Hz.
 *
 *  @code
 *  Timer_Handle    handle;
 *  Timer_Params    params;
 *
 *  Timer_Params_init(&params);
 *  params.periodUnits = Timer_PERIOD_HZ;
 *  params.period = 1000;
 *  params.timerMode  = Timer_CONTINUOUS_CALLBACK;
 *  params.timerCallback = UserCallbackFunction;
 *
 *  handle = Timer_open(CONFIG_TIMER0, &params);
 *
 *  if (handle == NULL) {
 *      // Timer_open() failed
 *      while (1);
 *  }
 *
 *  status = Timer_start(handle);
 *
 *  if (status == Timer_STATUS_ERROR) {
 *      //Timer_start() failed
 *      while (1);
 *  }
 *
 *  sleep(10000);
 *
 *  Timer_stop(handle);
 *  @endcode
 *
 *  ### Initializing the Timer Driver #
 *
 *  Timer_init() must be called before any other timer APIs. This function
 *  calls the device implementation's timer initialization function, for each
 *  element of Timer_config[].
 *
 *  <hr>
 *  @anchor ti_drivers_Timer_Configuration
 *  # Configuration
 *
 *  Refer to the @ref driver_configuration "Driver's Configuration" section
 *  for driver configuration information.
 *  <hr>
*******************************************************************************
 */

#ifndef ti_drivers_Timer__include
#define ti_drivers_Timer__include

#include <stdint.h>

#ifdef __cplusplus
extern "C"
{
#endif

/*!
 * Common Timer_control command code reservation offset.
 * Timer driver implementations should offset command codes with Timer_CMD_RESERVED
 * growing positively
 *
 * Example implementation specific command codes:
 * @code
 * #define TimerXYZ_CMD_COMMAND0      Timer_CMD_RESERVED + 0
 * #define TimerXYZ_CMD_COMMAND1      Timer_CMD_RESERVED + 1
 * @endcode
 */
#define Timer_CMD_RESERVED                (32)

/*!
 * Common Timer_control status code reservation offset.
 * Timer driver implementations should offset status codes with
 * Timer_STATUS_RESERVED growing negatively.
 *
 * Example implementation specific status codes:
 * @code
 * #define TimerXYZ_STATUS_ERROR0     Timer_STATUS_RESERVED - 0
 * #define TimerXYZ_STATUS_ERROR1     Timer_STATUS_RESERVED - 1
 * @endcode
 */
#define Timer_STATUS_RESERVED            (-32)

/*!
 * @brief   Successful status code.
 */
#define Timer_STATUS_SUCCESS               (0)

/*!
 * @brief   Generic error status code.
 */
#define Timer_STATUS_ERROR                (-1)

/*!
 * @brief   An error status code returned by Timer_control() for undefined
 *          command codes.
 *
 * Timer_control() returns Timer_STATUS_UNDEFINEDCMD if the control code is not
 * recognized by the driver implementation.
 */
#define Timer_STATUS_UNDEFINEDCMD         (-2)

/*!
 *  @brief      A handle that is returned from a Timer_open() call.
 */
typedef struct Timer_Config_ *Timer_Handle;

/*!
 *  @brief Timer mode settings
 *
 *  This enum defines the timer modes that may be specified in #Timer_Params.
 *
 *  The timer driver supports four modes of operation which may be specified in
 *  the Timer_Params. The device specific implementation may configure the timer
 *  peripheral as an up or down counter. In any case, Timer_getCount() will
 *  return a value characteristic of an up counter.
 */
typedef enum {
    Timer_ONESHOT_CALLBACK,       /*!< Is a non-blocking call. After Timer_start()
                                       is called, the calling thread will continue
                                       execution. When the timer interrupt is
                                       triggered, the specified callback function
                                       will be called. The timer will not generate
                                       another interrupt unless Timer_start() is
                                       called again. Calling Timer_stop() or
                                       Timer_close() after Timer_start() but,
                                       before the timer interrupt, will prevent
                                       the specified callback from ever being invoked.
                                        */
    Timer_ONESHOT_BLOCKING,       /*!< Is a blocking call. A semaphore is used to
                                       block the calling thread's execution until
                                       the timer generates an interrupt. If
                                       Timer_stop() is called, the calling thread
                                       will become unblocked immediately. The
                                       behavior of the timer in this mode is similar
                                       to a sleep function.
                                        */
    Timer_CONTINUOUS_CALLBACK,    /*!< Is a non-blocking call. After Timer_start()
                                       is called, the calling thread will continue
                                       execution. When the timer interrupt is
                                       triggered, the specified callback function
                                       will be called. The timer is automatically
                                       restarted and will continue to periodically
                                       generate interrupts until Timer_stop() is
                                       called.
                                        */
    Timer_FREE_RUNNING            /*!< Is a non-blocking call. After Timer_start()
                                       is called, the calling thread will continue
                                       execution. The timer will not generate an
                                       interrupt in this mode. The timer hardware
                                       will run until Timer_stop() is called.
                                        */
} Timer_Mode;

/*!
 *  @brief Timer period unit enum
 *
 *  This enum defines the units that may be specified for the period
 *  in #Timer_Params. This unit has no effect with Timer_getCounts.
 */
typedef enum {
    Timer_PERIOD_US,      /*!< Period specified in micro seconds. */
    Timer_PERIOD_HZ,      /*!< Period specified in hertz; interrupts per
                                  second. */
    Timer_PERIOD_COUNTS   /*!< Period specified in ticks or counts. Varies
                                  from board to board. */
} Timer_PeriodUnits;

/*!
 *  @brief  Timer callback function
 *
 *  User definable callback function prototype. The timer driver will call the
 *  defined function and pass in the timer driver's handle and the status code.
 *
 *  @param[in]  handle         Timer_Handle
 *  @param[in]  status         Status of timer interrupt
 */
typedef void (*Timer_CallBackFxn)(Timer_Handle handle, int_fast16_t status);

/*!
 *  @brief Timer Parameters
 *
 *  Timer parameters are used with the Timer_open() call. Default values for
 *  these parameters are set using Timer_Params_init().
 *
 */
typedef struct {
    /*! Mode to be used by the timer driver. */
    Timer_Mode           timerMode;

    /*! Units used to specify the period. */
    Timer_PeriodUnits    periodUnits;

    /*! Callback function called when timerMode is Timer_ONESHOT_CALLBACK or
         Timer_CONTINUOUS_CALLBACK. */
    Timer_CallBackFxn    timerCallback;

    /*! Period in units of periodUnits. */
    uint32_t             period;
} Timer_Params;

/*!
 *  @brief      A function pointer to a driver specific implementation of
 *              Timer_control().
 */
typedef int_fast16_t (*Timer_ControlFxn)(Timer_Handle handle,
    uint_fast16_t cmd, void *arg);

/*!
 *  @brief      A function pointer to a driver specific implementation of
 *              Timer_close().
 */
typedef void (*Timer_CloseFxn)(Timer_Handle handle);

/*!
 *  @brief      A function pointer to a driver specific implementation of
 *              Timer_getCount().
 */
typedef uint32_t (*Timer_GetCountFxn)(Timer_Handle handle);

/*!
 *  @brief      A function pointer to a driver specific implementation of
 *              Timer_init().
 */
typedef void (*Timer_InitFxn)(Timer_Handle handle);

/*!
 *  @brief      A function pointer to a driver specific implementation of
 *              Timer_open().
 */
typedef Timer_Handle (*Timer_OpenFxn)(Timer_Handle handle,
    Timer_Params *params);

/*!
 *  @brief      A function pointer to a driver specific implementation of
 *              Timer_setPeriod().
 */
typedef int32_t (*Timer_SetPeriodFxn)(Timer_Handle handle,
    Timer_PeriodUnits periodUnits, uint32_t period);

/*!
 *  @brief      A function pointer to a driver specific implementation of
 *              Timer_start().
 */
typedef int32_t (*Timer_StartFxn)(Timer_Handle handle);

/*!
 *  @brief      A function pointer to a driver specific implementation of
 *              Timer_stop().
 */
typedef void (*Timer_StopFxn)(Timer_Handle handle);

/*!
 *  @brief      The definition of a timer function table that contains the
 *              required set of functions to control a specific timer driver
 *              implementation.
 */
typedef struct {
    /*! Function to close the specified timer. */
    Timer_CloseFxn closeFxn;

    /*! Implementation-specific control function. */
    Timer_ControlFxn controlFxn;

    /*! Function to get the count of the specified timer. */
    Timer_GetCountFxn getCountFxn;

    /*! Function to initialize the driver instance. */
    Timer_InitFxn initFxn;

    /*! Function to open the specified timer. */
    Timer_OpenFxn openFxn;

    /*! Function to set the period of the specified timer. */
    Timer_SetPeriodFxn setPeriodFxn;

    /*! Function to start the specified timer. */
    Timer_StartFxn startFxn;

    /*! Function to stop the specified timer. */
    Timer_StopFxn stopFxn;
} Timer_FxnTable;

/*!
 *  @brief  Timer Global configuration
 *
 *  The Timer_Config structure contains a set of pointers used to characterize
 *  the timer driver implementation.
 *
 *  This structure needs to be defined before calling Timer_init() and it must
 *  not be changed thereafter.
 *
 *  @sa     Timer_init()
 */
typedef struct Timer_Config_ {
    /*! Pointer to a table of driver-specific implementations of timer APIs. */
    Timer_FxnTable const *fxnTablePtr;

    /*! Pointer to a driver-specific data object. */
    void                 *object;

    /*! Pointer to a driver-specific hardware attributes structure. */
    void           const *hwAttrs;
} Timer_Config;

/*!
 *  @brief  Function to close a timer. The corresponding timer
 *          becomes an available timer resource.
 *
 *  @pre    Timer_open() has been called.
 *
 *  @param[in]  handle  A Timer_Handle returned from Timer_open().
 *
 *  @sa     Timer_open()
 */
extern void Timer_close(Timer_Handle handle);

/*!
 *  @brief  Function performs device-specific features on a given
 *          timer.
 *
 *  @pre    Timer_open() has been called.
 *
 *  @param[in]  handle      A Timer_Handle returned from Timer_open().
 *
 *  @param[in]  cmd         A command value defined by the driver-specific
 *                      implementation.
 *
 *  @param[in]  arg         A pointer to an optional R/W (read/write) argument that
 *                      is accompanied with cmd.
 *
 *  @retval #Timer_STATUS_SUCCESS  The control call was successful.
 *  @retval #Timer_STATUS_ERROR    The control call failed
 *
 *  @sa     Timer_open()
 */
extern int_fast16_t Timer_control(Timer_Handle handle, uint_fast16_t cmd,
    void *arg);

/*!
 *  @brief  Function to get the current count of a timer. The value returned
 *          represents timer counts. The value returned is always
 *          characteristic of an up counter. This is true even if the timer
 *          peripheral is counting down. Some device-specific implementations
 *          may employ a prescaler in addition to this timer count.
 *
 *  @pre    Timer_open() has been called.
 *
 *  @param[in]  handle  A Timer_Handle returned from Timer_open().
 *
 *  @sa     Timer_open()
 *
 *  @return The current count of the timer in timer ticks.
 *
 */
extern uint32_t Timer_getCount(Timer_Handle handle);


/*!
 *  @brief  Function to initialize a timer. This function will go through
 *          all available hardware resources and mark them as "available".
 *
 *  @pre    The Timer_config structure must exist before this function is
 *          called, and must be persistent. This function must be called
 *          before any other timer driver APIs.
 *
 *  @sa     Timer_open()
 */
extern void Timer_init(void);

/*!
 *  @brief  Function to initialize a given timer peripheral specified by the
 *          index argument. The Timer_Params specifies the mode the timer will
 *          operate in. The accuracy of the desired period is limited by the
 *          the clock. For example, a 100 MHz clock will have a tick resolution
 *          of 10 nanoseconds. This function takes care of timer resource
 *          allocation. If the particular timer is available to use, the timer
 *          driver acquires it and returns a Timer_Handle.
 *
 *  @pre    Timer_init() has been called.
 *
 *  @param[in]  index         Logical peripheral number for the timer indexed into
 *                        the Timer_config table.
 *
 *  @param[in]  params        Pointer to an parameter block, if NULL it will use
 *                        default values.
 *
 *  @return A #Timer_Handle upon success, or NULL. NULL is returned if the
 *          desired period results in overflow or saturation of the timer, or
 *          if the timer resource is already in use.
 *
 *  @sa     Timer_init()
 *  @sa     Timer_close()
 */
extern Timer_Handle Timer_open(uint_least8_t index, Timer_Params *params);

/*!
 *  @brief  Function to set the period of a timer after it has been opened.
 *
 *  @pre    Timer_open() has been called. It is also recommended Timer_stop() has
 *          been called on an already running timer before calling this API as the
            period is updated asynchronously.
 *
 *  @param[in]  handle       A Timer_Handle returned from Timer_open().
 *
 *  @param[in]  periodUnits  #Timer_PeriodUnits of the desired period value.
 *
 *  @param[in]  period       Period value to set.
 *
 *  @retval #Timer_STATUS_SUCCESS  The setPeriod call was successful.
 *  @retval #Timer_STATUS_ERROR    The setPeriod call failed.
 *
 *  @sa     Timer_open()
 *  @sa     Timer_stop()
 */
extern int32_t Timer_setPeriod(Timer_Handle handle, Timer_PeriodUnits periodUnits, uint32_t period);

/*!
 *  @brief  Function to initialize the Timer_Params struct to its defaults.
 *
 *  @param[in]  params      A pointer to Timer_Params structure for
 *                      initialization.
 *
 *  Defaults values are:
 *      timerMode = Timer_ONESHOT_BLOCKING
 *      periodUnit = Timer_PERIOD_COUNTS
 *      timerCallback = NULL
 *      period = (uint16_t) ~0
 */
extern void Timer_Params_init(Timer_Params *params);

/*!
 *  @brief  Function to start a timer.
 *
 *  @pre    Timer_open() has been called.
 *
 *  @param[in]  handle  A Timer_Handle returned from Timer_open().
 *
 *  @retval #Timer_STATUS_SUCCESS  The start call was successful.
 *  @retval #Timer_STATUS_ERROR    The start call failed
 *
 *  @sa     Timer_stop()
 */
extern int32_t Timer_start(Timer_Handle handle);

/*!
 *  @brief  Function to stop a timer. If the timer is already stopped this
 *          function has no effect.
 *
 *  @pre    Timer_open() has been called.
 *
 *  @param[in]  handle  A Timer_Handle returned from Timer_open().
 *
 *  @sa     Timer_start()
 */
extern void Timer_stop(Timer_Handle handle);

#ifdef __cplusplus
}
#endif

#endif /* ti_drivers_Timer__include */
