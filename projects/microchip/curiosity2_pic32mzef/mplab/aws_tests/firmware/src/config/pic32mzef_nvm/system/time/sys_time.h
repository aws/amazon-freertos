/*******************************************************************************
  Time System Service Library Interface Header File

  Company
    Microchip Technology Inc.

  File Name
    sys_time.h

  Summary
    Time system service library interface.

  Description
    This file defines the interface to the Time system service library.  This
    library provides a free-running shared software timer/counter, (32--bit)
    giving the entire system a common time base and providing real-time
    capabilities such as delays and callbacks.  It also (optionally) provides
    the ability to create individual software timers (32--bit), under control
    of the client, with counter and callback capabilities.

  Remarks:
    This interface will be extended in the future to utilize Real Time Clock
    and Calendar (RTCC) support to provide time of day and date capabilities.
*******************************************************************************/

// DOM-IGNORE-BEGIN
/*******************************************************************************
* Copyright (C) 2018 Microchip Technology Inc. and its subsidiaries.
*
* Subject to your compliance with these terms, you may use Microchip software
* and any derivatives exclusively with Microchip products. It is your
* responsibility to comply with third party license terms applicable to your
* use of third party software (including open source software) that may
* accompany Microchip software.
*
* THIS SOFTWARE IS SUPPLIED BY MICROCHIP "AS IS". NO WARRANTIES, WHETHER
* EXPRESS, IMPLIED OR STATUTORY, APPLY TO THIS SOFTWARE, INCLUDING ANY IMPLIED
* WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY, AND FITNESS FOR A
* PARTICULAR PURPOSE.
*
* IN NO EVENT WILL MICROCHIP BE LIABLE FOR ANY INDIRECT, SPECIAL, PUNITIVE,
* INCIDENTAL OR CONSEQUENTIAL LOSS, DAMAGE, COST OR EXPENSE OF ANY KIND
* WHATSOEVER RELATED TO THE SOFTWARE, HOWEVER CAUSED, EVEN IF MICROCHIP HAS
* BEEN ADVISED OF THE POSSIBILITY OR THE DAMAGES ARE FORESEEABLE. TO THE
* FULLEST EXTENT ALLOWED BY LAW, MICROCHIP'S TOTAL LIABILITY ON ALL CLAIMS IN
* ANY WAY RELATED TO THIS SOFTWARE WILL NOT EXCEED THE AMOUNT OF FEES, IF ANY,
* THAT YOU HAVE PAID DIRECTLY TO MICROCHIP FOR THIS SOFTWARE.
*******************************************************************************/
// DOM-IGNORE-END

#ifndef SYS_TIME_H    // Guards against multiple inclusion
#define SYS_TIME_H


// *****************************************************************************
// *****************************************************************************
// Section: Included Files
// *****************************************************************************
// *****************************************************************************
/*  This section lists the other files that are included in this file.
*/

#include <stdint.h>
#include <stdbool.h>
#include "system/system.h"
#include "sys_time_definitions.h"

