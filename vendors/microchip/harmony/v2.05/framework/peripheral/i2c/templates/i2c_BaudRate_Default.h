/*******************************************************************************
  I2C Peripheral Library Template Implementation

  File Name:
    i2c_BaudRate_Default.h

  Summary:
    I2C PLIB Template Implementation

  Description:
    This header file contains template implementations
    For Feature : BaudRate
    and its Variant : Default
    For following APIs :
        PLIB_I2C_BaudRateSet
        PLIB_I2C_BaudRateGet
        PLIB_I2C_ExistsBaudRate

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

#ifndef _I2C_BAUDRATE_DEFAULT_H
#define _I2C_BAUDRATE_DEFAULT_H

#include "i2c_Registers.h"

// *****************************************************************************
/* Pulse gobbler delay

  Summary:
    Defines the Pulse gobbler delay in micro seconds.

  Description:
    This macro defines the Pulse gobbler delay in micro seconds.

  Remarks:
    None
*/

#define PULSE_GOBBLER_DELAY						0.104f

//******************************************************************************
/* Function :  I2C_BaudRateSet_Default

  Summary:
    Implements Default variant of PLIB_I2C_BaudRateSet
    Operation is atomic.

  Description:
    This template implements the Default variant of the PLIB_I2C_BaudRateSet function.
*/

PLIB_TEMPLATE void I2C_BaudRateSet_Default( I2C_MODULE_ID index , uint32_t clockFrequency , I2C_BAUD_RATE baudRate )
{
    volatile i2c_register_t *regs = (i2c_register_t *)index;

    regs->I2CxBRG = (clockFrequency / baudRate / 2) - (clockFrequency / 10000000) - 2;
}


//******************************************************************************
/* Function :  I2C_BaudRateGet_Default

  Summary:
    Implements Default variant of PLIB_I2C_BaudRateGet
    Operation is atomic

  Description:
    This template implements the Default variant of the PLIB_I2C_BaudRateGet function.
*/

PLIB_TEMPLATE I2C_BAUD_RATE I2C_BaudRateGet_Default( I2C_MODULE_ID index , uint32_t clockFrequency )
{
    volatile i2c_register_t *regs = (i2c_register_t *)index;
    uint32_t div = regs->I2CxBRG;

    return (clockFrequency / ((div + 2) + (clockFrequency / 10000000))) >> 1;
}


//******************************************************************************
/* Function :  I2C_ExistsBaudRate_Default

  Summary:
    Implements Default variant of PLIB_I2C_ExistsBaudRate

  Description:
    This template implements the Default variant of the PLIB_I2C_ExistsBaudRate function.
*/

#define PLIB_I2C_ExistsBaudRate PLIB_I2C_ExistsBaudRate
PLIB_TEMPLATE bool I2C_ExistsBaudRate_Default( I2C_MODULE_ID index )
{
    return true;
}


#endif /*_I2C_BAUDRATE_DEFAULT_H*/

/******************************************************************************
 End of File
*/

