/*******************************************************************************
  Interrupt System Service Library Interface Implementation File

  Company
    Microchip Technology Inc.

  File Name
    sys_int_nvic.c

  Summary
    NVIC implementation of interrupt system service library.

  Description
    This file implements the interface to the interrupt system service library
    not provided in CMSIS.

  Remarks:
    None.

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


// *****************************************************************************
// *****************************************************************************
// Section: Included Files
// *****************************************************************************
// *****************************************************************************
#include "system/int/sys_int.h"


// *****************************************************************************
// *****************************************************************************
// Section: Interface Implementation
// *****************************************************************************
// *****************************************************************************

// *****************************************************************************
void SYS_INT_Enable( void )
{
    __DMB();
    __enable_irq();

    return;
}


// *****************************************************************************
bool SYS_INT_Disable( void )
{
    bool processorStatus;

    processorStatus = (bool) (__get_PRIMASK() == 0);

    __disable_irq();
    __DMB();

    return processorStatus;
}


// *****************************************************************************
void SYS_INT_Restore( bool state )
{
    if( state == true )
    {
        __DMB();
        __enable_irq();
    }
    else
    {
        __disable_irq();
        __DMB();
    }

    return;
}

bool SYS_INT_SourceDisable( INT_SOURCE source )
{
    bool processorStatus;
    bool intSrcStatus;

    processorStatus = SYS_INT_Disable();

    intSrcStatus = NVIC_GetEnableIRQ(source);

    NVIC_DisableIRQ( source );

    SYS_INT_Restore( processorStatus );

    /* return the source status */
    return intSrcStatus;
}

void SYS_INT_SourceRestore( INT_SOURCE source, bool status )
{
    if( status ) {
        SYS_INT_SourceEnable( source );
    }
    return;
}
