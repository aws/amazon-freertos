/*******************************************************************************
  System Module Header

  Company:
    Microchip Technology Inc.

  File Name:
    sys_module.h

  Summary:
    Defines definitions and declarations related to system modules.

  Description:
    This file defines definitions and interfaces related to system modules.

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

#ifndef SYSTEM_MODULE_H
#define SYSTEM_MODULE_H


// *****************************************************************************
// *****************************************************************************
// Section: Included Files
// *****************************************************************************
// *****************************************************************************

#include "system_common.h"


#ifdef __cplusplus
    extern "C" {
#endif


// *****************************************************************************
/* System Module Index

  Summary:
    Identifies which instance of a system module should be initialized or opened.

  Description:
    This data type identifies to which instance of a system module a call to
    that module's "Initialize" and "Open" routines refers.

  Remarks:
    Each individual module will usually define macro names for the index values
    it supports (e.g., DRV_TMR_INDEX_1, DRV_TMR_INDEX_2, ...).
*/

typedef unsigned short int SYS_MODULE_INDEX;


// *****************************************************************************
/* System Module Object

  Summary:
    Handle to an instance of a system module.

  Description:
    This data type is a handle to a specific instance of a system module (such
    as a device driver).

  Remarks:
    Code outside of a specific module should consider this as an opaque type
    (much like a void *).  Do not make any assumptions about base type as it
    may change in the future or about the value stored in a variable of this
    type.
*/

typedef uintptr_t SYS_MODULE_OBJ;


// *****************************************************************************
/* System Module Object Invalid

  Summary:
    Object handle value returned if unable to initialize the requested instance
    of a system module.

  Description:
    This is the object handle value returned if unable to initialize the
    requested instance of a system module.

  Remarks:
    Do not rely on the actual value of this constant.  It may change in future
    implementations.
*/

#define SYS_MODULE_OBJ_INVALID      ((SYS_MODULE_OBJ) -1 )


// *****************************************************************************
/* System Module Object Static

  Summary:
    Object handle value returned by static modules.

  Description:
    This is the object handle value returned by static system modules.

  Remarks:
    Do not rely on the actual value of this constant.  It may change in future
    implementations.
*/

#define SYS_MODULE_OBJ_STATIC       ((SYS_MODULE_OBJ) 0 )


// *****************************************************************************
/* System Module Status

  Summary:
    Identifies the current status/state of a system module (including device
    drivers).

  Description:
    This enumeration identifies the current status/state of a system module
    (including device drivers).

  Remarks:
    This enumeration is the return type for the system-level status routine
    defined by each device driver or system module (for example, DRV_I2C_Status).
*/

typedef enum
{
    // Indicates that a non-system defined error has occurred.  The caller
    // must call the extended status routine for the module in question to
    // identify the error.
    SYS_STATUS_ERROR_EXTENDED   = -10,

    /*An unspecified error has occurred.*/
    SYS_STATUS_ERROR            = -1,

    // The module has not yet been initialized
    SYS_STATUS_UNINITIALIZED    = 0,

    // An operation is currently in progress
    SYS_STATUS_BUSY             = 1,

    // Any previous operations have succeeded and the module is ready for
    // additional operations
    SYS_STATUS_READY            = 2,

    // Indicates that the module is in a non-system defined ready/run state.
    // The caller must call the extended status routine for the module in
    // question to identify the state.
    SYS_STATUS_READY_EXTENDED   = 10

} SYS_STATUS;


// *****************************************************************************
/* System Module Init

  Summary:
    Initializes a module (including device drivers) as requested by the system.

  Description:
    This structure provides the necessary data to initialize or reinitialize
    a module (including device drivers).
    The structure can be extended in a module specific way as to carry
    module specific initialization data.

  Remarks:
    This structure is used in the device driver routines DRV_<module>_Initialize
    and DRV_<module>_Reinitialize that are defined by each device driver.

*/

typedef union
{
    uint8_t         value;

    struct
    {
        // Module-definable field, module-specific usage
        uint8_t     reserved    : 4;
    }sys;

} SYS_MODULE_INIT;


// *****************************************************************************
// *****************************************************************************
// Section:  Pointers to System Module Routines
// *****************************************************************************
// *****************************************************************************

// *****************************************************************************
/* System Module Initialization Routine Pointer

  Function:
    SYS_MODULE_OBJ (* SYS_MODULE_INITIALIZE_ROUTINE) (
                        const SYS_MODULE_INDEX index,
                        const SYS_MODULE_INIT * const init )

  Summary:
    Pointer to a routine that initializes a system module (driver, library, or
    system-maintained application).

  Description:
    This data type is a pointer to a routine that initializes a system module
    (driver, library, or system-maintained application).

  Precondition:
    The low-level board initialization must have (and will be) completed before
    the system will call the initialization routine for any modules.

  Parameters:
    index           - Identifier for the module instance to be initialized

    init            - Pointer to the data structure containing any data
                      necessary to initialize the module. This pointer may
                      be null if no data is required and default initialization
                      is to be used.

  Returns:
    A handle to the instance of the system module that was initialized.  This
    handle is a necessary parameter to all of the other system-module level
    routines for that module.

  Remarks:
    This function will only be called once during system initialization.
*/

