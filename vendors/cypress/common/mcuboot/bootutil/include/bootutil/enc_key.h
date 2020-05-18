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

#ifndef BOOTUTIL_ENC_KEY_H
#define BOOTUTIL_ENC_KEY_H

#include <stdbool.h>
#include <flash_map_backend/flash_map_backend.h>
#include "mcuboot_config/mcuboot_config.h"
#include "bootutil/image.h"

#if defined(MCUBOOT_USE_MBED_TLS)
#include "mbedtls/aes.h"
#else
#include "tinycrypt/aes.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif

#define BOOT_ENC_KEY_SIZE       16
#define BOOT_ENC_KEY_SIZE_BITS  (BOOT_ENC_KEY_SIZE * 8)

struct enc_key_data {
    uint8_t valid;
#if defined(MCUBOOT_USE_MBED_TLS)
    mbedtls_aes_context aes;
#else
    struct tc_aes_key_sched_struct aes;
#endif
};

extern const struct bootutil_key bootutil_enc_key;

int boot_enc_set_key(struct enc_key_data *enc_state, uint8_t slot,
        uint8_t *enckey);
int boot_enc_load(struct enc_key_data *enc_state, int image_index,
        const struct image_header *hdr, const struct flash_area *fap,
        uint8_t *enckey);
bool boot_enc_valid(struct enc_key_data *enc_state, int image_index,
        const struct flash_area *fap);
void boot_encrypt(struct enc_key_data *enc_state, int image_index,
        const struct flash_area *fap, uint32_t off, uint32_t sz,
        uint32_t blk_off, uint8_t *buf);
void boot_enc_zeroize(struct enc_key_data *enc_state);

#ifdef __cplusplus
}
#endif

#endif /* BOOTUTIL_ENC_KEY_H */
