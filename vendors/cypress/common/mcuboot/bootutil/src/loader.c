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

/**
 * This file provides an interface to the boot loader.  Functions defined in
 * this file should only be called while the boot loader is running.
 */

#include <assert.h>
#include <stddef.h>
#include <stdbool.h>
#include <inttypes.h>
#include <stdlib.h>
#include <string.h>
#include <os/os_malloc.h>
#include "bootutil/bootutil.h"
#include "bootutil/image.h"
#include "bootutil_priv.h"
#include "bootutil/bootutil_log.h"

#ifdef MCUBOOT_ENC_IMAGES
#include "bootutil/enc_key.h"
#endif

#include "mcuboot_config/mcuboot_config.h"

MCUBOOT_LOG_MODULE_DECLARE(mcuboot);

static struct boot_loader_state boot_data;

#if (BOOT_IMAGE_NUMBER > 1)
#define IMAGES_ITER(x) for ((x) = 0; (x) < BOOT_IMAGE_NUMBER; ++(x))
#else
#define IMAGES_ITER(x)
#endif

/*
 * This macro allows some control on the allocation of local variables.
 * When running natively on a target, we don't want to allocated huge
 * variables on the stack, so make them global instead. For the simulator
 * we want to run as many threads as there are tests, and it's safer
 * to just make those variables stack allocated.
 */
#if !defined(__BOOTSIM__)
#define TARGET_STATIC static
#else
#define TARGET_STATIC
#endif

#if defined(MCUBOOT_VALIDATE_PRIMARY_SLOT) && !defined(MCUBOOT_OVERWRITE_ONLY)
/*
 * FIXME: this might have to be updated for threaded sim
 */
static int boot_status_fails = 0;
#define BOOT_STATUS_ASSERT(x)                \
    do {                                     \
        if (!(x)) {                          \
            boot_status_fails++;             \
        }                                    \
    } while (0)
#else
#define BOOT_STATUS_ASSERT(x) ASSERT(x)
#endif

struct boot_status_table {
    uint8_t bst_magic_primary_slot;
    uint8_t bst_magic_scratch;
    uint8_t bst_copy_done_primary_slot;
    uint8_t bst_status_source;
};

/**
 * This set of tables maps swap state contents to boot status location.
 * When searching for a match, these tables must be iterated in order.
 */
static const struct boot_status_table boot_status_tables[] = {
    {
        /*           | primary slot | scratch      |
         * ----------+--------------+--------------|
         *     magic | Good         | Any          |
         * copy-done | Set          | N/A          |
         * ----------+--------------+--------------'
         * source: none                            |
         * ----------------------------------------'
         */
        .bst_magic_primary_slot =     BOOT_MAGIC_GOOD,
        .bst_magic_scratch =          BOOT_MAGIC_NOTGOOD,
        .bst_copy_done_primary_slot = BOOT_FLAG_SET,
        .bst_status_source =          BOOT_STATUS_SOURCE_NONE,
    },

    {
        /*           | primary slot | scratch      |
         * ----------+--------------+--------------|
         *     magic | Good         | Any          |
         * copy-done | Unset        | N/A          |
         * ----------+--------------+--------------'
         * source: primary slot                    |
         * ----------------------------------------'
         */
        .bst_magic_primary_slot =     BOOT_MAGIC_GOOD,
        .bst_magic_scratch =          BOOT_MAGIC_NOTGOOD,
        .bst_copy_done_primary_slot = BOOT_FLAG_UNSET,
        .bst_status_source =          BOOT_STATUS_SOURCE_PRIMARY_SLOT,
    },

    {
        /*           | primary slot | scratch      |
         * ----------+--------------+--------------|
         *     magic | Any          | Good         |
         * copy-done | Any          | N/A          |
         * ----------+--------------+--------------'
         * source: scratch                         |
         * ----------------------------------------'
         */
        .bst_magic_primary_slot =     BOOT_MAGIC_ANY,
        .bst_magic_scratch =          BOOT_MAGIC_GOOD,
        .bst_copy_done_primary_slot = BOOT_FLAG_ANY,
        .bst_status_source =          BOOT_STATUS_SOURCE_SCRATCH,
    },
    {
        /*           | primary slot | scratch      |
         * ----------+--------------+--------------|
         *     magic | Unset        | Any          |
         * copy-done | Unset        | N/A          |
         * ----------+--------------+--------------|
         * source: varies                          |
         * ----------------------------------------+--------------------------+
         * This represents one of two cases:                                  |
         * o No swaps ever (no status to read, so no harm in checking).       |
         * o Mid-revert; status in primary slot.                              |
         * -------------------------------------------------------------------'
         */
        .bst_magic_primary_slot =     BOOT_MAGIC_UNSET,
        .bst_magic_scratch =          BOOT_MAGIC_ANY,
        .bst_copy_done_primary_slot = BOOT_FLAG_UNSET,
        .bst_status_source =          BOOT_STATUS_SOURCE_PRIMARY_SLOT,
    },
};

#define BOOT_STATUS_TABLES_COUNT \
    (sizeof boot_status_tables / sizeof boot_status_tables[0])

#define BOOT_LOG_SWAP_STATE(area, state)                            \
    BOOT_LOG_INF("%s: magic=%s, swap_type=0x%x, copy_done=0x%x, "   \
                 "image_ok=0x%x",                                   \
                 (area),                                            \
                 ((state)->magic == BOOT_MAGIC_GOOD ? "good" :      \
                  (state)->magic == BOOT_MAGIC_UNSET ? "unset" :    \
                  "bad"),                                           \
                 (state)->swap_type,                                \
                 (state)->copy_done,                                \
                 (state)->image_ok)

/**
 * Determines where in flash the most recent boot status is stored. The boot
 * status is necessary for completing a swap that was interrupted by a boot
 * loader reset.
 *
 * @return      A BOOT_STATUS_SOURCE_[...] code indicating where status should
 *              be read from.
 */
static int
boot_status_source(struct boot_loader_state *state)
{
    const struct boot_status_table *table;
    struct boot_swap_state state_scratch;
    struct boot_swap_state state_primary_slot;
    int rc;
    size_t i;
    uint8_t source;
    uint8_t image_index;

#if (BOOT_IMAGE_NUMBER == 1)
    (void)state;
#endif

    image_index = BOOT_CURR_IMG(state);
    rc = boot_read_swap_state_by_id(FLASH_AREA_IMAGE_PRIMARY(image_index),
            &state_primary_slot);
    assert(rc == 0);

    rc = boot_read_swap_state_by_id(FLASH_AREA_IMAGE_SCRATCH, &state_scratch);
    assert(rc == 0);

    BOOT_LOG_SWAP_STATE("Primary image", &state_primary_slot);
    BOOT_LOG_SWAP_STATE("Scratch", &state_scratch);

    for (i = 0; i < BOOT_STATUS_TABLES_COUNT; i++) {
        table = &boot_status_tables[i];

        if (boot_magic_compatible_check(table->bst_magic_primary_slot,
                          state_primary_slot.magic) &&
            boot_magic_compatible_check(table->bst_magic_scratch,
                          state_scratch.magic) &&
            (table->bst_copy_done_primary_slot == BOOT_FLAG_ANY ||
             table->bst_copy_done_primary_slot == state_primary_slot.copy_done))
        {
            source = table->bst_status_source;

#if (BOOT_IMAGE_NUMBER > 1)
            /* In case of multi-image boot it can happen that if boot status
             * info is found on scratch area then it does not belong to the
             * currently examined image.
             */
            if (source == BOOT_STATUS_SOURCE_SCRATCH &&
                state_scratch.image_num != BOOT_CURR_IMG(state)) {
                source = BOOT_STATUS_SOURCE_NONE;
            }
#endif

            BOOT_LOG_INF("Boot source: %s",
                         source == BOOT_STATUS_SOURCE_NONE ? "none" :
                         source == BOOT_STATUS_SOURCE_SCRATCH ? "scratch" :
                         source == BOOT_STATUS_SOURCE_PRIMARY_SLOT ?
                                   "primary slot" : "BUG; can't happen");
            return source;
        }
    }

    BOOT_LOG_INF("Boot source: none");
    return BOOT_STATUS_SOURCE_NONE;
}

/*
 * Compute the total size of the given image.  Includes the size of
 * the TLVs.
 */
#if !defined(MCUBOOT_OVERWRITE_ONLY) || defined(MCUBOOT_OVERWRITE_ONLY_FAST)
static int
boot_read_image_size(struct boot_loader_state *state, int slot, uint32_t *size)
{
    const struct flash_area *fap;
    struct image_tlv_info info;
    uint32_t off;
    uint32_t protect_tlv_size;
    int area_id;
    int rc;

#if (BOOT_IMAGE_NUMBER == 1)
    (void)state;
#endif

    area_id = flash_area_id_from_multi_image_slot(BOOT_CURR_IMG(state), slot);
    rc = flash_area_open(area_id, &fap);
    if (rc != 0) {
        rc = BOOT_EFLASH;
        goto done;
    }

    off = BOOT_TLV_OFF(boot_img_hdr(state, slot));

    if (flash_area_read(fap, off, &info, sizeof(info))) {
        rc = BOOT_EFLASH;
        goto done;
    }

    protect_tlv_size = boot_img_hdr(state, slot)->ih_protect_tlv_size;
    if (info.it_magic == IMAGE_TLV_PROT_INFO_MAGIC) {
        if (protect_tlv_size != info.it_tlv_tot) {
            rc = BOOT_EBADIMAGE;
            goto done;
        }

        if (flash_area_read(fap, off + info.it_tlv_tot, &info, sizeof(info))) {
            rc = BOOT_EFLASH;
            goto done;
        }
    } else if (protect_tlv_size != 0) {
        rc = BOOT_EBADIMAGE;
        goto done;
    }

    if (info.it_magic != IMAGE_TLV_INFO_MAGIC) {
        rc = BOOT_EBADIMAGE;
        goto done;
    }

    *size = off + protect_tlv_size + info.it_tlv_tot;
    rc = 0;

done:
    flash_area_close(fap);
    return rc;
}
#endif /* !MCUBOOT_OVERWRITE_ONLY */

static int
boot_read_image_header(struct boot_loader_state *state, int slot,
                       struct image_header *out_hdr)
{
    const struct flash_area *fap;
    int area_id;
    int rc;

#if (BOOT_IMAGE_NUMBER == 1)
    (void)state;
#endif

    area_id = flash_area_id_from_multi_image_slot(BOOT_CURR_IMG(state), slot);
    rc = flash_area_open(area_id, &fap);
    if (rc != 0) {
        rc = BOOT_EFLASH;
        goto done;
    }

    rc = flash_area_read(fap, 0, out_hdr, sizeof *out_hdr);
    if (rc != 0) {
        rc = BOOT_EFLASH;
        goto done;
    }

    rc = 0;

done:
    flash_area_close(fap);
    return rc;
}

