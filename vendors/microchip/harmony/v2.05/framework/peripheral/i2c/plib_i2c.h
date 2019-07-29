/*******************************************************************************
  I2C Peripheral Library Interface Header

  Company:      Microchip Technology Inc.

  File Name:    plib_i2c.h

  Summary:
    This file contains the interface definition for the I2C Peripheral Library.

  Description:
	This library provides a low-level abstraction of the Inter-Integrated
	Circuit (I2C) module on Microchip microcontrollers with a convenient C language
	interface. It can be used to simplify low-level access to the module
	without the necessity of interacting directly with the module's registers,
	thus hiding differences between one microcontroller variant and another.
*******************************************************************************/

// DOM-IGNORE-BEGIN
/*******************************************************************************
Copyright 2013-2015 released Microchip Technology Inc.  All rights reserved.

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
// DOM-IGNORE-END
#ifndef _PLIB_I2C_H
#define _PLIB_I2C_H

// *****************************************************************************
// *****************************************************************************
// Section: Included Files (continued at end of file)
// *****************************************************************************
// *****************************************************************************
/*  This section lists the other files that are included in this file.  However,
    please see the bottom of the file for additional implementation header files
    that are also included
*/

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

// DOM-IGNORE-BEGIN
#ifdef __cplusplus  // Provide C++ Compatibility

    extern "C" {

#endif
// DOM-IGNORE-END

// *****************************************************************************
// *****************************************************************************
// Section: Included Files (continued at end of file)
// *****************************************************************************
// *****************************************************************************
/*  This section lists the other files that are included in this file.  However,
    please see the bottom of the file for additional implementation header files
    that are also included
*/
#include "peripheral/i2c/plib_i2c_helper_macros.h"
#include "peripheral/i2c/processor/i2c_processor.h"


// *****************************************************************************
// *****************************************************************************
// Section:  General Configuration
// *****************************************************************************
// *****************************************************************************

//******************************************************************************
/* Function:
    void PLIB_I2C_SlaveClockStretchingEnable ( I2C_MODULE_ID index )

  Summary:
    Enables the I2C module to stretch the slave clock.

  Description:
    This function enables the I2C module to stretch the slave clock to allow time
    for software to respond to bytes received from the master.

  Precondition:
    None.

  Parameters:
    index      - Identifies the desired I2C module

  Returns:
    None.

  Example:
    <code>
    #define MY_I2C_ID I2C_ID_1

    PLIB_I2C_SlaveClockStretchingEnable ( MY_I2C_ID );
    </code>

  Remarks:
    The clock stretching feature can be disabled by calling the
	PLIB_I2C_SlaveClockStretchingDisable function.

    The MY_I2C_ID macro in the example above is used as a placeholder for the
    actual I2C bus ID (as defined by the processor-specific I2C_MODULE_ID enum).

	This feature may not be available on all devices. Please refer to the specific
	device data sheet to determine availability or use PLIB_I2C_ExistsClockStretching
	in your application to determine whether this feature is available.
*/

void PLIB_I2C_SlaveClockStretchingEnable ( I2C_MODULE_ID index );


//******************************************************************************
/* Function:
    void PLIB_I2C_SlaveClockStretchingDisable ( I2C_MODULE_ID index )

  Summary:
    Disables the I2C module from stretching the slave clock.

  Description:
    This function disables the I2C module from stretching the slave clock to
    allow time for software to respond to bytes received from the master.

  Precondition:
    None.

  Parameters:
    index      - Identifies the desired I2C module

  Returns:
    None.

  Example:
    <code>
    #define MY_I2C_ID I2C_ID_1

    PLIB_I2C_SlaveClockStretchingDisable ( MY_I2C_ID );
    </code>

  Remarks:
    The clock stretching feature can be re-enabled by calling the
    PLIB_I2C_SlaveClockStretchingEnable function.

    The MY_I2C_ID macro in the example above is used as a placeholder for the
    actual I2C bus ID (as defined by the processor-specific I2C_MODULE_ID enum).

	This feature may not be available on all devices. Please refer to the specific
	device data sheet to determine availability or use PLIB_I2C_ExistsClockStretching
	in your application to determine whether this feature is available.
*/

void PLIB_I2C_SlaveClockStretchingDisable ( I2C_MODULE_ID index );


//******************************************************************************
/* Function:
    void PLIB_I2C_GeneralCallEnable ( I2C_MODULE_ID index )

  Summary:
    Enables the I2C module to recognize the general call address.

  Description:
    This function enables the I2C module to recognize the general call address
    when operating as a slave receiver.

  Precondition:
    None.

  Parameters:
    index      - Identifies the desired I2C module

  Returns:
    None.

  Example:
    <code>
    #define MY_I2C_ID I2C_ID_1

    PLIB_I2C_GeneralCallEnable ( MY_I2C_ID );
    </code>

  Remarks:
    The General-call feature can be disabled by calling the
    PLIB_I2C_GeneralCallDisable function.

    The MY_I2C_ID macro in the example above is used as a placeholder for the
    actual I2C bus ID (as defined by the processor-specific I2C_MODULE_ID enum).

	This feature may not be available on all devices. Please refer to the specific
	device data sheet to determine availability or use PLIB_I2C_ExistsGeneralCall
	in your application to determine whether this feature is available.
*/

void PLIB_I2C_GeneralCallEnable ( I2C_MODULE_ID index );


//******************************************************************************
/* Function:
    void PLIB_I2C_GeneralCallDisable ( I2C_MODULE_ID index )

  Summary:
    Disables the I2C module from recognizing the general call address.

  Description:
    This function disables the I2C module from recognizing the general call
    address when operating as a slave receiver.

  Precondition:
    None.

  Parameters:
    index      - Identifies the desired I2C module

  Returns:
    None.

  Example:
    <code>
    #define MY_I2C_ID I2C_ID_1

    PLIB_I2C_GeneralCallDisable(MY_I2C_ID);
    </code>

  Remarks:
    The General-call feature can be re-enabled by calling the
    PLIB_I2C_GeneralCallEnable function.

    The MY_I2C_ID macro in the example above is used as a placeholder for the
    actual I2C bus ID (as defined by the processor-specific I2C_MODULE_ID enum).

	This feature may not be available on all devices. Please refer to the specific
	device data sheet to determine availability or use PLIB_I2C_ExistsGeneralCall
	in your application to determine whether this feature is available.
*/

void PLIB_I2C_GeneralCallDisable ( I2C_MODULE_ID index );


//******************************************************************************
/* Function:
    void PLIB_I2C_SMBEnable ( I2C_MODULE_ID index )

  Summary:
    Enables the I2C module to support System Management Bus (SMBus) electrical
    signaling levels.

  Description:
    This function enables the I2C module to support SMBus electrical signaling
    levels.

  Precondition:
    None.

  Parameters:
    index      - Identifies the desired I2C module

  Returns:
    None.

  Example:
    <code>
    #define MY_I2C_ID I2C_ID_1

    PLIB_I2C_SMBEnable ( MY_I2C_ID );
    </code>

  Remarks:
    The SMB feature can be disabled by calling the PLIB_I2C_SMBDisable function.

    The MY_I2C_ID macro in the example above is used as a placeholder for the
    actual I2C bus ID (as defined by the processor-specific I2C_MODULE_ID enum).

	This feature may not be available on all devices. Please refer to the specific
	device data sheet to determine availability or use PLIB_I2C_ExistsSMBus
	in your application to determine whether this feature is available.
*/

void PLIB_I2C_SMBEnable ( I2C_MODULE_ID index );


//******************************************************************************
/* Function:
    void PLIB_I2C_SMBDisable ( I2C_MODULE_ID index )

  Summary:
    Disable the I2C module support for SMBus electrical signaling levels.

  Description:
    This function disables the I2C module support for SMBus electrical signaling levels.

  Precondition:
    None.

  Parameters:
    index      - Identifies the desired I2C module

  Returns:
    None.

  Example:
    <code>
    #define MY_I2C_ID I2C_ID_1

    PLIB_I2C_SMBDisable ( MY_I2C_ID );
    </code>

  Remarks:
    The SMB feature can be re-enabled by calling the PLIB_I2C_SMBEnable
    function.

    The MY_I2C_ID macro in the example above is used as a placeholder for the
    actual I2C bus ID (as defined by the processor-specific I2C_MODULE_ID enum).

	This feature may not be available on all devices. Please refer to the specific
	device data sheet to determine availability or use PLIB_I2C_ExistsSMBus
	in your application to determine whether this feature is available.
*/

void PLIB_I2C_SMBDisable ( I2C_MODULE_ID index );


//******************************************************************************
/* Function:
    void PLIB_I2C_HighFrequencyEnable ( I2C_MODULE_ID index )

  Summary:
    Enables the I2C module to use high frequency (400 kHz or 1 MHz) signaling.

  Description:
    This function enables the I2C module to use high-frequency signaling,
    allowing it to use the 400 kHz and 1 MHz signaling rates.

  Precondition:
    None.

  Parameters:
    index      - Identifies the desired I2C module

  Returns:
    None.

  Example:
    <code>
    #define MY_I2C_ID I2C_ID_1

    PLIB_I2C_HighFrequencyEnable ( MY_I2C_ID );
    </code>

  Remarks:
    The high-speed feature can be disabled by calling the
    PLIB_I2C_HighSpeedEnable function.

    This feature must be enabled if frequencies higher than 100 kHz programmed
    using the PLIB_I2C_BaudRateSet function.

    The MY_I2C_ID macro in the example above is used as a placeholder for the
    actual I2C bus ID (as defined by the processor-specific I2C_MODULE_ID enum).

	This feature may not be available on all devices. Please refer to the specific
	device data sheet to determine availability or use PLIB_I2C_ExistsHighFrequency
	in your application to determine whether this feature is available.
*/

void PLIB_I2C_HighFrequencyEnable ( I2C_MODULE_ID index );


//******************************************************************************
/* Function:
    void PLIB_I2C_HighFrequencyDisable ( I2C_MODULE_ID index )

  Summary:
    Disables the I2C module from using high frequency (400 kHz or 1 MHz)
    signaling.

  Description:
    This function disables the I2C module from using high-frequency signaling,
    preventing it from using the 400 kHz and 1 MHz signaling rates.

  Precondition:
    None.

  Parameters:
    index      - Identifies the desired I2C module

  Returns:
    None.

  Example:
    <code>
    #define MY_I2C_ID I2C_ID_1

    PLIB_I2C_HighFrequencyDisable ( MY_I2C_ID );
    </code>

  Remarks:
    The high-frequency feature can be re-enabled by calling the
    PLIB_I2C_HighFrequencyEnable function.

    The MY_I2C_ID macro in the example above is used as a placeholder for the
    actual I2C bus ID (as defined by the processor-specific I2C_MODULE_ID enum).

	This feature may not be available on all devices. Please refer to the specific
	device data sheet to determine availability or use PLIB_I2C_ExistsHighFrequency
	in your application to determine whether this feature is available.
*/

void PLIB_I2C_HighFrequencyDisable ( I2C_MODULE_ID index );


//******************************************************************************
/* Function:
    void PLIB_I2C_ReservedAddressProtectEnable ( I2C_MODULE_ID index )

  Summary:
    Enables the I2C module to protect (not respond to) reserved addresses.

  Description:
    This function enables the I2C module to protect reserved addresses by not
    responding to them, even if they match the module's slave address and mask.

  Precondition:
    None.

  Parameters:
    index      - Identifies the desired I2C module

  Returns:
    None.

  Example:
    <code>
    #define MY_I2C_ID I2C_ID_1

    PLIB_I2C_ReservedAddressProtectEnable(MY_I2C_ID);
    </code>

  Remarks:
    The reserved-address-protect feature can be disabled by calling the
    PLIB_I2C_ReservedAddressProtectDisable function.

    The MY_I2C_ID macro in the example above is used as a placeholder for the
    actual I2C bus ID (as defined by the processor-specific I2C_MODULE_ID enum).

	This feature may not be available on all devices. Please refer to the specific
	device data sheet to determine availability or use
	PLIB_I2C_ExistsReservedAddressProtect in your application to determine whether
	this feature is available.
*/

void PLIB_I2C_ReservedAddressProtectEnable ( I2C_MODULE_ID index );


//******************************************************************************
/* Function:
    void PLIB_I2C_ReservedAddressProtectDisable ( I2C_MODULE_ID index )

  Summary:
    Disables the I2C module from protecting reserved addresses, allowing it to
    respond to them.

  Description:
    This function disables the I2C module from protecting reserved addresses,
    allowing it to respond to them when they match the module's slave address and
    mask.

  Precondition:
    None.

  Parameters:
    index      - Identifies the desired I2C module

  Returns:
    None.

  Example:
    <code>
    #define MY_I2C_ID I2C_ID_1

    PLIB_I2C_ReservedAddressProtectDisable ( MY_I2C_ID );
    </code>

  Remarks:
    The reserved-address-protect feature can be re-enabled by calling the
    PLIB_I2C_ReservedAddressProtectEnable function.

    The MY_I2C_ID macro in the example above is used as a placeholder for the
    actual I2C bus ID (as defined by the processor-specific I2C_MODULE_ID enum).

	This feature may not be available on all devices. Please refer to the specific
	device data sheet to determine availability or use
	PLIB_I2C_ExistsReservedAddressProtect in your application to determine whether
	this feature is available.
*/

void PLIB_I2C_ReservedAddressProtectDisable ( I2C_MODULE_ID index );


//******************************************************************************
/* Function:
    void PLIB_I2C_StopInIdleEnable ( I2C_MODULE_ID index )

  Summary:
    Enables the I2C module to stop when the processor enters Idle mode

  Description:
    This function enables the I2C module to stop when the processor enters Idle
    mode.

  Precondition:
    None.

  Parameters:
    index      - Identifies the desired I2C module

  Returns:
    None.

  Example:
    <code>
    #define MY_I2C_ID I2C_ID_1

    PLIB_I2C_StopInIdleEnable ( MY_I2C_ID );
    </code>

  Remarks:
    The Stop-in-Idle feature can be disabled by calling the
    PLIB_I2C_StopInIdleDisable function.

    The MY_I2C_ID macro in the example above is used as a placeholder for the
    actual I2C bus ID (as defined by the processor-specific I2C_MODULE_ID enum).

	This feature may not be available on all devices. Please refer to the specific
	device data sheet to determine availability or use PLIB_I2C_ExistsStopInIdle
	in your application to determine whether this feature is available.
*/

void PLIB_I2C_StopInIdleEnable ( I2C_MODULE_ID index );


//******************************************************************************
/* Function:
    void PLIB_I2C_StopInIdleDisable ( I2C_MODULE_ID index )

  Summary:
    Disables the Stop-in-Idle feature.

  Description:
    This function disables the Stop-in-Idle feature, preventing the I2C module
    from stopping when the processor enters Idle mode.

  Precondition:
    None.

  Parameters:
    index      - Identifies the desired I2C module

  Returns:
    None.

  Example:
    <code>
    #define MY_I2C_ID I2C_ID_1

    PLIB_I2C_StopInIdleDisable ( MY_I2C_ID );
    </code>

  Remarks:
    The Stop-in-Idle feature can be re-enabled by calling the
    PLIB_I2C_StopInIdleEnable function.

    The MY_I2C_ID macro in the example above is used as a placeholder for the
    actual I2C bus ID (as defined by the processor-specific I2C_MODULE_ID enum).

	This feature may not be available on all devices. Please refer to the specific
	device data sheet to determine availability or use PLIB_I2C_ExistsStopInIdle
	in your application to determine whether this feature is available.
*/

void PLIB_I2C_StopInIdleDisable ( I2C_MODULE_ID index );


//******************************************************************************
/* Function:
    void PLIB_I2C_IPMIEnable ( I2C_MODULE_ID index )

  Summary:
    Enables the I2C module to support the Intelligent Platform Management
    Interface (IPMI) specification (see Remarks).

  Description:
    This function enables the I2C module to support the IPMI specification.

  Precondition:
    None.

  Parameters:
    index      - Identifies the desired I2C module

  Returns:
    None.

  Example:
    <code>
    #define MY_I2C_ID I2C_ID_1

    PLIB_I2C_IPMIEnable ( MY_I2C_ID );
    </code>

  Remarks:
    Please refer to the IPMI specification for details of the Intelligent
    Platform Management Interface. The IPMI specification is the property of
    Intel Corporation, Hewlett-Packard Company, NEC Corporation, and Dell Inc.

    The MY_I2C_ID macro in the example above is used as a placeholder for the
    actual I2C bus ID (as defined by the processor-specific I2C_MODULE_ID enum).

	This feature may not be available on all devices. Please refer to the specific
	device data sheet to determine availability or use PLIB_I2C_ExistsIPMI
	in your application to determine whether this feature is available.
*/

void PLIB_I2C_IPMIEnable ( I2C_MODULE_ID index );


//******************************************************************************
/* Function:
    void PLIB_I2C_IPMIDisable ( I2C_MODULE_ID index )

  Summary:
    Disables the I2C module's support for the IPMI specification

  Description:
    This function disables the I2C module's support for the IPMI specification.

  Precondition:
    None.

  Parameters:
    index      - Identifies the desired I2C module

  Returns:
    None.

  Example:
    <code>
    #define MY_I2C_ID I2C_ID_1

    PLIB_I2C_IPMIDisable ( MY_I2C_ID );
    </code>

  Remarks:
    Please refer to the IPMI specification for details of the Intelligent
    Platform Management Interface.  The IPMI specification is the property of
    Intel Corporation, Hewlett-Packard Company, NEC Corporation, and Dell Inc.

    The MY_I2C_ID macro in the example above is used as a placeholder for the
    actual I2C bus ID (as defined by the processor-specific I2C_MODULE_ID enum).

	This feature may not be available on all devices. Please refer to the specific
	device data sheet to determine availability or use PLIB_I2C_ExistsIPMI
	in your application to determine whether this feature is available.
*/

void PLIB_I2C_IPMIDisable ( I2C_MODULE_ID index );


//******************************************************************************
/* Function:
    void PLIB_I2C_SlaveInterruptOnStopEnable ( I2C_MODULE_ID index )

  Summary:
    Enables the feature of generating interrupt on stop condition.

  Description:
    This function enables the feature of generating interrupt on stop condition.

  Precondition:
    None.

  Parameters:
    index      - Identifies the desired I2C module

  Returns:
    None.

  Example:
    <code>
    #define MY_I2C_ID I2C_ID_1

    PLIB_I2C_SlaveInterruptOnStopEnable ( MY_I2C_ID );
    </code>

  Remarks:
    The MY_I2C_ID macro in the example above is used as a placeholder for the
    actual I2C bus ID (as defined by the processor-specific I2C_MODULE_ID enum).

	This feature may not be available on all devices. Please refer to the specific
	device data sheet to determine availability or use PLIB_I2C_ExistsSlaveInterruptOnStop
	in your application to determine whether this feature is available.

	This API is applicable only in I2C slave mode.
*/

void PLIB_I2C_SlaveInterruptOnStopEnable ( I2C_MODULE_ID index );


//******************************************************************************
/* Function:
    void PLIB_I2C_SlaveInterruptOnStopDisable ( I2C_MODULE_ID index )

  Summary:
    Disables the feature of generating interrupt on stop condition.

  Description:
    This function disables the feature of generating interrupt on stop condition.

  Precondition:
    None.

  Parameters:
    index      - Identifies the desired I2C module

  Returns:
    None.

  Example:
    <code>
    #define MY_I2C_ID I2C_ID_1

    PLIB_I2C_SlaveInterruptOnStopDisable ( MY_I2C_ID );
    </code>

  Remarks:
    The MY_I2C_ID macro in the example above is used as a placeholder for the
    actual I2C bus ID (as defined by the processor-specific I2C_MODULE_ID enum).

	This feature may not be available on all devices. Please refer to the specific
	device data sheet to determine availability or use PLIB_I2C_ExistsSlaveInterruptOnStop
	in your application to determine whether this feature is available.
*/

void PLIB_I2C_SlaveInterruptOnStopDisable ( I2C_MODULE_ID index );


//******************************************************************************
/* Function:
    void PLIB_I2C_SlaveInterruptOnStartEnable ( I2C_MODULE_ID index )

  Summary:
    Enables the feature of generating interrupt on start condition.

  Description:
    This function enables the feature of generating interrupt on start condition.

  Precondition:
    None.

  Parameters:
    index      - Identifies the desired I2C module

  Returns:
    None.

  Example:
    <code>
    #define MY_I2C_ID I2C_ID_1

    PLIB_I2C_SlaveInterruptOnStartEnable ( MY_I2C_ID );
    </code>

  Remarks:
    The MY_I2C_ID macro in the example above is used as a placeholder for the
    actual I2C bus ID (as defined by the processor-specific I2C_MODULE_ID enum).

	This feature may not be available on all devices. Please refer to the specific
	device data sheet to determine availability or use PLIB_I2C_ExistsSlaveInterruptOnStart
	in your application to determine whether this feature is available.

	This API is applicable only in I2C slave mode.
*/

void PLIB_I2C_SlaveInterruptOnStartEnable ( I2C_MODULE_ID index );


//******************************************************************************
/* Function:
    void PLIB_I2C_SlaveInterruptOnStartDisable ( I2C_MODULE_ID index )

  Summary:
    Disables the feature of generating interrupt on start condition.

  Description:
    This function disables the feature of generating interrupt on start condition.

  Precondition:
    None.

  Parameters:
    index      - Identifies the desired I2C module

  Returns:
    None.

  Example:
    <code>
    #define MY_I2C_ID I2C_ID_1

    PLIB_I2C_SlaveInterruptOnStartDisable ( MY_I2C_ID );
    </code>

  Remarks:
    The MY_I2C_ID macro in the example above is used as a placeholder for the
    actual I2C bus ID (as defined by the processor-specific I2C_MODULE_ID enum).

	This feature may not be available on all devices. Please refer to the specific
	device data sheet to determine availability or use PLIB_I2C_ExistsSlaveInterruptOnStart
	in your application to determine whether this feature is available.
*/

void PLIB_I2C_SlaveInterruptOnStartDisable ( I2C_MODULE_ID index );


//******************************************************************************
/* Function:
    void PLIB_I2C_SlaveBufferOverwriteEnable ( I2C_MODULE_ID index )

  Summary:
    Enables buffer overwrite.

  Description:
    This function enables buffer overwrite. If the buffer overwrite is enabled,
    on data receive, even if the previous data is not read, the buffer will be
    filled with new data and acknowledge will be generated.

  Precondition:
    None.

  Parameters:
    index      - Identifies the desired I2C module

  Returns:
    None.

  Example:
    <code>
    #define MY_I2C_ID I2C_ID_1

    PLIB_I2C_SlaveBufferOverwriteEnable ( MY_I2C_ID );
    </code>

  Remarks:
    The MY_I2C_ID macro in the example above is used as a placeholder for the
    actual I2C bus ID (as defined by the processor-specific I2C_MODULE_ID enum).

	This feature may not be available on all devices. Please refer to the specific
	device data sheet to determine availability or use PLIB_I2C_ExistsSlaveBufferOverwrite
	in your application to determine whether this feature is available.

	This API is applicable only in I2C slave mode.
*/

void PLIB_I2C_SlaveBufferOverwriteEnable ( I2C_MODULE_ID index );


//******************************************************************************
/* Function:
    void PLIB_I2C_SlaveBufferOverwriteDisable ( I2C_MODULE_ID index )

  Summary:
    Disables buffer overwrite.

  Description:
    This function disables buffer overwrite. If the buffer overwrite is disabled,
    on data receive, when the previous data is not read, the buffer will not be
    overwritten.

  Precondition:
    None.

  Parameters:
    index      - Identifies the desired I2C module

  Returns:
    None.

  Example:
    <code>
    #define MY_I2C_ID I2C_ID_1

    PLIB_I2C_SlaveBufferOverwriteDisable ( MY_I2C_ID );
    </code>

  Remarks:
    The MY_I2C_ID macro in the example above is used as a placeholder for the
    actual I2C bus ID (as defined by the processor-specific I2C_MODULE_ID enum).

	This feature may not be available on all devices. Please refer to the specific
	device data sheet to determine availability or use PLIB_I2C_ExistsSlaveBufferOverwrite
	in your application to determine whether this feature is available.
*/

void PLIB_I2C_SlaveBufferOverwriteDisable ( I2C_MODULE_ID index );


//******************************************************************************
/* Function:
    void PLIB_I2C_DataLineHoldTimeSet ( I2C_MODULE_ID index, I2C_SDA_MIN_HOLD_TIME sdaHoldTimeNs )

  Summary:
    Sets the data line hold time.

  Description:
    This function sets the data line hold time.

  Precondition:
    None.

  Parameters:
    index      - Identifies the desired I2C module.

    sdaHoldTimeNs - SDA hold time in nanoseconds.

  Returns:
    None.

  Example:
    <code>
    #define MY_I2C_ID I2C_ID_1

    PLIB_I2C_PLIB_I2C_DataLineHoldTimeSet ( MY_I2C_ID, I2C_SDA_MIN_HOLD_TIME_300NS );
    </code>

  Remarks:
    The MY_I2C_ID macro in the example above is used as a placeholder for the
    actual I2C bus ID (as defined by the processor-specific I2C_MODULE_ID enum).

	This feature may not be available on all devices. Please refer to the specific
	device data sheet to determine availability or use PLIB_I2C_ExistsDataLineHoldTime
	in your application to determine whether this feature is available.
*/

void PLIB_I2C_DataLineHoldTimeSet ( I2C_MODULE_ID index, I2C_SDA_MIN_HOLD_TIME sdaHoldTimeNs );


//******************************************************************************
/* Function:
    void PLIB_I2C_SlaveBusCollisionDetectEnable ( I2C_MODULE_ID index )

  Summary:
    Enables slave bus collision interrupts.

  Description:
    This function enables bus collision interrupts.

  Precondition:
    None.

  Parameters:
    index      - Identifies the desired I2C module

  Returns:
    None.

  Example:
    <code>
    #define MY_I2C_ID I2C_ID_1

    PLIB_I2C_SlaveAddressHoldEnable ( MY_I2C_ID );
    </code>

  Remarks:
    The MY_I2C_ID macro in the example above is used as a placeholder for the
    actual I2C bus ID (as defined by the processor-specific I2C_MODULE_ID enum).

	This feature may not be available on all devices. Please refer to the specific
	device data sheet to determine availability or use PLIB_I2C_SlaveBusCollisionDetect
	in your application to determine whether this feature is available.

	This API is applicable only in I2C slave mode.
*/

void PLIB_I2C_SlaveBusCollisionDetectEnable ( I2C_MODULE_ID index );


//******************************************************************************
/* Function:
    void PLIB_SlaveBusCollisionDetectDisable ( I2C_MODULE_ID index )

  Summary:
    Disables bus collision detect interrupts.

  Description:
    This function disables bus collision detect interrupts.

  Precondition:
    None.

  Parameters:
    index      - Identifies the desired I2C module

  Returns:
    None.

  Example:
    <code>
    #define MY_I2C_ID I2C_ID_1

    PLIB_I2C_SlaveBusCollisionDetectDisable ( MY_I2C_ID );
    </code>

  Remarks:
    The MY_I2C_ID macro in the example above is used as a placeholder for the
    actual I2C bus ID (as defined by the processor-specific I2C_MODULE_ID enum).

	This feature may not be available on all devices. Please refer to the specific
	device data sheet to determine availability or use PLIB_I2C_SlaveBusCollisionDetect
	in your application to determine whether this feature is available.
*/

void PLIB_I2C_SlaveBusCollisionDetectDisable ( I2C_MODULE_ID index );


//******************************************************************************
/* Function:
    void PLIB_I2C_SlaveAddressHoldEnable ( I2C_MODULE_ID index )

  Summary:
    Enables address holding.

  Description:
    This function enables address holding. If address byte is received, following
    the 8th falling edge of clock for a clock line will be held low.

  Precondition:
    None.

  Parameters:
    index      - Identifies the desired I2C module

  Returns:
    None.

  Example:
    <code>
    #define MY_I2C_ID I2C_ID_1

    PLIB_I2C_SlaveAddressHoldEnable ( MY_I2C_ID );
    </code>

  Remarks:
    The MY_I2C_ID macro in the example above is used as a placeholder for the
    actual I2C bus ID (as defined by the processor-specific I2C_MODULE_ID enum).

	This feature may not be available on all devices. Please refer to the specific
	device data sheet to determine availability or use PLIB_I2C_SlaveAddressHoldEnable
	in your application to determine whether this feature is available.

	This API is applicable only in I2C slave mode.
*/

void PLIB_I2C_SlaveAddressHoldEnable ( I2C_MODULE_ID index );


//******************************************************************************
/* Function:
    void PLIB_I2C_SlaveAddressHoldDisable ( I2C_MODULE_ID index )

  Summary:
    Disables Address holding.

  Description:
    This function disables address holding.

  Precondition:
    None.

  Parameters:
    index      - Identifies the desired I2C module

  Returns:
    None.

  Example:
    <code>
    #define MY_I2C_ID I2C_ID_1

    PLIB_I2C_SlaveAddressHoldDisable ( MY_I2C_ID );
    </code>

  Remarks:
    The MY_I2C_ID macro in the example above is used as a placeholder for the
    actual I2C bus ID (as defined by the processor-specific I2C_MODULE_ID enum).

	This feature may not be available on all devices. Please refer to the specific
	device data sheet to determine availability or use PLIB_I2C_SlaveAddressHoldEnable
	in your application to determine whether this feature is available.
*/

void PLIB_I2C_SlaveAddressHoldDisable ( I2C_MODULE_ID index );


//******************************************************************************
/* Function:
    void PLIB_I2C_SlaveDataHoldEnable ( I2C_MODULE_ID index )

  Summary:
    Enables data holding.

  Description:
    This function enables data holding. If a data byte is received, following
    the 8th falling edge of clock for a clock line will be held low.

  Precondition:
    None.

  Parameters:
    index      - Identifies the desired I2C module

  Returns:
    None.

  Example:
    <code>
    #define MY_I2C_ID I2C_ID_1

    PLIB_I2C_SlaveDataHoldEnable ( MY_I2C_ID );
    </code>

  Remarks:
    The MY_I2C_ID macro in the example above is used as a placeholder for the
    actual I2C bus ID (as defined by the processor-specific I2C_MODULE_ID enum).

	This feature may not be available on all devices. Please refer to the specific
	device data sheet to determine availability or use PLIB_I2C_SlaveDataHoldEnable
	in your application to determine whether this feature is available.

	This API is supported only for the slave mode of I2C.
*/

void PLIB_I2C_SlaveDataHoldEnable ( I2C_MODULE_ID index );


//******************************************************************************
/* Function:
    void PLIB_I2C_SlaveDataHoldDisable ( I2C_MODULE_ID index )

  Summary:
    Disables data holding.

  Description:
    This function disables data holding.

  Precondition:
    None.

  Parameters:
    index      - Identifies the desired I2C module

  Returns:
    None.

  Example:
    <code>
    #define MY_I2C_ID I2C_ID_1

    PLIB_I2C_SlaveDataHoldDisable ( MY_I2C_ID );
    </code>

  Remarks:
    The MY_I2C_ID macro in the example above is used as a placeholder for the
    actual I2C bus ID (as defined by the processor-specific I2C_MODULE_ID enum).

	This feature may not be available on all devices. Please refer to the specific
	device data sheet to determine availability or use PLIB_I2C_SlaveDataHoldEnable
	in your application to determine whether this feature is available.
*/

void PLIB_I2C_SlaveDataHoldDisable ( I2C_MODULE_ID index );


//******************************************************************************
/* Function:
    void PLIB_I2C_Enable( I2C_MODULE_ID index )

  Summary:
    Enables the specified I2C module.

  Description:
    This function enables the specified I2C module.

  Precondition:
    The module should be appropriately configured before being enabled.

  Parameters:
    index      - Identifies the desired I2C module

  Returns:
    None.

  Example:
    <code>
    #define MY_I2C_ID 				I2C_ID_1
    #define MY_CLOCK_FREQUENCY    	80000000
	#define MY_BAUD_RATE			10000
	#define MY_SLAVE_ADDRESS		0x23

    PLIB_I2C_SlaveClockStretchingEnable ( MY_I2C_ID );
    PLIB_I2C_SMBDisable ( MY_I2C_ID );
    PLIB_I2C_HighFrequencyDisable ( MY_I2C_ID );
    PLIB_I2C_ReservedAddressProtectEnable ( MY_I2C_ID );
    PLIB_I2C_StopInIdleDisable ( MY_I2C_ID );
    PLIB_I2C_BaudRateSet ( MY_I2C_ID, MY_CLOCK_FREQUENCY, MY_BAUD_RATE );

    PLIB_I2C_SlaveAddress7BitSet( MY_I2C_ID, MY_SLAVE_ADDRESS );
    PLIB_I2C_Enable ( MY_I2C_ID );
    </code>

  Remarks:

    The MY_I2C_ID macro in the example above is used as a placeholder for the
    actual I2C bus ID (as defined by the processor-specific I2C_MODULE_ID enum).

	This feature may not be available on all devices. Please refer to the specific
	device data sheet to determine availability or use PLIB_I2C_ExistsModuleEnable
	in your application to determine whether this feature is available.
*/

void PLIB_I2C_Enable( I2C_MODULE_ID index );


//******************************************************************************
/* Function:
    void PLIB_I2C_Disable( I2C_MODULE_ID index )

  Summary:
    Disables the specified I2C module.

  Description:
    This function disables the specified I2C module.

  Precondition:
    None.

  Parameters:
    index      - Identifies the desired I2C module

  Returns:
    None.

  Example:
    <code>
    #define MY_I2C_ID I2C_ID_1

    PLIB_I2C_Disable( MY_I2C_ID );
    </code>

  Remarks:

    The MY_I2C_ID macro in the example above is used as a placeholder for the
    actual I2C bus ID (as defined by the processor-specific I2C_MODULE_ID enum).

	This feature may not be available on all devices. Please refer to the specific
	device data sheet to determine availability or use PLIB_I2C_ExistsModuleEnable
	in your application to determine whether this feature is available.
*/

void PLIB_I2C_Disable( I2C_MODULE_ID index );


// *****************************************************************************
// *****************************************************************************
// Section: General Status
// *****************************************************************************
// *****************************************************************************

//******************************************************************************
/* Function:
    bool PLIB_I2C_StartWasDetected ( I2C_MODULE_ID index )

  Summary:
    Identifies when a Start condition has been detected.

  Description:
    This function identifies when a Start condition has been detected.

  Precondition:
    None.

  Parameters:
    index     	- Identifies the desired I2C module

  Returns:
    - true  - A Start Condition has been detected
    - false - No Start condition has been detected since the last time a
              Stop condition was detected (or the module was initialized)

  Example:
    <code>
    #define MY_I2C_ID I2C_ID_1

    if ( PLIB_I2C_StartWasDetected ( MY_I2C_ID ) )
    {
        // Handle Start condition
    }
    </code>

  Remarks:
    This flag is cleared automatically by the hardware when a stop condition is
    detected, but it can also be cleared by software using PLIB_I2C_StartClear
    function.

    The MY_I2C_ID macro in the example above is used as a placeholder for the
    actual I2C bus ID (as defined by the processor-specific I2C_MODULE_ID enum).

	This feature may not be available on all devices. Please refer to the specific
	device data sheet to determine availability or use PLIB_I2C_ExistsStartDetect
	in your application to determine whether this feature is available.
*/

bool PLIB_I2C_StartWasDetected ( I2C_MODULE_ID index );


//******************************************************************************
/* Function:
    void PLIB_I2C_StartClear ( I2C_MODULE_ID index )

  Summary:
    Clears the start status flag

  Description:
    This function clears the start status flag.

  Precondition:
    None.

  Parameters:
    index          - Identifies the desired I2C module

  Returns:
    None.

  Example:
    <code>
    #define MY_I2C_ID I2C_ID_1

    if ( PLIB_I2C_StartWasDetected( MY_I2C_ID ) )
    {
        // Handle Start condition

        PLIB_I2C_StartClear(MY_I2C_ID);
    }
    </code>

  Remarks:
    This flag is cleared automatically by the hardware when a Stop condition is
    detected, but it can also be cleared by software.

    The MY_I2C_ID macro in the example above is used as a placeholder for the
    actual I2C bus ID (as defined by the processor-specific I2C_MODULE_ID enum).

	This feature may not be available on all devices. Please refer to the specific
	device data sheet to determine availability or use PLIB_I2C_ExistsStartDetect
	in your application to determine whether this feature is available.
*/

void PLIB_I2C_StartClear ( I2C_MODULE_ID index );


//******************************************************************************
/* Function:
    bool PLIB_I2C_StopWasDetected ( I2C_MODULE_ID index )

  Summary:
    Identifies when a Stop condition has been detected

  Description:
    This function identifies when a Stop condition has been detected.

  Precondition:
    None.

  Parameters:
    index          - Identifies the desired I2C module

  Returns:
    - true  - A Stop condition has been detected
    - false - No Stop condition has been detected since the last time a
              Start condition was detected (or the module was initialized)

  Example:
    <code>
    #define MY_I2C_ID I2C_ID_1

    if ( PLIB_I2C_StopWasDetected ( MY_I2C_ID ) )
    {
        // Handle stop condition
    }
    </code>

  Remarks:
    This flag is cleared automatically by the hardware when a Start condition is
    detected, but it can also be cleared by software using the PLIB_I2C_StopClear
    function.

    The MY_I2C_ID macro in the example above is used as a placeholder for the
    actual I2C bus ID (as defined by the processor-specific I2C_MODULE_ID enum).

	This feature may not be available on all devices. Please refer to the specific
	device data sheet to determine availability or use PLIB_I2C_ExistsStopDetect
	in your application to determine whether this feature is available.
*/

bool PLIB_I2C_StopWasDetected ( I2C_MODULE_ID index );


//******************************************************************************
/* Function:
    void PLIB_I2C_StopClear ( I2C_MODULE_ID index )

  Summary:
    Clears the stop status flag

  Description:
    This function clears the stop status flag.

  Precondition:
    None.

  Parameters:
    index          - Identifies the desired I2C module

  Returns:
    None.

  Example:
    <code>
    #define MY_I2C_ID I2C_ID_1

    if ( PLIB_I2C_StopWasDetected ( MY_I2C_ID ) )
    {
        // Handle stop condition

        PLIB_I2C_StopClear ( MY_I2C_ID );
    }
    </code>

  Remarks:
    This flag is cleared automatically by the hardware when a Start condition is
    detected, but it can also be cleared by software.

    The MY_I2C_ID macro in the example above is used as a placeholder for the
    actual I2C bus ID (as defined by the processor-specific I2C_MODULE_ID enum).

	This feature may not be available on all devices. Please refer to the specific
	device data sheet to determine availability or use PLIB_I2C_ExistsStopDetect
	in your application to determine whether this feature is available.
*/

void PLIB_I2C_StopClear ( I2C_MODULE_ID index );


//******************************************************************************
/* Function:
    bool PLIB_I2C_ArbitrationLossHasOccurred ( I2C_MODULE_ID index )

  Summary:
    Identifies if bus arbitration has been lost.

  Description:
    This function identifies if bus arbitration has been lost.

  Precondition:
    Bus collisions can occur during any master-mode transmission including:
        * Sending a Start condition
        * Sending a repeated Start condition
        * Sending an address or data byte
        * sending an ACK/NAK bit
        * Sending a Stop condition

  Parameters:
    index          - Identifies the desired I2C module

  Returns:
    - true  - If software if a bus collision occurred, resulting in loss of
              bus arbitration
    - false - If no bus collision occurred

  Example:
    <code>
    #define MY_I2C_ID I2C_ID_1

    if ( PLIB_I2C_ArbitrationLossHasOccurred ( MY_I2C_ID ) )
    {
        // Handle bus collision

        PLIB_I2C_ArbitrationLossClear( MY_I2C_ID );
    }
    </code>

  Remarks:
    The arbitration status should be checked after any Master mode transmission
    or if an error interrupt occurs.  If a bus collision occurs, the entire
    transmission (from the Start condition to the Stop or restart condition)
    must be retried later when the bus becomes idle.

    This flag should be cleared by software using the
    PLIB_I2C_ArbitrationLossClear function.

    The MY_I2C_ID macro in the example above is used as a placeholder for the
    actual I2C bus ID (as defined by the processor-specific I2C_MODULE_ID enum).

	This feature may not be available on all devices. Please refer to the specific
	device data sheet to determine availability or use
	PLIB_I2C_ExistsArbitrationLoss in your application to determine whether this
	feature is available.
*/

bool PLIB_I2C_ArbitrationLossHasOccurred ( I2C_MODULE_ID index );


//******************************************************************************
/* Function:
    void PLIB_I2C_ArbitrationLossClear ( I2C_MODULE_ID index )

  Summary:
    Clears the arbitration loss status flag

  Description:
    This function clears the arbitration loss status flag.

  Precondition:
    None.

  Parameters:
    index          - Identifies the desired I2C module

  Returns:
    None.

  Example:
    <code>
    #define MY_I2C_ID I2C_ID_1

    if ( PLIB_I2C_ArbitrationLossHasOccurred ( MY_I2C_ID ) )
    {
        // Handle bus collision

        PLIB_I2C_ArbitrationLossClear ( MY_I2C_ID );
    }
    </code>

  Remarks:
    This flag is set by hardware when bus arbitration loss occurs. Its status
    can be tested using the PLIB_I2C_ArbitrationLossHasOccurred function.  This
    flag should be cleared by software after the arbitration loss has been
    handled. To handle the error, the entire transmission (from the Start
    condition to the Stop or restart condition) must be retried later when the
    bus becomes idle.

    The MY_I2C_ID macro in the example above is used as a placeholder for the
    actual I2C bus ID (as defined by the processor-specific I2C_MODULE_ID enum).

	This feature may not be available on all devices. Please refer to the specific
	device data sheet to determine availability or use PLIB_I2C_ExistsArbitrationLoss
	in your application to determine whether this feature is available.
*/

void PLIB_I2C_ArbitrationLossClear ( I2C_MODULE_ID index );


//******************************************************************************
/* Function:
    bool PLIB_I2C_BusIsIdle ( I2C_MODULE_ID index )

  Summary:
    Determines whether the I2C bus is idle or busy.

  Description:
    This function checks to see if the I2C bus is currently idle or if there is
    some activity currently taking place.

  Precondition:
    The module must be configured appropriately and enabled.

  Parameters:
    index          - Identifies the desired I2C module

  Returns:
    - true   - The bus is currently idle. It is safe to start a master transfer.
    - false  - The bus is currently busy. Do not start a master transfer.

  Example:
    <code>
    #define MY_I2C_ID I2C_ID_1

    if (PLIB_I2C_BusIsIdle ( MY_I2C_ID ))
    {
        PLIB_I2C_MasterStart ( MY_I2C_ID );
    }
    </code>

  Remarks:
    When this function returns true it does not guarantee that a bus
    arbitration loss cannot occur.  Two or more masters can start a transfer
    within the minimum start signal hold time. (Refer to the I2C specification
    for a definition of the minimum Start condition hold time.)

    The MY_I2C_ID macro in the example above is used as a placeholder for the
    actual I2C bus ID (as defined by the processor-specific I2C_MODULE_ID enum).

	This feature may not be available on all devices. Please refer to the specific
	device data sheet to determine availability or use PLIB_I2C_ExistsBusIsIdle
	in your application to determine whether this feature is available.
*/

bool PLIB_I2C_BusIsIdle ( I2C_MODULE_ID index );


// *****************************************************************************
// *****************************************************************************
// Section: Baud Rate Generator Control Functions
// *****************************************************************************
// *****************************************************************************

//******************************************************************************
/* Function:
    void PLIB_I2C_BaudRateSet ( I2C_MODULE_ID index, uint32_t clockFrequencyHz,
    	I2C_BAUD_RATE baudRate )

  Summary:
    Sets the desired baud rate.

  Description:
    This function sets the desired baud rate so that the I2C module will operate
    at the desired clock frequency (on the SCL line of the bus.)

  Precondition:
    The source clock, being sent to the I2C module (internal to the
    microcontroller) must be operating at the frequency passed.

  Parameters:
    index                 - Identifies the desired I2C module

    clockFrequencyHz      - Clock Frequency (Hz)

    baudRate      	  - The desired baud rate value. This should be an appropriate
 	                    value for the frequency and microcontroller in use.


  Returns:
    None.

  Example:
    <code>
    #define MY_I2C_ID I2C_ID_1
	#define MY_CLOCK_FREQUENCY_INPUT 80000000

    PLIB_I2C_HighFrequencyDisable ( MY_I2C_ID );
    PLIB_I2C_BaudRateSet ( MY_I2C_ID, MY_CLOCK_FREQUENCY_INPUT, 40000 );

    </code>

  Remarks:
    IMPORTANT: The I2C module's high-frequency mode must be enabled for
    frequencies higher than 100 kHz using the PLIB_I2C_HighFrequencyEnable
    function.  Otherwise, the high-frequency mode must be disabled using the
    PLIB_I2C_HighFrequencyDisable function.

    The actual frequency selected may be slightly different than the frequency
    requested due to truncation errors.  Use the PLIB_I2C_BaudRateGet function to
    obtain the actual baud rate value that has been programmed.

    The actual frequency observed on the SCL line may be lower due to clock
    stretching.

    If the system clock is changed during an active receive operation, a
    receiver error or data loss may result. To avoid this issue, verify that
    no receptions are in progress before changing the system clock.

    The MY_I2C_ID macro in the example above is used as a placeholder for the
    actual I2C bus ID (as defined by the processor-specific I2C_MODULE_ID enum).

	This feature may not be available on all devices. Please refer to the specific
	device data sheet to determine availability or use PLIB_I2C_ExistsBaudRate
	in your application to determine whether this feature is available.
*/

 void PLIB_I2C_BaudRateSet ( I2C_MODULE_ID index, uint32_t clockFrequencyHz, I2C_BAUD_RATE baudRate );


//******************************************************************************
/* Function:
    I2C_BAUD_RATE PLIB_I2C_BaudRateGet ( I2C_MODULE_ID index, uint32_t clockFrequencyHz )

  Summary:
    Calculates the I2C module's current SCL clock frequency.

  Description:
    This function calculates the I2C module's current SCL clock frequency.

  Precondition:
    The returned value may not be valid if PLIB_I2C_BaudRateSet has not been
    previously called to set the SCL clock frequency.

  Parameters:
	index          	- Identifies the desired I2C module

	clockFrequencyHz 	- Clock Frequency (Hz) provided for the I2C module

  Returns:
    SCL frequency currently used

  Example:
    <code>
    #define MY_I2C_ID I2C_ID_1
	#define MY_CLOCK_FREQ_INPUT		80000000
    uint32_t myBaudRate;

    myBaudRate = PLIB_I2C_BaudRateGet ( MY_I2C_ID, MY_CLOCK_FREQ_INPUT );

    </code>

  Remarks:
    The actual frequency provided may be slightly different than the frequency
    requested due to truncation errors. The actual frequency observed on the SCL
    line may be lower due to clock stretching.

    The MY_CLOCK_FREQUENCY macro in the example is used as placeholder
    for the actual clock frequency.

    The MY_I2C_ID macro in the example above is used as a placeholder for the
    actual I2C bus ID (as defined by the processor-specific I2C_MODULE_ID enum).

	This feature may not be available on all devices. Please refer to the specific
	device data sheet to determine availability or use PLIB_I2C_ExistsBaudRate
	in your application to determine whether this feature is available.
*/

I2C_BAUD_RATE PLIB_I2C_BaudRateGet ( I2C_MODULE_ID index, uint32_t clockFrequencyHz );


// *****************************************************************************
// *****************************************************************************
// Section: Slave Address Control Functions
// *****************************************************************************
// *****************************************************************************

//******************************************************************************
/* Function:
    void PLIB_I2C_SlaveAddress7BitSet ( I2C_MODULE_ID index, uint8_t address )

  Summary:
    Selects 7-bit Slave mode addressing and sets the slave address value.

  Description:
    This function selects 7-bit Slave mode addressing sets the address to which
    the module will respond when operating in Slave mode.

  Precondition:
    None

  Parameters:
    index      - Identifies the desired I2C module

    address - The 7-bit slave address to which the module will respond
              (The address should be right-aligned in the 8-bit parameter,
              without any read/write bit in the 0 position)

  Returns:
    None.

  Example:
    <code>
    #define MY_I2C_ID I2C_ID_1

    PLIB_I2C_SlaveAddress7BitSet ( MY_I2C_ID, MY_SLAVE_ADDRESS_7_BIT );
    </code>

  Remarks:
    I2C modules on some microcontroller families may also support an address
    mask (to allow the module to respond to multiple addresses.  When using
    these microcontrollers, the PLIB_I2C_SlaveAddress7BitSetMasked may be used
    to support the mask feature.  Refer to the specific device data sheet to
    determine whether this feature is supported for your device.

    The MY_SLAVE_ADDRESS_7_BIT macro in the example is used as a  placeholder
    for the actual desired slave address.

    The MY_I2C_ID macro in the example above is used as a placeholder for the
    actual I2C bus ID (as defined by the processor-specific I2C_MODULE_ID enum).

	This feature may not be available on all devices. Please refer to the
	specific device data sheet to determine availability or use
	PLIB_I2C_ExistsSlaveAddress7Bit in your application to determine whether this
	feature is available.
*/

void PLIB_I2C_SlaveAddress7BitSet ( I2C_MODULE_ID index, uint8_t address );


//******************************************************************************
/* Function:
    uint8_t PLIB_I2C_SlaveAddress7BitGet ( I2C_MODULE_ID index )

  Summary:
    Identifies the current 7-bit Slave mode address.

  Description:
    This function identifies the 7-bit slave address to which the module
    will currently respond.

  Precondition:
    The address provided may not be valid if it has not been previously set.

  Parameters:
    index          - Identifies the desired I2C module

  Returns:
    The 7-bit slave address to which the module is currently set to respond.

  Example:
    <code>
	#define MY_I2C_ID I2C_ID_1
	uint8_t slave_address7bit;

    if (!PLIB_I2C_SlaveAddressModeIs10Bits ( MY_I2C_ID ) )
    {
        slave_address7bit = PLIB_I2C_SlaveAddress7BitGet ( MY_I2C_ID );
    }
    </code>

  Remarks:
    The 7-bit address will be right-aligned in the 8-bit return value.

    The MY_I2C_ID macro in the example above is used as a placeholder for the
    actual I2C bus ID (as defined by the processor-specific I2C_MODULE_ID enum).

	This feature may not be available on all devices. Please refer to the
	specific device data sheet to determine availability or use
	PLIB_I2C_ExistsSlaveAddress7Bit in your application to determine whether
	this feature is available.
*/

uint8_t PLIB_I2C_SlaveAddress7BitGet ( I2C_MODULE_ID index );


//******************************************************************************
/* Function:
    void PLIB_I2C_SlaveAddress10BitSet ( I2C_MODULE_ID index, uint16_t address )

  Summary:
    Selects 10-bit Slave mode addressing and sets the address value.

  Description:
    This function selects 10-bit Slave mode addressing sets the slave address to
    which the module will respond when operating in Slave mode.

  Precondition:
    None

  Parameters:
    index      - Identifies the desired I2C module

    address - The 10-bit slave address to which the module will respond
              (The address should be right-aligned in the 16-bit parameter,
              without any read/write bit in the 0 position)

  Returns:
    None.

  Example:
    <code>
    #define MY_I2C_ID I2C_ID_1
	#define MY_SLAVE_ADDR_10_BIT	0x23

    PLIB_I2C_SlaveAddress10BitSet ( MY_I2C_ID, MY_SLAVE_ADDR_10_BIT );
    </code>

  Remarks:
    I2C modules on some microcontroller families may also support an address
    mask (to allow the module to respond to multiple addresses.  When using
    these microcontrollers, the PLIB_I2C_SlaveAddress10BitSetMasked may be used
    to support the mask feature.  Refer to the specific device data sheet to
    determine whether this feature is supported for your device.

    The MY_SLAVE_ADDRESS_10_BIT macro in the example is used as a placeholder
    for the actual desired slave address.

    The MY_I2C_ID macro in the example above is used as a placeholder for the
    actual I2C bus ID (as defined by the processor-specific I2C_MODULE_ID enum).

	This feature may not be available on all devices. Please refer to the
	specific device data sheet to determine availability or use
	PLIB_I2C_ExistsSlaveAddress10Bit in your application to determine whether
	this feature is available.
*/

void PLIB_I2C_SlaveAddress10BitSet ( I2C_MODULE_ID index, uint16_t address );


//******************************************************************************
/* Function:
    uint16_t PLIB_I2C_SlaveAddress10BitGet ( I2C_MODULE_ID index )

  Summary:
    Identifies the current 10-bit Slave mode address.

  Description:
    This function identifies the 10-bit slave address to which the module
    will currently respond.

  Precondition:
    The address provided may not be valid if it has not been previously set.

  Parameters:
    index          - Identifies the desired I2C module

  Returns:
    The 10-bit slave address to which the module is currently set to respond.

  Example:
    <code>
    #define MY_I2C_ID I2C_ID_1
    uint16_t slaveAddress;

    if (PLIB_I2C_SlaveAddressModeIs10Bits ( MY_I2C_ID ) )
    {
        slaveAddress = PLIB_I2C_SlaveAddress10BitGet ( MY_I2C_ID );
    }
    </code>

  Remarks:
    The 16-bit address will be right-aligned in the 16-bit return value.

    The MY_I2C_ID macro in the example above is used as a placeholder for the
    actual I2C bus ID (as defined by the processor-specific I2C_MODULE_ID enum).

	This feature may not be available on all devices. Please refer to the
	specific device data sheet to determine availability or use
	PLIB_I2C_ExistsSlaveAddress10Bit in your application to determine whether
	this feature is available.
*/

uint16_t PLIB_I2C_SlaveAddress10BitGet ( I2C_MODULE_ID index );


//******************************************************************************
/* Function:
    bool PLIB_I2C_SlaveAddressModeIs10Bits ( I2C_MODULE_ID index )

  Summary:
    Identifies if the current slave address mode is 7-bits or 10-bits.

  Description:
    This function identifies if the current slave addressing mode is
    7-bits or 10-bits.

  Precondition:
    The mode provided may not be valid if the address mode has not been
    previously set.

  Parameters:
    index       - Identifies the desired I2C module

  Returns:
    - true        - If the current slave addressing mode is 10-bits
    - false       - if the current slave addressing mode is 7-bits

  Example:
    <code>
    uint8_t  slave_address7Bit;
    uint16_t slave_address10Bit;

    if ( PLIB_I2C_SlaveAddressModeIs10Bits ( MY_I2C_ID ) )
    {
        slave_address10Bit = PLIB_I2C_SlaveAddress10BitGet ( MY_I2C_ID );
    }
    else
    {
        slave_address7Bit = PLIB_I2C_SlaveAddress7BitGet ( MY_I2C_ID );
    }
    </code>

  Remarks:
    The MY_I2C_ID macro in the example above is used as a placeholder for the
    actual I2C bus ID (as defined by the processor-specific I2C_MODULE_ID enum).

	This feature may not be available on all devices. Please refer to the
	specific device data sheet to determine availability or use
	PLIB_I2C_ExistsSlaveAddress10Bit in your application to determine whether
	this feature is available.
*/

bool PLIB_I2C_SlaveAddressModeIs10Bits ( I2C_MODULE_ID index );


//******************************************************************************
/* Function:
    bool PLIB_I2C_SlaveAddressIsDetected ( I2C_MODULE_ID index )

  Summary:
    Detects if the most recent byte received is a data or an address byte.

  Description:
    This function identifies if the most recent byte received was a data byte or
    an address byte.

  Precondition:
    The module must have been configured appropriately and enabled, and a
    transfer must have been previously started.

  Parameters:
    index          - Identifies the desired I2C module

  Returns:
    - true      - If the byte received is an address byte
    - false     - If the byte received is a data byte

  Example:
    <code>
    #define MY_I2C_ID I2C_ID_1
    uint8_t getData;

    if ( PLIB_I2C_ReceivedByteIsAvailable ( MY_I2C_ID ) )
    {
        if ( PLIB_I2C_SlaveAddressIsDetected ( MY_I2C_ID ) )
        {
            if ( PLIB_I2C_SlaveReadIsRequested ( MY_I2C_ID ) )
            {
                // Begin slave transmit mode
            }
            else
            {
                // Begin slave receive mode
            }
        }

        // Read the data and release the bus
        getData = PLIB_I2C_ReceivedByteGet ( MY_I2C_ID );
        PLIB_I2C_SlaveClockRelease ( MY_I2C_ID );
    }
    </code>

  Remarks:
    This function should only be used by slave receivers.

	This feature may not be available on all devices. Please refer to the
	specific device data sheet to determine availability or use
	PLIB_I2C_ExistsSlaveReadRequest in your application to determine whether
	this feature is available.
*/

bool PLIB_I2C_SlaveAddressIsDetected ( I2C_MODULE_ID index );


/*******************************************************************************
  Function:
    bool PLIB_I2C_SlaveAddress10BitWasDetected ( I2C_MODULE_ID index )

  Summary:
    Detects reception of the second byte of a 10-bit slave address.

  Description:
    This function detects if the second byte of a 10-bit slave address
    (containing the low-order 8 bits) matching the local address has been
    received.

  Precondition:
    The module must have been configured for 10-bit addressing in Slave mode,
    enabled, and the first byte of the 10-bit local slave address must have
    already been received and matched.

  Parameters:
    index          - Identifies the desired I2C module

  Returns:
    - true  - If the second byte of the local 10-bit address has been received
    - false - If the second byte of the local 10-bit address has not been received

  Example:
    <code>
	#define MY_I2C_ID I2C_ID_1
	uint8_t  i2cReadData;

    if ( PLIB_I2C_SlaveAddress10BitWasDetected ( MY_I2C_ID ) )
    {
        i2cReadData = PLIB_I2C_ReceivedByteGet ( MY_I2C_ID );
    }
    </code>

  Remarks:
    This function should only be used by slave receivers.

	This feature may not be available on all devices. Please refer to the
	specific device data sheet to determine availability or use
	PLIB_I2C_ExistsSlaveAddressDetect in your application to determine whether
	this feature is available.
*/

bool PLIB_I2C_SlaveAddress10BitWasDetected ( I2C_MODULE_ID index );


//******************************************************************************
/* Function:
    bool PLIB_I2C_SlaveAddressIsGeneralCall ( I2C_MODULE_ID index )

  Summary:
    Identifies if the current slave address received is the general call address.

  Description:
    This function identifies if the current slave address received is the general
    call address.

  Precondition:
    A slave address must have been received.

  Parameters:
    index          - Identifies the desired I2C module

  Returns:
    - true  - If the slave address received is the general call address
    - false - if the slave address received is not the general call address

  Example:
    <code>
	#define MY_I2C_ID I2C_ID_1
	uint8_t  slaveAddress7Bit;

    if ( !PLIB_I2C_SlaveAddressModeIs10Bits ( MY_I2C_ID ) )
    {
        if ( PLIB_I2C_SlaveAddressIsGeneralCall( MY_I2C_ID ) )
        {
            // Handle general call address
        }

        slaveAddress7Bit = PLIB_I2C_SlaveAddress7BitGet ( MY_I2C_ID );
    }
    </code>

  Remarks:
    This bit is set when the general call address has been received.

    This bit is automatically cleared by hardware when a Stop condition occurs
    and cannot be cleared by software.

    The MY_I2C_ID macro in the example above is used as a placeholder for the
    actual I2C bus ID (as defined by the processor-specific I2C_MODULE_ID enum).

	This feature may not be available on all devices. Please refer to the
	specific device data sheet to determine availability or use
	PLIB_I2C_ExistsGeneralCallAddressDetect in your application to determine
	whether this feature is available.
*/

bool PLIB_I2C_SlaveAddressIsGeneralCall ( I2C_MODULE_ID index );


// *****************************************************************************
// *****************************************************************************
// Section: Slave Mask Functions
// *****************************************************************************
// *****************************************************************************

//******************************************************************************
/* Function:
    void PLIB_I2C_SlaveMask7BitSet ( I2C_MODULE_ID index, uint8_t mask )

  Summary:
    Sets the 7-bit mask for Slave mode addressing .

  Description:
    This function sets the 7-bit mask for Slave mode addressing.  The address
    mask (if supported) is used, along with the slave address to identify to
    which addresses the module will respond when operating in Slave mode. It
    acts as an "ignore" mask, allowing the module to ignore bits within the
    slave address and will respond to multiple slave addresses on
    microcontrollers with I2C modules that support the mask feature.

  Precondition:
    None.

  Parameters:
    index      - Identifies the desired I2C module

    mask    - This parameter identifies bits in the address that are "don't
              care" bits.  These bits will be ignored when attempting to match
              the address, effectively allowing the module to recognize
              multiple slave addresses.  (To match an address exactly, this
              parameter must be zero (0).)

  Returns:
    None.

  Example:
    <code>
    #define MY_I2C_ID 					I2C_ID_1
    #define MY_SLAVE_ADDR_7_BIT 		0x23
    #define MY_SLAVE_ADDR_MASK_7_BIT 	0x12

    PLIB_I2C_SlaveAddress7BitSet ( MY_I2C_ID, MY_SLAVE_ADDR_7_BIT );
    PLIB_I2C_SlaveMask7BitSet ( MY_I2C_ID, MY_SLAVE_ADDR_MASK_7_BIT );
    </code>

  Remarks:
    I2C modules on some microcontroller families may not support the mask
    feature, in which case the PLIB_I2C_SlaveMask7BitSet function will not be
    supported.  Refer to the specific device data sheet to determine whether
    this feature is supported for your device.

    The MY_SLAVE_ADDRESS_7_BIT and MY_SLAVE_ADDRESS_MASK_7_BIT macros in the
    example code are used as placeholders for the actual desired slave address
    and mask that must be filled in by the caller.

    The MY_I2C_ID macro in the example above is used as a placeholder for the
    actual I2C bus ID (as defined by the processor-specific I2C_MODULE_ID enum).

	This feature may not be available on all devices. Please refer to the specific
	device data sheet to determine availability or use PLIB_I2C_ExistsSlaveMask
	in your application to determine whether this feature is available.
*/

void PLIB_I2C_SlaveMask7BitSet ( I2C_MODULE_ID index, uint8_t mask );


//******************************************************************************
/* Function:
    uint8_t PLIB_I2C_SlaveMask7BitGet ( I2C_MODULE_ID index )

  Summary:
    Identifies the current 7-bit Slave mode address mask.

  Description:
    This function identifies the 7-bit Slave mode address mask that is currently
    being used to determine which slave addresses the module will respond.

  Precondition:
    The address mask provided may not be valid if it has not been previously
    been set.

  Parameters:
    index          - Identifies the desired I2C module

  Returns:
    The 7-bit Slave mode address mask that the module is currently using to
    determine to which addresses the module will respond.

  Example:
    <code>
    #define MY_I2C_ID I2C_ID_1
    uint8_t slaveAddr;
    uint8_t slaveAddressMsk;

    if ( !PLIB_I2C_SlaveAddressModeIs10Bits ( MY_I2C_ID ) )
    {
        slaveAddr     = PLIB_I2C_SlaveAddress7BitGet ( MY_I2C_ID );
        slaveAddressMsk = PLIB_I2C_SlaveMask7BitGet ( MY_I2C_ID );
    }
    </code>

  Remarks:
    The 7-bit address mask will be right-aligned in the 8-bit return value.

    This function is not supported on microcontrollers with I2C modules that do
    not support the slave address mask feature.  Refer to the specific device data
    sheet to determine whether this feature is supported for your device.

    The MY_I2C_ID macro in the example above is used as a placeholder for the
    actual I2C bus ID (as defined by the processor-specific I2C_MODULE_ID enum).

	This feature may not be available on all devices. Please refer to the specific
	device data sheet to determine availability or use PLIB_I2C_ExistsSlaveMask
	in your application to determine whether this feature is available.
*/

uint8_t PLIB_I2C_SlaveMask7BitGet ( I2C_MODULE_ID index );


//******************************************************************************
/* Function:
    void PLIB_I2C_SlaveMask10BitSet ( I2C_MODULE_ID index, uint16_t mask )

  Summary:
    Sets the 10-bit mask for Slave mode addressing.

  Description:
    This function sets the 10-bit mask for Slave mode addressing.  The address
    mask (if supported) is used, along with the slave address to identify to
    which addresses the module will respond when operating in Slave mode.  It
    acts as an "ignore" mask, allowing the module to ignore bits within the
    slave address and thus respond to multiple slave addresses on
    microcontrollers with I2C modules that support the mask feature.

  Precondition:
    None

  Parameters:
    index      - Identifies the desired I2C module

    mask    - This parameter identifies bits in the address that are "don't
              care" bits.  These bits will be ignored when attempting to match
              the address, effectively allowing the module to recognize
              multiple slave addresses.  (To match an address exactly, this
              value must be zero (0).)  (This value must also be right-
              aligned in the 16-bit parameter.)

  Returns:
    None.

  Example:
    <code>
    #define MY_I2C_ID 						I2C_ID_1
    #define MY_SLAVE_ADDR_10_BIT 			0x23
    #define MY_SLAVE_ADDR_MASK_10_BIT		0x12

   	PLIB_I2C_SlaveAddress10BitSet ( MY_I2C_ID, MY_SLAVE_ADDR_10_BIT );
    PLIB_I2C_SlaveMask10BitSet ( MY_I2C_ID, MY_SLAVE_ADDR_MASK_10_BIT );
    </code>

  Remarks:
    I2C modules on some microcontroller families may not support the mask
    feature, in which case the PLIB_I2C_SlaveMask10BitSet function will not be
    supported.  Refer to the specific device data sheet to determine whether
    this feature is supported for your device.

    The MY_SLAVE_ADDRESS_10_BIT and MY_SLAVE_ADDRESS_MASK_10_BIT macros in the
    example code are used as placeholders for the actual desired slave address
    and mask that must be filled in by the caller.

    The MY_I2C_ID macro in the example above is used as a placeholder for the
    actual I2C bus ID (as defined by the processor-specific I2C_MODULE_ID enum).

	This feature may not be available on all devices. Please refer to the
	specific device data sheet to determine availability or use
	PLIB_I2C_ExistsSlaveMask in your application to determine whether this
	feature is available.
*/

void PLIB_I2C_SlaveMask10BitSet ( I2C_MODULE_ID index, uint16_t mask );


//******************************************************************************
/* Function:
    uint16_t PLIB_I2C_SlaveMask10BitGet ( I2C_MODULE_ID index )

  Summary:
    Identifies the current 10-bit Slave mode address mask.

  Description:
    This function identifies the 10-bit slave address mask that is currently
    being used to determine which slave addresses the module will respond.

  Precondition:
    The address mask provided may not be valid if it has not been previously
    been set.

  Parameters:
    index          - Identifies the desired I2C module

  Returns:
    The 10-bit slave address mask that the module is currently using to
    determine to which addresses the module will respond.

  Example:
    <code>
    #define MY_I2C_ID I2C_ID_1

    uint16_t slave_address;
    uint16_t slave_addressMask;

    if ( PLIB_I2C_SlaveAddressModeIs10Bits ( MY_I2C_ID ) )
    {
        slave_address     = PLIB_I2C_SlaveAddress10BitGet ( MY_I2C_ID );
        slave_addressMask = PLIB_I2C_SlaveMask10BitGet ( MY_I2C_ID );
    }
    </code>

  Remarks:
    The 10-bit address mask will be Right-aligned in the 16-bit return value.

    This function is not supported on microcontrollers with I2C modules that do
    not support the slave address mask feature. Refer to the specific device
    data sheet to determine whether this feature is supported for your device.

    The MY_I2C_ID macro in the example above is used as a placeholder for the
    actual I2C bus ID (as defined by the processor-specific I2C_MODULE_ID enum).

	This feature may not be available on all devices. Please refer to the specific
	device data sheet to determine availability or use PLIB_I2C_ExistsSlaveMask
	in your application to determine whether this feature is available.
*/

uint16_t PLIB_I2C_SlaveMask10BitGet ( I2C_MODULE_ID index );


// *****************************************************************************
// *****************************************************************************
// Section: Slave Control Functions
// *****************************************************************************
// *****************************************************************************

//******************************************************************************
/* Function:
    bool PLIB_I2C_SlaveReadIsRequested ( I2C_MODULE_ID index )

  Summary:
    Detects if the request from the master was a read or write.

  Description:
    This function identifies if a slave read (transmit) or a slave write
    (receive) was requested by the master that addressed the module.

  Precondition:
    The module must have been configured appropriately and enabled, and a
    transfer must have been previously started.

  Parameters:
    index          - Identifies the desired I2C module

  Returns:
    - true  - If an external master is requesting data (slave read/transmit)
    - false - If an external master is sending data (slave write/receive)

  Example:
    <code>
    #define MY_I2C_ID I2C_ID_1
	uint8_t slaveTxData;
	uint8_t slaveRxData;

    if ( PLIB_I2C_SlaveReadIsRequested ( MY_I2C_ID ) )
    {
        if ( PLIB_I2C_TransmitterIsReady ( MY_I2C_ID ) )
        {
            PLIB_I2C_TransmitterByteSend ( MY_I2C_ID, slaveTxData );
        }
    }
    else
    {
        slaveRxData = PLIB_I2C_ReceivedByteGet ( MY_I2C_ID );
    }
    </code>

  Remarks:
    The MY_I2C_ID macro in the example above is used as a placeholder for the
    actual I2C bus ID (as defined by the processor-specific I2C_MODULE_ID enum).

	This feature may not be available on all devices. Please refer to the
	specific device data sheet to determine availability or use
	PLIB_I2C_ExistsSlaveReadRequest in your application to determine whether
	this feature is available.
*/

bool PLIB_I2C_SlaveReadIsRequested ( I2C_MODULE_ID index );


//******************************************************************************
/* Function:
    bool PLIB_I2C_SlaveDataIsDetected ( I2C_MODULE_ID index )

  Summary:
    Detects if the most recent byte received is a data or an address byte.

  Description:
    This function identifies if the most recent byte received was a data byte or
    an address byte.

  Precondition:
    The module must have been configured appropriately and enabled, and a
    transfer must have been previously started.

  Parameters:
    index          - Identifies the desired I2C module.

  Returns:
    - true  - If the byte received is a data byte
    - false - If the byte received is an address byte

  Example:
    <code>
    #define MY_I2C_ID I2C_ID_1
    uint8_t dataReceived;

    if ( PLIB_I2C_ReceivedByteIsAvailable ( MY_I2C_ID ) )
    {
        if ( PLIB_I2C_SlaveDataIsDetected( MY_I2C_ID ) )
        {
            // Read the data and release the bus
            dataReceived = PLIB_I2C_ReceivedByteGet ( MY_I2C_ID );
            PLIB_I2C_SlaveClockRelease ( MY_I2C_ID );
        }
    }
    </code>

  Remarks:
    This function should only be used by slave receivers.

    This function returns the opposite of the PLIB_I2C_SlaveAddressIsDetected
    function.

    The MY_I2C_ID macro in the example above is used as a placeholder for the
    actual I2C bus ID (as defined by the processor-specific I2C_MODULE_ID enum).

	This feature may not be available on all devices. Please refer to the
	specific device data sheet to determine availability or use
	PLIB_I2C_ExistsSlaveDataDetect in your application to determine whether
	this feature is available.
*/

bool PLIB_I2C_SlaveDataIsDetected ( I2C_MODULE_ID index );


//******************************************************************************
/* Function:
    void PLIB_I2C_SlaveClockHold ( I2C_MODULE_ID index )

  Summary:
    Holds the SCL clock line low after receiving a byte.

  Description:
    This function allows an I2C slave to stretch the SCL clock line,
    holding it low to throttle data transfer from a master transmitter.

  Precondition:
    The module must have been configured appropriately and enabled, and a
    transfer must have been previously started by an external master.

  Parameters:
    index          - Identifies the desired I2C module

  Returns:
    None.

  Example:
    <code>
    #define MY_I2C_ID I2C_ID_1

    PLIB_I2C_SlaveClockHold ( MY_I2C_ID );
    </code>

  Remarks:
    This function will cause the SCL line to be forced low, after the current
    byte has been fully received.

    The MY_I2C_ID macro in the example above is used as a placeholder for the
    actual I2C bus ID (as defined by the processor-specific I2C_MODULE_ID enum).

	This feature may not be available on all devices. Please refer to the
	specific device data sheet to determine availability or use
	PLIB_I2C_ExistsSlaveClockHold in your application to determine whether this
	feature is available.
*/

void PLIB_I2C_SlaveClockHold ( I2C_MODULE_ID index );


//******************************************************************************
/* Function:
    void PLIB_I2C_SlaveClockRelease ( I2C_MODULE_ID index )

  Summary:
    Releases a previously held SCL clock line.

  Description:
    This function allows a slave receiver to release a previously held SCL clock
    line, allowing it to go high and allowing data transfer to continue.

  Precondition:
    The module must have been configured appropriately and enabled, and a
    transfer must have been previously started by an external master, and the
    SCL clock line should have been previously held (forced low) by the I2C
    module.

  Parameters:
    index          - Identifies the desired I2C module

  Returns:
    None.

  Example:
    <code>
    #define MY_I2C_ID I2C_ID_1

    PLIB_I2C_SlaveClockRelease ( MY_I2C_ID );
    </code>

  Remarks:
    Calling this function when the clock has not been held will not cause any
    problems.

    The MY_I2C_ID macro in the example above is used as a placeholder for the
    actual I2C bus ID (as defined by the processor-specific I2C_MODULE_ID enum).

	This feature may not be available on all devices. Please refer to the
	specific device data sheet to determine availability or use
	PLIB_I2C_ExistsSlaveClockHold in your application to determine whether this
	feature is available.
*/

void PLIB_I2C_SlaveClockRelease ( I2C_MODULE_ID index );


// *****************************************************************************
// *****************************************************************************
// Section: Master Control Functions
// *****************************************************************************
// *****************************************************************************

//******************************************************************************
/* Function:
    void PLIB_I2C_MasterStart ( I2C_MODULE_ID index )

  Summary:
    Sends an I2C Start condition on the I2C bus in Master mode.

  Description:
    This function sends the start signal (a falling edge on SDA while SCL is
    high) to start a transfer on the I2C bus when acting in Master mode.

  Precondition:
    The module must have been configured appropriately and enabled and the I2C
    bus must currently be idle.

  Parameters:
    index          - Identifies the desired I2C module

  Returns:
    None.

  Example:
    <code>
    #define MY_I2C_ID I2C_ID_1

    if ( PLIB_I2C_BusIsIdle ( MY_I2C_ID ) )
    {
        PLIB_I2C_MasterStart ( MY_I2C_ID );
    }
    </code>

  Remarks:
    Only an I2C master can start a transfer on the bus.  The bus is considered
    to be busy after a Start condition.

    After the Start condition has completed (detected by responding to the I2C
    master interrupt), software should check for arbitration loss by calling
    the PLIB_I2C_ArbitrationLossHasOccurred function.

    The MY_I2C_ID macro in the example above is used as a placeholder for the
    actual I2C bus ID (as defined by the processor-specific I2C_MODULE_ID enum).

	This feature may not be available on all devices. Please refer to the
	specific device data sheet to determine availability or use
	PLIB_I2C_ExistsMasterStart in your application to determine whether
	this feature is available.
*/

void PLIB_I2C_MasterStart( I2C_MODULE_ID index );


//******************************************************************************
/* Function:
    void PLIB_I2C_MasterStartRepeat ( I2C_MODULE_ID index )

  Summary:
    Sends a repeated Start condition during an ongoing transfer in Master mode.

  Description:
    This function supports sending a repeated Start condition to change slave
    targets or transfer direction to support certain I2C transfer formats in
    Master mode.

  Precondition:
    The module must have been configured appropriately and enabled, and a
    transfer must have been previously started.

  Parameters:
    index       - Identifies the desired I2C module

  Returns:
    None.

  Example:
    <code>
    #define MY_I2C_ID I2C_ID_1

    PLIB_I2C_MasterStartRepeat ( MY_I2C_ID );
    </code>

  Remarks:
    Only an I2C master that has already started a transfer can send a
    repeated Start condition.

    After the repeated-start condition has completed (detected by responding to
    the I2C master interrupt), software should check for arbitration loss by the
    PLIB_I2C_ArbitrationLossHasOccurred function.

    The MY_I2C_ID macro in the example above is used as a placeholder for the
    actual I2C bus ID (as defined by the processor-specific I2C_MODULE_ID enum).

	This feature may not be available on all devices. Please refer to the
	specific device data sheet to determine availability or use
	PLIB_I2C_ExistsMasterStartRepeat in your application to determine whether
	this feature is available.
*/

void PLIB_I2C_MasterStartRepeat ( I2C_MODULE_ID index );


//******************************************************************************
/* Function:
    void PLIB_I2C_MasterReceiverClock1Byte ( I2C_MODULE_ID index )

  Summary:
    Drives the bus clock to receive 1 byte of data from a slave device.

  Description:
    This function drives the bus clock to receive 1 byte of data from a slave
    device.

  Precondition:
    The module must have been configured appropriately and enabled, a transfer
    must have been previously started, and the module must be the intended
    receiver of the next byte of data.

  Parameters:
    index          - Identifies the desired I2C module

  Returns:
    None.

  Example:
    <code>
    #define MY_I2C_ID I2C_ID_1

    PLIB_I2C_MasterReceiverClock1Byte ( MY_I2C_ID );
    </code>

  Remarks:
    The module stops driving the bus clock after the reception of a
    single byte of data and this function must be called again to receive
    another byte.

    After the module has finished receiving a data byte (determined by
    responding to an I2C master interrupt and/or by checking the
    PLIB_I2C_ReceivedByteIsAvailable function), software should check the
    PLIB_I2C_ReceiverOverflowHasOccurred function to ensure that no data was lost
    because the receiver buffer was full when the byte was completely received
    and ready to be placed into the receiver buffer.

    The MY_I2C_ID macro in the example above is used as a placeholder for the
    actual I2C bus ID (as defined by the processor-specific I2C_MODULE_ID enum).

	This feature may not be available on all devices. Please refer to the
	specific device data sheet to determine availability or use
	PLIB_I2C_ExistsMasterReceiverClock1Byte in your application to determine
	whether this feature is available.
*/

void PLIB_I2C_MasterReceiverClock1Byte ( I2C_MODULE_ID index );


//******************************************************************************
/* Function:
    void PLIB_I2C_MasterStop ( I2C_MODULE_ID index )

  Summary:
    Sends an I2C Stop condition to terminate a transfer in Master mode.

  Description:
    This function sends the stop signal (a rising edge on SDA while SCL is
    high) on the I2C bus, to end a transfer on the I2C bus when in Master mode.

  Precondition:
    The module must have been configured appropriately, enabled, and a previously
    started transfer must be completed.

  Parameters:
    index          - Identifies the desired I2C module

  Returns:
    None.

  Example:
    <code>
    #define MY_I2C_ID I2C_ID_1

    PLIB_I2C_MasterStop ( MY_I2C_ID );
    </code>

  Remarks:
    Only an I2C master that has already started a transfer can send a
    Stop condition.

    The MY_I2C_ID macro in the example above is used as a placeholder for the
    actual I2C bus ID (as defined by the processor-specific I2C_MODULE_ID enum).

	This feature may not be available on all devices. Please refer to the specific
	device data sheet to determine availability or use PLIB_I2C_ExistsMasterStop
	in your application to determine whether this feature is available.
*/

void PLIB_I2C_MasterStop ( I2C_MODULE_ID index );


// *****************************************************************************
// *****************************************************************************
// Section: Transmitter Control Functions
// *****************************************************************************
// *****************************************************************************

//******************************************************************************
/* Function:
    bool PLIB_I2C_TransmitterIsBusy ( I2C_MODULE_ID index )

  Summary:
    Identifies if the transmitter of the specified I2C module is currently busy
    (unable to accept more data).

  Description:
    This function identifies if the transmitter of the specified I2C module is
    currently busy (unable to accept more data).

  Precondition:
    None.

  Parameters:
    index          - Identifies the desired I2C module

  Returns:
    - true  - The transmitter is busy (unable to accept new data)
    - false - The transmitter is ready (able to accept new data)

  Example:
    <code>
    #define MY_I2C_ID I2C_ID_1
	uint8_t transData;

	//set 'transData' variable

    if ( !PLIB_I2C_TransmitterIsBusy( MY_I2C_ID ) )
    {
        PLIB_I2C_TransmitterByteSend( MY_I2C_ID, transData );
    }
    </code>

  Remarks:
    This function returns the inverse of the PLIB_I2C_TransmitterIsReady
    function.

    This flag is cleared automatically by the hardware when the transmitter
    is ready. It cannot be cleared by software.

    The MY_I2C_ID macro in the example above is used as a placeholder for the
    actual I2C bus ID (as defined by the processor-specific I2C_MODULE_ID enum).

	This feature may not be available on all devices. Please refer to the
	specific device data sheet to determine availability or use
	PLIB_I2C_ExistsTransmitterIsBusy in your application to determine whether
	this feature is available.
*/

bool PLIB_I2C_TransmitterIsBusy ( I2C_MODULE_ID index );


//******************************************************************************
/* Function:
    bool PLIB_I2C_TransmitterIsReady ( I2C_MODULE_ID index )

  Summary:
    Detects if the transmitter is ready to accept data to transmit.

  Description:
    This function determines if the transmitter is ready to accept more data to
    be transmitted on the I2C bus.

  Precondition:
    The module must have been configured appropriately, enabled, and a
    transfer must have been previously started.

  Parameters:
    index          - Identifies the desired I2C module

  Returns:
    - true  - If the transmitter is ready to accept more data
    - false - If the transmitter is not ready to accept more data

  Example:
    <code>
	#define MY_I2C_ID I2C_ID_1
	uint8_t sendData;

	//set 'sendData' variable

    if ( PLIB_I2C_TransmitterIsReady ( MY_I2C_ID ) )
    {
        PLIB_I2C_TransmitterByteSend ( MY_I2C_ID, sendData );
    }
    </code>

  Remarks:
    This function should be used by both master and slave transmitters.

    This function returns the inverse of the PLIB_I2C_TransmitterIsBusy
    function.

    The MY_I2C_ID macro in the example above is used as a placeholder for the
    actual I2C bus ID (as defined by the processor-specific I2C_MODULE_ID enum).

	This feature may not be available on all devices. Please refer to the
	specific device data sheet to determine availability or use
	PLIB_I2C_ExistsTransmitterIsBusy in your application to determine whether
	this feature is available.
*/

bool PLIB_I2C_TransmitterIsReady ( I2C_MODULE_ID index );


//******************************************************************************
/* Function:
    void PLIB_I2C_TransmitterByteSend ( I2C_MODULE_ID index, uint8_t data )

  Summary:
    Sends a byte of data on the I2C bus.

  Description:
    This function allows the caller to send a byte of data on the I2C bus.

  Precondition:
    The module must have been configured appropriately and enabled, a transfer
    must have been previously started, and the I2C module's transmitter must be
    ready to accept a byte of data to send.

  Parameters:
    index          - Identifies the desired I2C module

    data        - Data byte to send on the I2C bus

  Returns:
    None.

  Example:
    <code>
	#define MY_I2C_ID I2C_ID_1
	uint8_t transmitData;

	//set 'transmitData' variable

    if ( PLIB_I2C_TransmitterIsReady ( MY_I2C_ID ) )
    {
        PLIB_I2C_TransmitterByteSend ( MY_I2C_ID, transmitData );
    }
    </code>

  Remarks:
    This function should be used by both master and slave transmitters.

    The caller must either first call the PLIB_I2C_TransmitterIsReady function
    before calling this function to ensure that the transmitter is ready to
    receive a new byte to transmit or call the
    PLIB_I2C_TransmitterOverflowHasOccurred function immediately after calling
    this function to ensure that a transmitter write collision did not occur.

    The MY_I2C_ID macro in the example above is used as a placeholder for the
    actual I2C bus ID (as defined by the processor-specific I2C_MODULE_ID enum).

	This feature may not be available on all devices. Please refer to the
	specific device data sheet to determine availability or use
	PLIB_I2C_ExistsTransmitterByteSend in your application to determine whether
	this feature is available.
*/

void PLIB_I2C_TransmitterByteSend ( I2C_MODULE_ID index, uint8_t data );


//******************************************************************************
/* Function:
    bool PLIB_I2C_TransmitterByteHasCompleted ( I2C_MODULE_ID index )

  Summary:
    Detects whether the module has finished transmitting the most recent
    byte.

  Description:
    This function determines if the transmitter has finished sending the most
    recently sent byte on the I2C bus.

  Precondition:
    The module must have been configured appropriately and enabled, a transfer
    must have been previously started, and a data or address byte must have
    been sent.

  Parameters:
    index          - Identifies the desired I2C module

  Returns:
    - true  - If the transmitter has completed sending the data byte
    - false - If the transmitter is still busy sending the data byte

  Example:
    <code>
    #define MY_I2C_ID I2C_ID_1

    if ( PLIB_I2C_TransmitterByteHasCompleted ( MY_I2C_ID ) )
    {
      //Transmission completed
    }
    </code>

  Remarks:
    This function should be used by both master and slave transmitters.

    The MY_I2C_ID macro in the example above is used as a placeholder for the
    actual I2C bus ID (as defined by the processor-specific I2C_MODULE_ID enum).

	This feature may not be available on all devices. Please refer to the
	specific device data sheet to determine availability or use
	PLIB_I2C_ExistsTransmitterByteComplete in your application to determine
	whether this feature is available.
*/

bool PLIB_I2C_TransmitterByteHasCompleted ( I2C_MODULE_ID index );


//******************************************************************************
/* Function:
    bool PLIB_I2C_TransmitterByteWasAcknowledged ( I2C_MODULE_ID index )

  Summary:
    Determines whether the most recently sent byte was acknowledged.

  Description:
    This function allows a transmitter to determine if the byte just sent was
    positively acknowledged (ACKed) or negatively acknowledged (NAKed) by the
    receiver.

  Precondition:
    The module must have been configured appropriately and enabled, a transfer
    must have been previously started, a byte of data must have been sent
    on the I2C bus, and the transmission must have completed.

  Parameters:
    index          - Identifies the desired I2C module

  Returns:
    - true  - If the receiver ACKed the byte
    - false - If the receiver NAKed the byte

  Example:
    <code>
    #define MY_I2C_ID I2C_ID_1

    if ( PLIB_I2C_TransmitterByteHasCompleted ( MY_I2C_ID ) )
    {
        if ( PLIB_I2C_TransmitterByteWasAcknowledged ( MY_I2C_ID ) )
        {
            // transmission successful
        }
    }
    </code>

  Remarks:
    This function can be used by both master or slave transmitters.

    The MY_I2C_ID macro in the example above is used as a placeholder for the
    actual I2C bus ID (as defined by the processor-specific I2C_MODULE_ID enum).

	This feature may not be available on all devices. Please refer to the
	specific device data sheet to determine availability or use
	PLIB_I2C_ExistsTransmitterByteAcknowledge in your application to determine
	whether this feature is available.
*/

bool PLIB_I2C_TransmitterByteWasAcknowledged ( I2C_MODULE_ID index );


//******************************************************************************
/* Function:
    bool PLIB_I2C_TransmitterOverflowHasOccurred ( I2C_MODULE_ID index )

  Summary:
    Identifies if a transmitter overflow error has occurred.

  Description:
    This function identifies if a transmitter overflow error has occurred.

  Precondition:
    None.

  Parameters:
    index          - Identifies the desired I2C module

  Returns:
    - true  - If software attempted to write a byte to the transmitter
              buffer while the transmitter was busy and unable to accept a
              new byte (i.e., the write will not occur)

    - false - If no transmitter overflow occurred when software attempted
              to write a byte to the transmitter buffer (i.e., the write
              occurred successfully)

  Example:
    <code>
    #define MY_I2C_ID I2C_ID_1
    uint8_t txData;

	//set 'txData' variable

    PLIB_I2C_TransmitterByteSend ( MY_I2C_ID, txData );
    if ( PLIB_I2C_TransmitterOverflowHasOccurred ( MY_I2C_ID ) )
    {
        // Handle overflow error

        PLIB_I2C_TransmitterOverflowClear ( MY_I2C_ID );
    }
    </code>

  Remarks:
    This flag must be cleared be cleared by software using the
    PLIB_I2C_TransmitterOverflowClear function.

    The MY_I2C_ID macro in the example above is used as a placeholder for the
    actual I2C bus ID (as defined by the processor-specific I2C_MODULE_ID enum).

	This feature may not be available on all devices. Please refer to the
	specific device data sheet to determine availability or use
	PLIB_I2C_ExistsTransmitterOverflow in your application to determine whether
	this feature is available.
*/

bool PLIB_I2C_TransmitterOverflowHasOccurred ( I2C_MODULE_ID index );


/*************************************************************************
  Function:
       void PLIB_I2C_TransmitterOverflowClear ( I2C_MODULE_ID index )

  Summary:
    Clears the transmitter overflow status flag.
  Description:
    This function clears the transmitter overflow status flag.
  Conditions:
    None.
  Input:
    index -  Identifies the desired I2C module
  Return:
    None.
  Example:
    <code>
    #define MY_I2C_ID I2C_ID_1
    uint8_t my_data;

    //set 'my_data' variable
    PLIB_I2C_TransmitterByteSend ( MY_I2C_ID, my_data );
    if ( PLIB_I2C_TransmitterOverflowHasOccurred ( MY_I2C_ID ) )
    {
        // Handle overflow error

        PLIB_I2C_TransmitterOverflowClear ( MY_I2C_ID );
    }
    </code>
  Remarks:
    This flag is set by hardware when an overflow error occurs. Its status
    can be tested using the PLIB_I2C_TransmitterOverflowHasOccurred
    function. This flag must be cleared be cleared by software after the
    overflow error has been handled. To handle the error, software must
    retry the write later after the PLIB_I2C_TransmitterByteSend function
    returns TRUE.

    The MY_I2C_ID macro in the example above is used as a placeholder for
    the actual I2C bus ID (as defined by the processor-specific
    I2C_MODULE_ID enum).

    This feature may not be available on all devices. Please refer to the
    specific device data sheet to determine availability or use
    PLIB_I2C_ExistsTransmitterOverflow in your application to determine
    whether this feature is available.
  *************************************************************************/

void PLIB_I2C_TransmitterOverflowClear ( I2C_MODULE_ID index );


//******************************************************************************
/* Function:
    bool PLIB_I2C_AcksequenceIsInProgress ( I2C_MODULE_ID index )

  Summary:
    Checks whether the acknowledge sequence is in progress or it is completed.

  Description:
    This function checks whether the acknowledge sequence is in progress or it
    is completed.

  Precondition:
    The module must have been configured appropriately and enabled.

  Parameters:
    index          - Identifies the desired I2C module

  Returns:
    - true  - Acknowledge sequence is in progress.
    - false - Acknowledge sequence is not started or completed.

  Example:
    <code>
    #define MY_I2C_ID I2C_ID_1

    if ( PLIB_I2C_AcksequenceIsInProgress ( MY_I2C_ID ) )
    {
      //Transmission completed
    }
    </code>

  Remarks:

    The MY_I2C_ID macro in the example above is used as a placeholder for the
    actual I2C bus ID (as defined by the processor-specific I2C_MODULE_ID enum).

	This feature may not be available on all devices. Please refer to the
	specific device data sheet to determine availability or use
	PLIB_I2C_ExistsAcksequenceProgress in your application to determine
	whether this feature is available.
*/

bool PLIB_I2C_AcksequenceIsInProgress ( I2C_MODULE_ID index );


// *****************************************************************************
// *****************************************************************************
// Section: Receiver Control
// *****************************************************************************
// *****************************************************************************

//******************************************************************************
/* Function:
    bool PLIB_I2C_ReceivedByteIsAvailable ( I2C_MODULE_ID index )

  Summary:
    Detects whether the receiver has data available.

  Description:
    This function determines if the receiver has data available to be read by
    software.

  Precondition:
    The I2C module must have been configured appropriately and enabled, and a
    transfer must have been previously started.

  Parameters:
    index          - Identifies the desired I2C module

  Returns:
    - true  - If the receiver has data available
    - false - If the receiver does not have data available

  Example:
    <code>
    #define MY_I2C_ID I2C_ID_1
    uint8_t readData;

    if ( PLIB_I2C_ReceivedByteIsAvailable ( MY_I2C_ID ) )
    {
        readData = PLIB_I2C_ReceivedByteGet ( MY_I2C_ID );
    }
    </code>

  Remarks:
    This function should be used by both master and slave receivers.

    The MY_I2C_ID macro in the example above is used as a placeholder for the
    actual I2C bus ID (as defined by the processor-specific I2C_MODULE_ID enum).

	This feature may not be available on all devices. Please refer to the
	specific device data sheet to determine availability or use
	PLIB_I2C_ExistsReceivedByteAvailable in your application to determine whether
	this feature is available.
*/

bool PLIB_I2C_ReceivedByteIsAvailable ( I2C_MODULE_ID index );


//******************************************************************************
/* Function:
    uint8_t PLIB_I2C_ReceivedByteGet ( I2C_MODULE_ID index )

  Summary:
    Gets a byte of data received from the I2C bus interface.

  Description:
    This function allows the caller to read a byte of data received from the I2C
    bus.

  Precondition:
    The module must have been configured appropriately and enabled, a transfer
    must have been previously started, and a byte of data must have been
    received from the I2C bus.

  Parameters:
    index          - Identifies the desired I2C module

  Returns:
    A byte of data received from the I2C bus.

  Example:
    <code>
	#define MY_I2C_ID I2C_ID_1
	uint8_t receivedData;

    if ( PLIB_I2C_ReceivedByteIsAvailable ( MY_I2C_ID ) )
    {
        PLIB_I2C_ReceivedByteAcknowledge ( MY_I2C_ID, true );
        receivedData = PLIB_I2C_ReceivedByteGet ( MY_I2C_ID );
    }
    </code>

  Remarks:
    This function should be used by both master and slave receivers.

    The MY_I2C_ID macro in the example above is used as a placeholder for the
    actual I2C bus ID (as defined by the processor-specific I2C_MODULE_ID enum).

	This feature may not be available on all devices. Please refer to the
	specific device data sheet to determine availability or use
	PLIB_I2C_ExistsReceivedByteGet in your application to determine whether
	this feature is available.
*/

uint8_t PLIB_I2C_ReceivedByteGet ( I2C_MODULE_ID index );


//******************************************************************************
/* Function:
    bool PLIB_I2C_MasterReceiverReadyToAcknowledge ( I2C_MODULE_ID index )

  Summary:
    Checks whether the hardware is ready to acknowledge.

  Description:
    This function checks for preconditions before acknowledging a slave.

  Precondition:
    The module must have been configured appropriately and enabled, a transfer
    must have been previously started and a byte of data must have been
    received from the I2C bus.

  Parameters:
    index      - Identifies the desired I2C module

  Returns:
      - true  - If the hardware status is ready to acknowledge
      - false - If the hardware is not ready to acknowledge


  Example:
    <code>
    #define MY_I2C_ID I2C_ID_1
    uint8_t data;

    if ( PLIB_I2C_ReceivedByteIsAvailable ( MY_I2C_ID ) )
    {
        if ( PLIB_I2C_MasterReceiverReadyToAcknowledge ( MY_I2C_ID ) )
        {
            PLIB_I2C_ReceivedByteAcknowledge ( MY_I2C_ID, true );
            data = PLIB_I2C_ReceivedByteGet ( MY_I2C_ID );
        }
    }
    </code>

  Remarks:
    This function can only be used by master receivers.

    The MY_I2C_ID macro in the example above is used as a placeholder for the
    actual I2C bus ID (as defined by the processor-specific I2C_MODULE_ID enum).

	This feature may not be available on all devices. Please refer to the
	specific device data sheet to determine availability or use
	PLIB_I2C_ExistsReceivedByteAcknowledge in your application to determine
	whether this feature is available.
*/

bool PLIB_I2C_MasterReceiverReadyToAcknowledge ( I2C_MODULE_ID index );


//******************************************************************************
/* Function:
    void PLIB_I2C_ReceivedByteAcknowledge ( I2C_MODULE_ID index, bool ack )

  Summary:
    Allows a receiver to acknowledge a that a byte of data has been received.

  Description:
    This function allows a receiver to positively acknowledge (ACK) or negatively
    acknowledge (NAK) a byte of data that has been received from the I2C bus.

  Precondition:
    The module must have been configured appropriately and enabled, a transfer
    must have been previously started, and a byte of data must have been
    received from the I2C bus.

  Parameters:
    index   - Identifies the desired I2C module
    ack     - Determines how the byte should be acknowledged:
              * If true, positively acknowledges (ACK) the byte of data received
              * If false, negatively acknowledges (NAK) the byte of data received

  Returns:
    None.

  Example:
    <code>
    #define MY_I2C_ID I2C_ID_1

    if ( PLIB_I2C_ReceivedByteIsAvailable ( MY_I2C_ID ) )
    {
        if ( PLIB_I2C_MasterReceiverReadyToAcknowledge ( MY_I2C_ID ) )
        {
            PLIB_I2C_ReceivedByteAcknowledge ( MY_I2C_ID, true );
            data = PLIB_I2C_ReceivedByteGet ( MY_I2C_ID );
        }
    }
    </code>

  Remarks:
    This function can only be used by master receivers.  Slave receivers
    automatically ACK or NAK bytes.

    The MY_I2C_ID macro in the example above is used as a placeholder for the
    actual I2C bus ID (as defined by the processor-specific I2C_MODULE_ID enum).

	This feature may not be available on all devices. Please refer to the
	specific device data sheet to determine availability or use
	PLIB_I2C_ExistsReceivedByteAcknowledge in your application to determine
	whether this feature is available.
*/

void PLIB_I2C_ReceivedByteAcknowledge ( I2C_MODULE_ID index, bool ack );


//******************************************************************************
/* Function:
    bool PLIB_I2C_ReceiverByteAcknowledgeHasCompleted ( I2C_MODULE_ID index )

  Summary:
    Determines if the previous acknowledge has completed.

  Description:
    This function allows the receiver to determine if the acknowledgment signal
    has completed.

  Precondition:
    The module must have been configured appropriately and enabled, a transfer
    must have been previously started, a byte of data must have been received
    on the I2C bus, and the acknowledgment must have been started (by calling
    the PLIB_I2C_ReceivedByteAcknowledge function).

  Parameters:
    index          - Identifies the desired I2C module

  Returns:
    - true  - If the acknowledgment has completed
    - false - If the acknowledgment has not completed

  Example:
    <code>
    PLIB_I2C_ReceivedByteAcknowledge ( MY_I2C_ID, true );

    if ( PLIB_I2C_ReceiverByteAcknowledgeHasCompleted ( MY_I2C_ID ) )
    {
        // acknowledgment completed
    }
    </code>

  Remarks:
    This function can only be used by master receivers.  Slave receivers
    automatically ACK or NAK bytes.

    The MY_I2C_ID macro in the example above is used as a placeholder for the
    actual I2C bus ID (as defined by the processor-specific I2C_MODULE_ID enum).

	This feature may not be available on all devices. Please refer to the
	specific device data sheet to determine availability or use
	PLIB_I2C_ExistsReceivedByteAcknowledge in your application to determine
	whether this feature is available.
*/

bool PLIB_I2C_ReceiverByteAcknowledgeHasCompleted ( I2C_MODULE_ID index );


//******************************************************************************
/* Function:
    bool PLIB_I2C_ReceiverOverflowHasOccurred ( I2C_MODULE_ID index )

  Summary:
    Identifies if a receiver overflow error has occurred.

  Description:
    This function identifies if a receiver overflow error has occurred.

  Precondition:
    None.

  Parameters:
    index          - Identifies the desired I2C module

  Returns:
    - true  - If a byte was received while the receiver buffer still held
    		  a previously received byte (The new byte will be lost)

    - false - If no receiver overflow occurred when a byte has been received

  Example:
    <code>
    #define MY_I2C_ID I2C_ID_1

    if ( PLIB_I2C_ReceiverOverflowHasOccurred ( MY_I2C_ID ) )
    {
        // Handle overflow error

        PLIB_I2C_ReceiverOverflowClear ( MY_I2C_ID );
    }
    </code>

  Remarks:
    This flag should be cleared be cleared by software using the
    PLIB_I2C_ReceiverOverflowClear function.

    The MY_I2C_ID macro in the example above is used as a placeholder for the
    actual I2C bus ID (as defined by the processor-specific I2C_MODULE_ID enum).

	This feature may not be available on all devices. Please refer to the
	specific device data sheet to determine availability or use
	PLIB_I2C_ExistsReceiverOverflow in your application to determine
	whether this feature is available.
*/

bool PLIB_I2C_ReceiverOverflowHasOccurred ( I2C_MODULE_ID index );


/*************************************************************************
  Function:
       void PLIB_I2C_ReceiverOverflowClear ( I2C_MODULE_ID index )

  Summary:
    Clears the receiver overflow status flag.
  Description:
    This function clears the receiver overflow status flag.
  Conditions:
    None.
  Input:
    index -  Identifies the desired I2C module
  Return:
    None.
  Example:
    <code>
    #define MY_I2C_ID I2C_ID_1

    if ( PLIB_I2C_ReceiverOverflowHasOccurred ( MY_I2C_ID ) )
    {
        // Handle overflow error

        PLIB_I2C_ReceiverOverflowClear ( MY_I2C_ID );
    }
    </code>
  Remarks:
    This flag is set by hardware when an overflow error occurs. Its status
    can be tested using the PLIB_I2C_ReceiverOverflowHasOccurred function.
    This flag should be cleared be cleared by software after the overflow
    error has been handled by reading the byte in the receiver buffer.

    The MY_I2C_ID macro in the example above is used as a placeholder for
    the actual I2C bus ID (as defined by the processor-specific
    I2C_MODULE_ID enum).

    This feature may not be available on all devices. Please refer to the
    specific device data sheet to determine availability or use
    PLIB_I2C_ExistsReceiverOverflow in your application to determine
    whether this feature is available.
  *************************************************************************/

void PLIB_I2C_ReceiverOverflowClear ( I2C_MODULE_ID index );

// *****************************************************************************
// *****************************************************************************
// Section: I2C Peripheral Library Exists API Routines
// *****************************************************************************
// *****************************************************************************
/* The following functions indicate the existence of the features on the device.
*/

//******************************************************************************
/* Function:
    PLIB_I2C_ExistsClockStretching( I2C_MODULE_ID index )

  Summary:
    Identifies whether the ClockStretching feature exists on the I2C module.

  Description:
    This function identifies whether the ClockStretching feature is available on the I2C module.
    When this function returns true, these functions are supported on the device:
    - PLIB_I2C_SlaveClockStretchingEnable
    - PLIB_I2C_SlaveClockStretchingDisable

  Preconditions:
    None.

  Parameters:
    index           - Identifier for the device instance

  Returns:
    - true  - If the ClockStretching feature is supported on the device
    - false - If the ClockStretching feature is not supported on the device

  Remarks:
    None.
*/

bool PLIB_I2C_ExistsClockStretching( I2C_MODULE_ID index );


//******************************************************************************
/* Function:
    PLIB_I2C_ExistsGeneralCall( I2C_MODULE_ID index )

  Summary:
    Identifies whether the GeneralCall feature exists on the I2C module.

  Description:
    This function identifies whether the GeneralCall feature is available on the I2C module.
    When this function returns true, these functions are supported on the device:
    - PLIB_I2C_GeneralCallEnable
    - PLIB_I2C_GeneralCallDisable

  Preconditions:
    None.

  Parameters:
    index           - Identifier for the device instance

  Returns:
    - true  - If the GeneralCall feature is supported on the device
    - false - If the GeneralCall feature is not supported on the device

  Remarks:
    None.
*/

bool PLIB_I2C_ExistsGeneralCall( I2C_MODULE_ID index );


//******************************************************************************
/* Function:
    PLIB_I2C_ExistsSMBus( I2C_MODULE_ID index )

  Summary:
    Identifies whether the SMBus feature exists on the I2C module.

  Description:
    This function identifies whether the SMBus feature is available on the I2C module.
    When this function returns true, these functions are supported on the device:
    - PLIB_I2C_SMBEnable
    - PLIB_I2C_SMBDisable

  Preconditions:
    None.

  Parameters:
    index           - Identifier for the device instance

  Returns:
    - true  - If the SMBus feature is supported on the device
    - false - If the SMBus feature is not supported on the device

  Remarks:
    None.
*/

bool PLIB_I2C_ExistsSMBus( I2C_MODULE_ID index );


//******************************************************************************
/* Function:
    PLIB_I2C_ExistsHighFrequency( I2C_MODULE_ID index )

  Summary:
    Identifies whether the HighFrequency feature exists on the I2C module.

  Description:
    This function identifies whether the HighFrequency feature is available on the I2C module.
    When this function returns true, these functions are supported on the device:
    - PLIB_I2C_HighFrequencyEnable
    - PLIB_I2C_HighFrequencyDisable

  Preconditions:
    None.

  Parameters:
    index           - Identifier for the device instance

  Returns:
    - true  - If the HighFrequency feature is supported on the device
    - false - If the HighFrequency feature is not supported on the device

  Remarks:
    None.
*/

bool PLIB_I2C_ExistsHighFrequency( I2C_MODULE_ID index );


//******************************************************************************
/* Function:
    PLIB_I2C_ExistsReservedAddressProtect( I2C_MODULE_ID index )

  Summary:
    Identifies whether the ReservedAddressProtect feature exists on the I2C module.

  Description:
    This function identifies whether the ReservedAddressProtect feature is available on the I2C module.
    When this function returns true, these functions are supported on the device:
    - PLIB_I2C_ReservedAddressProtectEnable
    - PLIB_I2C_ReservedAddressProtectDisable

  Preconditions:
    None.

  Parameters:
    index           - Identifier for the device instance

  Returns:
    - true  - If the ReservedAddressProtect feature is supported on the device
    - false - If the ReservedAddressProtect feature is not supported on the device

  Remarks:
    None.
*/

bool PLIB_I2C_ExistsReservedAddressProtect( I2C_MODULE_ID index );


//******************************************************************************
/* Function:
    PLIB_I2C_ExistsStopInIdle( I2C_MODULE_ID index )

  Summary:
    Identifies whether the StopInIdle feature exists on the I2C module.

  Description:
    This function identifies whether the StopInIdle feature is available on the I2C module.
    When this function returns true, these functions are supported on the device:
    - PLIB_I2C_StopInIdleEnable
    - PLIB_I2C_StopInIdleDisable

  Preconditions:
    None.

  Parameters:
    index           - Identifier for the device instance

  Returns:
    - true  - If the StopInIdle feature is supported on the device
    - false - If the StopInIdle feature is not supported on the device

  Remarks:
    None.
*/

bool PLIB_I2C_ExistsStopInIdle( I2C_MODULE_ID index );


//******************************************************************************
/* Function:
    PLIB_I2C_ExistsIPMI( I2C_MODULE_ID index )

  Summary:
    Identifies whether the IPMI feature exists on the I2C module.

  Description:
    This function identifies whether the IPMI feature is available on the I2C module.
    When this function returns true, these functions are supported on the device:
    - PLIB_I2C_IPMIEnable
    - PLIB_I2C_IPMIDisable

  Preconditions:
    None.

  Parameters:
    index           - Identifier for the device instance

  Returns:
    - true  - If the IPMI feature is supported on the device
    - false - If the IPMI feature is not supported on the device

  Remarks:
    None.
*/

bool PLIB_I2C_ExistsIPMI( I2C_MODULE_ID index );


//******************************************************************************
/* Function:
    PLIB_I2C_ExistsModuleEnable( I2C_MODULE_ID index )

  Summary:
    Identifies whether the ModuleEnable feature exists on the I2C module.

  Description:
    This function identifies whether the ModuleEnable feature is available on the I2C module.
    When this function returns true, these functions are supported on the device:
    - PLIB_I2C_Enable
    - PLIB_I2C_Disable

  Preconditions:
    None.

  Parameters:
    index           - Identifier for the device instance

  Returns:
    - true  - If the ModuleEnable feature is supported on the device
    - false - If the ModuleEnable feature is not supported on the device

  Remarks:
    None.
*/

bool PLIB_I2C_ExistsModuleEnable( I2C_MODULE_ID index );


//******************************************************************************
/* Function:
    PLIB_I2C_ExistsStartDetect( I2C_MODULE_ID index )

  Summary:
    Identifies whether the StartDetect feature exists on the I2C module.

  Description:
    This function identifies whether the StartDetect feature is available on the I2C module.
    When this function returns true, these functions are supported on the device:
    - PLIB_I2C_StartWasDetected
    - PLIB_I2C_StartClear

  Preconditions:
    None.

  Parameters:
    index           - Identifier for the device instance

  Returns:
    - true  - If the StartDetect feature is supported on the device
    - false - If the StartDetect feature is not supported on the device

  Remarks:
    None.
*/

bool PLIB_I2C_ExistsStartDetect( I2C_MODULE_ID index );


//******************************************************************************
/* Function:
    PLIB_I2C_ExistsStopDetect( I2C_MODULE_ID index )

  Summary:
    Identifies whether the StopDetect feature exists on the I2C module.

  Description:
    This function identifies whether the StopDetect feature is available on the I2C module.
    When this function returns true, these functions are supported on the device:
    - PLIB_I2C_StopWasDetected
    - PLIB_I2C_StopClear

  Preconditions:
    None.

  Parameters:
    index           - Identifier for the device instance

  Returns:
    - true  - If the StopDetect feature is supported on the device
    - false - If the StopDetect feature is not supported on the device

  Remarks:
    None.
*/

bool PLIB_I2C_ExistsStopDetect( I2C_MODULE_ID index );


//******************************************************************************
/* Function:
    PLIB_I2C_ExistsArbitrationLoss( I2C_MODULE_ID index )

  Summary:
    Identifies whether the ArbitrationLoss feature exists on the I2C module.

  Description:
    This function identifies whether the ArbitrationLoss feature is available on the I2C module.
    When this function returns true, these functions are supported on the device:
    - PLIB_I2C_ArbitrationLossHasOccurred
    - PLIB_I2C_ArbitrationLossClear

  Preconditions:
    None.

  Parameters:
    index           - Identifier for the device instance

  Returns:
    - true  - If the ArbitrationLoss feature is supported on the device
    - false - If the ArbitrationLoss feature is not supported on the device

  Remarks:
    None.
*/

bool PLIB_I2C_ExistsArbitrationLoss( I2C_MODULE_ID index );


//******************************************************************************
/* Function:
    PLIB_I2C_ExistsBusIsIdle( I2C_MODULE_ID index )

  Summary:
    Identifies whether the BusIdle feature exists on the I2C module.

  Description:
    This function identifies whether the BusIdle feature is available on the I2C module.
    When this function returns true, this function is supported on the device:
    - PLIB_I2C_BusIsIdle

  Preconditions:
    None.

  Parameters:
    index           - Identifier for the device instance

  Returns:
    - true  - If the BusIdle feature is supported on the device
    - false - If the BusIdle feature is not supported on the device

  Remarks:
    None.
*/

bool PLIB_I2C_ExistsBusIsIdle( I2C_MODULE_ID index );


//******************************************************************************
/* Function:
    PLIB_I2C_ExistsBaudRate( I2C_MODULE_ID index )

  Summary:
    Identifies whether the BaudRate feature exists on the I2C module.

  Description:
    This function identifies whether the BaudRate feature is available on the I2C module.
    When this function returns true, these functions are supported on the device:
    - PLIB_I2C_BaudRateSet
    - PLIB_I2C_BaudRateGet

  Preconditions:
    None.

  Parameters:
    index           - Identifier for the device instance

  Returns:
    - true  - If the BaudRate feature is supported on the device
    - false - If the BaudRate feature is not supported on the device

  Remarks:
    None.
*/

bool PLIB_I2C_ExistsBaudRate( I2C_MODULE_ID index );


//******************************************************************************
/* Function:
    PLIB_I2C_ExistsSlaveAddress7Bit( I2C_MODULE_ID index )

  Summary:
    Identifies whether the SlaveAddress7Bit feature exists on the I2C module.

  Description:
    This function identifies whether the SlaveAddress7Bit feature is available on the I2C module.
    When this function returns true, these functions are supported on the device:
    - PLIB_I2C_SlaveAddress7BitSet
    - PLIB_I2C_SlaveAddress7BitGet

  Preconditions:
    None.

  Parameters:
    index           - Identifier for the device instance

  Returns:
    - true  - If the SlaveAddress7Bit feature is supported on the device
    - false - If the SlaveAddress7Bit feature is not supported on the device

  Remarks:
    None.
*/

bool PLIB_I2C_ExistsSlaveAddress7Bit( I2C_MODULE_ID index );


//******************************************************************************
/* Function:
    PLIB_I2C_ExistsSlaveAddress10Bit( I2C_MODULE_ID index )

  Summary:
    Identifies whether the SlaveAddress10Bit feature exists on the I2C module.

  Description:
    This function identifies whether the SlaveAddress10Bit feature is available on the I2C module.
    When this function returns true, these functions are supported on the device:
    - PLIB_I2C_SlaveAddress10BitSet
    - PLIB_I2C_SlaveAddress10BitGet
    - PLIB_I2C_SlaveAddressModeIs10Bits

  Preconditions:
    None.

  Parameters:
    index           - Identifier for the device instance

  Returns:
    - true  - If the SlaveAddress10Bit feature is supported on the device
    - false - If the SlaveAddress10Bit feature is not supported on the device

  Remarks:
    None.
*/

bool PLIB_I2C_ExistsSlaveAddress10Bit( I2C_MODULE_ID index );


//******************************************************************************
/* Function:
    PLIB_I2C_ExistsSlaveAddressDetect( I2C_MODULE_ID index )

  Summary:
    Identifies whether the SlaveAddressDetect feature exists on the I2C module.

  Description:
    This function identifies whether the SlaveAddressDetect feature is available on the I2C module.
    When this function returns true, this function is supported on the device:
    - PLIB_I2C_SlaveAddress10BitWasDetected

  Preconditions:
    None.

  Parameters:
    index           - Identifier for the device instance

  Returns:
    - true  - If the SlaveAddressDetect feature is supported on the device
    - false - If the SlaveAddressDetect feature is not supported on the device

  Remarks:
    None.
*/

bool PLIB_I2C_ExistsSlaveAddressDetect( I2C_MODULE_ID index );


//******************************************************************************
/* Function:
    PLIB_I2C_ExistsGeneralCallAddressDetect( I2C_MODULE_ID index )

  Summary:
    Identifies whether the GeneralCallAddressDetect feature exists on the I2C module.

  Description:
    This function identifies whether the GeneralCallAddressDetect feature is available on the I2C module.
    When this function returns true, this function is supported on the device:
    - PLIB_I2C_SlaveAddressIsGeneralCall

  Preconditions:
    None.

  Parameters:
    index           - Identifier for the device instance

  Returns:
    - true  - If the GeneralCallAddressDetect feature is supported on the device
    - false - If the GeneralCallAddressDetect feature is not supported on the device

  Remarks:
    None.
*/

bool PLIB_I2C_ExistsGeneralCallAddressDetect( I2C_MODULE_ID index );


//******************************************************************************
/* Function:
    PLIB_I2C_ExistsSlaveMask( I2C_MODULE_ID index )

  Summary:
    Identifies whether the SlaveMask feature exists on the I2C module.

  Description:
    This function identifies whether the SlaveMask feature is available on the I2C module.
    When this function returns true, these functions are supported on the device:
    - PLIB_I2C_SlaveMask7BitSet
    - PLIB_I2C_SlaveMask7BitGet
    - PLIB_I2C_SlaveMask10BitSet
    - PLIB_I2C_SlaveMask10BitGet

  Preconditions:
    None.

  Parameters:
    index           - Identifier for the device instance

  Returns:
    - true  - If the SlaveMask feature is supported on the device
    - false - If the SlaveMask feature is not supported on the device

  Remarks:
    None.
*/

bool PLIB_I2C_ExistsSlaveMask( I2C_MODULE_ID index );


//******************************************************************************
/* Function:
    PLIB_I2C_ExistsSlaveReadRequest( I2C_MODULE_ID index )

  Summary:
    Identifies whether the SlaveReadRequest feature exists on the I2C module.

  Description:
    This function identifies whether the SlaveReadRequest feature is available on the I2C module.
    When this function returns true, this function is supported on the device:
    - PLIB_I2C_SlaveReadIsRequested

  Preconditions:
    None.

  Parameters:
    index           - Identifier for the device instance

  Returns:
    - true  - If the SlaveReadRequest feature is supported on the device
    - false - If the SlaveReadRequest feature is not supported on the device

  Remarks:
    None.
*/

bool PLIB_I2C_ExistsSlaveReadRequest( I2C_MODULE_ID index );


//******************************************************************************
/* Function:
    PLIB_I2C_ExistsSlaveDataDetect( I2C_MODULE_ID index )

  Summary:
    Identifies whether the SlaveDataDetect feature exists on the I2C module.

  Description:
    This function identifies whether the SlaveDataDetect feature is available on the I2C module.
    When this function returns true, these functions are supported on the device:
    - PLIB_I2C_SlaveDataIsDetected
    - PLIB_I2C_SlaveAddressIsDetected

  Preconditions:
    None.

  Parameters:
    index           - Identifier for the device instance

  Returns:
    - true  - If the SlaveDataDetect feature is supported on the device
    - false - If the SlaveDataDetect feature is not supported on the device

  Remarks:
    None.
*/

bool PLIB_I2C_ExistsSlaveDataDetect( I2C_MODULE_ID index );


//******************************************************************************
/* Function:
    PLIB_I2C_ExistsSlaveClockHold( I2C_MODULE_ID index )

  Summary:
    Identifies whether the SlaveClockHold feature exists on the I2C module.

  Description:
    This function identifies whether the SlaveClockHold feature is available on the I2C module.
    When this function returns true, these functions are supported on the device:
    - PLIB_I2C_SlaveClockHold
    - PLIB_I2C_SlaveClockRelease

  Preconditions:
    None.

  Parameters:
    index           - Identifier for the device instance

  Returns:
    - true  - If the SlaveClockHold feature is supported on the device
    - false - If the SlaveClockHold feature is not supported on the device

  Remarks:
    None.
*/

bool PLIB_I2C_ExistsSlaveClockHold( I2C_MODULE_ID index );


//******************************************************************************
/* Function:
    PLIB_I2C_ExistsMasterStart( I2C_MODULE_ID index )

  Summary:
    Identifies whether the MasterStart feature exists on the I2C module.

  Description:
    This function identifies whether the MasterStart feature is available on the I2C module.
    When this function returns true, this function is supported on the device:
    - PLIB_I2C_MasterStart

  Preconditions:
    None.

  Parameters:
    index           - Identifier for the device instance

  Returns:
    - true  - If the MasterStart feature is supported on the device
    - false - If the MasterStart feature is not supported on the device

  Remarks:
    None.
*/

bool PLIB_I2C_ExistsMasterStart( I2C_MODULE_ID index );


//******************************************************************************
/* Function:
    PLIB_I2C_ExistsMasterStartRepeat( I2C_MODULE_ID index )

  Summary:
    Identifies whether the MasterStartRepeat feature exists on the I2C module.

  Description:
    This function identifies whether the MasterStartRepeat feature is available on the I2C module.
    When this function returns true, this function is supported on the device:
    - PLIB_I2C_MasterStartRepeat

  Preconditions:
    None.

  Parameters:
    index           - Identifier for the device instance

  Returns:
    - true  - If the MasterStartRepeat feature is supported on the device
    - false - If the MasterStartRepeat feature is not supported on the device

  Remarks:
    None.
*/

bool PLIB_I2C_ExistsMasterStartRepeat( I2C_MODULE_ID index );


//******************************************************************************
/* Function:
    PLIB_I2C_ExistsMasterReceiverClock1Byte( I2C_MODULE_ID index )

  Summary:
    Identifies whether the MasterReceiverClock1Byte feature exists on the I2C module.

  Description:
    This function identifies whether the MasterReceiverClock1Byte feature is available on the I2C module.
    When this function returns true, this function is supported on the device:
    - PLIB_I2C_MasterReceiverClock1Byte

  Preconditions:
    None.

  Parameters:
    index           - Identifier for the device instance

  Returns:
    - true  - If the MasterReceiverClock1Byte feature is supported on the device
    - false - If the MasterReceiverClock1Byte feature is not supported on the device

  Remarks:
    None.
*/

bool PLIB_I2C_ExistsMasterReceiverClock1Byte( I2C_MODULE_ID index );


//******************************************************************************
/* Function:  
    PLIB_I2C_ExistsMasterStop( I2C_MODULE_ID index )

  Summary:
    Identifies whether the MasterStop feature exists on the I2C module.

  Description:
    This function identifies whether the MasterStop feature is available on the I2C module.
    When this function returns true, this function is supported on the device:
    - PLIB_I2C_MasterStop

  Preconditions:
    None.

  Parameters:
    index           - Identifier for the device instance

  Returns:
    - true  - If the MasterStop feature is supported on the device
    - false - If the MasterStop feature is not supported on the device

  Remarks:
    None.
*/

bool PLIB_I2C_ExistsMasterStop( I2C_MODULE_ID index );


//******************************************************************************
/* Function:
    PLIB_I2C_ExistsTransmitterIsBusy( I2C_MODULE_ID index )

  Summary:
    Identifies whether the TransmitterBusy feature exists on the I2C module.

  Description:
    This function identifies whether the TransmitterBusy feature is available on the I2C module.
    When this function returns true, these functions are supported on the device:
    - PLIB_I2C_TransmitterIsBusy
    - PLIB_I2C_TransmitterIsReady

  Preconditions:
    None.

  Parameters:
    index           - Identifier for the device instance

  Returns:
    - true  - If the TransmitterBusy feature is supported on the device
    - false - If the TransmitterBusy feature is not supported on the device

  Remarks:
    None.
*/

bool PLIB_I2C_ExistsTransmitterIsBusy( I2C_MODULE_ID index );


//******************************************************************************
/* Function:
    PLIB_I2C_ExistsTransmitterByteSend( I2C_MODULE_ID index )

  Summary:
    Identifies whether the TransmitterByteSend feature exists on the I2C module.

  Description:
    This function identifies whether the TransmitterByteSend feature is available on the I2C module.
    When this function returns true, this function is supported on the device:
    - PLIB_I2C_TransmitterByteSend

  Preconditions:
    None.

  Parameters:
    index           - Identifier for the device instance

  Returns:
    - true  - If the TransmitterByteSend feature is supported on the device
    - false - If the TransmitterByteSend feature is not supported on the device

  Remarks:
    None.
*/

bool PLIB_I2C_ExistsTransmitterByteSend( I2C_MODULE_ID index );


//******************************************************************************
/* Function:
    PLIB_I2C_ExistsTransmitterByteComplete( I2C_MODULE_ID index )

  Summary:
    Identifies whether the TransmitterByteComplete feature exists on the I2C 
	module.

  Description:
    This function identifies whether the TransmitterByteComplete feature is available on the I2C module.
    When this function returns true, this function is supported on the device:
    - PLIB_I2C_TransmitterByteHasCompleted

  Preconditions:
    None.

  Parameters:
    index           - Identifier for the device instance

  Returns:
    - true  - If the TransmitterByteComplete feature is supported on the device
    - false - If the TransmitterByteComplete feature is not supported on the device

  Remarks:
    None.
*/

bool PLIB_I2C_ExistsTransmitterByteComplete( I2C_MODULE_ID index );


//******************************************************************************
/* Function:
    PLIB_I2C_ExistsTransmitterByteAcknowledge( I2C_MODULE_ID index )

  Summary:
    Identifies whether the TransmitterByteAcknowledge feature exists on the I2C 
	module.

  Description:
    This function identifies whether the TransmitterByteAcknowledge feature is 
	available on the I2C module.
    When this function returns true, this function is supported on the device:
    - PLIB_I2C_TransmitterByteWasAcknowledged

  Preconditions:
    None.

  Parameters:
    index           - Identifier for the device instance

  Returns:
    - true  - If the TransmitterByteAcknowledge feature is supported on the device
    - false - If the TransmitterByteAcknowledge feature is not supported on the device

  Remarks:
    None.
*/

bool PLIB_I2C_ExistsTransmitterByteAcknowledge( I2C_MODULE_ID index );


//******************************************************************************
/* Function:
    PLIB_I2C_ExistsTransmitterOverflow( I2C_MODULE_ID index )

  Summary:
    Identifies whether the TransmitterOverflow feature exists on the I2C module.

  Description:
    This function identifies whether the TransmitterOverflow feature is available 
	on the I2C module.
    When this function returns true, these functions are supported on the device:
    - PLIB_I2C_TransmitterOverflowHasOccurred
    - PLIB_I2C_TransmitterOverflowClear

  Preconditions:
    None.

  Parameters:
    index           - Identifier for the device instance

  Returns:
    - true  - If the TransmitterOverflow feature is supported on the device
    - false - If the TransmitterOverflow feature is not supported on the device

  Remarks:
    None.
*/

bool PLIB_I2C_ExistsTransmitterOverflow( I2C_MODULE_ID index );


//******************************************************************************
/* Function :  PLIB_I2C_ExistsReceivedByteAvailable( I2C_MODULE_ID index )

  Summary:
    Identifies whether the ReceivedByteAvailable feature exists on the I2C module

  Description:
    This function identifies whether the ReceivedByteAvailable feature is available on the I2C module.
    When this function returns true, this function is supported on the device:
    - PLIB_I2C_ReceivedByteIsAvailable

  Preconditions:
    None.

  Parameters:
    index           - Identifier for the device instance

  Returns:
    - true  - If the ReceivedByteAvailable feature is supported on the device
    - false - If the ReceivedByteAvailable feature is not supported on the device

  Remarks:
    None.
*/

bool PLIB_I2C_ExistsReceivedByteAvailable( I2C_MODULE_ID index );


//******************************************************************************
/* Function :  PLIB_I2C_ExistsReceivedByteGet( I2C_MODULE_ID index )

  Summary:
    Identifies whether the ReceivedByteGet feature exists on the I2C module

  Description:
    This function identifies whether the ReceivedByteGet feature is available 
	on the I2C module.
    When this function returns true, this function is supported on the device:
    - PLIB_I2C_ReceivedByteGet

  Preconditions:
    None.

  Parameters:
    index           - Identifier for the device instance

  Returns:
    - true  - If the ReceivedByteGet feature is supported on the device
    - false - If the ReceivedByteGet feature is not supported on the device

  Remarks:
    None.
*/

bool PLIB_I2C_ExistsReceivedByteGet( I2C_MODULE_ID index );


//******************************************************************************
/* Function:
    PLIB_I2C_ExistsReceivedByteAcknowledge( I2C_MODULE_ID index )

  Summary:
    Identifies whether the ReceivedByteAcknowledge feature exists on the I2C module.

  Description:
    This function identifies whether the ReceivedByteAcknowledge feature is available 
	on the I2C module.
    When this function returns true, these functions are supported on the device:
    - PLIB_I2C_ReceivedByteAcknowledge
    - PLIB_I2C_ReceiverByteAcknowledgeHasCompleted
    - PLIB_I2C_MasterReceiverReadyToAcknowledge

  Preconditions:
    None.

  Parameters:
    index           - Identifier for the device instance

  Returns:
    - true  - If the ReceivedByteAcknowledge feature is supported on the device
    - false - If the ReceivedByteAcknowledge feature is not supported on the device

  Remarks:
    None.
*/

bool PLIB_I2C_ExistsReceivedByteAcknowledge( I2C_MODULE_ID index );


//******************************************************************************
/* Function:
    PLIB_I2C_ExistsReceiverOverflow( I2C_MODULE_ID index )

  Summary:
    Identifies whether the ReceiverOverflow feature exists on the I2C module.

  Description:
    This function identifies whether the ReceiverOverflow feature is available 
	on the I2C module.
    When this function returns true, these functions are supported on the device:
    - PLIB_I2C_ReceiverOverflowHasOccurred
    - PLIB_I2C_ReceiverOverflowClear

  Preconditions:
    None.

  Parameters:
    index           - Identifier for the device instance

  Returns:
    - true  - If the ReceiverOverflow feature is supported on the device
    - false - If the ReceiverOverflow feature is not supported on the device

  Remarks:
    None.
*/

bool PLIB_I2C_ExistsReceiverOverflow( I2C_MODULE_ID index );


//******************************************************************************
/* Function:
    PLIB_I2C_ExistsSlaveInterruptOnStop( I2C_MODULE_ID index )

  Summary:
    Identifies whether the SlaveInterruptOnStop feature exists on the I2C module.

  Description:
    This function identifies whether the SlaveInterruptOnStop feature is available 
	on the I2C module.
    When this function returns true, these functions are supported on the device:
    - PLIB_I2C_SlaveInterruptOnStopEnable
    - PLIB_I2C_SlaveInterruptOnStopDisable

  Preconditions:
    None.

  Parameters:
    index           - Identifier for the device instance

  Returns:
    - true  - If the SlaveInterruptOnStop feature is supported on the device
    - false - If the SlaveInterruptOnStop feature is not supported on the device

  Remarks:
    None.
*/

bool PLIB_I2C_ExistsSlaveInterruptOnStop( I2C_MODULE_ID index );


//******************************************************************************
/* Function:
    PLIB_I2C_ExistsSlaveInterruptOnStart( I2C_MODULE_ID index )

  Summary:
    Identifies whether the SlaveInterruptOnStart feature exists on the I2C module.

  Description:
    This function identifies whether the SlaveInterruptOnStart feature is available 
	on the I2C module.
    When this function returns true, these functions are supported on the device:
    - PLIB_I2C_SlaveInterruptOnStartEnable
    - PLIB_I2C_SlaveInterruptOnStartDisable

  Preconditions:
    None.

  Parameters:
    index           - Identifier for the device instance

  Returns:
    - true  - If the SlaveInterruptOnStart feature is supported on the device
    - false - If the SlaveInterruptOnStart feature is not supported on the device

  Remarks:
    None.
*/

bool PLIB_I2C_ExistsSlaveInterruptOnStart( I2C_MODULE_ID index );


//******************************************************************************
/* Function:
    PLIB_I2C_ExistsSlaveBufferOverwrite( I2C_MODULE_ID index )

  Summary:
    Identifies whether the SlaveBufferOverwrite feature exists on the I2C module.

  Description:
    This function identifies whether the SlaveBufferOverwrite feature is available 
	on the I2C module.
    When this function returns true, these functions are supported on the device:
    - PLIB_I2C_SlaveBufferOverwriteEnable
    - PLIB_I2C_SlaveBufferOverwriteDisable

  Preconditions:
    None.

  Parameters:
    index           - Identifier for the device instance

  Returns:
    - true  - If the SlaveBufferOverwrite feature is supported on the device
    - false - If the SlaveBufferOverwrite feature is not supported on the device

  Remarks:
    None.
*/

bool PLIB_I2C_ExistsSlaveBufferOverwrite( I2C_MODULE_ID index );


//******************************************************************************
/* Function:
    PLIB_I2C_ExistsDataLineHoldTime( I2C_MODULE_ID index )

  Summary:
    Identifies whether the DataLineHoldTime feature exists on the I2C module.

  Description:
    This function identifies whether the DataLineHoldTime feature is available 
	on the I2C module.
    When this function returns true, this function is supported on the device:
    - PLIB_I2C_DataLineHoldTimeSet

  Preconditions:
    None.

  Parameters:
    index           - Identifier for the device instance

  Returns:
    - true  - If the DataLineHoldTime feature is supported on the device
    - false - If the DataLineHoldTime feature is not supported on the device

  Remarks:
    None.
*/

bool PLIB_I2C_ExistsDataLineHoldTime( I2C_MODULE_ID index );


//******************************************************************************
/* Function:
    PLIB_I2C_ExistsSlaveBusCollisionDetect( I2C_MODULE_ID index )

  Summary:
    Identifies whether the SlaveBusCollisionDetect feature exists on the I2C module.

  Description:
    This function identifies whether the SlaveBusCollisionDetect feature is available 
	on the I2C module.
    When this function returns true, these functions are supported on the device:
    - PLIB_I2C_SlaveBusCollisionDetectEnable
    - PLIB_I2C_SlaveBusCollisionDetectDisable

  Preconditions:
    None.

  Parameters:
    index           - Identifier for the device instance

  Returns:
    - true  - If the SlaveBusCollisionDetect feature is supported on the device
    - false - If the SlaveBusCollisionDetect feature is not supported on the device

  Remarks:
    None.
*/

bool PLIB_I2C_ExistsSlaveBusCollisionDetect( I2C_MODULE_ID index );


//******************************************************************************
/* Function:
   PLIB_I2C_ExistsSlaveAddressHoldEnable( I2C_MODULE_ID index )

  Summary:
    Identifies whether the SlaveAddressHoldEnable feature exists on the I2C module.

  Description:
    This function identifies whether the SlaveAddressHoldEnable feature is available 
	on the I2C module.
    When this function returns true, these functions are supported on the device:
    - PLIB_I2C_SlaveAddressHoldEnable
    - PLIB_I2C_SlaveAddressHoldDisable

  Preconditions:
    None.

  Parameters:
    index           - Identifier for the device instance

  Returns:
    - true  - If the SlaveAddressHoldEnable feature is supported on the device
    - false - If the SlaveAddressHoldEnable feature is not supported on the device

  Remarks:
    None.
*/

bool PLIB_I2C_ExistsSlaveAddressHoldEnable( I2C_MODULE_ID index );


//******************************************************************************
/* Function:
    PLIB_I2C_ExistsSlaveDataHoldEnable( I2C_MODULE_ID index )

  Summary:
    Identifies whether the SlaveDataHoldEnable feature exists on the I2C module.

  Description:
    This function identifies whether the SlaveDataHoldEnable feature is available 
	on the I2C module.
    When this function returns true, these functions are supported on the device:
    - PLIB_I2C_SlaveDataHoldEnable
    - PLIB_I2C_SlaveDataHoldDisable

  Preconditions:
    None.

  Parameters:
    index           - Identifier for the device instance

  Returns:
    - true  - If the SlaveDataHoldEnable feature is supported on the device
    - false - If the SlaveDataHoldEnable feature is not supported on the device

  Remarks:
    None.
*/

bool PLIB_I2C_ExistsSlaveDataHoldEnable( I2C_MODULE_ID index );


//******************************************************************************
/* Function:
    PLIB_I2C_ExistsAcksequenceProgress( I2C_MODULE_ID index )

  Summary:
    Identifies whether the AcksequenceIsInProgress feature exists on the I2C module.

  Description:
    This function identifies whether the AcksequenceIsInProgress feature is available 
	on the I2C module.
    When this function returns true, this function is supported on the device:
    - PLIB_I2C_AcksequenceIsInProgress

  Preconditions:
    None.

  Parameters:
    index           - Identifier for the device instance

  Returns:
    - true  - If the AcksequenceIsInProgress feature is supported on the device
    - false - If the AcksequenceIsInProgress feature is not supported on the device

  Remarks:
    None.
*/

bool PLIB_I2C_ExistsAcksequenceProgress( I2C_MODULE_ID index );

//DOM-IGNORE-BEGIN
#ifdef __cplusplus
}
#endif
//DOM-IGNORE-END

#endif // _PLIB_I2C_H
/*******************************************************************************
 End of File
*/