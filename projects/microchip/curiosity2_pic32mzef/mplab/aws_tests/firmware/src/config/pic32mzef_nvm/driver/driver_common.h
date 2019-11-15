/*******************************************************************************
  Driver Common Header Definitions

  Company:
    Microchip Technology Inc.

  File Name:
    drv_common.h

  Summary:
    This file defines the common macros and definitions used by the driver
    definition and implementation headers.

  Description:
    This file defines the common macros and definitions used by the driver
    definition and the implementation header.

  Remarks:
    None.
 *******************************************************************************/

//DOM-IGNORE-BEGIN
/*******************************************************************************
* Copyright (C) 2018 Microchip Technology Inc. and its subsidiaries.
*
* Subject to your compliance with these terms, you may use Microchip software
* and any derivatives exclusively with Microchip products. It is your
* responsibility to comply with third party license terms applicable to your
* use of third party software (including open source software) that may
* accompany Microchip software.
*
* THIS SOFTWARE IS SUPPLIED BY MICROCHIP "AS IS". NO WARRANTIES, WHETHER
* EXPRESS, IMPLIED OR STATUTORY, APPLY TO THIS SOFTWARE, INCLUDING ANY IMPLIED
* WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY, AND FITNESS FOR A
* PARTICULAR PURPOSE.
*
* IN NO EVENT WILL MICROCHIP BE LIABLE FOR ANY INDIRECT, SPECIAL, PUNITIVE,
* INCIDENTAL OR CONSEQUENTIAL LOSS, DAMAGE, COST OR EXPENSE OF ANY KIND
* WHATSOEVER RELATED TO THE SOFTWARE, HOWEVER CAUSED, EVEN IF MICROCHIP HAS
* BEEN ADVISED OF THE POSSIBILITY OR THE DAMAGES ARE FORESEEABLE. TO THE
* FULLEST EXTENT ALLOWED BY LAW, MICROCHIP'S TOTAL LIABILITY ON ALL CLAIMS IN
* ANY WAY RELATED TO THIS SOFTWARE WILL NOT EXCEED THE AMOUNT OF FEES, IF ANY,
* THAT YOU HAVE PAID DIRECTLY TO MICROCHIP FOR THIS SOFTWARE.
 *******************************************************************************/
//DOM-IGNORE-END

#ifndef DRIVER_COMMON_H
#define DRIVER_COMMON_H

// *****************************************************************************
// *****************************************************************************
// Section: Included Files
// *****************************************************************************
// *****************************************************************************

#include <stdint.h>

