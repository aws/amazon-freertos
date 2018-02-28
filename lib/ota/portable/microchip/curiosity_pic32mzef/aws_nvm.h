/*
Amazon FreeRTOS OTA PAL for Curiosity_PIC32MZEF V0.9.0
Copyright (C) 2018 Amazon.com, Inc. or its affiliates.  All Rights Reserved.

Permission is hereby granted, free of charge, to any person obtaining a copy of
this software and associated documentation files (the "Software"), to deal in
the Software without restriction, including without limitation the rights to
use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
the Software, and to permit persons to whom the Software is furnished to do so,
subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

 http://aws.amazon.com/freertos
 http://www.FreeRTOS.org
*/

/* OTA PAL implementation for Microchip PIC32MZ platform. */


#ifndef _AWS_NVM_H_
#define _AWS_NVM_H_

#include <stdint.h>
#include <stdbool.h>

#define AWS_NVM_QUAD_SIZE       16


// erases the flash page and prepares for OTA update
// returns true if the erase succeeded
// false otherwise
bool AWS_FlashErase(void);


// performs a quad write operation
bool    AWS_NVM_QuadWordWrite(const uint32_t* address, const uint32_t* data, int nQuads);

// programs a block of data into the flash memory
//      address - starting address in flash
//      pData   - pointer to data buffer
//      size    - size of buffer
// returns true if programming succeeds, false otherwise
// Uses the quad write operation
bool AWS_FlashProgramBlock(const uint8_t* address, const uint8_t* pData, uint32_t size);


// CRC calculation

void         AWS_BootCrcInit(uint32_t seed);
void         AWS_BootCrcAddBuffer(const uint8_t* buff, int buffLen);
uint32_t     AWS_BootCrcResult(void);


#endif // _AWS_NVM_H_