static int
boot_read_image_headers(struct boot_loader_state *state, bool require_all)
{
    int rc;
    int i;

    for (i = 0; i < BOOT_NUM_SLOTS; i++) {
        rc = boot_read_image_header(state, i, boot_img_hdr(state, i));
        if (rc != 0) {
            /* If `require_all` is set, fail on any single fail, otherwise
             * if at least the first slot's header was read successfully,
             * then the boot loader can attempt a boot.
             *
             * Failure to read any headers is a fatal error.
             */
            if (i > 0 && !require_all) {
                return 0;
            } else {
                return rc;
            }
        }
    }

    return 0;
}

static uint32_t
boot_write_sz(struct boot_loader_state *state)
{
    uint32_t elem_sz;
    uint32_t align;

    /* Figure out what size to write update status update as.  The size depends
     * on what the minimum write size is for scratch area, active image slot.
     * We need to use the bigger of those 2 values.
     */
    elem_sz = flash_area_align(BOOT_IMG_AREA(state, BOOT_PRIMARY_SLOT));
    align = flash_area_align(BOOT_SCRATCH_AREA(state));
    if (align > elem_sz) {
        elem_sz = align;
    }

    return elem_sz;
}

/*
 * Slots are compatible when all sectors that store up to to size of the image
 * round up to sector size, in both slot's are able to fit in the scratch
 * area, and have sizes that are a multiple of each other (powers of two
 * presumably!).
 */
static int
boot_slots_compatible(struct boot_loader_state *state)
{
    size_t num_sectors_primary;
    size_t num_sectors_secondary;
    size_t sz0, sz1;
    size_t primary_slot_sz, secondary_slot_sz;
    size_t scratch_sz;
    size_t i, j;
    int8_t smaller;

    num_sectors_primary = boot_img_num_sectors(state, BOOT_PRIMARY_SLOT);
    num_sectors_secondary = boot_img_num_sectors(state, BOOT_SECONDARY_SLOT);
    if ((num_sectors_primary > BOOT_MAX_IMG_SECTORS) ||
        (num_sectors_secondary > BOOT_MAX_IMG_SECTORS)) {
        BOOT_LOG_WRN("Cannot upgrade: more sectors than allowed");
        return 0;
    }

    scratch_sz = boot_scratch_area_size(state);

    /*
     * The following loop scans all sectors in a linear fashion, assuring that
     * for each possible sector in each slot, it is able to fit in the other
     * slot's sector or sectors. Slot's should be compatible as long as any
     * number of a slot's sectors are able to fit into another, which only
     * excludes cases where sector sizes are not a multiple of each other.
     */
    i = sz0 = primary_slot_sz = 0;
    j = sz1 = secondary_slot_sz = 0;
    smaller = 0;
    while (i < num_sectors_primary || j < num_sectors_secondary) {
        if (sz0 == sz1) {
            sz0 += boot_img_sector_size(state, BOOT_PRIMARY_SLOT, i);
            sz1 += boot_img_sector_size(state, BOOT_SECONDARY_SLOT, j);
            i++;
            j++;
        } else if (sz0 < sz1) {
            sz0 += boot_img_sector_size(state, BOOT_PRIMARY_SLOT, i);
            /* Guarantee that multiple sectors of the secondary slot
             * fit into the primary slot.
             */
            if (smaller == 2) {
                BOOT_LOG_WRN("Cannot upgrade: slots have non-compatible sectors");
                return 0;
            }
            smaller = 1;
            i++;
        } else {
            sz1 += boot_img_sector_size(state, BOOT_SECONDARY_SLOT, j);
            /* Guarantee that multiple sectors of the primary slot
             * fit into the secondary slot.
             */
            if (smaller == 1) {
                BOOT_LOG_WRN("Cannot upgrade: slots have non-compatible sectors");
                return 0;
            }
            smaller = 2;
            j++;
        }
        if (sz0 == sz1) {
            primary_slot_sz += sz0;
            secondary_slot_sz += sz1;
            /* Scratch has to fit each swap operation to the size of the larger
             * sector among the primary slot and the secondary slot.
             */
            if (sz0 > scratch_sz || sz1 > scratch_sz) {
                BOOT_LOG_WRN("Cannot upgrade: not all sectors fit inside scratch");
                return 0;
            }
            smaller = sz0 = sz1 = 0;
        }
    }

    if ((i != num_sectors_primary) ||
        (j != num_sectors_secondary) ||
        (primary_slot_sz != secondary_slot_sz)) {
        BOOT_LOG_WRN("Cannot upgrade: slots are not compatible");
        return 0;
    }

    return 1;
}

#ifndef MCUBOOT_USE_FLASH_AREA_GET_SECTORS
static int
boot_initialize_area(struct boot_loader_state *state, int flash_area)
{
    int num_sectors = BOOT_MAX_IMG_SECTORS;
    int rc;

    if (flash_area == FLASH_AREA_IMAGE_PRIMARY(BOOT_CURR_IMG(state))) {
        rc = flash_area_to_sectors(flash_area, &num_sectors,
                                   BOOT_IMG(state, BOOT_PRIMARY_SLOT).sectors);
        BOOT_IMG(state, BOOT_PRIMARY_SLOT).num_sectors = (size_t)num_sectors;

    } else if (flash_area == FLASH_AREA_IMAGE_SECONDARY(BOOT_CURR_IMG(state))) {
        rc = flash_area_to_sectors(flash_area, &num_sectors,
                                 BOOT_IMG(state, BOOT_SECONDARY_SLOT).sectors);
        BOOT_IMG(state, BOOT_SECONDARY_SLOT).num_sectors = (size_t)num_sectors;

    } else if (flash_area == FLASH_AREA_IMAGE_SCRATCH) {
        rc = flash_area_to_sectors(flash_area, &num_sectors,
                                   state->scratch.sectors);
        state->scratch.num_sectors = (size_t)num_sectors;
    } else {
        return BOOT_EFLASH;
    }

    return rc;
}
#else  /* defined(MCUBOOT_USE_FLASH_AREA_GET_SECTORS) */
static int
boot_initialize_area(struct boot_loader_state *state, int flash_area)
{
    uint32_t num_sectors;
    struct flash_sector *out_sectors;
    size_t *out_num_sectors;
    int rc;

    num_sectors = BOOT_MAX_IMG_SECTORS;

    if (flash_area == FLASH_AREA_IMAGE_PRIMARY(BOOT_CURR_IMG(state))) {
        out_sectors = BOOT_IMG(state, BOOT_PRIMARY_SLOT).sectors;
        out_num_sectors = &BOOT_IMG(state, BOOT_PRIMARY_SLOT).num_sectors;
    } else if (flash_area == FLASH_AREA_IMAGE_SECONDARY(BOOT_CURR_IMG(state))) {
        out_sectors = BOOT_IMG(state, BOOT_SECONDARY_SLOT).sectors;
        out_num_sectors = &BOOT_IMG(state, BOOT_SECONDARY_SLOT).num_sectors;
    } else if (flash_area == FLASH_AREA_IMAGE_SCRATCH) {
        out_sectors = state->scratch.sectors;
        out_num_sectors = &state->scratch.num_sectors;
    } else {
        return BOOT_EFLASH;
    }

    rc = flash_area_get_sectors(flash_area, &num_sectors, out_sectors);
    if (rc != 0) {
        return rc;
    }
    *out_num_sectors = num_sectors;
    return 0;
}
#endif  /* !defined(MCUBOOT_USE_FLASH_AREA_GET_SECTORS) */

/**
 * Determines the sector layout of both image slots and the scratch area.
 * This information is necessary for calculating the number of bytes to erase
 * and copy during an image swap.  The information collected during this
 * function is used to populate the state.
 */
static int
boot_read_sectors(struct boot_loader_state *state)
{
    uint8_t image_index;
    int rc;

    image_index = BOOT_CURR_IMG(state);

    rc = boot_initialize_area(state, FLASH_AREA_IMAGE_PRIMARY(image_index));
    if (rc != 0) {
        return BOOT_EFLASH;
    }

    rc = boot_initialize_area(state, FLASH_AREA_IMAGE_SECONDARY(image_index));
    if (rc != 0) {
        return BOOT_EFLASH;
    }

    rc = boot_initialize_area(state, FLASH_AREA_IMAGE_SCRATCH);
    if (rc != 0) {
        return BOOT_EFLASH;
    }

    BOOT_WRITE_SZ(state) = boot_write_sz(state);

    return 0;
}

static uint32_t
boot_status_internal_off(int idx, int state, int elem_sz)
{
    int idx_sz;

    idx_sz = elem_sz * BOOT_STATUS_STATE_COUNT;

    return (idx - BOOT_STATUS_IDX_0) * idx_sz +
           (state - BOOT_STATUS_STATE_0) * elem_sz;
}

/**
 * Reads the status of a partially-completed swap, if any.  This is necessary
 * to recover in case the boot lodaer was reset in the middle of a swap
 * operation.
 */
static int
boot_read_status_bytes(const struct flash_area *fap,
        struct boot_loader_state *state, struct boot_status *bs)
{
    uint32_t off;
    uint8_t status;
    int max_entries;
    int found;
    int found_idx;
    int invalid;
    int rc;
    int i;

    off = boot_status_off(fap);
    max_entries = boot_status_entries(BOOT_CURR_IMG(state), fap);
    if (max_entries < 0) {
        return BOOT_EBADARGS;
    }

    found = 0;
    found_idx = 0;
    invalid = 0;
    for (i = 0; i < max_entries; i++) {
        rc = flash_area_read_is_empty(fap, off + i * BOOT_WRITE_SZ(state),
                &status, 1);
        if (rc < 0) {
            return BOOT_EFLASH;
        }

        if (rc == 1) {
            if (found && !found_idx) {
                found_idx = i;
            }
        } else if (!found) {
            found = 1;
        } else if (found_idx) {
            invalid = 1;
            break;
        }
    }

    if (invalid) {
        /* This means there was an error writing status on the last
         * swap. Tell user and move on to validation!
         */
        BOOT_LOG_ERR("Detected inconsistent status!");

#if !defined(MCUBOOT_VALIDATE_PRIMARY_SLOT)
        /* With validation of the primary slot disabled, there is no way
         * to be sure the swapped primary slot is OK, so abort!
         */
        assert(0);
#endif
    }

    if (found) {
        if (!found_idx) {
            found_idx = i;
        }
        bs->idx = (found_idx / BOOT_STATUS_STATE_COUNT) + 1;
        bs->state = (found_idx % BOOT_STATUS_STATE_COUNT) + 1;
    }

    return 0;
}

/**
 * Reads the boot status from the flash.  The boot status contains
 * the current state of an interrupted image copy operation.  If the boot
 * status is not present, or it indicates that previous copy finished,
 * there is no operation in progress.
 */
