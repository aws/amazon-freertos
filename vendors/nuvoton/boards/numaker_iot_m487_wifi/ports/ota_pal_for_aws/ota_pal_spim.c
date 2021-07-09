/*
 * FreeRTOS OTA PAL for Nuvoton Numaker-IoT-M487
 * Copyright (C) 2020 Amazon.com, Inc. or its affiliates.  All Rights Reserved.
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

 /* OTA SPI Flash implementation for M487 platform. */

#include <stdio.h>
#include <stdlib.h>
#include "FreeRTOS.h"
#include "ota_pal.h"

#include "NuMicro.h"

#define USE_4_BYTES_MODE            0            /* SPI Flash W25Q20 does not support 4-bytes address mode. */
#if 0
#define FLASH_BANK_UB               (8*64*1024)    /* Flash bank boundary. Depend on the physical flash. */
#define FLASH_BLOCK_SIZE            (64*1024)
#else		/* W25Q32 blokc size 4KB, chip size 4MB */
#define FLASH_BANK_UB               (1024*4*1024)    /* Flash bank boundary. Depend on the physical flash. */
#define FLASH_BLOCK_SIZE            (4*1024)
#endif

#define BUFFER_SIZE                 512//2048

#ifdef __ICCARM__
#pragma data_alignment=4
static uint8_t  spi_buff[BUFFER_SIZE];
#else
static uint8_t  spi_buff[BUFFER_SIZE] __attribute__((aligned(4)));
#endif


bool_t NVT_SPI_Flash_Init(void)
{
    uint8_t     idBuf[3];
    bool_t result = pdFALSE;
    
    /* Unlock protected registers */
    SYS_UnlockReg();
    
    /* Enable SPIM module clock */
    CLK_EnableModuleClock(SPIM_MODULE);
    
    /* Init SPIM multi-function pins, MOSI(PC.0), MISO(PC.1), CLK(PC.2), SS(PC.3), D3(PC.4), and D2(PC.5) */
    SYS->GPC_MFPL &= ~(SYS_GPC_MFPL_PC0MFP_Msk | SYS_GPC_MFPL_PC1MFP_Msk | SYS_GPC_MFPL_PC2MFP_Msk |
                       SYS_GPC_MFPL_PC3MFP_Msk | SYS_GPC_MFPL_PC4MFP_Msk | SYS_GPC_MFPL_PC5MFP_Msk);
    SYS->GPC_MFPL |= SYS_GPC_MFPL_PC0MFP_SPIM_MOSI | SYS_GPC_MFPL_PC1MFP_SPIM_MISO |
                     SYS_GPC_MFPL_PC2MFP_SPIM_CLK | SYS_GPC_MFPL_PC3MFP_SPIM_SS |
                     SYS_GPC_MFPL_PC4MFP_SPIM_D3 | SYS_GPC_MFPL_PC5MFP_SPIM_D2;
    PC->SMTEN |= GPIO_SMTEN_SMTEN2_Msk;

    /* Set SPIM I/O pins as high slew rate up to 80 MHz. */
    PC->SLEWCTL = (PC->SLEWCTL & 0xFFFFF000) |
                  (0x1<<GPIO_SLEWCTL_HSREN0_Pos) | (0x1<<GPIO_SLEWCTL_HSREN1_Pos) |
                  (0x1<<GPIO_SLEWCTL_HSREN2_Pos) | (0x1<<GPIO_SLEWCTL_HSREN3_Pos) |
                  (0x1<<GPIO_SLEWCTL_HSREN4_Pos) | (0x1<<GPIO_SLEWCTL_HSREN5_Pos);

    SPIM_SET_CLOCK_DIVIDER(1);        /* Set SPIM clock as HCLK divided by 2 */

    SPIM_SET_RXCLKDLY_RDDLYSEL(0);    /* Insert 0 delay cycle. Adjust the sampling clock of received data to latch the correct data. */
    SPIM_SET_RXCLKDLY_RDEDGE();       /* Use SPI input clock rising edge to sample received data. */

    SPIM_SET_DCNUM(8);                /* Set 8 dummy cycle. */

    if (SPIM_InitFlash(1) != 0)        /* Initialized SPI flash */
    {
        LogError(("[%s] SPIM flash initialize failed!\n", __FUNCTION__));
        goto lexit;
    }

    SPIM_ReadJedecId(idBuf, sizeof (idBuf), 1);
    LogInfo(("[%s] SPIM get JEDEC ID=0x%02X, 0x%02X, 0x%02X\n", __FUNCTION__, idBuf[0], idBuf[1], idBuf[2]));
    if( (idBuf[0] != 0xef) || (idBuf[1] != 0x40) || (idBuf[2] != 0x16) )
    {
        LogError(("[%s] SPIM get wrong JEDEC ID\n", __FUNCTION__));
        goto lexit;
    }    
    SPIM_Enable_4Bytes_Mode(USE_4_BYTES_MODE, 1);
    result = pdTRUE;
lexit:    
    /* Lock protected registers */
    SYS_LockReg();    
    return result;
}

