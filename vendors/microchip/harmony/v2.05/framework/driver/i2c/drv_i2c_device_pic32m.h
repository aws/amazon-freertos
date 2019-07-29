/*******************************************************************************
  I2C Driver Local Data Structures

  Company:
    Microchip Technology Inc.

  File Name:
    drv_i2c_local.h

  Summary:
    I2C driver local declarations and definitions.

  Description:
    This file contains the I2C driver's local declarations and definitions.
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

#ifndef _DRV_I2C_DEVICE_PIC32M_H
#define _DRV_I2C_DEVICE_PIC32M_H

#include "peripheral/i2c/plib_i2c.h"
#include "system/clk/sys_clk.h"
#include "system/ports/sys_ports.h"

// DOM-IGNORE-BEGIN
#ifdef __cplusplus  // Provide C++ Compatibility

extern "C" {

#endif
// DOM-IGNORE-END

// *****************************************************************************
// *****************************************************************************
// Section: I2C Driver Data Types
// *****************************************************************************
// *****************************************************************************

// *****************************************************************************
/* Driver Instance Index 0

  Summary:
    Identifies the instance of the I2C driver.

  Description:
    This value identifies the instance of the I2C driver.  It is a zero-based
    index.

  Remarks:
    This number is not directly related to the peripheral hardware instance.
    The peripehral hardware instances is associated with the driver instance
    by the DRV_I2C_Initialize function.
 */

#define DRV_I2C_INDEX_0         0


// *****************************************************************************
/* Driver Instance Index 1

  Summary:
    Identifies the instance of the I2C driver.

  Description:
    This value identifies the instance of the I2C driver.  It is a zero-based
    index.

  Remarks:
    This number is not directly related to the peripheral hardware instance.
    The peripheral hardware instances is associated with the driver instance
    by the DRV_I2C_Initialize function.
 */

#define DRV_I2C_INDEX_1         1


// *****************************************************************************
/* Driver Instance Index 2

  Summary:
    Identifies the instance of the I2C driver.

  Description:
    This value identifies the instance of the I2C driver.  It is a zero-based
    index.

  Remarks:
    This number is not directly related to the peripheral hardware instance.
    The peripheral hardware instances is associated with the driver instance
    by the DRV_I2C_Initialize function.
 */

#define DRV_I2C_INDEX_2         2


// *****************************************************************************
/* Driver Instance Index 3

  Summary:
    Identifies the instance of the I2C driver.

  Description:
    This value identifies the instance of the I2C driver.  It is a zero-based
    index.

  Remarks:
    This number is not directly related to the peripheral hardware instance.
    The peripheral hardware instances is associated with the driver instance
    by the DRV_I2C_Initialize function.
 */

#define DRV_I2C_INDEX_3         3


// *****************************************************************************
/* Driver Instance Index 4

  Summary:
    Identifies the instance of the I2C driver.

  Description:
    This value identifies the instance of the I2C driver.  It is a zero-based
    index.

  Remarks:
    This number is not directly related to the peripheral hardware instance.
    The peripheral hardware instances is associated with the driver instance
    by the DRV_I2C_Initialize function.
 */

#define DRV_I2C_INDEX_4         4


// *****************************************************************************
/* Driver Instance Index 5

  Summary:
    Identifies the instance of the I2C driver.

  Description:
    This value identifies the instance of the I2C driver.  It is a zero-based
    index.

  Remarks:
    This number is not directly related to the peripheral hardware instance.
    The peripehral hardware instances is associated with the driver instance
    by the DRV_I2C_Initialize function.
 */

#define DRV_I2C_INDEX_5         5

// *****************************************************************************
/* I2C Buffer Handle

  Summary:
    Handle to an I2C data buffer.

  Description:
    This data type provides a handle to an I2C buffer that has been passed to
    the driver for processing.

  Remarks:
    This handle is provided by one of the data transfer functions
    (DRV_I2C_Receive, DRV_I2C_Transmit, DRV_I2C_TransmitThenReceive) and is used
    by the buffer event-handler (DRV_I2C_BUFFER_EVENT_HANDLER) call-back
    function and buffer status functions (DRV_I2C_TransferStatusGet,
    DRV_I2C_BytesTransferred).
*/

typedef uintptr_t DRV_I2C_BUFFER_HANDLE;

