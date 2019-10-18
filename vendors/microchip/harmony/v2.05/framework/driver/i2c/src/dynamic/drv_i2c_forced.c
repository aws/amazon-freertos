/*******************************************************************************
  I2C Bit-Bang Interrupt model function definitions file

  Company:
    Microchip Technology Inc.

  File Name:
    drv_i2c_forced.c        
 * ****************************************************************************/

// DOM-IGNORE-BEGIN
/*******************************************************************************
Copyright (c) 2015 released Microchip Technology Inc.  All rights reserved.

Microchip licenses to you the right to use, modify, copy and distribute
Software only when embedded on a Microchip microcontroller or digital signal
controller that is integrated into your product or third party product
(pursuant to the sublicense terms in the accompanying license agreement).

You should refer to the license agreement accompanying this Software for
additional information regarding your rights and obligations.

SOFTWARE AND DOCUMENTATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND,
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
// DOM-IGNORE-END

#include <xc.h>
#include "driver/i2c/drv_i2c.h"
#include "driver/i2c/src/drv_i2c_local.h"



// *****************************************************************************
/*
 Upper address limit for a 7-bit address 
 */

#define ADDRESS_7BIT_UPPER_LIMIT                    0xFF

DRV_I2C_BUFFER_HANDLE DRV_I2C_TransmitForced (DRV_HANDLE handle, uint16_t deviceaddress, void* txBuffer, size_t txbuflen, DRV_I2C_BUS_ERROR_EVENT eventFlag, void * calbackContext)
{
    DRV_I2C_BUFFER_OBJECT *i2cDataObj;
    DRV_I2C_OBJ *dObj = ((DRV_I2C_CLIENT_OBJ *) handle)->driverObject;

    /* Get a slot in the queue */
    i2cDataObj = _DRV_I2C_QueueSlotGet (dObj);

    if ( i2cDataObj != NULL )
    {
        /* Fill the data directly to the queue. Set the inUse flag only at the end */
        i2cDataObj->clientHandle    = handle;
        if (deviceaddress > ADDRESS_7BIT_UPPER_LIMIT )
        {
            i2cDataObj->slaveaddresshighbyte = (uint8_t)((deviceaddress & 0xFF00)>>8);
            i2cDataObj->slaveaddresslowbyte  = (uint8_t)(deviceaddress & 0x00FF);
        }
        else
        {
            i2cDataObj->slaveaddresshighbyte = (uint8_t)(deviceaddress & 0x00FF);
            i2cDataObj->slaveaddresslowbyte  = 0;
        }
        i2cDataObj->operation       = DRV_I2C_OP_WRITE;
        i2cDataObj->txBuffer        = txBuffer;
        i2cDataObj->rxBuffer        = NULL;
        i2cDataObj->transferSize    = txbuflen;
        i2cDataObj->actualtransfersize = 0;
        i2cDataObj->status          = DRV_I2C_BUFFER_EVENT_PENDING;
        i2cDataObj->context         = calbackContext;
        i2cDataObj->errorEvent      = DRV_I2C_HALT_ON_ERROR;
        i2cDataObj->transmitForced  = true;

        _DRV_I2C_QueuePush( dObj, i2cDataObj);

        if ( (dObj->i2cMode) == DRV_I2C_MODE_MASTER)
        {
            /*  if either START and STOP were not detected which is true the 
                first time OR if STOP was detected, then it assumed the 
                transaction on the bus is complete */
            
            if ( ((!(PLIB_I2C_StartWasDetected(dObj->i2cId))) && (!(PLIB_I2C_StopWasDetected(dObj->i2cId)))) ||
                    (PLIB_I2C_StopWasDetected(dObj->i2cId)) )
            {
                            
                /* if Bus IDLE and I2CxMIF = 0, then I2C is not running*/
                
                if ( (PLIB_I2C_BusIsIdle(dObj->i2cId)) &&
                       (!(SYS_INT_SourceStatusGet(dObj->mstrInterruptSource))) )
                {
//                    SYS_INT_SourceStatusClear(dObj->mstrInterruptSource);
                    _DRV_I2C_InterruptSourceEnable( dObj->mstrInterruptSource ) ;
                    
                    _DRV_I2C_InterruptSourceEnable( dObj->errInterruptSource );
                    dObj->task = DRV_I2C_TASK_SEND_DEVICE_ADDRESS;
                    PLIB_I2C_MasterStart(dObj->i2cId);
                }
            }
        }
        else
        {
           SYS_INT_SourceStatusClear(dObj->slaveInterruptSource);
           _DRV_I2C_InterruptSourceEnable( dObj->slaveInterruptSource ) ;

        }
        return (DRV_I2C_BUFFER_HANDLE) i2cDataObj;
    }
    return DRV_I2C_BUFFER_HANDLE_INVALID;
} /* DRV_I2C_Transmit */