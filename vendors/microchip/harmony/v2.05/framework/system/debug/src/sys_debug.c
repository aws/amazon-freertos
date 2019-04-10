/*******************************************************************************
  Debug System Service Implementation

  Company:
    Microchip Technology Inc.

  File Name:
    sys_debug.c

  Summary:
    Debug System Service interface implementation.

  Description:
    The DEBUG system service provides a simple interface to manage the DEBUG 
    module on Microchip microcontrollers. This file Implements the core
    interface routines for the DEBUG system service. While building the system
    service from source, ALWAYS include this file in the build.
*******************************************************************************/

//DOM-IGNORE-BEGIN
/*******************************************************************************
Copyright (c) 2013 released Microchip Technology Inc.  All rights reserved.

Microchip licenses to you the right to use, modify, copy and distribute
Software only when embedded on a Microchip microcontroller or digital signal
controller that is integrated into your product or third party product
(pursuant to the sublicense terms in the accompanying license agreement).

You should refer to the license agreement accompanying this Software for
additional information regarding your rights and obligations.

SOFTWARE AND DOCUMENTATION ARE PROVIDED AS IS WITHOUT WARRANTY OF ANY KIND,
EITHER EXPRESS OR IMPLIED, INCLUDING WITHOUT LIMITATION, ANY WARRANTY OF
MERCHANTABILITY, TITLE, NON-INFRINGEMENT AND FITNESS FOR A PARTICULAR PURPOSE.
IN NO EVENT SHALL MICROCHIP OR ITS LICENSORS BE LIABLE OR OBLIGATED UNDER
CONTRACT, NEGLIGENCE, STRICT LIABILITY, CONTRIBUTION, BREACH OF WARRANTY, OR
OTHER LEGAL EQUITABLE THEORY ANY DIRECT OR INDIRECT DAMAGES OR EXPENSES
INCLUDING BUT NOT LIMITED TO ANY INCIDENTAL, SPECIAL, INDIRECT, PUNITIVE OR
CONSEQUENTIAL DAMAGES, LOST PROFITS OR LOST DATA, COST OF PROCUREMENT OF
SUBSTITUTE GOODS, TECHNOLOGY, SERVICES, OR ANY CLAIMS BY THIRD PARTIES
(INCLUDING BUT NOT LIMITED TO ANY DEFENSE THEREOF), OR OTHER SIMILAR COSTS.
*******************************************************************************/
//DOM-IGNORE-END
// *****************************************************************************
// *****************************************************************************
// Section: Macro Definitions
// *****************************************************************************
// *****************************************************************************

// *****************************************************************************
// *****************************************************************************
// Section: Included Files
// *****************************************************************************
// *****************************************************************************

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#if defined (__PIC32C__)
#else
#include <sys/appio.h>
#endif
#include "system_config.h"
#include "system/system.h"
#include "system/console/sys_console.h"
#include "system/debug/src/sys_debug_local.h"
#include <stdarg.h>

SYS_DEBUG_INSTANCE sysDebugInstance;

SYS_ERROR_LEVEL gblErrLvl;

static char printBuffer[DEBUG_PRINT_BUFFER_SIZE] SYS_DEBUG_BUFFER_DMA_READY;
static char tmpBuf[DEBUG_PRINT_BUFFER_SIZE] SYS_DEBUG_BUFFER_DMA_READY;

/*
char *errlvl[] =
{
    "SYS_ERROR_FATAL",
    "SYS_ERROR_ERROR",
    "SYS_ERROR_WARNING",
    "SYS_ERROR_INFO",
    "SYS_ERROR_DEBUG"
};
*/


// *****************************************************************************
/* Function:
    SYS_MODULE_OBJ SYS_DEBUG_Initialize( const SYS_MODULE_INDEX index,
                                       const SYS_MODULE_INIT * const init )

  Summary:
    Initializes data for the instance of the Console module and opens the
    specific module instance.

  Description:
    This function initializes the Console module, and selects the I/O device to
    be used. It also initializes any internal data structures.

  Precondition:
    None.

  Parameters:
    index           - Index for the instance to be initialized

    init            - Pointer to a data structure containing any data necessary
                      to initialize the sys timer. This pointer may be null if no
                      data is required because static overrides have been
                      provided.

 Returns:
    If successful, returns a valid handle to an object.  Otherwise, it
    returns SYS_MODULE_OBJ_INVALID. The returned object must be passed as
    argument to SYS_DEBUG_Reinitialize, SYS_DEBUG_Deinitialize,
    SYS_DEBUG_Tasks and SYS_DEBUG_Status routines.

  Example:
    <code>
    </code>

  Remarks:
    This routine should only be called once during system initialization
    unless SYS_DEBUG_Deinitialize is first called to deinitialize the device
    instance before reinitializing it. If the system was already initialized
    it safely returns without causing any disturbance.
*/

