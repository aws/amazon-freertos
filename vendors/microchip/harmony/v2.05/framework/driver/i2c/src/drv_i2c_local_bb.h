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

#ifndef _DRV_I2C_LOCAL_H
#define _DRV_I2C_LOCAL_H


// *****************************************************************************
// *****************************************************************************
// Section: File includes
// *****************************************************************************
// *****************************************************************************

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

#include "driver/i2c/drv_i2c_bb_a.h"
#include "drv_i2c_variant_mapping.h"
#include "osal/osal.h"

// *****************************************************************************
// *****************************************************************************
// Section: Version Numbers
// *****************************************************************************
// *****************************************************************************
/* Versioning of the driver */

// *****************************************************************************
/* I2C Driver Version Macros

  Summary:
    I2C driver version.

  Description:
    These constants provide I2C driver version information. The driver
    version is:
    DRV_I2C_VERSION_MAJOR.DRV_I2C_VERSION_MINOR.DRV_I2C_VERSION_PATCH.
    It is represented in DRV_I2C_VERSION as:
    MAJOR *10000 + MINOR * 100 + PATCH, so as to allow comparisons.
    It is also represented in string format in DRV_I2C_VERSION_STR.
    DRV_I2C_TYPE provides the type of the release when the release is alpha
    or beta. The interfaces DRV_I2C_VersionGet and DRV_I2C_VersionStrGet
    provide interfaces to the access the version and the version string.

  Remarks:
    Modify the return value of DRV_I2C_VersionStrGet and DRV_I2C_VERSION_MAJOR,
    DRV_I2C_VERSION_MINOR, DRV_I2C_VERSION_PATCH, and DRV_I2C_VERSION_TYPE.
*/

#define _DRV_I2C_VERSION_MAJOR         0
#define _DRV_I2C_VERSION_MINOR         2
#define _DRV_I2C_VERSION_PATCH         0
#define _DRV_I2C_VERSION_TYPE          "alpha"
#define _DRV_I2C_VERSION_STR           "0.02 alpha"


// *****************************************************************************
/* I2C data object size

  Summary
    Specifies driver data object size.

  Description
    This type specifies the driver data object size.

  Remarks:
  	None.
*/

#define DRV_I2C_NUM_OF_BUFFER_OBJECTS        		7


// *****************************************************************************
/* I2C Driver task states

  Summary
    Lists the different states that I2C task routine can have.

  Description
    This enumeration lists the different states that I2C task routine can have.

  Remarks:
    None.
*/

typedef enum
{
    I2C_NULL_STATE = 0,
    
    /* Start condition on I2C BUS with SDA low */
    I2C_SDA_LOW_START,
            
    /* I2C SDA low start check */
    I2C_SDA_LOW_START_CHECK,

    /* Start condition on I2C Bus with SCL low after SDA low */
    I2C_SCL_LOW_START,
            
    /* I2C SCL low start check */
    I2C_SCL_LOW_START_CHECK,
            
    /* setting SDA high on RESTART */
    I2C_SDA_HIGH_RESTART,
            
    /* check SDA high RESTART */
    I2C_SDA_HIGH_RESTART_CHECK,
            
    /* setting SCL high on RESTART */
    I2C_SCL_HIGH_RESTART,
            
    /* check SCL high on RESTART */
    I2C_SCL_HIGH_RESTART_CHECK,

    /* SCL High during data transfer to ensure Data is not changing */
    I2C_SCL_HIGH_DATA,
            
    /* High data check */
    I2C_SCL_HIGH_DATA_CHECK,

    /* SCL Low during data transfer where data can change */
    I2C_SCL_LOW_DATA,
            
    /* SCL low data check */
    I2C_SCL_LOW_DATA_CHECK,
            
    /* keep SCL and SDA low for 1 BRG time */
    I2C_SCL_SDA_LOW_STOP,      
            
    /* keep SCL and SDA low check */
    I2C_SCL_SDA_LOW_STOP_CHECK,

    /* SCL going high during STOP condition */            
    I2C_SCL_HIGH_STOP,
            
    /* SCL going HIGH check*/
    I2C_SCL_HIGH_STOP_CHECK,
            
    /* SDA going low during STOP condition */
    I2C_SDA_HIGH_STOP,
                
    /* SDA going high STOP check */
    I2C_SDA_HIGH_STOP_CHECK

} BUS_STATE;


