/*
 * Amazon FreeRTOS Demo Bootloader V1.4.4
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


#ifndef _AWS_BOOT_NVM_H_
#define _AWS_BOOT_NVM_H_

#include <stdint.h>
#include <stdbool.h>


#define AWS_NVM_QUAD_SIZE    16

/* performs a quad write operation */
bool AWS_NVM_QuadWordWrite( const uint32_t * address,
                            const uint32_t * data,
                            int nQuads );

/* toggles the mapping of the program flash panels: */
/* lower <-> upper */
void AWS_NVM_ToggleFlashBanks( void );

bool AWS_UpperBootPageErase( const uint32_t * pagePtr );

bool AWS_UpperBootWriteRow( const uint32_t * ptrFlash,
                            const uint32_t * rowData );

void AWS_UpperBootPageProtectionDisable( void );
void AWS_UpperBootPageProtectionEnable( void );
bool AWS_UpperBootPageIsProtected( void );

bool AWS_FlashProgramBlock( const uint8_t * address,
                            const uint8_t * pData,
                            uint32_t size );


/* Returns true if flash bank 2 is mapped to the lower region */
/* i.e. the panels are swapped from the "normal" mapping */
/* which is flash bank 1 to the lower region */
bool AWS_FlashProgramIsSwapped( void );


#endif /* _AWS_BOOT_NVM_H_ */