bool_t NVT_SPI_Flash_Bank_Erase(uint32_t startAddress, uint32_t bankSize)
{
    uint32_t    i, offset;             /* variables */
    uint32_t    *pData;
    bool_t result = pdFALSE;
    
    /* Unlock protected registers */
    SYS_UnlockReg();
    
    /*
     *  Erase flash page
     */
    if( (startAddress + bankSize) > FLASH_BANK_UB )
    {
        LogError(("[%s] FAILED!\n", __FUNCTION__));
        LogError(("[%s] Exceed reserved Flash Bank boundary 0x%x > 0x%x \n", __FUNCTION__, (startAddress + bankSize), FLASH_BANK_UB));
        goto lexit;
    }
    LogInfo(("[%s] Erase SPI flash bank 0x%x ~ 0x%x ...", __FUNCTION__, startAddress, (startAddress + bankSize)));
    for (offset = 0; offset < bankSize; offset += FLASH_BLOCK_SIZE )
    {
      SPIM_EraseBlock(startAddress+offset, USE_4_BYTES_MODE, OPCODE_BE_64K, 1, 1);
    }
    LogInfo(("[%s] done.\n", __FUNCTION__));

    /*
     *  Verify flash page be erased
     */
    LogInfo(("[%s] Verify SPI flash block 0x%x be erased...", __FUNCTION__, startAddress));
    for (offset = 0; offset < bankSize; offset += BUFFER_SIZE)
    {
        memset(spi_buff, 0, BUFFER_SIZE);
        SPIM_IO_Read(startAddress+offset, USE_4_BYTES_MODE, BUFFER_SIZE, spi_buff, OPCODE_FAST_READ, 1, 1, 1, 1);

        pData = (uint32_t *)spi_buff;
        for (i = 0; i < BUFFER_SIZE; i += 4, pData++)
        {
            if (*pData != 0xFFFFFFFF)
            {
                LogError(("[%s] FAILED!\n", __FUNCTION__));
                LogError(("[%s] Flash address 0x%x, read 0x%x!\n", __FUNCTION__, startAddress+offset+i, *pData));
                goto lexit;
            }
        }
    }
    result = pdTRUE;
lexit:    
    /* Lock protected registers */
    SYS_LockReg();    
    return result;
}

/* Write a block of data to the specified file. 
   Returns the number of bytes written on success or negative error code.
*/
int32_t NVT_SPI_Flash_Block_Write(uint32_t ulOffset,
                           uint8_t * const pacData,
                           uint32_t ulBlockSize )
{
    int32_t lResult = 0;
    
    /* Unlock protected registers */
    SYS_UnlockReg();
    
    if( (ulOffset + ulBlockSize) > FLASH_BANK_UB )
    {
        LogError(("[%s] FAILED!\n", __FUNCTION__));
        LogError(("[%s] Exceed reserved Flash Bank boundary 0x%x > 0x%x \n", __FUNCTION__, (ulOffset + ulBlockSize), FLASH_BANK_UB));
        lResult = -1;
        goto lexit;
    }
    SPIM_IO_Write(ulOffset, USE_4_BYTES_MODE, ulBlockSize, pacData, OPCODE_PP, 1, 1, 1);
    lResult = ulBlockSize;
lexit:    
    /* Lock protected registers */
    SYS_LockReg();   
    return lResult;
}

/* Read a block of data to the specified file. 
   Returns the number of bytes read on success or negative error code.
*/
int32_t NVT_SPI_Flash_Block_Read(uint32_t ulOffset,
                           uint8_t* pacData,
                           uint32_t ulBlockSize )
{
    int32_t lResult = 0;

    
    /* Unlock protected registers */
    SYS_UnlockReg();
    
    if( (ulOffset + ulBlockSize) > FLASH_BANK_UB )
    {
        LogError(("[%s] FAILED!\n"));
        LogError(("[%s] Exceed reserved Flash Bank boundary 0x%x > 0x%x \n", __FUNCTION__, (ulOffset + ulBlockSize), FLASH_BANK_UB));
        lResult = -1;
        goto lexit;
    }
    SPIM_IO_Read(ulOffset, USE_4_BYTES_MODE, ulBlockSize, pacData, OPCODE_FAST_DUAL_READ, 1, 1, 2, 1);
    lResult = ulBlockSize;
lexit:    
    /* Lock protected registers */
    SYS_LockReg();   
    return lResult;
}