SYS_MODULE_OBJ SYS_DEBUG_Initialize(const SYS_MODULE_INDEX index, const SYS_MODULE_INIT * const init )
{
    SYS_DEBUG_INIT *initConfig = (SYS_DEBUG_INIT*)init;

    gblErrLvl = initConfig->errorLevel;

    sysDebugInstance.debugConsole = initConfig->consoleIndex;
    sysDebugInstance.state = SYS_DEBUG_STATE_IDLE;
    sysDebugInstance.status = SYS_STATUS_READY;
    sysDebugInstance.prtBufPtr = 0;

    return SYS_MODULE_OBJ_STATIC;
}


// *****************************************************************************
/* Function:
     void SYS_DEBUG_Reinitialize ( SYS_MODULE_OBJ object,
                                 const SYS_MODULE_INIT * const init )

   Summary:
     Reinitializes and refreshes the data structure for the instance of the
     Debug module.

   Description:
     This function reinitializes and refreshes the data structure for the
     instance of the Debug module.

  PreCondition:
     The SYS_DEBUG_Initialize function should have been called before calling
     this function.

  Parameters:
    object          - Identifies the SYS DEBUG Object returned by the Initialize
                      interface
    init            - Pointer to the data structure containing any data
                      necessary to initialize the hardware

   Returns:
     None.
*/

void SYS_DEBUG_Reinitialize( SYS_MODULE_OBJ object, const SYS_MODULE_INIT * const init )
{
    SYS_DEBUG_INIT *initConfig = (SYS_DEBUG_INIT*)init;

    gblErrLvl = initConfig->errorLevel;
    sysDebugInstance.debugConsole = initConfig->consoleIndex;
    sysDebugInstance.state = SYS_DEBUG_STATE_IDLE;
    sysDebugInstance.status = SYS_STATUS_READY;
    sysDebugInstance.prtBufPtr = 0;
}


// *****************************************************************************
/* Function:
    void SYS_DEBUG_Deinitialize ( SYS_MODULE_OBJ object )

  Summary:
    Deinitializes the specific module instance of the Debug module.

  Description:
    Deinitializes the specific module instance disabling its operation (and
    any hardware for driver modules). Resets all the internal data
    structures and fields for the specified instance to the default
    settings.

  PreCondition:
    The SYS_DEBUG_Initialize function should have been called before calling
    this function.

  Parameters:
    object    - SYS DEBUG object handle, returned from SYS_DEBUG_Initialize

  Returns:
    None.
*/

void SYS_DEBUG_Deinitialize ( SYS_MODULE_OBJ object )
{
    gblErrLvl = SYS_ERROR_INFO;
    sysDebugInstance.debugConsole = 0;
    sysDebugInstance.status = SYS_STATUS_UNINITIALIZED;
}


// *****************************************************************************
/* Function:
    void SYS_DEBUG_Tasks ( SYS_MODULE_OBJ object )

  Summary:
    Maintains the debug module's state machine and implements its ISR.

  Description:
    This routine is used to maintain the debug module's internal state machine
    and implement its ISR for interrupt-driven implementations.

  Precondition:
    The SYS_DEBUG_Initialize function must have been called for the specified
    console instance.

  Parameters:
    object   - SYS DEBUG object handle, returned from SYS_DEBUG_Initialize

  Returns:
    None.
*/

void SYS_DEBUG_Tasks(SYS_MODULE_OBJ object)
{
    switch (sysDebugInstance.state)
    {
        case SYS_DEBUG_STATE_IDLE:
            break;
        default:
            break;
    }

}