typedef SYS_MODULE_OBJ (* SYS_MODULE_INITIALIZE_ROUTINE) ( const SYS_MODULE_INDEX index,
                                                           const SYS_MODULE_INIT * const init );



// *****************************************************************************
/* System Module Reinitialization Routine Pointer

  Function:
    void (* SYS_MODULE_REINITIALIZE_ROUTINE) ( SYS_MODULE_OBJ object,
                const SYS_MODULE_INIT * const init)

  Summary:
    Pointer to a routine that reinitializes a system module (driver, library,
    or system-maintained application)

  Description:
    This data type is a pointer to a routine that reinitializes a system
    module (driver, library, or system-maintained application).

  Precondition:
    The low-level board initialization must have (and will be) completed and the
    module's initialization routine will have been called before the system will
    call the reinitialization routine for any modules.

  Parameters:
    object          - Handle to the module instance

    init            - Pointer to the data structure containing any data
                      necessary to initialize the module. This pointer may
                      be null if no data is required and default initialization
                      is to be used.

  Returns:
    None.

  Example:
    None.

  Remarks:
    This operation uses the same initialization data structure as the Initialize
    operation.

    This operation can be used to refresh the hardware state as defined
    by the initialization data, thus it must guarantee that all hardware
    state has been refreshed.

    This function can be called multiple times to reinitialize the module.
*/

typedef void (* SYS_MODULE_REINITIALIZE_ROUTINE) ( SYS_MODULE_OBJ object,
                                                   const SYS_MODULE_INIT * const init );


//*************************************************************************
/*
  Function:
    void (* SYS_MODULE_DEINITIALIZE_ROUTINE) (  SYS_MODULE_OBJ object )

  Summary:
    Pointer to a routine that deinitializes a system module (driver,
    library, or system-maintained application).

  Description:
    System Module Deinitialization Routine Pointer. This data type is a
	pointer to a routine that deinitializes a system module (driver, library,
	or system-maintained application).

  Preconditions:
    The low-level board initialization must have (and will be) completed
    and the module's initialization routine will have been called before
    the system will call the deinitialization routine for any modules.

  Parameters:
    object -  Handle to the module instance

  Returns:
    None.

  Example:
    None.

  Remarks:
    If the module instance has to be used again, the module's "initialize"
    function must first be called.
*/

typedef void (* SYS_MODULE_DEINITIALIZE_ROUTINE) (  SYS_MODULE_OBJ object );


// *****************************************************************************
/* System Module Status Routine Pointer

  Function:
    SYS_STATUS (* SYS_MODULE_STATUS_ROUTINE) (  SYS_MODULE_OBJ object )

  Summary:
    Pointer to a routine that gets the current status of a system module
    (driver, library, or system-maintained application).

  Description:
    This data type is a pointer to a routine that gets the current status of a
    system module (driver, library, or system-maintained application).

  Precondition:
    The low-level board initialization must have (and will be) completed and the
    module's initialization routine will have been called before the system will
    call the status routine for any modules.

  Parameters:
    object          - Handle to the module instance

  Returns:
    One of the possible status codes from SYS_STATUS

  Example:
    None.

  Remarks:
    A module's status operation can be used to determine when any of the
    other module level operations has completed as well as to obtain general
    status of the module.  The value returned by the status routine will be
    checked after calling any of the module operations to find out when they
    have completed.

    If the status operation returns SYS_STATUS_BUSY, the previous operation
    has not yet completed. Once the status operation returns SYS_STATUS_READY,
    any previous operations have completed.

    The value of SYS_STATUS_ERROR is negative (-1). A module may define
    module-specific error values of less or equal SYS_STATUS_ERROR_EXTENDED
    (-10).

    The status function must NEVER block.

    If the status operation returns an error value, the error may be cleared by
    calling the reinitialize operation. If that fails, the deinitialize
    operation will need to be called, followed by the initialize operation to
    return to normal operations.
*/

typedef SYS_STATUS (* SYS_MODULE_STATUS_ROUTINE) (  SYS_MODULE_OBJ object );


// *****************************************************************************
/* System Module Tasks Routine Pointer

  Function:
    void (* SYS_MODULE_TASKS_ROUTINE) ( SYS_MODULE_OBJ object )

  Summary:
    Pointer to a routine that performs the tasks necessary to maintain a state
    machine in a module system module (driver, library, or system-maintained
    application).

  Description:
    This data type is a pointer to a routine that performs the tasks necessary
    to maintain a state machine in a module system module (driver, library, or
    system-maintained application).

  Precondition:
    The low-level board initialization must have (and will be) completed and the
    module's initialization routine will have been called before the system will
    call the deinitialization routine for any modules.

  Parameters:
    object          - Handle to the module instance

  Returns:
    None.

  Example:
    None.

  Remarks:
    If the module is interrupt driven, the system will call this routine from
    an interrupt context.
*/

typedef void (* SYS_MODULE_TASKS_ROUTINE) ( SYS_MODULE_OBJ object );


#ifdef __cplusplus
    }
#endif

#endif // SYSTEM_MODULE_H

/*******************************************************************************
 End of File
*/
