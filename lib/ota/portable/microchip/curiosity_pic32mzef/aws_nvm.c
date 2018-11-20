/*
Amazon FreeRTOS OTA PAL for Curiosity_PIC32MZEF V1.0.2
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

#include <stdlib.h>
#include <string.h>
#include <sys/kmem.h>

#include "aws_nvm.h"
#include "peripheral/nvm/plib_nvm.h"
#include "peripheral/int/plib_int.h"

/* NVM Flash Memory programming
 * key 1*/

#define NVM_PROGRAM_UNLOCK_KEY1     0xAA996655

/* NVM Driver Flash Memory programming
 * key 1*/

#define NVM_PROGRAM_UNLOCK_KEY2     0x556699AA

// mask corresponding to AWS_NVM_QUAD_SIZE == 16
#define AWS_NVM_QUAD_MASK       0xfffffff0

// prototypes
//
static bool_t AWS_NVMOperation(uint32_t nvmop);
static void AWS_NVMClearError(void);


// implementation

static bool_t AWS_NVMOperation(uint32_t nvmop)
{
    uint32_t processorStatus;

    // TODO aa: taskENTER_CRITICAL() should be used?
    // But that depends on configMAX_SYSCALL_INTERRUPT_PRIORITY...!
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
    bool_t success = PLIB_NVM_WriteOperationHasTerminated(NVM_ID_0) == pdFALSE;

    PLIB_INT_SetState(INT_ID_0, processorStatus);

    return success;
}


bool_t AWS_FlashEraseUpdateBank( void )
{
    if(!AWS_NVMOperation(UPPER_FLASH_REGION_ERASE_OPERATION))
    {   // failed; clear the NVM error
        AWS_NVMClearError();
        return pdFALSE;
    }

    return pdTRUE;
}

static void AWS_NVMClearError(void)
{
    AWS_NVMOperation(NO_OPERATION);
}

bool_t AWS_NVM_QuadWordWrite(const uint32_t* address, const uint32_t* data, uint32_t nQuads)
{
    if(!PLIB_NVM_ExistsProvideQuadData(NVM_ID_0))
    {
        return pdFALSE;
    }

    uint32_t flash_phys_addr = KVA_TO_PA((uint32_t)address);

    bool_t success = pdTRUE;
    while(nQuads--)
    {
        PLIB_NVM_FlashAddressToModify(NVM_ID_0, flash_phys_addr);

        PLIB_NVM_FlashProvideQuadData(NVM_ID_0, (uint32_t*)data);

        if(!AWS_NVMOperation(QUAD_WORD_PROGRAM_OPERATION))
        {
            success = pdFALSE;
            AWS_NVMClearError();
            break;
        }
        flash_phys_addr += AWS_NVM_QUAD_SIZE;
        data += AWS_NVM_QUAD_SIZE / sizeof(*data);
    }

    return success;
}


bool_t AWS_FlashProgramBlock(const uint8_t* address, const uint8_t* pData, uint32_t size)
{
    uint32_t quad_buff[AWS_NVM_QUAD_SIZE / sizeof(uint32_t)];

    const uint8_t* start_quad = (const uint8_t*)((uint32_t)address & AWS_NVM_QUAD_MASK);    // quad to start with
    const uint8_t* first_quad = start_quad + AWS_NVM_QUAD_SIZE; // first full quad
    const uint8_t* end_quad = (const uint8_t*)(((uint32_t)address + size + AWS_NVM_QUAD_SIZE - 1) & AWS_NVM_QUAD_MASK); // last quad pointer, past the memory area
    const uint8_t* last_quad = end_quad - AWS_NVM_QUAD_SIZE; // last full quad pointer

    uint32_t n_quads = (end_quad - start_quad ) / AWS_NVM_QUAD_SIZE;

    bool_t flash_fault = pdFALSE;

    while(n_quads)
    {
        uint32_t start_gap = address - start_quad;    // not used bytes in the start quad
        uint32_t last_gap = end_quad - (address + size);// not used bytes in the last quad
        uint32_t last_fill = (address + size) - last_quad ;    // payload bytes in the last quad
        uint32_t start_load = AWS_NVM_QUAD_SIZE - start_gap; // useful bytes at beginning

        if(start_gap != 0)
        {   // not complete quad at the beginning
            memset(quad_buff, 0xff, sizeof(quad_buff));
            memcpy((uint8_t*)quad_buff + start_gap, pData, start_load);
            if(!AWS_NVM_QuadWordWrite((uint32_t*)start_quad, quad_buff, 1))
            {
                flash_fault = pdTRUE;
                break;
            }
            start_quad = first_quad;
            pData += start_load;
            n_quads--;
        }

        if(last_gap == 0)
        {
            if(!AWS_NVM_QuadWordWrite((uint32_t*)start_quad, (uint32_t*)pData, n_quads))
            {
                flash_fault = pdTRUE;
            }
            break;
        }

        // there is left payload data in the last quad
        // program full quads first
        if(!AWS_NVM_QuadWordWrite((uint32_t*)start_quad, (uint32_t*)pData, n_quads - 1))
        {
            flash_fault = pdTRUE;
            break;
        }

        // now program the last quad
        memset(quad_buff, 0xff, sizeof(quad_buff));
        memcpy((uint8_t*)quad_buff, pData + (n_quads - 1) * AWS_NVM_QUAD_SIZE, last_fill);
        if(!AWS_NVM_QuadWordWrite((uint32_t*)last_quad, quad_buff , 1))
        {
            flash_fault = pdTRUE;
        }

        break;
    }


    return !flash_fault;

}
