/*******************************************************************************
  TRNG Peripheral Library

  Company:
    Microchip Technology Inc.

  File Name:
    plib_trng.c

  Summary:
    TRNG Source File

  Description:
    None

*******************************************************************************/

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


#include "device.h"
#include "plib_trng.h"


void TRNG_Initialize( void )
{}

uint32_t TRNG_ReadData( void )
{
	TRNG_REGS->TRNG_CTRLA |= TRNG_CTRLA_ENABLE_Msk;
	while(((TRNG_REGS->TRNG_INTFLAG) & (TRNG_INTFLAG_DATARDY_Msk)) != TRNG_INTFLAG_DATARDY_Msk);			
	TRNG_REGS->TRNG_CTRLA &= ~(TRNG_CTRLA_ENABLE_Msk);
	return (TRNG_REGS->TRNG_DATA);
}


