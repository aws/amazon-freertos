/*******************************************************************************
  Console System Service Interface Definition

  Company:
    Microchip Technology Inc.

  File Name:
    sys_console.h

  Summary:
    Console System Service interface definitions.

  Description:
    This file contains the interface definition for the Console system
    service.  It provides a way to interact with the Console subsystem to
    manage the timing requests supported by the system.
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

#ifndef SYS_CONSOLE_H
#define SYS_CONSOLE_H


// *****************************************************************************
// *****************************************************************************
// Section: Included Files
// *****************************************************************************
// *****************************************************************************
#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>

#include "configuration.h"
#include "system/system.h"
#include "driver/driver.h"
#include "system/console/src/sys_console_local.h"

// DOM-IGNORE-BEGIN
#ifdef __cplusplus

    extern "C" {

#endif
// DOM-IGNORE-END

// *****************************************************************************
// *****************************************************************************
// Section: SYS CONSOLE Data Types
// *****************************************************************************
// *****************************************************************************

/* SYS Console Module Index Numbers

  Summary:
    Console System Service index definitions.

  Description:
    These constants provide Console System Service index definitions.

  Remarks:
    These constants should be used in place of hard-coded numeric literals.

*/

#define SYS_CONSOLE_INDEX_0                 0
#define SYS_CONSOLE_INDEX_1                 1
#define SYS_CONSOLE_INDEX_2                 2
#define SYS_CONSOLE_INDEX_3                 3

/* These are in unistd.h */
#define STDIN_FILENO     0
#define STDOUT_FILENO    1
#define STDERR_FILENO    2

#ifndef SYS_CONSOLE_MESSAGE
    #define SYS_CONSOLE_MESSAGE(message)
#endif

#ifndef SYS_CONSOLE_PRINT
    #define SYS_CONSOLE_PRINT(fmt, ...)
#endif

// *****************************************************************************
/* System Console Event

  Summary:
    Identifies the system console event for which the callback is being registered.

  Description:
    This enum is used to identify if the callback being registered is to be called
    on a read or on a write complete event.
*/
typedef enum
{
    // System console write complete event
    SYS_CONSOLE_EVENT_WRITE_COMPLETE,

    // System console read complete event
    SYS_CONSOLE_EVENT_READ_COMPLETE,

} SYS_CONSOLE_EVENT;

// *****************************************************************************
/* Function:
    void ( * SYS_CONSOLE_CALLBACK ) ( void* pBuffer )

   Summary:
    Pointer to a console system service callback function.

   Description:
    This data type defines a pointer to a console service callback function, thus
    defining the function signature.

   Precondition:
    The console service must have been initialized using the SYS_CONSOLE_Initialize
    function before attempting to register a SYS Console callback function.

   Parameters:
    pBuffer    - Pointer to the processed read/write buffer.
                 It can be used identify the buffer that is processed by the console
                 system service and free the buffer memory if it was allocated 
                 dynamically.

   Returns:
    None.

  Example:
    <code>
    void MyCallback ( void* pBuffer )
    {
        if (pBuffer != NULL)
        {
            //Free the memory pointed by pBuffer if it was allocated dynamically.
        }
    }
    </code>

  Remarks:
    None.
*/

typedef void (*SYS_CONSOLE_CALLBACK) (void* pBuffer);

// DOM-IGNORE-BEGIN

typedef enum
{
    SYS_CONSOLE_STATUS_NOT_CONFIGURED,

    SYS_CONSOLE_STATUS_CONFIGURED,

    SYS_CONSOLE_STATUS_BUSY,

    SYS_CONSOLE_STATUS_ERROR

} SYS_CONSOLE_STATUS;

// *****************************************************************************
/*  Console device enumeration

  Summary:
    Lists the available console devices.

  Description:
    This enumeration lists all of the available console devices. A console
    device is a physical peripheral used by the console service to send
    and receive data.

  Remarks:
    None.
*/
typedef enum
{
    SYS_CONSOLE_DEV_USART,

    SYS_CONSOLE_DEV_USB_CDC,

    SYS_CONSOLE_DEV_APPIO,

    SYS_CONSOLE_NR_DEVS,

} SYS_CONSOLE_DEVICE;

