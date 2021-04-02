/*
 * Licensed to the Apache Software Foundation (ASF) under one
 * or more contributor license agreements.  See the NOTICE file
 * distributed with this work for additional information
 * regarding copyright ownership.  The ASF licenses this file
 * to you under the Apache License, Version 2.0 (the
 * "License"); you may not use this file except in compliance
 * with the License.  You may obtain a copy of the License at
 *
 *  http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing,
 * software distributed under the License is distributed on an
 * "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
 * KIND, either express or implied.  See the License for the
 * specific language governing permissions and limitations
 * under the License.
 */

/*
 * Modifications are Copyright (c) 2019 Arm Limited.
 */

#include <assert.h>
#include <string.h>
#include <inttypes.h>
#include <stddef.h>

#include "sysflash/sysflash.h"
#include "flash_map_backend/flash_map_backend.h"

#include "bootutil/image.h"
#include "bootutil/bootutil.h"
#include "bootutil_priv.h"
#include "bootutil/bootutil_log.h"
#ifdef MCUBOOT_ENC_IMAGES
#include "bootutil/enc_key.h"
#endif

MCUBOOT_LOG_MODULE_DECLARE(mcuboot);

#define LOG_SWAP_STATE(area, state)                            \
    printf("%s: magic=%s, swap_type=0x%x, copy_done=0x%x, "   \
                 "image_ok=0x%x\n",                                   \
                 (area),                                            \
                 ((state)->magic == BOOT_MAGIC_GOOD ? "good" :      \
                  (state)->magic == BOOT_MAGIC_UNSET ? "unset" :    \
                  "bad"),                                           \
                 (state)->swap_type,                                \
                 (state)->copy_done,                                \
                 (state)->image_ok)



const uint32_t boot_img_magic[] = {
    0xf395c277,
    0x7fefd260,
    0x0f505235,
    0x8079b62c,
};

#define BOOT_MAGIC_ARR_SZ \
    (sizeof boot_img_magic / sizeof boot_img_magic[0])

static inline uint32_t
boot_magic_off(const struct flash_area *fap)
{
    return fap->fa_size - BOOT_MAGIC_SZ;
}

static inline uint32_t
boot_image_ok_off(const struct flash_area *fap)
{
    return boot_magic_off(fap) - BOOT_MAX_ALIGN;
}

int
boot_write_magic(const struct flash_area *fap)
{
    uint32_t off;
    int rc;

    off = boot_magic_off(fap);

    rc = flash_area_write(fap, off, boot_img_magic, BOOT_MAGIC_SZ);
    if (rc != 0) {
        return BOOT_EFLASH;
    }

    return 0;
}

/**
 * Marks the image in the secondary slot as pending.  On the next reboot,
 * the system will perform a one-time boot of the the secondary slot image.
 *
 * @param image             0 = Secondary_Slot_1, 1 = Secondary_Slot_2
 * @param permanent         Whether the image should be used permanently or
 *                              only tested once:
 *                                  0=run image once, then confirm or revert.
 *                                  1=run image forever.
 *
 * @return                  0 on success; nonzero on failure.
 */
int
boot_set_pending(int image, int permanent)
{
    const struct flash_area *fap;
    int                     rc;

    rc = flash_area_open(FLASH_AREA_IMAGE_SECONDARY(image), &fap);
    if (rc == 0)
    {
        rc = boot_write_magic(fap);
        if (rc != 0)
        {
            printf("%s() %d FAILED TO WRITE MAGIC\n", __func__, __LINE__);
        }
        flash_area_close(fap);
    }

    return rc;
}

/**
 * Marks the image in the primary slot as confirmed.  The system will continue
 * booting into the image in the primary slot until told to boot from a
 * different slot.
 *
 * @return                  0 on success; nonzero on failure.
 */
int
boot_set_confirmed(void)
{
    const struct flash_area *fap;
    int                     rc;
    uint32_t                off;
    uint8_t                 buff[1];

    rc = flash_area_open(FLASH_AREA_IMAGE_PRIMARY(0), &fap);
    if (rc == 0)
    {
        rc = boot_write_magic(fap);

        off = boot_image_ok_off(fap);
        buff[0] = BOOT_FLAG_SET;
        rc = flash_area_write(fap, off, buff, 1);
        flash_area_close(fap);
    }

    return rc;
}
