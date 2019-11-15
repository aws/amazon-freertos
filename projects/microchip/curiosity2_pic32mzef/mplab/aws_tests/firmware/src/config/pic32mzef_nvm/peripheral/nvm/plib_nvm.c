/*******************************************************************************
  Non-Volatile Memory Controller(NVM) PLIB.

  Company:
    Microchip Technology Inc.

  File Name:
    plib_nvm.c

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

// *****************************************************************************
// *****************************************************************************
// Section: Included Files
// *****************************************************************************
// *****************************************************************************

#include <string.h>
#include "sys/kmem.h"
#include "plib_nvm.h"

/* ************************************************************************** */
/* ************************************************************************** */
/* Section: File Scope or Global Data                                         */
/* ************************************************************************** */
/* ************************************************************************** */
// *****************************************************************************

/*******************************************
 * Internal operation type
 ******************************************/
typedef enum
{
    PROGRAM_ERASE_OPERATION         = 0x7,
    UPPER_PROGRAM_ERASE_OPERATION   = 0x6,
    LOWER_PROGRAM_ERASE_OPERATION   = 0x5,
    PAGE_ERASE_OPERATION            = 0x4,
    ROW_PROGRAM_OPERATION           = 0x3,
    QUAD_WORD_PROGRAM_OPERATION     = 0x2,
    WORD_PROGRAM_OPERATION          = 0x1,
    NO_OPERATION                    = 0x0,
} NVM_OPERATION_MODE;

/*******************************************
 * Internal Flash Programming Unlock Keys
 ******************************************/
typedef enum
{
    NVM_UNLOCK_KEY1 = 0xAA996655,
    NVM_UNLOCK_KEY2 = 0x556699AA
} NVM_UNLOCK_KEYS;

#define NVM_INTERRUPT_ENABLE_MASK   0x80
#define NVM_INTERRUPT_FLAG_MASK     0x80

/* ************************************************************************** */
/* ************************************************************************** */
// Section: Local Functions                                                   */
/* ************************************************************************** */
/* ************************************************************************** */
// *****************************************************************************

// *****************************************************************************
// Section: NVM Implementation
// *****************************************************************************
// *****************************************************************************

NVM_CALLBACK nvmCallbackFunc;

uintptr_t nvmContext;

void NVM_CallbackRegister( NVM_CALLBACK callback, uintptr_t context )
{
    /* Register callback function */
    nvmCallbackFunc    = callback;
    nvmContext         = context;
}

void NVM_InterruptHandler(void)
{
    IFS5CLR = NVM_INTERRUPT_FLAG_MASK;

    if(nvmCallbackFunc != NULL)
    {
        nvmCallbackFunc(nvmContext);
    }
}

static void NVM_WriteUnlockSequence( void )
{
    // Write the unlock key sequence
    NVMKEY = 0x0;
    NVMKEY = NVM_UNLOCK_KEY1;
    NVMKEY = NVM_UNLOCK_KEY2;
}

static void NVM_StartOperationAtAddress( uint32_t address,  NVM_OPERATION_MODE operation)
{
    volatile uint32_t processorStatus;

    processorStatus = __builtin_disable_interrupts();

    // Set the target Flash address to be operated on (destination).
    NVMADDR = KVA_TO_PA(address);

    // Set the flash operation:
    /***************************************************************************
     * Page erase: Erases the entire page which includes the target address
     *    (NVMADDR) if it is not write-protected.
     * Word (32-bit) program: Programs the 32 bit word in NVMDATA0 to the flash
     *    address selected by NVMADDR, if it is not write-protected.
     * Quad Word (128-bit) program: Programs the 128 bit quad words in NVMDATA0
     *    through NVMDATA3 to flash address selected by NVMADDR, if it they are
     *    not write-protected.
     * Row program: Programs the entire row from the physical address in
     *    NVMSRCADDR to the flash address selected by NVMADDR if it is not
     *    write-protected
     **************************************************************************/

    // NVMOP can be written only when WREN is zero. So, clear WREN.
    NVMCONCLR = _NVMCON_WREN_MASK;

    /* Clear and Set, as NVMCON contains status bits and hence need to be accessed atomically.
     * Using bit field access may erroneously cause status bits to get cleared */
    NVMCONCLR = _NVMCON_NVMOP_MASK;
    NVMCONSET = ( _NVMCON_NVMOP_MASK & (((uint32_t)operation) << _NVMCON_NVMOP_POSITION) );

    // Set WREN to enable writes to the WR bit and to prevent NVMOP modification
    NVMCONSET = _NVMCON_WREN_MASK;

    NVM_WriteUnlockSequence();

    // Start the operation
    NVMCONSET = _NVMCON_WR_MASK;

    __builtin_mtc0(12, 0, processorStatus);

    IEC5SET   = NVM_INTERRUPT_ENABLE_MASK;
}