// *****************************************************************************
/*  Console device descriptor function prototypes

  Summary:
    Function prototype for the device descriptor expected by the system console.

  Description:
    Defines the function prototypes for the device descriptor that each device
    must implement in order to plug in to the system console service.

  Remarks:
    None.
*/

typedef void (*SYS_CONSOLE_INIT_FPTR) (uint32_t index, const void* initData);

typedef ssize_t (*SYS_CONSOLE_READ_FPTR) (uint32_t index, int fd, void* buf, size_t count);

typedef ssize_t (*SYS_CONSOLE_WRITE_FPTR) (uint32_t index, int fd, const void* buf, size_t count);

typedef void (*SYS_CONSOLE_CALLBACK_REG_FPTR) (uint32_t index, SYS_CONSOLE_CALLBACK cbFunc, SYS_CONSOLE_EVENT event);

typedef void (*SYS_CONSOLE_TASK_FPTR) (uint32_t index, SYS_MODULE_OBJ object);

typedef SYS_CONSOLE_STATUS (*SYS_CONSOLE_STATUS_FPTR) (uint32_t index);

typedef void (*SYS_CONSOLE_FLUSH_FPTR) (uint32_t index);

// *****************************************************************************
/*  Console device descriptor

  Summary:
    The console device must provide the implementation for the APIs described in
    the console device descriptor structure.

  Description:
    Each device must implement register its capability to the console system
    service. The capability APIs must confirm to the interface expected by the
    console system service.

  Remarks:
    None.
*/
typedef struct
{
    SYS_CONSOLE_DEVICE consoleDevice;

    DRV_IO_INTENT intent;

    SYS_CONSOLE_INIT_FPTR init;

    SYS_CONSOLE_READ_FPTR read;

    SYS_CONSOLE_WRITE_FPTR write;

    SYS_CONSOLE_CALLBACK_REG_FPTR callbackRegister;

    SYS_CONSOLE_TASK_FPTR task;

    SYS_CONSOLE_STATUS_FPTR status;

    SYS_CONSOLE_FLUSH_FPTR flush;

} SYS_CONSOLE_DEV_DESC;

// *****************************************************************************
/* SYS CONSOLE OBJECT INSTANCE structure

  Summary
    System Console object instance structure.

  Description:
    This data type defines the System Console object instance.

  Remarks:
    None.
*/

typedef struct
{
    /* State of this instance */
    SYS_STATUS status;

    const SYS_CONSOLE_DEV_DESC* devDesc;

    CONSOLE_DEVICE_INDEX devIndex;

} SYS_CONSOLE_OBJECT_INSTANCE;


// *****************************************************************************
//
/* SYS Console Initialize structure

  Summary:
    Identifies the system console initialize structure.

  Description:
    This structure identifies the system console initialize structure.

  Remarks:
    None.
*/

typedef struct
{
    /* Initialization data for the underlying device */
    const void* deviceInitData;

    const SYS_CONSOLE_DEV_DESC* consDevDesc;

    uint32_t deviceIndex;

} SYS_CONSOLE_INIT;

// DOM-IGNORE-END

// *****************************************************************************
// *****************************************************************************
// Section: SYS CONSOLE CONTROL Routines
// *****************************************************************************
// *****************************************************************************

// *****************************************************************************
/* Function:
    SYS_MODULE_OBJ SYS_CONSOLE_Initialize(
        const SYS_MODULE_INDEX index,
        const SYS_MODULE_INIT* const init
    )

  Summary:
    Initializes the console instance module and opens or initializes the
    specific module instance to which it is associated.

  Description:
    This function initializes the internal data structures used by the console
    module. It also initializes the associated I/O driver/PLIB.

  Precondition:
    None.

  Parameters:
    index           - Index for the instance to be initialized

    init            - Pointer to a data structure containing any data necessary
                      to initialize the Console System service. This pointer
                      may be null if no data is required because static
                      overrides have been provided.

  Returns:
    If successful, returns a valid handle to the console instance. Otherwise, it
    returns SYS_MODULE_OBJ_INVALID. The returned object must be passed as
    argument to SYS_CONSOLE_Tasks and SYS_CONSOLE_Status routines.

  Example:
    <code>
    SYS_MODULE_OBJ  objectHandle;

    // Populate the console initialization structure
    const SYS_CONSOLE_INIT sysConsole0Init =
    {
        .deviceInitData = (void*)&sysConsole0UARTInitData,
        .consDevDesc = &sysConsoleUARTDevDesc,
        .deviceIndex = 0,
    };

    objectHandle = SYS_CONSOLE_Initialize(SYS_CONSOLE_INDEX_0, (SYS_MODULE_INIT *)&sysConsole0Init);
    if (objectHandle == SYS_MODULE_OBJ_INVALID)
    {
        // Handle error
    }
    </code>

  Remarks:
    This routine should only be called once during system initialization.
*/