// *****************************************************************************
/* I2C Driver Invalid Buffer Handle

  Summary:
    Definition of an invalid buffer handle.

  Description:
    This is the definition of an invalid buffer handle. An invalid buffer handle
    is returned by DRV_I2C_Receive, DRV_I2C_Transmit and DRV_I2C_TransmitThenReceive
    functions if the buffer add request was not successful.

  Remarks:
    None
*/

#define DRV_I2C_BUFFER_HANDLE_INVALID /*DOM-IGNORE-BEGIN*/((DRV_I2C_BUFFER_HANDLE)(NULL))/*DOM-IGNORE-END*/

typedef unsigned char I2C_DATA_TYPE;

typedef uint8_t	 I2C_SLAVE_ADDRESS_7bit;        // need to type cast 10-bit slave address into 8 bit mode
// *****************************************************************************
/* I2C Slave Device Address

  Summary:
    Data type for a slave device address.

  Description:
    This is the data type for an I2C slave device address.  It can hold both
    7-bit and 10-bit slave addresses.

  Remarks:
    None.
*/

typedef uint16_t I2C_SLAVE_ADDRESS_VALUE;


// *****************************************************************************
/* I2C Slave Device Address Mask

  Summary:
    Data type for a slave device address bit-mask.

  Description:
    This is the data type for an I2C slave device address bit-mask.  It can hold
    both 7-bit and 10-bit slave addresses masks.

  Remarks:
    A slave address mask is used in slave mode to identify "don't care" bits
    in the base slave address to which the I2C interface will respond..
*/

typedef uint8_t DRV_I2C_SLAVE_ADDRESS_MASK;


// *****************************************************************************
/* I2C Buffer Events

  Summary:
    Lists the different conditions that happens during a buffer transfer.

  Description:
    This enumeration identifies the different conditions that can happen during
    a buffer transaction. Callbacks can be made with the appropriate buffer
    condition passed as a parameter to execute the desired action.
    The application can also poll the BufferStatus flag to check the status of
    transfer.

    The values act like flags and multiple flags can be set.

  Remarks:
    None.
*/

typedef enum
{
    /* Buffer is pending to get processed */
    DRV_I2C_BUFFER_EVENT_PENDING,

    /* All data from or to the buffer was transferred successfully. */
    DRV_I2C_BUFFER_EVENT_COMPLETE,

    /* There was an error while processing the buffer transfer request. */
    DRV_I2C_BUFFER_EVENT_ERROR,

    /*  Send Stop by Master */
    DRV_I2C_SEND_STOP_EVENT,

    /* Send Restart Event by Master */
    DRV_I2C_SEND_RESTART_EVENT,

    /* Master sends data to slave */
    DRV_I2C_BUFFER_SLAVE_READ_REQUESTED,

    /* Master requests data from slave */
    DRV_I2C_BUFFER_SLAVE_WRITE_REQUESTED,

    /* Slave read byte send by Master */
    DRV_I2C_BUFFER_SLAVE_READ_BYTE,

    /* sending ACK to Slave for more bytes */
    DRV_I2C_BUFFER_MASTER_ACK_SEND,

    /* sending ACK to Slave for more bytes */
    DRV_I2C_BUFFER_MASTER_NACK_SEND,

   /* Slave send byte to Master */
    DRV_I2C_BUFFER_SLAVE_WRITE_BYTE

} DRV_I2C_BUFFER_EVENT;


// *****************************************************************************
/* I2C Buffer Event Callback

  Function:
    typedef void (*DRV_I2C_CallBack) ( DRV_I2C_BUFFER_EVENT event,
                       void  * context )

  Summary:
    Points to a callback to initiate a particular function.

  Description:
    This type identifies the I2C Buffer Event.  It allows the client driver
    to register a callback using DRV_I2C_BUFFER_EVENT. By using this
    mechanism, the driver client will can initiate an operation. This is
    intended to be used for SLAVE mode because the Master drives the I2C bus
    and through this the Slave can issue a READ or WRITE based on the status of
    R/W bit received from the Master

  Parameters:
   DRV_I2C_BUFFER_EVENT - Status of I2C transfer

   context     - This is left for future implementation

  Remarks:
    A transfer can be composed of various transfer segments.  Once a transfer
    is completed the driver will call the client registered transfer
    callback.

    The callback could be called from ISR context and should be kept as short
    as possible.  It is meant for signaling and it should not be blocking.

*/


