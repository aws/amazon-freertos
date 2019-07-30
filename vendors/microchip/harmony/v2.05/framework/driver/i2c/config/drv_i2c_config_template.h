/*******************************************************************************
  I2C Device Driver Configuration

  Company:
    Microchip Technology Inc.
  
  File Name:
    drv_i2c_config.h

  Summary:
    I2C device driver configuration file.
    
  Description:
  Description:
    These file provides the list of all the configurations that can be used with
    the driver. This file should not be included in the driver.
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

SOFTWARE AND DOCUMENTATION ARE PROVIDED “AS IS” WITHOUT WARRANTY OF ANY KIND,
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

//DOM-IGNORE-BEGIN
#error "This is a configuration template file.  Do not include it directly."
//DOM-IGNORE-END

#ifndef _DRV_I2C_CONFIG_H_
#define _DRV_I2C_CONFIG_H_

// *****************************************************************************
// *****************************************************************************
// Section: I2C Device Driver Dynamic and Static Selection
// *****************************************************************************
// *****************************************************************************

// *****************************************************************************
/* Static Driver Build Configuration

  Summary:
    Static driver build, static device instance parameters.

  Description:
    This value, if used to identify the build type for a driver, will cause the
    driver to be built using a specific statically identified instance of the 
    peripheral.
*/

#define     DRV_STATIC_BUILD        0


// *****************************************************************************
/* Dynamic Driver Build Configuration

  Summary:
    Dynamic driver build, dynamic device instance parameters.

  Description:
    This value, if used to identify the build type for a driver, will cause the
    driver to be built to dynamically, identify the instance of the peripheral at
    run-time using the parameter passed into its API routines.
*/

#define     DRV_DYNAMIC_BUILD       1


// *****************************************************************************
/* I2C Driver Build Configuration Type

  Summary:
    Selects static or dynamic driver build configuration.

  Description:
    This definition selects if I2C device driver is to be used with static or 
    dynamic build parameters.  Must be equated to one of the following values:
    
     - DRV_STATIC_BUILD  - Build the driver using static accesses to the
                            peripheral identified by the DRV_I2C_INSTANCE
                            macro
                            
     - DRV_DYNAMIC_BUILD - Build the driver using dynamic accesses to the
                           peripherals
                           
    Affects all the drv_i2c.h driver functions.
*/

#define DRV_I2C_CONFIG_BUILD_TYPE   DRV_DYNAMIC_BUILD

// *****************************************************************************
/* I2C Static Driver type

  Summary:
    Selects the type of STATIC driver

  Description:
    This selects either the BYTE_MODEL_STATIC or BUFFER_MODEL_STATIC version of
    I2C driver.      
    The BYTE_MODEL_STATIC version is equivalent to and is referred to as STATIC 
    driver implementation in Harmony Versions 1.06.02 and below. This version
    of STATIC driver is not recommended for new design and will be deprecated
    in future release.
    The BUFFER_MODEL_STATIC supports transfer of buffers and is API compatible
    with the DYNAMIC implementation of I2C.

*/

#define I2C_STATIC_DRIVER_MODE      BUFFER_MODEL_STATIC

// *****************************************************************************
/* I2C driver objects configuration

  Summary
    Includes function that writes to slave irrespective of whether receiving
    a ACK or NACK from slave

  Description
    When this option is checked, this will include Forced Write function.
    The Force Write function will send all data  bytes to the slave irrespective    
    of receiving ACK or NACK from slave. If writing data to the slave is 
    invoked using DRV_I2C_Transfer, the transaction will be aborted if the
    Slave NACKs address or any data byte and a STOP condition will be send.
    This function is typically included for Slaves that require a special
    reset sequence.

  Remarks:
    None
*/

#define DRV_I2C_FORCED_WRITE  true

// *****************************************************************************
/* I2C driver objects configuration

  Summary
    Sets up the maximum number of hardware instances that can be supported

  Description
    Sets up the maximum number of hardware instances that can be supported.
    It is recommended that this number be set exactly equal to the number of
    I2C modules that are needed by the application. Hardware Instance
    support consumes RAM memory space. 
    If this macro is not defined, then the driver will be built statically.

  Remarks:
    None
*/

#define DRV_I2C_INSTANCES_NUMBER  5

// *****************************************************************************
/* I2C Interrupt Mode Operation Control

  Summary:
    Macro controls interrupt based operation of the driver

  Description:
    This macro controls the interrupt based operation of the driver. The 
    possible values it can take are
    - true - Enables the interrupt mode
    - false - Enables the polling mode
    If the macro value is true, then Interrupt Service Routine for the interrupt
    should be defined in the application. The DRV_I2C_Tasks() routine should
    be called in the ISR. 

  Remarks:
    None
*/

#define DRV_I2C_INTERRUPT_MODE  true