SYS_MODULE_OBJ SYS_CONSOLE_Initialize(
    const SYS_MODULE_INDEX index,
    const SYS_MODULE_INIT* const init
);

// *****************************************************************************
/* Function:
    void SYS_CONSOLE_Tasks ( SYS_MODULE_OBJ object )

  Summary:
    Maintains the console's state machine.

  Description:
    This function runs the console system service's internal state machine.

  Precondition:
    The SYS_CONSOLE_Initialize function must have been called for the specified
    CONSOLE driver instance.

  Parameters:
    object - SYS CONSOLE object handle, returned from SYS_CONSOLE_Initialize

  Returns:
    None

  Example:
    <code>
    SYS_MODULE_OBJ object;     // Returned from SYS_CONSOLE_Initialize

    while (true)
    {
        SYS_CONSOLE_Tasks (object);

        // Do other tasks
    }
    </code>

  Remarks:
    This function is normally not called directly by an application.  It is
    called by the system's Tasks routine (SYS_Tasks) or by the appropriate raw
    ISR.
*/

void SYS_CONSOLE_Tasks ( SYS_MODULE_OBJ object );

// *****************************************************************************
/* Function:
    SYS_STATUS SYS_CONSOLE_Status ( SYS_MODULE_OBJ object )

  Summary:
    Returns status of the specific instance of the Console module.

  Description:
    This function returns the status of the specific module instance.

  Precondition:
    The SYS_CONSOLE_Initialize function should have been called before calling
    this function.

  Parameters:
    object    - SYS CONSOLE object handle, returned from SYS_CONSOLE_Initialize

  Returns:
    * SYS_STATUS_READY          - Indicates that the driver is initialized and is
                                  ready to accept new requests from the clients.

    * SYS_STATUS_BUSY           - Indicates that the driver is busy with a
                                  previous requests from the clients. However,
                                  depending on the configured queue size for
                                  transmit and receive, it may be able to queue
                                  a new request.

    * SYS_STATUS_ERROR          - Indicates that the driver is in an error state.
                                  Any value less than SYS_STATUS_ERROR is
                                  also an error state.

    * SYS_STATUS_UNINITIALIZED  - Indicates that the driver is not initialized.

  Example:
    <code>
    // Given "object" returned from SYS_CONSOLE_Initialize

    SYS_STATUS          consStatus;

    consStatus = SYS_CONSOLE_Status (object);
    if (consStatus == SYS_STATUS_READY)
    {
        // Console is initialized and is ready to accept client requests.
    }
    </code>

  Remarks:
    None.
*/

SYS_STATUS SYS_CONSOLE_Status( SYS_MODULE_OBJ object );

// *****************************************************************************
/* Function:
    ssize_t SYS_CONSOLE_Read(
        const SYS_MODULE_INDEX index,
        int fd,
        void* buf,
        size_t count
    )

  Summary:
    Reads data from the console device.

  Description:
    This function reads the data from the console device.

  Preconditions:
    The SYS_CONSOLE_Initialize function should have been called before calling
    this function.

  Parameters:
    index           - Console instance index
    fd              - I/O stream handle.
                      Maintained for backward compatibility.
                      NULL value can be passed as a parameter.
    buf             - Buffer to hold the read data.
    count           - Number of bytes to read.

  Returns:
    The requested number of bytes to read is returned back if the request is
    accepted successfully. In case of error, the returned value is less than the
    requested number of bytes to read.

  Example:
    <code>
    ssize_t nr;
    char myBuffer[MY_BUFFER_SIZE];
    nr = SYS_CONSOLE_Read( SYS_CONSOLE_INDEX_0, 0, myBuffer, MY_BUFFER_SIZE );
    if (nr != MY_BUFFER_SIZE)
    {
        // handle error
    }
    </code>

  Remarks:
    None.
*/

