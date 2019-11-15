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

#ifndef SYS_DEBUG_LOCAL_H
#define SYS_DEBUG_LOCAL_H

// *****************************************************************************
// *****************************************************************************
// Section: File includes
// *****************************************************************************
// *****************************************************************************

#include "configuration.h"
#include "driver/driver.h"
#include "system/debug/sys_debug.h"

// DOM-IGNORE-BEGIN
#ifdef __cplusplus  // Provide C++ Compatibility

    extern "C" {

#endif
// DOM-IGNORE-END

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

//DOM-IGNORE-BEGIN
#ifdef __cplusplus

    }

#endif
//DOM-IGNORE-END

#endif //#ifndef SYS_DEBUG_LOCAL_H
