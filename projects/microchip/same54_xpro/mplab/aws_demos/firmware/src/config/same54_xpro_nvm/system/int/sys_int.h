/*******************************************************************************
  Interrupt System Service Library Interface Header File

  Company
    Microchip Technology Inc.

  File Name
    sys_int.h

  Summary
    Interrupt system service library interface.

  Description
    This file defines the interface to the interrupt system service library.
    This library provides access to and control of the interrupt controller.

  Remarks:
    Interrupt controller initialize will be done from within the MCC.

*******************************************************************************/

// DOM-IGNORE-BEGIN
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
// DOM-IGNORE-END

#ifndef SYS_INT_H    // Guards against multiple inclusion
#define SYS_INT_H


// *****************************************************************************
// *****************************************************************************
// Section: Included Files
// *****************************************************************************
// *****************************************************************************

/*  This section lists the other files that are included in this file.
*/

#include <stdbool.h>
#include "device.h"

// DOM-IGNORE-BEGIN
#ifdef __cplusplus  // Provide C++ Compatibility

    extern "C" {

#endif
// DOM-IGNORE-END


// *****************************************************************************
// *****************************************************************************
// Section: Data Types
// *****************************************************************************
// *****************************************************************************

// *****************************************************************************
/* Interrupt Sources

   Summary:
    Defines the data type for all the interrupt sources associated with the
       interrupt controller of the device.

   Description:
    This data type can be used with interface functions to enable, disable,
       set, clear and to get status of any particular interrupt source.

   Remarks:
    This data type is defined using the CMSIS data type that defines the
    interrupt sources set available.
*/

typedef IRQn_Type INT_SOURCE;


// *****************************************************************************
// *****************************************************************************
// Section: Interface Routines
// *****************************************************************************
// *****************************************************************************
/* The following functions make up the methods (set of possible operations) of
   this interface.
*/

// *****************************************************************************
/* Function:
    void SYS_INT_Enable( void )

   Summary:
    Enable Global Interrupt

   Description:
    This function enables global interrupt.

   Precondition:
    None

   Parameters:
    None.

   Returns:
    None.

  Example:
    <code>
    SYS_INT_Enable();
    </code>

  Remarks:
    None.
*/

void SYS_INT_Enable( void );


// *****************************************************************************
/* Function:
    bool SYS_INT_Disable( void )

   Summary:
    Disable Global Interrupt

   Description:
    This function disables global interrupt and returns the state of the global
    interrupt prior to disabling it. When global interrupt is disabled, only
    NMI and HardFault exceptions are allowed. This may be used to disable global
    interrupt during critical section and restore the global interrupt state
    after the critical section.

   Precondition:
    None.

   Parameters:
    None.

   Returns:
    This function disables the global interrupt and return the state of global
    interrupt prior to disabling it. The state information will be used to
    restore the global interrupt to the original state after the critical
    section.
    - true -  Global Interrupt is enabled
    - false -  Global Interrupt is disabled

  Example:
    <code>
      bool interruptState;

      // Save global interrupt state and disable interrupt
      interruptState = SYS_INT_Disable();

      // Critical Section

      // Restore interrupt state
      SYS_INT_Restore(interruptState)
    </code>

  Remarks:
    Returned status can be passed to SYS_INT_Restore to restore the previous
    global interrupt status (whether it was enabled or disabled).
*/

bool SYS_INT_Disable( void );


// *****************************************************************************
/* Function:
    bool SYS_INT_IsEnabled( void )

   Summary:
    Returns the interrupt controller's global enable/disable status.

   Description:
    This function returns global interrupt enable status.

   Precondition:
    None.

   Parameters:
    None.

   Returns:
    - true - Global Interrupt is enabled.
    - false - Gloable Interrupt is disabled.

  Example:
    <code>
       if(true == SYS_INT_IsEnabled())
       {
             // Gloable Interrupt is enabled
       }
    </code>

  Remarks:
    None.
*/

bool SYS_INT_IsEnabled( void );


// *****************************************************************************
/* Function:
    void SYS_INT_Restore( bool state )

   Summary:
    Restores the interrupt controller to the state specificed in the parameter.

   Description:
    This function restores the interrupt controller to the state specified in the parmeters.

   Precondition:
    SYS_INT_Disable must have been called to get previous state of the global interrupt.

   Parameters:
    - true -  Enable Global Interrupt
    - false -  Disable Global Interrupt

   Returns:
    None.

  Example:
    <code>
      bool interruptState;

      // Save global interrupt state and disable interrupt
      interruptState = SYS_INT_Disable();

      // Critical Section

      // Restore interrupt state
      SYS_INT_Restore(interruptState)
    </code>

  Remarks:
    None.
*/

void SYS_INT_Restore( bool state );


// *****************************************************************************
/* Function:
    void SYS_INT_SourceEnable( INT_SOURCE source )

   Summary:
    Enables the specified interrupt source.

   Description:
    This function enables the specified interrupt source/line at the interrupt
    controller level.

   Precondition:
    None.

   Parameters:
    source - Interrupt source/line available at interrupt controller.

   Returns:
    None.

  Example:
    <code>
    SYS_INT_SourceEnable(USART0_IRQn);
    </code>

  Remarks:
    The corresponding module level interrupts must be enabled to trigger the
    specified interrupt source/line. For example, USART0 interrupt enable at
    interrupt controller level is not triggered unless USART0 module level
    interrupt for TXRDY or RXRDY or Error interrupts are not enabled.
*/

void SYS_INT_SourceEnable( INT_SOURCE source );


// *****************************************************************************
/* Function:
    bool SYS_INT_SourceDisable( INT_SOURCE source )

   Summary:
    Disables the specified interrupt source.

   Description:
    This function returns the current interrupt enable/disable status and
    disables the specified interrupt source/line at the interrupt controller
    level.

   Precondition:
    None.

   Parameters:
    source - Interrupt source/line available at interrupt controller.

   Returns:
    - true - Interrupt line was enabled.
    - false - Interrupt line was disabled.

  Example:
    <code>
        bool usart0Int = false;
        usart0Int = SYS_INT_SourceDisable(USART0_IRQn);
    </code>

  Remarks:
    If the corresponding module level interrupts are enabled and triggered,
    the triggers will be ignored at interrupt controller. For example, USART0
    doesn't cause interrupt unless both the interrupt controller source/line and
    USART0 module level interrupt for TX or RX or Error are enabled.
*/

bool SYS_INT_SourceDisable( INT_SOURCE source );


// *****************************************************************************
/* Function:
    void SYS_INT_SourceRestore( INT_SOURCE source, bool state )

   Summary:
    Restores an interrupt vector to the state specified in the parameter.

   Description:
    This function restores the interrupt vector to the state specified in the parameters.

   Precondition:
    SYS_INT_SourceDisable must have been called previously to get the state value.

   Parameters:
    - true -  Enable Interrupt Vector
    - false - Disable Interrupt Vector

   Returns:
    None.

  Example:
    <code>
      bool aState;

      // Save interrupt vector state and disable interrupt
      aState = SYS_INT_SourceDisable( aSrcId );

      // Critical Section

      // Restore interrupt vector state
      SYS_INT_SourceRestore( aSrcId, aState )
    </code>

  Remarks:
    None.
*/

void SYS_INT_SourceRestore( INT_SOURCE source, bool aState );


// *****************************************************************************
/* Function:
    bool SYS_INT_SourceIsEnabled( INT_SOURCE source )

   Summary:
    Gets the enable/disable status of the specified interrupt source.

   Description:
    This function returns the enable/disable status of the specified interrupt
    source/line at the interrupt controller level.

   Precondition:
    None.

   Parameters:
    source - Interrupt source/line available at interrupt controller.

   Returns:
    - true - Interrupt line is enabled.
    - false - Interrupt line is disabled.

  Example:
    <code>
       bool usart0Int = false;
       usart0Int = SYS_INT_SourceIsEnabled(USART0_IRQn);
    </code>

  Remarks:
    Unlike SYS_INT_Disable, this function just returns the status and doesn't
    disable the interrupt line.
*/

bool SYS_INT_SourceIsEnabled( INT_SOURCE source );


// *****************************************************************************
/* Function:
    bool SYS_INT_SourceStatusGet( INT_SOURCE source )

   Summary:
    Gets the pending status of the specified interrupt source.

   Description:
    This function returns the pending status of the specified interrupt source
    at the interrupt controller level.

   Precondition:
    None.

   Parameters:
    source - Interrupt source/line available at interrupt controller.

   Returns:
    - true - Interrupt status is pending.
    - false - Interrupt status is not pending.

  Example:
    <code>
       bool usart0IntStatus = SYS_INT_SourceStatusGet(USART0_IRQn);
    </code>

  Remarks:
    Interrupt pending status may get cleared automatically once the
    corresponding interrupt vector executes on some devices.
*/

bool SYS_INT_SourceStatusGet( INT_SOURCE source );


// *****************************************************************************
/* Function:
    void SYS_INT_SourceStatusSet( INT_SOURCE source )

   Summary:
    Sets the pending status of the specified interrupt source manually.

   Description:
    This function manually sets the pending status of the specified interrupt
    source at the interrupt controller level. This triggers interrupt controller
    for a specified source even though the interrupt condition has not met at
    hardware.

   Precondition:
    None.

   Parameters:
    source - Interrupt source/line available at interrupt controller.

   Returns:
    None.

  Example:
    <code>
       //Trigger USART0 ISR handler manually
    SYS_INT_SourceStatusSet(USART0_IRQn);
    </code>

  Remarks:
    This feature may not be supported by some devices.
*/

void SYS_INT_SourceStatusSet( INT_SOURCE source );


// *****************************************************************************
/* Function:
    void SYS_INT_SourceStatusClear( INT_SOURCE source )

   Summary:
    Clears the pending status of the specified interrupt source.

   Description:
    This function clears the pending status of the specified interrupt source
    at the interrupt controller level. It is ignored if the interrupt condition
    has already been cleared in hardware.

   Precondition:
    None.

   Parameters:
    source - Interrupt source/line available at interrupt controller.

   Returns:
    None.

  Example:
    <code>
       //Clear a pending interrupt.
       SYS_INT_SourceStatusClear(USART0_IRQn);
    </code>

  Remarks:
    None.
*/

void SYS_INT_SourceStatusClear( INT_SOURCE source );


#include "sys_int_mapping.h"

// DOM-IGNORE-BEGIN
#ifdef __cplusplus  // Provide C++ Compatibility

    }

#endif
// DOM-IGNORE-END

#endif //SYS_INT_H