static int
boot_read_status(struct boot_loader_state *state, struct boot_status *bs)
{
    const struct flash_area *fap;
    uint32_t off;
    uint8_t swap_info;
    int status_loc;
    int area_id;
    int rc;

    memset(bs, 0, sizeof *bs);
    bs->idx = BOOT_STATUS_IDX_0;
    bs->state = BOOT_STATUS_STATE_0;
    bs->swap_type = BOOT_SWAP_TYPE_NONE;

#ifdef MCUBOOT_OVERWRITE_ONLY
    /* Overwrite-only doesn't make use of the swap status area. */
    return 0;
#endif

    status_loc = boot_status_source(state);
    switch (status_loc) {
    case BOOT_STATUS_SOURCE_NONE:
        return 0;

    case BOOT_STATUS_SOURCE_SCRATCH:
        area_id = FLASH_AREA_IMAGE_SCRATCH;
        break;

    case BOOT_STATUS_SOURCE_PRIMARY_SLOT:
        area_id = FLASH_AREA_IMAGE_PRIMARY(BOOT_CURR_IMG(state));
        break;

    default:
        assert(0);
        return BOOT_EBADARGS;
    }

    rc = flash_area_open(area_id, &fap);
    if (rc != 0) {
        return BOOT_EFLASH;
    }

    rc = boot_read_status_bytes(fap, state, bs);
    if (rc == 0) {
        off = boot_swap_info_off(fap);
        rc = flash_area_read_is_empty(fap, off, &swap_info, sizeof swap_info);
        if (rc == 1) {
            BOOT_SET_SWAP_INFO(swap_info, 0, BOOT_SWAP_TYPE_NONE);
            rc = 0;
        }

        /* Extract the swap type info */
        bs->swap_type = BOOT_GET_SWAP_TYPE(swap_info);
    }

    flash_area_close(fap);

    return rc;
}

/**
 * Writes the supplied boot status to the flash file system.  The boot status
 * contains the current state of an in-progress image copy operation.
 *
 * @param bs                    The boot status to write.
 *
 * @return                      0 on success; nonzero on failure.
 */
int
boot_write_status(struct boot_loader_state *state, struct boot_status *bs)
{
    const struct flash_area *fap;
    uint32_t off;
    int area_id;
    int rc;
    uint8_t buf[BOOT_MAX_ALIGN];
    uint8_t align;
    uint8_t erased_val;

    /* NOTE: The first sector copied (that is the last sector on slot) contains
     *       the trailer. Since in the last step the primary slot is erased, the
     *       first two status writes go to the scratch which will be copied to
     *       the primary slot!
     */

    if (bs->use_scratch) {
        /* Write to scratch. */
        area_id = FLASH_AREA_IMAGE_SCRATCH;
    } else {
        /* Write to the primary slot. */
        area_id = FLASH_AREA_IMAGE_PRIMARY(BOOT_CURR_IMG(state));
    }

    rc = flash_area_open(area_id, &fap);
    if (rc != 0) {
        rc = BOOT_EFLASH;
        goto done;
    }

    off = boot_status_off(fap) +
          boot_status_internal_off(bs->idx, bs->state, BOOT_WRITE_SZ(state));
    align = flash_area_align(fap);
    erased_val = flash_area_erased_val(fap);
    memset(buf, erased_val, BOOT_MAX_ALIGN);
    buf[0] = bs->state;

    rc = flash_area_write(fap, off, buf, align);
    if (rc != 0) {
        rc = BOOT_EFLASH;
        goto done;
    }

    rc = 0;

done:
    flash_area_close(fap);
    return rc;
}

/*
 * Validate image hash/signature in a slot.
 */
static int
boot_image_check(struct boot_loader_state *state, struct image_header *hdr,
                 const struct flash_area *fap, struct boot_status *bs)
{
    TARGET_STATIC uint8_t tmpbuf[BOOT_TMPBUF_SZ];
    uint8_t image_index;
    int rc;

#if (BOOT_IMAGE_NUMBER == 1)
    (void)state;
#endif

    (void)bs;
    (void)rc;

    image_index = BOOT_CURR_IMG(state);

#ifdef MCUBOOT_ENC_IMAGES
    if (MUST_DECRYPT(fap, image_index, hdr)) {
        rc = boot_enc_load(BOOT_CURR_ENC(state), image_index, hdr, fap, bs->enckey[1]);
        if (rc < 0) {
            return BOOT_EBADIMAGE;
        }
        if (rc == 0 && boot_enc_set_key(BOOT_CURR_ENC(state), 1, bs->enckey[1])) {
            return BOOT_EBADIMAGE;
        }
    }
#endif

    if (bootutil_img_validate(BOOT_CURR_ENC(state), image_index, hdr, fap, tmpbuf,
                              BOOT_TMPBUF_SZ, NULL, 0, NULL)) {
        return BOOT_EBADIMAGE;
    }

    return 0;
}

static int
split_image_check(struct image_header *app_hdr,
                  const struct flash_area *app_fap,
                  struct image_header *loader_hdr,
                  const struct flash_area *loader_fap)
{
    static void *tmpbuf;
    uint8_t loader_hash[32];

    if (!tmpbuf) {
        tmpbuf = malloc(BOOT_TMPBUF_SZ);
        if (!tmpbuf) {
            return BOOT_ENOMEM;
        }
    }

    if (bootutil_img_validate(NULL, 0, loader_hdr, loader_fap, tmpbuf,
                              BOOT_TMPBUF_SZ, NULL, 0, loader_hash)) {
        return BOOT_EBADIMAGE;
    }

    if (bootutil_img_validate(NULL, 0, app_hdr, app_fap, tmpbuf,
                              BOOT_TMPBUF_SZ, loader_hash, 32, NULL)) {
        return BOOT_EBADIMAGE;
    }

    return 0;
}

/*
 * Check that this is a valid header.  Valid means that the magic is
 * correct, and that the sizes/offsets are "sane".  Sane means that
 * there is no overflow on the arithmetic, and that the result fits
 * within the flash area we are in.
 */
static bool
boot_is_header_valid(const struct image_header *hdr, const struct flash_area *fap)
{
    uint32_t size;

    if (hdr->ih_magic != IMAGE_MAGIC) {
        return false;
    }

    if (!boot_u32_safe_add(&size, hdr->ih_img_size, hdr->ih_hdr_size)) {
        return false;
    }

    if (size >= fap->fa_size) {
        return false;
    }

    return true;
}

/*
 * Check that a memory area consists of a given value.
 */
static inline bool
boot_data_is_set_to(uint8_t val, void *data, size_t len)
{
    uint8_t i;
    uint8_t *p = (uint8_t *)data;
    for (i = 0; i < len; i++) {
        if (val != p[i]) {
            return false;
        }
    }
    return true;
}

static int
boot_check_header_erased(struct boot_loader_state *state, int slot)
{
    const struct flash_area *fap;
    struct image_header *hdr;
    uint8_t erased_val;
    int area_id;
    int rc;

    area_id = flash_area_id_from_multi_image_slot(BOOT_CURR_IMG(state), slot);
    rc = flash_area_open(area_id, &fap);
    if (rc != 0) {
        return -1;
    }

    erased_val = flash_area_erased_val(fap);
    flash_area_close(fap);

    hdr = boot_img_hdr(state, slot);
    if (!boot_data_is_set_to(erased_val, &hdr->ih_magic, sizeof(hdr->ih_magic))) {
        return -1;
    }

    return 0;
}

/*
 * Check that there is a valid image in a slot
 *
 * @returns
 *         0 if image was successfully validated
 *         1 if no bootloable image was found
 *         -1 on any errors
 */
static int
boot_validate_slot(struct boot_loader_state *state, int slot,
                   struct boot_status *bs)
{
    const struct flash_area *fap;
    struct image_header *hdr;
    int area_id;
    int rc;

    area_id = flash_area_id_from_multi_image_slot(BOOT_CURR_IMG(state), slot);
    rc = flash_area_open(area_id, &fap);
    if (rc != 0) {
        return -1;
    }

    hdr = boot_img_hdr(state, slot);
    if (boot_check_header_erased(state, slot) == 0 ||
        (hdr->ih_flags & IMAGE_F_NON_BOOTABLE)) {
        /* No bootable image in slot; continue booting from the primary slot. */
        rc = 1;
        goto out;
    }

    if (!boot_is_header_valid(hdr, fap) || boot_image_check(state, hdr, fap, bs)) {
        if (slot != BOOT_PRIMARY_SLOT) {
            flash_area_erase(fap, 0, fap->fa_size);
            /* Image in the secondary slot is invalid. Erase the image and
             * continue booting from the primary slot.
             */
        }
        BOOT_LOG_ERR("Image in the %s slot is not valid!",
                     (slot == BOOT_PRIMARY_SLOT) ? "primary" : "secondary");
        rc = -1;
        goto out;
    }

    /* Image in the secondary slot is valid. */
    rc = 0;

out:
    flash_area_close(fap);
    return rc;
}

/**
 * Determines which swap operation to perform, if any.  If it is determined
 * that a swap operation is required, the image in the secondary slot is checked
 * for validity.  If the image in the secondary slot is invalid, it is erased,
 * and a swap type of "none" is indicated.
 *
 * @return                      The type of swap to perform (BOOT_SWAP_TYPE...)
 */
static int
boot_validated_swap_type(struct boot_loader_state *state,
                         struct boot_status *bs)
{
    int swap_type;
    int rc;

    swap_type = boot_swap_type_multi(BOOT_CURR_IMG(state));
    if (BOOT_IS_UPGRADE(swap_type)) {
        /* Boot loader wants to switch to the secondary slot.
         * Ensure image is valid.
         */
        rc = boot_validate_slot(state, BOOT_SECONDARY_SLOT, bs);
        if (rc == 1) {
            swap_type = BOOT_SWAP_TYPE_NONE;
        } else if (rc != 0) {
            swap_type = BOOT_SWAP_TYPE_FAIL;
        }
    }

    return swap_type;
}

/**
 * Calculates the number of sectors the scratch area can contain.  A "last"
 * source sector is specified because images are copied backwards in flash
 * (final index to index number 0).
 *
 * @param last_sector_idx       The index of the last source sector
 *                                  (inclusive).
 * @param out_first_sector_idx  The index of the first source sector
 *                                  (inclusive) gets written here.
 *
 * @return                      The number of bytes comprised by the
 *                                  [first-sector, last-sector] range.
 */
#ifndef MCUBOOT_OVERWRITE_ONLY
static uint32_t
boot_copy_sz(struct boot_loader_state *state, int last_sector_idx,
             int *out_first_sector_idx)
{
    size_t scratch_sz;
    uint32_t new_sz;
    uint32_t sz;
    int i;

    sz = 0;

    scratch_sz = boot_scratch_area_size(state);
    for (i = last_sector_idx; i >= 0; i--) {
        new_sz = sz + boot_img_sector_size(state, BOOT_PRIMARY_SLOT, i);
        /*
         * The secondary slot is not being checked here, because
         * `boot_slots_compatible` already provides assurance that the copy size
         * will be compatible with the primary slot and scratch.
         */
        if (new_sz > scratch_sz) {
            break;
        }
        sz = new_sz;
    }

    /* i currently refers to a sector that doesn't fit or it is -1 because all
     * sectors have been processed.  In both cases, exclude sector i.
     */
    *out_first_sector_idx = i + 1;
    return sz;
}
#endif /* !MCUBOOT_OVERWRITE_ONLY */

