/*******************************************************************************
  Debug System Services Library Header

  Company:
    Microchip Technology Inc.

  File Name:
    sys_debug.h

  Summary:
    Defines the common debug definitions and interfaces used by MPLAB Harmony
    libraries to report errors and debug information to the user.

  Description:
    This header file defines the common debug definitions and interface macros
    (summary below) and prototypes used by MPLAB Harmony libraries to report
    errors and debug information to the user.
 *******************************************************************************/

//DOM-IGNORE-BEGIN
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
//DOM-IGNORE-END

#ifndef SYS_DEBUG_H
#define SYS_DEBUG_H

// *****************************************************************************
// *****************************************************************************
// Section: File includes
// *****************************************************************************
// *****************************************************************************

#include "system/system.h"

// DOM-IGNORE-BEGIN
#ifdef __cplusplus  // Provide C++ Compatibility

    extern "C" {

#endif
// DOM-IGNORE-END

// *****************************************************************************
// *****************************************************************************
// Section: SYS DEBUG Data Types
// *****************************************************************************
// *****************************************************************************

// *****************************************************************************
/* SYS_ERROR_LEVEL enumeration

   Summary:
    System error message priority levels.

   Description:
    This enumeration defines the supported system error message priority values.

   Remarks:
    Used by debug message macros to compare individual message priority against
    a global system-wide error message priority level to determine if an
    individual message should be displayed.
*/

typedef enum
{
    /* Errors that have the potential to cause a system crash. */
    SYS_ERROR_FATAL     = 0,

    /* Errors that have the potential to cause incorrect behavior. */
    SYS_ERROR_ERROR     = 1,

    /* Warnings about potentially unexpected behavior or side effects. */
    SYS_ERROR_WARNING   = 2,

    /* Information helpful to understanding potential errors and warnings. */
    SYS_ERROR_INFO      = 3,

    /* Verbose information helpful during debugging and testing. */
    SYS_ERROR_DEBUG     = 4

} SYS_ERROR_LEVEL;


// DOM-IGNORE-BEGIN

// *****************************************************************************
/* Do not use this variable directly.  Use SYS_DEBUG_ErrorLevelGet or
   SYS_DEBUG_ErrorLevelSet functions to access this value.
*/
extern SYS_ERROR_LEVEL gblErrLvl;

// *****************************************************************************
/* SYS Debug Initialize structure

  Summary:
    Defines the data required to initialize the debug system service.

  Description:
    This structure defines the data required to initialize the debug system
    service.

  Remarks:
    None.
*/

typedef struct
{
    /* System module initialization */
    SYS_MODULE_INIT                 moduleInit;

    /* Initial system error level setting. */
    SYS_ERROR_LEVEL                 errorLevel;

    /* Console index to receive debug messages */
    SYS_MODULE_INDEX                consoleIndex;

} SYS_DEBUG_INIT;

// DOM-IGNORE-END

// *****************************************************************************
/* SYS Debug Module Index Number

  Summary:
    Debug System Service index.

  Description:
    This constant defines a symbolic name for the debug system service index.

  Remarks:
    There can only be a single debug system service instance in the system.
*/

#define SYS_DEBUG_INDEX_0           0


// *****************************************************************************
// *****************************************************************************
// Section: SYS DEBUG Functions
// *****************************************************************************
// *****************************************************************************

// *****************************************************************************
/* Function:
    SYS_MODULE_OBJ SYS_DEBUG_Initialize(
        const SYS_MODULE_INDEX index,
        const SYS_MODULE_INIT* const init
    )

  Summary:
    Initializes the global error level and specific module instance.

  Description:
    This function initializes the global error level. It also initializes any
    internal system debug module data structures.

  Precondition:
    None.

  Parameters:
    index           - Index for the instance to be initialized.
    init            - Pointer to a data structure containing any data necessary
                      to initialize the debug service. This pointer may be null
                      if no data is required because static overrides have
                      been provided.

  Returns:
    If successful, returns SYS_MODULE_OBJ_STATIC.  Otherwise, it returns
    SYS_MODULE_OBJ_INVALID.

  Example:
    <code>
    SYS_MODULE_OBJ objectHandle;
    SYS_DEBUG_INIT debugInit =
    {
        .moduleInit = {0},
        .errorLevel = SYS_DEBUG_GLOBAL_ERROR_LEVEL,
        .consoleIndex = 0,
    };

    objectHandle = SYS_DEBUG_Initialize(SYS_DEBUG_INDEX_0, (SYS_MODULE_INIT*)&debugInit);
    if (objectHandle == SYS_MODULE_OBJ_INVALID)
    {
        // Handle error
    }
    </code>

  Remarks:
    This routine should only be called once during system initialization.
*/

SYS_MODULE_OBJ SYS_DEBUG_Initialize(
    const SYS_MODULE_INDEX index,
    const SYS_MODULE_INIT* const init
);

// *****************************************************************************
/* Function:
    void SYS_DEBUG_Tasks( SYS_MODULE_OBJ object )

  Summary:
    Maintains the debug module's state machine.

  Description:
    This function is used to maintain the debug module's internal state
    machine.

  Precondition:
    The SYS_DEBUG_Initialize function must have been called.

  Parameters:
    object    - SYS DEBUG object handle, returned from SYS_DEBUG_Initialize

  Returns:
    None.

  Example:
    <code>
    SYS_MODULE_OBJ      object;     // Returned from SYS_DEBUG_Initialize

    while (true)
    {
        SYS_DEBUG_Tasks (object);

        // Do other tasks
    }
    </code>

  Remarks:
    This function is normally not called directly by an application.
    The task routine may not be called if the debug service does not require
    maintaining an internal state machine.
*/

void SYS_DEBUG_Tasks(SYS_MODULE_OBJ object);

// *****************************************************************************
/* Function:
    SYS_STATUS SYS_DEBUG_Status ( SYS_MODULE_OBJ object )

  Summary:
    Returns status of the specific instance of the debug service module.

  Description:
    This function returns the status of the specific debug service module
    instance.

  Precondition:
    The SYS_DEBUG_Initialize function should have been called before calling
    this function.

  Parameters:
    object    - Debug module object handle, returned from SYS_DEBUG_Initialize

  Returns:
    * SYS_STATUS_READY          - Indicates that the module is running and
                                  ready to service requests.  Any value greater
                                  than SYS_STATUS_READY is also a normal
                                  running state in which the module is ready
                                  to accept new operations.
    * SYS_STATUS_BUSY           - Indicates that the module is busy with a
                                  previous system level operation.
    * SYS_STATUS_ERROR          - Indicates that the module is in an error
                                  state.  Any value less than SYS_STATUS_ERROR
                                  is also an error state.
    * SYS_STATUS_UNINITIALIZED  - Indicates that the module has not been
                                  initialized.

  Example:
    <code>
    SYS_MODULE_OBJ      object;     // Returned from SYS_CONSOLE_Initialize
    SYS_STATUS          debugStatus;

    debugStatus = SYS_DEBUG_Status (object);
    if (debugStatus == SYS_STATUS_READY)
    {
        // Debug service is initialized and ready to accept requests.
    }
    </code>

  Remarks:
    None.
*/

SYS_STATUS SYS_DEBUG_Status ( SYS_MODULE_OBJ object );


// *****************************************************************************
// *****************************************************************************
// Section: SYS DEBUG Console Functions
// *****************************************************************************
// *****************************************************************************

// *****************************************************************************
/* Function:
    void SYS_DEBUG_Message(const char *message)

  Summary:
    Prints a message to the console regardless of the system error level.

  Description:
    This function prints a message to the console regardless of the system
    error level. It can be used as an implementation of the SYS_MESSAGE and
    SYS_DEBUG_MESSAGE macros.

  Precondition:
    SYS_DEBUG_Initialize must have returned a valid object handle.

  Parameters:
    message - Pointer to a message string to be displayed.

  Returns:
    None.

  Example:
    <code>
    // In configuration.h file: #define SYS_DEBUG_USE_CONSOLE
    // In sys_debug.h file: #define SYS_MESSAGE(message) SYS_DEBUG_Message(message)

    SYS_MESSAGE("My Message\r\n");
    </code>

  Remarks:
    Do not call this function directly.  Call the SYS_MESSAGE or
    SYS_DEBUG_MESSAGE macros instead.

    The default SYS_MESSAGE and SYS_DEBUG_MESSAGE macro definitions remove
    the messages and message function calls from the source code.  To access
    and utilize the messages, define the SYS_DEBUG_USE_CONSOLE macro or
    override the definitions of the individual macros.
*/

void SYS_DEBUG_Message(const char *message);


// *****************************************************************************
/* Function:
    void SYS_DEBUG_Print(const char *format, ...)

  Summary:
    Formats and prints a message with a variable number of arguments to the
    console regardless of the system error level.

  Description:
    This function formats and prints a message with a variable number of
    arguments to the console regardless of the system error level. It can be
    used to implement the SYS_PRINT and SYS_DEBUG_PRINT macros.

  Precondition:
    SYS_DEBUG_Initialize must have returned a valid object handle.

  Parameters:
    format          - Pointer to a buffer containing the format string for
                      the message to be displayed.
    ...             - Zero or more optional parameters to be formated as
                      defined by the format string.

  Returns:
    None.

  Example:
    <code>
    // In configuration.h file: #define SYS_DEBUG_USE_CONSOLE
    // In sys_debug.h file: #define SYS_PRINT(fmt, ...) SYS_DEBUG_Print(fmt, ##__VA_ARGS__)

    // In source code
    int result;

    result = SomeOperation();
    if (result > MAX_VALUE)
    {
        SYS_PRINT("Result of %d exceeds max value\r\n", result);
    }
    </code>

  Remarks:
    The format string and arguments follow the printf convention.

    Do not call this function directly. Call the SYS_PRINT or SYS_DEBUG_PRINT
    macros instead.

    The default SYS_PRINT and SYS_DEBUG_PRINT macro definitions remove the
    messages and message function calls.  To access and utilize the messages,
    define the SYS_DEBUG_USE_CONSOLE macro or override the definitions of the
    individual macros.
*/

void SYS_DEBUG_Print( const char *format, ... );

// *****************************************************************************
/* Function:
    void SYS_DEBUG_ErrorLevelSet(SYS_ERROR_LEVEL level)

  Summary:
    Sets the global system error reporting level.

  Description:
    This function sets the global system error reporting level.

  Precondition:
    SYS_DEBUG_Initialize must have returned a valid object handle.

  Parameters:
    level - The desired system error level.

  Returns:
    None.

  Example:
    <code>
    SYS_DEBUG_ErrorLevelSet(SYS_ERROR_WARNING);
    </code>

  Remarks:
    None.
*/

void SYS_DEBUG_ErrorLevelSet(SYS_ERROR_LEVEL level);

// *****************************************************************************
/* Function:
   SYS_ERROR_LEVEL SYS_DEBUG_ErrorLevelGet(void)

  Summary:
    Returns the global system Error reporting level.

  Description:
    This function returns the global System Error reporting level.

  Precondition:
    SYS_DEBUG_Initialize must have returned a valid object handle.

  Parameters:
    None.

  Returns:
    The global System Error Level.

  Example:
    <code>
    SYS_ERROR_LEVEL level;

    level = SYS_DEBUG_ErrorLevelGet();
    </code>

  Remarks:
    None.
*/

SYS_ERROR_LEVEL SYS_DEBUG_ErrorLevelGet(void);


// *****************************************************************************
// *****************************************************************************
// Section: SYS DEBUG Macros
// *****************************************************************************
// *****************************************************************************

// *****************************************************************************
/* Macro:
    _SYS_DEBUG_MESSAGE( SYS_ERROR_LEVEL level, const char* message )

  Summary:
    Prints a debug message if the specified level is at or below the global
    system error level.

  Description:
    This macro prints a debug message if the specified level is at or below the
    global error level.  It can be used to implement the SYS_DEBUG_MESSAGE macro.

  Precondition:
    SYS_DEBUG_Initialize must have returned a valid object handle.

  Parameters:
    level           - The current error level threshold for displaying the message.
    message         - Pointer to a buffer containing the message to be displayed.

  Returns:
    None.

  Example:
    <code>
    // In configuration.h file: #define SYS_DEBUG_USE_CONSOLE
    // In sys_debug.h file: #define SYS_DEBUG_MESSAGE(level,message)  _SYS_DEBUG_MESSAGE(level,message)

    // In source code
    SYS_DEBUG_MESSAGE(SYS_ERROR_WARNING, "My debug warning message\r\n");
    </code>

  Remarks:
    Do not call this macro directly.  Call the SYS_DEBUG_MESSAGE macro instead.

    The default SYS_DEBUG_MESSAGE macro definition removes the message and
    function call from the source code. To access and utilize the message,
    define the SYS_DEBUG_USE_CONSOLE macro or override the definition of the
    SYS_DEBUG_MESSAGE macro.
*/

#ifndef _SYS_DEBUG_MESSAGE
    #define _SYS_DEBUG_MESSAGE(level, message)  do { if((level) <= SYS_DEBUG_ErrorLevelGet()) SYS_DEBUG_Message(message); }while(0)
#endif

// *****************************************************************************
/* Macro:
    _SYS_DEBUG_PRINT( SYS_ERROR_LEVEL level, const char* format, ... )

  Summary:
    Formats and prints a debug message if the specified level is at or below the
    global system error level.

  Description:
    This function formats and prints a debug message if the specified level is
    at or below the global system error level.  It can be used to implement the
    SYS_DEBUG_PRINT macro.

  Precondition:
    SYS_DEBUG_Initialize must have returned a valid object handle.

  Parameters:
    level           - The current error level threshold for displaying the message.
    format          - Pointer to a buffer containing the format string for
                      the message to be displayed.
    ...             - Zero or more optional parameters to be formated as
                      defined by the format string.

  Returns:
    None.

  Example:
    <code>
    // In configuration.h file: #define SYS_DEBUG_USE_CONSOLE
    // In sys_debug.h file: #define SYS_DEBUG_PRINT(level, fmt, ...) _SYS_DEBUG_PRINT(level, fmt, ##__VA_ARGS__)

    // In source code
    int result;

    result = SomeOperation();
    if (result > MAX_VALUE)
    {
        SYS_DEBUG_PRINT(SYS_ERROR_WARNING, "Result of %d exceeds max value\r\n", result);
        // Take appropriate action
    }
    </code>

  Remarks:
    Do not call this macro directly.  Call the SYS_DEBUG_PRINT macro instead.

    The default SYS_DEBUG_PRINT macro definition removes the message and
    function call from the source code.  To access and utilize the message,
    define the SYS_DEBUG_USE_CONSOLE macro or override the definition of the
    SYS_DEBUG_PRINT macro.
*/

#ifndef _SYS_DEBUG_PRINT
    #define _SYS_DEBUG_PRINT(level, format, ...)    do { if((level) <= SYS_DEBUG_ErrorLevelGet()) SYS_DEBUG_Print(format, ##__VA_ARGS__); } while (0)
#endif

// DOM-IGNORE-BEGIN

// *****************************************************************************
/*  These definitions support the SYS_DEBUG_USE_CONSOLE build-time configuration
    option.  Defining SYS_DEBUG_USE_CONSOLE in the system configuration
    (configuration.h) will map the macros below to the appropriate system
    console functions.
*/

#ifdef SYS_DEBUG_USE_CONSOLE

#ifndef SYS_MESSAGE
    #define SYS_MESSAGE(message)                SYS_DEBUG_Message(message)
#endif

#ifndef SYS_DEBUG_MESSAGE
    #define SYS_DEBUG_MESSAGE(level, message)   _SYS_DEBUG_MESSAGE(level, message)
#endif

#ifndef SYS_PRINT
    #define SYS_PRINT(fmt, ...)                 SYS_DEBUG_Print(fmt, ##__VA_ARGS__)
#endif

#ifndef SYS_DEBUG_PRINT
    #define SYS_DEBUG_PRINT(level, fmt, ...)    _SYS_DEBUG_PRINT(level, fmt, ##__VA_ARGS__)
#endif

#endif


// DOM-IGNORE-END


// *****************************************************************************
/* Macro:
    SYS_MESSAGE( const char* message )

  Summary:
    Prints a message to the console regardless of the system error level.

  Description:
    This macro is used to print a message to the console regardless of the
    system error level.  It can be mapped to any desired implementation.

  Precondition:
    If mapped to the SYS_DEBUG_Message function, then the system debug service must
    be initialized and running.

  Parameters:
    message         - Pointer to a buffer containing the message string to be
                      displayed.

  Returns:
    None.

  Example:
    <code>
    // In configuration.h file: #define SYS_DEBUG_USE_CONSOLE
    // In sys_debug.h file: #define SYS_MESSAGE(message) SYS_DEBUG_Message(message)

    // In source code
    SYS_MESSAGE("My Message\r\n");
    </code>

  Remarks:
    By default, this macro is defined as nothing, effectively removing all code
    generated by calls to it.  To process SYS_MESSAGE calls, this macro must be
    defined in a way that maps calls to it to the desired implementation (see
    example, above).

    This macro can be mapped to the system console service (along with other
    system debug macros) by defining SYS_DEBUG_USE_CONSOLE in the system
    configuration (configuration.h) instead of defining it individually.
*/

#ifndef SYS_MESSAGE
    #define SYS_MESSAGE(message)
#endif


// *****************************************************************************
/* Function:
    SYS_DEBUG_MESSAGE(SYS_ERROR_LEVEL level, const char* message )

  Summary:
    Prints a debug message if the system error level is defined at
    or lower than the level specified.

  Description:
    This function prints a debug message if the system error level is defined at
    or lower than the level specified. If mapped to the SYS_DEBUG_Message function,
    then the system debug service must be initialized and running.

  Precondition:
    If mapped to the SYS_DEBUG_Message function, then the system debug service must be
    initialized and running.

  Parameters:
    level           - The current error level threshold for displaying the message.
    message         - Pointer to a buffer containing the message to be displayed.

  Returns:
    None.

  Example:
    <code>
    // In configuration.h file: #define SYS_DEBUG_USE_CONSOLE
    // In sys_debug.h file: #define SYS_DEBUG_MESSAGE(level, message) _SYS_DEBUG_MESSAGE(level, message)

    SYS_DEBUG_ErrorLevelSet(SYS_ERROR_DEBUG);
    SYS_DEBUG_MESSAGE(SYS_ERROR_WARNING, "System Debug Message \r\n");

    </code>

  Remarks:
    By default, this macro is defined as nothing, effectively removing all code
    generated by calls to it.  To process SYS_DEBUG_MESSAGE calls, this macro
    must be defined in a way that maps calls to it to the desired implementation
    (see example, above).

    This macro can be mapped to the system console service (along with other
    system debug macros) by defining SYS_DEBUG_USE_CONSOLE in the system
    configuration (configuration.h) instead of defining it individually.
*/

#ifndef SYS_DEBUG_MESSAGE
    #define SYS_DEBUG_MESSAGE(level,message)
#endif


// *****************************************************************************
/* Function:
    SYS_PRINT(const char* format, ...)

  Summary:
    Formats and prints an error message with a variable number of arguments
    regardless of the system error level.

  Description:
    This function formats and prints an error message with a variable number of
    arguments regardless of the system error level.

  Precondition:
    If mapped to the SYS_DEBUG_Print function, then the system debug service must
    be initialized and running.

  Parameters:
    format          - Pointer to a buffer containing the format string for
                      the message to be displayed.
    ...             - Zero or more optional parameters to be formated as
                      defined by the format string.

  Returns:
    None.

  Example:
    <code>
    // In configuration.h file: #define SYS_DEBUG_USE_CONSOLE
    // In sys_debug.h file: #define SYS_PRINT(fmt, ...) SYS_DEBUG_Print(fmt, ##__VA_ARGS__)

    // In source code
    int result;

    result = SomeOperation();
    if (result > MAX_VALUE)
    {
        SYS_PRINT("Result of %d exceeds max value\r\n", result);
        // Take appropriate action
    }
    </code>

  Remarks:
    The format string and arguments follow the printf convention.

    By default, this macro is defined as nothing, effectively removing all code
    generated by calls to it.  To process SYS_PRINT calls, this macro must be
    defined in a way that maps calls to it to the desired implementation (see
    example, above).

    This macro can be mapped to the system console service (along with other
    system debug macros) by defining SYS_DEBUG_USE_CONSOLE in the system
    configuration (configuration.h) instead of defining it individually.
*/

#ifndef SYS_PRINT
    #define SYS_PRINT(fmt, ...)
#endif


// *****************************************************************************
/* Macro:
    SYS_DEBUG_PRINT( SYS_ERROR_LEVEL level, const char* format, ... )

  Summary:
    Formats and prints an error message if the system error level is defined at
    or lower than the level specified.

  Description:
    This macro formats and prints an error message if the system error level
    is defined at or lower than the level specified.

  Precondition:
    If mapped to the SYS_DEBUG_Print function, then the system debug service must
    be initialized and running.

  Parameters:
    level           - The current error level threshold for displaying the message.
    format          - Pointer to a buffer containing the format string for
                      the message to be displayed.
    ...             - Zero or more optional parameters to be formated as
                      defined by the format string.

  Returns:
    None.

  Example:
    <code>
    // In configuration.h file: #define SYS_DEBUG_USE_CONSOLE
    // In sys_debug.h: #define SYS_DEBUG_PRINT(level, fmt, ...) _SYS_DEBUG_PRINT(level, fmt, ##__VA_ARGS__)

    // In source code
    int result;

    result = SomeOperation();
    if (result > MAX_VALUE)
    {
        SYS_DEBUG_PRINT(SYS_ERROR_WARNING, "Result of %d exceeds max value\r\n", result);
        // Take appropriate action
    }
    </code>

  Remarks:
    The format string and arguments follow the printf convention.

    By default, this macro is defined as nothing, effectively removing all code
    generated by calls to it.  To process SYS_DEBUG_PRINT calls, this macro must be
    defined in a way that maps calls to it to the desired implementation (see
    example, above).

    This macro can be mapped to the system console service (along with other
    system debug macros) by defining SYS_DEBUG_USE_CONSOLE in the system
    configuration (configuration.h) instead of defining it individually.
*/

#ifndef SYS_DEBUG_PRINT
    #define SYS_DEBUG_PRINT(level, fmt, ...)
#endif


// *****************************************************************************
/* Macro:
    SYS_DEBUG_BreakPoint( void )

  Summary:
    Inserts a software breakpoint instruction when building in Debug mode.

  Description:
    This macro inserts a software breakpoint instruction when building in Debug
    mode.

  Precondition:
    None.

  Parameters:
    None.

  Returns:
    None.

  Example:
    <code>
    if (myDebugTestFailed)
    {
        SYS_DEBUG_BreakPoint();
    }
    </code>

  Remarks:
    Compiles out if not built for debugging.
*/


#if defined(__DEBUG)
    #define SYS_DEBUG_BreakPoint()  __asm__ volatile (" sdbbp 0")
#else
    #define SYS_DEBUG_BreakPoint()
#endif


// *****************************************************************************
// *****************************************************************************
// Section: SYS DEBUG Deprecated Macros
// *****************************************************************************
// *****************************************************************************
/* Do not use these macros for new development.
*/

//DOM-IGNORE-BEGIN
#ifndef SYS_DEBUG
    #define SYS_DEBUG(level,message)    SYS_DEBUG_MESSAGE(level,message)
#endif

#ifndef SYS_ERROR
    #define SYS_ERROR(level,fmt, ...)   SYS_DEBUG_PRINT(level,fmt, ##__VA_ARGS__)
#endif

#ifndef SYS_ERROR_PRINT
    #define SYS_ERROR_PRINT(level,fmt, ...)   SYS_DEBUG_PRINT(level,fmt, ##__VA_ARGS__)
#endif
//DOM-IGNORE-END


//DOM-IGNORE-BEGIN
#ifdef __cplusplus

    }

#endif
//DOM-IGNORE-END

#endif // SYS_DEBUG_H