// *****************************************************************************
/* I2C Driver Instance combined queue depth.

  Summary:
    Number of entries of all queues in all instances of the driver.
  
  Description:
    This macro defines the number of entries of all queues in all instances of
    the driver.
    
    Each hardware instance supports a buffer queue for transmit and receive
    operations. The size of queue is specified either in driver
    initialization (for dynamic build) or by macros (for static build). The
    hardware instance transmit buffer queue will queue transmit buffers
    submitted by the DRV_I2C_Transmit() function.  The hardware instance
    receive buffer queue will queue receive buffers submitted by the
    DRV_I2C_Receive() function. 
    
    A buffer queue will contain buffer queue entries, each related to a
    BufferAdd request.  This configuration macro defines total number of buffer
    entries that will be available for use between all I2C driver hardware
    instances. The buffer queue entries are allocated to individual hardware
    instances as requested by hardware instances. Once the request is processed,
    the buffer queue entry is free for use by other hardware instances.

    The total number of buffer entries in the system determines the ability of
    the driver to service non blocking read and write requests. If a free buffer
    entry is not available, the driver will not add the request and will return
    an invalid buffer handle. More the number of buffer entries, greater the
    ability of the driver to service and add requests to its queue. A hardware
    instance additionally can queue up as many buffer entries as  specified by
    its transmit and receive buffer queue size. 

    As an example, consider the case of static single client driver application
    where full duplex non blocking operation is desired without queuing, the
    minimum transmit queue depth and minimum receive queue depth should be 1.
    Hence the total number of buffer entries should be 2.
	
	In the current implementation of I2C driver, queueing of Buffers is not supported.
	This will be added in a future release.
   
  Remarks:
    None
*/

#define  DRV_I2C_QUEUE_DEPTH_COMBINED   7


// *****************************************************************************
/* I2C Driver Buffer Queue Support 

  Summary:
    Specifies if the Buffer Queue support should be enabled. 
  
  Description:
    This macro defines if Buffer Queue support should be enabled. Setting this
    macro to true will enable buffer queue support and all buffer related driver
    function. 

  Remarks:
    None
*/

#define DRV_I2C_BUFFER_QUEUE_SUPPORT  false


// ****************************************************************************
// ****************************************************************************
// Section: I2C Device Driver Operation Mode Support Selection
// ****************************************************************************
// ****************************************************************************
/* The following labels can be defined to support specific features or modes
   of operation of the I2C device driver.
*/

// *****************************************************************************
/* Support Master Mode

  Summary:
    Enables the device driver to support operation in Master mode.

  Description:
    This definition enables the device driver to support operation in Master 
    mode.

  Remarks:
    During the configuration phase, the driver selects a list of operation modes
    that can be supported.  While initializing a hardware instance, the device 
    driver will properly perform the initialization base on the selected modes.
    
    The device driver can support multiple modes within a single build.
     
     
     Refer to the description of each function in the corresponding help file 
     for details.
*/

#define DRV_I2C_CONFIG_SUPPORT_OPERATION_MODE_MASTER


// *****************************************************************************
/* Support Slave Mode

  Summary:
    Enables the device driver to support operation in Slave mode.

  Description:
    This definition enables the device driver to support operation in Slave
    mode.

  Remarks:
    During the configuration phase, the driver selects a list of operation modes
    that can be supported.  While initializing a hardware instance, the device 
    driver will properly perform the initialization base on the selected modes.
    
    The device driver can support multiple modes within a single build.
        
     Refer to the description of each function in the corresponding help file
     for details.
*/

#define DRV_I2C_CONFIG_SUPPORT_OPERATION_MODE_SLAVE


// *****************************************************************************
/* Support Read Mode

  Summary:
    Enables the device driver to support read operations.

  Description:
    This definition enables the device driver to support read operations.

  Remarks:
    The device driver can support multiple modes within a single build.
     
    This definition affects the following functions:
     - DRV_I2C_Receive
     
     Refer to the description of each function in the corresponding help file
     for details.
*/

#define DRV_I2C_CONFIG_SUPPORT_OPERATION_MODE_READ


// *****************************************************************************
/* Support Write Mode

  Summary:
    Enables the device driver to support write operations.

  Description:
    This definition enables the device driver to support write operations.

  Remarks:
    The device driver can support multiple modes within a single build.
     
    This definition affects the following functions:
     - DRV_I2C_Transmit
     Refer to the description of each function in the corresponding help file
     for details.
*/

#define DRV_I2C_CONFIG_SUPPORT_OPERATION_MODE_WRITE

// *****************************************************************************
/* Support Write followed by a Read using Restart

  Summary:
    Enables the device driver to support write followed by read. 

  Description:
    This definition enables the device driver to support write followed by read
	without relinquishing control of the bus. Restart is issued instead of Stop 
	at the end of write. Stop is issued after read operation.

  Remarks:
    The device driver can support multiple modes within a single build.
     
    This definition affects the following functions:
     - DRV_I2C_TransmitThenReceive
     Refer to the description of each function in the corresponding help file
     for details.
*/

#define DRV_I2C_CONFIG_SUPPORT_OPERATION_MODE_WRITE_READ

// *****************************************************************************
/* Support Blocking Operations

  Summary:
    Enables the device driver to support blocking operations.

  Description:
    This definition enables the device driver to support blocking operations.

  Remarks:
    The device driver can support multiple modes within a single build.
     
    This definition affects the following functions:
     - DRV_I2C_Open
     - DRV_I2C_Close
     - DRV_I2C_Receive
     - DRV_I2C_Transmit
	 - DRV_I2C_TransmitThenReceive
     
     Refer to the description of each function in the corresponding help file
     for details.
     
*/

