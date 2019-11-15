/*******************************************************************************
  Non-Volatile Memory Controller(NVMCTRL) PLIB.

  Company:
    Microchip Technology Inc.

  File Name:
    plib_nvmctrl.c

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

// *****************************************************************************
// *****************************************************************************
// Section: Included Files
// *****************************************************************************
// *****************************************************************************

#include <string.h>
#include "plib_nvmctrl.h"

static volatile uint16_t nvm_error;
static uint16_t nvm_status;
static uint32_t smart_eep_status;

// *****************************************************************************
// *****************************************************************************
// Section: NVMCTRL Implementation
// *****************************************************************************
// *****************************************************************************



void NVMCTRL_Initialize(void)
{
   NVMCTRL_REGS->NVMCTRL_CTRLA = NVMCTRL_CTRLA_RWS(5) | NVMCTRL_CTRLA_AUTOWS_Msk;    
}

bool NVMCTRL_Read( uint32_t *data, uint32_t length, const uint32_t address )
{
    memcpy((void *)data, (void *)address, length);
    
    return true;
}

void NVMCTRL_SetWriteMode(NVMCTRL_WRITEMODE mode)
{
    NVMCTRL_REGS->NVMCTRL_CTRLA = (NVMCTRL_REGS->NVMCTRL_CTRLA & (~NVMCTRL_CTRLA_WMODE_Msk)) | mode;
}

bool NVMCTRL_QuadWordWrite(const uint32_t *data, const uint32_t address)
{
    uint8_t i = 0;
    bool wr_status = false;
    uint32_t * paddress = (uint32_t *)address;
    uint32_t wr_mode = (NVMCTRL_REGS->NVMCTRL_CTRLA & NVMCTRL_CTRLA_WMODE_Msk); 

    /* Clear global error flag */
    nvm_error = 0;

    /* If the address is not a quad word address, return error */
    if((address & 0x0f) != 0)
    {
        wr_status = false;
    }
    else
    {
        /* Configure Quad Word Write */
        NVMCTRL_SetWriteMode(NVMCTRL_WMODE_ADW);

        /* Writing 32-bit data into the given address.  Writes to the page buffer must be 32 bits. */
        for (i = 0; i <= 3; i++)
        {
            *paddress++ = data[i];
        }
        /* Restore the write mode */
        NVMCTRL_SetWriteMode((NVMCTRL_WRITEMODE)wr_mode);
        wr_status = true;
    }
    return wr_status;
}

bool NVMCTRL_DoubleWordWrite(const uint32_t *data, const uint32_t address)
{
    uint8_t i = 0;
    bool wr_status = false;
    uint32_t * paddress = (uint32_t *)address;
    uint32_t wr_mode = (NVMCTRL_REGS->NVMCTRL_CTRLA & NVMCTRL_CTRLA_WMODE_Msk); 

    /* Clear global error flag */
    nvm_error = 0;

    /* If the address is not a double word address, return error */
    if((address & 0x07) != 0)
    {
        wr_status = false;
    }
    else
    {
        /* Configure Double Word Write */
        NVMCTRL_SetWriteMode(NVMCTRL_WMODE_ADW);

        /* Writing 32-bit data into the given address.  Writes to the page buffer must be 32 bits. */
        for (i = 0; i <= 1; i++)
        {
            *paddress++ = data[i];
        }
        /* Restore the write mode */
        NVMCTRL_SetWriteMode((NVMCTRL_WRITEMODE)wr_mode);
        wr_status = true;
    }
    return wr_status;
}

/* This function assumes that the page written is fresh or it is erased by
 * calling NVMCTRL_BlockErase
 */
bool NVMCTRL_PageWrite( const uint32_t *data, const uint32_t address )
{
    uint32_t i = 0;
    uint32_t * paddress = (uint32_t *)address;

    /* Clear global error flag */
    nvm_error = 0;

    /* writing 32-bit data into the given address.  Writes to the page buffer must be 32 bits */
    for (i = 0; i < (NVMCTRL_FLASH_PAGESIZE/4); i++)
    {
        *paddress++ = data[i];
    }

    /* If write mode is manual, */
    if ((NVMCTRL_REGS->NVMCTRL_CTRLA & NVMCTRL_CTRLA_WMODE_MAN) == NVMCTRL_CTRLA_WMODE_MAN)
    {
        /* Set address and command */
        NVMCTRL_REGS->NVMCTRL_CTRLB = NVMCTRL_CTRLB_CMD_WP | NVMCTRL_CTRLB_CMDEX_KEY;
    }
    
    return true;
}

