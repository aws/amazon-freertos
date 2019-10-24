/*******************************************************************************
  I2C Peripheral Library Template Implementation

  File Name:
    i2c_SlaveAddress10Bit_Default.h

  Summary:
    I2C PLIB Template Implementation

  Description:
    This header file contains template implementations
    For Feature : SlaveAddress10Bit
    and its Variant : Default
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

#ifndef _I2C_SLAVEADDRESS10BIT_DEFAULT_H
#define _I2C_SLAVEADDRESS10BIT_DEFAULT_H

#include "i2c_Registers.h"

//******************************************************************************
/* Function :  I2C_SlaveAddress10BitSet_Default

  Summary:
    Implements Default variant of PLIB_I2C_SlaveAddress10BitSet 
    Operation is atomic

  Description:
    This template implements the Default variant of the PLIB_I2C_SlaveAddress10BitSet function.
*/

PLIB_TEMPLATE void I2C_SlaveAddress10BitSet_Default( I2C_MODULE_ID index , uint16_t address )
{
    volatile i2c_register_t *regs = (i2c_register_t *)index;

    regs->I2CxADD = address;

    /* Configure for 10-bit address */
    regs->I2CxCONSET = I2CxCON_A10M_MASK;
}


//******************************************************************************
/* Function :  I2C_SlaveAddress10BitGet_Default

  Summary:
    Implements Default variant of PLIB_I2C_SlaveAddress10BitGet 
    Operation is atomic

  Description:
    This template implements the Default variant of the PLIB_I2C_SlaveAddress10BitGet function.
*/

PLIB_TEMPLATE uint16_t I2C_SlaveAddress10BitGet_Default( I2C_MODULE_ID index )
{
    volatile i2c_register_t *regs = (i2c_register_t *)index;

    return (uint16_t)regs->I2CxADD;
}


//******************************************************************************
/* Function :  I2C_SlaveAddressModeIs10Bits_Default

  Summary:
    Implements Default variant of PLIB_I2C_SlaveAddressModeIs10Bits 
    Operation is atomic

  Description:
    This template implements the Default variant of the PLIB_I2C_SlaveAddressModeIs10Bits function.
*/

PLIB_TEMPLATE bool I2C_SlaveAddressModeIs10Bits_Default( I2C_MODULE_ID index )
{
    volatile i2c_register_t *regs = (i2c_register_t *)index;
    return (bool)regs->I2CxCON.A10M;
}


//******************************************************************************
/* Function :  I2C_ExistsSlaveAddress10Bit_Default

  Summary:
    Implements Default variant of PLIB_I2C_ExistsSlaveAddress10Bit

  Description:
    This template implements the Default variant of the PLIB_I2C_ExistsSlaveAddress10Bit function.
*/

#define PLIB_I2C_ExistsSlaveAddress10Bit PLIB_I2C_ExistsSlaveAddress10Bit
PLIB_TEMPLATE bool I2C_ExistsSlaveAddress10Bit_Default( I2C_MODULE_ID index )
{
    return true;
}


#endif /*_I2C_SLAVEADDRESS10BIT_DEFAULT_H*/

/******************************************************************************
 End of File
*/

