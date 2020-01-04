/*******************************************************************************
  I2C Driver Interface Mapping Dynamic APIs to Static APIs           

  Company:
    Microchip Technology Inc.

  File Name:
    drv_i2c_mapping.h

  Summary:
    This file allows maintaining a single set of APIs for all I2C transactions  
    by making the type of implementation transparent to the application. In case
    where static implementation of I2C driver is selected, this file maps the 
    API functions to a particular driver instance-specific static implementation
    function, eliminating unnecessary dynamic parameters. 
*******************************************************************************/

//DOM-IGNORE-BEGIN
/*******************************************************************************
Copyright (c) 2015 released Microchip Technology Inc.  All rights reserved.

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

#include "driver/i2c/drv_i2c.h"
#include "driver/i2c/drv_i2c_static_buffer_model.h"


DRV_HANDLE DRV_I2C_Open ( const SYS_MODULE_INDEX index, const DRV_IO_INTENT ioIntent )
{
    switch (index)
    {
      <#if CONFIG_DRV_I2C_INST_IDX0 == true>
        case DRV_I2C_INDEX_0:
        {
            return (DRV_HANDLE)DRV_I2C_INDEX_0;
            break;
        }
      </#if>
      <#if CONFIG_DRV_I2C_INST_IDX1 == true>
        case DRV_I2C_INDEX_1:
        {
            return (DRV_HANDLE)DRV_I2C_INDEX_1;
            break;
        }
      </#if>
      <#if CONFIG_DRV_I2C_INST_IDX2 == true>
        case DRV_I2C_INDEX_2:
        {
            return (DRV_HANDLE)DRV_I2C_INDEX_2;
            break;
        }
      </#if>
      <#if CONFIG_DRV_I2C_INST_IDX3 == true>
        case DRV_I2C_INDEX_3:
        {
            return (DRV_HANDLE)DRV_I2C_INDEX_3;
            break;
        }
      </#if>
      <#if CONFIG_DRV_I2C_INST_IDX4 == true>
        case DRV_I2C_INDEX_4:
        {
            return (DRV_HANDLE)DRV_I2C_INDEX_4;
            break;
        }
      </#if>
        default:
        {
            break;
        }
    }
    
    return DRV_HANDLE_INVALID;
}

void DRV_I2C_Close ( DRV_HANDLE drvhandle )
{
    switch (drvhandle)
    {
      <#if CONFIG_DRV_I2C_INST_IDX0 == true>
        case DRV_I2C_INDEX_0:
        {
            DRV_I2C0_DeInitialize();
            break;
        }
      </#if>
      <#if CONFIG_DRV_I2C_INST_IDX1 == true>
        case DRV_I2C_INDEX_1:
        {
            DRV_I2C1_DeInitialize();
            break;
        }
      </#if>
      <#if CONFIG_DRV_I2C_INST_IDX2 == true>
        case DRV_I2C_INDEX_2:
        {
            DRV_I2C2_DeInitialize();
            break;
        }
      </#if>
      <#if CONFIG_DRV_I2C_INST_IDX3 == true>
        case DRV_I2C_INDEX_3:
        {
            DRV_I2C3_DeInitialize();
            break;
        }
      </#if>
      <#if CONFIG_DRV_I2C_INST_IDX4 == true>
        case DRV_I2C_INDEX_4:
        {
            DRV_I2C4_DeInitialize();
            break;
        }
      </#if>
        default:
        {
            break;
        }
    }
}


DRV_I2C_BUFFER_HANDLE DRV_I2C_Transmit( DRV_HANDLE drvhandle, uint16_t deviceaddress,
                                        void *txBuffer, size_t size, void * context)
{
    
    switch (drvhandle)
    {
      <#if CONFIG_DRV_I2C_INST_IDX0 == true>
        case DRV_I2C_INDEX_0:
        {
            return ( DRV_I2C0_Transmit (deviceaddress, txBuffer, size, context) );
            break;
        }
      </#if>
      <#if CONFIG_DRV_I2C_INST_IDX1 == true>
        case DRV_I2C_INDEX_1:
        {
            return ( DRV_I2C1_Transmit (deviceaddress, txBuffer, size, context) );
            break;
        }
      </#if>
      <#if CONFIG_DRV_I2C_INST_IDX2 == true>
        case DRV_I2C_INDEX_2:
        {
            return ( DRV_I2C2_Transmit (deviceaddress, txBuffer, size, context) );
            break;
        }
      </#if>
      <#if CONFIG_DRV_I2C_INST_IDX3 == true>
        case DRV_I2C_INDEX_3:
        {
            return ( DRV_I2C3_Transmit (deviceaddress, txBuffer, size, context) );
            break;
        }
      </#if>
      <#if CONFIG_DRV_I2C_INST_IDX4 == true>
        case DRV_I2C_INDEX_4:
        {
            return ( DRV_I2C4_Transmit (deviceaddress, txBuffer, size, context) );
            break;
        }
      </#if>
        default:
        {
            return DRV_I2C_BUFFER_HANDLE_INVALID;
        }
    }
    
    return DRV_I2C_BUFFER_HANDLE_INVALID;
}

DRV_I2C_BUFFER_HANDLE DRV_I2C_TransmitForced( DRV_HANDLE drvhandle, uint16_t deviceaddress,
                                              void *txBuffer, size_t size, DRV_I2C_BUS_ERROR_EVENT eventFlag, void * context)
{
    
    switch (drvhandle)
    {
      <#if CONFIG_DRV_I2C_INST_IDX0 == true>
        case DRV_I2C_INDEX_0:
        {
            return ( DRV_I2C0_TransmitForced (deviceaddress, txBuffer, size, eventFlag, context) );
            break;
        }
      </#if>
      <#if CONFIG_DRV_I2C_INST_IDX1 == true>
        case DRV_I2C_INDEX_1:
        {
            return ( DRV_I2C1_TransmitForced (deviceaddress, txBuffer, size, eventFlag, context) );
            break;
        }
      </#if>
      <#if CONFIG_DRV_I2C_INST_IDX2 == true>
        case DRV_I2C_INDEX_2:
        {
            return ( DRV_I2C2_TransmitForced (deviceaddress, txBuffer, size, eventFlag, context) );
            break;
        }
      </#if>
      <#if CONFIG_DRV_I2C_INST_IDX3 == true>
        case DRV_I2C_INDEX_3:
        {
            return ( DRV_I2C3_TransmitForced (deviceaddress, txBuffer, size, eventFlag, context) );
            break;
        }
      </#if>
      <#if CONFIG_DRV_I2C_INST_IDX4 == true>
        case DRV_I2C_INDEX_4:
        {
            return ( DRV_I2C4_TransmitForced (deviceaddress, txBuffer, size, eventFlag, context) );
            break;
        }
      </#if>
        default:
        {
            return DRV_I2C_BUFFER_HANDLE_INVALID;
        }
    }
    
    return DRV_I2C_BUFFER_HANDLE_INVALID;
}


DRV_I2C_BUFFER_HANDLE DRV_I2C_Receive ( DRV_HANDLE drvhandle, uint16_t deviceaddress, 
                                        void *rxBuffer, size_t size, void * context)
{
    switch (drvhandle)
    {
      <#if CONFIG_DRV_I2C_INST_IDX0 == true>
        case DRV_I2C_INDEX_0:
        {
            return ( DRV_I2C0_Receive(deviceaddress, rxBuffer, size, context) );
            break;
        }
      </#if>
      <#if CONFIG_DRV_I2C_INST_IDX1 == true>
        case DRV_I2C_INDEX_1:
        {
            return ( DRV_I2C1_Receive(deviceaddress, rxBuffer, size, context) );
            break;
        }
      </#if>
      <#if CONFIG_DRV_I2C_INST_IDX2 == true>
        case DRV_I2C_INDEX_2:
        {
            return ( DRV_I2C2_Receive(deviceaddress, rxBuffer, size, context) );
            break;
        }
      </#if>
      <#if CONFIG_DRV_I2C_INST_IDX3 == true>
        case DRV_I2C_INDEX_3:
        {
            return ( DRV_I2C3_Receive(deviceaddress, rxBuffer, size, context) );
            break;
        }
      </#if>
      <#if CONFIG_DRV_I2C_INST_IDX4 == true>
        case DRV_I2C_INDEX_4:
        {
            return ( DRV_I2C4_Receive(deviceaddress, rxBuffer, size, context) );
            break;
        }
      </#if>
        default:
        {
            return DRV_I2C_BUFFER_HANDLE_INVALID;
        }
    }
    
    return DRV_I2C_BUFFER_HANDLE_INVALID;
}

DRV_I2C_BUFFER_HANDLE DRV_I2C_TransmitThenReceive ( DRV_HANDLE drvhandle, uint16_t deviceaddress,
                                                    void *txBuffer, size_t wsize, 
                                                    void *rxBuffer, size_t rsize, void * context)
{
    switch (drvhandle)
    {
      <#if CONFIG_DRV_I2C_INST_IDX0 == true>
        case DRV_I2C_INDEX_0:
        {
            return ( DRV_I2C0_TransmitThenReceive(deviceaddress, txBuffer, wsize, rxBuffer, rsize, context) );
            break;
        }
      </#if>
      <#if CONFIG_DRV_I2C_INST_IDX1 == true>
        case DRV_I2C_INDEX_1:
        {
            return ( DRV_I2C1_TransmitThenReceive(deviceaddress, txBuffer, wsize, rxBuffer, rsize, context) );
            break;
        }
      </#if>
      <#if CONFIG_DRV_I2C_INST_IDX2 == true>
        case DRV_I2C_INDEX_2:
        {
            return ( DRV_I2C2_TransmitThenReceive(deviceaddress, txBuffer, wsize, rxBuffer, rsize, context) );
            break;
        }
      </#if>
      <#if CONFIG_DRV_I2C_INST_IDX3 == true>
        case DRV_I2C_INDEX_3:
        {
            return ( DRV_I2C3_TransmitThenReceive(deviceaddress, txBuffer, wsize, rxBuffer, rsize, context) );
            break;
        }
      </#if>
      <#if CONFIG_DRV_I2C_INST_IDX4 == true>
        case DRV_I2C_INDEX_4:
        {
            return ( DRV_I2C4_TransmitThenReceive(deviceaddress, txBuffer, wsize, rxBuffer, rsize, context) );
            break;
        }
      </#if>
        default:
        {
            return DRV_I2C_BUFFER_HANDLE_INVALID;
        }
    }
    
    return DRV_I2C_BUFFER_HANDLE_INVALID;
}

DRV_I2C_BUFFER_EVENT DRV_I2C_TransferStatusGet (  DRV_HANDLE drvhandle, DRV_I2C_BUFFER_HANDLE bufferHandle )
{
    switch (drvhandle)
    {
      <#if CONFIG_DRV_I2C_INST_IDX0 == true && CONFIG_DRV_I2C_STATIC_OPERATION_MODE_IDX0 == "DRV_I2C_MODE_MASTER">
        case DRV_I2C_INDEX_0:
        {
            return ( DRV_I2C0_TransferStatusGet(bufferHandle) );
            break;
        }
      </#if>
      <#if CONFIG_DRV_I2C_INST_IDX1 == true && CONFIG_DRV_I2C_STATIC_OPERATION_MODE_IDX1 == "DRV_I2C_MODE_MASTER">
        case DRV_I2C_INDEX_1:
        {
            return ( DRV_I2C1_TransferStatusGet(bufferHandle) );
            break;
        }
      </#if>
      <#if CONFIG_DRV_I2C_INST_IDX2 == true && CONFIG_DRV_I2C_STATIC_OPERATION_MODE_IDX2 == "DRV_I2C_MODE_MASTER">
        case DRV_I2C_INDEX_2:
        {
            return ( DRV_I2C2_TransferStatusGet(bufferHandle) );
            break;
        }
      </#if>
      <#if CONFIG_DRV_I2C_INST_IDX3 == true && CONFIG_DRV_I2C_STATIC_OPERATION_MODE_IDX3 == "DRV_I2C_MODE_MASTER">
        case DRV_I2C_INDEX_3:
        {
            return ( DRV_I2C3_TransferStatusGet(bufferHandle) );
            break;
        }
      </#if>
      <#if CONFIG_DRV_I2C_INST_IDX4 == true && CONFIG_DRV_I2C_STATIC_OPERATION_MODE_IDX4 == "DRV_I2C_MODE_MASTER">
        case DRV_I2C_INDEX_4:
        {
            return ( DRV_I2C4_TransferStatusGet(bufferHandle) );
            break;
        }
      </#if>
        default:
        {
            return (DRV_I2C_BUFFER_EVENT) NULL;
        }
    }
    
    return (DRV_I2C_BUFFER_EVENT) NULL;
}


uint32_t DRV_I2C_BytesTransferred (  DRV_HANDLE drvhandle,  DRV_I2C_BUFFER_HANDLE bufferHandle )
{
    switch (drvhandle)
    {
      <#if CONFIG_DRV_I2C_INST_IDX0 == true>
        case DRV_I2C_INDEX_0:
        {
            return ( DRV_I2C0_BytesTransferred(bufferHandle) );
            break;
        }
      </#if>
      <#if CONFIG_DRV_I2C_INST_IDX1 == true>
        case DRV_I2C_INDEX_1:
        {
            return (DRV_I2C1_BytesTransferred(bufferHandle) );
            break;
        }
      </#if>
      <#if CONFIG_DRV_I2C_INST_IDX2 == true>
        case DRV_I2C_INDEX_2:
        {
            return ( DRV_I2C2_BytesTransferred(bufferHandle) );
            break;
        }
      </#if>
      <#if CONFIG_DRV_I2C_INST_IDX3 == true>
        case DRV_I2C_INDEX_3:
        {
            return ( DRV_I2C3_BytesTransferred(bufferHandle) );
            break;
        }
      </#if>
      <#if CONFIG_DRV_I2C_INST_IDX4 == true>
        case DRV_I2C_INDEX_4:
        {
            return ( DRV_I2C4_BytesTransferred(bufferHandle) );
            break;
        }
      </#if>
        default:
        {
            return 0;
        }
    }
    
    return 0;
}

void DRV_I2C_BufferEventHandlerSet ( const DRV_HANDLE drvhandle,
                                     const DRV_I2C_BUFFER_EVENT_HANDLER eventHandler,
                                     const uintptr_t context )
{
    switch (drvhandle)
    {
      <#if CONFIG_DRV_I2C_INST_IDX0 == true>
        case DRV_I2C_INDEX_0:
        {
            DRV_I2C0_BufferEventHandlerSet (eventHandler,context);
            break;
        }
      </#if>
      <#if CONFIG_DRV_I2C_INST_IDX1 == true>
        case DRV_I2C_INDEX_1:
        {
            DRV_I2C1_BufferEventHandlerSet (eventHandler,context);
            break;
        }
      </#if>
      <#if CONFIG_DRV_I2C_INST_IDX2 == true>
        case DRV_I2C_INDEX_2:
        {
            DRV_I2C2_BufferEventHandlerSet (eventHandler,context);
            break;
        }
      </#if>
      <#if CONFIG_DRV_I2C_INST_IDX3 == true>
        case DRV_I2C_INDEX_3:
        {
            DRV_I2C3_BufferEventHandlerSet (eventHandler,context);
            break;
        }
      </#if>
      <#if CONFIG_DRV_I2C_INST_IDX4 == true>
        case DRV_I2C_INDEX_4:
        {
            DRV_I2C4_BufferEventHandlerSet (eventHandler,context);
            break;
        }
      </#if>
        default:
        {
            break;
        }
    }
}


/*******************************************************************************
 End of File
*/

