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

// *****************************************************************************
/*
 Upper address limit for a 7-bit address
 */

#define ADDRESS_7BIT_UPPER_LIMIT                    0xFF



#define ERROR_TIMEOUT_COUNT         2000000

// *****************************************************************************
/* Macro: _DRV_I2C_CLIENT_OBJ(obj,mem)

  Summary:
    Returns the appropriate client member

  Description:
    Either return the static object or returns the indexed dynamic object.
    This macro has variations for dynamic or static driver.
*/

#define _DRV_I2C_CLIENT_OBJ(obj,mem)    obj->mem

void StartI2C(DRV_HANDLE);


// *****************************************************************************
/* Function:
    I2C_Initialize (uint32_t baudrate)

  Input: I2C baud rate

  Output: None

  Summary:
    Initializes the TRI, LAT and Open drain enable value for the I2C pins

  Description:
    Initializes the TRI, LAT and Open drain enable value for the I2C pins

  Remarks:
    None.
*/

void DRV_I2C_BB_Initialize(DRV_I2C_OBJ *dObj, DRV_I2C_INIT * i2cInit)
{

    PLIB_PORTS_PinClear(PORTS_ID_0, dObj->portSCL, dObj->pinSCL);		//Make SCL LAT value 0
	PLIB_PORTS_PinClear(PORTS_ID_0, dObj->portSDA, dObj->pinSDA);		//Make SDA LAT value 0

    PLIB_PORTS_PinDirectionInputSet(PORTS_ID_0, dObj->portSCL, dObj->pinSCL);			//set SCL_INPUT
    PLIB_PORTS_PinDirectionInputSet(PORTS_ID_0, dObj->portSDA, dObj->pinSDA);
	PLIB_PORTS_PinOpenDrainEnable(PORTS_ID_0, dObj->portSCL, dObj->pinSCL);
	PLIB_PORTS_PinOpenDrainEnable(PORTS_ID_0, dObj->portSDA, dObj->pinSDA);

	PLIB_TMR_PrescaleSelect(dObj->tmrSource, TMR_PRESCALE_VALUE_1);
	PLIB_TMR_ClockSourceSelect(dObj->tmrSource, TMR_CLOCK_SOURCE_PERIPHERAL_CLOCK);

	// Load the period register
	PLIB_TMR_Period16BitSet(dObj->tmrSource,
                           (SYS_CLK_PeripheralFrequencyGet( CLK_BUS_PERIPHERAL_2 )
                           /(i2cInit->baudRate<<2)));

    PLIB_INT_SourceFlagClear(INT_ID_0, dObj->tmrInterruptSource);
    PLIB_INT_SourceEnable(INT_ID_0, dObj->tmrInterruptSource);


}

DRV_I2C_BUFFER_HANDLE   DRV_I2C_MasterBufferWriteXE(DRV_HANDLE handle,
                        uint16_t deviceaddress,
                        uint8_t* txBuffer,
                        uint16_t txbuflen)
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
        } else {
            i2cDataObj->slaveaddresshighbyte = (uint8_t)(deviceaddress & 0x00FF);
            i2cDataObj->slaveaddresslowbyte  = 0;
        }
        i2cDataObj->operation           = DRV_I2C_OP_WRITE;
        i2cDataObj->txBuffer            = txBuffer;
        i2cDataObj->rxBuffer            = NULL;
        i2cDataObj->transferSize        = txbuflen;
        i2cDataObj->readtransferSize    = 0;
        i2cDataObj->actualtransfersize  = 0;
        i2cDataObj->status              = DRV_I2C_BUFFER_EVENT_PENDING;
        i2cDataObj->context             = 0x00;


        _DRV_I2C_QueuePush( dObj, i2cDataObj);


        /*  initiate START condition only if I2C bit-bang
            is not active, i.e. timer is not running, TMRx = 0  */

        if (queueEmptyTest)
        {
            StartI2C((DRV_HANDLE)handle);
            dObj->I2CSWCounter        = 9;
        }
        return (DRV_I2C_BUFFER_HANDLE)i2cDataObj;
    } else {
        return DRV_I2C_BUFFER_HANDLE_INVALID;
    }
}

