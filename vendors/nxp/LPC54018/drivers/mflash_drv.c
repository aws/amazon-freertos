/*
 * Copyright 2017 NXP
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * o Redistributions of source code must retain the above copyright notice, this list
 *   of conditions and the following disclaimer.
 *
 * o Redistributions in binary form must reproduce the above copyright notice, this
 *   list of conditions and the following disclaimer in the documentation and/or
 *   other materials provided with the distribution.
 *
 * o Neither the name of the copyright holder nor the names of its
 *   contributors may be used to endorse or promote products derived from this
 *   software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
 * ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "fsl_spifi.h"
#include "mflash_drv.h"
#include "pin_mux.h"
#include <stdbool.h>

/* Command ID */
#define COMMAND_NUM                 (6)
#define READ                        (0)
#define PROGRAM_PAGE                (1)
#define GET_STATUS                  (2)
#define ERASE_SECTOR                (3)
#define WRITE_ENABLE                (4)
#define WRITE_REGISTER              (5)


//#ifdef XIP_IMAGE
//#warning NOTE: MFLASH driver expects that application runs from XIP
//#else
//#warning NOTE: MFLASH driver expects that application runs from SRAM
//#endif


#if !defined(FLASHDRV_SMART_UPDATE) && (MFLASH_SECTOR_SIZE / MFLASH_PAGE_SIZE <= 32)
#define FLASHDRV_SMART_UPDATE 1
#endif


/* Temporary sector storage. Use uint32_t type to force 4B alignment and
 * improve copy operation */
static uint32_t g_flashm_sector[MFLASH_SECTOR_SIZE / sizeof(uint32_t)];


/* Commands definition, taken from SPIFI demo */
static spifi_command_t command[COMMAND_NUM] = {
    /* read */
    {MFLASH_PAGE_SIZE, false, kSPIFI_DataInput, 1, kSPIFI_CommandAllSerial, kSPIFI_CommandOpcodeAddrThreeBytes, 0x0B},
    /* program */
    {MFLASH_PAGE_SIZE, false, kSPIFI_DataOutput, 0, kSPIFI_CommandAllSerial, kSPIFI_CommandOpcodeAddrThreeBytes, 0x2},
    /* status */
    {1, false, kSPIFI_DataInput, 0, kSPIFI_CommandAllSerial, kSPIFI_CommandOpcodeOnly, 0x05},
    /* erase */
    {0, false, kSPIFI_DataOutput, 0, kSPIFI_CommandAllSerial, kSPIFI_CommandOpcodeAddrThreeBytes, 0x20},
    /* write enable */
    {0, false, kSPIFI_DataOutput, 0, kSPIFI_CommandAllSerial, kSPIFI_CommandOpcodeOnly, 0x06},
    /* write register */
    {4, false, kSPIFI_DataOutput, 0, kSPIFI_CommandAllSerial, kSPIFI_CommandOpcodeOnly, 0x01}
};


/* Wait until command finishes */
static inline void mflash_drv_check_if_finish(void)
{
    uint8_t val = 0;
    do
    {
        SPIFI_SetCommand(MFLASH_SPIFI, &command[GET_STATUS]);
        while ((MFLASH_SPIFI->STAT & SPIFI_STAT_INTRQ_MASK) == 0U)
        {
        }
        val = SPIFI_ReadDataByte(MFLASH_SPIFI);
    } while (val & 0x1);
}

/* return offset from sector */
static void mflash_drv_read_mode(void)
{
    /* Switch back to read mode */
    SPIFI_ResetCommand(MFLASH_SPIFI);
    SPIFI_SetMemoryCommand(MFLASH_SPIFI, &command[READ]);
}


/* Initialize SPIFI & flash peripheral,
 * cannot be invoked directly, requires calling wrapper in non XIP memory */
