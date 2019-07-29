/*******************************************************************************
 I2C Device Driver Interface Header File

  Company:
    Microchip Technology Inc.

  File Name:
    drv_i2c_compatibility.h

  Summary:
    I2C module driver interface header. This is maintained to keep it 
    backward compatible with the previous releases of I2C driver

  Description:
    This header file contains the function prototypes and definitions of the
    data types and constants that make up the interface to the I2C module
    driver.
*******************************************************************************/

//DOM-IGNORE-BEGIN
/*******************************************************************************
Copyright © 2014 released Microchip Technology Inc.  All rights
reserved.

Microchip licenses to you the right to use, modify, copy and distribute
Software only when embedded on a Microchip microcontroller or digital signal
controller that is integrated into your product or third party product
(pursuant to the sublicense terms in the accompanying license agreement).

You should refer to the license agreement accompanying this Software for
additional information regarding your rights and obligations.

SOFTWARE AND DOCUMENTATION ARE PROVIDED ?AS IS? WITHOUT WARRANTY OF ANY KIND,
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

#ifndef _DRV_I2C_COMPATIBILITY_H_
#define _DRV_I2C_COMPATIBILITY_H_

#include <stdint.h>
#include <stdbool.h>
#include "system_config.h"
#include "driver/driver_common.h"		// Common Driver Definitions
#include "peripheral/i2c/plib_i2c.h"		// I2C PLIB Header
#include "system/common/sys_common.h"      	// Common System Service Definitions
#include "system/common/sys_module.h"      	// Module/Driver Definitions
#include "system/int/sys_int.h"            	// System Interrupt Definitions
#include "system/clk/sys_clk.h"
#include "system/ports/sys_ports.h"

// DOM-IGNORE-BEGIN
#ifdef __cplusplus  

    extern "C" {

#endif
// DOM-IGNORE-END  





/*******************************************************************************
  Function:
    DRV_I2C_BUFFER_HANDLE DRV_I2C_BufferAddRead (   DRV_HANDLE handle,
                                                    uint8_t* slaveaddress,
                                                    void *rxBuffer,
                                                    size_t size,
                                                    void * context);

  Summary:
    This function reads data written from either Master or Slave.
	<p><b>Implementation:</b> Dynamic</p>

  Description:
    Master calls this function to read data send by Slave. The Slave calls this
	function to read data send by Master.
    In case of Master, a START condition is initiated on the I2C bus.

  Precondition:
    The DRV_I2C_Initialize routine must have been called for the specified I2C
    device instance and the DRV_I2C_Status must have returned SYS_STATUS_READY.

    DRV_I2C_Open must have been called to obtain a valid opened device handle.

  Parameters:
    handle      - A valid open-instance handle, returned from the driver's open
                  routine
    address     - Device address of slave. If this API is used in Slave mode,
                  then a dummy value can be used
    buffer      - This buffer holds data is received
    size        - The number of bytes that the Master expects to read from Slave.
                  This value can be kept as the MAX BUFFER SIZE for slave.
                  This is because the Master controls when the READ operation
                  is terminated.
    callbackContext     - Not implemented, future expansion

  Returns:
    None.

  Example:
    <code>
        drvI2CRDBUFHandle = DRV_I2C_BufferAddRead( appData.drvI2CHandle,
                    &deviceaddress,
                    rxbuffer, num_of_bytes, NULL );
    </code>

  Remarks:
    The handle that is passed into the function, drvI2CHandle is obtained by 
    calling the DRV_I2C_OPEN function.  If the function could not return a 
    valid buffer handle, then a NULL value is returned. 
*/

DRV_I2C_BUFFER_HANDLE DRV_I2C_BufferAddRead (   DRV_HANDLE handle, 
                                                uint8_t* address,
                                                void *buffer,
                                                size_t size,
                                                void * callbackContext);