#ifdef __cplusplus
    extern "C" {
#endif

// *****************************************************************************
/* Device Driver I/O Intent

  Summary:
    Identifies the intended usage of the device when it is opened.

  Description:
    This enumeration identifies the intended usage of the device when the
    caller opens the device. It identifies the desired behavior of the device
    driver for the following:

    * Blocking or non-blocking I/O behavior (do I/O calls such as read and write
      block until the operation is finished or do they return immediately and
      require the caller to call another routine to check the status of the
      operation)

    * Support reading and/or writing of data from/to the device

    * Identify the buffering behavior (sometimes called "double buffering" of
      the driver.  Indicates if the driver should maintain its own read/write
      buffers and copy data to/from these buffers to/from the caller's buffers.

    * Identify the DMA behavior of the peripheral

  Remarks:
    The buffer allocation method is not identified by this enumeration.  Buffers
    can be allocated statically at build time, dynamically at run-time, or
    even allocated by the caller and passed to the driver for its own usage if
    a driver-specific routine is provided for such.  This choice is left to
    the design of the individual driver and is considered part of its
    interface.

    These values can be considered "flags".  One selection from each of the
    groups below can be ORed together to create the complete value passed
    to the driver's open routine.
*/

typedef enum
{
    /* Read */
    DRV_IO_INTENT_READ               /*DOM-IGNORE-BEGIN*/ = 1 << 0 /* DOM-IGNORE-END*/,

    /* Write */
    DRV_IO_INTENT_WRITE              /*DOM-IGNORE-BEGIN*/ = 1 << 1 /* DOM-IGNORE-END*/,

    /* Read and Write*/
    DRV_IO_INTENT_READWRITE          /*DOM-IGNORE-BEGIN*/ \
            = DRV_IO_INTENT_READ|DRV_IO_INTENT_WRITE /* DOM-IGNORE-END*/,

    /* The driver will block and will return when the operation is complete */
    DRV_IO_INTENT_BLOCKING           /*DOM-IGNORE-BEGIN*/ = 0 << 2 /* DOM-IGNORE-END*/,

    /* The driver will return immediately */
    DRV_IO_INTENT_NONBLOCKING        /*DOM-IGNORE-BEGIN*/ = 1 << 2 /* DOM-IGNORE-END*/,

    /* The driver will support only one client at a time */
    DRV_IO_INTENT_EXCLUSIVE          /*DOM-IGNORE-BEGIN*/ = 1 << 3 /* DOM-IGNORE-END*/,

    /* The driver will support multiple clients at a time */
    DRV_IO_INTENT_SHARED             /*DOM-IGNORE-BEGIN*/ = 0 << 3 /* DOM-IGNORE-END*/

} DRV_IO_INTENT;


// *****************************************************************************
/* Driver Client Status

  Summary:
    Identifies the current status/state of a client's connection to a driver.

  Description:
    This enumeration identifies the current status/state of a client's link to
    a driver.

  Remarks:
    The enumeration used as the return type for the client-level status routines
    defined by each device driver or system module (for example,
    DRV_USART_ClientStatus) must be based on the values in this enumeration.
*/

typedef enum
{
    /* Indicates that a driver-specific error has occurred. */
    DRV_CLIENT_STATUS_ERROR_EXTENDED   = -10,

    /* An unspecified error has occurred.*/
    DRV_CLIENT_STATUS_ERROR            =  -1,

    /* The driver is closed, no operations for this client are ongoing,
    and/or the given handle is invalid. */
    DRV_CLIENT_STATUS_CLOSED           =   0,

    /* The driver is currently busy and cannot start additional operations. */
    DRV_CLIENT_STATUS_BUSY             =   1,

    /* The module is running and ready for additional operations */
    DRV_CLIENT_STATUS_READY            =   2,

    /* Indicates that the module is in a driver-specific ready/run state. */
    DRV_CLIENT_STATUS_READY_EXTENDED   =  10

} DRV_CLIENT_STATUS;


// *****************************************************************************
/* Device Driver Blocking Status Macro

  Summary:
    Returns if the I/O intent provided is blocking

  Description:
    This macro returns if the I/O intent provided is blocking.

  Remarks:
    None.
*/

#define DRV_IO_ISBLOCKING(intent)          (intent & DRV_IO_INTENT_BLOCKING)


// *****************************************************************************
/* Device Driver Non Blocking Status Macro

  Summary:
    Returns if the I/O intent provided is non-blocking.

  Description:
    This macro returns if the I/ intent provided is non-blocking.

  Remarks:
    None.
*/

#define DRV_IO_ISNONBLOCKING(intent)       (intent & DRV_IO_INTENT_NONBLOCKING )


// *****************************************************************************
/* Device Driver Exclusive Status Macro

  Summary:
    Returns if the I/O intent provided is non-blocking.

  Description:
    This macro returns if the I/O intent provided is non-blocking.

  Remarks:
    None.
*/

#define DRV_IO_ISEXCLUSIVE(intent)       (intent & DRV_IO_INTENT_EXCLUSIVE)


// *****************************************************************************
/* Device Driver IO Buffer Identifier

  Summary:
    Identifies to which buffer a device operation will apply.

  Description:
    This enumeration identifies to which buffer (read, write, both, or neither)
    a device operation will apply.  This is used for "flush" (or similar)
    operations.
*/

typedef enum
{
    // Operation does not apply to any buffer
    DRV_IO_BUFFER_TYPE_NONE      = 0x00,

    // Operation applies to read buffer
    DRV_IO_BUFFER_TYPE_READ      = 0x01,

    // Operation applies to write buffer
    DRV_IO_BUFFER_TYPE_WRITE     = 0x02,

    // Operation applies to both read and write buffers
    DRV_IO_BUFFER_TYPE_RW        = DRV_IO_BUFFER_TYPE_READ|DRV_IO_BUFFER_TYPE_WRITE

} DRV_IO_BUFFER_TYPES;


// *****************************************************************************
/* Device Handle

  Summary:
    Handle to an opened device driver.

  Description:
    This handle identifies the open instance of a device driver.  It must be
    passed to all other driver routines (except the initialization, deinitialization,
    or power routines) to identify the caller.

  Remarks:
    Every application or module that wants to use a driver must first call
    the driver's open routine.  This is the only routine that is absolutely
    required for every driver.

    If a driver is unable to allow an additional module to use it, it must then
    return the special value DRV_HANDLE_INVALID.  Callers should check the
    handle returned for this value to ensure this value was not returned before
    attempting to call any other driver routines using the handle.
*/

typedef uintptr_t DRV_HANDLE;


// *****************************************************************************
/* Invalid Device Handle

 Summary:
    Invalid device handle.

 Description:
    If a driver is unable to allow an additional module to use it, it must then
    return the special value DRV_HANDLE_INVALID.  Callers should check the
    handle returned for this value to ensure this value was not returned before
    attempting to call any other driver routines using the handle.

 Remarks:
    None.
*/

#define DRV_HANDLE_INVALID  (((DRV_HANDLE) -1))


#ifdef __cplusplus
    }
#endif

#endif //DRIVER_COMMON_H

/*******************************************************************************
 End of File
*/
