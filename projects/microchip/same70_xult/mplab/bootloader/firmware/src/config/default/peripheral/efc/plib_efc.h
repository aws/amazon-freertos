/*******************************************************************************
 Interface definition of EFC PLIB.

 Company:
    Microchip Technology Inc.

 File Name:
    plib_efc.h

 Summary:
    Interface definition of EFC Plib.

 Description:
    This file defines the interface for the EFC Plib.
    It allows user to Program, Erase and lock the on-chip FLASH memory.
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

#ifndef EFC_H    // Guards against multiple inclusion
#define EFC_H

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

// DOM-IGNORE-BEGIN
#ifdef __cplusplus // Provide C++ Compatibility
    extern "C" {
#endif
// DOM-IGNORE-END

// *****************************************************************************
// *****************************************************************************
// Section: Interface
// *****************************************************************************
// *****************************************************************************

#define EFC_SECTORSIZE              8192
#define EFC_PAGESIZE                512
#define EFC_LOCKSIZE                0x4000


typedef enum
{
    EFC_ERROR_NONE = 0x1,
    /*In-valid command*/
    EFC_CMD_ERROR = 0x2,
    /*Flash region is locked*/
    EFC_LOCK_ERROR = 0x4,
    /*Flash Error*/
    EFC_FLERR_ERROR = 0x8,
    /*Flash Encountered an ECC error*/
    EFC_ECC_ERROR = 0xF0000,
} EFC_ERROR;


void EFC_Initialize(void);

bool EFC_Read( uint32_t *data, uint32_t length, uint32_t address );

bool EFC_SectorErase( uint32_t address );

bool EFC_PageWrite( uint32_t *data, uint32_t address );

bool EFC_QuadWordWrite( uint32_t *data, uint32_t address );

EFC_ERROR EFC_ErrorGet( void );

bool EFC_IsBusy(void);

void EFC_RegionLock(uint32_t address);

void EFC_RegionUnlock(uint32_t address);


// DOM-IGNORE-BEGIN
#ifdef __cplusplus // Provide C++ Compatibility
}
#endif
// DOM-IGNORE-END

#endif