DRV_I2C_BUFFER_HANDLE   DRV_I2C_MasterBufferWrite(DRV_HANDLE handle,
                        uint8_t* deviceaddress,
                        uint8_t* txBuffer,
                        uint16_t txbuflen)
{

    DRV_I2C_BUFFER_OBJECT   *i2cDataObj;
    DRV_I2C_OBJ             *dObj = ((DRV_I2C_CLIENT_OBJ *) handle)->driverObject;

    bool queueEmptyTest = false;

    queueEmptyTest = _DRV_I2C_IsQueueEmpty(dObj);

        /* Get a slot in the queue */
    i2cDataObj = _DRV_I2C_QueueSlotGet ( dObj );


    if (i2cDataObj != NULL)
    {
        i2cDataObj->i2cPerph                = DRV_I2C_BIT_BANG;
        i2cDataObj->clientHandle            = handle;
        i2cDataObj->slaveaddresshighbyte    = *(deviceaddress);
        i2cDataObj->slaveaddresslowbyte     = 0x00;
        i2cDataObj->operation               = DRV_I2C_OP_WRITE;
        i2cDataObj->txBuffer                = txBuffer;
        i2cDataObj->rxBuffer                = NULL;
        i2cDataObj->transferSize            = txbuflen;
        i2cDataObj->readtransferSize        = 0;
        i2cDataObj->actualtransfersize      = 0;
        i2cDataObj->status                  = DRV_I2C_BUFFER_EVENT_PENDING;
        i2cDataObj->context                 = 0x00;

        _DRV_I2C_QueuePush( dObj, i2cDataObj);


        /*  initiate START condition only if I2C bit-bang
            is not active, i.e. timer is not running, TMRx = 0  */

        if (queueEmptyTest)
        {
            StartI2C((DRV_HANDLE)handle);
            dObj->I2CSWCounter        = 9;
        }
        return (DRV_I2C_BUFFER_HANDLE)i2cDataObj;
    } else {
        return DRV_I2C_BUFFER_HANDLE_INVALID;
    }
}

DRV_I2C_BUFFER_HANDLE   DRV_I2C_MasterBufferReadXE(DRV_HANDLE handle,
                        uint16_t deviceaddress,
                        uint8_t* rxBuffer,
                        uint16_t rxbuflen)
{

    DRV_I2C_BUFFER_OBJECT   *i2cDataObj;
    DRV_I2C_OBJ             *dObj = ((DRV_I2C_CLIENT_OBJ *) handle)->driverObject;

    bool queueEmptyTest = false;

    queueEmptyTest = _DRV_I2C_IsQueueEmpty(dObj);

        /* Get a slot in the queue */
    i2cDataObj = _DRV_I2C_QueueSlotGet ( dObj );


    if (i2cDataObj != NULL)
    {
        /* Fill the data directly to the queue. Set the inUse flag only at the end */
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
        i2cDataObj->operation           = DRV_I2C_OP_READ;
        i2cDataObj->txBuffer            = NULL;
        i2cDataObj->rxBuffer            = rxBuffer;
        i2cDataObj->transferSize        = 0;
        i2cDataObj->readtransferSize    = rxbuflen;
        i2cDataObj->actualtransfersize  = 0;
        i2cDataObj->status              = DRV_I2C_BUFFER_EVENT_PENDING;
        i2cDataObj->context             = 0x00;

        _DRV_I2C_QueuePush( dObj, i2cDataObj);


        /*  initiate START condition only if I2C bit-bang
            is not active, i.e. timer is not running, TMRx = 0  */
        if (queueEmptyTest)
        {
            StartI2C((DRV_HANDLE)handle);
            dObj->I2CSWCounter        = 9;
        }

        return (DRV_I2C_BUFFER_HANDLE)i2cDataObj;
    } else  {
        return DRV_I2C_BUFFER_HANDLE_INVALID;
    }
}

DRV_I2C_BUFFER_HANDLE   DRV_I2C_MasterBufferRead(DRV_HANDLE handle,
                        uint8_t* deviceaddress,
                        uint8_t* rxBuffer,
                        uint16_t rxbuflen)
{
    DRV_I2C_BUFFER_OBJECT   *i2cDataObj;
    DRV_I2C_OBJ             *dObj = ((DRV_I2C_CLIENT_OBJ *) handle)->driverObject;

    bool queueEmptyTest = false;

    queueEmptyTest = _DRV_I2C_IsQueueEmpty(dObj);

        /* Get a slot in the queue */
    i2cDataObj = _DRV_I2C_QueueSlotGet ( dObj );


    if (i2cDataObj != NULL)
    {
        /* Fill the data directly to the queue. Set the inUse flag only at the end */
        i2cDataObj->i2cPerph                = DRV_I2C_BIT_BANG;
        i2cDataObj->clientHandle            = handle;
        i2cDataObj->slaveaddresshighbyte    = *(deviceaddress);
        i2cDataObj->slaveaddresslowbyte     = 0x00;
        i2cDataObj->operation               = DRV_I2C_OP_READ;
        i2cDataObj->txBuffer                = NULL;
        i2cDataObj->rxBuffer                = rxBuffer;
        i2cDataObj->transferSize            = 0;
        i2cDataObj->readtransferSize        = rxbuflen;
        i2cDataObj->actualtransfersize      = 0;
        i2cDataObj->status                  = DRV_I2C_BUFFER_EVENT_PENDING;
        i2cDataObj->context                 = 0x00;

        _DRV_I2C_QueuePush( dObj, i2cDataObj);


        /*  initiate START condition only if I2C bit-bang
            is not active, i.e. timer is not running, TMRx = 0  */
        if (queueEmptyTest)
        {
            StartI2C((DRV_HANDLE)handle);
            dObj->I2CSWCounter        = 9;
        }

        return (DRV_I2C_BUFFER_HANDLE)i2cDataObj;
    } else  {
        return DRV_I2C_BUFFER_HANDLE_INVALID;
    }
}

