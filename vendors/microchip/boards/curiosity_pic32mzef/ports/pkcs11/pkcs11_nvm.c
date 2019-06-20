/*
  * Copyright 2017 Microchip Technology Incorporated and its subsidiaries.
  * 
  * Amazon FreeRTOS PKCS#11 for Curiosity PIC32MZEF V1.0.4
  * Copyright (C) 2018 Amazon.com, Inc. or its affiliates.  All Rights Reserved.
  *
  * Permission is hereby granted, free of charge, to any person obtaining a copy of 
  * this software and associated documentation files (the "Software"), to deal in 
  * the Software without restriction, including without limitation the rights to 
  * use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies 
  * of the Software, and to permit persons to whom the Software is furnished to do 
  * so, subject to the following conditions:
  * The above copyright notice and this permission notice shall be included in all 
  * copies or substantial portions of the Software.

  * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR 
  * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, 
  * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE 
  * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER 
  * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, 
  * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE 
  * SOFTWARE
  ******************************************************************************
 */

/* PKCS11 storage implementation for Microchip PIC32MZ platform. */

#include <stdlib.h>
#include <string.h>
#include <sys/kmem.h>

#include "pkcs11_nvm.h"
#include "peripheral/nvm/plib_nvm.h"
#include "peripheral/int/plib_int.h"


/* NVM Flash Memory programming
 * key 1*/

#define NVM_PROGRAM_UNLOCK_KEY1     0xAA996655

/* NVM Driver Flash Memory programming
 * key 1*/

#define NVM_PROGRAM_UNLOCK_KEY2     0x556699AA

// data



// prototypes
//
static bool AWS_NVMOperation(uint32_t nvmop);
static void AWS_NVMClearError(void);


// implementation

static bool AWS_NVMOperation(uint32_t nvmop)
{
    uint32_t processorStatus;
    
    processorStatus = PLIB_INT_GetStateAndDisable( INT_ID_0 );
    
    // Disable flash write/erase operations
    PLIB_NVM_MemoryModifyInhibit(NVM_ID_0);

    PLIB_NVM_MemoryOperationSelect(NVM_ID_0, nvmop);

    // Allow memory modifications
    PLIB_NVM_MemoryModifyEnable(NVM_ID_0);

    /* Unlock the Flash */
    PLIB_NVM_FlashWriteKeySequence(NVM_ID_0, 0);
    PLIB_NVM_FlashWriteKeySequence(NVM_ID_0, NVM_PROGRAM_UNLOCK_KEY1);
    PLIB_NVM_FlashWriteKeySequence(NVM_ID_0, NVM_PROGRAM_UNLOCK_KEY2);

    PLIB_NVM_FlashWriteStart(NVM_ID_0);
    
    while (!PLIB_NVM_FlashWriteCycleHasCompleted(NVM_ID_0));
    bool success = PLIB_NVM_WriteOperationHasTerminated(NVM_ID_0) == false;
    
    PLIB_INT_SetState(INT_ID_0, processorStatus);

    return success;
}


static void AWS_NVMClearError(void)
{
    AWS_NVMOperation(NO_OPERATION);
}

bool AWS_UpperBootPage4Erase(void)
{
    uint32_t phys_addr = KVA_TO_PA(__UPPERBOOTALIASLASTPAGE_BASE);
    PLIB_NVM_FlashAddressToModify(NVM_ID_0, phys_addr);

    if(!AWS_NVMOperation(PAGE_ERASE_OPERATION))
    {   // failed; clear the NVM error
        AWS_NVMClearError();
        return false;
    }

    return true;
}

bool AWS_UpperBootWriteRow(const uint32_t* ptrFlash, const uint32_t* rowData)
{
    uint32_t phys_flash_addr = KVA_TO_PA((uint32_t)ptrFlash);
    uint32_t phys_data_addr = KVA_TO_PA((uint32_t)rowData);

    PLIB_NVM_FlashAddressToModify(NVM_ID_0, phys_flash_addr);
    PLIB_NVM_DataBlockSourceAddress(NVM_ID_0, phys_data_addr);

    if(!AWS_NVMOperation(ROW_PROGRAM_OPERATION))
    {
        AWS_NVMClearError();
        return false;
    }

    return true;
}


void AWS_UpperBootPage4ProtectionDisable(void)
{
    PLIB_NVM_FlashWriteKeySequence(NVM_ID_0, 0);
    PLIB_NVM_FlashWriteKeySequence(NVM_ID_0, NVM_PROGRAM_UNLOCK_KEY1);
    PLIB_NVM_FlashWriteKeySequence(NVM_ID_0, NVM_PROGRAM_UNLOCK_KEY2);

    PLIB_NVM_BootPageWriteProtectionDisable( NVM_ID_0, UPPER_BOOT_ALIAS_PAGE4);
}

void AWS_UpperBootPage4ProtectionEnable(void)
{
    PLIB_NVM_FlashWriteKeySequence(NVM_ID_0, 0);
    PLIB_NVM_FlashWriteKeySequence(NVM_ID_0, NVM_PROGRAM_UNLOCK_KEY1);
    PLIB_NVM_FlashWriteKeySequence(NVM_ID_0, NVM_PROGRAM_UNLOCK_KEY2);

    PLIB_NVM_BootPageWriteProtectionEnable( NVM_ID_0, UPPER_BOOT_ALIAS_PAGE4);
}

bool AWS_UpperBootPage4IsProtected(void)
{
    return PLIB_NVM_IsBootPageWriteProtected(NVM_ID_0, UPPER_BOOT_ALIAS_PAGE4);
}


