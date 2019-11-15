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
#include <stdbool.h>
#include "pkcs11_nvm.h"


#include "device.h"
#include "system/int/sys_int.h"
#include "peripheral/efc/plib_efc.h"



// mask corresponding to AWS_NVM_PAGE_SIZE == 512 = 2^9
#define AWS_NVM_PAGE_MASK   (~((1 << 9) - 1))


// page command args for erasing min/max pages
#define AWS_NVM_ERASE_COMMAND_ARG_MIN   2
#define AWS_NVM_ERASE_COMMAND_ARG_MAX   3

// force erase in MIN pages, if needed for some reason
#define AWS_NVM_ERASE_FORCE_MIN         0

// prototype for all AWS_EFC_Command functions is:
// uint32_t AWS_EFC_Command(uint32_t efc_command);
// the function takes a properly EFC formatted parameter (password + arg + operation)
// waits for the FRDY to be set and then
// returns the EFC FSR register that shows the EFC operation outcome
static uint32_t AWS_EFC_Command(uint32_t efc_command);

// erases AWS_NVM_ERASE_PAGES_MIN/AWS_NVM_ERASE_PAGES_MAX pages at a time
// that's what the NVM controller can handle
static bool AWS_FlashEraseChunk(const uint32_t* ptrFlash, int nChunkPages);

// TODO aa: current version of xc32 does not handle properly the __longramfunc__ version
// If using just __ramfunc__ then the build results in "unresolvable R_ARM_ABS32 relocation against symbol `AWS_EFC_Command'"
// Moving the function in RAM manually workaround is implemented for now


#define AWS_RAM_EFC_WORKAROUND      2   // 0 no workaround, use xc32 __ramfunc__
                                        // 1 manually copy the function in RAM
                                        // 2 use ROM functions for EFC programming

#if ((AWS_RAM_EFC_WORKAROUND & 2) != 0)
static uint32_t AWS_EFC_Command_Rom(uint32_t efc_command);
#endif


bool AWS_FlashRegionLock(const uint32_t* ptrFlash)
{
    uint16_t page_number = ((uint32_t)ptrFlash - IFLASH_ADDR) / IFLASH_PAGE_SIZE;

    /* Issue the Lock Bit operation */
	uint32_t efc_command = (EEFC_FCR_FCMD_SLB | EEFC_FCR_FARG(page_number) | EEFC_FCR_FKEY_PASSWD);

    bool int_status = SYS_INT_Disable();
    uint32_t flash_status = AWS_EFC_Command(efc_command);
    SYS_INT_Restore(int_status);

    return (flash_status & (EEFC_FSR_FCMDE_Msk | EEFC_FSR_FLERR_Msk | EEFC_FSR_FLOCKE_Msk)) == 0;
}

bool AWS_FlashRegionUnlock(const uint32_t* ptrFlash)
{
    uint16_t page_number = ((uint32_t)ptrFlash - IFLASH_ADDR) / IFLASH_PAGE_SIZE;

    /* Issue the Lock Bit operation */
	uint32_t efc_command = (EEFC_FCR_FCMD_CLB | EEFC_FCR_FARG(page_number) | EEFC_FCR_FKEY_PASSWD);

    bool int_status = SYS_INT_Disable();
    uint32_t flash_status = AWS_EFC_Command(efc_command);
    SYS_INT_Restore(int_status);

    return (flash_status & (EEFC_FSR_FCMDE_Msk | EEFC_FSR_FLERR_Msk | EEFC_FSR_FLOCKE_Msk)) == 0;
}

// nPages needs to be multiple of AWS_NVM_ERASE_PAGES_MIN/AWS_NVM_ERASE_PAGES_MAX
bool AWS_FlashPagesErase(const uint32_t* ptrFlash, int nPages)
{
    int chunkIx, nMaxChunks, nMinChunks;
    nMaxChunks = nPages / AWS_NVM_ERASE_PAGES_MAX;
    nMinChunks = (nPages % AWS_NVM_ERASE_PAGES_MAX) / AWS_NVM_ERASE_PAGES_MIN;

    // start erasing
    bool eraseFail = false;
    
    for(chunkIx = 0; chunkIx < nMaxChunks && !eraseFail; chunkIx++)
    {
        if(!AWS_FlashEraseChunk(ptrFlash, AWS_NVM_ERASE_PAGES_MAX))
        {
            eraseFail = true;
            break;
        }
        ptrFlash += (AWS_NVM_PAGE_SIZE * AWS_NVM_ERASE_PAGES_MAX) / sizeof(uint32_t);
    }
    for(chunkIx = 0; chunkIx < nMinChunks && !eraseFail; chunkIx++)
    {
        if(!AWS_FlashEraseChunk(ptrFlash, AWS_NVM_ERASE_PAGES_MIN))
        {
            eraseFail = true;
            break;
        }
        ptrFlash += (AWS_NVM_PAGE_SIZE * AWS_NVM_ERASE_PAGES_MIN) / sizeof(uint32_t);
    }

    return !eraseFail;
}

