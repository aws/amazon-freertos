/*******************************************************************************
  I2C Peripheral Library Interface Header

  Company:      Microchip Technology Inc.

  File Name:    plib_i2c_helper_macros.h

  Summary:
    This file contains the helper macros for the I2C Peripheral Library.

  Description:
	This file contains the helper macros for the I2C Peripheral Library.
*******************************************************************************/

// DOM-IGNORE-BEGIN
/*******************************************************************************
Copyright © 2013 released Microchip Technology Inc.  All rights reserved.

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
#ifndef _PLIB_I2C_HELPER_MACROS_H
#define _PLIB_I2C_HELPER_MACROS_H

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
// Section: Data Types & Constants
// *****************************************************************************
// *****************************************************************************

// *****************************************************************************
/* I2C 7-Bit Address

  Summary:
    Structure of a 7-bit I2C address bit fields.

  Description:
    This union defines each field of the format of a 7-bit I2C slave address.

  Remarks:
*/

typedef struct _I2C_ADDRESS_7BIT_SPLIT
{
	/* The read/write bit of the I2C address */
	uint8_t rw       : 1;

	/* The 7-bit slave address */
	uint8_t address  : 7;

}I2C_ADDRESS_7BIT_SPLIT;


// *****************************************************************************
/* I2C 7-Bit Address

  Summary:
    Structure of a 7-bit I2C address.

  Description:
    This union defines the format of a 7-bit I2C slave address.

  Remarks:
*/

typedef union _I2C_7_BIT_ADDRESS
{
    /* Access as a complete byte */
    uint8_t     byte;

    /* Access to the 8-bit format with the 7-bit address and the read/write
    direction defined */
    I2C_ADDRESS_7BIT_SPLIT bits;

} I2C_7_BIT_ADDRESS;


// *****************************************************************************
/* I2C 10-Bit Address

  Summary:
    Structure of a 10-bit I2C address in bytes.

  Description:
   This union defines the format of a 10-bit I2C slave address in two bytes.

  Remarks:
*/

typedef struct _I2C_ADDRESS_10BIT_IN_TWO_BYTES_
{
	/* Access to the first byte */
	uint8_t     first_byte;

	/* Access to the second byte */
	uint8_t     second_byte;

}I2C_ADDRESS_10BIT_IN_TWO_BYTES;


// *****************************************************************************
/* I2C 10-Bit Address

  Summary:
    Structure of a 10-bit I2C address each bit fields.

  Description:
   This union defines the format of a 10-bit I2C slave address each bits.

  Remarks:
*/

typedef struct _I2C_ADDRESS_10BIT_SPLIT_
{
	/* The read/write bit of the I2C address */
	uint16_t    rw          : 1;

	/* The 2 high-order bits of the 10-bit address */
	uint16_t    high_bits   : 2;

	/* The 5 constant bits, indicating 10-bit addressing */
	uint16_t    const_bits  : 5;

	/* The 8 low-order bits of the 10-bit address */
	uint16_t    low_bits    : 8;

}I2C_ADDRESS_10BIT_SPLIT;


// *****************************************************************************
/* I2C 10-Bit Address

  Summary:
    Structure of a 10-bit I2C address.

  Description:
   This union defines the format of a 10-bit I2C slave address.

  Remarks:
*/

typedef union _I2C_10_BIT_ADDRESS
{
    /* Access to the 16-bit format */
    uint16_t        both_bytes;

    /* Access as two bytes */
    I2C_ADDRESS_10BIT_IN_TWO_BYTES bytes;

    /* Access every bit fields */
    I2C_ADDRESS_10BIT_SPLIT bits;

} I2C_10_BIT_ADDRESS;


// *****************************************************************************
/* I2C Baud Rate

  Summary:
    Holds the I2C baud rate value.

  Description:
    This data type holds information necessary to generate a desired baud rate.

  Remarks:

*/

typedef uint32_t I2C_BAUD_RATE;


// *****************************************************************************
/* I2C Read Bit

  Summary:
    Indicates that the following data transfer is from the slave to the master.

  Description:
    This definition can be used to set the read/write direction bit
    (indicating a read) when addressing I2C slave devices.
*/

