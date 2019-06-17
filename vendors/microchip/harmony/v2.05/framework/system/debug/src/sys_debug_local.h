/*******************************************************************************
  Debug System Service Local Data Structures

  Company:
    Microchip Technology Inc.

  File Name:
    sys_debug_local.h

  Summary:
    Debug System Service local declarations and definitions.

  Description:
    This file contains the Debug System Service local declarations and definitions.
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


#ifndef _SYS_DEBUG_LOCAL_H
#define _SYS_DEBUG_LOCAL_H


// *****************************************************************************
// *****************************************************************************
// Section: File includes
// *****************************************************************************
// *****************************************************************************

#include "driver/driver_common.h"
#include "system_config.h"
#include "system/debug/sys_debug.h"


// *****************************************************************************
// *****************************************************************************
// Section: Data Type Definitions
// *****************************************************************************
// *****************************************************************************


// *****************************************************************************
/* SYS Debug State Machine States

   Summary
    Defines the various states that can be achieved by the driver operation.

   Description
    This enumeration defines the various states that can be achieved by the 
    driver operation.

   Remarks:
    None.
*/

typedef enum
{
    SYS_DEBUG_STATE_IDLE

} SYS_DEBUG_STATES;


// *****************************************************************************
/* SYS DEBUG OBJECT INSTANCE structure

  Summary:
    System Debug object instance structure.

  Description:
    This data type defines the System Debug object instance.

  Remarks:
    None.
*/

typedef struct
{
    SYS_STATUS                        status;
    SYS_DEBUG_STATES                  state;
    SYS_MODULE_INDEX                  debugConsole;
    int                               prtBufPtr;
} SYS_DEBUG_INSTANCE;


// *****************************************************************************
// *****************************************************************************
// Section: Extern data Definitions
// *****************************************************************************
// *****************************************************************************


#endif //#ifndef _SYS_DEBUG_LOCAL_H

/*******************************************************************************
 End of File
*/

