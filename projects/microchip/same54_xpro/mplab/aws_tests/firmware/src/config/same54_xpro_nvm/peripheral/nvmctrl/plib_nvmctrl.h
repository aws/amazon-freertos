/*******************************************************************************
  Non-Volatile Memory Controller(NVMCTRL) PLIB.

  Company:
    Microchip Technology Inc.

  File Name:
    plib_nvmctrl.h

  Summary:
    Interface definition of NVMCTRL Plib.

  Description:
    This file defines the interface for the NVMCTRL Plib.
    It allows user to Program, Erase and lock the on-chip Non Volatile Flash
    Memory.
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

#ifndef PLIB_NVMCTRL_H
#define PLIB_NVMCTRL_H

// *****************************************************************************
// *****************************************************************************
// Section: Included Files
// *****************************************************************************
// *****************************************************************************

#include "device.h"
#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

// DOM-IGNORE-BEGIN
#ifdef __cplusplus // Provide C++ Compatibility
 extern "C" {
#endif

// DOM-IGNORE-END

#define NVMCTRL_FLASH_START_ADDRESS        (0U)
#define NVMCTRL_FLASH_SIZE                 (1048576U)
#define NVMCTRL_FLASH_PAGESIZE             (512U)
#define NVMCTRL_FLASH_BLOCKSIZE            (8192U)



/* NVM supports four write modes */
typedef enum
{
    NVMCTRL_WMODE_MAN = NVMCTRL_CTRLA_WMODE_MAN,
    NVMCTRL_WMODE_ADW = NVMCTRL_CTRLA_WMODE_ADW,
    NVMCTRL_WMODE_AQW = NVMCTRL_CTRLA_WMODE_AQW,
    NVMCTRL_WMODE_AP = NVMCTRL_CTRLA_WMODE_AP,
} NVMCTRL_WRITEMODE;



void NVMCTRL_Initialize(void);

bool NVMCTRL_Read( uint32_t *data, uint32_t length, uint32_t address );

void NVMCTRL_SetWriteMode(NVMCTRL_WRITEMODE mode);

bool NVMCTRL_QuadWordWrite(const uint32_t *data, const uint32_t address);

bool NVMCTRL_DoubleWordWrite(const uint32_t *data, const uint32_t address);

bool NVMCTRL_PageWrite( const uint32_t* data, uint32_t address );

bool NVMCTRL_BlockErase( uint32_t address );

uint16_t NVMCTRL_ErrorGet( void );

uint16_t NVMCTRL_StatusGet( void );

bool NVMCTRL_IsBusy( void );

void NVMCTRL_RegionLock (uint32_t address);

void NVMCTRL_RegionUnlock (uint32_t address);

uint32_t NVMCTRL_RegionLockStatusGet (void);

bool NVMCTRL_SmartEEPROM_IsBusy(void);

uint32_t NVMCTRL_SmartEEPROMStatusGet( void );

bool NVMCTRL_SmartEEPROM_IsActiveSectorFull(void);

void NVMCTRL_SmartEEPROMSectorReallocate(void);

void NVMCTRL_SmartEEPROMFlushPageBuffer(void);

void NVMCTRL_BankSwap(void);


// DOM-IGNORE-BEGIN
#ifdef __cplusplus // Provide C++ Compatibility
}
#endif
// DOM-IGNORE-END
#endif // PLIB_NVMCTRL_H
