/*******************************************************************************
  I2C Driver Interface Declarations for Static Single Instance Driver

  Company:
    Microchip Technology Inc.

  File Name:
    drv_i2c_static_buffer_model.h

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

#ifndef _DRV_I2C_STATIC_BUFFER_MODEL_H
#define _DRV_I2C_STATIC_BUFFER_MODEL_H

//DOM-IGNORE-END

// DOM-IGNORE-BEGIN
#ifdef __cplusplus  

    extern "C" {

#endif
// DOM-IGNORE-END 

#include "peripheral/i2c/plib_i2c.h"
#include "driver/i2c/src/drv_i2c_local.h"


<#macro DRV_I2C_STATIC_API DRV_INSTANCE MODE>
// *****************************************************************************
// *****************************************************************************
// Section: Interface Headers for Instance ${DRV_INSTANCE} for the static driver
// *****************************************************************************
// *****************************************************************************

void DRV_I2C${DRV_INSTANCE}_Initialize(void);

void DRV_I2C${DRV_INSTANCE}_DeInitialize(void);

void DRV_I2C${DRV_INSTANCE}_Tasks(void);


// *****************************************************************************
// Section: Instance ${DRV_INSTANCE} Buffer Transfer function (Master/Slave)
// *****************************************************************************

DRV_I2C_BUFFER_HANDLE DRV_I2C${DRV_INSTANCE}_Transmit ( uint16_t deviceaddress,
                                                        void *txBuffer,
                                                        size_t size,
                                                        void * context);


DRV_I2C_BUFFER_HANDLE DRV_I2C${DRV_INSTANCE}_Receive (  uint16_t deviceaddress,
                                                        void *rxBuffer,
                                                        size_t size,
                                                        void * context); 
                                    
DRV_I2C_BUFFER_HANDLE DRV_I2C${DRV_INSTANCE}_TransmitThenReceive (  uint16_t deviceaddress,
                                                                    void *txBuffer,
                                                                    size_t wsize,
                                                                    void *rxBuffer,
                                                                    size_t size,
                                                                    void * context); 
                                                                   
DRV_I2C_BUFFER_HANDLE DRV_I2C${DRV_INSTANCE}_TransmitForced (   uint16_t deviceaddress,
                                                                void *txBuffer,
                                                                size_t size,
                                                                DRV_I2C_BUS_ERROR_EVENT eventFlag,
                                                                void * context);

<#if MODE == "DRV_I2C_MODE_MASTER">
// *****************************************************************************
// Section: Instance ${DRV_INSTANCE} Buffer status function
// *****************************************************************************

DRV_I2C_BUFFER_EVENT DRV_I2C${DRV_INSTANCE}_TransferStatusGet ( DRV_I2C_BUFFER_HANDLE bufferHandle );

<#if MODE == "DRV_I2C_MODE_MASTER" && CONFIG_PIC32MZ == true && CONFIG_DEVICE?contains("EC") >
bool _DRV_I2C${DRV_INSTANCE}_MasterStop(void);

</#if>
</#if>

// *****************************************************************************
// Section: Instance ${DRV_INSTANCE} Number of Bytes transferred
// *****************************************************************************

uint32_t DRV_I2C${DRV_INSTANCE}_BytesTransferred ( DRV_I2C_BUFFER_HANDLE bufferHandle );


// *****************************************************************************
// Section: Buffer Event Callback function 
// *****************************************************************************

void DRV_I2C${DRV_INSTANCE}_BufferEventHandlerSet (const DRV_I2C_BUFFER_EVENT_HANDLER eventHandler,
                    const uintptr_t context );

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

#endif // #ifndef _DRV_I2C_STATIC_BUFFER_MODEL_H

/*******************************************************************************
 End of File
*/
