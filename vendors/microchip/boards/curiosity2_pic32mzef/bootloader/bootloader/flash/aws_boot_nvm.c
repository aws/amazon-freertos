/*
 * Amazon FreeRTOS Demo Bootloader V1.4.8
 * Copyright (C) 2018 Amazon.com, Inc. or its affiliates.  All Rights Reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of
 * this software and associated documentation files (the "Software"), to deal in
 * the Software without restriction, including without limitation the rights to
 * use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
 * the Software, and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 * http://aws.amazon.com/freertos
 * http://www.FreeRTOS.org
 */


/* Microchip includes.*/


#include "aws_boot_nvm.h"

/* defines */

#include "system/int/sys_int.h"
#include "peripheral/nvm/plib_nvm.h"



/* NVM Flash Memory programming
 * key 1*/

#define NVM_PROGRAM_UNLOCK_KEY1    0xAA996655

/* NVM Driver Flash Memory programming
 * key 1*/

#define NVM_PROGRAM_UNLOCK_KEY2    0x556699AA

#define AWS_NVM_QUAD_MASK          0xfffffff0

void nvm_callback(uintptr_t context);
// prototypes
//

volatile bool bTaskCompleted = false;

// implementation
void nvm_callback(uintptr_t context)
{
    bTaskCompleted=true;
}

/* implementation */


static void WriteNVMCallback(void)
{
    static bool bInstance = false;
    if(!bInstance)
    {
        NVM_CallbackRegister(nvm_callback, (uintptr_t)NULL);   
        bInstance=!bInstance;
    }
}


/*-----------------------------------------------------------*/

bool AWS_NVM_QuadWordWrite(uint32_t * address,
                           uint32_t * data,
                            int nQuads )
{
     bool bResult=true;
     
     uint32_t flash_phys_addr = (uint32_t) address;
     uint32_t *flash_data = data;
     
     WriteNVMCallback();
        /* Program a row of data */
     for (int i=0;i<nQuads && bResult; i++)
     {
        bTaskCompleted=false;
        while(NVM_IsBusy() == true);
        NVM_QuadWordWrite((uint32_t *)flash_data, (uint32_t) flash_phys_addr);
        
        while(bTaskCompleted == false);
        
        flash_phys_addr += AWS_NVM_QUAD_SIZE;
        flash_data += AWS_NVM_QUAD_SIZE / sizeof(*flash_data);

        bTaskCompleted = false;
        bResult = (NVM_ErrorGet() == 0)?true:false;
     }
    return bResult;
}


/*-----------------------------------------------------------*/

void AWS_NVM_ToggleFlashBanks( void )
{
    bool bank2Low=false;
    SYS_INT_Disable();
    while(NVM_IsBusy() == true);
   // NVMOP can be written only when WREN is zero. So, clear WREN.
    bank2Low = NVMCON & _NVMCON_PFSWAP_MASK;
    NVMCON2CLR = _NVMCON2_SWAPLOCK_MASK;
    NVMCONCLR = _NVMCON_WREN_MASK;
        // Write the unlock key sequence
    NVMKEY = 0x0;
    NVMKEY = NVM_PROGRAM_UNLOCK_KEY1;
    NVMKEY = NVM_PROGRAM_UNLOCK_KEY2;
    // Map Program Flash Memory Bank 2 to lower region
    
    if(!bank2Low)
    {
        // Do swap
        NVMCONSET = _NVMCON_PFSWAP_MASK;
    }
    else
    {
        // Do Nothing
        NVMCONCLR = _NVMCON_PFSWAP_MASK;
    }
    while(NVM_IsBusy() == true);
    //while(NVM_IsBusy() == true);
    SYS_INT_Enable();
}


/*-----------------------------------------------------------*/



/*-----------------------------------------------------------*/

bool AWS_UpperBootPageErase( const uint32_t  addr )
{
   bool bResult=true;
    NVM_CallbackRegister(nvm_callback, (uintptr_t)NULL);
    bTaskCompleted=false; 
    while(NVM_IsBusy() == true);
    
    /* Erase the Page */
    NVM_PageErase(addr);
    
    while(bTaskCompleted == false);
    bResult = (NVM_ErrorGet() == 0)?true:false;
    return bResult;
}

/* writes a row into the upper boot flash */
/* ptrFlash indicates the address and pData the data to be written */
bool AWS_UpperBootWriteRow(uint32_t ptrFlash, uint32_t* rowData)
{
    bool bResult=true;
        /* Program a row of data */
        bTaskCompleted=false;
        while(NVM_IsBusy() == true);
        NVM_RowWrite((uint32_t *)rowData, ptrFlash);

        while(bTaskCompleted == false);

        bTaskCompleted = false;
    bResult = (NVM_ErrorGet() == 0)?true:false;
    return bResult;
}



/*-----------------------------------------------------------*/


/*-----------------------------------------------------------*/

bool AWS_FlashErase( uint32_t ulFlashNvop )
{
   volatile uint32_t processorStatus;
    uint32_t operation = 5 + (ulFlashNvop == 0)? 0: 1; // Upper Boot Region
    bool bResult=true;
    processorStatus = __builtin_disable_interrupts();

    bTaskCompleted=false;
    while(NVM_IsBusy() == true);

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

    // Write the unlock key sequence
    NVMKEY = 0x0;
    NVMKEY = NVM_PROGRAM_UNLOCK_KEY1;
    NVMKEY = NVM_PROGRAM_UNLOCK_KEY2;

    // Start the operation
    NVMCONSET = _NVMCON_WR_MASK;

    __builtin_mtc0(12, 0, processorStatus);

    IEC5SET   = 0x80;
    while(bTaskCompleted == false);

    bTaskCompleted = false;
    bResult = (NVM_ErrorGet() == 0)?true:false;
    return bResult;
}