typedef enum
{
    /* Process queue */
    DRV_I2C_TASK_SEND_DEVICE_ADDRESS,

   /* I2C task handle write only buffer request */
    DRV_I2C_TASK_PROCESS_WRITE_ONLY,

   /* I2C task handle read only buffer request */
    DRV_I2C_TASK_PROCESS_READ_ONLY,

   /* I2C task handle for Read Write function */
    DRV_I2C_TASK_PROCESS_WRITE_READ,

   /* Set I2C Task to set RCEN flag */
    DRV_I2C_TASK_SET_RCEN_ONLY,

   /* Set I2C Task to issue RESTART */
    DRV_I2C_TASK_PROCESS_RESTART,

   /* I2C Bus Transaction Complete */
    DRV_I2C_BUS_SILENT,
            
  /* Send device address for Random Read */
    DRV_I2C_SEND_RANDOM_READ_DEVICE_ADDRESS,         

   /* Set I2C Task to issue STOP */
    DRV_I2C_TASK_PROCESS_STOP


} DRV_I2C_DATA_OBJECT_TASK;


// *****************************************************************************
/* I2C Driver task states

  Summary
    Lists the I2C operations.

  Description
    This enumeration lists all the I2C operations.

  Remarks:
    None.
*/

typedef enum
{

    /* I2C operation write only buffer request  */
    DRV_I2C_OP_WRITE = 0x00,

    /* I2C operation read only buffer request   */
    DRV_I2C_OP_READ = 0x01,

    /* I2C operation write followed by read     */
    DRV_I2C_OP_WRITE_READ = 0x02

} DRV_I2C_OPERATIONS;


// *****************************************************************************
/* I2C Driver State

  Summary
    Lists the different states of the I2C module

  Description
    This enumeration lists the various sub-states pertaining to Master or Slave
    during transactions
 *
  Remarks:
    None.
*/

typedef enum
{
    /* Master sends data to slave */
    DRV_I2C_SUB_STATE_IDLE = 0x00,

    /* Master needs to send ACK to Slave after each read of 1 to n-1 byte */
    DRV_I2C_MASTER_READ_ACK_TO_SLAVE = 0x01,

     /* Master needs to send NACK to Slave after read of nth byte */
    DRV_I2C_MASTER_READ_NACK_TO_SLAVE = 0x02,

} DRV_I2C_MODULE_SUB_STATE;


// *****************************************************************************
/* I2C Module State

  Summary
    Lists the different states of the I2C module

  Description
    This enumeration lists the various states the I2C module is in during
    various transactions
 *
  Remarks:
    None.
*/

typedef enum
{
    DRV_I2C_MODULE_IDLE = 0x00,

    /* Master reads data from slave */
    DRV_I2C_MASTER_TX_TO_SLAVE = 0x01,

    /* Master reads data from slave */
    DRV_I2C_MASTER_RX_FROM_SLAVE = 0x02,

    /* I2C Slave ready to receive data from Master */
    DRV_I2C_SLAVE_READY_TO_RX_FROM_MASTER = 0x03,

    /* I2C Slave ready to receive data from Master */
    DRV_I2C_SLAVE_READY_TO_TX_TO_MASTER = 0x04,

   /* I2C Stop condition */
   DRV_I2C_MASTER_STOP = 0x05

} DRV_I2C_MODULE_MAIN_STATE;


// *****************************************************************************
/* I2C instance address

  Summary:
    Gets the I2C instance address from its index.

  Description:
    This macro gets the I2C instance address from its index.

  Remarks:
    None.
*/

#define _DRV_I2C_INSTANCE_GET(object)        &gDrvI2CObj[object]


// *****************************************************************************
/* I2C IRQ SELECT

  Summary:
    Asserts the I2C IRQ

  Description:
    This macro redirects to PORTS system service function to select the SD card
    by making the I/O pin low.

  Remarks:
    None.
*/

#define _DRV_I2C_CHIP_SELECT_CLEAR(port,pin)    	SYS_PORTS_PinClear(PORTS_ID_0,port,pin)


