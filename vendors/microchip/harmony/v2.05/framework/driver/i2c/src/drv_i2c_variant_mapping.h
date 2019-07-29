/*******************************************************************************
  I2C Driver Feature Variant Implementations

  Company:
    Microchip Technology Inc.

  File Name:
    drv_i2c_variant_mapping.h

  Summary:
    I2C Driver feature variant implementations.

  Description:
    This file implements the functions which differ based on different parts
    and various implementations of the same feature.
*******************************************************************************/

//DOM-IGNORE-BEGIN
/*******************************************************************************
Copyright (c) 2013 released Microchip Technology Inc.  All rights reserved.

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
//DOM-IGNORE-END

#ifndef _DRV_I2C_VARIANT_MAPPING_H
#define _DRV_I2C_VARIANT_MAPPING_H


#include "osal/osal.h"

// *****************************************************************************
// *****************************************************************************
// Section: Feature Variant Mapping
// *****************************************************************************
// *****************************************************************************
/* Some variants are determined by hardware feature existence, some features
   are determined user configuration of the driver, and some variants are
   a combination of the two.
*/

// *****************************************************************************

#if defined(DRV_I2C_PERIPHERAL_ID)

    #define _DRV_I2C_PERIPHERAL_ID_GET(plibId)          DRV_I2C_PERIPHERAL_ID
    #define _DRV_I2C_STATIC_PLIB_ID(any)

#else

    #define _DRV_I2C_PERIPHERAL_ID_GET(plibId)          plibId
    #define _DRV_I2C_STATIC_PLIB_ID(any)                any

#endif

#if defined(DRV_I2C_INTERRUPT_SOURCE)

    /* Static access macros */
    #define _DRV_I2C_INT_SRC_GET(source)                DRV_I2C_INTERRUPT_SOURCE
    #define _DRV_I2C_STATIC_INT_SRC(any)

#else

    /* Dynamic access macros */
    #define _DRV_I2C_INT_SRC_GET(source)                source
    #define _DRV_I2C_STATIC_INT_SRC(any)                any

#endif

#if defined(DRV_I2C_CLIENT_SPECIFIC_CONTROL)
	#define _DRV_I2C_CLIENT_SWITCH_CLIENT()\
		if(dObj->lastClientHandle != lObj->clientHandle) \
		{ \
			_DRV_I2C_ClientHardwareSetup(lObj->clientHandle);\
			dObj->lastClientHandle = lObj->clientHandle; \
		}
        #define _DRV_I2C_SAVE_LAST_CLIENT()\
            {\
                dObj = clientObj->driverObject;\
                gDrvI2CObj[dObj].lastClientHandle = DRV_I2C_CLIENTS_NUMBER + 1 ;\
            }
#else
	#define _DRV_I2C_CLIENT_SWITCH_CLIENT()
        #define _DRV_I2C_SAVE_LAST_CLIENT()
#endif

// *****************************************************************************
/* Interrupt Source Control

  Summary:
    Macros to enable, disable or clear the interrupt source.

  Description:
    These macros enable, disable, or clear the interrupt source.

    The macros get mapped to the respective SYS module APIs if the configuration
    option DRV_I2C_INTERRUPT_MODE is set to true.

  Remarks:
    These macros are mandatory.
*/

#if defined (DRV_I2C_INTERRUPT_MODE) && (DRV_I2C_INTERRUPT_MODE == true)

#define _DRV_I2C_InterruptSourceIsEnabled(source)       SYS_INT_SourceIsEnabled(source)
#define _DRV_I2C_InterruptSourceEnable(source)          SYS_INT_SourceEnable(source)
#define _DRV_I2C_InterruptSourceDisable(source)         SYS_INT_SourceDisable(source)
#define _DRV_I2C_InterruptSourceStatusClear(source)     SYS_INT_SourceStatusClear(source)

#define _DRV_I2C_InterruptSourceClear(source)           SYS_INT_SourceStatusClear(source)

#define _DRV_I2C_InterruptSourceStatusGet(source)       SYS_INT_SourceStatusGet(source)
#define _DRV_I2C_InterruptSourceStatusSet(source)       SYS_INT_SourceStatusSet(source)

#elif defined (DRV_I2C_INTERRUPT_MODE) && (DRV_I2C_INTERRUPT_MODE == false)

#define _DRV_I2C_InterruptSourceIsEnabled(source)       false
#define _DRV_I2C_InterruptSourceEnable(source)
#define _DRV_I2C_InterruptSourceDisable(source)         false
#define _DRV_I2C_InterruptSourceStatusClear(source)     SYS_INT_SourceStatusClear( source )

#define _DRV_I2C_InterruptSourceClear(source)           SYS_INT_SourceStatusClear( source )

#define _DRV_I2C_InterruptSourceStatusGet(source)       SYS_INT_SourceStatusGet( source )
#define _DRV_I2C_InterruptSourceStatusSet(source)       SYS_INT_SourceStatusSet( source )

