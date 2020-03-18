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

#include <stddef.h>
#include <inttypes.h>
#include <string.h>

#include <flash_map_backend/flash_map_backend.h>

#include "bootutil/image.h"
#include "bootutil/sha256.h"
#include "bootutil/sign_key.h"

#include "mcuboot_config/mcuboot_config.h"

#ifdef MCUBOOT_ENC_IMAGES
#include "bootutil/enc_key.h"
#endif
#if defined(MCUBOOT_SIGN_RSA)
#include "mbedtls/rsa.h"
#endif
#if defined(MCUBOOT_SIGN_EC) || defined(MCUBOOT_SIGN_EC256)
#include "mbedtls/ecdsa.h"
#endif
#include "mbedtls/asn1.h"

#include "bootutil_priv.h"

/*
 * Compute SHA256 over the image.
 */
static int
bootutil_img_hash(struct enc_key_data *enc_state, int image_index,
                  struct image_header *hdr, const struct flash_area *fap,
                  uint8_t *tmp_buf, uint32_t tmp_buf_sz, uint8_t *hash_result,
                  uint8_t *seed, int seed_len)
{
    bootutil_sha256_context sha256_ctx;
    uint32_t blk_sz;
    uint32_t size;
    uint16_t hdr_size;
    uint32_t off;
    int rc;
    uint32_t blk_off;
    uint32_t tlv_off;

#if (BOOT_IMAGE_NUMBER == 1) || !defined(MCUBOOT_ENC_IMAGES)
    (void)enc_state;
    (void)image_index;
    (void)hdr_size;
    (void)blk_off;
    (void)tlv_off;
#endif

#ifdef MCUBOOT_ENC_IMAGES
    /* Encrypted images only exist in the secondary slot */
    if (MUST_DECRYPT(fap, image_index, hdr) &&
            !boot_enc_valid(enc_state, image_index, fap)) {
        return -1;
    }
#endif

    bootutil_sha256_init(&sha256_ctx);

    /* in some cases (split image) the hash is seeded with data from
     * the loader image */
    if (seed && (seed_len > 0)) {
        bootutil_sha256_update(&sha256_ctx, seed, seed_len);
    }

    /* Hash is computed over image header and image itself. */
    size = hdr_size = hdr->ih_hdr_size;
    size += hdr->ih_img_size;
    tlv_off = size;

    /* If protected TLVs are present they are also hashed. */
    size += hdr->ih_protect_tlv_size;

    for (off = 0; off < size; off += blk_sz) {
        blk_sz = size - off;
        if (blk_sz > tmp_buf_sz) {
            blk_sz = tmp_buf_sz;
        }
#ifdef MCUBOOT_ENC_IMAGES
        /* The only data that is encrypted in an image is the payload;
         * both header and TLVs (when protected) are not.
         */
        if ((off < hdr_size) && ((off + blk_sz) > hdr_size)) {
            /* read only the header */
            blk_sz = hdr_size - off;
        }
        if ((off < tlv_off) && ((off + blk_sz) > tlv_off)) {
            /* read only up to the end of the image payload */
            blk_sz = tlv_off - off;
        }
#endif
        rc = flash_area_read(fap, off, tmp_buf, blk_sz);
        if (rc) {
            return rc;
        }
#ifdef MCUBOOT_ENC_IMAGES
        if (MUST_DECRYPT(fap, image_index, hdr)) {
            /* Only payload is encrypted (area between header and TLVs) */
            if (off >= hdr_size && off < tlv_off) {
                blk_off = (off - hdr_size) & 0xf;
                boot_encrypt(enc_state, image_index, fap, off - hdr_size,
                        blk_sz, blk_off, tmp_buf);
            }
        }
#endif
        bootutil_sha256_update(&sha256_ctx, tmp_buf, blk_sz);
    }
    bootutil_sha256_finish(&sha256_ctx, hash_result);

    return 0;
}

/*
 * Currently, we only support being able to verify one type of
 * signature, because there is a single verification function that we
 * call.  List the type of TLV we are expecting.  If we aren't
 * configured for any signature, don't define this macro.
 */
#if (defined(MCUBOOT_SIGN_RSA)      + \
     defined(MCUBOOT_SIGN_EC)       + \
     defined(MCUBOOT_SIGN_EC256)    + \
     defined(MCUBOOT_SIGN_ED25519)) > 1
#error "Only a single signature type is supported!"
#endif