// *****************************************************************************
/* I2C chip de-select

  Summary:
    Makes the I2C slave not selected.

  Description:
    This macro redirects to PORTS system service function to de-select the I2C
    by making the I/O pin high.

  Remarks:
    None.
*/

#define _DRV_I2C_CHIP_SELECT_SET(port,pin) 	SYS_PORTS_PinSet(PORTS_ID_0,port,pin)


// *****************************************************************************
// *****************************************************************************
// Section: Data Type Definitions
// *****************************************************************************
// *****************************************************************************

// *****************************************************************************
/* I2C Driver Data Object

  Summary:
    Defines the object required for the maintainence of driver operation queue.

  Description:
    This defines the object required for the maintainence of the software
    clients instance. This object exists once per client instance.

  Remarks:
    None.
*/

typedef struct _DRV_I2C_BUFFER_OBJECT
{

    /* A flag states whether the buffer is in use or not */
    bool                                                    inUse;
    /* I2C Peripheral or not usage */
    DRV_I2C_PERPH                                           i2cPerph;


    /* A reference to the client instance */
    DRV_HANDLE                                              clientHandle;

    /* Slave address high byte, stores 7-bit address 
       and high byte of 10-bit  address */
    uint8_t                                                 slaveaddresshighbyte;
    
    /* Stores low byte of 10 bit address, 
       stores 0 when 7-bit address is used */
    uint8_t                                                 slaveaddresslowbyte;

    /* Specifies the operation */
    DRV_I2C_OPERATIONS                                      operation;

    /* Pointer to transmit buffer */
    uint8_t *                                               txBuffer;

    /* Pointer to receive buffer */
    uint8_t *                                               rxBuffer;

    /* size of buffer for transfer */
    uint32_t                                                transferSize;

    /* size of data to be received */
    uint32_t                                                readtransferSize;

    /*size of buffer actually transferred - used in Slave mode */
    uint32_t                                                actualtransfersize;
    
    /* perform a force write */
    bool                                                    transmitForced;
    
    /* ignore any error events */
    DRV_I2C_BUS_ERROR_EVENT                                 errorEvent;

    /* Transfer status */
    DRV_I2C_BUFFER_EVENT                                    status;

    /* Pointer to the next element in the queue */
    void                                                    *next;

    void                                                    *context;

} DRV_I2C_BUFFER_OBJECT;

// *****************************************************************************
/* I2C Driver Global Instances Object

  Summary:
    Object used to keep track of data that is common to all instances of the 
    I2C driver.

  Description:
    This object is used to keep track of any data that is common to all
    instances of the I2C driver.

  Remarks:
    None.
*/

typedef struct
{
    /* Set to true if all members of this structure
       have been initialized once */
    bool membersAreInitialized;

    /* Mutex to protect client object pool */
    OSAL_MUTEX_DECLARE(mutexClientObjects);

    /* Mutex to protect buffer queue object pool */
    OSAL_MUTEX_DECLARE(mutexBufferQueueObjects);

} DRV_I2C_COMMON_DATA_OBJ;


// *****************************************************************************
/* I2C Driver Hardware Instance Object

  Summary:
    Defines the object required for the maintenance of the hardware instance.

  Description:
    This defines the object required for the maintenance of the hardware
    instance. This object exists once per hardware instance of the peripheral.

  Remarks:
    None.
*/