static int32_t mflash_drv_init_internal(void)
{
    /* NOTE: Multithread access is not supported for SRAM target.
     *       XIP target MUST be protected by disabling global interrupts
     *       since all ISR (and API that is used inside) is placed at XIP.
     *       It is necessary to place at least "mflash_drv_drv.o", "fsl_spifi.o" to SRAM */
    /* disable interrupts when running from XIP
     * TODO: store/restore previous PRIMASK on stack to avoid
     * failure in case of nested critical sections !! */
    uint32_t primask = __get_PRIMASK();

    __asm("cpsid i");

    spifi_config_t config = {0};

#ifndef XIP_IMAGE
    uint32_t sourceClockFreq;
    BOARD_InitSPIFI();
    /* Reset peripheral */
    RESET_PeripheralReset(kSPIFI_RST_SHIFT_RSTn);
    /* Set SPIFI clock source */
    CLOCK_AttachClk(kFRO_HF_to_SPIFI_CLK);
    sourceClockFreq = CLOCK_GetSpifiClkFreq();
    /* Set the clock divider */
    CLOCK_SetClkDiv(kCLOCK_DivSpifiClk, sourceClockFreq / MFLASH_BAUDRATE, false);
    /* Enable SPIFI clock */
    CLOCK_EnableClock(kCLOCK_Spifi);
#endif

    SPIFI_GetDefaultConfig(&config);
    config.dualMode = kSPIFI_DualMode;
#ifdef XIP_IMAGE
    config.disablePrefetch = false; // true;
    config.disableCachePrefech = false; // true;
#else
    config.disablePrefetch = false; // true;
    config.disableCachePrefech = false; // true;
#endif

    /* Reset the Command register */
    SPIFI_ResetCommand(MFLASH_SPIFI);

    /* Set time delay parameter */
    MFLASH_SPIFI->CTRL = SPIFI_CTRL_TIMEOUT(config.timeout) | SPIFI_CTRL_CSHIGH(config.csHighTime) |
                 SPIFI_CTRL_D_PRFTCH_DIS(config.disablePrefetch) | SPIFI_CTRL_MODE3(config.spiMode) |
                 SPIFI_CTRL_PRFTCH_DIS(config.disableCachePrefech) | SPIFI_CTRL_DUAL(config.dualMode) |
                 SPIFI_CTRL_RFCLK(config.isReadFullClockCycle) | SPIFI_CTRL_FBCLK(config.isFeedbackClock);

    mflash_drv_read_mode();

    if (primask == 0)
    {
        __asm("cpsie i");
    }

    return 0;
}


/* API - initialize 'mflash' */
int32_t mflash_drv_init(void)
{
    volatile int32_t result;
    /* Necessary to have double wrapper call in non_xip memory */
    result = mflash_drv_init_internal();
    return result;
}


/* Internal - erase single sector */
static int32_t mflash_drv_sector_erase(uint32_t sector_addr)
{
    uint32_t primask = __get_PRIMASK();

    __asm("cpsid i");

    /* Reset the SPIFI to switch to command mode */
    SPIFI_ResetCommand(MFLASH_SPIFI);

    /* Write enable */
    SPIFI_SetCommand(MFLASH_SPIFI, &command[WRITE_ENABLE]);
    /* Set address */
    SPIFI_SetCommandAddress(MFLASH_SPIFI, sector_addr);
    /* Erase sector */
    SPIFI_SetCommand(MFLASH_SPIFI, &command[ERASE_SECTOR]);
    /* Check if finished */
    mflash_drv_check_if_finish();
    /* Switch to read mode to enable interrupts as soon ass possible */
    mflash_drv_read_mode();

    if (primask == 0)
    {
        __asm("cpsie i");
    }

    /* Flush pipeline to allow pending interrupts take place */
    __ISB();

    return 0;
}


/* Internal - write single page */
static int32_t mflash_drv_page_program(uint32_t page_addr, const uint32_t *page_data)
{
    uint32_t primask = __get_PRIMASK();

    __asm("cpsid i");

    /* Program page */
    SPIFI_ResetCommand(MFLASH_SPIFI);
    SPIFI_SetCommand(MFLASH_SPIFI, &command[WRITE_ENABLE]);
    SPIFI_SetCommandAddress(MFLASH_SPIFI, page_addr);
    SPIFI_SetCommand(MFLASH_SPIFI, &command[PROGRAM_PAGE]);

    /* Store 4B in each loop. Sector has always 4B alignment and size multiple of 4 */
    for (uint32_t i = 0; i < MFLASH_PAGE_SIZE/sizeof(page_data[0]); i++)
    {
        SPIFI_WriteData(MFLASH_SPIFI, page_data[i]);
    }

    mflash_drv_check_if_finish();
    /* Switch to read mode to enable interrupts as soon ass possible */
    mflash_drv_read_mode();

    if (primask == 0)
    {
        __asm("cpsie i");
    }

    /* Flush pipeline to allow pending interrupts take place */
    __ISB();

    return 0;
}