/*******************************************************************************
  Function:
    DRV_I2C_BUFFER_HANDLE DRV_I2C_BufferAddWrite (  DRV_HANDLE handle,
                                                    uint8_t* slaveaddress,
                                                    void *txBuffer,
                                                    size_t size,
                                                    void * context);

  Summary:
    This function writes data to Master or Slave.
	<p><b>Implementation:</b> Dynamic</p>

  Description:
    Master calls this function to write data to Slave. The Slave calls this function
	to write data to Master.
    In case of Master, a START condition is initiated on the I2C bus

  Precondition:
    The DRV_I2C_Initialize routine must have been called for the specified I2C
    device instance and the DRV_I2C_Status must have returned SYS_STATUS_READY.

    DRV_I2C_Open must have been called to obtain a valid opened device handle.

  Parameters:
    handle      - A valid open-instance handle, returned from the driver's open
                  routine
    address     - Device address of slave. If this API is used in Slave mode,
                  then a dummy value can be used
    buffer      - Contains data to be transferred
    size        - The number of bytes that the Master expects to write to Slave.
                  This value can be kept as the MAX BUFFER SIZE for slave.
                  This is because the Master controls when the WRITE operation
                  is terminated.
    callbackContext     - Not implemented, future expansion

  Returns:
    None.

  Example:
    <code>
       drvI2CWRBUFHandle = DRV_I2C_BufferAddWrite(appData.drvI2CHandle,
              deviceaddress,
              (I2C_DATA_TYPE *)&appData.drvI2CTXbuffer[0], num_of_bytes, NULL);
    </code>

  Remarks:
    The handle that is passed into the function, drvI2CHandle is obtained by 
    calling the DRV_I2C_OPEN function.  If the function could not return a 
    valid buffer handle, then a NULL value is returned. 
*/

DRV_I2C_BUFFER_HANDLE DRV_I2C_BufferAddWrite (  DRV_HANDLE handle, 
                                                uint8_t* address,
                                                void *buffer,
                                                size_t size,
                                                void * context);

/*******************************************************************************
  Function:
DRV_I2C_BUFFER_HANDLE DRV_I2C_BufferAddWriteRead ( DRV_HANDLE handle,
                                    uint8_t* slaveaddress,
                                    void *txBuffer,
                                    size_t wsize,
                                    void *rxBuffer,
                                    size_t rsize,
                                    void * context);

  Summary:
    This function writes data to Slave, inserts restart and requests read from
    slave.
	<p><b>Implementation:</b> Dynamic</p>

  Description:
    Master calls this function to send a register address value to the slave and
    then queries the slave with a read request to read the contents indexed by
    the register location. The Master sends a restart condition after the
    initial write before sending the device address with R/W = 1. The restart
    condition prevents the Master from relinquishing the control of the bus. The
    slave should not use this function.

  Precondition:
    The DRV_I2C_Initialize routine must have been called for the specified I2C
    device instance and the DRV_I2C_Status must have returned SYS_STATUS_READY.

    DRV_I2C_Open must have been called to obtain a valid opened device handle.

  Parameters:
    handle      - A valid open-instance handle, returned from the driver's open
                  routine
    address     - Device address of slave. If this API is used in Slave mode,
                  then a dummy value can be used
    writeBuffer - Contains data to be transferred
    writeSize   - The number of bytes that the Master expects to write to Slave.
                  This value can be kept as the MAX BUFFER SIZE for slave.
                  This is because the Master controls when the WRITE operation
                  is terminated.
    readBuffer  - This buffer holds data that is send back from slave after
                  read operation.
    readSize       - The number of bytes the Master expects to be read from the
                  slave
    callbackContext     - Not implemented, future expansion

  Returns:
    None.

  Example:
    <code>
        drvI2CRDBUFHandle = DRV_I2C_BufferAddWriteRead( appData.drvI2CHandle,
                    deviceaddress,
                    (I2C_DATA_TYPE *)&appData.drvI2CTXbuffer[0], registerbytesize,
                    rxbuffer, num_of_bytes, NULL );
    </code>

  Remarks:
    The handle that is passed into the function, drvI2CHandle is obtained by 
    calling the DRV_I2C_OPEN function.  If the function could not return a 
    valid buffer handle, then a NULL value is returned. 
*/

DRV_I2C_BUFFER_HANDLE DRV_I2C_BufferAddWriteRead (  DRV_HANDLE handle, 
                                                    uint8_t* address,
                                                    void *writeBuffer,
                                                    size_t writeSize,
                                                    void *readBuffer,
                                                    size_t readSize,
                                                    void * callbackContext);

