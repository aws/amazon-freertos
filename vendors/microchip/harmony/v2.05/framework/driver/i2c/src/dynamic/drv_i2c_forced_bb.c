/*******************************************************************************
  I2C Bit-Bang Interrupt model function definitions file

  Company:
    Microchip Technology Inc.

  File Name:
    drv_i2c_bb.c        

 * Summary:     This is a sample project demonstrating an I2C Master
 *              device simulated in software.  The project consists of an 
 *              initialization function, functions to perform I2C bus operations  
 *              and an Interrupt Service Routine (ISR) that contains the I2C Master 
 *              state machine.  The ISR is responsible for performing the START, 
 *              STOP, RESTART, WRITE byte and READ byte operation.              
 *              This project uses the IO pins of I2C-2 peripheral. If another
 *              I2C module, other that I2C2 is required to be used, this can be
 *              done so by modifying the port definition is i2c_bitbang_isr.h.
 * 
 *              This is developed in accordance with In accordance with the 
 *              I2C Bus Specification and User Manual Revision 6, April 2014.
 * 
 *              The i2c_bitbang_isr.c was tested with SYSCLK running at 200 MHz
 *              and PBCLK 3 that the timer is based on running at 100 MHz 
*******************************************************************************/
/*******************************************************************************
 *                      FEATURES NOT SUPPORTED
 *                 --------------------------------
 *  a. MULTI MASTER
 *  b. 10-BIT ADDRESSING
 *  c. CLOCK SYNCHRONIZATION 
 *  D. BUS ARBITRATION
 *  E. Does not support FM+ (1 Mbit/s) and greater
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
#include "driver/i2c/drv_i2c_bb.h"

extern uint16_t  I2CSWCounter;


#define ADDRESS_7BIT_UPPER_LIMIT                    0xFF

DRV_I2C_BUFFER_HANDLE DRV_I2C_TransmitForced(DRV_HANDLE handle, uint16_t deviceaddress, void* txBuffer, size_t txbuflen, DRV_I2C_BUS_ERROR_EVENT eventFlag, void * calbackContext)
{
    
    DRV_I2C_BUFFER_OBJECT   *i2cDataObj;
    DRV_I2C_OBJ             *dObj = ((DRV_I2C_CLIENT_OBJ *) handle)->driverObject;
    
    bool queueEmptyTest = false;
    
    queueEmptyTest = _DRV_I2C_IsQueueEmpty(dObj);
    
        /* Get a slot in the queue */
    i2cDataObj = _DRV_I2C_QueueSlotGet ( dObj );
    
    if (i2cDataObj != NULL)
    {
        i2cDataObj->i2cPerph            = DRV_I2C_BIT_BANG;
        i2cDataObj->clientHandle        = handle;
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
        i2cDataObj->operation           = DRV_I2C_OP_WRITE_READ;
        i2cDataObj->txBuffer            = txBuffer;
        i2cDataObj->transferSize        = txbuflen;

        i2cDataObj->actualtransfersize  = 0;
		i2cDataObj->transmitForced		= true;
        i2cDataObj->status              = DRV_I2C_BUFFER_EVENT_PENDING;
        i2cDataObj->errorEvent          = DRV_I2C_HALT_ON_ERROR;
        i2cDataObj->context             = 0x00;
        
        _DRV_I2C_QueuePush( dObj, i2cDataObj);
                              
        if (queueEmptyTest)
        {
            StartI2C((DRV_HANDLE)handle);
            dObj->I2CSWCounter       = 9;
        }
        
        return (DRV_I2C_BUFFER_HANDLE)i2cDataObj;
    }
    else 
        return DRV_I2C_BUFFER_HANDLE_INVALID;
    
}