/*******************************************************************************
  Non-Volatile Memory Controller(NVM) PLIB.

  Company:
    Microchip Technology Inc.

  File Name:
    plib_nvm.h

  Summary:
    Interface definition of NVM Plib.

  Description:
    This file defines the interface for the NVM Plib.
    It allows user to Program, Erase and lock the on-chip Non Volatile Flash
    Memory.

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

#ifndef PLIB_NVM_H
#define PLIB_NVM_H

// *****************************************************************************
// *****************************************************************************
// Section: Included Files
// *****************************************************************************
// *****************************************************************************

#include "device.h"     // For device registers and uint32_t
#include <stdbool.h>    // For bool

// DOM-IGNORE-BEGIN
#ifdef __cplusplus // Provide C++ Compatibility
 extern "C" {
#endif

// DOM-IGNORE-END

#define NVM_FLASH_START_ADDRESS    (0x9d000000U)
#define NVM_FLASH_SIZE             (0x200000U)
#define NVM_FLASH_ROWSIZE          (2048U)
#define NVM_FLASH_PAGESIZE         (16384U)


typedef enum
{
    /* No error */
    NVM_ERROR_NONE = 0x0,

    /* NVM write error */
    NVM_ERROR_WRITE = _NVMCON_WRERR_MASK,

    /* NVM Low Voltage Detect error */
    NVM_ERROR_LOWVOLTAGE = _NVMCON_LVDERR_MASK,

} NVM_ERROR;

typedef enum
{
    /* Upper Boot Flash Page 0 Write Protect */
    NVM_UPPER_BOOT_WRITE_PROTECT_0 = _NVMBWP_UBWP0_MASK,

    /* Upper Boot Flash Page 1 Write Protect */
    NVM_UPPER_BOOT_WRITE_PROTECT_1 = _NVMBWP_UBWP1_MASK,

    /* Upper Boot Flash Page 2 Write Protect */
    NVM_UPPER_BOOT_WRITE_PROTECT_2 = _NVMBWP_UBWP2_MASK,

    /* Upper Boot Flash Page 3 Write Protect */
    NVM_UPPER_BOOT_WRITE_PROTECT_3 = _NVMBWP_UBWP3_MASK,

    /* Lower Boot Flash Page 4 Write Protect */
    NVM_UPPER_BOOT_WRITE_PROTECT_4 = _NVMBWP_UBWP4_MASK,

    /* Lower Boot Flash Page 0 Write Protect */
    NVM_LOWER_BOOT_WRITE_PROTECT_0 = _NVMBWP_LBWP0_MASK,

    /* Lower Boot Flash Page 1 Write Protect */
    NVM_LOWER_BOOT_WRITE_PROTECT_1 = _NVMBWP_LBWP1_MASK,

    /* Lower Boot Flash Page 2 Write Protect */
    NVM_LOWER_BOOT_WRITE_PROTECT_2 = _NVMBWP_LBWP2_MASK,

    /* Lower Boot Flash Page 3 Write Protect */
    NVM_LOWER_BOOT_WRITE_PROTECT_3 = _NVMBWP_LBWP3_MASK,

    /* Lower Boot Flash Page 4 Write Protect */
    NVM_LOWER_BOOT_WRITE_PROTECT_4 = _NVMBWP_LBWP4_MASK,

} NVM_BOOT_FLASH_WRITE_PROTECT;

typedef enum
{
    /* Lock the Upper Boot Write Protect pages */
    NVM_UPPER_BOOT_WRITE_PROTECT_LOCK = _NVMBWP_UBWPULOCK_MASK,

    /* Lock the Upper Boot Write Protect pages */
    NVM_LOWER_BOOT_WRITE_PROTECT_LOCK = _NVMBWP_LBWPULOCK_MASK,

} NVM_BOOT_FLASH_WRITE_PROTECT_LOCK;

typedef void (*NVM_CALLBACK)(uintptr_t context);

bool NVM_Read( uint32_t *data, uint32_t length, const uint32_t address );

bool NVM_WordWrite(uint32_t data, uint32_t address);

bool NVM_QuadWordWrite(uint32_t *data, uint32_t address);

bool NVM_RowWrite(uint32_t *data, uint32_t address);

bool NVM_PageErase(uint32_t address);

NVM_ERROR NVM_ErrorGet( void );

bool NVM_IsBusy( void );

void NVM_ProgramFlashSwapBank( void );

void NVM_ProgramFlashWriteProtect( uint32_t address);

void NVM_ProgramFlashWriteProtectLock( void );

void NVM_BootFlashWriteProtectEnable( NVM_BOOT_FLASH_WRITE_PROTECT writeProtectPage );

void NVM_BootFlashWriteProtectDisable( NVM_BOOT_FLASH_WRITE_PROTECT writeProtectPage );

void NVM_BootFlashWriteProtectLock( NVM_BOOT_FLASH_WRITE_PROTECT_LOCK writeProtectLock );

void NVM_CallbackRegister ( NVM_CALLBACK callback, uintptr_t context );

// DOM-IGNORE-BEGIN
#ifdef __cplusplus // Provide C++ Compatibility
}
#endif

// DOM-IGNORE-END
#endif // PLIB_NVM_H
