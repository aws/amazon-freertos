/*
 * Copyright (c) 2018-2020, Texas Instruments Incorporated
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * *  Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * *  Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * *  Neither the name of Texas Instruments Incorporated nor the names of
 *    its contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
/*
 *  ======== SPIFFSNVS.c ========
 */
#include "spiffs_config.h"
#include "spiffs.h"
#include "SPIFFSNVS.h"

#include <ti/drivers/NVS.h>
#include <ti/drivers/dpl/MutexP.h>

static s32_t spiffs_hal_erase(spiffs *fs, u32_t addr, u32_t size);
static s32_t spiffs_hal_read(spiffs *fs, u32_t addr, u32_t size, u8_t *dst);
static s32_t spiffs_hal_write(spiffs *fs, u32_t addr, u32_t size, u8_t *src);

/*
 *  ======== SPIFFSNVS_close ========
 */
void SPIFFSNVS_close(SPIFFSNVS_Data *spiffsnvsData)
{
    NVS_close(spiffsnvsData->nvsHandle);
}

/*
 *  ======== SPIFFSNVS_config ========
 */
s32_t SPIFFSNVS_config(SPIFFSNVS_Data *spiffsnvsData,
    u32_t nvsIndex, spiffs *fs, spiffs_config *fsConfig,
    u32_t logicalBlockSize, u32_t logicalPageSize)
{
    NVS_Attrs nvsAttrs;

    NVS_init();

    /* Open the NVS region to be used for the file system */
    spiffsnvsData->nvsHandle = NVS_open(nvsIndex, NULL);
    if (spiffsnvsData->nvsHandle == NULL) {
        return (SPIFFSNVS_STATUS_INV_NVS_IDX);
    }

    NVS_getAttrs(spiffsnvsData->nvsHandle, &nvsAttrs);

    /*
     * Logical block size must be an integer multiple of the erase sector size;
     * fail if an invalid logical block size is attempted.
     */
    if ((logicalBlockSize == 0) ||
        (logicalBlockSize < nvsAttrs.sectorSize) ||
        (logicalBlockSize % nvsAttrs.sectorSize)) {
        NVS_close(spiffsnvsData->nvsHandle);

        return (SPIFFSNVS_STATUS_INV_BLOCK_SIZE);
    }

    /*
     * Logical block size must be an integer multiple of the logical page size
     */
    if ((logicalPageSize == 0) ||
        (logicalBlockSize < logicalPageSize) ||
        (logicalBlockSize % logicalPageSize)) {
        NVS_close(spiffsnvsData->nvsHandle);

        return (SPIFFSNVS_STATUS_INV_PAGE_SIZE);
    }

#if CC13XX_CC26XX_WRITE_LIMIT
    /*
     * For CC13XX and CC26XX ensure there are no more than 32 logical pages in
     * a logical block.  Allowing more than 32 pages per block would allow
     * situations where the 83 write limit could be violated on the index page.
     */
    if (logicalBlockSize / logicalPageSize > 32) {
        NVS_close(spiffsnvsData->nvsHandle);

        return (SPIFFSNVS_STATUS_INV_PAGE_SIZE);
    }
#endif

    spiffsnvsData->lock.mutex = MutexP_create(NULL);
    if (spiffsnvsData->lock.mutex == NULL) {
        NVS_close(spiffsnvsData->nvsHandle);

        return (SPIFFSNVS_STATUS_ERROR);
    }
    spiffsnvsData->lock.count = 0;

    /* Initialize SPIFFS configuration structure using the following: */
    fsConfig->phys_size = nvsAttrs.regionSize;
    fsConfig->phys_addr = 0;    /* Beginning of NVS region */
    fsConfig->phys_erase_block = nvsAttrs.sectorSize;
    fsConfig->log_block_size = logicalBlockSize;
    fsConfig->log_page_size = logicalPageSize;

    /* Set hardware abstraction functions to communicate with FLASH memory */
    fsConfig->hal_erase_f = &spiffs_hal_erase;
    fsConfig->hal_read_f = &spiffs_hal_read;
    fsConfig->hal_write_f = &spiffs_hal_write;

    /*
     * Store SPIFFSNVS object in the file system object; it is required by hardware
     * abstraction and lock/unlock functions.
     */
    fs->user_data = spiffsnvsData;

    return (SPIFFSNVS_STATUS_SUCCESS);
}

/*
 *  ======== SPIFFSNVS_lock ========
 */
void SPIFFSNVS_lock(void *fs)
{
    uintptr_t key;
    SPIFFSNVS_Lock *lock;
    lock = &(((SPIFFSNVS_Data *) ((spiffs *) fs)->user_data)->lock);

    key = MutexP_lock(lock->mutex);
    lock->keys[lock->count] = key;
    lock->count++;
}

/*
 *  ======== SPIFFSNVS_unlock ========
 */
void SPIFFSNVS_unlock(void *fs)
{
    SPIFFSNVS_Lock *lock;
    lock = &(((SPIFFSNVS_Data *) ((spiffs *)fs)->user_data)->lock);

    lock->count--;
    MutexP_unlock(lock->mutex, lock->keys[lock->count]);
}

/*
 *  ======== spiffs_hal_erase ========
 */
static s32_t spiffs_hal_erase(spiffs *fs, u32_t addr, u32_t size)
{
    s32_t      status;
    NVS_Handle handle = ((SPIFFSNVS_Data *) fs->user_data)->nvsHandle;

    status = NVS_erase(handle, addr, size);
    status = (status == NVS_STATUS_SUCCESS) ? SPIFFS_OK :
        SPIFFS_ERR_ERASE_FAIL;

    return (status);
}

/*
 *  ======== spiffs_hal_read ========
 */
static s32_t spiffs_hal_read(spiffs *fs, u32_t addr, u32_t size, u8_t *dst)
{
    s32_t      status;
    NVS_Handle handle = ((SPIFFSNVS_Data *) fs->user_data)->nvsHandle;

    status = NVS_read(handle, addr, dst, size);
    status = (status == NVS_STATUS_SUCCESS) ? SPIFFS_OK :
            SPIFFS_ERR_NOT_READABLE;

    return (status);
}

/*
 *  ======== spiffs_hal_write ========
 */
static s32_t spiffs_hal_write(spiffs *fs, u32_t addr, u32_t size, u8_t *src)
{
    s32_t      status;
    NVS_Handle handle = ((SPIFFSNVS_Data *) fs->user_data)->nvsHandle;

    status = NVS_write(handle, addr, src, size, 0);
    status = (status == NVS_STATUS_SUCCESS) ? SPIFFS_OK :
        SPIFFS_ERR_NOT_WRITABLE;

    return (status);
}
