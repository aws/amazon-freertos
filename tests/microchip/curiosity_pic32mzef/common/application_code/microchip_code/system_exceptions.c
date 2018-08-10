/*******************************************************************************
  MPLAB Harmony Exceptions Source File

  File Name:
    system_exceptions.c

  Summary:
    This file contains a function which overrides the deafult _weak_ exception 
    handler provided by the XC32 compiler.

  Description:
    This file redefines the default _weak_  exception handler with a more debug
    friendly one. If an unexpected exception occurs the code will stop in a
    while(1) loop.  The debugger can be halted and two variables _excep_code and
    _except_addr can be examined to determine the cause and address where the
    exception occured.
 *******************************************************************************/

// DOM-IGNORE-BEGIN
/*******************************************************************************
Copyright 2017 Microchip Technology Incorporated and its subsidiaries.

Permission is hereby granted, free of charge, to any person obtaining a copy of 
this software and associated documentation files (the "Software"), to deal in 
the Software without restriction, including without limitation the rights to 
use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies 
of the Software, and to permit persons to whom the Software is furnished to do 
so, subject to the following conditions:
The above copyright notice and this permission notice shall be included in all 
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR 
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, 
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE 
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER 
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, 
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE 
SOFTWARE
 *******************************************************************************/
// DOM-IGNORE-END


#include <xc.h>                 /* Defines special funciton registers, CP0 regs  */
#include "system_config.h"
#include "system_definitions.h"
#include "system/debug/sys_debug.h"

#include "FreeRTOS.h"
#include "task.h"


// *****************************************************************************
// *****************************************************************************
// Section: Global Data Definitions
// *****************************************************************************
// *****************************************************************************

/*******************************************************************************
  Exception Reason Data
  
  <editor-fold defaultstate="expanded" desc="Exception Reason Data">
  
  Remarks:
    These global static items are used instead of local variables in the 
    _general_exception_handler function because the stack may not be available
    if an exception has occured.
*/

/* Code identifying the cause of the exception (CP0 Cause register). */
static unsigned int _excep_code;

/* Address of instruction that caused the exception. */
static unsigned int _excep_addr;

/* Pointer to the string describing the cause of the exception. */
static char *_cause_str;

/* Array identifying the cause (indexed by _exception_code). */
static char *cause[] = 
{
    "Interrupt",
    "Undefined",
    "Undefined",
    "Undefined",
    "Load/fetch address error",
    "Store address error",
    "Instruction bus error",
    "Data bus error",
    "Syscall",
    "Breakpoint",
    "Reserved instruction",
    "Coprocessor unusable",
    "Arithmetic overflow",
    "Trap",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved"
};

// </editor-fold>


// *****************************************************************************
// *****************************************************************************
// Section: Exception Handling
// *****************************************************************************
// *****************************************************************************

/*******************************************************************************
  Function:
    void _general_exception_handler ( void )

  Summary:
    Overrides the XC32 _weak_ _generic_exception_handler.
    
  Description:
    This function overrides the XC32 default _weak_ _generic_exception_handler.

  Remarks:
    Refer to the XC32 User's Guide for additional information.
 */

void _general_exception_handler ( void )
{
    /* Mask off Mask of the ExcCode Field from the Cause Register
    Refer to the MIPs Software User's manual */
    _excep_code = (_CP0_GET_CAUSE() & 0x0000007C) >> 2;
    _excep_addr = _CP0_GET_EPC();
    _cause_str  = cause[_excep_code];

    SYS_DEBUG_PRINT(SYS_ERROR_ERROR, "\n\rGeneral Exception %s (cause=%d, addr=%x).\n\r", 
                    _cause_str, _excep_code, _excep_addr);
    configPRINTF( ("\n\rGeneral Exception %s (cause=%d, addr=%x).\n\r", 
                        _cause_str, _excep_code, _excep_addr) );
    while (1)
    {
        vTaskDelay( pdMS_TO_TICKS( 1000 ) );
        SYS_DEBUG_BreakPoint();
    }
}

/*******************************************************************************
 End of File
*/