/*****************************************************************************
  Function:
    DRV_I2C_BUFFER_EVENT DRV_I2C_BufferStatus ( DRV_I2C_BUFFER_HANDLE bufferHandle )

  Summary:
    Returns status of data transfer when Master or Slave acts either as a
	transmitter or a receiver.
	<p><b>Implementation:</b> Dynamic</p>

  Description:
	The bufferHandle parameter contains the buffer handle of the buffer that
    associated with the event.
	If the event is DRV_I2C_BUFFER_EVENT_COMPLETE, it means that the data was
    transferred successfully.
    If the event is DRV_I2C_BUFFER_EVENT_ERROR, it means that the data was not
    transferred successfully.

  Parameters:
    handle          -  A valid open-instance handle, returned from the driver's 
                       open routine 
    bufferHandle    -  A valid open-instance handle, returned when calling the 
                       BufferAddRead/BufferAddWrite/BufferAddReadWrite function

	Example:
    <code>
	if(DRV_I2C_BUFFER_EVENT_COMPLETE == DRV_I2C_BufferStatus (appData.drvI2CWRBUFHandle))
	{
            //perform action
            return true;
	}
	else
	{
            //perform action
            return false;
	}
    </code>

  Returns:
    A DRV_I2C_TRANSFER_STATUS value describing the current status of the
    transfer.
*/

DRV_I2C_BUFFER_EVENT DRV_I2C_BufferStatus ( DRV_I2C_BUFFER_HANDLE bufferHandle);


// *****************************************************************************
/* Function:
    void DRV_I2C_Bus_Reset ( SYS_MODULE_OBJ object )

  Summary:
    Resets any slaves on the I2C bus which might be stuck in thier internal state
    machines.  

  Description:
    This returns the transmitter and receiver transfer status.  From the I2C 
    Specification:
    "If the data line (SDA) is stuck LOW, the master should send nine clock pulses. 
    The device that held the bus LOW should release it sometime within those nine 
    clocks. If not, then use the HW reset or cycle power to clear the bus."
    This function sends a Start signal, 9 clocks, and then a Stop singal.

  Parameters:
    object          - Identifies the Driver Object returned by the Initialize
                      interface

  Returns:
  
  Remarks:
    NOTE:   This is a blocking function.  It is NOT multi client friendly.  It 
            should be used with caution.  It is meant to be used in a single 
            Master environment where the PIC using it is the sole Master device
            on the I2C bus.  Under normal conditions, it should only be 
            necessary during development.  Reseting the PIC32 without resetting
            slave devices on the I2C bus can sometimes cause them to be stuck
            in the middle of a reception or transmission.  This function can
            clear the bus in those cases.
*/
void DRV_I2C_Bus_Clear ( SYS_MODULE_OBJ object );

/*******************************************************************************
  Function:
    void DRV_I2C_IRQEventSend(DRV_HANDLE handle);

  Summary:
    IRQ line logic to let Master know that Slave has data to be sent.
	<p><b>Implementation:</b> Dynamic</p>

  Description:
    The Slave can call this function to toggle the IRQ line to request the
    Master for a READ (i.e., Slave has data to be send to Master).

  Precondition:
    The DRV_I2C_Initialize routine must have been called for the specified I2C
    device instance and the DRV_I2C_Status must have returned SYS_STATUS_READY.

    DRV_I2C_Open must have been called to obtain a valid opened device handle.

    DRV_I2C_ClientSetup should have been called to configure IRQ port pin

  Parameters:
    drvHandle   - A valid open-instance handle, returned from the driver's open
                  routine
  Returns:
    None.

  Example:
    <code>
       DRV_I2C_IRQEventSend(appData.drvI2CHandle);
    </code>

  Remarks:
    The handle that is passed into the function, drvI2CHandle is obtained by 
    calling the DRV_I2C_OPEN function. 
*/

void DRV_I2C_IRQEventSend(DRV_HANDLE handle);

