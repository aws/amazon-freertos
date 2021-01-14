/*
 * Copyright (c) 2015-2019, Texas Instruments Incorporated
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
 *  @file       Watchdog.h
 *
 *  @brief      Watchdog driver interface
 *
 *  @anchor ti_drivers_Watchdog_Overview
 *  # Overview #
 *
 *  A watchdog timer can be used to generate a reset signal if a system has
 *  become unresponsive. The Watchdog driver simplifies configuring and
 *  starting the watchdog peripherals. The watchdog peripheral can be
 *  configured with resets either on or off and a user-specified timeout
 *  period.
 *
 *  When the watchdog peripheral is configured not to generate a reset, it
 *  can be used to cause a hardware interrupt at a programmable interval.
 *  The driver provides the ability to specify a user-provided callback
 *  function that is called when the watchdog causes an interrupt.
 *
 *  The Watchdog driver simplifies configuring and starting the Watchdog
 *  peripherals. The Watchdog can be set up to produce a reset signal after a
 *  timeout, or simply cause a hardware interrupt at a programmable interval.
 *  The driver provides the ability to specify a callback function that is
 *  called when the Watchdog causes an interrupt.
 *
 *  When resets are turned on, it is the user application's responsibility to
 *  call Watchdog_clear() in order to clear the Watchdog and prevent a reset.
 *  Watchdog_clear() can be called at any time.
 *
 *  @anchor ti_drivers_Watchdog_Usage
 *  # Usage #
 *
 *  This section will cover driver usage.
 *  @anchor ti_drivers_Watchdog_Synopsis
 *  ## Synopsis #
 *
 *  Open the driver with default settings:
 *  @code
 *  Watchdog_Handle watchdogHandle;
 *
 *  Watchdog_init();
 *  watchdogHandle = Watchdog_open(WATCHDOG_INDEX, NULL);
 *  if (watchdogHandle == NULL) {
 *      // Spin forever
 *      while(1);
 *  }
 *  @endcode
 *
 *  The Watchdog driver must be initialized by calling Watchdog_init(),
 *  before any other Watchdog APIs can be called.
 *  Once the watchdog is initialized, a Watchdog object can be created
 *  through the following steps:
 *  -   Create and initialize the #Watchdog_Params structure.
 *  -   Assign desired values to parameters.
 *  -   Call Watchdog_open().
 *  -   Save the Watchdog_Handle returned by Watchdog_open(). This will be
 *  used to interact with the Watchdog object just created.
 *
 *  To have a user-defined function run at the hardware interrupt caused by
 *  a watchdog timer timeout, define a function of the following type:
 *  @code
 *    typedef void (*Watchdog_Callback)(uintptr_t);
 *  @endcode
 *  Then pass the function to Watchdog_open() through the #Watchdog_Params
 *  structure.
 *
 *  An example of the Watchdog creation process that uses a callback
 *  function:
 *  @anchor ti_drivers_Watchdog_example_callback
 *  @code
 *  void UserCallbackFxn(Watchdog_Handle handle)
 *  {
 *      printf("Watchdog timer triggered!\n");
 *      releaseResources();
 *  }
 *
 *  ...
 *
 *  Watchdog_Params params;
 *  Watchdog_Handle watchdogHandle;
 *
 *  Watchdog_init();
 *
 *  Watchdog_Params_init(&params);
 *  params.resetMode = Watchdog_RESET_ON;
 *  params.callbackFxn = (Watchdog_Callback) UserCallbackFxn;
 *
 *  watchdogHandle = Watchdog_open(CONFIG_WATCHDOG0, &params);
 *  if (watchdogHandle == NULL) {
 *     // Error opening Watchdog
 *     while (1);
 *  }
 *
 *  @endcode
 *
 *  If no #Watchdog_Params structure is passed to Watchdog_open(), the
 *  default values are used. By default, the Watchdog driver has resets
 *  turned on, no callback function specified, and stalls the timer at
 *  breakpoints during debugging.
 *
 *  Options for the resetMode parameter are #Watchdog_RESET_ON and
 *  #Watchdog_RESET_OFF. The latter allows the watchdog to be used like
 *  another timer interrupt. When resetMode is #Watchdog_RESET_ON, it is up
 *  to the application to call Watchdog_clear() to clear the Watchdog
 *  interrupt flag to prevent a reset. Watchdog_clear() can be called at
 *  any time.
 *
 *  @anchor ti_drivers_Watchdog_Examples
 *  # Examples
 *  - @ref ti_drivers_Watchdog_Synopsis "Default Example"
 *  - @ref ti_drivers_Watchdog_example_callback "Callback Function before watchdog reset"
 *
 *  @anchor ti_drivers_Watchdog_Configuration
 *  # Configuration
 *
 *  Refer to the @ref driver_configuration "Driver's Configuration" section
 *  for more information.
 *
 *******************************************************************************
 */