#if !defined(FLASHDRV_SMART_UPDATE) || (FLASHDRV_SMART_UPDATE == 0)
/* Internal - write whole sector */
static int32_t mflash_drv_sector_program(uint32_t sector_addr, const uint32_t *sector_data)
{
    if (false == mflash_drv_is_sector_aligned((uint32_t)sector_addr))
        return -1;

    uint32_t max_page = MFLASH_SECTOR_SIZE / MFLASH_PAGE_SIZE;
    uint32_t page_addr = sector_addr;
    const uint32_t *page_data = sector_data;

    for (uint32_t page_idx = 0; page_idx < max_page; page_idx++)
    {
        mflash_drv_page_program(page_addr, page_data);
        page_addr += MFLASH_PAGE_SIZE;
        page_data += MFLASH_PAGE_SIZE/sizeof(page_data[0]);
    }

    return 0;
}
#endif


/* Internal - write data of 'data_len' to single sector 'sector_addr', starting from 'sect_off' */
static int32_t mflash_drv_sector_update(uint32_t sector_addr, uint32_t sect_off, const uint8_t *data, uint32_t data_len)
{
#if FLASHDRV_SMART_UPDATE
    int sector_erase_req = 0;
    uint32_t page_program_map = 0; /* Current implementation is limited to 32 pages per sector */
#endif

    /* Address not aligned to sector boundary */
    if (false == mflash_drv_is_sector_aligned((uint32_t)sector_addr))
        return -1;
    /* Offset + length exceeed sector size */
    if (sect_off + data_len > MFLASH_SECTOR_SIZE)
        return -1;

    /* Switch back to read mode */
    mflash_drv_read_mode();

    /* Copy old sector data by 4B in each loop to buffer */
    for (uint32_t i = 0; i < sizeof(g_flashm_sector)/sizeof(g_flashm_sector[0]); i++)
    {
        g_flashm_sector[i] = *((uint32_t*)(sector_addr) + i);
    }

#if FLASHDRV_SMART_UPDATE /* Perform only the erase/program operations that are necessary */

    /* Copy custom data (1B in each loop) to buffer at specific position */
    for (uint32_t i = 0; i < data_len; i++)
    {
        /* Unless it was already decided to erase the whole sector, evaluate differences between current and new data */
        if (0 == sector_erase_req)
        {
            uint8_t cur_value = ((uint8_t*)(g_flashm_sector))[sect_off + i];
            uint8_t new_value = data[i];

            /* Check the the bit transitions */
            if ((cur_value | new_value) != cur_value)
            {
                sector_erase_req = 1; /* A bit needs to be flipped from 0 to 1, the sector has to be erased */
            }
            else if ((cur_value & new_value) != cur_value)
            {
                page_program_map |= 1 << ((sect_off + i) / MFLASH_PAGE_SIZE); /* A bit needs to be flipped from 1 to 0, the page has to be programmed */
            }
        }

        /* Copy data over to the buffer */
        ((uint8_t*)g_flashm_sector)[sect_off + i] = data[i];
    }

    /* Erase the sector if required */
    if (0 != sector_erase_req)
    {
        if (0 != mflash_drv_sector_erase(sector_addr))
        {
            return -2;
        }

        /* Update page program map according to non-blank areas in the buffer */
        for (int page_idx = 0; page_idx < MFLASH_SECTOR_SIZE / MFLASH_PAGE_SIZE; page_idx++)
        {
            int page_word_start = page_idx * (MFLASH_PAGE_SIZE / sizeof(g_flashm_sector[0]));
            int page_word_end = page_word_start + (MFLASH_PAGE_SIZE / sizeof(g_flashm_sector[0]));

            for (int i = page_word_start; i < page_word_end ; i++)
            {
                if (g_flashm_sector[i] != 0xFFFFFFFF)
                {
                    /* Mark the page for programming and go for next one */
                    page_program_map |= (1 << page_idx);
                    break;
                }
            }
        }
    }

    /* Program the pages */
    for (int page_idx = 0; page_idx < MFLASH_SECTOR_SIZE / MFLASH_PAGE_SIZE; page_idx++)
    {
        /* Skip programming of blank pages */
        if (0 == (page_program_map & (1 << page_idx)))
        {
            continue; /* The page needs not be programmed, skip it */
        }

        mflash_drv_page_program(sector_addr + page_idx * MFLASH_PAGE_SIZE, g_flashm_sector + page_idx * (MFLASH_PAGE_SIZE / sizeof(g_flashm_sector[0])));
    }

#else /* Erase the sector and all the pages unconditionally */

    /* Copy custom data (1B in each loop) to buffer at specific position */
    for (uint32_t i = 0; i < data_len; i++)
    {
        ((uint8_t*)g_flashm_sector)[sect_off + i] = data[i];
    }

    /* Erase the sector */
    if (0 != mflash_drv_sector_erase(sector_addr))
    {
        return -2;
    }

    /* Program whole sector */
    if (0 != mflash_drv_sector_program(sector_addr, g_flashm_sector))
    {
        return -2;
    }

#endif /* FLASHDRV_SMART_UPDATE */

    /* Switch back to read mode */
    mflash_drv_read_mode();
    return 0;
}