/**
 * Erases a region of flash.
 *
 * @param flash_area           The flash_area containing the region to erase.
 * @param off                   The offset within the flash area to start the
 *                                  erase.
 * @param sz                    The number of bytes to erase.
 *
 * @return                      0 on success; nonzero on failure.
 */
static inline int
boot_erase_region(const struct flash_area *fap, uint32_t off, uint32_t sz)
{
    return flash_area_erase(fap, off, sz);
}

/**
 * Copies the contents of one flash region to another.  You must erase the
 * destination region prior to calling this function.
 *
 * @param flash_area_id_src     The ID of the source flash area.
 * @param flash_area_id_dst     The ID of the destination flash area.
 * @param off_src               The offset within the source flash area to
 *                                  copy from.
 * @param off_dst               The offset within the destination flash area to
 *                                  copy to.
 * @param sz                    The number of bytes to copy.
 *
 * @return                      0 on success; nonzero on failure.
 */
static int
boot_copy_region(struct boot_loader_state *state,
                 const struct flash_area *fap_src,
                 const struct flash_area *fap_dst,
                 uint32_t off_src, uint32_t off_dst, uint32_t sz)
{
    uint32_t bytes_copied;
    int chunk_sz;
    int rc;
#ifdef MCUBOOT_ENC_IMAGES
    uint32_t off;
    uint32_t tlv_off;
    size_t blk_off;
    struct image_header *hdr;
    uint16_t idx;
    uint32_t blk_sz;
    uint8_t image_index;
#endif

    TARGET_STATIC uint8_t buf[1024];

#if !defined(MCUBOOT_ENC_IMAGES)
    (void)state;
#endif

    bytes_copied = 0;
    while (bytes_copied < sz) {
        if (sz - bytes_copied > sizeof buf) {
            chunk_sz = sizeof buf;
        } else {
            chunk_sz = sz - bytes_copied;
        }

        rc = flash_area_read(fap_src, off_src + bytes_copied, buf, chunk_sz);
        if (rc != 0) {
            return BOOT_EFLASH;
        }

#ifdef MCUBOOT_ENC_IMAGES
        image_index = BOOT_CURR_IMG(state);
        if (fap_src->fa_id == FLASH_AREA_IMAGE_SECONDARY(image_index) ||
            fap_dst->fa_id == FLASH_AREA_IMAGE_SECONDARY(image_index)) {
            /* assume the secondary slot as src, needs decryption */
            hdr = boot_img_hdr(state, BOOT_SECONDARY_SLOT);
            off = off_src;
            if (fap_dst->fa_id == FLASH_AREA_IMAGE_SECONDARY(image_index)) {
                /* might need encryption (metadata from the primary slot) */
                hdr = boot_img_hdr(state, BOOT_PRIMARY_SLOT);
                off = off_dst;
            }
            if (IS_ENCRYPTED(hdr)) {
                blk_sz = chunk_sz;
                idx = 0;
                if (off + bytes_copied < hdr->ih_hdr_size) {
                    /* do not decrypt header */
                    blk_off = 0;
                    blk_sz = chunk_sz - hdr->ih_hdr_size;
                    idx = hdr->ih_hdr_size;
                } else {
                    blk_off = ((off + bytes_copied) - hdr->ih_hdr_size) & 0xf;
                }
                tlv_off = BOOT_TLV_OFF(hdr);
                if (off + bytes_copied + chunk_sz > tlv_off) {
                    /* do not decrypt TLVs */
                    if (off + bytes_copied >= tlv_off) {
                        blk_sz = 0;
                    } else {
                        blk_sz = tlv_off - (off + bytes_copied);
                    }
                }
                boot_encrypt(BOOT_CURR_ENC(state), image_index, fap_src,
                        (off + bytes_copied + idx) - hdr->ih_hdr_size, blk_sz,
                        blk_off, &buf[idx]);
            }
        }
#endif

        rc = flash_area_write(fap_dst, off_dst + bytes_copied, buf, chunk_sz);
        if (rc != 0) {
            return BOOT_EFLASH;
        }

        bytes_copied += chunk_sz;

        MCUBOOT_WATCHDOG_FEED();
    }

    return 0;
}

#ifndef MCUBOOT_OVERWRITE_ONLY
static inline int
boot_status_init(const struct boot_loader_state *state,
                 const struct flash_area *fap,
                 const struct boot_status *bs)
{
    struct boot_swap_state swap_state;
    uint8_t image_index;
    int rc;

#if (BOOT_IMAGE_NUMBER == 1)
    (void)state;
#endif

    image_index = BOOT_CURR_IMG(state);

    BOOT_LOG_DBG("initializing status; fa_id=%d", fap->fa_id);

    rc = boot_read_swap_state_by_id(FLASH_AREA_IMAGE_SECONDARY(image_index),
            &swap_state);
    assert(rc == 0);

    if (bs->swap_type != BOOT_SWAP_TYPE_NONE) {
        rc = boot_write_swap_info(fap, bs->swap_type, image_index);
        assert(rc == 0);
    }

    if (swap_state.image_ok == BOOT_FLAG_SET) {
        rc = boot_write_image_ok(fap);
        assert(rc == 0);
    }

    rc = boot_write_swap_size(fap, bs->swap_size);
    assert(rc == 0);

#ifdef MCUBOOT_ENC_IMAGES
    rc = boot_write_enc_key(fap, 0, bs->enckey[0]);
    assert(rc == 0);

    rc = boot_write_enc_key(fap, 1, bs->enckey[1]);
    assert(rc == 0);
#endif

    rc = boot_write_magic(fap);
    assert(rc == 0);

    return 0;
}

#endif

#ifndef MCUBOOT_OVERWRITE_ONLY
static int
boot_erase_trailer_sectors(const struct boot_loader_state *state,
                           const struct flash_area *fap)
{
    uint8_t slot;
    uint32_t sector;
    uint32_t trailer_sz;
    uint32_t total_sz;
    uint32_t off;
    uint32_t sz;
    int fa_id_primary;
    int fa_id_secondary;
    uint8_t image_index;
    int rc;

    BOOT_LOG_DBG("erasing trailer; fa_id=%d", fap->fa_id);

    image_index = BOOT_CURR_IMG(state);
    fa_id_primary = flash_area_id_from_multi_image_slot(image_index,
            BOOT_PRIMARY_SLOT);
    fa_id_secondary = flash_area_id_from_multi_image_slot(image_index,
            BOOT_SECONDARY_SLOT);

    if (fap->fa_id == fa_id_primary) {
        slot = BOOT_PRIMARY_SLOT;
    } else if (fap->fa_id == fa_id_secondary) {
        slot = BOOT_SECONDARY_SLOT;
    } else {
        return BOOT_EFLASH;
    }

    /* delete starting from last sector and moving to beginning */
    sector = boot_img_num_sectors(state, slot) - 1;
    trailer_sz = boot_trailer_sz(BOOT_WRITE_SZ(state));
    total_sz = 0;
    do {
        sz = boot_img_sector_size(state, slot, sector);
        off = boot_img_sector_off(state, slot, sector);
        rc = boot_erase_region(fap, off, sz);
        assert(rc == 0);

        sector--;
        total_sz += sz;
    } while (total_sz < trailer_sz);

    return rc;
}
#endif /* !MCUBOOT_OVERWRITE_ONLY */

/**
 * Swaps the contents of two flash regions within the two image slots.
 *
 * @param idx                   The index of the first sector in the range of
 *                                  sectors being swapped.
 * @param sz                    The number of bytes to swap.
 * @param bs                    The current boot status.  This struct gets
 *                                  updated according to the outcome.
 *
 * @return                      0 on success; nonzero on failure.
 */
#ifndef MCUBOOT_OVERWRITE_ONLY
static void
boot_swap_sectors(int idx, uint32_t sz, struct boot_loader_state *state,
        struct boot_status *bs)
{
    const struct flash_area *fap_primary_slot;
    const struct flash_area *fap_secondary_slot;
    const struct flash_area *fap_scratch;
    uint32_t copy_sz;
    uint32_t trailer_sz;
    uint32_t img_off;
    uint32_t scratch_trailer_off;
    struct boot_swap_state swap_state;
    size_t last_sector;
    bool erase_scratch;
    uint8_t image_index;
    int rc;

    /* Calculate offset from start of image area. */
    img_off = boot_img_sector_off(state, BOOT_PRIMARY_SLOT, idx);

    copy_sz = sz;
    trailer_sz = boot_trailer_sz(BOOT_WRITE_SZ(state));

    /* sz in this function is always sized on a multiple of the sector size.
     * The check against the start offset of the last sector
     * is to determine if we're swapping the last sector. The last sector
     * needs special handling because it's where the trailer lives. If we're
     * copying it, we need to use scratch to write the trailer temporarily.
     *
     * NOTE: `use_scratch` is a temporary flag (never written to flash) which
     * controls if special handling is needed (swapping last sector).
     */
    last_sector = boot_img_num_sectors(state, BOOT_PRIMARY_SLOT) - 1;
    if ((img_off + sz) >
        boot_img_sector_off(state, BOOT_PRIMARY_SLOT, last_sector)) {
        copy_sz -= trailer_sz;
    }

    bs->use_scratch = (bs->idx == BOOT_STATUS_IDX_0 && copy_sz != sz);

    image_index = BOOT_CURR_IMG(state);

    rc = flash_area_open(FLASH_AREA_IMAGE_PRIMARY(image_index),
            &fap_primary_slot);
    assert (rc == 0);

    rc = flash_area_open(FLASH_AREA_IMAGE_SECONDARY(image_index),
            &fap_secondary_slot);
    assert (rc == 0);

    rc = flash_area_open(FLASH_AREA_IMAGE_SCRATCH, &fap_scratch);
    assert (rc == 0);