// DOM-IGNORE-BEGIN
#ifdef __cplusplus  // Provide C++ Compatibility

    extern "C" {

#endif
// DOM-IGNORE-END

// *****************************************************************************
// *****************************************************************************
// Section: Data Types
// *****************************************************************************
// *****************************************************************************

// *****************************************************************************
/* TIME System Service Initialization Data

  Summary:
    Defines the data required to initialize the TIME system service

  Description:
    This data type defines the data required to initialize the
    TIME system service.

  Remarks:
    This structure is implementation specific. It is fully defined in
    sys_time_definitions.h.
*/

typedef struct _SYS_TIME_INIT SYS_TIME_INIT;

// *****************************************************************************
/* System Time Result

  Summary:
    Result of a time service client interface operation.

  Description:
    Identifies the result of certain time service operations.
*/

typedef enum
{
    // Operation completed with success.
    SYS_TIME_SUCCESS,

    // Invalid handle or operation failed.
    SYS_TIME_ERROR

} SYS_TIME_RESULT;


// *****************************************************************************
/* System Time Handle

  Summary:
    Handle to a software timer instance.

  Description:
    This data type is a handle to a software timer instance.  It can be
    used to access and control a software timer.

  Remarks:
    Do not rely on the underlying type as it may change in different versions
    or implementations of the SYS Time service.
*/

typedef uintptr_t SYS_TIME_HANDLE;


// *****************************************************************************
/* Invalid System Time handle value to a software timer

  Summary:
    Invalid handle value to a software timer instance.

  Description:
    Defines the invalid handle value to a timer instance.

  Remarks:
    Do not rely on the actual value as it may change in different versions
    or implementations of the SYS Time service.
*/

#define SYS_TIME_HANDLE_INVALID   ((SYS_TIME_HANDLE) (-1))


// *****************************************************************************
/* System Time Callback Type

  Summary:
    Identifies the type of callback requested (single or periodic).

  Remarks:
    None.
*/

typedef enum
{
    // Requesting a single (one time) callback.
    SYS_TIME_SINGLE,

    // Requesting a periodically repeating callback.
    SYS_TIME_PERIODIC

} SYS_TIME_CALLBACK_TYPE;


// *****************************************************************************
/* Function:
    void ( * SYS_TIME_CALLBACK ) ( uintptr_t context )

   Summary:
    Pointer to a time system service callback function.

   Description:
    This data type defines a pointer to a time service callback function, thus
    defining the function signature.  Callback functions may be registered by
    clients of the time service either when creating a software timer or using
    the SYS_TIME_Callback shortcut functions.

   Precondition:
    The time service must have been initialized using the SYS_TIME_Initialize
    function before attempting to register a SYS Time callback function.

   Parameters:
    context    - A context value, returned untouched to the client when the
                 callback occurs.  It can be used to identify the instance of
                 the client who registered the callback.

   Returns:
    None.

  Example:
    <code>
    //"MY_APP_DATA_STRUCT" is a user defined data structure with "isTimerExpired" as
    //one of the structure members.
    void MyCallback ( uintptr_t context )
    {
        MY_APP_DATA_STRUCT* pAppData = (MY_APP_DATA_STRUCT *) context;
        if (pAppData != NULL)
        {
            pAppData->isTimerExpired = true;
        }
    }
    </code>

  Remarks:
    None.
*/

typedef void ( * SYS_TIME_CALLBACK ) ( uintptr_t context );


// *****************************************************************************
// *****************************************************************************
// Section: System Interface Functions
// *****************************************************************************
// *****************************************************************************
/*  System interface functions are called by system code to initialize the
    module and maintain proper operation of it.
*/

// *****************************************************************************
/* Function:
    SYS_MODULE_OBJ SYS_TIME_Initialize ( const SYS_MODULE_INDEX index,
                                         const SYS_MODULE_INIT * const init )

   Summary:
        Initializes the System Time module.

   Description:
        This function initializes the instance of the System Time module.

   Parameters:
       index    - Index for the instance to be initialized

       init     - Pointer to a data structure containing data necessary to
                  initialize the module.

   Returns:
        If successful, returns a valid handle to an object. Otherwise, it
        returns SYS_MODULE_OBJ_INVALID.

   Example:
        <code>

        const SYS_TIME_PLIB_INTERFACE sysTimePlibAPI = {
            .timerCallbackSet = (SYS_TIME_PLIB_CALLBACK_REGISTER)TC0_CH0_TimerCallbackRegister,
            .timerCounterGet = (SYS_TIME_PLIB_COUNTER_GET)TC0_CH0_TimerCounterGet,
            .timerPeriodSet = (SYS_TIME_PLIB_PERIOD_SET)TC0_CH0_TimerPeriodSet,
            .timerFrequencyGet = (SYS_TIME_PLIB_FREQUENCY_GET)TC0_CH0_TimerFrequencyGet,
            .timerCompareSet = (SYS_TIME_PLIB_COMPARE_SET)TC0_CH0_TimerCompareSet,
            .timerStart = (SYS_TIME_PLIB_START)TC0_CH0_TimerStart,
            .timerStop = (SYS_TIME_PLIB_STOP)TC0_CH0_TimerStop
        };

        const SYS_TIME_INIT sysTimeInitData =
        {
            .timePlib = &sysTimePlibAPI,
            .hwTimerIntNum = TC0_CH0_IRQn,
        };

        SYS_MODULE_OBJ  objSysTime;

        objSysTime = SYS_TIME_Initialize(SYS_TIME_INDEX_0, (SYS_MODULE_INIT *)&sysTimeInitData);
        if (objSysTime == SYS_MODULE_OBJ_INVALID)
        {
            // Handle error
        }
        </code>

  Remarks:
        This routine should normally only be called once during system
        initialization.
*/

SYS_MODULE_OBJ SYS_TIME_Initialize( const SYS_MODULE_INDEX index, const SYS_MODULE_INIT * const init );


// *****************************************************************************
/* Function:
   void SYS_TIME_Deinitialize ( SYS_MODULE_OBJ object )

  Summary:
       Deinitializes the specific module instance of the SYS TIMER module

  Description:
       This function deinitializes the specific module instance disabling its
       operation (and any hardware for driver modules). Resets all of the internal
       data structures and fields for the specified instance to the default settings.

  Precondition:
       The SYS_TIME_Initialize function should have been called before calling
       this function.

  Parameters:
       object   - SYS TIMER object handle, returned from SYS_TIME_Initialize

  Returns:
       None.

  Example:
        <code>
        // Handle "objSysTime" value must have been returned from SYS_TIME_Initialize.

        SYS_TIME_Deinitialize (objSysTime);

        if (SYS_TIME_Status (objSysTime) != SYS_STATUS_UNINITIALIZED)
        {
            // Check again later if you need to know
            // when the SYS TIME is De-initialized.
        }
        </code>

  Remarks:
       Once the Initialize operation has been called, the De-initialize
       operation must be called before the Initialize operation can be called
       again.
*/

void  SYS_TIME_Deinitialize ( SYS_MODULE_OBJ object );


// *****************************************************************************
/* Function:
       SYS_STATUS SYS_TIME_Status ( SYS_MODULE_OBJ object )

  Summary:
      Returns System Time status.

  Description:
       This function returns the current status of the System Time module.

  Precondition:
       None.

  Parameters:
       object  - SYS TIME object handle, returned from SYS_TIME_Initialize

  Returns:
       SYS_STATUS_UNINITIALIZED - Indicates that the driver is not initialized.

       SYS_STATUS_READY - Indicates that the module initialization is
                             complete and it ready to be used.

  Example:
       <code>
       // Handle "objSysTime" value must have been returned from SYS_TIME_Initialize.
       if (SYS_TIME_Status (objSysTime) == SYS_STATUS_READY)
       {
           // Time system service is initialized and ready to accept new requests.
       }
       </code>

  Remarks:
       None.
  */

SYS_STATUS SYS_TIME_Status ( SYS_MODULE_OBJ object );


// *****************************************************************************
// *****************************************************************************
// Section:  SYS TIME Delay Interface Functions
// *****************************************************************************
// *****************************************************************************

// *****************************************************************************
/* Function:
       SYS_TIME_RESULT SYS_TIME_DelayUS ( uint32_t us, SYS_TIME_HANDLE* handle )

   Summary:
       This function is used to generate a delay of a given number of
       microseconds.

   Description:
       The function will internally create a single shot timer which will be auto
       deleted when the application calls SYS_TIME_DelayIsComplete routine and
       the delay has expired. The function will return immediately, requiring the
       caller to use SYS_TIME_DelayIsComplete routine to check the delay timer's
       status.

   Precondition:
       The SYS_TIME_Initialize function must have been called before calling
       this function.

   Parameters:
       us     - The desired number of microseconds to delay.

       handle - Address of the variable to receive the timer handle value.

   Returns:
      SYS_TIME_SUCCESS - If the call succeeded.

      SYS_TIME_ERROR   - If the call failed.

   Example:
       <code>

       SYS_TIME_HANDLE timer = SYS_TIME_HANDLE_INVALID;

       if (SYS_TIME_DelayUS(50, &timer) != SYS_TIME_SUCCESS)
       {
           // Handle error
       }
       else if (SYS_TIME_DelayIsComplete(timer) != true)
       {
           // Wait till the delay has not expired
           while (SYS_TIME_DelayIsComplete(timer) == false);
       }
       </code>

   Remarks:
       Will delay the requested number of microseconds or longer
       depending on system performance.

       Delay values of 0 will return SYS_TIME_ERROR.

       Will return SYS_TIME_ERROR if timer handle pointer is NULL.
*/

SYS_TIME_RESULT SYS_TIME_DelayUS ( uint32_t us, SYS_TIME_HANDLE* handle );


// *****************************************************************************
/* Function:
       SYS_TIME_RESULT SYS_TIME_DelayMS ( uint32_t ms, SYS_TIME_HANDLE* handle )

   Summary:
       This function is used to generate a delay of a given number of
       milliseconds.

   Description:
       The function will internally create a single shot timer which will be auto
       deleted when the application calls SYS_TIME_DelayIsComplete routine and
       the delay has expired. The function will return immediately, requiring the
       caller to use SYS_TIME_DelayIsComplete routine to check the delay timer's
       status.

   Precondition:
       The SYS_TIME_Initialize function must have been called before calling
       this function.

   Parameters:
       ms     - The desired number of milliseconds to delay.

       handle - Address of the variable to receive the timer handle value.

   Returns:
      SYS_TIME_SUCCESS - If the call succeeded.

      SYS_TIME_ERROR   - If the call failed.

   Example:
       <code>
       SYS_TIME_HANDLE timer = SYS_TIME_HANDLE_INVALID;

       if (SYS_TIME_DelayMS(100, &timer) != SYS_TIME_SUCCESS)
       {
           // Handle error
       }
       else if(SYS_TIME_DelayIsComplete(timer) != true)
       {
           // Wait till the delay has not expired
           while (SYS_TIME_DelayIsComplete(timer) == false);
       }
       </code>

   Remarks:
       Will delay the requested number of milliseconds or longer
       depending on system performance.

       Delay values of 0 will return SYS_TIME_ERROR.

       Will return SYS_TIME_ERROR if the timer handle pointer is NULL.
*/

SYS_TIME_RESULT SYS_TIME_DelayMS ( uint32_t ms, SYS_TIME_HANDLE* handle );


// *****************************************************************************
/* Function:
       bool SYS_TIME_DelayIsComplete ( SYS_TIME_HANDLE handle )

   Summary:
       Determines if the given delay timer has completed.

   Description:
       This function determines if the requested delay is completed or is still
       in progress.

   Precondition:
       A delay request must have been created using either the SYS_TIME_DelayMS
       or SYS_TIME_DelayUS functions.

   Parameters:
       handle  - A SYS_TIME_HANDLE value provided by either SYS_TIME_DelayMS
       or SYS_TIME_DelayUS functions.

   Returns:
       true  - If the delay has completed.

       false - If the delay has not completed.

  Example:
       <code>
       // Check if the delay has expired.
       if (SYS_TIME_DelayIsComplete(timer) != true)
       {
           // Delay has not expired
       }
       </code>

  Remarks:
       SYS_TIME_DelayIsComplete must be called to poll the status of the delay
       requested through SYS_TIME_DelayMS or SYS_TIME_DelayUS.

       SYS_TIME_DelayIsComplete must not be used to poll the status of a periodic
       timer. Status of a periodic timer may be polled using the
       SYS_TIME_TimerPeriodHasExpired routine.
*/

bool SYS_TIME_DelayIsComplete ( SYS_TIME_HANDLE handle );


// *****************************************************************************
// *****************************************************************************
// Section:  SYS TIME Callback Interface Functions
// *****************************************************************************
// *****************************************************************************

// *****************************************************************************
/* Function:
    SYS_TIME_HANDLE SYS_TIME_CallbackRegisterUS ( SYS_TIME_CALLBACK callback,
                        uintptr_t context, uint32_t us, SYS_TIME_CALLBACK_TYPE type )

Summary:
    Registers a function with the time system service to be called back when the
    requested number of microseconds have expired (either once or repeatedly).

Description:
    Creates a timer object and registers a function with it to be called back
    when the requested delay (specified in microseconds) has completed.  The
    caller must identify if the timer should call the function once or repeatedly
    every time the given delay period expires.

Precondition:
    The SYS_TIME_Initialize function should have been called before calling this
    function.

Parameters:
    callback    - Pointer to the function to be called.
                  For single shot timers, the callback cannot be NULL.
                  For periodic timers, if the callback pointer is given as NULL,
                  no callback will occur, but SYS_TIME_TimerPeriodHasExpired can
                  still be polled to determine if the period has expired for a
                  periodic timer.

    context     - A client-defined value that is passed to the callback function.

    us          - Time period in microseconds.

    type        - Type of callback requested. If type is SYS_TIME_SINGLE, the
                  Callback function will be called once when the time period expires.
                  After the time period expires, the timer object will be freed.
                  If type is SYS_TIME_PERIODIC Callback function will be called
                  repeatedly, every time the time period expires until the timer
                  object is stopped or deleted.


Returns:
    SYS_TIME_HANDLE - A valid timer object handle if the call succeeds.
                      SYS_TIME_HANDLE_INVALID if it fails.

Example:
  Given a callback function implementation matching the following prototype:
  <code>
  void MyCallback ( uintptr_t context);
  </code>

  The following example call will register it, requesting a 500 microsecond
  periodic callback.
  <code>
  //Give a SYS_TIME_CALLBACK function "MyCallback",
  SYS_TIME_HANDLE handle = SYS_TIME_CallbackRegisterUS(MyCallback, (uintptr_t)0, 500, SYS_TIME_PERIODIC);
  if (handle != SYS_TIME_HANDLE_INVALID)
  {
        //timer is created successfully.
  }
  </code>
*/

SYS_TIME_HANDLE SYS_TIME_CallbackRegisterUS ( SYS_TIME_CALLBACK callback, uintptr_t context,
                                              uint32_t us, SYS_TIME_CALLBACK_TYPE type );


// *****************************************************************************
/* Function:
    SYS_TIME_HANDLE SYS_TIME_CallbackRegisterMS ( SYS_TIME_CALLBACK callback,
                        uintptr_t context, uint32_t ms, SYS_TIME_CALLBACK_TYPE type )

Summary:
    Registers a function with the time system service to be called back when the
    requested number of milliseconds has expired (either once or repeatedly).

Description:
    Creates a timer object and registers a function with it to be called back
    when the requested delay (specified in milliseconds) has completed.  The
    caller must identify if the timer should call the function once or repeatedly
    every time the given delay period expires.

Precondition:
    The SYS_TIME_Initialize function should have been called before calling this
    function.

Parameters:
    callback    - Pointer to the function to be called.
                  For single shot timers, the callback cannot be NULL.
                  For periodic timers, if the callback pointer is given as NULL,
                  no callback will occur, but SYS_TIME_TimerPeriodHasExpired can
                  still be polled to determine if the period has expired for a
                  periodic timer.

    context     - A client-defined value that is passed to the callback function.

    ms          - Time period in milliseconds.

    type        - Type of callback requested. If type is SYS_TIME_SINGLE, the
                  Callback function will be called once when the time period expires.
                  After the time period expires, the timer object will be freed.
                  If type is SYS_TIME_PERIODIC Callback function will be called
                  repeatedly, every time the time period expires until the timer
                  object is stopped or deleted.


Returns:
    SYS_TIME_HANDLE - A valid timer object handle if the call succeeds.
                      SYS_TIME_HANDLE_INVALID if it fails.

Example:
  Given a callback function implementation matching the following prototype:
  <code>
  void MyCallback ( uintptr_t context);
  </code>

  The following example call will register it, requesting a 50 millisecond
  periodic callback.
  <code>
  //Give a SYS_TIME_CALLBACK function "MyCallback",
  SYS_TIME_HANDLE handle = SYS_TIME_CallbackRegisterMS(MyCallback, (uintptr_t)0, 50, SYS_TIME_PERIODIC);
  if (handle != SYS_TIME_HANDLE_INVALID)
  {
        //timer is created successfully.
  }
  </code>
*/

SYS_TIME_HANDLE SYS_TIME_CallbackRegisterMS ( SYS_TIME_CALLBACK callback, uintptr_t context,
                                              uint32_t ms, SYS_TIME_CALLBACK_TYPE type );


// *****************************************************************************
// *****************************************************************************
// Section:  SYS TIME 32-bit Counter and Conversion Functions
// *****************************************************************************
// *****************************************************************************

// *****************************************************************************
/* Function:
    uint32_t SYS_TIME_FrequencyGet ( void )

  Summary:
    Gets the frequency at which the hardwaer timer counts.

  Description:
    Returns the frequency at which the hardware timer runs. This frequency
    determines the maximum resolution of all services provided by SYS_TIME.

  Precondition:
    The SYS_TIME_Initialize function should have been called before calling this
    function.

  Parameters:
    None

  Returns:
    The frequency at which the hardware timer runs, if the timer has been
    initialized and is ready. Otherwise, it returns 0.

  Example:
    <code>
    uint32_t frequency = SYS_TIME_FrequencyGet();
    </code>

  Remarks:
    This frequency is determined by hardware capabilities and how they are
    configured and initialized.
*/

uint32_t SYS_TIME_FrequencyGet ( void );


// *****************************************************************************
/* Function:
    uint32_t SYS_TIME_CounterGet ( void )

  Summary:
    Get the common 32-bit system counter value.

  Description:
    Returns the current "live" value of the common 32-bit system counter.

  Precondition:
    The SYS_TIME_Initialize function should have been called before calling this
    function.

  Parameters:
    None

  Returns:
    The current "live" value of the common 32-bit system counter.

  Example:
    <code>
    uint32_t timeStamp1;
    uint32_t timeStamp2;
    uint32_t timeDiffMs;

    // Take first time stamp
    timeStamp1 = SYS_TIME_CounterGet();

    //Perform some tasks....

    // Take second time stamp
    timeStamp2 = SYS_TIME_CounterGet();

    //Calculate the time difference. Use the appropriate function -
    //SYS_TIME_CountToMS or SYS_TIME_CountToUS to calculate the time difference.

    timeDiffMs = SYS_TIME_CountToMS((timeStamp2 - timeStamp1));
    </code>

  Remarks:
    The value returned may be stale as soon as it is provided, as the timer is
    live and running at full frequency resolution (as configured and as reported
    by the SYS_TIME_FrequencyGet function).  If additional accuracy is required,
    use a hardware timer instance.
*/

uint32_t SYS_TIME_CounterGet ( void );

// *****************************************************************************
/* Function:
    uint64_t SYS_TIME_Counter64Get ( void )

  Summary:
    Get the common 64-bit system counter value.

  Description:
    Returns the current "live" value of the common 64-bit system counter.

  Precondition:
    The SYS_TIME_Initialize function should have been called before calling this
    function.

  Parameters:
    None

  Returns:
    The current "live" value of the common 64-bit system counter.

  Example:
    <code>
    uint64_t timeStamp1;
    uint64_t timeStamp2;
    uint64_t timeDiffMs;

    // Take first time stamp
    timeStamp1 = SYS_TIME_Counter64Get();

    //Perform some tasks....

    // Take second time stamp
    timeStamp2 = SYS_TIME_Counter64Get();

    //Calculate the time difference.
    timeDiffMs = timeStamp2 - timeStamp1;
    timeDiffMs = (uint64_t)(((double)timeDiffMs/SYS_TIME_FrequencyGet())*1000.0);
    </code>

  Remarks:
    The value returned may be stale as soon as it is provided, as the timer is
    live and running at full frequency resolution (as configured and as reported
    by the SYS_TIME_FrequencyGet function).  If additional accuracy is required,
    use a hardware timer instance.
*/

uint64_t SYS_TIME_Counter64Get ( void );

// *****************************************************************************
/* Function:
    void SYS_TIME_CounterSet ( uint32_t count )

  Summary:
    Sets the common 32-bit system counter value.

  Description:
    Sets the current "live" value of the common 32-bit system counter.

  Precondition:
    The SYS_TIME_Initialize function should have been called before calling this
    function.

  Parameters:
    count  - The 32-bit counter value to write to the common system counter.

  Returns:
    None

  Example:
    SYS_TIME_CounterSet(1000);

  Remarks:
    None.
*/

void SYS_TIME_CounterSet ( uint32_t count );


// *****************************************************************************
/* Function:
      uint32_t SYS_TIME_CountToUS ( uint32_t count )

   Summary:
      Converts a counter value to time interval in microseconds.

   Description:
      This function converts a counter value to time interval in
      microseconds, based on the hardware timer frequency as configured
      and as reported by SYS_TIME_FrequencyGet.

   Precondition:
      The SYS_TIME_Initialize function should have been called before calling
      this function.

   Parameters:
      count  - Counter value to be converted to an equivalent value in
               microseconds.

   Returns:
      Number of microseconds represented by the given counter value.

   Example:
      Given a previously captured 32-bit counter value called "timestamp"
      captured using the SYS_TIME_CounterGet function, the following example
      will calculate the number of microseconds elapsed since timestamp was
      captured.
      <code>
      uint32_t count = SYS_TIME_CounterGet() - timestamp;
      uint32_t us    = SYS_TIME_CountToUS(count);
      </code>

  Remarks:
     None.
*/

uint32_t  SYS_TIME_CountToUS ( uint32_t count );


// *****************************************************************************
/* Function:
      uint32_t SYS_TIME_CountToMS ( uint32_t count )

   Summary:
      Converts a counter value to time interval in milliseconds.

   Description:
      This function converts a counter value to time interval in
      milliseconds, based on the hardware timer frequency as configured
      and as reported by SYS_TIME_FrequencyGet.

   Precondition:
      The SYS_TIME_Initialize function should have been called before calling
      this function.

   Parameters:
      count  - Counter value to be converted to an equivalent value in
               milliseconds.

   Returns:
      Number of milliseconds represented by the given counter value.

  Example:
      Given a previously captured uint32_t counter value called "timestamp"
      captured using the SYS_TIME_CounterGet function, the following example
      will calculate number of milliseconds elapsed since timestamp was
      captured.
      <code>
      uint32_t count = SYS_TIME_CounterGet() - timestamp;
      uint32_t ms    = SYS_TIME_CountToMS(count);
      </code>

  Remarks:
      None.
*/

uint32_t  SYS_TIME_CountToMS ( uint32_t count );


// *****************************************************************************
/* Function:
      uint32_t SYS_TIME_USToCount ( uint32_t us )

   Summary:
      Convert the given time interval in microseconds to an equivalent
      counter value.

   Description:
      This function converts a given time interval (measured in microseconds) to
      an equivalent 32-bit counter value, based on the configured hardware timer
      frequency as reported by SYS_TIME_FrequencyGet.

   Precondition:
      The SYS_TIME_Initialize function should have been called before calling
      this function.

   Parameters:
      us  - Time interval in microseconds.

   Returns:
      Number of hardware timer counts that will expire in the given time interval.

   Example:
      <code>
      uint32_t futureCounter = SYS_TIME_CounterGet() + SYS_TIME_USToCount(200);
      </code>

   Remarks:
      None.
*/

uint32_t SYS_TIME_USToCount ( uint32_t us );


// *****************************************************************************
/* Function:
      uint32_t SYS_TIME_MSToCount ( uint32_t ms )

   Summary:
      Convert the given time interval in milliseconds to an equivalent
      counter value.

   Description:
      This function converts a given time interval (measured in milliseconds) to
      an equivalent 32-bit counter value, based on the configured hardware timer
      frequency as reported by SYS_TIME_FrequencyGet.

   Precondition:
      The SYS_TIME_Initialize function should have been called before calling
      this function.

   Parameters:
      ms  - Time interval in milliseconds.

   Returns:
      Number of hardware timer counts that will expire in the given time interval.

   Example:
      <code>
      uint32_t futureCounter = SYS_TIME_CounterGet() + SYS_TIME_MSToCount(10);
      </code>

   Remarks:
    None.
*/

uint32_t SYS_TIME_MSToCount ( uint32_t ms );


// *****************************************************************************
// *****************************************************************************
// Section:  SYS TIME 32-bit Software Timers
// *****************************************************************************
// *****************************************************************************

// *****************************************************************************
/* Function:
    SYS_TIME_HANDLE SYS_TIME_TimerCreate (
        uint32_t count,
        uint32_t period,
        SYS_TIME_CALLBACK callback,
        uintptr_t context,
        SYS_TIME_CALLBACK_TYPE type )

   Summary:
    Creates and initializes a new 32-bit software timer instance.

   Description:
    This function creates/allocates a new instance of a 32-bit software timer.

    A software timer provides a counter that is separate from other timer
    counters and is under control of the caller. The counter can be started
    and stopped under caller control and its counter value and period value
    can be changed while the counter is either running or stopped.

   Precondition:
    The SYS_TIME_Initialize function should have been called before calling this
    function.

   Parameters:
    count      - The initial value of the counter, after the timer has been
                 created and before it has been started.

    period     - The counter interval at which the timer indicates time has
                 elapsed.

    callback   - Pointer to function that will be called every time the
                 period counts have elapsed.  (Actual timing will depend on
                 system performance and the base frequency at which the time
                 service is configured).

                 For single shot timers, the callback cannot be NULL.
                 For periodic timers, if the callback pointer is given as NULL,
                 no callback will occur, but SYS_TIME_TimerPeriodHasExpired can
                 still be polled to determine if the period has expired for a
                 periodic shot timer.

    context    - A caller-defined value that's passed (unmodified) back to
                 the client as a parameter of callback function.  It can be
                 used to identify the client's context or passed with any
                 value.

    type        - Type of callback requested. If type is SYS_TIME_SINGLE, the
                  Callback function will be called once when the time period expires.
                  If type is SYS_TIME_PERIODIC Callback function will be called
                  repeatedly, every time the time period expires until the timer
                  object is stopped or deleted.

   Returns:
    An opaque value used to identify software timer instance if the call
    succeeds in allocating/creating the software timer. If the call fails
    SYS_TIME_HANDLE_INVALID is returned.

  Example:
    Given an implementation of the following function prototype:
    <code>
    void MyCallback ( uintptr_t context);
    </code>
    The following example creates a software timer instance.
    <code>
    SYS_TIME_HANDLE handle;
    //myData is the user-defined data that will be passed back in the registered callback function.
    handle = SYS_TIME_TimerCreate(0, SYS_TIME_MSToCount(200), &MyCallback, (uintptr_t)&myData, SYS_TIME_SINGLE);
    if (handle != SYS_TIME_HANDLE_INVALID)
    {
        //timer is created successfully.
    }
    </code>

  Remarks:
    None.
*/

SYS_TIME_HANDLE SYS_TIME_TimerCreate (
    uint32_t count,
    uint32_t period,
    SYS_TIME_CALLBACK callback,
    uintptr_t context,
    SYS_TIME_CALLBACK_TYPE type
);


// *****************************************************************************
/* Function:
    SYS_TIME_RESULT SYS_TIME_TimerReload (
        SYS_TIME_HANDLE handle,
        uint32_t count,
        uint32_t period,
        SYS_TIME_CALLBACK callback,
        uintptr_t context,
        SYS_TIME_CALLBACK_TYPE type
    )

   Summary:
    Reloads (or reinitializes) the software timer instance.

   Description:
    This function reloads the initial values for an already created/allocated
    instance of a software timer, even if it is currently running.

   Precondition:
    The SYS_TIME_Initialize must have been called and a valid handle to the software
    timer to be reloaded must be available.

   Parameters:
    handle     - Handle to a software timer instance.

    count      - The new value of the counter.

    period     - The new period value.

    callback   - The new callback function pointer.
                 For single shot timers, the callback must be specified.
                 For periodic timers, if the callback pointer is given as NULL,
                 no callback will occur, but SYS_TIME_TimerPeriodHasExpired can
                 still be polled to determine if the period has expired for a
                 periodic timer.

    context    - The new caller-defined value that's passed (unmodified) back to
                 the client as a parameter of callback function.

    type        - Type of callback requested. If type is SYS_TIME_SINGLE, the
                  Callback function will be called once when the time period expires.
                  If type is SYS_TIME_PERIODIC Callback function will be called
                  repeatedly, every time the time period expires until the timer
                  object is stopped or deleted.

   Returns:
      SYS_TIME_SUCCESS - If the call succeeded.

      SYS_TIME_ERROR   - If the call failed (and the timer was not modified).

  Example:
    Given an implementation of the following function prototype:
    <code>
    void MyNewCallback ( uintptr_t context);
    </code>
    The following example updates a software timer instance.
    <code>
    //myNewData is the user-defined data that will be passed back in the registered callback function.
    if (SYS_TIME_TimerReload(timer, 0, SYS_TIME_MSToCount(500), &MyNewCallback, (uintptr_t)&myNewData, SYS_TIME_PERIODIC) != SYS_TIME_SUCCESS )
    {
        // Handle error
    }
    </code>

  Remarks:
    This function facilitates changing multiple timer parameters quickly and
    atomically.
*/
SYS_TIME_RESULT SYS_TIME_TimerReload (
    SYS_TIME_HANDLE handle,
    uint32_t count,
    uint32_t period,
    SYS_TIME_CALLBACK callback,
    uintptr_t context,
    SYS_TIME_CALLBACK_TYPE type
);


// *****************************************************************************
/* Function:
    SYS_TIME_RESULT SYS_TIME_TimerDestroy ( SYS_TIME_HANDLE handle)

   Summary:
       Destroys/deallocates a software timer instance.

   Description:
       This function deletes and deallocates a software timer instance,
       stopping its counter and releasing the associated resources.

   Precondition:
       The SYS_TIME_Initialize and a valid handle to the software timer to be
       destroyed must be available.

   Parameters:
       handle  - Handle to a software timer instance.

   Returns:
       SYS_TIME_SUCCESS - If the given software was successfully destroyed.

       SYS_TIME_ERROR   - If an error occured or the given handle was invalid.

  Example:
    <code>
    // "timer" is the handle to the software timer to be destroyed.
    if (SYS_TIME_TimerDestroy(timer) != SYS_TIME_SUCCESS)
    {
        // Handle Error
    }
    </code>

  Remarks:
    Released timer resources can be reused by other clients.
    Single shot timers are auto destroyed on expiry.
    Calling SYS_TIME_DelayIsComplete auto destroys the delay timer if it has expired.
*/

SYS_TIME_RESULT SYS_TIME_TimerDestroy ( SYS_TIME_HANDLE handle );


// *****************************************************************************
/* Function:
        SYS_TIME_RESULT  SYS_TIME_TimerCounterGet (
            SYS_TIME_HANDLE handle,
            uint32_t *count
        )

   Summary:
        Gets the elapsed counter value of a software timer.

   Description:
        This function gets the elapsed counter value of the software timer
        identified by the handle given.

   Precondition:
       The SYS_TIME_Initialize must have been called and a valid handle to the
       software timer must be available.

   Parameters:
       handle   - Handle to a software timer instance.

       count    - Address of the variable to receive the value of the given
                  software timer's elapsed counter.

                  This parameter is ignored when the return value is not
                  SYS_TIME_SUCCES.

   Returns:
       SYS_TIME_SUCCESS if the operation succeeds.

       SYS_TIME_ERROR if the operation fails (due, for example, to an
       to an invalid handle).

  Example:
       Given a "timer" handle, the following example will get the given
       software timer's elapsed counter value.
       <code>
       uint32_t count;
       if (SYS_TIME_TimerCounterGet(timer, &count) != SYS_TIME_SUCCESS)
       {
           // Handle error
       }
       </code>

  Remarks:
        The counter value may be stale immediately  upon function return,
        depending upon timer frequency and system performance.
*/

SYS_TIME_RESULT SYS_TIME_TimerCounterGet (
    SYS_TIME_HANDLE handle,
    uint32_t *count
);


// *****************************************************************************
/* Function:
       SYS_TIME_RESULT SYS_TIME_TimerStart ( SYS_TIME_HANDLE handle )
   Summary:
        Starts a software timer running.

   Description:
        This function starts a previously created software timer.

   Precondition:
        The SYS_TIME_Initialize must have been called and a valid handle to the
        software timer to be started must be available.

   Parameters:
       handle   - Handle to a software timer instance.

   Returns:
       SYS_TIME_SUCCESS if the operation succeeds.

       SYS_TIME_ERROR if the operation fails (due, for example, to an
                  to an invalid handle).

   Example:
       Given a "timer" handle, the following example will start the timer's
       counter running.
       <code>
       SYS_TIME_TimerStart(timer);
       </code>

  Remarks:
      Calling SYS_TIME_TimerStart on an already running timer will have no
      affect and will return SYS_TIME_SUCCESS.
      Calling SYS_TIME_TimerStart on a timer that is stopped, will always restart
      the timer from it's initial configured timer/counter value and will not
      resume the timer from the counter value at which it was stopped.
*/

SYS_TIME_RESULT SYS_TIME_TimerStart ( SYS_TIME_HANDLE handle );


// *****************************************************************************
/* Function:
       SYS_TIME_RESULT SYS_TIME_TimerStop ( SYS_TIME_HANDLE handle )
   Summary:
        Stops a running software timer.

   Description:
        This function stops a previously created and running software timer (i.e.
        the given timer's counter will stop incrementing).

   Precondition:
        The SYS_TIME_Initialize must have been called and a valid handle to the
        software timer to be stopped must be available.

   Parameters:
       handle   - Handle to a software timer instance.

   Returns:
       SYS_TIME_SUCCESS if the operation succeeds.

       SYS_TIME_ERROR if the operation fails (due, for example, to an
       to an invalid handle).

   Example:
       Given a "timer" handle, the following example will stops the timer's
       counter running.
       <code>
       SYS_TIME_TimerStop(timer);
       </code>

  Remarks:
      Calling SYS_TIME_TimerStop on a timer that is not running will have no
      affect and will return SYS_TIME_SUCCESS.
*/

SYS_TIME_RESULT SYS_TIME_TimerStop ( SYS_TIME_HANDLE handle );


// *****************************************************************************
/* Function:
       bool SYS_TIME_TimerPeriodHasExpired ( SYS_TIME_HANDLE handle )

   Summary:
       Reports whether or not the current period of a software timer has expired.

   Description:
       This function reports whether or not the current period of the given
       software timer has expired and clears the internal flag tracking period
       expiration so that each period expiration will only be reported once.

   Precondition:
       The SYS_TIME_Initialize and SYS_TIME_TimerCreate or SYS_TIME_CallbackRegisterMS/
       SYS_TIME_CallbackRegisterUS functions (with callback type set to
       SYS_TIME_PERIODIC) must have been called before calling this function.

   Parameters:
       handle   - Handle to a software timer instance

   Returns:
       true     - If the period has expired.

       false    - If the period is not expired.

  Example:
       <code>
       if (SYS_TIME_TimerPeriodHasExpired(timer) == true)
       {
           // Timer has expired. Take desired action.
       }
       </code>

  Remarks:
       1.  For a periodic timer, a call to SYS_TIME_TimerPeriodHasExpired
           returns true after the first time period has expired. After calling
           this function, the expiry flag is internally cleared and is set again
           once the ongoing period of the periodic timer expires.

       2.  Unlike the SYS_TIME_DelayIsComplete routine the
           SYS_TIME_TimerPeriodHasExpired does not delete the timer, it just returns
           the status of the timer.

       3.  To poll the status of the delay timers, SYS_TIME_DelayIsComplete must
           be used instead of the SYS_TIME_TimerPeriodHasExpired routine, as
           SYS_TIME_DelayIsComplete additionally deletes the delay timer object
           once the delay has expired.

       4.  Since single shot timers does not support polling (registration of a callback
           is mandatory for single shot timers), the SYS_TIME_CallbackRegisterMS
           routine must not be used to poll the status of the single shot timers.
*/

bool SYS_TIME_TimerPeriodHasExpired ( SYS_TIME_HANDLE handle );


// DOM-IGNORE-BEGIN
#ifdef __cplusplus  // Provide C++ Compatibility

    }

#endif
// DOM-IGNORE-END

#endif //SYS_TIME_H