bool NVMCTRL_BlockErase( uint32_t address )
{
    /* Clear global error flag */
    nvm_error = 0;

    /* Set address and command */
    NVMCTRL_REGS->NVMCTRL_ADDR = address;
    NVMCTRL_REGS->NVMCTRL_CTRLB = NVMCTRL_CTRLB_CMD_EB | NVMCTRL_CTRLB_CMDEX_KEY;
    
    return true;
}

uint16_t NVMCTRL_ErrorGet( void )
{
    nvm_error |= NVMCTRL_REGS->NVMCTRL_INTFLAG;

    /* Clear NVMCTRL INTFLAG register */
    NVMCTRL_REGS->NVMCTRL_INTFLAG = NVMCTRL_INTFLAG_Msk;
    return nvm_error;
}

uint16_t NVMCTRL_StatusGet( void )
{
    nvm_status = NVMCTRL_REGS->NVMCTRL_STATUS;
    
    return nvm_status;
}

bool NVMCTRL_IsBusy(void)
{
    return (bool)(!(NVMCTRL_REGS->NVMCTRL_STATUS & NVMCTRL_STATUS_READY_Msk));
}

void NVMCTRL_RegionLock(uint32_t address)
{
    /* Set address and command */
    NVMCTRL_REGS->NVMCTRL_ADDR = address;

    NVMCTRL_REGS->NVMCTRL_CTRLB = NVMCTRL_CTRLB_CMD_LR | NVMCTRL_CTRLB_CMDEX_KEY;
}

void NVMCTRL_RegionUnlock(uint32_t address)
{
    /* Set address and command */
    NVMCTRL_REGS->NVMCTRL_ADDR = address;

    NVMCTRL_REGS->NVMCTRL_CTRLB = NVMCTRL_CTRLB_CMD_UR | NVMCTRL_CTRLB_CMDEX_KEY;
}

uint32_t NVMCTRL_RegionLockStatusGet (void)
{
    return (NVMCTRL_REGS->NVMCTRL_RUNLOCK);
}

bool NVMCTRL_SmartEEPROM_IsBusy(void)
{
    return (bool)(NVMCTRL_REGS->NVMCTRL_SEESTAT & NVMCTRL_SEESTAT_BUSY_Msk);
}

uint32_t NVMCTRL_SmartEEPROMStatusGet( void )
{
    smart_eep_status = NVMCTRL_REGS->NVMCTRL_SEESTAT;
    
    return smart_eep_status;
}

bool NVMCTRL_SmartEEPROM_IsActiveSectorFull(void)
{
    return (bool)(NVMCTRL_REGS->NVMCTRL_INTFLAG & NVMCTRL_INTFLAG_SEESFULL_Msk);
}

/* Use BankSwap only when there are valid applications in both NVM Banks */
void NVMCTRL_BankSwap(void)
{
    NVMCTRL_REGS->NVMCTRL_CTRLB = NVMCTRL_CTRLB_CMD_BKSWRST | NVMCTRL_CTRLB_CMDEX_KEY;
}

void NVMCTRL_SmartEEPROMSectorReallocate(void)
{
    NVMCTRL_REGS->NVMCTRL_CTRLB = NVMCTRL_CTRLB_CMD_SEERALOC | NVMCTRL_CTRLB_CMDEX_KEY;
}

void NVMCTRL_SmartEEPROMFlushPageBuffer(void)
{
    /* Clear global error flag */
    nvm_error = 0;

    NVMCTRL_REGS->NVMCTRL_CTRLB = NVMCTRL_CTRLB_CMD_SEEFLUSH | NVMCTRL_CTRLB_CMDEX_KEY;
}