ssize_t SYS_CONSOLE_Read( const SYS_MODULE_INDEX index, int fd, void* buf, size_t count );

// *****************************************************************************
/* Function:
    ssize_t SYS_CONSOLE_Write(
        const SYS_MODULE_INDEX index,
        int fd,
        const void* buf,
        size_t count
    )

  Summary:
    Writes data to the console device.

  Description:
    This function writes data to the console device.

  Preconditions:
    The SYS_CONSOLE_Initialize function should have been called before calling
    this function.

  Parameters:
    index           - Console instance index
    fd              - I/O stream handle.
                      Maintained for backward compatibility.
                      NULL value can be passed as a parameter.
    buf             - Buffer holding the data to be written.
    count           - Number of bytes to write.

  Returns:
    The requested number of bytes to write is returned back if the request is
    accepted successfully. In case of error, the returned value is less than the
    requested number of bytes to write.

  Example:
    <code>
    ssize_t nr;
    char myBuffer[] = "message";
    nr = SYS_CONSOLE_Write( SYS_CONSOLE_INDEX_0, 0, myBuffer, strlen(myBuffer) );
    if (nr != strlen(myBuffer))
    {
        // Handle error
    }
    </code>

  Remarks:
    None.
*/

ssize_t SYS_CONSOLE_Write( const SYS_MODULE_INDEX index, int fd, const void* buf, size_t count );

// *****************************************************************************
/* Function:
    void SYS_CONSOLE_RegisterCallback(
        const SYS_MODULE_INDEX index,
        SYS_CONSOLE_CALLBACK cbFunc,
        SYS_CONSOLE_EVENT event
    )

  Summary:
    Registers a callback function with the console service that will be
    executed when the read or write request is complete (or in case of an error
    condition during read/write).

  Description:
    This function is used by an application to register a callback function
    with the console service. The callback function is called in response to
    a read/write completion (or error) event. Separate callback functions are
    required for each event. To receive events, the callback must be registered
    before submitting a read/write request.

  Preconditions:
    The SYS_CONSOLE_Initialize function should have been called before calling
    this function.

  Parameters:
    index               - Console instance index
    cbFunc              - The name of the callback function
    event               - Enumerated list of events that can trigger a callback

  Returns:
    None.

  Example:
    <code>
    //Registering a "APP_ReadComplete" read callback function
    SYS_CONSOLE_RegisterCallback(SYS_CONSOLE_INDEX_0, APP_ReadComplete, \
                                 SYS_CONSOLE_EVENT_READ_COMPLETE);

    //Registering a "APP_WriteComplete" write callback function
    SYS_CONSOLE_RegisterCallback(SYS_CONSOLE_INDEX_0, APP_WriteComplete, \
                                 SYS_CONSOLE_EVENT_WRITE_COMPLETE);
    </code>

  Remarks:
    None.
*/

void SYS_CONSOLE_RegisterCallback(
    const SYS_MODULE_INDEX index,
    SYS_CONSOLE_CALLBACK cbFunc,
    SYS_CONSOLE_EVENT event
);

// *****************************************************************************
/* Function:
    void SYS_CONSOLE_Flush(const SYS_MODULE_INDEX index)

  Summary:
    Flushes the read and write queues for the given console instance.

  Description:
    This function flushes queued read and write requests. The request that is
    already in progress cannot be flushed. Only the queued pending requests will
    be flushed.

  Preconditions:
    The SYS_CONSOLE_Initialize function should have been called before calling
    this function.

  Parameters:
    index               - Console instance index

  Returns:
    None.

  Example:
    <code>
    SYS_CONSOLE_Flush(SYS_CONSOLE_INDEX_0);
    </code>

  Remarks:
    None.
*/

void SYS_CONSOLE_Flush(const SYS_MODULE_INDEX index);

// DOM-IGNORE-BEGIN
#ifdef __cplusplus
}
#endif
// DOM-IGNORE-END

#endif //SYS_CONSOLE_H

/*******************************************************************************
 End of File
*/