#define I2C_READ                       1


// *****************************************************************************
/* I2C Write Bit

  Summary:
    Indicates that the following data transfer is from the master to the slave.

  Description:
    This definition can be used to clear the read/write direction bit
    (indicating a write) when addressing I2C slave devices.
*/

#define I2C_WRITE                      0


// *****************************************************************************
/* I2C General Call Reserved Slave Addresses

  Summary:
    I2C general call reserved slave addresses.

  Description:
    The General Call address can be used to broadcast a message to every slave
    device at the same time.
*/

#define I2C_GENERAL_CALL_ADDRESS       0x00


// *****************************************************************************
/* I2C "Start Byte" Reserved Slave Addresses

  Summary:
    Sends a long pulse to get the attention of a slow slave device.

  Description:
    The start byte can be used to send a long pulse to get the attention of a
    slow slave device.
*/

#define I2C_START_BYTE                 0x01


// *****************************************************************************
/* I2C 10-Bit Address Reserved Slave Addresses

  Summary:
    Forms a 10-bit address.

  Description:
	This constant is used by the PLIB_I2C_INITIALIZE_10_BIT_ADDRESS macro to form
	a 10-bit address so that it can be correctly transmitted and recognized
	as a 2-byte sequence.
*/

#define I2C_10_BIT_ADDRESS_CONST_BITS  0x1E


// *****************************************************************************
// *****************************************************************************
// Section: I2C Addressing Macros
// *****************************************************************************
// *****************************************************************************

// *****************************************************************************
/* Macro:
    void PLIB_I2C_ADDRESS_7_BIT_FORMAT( variable, address, read )

  Summary:
    Simplifies the process of initializing a I2C_7_BIT_ADDRESS variable
    with a correctly formatted 7-bit I2C slave address.

  Description:
    This macro simplifies the process of initializing a I2C_7_BIT_ADDRESS
    variable with a correctly formatted 7-bit I2C slave address.

  Precondition:
    None.

  Parameters:
    variable    - The variable to be initialized (by direct reference)

    address     - The 7-bit slave address, right-aligned

    read        - One bit, indicating the desired Read/Write operation

                  * I2C_READ    - if a Read operation is desired
                  * I2C_WRITE   - if a Write operation is desired

  Returns:
    None.

  Example:
    <code>
    #define MY_I2C_ID I2C_ID_1
    #define MY_SLAVE_ADDRESS_7_BIT 0x08

    I2C_7_BIT_ADDRESS slave7BitAddr;

    PLIB_I2C_ADDRESS_7_BIT_FORMAT(slave7BitAddr, MY_SLAVE_ADDRESS_7_BIT,
    	I2C_READ);
    if (PLIB_I2C_TransmitterIsReady(MY_I2C_ID))
    {
        PLIB_I2C_TransmitterByteSend( MY_I2C_ID,
        	PLIB_I2C_ADDRESS_7_BIT_BYTE_ACCESS(slave7BitAddr) );
    }
    </code>

  Remarks:
    This macro directly references the variable by name, so the first parameter
    must resolve to a valid C language L-value.

	The I2C_ID_1 macro in the example above is used as a placeholder for the
	actual I2C bus ID (as defined by the processor-specific I2C_MODULE_ID enum).
*/

#define PLIB_I2C_ADDRESS_7_BIT_FORMAT(v,a,r) ( (v).bits.address=(a), (v).bits.rw=(r) )