/* ************************************************************************** */
/* ************************************************************************** */
// Section: Interface Functions                                               */
/* ************************************************************************** */
/* ************************************************************************** */
bool NVM_Read( uint32_t *data, uint32_t length, const uint32_t address )
{
    memcpy((void *)data, (void *)KVA0_TO_KVA1(address), length);

    return true;
}
bool NVM_WordWrite(uint32_t data, uint32_t address)
{
    NVMDATA0 = (uint32_t )data;

    NVM_StartOperationAtAddress( address,  WORD_PROGRAM_OPERATION);

    return true;
}

bool NVM_QuadWordWrite(uint32_t *data, uint32_t address)
{
   NVMDATA0 = *(data++);
   NVMDATA1 = *(data++);
   NVMDATA2 = *(data++);
   NVMDATA3 = *(data++);

   NVM_StartOperationAtAddress( address,  QUAD_WORD_PROGRAM_OPERATION);

   return true;
}

bool NVM_RowWrite(uint32_t *data, uint32_t address)
{
   NVMSRCADDR = (uint32_t )KVA_TO_PA(data);

   NVM_StartOperationAtAddress( address,  ROW_PROGRAM_OPERATION);

   return true;
}

bool NVM_PageErase(uint32_t address)
{
   NVM_StartOperationAtAddress(address,  PAGE_ERASE_OPERATION);

   return true;
}

NVM_ERROR NVM_ErrorGet( void )
{
    // mask for WRERR and LVDERR bits
    return (NVMCON & (_NVMCON_LVDERR_MASK | _NVMCON_WRERR_MASK));
}

bool NVM_IsBusy( void )
{
    return (bool)NVMCONbits.WR;
}

void NVM_ProgramFlashSwapBank( void )
{
    // NVMOP can be written only when WREN is zero. So, clear WREN.
    NVMCONCLR = _NVMCON_WREN_MASK;

    NVM_WriteUnlockSequence();

    // Map Program Flash Memory Bank 2 to lower region
    NVMCONSET = _NVMCON_PFSWAP_MASK;
}

void NVM_ProgramFlashWriteProtect( uint32_t address)
{
    NVM_WriteUnlockSequence();

    /* Program the 24-Bit address till where the memory has to be protected
     * from start of flash memory.
     * The Page in which the address falls and all the lower pages below it will
     * be protected from writes
     */
    NVMPWPSET = (address & _NVMPWP_PWP_MASK);
}

void NVM_ProgramFlashWriteProtectLock( void )
{
    NVM_WriteUnlockSequence();

    // Lock the Program flash Write protect register
    NVMPWPCLR = _NVMPWP_PWPULOCK_MASK;
}

void NVM_BootFlashWriteProtectEnable( NVM_BOOT_FLASH_WRITE_PROTECT writeProtectPage )
{
    NVM_WriteUnlockSequence();

    // Protect the appropriate boot page to disable writes
    NVMBWPSET = writeProtectPage;
}

void NVM_BootFlashWriteProtectDisable( NVM_BOOT_FLASH_WRITE_PROTECT writeProtectPage )
{
    NVM_WriteUnlockSequence();

    // Un-Protect the appropriate boot page to enable writes
    NVMBWPCLR = writeProtectPage;
}

void NVM_BootFlashWriteProtectLock( NVM_BOOT_FLASH_WRITE_PROTECT_LOCK writeProtectLock )
{
    NVM_WriteUnlockSequence();

    // Lock the Boot flash Write protect register
    NVMBWPCLR = writeProtectLock;
}