    if (bs->state == BOOT_STATUS_STATE_0) {
        BOOT_LOG_DBG("erasing scratch area");
        rc = boot_erase_region(fap_scratch, 0, fap_scratch->fa_size);
        assert(rc == 0);

        if (bs->idx == BOOT_STATUS_IDX_0) {
            /* Write a trailer to the scratch area, even if we don't need the
             * scratch area for status.  We need a temporary place to store the
             * `swap-type` while we erase the primary trailer.
             */ 
            rc = boot_status_init(state, fap_scratch, bs);
            assert(rc == 0);

            if (!bs->use_scratch) {
                /* Prepare the primary status area... here it is known that the
                 * last sector is not being used by the image data so it's safe
                 * to erase.
                 */
                rc = boot_erase_trailer_sectors(state, fap_primary_slot);
                assert(rc == 0);

                rc = boot_status_init(state, fap_primary_slot, bs);
                assert(rc == 0);

                /* Erase the temporary trailer from the scratch area. */
                rc = boot_erase_region(fap_scratch, 0, fap_scratch->fa_size);
                assert(rc == 0);
            }
        }

        rc = boot_copy_region(state, fap_secondary_slot, fap_scratch,
                              img_off, 0, copy_sz);
        assert(rc == 0);

        rc = boot_write_status(state, bs);
        bs->state = BOOT_STATUS_STATE_1;
        BOOT_STATUS_ASSERT(rc == 0);
    }

    if (bs->state == BOOT_STATUS_STATE_1) {
        rc = boot_erase_region(fap_secondary_slot, img_off, sz);
        assert(rc == 0);

        rc = boot_copy_region(state, fap_primary_slot, fap_secondary_slot,
                              img_off, img_off, copy_sz);
        assert(rc == 0);

        if (bs->idx == BOOT_STATUS_IDX_0 && !bs->use_scratch) {
            /* If not all sectors of the slot are being swapped,
             * guarantee here that only the primary slot will have the state.
             */
            rc = boot_erase_trailer_sectors(state, fap_secondary_slot);
            assert(rc == 0);
        }

        rc = boot_write_status(state, bs);
        bs->state = BOOT_STATUS_STATE_2;
        BOOT_STATUS_ASSERT(rc == 0);
    }

    if (bs->state == BOOT_STATUS_STATE_2) {
        rc = boot_erase_region(fap_primary_slot, img_off, sz);
        assert(rc == 0);

        /* NOTE: If this is the final sector, we exclude the image trailer from
         * this copy (copy_sz was truncated earlier).
         */
        rc = boot_copy_region(state, fap_scratch, fap_primary_slot,
                              0, img_off, copy_sz);
        assert(rc == 0);

        if (bs->use_scratch) {
            scratch_trailer_off = boot_status_off(fap_scratch);

            /* copy current status that is being maintained in scratch */
            rc = boot_copy_region(state, fap_scratch, fap_primary_slot,
                        scratch_trailer_off, img_off + copy_sz,
                        (BOOT_STATUS_STATE_COUNT - 1) * BOOT_WRITE_SZ(state));
            BOOT_STATUS_ASSERT(rc == 0);

            rc = boot_read_swap_state_by_id(FLASH_AREA_IMAGE_SCRATCH,
                                            &swap_state);
            assert(rc == 0);

            if (swap_state.image_ok == BOOT_FLAG_SET) {
                rc = boot_write_image_ok(fap_primary_slot);
                assert(rc == 0);
            }

            if (swap_state.swap_type != BOOT_SWAP_TYPE_NONE) {
                rc = boot_write_swap_info(fap_primary_slot,
                        swap_state.swap_type, image_index);
                assert(rc == 0);
            }

            rc = boot_write_swap_size(fap_primary_slot, bs->swap_size);
            assert(rc == 0);

#ifdef MCUBOOT_ENC_IMAGES
            rc = boot_write_enc_key(fap_primary_slot, 0, bs->enckey[0]);
            assert(rc == 0);

            rc = boot_write_enc_key(fap_primary_slot, 1, bs->enckey[1]);
            assert(rc == 0);
#endif
            rc = boot_write_magic(fap_primary_slot);
            assert(rc == 0);
        }

        /* If we wrote a trailer to the scratch area, erase it after we persist
         * a trailer to the primary slot.  We do this to prevent mcuboot from
         * reading a stale status from the scratch area in case of immediate
         * reset.
         */
        erase_scratch = bs->use_scratch;
        bs->use_scratch = 0;

        rc = boot_write_status(state, bs);
        bs->idx++;
        bs->state = BOOT_STATUS_STATE_0;
        BOOT_STATUS_ASSERT(rc == 0);

        if (erase_scratch) {
            rc = boot_erase_region(fap_scratch, 0, sz);
            assert(rc == 0);
        }
    }

    flash_area_close(fap_primary_slot);
    flash_area_close(fap_secondary_slot);
    flash_area_close(fap_scratch);
}
#endif /* !MCUBOOT_OVERWRITE_ONLY */

/**
 * Overwrite primary slot with the image contained in the secondary slot.
 * If a prior copy operation was interrupted by a system reset, this function
 * redos the copy.
 *
 * @param bs                    The current boot status.  This function reads
 *                                  this struct to determine if it is resuming
 *                                  an interrupted swap operation.  This
 *                                  function writes the updated status to this
 *                                  function on return.
 *
 * @return                      0 on success; nonzero on failure.
 */
#if defined(MCUBOOT_OVERWRITE_ONLY) || defined(MCUBOOT_BOOTSTRAP)
static int
boot_copy_image(struct boot_loader_state *state, struct boot_status *bs)
{
    size_t sect_count;
    size_t sect;
    int rc;
    size_t size;
    size_t this_size;
    size_t last_sector;
    const struct flash_area *fap_primary_slot;
    const struct flash_area *fap_secondary_slot;
    uint8_t image_index;

    (void)bs;

#if defined(MCUBOOT_OVERWRITE_ONLY_FAST)
    uint32_t src_size = 0;
    rc = boot_read_image_size(state, BOOT_SECONDARY_SLOT, &src_size);
    assert(rc == 0);
#endif

    BOOT_LOG_INF("Image upgrade secondary slot -> primary slot");
    BOOT_LOG_INF("Erasing the primary slot");

    image_index = BOOT_CURR_IMG(state);

    rc = flash_area_open(FLASH_AREA_IMAGE_PRIMARY(image_index),
            &fap_primary_slot);
    assert (rc == 0);

    rc = flash_area_open(FLASH_AREA_IMAGE_SECONDARY(image_index),
            &fap_secondary_slot);
    assert (rc == 0);

    sect_count = boot_img_num_sectors(state, BOOT_PRIMARY_SLOT);
    for (sect = 0, size = 0; sect < sect_count; sect++) {
        this_size = boot_img_sector_size(state, BOOT_PRIMARY_SLOT, sect);
        rc = boot_erase_region(fap_primary_slot, size, this_size);
        assert(rc == 0);

        size += this_size;

#if defined(MCUBOOT_OVERWRITE_ONLY_FAST)
        if (size >= src_size) {
            break;
        }
#endif
    }

#ifdef MCUBOOT_ENC_IMAGES
    if (IS_ENCRYPTED(boot_img_hdr(state, BOOT_SECONDARY_SLOT))) {
        rc = boot_enc_load(BOOT_CURR_ENC(state), image_index,
                boot_img_hdr(state, BOOT_SECONDARY_SLOT),
                fap_secondary_slot, bs->enckey[1]);

        if (rc < 0) {
            return BOOT_EBADIMAGE;
        }
        if (rc == 0 && boot_enc_set_key(BOOT_CURR_ENC(state), 1, bs->enckey[1])) {
            return BOOT_EBADIMAGE;
        }
    }
#endif

    BOOT_LOG_INF("Copying the secondary slot to the primary slot: 0x%zx bytes",
                 size);
    rc = boot_copy_region(state, fap_secondary_slot, fap_primary_slot, 0, 0, size);

    /*
     * Erases header and trailer. The trailer is erased because when a new
     * image is written without a trailer as is the case when using newt, the
     * trailer that was left might trigger a new upgrade.
     */
    BOOT_LOG_DBG("erasing secondary header");
    rc = boot_erase_region(fap_secondary_slot,
                           boot_img_sector_off(state, BOOT_SECONDARY_SLOT, 0),
                           boot_img_sector_size(state, BOOT_SECONDARY_SLOT, 0));
    assert(rc == 0);
    last_sector = boot_img_num_sectors(state, BOOT_SECONDARY_SLOT) - 1;
    BOOT_LOG_DBG("erasing secondary trailer");
    rc = boot_erase_region(fap_secondary_slot,
                           boot_img_sector_off(state, BOOT_SECONDARY_SLOT,
                               last_sector),
                           boot_img_sector_size(state, BOOT_SECONDARY_SLOT,
                               last_sector));
    assert(rc == 0);

    flash_area_close(fap_primary_slot);
    flash_area_close(fap_secondary_slot);

    /* TODO: Perhaps verify the primary slot's signature again? */

    return 0;
}
#endif

#if !defined(MCUBOOT_OVERWRITE_ONLY)
/**
 * Swaps the two images in flash.  If a prior copy operation was interrupted
 * by a system reset, this function completes that operation.
 *
 * @param bs                    The current boot status.  This function reads
 *                                  this struct to determine if it is resuming
 *                                  an interrupted swap operation.  This
 *                                  function writes the updated status to this
 *                                  function on return.
 *
 * @return                      0 on success; nonzero on failure.
 */