// *****************************************************************************
/* Macro:
    void PLIB_I2C_ADDRESS_7_BIT_BYTE_ACCESS( variable )

  Summary:
    Provides access to the byte value of a I2C_7_BIT_ADDRESS variable.

  Description:
    This macro provides access to the byte value of a I2C_7_BIT_ADDRESS
    variable.

  Precondition:
    The variable must have been appropriately initialized using the
    PLIB_I2C_ADDRESS_7_BIT_FORMAT macro or equivalent.

  Parameters:
    variable    - The I2C_7_BIT_ADDRESS variable containing the desired
    			  address byte value (accessed by direct reference)

  Returns:
    The I2C address byte value (as transferred) from the correctly initialized
    7-bit I2C slave address variable.

  Example:
    <code>
    #define MY_I2C_ID I2C_ID_1
    #define MY_SLAVE_ADDRESS_7_BIT 0x08

    I2C_7_BIT_ADDRESS slave7_BitAddress;

    slave7_BitAddress.byte = MY_SLAVE_ADDRESS_7_BIT;


    PLIB_I2C_ADDRESS_7_BIT_FORMAT(slave7_BitAddress,
    	MY_SLAVE_ADDRESS_7_BIT, I2C_READ);

    if (PLIB_I2C_TransmitterIsReady(MY_I2C_ID))
    {
		PLIB_I2C_TransmitterByteSend( MY_I2C_ID,
		PLIB_I2C_ADDRESS_7_BIT_BYTE_ACCESS(slave7_BitAddress) );
    }
    </code>

  Remarks:
    This macro directly references the variable by name, so the first parameter
    must resolve to a valid C language L-value.

    The I2C_ID_1 macro in the example above is used as a placeholder for the
    actual I2C bus ID (as defined by the processor-specific I2C_MODULE_ID enum).
*/

#define PLIB_I2C_ADDRESS_7_BIT_BYTE_ACCESS(v) ( (v).byte )


// *****************************************************************************
/* Macro:
    void I2C_ADDRESS_10_BIT_FORMAT( I2C_10_BIT_ADDRESS variable,
                                         uint16_t address, bool read )

  Summary:
    Simplifies the process of initializing a I2C_10_BIT_ADDRESS variable
    with a correctly formatted 10-bit I2C slave address.

  Description:
    This macro simplifies the process of initializing a I2C_10_BIT_ADDRESS
    variable with a correctly formatted 10-bit I2C slave address.

  Precondition:
    None.

  Parameters:
    variable    - The variable to be initialized (by direct reference)

    address     - The 10-bit slave address, right-aligned

    read        - One bit, indicating the desired Read/Write operation

                  * I2C_READ    - if a Read operation is desired
                  * I2C_WRITE   - if a Write operation is desired

  Returns:
    None.

  Example:
    <code>
    #define MY_I2C_ID I2C_ID_1
    #define MY_SLAVE_ADDRESS_10_BIT 0xF5
    I2C_10_BIT_ADDRESS slave10Bit_address;

    I2C_ADDRESS_10_BIT_FORMAT(slave10Bit_address,
    	MY_SLAVE_ADDRESS_10_BIT, I2C_READ);

    if (PLIB_I2C_TransmitterIsReady(MY_I2C_ID))
    {
        PLIB_I2C_TransmitterByteSend( MY_I2C_ID,
        PLIB_I2C_ADDRESS_10_BIT_1ST_BYTE_ACCESS(slave10Bit_address) );
    }
    </code>

  Remarks:
    This macro directly references the variable by name, so the first parameter
    must resolve to a valid C language L-value.

    The MY_I2C_ID macro in the example above is used as a placeholder for the
    actual I2C bus ID (as defined by the processor-specific I2C_MODULE_ID enum).
*/

#define I2C_ADDRESS_10_BIT_FORMAT(v,a,r)  	( (v).bits.low_bits   = (a) & 0x00FF,  \
                        (v).bits.const_bits = I2C_10_BIT_ADDRESS_CONST_BITS, \
                        (v.bits.high_bits  = ((a) & 0x0300) >> 8,                \
                        (v).bits.rw         = (r) ))