#if defined(MCUBOOT_SIGN_RSA)
#    if MCUBOOT_SIGN_RSA_LEN == 2048
#        define EXPECTED_SIG_TLV IMAGE_TLV_RSA2048_PSS
#    elif MCUBOOT_SIGN_RSA_LEN == 3072
#        define EXPECTED_SIG_TLV IMAGE_TLV_RSA3072_PSS
#    else
#        error "Unsupported RSA signature length"
#    endif
#    define SIG_BUF_SIZE (MCUBOOT_SIGN_RSA_LEN / 8)
#    define EXPECTED_SIG_LEN(x) ((x) == SIG_BUF_SIZE) /* 2048 bits */
#elif defined(MCUBOOT_SIGN_EC)
#    define EXPECTED_SIG_TLV IMAGE_TLV_ECDSA224
#    define SIG_BUF_SIZE 128
#    define EXPECTED_SIG_LEN(x) ((x) >= 64) /* oids + 2 * 28 bytes */
#elif defined(MCUBOOT_SIGN_EC256)
#    define EXPECTED_SIG_TLV IMAGE_TLV_ECDSA256
#    define SIG_BUF_SIZE 128
#    define EXPECTED_SIG_LEN(x) ((x) >= 72) /* oids + 2 * 32 bytes */
#elif defined(MCUBOOT_SIGN_ED25519)
#    define EXPECTED_SIG_TLV IMAGE_TLV_ED25519
#    define SIG_BUF_SIZE 64
#    define EXPECTED_SIG_LEN(x) ((x) == SIG_BUF_SIZE)
#else
#    define SIG_BUF_SIZE 32 /* no signing, sha256 digest only */
#endif

#ifdef EXPECTED_SIG_TLV
static int
bootutil_find_key(uint8_t *keyhash, uint8_t keyhash_len)
{
    bootutil_sha256_context sha256_ctx;
    int i;
    const struct bootutil_key *key;
    uint8_t hash[32];

    if (keyhash_len > 32) {
        return -1;
    }

    for (i = 0; i < bootutil_key_cnt; i++) {
        key = &bootutil_keys[i];
        bootutil_sha256_init(&sha256_ctx);
        bootutil_sha256_update(&sha256_ctx, key->key, *key->len);
        bootutil_sha256_finish(&sha256_ctx, hash);
        if (!memcmp(hash, keyhash, keyhash_len)) {
            return i;
        }
    }
    return -1;
}
#endif

/*
 * Verify the integrity of the image.
 * Return non-zero if image could not be validated/does not validate.
 */
int
bootutil_img_validate(struct enc_key_data *enc_state, int image_index,
                      struct image_header *hdr, const struct flash_area *fap,
                      uint8_t *tmp_buf, uint32_t tmp_buf_sz, uint8_t *seed,
                      int seed_len, uint8_t *out_hash)
{
    uint32_t off;
    uint16_t len;
    uint8_t type;
    int sha256_valid = 0;
#ifdef EXPECTED_SIG_TLV
    int valid_signature = 0;
    int key_id = -1;
#endif
    struct image_tlv_iter it;
    uint8_t buf[SIG_BUF_SIZE];
    uint8_t hash[32];
    int rc;

    rc = bootutil_img_hash(enc_state, image_index, hdr, fap, tmp_buf,
            tmp_buf_sz, hash, seed, seed_len);
    if (rc) {
        return rc;
    }

    if (out_hash) {
        memcpy(out_hash, hash, 32);
    }

    rc = bootutil_tlv_iter_begin(&it, hdr, fap, IMAGE_TLV_ANY, false);
    if (rc) {
        return rc;
    }

    /*
     * Traverse through all of the TLVs, performing any checks we know
     * and are able to do.
     */
    while (true) {
        rc = bootutil_tlv_iter_next(&it, &off, &len, &type);
        if (rc < 0) {
            return -1;
        } else if (rc > 0) {
            break;
        }

        if (type == IMAGE_TLV_SHA256) {
            /*
             * Verify the SHA256 image hash.  This must always be
             * present.
             */
            if (len != sizeof(hash)) {
                return -1;
            }
            rc = flash_area_read(fap, off, buf, sizeof hash);
            if (rc) {
                return rc;
            }
            if (memcmp(hash, buf, sizeof(hash))) {
                return -1;
            }

            sha256_valid = 1;
#ifdef EXPECTED_SIG_TLV
        } else if (type == IMAGE_TLV_KEYHASH) {
            /*
             * Determine which key we should be checking.
             */
            if (len > 32) {
                return -1;
            }
            rc = flash_area_read(fap, off, buf, len);
            if (rc) {
                return rc;
            }
            key_id = bootutil_find_key(buf, len);
            /*
             * The key may not be found, which is acceptable.  There
             * can be multiple signatures, each preceded by a key.
             */
        } else if (type == EXPECTED_SIG_TLV) {
            /* Ignore this signature if it is out of bounds. */
            if (key_id < 0 || key_id >= bootutil_key_cnt) {
                key_id = -1;
                continue;
            }
            if (!EXPECTED_SIG_LEN(len) || len > sizeof(buf)) {
                return -1;
            }
            rc = flash_area_read(fap, off, buf, len);
            if (rc) {
                return -1;
            }
            rc = bootutil_verify_sig(hash, sizeof(hash), buf, len, key_id);
            if (rc == 0) {
                valid_signature = 1;
            }
            key_id = -1;
#endif
        }
    }

    if (!sha256_valid) {
        return -1;
    }

#ifdef EXPECTED_SIG_TLV
    if (!valid_signature) {
        return -1;
    }
#endif

    return 0;
}