static int
boot_swap_image(struct boot_loader_state *state, struct boot_status *bs)
{
    uint32_t sz;
    int first_sector_idx;
    int last_sector_idx;
    int last_idx_secondary_slot;
    uint32_t swap_idx;
    struct image_header *hdr;
#ifdef MCUBOOT_ENC_IMAGES
    const struct flash_area *fap;
    uint8_t slot;
    uint8_t i;
#endif
    uint32_t size;
    uint32_t copy_size;
    uint32_t primary_slot_size;
    uint32_t secondary_slot_size;
    uint8_t image_index;
    int rc;

    /* FIXME: just do this if asked by user? */

    size = copy_size = 0;
    image_index = BOOT_CURR_IMG(state);

    if (bs->idx == BOOT_STATUS_IDX_0 && bs->state == BOOT_STATUS_STATE_0) {
        /*
         * No swap ever happened, so need to find the largest image which
         * will be used to determine the amount of sectors to swap.
         */
        hdr = boot_img_hdr(state, BOOT_PRIMARY_SLOT);
        if (hdr->ih_magic == IMAGE_MAGIC) {
            rc = boot_read_image_size(state, BOOT_PRIMARY_SLOT, &copy_size);
            assert(rc == 0);
        }

#ifdef MCUBOOT_ENC_IMAGES
        if (IS_ENCRYPTED(hdr)) {
            fap = BOOT_IMG_AREA(state, BOOT_PRIMARY_SLOT);
            rc = boot_enc_load(BOOT_CURR_ENC(state), image_index, hdr, fap, bs->enckey[0]);
            assert(rc >= 0);

            if (rc == 0) {
                rc = boot_enc_set_key(BOOT_CURR_ENC(state), 0, bs->enckey[0]);
                assert(rc == 0);
            } else {
                rc = 0;
            }
        } else {
            memset(bs->enckey[0], 0xff, BOOT_ENC_KEY_SIZE);
        }
#endif

        hdr = boot_img_hdr(state, BOOT_SECONDARY_SLOT);
        if (hdr->ih_magic == IMAGE_MAGIC) {
            rc = boot_read_image_size(state, BOOT_SECONDARY_SLOT, &size);
            assert(rc == 0);
        }

#ifdef MCUBOOT_ENC_IMAGES
        hdr = boot_img_hdr(state, BOOT_SECONDARY_SLOT);
        if (IS_ENCRYPTED(hdr)) {
            fap = BOOT_IMG_AREA(state, BOOT_SECONDARY_SLOT);
            rc = boot_enc_load(BOOT_CURR_ENC(state), image_index, hdr, fap, bs->enckey[1]);
            assert(rc >= 0);

            if (rc == 0) {
                rc = boot_enc_set_key(BOOT_CURR_ENC(state), 1, bs->enckey[1]);
                assert(rc == 0);
            } else {
                rc = 0;
            }
        } else {
            memset(bs->enckey[1], 0xff, BOOT_ENC_KEY_SIZE);
        }
#endif

        if (size > copy_size) {
            copy_size = size;
        }

        bs->swap_size = copy_size;
    } else {
        /*
         * If a swap was under way, the swap_size should already be present
         * in the trailer...
         */
        rc = boot_read_swap_size(image_index, &bs->swap_size);
        assert(rc == 0);

        copy_size = bs->swap_size;

#ifdef MCUBOOT_ENC_IMAGES
        for (slot = 0; slot <= 1; slot++) {
            rc = boot_read_enc_key(image_index, slot, bs->enckey[slot]);
            assert(rc == 0);

            for (i = 0; i < BOOT_ENC_KEY_SIZE; i++) {
                if (bs->enckey[slot][i] != 0xff) {
                    break;
                }
            }

            if (i != BOOT_ENC_KEY_SIZE) {
                boot_enc_set_key(BOOT_CURR_ENC(state), slot, bs->enckey[slot]);
            }
        }
#endif
    }

    primary_slot_size = 0;
    secondary_slot_size = 0;
    last_sector_idx = 0;
    last_idx_secondary_slot = 0;

    /*
     * Knowing the size of the largest image between both slots, here we
     * find what is the last sector in the primary slot that needs swapping.
     * Since we already know that both slots are compatible, the secondary
     * slot's last sector is not really required after this check is finished.
     */
    while (1) {
        if ((primary_slot_size < copy_size) ||
            (primary_slot_size < secondary_slot_size)) {
           primary_slot_size += boot_img_sector_size(state,
                                                     BOOT_PRIMARY_SLOT,
                                                     last_sector_idx);
        }
        if ((secondary_slot_size < copy_size) ||
            (secondary_slot_size < primary_slot_size)) {
           secondary_slot_size += boot_img_sector_size(state,
                                                       BOOT_SECONDARY_SLOT,
                                                       last_idx_secondary_slot);
        }
        if (primary_slot_size >= copy_size &&
                secondary_slot_size >= copy_size &&
                primary_slot_size == secondary_slot_size) {
            break;
        }
        last_sector_idx++;
        last_idx_secondary_slot++;
    }

    swap_idx = 0;
    while (last_sector_idx >= 0) {
        sz = boot_copy_sz(state, last_sector_idx, &first_sector_idx);
        if (swap_idx >= (bs->idx - BOOT_STATUS_IDX_0)) {
            boot_swap_sectors(first_sector_idx, sz, state, bs);
        }

        last_sector_idx = first_sector_idx - 1;
        swap_idx++;
    }

#ifdef MCUBOOT_VALIDATE_PRIMARY_SLOT
    if (boot_status_fails > 0) {
        BOOT_LOG_WRN("%d status write fails performing the swap",
                     boot_status_fails);
    }
#endif

    return 0;
}
#endif

/**
 * Marks the image in the primary slot as fully copied.
 */
#ifndef MCUBOOT_OVERWRITE_ONLY
static int
boot_set_copy_done(uint8_t image_index)
{
    const struct flash_area *fap;
    int rc;

    rc = flash_area_open(FLASH_AREA_IMAGE_PRIMARY(image_index),
            &fap);
    if (rc != 0) {
        return BOOT_EFLASH;
    }

    rc = boot_write_copy_done(fap);
    flash_area_close(fap);
    return rc;
}
#endif /* !MCUBOOT_OVERWRITE_ONLY */

/**
 * Marks a reverted image in the primary slot as confirmed. This is necessary to
 * ensure the status bytes from the image revert operation don't get processed
 * on a subsequent boot.
 *
 * NOTE: image_ok is tested before writing because if there's a valid permanent
 * image installed on the primary slot and the new image to be upgrade to has a
 * bad sig, image_ok would be overwritten.
 */
#ifndef MCUBOOT_OVERWRITE_ONLY
static int
boot_set_image_ok(uint8_t image_index)
{
    const struct flash_area *fap;
    struct boot_swap_state state;
    int rc;

    rc = flash_area_open(FLASH_AREA_IMAGE_PRIMARY(image_index),
            &fap);
    if (rc != 0) {
        return BOOT_EFLASH;
    }

    rc = boot_read_swap_state(fap, &state);
    if (rc != 0) {
        rc = BOOT_EFLASH;
        goto out;
    }

    if (state.image_ok == BOOT_FLAG_UNSET) {
        rc = boot_write_image_ok(fap);
    }

out:
    flash_area_close(fap);
    return rc;
}
#endif /* !MCUBOOT_OVERWRITE_ONLY */

#if (BOOT_IMAGE_NUMBER > 1)
/**
 * Check if the version of the image is not older than required.
 *
 * @param req         Required minimal image version.
 * @param ver         Version of the image to be checked.
 *
 * @return            0 if the version is sufficient, nonzero otherwise.
 */
static int
boot_is_version_sufficient(struct image_version *req,
                           struct image_version *ver)
{
    if (ver->iv_major > req->iv_major) {
        return 0;
    }
    if (ver->iv_major < req->iv_major) {
        return BOOT_EBADVERSION;
    }
    /* The major version numbers are equal. */
    if (ver->iv_minor > req->iv_minor) {
        return 0;
    }
    if (ver->iv_minor < req->iv_minor) {
        return BOOT_EBADVERSION;
    }
    /* The minor version numbers are equal. */
    if (ver->iv_revision < req->iv_revision) {
        return BOOT_EBADVERSION;
    }

    return 0;
}

/**
 * Check the image dependency whether it is satisfied and modify
 * the swap type if necessary.
 *
 * @param dep               Image dependency which has to be verified.
 *
 * @return                  0 on success; nonzero on failure.
 */
static int
boot_verify_slot_dependency(struct boot_loader_state *state,
                            struct image_dependency *dep)
{
    struct image_version *dep_version;
    size_t dep_slot;
    int rc;
    uint8_t swap_type;

    /* Determine the source of the image which is the subject of
     * the dependency and get it's version. */
    swap_type = state->swap_type[dep->image_id];
    dep_slot = (swap_type != BOOT_SWAP_TYPE_NONE) ?
                BOOT_SECONDARY_SLOT : BOOT_PRIMARY_SLOT;
    dep_version = &state->imgs[dep->image_id][dep_slot].hdr.ih_ver;

    rc = boot_is_version_sufficient(&dep->image_min_version, dep_version);
    if (rc != 0) {
        /* Dependency not satisfied.
         * Modify the swap type to decrease the version number of the image
         * (which will be located in the primary slot after the boot process),
         * consequently the number of unsatisfied dependencies will be
         * decreased or remain the same.
         */
        switch (BOOT_SWAP_TYPE(state)) {
        case BOOT_SWAP_TYPE_TEST:
        case BOOT_SWAP_TYPE_PERM:
            BOOT_SWAP_TYPE(state) = BOOT_SWAP_TYPE_NONE;
            break;
        case BOOT_SWAP_TYPE_NONE:
            BOOT_SWAP_TYPE(state) = BOOT_SWAP_TYPE_REVERT;
            break;
        default:
            break;
        }
    }

    return rc;
}

/**
 * Read all dependency TLVs of an image from the flash and verify
 * one after another to see if they are all satisfied.
 *
 * @param slot              Image slot number.
 *
 * @return                  0 on success; nonzero on failure.
 */
static int
boot_verify_slot_dependencies(struct boot_loader_state *state, uint32_t slot)
{
    const struct flash_area *fap;
    struct image_tlv_iter it;
    struct image_dependency dep;
    uint32_t off;
    uint16_t len;
    int area_id;
    int rc;

    area_id = flash_area_id_from_multi_image_slot(BOOT_CURR_IMG(state), slot);
    rc = flash_area_open(area_id, &fap);
    if (rc != 0) {
        rc = BOOT_EFLASH;
        goto done;
    }

    rc = bootutil_tlv_iter_begin(&it, boot_img_hdr(state, slot), fap,
            IMAGE_TLV_DEPENDENCY, true);
    if (rc != 0) {
        goto done;
    }

    while (true) {
        rc = bootutil_tlv_iter_next(&it, &off, &len, NULL);
        if (rc < 0) {
            return -1;
        } else if (rc > 0) {
            rc = 0;
            break;
        }

        if (len != sizeof(dep)) {
            rc = BOOT_EBADIMAGE;
            goto done;
        }

        rc = flash_area_read(fap, off, &dep, len);
        if (rc != 0) {
            rc = BOOT_EFLASH;
            goto done;
        }

        if (dep.image_id >= BOOT_IMAGE_NUMBER) {
            rc = BOOT_EBADARGS;
            goto done;
        }

        /* Verify dependency and modify the swap type if not satisfied. */
        rc = boot_verify_slot_dependency(state, &dep);
        if (rc != 0) {
            /* Dependency not satisfied. */
            goto done;
        }
    }

done:
    flash_area_close(fap);
    return rc;
}

/**
 * Iterate over all the images and verify whether the image dependencies in the
 * TLV area are all satisfied and update the related swap type if necessary.
 */
static int
boot_verify_dependencies(struct boot_loader_state *state)
{
    int rc;
    uint8_t slot;

    BOOT_CURR_IMG(state) = 0;
    while (BOOT_CURR_IMG(state) < BOOT_IMAGE_NUMBER) {
        if (BOOT_SWAP_TYPE(state) != BOOT_SWAP_TYPE_NONE &&
            BOOT_SWAP_TYPE(state) != BOOT_SWAP_TYPE_FAIL) {
            slot = BOOT_SECONDARY_SLOT;
        } else {
            slot = BOOT_PRIMARY_SLOT;
        }

        rc = boot_verify_slot_dependencies(state, slot);
        if (rc == 0) {
            /* All dependencies've been satisfied, continue with next image. */
            BOOT_CURR_IMG(state)++;
        } else if (rc == BOOT_EBADVERSION) {
            /* Cannot upgrade due to non-met dependencies, so disable all
             * image upgrades.
             */
            for (int idx = 0; idx < BOOT_IMAGE_NUMBER; idx++) {
                BOOT_CURR_IMG(state) = idx;
                BOOT_SWAP_TYPE(state) = BOOT_SWAP_TYPE_NONE;
            }
            break;
        } else {
            /* Other error happened, images are inconsistent */
            return rc;
        }
    }
    return rc;
}
#endif /* (BOOT_IMAGE_NUMBER > 1) */

