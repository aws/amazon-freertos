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

// *****************************************************************************
// *****************************************************************************
// Section: Included Files
// *****************************************************************************
// *****************************************************************************

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include "configuration.h"
#include "system/system.h"
#include "system/console/sys_console.h"
#include "sys_debug_local.h"

SYS_DEBUG_INSTANCE sysDebugInstance;

SYS_ERROR_LEVEL gblErrLvl;

static char printBuffer[SYS_DEBUG_PRINT_BUFFER_SIZE] SYS_DEBUG_BUFFER_DMA_READY;
static char tmpBuf[SYS_DEBUG_PRINT_BUFFER_SIZE] SYS_DEBUG_BUFFER_DMA_READY;

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

SYS_MODULE_OBJ SYS_DEBUG_Initialize(
    const SYS_MODULE_INDEX index,
    const SYS_MODULE_INIT* const init
)
{
    SYS_DEBUG_INIT* initConfig = (SYS_DEBUG_INIT*)init;

    gblErrLvl = initConfig->errorLevel;

    sysDebugInstance.debugConsole = initConfig->consoleIndex;
    sysDebugInstance.state = SYS_DEBUG_STATE_IDLE;
    sysDebugInstance.status = SYS_STATUS_READY;
    sysDebugInstance.prtBufPtr = 0;

    return SYS_MODULE_OBJ_STATIC;
}

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

SYS_STATUS SYS_DEBUG_Status ( SYS_MODULE_OBJ object )
{
    return ( sysDebugInstance.status );
}

void SYS_DEBUG_Message(const char *message)
{
    SYS_CONSOLE_Write(sysDebugInstance.debugConsole, STDOUT_FILENO, message, strlen(message));
}

void SYS_DEBUG_Print(const char *format, ...)
{
    size_t len = 0;
    size_t padding = 0;
    va_list args = {0};

    va_start( args, format );

    len = vsnprintf(tmpBuf, SYS_DEBUG_PRINT_BUFFER_SIZE, format, args);

    va_end( args );

    if (len > 0 && len < SYS_DEBUG_PRINT_BUFFER_SIZE)
    {
        tmpBuf[len] = '\0';

        if (len + sysDebugInstance.prtBufPtr >= SYS_DEBUG_PRINT_BUFFER_SIZE)
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

void SYS_DEBUG_ErrorLevelSet(SYS_ERROR_LEVEL level)
{
    gblErrLvl = level;
}

SYS_ERROR_LEVEL SYS_DEBUG_ErrorLevelGet(void)
{
    return gblErrLvl;
}