typedef void (*DRV_I2C_CallBack) ( DRV_I2C_BUFFER_EVENT event, void * context );


// *****************************************************************************
/* I2C Buffer Event Callback

  Function:
    void ( *DRV_I2C_BUFFER_EVENT_HANDLER ) (DRV_I2C_BUFFER_EVENT event,
                DRV_I2C_BUFFER_HANDLE bufferHandle, uintptr_t context )

  Summary:
    Points to a callback after completion of an I2C  transfer.

  Description:
    This type identifies the I2C Buffer Event.  It allows the client driver
    to register a callback using DRV_I2C_BUFFER_EVENT_HANDLER. By using this
    mechanism, the driver client will be notified at the completion of the
    corresponding transfer.

  Parameters:
    DRV_I2C_BUFFER_EVENT - Status of I2C transfer

    bufferHandle - Handle that identifies that identifies the particular Buffer
                   Object

    context     - For future implementation

  Remarks:
    A transfer can be composed of various transfer segments.  Once a transfer
    is completed the driver will call the client registered transfer
    callback.

    The callback could be called from ISR context and should be kept as short
    as possible.  It is meant for signaling and it should not be blocking.

*/

typedef void ( *DRV_I2C_BUFFER_EVENT_HANDLER ) (DRV_I2C_BUFFER_EVENT event,
                    DRV_I2C_BUFFER_HANDLE bufferHandle, uintptr_t context );


// *****************************************************************************
/* I2C Operation Mode

  Summary
    Lists the operation mode of I2C module.

  Description
    This enumeration lists if the I2C module is configured as a Master or
    a Slave.

  Remarks:
    None.
*/

typedef enum
{
    /* I2C Mode Master */
    DRV_I2C_MODE_MASTER     /*DOM-IGNORE-BEGIN*/  = 0 /*DOM-IGNORE-END*/,

    /* I2C Mode Slave */
    DRV_I2C_MODE_SLAVE      /*DOM-IGNORE-BEGIN*/  = 1 /*DOM-IGNORE-END*/

} DRV_I2C_MODE;

// *****************************************************************************
/* I2C Configuration Choice

  Summary
    Lists the operation mode of I2C module.

  Description
    This enumeration lists if the I2C module is configured to use I2C
    peripheral or use Bit-Bang implementation.

  Remarks:
    None.
*/

typedef enum
{
    /* Make use of the I2C Peripheral. */
    DRV_I2C_PERIPHERAL
        /*DOM-IGNORE-BEGIN*/  = 0 /*DOM-IGNORE-END*/,

    /* Make use of the I2C Bit Bang Algorithm */
    DRV_I2C_BIT_BANG
        /*DOM-IGNORE-BEGIN*/  = 1 /*DOM-IGNORE-END*/

} DRV_I2C_PERPH;


// *****************************************************************************
/* I2C Slave Address Width

  Summary:
    Lists the Address Width of the Slave.

  Description:
    This enumeration lists if the I2C module is configured as a 7-bit Slave or
    a 10-bit Slave.

  Remarks:
    None.
*/

typedef enum
{

   /* I2C Slave 7 bit  */
    DRV_I2C_7BIT_SLAVE      /*DOM-IGNORE-BEGIN*/  = 1 /*DOM-IGNORE-END*/,

    /* I2C Slave 10 bit  */
    DRV_I2C_10BIT_SLAVE     /*DOM-IGNORE-BEGIN*/  = 2 /*DOM-IGNORE-END*/

} DRV_I2C_ADDRESS_WIDTH;


// *****************************************************************************
/* I2C Bus Levels

  Summary:
    Lists the Operational Voltage level of I2C.

  Description:
    This enumeration lists if the I2C is configured to operate in the traditional
    I2C mode or the SMBus mode.

  Remarks:
    None.
*/

typedef enum
{

    /* I2C BUS LEVEL */
    DRV_I2C_OPEN_COLLECTOR_LEVEL    /*DOM-IGNORE-BEGIN*/  = 0 /*DOM-IGNORE-END*/,

    /* SMBus level */
    DRV_I2C_SMBus_LEVEL             /*DOM-IGNORE-BEGIN*/  = 1 /*DOM-IGNORE-END*/

} DRV_I2C_BUS_LEVEL;