typedef struct _DRV_I2C_OBJ
{
    /* The status of the driver */
    SYS_STATUS                              status;

    /* The peripheral Id associated with the object */
    I2C_MODULE_ID                           i2cId;

    /* I2C Peripheral or not usage */
    DRV_I2C_PERPH                           i2cPerph;
    
    /* PORT which SCL belongs */
    PORTS_CHANNEL                           portSCL;

    /* Bit position in the port for SCL */
    PORTS_BIT_POS                           pinSCL;
    
    /* PORT which SDA belongs */
    PORTS_CHANNEL                           portSDA;

    /* Bit position in the port for SDA */
    PORTS_BIT_POS                           pinSDA;
    
    /* timer module used when I2C is done in bit-bang mode */
   TMR_MODULE_ID                            tmrSource;
   
   /* timer interrupt source for bit-bang */
   INT_SOURCE                               tmrInterruptSource;

    /* Save the index of the driver. Important to know this
    as we are using reference based accessing */
    SYS_MODULE_INDEX                        drvIndex;

    /* Flag to indicate in use  */
    bool                                    inUse;

    /* Flag to indicate that I2C is used in exclusive access mode */
    bool                                    isExclusive;

    /* Number of clients possible with the hardware instance */
    uint8_t                                 numClients;

    /* Hardware initialization parameters */
    /* I2C Usage Mode Type */
    DRV_I2C_MODE                            i2cMode;

    /* Address Width */
    DRV_I2C_ADDRESS_WIDTH                   addWidth;

    /* Reserved Address rule enable */
    bool                                    reservedaddenable;

    /* I2C Clock mode */
    DRV_I2C_BUS_LEVEL                       buslevel;

    /* Transmit/Receive or Transmit Interrupt
    Source for I2C module */
    INT_SOURCE                              mstrInterruptSource;

    /* Receive Interrupt Source for
     for I2C module */
    INT_SOURCE                              slaveInterruptSource;

    /* Error Interrupt Source for
     I2C module */
    INT_SOURCE                              errInterruptSource;

    /* Status of whether or not the TX interrupt is enabled*/
    bool                                    mstrInterruptEnabled;

    /* Status of whether or not the RX interrupt is enabled*/
    bool                                    slaveInterruptEnabled;

    /* Status of whether or not the ERR interrupt is enabled*/
    bool                                    errInterruptEnabled;

     /*  I2C module main state */
    DRV_I2C_MODULE_MAIN_STATE               modulemainstate;

    /* State of the task */
    DRV_I2C_DATA_OBJECT_TASK                task;

    /* Queue head is specific to the instance */
    DRV_I2C_BUFFER_OBJECT                   *queueHead;
    
        /*Queue tail is specific to the instance */
    DRV_I2C_BUFFER_OBJECT                   *queueTail;      
    
    uint32_t                                 queueIn;
    
    uint32_t                                 queueOut;

    /* Store the last client handle for every task */
    DRV_HANDLE                               lastClientHandle;

    DRV_I2C_BUFFER_OBJECT                    *taskLObj;

    /* Hardware instance mutex */
    OSAL_MUTEX_DECLARE(mutexDriverInstance);

    /* This callback is fired when an operation is about to start on the
       I2C bus.  This allows the user to set any pins that need to be set.
       This callback may be called from an ISR so should not include OSAL
       calls.  The context parameter is the same one passed into the
       BufferAddRead, BufferAddWrite, BufferAddWriteRead function.
     */
    DRV_I2C_CallBack                          operationStarting;
    
    
    
    /* Keeps track if the driver is in interrupt context
       and if so the nesting levels. */
    uint32_t interruptNestingCount;
    
    BUS_STATE  i2cState;
    
    uint16_t  I2CSWCounter;
    uint16_t  I2CSWData;
    bool     I2CNACKOut;     //= false;
    bool     I2CACKStatus;   //= false;

    uint16_t  I2CReadData;

    uint32_t errorTimeOut; // = ERROR_TIMEOUT_COUNT;

    bool _i2c_bit_written;

    bool ACKSTATUS_M;


} DRV_I2C_OBJ;


typedef unsigned int DRV_I2C_CLIENT_OBJ_HANDLE;
typedef unsigned int DRV_I2C_OBJ_HANDLE;


// *****************************************************************************
/* I2C Driver Client Object

  Summary:
    Defines the object required for the maintenance of the software clients.

  Description:
    This defines the object required for the maintenance of the software
    clients instance. This object exists once per client instance.

  Remarks:
    None.
*/

typedef struct _DRV_I2C_CLIENT_OBJ
{
    /* Driver Object associated with the client */
    DRV_I2C_OBJ*                                      	driverObject;

    /* Flag to indicate in use  */
    bool                                                inUse;

    /* The intent with which the client was opened */
    DRV_IO_INTENT                                      	intent;

    /* Chip Select logic level */
    bool                                                irqSelectLogicLevel;

    /* PORT which the chip select pin belongs to */
    PORTS_CHANNEL                                       irqSelectPort;

    /* Bit position in the port */
    PORTS_BIT_POS                                       irqSelectBitPos;

    /* Baud Rate Value */
    uint32_t                                            baudRate;

    /* Save the context, will be passed back with the call back */
    uintptr_t                                           context;

    /* I2C callback */
    DRV_I2C_BUFFER_EVENT_HANDLER                        callback;

} DRV_I2C_CLIENT_OBJ;