bool AWS_NVM_QuadWordWrite(const uint32_t u32Flashaddr, const uint32_t * ptrData, int nQuads)
{
    for(int i=0;i<nQuads;i++)
        EFC_QuadWordWrite(ptrData + (AWS_NVM_QUAD_SIZE *i)/sizeof(uint32_t), u32Flashaddr + (AWS_NVM_QUAD_SIZE *i)/sizeof(uint32_t));
}

// erases AWS_NVM_ERASE_PAGES_MIN/AWS_NVM_ERASE_PAGES_MAX pages at a time
// that's what the NVM controller can handle
static bool AWS_FlashEraseChunk(const uint32_t* ptrFlash, int nChunkPages)
{
    uint8_t page_arg;
    uint16_t page_step;

    if(nChunkPages == AWS_NVM_ERASE_PAGES_MIN)
    {
        page_arg = AWS_NVM_ERASE_COMMAND_ARG_MIN;
        page_step = AWS_NVM_ERASE_PAGES_MIN;
    }
    else if(nChunkPages == AWS_NVM_ERASE_PAGES_MAX)
    {
        page_arg = AWS_NVM_ERASE_COMMAND_ARG_MAX;
        page_step = AWS_NVM_ERASE_PAGES_MAX;
    }
    else
    {
        return false;
    }

    uint16_t page_start = ((uint32_t)ptrFlash - IFLASH_ADDR) / IFLASH_PAGE_SIZE;
    uint16_t page_end = page_start + nChunkPages;
#if (AWS_NVM_ERASE_FORCE_MIN != 0)
    // force AWS_NVM_ERASE_PAGES_MIN pages at a time
    page_arg = AWS_NVM_ERASE_COMMAND_ARG_MIN;
    page_step = AWS_NVM_ERASE_PAGES_MIN;
#endif  // (AWS_NVM_ERASE_FORCE_MIN != 0)

    bool int_status = SYS_INT_Disable();

    uint16_t page_no;
    uint32_t flash_status = 0;
    for(page_no = page_start; page_no < page_end; page_no += page_step)
    {
        uint32_t efc_command = (EEFC_FCR_FCMD_EPA | EEFC_FCR_FARG(page_no | page_arg) | EEFC_FCR_FKEY_PASSWD);
        flash_status |= AWS_EFC_Command(efc_command);
    }

    SYS_INT_Restore(int_status);

    return (flash_status & (EEFC_FSR_FCMDE_Msk | EEFC_FSR_FLERR_Msk | EEFC_FSR_FLOCKE_Msk)) == 0;
}

bool AWS_FlashPagesWrite(const uint32_t* ptrFlashData, const uint32_t* ptrPageData, int nPages)
{
    int ix, pageIx;
    uint32_t flash_status = 0;
    const uint32_t* ptrFlash = ptrFlashData;
    const uint32_t* pageData = ptrPageData;

    if(((uint32_t)ptrFlash % IFLASH_PAGE_SIZE) != 0)
    {
        return false;
    }

    uint16_t page_number = ((uint32_t)ptrFlash - IFLASH_ADDR) / IFLASH_PAGE_SIZE;

    bool int_status = SYS_INT_Disable();

    for(pageIx = 0; pageIx < nPages; pageIx++)
    {
        for (ix = 0; ix < IFLASH_PAGE_SIZE / sizeof(uint32_t); ix++)
        {
            *(uint32_t*)ptrFlash++ = *pageData++;
        }

        uint32_t efc_command = (EEFC_FCR_FCMD_WP | EEFC_FCR_FARG(page_number)| EEFC_FCR_FKEY_PASSWD);
        flash_status = AWS_EFC_Command(efc_command);

        if((flash_status & (EEFC_FSR_FCMDE_Msk | EEFC_FSR_FLERR_Msk | EEFC_FSR_FLOCKE_Msk)) != 0)
        {   // some error
            break;
        }
        page_number++;
    }

    SYS_INT_Restore(int_status);
    while(EFC_IsBusy());

    return (flash_status & (EEFC_FSR_FCMDE_Msk | EEFC_FSR_FLERR_Msk | EEFC_FSR_FLOCKE_Msk)) == 0;

}

static uint32_t AWS_EFC_Command(uint32_t efc_command)
{
    while(EFC_IsBusy());
    
#if ((AWS_RAM_EFC_WORKAROUND & 2) != 0)
    return AWS_EFC_Command_Rom(efc_command);
#endif  // ((AWS_RAM_EFC_WORKAROUND & 2) != 0)

    return 0xffffffff;
}


