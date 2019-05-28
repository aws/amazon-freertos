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


#ifndef _PKCS11_NVM_H_
#define _PKCS11_NVM_H_

#include <stdint.h>
#include <stdbool.h>

#define AWS_NVM_ROW_SIZE    (2 * 1024)
#define AWS_NVM_PAGE_SIZE   (16 * 1024)


// erases the boot upper page 4
bool AWS_UpperBootPage4Erase(void);


// writes a row in the upper boot flash
// the row is indicated by the ptrFlash address
// the data is pointed at by rowData 
bool AWS_UpperBootWriteRow(const uint32_t* ptrFlash, const uint32_t* rowData);


// disables the write protection to the upper boot page 4
void AWS_UpperBootPage4ProtectionDisable(void);

// enables the write protection to the upper boot page 4
void AWS_UpperBootPage4ProtectionEnable(void);

// returns true if the upper boot page 4 is write protected
bool AWS_UpperBootPage4IsProtected(void);


#endif // _PKCS11_NVM_H_