#else

    #error "No Task mode chosen at build, interrupt or polling needs to be selected. "

#endif


// *****************************************************************************
// *****************************************************************************
// Initialization Parameter Static Overrides
// *****************************************************************************
// *****************************************************************************


// *****************************************************************************
/* Power mode Configuration Override

  Summary:
    Allows static override of the power mode.

  Description:
    These macros allow the power mode to be statically overridden by
    the DRV_I2C_POWER_STATE configuration macro, if it is defined.

    _DRV_I2C_POWER_STATE_GET replaces the value passed in with the value defined by
    the DRV_I2C_POWER_STATE configuration option.

   Remarks:
    None.
*/

#if defined(DRV_I2C_POWER_STATE)

    #define _DRV_I2C_POWER_STATE_GET(arg)               DRV_I2C_POWER_STATE

#else

    #define _DRV_I2C_POWER_STATE_GET(arg)               (arg)

#endif

// *****************************************************************************
/* Baud rate value Configuration Override

  Summary:
    Allows static override of the baud rate value.

  Description:
    These macros allow the baud rate value to be statically overridden by
    the DRV_I2C_BAUD_RATE_VALUE configuration macro, if it is defined.

    _DRV_I2C_BAUD_RATE_VALUE_GET replaces the value passed in with the value
    defined by the DRV_I2C_BAUD_RATE_VALUE configuration option.

   Remarks:
    None.
*/

#if defined(DRV_I2C_BAUD_RATE_VALUE)

    #define _DRV_I2C_BAUD_RATE_VALUE_GET(arg)           DRV_I2C_BAUD_RATE_VALUE

#else

    #define _DRV_I2C_BAUD_RATE_VALUE_GET(arg)           (arg)

#endif


// *****************************************************************************
/* I2C clock operation mode Configuration Override

  Summary:
    Allows static override of the I2C clock operation mode.

  Description:
    These macros allow the I2C clock operation mode to be statically overridden by
    the DRV_I2C_CLOCK_OPERATION_MODE configuration macro, if it is defined.

    _DRV_I2C_CLOCK_OPERATION_MODE_GET replaces the value passed in with the value
    defined by the DRV_I2C_CLOCK_OPERATION_MODE configuration option.

   Remarks:
    None.
*/

#if defined(DRV_I2C_OPERATION_MODE)

    #define _DRV_I2C_CLOCK_OPERATION_MODE_GET(arg)      DRV_I2C_CLOCK_OPERATION_MODE

#else

    #define _DRV_I2C_OPERATION_MODE_GET(arg)      (arg)

#endif


#if defined(DRV_I2C_BUS_SPEED)

    #define _DRV_I2C_BUS_SPEED_GET(arg)      DRV_I2C_BUS_SPEED

#else

    #define _DRV_I2C_BUS_SPEED_GET(arg)      (arg)

#endif

#if defined(DRV_I2C_SMBus_LEVEL)

    #define _DRV_DRV_I2C_SMBus_LEVEL_GET(arg)      DRV_I2C_SMBus_LEVEL

#else

    #define _DRV_I2C_SMBus_LEVEL_GET(arg)      (arg)

#endif

#if defined(DRV_I2C_ADDRESS_WIDTH)

    #define _DRV_I2C_ADDRESS_WIDTH_GET(arg)      _DRV_I2C_ADDRESS_WIDTH

#else

    #define _DRV_I2C_ADDRESS_WIDTH_GET(arg)      (arg)

#endif


#if defined (I2C_SLAVE_ADDRESS_7bit)

    #define I2C_SLAVE_ADDRESS_7bit_GET(arg)         _I2C_SLAVE_ADDRESS_7bit

#else

    #define I2C_SLAVE_ADDRESS_7bit_GET(arg)         (arg)

#endif

#if defined (DRV_I2C_RESERVED_ADDRESS)

    #define _DRV_I2C_RESERVED_ADDRESS_EN_GET(arg)   DRV_I2C_RESERVED_ADDRESS

#else

    #define _DRV_I2C_RESERVED_ADDRESS_EN_GET(arg)   (arg)

#endif

#if defined (DRV_I2C_RECEIVE_ENABLE)

    #define _DRV_I2C_RECEIVE_ENABLE (arg)           &DRV_MASTER_SEND_RX_ENABLE

#else

    #define _DRV_I2C_RECEIVE_ENABLE(arg)                 &arg

#endif

#if defined (DRV_I2C_MODULE_STATE_GET)

    #define _DRV_I2C_MODULE_STATE_GET (arg)           DRV_I2C_MODULE_STATE_GET

#else

    #define _DRV_I2C_MODULE_STATE_GET(arg)                 arg

#endif



#endif //_DRV_I2C_VARIANT_MAPPING_H
/*******************************************************************************
 End of File
*/