/**
 * Performs a clean (not aborted) image update.
 *
 * @param bs                    The current boot status.
 *
 * @return                      0 on success; nonzero on failure.
 */
static int
boot_perform_update(struct boot_loader_state *state, struct boot_status *bs)
{
    int rc;
#ifndef MCUBOOT_OVERWRITE_ONLY
    uint8_t swap_type;
#endif

    /* At this point there are no aborted swaps. */
#if defined(MCUBOOT_OVERWRITE_ONLY)
    rc = boot_copy_image(state, bs);
#elif defined(MCUBOOT_BOOTSTRAP)
    /* Check if the image update was triggered by a bad image in the
     * primary slot (the validity of the image in the secondary slot had
     * already been checked).
     */
    if (boot_check_header_erased(state, BOOT_PRIMARY_SLOT) == 0 ||
        boot_validate_slot(state, BOOT_PRIMARY_SLOT, bs) != 0) {
        rc = boot_copy_image(state, bs);
    } else {
        rc = boot_swap_image(state, bs);
    }
#else
        rc = boot_swap_image(state, bs);
#endif
    assert(rc == 0);

#ifndef MCUBOOT_OVERWRITE_ONLY
    /* The following state needs image_ok be explicitly set after the
     * swap was finished to avoid a new revert.
     */
    swap_type = BOOT_SWAP_TYPE(state);
    if (swap_type == BOOT_SWAP_TYPE_REVERT ||
            swap_type == BOOT_SWAP_TYPE_PERM) {
        rc = boot_set_image_ok(BOOT_CURR_IMG(state));
        if (rc != 0) {
            BOOT_SWAP_TYPE(state) = swap_type = BOOT_SWAP_TYPE_PANIC;
        }
    }

    if (BOOT_IS_UPGRADE(swap_type)) {
        rc = boot_set_copy_done(BOOT_CURR_IMG(state));
        if (rc != 0) {
            BOOT_SWAP_TYPE(state) = BOOT_SWAP_TYPE_PANIC;
        }
    }
#endif /* !MCUBOOT_OVERWRITE_ONLY */

    return rc;
}

/**
 * Completes a previously aborted image swap.
 *
 * @param bs                    The current boot status.
 *
 * @return                      0 on success; nonzero on failure.
 */
#if !defined(MCUBOOT_OVERWRITE_ONLY)
static int
boot_complete_partial_swap(struct boot_loader_state *state,
        struct boot_status *bs)
{
    int rc;

    /* Determine the type of swap operation being resumed from the
     * `swap-type` trailer field.
     */
    rc = boot_swap_image(state, bs);
    assert(rc == 0);

    BOOT_SWAP_TYPE(state) = bs->swap_type;

    /* The following states need image_ok be explicitly set after the
     * swap was finished to avoid a new revert.
     */
    if (bs->swap_type == BOOT_SWAP_TYPE_REVERT ||
        bs->swap_type == BOOT_SWAP_TYPE_PERM) {
        rc = boot_set_image_ok(BOOT_CURR_IMG(state));
        if (rc != 0) {
            BOOT_SWAP_TYPE(state) = BOOT_SWAP_TYPE_PANIC;
        }
    }

    if (BOOT_IS_UPGRADE(bs->swap_type)) {
        rc = boot_set_copy_done(BOOT_CURR_IMG(state));
        if (rc != 0) {
            BOOT_SWAP_TYPE(state) = BOOT_SWAP_TYPE_PANIC;
        }
    }

    if (BOOT_SWAP_TYPE(state) == BOOT_SWAP_TYPE_PANIC) {
        BOOT_LOG_ERR("panic!");
        assert(0);

        /* Loop forever... */
        while (1) {}
    }

    return rc;
}
#endif /* !MCUBOOT_OVERWRITE_ONLY */

#if (BOOT_IMAGE_NUMBER > 1)
/**
 * Review the validity of previously determined swap types of other images.
 *
 * @param aborted_swap          The current image upgrade is a
 *                              partial/aborted swap.
 */
static void
boot_review_image_swap_types(struct boot_loader_state *state,
                             bool aborted_swap)
{
    /* In that case if we rebooted in the middle of an image upgrade process, we
     * must review the validity of swap types, that were previously determined
     * for other images. The image_ok flag had not been set before the reboot
     * for any of the updated images (only the copy_done flag) and thus falsely
     * the REVERT swap type has been determined for the previous images that had
     * been updated before the reboot.
     *
     * There are two separate scenarios that we have to deal with:
     *
     * 1. The reboot has happened during swapping an image:
     *      The current image upgrade has been determined as a
     *      partial/aborted swap.
     * 2. The reboot has happened between two separate image upgrades:
     *      In this scenario we must check the swap type of the current image.
     *      In those cases if it is NONE or REVERT we cannot certainly determine
     *      the fact of a reboot. In a consistent state images must move in the
     *      same direction or stay in place, e.g. in practice REVERT and TEST
     *      swap types cannot be present at the same time. If the swap type of
     *      the current image is either TEST, PERM or FAIL we must review the
     *      already determined swap types of other images and set each false
     *      REVERT swap types to NONE (these images had been successfully
     *      updated before the system rebooted between two separate image
     *      upgrades).
     */

    if (BOOT_CURR_IMG(state) == 0) {
        /* Nothing to do */
        return;
    }

    if (!aborted_swap) {
        if ((BOOT_SWAP_TYPE(state) == BOOT_SWAP_TYPE_NONE) ||
            (BOOT_SWAP_TYPE(state) == BOOT_SWAP_TYPE_REVERT)) {
            /* Nothing to do */
            return;
        }
    }

    for (uint8_t i = 0; i < BOOT_CURR_IMG(state); i++) {
        if (state->swap_type[i] == BOOT_SWAP_TYPE_REVERT) {
            state->swap_type[i] = BOOT_SWAP_TYPE_NONE;
        }
    }
}
#endif

/**
 * Prepare image to be updated if required.
 *
 * Prepare image to be updated if required with completing an image swap
 * operation if one was aborted and/or determining the type of the
 * swap operation. In case of any error set the swap type to NONE.
 *
 * @param state                 TODO
 * @param bs                    Pointer where the read and possibly updated
 *                              boot status can be written to.
 */
static void
boot_prepare_image_for_update(struct boot_loader_state *state,
                              struct boot_status *bs)
{
    int rc;

    /* Determine the sector layout of the image slots and scratch area. */
    rc = boot_read_sectors(state);
    if (rc != 0) {
        BOOT_LOG_WRN("Failed reading sectors; BOOT_MAX_IMG_SECTORS=%d"
                     " - too small?", BOOT_MAX_IMG_SECTORS);
        /* Unable to determine sector layout, continue with next image
         * if there is one.
         */
        BOOT_SWAP_TYPE(state) = BOOT_SWAP_TYPE_NONE;
        return;
    }

    /* Attempt to read an image header from each slot. */
    rc = boot_read_image_headers(state, false);
    if (rc != 0) {
        /* Continue with next image if there is one. */
        BOOT_LOG_WRN("Failed reading image headers; Image=%u",
                BOOT_CURR_IMG(state));
        BOOT_SWAP_TYPE(state) = BOOT_SWAP_TYPE_NONE;
        return;
    }

    /* If the current image's slots aren't compatible, no swap is possible.
     * Just boot into primary slot.
     */
    if (boot_slots_compatible(state)) {
        rc = boot_read_status(state, bs);
        if (rc != 0) {
            BOOT_LOG_WRN("Failed reading boot status; Image=%u",
                    BOOT_CURR_IMG(state));
            /* Continue with next image if there is one. */
            BOOT_SWAP_TYPE(state) = BOOT_SWAP_TYPE_NONE;
            return;
        }

        /* Determine if we rebooted in the middle of an image swap
         * operation. If a partial swap was detected, complete it.
         */
        if (bs->idx != BOOT_STATUS_IDX_0 || bs->state != BOOT_STATUS_STATE_0) {

#if (BOOT_IMAGE_NUMBER > 1)
            boot_review_image_swap_types(state, true);
#endif

#ifdef MCUBOOT_OVERWRITE_ONLY
            /* Should never arrive here, overwrite-only mode has
             * no swap state.
             */
            assert(0);
#else
            /* Determine the type of swap operation being resumed from the
             * `swap-type` trailer field.
             */
            rc = boot_complete_partial_swap(state, bs);
            assert(rc == 0);
#endif
            /* Attempt to read an image header from each slot. Ensure that
             * image headers in slots are aligned with headers in boot_data.
             */
            rc = boot_read_image_headers(state, false);
            assert(rc == 0);

            /* Swap has finished set to NONE */
            BOOT_SWAP_TYPE(state) = BOOT_SWAP_TYPE_NONE;
        } else {
            /* There was no partial swap, determine swap type. */
            if (bs->swap_type == BOOT_SWAP_TYPE_NONE) {
                BOOT_SWAP_TYPE(state) = boot_validated_swap_type(state, bs);
            } else if (boot_validate_slot(state, BOOT_SECONDARY_SLOT, bs) != 0) {
                BOOT_SWAP_TYPE(state) = BOOT_SWAP_TYPE_FAIL;
            } else {
                BOOT_SWAP_TYPE(state) = bs->swap_type;
            }

#if (BOOT_IMAGE_NUMBER > 1)
            boot_review_image_swap_types(state, false);
#endif

#ifdef MCUBOOT_BOOTSTRAP
            if (BOOT_SWAP_TYPE(state) == BOOT_SWAP_TYPE_NONE) {
                /* Header checks are done first because they are
                 * inexpensive. Since overwrite-only copies starting from
                 * offset 0, if interrupted, it might leave a valid header
                 * magic, so also run validation on the primary slot to be
                 * sure it's not OK.
                 */
                if (boot_check_header_erased(state, BOOT_PRIMARY_SLOT) == 0 ||
                        boot_validate_slot(state, BOOT_PRIMARY_SLOT, bs) != 0) {
                    if (boot_img_hdr(state,
                            BOOT_SECONDARY_SLOT)->ih_magic == IMAGE_MAGIC &&
                            boot_validate_slot(state, BOOT_SECONDARY_SLOT, bs) == 0) {
                        /* Set swap type to REVERT to overwrite the primary
                         * slot with the image contained in secondary slot
                         * and to trigger the explicit setting of the
                         * image_ok flag.
                         */
                        BOOT_SWAP_TYPE(state) = BOOT_SWAP_TYPE_REVERT;
                    }
                }
            }
#endif
        }
    } else {
        /* In that case if slots are not compatible. */
        BOOT_SWAP_TYPE(state) = BOOT_SWAP_TYPE_NONE;
    }
}