#ifndef ti_drivers_Watchdog__include
#define ti_drivers_Watchdog__include

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 *  @defgroup Watchdog_CONTROL Watchdog_control command and status codes
 *  These Watchdog macros are reservations for Watchdog.h
 *  @{
 */

/*!
 * Common Watchdog_control command code reservation offset.
 * Watchdog driver implementations should offset command codes with
 * Watchdog_CMD_RESERVED growing positively
 *
 * Example implementation specific command codes:
 * @code
 * #define WatchdogXYZ_CMD_COMMAND0     Watchdog_CMD_RESERVED + 0
 * #define WatchdogXYZ_CMD_COMMAND1     Watchdog_CMD_RESERVED + 1
 * @endcode
 */
#define Watchdog_CMD_RESERVED            (32)

/*!
 * Common Watchdog_control status code reservation offset.
 * Watchdog driver implementations should offset status codes with
 * Watchdog_STATUS_RESERVED growing negatively.
 *
 * Example implementation specific status codes:
 * @code
 * #define WatchdogXYZ_STATUS_ERROR0    Watchdog_STATUS_RESERVED - 0
 * #define WatchdogXYZ_STATUS_ERROR1    Watchdog_STATUS_RESERVED - 1
 * #define WatchdogXYZ_STATUS_ERROR2    Watchdog_STATUS_RESERVED - 2
 * @endcode
 */
#define Watchdog_STATUS_RESERVED        (-32)

/**
 *  @defgroup Watchdog_STATUS Status Codes
 *  Watchdog_STATUS_* macros are general status codes returned by Watchdog_control()
 *  @{
 *  @ingroup Watchdog_CONTROL
 */

/*!
 * @brief   Successful status code returned by Watchdog_control().
 *
 * Watchdog_control() returns Watchdog_STATUS_SUCCESS if the control code was
 * executed successfully.
 */
#define Watchdog_STATUS_SUCCESS        (0)

/*!
 * @brief   Generic error status code returned by Watchdog_control().
 *
 * Watchdog_control() returns Watchdog_STATUS_ERROR if the control code was not
 * executed successfully.
 */
#define Watchdog_STATUS_ERROR          (-1)

/*!
 * @brief   An error status code returned by Watchdog_control() for undefined
 * command codes.
 *
 * Watchdog_control() returns Watchdog_STATUS_UNDEFINEDCMD if the control code
 * is not recognized by the driver implementation.
 */
#define Watchdog_STATUS_UNDEFINEDCMD   (-2)

/*!
 * @brief   An error status code returned by Watchdog_setReload() for drivers
 * which do not support the aforementioned API.
 *
 * Watchdog_setReload() returns Watchdog_STATUS_UNSUPPORTED if the driver
 * implementation does not support the aforementioned API.
 */
#define Watchdog_STATUS_UNSUPPORTED   (-3)
/** @}*/

/**
 *  @defgroup Watchdog_CMD Command Codes
 *  Watchdog_CMD_* macros are general command codes for Watchdog_control(). Not all Watchdog
 *  driver implementations support these command codes.
 *  @{
 *  @ingroup Watchdog_CONTROL
 */

/* Add Watchdog_CMD_<commands> here */

/** @}*/

/** @}*/

/*!
*  @brief      Watchdog Handle
*/
typedef struct Watchdog_Config_ *Watchdog_Handle;

/*!
 *  @brief      Watchdog debug stall settings
 *
 *  This enumeration defines the debug stall modes for the Watchdog. On some
 *  targets, the Watchdog timer will continue to count down while a debugging
 *  session is halted. To avoid unwanted resets, the Watchdog can be set to
 *  stall while the processor is stopped by the debugger.
 */
typedef enum {
    Watchdog_DEBUG_STALL_ON, /*!< Watchdog will be stalled at breakpoints */
    Watchdog_DEBUG_STALL_OFF /*!< Watchdog will keep running at breakpoints */
} Watchdog_DebugMode;

/*!
 *  @brief      Watchdog reset mode settings
 *
 *  This enumeration defines the reset modes for the Watchdog. The Watchdog can
 *  be configured to either generate a reset upon timeout or simply produce a
 *  periodic interrupt.
 */
typedef enum {
    Watchdog_RESET_OFF, /*!< Timeouts generate interrupts only */
    Watchdog_RESET_ON   /*!< Generates reset after timeout */
} Watchdog_ResetMode;

/*!
 *  @brief      Watchdog callback pointer
 *
 *  This is the typedef for the function pointer that will allow a callback
 *  function to be specified in the #Watchdog_Params structure. The function
 *  will take a #Watchdog_Handle of the Watchdog causing the interrupt (cast as
 *  a uintptr_t) as an argument.
 */
typedef void (*Watchdog_Callback)(uintptr_t handle);

/*!
 *  @brief      Watchdog Parameters
 *
 *  Watchdog parameters are used to with the Watchdog_open() call. Default
 *  values for these parameters are set using Watchdog_Params_init().
 *
 *  @sa         Watchdog_Params_init()
 */
typedef struct {
    Watchdog_Callback   callbackFxn;    /*!< Pointer to callback. Not supported
                                             on all targets. */
    Watchdog_ResetMode  resetMode;      /*!< Mode to enable resets.
                                             Not supported on all targets. */
    Watchdog_DebugMode  debugStallMode; /*!< Mode to stall WDT at breakpoints.
                                             Not supported on all targets. */
    void               *custom;         /*!< Custom argument used by driver
                                             implementation */
} Watchdog_Params;

/*!
 *  @brief      A function pointer to a driver specific implementation of
 *              Watchdog_clear().
 */
typedef void (*Watchdog_ClearFxn)       (Watchdog_Handle handle);

/*!
 *  @brief      A function pointer to a driver specific implementation of
 *              Watchdog_close().
 */
typedef void (*Watchdog_CloseFxn)       (Watchdog_Handle handle);

/*!
 *  @brief      A function pointer to a driver specific implementation of
 *              Watchdog_control().
 */
typedef int_fast16_t (*Watchdog_ControlFxn) (Watchdog_Handle handle,
                                             uint_fast16_t cmd,
                                             void *arg);

/*!
 *  @brief      A function pointer to a driver specific implementation of
 *              Watchdog_init().
 */
typedef void (*Watchdog_InitFxn)        (Watchdog_Handle handle);

/*!
 *  @brief      A function pointer to a driver specific implementation of
 *              Watchdog_open().
 */
typedef Watchdog_Handle (*Watchdog_OpenFxn)  (Watchdog_Handle handle,
                                              Watchdog_Params *params);

/*!
 *  @brief      A function pointer to a driver specific implementation of
 *              Watchdog_setReload().
 */
typedef int_fast16_t (*Watchdog_SetReloadFxn)(Watchdog_Handle handle,
    uint32_t ticks);

/*!
 *  @brief      A function pointer to a driver specific implementation of
 *              Watchdog_ConvertMsToTicksFxn().
 */
typedef uint32_t (*Watchdog_ConvertMsToTicksFxn)   (Watchdog_Handle handle,
                                                    uint32_t milliseconds);

/*!
 *  @brief      The definition of a Watchdog function table that contains the
 *              required set of functions to control a specific Watchdog driver
 *              implementation.
 */
typedef struct {
    Watchdog_ClearFxn             watchdogClear;
    Watchdog_CloseFxn             watchdogClose;
    Watchdog_ControlFxn           watchdogControl;
    Watchdog_InitFxn              watchdogInit;
    Watchdog_OpenFxn              watchdogOpen;
    Watchdog_SetReloadFxn         watchdogSetReload;
    Watchdog_ConvertMsToTicksFxn  watchdogConvertMsToTicks;
} Watchdog_FxnTable;

/*!
 *  @brief      Watchdog Global configuration
 *
 *  The Watchdog_Config structure contains a set of pointers used to
 *  characterize the Watchdog driver implementation.
 *
 *  This structure needs to be defined before calling Watchdog_init() and
 *  it must not be changed thereafter.
 *
 *  @sa     Watchdog_init()
 */
typedef struct Watchdog_Config_ {
    /*!
     * Pointer to a table of driver-specific implementations of Watchdog APIs
     */
    Watchdog_FxnTable const *fxnTablePtr;

    /*! Pointer to a driver specific data object */
    void                    *object;

    /*! Pointer to a driver specific hardware attributes structure */
    void              const *hwAttrs;
} Watchdog_Config;

/*!
 *  @brief      Clears the Watchdog
 *
 *  Clears the Watchdog to to prevent a reset signal from being generated if the
 *  module is in #Watchdog_RESET_ON reset mode.
 *
 *  @param  handle      A #Watchdog_Handle
 */
extern void Watchdog_clear(Watchdog_Handle handle);

/*!
 *  @brief  Function to close a Watchdog peripheral specified by the Watchdog
 *          handle.It stops (holds) the Watchdog counting on applicable
 *          platforms.
 *
 *  @pre    Watchdog_open() has to be called first.
 *
 *  @param  handle      A #Watchdog_Handle returned from Watchdog_open()
 *
 *  @sa     Watchdog_open()
 */
extern void Watchdog_close(Watchdog_Handle handle);

/*!
 *  @brief  Function performs implementation specific features on a given
 *          #Watchdog_Handle.
 *
 *  Commands for Watchdog_control can originate from Watchdog.h or from implementation
 *  specific Watchdog*.h files.
 *  While commands from Watchdog.h are API portable across driver implementations,
 *  not all implementations may support all these commands.
 *  Conversely, commands from driver implementation specific Watchdog*.h files add
 *  unique driver capabilities but are not API portable across all Watchdog driver
 *  implementations.
 *
 *  Commands supported by Watchdog.h follow a Watchdog_CMD_\<cmd\> naming
 *  convention.<br>
 *  Commands supported by Watchdog*.h follow a Watchdog*_CMD_\<cmd\> naming
 *  convention.<br>
 *  Each control command defines @b arg differently. The types of @b arg are
 *  documented with each command.
 *
 *  See @ref Watchdog_CMD "Watchdog_control command codes" for command codes.
 *
 *  See @ref Watchdog_STATUS "Watchdog_control return status codes" for status codes.
 *
 *  @pre    Watchdog_open() has to be called first.
 *
 *  @param  handle      A #Watchdog_Handle returned from Watchdog_open()
 *
 *  @param  cmd         Watchdog.h or Watchdog*.h commands.
 *
 *  @param  arg         An optional R/W (read/write) command argument
 *                      accompanied with cmd
 *
 *  @return Implementation specific return codes. Negative values indicate
 *          unsuccessful operations.
 *
 *  @sa     Watchdog_open()
 */
extern int_fast16_t Watchdog_control(Watchdog_Handle handle,
                                     uint_fast16_t cmd,
                                     void *arg);

/*!
 *  @brief      Initializes the Watchdog module
 *
 *  The application-provided Watchdog_config must be present before the
 *  Watchdog_init() function is called. The Watchdog_config must be persistent
 *  and not changed after Watchdog_init is called. This function must be called
 *  before any of the other Watchdog driver APIs.
 */
extern void Watchdog_init(void);

/*!
 *  @brief      Opens a Watchdog
 *
 *  Opens a Watchdog object with the index and parameters specified, and
 *  returns a #Watchdog_Handle.
 *
 *  @param  index         Logical peripheral number for the Watchdog indexed
 *                        into the Watchdog_config table
 *
 *  @param  params        Pointer to a #Watchdog_Params, if NULL it will use
 *                        default values. All the fields in this structure are
 *                        RO (read-only).
 *
 *  @return A #Watchdog_Handle on success or a NULL on an error or if it has
 *          been opened already.
 *
 *  @sa     Watchdog_init()
 *  @sa     Watchdog_close()
 */
extern Watchdog_Handle Watchdog_open(uint_least8_t index, Watchdog_Params *params);

/*!
 *  @brief  Function to initialize the #Watchdog_Params structure to its defaults
 *
 *  @param  params      An pointer to #Watchdog_Params structure for
 *                      initialization
 *
 *  Default parameters:
 *      callbackFxn = NULL
 *      resetMode = #Watchdog_RESET_ON
 *      debugStallMode = #Watchdog_DEBUG_STALL_ON
 */
extern void Watchdog_Params_init(Watchdog_Params *params);

/*!
 *  @brief      Sets the Watchdog reload value
 *
 *  Sets the value from which the Watchdog will countdown after it reaches
 *  zero. This is how the reload value can be changed after the Watchdog has
 *  already been opened. The new reload value will be loaded into the Watchdog
 *  timer when this function is called. Watchdog_setReload is not reentrant.
 *  For CC13XX/CC26XX, if the parameter 'ticks' is set to zero (0), a Watchdog
 *  interrupt is immediately generated.
 *
 *  This API is not applicable for all platforms. See the page for your
 *  specific driver implementation for details.
 *
 *  @param      handle      A #Watchdog_Handle
 *
 *  @param      ticks       Value to be loaded into Watchdog timer
 *                          Unit is in Watchdog clock ticks
 *
 *  @return #Watchdog_STATUS_SUCCESS on success, #Watchdog_STATUS_UNSUPPORTED
 *          if driver does not support this API.
 */
extern int_fast16_t Watchdog_setReload(Watchdog_Handle handle, uint32_t ticks);

/*!
 *  @brief      Converts milliseconds to Watchdog clock ticks
 *
 *  Converts the input value into number of Watchdog clock ticks as close as
 *  possible.  If the converted value exceeds 32 bits, a zero (0) will be
 *  returned to indicate overflow.  The converted value can be used as the
 *  function parameter 'ticks' in Watchdog_setReload().
 *
 *  This API is not applicable for all platforms. See the page for your
 *  specific driver implementation for details.
 *
 *  @param      handle         A #Watchdog_Handle
 *
 *  @param      milliseconds   Value to be converted
 *
 *  @return Converted value in number of Watchdog clock ticks
 *          A value of zero (0) means the converted value exceeds 32 bits
 *          or that the operation is not supported for the specific device.
 *
 *  @sa     Watchdog_setReload()
 */
extern uint32_t Watchdog_convertMsToTicks(Watchdog_Handle handle,
    uint32_t milliseconds);

#ifdef __cplusplus
}
#endif

#endif /* ti_drivers_Watchdog__include */
