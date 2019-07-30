/*******************************************************************************
  I2C Driver Interface Declarations for Static Single Instance Driver

  Company:
    Microchip Technology Inc.

  File Name:
    drv_i2c_static.h

  Summary:
    I2C driver interface declarations for the static single instance driver.

  Description:
    The I2C device driver provides a simple interface to manage the I2C
    modules on Microchip microcontrollers. This file defines the interface
    Declarations for the I2C driver.
    
  Remarks:
    Static interfaces incorporate the driver instance number within the names
    of the routines, eliminating the need for an object ID or object handle.
    
    Static single-open interfaces also eliminate the need for the open handle.
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

#ifndef _DRV_I2C_STATIC_H
#define _DRV_I2C_STATIC_H

// DOM-IGNORE-BEGIN
#ifdef __cplusplus  

    extern "C" {

#endif
// DOM-IGNORE-END  

//DOM-IGNORE-END
#include <stdbool.h>
#include "system_config.h"
#include "peripheral/i2c/plib_i2c.h"
<#if CONFIG_DRV_I2C_INTERRUPT_MODE == true>
#include "peripheral/int/plib_int.h"
</#if>
#include "system/clk/sys_clk.h"
#include "peripheral/ports/plib_ports.h"

<#macro DRV_I2C_STATIC_API DRV_INSTANCE MODE>
// *****************************************************************************
// *****************************************************************************
// Section: Interface Headers for Instance ${DRV_INSTANCE} for the static driver
// *****************************************************************************
// *****************************************************************************
void DRV_I2C${DRV_INSTANCE}_Initialize(void);

void DRV_I2C${DRV_INSTANCE}_DeInitialize(void);

// *****************************************************************************
// Section: Instance ${DRV_INSTANCE} Byte transfer functions (Master/Slave)
// *****************************************************************************
bool DRV_I2C${DRV_INSTANCE}_SetUpByteRead(void);

bool DRV_I2C${DRV_INSTANCE}_WaitForReadByteAvailable(void);

uint8_t DRV_I2C${DRV_INSTANCE}_ByteRead(void);

bool DRV_I2C${DRV_INSTANCE}_ByteWrite(const uint8_t byte);

bool DRV_I2C${DRV_INSTANCE}_WaitForByteWriteToComplete(void);

bool DRV_I2C${DRV_INSTANCE}_WriteByteAcknowledged(void);

bool DRV_I2C${DRV_INSTANCE}_ReceiverBufferIsEmpty(void);

<#if MODE == "DRV_I2C_MODE_MASTER">

// *****************************************************************************
// Section: Instance ${DRV_INSTANCE} I2C Master functions
// *****************************************************************************
void DRV_I2C${DRV_INSTANCE}_BaudRateSet(I2C_BAUD_RATE baudRate);

bool DRV_I2C${DRV_INSTANCE}_MasterBusIdle(void);

bool DRV_I2C${DRV_INSTANCE}_MasterStart(void);

bool DRV_I2C${DRV_INSTANCE}_MasterRestart(void);

bool DRV_I2C${DRV_INSTANCE}_WaitForStartComplete(void);

bool DRV_I2C${DRV_INSTANCE}_MasterStop(void);

bool DRV_I2C${DRV_INSTANCE}_WaitForStopComplete(void);

void DRV_I2C${DRV_INSTANCE}_MasterACKSend(void);

void DRV_I2C${DRV_INSTANCE}_MasterNACKSend(void);

bool DRV_I2C${DRV_INSTANCE}_WaitForACKOrNACKComplete(void);

</#if>

</#macro>

<#if CONFIG_DRV_I2C_INST_IDX0 == true>
<@DRV_I2C_STATIC_API DRV_INSTANCE="0"
MODE=CONFIG_DRV_I2C_STATIC_OPERATION_MODE_IDX0
/>
</#if>
<#if CONFIG_DRV_I2C_INST_IDX1 == true>
<@DRV_I2C_STATIC_API DRV_INSTANCE="1"
MODE=CONFIG_DRV_I2C_STATIC_OPERATION_MODE_IDX1
/>
</#if>
<#if CONFIG_DRV_I2C_INST_IDX2 == true>
<@DRV_I2C_STATIC_API DRV_INSTANCE="2"
MODE=CONFIG_DRV_I2C_STATIC_OPERATION_MODE_IDX2
/>
</#if>
<#if CONFIG_DRV_I2C_INST_IDX3 == true>
<@DRV_I2C_STATIC_API DRV_INSTANCE="3"
MODE=CONFIG_DRV_I2C_STATIC_OPERATION_MODE_IDX3
/>
</#if>
<#if CONFIG_DRV_I2C_INST_IDX4 == true>
<@DRV_I2C_STATIC_API DRV_INSTANCE="4"
MODE=CONFIG_DRV_I2C_STATIC_OPERATION_MODE_IDX4
/>
</#if>

// DOM-IGNORE-BEGIN
#ifdef __cplusplus
}
#endif
// DOM-IGNORE-END

#endif // #ifndef _DRV_I2C_STATIC_H

/*******************************************************************************
 End of File
*/
