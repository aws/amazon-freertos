/*
  * Copyright 2017 Microchip Technology Incorporated and its subsidiaries.
  * 
  * Amazon FreeRTOS NVM routines for SAM E70 platform
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

/* NVM storage implementation for Microchip SAM E70 platform. */


#ifndef _AWS_NVM_H_
#define _AWS_NVM_H_

#include <stdint.h>
#include <stdbool.h>

#include "device.h" 


#define AWS_NVM_PAGE_SIZE           (IFLASH_PAGE_SIZE)
#define AWS_NVM_FLASH_SIZE          (IFLASH_SIZE)
#define AWS_NVM_FLASH_START_ADDRESS (IFLASH_ADDR)
#define AWS_NVM_ERASE_PAGES_MAX     (32)    // max number of pages that could be erased once
#define AWS_NVM_ERASE_PAGES_MIN     (16)    // min number of pages that could be erased once
#define AWS_NVM_LOCK_REGION_PAGES   (32)    // number of pages in a lock region


// locks the region indicated by the ptrFlash address
// ptrFlash should be a valid flash address, page aligned
// Note: lock regions are 16 KB each!
bool AWS_FlashRegionLock(const uint32_t* ptrFlash);

// unlocks the region indicated by the ptrFlash address
// ptrFlash should be a valid flash address, page aligned
// Note: lock regions are 16 KB each!
bool AWS_FlashRegionUnlock(const uint32_t* ptrFlash);

// erases flash pages indicated by the ptrFlash address
// the number of erased pages MUST be multiple of AWS_NVM_ERASE_PAGES_MIN/AWS_NVM_ERASE_PAGES_MAX!
// ptrFlash should be a valid flash address,
// properly aligned with the number of pages to erase
bool AWS_FlashPagesErase(const uint32_t* ptrFlash, int nPages);

// writes a number of flash pages starting at the ptrFlash address
// the data is pointed at by pageData 
// ptrFlash should be a valid flash address, page aligned
bool AWS_FlashPagesWrite(const uint32_t* ptrFlash, const uint32_t* pageData, int nPages);

// programs a block of data into the flash memory
//      address - starting address in flash
//      pData   - pointer to data buffer
//      size    - size of buffer
// returns true if programming succeeds, false otherwise
// Uses the page write operation but can program across pages
// by performing copy operations when working with fractionary pages
// So the "address" parameter does not have to be page aligned.
// The touched flash area should have been previously erased
bool AWS_FlashBlockWrite(const uint8_t* address, const uint8_t* pData, uint32_t size);

bool AWS_FlashBlockWriteEx(const uint8_t* address, const uint8_t* pData, uint32_t size);

// enables/disables a MPU region protection for the key storage flash
// this will add/remove protection for the keys storage flash area 
// Notes:
//  - by default the whole flash is write protected
//  - region size should be >= 4 and a power of 2
//  - flashAddress should be aligned to the size of the region
// Returns :
//      true if the call succeeded
//      false if the regionSize value is incorrect
//
// Note: currently not used!
bool AWS_FlashRegionProtect(int regionNo, uint32_t regionSize, const void* flashAddress, bool enable);

bool AWS_FlashBlockWriteEx(const uint8_t* address, const uint8_t* pData, uint32_t size);
#endif // _AWS_NVM_H_