// *****************************************************************************
/* I2C Driver Client Setup

  Summary:
    Defines parameters that the client uses when in SLAVE mode

  Description:
    When I2C module is used in SLAVE mode, IRQ flag needs to configured when the
    SLAVE needs to request a read from Master

  Remarks:
    None.
*/


typedef struct _DRV_I2C_CLIENT_SETUP
{

    /* Chip Select logic level */
    bool                            irqSetupLogicLevel;

    /* PORT which the chip select pin belongs to */
    PORTS_CHANNEL 					irqSetupPort;

    /* Bit position in the port */
    PORTS_BIT_POS 					irqSetupBitPos;

} DRV_I2C_CLIENT_SETUP;


// *****************************************************************************
// *****************************************************************************
// Section: Extern data Definitions
// *****************************************************************************
// *****************************************************************************



// *****************************************************************************
/*  Hardware Objects for the static driver
*/

extern DRV_I2C_OBJ            gDrvI2CObj0;
extern DRV_I2C_OBJ            gDrvI2CObj1;
extern DRV_I2C_OBJ            gDrvI2CObj2;
extern DRV_I2C_OBJ            gDrvI2CObj3;
extern DRV_I2C_OBJ            gDrvI2CObj4;
extern DRV_I2C_OBJ            gDrvI2CObj5;
extern DRV_I2C_OBJ            gDrvI2CObj6;


// *****************************************************************************
/*  Client Objects for the single-client driver
*/

extern DRV_I2C_CLIENT_OBJ     gDrvI2CClientObj0;
extern DRV_I2C_CLIENT_OBJ     gDrvI2CClientObj1;
extern DRV_I2C_CLIENT_OBJ     gDrvI2CClientObj2;
extern DRV_I2C_CLIENT_OBJ     gDrvI2CClientObj3;
extern DRV_I2C_CLIENT_OBJ     gDrvI2CClientObj4;
extern DRV_I2C_CLIENT_OBJ     gDrvI2CClientObj5;
extern DRV_I2C_CLIENT_OBJ     gDrvI2CClientObj6;

DRV_I2C_BUFFER_OBJECT* _DRV_I2C_QueueSlotGet ( DRV_I2C_OBJ *dObj );
DRV_I2C_BUFFER_OBJECT* _DRV_I2C_QueuePop ( DRV_I2C_OBJ * object );
void _DRV_I2C_QueuePush(DRV_I2C_OBJ *dObj, DRV_I2C_BUFFER_OBJECT *buf);

/*****************************************************************************
    Function:
    void DRV_I2C_ClientSetup ( DRV_HANDLE handle,
                                 const DRV_I2C_CLIENT_SETUP * const config )

  Summary:
    Sets up the IRQ line for slave with port parameters

  Description:
    This function sets up the idle IRQ logic level, port and pin number for
    which IRQ line is set-up. The IRQ line is used toggled by Slave; the Master
    can sense the toggle and take appropriate action. This is normally used
    when the slave has data to send to the Master and expects the Master to
    issue a read command.

  Parameters:
    handle       - A valid open-instance handle, returned from the driver's
                   open routine

    config       - Port parameters tied to IRQ line

 <code>
    DRV_I2C_CLIENT_SETUP slaveI2CIrq =
    {
        .irqSetupLogicLevel  = true,
        .irqSetupPort        = PORT_CHANNEL_A,
        .irqSetupBitPos      = PORTS_BIT_POS_15
    };
    DRV_I2C_ClientSetup(appData.drvI2CHandle, &slaveI2CIrq);
 </code>
  Returns:
    None
*/
void DRV_I2C_ClientSetup ( DRV_HANDLE handle,const DRV_I2C_CLIENT_SETUP * const config );

#endif //#ifndef _DRV_I2C_LOCAL_H

/*******************************************************************************
 End of File
*/


