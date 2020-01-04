/*******************************************************************************
  I2C Peripheral Library Template Implementation

  File Name:
    i2c_SlaveAddress10Bit_Unsupported.h

  Summary:
    I2C PLIB Template Implementation

  Description:
    This header file contains template implementations
    For Feature : SlaveAddress10Bit
    and its Variant : Unsupported
    For following APIs :
        PLIB_I2C_SlaveAddress10BitSet
        PLIB_I2C_SlaveAddress10BitGet
        PLIB_I2C_SlaveAddressModeIs10Bits
        PLIB_I2C_ExistsSlaveAddress10Bit

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

#ifndef _I2C_SLAVEADDRESS10BIT_UNSUPPORTED_H
#define _I2C_SLAVEADDRESS10BIT_UNSUPPORTED_H

//******************************************************************************
/* Routines available for accessing VREGS, MASKS, POS, LEN are 

  VREGs: 
    None.

  MASKs: 
    None.

  POSs: 
    None.

  LENs: 
    None.

*/


//******************************************************************************
/* Function :  I2C_SlaveAddress10BitSet_Unsupported

  Summary:
    Implements Unsupported variant of PLIB_I2C_SlaveAddress10BitSet 

  Description:
    This template implements the Unsupported variant of the PLIB_I2C_SlaveAddress10BitSet function.
*/

PLIB_TEMPLATE void I2C_SlaveAddress10BitSet_Unsupported( I2C_MODULE_ID index , uint16_t address )
{
    PLIB_ASSERT(false, "The device selected does not implement PLIB_I2C_SlaveAddress10BitSet");
}


//******************************************************************************
/* Function :  I2C_SlaveAddress10BitGet_Unsupported

  Summary:
    Implements Unsupported variant of PLIB_I2C_SlaveAddress10BitGet 

  Description:
    This template implements the Unsupported variant of the PLIB_I2C_SlaveAddress10BitGet function.
*/

PLIB_TEMPLATE uint16_t I2C_SlaveAddress10BitGet_Unsupported( I2C_MODULE_ID index )
{
    PLIB_ASSERT(false, "The device selected does not implement PLIB_I2C_SlaveAddress10BitGet");

    return 0;
}


//******************************************************************************
/* Function :  I2C_SlaveAddressModeIs10Bits_Unsupported

  Summary:
    Implements Unsupported variant of PLIB_I2C_SlaveAddressModeIs10Bits 

  Description:
    This template implements the Unsupported variant of the PLIB_I2C_SlaveAddressModeIs10Bits function.
*/

PLIB_TEMPLATE bool I2C_SlaveAddressModeIs10Bits_Unsupported( I2C_MODULE_ID index )
{
    PLIB_ASSERT(false, "The device selected does not implement PLIB_I2C_SlaveAddressModeIs10Bits");

    return false;
}


//******************************************************************************
/* Function :  I2C_ExistsSlaveAddress10Bit_Unsupported

  Summary:
    Implements Unsupported variant of PLIB_I2C_ExistsSlaveAddress10Bit

  Description:
    This template implements the Unsupported variant of the PLIB_I2C_ExistsSlaveAddress10Bit function.
*/

PLIB_TEMPLATE bool I2C_ExistsSlaveAddress10Bit_Unsupported( I2C_MODULE_ID index )
{
    return false;
}


#endif /*_I2C_SLAVEADDRESS10BIT_UNSUPPORTED_H*/

/******************************************************************************
 End of File
*/

