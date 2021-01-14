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
 *  @file       Capture.h
 *  @brief      Capture Driver
 *
 *  @anchor ti_drivers_Capture_Overview
 *  # Overview
 *
 *  The capture driver is used to detect and time edge triggered events on a
 *  GPIO pin.
 *
 *  The capture driver serves as the main interface for a typical RTOS
 *  application. Its purpose is to redirect the capture APIs to device specific
 *  implementations which are specified using a pointer to a #Capture_FxnTable.
 *  The device specific implementations are responsible for creating all the
 *  RTOS specific primitives to allow for thead-safe operation. The capture
 *  driver utilizes the general purpose timer hardware.
 *
 *  The capture driver internally handles the general purpose timer resource
 *  allocation. For each capture driver instance, Capture_open() occupies the
 *  specified timer, and Capture_close() releases the occupied timer resource.
 *
 *  <hr>
 *  @anchor ti_drivers_Capture_Usage
 *  # Usage
 *
 *  This documentation provides a basic @ref ti_drivers_Capture_Synopsis
 *  "usage summary" and a set of @ref ti_drivers_Capture_Examples "examples"
 *  in the form of commented code fragments. Detailed descriptions of the
 *  APIs are provided in subsequent sections.
 *
 *  @anchor ti_drivers_Capture_Synopsis
 *  ## Synopsis
 *  @anchor ti_drivers_Capture_Synopsis_Code
 *  @code
 *  // Import Capture Driver definitions
 *  #include <ti/drivers/Capture.h>
 *
 *  Capture_Handle    handle;
 *  Capture_Params    params;
 *
 *  Capture_Params_init(&params);
 *  params.mode  = Capture_FALLING_EDGE;
 *  params.callbackFxn = someCaptureCallbackFunction;
 *  params.periodUnit = Capture_PERIOD_US;
 *
 *  handle = Capture_open(CONFIG_CAPTURE0, &params);
 *
 *  if (handle == NULL) {
 *      //Capture_open() failed
 *      while(1);
 *  }
 *
 *  status = Capture_start(handle);
 *
 *  if (status == Capture_STATUS_ERROR) {
 *      //Capture_start() failed
 *      while(1);
 *  }
 *
 *  sleep(10000);
 *
 *  Capture_stop(handle);
 *  @endcode
 *
 *  <hr>
 *  @anchor ti_drivers_Capture_Examples
 *  # Examples
 *
 *  @li @ref ti_drivers_Capture_Examples_open "Opening a Capture instance"
 *
 *  @anchor ti_drivers_Capture_Examples_open
 *  ## Opening a Capture instance
 *
 *  @code
 *  Capture_Handle  handle;
 *  Capture_Params  params;
 *
 *  Capture_Params_init(&params);
 *  params.mode  = Capture_FALLING_EDGE;
 *  params.callbackFxn = someCaptureCallbackFunction;
 *  params.periodUnit = Capture_PERIOD_US;
 *
 *  handle = Capture_open(CONFIG_CAPTURE0, &params);
 *
 *  if (handle == NULL) {
 *      //Capture_open() failed
 *      while(1);
 *  }
 *  @endcode
 *
 *  <hr>
 *  @anchor ti_drivers_Capture_Configuration
 *  # Configuration
 *
 *  Refer to the @ref driver_configuration "Driver's Configuration" section
 *  for driver configuration information.
 *  <hr>
 *******************************************************************************
 */

#ifndef ti_drivers_Capture__include
#define ti_drivers_Capture__include

#include <stdint.h>