// *****************************************************************************
/* Function:
    SYS_STATUS SYS_DEBUG_Status ( SYS_MODULE_OBJ object )

  Summary:
    Returns status of the specific module instance of the Debug module.

  Description:
    This function returns the status of the specific module instance disabling its
    operation (and any hardware for driver modules).

  PreCondition:
    The SYS_DEBUG_Initialize function should have been called before calling
    this function.

  Parameters:
    object   - SYS DEBUG object handle, returned from SYS_CONSOLE_Initialize

  Returns:
    SYS_STATUS_READY    Indicates that any previous module operation for the
                        specified module has completed

    SYS_STATUS_BUSY     Indicates that a previous module operation for the
                        specified module has not yet completed

    SYS_STATUS_ERROR    Indicates that the specified module is in an error state
*/

SYS_STATUS SYS_DEBUG_Status ( SYS_MODULE_OBJ object )
{
    return ( sysDebugInstance.status );
}


// *****************************************************************************
/* Function:
   _SYS_MESSAGE(const char* message);

  Summary:
    Prints a debug message if the system error level is defined as
    SYS_ERROR_DEBUG.

  Description:
    This function prints a debug message if the system error level is defined as
    SYS_ERROR_DEBUG.

  Precondition:
    SYSTEM_CURRENT_ERROR_LEVEL must be defined as SYS_ERROR_DEBUG.

  Parameters:

    message       - Pointer to a buffer containing the message to be displayed.

  Returns:
    None.

  Remarks:
    Called by macros SYS_MESSAGE and SYS_DEBUG(message) if
    SYSTEM_CURRENT_ERROR_LEVEL is defined as SYS_ERROR_DEBUG.
*/

void SYS_DEBUG_Message(const char *message)
{
    SYS_CONSOLE_Write(sysDebugInstance.debugConsole, STDOUT_FILENO, message, strlen(message));
}


// *****************************************************************************
/* Function:
    _SYS_PRINT(const char* format, ...);

  Summary:
    Prints a formattedf message to the console.

  Description:
    This function prints a formatted message to the console.

  Precondition:
    None.

  Parameters:
    format         - C string that contains a format string that follows the
                     same specifications as format in printf.
   ...             - variable list of args.

  Returns:
    None.

  Remarks:
   Called by the macros SYS_PRINT and SYS_ERROR.
*/

void SYS_DEBUG_Print(const char *format, ...)
{
    size_t len = 0;
    size_t padding = 0;
    va_list args;

    va_start( args, format );

    len = vsnprintf(tmpBuf, DEBUG_PRINT_BUFFER_SIZE, format, args);

    va_end( args );

    if (len > 0 && len < DEBUG_PRINT_BUFFER_SIZE)
    {
        tmpBuf[len] = '\0';

        if (len + sysDebugInstance.prtBufPtr >= DEBUG_PRINT_BUFFER_SIZE)
        {
            sysDebugInstance.prtBufPtr = 0;
        }

        strcpy(&printBuffer[sysDebugInstance.prtBufPtr], tmpBuf);
        SYS_CONSOLE_Write(sysDebugInstance.debugConsole, STDOUT_FILENO, &printBuffer[sysDebugInstance.prtBufPtr], len);

        padding = len % 4;

        if (padding > 0 )
        {
            padding = 4 - padding;
        }

        sysDebugInstance.prtBufPtr += len + padding;
    }
}


// *****************************************************************************
/* Function:
    void SYS_ERROR_LevelSet(SYS_ERROR_LEVEL level);

  Summary:
    Sets the global System Error reporting level.

  Description:
    This function sets the global System Error reporting level.

  Precondition:
    None.

  Parameters:
    level           - The desired System Error Level.

  Returns:
    None.

  Remarks:
    None.
*/

void SYS_DEBUG_ErrorLevelSet(SYS_ERROR_LEVEL level)
{
    gblErrLvl = level;
}


// *****************************************************************************
/* Function:
   SYS_ERROR_LEVEL SYS_ERROR_LevelGet(void);

  Summary:
    Returns the global System Error reporting level.

  Description:
    This function returns the global System Error reporting level.

  Precondition:
    None.

  Parameters:
    None.

  Returns:
    The global System Error Level.

  Remarks:
    None.
*/

SYS_ERROR_LEVEL SYS_DEBUG_ErrorLevelGet(void)
{
    return gblErrLvl;
}


/*******************************************************************************
 End of File
*/