// *****************************************************************************
/* I2C Bus Speed

  Summary
    Lists the I2C Bus speed mode.

  Description
    This enumeration lists if the I2C is configured to operate at High-Speed or
    Normal Speed.

  Remarks:
    None.
*/

typedef enum
{
    DRV_I2C_NORMAL_SPEED    /*DOM-IGNORE-BEGIN*/  = 0 /*DOM-IGNORE-END*/,

    DRV_I2C_HIGH_SPEED      /*DOM-IGNORE-BEGIN*/  = 1 /*DOM-IGNORE-END*/

}DRV_I2C_BUS_SPEED;



typedef enum
{
    /* normal I2C with not ignoring any error */
    DRV_I2C_HALT_ON_ERROR = 0x00,

    /* ignore bus collision error */
    DRV_I2C_BUS_IGNORE_COLLISION_ERROR = 0x01,

    /* ignore overflow error */
    DRV_I2C_BUS_IGNORE_OVERFLOW_ERROR = 0x02,

}DRV_I2C_BUS_ERROR_EVENT;


/*******************************************************************************
 I2C Initialization Values

   Summary:
    Identifies the initialization values that are passed as parameters to the
    initialize and reinitialize routines of the I2C module.

   Description:
    This enumeration identifies the values that are passed as parameters to the
    initialize and reinitialize routines of the I2C module.

    These values specify different I2C module initialization parameters.
*/

typedef  struct _DRV_I2C_INIT
{
    /* System module initialization */
    SYS_MODULE_INIT                         moduleInit;

    /* Identifies peripheral (PLIB-level) ID */
    I2C_MODULE_ID                           i2cId;

    /* I2C Usage Mode Type */
    DRV_I2C_MODE                            i2cMode;

    /* I2C Peripheral or not usage */
    DRV_I2C_PERPH                    		i2cPerph;

    /* PORT which SCL belongs */
    PORTS_CHANNEL                           portSCL;

    /* Bit position in the port for SCL */
    PORTS_BIT_POS                           pinSCL;

    /* PORT which SDA belongs */
    PORTS_CHANNEL                           portSDA;

    /* Bit position in the port for SDA */
    PORTS_BIT_POS                           pinSDA;

    /* Communication Width */
    DRV_I2C_ADDRESS_WIDTH                   addWidth;

    /* Reserved Address rule enable */
    bool                                    reservedaddenable;

    /* General Call address enable */
    bool                                    generalcalladdress;

    /* Slave address value*/
    I2C_SLAVE_ADDRESS_VALUE                 slaveaddvalue;

    /* Baud Rate Value */
    uint32_t                                baudRate;

    /* I2C Clock mode */
    DRV_I2C_BUS_LEVEL                       buslevel;

    /*I2C Bus Speed */
    DRV_I2C_BUS_SPEED                       busspeed;

    /* Master Interrupt Source for I2C module */
    INT_SOURCE                              mstrInterruptSource;

    /* Slave Interrupt Source for I2C module */
    INT_SOURCE                              slaveInterruptSource;

    /* Error Interrupt Source for I2C module */
    INT_SOURCE                              errInterruptSource;

    /*  This is the buffer queue size. This is the maximum
        number of  transfer requests that driver will queue. For a
        static build of the driver, this is overridden by the
        DRV_I2C_QUEUE_SIZE macro in system_config.h             */
    unsigned int                            queueSize;

    /*  This callback is fired when an operation needs to be initiated on the
    I2C bus.  This callback is mainly intended when the driver is used in
    SLAVE mode and is required to send data to the Master. The callback
    signals the application to prepare data for transfer to Master.
    This callback may be called from an ISR so should not include OSAL
    calls.  The context parameter is the same one passed into the
    BufferAddRead, BufferAddWrite, BufferAddWriteRead function.     */

    DRV_I2C_CallBack                        operationStarting;

    /*  Slave address Mask value, the I2C slave address match ignored for the
        bit fields that the mask is set
    */

    DRV_I2C_SLAVE_ADDRESS_MASK              maskslaveaddress;

} DRV_I2C_INIT;


//DOM-IGNORE-BEGIN
#ifdef __cplusplus
}
#endif
//DOM-IGNORE-END

#endif //#ifndef _DRV_I2C_DEVICE_PIC32M_H

/*******************************************************************************
 End of File
*/