#if ((AWS_RAM_EFC_WORKAROUND & 2) != 0)
static uint32_t AWS_EFC_Command_Rom(uint32_t efc_command)
{
    uint32_t (*iap_function)(int ix, uint32_t cmd) = (uint32_t (*)(int ix, uint32_t cmd))(*(uint32_t*)0x00800008);

    return (*iap_function)(0, efc_command);
}
#endif  // ((AWS_RAM_EFC_WORKAROUND & 2) != 0)


bool AWS_FlashBlockWrite(const uint8_t* address, const uint8_t* pData, uint32_t size)
{
    uint32_t page_buff[AWS_NVM_PAGE_SIZE / sizeof(uint32_t)];

    const uint8_t* start_page = (const uint8_t*)((uint32_t)address & AWS_NVM_PAGE_MASK);    // page to start with
    const uint8_t* end_page = (const uint8_t*)(((uint32_t)address + size + AWS_NVM_PAGE_SIZE - 1) & AWS_NVM_PAGE_MASK); // last page pointer, past the memory area
    const uint8_t* last_page = end_page - AWS_NVM_PAGE_SIZE; // last full page pointer

    int n_pages = (end_page - start_page ) / AWS_NVM_PAGE_SIZE;

    bool flash_fault = false;


    while(n_pages)
    {
        int start_gap = address - start_page;    // not used bytes in the start page
        int last_gap = end_page - (address + size);// not used bytes in the last page
        int last_fill = (address + size) - last_page ;    // payload bytes in the last page
        int start_load = AWS_NVM_PAGE_SIZE - start_gap; // useful bytes at beginning
        if(start_load > size)
        {   // not more than available
            start_load = size;
        }

        // (incomplete) page at the beginning
        memset(page_buff, 0xff, sizeof(page_buff));
        memcpy((uint8_t*)page_buff + start_gap, pData, start_load);
        if(!AWS_FlashPagesWrite((uint32_t*)start_page, page_buff, 1))
        {
            flash_fault = true;
            break;
        }
        start_page += AWS_NVM_PAGE_SIZE;
        pData += start_load;
        n_pages--;

        if(n_pages == 0)
        {   // done
            break;
        }

        if(last_gap == 0)
        {
            if(!AWS_FlashPagesWrite((uint32_t*)start_page, (uint32_t*)pData, n_pages))
            {
                flash_fault = true;
            }
            break;
        }

        // there is left payload data in the last page
        // program full pages first

        if(n_pages > 1)
        {
            if(!AWS_FlashPagesWrite((uint32_t*)start_page, (uint32_t*)pData, n_pages - 1))
            {
                flash_fault = true;
                break;
            }
            start_page += (n_pages - 1) * AWS_NVM_PAGE_SIZE;
            pData += (n_pages - 1) * AWS_NVM_PAGE_SIZE;
        }

        // now program the last page
        memset(page_buff, 0xff, sizeof(page_buff));
        memcpy((uint8_t*)page_buff, pData, last_fill);
        if(!AWS_FlashPagesWrite((uint32_t*)last_page, page_buff, 1))
        {
            flash_fault = true;
        }

        break;
    }


    return !flash_fault;

}

static bool AWS_FlashQuadWordWrite(const uint8_t* address, const uint8_t* pData, uint32_t size)
{
    int i=0;
    uint8_t bMem[16];
    bool bResult=true;
    for(i=0;i<size;i=i+16)
    {
        while(EFC_IsBusy());
        bResult = bResult && EFC_QuadWordWrite(pData+i, address + i);
    }
    if(bResult && i!=size)
    {
        memcpy(bMem,pData+i,size-i);
        while(EFC_IsBusy());
        bResult = bResult && EFC_QuadWordWrite(pData+i, address + i);

    }
    return bResult;
}


bool AWS_FlashBlockWriteEx(const uint8_t* address, const uint8_t* pData, uint32_t size)
{
    bool flash_fault = true;
    int i=0;
    char myData[512];
    if(size !=1024)
        return false;
    else
    {
        while(size > 512 && flash_fault)
        {
            flash_fault= flash_fault && AWS_FlashPagesWrite(address + 512*i, pData+512*i,1);
            i++;
        }
        
        if(size <=512 && flash_fault)
        {
            flash_fault =   AWS_FlashQuadWordWrite(pData + 512*i, address + 512*i, size-i);
        }
    }

    return !flash_fault;

}

#define AWS_MPU_REGION_MIN_SIZE 4

// This does not seem to work. Not used for now!
bool AWS_FlashRegionProtect(int regionNo, uint32_t regionSize, const void* flashAddress, bool enable)
{
    return true;

}