#ifdef __cplusplus
extern "C"
{
#endif

/*!
 * Common Capture_control command code reservation offset.
 * Capture driver implementations should offset command codes with
 * Capture_CMD_RESERVED growing positively.
 *
 * Example implementation specific command codes:
 * @code
 * #define CaptureXYZ_CMD_COMMAND0      Capture_CMD_RESERVED + 0
 * #define CaptureXYZ_CMD_COMMAND1      Capture_CMD_RESERVED + 1
 * @endcode
 */
#define Capture_CMD_RESERVED             (32)

/*!
 * Common Capture_control status code reservation offset.
 * Capture driver implementations should offset status codes with
 * Capture_STATUS_RESERVED growing negatively.
 *
 * Example implementation specific status codes:
 * @code
 * #define CaptureXYZ_STATUS_ERROR0     Capture_STATUS_RESERVED - 0
 * #define CaptureXYZ_STATUS_ERROR1     Capture_STATUS_RESERVED - 1
 * @endcode
 */
#define Capture_STATUS_RESERVED         (-32)

/*!
 * @brief   Successful status code.
 */
#define Capture_STATUS_SUCCESS          (0)

/*!
 * @brief   Generic error status code.
 */
#define Capture_STATUS_ERROR            (-1)

/*!
 * @brief   An error status code returned by Capture_control() for undefined
 * command codes.
 *
 * Capture_control() returns Capture_STATUS_UNDEFINEDCMD if the control code is
 * not recognized by the driver implementation.
 */
#define Capture_STATUS_UNDEFINEDCMD     (-2)

/*!
 *  @brief      A handle that is returned from a Capture_open() call.
 */
typedef struct Capture_Config_ *Capture_Handle;


/*!
 *  @brief Capture mode settings
 *
 *  This enum defines the capture modes that may be specified in
 *  #Capture_Params.
 *
 *  Some modes are not available on all devices. Check the device specific
 *  implementations to see which modes are allowed.
 */
typedef enum {
    Capture_RISING_EDGE,     /*!< Capture is triggered on rising edges. */
    Capture_FALLING_EDGE,    /*!< Capture is triggered on falling edges. */
    Capture_ANY_EDGE,         /*!< Capture is triggered on both rising and
                                  falling edges. */
    Capture_RISING_EDGE_FALLING_EDGE,  /*!< Start capture is triggered on rising
                                            edge and stop capture is triggered
                                            on falling edge */
    Capture_FALLING_EDGE_RISING_EDGE,  /*!< Start capture is triggered on
                                            falling edge and stop capture is
                                            triggered on rising edge */
} Capture_Mode;

/*!
 *  @brief Capture period unit enum
 *
 *  This enum defines the units that may be specified for the period
 *  in #Capture_Params.
 */
typedef enum {
    Capture_PERIOD_US,       /*!< Period specified in micro seconds. */
    Capture_PERIOD_HZ,       /*!< Period specified in hertz; interrupts per
                                  second. */
    Capture_PERIOD_COUNTS,    /*!< Period specified in timer ticks. Varies
                                  by board. */
    Capture_PERIOD_NS,       /*!< Period specified in nano seconds. */
} Capture_PeriodUnits;


/*!
 *  @brief  Capture callback function
 *
 *  User definable callback function prototype. The capture driver will call
 *  the defined function and pass in the capture driver's handle and the
 *  pointer to the user-specified the argument.
 *
 *  @param[in]  handle         Capture_Handle
 *
 *  @param[in]  interval       Interval of two triggering edges in
 *                             #Capture_PeriodUnits
 *  @param[in]  status         Status of Capture interrupt
 */
typedef void (*Capture_CallBackFxn)(Capture_Handle handle, uint32_t interval,
                                    int_fast16_t status);

/*!
 *  @brief Capture Parameters
 *
 *  Capture parameters are used by the Capture_open() call. Default values for
 *  these parameters are set using Capture_Params_init().
 *
 */
typedef struct {
    /*! Mode to be used by the timer driver. */
    Capture_Mode           mode;

    /*! Callback function called when a trigger event occurs. */
    Capture_CallBackFxn    callbackFxn;

    /*! Units used to specify the interval. */
    Capture_PeriodUnits    periodUnit;
} Capture_Params;

/*!
 *  @brief      A function pointer to a driver specific implementation of
 *              Capture_close().
 */
typedef void (*Capture_CloseFxn)(Capture_Handle handle);

/*!
 *  @brief      A function pointer to a driver specific implementation of
 *              Capture_control().
 */
typedef int_fast16_t (*Capture_ControlFxn)(Capture_Handle handle,
    uint_fast16_t cmd, void *arg);

/*!
 *  @brief      A function pointer to a driver specific implementation of
 *              Capture_init().
 */
typedef void (*Capture_InitFxn)(Capture_Handle handle);

/*!
 *  @brief      A function pointer to a driver specific implementation of
 *              Capture_open().
 */
typedef Capture_Handle (*Capture_OpenFxn)(Capture_Handle handle,
    Capture_Params *params);

/*!
 *  @brief      A function pointer to a driver specific implementation of
 *              Capture_start().
 */
typedef int32_t (*Capture_StartFxn)(Capture_Handle handle);

/*!
 *  @brief      A function pointer to a driver specific implementation of
 *              Capture_stop().
 */
typedef void (*Capture_StopFxn)(Capture_Handle handle);

/*!
 *  @brief      The definition of a capture function table that contains the
 *              required set of functions to control a specific capture driver
 *              implementation.
 */
typedef struct {
    /*! Function to close the specified peripheral. */
    Capture_CloseFxn closeFxn;

    /*! Function to implementation specific control function. */
    Capture_ControlFxn controlFxn;

    /*! Function to initialize the given data object. */
    Capture_InitFxn initFxn;

    /*! Function to open the specified peripheral. */
    Capture_OpenFxn openFxn;

    /*! Function to start the specified peripheral. */
    Capture_StartFxn startFxn;

    /*! Function to stop the specified peripheral. */
    Capture_StopFxn stopFxn;
} Capture_FxnTable;

/*!
 *  @brief  Capture Global configuration
 *
 *  The Capture_Config structure contains a set of pointers used to
 *  characterize the capture driver implementation.
 *
 *  This structure needs to be defined before calling Capture_init() and it
 *  must not be changed thereafter.
 *
 *  @sa     Capture_init()
 */
typedef struct Capture_Config_ {
    /*! Pointer to a table of driver-specific implementations of capture
        APIs. */
    Capture_FxnTable const *fxnTablePtr;

    /*! Pointer to a driver specific data object. */
    void                   *object;

    /*! Pointer to a driver specific hardware attributes structure. */
    void             const *hwAttrs;
} Capture_Config;

/*!
 *  @brief  Function to close a capture driver instance. The corresponding
 *          timer peripheral to Capture_handle becomes an available resource.
 *
 *  @pre    Capture_open() has been called.
 *
 *  @param[in]  handle  A Capture_Handle returned from Capture_open().
 *
 *  @sa     Capture_open()
 */
extern void Capture_close(Capture_Handle handle);

/*!
 *  @brief  Function performs implementation specific features on a given
 *          Capture_Handle.
 *
 *  @pre    Capture_open() has been called.
 *
 *  @param[in]  handle      A Capture_Handle returned from Capture_open().
 *
 *  @param[in]  cmd         A command value defined by the driver specific
 *                      implementation.
 *
 *  @param[in]  arg         A pointer to an optional R/W (read/write) argument that
 *                      is accompanied with cmd.
 *
 *  @retval #Capture_STATUS_SUCCESS  The control call was successful.
 *  @retval #Capture_STATUS_ERROR    The control call failed.
 *
 *  @sa     Capture_open()
 */
extern int_fast16_t Capture_control(Capture_Handle handle, uint_fast16_t cmd,
    void *arg);

/*!
 *  @brief  Function to initialize the capture driver. This function will go
 *          through all available hardware resources and mark them as
 *          "available".
 *
 *  @pre    The Capture_config structure must exist and be persistent before
 *          this function can be called. This function must also be called
 *          before any other capture driver APIs.
 *
 *  @sa     Capture_open()
 */
extern void Capture_init(void);

/*!
 *  @brief  Function to open a given capture instance specified by the
 *          index argument. The Capture_Params specifies which mode the capture
 *          instance will operate. This function takes care of capture resource
 *          allocation. If the particular timer hardware is available to use,
 *          the capture driver acquires it and returns a Capture_Handle.
 *
 *  @pre    Capture_init() has been called.
 *
 *  @param[in]  index         Logical instance number for the capture indexed into
 *                        the Capture_config table.
 *
 *  @param[in]  params        Pointer to a parameter block. Cannot be NULL.
 *
 *  @return A #Capture_Handle on success, or NULL if the timer peripheral is
 *          already in use.
 *
 *  @sa     Capture_init()
 *  @sa     Capture_close()
 */
extern Capture_Handle Capture_open(uint_least8_t index, Capture_Params *params);

/*!
 *  @brief  Function to initialize the Capture_Params struct to its defaults.
 *
 *  @param[in]  params      An pointer to Capture_Params structure for
 *                      initialization.
 *
 *  Defaults values are:
 *      callbackFxn = NULL
 *      mode = #Capture_RISING_EDGE
 *      periodUnit = #Capture_PERIOD_COUNTS
 */
extern void Capture_Params_init(Capture_Params *params);

/*!
 *  @brief  Function to start the capture instance.
 *
 *  @pre    Capture_open() has been called.
 *
 *  @param[in]  handle  A Capture_Handle returned from Capture_open().
 *
 *  @retval #Capture_STATUS_SUCCESS  The start call was successful.
 *  @retval #Capture_STATUS_ERROR    The start call failed.
 *
 *  @sa     Capture_stop().
 *
 */
extern int32_t Capture_start(Capture_Handle handle);

/*!
 *  @brief  Function to stop a capture instance. If the capture instance is
 *          already stopped, this function has no effect.
 *
 *  @pre    Capture_open() has been called.
 *
 *  @param[in]  handle  A Capture_Handle returned from Capture_open().
 *
 *  @sa     Capture_start()
 */
extern void Capture_stop(Capture_Handle handle);

#ifdef __cplusplus
}
#endif

#endif /* ti_drivers_Capture__include */
