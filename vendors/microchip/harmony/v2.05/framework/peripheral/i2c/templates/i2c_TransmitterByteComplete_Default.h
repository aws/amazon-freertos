/*******************************************************************************
  I2C Peripheral Library Template Implementation

  File Name:
    i2c_TransmitterByteComplete_Default.h

  Summary:
    I2C PLIB Template Implementation

  Description:
    This header file contains template implementations
    For Feature : TransmitterByteComplete
    and its Variant : Default
    For following APIs :
        PLIB_I2C_TransmitterByteHasCompleted
        PLIB_I2C_ExistsTransmitterByteComplete

*******************************************************************************/

//DOM-IGNORE-BEGIN
/*******************************************************************************
Copyright (c) 2012 released Microchip Technology Inc.  All rights reserved.

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

//DOM-IGNORE-END

#ifndef _I2C_TRANSMITTERBYTECOMPLETE_DEFAULT_H
#define _I2C_TRANSMITTERBYTECOMPLETE_DEFAULT_H

#include "i2c_Registers.h"

//******************************************************************************
/* Function :  I2C_TransmitterByteHasCompleted_Default

  Summary:
    Implements Default variant of PLIB_I2C_TransmitterByteHasCompleted 
    Operation is atomic

  Description:
    This template implements the Default variant of the PLIB_I2C_TransmitterByteHasCompleted function.
*/

PLIB_TEMPLATE bool I2C_TransmitterByteHasCompleted_Default( I2C_MODULE_ID index )
{
    volatile i2c_register_t *regs = (i2c_register_t *)index;

    return (bool)!regs->I2CxSTAT.TRSTAT;
}


//******************************************************************************
/* Function :  I2C_ExistsTransmitterByteComplete_Default

  Summary:
    Implements Default variant of PLIB_I2C_ExistsTransmitterByteComplete

  Description:
    This template implements the Default variant of the PLIB_I2C_ExistsTransmitterByteComplete function.
*/

#define PLIB_I2C_ExistsTransmitterByteComplete PLIB_I2C_ExistsTransmitterByteComplete
PLIB_TEMPLATE bool I2C_ExistsTransmitterByteComplete_Default( I2C_MODULE_ID index )
{
    return true;
}


#endif /*_I2C_TRANSMITTERBYTECOMPLETE_DEFAULT_H*/

/******************************************************************************
 End of File
*/