DRV_I2C_BUFFER_HANDLE   DRV_I2C_MasterBufferWriteReadXE(DRV_HANDLE handle,
                        uint16_t deviceaddress,
                        uint8_t* txBuffer,
                        uint16_t txbuflen,
                        uint8_t* rxBuffer,
                        uint16_t rxbuflen)
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
        } else {
            i2cDataObj->slaveaddresshighbyte = (uint8_t)(deviceaddress & 0x00FF);
            i2cDataObj->slaveaddresslowbyte  = 0;
        }
        i2cDataObj->operation           = DRV_I2C_OP_WRITE_READ;
        i2cDataObj->txBuffer            = txBuffer;
        i2cDataObj->rxBuffer            = rxBuffer;
        i2cDataObj->transferSize        = txbuflen;
        i2cDataObj->readtransferSize    = rxbuflen;
        i2cDataObj->actualtransfersize  = 0;
        i2cDataObj->status              = DRV_I2C_BUFFER_EVENT_PENDING;
        i2cDataObj->context             = 0x00;

        _DRV_I2C_QueuePush( dObj, i2cDataObj);

        if (queueEmptyTest)
        {
            StartI2C((DRV_HANDLE)handle);
            dObj->I2CSWCounter        = 9;
        }

        return (DRV_I2C_BUFFER_HANDLE)i2cDataObj;
    } else  {
        return DRV_I2C_BUFFER_HANDLE_INVALID;
    }
}

DRV_I2C_BUFFER_HANDLE   DRV_I2C_MasterBufferWriteRead(
                        DRV_HANDLE handle,
                        uint8_t* deviceaddress,
                        uint8_t* txBuffer,
                        uint16_t txbuflen,
                        uint8_t* rxBuffer,
                        uint16_t rxbuflen)
{

    DRV_I2C_BUFFER_OBJECT   *i2cDataObj;
    DRV_I2C_OBJ             *dObj = ((DRV_I2C_CLIENT_OBJ *) handle)->driverObject;

    bool queueEmptyTest = false;

    queueEmptyTest = _DRV_I2C_IsQueueEmpty(dObj);

        /* Get a slot in the queue */
    i2cDataObj = _DRV_I2C_QueueSlotGet ( dObj );

    if (i2cDataObj != NULL)
    {
        i2cDataObj->i2cPerph                = DRV_I2C_BIT_BANG;
        i2cDataObj->clientHandle            = handle;
        i2cDataObj->slaveaddresshighbyte    = *(deviceaddress);
        i2cDataObj->slaveaddresslowbyte     = 0x00;
        i2cDataObj->operation               = DRV_I2C_OP_WRITE_READ;
        i2cDataObj->txBuffer                = txBuffer;
        i2cDataObj->rxBuffer                = rxBuffer;
        i2cDataObj->transferSize            = txbuflen;
        i2cDataObj->readtransferSize        = rxbuflen;
        i2cDataObj->actualtransfersize      = 0;
        i2cDataObj->status                  = DRV_I2C_BUFFER_EVENT_PENDING;
        i2cDataObj->context                 = 0x00;

        _DRV_I2C_QueuePush( dObj, i2cDataObj);

        if (queueEmptyTest)
        {
            StartI2C((DRV_HANDLE)handle);
            dObj->I2CSWCounter        = 9;
        }

        return (DRV_I2C_BUFFER_HANDLE)i2cDataObj;
    } else  {
        return DRV_I2C_BUFFER_HANDLE_INVALID;
    }
}

//bool DRV_I2C_MasterACKStatus(void)
//{
//    return I2CACKStatus;
//}

// *****************************************************************************
/* Function:
    void StartI2C(void)

  Input: None

  Output: None

  Summary:
    Generates START condition on I2C bus

  Description:
    Initiate a START condition on I2C bus.

  Remarks:
    Check for BUS IDLE before setting a START condition
*/