/* Erase area of flash, cannot be invoked directly, requires calling wrapper in non XIP memory */
int32_t mflash_drv_erase_internal(void *addr, uint32_t len)
{
    uint32_t sector_addr;

    /* Address not aligned to sector boundary */
    if (false == mflash_drv_is_sector_aligned((uint32_t)addr))
        return -1;

    /* Length is not aligned to sector size */
    if (0 != len % MFLASH_SECTOR_SIZE)
        return -1;

    sector_addr = (uint32_t)addr;
    while (len)
    {
        /* Perform blank-check of the sector and erase it if necessary */
        for (uint32_t i = 0; i < MFLASH_SECTOR_SIZE / sizeof(uint32_t); i++)
        {
            if (0xFFFFFFFF != *((uint32_t*)(sector_addr) + i))
            {
                mflash_drv_sector_erase(sector_addr);
                break;
            }
        }
        sector_addr += MFLASH_SECTOR_SIZE;
        len -= MFLASH_SECTOR_SIZE;
    }

    return 0;
}


/* Write data to flash, cannot be invoked directly, requires calling wrapper in non XIP memory */
int32_t mflash_drv_write_internal(void *any_addr, const uint8_t *data, uint32_t data_len)
{

    uint32_t sect_size = MFLASH_SECTOR_SIZE;
    /* Interval <0, sector_size) */
    uint32_t to_write = 0;
    /* Interval (data_len, 0>  */
    uint32_t to_remain = data_len;
    int32_t result = 0;

    for (
         /* Calculate address of first sector */
         uint32_t sect_a = mflash_drv_addr_to_sector_addr((uint32_t)any_addr),
         /* and first sector offset */
         sect_of = mflash_drv_addr_to_sector_of((uint32_t)any_addr),
         /* and set first data offset to 0*/
         data_of = 0;
         /* Continue until sector address exceed target adddress + data_length */
         sect_a < ((uint32_t)any_addr) + data_len;
         /* Move to next sector */
         sect_a += sect_size,
         /* and move pointer to data */
         data_of += to_write
    )
    {
        /* If remaining data is exceed 'sector_size', write 'sector_size' length */
        if (to_remain > sect_size - sect_of)
        {
            to_write = sect_size - sect_of;
            to_remain = to_remain - to_write;
        }
        /* else write remaining data length */
        else
        {
            to_write = to_remain;
            to_remain = 0;
        }

        /* Write at 'sect_a' sector, starting at 'sect_of' using '&data[data_of]' of length 'to_write' */
        result = mflash_drv_sector_update(sect_a, sect_of, data + data_of, to_write);
        if (0 != result)
            return -1;
        /* Only first sector is allowed to have an offset */
        sect_of = 0;
    }

    return 0;
}


/* Calling wrapper for 'mflash_drv_write_internal'.
 * Write 'data' of 'data_len' to 'any_addr' - which doesn't have to be sector aligned.
 * NOTE: Don't try to store constant data that are located in XIP !!
 */
int32_t mflash_drv_write(void *any_addr, const uint8_t *data, uint32_t data_len)
{
    volatile int32_t result;
    result = mflash_drv_write_internal(any_addr, data, data_len);
    return result;
}


#if 0
/* Dummy test to prove functionality */
volatile uint32_t lock2 = 1;
char * tmp_string = "Curabitur sit amet justo ac velit consectetur lobortis. Donec porttitor, eros sed sollicitudin viverra, massa enim placerat sapien, et vestibulum lacus quam et leo. Proin gravida";

void mflash_drv_do_test(void)
{
    lock2 = 1;
    while (lock2);

    // place to SRAM !!!
#ifndef XIP_IMAGE
    BOARD_InitSPIFI();
#endif

    /* Reset peripheral */
    lock2 = 1;
    while (lock2);
    mflash_drv_init();

    lock2 = 1;
    while (lock2)
    mflash_drv_write((void*)(0x10800010), tmp_string, 30);//TODO force read to recover

    lock2 = 1;
    while (lock2);
}
#endif