// *****************************************************************************
/* Macro:
    uint8_t PLIB_I2C_ADDRESS_10_BIT_1ST_BYTE_ACCESS( variable )

  Summary:
    Provides access to the value of the first byte of a I2C_7_BIT_ADDRESS
    variable.

  Description:
    This macro provides access to the value of the first byte of a
    I2C_7_BIT_ADDRESS variable.

  Precondition:
    The I2C_10_BIT_ADDRESS variable passed by reference must have been
    properly initialized by the I2C_ADDRESS_10_BIT_FORMAT macro or equivalent.

  Parameters:
    variable    - The I2C_7_BIT_ADDRESS variable containing the desired
    			  address byte value (accessed by direct reference)

  Returns:
    The first byte to be transmitted of the correctly formed 10-bit I2C slave
    address.

  Example:
    <code>
	#define MY_I2C_ID I2C_ID_1
	#define MY_SLAVE_ADDRESS_10_BIT 0xF5

    I2C_10_BIT_ADDRESS slave10BitAddress;

    I2C_ADDRESS_10_BIT_FORMAT(slave10BitAddress,
    	MY_SLAVE_ADDRESS_10_BIT, I2C_READ);

    if (PLIB_I2C_TransmitterIsReady(MY_I2C_ID))
    {
        PLIB_I2C_TransmitterByteSend( MY_I2C_ID,
        	PLIB_I2C_ADDRESS_10_BIT_1ST_BYTE_ACCESS(slave10BitAddress) );
    }
	if (PLIB_I2C_TransmitterIsReady(MY_I2C_ID))
	{
		PLIB_I2C_TransmitterByteSend( MY_I2C_ID,
			PLIB_I2C_ADDRESS_10_BIT_2ND_BYTE_ACCESS(slave10BitAddress) );
	}
    </code>

  Remarks:
    This macro directly references the variable by name, so the first parameter
    must resolve to a valid C language L-value.

    The I2C_ID_1 macro in the example above is used as a placeholder for the
    actual I2C bus ID (as defined by the processor-specific I2C_MODULE_ID enum).
*/

#define PLIB_I2C_ADDRESS_10_BIT_1ST_BYTE_ACCESS(v) ( (v).bytes.first_byte )


// *****************************************************************************
/* Macro:
    uint8_t PLIB_I2C_ADDRESS_10_BIT_2ND_BYTE_ACCESS( variable )

  Summary:
    Provides access to the value of the second byte of a I2C_7_BIT_ADDRESS
    variable.

  Description:
    This macro provides access to the value of the second byte of a
    I2C_7_BIT_ADDRESS variable.


  Conditions:
    The I2C_10_BIT_ADDRESS variable passed by reference must have been
    properly initialized by the I2C_ADDRESS_10_BIT_FORMAT macro or
    equivalent.

  Input:
    variable -  The I2C_10_BIT_ADDRESS variable containing the desired
                address byte value (accessed by direct reference)
  Return:
    The second byte to be transmitted of the correctly formed 10-bit I2C
    slave address.

  Example:
    <code>
	#define MY_I2C_ID I2C_ID_1
	#define MY_SLAVE_ADDRESS_10_BIT 0xF5

    I2C_10_BIT_ADDRESS slaveAddress10Bit;

    I2C_ADDRESS_10_BIT_FORMAT(slaveAddress10Bit,
    	MY_SLAVE_ADDRESS_10_BIT, I2C_READ);

	if (PLIB_I2C_TransmitterIsReady(MY_I2C_ID))
	    {
	        PLIB_I2C_TransmitterByteSend( MY_I2C_ID,
	        	PLIB_I2C_ADDRESS_10_BIT_1ST_BYTE_ACCESS(slaveAddress10Bit) );
    }

    if (PLIB_I2C_TransmitterIsReady(MY_I2C_ID))
    {
		PLIB_I2C_TransmitterByteSend( MY_I2C_ID,
		PLIB_I2C_ADDRESS_10_BIT_2ND_BYTE_ACCESS(slaveAddress10Bit) );
    }
    </code>

  Remarks:
    This macro directly references the variable by name, so the first
    parameter must resolve to a valid C language L-value.

    The MY_I2C_ID macro in the example above is used as a placeholder for the
    actual I2C bus ID (as defined by the processor-specific I2C_MODULE_ID enum).
*/

#define PLIB_I2C_ADDRESS_10_BIT_2ND_BYTE_ACCESS(v) ( (v).bytes.second_byte )

//DOM-IGNORE-BEGIN
#ifdef __cplusplus
}
#endif
//DOM-IGNORE-END

#endif // _PLIB_I2C_HELPER_MACROS_H
/*******************************************************************************
 End of File
*/