#define DRV_I2C_CONFIG_SUPPORT_OPERATION_MODE_BLOCKING


// *****************************************************************************
/* Support Non-Blocking Operations

  Summary:
    Enables the device driver to support non-blocking during operations

  Description:
    This definition enables the device driver to support non-blocking 
    operations.

  Remarks:
    The device driver can support multiple modes within a single build.
     
    This definition affects the following functions:
     - DRV_I2C_Open
     - DRV_I2C_Close
     
     Refer to the description of each function in the corresponding help file
     for details.
     
*/

#define DRV_I2C_CONFIG_SUPPORT_OPERATION_MODE_NON_BLOCKING


// *****************************************************************************
/* Support Exclusive Mode

  Summary:
    Enables the device driver to support operation in Exclusive mode.

  Description:
    This definition enables the device driver to support operation in Exclusive
    mode.

  Remarks:
    The device driver can support multiple modes within a single build.
     
    This definition affects the following functions:
     - DRV_I2C_Open
     
     Refer to the description of each function in the corresponding help file 
     for details.
*/

#define DRV_I2C_CONFIG_SUPPORT_OPERATION_MODE_EXCLUSIVE


// *****************************************************************************
/* Support Basic Transfer Mode

  Summary:
    Enables the device driver to support basic transfer mode.

  Description:
    This definition enables the device driver to support basic transfer mode.

  Remarks:
    The device driver can support multiple modes within a single build.
     
    This definition affects the following functions:
     - DRV_I2C_TransmitThenReceive     
     Refer to the description of each function in the corresponding help file
     for details.
*/

#define DRV_I2C_CONFIG_SUPPORT_OPERATION_MODE_BASIC

//DOM-IGNORE-BEGIN
// *****************************************************************************
/* Support Segment Transfer Mode

  Summary:
    Enables the device driver to support segment transfer mode.

  Description:
    This definition enables the device driver to segment basic transfer mode.

  Remarks:
    The device driver can support multiple modes within a single build.
     
    This definition affects the following functions:
     - DRV_I2C_TransferAdd
     - DRV_I2C_TransferRemove
     - DRV_I2C_TransferStart
     - DRV_I2C_TransferAck
     - DRV_I2C_TransferStatus
     - DRV_I2C_TransferStatusDcpt
     - DRV_I2C_TransferGetCompletedNumber
     - DRV_I2C_TransferGetPendingNumber
     - DRV_I2C_TransferGetComplete
     - DRV_I2C_TransferRegisterCallback 
     
     Refer to the description of each function in the corresponding help file
     for details.
*/

#define DRV_I2C_CONFIG_SUPPORT_OPERATION_MODE_SEGMENT
//DOM-IGNORE-END

// ****************************************************************************
// ****************************************************************************
// Section: I2C Device Driver Client Support Selection
// ****************************************************************************
// ****************************************************************************

// *****************************************************************************
/* Maximum Number of Clients

  Summary:
    Selects the maximum number of clients.

  Description:
    This definition selects the maximum number of clients that the I2C driver can
    support at run time.
    
  Remarks:
    This definition affects the following function:
     - DRV_I2C_Open
     
     Refer to the description of each function in the corresponding help file
     for details.
*/

DRV_I2C_CLIENTS_NUMBER							5

// ****************************************************************************
// ****************************************************************************
// Section: I2C Device Driver I/O Scheduling Support Selection
// ****************************************************************************
// ****************************************************************************

// *****************************************************************************
/* Maximum Number of Master Queue Slots

  Summary:
    Selects the maximum number of master queue slots.

  Description:
    This definition selects the maximum number of master transfer segments that
    can be simultaneously queued.
    
  Remarks:
    This definition affects the following functions:
     - DRV_I2C_Receive
     - DRV_I2C_Transmit
     Refer to the description of each function in the corresponding help file 
     for details.
*/

#define DRV_I2C_CONFIG_MAX_MASTER_QUEUE_SLOTS       10

        
// *****************************************************************************
/* Maximum Number of Slave Queue Slots

  Summary:
    Selects the maximum number of slave queue slots.

  Description:
    This definition selects the maximum number of slave transfer segments that
    can be simultaneously queued.
    
  Remarks:
    This definition affects the following functions:
     - DRV_I2C_Receive
     - DRV_I2C_Transmit
	
     Refer to the description of each function in the corresponding help file
     for details.
*/

#define DRV_I2C_CONFIG_MAX_SLAVE_QUEUE_SLOTS        10
        

// *****************************************************************************
/* Support Callbacks

  Summary:
    Selects if callbacks are to be supported by the driver

  Description:
    This definition selects if callbacks are to be supported by the driver.

    Refer to the description of each function in the corresponding help file
    for details.
*/

#define DRV_I2C_CONFIG_SUPPORT_CALLBACKS


#endif // _DRV_I2C_CONFIG_H_

/*******************************************************************************
 End of File
*/

