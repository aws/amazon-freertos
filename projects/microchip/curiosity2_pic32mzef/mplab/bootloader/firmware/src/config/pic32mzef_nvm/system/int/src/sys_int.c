/*******************************************************************************
  Interrupt System Service

  Company:
    Microchip Technology Inc.

  File Name:
    sys_int.c

  Summary:
    Interrupt System Service APIs.

  Description:
    This file contains functions related to the Interrupt System Service for PIC32
    devices.
*******************************************************************************/

// DOM-IGNORE-BEGIN
/*******************************************************************************
* Copyright (C) 2019 Microchip Technology Inc. and its subsidiaries.
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

// *****************************************************************************
// *****************************************************************************
// Section: Included Files
// *****************************************************************************
// *****************************************************************************

#include "system/int/sys_int.h"

// *****************************************************************************
// *****************************************************************************
// Section: Function Definitions
// *****************************************************************************
// *****************************************************************************

void SYS_INT_Enable( void )
{
    __builtin_enable_interrupts();
}

bool SYS_INT_Disable( void )
{
    uint32_t processorStatus;

    /* Save the processor status and then Disable the global interrupt */
    processorStatus = ( uint32_t )__builtin_disable_interrupts();

    /* return the interrupt status */
    return (bool)(processorStatus & 0x01);
}

void SYS_INT_Restore( bool state )
{
    if (state)
    {
        /* restore the state of CP0 Status register before the disable occurred */
        __builtin_enable_interrupts();
    }
}

bool SYS_INT_SourceDisable( INT_SOURCE source )
{
    bool intSrcStatus;
    bool interruptStatus;

    /* Save the interrupt status and then Disable the global interrupt */
    interruptStatus = (bool)(( uint32_t )__builtin_disable_interrupts() & 0x01);

    /* get the interrupt status of this source before disable is called */
    intSrcStatus = SYS_INT_SourceIsEnabled(source);

    /* disable the interrupts */
    EVIC_SourceDisable(source);

    SYS_INT_Restore(interruptStatus);

    /* return the source status */
    return intSrcStatus;
}

void SYS_INT_SourceRestore( INT_SOURCE source, bool status )
{
    if( status )
    {
        SYS_INT_SourceEnable( source );
    }
}