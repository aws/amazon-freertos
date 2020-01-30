/*******************************************************************************
  I2C Peripheral Library Template Implementation

  File Name:
    i2c_BusIdle_Default.h

  Summary:
    I2C PLIB Template Implementation

  Description:
    This header file contains template implementations
    For Feature : BusIdle
    and its Variant : Default
    For following APIs :
        PLIB_I2C_BusIsIdle
        PLIB_I2C_ExistsBusIsIdle

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

#ifndef _I2C_BUSIDLE_DEFAULT_H
#define _I2C_BUSIDLE_DEFAULT_H

#include "i2c_Registers.h"

// *****************************************************************************
/* I2C bus idle status bits

  Summary:
    Creates a mask for i2c bus idle status bits.

  Description:
    This macro defines a mask for bus idle bits in the i2c control register.

  Remarks:
    None
*/

#define I2C_BUS_IDLE_BITS						0x1F

//******************************************************************************
/* Function :  I2C_BusIsIdle_Default

  Summary:
    Implements Default variant of PLIB_I2C_BusIsIdle
    Operation is atomic

  Description:
    This template implements the Default variant of the PLIB_I2C_BusIsIdle function.
*/

PLIB_TEMPLATE bool I2C_BusIsIdle_Default( I2C_MODULE_ID index )
{
    volatile i2c_register_t *regs = (i2c_register_t *)index;
    volatile uint32_t *i2c_ctrl_reg = (volatile uint32_t *)&regs->I2CxCON;

    return (bool)!((*i2c_ctrl_reg & I2C_BUS_IDLE_BITS) || regs->I2CxSTAT.TRSTAT);
}


//******************************************************************************
/* Function :  I2C_ExistsBusIsIdle_Default

  Summary:
    Implements Default variant of PLIB_I2C_ExistsBusIsIdle

  Description:
    This template implements the Default variant of the PLIB_I2C_ExistsBusIsIdle function.
*/

#define PLIB_I2C_ExistsBusIsIdle PLIB_I2C_ExistsBusIsIdle
PLIB_TEMPLATE bool I2C_ExistsBusIsIdle_Default( I2C_MODULE_ID index )
{
    return true;
}


#endif /*_I2C_BUSIDLE_DEFAULT_H*/

/******************************************************************************
 End of File
*/