void StartI2C(DRV_HANDLE handle)
{
    DRV_I2C_OBJ             *dObj = ((DRV_I2C_CLIENT_OBJ *) handle)->driverObject;

    /* send START command only if SCL and SDA are pulled high */
    if ((PLIB_PORTS_PinGet(PORTS_ID_0, dObj->portSCL, dObj->pinSCL) == HIGH) &&
                    (PLIB_PORTS_PinGet(PORTS_ID_0, dObj->portSDA, dObj->pinSDA) == HIGH))
    {
        dObj->i2cState = I2C_SDA_LOW_START;
        PLIB_TMR_Counter16BitClear(dObj->tmrSource);
        PLIB_TMR_Start(dObj->tmrSource);

        dObj->I2CNACKOut = false;
        dObj->I2CACKStatus = false;

        dObj->errorTimeOut = ERROR_TIMEOUT_COUNT;
        dObj->ACKSTATUS_M  = false;
    }
}


// *****************************************************************************
/* Function:
    bool IsByteAcked(void)

  Input: None

  Output: status of ACK or ACK after sending a byte

  Summary:
    Checks the status of ACK or NACK of a byte

  Description:
    Checks the status of ACK or NACK and return true if ACK; false if NACK

  Remarks:
    None
*/

//inline bool IsByteAcked(void)
//{
    //if (I2CACKStatus == 0)
    //    return true;
    //else
    //    return false;
//}

/*  The Bit Bang task maintains a state machine to control the state of an I2C
    bus.  Specifically it maintains the state of a clock (SCL) and data (SDA)
    lines in accordance with the I2C specification.

   This is a typical Transaction timing diagram.
   The transaction consists of an address and then a single byte.  But it gives
   an example of all of the states which the driver below must go through. If
   the RD-WR bit indicates that the transaction is a write, then the byte is
   sent from the master and received by the slave device.  If the RD-WR bit
   indicates that the transaction is a read, then the byte will be sent by
   slave and received by the master device.
   Specific transactions on the I2C bus can include any number of bytes either
   sent or received.  And, it could include a series of bytes sent followed by
   a series of bytes received as long as the start signal and address portions
   are repeated.
   --\     /-A6~A0-\/--RD--\/-NACK-\/-D7~D0-\/-NACK-\/    /--
   SDA\---/\---~---/\--WR--/\--ACK-/\---~---/\--ACK-/\---/
   -----\    /-~-\    /--\    /--\    /-~-\    /--\    /-----
   SCL   \--/     \--/    \--/    \--/     \--/    \--/
     Start   Slave   Read or         Data               Stop
     Signal Address Write bit        Byte               Signal

   The state machine is further divided by the baud rate clock which runs at
   4x the clock rate thus requiring 4 states for each clock pulse.  A couple
   of states prepare the transition between an active bus and an idle bus.
   Finally, be aware that the bus specifies pull up resistors.  This means that
   converting a pin from an output to an input allows it to float high as well
   as allowing another participant on the bus to pull the line low.  Thus switching
   the line from an output to an input is the same as "sending" a HIGH on that line
   (Unless something else on the bus is "sending" a LOW).

   Please see the header file for more information on this function.
 */
