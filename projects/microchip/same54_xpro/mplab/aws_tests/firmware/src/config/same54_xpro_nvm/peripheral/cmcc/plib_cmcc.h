/*******************************************************************************
  Interface definition of CMCC PLIB.

  Company:
    Microchip Technology Inc.

  File Name:
    plib_cmcc.h

  Summary:
    Interface definition of the CMCC(Cortex M Cache Controller) Peripheral Library

  Description:
    This file defines the interface for the CMCC Plib.
*******************************************************************************/

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

#ifndef PLIB_CMCC_H    // Guards against multiple inclusion
#define PLIB_CMCC_H


#ifdef __cplusplus // Provide C++ Compatibility
	extern "C" {
#endif


// *****************************************************************************
// *****************************************************************************
// Section: Interface
// *****************************************************************************
// *****************************************************************************

#define CMCC_NO_OF_WAYS     4
#define CMCC_LINE_PER_WAY   64
#define CMCC_LINE_SIZE      16
#define CMCC_WAY_SIZE       1024

/***************************** CMCC API *******************************/
void CMCC_Disable (void );
void CMCC_EnableDCache (void );
void CMCC_DisableDCache (void );

void CMCC_EnableICache (void );
void CMCC_DisableICache (void );

void CMCC_InvalidateAll (void );

#endif