/*******************************************************************************
  Function:
    void DRV_I2C_StopEventSend(DRV_HANDLE handle);

  Summary:
    Master sends STOP condition on I2C bus indicating end of WRITE or READ.
	<p><b>Implementation:</b> Dynamic</p>

  Description:
    Master sends STOP condition after completing a WRITE or READ. This will
    relinquish the control of I2C bus and another Master is free to take control
    of the bus.

  Precondition:
    The DRV_I2C_Initialize routine must have been called for the specified I2C
    device instance and the DRV_I2C_Status must have returned SYS_STATUS_READY.

    DRV_I2C_Open must have been called to obtain a valid opened device handle.

    DRV_I2C_BufferAddWrite/DRV_I2C_BufferAddRead which performs a START and
    WRITE or READ operation on the bus.

  Parameters:
    object   - Instance of the I2C object

  Returns:
    None.

  Example:
    <code>
       DRV_I2C_StopEventSend(appData.drvI2CHandle)
    </code>

  Remarks:
    None.
*/
void DRV_I2C_StopEventSend(DRV_HANDLE handle);

/*******************************************************************************
  Function:
    void DRV_I2C_RestartEventSend(DRV_HANDLE handle);

  Summary:
    Master sends STOP condition on I2C bus indicating end of WRITE or READ.
	<p><b>Implementation:</b> Dynamic</p>

  Description:
    Master sends RESTART condition. after completing a WRITE. This is done when
    the Master does not want to relinquish control of bus but want to start
    another transaction. This usually happens when the Master wants to send
    a read data from a specific register in the slave device.

  Precondition:
    The DRV_I2C_Initialize routine must have been called for the specified I2C
    device instance and the DRV_I2C_Status must have returned SYS_STATUS_READY.

    DRV_I2C_Open must have been called to obtain a valid opened device handle.

    DRV_I2C_BufferAddWriteRead which performs a START but instead of terminating
    the I2C transaction with a STOP, a RESTART issued on the bus.

  Parameters:
    object   - Instance of the I2C object

  Returns:
    None.

  Example:
    <code>
       DRV_I2C_RestartEventSend(appData.drvI2CHandle)
    </code>

  Remarks:
    None.
*/


void DRV_I2C_RestartEventSend(DRV_HANDLE handle);
                                            

// *****************************************************************************
/* Function:
    bool DRV_I2C_SetUpByteRead(I2C_MODULE_ID i2cid)

  Summary:
    Prepare Master or Slave for receiving data.
	<p><b>Implementation:</b> Static</p>

  Description:
    Sets Receiver Enable for Master. When operating in slave mode, releases the
    Clock for Slave.

  Parameters:
    i2cid        - Instance of the module ID being used.

  Returns:
    Status of the operation.
*/
bool DRV_I2C_SetUpByteRead(I2C_MODULE_ID i2cid);

// *****************************************************************************
/* Function:
    bool DRV_I2C_WaitForReadByteAvailable(I2C_MODULE_ID i2cid)

  Summary:
    Checks if data is available in the receive register.
	<p><b>Implementation:</b> Static</p>

  Description:
    Checks if data is available in the receive register, by checking the status
    of RBF. Returns TRUE if data is available and FALSE if RBF = 0.

  Parameters:
    i2cid        - Instance of the module ID being used.

  Returns:
    Status of the operation.
*/

bool DRV_I2C_WaitForReadByteAvailable(I2C_MODULE_ID i2cid);

// *****************************************************************************
/* Function:
    uint8_t DRV_I2C_ByteRead(I2C_MODULE_ID i2cid)

  Summary:
    Reads data from the receive register.
	<p><b>Implementation:</b> Static</p>

  Description:
    Reads data from the receive register.

  Parameters:
    i2cid        - Instance of the module ID being used.

  Returns:
    Byte that was read from the I2CxRCV register.
*/

uint8_t DRV_I2C_ByteRead(I2C_MODULE_ID i2cid);

// *****************************************************************************
/* Function:
    bool DRV_I2C_ByteWrite(I2C_MODULE_ID i2cid, const uint8_t byte)

  Summary:
    Writes data byte to the appropriate module.
	<p><b>Implementation:</b> Static</p>

  Description:
    Writes data to the transmit register.

  Parameters:
    i2cid        - Instance of the module ID being used and data byte.

  Returns:
    TRUE if byte write is successful.
*/