int
context_boot_go(struct boot_loader_state *state, struct boot_rsp *rsp)
{
    size_t slot;
    struct boot_status bs;
    int rc;
    int fa_id;
    int image_index;
    bool has_upgrade;

    /* The array of slot sectors are defined here (as opposed to file scope) so
     * that they don't get allocated for non-boot-loader apps.  This is
     * necessary because the gcc option "-fdata-sections" doesn't seem to have
     * any effect in older gcc versions (e.g., 4.8.4).
     */
    TARGET_STATIC boot_sector_t primary_slot_sectors[BOOT_IMAGE_NUMBER][BOOT_MAX_IMG_SECTORS];
    TARGET_STATIC boot_sector_t secondary_slot_sectors[BOOT_IMAGE_NUMBER][BOOT_MAX_IMG_SECTORS];
    TARGET_STATIC boot_sector_t scratch_sectors[BOOT_MAX_IMG_SECTORS];

    memset(state, 0, sizeof(struct boot_loader_state));
    has_upgrade = false;

#if (BOOT_IMAGE_NUMBER == 1)
    (void)has_upgrade;
#endif

    /* Iterate over all the images. By the end of the loop the swap type has
     * to be determined for each image and all aborted swaps have to be
     * completed.
     */
    IMAGES_ITER(BOOT_CURR_IMG(state)) {

#if defined(MCUBOOT_ENC_IMAGES) && (BOOT_IMAGE_NUMBER > 1)
        /* The keys used for encryption may no longer be valid (could belong to
         * another images). Therefore, mark them as invalid to force their reload
         * by boot_enc_load().
         */
        boot_enc_zeroize(BOOT_CURR_ENC(state));
#endif

        image_index = BOOT_CURR_IMG(state);

        BOOT_IMG(state, BOOT_PRIMARY_SLOT).sectors =
            primary_slot_sectors[image_index];
        BOOT_IMG(state, BOOT_SECONDARY_SLOT).sectors =
            secondary_slot_sectors[image_index];
        state->scratch.sectors = scratch_sectors;

        /* Open primary and secondary image areas for the duration
         * of this call.
         */
        for (slot = 0; slot < BOOT_NUM_SLOTS; slot++) {
            fa_id = flash_area_id_from_multi_image_slot(image_index, slot);
            rc = flash_area_open(fa_id, &BOOT_IMG_AREA(state, slot));
            assert(rc == 0);
        }
        rc = flash_area_open(FLASH_AREA_IMAGE_SCRATCH,
                             &BOOT_SCRATCH_AREA(state));
        assert(rc == 0);

        /* Determine swap type and complete swap if it has been aborted. */
        boot_prepare_image_for_update(state, &bs);

        if (BOOT_IS_UPGRADE(BOOT_SWAP_TYPE(state))) {
            has_upgrade = true;
        }
    }

#if (BOOT_IMAGE_NUMBER > 1)
    if (has_upgrade) {
        /* Iterate over all the images and verify whether the image dependencies
         * are all satisfied and update swap type if necessary.
         */
        rc = boot_verify_dependencies(state);
        if (rc == BOOT_EBADVERSION) {
            /*
             * It was impossible to upgrade because the expected dependency version
             * was not available. Here we already changed the swap_type so that
             * instead of asserting the bootloader, we continue and no upgrade is
             * performed.
             */
            rc = 0;
        }
    }
#endif

    /* Iterate over all the images. At this point there are no aborted swaps
     * and the swap types are determined for each image. By the end of the loop
     * all required update operations will have been finished.
     */
    IMAGES_ITER(BOOT_CURR_IMG(state)) {

#if (BOOT_IMAGE_NUMBER > 1)
#ifdef MCUBOOT_ENC_IMAGES
        /* The keys used for encryption may no longer be valid (could belong to
         * another images). Therefore, mark them as invalid to force their reload
         * by boot_enc_load().
         */
        boot_enc_zeroize(BOOT_CURR_ENC(state));
#endif /* MCUBOOT_ENC_IMAGES */

        /* Indicate that swap is not aborted */
        memset(&bs, 0, sizeof bs);
        bs.idx = BOOT_STATUS_IDX_0;
        bs.state = BOOT_STATUS_STATE_0;
#endif /* (BOOT_IMAGE_NUMBER > 1) */

        /* Set the previously determined swap type */
        bs.swap_type = BOOT_SWAP_TYPE(state);

        switch (BOOT_SWAP_TYPE(state)) {
        case BOOT_SWAP_TYPE_NONE:
            break;

        case BOOT_SWAP_TYPE_TEST:          /* fallthrough */
        case BOOT_SWAP_TYPE_PERM:          /* fallthrough */
        case BOOT_SWAP_TYPE_REVERT:
            rc = boot_perform_update(state, &bs);
            assert(rc == 0);
            break;

        case BOOT_SWAP_TYPE_FAIL:
            /* The image in secondary slot was invalid and is now erased. Ensure
             * we don't try to boot into it again on the next reboot. Do this by
             * pretending we just reverted back to primary slot.
             */
#ifndef MCUBOOT_OVERWRITE_ONLY
            /* image_ok needs to be explicitly set to avoid a new revert. */
            rc = boot_set_image_ok(BOOT_CURR_IMG(state));
            if (rc != 0) {
                BOOT_SWAP_TYPE(state) = BOOT_SWAP_TYPE_PANIC;
            }
#endif /* !MCUBOOT_OVERWRITE_ONLY */
            break;

        default:
            BOOT_SWAP_TYPE(state) = BOOT_SWAP_TYPE_PANIC;
        }

        if (BOOT_SWAP_TYPE(state) == BOOT_SWAP_TYPE_PANIC) {
            BOOT_LOG_ERR("panic!");
            assert(0);

            /* Loop forever... */
            while (1) {}
        }
    }

    /* Iterate over all the images. At this point all required update operations
     * have finished. By the end of the loop each image in the primary slot will
     * have been re-validated.
     */
    IMAGES_ITER(BOOT_CURR_IMG(state)) {
        if (BOOT_SWAP_TYPE(state) != BOOT_SWAP_TYPE_NONE) {
            /* Attempt to read an image header from each slot. Ensure that image
             * headers in slots are aligned with headers in boot_data.
             */
            rc = boot_read_image_headers(state, false);
            if (rc != 0) {
                goto out;
            }
            /* Since headers were reloaded, it can be assumed we just performed
             * a swap or overwrite. Now the header info that should be used to
             * provide the data for the bootstrap, which previously was at
             * secondary slot, was updated to primary slot.
             */
        }

#ifdef MCUBOOT_VALIDATE_PRIMARY_SLOT
        rc = boot_validate_slot(state, BOOT_PRIMARY_SLOT, NULL);
        if (rc != 0) {
            rc = BOOT_EBADIMAGE;
            goto out;
        }
#else
        /* Even if we're not re-validating the primary slot, we could be booting
         * onto an empty flash chip. At least do a basic sanity check that
         * the magic number on the image is OK.
         */
        if (BOOT_IMG(state, BOOT_PRIMARY_SLOT).hdr.ih_magic != IMAGE_MAGIC) {
            BOOT_LOG_ERR("bad image magic 0x%lx; Image=%u", (unsigned long)
                         &boot_img_hdr(state,BOOT_PRIMARY_SLOT)->ih_magic,
                         BOOT_CURR_IMG(state));
            rc = BOOT_EBADIMAGE;
            goto out;
        }
#endif
    }

#if (BOOT_IMAGE_NUMBER > 1)
    /* Always boot from the primary slot of Image 0. */
    BOOT_CURR_IMG(state) = 0;
#endif

    rsp->br_flash_dev_id = BOOT_IMG_AREA(state, BOOT_PRIMARY_SLOT)->fa_device_id;
    rsp->br_image_off = boot_img_slot_off(state, BOOT_PRIMARY_SLOT);
    rsp->br_hdr = boot_img_hdr(state, BOOT_PRIMARY_SLOT);

out:
    IMAGES_ITER(BOOT_CURR_IMG(state)) {
        flash_area_close(BOOT_SCRATCH_AREA(state));
        for (slot = 0; slot < BOOT_NUM_SLOTS; slot++) {
            flash_area_close(BOOT_IMG_AREA(state, BOOT_NUM_SLOTS - 1 - slot));
        }
    }
    return rc;
}

/**
 * Prepares the booting process.  This function moves images around in flash as
 * appropriate, and tells you what address to boot from.
 *
 * @param rsp                   On success, indicates how booting should occur.
 *
 * @return                      0 on success; nonzero on failure.
 */
int
boot_go(struct boot_rsp *rsp)
{
    return context_boot_go(&boot_data, rsp);
}

int
split_go(int loader_slot, int split_slot, void **entry)
{
    boot_sector_t *sectors;
    uintptr_t entry_val;
    int loader_flash_id;
    int split_flash_id;
    int rc;

    sectors = malloc(BOOT_MAX_IMG_SECTORS * 2 * sizeof *sectors);
    if (sectors == NULL) {
        return SPLIT_GO_ERR;
    }
    BOOT_IMG(&boot_data, loader_slot).sectors = sectors + 0;
    BOOT_IMG(&boot_data, split_slot).sectors = sectors + BOOT_MAX_IMG_SECTORS;

    loader_flash_id = flash_area_id_from_image_slot(loader_slot);
    rc = flash_area_open(loader_flash_id,
                         &BOOT_IMG_AREA(&boot_data, loader_slot));
    assert(rc == 0);
    split_flash_id = flash_area_id_from_image_slot(split_slot);
    rc = flash_area_open(split_flash_id,
                         &BOOT_IMG_AREA(&boot_data, split_slot));
    assert(rc == 0);

    /* Determine the sector layout of the image slots and scratch area. */
    rc = boot_read_sectors(&boot_data);
    if (rc != 0) {
        rc = SPLIT_GO_ERR;
        goto done;
    }

    rc = boot_read_image_headers(&boot_data, true);
    if (rc != 0) {
        goto done;
    }

    /* Don't check the bootable image flag because we could really call a
     * bootable or non-bootable image.  Just validate that the image check
     * passes which is distinct from the normal check.
     */
    rc = split_image_check(boot_img_hdr(&boot_data, split_slot),
                           BOOT_IMG_AREA(&boot_data, split_slot),
                           boot_img_hdr(&boot_data, loader_slot),
                           BOOT_IMG_AREA(&boot_data, loader_slot));
    if (rc != 0) {
        rc = SPLIT_GO_NON_MATCHING;
        goto done;
    }

    entry_val = boot_img_slot_off(&boot_data, split_slot) +
                boot_img_hdr(&boot_data, split_slot)->ih_hdr_size;
    *entry = (void *) entry_val;
    rc = SPLIT_GO_OK;

done:
    flash_area_close(BOOT_IMG_AREA(&boot_data, split_slot));
    flash_area_close(BOOT_IMG_AREA(&boot_data, loader_slot));
    free(sectors);
    return rc;
}
