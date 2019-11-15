/*******************************************************************************
  Console System Service Implementation

  Company:
    Microchip Technology Inc.

  File Name:
    sys_console.c

  Summary:
    Console System Service interface implementation.

  Description:
    The CONSOLE system service provides a simple interface to manage the CONSOLE
    module on Microchip microcontrollers. This file Implements the core
    interface routines for the CONSOLE system service. While building the system
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
// Section: Macro Definitions
// *****************************************************************************
// *****************************************************************************

// *****************************************************************************
// *****************************************************************************
// Section: Included Files
// *****************************************************************************
// *****************************************************************************

#include "system/console/sys_console.h"

static SYS_CONSOLE_OBJECT_INSTANCE consoleDeviceInstance[SYS_CONSOLE_DEVICE_MAX_INSTANCES];

SYS_MODULE_OBJ SYS_CONSOLE_Initialize(
    const SYS_MODULE_INDEX index,
    const SYS_MODULE_INIT* const init
)
{
    const SYS_CONSOLE_INIT* initConfig = (const SYS_CONSOLE_INIT* )init;
    SYS_CONSOLE_OBJECT_INSTANCE* pConsoleObj;

    /* Confirm valid arguments */
    if (index >= SYS_CONSOLE_DEVICE_MAX_INSTANCES || init == NULL)
    {
        return SYS_MODULE_OBJ_INVALID;
    }

    pConsoleObj = &consoleDeviceInstance[index];

    if ((pConsoleObj->status == SYS_STATUS_UNINITIALIZED) && initConfig)
    {
        pConsoleObj->devIndex = initConfig->deviceIndex;
        pConsoleObj->devDesc = initConfig->consDevDesc;
        pConsoleObj->status = SYS_STATUS_READY;
        pConsoleObj->devDesc->init( pConsoleObj->devIndex, initConfig->deviceInitData);

        return index;
    }

    return SYS_MODULE_OBJ_INVALID;
}

SYS_STATUS SYS_CONSOLE_Status ( SYS_MODULE_OBJ object )
{
    SYS_STATUS ret = SYS_STATUS_ERROR;
    SYS_CONSOLE_STATUS status;
    SYS_MODULE_INDEX index = (SYS_MODULE_INDEX)object;
    SYS_CONSOLE_OBJECT_INSTANCE* pConsoleObj;

    if (index >= SYS_CONSOLE_DEVICE_MAX_INSTANCES)
    {
        return SYS_STATUS_ERROR;
    }

    pConsoleObj = &consoleDeviceInstance[index];

    if (pConsoleObj->devDesc == NULL)
    {
        return SYS_STATUS_UNINITIALIZED;
    }

    status = pConsoleObj->devDesc->status(pConsoleObj->devIndex);

    switch (status)
    {
        case SYS_CONSOLE_STATUS_NOT_CONFIGURED:
            ret = SYS_STATUS_UNINITIALIZED;
            break;
        case SYS_CONSOLE_STATUS_CONFIGURED:
            ret = SYS_STATUS_READY;
            break;
        case SYS_CONSOLE_STATUS_BUSY:
            ret = SYS_STATUS_BUSY;
            break;
        case SYS_CONSOLE_STATUS_ERROR:
            ret = SYS_STATUS_ERROR;
            break;
        default:
            break;
    }

    return ret;
}

void SYS_CONSOLE_Tasks ( SYS_MODULE_OBJ object )
{
    SYS_CONSOLE_OBJECT_INSTANCE* pConsoleObj = &consoleDeviceInstance[(SYS_MODULE_INDEX)object];

    if (pConsoleObj->devDesc == NULL)
    {
        return;
    }

    pConsoleObj->devDesc->task(pConsoleObj->devIndex, object);
}

ssize_t SYS_CONSOLE_Read(
    const SYS_MODULE_INDEX index,
    int fd,
    void* buf,
    size_t count
)
{
    SYS_CONSOLE_OBJECT_INSTANCE* pConsoleObj = &consoleDeviceInstance[index];

    if (pConsoleObj->status == SYS_STATUS_UNINITIALIZED
            || pConsoleObj->devDesc == NULL)
    {
        return 0;
    }

    return pConsoleObj->devDesc->read(pConsoleObj->devIndex, fd, buf, count);
}

ssize_t SYS_CONSOLE_Write(
    const SYS_MODULE_INDEX index,
    int fd,
    const void* buf,
    size_t count
)
{
    SYS_CONSOLE_OBJECT_INSTANCE* pConsoleObj;

    if (index >= SYS_CONSOLE_DEVICE_MAX_INSTANCES)
    {
        return 0;
    }

    pConsoleObj = &consoleDeviceInstance[index];

    if ((pConsoleObj->status == SYS_STATUS_UNINITIALIZED) || (pConsoleObj->devDesc == NULL))
    {
        return 0;
    }

    return pConsoleObj->devDesc->write(pConsoleObj->devIndex, fd, buf, count);
}

void SYS_CONSOLE_RegisterCallback(
    const SYS_MODULE_INDEX index,
    SYS_CONSOLE_CALLBACK cbFunc,
    SYS_CONSOLE_EVENT event
)
{
    SYS_CONSOLE_OBJECT_INSTANCE* pConsoleObj = &consoleDeviceInstance[index];

    if (pConsoleObj->devDesc == NULL)
    {
        return;
    }

    pConsoleObj->devDesc->callbackRegister(pConsoleObj->devIndex, cbFunc, event);
}

void SYS_CONSOLE_Flush(const SYS_MODULE_INDEX index)
{
    SYS_CONSOLE_OBJECT_INSTANCE* pConsoleObj = &consoleDeviceInstance[index];

    if (pConsoleObj->devDesc == NULL)
    {
        return;
    }

    pConsoleObj->devDesc->flush(pConsoleObj->devIndex);
}


/*******************************************************************************
 End of File
*/