inline void DRV_I2C_BB_Tasks ( SYS_MODULE_OBJ object )
{
    DRV_I2C_OBJ             *dObj           = (DRV_I2C_OBJ*)object;
    DRV_I2C_BUFFER_OBJECT   *lBufferObj     = dObj->taskLObj;

    // State transition time should be 1/2 baud rate clock.
    switch (dObj->i2cState)
    {
        // These first 4 states are the I2C Start Signal.  Begining with both
        // SDA and SCL floating HIGH, first SDA is pulled LOW, then SCL is
        // pulled LOW and allowed to remain LOW for 1/2 clocks.  This signals
        // a Start to the I2C Bus.
        // I2C Start Signal begin
        // SDA is driven LOW while SCL allowed to float HIGH
        case I2C_SDA_LOW_START:
            PLIB_PORTS_PinDirectionOutputSet(PORTS_ID_0, dObj->portSDA, dObj->pinSDA);
            dObj->i2cState = I2C_SDA_LOW_START_CHECK;
            break;
        // I2C Start Signal continue
        // No action is taken
        case I2C_SDA_LOW_START_CHECK:
            dObj->i2cState = I2C_SCL_LOW_START;
            break;
        // I2C Start Signal continue
        // SCL is driven LOW (SDA is already LOW from I2C_SDA_LOW_START)
        case I2C_SCL_LOW_START:
            PLIB_PORTS_PinDirectionOutputSet(PORTS_ID_0, dObj->portSCL, dObj->pinSCL);
            dObj->i2cState =  I2C_SCL_LOW_START_CHECK;
            break;
        // I2C Start Signal complete.
        // begin the next transfer - Copy transfer from QUEUE to internal variables.
        case I2C_SCL_LOW_START_CHECK:
            if( (lBufferObj != NULL)
                && (lBufferObj->operation == DRV_I2C_OP_WRITE_READ)
                && (lBufferObj->actualtransfersize > 0)
                && (lBufferObj->readtransferSize > 0) )
            {
                lBufferObj->operation  = DRV_I2C_OP_READ;
            } else {
                dObj->taskLObj = _DRV_I2C_QueuePop(dObj);
                lBufferObj     = dObj->taskLObj;
            }
            dObj->I2CSWData       =  (lBufferObj->slaveaddresshighbyte);
            if (lBufferObj->operation == DRV_I2C_OP_READ)
            {
                dObj->I2CSWData  |=  0x01;
            }
            dObj->i2cState        = I2C_SCL_LOW_DATA_CHECK;
            break;
        // These next 4 states are the I2C ReStart Signal.  This sequence
        // simply transitions the bus from the active to the idle state so that
        // a start signal can be sent.  It also checks that the bus is not
        // conflicted.
        // I2C Re-Start Signal state 1 of 4.
        // SDA is allowed to float HIGH to partially return bus to idle
        case I2C_SDA_HIGH_RESTART:
            PLIB_PORTS_PinDirectionInputSet(PORTS_ID_0, dObj->portSDA, dObj->pinSDA);
            dObj->errorTimeOut = ERROR_TIMEOUT_COUNT;
            dObj->i2cState    = I2C_SDA_HIGH_RESTART_CHECK;
            break;
        // I2C Re-Start Signal state 2 of 4.
        // SDA is checked to verify that it indeed floated HIGH
        // If so, then proceed to allowing SCL to float HIGH.
        // If not, then after a number of clocks declare a bus error.
        case I2C_SDA_HIGH_RESTART_CHECK:
            if (PLIB_PORTS_PinGet(PORTS_ID_0, dObj->portSDA, dObj->pinSDA) == INPUT)
            {
            	dObj->i2cState    = I2C_SCL_HIGH_RESTART;
            } else {
                if  (!(dObj->errorTimeOut--))
                {
                    lBufferObj->status = DRV_I2C_BUFFER_EVENT_ERROR;
                    PLIB_TMR_Stop(dObj->tmrSource);
                    PLIB_TMR_Counter16BitClear(dObj->tmrSource);
                    dObj->errorTimeOut = ERROR_TIMEOUT_COUNT;
                } else {
                    PLIB_PORTS_PinDirectionInputSet(PORTS_ID_0, dObj->portSDA, dObj->pinSDA);
                }
            }
            break;
        // I2C Re-Start Signal state 3 of 4.
        // SCL allowed to float HIGH thus bus is in the IDLE state.
        case I2C_SCL_HIGH_RESTART:
            PLIB_PORTS_PinDirectionInputSet(PORTS_ID_0, dObj->portSCL, dObj->pinSCL);
            dObj->errorTimeOut = ERROR_TIMEOUT_COUNT;
            dObj->i2cState    = I2C_SCL_HIGH_RESTART_CHECK;
            break;
        // I2C Re-Start Signal state 4 of 4.
        // SCL is checked to verify that it indeed floated HIGH
        // If so, then proceed to pulling SDA LOW which begins Start Signal
        // If not, then after a number of clocks declare a bus error.
        case I2C_SCL_HIGH_RESTART_CHECK:
            if (PLIB_PORTS_PinGet(PORTS_ID_0, dObj->portSCL, dObj->pinSCL) == INPUT)
            {
            	dObj->i2cState = I2C_SDA_LOW_START;
            } else {
                if  (!(dObj->errorTimeOut--))
                {
                    lBufferObj->status = DRV_I2C_BUFFER_EVENT_ERROR;
                    PLIB_TMR_Stop(dObj->tmrSource);
                    PLIB_TMR_Counter16BitClear(dObj->tmrSource);
                    dObj->errorTimeOut = ERROR_TIMEOUT_COUNT;
                } else {
                    PLIB_PORTS_PinDirectionInputSet(PORTS_ID_0, dObj->portSCL, dObj->pinSCL);
                }
            }
            break;
        // The next 4 states accomplish a single bit transfer.  The bit can be
        // Address sent from the master, Data sent from the master, or Data
        // received by the master.
        // I2C Data bit state 1 of 4
        // SCL pulled LOW in preperation for SDA change.
        case I2C_SCL_LOW_DATA_CHECK:
            PLIB_PORTS_PinDirectionOutputSet(PORTS_ID_0, dObj->portSCL, dObj->pinSCL);
            dObj->i2cState = I2C_SCL_LOW_DATA;
            break;
        // I2C Data bit state 2 of 4
        // SCL remains LOW AND
        // SDA is set to next bit to transmit
        // NOTE: During a receive, the transmit data is set to all 1s.  This causes
        //       SDA to float high allowing the transmitter to send 1s or 0s.
        //       So, even though this first if block looks like it is always tranmitting,
        //       during a receive operation, it is always setting SDA to input.
        case I2C_SCL_LOW_DATA:
            if (dObj->I2CSWCounter > 1)
            {
                if ((bool) (dObj->I2CSWData & 0x80))
                {
                    PLIB_PORTS_PinDirectionInputSet(PORTS_ID_0, dObj->portSDA, dObj->pinSDA);
                } else {
                    PLIB_PORTS_PinDirectionOutputSet(PORTS_ID_0, dObj->portSDA, dObj->pinSDA);
                }
            }
            // just before the 9th clock prepare for an acknowledge.
            else if (dObj->I2CSWCounter == 1)
            {
                dObj->_i2c_bit_written = 0;
                // If the operation is a read and the last byte received is
                // not the last one according to the read count,
                // AND the last byte was not the slave address,
                // then acknowledge the read.
                // NOTE: No validation of the data is performed.  A slave which
                // acknowledges its address, but then does not send any data,
                // will result in a buffer full of 0xff.
                if ((lBufferObj->operation == DRV_I2C_OP_READ)
                    && ((lBufferObj->readtransferSize != 0)
                    && (dObj->I2CSWData == 0xFF00)))
                {
                    PLIB_PORTS_PinDirectionOutputSet(PORTS_ID_0, dObj->portSDA, dObj->pinSDA);
                }
                else
                {
                    // On the other hand, if the last byte sent was the slave address,
                    // OR if it was the last byte of a receive sequence,
                    // OR if it was a write,
                    // Then we allow SDA to float so that the receiver can acknowledge it,
                    // And clear the flag indicating that the address is still sending.
                    PLIB_PORTS_PinDirectionInputSet(PORTS_ID_0, dObj->portSDA, dObj->pinSDA);
            	}
            }
            // After the 9th clock
            if (dObj->I2CSWCounter == 0)
            {
                // WRITE or WRITE_READ operaions with more bytes to send must
                // get the next byte out of the transmit buffer.
                // NOTE: if the last byte was NACKed, then transmission is
                // aborted.
                // ALSO NOTE: after the WRITE portion of a WRITE_READ, the
                // operation is changed to READ.  So, some portions of the
                // WRITE_READ operation are handled below.
                if ( (lBufferObj->transferSize)
                     && ((dObj->I2CACKStatus == M_ACK)
                      || (lBufferObj->transmitForced))
                     && ((lBufferObj->operation == DRV_I2C_OP_WRITE)
                      || (lBufferObj->operation == DRV_I2C_OP_WRITE_READ)) )
                {
                    dObj->I2CSWData   = *lBufferObj->txBuffer++;
                    dObj->i2cState    = I2C_SCL_LOW_DATA;
                    dObj->I2CSWCounter    = 9;
                    lBufferObj->actualtransfersize++;
                    lBufferObj->transferSize--;
                }
                // If a READ operation has more bytes to receive AND the last
                // byte was acknowledged, then the next byte can be readied
                // for a receive.
                else if (   (lBufferObj->readtransferSize)
                         && (dObj->I2CACKStatus == M_ACK)
                         && (lBufferObj->operation == DRV_I2C_OP_READ) )
                {
                    // NOTE: During a receive, the transmit data is set to all 1s.  This causes
                    //       SDA to float high allowing the transmitter to send 1s or 0s.
                    dObj->I2CSWData   = 0xFF;
                    dObj->i2cState    = I2C_SCL_LOW_DATA;
                    dObj->I2CSWCounter    = 9;
                    lBufferObj->actualtransfersize++;
                    lBufferObj->readtransferSize--;
                    if (lBufferObj->readtransferSize == 0)
                    {
                        dObj->I2CNACKOut = 0x01;
                    } else {
                        dObj->I2CNACKOut = 0;
                    }
                }
                //If the WRITE_READ operation has reached the end of its WRITE
                // portion, AND there are bytes to receive, AND the last byte
                // was ACKed, then the operation is set to READ and a ReStart
                // is begun.
                else if ( (!lBufferObj->transferSize)
                          && (lBufferObj->readtransferSize)
                          && (dObj->I2CACKStatus == M_ACK)
                          && (lBufferObj->operation == DRV_I2C_OP_WRITE_READ) )
                {
                    lBufferObj->operation = DRV_I2C_OP_WRITE_READ;
                    dObj->i2cState = I2C_SDA_HIGH_RESTART;
                    dObj->I2CSWCounter        = 9;
                // Finally, if the last clock was the last clock in our operation,
                // then begin the STOP signal sequence.
                } else {
                    dObj->i2cState    = I2C_SCL_SDA_LOW_STOP;
            	}
            } else {
                dObj->i2cState = I2C_SCL_HIGH_DATA;       //if clock count is not 0
                dObj->I2CSWCounter--;                     //decrement clock counter
                dObj->I2CSWData <<= 1;                    //shift data byte
            }
            break;
        // I2C Data bit state 3 of 4
        // SCL allowed to float HIGH so receiver can lock in bit.
        // Also, transmitter can check to see if a 1 was successfully transmitted.
        case I2C_SCL_HIGH_DATA:
            if ( (dObj->_i2c_bit_written == 1) && (lBufferObj->operation == DRV_I2C_OP_WRITE) )
            {
                /* check the value of bit that is just written onto the bus */
                if ((bool)(PLIB_PORTS_PinGet(PORTS_ID_0, dObj->portSDA, dObj->pinSDA)) == (bool)dObj->_i2c_bit_written)
                {
                    PLIB_PORTS_PinDirectionInputSet(PORTS_ID_0, dObj->portSCL, dObj->pinSCL);
                    dObj->i2cState = I2C_SCL_HIGH_DATA_CHECK;
                } else {
                    lBufferObj->status = DRV_I2C_BUFFER_EVENT_ERROR;
                    PLIB_TMR_Stop(dObj->tmrSource);
                    PLIB_TMR_Counter16BitClear(dObj->tmrSource);
                    dObj->errorTimeOut = ERROR_TIMEOUT_COUNT;
                    PLIB_PORTS_PinDirectionInputSet(PORTS_ID_0, dObj->portSCL, dObj->pinSCL);
                }
            } else {
                PLIB_PORTS_PinDirectionInputSet(PORTS_ID_0, dObj->portSCL, dObj->pinSCL);
                dObj->i2cState = I2C_SCL_HIGH_DATA_CHECK;
            }
            break;
        // I2C Data bit state 4 of 4
        // SCL checked to make sure it floated HIGH.
        // If not, then slave clock stretching is assumed.  Transfers are delayed until
        // the slave give up the clock.  Or an error timeout occurs.
        // Receiver checks SDA as next bit of data to receive or as NACK/ACK of address.
        // Also, transmitter can check to see if a 1 was successfully transmitted.
        case I2C_SCL_HIGH_DATA_CHECK:
            if (PLIB_PORTS_PinGet(PORTS_ID_0, dObj->portSCL, dObj->pinSCL) == INPUT)
            {
                if ((lBufferObj->operation == DRV_I2C_OP_READ)
                    && (dObj->I2CSWCounter > 0)
                    && (lBufferObj->actualtransfersize > 0))
                {
                    dObj->I2CReadData <<= 1;
                    dObj->I2CReadData |= PLIB_PORTS_PinGet(PORTS_ID_0, dObj->portSDA, dObj->pinSDA);
                }
                if (dObj->I2CSWCounter == 0)
                {
                    dObj->I2CACKStatus = PLIB_PORTS_PinGet(PORTS_ID_0, dObj->portSDA, dObj->pinSDA);

                    // Effectively, there are 2 types of reads.  Reads, and the
                    // read portion of a write then read.  For both types, the
                    // SWData value is set to the slave address right after the
                    // Start Signal.  For every other read byte, it is set to
                    // 0x00FF. By the time that the first received byte gets
                    // here, it has been shifted 9 times.  So, the I2CSWData
                    // value has become 0xFE00.
                    if( (lBufferObj->operation == DRV_I2C_OP_READ) &&
                        (dObj->I2CSWData == 0xFE00))
                    {
                        *lBufferObj->rxBuffer++ = dObj->I2CReadData;
                    }
                }
                dObj->i2cState = I2C_SCL_LOW_DATA_CHECK;
                dObj->errorTimeOut = ERROR_TIMEOUT_COUNT;
            } else {
                if  (!(dObj->errorTimeOut--))
                {
                    lBufferObj->status = DRV_I2C_BUFFER_EVENT_ERROR;    //i2c error flag
                    PLIB_TMR_Stop(dObj->tmrSource);     //stop and clear Timer
                    PLIB_TMR_Counter16BitClear(dObj->tmrSource);
                    dObj->errorTimeOut = ERROR_TIMEOUT_COUNT;
                }
            }
            break;
        // The next 6 states transition the bus between an active state, send a stop
        // signal and leave it in the idle state.
        // I2C Stop Signal Data state 1 of 6
        // SCL and SDA are both pulled low as the initial condition of a stop signal.
        case I2C_SCL_SDA_LOW_STOP:
            PLIB_PORTS_PinDirectionOutputSet(PORTS_ID_0, dObj->portSCL, dObj->pinSCL);
            PLIB_PORTS_PinDirectionOutputSet(PORTS_ID_0, dObj->portSDA, dObj->pinSDA);
            dObj->i2cState =  I2C_SCL_SDA_LOW_STOP_CHECK;
            break;
        // I2C Stop Signal Data state 2 of 6
        // No action is taken.
        case I2C_SCL_SDA_LOW_STOP_CHECK:
            dObj->i2cState = I2C_SCL_HIGH_STOP;
            break;
        // I2C Stop Signal Data state 3 of 6
        // SCL is allowed to float HIGH.
        case I2C_SCL_HIGH_STOP:
            PLIB_PORTS_PinDirectionInputSet(PORTS_ID_0, dObj->portSCL, dObj->pinSCL);
            dObj->i2cState    = I2C_SCL_HIGH_STOP_CHECK;
            dObj->errorTimeOut = ERROR_TIMEOUT_COUNT;
            break;
        // I2C Stop Signal Data state 4 of 6
        // SCL is checked to verify that it is indeed high,
        // Otherwise clock stretching by a slave is assumed.
        // If SCL is pulled low too long, an error is flagged.
        case I2C_SCL_HIGH_STOP_CHECK:
            if (PLIB_PORTS_PinGet(PORTS_ID_0, dObj->portSCL, dObj->pinSCL) == INPUT)
            {
                dObj->i2cState = I2C_SDA_HIGH_STOP;
            } else {
                if  (!(dObj->errorTimeOut--))                 //decrement error counter
                {
                    lBufferObj->status = DRV_I2C_BUFFER_EVENT_ERROR;    //i2c error flag
                    PLIB_TMR_Stop(dObj->tmrSource);     //stop and clear Timer
                    PLIB_TMR_Counter16BitClear(dObj->tmrSource);
                    dObj->errorTimeOut = ERROR_TIMEOUT_COUNT; //reset error counter
                } else {
                    PLIB_PORTS_PinDirectionInputSet(PORTS_ID_0, dObj->portSCL, dObj->pinSCL);
                }
            }
            break;
        // I2C Stop Signal Data state 5 of 6
        // SDA is allowed to float HIGH as a Stop Signal.
        case I2C_SDA_HIGH_STOP:
            PLIB_PORTS_PinDirectionInputSet(PORTS_ID_0, dObj->portSDA, dObj->pinSDA);
                                                    //set SDA to high
            dObj->i2cState    = I2C_SDA_HIGH_STOP_CHECK;
            break;
        // I2C Stop Signal Data state 6 of 6
        // After the Stop Signal has been allowed to propogate the bus
        // the queue is checked to see if any other transactions are pending.
        // This also leaves the bus in an idle state
        case I2C_SDA_HIGH_STOP_CHECK:
            if( (lBufferObj->transferSize != 0)
              || (lBufferObj->readtransferSize != 0) )
            {
                _DRV_I2C_CLIENT_OBJ(lBufferObj, status) = DRV_I2C_BUFFER_EVENT_ERROR;

            } else {
                _DRV_I2C_CLIENT_OBJ(lBufferObj, status) = DRV_I2C_BUFFER_EVENT_COMPLETE;
            }
            lBufferObj->inUse = false;

            /* Have a check here because DRV_I2C_ClientSetup function call is optional */
            if ( _DRV_I2C_CLIENT_OBJ(((DRV_I2C_CLIENT_OBJ *)lBufferObj->clientHandle), callback) != NULL )
            {
                dObj->interruptNestingCount++;

                /* Give an indication to the higher layer upon successful transmission */
                _DRV_I2C_CLIENT_OBJ(((DRV_I2C_CLIENT_OBJ *)lBufferObj->clientHandle), callback)
                        ( _DRV_I2C_CLIENT_OBJ(lBufferObj, status), (DRV_I2C_BUFFER_HANDLE)lBufferObj, 0x00 );

                dObj->interruptNestingCount--;
            }
            if ( (lBufferObj->status == DRV_I2C_BUFFER_EVENT_COMPLETE)
                 || (lBufferObj->status == DRV_I2C_BUFFER_EVENT_ERROR) )
            {
                _DRV_I2C_Advance_Queue(dObj);

                if ((_DRV_I2C_IsQueueEmpty(dObj) == true))
                {
                    PLIB_TMR_Stop(dObj->tmrSource);     //stop and clear Timer
                    PLIB_TMR_Counter16BitClear(dObj->tmrSource);
                    dObj->i2cState    = I2C_NULL_STATE;
                } else {
                    dObj->i2cState = I2C_SDA_LOW_START;
                    dObj->I2CSWCounter = 9;
                }
            }
            break;
        default:
            break;
    }

}