bool DRV_I2C_ByteWrite(I2C_MODULE_ID i2cid, const uint8_t byte);

// *****************************************************************************
/* Function:
    bool DRV_I2C_WaitForByteWriteToComplete(I2C_MODULE_ID i2cid)

  Summary:
    Checks for the write operation to be completed.
	<p><b>Implementation:</b> Static</p>

  Description:
    Waits for data write to be completed. Checks status of TBF and TRSTAT to
    determine if write is completed.

  Parameters:
    i2cid        - Instance of the module ID being used

  Returns:
    TRUE if write operation has completed.
*/

bool DRV_I2C_WaitForByteWriteToComplete(I2C_MODULE_ID i2cid);

// *****************************************************************************
/* Function:
    bool DRV_I2C_WriteByteAcknowledged(I2C_MODULE_ID i2cid)

  Summary:
    Checks if byte written was ACK'ed or NACK'ed.
	<p><b>Implementation:</b> Static</p>

  Description:
    Checks if byte written was ACK'ed or NACK'ed.

  Parameters:
    i2cid        - Instance of the module ID being used.

  Returns:
    TRUE if ACK'ed; FALSE if NACK'ed.
*/

bool DRV_I2C_WriteByteAcknowledged(I2C_MODULE_ID i2cid);

// *****************************************************************************
/* Function:
    bool DRV_I2C_ReceiverBufferIsEmpty(I2C_MODULE_ID i2cid);

  Summary:
    Checks if no byte is present in Receive register.
	<p><b>Implementation:</b> Static</p>

  Description:
    Checks if byte is present in receive register.

  Parameters:
    i2cid        - Instance of the module ID being used.

  Returns:
    TRUE if empty.
*/

bool DRV_I2C_ReceiverBufferIsEmpty(I2C_MODULE_ID i2cid);


// *****************************************************************************
// Section: Instance  I2C Master functions
// *****************************************************************************
// *****************************************************************************
/* Function:
    void DRV_I2C_BaudRateSet(I2C_MODULE_ID i2cid, I2C_BAUD_RATE baudRate)

  Summary:
    Sets the Baud-Rate for the I2C module.
	<p><b>Implementation:</b> Static</p>

  Description:
    Sets baud rate for the I2C module.

  Parameters:
    i2cid       - Instance of the module ID being used.
    baudRate    - Baud-Rate of the I2C module.

  Returns:
    None.
*/
void DRV_I2C_BaudRateSet(I2C_MODULE_ID i2cid, I2C_BAUD_RATE baudRate);

// *****************************************************************************
/* Function:
    bool DRV_I2C_MasterBusIdle(I2C_MODULE_ID i2cid)

  Summary:
    Checks if the I2C bus is idle.
	<p><b>Implementation:</b> Static</p>

  Description:
    Checks if the last 5 bytes of the I2CxCON register == 0.

  Parameters:
    i2cid       - Instance of the module ID being used.

  Returns:
    - TRUE - if I2C Bus is idle
	- FALSE - if I2C Bus is not idle
*/

bool DRV_I2C_MasterBusIdle(I2C_MODULE_ID i2cid);

// *****************************************************************************
/* Function:
    bool DRV_I2C_MasterStart(I2C_MODULE_ID i2cid)

  Summary:
    Issues a START on I2C bus.
	<p><b>Implementation:</b> Static</p>

  Description:
    Checks for any overflow on Receiver or Transmitter side and Bus-Collision.
    If no error condition is present, then START is issued.

  Parameters:
    i2cid       - Instance of the module ID being used.

  Returns:
    - TRUE - if START was issued
	- FALSE - if any error condition exists on I2C bus
*/

bool DRV_I2C_MasterStart(I2C_MODULE_ID i2cid);

