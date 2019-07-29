/*******************************************************************************
  I2C Bit Bang Functions Header File

  Company:
    Microchip Technology Inc.

  File Name:
    drv_i2c_bb.h       

  Summary:
    Contains prototypes for the I2C functions
*******************************************************************************/

// DOM-IGNORE-BEGIN
/*******************************************************************************
Copyright (c) 2013 released Microchip Technology Inc.  All rights reserved.

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
// DOM-IGNORE-END

#ifndef DRV_I2C_BB_H
#define	DRV_I2C_BB_H

// *****************************************************************************
// *****************************************************************************
// Section: Included Files
// *****************************************************************************
// *****************************************************************************

#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>
#include "drv_i2c_bb_a.h"
#include "driver/i2c/src/drv_i2c_local_bb.h"

// DOM-IGNORE-BEGIN
#ifdef __cplusplus  

    extern "C" {

#endif
// DOM-IGNORE-END  


#define OUTPUT              0
#define INPUT               1

#define LOW                 0
#define HIGH                1

#define M_ACK               0
#define M_NACK              1

// DOM-IGNORE-BEGIN
// *****************************************************************************
// *****************************************************************************
// Section: Function Prototypes
// *****************************************************************************
// *****************************************************************************

void DRV_I2C_BB_Initialize(DRV_I2C_OBJ *dObj, DRV_I2C_INIT * i2cInit);

DRV_I2C_BUFFER_HANDLE DRV_I2C_MasterBufferWriteXE(DRV_HANDLE handle, uint16_t deviceaddress, uint8_t* txBuffer, uint16_t txbuflen);

DRV_I2C_BUFFER_HANDLE DRV_I2C_MasterBufferReadXE(DRV_HANDLE handle, uint16_t deviceaddress, uint8_t* rxBuffer, uint16_t rxbuflen);

DRV_I2C_BUFFER_HANDLE DRV_I2C_MasterBufferWriteReadXE(DRV_HANDLE handle, uint16_t deviceaddress, uint8_t* txBuffer, uint16_t txbuflen, uint8_t* rxBuffer, uint16_t rxbuflen);

DRV_I2C_BUFFER_HANDLE DRV_I2C_MasterBufferWrite(DRV_HANDLE handle, uint8_t* deviceaddress, uint8_t* txBuffer, uint16_t txbuflen);

DRV_I2C_BUFFER_HANDLE DRV_I2C_MasterBufferRead(DRV_HANDLE handle, uint8_t* deviceaddress, uint8_t* rxBuffer, uint16_t rxbuflen);

DRV_I2C_BUFFER_HANDLE DRV_I2C_MasterBufferWriteRead(DRV_HANDLE handle, uint8_t* deviceaddress, uint8_t* txBuffer, uint16_t txbuflen, uint8_t* rxBuffer, uint16_t rxbuflen);

bool _DRV_I2C_IsQueueEmpty(DRV_I2C_OBJ *dObj);

void _DRV_I2C_Advance_Queue( DRV_I2C_OBJ *dObj );

// DOM-IGNORE-END 

// *****************************************************************************
/* Function: void DRV_I2C_BB_Tasks (SYS_MODULE_OBJ object)

    Summary:
        Maintains the State Machine of the I2C driver and performs all the
        protocol level actions.
		<p><b>Implementation:</b> Dynamic</p>

    Description:
        This functions maintains the internal state machine of the bit-banged
        I2C driver. This bit-banged implementation is only done for Master mode. 
        The This function implements all the protocol level details like setting 
        the START condition, sending the address with with R/W request, writing 
        data to the SFR, checking for acknowledge and setting the STOP 
        condition. Since the Timer interrupt is used to implement the I2C 
        state machine in bit-banged mode, the DRV_I2C_BB_Tasks() function 
        can only be used in interrupt mode.

    Precondition:
        The DRV_I2C_Initialize routine must have been called for the specified
        I2C device instance. 

    Example:
         SYS_MODULE_OBJ         object;

         while (true)
         {
             DRV_I2C_BB_Tasks ( object );

         }
*/

// *****************************************************************************
/* Function:
    void StartI2C(void)

  Input: None
 
  Output: None

  Summary:
    Generates START condition on I2C bus

  Description:
    Initiate a START condition on I2C bus. 

  Remarks:
    Check for BUS IDLE before setting a START condition
*/

void StartI2C(DRV_HANDLE handle);


inline void DRV_I2C_BB_Tasks ( SYS_MODULE_OBJ object );



// DOM-IGNORE-BEGIN
#ifdef __cplusplus
}
#endif
// DOM-IGNORE-END

#endif	/* I2C_FUNCTIONS_H */


/*******************************************************************************
 End of File
*/

