/****************************************************************
  Module for Microchip Bootloader Library

  Company:
    Microchip Technology Inc.

  File Name:
    aws_bootloader_nvm.h

  Summary:
    The header file joins all header files used in the Bootloader Library
    and contains compile options and defaults.

  Description:
    This header file includes all the header files required to use the
    Microchip Bootloader Library. Library features and options defined
    in the Bootloader Library configurations will be included in each build.
*******************************************************************************/

// DOM-IGNORE-BEGIN
/*******************************************************************************
Copyright (c) 2018 released Microchip Technology Inc.  All rights reserved.

Microchip licenses to you the right to use, modify, copy and distribute
Software only when embedded on a Microchip microcontroller or digital signal
controller that is integrated into your product or third party product
(pursuant to the sublicense terms in the accompanying license agreement).

You should refer to the license agreement accompanying this Software for
additional information regarding your rights and obligations.

SOFTWARE AND DOCUMENTATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND,
EITHER EXPRESS OR IMPLIED, INCLUDING WITHOUT LIMITATION, ANY WARRANTY OF
MERCHANTABILITY, TITLE, NON-INFRINGEMENT AND FITNESS FOR A PARTICULAR PURPOSE.
IN NO EVENT SHALL MICROCHIP OR ITS LICENSORS BE LIABLE OR OBLIGATED UNDER
CONTRACT, NEGLIGENCE, STRICT LIABILITY, CONTRIBUTION, BREACH OF WARRANTY, OR
OTHER LEGAL EQUITABLE THEORY ANY DIRECT OR INDIRECT DAMAGES OR EXPENSES
INCLUDING BUT NOT LIMITED TO ANY INCIDENTAL, SPECIAL, INDIRECT, PUNITIVE OR
CONSEQUENTIAL DAMAGES, LOST PROFITS OR LOST DATA, COST OF PROCUREMENT OF
SUBSTITUTE GOODS, TECHNOLOGY, SERVICES, OR ANY CLAIMS BY THIRD PARTIES
(INCLUDING BUT NOT LIMITED TO ANY DEFENSE THEREOF), OR OTHER SIMILAR COSTS.
*******************************************************************************/


#ifndef _AWS_BOOTLOADER_NVM_H_
#define _AWS_BOOTLOADER_NVM_H_

#include <stdint.h>
#include <stdbool.h>


#define AWS_NVM_QUAD_SIZE       16

// performs a quad write operation
bool    AWS_NVM_QuadWordWrite(const uint32_t* address, const uint32_t* data, int nQuads);

// toggles the mapping of the program flash panels: 
// lower <-> upper
void    AWS_NVM_ToggleFlashBanks(void);

// test routines

bool    AWS_UpperBootPageErase(const uint32_t* pagePtr);

bool    AWS_UpperBootWriteRow(const uint32_t* ptrFlash, const uint32_t* rowData);

void    AWS_UpperBootPageProtectionDisable(void);
void    AWS_UpperBootPageProtectionEnable(void);
bool    AWS_UpperBootPageIsProtected(void);

bool    AWS_FlashProgramBlock(const uint8_t* address, const uint8_t* pData, uint32_t size);


// returns true if flash bank 2 is mapped to the lower region
// i.e. the panels are swapped from the "normal" mapping
// which is flash bank 1 to the lower region 
bool    AWS_FlashProgramIsSwapped(void);


#endif  // _AWS_BOOTLOADER_NVM_H_