// *****************************************************************************
/* Function:
    bool DRV_I2C_MasterRestart(I2C_MODULE_ID i2cid)

  Summary:
    Issues a START on I2C bus.
	<p><b>Implementation:</b> Static</p>

  Description:
    Checks for any overflow on Receiver or Transmitter side and Bus-Collision.
    If no error condition is present, then START is issued.

  Parameters:
    i2cid       - Instance of the module ID being used.

  Returns:
    - TRUE - if RESTART was issued
	- FALSE - if any error condition exists on I2C bus
*/

bool DRV_I2C_MasterRestart(I2C_MODULE_ID i2cid);

// *****************************************************************************
/* Function:
    bool DRV_I2C_WaitForStartComplete(I2C_MODULE_ID i2cid)

  Summary:
    Checks if a successful START was issued on the I2C bus.
	<p><b>Implementation:</b> Static</p>

  Description:
    Waits for START condition to be present on the bus.

  Parameters:
    i2cid       - Instance of the module ID being used.

  Returns:
    TRUE if START is detected.
*/

bool DRV_I2C_WaitForStartComplete(I2C_MODULE_ID i2cid);

// *****************************************************************************
/* Function:
    bool DRV_I2C_MasterStop(I2C_MODULE_ID i2cid)

  Summary:
    Issues a STOP on I2C bus.
	<p><b>Implementation:</b> Static</p>

  Description:
    Checks for any error on the bus.
    If no error condition is present, then STOP is issued.

  Parameters:
    i2cid       - Instance of the module ID being used.

  Returns:
    - TRUE - if STOP was issued
	- FALSE - if any error condition exists on I2C bus
*/

bool DRV_I2C_MasterStop(I2C_MODULE_ID i2cid);

// *****************************************************************************
/* Function:
    bool DRV_I2C_WaitForStopComplete(I2C_MODULE_ID i2cid)

  Summary:
    Checks if a successful STOP was issued on the I2C bus.
	<p><b>Implementation:</b> Static</p>

  Description:
    Waits for STOP condition to be present on the bus.

  Parameters:
    i2cid       - Instance of the module ID being used.

  Returns:
    TRUE if STOP is detected.
*/

bool DRV_I2C_WaitForStopComplete(I2C_MODULE_ID i2cid);

// *****************************************************************************
/* Function:
    void DRV_I2C_MasterACKSend(I2C_MODULE_ID i2cid)

  Summary:
    Sends an ACK to the slave.
	<p><b>Implementation:</b> Static</p>

  Description:
    After reading a byte from SLAVE, Master sends an ACK if it expects more data.

  Parameters:
    i2cid       - Instance of the module ID being used.

  Returns:
    None.
*/

void DRV_I2C_MasterACKSend(I2C_MODULE_ID i2cid);

// *****************************************************************************
/* Function:
    void DRV_I2C_MasterNACKSend(I2C_MODULE_ID i2cid)

  Summary:
    Sends an ACK to the slave.
	<p><b>Implementation:</b> Static</p>

  Description:
    After reading a byte from SLAVE, Master sends an NACK if it doesn't want
    anymore data from the slave. Slave stops transmission when it receives NACK.

  Parameters:
    i2cid       - Instance of the module ID being used.

  Returns:
    None.
*/

void DRV_I2C_MasterNACKSend(I2C_MODULE_ID i2cid);

// *****************************************************************************
/* Function:
    bool DRV_I2C_WaitForACKOrNACKComplete(I2C_MODULE_ID i2cid)

  Summary:
    Waits for ACK/NACK sequence to be complete.
	<p><b>Implementation:</b> Static</p>

  Description:
    Master waits for the ACK/NACK to be completed it sends to the slave
    after it receives a byte from slave. Only after ACK/NACK is complete that
    Master either sets RCEN or sends a STOP.

  Parameters:
    i2cid       - Instance of the module ID being used.

  Returns:
    TRUE if ACK/NACK sequence is complete.
*/

bool DRV_I2C_WaitForACKOrNACKComplete(I2C_MODULE_ID i2cid);

#include "driver/i2c/drv_i2c_mapping.h"

// DOM-IGNORE-BEGIN
#ifdef __cplusplus
}
#endif
// DOM-IGNORE-END

#endif // _DRV_I2C_COMPATIBILITY_H_

/*******************************************************************************
 End of File
*/

